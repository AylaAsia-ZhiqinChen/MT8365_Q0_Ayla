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
#define LOG_TAG "AisPlugin"
//
#include <mtkcam/utils/std/Log.h>
//
#include <stdlib.h>
#include <utils/Errors.h>
#include <utils/List.h>
#include <utils/RefBase.h>
#include <sstream>
//
#include <mtkcam/utils/metadata/client/mtk_metadata_tag.h>
#include <mtkcam/utils/metadata/hal/mtk_platform_metadata_tag.h>
//
//
#include <mtkcam/utils/imgbuf/IIonImageBufferHeap.h>
//
#include <mtkcam/drv/IHalSensor.h>
#include <mtkcam/utils/std/Format.h>
//
#include <mtkcam3/pipeline/hwnode/NodeId.h>

#include <mtkcam/utils/metastore/IMetadataProvider.h>
#include <mtkcam/utils/metastore/ITemplateRequest.h>
#include <mtkcam/utils/metastore/IMetadataProvider.h>
#include <mtkcam3/3rdparty/plugin/PipelinePlugin.h>
#include <mtkcam3/3rdparty/plugin/PipelinePluginType.h>
#include <mtkcam/utils/hw/HwInfoHelper.h>
#include <cutils/properties.h>
//
using namespace NSCam;
using namespace android;
using namespace std;
using namespace NSCam::NSPipelinePlugin;
/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
//
#define __DEBUG // enable debug
#ifdef __DEBUG
#define ATRACE_TAG                                  ATRACE_TAG_CAMERA
#include <utils/Trace.h>
#define FUNCTION_TRACE()                            ATRACE_CALL()
#define FUNCTION_TRACE_NAME(name)                   ATRACE_NAME(name)
#define FUNCTION_TRACE_BEGIN(name)                  ATRACE_BEGIN(name)
#define FUNCTION_TRACE_END()                        ATRACE_END()
#define FUNCTION_TRACE_ASYNC_BEGIN(name, cookie)    ATRACE_ASYNC_BEGIN(name, cookie)
#define FUNCTION_TRACE_ASYNC_END(name, cookie)      ATRACE_ASYNC_END(name, cookie)
#else
#define FUNCTION_TRACE()
#define FUNCTION_TRACE_NAME(name)
#define FUNCTION_TRACE_BEGIN(name)
#define FUNCTION_TRACE_END()
#define FUNCTION_TRACE_ASYNC_BEGIN(name, cookie)
#define FUNCTION_TRACE_ASYNC_END(name, cookie)
#endif
//
#ifdef __DEBUG
#define FUNCTION_SCOPE          auto __scope_logger__ = create_scope_logger(__FUNCTION__)
#include <memory>
#include <functional>
static std::shared_ptr<char> create_scope_logger(const char* functionName)
{
    char* pText = const_cast<char*>(functionName);
    CAM_LOGD_IF(pText, "[%s] + ", pText);
    return std::shared_ptr<char>(pText, [](char* p){ CAM_LOGD_IF(p, "[%s] -", p); });
}
#else
#define FUNCTION_SCOPE          do{}while(0)
#endif
//
/******************************************************************************
 *
 ******************************************************************************/


/******************************************************************************
*
******************************************************************************/
class AisProviderImpl : public RawPlugin::IProvider
{
    typedef RawPlugin::Property Property;
    typedef RawPlugin::Selection Selection;
    typedef RawPlugin::Request::Ptr RequestPtr;
    typedef RawPlugin::RequestCallback::Ptr RequestCallbackPtr;
public:

    virtual void set(MINT32 iOpenId, MINT32 iOpenId2)
    {
        MY_LOGD("set openId:%d openId2:%d", iOpenId, iOpenId2);
        mOpenId = iOpenId;
        // for debug
        mEnable = property_get_int32("vendor.debug.camera.ais.enable", 0);
        mProcess = property_get_int32("vendor.debug.camera.ais.process", 0);
        mDumpBuffer = property_get_int32("vendor.debug.camera.ais.dump", 0);
    }

    virtual const Property& property()
    {
        //FUNCTION_SCOPE;
        static Property prop;
        static bool inited;

        if (!inited) {
            prop.mName = "MTK Ais";
            prop.mFeatures = MTK_FEATURE_AIS;
            prop.mPriority = ePriority_Highest;
            prop.mInPlace = MTRUE;
            inited = true;
        }
        return prop;
    };

