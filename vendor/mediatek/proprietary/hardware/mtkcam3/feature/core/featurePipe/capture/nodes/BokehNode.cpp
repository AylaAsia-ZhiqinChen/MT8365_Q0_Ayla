/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2018. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

// Standard C header file
#include <stdarg.h>
#include <chrono>
// Android system/core header file

// mtkcam custom header file

// mtkcam global header file

// Module header file
#include <mtkcam3/3rdparty/plugin/PipelinePluginType.h>
#include <mtkcam/drv/iopipe/SImager/IImageTransform.h>
#include <mtkcam/utils/metadata/client/mtk_metadata_tag.h>
// Local header file
#include "../CaptureFeaturePlugin.h"
#include "BokehNode.h"
// Logging
#define PIPE_CLASS_TAG "BokehNode"
#define PIPE_TRACE TRACE_BOKEH_NODE
#include <featurePipe/core/include/PipeLog.h>
#include <mtkcam/utils/metadata/hal/mtk_platform_metadata_tag.h>

CAM_ULOG_DECLARE_MODULE_ID(MOD_CAPTURE_BOKEH);


/*******************************************************************************
* MACRO Utilities Define.
********************************************************************************/
namespace { // anonymous namespace for MACRO function
using CamThread      = NSCam::NSCamFeature::NSFeaturePipe::CamThread;
using AutoObject     = std::unique_ptr<const char, std::function<void(const char*)>>;
using AutoDependency = std::unique_ptr<CamThread, std::function<MVOID(CamThread*)>>;
//
auto
createAutoScoper(const char* funcName) -> AutoObject
{
    CAM_ULOGMD("[%s] +", funcName);
    return AutoObject(funcName, [](const char* p)
    {
        CAM_ULOGMD("[%s] -", p);
    });
}
#define SCOPED_TRACER() auto scoped_tracer = ::createAutoScoper(__FUNCTION__)
//
auto
createAutoProfiler(const char* funcName, const char* fmt, ...) -> AutoObject
{
    static const MINT32 LENGTH = 512;

    char* pBuf = new char[LENGTH];

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(pBuf, LENGTH, fmt, ap);
    va_end(ap);

    CAM_TRACE_BEGIN(pBuf);
    CAM_ULOGMD("[%s]%s +", funcName, pBuf);

    return AutoObject(pBuf, [funcName](const char* p)
    {
        CAM_ULOGMD("[%s]%s -", funcName, p);
        CAM_TRACE_END();

        delete[] p;
    });
}
#define AUTO_PROFILER(FMT, arg...) auto auto_profiler = ::createAutoProfiler(__FUNCTION__, FMT, ##arg)
//
auto
createAutoTimer(const char* funcName, const char* fmt, ...) -> AutoObject
{
    static const MINT32 LENGTH = 512;
    using Timing = std::chrono::time_point<std::chrono::high_resolution_clock>;
    using DuationTime = std::chrono::duration<float, std::milli>;
    //
    char* pBuf = new char[LENGTH];

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(pBuf, LENGTH, fmt, ap);
    va_end(ap);
    //
    Timing startTime = std::chrono::high_resolution_clock::now();
    return AutoObject(pBuf, [funcName, startTime](const char* p)
    {
        Timing endTime = std::chrono::high_resolution_clock::now();
        DuationTime duationTime = endTime - startTime;
        CAM_ULOGMD("[%s]%s, elapsed(ms):%.4f", funcName, p, duationTime.count());

        delete[] p;
    });
}
#define AUTO_TIMER(FMT, arg...) auto auto_timer = ::createAutoTimer(__FUNCTION__, FMT, ##arg);
//
#define UNREFERENCED_PARAMETER(param) (param)
#define TO_STRING(val) (#val)
//
} // end anonymous namespace for MACRO function


