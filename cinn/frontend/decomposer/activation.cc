#include "cinn/frontend/decomposer_registry.h"
#include "cinn/frontend/syntax.h"

namespace cinn {
namespace frontend {
namespace decomposer {

void relu(const Instruction& instr, const DecomposerContext& context) {
  CHECK_EQ(instr->inputs.size(), 1UL) << " 1 input tensor for " << instr->op_type;
  CHECK_EQ(instr->outputs.size(), 1UL) << "1 output tensor for " << instr->op_type;
  auto x       = instr->inputs[0];
  auto output  = instr->outputs[0];
  auto builder = context.builder;

  auto zero_var   = builder->ConstScalar<float>(0.f, common::UniqName("zero"));
  auto bcast_zero = builder->BroadcastTo(zero_var, x->shape, {0});
  auto out        = builder->Max(x, bcast_zero);

  // out needs to be mapped to origin out_id
  out.set_id(output->id);
}

void relu_grad(const Instruction& instr, const DecomposerContext& context) {
  CHECK_EQ(instr->inputs.size(), 2UL) << " 2 input tensor for " << instr->op_type;
  CHECK_EQ(instr->outputs.size(), 1UL) << "1 output tensor for " << instr->op_type;
  auto dout    = instr->inputs[0];
  auto out     = instr->inputs[1];
  auto dx      = instr->outputs[0];
  auto builder = context.builder;

  auto zero_var   = builder->ConstScalar<float>(0.f, common::UniqName("zero"));
  auto bcast_zero = builder->BroadcastTo(zero_var, out->shape, {0});
  auto condition  = builder->Compare(out, bcast_zero, ComparisonKind::kGt);
  auto res        = builder->Select(condition, dout, bcast_zero);

  res.set_id(dx->id);
}

}  // namespace decomposer
}  // namespace frontend
}  // namespace cinn

CINN_REGISTER_HELPER(activation) {
  CINN_DECOMPOSER_REGISTER(relu, ::cinn::common::DefaultHostTarget()).set_body(cinn::frontend::decomposer::relu);
  CINN_DECOMPOSER_REGISTER(relu, ::cinn::common::DefaultNVGPUTarget()).set_body(cinn::frontend::decomposer::relu);

  return true;
}

CINN_REGISTER_HELPER(activation_grad) {
  CINN_DECOMPOSER_REGISTER(relu_grad, ::cinn::common::DefaultHostTarget())
      .set_body(cinn::frontend::decomposer::relu_grad);
  CINN_DECOMPOSER_REGISTER(relu_grad, ::cinn::common::DefaultNVGPUTarget())
      .set_body(cinn::frontend::decomposer::relu_grad);

  return true;
}
