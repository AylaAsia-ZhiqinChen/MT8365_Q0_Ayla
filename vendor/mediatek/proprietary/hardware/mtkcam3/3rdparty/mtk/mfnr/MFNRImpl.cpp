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
#define LOG_TAG "MFNRPlugin"
static const char* __CALLERNAME__ = LOG_TAG;

//
#include <mtkcam/utils/std/ULog.h>
//
#include <stdlib.h>
#include <utils/Errors.h>
#include <utils/List.h>
#include <utils/RefBase.h>
#include <sstream>
#include <unordered_map> // std::unordered_map
//
#include <mtkcam/utils/metadata/client/mtk_metadata_tag.h>
#include <mtkcam/utils/metadata/hal/mtk_platform_metadata_tag.h>
#include <custom/feature/mfnr/camera_custom_mfll.h>
//zHDR
#include <mtkcam/utils/hw/HwInfoHelper.h> // NSCamHw::HwInfoHelper
#include <mtkcam3/feature/utils/FeatureProfileHelper.h> //ProfileParam
#include <mtkcam/drv/IHalSensor.h>
//
#include <mtkcam/utils/imgbuf/IIonImageBufferHeap.h>
//
#include <mtkcam/utils/std/Format.h>
#include <mtkcam/utils/std/Time.h>
//
#include <mtkcam3/pipeline/hwnode/NodeId.h>
//
#include <mtkcam/utils/metastore/IMetadataProvider.h>
#include <mtkcam/utils/metastore/ITemplateRequest.h>
#include <mtkcam/utils/metastore/IMetadataProvider.h>
#include <mtkcam3/3rdparty/plugin/PipelinePlugin.h>
#include <mtkcam3/3rdparty/plugin/PipelinePluginType.h>
#include "MFNRShotInfo.h"
//
#include <isp_tuning/isp_tuning.h>  //EIspProfile_T, EOperMode_*


//

using namespace NSCam;
using namespace plugin;
using namespace android;
using namespace mfll;
using namespace std;
using namespace NSCam::NSPipelinePlugin;
using namespace NSIspTuning;
/******************************************************************************
 *
 ******************************************************************************/
CAM_ULOG_DECLARE_MODULE_ID(MOD_LIB_MFNR);
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
//
#define ASSERT(cond, msg)           do { if (!(cond)) { printf("Failed: %s\n", msg); return; } }while(0)


#define __DEBUG // enable debug

#ifdef __DEBUG
#include <memory>
#define FUNCTION_SCOPE \
auto __scope_logger__ = [](char const* f)->std::shared_ptr<const char>{ \
    CAM_ULOGMD("(%d)[%s] + ", ::gettid(), f); \
    return std::shared_ptr<const char>(f, [](char const* p){CAM_ULOGMD("(%d)[%s] -", ::gettid(), p);}); \
}(__FUNCTION__)
#else
#define FUNCTION_SCOPE
#endif



/******************************************************************************
*
******************************************************************************/
class MFNRProviderImpl : public MultiFramePlugin::IProvider
{
    typedef MultiFramePlugin::Property Property;
    typedef MultiFramePlugin::Selection Selection;
    typedef MultiFramePlugin::Request::Ptr RequestPtr;
    typedef MultiFramePlugin::RequestCallback::Ptr RequestCallbackPtr;

public:

    virtual void set(MINT32 iOpenId, MINT32 iOpenId2)
    {
        mOpenId = iOpenId;
        mSupport10BitOutput = property_get_int32("vendor.debug.p2c.10bits.enable", 1);
        MY_LOGD("set openId:%d openId2:%d support10bitOut:%d", iOpenId, iOpenId2, mSupport10BitOutput);
    }

    virtual const Property& property()
    {
        FUNCTION_SCOPE;
        static Property prop;
        static bool inited;

        if (!inited) {
            prop.mName              = "MTK MFNR";
            prop.mFeatures          = MTK_FEATURE_MFNR;
            prop.mThumbnailTiming   = eTiming_P2;
            prop.mPriority          = ePriority_Highest;
            prop.mZsdBufferMaxNum   = 8; // maximum frames requirement
            prop.mNeedRrzoBuffer    = MTRUE; // rrzo requirement for BSS
            prop.mBoost             = eBoost_CPU;
            inited                  = MTRUE;
        }
        return prop;
    };

