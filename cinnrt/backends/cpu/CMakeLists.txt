set(srcs
  cpu_kernels.cc
  )


foreach(cpp ${srcs})
  set(core_src
    "${core_src};${CMAKE_CURRENT_SOURCE_DIR}/${cpp}"
    CACHE INTERNAL "")
endforeach()

