//===- config.h -----------------------------------------------------------===//
// Shared layout constants for the DEFLATE NPU demo.  design.py mirrors these.
//
// The DMA tiles are SYMMETRIC (input tile == output tile == 256 int32 = 1024
// bytes), matching the proven mandelbrot topology exactly — asymmetric tile
// sizes caused a non-deterministic mem-tile join/drain race.  Each 1024-byte
// tile carries DFL_CHUNK = 1008 data bytes that the core compresses; the output
// tile holds a 4-byte length header + the raw-DEFLATE stream.  Worst case
// (stored block) is 1008 + 5 + 4 = 1017 <= 1024, so it always fits.
//===----------------------------------------------------------------------===//
#pragma once

#define DFL_TILE 1024  // bytes per DMA tile (input and output), 256 int32
#define DFL_CHUNK 1008 // data bytes compressed per tile
#define DFL_OUT 1024   // output tile bytes (== DFL_TILE)
#define DFL_HDR 4      // little-endian compressed-length header per output tile

// Hash table used by the encoder (per core).  Static scratch is
// head(2*HASH_SIZE) + prev(2*CHUNK) bytes; kept small for the per-core budget.
#define DFL_HASH_SIZE 1024 // power of two
// Worst-case match-finder cost per chunk is ~DFL_MAX_CHAIN * MAX_MATCH byte
// compares per input byte.  On the scalar AIE core a long chain over highly
// repetitive data dominates runtime, so keep this small to bound per-command
// compute (the device aborts/returns long commands with partial data).
#define DFL_MAX_CHAIN 16
