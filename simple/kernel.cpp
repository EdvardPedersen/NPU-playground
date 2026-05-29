#include <stdint.h>
#include <stdlib.h>
#include <aie_api/aie.hpp>

extern "C" {

#define VEC_WIDTH 32
#define UINT_TYPE uint16

alignas(128) static constexpr int16 kIotaI16[VEC_WIDTH] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
};

void passThroughLine(int32_t * __restrict in, int32_t * __restrict out, int32_t lineWidth, int32_t node, uint64_t split, int32_t nodeWidth, int32_t image_width, int32_t image_height, float stage) {
    int32_t *outPtr = out;
    uint32_t start = lineWidth * nodeWidth * node + (lineWidth * split);

    for(UINT_TYPE i = 0; i < lineWidth; i += VEC_WIDTH) {
        uint32_t base_s = i + start;
        uint32_t r = base_s % image_width;

        int16 x0_start = (int16)((int32)r * 9175 / image_width - 8192); // Fixed-point x * 4096
        int16 dx = (int16)(9175 / image_width);

        auto iota_raw = aie::load_v<VEC_WIDTH>(kIotaI16);
        auto dx_bcst = aie::broadcast<int16, VEC_WIDTH>(dx);
        auto x0_start_bcst = aie::broadcast<int16, VEC_WIDTH>(x0_start);

        auto x0s = aie::add(x0_start_bcst, aie::to_vector<int16>(aie::mul(dx_bcst, iota_raw), 0));

        uint32 p0 = i + start;
        uint32 r0 = p0 / image_width;
        uint32 cross = (r0 + 1) * image_width - p0;

        int16 y0_r0 = (int16)((int32)r0 * 10117 / image_height - 4588);
        int16 y0_r1 = (int16)((int32)(r0 + 1) * 10117 / image_height - 4588);

        auto y0s = aie::broadcast<int16, VEC_WIDTH>(y0_r0);
        if (cross < VEC_WIDTH) {
            auto mask = aie::ge(aie::load_v<VEC_WIDTH>(kIotaI16),
                                aie::broadcast<int16, VEC_WIDTH>((int16)cross));
            y0s = aie::select(y0s, aie::broadcast<int16, VEC_WIDTH>(y0_r1), mask);
        }

        aie::vector<UINT_TYPE, VEC_WIDTH> iters = aie::zeros<UINT_TYPE, VEC_WIDTH>();
        aie::vector<int16, VEC_WIDTH> xf = aie::zeros<int16, VEC_WIDTH>();
        aie::vector<int16, VEC_WIDTH> yf = aie::zeros<int16, VEC_WIDTH>();

        for(UINT_TYPE x = 0; x < 255; x++) {
            auto xsq = aie::to_vector<int16>(aie::mul(xf, xf), 12);
            auto ysq = aie::to_vector<int16>(aie::mul(yf, yf), 12);

            yf = aie::add(yf, yf);
            yf = aie::to_vector<int16>(aie::mul(xf, yf), 12);

            xf = aie::sub(xsq, ysq);
            yf = aie::add(yf, y0s);
            xf = aie::add(xf, x0s);

            auto added = aie::add(xsq, ysq);
            iters = aie::select(iters, x, aie::gt(added, (int16)12288));
        }

        iters = aie::upshift(iters, 8);
        aie::vector<uint32, VEC_WIDTH> output = iters.unpack<uint32>();
        output = aie::bit_or((uint32)0xff, output);
        aie::store_v((uint32 *)outPtr, output);
        outPtr += VEC_WIDTH;
    }
}

} // extern "C"
