CC=gcc
CPP=g++
PEANO=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/bin/clang++ --target=aie2p-none-unknown-elf -c
AIECC=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/bin/aiecc --no-xchesscc --peano=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/ --no-xbridge --no-compile-host --target-device=npu2

all: device.xclbin

kernel.o: device.cc Makefile
	$(PEANO) device.cc -o kernel.o

device.xclbin: kernel.o device.mlir Makefile
	$(AIECC) -v --aie-generate-xclbin --xclbin-name=device.xclbin device.mlir

host: host.c Makefile
	$(CC) host.c -g -lxrt_coreutil -o host

host2: host.cpp
	$(CPP) host.cpp -lxrt_coreutil -o host2
