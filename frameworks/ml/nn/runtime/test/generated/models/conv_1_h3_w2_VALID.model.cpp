// clang-format off
// Generated file (from: conv_1_h3_w2_VALID.mod.py). Do not edit
void CreateModel(Model *model) {
  OperandType type0(Type::INT32, {});
  OperandType type1(Type::TENSOR_FLOAT32, {1, 8, 8, 3});
  OperandType type2(Type::TENSOR_FLOAT32, {1, 6, 7, 1});
  OperandType type3(Type::TENSOR_FLOAT32, {1, 3, 2, 3});
  OperandType type4(Type::TENSOR_FLOAT32, {1});
  // Phase 1, operands
  auto op2 = model->addOperand(&type1);
  auto op0 = model->addOperand(&type3);
  auto op1 = model->addOperand(&type4);
  auto b4 = model->addOperand(&type0);
  auto b5 = model->addOperand(&type0);
  auto b6 = model->addOperand(&type0);
  auto b7 = model->addOperand(&type0);
  auto op3 = model->addOperand(&type2);
  // Phase 2, operations
  static float op0_init[] = {-0.966213f, -0.467474f, -0.82203f, -0.579455f, 0.0278809f, -0.79946f, -0.684259f, 0.563238f, 0.37289f, 0.738216f, 0.386045f, -0.917775f, 0.184325f, -0.270568f, 0.82236f, 0.0973683f, -0.941308f, -0.144706f};
  model->setOperandValue(op0, op0_init, sizeof(float) * 18);
  static float op1_init[] = {0.0f};
  model->setOperandValue(op1, op1_init, sizeof(float) * 1);
  static int32_t b4_init[] = {2};
  model->setOperandValue(b4, b4_init, sizeof(int32_t) * 1);
  static int32_t b5_init[] = {1};
  model->setOperandValue(b5, b5_init, sizeof(int32_t) * 1);
  static int32_t b6_init[] = {1};
  model->setOperandValue(b6, b6_init, sizeof(int32_t) * 1);
  static int32_t b7_init[] = {0};
  model->setOperandValue(b7, b7_init, sizeof(int32_t) * 1);
  model->addOperation(ANEURALNETWORKS_CONV_2D, {op2, op0, op1, b4, b5, b6, b7}, {op3});
  // Phase 3, inputs and outputs
  model->identifyInputsAndOutputs(
    {op2},
    {op3});
  assert(model->isValid());
}

inline bool is_ignored(int i) {
  static std::set<int> ignore = {};
  return ignore.find(i) != ignore.end();
}

void CreateModel_dynamic_output_shape(Model *model) {
  OperandType type0(Type::INT32, {});
  OperandType type1(Type::TENSOR_FLOAT32, {1, 8, 8, 3});
  OperandType type3(Type::TENSOR_FLOAT32, {1, 3, 2, 3});
  OperandType type4(Type::TENSOR_FLOAT32, {1});
  OperandType type5(Type::TENSOR_FLOAT32, {0, 0, 0, 0});
  // Phase 1, operands
  auto op2 = model->addOperand(&type1);
  auto op0 = model->addOperand(&type3);
  auto op1 = model->addOperand(&type4);
  auto b4 = model->addOperand(&type0);
  auto b5 = model->addOperand(&type0);
  auto b6 = model->addOperand(&type0);
  auto b7 = model->addOperand(&type0);
  auto op3 = model->addOperand(&type5);
  // Phase 2, operations
  static float op0_init[] = {-0.966213f, -0.467474f, -0.82203f, -0.579455f, 0.0278809f, -0.79946f, -0.684259f, 0.563238f, 0.37289f, 0.738216f, 0.386045f, -0.917775f, 0.184325f, -0.270568f, 0.82236f, 0.0973683f, -0.941308f, -0.144706f};
  model->setOperandValue(op0, op0_init, sizeof(float) * 18);
  static float op1_init[] = {0.0f};
  model->setOperandValue(op1, op1_init, sizeof(float) * 1);
  static int32_t b4_init[] = {2};
  model->setOperandValue(b4, b4_init, sizeof(int32_t) * 1);
  static int32_t b5_init[] = {1};
  model->setOperandValue(b5, b5_init, sizeof(int32_t) * 1);
  static int32_t b6_init[] = {1};
  model->setOperandValue(b6, b6_init, sizeof(int32_t) * 1);
  static int32_t b7_init[] = {0};
  model->setOperandValue(b7, b7_init, sizeof(int32_t) * 1);
  model->addOperation(ANEURALNETWORKS_CONV_2D, {op2, op0, op1, b4, b5, b6, b7}, {op3});
  // Phase 3, inputs and outputs
  model->identifyInputsAndOutputs(
    {op2},
    {op3});
  assert(model->isValid());
}

