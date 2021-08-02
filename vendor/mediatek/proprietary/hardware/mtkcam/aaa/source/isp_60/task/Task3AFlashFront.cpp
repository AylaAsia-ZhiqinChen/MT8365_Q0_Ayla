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
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#define LOG_TAG "Task3AFlashFront"

#include <mtkcam/utils/std/Log.h>
#include <cutils/properties.h>
#include <mtkcam/utils/std/Trace.h>
// 3A framework
#include <task/ITask.h>
#include <aaa_trace.h>
#include <aaa_types.h>
#include <aaa_timer.h>
#include <aaa_common_custom.h>
#include <aaa_hal_sttCtrl.h>
#include <IThreadRaw.h>
#include <debug/DebugUtil.h>
#include <file_utils.h>
// 3A module
#include <af_feature.h>
#include <af_algo_if.h>
#include <ae_mgr/ae_mgr_if.h>
#include <af_mgr/af_mgr_if.h>
#include <awb_mgr/awb_mgr_if.h>
#include <lsc_mgr/ILscTsf.h>
#include <flash_hal.h>
#include <flash_mgr.h>
// 3A Custom
#include <awb_tuning_custom.h>
#include <flash_awb_tuning_custom.h>


using namespace NS3Av3;

/*********************************************************
 *
 *   Interface of Task3AFlashFrontImp
 *
 *********************************************************/

class Task3AFlashFrontImp : public Task3AFlashFront
{
public:
    virtual MVOID destroy();
            Task3AFlashFrontImp(MINT32 const i4SensorDev);
    virtual ~Task3AFlashFrontImp();

    virtual TASK_RESULT run(MINT32 i4Opt, const TaskData& rData);
    virtual MVOID cancel();
    virtual MVOID waitFinish();
    virtual MINT32 getStatus();
    virtual TaskInfo getInfo(){return m_rTaskInfo;};

private:
    virtual MVOID updateStatus(MINT32 i4Status);
    MINT32 m_i4SensorDev;
    MINT32 m_i4FrameCount;
    MINT32 m_i4Status;
    MINT32 m_i4LogEn;
    MINT32 m_i4SkipFrame;
    AaaTimer m_rTimer;
    mutable std::mutex m_StatusLock;
    mutable std::mutex m_RunningLock;
};

/*********************************************************
 *
 *   Task3AFlashFront implement functions
 *
 *********************************************************/

Task3AFlashFront*
Task3AFlashFront::create(MINT32 const i4SensorDev, MINT32 i4Prioirty)
{
    return new Task3AFlashFrontImp(i4SensorDev);
}

/*********************************************************
 *
 *   Task3AFlashFrontImp implement functions
 *
 *********************************************************/

Task3AFlashFrontImp::Task3AFlashFrontImp(MINT32 const i4SensorDev)
    : m_i4SensorDev(i4SensorDev)
    , m_i4FrameCount(0)
    , m_i4Status(TASK_STATUS_INIT)
    , m_i4LogEn(0)
    , m_i4SkipFrame(-1)
{
    m_i4LogEn = property_get_int32("vendor.debug.hal3a.task",0);
}

Task3AFlashFrontImp::~Task3AFlashFrontImp()
{
}

MVOID
Task3AFlashFrontImp::destroy()
{
    CAM_LOGW("[%s] +", __FUNCTION__);
    waitFinish();
    delete this;
    CAM_LOGW("[%s] -", __FUNCTION__);
}

