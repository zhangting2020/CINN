#include "cinn/frontend/pass.h"

namespace cinn {
namespace frontend {
namespace pass {

class DecomposerPass : public ProgramPass {
 public:
  Program* ApplyImpl(Program* prog) const { return nullptr; }
};

}  // namespace pass
}  // namespace frontend
}  // namespace cinn

CINN_REGISTER_HELPER(Decompose) {
  CINN_REGISTER_PROGRAM_PASS(Decompose, ::cinn::frontend::pass::DecomposerPass());

  return true;
}
