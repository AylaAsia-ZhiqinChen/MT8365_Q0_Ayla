/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/compiler/tf2tensorrt/convert/convert_nodes.h"

#include <memory>
#include <unordered_map>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "absl/strings/match.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "absl/types/span.h"
#include "tensorflow/cc/framework/ops.h"
#include "tensorflow/cc/framework/scope.h"
#include "tensorflow/cc/ops/nn_ops_internal.h"
#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/compiler/tf2tensorrt/plugin/trt_plugin_factory.h"
#include "tensorflow/compiler/tf2tensorrt/utils/trt_logger.h"
#include "tensorflow/core/framework/node_def.pb.h"  // NOLINT
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/framework/tensor.pb.h"  // NOLINT
#include "tensorflow/core/framework/tensor_shape.h"
#include "tensorflow/core/framework/tensor_testutil.h"
#include "tensorflow/core/framework/types.h"
#include "tensorflow/core/grappler/costs/graph_properties.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/lib/core/status_test_util.h"
#include "tensorflow/core/lib/strings/str_util.h"
#include "tensorflow/core/lib/strings/strcat.h"
#include "tensorflow/core/platform/protobuf.h"
#include "tensorflow/core/platform/test.h"
#include "tensorflow/core/protobuf/config.pb.h"  // NOLINT
#include "tensorflow/core/public/session.h"

#if GOOGLE_CUDA
#if GOOGLE_TENSORRT
#include "cuda/include/cuda.h"
#include "cuda/include/cuda_runtime_api.h"
#include "tensorrt/include/NvInfer.h"

namespace tensorflow {
namespace tensorrt {
namespace convert {

using absl::StrCat;
using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using ::testing::NanSensitiveFloatNear;

// TODO(laigd): put this into some test utils file.
void ExpectStatus(Status status, error::Code code = error::OK,
                  const char* substr = nullptr) {
  EXPECT_EQ(code, status.code())
      << status << " vs expected error code \"" << error::Code_Name(code)
      << "\" and message \"" << substr << "\"";
  if (substr) {
    EXPECT_THAT(status.error_message(), ::testing::HasSubstr(substr)) << status;
  }
}

nvinfer1::Dims GetTestDims(const std::vector<int>& d) {
  nvinfer1::Dims dims;
  dims.nbDims = d.size();
  for (int i = 0; i < d.size(); ++i) {
    dims.d[i] = d[i];
  }
  return dims;
}

nvinfer1::DataType TfDataTypeToTrt(DataType tf_dtype) {
  switch (tf_dtype) {
    case DT_FLOAT:
      return nvinfer1::DataType::kFLOAT;
    case DT_HALF:
      return nvinfer1::DataType::kHALF;
    case DT_INT32:
      return nvinfer1::DataType::kINT32;
    default:
      QCHECK(false) << "Unexpected data type " << DataTypeString(tf_dtype);
  }
}

DataType TrtDataTypeToTf(nvinfer1::DataType trt_dtype) {
  switch (trt_dtype) {
    case nvinfer1::DataType::kFLOAT:
      return DT_FLOAT;
    case nvinfer1::DataType::kHALF:
      return DT_HALF;
    case nvinfer1::DataType::kINT32:
      return DT_INT32;
    default:
      QCHECK(false) << "Unexpected data type " << static_cast<int>(trt_dtype);
  }
}

NodeDef MakeNodeDef(const string& name, const string& op,
                    const std::vector<string>& inputs,
                    const std::map<string, AttrValue> attrs = {}) {
  NodeDef node_def;
  node_def.set_name(name);
  node_def.set_op(op);
  for (const string& input : inputs) {
    node_def.add_input(input);
  }
  for (const auto& attr : attrs) {
    (*node_def.mutable_attr())[attr.first] = attr.second;
  }
  return node_def;
}

template <typename T>
NodeDef MakeConstNodeDef(const string& name, const std::vector<T>& vals,
                         const TensorShape& shape) {
  Scope s = Scope::NewRootScope();
  Tensor t = test::AsTensor<T>(vals, shape);
  auto const_op = ops::Const(s.WithOpName(name), t);
  return const_op.node()->def();
}

template <typename T>
NodeDef MakeConstNodeDef(const string& name, const std::vector<T>& vals) {
  TensorShape shape;
  const std::vector<int32> shape_dims = {static_cast<int32>(vals.size())};
  TF_EXPECT_OK(TensorShapeUtils::MakeShape(shape_dims, &shape));
  return MakeConstNodeDef(name, vals, shape);
}

bool TrtDimsEquals(const nvinfer1::Dims& lhs, const nvinfer1::Dims& rhs) {
  if (lhs.nbDims != rhs.nbDims) return false;
  for (int i = 0; i < lhs.nbDims; ++i) {
    if (lhs.d[i] != rhs.d[i]) return false;
    // We don't check the types in the tests.
  }
  return true;
}

bool TrtDimsEqualsArray(const std::vector<int>& lhs,
                        const nvinfer1::Dims& rhs) {
  return TrtDimsEquals(GetTestDims(lhs), rhs);
}

// TODO(laigd): define a parameterized matcher that can compare against the
// vector.
void ExpectTrtDimsEqualsArray(const std::vector<int>& lhs,
                              const nvinfer1::Dims& rhs) {
  EXPECT_TRUE(TrtDimsEqualsArray(lhs, rhs))
      << "expected: " << DebugString(GetTestDims(lhs)) << "\n"
      << "  actual: " << DebugString(rhs);
}

template <typename T>
void ExpectArrayNear(const std::vector<T>& lhs, absl::Span<const T> rhs) {
  ASSERT_EQ(lhs.size(), rhs.size());
  for (int i = 0; i < lhs.size(); i++) {
    EXPECT_FLOAT_EQ(lhs[i], rhs[i]);
  }
}

// Eigen::half cannot implicitly convert to float which is required for
// EXPECT_FLOAT_EQ.
template <>
void ExpectArrayNear(const std::vector<Eigen::half>& lhs,
                     absl::Span<const Eigen::half> rhs) {
  ASSERT_EQ(lhs.size(), rhs.size());
  for (int i = 0; i < lhs.size(); i++) {
    EXPECT_FLOAT_EQ(Eigen::half_impl::half_to_float(lhs[i]),
                    Eigen::half_impl::half_to_float(rhs[i]));
  }
}

bool TrtShapedWeightsEquals(const TRT_ShapedWeights& lhs,
                            const TRT_ShapedWeights& rhs) {
  return TrtDimsEquals(lhs.shape_, rhs.shape_) && lhs.type_ == rhs.type_ &&
         lhs.GetValues() == rhs.GetValues();
}

template <typename T>
void ValidateWeights(const TRT_ShapedWeights& weights,
                     const std::vector<int>& expected_dims,
                     const std::vector<T>& expected_value) {
  ExpectTrtDimsEqualsArray(expected_dims, weights.shape_);
  ASSERT_EQ(expected_value.size(), weights.count()) << weights.DebugString();
  const T* actual_values = static_cast<const T*>(weights.GetValues());
  for (int i = 0; i < expected_value.size(); ++i) {
    EXPECT_EQ(expected_value[i], actual_values[i]);
  }
}

// Fake ITensor implementation for testing purposes.
class FakeITensor : public nvinfer1::ITensor {
 public:
  FakeITensor() : dynamic_range_(0.0f) {}

  FakeITensor(const nvinfer1::Dims& dims) : dims_(dims), dynamic_range_(0.0f) {}

  FakeITensor(const std::vector<int>& dims)
      : dims_(GetTestDims(dims)), dynamic_range_(0.0f) {}

  void setName(const char* name) override { name_ = name; }

  const char* getName() const override { return name_.c_str(); }

  void setDimensions(nvinfer1::Dims dimensions) override { dims_ = dimensions; }

  nvinfer1::Dims getDimensions() const override { return dims_; }

  void setType(nvinfer1::DataType type) override { type_ = type; }

  nvinfer1::DataType getType() const override { return type_; }

  bool isNetworkInput() const override { return false; }

  bool isNetworkOutput() const override { return false; }

  void setBroadcastAcrossBatch(bool broadcastAcrossBatch) override {}

  bool getBroadcastAcrossBatch() const override { return false; }

  nvinfer1::TensorLocation getLocation() const override { return location_; }

  void setLocation(nvinfer1::TensorLocation location) override {
    location_ = location;
  }

#if IS_TRT_VERSION_GE(5, 0, 0)
  bool setDynamicRange(float min, float max) override {
    dynamic_range_ = std::max(std::abs(min), std::abs(max));
    return true;
  }

  float getDynamicRange() const override { return dynamic_range_; }
#endif

#if IS_TRT_VERSION_GE(5, 1, 0)
  bool dynamicRangeIsSet() const override { return true; }

  void resetDynamicRange() override {}

  float getDynamicRangeMin() const override { return 0.f; }

  float getDynamicRangeMax() const override { return 0.f; }
#endif

 private:
  string name_;
  nvinfer1::Dims dims_;
  nvinfer1::DataType type_;
  nvinfer1::TensorLocation location_;
  float dynamic_range_;
};

TEST(TRT_ShapedWeights_Test, Basic) {
  // Test constructor with no arguments.
  {
    TRT_ShapedWeights weights;
    TRT_ShapedWeights copy(weights);
    for (auto ptr : {&weights, &copy}) {
      nvinfer1::Weights trt_weights = ptr->GetTrtWeights();
      EXPECT_EQ(nvinfer1::DataType::kFLOAT, trt_weights.type);
      EXPECT_EQ(nullptr, trt_weights.values);
      EXPECT_EQ(0, trt_weights.count);

      EXPECT_EQ(nullptr, ptr->GetValues());
      EXPECT_EQ(0, ptr->count());
      EXPECT_EQ(0, ptr->size_bytes());
    }
  }
  // Test constructor with DataType argument.
  {
    TRT_ShapedWeights weights(DT_FLOAT);
    TRT_ShapedWeights copy(weights);
    for (auto ptr : {&weights, &copy}) {
      nvinfer1::Weights trt_weights = ptr->GetTrtWeights();
      EXPECT_EQ(nvinfer1::DataType::kFLOAT, trt_weights.type);
      EXPECT_EQ(nullptr, trt_weights.values);
      EXPECT_EQ(0, trt_weights.count);

      EXPECT_EQ(nullptr, ptr->GetValues());
      EXPECT_EQ(0, ptr->count());
      EXPECT_EQ(0, ptr->size_bytes());
    }
  }
  // Test constructor with DataType and nvinfer1::Dims arguments.
  {
    TrtWeightStore store;
    TRT_ShapedWeights weights =
        store.GetTempWeights(DT_FLOAT, GetTestDims({2, 5}));
    TRT_ShapedWeights copy(weights);
    for (auto ptr : {&weights, &copy}) {
      nvinfer1::Weights trt_weights = ptr->GetTrtWeights();
      EXPECT_EQ(nvinfer1::DataType::kFLOAT, trt_weights.type);
      EXPECT_NE(nullptr, trt_weights.values);
      EXPECT_EQ(10, trt_weights.count);

      EXPECT_EQ(trt_weights.values, ptr->GetValues());
      EXPECT_EQ(10, ptr->count());
      EXPECT_EQ(40, ptr->size_bytes());
    }
    // Test that it doesn't copy the underlying buffer.
    EXPECT_EQ(weights.GetValues(), copy.GetValues());
  }
}

TEST(TRT_TensorOrWeights_Test, Basic) {
  // Test constructor with no arguments.
  {
    TRT_TensorOrWeights tw;
    TRT_TensorOrWeights copy(tw);
    TRT_TensorOrWeights assigned;
    assigned = tw;
    for (auto ptr : {&tw, &copy, &assigned}) {
      EXPECT_EQ(false, ptr->is_tensor());
      EXPECT_EQ(false, ptr->is_weights());
      EXPECT_EQ(-1, ptr->batch_size());
    }
  }

  // Test constructor with ITensor and batch size argument.
  {
    nvinfer1::Dims dims;
    dims.nbDims = 1;
    dims.d[0] = 1;
    FakeITensor itensor(dims);
    TRT_TensorOrWeights tw(&itensor);
    TRT_TensorOrWeights tw1(&itensor, /*batch_size=*/1);

    for (auto original_ptr : {&tw, &tw1}) {
      TRT_TensorOrWeights copy(*original_ptr);
      TRT_TensorOrWeights assigned;
      assigned = *original_ptr;

      for (auto ptr : {original_ptr, &copy, &assigned}) {
        EXPECT_EQ(true, ptr->is_tensor());
        EXPECT_EQ(false, ptr->is_weights());
        if (original_ptr == &tw) {
          EXPECT_EQ(-1, ptr->batch_size());
        } else {
          EXPECT_EQ(1, ptr->batch_size());
        }
        EXPECT_EQ(&itensor, ptr->tensor());
        ExpectTrtDimsEqualsArray({1}, ptr->GetTrtDims());
      }
    }
  }
  // Test constructor which creates and owns an ITensor.
  {
    nvinfer1::Dims dims;
    dims.nbDims = 1;
    dims.d[0] = 1;
    TRT_TensorOrWeights tw(nvinfer1::DataType::kFLOAT, dims, /*batch_size=*/1);
    TRT_TensorOrWeights copy(tw);
    TRT_TensorOrWeights assigned;
    assigned = tw;

    for (auto ptr : {&tw, &copy, &assigned}) {
      EXPECT_EQ(true, ptr->is_tensor());
      EXPECT_EQ(false, ptr->is_weights());
      EXPECT_EQ(1, ptr->batch_size());
      EXPECT_NE(nullptr, ptr->tensor());
      ExpectTrtDimsEqualsArray({1}, ptr->GetTrtDims());
    }
  }
  // Test constructor with TRT_ShapedWeights argument.
  {
    TRT_ShapedWeights weights;
    TRT_TensorOrWeights tw(weights);
    TRT_TensorOrWeights copy(tw);
    TRT_TensorOrWeights assigned;
    assigned = tw;
    for (auto ptr : {&tw, &copy, &assigned}) {
      EXPECT_EQ(false, ptr->is_tensor());
      EXPECT_EQ(true, ptr->is_weights());
      EXPECT_TRUE(TrtShapedWeightsEquals(weights, ptr->weights()));
      ExpectTrtDimsEqualsArray({}, ptr->GetTrtDims());
    }
  }
}

class ValidatorTest : public ::testing::Test {
 public:
  std::unordered_map<string, OpConverter>& op_validators() {
    return validator_.op_validators_;
  }

  Status ConvertToTensorOrWeights(
      const NodeDef& node_def, int output_port,
      const grappler::GraphProperties& graph_properties,
      TRT_TensorOrWeights* tensor_or_weights) {
    return validator_.ConvertToTensorOrWeights(
        node_def, output_port, graph_properties, tensor_or_weights);
  }

  const std::set<string>* GetQuantizeOps() { return validator_.quantize_ops; }

 protected:
  TrtNodeValidator validator_;
};

TEST_F(ValidatorTest, QuantizeOpsAreRegistered) {
  for (const string& quantize_op : *GetQuantizeOps()) {
    QCHECK(op_validators().count(quantize_op));
  }
}

TEST_F(ValidatorTest, ConvertToTensorOrWeights) {
  // Convert Const.
  {
    NodeDef node_def = MakeConstNodeDef<float>("my_const", {1.0f, 2.0f});
    TRT_TensorOrWeights output;
    grappler::GrapplerItem item;
    grappler::GraphProperties graph_properties(item);
    ExpectStatus(ConvertToTensorOrWeights(node_def, /*output_port=*/0,
                                          graph_properties, &output));
    ValidateWeights<float>(output.weights(), {2}, {1.0, 2.0});
  }

  // Helper method to run ConvertToTensorOrWeights() with predefined parameters.
  auto convert_to_tensor_or_weights = [this](const std::vector<int64>& dims,
                                             TRT_TensorOrWeights* output) {
    Scope s = Scope::NewRootScope();
    const auto attrs = ops::Placeholder::Shape(PartialTensorShape{dims});
    auto feed = ops::Placeholder(s.WithOpName("feed"), DT_FLOAT, attrs);
    auto add = ops::Add(s.WithOpName("add"), feed, feed);

    grappler::GrapplerItem item;
    TF_EXPECT_OK(s.ToGraphDef(&item.graph));
    grappler::GraphProperties graph_properties(item);
    TF_EXPECT_OK(graph_properties.InferStatically(true));
    const NodeDef& node_def = add.operation.node()->def();
    return this->ConvertToTensorOrWeights(node_def, /*output_port=*/0,
                                          graph_properties, output);
  };
  // Convert non-Const with #dims > nvinfer1::Dims::MAX_DIMS+1.
  {
    TRT_TensorOrWeights output;
    ExpectStatus(
        convert_to_tensor_or_weights(
            std::vector<int64>(nvinfer1::Dims::MAX_DIMS + 2, 1), &output),
        error::OUT_OF_RANGE, "Input tensor rank is greater than 9");
  }
  // Convert non-Const with #dims < 2.
  {
    TRT_TensorOrWeights output;
    ExpectStatus(
        convert_to_tensor_or_weights({1}, &output), error::INVALID_ARGUMENT,
        "Input tensor with rank<2 is not supported since the first dimension "
        "is treated as batch dimension by TRT");
  }
  // Convert non-Const. We test the case where the non-batch dimemsion is
  // unknown as well, to make sure the validator allows that.
  for (const int32 non_batch_dim : {-1, 2}) {
    const int32 batch_size = 12;
    TRT_TensorOrWeights output;
    ExpectStatus(
        convert_to_tensor_or_weights({batch_size, non_batch_dim}, &output));
    EXPECT_EQ(true, output.is_tensor());
    EXPECT_EQ(batch_size, output.batch_size());
    EXPECT_NE(nullptr, output.tensor());
    ExpectTrtDimsEqualsArray({non_batch_dim}, output.GetTrtDims());
  }
}

TEST_F(ValidatorTest, ValidateNode) {
  grappler::GrapplerItem item;
  grappler::GraphProperties graph_properties(item);

  bool start_conversion = false;
  bool should_fail = false;
  auto op_converter = [&start_conversion,
                       &should_fail](OpConverterParams* params) -> Status {
    if (should_fail) return errors::InvalidArgument("");
    if (!params->validation_only) start_conversion = true;
    return Status::OK();
  };
  NodeDef node_def = MakeNodeDef("my_op", "MyOp", {});

  // Validator not registered.
  ExpectStatus(validator_.ValidateNode(node_def, {}, TrtPrecisionMode::FP32,
                                       graph_properties),
               error::UNIMPLEMENTED, "Op type MyOp is not supported.");

  // Register validator.
  op_validators()["MyOp"] = op_converter;
  TF_EXPECT_OK(validator_.ValidateNode(node_def, {}, TrtPrecisionMode::FP32,
                                       graph_properties));
  EXPECT_EQ(false, start_conversion);

  // Let the converter return error.
  should_fail = true;
  ExpectStatus(validator_.ValidateNode(node_def, {}, TrtPrecisionMode::FP32,
                                       graph_properties),
               error::INVALID_ARGUMENT);

  // Test quantization ops, they're only supported in INT8 mode. The success
  // case is tested in OpConverterTest.ConvertQuantize.
  node_def = MakeNodeDef("my_op", "FakeQuantWithMinMaxArgs", {});
  ExpectStatus(validator_.ValidateNode(node_def, {}, TrtPrecisionMode::FP32,
                                       graph_properties),
               error::UNIMPLEMENTED,
               "Op type FakeQuantWithMinMaxArgs is not supported.");
}

class ConverterTest : public ::testing::Test {
 public:
  ConverterTest() {
    builder_.reset(nvinfer1::createInferBuilder(logger_));
    network_.reset(builder_->createNetwork());
    converter_.reset(new Converter(network_.get(), TrtPrecisionMode::FP32,
                                   /*use_calibration=*/false));
    weight_store_ = &converter_->weight_store_;
  }

