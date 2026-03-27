module @mandelbrot {
    aie.device(npu2) {
        %tile14 = aie.tile(0, 2)
        %buf = aie.buffer(%tile14) : memref<256xf32>
        func.func private @mandelbrot_kernel(%b: memref<256xf32>) -> () attributes {link_with = "kernel.o"}
        %core14 = aie.core(%tile14) {
            func.call @mandelbrot_kernel(%buf) : (memref<256xf32>) -> ()
            aie.end
        }
    }
}

