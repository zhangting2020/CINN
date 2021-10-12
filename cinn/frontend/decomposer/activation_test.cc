#include <gtest/gtest.h>

#include <random>

#include "cinn/frontend/decomposer/use_decomposer.h"
#include "cinn/frontend/decomposer_registry.h"
#include "cinn/frontend/net_builder.h"
#include "cinn/frontend/pass/use_program_pass.h"
#include "cinn/frontend/program_pass.h"
#include "cinn/hlir/framework/graph.h"
#include "cinn/hlir/framework/graph_compiler.h"
#include "cinn/hlir/framework/pass.h"
#include "cinn/hlir/framework/tensor.h"
#include "cinn/hlir/op/use_ops.h"
#include "cinn/hlir/pass/use_pass.h"

namespace cinn::frontend {

void SetRandData(hlir::framework::Tensor tensor, Target target) {
  auto* data = tensor->mutable_data<float>(target);
  std::random_device seed;
  std::default_random_engine engine(seed());
  std::uniform_real_distribution<float> dist(1.f, 2.f);
  size_t num_ele = tensor->shape().numel();
  std::vector<float> random_data(num_ele);
  for (size_t i = 0; i < num_ele; i++) {
    random_data[i] = dist(engine);  // All random data
  }

#ifdef CINN_WITH_CUDA
  cudaMemcpy(data, random_data.data(), num_ele * sizeof(float), cudaMemcpyHostToDevice);
#else
  std::copy(random_data.begin(), random_data.end(), data);
#endif
}

Target GetTarget() {
#ifdef CINN_WITH_CUDA
  return common::DefaultNVGPUTarget();
#else
  return common::DefaultHostTarget();
#endif
}

void RunProgram(const NetBuilder& builder, const Target& target, const std::vector<std::string>& inputs) {
  auto prog = builder.Build();
  ProgramPass::Apply(&prog, target, {"Decomposer"});
  auto graph = std::make_shared<hlir::framework::Graph>(prog, target);
  hlir::framework::ApplyPass(graph.get(), "InferShape");
  hlir::framework::ApplyPass(graph.get(), "OpFusion");
  auto scope = BuildScope(target, graph);
  hlir::framework::GraphCompiler gc(target, scope, graph);

  auto runtime_program = gc.Build();
  for (auto& in : inputs) {
    scope->Var<hlir::framework::Tensor>(in);
    auto tensor = scope->GetTensor(in);
    SetRandData(tensor, target);
  }
  runtime_program->Execute();
}

TEST(Decomposer, relu) {
  NetBuilder builder("relu");
  auto x   = builder.CreateInput(Float(32), {20, 10});
  auto out = builder.relu(x);

  Target target                   = GetTarget();
  std::vector<std::string> inputs = {"X"};
  RunProgram(builder, target, inputs);
}

TEST(Decomposer, relu_grad) {
  NetBuilder builder("relu_grad");
  auto dout = builder.CreateInput(Float(32), {20, 10});
  auto out  = builder.CreateInput(Float(32), {20, 10});
  auto dx   = builder.relu_grad(dout, out);

  Target target                   = GetTarget();
  std::vector<std::string> inputs = {"Dout", "Out"};
  // TODO(zhangting2020): Will enable after InferDtype for CompareOp be fixed.
  // RunProgram(builder, target, inputs);
}

TEST(Decomposer, compare) {
  NetBuilder builder("relu_grad");
  auto dout = builder.CreateInput(Float(32), {20, 10});
  auto out  = builder.CreateInput(Float(32), {20, 10});
  auto dx   = builder.relu_grad(dout, out);

  Target target                   = GetTarget();
  std::vector<std::string> inputs = {"Dout", "Out"};
  // TODO(zhangting2020): Will enable after InferDtype for CompareOp fixed.
  // RunProgram(builder, target, inputs);
}

}  // namespace cinn::frontend
