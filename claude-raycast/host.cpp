//===- host.cpp ---------------------------------------------------*- C++ -*-===//
//
// GPU raycaster sharing the NPU's nibble-packed, palette-indexed textures: the
// wall/sprite index data and 16-color palettes are the single source of truth
// (tex_data.h), uploaded to the compute shader as an SSBO + palette uniforms.
//
// Controls: arrow keys drive the GPU (main) camera — Up/Down move, Left/Right
// turn. WASD drive the NPU camera shown on the wall-4 monitor — W/S move, A/D
// turn. Tab cycles the render mode (combined / GPU only / NPU only). Esc/Q
// quit.
//
//===----------------------------------------------------------------------===//

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>

#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/experimental/xrt_elf.h"
#include "xrt/experimental/xrt_ext.h"
#include "xrt/experimental/xrt_module.h"

#include <glad/gl.h>             // GL function loader (must precede other GL headers)
#include <SDL3/SDL.h>

#include "tex_data.h"             // 4 x 64x64 RGBA wall textures
#include "shaders.h"              // kVertSrc / kFragSrc / kCompSrc GLSL sources

#define WIDTH 1024
#define HEIGHT 1024
#define N (WIDTH * HEIGHT)
#define Q12 4096


// World map (for sprite collision + GPU SSBO). Single source: map_data.inc,
// also included by kernel.cpp so the host and NPU scenes can't drift.
static const int g_map[256] = {
#include "map_data.inc"
};
#define MAX_SPRITES 8

// ---------------------------------------------------------------------------
// GLSL shaders (kVertSrc / kFragSrc / kCompSrc) live in shaders.h. The world
// map is uploaded to the compute shader as an SSBO from g_map above; the NPU
// keeps its own identical copy in kernel.cpp so the two scenes match.
// ---------------------------------------------------------------------------


static GLuint compileShader(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[4096];
        glGetShaderInfoLog(s, sizeof(log), nullptr, log);
        fprintf(stderr, "shader compile error:\n%s\n", log);
    }
    return s;
}

static GLuint linkProgram(std::vector<GLuint> shaders) {
    GLuint p = glCreateProgram();
    for (GLuint s : shaders) glAttachShader(p, s);
    glLinkProgram(p);
    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[4096];
        glGetProgramInfoLog(p, sizeof(log), nullptr, log);
        fprintf(stderr, "program link error:\n%s\n", log);
    }
    for (GLuint s : shaders) glDeleteShader(s);
    return p;
}

static inline int32_t pack_q12(float v) { return (int32_t)lroundf(v * Q12); }

using clk = std::chrono::high_resolution_clock;
static double now_us() {
    return std::chrono::duration<double, std::micro>(clk::now().time_since_epoch()).count();
}

struct SpriteW { float x, y, vx, vy; };

// True if (x,y) lands in a wall cell (or outside the map).
static bool solid(float x, float y) {
    int mx = (int)floorf(x), my = (int)floorf(y);
    if (mx < 0 || mx >= 16 || my < 0 || my >= 16) return true;
    return g_map[my * 16 + mx] != 0;
}

static void update_sprites(std::vector<SpriteW> &sprites, float dt) {
    for (auto &s : sprites) {
        float nx = s.x + s.vx * dt, ny = s.y + s.vy * dt;
        if (solid(nx, s.y)) { s.vx = -s.vx; nx = s.x; }
        if (solid(s.x, ny)) { s.vy = -s.vy; ny = s.y; }
        s.x = nx; s.y = ny;
    }
}