  void AddOpConverter(const string& op_name, OpConverter op_converter) {
    converter_->op_registry_[op_name] = op_converter;
  }

  // Below we expose private methods of Converter for testing.

  Status MaybeUpdateBatchSize(int batch_size) {
    return converter_->MaybeUpdateBatchSize(batch_size);
  }

  Status AddTensorOrWeights(const string& name, TRT_TensorOrWeights input) {
    return converter_->AddTensorOrWeights(name, input);
  }

  Status GetTensorOrWeights(const string& name, TRT_TensorOrWeights* output) {
    return converter_->GetTensorOrWeights(name, output);
  }

  Status GetInputs(const NodeDef& node_def,
                   std::vector<TRT_TensorOrWeights>* inputs) const {
    return converter_->GetInputs(node_def, inputs);
  }

  Status GetWeightRange(const TRT_ShapedWeights& weights, float* out_min,
                        float* out_max) const {
    return converter_->GetWeightRange(weights, out_min, out_max);
  }

  void PropagateQuantizationRanges() {
    converter_->PropagateQuantizationRanges();
  }

  int batch_size() const { return converter_->batch_size_; }

  std::unordered_map<nvinfer1::ITensor*, float>& quantization_ranges() {
    return converter_->quantization_ranges_;
  }

 private:
  Logger logger_;
  // These members are ordered in a way such that the destruction order is:
  // converter_ -> network_ -> builder_
  TrtUniquePtrType<nvinfer1::IBuilder> builder_;
  TrtUniquePtrType<nvinfer1::INetworkDefinition> network_;

 protected:
  std::unique_ptr<Converter> converter_;
  TrtWeightStore* weight_store_;
};

TEST_F(ConverterTest, ConvertNode) {
  FakeITensor output_tensors[2];
  auto op_converter = [&output_tensors](OpConverterParams* params) -> Status {
    nvinfer1::Dims dims = params->inputs[0].tensor()->getDimensions();
    for (int i = 0; i < 2; ++i) {
      dims.d[0] += 1;
      output_tensors[i].setDimensions(dims);
      params->outputs->push_back(TRT_TensorOrWeights(&output_tensors[i]));
    }
    return Status::OK();
  };
  NodeDef node_def = MakeNodeDef("my_op", "MyOp", {"my_input"});
  TF_EXPECT_OK(converter_->AddInputTensor(
      "my_input", nvinfer1::DataType::kFLOAT, GetTestDims({123}), 1));

  // Converter not registered.
  ExpectStatus(converter_->ConvertNode(node_def), error::UNIMPLEMENTED,
               "No converter registered for op: MyOp");

  // Register the converter and retry.
  AddOpConverter("MyOp", op_converter);
  TF_EXPECT_OK(converter_->ConvertNode(node_def));

  TRT_TensorOrWeights actual_output_1;
  TF_EXPECT_OK(GetTensorOrWeights("my_op", &actual_output_1));
  EXPECT_EQ(&output_tensors[0], actual_output_1.tensor());
  EXPECT_EQ(124, actual_output_1.tensor()->getDimensions().d[0]);

  TRT_TensorOrWeights actual_output_2;
  TF_EXPECT_OK(GetTensorOrWeights("my_op:1", &actual_output_2));
  EXPECT_EQ(&output_tensors[1], actual_output_2.tensor());
  EXPECT_EQ(125, actual_output_2.tensor()->getDimensions().d[0]);
}

TEST_F(ConverterTest, AddAndGetInputs) {
  NodeDef node_def;
  node_def.add_input("^control_input");
  node_def.add_input("input");
  node_def.add_input("input:0");
  node_def.add_input("input:1");
  node_def.add_input("weird_input:2:3:4:0");

  TF_EXPECT_OK(converter_->AddInputTensor("input", nvinfer1::DataType::kFLOAT,
                                          GetTestDims({1}), 1));
  TF_EXPECT_OK(converter_->AddInputTensor("input:1", nvinfer1::DataType::kINT32,
                                          GetTestDims({2, 3}), 1));
  TF_EXPECT_OK(converter_->AddInputTensor(
      "weird_input:2:3:4", nvinfer1::DataType::kHALF, GetTestDims({5, 3}), 1));

  std::vector<TRT_TensorOrWeights> inputs;
  TF_EXPECT_OK(GetInputs(node_def, &inputs));

  EXPECT_EQ(4, inputs.size());
  EXPECT_EQ(inputs[0].tensor(), inputs[1].tensor());

  EXPECT_EQ(nvinfer1::DataType::kFLOAT, inputs[0].tensor()->getType());
  EXPECT_EQ(nvinfer1::DataType::kINT32, inputs[2].tensor()->getType());
  EXPECT_EQ(nvinfer1::DataType::kHALF, inputs[3].tensor()->getType());
  ExpectTrtDimsEqualsArray({1}, inputs[0].tensor()->getDimensions());
  ExpectTrtDimsEqualsArray({2, 3}, inputs[2].tensor()->getDimensions());
  ExpectTrtDimsEqualsArray({5, 3}, inputs[3].tensor()->getDimensions());
}

TEST_F(ConverterTest, RenameAndMarkOutputTensors) {
  // Test that the tensor are actually named and marked as output after
  // Converter::RenameAndMarkOutputTensors() is called.

  // Register a custom converter which shuffles the input. We use it to build a
  // TRT network whose output will be later marked.
  std::vector<nvinfer1::ITensor*> output_tensors;
  auto op_converter = [&output_tensors](OpConverterParams* params) -> Status {
    nvinfer1::Permutation perm;
    perm.order[0] = 1;
    perm.order[1] = 0;
    for (int i = 0; i < 2; ++i) {
      nvinfer1::ITensor* input_tensor =
          const_cast<nvinfer1::ITensor*>(params->inputs[0].tensor());
      nvinfer1::IShuffleLayer* layer =
          params->converter->network()->addShuffle(*input_tensor);
      layer->setFirstTranspose(perm);
      nvinfer1::ITensor* output_tensor = layer->getOutput(0);
      params->outputs->emplace_back(output_tensor);
      output_tensors.push_back(output_tensor);
    }
    TRT_ShapedWeights output_weights(DT_FLOAT);
    params->outputs->emplace_back(output_weights);
    return Status::OK();
  };
  AddOpConverter("MyOp", op_converter);

  // Run the conversion.
  NodeDef node_def = MakeNodeDef("my_op", "MyOp", {"my_input"});
  TF_EXPECT_OK(converter_->AddInputTensor(
      "my_input", nvinfer1::DataType::kFLOAT, GetTestDims({1, 2}), 1));
  TF_EXPECT_OK(converter_->ConvertNode(node_def));

  // Mark a weight as output, should fail.
  ExpectStatus(
      converter_->RenameAndMarkOutputTensors({{"my_op:2", "my_output"}}),
      error::INVALID_ARGUMENT, "Output my_op:2 is weights not tensor");

  // Mark tensors as output, should pass.
  TF_EXPECT_OK(converter_->RenameAndMarkOutputTensors(
      {{"my_op", "my_output"}, {"my_op:1", "my_output_1"}}));
  EXPECT_EQ(2, output_tensors.size());
  for (auto output_tensor : output_tensors) {
    ExpectTrtDimsEqualsArray({2, 1}, output_tensor->getDimensions());
  }
  EXPECT_EQ("my_output", string(output_tensors[0]->getName()));
  EXPECT_EQ("my_output_1", string(output_tensors[1]->getName()));
}

TEST_F(ConverterTest, TransposeTensor) {
  nvinfer1::ITensor* input_tensor = converter_->network()->addInput(
      "", nvinfer1::DataType::kFLOAT, GetTestDims({2, 3, 5}));
  const nvinfer1::ITensor* output_tensor = nullptr;

  // Rank doesn't match.
  ExpectStatus(
      converter_->TransposeTensor(input_tensor, {0, 1}, &output_tensor),
      error::INVALID_ARGUMENT,
      "Rank of perm for transpose does not match with that of the input");

  // Transpose at batch dimension.
  ExpectStatus(
      converter_->TransposeTensor(input_tensor, {1, 0, 2, 3}, &output_tensor),
      error::UNIMPLEMENTED, "Transpose at batch dimension is not supported.");

  // OK.
  TF_EXPECT_OK(
      converter_->TransposeTensor(input_tensor, {0, 3, 1, 2}, &output_tensor));
  ExpectTrtDimsEqualsArray({5, 2, 3}, output_tensor->getDimensions());
}

TEST_F(ConverterTest, PrepareTensorForShape_Tensor) {
  nvinfer1::ITensor* input_tensor = converter_->network()->addInput(
      "", nvinfer1::DataType::kFLOAT, GetTestDims({2, 3, 5}));
  TRT_TensorOrWeights tw(input_tensor);
  const nvinfer1::ITensor* output_tensor = nullptr;

  for (bool validation_only : {false, true}) {
    // Shape size doesn't match.
    ExpectStatus(
        converter_->PrepareTensorForShape(tw, GetTestDims({2, 3, 6}),
                                          validation_only, &output_tensor),
        error::INVALID_ARGUMENT, "Reshape shapes are not compatible");

    // TODO(aaroey): we should check the case where uninferred dimensions are
    // not an exact divisor of input dim ensions, e.g. for dims {-1, 7}.

    // Infer shape, ok.
    TF_EXPECT_OK(converter_->PrepareTensorForShape(
        tw, GetTestDims({-1, 2}), validation_only, &output_tensor));
    if (validation_only) {
      EXPECT_EQ(nullptr, output_tensor);
    } else {
      ExpectTrtDimsEqualsArray({15, 2}, output_tensor->getDimensions());
    }

    // Regular shape.
    TF_EXPECT_OK(converter_->PrepareTensorForShape(
        tw, GetTestDims({10, 3}), validation_only, &output_tensor));
    if (validation_only) {
      EXPECT_EQ(nullptr, output_tensor);
    } else {
      ExpectTrtDimsEqualsArray({10, 3}, output_tensor->getDimensions());
    }
  }
}

TEST_F(ConverterTest, PrepareTensorForShape_Weights) {
  TRT_ShapedWeights weights =
      weight_store_->GetTempWeights(DT_FLOAT, GetTestDims({2, 3, 5}));
  TRT_TensorOrWeights tw(weights);
  const nvinfer1::ITensor* output_tensor = nullptr;
  for (bool validation_only : {false, true}) {
    TF_EXPECT_OK(converter_->PrepareTensorForShape(
        tw, GetTestDims({10, 3}), validation_only, &output_tensor));
    if (validation_only) {
      EXPECT_EQ(nullptr, output_tensor);
    } else {
      ExpectTrtDimsEqualsArray({10, 3}, output_tensor->getDimensions());
    }
  }
}

TEST_F(ConverterTest, MaybeUpdateBatchSize) {
  EXPECT_EQ(-1, batch_size());

  TF_EXPECT_OK(MaybeUpdateBatchSize(-1));
  EXPECT_EQ(-1, batch_size());

  TF_EXPECT_OK(MaybeUpdateBatchSize(123));
  EXPECT_EQ(123, batch_size());

  TF_EXPECT_OK(MaybeUpdateBatchSize(123));
  EXPECT_EQ(123, batch_size());

  TF_EXPECT_OK(MaybeUpdateBatchSize(-1));
  EXPECT_EQ(123, batch_size());

  ExpectStatus(MaybeUpdateBatchSize(124), error::INVALID_ARGUMENT,
               "Provided batch size does not match converter batch size");
}

TEST_F(ConverterTest, AddAndGetTensorOrWeights) {
  // Add a tensor.
  FakeITensor fake_tensor;
  TRT_TensorOrWeights tensor(&fake_tensor);
  EXPECT_EQ(-1, tensor.batch_size());
  TF_EXPECT_OK(MaybeUpdateBatchSize(123));
  TF_EXPECT_OK(AddTensorOrWeights("my_tensor", tensor));

  // Get the added tensor.
  TRT_TensorOrWeights added_tensor;
  TF_EXPECT_OK(GetTensorOrWeights("my_tensor", &added_tensor));
  EXPECT_EQ(123, added_tensor.batch_size());

  // Add the same tensor again.
  ExpectStatus(AddTensorOrWeights("my_tensor", tensor), error::ALREADY_EXISTS,
               "tensor/weights my_tensor already exist");
}

template <typename T>
void TestGetWeightRange(ConverterTest* test, TrtWeightStore* weight_store) {
  TRT_ShapedWeights weights =
      weight_store->GetTempWeights(DataTypeToEnum<T>::v(), GetTestDims({2, 3}));
  const std::vector<T> values = {T(3), T(1), T(2), T(6), T(5), T(4)};
  memcpy(const_cast<void*>(weights.GetValues()), values.data(),
         weights.size_bytes());

  float out_min = 0.0f;
  float out_max = 0.0f;
  TF_EXPECT_OK(test->GetWeightRange(weights, &out_min, &out_max));
  EXPECT_EQ(1.0f, out_min);
  EXPECT_EQ(6.0f, out_max);
}

TEST_F(ConverterTest, GetWeightRange) {
  TestGetWeightRange<float>(this, weight_store_);
  TestGetWeightRange<Eigen::half>(this, weight_store_);
  TestGetWeightRange<int32>(this, weight_store_);
}

TEST_F(ConverterTest, ProvideQuantizationRange) {
  FakeITensor fake_tensor;
  // Assymetric range
  converter_->ProvideQuantizationRange(&fake_tensor, 0.0f, 6.0f);
  EXPECT_EQ(6.0f, quantization_ranges()[&fake_tensor]);
  converter_->ProvideQuantizationRange(&fake_tensor, 1.0f, 6.0f);
  EXPECT_EQ(6.0f, quantization_ranges()[&fake_tensor]);
  converter_->ProvideQuantizationRange(&fake_tensor, -8.0f, 6.0f);
  EXPECT_EQ(8.0f, quantization_ranges()[&fake_tensor]);
  converter_->ProvideQuantizationRange(&fake_tensor, -8.123f, -6.123f);
  EXPECT_EQ(8.123f, quantization_ranges()[&fake_tensor]);
  // Symmetric range
  converter_->ProvideQuantizationRange(&fake_tensor, -6.123f, 6.123f);
  EXPECT_EQ(6.123f, quantization_ranges()[&fake_tensor]);
}

TEST_F(ConverterTest, MaybeApplyQuantizationRanges) {
  // input -> infer1 -> infer2 -> infer3
  FakeITensor input, infer_1, infer_2, infer_3;
  FakeITensor not_infer;
  Converter int8_converter(/*trt_network=*/nullptr, TrtPrecisionMode::INT8,
                           /*use_calibration=*/true);
  int8_converter.ProvideQuantizationRange(&input, -5.0f, 5.0f);
  int8_converter.ProvideQuantizationRange(&not_infer, -100.0f, 100.0f);
  int8_converter.MarkQuantizationRangesAsInferrable(&input, &infer_1);
  int8_converter.MarkQuantizationRangesAsInferrable(&infer_1, &infer_2);
  int8_converter.MarkQuantizationRangesAsInferrable(&infer_2, &infer_3);

  // Input range should be inferred along the chain and applied to tensors.
  int8_converter.MaybeApplyQuantizationRanges();
#if IS_TRT_VERSION_GE(5, 0, 0)
  EXPECT_EQ(input.getDynamicRange(), 5.0f);
  EXPECT_EQ(infer_1.getDynamicRange(), 5.0f);
  EXPECT_EQ(infer_2.getDynamicRange(), 5.0f);
  EXPECT_EQ(infer_3.getDynamicRange(), 5.0f);
  EXPECT_EQ(not_infer.getDynamicRange(), 100.0f);
#endif
}

TEST_F(ConverterTest, PropagateQuantizationRanges) {
  // infer0 <-> infer1 <-> infer2 <-> infer3
  //              |
  //            infer4 <-> infer5
  FakeITensor infer[6];
  FakeITensor not_infer;
  converter_->ProvideQuantizationRange(&infer[4], -5.0f, 5.0f);
  converter_->MarkQuantizationRangesAsInferrable(&infer[0], &infer[1]);
  converter_->MarkQuantizationRangesAsInferrable(&infer[1], &infer[2]);
  converter_->MarkQuantizationRangesAsInferrable(&infer[3], &infer[2]);
  converter_->MarkQuantizationRangesAsInferrable(&infer[4], &infer[1]);
  converter_->MarkQuantizationRangesAsInferrable(&infer[4], &infer[5]);

  // Input range should be inferred along the chain.
  PropagateQuantizationRanges();
  auto ranges = quantization_ranges();
  for (int i = 0; i < 6; ++i) {
    EXPECT_EQ(5.0f, ranges[&infer[i]]);
  }
  EXPECT_EQ(ranges.count(&not_infer), 0);
}

TEST_F(ConverterTest, GetTrtBroadcastShape) {
  const bool kIsTensor = true;
  const bool kIsNotTensor = false;
  auto symmetric_test = [this](const std::vector<int>& operand_1_shape,
                               const std::vector<int>& operand_2_shape,
                               const bool operand_1_is_tensor,
                               const bool operand_2_is_tensor,
                               const std::vector<int>& expected_operand_1_shape,
                               const std::vector<int>& expected_operand_2_shape,
                               error::Code expected_code = error::OK,
                               const char* expected_error_msg_substr = nullptr,
                               const int operand_1_batch_size = -1,
                               const int operand_2_batch_size = -1) {
    auto create_tensor_or_weights = [](const std::vector<int>& shape,
                                       bool is_tensor, int batch_size = -1) {
      if (is_tensor) {
        return TRT_TensorOrWeights{nvinfer1::DataType::kFLOAT,
                                   GetTestDims(shape), batch_size};
      }
      TRT_ShapedWeights weights;
      weights.shape_ = GetTestDims(shape);
      return TRT_TensorOrWeights(weights);
    };

    nvinfer1::Dims operand_1_new_dims, operand_2_new_dims;
    TRT_TensorOrWeights operand_1 = create_tensor_or_weights(
        operand_1_shape, operand_1_is_tensor, operand_1_batch_size);
    TRT_TensorOrWeights operand_2 = create_tensor_or_weights(
        operand_2_shape, operand_2_is_tensor, operand_2_batch_size);

    // operand_1 broadcast operand_2
    ExpectStatus(
        this->converter_->GetTrtBroadcastShape(
            operand_1, operand_2, &operand_1_new_dims, &operand_2_new_dims),
        expected_code, expected_error_msg_substr);
    if (expected_code == error::OK) {
      ExpectTrtDimsEqualsArray(expected_operand_1_shape, operand_1_new_dims);
      ExpectTrtDimsEqualsArray(expected_operand_2_shape, operand_2_new_dims);
    }
    // operand_2 broadcast operand_1
    ExpectStatus(
        this->converter_->GetTrtBroadcastShape(
            operand_2, operand_1, &operand_2_new_dims, &operand_1_new_dims),
        expected_code, expected_error_msg_substr);
    if (expected_code == error::OK) {
      ExpectTrtDimsEqualsArray(expected_operand_1_shape, operand_1_new_dims);
      ExpectTrtDimsEqualsArray(expected_operand_2_shape, operand_2_new_dims);
    }
  };

  // Both inputs are weights.
  symmetric_test(
      {1}, {1}, kIsNotTensor, kIsNotTensor, {}, {}, error::INVALID_ARGUMENT,
      "Broadcasting requires at least one of the operands be tensors");

  // One tensor and one weights.
  symmetric_test({1, 1, 1}, {2}, kIsTensor, kIsNotTensor, {1, 1, 1}, {1, 1, 2});
  symmetric_test({1, 1, 2}, {2}, kIsTensor, kIsNotTensor, {1, 1, 2}, {1, 1, 2});
  symmetric_test({1, 3, 2}, {1}, kIsTensor, kIsNotTensor, {1, 3, 2}, {1, 1, 1});
  symmetric_test({1, 1, 1}, {2, 3}, kIsTensor, kIsNotTensor, {1, 1, 1},
                 {1, 2, 3});
  symmetric_test({1, 1, 1}, {2, 3, 4}, kIsTensor, kIsNotTensor, {1, 1, 1},
                 {2, 3, 4});
  symmetric_test({1, 1, 1}, {1, 2, 3, 4}, kIsTensor, kIsNotTensor, {1, 1, 1},
                 {2, 3, 4});
  symmetric_test({1, 3, 4}, {1, 2, 1, 4}, kIsTensor, kIsNotTensor, {1, 3, 4},
                 {2, 1, 4});
  symmetric_test({1, 1, 1}, {2, 1, 1, 1}, kIsTensor, kIsNotTensor, {}, {},
                 error::INVALID_ARGUMENT, "Infeasible broadcast scheme");
  symmetric_test({1, 1, 1}, {2, 1, 1, 1}, kIsTensor, kIsNotTensor, {}, {},
                 error::INVALID_ARGUMENT, "Infeasible broadcast scheme",
                 /*operand_1_batch_size=*/2);
  symmetric_test({1, 1, 1}, {1, 1, 1, 1, 1}, kIsTensor, kIsNotTensor, {}, {},
                 error::INVALID_ARGUMENT,
                 "Broadcasting beyond batch dimension is not supported "
                 "(tensor #dims 4 vs broadcast #dims 5)");

  // Both inputs are tensors.
  symmetric_test({1, 1, 1}, {1, 1}, kIsTensor, kIsTensor, {}, {},
                 error::INVALID_ARGUMENT,
                 "Broadcasting beyond batch dimension is not supported "
                 "(tensor #dims 3 vs broadcast #dims 4)");
  symmetric_test({1, 3, 4}, {2, 1, 4}, kIsTensor, kIsTensor, {1, 3, 4},
                 {2, 1, 4});
  symmetric_test({1, 1, 1}, {1, 1, 1, 1}, kIsTensor, kIsTensor, {}, {},
                 error::INVALID_ARGUMENT,
                 "Broadcasting beyond batch dimension is not supported "
                 "(tensor #dims 4 vs broadcast #dims 5)");
}

TEST_F(ConverterTest, CreateConstantLayer) {
  for (auto dtype : {DT_FLOAT, DT_INT32}) {
    TRT_ShapedWeights weights =
        weight_store_->GetTempWeights(dtype, GetTestDims({2, 3, 5}));
    nvinfer1::ITensor* tensor =
        converter_->CreateConstantLayer(weights, GetTestDims({3, 10}));
    ASSERT_NE(nullptr, tensor);
    EXPECT_EQ(TfDataTypeToTrt(dtype), tensor->getType())
        << "Expected " << DebugString(TfDataTypeToTrt(dtype)) << " vs. actual "
        << DebugString(tensor->getType());
    ExpectTrtDimsEqualsArray({3, 10}, tensor->getDimensions());
  }
}

class ConvertGraphDefToEngineTest : public ::testing::Test {
 public:
  Status RunConvertGraphDefToEngine(Scope* s) {
    GraphDef gdef;
    TF_EXPECT_OK(s->ToGraphDef(&gdef));
    std::vector<PartialTensorShape> input_shapes;
    int batch_size = -1;
    for (const NodeDef& node : gdef.node()) {
      absl::string_view node_name(node.name());
      if (str_util::ConsumePrefix(&node_name, kInputPHName)) {
        int port = -1;
        EXPECT_TRUE(absl::SimpleAtoi(node_name, &port)) << node.name();
        if (input_shapes.size() < port + 1) input_shapes.resize(port + 1);
        input_shapes[port] =
            PartialTensorShape(node.attr().at("shape").shape());
        if (batch_size == -1) {
          batch_size = input_shapes[port].dim_size(0);
        } else {
          EXPECT_EQ(batch_size, input_shapes[port].dim_size(0));
        }
      }
    }
    // TODO(laigd): execute the engine and get outputs.
    return ConvertGraphDefToEngine(
        gdef, TrtPrecisionMode::FP32, /*max_batch_size=*/1,
        /*max_workspace_size_bytes=*/64 << 20, input_shapes, &logger_,
        /*allocator=*/nullptr, /*calibrator=*/nullptr, &engine_,
        /*use_calibration=*/false, /*convert_successfully=*/nullptr);
  }

