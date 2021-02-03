#pragma once
#include "cinn/common/object.h"
#include "cinn/hlir/framework/buffer.h"
#include "cinnrt/host_context/sreference.h"

namespace cinnrt {
namespace tensor {

class Buffer : public cinn::hlir::framework::Buffer, public cinn::common::Object {
 public:
  using Target = cinn::common::Target;
  Buffer()     = default;
  explicit Buffer(const Target& target) : cinn::hlir::framework::Buffer(target) {}

  host_context::SReference<Buffer> GetRef() { return host_context::SReference<Buffer>(this); }

  const char* type_info() const override { return "cinnrt::tensor::Buffer"; }
};

}  // namespace tensor
}  // namespace cinnrt
