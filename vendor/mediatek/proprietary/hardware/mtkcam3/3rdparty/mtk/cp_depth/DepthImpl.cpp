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

#define LOG_TAG "PipelinePlugin/TKDepthProvider"

// Standard C header file
#include <stdlib.h>
#include <chrono>
#include <random>
#include <thread>
// Android system/core header file

// mtkcam custom header file

// mtkcam global header file
#include <mtkcam/utils/std/Log.h>
#include <mtkcam/utils/std/ULog.h>
// Module header file
#include <mtkcam/drv/iopipe/SImager/IImageTransform.h>
#include <mtkcam/utils/metastore/IMetadataProvider.h>
#include <mtkcam3/3rdparty/plugin/PipelinePlugin.h>
#include <mtkcam3/3rdparty/plugin/PipelinePluginType.h>
//
#include <mtkcam/utils/metadata/client/mtk_metadata_tag.h>
#include <mtkcam/utils/metadata/hal/mtk_platform_metadata_tag.h>
//
#include <mtkcam3/feature/stereo/hal/stereo_setting_provider.h>
// Local header file


/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_ULOGMV("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_ULOGMD("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_ULOGMI("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_ULOGMW("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_ULOGME("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)

CAM_ULOG_DECLARE_MODULE_ID(MOD_LIB_DEPTH);
/*******************************************************************************
* MACRO Utilities Define.
********************************************************************************/
namespace { // anonymous namespace for debug MARCO function
using AutoObject = std::unique_ptr<const char, std::function<void(const char*)>>;
//
auto
createAutoScoper(const char* funcName) -> AutoObject
{
    CAM_LOGD("[%s] +", funcName);
    return AutoObject(funcName, [](const char* p)
    {
        CAM_LOGD("[%s] -", p);
    });
}
#define SCOPED_TRACER() auto scoped_tracer = ::createAutoScoper(__FUNCTION__)
//
auto
createAutoTimer(const char* funcName, const char* text) -> AutoObject
{
    using Timing = std::chrono::time_point<std::chrono::high_resolution_clock>;
    using DuationTime = std::chrono::duration<float, std::milli>;

    Timing startTime = std::chrono::high_resolution_clock::now();
    return AutoObject(text, [funcName, startTime](const char* p)
    {
        Timing endTime = std::chrono::high_resolution_clock::now();
        DuationTime duationTime = endTime - startTime;
        CAM_LOGD("[%s] %s, elapsed(ms):%.4f",funcName, p, duationTime.count());
    });
}
#define AUTO_TIMER(TEXT) auto auto_timer = ::createAutoTimer(__FUNCTION__, TEXT)
//
#define UNREFERENCED_PARAMETER(a) (a)
//
} // end anonymous namespace for debug MARCO function


/*******************************************************************************
* Used namespace.
********************************************************************************/
using namespace std;
//
using namespace android;
//
using namespace NSCam;
using namespace NSCam::NSPipelinePlugin;
using namespace NSCam::NSIoPipe::NSSImager;


/*******************************************************************************
* Type Alias.
********************************************************************************/
using Property = DepthPlugin::Property;
using Selection = DepthPlugin::Selection;
using RequestPtr = DepthPlugin::Request::Ptr;
using RequestCallbackPtr = DepthPlugin::RequestCallback::Ptr;
//
template<typename T>
using AutoPtr             = std::unique_ptr<T, std::function<void(T*)>>;
//
using ImgPtr              = AutoPtr<IImageBuffer>;
using MetaPtr             = AutoPtr<IMetadata>;
using ImageTransformPtr   = AutoPtr<IImageTransform>;


/*******************************************************************************
* Namespace Start.
********************************************************************************/
namespace { // anonymous namespace


/*******************************************************************************
* Class Definition
********************************************************************************/
/**
 * @brief MTK turkey depth algo. provider
 */
class MTKTurnkeyDepthProvider final: public DepthPlugin::IProvider
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Instantiation.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    MTKTurnkeyDepthProvider();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  DepthPlugin::IProvider Public Operations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    void set(MINT32 iOpenId, MINT32 iOpenId2) override;

    const Property& property() override;

    MERROR negotiate(Selection& sel) override;

    void init() override;

    MERROR process(RequestPtr requestPtr, RequestCallbackPtr callbackPtr) override;

    void abort(vector<RequestPtr>& requestPtrs) override;

    void uninit() override;

    ~MTKTurnkeyDepthProvider();
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  MTKTurnkeyDepthProvider Private Operator.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
private:
    MERROR processDone(const RequestPtr& requestPtr, const RequestCallbackPtr& callbackPtr, MERROR status);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  MTKTurnkeyDepthProvider Private Data Members.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
private:
    struct FlowControl
    {
        MINT8  mEnable;
        MBOOL  mIsNeedCurrentFD;
        MBOOL  mIsSupportThumbnail;
        MBOOL  mIsSupportResizeBuf;
        MUINT8 mInitPhase;

        FlowControl();
    };

private:
    FlowControl mFlowControl;
    //
    MINT32 mOpenId;
    MINT32 mOpenId2;
};
REGISTER_PLUGIN_PROVIDER(Depth, MTKTurnkeyDepthProvider);
/**
 * @brief utility class
 */
class TKDepthUtility final
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Instantiation.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    TKDepthUtility() = delete;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  TKDepthUtility Public Operations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    static inline ImgPtr createImgPtr(BufferHandle::Ptr& handle);

    static inline MetaPtr createMetaPtr(MetadataHandle::Ptr& handle);

    static inline ImageTransformPtr createImageTransformPtr();

    static inline MVOID dump(const IImageBuffer* pImgBuf, const std::string& dumpName);

    static inline MVOID dump(IMetadata* pMetaData, const std::string& dumpName);
};


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  TKDepthUtility implementation.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ImgPtr
TKDepthUtility::
createImgPtr(BufferHandle::Ptr& handle)
{
    return ImgPtr( ((handle == nullptr) ? nullptr : handle->acquire()), [handle](IImageBuffer* p)
    {
        UNREFERENCED_PARAMETER(p);
        handle->release();
    });
};

MetaPtr
TKDepthUtility::
createMetaPtr(MetadataHandle::Ptr& handle)
{
    return MetaPtr(handle->acquire(), [handle](IMetadata* p)
    {
        UNREFERENCED_PARAMETER(p);
        handle->release();
    });
};

ImageTransformPtr
TKDepthUtility::
createImageTransformPtr()
{
    return ImageTransformPtr(IImageTransform::createInstance(), [](IImageTransform *p)
    {
        p->destroyInstance();
    });
}

MVOID
TKDepthUtility::
dump(const IImageBuffer* pImgBuf, const std::string& dumpName)
{
    if (pImgBuf == nullptr)
        return;

    MY_LOGD("dump image info, dumpName:%s, info:[a:%p, si:%dx%d, st:%zu, f:0x%x, va:%p]",
        dumpName.c_str(), pImgBuf,
        pImgBuf->getImgSize().w, pImgBuf->getImgSize().h,
        pImgBuf->getBufStridesInBytes(0),
        pImgBuf->getImgFormat(),
        reinterpret_cast<void*>(pImgBuf->getBufVA(0)));
}

MVOID
TKDepthUtility::
dump(IMetadata* pMetaData, const std::string& dumpName)
{
    if (pMetaData == nullptr)
        return;

    MY_LOGD("dump meta info, dumpName:%s, info:[a:%p, c:%u]",
        dumpName.c_str(), pMetaData, pMetaData->count());
    pMetaData->dump();
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  MTKTurnkeyDepthProvider implementation.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MTKTurnkeyDepthProvider::FlowControl::
FlowControl()
: mEnable(-1)
, mIsNeedCurrentFD(MFALSE)
, mIsSupportThumbnail(MFALSE)
, mIsSupportResizeBuf(MFALSE)
, mInitPhase(ePhase_OnPipeInit)
{
    // on:1/off:0/auto:-1
    mEnable = ::property_get_int32("vendor.debug.camera.tk.depth.enable", mEnable);
    mIsNeedCurrentFD = ::property_get_bool("vendor.debug.camera.tk.depth.currentfd", mIsNeedCurrentFD);
    mIsSupportThumbnail = ::property_get_bool("vendor.debug.camera.tk.depth.thumbnail", mIsSupportThumbnail);
    mIsSupportResizeBuf = ::property_get_bool("vendor.debug.camera.tk.depth.resizebuf", mIsSupportThumbnail);
    mInitPhase = ::property_get_int32("vendor.debug.camera.tk.depth.initphase", mInitPhase);
    MY_LOGD("enable:%d, needCurrentFD:%d, supportThumbnail:%d, initPhase:%u",
        mEnable, mIsNeedCurrentFD, mIsSupportThumbnail, mInitPhase);
}

MTKTurnkeyDepthProvider::
MTKTurnkeyDepthProvider()
: mOpenId(-1)
, mOpenId2(-1)
{
    MY_LOGD("ctor:%p", this);
}

void
MTKTurnkeyDepthProvider::
set(MINT32 iOpenId, MINT32 iOpenId2)
{
    mOpenId = iOpenId;
    mOpenId2 = iOpenId2;
    MY_LOGD("set openId:%d openId2:%d", mOpenId, mOpenId2);
}

const Property&
MTKTurnkeyDepthProvider::
property()
{
    static const Property prop = [this]() -> const Property
    {
        Property ret;
        ret.mName = "MTK_DEPTH";
        ret.mFeatures = MTK_FEATURE_DEPTH;
        ret.mThumbnailTiming = (mFlowControl.mIsSupportThumbnail ? eTiming_Depth : eTiming_P2);
        ret.mFaceData = (mFlowControl.mIsNeedCurrentFD ? eFD_Current : eFD_Cache);
        ret.mPreprocess = ePreProcess_None;
        ret.mBoost = eBoost_CPU;
        ret.mInitPhase = (mFlowControl.mInitPhase == ePhase_OnPipeInit ? ePhase_OnPipeInit : ePhase_OnRequest);
        return ret;
    }();
    return prop;
}

MERROR
MTKTurnkeyDepthProvider::
negotiate(Selection& sel)
{
    SCOPED_TRACER();

    if( mFlowControl.mEnable == 0 )
    {
        MY_LOGD("force off tk depth");
        return BAD_VALUE;
    }
    // INPUT
    {
        sel.mIBufferFull
            .setRequired(MTRUE)
            .addAcceptedFormat(eImgFmt_NV21)
            .addAcceptedFormat(eImgFmt_NV12)
            .addAcceptedFormat(eImgFmt_I422)
            .addAcceptedSize(eImgSize_Full);

        sel.mIBufferFull2
            .setRequired(MTRUE)
            .addAcceptedFormat(eImgFmt_NV21)
            .addAcceptedFormat(eImgFmt_NV12)
            .addAcceptedFormat(eImgFmt_I422)
            .addAcceptedSize(eImgSize_Full);

        if (mFlowControl.mIsSupportResizeBuf)
        {
            sel.mIBufferResized
                .setRequired(MTRUE)
                .addAcceptedFormat(eImgFmt_NV21)
                .addAcceptedSize(eImgSize_Resized);

            sel.mIBufferResized2
                .setRequired(MTRUE)
                .addAcceptedFormat(eImgFmt_NV21)
                .addAcceptedSize(eImgSize_Resized);

            sel.mIBufferLCS
                .setRequired(MTRUE);

            sel.mIBufferLCS2
                .setRequired(MTRUE);
        }

        sel.mIMetadataApp.setRequired(MTRUE);
        sel.mIMetadataHal.setRequired(MTRUE);
        sel.mIMetadataHal2.setRequired(MTRUE);
        sel.mIMetadataDynamic.setRequired(MTRUE);
        sel.mIMetadataDynamic2.setRequired(MTRUE);
    }
    // OUTPUT
    {
        // NOTE: the depth map size is customized
        MTK_DEPTHMAP_INFO_T depthInfo = StereoSettingProvider::getDepthmapInfo();
        sel.mOBufferDepth.setRequired(MTRUE)
            .addAcceptedFormat(depthInfo.format)
            .addAcceptedSize(eImgSize_Specified)
            .setSpecifiedSize(depthInfo.size);

        if ( mFlowControl.mIsSupportThumbnail )
        {
            sel.mOBufferThumbnail
                .setRequired(MTRUE);
        }

        sel.mOMetadataApp.setRequired(MTRUE);
        sel.mOMetadataHal.setRequired(MTRUE);
    }
    return OK;
}

void
MTKTurnkeyDepthProvider::
init()
{
    SCOPED_TRACER();
    ::srand(time(nullptr));
}

MERROR
MTKTurnkeyDepthProvider::
process(RequestPtr requestPtr, RequestCallbackPtr callbackPtr)
{
    SCOPED_TRACER();

    auto isValidInput = [this](const RequestPtr& requestPtr) -> MBOOL
    {
        const MBOOL isImgBufferReady = requestPtr->mIBufferFull != nullptr
                                        && requestPtr->mIBufferFull2 != nullptr
                                        && (mFlowControl.mIsSupportResizeBuf ? (requestPtr->mIBufferResized != nullptr) : MTRUE)
                                        && (mFlowControl.mIsSupportResizeBuf ? (requestPtr->mIBufferResized2 != nullptr) : MTRUE)
                                        && (mFlowControl.mIsSupportResizeBuf ? (requestPtr->mIBufferLCS != nullptr) : MTRUE)
                                        && (mFlowControl.mIsSupportResizeBuf ? (requestPtr->mIBufferLCS2 != nullptr) : MTRUE);

        const MBOOL isMetaBufferReady = requestPtr->mIMetadataApp != nullptr
                                            && requestPtr->mIMetadataHal != nullptr
                                            && requestPtr->mIMetadataHal2 != nullptr;

        if( !isImgBufferReady )
        {
            MY_LOGE("invalid request with input imgBuffer:[ra:%p, ifsi:%p, ifsi2:%p, irsi:%p, irsi2:%p, ilcs1:%p, ilcs2:%p]",
                requestPtr.get(),
                requestPtr->mIBufferFull.get(),
                requestPtr->mIBufferFull2.get(),
                requestPtr->mIBufferResized.get(),
                requestPtr->mIBufferResized2.get(),
                requestPtr->mIBufferLCS.get(),
                requestPtr->mIBufferLCS2.get());
        }

        if( !isMetaBufferReady )
        {
            MY_LOGE("invalid request with input metaBuffer:[ra:%p, iam:%p, ihm:%p, ihm2:%p]",
                requestPtr.get(),
                requestPtr->mIMetadataApp.get(),
                requestPtr->mIMetadataHal.get(),
                requestPtr->mIMetadataHal2.get());
        }
        return (isImgBufferReady && isMetaBufferReady);
    };

    auto isValidOutput = [this](const RequestPtr& requestPtr) -> MBOOL
    {
        const MBOOL ret = requestPtr->mOBufferDepth != nullptr
                        && (mFlowControl.mIsSupportThumbnail ? (requestPtr->mOBufferThumbnail != nullptr) : MTRUE)
                        && requestPtr->mOMetadataApp != nullptr
                        && requestPtr->mOMetadataHal != nullptr;

        if( !ret )
        {
            MY_LOGE("invalid request with output, info:[ra:%p, odm:%p, otn:%p, oam:%p, ohm:%p]",
                requestPtr.get(),
                requestPtr->mOBufferDepth.get(),
                requestPtr->mOBufferThumbnail.get(),
                requestPtr->mOMetadataApp.get(),
                requestPtr->mOMetadataHal.get());
        }
        return ret;
    };

    MY_LOGD("process, reqAddr:%p", requestPtr.get());

    if( !isValidInput(requestPtr) )
    {
        return processDone(requestPtr, callbackPtr, BAD_VALUE);
    }

    if( !isValidOutput(requestPtr) )
    {
        return processDone(requestPtr, callbackPtr, BAD_VALUE);
    }
    //
    {
        // note: we can just call createXXXXPtr one time for a specified handle
        ImgPtr inFSMainImgPtr = TKDepthUtility::createImgPtr(requestPtr->mIBufferFull);
        ImgPtr inFSSubImgPtr = TKDepthUtility::createImgPtr(requestPtr->mIBufferFull2);
        ImgPtr inRSMainImgPtr = TKDepthUtility::createImgPtr(requestPtr->mIBufferResized);
        ImgPtr inRSSubImgPtr = TKDepthUtility::createImgPtr(requestPtr->mIBufferResized2);
        ImgPtr inLCSMainImgPtr = TKDepthUtility::createImgPtr(requestPtr->mIBufferLCS);
        ImgPtr inLCSSubImgPtr = TKDepthUtility::createImgPtr(requestPtr->mIBufferLCS2);
        ImgPtr outDepthImgPtr = TKDepthUtility::createImgPtr(requestPtr->mOBufferDepth);
        //
        MetaPtr inAppMetaPtr = TKDepthUtility::createMetaPtr(requestPtr->mIMetadataApp);
        MetaPtr inMainHalMetaPtr = TKDepthUtility::createMetaPtr(requestPtr->mIMetadataHal);
        MetaPtr inSubHalMetaPtr = TKDepthUtility::createMetaPtr(requestPtr->mIMetadataHal2);
        MetaPtr outAppMetaPtr = TKDepthUtility::createMetaPtr(requestPtr->mOMetadataApp);
        MetaPtr outHalMetaPtr = TKDepthUtility::createMetaPtr(requestPtr->mOMetadataHal);
        // dump info
        {
            TKDepthUtility::dump(inFSMainImgPtr.get(), "inputFSMainImg");
            TKDepthUtility::dump(inFSSubImgPtr.get(), "inputFSSubImg");
            TKDepthUtility::dump(inRSMainImgPtr.get(), "inputRSMainImg");
            TKDepthUtility::dump(inRSSubImgPtr.get(), "inputRSSubImg");
            TKDepthUtility::dump(inLCSMainImgPtr.get(), "inputLCSMainImg");
            TKDepthUtility::dump(inLCSSubImgPtr.get(), "inputLCSSubImg");
            TKDepthUtility::dump(outDepthImgPtr.get(), "outputDepthImg");
            //
            TKDepthUtility::dump(inAppMetaPtr.get(), "inAppMeta");
            TKDepthUtility::dump(inMainHalMetaPtr.get(), "inMainHalMeta");
            TKDepthUtility::dump(inSubHalMetaPtr.get(), "inSubHalMeta");
            TKDepthUtility::dump(outAppMetaPtr.get(), "outAppMeta");
            TKDepthUtility::dump(outHalMetaPtr.get(), "outHalMeta");
        }
        // process depth
        {
            AUTO_TIMER("process depth by copy");

            const size_t depthBufStrides = outDepthImgPtr->getBufStridesInBytes(0);
            const MSize& depthBufSize = outDepthImgPtr->getImgSize();
            const size_t maxValue = 256;
            const MFLOAT increaseFactor = maxValue/static_cast<MFLOAT>(depthBufSize.h);

            char* pDepthBufVA = reinterpret_cast<char*>(outDepthImgPtr->getBufVA(0));
            for(MINT32 i = 0; i < depthBufSize.h; i++)
            {
                const char value = i*increaseFactor;
                ::memset(pDepthBufVA, value, depthBufStrides);
                pDepthBufVA += depthBufStrides;
            }
            outDepthImgPtr->syncCache(eCACHECTRL_FLUSH);
        }
        // process thumbnail
        {
            if ( mFlowControl.mIsSupportThumbnail )
            {
                ImageTransformPtr transformPtr = TKDepthUtility::createImageTransformPtr();
                const MRect crop = MRect(inFSMainImgPtr->getImgSize().w, inFSMainImgPtr->getImgSize().h);
                ImgPtr outThumbnailImgPtr = TKDepthUtility::createImgPtr(requestPtr->mOBufferThumbnail);
                if( (transformPtr.get() == nullptr) || (!transformPtr->execute( inFSMainImgPtr.get(), outThumbnailImgPtr.get(), nullptr, crop, 0, 3000)) )
                {
                    MY_LOGE("failed to generate output thumbnail, transformPtr:%p", transformPtr.get());
                    return processDone(requestPtr, callbackPtr, BAD_VALUE);
                }
                outThumbnailImgPtr->syncCache(eCACHECTRL_FLUSH);
            }
        }
    }
    //
    {
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(0, 500);
        const int randomTime = distribution(generator);
        const int baseTime = 1000;
        auto sleepTime = std::chrono::microseconds(baseTime + randomTime);
        MY_LOGD("sleep for %lld ms", sleepTime.count());
        {
            std::this_thread::sleep_for(sleepTime);
        }
        MY_LOGD("wait up");
    }
    return processDone(requestPtr, callbackPtr, OK);
}

MERROR
MTKTurnkeyDepthProvider::
processDone(const RequestPtr& requestPtr, const RequestCallbackPtr& callbackPtr, MERROR status)
{
    SCOPED_TRACER();

    MY_LOGD("process done, call complete, reqAddr:%p, callbackPtr:%p, status:%d",
        requestPtr.get(), callbackPtr.get(), status);

    if( callbackPtr != nullptr )
    {
        callbackPtr->onCompleted(requestPtr, status);
    }
    return OK;
}

void
MTKTurnkeyDepthProvider::
abort(vector<RequestPtr>& requestPtrs)
{
    SCOPED_TRACER();

    for(auto& item : requestPtrs)
    {
        MY_LOGD("abort request, reqAddr:%p", item.get());
    }
}

void
MTKTurnkeyDepthProvider::
uninit()
{
    SCOPED_TRACER();
}

MTKTurnkeyDepthProvider::
~MTKTurnkeyDepthProvider()
{
    MY_LOGD("dtor:%p", this);
}


}  // anonymous namespace