    virtual MERROR negotiate(Selection& sel)
    {
        // only decision frames requirement and no need to execute plugin process
        sel.mDecision.mProcess = mProcess;
        if (!mEnable) {
            MY_LOGD("mEnable(%d)", mEnable);
            return OK;
        }
        //
        FUNCTION_SCOPE;
        FUNCTION_TRACE();
        MY_LOGD_IF(!mProcess, "only query frames decision, no need to execute plugin process");
        //
        sel.mIBufferFull
            .setRequired(MTRUE)
            .addAcceptedFormat(eImgFmt_BAYER10_UNPAK)
            .addAcceptedFormat(eImgFmt_BAYER10)
            .addAcceptedSize(eImgSize_Full);

        sel.mOBufferFull
            .setRequired(MTRUE)
            .addAcceptedFormat(eImgFmt_BAYER10_UNPAK)
            .addAcceptedFormat(eImgFmt_BAYER10)
            .addAcceptedSize(eImgSize_Full);

        sel.mIMetadataDynamic.setRequired(MTRUE);
        sel.mIMetadataApp.setRequired(MTRUE);
        sel.mIMetadataHal.setRequired(MTRUE);
        sel.mOMetadataApp.setRequired(MTRUE);
        sel.mOMetadataHal.setRequired(MTRUE);

        // Without control metadata, it's no need to append additional metadata
        // Use default frame setting
        if (sel.mIMetadataApp.getControl() != nullptr) {
            MetadataPtr pAppAddtional = make_shared<IMetadata>();
            MetadataPtr pHalAddtional = make_shared<IMetadata>();

            IMetadata* pAppMeta = pAppAddtional.get();
            //IMetadata* pHalMeta = pHalAddtional.get();

            if (sel.mState.mAppManual3A || sel.mState.mFlashFired) {
                MY_LOGD("don't change exposure setting, due to isAppManual3A(%d), isFlashOn(%d)",
                        sel.mState.mAppManual3A, sel.mState.mFlashFired);
            }
            else {
                // experimental code: test to set manual iso/exposure setting
                MUINT8 aeMode = MTK_CONTROL_AE_MODE_OFF;
                MINT32 manualIso = sel.mState.mRealIso*2;
                MINT64 manualExposureTime = static_cast<MINT64>(sel.mState.mExposureTime)*1000/2;
                IMetadata::setEntry<MUINT8>(pAppMeta, MTK_CONTROL_AE_MODE, aeMode);
                IMetadata::setEntry<MINT32>(pAppMeta, MTK_SENSOR_SENSITIVITY, manualIso);
                IMetadata::setEntry<MINT64>(pAppMeta, MTK_SENSOR_EXPOSURE_TIME, manualExposureTime);
                MY_LOGD("set aeMode(%d), iso(%d -> %d)/exposure(%dus -> %" PRId64 "ns)",
                        aeMode, sel.mState.mRealIso, manualIso, sel.mState.mExposureTime, manualExposureTime);
            }

            sel.mIMetadataApp.setAddtional(pAppAddtional);
            sel.mIMetadataHal.setAddtional(pHalAddtional);
        }
        else {
            MY_LOGW("cannot get sel.mIMetadataApp!");
        }
        return OK;
    };

    virtual void init()
    {
        FUNCTION_SCOPE;
        FUNCTION_TRACE();
    };

    virtual MERROR process(RequestPtr pRequest,
                           RequestCallbackPtr pCallback = nullptr)
    {
        FUNCTION_SCOPE;
        FUNCTION_TRACE();
        //
        if (CC_UNLIKELY(mProcess)) {
            IImageBuffer* pInBufferFull = nullptr;
            IImageBuffer* pOutBufferFull = nullptr;
            IMetadata *pInMetadataDynamic = nullptr;
            //
            if (pRequest->mIBufferFull != nullptr) {
                pInBufferFull = pRequest->mIBufferFull->acquire();
                MY_LOGD("[IN] Full image VA: %" PRIx64 "", pInBufferFull->getBufVA(0));
            }

            if (pRequest->mOBufferFull != nullptr) {
                pOutBufferFull = pRequest->mOBufferFull->acquire();
                MY_LOGD("[OUT] Full image VA: %" PRIx64 "", pOutBufferFull->getBufVA(0));
            }

            if (pRequest->mIMetadataDynamic != nullptr) {
                pInMetadataDynamic = pRequest->mIMetadataDynamic->acquire();
                if (pInMetadataDynamic != nullptr) {
                    MY_LOGD("[IN] Dynamic metadata count: %d", pInMetadataDynamic->count());
                }
                else {
                    MY_LOGD("[IN] Dynamic metadata empty");
                }
            }


            if(pInBufferFull != nullptr) {
                void *pImgVa   = (void *) (pInBufferFull->getBufVA(0));
                int imgWidth   = pInBufferFull->getImgSize().w;
                int imgHeight  = pInBufferFull->getImgSize().h;
                int imgStride  = pInBufferFull->getBufStridesInBytes(0);
                int bufferSize = pInBufferFull->getBufSizeInBytes(0);
                MY_LOGD("pInBufferFull(%p): size(%dx%d), stride(%d) bufferSize(%d)",
                        pImgVa, imgWidth, imgHeight, imgStride, bufferSize);
                //
                if(mDumpBuffer) {
                    dumpBuffer(pInBufferFull,"Ais-OriginalRawBuf","raw");
                }
            }
        }
        else {
            MY_LOGW("must bypass plugin process(mProcess:%d)", mProcess);
        }

        if (pCallback != nullptr) {
            MY_LOGD("callback request");
            pCallback->onCompleted(pRequest, 0);
        }

        return 0;
    };

    virtual void abort(vector<RequestPtr>& pRequests)
    {
        FUNCTION_SCOPE;
    };

    virtual void uninit()
    {
        FUNCTION_SCOPE;
        FUNCTION_TRACE();
    };

    virtual ~AisProviderImpl()
    {
        FUNCTION_SCOPE;
    };
private:
    //functions
    bool IsEnableAis()
    {
        FUNCTION_SCOPE;
        return false;
    }

    void dumpBuffer(IImageBuffer* pBuf, const char* filename, const char* fileext)
    {
#define dumppath "/sdcard/cameradump_AisProc"
        char fname[256];
        sprintf(fname, "%s/%s_%dx%d.%s",
                dumppath,
                filename,
                pBuf->getImgSize().w,
                pBuf->getImgSize().h,
                fileext
                );
        pBuf->saveToFile(fname);
#undef dumppath
    }

    //variables
    MINT32                      mOpenId;
    MINT32                      mEnable = 0;
    MINT32                      mProcess = 0;
    MINT32                      mDumpBuffer = 0;
};

REGISTER_PLUGIN_PROVIDER(Raw, AisProviderImpl);

