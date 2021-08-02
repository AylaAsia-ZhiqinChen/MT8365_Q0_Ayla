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

#include <cstdint>
#include <initializer_list>
#include <vector>

namespace armnn_wrapper {


/// An ActivationDescriptor for the ActivationLayer.
struct ActivationDescriptor
{
    ActivationDescriptor() : m_Function(ActivationFunction::Sigmoid), m_A(0), m_B(0) {}

    /// @brief The activation function to use
    /// (Sigmoid, TanH, Linear, ReLu, BoundedReLu, SoftReLu, LeakyReLu, Abs, Sqrt, Square).
    ActivationFunction m_Function;
    /// Alpha upper bound value used by the activation functions. (BoundedReLu, Linear, TanH).
    float              m_A;
    /// Beta lower bound value used by the activation functions. (BoundedReLu, Linear, TanH).
    float              m_B;
};

/// A SoftmaxDescriptor for the SoftmaxLayer.
struct SoftmaxDescriptor
{
    SoftmaxDescriptor()
    : m_Beta(1.0f)
    , m_Axis(-1)
    {}

    /// Exponentiation value.
    float m_Beta;
    /// Scalar, defaulted to the last index (-1), specifying the dimension the activation will be performed on.
    int m_Axis;
};

/// @brief An OriginsDescriptor for the ConcatLayer.
/// Descriptor to configure the concatenation process. Number of views must be equal to the number of inputs, and
/// their order must match - e.g. first view corresponds to the first input, second view to the second input, etc.
struct OriginsDescriptor
{
    OriginsDescriptor()
    : m_ConcatAxis(0)
    {}

    int m_ConcatAxis;
};

/// A Pooling2dDescriptor for the Pooling2dLayer.
struct Pooling2dDescriptor
{
    Pooling2dDescriptor()
    : m_PoolType(PoolingAlgorithm::Max)
    , m_PadLeft(0)
    , m_PadRight(0)
    , m_PadTop(0)
    , m_PadBottom(0)
    , m_PoolWidth(0)
    , m_PoolHeight(0)
    , m_StrideX(0)
    , m_StrideY(0)
    , m_OutputShapeRounding(OutputShapeRounding::Floor)
    , m_PaddingMethod(PaddingMethod::Exclude)
    , m_DataLayout(DataLayout::NCHW)
    {}

    /// The pooling algorithm to use (Max. Average, L2).
    PoolingAlgorithm    m_PoolType;
    /// Padding left value in the width dimension.
    uint32_t            m_PadLeft;
    /// Padding right value in the width dimension.
    uint32_t            m_PadRight;
    /// Padding top value in the height dimension.
    uint32_t            m_PadTop;
    /// Padding bottom value in the height dimension.
    uint32_t            m_PadBottom;
    /// Pooling width value.
    uint32_t            m_PoolWidth;
    /// Pooling height value.
    uint32_t            m_PoolHeight;
    /// Stride value when proceeding through input for the width dimension.
    uint32_t            m_StrideX;
    /// Stride value when proceeding through input for the height dimension.
    uint32_t            m_StrideY;
    /// The rounding method for the output shape. (Floor, Ceiling).
    OutputShapeRounding m_OutputShapeRounding;
    /// The padding method to be used. (Exclude, IgnoreValue).
    PaddingMethod       m_PaddingMethod;
    /// The data layout to be used (NCHW, NHWC).
    DataLayout   m_DataLayout;
};

/// A FullyConnectedDescriptor for the FullyConnectedLayer.
struct FullyConnectedDescriptor
{
    FullyConnectedDescriptor()
    : m_BiasEnabled(false)
    , m_TransposeWeightMatrix(false)
    {}

    /// Enable/disable bias.
    bool m_BiasEnabled;
    /// Enable/disable transpose weight matrix.
    bool m_TransposeWeightMatrix;
};

/// A Convolution2dDescriptor for the Convolution2dLayer.
struct Convolution2dDescriptor
{
    Convolution2dDescriptor()
    : m_PadLeft(0)
    , m_PadRight(0)
    , m_PadTop(0)
    , m_PadBottom(0)
    , m_StrideX(0)
    , m_StrideY(0)
    , m_DilationX(1)
    , m_DilationY(1)
    , m_BiasEnabled(false)
    , m_DataLayout(DataLayout::NCHW)
    {}

