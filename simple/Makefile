CC=gcc
CPP=g++
#PEANO=/home/edvard/work/llvm-aie/build/bin/clang++ --target=aie2p-none-unknown-elf -c -v  -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/include/ --std=c++20 -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/include -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/bin/../include/aie2p-none-unknown-elf/c++/v1 -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/bin/../include/c++/v1 -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/bin/../include/aie2p-none-unknown-elf -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/lib/clang/21/include -Wno-c++11-narrowing -Wno-unknown-attributes -Wno-undefined-inline -Wno-empty-body -Wno-macro-redefined -O2 -S
PEANO=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/bin/clang++ --target=aie2p-none-unknown-elf -c -v  -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/include/ --std=c++20 -O0 -Wno-unknown-attributes -Wno-empty-body -Wno-macro-redefined -O2
AIE_OPT=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/bin/aie-opt --aie-place-tiles
AIECC=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/bin/aiecc --no-xchesscc --peano=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/ --no-xbridge --no-compile-host --target-device=npu2 --alloc-scheme=basic-sequential

all: device.xclbin host

kernel.o: kernel.cpp Makefile
	$(PEANO) kernel.cpp -o kernel.o

device.mlir: design.py Makefile
	python3 design.py > device.mlir

device_placed.mlir: device.mlir Makefile
	$(AIE_OPT) device.mlir -o device_placed.mlir

device.xclbin: device_placed.mlir kernel.o Makefile
	$(AIECC) -v --link --aie-generate-elf --elf-name=insts.elf --aie-generate-xclbin --xclbin-name=device.xclbin device_placed.mlir

host: host.cpp Makefile
	$(CPP) host.cpp -lSDL3 -lxrt_coreutil -o host

run: host device.xclbin
	./host