// Project the sprites for a given camera. Fills whichever outputs are
// non-null: oA is the GPU screen-rect uniform array, oDepth the matching
// per-sprite depths, oPack the NPU packed block.
static void project(const std::vector<SpriteW> &sprites,
                    float px, float py, float dx, float dy, float plx, float ply,
                    float (*oA)[4], float *oDepth, int32_t (*oPack)[5], int &count) {
    struct P { float depth, cx, hw, top, bot; };
    std::vector<P> ps;
    float det = plx * dy - dx * ply;
    if (fabsf(det) < 1e-6f) det = 1e-6f;
    float invDet = 1.0f / det;
    for (auto &s : sprites) {
        float rx = s.x - px, ry = s.y - py;
        float tx = invDet * (dy * rx - dx * ry);
        float ty = invDet * (-ply * rx + plx * ry);          // camera depth
        if (ty < 0.2f) continue;                            // behind / too close
        float cx = (WIDTH * 0.5f) * (1.0f + tx / ty);
        float sh = fabsf((float)HEIGHT / ty);
        ps.push_back({ty, cx, sh * 0.5f, HEIGHT * 0.5f - sh * 0.5f,
                      HEIGHT * 0.5f + sh * 0.5f});
    }
    std::sort(ps.begin(), ps.end(),
              [](const P &a, const P &b) { return a.depth > b.depth; }); // far first
    count = std::min((int)ps.size(), (int)MAX_SPRITES);
    for (int i = 0; i < count; i++) {
        const P &p = ps[i];
        if (oA) { oA[i][0] = p.cx; oA[i][1] = p.hw; oA[i][2] = p.top; oA[i][3] = p.bot; }
        if (oDepth) oDepth[i] = p.depth;
        if (oPack) {
            oPack[i][0] = (int32_t)lroundf(p.cx);
            oPack[i][1] = (int32_t)lroundf(p.hw);
            oPack[i][2] = (int32_t)lroundf(p.top);
            oPack[i][3] = (int32_t)lroundf(p.bot);
            oPack[i][4] = (int32_t)lroundf(p.depth * Q12);
        }
    }
}

// Per-column ray + DDA setup for the NPU (column-major device buffer).
// Each tile also carries the (column-independent) sprite block at in[12..].
static void fill_params(int32_t *bufInA,
                        float npu_pos_x, float npu_pos_y,
                        float npu_dir_x, float npu_dir_y,
                        float npu_plane_x, float npu_plane_y,
                        int npuSprCount, int32_t sprPack[][5]) {
    int32_t pos_x_q12 = pack_q12(npu_pos_x);
    int32_t pos_y_q12 = pack_q12(npu_pos_y);
    int32_t map_x_0 = (int32_t)floorf(npu_pos_x);
    int32_t map_y_0 = (int32_t)floorf(npu_pos_y);
    for (int col = 0; col < WIDTH; col++) {
        float camera_x = 2.0f * col / (float)WIDTH - 1.0f;
        int32_t ray_dir_x = pack_q12(npu_dir_x + npu_plane_x * camera_x);
        int32_t ray_dir_y = pack_q12(npu_dir_y + npu_plane_y * camera_x);
        if (ray_dir_x == 0) ray_dir_x = 1;
        if (ray_dir_y == 0) ray_dir_y = 1;
        int32_t delta_x = (int32_t)((int64_t)Q12 * Q12 / std::abs(ray_dir_x));
        int32_t delta_y = (int32_t)((int64_t)Q12 * Q12 / std::abs(ray_dir_y));
        int32_t step_x = (ray_dir_x > 0) ? 1 : -1;
        int32_t step_y = (ray_dir_y > 0) ? 1 : -1;
        int32_t side_x_0, side_y_0;
        if (step_x < 0)
            side_x_0 = (int32_t)(((int64_t)pos_x_q12 - (int64_t)map_x_0 * Q12) * delta_x / Q12);
        else
            side_x_0 = (int32_t)((((int64_t)map_x_0 + 1) * Q12 - (int64_t)pos_x_q12) * delta_x / Q12);
        if (step_y < 0)
            side_y_0 = (int32_t)(((int64_t)pos_y_q12 - (int64_t)map_y_0 * Q12) * delta_y / Q12);
        else
            side_y_0 = (int32_t)((((int64_t)map_y_0 + 1) * Q12 - (int64_t)pos_y_q12) * delta_y / Q12);
        int32_t params[13 + MAX_SPRITES * 5] = {
            pos_x_q12, pos_y_q12, ray_dir_x, ray_dir_y,
            delta_x,   delta_y,   side_x_0,  side_y_0,
            step_x,    step_y,    map_x_0,   map_y_0,
        };
        params[12] = npuSprCount;
        for (int k = 0; k < npuSprCount; k++)
            for (int j = 0; j < 5; j++)
                params[13 + k * 5 + j] = sprPack[k][j];
        int plen = 13 + npuSprCount * 5;
        for (int band = 0; band < 4; band++)
            memcpy(&bufInA[col * HEIGHT + band * 256], params, plen * sizeof(int32_t));
    }
}

