#include <stdint.h>
#include <stdlib.h>
#include <aie_api/aie.hpp>

// ---------------------------------------------------------------------------
// Textured raycaster kernel for the AIE/NPU.
//
// Output is column-major (bufOut[col*H + row]); each 256-element tile is a
// contiguous vertical strip of one screen column.  Walls get a procedural
// brick texture (vectorized), then a scalar pass overlays billboarded sprites
// (procedural orbs) with a per-column depth test against the wall distance.
// ---------------------------------------------------------------------------

#define VEC_WIDTH 32
#define UINT_TYPE uint16
#define IMAGE_W 1024
#define IMAGE_H 1024
#define MAP_W 16
#define MAP_H 16
#define Q12 4096
#define MAX_DDA_STEPS 64

#define TEXW 64       // wall texture size (power of two)
#define BRICK_H 16    // brick row height
#define BRICK_W 32    // brick column width
#define MORTAR 3      // mortar line thickness

#define SPR_SIZE 64   // sprite texture size
#define SPR_R2   900  // orb radius^2 (30^2)
#define SPR_CORE 180  // inner highlight radius^2
#define MAX_SPRITES 8

alignas(128) static const int16 kIotaI16[VEC_WIDTH] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

alignas(128) static const int32 kIotaI32[VEC_WIDTH] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};

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

// Base wall colour (0xRRGGBBAA) per map value, darker on Y-side hits.
static inline uint32 wall_colour(int8_t v, int hit_side) {
    switch (v) {
    case 1:  return hit_side ? 0xAA3333FFu : 0xFF4D4DFFu; // red
    case 2:  return hit_side ? 0x3366AAFFu : 0x4D99FFFFu; // blue
    case 3:  return hit_side ? 0x33AA55FFu : 0x4DFF80FFu; // green
    case 4:  return hit_side ? 0xAAAA33FFu : 0xFFFF4DFFu; // yellow
    default: return hit_side ? 0x999999FFu : 0xDDDDDDFFu; // grey
    }
}

static inline uint32 apply_fog(uint32 c, int32 fog) {
    uint32 r = (((c >> 24) & 0xFF) * (uint32)fog) >> 8;
    uint32 g = (((c >> 16) & 0xFF) * (uint32)fog) >> 8;
    uint32 b = (((c >>  8) & 0xFF) * (uint32)fog) >> 8;
    return (r << 24) | (g << 16) | (b << 8) | 0xFFu;
}

