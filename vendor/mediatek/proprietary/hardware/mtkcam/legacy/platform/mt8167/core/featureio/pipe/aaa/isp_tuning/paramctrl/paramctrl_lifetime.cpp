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
#define LOG_TAG "paramctrl_lifetime"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif

#include <cutils/properties.h>
#include <aaa_types.h>
#include <aaa_error_code.h>
#include <aaa_log.h>
#include <camera_custom_nvram.h>
#include <awb_param.h>
#include <ae_param.h>
#include <af_param.h>
#include <flash_param.h>
#include <isp_tuning.h>
#include <isp_tuning_cam_info.h>
#include <isp_tuning_idx.h>
#include <isp_tuning_custom.h>
//#include <nvram_drv_mgr.h>
#include <ispdrv_mgr.h>
#include <isp_mgr.h>
#include <isp_mgr_helper.h>
#include <mtkcam/featureio/tdri_mgr.h>
#include <pca_mgr.h>
#include <gma_mgr.h>
#include <mtkcam/algorithm/lib3a/dynamic_ccm.h>
#include <ccm_mgr.h>
#include <lsc_mgr2.h>
#include <dbg_isp_param.h>
#include <mtkcam/hal/sensor_hal.h>
#include "paramctrl_if.h"
#include "paramctrl.h"
#include <mtkcam/hal/IHalSensor.h>
#include "nvbuf_util.h"

using namespace NSIspTuning;
using namespace NSCam;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
IParamctrl*
IParamctrl::createInstance(ESensorDev_T const eSensorDev)
{
    return Paramctrl::getInstance(eSensorDev);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Paramctrl*
Paramctrl::
getInstance(ESensorDev_T const eSensorDev)
{
    Paramctrl* pParamctrl = MNULL;
    IHalSensorList* const pIHalSensorList = IHalSensorList::get();
    //IHalSensor* pIHalSensor = pIHalSensorList->createSensor("paramctrl_lifetime", i4SensorIdx);
    NVRAM_CAMERA_ISP_PARAM_STRUCT*  pNvram_Isp = MNULL;

    //  Sensor info
    //SensorDynamicInfo rSensorDynamicInfo;
    SensorStaticInfo rSensorStaticInfo;

    MUINT32 u4SensorID;
    ESensorTG_T eSensorTG;

    int err;
    err = NvBufUtil::getInstance().getBufAndRead(CAMERA_NVRAM_DATA_ISP, eSensorDev, (void*&)pNvram_Isp);
    if(err!=0)
    {
        MY_ERR("NvBufUtil getBufAndRead fail\n");
        goto lbExit;
    }


/*
    // ISP NVRAM
    if (MERR_OK != NvramDrvMgr::getInstance().init(eSensorDev))
    {
        goto lbExit;
    }

    NvramDrvMgr::getInstance().getRefBuf(pNvram_Isp);

    if (! pNvram_Isp)
    {
        MY_ERR("[createInstance] (pNvram_Isp) = (%p)", pNvram_Isp);
        goto lbExit;
    }
*/
#if 0
    MY_LOG("sizeof(pNvram_Isp->Version) = %d\n", sizeof(pNvram_Isp->Version));
    MY_LOG("sizeof(pNvram_Isp->SensorId) = %d\n", sizeof(pNvram_Isp->SensorId));
    MY_LOG("sizeof(pNvram_Isp->ISPComm) = %d\n", sizeof(pNvram_Isp->ISPComm));
    MY_LOG("sizeof(pNvram_Isp->ISPPca) = %d\n", sizeof(pNvram_Isp->ISPPca));
    MY_LOG("sizeof(pNvram_Isp->ISPRegs) = %d\n", sizeof(pNvram_Isp->ISPRegs));
    MY_LOG("sizeof(pNvram_Isp->ISPMfbMixer) = %d\n", sizeof(pNvram_Isp->ISPMfbMixer));
    MY_LOG("sizeof(pNvram_Isp->ISPMulitCCM) = %d\n", sizeof(pNvram_Isp->ISPMulitCCM));
    MY_LOG("sizeof(pNvram_Isp) = %d\n", sizeof(NVRAM_CAMERA_ISP_PARAM_STRUCT));
#endif

    switch  ( eSensorDev )
    {
    case ESensorDev_Main:
        pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_MAIN, &rSensorStaticInfo);
        //pIHalSensor->querySensorDynamicInfo(NSCam::SENSOR_DEV_MAIN, &rSensorDynamicInfo);
        break;
    case ESensorDev_Sub:
        pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_SUB, &rSensorStaticInfo);
        //pIHalSensor->querySensorDynamicInfo(NSCam::SENSOR_DEV_SUB, &rSensorDynamicInfo);
        break;
    case ESensorDev_MainSecond:
        pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_MAIN_2, &rSensorStaticInfo);
        //pIHalSensor->querySensorDynamicInfo(NSCam::SENSOR_DEV_MAIN_2, &rSensorDynamicInfo);
        break;
    default:    //  Shouldn't happen.
        MY_ERR("Invalid sensor device: %d", eSensorDev);
        goto lbExit;
    }

    u4SensorID = rSensorStaticInfo.sensorDevID;

    MY_LOG("u4SensorID = %d\n", u4SensorID);

    eSensorTG = ESensorTG_1; // just default setting, will be updated later

    //  Here, pIspParam must be legal.
    pParamctrl = new Paramctrl(
        eSensorDev, u4SensorID, eSensorTG, pNvram_Isp
    );

