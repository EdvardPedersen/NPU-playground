#include <stdint.h>
#include <stdlib.h>
#include <aie_api/aie.hpp>

extern "C" {

#define VEC_WIDTH 32

void passThroughLine(int32_t *in, int32_t *out, int32_t lineWidth, int32_t node, uint64_t split, int32_t nodeWidth, int32_t image_width, int32_t image_height, float stage) {
    int32_t *inPtr = in;
    uint32_t *outPtr = (uint32_t *)out;
    uint32_t start = (uint16)lineWidth * (uint16)nodeWidth * (uint16)node + ((uint16)lineWidth * (uint16)split);

    for(uint16 i = 0; i < lineWidth; i+=VEC_WIDTH) {
        aie::vector<uint32, VEC_WIDTH> starts;
        aie::vector<uint32, VEC_WIDTH> mods;
        aie::vector<uint32, VEC_WIDTH> divs;
        for(int x = 0; x < VEC_WIDTH; x++) {
            starts[x] = (i + start + x); 
            mods[x] =  starts[x] % image_width;
            divs[x] = starts[x] / image_width;
        }

        divs = aie::downshift(starts, 10);
        uint32 test = 0x3ff;
        mods = aie::bit_and(test, starts);
        auto xes =  aie::to_float(mods, 0);
        auto real_xes = aie::div(xes, (float)image_width);

        auto yes = aie::to_float(divs, 0);
        auto real_yes = aie::div(yes, (float)image_height);

        aie::vector<bfloat16, VEC_WIDTH> yesyes;
        yesyes = aie::to_vector<bfloat16>(real_yes);

        aie::vector<bfloat16, VEC_WIDTH> y0s = aie::mul(real_yes, 2.24);
        aie::vector<bfloat16, VEC_WIDTH> x0s = aie::mul(real_xes, 2.47);

        //float y0 = y * 2.24 * stage - 1.12 * stage;
        //float x0 = x * 2.47 * stage - 2 * stage;

        aie::vector<uint16, VEC_WIDTH> iters = 0;

        bfloat16 xf = 0;
        bfloat16 yf = 0;
        uint8_t iter = 0;
        while(iter < 255 and xf * xf + yf * yf < 4) {
            bfloat16 xtemp = xf*xf - yf * yf + sx0;
            yf = 2 * xf * yf + sy0;
            xf = xtemp;
            iter += 1;
        }
        //*outPtr++ = iter << 8 | 0xff;

        aie::store_v(outPtr, mods);
        outPtr += VEC_WIDTH;
    }
}

} // extern "C"
