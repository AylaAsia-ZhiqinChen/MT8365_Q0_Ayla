/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2019. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */
#define LOG_TAG "AinrCore/AiAlgo"

#include "AiAlgo.h"

#include <mtkcam/utils/hw/IFDContainer.h>
#include <mtkcam3/feature/utils/ImageBufferUtils.h>
#include <mtkcam3/feature/ainr/AinrUlog.h>


// STD
#include <unordered_map>  // std::unordered_map

// ISP profile
#include <tuning_mapping/cam_idx_struct_ext.h>
// For RAWIspCamInfo
#include <isp_tuning/ver1/isp_tuning_cam_info.h>

// AOSP
#include <utils/String8.h>
#include <cutils/properties.h>

#define AINR_FD_TOLERENCE        (600000000)

using namespace android;
using namespace ainr;
using namespace NSCam;

AiAlgo::AiAlgo() {
    ainrLogD("AiAlgo ctor");
}

AiAlgo::~AiAlgo() {
    ainrLogD("AiAlgo desctructor");
}

void AiAlgo::init(std::shared_ptr<IAinrNvram> nvram) {
    mNvram = nvram;
    return;
}

enum AinrErr AiAlgo::doHG(const AiHgParam &params) {
    CAM_ULOGM_APILIFE();

    std::unique_ptr< MTKHG, std::function<void(MTKHG*)> >
                                pHGDrv = nullptr;
    pHGDrv = decltype(pHGDrv)(
        MTKHG::createInstance(DRV_HG_OBJ_SW),
        [](MTKHG* p) {
            if (CC_UNLIKELY(p == nullptr)) {
                ainrLogW("MTKHG is nullptr");
                return;
            } else {
                p->destroyInstance();
            }
        });
    if (CC_UNLIKELY(pHGDrv == nullptr))
    {
        ainrLogF(" Error: pHGDrv createInstance() fail");
        return AinrErr_UnexpectedError;
    }

    pHGDrv->HGInit(nullptr, nullptr);

    HG_VerInfo rHgVer;
    pHGDrv->HGFeatureCtrl(HG_FTCTRL_GET_VERSION , nullptr, &rHgVer);

    HG_GET_PROC_INFO_IN_STRUCT  rGetProcInfIn;  // only record "i4MaxFP"
    HG_GET_PROC_INFO_OUT_STRUCT rGetProcInfOut;  // only record "i4ReqWMSize"
    HG_SET_PROC_INFO_IN_STRUCT  rSetProcInfIn;
    HG_PROC1_IN_STRUCT          rProc1In;
    HG_PROC1_OUT_STRUCT         rProc1Out;

    // TODO: Need to remove algo magic num 40
    auto baseFeo = params.baseFeBuf;
    auto refFeo  = params.refFeBuf;
    MSize feoSize = baseFeo->getImgSize();
    rGetProcInfIn.i4MaxFP = feoSize.w * feoSize.h /40;
    pHGDrv->HGFeatureCtrl(HG_FTCTRL_GET_PROC_INFO , &rGetProcInfIn, &rGetProcInfOut);

    // Working buffer allocation
    auto wbSize = rGetProcInfOut.i4ReqWMSize;
    auto workingBuffer = std::unique_ptr<char[]>(new char[wbSize]{0});
    rSetProcInfIn.pvWM = reinterpret_cast<void*>(workingBuffer.get());
    rSetProcInfIn.i4WMSize = rGetProcInfOut.i4ReqWMSize;
    pHGDrv->HGFeatureCtrl(HG_FTCTRL_SET_PROC_INFO , &rSetProcInfIn, NULL);

    // Set up size
    auto imgo = params.imgoBuf;
    auto rrzo = params.rrzoBuf;
    MSize imgoSize = imgo->getImgSize();
    MSize rrzoSize = rrzo->getImgSize();
    const MRect& cropRect = params.cropRect;
    rProc1In.i4SrcWidth = imgoSize.w;
    rProc1In.i4SrcHeight = imgoSize.h;
    rProc1In.i4RzWidth = rrzoSize.w;
    rProc1In.i4RzHeight = rrzoSize.h;
    rProc1In.i4WCP = cropRect.p.x;
    rProc1In.i4HCP = cropRect.p.y;
    rProc1In.i4Idx = 0;
    rProc1In.u4Rand = 0;

