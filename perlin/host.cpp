#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <chrono>
#include <cmath>

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

  int32_t time_q12 = 0;
  int32_t scale_q12 = 1024;
  int32_t speed_q12 = 41;
  int32_t height_q12 = 4096;

  auto fill_params = [&]() {
    for (int col = 0; col < 8; col++)
      for (int it = 0; it < 32; it++)
        for (int row = 0; row < 4; row++) {
          int cs = col * 131072 + it * 4096 + row * 1024;
          bufInA[cs + 0] = time_q12;
          bufInA[cs + 1] = scale_q12;
          bufInA[cs + 2] = speed_q12;
          bufInA[cs + 3] = height_q12;
        }
  };

  fill_params();
  bo_inA.sync(XCL_BO_SYNC_BO_TO_DEVICE);
  auto warmup = kernel(opcode, 0, 0, bo_inA, bo_out);
  warmup.wait2();

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_CreateWindowAndRenderer("Perlin Noise NPU", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer);

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
        snprintf(data, 200, "Perlin Noise NPU FPS: %u", frames);
        SDL_SetWindowTitle(window, data);
        frames = 0;
        last_time = SDL_GetTicks();
    }

    auto t = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float>(t - t0).count();
    t0 = t;

    time_q12 += (int32_t)(40.0f * dt * 4096.0f);
    float phase = (float)time_q12 * 0.0000005f;
    scale_q12 = (int32_t)((0.2f + 0.2f * (0.5f + 0.5f * sinf(phase))) * 4096.0f);
    height_q12 = 4096;

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
