#include <stdint.h>
#include <aie_api/aie.hpp>

// fBm of 2D Perlin noise, one image row per call. Lattice cells and hashes are exact
// integers; gradients, fade and interpolation use bf16 multiplies accumulated
// in fp32. Inputs and output format: see perlin_params.h.

constexpr unsigned VEC = 32;
using vi16 = aie::vector<int16, VEC>;
using vbf = aie::vector<bfloat16, VEC>;
using accf = aie::accum<accfloat, VEC>;

alignas(64) static constexpr int16 kIota[VEC] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
};

static inline vi16 bc(int v) { return aie::broadcast<int16, VEC>((int16)v); }
static inline vbf bcf(float v) { return aie::broadcast<bfloat16, VEC>((bfloat16)v); }
static inline vbf q12_to_bf16(vi16 v) { return aie::to_float<bfloat16>(v, 12); }
static inline vbf bf16(accf a) { return a.to_vector<bfloat16>(); }
static inline accf acc_of(vbf v) { accf a; a.from_vector(v); return a; }

// hx = 13 * x cell, hy = 7 * y cell
static inline vi16 hash(vi16 hx, int hy) {
    auto v = aie::add(hx, bc(hy));
    auto a = aie::bit_and(v, bc(255)), b = aie::downshift(v, 8);
    return aie::sub(aie::bit_or(a, b), aie::bit_and(a, b)); // a ^ b, as vector xor is slow
}

// Improved-Perlin gradient for h & 15 dotted with (x, y), not halved. Works on
// the bf16 bit patterns, so negation is flipping the sign bit (adding 0x8000).
static inline accf grad(vi16 h, vi16 x, vi16 y) {
    auto u = aie::select(y, x, aie::lt(aie::bit_and(h, bc(15)), bc(8)));
    auto x_or_0 = aie::select(bc(0), x, aie::eq(aie::bit_and(h, bc(13)), bc(12))); // h == 12 || h == 14
    auto v = aie::select(x_or_0, y, aie::lt(aie::bit_and(h, bc(15)), bc(4)));
    u = aie::add(u, aie::upshift(h, 15));
    v = aie::add(v, aie::upshift(aie::bit_and(h, bc(2)), 14));
    const auto one = bcf(1.0f);
    return aie::mac(aie::mul(u.cast_to<bfloat16>(), one), v.cast_to<bfloat16>(), one);
}

// a + t (b - a), with a also given as accumulator
static inline accf lerp(accf a, accf b, vbf t) {
    return aie::mac(aie::msc(a, t, bf16(a)), t, bf16(b));
}

// 3t^2 - 2t^3
static inline vbf fade(vbf t) {
    return bf16(aie::mul(bf16(aie::mul(t, t)), bf16(aie::msc(acc_of(bcf(3.0f)), bcf(2.0f), t))));
}

extern "C" {

// The octave sum is accumulated in out as fp32, then packed into pixels.
void passThroughLine(int32_t * __restrict in, int32_t * __restrict out, int32_t lineWidth) {
    int32_t t = in[0], s = in[1];
    const int32_t y = in[2], octaves = in[4];
    float *sum = reinterpret_cast<float *>(out);
    const auto iota = aie::load_v<VEC>(kIota);

    for (int32_t i = 0; i < lineWidth; i += VEC)
        aie::store_v(sum + i, aie::zeros<float, VEC>());

    for (int32_t o = 0; o < octaves; o++) {
        // t + x s wraps past 2^32 at 4096 pixels; only the low 20 bits matter
        const uint32_t fy = (uint32_t)t + (uint32_t)y * (uint32_t)s;
        const int32_t yi = (fy >> 12) & 255, yf = fy & 4095;
        const int hy0 = yi * 7, hy1 = ((yi + 1) & 255) * 7;
        const auto y0f = q12_to_bf16(bc(yf));
        const auto y0 = y0f.cast_to<int16>(), y1 = q12_to_bf16(bc(yf - 4096)).cast_to<int16>();
        const auto v = fade(y0f);
        // 2^-o as a bf16 bit pattern; octaves beyond the exponent range vanish
        const auto amp = bc(o < 127 ? (127 - o) << 7 : 0).cast_to<bfloat16>();

        // Per-lane x offset iota * s, split into cell and Q12 fraction. s can be up
        // to 256 cells, so its cell and fraction parts are multiplied separately.
        const auto lo = aie::mul(iota, bc(s & 4095));
        const auto lo_cell = aie::to_vector<int16>(lo, 12);
        const auto lane_frac = aie::to_vector<int16>(aie::msc(lo, lo_cell, bc(4096)), 0);
        const auto lane_cell = aie::add(lo_cell, aie::to_vector<int16>(aie::mul(iota, bc(s >> 12)), 0));

        for (int32_t i = 0; i < lineWidth; i += VEC) {
            uint32_t bx = (uint32_t)t + (uint32_t)i * (uint32_t)s;
            auto frac = aie::add(bc(bx & 4095), lane_frac);
            auto xi = aie::bit_and(aie::add(aie::add(bc((bx >> 12) & 255), lane_cell), aie::downshift(frac, 12)), bc(255));
            auto xq = aie::bit_and(frac, bc(4095));
            auto x0f = q12_to_bf16(xq);
            auto x0 = x0f.cast_to<int16>(), x1 = q12_to_bf16(aie::sub(xq, bc(4096))).cast_to<int16>();
            auto hx0 = aie::to_vector<int16>(aie::mul(xi, bc(13)), 0);
            auto hx1 = aie::to_vector<int16>(aie::mul(aie::bit_and(aie::add(xi, bc(1)), bc(255)), bc(13)), 0);

            auto u = fade(x0f);
            auto n0 = lerp(grad(hash(hx0, hy0), x0, y0), grad(hash(hx1, hy0), x1, y0), u);
            auto n1 = lerp(grad(hash(hx0, hy1), x0, y1), grad(hash(hx1, hy1), x1, y1), u);
            auto n = lerp(n0, n1, v);

            accf total;
            total.from_vector(aie::load_v<VEC>(sum + i));
            aie::store_v(sum + i, aie::mac(total, bf16(n), amp).to_vector<float>());
        }
        t = (2 * t + 0x9E37) & 0xFFFFF; // perlin_next_octave
        s = (2 * s + 1) & 0xFFFFF;
    }

    // c = 256 (h n + 1/2) = 128 h sum + 128, as grad is not halved
    const auto h128 = aie::to_float<bfloat16>(aie::broadcast<int32, VEC>(in[3]), 5);
    const auto half = acc_of(bcf(128.0f));
    for (int32_t i = 0; i < lineWidth; i += VEC) {
        accf total;
        total.from_vector(aie::load_v<VEC>(sum + i));
        auto c = aie::to_fixed<int32>(aie::mac(half, bf16(total), h128).to_vector<float>(), 0);
        c = aie::max(aie::min(c, 255), 0);
        auto px = aie::bit_or(aie::bit_or(c, aie::upshift(c, 8)), aie::upshift(c, 16));
        aie::store_v(out + i, aie::bit_or((int32)0xFF000000, px));
    }
}

} // extern "C"
