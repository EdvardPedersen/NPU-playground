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

        auto y0s = aie::mul(yesyes, (bfloat16)2.24);
        y0s = aie::mul(aie::to_vector<bfloat16>(y0s), (bfloat16)stage);
        bfloat16 temp = 1.12*stage;
        y0s = aie::sub(y0s, temp);

        auto x0s = aie::mul(aie::to_vector<bfloat16>(real_xes), (bfloat16)2.47);
        x0s = aie::mul(aie::to_vector<bfloat16>(x0s), (bfloat16)stage);
        temp = 2*stage;
        x0s = aie::sub(x0s, temp);

        aie::vector<uint16, VEC_WIDTH> iters = aie::zeros<uint16>();
        aie::vector<bfloat16, VEC_WIDTH> xfs = aie::broadcast<bfloat16>(0);
        aie::vector<bfloat16, VEC_WIDTH> yfs = aie::broadcast<bfloat16>(0);

        for(int i = 0; i < 255; i++) {
            auto xftemp = aie::mul(xfs, xfs);
            auto yftemp = aie::mul(yfs, yfs);
            auto together = aie::add(xftemp, yftemp);

            auto xtemp = aie::sub(xftemp, yftemp);
            xtemp = aie::add(xtemp, x0s);

            auto yf = aie::mul(xfs, (bfloat16)2);
            yf = aie::mul(aie::to_vector<bfloat16>(yf), yfs);
            yf = aie::add(yf, x0s);

            auto mask = aie::le(aie::to_vector<bfloat16>(together), (bfloat16)4.0); 
            auto next_iter = aie::add(iters, (uint16)1);
            iters = aie::select(next_iter, iters, mask);
            

        }
/*
        while(iter < 255 and xf * xf + yf * yf < 4) {
            bfloat16 xtemp = xf*xf - yf * yf + sx0;
            yf = 2 * xf * yf + sy0;
            xf = xtemp;
            iter += 1;
        }*/
        //*outPtr++ = iter << 8 | 0xff;

        for(int i = 0; i < VEC_WIDTH; i++) {
            *outPtr++ = iters[i] << 8 | 0xff;
        }
        //aie::store_v(outPtr, iters);
        //outPtr += VEC_WIDTH;
    }
}

} // extern "C"
