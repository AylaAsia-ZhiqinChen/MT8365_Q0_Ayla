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
/**
* @file aaa_hal.h
* @brief Declarations of 3A Hal Class (public inherited from Hal3AIf)
*/

#ifndef _AAA_HAL_YUV_H_
#define _AAA_HAL_YUV_H_

//------------Thread-------------
#include <pthread.h>
#include <semaphore.h>
//-------------------------------

//#include <mtkcam/drv/isp_drv.h>
#include <aaa_types.h>
#include <aaa_error_code.h>
#include <aaa_log.h>
#include <hal/aaa/aaa_hal_if.h>
#include <hal/aaa/aaa_hal_flowCtrl.h>
#include <hal/aaa/ResultBufMgr/ResultBufMgr.h>
//#include <mtkcam/iopipe/CamIO/IHalCamIO.h>
//#include <mtkcam/iopipe/CamIO/INormalPipe.h>
#include <IEventIrq.h>

#include <utils/threads.h>
#include <utils/List.h>

#include <utils/threads.h>
#include <utils/List.h>
//-------------------------------
/*temp mark out*///#include <Local.h>
#include <mtkcam/hal/IHalSensor.h>

#include "strobe_drv.h"
#include "kd_imgsensor_define.h"

using namespace android;

#define ISP_P1_UPDATE (1)
#define NOTIFY_3A_DONE (1)

using namespace NSCam;
using namespace android;
using namespace NS3Av3;

/**
 * @brief AREA_T_YUV structure
 */
typedef struct
{
    MINT32 i4Left;
    MINT32 i4Right;
    MINT32 i4Top;
    MINT32 i4Bottom;
    MINT32 i4Info;
} AREA_T_YUV;