    auto gridX = params.gridX;
    auto gridY = params.gridY;
    const MSize &gridSize = params.gridMapSize;
    rProc1In.MapWidth = gridSize.w;
    rProc1In.MapHeight = gridSize.h;

    size_t mapAllocSize = 0;
    for (size_t i = 0; i < gridX->getPlaneCount(); i++) {
        mapAllocSize += gridX->getBufSizeInBytes(i);
    }
    rProc1Out.MapAllocSize = static_cast<MINT32>(mapAllocSize);
    ainrLogD("HG WbSize(%d), MapSize(%d)", rGetProcInfOut.i4ReqWMSize, rProc1Out.MapAllocSize);

    auto fmoBase = params.baseFmBuf;
    auto fmoRef  = params.refFmBuf;
    rProc1In.pi2BaseFe = reinterpret_cast<MINT16*>(baseFeo->getBufVA(0));
    rProc1In.pi2NewFe  = reinterpret_cast<MINT16*>(refFeo->getBufVA(0));
    rProc1In.pi2BaseFm = reinterpret_cast<MINT16*>(fmoBase->getBufVA(0));
    rProc1In.pi2NewFm  = reinterpret_cast<MINT16*>(fmoRef->getBufVA(0));

    // Set up homography in data. FEOs and FMOs
    // Allocate genGrid buffer
    rProc1Out.pi4MapX = reinterpret_cast<MINT32*>(gridX->getBufVA(0));
    rProc1Out.pi4MapY = reinterpret_cast<MINT32*>(gridY->getBufVA(0));

    // Start homography
    pHGDrv->HGMain(HG_PROC1, &rProc1In, &rProc1Out);

    gridX->syncCache(eCACHECTRL_INVALID);
    gridY->syncCache(eCACHECTRL_INVALID);

    return AinrErr_Ok;
}

enum AinrErr AiAlgo::refBufUpk(const AiParam &params, const AiRefPackage &bufPackage) {
    // Utils allocate and initial
    CAM_ULOGM_APILIFE();

    std::unique_ptr< MTKNRUtils, std::function<void(MTKNRUtils*)> >
                                pNRUtilsDrv = nullptr;
    pNRUtilsDrv = decltype(pNRUtilsDrv)(
        MTKNRUtils::createInstance(DRV_NRUTILS_OBJ_SW),
        [](MTKNRUtils* p) {
            if (CC_UNLIKELY(p == nullptr)) {
                ainrLogW("UV unpack is nullptr");
                return;
            } else {
                p->destroyInstance();
            }
        });
    if (CC_UNLIKELY(pNRUtilsDrv == nullptr))
    {
        ainrLogE(" Error: MTKNRUtils createInstance() fail");
        return AinrErr_UnexpectedError;
    }

    NRUTILS_VerInfo rUtilsVer;
    pNRUtilsDrv->NRUtilsFeatureCtrl(NRUTILS_FTCTRL_GET_VERSION , nullptr, &rUtilsVer);

    NRUTILS_CONFIG_UNPACK_BAYER_UV_OUT_STRUCT rUnpackUVOut;
    NRUTILS_UNPACK_BAYER_UV_IN_STRUCT rProcIn;
    NRUTILS_UNPACK_BAYER_UV_OUT_STRUCT rProcOut;
    //
    NRUTILS_CONFIG_UNPACK_BAYER_UV_IN_STRUCT rUnpackUVIn;

    rUnpackUVIn.u4CoreNum = 8;
    rUnpackUVIn.i4Width   = params.width;
    rUnpackUVIn.i4Height  = params.height;
    rUnpackUVIn.i4Stride  = params.stride;
    rUnpackUVIn.i4OutBits = params.outBits;
    // rUnpackUVIn.i4Mode = 1; // Used for CPU version of unpack

