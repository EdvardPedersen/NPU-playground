#include <stdint.h>
#include <stdlib.h>
#include "tex_data.h"

// ---------------------------------------------------------------------------
// Textured raycaster kernel for the AIE/NPU.
//
// Output is column-major (bufOut[col*H + row]); each 256-element tile is a
// contiguous vertical strip of one screen column.  Walls use palette-indexed
// texture lookups (same pixel data as the GPU atlas) with pre-multiplied
// fog + side-shading, then a scalar pass overlays billboarded sprites
// (procedural orbs) with a per-column depth test against the wall distance.
// ---------------------------------------------------------------------------

#define IMAGE_W 1024
#define IMAGE_H 1024
#define MAP_W 16
#define MAP_H 16
#define Q12 4096
#define MAX_DDA_STEPS 64

#define TEXW 64       // wall texture size (power of two)

#define SPR_SIZE 64   // demon sprite size
#define MAX_SPRITES 8

static const int8_t MAP[MAP_H][MAP_W] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 2, 2, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 1},
    {1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 1},
    {1, 0, 2, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 4, 4, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1},
    {1, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 4, 0, 1},
    {1, 0, 0, 0, 0, 3, 0, 0, 0, 2, 2, 2, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 3, 3, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

extern "C" {

void raycastLine(int32_t *in, int32_t *out, int32_t lineWidth, int32_t node,
                 uint64_t split, int32_t nodeWidth, int32_t image_width,
                 int32_t image_height, float stage) {
    uint32_t *tile = (uint32_t *)out;   // output buffer for this band

    uint32_t sp = (uint32_t)split;
    int32_t row_start = (int32_t)((sp & 3u) * 256u);
    int32_t screen_col = (int32_t)(sp >> 2);  // this tile's screen column

    int32 pos_x    = in[0];
    int32 pos_y    = in[1];
    int32 ray_dir_x = in[2];
    int32 ray_dir_y = in[3];
    int32 delta_x  = in[4];
    int32 delta_y  = in[5];
    int32 side_x   = in[6];
    int32 side_y   = in[7];
    int32 step_x   = in[8];
    int32 step_y   = in[9];
    int32 map_x    = in[10];
    int32 map_y    = in[11];

    // --- DDA grid traversal ------------------------------------------------
    int32 hit = 0;
    int32 hit_side = 0;
    for (int s = 0; s < MAX_DDA_STEPS; s++) {
        if (hit) break;
        if (side_x < side_y) { side_x += delta_x; map_x += step_x; hit_side = 0; }
        else                 { side_y += delta_y; map_y += step_y; hit_side = 1; }
        if (map_x < 0 || map_x >= MAP_W || map_y < 0 || map_y >= MAP_H) hit = 1;
        else if (MAP[map_y][map_x] > 0) hit = 1;
    }

    // --- Perpendicular distance + wall height ------------------------------
    int32 num, rd, rdir_perp, pos_perp;
    if (hit_side == 0) {
        num = map_x * Q12 - pos_x;
        if (step_x < 0) num += Q12;
        rd = ray_dir_x;
        rdir_perp = ray_dir_y;  // wallX uses the *other* axis
        pos_perp  = pos_y;
    } else {
        num = map_y * Q12 - pos_y;
        if (step_y < 0) num += Q12;
        rd = ray_dir_y;
        rdir_perp = ray_dir_x;
        pos_perp  = pos_x;
    }
    int32 num_a = num < 0 ? -num : num;
    int32 rd_a  = rd  < 0 ? -rd  : rd;
    if (num_a < 1) num_a = 1;
    if (rd_a  < 1) rd_a  = 1;

    int32 perp_q12 = (num_a * Q12) / rd_a;       // perp distance * 4096
    if (perp_q12 < 1) perp_q12 = 1;

    int32 line_h = (IMAGE_H * rd_a) / num_a;
    if (line_h > IMAGE_H * 2) line_h = IMAGE_H * 2;
    if (line_h < 1) line_h = 1;

    int16 wall_top = (int16)(IMAGE_H / 2 - line_h / 2);
    int16 wall_bot = (int16)(IMAGE_H / 2 + line_h / 2);

    // Wall texture column from the fractional hit position.
    int32 wallX_q12 = pos_perp + (int32)(((int64_t)perp_q12 * rdir_perp) >> 12);
    int32 texX = ((wallX_q12 & (Q12 - 1)) * TEXW) >> 12;   // [0, TEXW)

    int8_t mv = 1;
    if (map_x >= 0 && map_x < MAP_W && map_y >= 0 && map_y < MAP_H)
        mv = MAP[map_y][map_x];

    int32 fog = line_h >> 1;
    if (fog > 256) fog = 256;
    if (fog < 40)  fog = 40;

    uint32 ceiling_color = 0x1A1A2EFFu;
    uint32 floor_color   = 0x0F3460FFu;

    // Choose the texture LUT based on wall type
    const uint8_t *tex_base;
    switch (mv) {
    case 1: tex_base = brick_tex_idx_packed; break;
    case 2: tex_base = stone_tex_idx_packed; break;
    case 3: tex_base = marble_tex_idx_packed; break;
    default: tex_base = tanstone_tex_idx_packed; break;
    }

    // Pre-multiply side shading + fog into one Q8 factor applied to all texels.
    // GPU: texel.rgb * side_shade * fog  where side_shade = 0.65 for Y-side.
    uint32_t side_q8 = hit_side ? 166 : 255;  // 166/255 ≈ 0.65
    uint32_t shade_q8 = (uint32_t)fog * side_q8 >> 8;
    if (shade_q8 > 255) shade_q8 = 255;

    // Load one column (64 texels) from the packed indexed texture, expand to
    // RGBA, and pre-apply the fog + side-shade factor.
    alignas(128) uint32_t col_data[64];
    for (int y = 0; y < 64; y++) {
        int off = y * TEXW + texX;
        uint8_t packed_byte = tex_base[off >> 1];
        uint8_t idx = (off & 1) ? (packed_byte & 0x0Fu) : (packed_byte >> 4);
        uint32_t c = wall_pal[idx];
        uint32_t r = ((c >> 24) & 0xFF) * shade_q8 >> 8;
        uint32_t g = ((c >> 16) & 0xFF) * shade_q8 >> 8;
        uint32_t b = ((c >>  8) & 0xFF) * shade_q8 >> 8;
        col_data[y] = (r << 24) | (g << 16) | (b << 8) | 0xFFu;
    }

    // texY per row = (row - wall_top) * TEXW / line_h.  Q16 step avoids
    // stair-stepping that would be visible with lower-resolution Q8 rounding.
    int32 tstep = ((int32)TEXW << 16) / line_h;   // Q16

    // --- Scalar wall / ceiling / floor fill --------------------------------
    // Uses the pre-shaded column with per-pixel texY lookups and row-based
    // ceiling/floor detection.
    for (int r = 0; r < 256; r++) {
        uint32_t c;
        int32_t rr = row_start + r;
        if (rr < wall_top) {
            c = ceiling_color;
        } else if (rr >= wall_bot) {
            c = floor_color;
        } else {
            int32_t relrow = rr - wall_top;
            int32_t ty = (relrow * tstep) >> 16;
            c = col_data[ty & (TEXW - 1)];
        }
        tile[r] = c;
    }

    // --- Scalar sprite overlay (textured imp) ------------------------------
    int32 nspr = in[12];
    if (nspr > MAX_SPRITES) nspr = MAX_SPRITES;
    for (int k = 0; k < nspr; k++) {
        int32 base    = 13 + k * 6;
        int32 cx      = in[base + 0];
        int32 half_w  = in[base + 1];
        int32 sp_top  = in[base + 2];
        int32 sp_bot  = in[base + 3];
        int32 depth   = in[base + 4];   // q12

        if (depth >= perp_q12) continue;            // behind the wall
        if (half_w < 1) continue;
        if (screen_col < cx - half_w || screen_col >= cx + half_w) continue;

        int32 sp_h = sp_bot - sp_top;
        if (sp_h < 1) continue;

        int32 r0 = sp_top > row_start ? sp_top : row_start;
        int32 r1 = sp_bot < row_start + 256 ? sp_bot : row_start + 256;
        for (int32 r = r0; r < r1; r++) {
            // Texture coordinates
            int32 sx = (screen_col - (cx - half_w)) * SPR_SIZE / (2 * half_w);
            int32 sy = (r - sp_top) * SPR_SIZE / sp_h;
            if (sx < 0 || sx >= SPR_SIZE || sy < 0 || sy >= SPR_SIZE) continue;

            int off = sy * SPR_SIZE + sx;
            uint8_t packed_byte = demon_packed[off >> 1];
            uint8_t idx = (off & 1) ? (packed_byte & 0x0Fu) : (packed_byte >> 4);
            if (idx == 0) continue;  // transparent

            uint32_t c = demon_pal[idx];
            tile[r - row_start] = c;
        }
    }
}

} // extern "C"
