import numpy as np
import argparse
import sys

from aie.iron import Kernel, ObjectFifo, Program, Runtime, Worker
from aie.iron.device import NPU1, NPU2
from aie.helpers.taplib.tap import TensorAccessPattern
from aie.iron.controlflow import range_


PARAMS_PER_ROW = 16  # PERLIN_PARAMS in perlin_params.h


def perlin_noise_design(dev, image_width, image_height, num_columns):
    size = image_width * image_height
    in_size = PARAMS_PER_ROW * image_height

    xfr_dtype = np.int32
    in_type = np.ndarray[(in_size,), np.dtype[xfr_dtype]]
    out_type = np.ndarray[(size,), np.dtype[xfr_dtype]]

    cores_per_col = 4
    tile_size = image_width  # one row per kernel call, see perlin_params.h
    col_tile_size = tile_size * cores_per_col

    chunk_per_col = size // num_columns
    in_chunk_per_col = in_size // num_columns
    splits = image_height // (num_columns * cores_per_col)

    col_in_type = np.ndarray[(PARAMS_PER_ROW * cores_per_col,), np.dtype[xfr_dtype]]
    col_type = np.ndarray[(col_tile_size,), np.dtype[xfr_dtype]]
    core_in_type = np.ndarray[(PARAMS_PER_ROW,), np.dtype[xfr_dtype]]
    core_type = np.ndarray[(tile_size,), np.dtype[xfr_dtype]]

    of_ins = []
    of_outs = []

    for i in range(num_columns):
        of_in = ObjectFifo(col_in_type, name=f"in_col_{i}")
        of_out = ObjectFifo(col_type, name=f"out_col_{i}")

        core_offsets = [j * tile_size for j in range(cores_per_col)]
        core_in_fifos = of_in.cons().split(
            [j * PARAMS_PER_ROW for j in range(cores_per_col)],
            obj_types=[core_in_type] * cores_per_col,
            names=[f"in_core_{i}_{j}" for j in range(cores_per_col)],
        )
        core_out_fifos = of_out.prod().join(
            core_offsets,
            obj_types=[core_type] * cores_per_col,
            names=[f"out_core_{i}_{j}" for j in range(cores_per_col)],
        )

        of_ins.append((of_in, core_in_fifos))
        of_outs.append((of_out, core_out_fifos))

    perlin_fn = Kernel(
        "passThroughLine",
        "kernel.o",
        [core_in_type, core_type, np.int32],
    )

    def core_fn(of_in, of_out, passThroughLine):
        for it in range_(splits):
            elemOut = of_out.acquire(1)
            elemIn = of_in.acquire(1)
            passThroughLine(elemIn, elemOut, tile_size)
            of_in.release(1)
            of_out.release(1)

    my_workers = [
        Worker(
            core_fn,
            [
                of_ins[i][1][j].cons(),
                of_outs[i][1][j].prod(),
                perlin_fn,
            ],
            # The kernel's vector register spills exceed the default 1 KiB stack,
            # which silently corrupts the neighbouring buffers and hangs the NPU.
            stack_size=4096,
        )
        for i in range(num_columns)
        for j in range(cores_per_col)
    ]

    taps_in = [
        TensorAccessPattern(
            (1, in_size),
            i * in_chunk_per_col,
            [1, 1, 1, in_chunk_per_col],
            [0, 0, 0, 1],
        )
        for i in range(num_columns)
    ]
    taps_out = [
        TensorAccessPattern(
            (1, size),
            i * chunk_per_col,
            [1, 1, 1, chunk_per_col],
            [0, 0, 0, 1],
        )
        for i in range(num_columns)
    ]

    rt = Runtime()
    with rt.sequence(in_type, out_type) as (a_in, b_out):
        rt.start(*my_workers)
        for i in range(num_columns):
            rt.fill(
                of_ins[i][0].prod(),
                a_in,
                taps_in[i],
            )
        for i in range(num_columns):
            rt.drain(
                of_outs[i][0].cons(),
                b_out,
                taps_out[i],
                wait=True,
            )

    return Program(dev, rt).resolve_program()

print(perlin_noise_design(NPU2(), 4096, 4096, 8))  # PERLIN_W, PERLIN_H
