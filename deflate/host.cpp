//===- host.cpp -----------------------------------------------------------===//
// DEFLATE on the NPU: drives the 32-core fixed-Huffman compressor, verifies
// every produced stream with zlib's inflater, and benchmarks the device
// against the identical single-threaded CPU reference.
//
//   usage:  ./host [input-file]
// With no file a synthetic mixed corpus is used.
//
// Layout: symmetric 1024-byte DMA tiles; each tile carries DFL_CHUNK (1008)
// data bytes.  The host keeps the un-padded `source` separately and packs each
// 1008-byte logical chunk into a 1024-byte input tile (16 bytes zero pad).
//===----------------------------------------------------------------------===//
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>

#include <zlib.h>

#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/experimental/xrt_elf.h"
#include "xrt/experimental/xrt_ext.h"
#include "xrt/experimental/xrt_module.h"

#include "config.h"
#include "deflate.h" // CPU reference uses the same encoder as the kernel

// Must match design.py: splits=8, 8 columns x 4 cores.  One kernel invocation
// covers SLICE_CHUNKS chunks; the host issues NUM_SLICES invocations (over
// sub-buffers of one big BO) to cover the whole corpus.  Slices are kept small
// so each invocation's compute fits comfortably inside the command lifetime —
// at splits=64 the command "completed" in ~8 ms while the cores still owed
// hundreds of ms of compute, and the host read back garbage.
#define SPLITS 2
#define NUM_COLUMNS 8
#define CORES_PER_COL 4
#define SLICE_CHUNKS (NUM_COLUMNS * CORES_PER_COL * SPLITS) // 64 per run
#define NUM_SLICES 32
#define NUM_CHUNKS (SLICE_CHUNKS * NUM_SLICES)        // 2048
#define TOTAL_DATA ((size_t)NUM_CHUNKS * DFL_CHUNK)   // un-padded payload
#define SLICE_BYTES ((size_t)SLICE_CHUNKS * DFL_TILE) // device bytes per run
#define BO_BYTES ((size_t)NUM_CHUNKS * DFL_TILE)      // device in/out buffers

using clk = std::chrono::high_resolution_clock;

static uint32_t rd_len(const uint8_t *slot) {
    return (uint32_t)slot[0] | ((uint32_t)slot[1] << 8) |
           ((uint32_t)slot[2] << 16) | ((uint32_t)slot[3] << 24);
}

// Inflate one raw-DEFLATE stream and compare to the original chunk.
static bool verify(const uint8_t *stream, uint32_t clen, const uint8_t *orig) {
    uint8_t dec[DFL_CHUNK + 16];
    z_stream s;
    memset(&s, 0, sizeof(s));
    if (inflateInit2(&s, -15) != Z_OK)
        return false;
    s.next_in = (Bytef *)stream;
    s.avail_in = clen;
    s.next_out = dec;
    s.avail_out = sizeof(dec);
    int rc = inflate(&s, Z_FINISH);
    uint32_t produced = (uint32_t)s.total_out;
    inflateEnd(&s);
    return rc == Z_STREAM_END && produced == DFL_CHUNK &&
           memcmp(dec, orig, DFL_CHUNK) == 0;
}

// Verify every chunk against `source`; return total compressed bytes.
static uint64_t verify_all(const uint8_t *source, const uint8_t *out,
                           int *fail) {
    uint64_t comp = 0;
    *fail = 0;
    for (int n = 0; n < NUM_CHUNKS; n++) {
        const uint8_t *slot = out + (size_t)n * DFL_OUT;
        uint32_t clen = rd_len(slot);
        if (clen > (uint32_t)(DFL_OUT - DFL_HDR) ||
            !verify(slot + DFL_HDR, clen, source + (size_t)n * DFL_CHUNK))
            (*fail)++;
        comp += clen;
    }
    return comp;
}

