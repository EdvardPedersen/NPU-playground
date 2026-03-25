module {
 aie.device(npu2) {
  %tile22 = aie.tile(2, 2)
  aie.runtime_sequence() {
    //aiex.npu.sync { column = 2, row = 0, direction = 0, channel = 0 }
  }
 }
}
