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
#define LOG_TAG "isp_mgr_bnr"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (0)
#endif

#include <cutils/properties.h>
#include <aaa_types.h>
#include <aaa_error_code.h>
#include <mtkcam/utils/std/Log.h>
#include "isp_mgr.h"
#include <string>
#include <mtkcam/utils/imgbuf/IImageBuffer.h>
#include <mtkcam/utils/imgbuf/IIonImageBufferHeap.h>

#include "private/PDTblGen.h"

#include <private/aaa_utils.h>
#include <array>

using namespace NSCam;
using namespace NSIspTuningv3;

#define MY_INST_BNR NS3Av3::INST_T<ISP_MGR_BNR_T>
static std::array<MY_INST_BNR, SENSOR_IDX_MAX> gMultitonBNR;

#define MY_INST_BNR2 NS3Av3::INST_T<ISP_MGR_BNR2_T>
static std::array<MY_INST_BNR2, SENSOR_IDX_MAX> gMultitonBNR2;

namespace NSIspTuningv3
{

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// BNR
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ISP_MGR_BNR_T&
ISP_MGR_BNR_T::
getInstance(ESensorDev_T const eSensorDev)
{
    auto i4SensorIdx = NS3Av3::mapSensorDevToIdx(eSensorDev);
    if(i4SensorIdx >= SENSOR_IDX_MAX || i4SensorIdx < 0) {
        MY_LOGE("[%s] Unsupport sensor Index: %d\n", __FUNCTION__, i4SensorIdx);
    }

    MY_INST_BNR& rSingleton = gMultitonBNR[i4SensorIdx];
    std::call_once(rSingleton.onceFlag, [&] {
        rSingleton.instance = std::make_unique<ISP_MGR_BNR_T>(eSensorDev);
    } );

    return *(rSingleton.instance);
}

template <>
ISP_MGR_BNR_T&
ISP_MGR_BNR_T::
put(ISP_NVRAM_BNR_BPC_T const& rParam)
{
    PUT_REG_INFO(CAM_BNR_BPC_CON, con);
    PUT_REG_INFO(CAM_BNR_BPC_TH1, th1);
    PUT_REG_INFO(CAM_BNR_BPC_TH2, th2);
    PUT_REG_INFO(CAM_BNR_BPC_TH3, th3);
    PUT_REG_INFO(CAM_BNR_BPC_TH4, th4);
    PUT_REG_INFO(CAM_BNR_BPC_DTC, dtc);
    PUT_REG_INFO(CAM_BNR_BPC_COR, cor);
    //PUT_REG_INFO(CAM_BNR_BPC_TBLI1, tbli1);
    PUT_REG_INFO(CAM_BNR_BPC_TH1_C, th1_c);
    PUT_REG_INFO(CAM_BNR_BPC_TH2_C, th2_c);
    PUT_REG_INFO(CAM_BNR_BPC_TH3_C, th3_c);
    return  (*this);
}


template <>
ISP_MGR_BNR_T&
ISP_MGR_BNR_T::
get(ISP_NVRAM_BNR_BPC_T& rParam)
{
    GET_REG_INFO(CAM_BNR_BPC_CON, con);
    GET_REG_INFO(CAM_BNR_BPC_TH1, th1);
    GET_REG_INFO(CAM_BNR_BPC_TH2, th2);
    GET_REG_INFO(CAM_BNR_BPC_TH3, th3);
    GET_REG_INFO(CAM_BNR_BPC_TH4, th4);
    GET_REG_INFO(CAM_BNR_BPC_DTC, dtc);
    GET_REG_INFO(CAM_BNR_BPC_COR, cor);
    //GET_REG_INFO(CAM_BNR_BPC_TBLI1, tbli1);
    GET_REG_INFO(CAM_BNR_BPC_TH1_C, th1_c);
    GET_REG_INFO(CAM_BNR_BPC_TH2_C, th2_c);
    GET_REG_INFO(CAM_BNR_BPC_TH3_C, th3_c);

    return  (*this);
}


template <>
ISP_MGR_BNR_T&
ISP_MGR_BNR_T::
put(ISP_NVRAM_BNR_NR1_T const& rParam)
{
    PUT_REG_INFO(CAM_BNR_NR1_CON, con);
    PUT_REG_INFO(CAM_BNR_NR1_CT_CON, ct_con);
    PUT_REG_INFO(CAM_BNR_NR1_CT_CON2, ct_con2);
    PUT_REG_INFO(CAM_BNR_NR1_CT_CON3, ct_con3);

    return  (*this);
}


template <>
ISP_MGR_BNR_T&
ISP_MGR_BNR_T::
get(ISP_NVRAM_BNR_NR1_T& rParam)
{
    GET_REG_INFO(CAM_BNR_NR1_CON, con);
    GET_REG_INFO(CAM_BNR_NR1_CT_CON, ct_con);
    GET_REG_INFO(CAM_BNR_NR1_CT_CON2, ct_con2);
    GET_REG_INFO(CAM_BNR_NR1_CT_CON3, ct_con3);

    return  (*this);
}

template <>
ISP_MGR_BNR_T&
ISP_MGR_BNR_T::
put(ISP_NVRAM_BNR_PDC_T const& rParam)
{
    PUT_REG_INFO(CAM_BNR_PDC_CON, con);
    PUT_REG_INFO(CAM_BNR_PDC_GAIN_L0, gain_l0);
    PUT_REG_INFO(CAM_BNR_PDC_GAIN_L1, gain_l1);
    PUT_REG_INFO(CAM_BNR_PDC_GAIN_L2, gain_l2);
    PUT_REG_INFO(CAM_BNR_PDC_GAIN_L3, gain_l3);
    PUT_REG_INFO(CAM_BNR_PDC_GAIN_L4, gain_l4);
    PUT_REG_INFO(CAM_BNR_PDC_GAIN_R0, gain_r0);
    PUT_REG_INFO(CAM_BNR_PDC_GAIN_R1, gain_r1);
    PUT_REG_INFO(CAM_BNR_PDC_GAIN_R2, gain_r2);
    PUT_REG_INFO(CAM_BNR_PDC_GAIN_R3, gain_r3);
    PUT_REG_INFO(CAM_BNR_PDC_GAIN_R4, gain_r4);
    PUT_REG_INFO(CAM_BNR_PDC_TH_GB, th_gb);
    PUT_REG_INFO(CAM_BNR_PDC_TH_IA, th_ia);
    PUT_REG_INFO(CAM_BNR_PDC_TH_HD, th_hd);
    PUT_REG_INFO(CAM_BNR_PDC_SL, sl);
    //PUT_REG_INFO(CAM_BNR_PDC_POS, pos);

    return  (*this);
}


template <>
ISP_MGR_BNR_T&
ISP_MGR_BNR_T::
get(ISP_NVRAM_BNR_PDC_T& rParam)
{
    GET_REG_INFO(CAM_BNR_PDC_CON, con);
    GET_REG_INFO(CAM_BNR_PDC_GAIN_L0, gain_l0);
    GET_REG_INFO(CAM_BNR_PDC_GAIN_L1, gain_l1);
    GET_REG_INFO(CAM_BNR_PDC_GAIN_L2, gain_l2);
    GET_REG_INFO(CAM_BNR_PDC_GAIN_L3, gain_l3);
    GET_REG_INFO(CAM_BNR_PDC_GAIN_L4, gain_l4);
    GET_REG_INFO(CAM_BNR_PDC_GAIN_R0, gain_r0);
    GET_REG_INFO(CAM_BNR_PDC_GAIN_R1, gain_r1);
    GET_REG_INFO(CAM_BNR_PDC_GAIN_R2, gain_r2);
    GET_REG_INFO(CAM_BNR_PDC_GAIN_R3, gain_r3);
    GET_REG_INFO(CAM_BNR_PDC_GAIN_R4, gain_r4);
    GET_REG_INFO(CAM_BNR_PDC_TH_GB, th_gb);
    GET_REG_INFO(CAM_BNR_PDC_TH_IA, th_ia);
    GET_REG_INFO(CAM_BNR_PDC_TH_HD, th_hd);
    GET_REG_INFO(CAM_BNR_PDC_SL, sl);
    //GET_REG_INFO(CAM_BNR_PDC_POS, pos);

    return  (*this);
}

MBOOL
ISP_MGR_BNR_T::
apply(RAWIspCamInfo& rRawIspCamInfo, TuningMgr& rTuning, MINT32 i4SubsampleIdex)
{
    ISP_MGR_BNR2_T& rBnr2 = ISP_MGR_BNR2_T::getInstance(m_eSensorDev);

    if( rBnr2.getDMGItable_Default() == NULL ){
        // must after 3A start
        if(!rBnr2.createBPCI_Default_Buf()){
            CAM_LOGE("BPCI Table Create Fail");
        }

    }

    MBOOL bBPC_EN = (isBPCEnable() & (reinterpret_cast<ISP_CAM_BNR_BPC_CON_T*>(REG_INFO_VALUE_PTR(CAM_BNR_BPC_CON))->BPC_EN));
    MBOOL bCT_EN  = (isCTEnable()  & (reinterpret_cast<ISP_CAM_BNR_NR1_CON_T*>(REG_INFO_VALUE_PTR(CAM_BNR_NR1_CON))->NR1_CT_EN));
    MBOOL bPDC_EN = (isPDCEnable()  & (reinterpret_cast<ISP_CAM_BNR_PDC_CON_T*>(REG_INFO_VALUE_PTR(CAM_BNR_PDC_CON))->PDC_EN));

    if(bPDC_EN){
        //if(m_ePrvSensorMode != rRawIspCamInfo.rMapping_Info.eSensorMode){
            Tbl P1BPCITbl_Info;

            m_bPDC_TBL_Valid = IPDTblGen::getInstance()->getTbl( m_eSensorDev, rRawIspCamInfo.rMapping_Info.eSensorMode, P1BPCITbl_Info);

            if (m_bPDC_TBL_Valid){

                if(rRawIspCamInfo.rMapping_Info.eFrontBin == EFrontBin_Yes){
                    m_sPDOHWInfo.i4memID         = P1BPCITbl_Info.tbl_bin.memID;
                    m_sPDOHWInfo.u4Bpci_xsz      = P1BPCITbl_Info.tbl_bin.tbl_xsz;
                    m_sPDOHWInfo.u4Bpci_ysz      = P1BPCITbl_Info.tbl_bin.tbl_ysz;
                    m_sPDOHWInfo.phyAddrBpci_tbl = (MUINTPTR)P1BPCITbl_Info.tbl_bin.tbl_pa;
                    m_sPDOHWInfo.virAddrBpci_tbl = (MUINTPTR)P1BPCITbl_Info.tbl_bin.tbl_va;
                }
                else{
                    m_sPDOHWInfo.i4memID         = P1BPCITbl_Info.tbl.memID;
                    m_sPDOHWInfo.u4Bpci_xsz      = P1BPCITbl_Info.tbl.tbl_xsz;
                    m_sPDOHWInfo.u4Bpci_ysz      = P1BPCITbl_Info.tbl.tbl_ysz;
                    m_sPDOHWInfo.phyAddrBpci_tbl = (MUINTPTR)P1BPCITbl_Info.tbl.tbl_pa;
                    m_sPDOHWInfo.virAddrBpci_tbl = (MUINTPTR)P1BPCITbl_Info.tbl.tbl_va;
                }
            }

            //m_ePrvSensorMode = rRawIspCamInfo.rMapping_Info.eSensorMode;
        //}
    }

    bPDC_EN = (bPDC_EN && m_bPDC_TBL_Valid);

    if(!bPDC_EN){
        if(rBnr2.getDMGItable_Default()){
            rBnr2.get_BPCI_Info_Default(m_sPDOHWInfo.u4Bpci_xsz,
                                        m_sPDOHWInfo.u4Bpci_ysz,
                                        m_sPDOHWInfo.phyAddrBpci_tbl,
                                        m_sPDOHWInfo.virAddrBpci_tbl,
                                        m_sPDOHWInfo.i4memID);
        }
        else{
            CAM_LOGE("BPCI_Default Buffer Error");
        }
    }

    MBOOL bBNR_EN = (bBPC_EN | bCT_EN | bPDC_EN);

    setBPCEnable(bBPC_EN);
    setCTEnable(bCT_EN);
    setPDCEnable(bPDC_EN);

    //BNR HW limitation
    if (bBNR_EN && (bBPC_EN == 0))
    {
        bBNR_EN = 0;
        bCT_EN = 0;
        MY_LOG("BNR::apply warning: set (bBNR_EN, bBPC_EN, bCT_EN) = (%d, %d, %d)", bBNR_EN, bBPC_EN, bCT_EN);
    }

    reinterpret_cast<ISP_CAM_BNR_BPC_CON_T*>(REG_INFO_VALUE_PTR(CAM_BNR_BPC_CON))->BPC_EN    = bBPC_EN;
    // BPCI SW limitation
    reinterpret_cast<ISP_CAM_BNR_BPC_CON_T*>(REG_INFO_VALUE_PTR(CAM_BNR_BPC_CON))->BPC_LUT_EN = bBPC_EN;

    reinterpret_cast<ISP_CAM_BNR_NR1_CON_T*>(REG_INFO_VALUE_PTR(CAM_BNR_NR1_CON))->NR1_CT_EN = bCT_EN;
    reinterpret_cast<ISP_CAM_BNR_PDC_CON_T*>(REG_INFO_VALUE_PTR(CAM_BNR_PDC_CON))->PDC_EN    = bPDC_EN;

    reinterpret_cast<ISP_CAM_BNR_PDC_CON_T*>(REG_INFO_VALUE_PTR(CAM_BNR_PDC_CON))->PDC_OUT = 0;

    // BPCI size setting
    reinterpret_cast<ISP_CAM_BNR_BPC_TBLI2_T*>(REG_INFO_VALUE_PTR(CAM_BNR_BPC_TBLI2))->BPC_XSIZE = rRawIspCamInfo.RawSize.u4RAW_Width;
    reinterpret_cast<ISP_CAM_BNR_BPC_TBLI2_T*>(REG_INFO_VALUE_PTR(CAM_BNR_BPC_TBLI2))->BPC_YSIZE = rRawIspCamInfo.RawSize.u4RAW_Height;

    reinterpret_cast<ISP_CAM_BNR_PDC_GAIN_L0_T*>(REG_INFO_VALUE_PTR(CAM_BNR_PDC_GAIN_L0))->PDC_GCF_L00 = 256;
    reinterpret_cast<ISP_CAM_BNR_PDC_GAIN_R0_T*>(REG_INFO_VALUE_PTR(CAM_BNR_PDC_GAIN_R0))->PDC_GCF_R00 = 256;
    reinterpret_cast<ISP_CAM_BNR_PDC_TH_GB_T*>(REG_INFO_VALUE_PTR(CAM_BNR_PDC_TH_GB))->PDC_GTH = 4095;
    reinterpret_cast<ISP_CAM_BNR_PDC_TH_IA_T*>(REG_INFO_VALUE_PTR(CAM_BNR_PDC_TH_IA))->PDC_ATH = 4094;

    REG_INFO_VALUE(CAM_BPCI_BASE_ADDR) = m_sPDOHWInfo.phyAddrBpci_tbl;
    REG_INFO_VALUE(CAM_BPCI_XSIZE)     = m_sPDOHWInfo.u4Bpci_xsz+1;
    REG_INFO_VALUE(CAM_BPCI_YSIZE)     = 1;
    REG_INFO_VALUE(CAM_BPCI_STRIDE)    = m_sPDOHWInfo.u4Bpci_xsz+1;

    reinterpret_cast<ISP_CAM_BNR_BPC_CON_T*>(REG_INFO_VALUE_PTR(CAM_BNR_BPC_CON))->BNR_LE_INV_CTL = 0;

    rTuning.updateEngineFD(eTuningMgrFunc_BNR, i4SubsampleIdex, m_sPDOHWInfo.i4memID, m_sPDOHWInfo.virAddrBpci_tbl);

    rTuning.updateEngine(eTuningMgrFunc_BNR, bBNR_EN, i4SubsampleIdex);

    // TOP
    TUNING_MGR_WRITE_BITS_CAM((&rTuning), CAM_CTL_EN, BNR_EN, bBNR_EN, i4SubsampleIdex);
    ISP_MGR_CTL_EN_P1_T::getInstance(m_eSensorDev).setEnable_BNR(bBNR_EN);

    // Register setting
    rTuning.tuningMgrWriteRegs(static_cast<TUNING_MGR_REG_IO_STRUCT*>(m_pRegInfo),
                             m_u4RegInfoNum, i4SubsampleIdex);
/*
    CAM_LOGE("        Chooo, P1 Pro: %d, Sen: %d, PDC_EN: %d, BPC_EN: %d, LUT_EN:%d",
        rRawIspCamInfo.rMapping_Info.eIspProfile,
        rRawIspCamInfo.rMapping_Info.eSensorMode,
        reinterpret_cast<ISP_CAM_BNR_PDC_CON_T*>(REG_INFO_VALUE_PTR(CAM_BNR_PDC_CON))->PDC_EN,
        reinterpret_cast<ISP_CAM_BNR_BPC_CON_T*>(REG_INFO_VALUE_PTR(CAM_BNR_BPC_CON))->BPC_EN,
        reinterpret_cast<ISP_CAM_BNR_BPC_CON_T*>(REG_INFO_VALUE_PTR(CAM_BNR_BPC_CON))->BPC_LUT_EN
        );
*/

    dumpRegInfoP1("BNR");

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// BNR2
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ISP_MGR_BNR2_T&
ISP_MGR_BNR2_T::
getInstance(ESensorDev_T const eSensorDev)
{
    auto i4SensorIdx = NS3Av3::mapSensorDevToIdx(eSensorDev);
    if(i4SensorIdx >= SENSOR_IDX_MAX || i4SensorIdx < 0) {
        MY_LOGE("[%s] Unsupport sensor Index: %d\n", __FUNCTION__, i4SensorIdx);
    }

    MY_INST_BNR2& rSingleton = gMultitonBNR2[i4SensorIdx];
    std::call_once(rSingleton.onceFlag, [&] {
        rSingleton.instance = std::make_unique<ISP_MGR_BNR2_T>(eSensorDev);
    } );

    return *(rSingleton.instance);
}

ISP_MGR_BNR2::~ISP_MGR_BNR2()
{
    if (m_pBPCI_ImgBuf_Default != nullptr) {
        // using sp to release IImageBuffer
        sp<IImageBuffer> temp_bpci_default = static_cast<IImageBuffer*>(m_pBPCI_ImgBuf_Default);
    }
}

template <>
ISP_MGR_BNR2_T&
ISP_MGR_BNR2_T::
put(ISP_NVRAM_BNR_BPC_T const& rParam)
{
    PUT_REG_INFO(DIP_X_BNR2_BPC_CON, con);
    PUT_REG_INFO(DIP_X_BNR2_BPC_TH1, th1);
    PUT_REG_INFO(DIP_X_BNR2_BPC_TH2, th2);
    PUT_REG_INFO(DIP_X_BNR2_BPC_TH3, th3);
    PUT_REG_INFO(DIP_X_BNR2_BPC_TH4, th4);
    PUT_REG_INFO(DIP_X_BNR2_BPC_DTC, dtc);
    PUT_REG_INFO(DIP_X_BNR2_BPC_COR, cor);
    //PUT_REG_INFO(DIP_X_BNR2_BPC_TBLI1, tbli1);
    PUT_REG_INFO(DIP_X_BNR2_BPC_TH1_C, th1_c);
    PUT_REG_INFO(DIP_X_BNR2_BPC_TH2_C, th2_c);
    PUT_REG_INFO(DIP_X_BNR2_BPC_TH3_C, th3_c);

    return  (*this);
}

template <>
MBOOL
ISP_MGR_BNR2_T::
get(ISP_NVRAM_BNR_BPC_T& rParam, const dip_x_reg_t* pReg) const
{
    if (pReg)
    {
        GET_REG_INFO_BUF(DIP_X_BNR2_BPC_CON, con);
        GET_REG_INFO_BUF(DIP_X_BNR2_BPC_TH1, th1);
        GET_REG_INFO_BUF(DIP_X_BNR2_BPC_TH2, th2);
        GET_REG_INFO_BUF(DIP_X_BNR2_BPC_TH3, th3);
        GET_REG_INFO_BUF(DIP_X_BNR2_BPC_TH4, th4);
        GET_REG_INFO_BUF(DIP_X_BNR2_BPC_DTC, dtc);
        GET_REG_INFO_BUF(DIP_X_BNR2_BPC_COR, cor);
        //GET_REG_INFO_BUF(DIP_X_BNR2_BPC_TBLI1, tbli1);
        GET_REG_INFO_BUF(DIP_X_BNR2_BPC_TH1_C, th1_c);
        GET_REG_INFO_BUF(DIP_X_BNR2_BPC_TH2_C, th2_c);
        GET_REG_INFO_BUF(DIP_X_BNR2_BPC_TH3_C, th3_c);
    }
    return MTRUE;
}

template <>
ISP_MGR_BNR2_T&
ISP_MGR_BNR2_T::
put(ISP_NVRAM_BNR_NR1_T const& rParam)
{
    PUT_REG_INFO(DIP_X_BNR2_NR1_CON, con);
    PUT_REG_INFO(DIP_X_BNR2_NR1_CT_CON, ct_con);
    PUT_REG_INFO(DIP_X_BNR2_NR1_CT_CON2, ct_con2);
    PUT_REG_INFO(DIP_X_BNR2_NR1_CT_CON3, ct_con3);


    return  (*this);
}

template <>
MBOOL
ISP_MGR_BNR2_T::
get(ISP_NVRAM_BNR_NR1_T& rParam, const dip_x_reg_t* pReg) const
{
    if (pReg)
    {
        GET_REG_INFO_BUF(DIP_X_BNR2_NR1_CON, con);
        GET_REG_INFO_BUF(DIP_X_BNR2_NR1_CT_CON, ct_con);
        GET_REG_INFO_BUF(DIP_X_BNR2_NR1_CT_CON2, ct_con2);
        GET_REG_INFO_BUF(DIP_X_BNR2_NR1_CT_CON3, ct_con3);
    }
    return MTRUE;
}

template <>
ISP_MGR_BNR2_T&
ISP_MGR_BNR2_T::
put(ISP_NVRAM_BNR_PDC_T const& rParam)
{
    PUT_REG_INFO(DIP_X_BNR2_PDC_CON, con);
    PUT_REG_INFO(DIP_X_BNR2_PDC_GAIN_L0, gain_l0);
    PUT_REG_INFO(DIP_X_BNR2_PDC_GAIN_L1, gain_l1);
    PUT_REG_INFO(DIP_X_BNR2_PDC_GAIN_L2, gain_l2);
    PUT_REG_INFO(DIP_X_BNR2_PDC_GAIN_L3, gain_l3);
    PUT_REG_INFO(DIP_X_BNR2_PDC_GAIN_L4, gain_l4);
    PUT_REG_INFO(DIP_X_BNR2_PDC_GAIN_R0, gain_r0);
    PUT_REG_INFO(DIP_X_BNR2_PDC_GAIN_R1, gain_r1);
    PUT_REG_INFO(DIP_X_BNR2_PDC_GAIN_R2, gain_r2);
    PUT_REG_INFO(DIP_X_BNR2_PDC_GAIN_R3, gain_r3);
    PUT_REG_INFO(DIP_X_BNR2_PDC_GAIN_R4, gain_r4);
    PUT_REG_INFO(DIP_X_BNR2_PDC_TH_GB, th_gb);
    PUT_REG_INFO(DIP_X_BNR2_PDC_TH_IA, th_ia);
    PUT_REG_INFO(DIP_X_BNR2_PDC_TH_HD, th_hd);
    PUT_REG_INFO(DIP_X_BNR2_PDC_SL, sl);
    //PUT_REG_INFO(DIP_X_BNR2_PDC_POS, pos);

    return  (*this);
}

template <>
MBOOL
ISP_MGR_BNR2_T::
get(ISP_NVRAM_BNR_PDC_T& rParam, const dip_x_reg_t* pReg) const
{
    if (pReg)
    {
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_CON, con);
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_GAIN_L0, gain_l0);
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_GAIN_L1, gain_l1);
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_GAIN_L2, gain_l2);
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_GAIN_L3, gain_l3);
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_GAIN_L4, gain_l4);
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_GAIN_R0, gain_r0);
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_GAIN_R1, gain_r1);
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_GAIN_R2, gain_r2);
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_GAIN_R3, gain_r3);
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_GAIN_R4, gain_r4);
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_TH_GB, th_gb);
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_TH_IA, th_ia);
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_TH_HD, th_hd);
        GET_REG_INFO_BUF(DIP_X_BNR2_PDC_SL, sl);
        //GET_REG_INFO_BUF(DIP_X_BNR2_PDC_POS, pos);
    }
    return MTRUE;
}

