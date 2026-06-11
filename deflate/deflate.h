//===- deflate.h ----------------------------------------------------------===//
//
// Self-contained DEFLATE (RFC 1951) compressor used by BOTH the AIE2P NPU
// kernel and the CPU reference.  It emits real, spec-compliant raw-DEFLATE
// streams (verifiable with zlib's inflate, windowBits = -15).
//
//  * LZ77 greedy match finding with a hash-chain (3-byte hash).
//  * Fixed-Huffman blocks (BTYPE = 01) so there is no two-pass tree building.
//  * Stored-block fallback (BTYPE = 00) when compression would expand the data,
//    which bounds the worst-case output to len + 5 bytes.
//
// Constraints honoured so the same code compiles for target aie2p-none-elf:
//   - integer only (no scalar float, no 64-bit divide),
//   - no heap (all scratch buffers are caller provided),
//   - no libc beyond <stdint.h>.
//
//===----------------------------------------------------------------------===//
#pragma once
#include <stdint.h>

namespace dfl {

// ----------------------------- tuning -------------------------------------
static const int MIN_MATCH = 3;
static const int MAX_MATCH = 258;
static const uint16_t NIL = 0xFFFF;

// ------------------------- RFC 1951 tables --------------------------------
static const uint16_t kLenBase[29] = {
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59,
    67, 83, 99, 115, 131, 163, 195, 227, 258};
static const uint8_t kLenExtra[29] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
    4, 4, 4, 4, 5, 5, 5, 5, 0};
static const uint16_t kDistBase[30] = {
    1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513,
    769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577};
