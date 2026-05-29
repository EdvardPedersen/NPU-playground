module {
  aie.device(npu2) {
    %logical_core = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_0 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_1 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_2 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_3 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_4 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_5 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_6 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_7 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_8 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_9 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_10 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_11 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_12 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_13 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_14 = aie.logical_tile<CoreTile>(?, ?)
    %logical_shim_noc = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_15 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_16 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_17 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_18 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_19 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_20 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_21 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_22 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_23 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_24 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_25 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_26 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_27 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_28 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_29 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_30 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_31 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_32 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_33 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_34 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_35 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_36 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_37 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_38 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_39 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_40 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_41 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_42 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_43 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_44 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_shim_noc_45 = aie.logical_tile<ShimNOCTile>(?, ?)
    aie.objectfifo @in0_0(%logical_shim_noc, {%logical_core}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in0_1(%logical_shim_noc_15, {%logical_core_0}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in1_0(%logical_shim_noc_16, {%logical_core_1}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in1_1(%logical_shim_noc_17, {%logical_core_2}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in2_0(%logical_shim_noc_18, {%logical_core_3}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in2_1(%logical_shim_noc_19, {%logical_core_4}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in3_0(%logical_shim_noc_20, {%logical_core_5}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in3_1(%logical_shim_noc_21, {%logical_core_6}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in4_0(%logical_shim_noc_22, {%logical_core_7}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in4_1(%logical_shim_noc_23, {%logical_core_8}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in5_0(%logical_shim_noc_24, {%logical_core_9}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in5_1(%logical_shim_noc_25, {%logical_core_10}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in6_0(%logical_shim_noc_26, {%logical_core_11}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in6_1(%logical_shim_noc_27, {%logical_core_12}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in7_0(%logical_shim_noc_28, {%logical_core_13}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in7_1(%logical_shim_noc_29, {%logical_core_14}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out0_0(%logical_core, {%logical_shim_noc_30}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out0_1(%logical_core_0, {%logical_shim_noc_31}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out1_0(%logical_core_1, {%logical_shim_noc_32}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out1_1(%logical_core_2, {%logical_shim_noc_33}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out2_0(%logical_core_3, {%logical_shim_noc_34}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out2_1(%logical_core_4, {%logical_shim_noc_35}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out3_0(%logical_core_5, {%logical_shim_noc_36}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out3_1(%logical_core_6, {%logical_shim_noc_37}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out4_0(%logical_core_7, {%logical_shim_noc_38}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out4_1(%logical_core_8, {%logical_shim_noc_39}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out5_0(%logical_core_9, {%logical_shim_noc_40}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out5_1(%logical_core_10, {%logical_shim_noc_41}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out6_0(%logical_core_11, {%logical_shim_noc_42}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out6_1(%logical_core_12, {%logical_shim_noc_43}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out7_0(%logical_core_13, {%logical_shim_noc_44}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out7_1(%logical_core_14, {%logical_shim_noc_45}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    func.func private @passThroughLine(memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) attributes {link_with = "kernel.o"}
    %0 = aie.core(%logical_core) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out0_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in0_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c0_i32 = arith.constant 0 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c0_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in0_0(Consume, 1)
          aie.objectfifo.release @out0_0(Produce, 1)
        }
      }
      aie.end
    }
    %1 = aie.core(%logical_core_0) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out0_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in0_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c1_i32 = arith.constant 1 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c1_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in0_1(Consume, 1)
          aie.objectfifo.release @out0_1(Produce, 1)
        }
      }
      aie.end
    }
    %2 = aie.core(%logical_core_1) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out1_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in1_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c2_i32 = arith.constant 2 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c2_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in1_0(Consume, 1)
          aie.objectfifo.release @out1_0(Produce, 1)
        }
      }
      aie.end
    }
    %3 = aie.core(%logical_core_2) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out1_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in1_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c3_i32 = arith.constant 3 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c3_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in1_1(Consume, 1)
          aie.objectfifo.release @out1_1(Produce, 1)
        }
      }
      aie.end
    }
    %4 = aie.core(%logical_core_3) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out2_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in2_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c4_i32 = arith.constant 4 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c4_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in2_0(Consume, 1)
          aie.objectfifo.release @out2_0(Produce, 1)
        }
      }
      aie.end
    }
    %5 = aie.core(%logical_core_4) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out2_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in2_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c5_i32 = arith.constant 5 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c5_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in2_1(Consume, 1)
          aie.objectfifo.release @out2_1(Produce, 1)
        }
      }
      aie.end
    }
    %6 = aie.core(%logical_core_5) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out3_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in3_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c6_i32 = arith.constant 6 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c6_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in3_0(Consume, 1)
          aie.objectfifo.release @out3_0(Produce, 1)
        }
      }
      aie.end
    }
    %7 = aie.core(%logical_core_6) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out3_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in3_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c7_i32 = arith.constant 7 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c7_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in3_1(Consume, 1)
          aie.objectfifo.release @out3_1(Produce, 1)
        }
      }
      aie.end
    }
    %8 = aie.core(%logical_core_7) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out4_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in4_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c8_i32 = arith.constant 8 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c8_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in4_0(Consume, 1)
          aie.objectfifo.release @out4_0(Produce, 1)
        }
      }
      aie.end
    }
    %9 = aie.core(%logical_core_8) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out4_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in4_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c9_i32 = arith.constant 9 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c9_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in4_1(Consume, 1)
          aie.objectfifo.release @out4_1(Produce, 1)
        }
      }
      aie.end
    }
    %10 = aie.core(%logical_core_9) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out5_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in5_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c10_i32 = arith.constant 10 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c10_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in5_0(Consume, 1)
          aie.objectfifo.release @out5_0(Produce, 1)
        }
      }
      aie.end
    }
    %11 = aie.core(%logical_core_10) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out5_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in5_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c11_i32 = arith.constant 11 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c11_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in5_1(Consume, 1)
          aie.objectfifo.release @out5_1(Produce, 1)
        }
      }
      aie.end
    }
    %12 = aie.core(%logical_core_11) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out6_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in6_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c12_i32 = arith.constant 12 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c12_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in6_0(Consume, 1)
          aie.objectfifo.release @out6_0(Produce, 1)
        }
      }
      aie.end
    }
    %13 = aie.core(%logical_core_12) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out6_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in6_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c13_i32 = arith.constant 13 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c13_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in6_1(Consume, 1)
          aie.objectfifo.release @out6_1(Produce, 1)
        }
      }
      aie.end
    }
    %14 = aie.core(%logical_core_13) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out7_0(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in7_0(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c14_i32 = arith.constant 14 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c14_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in7_0(Consume, 1)
          aie.objectfifo.release @out7_0(Produce, 1)
        }
      }
      aie.end
    }
    %15 = aie.core(%logical_core_14) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_46 = arith.constant 0 : index
        %c64 = arith.constant 64 : index
        %c1_47 = arith.constant 1 : index
        scf.for %arg1 = %c0_46 to %c64 step %c1_47 {
          %16 = aie.objectfifo.acquire @out7_1(Produce, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %17 = aie.objectfifo.subview.access %16[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %18 = aie.objectfifo.acquire @in7_1(Consume, 1) : !aie.objectfifosubview<memref<1024xi32>>
          %19 = aie.objectfifo.subview.access %18[0] : !aie.objectfifosubview<memref<1024xi32>> -> memref<1024xi32>
          %c1024_i32 = arith.constant 1024 : i32
          %c15_i32 = arith.constant 15 : i32
          %c64_i32 = arith.constant 64 : i32
          %c1024_i32_48 = arith.constant 1024 : i32
          %c1024_i32_49 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%19, %17, %c1024_i32, %c15_i32, %arg1, %c64_i32, %c1024_i32_48, %c1024_i32_49, %cst) : (memref<1024xi32>, memref<1024xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in7_1(Consume, 1)
          aie.objectfifo.release @out7_1(Produce, 1)
        }
      }
      aie.end
    }
    aie.runtime_sequence(%arg0: memref<1048576xi32>, %arg1: memref<1048576xi32>) {
      %16 = aiex.dma_configure_task_for @in0_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 0, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%16)
      %17 = aiex.dma_configure_task_for @in0_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 65536, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%17)
      %18 = aiex.dma_configure_task_for @in1_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 131072, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%18)
      %19 = aiex.dma_configure_task_for @in1_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 196608, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%19)
      %20 = aiex.dma_configure_task_for @in2_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 262144, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%20)
      %21 = aiex.dma_configure_task_for @in2_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 327680, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%21)
      %22 = aiex.dma_configure_task_for @in3_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 393216, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%22)
      %23 = aiex.dma_configure_task_for @in3_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 458752, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%23)
      %24 = aiex.dma_configure_task_for @in4_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 524288, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%24)
      %25 = aiex.dma_configure_task_for @in4_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 589824, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%25)
      %26 = aiex.dma_configure_task_for @in5_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 655360, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%26)
      %27 = aiex.dma_configure_task_for @in5_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 720896, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%27)
      %28 = aiex.dma_configure_task_for @in6_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 786432, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%28)
      %29 = aiex.dma_configure_task_for @in6_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 851968, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%29)
      %30 = aiex.dma_configure_task_for @in7_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 917504, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%30)
      %31 = aiex.dma_configure_task_for @in7_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 983040, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%31)
      %32 = aiex.dma_configure_task_for @out0_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 0, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%32)
      %33 = aiex.dma_configure_task_for @out0_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 65536, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%33)
      %34 = aiex.dma_configure_task_for @out1_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 131072, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%34)
      %35 = aiex.dma_configure_task_for @out1_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 196608, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%35)
      %36 = aiex.dma_configure_task_for @out2_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 262144, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%36)
      %37 = aiex.dma_configure_task_for @out2_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 327680, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%37)
      %38 = aiex.dma_configure_task_for @out3_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 393216, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%38)
      %39 = aiex.dma_configure_task_for @out3_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 458752, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%39)
      %40 = aiex.dma_configure_task_for @out4_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 524288, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%40)
      %41 = aiex.dma_configure_task_for @out4_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 589824, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%41)
      %42 = aiex.dma_configure_task_for @out5_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 655360, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%42)
      %43 = aiex.dma_configure_task_for @out5_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 720896, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%43)
      %44 = aiex.dma_configure_task_for @out6_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 786432, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%44)
      %45 = aiex.dma_configure_task_for @out6_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 851968, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%45)
      %46 = aiex.dma_configure_task_for @out7_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 917504, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%46)
      %47 = aiex.dma_configure_task_for @out7_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 983040, 65536, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 65536, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%47)
      aiex.dma_await_task(%32)
      aiex.dma_await_task(%33)
      aiex.dma_await_task(%34)
      aiex.dma_await_task(%35)
      aiex.dma_await_task(%36)
      aiex.dma_await_task(%37)
      aiex.dma_await_task(%38)
      aiex.dma_await_task(%39)
      aiex.dma_await_task(%40)
      aiex.dma_await_task(%41)
      aiex.dma_await_task(%42)
      aiex.dma_await_task(%43)
      aiex.dma_await_task(%44)
      aiex.dma_await_task(%45)
      aiex.dma_await_task(%46)
      aiex.dma_await_task(%47)
      aiex.dma_free_task(%16)
      aiex.dma_free_task(%17)
      aiex.dma_free_task(%18)
      aiex.dma_free_task(%19)
      aiex.dma_free_task(%20)
      aiex.dma_free_task(%21)
      aiex.dma_free_task(%22)
      aiex.dma_free_task(%23)
      aiex.dma_free_task(%24)
      aiex.dma_free_task(%25)
      aiex.dma_free_task(%26)
      aiex.dma_free_task(%27)
      aiex.dma_free_task(%28)
      aiex.dma_free_task(%29)
      aiex.dma_free_task(%30)
      aiex.dma_free_task(%31)
    }
  }
}