namespace NS3Av3
{
class StateMgr;
//class AAA_Scheduler;
class AaaTimer;
//class IEventIrq;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


class Hal3AYuv : public Hal3AFlowCtrl
{

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:  //    Ctor/Dtor.
    Hal3AYuv();
    virtual ~Hal3AYuv();

private: // disable copy constructor and copy assignment operator
    Hal3AYuv(const Hal3AYuv&);
    Hal3AYuv& operator=(const Hal3AYuv&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interfaces.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    //
    static Hal3AYuv* createInstance(MINT32 i4SensorDevId, MINT32 i4SensorOpenIndex);
    static Hal3AYuv* getInstance(MINT32 i4SensorDevId);
    virtual MVOID destroyInstance();
    //virtual MBOOL sendCommandDerived(ECmd_T const eCmd, MINTPTR const i4Arg = 0);
    virtual MBOOL setParams(Param_T const &rNewParam);
    virtual MBOOL autoFocus();
    virtual MBOOL cancelAutoFocus();
    virtual MBOOL setZoom(MUINT32 u4ZoomRatio_x100, MUINT32 u4XOffset, MUINT32 u4YOffset, MUINT32 u4Width, MUINT32 u4Height);
    //virtual MBOOL set3AEXIFInfo(IBaseCamExif *pIBaseCamExif) const;
    virtual MBOOL getDebugInfo(android::Vector<MINT32>& keys, android::Vector< android::Vector<MUINT8> >& data) const;
    virtual MINT32 getDelayFrame(EQueryType_T const eQueryType) const;
    virtual MINT32 attachCb(I3ACallBack* cb);
    //virtual MINT32 detachCb(I3ACallBack* cb);
    virtual MBOOL setIspPass2(MINT32 flowType, const NSIspTuning::RAWIspCamInfo& rCamInfo, void* pRegBuf);
    virtual MINT32 updateCaptureParams(CaptureParam_T &a_rCaptureInfo);
    //virtual MINT32 isNeedFiringFlash(MBOOL bEnCal);
    virtual MVOID setSensorMode(MINT32 i4SensorMode);
    virtual MVOID setFDEnable(MBOOL fgEnable);
    virtual MBOOL setFDInfo(MVOID* prFaces);
    virtual MVOID notifyP1Done(MUINT32 u4MagicNum, MVOID* pvArg = 0);

    virtual MINT32 send3ACtrl(E3ACtrl_T e3ACtrl, MINTPTR i4Arg1, MINTPTR i4Arg2);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementation
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
     //
    MRESULT initYUV(MINT32 i4SensorDevId, MINT32 i4SensorOpenIndex);

    MRESULT uninitYUV();


protected: //private:

    virtual MBOOL           validatePass2(MINT32 flowType, const NSIspTuning::RAWIspCamInfo& rCamInfo, void* pRegBuf);
    virtual MBOOL           setSensorAndPass1(const ParamIspProfile_T& rParamIspProfile, MBOOL fgPerframe);
    virtual MINT32          getCurrResult(MUINT32 i4FrmId, Result_T& rResult) const;
    virtual MVOID           updateResult(MUINT32 u4MagicNum);
    virtual MBOOL           isStrobeBVTrigger();
    virtual MBOOL           setFlashLightOnOff(MBOOL bOnOff/*1=on; 0=off*/, MBOOL bMainPre/*1=main; 0=pre*/);

    virtual MBOOL           postCommandDerived(ECmd_T const r3ACmd, MINTPTR const i4Arg = 0);
    virtual MVOID           updateImmediateResult(MUINT32 u4MagicNum);

    virtual MBOOL           get3AEXIFInfo(EXIF_3A_INFO_T& rExifInfo) const;
    virtual MBOOL           getASDInfo(ASDInfo_T& rASDInfo) const;
    virtual MINT32          getCurrentHwId() const;

    MVOID           createThreadYUV();
    MVOID           destroyThreadYUV();
    //MVOID           changeThreadSetting();
    static  MVOID*  onThreadLoop(MVOID*);
    //MVOID           addCommandQ(ECmd_T const &r3ACmd, MINT32 i4Arg = 0);
    //MVOID           clearCommandQ();
    //MBOOL           getCommand(CmdQ_T &rCmd, MBOOL &bGetCmd, MBOOL en_timeout = MFALSE);
    //MBOOL           waitVSirqThenGetCommand(CmdQ_T &rCmd);
    //MBOOL           sem_wait_relativeTime(sem_t *pSem, nsecs_t reltime, const char* info);
    virtual MBOOL sendCommand(ECmd_T const eCmd, MINTPTR const i4Arg = 0);


    /**
    * @brief Enable AF thread
    * @param [in] a_bEnable set 1 to enable AF thread
    */
    virtual MRESULT EnableAFThread(MINT32 a_bEnable);

private:
    /**
    * @brief AF thread execution function
    */
    static MVOID* AFThreadFunc(void *arg);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Data member
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
private:
    volatile int  m_Users;
    mutable Mutex m_Lock;
    mutable Mutex m_ValidateLock;

    //I3ACallBack*    mpCbSet;
    //AAA_Scheduler*  mpScheduler;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  add 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
private:
        IHalSensor*   m_pSensorHal;
        mutable Mutex m_LockAF;
        MRESULT       m_errorCode;
        Param_T       m_rParam;
        MBOOL         m_bForceUpdatParam;
        MBOOL         m_bReadyToCapture;
        MINT32        m_i4SensorDev;
        //I3ACallBack*  m_pAFYuvCallBack;
        
        //for ASD
        MINT32        m_i4AELv_x10;
        MBOOL         m_bAEStable;
        SENSOR_AE_AWB_REF_STRUCT m_AsdRef;
        
        //AF related
        MINT32        m_bIsdummylens;
        MINT32        m_AFzone[6];
        MINT32        m_AEzone[6];
        AREA_T_YUV        m_sAFAREA[MAX_FOCUS_AREAS];      //
        AREA_T_YUV        m_sAEAREA[MAX_METERING_AREAS];   //
        MINT32        m_imageXS;
        MINT32        m_imageYS;
        MINT32        m_max_metering_areas;
        MINT32        m_max_af_areas;
        MINT32        m_i4PreAfStatus;
        MINT32        m_i4PreAfMode;
        MINT32        m_i4AFSwitchCtrl;
        MINT32        m_i4AutoFocus;
        MINT32        m_i4AutoFocusTimeout;
        MINT32        m_i4FDFrmCnt;
        MINT32        m_i4FDApplyCnt;
        MINT32        m_i4WinState;
        MINT32        m_i4State;
        MBOOL         m_fgAfTrig;
        
        MBOOL         bAELockSupp;
        MBOOL         bAWBLockSupp;
        MBOOL         m_bAeLimiter;
        
        //Strobe related
        StrobeDrv     *m_pStrobeDrvObj;
        double        m_strobeTrigerBV;
        double        BV_THRESHOLD;
        double        m_strobecurrent_BV;
        double        m_strobePreflashBV;
        MINT32        m_aeFlashlightType;
        SENSOR_FLASHLIGHT_AE_INFO_STRUCT m_AEFlashlightInfo;
        MINT32        pre_shutter;
        MINT32        pre_gain;
        MUINT32       m_preflashFrmCnt;
        MUINT32       m_strobeWidth;
        MBOOL         m_bFlashActive;
        MBOOL         m_bExifFlashOn;
        MBOOL         m_isFlashOnCapture;
        MBOOL         m_TrigFlashFire;
        
        // AF thread
        IspDrv*       m_pIspDrv;
        MINT32        m_bAFThreadLoop;
        pthread_t     m_AFThread;
        sem_t         m_semAFThreadStart;
        sem_t         m_semCBStart;
        sem_t         m_semCBEnd;
        MBOOL         m_CBUpdate;
private:
    /**
         * @brief Set auto exposure lock.
         * @param [in] bLock 1 for lock, 0 for unlock
         */
        MBOOL setAeLock(MBOOL bLock);
    
        /**
         * @brief Set auto white balance lock.
         * @param [in] bLock 1 for lock, 0 for unlock
         */
        MBOOL setAwbLock(MBOOL bLock);
    
        /**
         * @brief Map EV value to enum type AE_EVCOMP_T.
         * @param [in] mval, mstep
         * @return
         * - AE_EVCOMP_T refer to kd_camera_feature_enum.h in detail.
         */
        MINT32 mapAEToEnum(MINT32 mval,MFLOAT mstep);
    
        /**
         * @brief Map ISO value to enum type AE_ISO_T.
         * @param [in] u4NewAEISOSpeed
         * @return
         * - AE_ISO_T refer to kd_camera_feature_enum.h in detail.
         */
        MINT32 mapISOToEnum(MUINT32 u4NewAEISOSpeed);
    
        /**
         * @brief Map enum type AE_ISO_T to ISO value.
         * @param [in] u4NewAEIsoEnum.
         * @return
         * - ISO value
         */
        MINT32 mapEnumToISO(MUINT32 u4NewAEIsoEnum) const;
    
        //AF related
    
        /**
         * @brief Do AF update for reading AF status and callback for drawing AF window in AFThreadFunc.
         */
        MINT32 doAFUpdate(void);
    
        /**
         * @brief Set AF mode.
         * @param [in] AFMode refer to AF_MODE_T in kd_camera_feature_enum.h.
         */
        MINT32 setAFMode(MINT32 AFMode);
    
        /**
         * @brief Get AF status.
         * @return
         * - SENSOR_AF_FOCUSED indicates focused and lens stops moving.
         * - SENSOR_AF_FOCUSING indicates lens is moving for focusing.
         * - SENSOR_AF_IDLE indicates lens idle and no action.
         * - SENSOR_AF_ERROR indicates error happened.
         */
        MINT32 isFocused();
    
        /**
         * @brief Set focus area.
         * @param [in] a_i4Cnt number of area; a_psFocusArea array of areas.
         */
        MVOID  setFocusAreas(MINT32 a_i4Cnt, AREA_T_YUV *a_psFocusArea);
    
        /**
         * @brief Get focus area.
         * @param [out] a_i4Cnt number of area; a_psFocusArea array of areas.
         */
        MVOID  getFocusAreas(MINT32 &a_i4Cnt, AREA_T_YUV **a_psFocusArea);
    
        /**
         * @brief Utility for clamp x at range from min to max.
         * @param [in] x, min, max
         * @return
         * - clamped value
         */
        MINT32 clamp(MINT32 x, MINT32 min, MINT32 max);
    
        /**
         * @brief Get AE metering area.
         * @param [out] a_i4Cnt number of area; a_psAEArea array of areas.
         */
        MVOID  getMeteringAreas(MINT32 &a_i4Cnt, AREA_T_YUV **a_psAEArea);
    
        /**
         * @brief Set AE metering area.
         * @param [in] a_i4Cnt number of area; a_psAEArea array of areas.
         */
        MVOID  setMeteringAreas(MINT32 a_i4Cnt, AREA_T_YUV const *a_psAEArea);
    
        /**
         * @brief Map area to zone.
         * @param [in] p_area pointer to area.
         * @param [in] areaW, areaH dimension of area coordinate.
         * @param [in] zoneW, zoneH dimension of zone coordinate.
         * @param [out] p_zone pointer to zone.
         */
        MVOID  mapAeraToZone(AREA_T_YUV *p_area, MINT32 areaW,
                             MINT32 areaH, MINT32* p_zone,
                             MINT32 zoneW, MINT32 zoneH);
    
        /**
         * @brief Reset AF and AE window to center.
         */
        MBOOL  resetAFAEWindow();
    
        /**
         * @brief Set flashlight mode.
         * @param [in] mode refer to AE_STROBE_T in kd_camera_feature_enum.h.
         */
        MINT32 setFlashMode(MINT32 mode);
    
        /**
         * @brief Check if flashlight can be on.
         * @return
         * - MTRUE indicates ON.
         * - MFALSE indicates OFF.
         */
        MINT32 isAEFlashOn();
    
        /**
         * @brief Set lamp on or off in AF process
         * @param [in] bOnOff
         * @return
         * - S_3A_OK indicates OK.
         * - E_3A_NULL_OBJECT indicates no strobe.
         */
        MINT32 setAFLampOnOff(MBOOL bOnOff);
    
        /**
         * @brief Log2 operation
         * @param [in] x
         * @return
         * - Log2(x)
         */
        double AEFlashlightLog2(double x);
    
        /**
         * @brief Calculate brightness value.
         * @return
         * - BV
         */
        double calcBV();
    
        /**
         * @brief Query exposure info from sensor.
         */
        MINT32 queryAEFlashlightInfoFromSensor();
    
        /**
         * @brief Update capture parameters for flashlight on.
         */
        MINT32 updateAeFlashCaptureParams();
    
        /**
         * @brief Set exposure time and sensor gain.
         * @param [in] a_u4ExpTime exposure time in microsecond
         * @param [in] a_u4SensorGain sensor gain in GAIN_BASE
         */
        MINT32 setEShutterParam(MUINT32 a_u4ExpTime, MUINT32 a_u4SensorGain);
    
        /**
         * @brief Get HDR capture info.
         * @param [out] a_strHDROutputInfo
         */
        MINT32 getHDRCapInfo(Hal3A_HDROutputParam_T &a_strHDROutputInfo);
    
        /**
         * @brief Convert flashlight exposure parameters
         */
        void convertFlashExpPara(MINT32 flashEngRatio_x10, MINT32 minAfeGain_x1024,
                                     MINT32 bv0_x1024, MINT32 bv1_x1024,
                                     MINT32  exp1, MINT32  afeGain1_x1024, MINT32  ispGain1_x1024,
                                     MINT32& exp2, MINT32& afeGain2_x1024, MINT32& ispGain2_x1024) const;
    
    
    
        MINT32 ASDLog2Func(MUINT32 numerator, MUINT32 denominator) const;
    
        MBOOL isInVideo();

};

template <MINT32 sensorDevId>
class Hal3AYuvDev : public Hal3AYuv
{
public:
    static Hal3AYuvDev* getInstance()
    {
        static Hal3AYuvDev<sensorDevId> singleton;
        return &singleton;
    }
    Hal3AYuvDev()
        : Hal3AYuv()
    {
    }

private:

};


}; // namespace NS3Av3

#endif

