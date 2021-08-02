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

#ifndef _AWB_MGR_H_
#define _AWB_MGR_H_

#include <private/aaa_hal_private.h>
#include <dbg_aaa_param.h>
#include <awb_algo_if.h>
#include <libsync3a/MTKSyncAwb.h>
#include <isp_tuning.h>
#include <awb_mgr_if.h>


//tmp
typedef struct
{
   MBOOL bZoomChange;
   MUINT32 u4XOffset;
   MUINT32 u4YOffset;
   MUINT32 u4XWidth;
   MUINT32 u4YHeight;
} AWBZOOM_WINDOW_T;

using namespace NSIspTuning;

namespace NS3Av3
{

// AWB statistics info
typedef struct
{
  MINT32 i4SensorWidth[ESensorMode_NUM];   // Sensor width
    MINT32 i4SensorHeight[ESensorMode_NUM];  // Sensor height
  MINT32 i4NumX[ESensorMode_NUM];          // AWB window number (Horizontal)
  MINT32 i4NumY[ESensorMode_NUM];          // AWB window number (Vertical)
  MINT32 i4SizeX[ESensorMode_NUM];         // AWB window size (Horizontal)
  MINT32 i4SizeY[ESensorMode_NUM];         // AWB window size (Vertical)
  MINT32 i4PitchX[ESensorMode_NUM];        // AWB window pitch (Horizontal)
  MINT32 i4PitchY[ESensorMode_NUM];        // AWB window pitch (Vertical)
  MINT32 i4OriginX[ESensorMode_NUM];       // AWB window origin (Horizontal)
  MINT32 i4OriginY[ESensorMode_NUM];       // AWB window origin (Vertical)
//------------------------------------------------------------------------------------
    MBOOL  bNeedCrop[ESensorMode_NUM];           // if need crop
    MINT32 i4CropOffsetX[ESensorMode_NUM];       // crop offset x
    MINT32 i4CropOffsetY[ESensorMode_NUM];       // crop offset y
    MINT32 i4CropRegionWidth[ESensorMode_NUM];   // crop region width
    MINT32 i4CropRegionHeight[ESensorMode_NUM];  // crop region height
} AWB_WINDOW_CONFIG_T;

class AwbStateMgr;

class AwbMgr
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Ctor/Dtor.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
private:    ////    Disallowed.
    //  Copy constructor is disallowed.
    AwbMgr(AwbMgr const&);
    //  Copy-assignment operator is disallowed.
    AwbMgr& operator=(AwbMgr const&);

public:  ////
    AwbMgr(ESensorDev_T eSensorDev);
    ~AwbMgr();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Operations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    static AwbMgr& getInstance(MINT32 const i4SensorDev = ESensorDev_None);
    static AwbMgr* s_pAwbMgr; // FIXME: REMOVED LATTER
    MBOOL cameraPreviewInit(MINT32 i4SensorIdx, Param_T &rParam, IAWBInitPara* initPara);
    MBOOL camcorderPreviewInit(MINT32 i4SensorIdx, Param_T &rParam, IAWBInitPara* initPara);
    MBOOL cameraCaptureInit(IAWBInitPara* initPara);
    MBOOL cameraPreviewReinit(IAWBInitPara* initPara);

    MBOOL start();
    MBOOL stop();
    MBOOL init(MINT32 i4SensorIdx, IAWBInitPara* initPara);
    MBOOL uninit();

    inline MBOOL isAWBEnable()
    {
        return m_bEnableAWB;
    }

    MBOOL setAWBMode(MINT32 i4NewAWBMode);
    MINT32 getAWBMode() const;
    MBOOL setSensorMode(MINT32 i4NewSensorMode, MINT32 i4BinWidth, MINT32 i4BinHeight, MINT32 i4QbinWidth, MINT32 i4QbinHeight);
    MINT32 getSensorMode() const;
    MBOOL setStrobeMode(MINT32 i4NewStrobeMode);
    MINT32 getStrobeMode() const;
    MBOOL setFlashAWBData(FLASH_AWB_PASS_FLASH_INFO_T &rFlashAwbData);
    MBOOL setAWBLock(MBOOL bAWBLock);
    MBOOL enableAWB();
    MBOOL disableAWB();
    MBOOL setAWBStatCropRegion(MINT32 i4SensorMode, MINT32 i4CropOffsetX, MINT32 i4CropOffsetY, MINT32 i4CropRegionWidth, MINT32 i4CropRegionHeight);
    MBOOL doPvAWB(MINT32 i4FrameCount, MBOOL bAEStable, MINT32 i4SceneLV, MVOID *pAWBStatBuf, MBOOL bApplyToHW);
    MBOOL doAFAWB(MVOID *pAWBStatBuf, MINT32 i4SceneLV, MBOOL bApplyToHW);
    MBOOL doPreCapAWB(MINT32 i4SceneLV, MVOID *pAWBStatBuf, MBOOL bApplyToHW);
    MBOOL doCapAWB(MINT32 i4SceneLV, MVOID *pAWBStatBuf, MBOOL bApplyToHW);
    MBOOL doCapFlashAWBDisplay(MBOOL bApplyToHW);
    MBOOL applyAWB(AWB_SYNC_OUTPUT_N3D_T &rAWBSyncOutput);
    MBOOL getDebugInfo(AWB_DEBUG_INFO_T &rAWBDebugInfo, AWB_DEBUG_DATA_T &rAWBDebugData);
    MINT32 getAWBCCT();
    MBOOL getASDInfo(AWB_ASD_INFO_T &a_rAWBASDInfo);
    MBOOL getAWBOutput(AWB_OUTPUT_T &a_rAWBOutput);
    MBOOL getAWBGain(AWB_GAIN_T& rAwbGain, MINT32& i4ScaleUnit) const;
    MRESULT getAWBParentStat(AWB_PARENT_BLK_STAT_T &a_rAWBParentState,  MINT32 *a_ParentBlkNumX, MINT32 *a_ParentBlkNumY);

