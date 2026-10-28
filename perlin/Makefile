CC=gcc
CPP=g++
#PEANO=/home/edvard/work/llvm-aie/build/bin/clang++ --target=aie2p-none-unknown-elf -c -v  -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/include/ --std=c++20 -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/include -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/bin/../include/aie2p-none-unknown-elf/c++/v1 -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/bin/../include/c++/v1 -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/bin/../include/aie2p-none-unknown-elf -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/lib/clang/21/include -Wno-c++11-narrowing -Wno-unknown-attributes -Wno-undefined-inline -Wno-empty-body -Wno-macro-redefined -O2 -S
PEANO=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/bin/clang++ --target=aie2p-none-unknown-elf -c -v  -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/include/ --std=c++20 -O0 -Wno-unknown-attributes -Wno-empty-body -Wno-macro-redefined -O2
AIE_OPT=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/bin/aie-opt --aie-place-tiles
AIECC=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/bin/aiecc --no-xchesscc --peano=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/ --no-xbridge --no-compile-host --target-device=npu2
PYTHON=/home/edvard/work/mlir-aie/ironenv/bin/python3
GLSLC=glslc

TERRAIN_SHADERS=terrain.vert.spv terrain.geom.spv terrain.frag.spv tree.comp.spv tree.vert.spv tree.frag.spv
TERRAIN_INCLUDES=terrain_pc.glsl terrain_hmap.glsl terrain_trees.glsl terrain_rt.glsl

all: device.xclbin host perlin.comp.spv gpu_bench npu_bench fly $(TERRAIN_SHADERS)

terrain.%.spv: terrain.% $(TERRAIN_INCLUDES) Makefile
	$(GLSLC) $< -o $@

tree.%.spv: tree.% $(TERRAIN_INCLUDES) Makefile
	$(GLSLC) $< -o $@

fly: fly.cpp perlin_params.h Makefile
	$(CPP) fly.cpp -lSDL3 -lxrt_coreutil -lvulkan -o fly -O2

run-fly: fly device.xclbin perlin.comp.spv $(TERRAIN_SHADERS)
	./fly

perlin.comp.spv: perlin.comp Makefile
	$(GLSLC) $< -o $@

gpu_bench: gpu_bench.cpp perlin_params.h power.h Makefile
	$(CPP) gpu_bench.cpp -lvulkan -o gpu_bench -O2

npu_bench: npu_bench.cpp perlin_params.h power.h Makefile
	$(CPP) npu_bench.cpp -lxrt_coreutil -o npu_bench -O2

kernel.o: kernel.cpp Makefile
	$(PEANO) kernel.cpp -o kernel.o

device.mlir: design.py Makefile
	$(PYTHON) design.py > device.mlir

device_placed.mlir: device.mlir Makefile
	$(AIE_OPT) device.mlir -o device_placed.mlir

device.xclbin: device_placed.mlir kernel.o Makefile
	$(AIECC) -v --link --aie-generate-elf --elf-name=insts.elf --aie-generate-xclbin --xclbin-name=device.xclbin device_placed.mlir

host: host.cpp Makefile
	$(CPP) host.cpp -lSDL3 -lxrt_coreutil -o host -O2

run: host device.xclbin
	./host
