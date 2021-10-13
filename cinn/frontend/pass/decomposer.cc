// Copyright (c) 2021 CINN Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "cinn/frontend/decomposer_registry.h"
#include "cinn/frontend/program_pass.h"

namespace cinn {
namespace frontend {
namespace pass {

class DecomposerPass : public ProgramPass {
 public:
  using ProgramPass::ProgramPass;

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
  CINN_REGISTER_PROGRAM_PASS(Decomposer, ::cinn::frontend::pass::DecomposerPass);

  return true;
}
