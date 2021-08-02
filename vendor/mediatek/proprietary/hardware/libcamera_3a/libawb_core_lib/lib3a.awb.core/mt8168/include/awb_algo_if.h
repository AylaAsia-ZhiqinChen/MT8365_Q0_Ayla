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
 * @file awb_algo_if.h
 * @brief Interface to AWB algorithm library
 */

#ifndef _AWB_ALGO_IF_H_
#define _AWB_ALGO_IF_H_



#include "aaa_algo_option.h"


#define AWB_ALGO_IF_REVISION    6511001
namespace NS3A
{

/**
 * @brief Interface to AWB algorithm library
 */
class IAwbAlgo {

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:  //    Ctor/Dtor.
    IAwbAlgo() {}
    virtual ~IAwbAlgo() {}

private: // disable copy constructor and copy assignment operator
    IAwbAlgo(const IAwbAlgo&);
    IAwbAlgo& operator=(const IAwbAlgo&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interfaces.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    /**
     * @brief create instance
     * @param [in] eAWBOpt AWB option
     * @param [in] i4SensorDev Sensor device
     */
    template <EAAAOpt_T const eAAAOpt>
    static  IAwbAlgo* createInstance(MINT32 const i4SensorDev);
    /**
     * @brief destory instance
     */
    virtual MVOID   destroyInstance() = 0;

    /**
     * @brief AWB init function
     * @param [in] a_rAWBInitInput AWB init input parameters; please refer to awb_param.h
     * @param [out] a_rAWBOutput AWB algorithm output; please refer to awb_param.h
     */
    virtual MRESULT initAWB(AWB_INIT_INPUT_T &a_rAWBInitInput,
                            AWB_OUTPUT_T &a_rAWBOutput,
                            AWB_GAIN_T &a_rAWBRAWPreGain1) = 0;

    /**
     * @brief set AWB statistics config parameter
     * @param [in] a_rAWBStatConfig AWB statistics config parameter; please refer to awb_param.h
     * @param [in] a_i4SensorWidth sensor width
     * @param [in] a_i4SensorHeight sensor height
     */
    virtual MRESULT setAWBStatConfig(const AWB_STAT_CONFIG_T &a_rAWBStatConfig, MINT32 a_i4SensorWidth, MINT32 a_i4SensorHeight) = 0;

    /**
     * @brief set AWB mode
     * @param [in] a_eAWBMode AWB mode; please refer to awb_feature.h
     */
    virtual MRESULT setAWBMode(LIB3A_AWB_MODE_T a_eAWBMode) = 0;
    
    /**
     * @brief set AWB face detection area and weight information
     * @param [in] sNewAWBFDArea face detection information; please refer to awb_param.h
     */
    virtual MRESULT setAWBFDArea(AWBMeteringArea_T* sNewAWBFDArea) = 0;

    /**
     * @brief get MWB light area
     * @param [in] i4CCT manual color temprature
     * @param [in] rLightArea manual color temprature light area
     */
    virtual MRESULT getMWBLightArea(MINT32 i4CCT, LIGHT_AREA_T &rLightArea) = 0;
    
    /**
     * @brief AWB algorithm main function
     * @param [in] a_rAWBInput AWB frame-based input parameters; please refer to awb_param.h
     * @param [out] a_rAWBOutput AWB algorithm output; please refer to awb_param.h
     */
    virtual MRESULT handleAWB(AWB_INPUT_T &a_rAWBInput, AWB_OUTPUT_T &a_rAWBOutput) = 0;

    /**
     * @brief get scene LV
     * @return scene LV
     */
    virtual MINT32 getSceneLV() = 0;

    /**
     * @brief get correlated color temperature
     * @return correlated color temperature
     */
    virtual MINT32 getCCT() = 0;

    /**
     * @brief get ASD info
     * @param [out] a_rAWBASDInfo ASD info; please refer to awb_param.h
     */
    virtual MRESULT getASDInfo(AWB_ASD_INFO_T &a_rAWBASDInfo) = 0;

    /**
     * @brief get parant block info
     * @param [out] AWB_PARENT_BLK_STAT_T parant block info; please refer to awb_param.h
     */
    virtual MRESULT getAWBParentStat(AWB_PARENT_BLK_STAT_T &a_rAWBParentState,  MINT32 *a_ParentBlkNumX, MINT32 *a_ParentBlkNumY) = 0;

    /**
     * @brief get AWB infomation for customization
     * @param [out] AWB_CUST_INFO_T AWB output info; please refer to awb_param.h
     */
    virtual MRESULT getAWBOutInfo(AWB_CUST_INFO_OUT_T &a_rAWBOutInfo) = 0;

    /**
     * @brief get AWB infomation for customization
     * @param [out] AWB_CUST_INFO_T light state info; please refer to awb_param.h
     */
    virtual MRESULT getAWBInfo(AWB_CUST_INFO_T &a_rAWBInfo) = 0;

    /**
     * @brief get AWB frame infomation for customization
     * @param [out] AWB_CUST_INFO_FRAME_T frame info; please refer to awb_param.h
     */
    virtual MRESULT getAWBFrameInfo(AWB_CUST_INFO_FRAME_T &a_rAWBInfo) = 0;

    /**
     * @brief set AWB customization gain
     * @param [out] AWB_CUST_INFO_T light state info; please refer to awb_param.h
     */
    virtual MRESULT updateAWBCustGain(const AWB_GAIN_T &a_rAWBGain, AWB_OUTPUT_T &a_rAWBOutput) = 0;

    /**
     * @brief get light source probability; for CCT use only
     * @param [out] a_rAWBLightProb light source probability; please refer to awb_param.h
     */
    virtual MRESULT getLightProb(AWB_LIGHT_PROBABILITY_T &a_rAWBLightProb) = 0;

    /**
     * @brief get EXIF debug info
     * @param [out] a_rAWBDebugInfo AWB debug info; please refer to dbg_awb_param.h
     * @param [out] a_rAWBDebugData AWB debug data; please refer to dbg_awb_param.h
     */
    virtual MRESULT getDebugInfo(AWB_DEBUG_INFO_T &a_rAWBDebugInfo, AWB_DEBUG_DATA_T &a_rAWBDebugData) = 0;

    /**
     * @brief update AWB parameter; for CCT use only
     * @param [in] a_rAWBInitInput AWB init input parameters; please refer to awb_param.h
     * @param [out] a_rAWBStatConfig AWB statistics config parameters; please refer to awb_param.h
     */
    virtual MRESULT updateAWBParam(AWB_INIT_INPUT_T &a_rAWBInitInput) = 0;
    
    /**
     * @brief get AWB algorithm version info
     */
    virtual MRESULT getVersionInfo() = 0;
    
    virtual MVOID Flash_Awb_calResultGain(FLASH_AWB_CAL_GAIN_INPUT_T& calGainInput, FLASH_AWB_OUTPUT_T& flashAwbOuput) = 0;

    virtual MVOID SaveAwbCoreInfo(const char *){};
};

}; // namespace NS3A

#endif