    virtual MERROR negotiate(Selection& sel)
    {
        FUNCTION_SCOPE;

        // create MFNRShotInfo
        if (sel.mRequestIndex == 0) {

            mZSDMode = sel.mState.mZslRequest && sel.mState.mZslPoolReady;
            mFlashOn = sel.mState.mFlashFired;
            mRealIso = sel.mState.mRealIso;
            mShutterTime = sel.mState.mExposureTime;

            if (CC_UNLIKELY( mFlashOn && mZSDMode )) {
                MY_LOGD("do not use ZSD buffers due to Flash MFNR");
                mZSDMode = MFALSE;
            }

            setUniqueKey(static_cast<MINT32>(NSCam::Utils::TimeTool::getReadableTime()));

            if (CC_LIKELY(sel.mIMetadataApp.getControl() != NULL)) {
                IMetadata* pAppMeta = sel.mIMetadataApp.getControl().get();
                mMfbMode = updateMfbMode(pAppMeta, mZSDMode);
            } else {
                mMfbMode = MfllMode_NormalMfll;
                MY_LOGW("negotiate without metadata, using mMfbMode = MfllMode_NormalMfll.");
            }

            if (CC_UNLIKELY( mMfbMode == MfllMode_Off )) {
                return BAD_VALUE;
            }

            createShotInfo();
#ifdef __DEBUG
            dumpShotInfo();
#endif

            sel.mDecision.mZslEnabled = isZsdMode(mMfbMode);
            sel.mDecision.mZslPolicy.mPolicy = v3::pipeline::policy::eZslPolicy_AfState
                                             | v3::pipeline::policy::eZslPolicy_ContinuousFrame
                                             | v3::pipeline::policy::eZslPolicy_ZeroShutterDelay;
            sel.mDecision.mZslPolicy.mTimeouts = 1000;

        }

        //get MFNRShotInfo
        std::shared_ptr<MFNRShotInfo> pShotInfo = getShotInfo(getUniqueKey());
        if (CC_UNLIKELY(pShotInfo.get() == nullptr)) {
            removeShotInfo(getUniqueKey());
            MY_LOGE("get MFNRShotInfo instance failed! cannot apply MFNR shot.");
            return BAD_VALUE;
        }

        //Mfll Core Version
        static const auto mfllCoreVersion = MFLL_MAKE_REVISION(MFLL_MAJOR_VER(pShotInfo->getMfnrCoreVersion()), MFLL_MINOR_VER(pShotInfo->getMfnrCoreVersion()), 0);
        //update policy
        if (sel.mRequestIndex == 0) {
            pShotInfo->setSizeSrc(sel.mState.mSensorSize);
            MY_LOGD("set source size = %dx%d", pShotInfo->getSizeSrc().w, pShotInfo->getSizeSrc().h);

            switch (mfllCoreVersion) {
                /**
                 * TODO: new case must check FOV is porting or not first.
                 * case MFLL_MAKE_REVISION(new_isp_version):
                 *   break;
                 */
                case MFLL_MAKE_REVISION(1, 4, 0):
                case MFLL_MAKE_REVISION(2, 1, 0):
                    break;
                case MFLL_MAKE_REVISION(2, 0, 0):
                case MFLL_MAKE_REVISION(2, 5, 0):
                case MFLL_MAKE_REVISION(3, 0, 0):
                    if (CC_LIKELY( !sel.mState.mDualCamDedicatedYuvSize )) {
                        MY_LOGD("Fov is not found");
                    } else {
                        pShotInfo->setSizeSrc(sel.mState.mDualCamDedicatedYuvSize);
                        MY_LOGI("Apply FOV size, SensorSize(%dx%d), Fov(%dx%d), "
                            , sel.mState.mSensorSize.w, sel.mState.mSensorSize.h
                            , sel.mState.mDualCamDedicatedYuvSize.w, sel.mState.mDualCamDedicatedYuvSize.h);
                    }
                    MY_LOGD("Set multicam feature, mode:%d", sel.mState.mMultiCamFeatureMode);
                    pShotInfo->setMulitCamFeatureMode(sel.mState.mMultiCamFeatureMode);
                    break;
                default:
                    removeShotInfo(getUniqueKey());
                    MY_LOGE("MFNR Provider is not support for mfnrcore(%s)", pShotInfo->getMfnrCoreVersionString().c_str());
                    return BAD_VALUE;
            }

            pShotInfo->updateMfllStrategy();

            MY_LOGD("realIso = %d, shutterTime = %d, finalRealIso = %d, finalShutterTime = %d"
                , pShotInfo->getRealIso()
                , pShotInfo->getShutterTime()
                , pShotInfo->getFinalIso()
                , pShotInfo->getFinalShutterTime());
        }

        sel.mRequestCount = pShotInfo->getCaptureNum();

        MY_LOGD("Mfll apply = %d, frames = %d", pShotInfo->getIsEnableMfnr(), sel.mRequestCount);

        if (!pShotInfo->getIsEnableMfnr()
            || sel.mRequestCount == 0
            || !updateInputBufferInfo(mfllCoreVersion, sel)) {
            removeShotInfo(getUniqueKey());
            return BAD_VALUE;
        }

        sel.mIBufferFull.setRequired(MTRUE).setAlignment(mYuvAlign.w, mYuvAlign.h);
        sel.mIBufferSpecified.setRequired(MTRUE).setAlignment(mQYuvAlign.w, mQYuvAlign.h);
        sel.mIMetadataDynamic.setRequired(MTRUE);
        sel.mIMetadataApp.setRequired(MTRUE);
        sel.mIMetadataHal.setRequired(MTRUE);


        // Without control metadata, it's no need to append additional metadata
        // Use default mfnr setting
        if (sel.mIMetadataApp.getControl() != NULL) {
            //per frame
            {
                MetadataPtr pAppAddtional = make_shared<IMetadata>();
                MetadataPtr pHalAddtional = make_shared<IMetadata>();

                IMetadata* pAppMeta = pAppAddtional.get();
                IMetadata* pHalMeta = pHalAddtional.get();

                // update unique key
                IMetadata::setEntry<MINT32>(pHalMeta, MTK_PIPELINE_UNIQUE_KEY, getUniqueKey());
                IMetadata::setEntry<MINT32>(pHalMeta, MTK_FEATURE_MFNR_NVRAM_QUERY_INDEX, pShotInfo->getNvramIndex());
                IMetadata::setEntry<MINT32>(pHalMeta, MTK_FEATURE_MFNR_NVRAM_DECISION_ISO, pShotInfo->getFinalIso());
#if (SUPPORT_YUV_BSS==0)
                /* MTK_FEATURE_BSS_SELECTED_FRAME_COUNT for raw domain bss */
                IMetadata::setEntry<MINT32>(pHalMeta, MTK_FEATURE_BSS_SELECTED_FRAME_COUNT, pShotInfo->getBlendNum());
#endif
                IMetadata::setEntry<MUINT8>(pHalMeta, MTK_ISP_P2_TUNING_UPDATE_MODE, sel.mRequestIndex?2:0);

                updatePerFrameMetadata(pShotInfo.get(), pAppMeta, pHalMeta, (sel.mRequestIndex+1 == sel.mRequestCount));

                sel.mIMetadataApp.setAddtional(pAppAddtional);
                sel.mIMetadataHal.setAddtional(pHalAddtional);
            }

            //dummy frame
            {
                MetadataPtr pAppDummy = make_shared<IMetadata>();
                MetadataPtr pHalDummy = make_shared<IMetadata>();

                IMetadata* pAppMeta = pAppDummy.get();
                IMetadata* pHalMeta = pHalDummy.get();

                //first frame
                if (sel.mRequestIndex == 0) {
                    sel.mFrontDummy = pShotInfo->getDummyFrameNum();
                    if (pShotInfo->getIsFlashOn()) {
                        IMetadata::setEntry<MBOOL>(pHalMeta, MTK_3A_DUMMY_BEFORE_REQUEST_FRAME, 1);
                    }
                    // need pure raw for MFNR flow
                    IMetadata::setEntry<MINT32>(pHalMeta, MTK_P1NODE_RAW_TYPE, 1);
                }

                //last frame
                if (sel.mRequestIndex+1  == sel.mRequestCount) {
                    sel.mPostDummy = pShotInfo->getDelayFrameNum();
                    IMetadata::setEntry<MBOOL>(pHalMeta, MTK_3A_DUMMY_AFTER_REQUEST_FRAME, 1);
                    IMetadata::setEntry<MINT32>(pHalMeta, MTK_P1NODE_RAW_TYPE, 1);
                }

                sel.mIMetadataApp.setDummy(pAppDummy);
                sel.mIMetadataHal.setDummy(pHalDummy);
            }
        }

        if (sel.mRequestIndex == 0) {
            switch (mfllCoreVersion) {
                case MFLL_MAKE_REVISION(1, 4, 0):
                    sel.mOBufferFull
                        .setRequired(MTRUE)
                        .addAcceptedFormat(eImgFmt_YUY2) // YUY2 first
                        .addAcceptedFormat(eImgFmt_NV21)
                        .addAcceptedFormat(eImgFmt_NV12)
                        .addAcceptedFormat(eImgFmt_I420)
                        .addAcceptedSize(eImgSize_Full);
                    break;
                case MFLL_MAKE_REVISION(2, 0, 0):
                    sel.mOBufferFull
                        .setRequired(MTRUE)
                        .addAcceptedFormat(eImgFmt_YUY2) // YUY2 first
                        .addAcceptedFormat(eImgFmt_NV21)
                        .addAcceptedFormat(eImgFmt_NV12)
                        .addAcceptedFormat(eImgFmt_I420)
                        .addAcceptedSize(eImgSize_Full);
                    break;
                case MFLL_MAKE_REVISION(2, 1, 0):
                    sel.mOBufferFull
                        .setRequired(MTRUE)
                        .addAcceptedFormat(eImgFmt_YUY2) // YUY2 first
                        .addAcceptedFormat(eImgFmt_NV21)
                        .addAcceptedFormat(eImgFmt_NV12)
                        .addAcceptedFormat(eImgFmt_I420)
                        .addAcceptedSize(eImgSize_Full);
                    break;
                case MFLL_MAKE_REVISION(2, 5, 0):
                    sel.mOBufferFull
                        .setRequired(MTRUE)
                        .addAcceptedFormat(eImgFmt_NV12) // NV12 first
                        .addAcceptedFormat(eImgFmt_YUY2)
                        .addAcceptedFormat(eImgFmt_NV21)
                        .addAcceptedFormat(eImgFmt_I420)
                        .addAcceptedSize(eImgSize_Full);
                    break;
                case MFLL_MAKE_REVISION(3, 0, 0):
                    if (mSupport10BitOutput && sel.mState.mMultiCamFeatureMode != MTK_MULTI_CAM_FEATURE_MODE_VSDOF) // Normal MFNR
                        sel.mOBufferFull.addAcceptedFormat(eImgFmt_MTK_YUV_P010);
                    sel.mOBufferFull
                        .setRequired(MTRUE)
                        //.addAcceptedFormat(eImgFmt_MTK_YUV_P010) // YUV 10 bit Packed first
                        .addAcceptedFormat(eImgFmt_NV12) // NV12 first
                        .addAcceptedFormat(eImgFmt_YUY2)
                        .addAcceptedFormat(eImgFmt_NV21)
                        .addAcceptedFormat(eImgFmt_I420)
                        .addAcceptedSize(eImgSize_Full);
                    break;
                default:
                    removeShotInfo(getUniqueKey());
                    MY_LOGE("MFNR Provider is not support for mfnrcore(%s)", pShotInfo->getMfnrCoreVersionString().c_str());
                    return BAD_VALUE;
            }

            sel.mOMetadataApp.setRequired(MTRUE);
            sel.mOMetadataHal.setRequired(MTRUE);

        } else {
            sel.mOBufferFull.setRequired(MFALSE);
            sel.mOMetadataApp.setRequired(MFALSE);
            sel.mOMetadataHal.setRequired(MFALSE);
        }

        return OK;
    };

