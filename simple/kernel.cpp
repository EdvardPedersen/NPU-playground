#include <stdint.h>
#include <stdlib.h>
#include <aie_api/aie.hpp>

extern "C" {

#define VEC_WIDTH 16
#define FLOAT_TYPE bfloat16
#define UINT_TYPE uint32

void passThroughLine(int32_t * __restrict in, int32_t * __restrict out, int32_t lineWidth, int32_t node, uint64_t split, int32_t nodeWidth, int32_t image_width, int32_t image_height, float stage) {
    int32_t *outPtr = out;
    uint32_t start = lineWidth * nodeWidth * node + (lineWidth * split);

    for(UINT_TYPE i = 0; i < lineWidth; i += VEC_WIDTH) {
        aie::vector<FLOAT_TYPE, VEC_WIDTH> x0s;
        aie::vector<FLOAT_TYPE, VEC_WIDTH> y0s;
        for(int x = 0; x < VEC_WIDTH; x++) {
            uint32 s = (i + start + x);
            x0s[x] = (FLOAT_TYPE)((((s % image_width) / (FLOAT_TYPE)image_width) * (FLOAT_TYPE)2.24) - ((FLOAT_TYPE)2));
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
            //iters = aie::select(iters, x, aie::lt(aie::add(xsq, ysq), (bfloat16)4.0));
            for(int y = 0; y < VEC_WIDTH; y++) {
                if(xsq[y] + ysq[y] < 4.0) iters[y] = x;
            }
        }

        iters = aie::upshift(iters, 8);
        iters = aie::bit_or((UINT_TYPE)0xff, iters);
        aie::store_v((uint32 *)outPtr, iters);
        outPtr += VEC_WIDTH;
}
}

} // extern "C"
