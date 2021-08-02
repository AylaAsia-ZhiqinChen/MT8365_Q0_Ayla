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

#include "RootNode.h"

#define PIPE_CLASS_TAG "RootNode"
#define PIPE_TRACE TRACE_ROOT_NODE
#include <featurePipe/core/include/PipeLog.h>

#include <mtkcam3/feature/lmv/lmv_ext.h>
#include <custom/feature/mfnr/camera_custom_mfll.h>
#include <custom/debug_exif/dbg_exif_param.h>
#include <mtkcam/utils/metadata/hal/mtk_platform_metadata_tag.h>
#include <mtkcam/utils/metadata/client/mtk_metadata_tag.h>
#include <mtkcam/utils/hw/IFDContainer.h>
#include <mtkcam/utils/hw/HwTransform.h>

#include <mtkcam/utils/TuningUtils/FileReadRule.h>

#include <mtkcam/drv/IHalSensor.h>
#include <mtkcam/aaa/INvBufUtil.h>


#ifdef SUPPORT_MFNR
#include <mtkcam/utils/hw/IBssContainer.h>
#include <mtkcam3/feature/mfnr/IMfllNvram.h>
#include <mtkcam3/feature/mfnr/MfllProperty.h>

#include <custom/feature/mfnr/camera_custom_mfll.h>
#include <custom/debug_exif/dbg_exif_param.h>
#if (MFLL_MF_TAG_VERSION > 0)
using namespace __namespace_mf(MFLL_MF_TAG_VERSION);
#include <tuple>
#include <string>
#endif
#include <fstream>
#include <sstream>

// CUSTOM (platform)
#if MTK_CAM_NEW_NVRAM_SUPPORT
#include <mtkcam/utils/mapping_mgr/cam_idx_mgr.h>
#endif
#include <camera_custom_nvram.h>
#include <featurePipe/capture/exif/ExifWriter.h>
//
#define MFLLBSS_DUMP_PATH               "/data/vendor/camera_dump/"
#define MFLLBSS_DUMP_FD_FILENAME        "fd-data.txt"
#define MFLLBSS_DUMP_BSS_PARAM_FILENAME "bss-param.bin"
#define MFLLBSS_DUMP_BSS_IN_FILENAME    "bss-in.bin"
#define MFLLBSS_DUMP_BSS_OUT_FILENAME   "bss-out.bin"

// describes that how many frames we update to exif,
#define MFLLBSS_FD_RECT0_FRAME_COUNT    8
// describes that how many fd rect0 info we need at a frame,
#define MFLLBSS_FD_RECT0_PER_FRAME      4

#ifndef MFLL_ALGO_THREADS_PRIORITY
#define MFLL_ALGO_THREADS_PRIORITY 0
#endif

#define __TRANS_FD_TO_NOR(value, max)   (((value*2000)+(max/2))/max-1000)
#define FUNC_TRANS_FD_TO_NOR(value, max)   (value = __TRANS_FD_TO_NOR(value, max))


using std::vector;
using namespace mfll;

/*
 *  Tuning Param for BSS ALG.
 *  Should not be configure by customer
 */
#if (MFLL_MF_TAG_VERSION == 11)
static const int MF_BSS_VER = 3;
#else
static const int MF_BSS_VER = 2;
#endif
static const int MF_BSS_ON = 1;
static const int MF_BSS_ROI_PERCENTAGE = 95;
#endif //SUPPORT_MFNR

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
#endif //__DEBUG

#define MY_DBG_COND(level)          __builtin_expect( mDebugLevel >= level, false )
#define MY_LOGD3(...)               do { if ( MY_DBG_COND(3) ) MY_LOGD(__VA_ARGS__); } while(0)

#include <mtkcam3/3rdparty/plugin/PipelinePluginType.h>

CAM_ULOG_DECLARE_MODULE_ID(MOD_CAPTURE_ROOT);

