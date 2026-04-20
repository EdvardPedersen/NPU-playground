#include <stdint.h>
#include <stdlib.h>
#include <aie_api/aie.hpp>

extern "C" {

#define VEC_WIDTH 16
#define FLOAT_TYPE float

void passThroughLine(int32_t *in, int32_t *out, int32_t lineWidth, int32_t node, uint64_t split, int32_t nodeWidth, int32_t image_width, int32_t image_height, float stage) {
    int32_t *inPtr = in;
    int32_t *outPtr = out;
    uint32_t start = lineWidth * nodeWidth * node + (lineWidth * split);

    for(uint16 i = 0; i < lineWidth; i += VEC_WIDTH) {
        aie::vector<uint32, VEC_WIDTH> starts;
        FLOAT_TYPE real_xes[VEC_WIDTH];
        FLOAT_TYPE real_yes[VEC_WIDTH];
        for(int x = 0; x < VEC_WIDTH; x++) {
            starts[x] = (i + start + x);
            real_xes[x] = (FLOAT_TYPE)((starts[x] % image_width) / (FLOAT_TYPE)image_width);
            real_yes[x] = (FLOAT_TYPE)((starts[x] / image_width) / (FLOAT_TYPE)image_height);
        }

        FLOAT_TYPE y0s[VEC_WIDTH] = {0};
        FLOAT_TYPE x0s[VEC_WIDTH] = {0};
        for(int x = 0; x < VEC_WIDTH; x++) {
            y0s[x] = (real_yes[x] * 2.24) - (1.12);
            x0s[x] = (real_xes[x] * 2.47) - (2);
        }


        uint16 iters[VEC_WIDTH] = {0};
        FLOAT_TYPE xf[VEC_WIDTH] = {0};
        FLOAT_TYPE yf[VEC_WIDTH] = {0};
        for(uint16 x = 0; x < 255; x++) {
            for(int y = 0; y < VEC_WIDTH; y++) {
                FLOAT_TYPE xftemp = xf[y]*xf[y] - yf[y]*yf[y] + x0s[y];
                yf[y] = xf[y] * 2 * yf[y] + y0s[y];
                xf[y] = xftemp;
                FLOAT_TYPE xsq = xf[y] * xf[y];
                FLOAT_TYPE ysq = yf[y] * yf[y];
                FLOAT_TYPE sumsq = xsq + ysq;
                if(sumsq < 4.0) iters[y] += 1;
            }
        }
        for(uint16 x = 0; x < VEC_WIDTH; x++) {
            //uint16 lol = starts[x] % 255;
            *outPtr++ =  (uint32_t)(iters[x] << 8)  | 0xff;
        }
}
}

} // extern "C"
