//===- test.cpp -----------------------------------------------------------===//
// CPU correctness test for the DEFLATE kernel.  Runs the EXACT kernel code
// (deflateChunk, via KERNEL_HOST_TEST) over several data patterns, then proves
// every produced stream is spec-compliant by inflating it with zlib's raw
// inflater and checking the result matches the original chunk byte-for-byte.
//
//   build:  g++ test.cpp -DKERNEL_HOST_TEST -O2 -lz -o test_deflate
//===----------------------------------------------------------------------===//
#define KERNEL_HOST_TEST
#include "kernel.cpp" // pulls in deflateChunk + deflate.h + config.h

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include <zlib.h>

extern "C" void deflateChunk(int32_t *in, int32_t *out, int32_t chunk);

// Inflate a raw-DEFLATE stream and verify it reproduces `orig`.
static bool verify_stream(const uint8_t *stream, uint32_t clen,
                          const uint8_t *orig, uint32_t olen) {
    std::vector<uint8_t> dec(olen + 16, 0xAB);
    z_stream s;
    memset(&s, 0, sizeof(s));
    if (inflateInit2(&s, -15) != Z_OK)
        return false;
    s.next_in = (Bytef *)stream;
    s.avail_in = clen;
    s.next_out = dec.data();
    s.avail_out = (uInt)dec.size();
    int rc = inflate(&s, Z_FINISH);
    uint32_t produced = (uint32_t)s.total_out;
    inflateEnd(&s);
    if (rc != Z_STREAM_END)
        return false;
    if (produced != olen)
        return false;
    return memcmp(dec.data(), orig, olen) == 0;
}

struct Stats {
    uint64_t in = 0, out = 0;
    int chunks = 0, fail = 0;
};

// Compress `data` chunk-by-chunk through the kernel and verify each chunk.
static Stats run_case(const char *name, const std::vector<uint8_t> &data) {
    Stats st;
    std::vector<int32_t> inbuf(DFL_CHUNK / 4);
    std::vector<int32_t> outbuf(DFL_OUT / 4);

    for (size_t off = 0; off < data.size(); off += DFL_CHUNK) {
        uint32_t clen_chunk =
            (uint32_t)std::min<size_t>(DFL_CHUNK, data.size() - off);
        // pad partial chunk with zeros (kernel always processes DFL_CHUNK)
        memset(inbuf.data(), 0, DFL_CHUNK);
        memcpy(inbuf.data(), data.data() + off, clen_chunk);

        deflateChunk(inbuf.data(), outbuf.data(), DFL_CHUNK);

        const uint8_t *ob = (const uint8_t *)outbuf.data();
        uint32_t comp = (uint32_t)ob[0] | ((uint32_t)ob[1] << 8) |
                        ((uint32_t)ob[2] << 16) | ((uint32_t)ob[3] << 24);

        bool ok = comp <= (uint32_t)(DFL_OUT - DFL_HDR) &&
                  verify_stream(ob + DFL_HDR, comp,
                                (const uint8_t *)inbuf.data(), DFL_CHUNK);
        st.in += DFL_CHUNK;
        st.out += comp;
        st.chunks++;
        if (!ok)
            st.fail++;
    }
    double ratio = st.out ? (double)st.in / (double)st.out : 0.0;
    printf("  %-18s chunks=%4d  in=%llu out=%llu  ratio=%.2fx  %s\n", name,
           st.chunks, (unsigned long long)st.in, (unsigned long long)st.out,
           ratio, st.fail ? "*** FAIL ***" : "ok");
    return st;
}

int main() {
    srand(1234);
    printf("DEFLATE kernel CPU correctness (verified against zlib inflate)\n");

    int total_fail = 0;

    // 1. Highly repetitive text (great LZ77 matches).
    {
        std::vector<uint8_t> d;
        const char *frag = "the quick brown fox jumps over the lazy dog. ";
        while (d.size() < 64 * 1024)
            for (const char *p = frag; *p; ++p)
                d.push_back((uint8_t)*p);
        total_fail += run_case("repetitive-text", d).fail;
    }
    // 2. Long runs (RLE-style; exercises max-length matches).
    {
        std::vector<uint8_t> d(64 * 1024);
        for (size_t i = 0; i < d.size(); i++)
            d[i] = (uint8_t)((i / 300) & 0xFF);
        total_fail += run_case("runs", d).fail;
    }
    // 3. Incompressible random (exercises stored-block fallback).
    {
        std::vector<uint8_t> d(64 * 1024);
        for (auto &b : d)
            b = (uint8_t)rand();
        total_fail += run_case("random", d).fail;
    }
    // 4. Mixed: structured + noisy.
    {
        std::vector<uint8_t> d;
        for (int i = 0; i < 32 * 1024; i++) {
            if ((i & 63) < 40)
                d.push_back((uint8_t)('A' + (i % 26)));
            else
                d.push_back((uint8_t)rand());
        }
        total_fail += run_case("mixed", d).fail;
    }
    // 5. All zeros (degenerate).
    {
        std::vector<uint8_t> d(8 * 1024, 0);
        total_fail += run_case("zeros", d).fail;
    }
    // 6. Every chunk size from 0..DFL_CHUNK of patterned data (edge lengths).
    {
        int edge_fail = 0, n = 0;
        std::vector<int32_t> inbuf(DFL_CHUNK / 4), outbuf(DFL_OUT / 4);
        for (int len = 0; len <= DFL_CHUNK; len += 7) {
            memset(inbuf.data(), 0, DFL_CHUNK);
            uint8_t *ib = (uint8_t *)inbuf.data();
            for (int i = 0; i < len; i++)
                ib[i] = (uint8_t)((i * 31 + (i >> 3)) & 0xFF);
            deflateChunk(inbuf.data(), outbuf.data(), len);
            const uint8_t *ob = (const uint8_t *)outbuf.data();
            uint32_t comp = (uint32_t)ob[0] | ((uint32_t)ob[1] << 8) |
                            ((uint32_t)ob[2] << 16) | ((uint32_t)ob[3] << 24);
            if (!(comp <= (uint32_t)(DFL_OUT - DFL_HDR) &&
                  verify_stream(ob + DFL_HDR, comp, ib, (uint32_t)len)))
                edge_fail++;
            n++;
        }
        printf("  %-18s cases=%4d  %s\n", "edge-lengths", n,
               edge_fail ? "*** FAIL ***" : "ok");
        total_fail += edge_fail;
    }

    printf("\n%s\n", total_fail ? "RESULT: FAILED" : "RESULT: ALL PASS");
    return total_fail ? 1 : 0;
}