MBOOL
ISP_MGR_BNR2_T::
apply(const RAWIspCamInfo& rRawIspCamInfo, dip_x_reg_t* pReg)
{
    MBOOL bBPC2_EN = (isBPC2Enable() & (reinterpret_cast<ISP_CAM_BNR_BPC_CON_T*>(REG_INFO_VALUE_PTR(DIP_X_BNR2_BPC_CON))->BPC_EN));
    MBOOL bCT2_EN  = (isCT2Enable()  & (reinterpret_cast<ISP_CAM_BNR_NR1_CON_T*>(REG_INFO_VALUE_PTR(DIP_X_BNR2_NR1_CON))->NR1_CT_EN));
    MBOOL bPDC2_EN = (isPDC2Enable()  & (reinterpret_cast<ISP_CAM_BNR_PDC_CON_T*>(REG_INFO_VALUE_PTR(DIP_X_BNR2_PDC_CON))->PDC_EN));

    //if(m_ePrvSensorMode != rRawIspCamInfo.rMapping_Info.eSensorMode){
    if(bPDC2_EN){

        Tbl P2BPCITbl_Info;
        m_bPDC_TBL_Valid = IPDTblGen::getInstance()->getTbl( m_eSensorDev, rRawIspCamInfo.rMapping_Info.eSensorMode, P2BPCITbl_Info);

        if (m_bPDC_TBL_Valid){
            m_bPDC_TBL_Valid = MTRUE;
            m_pBPCI_ImgBuf = P2BPCITbl_Info.pbuf;

        }
        else{
            m_bPDC_TBL_Valid = MFALSE;
            m_pBPCI_ImgBuf = NULL;
        }
    }
    //m_ePrvSensorMode = rRawIspCamInfo.rMapping_Info.eSensorMode;
    //}

    bPDC2_EN = bPDC2_EN && m_bPDC_TBL_Valid;

    MBOOL bBNR2_EN = (bBPC2_EN | bCT2_EN | bPDC2_EN);

    setBPC2Enable(bBPC2_EN);
    setCT2Enable(bCT2_EN);
    setPDC2Enable(bPDC2_EN);

    reinterpret_cast<ISP_CAM_BNR_BPC_CON_T*>(REG_INFO_VALUE_PTR(DIP_X_BNR2_BPC_CON))->BPC_EN = bBPC2_EN;
    reinterpret_cast<ISP_CAM_BNR_BPC_CON_T*>(REG_INFO_VALUE_PTR(DIP_X_BNR2_BPC_CON))->BPC_LUT_EN = bBPC2_EN;

    if( !m_pBPCI_ImgBuf && !m_pBPCI_ImgBuf_Default){
        reinterpret_cast<ISP_CAM_BNR_BPC_CON_T*>(REG_INFO_VALUE_PTR(DIP_X_BNR2_BPC_CON))->BPC_LUT_EN = 0;
    }

    reinterpret_cast<ISP_CAM_BNR_NR1_CON_T*>(REG_INFO_VALUE_PTR(DIP_X_BNR2_NR1_CON))->NR1_CT_EN = bCT2_EN;
    reinterpret_cast<ISP_CAM_BNR_PDC_CON_T*>(REG_INFO_VALUE_PTR(DIP_X_BNR2_PDC_CON))->PDC_EN = bPDC2_EN;
    // no use register
    reinterpret_cast<ISP_CAM_BNR_PDC_CON_T*>(REG_INFO_VALUE_PTR(DIP_X_BNR2_PDC_CON))->PDC_OUT = 0;

    reinterpret_cast<ISP_CAM_BNR_PDC_GAIN_L0_T*>(REG_INFO_VALUE_PTR(DIP_X_BNR2_PDC_GAIN_L0))->PDC_GCF_L00 = 256;
    reinterpret_cast<ISP_CAM_BNR_PDC_GAIN_R0_T*>(REG_INFO_VALUE_PTR(DIP_X_BNR2_PDC_GAIN_R0))->PDC_GCF_R00 = 256;
    reinterpret_cast<ISP_CAM_BNR_PDC_TH_GB_T*>(REG_INFO_VALUE_PTR(DIP_X_BNR2_PDC_TH_GB))->PDC_GTH = 4095;
    reinterpret_cast<ISP_CAM_BNR_PDC_TH_IA_T*>(REG_INFO_VALUE_PTR(DIP_X_BNR2_PDC_TH_IA))->PDC_ATH = 4094;

    // TOP
    ISP_WRITE_ENABLE_BITS(pReg, DIP_X_CTL_RGB_EN, BNR2_EN, bBNR2_EN);
    //ISP_MGR_CTL_EN_P2_T::getInstance(m_eSensorDev).setEnable_BNR2(bBNR2_EN);

    // Register setting
    writeRegs(static_cast<RegInfo_T*>(m_pRegInfo), m_u4RegInfoNum, pReg);

    dumpRegInfo("BNR2");

    return  MTRUE;
}

