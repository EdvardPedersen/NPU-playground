#include <stdint.h>
#include <stdlib.h>
#include <aie_api/aie.hpp>

extern "C" {

#define VEC_WIDTH 32
#define FLOAT_TYPE bfloat16
#define UINT_TYPE uint16

alignas(128) static constexpr uint32_t kIotaU32[VEC_WIDTH] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
};
alignas(64) static bfloat16 kIotaBf16[VEC_WIDTH];
static bool kIotaInit = false;

void passThroughLine(int32_t * __restrict in, int32_t * __restrict out, int32_t lineWidth, int32_t node, uint64_t split, int32_t nodeWidth, int32_t image_width, int32_t image_height, float stage) {
    if (!kIotaInit) {
        for (int x = 0; x < VEC_WIDTH; x++)
            kIotaBf16[x] = (bfloat16)(float)x;
        kIotaInit = true;
    }

    int32_t *outPtr = out;
    uint32_t start = lineWidth * nodeWidth * node + (lineWidth * split);

    for(UINT_TYPE i = 0; i < lineWidth; i += VEC_WIDTH) {
        uint32_t base_s = i + start;
        uint32_t r = base_s % image_width;
        uint32_t k = base_s / image_width;

        FLOAT_TYPE x0_start = (FLOAT_TYPE)(((FLOAT_TYPE)r / (FLOAT_TYPE)image_width) * (FLOAT_TYPE)2.24 - (FLOAT_TYPE)2.0);
        FLOAT_TYPE dx = (FLOAT_TYPE)2.24 / (FLOAT_TYPE)image_width;
        FLOAT_TYPE y0_val = (FLOAT_TYPE)(((FLOAT_TYPE)k / (FLOAT_TYPE)image_height) * (FLOAT_TYPE)2.47 - (FLOAT_TYPE)1.12);

        auto offsets = aie::load_v<VEC_WIDTH>(kIotaBf16);
        auto dx_bcst = aie::broadcast<bfloat16, VEC_WIDTH>(dx);
        auto x0_start_bcst = aie::broadcast<bfloat16, VEC_WIDTH>(x0_start);

        auto x0s = aie::add(x0_start_bcst, aie::to_vector<bfloat16>(aie::mul<accfloat>(dx_bcst, offsets)));
        aie::vector<FLOAT_TYPE, VEC_WIDTH> y0s;
        for(UINT_TYPE x = 0; x < VEC_WIDTH; x++) {
            uint32 s = (i + start + x);
            y0s[x] = (FLOAT_TYPE)((((s / image_width) / (FLOAT_TYPE)image_height) * (FLOAT_TYPE)2.47) - ((FLOAT_TYPE)1.12));
        }

        aie::vector<UINT_TYPE, VEC_WIDTH> iters = aie::zeros<UINT_TYPE, VEC_WIDTH>();
        aie::vector<FLOAT_TYPE, VEC_WIDTH> xf = aie::zeros<FLOAT_TYPE, VEC_WIDTH>();
        aie::vector<FLOAT_TYPE, VEC_WIDTH> yf = aie::zeros<FLOAT_TYPE, VEC_WIDTH>();
        for(UINT_TYPE x = 0; x < 255; x++) {
            auto xsq = aie::to_vector<FLOAT_TYPE>(aie::mul<accfloat>(xf, xf)); 
            auto ysq = aie::to_vector<FLOAT_TYPE>(aie::mul<accfloat>(yf, yf)); 
            yf = aie::to_vector<FLOAT_TYPE>(aie::mul<accfloat>(yf, (FLOAT_TYPE)2));
            yf = aie::to_vector<FLOAT_TYPE>(aie::mul<accfloat>(xf, yf));
            xf = aie::sub(xsq, ysq);
            yf = aie::add(yf, y0s);
            xf = aie::add(xf, x0s);
            auto added = aie::add(xsq, ysq);
            auto added_int = aie::vector_cast<UINT_TYPE>(added);
            iters = aie::select(iters, (UINT_TYPE)x, aie::gt(added_int, (UINT_TYPE)0x4040));
        }

        iters = aie::upshift(iters, 8);
        iters = aie::bit_or((UINT_TYPE)0xff, iters);
        aie::store_v((uint32 *)outPtr, iters.unpack<uint32>());
        outPtr += VEC_WIDTH;
    }
}

} // extern "C"
