CC=gcc
PEANO=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/bin/clang++ --target=aie2p-none-unknown-elf -c
AIECC=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/mlir_aie/bin/aiecc --no-xchesscc --peano=/home/edvard/work/mlir-aie/ironenv/lib/python3.13/site-packages/llvm-aie/ --no-xbridge

all: device.xclbin

kernel.o: device.cc
	$(PEANO) device.cc -o kernel.o

device.xclbin: kernel.o device.mlir
	$(AIECC) -v --link --aie-generate-xclbin --xclbin-name=device.xclbin device.mlir

host: host.c
	$(CC) host.c -lxrt_coreutil -o host
