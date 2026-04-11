#include <stdint.h>
#include <stdlib.h>

extern "C" {

void passThroughLine(int32_t *in, int32_t *out, int32_t lineWidth, int32_t node, uint64_t split) {
    int32_t *inPtr = in;
    int32_t *outPtr = out;
    int32_t lwtemp = lineWidth;
    for(int i = 0; i < lineWidth; i++) {
        *outPtr++ = (lwtemp + 0) * (node + 0) * (split + 1) + i;
    }
}

} // extern "C"
