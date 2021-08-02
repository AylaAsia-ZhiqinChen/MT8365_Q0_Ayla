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
#define LOG_TAG "AinrCore/AiHdrAlgo"

#include "AiHdr.h"

#include <mtkcam3/feature/ainr/AinrUlog.h>
#include <mtkcam3/feature/utils/ImageBufferUtils.h>

// ISP profile
#include <tuning_mapping/cam_idx_struct_ext.h>
// For RAWIspCamInfo
#include <isp_tuning/ver1/isp_tuning_cam_info.h>

// AOSP
#include <utils/String8.h>
#include <cutils/properties.h>

// STD
#include <unordered_map>  // std::unordered_map


using namespace android;
using namespace ainr;
using namespace NSCam;

AiHdr::AiHdr() {
    ainrLogD("AiHdr ctor");
}

AiHdr::~AiHdr() {
    ainrLogD("AiHdr desctructor");
}

enum AinrErr AiHdr::doNrCore(const AiParam &params, const AiCoreParam &coreParam) {
    CAM_ULOGM_APILIFE();
    AinrErr err = AinrErr_Ok;

    std::unique_ptr<MTKHDRCore, std::function<void(MTKHDRCore*)>> coreDrv = nullptr;
    coreDrv = decltype(coreDrv)(MTKHDRCore::createInstance(DRV_HDRCORE_OBJ_SW),
        [](MTKHDRCore* core) {
            CAM_ULOGM_TAGLIFE("NrCore destroy");
            if (CC_UNLIKELY(core == nullptr)) {
                ainrLogW("HDR core is nullptr");
            } else {
                ainrLogD("HDR core unint");
                core->destroyInstance();
            }
        });
    if (CC_UNLIKELY(coreDrv == nullptr)) {
        ainrLogF("pNRCoreDrv is null");
        return AinrErr_UnexpectedError;
    }

    {
        // ainrTraceName("NR core init");
        HDRCORE_INIT_IN_STRUCT rInitIn;
        // TODO(Yuan Jung): Need to query from Nvram
        rInitIn.u4CoreNum   = 8;
        rInitIn.u4Width     = params.width;
        rInitIn.u4Height    = params.height;
        rInitIn.u4AlgoIdx   = 0;
        rInitIn.u4FrameNum  = coreParam.captureNum;
        rInitIn.u4TileMode  = coreParam.isTileMode ? 1 : 0;
        rInitIn.u4AlgoIdx   = static_cast<uint32_t>(coreParam.sampleMode);
        coreDrv->HDRCoreInit(&rInitIn, nullptr);
    }

    HDRCORE_GET_PROC_INFO_OUT_STRUCT rGetProcInfoOut = {};
    HDRCORE_SET_PROC_INFO_IN_STRUCT  rSetProcInfoIn = {};
    //
    HDRCORE_PROC2_IN_STRUCT rProc2In = {};
    HDRCORE_PROC2_OUT_STRUCT rProc2Out = {};

    size_t unPackSize = 0;
    auto outBuffer = coreParam.outBuffer;
    for (size_t i = 0; i < outBuffer->getPlaneCount(); i++) {
        unPackSize += outBuffer->getBufSizeInBytes(i);
    }
    rProc2In.u4ImgSize = unPackSize;
    rProc2Out.u4OutSize = unPackSize;

    // TODO(Yuan Jung) Wait algo ready
    //queryTuning(&rProc2In);

    // OB and OB ofst
    for (int i = 0; i < 4; i++) {
       rProc2In.i4ObOffst[i] = params.obOfst[i];
       rProc2In.i4DgnGain[i] = params.dgnGain[i];
    }

    HDRCore_VerInfo rCoreVer;
    coreDrv->HDRCoreFeatureCtrl(HDRCORE_FTCTRL_GET_VERSION , nullptr, &rCoreVer);
    coreDrv->HDRCoreFeatureCtrl(HDRCORE_FTCTRL_GET_PROC_INFO , nullptr, &rGetProcInfoOut);

    // auto workBufSize   = rGetProcInfoOut.u4ReqWMSize;
    // auto workingBuffer = std::unique_ptr<char[]>(new char[workBufSize]{0});
    rSetProcInfoIn.u4Num = rGetProcInfoOut.u4Num;

    // create ion working buffer
    std::vector< sp<IImageBuffer> >        vWorkingIonBuf;
    {
        CAM_ULOGM_TAGLIFE("MDLA working buffer allocation");
        for (int i = 0; i < rSetProcInfoIn.u4Num; i++) {
            rSetProcInfoIn.rBuf[i].u4Size = rGetProcInfoOut.rBuf[i].u4Size;
            rSetProcInfoIn.rBuf[i].bUseIon = rGetProcInfoOut.rBuf[i].bUseIon;
            ainrLogD("MDLA Working buffer[%d] size(%d)", i, rSetProcInfoIn.rBuf[i].u4Size);

            // Error check for working buffer size
            if (rSetProcInfoIn.rBuf[i].u4Size < 0) {
                ainrLogF("MDLA Working buffer[%d] size(%d) negative!!!", i, rSetProcInfoIn.rBuf[i].u4Size);
                return AinrErr_UnexpectedError;
            }

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
    coreDrv->HDRCoreFeatureCtrl(HDRCORE_FTCTRL_SET_PROC_INFO, &rSetProcInfoIn, nullptr);

    // N frame mv_unpackRaws
    for (int i = 0; i < coreParam.captureNum; i++) {
        auto inUpkBuffer = coreParam.inBuffers[i];
        rProc2In.u2Img[i] = reinterpret_cast<MUINT16*>(inUpkBuffer->getBufVA(0));
    }

    rProc2Out.u2OutImg = reinterpret_cast<MUINT16*>(outBuffer->getBufVA(0));
    ainrLogD("Execute HDR core in tile mode+");
    coreDrv->HDRCoreMain(HDRCORE_PROC2, &rProc2In, &rProc2Out);
    ainrLogD("Execute HDR core in tile mode-");

    {
        CAM_ULOGM_TAGLIFE("NrCore mdla buffer release");
        // Release MDLA working buffer
        for (auto && buf : vWorkingIonBuf) {
            ImageBufferUtils::getInstance().deallocBuffer(buf.get());
            buf = nullptr;
        }
        vWorkingIonBuf.clear();
    }

    return err;
}
