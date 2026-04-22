module {
  aie.device(npu2) {
    %tile_0_2 = aie.tile(0, 2)
    %tile_0_3 = aie.tile(0, 3)
    %tile_0_4 = aie.tile(0, 4)
    %tile_0_5 = aie.tile(0, 5)
    %tile_1_2 = aie.tile(1, 2)
    %tile_1_3 = aie.tile(1, 3)
    %tile_1_4 = aie.tile(1, 4)
    %tile_1_5 = aie.tile(1, 5)
    %tile_2_2 = aie.tile(2, 2)
    %tile_2_3 = aie.tile(2, 3)
    %tile_2_4 = aie.tile(2, 4)
    %tile_2_5 = aie.tile(2, 5)
    %tile_3_2 = aie.tile(3, 2)
    %tile_3_3 = aie.tile(3, 3)
    %tile_3_4 = aie.tile(3, 4)
    %tile_3_5 = aie.tile(3, 5)
    %shim_noc_tile_0_0 = aie.tile(0, 0)
    %shim_noc_tile_1_0 = aie.tile(1, 0)
    %shim_noc_tile_2_0 = aie.tile(2, 0)
    %shim_noc_tile_3_0 = aie.tile(3, 0)
    %shim_noc_tile_4_0 = aie.tile(4, 0)
    %shim_noc_tile_5_0 = aie.tile(5, 0)
    %shim_noc_tile_6_0 = aie.tile(6, 0)
    %shim_noc_tile_7_0 = aie.tile(7, 0)
    aie.objectfifo @in0_0(%shim_noc_tile_0_0, {%tile_0_2}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in0_1(%shim_noc_tile_0_0, {%tile_0_3}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in1_0(%shim_noc_tile_1_0, {%tile_0_4}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in1_1(%shim_noc_tile_1_0, {%tile_0_5}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in2_0(%shim_noc_tile_2_0, {%tile_1_2}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in2_1(%shim_noc_tile_2_0, {%tile_1_3}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in3_0(%shim_noc_tile_3_0, {%tile_1_4}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in3_1(%shim_noc_tile_3_0, {%tile_1_5}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in4_0(%shim_noc_tile_4_0, {%tile_2_2}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in4_1(%shim_noc_tile_4_0, {%tile_2_3}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in5_0(%shim_noc_tile_5_0, {%tile_2_4}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in5_1(%shim_noc_tile_5_0, {%tile_2_5}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in6_0(%shim_noc_tile_6_0, {%tile_3_2}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in6_1(%shim_noc_tile_6_0, {%tile_3_3}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in7_0(%shim_noc_tile_7_0, {%tile_3_4}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in7_1(%shim_noc_tile_7_0, {%tile_3_5}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out0_0(%tile_0_2, {%shim_noc_tile_0_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out0_1(%tile_0_3, {%shim_noc_tile_0_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out1_0(%tile_0_4, {%shim_noc_tile_1_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out1_1(%tile_0_5, {%shim_noc_tile_1_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out2_0(%tile_1_2, {%shim_noc_tile_2_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out2_1(%tile_1_3, {%shim_noc_tile_2_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out3_0(%tile_1_4, {%shim_noc_tile_3_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out3_1(%tile_1_5, {%shim_noc_tile_3_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out4_0(%tile_2_2, {%shim_noc_tile_4_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out4_1(%tile_2_3, {%shim_noc_tile_4_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out5_0(%tile_2_4, {%shim_noc_tile_5_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out5_1(%tile_2_5, {%shim_noc_tile_5_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out6_0(%tile_3_2, {%shim_noc_tile_6_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out6_1(%tile_3_3, {%shim_noc_tile_6_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out7_0(%tile_3_4, {%shim_noc_tile_7_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out7_1(%tile_3_5, {%shim_noc_tile_7_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    func.func private @passThroughLine(memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) attributes {link_with = "kernel.o"}
    %core_0_2 = aie.core(%tile_0_2) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out0_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in0_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c0_i32 = arith.constant 0 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c0_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in0_0(Consume, 1)
          aie.objectfifo.release @out0_0(Produce, 1)
        }
      }
      aie.end
    }
    %core_0_3 = aie.core(%tile_0_3) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out0_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in0_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c1_i32 = arith.constant 1 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c1_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in0_1(Consume, 1)
          aie.objectfifo.release @out0_1(Produce, 1)
        }
      }
      aie.end
    }
    %core_0_4 = aie.core(%tile_0_4) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out1_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in1_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c2_i32 = arith.constant 2 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c2_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in1_0(Consume, 1)
          aie.objectfifo.release @out1_0(Produce, 1)
        }
      }
      aie.end
    }
    %core_0_5 = aie.core(%tile_0_5) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out1_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in1_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c3_i32 = arith.constant 3 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c3_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in1_1(Consume, 1)
          aie.objectfifo.release @out1_1(Produce, 1)
        }
      }
      aie.end
    }
    %core_1_2 = aie.core(%tile_1_2) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out2_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in2_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c4_i32 = arith.constant 4 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c4_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in2_0(Consume, 1)
          aie.objectfifo.release @out2_0(Produce, 1)
        }
      }
      aie.end
    }
    %core_1_3 = aie.core(%tile_1_3) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out2_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in2_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c5_i32 = arith.constant 5 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c5_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in2_1(Consume, 1)
          aie.objectfifo.release @out2_1(Produce, 1)
        }
      }
      aie.end
    }
    %core_1_4 = aie.core(%tile_1_4) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out3_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in3_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c6_i32 = arith.constant 6 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c6_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in3_0(Consume, 1)
          aie.objectfifo.release @out3_0(Produce, 1)
        }
      }
      aie.end
    }
    %core_1_5 = aie.core(%tile_1_5) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out3_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in3_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c7_i32 = arith.constant 7 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c7_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in3_1(Consume, 1)
          aie.objectfifo.release @out3_1(Produce, 1)
        }
      }
      aie.end
    }
    %core_2_2 = aie.core(%tile_2_2) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out4_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in4_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c8_i32 = arith.constant 8 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c8_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in4_0(Consume, 1)
          aie.objectfifo.release @out4_0(Produce, 1)
        }
      }
      aie.end
    }
    %core_2_3 = aie.core(%tile_2_3) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out4_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in4_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c9_i32 = arith.constant 9 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c9_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in4_1(Consume, 1)
          aie.objectfifo.release @out4_1(Produce, 1)
        }
      }
      aie.end
    }
    %core_2_4 = aie.core(%tile_2_4) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out5_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in5_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c10_i32 = arith.constant 10 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c10_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in5_0(Consume, 1)
          aie.objectfifo.release @out5_0(Produce, 1)
        }
      }
      aie.end
    }
    %core_2_5 = aie.core(%tile_2_5) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out5_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in5_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c11_i32 = arith.constant 11 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c11_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in5_1(Consume, 1)
          aie.objectfifo.release @out5_1(Produce, 1)
        }
      }
      aie.end
    }
    %core_3_2 = aie.core(%tile_3_2) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out6_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in6_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c12_i32 = arith.constant 12 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c12_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in6_0(Consume, 1)
          aie.objectfifo.release @out6_0(Produce, 1)
        }
      }
      aie.end
    }
    %core_3_3 = aie.core(%tile_3_3) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out6_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in6_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c13_i32 = arith.constant 13 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c13_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in6_1(Consume, 1)
          aie.objectfifo.release @out6_1(Produce, 1)
        }
      }
      aie.end
    }
    %core_3_4 = aie.core(%tile_3_4) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out7_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in7_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c14_i32 = arith.constant 14 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c14_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in7_0(Consume, 1)
          aie.objectfifo.release @out7_0(Produce, 1)
        }
      }
      aie.end
    }
    %core_3_5 = aie.core(%tile_3_5) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_0 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_1 = arith.constant 1 : index
        scf.for %arg1 = %c0_0 to %c64 step %c1_1 {
          %0 = aie.objectfifo.acquire @out7_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %1 = aie.objectfifo.subview.access %0[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %2 = aie.objectfifo.acquire @in7_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %3 = aie.objectfifo.subview.access %2[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c15_i32 = arith.constant 15 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_2 = arith.constant 1024 : i32
          %c1024_i32_3 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%3, %1, %c1024_i32, %c15_i32, %arg1, %c64_i32, %c1024_i32_2, %c1024_i32_3, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in7_1(Consume, 1)
          aie.objectfifo.release @out7_1(Produce, 1)
        }
      }
      aie.end
    }
    aie.runtime_sequence(%arg0: memref<1048576xi32>, %arg1: memref<1048576xi32>) {
      %0 = aiex.dma_configure_task_for @in0_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 0, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%0)
      %1 = aiex.dma_configure_task_for @in0_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 65536, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%1)
      %2 = aiex.dma_configure_task_for @in1_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 131072, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%2)
      %3 = aiex.dma_configure_task_for @in1_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 196608, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%3)
      %4 = aiex.dma_configure_task_for @in2_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 262144, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%4)
      %5 = aiex.dma_configure_task_for @in2_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 327680, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%5)
      %6 = aiex.dma_configure_task_for @in3_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 393216, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%6)
      %7 = aiex.dma_configure_task_for @in3_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 458752, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%7)
      %8 = aiex.dma_configure_task_for @in4_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 524288, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%8)
      %9 = aiex.dma_configure_task_for @in4_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 589824, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%9)
      %10 = aiex.dma_configure_task_for @in5_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 655360, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%10)
      %11 = aiex.dma_configure_task_for @in5_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 720896, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%11)
      %12 = aiex.dma_configure_task_for @in6_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 786432, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%12)
      %13 = aiex.dma_configure_task_for @in6_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 851968, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%13)
      %14 = aiex.dma_configure_task_for @in7_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 917504, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%14)
      %15 = aiex.dma_configure_task_for @in7_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 983040, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%15)
      %16 = aiex.dma_configure_task_for @out0_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 0, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%16)
      %17 = aiex.dma_configure_task_for @out0_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 65536, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%17)
      %18 = aiex.dma_configure_task_for @out1_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 131072, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%18)
      %19 = aiex.dma_configure_task_for @out1_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 196608, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%19)
      %20 = aiex.dma_configure_task_for @out2_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 262144, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%20)
      %21 = aiex.dma_configure_task_for @out2_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 327680, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%21)
      %22 = aiex.dma_configure_task_for @out3_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 393216, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%22)
      %23 = aiex.dma_configure_task_for @out3_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 458752, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%23)
      %24 = aiex.dma_configure_task_for @out4_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 524288, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%24)
      %25 = aiex.dma_configure_task_for @out4_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 589824, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%25)
      %26 = aiex.dma_configure_task_for @out5_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 655360, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%26)
      %27 = aiex.dma_configure_task_for @out5_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 720896, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%27)
      %28 = aiex.dma_configure_task_for @out6_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 786432, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%28)
      %29 = aiex.dma_configure_task_for @out6_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 851968, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%29)
      %30 = aiex.dma_configure_task_for @out7_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 917504, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%30)
      %31 = aiex.dma_configure_task_for @out7_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 983040, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%31)
      aiex.dma_await_task(%16)
      aiex.dma_await_task(%17)
      aiex.dma_await_task(%18)
      aiex.dma_await_task(%19)
      aiex.dma_await_task(%20)
      aiex.dma_await_task(%21)
      aiex.dma_await_task(%22)
      aiex.dma_await_task(%23)
      aiex.dma_await_task(%24)
      aiex.dma_await_task(%25)
      aiex.dma_await_task(%26)
      aiex.dma_await_task(%27)
      aiex.dma_await_task(%28)
      aiex.dma_await_task(%29)
      aiex.dma_await_task(%30)
      aiex.dma_await_task(%31)
      aiex.dma_free_task(%0)
      aiex.dma_free_task(%1)
      aiex.dma_free_task(%2)
      aiex.dma_free_task(%3)
      aiex.dma_free_task(%4)
      aiex.dma_free_task(%5)
      aiex.dma_free_task(%6)
      aiex.dma_free_task(%7)
      aiex.dma_free_task(%8)
      aiex.dma_free_task(%9)
      aiex.dma_free_task(%10)
      aiex.dma_free_task(%11)
      aiex.dma_free_task(%12)
      aiex.dma_free_task(%13)
      aiex.dma_free_task(%14)
      aiex.dma_free_task(%15)
    }
  }
}

