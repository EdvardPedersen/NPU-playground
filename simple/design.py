import numpy as np
import argparse
import sys

from aie.iron import Kernel, ObjectFifo, Program, Runtime, Worker
from aie.iron.device import NPU1, NPU2
from aie.helpers.taplib.tap import TensorAccessPattern
from aie.iron.controlflow import range_


def my_memcpy(dev, image_width, image_height, num_columns, bypass):
    size = image_width * image_height

    xfr_dtype = np.int32
    transfer_type = np.ndarray[(size,), np.dtype[xfr_dtype]]

    cores_per_col = 4
    tile_size = 256
    col_tile_size = tile_size * cores_per_col  # 1024

    chunk_per_col = size // num_columns
    chunk_per_core = chunk_per_col // cores_per_col
    splits = chunk_per_core // tile_size

    col_type = np.ndarray[(col_tile_size,), np.dtype[xfr_dtype]]
    core_type = np.ndarray[(tile_size,), np.dtype[xfr_dtype]]

    of_ins = []
    of_outs = []

    for i in range(num_columns):
        of_in = ObjectFifo(col_type, name=f"in_col_{i}")
        of_out = ObjectFifo(col_type, name=f"out_col_{i}")

        core_offsets = [j * tile_size for j in range(cores_per_col)]
        core_in_fifos = of_in.cons().split(
            core_offsets,
            obj_types=[core_type] * cores_per_col,
            names=[f"in_core_{i}_{j}" for j in range(cores_per_col)],
        )
        core_out_fifos = of_out.prod().join(
            core_offsets,
            obj_types=[core_type] * cores_per_col,
            names=[f"out_core_{i}_{j}" for j in range(cores_per_col)],
        )

        of_ins.append((of_in, core_in_fifos))
        of_outs.append((of_out, core_out_fifos))

    passthrough_fn = Kernel(
        "passThroughLine",
        "kernel.o",
        [core_type, core_type, np.int32, np.int32, np.uint64, np.int32, np.int32, np.int32, np.float32],
    )

    def core_fn(of_in, of_out, passThroughLine, node):
        col = node // cores_per_col
        row = node % cores_per_col
        for it in range_(splits):
            elemOut = of_out.acquire(1)
            elemIn = of_in.acquire(1)
            global_start = col * chunk_per_col + it * col_tile_size + row * tile_size
            passThroughLine(elemIn, elemOut, tile_size, 0, global_start // tile_size, 1, image_width, image_height, 1.0)
            of_in.release(1)
            of_out.release(1)

    my_workers = [
        Worker(
            core_fn,
            [
                of_ins[i][1][j].cons(),
                of_outs[i][1][j].prod(),
                passthrough_fn,
                i * cores_per_col + j,
            ],
        )
        for i in range(num_columns)
        for j in range(cores_per_col)
    ]

    taps_in = [
        TensorAccessPattern(
            (1, size),
            i * chunk_per_col,
            [1, 1, 1, chunk_per_col],
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
    with rt.sequence(transfer_type, transfer_type) as (a_in, b_out):
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

print(my_memcpy(NPU2(), 1024, 1024, 8, False))
