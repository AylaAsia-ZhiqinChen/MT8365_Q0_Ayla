/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "ArmnnTypes.hpp"
#include "ArmnnDescriptors.hpp"

namespace armnn_wrapper {

class ArmnnWrapper {
public:
    explicit ArmnnWrapper(bool tunerEnabled = false,
                          const char* tunerConfigPath = "/vendor/etc/armnn_app.config",
                          TuningLevel tunerMode = TuningLevel::Rapid);

    ~ArmnnWrapper();

private:
    class ArmnnImpl;
    ArmnnImpl *mImpl;

public:
    /// Verify OP Layers
    bool VerifyActivationLayer(TensorInfo& input, TensorInfo& output, ActivationDescriptor& desc);

    bool VerifyAdditionLayer(TensorInfo& input0, TensorInfo& input1, TensorInfo& output);

    bool VerifyBatchToSpaceNdLayer(TensorInfo& input, TensorInfo& output,
                                   BatchToSpaceNdDescriptor& desc);

    bool VerifyConcatLayer(std::vector<TensorInfo>& inputs, TensorInfo& output,
                           OriginsDescriptor& desc);

    bool VerifyConv2dLayer(TensorInfo& input, TensorInfo& output, TensorInfo& weight,
                           TensorInfo& bias, Convolution2dDescriptor& desc);

    bool VerifyDequantizeLayer(TensorInfo& input, TensorInfo& output);

    bool VerifyDivisionLayer(TensorInfo& input0, TensorInfo& input1, TensorInfo& output);

    bool VerifyDwConv2dLayer(TensorInfo& input, TensorInfo& output, TensorInfo& weight,
                             TensorInfo& bias, DepthwiseConvolution2dDescriptor& desc);

    bool VerifyFullyConnectedLayer(TensorInfo& input, TensorInfo& output, TensorInfo& weight,
                                   TensorInfo& bias);

    bool VerifyLstmLayer(TensorInfo& input, TensorInfo& outputStateIn, TensorInfo& cellStateIn,
                         std::vector<TensorInfo>& outputs, LstmInputParams& params,
                         LstmDescriptor& desc);

    bool VerifyMaximumLayer(TensorInfo& input0, TensorInfo& input1, TensorInfo& output);

    bool VerifyMeanLayer(TensorInfo& input, TensorInfo& output, MeanDescriptor& desc);

    bool VerifyMinimumLayer(TensorInfo& input0, TensorInfo& input1, TensorInfo& output);

    bool VerifyMulLayer(TensorInfo& input0, TensorInfo& input1, TensorInfo& output);

    bool VerifyPadLayer(TensorInfo& input, TensorInfo& output, PadDescriptor& desc);

    bool VerifyPadV2Layer(TensorInfo& input, TensorInfo& output, PadDescriptor& desc);

    bool VerifyPoolingLayer(TensorInfo& input, TensorInfo& output, Pooling2dDescriptor& desc);

    bool VerifyPreluLayer(TensorInfo& input, TensorInfo& output, TensorInfo& alpha);

    bool VerifyQuantizedLstmLayer(TensorInfo& input, TensorInfo& cellStateIn,
                                  TensorInfo& outputStateIn, std::vector<TensorInfo>& outputs,
                                  QuantizedLstmInputParams& params);

    bool VerifyQuantizeLayer(TensorInfo& input, TensorInfo& output);

    bool VerifyReshapeLayer(TensorInfo& input, TensorInfo& output);

    bool VerifyResizeLayer(TensorInfo& input, TensorInfo& output, ResizeDescriptor& desc);

    bool VerifySoftmaxLayer(TensorInfo& input, TensorInfo& output, SoftmaxDescriptor& desc);

    bool VerifySpaceToBatchNdLayer(TensorInfo& input, TensorInfo& output,
                                   SpaceToBatchNdDescriptor& desc);

    bool VerifySpaceToDepthLayer(TensorInfo& input, TensorInfo& output,
                                 SpaceToDepthDescriptor& desc);

    bool VerifyStridedSliceLayer(TensorInfo& input, TensorInfo& output,
                                 StridedSliceDescriptor& desc);

    bool VerifySubLayer(TensorInfo& input0, TensorInfo& input1, TensorInfo& output);

    bool VerifyTransposeConv2dLayer(TensorInfo& input, TensorInfo& output, TensorInfo& weight,
                                    TensorInfo& bias, TransposeConvolution2dDescriptor& desc);

