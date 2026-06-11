CC=gcc
CPP=g++
PEANO=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/bin/clang++ --target=aie2p-none-unknown-elf -c -v  -I/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/include/ --std=c++20 -O0 -Wno-unknown-attributes -Wno-empty-body -Wno-macro-redefined -O2
AIE_OPT=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/bin/aie-opt --aie-place-tiles
AIECC=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/bin/aiecc --no-xchesscc --peano=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/ --no-xbridge --no-compile-host --target-device=npu2

all: device.xclbin host

kernel.o: kernel.cpp deflate.h config.h Makefile
	$(PEANO) kernel.cpp -o kernel.o

device.mlir: design.py Makefile
	python3 design.py > device.mlir

device_placed.mlir: device.mlir Makefile
	$(AIE_OPT) device.mlir -o device_placed.mlir

device.xclbin: device_placed.mlir kernel.o Makefile
	$(AIECC) -v --link --aie-generate-elf --elf-name=insts.elf --aie-generate-xclbin --xclbin-name=device.xclbin device_placed.mlir

host: host.cpp deflate.h config.h Makefile
	$(CPP) host.cpp -lxrt_coreutil -lz -o host -O2

# CPU correctness test (runs the kernel code, verifies against zlib). No device.
test_deflate: test.cpp kernel.cpp deflate.h config.h Makefile
	$(CPP) test.cpp -O2 -lz -o test_deflate

test: test_deflate
	./test_deflate

run: host device.xclbin
	./host

clean:
	rm -f kernel.o host test_deflate device.mlir device_placed.mlir device.xclbin insts.elf
