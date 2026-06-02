//===- host.cpp ---------------------------------------------------*- C++ -*-===//
//
// Raycaster with two interchangeable backends, displayed through OpenGL:
//   * NPU  : the AIE kernel marches the rays (XRT), result uploaded to a texture
//   * GPU  : a GLSL compute shader marches the rays straight into the texture
//
// Press TAB (or G/N) to switch backends live and compare FPS in the title bar.
// Movement: W/S forward/back, A/D strafe, Left/Right rotate, Esc/Q quit.
// F toggles a vertical flip if the image is upside down on your driver.
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

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>      // GL 1.1 protos + (via glext) modern typedefs/enums

#define WIDTH 1024
#define HEIGHT 1024
#define N (WIDTH * HEIGHT)
#define Q12 4096

// ---------------------------------------------------------------------------
// Minimal GL 2.0+/4.3 loader. The 1.1 calls (glViewport, glBindTexture, ...)
// link straight from libGL; everything below is resolved at runtime once the
// context exists. The dev headers for a loader (GLEW/epoxy) aren't installed.
// ---------------------------------------------------------------------------
#define GL_FUNCS \
    X(PFNGLCREATESHADERPROC, glCreateShader) \
    X(PFNGLSHADERSOURCEPROC, glShaderSource) \
    X(PFNGLCOMPILESHADERPROC, glCompileShader) \
    X(PFNGLGETSHADERIVPROC, glGetShaderiv) \
    X(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog) \
    X(PFNGLCREATEPROGRAMPROC, glCreateProgram) \
    X(PFNGLATTACHSHADERPROC, glAttachShader) \
    X(PFNGLLINKPROGRAMPROC, glLinkProgram) \
    X(PFNGLGETPROGRAMIVPROC, glGetProgramiv) \
    X(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog) \
    X(PFNGLDELETESHADERPROC, glDeleteShader) \
    X(PFNGLUSEPROGRAMPROC, glUseProgram) \
    X(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation) \
    X(PFNGLUNIFORM2FPROC, glUniform2f) \
    X(PFNGLUNIFORM1IPROC, glUniform1i) \
    X(PFNGLUNIFORM4FVPROC, glUniform4fv) \
    X(PFNGLTEXSTORAGE2DPROC, glTexStorage2D) \
    X(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays) \
    X(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray) \
    X(PFNGLBINDIMAGETEXTUREPROC, glBindImageTexture) \
    X(PFNGLDISPATCHCOMPUTEPROC, glDispatchCompute) \
    X(PFNGLMEMORYBARRIERPROC, glMemoryBarrier)

#define X(type, name) static type name;
GL_FUNCS
#undef X

static void loadGL() {
#define X(type, name) name = (type)SDL_GL_GetProcAddress(#name);
    GL_FUNCS
#undef X
}

// Same world map as kernel.cpp / the compute shader (for sprite collision).
static const int g_map[256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,2,2,0,0,0,0,0,0,3,3,0,0,0,1,
    1,0,2,0,0,0,0,0,0,0,0,3,0,0,0,1,
    1,0,2,0,0,0,1,1,1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,1,0,0,0,0,0,4,4,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,4,0,1,
    1,0,0,3,3,3,0,0,0,0,0,0,0,4,0,1,
    1,0,0,0,0,3,0,0,0,2,2,2,0,0,0,1,
    1,0,0,0,0,0,0,0,0,2,0,0,0,0,0,1,
    1,0,1,1,0,0,0,0,0,2,0,0,0,0,0,1,
    1,0,1,0,0,0,0,4,4,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,4,0,0,0,0,3,3,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};
#define MAX_SPRITES 8

// ---------------------------------------------------------------------------
// GLSL shaders (the world map lives in the compute shader; the NPU keeps its
// own copy in kernel.cpp -- both are kept identical so the scene matches).
// ---------------------------------------------------------------------------

// Fullscreen-triangle vertex shader.
static const char *kVertSrc = R"(#version 430 core
out vec2 vUV;
void main() {
    vec2 p = vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2));
    vUV = p;
    gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);
}
)";