TASK_RESULT
Task3AFlashFrontImp::run(MINT32 i4Opt, const TaskData& rData)
{
    std::lock_guard<std::mutex> lock(m_RunningLock);
    MBOOL bAFAELock = MFALSE;
    IAeMgr::getInstance().sendAECtrl(m_i4SensorDev, EAECtrl_SetAFAELock, bAFAELock, NULL, NULL, NULL);
    IAfMgr::getInstance(m_i4SensorDev).notify(E_AFNOTIFY_UNLOCKAE);
    TASK_RESULT eResult = TASK_RESULT_OK;
    MINT32 isMvHDREnable = 0;

/*

    if(m_i4SkipFrame >= 0){
        if(m_i4SkipFrame == 0)
            updateStatus(TASK_STATUS_DONE);
        m_i4SkipFrame--;
        CAM_LOGW("[%s] SkipFrame(%d)", __FUNCTION__, m_i4SkipFrame);
        return eResult;
    }

*/
    if(getStatus() >= TASK_STATUS_CANCELLED)
        return eResult;

    AE_PERFRAME_INFO_T AEPerframeInfo;
    IAeMgr::getInstance().getAEInfo(m_i4SensorDev, AEPerframeInfo);
    //===== prepare statistic data =====
    StatisticBufInfo* pAAOBuffer = NULL;

    //===== prepare params =====
    FlashExePara para;
    FlashExeRep rep;
    memset(&rep, 0, sizeof(rep));
    FLASH_AWB_PASS_FLASH_INFO_T passFlashAwbData;
    AWB_STAT_PARAM_T rAWBStatParam;
    MINT32 i4AoeCompLv = 80;
    if(rData.pAAOBuffer)
        pAAOBuffer = reinterpret_cast<StatisticBufInfo*>(rData.pAAOBuffer);

    if(pAAOBuffer == NULL)
    {
        CAM_LOGW("[%s] AAO buffer is NULL", __FUNCTION__);
        eResult = TASK_RESULT_ERROR;
        return eResult;
    }

    if(getStatus() >= TASK_STATUS_CANCELLED)
        return eResult;

    //===== Backup AE & AWB =====
    if(getStatus() == TASK_STATUS_INIT)
    {
        MBOOL isFlashOn = FlashHal::getInstance(m_i4SensorDev)->isAFLampOn();
        if(!isFlashOn)
        {
            IAeMgr::getInstance().sendAECtrl(m_i4SensorDev, EAECtrl_SetDoBackAEInfo, NULL, NULL, NULL, NULL);
            IAwbMgr::getInstance().backup(m_i4SensorDev);
        }
        // dump AE EXIF for Flash debug
        if(DebugUtil::getDebugLevel(DBG_FLASH))
        {
            MINT32 flashCount = 0;
            char fileName[512];
            char folderName[512];
            getFileCount("/sdcard/flash_file_cnt.txt", &flashCount, 0);
            sprintf(folderName, "/sdcard/flashdata/pf/");
            sprintf(fileName, "/sdcard/flashdata/pf/ae_exif_front_%03d.bin", flashCount);

            AE_DEBUG_INFO_T rAEDebugInfo;
            AE_DEBUG_DATA_T rAEDebugData;
            IAeMgr::getInstance().sendAECtrl(m_i4SensorDev, EAECtrl_GetDebugInfo, reinterpret_cast<MINTPTR>(&rAEDebugInfo), reinterpret_cast<MINTPTR>(&rAEDebugData), NULL, NULL);
            dumpBufToFile(fileName, folderName, &rAEDebugInfo, sizeof(AE_DEBUG_INFO_T));
        }
    }

    //===== Flash calculation =====

    if(!(i4Opt & TASK_OPTION_BYP_FLASH) && !(m_i4SkipFrame >= 0))
    {
        AAA_TRACE_D("doFlash");
        m_rTimer.start("Flash calc",m_i4SensorDev, m_i4LogEn, 1);
        if(pAAOBuffer)
        {
            //for temp solution
            rAWBStatParam.i4WindowNumX = 120;
            rAWBStatParam.i4WindowNumY = 90;

            AWB_OUTPUT_T awb_out = {};
            IAwbMgr::getInstance().getAWBOutput(m_i4SensorDev, awb_out);

            para.staBuf =reinterpret_cast<MVOID *>(pAAOBuffer->mVa);
            para.staX = rAWBStatParam.i4WindowNumX;
            para.staY = rAWBStatParam.i4WindowNumY;
            para.NonPreferencePrvAWBGain = awb_out.rPreviewFullWBGain;
            para.PreferencePrvAWBGain = awb_out.rPreviewAWBGain;
            para.i4AWBCCT = awb_out.rAWBInfo.i4CCT;
            //Set MvHDREnable
            isMvHDREnable = Hal3ASttCtrl::getInstance(m_i4SensorDev)->isMvHDREnable();
            FlashMgr::getInstance(m_i4SensorDev)->setMVHDRMode(isMvHDREnable);
            AE_MODE_CFG_T rCaptureInfo;
            IAeMgr::getInstance().sendAECtrl(m_i4SensorDev, EAECtrl_GetCapParams, reinterpret_cast<MINTPTR>(&rCaptureInfo), NULL, NULL, NULL);
            FlashMgr::getInstance(m_i4SensorDev)->setAlgoExpPara(AEPerframeInfo.rAEISPInfo.u4AlgoExposuretime_us,AEPerframeInfo.rAEISPInfo.u4AlgoSensorGain,AEPerframeInfo.rAEISPInfo.u4AlgoDGNGain,0);
#if CAM3_HIGH_BRIGHTNESS_FLOW_EN
            /* check whether current environment is high-brightness or not*/
            if((FlashMgr::getInstance(m_i4SensorDev)->getFlashMode() == 1) && (!IAeMgr::getInstance().IsStrobeBVTrigger(m_i4SensorDev)))
                 FlashMgr::getInstance(m_i4SensorDev)->setPfHighBrightnessFlow(1);
#endif
            CAM_LOGW("isHighBrightnessFlow(%d)",FlashMgr::getInstance(m_i4SensorDev)->isHighBrightnessFlow());
            FlashMgr::getInstance(m_i4SensorDev)->doPfOneFrame(&para, &rep);
        if(rep.isCurFlashOn && !(FlashHal::getInstance(m_i4SensorDev)->isAFLampOn()))
            eResult = TASK_RESULT_PREFLASH_REQUIRE;
        }else
            CAM_LOGW("[%s] AAO buffer is NULL", __FUNCTION__);
        m_rTimer.End();
        AAA_TRACE_END_D;

        //===== Flash calibration =====
        if(rep.isFlashCalibrationMode == 1)
        {
            ILscMgr::getInstance(static_cast<MUINT32>(m_i4SensorDev))->setTsfOnOff(0);
            IAfMgr::getInstance(m_i4SensorDev).setPauseAF( MTRUE);
        }
    }

    if(getStatus() >= TASK_STATUS_CANCELLED)
        return eResult;

    //set awb gain to ae_mgr
    AWB_OUTPUT_T rAWBOutput;
    IAwbMgr::getInstance().getAWBOutput(m_i4SensorDev, rAWBOutput);
    CCU_AAO_PROC_INFO_T rProcInfo;
    rProcInfo.awb_gain.i4R = rAWBOutput.rAWBInfo.rCurrentAWBGain.i4R;
    rProcInfo.awb_gain.i4G = rAWBOutput.rAWBInfo.rCurrentAWBGain.i4G;
    rProcInfo.awb_gain.i4B = rAWBOutput.rAWBInfo.rCurrentAWBGain.i4B;
    IAeMgr::getInstance().sendAECtrl(m_i4SensorDev, EAECtrl_SetAAOProcInfo, NULL, reinterpret_cast<MINTPTR>(&(rProcInfo)), NULL, NULL);

    //===== AWB calculation =====
    if(!(i4Opt & TASK_OPTION_BYP_AWB))
    {
        AAA_TRACE_D("doFlashAWB");
        m_rTimer.start("AWB calc",m_i4SensorDev, m_i4LogEn, 1);
        IAeMgr::getInstance().sendAECtrl(m_i4SensorDev, EAECtrl_SetUpdateAEBV, reinterpret_cast<MINTPTR>((MVOID *)(pAAOBuffer->mVa)), NULL, NULL, NULL);
        i4AoeCompLv = AEPerframeInfo.rAEUpdateInfo.i4AOECompLVvalue;
        if(rep.isCurFlashOn == 0 && !(FlashHal::getInstance(m_i4SensorDev)->isAFLampOn()))
            IAwbMgr::getInstance().setStrobeMode(m_i4SensorDev, AWB_STROBE_MODE_OFF);
        else
            IAwbMgr::getInstance().setStrobeMode(m_i4SensorDev, AWB_STROBE_MODE_ON);

        if(pAAOBuffer)
            IAwbMgr::getInstance().doPreCapAWB(m_i4SensorDev, i4AoeCompLv, reinterpret_cast<MVOID *>(pAAOBuffer->mVa));
        else
            CAM_LOGW("[%s] AAO buffer is NULL", __FUNCTION__);
        m_rTimer.End();
        AAA_TRACE_END_D;
    }

    //===== Print log =====
    MINT32 i4AeLv = 0;
    MUINT32 u4RealISO = 0;

    u4RealISO = AEPerframeInfo.rAEISPInfo.u4P1RealISOValue;
    i4AeLv = AEPerframeInfo.rAEISPInfo.i4LightValue_x10;
    CAM_LOGD("[%s] Cam %d : Req(#%d)/Stt(#%d)/i4Opt(%d)/FlashOnOff(%d) Lv(%d)/ISO(%d)/RGB(%d,%d,%d) isMvHDREnable(%d)",__FUNCTION__, m_i4SensorDev,
        rData.i4RequestMagic, pAAOBuffer->mMagicNumber, i4Opt, rData.bFlashOnOff,
        i4AeLv, u4RealISO,
        rAWBOutput.rAWBInfo.rCurrentAWBGain.i4R,
        rAWBOutput.rAWBInfo.rCurrentAWBGain.i4G,
        rAWBOutput.rAWBInfo.rCurrentAWBGain.i4B,
        isMvHDREnable);

    m_i4FrameCount++;
    CAM_LOGD("[%s] Frame Count(%d)",__FUNCTION__, m_i4FrameCount);

    if(m_i4SkipFrame >= 0){
        if(m_i4SkipFrame == 0)
            updateStatus(TASK_STATUS_DONE);
        else
            updateStatus(TASK_STATUS_RUNNING);
        m_i4SkipFrame--;
        CAM_LOGW("[%s] SkipFrame(%d)", __FUNCTION__, m_i4SkipFrame);
        return eResult;
    }

    //===== done =====
    if(!FlashMgr::getInstance(m_i4SensorDev)->isFlashOnCalibration())
    {
        if (eResult != TASK_RESULT_PREFLASH_REQUIRE){
            updateStatus(TASK_STATUS_DONE);
        } else {
            m_i4SkipFrame = 1;
        }
        return eResult;
    } else {
            IAfMgr::getInstance(m_i4SensorDev).setPauseAF( MFALSE);
    }

    //===== Timeout =====
    if(m_i4FrameCount >= 30 && !FlashMgr::getInstance(m_i4SensorDev)->isFlashOnCalibration())
    {
        //IAeMgr::getInstance().sendAECtrl(m_i4SensorDev, EAECtrl_SetState2Converge, NULL, NULL, NULL, NULL);
#if CAM3_FLASH_FEATURE_EN
        FlashMgr::getInstance(m_i4SensorDev)->endPrecapture();
#endif
        updateStatus(TASK_STATUS_DONE);
        CAM_LOGD("[%s] Task is timeout(%d)",__FUNCTION__, m_i4FrameCount);
    }

    if(getStatus() >= TASK_STATUS_CANCELLED)
        return eResult;

    updateStatus(TASK_STATUS_RUNNING);
    return eResult;
}

