#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"

int main(int argc, const char *argv[]) {
  unsigned int device_index = 0;
  auto device = xrt::device(device_index);

  auto xclbin = xrt::xclbin(std::string("device.xclbin"));

  auto xkernels = xclbin.get_kernels();
  device.register_xclbin(xclbin);
  xrt::hw_context context(device, xclbin.get_uuid());
  auto kernel = xrt::kernel(context, "MLIR_AIE");

  auto bank_1 = kernel.group_id(1);
  auto output_buffer = xrt::bo(device, 256*sizeof(float),xrt::bo::flags::device_only ,  bank_1);

  auto run = kernel(output_buffer);
  run.wait();

}