    /// Padding left value in the width dimension.
    uint32_t             m_PadLeft;
    /// Padding right value in the width dimension.
    uint32_t             m_PadRight;
    /// Padding top value in the height dimension.
    uint32_t             m_PadTop;
    /// Padding bottom value in the height dimension.
    uint32_t             m_PadBottom;
    /// Stride value when proceeding through input for the width dimension.
    uint32_t             m_StrideX;
    /// Stride value when proceeding through input for the height dimension.
    uint32_t             m_StrideY;
    /// Dilation along x axis
    uint32_t             m_DilationX;
    /// Dilation along y axis
    uint32_t             m_DilationY;
    /// Enable/disable bias.
    bool                 m_BiasEnabled;
    /// The data layout to be used (NCHW, NHWC).
    DataLayout           m_DataLayout;
};

/// A DepthwiseConvolution2dDescriptor for the DepthwiseConvolution2dLayer.
struct DepthwiseConvolution2dDescriptor
{
    DepthwiseConvolution2dDescriptor()
    :   m_PadLeft(0)
    ,   m_PadRight(0)
    ,   m_PadTop(0)
    ,   m_PadBottom(0)
    ,   m_StrideX(0)
    ,   m_StrideY(0)
    ,   m_DilationX(1)
    ,   m_DilationY(1)
    ,   m_BiasEnabled(false)
    ,   m_DataLayout(DataLayout::NCHW)
    {}

    /// Padding left value in the width dimension.
    uint32_t   m_PadLeft;
    /// Padding right value in the width dimension.
    uint32_t   m_PadRight;
    /// Padding top value in the height dimension.
    uint32_t   m_PadTop;
    /// Padding bottom value in the height dimension.
    uint32_t   m_PadBottom;
    /// Stride value when proceeding through input for the width dimension.
    uint32_t   m_StrideX;
    /// Stride value when proceeding through input for the height dimension.
    uint32_t   m_StrideY;
    /// Dilation factor value for width dimension.
    uint32_t   m_DilationX;
    /// Dilation factor value for height dimension.
    uint32_t   m_DilationY;
    /// Enable/disable bias.
    bool       m_BiasEnabled;
    /// The data layout to be used (NCHW, NHWC).
    DataLayout m_DataLayout;
};

struct DetectionPostProcessDescriptor
{
    DetectionPostProcessDescriptor()
    : m_MaxDetections(0)
    , m_MaxClassesPerDetection(1)
    , m_DetectionsPerClass(1)
    , m_NmsScoreThreshold(0)
    , m_NmsIouThreshold(0)
    , m_NumClasses(0)
    , m_UseRegularNms(false)
    , m_ScaleX(0)
    , m_ScaleY(0)
    , m_ScaleW(0)
    , m_ScaleH(0)
    {}

    /// Maximum numbers of detections.
    uint32_t m_MaxDetections;
    /// Maximum numbers of classes per detection, used in Fast NMS.
    uint32_t m_MaxClassesPerDetection;
    /// Detections per classes, used in Regular NMS.
    uint32_t m_DetectionsPerClass;
    /// NMS score threshold.
    float m_NmsScoreThreshold;
    /// Intersection over union threshold.
    float m_NmsIouThreshold;
    /// Number of classes.
    uint32_t m_NumClasses;
    /// Use Regular NMS.
    bool m_UseRegularNms;
    /// Center size encoding scale x.
    float m_ScaleX;
    /// Center size encoding scale y.
    float m_ScaleY;
    /// Center size encoding scale weight.
    float m_ScaleW;
    /// Center size encoding scale height.
    float m_ScaleH;
};

/// A NormalizationDescriptor for the NormalizationLayer.
struct NormalizationDescriptor
{
    NormalizationDescriptor()
    : m_NormChannelType(NormalizationAlgorithmChannel::Across)
    , m_NormMethodType(NormalizationAlgorithmMethod::LocalBrightness)
    , m_NormSize(0)
    , m_Alpha(0.f)
    , m_Beta(0.f)
    , m_K(0.f)
    , m_DataLayout(DataLayout::NCHW)
    {}

