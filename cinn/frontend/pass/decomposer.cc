#include "cinn/frontend/decomposer_registry.h"
#include "cinn/frontend/program_pass.h"

namespace cinn {
namespace frontend {
namespace pass {

class DecomposerPass : public ProgramPass {
 public:
  void ApplyImpl(Program* prog, const common::Target& target) const {
    // step 1: set the inputs of the origin program to the new program
    CinnBuilder builder("decomposer_builder");
    builder.SetInputs(prog->GetInputs());

    // step 2: use primitive instructions to build the new program
    DecomposerContext context(&builder);
    for (int i = 0; i < prog->size(); i++) {
      auto instr      = (*prog)[i];
      auto decomposer = InstrDecomposerRegistry::Global()->Find(instr->op_type, target);
      if (decomposer) {
        decomposer->Run(instr, context);
      } else {
        builder.AppendInstruction(instr);
      }
    }
    *prog = builder.Build();
  }
};

}  // namespace pass
}  // namespace frontend
}  // namespace cinn

CINN_REGISTER_HELPER(Decomposer) {
  CINN_REGISTER_PROGRAM_PASS(Decomposer, ::cinn::frontend::pass::DecomposerPass());

  return true;
}
