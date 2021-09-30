#include "cinn/frontend/decomposer_registry.h"
#include "cinn/frontend/syntax.h"

namespace cinn {
namespace frontend {
namespace decomposer {

void relu(const Instruction& instr, const DecomposerContext& context) {
  CHECK_EQ(instr->inputs.size(), 1UL) << " 1 input tensor for " << instr->op_type;
  CHECK_EQ(instr->outputs.size(), 1UL) << "1 output tensor for " << instr->op_type;
  auto x       = instr->inputs[0];
  auto outputs = instr->outputs;
  auto program = context.program;

  auto zero_var   = program->primitive_const_scalar<float>(0.f, common::UniqName("zero"));
  auto bcast_zero = program->primitive_broadcast_to(zero_var, x->shape, {0});
  auto out        = program->primitive_max(x, bcast_zero);

  // out needs to be mapped to origin out_id
  out.set_id(outputs[0]->id);
}

}  // namespace decomposer
}  // namespace frontend
}  // namespace cinn

CINN_REGISTER_HELPER(activation) {
  CINN_DECOMPOSER_REGISTER(relu, ::cinn::common::DefaultHostTarget()).set_body(cinn::frontend::decomposer::relu);
  CINN_DECOMPOSER_REGISTER(relu, ::cinn::common::DefaultNVGPUTarget()).set_body(cinn::frontend::decomposer::relu);

  return true;
}