/*******************************************************************************
* Namespace start.
********************************************************************************/
namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {
namespace NSCapture {


/*******************************************************************************
* Type Alias.
********************************************************************************/
using namespace NSCam::NSPipelinePlugin;
using namespace NSCam::NSIoPipe::NSSImager;
//
using NodeRequestPtr    = sp<CaptureFeatureNodeRequest>;
//
using PluginPtr         = BokehNode::PluginPtr;
using PipeRequestPtr    = BokehNode::PipeRequestPtr;
using PluginRequestPtr  = BokehNode::PluginRequestPtr;
using BufferHandlePtr   = BokehNode::BufferHandlePtr;
using MetadataHandlePtr = BokehNode::MetadataHandlePtr;
//
using ImageTransformPtr   = UniquePtr<IImageTransform>;


/*******************************************************************************
* Global Function.
*******************************************************************************/
static inline auto toString(IImageBuffer* pImgBuf) -> std::string;


/*******************************************************************************
* Class Define
*******************************************************************************/
/**
 * @brief the depth offer interface
 */
class BokehInterface final: public BokehPlugin::IInterface
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  BokehPlugin::IInterface Public Operators.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    MERROR offer(BokehPlugin::Selection& sel) override;

};
REGISTER_PLUGIN_INTERFACE(Bokeh, BokehInterface);
/**
 * @brief utility class of the bokeh node
 */
class BokehNodeUtility final
{
public:
    BokehNodeUtility() = delete;

public:
    static auto createPluginRequestPtr(NodeID_T nodeId, PluginPtr& pluginPtr, PipeRequestPtr& pipeRequestPtr) -> PluginRequestPtr;

    static auto tryGetFeatureWarning(RequestPtr& pipeRequestPtr, MINT32& featureWarning) -> MBOOL;

    static auto imgCopy(IImageBuffer* pSrcImg, IImageBuffer* pDstImg) -> MBOOL;

    static auto getIsForceByPassFlow() -> MBOOL;

    static auto processNextCapture(const RequestPtr& pipeRequestPtr, MUINT32 timing) -> MVOID;

private:

    static auto createImageTransformPtr() -> ImageTransformPtr;
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Global Function Implementation.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
auto
toString(IImageBuffer* pImgBuf) -> std::string
{
    String8 ret = String8::format("[a:%p, si:%dx%d, st:%zu, f:%#09x, va:%p]",
        pImgBuf,
        pImgBuf->getImgSize().w, pImgBuf->getImgSize().h,
        pImgBuf->getBufStridesInBytes(0),
        pImgBuf->getImgFormat(),
        reinterpret_cast<void*>(pImgBuf->getBufVA(0)));
    return std::string(ret.string());
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  BokehInterface Implementation.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MERROR
BokehInterface::
offer(BokehPlugin::Selection& sel)
{
    sel.mIBufferFull
        .addSupportFormat(eImgFmt_NV21)
        .addSupportFormat(eImgFmt_NV12)
        .addSupportSize(eImgSize_Full);

    sel.mIBufferDepth
        .addSupportFormat(eImgFmt_Y8)
        .addSupportFormat(eImgFmt_NV12)
        .addSupportSize(eImgSize_Specified);

    sel.mOBufferFull
        .addSupportFormat(eImgFmt_NV21)
        .addSupportFormat(eImgFmt_NV12)
        .addSupportSize(eImgSize_Full);

    sel.mOBufferThumbnail
        .addSupportSize(eImgSize_Arbitrary);
    return OK;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  BokehNodeUtility Implementation.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
auto
BokehNodeUtility::
createPluginRequestPtr(NodeID_T nodeId, PluginPtr& pluginPtr, PipeRequestPtr& pipeRequestPtr) -> PluginRequestPtr
{
    NodeRequestPtr nodeRequestPtr = pipeRequestPtr->getNodeRequest(nodeId);
    //
    PluginRequestPtr ret = pluginPtr->createRequest();
    {
        // in/out buffer
        ret->mIBufferFull       = PluginHelper::CreateBuffer(nodeRequestPtr, TID_MAN_FULL_YUV, INPUT);
        ret->mIBufferDepth      = PluginHelper::CreateBuffer(nodeRequestPtr, TID_MAN_DEPTH, INPUT);
        ret->mOBufferFull       = PluginHelper::CreateBuffer(nodeRequestPtr, TID_MAN_FULL_YUV, OUTPUT);
        ret->mOBufferThumbnail   = PluginHelper::CreateBuffer(nodeRequestPtr, TID_POSTVIEW, OUTPUT);
        // in/out metadata
        ret->mIMetadataHal      = PluginHelper::CreateMetadata(nodeRequestPtr, MID_MAN_IN_HAL);
        ret->mIMetadataApp      = PluginHelper::CreateMetadata(nodeRequestPtr, MID_MAN_IN_APP);
        ret->mIMetadataDynamic  = PluginHelper::CreateMetadata(nodeRequestPtr, MID_MAN_IN_P1_DYNAMIC);
        ret->mOMetadataApp      = PluginHelper::CreateMetadata(nodeRequestPtr, MID_MAN_OUT_APP);
        ret->mOMetadataHal      = PluginHelper::CreateMetadata(nodeRequestPtr, MID_MAN_OUT_HAL);
    }
    return ret;
};

auto
BokehNodeUtility::
imgCopy(IImageBuffer* pSrcImg, IImageBuffer* pDstImg) -> MBOOL
{
    MY_LOGD("image copy, srcImgInfo:%s, dstImgInfo:%s", toString(pSrcImg).c_str(), toString(pDstImg).c_str());
    ImageTransformPtr imageTransformPtr = createImageTransformPtr();
    if ( imageTransformPtr == nullptr )
    {
        MY_LOGW("failed to get imageTransform");
        return MFALSE;
    }
    //
    const MSize imgSize = pSrcImg->getImgSize();
    const MRect cropRect = MRect(imgSize.w, imgSize.h);
    if ( !imageTransformPtr->execute(pSrcImg, pDstImg, nullptr, cropRect, 0, 3000) )
    {
        MY_LOGW("failed to execute image transform, srcImg:%p, dstImg:%p", pSrcImg, pDstImg);
        return MFALSE;
    }
    return MTRUE;
}

auto
BokehNodeUtility::
getIsForceByPassFlow() -> MBOOL
{
    MUINT8 flowByPass = 0;
    flowByPass = ::property_get_int32("vendor.debug.camera.bokehnode.force.flowbypass", flowByPass);
    if( flowByPass != 0 )
    {
        MY_LOGD("force flow bypass, val:%d", flowByPass);
        return MTRUE;
    }
    return MFALSE;
}

auto
BokehNodeUtility::
processNextCapture(const RequestPtr& pipeRequestPtr, MUINT32 timing) -> MVOID
{
    const MBOOL isEnableNextCapture = (pipeRequestPtr->getParameter(PID_ENABLE_NEXT_CAPTURE) > 0);
    const MBOOL hasTiming = pipeRequestPtr->hasParameter(PID_THUMBNAIL_TIMING);
    if (!isEnableNextCapture || !hasTiming)
    {
        MY_LOGD("failed to processor next capure, isEnableNextCapture:%d, hasTiming:%d",
            isEnableNextCapture, hasTiming);
        return;
    }

    if (pipeRequestPtr->getParameter(PID_THUMBNAIL_TIMING) == timing)
    {
        const MINT32 requestNo = pipeRequestPtr->getRequestNo();
        const MINT32 frameNo = pipeRequestPtr->getFrameNo();
        if (pipeRequestPtr->mpCallback != nullptr)
        {
            MY_LOGD("Nofity next capture, R/F Num: %d/%d", requestNo, frameNo);
            pipeRequestPtr->mpCallback->onContinue(pipeRequestPtr);
        }
        else
        {
            MY_LOGW("have no request callback instance!");
        }
    }
}

auto
BokehNodeUtility::
createImageTransformPtr() -> ImageTransformPtr
{
    return ImageTransformPtr(IImageTransform::createInstance(), [](IImageTransform *p)
    {
        p->destroyInstance();
    });
}

auto
BokehNodeUtility::
tryGetFeatureWarning(RequestPtr& pipeRequestPtr, MINT32& featureWarning) -> MBOOL
{
    const static MINT32 defaultValue = 0;
    featureWarning = defaultValue;
    if( ::property_get_int32("vendor.debug.camera.vsdof.disable.featurnWarning", 0) != 0 )
    {
        MY_LOGD("force disable feature warning");
        return MTRUE;
    }
    //
    MINT32 featureWarningFromP1 = 0;
    {
        sp<MetadataHandle> inDynamicMetaPtr = pipeRequestPtr->getMetadata(MID_MAN_IN_P1_DYNAMIC);
        IMetadata* pInDynamicMeta = (inDynamicMetaPtr != nullptr) ? inDynamicMetaPtr->native() : nullptr;
        if( pInDynamicMeta == nullptr )
        {
            MY_LOGW("failed to get featureWarning, in-dynamic metadata is not existing");
            return MFALSE;
        }

        IMetadata::IEntry entry = pInDynamicMeta->entryFor(MTK_STEREO_FEATURE_WARNING);
        if( entry.isEmpty() )
        {
            MY_LOGW("failed to get metadata from in-dynamic, tag:%#09x(%s), and set as default value:%d",
                MTK_STEREO_FEATURE_WARNING, TO_STRING(MTK_STEREO_FEATURE_WARNING), defaultValue);
            return MFALSE;
        }
        featureWarningFromP1 = entry.itemAt(0, Type2Type<MINT32>());
    }
    featureWarning |= featureWarningFromP1;
    //
    MINT32 featureWarningFromApp = 0;
    {
        sp<MetadataHandle> inAppMetaPtr = pipeRequestPtr->getMetadata(MID_MAN_IN_APP);
        IMetadata* pInAppMeta = (inAppMetaPtr != nullptr) ? inAppMetaPtr->native() : nullptr;
        if( pInAppMeta == nullptr )
        {
            MY_LOGW("failed to get featureWarning, in-app metadata is not existing");
            return MFALSE;
        }

        IMetadata::IEntry entry = pInAppMeta->entryFor(MTK_VSDOF_FEATURE_CAPTURE_WARNING_MSG);
        if( entry.isEmpty() )
        {
            MY_LOGW("failed to get metadata from in-app, tag:%#09x(%s), and set as default value:%d",
                MTK_VSDOF_FEATURE_CAPTURE_WARNING_MSG, TO_STRING(MTK_VSDOF_FEATURE_CAPTURE_WARNING_MSG), defaultValue);
            return MFALSE;
        }
        featureWarningFromApp = entry.itemAt(0, Type2Type<MINT32>());
    }
    MY_LOGD("get featureWarning, featureWarningFromP1:%#09x, featureWarningFromApp:%#09x", featureWarningFromP1, featureWarningFromApp);
    featureWarning |= featureWarningFromApp;
    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  BokehNode Implementation.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BokehNode::
BokehNode(NodeID_T nodeId, const char* name, MINT32 policy, MINT32 priority)
: CamNodeULogHandler(Utils::ULog::MOD_CAPTURE_BOKEH)
, CaptureFeatureNode(nodeId, name, 0, policy, priority)
// note: we have to use the only one callback object to advoid leak
, mRequestCallbackPtr(this, [](RequestCallback* ptr){ UNREFERENCED_PARAMETER(ptr); })
{
    this->addWaitQueue(&mRequests);
    MY_LOGI("(%p) ctor", this);
}

BokehNode::
~BokehNode()
{
    MY_LOGI("(%p) dtor", this);
}

MBOOL
BokehNode::
onData(DataID dataId, const RequestPtr& pRequest)
{
    SCOPED_TRACER();

    MY_LOGD("request arrived, req#:%d, dataId:%d(%s)",
        pRequest->getRequestNo(), dataId, ID2Name(dataId));

    if ( pRequest->isReadyFor(mNodeId) )
    {
        mRequests.enque(pRequest);
    }
    else
    {
        MY_LOGD("not ready to enque, request is not satisfied, req#:%d, dataId:%d(%s)",
            pRequest->getRequestNo(), dataId, ID2Name(dataId));
    }
    return MTRUE;
}

MVOID
BokehNode::
onFlush()
{
    SCOPED_TRACER();
    CaptureFeatureNode::onFlush();

    MY_LOGD("extThreadDependency:%d, requestSize:%zu", this->getExtThreadDependency(), mRequests.size());
    RequestPtr pipeRequestPtr;
    while ( mRequests.deque(pipeRequestPtr) )
    {
        // TODO: should we need dispatch the request?
        // dispatch(pipeRequestPtr);
    }
}

MBOOL
BokehNode::
onInit()
{
    SCOPED_TRACER();
    CaptureFeatureNode::onInit();
    MY_LOGI("(%p) uniqueKey:%d, sensorIndex:%d, sensorIndex2:%d", this, mUsageHint.mPluginUniqueKey, mSensorIndex, mSensorIndex2);

    mPluginPtr = Plugin::getInstance(mUsageHint.mPluginUniqueKey, {mSensorIndex, mSensorIndex2});
    auto& providers = mPluginPtr->getProviders();
    auto interface = mPluginPtr->getInterface();

    const MUINT64 supportedScenarioFeatures = mUsageHint.mSupportedScenarioFeatures;
    for ( auto& provider : providers )
    {
        const Property& rProperty =  provider->property();
        if( !(supportedScenarioFeatures & rProperty.mFeatures) )
        {
            MY_LOGD("not in supportedScenarioFeatures(%#012" PRIx64 "), skip the provider, name:%s, feature:%#012" PRIx64,
                supportedScenarioFeatures, rProperty.mName, rProperty.mFeatures);
            continue;
        }

        FeatureID_T featureId = NULL_FEATURE;
        if ( rProperty.mFeatures & MTK_FEATURE_BOKEH )
        {
            featureId = FID_BOKEH;
        }
        else if ( rProperty.mFeatures & TP_FEATURE_BOKEH )
        {
            featureId = FID_BOKEH_3RD_PARTY;
        }
        else
        {
            MY_LOGW("can't match the supported features, skip the provider, name:%s, feature:%#012" PRIx64,
                rProperty.mName, rProperty.mFeatures);
            continue;
        }

        SelectionPtr selectionPtr = mPluginPtr->createSelection();
        Selection& rSelection = *selectionPtr.get();
        interface->offer(rSelection);
        provider->negotiate(rSelection);
        //
        std::launch initPolicy = std::launch::deferred;
        if(rProperty.mInitPhase == ePhase_OnPipeInit)
        {
            initPolicy = std::launch::async;
        }
        //
        ProviderInfo providerInfo =
        {
            .mProviderPtr  = provider,
            .mSelectionPtr = selectionPtr,
            .mInitJob = std::async(initPolicy, [provider, initPolicy]()
            {
                const Property& rProperty = provider->property();
                AUTO_TIMER("init provider, name:%s, features:%#012" PRIx64 ", initPolicy:%d", rProperty.mName, rProperty.mFeatures, initPolicy);
                provider->init();
            })
        };
        mProviderInfoTable.insert(FeaturePairItem(featureId, std::move(providerInfo)));
        MY_LOGD("insert item table, featureId:%d(%s), name:%s, features:%#012" PRIx64 ", initPhase:%d, initPolicy:%d",
                featureId, FeatID2Name(featureId),
                rProperty.mName, rProperty.mFeatures,
                rProperty.mInitPhase, static_cast<MINT32>(initPolicy));
    }
    return MTRUE;
}

MBOOL
BokehNode::
onUninit()
{
    SCOPED_TRACER();
    MY_LOGI("(%p) uniqueKey:%d, sensorIndex:%d, sensorIndex2:%d", this, mUsageHint.mPluginUniqueKey, mSensorIndex, mSensorIndex2);

    for ( auto& pair : mProviderInfoTable )
    {
        const FeatureID_T featureId = pair.first;
        ProviderInfo& item = pair.second;
        // TODO: we need the supportScenarioFeatures to advoid init non-used provider
        if ( item.mInitJob.valid() )
        {
            const Property& rProperty = item.mProviderPtr->property();
            MY_LOGD("wait for init job completion, featureId:%d(%s), name:%s, features:%#012" PRIx64 ", initPhase:%d",
                featureId, FeatID2Name(featureId), rProperty.mName, rProperty.mFeatures, rProperty.mInitPhase);
            item.mInitJob.get();
        }
        //
        auto& providerPtr = item.mProviderPtr;
        providerPtr = nullptr;
    }
    mProviderInfoTable.clear();

    return MTRUE;
}

MBOOL
BokehNode::
onThreadStart()
{
    return MTRUE;
}

MBOOL
BokehNode::
onThreadStop()
{
    return MTRUE;
}

MBOOL
BokehNode::
onThreadLoop()
{
    SCOPED_TRACER();

    if ( !waitAllQueue() )
    {
        return MFALSE;
    }

    RequestPtr pipeRequestPtr;
    if ( !mRequests.deque(pipeRequestPtr) )
    {
        MY_LOGE("failed to deque");
        return MFALSE;
    }

    if ( pipeRequestPtr == nullptr )
    {
        MY_LOGE("pipeRequestPtr is nullptr");
        return MFALSE;
    }
    //
    const MINT32 reqNum = pipeRequestPtr->getRequestNo();
    //
    auto foundItem = mProviderInfoTable.end();
    MBOOL isFlowByPass = BokehNodeUtility::getIsForceByPassFlow();
    {
        if ( !isFlowByPass )
        {
            MINT32 featureWarning = 0;
            BokehNodeUtility::tryGetFeatureWarning(pipeRequestPtr, featureWarning);
            if ( featureWarning != 0 )
            {
                MY_LOGD("feature warning occur, req#:%d, featureWarning:%d", reqNum, featureWarning);
                isFlowByPass = MTRUE;
            }
        }
        //
        if ( !isFlowByPass )
        {
            auto item = mProviderInfoTable.begin();
            while( item != mProviderInfoTable.end() )
            {
                if( pipeRequestPtr->hasFeature(item->first) )
                {
                    break;
                }
                ++item;
            }
            //
            if ( item != mProviderInfoTable.end() )
            {
                foundItem = item;
            }
            else
            {
                MY_LOGD("failed to find provider, req#:%d", reqNum);
                isFlowByPass = MTRUE;
            }
        }
    }
    //
    if ( isFlowByPass )
    {
        MY_LOGD("something happen and by pass flow, req#:%d", reqNum);
        // copy from inputFSImgBuf to outputFSImgBuf
        NodeRequestPtr nodeRequestPtr = pipeRequestPtr->getNodeRequest(getNodeID());
        const BufferID_T inputFSBufId = nodeRequestPtr->mapBufferID(TID_MAN_FULL_YUV, INPUT);
        const BufferID_T outputFSBufId = nodeRequestPtr->mapBufferID(TID_MAN_FULL_YUV, OUTPUT);
        //
        sp<BufferHandle> inputFSImgBufPtr = pipeRequestPtr->getBuffer(inputFSBufId);
        sp<BufferHandle> outputFSImgBufPtr = pipeRequestPtr->getBuffer(outputFSBufId);
        const MBOOL isReadyToExecuteByPassFlow = (inputFSImgBufPtr.get() != nullptr) && (outputFSImgBufPtr.get() != nullptr);
        if ( !isReadyToExecuteByPassFlow )
        {
            MY_LOGW("failed to parpare by pass flow, req#:%d, inputFSImgBuf:%p, outputFSImgBuf:%p",
                reqNum, inputFSImgBufPtr.get(), outputFSImgBufPtr.get());
        }
        //
        IImageBuffer* pInputFSYuvImgBuf = inputFSImgBufPtr->native();
        IImageBuffer* pOutputFSYuvImgBuf = outputFSImgBufPtr->native();
        if( !BokehNodeUtility::imgCopy(pInputFSYuvImgBuf, pOutputFSYuvImgBuf) )
        {
            MY_LOGW("failed to execute image copy, req#:%d", reqNum);
        }
        pOutputFSYuvImgBuf->syncCache(eCACHECTRL_FLUSH);
        //
        dispatch(pipeRequestPtr);
    }
    else
    {
        const FeatureID_T featureId = foundItem->first;
        ProviderPtr& providerPtr = foundItem->second.mProviderPtr;
        const Property& rProperty = providerPtr->property();
        //
        InitJob& initJob = foundItem->second.mInitJob;
        if ( initJob.valid() )
        {
            MY_LOGD("wait for init job completion, req#:%d, featureId:%d(%s), name:%s, features:%#012" PRIx64 ", initPhase:%d",
                reqNum, featureId, FeatID2Name(featureId), rProperty.mName, rProperty.mFeatures, rProperty.mInitPhase);
            initJob.get();
        }
        MY_LOGD("execute provider, req#:%d, featureId:%d(%s), name:%s, features:%#012" PRIx64,
            reqNum, featureId, FeatID2Name(featureId), rProperty.mName, rProperty.mFeatures);
        //
        process(providerPtr, pipeRequestPtr);
    }
    return MTRUE;
}

MVOID
BokehNode::
process(ProviderPtr& providerPtr, RequestPtr& pipeRequestPtr)
{
    const MINT32 reqNum = pipeRequestPtr->getRequestNo();
    AUTO_PROFILER("process bokeh request, req#:%d", reqNum);
    AUTO_TIMER("profile process, req#:%d", reqNum);
    //
    auto pluginRequestPtr = BokehNodeUtility::createPluginRequestPtr(getNodeID(), mPluginPtr, pipeRequestPtr);
    {
        std::lock_guard<std::mutex> guard(mRequestPairLock);
        mRequestPairTable.insert(RequestPairItem(pluginRequestPtr.get(),
        {
            .mPipeRequest   = pipeRequestPtr,
            .mPluginRequest = pluginRequestPtr
        }));
        // mark on-going-request start
        this->incExtThreadDependency();
        MY_LOGD("(%p) insert requestPair into table, req#:%d, pluginRequest:%p, dependencyCount:%d",
            this, reqNum, pluginRequestPtr.get(), this->getExtThreadDependency());
    }
    //
    if ( getIsDumpImage<NID_BOKEH>() )
    {
        pipeRequestPtr->dump("inMainFSYuv", getNodeID(), TID_MAN_FULL_YUV, INPUT);
        pipeRequestPtr->dump("depthmap", getNodeID(), TID_MAN_DEPTH, INPUT);
    }
    providerPtr->process(pluginRequestPtr, mRequestCallbackPtr);
}

MERROR
BokehNode::
evaluate(NodeID_T nodeId, CaptureFeatureInferenceData& rInfer)
{
    SCOPED_TRACER();

    UNREFERENCED_PARAMETER(nodeId);

    const MINT8 reqtIndex = rInfer.getRequestIndex();
    const MUINT8 reqCount = rInfer.getRequestCount();
    const MBOOL isMainFrame = rInfer.isMainFrame();

    MY_LOGD("evaluate info, reqtIndex:%u, reqCount:%u, isMainFrame:%d",
        reqtIndex, reqCount, isMainFrame);

    if ( !isMainFrame )
        return OK;

    auto& rSrcData = rInfer.getSharedSrcData();
    auto& rDstData = rInfer.getSharedDstData();
    auto& rFeatures = rInfer.getSharedFeatures();
    auto& rMetadatas = rInfer.getSharedMetadatas();

    MBOOL isEvaluated = MFALSE;
    MERROR status = OK;
    for ( auto& item : mProviderInfoTable )
    {
        const FeatureID_T featureId = item.first;
        if ( !rInfer.hasFeature(featureId) )
        {
            continue;
        }
        else if ( isEvaluated )
        {
            MY_LOGE("duplicated feature:%d(%s)", featureId, FeatID2Name(featureId));
            continue;
        }

        const Selection& rSel = *(item.second.mSelectionPtr);
        MBOOL isOK = MTRUE;

        if ( isOK && rSel.mIBufferFull.getRequired() )
        {
            const TypeID_T typeId = TID_MAN_FULL_YUV;
            const MBOOL hasType = rInfer.hasType(typeId);
            const MBOOL isValid = rSel.mIBufferFull.isValid();

            isOK = (isValid && hasType);
            if ( isOK )
            {
                auto& srcTemp = rSrcData.editItemAt(rSrcData.add());
                srcTemp.mTypeId = typeId;
                srcTemp.mSizeId = rSel.mIBufferFull.getSizes()[0];
                srcTemp.addSupportFormats(rSel.mIBufferFull.getFormats());
            }
            else
            {
                MY_LOGW("failed to add src, hasType:%d, typeId:%d(%s), featureId:%d(%s)",
                    hasType, typeId, TypeID2Name(typeId), featureId, FeatID2Name(featureId));
            }
        }

        if ( isOK && rSel.mIBufferDepth.getRequired() )
        {
            const TypeID_T typeId = TID_MAN_DEPTH;
            const MBOOL hasType = rInfer.hasType(typeId);
            const MBOOL isValid = rSel.mIBufferDepth.isValid();

            isOK = (isValid && hasType);
            if ( isOK )
            {
                auto& srcTemp = rSrcData.editItemAt(rSrcData.add());
                srcTemp.mTypeId = typeId;
                srcTemp.mSizeId = rSel.mIBufferDepth.getSizes()[0];
                srcTemp.mSize = rInfer.getSize(typeId);
                srcTemp.addSupportFormats(rSel.mIBufferDepth.getFormats());
            }
            else
            {
                MY_LOGW("failed to add src, hasType:%d, typeId:%d(%s), featureId:%d(%s)",
                    hasType, typeId, TypeID2Name(typeId), featureId, FeatID2Name(featureId));
            }
        }

        const Plugin::Property& rProperty =  item.second.mProviderPtr->property();
        if ( rProperty.mFaceData == eFD_Current )
        {
            auto& srcTemp = rSrcData.editItemAt(rSrcData.add());
            srcTemp.mTypeId = TID_MAN_FD;
            srcTemp.mSizeId = NULL_SIZE;
            rInfer.markFaceData(eFD_Current);
        }
        else if ( rProperty.mFaceData == eFD_Cache )
        {
            rInfer.markFaceData(eFD_Cache);
        }
        else if ( rProperty.mFaceData == eFD_None )
        {
            rInfer.markFaceData(eFD_None);
        }
        else
        {
            MY_LOGW("unknow faceDateType:%#09x", rInfer.mFaceDateType.value);
        }
        //
        if ( rProperty.mBoost != 0 )
        {
            MY_LOGD("append boostType, value:%#09x", rProperty.mBoost);
            rInfer.appendBootType(rProperty.mBoost);
        }
        //
        if ( isOK && rSel.mOBufferFull.getRequired() )
        {
            const TypeID_T typeId = TID_MAN_FULL_YUV;
            isOK = rSel.mOBufferFull.isValid();
            if ( isOK )
            {
                auto& dstTemp = rDstData.editItemAt(rDstData.add());
                dstTemp.mTypeId = typeId;
                dstTemp.mSizeId = rSel.mOBufferFull.getSizes()[0];
                dstTemp.mSize = rInfer.getSize(typeId);
                dstTemp.addSupportFormats(rSel.mOBufferFull.getFormats());
            }
            else
            {
                MY_LOGW("failed to add dst, typeId:%d(%s), featureId:%d(%s)",
                    typeId, TypeID2Name(typeId), featureId, FeatID2Name(featureId));
            }
        }
        //
        if ( isOK )
        {
            rInfer.markThumbnailTiming(rProperty.mThumbnailTiming);
        }
        //
        if ( isOK && rSel.mOBufferThumbnail.getRequired() )
        {
            auto& dstTemp = rDstData.editItemAt(rDstData.add());
            dstTemp.mTypeId = TID_POSTVIEW;
        }
        //
        if ( isOK )
        {
            if (rSel.mIMetadataDynamic.getRequired())
                rMetadatas.push_back(MID_MAN_IN_P1_DYNAMIC);
            if (rSel.mIMetadataApp.getRequired())
                rMetadatas.push_back(MID_MAN_IN_APP);
            if (rSel.mIMetadataHal.getRequired())
                rMetadatas.push_back(MID_MAN_IN_HAL);
            if (rSel.mOMetadataApp.getRequired())
                rMetadatas.push_back(MID_MAN_OUT_APP);
            if (rSel.mOMetadataHal.getRequired())
                rMetadatas.push_back(MID_MAN_OUT_HAL);
        }
        if ( isOK )
        {
            isEvaluated = MTRUE;
            rFeatures.push_back(featureId);
            if(!rInfer.addNodeIO(NID_BOKEH, rSrcData, rDstData, rMetadatas, rFeatures))
            {
                status = BAD_VALUE;
            }
            else
            {
                MY_LOGD("add feature, featureId:%d(%s)", featureId, FeatID2Name(featureId));
            }
        }
    }
    return status;
}

MVOID
BokehNode::
dispatch(const RequestPtr& pRequest, NodeID_T nodeId)
{
    BokehNodeUtility::processNextCapture(pRequest, NSPipelinePlugin::eTiming_Bokeh);
    //
    CaptureFeatureNode::dispatch(pRequest);
}

void
BokehNode::
onAborted(PluginRequestPtr pluginRequestPtr)
{
    MY_LOGD("aborted requestAddr:%p", pluginRequestPtr.get());
}

void
BokehNode::
onCompleted(PluginRequestPtr pluginRequestPtr, MERROR status)
{
    MY_LOGD("completed pluginRequest, addr:%p, status:%d", pluginRequestPtr.get(), status);

    const auto key = pluginRequestPtr.get();
    RequestPtr pipeRequestPtr = nullptr;
    {
        std::lock_guard<std::mutex> guard(mRequestPairLock);

        auto foundItem = mRequestPairTable.find(key);
        if ( foundItem != mRequestPairTable.end() )
        {
            pipeRequestPtr = foundItem->second.mPipeRequest;
            mRequestPairTable.erase(foundItem);
            MY_LOGI("(%p) remove requestPair from table, req#:%d, pluginRequest:%p, RequestPairTableCount:%zu",
                this, pipeRequestPtr->getRequestNo(), pluginRequestPtr.get(), mRequestPairTable.size());
        }
        *pluginRequestPtr = Plugin::Request();
    }

    if ( pipeRequestPtr == nullptr )
    {
        MY_LOGE("failed to find pipeRequest from pluginRequest, addr:%p", key);
        return;
    }
    //
    if ( getIsDumpImage<NID_BOKEH>() )
    {
        pipeRequestPtr->dump("outMainFullYuv", getNodeID(), TID_MAN_FULL_YUV, OUTPUT);
    }
    //
    dispatch(pipeRequestPtr);
    //
    this->decExtThreadDependency();
    MY_LOGD("finish request, req#:%d, dependence:%d",
        pipeRequestPtr->getRequestNo(), this->getExtThreadDependency());
}

void
BokehNode::
onNextCapture(PluginRequestPtr pluginRequestPtr)
{
    MY_LOGD("no support next capture, requestAddr:%p", pluginRequestPtr.get());
}

} // NSCapture
} // namespace NSFeaturePipe
} // namespace NSCamFeature
} // namespace NSCam