#define CHECK_OBJECT(x)  do{                                            \
    if (x == nullptr) { MY_LOGE("Null %s Object", #x); return -MFALSE;} \
} while(0)

using namespace NSCam::TuningUtils;

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {
namespace NSCapture {

class RootNode::FDContainerWraper final
{
public:
    using Ptr = UniquePtr<FDContainerWraper>;

public:
    static Ptr createInstancePtr(const RequestPtr& pRequest, MINT32 sensorIndex);

public:
    MBOOL updateFaceData(const RequestPtr& pRequest, function<MVOID(const MPoint& in, MPoint& out)> transferTG2Pipe);

private:
    FDContainerWraper(const NSCamHW::HwMatrix& active2SensorMatrix, sp<IFDContainer>& fdReader);

    ~FDContainerWraper();

private:
    NSCamHW::HwMatrix   mActive2TGMatrix;
    sp<IFDContainer>    mFDReader;
};

auto
RootNode::FDContainerWraper::
createInstancePtr(const RequestPtr& pRequest, MINT32 sensorIndex) -> Ptr
{
    MY_LOGD("create FDContainerWraper ptr, reqNo:%d", pRequest->getRequestNo());

    auto pInMetaHal = pRequest->getMetadata(MID_MAN_IN_HAL);
    IMetadata* pHalMeta = (pInMetaHal != nullptr) ? pInMetaHal->native() : nullptr;
    if(pHalMeta == nullptr) {
        MY_LOGW("failed to create instance, can not get inHalMetadata");
        return MFALSE;
    }
    //
    MINT32 sensorMode;
    if (!tryGetMetadata<MINT32>(pHalMeta, MTK_P1NODE_SENSOR_MODE, sensorMode)) {
        MY_LOGW("failed to create instance, can not get tag MTK_P1NODE_SENSOR_MODE from metadata, addr:%p", pHalMeta);
        return nullptr;
    }
    //
    NSCamHW::HwTransHelper hwTransHelper(sensorIndex);
    NSCamHW::HwMatrix active2TGMatrix;
    if(!hwTransHelper.getMatrixFromActive(sensorMode, active2TGMatrix)) {
        MY_LOGW("failed to create instance, can not get active2TGMatrix, sensorMode:%d", sensorMode);
        return nullptr;
    }
    //
    sp<IFDContainer> fdReader = IFDContainer::createInstance(PIPE_CLASS_TAG, IFDContainer::eFDContainer_Opt_Read);
    if(fdReader == nullptr) {
        MY_LOGW("failed to create instance, can not get fdContainer");
        return MFALSE;
    }

    return Ptr(new FDContainerWraper(active2TGMatrix, fdReader), [](FDContainerWraper* p)
    {
        delete p;
    });
}

auto
RootNode::FDContainerWraper::
updateFaceData(const RequestPtr& pRequest, function<MVOID(const MPoint& in, MPoint& out)> transferTG2Pipe) -> MBOOL
{
    const MINT32 reqNo = pRequest->getRequestNo();
    MY_LOGD("update default fd info for request, reqNo:%d", reqNo);
    //
    auto pInMetaHal = pRequest->getMetadata(MID_MAN_IN_HAL);
    IMetadata* pHalMeta = (pInMetaHal != nullptr) ? pInMetaHal->native() : nullptr;
    if(pHalMeta == nullptr) {
        MY_LOGW("failed to get inHalMetadata, reqNo:%d", reqNo);
        return MFALSE;
    }
    //
    //
    MINT64 frameStartTimestamp = 0;
    if(!tryGetMetadata<MINT64>(pHalMeta, MTK_P1NODE_FRAME_START_TIMESTAMP, frameStartTimestamp)) {
        MY_LOGW("failed to get p1 node frame start timestamp, reqNo:%d", reqNo);
        return MFALSE;
    }
    //
    auto createFaceDataPtr = [&fdReader=mFDReader](MINT64 tsStart, MINT64 tsEnd)
    {
        using FaceData = vector<FD_DATATYPE*>;
        FaceData* pFaceData = new FaceData();
        (*pFaceData) = fdReader->queryLock(tsStart, tsEnd);
        using FaceDataPtr = std::unique_ptr<FaceData, std::function<MVOID(FaceData*)>>;
        return FaceDataPtr(pFaceData, [&fdReader](FaceData* p)
        {
            fdReader->queryUnlock(*p);
            delete p;
        });

    };
    // query fd info by timestamps, fdData must be return after use
    static const MINT64 tolerence = 600000000; // 600ms
    const MINT64 tsStart = frameStartTimestamp - tolerence;
    const MINT64 tsEnd = frameStartTimestamp;
    auto faceDataPtr = createFaceDataPtr(tsStart, tsEnd);
    // back element is the closest to the timestamp we assigned
    auto foundItem = std::find_if(faceDataPtr->rbegin(), faceDataPtr->rend(), [] (const FD_DATATYPE* e)
    {
        return e != nullptr;
    });
    //
    if(foundItem == faceDataPtr->rend())
    {
        MY_LOGW("no faceData, reqNo:%d, timestampRange:(%ld, %ld)", reqNo, tsStart, tsEnd);
        return MFALSE;
    }
    //
    MtkCameraFaceMetadata faceData = (*foundItem)->facedata;
    const MINT32 faceCount = faceData.number_of_faces;
    if(faceCount == 0)
    {
        MY_LOGD("no faceData, reqNo:%d, count:%d", reqNo, faceCount);
        return MTRUE;
    }

    IMetadata::IEntry entryFaceRects(MTK_FEATURE_FACE_RECTANGLES);
    IMetadata::IEntry entryPoseOriens(MTK_FACE_FEATURE_POSE_ORIENTATIONS);
    for(MINT32 index = 0; index < faceCount; ++index) {
        MtkCameraFace& faceInfo = faceData.faces[index];
        // Face Rectangle
        // the source face data use point left-top and right-bottom to show the fd rectangle
        const MPoint actLeftTop(faceInfo.rect[0], faceInfo.rect[1]);
        const MPoint actRightBottom(faceInfo.rect[2], faceInfo.rect[3]);
        // active to tg domain
        MPoint tgLeftTop;
        MPoint tgRightBottom;
        mActive2TGMatrix.transform(actLeftTop, tgLeftTop);
        mActive2TGMatrix.transform(actRightBottom, tgRightBottom);
        // tg to pipe domain
        MPoint pipeLeftTop;
        MPoint pipeRightBottom;
        transferTG2Pipe(tgLeftTop, pipeLeftTop);
        transferTG2Pipe(tgRightBottom, pipeRightBottom);
        MY_LOGD("detected face rectangle, reqNo:%d, faceNum:%d/%d, act:(%d, %d)x(%d, %d), tg:(%d, %d)x(%d, %d), pipe:(%d, %d)x(%d, %d)",
            reqNo, index, faceCount,
            actLeftTop.x, actLeftTop.y, actRightBottom.x, actRightBottom.y,
            tgLeftTop.x, tgLeftTop.y, tgRightBottom.x, tgRightBottom.y,
            pipeLeftTop.x, pipeLeftTop.y, pipeRightBottom.x, pipeRightBottom.y);
        // note: we use the MRect to pass points left-top and right-bottom
        entryFaceRects.push_back(MRect(pipeLeftTop, MSize(pipeRightBottom.x, pipeRightBottom.y)), Type2Type<MRect>());
        // Face Pose
        const MINT32 poseX = 0;
        const MINT32 poseY = faceData.fld_rop[index];
        const MINT32 poseZ = faceData.fld_rip[index];
        MY_LOGD("detected face pose orientation, reqNo:%d, faceNum:%d/%d, (x, y, z):(%d, %d, %d)",
            reqNo, index, faceCount, poseX, poseY, poseZ);
        entryPoseOriens.push_back(poseX, Type2Type<MINT32>());    // pose X asix
        entryPoseOriens.push_back(poseY, Type2Type<MINT32>());    // pose Y asix
        entryPoseOriens.push_back(poseZ, Type2Type<MINT32>());    // pose Z asix
    }
    // update to appMeta
    pHalMeta->update(MTK_FEATURE_FACE_RECTANGLES, entryFaceRects);
    pHalMeta->update(MTK_FEATURE_FACE_POSE_ORIENTATIONS, entryPoseOriens);

    return MTRUE;
}

RootNode::FDContainerWraper::
FDContainerWraper(const NSCamHW::HwMatrix& active2SensorMatrix, sp<IFDContainer>& fdReader)
: mActive2TGMatrix(active2SensorMatrix)
, mFDReader(fdReader)
{
    MY_LOGD("ctor:%p", this);
}

RootNode::FDContainerWraper::
~FDContainerWraper()
{
    MY_LOGD("dtor:%p", this);
}

RootNode::RootNode(NodeID_T nid, const char* name, MINT32 policy, MINT32 priority)
    : CamNodeULogHandler(Utils::ULog::MOD_CAPTURE_ROOT)
    , CaptureFeatureNode(nid, name, 0, policy, priority)
    , mFDContainerWraperPtr(nullptr)
    , mDebugLevel(0)
    , mMfnrQueryIndex(-1)
    , mMfnrDecisionIso(-1)
{
    TRACE_FUNC_ENTER();
    this->addWaitQueue(&mRequests);
    mDebugDump = property_get_int32("vendor.debug.camera.bss.dump", 0);
    mEnableBSSOrdering = property_get_int32("vendor.debug.camera.bss.enable", 1);
    mDebugDrop = property_get_int32("vendor.debug.camera.bss.drop", -1);
    mDebugLoadIn = property_get_int32("vendor.debug.camera.dumpin.en", -1);

    MY_LOGD("mDebugDump=%d mEnableBSSOrdering=%d mDebugDrop=%d mDebugLoadIn=%d", mDebugDump, mEnableBSSOrdering, mDebugDrop, mDebugLoadIn);
    TRACE_FUNC_EXIT();
}

RootNode::~RootNode()
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC_EXIT();
}

MBOOL RootNode::onData(DataID id, const RequestPtr& pRequest)
{
    TRACE_FUNC_ENTER();
    MY_LOGD_IF(mLogLevel, "Frame %d: %s arrived", pRequest->getFrameNo(), PathID2Name(id));

    MBOOL ret = MFALSE;
    Mutex::Autolock _l(mLock);
    switch(id)
    {
        case PID_ENQUE:
        {
            const MINT32 frameIndex = pRequest->getPipelineFrameIndex();
            const MBOOL isFirstFrame = pRequest->isPipelineFirstFrame();
            const MBOOL isNeededCachedFD = pRequest->getParameter(PID_FD_CACHED_DATA);
            MY_LOGD_IF(mLogLevel, "updateFaceData info., reqNo:%d, frameIndex:%d, isFirstFrame:%d, isNeededCachedFD:%d",
                pRequest->getRequestNo(), frameIndex, isFirstFrame, isNeededCachedFD);

            if (isFirstFrame && isNeededCachedFD) {
                // note: not tread-saft, in this function, we assert that is just only one thread call it
                if(mFDContainerWraperPtr == nullptr) {
                    mFDContainerWraperPtr = FDContainerWraper::createInstancePtr(pRequest, mSensorIndex);
                }
                //
                auto transferTG2Pipe = [this](const MPoint& in, MPoint& out) -> MVOID
                {
                    if(!mpFOVCalculator->getIsEnable() || !mpFOVCalculator->transform(mSensorIndex, in, out)) {
                        out = in;
                    }
                };
                mFDContainerWraperPtr->updateFaceData(pRequest, transferTG2Pipe);
            }

            if (pRequest->hasParameter(PID_VSDOF_MULTICAM)) {
                auto pInMetaDynamicHndMan = pRequest->getMetadata(MID_MAN_IN_P1_DYNAMIC);
                auto pInMetaDynamicHndSub = pRequest->getMetadata(MID_SUB_IN_P1_DYNAMIC);
                auto pInMetaHal = pRequest->getMetadata(MID_MAN_IN_HAL);
                auto pOutMetaApp = pRequest->getMetadata(MID_MAN_OUT_APP);
                auto pOutMetaPhyAppMan = pRequest->getMetadata(MID_MAN_OUT_APP_PHY);
                auto pOutMetaPhyAppSub = pRequest->getMetadata(MID_SUB_OUT_APP_PHY);

                IMetadata* pInMetaDynamicMan = pInMetaDynamicHndMan->native();
                IMetadata* pInMetaDynamicSub = pInMetaDynamicHndSub->native();
                IMetadata* pHalMeta = (pInMetaHal != nullptr) ? pInMetaHal->native() : nullptr;
                IMetadata* pOAppMeta = (pOutMetaApp != nullptr) ? pOutMetaApp->native() : nullptr;
                IMetadata* pOAppMetaMan = (pOutMetaPhyAppMan != nullptr) ? pOutMetaPhyAppMan->native() : nullptr;
                IMetadata* pOAppMetaSub = (pOutMetaPhyAppSub != nullptr) ? pOutMetaPhyAppSub->native() : nullptr;

                IMetadata::IEntry afRoiMan = pInMetaDynamicMan->entryFor(MTK_3A_FEATURE_AF_ROI);
                IMetadata::IEntry afRoiSub = pInMetaDynamicSub->entryFor(MTK_3A_FEATURE_AF_ROI);

                if (pOAppMeta != nullptr) {
                    IMetadata::setEntry<MINT32>(pOAppMeta, MTK_MULTI_CAM_MASTER_ID, mSensorIndex);
                    if(pHalMeta != nullptr) {
                        IMetadata::IEntry entryFaceRects = pHalMeta->entryFor(MTK_FEATURE_FACE_RECTANGLES);
                        IMetadata::IEntry outEntryFaceRects(MTK_STATISTICS_FACE_RECTANGLES);
                        MY_LOGD("add face rectangle, faceNum:%d", entryFaceRects.count());
                        for (size_t i = 0; i < entryFaceRects.count(); i++) {
                            outEntryFaceRects.push_back(entryFaceRects.itemAt(i, Type2Type<MRect>()), Type2Type<MRect>());
                        }
                        pOAppMeta->update(outEntryFaceRects.tag(), outEntryFaceRects);
                    }
                    else {
                        MY_LOGW("cannot get hal input meta");
                    }
                }
                else {
                    MY_LOGW("cannot get app output meta");
                }

                if (!afRoiMan.isEmpty() && (pOAppMetaMan != nullptr)) {
                    pOAppMetaMan->update(MTK_MULTI_CAM_AF_ROI, afRoiMan);
                }
                else {
                    MY_LOGW("main af region is empty %d main physical output meta is null %d",
                            afRoiMan.isEmpty(), (pOAppMetaMan == nullptr));
                }
                if (!afRoiSub.isEmpty() && (pOAppMetaSub != nullptr)) {
                    pOAppMetaSub->update(MTK_MULTI_CAM_AF_ROI, afRoiSub);
                }
                else {
                    MY_LOGW("sub af region is empty %d sub physical output meta is null %d",
                            afRoiSub.isEmpty(), (pOAppMetaSub == nullptr));
                }
            }

#ifdef SUPPORT_MFNR
            auto isManualBypassBSS = [this](const RequestPtr& pRequest) -> bool {
                auto pInMetaHal = pRequest->getMetadata(MID_MAN_IN_HAL);
                IMetadata* pHalMeta = pInMetaHal->native();
                MUINT8 iBypassBSS = 0;
                if (!IMetadata::getEntry<MUINT8>(pHalMeta, MTK_FEATURE_BSS_BYPASSED, iBypassBSS)) {
                    MY_LOGW("get MTK_FEATURE_BSS_BYPASSED failed, set to 0");
                }

                if(iBypassBSS == MTK_FEATURE_BSS_TO_BE_BYPASSED)
                {
                    MY_LOGD("R/F: %d/%d is BSS-Bypassed.", pRequest->getRequestNo(), pRequest->getFrameNo());
                    return true;
                }
                return false;
            };
            auto isApplyBss = [this](const RequestPtr& pRequest) -> bool {
                if (pRequest->getPipelineFrameCount() <= 1)
                    return false;

                MY_LOGD3("pRequest->getPipelineFrameCount() = %d", pRequest->getPipelineFrameCount());

                auto pInMetaHal = pRequest->getMetadata(MID_MAN_IN_HAL);
                IMetadata* pHalMeta = pInMetaHal->native();
                MINT32 selectedBssCount = 0;
                if (!IMetadata::getEntry<MINT32>(pHalMeta, MTK_FEATURE_BSS_SELECTED_FRAME_COUNT, selectedBssCount)) {
                    MY_LOGW("get MTK_FEATURE_BSS_SELECTED_FRAME_COUNT failed, set to 0");
                }

                if (selectedBssCount < 1)
                    return false;

                if (CC_UNLIKELY(mfll::MfllProperty::getBss() <= 0)) {
                    MY_LOGD("%s: Bypass bss due to force disable by property", __FUNCTION__);
                    return false;
                }

                return true;
            };

            MBOOL isBSSReq = isApplyBss(pRequest);
            MBOOL isBypassBSS = isManualBypassBSS(pRequest);
            // update BSS REQ STATE
            if(!isBSSReq)
                pRequest->addParameter(PID_BSS_REQ_STATE, BSS_STATE_NOT_BSS_REQ);
            else if(isBypassBSS)
                pRequest->addParameter(PID_BSS_REQ_STATE, BSS_STATE_BYPASS_BSS);
            else
                pRequest->addParameter(PID_BSS_REQ_STATE, BSS_STATE_DO_BSS);
            //
            if(isBSSReq)
            {
                mRequests.enque(pRequest);
            } else
#endif

            {
                if (pRequest->hasDelayInference())
                    pRequest->startInference();

                dispatch(pRequest);
            }
            ret = MTRUE;
            break;
        }
        default:
        {
            ret = MFALSE;
            MY_LOGE("unknown data id :%d", id);
            break;
        }
    }

    TRACE_FUNC_EXIT();
    return ret;
}

MBOOL RootNode::onInit()
{
    TRACE_FUNC_ENTER();
    CaptureFeatureNode::onInit();
#ifdef SUPPORT_MFNR
    mDebugLevel = mfll::MfllProperty::readProperty(mfll::Property_LogLevel);
    mDebugLevel = max(property_get_int32("vendor.debug.camera.bss.log", mDebugLevel), mDebugLevel);
#endif
    TRACE_FUNC_EXIT();
    return MTRUE;
}


MBOOL RootNode::onThreadStart()
{
    return MTRUE;
}

MBOOL RootNode::onThreadStop()
{
    return MTRUE;
}

MERROR RootNode::evaluate(NodeID_T nodeId, CaptureFeatureInferenceData& rInfer)
{
    (void) rInfer;
    (void) nodeId;
    return OK;
}

MBOOL RootNode::onThreadLoop()
{
    TRACE_FUNC("Waitloop");
    RequestPtr pRequest;

    CAM_TRACE_CALL();

    if (!waitAllQueue()) {
        return MFALSE;
    }

    {
        Mutex::Autolock _l(mLock);
        if (!mRequests.deque(pRequest)) {
            MY_LOGE("Request deque out of sync");
            return MFALSE;
        } else if (pRequest == NULL) {
            MY_LOGE("Request out of sync");
            return MFALSE;
        }

        onRequestProcess(pRequest);

        if (mbWait && mRequests.size() == 0) {
            mbWait = false;
            mWaitCondition.broadcast();
        }
    }

    return MTRUE;
}

MBOOL RootNode::onAbort(RequestPtr& pRequest)
{
    Mutex::Autolock _l(mLock);

    MBOOL bRestored = MFALSE;

    if (mRequests.size() > 0) {
        mbWait = true;
        MY_LOGI("Wait+:deque R/F Num: %d/%d, request size %zu", pRequest->getRequestNo(), pRequest->getFrameNo(), mRequests.size());
        auto ret = mWaitCondition.waitRelative(mLock, 100000000); // 100msec
        if (ret != OK) {
            MY_LOGW("wait timeout!!");
        }
        MY_LOGI("Wait-");
    }

    MINT32 bypassBSSCount = mvBypassBSSRequest.size();
    auto aborthRequestInsideContatiner = [&] (Vector<RequestPtr>& container)
    {
        auto it = container.begin();
        for (; it != container.end(); it++) {
            if ((*it) != pRequest)
                continue;

            container.erase(it);
#ifdef SUPPORT_MFNR
            if (pRequest->hasFeature(FID_MFNR)) {
                pRequest->addParameter(PID_RESTORED, 1);
                pRequest->mpCallback->onRestored(pRequest);
                bRestored = MTRUE;
                mvRestoredRequests.push_back(pRequest);
                MY_LOGI("Restore R/F Num: %d/%d", pRequest->getRequestNo(), pRequest->getFrameNo());
            } else
#endif
            {
                MY_LOGI("Abort R/F Num: %d/%d", pRequest->getRequestNo(), pRequest->getFrameNo());
                onRequestFinish(pRequest);
            }
            break;
        }
    };

    aborthRequestInsideContatiner(mvPendingRequests);
    aborthRequestInsideContatiner(mvBypassBSSRequest);

#ifdef SUPPORT_MFNR
    MINT32 i4RestoredCount = mvRestoredRequests.size();
    //
    if (mvPendingRequests.size() == 0 && mvBypassBSSRequest.size() == 0 && i4RestoredCount != 0) {
        MY_LOGI("Restored requests[%zu], R/F Num: %d/%d",
                mvRestoredRequests.size(), pRequest->getRequestNo(), pRequest->getFrameNo());

        for (auto pRestored : mvRestoredRequests) {
            pRestored->addParameter(PID_FRAME_COUNT, i4RestoredCount);
            pRestored->addParameter(PID_BSS_BYPASSED_COUNT, bypassBSSCount);
        }
        doBss(mvRestoredRequests);
        mvRestoredRequests.clear();
    }
#endif

    return !bRestored;
}

MBOOL RootNode::onRequestProcess(RequestPtr& pRequest)
{
    this->incExtThreadDependency();

    if (pRequest->isCancelled() && !mbWait) {
        MY_LOGD("Cancel, R/F Num: %d/%d", pRequest->getRequestNo(), pRequest->getFrameNo());
        onRequestFinish(pRequest);
        return MFALSE;
    }

    if(!pRequest->isBypassBSS())
        mvPendingRequests.push_back(pRequest);
    else
        mvBypassBSSRequest.push_back(pRequest);

    auto frameIndex = pRequest->getPipelineFrameIndex();
    auto frameCount = pRequest->getPipelineFrameCount();
    // wait request
    if (mvPendingRequests.size() + mvBypassBSSRequest.size() < (size_t)frameCount) {
        MY_LOGD("BSS: Keep waiting, Index:%d Pending/ByPassBSS/Expected: %zu/%zu/%d",
                frameIndex, mvPendingRequests.size(), mvBypassBSSRequest.size(), frameCount);
    } else {
        MY_LOGD("BSS: Do re-order, Index:%d Pending/ByPassBSS/Expected: %zu/%zu/%d",
                frameIndex, mvPendingRequests.size(), mvBypassBSSRequest.size(), frameCount);

        Vector<RequestPtr> vReadyRequests = mvPendingRequests;
        // add BSS-bypass count parameters
        for(size_t index = 0;index < vReadyRequests.size();++index)
        {
            auto& pRequest = vReadyRequests.editItemAt(index);
            pRequest->addParameter(PID_BSS_BYPASSED_COUNT, mvBypassBSSRequest.size());
        }
        for(size_t index = 0;index < mvBypassBSSRequest.size();++index)
        {
            auto& pRequest = mvBypassBSSRequest.editItemAt(index);
            pRequest->addParameter(PID_BSS_BYPASSED_COUNT, mvBypassBSSRequest.size());
        }
        mvPendingRequests.clear();
#ifdef SUPPORT_MFNR
        doBss(vReadyRequests);
        // dispatch the bypass-bss request
        for(size_t index = 0;index < mvBypassBSSRequest.size();++index)
        {
            auto& pRequest = mvBypassBSSRequest.editItemAt(index);
            MY_LOGD_IF(mLogLevel, "dispatch bypass requests: R/F: %d/%d", pRequest->getRequestNo(), pRequest->getFrameNo());
            onRequestFinish(pRequest);
        }
        mvBypassBSSRequest.clear();
#else
        MY_LOGE("Not support BSS: dispatch");
        return MFALSE;
#endif
    }

    return MTRUE;
}

MVOID RootNode::onRequestFinish(const RequestPtr& pRequest)
{
    if (pRequest->hasDelayInference())
        pRequest->startInference();

    if (mDebugDump) {
        MINT32 uniqueKey = 0;
        MINT32 requestNo = pRequest->getRequestNo();
        MINT32 frameNo = pRequest->getFrameNo();
        MINT32 iso = 0;
        MINT64 exp = 0;

        auto pInMetaHalHnd = pRequest->getMetadata(MID_MAN_IN_HAL);
        auto pInMetaDynamicHnd = pRequest->getMetadata(MID_MAN_IN_P1_DYNAMIC);

        IMetadata* pInMetaHal = pInMetaHalHnd->native();
        IMetadata* pInMetaDynamic = pInMetaDynamicHnd->native();

        tryGetMetadata<MINT32>(pInMetaHal, MTK_PIPELINE_UNIQUE_KEY, uniqueKey);
        tryGetMetadata<MINT32>(pInMetaDynamic, MTK_SENSOR_SENSITIVITY, iso);
        tryGetMetadata<MINT64>(pInMetaDynamic, MTK_SENSOR_EXPOSURE_TIME, exp);
        // convert ns into us
        exp /= 1000;

        char filename[256] = {0};
        FILE_DUMP_NAMING_HINT hint;
        hint.UniqueKey          = uniqueKey;
        hint.RequestNo          = requestNo;
        hint.FrameNo            = frameNo;

        extract_by_SensorOpenId(&hint, mSensorIndex);
        auto DumpRawBuffer = [&](IImageBuffer* pImgBuf, RAW_PORT port, const char* pStr) -> MVOID {
            if (pImgBuf == NULL)
                return;

            extract(&hint, pImgBuf);
            genFileName_RAW(filename, sizeof(filename), &hint, port, pStr);
            pImgBuf->saveToFile(filename);
            MY_LOGD("Dump RAW: %s", filename);
        };
        auto DumpLcsBuffer = [&](IImageBuffer* pImgBuf, const char* pStr) -> MVOID {
            if (pImgBuf == NULL)
                return;

            extract(&hint, pImgBuf);
            genFileName_LCSO(filename, sizeof(filename), &hint, pStr);
            pImgBuf->saveToFile(filename);
            MY_LOGD("Dump LCEI: %s", filename);
        };

        auto pInBufRszHnd = pRequest->getBuffer(BID_MAN_IN_RSZ);
        if (pInBufRszHnd != NULL) {
            IImageBuffer* pInBufRsz = pInBufRszHnd->native();
            String8 str = String8::format("mfll-iso-%d-exp-%" PRId64 "-bfbld-rrzo", iso, exp);
            DumpRawBuffer(pInBufRsz, RAW_PORT_NULL, str.string());
        }

        // Dump imgo and lcso for skip frame
        if (pRequest->hasParameter(PID_DROPPED_FRAME)) {
            String8 str;
            hint.SensorDev = -1;
            MY_LOGD("== dump drop frame(R/F Num: %d/%d) for MFNR == ", requestNo, frameNo);
            auto pInBufLcsHnd = pRequest->getBuffer(BID_MAN_IN_LCS);
            if (pInBufLcsHnd != NULL) {
                IImageBuffer* pInBufLcs = pInBufLcsHnd->native();
                str = String8::format("mfll-iso-%d-exp-%" PRId64 "-bfbld-lcso__%dx%d",
                            iso, exp,
                            pInBufLcs->getImgSize().w,
                            pInBufLcs->getImgSize().h);
                    DumpLcsBuffer(pInBufLcs, str.string());
            }

            auto pInBufFullHnd = pRequest->getBuffer(BID_MAN_IN_FULL);
            if (pInBufFullHnd != NULL) {
                IImageBuffer* pInBufFull = pInBufFullHnd->native();
                String8 str = String8::format("mfll-iso-%d-exp-%" PRId64 "-bfbld-raw", iso, exp);
                DumpRawBuffer(pInBufFull, RAW_PORT_NULL, str.string());
            }
            MY_LOGD("==============================================");
        }
    }
    MY_LOGD_IF(mLogLevel, "dispatch  I/C:%d/%d R/F:%d/%d isCross:%d",
                pRequest->getActiveFrameIndex(), pRequest->getActiveFrameCount(),
                pRequest->getRequestNo(), pRequest->getFrameNo(), pRequest->isCross());
    dispatch(pRequest);
    this->decExtThreadDependency();
}

#ifdef SUPPORT_MFNR
/*******************************************************************************
 *
 ********************************************************************************/
MBOOL RootNode::retrieveGmvInfo(IMetadata* pMetadata, int& x, int& y, MSize& size) const
{
    MBOOL  ret = MTRUE;
    MSize  rrzoSize;
    IMetadata::IEntry entry;
    struct __confidence{
        MINT32 x;
        MINT32 y;
        __confidence() : x(0), y(0) {}
    } confidence;

    /* get size first */
    ret = tryGetMetadata<MSize>(pMetadata, MTK_P1NODE_RESIZER_SIZE, rrzoSize);
    if (ret != MTRUE) {
        MY_LOGE("%s: cannot get rzo size", __FUNCTION__);
        goto lbExit;
    }

    entry = pMetadata->entryFor(MTK_EIS_REGION);

    /* check if a valid EIS_REGION */
    if (entry.count() < LMV_REGION_INDEX_SIZE) {
        MY_LOGE("%s: entry is not a valid LMV_REGION, size = %d",
                __FUNCTION__,
                entry.count());
        ret = MFALSE;
        goto lbExit;
    }

    /* read confidence */
    confidence.x = static_cast<MINT32>(entry.itemAt(LMV_REGION_INDEX_CONFX, Type2Type<MINT32>()));
    confidence.y = static_cast<MINT32>((MINT32)entry.itemAt(LMV_REGION_INDEX_CONFY, Type2Type<MINT32>()));

    /* to read GMV if confidence is enough */
    if (confidence.x > MFC_GMV_CONFX_TH) {
        x = entry.itemAt(LMV_REGION_INDEX_GMVX, Type2Type<MINT32>());
    }

    if (confidence.y > MFC_GMV_CONFY_TH) {
        y = entry.itemAt(LMV_REGION_INDEX_GMVY, Type2Type<MINT32>());
    }

    size = rrzoSize;

    MY_LOGD("LMV info conf(x,y) = (%d, %d), gmv(x, y) = (%d, %d)",
            confidence.x, confidence.y, x, y);

lbExit:
    return ret;
}
/*******************************************************************************
 *
 ********************************************************************************/
RootNode::GMV RootNode::calMotionVector(IMetadata* pMetadata, MBOOL isMain) const
{
    RootNode::GMV        mv;
    MSize               rrzoSize;
    MRect               p1ScalarRgn;
    MBOOL               ret = MTRUE;

    /* to get GMV info and the working resolution */
    ret = retrieveGmvInfo(pMetadata, mv.x, mv.y, rrzoSize);
    if (ret == MTRUE) {
        ret = tryGetMetadata<MRect>(
                pMetadata,
                MTK_P1NODE_SCALAR_CROP_REGION,
                p1ScalarRgn);
    }

    /* if works, mapping it from rzoDomain to MfllCore domain */
    if (ret == MTRUE) {
        /* the first frame, set GMV as zero */
        if (isMain) {
            mv.x = 0;
            mv.y = 0;
        }

        MY_LOGD("GMV(x,y)=(%d,%d), unit based on resized RAW",
                mv.x, mv.y);

        MY_LOGD("p1node scalar crop rgion (width): %d, gmv domain(width): %d",
                p1ScalarRgn.s.w, rrzoSize.w);
        /**
         *  the cropping crops height only, not for width. Hence, just
         *  simply uses width to calculate the ratio.
         */
        float ratio =
            static_cast<float>(p1ScalarRgn.s.w)
            /
            static_cast<float>(rrzoSize.w)
            ;
        MY_LOGD("%s: ratio = %f", __FUNCTION__, ratio);

        // we don't need floating computing because GMV is formated
        // with 8 bits floating point
        mv.x *= ratio;
        mv.y *= ratio;

        /* normalization */
        mv.x = mv.x >> 8;
        mv.y = mv.y >> 8;

        // assume the ability of EIS algo, which may seach near by
        // N pixels only, so if the GMV is more than N pixels,
        // we clip it

        auto CLIP = [](int x, const int n) -> int {
            if (x < -n)     return -n;
            else if(x > n)  return n;
            else            return x;
        };

        // Hence we've already known that search region is 32 by 32
        // pixel based on RRZO domain, we can map it to full size
        // domain and makes clip if it's out-of-boundary.
        int c = static_cast<int>(ratio * 32.0f);
        mv.x = CLIP(mv.x, c);
        mv.y = CLIP(mv.y, c);

        MY_LOGI("GMV'(x,y)=(%d,%d), unit: Mfll domain", mv.x, mv.y);
    }
    return mv;
}

/*******************************************************************************
 *
 ********************************************************************************/
MVOID RootNode::updateBssProcInfo(IImageBuffer* pBuf, BSS_PARAM_STRUCT& p, MINT32 frameNum) const
{
    FUNCTION_SCOPE;

    MSize srcSize(pBuf->getImgSize());

    MINT32 roiPercentage = MF_BSS_ROI_PERCENTAGE;
    MINT32 w = (srcSize.w * roiPercentage + 5) / 100;
    MINT32 h = (srcSize.h * roiPercentage + 5) / 100;
    MINT32 x = (srcSize.w - w) / 2;
    MINT32 y = (srcSize.h - h) / 2;
    MINT32 iBssVerFromNvram = 0;

    #define MAKE_TAG(prefix, tag, id)   prefix##tag##id
    #define MAKE_TUPLE(tag, id)         std::make_tuple(#tag, id)
    #define DECLARE_BSS_ENUM_MAP()      std::map<std::tuple<std::string, int>, MUINT32> enumMap
    #define BUILD_BSS_ENUM_MAP(tag) \
            do { \
                if (enumMap[MAKE_TUPLE(tag,-1)] == 1) break; \
                enumMap[MAKE_TUPLE(tag,-1)] = 1; \
                enumMap[MAKE_TUPLE(tag, 0)] = (MUINT32)MAKE_TAG(CUST_MFLL_BSS_, tag, _00); \
                enumMap[MAKE_TUPLE(tag, 1)] = (MUINT32)MAKE_TAG(CUST_MFLL_BSS_, tag, _01); \
                enumMap[MAKE_TUPLE(tag, 2)] = (MUINT32)MAKE_TAG(CUST_MFLL_BSS_, tag, _02); \
                enumMap[MAKE_TUPLE(tag, 3)] = (MUINT32)MAKE_TAG(CUST_MFLL_BSS_, tag, _03); \
            } while (0)
    #define SET_CUST_MFLL_BSS(tag, idx, value) \
            do { \
                BUILD_BSS_ENUM_MAP(tag); \
                enumMap[MAKE_TUPLE(tag, idx)] = (MUINT32)value; \
            } while (0)
    #define GET_CUST_MFLL_BSS(tag) \
            [&, this]() { \
                BUILD_BSS_ENUM_MAP(tag); \
                return enumMap[MAKE_TUPLE(tag, mSensorIndex)]; \
            }()

    DECLARE_BSS_ENUM_MAP();

#if MTK_CAM_NEW_NVRAM_SUPPORT
    //replace by NVRAM
    if (CC_LIKELY( mMfnrQueryIndex >= 0 && mMfnrDecisionIso >= 0 )) {

        MY_LOGD3("replace BSS parameter by NVRAM data");

        /* read NVRAM for tuning data */
        size_t chunkSize = 0;
        std::shared_ptr<char> pChunk = getNvramChunk(&chunkSize);
        if (CC_UNLIKELY(pChunk == NULL)) {
            MY_LOGE("%s: read NVRAM failed, use default", __FUNCTION__);
        }
        else {
            char *pMutableChunk = const_cast<char*>(pChunk.get());
#if (MFLL_MF_TAG_VERSION == 11)
            FEATURE_NVRAM_BSS_T* pNvram = reinterpret_cast<FEATURE_NVRAM_BSS_T*>(pMutableChunk);

            //basic setting
            iBssVerFromNvram = pNvram->bss_ver;
            SET_CUST_MFLL_BSS(SCALE_FACTOR, mSensorIndex, pNvram->scale_factor);
            SET_CUST_MFLL_BSS(CLIP_TH0, mSensorIndex, pNvram->clip_th0);
            SET_CUST_MFLL_BSS(CLIP_TH1, mSensorIndex, pNvram->clip_th1);
            SET_CUST_MFLL_BSS(CLIP_TH2, mSensorIndex, pNvram->clip_th2);
            SET_CUST_MFLL_BSS(CLIP_TH3, mSensorIndex, pNvram->clip_th3);
            SET_CUST_MFLL_BSS(ZERO, mSensorIndex, pNvram->zero_gmv);
            SET_CUST_MFLL_BSS(ADF_TH, mSensorIndex, pNvram->adf_th);
            SET_CUST_MFLL_BSS(SDF_TH, mSensorIndex, pNvram->sdf_th);
            //YPF
            SET_CUST_MFLL_BSS(YPF_EN, mSensorIndex, pNvram->ypf_en);
            SET_CUST_MFLL_BSS(YPF_FAC, mSensorIndex, pNvram->ypf_fac);
            SET_CUST_MFLL_BSS(YPF_ADJTH, mSensorIndex, pNvram->ypf_adj_th);
            SET_CUST_MFLL_BSS(YPF_DFMED0, mSensorIndex, pNvram->ypf_dfmed0);
            SET_CUST_MFLL_BSS(YPF_DFMED1, mSensorIndex, pNvram->ypf_dfmed1);
            SET_CUST_MFLL_BSS(YPF_TH0, mSensorIndex, pNvram->ypf_th[0]);
            SET_CUST_MFLL_BSS(YPF_TH1, mSensorIndex, pNvram->ypf_th[1]);
            SET_CUST_MFLL_BSS(YPF_TH2, mSensorIndex, pNvram->ypf_th[2]);
            SET_CUST_MFLL_BSS(YPF_TH3, mSensorIndex, pNvram->ypf_th[3]);
            SET_CUST_MFLL_BSS(YPF_TH4, mSensorIndex, pNvram->ypf_th[4]);
            SET_CUST_MFLL_BSS(YPF_TH5, mSensorIndex, pNvram->ypf_th[5]);
            SET_CUST_MFLL_BSS(YPF_TH6, mSensorIndex, pNvram->ypf_th[6]);
            SET_CUST_MFLL_BSS(YPF_TH7, mSensorIndex, pNvram->ypf_th[7]);
            //FD
            SET_CUST_MFLL_BSS(FD_EN, mSensorIndex, pNvram->fd_en);
            SET_CUST_MFLL_BSS(FD_FAC, mSensorIndex, pNvram->fd_fac);
            SET_CUST_MFLL_BSS(FD_FNUM, mSensorIndex, pNvram->fd_fnum);
            //EYE blinking
            SET_CUST_MFLL_BSS(EYE_EN, mSensorIndex, pNvram->eye_en);
            SET_CUST_MFLL_BSS(EYE_CFTH, mSensorIndex, pNvram->eye_cfth);
            SET_CUST_MFLL_BSS(EYE_RATIO0, mSensorIndex, pNvram->eye_ratio0);
            SET_CUST_MFLL_BSS(EYE_RATIO1, mSensorIndex, pNvram->eye_ratio1);
            SET_CUST_MFLL_BSS(EYE_FAC, mSensorIndex, pNvram->eye_fac);
            // BSS 3.0
            SET_CUST_MFLL_BSS(FaceCVTh, mSensorIndex, pNvram->FaceCVTh);
            SET_CUST_MFLL_BSS(GradThL, mSensorIndex, pNvram->GradThL);
            SET_CUST_MFLL_BSS(GradThH, mSensorIndex, pNvram->GradThH);
            SET_CUST_MFLL_BSS(FaceAreaThL00, mSensorIndex, pNvram->FaceAreaThL[0]);
            SET_CUST_MFLL_BSS(FaceAreaThL01, mSensorIndex, pNvram->FaceAreaThL[1]);
            SET_CUST_MFLL_BSS(FaceAreaThH00, mSensorIndex, pNvram->FaceAreaThH[0]);
            SET_CUST_MFLL_BSS(FaceAreaThH01, mSensorIndex, pNvram->FaceAreaThH[1]);
            SET_CUST_MFLL_BSS(APLDeltaTh00, mSensorIndex, pNvram->APLDeltaTh[0]);
            SET_CUST_MFLL_BSS(APLDeltaTh01, mSensorIndex, pNvram->APLDeltaTh[1]);
            SET_CUST_MFLL_BSS(APLDeltaTh02, mSensorIndex, pNvram->APLDeltaTh[2]);
            SET_CUST_MFLL_BSS(APLDeltaTh03, mSensorIndex, pNvram->APLDeltaTh[3]);
            SET_CUST_MFLL_BSS(APLDeltaTh04, mSensorIndex, pNvram->APLDeltaTh[4]);
            SET_CUST_MFLL_BSS(APLDeltaTh05, mSensorIndex, pNvram->APLDeltaTh[5]);
            SET_CUST_MFLL_BSS(APLDeltaTh06, mSensorIndex, pNvram->APLDeltaTh[6]);
            SET_CUST_MFLL_BSS(APLDeltaTh07, mSensorIndex, pNvram->APLDeltaTh[7]);
            SET_CUST_MFLL_BSS(APLDeltaTh08, mSensorIndex, pNvram->APLDeltaTh[8]);
            SET_CUST_MFLL_BSS(APLDeltaTh09, mSensorIndex, pNvram->APLDeltaTh[9]);
            SET_CUST_MFLL_BSS(APLDeltaTh10, mSensorIndex, pNvram->APLDeltaTh[10]);
            SET_CUST_MFLL_BSS(APLDeltaTh11, mSensorIndex, pNvram->APLDeltaTh[11]);
            SET_CUST_MFLL_BSS(APLDeltaTh12, mSensorIndex, pNvram->APLDeltaTh[12]);
            SET_CUST_MFLL_BSS(APLDeltaTh13, mSensorIndex, pNvram->APLDeltaTh[13]);
            SET_CUST_MFLL_BSS(APLDeltaTh14, mSensorIndex, pNvram->APLDeltaTh[14]);
            SET_CUST_MFLL_BSS(APLDeltaTh15, mSensorIndex, pNvram->APLDeltaTh[15]);
            SET_CUST_MFLL_BSS(APLDeltaTh16, mSensorIndex, pNvram->APLDeltaTh[16]);
            SET_CUST_MFLL_BSS(APLDeltaTh17, mSensorIndex, pNvram->APLDeltaTh[17]);
            SET_CUST_MFLL_BSS(APLDeltaTh18, mSensorIndex, pNvram->APLDeltaTh[18]);
            SET_CUST_MFLL_BSS(APLDeltaTh19, mSensorIndex, pNvram->APLDeltaTh[19]);
            SET_CUST_MFLL_BSS(APLDeltaTh20, mSensorIndex, pNvram->APLDeltaTh[20]);
            SET_CUST_MFLL_BSS(APLDeltaTh21, mSensorIndex, pNvram->APLDeltaTh[21]);
            SET_CUST_MFLL_BSS(APLDeltaTh22, mSensorIndex, pNvram->APLDeltaTh[22]);
            SET_CUST_MFLL_BSS(APLDeltaTh23, mSensorIndex, pNvram->APLDeltaTh[23]);
            SET_CUST_MFLL_BSS(APLDeltaTh24, mSensorIndex, pNvram->APLDeltaTh[24]);
            SET_CUST_MFLL_BSS(APLDeltaTh25, mSensorIndex, pNvram->APLDeltaTh[25]);
            SET_CUST_MFLL_BSS(APLDeltaTh26, mSensorIndex, pNvram->APLDeltaTh[26]);
            SET_CUST_MFLL_BSS(APLDeltaTh27, mSensorIndex, pNvram->APLDeltaTh[27]);
            SET_CUST_MFLL_BSS(APLDeltaTh28, mSensorIndex, pNvram->APLDeltaTh[28]);
            SET_CUST_MFLL_BSS(APLDeltaTh29, mSensorIndex, pNvram->APLDeltaTh[29]);
            SET_CUST_MFLL_BSS(APLDeltaTh30, mSensorIndex, pNvram->APLDeltaTh[30]);
            SET_CUST_MFLL_BSS(APLDeltaTh31, mSensorIndex, pNvram->APLDeltaTh[31]);
            SET_CUST_MFLL_BSS(APLDeltaTh32, mSensorIndex, pNvram->APLDeltaTh[32]);
            SET_CUST_MFLL_BSS(GradRatioTh00, mSensorIndex, pNvram->GradRatioTh[0]);
            SET_CUST_MFLL_BSS(GradRatioTh01, mSensorIndex, pNvram->GradRatioTh[1]);
            SET_CUST_MFLL_BSS(GradRatioTh02, mSensorIndex, pNvram->GradRatioTh[2]);
            SET_CUST_MFLL_BSS(GradRatioTh03, mSensorIndex, pNvram->GradRatioTh[3]);
            SET_CUST_MFLL_BSS(GradRatioTh04, mSensorIndex, pNvram->GradRatioTh[4]);
            SET_CUST_MFLL_BSS(GradRatioTh05, mSensorIndex, pNvram->GradRatioTh[5]);
            SET_CUST_MFLL_BSS(GradRatioTh06, mSensorIndex, pNvram->GradRatioTh[6]);
            SET_CUST_MFLL_BSS(GradRatioTh07, mSensorIndex, pNvram->GradRatioTh[7]);
            SET_CUST_MFLL_BSS(EyeDistThL, mSensorIndex, pNvram->EyeDistThL);
            SET_CUST_MFLL_BSS(EyeDistThH, mSensorIndex, pNvram->EyeDistThH);
            SET_CUST_MFLL_BSS(EyeMinWeight, mSensorIndex, pNvram->EyeMinWeight);
#else
            NVRAM_CAMERA_FEATURE_MFLL_STRUCT* pNvram = reinterpret_cast<NVRAM_CAMERA_FEATURE_MFLL_STRUCT*>(pMutableChunk);

            if (CC_LIKELY( pNvram->bss_iso_th0 != 0 )) {
                /* update bad range and bad threshold */
                // do nothing?
                // get memc_noise_level by the current ISO
                if (mMfnrDecisionIso < static_cast<MINT32>(pNvram->bss_iso_th0)) {
                    SET_CUST_MFLL_BSS(CLIP_TH0, mSensorIndex, pNvram->bss_iso0_clip_th0);
                    SET_CUST_MFLL_BSS(CLIP_TH1, mSensorIndex, pNvram->bss_iso0_clip_th1);
                    SET_CUST_MFLL_BSS(CLIP_TH2, mSensorIndex, pNvram->bss_iso0_clip_th2);
                    SET_CUST_MFLL_BSS(CLIP_TH3, mSensorIndex, pNvram->bss_iso0_clip_th3);
                    SET_CUST_MFLL_BSS(ADF_TH,   mSensorIndex, pNvram->bss_iso0_adf_th);
                    SET_CUST_MFLL_BSS(SDF_TH,   mSensorIndex, pNvram->bss_iso0_sdf_th);
                } else if (mMfnrDecisionIso < static_cast<MINT32>(pNvram->bss_iso_th1)) {
                    SET_CUST_MFLL_BSS(CLIP_TH0, mSensorIndex, pNvram->bss_iso1_clip_th0);
                    SET_CUST_MFLL_BSS(CLIP_TH1, mSensorIndex, pNvram->bss_iso1_clip_th1);
                    SET_CUST_MFLL_BSS(CLIP_TH2, mSensorIndex, pNvram->bss_iso1_clip_th2);
                    SET_CUST_MFLL_BSS(CLIP_TH3, mSensorIndex, pNvram->bss_iso1_clip_th3);
                    SET_CUST_MFLL_BSS(ADF_TH,   mSensorIndex, pNvram->bss_iso1_adf_th);
                    SET_CUST_MFLL_BSS(SDF_TH,   mSensorIndex, pNvram->bss_iso1_sdf_th);
                } else if (mMfnrDecisionIso < static_cast<MINT32>(pNvram->bss_iso_th2)) {
                    SET_CUST_MFLL_BSS(CLIP_TH0, mSensorIndex, pNvram->bss_iso2_clip_th0);
                    SET_CUST_MFLL_BSS(CLIP_TH1, mSensorIndex, pNvram->bss_iso2_clip_th1);
                    SET_CUST_MFLL_BSS(CLIP_TH2, mSensorIndex, pNvram->bss_iso2_clip_th2);
                    SET_CUST_MFLL_BSS(CLIP_TH3, mSensorIndex, pNvram->bss_iso2_clip_th3);
                    SET_CUST_MFLL_BSS(ADF_TH,   mSensorIndex, pNvram->bss_iso2_adf_th);
                    SET_CUST_MFLL_BSS(SDF_TH,   mSensorIndex, pNvram->bss_iso2_sdf_th);
                } else if (mMfnrDecisionIso < static_cast<MINT32>(pNvram->bss_iso_th3)) {
                    SET_CUST_MFLL_BSS(CLIP_TH0, mSensorIndex, pNvram->bss_iso3_clip_th0);
                    SET_CUST_MFLL_BSS(CLIP_TH1, mSensorIndex, pNvram->bss_iso3_clip_th1);
                    SET_CUST_MFLL_BSS(CLIP_TH2, mSensorIndex, pNvram->bss_iso3_clip_th2);
                    SET_CUST_MFLL_BSS(CLIP_TH3, mSensorIndex, pNvram->bss_iso3_clip_th3);
                    SET_CUST_MFLL_BSS(ADF_TH,   mSensorIndex, pNvram->bss_iso3_adf_th);
                    SET_CUST_MFLL_BSS(SDF_TH,   mSensorIndex, pNvram->bss_iso3_sdf_th);
                } else {
                    SET_CUST_MFLL_BSS(CLIP_TH0, mSensorIndex, pNvram->bss_iso4_clip_th0);
                    SET_CUST_MFLL_BSS(CLIP_TH1, mSensorIndex, pNvram->bss_iso4_clip_th1);
                    SET_CUST_MFLL_BSS(CLIP_TH2, mSensorIndex, pNvram->bss_iso4_clip_th2);
                    SET_CUST_MFLL_BSS(CLIP_TH3, mSensorIndex, pNvram->bss_iso4_clip_th3);
                    SET_CUST_MFLL_BSS(ADF_TH,   mSensorIndex, pNvram->bss_iso4_adf_th);
                    SET_CUST_MFLL_BSS(SDF_TH,   mSensorIndex, pNvram->bss_iso4_sdf_th);
                }
                MY_LOGD3("%s: bss clip/adf/sdf apply nvram setting.", __FUNCTION__);
            }
#endif
        }
        pChunk = nullptr;
    } else {
        MY_LOGD3("use default values defined in custom/feature/mfnr/camera_custom_mfll.h");
    }
#endif

#if (MFLL_MF_TAG_VERSION == 11)
    //using BSS_PARAM_STRUCT default value if not set.
#else
    ::memset(&p, 0x00, sizeof(BSS_PARAM_STRUCT));
#endif

    p.BSS_ON            = MF_BSS_ON;
    p.BSS_VER           = (iBssVerFromNvram > 0)?iBssVerFromNvram:MF_BSS_VER;
    p.BSS_ROI_WIDTH     = w;
    p.BSS_ROI_HEIGHT    = h;
    p.BSS_ROI_X0        = x;
    p.BSS_ROI_Y0        = y;
    p.BSS_SCALE_FACTOR  = GET_CUST_MFLL_BSS(SCALE_FACTOR);

    p.BSS_CLIP_TH0      = GET_CUST_MFLL_BSS(CLIP_TH0);
    p.BSS_CLIP_TH1      = GET_CUST_MFLL_BSS(CLIP_TH1);
    p.BSS_CLIP_TH2      = GET_CUST_MFLL_BSS(CLIP_TH2);
    p.BSS_CLIP_TH3      = GET_CUST_MFLL_BSS(CLIP_TH3);

    p.BSS_ZERO          = GET_CUST_MFLL_BSS(ZERO);
    p.BSS_FRAME_NUM     = frameNum;
    p.BSS_ADF_TH        = GET_CUST_MFLL_BSS(ADF_TH);
    p.BSS_SDF_TH        = GET_CUST_MFLL_BSS(SDF_TH);

    p.BSS_GAIN_TH0      = GET_CUST_MFLL_BSS(GAIN_TH0);
    p.BSS_GAIN_TH1      = GET_CUST_MFLL_BSS(GAIN_TH1);
    p.BSS_MIN_ISP_GAIN  = GET_CUST_MFLL_BSS(MIN_ISP_GAIN);
    p.BSS_LCSO_SIZE     = 0; // TODO: query lcso size for AE compensation

    p.BSS_YPF_EN        = GET_CUST_MFLL_BSS(YPF_EN);
    p.BSS_YPF_FAC       = GET_CUST_MFLL_BSS(YPF_FAC);
    p.BSS_YPF_ADJTH     = GET_CUST_MFLL_BSS(YPF_ADJTH);
    p.BSS_YPF_DFMED0    = GET_CUST_MFLL_BSS(YPF_DFMED0);
    p.BSS_YPF_DFMED1    = GET_CUST_MFLL_BSS(YPF_DFMED1);
    p.BSS_YPF_TH0       = GET_CUST_MFLL_BSS(YPF_TH0);
    p.BSS_YPF_TH1       = GET_CUST_MFLL_BSS(YPF_TH1);
    p.BSS_YPF_TH2       = GET_CUST_MFLL_BSS(YPF_TH2);
    p.BSS_YPF_TH3       = GET_CUST_MFLL_BSS(YPF_TH3);
    p.BSS_YPF_TH4       = GET_CUST_MFLL_BSS(YPF_TH4);
    p.BSS_YPF_TH5       = GET_CUST_MFLL_BSS(YPF_TH5);
    p.BSS_YPF_TH6       = GET_CUST_MFLL_BSS(YPF_TH6);
    p.BSS_YPF_TH7       = GET_CUST_MFLL_BSS(YPF_TH7);

    p.BSS_FD_EN         = GET_CUST_MFLL_BSS(FD_EN);
    p.BSS_FD_FAC        = GET_CUST_MFLL_BSS(FD_FAC);
    p.BSS_FD_FNUM       = GET_CUST_MFLL_BSS(FD_FNUM);

    p.BSS_EYE_EN        = GET_CUST_MFLL_BSS(EYE_EN);
    p.BSS_EYE_CFTH      = GET_CUST_MFLL_BSS(EYE_CFTH);
    p.BSS_EYE_RATIO0    = GET_CUST_MFLL_BSS(EYE_RATIO0);
    p.BSS_EYE_RATIO1    = GET_CUST_MFLL_BSS(EYE_RATIO1);
    p.BSS_EYE_FAC       = GET_CUST_MFLL_BSS(EYE_FAC);

    //p.BSS_AEVC_EN       = GET_CUST_MFLL_BSS(AEVC_EN);
    //p.BSS_AEVC_DCNT     = GET_CUST_MFLL_BSS(AEVC_DCNT);

#if (MFLL_MF_TAG_VERSION == 11)
    // BSS 3.0
    p.BSS_FaceCVTh          = GET_CUST_MFLL_BSS(FaceCVTh);
    p.BSS_GradThL           = GET_CUST_MFLL_BSS(GradThL);
    p.BSS_GradThH           = GET_CUST_MFLL_BSS(GradThH);
    p.BSS_FaceAreaThL[0]    = GET_CUST_MFLL_BSS(FaceAreaThL00);
    p.BSS_FaceAreaThL[1]    = GET_CUST_MFLL_BSS(FaceAreaThL01);
    p.BSS_FaceAreaThH[0]    = GET_CUST_MFLL_BSS(FaceAreaThH00);
    p.BSS_FaceAreaThH[1]    = GET_CUST_MFLL_BSS(FaceAreaThH01);
    p.BSS_APLDeltaTh[0]     = GET_CUST_MFLL_BSS(APLDeltaTh00);
    p.BSS_APLDeltaTh[1]     = GET_CUST_MFLL_BSS(APLDeltaTh01);
    p.BSS_APLDeltaTh[2]     = GET_CUST_MFLL_BSS(APLDeltaTh02);
    p.BSS_APLDeltaTh[3]     = GET_CUST_MFLL_BSS(APLDeltaTh03);
    p.BSS_APLDeltaTh[4]     = GET_CUST_MFLL_BSS(APLDeltaTh04);
    p.BSS_APLDeltaTh[5]     = GET_CUST_MFLL_BSS(APLDeltaTh05);
    p.BSS_APLDeltaTh[6]     = GET_CUST_MFLL_BSS(APLDeltaTh06);
    p.BSS_APLDeltaTh[7]     = GET_CUST_MFLL_BSS(APLDeltaTh07);
    p.BSS_APLDeltaTh[8]     = GET_CUST_MFLL_BSS(APLDeltaTh08);
    p.BSS_APLDeltaTh[9]     = GET_CUST_MFLL_BSS(APLDeltaTh09);
    p.BSS_APLDeltaTh[10]    = GET_CUST_MFLL_BSS(APLDeltaTh10);
    p.BSS_APLDeltaTh[11]    = GET_CUST_MFLL_BSS(APLDeltaTh11);
    p.BSS_APLDeltaTh[12]    = GET_CUST_MFLL_BSS(APLDeltaTh12);
    p.BSS_APLDeltaTh[13]    = GET_CUST_MFLL_BSS(APLDeltaTh13);
    p.BSS_APLDeltaTh[14]    = GET_CUST_MFLL_BSS(APLDeltaTh14);
    p.BSS_APLDeltaTh[15]    = GET_CUST_MFLL_BSS(APLDeltaTh15);
    p.BSS_APLDeltaTh[16]    = GET_CUST_MFLL_BSS(APLDeltaTh16);
    p.BSS_APLDeltaTh[17]    = GET_CUST_MFLL_BSS(APLDeltaTh17);
    p.BSS_APLDeltaTh[18]    = GET_CUST_MFLL_BSS(APLDeltaTh18);
    p.BSS_APLDeltaTh[19]    = GET_CUST_MFLL_BSS(APLDeltaTh19);
    p.BSS_APLDeltaTh[20]    = GET_CUST_MFLL_BSS(APLDeltaTh20);
    p.BSS_APLDeltaTh[21]    = GET_CUST_MFLL_BSS(APLDeltaTh21);
    p.BSS_APLDeltaTh[22]    = GET_CUST_MFLL_BSS(APLDeltaTh22);
    p.BSS_APLDeltaTh[23]    = GET_CUST_MFLL_BSS(APLDeltaTh23);
    p.BSS_APLDeltaTh[24]    = GET_CUST_MFLL_BSS(APLDeltaTh24);
    p.BSS_APLDeltaTh[25]    = GET_CUST_MFLL_BSS(APLDeltaTh25);
    p.BSS_APLDeltaTh[26]    = GET_CUST_MFLL_BSS(APLDeltaTh26);
    p.BSS_APLDeltaTh[27]    = GET_CUST_MFLL_BSS(APLDeltaTh27);
    p.BSS_APLDeltaTh[28]    = GET_CUST_MFLL_BSS(APLDeltaTh28);
    p.BSS_APLDeltaTh[29]    = GET_CUST_MFLL_BSS(APLDeltaTh29);
    p.BSS_APLDeltaTh[30]    = GET_CUST_MFLL_BSS(APLDeltaTh30);
    p.BSS_APLDeltaTh[31]    = GET_CUST_MFLL_BSS(APLDeltaTh31);
    p.BSS_APLDeltaTh[32]    = GET_CUST_MFLL_BSS(APLDeltaTh32);
    p.BSS_GradRatioTh[0]    = GET_CUST_MFLL_BSS(GradRatioTh00);
    p.BSS_GradRatioTh[1]    = GET_CUST_MFLL_BSS(GradRatioTh01);
    p.BSS_GradRatioTh[2]    = GET_CUST_MFLL_BSS(GradRatioTh02);
    p.BSS_GradRatioTh[3]    = GET_CUST_MFLL_BSS(GradRatioTh03);
    p.BSS_GradRatioTh[4]    = GET_CUST_MFLL_BSS(GradRatioTh04);
    p.BSS_GradRatioTh[5]    = GET_CUST_MFLL_BSS(GradRatioTh05);
    p.BSS_GradRatioTh[6]    = GET_CUST_MFLL_BSS(GradRatioTh06);
    p.BSS_GradRatioTh[7]    = GET_CUST_MFLL_BSS(GradRatioTh07);
    p.BSS_EyeDistThL        = GET_CUST_MFLL_BSS(EyeDistThL);
    p.BSS_EyeDistThH        = GET_CUST_MFLL_BSS(EyeDistThH);
    p.BSS_EyeMinWeight      = GET_CUST_MFLL_BSS(EyeMinWeight);
#endif

    if (CC_UNLIKELY(getForceBss(reinterpret_cast<void*>(&p), sizeof(BSS_PARAM_STRUCT)))) {
        MY_LOGI("%s: force set BSS param as manual setting", __FUNCTION__);
    }

    if (mDebugLevel > 2) {
        String8 str = String8::format("\n======= updateBssProcInfo start ======\n");

        str = str + String8::format("ON(%d) VER(%d) ROI(%d,%d, %dx%d) SCALE(%d)\n",
            p.BSS_ON, p.BSS_VER, p.BSS_ROI_X0, p.BSS_ROI_Y0, p.BSS_ROI_WIDTH, p.BSS_ROI_HEIGHT, p.BSS_SCALE_FACTOR
        );
        str = str + String8::format("CLIP(%d,%d,%d,%d)\n",
            p.BSS_CLIP_TH0, p.BSS_CLIP_TH1, p.BSS_CLIP_TH2, p.BSS_CLIP_TH3
        );
        str = str + String8::format("ZERO(%d) FRAME_NUM(%d) ADF_TH(%d) SDF_TH(%d)\n",
            p.BSS_ZERO, p.BSS_FRAME_NUM, p.BSS_ADF_TH, p.BSS_SDF_TH
        );
        str = str + String8::format("GAIN0(%d) GAIN1(%d) MIN_ISP_GAIN(%d) LCSO_SIZE(%d)\n",
            p.BSS_GAIN_TH0, p.BSS_GAIN_TH1, p.BSS_MIN_ISP_GAIN, p.BSS_LCSO_SIZE
        );
        str = str + String8::format("YPF: EN(%d) FAC(%d) ADJTH(%d) DFMED0(%d) DFMED1(%d)\n",
            p.BSS_YPF_EN, p.BSS_YPF_FAC, p.BSS_YPF_ADJTH, p.BSS_YPF_DFMED0, p.BSS_YPF_DFMED1
        );
        str = str + String8::format("YPF: TH(%d,%d,%d,%d,%d,%d,%d,%d)\n",
            p.BSS_YPF_TH0, p.BSS_YPF_TH1, p.BSS_YPF_TH2, p.BSS_YPF_TH3,
            p.BSS_YPF_TH4, p.BSS_YPF_TH5, p.BSS_YPF_TH6, p.BSS_YPF_TH7
        );
        str = str + String8::format("FD_EN(%d) BSS_FD_FAC(%d) BSS_FD_FNUM(%d)\n",
            p.BSS_FD_EN, p.BSS_FD_FAC, p.BSS_FD_FNUM
        );
        str = str + String8::format("EYE: EN(%d) CFTH(%d) RATIO0(%d) RATIO1(%d) FAC(%d)\n",
            p.BSS_EYE_EN, p.BSS_EYE_CFTH, p.BSS_EYE_RATIO0, p.BSS_EYE_RATIO1, p.BSS_EYE_FAC
        );
        //str = str + String8::format("AEVC: EN(%d) DCNT(%d)\n",
        //    p.BSS_AEVC_EN, p.BSS_AEVC_DCNT
        //);

#if (MFLL_MF_TAG_VERSION == 11)
        // BSS 3.0
        str = str + String8::format("FaceCVTh(%d) GradThL(%d) GradThH(%d)\n",
            p.BSS_FaceCVTh, p.BSS_GradThL, p.BSS_GradThH
        );
        str = str + String8::format("FaceArea ThL0(%d) ThL1(%d) ThH0(%d) ThH1(%d)\n",
            p.BSS_FaceAreaThL[0], p.BSS_FaceAreaThL[1], p.BSS_FaceAreaThH[0], p.BSS_FaceAreaThH[1]
        );
        str = str + String8::format("APLDeltaTh");
        for (size_t i = 0 ; i < 33 ; i++)
            str = str + String8::format(" [%d](%d)", i, p.BSS_APLDeltaTh[i]);
        str = str + String8::format("\n");
        str = str + String8::format("GradRatioTh");
        for (size_t i = 0 ; i < 8 ; i++)
            str = str + String8::format(" [%d](%d)", i, p.BSS_GradRatioTh[i]);
        str = str + String8::format("\n");
        str = str + String8::format("EyeDistThL(%d) EyeDistThH(%d) EyeMinWeight(%d)\n",
            p.BSS_EyeDistThL, p.BSS_EyeDistThH, p.BSS_EyeMinWeight
        );
#endif
        str = str + String8::format("======= updateBssProcInfo end ======\n");
        MY_LOGD("%s", str.string());
    }

}

/*******************************************************************************
 *
 ********************************************************************************/
MVOID RootNode::updateBssIOInfo(IImageBuffer* pBuf, BSS_INPUT_DATA_G& bss_input) const
{
    FUNCTION_SCOPE;

    memset(&bss_input, 0, sizeof(bss_input));

    IHalSensorList* sensorList = MAKE_HalSensorList();
    if(sensorList == NULL){
        MY_LOGE("get sensor Vector failed");
        return;
    }else{
        int sensorDev = sensorList->querySensorDevIdx(mSensorIndex);

        SensorStaticInfo sensorStaticInfo;
        sensorList->querySensorStaticInfo(sensorDev, &sensorStaticInfo);

        bss_input.BayerOrder = sensorStaticInfo.sensorFormatOrder;
        bss_input.Bitnum = [&]() -> MUINT32 {
            switch (sensorStaticInfo.rawSensorBit) {
                case RAW_SENSOR_8BIT:   return 8;
                case RAW_SENSOR_10BIT:  return 10;
                case RAW_SENSOR_12BIT:  return 12;
                case RAW_SENSOR_14BIT:  return 14;
                default:
                    MY_LOGE("get sensor raw bitnum failed");
                    return 0xFF;
            }
        }();
    }

    bss_input.Stride = pBuf->getBufStridesInBytes(0);
    bss_input.inWidth = pBuf->getImgSize().w;
    bss_input.inHeight = pBuf->getImgSize().h;

    if (mDebugLevel > 2) {
        String8 str = String8::format("\n======= updateBssIOInfo start ======\n");
        str = str + String8::format("BayerOrder(%d) Bitnum(%d) Stride(%d) Size(%dx%d)\n",
            bss_input.BayerOrder, bss_input.Bitnum, bss_input.Stride,
            bss_input.inWidth, bss_input.inHeight
        );

        str = str + String8::format("======= updateBssIOInfo end ======\n");
        MY_LOGD("%s", str.string());
    }
}
/*******************************************************************************
 *
 ********************************************************************************/
MBOOL
RootNode::
appendBSSInput(Vector<RequestPtr>& rvRequests, BSS_INPUT_DATA_G& bss_input, vector<FD_DATATYPE>& bss_fddata) const
{
    FUNCTION_SCOPE;

    MINT32 idx = 0;
    vector<int64_t> timestamps(rvRequests.size(), 0);
    MSize imgSize;

    MY_LOGD("appendBSSInput for (%zu) frames", rvRequests.size());
    for (size_t idx = 0; idx < rvRequests.size(); idx++) {
        RequestPtr& request = rvRequests.editItemAt(idx);

        MY_LOGD("idx(%zu) reqNo(%d)", idx, request->getRequestNo());

        auto pInMetaHal = request->getMetadata(MID_MAN_IN_HAL);
        auto pInMetaApp = request->getMetadata(MID_MAN_IN_APP);
        auto pInBufRszHnd = request->getBuffer(BID_MAN_IN_RSZ);

        IMetadata* pHalMeta = pInMetaHal->native();
        IMetadata* pAppMeta = pInMetaApp->native();
        RootNode::GMV mv = calMotionVector(
            pHalMeta,
            (idx == 0) ? MTRUE : MFALSE
        );
        bss_input.gmv[idx].x = mv.x;
        bss_input.gmv[idx].y = mv.y;

        {
            struct T {
                int64_t val;
                MBOOL result;
                T() : val(-1), result(MFALSE) {};
            } timestamp;

        struct R {
            MRect val;
            MBOOL result;
            R() : val(0, 0), result(MFALSE) {};
        } p1ScalarRgn;


#if MTK_CAM_DISPAY_FRAME_CONTROL_ON
            timestamp.result = NSCam::IMetadata::getEntry<int64_t>(pHalMeta, MTK_P1NODE_FRAME_START_TIMESTAMP, timestamp.val);
#else
            timestamp.result = NSCam::IMetadata::getEntry<int64_t>(pAppMeta, MTK_SENSOR_TIMESTAMP, timestamp.val);
#endif

            MY_LOGD3("%s:=========================", __FUNCTION__);
            MY_LOGD3("%s: Get timestamp -> %d, timestamp->: %" PRIi64, __FUNCTION__, timestamp.result, timestamp.val);
            MY_LOGD3("%s:=========================", __FUNCTION__);

            timestamps[idx] = timestamp.val;

            p1ScalarRgn.result = NSCam::IMetadata::getEntry<MRect>(pHalMeta, MTK_P1NODE_SCALAR_CROP_REGION, p1ScalarRgn.val);

            MY_LOGD3("%s:=========================", __FUNCTION__);
            MY_LOGD3("%s: Get p1ScalarRgn -> %d, p1ScalarRgn->: (%d, %d)", __FUNCTION__, p1ScalarRgn.result, p1ScalarRgn.val.s.w, p1ScalarRgn.val.s.h);
            MY_LOGD3("%s:=========================", __FUNCTION__);

            imgSize = p1ScalarRgn.val.s;
        }

        auto pInBufRsz = pInBufRszHnd ? pInBufRszHnd->native() : NULL;
        CHECK_OBJECT(pInBufRsz);

        if (pInBufRsz != NULL)
            bss_input.apbyBssInImg[idx] = (MUINT8*)pInBufRsz->getBufVA(0);

        MY_LOGD("gmv(%d,%d) pBuf(%p) ts(%ld)",
            bss_input.gmv[idx].x, bss_input.gmv[idx].y,
            bss_input.apbyBssInImg[idx],
            timestamps[idx]
        );
    }

    /* set fd info */
    {
        // 1. create IFDContainer instance
        auto fdReader = IFDContainer::createInstance(LOG_TAG,  IFDContainer::eFDContainer_Opt_Read);

        // 2. query fd info by timestamps, fdData must be return after use
        auto fdData = fdReader->queryLock(timestamps);

        if (mfll::MfllProperty::readProperty(mfll::Property_BssFdDump) == 1)
            fdReader->dumpInfo();

        // 3. fill fd info to bss
        {
            if (CC_LIKELY( fdData.size() == rvRequests.size() )) {
                bss_fddata.resize(fdData.size());
                for (size_t idx = 0 ; idx < fdData.size() ; idx++) {
                    if (fdData[idx] != nullptr && imgSize.w > 0 && imgSize.h > 0) {
                        fdData[idx]->clone(bss_fddata[idx]);
                        bss_input.Face[idx] = &bss_fddata[idx].facedata;
                        FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->faces->rect[0], imgSize.w);
                        FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->faces->rect[1], imgSize.h);
                        FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->faces->rect[2], imgSize.w);
                        FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->faces->rect[3], imgSize.h);
                        for (int i = 0 ; i < bss_input.Face[idx]->number_of_faces && i < 15 ; i++) {
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->leyex0[i], imgSize.w);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->leyey0[i], imgSize.h);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->leyex1[i], imgSize.w);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->leyey1[i], imgSize.h);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->reyex0[i], imgSize.w);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->reyey0[i], imgSize.h);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->reyex1[i], imgSize.w);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->reyey1[i], imgSize.h);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->nosex[i], imgSize.w);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->nosey[i], imgSize.h);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->mouthx0[i], imgSize.w);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->mouthy0[i], imgSize.h);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->mouthx1[i], imgSize.w);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->mouthy1[i], imgSize.h);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->leyeux[i], imgSize.w);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->leyeuy[i], imgSize.h);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->leyedx[i], imgSize.w);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->leyedy[i], imgSize.h);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->reyeux[i], imgSize.w);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->reyeuy[i], imgSize.h);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->reyedx[i], imgSize.w);
                            FUNC_TRANS_FD_TO_NOR(bss_input.Face[idx]->reyedy[i], imgSize.h);
                        }
                    } else {
                        bss_input.Face[idx] = nullptr;
                    }
                }
            }
            else {
                MY_LOGE("%s: query fdData size is not sync. input_ts(%zu), query(%zu), expect(%zu)", __FUNCTION__, timestamps.size(), fdData.size(), rvRequests.size());
            }
        }