    // OB and OB ofst
    for (int i = 0; i < 4; i++) {
        rUnpackUVIn.i4ObOffst[i] = params.obOfst[i];
        rUnpackUVIn.i4DgnGain[i] = params.dgnGain[i];
    }

    // config
    pNRUtilsDrv->NRUtilsFeatureCtrl(NRUTILS_FTCTRL_CONFIG_UNPACK_BAYER_UV, &rUnpackUVIn, &rUnpackUVOut);

    // Buffers
    sp<IImageBuffer> refWorkingBuf = nullptr;

    // prepare working buffer
    if (rUnpackUVOut.i4WMSize) {
        NRUTILS_BUFFER_STRUCT &rWM = rProcIn.rWM;
        rWM.Size = rUnpackUVOut.i4WMSize;
        rWM.BufferNum = 1;
        // create working buffer
        ImageBufferUtils::getInstance().allocBuffer(
            refWorkingBuf, rWM.Size, 1, eImgFmt_STA_BYTE);
        if (CC_UNLIKELY( refWorkingBuf.get() == nullptr )) {
            ainrLogE("baseWorkingBuf is null");
            return AinrErr_UnexpectedError;
        }
        rWM.pBuff[0]  = reinterpret_cast<void*>(refWorkingBuf->getBufVA(0));
        rWM.BuffFD[0] = refWorkingBuf->getFD(0);
    } else {
        ainrLogD("No need to allocate working buffer");
    }

    // Input1 for RG buffer
    const auto rgBuf = bufPackage.rgBuf;
    size_t rgBufSize = 0;
    for (size_t i = 0; i < rgBuf->getPlaneCount(); i++) {
        rgBufSize += rgBuf->getBufSizeInBytes(i);
    }
    rProcIn.rInput1.Size = rgBufSize;
    rProcIn.rInput1.BufferNum = 1;
    rProcIn.rInput1.pBuff[0]  = reinterpret_cast<void*>(rgBuf->getBufVA(0));
    rProcIn.rInput1.BuffFD[0] = rgBuf->getFD(0);

    // Input2 for GB buffer
    const auto gbBuf = bufPackage.gbBuf;
    size_t gbBufSize = 0;
    for (size_t i = 0; i < gbBuf->getPlaneCount(); i++) {
        gbBufSize += gbBuf->getBufSizeInBytes(i);
    }
    rProcIn.rInput2.Size = gbBufSize;
    rProcIn.rInput2.BufferNum = 1;
    rProcIn.rInput2.pBuff[0] = reinterpret_cast<void*>(gbBuf->getBufVA(0));
    rProcIn.rInput2.BuffFD[0] = gbBuf->getFD(0);

    // create output buffer
    const auto outBuf = bufPackage.outBuf;
    size_t outBufSize = 0;
    for (size_t i = 0; i < outBuf->getPlaneCount(); i++) {
        outBufSize += outBuf->getBufSizeInBytes(i);
    }
    rProcOut.rOutput.Size = outBufSize;
    rProcOut.rOutput.BufferNum = 1;
    rProcOut.rOutput.pBuff[0]  = reinterpret_cast<void*>(outBuf->getBufVA(0));
    rProcOut.rOutput.BuffFD[0] = outBuf->getFD(0);
    ainrLogD("RG bufferSize(%lu), GB bufferSize(%lu)", rgBufSize, gbBufSize);

    // process
    pNRUtilsDrv->NRUtilsFeatureCtrl(NRUTILS_FTCTRL_PROC_UNPACK_BAYER_UV, &rProcIn, &rProcOut);

    //
    ImageBufferUtils::getInstance().deallocBuffer(refWorkingBuf.get());

    return AinrErr_Ok;
}

enum AinrErr AiAlgo::baseBufUpk(const AiParam &params, const AiBasePackage &bufPackage) {
    CAM_ULOGM_APILIFE();

