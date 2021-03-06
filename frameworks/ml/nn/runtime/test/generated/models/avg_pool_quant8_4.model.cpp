// clang-format off
// Generated file (from: avg_pool_quant8_4.mod.py). Do not edit
void CreateModel(Model *model) {
  OperandType type0(Type::TENSOR_QUANT8_ASYMM, {1, 3, 3, 1}, 0.5f, 0);
  OperandType type1(Type::INT32, {});
  // Phase 1, operands
  auto op1 = model->addOperand(&type0);
  auto pad0 = model->addOperand(&type1);
  auto cons1 = model->addOperand(&type1);
  auto relu1_activitation = model->addOperand(&type1);
  auto op3 = model->addOperand(&type0);
  // Phase 2, operations
  static int32_t pad0_init[] = {0};
  model->setOperandValue(pad0, pad0_init, sizeof(int32_t) * 1);
  static int32_t cons1_init[] = {1};
  model->setOperandValue(cons1, cons1_init, sizeof(int32_t) * 1);
  static int32_t relu1_activitation_init[] = {2};
  model->setOperandValue(relu1_activitation, relu1_activitation_init, sizeof(int32_t) * 1);
  model->addOperation(ANEURALNETWORKS_AVERAGE_POOL_2D, {op1, pad0, pad0, pad0, pad0, cons1, cons1, cons1, cons1, relu1_activitation}, {op3});
  // Phase 3, inputs and outputs
  model->identifyInputsAndOutputs(
    {op1},
    {op3});
  assert(model->isValid());
}

inline bool is_ignored(int i) {
  static std::set<int> ignore = {};
  return ignore.find(i) != ignore.end();
}

void CreateModel_dynamic_output_shape(Model *model) {
  OperandType type0(Type::TENSOR_QUANT8_ASYMM, {1, 3, 3, 1}, 0.5f, 0);
  OperandType type1(Type::INT32, {});
  OperandType type2(Type::TENSOR_QUANT8_ASYMM, {0, 0, 0, 0}, 0.5f, 0);
  // Phase 1, operands
  auto op1 = model->addOperand(&type0);
  auto pad0 = model->addOperand(&type1);
  auto cons1 = model->addOperand(&type1);
  auto relu1_activitation = model->addOperand(&type1);
  auto op3 = model->addOperand(&type2);
  // Phase 2, operations
  static int32_t pad0_init[] = {0};
  model->setOperandValue(pad0, pad0_init, sizeof(int32_t) * 1);
  static int32_t cons1_init[] = {1};
  model->setOperandValue(cons1, cons1_init, sizeof(int32_t) * 1);
  static int32_t relu1_activitation_init[] = {2};
  model->setOperandValue(relu1_activitation, relu1_activitation_init, sizeof(int32_t) * 1);
  model->addOperation(ANEURALNETWORKS_AVERAGE_POOL_2D, {op1, pad0, pad0, pad0, pad0, cons1, cons1, cons1, cons1, relu1_activitation}, {op3});
  // Phase 3, inputs and outputs
  model->identifyInputsAndOutputs(
    {op1},
    {op3});
  assert(model->isValid());
}

inline bool is_ignored_dynamic_output_shape(int i) {
  static std::set<int> ignore = {};
  return ignore.find(i) != ignore.end();
}