#if (MFLL_APPLY_RAW_BSS == 1)
        // 4. fill fd rect0 to exif
        {
            MINT32 reqId = rvRequests[0]->getRequestNo();

            ExifWriter writer(PIPE_CLASS_TAG);

            for (size_t i = 0; i < fdData.size(); i++) {
                if (fdData[i] != nullptr) {
                    writer.sendData(reqId, MF_TAG_FD_RECT0_X0_00 + i*MFLLBSS_FD_RECT0_PER_FRAME, (uint32_t)fdData[i]->faces[0].rect[0]);
                    writer.sendData(reqId, MF_TAG_FD_RECT0_Y0_00 + i*MFLLBSS_FD_RECT0_PER_FRAME, (uint32_t)fdData[i]->faces[0].rect[1]);
                    writer.sendData(reqId, MF_TAG_FD_RECT0_X1_00 + i*MFLLBSS_FD_RECT0_PER_FRAME, (uint32_t)fdData[i]->faces[0].rect[2]);
                    writer.sendData(reqId, MF_TAG_FD_RECT0_Y1_00 + i*MFLLBSS_FD_RECT0_PER_FRAME, (uint32_t)fdData[i]->faces[0].rect[3]);
                }
            }
        }
#endif
        // 5. fdData must be return after use
        fdReader->queryUnlock(fdData);
    }

    return MTRUE;
}
/*******************************************************************************
 *
 ********************************************************************************/