    std::unique_ptr< MTKNRUtils, std::function<void(MTKNRUtils*)> >
                                pNRUtilsDrv = nullptr;
    pNRUtilsDrv = decltype(pNRUtilsDrv)(
        MTKNRUtils::createInstance(DRV_NRUTILS_OBJ_SW),
        [](MTKNRUtils* p) {
            if (CC_UNLIKELY(p == nullptr)) {
                ainrLogW("Base unpack is nullptr");
                return;
            } else {
                ainrLogD("Destroy base unpack");
                p->destroyInstance();
            }
        });
    if (CC_UNLIKELY(pNRUtilsDrv == nullptr)) {
        ainrLogF(" Error: MTKNRUtils createInstance() fail");
        return AinrErr_UnexpectedError;
    }

    NRUTILS_VerInfo rUtilsVer;
    pNRUtilsDrv->NRUtilsFeatureCtrl(NRUTILS_FTCTRL_GET_VERSION , nullptr, &rUtilsVer);

    //
    NRUTILS_CONFIG_UNPACK_BAYER_IN_STRUCT rUnpackBaseIn;
    rUnpackBaseIn.u4CoreNum = 8;
    rUnpackBaseIn.i4Width   = params.width;
    rUnpackBaseIn.i4Height  = params.height;
    rUnpackBaseIn.i4Stride  = params.stride;
    rUnpackBaseIn.i4OutBits = params.outBits;
    //rUnpackBaseIn.i4Mode = 1; // Used for CPU version of unpack

    // OB and OB ofst
    for (int i = 0; i < 4; i++) {
        rUnpackBaseIn.i4ObOffst[i] = params.obOfst[i];
        rUnpackBaseIn.i4DgnGain[i] = params.dgnGain[i];
    }

    // config
    NRUTILS_CONFIG_UNPACK_BAYER_OUT_STRUCT rUnpackBaseOut;
    pNRUtilsDrv->NRUtilsFeatureCtrl(NRUTILS_FTCTRL_CONFIG_UNPACK_BAYER, &rUnpackBaseIn, &rUnpackBaseOut);

    NRUTILS_UNPACK_BAYER_IN_STRUCT rProcIn;
    // Only CPU version need working buffer
    sp<IImageBuffer> baseWorkingBuf = nullptr;
    if (rUnpackBaseOut.i4WMSize) {
        // prepare working buffer
        NRUTILS_BUFFER_STRUCT &rWM = rProcIn.rWM;
        rWM.Size = rUnpackBaseOut.i4WMSize;
        rWM.BufferNum = 1;
        // create working buffer
        ImageBufferUtils::getInstance().allocBuffer(
            baseWorkingBuf, rWM.Size, 1, eImgFmt_STA_BYTE);
        if (CC_UNLIKELY(baseWorkingBuf.get() == nullptr)) {
            ainrLogF("baseWorkingBuf is null");;
            return AinrErr_UnexpectedError;
        }
        rWM.pBuff[0]  = reinterpret_cast<void*>(baseWorkingBuf->getBufVA(0));
        rWM.BuffFD[0] = baseWorkingBuf->getFD(0);
    } else {
        ainrLogD("No need to allocate working buffer");
    }

    // prepare Input
    const auto imgoBase = bufPackage.inBuf;
    NRUTILS_BUFFER_STRUCT &rInput = rProcIn.rInput;
    rInput.Size = rUnpackBaseIn.i4Stride*rUnpackBaseIn.i4Height;
    rInput.BufferNum = 1;
    rInput.pBuff[0]  = reinterpret_cast<void*>(imgoBase->getBufVA(0));
    rInput.BuffFD[0] = imgoBase->getFD(0);

    // prepare output
    const auto outBuf = bufPackage.outBuf;
    NRUTILS_UNPACK_BAYER_OUT_STRUCT rProcOut;
    rProcOut.rOutput.Size = rUnpackBaseIn.i4Width*rUnpackBaseIn.i4Height*sizeof(short);
    rProcOut.rOutput.BufferNum = 1;
    rProcOut.rOutput.pBuff[0]  = reinterpret_cast<void*>(outBuf->getBufVA(0));
    rProcOut.rOutput.BuffFD[0] = outBuf->getFD(0);