    /// Normalization channel algorithm to use (Across, Within).
    NormalizationAlgorithmChannel m_NormChannelType;
    /// Normalization method algorithm to use (LocalBrightness, LocalContrast).
    NormalizationAlgorithmMethod  m_NormMethodType;
    /// Depth radius value.
    uint32_t                      m_NormSize;
    /// Alpha value for the normalization equation.
    float                         m_Alpha;
    /// Beta value for the normalization equation.
    float                         m_Beta;
    /// Kappa value used for the across channel normalization equation.
    float                         m_K;
    /// The data layout to be used (NCHW, NHWC).
    DataLayout                    m_DataLayout;
};

/// A L2NormalizationDescriptor for the L2NormalizationLayer.
struct L2NormalizationDescriptor
{
    L2NormalizationDescriptor()
    : m_Eps(1e-12f)
    , m_DataLayout(DataLayout::NCHW)
    {}

    /// Used to avoid dividing by zero.
    float m_Eps;
    /// The data layout to be used (NCHW, NHWC).
    DataLayout m_DataLayout;
};

/// A BatchNormalizationDescriptor for the BatchNormalizationLayer.
struct BatchNormalizationDescriptor
{
    BatchNormalizationDescriptor()
    : m_Eps(0.0001f)
    , m_DataLayout(DataLayout::NCHW)
    {}

    /// Value to add to the variance. Used to avoid dividing by zero.
    float m_Eps;
    /// The data layout to be used (NCHW, NHWC).
    DataLayout m_DataLayout;
};

/// A BatchToSpaceNdDescriptor for the BatchToSpaceNdLayer.
struct BatchToSpaceNdDescriptor
{
    BatchToSpaceNdDescriptor()
        : m_BlockShape({1, 1})
        , m_Crops({{0, 0}, {0, 0}})
        , m_DataLayout(DataLayout::NCHW)
    {}

    BatchToSpaceNdDescriptor(std::vector<unsigned int> blockShape,
                             std::vector<std::pair<unsigned int, unsigned int>> crops)
        : m_BlockShape(blockShape)
        , m_Crops(crops)
        , m_DataLayout(DataLayout::NCHW)
    {}

    /// Block shape values.
    std::vector<unsigned int> m_BlockShape;
    /// The values to crop from the input dimension.
    std::vector<std::pair<unsigned int, unsigned int>> m_Crops;
    /// The data layout to be used (NCHW, NHWC).
    DataLayout m_DataLayout;
};

/// A FakeQuantizationDescriptor for the FakeQuantizationLayer.
struct FakeQuantizationDescriptor
{
    FakeQuantizationDescriptor()
    : m_Min(-6.0f)
    , m_Max(6.0f)
    {}

    /// Minimum value.
    float m_Min;
    /// Maximum value.
    float m_Max;
};

/// A ResizeBilinearDescriptor for the ResizeBilinearLayer.
struct ResizeBilinearDescriptor
{
    ResizeBilinearDescriptor()
    : m_TargetWidth(0)
    , m_TargetHeight(0)
    , m_DataLayout(DataLayout::NCHW)
    {}

    /// Target width value.
    uint32_t          m_TargetWidth;
    /// Target height value.
    uint32_t          m_TargetHeight;
    /// The data layout to be used (NCHW, NHWC).
    DataLayout m_DataLayout;
};

/// A ResizeDescriptor for the ResizeLayer.
struct ResizeDescriptor
{
    ResizeDescriptor()
            : m_TargetWidth(0)
            , m_TargetHeight(0)
            , m_Method(ResizeMethod::NearestNeighbor)
            , m_DataLayout(DataLayout::NCHW)
    {}