lbExit:
  //  NvramDrvMgr::getInstance().uninit();

    //if  ( pIHalSensor )
    //    pIHalSensor->destroyInstance("paramctrl_lifetime");

    return  pParamctrl;
}


void
Paramctrl::
destroyInstance()
{
    delete this;
}


Paramctrl::
Paramctrl(
    ESensorDev_T const eSensorDev,
    MUINT32 const i4SensorIdx,
    ESensorTG_T const eSensorTG,
    NVRAM_CAMERA_ISP_PARAM_STRUCT*const pNvram_Isp
)
    : IParamctrl(eSensorDev)
    , m_u4ParamChangeCount(0)
    , m_fgDynamicTuning(MTRUE)
    , m_fgDynamicCCM(MTRUE)
    , m_fgDynamicBypass(MFALSE)
    , m_fgDynamicShading(MTRUE)
    , m_fgEnableRPG(MTRUE)
    , m_backup_OBCInfo()
    , m_eIdx_Effect(MTK_CONTROL_EFFECT_MODE_OFF)
    , m_eSensorDev(eSensorDev)
    , m_eOperMode(EOperMode_Normal)
    , m_rIspExifDebugInfo()
    , m_IspUsrSelectLevel()
    , m_rIspCamInfo()
    , m_pIspTuningCustom(IspTuningCustom::createInstance(eSensorDev, i4SensorIdx))
    , m_rIspParam(*pNvram_Isp)
    , m_rIspComm(m_rIspParam.ISPComm)
    , m_IspNvramMgr(&m_rIspParam.ISPRegs)
    , m_pPcaMgr(PcaMgr::createInstance(eSensorDev, m_rIspParam.ISPPca))
    , m_pCcmMgr(CcmMgr::createInstance(eSensorDev, m_rIspParam.ISPRegs, m_rIspParam.ISPMulitCCM, m_pIspTuningCustom))
    //, m_pLscMgr(LscMgr2::createInstance(eSensorDev, 0, m_rIspParam))
    , m_pNvram_Shading(NULL)
    , m_Lock()
    , m_bDebugEnable(MFALSE)
    , m_bProfileEnable(MFALSE)
    , m_eSensorTG(eSensorTG)
    , m_ePCAMode(EPCAMode_180BIN)
    //, m_eSoftwareScenario(eSoftwareScenario_Main_Normal_Stream)
    //, m_u4SwnrEncEnableIsoThreshold(0)
    , m_i4SensorIdx(i4SensorIdx)
    , m_i4CamMode(0) // normal mode
    , m_u4SwnrEncEnableIsoThreshold(MFALSE)
    , m_pAaaTimer(MNULL)
    , m_i4FlashOnOff(0)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("vendor.debug.lsc_mgr.type", value, "2");
    int type = atoi(value);
    m_pLscMgr = LscMgr2::createInstance(eSensorDev, 0, m_rIspParam, type);

    m_pGmaMgr = GmaMgr::createInstance(eSensorDev, m_rIspParam.ISPRegs, (MTK_GMA_ENV_INFO_STRUCT*)(m_pIspTuningCustom->get_custom_GMA_env_info(eSensorDev)));
    memset(&m_GmaExifInfo, 0, sizeof(NSIspExifDebug::IspExifDebugInfo_T::IspGmaInfo));
    MY_LOG("[%s ctor] eSensorDev(%d), env(%p), m_pGmaMgr(%ld)", __FUNCTION__, eSensorDev, m_pIspTuningCustom->get_custom_GMA_env_info(eSensorDev), (unsigned long)m_pGmaMgr);
}