 protected:
  TrtUniquePtrType<nvinfer1::ICudaEngine> engine_;

 private:
  Logger logger_;
};

TEST_F(ConvertGraphDefToEngineTest, IdentityGraph) {
  Scope s = Scope::NewRootScope();
  auto input = ops::Placeholder(s.WithOpName(StrCat(kInputPHName, 0)), DT_FLOAT,
                                ops::Placeholder::Shape({1, 1}));
  auto output = ops::Identity(s.WithOpName("identity1"), input);
  output = ops::Identity(s.WithOpName("identity2"), output);
  output = ops::Identity(s.WithOpName(StrCat(kOutputPHName, 0)), output);
  // If the converter marks the input tensor as output tensor, the conversion
  // below will fail with:
  // > TensorRTOutputPH_0 cannot be both input and output
  // > Network must have at least one output
  TF_EXPECT_OK(RunConvertGraphDefToEngine(&s));
}

// Input/output data format for OpConverterTest::BuildAndRun().
struct InputOutputData {
  void* Buffer() const {
    return const_cast<char*>(tensor.tensor_data().data());
  }

  size_t TotalBytes() const { return tensor.TotalBytes(); }

  const char* name;
  Tensor tensor;
};

template <typename T>
Tensor ConstructTensor(int data_size, const T& value = T()) {
  std::vector<T> values(data_size, value);
  return test::AsTensor<T>(values);
}

using DataVec = std::vector<InputOutputData>;

template <typename T>
inline absl::Span<const T> GetSpanForData(const InputOutputData& data) {
  const auto& tensor_map = data.tensor.flat<T>();
  return absl::Span<const T>(tensor_map.data(), tensor_map.size());
}

// Class to test various op converters, using both a TrtNodeValidator and
// Converter.
class OpConverterTest : public ::testing::Test {
 public:
  OpConverterTest() : scope_(Scope::NewRootScope()) {
    QCHECK_EQ(0, cudaStreamCreate(&stream_));
    Reset();
  }

  ~OpConverterTest() override { QCHECK_EQ(0, cudaStreamDestroy(stream_)); }

  Status GetTensorOrWeights(const string& name, TRT_TensorOrWeights* output) {
    return converter_->GetTensorOrWeights(name, output);
  }

  void Reset() {
    validator_.reset(nullptr);
    converter_.reset(nullptr);

    // Reset the INetworkDefinition.
    engine_.reset(nullptr);
    network_.reset(nullptr);
    builder_.reset(nvinfer1::createInferBuilder(logger_));
    network_.reset(builder_->createNetwork());
    builder_->setMaxBatchSize(1);
    builder_->setMaxWorkspaceSize(1 << 26);

    // Reset the validator and converter.
    validator_.reset(new TrtNodeValidator);
    converter_.reset(new Converter(network_.get(), precision_mode_to_test_,
                                   /*use_calibration=*/false));

    // Reset other related artifacts.
    scope_ = Scope::NewRootScope();
    validator_inputs_.clear();
  }

  void CheckDataTypeMatches(const DataVec& datas) {
    for (const auto& data : datas) {
      const int input_index = engine_->getBindingIndex(data.name);
      ASSERT_NE(-1, input_index);
      const nvinfer1::DataType trt_dtype =
          engine_->getBindingDataType(input_index);
      const DataType tf_dtype = TrtDataTypeToTf(trt_dtype);
      ASSERT_EQ(data.tensor.dtype(), tf_dtype)
          << DataTypeString(data.tensor.dtype()) << " vs. "
          << DataTypeString(tf_dtype);
    }
  }

  // TODO(laigd): test fp16 and int8 support for more converters.
  void BuildAndRun(const DataVec& input_data, DataVec* output_data,
                   TrtPrecisionMode precision_mode = TrtPrecisionMode::FP32) {
    // Mark the output tensor as TRT engine output.
    std::vector<Converter::EngineOutputInfo> output_info;
    for (const auto& data : *output_data) {
      output_info.push_back(
          {data.name, data.name, TfDataTypeToTrt(data.tensor.dtype())});
    }
    TF_EXPECT_OK(converter_->RenameAndMarkOutputTensors(output_info));

    // Build the TRT engine.
    if (precision_mode == TrtPrecisionMode::FP16) {
      builder_->setFp16Mode(true);
    } else if (precision_mode == TrtPrecisionMode::INT8) {
      // Setting FP16 mode as well allows TRT to also consider FP16 kernels and
      // use them in situations where they are faster than INT8 or where INT8 is
      // not supported for a given layer.
      builder_->setFp16Mode(true);
      builder_->setInt8Mode(true);
    }
    ASSERT_EQ(nullptr, engine_.get());
    engine_.reset(builder_->buildCudaEngine(*converter_->network()));
    CHECK_NOTNULL(engine_.get());
    CheckDataTypeMatches(input_data);
    CheckDataTypeMatches(*output_data);

    // Execute the TRT engine.
    const int num_bindings = input_data.size() + output_data->size();
    std::vector<void*> buffers(num_bindings);

    for (const auto& data : input_data) {
      const int input_index = engine_->getBindingIndex(data.name);
      ASSERT_EQ(0, cudaMalloc(&buffers[input_index], data.TotalBytes()));
      ASSERT_EQ(0, cudaMemcpyAsync(buffers[input_index], data.Buffer(),
                                   data.TotalBytes(), cudaMemcpyHostToDevice,
                                   stream_));
    }
    struct SizeAndIndex {
      SizeAndIndex(int in_size, int in_index)
          : size(in_size), index(in_index) {}
      int size;
      int index;
    };
    std::vector<SizeAndIndex> output_infos;
    for (const auto& data : *output_data) {
      const int output_index = engine_->getBindingIndex(data.name);
      output_infos.emplace_back(data.TotalBytes(), output_index);
      ASSERT_EQ(0, cudaMalloc(&buffers[output_index], data.TotalBytes()));
    }

    ASSERT_EQ(engine_->getNbBindings(), num_bindings);
    TrtUniquePtrType<nvinfer1::IExecutionContext> execution_context(
        engine_->createExecutionContext());
    execution_context->enqueue(/*batchSize=*/1, buffers.data(), stream_,
                               nullptr);

    for (int i = 0; i < output_infos.size(); ++i) {
      const auto& output_info = output_infos[i];
      ASSERT_EQ(0, cudaMemcpyAsync(output_data->at(i).Buffer(),
                                   buffers[output_info.index], output_info.size,
                                   cudaMemcpyDeviceToHost, stream_));
    }
    cudaStreamSynchronize(stream_);

    for (int i = 0; i < num_bindings; ++i) {
      ASSERT_EQ(0, cudaFree(buffers[i]));
    }
  }

  bool HasStaticShape(const nvinfer1::Dims& dims) const {
    if (dims.nbDims < 0) return false;
    for (int i = 0; i < dims.nbDims; ++i) {
      if (dims.d[i] < 0) return false;
    }
    return true;
  }

  // Add ITensor for both validation and conversion.
  void AddTestTensor(
      const char* name, const std::vector<int32>& dims, int batch_size = 1,
      nvinfer1::DataType trt_dtype = nvinfer1::DataType::kFLOAT) {
    DataType tf_dtype = TrtDataTypeToTf(trt_dtype);
    ops::Placeholder::Attrs attrs;
    TF_EXPECT_OK(TensorShapeUtils::MakeShape(dims, &attrs.shape_));
    attrs.shape_.InsertDim(0, batch_size);
    auto input = ops::Placeholder(scope_.WithOpName(name), tf_dtype, attrs);
    validator_inputs_[name] = input.operation.node()->def();

    // Add a real ITensor for conversion conditionally.
    const nvinfer1::Dims trt_dims = GetTestDims(dims);
    if (HasStaticShape(trt_dims)) {
      TF_EXPECT_OK(
          converter_->AddInputTensor(name, trt_dtype, trt_dims, batch_size));
      ASSERT_EQ(batch_size, converter_->batch_size_);
    }
  }

  // Add weights for both validation and conversion.
  template <typename T>
  void AddTestWeights(const char* name, const std::vector<int>& dims,
                      const std::vector<T>& values) {
    const DataType dtype = DataTypeToEnum<T>::v();
    const nvinfer1::Dims trt_dims = GetTestDims(dims);
    const int64_t num_elements = TrtDimsNumElements(trt_dims);
    QCHECK_EQ(num_elements, values.size())
        << num_elements << " vs " << values.size();
    TRT_ShapedWeights weights(dtype);
    if (num_elements) {
      weights = converter_->weight_store_.GetTempWeights(dtype, trt_dims);
      QCHECK_EQ(weights.size_bytes(), sizeof(T) * values.size())
          << weights.size_bytes() << " vs " << sizeof(T) * values.size();
      memcpy(const_cast<void*>(weights.GetValues()), values.data(),
             weights.size_bytes());
    }
    // Add weights for validation.
    TensorShape shape;
    TF_EXPECT_OK(TensorShapeUtils::MakeShape(dims, &shape));
    validator_inputs_[name] = MakeConstNodeDef<T>(name, values, shape);
    // Add weights for conversion.
    TF_EXPECT_OK(
        converter_->AddTensorOrWeights(name, TRT_TensorOrWeights{weights}));
  }

  // Test validation in validation-only mode.
  void RunValidation(const NodeDef& node_def,
                     error::Code expected_code = error::OK,
                     const char* expected_msg_substr = nullptr) {
    std::vector<std::pair<const NodeDef*, int>> input_node_and_ports;
    for (const string& input : node_def.input()) {
      input_node_and_ports.emplace_back(&validator_inputs_[input], 0);
    }
    grappler::GrapplerItem item;
    TF_EXPECT_OK(scope_.ToGraphDef(&item.graph));
    grappler::GraphProperties graph_properties(item);
    TF_EXPECT_OK(graph_properties.InferStatically(true));

    ExpectStatus(
        validator_->ValidateNode(node_def, input_node_and_ports,
                                 precision_mode_to_test_, graph_properties),
        expected_code, expected_msg_substr);
  }

  void RunConversion(const NodeDef& node_def,
                     error::Code expected_code = error::OK,
                     const char* expected_msg_substr = nullptr) {
    ExpectStatus(converter_->ConvertNode(node_def), expected_code,
                 expected_msg_substr);
  }

  // Helper method to run both validation and conversion, when the expected
  // output are same.
  void RunValidationAndConversion(const NodeDef& node_def,
                                  error::Code expected_code = error::OK,
                                  const char* expected_msg_substr = nullptr,
                                  bool should_run_conversion = true) {
    RunValidation(node_def, expected_code, expected_msg_substr);
    if (should_run_conversion) {
      RunConversion(node_def, expected_code, expected_msg_substr);
    }
  }

  // Expose quantization_ranges_ for tests
  std::unordered_map<nvinfer1::ITensor*, float>& quantization_ranges() {
    return converter_->quantization_ranges_;
  }

  std::unique_ptr<Converter> converter_;
  std::unique_ptr<TrtNodeValidator> validator_;

 protected:
  // TODO(laigd): parameterize the test and make the precision mode a parameter.
  TrtPrecisionMode precision_mode_to_test_ = TrtPrecisionMode::FP32;

 private:
  Logger logger_;
  TrtUniquePtrType<nvinfer1::IBuilder> builder_;
  TrtUniquePtrType<nvinfer1::INetworkDefinition> network_;
  TrtUniquePtrType<nvinfer1::ICudaEngine> engine_;
  cudaStream_t stream_;
  // Used to create placeholders with shape and data type information. The
  // created placeholders will be used as inputs to the node to be verified,
  // thus we need the shape and data type information to get a non-empty
  // GraphProperties.
  // TODO(laigd): consider use this Scope to create the NodeDef to verify.
  Scope scope_;
  std::unordered_map<string, NodeDef> validator_inputs_;
};

template <typename T>
void CopyTensorElements(const Tensor& tensor, protobuf::RepeatedField<T>* out) {
  out->Clear();
  if (tensor.NumElements() == 0) return;

  // TensorProto does not need to have all the elements present and can truncate
  // trailing elements with the same value for compressed representation. Such
  // elements are derived based on the tensor shape.
  const auto flat = tensor.flat<T>();
  int64 last_index = 0;
  for (int64 i = 0; i < tensor.NumElements(); ++i) {
    if (flat(i) != flat(last_index)) {
      last_index = i;
    }
  }

  int num_out_elements = last_index + 1;
  out->Reserve(num_out_elements);
  out->AddNAlreadyReserved(num_out_elements);
  const T* src = flat.data();
  T* dst = out->mutable_data();
  std::copy(src, src + num_out_elements, dst);
}

template <DataType dtype, typename InputCType, typename OutputCType>
void TestConvertConst(OpConverterTest* test) {
  NodeDef node_def;
  node_def.set_name("my_const");
  node_def.set_op("Const");

  auto reset_and_test = [&node_def, test](
                            const Tensor& tensor, const bool as_tensor_content,
                            const std::vector<int>& expected_dims,
                            const std::vector<OutputCType>& expected_value) {
    test->Reset();

    TensorProto* tensor_attr =
        (*node_def.mutable_attr())["value"].mutable_tensor();
    tensor_attr->Clear();

    if (as_tensor_content) {
      tensor.AsProtoTensorContent(tensor_attr);
    } else {
      tensor.shape().AsProto(tensor_attr->mutable_tensor_shape());
      tensor_attr->set_dtype(tensor.dtype());

      if (tensor.dtype() == DT_FLOAT) {
        CopyTensorElements<float>(tensor, tensor_attr->mutable_float_val());
      } else if (tensor.dtype() == DT_INT32) {
        CopyTensorElements<int32>(tensor, tensor_attr->mutable_int_val());
      } else {
        tensor.AsProtoField(tensor_attr);
      }
    }
    test->RunValidationAndConversion(node_def);
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(test->GetTensorOrWeights("my_const", &output));
    ValidateWeights(output.weights(), expected_dims, expected_value);
  };

  auto& attr = *node_def.mutable_attr();
  attr["dtype"].set_type(dtype);
  {
    // By default empty tensor will pick DT_FLOAT as data type and we fix it
    // here.
    Tensor t(dtype);  // Empty tensor.
    reset_and_test(t, false, {}, {});
  }
  {
    Tensor t = test::AsScalar<InputCType>(12);
    reset_and_test(t, false, {1}, {12});
    reset_and_test(t, true, {1}, {12});
  }
  {
    Tensor t = test::AsTensor<InputCType>({1, 2});
    reset_and_test(t, false, {2}, {1, 2});
    reset_and_test(t, true, {2}, {1, 2});
  }
  {
    Tensor t =
        test::AsTensor<InputCType>({1, 2, 3, 4, 5, 6}, TensorShape({2, 3}));
    reset_and_test(t, false, {2, 3}, {1, 2, 3, 4, 5, 6});
    reset_and_test(t, true, {2, 3}, {1, 2, 3, 4, 5, 6});
  }
  {
    // Set all tensor elements to the same value. Such tensors are encoded
    // using a single element list in tensor proto.
    Tensor t =
        test::AsTensor<InputCType>({1, 1, 1, 1, 1, 1}, TensorShape({2, 3}));
    reset_and_test(t, false, {2, 3}, {1, 1, 1, 1, 1, 1});
    reset_and_test(t, true, {2, 3}, {1, 1, 1, 1, 1, 1});
  }
  {
    // Set trailing tensor elements to the same value. Such tensors are
    // encoded by truncating all equal elements except the first one.
    Tensor t =
        test::AsTensor<InputCType>({2, 2, 1, 1, 1, 1}, TensorShape({2, 3}));
    reset_and_test(t, false, {2, 3}, {2, 2, 1, 1, 1, 1});
    reset_and_test(t, true, {2, 3}, {2, 2, 1, 1, 1, 1});
  }
}

TEST_F(OpConverterTest, ConvertConst) {
  {
    Reset();
    NodeDef node_def = MakeNodeDef("my_const", "Const", {"input"});
    AddTestTensor("input", {1});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Constant node is expected to have empty input list: my_const");
  }
  {
    Reset();
    NodeDef node_def = MakeConstNodeDef<double>("my_const", {});
    RunValidationAndConversion(node_def, error::INVALID_ARGUMENT,
                               "Unsupported data type double");
  }

  TestConvertConst<DT_FLOAT, float, float>(this);
  TestConvertConst<DT_INT8, int8, int32>(this);
  TestConvertConst<DT_INT32, int32, int32>(this);
}

TEST_F(OpConverterTest, ConvertTranspose) {
  {
    // Input list is empty, should fail.
    NodeDef node_def = MakeNodeDef("my_transpose", "Transpose", {});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Transpose got 0 inputs but expected 2, at my_transpose");
  }

  // Get the NodeDef for Transpose.
  Scope s = Scope::NewRootScope();
  auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
  auto weights = ops::Placeholder(s.WithOpName("weights"), DT_INT32);
  auto transpose = ops::Transpose(s.WithOpName("my_transpose"), input, weights);
  const NodeDef& node_def = transpose.operation.node()->def();

  {
    // Permutation is a tensor, should fail.
    Reset();
    AddTestTensor("input", {1, 2, 3});
    AddTestTensor("weights", {3});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "The input \"perm\" for Transpose must be a constant, at my_transpose");
  }
  {
    // Transpose at batch dimension, should fail.
    Reset();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("weights", {4}, {1, 0, 2, 3});
    RunValidationAndConversion(node_def, error::UNIMPLEMENTED,
                               "Transpose at batch dimension is not supported");
  }
  {
    // Permutation rank doesn't match, should fail.
    Reset();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("weights", {3}, {0, 1, 2});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Rank of perm for transpose does not match with that of the input.");
  }
  {
    // Ok.
    Reset();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("weights", {4}, {0, 3, 1, 2});
    RunValidationAndConversion(node_def);
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(GetTensorOrWeights("my_transpose", &output));
    EXPECT_TRUE(output.is_tensor());
    ExpectTrtDimsEqualsArray({3, 1, 2}, output.tensor()->getDimensions());

    const DataVec input_data{
        {"input", test::AsTensor<float>({1, 2, 3, 4, 5, 6})}};
    DataVec output_data{{"my_transpose", ConstructTensor<float>(6)}};
    BuildAndRun(input_data, &output_data);
    EXPECT_THAT(GetSpanForData<float>(output_data[0]),
                ElementsAre(1, 4, 2, 5, 3, 6));
  }
}

TEST_F(OpConverterTest, ConvertReshape) {
  {
    // Input list is empty, should fail.
    NodeDef node_def = MakeNodeDef("my_reshape", "Reshape", {});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Reshape got 0 inputs but expected 2, at my_reshape");
  }

  // Get the NodeDef for Reshape.
  Scope s = Scope::NewRootScope();
  auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
  auto weights = ops::Placeholder(s.WithOpName("weights"), DT_INT32);
  auto reshape = ops::Reshape(s.WithOpName("my_reshape"), input, weights);
  const NodeDef& node_def = reshape.operation.node()->def();

  {
    // Shape is a tensor, should fail.
    Reset();
    AddTestTensor("input", {1, 2, 3});
    AddTestTensor("weights", {3});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "The input \"shape\" for Reshape must be a constant, at my_reshape");
  }
  {
    // Reshape to scalar, should fail.
    Reset();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("weights", {0}, {});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "Reshape to shape=[] is not supported, at my_reshape");
  }

