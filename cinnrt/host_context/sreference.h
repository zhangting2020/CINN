#pragma once
#include <type_traits>
#include "cinn/common/shared.h"

namespace cinnrt {
namespace host_context {

/**
 * SReference: Shared reference.
 * @tparam T type of the instance hold.
 */
template <typename T>
class SReference {
 public:
  SReference() = default;

  SReference(SReference&& other) = default;

  // Support implicit conversion from SReference<Derived> to SReference<Base>.
  template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
  SReference(SReference<U>&& u) : data_(u.data_) {
    u.data_ = nullptr;
  }

  explicit SReference(T* data) : data_(data) {}

  SReference& operator=(SReference&& other) { data_ = other.data_; }

  void Reset(T* pointer = nullptr) { data_.Reset(pointer); }

  T* release() { return data_.release(); }

 private:
  cinn::common::Shared<T> data_;
};

}  // namespace host_context
}  // namespace cinnrt
