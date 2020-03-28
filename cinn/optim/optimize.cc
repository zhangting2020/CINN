#include "cinn/optim/optimize.h"

#include "cinn/optim/ir_copy.h"
#include "cinn/optim/ir_eliminate_mod.h"
#include "cinn/optim/ir_simplify.h"
#include "cinn/optim/remove_nested_block.h"
#include "cinn/optim/unroll_loops.h"
#include "cinn/ir/ir_printer.h"
#include "cinn/optim/transform_polyfor_to_for.h"
#include "cinn/optim/vectorize_loops.h"

namespace cinn {
namespace optim {

Expr Optimize(Expr e) {
  auto copied = IRCopy(e);
  IrEliminateMod(&copied);
  TransformPolyForToFor(&copied);
  LOG(INFO) << "before simplify " << copied;
  Simplify(&copied);
  LOG(INFO) << "before vectorize get:\n"  << copied;
  VectorizeLoops(&copied, Target());
  UnrollLoop(&copied);
  RemoveNestedBlock(&copied);
  Simplify(&copied);

  return copied;
}

}  // namespace optim
}  // namespace cinn