  struct TestParams {
    int batch_size;
    std::vector<int> tensor_dims;
    std::vector<int> shape;
  };

  // Reshape at batch dimension, should fail.
  const int kReshapeBatchDimsCases = 5;
  TestParams params[kReshapeBatchDimsCases] = {
      TestParams{1, {1, 2, 3}, {3, 1, 1, 2}},
      TestParams{1, {1, 2, -1}, {-1, 1, 1, 2}},
      TestParams{1, {1, 2, 3}, {-1, 1, 1, 2}},
      TestParams{-1, {1, 2, 3}, {1, 1, 1, 2}},
      TestParams{-1, {-1, 2, 3}, {1, 1, 1, 6}},  // TODO(laigd): it should pass.
  };
  for (int i = 0; i < kReshapeBatchDimsCases; ++i) {
    Reset();
    const std::vector<int>& dims = params[i].tensor_dims;
    AddTestTensor("input", dims, params[i].batch_size);
    AddTestWeights<int32>("weights", {4}, params[i].shape);
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "Reshape on batch dimension is not supported, at my_reshape",
        /*should_run_conversion=*/(dims[0] > 0 && dims[1] > 0 && dims[2] > 0));
  }

  // Reshape on non batch dimensions, ok.
  const int kReshapeOKCases = 3;
  TestParams ok_params[kReshapeOKCases] = {
      TestParams{-1, {1, 2, 3}, {-1, 1, 3, 2}},
      TestParams{1, {1, 2, 3}, {-1, 1, 3, 2}},
      TestParams{1, {1, 2, 3}, {1, 1, 3, 2}},
  };
  for (int i = 0; i < kReshapeOKCases; ++i) {
    Reset();
    AddTestTensor("input", ok_params[i].tensor_dims, ok_params[i].batch_size);
    AddTestWeights<int32>("weights", {4}, ok_params[i].shape);
    RunValidationAndConversion(node_def);
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(GetTensorOrWeights("my_reshape", &output));
    EXPECT_TRUE(output.is_tensor());
    ExpectTrtDimsEqualsArray({1, 3, 2}, output.tensor()->getDimensions());

    const DataVec input_data{
        {"input", test::AsTensor<float>({1, 2, 3, 4, 5, 6})}};
    DataVec output_data{{"my_reshape", ConstructTensor<float>(6)}};
    BuildAndRun(input_data, &output_data);
    EXPECT_THAT(GetSpanForData<float>(output_data[0]),
                ElementsAre(1, 2, 3, 4, 5, 6));
  }
}

TEST_F(OpConverterTest, ConvertMatMul) {
  {
    // Input list is empty, should fail.
    NodeDef node_def = MakeNodeDef("my_matmul", "MatMul", {});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "MatMul got 0 inputs but expected 2, at my_matmul");
  }

  // Get the NodeDef for MatMul.
  auto get_matmul_nodedef = [](DataType dtype, bool transpose_a,
                               bool transpose_b) -> NodeDef {
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), dtype);
    auto weights = ops::Placeholder(s.WithOpName("weights"), dtype);
    const auto matmul_attrs =
        ops::MatMul::TransposeA(transpose_a).TransposeB(transpose_b);
    auto matmul =
        ops::MatMul(s.WithOpName("my_matmul"), input, weights, matmul_attrs);
    return matmul.operation.node()->def();
  };

  {
    // Unsupported data type.
    Reset();
    NodeDef node_def = get_matmul_nodedef(DT_INT32, false, false);
    AddTestTensor("input", {2}, /*batch_size=*/1, nvinfer1::DataType::kINT32);
    AddTestWeights<int32>("weights", {2, 1}, {3, 5});
    RunValidationAndConversion(node_def, error::UNIMPLEMENTED,
                               "Data type int32 is not supported for MatMul, "
                               "must be one of [float, half], at my_matmul");
  }
  // transpose_a is set.
  for (bool transpose_b : {false, true}) {
    Reset();
    NodeDef node_def =
        get_matmul_nodedef(DT_FLOAT, /*transpose_a=*/true, transpose_b);
    AddTestTensor("input", {2}, /*batch_size=*/1);
    AddTestWeights<float>("weights", {2, 2}, {0, 1, 2, 3});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "transpose_a is not supported for TensorRT FullyConnected");
  }
  // OK.
  for (bool transpose_b : {false, true}) {
    Reset();
    NodeDef node_def =
        get_matmul_nodedef(DT_FLOAT, /*transpose_a=*/false, transpose_b);
    AddTestTensor("input", {2}, /*batch_size=*/1);
    AddTestWeights<float>("weights", {2, 2}, {0, 1, 2, 3});
    RunValidationAndConversion(node_def);
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(GetTensorOrWeights("my_matmul", &output));
    EXPECT_TRUE(output.is_tensor());
    ExpectTrtDimsEqualsArray({2}, output.tensor()->getDimensions());

    const DataVec input_data{{"input", test::AsTensor<float>({0, 1})}};
    DataVec output_data{{"my_matmul", ConstructTensor<float>(2)}};
    BuildAndRun(input_data, &output_data);
    if (transpose_b) {
      EXPECT_THAT(GetSpanForData<float>(output_data[0]), ElementsAre(1, 3));
    } else {
      EXPECT_THAT(GetSpanForData<float>(output_data[0]), ElementsAre(2, 3));
    }
  }
}

template <DataType dtype>
void TestConvertBiasAdd(OpConverterTest* test) {
  // Get the NodeDef for BiasAdd.
  auto get_biasadd_nodedef = [](const string& data_format) -> NodeDef {
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), dtype);
    auto weights = ops::Placeholder(s.WithOpName("weights"), dtype);
    const auto biasadd_attrs = ops::BiasAdd::DataFormat(data_format);
    auto biasadd =
        ops::BiasAdd(s.WithOpName("my_biasadd"), input, weights, biasadd_attrs);
    return biasadd.operation.node()->def();
  };

  typedef typename EnumToDataType<dtype>::Type CType;
  for (const string& data_format : {"NHWC", "NCHW"}) {
    for (const int trt_input_rank : {1, 2, 3, 4}) {
      test->Reset();
      NodeDef node_def = get_biasadd_nodedef(data_format);

      // Add input, dims_array will be like {2, 1, ..., 1, 3}
      std::vector<int32> dims_array(trt_input_rank, 1);
      if (trt_input_rank == 1) {
        dims_array[0] = (data_format == "NHWC" ? 3 : 2);
      } else {
        dims_array[0] = 2;
        dims_array[trt_input_rank - 1] = 3;
      }
      test->AddTestTensor("input", dims_array, /*batch_size=*/1,
                          TfDataTypeToTrt(dtype));

      // Add bias weights.
      const int channel_size = (data_format == "NHWC" ? 3 : 2);
      std::vector<CType> bias(channel_size);
      for (int i = 0; i < channel_size; ++i) {
        bias[i] = CType(i + 1);  // bias will be {1, 2, 3, ...}
      }
      test->AddTestWeights<CType>("weights", {channel_size}, bias);

      // Run the conversion.
      test->RunValidationAndConversion(node_def);
      TRT_TensorOrWeights output;
      TF_EXPECT_OK(test->GetTensorOrWeights("my_biasadd", &output));
      EXPECT_TRUE(output.is_tensor());
      ExpectTrtDimsEqualsArray(dims_array, output.tensor()->getDimensions());

      // Build and run the engine.
      const int num_input = TrtDimsNumElements(GetTestDims(dims_array));
      ASSERT_EQ(trt_input_rank > 1 ? 6 : (data_format == "NHWC" ? 3 : 2),
                num_input);

      const DataVec input_data{
          {"input", ConstructTensor<CType>(num_input, CType(0))}};
      DataVec output_data{{"my_biasadd", ConstructTensor<CType>(num_input)}};
      test->BuildAndRun(input_data, &output_data);
      if (trt_input_rank == 1) {
        if (data_format == "NHWC") {
          EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                      ElementsAre(CType(1), CType(2), CType(3)));
        } else {
          EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                      ElementsAre(CType(1), CType(2)));
        }
      } else {
        if (data_format == "NHWC") {
          EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                      ElementsAre(CType(1), CType(2), CType(3), CType(1),
                                  CType(2), CType(3)));
        } else {
          EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                      ElementsAre(CType(1), CType(1), CType(1), CType(2),
                                  CType(2), CType(2)));
        }
      }
    }
  }
}

TEST_F(OpConverterTest, ConvertBiasAdd) {
  {
    // Input list is empty, should fail.
    NodeDef node_def = MakeNodeDef("my_biasadd", "BiasAdd", {});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "BiasAdd got 0 inputs but expected 2, at my_biasadd");
  }

  // OK. Note that kINT32 is not supported by IScaleLayer, so we don't test
  // DT_INT32 type here.
  TestConvertBiasAdd<DT_FLOAT>(this);
  TestConvertBiasAdd<DT_HALF>(this);
}

template <typename OpType>
NodeDef GetBinaryOpNodeDef(const string& input_name_l,
                           const string& input_name_r, DataType dtype) {
  Scope s = Scope::NewRootScope();
  auto input_l = ops::Placeholder(s.WithOpName(input_name_l), dtype);
  auto input_r = ops::Placeholder(s.WithOpName(input_name_r), dtype);
  auto op = OpType(s.WithOpName("my_binary"), input_l, input_r);
  return op.operation.node()->def();
}

void CheckAddedLayers(OpConverterTest* test, bool expect_scale_layer) {
  bool element_wise_layer_found = false;
  bool scale_layer_found = false;
  for (int i = 0; i < test->converter_->network()->getNbLayers(); i++) {
    nvinfer1::ILayer* layer = test->converter_->network()->getLayer(i);
    if (dynamic_cast<nvinfer1::IScaleLayer*>(layer)) {
      scale_layer_found = true;
    } else if (dynamic_cast<nvinfer1::IElementWiseLayer*>(layer)) {
      element_wise_layer_found = true;
    }
  }
  EXPECT_EQ(expect_scale_layer, scale_layer_found);
  EXPECT_NE(expect_scale_layer, element_wise_layer_found);
}

template <typename OpType, DataType dtype>
void TestBinaryTensorOpWeightNoBroadcast(OpConverterTest* test) {
  typedef typename EnumToDataType<dtype>::Type CType;
  for (auto swap_inputs : {false, true}) {
    test->Reset();
    NodeDef node_def;
    if (swap_inputs) {
      node_def = GetBinaryOpNodeDef<OpType>("weights", "input", dtype);
    } else {
      node_def = GetBinaryOpNodeDef<OpType>("input", "weights", dtype);
    }

    const std::vector<CType> operand1{CType(3), CType(7.5)};
    const std::vector<CType> operand2{CType(2), CType(3)};

    // It requires the dims to be at least of rank 3 to apply an IScaleLayer.
    test->AddTestTensor("input", /*dims=*/{1, 1, 2}, /*batch_size=*/1,
                        TfDataTypeToTrt(dtype));
    test->AddTestWeights<CType>("weights", /*dims=*/{1, 1, 2},
                                /*values=*/swap_inputs ? operand1 : operand2);
    test->RunValidationAndConversion(node_def);

    // Make sure it does use BinaryTensorOpWeight, not BinaryTensorOpTensor.
    CheckAddedLayers(test, /*expect_scale_layer=*/true);

    // Check the dims of the output ITensor.
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(test->GetTensorOrWeights("my_binary", &output));
    EXPECT_TRUE(output.is_tensor());
    ExpectTrtDimsEqualsArray({1, 1, 2}, output.tensor()->getDimensions());

    const DataVec input_data{
        {"input", test::AsTensor<CType>(swap_inputs ? operand2 : operand1)}};
    DataVec output_data{{"my_binary", ConstructTensor<CType>(2)}};
    test->BuildAndRun(
        input_data, &output_data,
        dtype == DT_HALF ? TrtPrecisionMode::FP16 : TrtPrecisionMode::FP32);
    if (node_def.op() == "Add") {
      EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                  ElementsAre(CType(5), CType(10.5)));
    } else if (node_def.op() == "Sub") {
      EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                  ElementsAre(CType(1), CType(4.5)));
    } else if (node_def.op() == "Mul") {
      EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                  ElementsAre(CType(6), CType(22.5)));
    } else if (node_def.op() == "Div") {
      EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                  ElementsAre(CType(1.5), CType(2.5)));
    } else if (node_def.op() == "RealDiv") {
      EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                  ElementsAre(CType(1.5), CType(2.5)));
    } else {
      ASSERT_TRUE(false);
    }
  }
}

template <DataType dtype>
void TestBinaryTensorOpWeightWithChannelWiseBroadcast(OpConverterTest* test) {
  typedef typename EnumToDataType<dtype>::Type CType;
  const NodeDef node_def =
      GetBinaryOpNodeDef<ops::Add>("input", "weights", dtype);
  const std::vector<CType> input{CType(1), CType(2), CType(3), CType(4)};
  const std::vector<CType> weights{CType(10), CType(20)};
  // There are two types of valid dim pairs which requires channel-wise
  // broadcasting:
  // - input dims (X Y Z) vs weights dims (X 1 1)
  // - input dims (X Y Z) vs weights dims (Z)
  // Here X=Z=2 and Y=1.
  for (auto weights_dims : std::vector<std::vector<int>>{{2, 1, 1}, {2}}) {
    test->Reset();
    test->AddTestTensor("input", /*dims=*/{2, 1, 2}, /*batch_size=*/1,
                        TfDataTypeToTrt(dtype));
    test->AddTestWeights<CType>("weights", weights_dims, weights);
    test->RunValidationAndConversion(node_def);

    // Make sure it does use BinaryTensorOpWeight, not BinaryTensorOpTensor.
    CheckAddedLayers(test, /*expect_scale_layer=*/true);

    // Check the dims of the output ITensor.
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(test->GetTensorOrWeights("my_binary", &output));
    EXPECT_TRUE(output.is_tensor());
    ExpectTrtDimsEqualsArray({2, 1, 2}, output.tensor()->getDimensions());

    const DataVec input_data{{"input", test::AsTensor<CType>(input)}};
    DataVec output_data{{"my_binary", ConstructTensor<CType>(4)}};
    test->BuildAndRun(input_data, &output_data);
    if (weights_dims.size() == 1) {
      EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                  ElementsAre(CType(11), CType(22), CType(13), CType(24)));
    } else {
      EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                  ElementsAre(CType(11), CType(12), CType(23), CType(24)));
    }
  }
}

template <DataType dtype>
void TestBinaryTensorOpWeightWithUniformlyBroadcast(OpConverterTest* test) {
  typedef typename EnumToDataType<dtype>::Type CType;
  const NodeDef node_def =
      GetBinaryOpNodeDef<ops::Add>("input", "weights", dtype);
  const std::vector<CType> input{CType(1), CType(2), CType(3), CType(4)};
  const std::vector<CType> weights{CType(10)};
  test->Reset();
  test->AddTestTensor("input", /*dims=*/{2, 1, 2}, /*batch_size=*/1,
                      TfDataTypeToTrt(dtype));
  test->AddTestWeights<CType>("weights", {1, 1, 1, 1}, weights);
  test->RunValidationAndConversion(node_def);

  // Make sure it does use BinaryTensorOpWeight, not BinaryTensorOpTensor.
  CheckAddedLayers(test, /*expect_scale_layer=*/true);

  // Check the dims of the output ITensor.
  TRT_TensorOrWeights output;
  TF_EXPECT_OK(test->GetTensorOrWeights("my_binary", &output));
  EXPECT_TRUE(output.is_tensor());
  ExpectTrtDimsEqualsArray({2, 1, 2}, output.tensor()->getDimensions());

  const DataVec input_data{{"input", test::AsTensor<CType>(input)}};
  DataVec output_data{{"my_binary", ConstructTensor<CType>(4)}};
  test->BuildAndRun(input_data, &output_data);
  EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
              ElementsAre(CType(11), CType(12), CType(13), CType(14)));
}

