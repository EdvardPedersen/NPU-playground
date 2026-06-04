#include <stdint.h>
#include <stdlib.h>
#include <aie_api/aie.hpp>
#include "tex_data.h"

// ---------------------------------------------------------------------------
// Textured raycaster kernel for the AIE/NPU.
//
// Output is column-major (bufOut[col*H + row]); each 256-element tile is a
// contiguous vertical strip of one screen column.  Walls use palette-indexed
// texture lookups (same pixel data as the GPU atlas) with pre-multiplied
// fog + side-shading, then a scalar pass overlays billboarded textured demon
// sprites with a per-column depth test against the wall distance.
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

// World map. Single source: map_data.inc, also included by host.cpp. Brace
// elision fills this 2D array from the flat list (MAP[y][x] still works).
static const int8_t MAP[MAP_H][MAP_W] = {
#include "map_data.inc"
};

extern "C" {

void raycastLine(int32_t *in, int32_t *out, uint64_t split) {
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

    // Gather the 64 raw palette colors for this texture column. The nibble
    // unpack + 16-entry palette lookup stay scalar (data-dependent gathers).
    alignas(128) uint32_t col_raw[64];
    for (int y = 0; y < 64; y++) {
        int off = y * TEXW + texX;
        uint8_t packed_byte = tex_base[off >> 1];
        uint8_t idx = (off & 1) ? (packed_byte & 0x0Fu) : (packed_byte >> 4);
        col_raw[y] = wall_pal[idx];
    }

    // Vectorized fog + side-shade: every RGB byte *= shade_q8 >> 8, alpha set to
    // 0xFF. 64 RGBA colors = 256 bytes = two uint8x128 (1024-bit) vectors on
    // aie2p, so the whole column shades in two mul/srs ops instead of 64*3.
    alignas(128) uint32_t col_data[64];
    {
        const uint8_t *src = (const uint8_t *)col_raw;
        uint8_t *dst = (uint8_t *)col_data;
        aie::vector<uint8_t, 128> shade_v = aie::broadcast<uint8_t, 128>((uint8_t)shade_q8);
        // 0x000000FF per uint32 -> 0xFF in each color's alpha byte (little-endian).
        aie::vector<uint8_t, 128> amask =
            aie::broadcast<uint32_t, 32>(0x000000FFu).cast_to<uint8_t>();
        for (int c = 0; c < 256; c += 128) {
            aie::vector<uint8_t, 128> v = aie::load_v<128>(src + c);
            aie::vector<uint8_t, 128> s = aie::mul(v, shade_v).to_vector<uint8_t>(8);
            aie::store_v(dst + c, aie::bit_or(s, amask));
        }
    }

    // texY per row = (row - wall_top) * TEXW / line_h.  Q16 step avoids
    // stair-stepping that would be visible with lower-resolution Q8 rounding.
    int32 tstep = ((int32)TEXW << 16) / line_h;   // Q16

    // --- Scalar wall / ceiling / floor fill --------------------------------
    // The shade pass above is vectorized; this textured fill stays scalar.
    // A vectorized gather via aie::parallel_lookup was tried (faster!) but the
    // per-column-rebuilt LUT left thin banding the toolchain couldn't fix here
    // (chess_separator is a no-op under Peano); parallel_lookup wants a static,
    // reused LUT. Reverted to the scalar gather.
    for (int r = 0; r < 256; r++) {
        uint32_t c;
        int32_t rr = row_start + r;
        if (rr < wall_top) {
            c = ceiling_color;
        } else if (rr >= wall_bot) {
            c = floor_color;
        } else {
            int32_t ty = ((rr - wall_top) * tstep) >> 16;
            c = col_data[ty & (TEXW - 1)];
        }
        tile[r] = c;
    }

    // --- Scalar sprite overlay (textured demon) ----------------------------
    int32 nspr = in[12];
    if (nspr > MAX_SPRITES) nspr = MAX_SPRITES;
    for (int k = 0; k < nspr; k++) {
        int32 base    = 13 + k * 5;
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

        // sx is constant down the column; compute it once.
        int32 sx = (screen_col - (cx - half_w)) * SPR_SIZE / (2 * half_w);
        if (sx < 0 || sx >= SPR_SIZE) continue;

        // sy per row = (r - sp_top) * SPR_SIZE / sp_h.  Use a Q16 step (one
        // divide) instead of a per-row divide, matching the wall texturing and
        // avoiding the horizontal banding the per-row divide produced.
        int32 sy_step = ((int32)SPR_SIZE << 16) / sp_h;   // Q16

        int32 r0 = sp_top > row_start ? sp_top : row_start;
        int32 r1 = sp_bot < row_start + 256 ? sp_bot : row_start + 256;
        for (int32 r = r0; r < r1; r++) {
            int32 sy = ((r - sp_top) * sy_step) >> 16;
            if (sy < 0 || sy >= SPR_SIZE) continue;

            int off = sy * SPR_SIZE + sx;
            uint8_t packed_byte = demon_packed[off >> 1];
            // demon_packed uses the opposite nibble order from the wall textures:
            // even texel in the low nibble, odd texel in the high nibble.
            uint8_t idx = (off & 1) ? (packed_byte >> 4) : (packed_byte & 0x0Fu);
            if (idx == 0) continue;  // transparent

            uint32_t c = demon_pal[idx];
            tile[r - row_start] = c;
        }
    }
}

} // extern "C"
