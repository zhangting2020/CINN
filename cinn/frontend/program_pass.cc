#include "cinn/frontend/program_pass.h"

namespace cinn {
namespace frontend {

void ProgramPass::Apply(Program* prog, const common::Target& target, const std::vector<std::string>& passes) {
  std::vector<const ProgramPass*> fpass;
  for (auto& name : passes) {
    auto pass = ProgramPassRegistry::Global()->Get(name);
    fpass.push_back(pass);
  }
  for (auto& pass : fpass) {
    pass->ApplyImpl(prog, target);
  }
}

}  // namespace frontend
}  // namespace cinn