    virtual void init()
    {
        FUNCTION_SCOPE;
        //nothing to do for MFNR
    };

    virtual MERROR process(RequestPtr pRequest, RequestCallbackPtr pCallback)
    {
        FUNCTION_SCOPE;

        std::lock_guard<decltype(mProcessMx)> lk(mProcessMx);

        // restore callback function for abort API
        if(pCallback != nullptr) {
           m_callbackprt = pCallback;
        }

        //maybe need to keep a copy in member<sp>
        IMetadata* pAppMeta = pRequest->mIMetadataApp->acquire();
        IMetadata* pHalMeta = pRequest->mIMetadataHal->acquire();
        IMetadata* pHalMetaDynamic = pRequest->mIMetadataDynamic->acquire();
        MINT32 processUniqueKey = 0;

#if (SUPPORT_YUV_BSS==0)
        MINT8   RequestIndex = pRequest->mRequestBSSIndex;
        MINT8   RequestCount = pRequest->mRequestBSSCount;
#else
        MUINT8  RequestIndex = pRequest->mRequestIndex;
        MUINT8  RequestCount = pRequest->mRequestCount;
#endif

        // clean to zero
        IMetadata::setEntry<MUINT8>(pHalMeta, MTK_ISP_P2_TUNING_UPDATE_MODE, 0);

        if (!IMetadata::getEntry<MINT32>(pHalMeta, MTK_PIPELINE_UNIQUE_KEY, processUniqueKey)) {
            MY_LOGE("cannot get unique about MFNR capture");
            return BAD_VALUE;
        }

        //get MFNRShotInfo
        std::shared_ptr<MFNRShotInfo> pShotInfo = getShotInfo(processUniqueKey);
        if (CC_UNLIKELY(pShotInfo.get() == nullptr)) {
            removeShotInfo(processUniqueKey);
            MY_LOGE("get MFNRShotInfo instance failed! cannot process MFNR shot.");
            return BAD_VALUE;
        }

#if 1
        if (RequestIndex == 0) {
            MY_LOGE("TODO: please fix it. <initMfnrCore timing>");
            {
                std::lock_guard<std::mutex> __l(m_futureExeMx);
                auto t1 = std::async(std::launch::async, [this, pShotInfo]() {
                        MFLL_THREAD_NAME("initMfnrCore");
                        pShotInfo->initMfnrCore(); // init MFNR controller
                    });
                m_futureExe = std::shared_future<void>(std::move(t1));
            }
        }
#endif

        //wait initMfnrCore() done
        if (RequestIndex == 0) {
            pShotInfo->setRealBlendNum(RequestCount);
            pShotInfo->setMainFrameHalMetaIn(pRequest->mIMetadataHal->acquire());
            pShotInfo->setMainFrameHalMetaOut(pRequest->mOMetadataHal->acquire());
            std::shared_future<void> t1;
            {
                std::lock_guard<std::mutex> __l(m_futureExeMx);
                t1 = m_futureExe;
            }

            if (t1.valid())
                t1.wait();

            pShotInfo->execute();
        }

        /* create IMfllImageBuffer of Full/ Quarter YUV */
        sp<IMfllImageBuffer> mfllImgBuf_yuv_full = IMfllImageBuffer::createInstance("FullYuv");
        sp<IMfllImageBuffer> mfllImgBuf_yuv_quarter = IMfllImageBuffer::createInstance("QuarterYuv");
        sp<IMfllImageBuffer> mfllMixWorkingBuf = IMfllImageBuffer::createInstance("MixWorking");

        if (pRequest->mIBufferFull != nullptr) {
            IImageBuffer* pImgBuffer = pRequest->mIBufferFull->acquire();
            MY_LOGD("[IN] Full image VA: 0x%p, Size(%dx%d)", (void*)pImgBuffer->getBufVA(0), pImgBuffer->getImgSize().w, pImgBuffer->getImgSize().h);
            mfllImgBuf_yuv_full->setImageBuffer(pImgBuffer);
            mfllImgBuf_yuv_full->setAligned(mYuvAlign.w, mYuvAlign.h);
        }
        if (pRequest->mIBufferSpecified != nullptr) {
            IImageBuffer* pImgBuffer = pRequest->mIBufferSpecified->acquire();
            MY_LOGD("[IN] Quarter image VA: 0x%p, Size(%dx%d)", (void*)pImgBuffer->getBufVA(0), pImgBuffer->getImgSize().w, pImgBuffer->getImgSize().h);
            mfllImgBuf_yuv_quarter->setImageBuffer(pImgBuffer);
            mfllImgBuf_yuv_quarter->setAligned(mQYuvAlign.w, mQYuvAlign.h);
        }
        if (pRequest->mOBufferFull != nullptr) {
            IImageBuffer* pImgBuffer = pRequest->mOBufferFull->acquire();
            MY_LOGD("[OUT] Full image VA: 0x%p, Size(%dx%d)", (void*)pImgBuffer->getBufVA(0), pImgBuffer->getImgSize().w, pImgBuffer->getImgSize().h);
            mfllMixWorkingBuf->setImageBuffer(pImgBuffer);
            pShotInfo->setOutputBufToMfnrCore(mfllMixWorkingBuf);
        }

        if (pRequest->mIMetadataDynamic != nullptr) {
            IMetadata *meta = pRequest->mIMetadataDynamic->acquire();
            if (meta != NULL)
                MY_LOGD("[IN] Dynamic metadata count: %lu", meta->count());
            else
                MY_LOGD("[IN] Dynamic metadata Empty");
        }

        MY_LOGD("collected request(%d/%d)",
                RequestIndex,
                RequestCount);

        if (CC_UNLIKELY( mvRequests.size() != RequestIndex ))
            MY_LOGE("Input sequence of requests from P2A is wrong");

        mvRequests.push_back(pRequest);

        MfllMotionVector mv = pShotInfo->calMotionVector(pHalMeta, RequestIndex);
        pShotInfo->addDataToMfnrCore(mfllImgBuf_yuv_full, mfllImgBuf_yuv_quarter, mv, pAppMeta, pHalMeta, pHalMetaDynamic, RequestIndex);

        if (RequestIndex == RequestCount - 1)
        {
            pShotInfo->waitExecution();
            MY_LOGD("have collected all requests");
            for (auto req : mvRequests) {
                MY_LOGD("callback request(%d/%d) %p",
                        RequestIndex,
                        RequestCount, pCallback.get());
                if (pCallback != nullptr) {
                    pCallback->onCompleted(req, 0);
                }
            }
            mvRequests.clear();
            removeShotInfo(processUniqueKey);
#ifdef __DEBUG
            dumpShotInfo();
#endif
        }

        return 0;
    };