template <typename OpType>
void TestBinaryTensorOpWeightFallback(OpConverterTest* test,
                                      const std::vector<int32>& input_dims,
                                      const std::vector<int>& weights_dims,
                                      error::Code code = error::OK,
                                      const char* error_msg_substr = nullptr,
                                      const int input_batch_size = 1) {
  const DataType dtype = DT_FLOAT;
  typedef typename EnumToDataType<dtype>::Type CType;
  const size_t num_inputs = TrtDimsNumElements(GetTestDims(input_dims));
  const size_t num_weights = TrtDimsNumElements(GetTestDims(weights_dims));

  test->Reset();
  const NodeDef node_def =
      GetBinaryOpNodeDef<OpType>("input", "weights", dtype);
  test->AddTestTensor("input", /*dims=*/input_dims, input_batch_size,
                      TfDataTypeToTrt(dtype));
  test->AddTestWeights<CType>(
      "weights", /*dims=*/weights_dims,
      /*values=*/std::vector<CType>(num_weights, CType(1)));
  test->RunValidationAndConversion(node_def, code, error_msg_substr);
  if (code != error::OK) return;

  // Make sure it does use BinaryTensorOpTensor, not BinaryTensorOpWeight.
  CheckAddedLayers(test, /*expect_scale_layer=*/false);

  TRT_TensorOrWeights output;
  TF_EXPECT_OK(test->GetTensorOrWeights("my_binary", &output));
  EXPECT_TRUE(output.is_tensor());

  // Check the dims of the output ITensor.
  std::vector<int> expected_output_dims = input_dims;
  for (int i = expected_output_dims.size() - 1, j = weights_dims.size() - 1;
       i >= 0 && j >= 0; --i, --j) {
    if (expected_output_dims[i] == 1) {
      expected_output_dims[i] = weights_dims[j];
    }
  }
  ExpectTrtDimsEqualsArray(expected_output_dims,
                           output.tensor()->getDimensions());

  // Check the result of running the engine.
  const int expected_num_outputs =
      TrtDimsNumElements(GetTestDims(expected_output_dims));
  const DataVec input_data{
      {"input", ConstructTensor<CType>(num_inputs, CType(2))}};
  DataVec output_data{
      {"my_binary", ConstructTensor<CType>(expected_num_outputs)}};
  test->BuildAndRun(input_data, &output_data);
  if (node_def.op() == "Add") {
    EXPECT_THAT(
        GetSpanForData<CType>(output_data[0]),
        ElementsAreArray(std::vector<CType>(expected_num_outputs, CType(3))));
  } else if (node_def.op() == "Minimum") {
    EXPECT_THAT(
        GetSpanForData<CType>(output_data[0]),
        ElementsAreArray(std::vector<CType>(expected_num_outputs, CType(1))));
  } else {
    ASSERT_TRUE(false);
  }
}

template <typename OpType, DataType dtype>
void TestBinaryTensorOpTensor(OpConverterTest* test) {
  typedef typename EnumToDataType<dtype>::Type CType;
  test->Reset();
  const NodeDef node_def =
      GetBinaryOpNodeDef<OpType>("input1", "input2", dtype);
  test->AddTestTensor("input1", /*dims=*/{1, 2}, /*batch_size=*/1,
                      TfDataTypeToTrt(dtype));
  test->AddTestTensor("input2", /*dims=*/{2, 1}, /*batch_size=*/1,
                      TfDataTypeToTrt(dtype));
  test->RunValidationAndConversion(node_def);

  // Make sure it does use BinaryTensorOpTensor, not BinaryTensorOpWeight.
  CheckAddedLayers(test, /*expect_scale_layer=*/false);

  // Check output dims.
  TRT_TensorOrWeights output;
  TF_EXPECT_OK(test->GetTensorOrWeights("my_binary", &output));
  EXPECT_TRUE(output.is_tensor());
  ExpectTrtDimsEqualsArray({2, 2}, output.tensor()->getDimensions());

  const DataVec input_data{
      {"input1", test::AsTensor<CType>({CType(3), CType(6)})},
      {"input2", test::AsTensor<CType>({CType(2), CType(3)})}};
  DataVec output_data{{"my_binary", ConstructTensor<CType>(4)}};
  // After broadcasting first input becomes {3, 6, 3, 6} and second input
  // becomes {2, 3, 2, 3}.
  test->BuildAndRun(
      input_data, &output_data,
      dtype == DT_HALF ? TrtPrecisionMode::FP16 : TrtPrecisionMode::FP32);
  if (node_def.op() == "Add") {
    EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                ElementsAre(CType(5), CType(8), CType(6), CType(9)));
  } else if (node_def.op() == "Sub") {
    EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                ElementsAre(CType(1), CType(4), CType(0), CType(3)));
  } else if (node_def.op() == "Mul") {
    EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                ElementsAre(CType(6), CType(12), CType(9), CType(18)));
  } else if (node_def.op() == "Div") {
    EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                ElementsAre(CType(1.5), CType(3), CType(1), CType(2)));
  } else if (node_def.op() == "RealDiv") {
    EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                ElementsAre(CType(1.5), CType(3), CType(1), CType(2)));
  } else if (node_def.op() == "Minimum") {
    EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                ElementsAre(CType(2), CType(2), CType(3), CType(3)));
  } else if (node_def.op() == "Maximum") {
    EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                ElementsAre(CType(3), CType(6), CType(3), CType(6)));
  } else if (node_def.op() == "Pow") {
    ExpectArrayNear(
        std::vector<CType>{CType(9), CType(36), CType(27), CType(216)},
        GetSpanForData<CType>(output_data[0]));
  } else {
    ASSERT_TRUE(false);
  }
}

TEST_F(OpConverterTest, ConvertBinary) {
  AttrValue dtype;
  dtype.set_type(DT_FLOAT);
  // Input size doesn't match, should fail.
  for (size_t num_inputs = 0; num_inputs < 2; ++num_inputs) {
    Reset();
    NodeDef node_def =
        MakeNodeDef("my_add", "Add", {num_inputs, "input"}, {{"T", dtype}});
    AddTestTensor("input", {1}, /*batch_size=*/1, nvinfer1::DataType::kFLOAT);
    RunValidationAndConversion(node_def, error::INVALID_ARGUMENT,
                               StrCat("Add got ", std::to_string(num_inputs),
                                      " inputs but expected 2, at my_add")
                                   .c_str());
  }
  {
    // Both inputs are weights.
    Reset();
    NodeDef node_def =
        MakeNodeDef("my_add", "Add", {"weights1", "weights2"}, {{"T", dtype}});
    AddTestWeights<float>("weights1", {1}, {1});
    AddTestWeights<float>("weights2", {1}, {1});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "Constant folding is falled back to TensorFlow, binary op received "
        "both input as constant at: my_add");
  }

  // Test BinaryTensorOpWeight() without broadcasting.
  TestBinaryTensorOpWeightNoBroadcast<ops::Add, DT_FLOAT>(this);
  TestBinaryTensorOpWeightNoBroadcast<ops::Sub, DT_FLOAT>(this);
  TestBinaryTensorOpWeightNoBroadcast<ops::Mul, DT_FLOAT>(this);
  TestBinaryTensorOpWeightNoBroadcast<ops::Div, DT_FLOAT>(this);
  TestBinaryTensorOpWeightNoBroadcast<ops::RealDiv, DT_FLOAT>(this);

  TestBinaryTensorOpWeightNoBroadcast<ops::Add, DT_HALF>(this);
  TestBinaryTensorOpWeightNoBroadcast<ops::Sub, DT_HALF>(this);
  TestBinaryTensorOpWeightNoBroadcast<ops::Mul, DT_HALF>(this);
  TestBinaryTensorOpWeightNoBroadcast<ops::Div, DT_HALF>(this);
  TestBinaryTensorOpWeightNoBroadcast<ops::RealDiv, DT_HALF>(this);

  // Test BinaryTensorOpWeight() with channel-wise broadcasting.
  TestBinaryTensorOpWeightWithChannelWiseBroadcast<DT_FLOAT>(this);

  // Test BinaryTensorOpWeight() with uniformly broadcasting.
  TestBinaryTensorOpWeightWithUniformlyBroadcast<DT_FLOAT>(this);

  // Test BinaryTensorOpWeight() falling back to BinaryTensorOpTensor().
  // Unsupported op.
  TestBinaryTensorOpWeightFallback<ops::Minimum>(this, {1, 1, 1}, {1});
  // Rank of input tensor dimension <3.
  TestBinaryTensorOpWeightFallback<ops::Add>(this, {1, 1}, {1});
  // Broadcast on batch dimension, should fail.
  TestBinaryTensorOpWeightFallback<ops::Add>(
      this, {1, 1, 1}, {2, 1, 1, 1}, error::INVALID_ARGUMENT,
      "Unsupported binary op broadcast scheme for op my_binary",
      /*input_batch_size=*/2);
  // Incompatible dims with per-channel mode.
  TestBinaryTensorOpWeightFallback<ops::Add>(this, {1, 1, 1}, {1, 2, 1});
  // Incompatible dims.
  TestBinaryTensorOpWeightFallback<ops::Add>(this, {1, 2, 1}, {2});

  // Test BinaryTensorOpTensor() with broadcasting.
  TestBinaryTensorOpTensor<ops::Add, DT_FLOAT>(this);
  TestBinaryTensorOpTensor<ops::Sub, DT_FLOAT>(this);
  TestBinaryTensorOpTensor<ops::Mul, DT_FLOAT>(this);
  TestBinaryTensorOpTensor<ops::Div, DT_FLOAT>(this);
  TestBinaryTensorOpTensor<ops::RealDiv, DT_FLOAT>(this);
  TestBinaryTensorOpTensor<ops::Minimum, DT_FLOAT>(this);
  TestBinaryTensorOpTensor<ops::Maximum, DT_FLOAT>(this);
  TestBinaryTensorOpTensor<ops::Pow, DT_FLOAT>(this);

  TestBinaryTensorOpTensor<ops::Add, DT_HALF>(this);
  TestBinaryTensorOpTensor<ops::Sub, DT_HALF>(this);
  TestBinaryTensorOpTensor<ops::Mul, DT_HALF>(this);
  TestBinaryTensorOpTensor<ops::Div, DT_HALF>(this);
  TestBinaryTensorOpTensor<ops::RealDiv, DT_HALF>(this);
  TestBinaryTensorOpTensor<ops::Minimum, DT_HALF>(this);
  TestBinaryTensorOpTensor<ops::Maximum, DT_HALF>(this);
  TestBinaryTensorOpTensor<ops::Pow, DT_HALF>(this);
}

TEST_F(OpConverterTest, ConvertQuantize) {
  precision_mode_to_test_ = TrtPrecisionMode::INT8;
  const std::pair<string, int> op_with_num_inputs[4] = {
      {"FakeQuantWithMinMaxArgs", 1},
      {"FakeQuantWithMinMaxVars", 3},
      {"QuantizeAndDequantizeV2", 3},
      {"QuantizeAndDequantizeV3", 4}};
  for (const auto& pair : op_with_num_inputs) {
    // Input list is empty, should fail.
    NodeDef node_def = MakeNodeDef("my_quantize", pair.first, {});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        StrCat(pair.first, " got 0 inputs but expected ",
               std::to_string(pair.second), ", at my_quantize")
            .c_str());
  }
  {
    // FakeQuantWithMinMaxArgs attributes are empty, should fail.
    NodeDef node_def =
        MakeNodeDef("my_quantize", "FakeQuantWithMinMaxArgs", {"input"});
    AddTestTensor("input", {1, 2, 3});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Min or max attribute not found for FakeQuantWithMinMaxArgs "
        "at my_quantize");
  }
  {
    // FakeQuantWithMinMaxArgs ranges set via attributes, ok.
    Reset();
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    auto quantize_attrs = ops::FakeQuantWithMinMaxArgs::Min(-6.0f).Max(6.0f);
    auto quantize = ops::FakeQuantWithMinMaxArgs(s.WithOpName("my_quantize"),
                                                 input, quantize_attrs);
    const NodeDef& node_def = quantize.operation.node()->def();
    AddTestTensor("input", {1, 2, 3});
    RunValidationAndConversion(node_def);
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(GetTensorOrWeights("my_quantize", &output));
    EXPECT_TRUE(output.is_tensor());
    auto ranges = quantization_ranges();
    EXPECT_EQ(1, ranges.count(output.tensor()));
    EXPECT_EQ(6.0f, ranges[output.tensor()]);
  }
  {
    // FakeQuantWithMinMaxVars ranges set via inputs, ok.
    Reset();
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    auto weights_min = ops::Placeholder(s.WithOpName("weights_min"), DT_FLOAT);
    auto weights_max = ops::Placeholder(s.WithOpName("weights_max"), DT_FLOAT);
    auto quantize = ops::FakeQuantWithMinMaxVars(
        s.WithOpName("my_quantize"), input, weights_min, weights_max);
    const NodeDef& node_def = quantize.operation.node()->def();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<float>("weights_min", {1}, {-6.0f});
    AddTestWeights<float>("weights_max", {1}, {6.0f});
    RunValidationAndConversion(node_def);
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(GetTensorOrWeights("my_quantize", &output));
    EXPECT_TRUE(output.is_tensor());
    auto ranges = quantization_ranges();
    EXPECT_EQ(1, ranges.count(output.tensor()));
    EXPECT_EQ(6.0f, ranges[output.tensor()]);
  }
  {
    // QuantizeAndDequantizeV2 ranges set via inputs, ok.
    Reset();
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    auto weights_min = ops::Placeholder(s.WithOpName("weights_min"), DT_FLOAT);
    auto weights_max = ops::Placeholder(s.WithOpName("weights_max"), DT_FLOAT);
    auto quantize = ops::QuantizeAndDequantizeV2(
        s.WithOpName("my_quantize"), input, weights_min, weights_max);
    const NodeDef& node_def = quantize.operation.node()->def();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<float>("weights_min", {1}, {-6.0f});
    AddTestWeights<float>("weights_max", {1}, {6.0f});
    RunValidationAndConversion(node_def);
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(GetTensorOrWeights("my_quantize", &output));
    EXPECT_TRUE(output.is_tensor());
    auto ranges = quantization_ranges();
    EXPECT_EQ(1, ranges.count(output.tensor()));
    EXPECT_EQ(6.0f, ranges[output.tensor()]);
  }
  {
    // QuantizeAndDequantizeV2 Range inputs are tensors, should fail.
    Reset();
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    auto weights_min = ops::Placeholder(s.WithOpName("weights_min"), DT_FLOAT);
    auto weights_max = ops::Placeholder(s.WithOpName("weights_max"), DT_FLOAT);
    auto quantize = ops::QuantizeAndDequantizeV2(
        s.WithOpName("my_quantize"), input, weights_min, weights_max);
    const NodeDef& node_def = quantize.operation.node()->def();
    AddTestTensor("input", {1, 2, 3});
    AddTestTensor("weights_min", {1});
    AddTestTensor("weights_max", {1});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "The input \"input_min\" for QuantizeAndDequantizeV2 must be a constant"
        ", at my_quantize");
  }
  {
    // QuantizeAndDequantizeV3 ranges set via inputs, ok.
    Reset();
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    auto weights_min = ops::Placeholder(s.WithOpName("weights_min"), DT_FLOAT);
    auto weights_max = ops::Placeholder(s.WithOpName("weights_max"), DT_FLOAT);
    auto num_bits = ops::Placeholder(s.WithOpName("num_bits"), DT_INT32);
    auto quantize = ops::QuantizeAndDequantizeV3(
        s.WithOpName("my_quantize"), input, weights_min, weights_max, num_bits);
    const NodeDef& node_def = quantize.operation.node()->def();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<float>("weights_min", {1}, {-6.0f});
    AddTestWeights<float>("weights_max", {1}, {6.0f});
    AddTestWeights<int>("num_bits", {1}, {8});
    RunValidationAndConversion(node_def);
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(GetTensorOrWeights("my_quantize", &output));
    EXPECT_TRUE(output.is_tensor());
    auto ranges = quantization_ranges();
    EXPECT_EQ(1, ranges.count(output.tensor()));
    EXPECT_EQ(6.0f, ranges[output.tensor()]);
  }
}

template <DataType dtype>
void TestConvertSquare(OpConverterTest* test) {
  test->Reset();
  typedef typename EnumToDataType<dtype>::Type CType;

  Scope s = Scope::NewRootScope();
  auto input = ops::Placeholder(s.WithOpName("input"), dtype);
  auto square = ops::Square(s.WithOpName("my_square"), input);
  NodeDef node_def = square.operation.node()->def();

  test->AddTestTensor("input", {1, 20}, /*batch_size=*/1,
                      TfDataTypeToTrt(dtype));
  test->RunValidationAndConversion(node_def);
  TRT_TensorOrWeights output;
  TF_EXPECT_OK(test->GetTensorOrWeights("my_square", &output));
  EXPECT_TRUE(output.is_tensor());
  ExpectTrtDimsEqualsArray({1, 20}, output.tensor()->getDimensions());

  const int num_inputs = 20;
  std::vector<CType> inputs(num_inputs);
  std::vector<CType> expected_outputs(num_inputs);
  for (int i = 0; i < num_inputs; ++i) {
    const CType value = CType(i - 9);
    inputs[i] = value;
    expected_outputs[i] = value * value;
  }
  const DataVec input_data{{"input", test::AsTensor<CType>(inputs)}};
  // Engine outputs are converted to FP16 automatically if we set FP16 mode in
  // the builder.
  DataVec output_data{{"my_square", ConstructTensor<CType>(num_inputs)}};
  test->BuildAndRun(
      input_data, &output_data,
      dtype == DT_HALF ? TrtPrecisionMode::FP16 : TrtPrecisionMode::FP32);
  ExpectArrayNear(expected_outputs, GetSpanForData<CType>(output_data[0]));
}

TEST_F(OpConverterTest, ConvertSquare) {
  {
    // Input list is empty, should fail.
    NodeDef node_def = MakeNodeDef("my_square", "Square", {});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Square got 0 inputs but expected 1, at my_square");
  }
  {
    // Input is weights, should fail.
    Reset();
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    auto square = ops::Square(s.WithOpName("my_square"), input);
    NodeDef node_def = square.operation.node()->def();
    AddTestWeights<float>("input", {1, 2, 3}, {1, 2, 3, 4, -5, 6});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "The input \"x\" for Square must be a tensor, at my_square");
  }

  // OK. Note that kINT32 is not supported by IElementWiseLayer, so we don't
  // test DT_INT32 type here.
  TestConvertSquare<DT_FLOAT>(this);
  TestConvertSquare<DT_HALF>(this);
}

