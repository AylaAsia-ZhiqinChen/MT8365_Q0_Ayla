/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2018. All rights reserved.
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
#define LOG_TAG "AinrCore/FeFm"

#include "AinrFeFm.h"
#include <mtkcam3/feature/ainr/AinrUlog.h>
// Pass2 header
#include <mtkcam/drv/def/ispio_port_index.h> // port id

#include <unordered_map> // std::unordered_map

// platform dependent headers
// If you want to debug with dip_reg.h remember to add
// "in mk (AINR_INCLUDE_PATH += $(MTK_PATH_SOURCE)/hardware/mtkcam/drv/include/dip/isp_50/)"
//#include <dip_reg.h> // dip_x_reg_t
// ISP profile
#include <tuning_mapping/cam_idx_struct_ext.h>
// Allocate working buffer. Be aware of that we use AOSP library
#include <mtkcam3/feature/utils/ImageBufferUtils.h>
#include <mtkcam/utils/TuningUtils/FileDumpNamingRule.h> // tuning file naming

//AOSP
#include <utils/Vector.h>


using namespace ainr;
using namespace NSImageio::NSIspio;

//-----------------------------------------------------------------------------
// IAinrFeFm methods
//-----------------------------------------------------------------------------
std::shared_ptr<IAinrFeFm> IAinrFeFm::createInstance()
{
    std::shared_ptr<IAinrFeFm> pAinrFeFm = std::make_shared<AinrFeFm>();
    return pAinrFeFm;
}

AinrFeFm::AinrFeFm()
    : m_sensorId(0)
    , m_blockSize(0)
    , m_regTableSize(0)
    , m_ispProfile(0)
{
}

AinrFeFm::~AinrFeFm()
{
    m_pNormalStream = nullptr;
    m_pHalIsp = nullptr;
}

enum AinrErr AinrFeFm::init(IAinrFeFm::ConfigParams const& params)
{
    enum AinrErr err = AinrErr_Ok;
    auto sensorId = params.openId;

    m_sensorId = params.openId;
    /* RAII for INormalStream */
    m_pNormalStream = decltype(m_pNormalStream)(
        INormalStream::createInstance(sensorId),
        [](INormalStream* p) {
            if (!p) return;
            p->uninit(LOG_TAG);
            p->destroyInstance();
        }
    );

    if (CC_UNLIKELY( m_pNormalStream.get() == nullptr )) {
        ainrLogF("create INormalStream fail");
        return AinrErr_UnexpectedError;
    }
    else {
        auto bResult = m_pNormalStream->init(LOG_TAG);
        if (CC_UNLIKELY(bResult == MFALSE)) {
            ainrLogF("init INormalStream returns MFALSE");
            return AinrErr_UnexpectedError;
        }
    }

     // create IHal3A
    m_pHalIsp =
        std::unique_ptr< IHalISP, std::function<void(IHalISP*)> >
        (
            MAKE_HalISP(sensorId, "AinrFefm"),
            [](IHalISP* p) { if(CC_LIKELY(p)) p->destroyInstance("AinrFefm"); }
        );

    if (CC_UNLIKELY(m_pHalIsp.get() == nullptr)) {
        ainrLogF("create IHalIsp failed");
        return AinrErr_UnexpectedError;
    }

    // Tuning size
    m_regTableSize = m_pNormalStream->getRegTableSize();
    m_ispProfile = params.ispProfile;

    // Set up FEFM tuning
    setupFeFmTuning();
    //
    m_blockSize = getFeFmBlockSize(m_feInfo.mFEMODE);

    if(CC_UNLIKELY(m_blockSize == 0)) {
        ainrLogF("FEFM block size is null!!!");
        return AinrErr_UnexpectedError;
    } else {
        ainrLogD("Fe mode(%d), block size(%d)", m_feInfo.mFEMODE, m_blockSize);
    }

    // Record resolution and fefm size
    // FeFm size should be block size alignment
    m_bufferSize = params.bufferSize;

    int downSizeRatio = 1;
    m_bufferSize.w = m_bufferSize.w/downSizeRatio;
    m_bufferSize.h = m_bufferSize.h/downSizeRatio;
    //
    m_CropSize.w = (m_bufferSize.w/m_blockSize) * m_blockSize;
    m_CropSize.h = (m_bufferSize.h/m_blockSize) * m_blockSize;
    ainrLogD("FeFm input size (%d, %d) crop size(%d, %d)"
        , m_bufferSize.w, m_bufferSize.h
        , m_CropSize.w, m_CropSize.h);
    // set up Feo and Fmo size
    //m_feoSizeInfo.width = ALIGN( (m_CropSize.w/m_blockSize), m_blockSize );
    //m_feoSizeInfo.height = ALIGN( (m_CropSize.h/m_blockSize), m_blockSize);
    //m_feoSizeInfo.stride = ALIGN( (m_CropSize.w/m_blockSize)*40, m_blockSize );

