//===- test.cpp -------------------------------------------000---*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Copyright (C) 2025, Advanced Micro Devices, Inc.
//
//===----------------------------------------------------------------------===//

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"

#include "xrt/experimental/xrt_elf.h"
#include "xrt/experimental/xrt_ext.h"
#include "xrt/experimental/xrt_module.h"

#define N 1024*1024

int main(int argc, const char *argv[]) {
  // Start the XRT test code
  // Get a device handle
  unsigned int device_index = 0;
  auto device = xrt::device(device_index);

  auto xclbin = xrt::xclbin(std::string("device.xclbin"));

  // Get the kernel from the xclbin
  device.register_xclbin(xclbin);
  xrt::elf elf(std::string("insts.elf"));
  xrt::module mod{elf};
  xrt::hw_context context(device, xclbin.get_uuid());
  auto kernel = xrt::ext::kernel(context, mod, "MLIR_AIE");
  xrt::bo bo_inA = xrt::ext::bo{device, N * sizeof(int32_t)};
  xrt::bo bo_out = xrt::ext::bo{device, N * sizeof(int32_t)};

  int32_t *bufInA = bo_inA.map<int32_t *>();
  std::vector<uint32_t> srcVecA;
  for (int i = 0; i < N; i++)
    srcVecA.push_back(i + 1);
  memcpy(bufInA, srcVecA.data(), (srcVecA.size() * sizeof(uint32_t)));

  std::cout << "Running memsync kernel" << std::endl;
  bo_inA.sync(XCL_BO_SYNC_BO_TO_DEVICE);

  unsigned int opcode = 3;
  // Setup run to configure
  std::cout << "Running setup kernel" << std::endl;
  auto cfg_run = kernel(opcode, 0, 0, bo_inA, bo_out);
  cfg_run.wait2();
  std::cout << "Kernel complete" << std::endl;
  bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
  uint32_t *bufOut = bo_out.map<uint32_t *>();

  int errors = 0;

  for (uint32_t i = 0; i < N; i++) {
    uint32_t ref = i;
    std::cout << bufOut[i] << " - " << ref << std::endl;
    if (*(bufOut + i) != ref) {
      errors++;
    }
  }

  if (!errors) {
    std::cout << std::endl << "PASS!" << std::endl << std::endl;
    return 0;
  } else {
    std::cout << std::endl
              << errors << " mismatches." << std::endl
              << std::endl;
    std::cout << std::endl << "fail." << std::endl << std::endl;
    return 1;
  }
}
