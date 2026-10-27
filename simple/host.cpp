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

#define WIDTH 1024
#define HEIGHT 1024
#define N WIDTH*HEIGHT
#define TILE 256 // pixels per kernel call, see design.py
static_assert(WIDTH % TILE == 0, "a tile must not span rows");

int main(int argc, const char *argv[]) {
  unsigned int device_index = 0;
  auto device = xrt::device(device_index);

  auto xclbin = xrt::xclbin(std::string("device.xclbin"));

  device.register_xclbin(xclbin);
  xrt::elf elf(std::string("insts.elf"));
  xrt::module mod{elf};
  xrt::hw_context context(device, xclbin.get_uuid());
  auto kernel = xrt::ext::kernel(context, mod, "MLIR_AIE");
  xrt::bo bo_inA = xrt::ext::bo{device, N * sizeof(int32_t)};
  xrt::bo bo_out = xrt::ext::bo{device, N * sizeof(int32_t)};

  int32_t *bufInA = bo_inA.map<int32_t *>();
  int32_t *bufOut = bo_out.map<int32_t *>();

  unsigned int opcode = 3;

  // Zoom animation state
  float cx = -0.5f, cy = 0.0f;
  float target_cx = -0.75f, target_cy = 0.1f;
  float start_range = 3.0f;
  float zoom = 1.0f;
  const float zoom_speed = 1.02f;
  const float center_lerp = 0.002f;

  auto fill_params = [&]() {
    float range_f = start_range / zoom;
    float ox = cx - range_f / 2, oy = cy - range_f / 2;
    float dx = range_f / WIDTH, dy = range_f / HEIGHT;
    for (int p = 0; p < N; p += TILE) {
      float params[3] = {ox + (p % WIDTH) * dx, oy + (p / WIDTH) * dy, dx};
      memcpy(&bufInA[p], params, sizeof(params));
    }
  };

  fill_params();
  bo_inA.sync(XCL_BO_SYNC_BO_TO_DEVICE);
  auto warmup = kernel(opcode, 0, 0, bo_inA, bo_out);
  warmup.wait2();

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_CreateWindowAndRenderer("Mandelbrot NPU", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer);

  SDL_Texture *tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

  bool running = true;
  uint64_t last_time = SDL_GetTicks();
  uint32_t frames = 0;
  auto t0 = std::chrono::high_resolution_clock::now();
  while (running) {
    frames += 1;
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_EVENT_QUIT)
        running = false;
      if (ev.type == SDL_EVENT_KEY_DOWN)
        running = false;
    }
    if (!running)
      break;
    if(SDL_GetTicks() - last_time > 1000) {
        char data[200];
        snprintf(data, 200, "NPU FPS: %u | zoom: %.1fx", frames, (double)zoom);
        SDL_SetWindowTitle(window, data);
        frames = 0;
        last_time = SDL_GetTicks();
    }

    auto t = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float>(t - t0).count();
    t0 = t;

    cx += (target_cx - cx) * center_lerp * dt * 60.0f;
    cy += (target_cy - cy) * center_lerp * dt * 60.0f;
    zoom *= zoom_speed;
    if(zoom > 4.0) zoom = 1.0;

    fill_params();
    bo_inA.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    auto frame_run = kernel(opcode, 0, 0, bo_inA, bo_out);
    frame_run.wait2();
    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    SDL_UpdateTexture(tex, NULL, bufOut, WIDTH * sizeof(int32_t));
    SDL_RenderTexture(renderer, tex, NULL, NULL);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyTexture(tex);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