// Samples the shared screen texture. uSwap transposes (used for the NPU buffer,
// which is column-major); uFlipY corrects GL's bottom-left origin if needed.
static const char *kFragSrc = R"(#version 430 core
in vec2 vUV;
out vec4 frag;
uniform sampler2D uTex;
uniform int uSwap;
uniform int uFlipY;
void main() {
    vec2 uv = vUV;
    if (uFlipY == 1) uv.y = 1.0 - uv.y;
    if (uSwap == 1) uv = uv.yx;
    frag = texture(uTex, uv);
}
)";

// Compute raycaster: one invocation per screen column, writes the whole column.
static const char *kCompSrc = R"(#version 430 core
layout(local_size_x = 64) in;
layout(rgba8, binding = 0) uniform writeonly image2D uImg;

uniform vec2 uPos;
uniform vec2 uDir;
uniform vec2 uPlane;

uniform int  uSpriteCount;
uniform vec4 uSprA[8];   // (screenCx, halfW, top, bot)
uniform vec4 uSprB[8];   // (depth, r, g, b)

const int W = 1024;
const int H = 1024;
const int MAP_W = 16;
const int MAP_H = 16;
const int TEXW = 64;
const float SPR = 64.0;
const float SPR_R2 = 900.0;
const float SPR_CORE = 180.0;

const int mapData[256] = int[256](
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,2,2,0,0,0,0,0,0,3,3,0,0,0,1,
    1,0,2,0,0,0,0,0,0,0,0,3,0,0,0,1,
    1,0,2,0,0,0,1,1,1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
    1,0,0,0,0,0,1,0,0,0,0,0,4,4,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,4,0,1,
    1,0,0,3,3,3,0,0,0,0,0,0,0,4,0,1,
    1,0,0,0,0,3,0,0,0,2,2,2,0,0,0,1,
    1,0,0,0,0,0,0,0,0,2,0,0,0,0,0,1,
    1,0,1,1,0,0,0,0,0,2,0,0,0,0,0,1,
    1,0,1,0,0,0,0,4,4,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,4,0,0,0,0,3,3,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
);

vec3 wallColor(int v) {
    if (v == 1) return vec3(0xFF, 0x4D, 0x4D) / 255.0;
    if (v == 2) return vec3(0x4D, 0x99, 0xFF) / 255.0;
    if (v == 3) return vec3(0x4D, 0xFF, 0x80) / 255.0;
    if (v == 4) return vec3(0xFF, 0xFF, 0x4D) / 255.0;
    return vec3(0xDD, 0xDD, 0xDD) / 255.0;
}

// Procedural brick: horizontal mortar + per-row-offset vertical mortar.
bool isMortar(int tx, int ty) {
    bool hm = (ty % 16) < 3;
    int parity = (ty / 16) & 1;
    int txo = (tx + parity * 16) % 32;
    bool vm = txo < 3;
    return hm || vm;
}