std::shared_ptr<char>
RootNode::getNvramChunk(size_t *bufferSize) const
{
    FUNCTION_SCOPE;

    Mutex::Autolock _l(mNvramChunkLock);
#ifdef MTK_CAM_NEW_NVRAM_SUPPORT
#if (MFLL_MF_TAG_VERSION == 11)
    size_t chunkSize = sizeof(FEATURE_NVRAM_BSS_T);
    std::shared_ptr<char> chunk(new char[chunkSize]);
    NVRAM_CAMERA_FEATURE_STRUCT *pNvram;
    MUINT sensorDev = SENSOR_DEV_NONE;

    if (bufferSize)
        *bufferSize = 0;

    {
        IHalSensorList* const pHalSensorList = MAKE_HalSensorList();
        if (pHalSensorList == NULL) {
            MY_LOGE("get IHalSensorList instance failed");
            return nullptr;
        }
        sensorDev = pHalSensorList->querySensorDevIdx(mSensorIndex);
    }

    auto pNvBufUtil = MAKE_NvBufUtil();
    if (pNvBufUtil == NULL) {
        MY_LOGE("pNvBufUtil==0");
        return nullptr;
    }
    auto result = pNvBufUtil->getBufAndRead(
            CAMERA_NVRAM_DATA_FEATURE,
            sensorDev, (void*&)pNvram);
    if (result != 0) {
        MY_LOGE("read buffer chunk fail");
        return nullptr;
    }

    memcpy((void*)chunk.get(), (void*)&pNvram->BSS[mMfnrQueryIndex], chunkSize);
    MY_LOGD3("%s: read FEATURE_NVRAM_BSS_T, size=%zu(byte)",
            __FUNCTION__, chunkSize);
#else
    size_t chunkSize = sizeof(NVRAM_CAMERA_FEATURE_MFLL_STRUCT);
    std::shared_ptr<char> chunk(new char[chunkSize]);
    NVRAM_CAMERA_FEATURE_STRUCT *pNvram;
    MUINT sensorDev = SENSOR_DEV_NONE;

    if (bufferSize)
        *bufferSize = 0;

    {
        IHalSensorList* const pHalSensorList = MAKE_HalSensorList();
        if (pHalSensorList == NULL) {
            MY_LOGE("get IHalSensorList instance failed");
            return nullptr;
        }
        sensorDev = pHalSensorList->querySensorDevIdx(mSensorIndex);
    }

    auto pNvBufUtil = MAKE_NvBufUtil();
    if (pNvBufUtil == NULL) {
        MY_LOGE("pNvBufUtil==0");
        return nullptr;
    }
    auto result = pNvBufUtil->getBufAndRead(
            CAMERA_NVRAM_DATA_FEATURE,
            sensorDev, (void*&)pNvram);
    if (result != 0) {
        MY_LOGE("read buffer chunk fail");
        return nullptr;
    }

    memcpy((void*)chunk.get(), (void*)&pNvram->MFNR[mMfnrQueryIndex], chunkSize);
    MY_LOGD3("%s: read NVRAM_CAMERA_FEATURE_MFLL_STRUCT, size=%zu(byte)",
            __FUNCTION__, chunkSize);
#endif
    if (bufferSize)
        *bufferSize = chunkSize;

    return chunk;
#else
    MY_LOGD3("Not support for NVRAM 2.0");
    return nullptr;
#endif
}
/*******************************************************************************
 *
 ********************************************************************************/

