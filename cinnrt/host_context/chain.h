#pragma once

namespace cinnrt {
namespace host_context {

struct Chain {
  Chain()             = default;
  Chain(Chain&&)      = default;
  Chain(const Chain&) = default;
  Chain& operator=(const Chain&) = default;
};

}  // namespace host_context
}  // namespace cinnrt
