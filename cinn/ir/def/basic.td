#ifdef CINN_BASIC_IR
#else
#define CINN_BASIC_IR


include "mlir/IR/OpBase.td"

def Basic_Dialect : Dialect {
  let name = "basic";

  let description = [{
    The CINN fine-grained IR.
  }];

  let cppNamespace = "::cinn::ir";
}

// Type definitions

def BasicType : TypeAlias<AnyTypeOf<[AnyFloat, AnyInteger]>>;
def BoolType : TypeAlias<I1>;
def IndiceType : TypeAlias<I32>;

def TensorType : OpaqueType<"ir", "tensor", "tensor">;
def BufferType : OpaqueType<"ir", "buffer", "buffer">;
def VarType : OpaqueType<"ir", "var", "var">;

#endif // CINN_BASIC_IR