MBOOL
RootNode::getForceBss(void* param_addr, size_t param_size) const
{
    FUNCTION_SCOPE;

    if ( param_size != sizeof(BSS_PARAM_STRUCT)) {
        MY_LOGE("%s: invalid sizeof param, param_size:%zu, sizeof(BSS_PARAM_STRUCT):%zu",
                 __FUNCTION__, param_size, sizeof(BSS_PARAM_STRUCT));
        return false;
    }

    int r = 0;
    bool isForceBssSetting = false;
    BSS_PARAM_STRUCT* param = reinterpret_cast<BSS_PARAM_STRUCT*>(param_addr);

    r = mfll::MfllProperty::readProperty(mfll::Property_BssOn);
    if (r != -1) {
        MY_LOGI("%s: Force BSS_ON = %d (original:%d)", __FUNCTION__, r, param->BSS_ON);
        param->BSS_ON = r;
        isForceBssSetting = true;
    }

    r = mfll::MfllProperty::readProperty(mfll::Property_BssRoiWidth);
    if (r != -1) {
        MY_LOGI("%s: Force BSS_ROI_WIDTH = %d (original:%d)", __FUNCTION__, r, param->BSS_ROI_WIDTH);
        param->BSS_ROI_WIDTH = r;
        isForceBssSetting = true;
    }

    r = mfll::MfllProperty::readProperty(mfll::Property_BssRoiHeight);
    if (r != -1) {
        MY_LOGI("%s: Force BSS_ROI_HEIGHT = %d (original:%d)", __FUNCTION__, r, param->BSS_ROI_HEIGHT);
        param->BSS_ROI_HEIGHT = r;
        isForceBssSetting = true;
    }

    r = mfll::MfllProperty::readProperty(mfll::Property_BssRoiX0);
    if (r != -1) {
        MY_LOGI("%s: Force BSS_ROI_X0 = %d (original:%d)", __FUNCTION__, r, param->BSS_ROI_X0);
        param->BSS_ROI_X0 = r;
        isForceBssSetting = true;
    }

    r = mfll::MfllProperty::readProperty(mfll::Property_BssRoiY0);
    if (r != -1) {
        MY_LOGI("%s: Force BSS_ROI_Y0 = %d (original:%d)", __FUNCTION__, r, param->BSS_ROI_Y0);
        param->BSS_ROI_Y0 = r;
        isForceBssSetting = true;
    }

    r = mfll::MfllProperty::readProperty(mfll::Property_BssScaleFactor);
    if (r != -1) {
        MY_LOGI("%s: Force BSS_SCALE_FACTOR = %d (original:%d)", __FUNCTION__, r, param->BSS_SCALE_FACTOR);
        param->BSS_SCALE_FACTOR = r;
        isForceBssSetting = true;
    }

    r = mfll::MfllProperty::readProperty(mfll::Property_BssClipTh0);
    if (r != -1) {
        MY_LOGI("%s: Force BSS_CLIP_TH0 = %d (original:%d)", __FUNCTION__, r, param->BSS_CLIP_TH0);
        param->BSS_CLIP_TH0 = r;
        isForceBssSetting = true;
    }

    r = mfll::MfllProperty::readProperty(mfll::Property_BssClipTh1);
    if (r != -1) {
        MY_LOGI("%s: Force BSS_CLIP_TH1 = %d (original:%d)", __FUNCTION__, r, param->BSS_CLIP_TH1);
        param->BSS_CLIP_TH1 = r;
        isForceBssSetting = true;
    }

    r = mfll::MfllProperty::readProperty(mfll::Property_BssZero);
    if (r != -1) {
        MY_LOGI("%s: Force BSS_ZERO = %d (original:%d)", __FUNCTION__, r, param->BSS_ZERO);
        param->BSS_ZERO = r;
        isForceBssSetting = true;
    }

    r = mfll::MfllProperty::readProperty(mfll::Property_BssAdfTh);
    if (r != -1) {
        MY_LOGI("%s: Force BSS_ADF_TH = %d (original:%d)", __FUNCTION__, r, param->BSS_ADF_TH);
        param->BSS_ADF_TH = r;
        isForceBssSetting = true;
    }

    r = mfll::MfllProperty::readProperty(mfll::Property_BssSdfTh);
    if (r != -1) {
        MY_LOGI("%s: Force BSS_SDF_TH = %d (original:%d)", __FUNCTION__, r, param->BSS_SDF_TH);
        param->BSS_SDF_TH = r;
        isForceBssSetting = true;
    }

    return isForceBssSetting;
}
/*******************************************************************************
 *
 ********************************************************************************/