    inline MVOID setAFLV(MINT32 i4AFLV)
    {
         m_i4AFLV = i4AFLV;
    }

    inline MINT32 getAFLV()
    {
         return m_i4AFLV;
    }

    //MBOOL getInitInputParam(SYNC_CTC_INIT_PARAM &CCTInitParam, NORMAL_GAIN_INIT_INPUT_PARAM &NoramlGainInitParam);
    MBOOL queryHBinInfo();
    MBOOL setTGInfo(MINT32 const i4TGInfo);
    MBOOL setAWBNvramIdx(MINT32 const i4AWBNvramIdx);
    MRESULT setCamScenarioMode(MUINT32 u4NewCamScenarioMode);
    MBOOL setIsMono(int isMono , MUINT32 i4SensorType);
    MBOOL setRgbGain(MINT32 const i4SensorDev, AWB_GAIN_T& rIspAWBGain);
    MBOOL setAEPreGain2(MINT32 const i4SensorDev, MINT32  sensorIdx, AWB_GAIN_T& rPreGain2);
    MBOOL setIspAwbInfo(MINT32 const i4SensorDev, AWB_INFO_T &rAWBInfo);
    // Face detection
    MRESULT setFDenable(MBOOL bFDenable);
    MRESULT setFDInfo(MVOID* a_sFaces, MINT32 i4tgwidth, MINT32 i4tgheight);
    MRESULT setZoomWinInfo(MUINT32 u4XOffset, MUINT32 u4YOffset, MUINT32 u4Width, MUINT32 u4Height);
    MRESULT getFocusArea( android::Vector<MINT32> &vecOut, MINT32 i4tgwidth, MINT32 i4tgheight);
    MBOOL CalSencondSensorDefaultGain(AWB_GAIN_T &gain, MINT32 cct);

    //__________________________Camera HAL3.2__________________________

  MRESULT backup();
  MRESULT restore();
    MBOOL getColorCorrectionGain(MFLOAT& fGain_R, MFLOAT& fGain_G, MFLOAT& fGain_B);
    MBOOL setColorCorrectionGain(MFLOAT fGain_R, MFLOAT fGain_G, MFLOAT fGain_B);
    MBOOL getAWBState(mtk_camera_metadata_enum_android_control_awb_state_t& eAWBState);
    MBOOL setColorCorrectionMode(MINT32 i4ColorCorrectionMode);

