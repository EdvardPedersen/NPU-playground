// Parameters shared by the NPU kernel (kernel.cpp) and the GPU shader (perlin.comp).
// Lattice coordinates are exact Q12 integers on every generator, so they only
// differ in the precision of the interpolation.
//
// The output is fBm: octave 0 is 2D Perlin noise at (t + x s, t + y s), and each
// further octave uses perlin_next_octave(t, s) at half the amplitude.
#pragma once
#include <cmath>
#include <cstdint>

constexpr int PERLIN_W = 4096;
constexpr int PERLIN_H = 4096;
constexpr int PERLIN_PARAMS = 16;   // NPU input ints per row (kernel call), see design.py
constexpr int PERLIN_NPU_IN = PERLIN_PARAMS * PERLIN_H;

struct PerlinParams {
    float time = 0.0f;   // lattice offset in cells, added to x and y
    float scale = 0.25f; // lattice cells per pixel
    float height = 1.0f; // contrast
    int octaves = 1;     // detail
};

constexpr int32_t PERLIN_OCTAVE_SHIFT = 0x9E37; // decorrelates the octaves' origins

// Doubles the frequency, modulo the 256-cell period of the lattice. The +1 keeps
// s from collapsing to 0 once it has wrapped.
inline void perlin_next_octave(int32_t &t, int32_t &s) {
    t = (2 * t + PERLIN_OCTAVE_SHIFT) & 0xFFFFF;
    s = (2 * s + 1) & 0xFFFFF;
}

// Also the GPU push constants
struct PerlinQ12 {
    int32_t t, s, h, octaves;
};

inline PerlinQ12 perlin_q12(const PerlinParams &p) {
    // The hash repeats every 256 cells, so t is kept in [0, 256) cells.
    return {(int32_t)std::lround(p.time * 4096.0) & 0xFFFFF,
            (int32_t)std::lround(p.scale * 4096.0f),
            (int32_t)std::lround(p.height * 4096.0f),
            p.octaves};
}

// NPU input: PERLIN_PARAMS ints per row (one kernel call each), of which the first
// five are {t, s, y, height, octaves}.
inline void perlin_fill_npu_input(int32_t *in, const PerlinParams &p) {
    PerlinQ12 q = perlin_q12(p);
    for (int y = 0; y < PERLIN_H; y++) {
        int32_t *row = in + y * PERLIN_PARAMS;
        row[0] = q.t;
        row[1] = q.s;
        row[2] = y;
        row[3] = q.h;
        row[4] = q.octaves;
    }
}
