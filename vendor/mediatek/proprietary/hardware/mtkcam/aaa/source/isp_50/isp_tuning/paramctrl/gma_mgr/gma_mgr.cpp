/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
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

/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#define LOG_TAG "gma_mgr"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif

#include <cutils/properties.h>
#include <aaa_types.h>
#include <mtkcam/utils/std/Log.h>
#include <camera_custom_nvram.h>
#include <isp_tuning.h>
#include <awb_param.h>
#include <ae_param.h>
#include <af_param.h>
#include <flash_param.h>
#include <isp_tuning_cam_info.h>
#include <isp_tuning_custom.h>
#include <isp_mgr.h>
#include <isp_mgr_helper.h>
#include "gma_mgr.h"
#include <isp_tuning_mgr.h>
#include <private/aaa_hal_private.h>
#include <private/aaa_utils.h>
#include <array>
#include <assert.h>


using namespace NSIspTuning;
using namespace NSIspTuningv3;

#define MY_INST NS3Av3::INST_T<GmaMgr>
static std::array<MY_INST, SENSOR_IDX_MAX> gMultiton;

namespace NSIspTuningv3
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
GmaMgr*
GmaMgr::
createInstance(ESensorDev_T const eSensorDev, ISP_NVRAM_TONEMAP_PARAM_T* pIspNvramGmaPara)
{
    auto i4SensorIdx = NS3Av3::mapSensorDevToIdx(static_cast<MINT32>(eSensorDev));

    if(i4SensorIdx >= SENSOR_IDX_MAX || i4SensorIdx < 0) {
        CAM_LOGE("[%s] Unsupport sensor Index: %d\n", __FUNCTION__, i4SensorIdx);
        return NULL;
    }

    MY_INST& rSingleton = gMultiton[i4SensorIdx];
    std::call_once(rSingleton.onceFlag, [&] {
        rSingleton.instance = std::make_unique<GmaMgr>(eSensorDev, pIspNvramGmaPara);
    } );

    return rSingleton.instance.get();
}

GmaMgr::GmaMgr(ESensorDev_T const eSensorDev, ISP_NVRAM_TONEMAP_PARAM_T* pIspNvramGmaPara)
    : m_pIspNvramGmaPara (pIspNvramGmaPara)
    , m_eSensorDev  (eSensorDev)
    //, mGmaScenario (0) //E_GMA_SCENARIO_PREVIEW
    , mbAEStable (MFALSE)
    , mLogEn (0)
    , m_PrvIdx(0)
    //, m_ePrev_Tuning_Set(MTKGMA_TUNING_SET_Preview)
    //, mfgGMA_HDR(MFALSE)
{
    if(sizeof(DUAL_GGM_SYNC_INFO_T) != sizeof(MTK_GMA_DUALCAM_SYNC_INFO_T)){
        CAM_LOGE("DUAL_GGM_SYNC_INFO_T size error");
        assert(0);
    }

    if(sizeof(NSIspExifDebug::IspExifDebugInfo_T::IspGmaInfo) != sizeof(MTK_GMA_EXIF_INFO_STRUCT)){
        CAM_LOGE("IspGmaInfo size error");
        assert(0);
    }

    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("vendor.debug.gma_algo.enable", value, "1");
    mAlgoEn = atoi(value);

    memset(&mAEInfo, 0, sizeof(AE_INFO_T));
    memset(&mOutExif, 0, sizeof(MTK_GMA_EXIF_INFO_STRUCT));
    memset(&mOutGGM, 0, sizeof(MTK_GMA_RESULT_INFO_STRUCT));
    memset(&mInInfo, 0, sizeof(MTK_GMA_PROC_INFO_STRUCT));
    memset(&mGMASyncInfo,0, sizeof(MTK_GMA_DUALCAM_SYNC_INFO_T));

    CAM_LOGD_IF(1, "[%s()] before MTKGma::createInstance", __FUNCTION__);

    if (mAlgoEn) m_pGmaAlgo = MTKGma::createInstance(static_cast<eGMASensorDev_T>(m_eSensorDev));
    else m_pGmaAlgo = NULL;

    CAM_LOGD_IF(1, "[%s()] m_pGmaAlgo(%p), m_eSensorDev(%d), m_pIspNvramGmaPara(%p)", __FUNCTION__, m_pGmaAlgo, m_eSensorDev, m_pIspNvramGmaPara);

    if (mAlgoEn) m_pGmaAlgo->GmaReset();

    CAM_LOGD_IF(1, "[%s()] after GmaReset", __FUNCTION__);

    for (int tb_num = 0; tb_num<GMA_TBL_NUM; tb_num++)
    {
        for (int i = 0; i<GGM_LUT_SIZE; i++)
        {
            mEnv.rNVRAMGmaParam.i4EncNVRAMGMA[tb_num][i] = m_pIspNvramGmaPara->GMA[m_PrvIdx].GGM_Reg[tb_num].lut[i].GGM_G;
        }
        for(int i=GGM_LUT_SIZE; i<GMA_TBL_SIZE; i++)
        {
            mEnv.rNVRAMGmaParam.i4EncNVRAMGMA[tb_num][i] = 0xFFFF;
        }
    }
    mEnv.rTuningGmaParam = *((MTK_GMA_TUNING_PARAM_T*)&(m_pIspNvramGmaPara->GMA[m_PrvIdx].rGmaParam));

    CAM_LOGD_IF(1, "[%s()] before GmaInit", __FUNCTION__);

    if (mAlgoEn) m_pGmaAlgo->GmaInit(&mEnv, NULL);

    CAM_LOGD_IF(1, "[%s()] after GmaInit", __FUNCTION__);

}