inline bool is_ignored_dynamic_output_shape(int i) {
  static std::set<int> ignore = {};
  return ignore.find(i) != ignore.end();
}

void CreateModel_2(Model *model) {
  OperandType type0(Type::INT32, {});
  OperandType type1(Type::TENSOR_FLOAT32, {1, 8, 8, 3});
  OperandType type2(Type::TENSOR_FLOAT32, {1, 6, 7, 1});
  OperandType type3(Type::TENSOR_FLOAT32, {1, 3, 2, 3});
  OperandType type4(Type::TENSOR_FLOAT32, {1});
  // Phase 1, operands
  auto op2 = model->addOperand(&type1);
  auto op0 = model->addOperand(&type3);
  auto op1 = model->addOperand(&type4);
  auto b4 = model->addOperand(&type0);
  auto b5 = model->addOperand(&type0);
  auto b6 = model->addOperand(&type0);
  auto b7 = model->addOperand(&type0);
  auto op3 = model->addOperand(&type2);
  // Phase 2, operations
  static float op0_init[] = {-0.966213f, -0.467474f, -0.82203f, -0.579455f, 0.0278809f, -0.79946f, -0.684259f, 0.563238f, 0.37289f, 0.738216f, 0.386045f, -0.917775f, 0.184325f, -0.270568f, 0.82236f, 0.0973683f, -0.941308f, -0.144706f};
  model->setOperandValue(op0, op0_init, sizeof(float) * 18);
  static float op1_init[] = {0.0f};
  model->setOperandValue(op1, op1_init, sizeof(float) * 1);
  static int32_t b4_init[] = {2};
  model->setOperandValue(b4, b4_init, sizeof(int32_t) * 1);
  static int32_t b5_init[] = {1};
  model->setOperandValue(b5, b5_init, sizeof(int32_t) * 1);
  static int32_t b6_init[] = {1};
  model->setOperandValue(b6, b6_init, sizeof(int32_t) * 1);
  static int32_t b7_init[] = {0};
  model->setOperandValue(b7, b7_init, sizeof(int32_t) * 1);
  model->addOperation(ANEURALNETWORKS_CONV_2D, {op2, op0, op1, b4, b5, b6, b7}, {op3});
  // Phase 3, inputs and outputs
  model->identifyInputsAndOutputs(
    {op2},
    {op3});
  assert(model->isValid());
}

inline bool is_ignored_2(int i) {
  static std::set<int> ignore = {};
  return ignore.find(i) != ignore.end();
}

void CreateModel_dynamic_output_shape_2(Model *model) {
  OperandType type0(Type::INT32, {});
  OperandType type1(Type::TENSOR_FLOAT32, {1, 8, 8, 3});
  OperandType type3(Type::TENSOR_FLOAT32, {1, 3, 2, 3});
  OperandType type4(Type::TENSOR_FLOAT32, {1});
  OperandType type5(Type::TENSOR_FLOAT32, {0, 0, 0, 0});
  // Phase 1, operands
  auto op2 = model->addOperand(&type1);
  auto op0 = model->addOperand(&type3);
  auto op1 = model->addOperand(&type4);
  auto b4 = model->addOperand(&type0);
  auto b5 = model->addOperand(&type0);
  auto b6 = model->addOperand(&type0);
  auto b7 = model->addOperand(&type0);
  auto op3 = model->addOperand(&type5);
  // Phase 2, operations
  static float op0_init[] = {-0.966213f, -0.467474f, -0.82203f, -0.579455f, 0.0278809f, -0.79946f, -0.684259f, 0.563238f, 0.37289f, 0.738216f, 0.386045f, -0.917775f, 0.184325f, -0.270568f, 0.82236f, 0.0973683f, -0.941308f, -0.144706f};
  model->setOperandValue(op0, op0_init, sizeof(float) * 18);
  static float op1_init[] = {0.0f};
  model->setOperandValue(op1, op1_init, sizeof(float) * 1);
  static int32_t b4_init[] = {2};
  model->setOperandValue(b4, b4_init, sizeof(int32_t) * 1);
  static int32_t b5_init[] = {1};
  model->setOperandValue(b5, b5_init, sizeof(int32_t) * 1);
  static int32_t b6_init[] = {1};
  model->setOperandValue(b6, b6_init, sizeof(int32_t) * 1);
  static int32_t b7_init[] = {0};
  model->setOperandValue(b7, b7_init, sizeof(int32_t) * 1);
  model->addOperation(ANEURALNETWORKS_CONV_2D, {op2, op0, op1, b4, b5, b6, b7}, {op3});
  // Phase 3, inputs and outputs
  model->identifyInputsAndOutputs(
    {op2},
    {op3});
  assert(model->isValid());
}

inline bool is_ignored_dynamic_output_shape_2(int i) {
  static std::set<int> ignore = {};
  return ignore.find(i) != ignore.end();
}