TEST_F(OpConverterTest, ConvertActivation) {
  {
    // Input list is empty, should fail.
    NodeDef node_def = MakeNodeDef("my_act", "Relu", {});
    RunValidationAndConversion(node_def, error::INVALID_ARGUMENT,
                               "Relu got 0 inputs but expected 1, at my_act");
  }
  {
    // Input is weights, should fail.
    Reset();
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    auto relu = ops::Relu(s.WithOpName("my_act"), input);
    const NodeDef& node_def = relu.operation.node()->def();
    AddTestWeights<int32>("input", {1, 2, 3}, {-3, -2, -1, 0, 1, 2});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "The input \"input\" for Relu must be a tensor, at my_act");
  }

  constexpr float kAlpha = 0.2f;

  // Get nodedef for activation layer.
  auto get_act_nodedef = [](string op_name) -> NodeDef {
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    if (op_name == "LeakyRelu") {
      auto act =
          ops::internal::LeakyRelu(s.WithOpName("my_act"), input,
                                   ops::internal::LeakyRelu::Alpha(kAlpha));
      return act.operation.node()->def();
    } else if (op_name == "Relu") {
      auto act = ops::Relu(s.WithOpName("my_act"), input);
      return act.operation.node()->def();
    } else if (op_name == "Relu6") {
      auto act = ops::Relu6(s.WithOpName("my_act"), input);
      return act.operation.node()->def();
    } else if (op_name == "Sigmoid") {
      auto act = ops::Sigmoid(s.WithOpName("my_act"), input);
      return act.operation.node()->def();
    } else if (op_name == "Tanh") {
      auto act = ops::Tanh(s.WithOpName("my_act"), input);
      return act.operation.node()->def();
    }
    EXPECT_TRUE(false);
    return NodeDef();
  };
  // Get expected output for activation layer.
  auto get_act_output = [](string op_name, float input) -> float {
    if (op_name == "LeakyRelu") {
      return (input > 0.0f) ? input : input * kAlpha;
    } else if (op_name == "Relu") {
      return (input > 0.0f) ? input : 0.0f;
    } else if (op_name == "Relu6") {
      return std::min(std::max(input, 0.0f), 6.0f);
    } else if (op_name == "Sigmoid") {
      return 1.0f / (1.0f + std::exp(-input));
    } else if (op_name == "Tanh") {
      return std::tanh(input);
    }
    EXPECT_TRUE(false);
    return 0;
  };

  // Ok.
  for (const string& op_name :
       {"LeakyRelu", "Relu", "Relu6", "Sigmoid", "Tanh"}) {
    Reset();
    NodeDef node_def = get_act_nodedef(op_name);
    AddTestTensor("input", {1, 2, 3});
    RunValidationAndConversion(node_def);
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(GetTensorOrWeights("my_act", &output));
    EXPECT_TRUE(output.is_tensor());
    ExpectTrtDimsEqualsArray({1, 2, 3}, output.tensor()->getDimensions());
    if (op_name == "Relu6") {
      // Relu6 should set quantization range automatically.
      auto ranges = quantization_ranges();
      EXPECT_EQ(ranges[output.tensor()], 6.0f);
    }

    const std::vector<float> input = {-100, -2, -1, 0, 1, 100};
    const DataVec input_data{{"input", test::AsTensor<float>(input)}};
    DataVec output_data{{"my_act", ConstructTensor<float>(6)}};
    BuildAndRun(input_data, &output_data);
    for (int i = 0; i < input.size(); i++) {
      const float expected_output = get_act_output(op_name, input[i]);
      EXPECT_FLOAT_EQ(GetSpanForData<float>(output_data[0])[i],
                      expected_output);
    }
  }
}

TEST_F(OpConverterTest, ConvertExpandDims) {
  {
    // Input list is empty, should fail.
    NodeDef node_def = MakeNodeDef("my_expanddims", "ExpandDims", {});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "ExpandDims got 0 inputs but expected 2, at my_expanddims");
  }

  // Get the NodeDef for ExpandDims.
  Scope s = Scope::NewRootScope();
  auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
  auto weights = ops::Placeholder(s.WithOpName("weights"), DT_INT32);
  auto expanddims =
      ops::ExpandDims(s.WithOpName("my_expanddims"), input, weights);
  const NodeDef& node_def = expanddims.operation.node()->def();
  {
    // Input is weights, should fail.
    Reset();
    AddTestWeights<int32>("input", {1, 2, 3}, {1, 2, 3, 4, 5, 6});
    AddTestWeights<int32>("weights", {1}, {1});
    RunValidationAndConversion(node_def, error::UNIMPLEMENTED,
                               "The input \"input\" for ExpandDims must be a "
                               "tensor, at my_expanddims");
  }
  {
    // Axis is a tensor, should fail.
    Reset();
    AddTestTensor("input", {1, 2, 3});
    AddTestTensor("weights", {3});
    RunValidationAndConversion(node_def, error::UNIMPLEMENTED,
                               "The input \"axis\" for ExpandDims must be a "
                               "constant, at my_expanddims");
  }
  {
    // Add dim at batch dimension, should fail.
    Reset();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("weights", {1}, {0});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "Modifying batch dimension is not supported for ExpandDims, at "
        "my_expanddims");
  }
  {
    // Add dim at batch dimension via negative axis, should fail.
    Reset();
    AddTestTensor("input", {1, 2, 3});
    // Input is rank 4 (batch dim included)
    AddTestWeights<int32>("weights", {1}, {-5});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "Modifying batch dimension is not supported for ExpandDims, at "
        "my_expanddims");
  }
  {
    // Axis > rank(input), should fail.
    Reset();
    AddTestTensor("input", {1, 2, 3});
    // Input is rank 4 (batch dim included)
    AddTestWeights<int32>("weights", {1}, {5});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Axis for ExpandDims is invalid, must be in the range "
        "[-rank(input) - 1, rank(input)], at my_expanddims");
  }
  {
    // Axis < -rank(input)-1, should fail.
    Reset();
    AddTestTensor("input", {1, 2, 3});
    // Input is rank 4 (batch dim included)
    AddTestWeights<int32>("weights", {1}, {-6});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Axis for ExpandDims is invalid, must be in the range "
        "[-rank(input) - 1, rank(input)], at my_expanddims");
  }

  struct TestParams {
    std::vector<int> input_dims;
    int axis;
    std::vector<int> expected_output_dims;
  };

  // Ok.
  const int kExpandDimsOKCases = 8;
  TestParams ok_params[kExpandDimsOKCases] = {
      TestParams{{2, 3}, 1, {1, 2, 3}}, TestParams{{2, 3}, -3, {1, 2, 3}},
      TestParams{{2, 3}, 3, {2, 3, 1}}, TestParams{{2, 3}, -1, {2, 3, 1}},
      TestParams{{2, 3}, 2, {2, 1, 3}}, TestParams{{2, 3}, -2, {2, 1, 3}},
      TestParams{{6}, 1, {1, 6}},       TestParams{{6}, -1, {6, 1}},
  };
  for (int i = 0; i < kExpandDimsOKCases; ++i) {
    Reset();
    AddTestTensor("input", ok_params[i].input_dims);
    AddTestWeights<int32>("weights", {1}, {ok_params[i].axis});
    RunValidationAndConversion(node_def);
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(GetTensorOrWeights("my_expanddims", &output));
    EXPECT_TRUE(output.is_tensor());
    ExpectTrtDimsEqualsArray(ok_params[i].expected_output_dims,
                             output.tensor()->getDimensions());

    const DataVec input_data{
        {"input", test::AsTensor<float>({1, 2, 3, 4, 5, 6})}};
    DataVec output_data{{"my_expanddims", ConstructTensor<float>(6)}};
    BuildAndRun(input_data, &output_data);
    EXPECT_THAT(GetSpanForData<float>(output_data[0]),
                ElementsAre(1, 2, 3, 4, 5, 6));
  }
}

TEST_F(OpConverterTest, ConvertSqueeze) {
  {
    // Input list is empty, should fail.
    NodeDef node_def = MakeNodeDef("my_squeeze", "Squeeze", {});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Squeeze got 0 inputs but expected 1, at my_squeeze");
  }
  {
    // No attrs, should fail.
    Reset();
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    auto squeeze = ops::Squeeze(s.WithOpName("my_squeeze"), input);
    const NodeDef& node_def = squeeze.operation.node()->def();
    AddTestTensor("input", {1, 2, 3});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "Squeeze is only implemented for explicit dims, at my_squeeze");
  }

  // Get the NodeDef for Squeeze.
  auto get_squeeze_nodedef = [](std::vector<int> axis) -> NodeDef {
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    ops::Squeeze::Attrs squeeze_attrs;
    squeeze_attrs.axis_ = gtl::ArraySlice<int>(axis);  // non-absl ok
    auto squeeze =
        ops::Squeeze(s.WithOpName("my_squeeze"), input, squeeze_attrs);
    return squeeze.operation.node()->def();
  };

  {
    // Input is weights, should fail.
    Reset();
    NodeDef node_def = get_squeeze_nodedef({0});
    AddTestWeights<float>("input", {1, 2, 3}, {1, 2, 3, 4, 5, 6});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "The input \"input\" for Squeeze must be a tensor, at my_squeeze");
  }
  {
    // Squeeze batch dim, should fail.
    Reset();
    NodeDef node_def = get_squeeze_nodedef({0});
    AddTestTensor("input", {1, 2, 3});
    RunValidationAndConversion(node_def, error::UNIMPLEMENTED,
                               "Cannot squeeze batch dimension, at my_squeeze");
  }
  {
    // Squeeze batch dim via negative axis, should fail.
    Reset();
    NodeDef node_def = get_squeeze_nodedef({-4});
    AddTestTensor("input", {1, 2, 3});
    RunValidationAndConversion(node_def, error::UNIMPLEMENTED,
                               "Cannot squeeze batch dimension, at my_squeeze");
  }
  {
    // Squeeze >= rank(input), should fail.
    Reset();
    NodeDef node_def = get_squeeze_nodedef({4});
    AddTestTensor("input", {1, 2, 3});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Axis for Squeeze is invalid, must be in the range "
        "[-rank(input), rank(input)), at my_squeeze");
  }
  {
    // Squeeze < -rank(input), should fail.
    Reset();
    NodeDef node_def = get_squeeze_nodedef({-5});
    AddTestTensor("input", {1, 2, 3});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Axis for Squeeze is invalid, must be in the range "
        "[-rank(input), rank(input)), at my_squeeze");
  }

  struct TestParams {
    std::vector<int> input_dims;
    std::vector<int> axis;
    std::vector<int> expected_output_dims;
  };

  // Ok.
  const int kSqueezeOKCases = 10;
  TestParams ok_params[kSqueezeOKCases] = {
      TestParams{{1, 2, 3}, {1}, {2, 3}},
      TestParams{{1, 2, 3}, {-3}, {2, 3}},
      TestParams{{2, 3, 1}, {3}, {2, 3}},
      TestParams{{2, 3, 1}, {-1}, {2, 3}},
      TestParams{{1, 2, 1, 3, 1}, {1, 3, 5}, {2, 3}},
      TestParams{{1, 2, 1, 3, 1}, {3, 1, 5}, {2, 3}},
      TestParams{{1, 2, 1, 3, 1}, {-1, -3, -5}, {2, 3}},
      TestParams{{1, 2, 1, 3, 1}, {1, -3, 5}, {2, 3}},
      TestParams{{1, 6}, {1}, {6}},
      TestParams{{6, 1}, {2}, {6}},
  };
  for (int i = 0; i < kSqueezeOKCases; ++i) {
    Reset();
    NodeDef node_def = get_squeeze_nodedef(ok_params[i].axis);
    AddTestTensor("input", ok_params[i].input_dims);
    RunValidationAndConversion(node_def);
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(GetTensorOrWeights("my_squeeze", &output));
    EXPECT_TRUE(output.is_tensor());
    ExpectTrtDimsEqualsArray(ok_params[i].expected_output_dims,
                             output.tensor()->getDimensions());

    const DataVec input_data{
        {"input", test::AsTensor<float>({1, 2, 3, 4, 5, 6})}};
    DataVec output_data{{"my_squeeze", ConstructTensor<float>(6)}};
    BuildAndRun(input_data, &output_data);
    EXPECT_THAT(GetSpanForData<float>(output_data[0]),
                ElementsAre(1, 2, 3, 4, 5, 6));
  }
}

TEST_F(OpConverterTest, ConvertStridedSlice) {
  {
    // Input list is empty, should fail.
    NodeDef node_def = MakeNodeDef("my_strided_slice", "StridedSlice", {});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "StridedSlice got 0 inputs but expected 4, at my_strided_slice");
  }

  // Get nodedef for StridedSlice layer.
  auto get_strided_slice_nodedef =
      [](int64 begin_mask = 0, int64 end_mask = 0, int64 ellipsis_mask = 0,
         int64 new_axis_mask = 0, int64 shrink_axis_mask = 0) -> NodeDef {
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    auto begin = ops::Placeholder(s.WithOpName("begin"), DT_INT32);
    auto end = ops::Placeholder(s.WithOpName("end"), DT_INT32);
    auto strides = ops::Placeholder(s.WithOpName("strides"), DT_INT32);
    ops::StridedSlice::Attrs attrs = ops::StridedSlice::Attrs()
                                         .BeginMask(begin_mask)
                                         .EndMask(end_mask)
                                         .EllipsisMask(ellipsis_mask)
                                         .NewAxisMask(new_axis_mask)
                                         .ShrinkAxisMask(shrink_axis_mask);
    auto strided_slice = ops::StridedSlice(s.WithOpName("my_strided_slice"),
                                           input, begin, end, strides, attrs);
    return strided_slice.operation.node()->def();
  };

  {
    // Input is weights, should fail.
    Reset();
    NodeDef node_def = get_strided_slice_nodedef();
    AddTestWeights<int32>("input", {1, 2, 3}, {1, 2, 3, 4, 5, 6});
    AddTestWeights<int32>("begin", {4}, {0, 0, 0, 0});
    AddTestWeights<int32>("end", {4}, {1, 1, 2, 3});
    AddTestWeights<int32>("strides", {4}, {1, 1, 1, 1});
    RunValidationAndConversion(node_def, error::UNIMPLEMENTED,
                               "The input \"input\" for StridedSlice must be a "
                               "tensor, at my_strided_slice");
  }
  {
    // Begin, end, strides are tensors, should fail.
    Reset();
    NodeDef node_def = get_strided_slice_nodedef();
    AddTestTensor("input", {1, 2, 3});
    AddTestTensor("begin", {4});
    AddTestTensor("end", {4});
    AddTestTensor("strides", {4});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "The input \"begin\" for StridedSlice must be a constant, at "
        "my_strided_slice");
  }
  {
    // Non-zero ellipsis_mask, should fail.
    Reset();
    NodeDef node_def = get_strided_slice_nodedef(
        /*begin_mask=*/0, /*end_mask=*/0, /*ellipsis_mask=*/2,
        /*new_axis_mask=*/0, /*shrink_axis_mask=*/0);
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("begin", {4}, {0, 0, 0, 0});
    AddTestWeights<int32>("end", {4}, {1, 1, 2, 3});
    AddTestWeights<int32>("strides", {4}, {1, 1, 1, 1});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "ellipsis_mask is not supported for StridedSlice, at "
        "my_strided_slice");
  }
  {
    // Modify batch dim, should fail.
    Reset();
    NodeDef node_def = get_strided_slice_nodedef();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("begin", {4}, {0, 0, 0, 0});
    AddTestWeights<int32>("end", {4}, {0, 1, 2, 3});
    AddTestWeights<int32>("strides", {4}, {1, 1, 1, 1});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "TensorRT does not allow modifications to the batch dimension, at "
        "my_strided_slice");
  }
  {
    // Dynamic batch size without end_mask, should fail.
    Reset();
    NodeDef node_def = get_strided_slice_nodedef();
    AddTestTensor("input", {1, 2, 3}, /*batch_size=*/-1);
    AddTestWeights<int32>("begin", {4}, {0, 0, 0, 0});
    AddTestWeights<int32>("end", {4}, {1, 1, 2, 3});
    AddTestWeights<int32>("strides", {4}, {1, 1, 1, 1});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "TensorRT does not allow modifications to the batch dimension, at "
        "my_strided_slice");
  }
  {
    // Dynamic batch size but using end_mask, ok.
    Reset();
    NodeDef node_def = get_strided_slice_nodedef(/*begin_mask=*/0,
                                                 /*end_mask=*/1);
    AddTestTensor("input", {1, 2, 3}, /*batch_size=*/-1);
    AddTestWeights<int32>("begin", {4}, {0, 0, 0, 0});
    AddTestWeights<int32>("end", {4}, {0, 1, 2, 2});
    AddTestWeights<int32>("strides", {4}, {1, 1, 1, 1});
    RunValidationAndConversion(node_def);
  }
// TRT 5.1+ supports strides
#if IS_TRT_VERSION_GE(5, 1, 0)
  {
    // Negative strides, should fail.
    Reset();
    NodeDef node_def = get_strided_slice_nodedef();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("begin", {4}, {0, 0, 0, 0});
    AddTestWeights<int32>("end", {4}, {1, 1, 2, 3});
    AddTestWeights<int32>("strides", {4}, {1, 1, 1, -1});
    RunValidationAndConversion(node_def, error::UNIMPLEMENTED,
                               "Negative or zero stride values are not "
                               "supported for StridedSlice, at "
                               "my_strided_slice");
  }
#else
  {
    // Stride is not 1, should fail.
    Reset();
    NodeDef node_def = get_strided_slice_nodedef();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("begin", {4}, {0, 0, 0, 0});
    AddTestWeights<int32>("end", {4}, {1, 1, 2, 3});
    AddTestWeights<int32>("strides", {4}, {1, 2, 1, 3});
    RunValidationAndConversion(node_def, error::UNIMPLEMENTED,
                               "Strides other than 1 are not supported with "
                               "this version of TRT, at my_strided_slice");
  }
#endif
  {
    // Size of sliced dim is negative, should fail.
    Reset();
    NodeDef node_def = get_strided_slice_nodedef();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("begin", {4}, {0, 0, 2, 0});
    AddTestWeights<int32>("end", {4}, {1, 1, 0, 3});
    AddTestWeights<int32>("strides", {4}, {1, 1, 1, 1});
    RunValidationAndConversion(node_def, error::INVALID_ARGUMENT,
                               "\"size\" cannot be negative or zero for "
                               "StridedSlice, at my_strided_slice");
  }

  struct TestParams {
    std::vector<int> input_dims;
    std::vector<int> begin;
    std::vector<int> end;
    std::vector<int> strides;
    int begin_mask;
    int end_mask;
    std::vector<int> expected_output_dims;
    std::vector<float> expected_output;
  };

  auto get_mask = [](const std::vector<int>& mask) {
    int result = 0;
    for (int i = 0; i < mask.size(); i++) {
      if (mask[i]) result += (1 << i);
    }
    return result;
  };

  // Same input is used for all tests.
  const std::vector<float> ok_input = {1, 2, 3, 4, 5, 6};

#if IS_TRT_VERSION_GE(5, 1, 0)
  const int kStridedSliceOKCases = 23;
#else
  const int kStridedSliceOKCases = 19;
