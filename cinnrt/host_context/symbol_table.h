#pragma once

#include <memory>
#include <unordered_map>

#include "cinnrt/host_context/value.h"

namespace mlir {
struct FuncOp;
}  // namespace mlir

namespace cinnrt {
namespace host_context {

/**
 * SymbolTable holds the variables and functions of the current scope.
 * It is a hierarchical structure and the child can access the symbols in the parent.
 *
 * e.g.
 * \code
 * %1 = 1 : i32
 * function func(%arg) {
 *   %2 = 2 : i32
 *   cinn.print.i32 %1
 * }
 * \endcode
 *
 * there are two SymbolTables in the program above:
 * - the first holds the symbols of { %1, func }
 * - the second is the one inside the function `func`, which holds the { %arg, %2 }, but it can access %1 in the parent
 * scope.
 */
class SymbolTable {
 public:
  SymbolTable();

  /**
   * Insert a state called \p key.
   */
  Value* Insert(std::string_view key);

  Value* Insert(std::string_view key, ValueRef value);

  /**
   * Insert a function.
   */
  mlir::FuncOp* Insert(std::string_view key, mlir::FuncOp* value);

  /**
   * Register a state and set value.
   */
  template <typename T>
  Value* Insert(std::string_view key, T&& v);

  /**
   * Get a state called \p key.
   */
  Value* Lookup(std::string_view key) const;

  mlir::FuncOp* LookupFunc(std::string_view) const;

  template <typename T>
  T Lookup(std::string_view key);

  const SymbolTable* parent() const;

  size_t size() const;

  /**
   * Create a new SymbolTable inherient this one.
   */
  SymbolTable* New();

  ~SymbolTable();

 private:
  class Impl;

  std::unique_ptr<Impl> impl_;
};

}  // namespace host_context
}  // namespace cinnrt