// CPU reference: same encoder, same per-chunk framing, single threaded.
static void cpu_compress(const uint8_t *source, uint8_t *out) {
    static uint16_t head[DFL_HASH_SIZE];
    static uint16_t prev[DFL_CHUNK];
    for (int n = 0; n < NUM_CHUNKS; n++) {
        const uint8_t *src = source + (size_t)n * DFL_CHUNK;
        uint8_t *slot = out + (size_t)n * DFL_OUT;
        memset(slot, 0, DFL_OUT);
        uint32_t clen = dfl::deflate_fixed(src, DFL_CHUNK, slot + DFL_HDR,
                                           DFL_OUT - DFL_HDR, head, prev,
                                           DFL_HASH_SIZE, DFL_MAX_CHAIN);
        slot[0] = (uint8_t)(clen & 0xFF);
        slot[1] = (uint8_t)((clen >> 8) & 0xFF);
        slot[2] = (uint8_t)((clen >> 16) & 0xFF);
        slot[3] = (uint8_t)((clen >> 24) & 0xFF);
    }
}

static void make_source(const char *file, std::vector<uint8_t> &src) {
    src.resize(TOTAL_DATA);
    if (file) {
        FILE *f = fopen(file, "rb");
        if (!f) {
            fprintf(stderr, "cannot open %s\n", file);
            exit(1);
        }
        std::vector<uint8_t> raw;
        uint8_t buf[65536];
        size_t r;
        while ((r = fread(buf, 1, sizeof(buf), f)) > 0)
            raw.insert(raw.end(), buf, buf + r);
        fclose(f);
        if (raw.empty())
            raw.push_back(0);
        for (size_t i = 0; i < src.size(); i++)
            src[i] = raw[i % raw.size()];
        printf("input: %s (%zu bytes, tiled to %zu)\n", file, raw.size(),
               TOTAL_DATA);
    } else {
        srand(12345);
        const char *frag =
            "DEFLATE on the NPU: 32 cores each compress an independent chunk. ";
        size_t fl = strlen(frag);
        for (size_t i = 0; i < src.size(); i++)
            src[i] = ((i & 127) < 96) ? (uint8_t)frag[i % fl] : (uint8_t)rand();
        printf("input: synthetic mixed corpus (%zu bytes)\n", TOTAL_DATA);
    }
}