static const uint8_t kDistExtra[30] = {
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
    9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

// --------------------------- bit writer -----------------------------------
// DEFLATE packs the bit stream LSB-first, but Huffman codes are emitted
// MSB-first; emitting the bit-reversed code LSB-first achieves that.
struct BitWriter {
    uint8_t *out;
    uint32_t cap;
    uint32_t pos;     // next byte index to write
    uint32_t acc;     // bit accumulator (low `cnt` bits valid)
    uint32_t cnt;     // number of valid bits in acc
    uint32_t overflow; // set if we ran past cap
};

static inline void bw_init(BitWriter *w, uint8_t *out, uint32_t cap) {
    w->out = out;
    w->cap = cap;
    w->pos = 0;
    w->acc = 0;
    w->cnt = 0;
    w->overflow = 0;
}

static inline void bw_bits(BitWriter *w, uint32_t val, uint32_t n) {
    uint32_t mask = (n >= 32) ? 0xFFFFFFFFu : ((1u << n) - 1u);
    w->acc |= (val & mask) << w->cnt;
    w->cnt += n;
    while (w->cnt >= 8) {
        if (w->pos < w->cap)
            w->out[w->pos] = (uint8_t)(w->acc & 0xFF);
        else
            w->overflow = 1;
        w->pos++;
        w->acc >>= 8;
        w->cnt -= 8;
    }
}

static inline uint32_t bw_rev(uint32_t v, uint32_t n) {
    uint32_t r = 0;
    for (uint32_t i = 0; i < n; i++) {
        r = (r << 1) | (v & 1u);
        v >>= 1;
    }
    return r;
}

// emit a Huffman code (MSB-first) of `n` bits
static inline void bw_huff(BitWriter *w, uint32_t code, uint32_t n) {
    bw_bits(w, bw_rev(code, n), n);
}

static inline void bw_align(BitWriter *w) {
    if (w->cnt > 0) {
        if (w->pos < w->cap)
            w->out[w->pos] = (uint8_t)(w->acc & 0xFF);
        else
            w->overflow = 1;
        w->pos++;
        w->acc = 0;
        w->cnt = 0;
    }
}

static inline uint32_t bw_finish(BitWriter *w) {
    bw_align(w);
    return w->pos;
}

// ------------------- fixed-Huffman symbol emitters ------------------------
// literal / length symbol 0..287 with the RFC fixed code lengths
static inline void emit_litlen(BitWriter *w, int sym) {
    if (sym <= 143)
        bw_huff(w, 0x30 + sym, 8);
    else if (sym <= 255)
        bw_huff(w, 0x190 + (sym - 144), 9);
    else if (sym <= 279)
        bw_huff(w, sym - 256, 7);
    else
        bw_huff(w, 0xC0 + (sym - 280), 8);
}

static inline void emit_length(BitWriter *w, int len) {
    int i = 28;
    while (i > 0 && kLenBase[i] > len)
        i--;
    emit_litlen(w, 257 + i);
    if (kLenExtra[i])
        bw_bits(w, (uint32_t)(len - kLenBase[i]), kLenExtra[i]);
}

static inline void emit_distance(BitWriter *w, int dist) {
    int i = 29;
    while (i > 0 && kDistBase[i] > dist)
        i--;
    bw_huff(w, (uint32_t)i, 5); // fixed 5-bit distance code
    if (kDistExtra[i])
        bw_bits(w, (uint32_t)(dist - kDistBase[i]), kDistExtra[i]);
}

// ----------------------------- hashing ------------------------------------
static inline uint32_t hash3(const uint8_t *p, uint32_t hbits) {
    uint32_t h = ((uint32_t)p[0] << 16) | ((uint32_t)p[1] << 8) | (uint32_t)p[2];
    return (h * 2654435761u) >> (32 - hbits);
}

// ---------------------- stored (uncompressed) block -----------------------
static uint32_t store_block(const uint8_t *in, uint32_t len, uint8_t *out,
                            uint32_t cap) {
    BitWriter w;
    bw_init(&w, out, cap);
    bw_bits(&w, 1, 1); // BFINAL
    bw_bits(&w, 0, 2); // BTYPE = 00 (stored)
    bw_align(&w);
    // LEN / NLEN little-endian, then raw bytes
    uint32_t p = w.pos;
    if (p + 4 + len <= cap) {
        out[p + 0] = (uint8_t)(len & 0xFF);
        out[p + 1] = (uint8_t)((len >> 8) & 0xFF);
        out[p + 2] = (uint8_t)(~len & 0xFF);
        out[p + 3] = (uint8_t)((~len >> 8) & 0xFF);
        for (uint32_t i = 0; i < len; i++)
            out[p + 4 + i] = in[i];
    }
    return p + 4 + len;
}

// ----------------------------- encoder ------------------------------------
// Compress `len` bytes of `in` into `out` (capacity `cap`).  `head` is a
// hash table of `hash_size` (power of two) uint16 entries, `prev` is a
// uint16 chain of at least `len` entries; both are caller owned scratch.
// Returns the number of bytes written.  Falls back to a stored block if the
// fixed-Huffman encoding would expand the data or overflow `cap`.
static uint32_t deflate_fixed(const uint8_t *in, uint32_t len, uint8_t *out,
                              uint32_t cap, uint16_t *head, uint16_t *prev,
                              uint32_t hash_size, int max_chain) {
    uint32_t hbits = 0;
    for (uint32_t s = hash_size; s > 1; s >>= 1)
        hbits++;
    // Clear the hash table to NIL.  Written byte-wise (NIL = 0xFFFF, so every
    // byte is 0xFF) because the element-wise uint16 store loop was left
    // partially un-cleared on the AIE core, so stale positions from the
    // previous chunk survived and produced bogus matches at pos 0 (invalid
    // back-references that zlib rejects).
    uint8_t *hb = (uint8_t *)head;
    for (uint32_t i = 0; i < hash_size * 2u; i++)
        hb[i] = 0xFF;

    BitWriter w;
    bw_init(&w, out, cap);
    bw_bits(&w, 1, 1); // BFINAL
    bw_bits(&w, 1, 2); // BTYPE = 01 (fixed Huffman)

    uint32_t pos = 0;
    while (pos < len) {
        int best_len = 0;
        uint32_t best_dist = 0;

        if (pos + MIN_MATCH <= len) {
            uint32_t h = hash3(in + pos, hbits) & (hash_size - 1);
            uint16_t cand = head[h];
            int chain = max_chain;
            uint32_t maxl = len - pos;
            if (maxl > (uint32_t)MAX_MATCH)
                maxl = MAX_MATCH;
            while (cand != NIL && chain-- > 0) {
                // A valid back-reference must point strictly before pos.  On a
                // correctly-reset table this is always true; the guard makes it
                // impossible to emit an invalid distance even if the table
                // somehow holds a stale entry >= pos (belt-and-suspenders for
                // the AIE core, a no-op on the CPU reference).
                if ((uint32_t)cand >= pos)
                    break;
                const uint8_t *a = in + pos;
                const uint8_t *b = in + cand;
                uint32_t l = 0;
                while (l < maxl && a[l] == b[l])
                    l++;
                if ((int)l > best_len) {
                    best_len = (int)l;
                    best_dist = pos - cand;
                    if (l >= maxl)
                        break;
                }
                cand = prev[cand];
            }
            // insert current position into the hash chain
            prev[pos] = head[h];
            head[h] = (uint16_t)pos;
        }

        if (best_len >= MIN_MATCH) {
            emit_length(&w, best_len);
            emit_distance(&w, (int)best_dist);
            // insert hashes for the bytes covered by the match
            uint32_t end = pos + best_len;
            pos++;
            while (pos < end) {
                if (pos + MIN_MATCH <= len) {
                    uint32_t h = hash3(in + pos, hbits) & (hash_size - 1);
                    prev[pos] = head[h];
                    head[h] = (uint16_t)pos;
                }
                pos++;
            }
        } else {
            emit_litlen(&w, in[pos]);
            pos++;
        }
    }
    emit_litlen(&w, 256); // end of block
    uint32_t clen = bw_finish(&w);

    // Fall back to a stored block if we expanded or overflowed.
    if (w.overflow || clen >= len + 5) {
        return store_block(in, len, out, cap);
    }
    return clen;
}

} // namespace dfl
