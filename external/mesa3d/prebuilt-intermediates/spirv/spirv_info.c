/* DO NOT EDIT - This file is generated automatically by spirv_info_c.py script */

/*
 * Copyright (C) 2017 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "spirv_info.h"

const char *
spirv_capability_to_string(SpvCapability v)
{
   switch (v) {
   case SpvCapabilityMatrix: return "SpvCapabilityMatrix";
   case SpvCapabilityShader: return "SpvCapabilityShader";
   case SpvCapabilityGeometry: return "SpvCapabilityGeometry";
   case SpvCapabilityTessellation: return "SpvCapabilityTessellation";
   case SpvCapabilityAddresses: return "SpvCapabilityAddresses";
   case SpvCapabilityLinkage: return "SpvCapabilityLinkage";
   case SpvCapabilityKernel: return "SpvCapabilityKernel";
   case SpvCapabilityVector16: return "SpvCapabilityVector16";
   case SpvCapabilityFloat16Buffer: return "SpvCapabilityFloat16Buffer";
   case SpvCapabilityFloat16: return "SpvCapabilityFloat16";
   case SpvCapabilityFloat64: return "SpvCapabilityFloat64";
   case SpvCapabilityInt64: return "SpvCapabilityInt64";
   case SpvCapabilityInt64Atomics: return "SpvCapabilityInt64Atomics";
   case SpvCapabilityImageBasic: return "SpvCapabilityImageBasic";
   case SpvCapabilityImageReadWrite: return "SpvCapabilityImageReadWrite";
   case SpvCapabilityImageMipmap: return "SpvCapabilityImageMipmap";
   case SpvCapabilityPipes: return "SpvCapabilityPipes";
   case SpvCapabilityGroups: return "SpvCapabilityGroups";
   case SpvCapabilityDeviceEnqueue: return "SpvCapabilityDeviceEnqueue";
   case SpvCapabilityLiteralSampler: return "SpvCapabilityLiteralSampler";
   case SpvCapabilityAtomicStorage: return "SpvCapabilityAtomicStorage";
   case SpvCapabilityInt16: return "SpvCapabilityInt16";
   case SpvCapabilityTessellationPointSize: return "SpvCapabilityTessellationPointSize";
   case SpvCapabilityGeometryPointSize: return "SpvCapabilityGeometryPointSize";
   case SpvCapabilityImageGatherExtended: return "SpvCapabilityImageGatherExtended";
   case SpvCapabilityStorageImageMultisample: return "SpvCapabilityStorageImageMultisample";
   case SpvCapabilityUniformBufferArrayDynamicIndexing: return "SpvCapabilityUniformBufferArrayDynamicIndexing";
   case SpvCapabilitySampledImageArrayDynamicIndexing: return "SpvCapabilitySampledImageArrayDynamicIndexing";
   case SpvCapabilityStorageBufferArrayDynamicIndexing: return "SpvCapabilityStorageBufferArrayDynamicIndexing";
   case SpvCapabilityStorageImageArrayDynamicIndexing: return "SpvCapabilityStorageImageArrayDynamicIndexing";
   case SpvCapabilityClipDistance: return "SpvCapabilityClipDistance";
   case SpvCapabilityCullDistance: return "SpvCapabilityCullDistance";
   case SpvCapabilityImageCubeArray: return "SpvCapabilityImageCubeArray";
   case SpvCapabilitySampleRateShading: return "SpvCapabilitySampleRateShading";
   case SpvCapabilityImageRect: return "SpvCapabilityImageRect";
   case SpvCapabilitySampledRect: return "SpvCapabilitySampledRect";
   case SpvCapabilityGenericPointer: return "SpvCapabilityGenericPointer";
   case SpvCapabilityInt8: return "SpvCapabilityInt8";
   case SpvCapabilityInputAttachment: return "SpvCapabilityInputAttachment";
   case SpvCapabilitySparseResidency: return "SpvCapabilitySparseResidency";
   case SpvCapabilityMinLod: return "SpvCapabilityMinLod";
   case SpvCapabilitySampled1D: return "SpvCapabilitySampled1D";
   case SpvCapabilityImage1D: return "SpvCapabilityImage1D";
   case SpvCapabilitySampledCubeArray: return "SpvCapabilitySampledCubeArray";
   case SpvCapabilitySampledBuffer: return "SpvCapabilitySampledBuffer";
   case SpvCapabilityImageBuffer: return "SpvCapabilityImageBuffer";
   case SpvCapabilityImageMSArray: return "SpvCapabilityImageMSArray";
   case SpvCapabilityStorageImageExtendedFormats: return "SpvCapabilityStorageImageExtendedFormats";
   case SpvCapabilityImageQuery: return "SpvCapabilityImageQuery";
   case SpvCapabilityDerivativeControl: return "SpvCapabilityDerivativeControl";
   case SpvCapabilityInterpolationFunction: return "SpvCapabilityInterpolationFunction";
   case SpvCapabilityTransformFeedback: return "SpvCapabilityTransformFeedback";
   case SpvCapabilityGeometryStreams: return "SpvCapabilityGeometryStreams";
   case SpvCapabilityStorageImageReadWithoutFormat: return "SpvCapabilityStorageImageReadWithoutFormat";
   case SpvCapabilityStorageImageWriteWithoutFormat: return "SpvCapabilityStorageImageWriteWithoutFormat";
   case SpvCapabilityMultiViewport: return "SpvCapabilityMultiViewport";
   case SpvCapabilitySubgroupDispatch: return "SpvCapabilitySubgroupDispatch";
   case SpvCapabilityNamedBarrier: return "SpvCapabilityNamedBarrier";
   case SpvCapabilityPipeStorage: return "SpvCapabilityPipeStorage";
   case SpvCapabilitySubgroupBallotKHR: return "SpvCapabilitySubgroupBallotKHR";
   case SpvCapabilityDrawParameters: return "SpvCapabilityDrawParameters";
   case SpvCapabilitySubgroupVoteKHR: return "SpvCapabilitySubgroupVoteKHR";
   case SpvCapabilityStorageBuffer16BitAccess: return "SpvCapabilityStorageBuffer16BitAccess";
   case SpvCapabilityUniformAndStorageBuffer16BitAccess: return "SpvCapabilityUniformAndStorageBuffer16BitAccess";
   case SpvCapabilityStoragePushConstant16: return "SpvCapabilityStoragePushConstant16";
   case SpvCapabilityStorageInputOutput16: return "SpvCapabilityStorageInputOutput16";
   case SpvCapabilityDeviceGroup: return "SpvCapabilityDeviceGroup";
   case SpvCapabilityMultiView: return "SpvCapabilityMultiView";
   case SpvCapabilityVariablePointersStorageBuffer: return "SpvCapabilityVariablePointersStorageBuffer";
   case SpvCapabilityVariablePointers: return "SpvCapabilityVariablePointers";
   case SpvCapabilityAtomicStorageOps: return "SpvCapabilityAtomicStorageOps";
   case SpvCapabilitySampleMaskPostDepthCoverage: return "SpvCapabilitySampleMaskPostDepthCoverage";
   case SpvCapabilityImageGatherBiasLodAMD: return "SpvCapabilityImageGatherBiasLodAMD";
   case SpvCapabilityFragmentMaskAMD: return "SpvCapabilityFragmentMaskAMD";
   case SpvCapabilityStencilExportEXT: return "SpvCapabilityStencilExportEXT";
   case SpvCapabilityImageReadWriteLodAMD: return "SpvCapabilityImageReadWriteLodAMD";
   case SpvCapabilitySampleMaskOverrideCoverageNV: return "SpvCapabilitySampleMaskOverrideCoverageNV";
   case SpvCapabilityGeometryShaderPassthroughNV: return "SpvCapabilityGeometryShaderPassthroughNV";
   case SpvCapabilityShaderViewportIndexLayerEXT: return "SpvCapabilityShaderViewportIndexLayerEXT";
   case SpvCapabilityShaderViewportMaskNV: return "SpvCapabilityShaderViewportMaskNV";
   case SpvCapabilityShaderStereoViewNV: return "SpvCapabilityShaderStereoViewNV";
   case SpvCapabilityPerViewAttributesNV: return "SpvCapabilityPerViewAttributesNV";
   case SpvCapabilityFragmentFullyCoveredEXT: return "SpvCapabilityFragmentFullyCoveredEXT";
   case SpvCapabilitySubgroupShuffleINTEL: return "SpvCapabilitySubgroupShuffleINTEL";
   case SpvCapabilitySubgroupBufferBlockIOINTEL: return "SpvCapabilitySubgroupBufferBlockIOINTEL";
   case SpvCapabilitySubgroupImageBlockIOINTEL: return "SpvCapabilitySubgroupImageBlockIOINTEL";
   case SpvCapabilityMax: break; /* silence warnings about unhandled enums. */
   }

   return "unknown";
}