void main() {
    int col = int(gl_GlobalInvocationID.x);
    if (col >= W) return;

    float cameraX = 2.0 * float(col) / float(W) - 1.0;
    vec2 rd = uDir + uPlane * cameraX;
    if (abs(rd.x) < 1e-6) rd.x = 1e-6;
    if (abs(rd.y) < 1e-6) rd.y = 1e-6;

    ivec2 mp = ivec2(floor(uPos));
    vec2 deltaDist = abs(1.0 / rd);
    ivec2 stp;
    vec2 sideDist;
    if (rd.x < 0.0) { stp.x = -1; sideDist.x = (uPos.x - float(mp.x)) * deltaDist.x; }
    else            { stp.x =  1; sideDist.x = (float(mp.x) + 1.0 - uPos.x) * deltaDist.x; }
    if (rd.y < 0.0) { stp.y = -1; sideDist.y = (uPos.y - float(mp.y)) * deltaDist.y; }
    else            { stp.y =  1; sideDist.y = (float(mp.y) + 1.0 - uPos.y) * deltaDist.y; }

    int side = 0, hit = 0, mv = 1;
    for (int i = 0; i < 64 && hit == 0; i++) {
        if (sideDist.x < sideDist.y) { sideDist.x += deltaDist.x; mp.x += stp.x; side = 0; }
        else                         { sideDist.y += deltaDist.y; mp.y += stp.y; side = 1; }
        if (mp.x < 0 || mp.x >= MAP_W || mp.y < 0 || mp.y >= MAP_H) { hit = 1; mv = 1; }
        else { mv = mapData[mp.y * 16 + mp.x]; if (mv > 0) hit = 1; }
    }

    float perp = (side == 0) ? (sideDist.x - deltaDist.x) : (sideDist.y - deltaDist.y);
    if (perp < 1e-4) perp = 1e-4;

    int lineH = int(float(H) / perp);
    if (lineH > H * 2) lineH = H * 2;
    int top = H / 2 - lineH / 2;
    int bot = H / 2 + lineH / 2;

    // Wall texture column from the fractional hit position.
    float wallX = (side == 0) ? (uPos.y + perp * rd.y) : (uPos.x + perp * rd.x);
    wallX -= floor(wallX);
    int texX = int(wallX * float(TEXW));

    float fog = clamp(float(lineH) / 512.0, 0.125, 1.0);
    vec3 brickBase = wallColor(mv);
    if (side == 1) brickBase *= 0.65;            // shade Y-side hits
    vec3 mortarBase = vec3(0x30, 0x30, 0x30) / 255.0;

    vec4 ceilC  = vec4(vec3(0x1A, 0x1A, 0x2E) / 255.0, 1.0);
    vec4 floorC = vec4(vec3(0x0F, 0x34, 0x60) / 255.0, 1.0);

    // Collect sprites that cover this column and sit in front of the wall.
    int  sc = 0;
    vec4 sA[8]; vec4 sB[8];
    for (int k = 0; k < uSpriteCount; k++) {
        float cx = uSprA[k].x, hw = uSprA[k].y;
        if (uSprB[k].x >= perp) continue;        // behind wall
        if (float(col) < cx - hw || float(col) >= cx + hw) continue;
        sA[sc] = uSprA[k]; sB[sc] = uSprB[k]; sc++;
    }

    for (int y = 0; y < H; y++) {
        vec4 c;
        if (y < top) c = ceilC;
        else if (y >= bot) c = floorC;
        else {
            int texY = int(float(y - top) / float(lineH) * float(TEXW)) & (TEXW - 1);
            vec3 base = isMortar(texX, texY) ? mortarBase : brickBase;
            c = vec4(base * fog, 1.0);
        }
        // Sprites (already filtered to this column); nearest is last -> wins.
        for (int k = 0; k < sc; k++) {
            float cx = sA[k].x, hw = sA[k].y, st = sA[k].z, sb = sA[k].w;
            if (float(y) < st || float(y) >= sb) continue;
            float sx = (float(col) - (cx - hw)) / (2.0 * hw) * SPR;
            float sy = (float(y) - st) / (sb - st) * SPR;
            float dx = sx - SPR * 0.5, dy = sy - SPR * 0.5;
            float d2 = dx * dx + dy * dy;
            if (d2 >= SPR_R2) continue;
            vec3 scol = sB[k].yzw;
            if (d2 < SPR_CORE) scol = mix(scol, vec3(1.0), 0.5);
            c = vec4(scol, 1.0);
        }
        imageStore(uImg, ivec2(col, y), c);
    }
}
)";

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

    // ----- Camera state ----------------------------------------------------
    float pos_x = 3.5f, pos_y = 3.5f;
    float dir_x = 1.0f, dir_y = 0.0f;
    float plane_x = 0.0f, plane_y = 0.66f;
    const float move_speed = 3.0f;
    const float rot_speed  = 2.2f;

    // ----- Sprites: wandering orbs ----------------------------------------
    struct SpriteW { float x, y, vx, vy; uint32_t color; };
    std::vector<SpriteW> sprites = {
        { 2.5f,  7.5f,  0.9f,  0.7f, 0xFF6030u}, // orange
        { 8.5f,  2.5f, -0.6f,  1.0f, 0x30FF80u}, // green
        {12.5f, 12.5f,  0.8f, -0.9f, 0x4080FFu}, // blue
        { 6.5f,  9.5f,  1.1f,  0.4f, 0xFFE040u}, // yellow
        {10.5f,  5.5f, -1.0f, -0.6f, 0xFF40C0u}, // pink
        { 4.5f, 12.5f,  0.5f, -1.1f, 0x40FFE0u}, // cyan
    };

    // Projected (screen-space) sprite data, shared by both backends.
    int     sprCount = 0;
    float   sprA[MAX_SPRITES][4];     // screenCx, halfW, top, bot
    float   sprB[MAX_SPRITES][4];     // depth, r, g, b   (GPU uniforms)
    int32_t sprPack[MAX_SPRITES][6];  // cx, halfW, top, bot, depth_q12, 0xRRGGBB (NPU)

    auto solid = [&](float x, float y) -> bool {
        int mx = (int)floorf(x), my = (int)floorf(y);
        if (mx < 0 || mx >= 16 || my < 0 || my >= 16) return true;
        return g_map[my * 16 + mx] != 0;
    };
    auto update_sprites = [&](float dt) {
        for (auto &s : sprites) {
            float nx = s.x + s.vx * dt, ny = s.y + s.vy * dt;
            if (solid(nx, s.y)) { s.vx = -s.vx; nx = s.x; }
            if (solid(s.x, ny)) { s.vy = -s.vy; ny = s.y; }
            s.x = nx; s.y = ny;
        }
    };
    auto project_sprites = [&]() {
        struct P { float depth, cx, hw, top, bot; uint32_t color; };
        std::vector<P> ps;
        float det = plane_x * dir_y - dir_x * plane_y;
        if (fabsf(det) < 1e-6f) det = 1e-6f;
        float invDet = 1.0f / det;
        for (auto &s : sprites) {
            float rx = s.x - pos_x, ry = s.y - pos_y;
            float tx = invDet * (dir_y * rx - dir_x * ry);
            float ty = invDet * (-plane_y * rx + plane_x * ry); // camera depth
            if (ty < 0.2f) continue;                            // behind / too close
            float cx = (WIDTH * 0.5f) * (1.0f + tx / ty);
            float sh = fabsf((float)HEIGHT / ty);
            ps.push_back({ty, cx, sh * 0.5f, HEIGHT * 0.5f - sh * 0.5f,
                          HEIGHT * 0.5f + sh * 0.5f, s.color});
        }
        std::sort(ps.begin(), ps.end(),
                  [](const P &a, const P &b) { return a.depth > b.depth; }); // far first
        sprCount = std::min((int)ps.size(), (int)MAX_SPRITES);
        for (int i = 0; i < sprCount; i++) {
            const P &p = ps[i];
            sprA[i][0] = p.cx; sprA[i][1] = p.hw; sprA[i][2] = p.top; sprA[i][3] = p.bot;
            sprB[i][0] = p.depth;
            sprB[i][1] = ((p.color >> 16) & 0xFF) / 255.0f;
            sprB[i][2] = ((p.color >>  8) & 0xFF) / 255.0f;
            sprB[i][3] = ( p.color        & 0xFF) / 255.0f;
            sprPack[i][0] = (int32_t)lroundf(p.cx);
            sprPack[i][1] = (int32_t)lroundf(p.hw);
            sprPack[i][2] = (int32_t)lroundf(p.top);
            sprPack[i][3] = (int32_t)lroundf(p.bot);
            sprPack[i][4] = (int32_t)lroundf(p.depth * Q12);
            sprPack[i][5] = (int32_t)(p.color & 0xFFFFFF);
        }
    };

    // Per-column ray + DDA setup for the NPU (column-major device buffer).
    // Each tile also carries the (column-independent) sprite block at in[12..].
    auto fill_params = [&]() {
        int32_t pos_x_q12 = pack_q12(pos_x);
        int32_t pos_y_q12 = pack_q12(pos_y);
        int32_t map_x_0 = (int32_t)floorf(pos_x);
        int32_t map_y_0 = (int32_t)floorf(pos_y);
        for (int col = 0; col < WIDTH; col++) {
            float camera_x = 2.0f * col / (float)WIDTH - 1.0f;
            int32_t ray_dir_x = pack_q12(dir_x + plane_x * camera_x);
            int32_t ray_dir_y = pack_q12(dir_y + plane_y * camera_x);
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
            int32_t params[13 + MAX_SPRITES * 6] = {
                pos_x_q12, pos_y_q12, ray_dir_x, ray_dir_y,
                delta_x,   delta_y,   side_x_0,  side_y_0,
                step_x,    step_y,    map_x_0,   map_y_0,
            };
            params[12] = sprCount;
            for (int k = 0; k < sprCount; k++)
                for (int j = 0; j < 6; j++)
                    params[13 + k * 6 + j] = sprPack[k][j];
            int plen = 13 + sprCount * 6;
            for (int band = 0; band < 4; band++)
                memcpy(&bufInA[col * HEIGHT + band * 256], params, plen * sizeof(int32_t));
        }
    };

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
    loadGL();

    fprintf(stderr, "GL: %s | %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));

    GLuint blitProg = linkProgram({compileShader(GL_VERTEX_SHADER, kVertSrc),
                                   compileShader(GL_FRAGMENT_SHADER, kFragSrc)});
    GLuint compProg = linkProgram({compileShader(GL_COMPUTE_SHADER, kCompSrc)});

    GLint locSwap  = glGetUniformLocation(blitProg, "uSwap");
    GLint locFlipY = glGetUniformLocation(blitProg, "uFlipY");
    GLint locPos   = glGetUniformLocation(compProg, "uPos");
    GLint locDir   = glGetUniformLocation(compProg, "uDir");
    GLint locPlane = glGetUniformLocation(compProg, "uPlane");
    GLint locSprCount = glGetUniformLocation(compProg, "uSpriteCount");
    GLint locSprA  = glGetUniformLocation(compProg, "uSprA");
    GLint locSprB  = glGetUniformLocation(compProg, "uSprB");

    // Shared screen texture: compute writes it (image), blit samples it.
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, WIDTH, HEIGHT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLuint vao;
    glGenVertexArrays(1, &vao);

    // NPU warmup so its first measured frame is representative.
    fill_params();
    bo_inA.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    kernel(opcode, 0, 0, bo_inA, bo_out).wait2();

    bool running = true;
    bool use_npu = true;   // start on the NPU backend
    int  flipY   = 1;      // GL origin is bottom-left; flip by default

    uint64_t last_time = SDL_GetTicks();
    uint32_t frames = 0;
    double acc_work = 0, acc_present = 0, acc_total = 0;
    uint32_t prof_frames = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    using clk = std::chrono::high_resolution_clock;
    auto now_us = [] {
        return std::chrono::duration<double, std::micro>(clk::now().time_since_epoch()).count();
    };

    auto rotate = [&](float ang) {
        float cs = cosf(ang), sn = sinf(ang);
        float ndx = dir_x * cs - dir_y * sn, ndy = dir_x * sn + dir_y * cs;
        dir_x = ndx; dir_y = ndy;
        float npx = plane_x * cs - plane_y * sn, npy = plane_x * sn + plane_y * cs;
        plane_x = npx; plane_y = npy;
    };

    while (running) {
        frames++; prof_frames++;
        double t_frame0 = now_us();

        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = false;
            if (ev.type == SDL_EVENT_KEY_DOWN) {
                SDL_Keycode k = ev.key.key;
                if (k == SDLK_ESCAPE || k == SDLK_Q) running = false;
                else if (k == SDLK_TAB) use_npu = !use_npu;
                else if (k == SDLK_N) use_npu = true;
                else if (k == SDLK_G) use_npu = false;
                else if (k == SDLK_F) flipY = !flipY;
            }
        }
        if (!running) break;

        auto t = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(t - t0).count();
        t0 = t;
        if (dt > 0.1f) dt = 0.1f;

        const bool *keys = SDL_GetKeyboardState(NULL);
        float move = move_speed * dt, rot = rot_speed * dt;
        if (keys[SDL_SCANCODE_LEFT])  rotate(rot);
        if (keys[SDL_SCANCODE_RIGHT]) rotate(-rot);
        float nx = pos_x, ny = pos_y;
        if (keys[SDL_SCANCODE_W]) { nx += dir_x * move;   ny += dir_y * move; }
        if (keys[SDL_SCANCODE_S]) { nx -= dir_x * move;   ny -= dir_y * move; }
        if (keys[SDL_SCANCODE_A]) { nx -= plane_x * move; ny -= plane_y * move; }
        if (keys[SDL_SCANCODE_D]) { nx += plane_x * move; ny += plane_y * move; }
        if (nx < 1.05f) nx = 1.05f; if (nx > 14.95f) nx = 14.95f;
        if (ny < 1.05f) ny = 1.05f; if (ny > 14.95f) ny = 14.95f;
        pos_x = nx; pos_y = ny;

        // Advance + project the sprites once; both backends consume the result.
        update_sprites(dt);
        project_sprites();

        // ----- Produce the frame into the shared texture -------------------
        double tw0 = now_us();
        if (use_npu) {
            fill_params();
            bo_inA.sync(XCL_BO_SYNC_BO_TO_DEVICE);
            kernel(opcode, 0, 0, bo_inA, bo_out).wait2();
            bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
            glBindTexture(GL_TEXTURE_2D, tex);
            // bufOut is 0xRRGGBBAA packed; UNSIGNED_INT_8_8_8_8 reads R from MSB.
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT,
                            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, bufOut);
        } else {
            glUseProgram(compProg);
            glUniform2f(locPos, pos_x, pos_y);
            glUniform2f(locDir, dir_x, dir_y);
            glUniform2f(locPlane, plane_x, plane_y);
            glUniform1i(locSprCount, sprCount);
            glUniform4fv(locSprA, sprCount, &sprA[0][0]);
            glUniform4fv(locSprB, sprCount, &sprB[0][0]);
            glBindImageTexture(0, tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
            glDispatchCompute(WIDTH / 64, 1, 1);
            glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
        }
        glFinish(); // make the work timing meaningful for the profiler
        double tw1 = now_us();

        // ----- Blit the texture to the window ------------------------------
        int pw, ph;
        SDL_GetWindowSizeInPixels(window, &pw, &ph);
        glViewport(0, 0, pw, ph);
        glUseProgram(blitProg);
        glUniform1i(locSwap, use_npu ? 1 : 0); // NPU buffer is transposed
        glUniform1i(locFlipY, flipY);
        glBindTexture(GL_TEXTURE_2D, tex);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        SDL_GL_SwapWindow(window);
        double tw2 = now_us();

        acc_work    += tw1 - tw0;
        acc_present += tw2 - tw1;
        acc_total   += tw2 - t_frame0;

        if (SDL_GetTicks() - last_time > 1000) {
            double f = (double)(prof_frames ? prof_frames : 1);
            const char *mode = use_npu ? "NPU" : "GPU";
            char title[200];
            snprintf(title, sizeof(title), "Raycast [%s] | FPS: %u | pos (%.1f, %.1f)",
                     mode, frames, pos_x, pos_y);
            SDL_SetWindowTitle(window, title);
            fprintf(stderr, "[prof] %s %4u fps | total %6.3f | work %6.3f | present %6.3f (ms)\n",
                    mode, frames, acc_total / f / 1000.0,
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
