#include "cinn/backends/codegen_cuda_dev.h"

#include <fstream>

#include "cinn/optim/remove_nested_block.h"

namespace cinn {
namespace backends {

CodeGenCUDA_Dev::CodeGenCUDA_Dev(Target target) : CodeGenC(target) {}

std::string CodeGenCUDA_Dev::Compile(const lang::Module &module, bool for_nvrtc) {
  for_nvrtc_  = for_nvrtc;
  auto source = Compile(module, OutputKind::CImpl);

  return source;
}

void CodeGenCUDA_Dev::Compile(const lang::Module &module, const Outputs &outputs) {
  CodeGenC::inline_builtin_codes_ = false;
  if (!outputs.c_header_name.empty()) {
    auto source = Compile(module, OutputKind::CHeader);
    std::ofstream file(outputs.c_header_name);
    CHECK(file.is_open()) << "failed to open file " << outputs.c_header_name;
    file << source;
    file.close();
    LOG(WARNING) << "Output C header to file " << outputs.c_header_name;
  }

  if (!outputs.cuda_source_name.empty()) {
    auto source = Compile(module, OutputKind::CImpl);
    std::ofstream file(outputs.cuda_source_name);
    CHECK(file.is_open()) << "failed to open file " << outputs.cuda_source_name;
    file << source;
    file.close();
    LOG(WARNING) << "Output C source to file " << outputs.cuda_source_name;
  }
}

std::string CodeGenCUDA_Dev::Compile(const ir::LoweredFunc &func) {
  Print(Expr(func));
  return ss_.str();
}

void CodeGenCUDA_Dev::Visit(const ir::_LoweredFunc_ *op) {
  os() << "__global__\n";

  PrintFunctionDeclaration(op);
  os() << "\n";

  DoIndent();

  Expr func_body = op->body;

  optim::RemoveNestedBlock(&func_body);

  Print(func_body);
}

void CodeGenCUDA_Dev::Visit(const ir::Min *op) {
  os() << "min(";
  Print(op->a());
  os() << ", ";
  Print(op->b());
  os() << ")";
}

void CodeGenCUDA_Dev::PrintFunctionDeclaration(const ir::_LoweredFunc_ *op) {
  os() << "void " << GenKernelName(op->name) << "(";
  for (int i = 0; i < op->args.size() - 1; i++) {
    auto &arg = op->args[i];
    PrintFuncArg(arg);
    os() << ", ";
  }
  if (!op->args.empty()) {
    PrintFuncArg(op->args.back());
  }
  os() << ")";
}

void CodeGenCUDA_Dev::PrintFuncArg(const ir::Argument &arg) {
  if (arg.is_buffer()) {
    // In CUDA kernel, only primitive type is supported, so we replace the buffer with T*j
    if (arg.is_input()) os() << "const ";
    os() << PrintType(arg.type());
    if (!arg.type().is_cpp_handle()) {
      os() << "* ";
    }
    os() << kCKeywordRestrict << " ";
    os() << ir::BufferGetTensorName(arg.buffer_arg().As<ir::_Buffer_>());
  } else if (arg.is_var()) {
    if (arg.var_arg()->type().is_cpp_handle()) {
      os() << kCKeywordRestrict;
    }
    os() << PrintType(arg.type()) << " ";
    os() << arg.name();
  } else {
    NOT_IMPLEMENTED
  }
}

void CodeGenCUDA_Dev::PrintBuiltinCodes() {
  os() << R"ROC(
)ROC";
}

std::string CodeGenCUDA_Dev::Compile(const lang::Module &module, CodeGenC::OutputKind output_kind) {
  ss_.str("");

  if (for_nvrtc_) {
    os() << "extern \"C\" {\n\n";
  }

  if (output_kind == OutputKind::CHeader) {
    GenerateHeaderFile(module);
  } else if (output_kind == OutputKind::CImpl) {
    PrintIncludes();

    PrintBuiltinCodes();

    PrintBufferCreation(module.buffers());

    for (auto &func : module.functions()) {
      Compile(func);
    }
  } else {
    LOG(FATAL) << "Not supported OutputKind";
  }

  if (for_nvrtc_) {
    os() << "\n\n}";
  }

  return ss_.str();
}

void CodeGenCUDA_Dev::PrintIncludes() {
  os() << "#ifdef __CUDACC_RTC__\n";
  os() << "typedef int int32_t;\n";
  os() << "typedef char int8_t;\n";
  os() << "#endif\n";
  os() << "\n";
  os() << "\n";
}

}  // namespace backends
}  // namespace cinn