const char *
spirv_decoration_to_string(SpvDecoration v)
{
   switch (v) {
   case SpvDecorationRelaxedPrecision: return "SpvDecorationRelaxedPrecision";
   case SpvDecorationSpecId: return "SpvDecorationSpecId";
   case SpvDecorationBlock: return "SpvDecorationBlock";
   case SpvDecorationBufferBlock: return "SpvDecorationBufferBlock";
   case SpvDecorationRowMajor: return "SpvDecorationRowMajor";
   case SpvDecorationColMajor: return "SpvDecorationColMajor";
   case SpvDecorationArrayStride: return "SpvDecorationArrayStride";
   case SpvDecorationMatrixStride: return "SpvDecorationMatrixStride";
   case SpvDecorationGLSLShared: return "SpvDecorationGLSLShared";
   case SpvDecorationGLSLPacked: return "SpvDecorationGLSLPacked";
   case SpvDecorationCPacked: return "SpvDecorationCPacked";
   case SpvDecorationBuiltIn: return "SpvDecorationBuiltIn";
   case SpvDecorationNoPerspective: return "SpvDecorationNoPerspective";
   case SpvDecorationFlat: return "SpvDecorationFlat";
   case SpvDecorationPatch: return "SpvDecorationPatch";
   case SpvDecorationCentroid: return "SpvDecorationCentroid";
   case SpvDecorationSample: return "SpvDecorationSample";
   case SpvDecorationInvariant: return "SpvDecorationInvariant";
   case SpvDecorationRestrict: return "SpvDecorationRestrict";
   case SpvDecorationAliased: return "SpvDecorationAliased";
   case SpvDecorationVolatile: return "SpvDecorationVolatile";
   case SpvDecorationConstant: return "SpvDecorationConstant";
   case SpvDecorationCoherent: return "SpvDecorationCoherent";
   case SpvDecorationNonWritable: return "SpvDecorationNonWritable";
   case SpvDecorationNonReadable: return "SpvDecorationNonReadable";
   case SpvDecorationUniform: return "SpvDecorationUniform";
   case SpvDecorationSaturatedConversion: return "SpvDecorationSaturatedConversion";
   case SpvDecorationStream: return "SpvDecorationStream";
   case SpvDecorationLocation: return "SpvDecorationLocation";
   case SpvDecorationComponent: return "SpvDecorationComponent";
   case SpvDecorationIndex: return "SpvDecorationIndex";
   case SpvDecorationBinding: return "SpvDecorationBinding";
   case SpvDecorationDescriptorSet: return "SpvDecorationDescriptorSet";
   case SpvDecorationOffset: return "SpvDecorationOffset";
   case SpvDecorationXfbBuffer: return "SpvDecorationXfbBuffer";
   case SpvDecorationXfbStride: return "SpvDecorationXfbStride";
   case SpvDecorationFuncParamAttr: return "SpvDecorationFuncParamAttr";
   case SpvDecorationFPRoundingMode: return "SpvDecorationFPRoundingMode";
   case SpvDecorationFPFastMathMode: return "SpvDecorationFPFastMathMode";
   case SpvDecorationLinkageAttributes: return "SpvDecorationLinkageAttributes";
   case SpvDecorationNoContraction: return "SpvDecorationNoContraction";
   case SpvDecorationInputAttachmentIndex: return "SpvDecorationInputAttachmentIndex";
   case SpvDecorationAlignment: return "SpvDecorationAlignment";
   case SpvDecorationMaxByteOffset: return "SpvDecorationMaxByteOffset";
   case SpvDecorationAlignmentId: return "SpvDecorationAlignmentId";
   case SpvDecorationMaxByteOffsetId: return "SpvDecorationMaxByteOffsetId";
   case SpvDecorationExplicitInterpAMD: return "SpvDecorationExplicitInterpAMD";
   case SpvDecorationOverrideCoverageNV: return "SpvDecorationOverrideCoverageNV";
   case SpvDecorationPassthroughNV: return "SpvDecorationPassthroughNV";
   case SpvDecorationViewportRelativeNV: return "SpvDecorationViewportRelativeNV";
   case SpvDecorationSecondaryViewportRelativeNV: return "SpvDecorationSecondaryViewportRelativeNV";
   case SpvDecorationMax: break; /* silence warnings about unhandled enums. */
   }

   return "unknown";
}

