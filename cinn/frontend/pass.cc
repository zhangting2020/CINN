#include "cinn/frontend/pass.h"

namespace cinn {
namespace frontend {

Program* ProgramPass::Apply(Program* prog, const std::vector<std::string>& passes) const {
  std::vector<const ProgramPass*> fpass;
  for (auto& name : passes) {
    auto pass = ProgramPassRegistry::Global()->Find(name);
    if (pass) {
      fpass.push_back(pass);
    }
  }
  Program* new_prog = prog;
  for (auto& pass : fpass) {
    new_prog = pass->ApplyImpl(new_prog);
  }
  return new_prog;
}

}  // namespace frontend
}  // namespace cinn
