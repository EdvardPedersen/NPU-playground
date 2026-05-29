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
    %logical_core_15 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_16 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_17 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_18 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_19 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_20 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_21 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_22 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_23 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_24 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_25 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_26 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_27 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_28 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_29 = aie.logical_tile<CoreTile>(?, ?)
    %logical_core_30 = aie.logical_tile<CoreTile>(?, ?)
    %logical_shim_noc = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_31 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem_32 = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_33 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem_34 = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_35 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem_36 = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_37 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem_38 = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_39 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem_40 = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_41 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem_42 = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_43 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem_44 = aie.logical_tile<MemTile>(?, ?)
    %logical_mem_45 = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_46 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem_47 = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_48 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem_49 = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_50 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem_51 = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_52 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem_53 = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_54 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem_55 = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_56 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem_57 = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_58 = aie.logical_tile<ShimNOCTile>(?, ?)
    %logical_mem_59 = aie.logical_tile<MemTile>(?, ?)
    %logical_shim_noc_60 = aie.logical_tile<ShimNOCTile>(?, ?)
    aie.objectfifo @in_col_0(%logical_shim_noc, {%logical_mem}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in_core_0_0(%logical_mem, {%logical_core}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_0_1(%logical_mem, {%logical_core_0}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_0_2(%logical_mem, {%logical_core_1}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_0_3(%logical_mem, {%logical_core_2}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@in_col_0] -> [@in_core_0_0, @in_core_0_1, @in_core_0_2, @in_core_0_3]([] [0, 256, 512, 768])
    aie.objectfifo @in_col_1(%logical_shim_noc_31, {%logical_mem_32}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in_core_1_0(%logical_mem_32, {%logical_core_3}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_1_1(%logical_mem_32, {%logical_core_4}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_1_2(%logical_mem_32, {%logical_core_5}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_1_3(%logical_mem_32, {%logical_core_6}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@in_col_1] -> [@in_core_1_0, @in_core_1_1, @in_core_1_2, @in_core_1_3]([] [0, 256, 512, 768])
    aie.objectfifo @in_col_2(%logical_shim_noc_33, {%logical_mem_34}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in_core_2_0(%logical_mem_34, {%logical_core_7}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_2_1(%logical_mem_34, {%logical_core_8}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_2_2(%logical_mem_34, {%logical_core_9}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_2_3(%logical_mem_34, {%logical_core_10}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@in_col_2] -> [@in_core_2_0, @in_core_2_1, @in_core_2_2, @in_core_2_3]([] [0, 256, 512, 768])
    aie.objectfifo @in_col_3(%logical_shim_noc_35, {%logical_mem_36}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in_core_3_0(%logical_mem_36, {%logical_core_11}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_3_1(%logical_mem_36, {%logical_core_12}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_3_2(%logical_mem_36, {%logical_core_13}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_3_3(%logical_mem_36, {%logical_core_14}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@in_col_3] -> [@in_core_3_0, @in_core_3_1, @in_core_3_2, @in_core_3_3]([] [0, 256, 512, 768])
    aie.objectfifo @in_col_4(%logical_shim_noc_37, {%logical_mem_38}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in_core_4_0(%logical_mem_38, {%logical_core_15}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_4_1(%logical_mem_38, {%logical_core_16}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_4_2(%logical_mem_38, {%logical_core_17}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_4_3(%logical_mem_38, {%logical_core_18}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@in_col_4] -> [@in_core_4_0, @in_core_4_1, @in_core_4_2, @in_core_4_3]([] [0, 256, 512, 768])
    aie.objectfifo @in_col_5(%logical_shim_noc_39, {%logical_mem_40}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in_core_5_0(%logical_mem_40, {%logical_core_19}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_5_1(%logical_mem_40, {%logical_core_20}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_5_2(%logical_mem_40, {%logical_core_21}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_5_3(%logical_mem_40, {%logical_core_22}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@in_col_5] -> [@in_core_5_0, @in_core_5_1, @in_core_5_2, @in_core_5_3]([] [0, 256, 512, 768])
    aie.objectfifo @in_col_6(%logical_shim_noc_41, {%logical_mem_42}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in_core_6_0(%logical_mem_42, {%logical_core_23}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_6_1(%logical_mem_42, {%logical_core_24}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_6_2(%logical_mem_42, {%logical_core_25}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_6_3(%logical_mem_42, {%logical_core_26}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@in_col_6] -> [@in_core_6_0, @in_core_6_1, @in_core_6_2, @in_core_6_3]([] [0, 256, 512, 768])
    aie.objectfifo @in_col_7(%logical_shim_noc_43, {%logical_mem_44}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @in_core_7_0(%logical_mem_44, {%logical_core_27}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_7_1(%logical_mem_44, {%logical_core_28}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_7_2(%logical_mem_44, {%logical_core_29}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @in_core_7_3(%logical_mem_44, {%logical_core_30}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@in_col_7] -> [@in_core_7_0, @in_core_7_1, @in_core_7_2, @in_core_7_3]([] [0, 256, 512, 768])
    aie.objectfifo @out_col_0(%logical_mem_45, {%logical_shim_noc_46}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out_core_0_0(%logical_core, {%logical_mem_45}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_0_1(%logical_core_0, {%logical_mem_45}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_0_2(%logical_core_1, {%logical_mem_45}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_0_3(%logical_core_2, {%logical_mem_45}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@out_core_0_0, @out_core_0_1, @out_core_0_2, @out_core_0_3] -> [@out_col_0]([0, 256, 512, 768] [])
    aie.objectfifo @out_col_1(%logical_mem_47, {%logical_shim_noc_48}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out_core_1_0(%logical_core_3, {%logical_mem_47}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_1_1(%logical_core_4, {%logical_mem_47}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_1_2(%logical_core_5, {%logical_mem_47}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_1_3(%logical_core_6, {%logical_mem_47}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@out_core_1_0, @out_core_1_1, @out_core_1_2, @out_core_1_3] -> [@out_col_1]([0, 256, 512, 768] [])
    aie.objectfifo @out_col_2(%logical_mem_49, {%logical_shim_noc_50}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out_core_2_0(%logical_core_7, {%logical_mem_49}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_2_1(%logical_core_8, {%logical_mem_49}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_2_2(%logical_core_9, {%logical_mem_49}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_2_3(%logical_core_10, {%logical_mem_49}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@out_core_2_0, @out_core_2_1, @out_core_2_2, @out_core_2_3] -> [@out_col_2]([0, 256, 512, 768] [])
    aie.objectfifo @out_col_3(%logical_mem_51, {%logical_shim_noc_52}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out_core_3_0(%logical_core_11, {%logical_mem_51}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_3_1(%logical_core_12, {%logical_mem_51}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_3_2(%logical_core_13, {%logical_mem_51}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_3_3(%logical_core_14, {%logical_mem_51}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@out_core_3_0, @out_core_3_1, @out_core_3_2, @out_core_3_3] -> [@out_col_3]([0, 256, 512, 768] [])
    aie.objectfifo @out_col_4(%logical_mem_53, {%logical_shim_noc_54}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out_core_4_0(%logical_core_15, {%logical_mem_53}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_4_1(%logical_core_16, {%logical_mem_53}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_4_2(%logical_core_17, {%logical_mem_53}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_4_3(%logical_core_18, {%logical_mem_53}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@out_core_4_0, @out_core_4_1, @out_core_4_2, @out_core_4_3] -> [@out_col_4]([0, 256, 512, 768] [])
    aie.objectfifo @out_col_5(%logical_mem_55, {%logical_shim_noc_56}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out_core_5_0(%logical_core_19, {%logical_mem_55}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_5_1(%logical_core_20, {%logical_mem_55}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_5_2(%logical_core_21, {%logical_mem_55}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_5_3(%logical_core_22, {%logical_mem_55}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@out_core_5_0, @out_core_5_1, @out_core_5_2, @out_core_5_3] -> [@out_col_5]([0, 256, 512, 768] [])
    aie.objectfifo @out_col_6(%logical_mem_57, {%logical_shim_noc_58}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out_core_6_0(%logical_core_23, {%logical_mem_57}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_6_1(%logical_core_24, {%logical_mem_57}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_6_2(%logical_core_25, {%logical_mem_57}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_6_3(%logical_core_26, {%logical_mem_57}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@out_core_6_0, @out_core_6_1, @out_core_6_2, @out_core_6_3] -> [@out_col_6]([0, 256, 512, 768] [])
    aie.objectfifo @out_col_7(%logical_mem_59, {%logical_shim_noc_60}, 2 : i32) : !aie.objectfifo<memref<1024xi32>> 
    aie.objectfifo @out_core_7_0(%logical_core_27, {%logical_mem_59}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_7_1(%logical_core_28, {%logical_mem_59}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_7_2(%logical_core_29, {%logical_mem_59}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo @out_core_7_3(%logical_core_30, {%logical_mem_59}, 2 : i32) : !aie.objectfifo<memref<256xi32>> 
    aie.objectfifo.link [@out_core_7_0, @out_core_7_1, @out_core_7_2, @out_core_7_3] -> [@out_col_7]([0, 256, 512, 768] [])
    func.func private @passThroughLine(memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) attributes {link_with = "kernel.o"}
    %0 = aie.core(%logical_core) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_0_0(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_0_0(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c0_63 = arith.constant 0 : index
          %37 = arith.addi %c0_63, %36 : index
          %c0_64 = arith.constant 0 : index
          %38 = arith.addi %37, %c0_64 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_65 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_65, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_0_0(Consume, 1)
          aie.objectfifo.release @out_core_0_0(Produce, 1)
        }
      }
      aie.end
    }
    %1 = aie.core(%logical_core_0) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_0_1(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_0_1(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c0_63 = arith.constant 0 : index
          %37 = arith.addi %c0_63, %36 : index
          %c256 = arith.constant 256 : index
          %38 = arith.addi %37, %c256 : index
          %c256_64 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256_64 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_65 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_65, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_0_1(Consume, 1)
          aie.objectfifo.release @out_core_0_1(Produce, 1)
        }
      }
      aie.end
    }
    %2 = aie.core(%logical_core_1) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_0_2(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_0_2(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c0_63 = arith.constant 0 : index
          %37 = arith.addi %c0_63, %36 : index
          %c512 = arith.constant 512 : index
          %38 = arith.addi %37, %c512 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_0_2(Consume, 1)
          aie.objectfifo.release @out_core_0_2(Produce, 1)
        }
      }
      aie.end
    }
    %3 = aie.core(%logical_core_2) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_0_3(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_0_3(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c0_63 = arith.constant 0 : index
          %37 = arith.addi %c0_63, %36 : index
          %c768 = arith.constant 768 : index
          %38 = arith.addi %37, %c768 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_0_3(Consume, 1)
          aie.objectfifo.release @out_core_0_3(Produce, 1)
        }
      }
      aie.end
    }
    %4 = aie.core(%logical_core_3) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_1_0(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_1_0(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c131072 = arith.constant 131072 : index
          %37 = arith.addi %c131072, %36 : index
          %c0_63 = arith.constant 0 : index
          %38 = arith.addi %37, %c0_63 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_1_0(Consume, 1)
          aie.objectfifo.release @out_core_1_0(Produce, 1)
        }
      }
      aie.end
    }
    %5 = aie.core(%logical_core_4) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_1_1(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_1_1(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c131072 = arith.constant 131072 : index
          %37 = arith.addi %c131072, %36 : index
          %c256 = arith.constant 256 : index
          %38 = arith.addi %37, %c256 : index
          %c256_63 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256_63 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_1_1(Consume, 1)
          aie.objectfifo.release @out_core_1_1(Produce, 1)
        }
      }
      aie.end
    }
    %6 = aie.core(%logical_core_5) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_1_2(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_1_2(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c131072 = arith.constant 131072 : index
          %37 = arith.addi %c131072, %36 : index
          %c512 = arith.constant 512 : index
          %38 = arith.addi %37, %c512 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_63 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_63, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_1_2(Consume, 1)
          aie.objectfifo.release @out_core_1_2(Produce, 1)
        }
      }
      aie.end
    }
    %7 = aie.core(%logical_core_6) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_1_3(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_1_3(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c131072 = arith.constant 131072 : index
          %37 = arith.addi %c131072, %36 : index
          %c768 = arith.constant 768 : index
          %38 = arith.addi %37, %c768 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_63 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_63, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_1_3(Consume, 1)
          aie.objectfifo.release @out_core_1_3(Produce, 1)
        }
      }
      aie.end
    }
    %8 = aie.core(%logical_core_7) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_2_0(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_2_0(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c262144 = arith.constant 262144 : index
          %37 = arith.addi %c262144, %36 : index
          %c0_63 = arith.constant 0 : index
          %38 = arith.addi %37, %c0_63 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_2_0(Consume, 1)
          aie.objectfifo.release @out_core_2_0(Produce, 1)
        }
      }
      aie.end
    }
    %9 = aie.core(%logical_core_8) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_2_1(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_2_1(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c262144 = arith.constant 262144 : index
          %37 = arith.addi %c262144, %36 : index
          %c256 = arith.constant 256 : index
          %38 = arith.addi %37, %c256 : index
          %c256_63 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256_63 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_2_1(Consume, 1)
          aie.objectfifo.release @out_core_2_1(Produce, 1)
        }
      }
      aie.end
    }
    %10 = aie.core(%logical_core_9) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_2_2(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_2_2(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c262144 = arith.constant 262144 : index
          %37 = arith.addi %c262144, %36 : index
          %c512 = arith.constant 512 : index
          %38 = arith.addi %37, %c512 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_63 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_63, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_2_2(Consume, 1)
          aie.objectfifo.release @out_core_2_2(Produce, 1)
        }
      }
      aie.end
    }
    %11 = aie.core(%logical_core_10) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_2_3(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_2_3(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c262144 = arith.constant 262144 : index
          %37 = arith.addi %c262144, %36 : index
          %c768 = arith.constant 768 : index
          %38 = arith.addi %37, %c768 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_63 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_63, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_2_3(Consume, 1)
          aie.objectfifo.release @out_core_2_3(Produce, 1)
        }
      }
      aie.end
    }
    %12 = aie.core(%logical_core_11) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_3_0(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_3_0(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c393216 = arith.constant 393216 : index
          %37 = arith.addi %c393216, %36 : index
          %c0_63 = arith.constant 0 : index
          %38 = arith.addi %37, %c0_63 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_3_0(Consume, 1)
          aie.objectfifo.release @out_core_3_0(Produce, 1)
        }
      }
      aie.end
    }
    %13 = aie.core(%logical_core_12) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_3_1(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_3_1(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c393216 = arith.constant 393216 : index
          %37 = arith.addi %c393216, %36 : index
          %c256 = arith.constant 256 : index
          %38 = arith.addi %37, %c256 : index
          %c256_63 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256_63 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_3_1(Consume, 1)
          aie.objectfifo.release @out_core_3_1(Produce, 1)
        }
      }
      aie.end
    }
    %14 = aie.core(%logical_core_13) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_3_2(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_3_2(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c393216 = arith.constant 393216 : index
          %37 = arith.addi %c393216, %36 : index
          %c512 = arith.constant 512 : index
          %38 = arith.addi %37, %c512 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_63 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_63, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_3_2(Consume, 1)
          aie.objectfifo.release @out_core_3_2(Produce, 1)
        }
      }
      aie.end
    }
    %15 = aie.core(%logical_core_14) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_3_3(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_3_3(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c393216 = arith.constant 393216 : index
          %37 = arith.addi %c393216, %36 : index
          %c768 = arith.constant 768 : index
          %38 = arith.addi %37, %c768 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_63 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_63, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_3_3(Consume, 1)
          aie.objectfifo.release @out_core_3_3(Produce, 1)
        }
      }
      aie.end
    }
    %16 = aie.core(%logical_core_15) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_4_0(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_4_0(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c524288 = arith.constant 524288 : index
          %37 = arith.addi %c524288, %36 : index
          %c0_63 = arith.constant 0 : index
          %38 = arith.addi %37, %c0_63 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_4_0(Consume, 1)
          aie.objectfifo.release @out_core_4_0(Produce, 1)
        }
      }
      aie.end
    }
    %17 = aie.core(%logical_core_16) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_4_1(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_4_1(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c524288 = arith.constant 524288 : index
          %37 = arith.addi %c524288, %36 : index
          %c256 = arith.constant 256 : index
          %38 = arith.addi %37, %c256 : index
          %c256_63 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256_63 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_4_1(Consume, 1)
          aie.objectfifo.release @out_core_4_1(Produce, 1)
        }
      }
      aie.end
    }
    %18 = aie.core(%logical_core_17) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_4_2(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_4_2(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c524288 = arith.constant 524288 : index
          %37 = arith.addi %c524288, %36 : index
          %c512 = arith.constant 512 : index
          %38 = arith.addi %37, %c512 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_63 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_63, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_4_2(Consume, 1)
          aie.objectfifo.release @out_core_4_2(Produce, 1)
        }
      }
      aie.end
    }
    %19 = aie.core(%logical_core_18) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_4_3(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_4_3(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c524288 = arith.constant 524288 : index
          %37 = arith.addi %c524288, %36 : index
          %c768 = arith.constant 768 : index
          %38 = arith.addi %37, %c768 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_63 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_63, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_4_3(Consume, 1)
          aie.objectfifo.release @out_core_4_3(Produce, 1)
        }
      }
      aie.end
    }
    %20 = aie.core(%logical_core_19) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_5_0(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_5_0(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c655360 = arith.constant 655360 : index
          %37 = arith.addi %c655360, %36 : index
          %c0_63 = arith.constant 0 : index
          %38 = arith.addi %37, %c0_63 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_5_0(Consume, 1)
          aie.objectfifo.release @out_core_5_0(Produce, 1)
        }
      }
      aie.end
    }
    %21 = aie.core(%logical_core_20) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_5_1(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_5_1(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c655360 = arith.constant 655360 : index
          %37 = arith.addi %c655360, %36 : index
          %c256 = arith.constant 256 : index
          %38 = arith.addi %37, %c256 : index
          %c256_63 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256_63 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_5_1(Consume, 1)
          aie.objectfifo.release @out_core_5_1(Produce, 1)
        }
      }
      aie.end
    }
    %22 = aie.core(%logical_core_21) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_5_2(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_5_2(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c655360 = arith.constant 655360 : index
          %37 = arith.addi %c655360, %36 : index
          %c512 = arith.constant 512 : index
          %38 = arith.addi %37, %c512 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_63 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_63, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_5_2(Consume, 1)
          aie.objectfifo.release @out_core_5_2(Produce, 1)
        }
      }
      aie.end
    }
    %23 = aie.core(%logical_core_22) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_5_3(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_5_3(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c655360 = arith.constant 655360 : index
          %37 = arith.addi %c655360, %36 : index
          %c768 = arith.constant 768 : index
          %38 = arith.addi %37, %c768 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_63 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_63, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_5_3(Consume, 1)
          aie.objectfifo.release @out_core_5_3(Produce, 1)
        }
      }
      aie.end
    }
    %24 = aie.core(%logical_core_23) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_6_0(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_6_0(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c786432 = arith.constant 786432 : index
          %37 = arith.addi %c786432, %36 : index
          %c0_63 = arith.constant 0 : index
          %38 = arith.addi %37, %c0_63 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_6_0(Consume, 1)
          aie.objectfifo.release @out_core_6_0(Produce, 1)
        }
      }
      aie.end
    }
    %25 = aie.core(%logical_core_24) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_6_1(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_6_1(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c786432 = arith.constant 786432 : index
          %37 = arith.addi %c786432, %36 : index
          %c256 = arith.constant 256 : index
          %38 = arith.addi %37, %c256 : index
          %c256_63 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256_63 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_6_1(Consume, 1)
          aie.objectfifo.release @out_core_6_1(Produce, 1)
        }
      }
      aie.end
    }
    %26 = aie.core(%logical_core_25) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_6_2(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_6_2(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c786432 = arith.constant 786432 : index
          %37 = arith.addi %c786432, %36 : index
          %c512 = arith.constant 512 : index
          %38 = arith.addi %37, %c512 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_63 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_63, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_6_2(Consume, 1)
          aie.objectfifo.release @out_core_6_2(Produce, 1)
        }
      }
      aie.end
    }
    %27 = aie.core(%logical_core_26) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_6_3(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_6_3(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c786432 = arith.constant 786432 : index
          %37 = arith.addi %c786432, %36 : index
          %c768 = arith.constant 768 : index
          %38 = arith.addi %37, %c768 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_63 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_63, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_6_3(Consume, 1)
          aie.objectfifo.release @out_core_6_3(Produce, 1)
        }
      }
      aie.end
    }
    %28 = aie.core(%logical_core_27) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_7_0(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_7_0(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c917504 = arith.constant 917504 : index
          %37 = arith.addi %c917504, %36 : index
          %c0_63 = arith.constant 0 : index
          %38 = arith.addi %37, %c0_63 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_7_0(Consume, 1)
          aie.objectfifo.release @out_core_7_0(Produce, 1)
        }
      }
      aie.end
    }
    %29 = aie.core(%logical_core_28) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_7_1(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_7_1(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c917504 = arith.constant 917504 : index
          %37 = arith.addi %c917504, %36 : index
          %c256 = arith.constant 256 : index
          %38 = arith.addi %37, %c256 : index
          %c256_63 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256_63 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_64 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_64, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_7_1(Consume, 1)
          aie.objectfifo.release @out_core_7_1(Produce, 1)
        }
      }
      aie.end
    }
    %30 = aie.core(%logical_core_29) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_7_2(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_7_2(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c917504 = arith.constant 917504 : index
          %37 = arith.addi %c917504, %36 : index
          %c512 = arith.constant 512 : index
          %38 = arith.addi %37, %c512 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_63 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_63, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_7_2(Consume, 1)
          aie.objectfifo.release @out_core_7_2(Produce, 1)
        }
      }
      aie.end
    }
    %31 = aie.core(%logical_core_30) {
      %c0 = arith.constant 0 : index
      %c9223372036854775807 = arith.constant 9223372036854775807 : index
      %c1 = arith.constant 1 : index
      scf.for %arg0 = %c0 to %c9223372036854775807 step %c1 {
        %c0_61 = arith.constant 0 : index
        %c128 = arith.constant 128 : index
        %c1_62 = arith.constant 1 : index
        scf.for %arg1 = %c0_61 to %c128 step %c1_62 {
          %32 = aie.objectfifo.acquire @out_core_7_3(Produce, 1) : !aie.objectfifosubview<memref<256xi32>>
          %33 = aie.objectfifo.subview.access %32[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %34 = aie.objectfifo.acquire @in_core_7_3(Consume, 1) : !aie.objectfifosubview<memref<256xi32>>
          %35 = aie.objectfifo.subview.access %34[0] : !aie.objectfifosubview<memref<256xi32>> -> memref<256xi32>
          %c1024 = arith.constant 1024 : index
          %36 = arith.muli %arg1, %c1024 : index
          %c917504 = arith.constant 917504 : index
          %37 = arith.addi %c917504, %36 : index
          %c768 = arith.constant 768 : index
          %38 = arith.addi %37, %c768 : index
          %c256 = arith.constant 256 : index
          %39 = arith.floordivsi %38, %c256 : index
          %c256_i32 = arith.constant 256 : i32
          %c0_i32 = arith.constant 0 : i32
          %c1_i32 = arith.constant 1 : i32
          %c1024_i32 = arith.constant 1024 : i32
          %c1024_i32_63 = arith.constant 1024 : i32
          %cst = arith.constant 1.000000e+00 : f32
          func.call @passThroughLine(%35, %33, %c256_i32, %c0_i32, %39, %c1_i32, %c1024_i32, %c1024_i32_63, %cst) : (memref<256xi32>, memref<256xi32>, i32, i32, index, i32, i32, i32, f32) -> ()
          aie.objectfifo.release @in_core_7_3(Consume, 1)
          aie.objectfifo.release @out_core_7_3(Produce, 1)
        }
      }
      aie.end
    }
    aie.runtime_sequence(%arg0: memref<1048576xi32>, %arg1: memref<1048576xi32>) {
      %32 = aiex.dma_configure_task_for @in_col_0 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 0, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%32)
      %33 = aiex.dma_configure_task_for @in_col_1 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 131072, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%33)
      %34 = aiex.dma_configure_task_for @in_col_2 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 262144, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%34)
      %35 = aiex.dma_configure_task_for @in_col_3 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 393216, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%35)
      %36 = aiex.dma_configure_task_for @in_col_4 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 524288, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%36)
      %37 = aiex.dma_configure_task_for @in_col_5 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 655360, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%37)
      %38 = aiex.dma_configure_task_for @in_col_6 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 786432, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%38)
      %39 = aiex.dma_configure_task_for @in_col_7 {
        aie.dma_bd(%arg0 : memref<1048576xi32>, 917504, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      }
      aiex.dma_start_task(%39)
      %40 = aiex.dma_configure_task_for @out_col_0 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 0, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%40)
      %41 = aiex.dma_configure_task_for @out_col_1 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 131072, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%41)
      %42 = aiex.dma_configure_task_for @out_col_2 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 262144, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%42)
      %43 = aiex.dma_configure_task_for @out_col_3 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 393216, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%43)
      %44 = aiex.dma_configure_task_for @out_col_4 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 524288, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%44)
      %45 = aiex.dma_configure_task_for @out_col_5 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 655360, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%45)
      %46 = aiex.dma_configure_task_for @out_col_6 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 786432, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%46)
      %47 = aiex.dma_configure_task_for @out_col_7 {
        aie.dma_bd(%arg1 : memref<1048576xi32>, 917504, 131072, [<size = 1, stride = 0>, <size = 1, stride = 0>, <size = 1, stride = 0>, <size = 131072, stride = 1>]) {burst_length = 0 : i32}
        aie.end
      } {issue_token = true}
      aiex.dma_start_task(%47)
      aiex.dma_await_task(%40)
      aiex.dma_await_task(%41)
      aiex.dma_await_task(%42)
      aiex.dma_await_task(%43)
      aiex.dma_await_task(%44)
      aiex.dma_await_task(%45)
      aiex.dma_await_task(%46)
      aiex.dma_await_task(%47)
      aiex.dma_free_task(%32)
      aiex.dma_free_task(%33)
      aiex.dma_free_task(%34)
      aiex.dma_free_task(%35)
      aiex.dma_free_task(%36)
      aiex.dma_free_task(%37)
      aiex.dma_free_task(%38)
      aiex.dma_free_task(%39)
    }
  }
}

