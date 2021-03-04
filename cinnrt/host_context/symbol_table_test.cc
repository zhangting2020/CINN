#include "cinnrt/host_context/symbol_table.h"
#include <gtest/gtest.h>

namespace cinnrt {
namespace host_context {

TEST(SymbolTable, inherit) {
  SymbolTable st0;
  st0.Insert("a", 1);
  st0.Insert("b", 2);

  auto* st1 = st0.New();

  auto* v1 = st1->Lookup("a");
  ASSERT_TRUE(v1);
  ASSERT_EQ(v1->get<int32_t>(), 1);

  auto* v2 = st1->Lookup("b");
  ASSERT_TRUE(v2);
  ASSERT_EQ(v2->get<int32_t>(), 2);
}

}  // namespace host_context
}  // namespace cinnrt
