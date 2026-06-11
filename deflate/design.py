import numpy as np

from aie.iron import Kernel, ObjectFifo, Program, Runtime, Worker
from aie.iron.device import NPU2
from aie.helpers.taplib.tap import TensorAccessPattern
from aie.iron.controlflow import range_

# Symmetric tiles (input == output == 256 int32 = 1024 bytes), exactly like the
# proven mandelbrot design.  Each tile carries DFL_CHUNK (1008) data bytes that
# the core compresses; the core writes a length header + stream back into the
# same-size output tile.
TILE = 256              # int32 elems per core tile (1024 bytes)
CORES_PER_COL = 4
NUM_COLUMNS = 8
COL = TILE * CORES_PER_COL  # 1024 int32 per column tile
CHUNK_BYTES = 1008          # DFL_CHUNK: data bytes compressed per tile


def deflate_design(dev, splits):
    chunk_per_col = COL * splits
    total = chunk_per_col * NUM_COLUMNS

    xfr_type = np.ndarray[(total,), np.dtype[np.int32]]
    col_type = np.ndarray[(COL,), np.dtype[np.int32]]
    core_type = np.ndarray[(TILE,), np.dtype[np.int32]]

    cols_in, cols_out, cores_in, cores_out = [], [], [], []
    for i in range(NUM_COLUMNS):
        of_in = ObjectFifo(col_type, name=f"in_col_{i}")
        of_out = ObjectFifo(col_type, name=f"out_col_{i}")
        offsets = [j * TILE for j in range(CORES_PER_COL)]
        core_in = of_in.cons().split(
            offsets, obj_types=[core_type] * CORES_PER_COL,
            names=[f"in_core_{i}_{j}" for j in range(CORES_PER_COL)],
        )
        core_out = of_out.prod().join(
            offsets, obj_types=[core_type] * CORES_PER_COL,
            names=[f"out_core_{i}_{j}" for j in range(CORES_PER_COL)],
        )
        cols_in.append(of_in)
        cols_out.append(of_out)
        cores_in.append(core_in)
        cores_out.append(core_out)

    deflate_fn = Kernel("deflateChunk", "kernel.o", [core_type, core_type, np.int32])

    def core_fn(of_in, of_out, deflateChunk):
        for _ in range_(splits):
            eout = of_out.acquire(1)
            ein = of_in.acquire(1)
            deflateChunk(ein, eout, CHUNK_BYTES)
            of_in.release(1)
            of_out.release(1)

    workers = [
        Worker(core_fn, [cores_in[i][j].cons(), cores_out[i][j].prod(), deflate_fn])
        for i in range(NUM_COLUMNS)
        for j in range(CORES_PER_COL)
    ]

    taps = [
        TensorAccessPattern(
            (1, total), i * chunk_per_col,
            [1, 1, 1, chunk_per_col], [0, 0, 0, 1],
        )
        for i in range(NUM_COLUMNS)
    ]

    rt = Runtime()
    with rt.sequence(xfr_type, xfr_type) as (a_in, b_out):
        rt.start(*workers)
        for i in range(NUM_COLUMNS):
            rt.fill(cols_in[i].prod(), a_in, taps[i])
        for i in range(NUM_COLUMNS):
            rt.drain(cols_out[i].cons(), b_out, taps[i], wait=True)

    return Program(dev, rt).resolve_program()


# splits = compress iterations per core per kernel invocation.  Kept tiny (2,
# one per ping/pong buffer) so a command's total compute is well-bounded and the
# drain cannot complete before the cores have produced their tiles.  At larger
# splits the command returned with partial/stale mem-tile data.  The host covers
# the whole corpus with NUM_SLICES sequential invocations.
print(deflate_design(NPU2(), 2))