    m_feoSize.w = (m_CropSize.w/m_blockSize)*40;
    m_feoSize.h = m_CropSize.h/m_blockSize;
    ainrLogD("Feo size(%d, %d)", m_feoSize.w, m_feoSize.h);
    //
    m_fmoSize.w = (m_feoSize.w/40)*2;
    m_fmoSize.h = m_feoSize.h;
    ainrLogD("Fmo size(%d, %d)", m_fmoSize.w, m_fmoSize.h);
    // FMO tuning width/height
    m_fmInfo.mFMWIDTH = m_CropSize.w/m_blockSize;
    m_fmInfo.mFMHEIGHT = m_CropSize.h/m_blockSize;

    {
        std::lock_guard<std::mutex> __lk(m_cbMx);
        m_cbMethod = params.cbMethod;
    }

    return err;
}

enum AinrErr AinrFeFm::getFeFmSize(MSize &feoSize, MSize &fmoSize)
{
    //
    feoSize = m_feoSize;
    fmoSize = m_fmoSize;
    return AinrErr_Ok;
}

enum AinrErr AinrFeFm::prepareFE(QParams &qParams, IImageBuffer *inputBuff, IImageBuffer *outputFeo, IImageBuffer *outputYuv, MetaSet_T &metaset)
{
    // create a Pass2DataPack, including tuning buffer, and threa sync mechanism
    //Pass2DataPack pass2data(m_regTableSize);

    // PrepareFE
    FrameParams paramsFe;
    paramsFe.mStreamTag = ENormalStreamTag_Normal; //Normal case FE would work as well. ENormalStreamTag_FE;

    // input
    Input inputFe;
    inputFe.mPortID = PORT_IMGI;
    inputFe.mBuffer = inputBuff;
    paramsFe.mvIn.push_back(inputFe);
    // Output
    Output outFe;
    outFe.mPortID = PORT_FEO;
    outFe.mBuffer = outputFeo;
    paramsFe.mvOut.push_back(outFe);

    // Bittrue flow
    if(outputYuv) {
        // Output YUV in IMG3O for bit true
        Output outYuv;
        outYuv.mPortID       = PORT_IMG3O;
        outYuv.mBuffer       = outputYuv;
        outYuv.mPortID.group = 0;
        paramsFe.mvOut.push_back(outYuv);
    }
    // Module SRZ
    // Be aware of that out_w/out_h should be
    // the same as crop_w/crop_h
    m_srzSizeInfo.in_w = m_bufferSize.w;
    m_srzSizeInfo.in_h = m_bufferSize.h;
    m_srzSizeInfo.out_w =  m_CropSize.w;
    m_srzSizeInfo.out_h =  m_CropSize.h;
    m_srzSizeInfo.crop_w = m_CropSize.w;
    m_srzSizeInfo.crop_h = m_CropSize.h;

    // Tuning data
//    std::shared_ptr<char> tuningData = std::make_shared<char>(m_regTableSize);
    //
    // Allocate tuning data. P2 user should provide tuning buffer
    // otherwise unexpect error would occur
    char *tuningBuffer = (char *)malloc(m_regTableSize);
    if(CC_UNLIKELY(tuningBuffer == nullptr)) {
        ainrLogF("FE tuning buffer is nullptr");
        return AinrErr_BadArgument;
    }

    memset(tuningBuffer, 0, m_regTableSize);

    ModuleInfo moduleInfo;
    moduleInfo.moduleTag = EDipModule_SRZ1;
    moduleInfo.moduleStruct = &m_srzSizeInfo;
    paramsFe.mvModuleData.push_back(moduleInfo);
    // ExtraParam
    ExtraParam extra;
    extra.CmdIdx = EPIPE_FE_INFO_CMD;
    extra.moduleStruct = static_cast<void*>(&m_feInfo);
    paramsFe.mvExtraParam.push_back(extra);

