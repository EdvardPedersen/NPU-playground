// Headless NPU runner, same interface as gpu_bench.
//   npu_bench dump  t s h octaves file [t s h octaves file ...]   write raw frames
//   npu_bench bench octaves [seconds]                             time (and power)
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/experimental/xrt_elf.h"
#include "xrt/experimental/xrt_ext.h"
#include "xrt/experimental/xrt_module.h"
#include "perlin_params.h"
#include "power.h"

int main(int argc, char **argv) {
    if (argc < 3) { fprintf(stderr, "usage: %s dump t s h octaves file ... | bench octaves [seconds]\n", argv[0]); return 1; }
    auto device = xrt::device(0);
    auto xclbin = xrt::xclbin(std::string("device.xclbin"));
    device.register_xclbin(xclbin);
    xrt::elf elf(std::string("insts.elf"));
    xrt::module mod{elf};
    xrt::hw_context context(device, xclbin.get_uuid());
    auto kernel = xrt::ext::kernel(context, mod, "MLIR_AIE");
    xrt::bo in = xrt::ext::bo{device, PERLIN_NPU_IN * 4}, out = xrt::ext::bo{device, PERLIN_W * PERLIN_H * 4};
    auto run = [&](const PerlinParams &p) {
        perlin_fill_npu_input(in.map<int32_t *>(), p);
        in.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        kernel(3, 0, 0, in, out).wait2();
    };

    if (!strcmp(argv[1], "dump")) {
        for (int k = 2; k + 5 <= argc; k += 5) {
            PerlinParams p{(float)atof(argv[k]), (float)atof(argv[k + 1]), (float)atof(argv[k + 2]), atoi(argv[k + 3])};
            run(p);
            out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
            FILE *f = fopen(argv[k + 4], "wb");
            fwrite(out.map<uint32_t *>(), 4, PERLIN_W * PERLIN_H, f);
            fclose(f);
            PerlinQ12 q = perlin_q12(p);
            printf("%s %d %d %d %d\n", argv[k + 4], q.t, q.s, q.h, q.octaves);
        }
        return 0;
    }
    using C = std::chrono::steady_clock;
    PerlinParams p{12.3f, 0.3f, 1.0f, atoi(argv[2])};
    double secs = argc > 3 ? atof(argv[3]) : 0;
    run(p);
    for (int i = 0; i < 20; i++) kernel(3, 0, 0, in, out).wait2();
    const int R = 200;
    auto t0 = C::now();
    for (int i = 0; i < R; i++) kernel(3, 0, 0, in, out).wait2();
    double ms = std::chrono::duration<double, std::milli>(C::now() - t0).count() / R;
    printf("octaves %d  wall %.3f ms/frame\n", p.octaves, ms);
    if (secs > 0) {
        double sum = 0;
        int n = 0;
        auto end = C::now() + std::chrono::milliseconds((long)(secs * 1000)), next = C::now() + std::chrono::seconds(1);
        while (C::now() < end) {
            kernel(3, 0, 0, in, out).wait2();
            if (C::now() > next) { sum += apu_power_w(); n++; next += std::chrono::milliseconds(250); }
        }
        printf("power %.1f W\n", sum / n);
    }
}