const char *
spirv_op_to_string(SpvOp v)
{
   switch (v) {
   case SpvOpNop: return "SpvOpNop";
   case SpvOpUndef: return "SpvOpUndef";
   case SpvOpSourceContinued: return "SpvOpSourceContinued";
   case SpvOpSource: return "SpvOpSource";
   case SpvOpSourceExtension: return "SpvOpSourceExtension";
   case SpvOpName: return "SpvOpName";
   case SpvOpMemberName: return "SpvOpMemberName";
   case SpvOpString: return "SpvOpString";
   case SpvOpLine: return "SpvOpLine";
   case SpvOpExtension: return "SpvOpExtension";
   case SpvOpExtInstImport: return "SpvOpExtInstImport";
   case SpvOpExtInst: return "SpvOpExtInst";
   case SpvOpMemoryModel: return "SpvOpMemoryModel";
   case SpvOpEntryPoint: return "SpvOpEntryPoint";
   case SpvOpExecutionMode: return "SpvOpExecutionMode";
   case SpvOpCapability: return "SpvOpCapability";
   case SpvOpTypeVoid: return "SpvOpTypeVoid";
   case SpvOpTypeBool: return "SpvOpTypeBool";
   case SpvOpTypeInt: return "SpvOpTypeInt";
   case SpvOpTypeFloat: return "SpvOpTypeFloat";
   case SpvOpTypeVector: return "SpvOpTypeVector";
   case SpvOpTypeMatrix: return "SpvOpTypeMatrix";
   case SpvOpTypeImage: return "SpvOpTypeImage";
   case SpvOpTypeSampler: return "SpvOpTypeSampler";
   case SpvOpTypeSampledImage: return "SpvOpTypeSampledImage";
   case SpvOpTypeArray: return "SpvOpTypeArray";
   case SpvOpTypeRuntimeArray: return "SpvOpTypeRuntimeArray";
   case SpvOpTypeStruct: return "SpvOpTypeStruct";
   case SpvOpTypeOpaque: return "SpvOpTypeOpaque";
   case SpvOpTypePointer: return "SpvOpTypePointer";
   case SpvOpTypeFunction: return "SpvOpTypeFunction";
   case SpvOpTypeEvent: return "SpvOpTypeEvent";
   case SpvOpTypeDeviceEvent: return "SpvOpTypeDeviceEvent";
   case SpvOpTypeReserveId: return "SpvOpTypeReserveId";
   case SpvOpTypeQueue: return "SpvOpTypeQueue";
   case SpvOpTypePipe: return "SpvOpTypePipe";
   case SpvOpTypeForwardPointer: return "SpvOpTypeForwardPointer";
   case SpvOpConstantTrue: return "SpvOpConstantTrue";
   case SpvOpConstantFalse: return "SpvOpConstantFalse";
   case SpvOpConstant: return "SpvOpConstant";
   case SpvOpConstantComposite: return "SpvOpConstantComposite";
   case SpvOpConstantSampler: return "SpvOpConstantSampler";
   case SpvOpConstantNull: return "SpvOpConstantNull";
   case SpvOpSpecConstantTrue: return "SpvOpSpecConstantTrue";
   case SpvOpSpecConstantFalse: return "SpvOpSpecConstantFalse";
   case SpvOpSpecConstant: return "SpvOpSpecConstant";
   case SpvOpSpecConstantComposite: return "SpvOpSpecConstantComposite";
   case SpvOpSpecConstantOp: return "SpvOpSpecConstantOp";
   case SpvOpFunction: return "SpvOpFunction";
   case SpvOpFunctionParameter: return "SpvOpFunctionParameter";
   case SpvOpFunctionEnd: return "SpvOpFunctionEnd";
   case SpvOpFunctionCall: return "SpvOpFunctionCall";
   case SpvOpVariable: return "SpvOpVariable";
   case SpvOpImageTexelPointer: return "SpvOpImageTexelPointer";
   case SpvOpLoad: return "SpvOpLoad";
   case SpvOpStore: return "SpvOpStore";
   case SpvOpCopyMemory: return "SpvOpCopyMemory";
   case SpvOpCopyMemorySized: return "SpvOpCopyMemorySized";
   case SpvOpAccessChain: return "SpvOpAccessChain";
   case SpvOpInBoundsAccessChain: return "SpvOpInBoundsAccessChain";
   case SpvOpPtrAccessChain: return "SpvOpPtrAccessChain";
   case SpvOpArrayLength: return "SpvOpArrayLength";
   case SpvOpGenericPtrMemSemantics: return "SpvOpGenericPtrMemSemantics";
   case SpvOpInBoundsPtrAccessChain: return "SpvOpInBoundsPtrAccessChain";
   case SpvOpDecorate: return "SpvOpDecorate";
   case SpvOpMemberDecorate: return "SpvOpMemberDecorate";
   case SpvOpDecorationGroup: return "SpvOpDecorationGroup";
   case SpvOpGroupDecorate: return "SpvOpGroupDecorate";
   case SpvOpGroupMemberDecorate: return "SpvOpGroupMemberDecorate";
   case SpvOpVectorExtractDynamic: return "SpvOpVectorExtractDynamic";
   case SpvOpVectorInsertDynamic: return "SpvOpVectorInsertDynamic";
   case SpvOpVectorShuffle: return "SpvOpVectorShuffle";
   case SpvOpCompositeConstruct: return "SpvOpCompositeConstruct";
   case SpvOpCompositeExtract: return "SpvOpCompositeExtract";
   case SpvOpCompositeInsert: return "SpvOpCompositeInsert";
   case SpvOpCopyObject: return "SpvOpCopyObject";
   case SpvOpTranspose: return "SpvOpTranspose";
   case SpvOpSampledImage: return "SpvOpSampledImage";
   case SpvOpImageSampleImplicitLod: return "SpvOpImageSampleImplicitLod";
   case SpvOpImageSampleExplicitLod: return "SpvOpImageSampleExplicitLod";
   case SpvOpImageSampleDrefImplicitLod: return "SpvOpImageSampleDrefImplicitLod";
   case SpvOpImageSampleDrefExplicitLod: return "SpvOpImageSampleDrefExplicitLod";
   case SpvOpImageSampleProjImplicitLod: return "SpvOpImageSampleProjImplicitLod";
   case SpvOpImageSampleProjExplicitLod: return "SpvOpImageSampleProjExplicitLod";
   case SpvOpImageSampleProjDrefImplicitLod: return "SpvOpImageSampleProjDrefImplicitLod";
   case SpvOpImageSampleProjDrefExplicitLod: return "SpvOpImageSampleProjDrefExplicitLod";
   case SpvOpImageFetch: return "SpvOpImageFetch";
   case SpvOpImageGather: return "SpvOpImageGather";
   case SpvOpImageDrefGather: return "SpvOpImageDrefGather";
   case SpvOpImageRead: return "SpvOpImageRead";
   case SpvOpImageWrite: return "SpvOpImageWrite";
   case SpvOpImage: return "SpvOpImage";
   case SpvOpImageQueryFormat: return "SpvOpImageQueryFormat";
   case SpvOpImageQueryOrder: return "SpvOpImageQueryOrder";
   case SpvOpImageQuerySizeLod: return "SpvOpImageQuerySizeLod";
   case SpvOpImageQuerySize: return "SpvOpImageQuerySize";
   case SpvOpImageQueryLod: return "SpvOpImageQueryLod";
   case SpvOpImageQueryLevels: return "SpvOpImageQueryLevels";
   case SpvOpImageQuerySamples: return "SpvOpImageQuerySamples";
   case SpvOpConvertFToU: return "SpvOpConvertFToU";
   case SpvOpConvertFToS: return "SpvOpConvertFToS";
   case SpvOpConvertSToF: return "SpvOpConvertSToF";
   case SpvOpConvertUToF: return "SpvOpConvertUToF";
   case SpvOpUConvert: return "SpvOpUConvert";
   case SpvOpSConvert: return "SpvOpSConvert";
   case SpvOpFConvert: return "SpvOpFConvert";
   case SpvOpQuantizeToF16: return "SpvOpQuantizeToF16";
   case SpvOpConvertPtrToU: return "SpvOpConvertPtrToU";
   case SpvOpSatConvertSToU: return "SpvOpSatConvertSToU";
   case SpvOpSatConvertUToS: return "SpvOpSatConvertUToS";
   case SpvOpConvertUToPtr: return "SpvOpConvertUToPtr";
   case SpvOpPtrCastToGeneric: return "SpvOpPtrCastToGeneric";
   case SpvOpGenericCastToPtr: return "SpvOpGenericCastToPtr";
   case SpvOpGenericCastToPtrExplicit: return "SpvOpGenericCastToPtrExplicit";
   case SpvOpBitcast: return "SpvOpBitcast";
   case SpvOpSNegate: return "SpvOpSNegate";
   case SpvOpFNegate: return "SpvOpFNegate";
   case SpvOpIAdd: return "SpvOpIAdd";
   case SpvOpFAdd: return "SpvOpFAdd";
   case SpvOpISub: return "SpvOpISub";
   case SpvOpFSub: return "SpvOpFSub";
   case SpvOpIMul: return "SpvOpIMul";
   case SpvOpFMul: return "SpvOpFMul";
   case SpvOpUDiv: return "SpvOpUDiv";
   case SpvOpSDiv: return "SpvOpSDiv";
   case SpvOpFDiv: return "SpvOpFDiv";
   case SpvOpUMod: return "SpvOpUMod";
   case SpvOpSRem: return "SpvOpSRem";
   case SpvOpSMod: return "SpvOpSMod";
   case SpvOpFRem: return "SpvOpFRem";
   case SpvOpFMod: return "SpvOpFMod";
   case SpvOpVectorTimesScalar: return "SpvOpVectorTimesScalar";
   case SpvOpMatrixTimesScalar: return "SpvOpMatrixTimesScalar";
   case SpvOpVectorTimesMatrix: return "SpvOpVectorTimesMatrix";
   case SpvOpMatrixTimesVector: return "SpvOpMatrixTimesVector";
   case SpvOpMatrixTimesMatrix: return "SpvOpMatrixTimesMatrix";
   case SpvOpOuterProduct: return "SpvOpOuterProduct";
   case SpvOpDot: return "SpvOpDot";
   case SpvOpIAddCarry: return "SpvOpIAddCarry";
   case SpvOpISubBorrow: return "SpvOpISubBorrow";
   case SpvOpUMulExtended: return "SpvOpUMulExtended";
   case SpvOpSMulExtended: return "SpvOpSMulExtended";
   case SpvOpAny: return "SpvOpAny";
   case SpvOpAll: return "SpvOpAll";
   case SpvOpIsNan: return "SpvOpIsNan";
   case SpvOpIsInf: return "SpvOpIsInf";
   case SpvOpIsFinite: return "SpvOpIsFinite";
   case SpvOpIsNormal: return "SpvOpIsNormal";
   case SpvOpSignBitSet: return "SpvOpSignBitSet";
   case SpvOpLessOrGreater: return "SpvOpLessOrGreater";
   case SpvOpOrdered: return "SpvOpOrdered";
   case SpvOpUnordered: return "SpvOpUnordered";
   case SpvOpLogicalEqual: return "SpvOpLogicalEqual";
   case SpvOpLogicalNotEqual: return "SpvOpLogicalNotEqual";
   case SpvOpLogicalOr: return "SpvOpLogicalOr";
   case SpvOpLogicalAnd: return "SpvOpLogicalAnd";
   case SpvOpLogicalNot: return "SpvOpLogicalNot";
   case SpvOpSelect: return "SpvOpSelect";
   case SpvOpIEqual: return "SpvOpIEqual";
   case SpvOpINotEqual: return "SpvOpINotEqual";
   case SpvOpUGreaterThan: return "SpvOpUGreaterThan";
   case SpvOpSGreaterThan: return "SpvOpSGreaterThan";
   case SpvOpUGreaterThanEqual: return "SpvOpUGreaterThanEqual";
   case SpvOpSGreaterThanEqual: return "SpvOpSGreaterThanEqual";
   case SpvOpULessThan: return "SpvOpULessThan";
   case SpvOpSLessThan: return "SpvOpSLessThan";
   case SpvOpULessThanEqual: return "SpvOpULessThanEqual";
   case SpvOpSLessThanEqual: return "SpvOpSLessThanEqual";
   case SpvOpFOrdEqual: return "SpvOpFOrdEqual";
   case SpvOpFUnordEqual: return "SpvOpFUnordEqual";
   case SpvOpFOrdNotEqual: return "SpvOpFOrdNotEqual";
   case SpvOpFUnordNotEqual: return "SpvOpFUnordNotEqual";
   case SpvOpFOrdLessThan: return "SpvOpFOrdLessThan";
   case SpvOpFUnordLessThan: return "SpvOpFUnordLessThan";
   case SpvOpFOrdGreaterThan: return "SpvOpFOrdGreaterThan";
   case SpvOpFUnordGreaterThan: return "SpvOpFUnordGreaterThan";
   case SpvOpFOrdLessThanEqual: return "SpvOpFOrdLessThanEqual";
   case SpvOpFUnordLessThanEqual: return "SpvOpFUnordLessThanEqual";
   case SpvOpFOrdGreaterThanEqual: return "SpvOpFOrdGreaterThanEqual";
   case SpvOpFUnordGreaterThanEqual: return "SpvOpFUnordGreaterThanEqual";
   case SpvOpShiftRightLogical: return "SpvOpShiftRightLogical";
   case SpvOpShiftRightArithmetic: return "SpvOpShiftRightArithmetic";
   case SpvOpShiftLeftLogical: return "SpvOpShiftLeftLogical";
   case SpvOpBitwiseOr: return "SpvOpBitwiseOr";
   case SpvOpBitwiseXor: return "SpvOpBitwiseXor";
   case SpvOpBitwiseAnd: return "SpvOpBitwiseAnd";
   case SpvOpNot: return "SpvOpNot";
   case SpvOpBitFieldInsert: return "SpvOpBitFieldInsert";
   case SpvOpBitFieldSExtract: return "SpvOpBitFieldSExtract";
   case SpvOpBitFieldUExtract: return "SpvOpBitFieldUExtract";
   case SpvOpBitReverse: return "SpvOpBitReverse";
   case SpvOpBitCount: return "SpvOpBitCount";
   case SpvOpDPdx: return "SpvOpDPdx";
   case SpvOpDPdy: return "SpvOpDPdy";
   case SpvOpFwidth: return "SpvOpFwidth";
   case SpvOpDPdxFine: return "SpvOpDPdxFine";
   case SpvOpDPdyFine: return "SpvOpDPdyFine";
   case SpvOpFwidthFine: return "SpvOpFwidthFine";
   case SpvOpDPdxCoarse: return "SpvOpDPdxCoarse";
   case SpvOpDPdyCoarse: return "SpvOpDPdyCoarse";
   case SpvOpFwidthCoarse: return "SpvOpFwidthCoarse";
   case SpvOpEmitVertex: return "SpvOpEmitVertex";
   case SpvOpEndPrimitive: return "SpvOpEndPrimitive";
   case SpvOpEmitStreamVertex: return "SpvOpEmitStreamVertex";
   case SpvOpEndStreamPrimitive: return "SpvOpEndStreamPrimitive";
   case SpvOpControlBarrier: return "SpvOpControlBarrier";
   case SpvOpMemoryBarrier: return "SpvOpMemoryBarrier";
   case SpvOpAtomicLoad: return "SpvOpAtomicLoad";
   case SpvOpAtomicStore: return "SpvOpAtomicStore";
   case SpvOpAtomicExchange: return "SpvOpAtomicExchange";
   case SpvOpAtomicCompareExchange: return "SpvOpAtomicCompareExchange";
   case SpvOpAtomicCompareExchangeWeak: return "SpvOpAtomicCompareExchangeWeak";
   case SpvOpAtomicIIncrement: return "SpvOpAtomicIIncrement";
   case SpvOpAtomicIDecrement: return "SpvOpAtomicIDecrement";
   case SpvOpAtomicIAdd: return "SpvOpAtomicIAdd";
   case SpvOpAtomicISub: return "SpvOpAtomicISub";
   case SpvOpAtomicSMin: return "SpvOpAtomicSMin";
   case SpvOpAtomicUMin: return "SpvOpAtomicUMin";
   case SpvOpAtomicSMax: return "SpvOpAtomicSMax";
   case SpvOpAtomicUMax: return "SpvOpAtomicUMax";
   case SpvOpAtomicAnd: return "SpvOpAtomicAnd";
   case SpvOpAtomicOr: return "SpvOpAtomicOr";
   case SpvOpAtomicXor: return "SpvOpAtomicXor";
   case SpvOpPhi: return "SpvOpPhi";
   case SpvOpLoopMerge: return "SpvOpLoopMerge";
   case SpvOpSelectionMerge: return "SpvOpSelectionMerge";
   case SpvOpLabel: return "SpvOpLabel";
   case SpvOpBranch: return "SpvOpBranch";
   case SpvOpBranchConditional: return "SpvOpBranchConditional";
   case SpvOpSwitch: return "SpvOpSwitch";
   case SpvOpKill: return "SpvOpKill";
   case SpvOpReturn: return "SpvOpReturn";
   case SpvOpReturnValue: return "SpvOpReturnValue";
   case SpvOpUnreachable: return "SpvOpUnreachable";
   case SpvOpLifetimeStart: return "SpvOpLifetimeStart";
   case SpvOpLifetimeStop: return "SpvOpLifetimeStop";
   case SpvOpGroupAsyncCopy: return "SpvOpGroupAsyncCopy";
   case SpvOpGroupWaitEvents: return "SpvOpGroupWaitEvents";
   case SpvOpGroupAll: return "SpvOpGroupAll";
   case SpvOpGroupAny: return "SpvOpGroupAny";
   case SpvOpGroupBroadcast: return "SpvOpGroupBroadcast";
   case SpvOpGroupIAdd: return "SpvOpGroupIAdd";
   case SpvOpGroupFAdd: return "SpvOpGroupFAdd";
   case SpvOpGroupFMin: return "SpvOpGroupFMin";
   case SpvOpGroupUMin: return "SpvOpGroupUMin";
   case SpvOpGroupSMin: return "SpvOpGroupSMin";
   case SpvOpGroupFMax: return "SpvOpGroupFMax";
   case SpvOpGroupUMax: return "SpvOpGroupUMax";
   case SpvOpGroupSMax: return "SpvOpGroupSMax";
   case SpvOpReadPipe: return "SpvOpReadPipe";
   case SpvOpWritePipe: return "SpvOpWritePipe";
   case SpvOpReservedReadPipe: return "SpvOpReservedReadPipe";
   case SpvOpReservedWritePipe: return "SpvOpReservedWritePipe";
   case SpvOpReserveReadPipePackets: return "SpvOpReserveReadPipePackets";
   case SpvOpReserveWritePipePackets: return "SpvOpReserveWritePipePackets";
   case SpvOpCommitReadPipe: return "SpvOpCommitReadPipe";
   case SpvOpCommitWritePipe: return "SpvOpCommitWritePipe";
   case SpvOpIsValidReserveId: return "SpvOpIsValidReserveId";
   case SpvOpGetNumPipePackets: return "SpvOpGetNumPipePackets";
   case SpvOpGetMaxPipePackets: return "SpvOpGetMaxPipePackets";
   case SpvOpGroupReserveReadPipePackets: return "SpvOpGroupReserveReadPipePackets";
   case SpvOpGroupReserveWritePipePackets: return "SpvOpGroupReserveWritePipePackets";
   case SpvOpGroupCommitReadPipe: return "SpvOpGroupCommitReadPipe";
   case SpvOpGroupCommitWritePipe: return "SpvOpGroupCommitWritePipe";
   case SpvOpEnqueueMarker: return "SpvOpEnqueueMarker";
   case SpvOpEnqueueKernel: return "SpvOpEnqueueKernel";
   case SpvOpGetKernelNDrangeSubGroupCount: return "SpvOpGetKernelNDrangeSubGroupCount";
   case SpvOpGetKernelNDrangeMaxSubGroupSize: return "SpvOpGetKernelNDrangeMaxSubGroupSize";
   case SpvOpGetKernelWorkGroupSize: return "SpvOpGetKernelWorkGroupSize";
   case SpvOpGetKernelPreferredWorkGroupSizeMultiple: return "SpvOpGetKernelPreferredWorkGroupSizeMultiple";
   case SpvOpRetainEvent: return "SpvOpRetainEvent";
   case SpvOpReleaseEvent: return "SpvOpReleaseEvent";
   case SpvOpCreateUserEvent: return "SpvOpCreateUserEvent";
   case SpvOpIsValidEvent: return "SpvOpIsValidEvent";
   case SpvOpSetUserEventStatus: return "SpvOpSetUserEventStatus";
   case SpvOpCaptureEventProfilingInfo: return "SpvOpCaptureEventProfilingInfo";
   case SpvOpGetDefaultQueue: return "SpvOpGetDefaultQueue";
   case SpvOpBuildNDRange: return "SpvOpBuildNDRange";
   case SpvOpImageSparseSampleImplicitLod: return "SpvOpImageSparseSampleImplicitLod";
   case SpvOpImageSparseSampleExplicitLod: return "SpvOpImageSparseSampleExplicitLod";
   case SpvOpImageSparseSampleDrefImplicitLod: return "SpvOpImageSparseSampleDrefImplicitLod";
   case SpvOpImageSparseSampleDrefExplicitLod: return "SpvOpImageSparseSampleDrefExplicitLod";
   case SpvOpImageSparseSampleProjImplicitLod: return "SpvOpImageSparseSampleProjImplicitLod";
   case SpvOpImageSparseSampleProjExplicitLod: return "SpvOpImageSparseSampleProjExplicitLod";
   case SpvOpImageSparseSampleProjDrefImplicitLod: return "SpvOpImageSparseSampleProjDrefImplicitLod";
   case SpvOpImageSparseSampleProjDrefExplicitLod: return "SpvOpImageSparseSampleProjDrefExplicitLod";
   case SpvOpImageSparseFetch: return "SpvOpImageSparseFetch";
   case SpvOpImageSparseGather: return "SpvOpImageSparseGather";
   case SpvOpImageSparseDrefGather: return "SpvOpImageSparseDrefGather";
   case SpvOpImageSparseTexelsResident: return "SpvOpImageSparseTexelsResident";
   case SpvOpNoLine: return "SpvOpNoLine";
   case SpvOpAtomicFlagTestAndSet: return "SpvOpAtomicFlagTestAndSet";
   case SpvOpAtomicFlagClear: return "SpvOpAtomicFlagClear";
   case SpvOpImageSparseRead: return "SpvOpImageSparseRead";
   case SpvOpSizeOf: return "SpvOpSizeOf";
   case SpvOpTypePipeStorage: return "SpvOpTypePipeStorage";
   case SpvOpConstantPipeStorage: return "SpvOpConstantPipeStorage";
   case SpvOpCreatePipeFromPipeStorage: return "SpvOpCreatePipeFromPipeStorage";
   case SpvOpGetKernelLocalSizeForSubgroupCount: return "SpvOpGetKernelLocalSizeForSubgroupCount";
   case SpvOpGetKernelMaxNumSubgroups: return "SpvOpGetKernelMaxNumSubgroups";
   case SpvOpTypeNamedBarrier: return "SpvOpTypeNamedBarrier";
   case SpvOpNamedBarrierInitialize: return "SpvOpNamedBarrierInitialize";
   case SpvOpMemoryNamedBarrier: return "SpvOpMemoryNamedBarrier";
   case SpvOpModuleProcessed: return "SpvOpModuleProcessed";
   case SpvOpExecutionModeId: return "SpvOpExecutionModeId";
   case SpvOpDecorateId: return "SpvOpDecorateId";
   case SpvOpSubgroupBallotKHR: return "SpvOpSubgroupBallotKHR";
   case SpvOpSubgroupFirstInvocationKHR: return "SpvOpSubgroupFirstInvocationKHR";
   case SpvOpSubgroupAllKHR: return "SpvOpSubgroupAllKHR";
   case SpvOpSubgroupAnyKHR: return "SpvOpSubgroupAnyKHR";
   case SpvOpSubgroupAllEqualKHR: return "SpvOpSubgroupAllEqualKHR";
   case SpvOpSubgroupReadInvocationKHR: return "SpvOpSubgroupReadInvocationKHR";
   case SpvOpGroupIAddNonUniformAMD: return "SpvOpGroupIAddNonUniformAMD";
   case SpvOpGroupFAddNonUniformAMD: return "SpvOpGroupFAddNonUniformAMD";
   case SpvOpGroupFMinNonUniformAMD: return "SpvOpGroupFMinNonUniformAMD";
   case SpvOpGroupUMinNonUniformAMD: return "SpvOpGroupUMinNonUniformAMD";
   case SpvOpGroupSMinNonUniformAMD: return "SpvOpGroupSMinNonUniformAMD";
   case SpvOpGroupFMaxNonUniformAMD: return "SpvOpGroupFMaxNonUniformAMD";
   case SpvOpGroupUMaxNonUniformAMD: return "SpvOpGroupUMaxNonUniformAMD";
   case SpvOpGroupSMaxNonUniformAMD: return "SpvOpGroupSMaxNonUniformAMD";
   case SpvOpFragmentMaskFetchAMD: return "SpvOpFragmentMaskFetchAMD";
   case SpvOpFragmentFetchAMD: return "SpvOpFragmentFetchAMD";
   case SpvOpSubgroupShuffleINTEL: return "SpvOpSubgroupShuffleINTEL";
   case SpvOpSubgroupShuffleDownINTEL: return "SpvOpSubgroupShuffleDownINTEL";
   case SpvOpSubgroupShuffleUpINTEL: return "SpvOpSubgroupShuffleUpINTEL";
   case SpvOpSubgroupShuffleXorINTEL: return "SpvOpSubgroupShuffleXorINTEL";
   case SpvOpSubgroupBlockReadINTEL: return "SpvOpSubgroupBlockReadINTEL";
   case SpvOpSubgroupBlockWriteINTEL: return "SpvOpSubgroupBlockWriteINTEL";
   case SpvOpSubgroupImageBlockReadINTEL: return "SpvOpSubgroupImageBlockReadINTEL";
   case SpvOpSubgroupImageBlockWriteINTEL: return "SpvOpSubgroupImageBlockWriteINTEL";
   case SpvOpMax: break; /* silence warnings about unhandled enums. */
   }

   return "unknown";
}