MBOOL
ISP_MGR_BNR2_T::
createBPCI_Default_Buf()
{
    //Default Table
    TblInfo pTempCast_default;

    pTempCast_default.tbl_xsz = Default_BPCI_NUM;
    pTempCast_default.tbl_va = m_Default_BPCI;

    MUINT32 x_size_default = pTempCast_default.tbl_xsz + 1;
    MUINT32 y_size_default = 1;
    // create buffer
    MINT32 bufBoundaryInBytes_default[3] = {0, 0, 0};
    MUINT32 bufStridesInBytes_bpci_default[3] = {x_size_default, 0, 0};

    std::string strName_bpci_default = "BPCI_tbl_default";

    IImageBufferAllocator::ImgParam imgParam_bpci_default =
        IImageBufferAllocator::ImgParam((EImageFormat)eImgFmt_STA_BYTE,
            MSize(x_size_default, y_size_default), bufStridesInBytes_bpci_default, bufBoundaryInBytes_default, 1);

    sp<IIonImageBufferHeap> pHeap_bpci_default =
        IIonImageBufferHeap::create(strName_bpci_default.c_str(), imgParam_bpci_default);

    if (pHeap_bpci_default == NULL) {
        CAM_LOGE("[%s] ImageBufferHeap create fail", strName_bpci_default.c_str());
        return MFALSE;
    }

    IImageBuffer* pImgBuf_bpci_default = pHeap_bpci_default->createImageBuffer();

    if (pImgBuf_bpci_default == NULL) {
        CAM_LOGE("[%s] ImageBufferHeap create fail", strName_bpci_default.c_str());
        return MFALSE;
    }

    // lock buffer
    MUINT const usage_bpci_default = (GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN |
                        GRALLOC_USAGE_HW_CAMERA_READ |
                        GRALLOC_USAGE_HW_CAMERA_WRITE);

    if (!(pImgBuf_bpci_default->lockBuf(strName_bpci_default.c_str(), usage_bpci_default)))
    {
        CAM_LOGE("[%s] Stuff ImageBuffer lock fail",  strName_bpci_default.c_str());
        return MFALSE;
    }

    memcpy(reinterpret_cast<MVOID*>(pImgBuf_bpci_default->getBufVA(0)), pTempCast_default.tbl_va, (x_size_default*y_size_default));

    char value_default[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("vendor.dump.bpci_tbl.enable", value_default, "0"); // 0: enable, 1: disable
    MBOOL bDumpTblEnable_default = atoi(value_default);

    if (bDumpTblEnable_default)
    {
        char filename_bpci_default[512];
        sprintf(filename_bpci_default, "/sdcard/debug/bpci_table_default_size_%d.bin", (x_size_default*y_size_default));
        FILE* fp_bpci_default = fopen(filename_bpci_default, "wb");
        if (fp_bpci_default)
        {
            ::fwrite(pTempCast_default.tbl_va, (x_size_default*y_size_default), 1, fp_bpci_default);
            fclose(fp_bpci_default);
        }
    }

    // release m_pPDCBuffer
    if (m_pBPCI_ImgBuf_Default){
        sp<IImageBuffer> temp_default = static_cast<IImageBuffer*>(m_pBPCI_ImgBuf_Default);
        temp_default->unlockBuf(strName_bpci_default.c_str());
    }

    m_pBPCI_ImgBuf_Default  = static_cast<MVOID*>(pImgBuf_bpci_default);


    return MTRUE;
}


MVOID
ISP_MGR_BNR2_T::
unlockBPCIBuf()
{
    std::string strName_bpci_default = "BPCI_tbl_default";  //same in setPBCIBuf

    if (m_pBPCI_ImgBuf_Default != NULL)
    {
        static_cast<IImageBuffer*>(m_pBPCI_ImgBuf_Default)->unlockBuf(strName_bpci_default.c_str());
        // destroy buffer
        sp<IImageBuffer> temp_bpci_default = static_cast<IImageBuffer*>(m_pBPCI_ImgBuf_Default);
        m_pBPCI_ImgBuf_Default = NULL;
    }
}

}

