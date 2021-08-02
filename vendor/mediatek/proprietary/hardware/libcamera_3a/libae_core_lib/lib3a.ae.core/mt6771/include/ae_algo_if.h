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

/**
 * @file ae_algo_if.h
 * @brief Interface to AE algorithm library
 */

#ifndef _AE_ALGO_IF_H_
#define _AE_ALGO_IF_H_

#include "aaa/ae_param.h"
#include "aaa_algo_option.h"
#define AE_ALGO_IF_REVISION   7471001 

namespace NS3A
{
/**
 * @brief Interface to AE algorithm library
 */
class IAeAlgo {

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:  //    Ctor/Dtor.
    IAeAlgo() {}
    virtual ~IAeAlgo() {}

private: // disable copy constructor and copy assignment operator
    IAeAlgo(const IAeAlgo&);
    IAeAlgo& operator=(const IAeAlgo&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interfaces.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    /**
     * @brief create instance
     * @param [in] eAAAOpt 3A option
     * @param [in] i4SensorDev input sensor device for algorithm init; please refer to isp_tuning.h
     */
    template <EAAAOpt_T const eAAAOpt>
    static  IAeAlgo* createInstance(MINT32 const i4SensorDev);
    /**
     * @brief destroy instance
     */
    virtual MVOID   destroyInstance() = 0;
    /**
     * @brief AE init function
     * @param [in] a_pAEInitParam AE init input parameters; please refer to ae_param.h
     * @param [out] a_pAEOutput AE algorithm output; please refer to ae_param.h
     * @param [out] a_pAEConfig AE statistics config parameter; please refer to ae_param.h
     */
    virtual MRESULT initAE(AE_INITIAL_INPUT_T *a_pAEInitParam, strAEOutput *a_pAEOutput, AE_STAT_PARAM_T *a_pAEConfig) = 0;
    ////        HTLU:ADD_FACE_FEATURE_START       ////
    /**
     * @brief FaceAE init function
     */
    virtual MRESULT initFaceAE(void) = 0;
    ////        HTLU:ADD_FACE_FEATURE_END       ////
    /**
     * @brief update AE parameter information
     * @param [in] a_pAEInitParam AE input parameters; please refer to ae_param.h
     */
    virtual MRESULT updateAEParam(AE_INITIAL_INPUT_T *a_pAEUpdateParam) = 0;
    /**
     * @brief set AE metering mode for AE algorithm
     * @param [in] i4NewAEMeteringMode metering mode value; please refer to ae_feature.h
     */
    virtual MRESULT setAEMeteringMode(LIB3A_AE_METERING_MODE_T i4NewAEMeteringMode) = 0;
    /**
     * @brief set AE mode for AE algorithm
     * @param [in] a_eAEScene AE scene; please refer to camera_custom_AEPlinetable.h
     */
    virtual MRESULT setAEScene(LIB3A_AE_SCENE_T  a_eAEScene) = 0;
    /**
     * @brief output the AE capture parameter information by the difference EV compensation
     * @param [out] aeoutput AE capture information ouput; please refer to ae_param.h
     * @param [in] iDiffEV difference EV compensation value
     */
    virtual MRESULT switchCapureDiffEVState(strAEOutput *aeoutput, MINT8 iDiffEV) = 0;
    /**
     * @brief AE algorithm main function
     * @param [in] a_Input AE frame-based input parameters; please refer to ae_param.h
     * @param [out] a_Output AE algorithm output; please refer to ae_param.h
     */
    virtual MRESULT handleAE(strAEInput*  a_Input,strAEOutput* a_Output) = 0;

    /**
     * @brief AE TG algorithm main function, for converge speedup
     * @param [in] a_Input AE frame-based input parameters; please refer to ae_param.h
     * @param [out] a_Output AE algorithm output; please refer to ae_param.h
     */
    virtual MBOOL handleInterAE(strAEInput*  a_Input,strAEOutput* a_Output) = 0;
	
    /**
     * @brief set AE ISO speed
     * @param [in] a_eISO ISO speed value; please refer to camera_custom_AEPlinetable.h
     */
    virtual MRESULT setIsoSpeed(MUINT32  a_u4ISO) = 0;
    /**
     * @brief set Anti-banding mode to let AE choose difference AE Pline table
     * @param [in] a_eAEFlickerMode flicker mode; please refer to camera_custom_AEPlinetable.h
     */
    virtual MRESULT setAEFlickerMode(LIB3A_AE_FLICKER_MODE_T a_eAEFlickerMode) = 0;
    /**
     * @brief set flicker detection result mode to let AE choose difference AE Pline table
     * @param [in] a_eAEFlickerAutoMode flicker detection result; please refer to camera_custom_AEPlinetable.h
     */
    virtual MRESULT setAEFlickerAutoMode(LIB3A_AE_FLICKER_AUTO_MODE_T a_eAEFlickerAutoMode) = 0;
    /**
     * @brief output the AE statistic window config for AE algorithm
     * @param [in] a_eZoomWindow update AE algorithm calculate window information; please refer to Ae_param.h
     * @param [out] a_pAEHistConfig AE statistics config parameter; please refer to ae_param.h
     */
    //virtual MRESULT modifyHistogramWinConfig(EZOOM_WINDOW_T a_eZoomWindow, AE_STAT_PARAM_T *a_pAEHistConfig, MUINT32 u4PixelWidth, MUINT32 u4PixelHeight) = 0;
    virtual MRESULT modifyHistogramWinConfig(EZOOM_WINDOW_T a_eZoomWindow, AE_STAT_PARAM_T *a_pAEHistConfig, MUINT32 u4PixelWidth, MUINT32 u4PixelHeight,MUINT32 m_i4AETgValidBlockWidth,MUINT32 m_i4AETgValidBlockHeight) = 0;
    /**
     * @brief set AE meter area window and weight information
     * @param [in] sNewAEMeteringArea meter area; please refer to ae_param.h
     */
    virtual MRESULT setAEMeteringArea(AEMeteringArea_T *sNewAEMeteringArea) = 0;
    /**
     * @brief set AE face detection area and weight information
     * @param [in] sNewAEFDArea face detection information; please refer to ae_param.h
     */
    virtual MRESULT setAEFDArea(AEMeteringArea_T* sNewAEFDArea) = 0;
    /**
     * @brief set AE EV compensation value
     * @param [in] a_eEVComp EV compensation value; please refer to ae_feature.h
     */
    virtual MRESULT setEVCompensate(LIB3A_AE_EVCOMP_T a_eEVComp) = 0;
    /**
     * @brief set AE min / max frame rate value
     * @param [in] a_eAEMinFrameRate minimun frame rate value
     et @param [in] a_eAEMaxFrameRate maximun frame rate value
     */
    virtual MRESULT setAEMinMaxFrameRate(MINT32 a_eAEMinFrameRate, MINT32 a_eAEMaxFrameRate) = 0;
    /**
     * @brief set limiter control for AE algorithm
     * @param [in] bAElimitorEnable enable or disable AE limiter
     */
    virtual MVOID setAElimitorEnable(MBOOL bAElimitorEnable) = 0;
    /**
     * @brief set camera mode for AE algorithm
     * @param [in] a_eAECamMode AE camera mode; please refer to ae_feature.h
     */
    virtual MRESULT setAECamMode(LIB3A_AECAM_MODE_T a_eAECamMode) = 0;
    /**
     * @brief set sensor mode for AE algorithm
     * @param [in] a_i4AESensorMode AE sensor mode;
     */
    virtual MRESULT setAESensorMode(MINT32 a_i4AESensorMode) = 0;
    /**
     * @brief get the information for AE algorithm debug
     * @param [out] a_rAEDebugInfo debug information; please refer to Dbg_ae_param.h
     */
    virtual MRESULT getDebugInfo(AE_DEBUG_INFO_T &a_rAEDebugInfo) = 0;
    /**
     * @brief set video dynamic frame rate enable or disable
     * @param [in] bVdoEnable enable or disable video dynamic frame rate
     */
    virtual MVOID setAEVideoDynamicEnable(MBOOL bVdoEnable) = 0;
    /**
     * @brief set ISO speed is real or not
     * @param [in] bAERealISO using real iso to calculate the sensor gain and isp gain or not
     */
    virtual MRESULT setAERealISOSpeed(MBOOL bAERealISO) = 0;
    /**
     * @brief lock or unlock AE
     * @param [in] bLockAE lock AE (MTRUE) or unlock AE (MFALSE)
     */
    virtual MVOID lockAE(MBOOL bLockAE) = 0;
    /**
     * @brief set video is the record state or not
     * @param [in] bVdoRecord video recording (MTRUE) or video not recording (MFALSE)
     */
    virtual MVOID setAEVideoRecord(MBOOL bVdoRecord) = 0;
    /**
     * @brief get AE capture index and capturePline table information
     * @param [out] i4CapIndex capture index
     * @param [out] a_AEPlineTable current AE Pline table pointer; please refer to camera_custom_AEPlinetable.h
     */
    virtual MRESULT getCapPlineTable(MINT32 *i4CapIndex, strAETable &a_CapAEPlineTable) = 0;
    /**
     * @brief get AE senstivity delta value information
     * @param [in] u4NextSenstivity the brightness different ratio
     */
    virtual MRESULT getSenstivityDeltaIndex(MUINT32 u4NextSenstivity) = 0;
    /**
     * @brief get AE meter area luminance value
     * @param [in] sAEMeteringArea meter area information; please refer to Ae_param.h
     * @param [out] iYvalue luminance value
    */
    virtual MRESULT getAEMeteringAreaValue(AEMeterArea_T sAEMeteringArea, MUINT8 *iYvalue) = 0;
    /**
     * @brief get AE meter block area luminance value
     * @param [in] sAEMeteringArea meter area information; please refer to Ae_param.h
     * @param [out] uYvalue luminance value
     * @param [out] u2YCnt count value
    */
    virtual MRESULT getAEMeteringBlockAreaValue(AEMeterArea_T sAEMeteringArea, MUINT8 *uYvalue, MUINT16 *u2YCnt) = 0;
    /**
     * @brief get AE histogram value
     * @param [in] uHistIndex input histogram index
     * @param [out] pAEHistogram AE histgoram value pointer
    */
    virtual MRESULT getAEHistogram(MUINT8 uHistIndex, MUINT32 *pAEHistogram) = 0;
    /**
     * @brief get AE histogram value
     * @param [out] pAEHistogram AE histgoram value pointer
    */
    virtual MRESULT getAESWHdrInfo(HDR_CHDR_INFO_T &rSWHdrInfo) = 0;
    /**
     * @brief modify the sensor shutter and sensor gain to meet the sensor driver request
     * @param [in] bCaptureTable search table by preview or capture table
     * @param [in] rInputData input sensor shutter, sensor gain, isp gain and ISO speed; please refer to Ae_param.h
     * @param [out] rOutputData output sensor shutter, sensor gain, isp gain and ISO speed; please refer to Ae_param.h
    */
    virtual MRESULT switchSensorExposureGain(MBOOL bCaptureTable, AE_EXP_GAIN_MODIFY_T &rInputData, AE_EXP_GAIN_MODIFY_T &rOutputData) = 0;
    /**
     * @brief get AE information for ISP tuning used
     * @param [out] rAEISPInfo output AE information for ISP tuning; please refer to Ae_param.h
    */
    virtual MRESULT getAEInfoForISP(AE_INFO_T &rAEISPInfo, LIB3A_SENSOR_MODE_T eSensorMode = LIB3A_SENSOR_MODE_PRVIEW) = 0;
    /**
     * @brief set strobe on or off infomation
     * @param [in] bIsStrobeOn strobe on (MTRUE) or strobe off (MFALSE)
     */
    virtual MRESULT setStrobeMode(MBOOL bIsStrobeOn) = 0;
    /**
     * @brief set phone rotate AE weighting or not
     * @param [in] bIsRotateWeighting rotate AE weighting degree (MTRUE) or don't rotate (MFALSE).
     */
    virtual MRESULT setAERotateWeighting(MBOOL bIsRotateWeighting) = 0;
    /**
     * @brief set the AE statistic buffer pinter
     * @param [in] a_pAEBuffer AE statistic buffer pointer
     */
    virtual MVOID setAESatisticBufferAddr(void* a_pAEBuffer, AWB_GAIN_T gains) = 0;
    /**
     * @brief set the AE statistic buffer pinter
     * @param [in] a_pAEBuffer AE statistic buffer pointer
     */
    virtual MVOID setAESatisticBufferAddr_v4p0(void* a_pPSOBuffer ,void* a_pAEBuffer, AAO_PROC_INFO_T  *pAAOProcInfo)  = 0;
    /**
     * @brief set the mVHDR AE statistic buffer pinter
     * @param [in] a_pAEBuffer AE statistic buffer pointer
     */
    virtual MVOID setmVHDRAESatisticBufferAddr(void* a_pAEBuffer, AE_3EXPO_PROC_INFO_T  *pProcInfo)  = 0;    
    /**
     * @brief get the AE blocks y values
     * @param [in] size at most 25
     * @param [out] pYvalues luminance values
     */
    virtual MVOID getAEBlockYvalues(MUINT8 *pYvalues, MUINT8 size) = 0;
    /**
     * @brief get LCE index range information
     * @param [out] u4StartIdx LCE AE start index in the AE Pline table
     * @param [out] u4EndIdx LCE AE end index in the AE Pline table
    */
    virtual MRESULT getAELCEIndexInfo(MUINT32 *u4StartIdx, MUINT32 *u4EndIdx) = 0;
    /**
     * @brief capture dynamic flare calculate
     * @param [in] a_pAEBuffer AE statistic buffer pointer
     * @param [in] bWithStrobe On or OFF strobe
     */
    virtual MUINT32    CalculateCaptureFlare( void* a_pAEBuffer,MBOOL bWithStrobe  )=0;
    /**
     * @brief preview dynamic flare calculate
     * @param [in] a_pAEBuffer AE statistic buffer pointer
     */
    virtual MVOID DoPreFlare(void* a_pAEBuffer)=0;
    /**
     * @brief set preview flare value
     * @param [in] nPreFlare preview flare value in 12 bit domain
     */
    virtual void  SetPreviewFlareValue(MINT32 nPreFlare)=0;
    /**
     * @brief set capture flare value
     * @param [in] nPreFlare preview flare value in 12 bit domain
     */
    virtual void  SetCaptureFlareValue(MINT32 nCapFlare)=0;
    /**
     * @brief get brightness value in the preview mode
     * @return the brightness value
     */
    virtual MINT32 getBrightnessAverageValue(void) = 0;
    /**
     * @brief get brightness Center Weighted value in the preview mode
     * @return the brightness Center Weighted value
     */
     virtual MINT32 getBrightnessCenterWeightedValue(void) = 0;
    /**
     * @brief get capture luminance value
     * @param [out] i4CapLV capture luminance value
     */
    virtual MRESULT CalculateCaptureLV(MINT32 *i4CapLV) = 0;
    /**
     * @brief get flare offset value
     * @param [out] a_FlareOffsetCali flare offset calculate
     */
    virtual MUINT32  CalculateFlareOffset(MUINT32 a_FlareOffsetCali) = 0;
    /**
     * @brief set AE target value by calibration
     * @param [in] u4AETargetValue AE target valie value in 8 bit domain
     */
    virtual MRESULT setAETargetValue(MUINT32 u4AETargetValue) = 0;
    /**
     * @brief update the AE next inde for preview
     * @param [in] i4AEIndex preview next index value
     */
    virtual MRESULT updateAEIndex(MINT32 i4AEIndex,MINT32 i4AEIndexF) = 0;
    /**
     * @brief get the AE current table max ISO
     * @param [out] u4MaxISO output max ISO of AE Pline table
     * @param [out] u4MaxShutter output max shutter of AE Pline table
     */
    virtual MRESULT getAEMaxISO(MUINT32 &u4MaxShutter, MUINT32 &u4MaxISO) = 0;
    /**
     * @brief set HDR AE info
     * @param [in] i4Ratio is the ratio between SE & LE
     * @param [in] i4Segment is the segment between SE & LE
     */
    virtual MVOID setHdrAEInfo(rVHDRExpSettingInfo_T rHDRDataIn) = 0;
     /**
     * @brief update AE object tracking statue
     * @param [in] bAEOTenable enable/disable object tracking
     */
    virtual MRESULT setAEObjectTracking(MBOOL bAEOTenable) = 0;
     /**
     * @brief update AE tuning parameters
     * @param [in] AE tuning file
     */
    virtual MRESULT updateAETuningParam(AE_PARAM_T *a_pAEUpdateTuningParam) = 0;
    /**
     * @brief Save AE algorithm information
     * @param [in] file name
     */
    virtual MVOID SaveAeCoreInfo(const char *) = 0;

    /**
     * @brief update AE init index for preview
     * @param [in] new AE index
     */
    virtual MRESULT updateAEInitIndex(MINT32 i4LV9Offset) = 0;
    /**
     * @brief update AE pline pointer
     * @param [in] AE tuning file
     */
    virtual MRESULT updateAEPlineInfo_v4p0(strAETable *pCurrentAEPlineTable, strAETable *pCurrentCaptureAEPlineTable,strFinerEvPline *m_pCurrentTableF,strFinerEvPline *m_pCurrentCaptureTableF,strAEPLineGainList *AEGainList,AE_NVRAM_T* pAENVRAM, AE_PARAM_T* pAeTuningParam,MUINT32 &u4FinerEVIndex) = 0;
    /**
     * @brief get the AE current EV Index info
     * @param [out] Max EV idx
     * @param [out] Min EV idx
     * @param [out] Current EV idx
     */
    virtual MRESULT getEVIdxInfo_v4p0(MUINT32 &u4MaxEVidx, MUINT32 &u4MinEVidx, MUINT32 &u4CurrentEVidx,MUINT32 &u4MaxEVidxF, MUINT32 &u4MinEVidxF, MUINT32 &u4CurrentEVidxF) = 0;
    /**
     * @brief set the AE current EV Index info
     * @param [in] Max EV idx
     * @param [in] Min EV idx
     * @param [in] Current EV idx
     */
    virtual MRESULT setEVIdxInfo_v4p0(MUINT32 u4MaxEVidx, MUINT32 u4MinEVidx, MUINT32 u4CurrentEVidx,MUINT32 u4MaxEVidxF, MUINT32 u4MinEVidxF, MUINT32 u4CurrentEVidxF) = 0;
    /**
     * @brief update AE pline pointer
     * @param [in] AE tuning file
     */
    virtual MRESULT updateAEPlineInfo(strAETable *pCurrentAEPlineTable, strAETable *pCurrentCaptureAEPlineTable) = 0;
    /**
     * @brief get the AE current EV Index info
     * @param [out] Max EV idx
     * @param [out] Min EV idx
     * @param [out] Current EV idx
     */
    virtual MRESULT getEVIdxInfo(MUINT32 &u4MaxEVidx, MUINT32 &u4MinEVidx, MUINT32 &u4CurrentEVidx) = 0;
    /**
     * @brief set the AE current EV Index info
     * @param [in] Max EV idx
     * @param [in] Min EV idx
     * @param [in] Current EV idx
     */
    virtual MRESULT setEVIdxInfo(MUINT32 u4MaxEVidx, MUINT32 u4MinEVidx, MUINT32 u4CurrentEVidx) = 0;
    /**
     * @brief set the AE max BV and sensivity difference as sensor mode is changed
     * @param [in] sensitivity delta idx
     * @param [in] Max BV delta idx
     */
    virtual MRESULT setSensorModeMaxBVdiff(MINT32 i4SenstivityDeltaIdx ,MINT32 i4BVDeltaIdx) = 0;
    /**
     * @brief get flare parameters
     * @param [out] FlareOffset & FlareGain
     */
    virtual MRESULT getPreviewFlare(MINT16 *i2FlareOffset,MINT16 *i2FlareGain) = 0;
    /**
     * @brief get touch flag to know whether touch bounding box is used by AE algo
     * @param [out] touch flag
     */
    virtual MRESULT getTouchIsReferencedFlag(MBOOL &bTouchIsReferencedFlag) = 0;
    /**
     * @brief get face flag to know whether face bounding box is used by AE algo
     * @param [out] face flag
     */    
    virtual MRESULT getFaceIsReferencedFlag(MBOOL &bFaceIsReferencedFlag) = 0;
    /**
     * @brief calculate the SE info with given LE
     * @param [out] SE info
     */
     virtual MVOID calRatio2AEInfoIf(strEvSetting rEvSetting) = 0;
     /**
     * @brief get HDR info. from algo.
     * @param [out] HDR info.
     */
     virtual MVOID updateHdrInfo(strEvHdrSetting &rHdrEvSetting) = 0;
     /**
     * @brief set HDR ratio
     * @param [out]
     */
     virtual MVOID setHdrRatio(MUINT32 u4HdrRatioM) = 0;
     /**
     * @brief set Zoom ratio
     * @param [out] zoom ratio
     */
     virtual MRESULT setZoomRatio(MUINT32 u4ZoomRatio) = 0;
     /*
     * @brief get preview flare value from ccu
     * @param [out] get flare value
     */
     virtual MRESULT setFlareInfo(MUINT32 u4Prvflare) = 0;
};

}; // namespace NS3A

#endif