    /// Target width value.
    uint32_t m_TargetWidth;
    /// Target height value.
    uint32_t m_TargetHeight;
    /// The Interpolation method to use
    /// (Bilinear, NearestNeighbor).
    ResizeMethod m_Method;
    /// The data layout to be used (NCHW, NHWC).
    DataLayout m_DataLayout;
};

/// A ReshapeDescriptor for the ReshapeLayer.
struct ReshapeDescriptor
{
    ReshapeDescriptor()
    : m_TargetShape()
    {}

    ReshapeDescriptor(std::vector<uint32_t>& inputShape)
    : m_TargetShape(inputShape)
    {}

    /// Target shape value.
    std::vector<uint32_t> m_TargetShape;
};

/// A SpaceToBatchNdDescriptor for the SpaceToBatchNdLayer.
struct SpaceToBatchNdDescriptor
{
    SpaceToBatchNdDescriptor()
    : m_BlockShape({1, 1})
    , m_PadList({{0, 0}, {0, 0}})
    , m_DataLayout(DataLayout::NCHW)
    {}

    SpaceToBatchNdDescriptor(const std::vector<unsigned int>& blockShape,
                             const std::vector<std::pair<unsigned int, unsigned int>>& padList)
    : m_BlockShape(blockShape)
    , m_PadList(padList)
    , m_DataLayout(DataLayout::NCHW)
    {}

    /// Block shape value.
    std::vector<unsigned int> m_BlockShape;
    /// @brief Specifies the padding values for the input dimension:
    /// heightPad{top, bottom} widthPad{left, right}.
    std::vector<std::pair<unsigned int, unsigned int>> m_PadList;
    /// The data layout to be used (NCHW, NHWC).
    DataLayout m_DataLayout;
};

/// A SpaceToDepthDescriptor for the SpaceToDepthLayer
struct SpaceToDepthDescriptor
{
    SpaceToDepthDescriptor()
    : m_BlockSize(1u)
    , m_DataLayout(DataLayout::NHWC)
    {}

    /// Scalar specifying the input block size. It must be >= 1
    unsigned int m_BlockSize;
    /// The data layout to be used (NCHW, NHWC).
    DataLayout m_DataLayout;
};

/// An LstmDescriptor for the LstmLayer.
struct LstmDescriptor
{
    LstmDescriptor()
    : m_ActivationFunc(1) // 0: None, 1: Relu, 3: Relu6, 4: Tanh, 6: Sigmoid
    , m_ClippingThresCell(0.0)
    , m_ClippingThresProj(0.0)
    {}

    /// @brief The activation function to use.
    /// 0: None, 1: Relu, 3: Relu6, 4: Tanh, 6: Sigmoid.
    uint32_t m_ActivationFunc;
    /// Clipping threshold value for the cell state.
    float m_ClippingThresCell;
    /// Clipping threshold value for the projection.
    float m_ClippingThresProj;
};

struct LstmInputParams
{
    LstmInputParams()
        : m_InputToInputWeights(nullptr)
        , m_InputToForgetWeights(nullptr)
        , m_InputToCellWeights(nullptr)
        , m_InputToOutputWeights(nullptr)
        , m_RecurrentToInputWeights(nullptr)
        , m_RecurrentToForgetWeights(nullptr)
        , m_RecurrentToCellWeights(nullptr)
        , m_RecurrentToOutputWeights(nullptr)
        , m_CellToInputWeights(nullptr)
        , m_CellToForgetWeights(nullptr)
        , m_CellToOutputWeights(nullptr)
        , m_InputGateBias(nullptr)
        , m_ForgetGateBias(nullptr)
        , m_CellBias(nullptr)
        , m_OutputGateBias(nullptr)
        , m_ProjectionWeights(nullptr)
        , m_ProjectionBias(nullptr)
        , m_InputLayerNormWeights(nullptr)
        , m_ForgetLayerNormWeights(nullptr)
        , m_CellLayerNormWeights(nullptr)
        , m_OutputLayerNormWeights(nullptr)
    {
    }