#endif
  // Ok.
  TestParams ok_params[kStridedSliceOKCases] = {
    // 2D Crop.
    TestParams{/*input_dims=*/{1, 2, 3}, /*begin=*/{0, 0, 0, 0},
               /*end=*/{0, 0, 1, 2}, /*strides=*/{1, 1, 1, 1},
               /*begin_mask=*/get_mask({0, 0, 0, 0}),
               /*end_mask=*/get_mask({1, 1, 0, 0}),
               /*expected_output_dims=*/{1, 1, 2}, /*expected_output=*/{1, 2}},
    TestParams{
        /*input_dims=*/{1, 2, 3},
        /*begin=*/{0, 0, 1, 1}, /*end=*/{0, 0, 0, 0}, /*strides=*/{1, 1, 1, 1},
        /*begin_mask=*/get_mask({0, 0, 0, 0}),
        /*end_mask=*/get_mask({1, 1, 1, 1}), /*expected_output_dims=*/{1, 1, 2},
        /*expected_output=*/{5, 6}},
    TestParams{
        /*input_dims=*/{1, 2, 3},
        /*begin=*/{0, 0, 1, 1}, /*end=*/{0, 1, 2, 3}, /*strides=*/{1, 1, 1, 1},
        /*begin_mask=*/get_mask({0, 0, 0, 0}),
        /*end_mask=*/get_mask({1, 1, 0, 0}), /*expected_output_dims=*/{1, 1, 2},
        /*expected_output=*/{5, 6}},
    // 2D Crop, with transpose.
    TestParams{
        /*input_dims=*/{2, 3, 1},
        /*begin=*/{0, 0, 0, 0}, /*end=*/{0, 1, 2, 1}, /*strides=*/{1, 1, 1, 1},
        /*begin_mask=*/get_mask({0, 0, 0, 0}),
        /*end_mask=*/get_mask({1, 0, 0, 0}), /*expected_output_dims=*/{1, 2, 1},
        /*expected_output=*/{1, 2}},
    TestParams{
        /*input_dims=*/{2, 3, 1},
        /*begin=*/{0, 1, 1, 0}, /*end=*/{0, 2, 3, 1}, /*strides=*/{1, 1, 1, 1},
        /*begin_mask=*/get_mask({0, 0, 0, 0}),
        /*end_mask=*/get_mask({1, 0, 0, 0}), /*expected_output_dims=*/{1, 2, 1},
        /*expected_output=*/{5, 6}},
    TestParams{
        /*input_dims=*/{2, 1, 3},
        /*begin=*/{0, 0, 0, 0}, /*end=*/{0, 1, 1, 2}, /*strides=*/{1, 1, 1, 1},
        /*begin_mask=*/get_mask({0, 0, 0, 0}),
        /*end_mask=*/get_mask({1, 0, 0, 0}), /*expected_output_dims=*/{1, 1, 2},
        /*expected_output=*/{1, 2}},
    TestParams{
        /*input_dims=*/{2, 1, 3},
        /*begin=*/{0, 1, 0, 1}, /*end=*/{0, 2, 1, 3}, /*strides=*/{1, 1, 1, 1},
        /*begin_mask=*/get_mask({0, 0, 0, 0}),
        /*end_mask=*/get_mask({1, 0, 0, 0}), /*expected_output_dims=*/{1, 1, 2},
        /*expected_output=*/{5, 6}},
    // 2D Crop, with reshape.
    TestParams{/*input_dims=*/{2, 3},
               /*begin=*/{0, 0, 0}, /*end=*/{0, 1, 2}, /*strides=*/{1, 1, 1},
               /*begin_mask=*/get_mask({0, 0, 0}),
               /*end_mask=*/get_mask({1, 0, 0}),
               /*expected_output_dims=*/{1, 2},
               /*expected_output=*/{1, 2}},
    TestParams{/*input_dims=*/{2, 3},
               /*begin=*/{0, 1, 1}, /*end=*/{0, 0, 0}, /*strides=*/{1, 1, 1},
               /*begin_mask=*/get_mask({0, 0, 0}),
               /*end_mask=*/get_mask({1, 1, 1}),
               /*expected_output_dims=*/{1, 2},
               /*expected_output=*/{5, 6}},
    // 1D Crop.
    TestParams{
        /*input_dims=*/{1, 2, 3},
        /*begin=*/{0, 0, 0, 0}, /*end=*/{0, 0, 0, 2}, /*strides=*/{1, 1, 1, 1},
        /*begin_mask=*/get_mask({0, 0, 0, 0}),
        /*end_mask=*/get_mask({1, 1, 1, 0}), /*expected_output_dims=*/{1, 2, 2},
        /*expected_output=*/{1, 2, 4, 5}},
    TestParams{
        /*input_dims=*/{1, 2, 3},
        /*begin=*/{0, 0, 1, 0}, /*end=*/{0, 0, 0, 0}, /*strides=*/{1, 1, 1, 1},
        /*begin_mask=*/get_mask({0, 0, 0, 0}),
        /*end_mask=*/get_mask({1, 1, 1, 1}), /*expected_output_dims=*/{1, 1, 3},
        /*expected_output=*/{4, 5, 6}},
    // 1D Crop, with transpose.
    TestParams{
        /*input_dims=*/{2, 3, 1},
        /*begin=*/{0, 0, 0, 0}, /*end=*/{0, 1, 0, 0}, /*strides=*/{1, 1, 1, 1},
        /*begin_mask=*/get_mask({0, 0, 0, 0}),
        /*end_mask=*/get_mask({1, 0, 1, 1}), /*expected_output_dims=*/{1, 3, 1},
        /*expected_output=*/{1, 2, 3}},
    TestParams{
        /*input_dims=*/{2, 3, 1},
        /*begin=*/{0, 1, 0, 0}, /*end=*/{0, 0, 0, 0}, /*strides=*/{1, 1, 1, 1},
        /*begin_mask=*/get_mask({0, 0, 0, 0}),
        /*end_mask=*/get_mask({1, 1, 1, 1}), /*expected_output_dims=*/{1, 3, 1},
        /*expected_output=*/{4, 5, 6}},
    // 1D Crop, with reshape.
    TestParams{/*input_dims=*/{6},
               /*begin=*/{0, 0}, /*end=*/{0, 3}, /*strides=*/{1, 1},
               /*begin_mask=*/get_mask({0, 0}), /*end_mask=*/get_mask({1, 0}),
               /*expected_output_dims=*/{3},
               /*expected_output=*/{1, 2, 3}},
    TestParams{/*input_dims=*/{1, 6},
               /*begin=*/{0, 0, 2}, /*end=*/{0, 0, 5}, /*strides=*/{1, 1, 1},
               /*begin_mask=*/get_mask({0, 0, 0}),
               /*end_mask=*/get_mask({1, 1, 0}),
               /*expected_output_dims=*/{1, 3},
               /*expected_output=*/{3, 4, 5}},
    TestParams{/*input_dims=*/{6, 1},
               /*begin=*/{0, 2, 0}, /*end=*/{0, 5, 0}, /*strides=*/{1, 1, 1},
               /*begin_mask=*/get_mask({0, 0, 0}),
               /*end_mask=*/get_mask({1, 0, 1}),
               /*expected_output_dims=*/{3, 1},
               /*expected_output=*/{3, 4, 5}},
    // Negative axis.
    TestParams{/*input_dims=*/{6, 1},
               /*begin=*/{0, -6, 0}, /*end=*/{0, -3, 0}, /*strides=*/{1, 1, 1},
               /*begin_mask=*/get_mask({0, 0, 0}),
               /*end_mask=*/get_mask({1, 0, 1}),
               /*expected_output_dims=*/{3, 1},
               /*expected_output=*/{1, 2, 3}},
    TestParams{/*input_dims=*/{6, 1},
               /*begin=*/{0, 0, 0}, /*end=*/{0, -1, 0}, /*strides=*/{1, 1, 1},
               /*begin_mask=*/get_mask({0, 0, 0}),
               /*end_mask=*/get_mask({1, 0, 1}),
               /*expected_output_dims=*/{5, 1},
               /*expected_output=*/{1, 2, 3, 4, 5}},
    // Clamp out of bounds begin and end.
    TestParams{/*input_dims=*/{1, 2, 3}, /*begin=*/{0, 0, -9999, -9},
               /*end=*/{0, 1, 1000, 4}, /*strides=*/{1, 1, 1, 1},
               /*begin_mask=*/get_mask({0, 0, 0, 0}),
               /*end_mask=*/get_mask({1, 0, 0, 0}),
               /*expected_output_dims=*/{1, 2, 3},
               /*expected_output=*/{1, 2, 3, 4, 5, 6}},
#if IS_TRT_VERSION_GE(5, 1, 0)
    // Strides
    TestParams{/*input_dims=*/{6},
               /*begin=*/{0, 0}, /*end=*/{0, 5}, /*strides=*/{1, 2},
               /*begin_mask=*/get_mask({0, 0}), /*end_mask=*/get_mask({1, 0}),
               /*expected_output_dims=*/{3},
               /*expected_output=*/{1, 3, 5}},
    TestParams{/*input_dims=*/{6},
               /*begin=*/{0, 0}, /*end=*/{0, 6}, /*strides=*/{1, 2},
               /*begin_mask=*/get_mask({0, 0}), /*end_mask=*/get_mask({1, 0}),
               /*expected_output_dims=*/{3},
               /*expected_output=*/{1, 3, 5}},
    TestParams{/*input_dims=*/{6},
               /*begin=*/{0, 1}, /*end=*/{0, 6}, /*strides=*/{1, 2},
               /*begin_mask=*/get_mask({0, 0}), /*end_mask=*/get_mask({1, 0}),
               /*expected_output_dims=*/{3},
               /*expected_output=*/{2, 4, 6}},
    TestParams{/*input_dims=*/{6},
               /*begin=*/{0, 2}, /*end=*/{0, 6}, /*strides=*/{1, 3},
               /*begin_mask=*/get_mask({0, 0}), /*end_mask=*/get_mask({1, 0}),
               /*expected_output_dims=*/{2},
               /*expected_output=*/{3, 6}},
#endif
  };

  for (int i = 0; i < kStridedSliceOKCases; i++) {
    Reset();
    NodeDef node_def = get_strided_slice_nodedef(ok_params[i].begin_mask,
                                                 ok_params[i].end_mask);
    AddTestTensor("input", ok_params[i].input_dims);
    AddTestWeights<int32>("begin",
                          {static_cast<int>(ok_params[i].begin.size())},
                          ok_params[i].begin);
    AddTestWeights<int32>("end", {static_cast<int>(ok_params[i].end.size())},
                          ok_params[i].end);
    AddTestWeights<int32>("strides",
                          {static_cast<int>(ok_params[i].strides.size())},
                          ok_params[i].strides);
    RunValidationAndConversion(node_def);

    TRT_TensorOrWeights output;
    TF_EXPECT_OK(GetTensorOrWeights("my_strided_slice", &output));
    EXPECT_TRUE(output.is_tensor());
    ExpectTrtDimsEqualsArray(ok_params[i].expected_output_dims,
                             output.tensor()->getDimensions());

    const DataVec input_data{{"input", test::AsTensor<float>(ok_input)}};
    DataVec output_data{
        {"my_strided_slice",
         ConstructTensor<float>(ok_params[i].expected_output.size())}};
    BuildAndRun(input_data, &output_data);
    EXPECT_THAT(GetSpanForData<float>(output_data[0]),
                ElementsAreArray(ok_params[i].expected_output));
  }
}

TEST_F(OpConverterTest, ConvertSlice) {
  // Get nodedef for Slice layer.
  auto get_slice_nodedef = []() -> NodeDef {
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    auto begin = ops::Placeholder(s.WithOpName("begin"), DT_INT32);
    auto size = ops::Placeholder(s.WithOpName("size"), DT_INT32);
    auto slice = ops::Slice(s.WithOpName("my_slice"), input, begin, size);
    return slice.operation.node()->def();
  };

  {
    // Begin is below bounds, should fail.
    Reset();
    NodeDef node_def = get_slice_nodedef();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("begin", {4}, {0, 0, -1, 0});
    AddTestWeights<int32>("size", {4}, {1, 1, 2, 3});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "\"begin\" for dimension 2 in Slice is out of range, at my_slice");
  }
  {
    // Begin is above bounds, should fail.
    Reset();
    NodeDef node_def = get_slice_nodedef();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("begin", {4}, {0, 0, 3, 0});
    AddTestWeights<int32>("size", {4}, {1, 1, 2, 3});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "\"begin\" for dimension 2 in Slice is out of range, at my_slice");
  }
  {
    // Size is below bounds, should fail.
    Reset();
    NodeDef node_def = get_slice_nodedef();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("begin", {4}, {0, 0, 0, 0});
    AddTestWeights<int32>("size", {4}, {1, 1, 2, -2});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "\"begin\" + \"size\" for dimension 3 in Slice is out of range, at "
        "my_slice");
  }
  {
    // Size is above bounds, should fail.
    Reset();
    NodeDef node_def = get_slice_nodedef();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("begin", {4}, {0, 0, 0, 0});
    AddTestWeights<int32>("size", {4}, {1, 1, 3, 3});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "\"begin\" + \"size\" for dimension 2 in Slice is out of range, at "
        "my_slice");
  }
  {
    // Modify batch dim, should fail.
    Reset();
    NodeDef node_def = get_slice_nodedef();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<int32>("begin", {4}, {0, 0, 0, 0});
    AddTestWeights<int32>("size", {4}, {0, 1, 2, 3});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "TensorRT does not allow modifications to the batch dimension, at "
        "my_slice");
  }
  {
    // Dynamic batch size with size[0] not -1, should fail.
    Reset();
    NodeDef node_def = get_slice_nodedef();
    AddTestTensor("input", {1, 2, 3}, /*batch_size=*/-1);
    AddTestWeights<int32>("begin", {4}, {0, 0, 0, 0});
    AddTestWeights<int32>("size", {4}, {1, 1, 2, 3});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "TensorRT does not allow modifications to the batch dimension, at "
        "my_slice");
  }
  {
    // Dynamic batch size but using size[0] of -1, ok.
    Reset();
    NodeDef node_def = get_slice_nodedef();
    AddTestTensor("input", {1, 2, 3}, /*batch_size=*/-1);
    AddTestWeights<int32>("begin", {4}, {0, 0, 0, 0});
    AddTestWeights<int32>("size", {4}, {-1, 1, 2, 2});
    RunValidationAndConversion(node_def);
  }

  struct TestParams {
    std::vector<int> input_dims;
    std::vector<int> begin;
    std::vector<int> size;
    std::vector<int> expected_output_dims;
    std::vector<int> expected_output;
  };

  // Ok.
  const int kSliceOKCases = 5;
  TestParams ok_params[kSliceOKCases] = {
      TestParams{{1, 2, 3},
                 {0, 0, 0, 0},
                 {-1, -1, -1, -1},
                 {1, 2, 3},
                 {1, 2, 3, 4, 5, 6}},
      TestParams{
          {1, 2, 3}, {0, 0, 0, 0}, {1, 1, 2, 3}, {1, 2, 3}, {1, 2, 3, 4, 5, 6}},
      TestParams{
          {1, 2, 3}, {0, 0, 0, 0}, {1, -1, 2, 2}, {1, 2, 2}, {1, 2, 4, 5}},
      TestParams{{6}, {0, 1}, {1, 5}, {5}, {2, 3, 4, 5, 6}},
      TestParams{{6}, {0, 1}, {-1, 3}, {3}, {2, 3, 4}},
  };

  for (int i = 0; i < kSliceOKCases; i++) {
    Reset();
    NodeDef node_def = get_slice_nodedef();
    AddTestTensor("input", ok_params[i].input_dims);
    AddTestWeights<int32>("begin",
                          {static_cast<int>(ok_params[i].begin.size())},
                          ok_params[i].begin);
    AddTestWeights<int32>("size", {static_cast<int>(ok_params[i].size.size())},
                          ok_params[i].size);
    RunValidationAndConversion(node_def);

    TRT_TensorOrWeights output;
    TF_EXPECT_OK(GetTensorOrWeights("my_slice", &output));
    EXPECT_TRUE(output.is_tensor());
    ExpectTrtDimsEqualsArray(ok_params[i].expected_output_dims,
                             output.tensor()->getDimensions());

    const DataVec input_data{
        {"input", test::AsTensor<float>({1, 2, 3, 4, 5, 6})}};
    DataVec output_data{{"my_slice", ConstructTensor<float>(
                                         ok_params[i].expected_output.size())}};
    BuildAndRun(input_data, &output_data);
    EXPECT_THAT(GetSpanForData<float>(output_data[0]),
                ElementsAreArray(ok_params[i].expected_output));
  }
}

