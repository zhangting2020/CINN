// CHECK-LABEL: build_tensor1
func @build_tensor1() {
  %a = dt.create_uninit_tensor.f32 [3, 4] -> !cinn.tensor<X86, NCHW, F32>

  %ch0 = cinn.new_chain

  %ch1 = dt.fill_tensor_with_constant.f32 (%a : !cinn.tensor<X86, NCHW, F32> %ch0 : !cinn.chain) {value=1.0:f32}

  // CHECK: tensor: shape=shape[3,4], values=[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
  dt.print_tensor (%a : !cinn.tensor<X86, NCHW, F32>)

  cinn.return
}

func @build_tensor2() {
  %a = dt.create_uninit_tensor.f32 [3, 4] -> !cinn.tensor<X86, NCHW, F32>
  %ch0 = cinn.new_chain
  %buffer, %ch1 = dt.get_buffer(%a, %ch0) : !cinn.tensor<X86, NCHW, F32> -> !cinn.buffer

  cinn.return
}
