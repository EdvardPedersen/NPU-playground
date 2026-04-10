#include <stdint.h>
#include <stdlib.h>

extern "C" {

void passThroughLine(int32_t *in, int32_t *out, int32_t lineWidth, int32_t node) {
    int32_t *inPtr = in;
    int32_t *outPtr = out;
    for(int i = 0; i < lineWidth; i++) {
        *outPtr++ = (lineWidth * node) + i;
    }
}

} // extern "C"