// Rotate a camera's direction + plane in place.
static void turn(float &dx, float &dy, float &plx, float &ply, float ang) {
    float cs = cosf(ang), sn = sinf(ang);
    float ndx = dx * cs - dy * sn, ndy = dx * sn + dy * cs;
    dx = ndx; dy = ndy;
    float npx = plx * cs - ply * sn, npy = plx * sn + ply * cs;
    plx = npx; ply = npy;
}

// Move a camera position, clamped to the playable area.
static void moveCam(float &px, float &py, float ddx, float ddy) {
    float nx = px + ddx, ny = py + ddy;
    if (nx < 1.05f) nx = 1.05f; if (nx > 14.95f) nx = 14.95f;
    if (ny < 1.05f) ny = 1.05f; if (ny > 14.95f) ny = 14.95f;
    px = nx; py = ny;
}

int main(int argc, const char *argv[]) {
    // ----- NPU / XRT setup -------------------------------------------------
    auto device = xrt::device(0);
    auto xclbin = xrt::xclbin(std::string("device.xclbin"));
    device.register_xclbin(xclbin);
    xrt::elf elf(std::string("insts.elf"));
    xrt::module mod{elf};
    xrt::hw_context context(device, xclbin.get_uuid());
    auto kernel = xrt::ext::kernel(context, mod, "MLIR_AIE");
    xrt::bo bo_inA = xrt::ext::bo{device, N * sizeof(int32_t)};
    xrt::bo bo_out = xrt::ext::bo{device, N * sizeof(int32_t)};
    int32_t *bufInA = bo_inA.map<int32_t *>();
    int32_t *bufOut = bo_out.map<int32_t *>();
    unsigned int opcode = 3;

    // ----- Camera state (two independent views) ---------------------------
    // GPU camera drives the main view (arrow keys); NPU camera drives the
    // scene shown on the wall-4 "monitor" (WASD).
    float pos_x = 3.5f, pos_y = 3.5f;            // GPU camera
    float dir_x = 1.0f, dir_y = 0.0f;
    float plane_x = 0.0f, plane_y = 0.66f;
    float npu_pos_x = 3.5f, npu_pos_y = 3.5f;    // NPU camera
    float npu_dir_x = 1.0f, npu_dir_y = 0.0f;
    float npu_plane_x = 0.0f, npu_plane_y = 0.66f;
    const float move_speed = 3.0f;
    const float rot_speed  = 2.2f;


    std::vector<SpriteW> sprites = {
        { 2.5f,  7.5f,  0.9f,  0.7f},
        { 8.5f,  2.5f, -0.6f,  1.0f},
        {12.5f, 12.5f,  0.8f, -0.9f},
        { 6.5f,  9.5f,  1.1f,  0.4f},
        {10.5f,  5.5f, -1.0f, -0.6f},
        { 4.5f, 12.5f,  0.5f, -1.1f},
    };

    // Projected (screen-space) sprite data; each camera projects its own.
    int     gpuSprCount = 0, npuSprCount = 0;
    float   sprA[MAX_SPRITES][4];     // screenCx, halfW, top, bot (GPU)
    float   sprDepth[MAX_SPRITES];    // per-sprite camera depth   (GPU uniform)
    int32_t sprPack[MAX_SPRITES][5];  // cx, halfW, top, bot, depth_q12 (NPU)

    // ----- SDL window + OpenGL 4.3 core context ---------------------------
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window *window = SDL_CreateWindow("Raycast (NPU)", WIDTH, HEIGHT,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext glctx = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glctx);
    SDL_GL_SetSwapInterval(0); // uncapped, so we can actually measure throughput
    if (!gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress)) {
        fprintf(stderr, "gladLoadGL failed\n");
        return 1;
    }

    fprintf(stderr, "GL: %s | %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));

    GLuint blitProg = linkProgram({compileShader(GL_VERTEX_SHADER, kVertSrc),
                                   compileShader(GL_FRAGMENT_SHADER, kFragSrc)});
    GLuint compProg = linkProgram({compileShader(GL_COMPUTE_SHADER, kCompSrc)});

    GLint locTranspose = glGetUniformLocation(blitProg, "uTranspose");
    GLint locPos   = glGetUniformLocation(compProg, "uPos");
    GLint locDir   = glGetUniformLocation(compProg, "uDir");
    GLint locPlane = glGetUniformLocation(compProg, "uPlane");
    GLint locSprCount = glGetUniformLocation(compProg, "uSpriteCount");
    GLint locSprA  = glGetUniformLocation(compProg, "uSprA");
    GLint locSprDepth = glGetUniformLocation(compProg, "uSprDepth");
    GLint locWallPal  = glGetUniformLocation(compProg, "uWallPal");
    GLint locDemonPal = glGetUniformLocation(compProg, "uDemonPal");
    GLint locNpuTex = glGetUniformLocation(compProg, "uNpuTex");
    GLint locMode = glGetUniformLocation(compProg, "uMode");

    // Shared screen texture: compute writes it (image), blit samples it.
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, WIDTH, HEIGHT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Palette-indexed textures (shared with the NPU kernel): pack the 4 walls
    // then the demon sprite into one SSBO of 4-bit indices, and hand the host's
    // 16-entry palettes to the shader as uniforms. The layout matches shaders.h
    // (wall t at byte t*2048; demon at byte 4*2048) and kernel.cpp's unpack.
    {
        const uint8_t *wall_packed[4] = { brick_tex_idx_packed, stone_tex_idx_packed,
                                          marble_tex_idx_packed, tanstone_tex_idx_packed };
        uint8_t packed[5 * 2048];
        for (int t = 0; t < 4; t++) memcpy(packed + t * 2048, wall_packed[t], 2048);
        memcpy(packed + 4 * 2048, demon_packed, 2048);
        GLuint packBuf;
        glGenBuffers(1, &packBuf);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, packBuf);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(packed), packed, GL_STATIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, packBuf);
    }
    glUseProgram(compProg);
    glUniform1uiv(locWallPal, 16, wall_pal);
    glUniform1uiv(locDemonPal, 16, demon_pal);

    // NPU output texture: GPU compute shader samples this for wall 4.
    GLuint npuTex;
    glGenTextures(1, &npuTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, npuTex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, WIDTH, HEIGHT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // World map SSBO: uploaded once, sampled by the compute raycaster (binding 1).
    GLuint mapBuf;
    glGenBuffers(1, &mapBuf);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mapBuf);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(g_map), g_map, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mapBuf);

    GLuint vao;
    glGenVertexArrays(1, &vao);

    bool running = true;
    int  mode    = 0;      // 0 = combined, 1 = GPU only, 2 = NPU only (Tab cycles)
    const char *modeName[3] = { "Combined", "GPU only", "NPU only" };

    uint64_t last_time = SDL_GetTicks();
    uint32_t frames = 0;
    double acc_work = 0, acc_present = 0, acc_total = 0;
    uint32_t prof_frames = 0;
    auto t0 = std::chrono::high_resolution_clock::now();

    while (running) {
        frames++; prof_frames++;
        double t_frame0 = now_us();

        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = false;
            if (ev.type == SDL_EVENT_KEY_DOWN) {
                SDL_Keycode k = ev.key.key;
                if (k == SDLK_ESCAPE || k == SDLK_Q) running = false;
                else if (k == SDLK_TAB) mode = (mode + 1) % 3;
            }
        }
        if (!running) break;

        auto t = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(t - t0).count();
        t0 = t;
        if (dt > 0.1f) dt = 0.1f;

        const bool *keys = SDL_GetKeyboardState(NULL);
        float move = move_speed * dt, rot = rot_speed * dt;

        // GPU camera (main view): arrow keys. Up/Down move, Left/Right turn.
        if (keys[SDL_SCANCODE_LEFT])  turn(dir_x, dir_y, plane_x, plane_y, -rot);
        if (keys[SDL_SCANCODE_RIGHT]) turn(dir_x, dir_y, plane_x, plane_y,  rot);
        if (keys[SDL_SCANCODE_UP])    moveCam(pos_x, pos_y,  dir_x * move,  dir_y * move);
        if (keys[SDL_SCANCODE_DOWN])  moveCam(pos_x, pos_y, -dir_x * move, -dir_y * move);

        // NPU camera (wall-4 monitor): WASD. W/S move, A/D turn.
        if (keys[SDL_SCANCODE_A]) turn(npu_dir_x, npu_dir_y, npu_plane_x, npu_plane_y, -rot);
        if (keys[SDL_SCANCODE_D]) turn(npu_dir_x, npu_dir_y, npu_plane_x, npu_plane_y,  rot);
        if (keys[SDL_SCANCODE_W]) moveCam(npu_pos_x, npu_pos_y,  npu_dir_x * move,  npu_dir_y * move);
        if (keys[SDL_SCANCODE_S]) moveCam(npu_pos_x, npu_pos_y, -npu_dir_x * move, -npu_dir_y * move);

        // Advance the sprites, then project them for each camera independently.
        update_sprites(sprites, dt);
        project(sprites, pos_x, pos_y, dir_x, dir_y, plane_x, plane_y,
                sprA, sprDepth, nullptr, gpuSprCount);
        project(sprites, npu_pos_x, npu_pos_y, npu_dir_x, npu_dir_y, npu_plane_x, npu_plane_y,
                nullptr, nullptr, sprPack, npuSprCount);

        // ----- Produce the frame into the shared texture -------------------
        double tw0 = now_us();

        // The NPU is only needed for the combined and NPU-only modes; skip it
        // entirely in GPU-only mode. Its output is uploaded to npuTex.
        if (mode != 1) {
            fill_params(bufInA, npu_pos_x, npu_pos_y, npu_dir_x, npu_dir_y,
                        npu_plane_x, npu_plane_y, npuSprCount, sprPack);
            bo_inA.sync(XCL_BO_SYNC_BO_TO_DEVICE);
            kernel(opcode, 0, 0, bo_inA, bo_out).wait2();
            bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
            glBindTexture(GL_TEXTURE_2D, npuTex);
            // bufOut is column-major 0xRRGGBBAA; UNSIGNED_INT_8_8_8_8 reads R from MSB.
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT,
                            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, bufOut);
        }

        // NPU-only mode blits the NPU texture straight to the screen, so the
        // compute shader is skipped entirely (no full-frame GPU copy).
        if (mode != 2) {
            glUseProgram(compProg);
            glUniform1i(locMode, mode);
            glUniform2f(locPos, pos_x, pos_y);
            glUniform2f(locDir, dir_x, dir_y);
            glUniform2f(locPlane, plane_x, plane_y);
            glUniform1i(locSprCount, gpuSprCount);
            glUniform4fv(locSprA, gpuSprCount, &sprA[0][0]);
            glUniform1fv(locSprDepth, gpuSprCount, sprDepth);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, npuTex);
            glUniform1i(locNpuTex, 2);
            glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
            glDispatchCompute(WIDTH / 64, 1, 1);
            glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
        }
        glFinish(); // make the work timing meaningful for the profiler
        double tw1 = now_us();

        // ----- Blit to the window ------------------------------------------
        // Combined / GPU-only sample the compute output (tex); NPU-only samples
        // the column-major NPU texture directly, hence the transpose.
        int pw, ph;
        SDL_GetWindowSizeInPixels(window, &pw, &ph);
        glViewport(0, 0, pw, ph);
        glActiveTexture(GL_TEXTURE0);
        glUseProgram(blitProg);
        glUniform1i(locTranspose, mode == 2 ? 1 : 0);
        glBindTexture(GL_TEXTURE_2D, mode == 2 ? npuTex : tex);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        SDL_GL_SwapWindow(window);
        double tw2 = now_us();

        acc_work    += tw1 - tw0;
        acc_present += tw2 - tw1;
        acc_total   += tw2 - t_frame0;

        if (SDL_GetTicks() - last_time > 1000) {
            double f = (double)(prof_frames ? prof_frames : 1);
            char title[200];
            snprintf(title, sizeof(title),
                     "Raycast [%s] | FPS: %u | GPU (%.1f, %.1f) | NPU (%.1f, %.1f)",
                     modeName[mode], frames, pos_x, pos_y, npu_pos_x, npu_pos_y);
            SDL_SetWindowTitle(window, title);
            fprintf(stderr, "[prof] %-8s %4u fps | total %6.3f | work %6.3f | present %6.3f (ms)\n",
                    modeName[mode], frames, acc_total / f / 1000.0,
                    acc_work / f / 1000.0, acc_present / f / 1000.0);
            acc_work = acc_present = acc_total = 0;
            prof_frames = 0; frames = 0;
            last_time = SDL_GetTicks();
        }
    }

    SDL_GL_DestroyContext(glctx);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
