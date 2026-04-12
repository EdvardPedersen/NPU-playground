#include <stdint.h>
#include <stdlib.h>

extern "C" {

void passThroughLine(int32_t *in, int32_t *out, int32_t lineWidth, int32_t node, uint64_t split, int32_t nodeWidth, int32_t image_width, int32_t image_height, float stage) {
    int32_t *inPtr = in;
    int32_t *outPtr = out;
    int32_t start = lineWidth * nodeWidth * node + (lineWidth * split);

    for(int i = 0; i < lineWidth; i++) {
        int32_t total = i + start;
        float x = (total % image_width) / (float)image_width;
        float y = (total / image_width) / (float)image_height;
        float y0 = y * 2.24 * stage - 1.12 * stage;
        float x0 = x * 2.47 * stage - 2 * stage;

        float xf = 0;
        float yf = 0;
        uint8_t iter = 0;
        while(iter < 255 and xf * xf + yf * yf < 4) {
            float xtemp = xf*xf - yf * yf + x0;
            yf = 2 * xf * yf + y0;
            xf = xtemp;
            iter += 1;
        }
        *outPtr++ = iter;
    }
}

} // extern "C"