MVOID
RootNode::
dumpBssInputData2File(RequestPtr& firstRequest, BSS_PARAM_STRUCT& bss_param, BSS_INPUT_DATA_G& bss_input, BSS_OUTPUT_DATA& bssOutData) const
{
    FUNCTION_SCOPE;

#if (MFLL_APPLY_RAW_BSS == 1)

    auto pInMetaHal = firstRequest->getMetadata(MID_MAN_IN_HAL);
    IMetadata* pHalMeta = pInMetaHal->native();

    // dump info
    MINT32 uniqueKey = 0;

    if (!IMetadata::getEntry<MINT32>(pHalMeta, MTK_PIPELINE_UNIQUE_KEY, uniqueKey)) {
        MY_LOGW("get MTK_PIPELINE_UNIQUE_KEY failed, set to 0");
    }

    //dump binary
    auto dump2Binary = [this](MINT32 uniqueKey, const char* buf, size_t size, string fn) -> bool {
        char filepath[256] = {0};
        snprintf(filepath, sizeof(filepath)-1, "%s/%09d-%04d-%04d-""%s", MFLLBSS_DUMP_PATH, uniqueKey, 0, 0, fn.c_str());
        std::ofstream ofs (filepath, std::ofstream::binary);
        if (!ofs.is_open()) {
            MY_LOGW("dump2Binary: open file(%s) fail", filepath);
            return false;
        }
        ofs.write(buf, size);
        ofs.close();
        return true;
    };

    dump2Binary(uniqueKey, (const char*)(&bss_param), sizeof(struct BSS_PARAM_STRUCT), MFLLBSS_DUMP_BSS_PARAM_FILENAME);
    dump2Binary(uniqueKey, (const char*)(&bss_input), sizeof(struct BSS_INPUT_DATA_G), MFLLBSS_DUMP_BSS_IN_FILENAME);
    dump2Binary(uniqueKey, (const char*)(&bssOutData), sizeof(struct BSS_OUTPUT_DATA), MFLLBSS_DUMP_BSS_OUT_FILENAME);

    //dump FD info
    {
        char filepath[256] = {0};
        snprintf(filepath, sizeof(filepath)-1, "%s/%09d-%04d-%04d-""%s", MFLLBSS_DUMP_PATH, uniqueKey, 0, 0, MFLLBSS_DUMP_FD_FILENAME);

        //dump txt
        std::ofstream ofs (filepath, std::ofstream::out);

        if (!ofs.is_open()) {
            MY_LOGW("%s: open file(%s) fail", __FUNCTION__, filepath);
            return;
        }

#define MFLLBSS_WRITE_TO_FILE(pre, val) ofs << pre << " = " << val << std::endl
#define MFLLBSS_WRITE_ARRAY_TO_FILE(pre, array, size) \
        do { \
            ofs << pre << " = "; \
            for (int i = 0 ; i  < size ; i++) { \
                if (i != size-1) \
                    ofs << array[i] << ","; \
                else \
                    ofs << array[i] << std::endl; \
            } \
            if (size == 0) \
                ofs << std::endl; \
        } while (0)
#define MFLLBSS_WRITE_ARRAY_TO_FILE_CAST(pre, array, size) \
                    do { \
                        ofs << pre << " = "; \
                        for (int i = 0 ; i  < size ; i++) { \
                            if (i != size-1) \
                                ofs << static_cast<int32_t>(array[i]) << ","; \
                            else \
                                ofs << static_cast<int32_t>(array[i]) << std::endl; \
                        } \
                        if (size == 0) \
                            ofs << std::endl; \
                    } while (0)
#define MFLLBSS_WRITE_ARRAY_2D_TO_FILE(pre, array, M, N) \
                    do { \
                        ofs << pre << " = "; \
                        for (int i = 0 ; i  < M ; i++) { \
                            ofs << "{"; \
                            for (int j = 0 ; j  < N ; j++) { \
                                ofs << array[i][j]; \
                                if (j != N-1) \
                                    ofs << ","; \
                            } \
                            if (i != M-1) \
                                ofs << "},"; \
                            else \
                                ofs << "}" << std::endl; \
                        } \
                        if (M == 0) \
                            ofs << std::endl; \
                    } while (0)


        for (int f = 0 ; f < MAX_FRAME_NUM ; f++) {
            MFLLBSS_WRITE_TO_FILE("FD frame ", f);
            MtkCameraFaceMetadata* facedata = bss_input.Face[f];

            if (facedata == nullptr)
                continue;


            MFLLBSS_WRITE_TO_FILE("number_of_faces",            facedata->number_of_faces);

            //MtkCameraFace
            if (facedata->faces != nullptr) {
                MFLLBSS_WRITE_ARRAY_TO_FILE("faces->rect",          facedata->faces->rect, 4);
                MFLLBSS_WRITE_TO_FILE("faces->score",               facedata->faces->score);
                MFLLBSS_WRITE_TO_FILE("faces->id",                  facedata->faces->id);
                MFLLBSS_WRITE_ARRAY_TO_FILE("faces->left_eye",      facedata->faces->left_eye, 2);
                MFLLBSS_WRITE_ARRAY_TO_FILE("faces->right_eye",     facedata->faces->right_eye, 2);
                MFLLBSS_WRITE_ARRAY_TO_FILE("faces->mouth",         facedata->faces->mouth, 2);
            }

            //MtkFaceInfo
            if (facedata->posInfo != nullptr) {
                MFLLBSS_WRITE_TO_FILE("posInfo->rop_dir",           facedata->posInfo->rop_dir);
                MFLLBSS_WRITE_TO_FILE("posInfo->rip_dir",           facedata->posInfo->rip_dir);
            }

            MFLLBSS_WRITE_ARRAY_TO_FILE("faces_type",           facedata->faces_type, 15);
            MFLLBSS_WRITE_ARRAY_2D_TO_FILE("motion",            facedata->motion, 15, 2);
            MFLLBSS_WRITE_TO_FILE("ImgWidth",                   facedata->ImgWidth);
            MFLLBSS_WRITE_TO_FILE("ImgHeight",                  facedata->ImgHeight);
            MFLLBSS_WRITE_ARRAY_TO_FILE("leyex0",               facedata->leyex0, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("leyey0",               facedata->leyey0, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("leyex1",               facedata->leyex1, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("leyey1",               facedata->leyey1, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("reyex0",               facedata->reyex0, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("reyey0",               facedata->reyey0, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("reyex1",               facedata->reyex1, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("reyey1",               facedata->reyey1, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("nosex",                facedata->nosex, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("nosey",                facedata->nosey, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("mouthx0",              facedata->mouthx0, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("mouthy0",              facedata->mouthy0, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("mouthx1",              facedata->mouthx1, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("mouthy1",              facedata->mouthy1, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("leyeux",               facedata->leyeux, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("leyeuy",               facedata->leyeuy, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("leyedx",               facedata->leyedx, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("leyedy",               facedata->leyedy, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("reyeux",               facedata->reyeux, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("reyeuy",               facedata->reyeuy, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("reyedx",               facedata->reyedx, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("reyedy",               facedata->reyedy, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("fa_cv",                facedata->fa_cv, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("fld_rip",              facedata->fld_rip, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("fld_rop",              facedata->fld_rop, 15);
            MFLLBSS_WRITE_ARRAY_2D_TO_FILE("YUVsts",            facedata->YUVsts, 15, 5);
            MFLLBSS_WRITE_ARRAY_TO_FILE_CAST("fld_GenderLabel", facedata->fld_GenderLabel, 15);
            MFLLBSS_WRITE_ARRAY_TO_FILE("fld_GenderInfo",       facedata->fld_GenderInfo, 15);
            MFLLBSS_WRITE_TO_FILE("timestamp",                  facedata->timestamp);
            //MtkCNNFaceInfo
            MFLLBSS_WRITE_TO_FILE("CNNFaces.PortEnable",        facedata->CNNFaces.PortEnable);
            MFLLBSS_WRITE_TO_FILE("CNNFaces.IsTrueFace",        facedata->CNNFaces.IsTrueFace);
            MFLLBSS_WRITE_TO_FILE("CNNFaces.CnnResult0",        facedata->CNNFaces.CnnResult0);
            MFLLBSS_WRITE_TO_FILE("CNNFaces.CnnResult1",        facedata->CNNFaces.CnnResult1);
        }


#undef MFLLBSS_WRITE_TO_FILE
#undef MFLLBSS_WRITE_ARRAY_TO_FILE
#undef MFLLBSS_WRITE_ARRAY_TO_FILE_CAST
#undef MFLLBSS_WRITE_ARRAY_2D_TO_FILE

        ofs.close();
    }
#else
    MY_LOGD3("BSS is not support FD");
#endif

}
/*******************************************************************************
 *
 ********************************************************************************/
MBOOL
RootNode::
doBss(Vector<RequestPtr>& rvReadyRequests)
{
    FUNCTION_SCOPE;

    MBOOL ret = MFALSE;
    MINT32 frameNum = rvReadyRequests.size();

    if (frameNum == 0)
        return MTRUE;
    else if (frameNum == 1) {
        onRequestFinish(rvReadyRequests[0]);
        return MTRUE;
    }

    // Check each frame's required buffer
    Vector<RequestPtr> vInvalidRequests;
    auto it = rvReadyRequests.begin();
    for (; it != rvReadyRequests.end();) {
        auto pRequest = *it;
        if (pRequest->getMetadata(MID_MAN_IN_HAL) == NULL ||
            pRequest->getBuffer(BID_MAN_IN_RSZ) == NULL)
        {
            vInvalidRequests.push_back(pRequest);
            it = rvReadyRequests.erase(it);
            MY_LOGW("Have no required HAL_META(%d) or RRZO(%d), R/F Num: %d/%d",
                    pRequest->getMetadata(MID_MAN_IN_HAL) == NULL,
                    pRequest->getBuffer(BID_MAN_IN_RSZ) == NULL,
                    pRequest->getRequestNo(), pRequest->getFrameNo());
        } else
            it++;
    }

    auto& pMainRequest = rvReadyRequests.editItemAt(0);
    int frameNumToSkip = 0;

    CAM_TRACE_FMT_BEGIN("doBss req(%d)", pMainRequest->getRequestNo());

    BSS_PARAM_STRUCT bss_param;
    BSS_WB_STRUCT workingBufferInfo;
    std::unique_ptr<MUINT8[]> bss_working_buffer;
    Vector<RequestPtr> vOrderedRequests;

    auto pInBufRszHnd = pMainRequest->getBuffer(BID_MAN_IN_RSZ);

    // main frame's input, rrzo
    IImageBuffer* pInBufRsz = pInBufRszHnd ? pInBufRszHnd->native() : NULL;
    CHECK_OBJECT(pInBufRsz);
#if MTK_CAM_NEW_NVRAM_SUPPORT
    // 0. Prepare NVRAM
    {
        // Need to use same index as MFNR if MTK_FEATURE_MFNR_QUERY_INDEX is set
        auto pInMetaHal = pMainRequest->getMetadata(MID_MAN_IN_HAL);
        IMetadata* pHalMeta = pInMetaHal->native();

        mMfnrQueryIndex = -1;
        mMfnrDecisionIso = -1;

        IMetadata::getEntry<MINT32>(pHalMeta, MTK_FEATURE_MFNR_NVRAM_QUERY_INDEX, mMfnrQueryIndex);
        IMetadata::getEntry<MINT32>(pHalMeta, MTK_FEATURE_MFNR_NVRAM_DECISION_ISO, mMfnrDecisionIso);

        MY_LOGD("get MTK_FEATURE_MFNR_NVRAM_QUERY_INDEX = %d and MTK_FEATURE_MFNR_NVRAM_DECISION_ISO = %d", mMfnrQueryIndex, mMfnrDecisionIso);
    }
#else
    MY_LOGD("use default values defined in custom/feature/mfnr/camera_custom_mfll.h");
#endif

    // 1. Create MTK BSS Algo
    MTKBss* pMtkBss = MTKBss::createInstance(DRV_BSS_OBJ_SW);
    if (pMtkBss == NULL) {
        MY_LOGE("%s: create BSS instance failed", __FUNCTION__);
        goto lbExit;
    }

    // 2. pMtkBss->Init
    {
        // thread priority usage
        int _priority = 0;
        int _oripriority = 0;
        int _result = 0;

        // change the current thread's priority, the algorithm threads will inherits
        // this value.
        _priority = mfll::MfllProperty::readProperty(mfll::Property_AlgoThreadsPriority, MFLL_ALGO_THREADS_PRIORITY);
        _oripriority = 0;
        _result = Utils::setThreadPriority(_priority, _oripriority);
        if (CC_UNLIKELY( _result != true )) {
            MY_LOGW("set algo threads priority failed(err=%d)", _result);
        }
        else {
            MY_LOGD3("set algo threads priority to %d", _priority);
        }

        if (pMtkBss->BssInit(NULL, NULL) != S_BSS_OK) {
            MY_LOGE("%s: init MTKBss failed", __FUNCTION__);
            goto lbExit;
        }

        // algorithm threads have been forked,
        // if priority set OK, reset it back to the original one
        if (CC_LIKELY( _result == true )) {
            _result = Utils::setThreadPriority( _oripriority, _oripriority );
            if (CC_UNLIKELY( _result != true )) {
                MY_LOGE("set priority back failed, weird!");
            }
        }
    }

    // 3. pMtkBss->BssFeatureCtrl Setup working buffer
    {
        workingBufferInfo.rProcId    = BSS_PROC2;
        workingBufferInfo.u4Width    = pInBufRsz->getImgSize().w;
        workingBufferInfo.u4Height   = pInBufRsz->getImgSize().h;
        workingBufferInfo.u4FrameNum = frameNum;
        workingBufferInfo.u4WKSize   = 0; //it will return working buffer require size
        workingBufferInfo.pu1BW      = nullptr; // assign working buffer latrer.

        auto b = pMtkBss->BssFeatureCtrl(BSS_FTCTRL_GET_WB_SIZE, (void*)&workingBufferInfo, NULL);
        if (b != S_BSS_OK) {
            MY_LOGE("get working buffer size from MTKBss failed (%d)", (int)b);
            goto lbExit;
        }
        if (workingBufferInfo.u4WKSize <= 0) {
            MY_LOGE("unexpected bss working buffer size: %u", workingBufferInfo.u4WKSize);
            goto lbExit;
        }

        bss_working_buffer = std::unique_ptr<MUINT8[]>(new MUINT8[workingBufferInfo.u4WKSize]{0});
        workingBufferInfo.pu1BW = bss_working_buffer.get(); // assign working buffer for bss algo.

        /* print out bss working buffer information */
        MY_LOGD3("%s: rProcId    = %d", __FUNCTION__, workingBufferInfo.rProcId);
        MY_LOGD3("%s: u4Width    = %u", __FUNCTION__, workingBufferInfo.u4Width);
        MY_LOGD3("%s: u4Height   = %u", __FUNCTION__, workingBufferInfo.u4Height);
        MY_LOGD3("%s: u4FrameNum = %u", __FUNCTION__, workingBufferInfo.u4FrameNum);
        MY_LOGD3("%s: u4WKSize   = %u", __FUNCTION__, workingBufferInfo.u4WKSize);
        MY_LOGD3("%s: pu1BW      = %p", __FUNCTION__, workingBufferInfo.pu1BW);

        b = pMtkBss->BssFeatureCtrl(BSS_FTCTRL_SET_WB_SIZE, (void*)&workingBufferInfo, NULL);
        if (b != S_BSS_OK) {
            MY_LOGE("set working buffer to MTKBss failed, size=%d (%u)",(int)b, workingBufferInfo.u4WKSize);
            goto lbExit;
        }
    }
    {
        updateBssProcInfo(pInBufRsz, bss_param, frameNum);

        auto b = pMtkBss->BssFeatureCtrl(BSS_FTCTRL_SET_PROC_INFO, (void*)&bss_param, NULL);
        if (b != S_BSS_OK) {
            MY_LOGE("Set info to MTKBss failed (%d)", (int)b);
            goto lbExit;
        }
    }

    {
        BSS_INPUT_DATA_G bssInData;
        BSS_OUTPUT_DATA bssOutData;
        vector<FD_DATATYPE> bssFdData;

        memset(&bssInData, 0, sizeof(bssInData));
        memset(&bssOutData, 0, sizeof(bssOutData));

        updateBssIOInfo(pInBufRsz, bssInData);
        appendBSSInput(rvReadyRequests, bssInData, bssFdData);

        collectPreBSSExifData(rvReadyRequests, bss_param, bssInData);

        auto b = pMtkBss->BssMain(BSS_PROC2, &bssInData, &bssOutData);

        // dump bss input info to text file for bss simulation
        if (mfll::MfllProperty::readProperty(mfll::Property_DumpRaw) == 1 || mfll::MfllProperty::readProperty(mfll::Property_DumpSim) == 1)
            dumpBssInputData2File(pMainRequest, bss_param, bssInData, bssOutData);

        if (b != S_BSS_OK) {
            MY_LOGE("MTKBss::Main returns failed (%d)", (int)b);
            goto lbExit;
        }

        // Update number of dropped frame
        {
            frameNumToSkip = bssOutData.i4SkipFrmCnt;

            auto pInMetaHalHnd = pMainRequest->getMetadata(MID_MAN_IN_HAL);
            IMetadata* pInHalMeta = pInMetaHalHnd ? pInMetaHalHnd->native() : NULL;
            // From metadata hint
            MINT32 forceDropNum = -1;
            if (IMetadata::getEntry<MINT32>(pInHalMeta, MTK_FEATURE_BSS_FORCE_DROP_NUM, forceDropNum)) {
                if (CC_UNLIKELY(forceDropNum > -1)) {
                    MY_LOGD("%s: metadata force drop frame count = %d, original bss drop frame = %zu",
                            __FUNCTION__, forceDropNum, frameNumToSkip);
                    frameNumToSkip = static_cast<size_t>(forceDropNum);
                }
            }

            // From adb property
            forceDropNum = mfll::MfllProperty::getDropNum();
            if (CC_UNLIKELY(forceDropNum > -1)) {
                MY_LOGD("%s: adb force drop frame count = %d, original bss drop frame = %d",
                        __FUNCTION__, forceDropNum, frameNumToSkip);
                frameNumToSkip = static_cast<size_t>(forceDropNum);
            }

            // due to capture M and blend N
            MINT32 selectedBssCount = rvReadyRequests.size();
            if (CC_LIKELY( IMetadata::getEntry<MINT32>(pInHalMeta, MTK_FEATURE_BSS_SELECTED_FRAME_COUNT, selectedBssCount) )) {
                int frameNumToSkipBase = static_cast<int>(rvReadyRequests.size()) - static_cast<int>(selectedBssCount);
                if (CC_UNLIKELY(frameNumToSkip < frameNumToSkipBase)) {
                    MY_LOGD("%s: update drop frame count = %d, original drop frame = %d",
                            __FUNCTION__, frameNumToSkipBase, frameNumToSkip);

                    frameNumToSkip = static_cast<size_t>(frameNumToSkipBase);
                }
            }
        }

        Vector<MINT32> vNewOrdering;
        MUINT32 order;
        for (size_t i = 0; i < rvReadyRequests.size(); i++) {
            order = (mEnableBSSOrdering == 0) ? i : bssOutData.originalOrder[i];
            MY_LOGD("BSS output(enable bss:%d) - order(%d)", mEnableBSSOrdering, order);
            vOrderedRequests.push_back(rvReadyRequests[order]);
            // record BSS order
            rvReadyRequests[order]->addParameter(PID_BSS_ORDER, i);
            vNewOrdering.push_back(order);
        }

        collectPostBSSExifData(rvReadyRequests, vNewOrdering, bssOutData);

//TODO: Check combination of VSDOF + MFLL wether required or not
#if 0
        // push Bss result into hal meta, all request have same information:
        // 1. MTK_STEREO_FRAME_PER_CAPTURE
        // 2. MTK_STEREO_BSS_RESULT: the result info set containing 3 integer:
        //                           1.original order
        //                           2.gmv.x
        //                           3.gmv.y
        for(auto &e : rvReadyRequests){
            auto pInMetaHal = e->getMetadata(MID_MAN_IN_HAL);
            IMetadata* pHalMeta = pInMetaHal->native();
            CHECK_OBJECT(pHalMeta);

            // frame per capture
            IMetadata::IEntry entry(MTK_STEREO_FRAME_PER_CAPTURE);
            entry.push_back(vOrderedRequests.size(), Type2Type<MINT32>());
            pHalMeta->update(entry.tag(), entry);

            // bss result
            IMetadata::IEntry entry2(MTK_STEREO_BSS_RESULT);
            for(int bssIdx=0 ; bssIdx<vOrderedRequests.size() ; bssIdx++){
                entry2.push_back(bssOutData.originalOrder[bssIdx], Type2Type<MINT32>());
                entry2.push_back(bssOutData.gmv[bssIdx].x, Type2Type<MINT32>());
                entry2.push_back(bssOutData.gmv[bssIdx].y, Type2Type<MINT32>());
            }
            pHalMeta->update(entry2.tag(), entry2);

            // enable mfb, must be 1
            IMetadata::IEntry entry3(MTK_STEREO_ENABLE_MFB);
            entry3.push_back(1, Type2Type<MINT32>());
            pHalMeta->update(entry3.tag(), entry3);
            // update feature mode to mfnr+bokeh
            IMetadata::IEntry entry4(MTK_STEREO_DCMF_FEATURE_MODE);
            entry4.push_back(MTK_DCMF_FEATURE_MFNR_BOKEH, Type2Type<MINT32>());
            pHalMeta->update(entry4.tag(), entry4);
        }
#endif
    }


    ret = MTRUE;

lbExit:
    if (pMtkBss)
        pMtkBss->destroyInstance();

    // Ignore the invalid request to BSS algo, and treat them as dropped frames
    if (vInvalidRequests.size() > 0) {
        rvReadyRequests.appendVector(vInvalidRequests);
        vOrderedRequests.appendVector(vInvalidRequests);
        frameNumToSkip += vInvalidRequests.size();
    }

    if (mDebugDrop > 0)
        frameNumToSkip = mDebugDrop;

    if (mEnableBSSOrdering == 0)
        reorder(rvReadyRequests, rvReadyRequests, frameNumToSkip);
    else if (!ret)
        reorder(rvReadyRequests, rvReadyRequests, frameNumToSkip);
    else
        reorder(rvReadyRequests, vOrderedRequests, frameNumToSkip);
    CAM_TRACE_FMT_END();

    return ret;
}
/*******************************************************************************
 *
 ********************************************************************************/
MVOID
RootNode::
collectPreBSSExifData(
    Vector<RequestPtr>& rvReadyRequests,
    BSS_PARAM_STRUCT& p,
    BSS_INPUT_DATA_G& bss_input) const
{
    FUNCTION_SCOPE;

    (void)rvReadyRequests;
    (void)p;
    (void)bss_input;

    MINT32 reqId = rvReadyRequests[0]->getRequestNo();

    ExifWriter writer(PIPE_CLASS_TAG);


        /* update debug info */
#if (MFLL_MF_TAG_VERSION > 0)
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_ENABLE             ,(uint32_t)mfll::MfllProperty::getBss());
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_ROI_WIDTH          ,(uint32_t)p.BSS_ROI_WIDTH      );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_ROI_HEIGHT         ,(uint32_t)p.BSS_ROI_HEIGHT     );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_SCALE_FACTOR       ,(uint32_t)p.BSS_SCALE_FACTOR   );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_CLIP_TH0           ,(uint32_t)p.BSS_CLIP_TH0       );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_CLIP_TH1           ,(uint32_t)p.BSS_CLIP_TH1       );

#   if (MFLL_APPLY_RAW_BSS == 1)
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_CLIP_TH2           ,(uint32_t)p.BSS_CLIP_TH2       );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_CLIP_TH3           ,(uint32_t)p.BSS_CLIP_TH3       );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_FRAME_NUM          ,(uint32_t)p.BSS_FRAME_NUM      );
#   endif

    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_ZERO               ,(uint32_t)p.BSS_ZERO           );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_ROI_X0             ,(uint32_t)p.BSS_ROI_X0         );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_ROI_Y0             ,(uint32_t)p.BSS_ROI_Y0         );

#   if (MFLL_MF_TAG_VERSION >= 3)
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_ADF_TH             ,(uint32_t)p.BSS_ADF_TH         );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_SDF_TH             ,(uint32_t)p.BSS_SDF_TH         );
#   endif

#   if (MFLL_APPLY_RAW_BSS == 1)
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_ON                 ,(uint32_t)p.BSS_ON             );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_VER                ,(uint32_t)p.BSS_VER            );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_GAIN_TH0           ,(uint32_t)p.BSS_GAIN_TH0       );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_GAIN_TH1           ,(uint32_t)p.BSS_GAIN_TH1       );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_MIN_ISP_GAIN       ,(uint32_t)p.BSS_MIN_ISP_GAIN   );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_LCSO_SIZE          ,(uint32_t)p.BSS_LCSO_SIZE      );
    /* YPF info */
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_YPF_EN             ,(uint32_t)p.BSS_YPF_EN         );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_YPF_FAC            ,(uint32_t)p.BSS_YPF_FAC        );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_YPF_ADJTH          ,(uint32_t)p.BSS_YPF_ADJTH      );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_YPF_DFMED0         ,(uint32_t)p.BSS_YPF_DFMED0     );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_YPF_DFMED1         ,(uint32_t)p.BSS_YPF_DFMED1     );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_YPF_TH0            ,(uint32_t)p.BSS_YPF_TH0        );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_YPF_TH1            ,(uint32_t)p.BSS_YPF_TH1        );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_YPF_TH2            ,(uint32_t)p.BSS_YPF_TH2        );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_YPF_TH3            ,(uint32_t)p.BSS_YPF_TH3        );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_YPF_TH4            ,(uint32_t)p.BSS_YPF_TH4        );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_YPF_TH5            ,(uint32_t)p.BSS_YPF_TH5        );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_YPF_TH6            ,(uint32_t)p.BSS_YPF_TH6        );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_YPF_TH7            ,(uint32_t)p.BSS_YPF_TH7        );
    /* FD & eye info*/
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_FD_EN              ,(uint32_t)p.BSS_FD_EN          );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_FD_FAC             ,(uint32_t)p.BSS_FD_FAC         );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_FD_FNUM            ,(uint32_t)p.BSS_FD_FNUM        );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_EYE_EN             ,(uint32_t)p.BSS_EYE_EN         );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_EYE_CFTH           ,(uint32_t)p.BSS_EYE_CFTH       );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_EYE_RATIO0         ,(uint32_t)p.BSS_EYE_RATIO0     );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_EYE_RATIO1         ,(uint32_t)p.BSS_EYE_RATIO1     );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_EYE_FAC            ,(uint32_t)p.BSS_EYE_FAC        );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_AEVC_EN            ,(uint32_t)p.BSS_AEVC_EN        );
#   endif

