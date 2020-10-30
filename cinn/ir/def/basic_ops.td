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


#endif  // IR_BASIC_OPS
