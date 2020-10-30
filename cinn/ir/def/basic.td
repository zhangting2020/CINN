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

#endif // CINN_BASIC_IR