#   if (MFLL_MF_TAG_VERSION == 11)
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_FACECVTH           ,(uint32_t)p.BSS_FaceCVTh       );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_GRADTHL            ,(uint32_t)p.BSS_GradThL        );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_GRADTHH            ,(uint32_t)p.BSS_GradThH        );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_FACEAREATHL0       ,(uint32_t)p.BSS_FaceAreaThL[0] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_FACEAREATHL1       ,(uint32_t)p.BSS_FaceAreaThL[1] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_FACEAREATHH0       ,(uint32_t)p.BSS_FaceAreaThH[0] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_FACEAREATHH1       ,(uint32_t)p.BSS_FaceAreaThH[1] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH0        ,(uint32_t)p.BSS_APLDeltaTh[0]  );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH1        ,(uint32_t)p.BSS_APLDeltaTh[1]  );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH2        ,(uint32_t)p.BSS_APLDeltaTh[2]  );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH3        ,(uint32_t)p.BSS_APLDeltaTh[3]  );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH4        ,(uint32_t)p.BSS_APLDeltaTh[4]  );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH5        ,(uint32_t)p.BSS_APLDeltaTh[5]  );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH6        ,(uint32_t)p.BSS_APLDeltaTh[6]  );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH7        ,(uint32_t)p.BSS_APLDeltaTh[7]  );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH8        ,(uint32_t)p.BSS_APLDeltaTh[8]  );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH9        ,(uint32_t)p.BSS_APLDeltaTh[9]  );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH10       ,(uint32_t)p.BSS_APLDeltaTh[10] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH11       ,(uint32_t)p.BSS_APLDeltaTh[11] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH12       ,(uint32_t)p.BSS_APLDeltaTh[12] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH13       ,(uint32_t)p.BSS_APLDeltaTh[13] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH14       ,(uint32_t)p.BSS_APLDeltaTh[14] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH15       ,(uint32_t)p.BSS_APLDeltaTh[15] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH16       ,(uint32_t)p.BSS_APLDeltaTh[16] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH17       ,(uint32_t)p.BSS_APLDeltaTh[17] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH18       ,(uint32_t)p.BSS_APLDeltaTh[18] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH19       ,(uint32_t)p.BSS_APLDeltaTh[19] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH20       ,(uint32_t)p.BSS_APLDeltaTh[20] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH21       ,(uint32_t)p.BSS_APLDeltaTh[21] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH22       ,(uint32_t)p.BSS_APLDeltaTh[22] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH23       ,(uint32_t)p.BSS_APLDeltaTh[23] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH24       ,(uint32_t)p.BSS_APLDeltaTh[24] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH25       ,(uint32_t)p.BSS_APLDeltaTh[25] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH26       ,(uint32_t)p.BSS_APLDeltaTh[26] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH27       ,(uint32_t)p.BSS_APLDeltaTh[27] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH28       ,(uint32_t)p.BSS_APLDeltaTh[28] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH29       ,(uint32_t)p.BSS_APLDeltaTh[29] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH30       ,(uint32_t)p.BSS_APLDeltaTh[30] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH31       ,(uint32_t)p.BSS_APLDeltaTh[31] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_APLDELTATH32       ,(uint32_t)p.BSS_APLDeltaTh[32] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_GRADRATIOTH0       ,(uint32_t)p.BSS_GradRatioTh[0] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_GRADRATIOTH1       ,(uint32_t)p.BSS_GradRatioTh[1] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_GRADRATIOTH2       ,(uint32_t)p.BSS_GradRatioTh[2] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_GRADRATIOTH3       ,(uint32_t)p.BSS_GradRatioTh[3] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_GRADRATIOTH4       ,(uint32_t)p.BSS_GradRatioTh[4] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_GRADRATIOTH5       ,(uint32_t)p.BSS_GradRatioTh[5] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_GRADRATIOTH6       ,(uint32_t)p.BSS_GradRatioTh[6] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_GRADRATIOTH7       ,(uint32_t)p.BSS_GradRatioTh[7] );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_EYEDISTTHL         ,(uint32_t)p.BSS_EyeDistThL     );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_EYEDISTTHH         ,(uint32_t)p.BSS_EyeDistThH     );
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_EYEMINWEIGHT       ,(uint32_t)p.BSS_EyeMinWeight   );

#       if MTK_CAM_NEW_NVRAM_SUPPORT
        if (CC_LIKELY( mMfnrQueryIndex >= 0 && mMfnrDecisionIso >= 0 )) {
            /* read NVRAM for tuning data */
            size_t chunkSize = 0;
            std::shared_ptr<char> pChunk = getNvramChunk(&chunkSize);
            if (CC_UNLIKELY(pChunk != NULL)) {
                char *pMutableChunk = const_cast<char*>(pChunk.get());

                FEATURE_NVRAM_BSS_T* pNvram = reinterpret_cast<FEATURE_NVRAM_BSS_T*>(pMutableChunk);

                writer.sendData(reqId, (unsigned int)MF_TAG_BSS_EXT_SETTING, (uint32_t)pNvram->ext_setting);
            }
        }
#       endif
#   endif