    TensorInfo* m_InputToInputWeights;
    TensorInfo* m_InputToForgetWeights;
    TensorInfo* m_InputToCellWeights;
    TensorInfo* m_InputToOutputWeights;
    TensorInfo* m_RecurrentToInputWeights;
    TensorInfo* m_RecurrentToForgetWeights;
    TensorInfo* m_RecurrentToCellWeights;
    TensorInfo* m_RecurrentToOutputWeights;
    TensorInfo* m_CellToInputWeights;
    TensorInfo* m_CellToForgetWeights;
    TensorInfo* m_CellToOutputWeights;
    TensorInfo* m_InputGateBias;
    TensorInfo* m_ForgetGateBias;
    TensorInfo* m_CellBias;
    TensorInfo* m_OutputGateBias;
    TensorInfo* m_ProjectionWeights;
    TensorInfo* m_ProjectionBias;
    TensorInfo* m_InputLayerNormWeights;
    TensorInfo* m_ForgetLayerNormWeights;
    TensorInfo* m_CellLayerNormWeights;
    TensorInfo* m_OutputLayerNormWeights;
};

struct QuantizedLstmInputParams
{
    QuantizedLstmInputParams()
        : m_InputToInputWeights(nullptr)
        , m_InputToForgetWeights(nullptr)
        , m_InputToCellWeights(nullptr)
        , m_InputToOutputWeights(nullptr)

        , m_RecurrentToInputWeights(nullptr)
        , m_RecurrentToForgetWeights(nullptr)
        , m_RecurrentToCellWeights(nullptr)
        , m_RecurrentToOutputWeights(nullptr)

        , m_InputGateBias(nullptr)
        , m_ForgetGateBias(nullptr)
        , m_CellBias(nullptr)
        , m_OutputGateBias(nullptr)
    {
    }

    TensorInfo* m_InputToInputWeights;
    TensorInfo* m_InputToForgetWeights;
    TensorInfo* m_InputToCellWeights;
    TensorInfo* m_InputToOutputWeights;

    TensorInfo* m_RecurrentToInputWeights;
    TensorInfo* m_RecurrentToForgetWeights;
    TensorInfo* m_RecurrentToCellWeights;
    TensorInfo* m_RecurrentToOutputWeights;

    TensorInfo* m_InputGateBias;
    TensorInfo* m_ForgetGateBias;
    TensorInfo* m_CellBias;
    TensorInfo* m_OutputGateBias;
};

/// A MeanDescriptor for the MeanLayer.
struct MeanDescriptor
{
    MeanDescriptor()
    : m_Axis()
    , m_KeepDims(false)
    {}

    MeanDescriptor(const std::vector<unsigned int>& axis, bool keepDims)
    : m_Axis(axis)
    , m_KeepDims(keepDims)
    {}

    /// Values for the dimensions to reduce.
    std::vector<unsigned int> m_Axis;
    /// Enable/disable keep dimensions. If true, then the reduced dimensions that are of length 1 are kept.
    bool m_KeepDims;
};

/// A PadDescriptor for the PadLayer.
struct PadDescriptor
{
    PadDescriptor() : m_PadValue(0)
    {}

    PadDescriptor(const std::vector<std::pair<unsigned int, unsigned int>>& padList, const float& padValue = 0)
    : m_PadList(padList), m_PadValue(padValue)
    {}

    /// @brief Specifies the padding for input dimension.
    /// First is the number of values to add before the tensor in the dimension.
    /// Second is the number of values to add after the tensor in the dimension.
    /// The number of pairs should match the number of dimensions in the input tensor.
    std::vector<std::pair<unsigned int, unsigned int>> m_PadList;

    /// Optional value to use for padding, defaults to 0
    float m_PadValue;
};

/// A StackDescriptor for the StackLayer.
struct StackDescriptor
{
    StackDescriptor()
    : m_Axis(0)
    , m_NumInputs(0)
    , m_InputShape()
    {}

