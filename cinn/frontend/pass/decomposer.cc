#include "cinn/frontend/decomposer_registry.h"
#include "cinn/frontend/program_pass.h"

namespace cinn {
namespace frontend {
namespace pass {

class DecomposerPass : public ProgramPass {
 public:
  void ApplyImpl(Program* prog, const common::Target& target) const {
    auto instrs = prog->GetInstructions();
    prog->CleanInstructions();
    DecomposerContext context(prog);
    for (int i = 0; i < instrs.size(); i++) {
      auto instr      = instrs[i];
      auto decomposer = InstrDecomposerRegistry::Global()->Find(instr->op_type, target);
      if (decomposer) {
        decomposer->Run(instr, context);
      } else {
        prog->AppendInstruction(instr);
      }
    }
    prog->Validate();
  }
};

}  // namespace pass
}  // namespace frontend
}  // namespace cinn

CINN_REGISTER_HELPER(Decomposer) {
  CINN_REGISTER_PROGRAM_PASS(Decomposer, ::cinn::frontend::pass::DecomposerPass());

  return true;
}