MVOID
Task3AFlashFrontImp::cancel()
{
    IAeMgr::getInstance().sendAECtrl(m_i4SensorDev, EAECtrl_SetState2Converge, NULL, NULL, NULL, NULL);
    updateStatus(TASK_STATUS_CANCELLED);
}

MVOID
Task3AFlashFrontImp::waitFinish()
{
    CAM_LOGD("[%s] wait for Task done +", __FUNCTION__);
    std::lock_guard<std::mutex> lock(m_RunningLock);

    MINT32 i4Status = getStatus();
    if(i4Status < TASK_STATUS_CANCELLED)
        cancel();

    updateStatus(TASK_STATUS_DONE);
    CAM_LOGD("[%s] wait for Task done -", __FUNCTION__);
}

MINT32
Task3AFlashFrontImp::getStatus()
{
    std::lock_guard<std::mutex> lock(m_StatusLock);
    return m_i4Status;
}

MVOID
Task3AFlashFrontImp::updateStatus(MINT32 i4Status)
{
    std::lock_guard<std::mutex> lock(m_StatusLock);
    if(m_i4Status == TASK_STATUS_INIT && i4Status == TASK_STATUS_RUNNING)
    {
        AAA_TRACE_D("Task3AFlashFront_start");
        AAA_TRACE_END_D;
    }
    if(m_i4Status == TASK_STATUS_RUNNING && i4Status == TASK_STATUS_DONE)
    {
        AAA_TRACE_D("Task3AFlashFront_end");
        AAA_TRACE_END_D;
    }
    if(m_i4Status != i4Status)
    {
        CAM_LOGD("[%s] status(%d->%d)", __FUNCTION__, m_i4Status, i4Status);
        m_i4Status = i4Status;
    }
}
