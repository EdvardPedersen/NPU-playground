#include <stdint.h>
#include <stdlib.h>
#include <aie_api/aie.hpp>

extern "C" {

#define VEC_WIDTH 16

void passThroughLine(int32_t *in, int32_t *out, int32_t lineWidth, int32_t node, uint64_t split, int32_t nodeWidth, int32_t image_width, int32_t image_height, float stage) {
    int32_t *inPtr = in;
    int32_t *outPtr = out;
    uint32_t start = lineWidth * nodeWidth * node + (lineWidth * split);

    for(uint16 i = 0; i < lineWidth; i += VEC_WIDTH) {
        aie::vector<uint32, VEC_WIDTH> starts;
        aie::vector<bfloat16, VEC_WIDTH> real_xes;
        aie::vector<bfloat16, VEC_WIDTH> real_yes;
        for(int x = 0; x < VEC_WIDTH; x++) {
            starts[x] = (i + start + x);
            real_xes[x] = (bfloat16)((starts[x] % image_width) / image_width);
            real_yes[x] = (bfloat16)((starts[x] / image_width) / image_height);
        }

        /*auto y0s = aie::mul(real_yes, (bfloat16)2.24);
        y0s = aie::mul(aie::to_vector<bfloat16>(y0s), (bfloat16)stage);
        y0s = aie::sub(y0s, (bfloat16)1.12*stage);
        aie::vector<bfloat16, VEC_WIDTH> y0v = aie::to_vector<bfloat16>(y0s);

        aie::vector<bfloat16, VEC_WIDTH> x0v = aie::to_vector<bfloat16>(aie::sub(aie::mul(real_xes, (bfloat16)2.47), (bfloat16)2));
*/
        /*aie::vector<uint16, VEC_WIDTH> iters = aie::zeros<uint16, VEC_WIDTH>();
        aie::vector<bfloat16, VEC_WIDTH> xfs = aie::zeros<bfloat16, VEC_WIDTH>();
        aie::vector<bfloat16, VEC_WIDTH> yfs = aie::zeros<bfloat16, VEC_WIDTH>();

        for(uint16 x = 0; x < 5; x++) {
            aie::vector<bfloat16, VEC_WIDTH> xftemp = aie::to_vector<bfloat16>(aie::mul(xfs, xfs));
            aie::vector<bfloat16, VEC_WIDTH> yftemp = aie::to_vector<bfloat16>(aie::mul(yfs, yfs));
            aie::vector<bfloat16, VEC_WIDTH> together = aie::add(xftemp, yftemp);

            aie::vector<bfloat16, VEC_WIDTH> xtemp = aie::sub(xftemp, yftemp);
            xtemp = aie::add(xtemp, x0v);
            aie::vector<bfloat16, VEC_WIDTH> yf = aie::to_vector<bfloat16>(aie::mul(xfs, (bfloat16)2));
            yf = aie::to_vector<bfloat16>(aie::mul(yf, yfs));
            yf = aie::add(yf, y0v);
            yfs = yf;
            xfs = xtemp;
            auto mask = aie::le(together, (bfloat16)4.0); 
            aie::vector<uint16, VEC_WIDTH> next_iter = aie::add(iters, (uint16)1);
            //iters = aie::select(iters, next_iter, mask);
            iters = aie::to_fixed<uint16>(together);
            //iters = next_iter;
        }*/
/*
        while(iter < 255 and xf * xf + yf * yf < 4) {
            bfloat16 xtemp = xf*xf - yf * yf + sx0;
            yf = 2 * xf * yf + sy0;
            xf = xtemp;
            iter += 1;
        }
*/
        //auto x_values = aie::to_vector<bfloat16>(aie::mul(real_yes, (bfloat16) 254));
        //auto x_values = aie::to_vector<bfloat16>(aie::mul(aie::add(x0v,(bfloat16) 2.0), (bfloat16)64.0));
        //auto y_values = aie::to_vector<bfloat16>(aie::mul(aie::to_vector<bfloat16>(real_yes), (bfloat16)255.0));

        for(uint16 x = 0; x < VEC_WIDTH; x++) {
            //uint16 lol = starts[x] % 255;
            *outPtr++ =  (uint32_t)(starts[x] % 255) << 16  | 0xff;
        }
        aie::vector<uint32, VEC_WIDTH> test = starts.cast_to<uint32>();
        test = aie::bit_and((uint32)0xff, test);
        //test = aie::upshift(test, 0);
        test = aie::bit_or((uint32)0xff, test);
        aie::store_v((uint32 *)outPtr, test);
        //outPtr += 1;
 }
}

} // extern "C"