GmaMgr::~GmaMgr()
{
   CAM_LOGD_IF(1, "[%s()] before GmaExit (%p)", __FUNCTION__, m_pGmaAlgo);
   if (mAlgoEn) m_pGmaAlgo->GmaExit();
   CAM_LOGD_IF(1, "[%s()] after GmaExit", __FUNCTION__);
   if (mAlgoEn) m_pGmaAlgo->destroyInstance(m_pGmaAlgo);
   CAM_LOGD_IF(1, "[%s()] after destroyInstance (%p)", __FUNCTION__, m_pGmaAlgo);
}

MVOID
GmaMgr::
printAEInfo(AE_INFO_T const & rAEInfo, const char* username)
{
        CAM_LOGD("%s: [%s] rAEInfo: %d/%d/%d/%d/%d; %d/%d/%d/%d/%d; %d/%d/%d/%d/%d; %d/%d/%d;"
            , __FUNCTION__
            , username

            , rAEInfo.u4AETarget
            , rAEInfo.u4AECurrentTarget
            , rAEInfo.u4Eposuretime
            , rAEInfo.u4AfeGain
            , rAEInfo.u4IspGain

            , rAEInfo.u4RealISOValue
            , rAEInfo.i4LightValue_x10
            , rAEInfo.u4AECondition
            , rAEInfo.i2FlareOffset
            , rAEInfo.i4GammaIdx

            , rAEInfo.i4LESE_Ratio
            , rAEInfo.u4SWHDR_SE
            , rAEInfo.u4MaxISO
            , rAEInfo.u4AEStableCnt
            , rAEInfo.u4OrgExposuretime

            , rAEInfo.u4OrgRealISOValue
            , rAEInfo.bGammaEnable
            , rAEInfo.u4EVRatio
            );


        const MUINT32* pHist = &rAEInfo.u4Histogrm[0];
        for (int i=0; i<120; i+=10)
        {
            CAM_LOGD( "%s: rAEInfo.u4Histogrm[%d-%d]=(%x,%x,%x,%x,%x,  %x,%x,%x,%x,%x)"
                , __FUNCTION__
                , i
                , i+9
                , pHist[i], pHist[i+1], pHist[i+2], pHist[i+3], pHist[i+4]
                , pHist[i+5], pHist[i+6], pHist[i+7], pHist[i+8], pHist[i+9]);
        }
        //(i == 120)
        {
            int i=120;
            CAM_LOGD( "%s: rAEInfo.u4Histogrm[%d-%d]=(%x,%x,%x,%x,%x,  %x,%x,%x)"
                , __FUNCTION__
                , i
                , i+7
                , pHist[i], pHist[i+1], pHist[i+2], pHist[i+3], pHist[i+4]
                , pHist[i+5], pHist[i+6], pHist[i+7]);
        }

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID
GmaMgr::
stop()
{
    //CAM_LOGD_IF(mLogEn, "[%s()] mGmaScenario(%d)", __FUNCTION__, mGmaScenario);
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID
GmaMgr::
start()
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("vendor.debug.dynamic_gma.log", value, "0");
    mLogEn = atoi(value);

    mInInfo.rGMAAEInfo.u4AETarget= mAEInfo.u4AETarget;
    mInInfo.rGMAAEInfo.u4AECurrentTarget= mAEInfo.u4AECurrentTarget;
    mInInfo.rGMAAEInfo.u4Eposuretime= mAEInfo.u4Eposuretime;
    mInInfo.rGMAAEInfo.u4AfeGain= mAEInfo.u4AfeGain;
    mInInfo.rGMAAEInfo.u4IspGain= mAEInfo.u4IspGain;
    mInInfo.rGMAAEInfo.u4RealISOValue= mAEInfo.u4RealISOValue;
    mInInfo.rGMAAEInfo.i4LightValue_x10= mAEInfo.i4LightValue_x10;
    mInInfo.rGMAAEInfo.u4AECondition= mAEInfo.u4AECondition;
    mInInfo.rGMAAEInfo.i2FlareOffset= mAEInfo.i2FlareOffset;
    mInInfo.rGMAAEInfo.i4GammaIdx= mAEInfo.i4GammaIdx;
    mInInfo.rGMAAEInfo.i4LESE_Ratio= mAEInfo.i4LESE_Ratio;
    mInInfo.rGMAAEInfo.u4SWHDR_SE= mAEInfo.u4SWHDR_SE;
    mInInfo.rGMAAEInfo.u4MaxISO= mAEInfo.u4MaxISO;
    mInInfo.rGMAAEInfo.u4AEStableCnt= mAEInfo.u4AEStableCnt;
    mInInfo.rGMAAEInfo.u4OrgExposuretime= mAEInfo.u4OrgExposuretime;
    mInInfo.rGMAAEInfo.u4OrgRealISOValue= mAEInfo.u4OrgRealISOValue;


    m_PrvIdx = 0;
    for (int i = 0; i<GMA_AE_HISTOGRAM_BIN; i++)
    {
        mInInfo.rGMAAEInfo.u4Histogrm[i]= mAEInfo.u4Histogrm[i];
    }
    for (int i = 0; i<GGM_LUT_SIZE; i++)
    {
        mInInfo.i4CurrEncGMA[i] = m_pIspNvramGmaPara->GMA[m_PrvIdx].GGM_Reg[0].lut[i].GGM_G;
    }

    mInInfo.rGMAAEInfo.bGammaEnable = mAEInfo.bGammaEnable;

    mInInfo.rGMAAEInfo.i4AEStable = mAEInfo.bAEStable;
    mInInfo.rGMAAEInfo.i4EVRatio = mAEInfo.u4EVRatio; //1024; //FIXME: 1024

    for (int tb_num = 0; tb_num<GMA_TBL_NUM; tb_num++)
    {
        for (int i = 0; i<GGM_LUT_SIZE; i++)
        {
            mEnv.rNVRAMGmaParam.i4EncNVRAMGMA[tb_num][i] = m_pIspNvramGmaPara->GMA[m_PrvIdx].GGM_Reg[tb_num].lut[i].GGM_G;
        }
        for(int i=GGM_LUT_SIZE; i<GMA_TBL_SIZE; i++)
        {
            mEnv.rNVRAMGmaParam.i4EncNVRAMGMA[tb_num][i] = 0xFFFF;
        }
    }
    mEnv.rTuningGmaParam = *((MTK_GMA_TUNING_PARAM_T*)&(m_pIspNvramGmaPara->GMA[m_PrvIdx].rGmaParam));


    if (mAlgoEn) m_pGmaAlgo->GmaFeatureCtrl(MTKGMA_FEATURE_SET_ENV_INFO, &mEnv, NULL);
    if (mAlgoEn) m_pGmaAlgo->GmaFeatureCtrl(MTKGMA_FEATURE_SET_PROC_INFO, &mInInfo, NULL);

    CAM_LOGD_IF(mLogEn, "[%s()] after MTKGMA_FEATURE_SET_PROC_INFO", __FUNCTION__);

}




//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID
GmaMgr::
calculateGGM(ISP_NVRAM_GGM_T* pGGMReg, NSIspExifDebug::IspExifDebugInfo_T::IspGmaInfo* pGmaExif,
             MBOOL bSlave, DUAL_ISP_SYNC_INFO_T& rISP_Sync_Info,
             AE_INFO_T const& rAEInfo, MUINT16 Idx)
{

    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("vendor.debug.dynamic_gma.log", value, "0");
    mLogEn = atoi(value);

    if( Idx != m_PrvIdx){

        m_PrvIdx = Idx;

        for (int tb_num = 0; tb_num<GMA_TBL_NUM; tb_num++)
        {
            for (int i = 0; i<GGM_LUT_SIZE; i++)
            {
                mEnv.rNVRAMGmaParam.i4EncNVRAMGMA[tb_num][i] = m_pIspNvramGmaPara->GMA[Idx].GGM_Reg[tb_num].lut[i].GGM_G;
            }
            for(int i=GGM_LUT_SIZE; i<GMA_TBL_SIZE; i++)
            {
                mEnv.rNVRAMGmaParam.i4EncNVRAMGMA[tb_num][i] = 0xFFFF;
            }
        }
        mEnv.rTuningGmaParam = *((MTK_GMA_TUNING_PARAM_T*)&(m_pIspNvramGmaPara->GMA[Idx].rGmaParam));

    }

    if (mAlgoEn){
       m_pGmaAlgo->GmaInit(&mEnv, NULL);
       m_pGmaAlgo->GmaFeatureCtrl(MTKGMA_FEATURE_SET_ENV_INFO, &mEnv, NULL);
    }

    setAEInfo(rAEInfo);

    mInInfo.bIsDualcamSlave = bSlave;
    if(bSlave){
        mInInfo.rGMADualcamSyncInfo = *((MTK_GMA_DUALCAM_SYNC_INFO_T*)&(rISP_Sync_Info.sGmaSyncInfo));
    }

    for (int i = 0; i<GGM_LUT_SIZE; i++)
    {
        mInInfo.i4CurrEncGMA[i] = pGGMReg->lut.lut[i].GGM_G;
    }

    if (mLogEn) printInfo();

    if (mAlgoEn) m_pGmaAlgo->GmaFeatureCtrl(MTKGMA_FEATURE_SET_PROC_INFO, &mInInfo, NULL);
    if (mAlgoEn) m_pGmaAlgo->GmaMain();
    if (mAlgoEn) m_pGmaAlgo->GmaFeatureCtrl(MTKGMA_FEATURE_GET_RESULT, NULL, &mOutGGM);
    if (mAlgoEn) m_pGmaAlgo->GmaFeatureCtrl(MTKGMA_FEATURE_GET_EXIF, NULL, &mOutExif);

    if (mLogEn)
    {
        CAM_LOGD_IF(1, "[%s()] m_eSensorDev(%d) after GmaMain()", __FUNCTION__, m_eSensorDev);
        printInfo();
        printAEInfo(mAEInfo, "mAEInfo_afterAlgo");
    }

    if (mAlgoEn)
    {
        for (int i = 0; i<(GGM_LUT_SIZE); i++)
        {
            pGGMReg->lut.lut[i].GGM_R = pGGMReg->lut.lut[i].GGM_B = pGGMReg->lut.lut[i].GGM_G = mOutGGM.i4EncGMA[i];
            pGGMReg->lut.lut[i].rsv_30 = 0;

            if (mLogEn)
            {
                if (((i%10) == 0) && (i!=190))
                {
                    CAM_LOGD( "%s: mOutGGM.i4EncGMA[%d-%d]=(%x,%x,%x,%x,%x,  %x,%x,%x,%x,%x)"
                        , __FUNCTION__
                        , i
                        , i+9
                        , mOutGGM.i4EncGMA[i], mOutGGM.i4EncGMA[i+1], mOutGGM.i4EncGMA[i+2], mOutGGM.i4EncGMA[i+3], mOutGGM.i4EncGMA[i+4]
                        , mOutGGM.i4EncGMA[i+5], mOutGGM.i4EncGMA[i+6], mOutGGM.i4EncGMA[i+7], mOutGGM.i4EncGMA[i+8], mOutGGM.i4EncGMA[i+9]);
                }
                else if (i == 190)
                {
                    CAM_LOGD( "%s: mOutGGM.i4EncGMA[%d-%d]=(%x,%x)"
                        , __FUNCTION__
                        , i
                        , i+1
                        , mOutGGM.i4EncGMA[i], mOutGGM.i4EncGMA[i+1]);

                }
            }
        }

        if(!bSlave){
            if (mAlgoEn) m_pGmaAlgo->GmaFeatureCtrl(MTKGMA_FEATURE_GET_DUALCAM_SYNC_INFO, NULL, &mGMASyncInfo);
            ::memcpy(((MTK_GMA_DUALCAM_SYNC_INFO_T*)&(rISP_Sync_Info.sGmaSyncInfo)), &mGMASyncInfo, sizeof(DUAL_GGM_SYNC_INFO_T));
        }

        ::memcpy(pGmaExif, &mOutExif, sizeof(NSIspExifDebug::IspExifDebugInfo_T::IspGmaInfo));

        if (mLogEn)
        {
            CAM_LOGD("%s: GMA EXIF:  %x/%x; %x/%x/%x/%x/%x; %x/%x/%x/%x/%x; %x/%x/%x/%x/%x; %x/%x/%x/%x/%x; %x/%x/%x/%x/%x; %x/%x/%x/%x/%x;"
                , __FUNCTION__

                , mOutExif.i4GMAMode
                , mOutExif.i4FaceGMAFlag
                , mOutExif.i4ChipVersion
                , mOutExif.i4MainVersion
                , mOutExif.i4SubVersion
                , mOutExif.i4SystemVersion
                , mOutExif.i4EVRatio
                , mOutExif.i4LowContrastThr
                , mOutExif.i4LowContrastRatio
                , mOutExif.i4LowContrastSeg
                , mOutExif.i4Contrast
                , mOutExif.i4Contrast_L
                , mOutExif.i4Contrast_H
                , mOutExif.i4HdrContrastWeight
                , mOutExif.i4EVContrastY
                , mOutExif.i4ContrastY_L
                , mOutExif.i4ContrastY_H
                , mOutExif.i4NightContrastWeight
                , mOutExif.i4LV
                , mOutExif.i4LV_L
                , mOutExif.i4LV_H
                , mOutExif.i4HdrLVWeight
                , mOutExif.i4NightLVWeight
                , mOutExif.i4SmoothEnable
                , mOutExif.i4SmoothSpeed
                , mOutExif.i4SmoothWaitAE
                , mOutExif.i4GMACurveEnable

                , mOutExif.i4CenterPt
                , mOutExif.i4LowCurve
                , mOutExif.i4SlopeL
                , mOutExif.i4FlareEnable
                , mOutExif.i4FlareOffset);

        /*
            typedef struct
            {
                MINT32 i4GMAMode;
                MINT32 i4GmaProfile;
                MINT32 i4ChipVersion;
                MINT32 i4MainVersion;
                MINT32 i4SubVersion;
                MINT32 i4SystemVersion;
                MINT32 i4EVRatio;
                MINT32 i4LowContrastThr;
                MINT32 i4LowContrastRatio;
                MINT32 i4LowContrastSeg;
                MINT32 i4Contrast;
                MINT32 i4Contrast_L;
                MINT32 i4Contrast_H;
                MINT32 i4HdrContrastWeight;
                MINT32 i4EVContrastY;
                MINT32 i4ContrastY_L;
                MINT32 i4ContrastY_H;
                MINT32 i4NightContrastWeight;
                MINT32 i4LV;
                MINT32 i4LV_L;
                MINT32 i4LV_H;
                MINT32 i4HdrLVWeight;
                MINT32 i4NightLVWeight;
                MINT32 i4SmoothEnable;
                MINT32 i4SmoothSpeed;
                MINT32 i4SmoothWaitAE;
                MINT32 i4GMACurveEnable;
                MINT32 i4CenterPt;
                MINT32 i4LowCurve;
                MINT32 i4SlopeL;
                MINT32 i4FlareEnable;
                MINT32 i4FlareOffset;
            } MTK_GMA_EXIF_INFO_STRUCT, *P_MTK_GMA_EXIF_INFO_STRUCT;
        */



        }
    }
}

MVOID GmaMgr::printInfo() const
{

    CAM_LOGD("%s: mInInfo(%d): /%d/%d/%d/%d; %d/%d/%d/%d/%d; %d/%d/%d/%d/%d; %d/%d/%d/%d;"
        , __FUNCTION__

        , mInInfo.rGMAAEInfo.u4AETarget
        , mInInfo.rGMAAEInfo.u4AECurrentTarget
        , mInInfo.rGMAAEInfo.u4Eposuretime
        , mInInfo.rGMAAEInfo.u4AfeGain
        , mInInfo.rGMAAEInfo.u4IspGain

        , mInInfo.rGMAAEInfo.u4RealISOValue
        , mInInfo.rGMAAEInfo.i4LightValue_x10
        , mInInfo.rGMAAEInfo.u4AECondition
        , mInInfo.rGMAAEInfo.i2FlareOffset
        , mInInfo.rGMAAEInfo.i4GammaIdx

        , mInInfo.rGMAAEInfo.i4LESE_Ratio
        , mInInfo.rGMAAEInfo.u4SWHDR_SE
        , mInInfo.rGMAAEInfo.u4MaxISO
        , mInInfo.rGMAAEInfo.u4AEStableCnt
        , mInInfo.rGMAAEInfo.u4OrgExposuretime

        , mInInfo.rGMAAEInfo.u4OrgRealISOValue
        , mInInfo.rGMAAEInfo.bGammaEnable
        , mInInfo.rGMAAEInfo.i4AEStable
        , mInInfo.rGMAAEInfo.i4EVRatio
        );

    /*
        typedef struct {
            MUINT32 u4AETarget;
            MUINT32 u4AECurrentTarget;
            MUINT32 u4Eposuretime;   //!<: Exposure time in ms
            MUINT32 u4AfeGain;           //!<: raw gain
            MUINT32 u4IspGain;           //!<: sensor gain
            MUINT32 u4RealISOValue;
            MINT32  i4LightValue_x10;
            MUINT32 u4AECondition;
            MINT16  i2FlareOffset;
            MINT32  i4GammaIdx;   // next gamma idx
            MINT32  i4LESE_Ratio;    // LE/SE ratio
            MUINT32 u4SWHDR_SE;      //for sw HDR SE ,  -x EV , compare with converge AE
            MUINT32 u4MaxISO;
            MUINT32 u4AEStableCnt;
            MUINT32 u4OrgExposuretime;   //!<: Exposure time in ms
            MUINT32 u4OrgRealISOValue;
            MUINT16 u2Histogrm[GMA_AE_HISTOGRAM_BIN];
            MBOOL bGammaEnable;

            MINT32 i4AEStable;
            MINT32 i4EVRatio;
        } GMA_AE_INFO_T;
    */

    const MUINT32* pHist = &mInInfo.rGMAAEInfo.u4Histogrm[0];
    for (int i=0; i<120; i+=10)
    {
        CAM_LOGD( "%s: mInInfo.rGMAAEInfo.u4Histogrm[%d-%d]=(%x,%x,%x,%x,%x,  %x,%x,%x,%x,%x)"
            , __FUNCTION__
            , i
            , i+9
            , pHist[i], pHist[i+1], pHist[i+2], pHist[i+3], pHist[i+4]
            , pHist[i+5], pHist[i+6], pHist[i+7], pHist[i+8], pHist[i+9]);
    }
    //(i == 120)
    {
        int i=120;
        CAM_LOGD( "%s: mInInfo.rGMAAEInfo.u4Histogrm[%d-%d]=(%x,%x,%x,%x,%x,  %x,%x,%x)"
            , __FUNCTION__
            , i
            , i+7
            , pHist[i], pHist[i+1], pHist[i+2], pHist[i+3], pHist[i+4]
            , pHist[i+5], pHist[i+6], pHist[i+7]);
    }



    for (int i=0; i<140; i+=10)
    {
        CAM_LOGD( "%s: mInInfo.i4CurrEncGMA[%d-%d]=(%x,%x,%x,%x,%x,  %x,%x,%x,%x,%x)"
            , __FUNCTION__
            , i
            , i+9
            , mInInfo.i4CurrEncGMA[i], mInInfo.i4CurrEncGMA[i+1], mInInfo.i4CurrEncGMA[i+2], mInInfo.i4CurrEncGMA[i+3], mInInfo.i4CurrEncGMA[i+4]
            , mInInfo.i4CurrEncGMA[i+5], mInInfo.i4CurrEncGMA[i+6], mInInfo.i4CurrEncGMA[i+7], mInInfo.i4CurrEncGMA[i+8], mInInfo.i4CurrEncGMA[i+9]);
    }
    //(i == 140)
    {
        CAM_LOGD( "%s: mInInfo.i4CurrEncGMA[%d-%d]=(%x,%x,%x,%x)"
            , __FUNCTION__
            , 140
            , 140+3
            , mInInfo.i4CurrEncGMA[140], mInInfo.i4CurrEncGMA[140+1], mInInfo.i4CurrEncGMA[140+2], mInInfo.i4CurrEncGMA[140+3]);

    }

}

MVOID GmaMgr::setAEInfo(AE_INFO_T const & rAEInfo)
{
    mAEInfo = rAEInfo;

    if (!mAlgoEn) return;

    mInInfo.rGMAAEInfo.u4AETarget         = mAEInfo.u4AETarget;
    mInInfo.rGMAAEInfo.u4AECurrentTarget  = mAEInfo.u4AECurrentTarget;
    mInInfo.rGMAAEInfo.u4Eposuretime      = mAEInfo.u4Eposuretime;
    mInInfo.rGMAAEInfo.u4AfeGain          = mAEInfo.u4AfeGain;
    mInInfo.rGMAAEInfo.u4IspGain          = mAEInfo.u4IspGain;
    mInInfo.rGMAAEInfo.u4RealISOValue     = mAEInfo.u4RealISOValue;
    mInInfo.rGMAAEInfo.i4LightValue_x10   = mAEInfo.i4LightValue_x10;
    mInInfo.rGMAAEInfo.u4AECondition      = mAEInfo.u4AECondition;
    mInInfo.rGMAAEInfo.i2FlareOffset      = mAEInfo.i2FlareOffset;
    mInInfo.rGMAAEInfo.i4GammaIdx         = mAEInfo.i4GammaIdx;
    mInInfo.rGMAAEInfo.i4LESE_Ratio       = mAEInfo.i4LESE_Ratio;
    mInInfo.rGMAAEInfo.u4SWHDR_SE         = mAEInfo.u4SWHDR_SE;
    mInInfo.rGMAAEInfo.u4MaxISO           = mAEInfo.u4MaxISO;
    mInInfo.rGMAAEInfo.u4AEStableCnt      = mAEInfo.u4AEStableCnt;
    mInInfo.rGMAAEInfo.u4OrgExposuretime  = mAEInfo.u4OrgExposuretime;
    mInInfo.rGMAAEInfo.u4OrgRealISOValue  = mAEInfo.u4OrgRealISOValue;
    mInInfo.rGMAAEInfo.bGammaEnable       = mAEInfo.bGammaEnable;
    mInInfo.rGMAAEInfo.i4AEStable         = mAEInfo.bAEStable;
    mInInfo.rGMAAEInfo.i4EVRatio          = mAEInfo.u4EVRatio;
    mInInfo.rGMAAEInfo.i4DeltaIndex       = mAEInfo.i4deltaIndex;
    mInInfo.rGMAAEInfo.u4FDProb           = mAEInfo.u4FDProb;
    mInInfo.rGMAAEInfo.u4FaceNum       	  = mAEInfo.u4FaceNum;

    for (int i = 0; i<GMA_AE_HISTOGRAM_BIN; i++)
    {
        mInInfo.rGMAAEInfo.u4Histogrm[i]  = mAEInfo.u4Histogrm[i];
    }

    if (mLogEn){
        printAEInfo(rAEInfo, "setAESrc");
        printAEInfo(mAEInfo, "setAEDst");
    }

}

}