    virtual void abort(vector<RequestPtr>& pRequests)
    {
        FUNCTION_SCOPE;

#if (SUPPORT_YUV_BSS==0)
        MY_LOGD("not support abort() for RAW BSS");
#else
        std::lock_guard<decltype(mProcessMx)> lk(mProcessMx);

        bool bAbort = false;
        IMetadata *pHalMeta;
        MINT32 processUniqueKey = 0;
        std::shared_ptr<MFNRShotInfo> pShotInfo;

        for(auto req:pRequests) {
            bAbort = false;
            pHalMeta = req->mIMetadataHal->acquire();
            if(!IMetadata::getEntry<MINT32>(pHalMeta, MTK_PIPELINE_UNIQUE_KEY, processUniqueKey)){
                MY_LOGW("cannot get unique about MFNR capture");
            }
            pShotInfo = getShotInfo(processUniqueKey);

            /* if MFNR plugin receives abort, it will cancel mfll immediately */
            if(pShotInfo != nullptr){
               if (!pShotInfo->getDoCancelStatus()) {
                    pShotInfo->doCancel();
                    pShotInfo->waitExecution(); //wait mfll done
                    pShotInfo->setDoCancelStatus(true);
               }
            }else{
               MY_LOGW("pShotInfo is null");
            }

            if(m_callbackprt != nullptr){
               MY_LOGD("m_callbackprt is %p", m_callbackprt.get());
               /*MFNR plugin callback request to MultiFrameNode */
               for (Vector<RequestPtr>::iterator it = mvRequests.begin() ; it != mvRequests.end(); it++){
                    if((*it) == req){
                        mvRequests.erase(it);
                        m_callbackprt->onAborted(req);
                        bAbort = true;
                        break;
                    }
               }
            }else{
               MY_LOGW("callbackptr is null");
            }

            if (!bAbort){
               MY_LOGW("Desire abort request[%d] is not found", req->mRequestIndex);
            }

            if(mvRequests.empty()) {
               removeShotInfo(processUniqueKey);
               MY_LOGD("abort() cleans all the requests");
            }else {
               MY_LOGW("abort() does not clean all the requests");
            }
        }
#endif
    };

