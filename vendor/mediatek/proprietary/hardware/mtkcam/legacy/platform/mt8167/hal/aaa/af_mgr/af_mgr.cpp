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
#define LOG_TAG "af_mgr"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif

#include <utils/threads.h>  // For Mutex::Autolock.
#include <cutils/properties.h>
#include <aaa_types.h>
#include <aaa_error_code.h>
//#include <kd_camera_feature.h>
#include <aaa_log.h>
#include <mtkcam/common/faces.h>

//#include <aaa_hal.h>
//#include <camera_custom_nvram.h>
//#include <af_param.h>
//#include <awb_param.h>
//#include <ae_param.h>
#include <isp_tuning.h>
#include <isp_tuning_mgr.h>
#include <af_tuning_custom.h>
#include <mcu_drv.h>
#include <mtkcam/drv_common/isp_reg.h>
//#include <mtkcam/drv/isp_drv.h>
//#include <mtkcam/drv_FrmB/isp_drv_FrmB.h>
#include <mtkcam/iopipe/CamIO/INormalPipe.h>

#include <mtkcam/hal/sensor_hal.h>
#include <mtkcam/hal/IHalSensor.h>

#include <nvram_drv.h>
#include <nvram_drv_mgr.h>
#include <mtkcam/acdk/cct_feature.h>
#include <mtkcam/featureio/flicker_hal_base.h>
#include "af_mgr.h"
#include <mtkcam/common.h>
using namespace NSCam;
#include <ae_mgr_if.h>
#include <mtkcam/featureio/nvbuf_util.h>


#include "camera_custom_cam_cal.h"  //seanlin 121022 for test
#include "cam_cal_drv.h" //seanlin 121022 for test
#include "aaa_common_custom.h"
// AF v1.2
#include <math.h>
#include <android/sensor.h>             // for g/gyro sensor listener

#define SENSOR_ACCE_POLLING_MS  33
#define SENSOR_GYRO_POLLING_MS  33
#define SENSOR_ACCE_SCALE       100
#define SENSOR_GYRO_SCALE       100

#define WIN_SIZE_MIN 8
#define WIN_SIZE_MAX 510
#define WIN_POS_MIN  16

#define FL_WIN_SIZE_MIN 8
#define FL_WIN_SIZE_MAX 4094
#define FL_WIN_POS_MIN  16
#define FLKPAUS

NVRAM_LENS_PARA_STRUCT* g_pNVRAM_LENS;

#ifdef FLKPAUS
//FlickerHalBase* m_pFlickerHal;
#endif
#define ISPREADREG

using namespace NS3Av3;
using namespace NSCam;
using namespace NSIoPipe;
using namespace NSCamIOPipe;
using namespace NSIspTuningv3;

// AF v1.2
// g/gyro sensor listener handler and data
static MBOOL gListenEnAcce = MFALSE;
static MINT32 gAcceInfo[3];
static MUINT64 gAcceTS;
static MUINT64 gPreAcceTS;
static MBOOL gListenEnGyro = MFALSE;
static MINT32 gGyroInfo[3];
static MUINT64 gGyroTS;
static MUINT64 gPreGyroTS;
static MBOOL gListenLogOn = MFALSE;

static Mutex  gSensorCommonLock;
static Mutex  gSensorEventLock;
static sp<ISensorManager> mpSensorManager = NULL;
static sp<IEventQueue>    mpEventQueue = NULL;
static sp<AfSensorListener> mpAfSensorListener = NULL;
static MINT32             mGyroSensorHandle = 0;
static MINT32             mAcceSensorHandle = 0;

