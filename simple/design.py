import numpy as np
import argparse
import sys

from aie.iron import Kernel, ObjectFifo, Program, Runtime, Worker
from aie.iron.placers import SequentialPlacer
from aie.iron.device import Tile, NPU1, NPU2
from aie.helpers.taplib.tap import TensorAccessPattern
from aie.iron.controlflow import range_


def my_memcpy(dev, size, num_columns, num_channels, bypass):
    # --------------------------------------------------------------------------
    # Configuration
    # --------------------------------------------------------------------------

    # Use int32 dtype as it is the addr generation granularity
    xfr_dtype = np.int32

    # Define tensor types

    elems_per_tile = 1024
    splits = size // elems_per_tile
    line_size = int(size / splits) // 16
    line_type = np.ndarray[(line_size,), np.dtype[xfr_dtype]]
    transfer_type = np.ndarray[(size,), np.dtype[xfr_dtype]]

    # Chunk size sent per DMA channel
    chunk = size // num_columns // num_channels

    # --------------------------------------------------------------------------
    # In-Array Data Movement
    # --------------------------------------------------------------------------

    of_ins = [
        ObjectFifo(line_type, name=f"in{i}_{j}")
        for i in range(num_columns)
        for j in range(num_channels)
    ]
    of_outs = [
        ObjectFifo(line_type, name=f"out{i}_{j}")
        for i in range(num_columns)
        for j in range(num_channels)
    ]

    # --------------------------------------------------------------------------
    # Task core will run
    # --------------------------------------------------------------------------

    # External, binary kernel definition
    passthrough_fn = Kernel(
        "passThroughLine",
        "kernel.o",
        [line_type, line_type, np.int32, np.int32, np.uint64],
    )

    # Task for the core to perform
    def core_fn(of_in, of_out, passThroughLine, node):
        for i in range_(splits):
          elemOut = of_out.acquire(1)
          elemIn = of_in.acquire(1)
          passThroughLine(elemIn, elemOut, line_size, node, i)
          of_in.release(1)
          of_out.release(1)

    # Create a worker to perform the task
    my_workers = [
        Worker(
            core_fn,
            [
                of_ins[i * num_channels + j].cons(),
                of_outs[i * num_channels + j].prod(),
                passthrough_fn,
                i * num_channels + j,
            ],
        )
        for i in range(num_columns)
        for j in range(num_channels)
    ]

    # --------------------------------------------------------------------------
    # DRAM-NPU data movement and work dispatch
    # --------------------------------------------------------------------------

    # Create a TensorAccessPattern for each channel to describe the data movement.
    # The pattern chops the data in equal chunks and moves them in parallel across
    # the columns and channels.
    taps = [
        TensorAccessPattern(
            (1, size),
            chunk * i * num_channels + chunk * j,
            [1, 1, 1, chunk],
            [0, 0, 0, 1],
        )
        for i in range(num_columns)
        for j in range(num_channels)
    ]

    # Runtime operations to move data to/from the AIE-array
    # START EXERCISE: Modify the code below to use task groups
    rt = Runtime()
    with rt.sequence(transfer_type, transfer_type) as (a_in, b_out):
        # Start the workers if not bypass
        rt.start(*my_workers)
        # Fill the input objectFIFOs with data
        for i in range(num_columns):
            for j in range(num_channels):
                rt.fill(
                    of_ins[i * num_channels + j].prod(),
                    a_in,
                    taps[i * num_channels + j],
                )
        # Drain the output objectFIFOs with data
        for i in range(num_columns):
            for j in range(num_channels):
                rt.drain(
                    of_outs[i * num_channels + j].cons(),
                    b_out,
                    taps[i * num_channels + j],
                    wait=True,  # wait for the transfer to complete and data to be available
                )
    # END EXERCISE

    # Place components (assign them resources on the device) and generate an MLIR module
    return Program(dev, rt).resolve_program(SequentialPlacer())

## Call the my_memcpy function with the parsed arguments
## and print the MLIR as a result
print(my_memcpy(NPU2(), 128*128 , 8, 2, False))
