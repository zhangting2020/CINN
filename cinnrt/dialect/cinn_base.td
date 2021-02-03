#ifdef CINN_BASE
#else
#define CINN_BASE

include "mlir/IR/OpBase.td"

def CINN_Dialect : Dialect {
  let name = "cinn";

  let description = [{
    The CINN host dialect.
  }];

  let cppNamespace = "::cinnrt::dialect";
}

// Type definitions
def StringType : OpaqueType<"cinn", "string", "!cinn.string type">;

def TensorType :
    Type<CPred<"$_self.isa<::cinnrt::dt::TensorType>()">, "!cinn.tensor type">;

def BufferType :
    Type<CPred<"$_self.isa<::cinnrt::BufferType>()">, "!cinn.buffer type">,
    BuildableType<"$_builder.getType<::cinnrt::BufferType>()">;

def ChainType :
    Type<CPred<"$_self.isa<::cinnrt::ChainType>()">, "!cinn.chain type">,
    BuildableType<"$_builder.getType<::cinnrt::ChainType>()">;

#endif  // CINN_BASE