    // process
    pNRUtilsDrv->NRUtilsFeatureCtrl(NRUTILS_FTCTRL_PROC_UNPACK_BAYER, &rProcIn, &rProcOut);

    // Finish base frame handling. We release buffer
    // But only CPU version has working buffer
    ImageBufferUtils::getInstance().deallocBuffer(baseWorkingBuf.get());

    return AinrErr_Ok;
}

enum AinrErr AiAlgo::doNrCore(const AiParam &params, const AiCoreParam &coreParam) {
    CAM_ULOGM_APILIFE();
    AinrErr err = AinrErr_Ok;

    std::unique_ptr<MTKNRCore, std::function<void(MTKNRCore*)>> pNRCoreDrv = nullptr;
    pNRCoreDrv = decltype(pNRCoreDrv)( MTKNRCore::createInstance(DRV_NRCORE_OBJ_SW),
        [](MTKNRCore* nrCore) {
            //ainrTraceName("NrCore destroy");
            if (CC_UNLIKELY( nrCore == nullptr )) {
                ainrLogW("NRCore is nullptr");
            } else {
                ainrLogD("NR core unint");
                nrCore->destroyInstance();
            }
        }
    );
    if (CC_UNLIKELY( pNRCoreDrv == nullptr ))
    {
        ainrLogF("pNRCoreDrv is null");
        return AinrErr_UnexpectedError;
    }

    {
        CAM_ULOGM_TAGLIFE("NR core init");
        NRCORE_INIT_IN_STRUCT rInitIn;
        // TODO: Need to query from Nvram
        rInitIn.u4CoreNum   = 8;
        rInitIn.u4Width     = params.width;
        rInitIn.u4Height    = params.height;
        rInitIn.u4AlgoIdx   = 0;
        rInitIn.u4FrameNum  = coreParam.captureNum;
        rInitIn.u4TileMode  = coreParam.isTileMode ? 1 : 0;
        rInitIn.u4AlgoIdx   = static_cast<uint32_t>(coreParam.sampleMode);
        pNRCoreDrv->NRCoreInit(&rInitIn, nullptr);
    }

    NRCORE_GET_PROC_INFO_OUT_STRUCT rGetProcInfoOut;
    NRCORE_SET_PROC_INFO_IN_STRUCT  rSetProcInfoIn;
    //
    NRCORE_PROC2_IN_STRUCT rProc2In;
    NRCORE_PROC2_OUT_STRUCT rProc2Out;

    size_t       unPackSize      = 0;
    const auto outBuffer = coreParam.outBuffer;
    for (size_t i = 0; i < outBuffer->getPlaneCount(); i++) {
        unPackSize += outBuffer->getBufSizeInBytes(i);
    }
    rProc2In.u4ImgSize = unPackSize;

    // TODO(Yuan Jung) Wait algo ready
    // queryTuning(&rProc2In);

    // OB and OB ofst
    for (int i = 0; i < 4; i++) {
       rProc2In.i4ObOffst[i] = params.obOfst[i];
       rProc2In.i4DgnGain[i] = params.dgnGain[i];
    }
    rProc2Out.u4OutSize = unPackSize;

    NRCore_VerInfo rCoreVer;
    pNRCoreDrv->NRCoreFeatureCtrl(NRCORE_FTCTRL_GET_VERSION , nullptr, &rCoreVer);
    pNRCoreDrv->NRCoreFeatureCtrl(NRCORE_FTCTRL_GET_PROC_INFO , nullptr, &rGetProcInfoOut);

    // auto workBufSize   = rGetProcInfoOut.u4ReqWMSize;
    // auto workingBuffer = std::unique_ptr<char[]>(new char[workBufSize]{0});
    rSetProcInfoIn.u4Num = rGetProcInfoOut.u4Num;

    // create ion working buffer

