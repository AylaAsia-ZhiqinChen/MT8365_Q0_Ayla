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
#define LOG_TAG "isp_mgr_ggm"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (0)
#endif

#include <cutils/properties.h>
#include <aaa_types.h>
#include <aaa_error_code.h>
#include <mtkcam/utils/std/Log.h>
#include "isp_mgr.h"
#include <private/aaa_utils.h>
#include <array>

#define MY_INST_GGM NS3Av3::INST_T<NSIspTuningv3::ISP_MGR_GGM_T>
static std::array<MY_INST_GGM, SENSOR_IDX_MAX> gMultitonGGM;

#define MY_INST_GGM2 NS3Av3::INST_T<NSIspTuningv3::ISP_MGR_GGM2_T>
static std::array<MY_INST_GGM2, SENSOR_IDX_MAX> gMultitonGGM2;

namespace NSIspTuningv3
{

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// GGM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ISP_MGR_GGM_T&
ISP_MGR_GGM_T::
getInstance(ESensorDev_T const eSensorDev)
{
    auto i4SensorIdx = NS3Av3::mapSensorDevToIdx(eSensorDev);

    if(i4SensorIdx >= SENSOR_IDX_MAX || i4SensorIdx < 0) {
        MY_LOGE("[%s] Unsupport sensor Index: %d\n", __FUNCTION__, i4SensorIdx);
    }

    MY_INST_GGM& rSingleton = gMultitonGGM[i4SensorIdx];
    std::call_once(rSingleton.onceFlag, [&] {
        rSingleton.instance = std::make_unique<ISP_MGR_GGM_T>(eSensorDev);
    } );

    return *(rSingleton.instance);
}

template <>
ISP_MGR_GGM_T&
ISP_MGR_GGM_T::
put(ISP_NVRAM_GGM_T const& rParam)
{
    for (MINT32 i = 0; i < GGM_LUT_SIZE; i++) {
        m_rIspRegInfo_GGM[i].val = rParam.lut.set[i];
        //CAM_LOGD("m_rIspRegInfo_GGM_RB[%d].val = 0x%8x", i, m_rIspRegInfo_GGM_RB[i].val);
        //CAM_LOGD("m_rIspRegInfo_GGM_G[%d].val = 0x%8x", i, m_rIspRegInfo_GGM_G[i].val);
    }

    return  (*this);
}

template <>
ISP_MGR_GGM_T&
ISP_MGR_GGM_T::
get(ISP_NVRAM_GGM_T& rParam)
{
    for (MINT32 i = 0; i < GGM_LUT_SIZE; i++) {
        rParam.lut.set[i] = m_rIspRegInfo_GGM[i].val;
//        rParam.lut_g.set[i] = m_rIspRegInfo_GGM_G[i].val;
    }

    return  (*this);
}

template <>
MBOOL
ISP_MGR_GGM_T::
get(ISP_NVRAM_GGM_T& rParam, const dip_x_reg_t* pReg) const
{
    if (pReg)
    {
        for (MINT32 i = 0; i < GGM_LUT_SIZE; i++) {
            rParam.lut.set[i] = pReg->DIP_X_GGM_LUT[i].Raw;
            //rParam.lut_g.set[i] = pReg->DIP_X_GGM_LUT_G[i].Raw;
        }
    }
    return MTRUE;
}

MBOOL
ISP_MGR_GGM_T::
apply(EIspProfile_T /*eIspProfile*/, dip_x_reg_t* pReg)
{
    // TOP ==> don't care
    ISP_WRITE_ENABLE_BITS(pReg, DIP_X_CTL_RGB_EN, GGM_EN, 1);
    //ISP_MGR_CTL_EN_P2_T::getInstance(m_eSensorDev).setEnable_GGM(MTRUE);

    MBOOL bLNR_EN = isLNREnable();

    // LNR, linear mapping for debug
    reinterpret_cast<ISP_DIP_X_GGM_CTRL_T*>(REG_INFO_VALUE_PTR(DIP_X_GGM_CTRL))->GGM_LNR = bLNR_EN;
    reinterpret_cast<ISP_DIP_X_GGM_CTRL_T*>(REG_INFO_VALUE_PTR(DIP_X_GGM_CTRL))->GGM_END_VAR = 1023;
    reinterpret_cast<ISP_DIP_X_GGM_CTRL_T*>(REG_INFO_VALUE_PTR(DIP_X_GGM_CTRL))->GGM_RMP_VAR = 4099;
    writeRegs(static_cast<RegInfo_T*>(m_rIspRegInfo), m_u4RegInfoNum, pReg);

    // RB table
    writeRegs(static_cast<RegInfo_T*>(m_rIspRegInfo_GGM), GGM_LUT_SIZE, pReg);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// GGM2
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ISP_MGR_GGM2_T&
ISP_MGR_GGM2_T::
getInstance(ESensorDev_T const eSensorDev)
{
    auto i4SensorIdx = NS3Av3::mapSensorDevToIdx(eSensorDev);

    if(i4SensorIdx >= SENSOR_IDX_MAX || i4SensorIdx < 0) {
        MY_LOGE("[%s] Unsupport sensor Index: %d\n", __FUNCTION__, i4SensorIdx);
    }

    MY_INST_GGM2& rSingleton = gMultitonGGM2[i4SensorIdx];
    std::call_once(rSingleton.onceFlag, [&] {
        rSingleton.instance = std::make_unique<ISP_MGR_GGM2_T>(eSensorDev);
    } );

    return *(rSingleton.instance);
}

ISP_MGR_GGM2_T&
ISP_MGR_GGM2_T::
put(ISP_NVRAM_GGM_T const& rParam)
{
    for (MINT32 i = 0; i < GGM_LUT_SIZE; i++) {
        m_rIspRegInfo_GGM2[i].val = rParam.lut.set[i];
    }
    return  (*this);
}

ISP_MGR_GGM2_T&
ISP_MGR_GGM2_T::
get(ISP_NVRAM_GGM_T& rParam)
{
    for (MINT32 i = 0; i < GGM_LUT_SIZE; i++) {
        rParam.lut.set[i] = m_rIspRegInfo_GGM2[i].val;
    }

    return  (*this);
}

MBOOL
ISP_MGR_GGM2_T::
get(ISP_NVRAM_GGM_T& rParam, const dip_x_reg_t* pReg) const
{
    if (pReg)
    {
        for (MINT32 i = 0; i < GGM_LUT_SIZE; i++) {
            rParam.lut.set[i] = pReg->DIP_X_GGM2_LUT[i].Raw;
        }
    }
    return MTRUE;
}

MBOOL
ISP_MGR_GGM2_T::
apply(EIspProfile_T /*eIspProfile*/, dip_x_reg_t* pReg)
{
    // TOP ==> don't care
    ISP_WRITE_ENABLE_BITS(pReg, DIP_X_CTL_RGB_EN, GGM2_EN, 1);
    //ISP_MGR_CTL_EN_P2_T::getInstance(m_eSensorDev).setEnable_GGM(MTRUE);

    MBOOL bLNR_EN = isLNREnable();

    // LNR, linear mapping for debug
    reinterpret_cast<ISP_DIP_X_GGM2_CTRL_T*>(REG_INFO_VALUE_PTR(DIP_X_GGM2_CTRL))->GGM_LNR = bLNR_EN;
    reinterpret_cast<ISP_DIP_X_GGM2_CTRL_T*>(REG_INFO_VALUE_PTR(DIP_X_GGM2_CTRL))->GGM_END_VAR = 1023;
    reinterpret_cast<ISP_DIP_X_GGM2_CTRL_T*>(REG_INFO_VALUE_PTR(DIP_X_GGM2_CTRL))->GGM_RMP_VAR = 4099;
    writeRegs(static_cast<RegInfo_T*>(m_rIspRegInfo), m_u4RegInfoNum, pReg);

    // RB table
    writeRegs(static_cast<RegInfo_T*>(m_rIspRegInfo_GGM2), GGM_LUT_SIZE, pReg);

    return MTRUE;
}
}
