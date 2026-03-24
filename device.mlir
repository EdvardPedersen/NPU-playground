module @mandelbrot {

    aie.device(npu1_1col) {
        %shim_noc_tile_0_0 = aie.tile(0, 0)
        %tile14 = aie.tile(0, 2)
    }
    %tile14 = aie.tile(0, 2)
    %buf = aie.buffer(%tile14) { sym_name = "a14" } : memref<256xi32>
    func.func private @mandelbrot_kernel(%b: memref<256xi32>) -> () attributes {link_with = "kernel.o"}
    %core14 = aie.core(%tile14) {
        func.call @mandelbrot_kernel(%buf) : (memref<256xi32>) -> ()
        aie.end
    }
}