    virtual void uninit()
    {
        FUNCTION_SCOPE;
        //nothing to do for MFNR
    };

    virtual ~MFNRProviderImpl()
    {
        FUNCTION_SCOPE;
        removeAllShotInfo();

#ifdef __DEBUG
        dumpShotInfo();
#endif
    };

private:
    MINT32 getUniqueKey()
    {
        FUNCTION_SCOPE;

        std::lock_guard<decltype(mShotInfoMx)> lk(mShotInfoMx);
        return mUniqueKey;
    }

    void setUniqueKey(MINT32 key)
    {
        FUNCTION_SCOPE;

        std::lock_guard<decltype(mShotInfoMx)> lk(mShotInfoMx);
        mUniqueKey = key;
    }

    void createShotInfo()
    {
        FUNCTION_SCOPE;

        std::lock_guard<decltype(mShotInfoMx)> lk(mShotInfoMx);
        mShots[mUniqueKey] = std::shared_ptr<MFNRShotInfo>(new MFNRShotInfo(mUniqueKey, mOpenId, mMfbMode, mRealIso, mShutterTime, mFlashOn));

        MY_LOGD("Create ShotInfos: %d", mUniqueKey);
    };

    void removeShotInfo(MINT32 key)
    {
        FUNCTION_SCOPE;

        std::lock_guard<decltype(mShotInfoMx)> lk(mShotInfoMx);
        if (mShots.count(key)) {
            mShots[key] = nullptr;
            mShots.erase(key);

            MY_LOGD("Remvoe ShotInfos: %d", key);
        }
    };

    void removeAllShotInfo()
    {
        FUNCTION_SCOPE;

        std::lock_guard<decltype(mShotInfoMx)> lk(mShotInfoMx);
        for (auto it = mShots.begin(); it != mShots.end(); ++it) {
            it->second = nullptr;
            MY_LOGD("Remvoe ShotInfos: %d", it->first);
        }
        mShots.clear();
    };

    std::shared_ptr<MFNRShotInfo> getShotInfo(MINT32 key)
    {
        FUNCTION_SCOPE;

        std::lock_guard<decltype(mShotInfoMx)> lk(mShotInfoMx);
        return mShots[key];
    };

    void dumpShotInfo()
    {
        FUNCTION_SCOPE;

        std::lock_guard<decltype(mShotInfoMx)> lk(mShotInfoMx);

        std::string usage;
        for ( auto it = mShots.begin(); it != mShots.end(); ++it )
            usage += " " + std::to_string(it->first);

        MY_LOGD("All ShotInfos:%s", usage.c_str());
    };

