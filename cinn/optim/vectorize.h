#pragma once

#include "cinn/ir/ir.h"

namespace cinn {
namespace optim {

//! Vectorize the forloop.
void Vectorize(ir::PolyFor* forloop);

}  // namespace optim
}  // namespace cinn