    // Isp tuning. We need to do raw2yuv for feo
    // therefore we need to setIsp
    {
        CAM_ULOGM_TAGLIFE("FEFM setIsp");
        IMetadata::setEntry<MUINT8>(&metaset.halMeta, MTK_3A_ISP_PROFILE, m_ispProfile);
        // We dont need lsc therefore we bypass it
        IMetadata::setEntry<MINT32>(&metaset.halMeta, MTK_3A_ISP_BYPASS_LCE, 1);
        //trySetMetadata<MUINT8>(pIMetaHal, MTK_3A_PGN_ENABLE, 0);
        IMetadata::setEntry<MUINT8>(&metaset.halMeta, MTK_3A_PGN_ENABLE, 0); //Disable shading
        // Our input is raw buffer. We should indicate it as raw to ISP
        IMetadata::setEntry<MINT32>(&metaset.halMeta, MTK_ISP_P2_IN_IMG_FMT, 0);

        TuningParam rTuningParam;
        rTuningParam.pRegBuf = tuningBuffer; //pass2data.getBuf();
        rTuningParam.pLcsBuf = nullptr;

        auto rSetIsp = [&] {
            if (CC_UNLIKELY(m_pHalIsp.get() == nullptr)) {
                ainrLogE("IHal3A instance is NULL");
                return -1;
            }
            return m_pHalIsp->setP2Isp(
                    0,
                    metaset,
                    &rTuningParam,
                    &metaset);
        }();

        if (CC_UNLIKELY(rSetIsp != 0)) {
            ainrLogF("setIsp returns fail");
            return AinrErr_BadArgument;
        }
        // End
    }

    paramsFe.mTuningData = tuningBuffer;
    qParams.mvFrameParams.push_back(paramsFe);
    return AinrErr_Ok;
}

enum AinrErr AinrFeFm::buildFe(IAinrFeFm::DataPackage *package)
{
    if (CC_UNLIKELY(package->appMeta == nullptr)
        || CC_UNLIKELY(package->halMeta == nullptr)
        || CC_UNLIKELY(package->inBuf == nullptr)
        || CC_UNLIKELY(package->outBuf== nullptr)) {
        ainrLogE("One of data is null in packages!!");
        return AinrErr_BadArgument;
    }

    MetaSet_T metaset;
    metaset.appMeta = *(package->appMeta);
    metaset.halMeta = *(package->halMeta);

    prepareFE(m_qParams, package->inBuf, package->outBuf, package->outYuv, metaset);
    return AinrErr_Ok;
}

enum AinrErr AinrFeFm::buildFm(IImageBuffer *baseBuf, IImageBuffer *refBuf
                                        , IImageBuffer *fmo, int index, bool needCb)
{
    if (CC_UNLIKELY(baseBuf == nullptr)
        || CC_UNLIKELY(refBuf == nullptr)
        || CC_UNLIKELY(fmo == nullptr)) {
        ainrLogE("One of data is null in FM argument list !!");
        return AinrErr_BadArgument;
    }

    prepareFM(m_qParams, baseBuf, refBuf, fmo, index, needCb);
    return AinrErr_Ok;
}

enum AinrErr AinrFeFm::doFeFm()
{
    // Set up normalPipe callback
    Pass2Async p2Async;
    m_qParams.mpCookie = static_cast<void*>(&p2Async);
    m_qParams.mpfnCallback = [](QParams& rParams)->MVOID
    {
        if (CC_UNLIKELY(rParams.mpCookie == nullptr))
            return;

        Pass2Async* pAsync = static_cast<Pass2Async*>(rParams.mpCookie);
        std::lock_guard<std::mutex> __l(pAsync->getLocker());
        // Start to release tuning buffer
        for (size_t i = 0; i < rParams.mvFrameParams.size(); i++)
        {
            NSCam::NSIoPipe::FrameParams& param = rParams.mvFrameParams.editItemAt(i);
            if (param.mTuningData)
            {
                void* pTuning = param.mTuningData;
                if (pTuning)
                {
                    //ainrLogD("Release tuning buffer(%p)", pTuning);
                    free(pTuning);
                }
            }
        }
        pAsync->notifyOne();
        ainrLogD("FEFM pass2 done");
    };

    // Start to enque request to pass2
    {
        auto __l = p2Async.uniqueLock();
        CAM_ULOGM_TAGLIFE("FEFM processing");
        MBOOL bEnqueResult = MTRUE;
        {
            if (CC_UNLIKELY(m_pNormalStream.get() == nullptr)) {
                ainrLogE("INormalStream instance is NULL");
                return AinrErr_BadArgument;
            }
            ainrLogD("FEFM pass2 enque");
            bEnqueResult = m_pNormalStream->enque(m_qParams);
        }

        if (CC_UNLIKELY(!bEnqueResult)) {
            ainrLogE("FEFM enque fail");
            return AinrErr_BadArgument;
        }
        else {
            p2Async.wait(std::move(__l));
        }
    }
    return AinrErr_Ok;
}