extern "C" {

void raycastLine(int32_t *in, int32_t *out, int32_t lineWidth, int32_t node,
                 uint64_t split, int32_t nodeWidth, int32_t image_width,
                 int32_t image_height, float stage) {
    uint32_t *outPtr = (uint32_t *)out;
    uint32_t *tile = (uint32_t *)out;   // scalar view for the sprite pass

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
    uint32 brick_color  = apply_fog(wall_colour(mv, hit_side), fog);
    uint32 mortar_color = apply_fog(0x303030FFu, fog);

    uint32 ceiling_color = 0x1A1A2EFFu;
    uint32 floor_color   = 0x0F3460FFu;

    // texY per row = (row - wall_top) * TEXW / line_h.  The step is kept in
    // Q16 (int32): with only ~5 bits (Q8) the per-column rounding of the step,
    // amplified by relrow, made the brick courses visibly stair-step.
    int32 tstep = ((int32)TEXW << 16) / line_h;   // Q16

    // --- Vectorized textured wall / ceiling / floor fill -------------------
    auto top_v    = aie::broadcast<int32, VEC_WIDTH>((int32)wall_top);
    auto bot_v    = aie::broadcast<int32, VEC_WIDTH>((int32)wall_bot);
    auto brick_v  = aie::broadcast<uint32, VEC_WIDTH>(brick_color);
    auto mortar_v = aie::broadcast<uint32, VEC_WIDTH>(mortar_color);
    auto ceil_v   = aie::broadcast<uint32, VEC_WIDTH>(ceiling_color);
    auto floor_v  = aie::broadcast<uint32, VEC_WIDTH>(floor_color);
    auto iota     = aie::load_v<VEC_WIDTH>(kIotaI32);
    auto row_v    = aie::add(aie::broadcast<int32, VEC_WIDTH>(row_start), iota);
    auto step_v   = aie::broadcast<int32, VEC_WIDTH>((int32)VEC_WIDTH);
    auto tstep_v  = aie::broadcast<int32, VEC_WIDTH>(tstep);
    auto wtop_v   = aie::broadcast<int32, VEC_WIDTH>((int32)wall_top);
    auto texX_v   = aie::broadcast<int32, VEC_WIDTH>(texX);
    auto m15      = aie::broadcast<int32, VEC_WIDTH>((int32)(BRICK_H - 1));
    auto m31      = aie::broadcast<int32, VEC_WIDTH>((int32)(BRICK_W - 1));
    auto mmort    = aie::broadcast<int32, VEC_WIDTH>((int32)MORTAR);
    auto one_v    = aie::broadcast<int32, VEC_WIDTH>((int32)1);

    for (UINT_TYPE ri = 0; ri < 256; ri += VEC_WIDTH) {
        // texY = ((row - wall_top) * tstep) >> 16
        auto relrow = aie::sub(row_v, wtop_v);
        auto texY   = aie::to_vector<int32>(aie::mul(relrow, tstep_v), 16);

        // Brick pattern: horizontal mortar + (offset) vertical mortar.
        auto hmort  = aie::lt(aie::bit_and(texY, m15), mmort);
        auto parity = aie::bit_and(aie::downshift(texY, 4), one_v);
        auto txcol  = aie::add(aie::upshift(parity, 4), texX_v);   // shift odd rows
        auto vmort  = aie::lt(aie::bit_and(txcol, m31), mmort);

        auto color = aie::select(brick_v, mortar_v, hmort);
        color = aie::select(color, mortar_v, vmort);

        auto ceil_mask  = aie::lt(row_v, top_v);
        auto floor_mask = aie::ge(row_v, bot_v);
        color = aie::select(color, ceil_v, ceil_mask);
        color = aie::select(color, floor_v, floor_mask);

        aie::store_v(outPtr, color);
        outPtr += VEC_WIDTH;
        row_v = aie::add(row_v, step_v);
    }

    // --- Scalar sprite overlay (procedural orbs) ---------------------------
    int32 nspr = in[12];
    if (nspr > MAX_SPRITES) nspr = MAX_SPRITES;
    for (int k = 0; k < nspr; k++) {
        int32 base    = 13 + k * 6;
        int32 cx      = in[base + 0];
        int32 half_w  = in[base + 1];
        int32 sp_top  = in[base + 2];
        int32 sp_bot  = in[base + 3];
        int32 depth   = in[base + 4];   // q12
        uint32 col    = (uint32)in[base + 5];

        if (depth >= perp_q12) continue;            // behind the wall
        if (half_w < 1) continue;
        if (screen_col < cx - half_w || screen_col >= cx + half_w) continue;

        // Evaluate the orb directly in screen space (full resolution).  The GPU
        // uses a 64-texel circle of radius 30, i.e. radius 30/32 of the half
        // width, so R^2 = half_w^2 * SPR_R2 / 1024 (and likewise the core).
        int64_t hw2 = (int64_t)half_w * half_w;
        int64_t R2  = hw2 * SPR_R2   / 1024;
        int64_t Rc2 = hw2 * SPR_CORE / 1024;
        int32 cyr = (sp_top + sp_bot) / 2;          // sprite centre row
        int32 dx  = screen_col - cx;
        int64_t dx2 = (int64_t)dx * dx;
        if (dx2 >= R2) continue;                     // outside orb horizontally

        int32 r0 = sp_top > row_start ? sp_top : row_start;
        int32 r1 = sp_bot < row_start + 256 ? sp_bot : row_start + 256;
        for (int32 r = r0; r < r1; r++) {
            int32 dy = r - cyr;
            int64_t dist2 = dx2 + (int64_t)dy * dy;
            if (dist2 >= R2) continue;
            uint32 pr = (col >> 16) & 0xFF;
            uint32 pg = (col >> 8) & 0xFF;
            uint32 pb = col & 0xFF;
            if (dist2 < Rc2) {                       // bright core highlight
                pr = pr + ((255 - pr) >> 1);
                pg = pg + ((255 - pg) >> 1);
                pb = pb + ((255 - pb) >> 1);
            }
            tile[r - row_start] = (pr << 24) | (pg << 16) | (pb << 8) | 0xFFu;
        }
    }
}

} // extern "C"