Paramctrl::
~Paramctrl()
{

}

MERROR_ENUM
Paramctrl::
init()
{
    MERROR_ENUM err = MERR_OK;

    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("vendor.debug.paramctrl.enable", value, "0");
    m_bDebugEnable = atoi(value);

    property_get("vendor.profile.paramctrl.enable", value, "0");
    m_bProfileEnable = atoi(value);

    //  (1) Force to assume all params have chagned and different.
    m_u4ParamChangeCount = 1;

    //  (2) Init ISP/T driver manager.
    CPTLog(Event_Pipe_3A_ISP_DRVMGR_INIT, CPTFlagStart); // Profiling Start.
    IspDrvMgr::getInstance().init();
    CPTLog(Event_Pipe_3A_ISP_DRVMGR_INIT, CPTFlagEnd);   // Profiling End.

    CPTLog(Event_Pipe_3A_ISP_TDRIMGR_INIT, CPTFlagStart); // Profiling Start.
    TdriMgr::getInstance().init();
    CPTLog(Event_Pipe_3A_ISP_TDRIMGR_INIT, CPTFlagEnd);   // Profiling End.

    CPTLog(Event_Pipe_3A_ISP_LSCMGR_INIT, CPTFlagStart); // Profiling Start.
    m_pLscMgr->init();
    CPTLog(Event_Pipe_3A_ISP_LSCMGR_INIT, CPTFlagEnd);   // Profiling End.

    //  (3) validateFrameless() is invoked
    //  when init() or status change, like Camera Mode.
    CPTLog(Event_Pipe_3A_ISP_VALIDATE_FRAMELESS, CPTFlagStart); // Profiling Start.
    err = validateFrameless();
    CPTLog(Event_Pipe_3A_ISP_VALIDATE_FRAMELESS, CPTFlagEnd); // Profiling End.

    if  ( MERR_OK != err )
    {
        goto lbExit;
    }

    //  (4) however, is it needed to invoke validatePerFrame() in init()?
    //  or just invoke it only when a frame comes.
    CPTLog(Event_Pipe_3A_ISP_VALIDATE_PERFRAME, CPTFlagStart); // Profiling Start.
    err = validatePerFrame(MTRUE);
    CPTLog(Event_Pipe_3A_ISP_VALIDATE_PERFRAME, CPTFlagEnd); // Profiling End.


    // For debug
    if (m_bDebugEnable) {
        IspDebug::getInstance().init();
    }

    if (m_bProfileEnable) {
        m_pAaaTimer = new AaaTimer;
    }

    if  ( MERR_OK != err )
    {
        goto lbExit;
    }

lbExit:
    if  ( MERR_OK != err )
    {
        uninit();
    }

    MY_LOG("[-ParamctrlRAW::init]err(%X)", err);
    return  err;
}


MERROR_ENUM
Paramctrl::
uninit()
{
    MY_LOG("[+uninit]");

    //  Uninit ISP driver manager.
    IspDrvMgr::getInstance().uninit();

    TdriMgr::getInstance().uninit();

    m_pLscMgr->uninit();

    if (m_bDebugEnable) {
        IspDebug::getInstance().uninit();
    }

    if (m_bProfileEnable) {
        delete m_pAaaTimer;
        m_pAaaTimer = MNULL;
    }

    return  MERR_OK;
}

