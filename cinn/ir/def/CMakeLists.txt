set(srcs
  basic_ops.cc)

mlir_tablegen_on(basic DIALECT basic)
mlir_tablegen_on(basic_ops)

foreach(cpp ${srcs})
  set(core_src
    "${core_src};cinn/ir/def/${cpp}"
    CACHE INTERNAL "")
endforeach()
