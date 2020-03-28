#include "cinn/poly/ast_gen.h"

#include <gtest/gtest.h>

#include "cinn/ir/ir.h"
#include "cinn/ir/ir_operators.h"
#include "cinn/ir/ir_printer.h"
#include "cinn/lang/tensor.h"

namespace cinn {
namespace poly {

TEST(ast_gen, basic) {
  isl::ctx ctx = Context::Global().isl_ctx();
  auto A       = Stage::New(isl::set(ctx, "{ A[i,j,k]: 0<i,j,k<100 }"));
  auto B       = Stage::New(isl::set(ctx, "{ B[i,j,k]: 0<i,j,k<100 }"));

  Iterator A_i0, A_i1;
  Iterator B_i0, B_i1;

  std::tie(A_i0, A_i1) = A->Split(Iterator("k"), 4);
  std::tie(B_i0, B_i1) = B->Split(Iterator("k"), 4);

  std::vector<Stage*> stages({A.get(), B.get()});

  PolyScheduler scheduler(stages);
  auto schedule = scheduler.BuildSchedule();
  ASSERT_EQ(schedule->groups.size(), 2UL);
  // scheduler.After(*A, *B, 3);

  for (int i = 0; i < schedule->groups.size(); i++) {
    AstGen gen(isl::set(ctx, "{:}"), {stages[i]}, schedule->groups[i]);

    gen.SetIteratorNames({"i.outer", "i.inner", "j", "k"});
    isl::ast_node ast = gen.Build();
    ir::Expr e;
    IslAstNodeToCinnExpr(ast, &e);

    LOG(INFO) << "\n" << e;
  }
}

TEST(ast_gen, IndexExprToIslTransform) {
  auto ctx = Context::Global().isl_ctx();
  isl::set domain(ctx, "{ S[i,j] : 0<i,j<100 }");

  Var i("i");
  Var j("j");

  auto transform = IndexExprToIslTransform(domain, {i, j % 100 + 1});
  EXPECT_EQ(utils::GetStreamCnt(transform), "{ S[i, j] -> XXX[i, o1] : (-1 - j + o1) mod 100 = 0 and 0 < o1 <= 100 }");
}

}  // namespace poly
}  // namespace cinn