    void updatePerFrameMetadata(const MFNRShotInfo* pShotInfo, IMetadata* pAppMeta, IMetadata* pHalMeta, bool bLastFrame)
    {
        FUNCTION_SCOPE;

        ASSERT(pShotInfo, "updatePerFrameMetadata::pShotInfo is null.");
        ASSERT(pAppMeta,  "updatePerFrameMetadata::pAppMeta is null.");
        ASSERT(pHalMeta,  "updatePerFrameMetadata::pHalMeta is null.");

        // update_app_setting(pAppMeta, pShotInfo);
        {
            bool bNeedManualAe = [&](){
                // if using ZHDR, cannot apply manual AE
                if (mfll::isZhdrMode(pShotInfo->getMfbMode()))
                    return false;
                // if using FLASH, cannot apply menual AE
                if (pShotInfo->getIsFlashOn())
                    return false;
                // if uisng MFNR (since MFNR uses ZSD buffers), we don't need manual AE
                // but if MFNR using non-ZSD flow, we need to apply manual AE
                if (mfll::isMfllMode(pShotInfo->getMfbMode())) {
                    if (mfll::isZsdMode(pShotInfo->getMfbMode()))
                        return false;
                    else
                        return true;
                }
                /// otherwise, we need it
                return true;
            }();

            if (bNeedManualAe) {
                IMetadata::setEntry<MUINT8>(pAppMeta, MTK_CONTROL_AE_MODE, MTK_CONTROL_AE_MODE_OFF);
                IMetadata::setEntry<MINT32>(pAppMeta, MTK_SENSOR_SENSITIVITY, pShotInfo->getFinalIso());
                IMetadata::setEntry<MINT64>(pAppMeta, MTK_SENSOR_EXPOSURE_TIME, pShotInfo->getFinalShutterTime() * 1000); // ms->us
                IMetadata::setEntry<MUINT8>(pAppMeta, MTK_CONTROL_AWB_LOCK, MTRUE);
            }

            IMetadata::setEntry<MUINT8>(pAppMeta, MTK_CONTROL_VIDEO_STABILIZATION_MODE, MTK_CONTROL_VIDEO_STABILIZATION_MODE_ON);
        }

        {
            MUINT8 bOriFocusPause  = 0;
            if ( !IMetadata::getEntry<MUINT8>(pHalMeta, MTK_FOCUS_PAUSE, bOriFocusPause) ) {
                MY_LOGW("%s: cannot retrieve MTK_FOCUS_PAUSE from HAL metadata, assume "\
                        "it to 0", __FUNCTION__);
            }

            // update ISP profile for zHDR (single frame default)
            if (mfll::isZhdrMode(pShotInfo->getMfbMode())) {
                MY_LOGE("Zhdr is not support in MFNR plugin");
                bool isAutoHDR = mfll::isAutoHdr(pShotInfo->getMfbMode());
                MUINT sensorMode = SENSOR_SCENARIO_ID_NORMAL_CAPTURE;
                MSize sensorSize;
                NSCamHW::HwInfoHelper helper(pShotInfo->getOpenId());
                if (!helper.getSensorSize(sensorMode, sensorSize)) {
                    MY_LOGW("cannot get sensor size");
                }
                else {
                    // Prepare query Feature Shot ISP Profile
                    ProfileParam profileParam
                    {
                        sensorSize,
                        SENSOR_VHDR_MODE_ZVHDR, /*VHDR mode*/
                        sensorMode,
                        ProfileParam::FLAG_NONE,
                        ((isAutoHDR) ? (ProfileParam::FMASK_AUTO_HDR_ON) : (ProfileParam::FMASK_NONE)),
                    };

                    MUINT8 profile = 0;
                    if (FeatureProfileHelper::getShotProf(profile, profileParam))
                    {
                        MY_LOGD("ISP profile is set(%u)", profile);
                        // modify hal control metadata for zHDR
                        IMetadata::setEntry<MUINT8>(
                            pHalMeta , MTK_3A_ISP_PROFILE , profile);
                        IMetadata::setEntry<MUINT8>(
                            pHalMeta, MTK_3A_AE_CAP_SINGLE_FRAME_HDR, 1);
                    }
                    else
                    {
                        MY_LOGW("ISP profile is not set(%u)", profile);
                    }
                }
            }

            // pause AF for (N - 1) frames and resume for the last frame
            IMetadata::setEntry<MUINT8>(pHalMeta, MTK_FOCUS_PAUSE, bLastFrame ? bOriFocusPause : 1);
            IMetadata::setEntry<MUINT8>(pHalMeta, MTK_HAL_REQUEST_REQUIRE_EXIF, 1);
            IMetadata::setEntry<MUINT8>(pHalMeta, MTK_HAL_REQUEST_DUMP_EXIF, 1);

            // need pure raw for MFNR flow
            IMetadata::setEntry<MINT32>(pHalMeta, MTK_P1NODE_RAW_TYPE, 1);

            MINT32 customHintInMFNR = mfll::getCustomHint(pShotInfo->getMfbMode());
            MINT32 customHintInHal;
            // check customHint in metadata for customize feature
            if ( !IMetadata::getEntry<MINT32>(pHalMeta, MTK_PLUGIN_CUSTOM_HINT, customHintInHal) ) {
                MY_LOGW("%s: cannot retrieve MTK_PLUGIN_CUSTOM_HINT from HAL metadata, assume "\
                        "it to %d", __FUNCTION__, customHintInMFNR);
                IMetadata::setEntry<MINT32>( pHalMeta, MTK_PLUGIN_CUSTOM_HINT, customHintInMFNR);
            }
            else if (customHintInMFNR != customHintInHal) { // query and check the result
                MY_LOGW("%s: MTK_PLUGIN_CUSTOM_HINT in MFNR(%d) and Hal(%d) setting are different"
                        , __FUNCTION__, customHintInMFNR, customHintInHal);
            }
        }

        //AE Flare Enable, due to MTK_CONTROL_AE_MODE_OFF will disable AE_Flare
        // MFNR must set MTK_3A_FLARE_IN_MANUAL_CTRL_ENABLE to enable AE_Flare
        // TODO: hal3a need to implement this command for each platform.
        {
            IMetadata::setEntry<MBOOL>(pHalMeta, MTK_3A_FLARE_IN_MANUAL_CTRL_ENABLE, MTRUE);
        }
    }

