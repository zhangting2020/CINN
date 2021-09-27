#include <gtest/gtest.h>

#include "cinn/frontend/pass.h"
#include "cinn/frontend/pass/use_program_pass.h"

namespace cinn::frontend {

TEST(DecomposePass, basic) {
  ASSERT_NE(cinn::frontend::ProgramPassRegistry::Global()->Find("Decompose"), nullptr);
  ASSERT_EQ(cinn::frontend::ProgramPassRegistry::Global()->Find("Test"), nullptr);
}

}  // namespace cinn::frontend