    StackDescriptor(uint32_t axis, uint32_t numInputs, std::vector<uint32_t>& inputShape)
    : m_Axis(axis)
    , m_NumInputs(numInputs)
    , m_InputShape(inputShape)
    {}

    /// 0-based axis along which to stack the input tensors.
    uint32_t m_Axis;
    /// Number of input tensors.
    uint32_t m_NumInputs;
    /// Required shape of all input tensors.
    std::vector<uint32_t> m_InputShape;
};

/// A StridedSliceDescriptor for the StridedSliceLayer.
struct StridedSliceDescriptor
{
    StridedSliceDescriptor(const std::vector<int>& begin,
                           const std::vector<int>& end,
                           const std::vector<int>& stride)
    : m_Begin(begin)
    , m_End(end)
    , m_Stride(stride)
    , m_BeginMask(0)
    , m_EndMask(0)
    , m_ShrinkAxisMask(0)
    , m_EllipsisMask(0)
    , m_NewAxisMask(0)
    , m_DataLayout(DataLayout::NCHW)
    {}

    StridedSliceDescriptor()
    : StridedSliceDescriptor({}, {}, {})
    {}

    // int GetStartForAxis(std::vector<uint32_t> inputShape, unsigned int axis) const;
    // int GetStopForAxis(std::vector<uint32_t> inputShape,
    //                    unsigned int axis,
    //                    int startForAxis) const;

    /// Begin values for the input that will be sliced.
    std::vector<int> m_Begin;
    /// End values for the input that will be sliced.
    std::vector<int> m_End;
    /// Stride values for the input that will be sliced.
    std::vector<int> m_Stride;

    /// @brief Begin mask value. If set, then the begin is disregarded and the fullest
    /// range is used for the dimension.
    int32_t m_BeginMask;
    /// @brief End mask value. If set, then the end is disregarded and the fullest range
    /// is used for the dimension.
    int32_t m_EndMask;
    /// Shrink axis mask value. If set, the nth specification shrinks the dimensionality by 1.
    int32_t m_ShrinkAxisMask;
    /// Ellipsis mask value.
    int32_t m_EllipsisMask;
    /// @brief New axis mask value. If set, the begin, end and stride is disregarded and
    /// a new 1 dimension is inserted to this location of the output tensor.
    int32_t m_NewAxisMask;

    /// The data layout to be used (NCHW, NHWC).
    DataLayout m_DataLayout;
};

/// A PreCompiledDescriptor for the PreCompiledLayer.
struct PreCompiledDescriptor
{
    PreCompiledDescriptor(unsigned int numInputSlots = 1u, unsigned int numOutputSlots = 1u)
        : m_NumInputSlots(numInputSlots), m_NumOutputSlots(numOutputSlots)
    {}

    ~PreCompiledDescriptor() = default;

    unsigned int m_NumInputSlots;
    unsigned int m_NumOutputSlots;
};


/// A TransposeConvolution2dDescriptor for the TransposeConvolution2dLayer.
struct TransposeConvolution2dDescriptor
{
    TransposeConvolution2dDescriptor() :
        m_PadLeft(0),
        m_PadRight(0),
        m_PadTop(0),
        m_PadBottom(0),
        m_StrideX(0),
        m_StrideY(0),
        m_BiasEnabled(false),
        m_DataLayout(DataLayout::NCHW)
    {}

    /// Padding left value in the width dimension.
    uint32_t   m_PadLeft;
    /// Padding right value in the width dimension.
    uint32_t   m_PadRight;
    /// Padding top value in the height dimension.
    uint32_t   m_PadTop;
    /// Padding bottom value in the height dimension.
    uint32_t   m_PadBottom;
    /// Stride value when proceeding through input for the width dimension.
    uint32_t   m_StrideX;
    /// Stride value when proceeding through input for the height dimension.
    uint32_t   m_StrideY;
    /// Enable/disable bias.
    bool       m_BiasEnabled;
    /// The data layout to be used (NCHW, NHWC).
    DataLayout m_DataLayout;
};

}  // namespace armnn_wrapper