    MfllMode updateMfbMode(IMetadata* pAppMeta, MBOOL isZSDMode) {

        IMetadata::IEntry const eMfb           = pAppMeta->entryFor(MTK_MFNR_FEATURE_MFB_MODE);
        IMetadata::IEntry const eAis           = pAppMeta->entryFor(MTK_MFNR_FEATURE_AIS_MODE);
        IMetadata::IEntry const eSceneMode     = pAppMeta->entryFor(MTK_CONTROL_SCENE_MODE);

        int mfbMode = [&]()
        {
            // If MTK specific parameter AIS on or MFB mode is AIS, set to AIS mode (2)
            if (( ! eMfb.isEmpty() && eMfb.itemAt(0, Type2Type<MINT32>()) == MTK_MFNR_FEATURE_MFB_AIS)  ||
                     ( ! eAis.isEmpty() && eAis.itemAt(0, Type2Type<MINT32>()) == MTK_MFNR_FEATURE_AIS_ON)) {
                return MTK_MFB_MODE_AIS;
            }
            // Scene mode is Night or MFB mode is MFLL, set to MFLL mode (1)
            else if (( ! eMfb.isEmpty() && eMfb.itemAt(0, Type2Type<MINT32>()) == MTK_MFNR_FEATURE_MFB_MFLL) ||
                     ( ! eSceneMode.isEmpty() && eSceneMode.itemAt(0, Type2Type<MUINT8>()) == MTK_CONTROL_SCENE_MODE_NIGHT)) {
                return MTK_MFB_MODE_MFLL;
            }
            else if (( ! eMfb.isEmpty() && eMfb.itemAt(0, Type2Type<MINT32>()) == MTK_MFNR_FEATURE_MFB_AUTO)) {
#ifdef CUST_MFLL_AUTO_MODE
                static_assert( ((CUST_MFLL_AUTO_MODE >= MTK_MFB_MODE_OFF)&&(CUST_MFLL_AUTO_MODE < MTK_MFB_MODE_NUM)),
                               "CUST_MFLL_AUTO_MODE is invalid in custom/feature/mfnr/camera_custom_mfll.h" );

                MY_LOGD("CUST_MFLL_AUTO_MODE:%d", CUST_MFLL_AUTO_MODE);
                return static_cast<mtk_platform_metadata_enum_mfb_mode>(CUST_MFLL_AUTO_MODE);
#else
#error "CUST_MFLL_AUTO_MODE is no defined in custom/feature/mfnr/camera_custom_mfll.h"
#endif
            }
            // Otherwise, set MFB off (0)
            else {
                return MTK_MFB_MODE_OFF;
            }
        }();

        int bForceMfb = MfllProperty::isForceMfll();
        if (CC_UNLIKELY( bForceMfb == 0 )) {
            MY_LOGD("Force disable MFNR");
            mfbMode = MTK_MFB_MODE_OFF;
        }
        else if (CC_UNLIKELY(bForceMfb > 0)) {
            MY_LOGD("Force MFNR (bForceMfb:%d)", bForceMfb);
            mfbMode = bForceMfb;
        }

        MfllMode mfllMode = MfllMode_NormalMfll;

        // 0: Not specific, 1: MFNR, 2: AIS
        switch (mfbMode) {
            case MTK_MFB_MODE_MFLL:
                mfllMode = (CC_LIKELY(isZSDMode) ? MfllMode_ZsdMfll : MfllMode_NormalMfll);
                break;
            case MTK_MFB_MODE_AIS:
                mfllMode = MfllMode_NormalAis;//(CC_LIKELY(isZSDMode) ? MfllMode_ZsdAis : MfllMode_NormalAis);
                break;
            case MTK_MFB_MODE_OFF:
                mfllMode = MfllMode_Off;
                break;
            default:
                mfllMode = (CC_LIKELY(isZSDMode) ? MfllMode_ZsdMfll : MfllMode_NormalMfll);
                break;
        }

        MY_LOGD("MfllMode(0x%X), mfbMode(%d), isZsd(%d)",
                mfllMode, mfbMode, isZSDMode);


        return mfllMode;
    }