int main(int argc, const char *argv[]) {
    const char *file = (argc > 1) ? argv[1] : nullptr;
    std::vector<uint8_t> source;
    make_source(file, source);

    // ---------------- NPU setup ----------------
    auto device = xrt::device(0);
    auto xclbin = xrt::xclbin(std::string("device.xclbin"));
    device.register_xclbin(xclbin);
    xrt::elf elf(std::string("insts.elf"));
    xrt::module mod{elf};
    xrt::hw_context context(device, xclbin.get_uuid());
    auto kernel = xrt::ext::kernel(context, mod, "MLIR_AIE");

    // The xclbin's runtime sequence is built for ONE slice (SLICE_BYTES); the
    // BOs are exactly that size and reused for every invocation.  Each slice's
    // input is packed in, the command run, and the output copied to the right
    // place in `result`.  No sub-BO offset arithmetic — addressing is trivially
    // correct, which isolates the drain/compute behaviour from BO addressing.
    xrt::bo bo_in = xrt::ext::bo{device, SLICE_BYTES};
    xrt::bo bo_out = xrt::ext::bo{device, SLICE_BYTES};
    uint8_t *bin = bo_in.map<uint8_t *>();
    uint8_t *bslice = bo_out.map<uint8_t *>();
    std::vector<uint8_t> result(BO_BYTES, 0);
    uint8_t *bout = result.data();

    unsigned opcode = 3;
    // Whole corpus = NUM_SLICES sequential invocations.  Returns false if any
    // command finishes in a state other than COMPLETED (timeout/abort).
    auto run_all = [&](bool verbose) {
        bool ok = true;
        for (int s = 0; s < NUM_SLICES; s++) {
            // Pack this slice's chunks (1008 data bytes per 1024-byte tile).
            memset(bin, 0, SLICE_BYTES);
            for (int c = 0; c < SLICE_CHUNKS; c++) {
                int n = s * SLICE_CHUNKS + c;
                memcpy(bin + (size_t)c * DFL_TILE,
                       source.data() + (size_t)n * DFL_CHUNK, DFL_CHUNK);
            }
            bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);

            auto ts = clk::now();
            auto run = kernel(opcode, 0, 0, bo_in, bo_out);
            auto st = run.wait(std::chrono::seconds(10));
            double ms = std::chrono::duration<double>(clk::now() - ts).count() * 1e3;
            bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
            memcpy(bout + (size_t)s * SLICE_BYTES, bslice, SLICE_BYTES);

            if (st != ERT_CMD_STATE_COMPLETED) {
                ok = false;
                if (verbose)
                    printf("[diag] slice %d: command state %d (not COMPLETED)\n",
                           s, (int)st);
            }
            if (verbose && s < 4)
                printf("[diag] slice %d: %.3f ms (state %d)\n", s, ms, (int)st);
        }
        return ok;
    };
    if (!run_all(true))
        printf("[diag] warm pass had non-completed commands\n");

    int npu_fail = 0;
    uint64_t npu_comp = verify_all(source.data(), bout, &npu_fail);

    if (npu_fail) {
        uint64_t nonzero = 0;
        long first_nz = -1;
        for (size_t i = 0; i < BO_BYTES; i++)
            if (bout[i] != 0) {
                nonzero++;
                if (first_nz < 0)
                    first_nz = (long)i;
            }
        printf("[diag] device output: %llu / %zu bytes nonzero\n",
               (unsigned long long)nonzero, BO_BYTES);
        if (first_nz >= 0) {
            printf("[diag] first nonzero at byte %ld (slot %ld, off %ld):",
                   first_nz, first_nz / DFL_OUT, first_nz % DFL_OUT);
            for (int i = 0; i < 16; i++)
                printf(" %02x", bout[first_nz + i]);
            printf("\n");
        }
        int hdr_nz = 0, by_core[CORES_PER_COL] = {0}, by_col[NUM_COLUMNS] = {0};
        for (int n = 0; n < NUM_CHUNKS; n++) {
            uint32_t clen = rd_len(bout + (size_t)n * DFL_OUT);
            if (clen && clen <= (uint32_t)(DFL_OUT - DFL_HDR)) {
                hdr_nz++;
                by_col[(n % SLICE_CHUNKS) / (SPLITS * CORES_PER_COL)]++;
                by_core[n % CORES_PER_COL]++;
            }
        }
        printf("[diag] slots with valid length header: %d / %d\n", hdr_nz,
               NUM_CHUNKS);
        printf("[diag] valid-by-core:");
        for (int c = 0; c < CORES_PER_COL; c++)
            printf(" core%d=%d", c, by_core[c]);
        printf("\n[diag] valid-by-col: ");
        for (int c = 0; c < NUM_COLUMNS; c++)
            printf(" col%d=%d", c, by_col[c]);
        printf("\n");

        // Compare device output to the CPU reference per chunk to see HOW they
        // differ: identical, same length but different bytes, or different
        // length.  Bucket by the per-core split index (n%SPLITS = ping/pong
        // buffer) to expose any double-buffer-specific corruption.
        std::vector<uint8_t> ref(BO_BYTES, 0);
        cpu_compress(source.data(), ref.data());
        int diff = 0, len_diff = 0, byte_diff = 0;
        int fail_by_split[SPLITS] = {0};
        int shown = 0;
        for (int n = 0; n < NUM_CHUNKS; n++) {
            const uint8_t *d = bout + (size_t)n * DFL_OUT;
            const uint8_t *r = ref.data() + (size_t)n * DFL_OUT;
            if (memcmp(d, r, DFL_OUT) == 0)
                continue;
            diff++;
            fail_by_split[n % SPLITS]++;
            uint32_t dl = rd_len(d), rl = rd_len(r);
            if (dl != rl)
                len_diff++;
            else
                byte_diff++;
            if (shown < 6) {
                // first differing byte offset within the slot
                int off = 0;
                while (off < DFL_OUT && d[off] == r[off])
                    off++;
                printf("[diag] chunk %d differs: dev_len=%u ref_len=%u "
                       "first_diff@%d dev=%02x ref=%02x\n",
                       n, dl, rl, off, d[off], r[off]);
                shown++;
            }
        }
        printf("[diag] device!=reference: %d/%d chunks (len_diff=%d "
               "byte_diff=%d)\n",
               diff, NUM_CHUNKS, len_diff, byte_diff);
        printf("[diag] fails-by-split(ping/pong):");
        for (int sp = 0; sp < SPLITS; sp++)
            printf(" split%d=%d", sp, fail_by_split[sp]);
        printf("\n");

        // Inflate the first few FAILING device streams and report zlib's exact
        // verdict + how many bytes it decoded before giving up.  "invalid
        // distance too far back" => the match finder emitted a back-reference
        // before the chunk start (stale prev/head).  A short total_out =>
        // truncated/garbled mid-stream.
        int reported = 0;
        for (int n = 0; n < NUM_CHUNKS && reported < 4; n++) {
            const uint8_t *slot = bout + (size_t)n * DFL_OUT;
            uint32_t clen = rd_len(slot);
            if (clen <= (uint32_t)(DFL_OUT - DFL_HDR) &&
                verify(slot + DFL_HDR, clen, source.data() + (size_t)n * DFL_CHUNK))
                continue; // this one passed
            uint8_t dec[DFL_CHUNK + 64];
            z_stream s;
            memset(&s, 0, sizeof(s));
            inflateInit2(&s, -15);
            s.next_in = (Bytef *)(slot + DFL_HDR);
            s.avail_in = clen;
            s.next_out = dec;
            s.avail_out = sizeof(dec);
            int rc = inflate(&s, Z_FINISH);
            printf("[diag] FAIL chunk %d: zlib rc=%d (%s) decoded=%lu/%d "
                   "clen=%u\n",
                   n, rc, s.msg ? s.msg : "(no msg)",
                   (unsigned long)s.total_out, DFL_CHUNK, clen);
            inflateEnd(&s);
            // First 24 stream bytes, device vs reference, for hand-decoding.
            const uint8_t *rslot = ref.data() + (size_t)n * DFL_OUT;
            printf("[diag]   dev:");
            for (int i = 0; i < 24; i++) printf(" %02x", slot[DFL_HDR + i]);
            printf("\n[diag]   ref:");
            for (int i = 0; i < 24; i++) printf(" %02x", rslot[DFL_HDR + i]);
            printf("\n");
            reported++;
        }
    }

    // NPU benchmark (one "run" = all NUM_SLICES invocations).
    const int ITERS = 50;
    auto t0 = clk::now();
    for (int it = 0; it < ITERS; it++)
        run_all(false);
    auto t1 = clk::now();
    double npu_s = std::chrono::duration<double>(t1 - t0).count() / ITERS;

    // ---------------- CPU reference ----------------
    std::vector<uint8_t> cout(BO_BYTES, 0);
    cpu_compress(source.data(), cout.data()); // warm
    int cpu_fail = 0;
    uint64_t cpu_comp = verify_all(source.data(), cout.data(), &cpu_fail);

    const int CITERS = 5;
    auto c0 = clk::now();
    for (int it = 0; it < CITERS; it++)
        cpu_compress(source.data(), cout.data());
    auto c1 = clk::now();
    double cpu_s = std::chrono::duration<double>(c1 - c0).count() / CITERS;

    bool identical = (npu_comp == cpu_comp) &&
                     memcmp(bout, cout.data(), BO_BYTES) == 0;

    double mib = (double)TOTAL_DATA / (1024.0 * 1024.0);
    printf("\n");
    printf("chunks: %d x %d bytes = %.2f MiB payload\n", NUM_CHUNKS, DFL_CHUNK,
           mib);
    printf("verify: NPU %s (%d fail)   CPU %s (%d fail)   device==reference: %s\n",
           npu_fail ? "FAIL" : "ok", npu_fail, cpu_fail ? "FAIL" : "ok",
           cpu_fail, identical ? "yes" : "NO");
    printf("\n");
    printf("              compressed     ratio      time/run    throughput\n");
    printf("  NPU (x32)   %9llu   %6.2fx   %8.3f ms   %8.2f MiB/s\n",
           (unsigned long long)npu_comp, (double)TOTAL_DATA / (double)npu_comp,
           npu_s * 1e3, mib / npu_s);
    printf("  CPU (x1)    %9llu   %6.2fx   %8.3f ms   %8.2f MiB/s\n",
           (unsigned long long)cpu_comp, (double)TOTAL_DATA / (double)cpu_comp,
           cpu_s * 1e3, mib / cpu_s);
    printf("\n  speedup (NPU vs 1 CPU core): %.2fx\n", cpu_s / npu_s);

    return (npu_fail || cpu_fail || !identical) ? 1 : 0;
}