Return<void> AfSensorListener::onEvent(const Event &e) {
    sensors_event_t sensorEvent;
    android::hardware::sensors::V1_0::implementation::convertToSensorEvent(e, &sensorEvent);

    Mutex::Autolock lock(gSensorEventLock);

    switch(e.sensorType)
    {
        case SensorType::ACCELEROMETER:
            if(gListenLogOn)
            {
                MY_LOG("Acc(%f,%f,%f,%lld)",
                        sensorEvent.acceleration.x,
                        sensorEvent.acceleration.y,
                        sensorEvent.acceleration.z,
                        sensorEvent.timestamp);
            }
            gPreAcceTS = gAcceTS;
            gAcceInfo[0] = sensorEvent.acceleration.x * SENSOR_ACCE_SCALE;
            gAcceInfo[1] = sensorEvent.acceleration.y * SENSOR_ACCE_SCALE;
            gAcceInfo[2] = sensorEvent.acceleration.z * SENSOR_ACCE_SCALE;
            gAcceTS = sensorEvent.timestamp;
            break;

        case SensorType::GYROSCOPE:
            if(gListenLogOn)
            {
                MY_LOG("Gyro(%f,%f,%f,%lld)",
                        sensorEvent.gyro.x,
                        sensorEvent.gyro.y,
                        sensorEvent.gyro.z,
                        sensorEvent.timestamp);
            }
            gPreGyroTS = gGyroTS;
            gGyroInfo[0] = sensorEvent.gyro.x * SENSOR_GYRO_SCALE;
            gGyroInfo[1] = sensorEvent.gyro.y * SENSOR_GYRO_SCALE;
            gGyroInfo[2] = sensorEvent.gyro.z * SENSOR_GYRO_SCALE;
            gGyroTS = sensorEvent.timestamp;
            break;

        default:
            MY_LOG("unknown type(%d)",sensorEvent.type);
            break;
        }

    return android::hardware::Void();
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AfMgr& AfMgr::getInstance()
{
    static  AfMgr singleton;
    return  singleton;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AfMgr::AfMgr()
{
    m_Users   = 0;
    m_pMcuDrv = NULL;
    m_pIspReg = NULL;
    m_pIAfAlgo = NULL;
    memset(&m_sAFInput,   0, sizeof(m_sAFInput));
    memset(&m_sAFOutput,   0, sizeof(m_sAFOutput));
    memset(&m_sAFParam,   0, sizeof(m_sAFParam));
    memset(&m_sAFConfig,   0, sizeof(m_sAFConfig));
    memset(&m_NVRAM_LENS,   0, sizeof(m_NVRAM_LENS));
    memset(&m_CameraFocusArea,   0, sizeof(m_CameraFocusArea));
    memset(&m_FDArea,   0, sizeof(m_FDArea));
    memset(&m_sAFFullStat,   0, sizeof(m_sAFFullStat));
    memset(&m_sEZoom,   0, sizeof(m_sEZoom));

    m_i4AF_in_Hsize = 0;
    m_i4AF_in_Vsize = 0;
    m_i4CurrSensorDev = 1;
    m_i4CurrSensorId = 0x1;
    m_i4CurrLensId = 0;

    m_eCurrAFMode=1;
    m_i4BackupAFModeWithFlash = m_eLIB3A_AFMode = LIB3A_AF_MODE_AFS;
    m_i4EnableBackupAFMode = 0;
    m_i4EnableAF = -1;
    m_sAFInput.i4FullScanStep = 1;
    m_pAFCallBack = NULL;
    m_i4AFPreStatus = AF_MARK_NONE;
    m_bDebugEnable = FALSE;
    m_i4AutoFocuscb = FALSE;
    m_i4AF_TH[0] = -1;
    m_i4AF_TH[2] = -1;
    m_tcaf_mode=0;
    m_i4AutoFocus = FALSE;
    m_i4SensorIdx=0;
    for (MINT32 i=0; i<36; i++)   {
        m_i8PreVStat[i] = 0;
    }
    m_i8BSSVlu   = 0;
    m_flkwin_syncflag  = 0;

    m_ThreadGGyro = NULL;
    m_bGGyroEn = MFALSE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AfMgr::~AfMgr()
{
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AfMgr::sendAFNormalPipe(MINT32 cmd, MINT32 arg1, MINT32 arg2, MINT32 arg3)
{
    INormalPipe_FrmB* pPipe = (INormalPipe_FrmB*)INormalPipe_FrmB::createInstance(m_i4SensorIdx, "afmgr", 1);
    MBOOL fgRet = pPipe->sendCommand(cmd, arg1, arg2, arg3);
    pPipe->destroyInstance("afmgr");
    return fgRet;
}

MRESULT AfMgr::init(MINT32 eSensorDev, MINT32 i4SensorIdx)
{
    MRESULT ret = S_3A_OK;
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("vendor.debug.af_mgr.enable", value, "0");
    m_bDebugEnable = atoi(value);

    Mutex::Autolock lock(m_Lock);
    m_i4CurrSensorDev=(MINT32)eSensorDev;
    m_tcaf_mode=0;
    m_prePosition=0;
    m_i4SensorIdx=i4SensorIdx;
    if (m_Users > 0)
    {
        MY_LOG("[init] no init, %d has created \n", m_Users);
        android_atomic_inc(&m_Users);
        return S_3A_OK;
    }
    else
    {
        m_i4EnableAF = -1;
        android_atomic_inc(&m_Users);
        MY_LOG("[init]EnableAF %d", m_i4EnableAF);
    }

    m_i4InactiveCntInContiPicMode = 0;
    m_i4IsAFAlgoSearchDone = 1;
    mi4IsAFDoneFlg_pre = mi4IsAFDoneFlg_cur = m_i4SearchDone = 1;
    m_i4ChangeSearchingState = m_i4SetAFModeCmd = m_i4AutoFocusCmd = m_i4CancelAutoFocusCmd = 0;
    m_i4LockAFInContiMode = m_i4AuotoFocusInContiPicMode = 0;
    m_i4InitAFCmd = 1;
    m_eAFState = NS3A::E_AF_INACTIVE;

    MY_LOG("[init] start, m_Users: %d", m_Users);
    // --- init MCU ---
    SensorStaticInfo rSensorStaticInfo;
    if (m_i4EnableAF == -1)
    {
        IHalSensorList* const pIHalSensorList = IHalSensorList::get();
        IHalSensor* pIHalSensor = pIHalSensorList->createSensor("af_mgr", m_i4SensorIdx);
        SensorDynamicInfo rSensorDynamicInfo;

       m_AFMeta = pIHalSensorList->queryStaticInfo(m_i4SensorIdx);
       m_getAFmeta=1;

       switch(m_i4CurrSensorDev)
       {
           case ESensorDev_Main:
               pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_MAIN, &rSensorStaticInfo);
               pIHalSensor->querySensorDynamicInfo(NSCam::SENSOR_DEV_MAIN, &rSensorDynamicInfo);
               break;
           case ESensorDev_Sub:
               pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_SUB, &rSensorStaticInfo);
               pIHalSensor->querySensorDynamicInfo(NSCam::SENSOR_DEV_SUB, &rSensorDynamicInfo);
               break;
           case ESensorDev_MainSecond:
               pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_MAIN_2, &rSensorStaticInfo);
               pIHalSensor->querySensorDynamicInfo(NSCam::SENSOR_DEV_MAIN_2, &rSensorDynamicInfo);
               break;
           default:
               MY_ERR("Invalid sensor device: %d", m_i4CurrSensorDev);
       }
       if(pIHalSensor) pIHalSensor->destroyInstance("af_mgr");

       m_Pix_Id = (MINT32)rSensorStaticInfo.sensorFormatOrder;  // 0:BGGR, 1:GBRG, 2GRBG, 3RGGB
       MY_LOG("m_Pix_Id = %d (0:B,1:GB,2:GR,3:R)\n", m_Pix_Id);

       m_i4CurrSensorId=rSensorStaticInfo.sensorDevID;
       MCUDrv::lensSearch(m_i4CurrSensorDev, m_i4CurrSensorId);
       m_i4CurrLensId = MCUDrv::getCurrLensID(m_i4CurrSensorDev);
       MY_LOG("[lens][SensorDev]0x%04x, [SensorId]0x%04x, [CurrLensId]0x%04x", m_i4CurrSensorDev, m_i4CurrSensorId, m_i4CurrLensId);

        if (m_i4CurrLensId == 0xFFFF)   m_i4EnableAF = 0;
        else                            m_i4EnableAF = 1;
    }

    m_pMcuDrv = MCUDrv::createInstance(m_i4CurrLensId);
    if (!m_pMcuDrv)
    {
        MY_ERR("McuDrv::createInstance fail");
        m_i4EnableAF = 0;
    }

    if (m_pMcuDrv->init(m_i4CurrSensorDev) < 0)
    {
        MY_ERR("m_pMcuDrv->init() fail");
        m_i4EnableAF = 0;
    }

    // --- init ISP Drv/Reg ---
    #ifdef ISPREADREG
    m_pIspReg = IspDrv::createInstance();
    if (!m_pIspReg) {
        MY_ERR("IspDrv::createInstance() fail \n");
    }

    if (m_pIspReg->init("af_mgr") == MFALSE) {
        MY_ERR("pIspDrv->init() fail \n");
        m_pIspReg = NULL;
    }
    #endif

    setAF_IN_HSIZE();
    if ((m_sAFInput.sEZoom.i4W == 0) || (m_sAFInput.sEZoom.i4H == 0))
    {
        m_sAFInput.sEZoom.i4W = m_i4AF_in_Hsize;
        m_sAFInput.sEZoom.i4H = m_i4AF_in_Vsize;
    }

    // --- init af algo ---
    m_pIAfAlgo=IAfAlgo::createInstance<EAAAOpt_MTK>(m_i4CurrSensorDev);
    if (!m_pIAfAlgo)
    {
        MY_ERR("AfAlgo pointer NULL \n");
        m_i4EnableAF = 0;
        return S_AF_OK;
    }
    m_i4AFPreStatus = AF_MARK_NONE;
    m_sAFOutput.sAFArea.sRect[0].i4Info=AF_MARK_NONE;
    m_i4AutoFocuscb = FALSE;
    m_sAFOutput.i4FDDetect=0;

    #ifdef FLKPAUS
    //m_pFlickerHal = FlickerHalBase::getInstance();

    //if (!m_pFlickerHal) {
      //  MY_ERR("m_pFlickerHal pointer NULL \n");
        //return E_AF_NULL_POINTER;
    //}
    #endif

    void* p;
    NvBufUtil::getInstance().getBufAndRead(CAMERA_NVRAM_DATA_LENS, m_i4CurrSensorDev, p);
    g_pNVRAM_LENS = (NVRAM_LENS_PARA_STRUCT*)p;
    // --- NVRAM ---
    //if (FAILED(NvramDrvMgr::getInstance().init(m_i4CurrSensorDev))) {
    //     MY_ERR("NvramDrvMgr init fail\n");
    //     return E_AWB_SENSOR_ERROR;
    //}
    //NvramDrvMgr::getInstance().getRefBuf(g_pNVRAM_LENS);
    //NvramDrvMgr::getInstance().uninit();
    m_NVRAM_LENS.Version = g_pNVRAM_LENS->Version;
    m_NVRAM_LENS.rFocusRange = g_pNVRAM_LENS->rFocusRange;
    m_NVRAM_LENS.rAFNVRAM= g_pNVRAM_LENS->rAFNVRAM;

    #if 1
    //MY_LOG("[nvram][THRES_MAIN]%d", m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4THRES_MAIN);
    //MY_LOG("[nvram][SUB_MAIN]%d", m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4THRES_SUB);
    MY_LOG("[nvram][Normal Num]%d [Macro Num]%d", m_NVRAM_LENS.rAFNVRAM.sAF_Coef.sTABLE.i4NormalNum, m_NVRAM_LENS.rAFNVRAM.sAF_Coef.sTABLE.i4MacroNum);
    //MY_LOG("[nvram][VAFC Fail Cnt]%d", m_NVRAM_LENS.rAFNVRAM.i4VAFC_FAIL_CNT);
    //MY_LOG("[nvram][LV thres]%d", m_NVRAM_LENS.rAFNVRAM.i4LV_THRES);
    //MY_LOG("[nvram][PercentW]%d [PercentH]%d", m_NVRAM_LENS.rAFNVRAM.i4SPOT_PERCENT_W, m_NVRAM_LENS.rAFNVRAM.i4SPOT_PERCENT_H);
    //MY_LOG("[nvram][AFC step]%d", m_NVRAM_LENS.rAFNVRAM.i4AFC_STEP_SIZE);
    //MY_LOG("[nvram][InfPos]%d", m_NVRAM_LENS.rFocusRange.i4InfPos);
    //MY_LOG("[nvram][MacroPos]%d", m_NVRAM_LENS.rFocusRange.i4MacroPos);
    #endif

    //------------------------------------------- init g/gyro sensor listener -----------------------------------------
    // GyroSensor init
    m_bGGyroEn = MTRUE;
    ::pthread_create(&m_ThreadGGyro, NULL, createSensorListenerThread, this);

    // --- Param ---
    m_sAFParam = getAFParam();

    m_sAFParam.i4ReadOTP          =   m_NVRAM_LENS.rAFNVRAM.i4ReadOTP;
    m_sAFParam.i4FD_DETECT_CNT    =   m_NVRAM_LENS.rAFNVRAM.i4FD_DETECT_CNT;
    m_sAFParam.i4FD_NONE_CNT      =   m_NVRAM_LENS.rAFNVRAM.i4FD_NONE_CNT;

    m_sAFParam.i4FV_SHOCK_THRES   =   m_NVRAM_LENS.rAFNVRAM.i4FV_SHOCK_THRES;
    m_sAFParam.i4FV_SHOCK_OFFSET  =   m_NVRAM_LENS.rAFNVRAM.i4FV_SHOCK_OFFSET;
    m_sAFParam.i4FV_VALID_CNT     =   m_NVRAM_LENS.rAFNVRAM.i4FV_VALID_CNT;
    m_sAFParam.i4FV_SHOCK_FRM_CNT =   m_NVRAM_LENS.rAFNVRAM.i4FV_SHOCK_FRM_CNT;
    m_sAFParam.i4FV_SHOCK_CNT     =   m_NVRAM_LENS.rAFNVRAM.i4FV_SHOCK_CNT;

    // AF v2.0 : use NVRAM parameters instead of AFParam
    MY_LOG("[Param][Normal min step]%d [Macro min step]%d", m_NVRAM_LENS.rAFNVRAM.i4AFS_STEP_MIN_NORMAL, m_NVRAM_LENS.rAFNVRAM.i4AFS_STEP_MIN_MACRO);

    // --- Config ---
    m_sAFConfig = getAFConfig();

    setAFArea(m_CameraFocusArea);

    // AF v1.2
    // i4Coef[7] > 0: enable new scenechange , else: disable
    //if((m_NVRAM_LENS.rAFNVRAM.i4SceneMonitorLevel > 0) && (m_AndroidServiceState==TRUE) )
    //{   // update m_NVRAM_LENS.rAFNVRAM before setAFParam to AF alg
    //    updateSceneChangeParams();
    //}

    m_pIAfAlgo->setAFParam(m_sAFParam, m_sAFConfig, m_NVRAM_LENS.rAFNVRAM);
    m_pIAfAlgo->initAF(m_sAFInput, m_sAFOutput);

    if (m_sAFParam.i4ReadOTP == TRUE)
    {
        readOTP();
    }
    m_i4AF_TH[0] = -1;
    m_i4AF_TH[2] = -1;
    setAFConfig(m_sAFOutput.sAFStatConfig);
    setAFWinTH(m_sAFOutput.sAFStatConfig);
    setGMR(m_sAFOutput.sAFStatConfig);
    setAFWinConfig(m_sAFOutput.sAFArea);
    setFlkWinConfig();

    printAFConfigLog0();
    printAFConfigLog1();
    if (m_pIAfAlgo)
        m_pIAfAlgo->setAFMode(m_eLIB3A_AFMode);
    m_NonInitState=MFALSE;
    m_isTriggerAFS=MFALSE;


    MY_LOG("[init] finish");
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::uninit()
{
    MY_LOG("[uninit] +");
    MRESULT ret = S_3A_OK;
    Mutex::Autolock lock(m_Lock);
    if (m_Users <= 0)
    {
        return S_3A_OK;
    }
    android_atomic_dec(&m_Users);
    if (m_Users != 0)
    {
        MY_LOG("[uninit] Still %d users \n", m_Users);
        return S_AF_OK;
    }
    if (m_pIAfAlgo)
    {
        m_pIAfAlgo->destroyInstance();
        m_pIAfAlgo = NULL;
    }

    // AF v1.2
    // --- uninit g/gyro sensor listener ---
    // GyroSensor uninit
    m_bGGyroEn = MFALSE;
    EnableGGyro();
    //::pthread_create(&m_ThreadGGyro, NULL, createSensorListenerThread, this);

    if (m_pMcuDrv)
    {
        MY_LOG("[uninitMcuDrv] - SensorDev: %d", m_i4CurrSensorDev);
        m_pMcuDrv->uninit(m_i4CurrSensorDev);
        m_pMcuDrv->destroyInstance();
        m_pMcuDrv = NULL;
    }
    #ifdef ISPREADREG
    if (m_pIspReg)   {
        m_pIspReg->uninit("af_mgr");
        m_pIspReg = NULL;
    }
    #endif
    #ifdef FLKPAUS
    //if (m_pFlickerHal)   {
      //  m_pFlickerHal = NULL;
    //}
    #endif
    m_i4EnableAF = -1;
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFOpeartion()
{
    MY_LOG("[ACDK_CCT_V2_OP_AF_OPERATION]\n");
    setAFMode(AF_MODE_AFS);
    triggerAF();
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPMFOpeartion(MINT32 a_i4MFpos)
{
    MINT32 i4TimeOutCnt = 0;

    MY_LOG("[ACDK_CCT_V2_OP_MF_OPERATION]\n");

    setAFMode(AF_MODE_MF);
    triggerAF();
    setMFPos(a_i4MFpos);

    while (!isFocusFinish())
    {
        usleep(5000); // 5ms
        i4TimeOutCnt++;
        if (i4TimeOutCnt > 100)
            break;
        }

    MY_LOG("[MF]pos:%d, value:%lld\n", a_i4MFpos, m_sAFInput.sAFStat.i8Stat24);

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFGetAFInfo(MVOID *a_pAFInfo, MUINT32 *a_pOutLen)
{
    ACDK_AF_INFO_T *pAFInfo = (ACDK_AF_INFO_T *)a_pAFInfo;

    MY_LOG("[ACDK_CCT_V2_OP_GET_AF_INFO]\n");

    pAFInfo->i4AFMode = m_eLIB3A_AFMode;
    pAFInfo->i4AFMeter = LIB3A_AF_METER_SPOT;
    pAFInfo->i4CurrPos = m_sAFOutput.i4AFPos;

    *a_pOutLen = sizeof(ACDK_AF_INFO_T);

    MY_LOG("[AF Mode] = %d\n", pAFInfo->i4AFMode);
    MY_LOG("[AF Meter] = %d\n", pAFInfo->i4AFMeter);
    MY_LOG("[AF Current Pos] = %d\n", pAFInfo->i4CurrPos);

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFGetBestPos(MINT32 *a_pAFBestPos, MUINT32 *a_pOutLen)
{
    MY_LOG("[ACDK_CCT_V2_OP_AF_GET_BEST_POS]%d\n", m_sAFOutput.i4AFBestPos);
    *a_pAFBestPos = m_sAFOutput.i4AFBestPos;
    *a_pOutLen = sizeof(MINT32);
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFCaliOperation(MVOID *a_pAFCaliData, MUINT32 *a_pOutLen)
{
    ACDK_AF_CALI_DATA_T *pAFCaliData = (ACDK_AF_CALI_DATA_T *)a_pAFCaliData;
    AF_DEBUG_INFO_T rAFDebugInfo;
    MUINT32 aaaDebugSize;
    MINT32 i4TimeOutCnt = 0;

    MY_LOG("[ACDK_CCT_V2_OP_AF_CALI_OPERATION]\n");

    setAFMode(AF_MODE_AFS);
    usleep(500000);    // 500ms
    m_eLIB3A_AFMode = LIB3A_AF_MODE_CALIBRATION;
    m_pIAfAlgo->setAFMode(m_eLIB3A_AFMode);

    usleep(500000);    // 500ms
    while(!isFocusFinish())
    {
        usleep(30000); // 30ms
        i4TimeOutCnt++;
        if (i4TimeOutCnt > 2000)   break;
    }

    getDebugInfo(rAFDebugInfo);
    pAFCaliData->i4Gap = (MINT32)rAFDebugInfo.Tag[3].u4FieldValue;

    for (MINT32 i = 0; i < 512; i++)
    {
        if (rAFDebugInfo.Tag[i+4].u4FieldValue != 0)
        {
            pAFCaliData->i8Vlu[i] = (MINT64)rAFDebugInfo.Tag[i+4].u4FieldValue;  // need fix it
            pAFCaliData->i4Num = i+1;
        }
        else    break;
    }

    pAFCaliData->i4BestPos = m_sAFOutput.i4AFBestPos;

    MY_LOG("[AFCaliData] Num = %d\n", pAFCaliData->i4Num);
    MY_LOG("[AFCaliData] Gap = %d\n", pAFCaliData->i4Gap);
    for (MINT32 i=0; i<pAFCaliData->i4Num; i++)
        MY_LOG("[AFCaliData] Vlu %d = %lld\n", i, pAFCaliData->i8Vlu[i]);
    MY_LOG("[AFCaliData] Pos = %d\n", pAFCaliData->i4BestPos);

    setAFMode(AF_MODE_AFS);
    *a_pOutLen = sizeof(MINT32);

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFSetFocusRange(MVOID *a_pFocusRange)
{
    MY_LOG("[ACDK_CCT_V2_OP_AF_SET_RANGE]\n");
    FOCUS_RANGE_T *pFocusRange = (FOCUS_RANGE_T *)a_pFocusRange;

    m_NVRAM_LENS.rFocusRange = *pFocusRange;

    if (m_pMcuDrv)
    {
        m_pMcuDrv->setMCUInfPos(m_NVRAM_LENS.rFocusRange.i4InfPos,m_i4CurrSensorDev);
        m_pMcuDrv->setMCUMacroPos(m_NVRAM_LENS.rFocusRange.i4MacroPos,m_i4CurrSensorDev);
    }

    if (m_pIAfAlgo)
    {
        m_pIAfAlgo->setAFParam(m_sAFParam, m_sAFConfig, m_NVRAM_LENS.rAFNVRAM);
    }

    MY_LOG("[Inf Pos] = %d\n", m_NVRAM_LENS.rFocusRange.i4InfPos);
    MY_LOG("[Marco Pos] = %d\n", m_NVRAM_LENS.rFocusRange.i4MacroPos);
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFGetFocusRange(MVOID *a_pFocusRange, MUINT32 *a_pOutLen)
{
    MY_LOG("[ACDK_CCT_V2_OP_AF_GET_RANGE]\n");
    FOCUS_RANGE_T *pFocusRange = (FOCUS_RANGE_T *)a_pFocusRange;

    *pFocusRange = m_NVRAM_LENS.rFocusRange;
    *a_pOutLen = sizeof(FOCUS_RANGE_T);

    MY_LOG("[Inf Pos] = %d\n", pFocusRange->i4InfPos);
    MY_LOG("[Marco Pos] = %d\n", pFocusRange->i4MacroPos);
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFGetNVRAMParam(MVOID *a_pAFNVRAM, MUINT32 *a_pOutLen)
{
    MY_LOG("[ACDK_CCT_V2_OP_AF_READ]\n");

    NVRAM_LENS_PARA_STRUCT *pAFNVRAM = reinterpret_cast<NVRAM_LENS_PARA_STRUCT*>(a_pAFNVRAM);

/*
    if (FAILED(NvramDrvMgr::getInstance().init(m_i4CurrSensorDev))) {
         MY_ERR("NvramDrvMgr init fail\n");
         return E_AWB_SENSOR_ERROR;
    }

    NvramDrvMgr::getInstance().getRefBuf(g_pNVRAM_LENS);
    NvramDrvMgr::getInstance().uninit();
    */
    void* p;
    NvBufUtil::getInstance().getBufAndRead(CAMERA_NVRAM_DATA_LENS, m_i4CurrSensorDev, p);
    g_pNVRAM_LENS = (NVRAM_LENS_PARA_STRUCT*) p;


    m_NVRAM_LENS.Version = g_pNVRAM_LENS->Version;
    m_NVRAM_LENS.rFocusRange = g_pNVRAM_LENS->rFocusRange;
    m_NVRAM_LENS.rAFNVRAM= g_pNVRAM_LENS->rAFNVRAM;

    *pAFNVRAM = m_NVRAM_LENS;
    *a_pOutLen = sizeof(NVRAM_LENS_PARA_STRUCT);

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFApplyNVRAMParam(MVOID *a_pAFNVRAM)
{
    MY_LOG("[ACDK_CCT_V2_OP_AF_APPLY]\n");
    NVRAM_LENS_PARA_STRUCT *pAFNVRAM = (NVRAM_LENS_PARA_STRUCT *)a_pAFNVRAM;
    m_NVRAM_LENS = *pAFNVRAM;

    MY_LOG("Apply to Phone[Thres Main]%d\n", m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4THRES_MAIN);
    MY_LOG("Apply to Phone[Thres Sub]%d\n", m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4THRES_MAIN);
    MY_LOG("Apply to Phone[HW_TH]%d\n", m_NVRAM_LENS.rAFNVRAM.sAF_TH.i4HW_TH[0]);   // AF v2.0
    MY_LOG("Apply to Phone[Statgain]%d\n", m_NVRAM_LENS.rAFNVRAM.i4StatGain);

    if (m_pIAfAlgo)
    {
        m_pIAfAlgo->setAFParam(m_sAFParam, m_sAFConfig, m_NVRAM_LENS.rAFNVRAM);
    }
    if (m_pMcuDrv)
    {
        m_pMcuDrv->setMCUInfPos(m_NVRAM_LENS.rFocusRange.i4InfPos,m_i4CurrSensorDev);
        m_pMcuDrv->setMCUMacroPos(m_NVRAM_LENS.rFocusRange.i4MacroPos,m_i4CurrSensorDev);
    }
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFSaveNVRAMParam()
{
    MY_LOG("[ACDK_CCT_V2_OP_AF_SAVE_TO_NVRAM]\n");
/*
    MUINT32 u4SensorID;
    CAMERA_DUAL_CAMERA_SENSOR_ENUM eSensorEnum;

    MRESULT err = S_AF_OK;

    NvramDrvBase* pNvramDrvObj = NvramDrvBase::createInstance();

    NSNvram::BufIF<NVRAM_LENS_PARA_STRUCT>*const pBufIF_Lens = pNvramDrvObj->getBufIF< NVRAM_LENS_PARA_STRUCT>();

    //  Sensor driver.
    SensorHal*const pSensorHal = SensorHal::createInstance();

    switch  ( m_i4CurrSensorDev )
    {
    case ESensorDev_Main:
        eSensorEnum = DUAL_CAMERA_MAIN_SENSOR;
        pSensorHal->sendCommand(SENSOR_DEV_MAIN, SENSOR_CMD_GET_SENSOR_ID, reinterpret_cast<MINT32>(&u4SensorID), 0, 0);
        break;
    case ESensorDev_Sub:
        eSensorEnum = DUAL_CAMERA_SUB_SENSOR;
        pSensorHal->sendCommand(SENSOR_DEV_SUB, SENSOR_CMD_GET_SENSOR_ID, reinterpret_cast<MINT32>(&u4SensorID), 0, 0);
        break;
    case ESensorDev_MainSecond:
        eSensorEnum = DUAL_CAMERA_MAIN_SECOND_SENSOR;
        pSensorHal->sendCommand(SENSOR_DEV_MAIN_2, SENSOR_CMD_GET_SENSOR_ID, reinterpret_cast<MINT32>(&u4SensorID), 0, 0);
        break;
    default:    //  Shouldn't happen.
        MY_ERR("Invalid sensor device: %d", m_i4CurrSensorDev);
        err = E_NVRAM_BAD_PARAM;
        goto lbExit;
    }

    g_pNVRAM_LENS = pBufIF_Lens->getRefBuf(eSensorEnum, u4SensorID);

    MY_LOG("WriteNVRAM from Phone[Thres Main]%d\n", m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4THRES_MAIN);
    MY_LOG("WriteNVRAM from Phone[Thres Sub]%d\n", m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4THRES_MAIN);
    MY_LOG("WriteNVRAM from Phone[HW_TH]%d\n", m_NVRAM_LENS.rAFNVRAM.sAF_TH.i4HW_TH[0][0]);
    MY_LOG("WriteNVRAM from Phone[Statgain]%d\n", m_NVRAM_LENS.rAFNVRAM.i4StatGain);

    g_pNVRAM_LENS->Version = m_NVRAM_LENS.Version;
    g_pNVRAM_LENS->rFocusRange = m_NVRAM_LENS.rFocusRange;
    g_pNVRAM_LENS->rAFNVRAM = m_NVRAM_LENS.rAFNVRAM;

    pBufIF_Lens->flush(eSensorEnum, u4SensorID);

    lbExit:
    if  ( pSensorHal )
        pSensorHal->destroyInstance();

    if ( pNvramDrvObj )
        pNvramDrvObj->destroyInstance();
*/
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFGetFV(MVOID *a_pAFPosIn, MVOID *a_pAFValueOut, MUINT32 *a_pOutLen)
{
    ACDK_AF_POS_T *pAFPos = (ACDK_AF_POS_T *) a_pAFPosIn;
    ACDK_AF_VLU_T *pAFValue = (ACDK_AF_VLU_T *) a_pAFValueOut;

    MY_LOG("[ACDK_CCT_V2_OP_AF_GET_FV]\n");
    pAFValue->i4Num = pAFPos->i4Num;
    setAFMode(AF_MODE_AFS);
    usleep(500000); // 500ms
    setAFMode(AF_MODE_MF);

    for (MINT32 i = 0; i < pAFValue->i4Num; i++)
    {
        setMFPos(pAFPos->i4Pos[i]);
        usleep(500000); // 500ms
        pAFValue->i8Vlu[i] = m_sAFInput.sAFStat.i8Stat24;
        MY_LOG("[FV]pos = %d, value = %lld\n", pAFPos->i4Pos[i], pAFValue->i8Vlu[i]);
    }

    setAFMode(AF_MODE_AFS);
    *a_pOutLen = sizeof(ACDK_AF_VLU_T);
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFEnable()
{
    MY_LOG("[ACDK_CCT_OP_AF_ENABLE]\n");
    m_i4EnableAF = 1;
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFDisable()
{
    MY_LOG("[ACDK_CCT_OP_AF_DISABLE]\n");
    m_i4EnableAF = 0;
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFGetEnableInfo(MVOID *a_pEnableAF, MUINT32 *a_pOutLen)
{
    MY_LOG("[ACDK_CCT_OP_AF_GET_ENABLE_INFO]%d\n", m_i4EnableAF);
    MINT32 *pEnableAF = (MINT32 *)a_pEnableAF;
    *pEnableAF = m_i4EnableAF;
    *a_pOutLen = sizeof(MINT32);
    return S_AF_OK;
}

MRESULT AfMgr::retriggerAFWithFlashOn(MINT32 i4isTurnOn)
{
    MY_LOG("[%s] isTurnOn(%d) CurrAFMode(%d)\n", __FUNCTION__, i4isTurnOn, m_eCurrAFMode);
    if (m_eCurrAFMode == 3 || m_eCurrAFMode == 4)
    {
        if (i4isTurnOn == MTRUE)
        {
            m_i4EnableBackupAFMode = 1;
            m_i4BackupAFModeWithFlash = m_eLIB3A_AFMode;
            if (m_pIAfAlgo)
            {
                char value[PROPERTY_VALUE_MAX] = {'\0'};
                MINT32 DebugEnable = 0;
                property_get("vendor.debug.af_mgr.retrigger", value, "0");
                DebugEnable =  atoi(value);

                if (m_i4LockAFInContiMode == 1)
                    m_pIAfAlgo->cancelAF();

                if (DebugEnable == 0)
                {
                    m_pIAfAlgo->setAFMode(LIB3A_AF_MODE_AFC);
                }
                else
                {
                    m_pIAfAlgo->setAFMode(LIB3A_AF_MODE_AFS);
                    m_pIAfAlgo->triggerAF();
                }
                m_eAFState = NS3A::E_AF_PASSIVE_SCAN;
                MY_LOG("[%s] m_eAFState(%d)\n", __FUNCTION__, m_eAFState);
            }
            else
            {
                MY_LOG("Null m_pIAfAlgo\n");
            }
        }
        else
        {
            if (m_i4EnableBackupAFMode)
            {
                char value[PROPERTY_VALUE_MAX] = {'\0'};
                MINT32 DebugEnable = 0;
                property_get("vendor.debug.af_mgr.backup", value, "0");
                DebugEnable =  atoi(value);

                m_i4EnableBackupAFMode = 0;
                if (m_pIAfAlgo)
                {
                    if (DebugEnable == 0)
                    {
                        if (m_i4BackupAFModeWithFlash != LIB3A_AF_MODE_AFC)
                        {
                            m_pIAfAlgo->setAFMode(m_i4BackupAFModeWithFlash);
                        }
                    }
                    else
                    {
                        m_pIAfAlgo->setAFMode(m_i4BackupAFModeWithFlash);
                    }

                    if (m_i4LockAFInContiMode == 1)
                        m_pIAfAlgo->triggerAF();
                }
                else
                {
                    MY_LOG("Null m_pIAfAlgo\n");
                }
            }
        }
    }
    else if (m_eCurrAFMode == 1 || m_eCurrAFMode == 2)
    {
        if (i4isTurnOn == MTRUE)
        {
            if (m_pIAfAlgo)
            {
                m_pIAfAlgo->triggerAF();
            }
        }
    }

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::triggerAF()
{
    if (m_pIAfAlgo)
        m_pIAfAlgo->triggerAF();

    m_sAFOutput.i4IsAFDone = MFALSE;
    m_isTriggerAFS = TRUE;
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setAFMode(MINT32 eAFMode)
{
    //MINT32 eAFMode = a_eAFMode;
    MY_LOG_IF(m_bDebugEnable, "[setAFMode] preAFMode(%d) to AFMode(%d)\n", m_eCurrAFMode, eAFMode);

    if (m_eCurrAFMode == eAFMode)
        return S_AF_OK;

    switch (eAFMode) {
        case 0: // AF-Single Shot Mode
            m_eLIB3A_AFMode = LIB3A_AF_MODE_OFF;
            break;
        case 1: // AF-Single Shot Mode
            m_eLIB3A_AFMode = LIB3A_AF_MODE_AFS;
            break;
        case 2: // AF Macro Mode
            m_eLIB3A_AFMode = LIB3A_AF_MODE_MACRO;
            break;
        case 3: // AF-Continuous Mode
            m_eLIB3A_AFMode = LIB3A_AF_MODE_AFC_VIDEO;
            break;
        case 4: // AF-Continuous Mode (Video)
            m_eLIB3A_AFMode = LIB3A_AF_MODE_AFC;
            break;
        case 5: // Focus is set at infinity
            m_eLIB3A_AFMode = LIB3A_AF_MODE_INFINITY;
            break;
        case 6: // Manual Focus Mode
            m_eLIB3A_AFMode = LIB3A_AF_MODE_MF;
            break;
        case 7: // AF Full Scan Mode
            m_eLIB3A_AFMode = LIB3A_AF_MODE_FULLSCAN;
            break;
        default:
            m_eLIB3A_AFMode = LIB3A_AF_MODE_AFS;
            break;
    }

    if (m_pIAfAlgo)
        m_pIAfAlgo->setAFMode(m_eLIB3A_AFMode);
    else
        MY_LOG("Null m_pIAfAlgo\n");

    MY_LOG("[setAFMode] CtrlMode %d -> %d, LIB3A_AFMode(%d)\n", m_eCurrAFMode, eAFMode, m_eLIB3A_AFMode);

    m_eCurrAFMode = eAFMode;

    m_i4EnableBackupAFMode = FALSE;
    m_i4SetAFModeCmd = TRUE;
    mi4IsAFDoneFlg_pre = mi4IsAFDoneFlg_cur = m_i4IsAFAlgoSearchDone;
    m_i4ChangeSearchingState = 0;

    return S_AF_OK;
}
EAfState_T AfMgr::getAFState()
{
    NS3A::EAfState_T NewState     = m_eAFState;
    MINT32 i4SetAFModeCmd         = m_i4SetAFModeCmd;
    MINT32 i4AutoFocusCmd         = m_i4AutoFocusCmd;
    MINT32 i4CancelAutoFocusCmd   = m_i4CancelAutoFocusCmd;
    MINT32 i4ChangeSearchingState = m_i4ChangeSearchingState;

    switch(m_eCurrAFMode)
    {
    case 1: //MTK_CONTROL_AF_MODE_AUTO :
    case 2: //MTK_CONTROL_AF_MODE_MACRO :
        if (m_i4ChangeSearchingState)
        {
            if (m_i4SearchDone)
            {
                if (m_eAFState == NS3A::E_AF_ACTIVE_SCAN)
                {
                    if (m_sAFOutput.i4IsFocused == 1) NewState = NS3A::E_AF_FOCUSED_LOCKED;
                    else                              NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
                }
                else
                {
                    NewState = m_eAFState;
                }
            }
            else
            {
                NewState = NS3A::E_AF_ACTIVE_SCAN;
            }
            m_i4ChangeSearchingState = 0;
        }
        if (m_i4SetAFModeCmd)
        {
            NewState = NS3A::E_AF_INACTIVE;
            m_i4SetAFModeCmd = 0;
        }
        if (m_i4AutoFocusCmd)
        {
            NewState = NS3A::E_AF_ACTIVE_SCAN;
            m_i4AutoFocusCmd = 0;
        }
        if (m_i4CancelAutoFocusCmd)
        {
            NewState = NS3A::E_AF_INACTIVE;
            m_i4CancelAutoFocusCmd = 0;
        }

        break;

    case 3: //MTK_CONTROL_AF_MODE_CONTINUOUS_VIDEO :
    case 4: //MTK_CONTROL_AF_MODE_CONTINUOUS_PICTURE :
        if (m_i4ChangeSearchingState)
        {
            if (m_i4SearchDone)
            {
                if (m_i4AuotoFocusInContiPicMode == 1 || m_i4LockAFInContiMode == 1)
                {
                    if (m_sAFOutput.i4IsFocused == 1) NewState = NS3A::E_AF_FOCUSED_LOCKED;
                    else                              NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
                }
                else if (m_eAFState == NS3A::E_AF_PASSIVE_SCAN)
                {
                    if (m_sAFOutput.i4IsFocused == 1) NewState = NS3A::E_AF_PASSIVE_FOCUSED;
                    else                              NewState = NS3A::E_AF_PASSIVE_UNFOCUSED;
                }
                else
                {
                    NewState = m_eAFState;
                }
            }
            else
            {
                NewState = NS3A::E_AF_PASSIVE_SCAN;
            }
            m_i4ChangeSearchingState = 0;
        }
        if (m_i4SetAFModeCmd)
        {
            NewState = NS3A::E_AF_PASSIVE_UNFOCUSED;
            //NewState = NS3A::E_AF_INACTIVE;
            m_i4SetAFModeCmd = 0;
            m_i4AuotoFocusInContiPicMode = 0;
        }
        if (m_i4AutoFocusCmd)
        {
            if (     m_eAFState == NS3A::E_AF_INACTIVE    ) NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
            else if (m_eAFState == NS3A::E_AF_PASSIVE_SCAN)
            {
                if (m_eCurrAFMode == 3/*MTK_CONTROL_AF_MODE_CONTINUOUS_PICTURE*/)
                {
                    /**
                     * During continuous-picture mode :
                     * Eventual transition once the focus is good.
                     * If autofocus(AF_TRIGGER) command is sent during searching, AF will be locked once searching done.
                     */
                    NewState = m_eAFState;
                    m_i4AuotoFocusInContiPicMode = 1;
                }
                else
                {
                    /**
                     * During continuous-video mode :
                     * Immediate transition to lock state
                     */
                    NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
                }
            }
            else if (m_eAFState == NS3A::E_AF_PASSIVE_FOCUSED   ) NewState = NS3A::E_AF_FOCUSED_LOCKED;
            else if (m_eAFState == NS3A::E_AF_PASSIVE_UNFOCUSED ) NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
            else if (m_eAFState == NS3A::E_AF_FOCUSED_LOCKED    ) NewState = NS3A::E_AF_FOCUSED_LOCKED;
            else if (m_eAFState == NS3A::E_AF_NOT_FOCUSED_LOCKED) NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
            else                                                  NewState = m_eAFState;

            m_i4AutoFocusCmd = 0;
        }
        if (m_i4CancelAutoFocusCmd)
        {
            NewState = NS3A::E_AF_INACTIVE;
            #if 0
            if (m_eAFState == NS3A::E_AF_FOCUSED_LOCKED)
            {
                NewState = NS3A::E_AF_PASSIVE_FOCUSED;
                //UnlockAlgo();
            }
            else if (m_eAFState == NS3A::E_AF_NOT_FOCUSED_LOCKED)
            {
                NewState = NS3A::E_AF_PASSIVE_UNFOCUSED;
                //UnlockAlgo();
            }
            else if (m_eAFState == NS3A::E_AF_PASSIVE_SCAN)
            {
                NewState = NS3A::E_AF_INACTIVE;
                CAM_LOGD( "Abort search");
            }
            else
            {
                NewState = m_eAFState;
            }
            #endif
            m_i4CancelAutoFocusCmd = 0;
            m_i4InactiveCntInContiPicMode = 10;
        }
        if (m_i4InactiveCntInContiPicMode > 0)
        {
            if (m_i4InactiveCntInContiPicMode == 1 && m_eAFState == NS3A::E_AF_INACTIVE)
            {
                NewState = NS3A::E_AF_PASSIVE_UNFOCUSED;
                CAM_LOGD( "[%s] force to set AF state %d -> %d", __FUNCTION__, m_eAFState, NewState);
            }
            m_i4InactiveCntInContiPicMode--;
        }
        break;
    case 0: //MTK_CONTROL_AF_MODE_OFF :
    case 5: //MTK_CONTROL_AF_MODE_EDOF :
    default :
        m_i4SetAFModeCmd = 0;
        m_i4ChangeSearchingState = 0;
        m_i4AutoFocusCmd = 0;
        m_i4CancelAutoFocusCmd = 0;
        NewState = NS3A::E_AF_INACTIVE;
        break;
    }

    MY_LOG( "[%s] State(%d -> %d), Mode(%d), SearchDone(%d) Cmd: setAFMode(%d), AutoFocus(%d), CancelAutoFocus(%d), ChangeSearchingState(%d), AuotoFocusInContiPicMode(%d) LockAFInContiMode(%d)",
            __FUNCTION__,
            m_eAFState,
            NewState,
            m_eCurrAFMode,
            m_i4SearchDone,
            i4SetAFModeCmd, 
            i4AutoFocusCmd,
            i4CancelAutoFocusCmd,
            i4ChangeSearchingState,
            m_i4AuotoFocusInContiPicMode,
            m_i4LockAFInContiMode);

    m_eAFState = NewState;
    return m_eAFState;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setAFArea(CameraFocusArea_T a_sAFArea)
{
    MY_LOG_IF(m_bDebugEnable, "[setAFArea][Cnt]%d [L]%d [R]%d [U]%d [B]%d\n", a_sAFArea.u4Count, a_sAFArea.rAreas[0].i4Left, a_sAFArea.rAreas[0].i4Right, a_sAFArea.rAreas[0].i4Top, a_sAFArea.rAreas[0].i4Bottom);

    if (m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC)
        a_sAFArea.u4Count = 0;

    if (m_tcaf_mode && a_sAFArea.u4Count==0)
        return S_AF_OK;

    if ((a_sAFArea.u4Count == 0) || (a_sAFArea.rAreas[0].i4Left == a_sAFArea.rAreas[0].i4Right) || (a_sAFArea.rAreas[0].i4Top == a_sAFArea.rAreas[0].i4Bottom))
    {
        a_sAFArea.u4Count = 1;
        a_sAFArea.rAreas[0].i4Left   = -1000 * m_NVRAM_LENS.rAFNVRAM.i4SPOT_PERCENT_W / 100;
        a_sAFArea.rAreas[0].i4Right  =  1000 * m_NVRAM_LENS.rAFNVRAM.i4SPOT_PERCENT_W / 100;
        a_sAFArea.rAreas[0].i4Top    = -1000 * m_NVRAM_LENS.rAFNVRAM.i4SPOT_PERCENT_H / 100;
        a_sAFArea.rAreas[0].i4Bottom =  1000 * m_NVRAM_LENS.rAFNVRAM.i4SPOT_PERCENT_H / 100;
    }

    if ((m_Users>0) && (memcmp(&m_CameraFocusArea, &a_sAFArea, sizeof(m_CameraFocusArea)) == 0))
        return S_AF_OK;

    m_CameraFocusArea = a_sAFArea;
    if (a_sAFArea.u4Count >= AF_WIN_NUM_SPOT)
        m_sAFInput.sAFArea.i4Count = AF_WIN_NUM_SPOT;
    else
        m_sAFInput.sAFArea.i4Count = a_sAFArea.u4Count;

    if (m_sAFInput.sAFArea.i4Count != 0)
    {
        for (MINT32 i=0; i<m_sAFInput.sAFArea.i4Count; i++)
        {
            a_sAFArea.rAreas[i].i4Left   = (a_sAFArea.rAreas[i].i4Left   +1000) * m_i4AF_in_Hsize / 2000;
            a_sAFArea.rAreas[i].i4Right  = (a_sAFArea.rAreas[i].i4Right  +1000) * m_i4AF_in_Hsize / 2000;
            a_sAFArea.rAreas[i].i4Top    = (a_sAFArea.rAreas[i].i4Top    +1000) * m_i4AF_in_Vsize / 2000;
            a_sAFArea.rAreas[i].i4Bottom = (a_sAFArea.rAreas[i].i4Bottom +1000) * m_i4AF_in_Vsize / 2000;

            m_sAFInput.sAFArea.sRect[i].i4W = a_sAFArea.rAreas[i].i4Right - a_sAFArea.rAreas[i].i4Left;
            m_sAFInput.sAFArea.sRect[i].i4H = a_sAFArea.rAreas[i].i4Bottom - a_sAFArea.rAreas[i].i4Top;
            m_sAFInput.sAFArea.sRect[i].i4X = a_sAFArea.rAreas[i].i4Left;
            m_sAFInput.sAFArea.sRect[i].i4Y = a_sAFArea.rAreas[i].i4Top;
            m_sAFInput.sAFArea.sRect[i].i4Info   = a_sAFArea.rAreas[i].i4Weight;
        }
    }

    MY_LOG("[setAFArea][Cnt]%d [L]%d [R]%d [U]%d [B]%d to [Cnt]%d [W]%d [H]%d [X]%d [Y]%d\n", a_sAFArea.u4Count, a_sAFArea.rAreas[0].i4Left, a_sAFArea.rAreas[0].i4Right, a_sAFArea.rAreas[0].i4Top, a_sAFArea.rAreas[0].i4Bottom, m_sAFInput.sAFArea.i4Count, m_sAFInput.sAFArea.sRect[0].i4W, m_sAFInput.sAFArea.sRect[0].i4H, m_sAFInput.sAFArea.sRect[0].i4X, m_sAFInput.sAFArea.sRect[0].i4Y);

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setCamMode(MINT32 a_eCamMode)
{
    MY_LOG("[setCamMode]%d\n", a_eCamMode);
    m_tcaf_mode=0;
    if (a_eCamMode == eAppMode_ZsdMode)   {m_sAFInput.i4IsZSD = TRUE;}
    else                                  {m_sAFInput.i4IsZSD = FALSE;}
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setZoomWinInfo(MUINT32 u4XOffset, MUINT32 u4YOffset, MUINT32 u4Width, MUINT32 u4Height)
{
    if (((m_sEZoom.i4X == (MINT32)u4XOffset) &&
        (m_sEZoom.i4Y == (MINT32)u4YOffset) &&
        (m_sEZoom.i4W == (MINT32)u4Width) &&
         (m_sEZoom.i4H == (MINT32)u4Height ))
        ||(m_i4EnableAF != 1))
    {
        return S_AF_OK;
    }

    MY_LOG("[setZoomWinInfo][w]%d[h]%d[x]%d[y]%d\n", u4Width, u4Height, u4XOffset, u4YOffset);
    m_sEZoom.i4X = (MINT32)u4XOffset;
    m_sEZoom.i4Y = (MINT32)u4YOffset;
    m_sEZoom.i4W = (MINT32)u4Width;
    m_sEZoom.i4H = (MINT32)u4Height;
    m_tcaf_mode=0;

    if((m_sEZoom.i4X == 0)||(m_sEZoom.i4Y == 0))
    {
        setAF_IN_HSIZE();
        if (m_i4AF_in_Hsize != m_sEZoom.i4W)
            m_i4Factor = m_i4AF_in_Hsize * 100 / m_sEZoom.i4W;
        else
            m_i4Factor = 100;
    }
    /* real digital factor, m_i4Factor is always 1.00 */
    MINT32 i4DzFactor = 100*(2*m_sEZoom.i4X + m_sEZoom.i4W)/m_sEZoom.i4W; /* 1X-> 100, 2X->200, ... */
    /* init window config from customized parameter */
    //MINT32 i4WinCfg=(m_NVRAM_LENS.rAFNVRAM.i4Coef[4]);
    /* error check */
    //if(i4WinCfg<1) i4WinCfg = 1; /*i4WinCfg=0 --> 1 & error check*/
    //if(i4WinCfg>4) i4WinCfg = 4; /*error check*/
    //if(i4WinCfg==4)
    {
        //MY_LOG("[setZoomWinInfo] always DZ, DZ=%d, Bound=%d \n",i4DzFactor,i4WinCfg*100);
        /* i4WinCfg=4-> DigZoomFac>4, AF win no change */
    m_sAFInput.sEZoom.i4X = m_sEZoom.i4X * m_i4Factor / 100;
    m_sAFInput.sEZoom.i4Y = m_sEZoom.i4Y * m_i4Factor / 100;
    m_sAFInput.sEZoom.i4W = m_sEZoom.i4W * m_i4Factor / 100;
    m_sAFInput.sEZoom.i4H = m_sEZoom.i4H * m_i4Factor / 100;
    }
    //else /* i4WinCfg = 1~3 */
    {
        /* i4WinCfg=1-> DigZoomFac>1, AF win no change */
        /* i4WinCfg=2-> DigZoomFac>2, AF win no change */
        /* i4WinCfg=3-> DigZoomFac>3, AF win no change */
        //if(i4DzFactor >= i4WinCfg*100) /* fix to upper bound */
        {
            //MY_LOG("[setZoomWinInfo] >bound2fix, DZ=%d, Bound=%d\n",i4DzFactor,i4WinCfg*100);
            //m_sAFInput.sEZoom.i4W = (2*m_sEZoom.i4X + m_sEZoom.i4W)/i4WinCfg ;
            //m_sAFInput.sEZoom.i4H = (2*m_sEZoom.i4Y + m_sEZoom.i4H)/i4WinCfg ;
            m_sAFInput.sEZoom.i4X = ((2*m_sEZoom.i4X + m_sEZoom.i4W)-m_sAFInput.sEZoom.i4W)/2;
            m_sAFInput.sEZoom.i4Y = ((2*m_sEZoom.i4Y + m_sEZoom.i4H)-m_sAFInput.sEZoom.i4H)/2;
        }
        //else /* (i4DzFactor < i4WinCfg*100), AF win change aligning to digital zoom factor */
        {
            //MY_LOG("[setZoomWinInfo] <bound2DZ, DZ=%d, Bound=%d \n",i4DzFactor,i4WinCfg*100);
            m_sAFInput.sEZoom.i4X = m_sEZoom.i4X ;
            m_sAFInput.sEZoom.i4Y = m_sEZoom.i4Y ;
            m_sAFInput.sEZoom.i4W = m_sEZoom.i4W ;
            m_sAFInput.sEZoom.i4H = m_sEZoom.i4H ;
        }
    }

    MY_LOG("[setZoomWinInfo][w]%d[h]%d[x]%d[y]%d to [w]%d[h]%d[x]%d[y]%d\n", u4Width, u4Height, u4XOffset, u4YOffset, m_sAFInput.sEZoom.i4W, m_sAFInput.sEZoom.i4H, m_sAFInput.sEZoom.i4X, m_sAFInput.sEZoom.i4Y);

    CameraFocusArea_T sAFArea;
    sAFArea.u4Count = 0;
    setAFArea(sAFArea);

    if (m_sAFOutput.i4IsMonitorFV == TRUE)
        setFlkWinConfig();
    else
        setAFWinConfig(m_sAFInput.sAFArea);

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setFDInfo(MVOID* a_sFaces)
{
    MtkCameraFaceMetadata *pFaces = (MtkCameraFaceMetadata *)a_sFaces;
    if(pFaces == NULL)
    {
    MY_LOG("[%s()] FD window is NULL pointer 1\n", __FUNCTION__);
    return S_AF_OK;
    }

    if((pFaces->faces) == NULL)
    {
    MY_LOG("[%s()] FD window is NULL pointer 2\n", __FUNCTION__);
    return S_AF_OK;
    }

    m_tcaf_mode=0;
    CameraFocusArea_T sFDArea;

    sFDArea.rAreas[0].i4Left   = (pFaces->faces->rect[0]+1000) * m_sEZoom.i4W / 2000 + m_sEZoom.i4X;
    sFDArea.rAreas[0].i4Right  = (pFaces->faces->rect[2]+1000) * m_sEZoom.i4W / 2000 + m_sEZoom.i4X;
    sFDArea.rAreas[0].i4Top    = (pFaces->faces->rect[1]+1000) * m_sEZoom.i4H / 2000 + m_sEZoom.i4Y;
    sFDArea.rAreas[0].i4Bottom = (pFaces->faces->rect[3]+1000) * m_sEZoom.i4H / 2000 + m_sEZoom.i4Y;

    m_FDArea.i4Count = (MINT32)pFaces->number_of_faces;
    m_FDArea.sRect[0].i4X = sFDArea.rAreas[0].i4Left;
    m_FDArea.sRect[0].i4Y = sFDArea.rAreas[0].i4Top;
    m_FDArea.sRect[0].i4W = sFDArea.rAreas[0].i4Right - sFDArea.rAreas[0].i4Left;
    m_FDArea.sRect[0].i4H = sFDArea.rAreas[0].i4Bottom - sFDArea.rAreas[0].i4Top;

    if (m_NVRAM_LENS.rAFNVRAM.i4FDWinPercent > 150)   {m_NVRAM_LENS.rAFNVRAM.i4FDWinPercent = 150;}

    m_FDArea.sRect[0].i4X = m_FDArea.sRect[0].i4X + m_FDArea.sRect[0].i4W * ((100 - m_NVRAM_LENS.rAFNVRAM.i4FDWinPercent)>>1) / 100;
    m_FDArea.sRect[0].i4Y = m_FDArea.sRect[0].i4Y + m_FDArea.sRect[0].i4H * ((100 - m_NVRAM_LENS.rAFNVRAM.i4FDWinPercent)>>1) / 100;
    m_FDArea.sRect[0].i4W = m_FDArea.sRect[0].i4W * (m_NVRAM_LENS.rAFNVRAM.i4FDWinPercent) / 100;
    m_FDArea.sRect[0].i4H = m_FDArea.sRect[0].i4H * (m_NVRAM_LENS.rAFNVRAM.i4FDWinPercent) / 100;

    if ((m_FDArea.sRect[0].i4W == 0) || (m_FDArea.sRect[0].i4H == 0))   {
        m_FDArea.i4Count = 0;
    }

    MY_LOG("[setFDInfo]cnt:%d, X:%d Y:%d W:%d H:%d", m_FDArea.i4Count, m_FDArea.sRect[0].i4X, m_FDArea.sRect[0].i4Y, m_FDArea.sRect[0].i4W, m_FDArea.sRect[0].i4H);

    if (m_pIAfAlgo)  {
        m_pIAfAlgo->setFDWin(m_FDArea);
    }
    else   {
        MY_LOG("Null m_pIAfAlgo\n");
    }

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setOTInfo(MVOID* a_sOT)
{
    MtkCameraFaceMetadata *pObtinfo = (MtkCameraFaceMetadata *)a_sOT;
    m_tcaf_mode=0;
    m_FDArea.i4Score=pObtinfo->faces->score;

    CameraFocusArea_T sOBTArea;

    sOBTArea.rAreas[0].i4Left   = (pObtinfo->faces->rect[0]+1000) * m_i4AF_in_Hsize / 2000;
    sOBTArea.rAreas[0].i4Right  = (pObtinfo->faces->rect[2]+1000) * m_i4AF_in_Hsize / 2000;
    sOBTArea.rAreas[0].i4Top    = (pObtinfo->faces->rect[1]+1000) * m_i4AF_in_Vsize / 2000;
    sOBTArea.rAreas[0].i4Bottom = (pObtinfo->faces->rect[3]+1000) * m_i4AF_in_Vsize / 2000;

    m_FDArea.i4Count = (MINT32)pObtinfo->number_of_faces;
    m_FDArea.sRect[0].i4X = sOBTArea.rAreas[0].i4Left;
    m_FDArea.sRect[0].i4Y = sOBTArea.rAreas[0].i4Top;
    m_FDArea.sRect[0].i4W = sOBTArea.rAreas[0].i4Right - sOBTArea.rAreas[0].i4Left;
    m_FDArea.sRect[0].i4H = sOBTArea.rAreas[0].i4Bottom - sOBTArea.rAreas[0].i4Top;

    if (m_NVRAM_LENS.rAFNVRAM.i4FDWinPercent > 100)   {m_NVRAM_LENS.rAFNVRAM.i4FDWinPercent = 100;}

    m_FDArea.sRect[0].i4X = m_FDArea.sRect[0].i4X + m_FDArea.sRect[0].i4W * ((100 - m_NVRAM_LENS.rAFNVRAM.i4FDWinPercent)>>1) / 100;
    m_FDArea.sRect[0].i4Y = m_FDArea.sRect[0].i4Y + m_FDArea.sRect[0].i4H * ((100 - m_NVRAM_LENS.rAFNVRAM.i4FDWinPercent)>>1) / 100;
    m_FDArea.sRect[0].i4W = m_FDArea.sRect[0].i4W * (m_NVRAM_LENS.rAFNVRAM.i4FDWinPercent) / 100;
    m_FDArea.sRect[0].i4H = m_FDArea.sRect[0].i4H * (m_NVRAM_LENS.rAFNVRAM.i4FDWinPercent) / 100;

    if ((m_FDArea.sRect[0].i4W == 0) || (m_FDArea.sRect[0].i4H == 0))   {
        m_FDArea.i4Count = 0;
    }

    MY_LOG("[setOTInfo]cnt:%d, X:%d Y:%d W:%d H:%d", m_FDArea.i4Count, m_FDArea.sRect[0].i4X, m_FDArea.sRect[0].i4Y, m_FDArea.sRect[0].i4W, m_FDArea.sRect[0].i4H);

    if (m_pIAfAlgo)  {
        m_pIAfAlgo->setFDWin(m_FDArea);
    }
    else   {
        MY_LOG("Null m_pIAfAlgo\n");
    }

    return S_AF_OK;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getAFMaxAreaNum(MINT32 eSensorDev)
{
    m_i4CurrSensorDev=(MINT32)eSensorDev;
    IHalSensorList* const pIHalSensorList = IHalSensorList::get();
    SensorStaticInfo rSensorStaticInfo;
    switch(m_i4CurrSensorDev)
    {
        case ESensorDev_Main:
            pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_MAIN, &rSensorStaticInfo);
            break;
        case ESensorDev_Sub:
            pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_SUB, &rSensorStaticInfo);
            break;
        case ESensorDev_MainSecond:
            pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_MAIN_2, &rSensorStaticInfo);
            break;
        default:
            MY_ERR("Invalid sensor device: %d", m_i4CurrSensorDev);
    }
    m_i4CurrSensorId=rSensorStaticInfo.sensorDevID;
    MCUDrv::lensSearch(m_i4CurrSensorDev, m_i4CurrSensorId);
    m_i4CurrLensId = MCUDrv::getCurrLensID(m_i4CurrSensorDev);

    MY_LOG("[lens][SensorDev]0x%04x, [SensorId]0x%04x, [CurrLensId]0x%04x", m_i4CurrSensorDev, m_i4CurrSensorId, m_i4CurrLensId);

    if (m_i4CurrLensId == 0xFFFF)
    {
        MY_LOG("[getAFMaxAreaNum][AF disable]%d\n", 0);
        return 0;
    }
    else
    {
        MY_LOG("[getAFMaxAreaNum]%d\n", AF_WIN_NUM_SPOT);
        return AF_WIN_NUM_SPOT;
    }

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getMaxLensPos()
{
    if (m_pMcuDrv)
    {
        mcuMotorInfo MotorInfo;
        m_pMcuDrv->getMCUInfo(&MotorInfo,m_i4CurrSensorDev);
        MY_LOG("[getMaxLensPos]%d\n", (MINT32)MotorInfo.u4MacroPosition);
        return (MINT32)MotorInfo.u4MacroPosition;
    }
    else
    {
        MY_LOG("[getMaxLensPos]m_pMcuDrv NULL\n");
        return 0;
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getMinLensPos()
{
    if (m_pMcuDrv)
    {
        mcuMotorInfo MotorInfo;
        m_pMcuDrv->getMCUInfo(&MotorInfo,m_i4CurrSensorDev);
        MY_LOG("[getMinLensPos]%d\n", (MINT32)MotorInfo.u4InfPosition);
        return (MINT32)MotorInfo.u4InfPosition;
    }
    else
    {
        MY_LOG("[getMinLensPos]m_pMcuDrv NULL\n");
        return 0;
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getAFBestPos()
{
    return m_sAFOutput.i4AFBestPos;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getAFPos()
{
    return m_sAFOutput.i4AFPos;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getAFStable()
{
    return m_sAFOutput.i4IsAFDone;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getLensMoving()
{
    return m_sAFInput.sLensInfo.bIsMotorMoving;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getAFTableOffset()
{
// AF v2.0
//    if (m_sAFInput.i4IsZSD)
//    {
//        return m_NVRAM_LENS.rAFNVRAM.sZSD_AF_Coef.sTABLE.i4Offset;
//    }
//    else
//    {
        if (m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC_VIDEO)
            return m_NVRAM_LENS.rAFNVRAM.sVAFC_Coef.sTABLE.i4Offset;
        else
            return m_NVRAM_LENS.rAFNVRAM.sAF_Coef.sTABLE.i4Offset;
//    }
    }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getAFTableMacroIdx()
{
// AF v2.0
//    if (m_sAFInput.i4IsZSD)
//    {
//        return m_NVRAM_LENS.rAFNVRAM.sZSD_AF_Coef.sTABLE.i4NormalNum;
//    }
//    else
//    {
        if (m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC_VIDEO)
            return m_NVRAM_LENS.rAFNVRAM.sVAFC_Coef.sTABLE.i4NormalNum;
        else
            return m_NVRAM_LENS.rAFNVRAM.sAF_Coef.sTABLE.i4NormalNum;
//    }
    }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getAFTableIdxNum()
{
    return AF_TABLE_NUM;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID* AfMgr::getAFTable()
{
// AF v2.0
//    if (m_sAFInput.i4IsZSD)
//    {
//        return (MVOID*)m_NVRAM_LENS.rAFNVRAM.sZSD_AF_Coef.sTABLE.i4Pos;
//    }
//    else
//    {
        if (m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC_VIDEO)
            return (MVOID*)m_NVRAM_LENS.rAFNVRAM.sVAFC_Coef.sTABLE.i4Pos;

        else
            return (MVOID*)m_NVRAM_LENS.rAFNVRAM.sAF_Coef.sTABLE.i4Pos;
//    }
    }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setMFPos(MINT32 a_i4Pos)
{
    if (((m_eLIB3A_AFMode == LIB3A_AF_MODE_MF) || (m_eLIB3A_AFMode == LIB3A_AF_MODE_OFF)) && (m_sAFOutput.i4AFPos != a_i4Pos))
    {
        MY_LOG("[setMFPos]%d\n", a_i4Pos);
        m_i4MFPos = a_i4Pos;

        if (a_i4Pos==(MINT32)0xFFFFFFFF)
        {
            MY_LOG("[setMFPos]skip set mf pos\n");
            return S_AF_OK;
        }

        if (m_pIAfAlgo)
            m_pIAfAlgo->setMFPos(m_i4MFPos);
        else
            MY_LOG("Null m_pIAfAlgo\n");
        }
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setFullScanstep(MINT32 a_i4Step)
{
    if (m_sAFInput.i4FullScanStep != a_i4Step)
    {
        MY_LOG("[setFullScanstep]%d\n", a_i4Step);
        m_sAFInput.i4FullScanStep = a_i4Step;
    }
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AF_FULL_STAT_T AfMgr::getFLKStat()
{
     if(m_i4FLKValid==0)    m_sAFFullStat.bValid=MTRUE;
     else                   m_sAFFullStat.bValid=MFALSE;
     MY_LOG_IF((m_bDebugEnable&1), "[getFLKStat]bValid %d\n",m_sAFFullStat.bValid);

    return m_sAFFullStat;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::doAF(MVOID *pAFStatBuf)
{
    if (m_i4EnableAF == 0)
    {
        m_sAFOutput.i4IsAFDone = 1;
        m_sAFOutput.i4IsFocused = 0;
        m_sAFOutput.i4AFPos = 0;
        MY_LOG("disableAF");
        return S_AF_OK;
    }
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("vendor.debug.af_motor.disable", value, "0");
    m_bMotorDisable = atoi(value);
    property_get("vendor.debug.af_motor.position", value, "0");
    m_MotorManual = atoi(value);

    printAFConfigLog0();
    printAFConfigLog1();

    AE_MODE_CFG_T rPreviewInfo;
    IAeMgr::getInstance().getPreviewParams(m_i4CurrSensorDev, rPreviewInfo);

    MUINT8 iYvalue;
    AEMeterArea_T rWinSize;
    rWinSize.i4Left   = m_CameraFocusArea.rAreas[0].i4Left;
    rWinSize.i4Right  = m_CameraFocusArea.rAreas[0].i4Right;
    rWinSize.i4Top    = m_CameraFocusArea.rAreas[0].i4Top;
    rWinSize.i4Bottom = m_CameraFocusArea.rAreas[0].i4Bottom;

    IAeMgr::getInstance().getAEMeteringYvalue(m_i4CurrSensorDev, rWinSize, &iYvalue);

    m_sAFInput.i8GSum = (MINT64)iYvalue;
    m_sAFInput.i4ISO = (MINT32)rPreviewInfo.u4RealISO;
    m_sAFInput.i4IsAEStable = IAeMgr::getInstance().IsAEStable(m_i4CurrSensorDev);
    m_sAFInput.i4SceneLV = IAeMgr::getInstance().getLVvalue(m_i4CurrSensorDev, MTRUE);

    // AF v1.2
    // i4Coef[7] > 0: enable new scenechange , else: disable
    if( (m_NVRAM_LENS.rAFNVRAM.i4SceneMonitorLevel > 0)
      ||(m_NVRAM_LENS.rAFNVRAM.i4VdoSceneMonitorLevel > 0) )
    {
        m_pIAfAlgo->setAEBlockInfo(m_aeBlockV, 25);
    }

    // i4Coef[5] == 1:enable SensorListener, 0: disable
    if(m_NVRAM_LENS.rAFNVRAM.i4SensorEnable > 0)
    {
        MY_LOG_IF((m_bDebugEnable&1),"SensorEventAF gAcceValid=%d gPreAcceTS=%lld gAcceTS=%lld gAcceInfo[]=(%d %d %d)\n"
                  , gListenEnAcce, gPreAcceTS, gAcceTS, gAcceInfo[0], gAcceInfo[1], gAcceInfo[2]);
        MY_LOG_IF((m_bDebugEnable&1),"SensorEventAF gGyroValid=%d gPreGyroTS=%lld gGyroTS=%lld gGyroInfo[]=(%d %d %d)\n"
                  , gListenEnGyro, gPreGyroTS, gGyroTS, gGyroInfo[0], gGyroInfo[1], gGyroInfo[2]);
        if( gListenEnAcce && (gAcceTS!=gPreAcceTS) )
        {
            MY_LOG_IF((m_bDebugEnable&1),"SensorEventAF AcceValid");
            m_pIAfAlgo->setAcceSensorInfo(gAcceInfo, SENSOR_ACCE_SCALE);
        }
        else
        {
            MY_LOG_IF((m_bDebugEnable&1),"SensorEventAF AcceInValid");
            m_pIAfAlgo->setAcceSensorInfo(gAcceInfo, 0);    // set scale 0 means invalid to algo
        }

        if( gListenEnGyro && (gGyroTS!=gPreGyroTS) )
        {
            MY_LOG_IF((m_bDebugEnable&1),"SensorEventAF GyroValid");
            m_pIAfAlgo->setGyroSensorInfo(gGyroInfo, SENSOR_GYRO_SCALE);
        }
        else
        {
            MY_LOG_IF((m_bDebugEnable&1),"SensorEventAF GyroInValid");
            m_pIAfAlgo->setGyroSensorInfo(gGyroInfo, 0);    // set scale 0 means invalid to algo
        }

    }
    getLensInfo(m_sAFInput.sLensInfo);

    if (m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC || m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC_VIDEO)
    {
        if (m_sAFOutput.i4IsMonitorFV == TRUE)
        {
        m_sAFInput.sAFStat = Trans4WintoOneStat(pAFStatBuf);
        #ifdef FLKPAUS
        //toDo: change function or remove (flicker interface is same as mt6595 kk1.mp11)
        //if(m_pFlickerHal)
          //  m_pFlickerHal->resume();
        #endif
    }
        else
        {
        m_sAFInput.sAFStat = TransAFtoOneStat(pAFStatBuf);
        #ifdef FLKPAUS
        //toDo: change function or remove (flicker interface is same as mt6595 kk1.mp11)
        //if(m_pFlickerHal)
          // m_pFlickerHal->pause();
        #endif
    }
    }
    else   //AFS mode or else
    {
        m_FDArea.i4Count = 0;
        m_sAFOutput.i4FDDetect=0;
        if((m_sAFOutput.i4IsAFDone==0) && (    m_flkwin_syncflag==0)) //doing AF
        {
        m_sAFInput.sAFStat = TransAFtoOneStat(pAFStatBuf);
        #ifdef FLKPAUS
            //MY_LOG("FLKResume\n");
        //toDo: change function or remove (flicker interface is same as mt6595 kk1.mp11)
        //if(m_pFlickerHal)
          // m_pFlickerHal->pause();
        #endif
    }
        else
        {
            m_sAFInput.sAFStat = Trans4WintoOneStat(pAFStatBuf);
            #ifdef FLKPAUS
            //MY_LOG("FLKResume\n");
            //toDo: change function or remove (flicker interface is same as mt6595 kk1.mp11)
            //if(m_pFlickerHal)
              //  m_pFlickerHal->resume();
            #endif
        }
    }

    m_sAFFullStat = TransToFullStat(pAFStatBuf);

    if( (m_sAFOutput.i4IsAFDone == MFALSE) && (m_flkwin_syncflag>0) && m_NonInitState
     && (m_eLIB3A_AFMode == LIB3A_AF_MODE_AFS || m_eLIB3A_AFMode == LIB3A_AF_MODE_MACRO) )
        MY_LOG("AF win Preparing\n");
    else
    {
        if (m_pIAfAlgo)
        {
            m_pIAfAlgo->handleAF(m_sAFInput, m_sAFOutput);
            if(m_eLIB3A_AFMode == LIB3A_AF_MODE_AFS && m_pIAfAlgo->getAFState()!=E_AF_PASSIVE_FOCUSED)
                m_isTriggerAFS=FALSE;

            if (m_pIAfAlgo->getAFState() == E_AF_PASSIVE_SCAN || m_pIAfAlgo->getAFState() == E_AF_ACTIVE_SCAN)
            {
                m_i4IsAFAlgoSearchDone = 0;
            }
            else
            {
                m_i4IsAFAlgoSearchDone = 1;
            }
            MY_LOG_IF(m_bDebugEnable,"trigger %d, status %d\n", m_isTriggerAFS, m_pIAfAlgo->getAFState());

        }
        else
        {
            MY_LOG("Null m_pIAfAlgo\n");
        }
    }

    if (m_pMcuDrv)
    {
        MY_LOG("[AFStatH]%lld [AFStatV]%lld [moveMCU] %d, [afdone] %d, [algostate] %d ", m_sAFInput.sAFStat.i8Stat24, m_sAFInput.sAFStat.i8StatV,m_sAFOutput.i4AFPos, m_i4IsAFAlgoSearchDone, m_pIAfAlgo->getAFState());
        if(m_bMotorDisable==0)
            m_pMcuDrv->moveMCU(m_sAFOutput.i4AFPos,m_i4CurrSensorDev);
        else
            MY_LOG("[ADB Disable Motor]\n");
        if((m_MotorManual!=0) && (m_bMotorDisable==1))
            m_pMcuDrv->moveMCU(m_MotorManual,m_i4CurrSensorDev);
    }
    else  MY_LOG("Null m_pMcuDrv\n");

    setAFWinTH(m_sAFOutput.sAFStatConfig);
    setGMR(m_sAFOutput.sAFStatConfig);

    if (m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC || m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC_VIDEO)
    {
        if (m_sAFOutput.i4IsMonitorFV == TRUE)
        setFlkWinConfig();
        else
        setAFWinConfig(m_sAFOutput.sAFArea);
    }
    else    //AFS mode or else
    {
        m_sAFOutput.i4FDDetect=0;
        if(m_sAFOutput.i4IsAFDone==0)  //Doing AF
        setAFWinConfig(m_sAFOutput.sAFArea);
        else    setFlkWinConfig();
        if(m_sAFParam.i4AFC_MODE==2)  m_tcaf_mode=1;
    }

    mi4IsAFDoneFlg_pre = mi4IsAFDoneFlg_cur;
    mi4IsAFDoneFlg_cur = m_i4IsAFAlgoSearchDone;

    if( mi4IsAFDoneFlg_pre != mi4IsAFDoneFlg_cur)
    {
        m_i4ChangeSearchingState = TRUE;
        if( mi4IsAFDoneFlg_cur==1)
            m_i4SearchDone = MTRUE;
        else
            m_i4SearchDone = MFALSE;
    }

    // set AF info
    AF_INFO_T sAFInfo;
    sAFInfo.i4AFPos = m_sAFOutput.i4AFPos;
    IspTuningMgr::getInstance().setAFInfo(sAFInfo);
    if(m_sAFOutput.i4IsAFDone)
        m_NonInitState=MTRUE;
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::setFlkWinConfig()
{
    m_flkwin_syncflag  = 2;
    MY_LOG_IF(m_bDebugEnable, "[setFlkWinConfig]\n");
    MUINTPTR handle;

    MINT32 i4WOri = Boundary((WIN_SIZE_MIN*6 +WIN_POS_MIN*2),m_i4AF_in_Hsize, (WIN_SIZE_MAX*6+WIN_POS_MIN*2));
    MINT32 i4HOri = Boundary((WIN_SIZE_MIN*6 +WIN_POS_MIN*2),m_i4AF_in_Vsize, (WIN_SIZE_MAX*6+WIN_POS_MIN*2));
    MINT32  i4W = (((i4WOri-(WIN_POS_MIN*2))/6)>>1)<<1;
    MINT32  i4H = (((i4HOri-(WIN_POS_MIN*2))/6)>>1)<<1;
    MINT32 i4X = WIN_POS_MIN;
    MINT32 i4Y = WIN_POS_MIN;
    MINT32 wintmp;

    if (MFALSE == sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_GET_MODULE_HANDLE,
                                   NSImageio_FrmB::NSIspio_FrmB::EModule_AF,
                                   (MUINTPTR)&handle,
                                   (MUINTPTR)(&("AFMgr::setFlkWinConfig()"))))
    {
        MY_ERR("EPIPECmd_GET_MODULE_HANDLE setFlkWinConfig fail");
    }
    else
    {
    // 13 bits (8192x8192) - double buffer, "must even position"
        wintmp=(Boundary(WIN_POS_MIN, i4X,            i4WOri-WIN_POS_MIN)&0x1FFE)+ ((Boundary(WIN_POS_MIN, i4X + i4W  , i4WOri-WIN_POS_MIN)&0x1FFE)<<16);
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_WINX01,  wintmp );
        wintmp=(Boundary(WIN_POS_MIN, i4X + i4W*2, i4WOri-WIN_POS_MIN)&0x1FFE)+ ((Boundary(WIN_POS_MIN, i4X + i4W*3, i4WOri-WIN_POS_MIN)&0x1FFE)<<16);
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_WINX23, wintmp );
        wintmp=(Boundary(WIN_POS_MIN, i4X + i4W*4, i4WOri-WIN_POS_MIN)&0x1FFE)+ ((Boundary(WIN_POS_MIN, i4X + i4W*5, i4WOri-WIN_POS_MIN)&0x1FFE)<<16);
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_WINX45, wintmp );

        wintmp=(Boundary(WIN_POS_MIN, i4Y,            i4HOri-WIN_POS_MIN)&0x1FFE)+ ((Boundary(WIN_POS_MIN, i4Y + i4H, i4HOri-WIN_POS_MIN)&0x1FFE)<<16);
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_WINY01,wintmp);
        wintmp=(Boundary(WIN_POS_MIN, i4Y + i4H*2, i4HOri-WIN_POS_MIN)&0x1FFE)+ ((Boundary(WIN_POS_MIN, i4Y + i4H*3, i4HOri-WIN_POS_MIN)&0x1FFE)<<16);
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_WINY23,wintmp);
        wintmp=(Boundary(WIN_POS_MIN, i4Y + i4H*4, i4HOri-WIN_POS_MIN)&0x1FFE)+ ((Boundary(WIN_POS_MIN, i4Y + i4H*5, i4HOri-WIN_POS_MIN)&0x1FFE)<<16);
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_WINY45,wintmp);

        // 10 bits (1022x1022) - double buffer
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_SIZE, (i4W & 0x3FF) + ((i4H&0x3FF)<<16) );

    if (m_sAFOutput.i4FDDetect==0)
    {
        // 13 bits (8192x8192) - double buffer, "must even position"
            IOPIPE_SET_MODUL_REG(handle, CAM_AF_WIN_E, (Boundary(FL_WIN_POS_MIN, i4X + i4W*2, i4WOri - FL_WIN_POS_MIN) & 0x1FFE)
            + ((Boundary(FL_WIN_POS_MIN, i4Y + i4H*2, i4HOri - FL_WIN_POS_MIN)&0x1FFE)<<16) );
        // 12 bits (4096x4096) - double buffer
            IOPIPE_SET_MODUL_REG(handle, CAM_AF_SIZE_E, (Boundary(FL_WIN_SIZE_MIN, i4W*2, FL_WIN_SIZE_MAX) & 0xFFE)
        + ((Boundary(FL_WIN_SIZE_MIN, i4H*2, FL_WIN_SIZE_MAX)&0xFFE)<<16) );
    }
    else
    {
            MINT32 i4XE = Boundary(FL_WIN_POS_MIN, (m_FDArea.sRect[0].i4X>>1)<<1, i4WOri - FL_WIN_POS_MIN - FL_WIN_SIZE_MIN);
            MINT32 i4YE = Boundary(FL_WIN_POS_MIN, (m_FDArea.sRect[0].i4Y>>1)<<1, i4HOri - FL_WIN_POS_MIN - FL_WIN_SIZE_MIN);
        // 13 bits (8192x8192) - double buffer, "must even position"
            IOPIPE_SET_MODUL_REG(handle, CAM_AF_WIN_E, (i4XE & 0x1FFE) + ((i4YE&0x1FFE)<<16) );

        // 12 bits (4096x4096) - double buffer
            IOPIPE_SET_MODUL_REG(handle, CAM_AF_SIZE_E, (Boundary(FL_WIN_SIZE_MIN, (m_FDArea.sRect[0].i4W>>1)<<1, i4WOri - i4XE - FL_WIN_POS_MIN) & 0xFFE)
            + ((Boundary(FL_WIN_SIZE_MIN, (m_FDArea.sRect[0].i4H>>1)<<1, i4HOri - i4YE - FL_WIN_POS_MIN)&0xFFE)<<16) );
        }
        if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_SET_MODULE_CFG_DONE, handle, MNULL, MNULL))
            MY_ERR("EPIPECmd_SET_MODULE_CFG_DONE setFlkWinConfig fail");
    }
    if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_RELEASE_MODULE_HANDLE, handle, (MUINTPTR)(&("AFMgr::setFlkWinConfig()")), MNULL))
        MY_ERR("EPIPECmd_SET_MODULE_CFG_DONE setFlkWinConfig fail");
    }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::setAFWinConfig(AF_AREA_T a_sAFArea)
{
    if(m_flkwin_syncflag)  m_flkwin_syncflag--;
    MY_LOG("[setAFWinConfig][cnt]%d[w]%d[h]%d[x]%d[y]%d\n", a_sAFArea.i4Count, a_sAFArea.sRect[0].i4W, a_sAFArea.sRect[0].i4H, a_sAFArea.sRect[0].i4X, a_sAFArea.sRect[0].i4Y);
    if ((a_sAFArea.i4Count != 1) || (a_sAFArea.sRect[0].i4W == 0) || (a_sAFArea.sRect[0].i4H == 0))   return;

    MINT32 i4WOri = Boundary((WIN_SIZE_MIN*6 +WIN_POS_MIN*2),m_i4AF_in_Hsize, (WIN_SIZE_MAX*6+WIN_POS_MIN*2));
    MINT32 i4HOri = Boundary((WIN_SIZE_MIN*6 +WIN_POS_MIN*2),m_i4AF_in_Vsize, (WIN_SIZE_MAX*6+WIN_POS_MIN*2));
    MUINTPTR handle;
    MINT32 i4W = Boundary(WIN_SIZE_MIN, (((a_sAFArea.sRect[0].i4W/6)>>1)<<1), WIN_SIZE_MAX);
    MINT32 i4H = Boundary(WIN_SIZE_MIN, (((a_sAFArea.sRect[0].i4H/6)>>1)<<1), WIN_SIZE_MAX);
    MINT32 i4X = Boundary(WIN_POS_MIN, (a_sAFArea.sRect[0].i4X >>1)<<1, i4WOri-WIN_POS_MIN-i4W*6);
    MINT32 i4Y = Boundary(WIN_POS_MIN, (a_sAFArea.sRect[0].i4Y >>1)<<1, i4HOri-WIN_POS_MIN-i4H*6);
    MINT32 wintmp;

    if (MFALSE ==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_GET_MODULE_HANDLE,NSImageio_FrmB::NSIspio_FrmB::EModule_AF,
                                           (MUINTPTR)&handle, (MUINTPTR)(&("AFMgr::setAFWinConfig()"))))
    {
        MY_ERR("EPIPECmd_GET_MODULE_HANDLE setAFWinConfig fail");
    }
    else
    {
        if(i4X + i4W*6 + WIN_POS_MIN >= i4WOri)
        {
            if((i4WOri - WIN_POS_MIN*2- i4X) > (WIN_SIZE_MIN*6))
                i4W = ((((i4WOri - WIN_POS_MIN*2- i4X)/6)>>1)<<1) ;
        else
        {
                i4W = ((((i4WOri - WIN_POS_MIN*2)/6)>>1)<<1);
            i4X =WIN_POS_MIN;
        }

        MY_LOG("[setAFWinConfig] over Hsiz %d\n", i4X + i4W*6);
    }
        if(i4Y + i4H*6 + WIN_POS_MIN >= i4HOri)
    {
            if((i4HOri - WIN_POS_MIN*2- i4Y) >(WIN_SIZE_MIN*6))
                i4H = ((((i4HOri - WIN_POS_MIN*2- i4Y)/6)>>1)<<1) ;
        else
        {
                i4H = ((((i4HOri - WIN_POS_MIN*2)/6)>>1)<<1);
            i4Y =WIN_POS_MIN;
        }
        MY_LOG("[setAFWinConfig]over Vsiz %d\n", i4Y + i4H*6);
    }
    // 13 bits (8192x8192) - double buffer, "must even position"
        wintmp= (i4X    &     0x1FFE) + (((i4X + i4W  )&0x1FFE)<<16) ;
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_WINX01,  wintmp);
        wintmp=((i4X + i4W*2)&0x1FFE) + (((i4X + i4W*3)&0x1FFE)<<16) ;
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_WINX23, wintmp);
        wintmp=((i4X + i4W*4)&0x1FFE) + (((i4X + i4W*5)&0x1FFE)<<16) ;
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_WINX45, wintmp );
        wintmp=  (i4Y    &     0x1FFE) + (((i4Y + i4H  )&0x1FFE)<<16);
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_WINY01,  wintmp );
        wintmp= ((i4Y + i4H*2)&0x1FFE) + (((i4Y + i4H*3)&0x1FFE)<<16);
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_WINY23, wintmp );
        wintmp= ((i4Y + i4H*4)&0x1FFE) + (((i4Y + i4H*5)&0x1FFE)<<16);
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_WINY45, wintmp );

        // 10 bits (1022x1022) - double buffer
        wintmp=(i4W & 0x3FE) + ((i4H&0x3FE)<<16);
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_SIZE, wintmp );

        wintmp=(Boundary(FL_WIN_POS_MIN, i4X, i4WOri - FL_WIN_POS_MIN) & 0x1FFE)
        + ((Boundary(FL_WIN_POS_MIN, i4Y, i4HOri - FL_WIN_POS_MIN)&0x1FFE)<<16);
        IOPIPE_SET_MODUL_REG(handle,  CAM_AF_WIN_E,  wintmp);

        // 12 bits (4096x4096) - double buffer
        wintmp=(Boundary(FL_WIN_SIZE_MIN, i4W, FL_WIN_SIZE_MAX) & 0xFFE)
        + ((Boundary(FL_WIN_SIZE_MIN, i4H, FL_WIN_SIZE_MAX)&0xFFE)<<16);
        IOPIPE_SET_MODUL_REG(handle,  CAM_AF_SIZE_E, wintmp );

    if (m_sAFOutput.i4FDDetect==0)
    {
            IOPIPE_SET_MODUL_REG(handle, CAM_AF_WIN_E,  (Boundary(FL_WIN_POS_MIN, i4X, i4WOri - FL_WIN_POS_MIN) & 0x1FFE)
            + ((Boundary(FL_WIN_POS_MIN, i4Y, i4HOri - FL_WIN_POS_MIN)&0x1FFE)<<16));

        // 12 bits (4096x4096) - double buffer
            IOPIPE_SET_MODUL_REG(handle, CAM_AF_SIZE_E, (Boundary(FL_WIN_SIZE_MIN, i4W, FL_WIN_SIZE_MAX) & 0xFFE)
            + ((Boundary(FL_WIN_SIZE_MIN, i4H, FL_WIN_SIZE_MAX)&0xFFE)<<16) );
    }
    else if (m_sAFOutput.i4IsAFDone==1)
    {
            MINT32 i4XE = Boundary(FL_WIN_POS_MIN, (m_FDArea.sRect[0].i4X>>1)<<1, i4WOri - FL_WIN_POS_MIN - FL_WIN_SIZE_MIN);
            MINT32 i4YE = Boundary(FL_WIN_POS_MIN, (m_FDArea.sRect[0].i4Y>>1)<<1, i4HOri - FL_WIN_POS_MIN - FL_WIN_SIZE_MIN);
        // 13 bits (8192x8192) - double buffer, "must even position"
            IOPIPE_SET_MODUL_REG(handle, CAM_AF_WIN_E, (i4XE & 0x1FFE) + ((i4YE&0x1FFE)<<16) );

        // 12 bits (4096x4096) - double buffer
            IOPIPE_SET_MODUL_REG(handle, CAM_AF_SIZE_E, (Boundary(FL_WIN_SIZE_MIN, (m_FDArea.sRect[0].i4W>>1)<<1, i4WOri - i4XE - FL_WIN_POS_MIN) & 0xFFE)
            + ((Boundary(FL_WIN_SIZE_MIN, (m_FDArea.sRect[0].i4H>>1)<<1, i4HOri - i4YE - FL_WIN_POS_MIN)&0xFFE)<<16) );
        }
        if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_SET_MODULE_CFG_DONE, handle, MNULL, MNULL))
            MY_ERR("EPIPECmd_SET_MODULE_CFG_DONE setAFWinConfig fail");
    }
    if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_RELEASE_MODULE_HANDLE, handle, (MUINTPTR)(&("AFMgr::setAFWinConfig")), MNULL))
        MY_ERR("EPIPECmd_SET_MODULE_CFG_DONE setAFWinConfig fail");
    }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::setAF_IN_HSIZE()
{
    MUINTPTR handle;
    if (!m_pIspReg)
    {
        MY_LOG("[setAF_IN_HSIZE] m_pIspReg NULL");
        return;
    }

    MINT32 i4Pxl_S, i4Pxl_E, i4lin_S, i4lin_E;

    i4Pxl_S = ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_TG_SEN_GRAB_PXL, PXL_S);
    i4Pxl_E = ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_TG_SEN_GRAB_PXL, PXL_E);
    i4lin_S = ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_TG_SEN_GRAB_LIN, LIN_S);
    i4lin_E = ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_TG_SEN_GRAB_LIN, LIN_E);

    MINT32 i4Hrz_En = ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_CTL_EN1, HRZ_EN);
    MINT32 i4SGG_Hrz_Sel = ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_CTL_SRAM_MUX_CFG, SGG_HRZ_SEL);   // 1: SGG after HRZ
    m_i4AF_in_Vsize = i4lin_E - i4lin_S;
    if (i4SGG_Hrz_Sel && i4Hrz_En)
        m_i4AF_in_Hsize = ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_HRZ_OUT, HRZ_OUTSIZE);  // need confirm
    else
        m_i4AF_in_Hsize = i4Pxl_E - i4Pxl_S;

    MY_LOG("[setAF_IN_HSIZE_isp][SensorDev]%d [Pxl]%d [lin]%d [H]%d [V]%d\n",
        m_i4CurrSensorDev,
        i4Pxl_E - i4Pxl_S,
        i4lin_E - i4lin_S,
        m_i4AF_in_Hsize,
        m_i4AF_in_Vsize);


    if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_GET_TG_OUT_SIZE, m_i4SensorIdx, (MUINTPTR)(&m_i4AF_in_Hsize), (MUINTPTR)(&m_i4AF_in_Vsize)))
        MY_ERR("EPIPECmd_GET_TG_OUT_SIZE fail");

    MY_LOG("[setAF_IN_HSIZE][SensorDev]%d [H]%d [V]%d\n", m_i4CurrSensorDev, m_i4AF_in_Hsize,  m_i4AF_in_Vsize);


    if (MFALSE ==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_GET_MODULE_HANDLE,NSImageio_FrmB::NSIspio_FrmB::EModule_AF,
           (MUINTPTR)&handle, (MUINTPTR)(&("AFMgr::setAF_IN_HSIZE()"))))
    {
        MY_ERR("EPIPECmd_GET_MODULE_HANDLE setAF_IN_HSIZE fail");
    }
    else
    {
        IOPIPE_SET_MODUL_REG(handle,  CAM_AF_IN_SIZE, m_i4AF_in_Hsize);
        if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_SET_MODULE_CFG_DONE, handle, MNULL, MNULL))
                MY_ERR("EPIPECmd_SET_MODULE_CFG_DONE setAF_IN_HSIZE fail");
    }
    if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_RELEASE_MODULE_HANDLE, handle, (MUINTPTR)(&("AFMgr::setAF_IN_HSIZE")), MNULL))
        MY_ERR("EPIPECmd_SET_MODULE_CFG_DONE setAF_D_IN_HSIZE fail");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::setAFWinTH(AF_CONFIG_T a_sAFConfig)
{
    MUINTPTR handle;
    if (MFALSE ==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_GET_MODULE_HANDLE,NSImageio_FrmB::NSIspio_FrmB::EModule_AF,
           (MUINTPTR)&handle, (MUINTPTR)(&("AFMgr::setAFWinTH()"))))
    {
        MY_ERR("EPIPECmd_GET_MODULE_HANDLE setAFWinTH fail");
    }
    else
    {
        MY_LOG_IF(m_bDebugEnable, "[TH0/2]%d %d\n", a_sAFConfig.AF_TH[0], a_sAFConfig.AF_TH[1]);
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_TH, 0x01000000+(Boundary(0, a_sAFConfig.AF_TH[1], 255)<<16)+ Boundary(0, a_sAFConfig.AF_TH[0], 255));
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_TH_E, Boundary(0, a_sAFConfig.AF_TH[0], 255));
        m_i4AF_TH[0] = a_sAFConfig.AF_TH[0];
        m_i4AF_TH[2] = a_sAFConfig.AF_TH[1];

        if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_SET_MODULE_CFG_DONE, handle, MNULL, MNULL))
            MY_ERR("EPIPECmd_SET_MODULE_CFG_DONE setAFWinTH fail");
    }
    if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_RELEASE_MODULE_HANDLE, handle, (MUINTPTR)(&("AFMgr::setAFWinTH")), MNULL))
        MY_ERR("EPIPECmd_SET_MODULE_CFG_DONE setAFWinTH fail");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::setGMR(AF_CONFIG_T a_sAFConfig)
{
    MUINTPTR handle, i4_SGG[3], i4_SGG_setting1;
    i4_SGG[0] = Boundary(0, a_sAFConfig.i4SGG_GMR1, 255);
    i4_SGG[1] = Boundary(0, a_sAFConfig.i4SGG_GMR2, 255);
    i4_SGG[2] = Boundary(0, a_sAFConfig.i4SGG_GMR3, 255);

    i4_SGG_setting1 = i4_SGG[0] + (i4_SGG[1]<<8)+ (i4_SGG[2]<<16);
    if (MFALSE ==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_GET_MODULE_HANDLE,NSImageio_FrmB::NSIspio_FrmB::EModule_SGG1,
        (MUINTPTR)&handle, (MUINTPTR)(&("AFMgr::setGMR()"))))
    {
        MY_ERR("EPIPECmd_GET_MODULE_HANDLE setGMR fail");
    }
    else
    {
        IOPIPE_SET_MODUL_REG(handle, CAM_SGG_GMR, i4_SGG_setting1);

        if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_SET_MODULE_CFG_DONE, handle, MNULL, MNULL))
            MY_ERR("EPIPECmd_SET_MODULE_CFG_DONE setGMR fail");
    }
    if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_RELEASE_MODULE_HANDLE, handle, (MUINTPTR)(&("AFMgr::setGMR")), MNULL))
        MY_ERR("EPIPECmd_SET_MODULE_CFG_DONE setGMR fail");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::setAFConfig(AF_CONFIG_T a_sAFConfig)
{
    MY_LOG("[setAFConfig]\n");
    MUINTPTR handle;
    MINT32 AFContmp, pixelodd;

    if (!m_pIspReg)
    {
        MY_LOG("[setAFConfig] m_pIspReg NULL");
        return;
    }

    if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_SET_MODULE_SEL, EPipe_P1Sel_SGG_EN, 0,0))
        MY_ERR("EPipe_P1Sel_SGG_EN fail");

    if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_SET_MODULE_SEL, EPipe_P1Sel_SGG, 2,0))
        MY_ERR("EPipe_P1Sel_SGG_EN fail");

    if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_SET_MODULE_SEL, EPipe_P1Sel_SGG_HRZ, 0,0))
        MY_ERR("EPipe_P1Sel_SGG_EN fail");

    MINT32 i4SGG_Sel_En = ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_CTL_MUX_SEL, SGG_SEL_EN);
    MINT32 i4SGG_Sel = ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_CTL_MUX_SEL, SGG_SEL);                // 0 : bin, 1 : before bin, 2 : lsc, 3 : imgi
    MINT32 i4SGG_Hrz_Sel = ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_CTL_SRAM_MUX_CFG, SGG_HRZ_SEL);   // 1: SGG after HRZ
    MY_LOG("[SGG_Sel_En]%d [SGG_Sel]%d [SGG_Hrz_Sel]%d", i4SGG_Sel_En, i4SGG_Sel, i4SGG_Hrz_Sel);

    // AF
    MINT32 i4TG_Pix_Id_En = ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_CTL_PIX_ID, TG_PIX_ID_EN);
    MINT32 i4TG_Pix_Id = ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_CTL_PIX_ID, TG_PIX_ID);   // 0:BGGR, 1:GBRG, 2GRBG, 3RGGB
    MINT32 i4Pix_Id = ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_CTL_PIX_ID, PIX_ID);   // 0:BGGR, 1:GBRG, 2GRBG, 3RGGB
    MY_LOG("[TG_Pix_Id_En]%d [TG_Pix_Id]%d [Pix_Id]%d", i4TG_Pix_Id_En, i4TG_Pix_Id, i4Pix_Id);
    if (i4TG_Pix_Id_En == 1)    i4Pix_Id = i4TG_Pix_Id;
    pixelodd=0;
    if ((i4Pix_Id == 1) || (i4Pix_Id == 2)) pixelodd=1;


    AFContmp=(Boundary(0, /*AF_TARY*/ 0          , 5)<<8) //AF_TARY   value= 0~5, bit 8~10
            +(Boundary(0, /*AF_TARX*/ 0          , 5)<<4) //AF_TARX   value= 0~5, bit 4~6
            +(Boundary(0, /*AF_ODD*/pixelodd     , 1)<<3) //AF ODD      0 or 1,      bit 3
            +(Boundary(0, a_sAFConfig.AF_ZIGZAG, 1)<<2)    //AF ZZ      0 or 1,     bit 2
            + Boundary(0, a_sAFConfig.AF_DECI_1, 2);       //AF DECI,     value= 0~2, bit 0~1


    if (MFALSE ==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_GET_MODULE_HANDLE,NSImageio_FrmB::NSIspio_FrmB::EModule_AF,
        (MUINTPTR)&handle, (MUINTPTR)(&("AFMgr::setAFConfig()"))))
    {
        MY_ERR("EPIPECmd_GET_MODULE_HANDLE setAFConfig fail");
    }
    else
    {
        IOPIPE_SET_MODUL_REG(handle,  CAM_AF_CON,        AFContmp);
        IOPIPE_SET_MODUL_REG(handle,  CAM_AF_FILT1_P14, (a_sAFConfig.AF_FILT1[3]<<24) + (a_sAFConfig.AF_FILT1[2]<<16) + (a_sAFConfig.AF_FILT1[1]<<8) + a_sAFConfig.AF_FILT1[0]);
        IOPIPE_SET_MODUL_REG(handle,  CAM_AF_FILT1_P58, (a_sAFConfig.AF_FILT1[7]<<24) + (a_sAFConfig.AF_FILT1[6]<<16) + (a_sAFConfig.AF_FILT1[5]<<8) + a_sAFConfig.AF_FILT1[4]);
        IOPIPE_SET_MODUL_REG(handle,  CAM_AF_FILT1_P912, (a_sAFConfig.AF_FILT1[11]<<24) + (a_sAFConfig.AF_FILT1[10]<<16) + (a_sAFConfig.AF_FILT1[9]<<8) + a_sAFConfig.AF_FILT1[8]);
        IOPIPE_SET_MODUL_REG(handle,  CAM_AF_VFILT_X01,(((a_sAFConfig.AF_FILT2[1]&0x7FF)<<16) + (a_sAFConfig.AF_FILT2[0]&0x7FF)));
        IOPIPE_SET_MODUL_REG(handle,  CAM_AF_VFILT_X23,(((a_sAFConfig.AF_FILT2[3]&0x7FF)<<16) + (a_sAFConfig.AF_FILT2[2]&0x7FF)));

    }
    if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_RELEASE_MODULE_HANDLE, handle, (MUINTPTR)(&("AFMgr::setAFConfig")), MNULL))
        MY_ERR("EPIPECmd_SET_MODULE_CFG_DONE setAFConfig fail");


    if (MFALSE ==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_GET_MODULE_HANDLE,NSImageio_FrmB::NSIspio_FrmB::EModule_SGG1,
        (MUINTPTR)&handle, (MUINTPTR)(&("AFMgr::setAFConfig()"))))
    {
        MY_ERR("EPIPECmd_GET_MODULE_HANDLE setAFConfig fail");
    }
    else
    {
        IOPIPE_SET_MODUL_REG(handle,  CAM_SGG_PGN, Boundary(0, a_sAFConfig.i4SGG_GAIN, 2047));
    }
    if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_RELEASE_MODULE_HANDLE, handle, (MUINTPTR)(&("AFMgr::setAFConfig")), MNULL))
        MY_ERR("EPIPECmd_SET_MODULE_CFG_DONE setAFConfig fail");


}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::printAFConfigLog0()
{
    if (!m_pIspReg)
    {
        MY_LOG("[printAFConfigLog0] m_pIspReg NULL");
        return;
    }

    #ifdef ISPREADREG
    MINT32 i4EN = (ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_CTL_EN1, SGG_EN)<<7)+
                  (ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_CTL_EN1, AF_EN)<<6)+
                  (ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_CTL_EN1, FLK_EN)<<5)+
                  (ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_CTL_EN2, EIS_EN)<<4)+
                  (ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_CTL_EN1, HRZ_EN)<<3)+
                  (ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_CTL_DMA_EN, ESFKO_EN)<<2);
    // monitor EIS / Flicker En
    MY_LOG_IF(m_bDebugEnable, "DoAF[sw]%x [XY]%d %d [Sz]%d %d  [G]%d %d %d [Hz]%d [WE|SE]%d %d %d %d [AFMode]%d [AE] %d, ", i4EN,
                    ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_WINX01, AF_WINX0),
                    ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_WINY01, AF_WINY0),
                    ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_SIZE, AF_XSIZE),
                    ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_SIZE, AF_YSIZE),
                    ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_SGG_GMR, SGG_GMR1),
                    ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_SGG_GMR, SGG_GMR2),
                    ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_SGG_GMR, SGG_GMR3),
                    ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_IN_SIZE, AF_IN_HSIZE),
                    ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_WIN_E, AF_WINXE),
                    ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_WIN_E, AF_WINYE),
                    ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_SIZE_E, AF_SIZE_XE),
                    ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_SIZE_E, AF_SIZE_YE),
                    m_eLIB3A_AFMode,
                    m_sAFInput.i4IsAEStable);
    #endif

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::printAFConfigLog1()
{
    if (!m_pIspReg)
    {
        MY_LOG("[printAFConfigLog1] m_pIspReg NULL");
        return;
    }

    #ifdef ISPREADREG
    MY_LOG_IF(m_bDebugEnable, "[X1-5] %d %d %d %d %d [Y1-5] %d %d %d %d %d ",
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_WINX01, AF_WINX1),
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_WINX23, AF_WINX2),
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_WINX23, AF_WINX3),
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_WINX45, AF_WINX4),
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_WINX45, AF_WINX5),
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_WINY01, AF_WINY1),
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_WINY23, AF_WINY2),
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_WINY23, AF_WINY3),
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_WINY45, AF_WINY4),
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_WINY45, AF_WINY5));

    MY_LOG_IF(m_bDebugEnable, "[THE]%d  [SGG_GN]%d [Deci]%d [Zig]%d [Odd]%d ",
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_TH_E, AF_TH0EX),
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_SGG_PGN, SGG_GAIN),
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_CON, AF_DECI_1),
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_CON, AF_ZIGZAG),
                                                         ISP_READ_BITS_NOPROTECT(m_pIspReg , CAM_AF_CON, AF_ODD));
    #endif


}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AF_FULL_STAT_T AfMgr::TransToFullStat(MVOID *pAFStatBuf)
{
    AF_HW_STAT_T *pAFStat = reinterpret_cast<AF_HW_STAT_T *>(pAFStatBuf);
    AF_FULL_STAT_T sAFStat;
    for (MINT32 i=0; i<(MAX_AF_HW_WIN-1); i++)
    {
        if (m_sAFOutput.i4IsMonitorFV == TRUE)
        {
            sAFStat.i8StatH[i] = ((((MINT64)pAFStat->sStat[i].u4StatV>>28)&0xF)<<32) + (MINT64)pAFStat->sStat[i].u4Stat24;
            sAFStat.i8StatV[i] = 0;
        }
        else
        {
            sAFStat.i8StatH[i] = 0;
            sAFStat.i8StatV[i] = 0;
        }
    }
    return sAFStat;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AF_STAT_T AfMgr::Trans4WintoOneStat(MVOID *pAFStatBuf)
{
    AF_STAT_T sAFStat;
    MINT64 i8Stat24 = 0;
    MINT64 i8StatV = 0;
    MINT32 i4posx = 2;
    MINT32 i4posy = 2;

    AF_HW_STAT_T *pAFStat = reinterpret_cast<AF_HW_STAT_T *>(pAFStatBuf);

    sAFStat.i8Stat24 = 0;
    sAFStat.i8StatV  = 0;

    if(m_tcaf_mode && !m_sAFOutput.i4FDDetect)    {
        i4posx = m_sAFOutput.sAFArea.sRect[0].i4X -m_sAFInput.sEZoom.i4X+ (m_sAFOutput.sAFArea.sRect[0].i4W>>1);
        i4posy = m_sAFOutput.sAFArea.sRect[0].i4Y -m_sAFInput.sEZoom.i4Y+ (m_sAFOutput.sAFArea.sRect[0].i4H>>1);
        if (i4posx > m_sAFOutput.sAFArea.sRect[0].i4W/12+FL_WIN_POS_MIN) i4posx-= (m_sAFOutput.sAFArea.sRect[0].i4W/12+FL_WIN_POS_MIN);
        else i4posx=0;
        if (i4posy > m_sAFOutput.sAFArea.sRect[0].i4H/12+FL_WIN_POS_MIN) i4posy-= (m_sAFOutput.sAFArea.sRect[0].i4H/12+FL_WIN_POS_MIN);
        else i4posy=0;
        i4posx = i4posx/ (m_sAFInput.sEZoom.i4W/6);
        i4posy = i4posy/ (m_sAFInput.sEZoom.i4H/6);
        i4posx = Boundary(0,i4posx,4);
        i4posy = Boundary(0,i4posy,4);
    }

    for (MINT32 i=0; i<(MAX_AF_HW_WIN-1); i++)
    {
        // --- H24 ---
        if (i==(i4posx + i4posy*6) || i==(i4posx+1+ i4posy*6) || i==(i4posx+6+ i4posy*6) || i==(i4posx+7+ i4posy*6))
        {
            i8Stat24 = ((((MINT64)pAFStat->sStat[i].u4StatV>>28)&0xF)<<32) + (MINT64)pAFStat->sStat[i].u4Stat24;
            sAFStat.i8Stat24 += i8Stat24;

            // --- V ---
            i8StatV = pAFStat->sStat[i].u4StatV&0x3FFFFFF;
            sAFStat.i8StatV += i8StatV;
        }
    }
    // --- Floating ---
    sAFStat.i8StatFL = (((MINT64)(pAFStat->sStat[36].u4StatV)&0xFF)<<32)+ (MINT64)pAFStat->sStat[36].u4Stat24;

    if ((m_sAFOutput.i4FDDetect) && (m_FDArea.i4Count))    {
        sAFStat.i8Stat24 = sAFStat.i8StatFL;
        sAFStat.i8StatV = sAFStat.i8StatFL;
    }

    return sAFStat;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AF_STAT_T AfMgr::TransAFtoOneStat(MVOID *pAFStatBuf)
{
    AF_STAT_T sAFStat;
    MINT64 i8Stat24 = 0;
    MINT64 i8StatV = 0;

    AF_HW_STAT_T *pAFStat = reinterpret_cast<AF_HW_STAT_T *>(pAFStatBuf);

    sAFStat.i8Stat24 = 0;
    sAFStat.i8StatV  = 0;

    for (MINT32 i=0; i<(MAX_AF_HW_WIN-1); i++)
    {
        // --- H24 ---
        i8Stat24 = ((((MINT64)(pAFStat->sStat[i].u4StatV>>28))&0xF)<<32) + (MINT64)pAFStat->sStat[i].u4Stat24;
        sAFStat.i8Stat24 += i8Stat24;

        // --- V ---
        i8StatV = pAFStat->sStat[i].u4StatV&0x3FFFFFF;
        sAFStat.i8StatV += i8StatV;

    }

    // --- Floating ---
    sAFStat.i8StatFL = (((MINT64)(pAFStat->sStat[36].u4StatV)&0xFF)<<32)+ (MINT64)pAFStat->sStat[36].u4Stat24;

    if ((m_sAFOutput.i4FDDetect) && (m_FDArea.i4Count))   {
        sAFStat.i8Stat24 = sAFStat.i8StatFL;
        sAFStat.i8StatV = sAFStat.i8StatFL;
    }
    return sAFStat;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::Boundary(MINT32 a_i4Min, MINT32 a_i4Vlu, MINT32 a_i4Max)
{
    if (a_i4Max < a_i4Min)  {a_i4Max = a_i4Min;}
    if (a_i4Vlu < a_i4Min)  {a_i4Vlu = a_i4Min;}
    if (a_i4Vlu > a_i4Max)  {a_i4Vlu = a_i4Max;}
    return a_i4Vlu;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::enableAF(MINT32 a_i4En)
{
    MY_LOG("[enableAF]%d\n", a_i4En);
    m_i4EnableAF = a_i4En;
    return S_AF_OK;
}
MBOOL AfMgr::isAFavailable()
{
    return m_i4EnableAF;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::isFocusFinish()
{
    MINT32 ret = 1;
    if( (m_eAFState == NS3A::E_AF_PASSIVE_SCAN) ||
        (m_eAFState == NS3A::E_AF_ACTIVE_SCAN) )
    {
        ret = 0;
    }
    return ret; //m_sAFOutput.i4IsAFDone;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::isFocused()
{
    return m_sAFOutput.i4IsFocused;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::getDebugInfo(AF_DEBUG_INFO_T &rAFDebugInfo)
{
    if (m_pIAfAlgo)
        return m_pIAfAlgo->getDebugInfo(rAFDebugInfo);
    else
   {
        MY_LOG("Null m_pIAfAlgo\n");
        return E_AF_NULL_POINTER;
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::getLensInfo(LENS_INFO_T &a_rLensInfo)
{
    MINT32 err = S_AF_OK;
    mcuMotorInfo rMotorInfo;

    if (m_pMcuDrv)
    {
        err = m_pMcuDrv->getMCUInfo(&rMotorInfo,m_i4CurrSensorDev);
        a_rLensInfo.bIsMotorMoving = rMotorInfo.bIsMotorMoving;
        a_rLensInfo.bIsMotorOpen   = rMotorInfo.bIsMotorOpen;
        a_rLensInfo.i4CurrentPos   = (MINT32)rMotorInfo.u4CurrentPosition;
        a_rLensInfo.i4MacroPos     = (MINT32)rMotorInfo.u4MacroPosition;
        a_rLensInfo.i4InfPos       = (MINT32)rMotorInfo.u4InfPosition;
        a_rLensInfo.bIsSupportSR   = rMotorInfo.bIsSupportSR;
    }
    return err;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AfMgr::setCallbacks(I3ACallBack* cb)
{
    m_pAFCallBack = cb;
    return TRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::SingleAF_CallbackNotify()
{
    m_pAFCallBack->doNotifyCb(I3ACallBack::eID_NOTIFY_AF_FOCUSED, m_sAFOutput.i4IsFocused, 0, 0);
    //m_pAFCallBack->doDataCb(I3ACallBack::eID_DATA_AF_FOCUSED, &m_sAFOutput.sAFArea, sizeof(m_sAFOutput.sAFArea));
    m_i4AutoFocus = FALSE;
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setBestShotConfig()
{
    MY_LOG("[setBestShotConfig]");
    MUINTPTR handle;
    setAF_IN_HSIZE();
    MINT32 i4WE = Boundary(FL_WIN_SIZE_MIN, (((m_i4AF_in_Hsize>>2)*3)>>1)<<1, FL_WIN_SIZE_MAX);
    MINT32 i4HE = Boundary(FL_WIN_SIZE_MIN, (((m_i4AF_in_Vsize>>2)*3)>>1)<<1, FL_WIN_SIZE_MAX);
    MINT32 i4XE = Boundary(FL_WIN_POS_MIN, (m_i4AF_in_Hsize-i4WE)>>1, m_i4AF_in_Hsize - i4WE - FL_WIN_POS_MIN);
    MINT32 i4YE = Boundary(FL_WIN_POS_MIN, (m_i4AF_in_Vsize-i4WE)>>1, m_i4AF_in_Vsize - i4HE - FL_WIN_POS_MIN);

    if (MFALSE ==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_GET_MODULE_HANDLE,NSImageio_FrmB::NSIspio_FrmB::EModule_AF,
        (MUINTPTR)&handle, (MUINTPTR)(&("AFMgr::setBestShotConfig()"))))
    {
        MY_ERR("EPIPECmd_GET_MODULE_HANDLE setBestShotConfig fail");
    }
    else
    {
    // setAFFloatingWinConfig
    // 13 bits (8192x8192) - double buffer, "must even position"
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_WIN_E,  (i4XE & 0x1FFE) + ((i4YE&0x1FFE)<<16) );
    // 12 bits (4096x4096) - double buffer
        IOPIPE_SET_MODUL_REG(handle, CAM_AF_SIZE_E, (Boundary(FL_WIN_SIZE_MIN, i4WE, FL_WIN_SIZE_MAX) & 0xFFE)
            + ((Boundary(FL_WIN_SIZE_MIN, i4HE, FL_WIN_SIZE_MAX)&0xFFE)<<16));
    }
    if (MFALSE==sendAFNormalPipe(NSImageio_FrmB::NSIspio_FrmB::EPIPECmd_RELEASE_MODULE_HANDLE, handle, (MUINTPTR)(&("AFMgr::setAFConfig")), MNULL))
        MY_ERR("EPIPECmd_SET_MODULE_CFG_DONE setBestShotConfig fail");
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::calBestShotValue(MVOID *pAFStatBuf)
{
    AF_HW_STAT_T *pAFStat = reinterpret_cast<AF_HW_STAT_T *>(pAFStatBuf);
    m_i8BSSVlu = (((MINT64)(pAFStat->sStat[36].u4StatV)&0x3F)<<32) + (MINT64)pAFStat->sStat[36].u4Stat24;
    MY_LOG("[calBestShotValue] %lld", m_i8BSSVlu);

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT64 AfMgr::getBestShotValue()
{
    return m_i8BSSVlu;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::readOTP()
{
    MUINT32 result=0;
    CAM_CAL_DATA_STRUCT GetCamCalData;
    CamCalDrvBase *pCamCalDrvObj = CamCalDrvBase::createInstance();
    MINT32 i4SensorDevID;

    CAMERA_CAM_CAL_TYPE_ENUM enCamCalEnum = CAMERA_CAM_CAL_DATA_3A_GAIN;

    switch (m_i4CurrSensorDev)
    {
    case ESensorDev_Main:
        i4SensorDevID = (MINT32)NSCam::SENSOR_DEV_MAIN;
        break;
    case ESensorDev_Sub:
        i4SensorDevID = (MINT32)NSCam::SENSOR_DEV_SUB;
        break;
    case ESensorDev_MainSecond:
        i4SensorDevID = (MINT32)NSCam::SENSOR_DEV_MAIN_2;
        return S_AWB_OK;
    case ESensorDev_Main3D:
        i4SensorDevID = (MINT32)NSCam::SENSOR_DEV_MAIN_3D;
        return S_AWB_OK;
    default:
        i4SensorDevID = (MINT32)NSCam::SENSOR_DEV_NONE;
        return S_AWB_OK;
    }

    result= pCamCalDrvObj->GetCamCalCalData(i4SensorDevID, enCamCalEnum, (void *)&GetCamCalData);
    MY_LOG("(0x%8x)=pCamCalDrvObj->GetCamCalCalData", result);

    if (result&CamCalReturnErr[enCamCalEnum])
    {
        MY_LOG("err (%s)", CamCalErrString[enCamCalEnum]);
        return E_AF_NOSUPPORT;
    }

    MY_LOG("OTP data [S2aBitEn]%d [S2aAfBitflagEn]%d [S2aAf0]%d [S2aAf1]%d", GetCamCalData.Single2A.S2aBitEn
    ,GetCamCalData.Single2A.S2aAfBitflagEn,  GetCamCalData.Single2A.S2aAf[0],  GetCamCalData.Single2A.S2aAf[1]);

    MINT32 i4InfPos, i4MacroPos;
    if (GetCamCalData.Single2A.S2aBitEn & 0x1)
    {
        i4InfPos = GetCamCalData.Single2A.S2aAf[0];
        if (GetCamCalData.Single2A.S2aBitEn & 0x2)
        {
            i4MacroPos = GetCamCalData.Single2A.S2aAf[1];
            if (i4MacroPos < i4InfPos)
            {
                MY_LOG("OTP abnormal return [Inf]%d [Macro]%d", i4InfPos, i4MacroPos);
                return S_AF_OK;
            }
        }
        else    i4MacroPos = 0;
        if (  (i4MacroPos >1023) || (i4InfPos >1023)
            ||(i4MacroPos <0   ) || (i4InfPos <0   ) )
        {
            MY_LOG("OTP abnormal return [Inf]%d [Macro]%d", i4InfPos, i4MacroPos);
            return S_AF_OK;
        }
        MY_LOG("OTP [Inf]%d [Macro]%d", i4InfPos, i4MacroPos);
        if (m_pIAfAlgo)
            m_pIAfAlgo->updateAFtableBoundary(i4InfPos, i4MacroPos);
        }
    return S_AF_OK;
}

// AF v1.2
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/* This update function should be called before m_pIAfAlgo->setAFParam */
MRESULT AfMgr::updateSceneChangeParams()
{
#if 0
    UINT32 Level = m_NVRAM_LENS.rAFNVRAM.i4Coef[7];
    if(0 == Level)
    {   // disable new part and use org only
        m_NVRAM_LENS.rAFNVRAM.i4Coef[8]=0;
        m_NVRAM_LENS.rAFNVRAM.i4Coef[9]=0;
        m_NVRAM_LENS.rAFNVRAM.i4Coef[10]=0;
        m_NVRAM_LENS.rAFNVRAM.i4Coef[11]=0;
        m_NVRAM_LENS.rAFNVRAM.i4Coef[12]=0;
        m_NVRAM_LENS.rAFNVRAM.i4Coef[13]=0;
        m_NVRAM_LENS.rAFNVRAM.i4Coef[14]=0;
        m_NVRAM_LENS.rAFNVRAM.i4Coef[15]=0;
    }
    else if((1<=Level)&&(Level<=3))
    {   // level 1~3
        UINT32 levelIdx = Level-1;

        //-- org part --
        // normal
        m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4FV_CHANGE_THRES += SCENE_FV_CHG_THR_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4FV_CHANGE_CNT   += SCENE_FV_CHG_CNT_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4FV_STABLE_THRES += SCENE_FV_STB_THR_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4FV_STABLE_CNT   += SCENE_FV_STB_CNT_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4FV_STABLE_NUM   += SCENE_FV_STB_CNT_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4GS_CHANGE_THRES += SCENE_GS_CHG_THR_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4GS_CHANGE_CNT   += SCENE_GS_CHG_CNT_LEVEL_DIFF[levelIdx];
        // zsd
        m_NVRAM_LENS.rAFNVRAM.sZSD_AF_Coef.i4FV_CHANGE_THRES += SCENE_FV_CHG_THR_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sZSD_AF_Coef.i4FV_CHANGE_CNT   += SCENE_FV_CHG_CNT_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sZSD_AF_Coef.i4FV_STABLE_THRES += SCENE_FV_STB_THR_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sZSD_AF_Coef.i4FV_STABLE_CNT   += SCENE_FV_STB_CNT_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sZSD_AF_Coef.i4FV_STABLE_NUM   += SCENE_FV_STB_CNT_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sZSD_AF_Coef.i4GS_CHANGE_THRES += SCENE_GS_CHG_THR_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sZSD_AF_Coef.i4GS_CHANGE_CNT   += SCENE_GS_CHG_CNT_LEVEL_DIFF[levelIdx];
        // video
        m_NVRAM_LENS.rAFNVRAM.sVAFC_Coef.i4FV_CHANGE_THRES += SCENE_FV_CHG_THR_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sVAFC_Coef.i4FV_CHANGE_CNT   += SCENE_FV_CHG_CNT_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sVAFC_Coef.i4FV_STABLE_THRES += SCENE_FV_STB_THR_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sVAFC_Coef.i4FV_STABLE_CNT   += SCENE_FV_STB_CNT_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sVAFC_Coef.i4FV_STABLE_NUM   += SCENE_FV_STB_CNT_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sVAFC_Coef.i4GS_CHANGE_THRES += SCENE_GS_CHG_THR_LEVEL_DIFF[levelIdx];
        m_NVRAM_LENS.rAFNVRAM.sVAFC_Coef.i4GS_CHANGE_CNT   += SCENE_GS_CHG_CNT_LEVEL_DIFF[levelIdx];

        //-- new part --
        // encode param to i4Coef[8]/[9] format: chgT|chgN|stbT|stbN = ooxxooxx
        UINT32 GS_CHG_PARAM          = 1000000*(m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4GS_CHANGE_THRES) \
                                       + 10000*(m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4GS_CHANGE_CNT%100);
        UINT32 AEBLOCK_CHG_PARAM     = 1000000*(m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4GS_CHANGE_THRES+SCENE_AEBLOCK_CHG_THR_DIFF) \
                                       + 10000*((m_NVRAM_LENS.rAFNVRAM.sAF_Coef.i4GS_CHANGE_CNT+SCENE_AEBLOCK_CHG_CNT_DIFF)%100);
        UINT32 GS_VID_CHG_PARAM      = 1000000*(m_NVRAM_LENS.rAFNVRAM.sVAFC_Coef.i4GS_CHANGE_THRES) \
                                       + 10000*(m_NVRAM_LENS.rAFNVRAM.sVAFC_Coef.i4GS_CHANGE_CNT%100);
        UINT32 AEBLOCK_VID_CHG_PARAM = 1000000*(m_NVRAM_LENS.rAFNVRAM.sVAFC_Coef.i4GS_CHANGE_THRES+SCENE_AEBLOCK_CHG_THR_DIFF) \
                                       + 10000*((m_NVRAM_LENS.rAFNVRAM.sVAFC_Coef.i4GS_CHANGE_CNT+SCENE_AEBLOCK_CHG_CNT_DIFF)%100);
        // normal
        if(0 == m_NVRAM_LENS.rAFNVRAM.i4Coef[8])
            m_NVRAM_LENS.rAFNVRAM.i4Coef[8]   = GS_CHG_PARAM + SCENE_GS_LEVEL[levelIdx];
        if(0 == m_NVRAM_LENS.rAFNVRAM.i4Coef[9])
            m_NVRAM_LENS.rAFNVRAM.i4Coef[9]   = AEBLOCK_CHG_PARAM + SCENE_AEBLOCK_LEVEL[levelIdx];
        if(0 == m_NVRAM_LENS.rAFNVRAM.i4Coef[10])
            m_NVRAM_LENS.rAFNVRAM.i4Coef[10]  = SCENE_GYRO_LEVEL[levelIdx];
        if(0 == m_NVRAM_LENS.rAFNVRAM.i4Coef[11])
            m_NVRAM_LENS.rAFNVRAM.i4Coef[11]  = SCENE_ACCE_LEVEL[levelIdx];
        // video
        if(0 == m_NVRAM_LENS.rAFNVRAM.i4Coef[12])
            m_NVRAM_LENS.rAFNVRAM.i4Coef[12]   = GS_VID_CHG_PARAM + SCENE_GS_VID_LEVEL[levelIdx];
        if(0 == m_NVRAM_LENS.rAFNVRAM.i4Coef[13])
            m_NVRAM_LENS.rAFNVRAM.i4Coef[13]   = AEBLOCK_VID_CHG_PARAM + SCENE_AEBLOCK_VID_LEVEL[levelIdx];
        if(0 == m_NVRAM_LENS.rAFNVRAM.i4Coef[14])
            m_NVRAM_LENS.rAFNVRAM.i4Coef[14]  = SCENE_GYRO_VID_LEVEL[levelIdx];
        if(0 == m_NVRAM_LENS.rAFNVRAM.i4Coef[15])
            m_NVRAM_LENS.rAFNVRAM.i4Coef[15]  = SCENE_ACCE_VID_LEVEL[levelIdx];
    }
#endif
    return S_AF_OK;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::autoFocus()
{
    m_i4AutoFocusCmd = TRUE;

    MY_LOG("autofocus %d", m_eLIB3A_AFMode);
    if ((m_eLIB3A_AFMode != LIB3A_AF_MODE_AFC) && (m_eLIB3A_AFMode != LIB3A_AF_MODE_AFC_VIDEO))
    {
        m_i4AutoFocus = TRUE;
        m_isTriggerAFS = TRUE;
    }
    else
    {
        if (m_pIAfAlgo) {
            m_pIAfAlgo->triggerAF();
            m_i4LockAFInContiMode = 1;
        }
        m_i4AutoFocuscb = TRUE;
    }

    MY_LOG("%s AFCtrlMode(%d), SearchDoneFlg(%d) AutoFocusCmd(%d)",
           __FUNCTION__,
           m_eLIB3A_AFMode,
           m_i4SearchDone,
           m_i4AutoFocusCmd);

    m_sAFOutput.i4IsAFDone = MFALSE;
}
MVOID AfMgr::DoCallback()
{
    if(m_pAFCallBack!=NULL && m_i4AutoFocuscb==TRUE)
    {
        m_pAFCallBack->doNotifyCb(I3ACallBack::eID_NOTIFY_AF_MOVING, !m_sAFOutput.i4IsAFDone, 0, 0);
        m_pAFCallBack->doNotifyCb(I3ACallBack::eID_NOTIFY_AF_FOCUSED, m_sAFOutput.i4IsFocused, 0, 0);
        MY_LOG("autofocus callback in conti mode %d", m_eLIB3A_AFMode);
        m_i4AutoFocuscb = FALSE;
    }
    if (m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC || m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC_VIDEO)
    {
        if (m_sAFOutput.sAFArea.sRect[0].i4Info != m_i4AFPreStatus)
        {
            if (m_pAFCallBack)
            {
                MY_LOG("Callback notify [pre]%d [now]%d", m_i4AFPreStatus, m_sAFOutput.sAFArea.sRect[0].i4Info);
                if (m_sAFOutput.sAFArea.sRect[0].i4Info < 3)
                {
                    m_pAFCallBack->doNotifyCb(I3ACallBack::eID_NOTIFY_AF_MOVING, !m_sAFOutput.i4IsAFDone, 0, 0);
                }
                //m_pAFCallBack->doNotifyCb(I3ACallBack::eID_NOTIFY_AF_FOCUSED, m_sAFOutput.i4IsFocused, 0, 0);
                m_pAFCallBack->doDataCb(I3ACallBack::eID_DATA_AF_FOCUSED, &m_sAFOutput.sAFArea, sizeof(m_sAFOutput.sAFArea));
            }
            m_i4AFPreStatus = m_sAFOutput.sAFArea.sRect[0].i4Info;
        }
    }
    else
    {
        m_i4AFPreStatus = AF_MARK_NONE;
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::cancelAutoFocus()
{
    m_i4CancelAutoFocusCmd = TRUE;

    if ((m_eLIB3A_AFMode != LIB3A_AF_MODE_AFC) && (m_eLIB3A_AFMode != LIB3A_AF_MODE_AFC_VIDEO))
    {
        m_i4AutoFocus = FALSE;
    }

    if(m_pIAfAlgo)
    {
        m_pIAfAlgo->cancelAF();
        m_i4LockAFInContiMode = 0;
    }

    MY_LOG("%s AFCtrlMode(%d), SearchDoneFlg(%d) CancelAutoFocusCmd(%d)",
           __FUNCTION__,
           m_eLIB3A_AFMode,
           m_i4SearchDone,
           m_i4CancelAutoFocusCmd);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::TimeOutHandle()
{
    if (m_i4AutoFocus == TRUE)
    {
        MY_LOG("timeout callback");
        SingleAF_CallbackNotify();
        m_i4AutoFocus = FALSE;
    }
}
MVOID AfMgr::setAndroidServiceState(MBOOL a_state)
{
    m_AndroidServiceState=a_state;
}
//Camera 3.0
/*MVOID AfMgr::setAFRegions(CameraFocusArea_T a_sAFArea)
{
    if (a_sAFArea.u4Count >= AF_WIN_NUM_SPOT)
        a_sAFArea.u4Count = AF_WIN_NUM_SPOT;

        m_sAFInput.sAFArea.i4Count = a_sAFArea.u4Count;

    if (a_sAFArea.u4Count != 0)
    {
        for (MUINT32 i=0; i< a_sAFArea.u4Count; i++)
        {
            m_sAFInput.sAFArea.sRect[i].i4W = a_sAFArea.rAreas[i].i4Right - a_sAFArea.rAreas[i].i4Left;
            m_sAFInput.sAFArea.sRect[i].i4H = a_sAFArea.rAreas[i].i4Bottom - a_sAFArea.rAreas[i].i4Top;
            m_sAFInput.sAFArea.sRect[i].i4X = a_sAFArea.rAreas[i].i4Left;
            m_sAFInput.sAFArea.sRect[i].i4Y = a_sAFArea.rAreas[i].i4Top;
            m_sAFInput.sAFArea.sRect[i].i4Info   = a_sAFArea.rAreas[i].i4Weight;
        }
    }
    MY_LOG("[setAFRegions][Cnt]%d [L]%d [R]%d [U]%d [B]%d to [Cnt]%d [W]%d [H]%d [X]%d [Y]%d\n", a_sAFArea.u4Count, a_sAFArea.rAreas[0].i4Left, a_sAFArea.rAreas[0].i4Right, a_sAFArea.rAreas[0].i4Top, a_sAFArea.rAreas[0].i4Bottom, m_sAFInput.sAFArea.i4Count, m_sAFInput.sAFArea.sRect[0].i4W, m_sAFInput.sAFArea.sRect[0].i4H, m_sAFInput.sAFArea.sRect[0].i4X, m_sAFInput.sAFArea.sRect[0].i4Y);
}
MVOID AfMgr::getAFRegions(CameraFocusArea_T *a_sAFArea)
{
 *a_sAFArea = m_CameraFocusArea;
}*/



MVOID AfMgr::setAperture(MFLOAT lens_aperture)
{

}
MFLOAT AfMgr::getAperture()
{
    MFLOAT lens_aperture=0;
    if(m_getAFmeta==0)    return 0;

    for(MUINT8 ii=0; ii< m_AFMeta.entryFor(MTK_LENS_INFO_AVAILABLE_APERTURES).count();ii++)
    {
        lens_aperture= m_AFMeta.entryFor(MTK_LENS_INFO_AVAILABLE_APERTURES).itemAt(ii, Type2Type< MFLOAT >());
        MY_LOG("AFmeta APERTURES %d  %f",ii, lens_aperture);
    }
    return  lens_aperture;
}
MVOID AfMgr::setFilterDensity(MFLOAT lens_filterDensity){}
MFLOAT AfMgr::getFilterDensity()
{
    MFLOAT lens_filterDensity=0;
    if(m_getAFmeta==0)    return 0;
    for(MUINT8 ii=0; ii< m_AFMeta.entryFor(MTK_LENS_INFO_AVAILABLE_FILTER_DENSITIES).count();ii++)
    {
        lens_filterDensity= m_AFMeta.entryFor(MTK_LENS_INFO_AVAILABLE_FILTER_DENSITIES).itemAt(ii, Type2Type< MFLOAT >());
        MY_LOG("AFmeta FILTER_DENSITIES %d  %f",ii, lens_filterDensity);
    }
    return  lens_filterDensity;
}
MVOID AfMgr::setFocalLength(MFLOAT lens_focalLength){}
MFLOAT AfMgr::getFocalLength ()
{
    MFLOAT lens_focalLength =34.0;
    if(m_getAFmeta==0)    return 0;
    for(MUINT8 ii=0; ii< m_AFMeta.entryFor(MTK_LENS_INFO_AVAILABLE_FOCAL_LENGTHS).count();ii++)
    {
        lens_focalLength= m_AFMeta.entryFor(MTK_LENS_INFO_AVAILABLE_FOCAL_LENGTHS).itemAt(ii, Type2Type< MFLOAT >());
        MY_LOG("AFmeta APERTURES %d  %f",ii, lens_focalLength);
    }
    return  lens_focalLength;
}
MVOID AfMgr::setFocusDistance(MFLOAT lens_focusDistance)
{
    MINT32 fdistidx=0;
    MINT32 i4distmm;
    MINT32 i4tblLL;
    MINT32 i4ResultDac;

    //lens_focusDistance is in unit dipoter, means 1/distance,
    //if distance is 100 cm,  then the value is  1/(0.1M) =10,
    // 10 cm => dipoter 100
    // 1 cm => dipoter 1000
    MY_LOG("[setFocusDistance] in %f\n", lens_focusDistance);
    i4tblLL = m_sAFParam.i4TBLL;
    if(lens_focusDistance <= 0.0)
    {
        i4ResultDac=m_sAFParam.i4Dacv[i4tblLL-1];
    }
    else if(lens_focusDistance <1000.0)
    {
        i4distmm = (MINT32)(1000/lens_focusDistance);

        for(fdistidx=0; fdistidx< i4tblLL ;fdistidx++)
        {
             if( i4distmm <  m_sAFParam.i4Dist[fdistidx])
                 break;

        }
        if(fdistidx==0) i4ResultDac=m_sAFParam.i4Dacv[0];
        else
        {
            i4ResultDac=
            ( m_sAFParam.i4Dacv[fdistidx]   * (i4distmm - m_sAFParam.i4Dist[fdistidx-1])
            + m_sAFParam.i4Dacv[fdistidx-1] * (m_sAFParam.i4Dist[fdistidx] - i4distmm ))
            /(m_sAFParam.i4Dist[fdistidx] - m_sAFParam.i4Dist[fdistidx-1] );
        }
    }
    else
        i4ResultDac=m_sAFParam.i4Dacv[0];

    if (m_eLIB3A_AFMode == LIB3A_AF_MODE_OFF)
    {
        MY_LOG("[setFocusDistance]%d\n", i4ResultDac);
        m_sAFOutput.i4AFPos=i4ResultDac;
        if (m_pIAfAlgo)    m_pIAfAlgo->setMFPos(m_sAFOutput.i4AFPos);
        else            MY_LOG("Null m_pIAfAlgo\n");
    }
}
MFLOAT AfMgr::getFocusDistance   ()
{
    MINT32 fdacidx=0;
    MINT32 i4tblLL;
    MINT32 i4ResultDist  = 100;
    MFLOAT retDist=0.0;
    if (m_i4EnableAF == 0)
    {
        MY_LOG("no focus lens");
        return 0.0;
    }
     i4tblLL = m_sAFParam.i4TBLL;
    for(fdacidx=0; fdacidx< i4tblLL ;fdacidx++)
    {
        if( m_sAFOutput.i4AFPos >  m_sAFParam.i4Dacv[fdacidx])
            break;
    }
    if(fdacidx==0) i4ResultDist = m_sAFParam.i4Dist[0];
    else
    {
        i4ResultDist=
        ( m_sAFParam.i4Dist[fdacidx]   * (m_sAFParam.i4Dacv[fdacidx-1] - m_sAFOutput.i4AFPos)
        + m_sAFParam.i4Dist[fdacidx-1] * (m_sAFOutput.i4AFPos - m_sAFParam.i4Dacv[fdacidx]))
        /(m_sAFParam.i4Dacv[fdacidx-1] - m_sAFParam.i4Dacv[fdacidx] );
    }
    if(i4ResultDist<=0) i4ResultDist= m_sAFParam.i4Dist[i4tblLL-1];
    retDist = 1000.0/ ((MFLOAT)i4ResultDist);

    //MY_LOG("[getFocusDistance] %f", retDist);
    return  retDist;
}
MVOID AfMgr::setOpticalStabilizationMode (MINT32 ois_OnOff)
{
    MUINT8 oismode=0;
    if(m_getAFmeta==0)    return;

    for(MUINT8 ii=0; ii< m_AFMeta.entryFor(MTK_LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION).count();ii++)
    {
        oismode= m_AFMeta.entryFor(MTK_LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION).itemAt(ii, Type2Type< MUINT8 >());
        MY_LOG("AFmeta OPTICAL_STABILIZATION %d  %d",ii, oismode);
        if(oismode==1)
        {
            //set_ois_drv_on(ois_OnOff);
            break;
        }
    }
}
MINT32 AfMgr::getOpticalStabilizationMode()
{
    MUINT8 oismode=0;
    if(m_getAFmeta==0)    return 0;

    for(MUINT8 ii=0; ii< m_AFMeta.entryFor(MTK_LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION).count();ii++)
    {
        oismode= m_AFMeta.entryFor(MTK_LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION).itemAt(ii, Type2Type< MUINT8 >());
        MY_LOG("AFmeta OPTICAL_STABILIZATION %d  %d",ii, oismode);
    }
    return  oismode;/*OFF, ON */
}
MVOID AfMgr::getFocusRange (MFLOAT *vnear, MFLOAT *vfar)
{
    MINT32 fdacidx=0;
    MINT32 i4tblLL;
    MINT32 i4ResultDist  = 100;
    MINT32 i4ResultRange  = 100;
     MINT32 i4ResultNear  = 100;
    MINT32 i4ResultFar  = 100;
    MFLOAT retDist=0.0;
    if (m_i4EnableAF == 0)
    {
        MY_LOG("no focus lens");
        *vnear = 1/0.6;
        *vfar = 1/3.0;
        return;
    }
     i4tblLL = m_sAFParam.i4TBLL;
    for(fdacidx=0; fdacidx< i4tblLL ;fdacidx++)
    {
        if( m_sAFOutput.i4AFPos >  m_sAFParam.i4Dacv[fdacidx])
            break;
    }
    if(fdacidx==0) i4ResultDist = m_sAFParam.i4Dist[0];
    else
    {
        i4ResultDist=
        ( m_sAFParam.i4Dist[fdacidx]   * (m_sAFParam.i4Dacv[fdacidx-1] - m_sAFOutput.i4AFPos)
        + m_sAFParam.i4Dist[fdacidx-1] * (m_sAFOutput.i4AFPos - m_sAFParam.i4Dacv[fdacidx]))
        /(m_sAFParam.i4Dacv[fdacidx-1] - m_sAFParam.i4Dacv[fdacidx] );

        i4ResultRange=
        ( m_sAFParam.i4FocusRange[fdacidx]   * (m_sAFParam.i4Dacv[fdacidx-1] - m_sAFOutput.i4AFPos)
        + m_sAFParam.i4FocusRange[fdacidx-1] * (m_sAFOutput.i4AFPos - m_sAFParam.i4Dacv[fdacidx]))
        /(m_sAFParam.i4Dacv[fdacidx-1] - m_sAFParam.i4Dacv[fdacidx] );
    }
    if(i4ResultDist<=0)  i4ResultDist= m_sAFParam.i4Dist[i4tblLL-1];
    if(i4ResultRange<=0) i4ResultRange= m_sAFParam.i4Dist[i4tblLL-1];

    i4ResultNear = i4ResultDist - (i4ResultRange/2);
    i4ResultFar  = i4ResultDist + (i4ResultRange/2);

    *vnear = 1000.0/ ((MFLOAT)i4ResultNear);
    *vfar  = 1000.0/ ((MFLOAT)i4ResultFar);


    //MY_LOG("[getFocusRange] [%f, %f]", *vnear,*vfar);
}
MINT32 AfMgr::getLensState  ()
{

    MUINT8 lens_item_ivalue;

    if(m_prePosition!=m_sAFOutput.i4AFPos)
    {
        m_prePosition=m_sAFOutput.i4AFPos;
        return  1; /* MOVING */
    }
    else
        return  0;  /* STATIONARY */

    return  0;
}

MVOID* AfMgr::createSensorListenerThread(MVOID* arg)
{
    MY_LOG("[%s] +\n", __FUNCTION__);
    // thread auto detach after this function finish
    ::pthread_detach(pthread_self());

    ::prctl(PR_SET_NAME, "threadGGyroSensor", 0, 0, 0);
    //struct sched_param sched_p;
    //::sched_getparam(0, &sched_p);
    //sched_p.sched_priority = NICE_CAMERA_AF;  //  Note: "priority" is nice value.
    //::sched_setscheduler(0, SCHED_OTHER, &sched_p);
    //::setpriority(PRIO_PROCESS, 0, priority);

    AfMgr *_this = static_cast<AfMgr*>(arg);
    _this->EnableGGyro();

    MY_LOG("[%s] -\n", __FUNCTION__);
    ::pthread_exit((MVOID*)0);
}

MVOID AfMgr::EnableGGyro()
{
    if(m_bGGyroEn)
    {

    MY_LOG("[%s] m_bGGyroEn(%d) +\n", __FUNCTION__, m_bGGyroEn);
     // i4Coef[5] == 1:enable SensorListener, else: disable
    {
        Mutex::Autolock lock(gSensorCommonLock);

        if((m_NVRAM_LENS.rAFNVRAM.i4SensorEnable > 0) && (mpSensorManager == NULL) && (gListenEnAcce==MFALSE) && (gListenEnGyro==MFALSE))
        {
            //============ enableSensor ============
            mpSensorManager = ISensorManager::getService();

            if (mpSensorManager == NULL)
            {
                MY_ERR("get SensorManager FAIL!");
                goto create_fail_exit;
            }

            mpSensorManager->getDefaultSensor(SensorType::GYROSCOPE,
                [&](const SensorInfo& sensor, Result ret) {
                    ret == Result::OK ? mGyroSensorHandle = sensor.sensorHandle : mGyroSensorHandle = -1;
                });
            mpSensorManager->getDefaultSensor(SensorType::ACCELEROMETER,
                [&](const SensorInfo& sensor, Result ret) {
                    ret == Result::OK ? mAcceSensorHandle = sensor.sensorHandle : mAcceSensorHandle = -1;
                });

            if (mGyroSensorHandle == -1 || mAcceSensorHandle == -1)
            {
                MY_ERR("get DefaultSensor FAIL! %d %d", mGyroSensorHandle, mAcceSensorHandle);
                goto create_fail_exit;
            }
            //create SensorEventQueue and register callback
            mpAfSensorListener = new AfSensorListener();
            mpSensorManager->createEventQueue(mpAfSensorListener,
                [&](const sp<IEventQueue>& queue, Result ret) {
                    ret == Result::OK ? mpEventQueue = queue : mpEventQueue = NULL;
                });

            if (mpEventQueue == NULL)
            {
                MY_ERR("createEventQueue FAIL!");
                goto create_fail_exit;
            }
            //enable sensor
            if (mpEventQueue->enableSensor(mGyroSensorHandle, SENSOR_GYRO_POLLING_MS /* sample period */, 0 /* latency */) != Result::OK ||
                mpEventQueue->enableSensor(mAcceSensorHandle, SENSOR_ACCE_POLLING_MS /* sample period */, 0 /* latency */) != Result::OK)
            {
                MY_ERR("enable Sensor FAIL!");
                goto create_fail_exit;
            }
            else
            {
                gListenEnAcce = MTRUE;
                gListenEnGyro = MTRUE;
            }

            create_fail_exit:
                if( (gListenEnAcce!=MTRUE) || (gListenEnGyro!=MTRUE))
                {
                    MY_ERR("sensor fail : Acc(%d) Gyro(%d)", gListenEnAcce, gListenEnGyro);
                }
        }
    }
    MY_LOG("[%s] m_bGGyroEn(%d) -\n", __FUNCTION__, m_bGGyroEn);

    }
    else
    {

    MY_LOG("[%s] m_bGGyroEn(%d) +\n", __FUNCTION__, m_bGGyroEn);
    {
        Mutex::Autolock lock(gSensorCommonLock);

        if((m_NVRAM_LENS.rAFNVRAM.i4SensorEnable > 0) && (mpSensorManager != NULL))
        {
            if (mpSensorManager != NULL)
            {
                mpSensorManager = NULL;
            }
            if (mpEventQueue != NULL)
            {
                if(gListenEnGyro)
                {
                    MY_LOG("[%s] mpEventQueue->disableSensor(Gyro) + \n", __FUNCTION__);
                    ::android::hardware::Return<Result> _ret = mpEventQueue->disableSensor(mGyroSensorHandle);
                    if (!_ret.isOk())
                    {
                        MY_ERR("disableSensor fail");
                    }
                    gListenEnGyro = MFALSE;
                    MY_LOG("[%s] mpEventQueue->disableSensor(Gyro) - \n", __FUNCTION__);
                }

                if(gListenEnAcce)
                {
                    MY_LOG("[%s] mpEventQueue->disableSensor(Acce) + \n", __FUNCTION__);
                    ::android::hardware::Return<Result> _ret = mpEventQueue->disableSensor(mAcceSensorHandle);
                    if (!_ret.isOk())
                    {
                        MY_ERR("disableSensor fail");
                    }
                    gListenEnAcce = MFALSE;
                    MY_LOG("[%s] mpEventQueue->disableSensor(Acce) - \n", __FUNCTION__);
                }
                mpEventQueue = NULL;
            }
            if (mpAfSensorListener != NULL)
            {
                mpAfSensorListener = NULL;
            }
        }
    }
    MY_LOG("[%s] m_bGGyroEn(%d) -\n", __FUNCTION__, m_bGGyroEn);

    }
}