    bool VerifyTransposeLayer(TensorInfo& input, TensorInfo& output, std::vector<int32_t>& perm);

public:
    /// Add OP Layers
    void AddInputLayer(TensorInfo& input, uint32_t& inputId);

    bool AddActivationLayer(TensorInfo& input, TensorInfo& output, ActivationDescriptor& desc);

    bool AddAdditionLayer(TensorInfo& input0, TensorInfo& input1, TensorInfo& output);

    bool AddBatchToSpaceNdLayer(TensorInfo& input, TensorInfo& output,
                                BatchToSpaceNdDescriptor& desc);

    bool AddConcatLayer(std::vector<TensorInfo>& inputs, TensorInfo& output,
                        OriginsDescriptor& desc);

    bool AddConv2dLayer(TensorInfo& input, TensorInfo& output, TensorInfo& weight,
                        TensorInfo& bias, Convolution2dDescriptor& desc);

    bool AddDequantizeLayer(TensorInfo& input, TensorInfo& output);

    bool AddDivisionLayer(TensorInfo& input0, TensorInfo& input1, TensorInfo& output);

    bool AddDwConv2dLayer(TensorInfo& input, TensorInfo& output, TensorInfo& weight,
                          TensorInfo& bias, DepthwiseConvolution2dDescriptor& desc);

    bool AddFullyConnectedLayer(TensorInfo& input, TensorInfo& output, TensorInfo& weight,
                                TensorInfo& bias);

    bool AddLstmLayer(TensorInfo& input, TensorInfo& outputStateIn, TensorInfo& cellStateIn,
                      std::vector<TensorInfo>& outputs, LstmInputParams& params,
                      LstmDescriptor& desc);

    bool AddMaximumLayer(TensorInfo& input0, TensorInfo& input1, TensorInfo& output);

    bool AddMeanLayer(TensorInfo& input, TensorInfo& output, MeanDescriptor& desc);

    bool AddMinimumLayer(TensorInfo& input0, TensorInfo& input1, TensorInfo& output);

    bool AddMulLayer(TensorInfo& input0, TensorInfo& input1, TensorInfo& output);

    bool AddPadLayer(TensorInfo& input, TensorInfo& output, PadDescriptor& desc);

    bool AddPadV2Layer(TensorInfo& input, TensorInfo& output, PadDescriptor& desc);

    bool AddPoolingLayer(TensorInfo& input, TensorInfo& output, Pooling2dDescriptor& desc);

    bool AddPreluLayer(TensorInfo& input, TensorInfo& output, TensorInfo& alpha);

    bool AddQuantizedLstmLayer(TensorInfo& input, TensorInfo& cellStateIn,
                               TensorInfo& outputStateIn, std::vector<TensorInfo>& outputs,
                               QuantizedLstmInputParams& params);

    bool AddQuantizeLayer(TensorInfo& input, TensorInfo& output);

    bool AddReshapeLayer(TensorInfo& input, TensorInfo& output);

    bool AddResizeLayer(TensorInfo& input, TensorInfo& output, ResizeDescriptor& desc);

    bool AddSoftmaxLayer(TensorInfo& input, TensorInfo& output, SoftmaxDescriptor& desc);

    bool AddSpaceToBatchNdLayer(TensorInfo& input, TensorInfo& output,
                                SpaceToBatchNdDescriptor& desc);

    bool AddSpaceToDepthLayer(TensorInfo& input, TensorInfo& output, SpaceToDepthDescriptor& desc);

    bool AddStridedSliceLayer(TensorInfo& input, TensorInfo& output,
                              StridedSliceDescriptor& desc);

    bool AddSubLayer(TensorInfo& input0, TensorInfo& input1, TensorInfo& output);

    bool AddTransposeConv2dLayer(TensorInfo& input, TensorInfo& output, TensorInfo& weight,
                                 TensorInfo& bias, TransposeConvolution2dDescriptor& desc);

    bool AddTransposeLayer(TensorInfo& input, TensorInfo& output, std::vector<int32_t>& perm);

    void AddOutputLayer(TensorInfo& output, uint32_t& outputId);

public:
    /// Control ArmNN Flow
    void Optimize(bool reduceFp32ToFp16 = false);

    bool LoadNetwork();

    void SetInput(uint32_t index, std::vector<float>& input);

    void SetInput(uint32_t index, std::vector<uint8_t>& input);

    void SetOutput(uint32_t index, std::vector<float>& output);

    void SetOutput(uint32_t index, std::vector<uint8_t>& output);

    bool EnqueueWorkload();

    bool UnloadNetwork();
};

}  // namespace armnn_wrapper