    bool updateInputBufferInfo(MINT32 mfllCoreVersion, Selection& sel)
    {
        std::shared_ptr<MFNRShotInfo> pShotInfo = getShotInfo(getUniqueKey());

        if (pShotInfo == nullptr)
            return false;

        switch (mfllCoreVersion) {
            case MFLL_MAKE_REVISION(1, 4, 0):
                if (CC_LIKELY( pShotInfo->getIsFullSizeMc() )) {
                    sel.mIBufferFull.addAcceptedFormat(eImgFmt_I422);
                    sel.mIBufferSpecified.addAcceptedFormat(eImgFmt_Y8);
                } else {
                    sel.mIBufferFull.addAcceptedFormat(eImgFmt_BAYER10);
                    sel.mIBufferSpecified.addAcceptedFormat(eImgFmt_YUY2);
                }
                //
                sel.mIBufferFull.addAcceptedSize(eImgSize_Full);
                sel.mIBufferSpecified.addAcceptedSize(eImgSize_Quarter);
                mYuvAlign  = MSize(2, 2);
                mQYuvAlign = MSize(2, 2);
                break;

            case MFLL_MAKE_REVISION(2, 0, 0):
                if (CC_LIKELY( pShotInfo->getIsFullSizeMc() )) {
                    sel.mIBufferFull.addAcceptedFormat(eImgFmt_I422);
                    sel.mIBufferSpecified.addAcceptedFormat(eImgFmt_Y8);
                } else {
                    sel.mIBufferFull.addAcceptedFormat(eImgFmt_YUY2);
                    sel.mIBufferSpecified.addAcceptedFormat(eImgFmt_I422);
                }
                //
                sel.mIBufferFull.addAcceptedSize(eImgSize_Full);
                sel.mIBufferSpecified.addAcceptedSize(eImgSize_Specified).setSpecifiedSize(
                    calcDownScaleSize(pShotInfo->getSizeSrc(), 1, 2) );
                mYuvAlign  = MSize(16, 16);
                mQYuvAlign = MSize(16, 16);
                break;

            case MFLL_MAKE_REVISION(2, 1, 0):
                if (CC_LIKELY( pShotInfo->getIsFullSizeMc() )) {
                    sel.mIBufferFull.addAcceptedFormat(eImgFmt_I422);
                    sel.mIBufferSpecified.addAcceptedFormat(eImgFmt_Y8);
                } else {
                    sel.mIBufferFull.addAcceptedFormat(eImgFmt_YUY2);
                    sel.mIBufferSpecified.addAcceptedFormat(eImgFmt_I422);
                }
                //
                sel.mIBufferFull.addAcceptedSize(eImgSize_Full);
                sel.mIBufferSpecified.addAcceptedSize(eImgSize_Quarter);
                mYuvAlign  = MSize(16, 16);
                mQYuvAlign = MSize(16, 16);
                break;

            case MFLL_MAKE_REVISION(2, 5, 0):
                sel.mIBufferFull.addAcceptedFormat(eImgFmt_NV12);
                sel.mIBufferSpecified.addAcceptedFormat(eImgFmt_Y8);
                //
                if (CC_LIKELY( !pShotInfo->getEnableDownscale() )) {
                    sel.mIBufferFull.addAcceptedSize(eImgSize_Full);
                } else {
                    if (CC_UNLIKELY( pShotInfo->getDownscaleRatio() < 1 )) {
                        MY_LOGE("MFNR Provider is not support due to error DownScaleFactor %d", pShotInfo->getDownscaleRatio());
                        return false;
                    }
                    sel.mIBufferFull.addAcceptedSize(eImgSize_Specified).setSpecifiedSize(
                        calcDownScaleSize(pShotInfo->getSizeSrc(), 1,  pShotInfo->getDownscaleRatio()) );
                }
                sel.mIBufferSpecified.addAcceptedSize(eImgSize_Specified).setSpecifiedSize(
                    calcDownScaleSize(pShotInfo->getSizeSrc(), 1, 4) );
                mYuvAlign  = MSize(16, 16);
                mQYuvAlign = MSize(16, 16);
                break;

            case MFLL_MAKE_REVISION(3, 0, 0):
                if (sel.mState.mMultiCamFeatureMode != MTK_MULTI_CAM_FEATURE_MODE_VSDOF) { // Normal MFNR
                    sel.mIBufferFull.addAcceptedFormat(eImgFmt_MTK_YUV_P010);
                    sel.mIBufferSpecified.addAcceptedFormat(eImgFmt_NV12/*eImgFmt_Y8*/); //Due to postview issue
                } else {
                    MY_LOGE("TODO: please fix it. <MFNR3.0 should apply 10bit YUV>");
                    pShotInfo->setInputYuvFmt(InputYuvFmt_Nv12);
                    sel.mIBufferFull.addAcceptedFormat(eImgFmt_NV12);
                    sel.mIBufferSpecified.addAcceptedFormat(eImgFmt_Y8);
                }
                //
                if (CC_UNLIKELY( pShotInfo->getDownscaleDividend() < 1 || pShotInfo->getDownscaleDividend() > pShotInfo->getDownscaleDivisor() )) {
                    MY_LOGE("MFNR Provider is not support due to error DownScaleFactor is %d/%d", pShotInfo->getDownscaleDividend(), pShotInfo->getDownscaleDivisor());
                    return false;
                } else {
                    MSize fullSize = calcDownScaleSize(pShotInfo->getSizeSrc(), pShotInfo->getDownscaleDividend(), pShotInfo->getDownscaleDivisor());
                    MSize quaterSize = calcDownScaleSize(fullSize, 1, (!pShotInfo->getEnableDownscale()?4:2));
                    sel.mIBufferFull.addAcceptedSize(eImgSize_Specified).setSpecifiedSize(fullSize);
                    sel.mIBufferSpecified.addAcceptedSize(eImgSize_Specified).setSpecifiedSize(quaterSize);
                }
                mYuvAlign  = MSize(64, 16);
                mQYuvAlign = MSize(16, 16);
                break;

            default:
                MY_LOGE("MFNR Provider is not support for mfnrcore(%s)", pShotInfo->getMfnrCoreVersionString().c_str());
                return false;
        }

        return true;
    }

    MSize calcDownScaleSize(const MSize& m, int dividend, int divisor)
    {
        if (CC_LIKELY( divisor ))
            return MSize(m.w * dividend / divisor, m.h * dividend / divisor);

        MY_LOGW("%s: divisor is zero", __FUNCTION__);
        return m;
    }

private:

    MINT32                          mUniqueKey;
    MINT32                          mOpenId;
    MINT32                          mSupport10BitOutput;
    MfllMode                        mMfbMode;
    MINT32                          mRealIso;
    MINT32                          mShutterTime;
    MBOOL                           mZSDMode;
    MBOOL                           mFlashOn;
    MSize                           mYuvAlign;
    MSize                           mQYuvAlign;

    Vector<RequestPtr>              mvRequests;
    std::mutex                      mShotInfoMx; // protect MFNRShotInfo
    std::mutex                      mProcessMx; // protect MFNRShotInfo
    std::shared_future<void>        m_futureExe;
    mutable std::mutex              m_futureExeMx;

    std::unordered_map<MUINT32, std::shared_ptr<MFNRShotInfo>>
                                    mShots;

    RequestCallbackPtr              m_callbackprt;
};

REGISTER_PLUGIN_PROVIDER(MultiFrame, MFNRProviderImpl);

