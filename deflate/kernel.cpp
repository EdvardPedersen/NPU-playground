//===- kernel.cpp ---------------------------------------------------------===//
// AIE2P (npu2) DEFLATE kernel.  Each core compresses one independent
// DFL_CHUNK-byte chunk into a DFL_OUT-byte output tile:
//   out[0..3] = little-endian compressed length
//   out[4..]  = raw-DEFLATE stream (RFC 1951, fixed Huffman / stored)
//
// The whole output tile is written every call (compressed data + zero pad), so
// the core behaves like a full-tile producer — matching the mandelbrot
// objectfifo pattern that the mem-tile join/drain expects.  Partial writes left
// the drained buffer non-deterministic.
//
// Compression lives in deflate.h and is shared bit-for-bit with the CPU
// reference, so device output and CPU output are identical.
//===----------------------------------------------------------------------===//
#include <stdint.h>
#include <stdlib.h>
#ifndef KERNEL_HOST_TEST
#include <aie_api/aie.hpp> // not used directly; kept for toolchain parity
#endif

#include "config.h"
#include "deflate.h"

extern "C" {

// Per-core scratch.  File-scope static (not stack): reused across iterations.
static uint16_t g_head[DFL_HASH_SIZE];
static uint16_t g_prev[DFL_CHUNK];

// in/out are int32* tiles (256 elems = 1024 bytes) treated as byte buffers.
void deflateChunk(int32_t *__restrict in, int32_t *__restrict out,
                  int32_t chunk) {
    const uint8_t *src = (const uint8_t *)in;
    uint8_t *dst = (uint8_t *)out;

    // Write the full output tile so the producer touches every byte.
    for (int i = 0; i < DFL_OUT; i++)
        dst[i] = 0;

    uint32_t clen =
        dfl::deflate_fixed(src, (uint32_t)chunk, dst + DFL_HDR,
                           (uint32_t)(DFL_OUT - DFL_HDR), g_head, g_prev,
                           DFL_HASH_SIZE, DFL_MAX_CHAIN);

    dst[0] = (uint8_t)(clen & 0xFF);
    dst[1] = (uint8_t)((clen >> 8) & 0xFF);
    dst[2] = (uint8_t)((clen >> 16) & 0xFF);
    dst[3] = (uint8_t)((clen >> 24) & 0xFF);
}

} // extern "C"
