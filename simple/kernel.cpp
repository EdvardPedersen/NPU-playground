//===- passThrough.cc -------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Copyright (C) 2022, Advanced Micro Devices, Inc.
//
//===----------------------------------------------------------------------===//

// #define __AIENGINE__ 1
#define NOCPP

#include <stdint.h>
#include <stdlib.h>

extern "C" {


void passThroughLine(int32_t *in, int32_t *out, int32_t lineWidth) {
    int32_t *inPtr = in;
    int32_t *outPtr = out;
    for(int i = 0; i < lineWidth; i++) {
        *outPtr++ = 10;
    }
}
} // extern "C"
