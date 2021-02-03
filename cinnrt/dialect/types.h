#pragma once
#include <mlir/IR/StandardTypes.h>

using namespace mlir;

namespace cinnrt {

class ChainType : public mlir::Type::TypeBase<ChainType, mlir::Type, mlir::TypeStorage> {
 public:
  using Base::Base;
};

class BufferType : public mlir::Type::TypeBase<BufferType, mlir::Type, mlir::TypeStorage> {
 public:
  using Base::Base;
};

}  // namespace cinnrt
