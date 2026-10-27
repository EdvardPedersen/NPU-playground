#include <stdint.h>
#include <bit>
#include <aie_api/aie.hpp>

constexpr unsigned VEC_WIDTH = 32;
using iter_t = uint16;

alignas(128) static constexpr int32 kIota[VEC_WIDTH] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
};

static inline aie::vector<bfloat16, VEC_WIDTH> to_bf16(aie::vector<float, VEC_WIDTH> v) {
    aie::accum<accfloat, VEC_WIDTH> acc;
    acc.from_vector(v);
    return acc.to_vector<bfloat16>();
}

static inline aie::vector<bfloat16, VEC_WIDTH> bcst_bf16(int32_t bits) {
    return to_bf16(aie::broadcast<float, VEC_WIDTH>(std::bit_cast<float>(bits)));
}

extern "C" {

// in[0..2] = float32 bits of x_start, y and dx for this tile, which lies within one row.
// AIE2P has no scalar FPU and soft-float libcalls are not linked, so scalar
// floats are only bit_cast and moved; all float arithmetic happens in vectors.
void passThroughLine(int32_t * __restrict in, int32_t * __restrict out, int32_t lineWidth) {
    const auto x_start = aie::broadcast<float, VEC_WIDTH>(std::bit_cast<float>(in[0]));
    aie::accum<accfloat, VEC_WIDTH> y0s;
    y0s.from_vector(aie::broadcast<float, VEC_WIDTH>(std::bit_cast<float>(in[1])));
    const auto dx = bcst_bf16(in[2]);
    const auto iota = aie::load_v<VEC_WIDTH>(kIota);
    const auto threshold = aie::broadcast<bfloat16, VEC_WIDTH>((bfloat16)3.0f);

    for (int32_t i = 0; i < lineWidth; i += VEC_WIDTH) {
        auto cols = to_bf16(aie::to_float(aie::add(iota, (int32)i)));
        aie::accum<accfloat, VEC_WIDTH> x0s;
        x0s.from_vector(x_start);
        x0s = aie::mac(x0s, cols, dx);

        auto iters = aie::zeros<iter_t, VEC_WIDTH>();
        auto xf = aie::zeros<bfloat16, VEC_WIDTH>();
        auto yf = aie::zeros<bfloat16, VEC_WIDTH>();

        // Only mac/msc into fp32 accumulators: bf16 add/sub would each cost an
        // fp32 round trip, so z is rounded to bf16 only as multiplier input.
        for (iter_t x = 0; x < 255; x++) {
            auto mag = aie::mac(aie::mul(xf, xf), yf, yf).to_vector<bfloat16>();
            auto nx = aie::msc(aie::mac(x0s, xf, xf), yf, yf);
            auto ny = aie::mac(aie::mac(y0s, xf, yf), xf, yf);
            xf = nx.to_vector<bfloat16>();
            yf = ny.to_vector<bfloat16>();

            iters = aie::select(iters, x, aie::gt(mag, threshold));
        }

        auto output = aie::upshift(iters, 8).unpack<uint32>();
        aie::store_v((uint32 *)out + i, aie::bit_or((uint32)0xff, output));
    }
}

} // extern "C"