    std::vector< sp<IImageBuffer> >        vWorkingIonBuf;
    {
        CAM_ULOGM_TAGLIFE("MDLA working buffer allocation");
        for (int i = 0; i < rSetProcInfoIn.u4Num; i++)
        {
            rSetProcInfoIn.rBuf[i].u4Size = rGetProcInfoOut.rBuf[i].u4Size;
            rSetProcInfoIn.rBuf[i].bUseIon = rGetProcInfoOut.rBuf[i].bUseIon;
            ainrLogD("MDLA Working buffer[%d] size(%d)", i, rSetProcInfoIn.rBuf[i].u4Size);

            sp<IImageBuffer> working = nullptr;
            auto ret = ImageBufferUtils::getInstance().allocBuffer(
            working, rSetProcInfoIn.rBuf[i].u4Size, 1, eImgFmt_STA_BYTE);
            if (CC_UNLIKELY(ret == MFALSE)) {
                // Release previous mdla input buffer
                if (!vWorkingIonBuf.empty()) {
                    for (auto && buf : vWorkingIonBuf) {
                        ImageBufferUtils::getInstance().deallocBuffer(buf.get());
                        buf = nullptr;
                    }
                    vWorkingIonBuf.clear();
                }
                ainrLogF("allocate mdla input error!!!");
                return AinrErr_UnexpectedError;
            }

            rSetProcInfoIn.rBuf[i].i4BuffFD = working->getFD(0);
            rSetProcInfoIn.rBuf[i].prBuff   = reinterpret_cast<void*>(working->getBufVA(0));
            vWorkingIonBuf.push_back(working);
        }
    }

    //
    pNRCoreDrv->NRCoreFeatureCtrl(NRCORE_FTCTRL_SET_PROC_INFO, &rSetProcInfoIn, nullptr);

    // N frame mv_unpackRaws
    for (int i = 0; i < coreParam.captureNum; i++)
    {
        auto inUpkBuffer = coreParam.inBuffers[i];
        rProc2In.u2Img[i] = reinterpret_cast<MUINT16*>(inUpkBuffer->getBufVA(0));
    }

    rProc2Out.u2OutImg = reinterpret_cast<MUINT16*>(outBuffer->getBufVA(0));
    ainrLogD("Execute NR core in tile mode+");
    pNRCoreDrv->NRCoreMain(NRCORE_PROC2, &rProc2In, &rProc2Out);
    ainrLogD("Execute NR core in tile mode-");

    {
        CAM_ULOGM_TAGLIFE("NrCore mdla buffer release");
        // Release MDLA working buffer
        for(auto && buf : vWorkingIonBuf) {
            ImageBufferUtils::getInstance().deallocBuffer(buf.get());
            buf = nullptr;
        }
        vWorkingIonBuf.clear();
    }

    return err;
}

