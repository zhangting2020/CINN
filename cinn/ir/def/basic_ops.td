// Operations for the basic dialect
#ifdef IR_BASIC_OPS
#else
#define IR_BASIC_OPS

include "cinn/ir/def/basic.td"
include "mlir/Interfaces/SideEffectInterfaces.td"


// Base class of all the basic operations.
class Basic_Op<string mnemonic, list<OpTrait> traits = []> :
        Op<Basic_Dialect, mnemonic, !listconcat(traits, [IsolatedFromAbove])> {
  // Each registered op needs to provide all of a printer, parser and verifier.
  let printer = [{ return cinn::ir::print(p, *this); }];
  let verifier = [{ return cinn::ir::verify(*this); }];
  let parser = [{ return cinn::ir::parse$cppClass(parser, result); }];

}

class Basic_BinaryOp<string mnemonic, string desc> : Basic_Op<mnemonic, [NoSideEffect]> {
  let summary = mnemonic # " operation";
  string description = "An operation that " # desc # " two operations.\nThe types of the operands should match.";

  let arguments = (ins BasicType:$a, BasicType:$b);
  let results = (outs BasicType:$out);
}

class Basic_VarBase<string suffix, Type type> : Basic_Op<mnemonic, type> {
  let summary = "var operation";
  let description = [{}];
}

def Basic_Var_I32 : Basic_VarBase<"i32", I32>;
def Basic_Var_I64 : Basic_VarBase<"i64", I32>;
def Basic_Var_F32 : Basic_VarBase<"f32", I32>;
def Basic_Var_F32 : Basic_VarBase<"f32", I32>;


// === binary operations
// @{
def Basic_Add : Basic_BinaryOp<"add", "addes">;
def Basic_Sub : Basic_BinaryOp<"sub", "substract">;
def Basic_Mul : Basic_BinaryOp<"mul", "">;
def Basic_Div : Basic_BinaryOp<"div", "">;
def Basic_Mod : Basic_BinaryOp<"mod", "">;

def Basic_Min : Basic_BinaryOp<"min", "">;
def Basic_Max : Basic_BinaryOp<"max", "">;
def Basic_NE : Basic_BinaryOp<"ne", "">;
def Basic_LT : Basic_BinaryOp<"lt", "">;
def Basic_LE : Basic_BinaryOp<"le", "">;
def Basic_GT : Basic_BinaryOp<"gt", "">;
def Basic_GE : Basic_BinaryOp<"ge", "">;
// @}

// === logical operations
// @{
class LogicalBinaryOp<string mnemonic> : Basic_Op<mnemonic, [NoSideEffect]> {
  let summary = mnemonic # " operation";
  let arguments = (ins I1);
  let results = (outs I1);
}

def Basic_And : LogicalBinaryOp<"and"> {
  let description = [{
  This operation get the logical AND of two operands.
  }];
}

def Basic_Or : LogicalBinaryOp<"or"> {
  let description = [{
  This operation get the logical OR of two operands.
  }];
}
// @}

def Basic_Let : Basic_Op<"let", [NoSideEffect]> {
  let summary = "let operation";
  let description = [{
    Create a local variable with a default value.
  }];

  let arguments = (ins AnyType:$value, StrAttr:$var_name);
  let results = (outs AnyType);
}

def Basic_IntrinsicCall : Basic_Op<"intrinsic_call", [NoSideEffect]> {
  let summary = "intrinsic_call operation";
  let description = [{
    Call an intrisic.
  }];

  let arguments = (ins StrAttr:$name, Variadic<AnyType>:$read_args,
                   Variadic<AnyType>:$write_args);
}

def Basic_ExternCall : Basic_Op<"extern_call", [NoSideEffect]> {
  let summary = "extern_call operation";
  let description = [{
    Call an enternal function.
  }];

  let arguments = (ins StrAttr:$name, Variadic<AnyType>:$read_args,
                   Variadic<AnyType>:$write_args);
}

// Reduce operations
// @{
class Basic_Reduce<string type> : Basic_Op<"reduce." # type, [NoSideEffect]> {
  let summary = "reduce." # type # " operation";
  let description = [{
    Reduce operation.
  }];

  let arguments = (ins BasicType:$init, BasicType:$body, Variadic<StrAttr>:$reduce_axis);
  let results = (outs);
}

def Basic_ReduceSum : Basic_Reduce<"sum">;
// @}

def Basic_Select : Basic_Op<"select", [NoSideEffect]> {
  let summary = "select operation";
  let description = [{
   Evaluates `true_value` and `false_value` then selects between them based on `condition`.
  }];

  let arguments = (ins BoolType:$condition, BasicType:$true_value, BasicType:$false_value);
  let results = (outs BasicType);
}

// Load store operations
// @{
class Basic_LoadStore<string name> : Basic_Op<name, [NoSideEffect]> {
  let arguments = (ins TensorType:$tensor, Variadic<IndiceType>:indices);
  let results = (outs BasicType);
};

def Basic_Load : Basic_LoadStore<"load">;
def Basic_Store : Basic_LoadStore<"store">;
// @}

#endif  // IR_BASIC_OPS