TEST_F(OpConverterTest, ConvertConv2D) {
  {
    // Input list is empty, should fail.
    NodeDef node_def = MakeNodeDef("my_conv2d", "Conv2D", {});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Conv2D got 0 inputs but expected 2, at my_conv2d");
  }

  // Get nodedef for Conv2D layer.
  auto get_conv2d_nodedef =
      [](std::vector<int> strides = {1, 1, 1, 1}, string padding = "SAME",
         string data_format = "NCHW", std::vector<int> dilations = {1, 1, 1, 1},
         bool is_conv2d_backprop_input = false) -> NodeDef {
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    auto filter = ops::Placeholder(s.WithOpName("weights"), DT_FLOAT);
    if (is_conv2d_backprop_input) {
      auto input_sizes =
          ops::Placeholder(s.WithOpName("input_sizes"), DT_INT32);
      ops::Conv2DBackpropInput::Attrs attrs = ops::Conv2DBackpropInput::Attrs()
                                                  .DataFormat(data_format)
                                                  .Dilations(dilations);
      auto conv2d =
          ops::Conv2DBackpropInput(s.WithOpName("my_conv2d"), input_sizes,
                                   filter, input, strides, padding, attrs);
      return conv2d.operation.node()->def();
    } else {
      ops::Conv2D::Attrs attrs =
          ops::Conv2D::Attrs().DataFormat(data_format).Dilations(dilations);
      auto conv2d = ops::Conv2D(s.WithOpName("my_conv2d"), input, filter,
                                strides, padding, attrs);
      return conv2d.operation.node()->def();
    }
  };

  {
    // Input is weights, should fail.
    Reset();
    NodeDef node_def = get_conv2d_nodedef();
    AddTestWeights<float>("input", {1, 2, 3}, {1, 2, 3, 4, 5, 6});
    AddTestWeights<float>("weights", {3, 3, 1, 1}, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "The input \"input\" for Conv2D must be a tensor, at my_conv2d");
  }
  {
    // Filter is tensor, should fail.
    Reset();
    NodeDef node_def = get_conv2d_nodedef();
    AddTestTensor("input", {1, 2, 3});
    AddTestTensor("weights", {3, 3, 1, 1});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "The input \"filter\" for Conv2D must be a constant, at my_conv2d");
  }
  {
    // Filter is not 4D, should fail.
    Reset();
    NodeDef node_def = get_conv2d_nodedef();
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<float>("weights", {3, 3, 1}, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Conv2D expects kernel of dimension 4, at my_conv2d");
  }
  {
    // Dilations is not 4D, should fail.
    Reset();
    NodeDef node_def =
        get_conv2d_nodedef({1, 1, 1, 1}, "SAME", "NCHW", {1, 1, 1});
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<float>("weights", {3, 3, 1, 1}, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Convolution dilations field must specify 4 dimensions, at my_conv2d");
  }
  {
    // Dilation value is not 1 for channel, should fail.
    Reset();
    NodeDef node_def =
        get_conv2d_nodedef({1, 1, 1, 1}, "SAME", "NCHW", {1, 2, 1, 1});
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<float>("weights", {3, 3, 1, 1}, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    RunValidationAndConversion(node_def, error::UNIMPLEMENTED,
                               "Dilation rate must be 1 for batch and channel "
                               "dimensions, at my_conv2d");
  }
  {
    // Dilation value is not 1 for channel (NHWC), should fail.
    Reset();
    NodeDef node_def =
        get_conv2d_nodedef({1, 1, 1, 1}, "SAME", "NHWC", {1, 1, 1, 2});
    AddTestTensor("input", {2, 3, 1});
    AddTestWeights<float>("weights", {3, 3, 1, 1}, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    RunValidationAndConversion(node_def, error::UNIMPLEMENTED,
                               "Dilation rate must be 1 for batch and channel "
                               "dimensions, at my_conv2d");
  }
  {
    // Dilation + Conv2DBackpropInput, should fail.
    Reset();
    NodeDef node_def =
        get_conv2d_nodedef({1, 1, 1, 1}, "SAME", "NHWC", {1, 1, 2, 1}, true);
    AddTestTensor("input", {2, 3, 1});
    AddTestWeights<float>("weights", {3, 3, 1, 1}, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    AddTestWeights<int>("input_sizes", {4}, {1, 2, 3, 1});
    RunValidationAndConversion(node_def, error::UNIMPLEMENTED,
                               "Dilation with Conv2DBackpropInput "
                               "(conv2d_transpose) is not supported, "
                               "at my_conv2d");
  }
  {
    // Strides is not 4D, should fail.
    Reset();
    NodeDef node_def =
        get_conv2d_nodedef({1, 1, 1}, "SAME", "NCHW", {1, 1, 1, 1});
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<float>("weights", {3, 3, 1, 1}, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "Convolution strides field must specify 4 dimensions, at my_conv2d");
  }
  {
    // Stride value is not 1 for channel, should fail.
    Reset();
    NodeDef node_def =
        get_conv2d_nodedef({1, 2, 1, 1}, "SAME", "NCHW", {1, 1, 1, 1});
    AddTestTensor("input", {1, 2, 3});
    AddTestWeights<float>("weights", {3, 3, 1, 1}, {1, 2, 3, 4, 5, 6, 7, 8, 9});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "Stride must be 1 for batch and channel dimensions, at my_conv2d");
  }

  struct TestParams {
    std::vector<int> input_dims;
    std::vector<float> input;
    std::vector<int> filter_dims;
    std::vector<float> filter;
    std::vector<int> strides;
    string padding;
    string data_format;
    std::vector<int> dilations;
    bool is_conv2d_backprop_input;
    std::vector<int> expected_output_dims;
    std::vector<float> expected_output;
  };

  // Ok.
  const int kConv2DOKCases = 7;
  TestParams ok_params[kConv2DOKCases] = {
      // Basic
      TestParams{/*input_dims=*/{1, 2, 3},
                 /*input=*/{0, 1, 2, 3, 3, 4},
                 /*filter_dims=*/{1, 2, 1, 1},
                 /*filter=*/{-1, 1},
                 /*strides=*/{1, 1, 1, 1},
                 /*padding=*/"VALID",
                 /*data_format=*/"NCHW",
                 /*dilations=*/{1, 1, 1, 1},
                 /*is_conv2d_backprop_input=*/false,
                 /*expected_output_dims=*/{1, 2, 2},
                 /*expected_output=*/{1, 1, 0, 1}},
      // SAME padding (Asymmetric)
      TestParams{/*input_dims=*/{1, 2, 3},
                 /*input=*/{0, 1, 2, 3, 3, 4},
                 /*filter_dims=*/{1, 2, 1, 1},
                 /*filter=*/{-1, 1},
                 /*strides=*/{1, 1, 1, 1},
                 /*padding=*/"SAME",
                 /*data_format=*/"NCHW",
                 /*dilations=*/{1, 1, 1, 1},
                 /*is_conv2d_backprop_input=*/false,
                 /*expected_output_dims=*/{1, 2, 3},
                 /*expected_output=*/{1, 1, -2, 0, 1, -4}},
      // SAME padding (Symmetric)
      TestParams{/*input_dims=*/{1, 2, 3},
                 /*input=*/{0, 1, 2, 3, 3, 4},
                 /*filter_dims=*/{1, 3, 1, 1},
                 /*filter=*/{-1, 0, 1},
                 /*strides=*/{1, 1, 1, 1},
                 /*padding=*/"SAME",
                 /*data_format=*/"NCHW",
                 /*dilations=*/{1, 1, 1, 1},
                 /*is_conv2d_backprop_input=*/false,
                 /*expected_output_dims=*/{1, 2, 3},
                 /*expected_output=*/{1, 2, -1, 3, 1, -3}},
      // NHWC
      TestParams{/*input_dims=*/{2, 3, 1},
                 /*input=*/{0, 1, 2, 3, 3, 4},
                 /*filter_dims=*/{1, 2, 1, 1},
                 /*filter=*/{-1, 1},
                 /*strides=*/{1, 1, 1, 1},
                 /*padding=*/"VALID",
                 /*data_format=*/"NHWC",
                 /*dilations=*/{1, 1, 1, 1},
                 /*is_conv2d_backprop_input=*/false,
                 /*expected_output_dims=*/{2, 2, 1},
                 /*expected_output=*/{1, 1, 0, 1}},
      // Dilated
      TestParams{/*input_dims=*/{1, 2, 3},
                 /*input=*/{0, 1, 2, 3, 3, 4},
                 /*filter_dims=*/{1, 2, 1, 1},
                 /*filter=*/{-1, 1},
                 /*strides=*/{1, 1, 1, 1},
                 /*padding=*/"VALID",
                 /*data_format=*/"NCHW",
                 /*dilations=*/{1, 1, 1, 2},
                 /*is_conv2d_backprop_input=*/false,
                 /*expected_output_dims=*/{1, 2, 1},
                 /*expected_output=*/{2, 1}},
      // Strided
      TestParams{/*input_dims=*/{1, 2, 4},
                 /*input=*/{0, 1, 2, 2, 3, 4, 4, 7},
                 /*filter_dims=*/{1, 2, 1, 1},
                 /*filter=*/{-1, 1},
                 /*strides=*/{1, 1, 1, 2},
                 /*padding=*/"VALID",
                 /*data_format=*/"NCHW",
                 /*dilations=*/{1, 1, 1, 1},
                 /*is_conv2d_backprop_input=*/false,
                 /*expected_output_dims=*/{1, 2, 2},
                 /*expected_output=*/{1, 0, 1, 3}},
      // Transpose Strided
      TestParams{/*input_dims=*/{1, 2, 2},
                 /*input=*/{0, 1, 2, 3},
                 /*filter_dims=*/{1, 2, 1, 1},
                 /*filter=*/{-1, 1},
                 /*strides=*/{1, 1, 1, 2},
                 /*padding=*/"SAME",
                 /*data_format=*/"NCHW",
                 /*dilations=*/{1, 1, 1, 1},
                 /*is_conv2d_backprop_input=*/true,
                 /*expected_output_dims=*/{1, 2, 4},
                 /*expected_output=*/{0, 0, -1, 1, -2, 2, -3, 3}},
  };

  for (int i = 0; i < kConv2DOKCases; i++) {
    Reset();
    NodeDef node_def = get_conv2d_nodedef(
        ok_params[i].strides, ok_params[i].padding, ok_params[i].data_format,
        ok_params[i].dilations, ok_params[i].is_conv2d_backprop_input);
    AddTestTensor("input", ok_params[i].input_dims);
    AddTestWeights<float>("weights", ok_params[i].filter_dims,
                          ok_params[i].filter);
    if (ok_params[i].is_conv2d_backprop_input) {
      AddTestWeights<float>(
          "input_sizes",
          {static_cast<int>(ok_params[i].expected_output.size())},
          ok_params[i].expected_output);
    }
    RunValidationAndConversion(node_def);
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(GetTensorOrWeights("my_conv2d", &output));
    EXPECT_TRUE(output.is_tensor());
    ExpectTrtDimsEqualsArray(ok_params[i].expected_output_dims,
                             output.tensor()->getDimensions());

    const DataVec input_data{
        {"input", test::AsTensor<float>(ok_params[i].input)}};
    DataVec output_data{
        {"my_conv2d",
         ConstructTensor<float>(ok_params[i].expected_output.size())}};
    BuildAndRun(input_data, &output_data);
    EXPECT_THAT(GetSpanForData<float>(output_data[0]),
                ElementsAreArray(ok_params[i].expected_output));
  }
}

TEST_F(OpConverterTest, ConvertTopK) {
  {
    // Input list is empty, should fail.
    NodeDef node_def = MakeNodeDef("my_topk", "TopKV2", {});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "TopKV2 got 0 inputs but expected 2, at my_topk");
  }

  for (const auto dtype : {DT_FLOAT, DT_INT32}) {
    // Get the NodeDef for TopKV2.
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), dtype);
    auto weights = ops::Placeholder(s.WithOpName("weights"), DT_INT32);
    auto topk = ops::TopK(s.WithOpName("my_topk"), input, weights);
    const NodeDef& node_def = topk.operation.node()->def();
    {
      // K is a tensor, should fail.
      Reset();
      AddTestTensor("input", {1, 2, 3}, /*batch_size=*/1,
                    /*trt_dtype=*/TfDataTypeToTrt(dtype));
      AddTestTensor("weights", {2});
      RunValidationAndConversion(
          node_def, error::UNIMPLEMENTED,
          "The input \"k\" for TopKV2 must be a constant, at my_topk");
    }
    {
      // Ok.
      Reset();
      AddTestTensor("input", {1, 2, 5});
      AddTestWeights<int32>("weights", {1}, {2});
      RunValidationAndConversion(node_def);
      TRT_TensorOrWeights outputs[2];
      TF_EXPECT_OK(GetTensorOrWeights("my_topk", &outputs[0]));
      TF_EXPECT_OK(GetTensorOrWeights("my_topk:1", &outputs[1]));
      for (auto& output : outputs) {
        EXPECT_TRUE(output.is_tensor());
        ExpectTrtDimsEqualsArray({1, 2, 2}, output.tensor()->getDimensions());
      }

      const DataVec input_data{
          {"input", test::AsTensor<float>({-9, 3, 5, 1, 6, -5, 7, 1, 0, -1})}};
      DataVec output_data{{"my_topk", ConstructTensor<float>(4)},
                          {"my_topk:1", ConstructTensor<int32>(4)}};
      BuildAndRun(input_data, &output_data);
      EXPECT_THAT(GetSpanForData<float>(output_data[0]),
                  ElementsAre(6, 5, 7, 1));
      EXPECT_THAT(GetSpanForData<int32>(output_data[1]),
                  ElementsAre(4, 2, 1, 2));
    }
  }
}

template <DataType dtype>
void TestConvertGather(OpConverterTest* test) {
  typedef typename EnumToDataType<dtype>::Type CType;

  // Get the NodeDef for GatherV2.
  Scope s = Scope::NewRootScope();
  auto params = ops::Placeholder(s.WithOpName("params"), dtype);
  auto indices = ops::Placeholder(s.WithOpName("indices"), DT_INT32);
  auto axis = ops::Placeholder(s.WithOpName("axis"), DT_INT32);
  auto gather = ops::GatherV2(s.WithOpName("my_gather"), params, indices, axis);
  const NodeDef& node_def = gather.operation.node()->def();

  struct TestParams {
    std::vector<int> params_dims;
    std::vector<int> indices_dims;
    std::vector<int> indices;
    int axis;
    std::vector<int> expected_output_dims;
    std::vector<int> expected_output;
  };

  // Input is the same {1, 2, 3, 4, 5, 6} for all cases.
  const int kGatherOKCases = 5;
  const std::vector<CType> params_input = {CType(1), CType(2), CType(3),
                                           CType(4), CType(5), CType(6)};
  TestParams ok_params[kGatherOKCases] = {
      // Indices are always of rank>1, and output rank is
      // rank(params) + rank(indices) - 1.
      // TODO(laigd): do we support 0-rank ITensor as indices?
      TestParams{{1, 2, 3}, {1}, {0}, 3, {1, 2, 1, 1}, {1, 4}},
      TestParams{{1, 2, 3}, {1}, {1}, 3, {1, 2, 1, 1}, {2, 5}},
      TestParams{{1, 2, 3}, {1}, {2}, -1, {1, 2, 1, 1}, {3, 6}},
      TestParams{
          {1, 2, 3}, {3}, {2, 0, 1}, 3, {1, 2, 1, 3}, {3, 1, 2, 6, 4, 5}},
      TestParams{{3, 2},
                 {2, 2},
                 {0, 0, 1, 0},
                 2,
                 {3, 1, 2, 2},
                 {1, 1, 2, 1, 3, 3, 4, 3, 5, 5, 6, 5}},
  };

  // Ok.
  for (int i = 0; i < kGatherOKCases; i++) {
    test->Reset();
    test->AddTestTensor("params", ok_params[i].params_dims, 1,
                        TfDataTypeToTrt(dtype));
    test->AddTestTensor("indices", ok_params[i].indices_dims, 1,
                        nvinfer1::DataType::kINT32);
    test->AddTestWeights<int32>("axis", {1}, {ok_params[i].axis});
    test->RunValidationAndConversion(node_def);
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(test->GetTensorOrWeights("my_gather", &output));
    EXPECT_TRUE(output.is_tensor());
    ExpectTrtDimsEqualsArray(ok_params[i].expected_output_dims,
                             output.tensor()->getDimensions());

    // Create input in CType and convert expected output to CType.
    std::vector<CType> converted_expected_output(
        ok_params[i].expected_output.begin(),
        ok_params[i].expected_output.end());

    const DataVec input_data{
        {"params", test::AsTensor<CType>(params_input)},
        {"indices", test::AsTensor<int32>(ok_params[i].indices)}};
    DataVec output_data{
        {"my_gather",
         ConstructTensor<CType>(ok_params[i].expected_output.size())}};
    test->BuildAndRun(input_data, &output_data);
    EXPECT_THAT(GetSpanForData<CType>(output_data[0]),
                ElementsAreArray(converted_expected_output));
  }
}

TEST_F(OpConverterTest, ConvertGather) {
  {
    // Input list is empty, should fail.
    NodeDef node_def = MakeNodeDef("my_gather", "GatherV2", {});
    RunValidationAndConversion(
        node_def, error::INVALID_ARGUMENT,
        "GatherV2 got 0 inputs but expected 3, at my_gather");
  }

  // Get the NodeDef for GatherV2.
  Scope s = Scope::NewRootScope();
  auto params = ops::Placeholder(s.WithOpName("params"), DT_FLOAT);
  auto indices = ops::Placeholder(s.WithOpName("indices"), DT_INT32);
  auto axis = ops::Placeholder(s.WithOpName("axis"), DT_INT32);
  auto gather = ops::GatherV2(s.WithOpName("my_gather"), params, indices, axis);
  const NodeDef& node_def = gather.operation.node()->def();
  {
    // Axis is a tensor, should fail.
    Reset();
    AddTestTensor("params", {1, 2, 3});
    AddTestTensor("indices", {2});
    AddTestTensor("axis", {1});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "The input \"axis\" for GatherV2 must be a constant, at my_gather");
  }
  {
    // Axis is out of bounds, should fail.
    Reset();
    AddTestTensor("params", {1, 2, 3});
    AddTestTensor("indices", {2});
    AddTestWeights<int32>("axis", {1}, {4});
    RunValidationAndConversion(node_def, error::INVALID_ARGUMENT,
                               "Axis value of 4 is out of bounds, must be in "
                               "range [-4, 4), at my_gather");
  }
  {
    // Axis is batch dimension, should fail.
    Reset();
    AddTestTensor("params", {1, 2, 3});
    AddTestTensor("indices", {2});
    AddTestWeights<int32>("axis", {1}, {0});
    RunValidationAndConversion(node_def, error::UNIMPLEMENTED,
                               "TensorRT does not allow manipulation of the "
                               "batch dimension, at my_gather");
  }

  Reset();
  TestConvertGather<DT_FLOAT>(this);
  TestConvertGather<DT_HALF>(this);
  TestConvertGather<DT_INT32>(this);
}

TEST_F(OpConverterTest, ConvertUnary) {
  {
    // Input list is empty, should fail.
    NodeDef node_def = MakeNodeDef("my_unary", "Neg", {});
    RunValidationAndConversion(node_def, error::INVALID_ARGUMENT,
                               "Neg got 0 inputs but expected 1, at my_unary");
  }
  {
    // Input is weights, should fail.
    Reset();
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    auto neg = ops::Neg(s.WithOpName("my_unary"), input);
    const NodeDef& node_def = neg.operation.node()->def();
    AddTestWeights<float>("input", {1, 2, 3}, {-3, -2, -1, 0, 1, 2});
    RunValidationAndConversion(
        node_def, error::UNIMPLEMENTED,
        "The input \"x\" for Neg must be a tensor, at my_unary");
  }

  // Get nodedef for unary layer.
  auto get_unary_nodedef = [](string op_name) -> NodeDef {
    Scope s = Scope::NewRootScope();
    auto input = ops::Placeholder(s.WithOpName("input"), DT_FLOAT);
    if (op_name == "Abs") {
      auto unary = ops::Abs(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Acos") {
      auto unary = ops::Acos(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Acosh") {
      auto unary = ops::Acosh(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Asin") {
      auto unary = ops::Asin(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Asinh") {
      auto unary = ops::Asinh(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Atan") {
      auto unary = ops::Atan(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Atanh") {
      auto unary = ops::Atanh(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Ceil") {
      auto unary = ops::Ceil(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Cos") {
      auto unary = ops::Cos(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Cosh") {
      auto unary = ops::Cosh(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Exp") {
      auto unary = ops::Exp(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Floor") {
      auto unary = ops::Floor(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Log") {
      auto unary = ops::Log(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Neg") {
      auto unary = ops::Neg(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Reciprocal") {
      auto unary = ops::Reciprocal(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Rsqrt") {
      auto unary = ops::Rsqrt(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Sin") {
      auto unary = ops::Sin(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Sinh") {
      auto unary = ops::Sinh(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Sqrt") {
      auto unary = ops::Sqrt(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    } else if (op_name == "Tan") {
      auto unary = ops::Tan(s.WithOpName("my_unary"), input);
      return unary.operation.node()->def();
    }
    EXPECT_TRUE(false);
    return NodeDef();
  };
  // Get expected output for unary layer.
  auto get_unary_output = [](string op_name, float input) -> float {
    if (op_name == "Abs") {
      return std::abs(input);
    } else if (op_name == "Acos") {
      return std::acos(input);
    } else if (op_name == "Acosh") {
      return std::acosh(input);
    } else if (op_name == "Asin") {
      return std::asin(input);
    } else if (op_name == "Asinh") {
      return std::asinh(input);
    } else if (op_name == "Atan") {
      return std::atan(input);
    } else if (op_name == "Atanh") {
      return std::atanh(input);
    } else if (op_name == "Ceil") {
      return std::ceil(input);
    } else if (op_name == "Cos") {
      return std::cos(input);
    } else if (op_name == "Cosh") {
      return std::cosh(input);
    } else if (op_name == "Exp") {
      return std::exp(input);
    } else if (op_name == "Floor") {
      return std::floor(input);
    } else if (op_name == "Log") {
      return std::log(input);
    } else if (op_name == "Neg") {
      return -input;
    } else if (op_name == "Reciprocal") {
      return 1.0 / input;
    } else if (op_name == "Rsqrt") {
      return 1.0 / std::sqrt(input);
    } else if (op_name == "Sin") {
      return std::sin(input);
    } else if (op_name == "Sinh") {
      return std::sinh(input);
    } else if (op_name == "Sqrt") {
      return std::sqrt(input);
    } else if (op_name == "Tan") {
      return std::tan(input);
    }
    EXPECT_TRUE(false);
    return 0;
  };

  // Get list of ops to test.
  std::vector<string> ops_to_test;
  // Add all ops supported by ConvertUnary.
  auto* map = UnaryOperationMap();
  ops_to_test.reserve(map->size());
  for (auto& pair : *map) {
    ops_to_test.push_back(pair.first);
  }
  // Add other unary ops to test.
  ops_to_test.push_back("Rsqrt");
  // Ok.
  for (string op_name : ops_to_test) {
    Reset();
    NodeDef node_def = get_unary_nodedef(op_name);
    AddTestTensor("input", {1, 2, 3});
    RunValidationAndConversion(node_def);
    TRT_TensorOrWeights output;
    TF_EXPECT_OK(GetTensorOrWeights("my_unary", &output));
    EXPECT_TRUE(output.is_tensor());
    ExpectTrtDimsEqualsArray({1, 2, 3}, output.tensor()->getDimensions());

    const std::vector<float> input = {-0.9f, 0.6f, 0.0f, -3.5f, 100.0f, 2.9f};
    const DataVec input_data{{"input", test::AsTensor<float>(input)}};
    DataVec output_data{{"my_unary", ConstructTensor<float>(6)}};
    BuildAndRun(input_data, &output_data);
    for (int i = 0; i < input.size(); ++i) {
      const float expected_output = get_unary_output(op_name, input[i]);
      EXPECT_THAT(GetSpanForData<float>(output_data[0])[i],
                  NanSensitiveFloatNear(expected_output, 0.0001));
    }
  }
}

}  // namespace convert
}  // namespace tensorrt
}  // namespace tensorflow

#endif  // GOOGLE_TENSORRT
#endif  // GOOGLE_CUDA