    MBOOL SetAETargetMode(int mode); //for hdr


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CCT feature
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    MRESULT CCTOPAWBEnable();
    MRESULT CCTOPAWBDisable();
    MRESULT CCTOPAWBGetEnableInfo(MINT32 *a_pEnableAWB,MUINT32 *a_pOutLen);
    MRESULT CCTOPAWBGetAWBGain(MVOID *a_pAWBGain, MUINT32 *a_pOutLen);
    MRESULT CCTOPAWBSetAWBGain(MVOID *a_pAWBGain);
    MRESULT CCTOPAWBApplyNVRAMParam(MVOID *a_pAWBNVRAM, MUINT32 u4CamScenarioMode);
    MRESULT CCTOPAWBGetNVRAMParam(MVOID *a_pAWBNVRAM, MUINT32 *a_pOutLen);
    MRESULT CCTOPAWBGetNVRAMParam(CAM_SCENARIO_T eIdx, MVOID *a_pAWBNVRAM);
    MRESULT CCTOPAWBSaveNVRAMParam();
    MRESULT CCTOPAWBSetAWBMode(MINT32 a_AWBMode);
    MRESULT CCTOPAWBGetAWBMode(MINT32 *a_pAWBMode, MUINT32 *a_pOutLen);
    MRESULT CCTOPAWBGetLightProb(MVOID *a_pAWBLightProb, MUINT32 *a_pOutLen);
    MRESULT CCTOPAWBBypassCalibration(MBOOL bBypassCalibration);
    MRESULT CCTOPFlashAWBApplyNVRAMParam(MVOID *a_pFlashAWBNVRAM);
    MRESULT CCTOPFlashAWBGetNVRAMParam(MVOID *a_pFlashAWBNVRAM, MUINT32 *a_pOutLen);
    MRESULT CCTOPFlashAWBSaveNVRAMParam();
    MBOOL setAAOMode(MUINT32 u4NewAWBAAOMode);
    MBOOL getAAOConfig(AWB_AAO_CONFIG_Param_T &rAWBConfig);
	MBOOL setMWBColorTemperature(int colorTemperature);
	MBOOL getSuppotMWBColorTemperature(MUINT32 &max, MUINT32 &min);
	MBOOL getAWBColorTemperature(MUINT32 &colorTemperature);
    MBOOL SetTorchMode(MBOOL is_torch_now);
    MBOOL SetMainFlashInfo(MBOOL is_main_flash_on);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Private function
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
private:
    MBOOL getSensorResolution();
    MBOOL getNvramData();
    MBOOL AWBInit();
    MBOOL AWBWindowConfig();
    MBOOL AWBStatConfig();
    MBOOL AWBRAWPreGain1Config();
    MBOOL getEEPROMData();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Data member
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
private:
    AWB_GAIN_T mBackupGain;
    NS3A::IAwbAlgo* m_pIAwbAlgo;
    AwbStateMgr* m_pAwbStateMgr;
    LIB3A_AWB_MODE_T m_eAWBMode;
    MINT32 m_i4StrobeMode;
    MINT32 m_i4AWBNvramIdx;
    CAM_SCENARIO_T m_eCamScenarioMode;
    MBOOL m_bEnableAWB;
    MBOOL m_bAWBLock;
    MBOOL m_bAdbAWBLock;
    MBOOL m_bOneShotAWB;
    MBOOL m_bAWBModeChanged;
    MBOOL m_bStrobeModeChanged;
    MBOOL m_bColdBoot;
    MBOOL m_bAWBNvramIdxChanged;
    ESensorDev_T m_eSensorDev;
    ESensorTG_T m_eSensorTG;
    ESensorMode_T m_eSensorMode;
    MINT32      m_i4SensorIdx;
    MBOOL m_bDebugEnable;
    MBOOL m_bInitState;
    MINT32 m_i4AFLV;
    int m_bSkipOneFrame;
    MBOOL m_bHBIN2Enable;
    MBOOL m_bAWBCalibrationBypassed;
    NVRAM_CAMERA_3A_STRUCT* m_pNVRAM_3A;
    NVRAM_CAMERA_FLASH_CALIBRATION_STRUCT* m_pNVRAM_FLASH_CALIBRATION;
    MUINT32 m_flashAwbWeight;
    MINT32 m_flashDuty;
    MINT32 m_flashStep;
    AWB_STAT_PARAM_T m_rAWBStatParam;
    AWB_WINDOW_CONFIG_T m_rAWBWindowConfig;
    AWB_GAIN_T m_rAWBRAWPreGain1;
    AWB_INIT_INPUT_T m_rAWBInitInput;
    AWB_OUTPUT_T m_rAWBOutput;
    AWB_STAT_CONFIG_T m_rAWBStatCfg[AWB_STROBE_MODE_NUM][ESensorMode_NUM][LIB3A_AWB_MODE_NUM];
    mtk_camera_metadata_enum_android_color_correction_mode_t m_eColorCorrectionMode;
    AWB_GAIN_T m_rColorCorrectionGain;
    MINT32 m_i4BinW;
    MINT32 m_i4BinH;
    MINT32 mAaoW;
    MINT32 mAaoH;
    MINT32 m_i4QbinRatio;
    UINT8 *mpAao;
    UINT8 *mpHist;
    int mIsMono;
    MUINT32 mSensorType;

    int mGainSetNum;
    AWB_GAIN_T mGoldenGain2;
    AWB_GAIN_T mGoldenGain3;
    AWB_GAIN_T mUnitGain2;
    AWB_GAIN_T mUnitGain3;
/*
    AWB_BAYER_VALUE_T mGoldenValueM;
    AWB_BAYER_VALUE_T mGoldenValueL;
    AWB_BAYER_VALUE_T mUnitValueM;
    AWB_BAYER_VALUE_T mUnitValueL;
*/
    MINT32   m_i4DgbLog;
    MUINT32 m_i4AAOmode;

    // Face detection
    BOOL m_bFDenable;
    BOOL m_bFaceAWBAreaChage;
    AWBMeteringArea_T m_eAWBFDArea;
    AWBZOOM_WINDOW_T m_eZoomWinInfo;

    // 2016/01/06 hdr
    MBOOL m_bFlashAwb;

    MBOOL m_bAlgoInit;
	MBOOL mIsMwbCctChanged;
    int mMWBColorTemperature;
    MBOOL m_bIsTorch;
    MBOOL m_bIsMainFlashOn;
    MUINT32 m_bIsUseLastGain;
    MINT32 AAOStride;
};

};  //  namespace NS3Av3
#endif // _AWB_MGR_H_

