#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "cinn/frontend/syntax.h"
#include "cinn/utils/registry.h"

namespace cinn {
namespace frontend {

class ProgramPass {
 public:
  /**
   * \brief Apply a sequence of passes on a program.
   * @param g The input program to apply passes on.
   * @param passes The sequence of pass.
   * @return The program after being modified by the passes.
   */
  Program* Apply(Program* prog, const std::vector<std::string>& passes) const;
  virtual Program* ApplyImpl(Program* prog) const { return nullptr; }

  std::string name;
};

class ProgramPassRegistry : public Registry<ProgramPass> {
 public:
  static ProgramPassRegistry* Global() {
    static ProgramPassRegistry x;
    return &x;
  }

  inline ProgramPass* __REGISTER__(const std::string& name, ProgramPass* pass) {
    RAW_LOG(INFO, "Register %s program pass", name.c_str());
    std::lock_guard<std::mutex> guard(registering_mutex);
    if (fmap_.count(name)) {
      return fmap_[name];
    }

    pass->name  = name;
    fmap_[name] = pass;
    const_list_.push_back(pass);
    entry_list_.push_back(pass);
    return pass;
  }

 private:
  ProgramPassRegistry() = default;
  CINN_DISALLOW_COPY_AND_ASSIGN(ProgramPassRegistry);
};

/**
 * @def CINN_REGISTER_PROGRAM_PASS
 * \brief Register a new program pass
 *
 * @param PassType The type of pass
 * @param PassClass The pass inherited from ProgramPass
 *
 * \code
 *  CINN_REGISTER_PROGRAM_PASS(decompose, DecomposerPass());
 * \endcode
 */
#define CINN_REGISTER_PROGRAM_PASS(PassType, PassClass)         \
  static ::cinn::frontend::ProgramPass* __make_##PassType##__ = \
      ::cinn::frontend::ProgramPassRegistry::Global()->__REGISTER__(#PassType, new PassClass)

}  // namespace frontend
}  // namespace cinn
