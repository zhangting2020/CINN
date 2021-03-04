#include "cinnrt/host_context/symbol_table.h"
#include <mlir/Dialect/StandardOps/IR/Ops.h>
#include <mlir/IR/Function.h>

#include <string>

namespace cinnrt {
namespace host_context {

struct SymbolTable::Impl {
  std::unordered_map<std::string, ValueRef> values;

  std::unordered_map<std::string, mlir::FuncOp> functions;

  // The SymbolTables inherit this one.
  llvm::SmallVector<std::unique_ptr<SymbolTable>, 3> children;

  SymbolTable* parent{};
};

SymbolTable::SymbolTable() : impl_(new Impl) {}

Value* SymbolTable::Insert(std::string_view key) {
  auto it = impl_->values.try_emplace(std::string(key), ValueRef(new Value));
  CHECK(it.second) << "Duplicate register [" << key << "]";
  return it.first->second.get();
}

Value* SymbolTable::Insert(std::string_view key, ValueRef value) {
  auto it = impl_->values.try_emplace(std::string(key), value);
  CHECK(it.second) << "Duplicate register [" << key << "]";
  return it.first->second.get();
}

Value* SymbolTable::Lookup(std::string_view key) const {
  auto it = impl_->values.find(std::string(key));
  auto* v = it != impl_->values.end() ? it->second.get() : nullptr;
  if (v) return v;

  return parent() ? parent()->Lookup(key) : nullptr;
}

SymbolTable* SymbolTable::New() {
  impl_->children.emplace_back(new SymbolTable);
  auto* res          = impl_->children.back().get();
  res->impl_->parent = this;
  return res;
}

// @{
#define REGISTER_TYPE__(T)                                         \
  template <>                                                      \
  T SymbolTable::Lookup<T>(std::string_view key) {                 \
    auto it = impl_->values.find(std::string(key));                \
    CHECK(it != impl_->values.end()) << "No value called " << key; \
    return it->second->get<T>();                                   \
  }
REGISTER_TYPE__(int32_t);
REGISTER_TYPE__(float);
REGISTER_TYPE__(double);
REGISTER_TYPE__(int64_t);
#undef REGISTER_TYPE__
// @}

SymbolTable::~SymbolTable() {}

size_t SymbolTable::size() const { return impl_->values.size(); }
const SymbolTable* SymbolTable::parent() const { return impl_->parent; }

mlir::FuncOp* SymbolTable::Insert(std::string_view key, mlir::FuncOp* value) {
  auto it = impl_->functions.try_emplace(std::string(key), *value);
  CHECK(it.second) << "Duplicate insert a function [" << key << "]";
  return value;
}

mlir::FuncOp* SymbolTable::LookupFunc(std::string_view key) const {
  auto it = impl_->functions.find(std::string(key));
  if (it != impl_->functions.end()) {
    return &it->second;
  }

  return parent() ? parent()->LookupFunc(key) : nullptr;
}

// @{
#define REGISTER_TYPE__(T)                                              \
  template <>                                                           \
  Value* SymbolTable::Insert(std::string_view key, T&& v) {             \
    auto it = impl_->values.try_emplace(std::string(key), ValueRef(v)); \
    CHECK(it.second) << "Duplicate register [" << key << "]";           \
    return it.first->second.get();                                      \
  }
REGISTER_TYPE__(int)
REGISTER_TYPE__(float)
REGISTER_TYPE__(double)
REGISTER_TYPE__(bool)
#undef REGISTER_TYPE__
// @}

}  // namespace host_context
}  // namespace cinnrt
