#include <stdint.h>
#include <stdlib.h>
#include <aie_api/aie.hpp>

extern "C" {

#define VEC_WIDTH 16
#define FLOAT_TYPE float

void passThroughLine(int32_t * __restrict in, int32_t * __restrict out, int32_t lineWidth, int32_t node, uint64_t split, int32_t nodeWidth, int32_t image_width, int32_t image_height, float stage) {
    int32_t *inPtr = in;
    int32_t *outPtr = out;
    uint32_t start = lineWidth * nodeWidth * node + (lineWidth * split);

    for(uint16 i = 0; i < lineWidth; i += VEC_WIDTH) chess_prepare_for_pipelining chess_loop_range(1024/16, 1024/16) {
        aie::vector<FLOAT_TYPE, VEC_WIDTH> x0s;
        aie::vector<FLOAT_TYPE, VEC_WIDTH> y0s;
        for(int x = 0; x < VEC_WIDTH; x++) {
            uint32 s = (i + start + x);
            x0s[x] = (FLOAT_TYPE)((((s % image_width) / (FLOAT_TYPE)image_width) * 2.24) - (2));
            y0s[x] = (FLOAT_TYPE)((((s / image_width) / (FLOAT_TYPE)image_height) * 2.47) - (1.12));
        }

        aie::vector<uint32, VEC_WIDTH> iters = aie::zeros<uint32, VEC_WIDTH>();
        aie::vector<FLOAT_TYPE, VEC_WIDTH> xf = aie::zeros<FLOAT_TYPE, VEC_WIDTH>();
        aie::vector<FLOAT_TYPE, VEC_WIDTH> yf = aie::zeros<FLOAT_TYPE, VEC_WIDTH>();
        for(uint16 x = 0; x < 63; x++) chess_prepare_for_pipelining chess_loop_range(1, 255) {
            auto xsq = aie::to_vector<FLOAT_TYPE>(aie::mul<accfloat>(xf, xf)); // Uncomment this = corrupt image, infinite loop if uncommenting other xsq use
            //auto ysq = aie::to_vector<FLOAT_TYPE>(aie::mul<accfloat>(yf, yf)); // Uncomment this = corrupt image, infinite loop if uncommenting other xsq use
            aie::vector<FLOAT_TYPE, VEC_WIDTH> ysq = aie::zeros<FLOAT_TYPE, VEC_WIDTH>();
            for(int y = 0; y < VEC_WIDTH; y++) {
                //xsq[y] = xf[y] * xf[y];
                //ysq[y] = yf[y] * yf[y];
                yf[y] = xf[y] * 2 * yf[y] + y0s[y];
                xf[y] = xsq[y] - ysq[y] + x0s[y];
                if(xsq[y] + ysq[y] < 4.0) iters[y] = iters[y] + 1;
            }
        }

        iters = aie::upshift(iters, 10);
        iters = aie::bit_or((uint32)0xff, iters);
        aie::store_v((uint32 *)outPtr, iters);
        outPtr += VEC_WIDTH;
}
}

} // extern "C"