#endif

    auto makeGmv32bits = [](short x, short y){
        return (uint32_t) y << 16 | (x & 0x0000FFFF);
    };
    for(auto i=0 ; i<rvReadyRequests.size() ; i++){
        if(i >= MAX_GMV_CNT){
            MY_LOGE("gmv count exceeds limitatin(%d)!", MAX_GMV_CNT);
            break;
        }
#if (MFLL_MF_TAG_VERSION > 0)
        writer.sendData(reqId,
            (unsigned int)MF_TAG_GMV_00 + i,
            (uint32_t)makeGmv32bits((short)bss_input.gmv[i].x, (short)bss_input.gmv[i].y)
        );
#endif
    }

    // set all sub requests belong to main request
    set<MINT32> mappingSet;
    for(auto &e : rvReadyRequests){
        mappingSet.insert(e->getRequestNo());
    }
    writer.addReqMapping(reqId, mappingSet);
}
/*******************************************************************************
 *
 ********************************************************************************/
MVOID RootNode::collectPostBSSExifData(
    Vector<RequestPtr>& rvReadyRequests,
    Vector<MINT32>& vNewIndex,
    BSS_OUTPUT_DATA& bss_output)
{
    FUNCTION_SCOPE;

    (void)rvReadyRequests;
    (void)vNewIndex;
    (void)bss_output;

    MINT32 reqId = rvReadyRequests[0]->getRequestNo();
    ExifWriter writer(PIPE_CLASS_TAG);

    /* copy to bss container */
    {
        auto pInMetaHal = rvReadyRequests[0]->getMetadata(MID_MAN_IN_HAL);
        IMetadata* pHalMeta = pInMetaHal->native();

        // dump info
        MINT32 uniqueKey = 0;

        if (!IMetadata::getEntry<MINT32>(pHalMeta, MTK_PIPELINE_UNIQUE_KEY, uniqueKey)) {
            MY_LOGW("get MTK_PIPELINE_UNIQUE_KEY failed, fail to edit BSS conatainer");
        } else {
            MY_LOGD("Write BSS result to conatainer (%d)", uniqueKey);
            auto bssProducer = IBssContainer::createInstance(LOG_TAG,  IBssContainer::eBssContainer_Opt_Write);
            auto bssData = bssProducer->editLock(uniqueKey);

            if (bssData != nullptr)
            {
                memcpy(&(bssData->bssdata), &bss_output, sizeof(bss_output));

                for (size_t idx = 0; idx < rvReadyRequests.size() && idx < MAX_FRAME_NUM; idx++) {
                    RequestPtr& request = rvReadyRequests.editItemAt(idx);

                    auto pInMetaHal = request->getMetadata(MID_MAN_IN_HAL);
                    auto pInMetaApp = request->getMetadata(MID_MAN_IN_APP);
                    IMetadata* pHalMeta = pInMetaHal->native();
                    IMetadata* pAppMeta = pInMetaApp->native();

                    MY_LOGD3("idx(%zu) reqNo(%d))", idx, request->getRequestNo());
                    {
                        struct T {
                            int64_t val;
                            MBOOL result;
                            T() : val(-1), result(MFALSE) {};
                        } timestamp;

#if MTK_CAM_DISPAY_FRAME_CONTROL_ON
                        timestamp.result = NSCam::IMetadata::getEntry<int64_t>(pHalMeta, MTK_P1NODE_FRAME_START_TIMESTAMP, timestamp.val);
#else
                        timestamp.result = NSCam::IMetadata::getEntry<int64_t>(pAppMeta, MTK_SENSOR_TIMESTAMP, timestamp.val);
#endif

                        MY_LOGD3("%s:=========================", __FUNCTION__);
                        MY_LOGD3("%s: Get timestamp -> %d, timestamp->: %" PRIi64, __FUNCTION__, timestamp.result, timestamp.val);
                        MY_LOGD3("%s:=========================", __FUNCTION__);

                        bssData->timestamps[idx] = timestamp.val;
                    }
                }
            }
            bssProducer->editUnlock(bssData);
        }
    }

#if (MFLL_APPLY_RAW_BSS == 1)
    /* bss result score */
    const size_t dbgIdxBssScoreCount = 8; // only 8 scores
    size_t dbgIdxBssScoreMSB = static_cast<size_t>(MF_TAG_BSS_FINAL_SCORE_00_MSB);
    size_t dbgIdxBssScoreLSB = static_cast<size_t>(MF_TAG_BSS_FINAL_SCORE_00_LSB);
    size_t dbgIdxBssSharpScoreMSB = static_cast<size_t>(MF_TAG_BSS_SHARP_SCORE_00_MSB);
    size_t dbgIdxBssSharpScoreLSB = static_cast<size_t>(MF_TAG_BSS_SHARP_SCORE_00_LSB);
    size_t dbgIdxBssOrderMapping = static_cast<size_t>(MF_TAG_BSS_REORDER_MAPPING_00);

    Vector<MINT32> vNewIndexMap;
    vNewIndexMap.resize(vNewIndex.size());
    for (size_t i = 0; i < vNewIndexMap.size(); i++)
        vNewIndexMap.editItemAt(vNewIndex[i]) = i;
#endif

    for (size_t i = 0; i < rvReadyRequests.size(); i++) {
        MY_LOGD("%s: SharpScore[%zu]  = %lld", __FUNCTION__, i, bss_output.SharpScore[i]);
        MY_LOGD("%s: adj1_score[%zu]  = %lld", __FUNCTION__, i, bss_output.adj1_score[i]);
        MY_LOGD("%s: adj2_score[%zu]  = %lld", __FUNCTION__, i, bss_output.adj2_score[i]);
        MY_LOGD("%s: adj3_score[%zu]  = %lld", __FUNCTION__, i, bss_output.adj3_score[i]);
        MY_LOGD("%s: final_score[%zu] = %lld", __FUNCTION__, i, bss_output.final_score[i]);

#if (MFLL_APPLY_RAW_BSS == 1)
        int iBssOrderMapping = vNewIndexMap[i];
        {
            size_t idx = i;
            if (i == 0) {
                idx = vNewIndex[0];
            } else if (i == vNewIndex[0]) {
                idx = 0;
            }
            iBssOrderMapping += (rvReadyRequests[idx]->getFrameNo()%10000)*100;
        }
        MY_LOGD("%s: BssOrderMapping[%zu] = %d", __FUNCTION__, i, iBssOrderMapping);

        /* update final scores */
        if (__builtin_expect( i < dbgIdxBssScoreCount, true )) {
            const long long mask32bits = 0x00000000FFFFFFFF;
            writer.sendData(reqId, dbgIdxBssScoreMSB, (bss_output.final_score[i] >> 32) & mask32bits);
            writer.sendData(reqId, dbgIdxBssScoreLSB, bss_output.final_score[i] & mask32bits);
            writer.sendData(reqId, dbgIdxBssSharpScoreMSB, (bss_output.SharpScore[i] >> 32) & mask32bits);
            writer.sendData(reqId, dbgIdxBssSharpScoreLSB, bss_output.SharpScore[i] & mask32bits);
            writer.sendData(reqId, dbgIdxBssOrderMapping, iBssOrderMapping);
        }
        dbgIdxBssScoreMSB++;
        dbgIdxBssScoreLSB++;
        dbgIdxBssSharpScoreMSB++;
        dbgIdxBssSharpScoreLSB++;
        dbgIdxBssOrderMapping++;
#endif
    }


#if (MFLL_MF_TAG_VERSION > 0)
#   if (MFLL_APPLY_RAW_BSS == 1)
    // bss order
    {
        // encoding for bss order
        /** MF_TAG_BSS_ORDER_IDX
         *
         *  BSS order for top 8 frames (MSB -> LSB)
         *
         *  |     4       |     4       |     4       |     4       |     4       |     4       |     4       |     4       |
         *  | bssOrder[0] | bssOrder[1] | bssOrder[2] | bssOrder[3] | bssOrder[4] | bssOrder[5] | bssOrder[6] | bssOrder[7] |
         */
        uint32_t bssOrder = 0;
        size_t i = 0;

        for ( ; i < vNewIndex.size() && i < 8 ; i++)
            bssOrder = (bssOrder << 4) | ((uint32_t)vNewIndex[i]<0xf?(uint32_t)vNewIndex[i]:0xf);
        for ( ; i < 8 ; i++)
            bssOrder = (bssOrder << 4) | 0xf;

        writer.sendData(reqId, (unsigned int)MF_TAG_BSS_ORDER_IDX           ,bssOrder                     );
    }
#   endif
    writer.sendData(reqId, (unsigned int)MF_TAG_BSS_BEST_IDX            ,(uint32_t)vNewIndex[0]        );
#endif

}

MVOID RootNode::updateMetadata(Vector<RequestPtr>& rvOrderedRequests, size_t i4SkipFrmCnt, MetadataID_T metaId)
{
    if (rvOrderedRequests.empty() || i4SkipFrmCnt >= rvOrderedRequests.size()) {
        MY_LOGE("%s: Cannot update metadata due to no avaliable request, metaId:%d", __FUNCTION__, metaId);
        return;
    }
    sp<MetadataHandle> pInMetaHal = rvOrderedRequests.editItemAt(0)->getMetadata(metaId);
    if (pInMetaHal == NULL) {
        MY_LOGD("%s: no neet to update, cannot get the halMetaHandle, metaId:%d", __FUNCTION__, metaId);
        return;
    }

    IMetadata* pHalMeta = pInMetaHal->native();
    if (pHalMeta == NULL) {
        MY_LOGD("%s: no neet to update, cannot get the halMetaPtr, metaId:%d", __FUNCTION__, metaId);
        return;
    }

    // Fix LSC
    {
        MUINT8 fixLsc = 0;
        if (CC_UNLIKELY( !IMetadata::getEntry<MUINT8>(pHalMeta, MTK_FEATURE_BSS_FIXED_LSC_TBL_DATA, fixLsc) )) {
            MY_LOGD("%s: cannot get MTK_FEATURE_BSS_FIXED_LSC_TBL_DATA, metaId:%d", __FUNCTION__, metaId);
        }
        if (fixLsc) {
            MY_LOGD3("%s: Fixed LSC due to MTK_FEATURE_BSS_FIXED_LSC_TBL_DATA is set, metaId:%d", __FUNCTION__, metaId);
            auto pBestMetaHal = rvOrderedRequests.editItemAt(0)->getMetadata(metaId);
            IMetadata* pBestHalMeta = pBestMetaHal->native();
            IMetadata::Memory prLscData;
            if (CC_LIKELY( IMetadata::getEntry<IMetadata::Memory>(pBestHalMeta, MTK_LSC_TBL_DATA, prLscData) )) {
                for (size_t i = 1 ; i < rvOrderedRequests.size(); i++) {
                    auto pInRefMetaHal = rvOrderedRequests.editItemAt(i)->getMetadata(metaId);
                    IMetadata* pRefHalMeta = pInRefMetaHal->native();
                    IMetadata::setEntry<IMetadata::Memory>(pRefHalMeta, MTK_LSC_TBL_DATA, prLscData);
                }
            } else {
                MY_LOGW("%s: cannot get MTK_LSC_TBL_DATA at idx, metaId:%d", __FUNCTION__, metaId);
            }
        }
    }
    // metadata keep in first
    {
        MUINT8 keepIsp = -1;
        if (CC_UNLIKELY( !IMetadata::getEntry<MUINT8>(pHalMeta, MTK_ISP_P2_TUNING_UPDATE_MODE, keepIsp) )) {
            MY_LOGD("%s: cannot get MTK_ISP_P2_TUNING_UPDATE_MODE, metaId:%d", __FUNCTION__, metaId);
        } else {
            MY_LOGD3("%s: Update keepIsp due to MTK_ISP_P2_TUNING_UPDATE_MODE is set, metaId:%d", __FUNCTION__, metaId);
            for (size_t i = 0 ; i < rvOrderedRequests.size(); i++) {
                auto pInRefMetaHal = rvOrderedRequests.editItemAt(i)->getMetadata(metaId);
                IMetadata* pRefHalMeta = pInRefMetaHal->native();
                IMetadata::setEntry<MUINT8>(pRefHalMeta, MTK_ISP_P2_TUNING_UPDATE_MODE, i?2:0);
            }
        }
    }
    // metadata keep in last
    {
        MUINT8 focusPause = -1;
        if (CC_UNLIKELY( !IMetadata::getEntry<MUINT8>(pHalMeta, MTK_FOCUS_PAUSE, focusPause) )) {
            MY_LOGD("%s: cannot get MTK_FOCUS_PAUSE, metaId:%d", __FUNCTION__, metaId);
        } else {
            MY_LOGD3("%s: Update focusPause due to MTK_FOCUS_PAUSE is set, metaId:%d", __FUNCTION__, metaId);
            size_t resumeIdx = rvOrderedRequests.size() - i4SkipFrmCnt - 1; //i4SkipFrmCnt < rvOrderedRequests.size()
            for (size_t i = 0 ; i < rvOrderedRequests.size(); i++) {
                auto pInRefMetaHal = rvOrderedRequests.editItemAt(i)->getMetadata(metaId);
                IMetadata* pRefHalMeta = pInRefMetaHal->native();
                IMetadata::setEntry<MUINT8>(pRefHalMeta, MTK_FOCUS_PAUSE, (i<resumeIdx)?0:1);
            }
        }
    }
}
#endif //SUPPORT_MFNR
/*******************************************************************************
 *
 ********************************************************************************/
MVOID RootNode::reorder(
    Vector<RequestPtr>& rvRequests, Vector<RequestPtr>& rvOrderedRequests, size_t skipCount)
{

    FUNCTION_SCOPE;
    MY_LOGD("skip count=%zu", skipCount);
    if (rvRequests.size() != rvOrderedRequests.size()) {
        MY_LOGE("input(%zu) != result(%zu)", rvRequests.size(), rvOrderedRequests.size());
        return;
    }
    size_t frameCount = rvOrderedRequests.size();

    if (skipCount >= frameCount) {
        skipCount = frameCount - 1;
    }

    MY_LOGI("BSS: skip frame count: %zu", skipCount);

#ifdef SUPPORT_MFNR
    updateMetadata(rvOrderedRequests, skipCount, MID_MAN_IN_HAL);
    updateMetadata(rvOrderedRequests, skipCount, MID_SUB_IN_HAL);
#endif

    // Switch input buffers with each other. To keep the first request's data path
    // Bind the life cycle of the request with bss input buffers to main request
    if (rvOrderedRequests[0] != rvRequests[0]) {
        MY_LOGD_IF(mLogLevel, "Set cross   %d   %d", rvRequests[0]->getFrameNo(), rvOrderedRequests[0]->getFrameNo());
        rvRequests[0]->setCrossRequest(rvOrderedRequests[0]);
        rvOrderedRequests[0]->setCrossRequest(rvRequests[0]);
    }

#ifdef SUPPORT_MFNR
   // write BSS-related EXIF
   ExifWriter writer(PIPE_CLASS_TAG);
   writer.makeExifFromCollectedData(rvRequests[0]);
#endif

    vector<int> bssForceOrder;
    if (mDebugLoadIn == 2) {
#ifdef SUPPORT_MFNR
        for (size_t i = 0; i < rvRequests.size(); i++) {
            bssForceOrder.push_back(i);
        }
#endif
    }

    const RequestPtr& rpPrimaryRequest = rvRequests.itemAt(0);

    Vector<RequestPtr> vDispatchRequest;
    for (size_t i = 0; i < rvOrderedRequests.size(); i++) {
        const RequestPtr& rpOrderedRequest = rvOrderedRequests.itemAt(i);
        RequestPtr pDispatchRequest;
        if (i == 0)
            pDispatchRequest = rpPrimaryRequest;
        else if (rpOrderedRequest == rpPrimaryRequest)
            pDispatchRequest = rvOrderedRequests.editItemAt(0);
        else
            pDispatchRequest = rpOrderedRequest;

        pDispatchRequest->addParameter(PID_DROPPED_COUNT, skipCount);
        if (i + skipCount >= frameCount)
            pDispatchRequest->addParameter(PID_DROPPED_FRAME, 1);

        if (mDebugLoadIn == 2 && bssForceOrder.size() > i) {
            pDispatchRequest->addParameter(PID_FRAME_INDEX_FORCE_BSS, bssForceOrder[i]);
        }

        if (pDispatchRequest->hasDelayInference()) {
            pDispatchRequest->startInference();
        }
        vDispatchRequest.push_back(pDispatchRequest);
    }

    for (size_t i = 0; i < vDispatchRequest.size(); i++) {
        RequestPtr& pRequest = vDispatchRequest.editItemAt(i);
        MY_LOGD_IF(mLogLevel, "dispatch BSS-reorder request: order:%zu R/F:%d/%d isCross:%d",
                    i, pRequest->getRequestNo(), pRequest->getFrameNo(), pRequest->isCross());
        onRequestFinish(vDispatchRequest.editItemAt(i));
    }
}

} // NSCapture
} // namespace NSFeaturePipe
} // namespace NSCamFeature
} // namespace NSCam