enum AinrErr AiAlgo::doDRC(NSCam::IImageBuffer* rawBuf, NSCam::IMetadata* appMeta
                                , NSCam::IMetadata* halMeta, NSCam::IMetadata* dynamicMeta) {
    AinrErr err = AinrErr_Ok;
    String8 strLog;

    if (rawBuf == nullptr || appMeta == nullptr
        || halMeta == nullptr || dynamicMeta == nullptr) {
        ainrLogF("doDRC fail because one of parameter is nullptr");
        return AinrErr_UnexpectedError;
    }

    MINT64 p1timestamp = 0;
    if (!IMetadata::getEntry<MINT64>(halMeta, MTK_P1NODE_FRAME_START_TIMESTAMP, p1timestamp)) {
        ainrLogF("cannot get timestamp MTK_P1NODE_FRAME_START_TIMESTAMP");
        return AinrErr_UnexpectedError;
    }

    // Face information
    MTKFDContainerInfo* pFdDataPtr = nullptr;
    MTKFDContainerInfo fdData;

    sp<IFDContainer> fdReader = IFDContainer::createInstance(LOG_TAG,  IFDContainer::eFDContainer_Opt_Read);
    if (fdReader.get()) {
        std::vector<MTKFDContainerInfo*> vFdData = fdReader->queryLock(p1timestamp - AINR_FD_TOLERENCE, p1timestamp);
        if (vFdData.empty()) {
            ainrLogD("There is no face at ts:%" PRId64 "", p1timestamp);
        } else {
            // The latest fd info
            pFdDataPtr = vFdData.back();
            if (pFdDataPtr) {
                pFdDataPtr->clone(fdData);
            } else {
                ainrLogW("pFdDataPtr is nullptr");
            }

            MtkCameraFaceMetadata *pDetectedFaces = &fdData.facedata;
            ainrLogD("number_of_faces (%d)", pDetectedFaces->number_of_faces);
        }

        fdReader->queryUnlock(vFdData);
    } else {
        ainrLogW("FDContainer createInstance fail");
    }

    // Get AE exposure information
    int64_t shutterTime = 0;  // nanoseconds (ns)
    if (!IMetadata::getEntry<MINT64>(dynamicMeta, MTK_SENSOR_EXPOSURE_TIME, shutterTime)) {
        ainrLogW("Get MTK_SENSOR_EXPOSURE_TIME fail");
    }

    int32_t sensorGain = 0;
    if (!IMetadata::getEntry<MINT32>(dynamicMeta, MTK_3A_FEATURE_AE_SENSOR_GAIN_VALUE, sensorGain)) {
        ainrLogF("cannot get MTK_3A_FEATURE_AE_SENSOR_GAIN_VALUE");
        return AinrErr_UnexpectedError;
    }

    int32_t ispGain = 0;
    if (!IMetadata::getEntry<MINT32>(dynamicMeta, MTK_3A_FEATURE_AE_ISP_GAIN_VALUE, ispGain)) {
        ainrLogF("cannot get MTK_3A_FEATURE_AE_ISP_GAIN_VALUE");
        return AinrErr_UnexpectedError;
    }

    int32_t luxValue = 0;
    if (!IMetadata::getEntry<MINT32>(halMeta, MTK_3A_AE_LV_VALUE, luxValue)) {
        ainrLogF("cannot get MTK_3A_AE_LV_VALUE");
        return AinrErr_UnexpectedError;
    }
    ainrLogD("Shutter(%" PRId64 " ns), sensorGain(%d), ispGain(%d), luxValue(%d)"
        , shutterTime, sensorGain, ispGain, luxValue);

    // Get WB gain table
    constexpr int wbSize = 3;
    MRational wbTable[wbSize] = {};  // Gain table for RGB
    for (int i = 0; i < wbSize; i++) {
        if (!IMetadata::getEntry<MRational>(dynamicMeta, MTK_SENSOR_NEUTRAL_COLOR_POINT, wbTable[i], i)) {
            ainrLogF("cannot get MTK_SENSOR_NEUTRAL_COLOR_POINT");
            return AinrErr_UnexpectedError;
        } else {
            strLog += String8::format("Gain[%d]=(%d) ", i, wbTable[i].denominator);
        }
    }

    // Get Lens shading table
    IMetadata::Memory lscData;
    if (!IMetadata::getEntry<IMetadata::Memory>(halMeta, MTK_LSC_TBL_DATA, lscData)) {
        ainrLogF("cannot get MTK_LSC_TBL_DATA");
        return AinrErr_UnexpectedError;
    } else {
        strLog += String8::format("lsc table size(%lu) ", lscData.size());
    }

    // Get AE region table
    constexpr int regionTableSize = 5;
    int regionTable[regionTableSize];
    for (int i = 0; i < regionTableSize; i++) {
        if (!IMetadata::getEntry<MINT32>(appMeta, MTK_CONTROL_AE_REGIONS, regionTable[i], i)) {
            ainrLogF("cannot get MTK_SENSOR_NEUTRAL_COLOR_POINT");
            return AinrErr_UnexpectedError;
        }
    }
    strLog += String8::format("Left(%d) Top(%d) Right(%d) Bottom(%d) Weight(%d)", regionTable[0]
                                , regionTable[1], regionTable[2], regionTable[3], regionTable[4]);

    ainrLogD("DRC %s", strLog.string());

    return err;
}
