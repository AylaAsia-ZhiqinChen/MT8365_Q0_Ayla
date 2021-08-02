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
 * MediaTek Inc. (C) 2016. All rights reserved.
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

#include "P2_Util.h"
#include "P2_DebugControl.h"
#define P2_CLASS_TAG    P2Util
#define P2_TRACE        TRACE_P2_UTIL
#include "P2_LogHeader.h"

CAM_ULOG_DECLARE_MODULE_ID(MOD_P2_PROC_COMMON);

#define toBool(x) (!!(x))

#define FORCE_TEST_MDP 0

namespace P2
{

P2Util::SimpleIO::SimpleIO()
    : mResized(MFALSE)
    , mUseLMV(MFALSE)
{
}

MVOID P2Util::SimpleIO::setUseLMV(MBOOL useLMV)
{
    mUseLMV = useLMV;
}

MBOOL P2Util::SimpleIO::hasInput() const
{
    return isValid(mIMGI);
}

MBOOL P2Util::SimpleIO::hasOutput() const
{
    return isValid(mIMG2O) ||
           isValid(mIMG3O) ||
           isValid(mWROTO) ||
           isValid(mWDMAO);
}

MBOOL P2Util::SimpleIO::isResized() const
{
    return mResized;
}

MSize P2Util::SimpleIO::getInputSize() const
{
    MSize size(0, 0);
    if( isValid(mIMGI) )
    {
        IImageBuffer* img = mIMGI->getIImageBufferPtr();
        if(img != NULL)
        {
            size = img->getImgSize();
        }
    }
    return size;
}

MVOID P2Util::SimpleIO::updateResult(MBOOL result) const
{
    if( mIMG2O != NULL ) mIMG2O->updateResult(result);
    if( mWROTO != NULL ) mWROTO->updateResult(result);
    if( mWDMAO != NULL ) mWDMAO->updateResult(result);
}

MVOID P2Util::SimpleIO::dropRecord() const
{
    if( mWROTO != NULL && mWROTO->isRecord() )
    {
        mWROTO->updateResult(MFALSE);
    }
    if( mWDMAO != NULL && mWDMAO->isRecord() )
    {
        mWDMAO->updateResult(MFALSE);
    }
}

MBOOL P2Util::SimpleIO::findDisplayAndRelease(MBOOL result)
{
    MBOOL ret = MFALSE;
    if (mWROTO != nullptr && mWROTO->isDisplay())
    {
        mWROTO->updateResult(result);
        mWROTO = nullptr;
        ret = MTRUE;
    }
    else if (mWDMAO != nullptr && mWDMAO->isDisplay())
    {
        mWDMAO->updateResult(result);
        mWDMAO = nullptr;
        ret = MTRUE;
    }
    return ret;
}

MVOID P2Util::SimpleIO::earlyRelease(MUINT32 mask, MBOOL result)
{
    if( mask & P2Util::RELEASE_DISP )
    {
        mIMGI = NULL;
        mLCEI = NULL;
        if( mWROTO != NULL && mWROTO->isDisplay() )
        {
            mWROTO->updateResult(result);
            mWROTO = NULL;
        }
        if( mWDMAO != NULL && mWDMAO->isDisplay() )
        {
            mWDMAO->updateResult(result);
            mWDMAO = NULL;
        }
    }
    if( mask & P2Util::RELEASE_FD )
    {
        if( mIMG2O != NULL )
        {
            mIMG2O->updateResult(result);
            mIMG2O = NULL;
        }
    }
}

sp<P2Img> P2Util::SimpleIO::getMDPSrc() const
{
    if( mWDMAO != NULL )
    {
        return mWDMAO;
    }
    else if( mWROTO != NULL )
    {
        return mWROTO;
    }
    return NULL;
}

sp<P2Img> P2Util::SimpleIO::getLcso() const
{
    return mLCEI;
}

EPortCapbility toCapability(const IMG_TYPE type)
{
    EPortCapbility cap = EPortCapbility_None;

    if (type == IMG_TYPE_DISPLAY)
    {
        cap = EPortCapbility_Disp;
    }
    else if (type == IMG_TYPE_RECORD)
    {
        cap = EPortCapbility_Rcrd;
    }
    else
    {
        // do nothing
    }
    return cap;
}

P2IO toP2IO(sp<P2Img> img)
{
    P2IO io;
    if( isValid(img) )
    {
        io.mBuffer = img->getIImageBufferPtr();
        io.mCapability = toCapability(img->getImgType());
        io.mTransform = img->getTransform();
    }
    return io;
}

P2IOPack P2Util::SimpleIO::toP2IOPack() const
{
    P2IOPack pack;
    //pack.mMargin = mMargin;
    pack.mFlag |= mResized ? P2Flag::FLAG_RESIZED : 0;
    pack.mFlag |= mUseLMV ? P2Flag::FLAG_LMV : 0;
    //pack.mFlag |= mUseMargin ? P2Flag::FLAG_MARGIN : 0;

    pack.mIMGI = toP2IO(mIMGI);
    pack.mIMG2O = toP2IO(mIMG2O);
    pack.mWDMAO = toP2IO(mWDMAO);
    pack.mWROTO = toP2IO(mWROTO);

    pack.mLCSO = toP2IO(mLCEI);
    return pack;
}

MVOID P2Util::SimpleIO::printIO(const ILog &log) const
{
    MY_S_LOGD(log, "resize(%d),lmv(%d)", mResized, mUseLMV);
    MY_S_LOGD(log, "imgi(%d),lcei(%d),img2o(%d),img3o(%d),wroto(%d),wdmao(%d)",
              isValid(mIMGI), isValid(mLCEI),
              isValid(mIMG2O), isValid(mIMG3O),
              isValid(mWROTO), isValid(mWDMAO));
}

P2Util::SimpleIO P2Util::extractSimpleIO(const sp<P2Request> &request, MUINT32 portFlag)
{
    ILog log = spToILog(request);
    TRACE_S_FUNC_ENTER(log);
    SimpleIO io;
    MBOOL useVenc = !!(portFlag & P2Util::USE_VENC);
    if( isValid(request->mImg[IN_RESIZED]) )
    {
        io.mResized = MTRUE;
        io.mIMGI = std::move(request->mImg[IN_RESIZED]);
    }
    else if( isValid(request->mImg[IN_FULL]) )
    {
        io.mResized = MFALSE;
        io.mIMGI = std::move(request->mImg[IN_FULL]);
    }
    if( isValid(request->mImg[IN_LCSO]) )
    {
        io.mLCEI = std::move(request->mImg[IN_LCSO]);
    }
    if( isValid(request->mImg[IN_LCSHO]) )
    {
        io.mLCSHO = std::move(request->mImg[IN_LCSHO]);
    }
    if( isValid(request->mImg[OUT_FD]) )
    {
        io.mIMG2O = std::move(request->mImg[OUT_FD]);
    }
#if FORCE_TEST_MDP
    io.mWDMAO = useVenc ? NULL : P2Util::extractOut(request, P2Util::FIND_NO_ROTATE);
    if( io.mWDMAO == NULL )
    {
        io.mWROTO = P2Util::extractOut(request, P2Util::FIND_ROTATE);
    }
#else
    io.mWROTO = P2Util::extractOut(request, P2Util::FIND_ROTATE);
    if( io.mWROTO == NULL )
    {
        io.mWROTO = P2Util::extractOut(request, P2Util::FIND_NO_ROTATE);
    }
    io.mWDMAO = useVenc ? NULL : P2Util::extractOut(request, P2Util::FIND_NO_ROTATE);
#endif // FORCE_TEST_MDP
    TRACE_S_FUNC_EXIT(log);
    return io;
}

TuningParam P2Util::xmakeTuning(const P2Pack &p2Pack, const SimpleIO &io, NS3Av3::IHalISP *halISP, P2MetaSet &metaSet)
{
    const ILog &log = p2Pack.mLog;
    TuningParam tuning;
    MetaSet_T inMetaSet, outMetaSet, *pOutMetaSet = NULL;

    inMetaSet.MagicNum = 0;
    inMetaSet.appMeta = metaSet.mInApp;
    inMetaSet.halMeta = metaSet.mInHal;
    pOutMetaSet = metaSet.mHasOutput ? &outMetaSet : NULL;

    void *regBuffer = allocateRegBuffer();
    tuning = ::NSCam::Feature::P2Util::makeTuningParam(log, p2Pack, halISP, inMetaSet, pOutMetaSet, io.isResized(), regBuffer, toIImageBufferPtr(io.mLCEI), -1, NULL, MFALSE, NULL, toIImageBufferPtr(io.mLCSHO));

    if( metaSet.mHasOutput )
    {
        metaSet.mOutApp = outMetaSet.appMeta;
        metaSet.mOutHal = outMetaSet.halMeta;
    }
    return tuning;
}

DIPParams P2Util::xmakeDIPParams(const P2Pack &p2Pack, const SimpleIO &io, const TuningParam &tuning, const P2ObjPtr &p2ObjPtr)
{
    DIPParams dipParams;
    dipParams = ::NSCam::Feature::P2Util::makeDIPParams(p2Pack, ENormalStreamTag_Prv, io.toP2IOPack(), p2ObjPtr, tuning);

    return dipParams;
}

MVOID P2Util::xmakeDpPqParam(const P2Pack &p2Pack, const SimpleOut &out, FD_DATATYPE *&pfdData)
{
    const P2ObjPtr &obj = out.mP2Obj.toPtrTable();

    obj.pqParam->WDMAPQParam = ::NSCam::Feature::P2Util::makeDpPqParam(
        obj.pqWDMA,
        p2Pack,
        toCapability(out.mImg->getImgType())
    );

    if (obj.pqWDMA != NULL)
    {
        DpPqParam *param = obj.pqWDMA;
        DpIspParam &ispParam = param->u.isp;

        if(p2Pack.getConfigInfo().mSupportDRE)
        {
            // set FD info to DpPqParam for DRE
            ispParam.p_faceInfor = (void*) pfdData;
        }
        TRACE_FUNC(
            "=== DpPqParam info dump: START ===\n"
            "SimpleOut(isDisplay=%d, isMDPOutput=%d, Img(ID=%d, getHumanName=%s\n"
            "P2PQIndex=%d, ispProfile=%d\n"
            "SupportCZ-Condition(ConfigDataInfo.mSupportClearZoom=%d, Plat->supportClearZoom=%d, portCapabitity=%d, EPortCapbility_Disp=%d)\n"
            "SupportDRE-Condition(ConfigDataInfo.mSupportDRE=%d, plat->supportDRE=%d)\n"
            "SupportHFG-Condition(ConfigDataInfo.mSupportHFG=%d, plat->supportHFG=%d)\n"
            "DpPqParam(enable=0x%x, scenario=%d)\n"
            "isp(\n"
            "\t iso=%d, lv=%d\n"
            "\t clearZoomParam(captureShot=%d, p_customSetting=%p)\n"
            "\t ispScenario=%d,timestampe=%d,frameNo=%d,requestNo=%d,lensId=%d, userString[0]=%x\n"
            "\t DRE(cmd=%d, userId=%llu, p_customSetting=%p, customIndex=%d,\n"
            "\t     activeArraySize(%d,%d))\n"
            "\t HFG(p_lowerSetting=%p, p_upperSetting=%p, lowerISO=%d, upperISO=%d)\n"
            "\t p_faceInfor=%p\n"
            ")\n"
            "=== DpPqParam info dump: END ==="
            ,
            out.isDisplay(), out.isMDPOutput(), (out.mImg)->getID(), (out.mImg)->getHumanName(),
            p2Pack.getConfigInfo().mUsageHint.mP2PQIndex, p2Pack.getSensorData(p2Pack.getFrameData().mMasterSensorID).mIspProfile,
            p2Pack.getConfigInfo().mSupportClearZoom, p2Pack.getPlatInfo()->supportClearZoom(), toCapability(out.mImg->getImgType()), EPortCapbility_Disp,
            p2Pack.getConfigInfo().mSupportDRE, p2Pack.getPlatInfo()->supportDRE(),
            p2Pack.getConfigInfo().mSupportHFG, p2Pack.getPlatInfo()->supportHFG(),
            param->enable, param->scenario,
            param->u.isp.iso, param->u.isp.LV,
            param->u.isp.clearZoomParam.captureShot, param->u.isp.clearZoomParam.p_customSetting,
            param->u.isp.ispScenario, param->u.isp.timestamp, param->u.isp.frameNo, param->u.isp.requestNo, param->u.isp.lensId, param->u.isp.userString[0],
            param->u.isp.dpDREParam.cmd, param->u.isp.dpDREParam.userId, param->u.isp.dpDREParam.p_customSetting, param->u.isp.dpDREParam.customIndex,
            param->u.isp.dpDREParam.activeWidth, param->u.isp.dpDREParam.activeHeight,
            param->u.isp.dpHFGParam.p_lowerSetting, param->u.isp.dpHFGParam.p_upperSetting, param->u.isp.dpHFGParam.lowerISO, param->u.isp.dpHFGParam.upperISO,
            param->u.isp.p_faceInfor
            );
    }
    else
    {
        TRACE_FUNC("!!warn: obj.pqWDMA == NULL");
    }

}

const char *P2Util::p2PQIdx2String(const MUINT32 pqIdx)
{
    if (pqIdx == P2_PQ_DEFAULT)
    {
        return (const char*)"pq_default";
    }
    else if (pqIdx == P2_PQ_NORMAL)
    {
        return (const char*)"pq_normal";
    }
    else if (pqIdx == P2_PQ_EIS12_VIDEO_4k)
    {
        return (const char*)"pq_eis12_v4k";
    }
    else if (pqIdx == P2_PQ_EIS30_VIDEO)
    {
        return (const char*)"pq_eis30_v";
    }
    else if (pqIdx == P2_PQ_EIS30_VIDEO_4k)
    {
        return (const char*)"pq_eis30_v4k";
    }
    else if (pqIdx == P2_PQ_EIS35_VIDEO)
    {
        return (const char*)"pq_eis35_v";
    }
    else if (pqIdx == P2_PQ_EIS35_VIDEO_4k)
    {
        return (const char*)"pq_eis35_v4k";
    }
    else if (pqIdx == P2_PQ_EIS35_NO_VIDEO)
    {
        return (const char*)"pq_eis35_noV";
    }
    else if (pqIdx == P2_PQ_SMVRCONSTRAINT)
    {
        return (const char*)"pq_smvrC";
    }
    else if (pqIdx == P2_PQ_SMVRBATCH)
    {
        return (const char*)"pq_smvrB";
    }
    else
    {
        return (const char*)"pq_unknown";
    }
}


MVOID P2Util::releaseTuning(TuningParam &tuning)
{
    releaseRegBuffer(tuning.pRegBuf);
    tuning.pRegBuf = NULL;
}

sp<P2Img> P2Util::extractOut(const sp<P2Request> &request, MUINT32 target)
{
    ILog log = spToILog(request);
    TRACE_S_FUNC_ENTER(log);
    sp<P2Img> out;
    MBOOL useRotate = toBool(target & P2Util::FIND_ROTATE);
    MBOOL checkRotate = useRotate != toBool(target & P2Util::FIND_NO_ROTATE);
    MBOOL useDisp = toBool(target & P2Util::FIND_DISP);
    MBOOL useVideo = toBool(target & P2Util::FIND_VIDEO);
    MBOOL checkType = useDisp || useVideo;
    if( request != NULL )
    {
        MSize max(0, 0);
        auto maxIt = request->mImgOutArray.end();
        for( auto it = request->mImgOutArray.begin(), end = request->mImgOutArray.end(); it != end; ++it )
        {
            if( isValid(*it) )
            {
                if( checkRotate &&
                    (useRotate != toBool((*it)->getTransform())) )
                {
                    continue;
                }
                if( checkType &&
                    !(useDisp && toBool((*it)->isDisplay())) &&
                    !(useVideo && toBool((*it)->isRecord())) )
                {
                    continue;
                }
                MSize size = (*it)->getImgSize();
                if( size.w*size.h > max.w*max.h )
                {
                    max = size;
                    maxIt = it;
                }
            }
        }
        if( maxIt != request->mImgOutArray.end() )
        {
            out = *maxIt;
            (*maxIt) = NULL;
        }
    }
    TRACE_S_FUNC_EXIT(log);
    return out;
}

P2Util::SimpleIn::SimpleIn(MUINT32 sensorId, sp<P2Request> pRequest)
: mRequest(pRequest)
, mSensorId(sensorId)
{
}

MUINT32 P2Util::SimpleIn::getSensorId() const
{
    return mSensorId;
}

MVOID P2Util::SimpleIn::setUseLMV(MBOOL useLMV)
{
    mUseLMV = useLMV;
}

MVOID P2Util::SimpleIn::setISResized(MBOOL isResized)
{
    mResized = isResized;
}

MBOOL P2Util::SimpleIn::isResized() const
{
    return mResized;
}

MBOOL P2Util::SimpleIn::useLMV() const
{
    return mUseLMV;
}

MBOOL P2Util::SimpleIn::useCropRatio() const
{
    return mUseCropRatio;
}

MSize P2Util::SimpleIn::getInputSize() const
{
    return mIMGI->getImgSize();
}

sp<P2Img> P2Util::SimpleIn::getLcso() const
{
    return mLCEI;
}

MVOID P2Util::SimpleIn::addCropRatio(const char* name, const float cropRatio)
{
    addCropRatio(name, cropRatio, cropRatio);
}

MVOID P2Util::SimpleIn::addCropRatio(const char* name, const float widthCropRatio, const float heightCropRatio)
{
    TRACE_FUNC_ENTER();
    mUseCropRatio = MTRUE;
    mCropRatio.w *= widthCropRatio;
    mCropRatio.h *= heightCropRatio;
    TRACE_FUNC("%s cropRatio=(%f,%f), total cropRatio=(%f,%f)", name, widthCropRatio, heightCropRatio, mCropRatio.w, mCropRatio.h);
    TRACE_FUNC_EXIT();
}

MBOOL P2Util::SimpleIn::hasCropRatio() const
{
    return mUseCropRatio;
}

MSizeF P2Util::SimpleIn::getCropRatio() const
{
    return mCropRatio;
}

MVOID P2Util::SimpleIn::releaseAllImg()
{
    mIMGI = nullptr;
    mLCEI = nullptr;
    mRSSO = nullptr;
    mPreRSSO = nullptr;
    mFullYuv  = nullptr;
    mRrzYuv1  = nullptr;
    mRrzYuv2  = nullptr;
}

P2Util::SimpleOut::SimpleOut(MUINT32 sensorId, sp<P2Request> pRequest, sp<P2Img>& pImg)
:  mRequest(pRequest)
,  mDMAConstrain(NSCam::Feature::P2Util::DMAConstrain::DEFAULT)
,  mSensorId(sensorId)
{
    mImg = std::move(pImg);
}

MUINT32 P2Util::SimpleOut::getSensorId() const
{
    return mSensorId;
}

MVOID P2Util::SimpleOut::setIsFD(MBOOL isFDBuffer)
{
    mFD = isFDBuffer;
}

MBOOL P2Util::SimpleOut::isDisplay() const
{
    return mImg->isDisplay();
}

MBOOL P2Util::SimpleOut::isRecord() const
{
    return mImg->isRecord();
}

MBOOL P2Util::SimpleOut::isFD() const
{
    return mFD;
}

MBOOL P2Util::SimpleOut::isMDPOutput() const
{
    ID_IMG outID = mImg->getID();
    return (outID == OUT_YUV) || (outID == OUT_JPEG_YUV) || (outID == OUT_THN_YUV);
}


} // namespace P2