enum AinrErr AinrFeFm::prepareFM(QParams &qParams, IImageBuffer *feoBase, IImageBuffer *feoRef
                                        , IImageBuffer *fmo, int index, bool needCb)
{
    char *tuningBuffer = (char *)malloc(m_regTableSize);
    if(CC_UNLIKELY(tuningBuffer == nullptr)) {
        ainrLogF("FE tuning buffer is nullptr");
        return AinrErr_BadArgument;
    }

    memset(tuningBuffer, 0, m_regTableSize);
    //ainrLogD("FM tuning buffer(%p)", tuningBuffer);


    // PrepareFE
    FrameParams paramsFm;
    paramsFm.mStreamTag = ENormalStreamTag_FM;

    // input
    Input inputFeoBase;
    inputFeoBase.mPortID = PORT_LFEOI;
    inputFeoBase.mBuffer = feoBase;
    paramsFm.mvIn.push_back(inputFeoBase);
    Input inputFeoRef;
    inputFeoRef.mPortID = PORT_RFEOI;
    inputFeoRef.mBuffer = feoRef;
    paramsFm.mvIn.push_back(inputFeoRef);

    // Output
    Output outFm;
    outFm.mPortID = PORT_FMO;
    outFm.mBuffer = fmo;
    paramsFm.mvOut.push_back(outFm);

    // Tuning data
    paramsFm.mTuningData = static_cast<MVOID*>(tuningBuffer);

    // Extra param FM tuning
    ExtraParam extra;
    extra.CmdIdx = EPIPE_FM_INFO_CMD;
    extra.moduleStruct = static_cast<void*>(&m_fmInfo);
    paramsFm.mvExtraParam.push_back(extra);

    if(needCb) {
        paramsFm.mpCookie = reinterpret_cast<MVOID*>(this);
        paramsFm.FrameNo  = index;

        // Per-Frame callback
        paramsFm.mpfnCallback = [](const FrameParams& rFrmParams, EFrameCallBackMsg FrmCBMsg) -> bool {
            AinrFeFm* fefm = reinterpret_cast<AinrFeFm*>(rFrmParams.mpCookie);
            std::lock_guard<std::mutex> __lk(fefm->m_cbMx);

            ainrLogD("FEFM callback frame number(%d)", rFrmParams.FrameNo);
            fefm->m_cbMethod(rFrmParams.FrameNo);

            return true;
        };
    }

    qParams.mvFrameParams.push_back(paramsFm);
    return AinrErr_Ok;
}

enum AinrErr AinrFeFm::checkPackage(IAinrFeFm::BufferPackage &bufferSet, IAinrFeFm::MetaDataPackage &metaSet)
{
    if(CC_UNLIKELY(!bufferSet.inBase
        && !bufferSet.inRef
        && !bufferSet.outFeoBase
        && !bufferSet.outFeoRef
        && !bufferSet.outFmoBase
        && !bufferSet.outFmoRef
        && !metaSet.appMetaBase
        && !metaSet.halMetaBase
        && !metaSet.dynamicMetaBase
        && !metaSet.appMetaRef
        && !metaSet.halMetaRef
        && !metaSet.dynamicMetaRef)) {
        ainrLogE("Check pacakge fail because of null pointer");
        return AinrErr_NullPointer;
    }
    return AinrErr_Ok;
}

enum AinrErr AinrFeFm::setupFeFmTuning()
{
    // TODO: We need to load tuning data by Json file instead of hard code
    // FE info
    m_feInfo.mFEDSCR_SBIT = 3;
    m_feInfo.mFETH_C = 2;
    m_feInfo.mFETH_G = 0;
    m_feInfo.mFEFLT_EN = 1;
    m_feInfo.mFEPARAM = 8;
    m_feInfo.mFEMODE = 1;
    // FM info
    m_fmInfo.mFMSR_TYPE = 0;
    m_fmInfo.mFMOFFSET_X = 16;
    m_fmInfo.mFMOFFSET_Y = 16;
    m_fmInfo.mFMRES_TH = 2;
    m_fmInfo.mFMSAD_TH = 1023;
    m_fmInfo.mFMMIN_RATIO = 90;

    return AinrErr_Ok;
}

int AinrFeFm::getFeFmBlockSize(const int FE_MODE)
{
    int blockSize = 16;
    switch(FE_MODE)
    {
        case 0:
            blockSize = 32;
            break;
        case 1:
            blockSize = 16;
            break;
        case 2:
            blockSize = 8;
            break;
        default:
            break;
    }
    return blockSize;
}
