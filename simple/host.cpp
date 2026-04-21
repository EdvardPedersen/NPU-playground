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

#include <chrono>

#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"

#include "xrt/experimental/xrt_elf.h"
#include "xrt/experimental/xrt_ext.h"
#include "xrt/experimental/xrt_module.h"

#include <SDL3/SDL.h>

#define WIDTH 128
#define HEIGHT 128
#define N WIDTH*HEIGHT

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

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  std::cout << "Running setup kernel" << std::endl;
  auto cfg_run = kernel(opcode, 0, 0, bo_inA, bo_out);
  cfg_run.wait2();
  bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

  std::cout << "Time difference = " << millis << "[ms]" << std::endl;
  uint32_t *bufOut = bo_out.map<uint32_t *>();

  int errors = 0;

  for (uint32_t i = 0; i < N; i++) {
    uint32_t ref = i;
    if (*(bufOut + i) != ref) {
      errors++;
    }
  }

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_CreateWindowAndRenderer("Mandelbrot example", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer);

  SDL_Surface *output = SDL_CreateSurfaceFrom(WIDTH, HEIGHT, SDL_PIXELFORMAT_RGBA8888, bufOut, WIDTH * sizeof(int32_t));
  SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, output);

  while(true) {
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) {
        if(ev.type == SDL_EVENT_KEY_DOWN) return 0;
    }

    SDL_RenderTexture(renderer, tex, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_Delay(16);
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
