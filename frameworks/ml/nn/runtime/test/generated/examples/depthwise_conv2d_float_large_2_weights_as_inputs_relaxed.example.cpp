// clang-format off
// Generated file (from: depthwise_conv2d_float_large_2_weights_as_inputs_relaxed.mod.py). Do not edit
std::vector<MixedTypedExample>& get_examples() {
static std::vector<MixedTypedExample> examples = {
// Begin of an example
{
.operands = {
//Input(s)
{ // See tools/test_generator/include/TestHarness.h:MixedTyped
  // int -> Dimensions map
  .operandDimensions = {{0, {1, 2, 2, 4}}, {1, {1, 2, 2, 4}}, {2, {4}}},
  // int -> FLOAT32 map
  .float32Operands = {{0, {10.0f, 21.0f, 10.0f, 0.0f, 10.0f, 22.0f, 20.0f, 0.0f, 10.0f, 23.0f, 30.0f, 0.0f, 10.0f, 24.0f, 40.0f, 0.0f}}, {1, {0.25f, 0.0f, 10.0f, 100.0f, 0.25f, 1.0f, 20.0f, 100.0f, 0.25f, 0.0f, 30.0f, 100.0f, 0.25f, 1.0f, 40.0f, 100.0f}}, {2, {6000.0f, 7000.0f, 8000.0f, 9000.0f}}},
  // int -> INT32 map
  .int32Operands = {},
  // int -> QUANT8_ASYMM map
  .quant8AsymmOperands = {},
  // int -> QUANT16_SYMM map
  .quant16SymmOperands = {},
  // int -> FLOAT16 map
  .float16Operands = {},
  // int -> BOOL8 map
  .bool8Operands = {},
  // int -> QUANT8_SYMM_PER_CHANNEL map
  .quant8ChannelOperands = {},
  // int -> QUANT16_ASYMM map
  .quant16AsymmOperands = {},
  // int -> QUANT8_SYMM map
  .quant8SymmOperands = {},
},
//Output(s)
{ // See tools/test_generator/include/TestHarness.h:MixedTyped
  // int -> Dimensions map
  .operandDimensions = {{0, {1, 1, 1, 4}}},
  // int -> FLOAT32 map
  .float32Operands = {{0, {6010.0f, 7046.0f, 11000.0f, 9000.0f}}},
  // int -> INT32 map
  .int32Operands = {},
  // int -> QUANT8_ASYMM map
  .quant8AsymmOperands = {},
  // int -> QUANT16_SYMM map
  .quant16SymmOperands = {},
  // int -> FLOAT16 map
  .float16Operands = {},
  // int -> BOOL8 map
  .bool8Operands = {},
  // int -> QUANT8_SYMM_PER_CHANNEL map
  .quant8ChannelOperands = {},
  // int -> QUANT16_ASYMM map
  .quant16AsymmOperands = {},
  // int -> QUANT8_SYMM map
  .quant8SymmOperands = {},
}
},
}, // End of an example
};
return examples;
};

std::vector<MixedTypedExample>& get_examples_dynamic_output_shape() {
static std::vector<MixedTypedExample> examples_dynamic_output_shape = {
// Begin of an example
{
.operands = {
//Input(s)
{ // See tools/test_generator/include/TestHarness.h:MixedTyped
  // int -> Dimensions map
  .operandDimensions = {{0, {1, 2, 2, 4}}, {1, {1, 2, 2, 4}}, {2, {4}}},
  // int -> FLOAT32 map
  .float32Operands = {{0, {10.0f, 21.0f, 10.0f, 0.0f, 10.0f, 22.0f, 20.0f, 0.0f, 10.0f, 23.0f, 30.0f, 0.0f, 10.0f, 24.0f, 40.0f, 0.0f}}, {1, {0.25f, 0.0f, 10.0f, 100.0f, 0.25f, 1.0f, 20.0f, 100.0f, 0.25f, 0.0f, 30.0f, 100.0f, 0.25f, 1.0f, 40.0f, 100.0f}}, {2, {6000.0f, 7000.0f, 8000.0f, 9000.0f}}},
  // int -> INT32 map
  .int32Operands = {},
  // int -> QUANT8_ASYMM map
  .quant8AsymmOperands = {},
  // int -> QUANT16_SYMM map
  .quant16SymmOperands = {},
  // int -> FLOAT16 map
  .float16Operands = {},
  // int -> BOOL8 map
  .bool8Operands = {},
  // int -> QUANT8_SYMM_PER_CHANNEL map
  .quant8ChannelOperands = {},
  // int -> QUANT16_ASYMM map
  .quant16AsymmOperands = {},
  // int -> QUANT8_SYMM map
  .quant8SymmOperands = {},
},
//Output(s)
{ // See tools/test_generator/include/TestHarness.h:MixedTyped
  // int -> Dimensions map
  .operandDimensions = {{0, {1, 1, 1, 4}}},
  // int -> FLOAT32 map
  .float32Operands = {{0, {6010.0f, 7046.0f, 11000.0f, 9000.0f}}},
  // int -> INT32 map
  .int32Operands = {},
  // int -> QUANT8_ASYMM map
  .quant8AsymmOperands = {},
  // int -> QUANT16_SYMM map
  .quant16SymmOperands = {},
  // int -> FLOAT16 map
  .float16Operands = {},
  // int -> BOOL8 map
  .bool8Operands = {},
  // int -> QUANT8_SYMM_PER_CHANNEL map
  .quant8ChannelOperands = {},
  // int -> QUANT16_ASYMM map
  .quant16AsymmOperands = {},
  // int -> QUANT8_SYMM map
  .quant8SymmOperands = {},
}
},
}, // End of an example
};
return examples_dynamic_output_shape;
};

