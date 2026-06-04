//===- shaders.h --------------------------------------------------*- C++ -*-===//
//
// GLSL shader sources for the GPU raycaster (extracted from host.cpp).
//
//===----------------------------------------------------------------------===//
#pragma once

// Fullscreen-triangle vertex shader.
static const char *kVertSrc = R"(#version 430 core
out vec2 vUV;
void main() {
    vec2 p = vec2(float((gl_VertexID << 1) & 2), float(gl_VertexID & 2));
    vUV = p;
    gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);
}
)";

// Samples the shared screen texture. The y-flip corrects GL's bottom-left origin.
static const char *kFragSrc = R"(#version 430 core
in vec2 vUV;
out vec4 frag;
uniform sampler2D uTex;
uniform int uTranspose;   // 1 when sampling the column-major NPU texture directly
void main() {
    vec2 uv = vec2(vUV.x, 1.0 - vUV.y);
    if (uTranspose == 1) uv = uv.yx;
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
uniform vec4  uSprA[8];      // (screenCx, halfW, top, bot)
uniform float uSprDepth[8];  // per-sprite camera depth (for wall occlusion)

const int W = 1024;
const int H = 1024;
const int MAP_W = 16;
const int MAP_H = 16;
const int TEXW = 64;
const float SPR = 64.0;

uniform sampler2D uNpuTex;   // NPU-rendered scene (column-major → transposed)
uniform int uMode;           // 0 = combined, 1 = GPU only, 2 = NPU only

// Palette-indexed textures, shared with the NPU kernel (the single source of
// truth). uPacked holds 4-bit indices: 4 walls (2048 bytes each) then the demon
// sprite (2048 bytes); uWallPal / uDemonPal turn an index into 0xRRGGBBAA.
layout(std430, binding = 2) readonly buffer PackBuf { uint uPacked[]; };
uniform uint uWallPal[16];
uniform uint uDemonPal[16];
const int DEMON_BYTE0 = 4 * 2048;   // demon texels start after the 4 walls

vec4 palColor(uint v) {   // v = 0xRRGGBBAA (R in MSB), matches the NPU output
    return vec4(float((v >> 24) & 0xFFu), float((v >> 16) & 0xFFu),
                float((v >>  8) & 0xFFu), float( v        & 0xFFu)) / 255.0;
}
uint packedByte(int bytePos) {
    return (uPacked[bytePos >> 2] >> uint((bytePos & 3) * 8)) & 0xFFu;
}

// World map, uploaded by the host as an SSBO (see g_map in host.cpp).
layout(std430, binding = 1) readonly buffer MapBuf { int mapData[]; };

vec4 sampleWall(int mv, int tx, int ty) {
    int off = ty * TEXW + tx;                       // texel index in the 64x64 tile
    uint b = packedByte((mv - 1) * 2048 + (off >> 1));
    uint idx = ((off & 1) == 0) ? (b >> 4) : (b & 0xFu);   // wall nibble order
    return palColor(uWallPal[idx]);
}

void main() {
    int col = int(gl_GlobalInvocationID.x);
    if (col >= W) return;

    // NPU-only mode (uMode == 2) never dispatches this shader; the host blits
    // the NPU texture directly. Here uMode is 0 (combined) or 1 (GPU only).

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
    float side_shade = (side == 1) ? 0.65 : 1.0;

    vec4 ceilC  = vec4(vec3(0x1A, 0x1A, 0x2E) / 255.0, 1.0);
    vec4 floorC = vec4(vec3(0x0F, 0x34, 0x60) / 255.0, 1.0);

    // Collect sprites that cover this column and sit in front of the wall.
    int  sc = 0;
    vec4 sA[8];
    for (int k = 0; k < uSpriteCount; k++) {
        float cx = uSprA[k].x, hw = uSprA[k].y;
        if (uSprDepth[k] >= perp) continue;      // behind wall
        if (float(col) < cx - hw || float(col) >= cx + hw) continue;
        sA[sc] = uSprA[k]; sc++;
    }

    for (int y = 0; y < H; y++) {
        vec4 c;
        if (y < top) c = ceilC;
        else if (y >= bot) c = floorC;
        else {
            int texY = int(float(y - top) / float(lineH) * float(TEXW)) & (TEXW - 1);
            vec4 texel;
            if (mv == 4 && uMode == 0) {
                // Map the whole NPU frame onto the wall face like a texture:
                // wallX runs across the face, the wall slice maps top..bot.
                // NPU output is column-major, so the fetch is transposed.
                int nx = int(wallX * float(W));
                int ny = int(clamp(float(y - top) / float(lineH), 0.0, 0.999) * float(H));
                texel = texelFetch(uNpuTex, ivec2(ny, nx), 0);
            } else {
                texel = sampleWall(mv, texX, texY);
            }
            c = vec4(texel.rgb * side_shade * fog, 1.0);
        }
        // Sprites (already filtered to this column); nearest is last -> wins.
        for (int k = 0; k < sc; k++) {
            float cx = sA[k].x, hw = sA[k].y, st = sA[k].z, sb = sA[k].w;
            if (float(y) < st || float(y) >= sb) continue;
            float sx = (float(col) - (cx - hw)) / (2.0 * hw) * SPR;
            float sy = (float(y) - st) / (sb - st) * SPR;
            int tx = clamp(int(sx), 0, 63);
            int ty = clamp(int(sy), 0, 63);
            int off = ty * 64 + tx;
            uint b = packedByte(DEMON_BYTE0 + (off >> 1));
            uint idx = ((off & 1) == 0) ? (b & 0xFu) : (b >> 4);   // demon nibble order
            if (idx == 0u) continue;   // palette index 0 = transparent
            c = palColor(uDemonPal[idx]);
        }
        imageStore(uImg, ivec2(col, y), c);
    }
}
)";
