/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
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
#ifndef _ISP_MGR_COLOR_H_
#define _ISP_MGR_COLOR_H_

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// COLOR
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define INIT_COLOR_ADDR(reg)\
    INIT_REG_INFO_ADDR_P2_MULTI(ECOLOR_D1, reg, COLOR_D1A_COLOR_);\

typedef class ISP_MGR_COLOR : public ISP_MGR_BASE_T
{
    typedef ISP_MGR_COLOR    MyType;
public:
    enum
    {
        ECOLOR_D1,
        ESubModule_NUM
    };
private:
    MBOOL m_bEnable[ESubModule_NUM];
    MBOOL m_bCCTEnable;

    enum
    {
        ERegInfo_DIP_COLOR_CFG_MAIN,
		ERegInfo_WIN_X_MAIN,
		ERegInfo_WIN_Y_MAIN,
		ERegInfo_DBG_CFG_MAIN,
		ERegInfo_INK_DATA_MAIN,
		ERegInfo_INK_DATA_MAIN_CR,
		ERegInfo_SAT_HIST_X_CFG_MAIN,
		ERegInfo_SAT_HIST_Y_CFG_MAIN,
		ERegInfo_CRC_0,
		ERegInfo_CRC_1,
		ERegInfo_CRC_2,
		ERegInfo_CRC_3,
		ERegInfo_FRAME_DONE_DEL,
		ERegInfo_CM1_EN,
		ERegInfo_CM2_EN,
        ERegInfo_DIP_COLOR_C_BOOST_MAIN,
        ERegInfo_DIP_COLOR_C_BOOST_MAIN_2,
        ERegInfo_DIP_COLOR_LUMA_ADJ,
        ERegInfo_DIP_COLOR_G_PIC_ADJ_MAIN_1,
        ERegInfo_DIP_COLOR_G_PIC_ADJ_MAIN_2,
        ERegInfo_DIP_COLOR_Y_SLOPE_1_0_MAIN,
        ERegInfo_DIP_COLOR_Y_SLOPE_3_2_MAIN,
        ERegInfo_DIP_COLOR_Y_SLOPE_5_4_MAIN,
        ERegInfo_DIP_COLOR_Y_SLOPE_7_6_MAIN,
        ERegInfo_DIP_COLOR_Y_SLOPE_9_8_MAIN,
        ERegInfo_DIP_COLOR_Y_SLOPE_11_10_MAIN,
        ERegInfo_DIP_COLOR_Y_SLOPE_13_12_MAIN,
        ERegInfo_DIP_COLOR_Y_SLOPE_15_14_MAIN,
        ERegInfo_DIP_COLOR_LOCAL_HUE_CD_0,
        ERegInfo_DIP_COLOR_LOCAL_HUE_CD_1,
        ERegInfo_DIP_COLOR_LOCAL_HUE_CD_2,
        ERegInfo_DIP_COLOR_LOCAL_HUE_CD_3,
        ERegInfo_DIP_COLOR_LOCAL_HUE_CD_4,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN1_0,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN1_1,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN1_2,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN1_3,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN1_4,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN2_0,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN2_1,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN2_2,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN2_3,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN2_4,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN3_0,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN3_1,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN3_2,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN3_3,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_GAIN3_4,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_POINT1_0,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_POINT1_1,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_POINT1_2,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_POINT1_3,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_POINT1_4,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_POINT2_0,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_POINT2_1,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_POINT2_2,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_POINT2_3,
        ERegInfo_DIP_COLOR_PARTIAL_SAT_POINT2_4,
        ERegInfo_DIP_COLOR_START,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y0_0,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y0_1,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y0_2,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y0_3,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y0_4,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y64_0,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y64_1,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y64_2,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y64_3,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y64_4,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y128_0,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y128_1,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y128_2,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y128_3,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y128_4,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y192_0,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y192_1,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y192_2,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y192_3,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y192_4,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y256_0,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y256_1,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y256_2,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y256_3,
        ERegInfo_DIP_COLOR_S_GAIN_BY_Y256_4,
        ERegInfo_DIP_COLOR_LSP_1,
        ERegInfo_DIP_COLOR_LSP_2,
        ERegInfo_DIP_COLOR_CM_CONTROL,
        ERegInfo_DIP_COLOR_CM_W1_HUE_0,
        ERegInfo_DIP_COLOR_CM_W1_HUE_1,
        ERegInfo_DIP_COLOR_CM_W1_HUE_2,
        ERegInfo_DIP_COLOR_CM_W1_HUE_3,
        ERegInfo_DIP_COLOR_CM_W1_HUE_4,
        ERegInfo_DIP_COLOR_CM_W1_LUMA_0,
        ERegInfo_DIP_COLOR_CM_W1_LUMA_1,
        ERegInfo_DIP_COLOR_CM_W1_LUMA_2,
        ERegInfo_DIP_COLOR_CM_W1_LUMA_3,
        ERegInfo_DIP_COLOR_CM_W1_LUMA_4,
        ERegInfo_DIP_COLOR_CM_W1_SAT_0,
        ERegInfo_DIP_COLOR_CM_W1_SAT_1,
        ERegInfo_DIP_COLOR_CM_W1_SAT_2,
        ERegInfo_DIP_COLOR_CM_W1_SAT_3,
        ERegInfo_DIP_COLOR_CM_W1_SAT_4,
        ERegInfo_DIP_COLOR_CM_W2_HUE_0,
        ERegInfo_DIP_COLOR_CM_W2_HUE_1,
        ERegInfo_DIP_COLOR_CM_W2_HUE_2,
        ERegInfo_DIP_COLOR_CM_W2_HUE_3,
        ERegInfo_DIP_COLOR_CM_W2_HUE_4,
        ERegInfo_DIP_COLOR_CM_W2_LUMA_0,
        ERegInfo_DIP_COLOR_CM_W2_LUMA_1,
        ERegInfo_DIP_COLOR_CM_W2_LUMA_2,
        ERegInfo_DIP_COLOR_CM_W2_LUMA_3,
        ERegInfo_DIP_COLOR_CM_W2_LUMA_4,
        ERegInfo_DIP_COLOR_CM_W2_SAT_0,
        ERegInfo_DIP_COLOR_CM_W2_SAT_1,
        ERegInfo_DIP_COLOR_CM_W2_SAT_2,
        ERegInfo_DIP_COLOR_CM_W2_SAT_3,
        ERegInfo_DIP_COLOR_CM_W2_SAT_4,
        ERegInfo_DIP_COLOR_CM_W3_HUE_0,
        ERegInfo_DIP_COLOR_CM_W3_HUE_1,
        ERegInfo_DIP_COLOR_CM_W3_HUE_2,
        ERegInfo_DIP_COLOR_CM_W3_HUE_3,
        ERegInfo_DIP_COLOR_CM_W3_HUE_4,
        ERegInfo_DIP_COLOR_CM_W3_LUMA_0,
        ERegInfo_DIP_COLOR_CM_W3_LUMA_1,
        ERegInfo_DIP_COLOR_CM_W3_LUMA_2,
        ERegInfo_DIP_COLOR_CM_W3_LUMA_3,
        ERegInfo_DIP_COLOR_CM_W3_LUMA_4,
        ERegInfo_DIP_COLOR_CM_W3_SAT_0,
        ERegInfo_DIP_COLOR_CM_W3_SAT_1,
        ERegInfo_DIP_COLOR_CM_W3_SAT_2,
        ERegInfo_DIP_COLOR_CM_W3_SAT_3,
        ERegInfo_DIP_COLOR_CM_W3_SAT_4,
        ERegInfo_DIP_COLOR_TILE,
        ERegInfo_DIP_COLOR_CROP,
        ERegInfo_NUM
    };

    RegInfo_T m_rIspRegInfo[ESubModule_NUM][ERegInfo_NUM];

protected:
    ISP_MGR_COLOR(ESensorDev_T const eSensorDev)
        : ISP_MGR_BASE_T(m_rIspRegInfo, ERegInfo_NUM, eSensorDev, ESubModule_NUM)
        , m_bCCTEnable(MTRUE)
    {
        for(int i=0; i<ESubModule_NUM; i++){
            m_bEnable[i] = MFALSE;
            ::memset(m_rIspRegInfo[i], 0, sizeof(RegInfo_T)*ERegInfo_NUM);
        }
        INIT_COLOR_ADDR(DIP_COLOR_CFG_MAIN);
        INIT_COLOR_ADDR(DIP_COLOR_C_BOOST_MAIN);
        INIT_COLOR_ADDR(DIP_COLOR_C_BOOST_MAIN_2);
        INIT_COLOR_ADDR(DIP_COLOR_LUMA_ADJ);
        INIT_COLOR_ADDR(DIP_COLOR_G_PIC_ADJ_MAIN_1);
        INIT_COLOR_ADDR(DIP_COLOR_G_PIC_ADJ_MAIN_2);
        INIT_COLOR_ADDR(DIP_COLOR_Y_SLOPE_1_0_MAIN);
        INIT_COLOR_ADDR(DIP_COLOR_Y_SLOPE_3_2_MAIN);
        INIT_COLOR_ADDR(DIP_COLOR_Y_SLOPE_5_4_MAIN);
        INIT_COLOR_ADDR(DIP_COLOR_Y_SLOPE_7_6_MAIN);
        INIT_COLOR_ADDR(DIP_COLOR_Y_SLOPE_9_8_MAIN);
        INIT_COLOR_ADDR(DIP_COLOR_Y_SLOPE_11_10_MAIN);
        INIT_COLOR_ADDR(DIP_COLOR_Y_SLOPE_13_12_MAIN);
        INIT_COLOR_ADDR(DIP_COLOR_Y_SLOPE_15_14_MAIN);
        INIT_COLOR_ADDR(DIP_COLOR_LOCAL_HUE_CD_0);
        INIT_COLOR_ADDR(DIP_COLOR_LOCAL_HUE_CD_1);
        INIT_COLOR_ADDR(DIP_COLOR_LOCAL_HUE_CD_2);
        INIT_COLOR_ADDR(DIP_COLOR_LOCAL_HUE_CD_3);
        INIT_COLOR_ADDR(DIP_COLOR_LOCAL_HUE_CD_4);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN1_0);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN1_1);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN1_2);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN1_3);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN1_4);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN2_0);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN2_1);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN2_2);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN2_3);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN2_4);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN3_0);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN3_1);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN3_2);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN3_3);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_GAIN3_4);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_POINT1_0);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_POINT1_1);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_POINT1_2);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_POINT1_3);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_POINT1_4);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_POINT2_0);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_POINT2_1);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_POINT2_2);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_POINT2_3);
        INIT_COLOR_ADDR(DIP_COLOR_PARTIAL_SAT_POINT2_4);
        INIT_COLOR_ADDR(DIP_COLOR_START);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y0_0);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y0_1);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y0_2);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y0_3);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y0_4);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y64_0);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y64_1);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y64_2);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y64_3);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y64_4);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y128_0);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y128_1);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y128_2);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y128_3);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y128_4);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y192_0);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y192_1);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y192_2);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y192_3);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y192_4);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y256_0);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y256_1);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y256_2);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y256_3);
        INIT_COLOR_ADDR(DIP_COLOR_S_GAIN_BY_Y256_4);
        INIT_COLOR_ADDR(DIP_COLOR_LSP_1);
        INIT_COLOR_ADDR(DIP_COLOR_LSP_2);
        INIT_COLOR_ADDR(DIP_COLOR_CM_CONTROL);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_HUE_0);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_HUE_1);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_HUE_2);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_HUE_3);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_HUE_4);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_LUMA_0);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_LUMA_1);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_LUMA_2);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_LUMA_3);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_LUMA_4);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_SAT_0);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_SAT_1);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_SAT_2);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_SAT_3);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W1_SAT_4);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_HUE_0);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_HUE_1);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_HUE_2);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_HUE_3);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_HUE_4);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_LUMA_0);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_LUMA_1);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_LUMA_2);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_LUMA_3);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_LUMA_4);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_SAT_0);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_SAT_1);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_SAT_2);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_SAT_3);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W2_SAT_4);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_HUE_0);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_HUE_1);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_HUE_2);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_HUE_3);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_HUE_4);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_LUMA_0);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_LUMA_1);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_LUMA_2);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_LUMA_3);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_LUMA_4);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_SAT_0);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_SAT_1);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_SAT_2);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_SAT_3);
        INIT_COLOR_ADDR(DIP_COLOR_CM_W3_SAT_4);
        INIT_COLOR_ADDR(DIP_COLOR_TILE);
        INIT_COLOR_ADDR(DIP_COLOR_CROP);
        INIT_COLOR_ADDR(DIP_COLOR_START);
        for(int i=0; i<ESubModule_NUM; i++){
            //Sub module always ENABLE
            reinterpret_cast<COLOR_REG_D1A_COLOR_DIP_COLOR_START*>(REG_INFO_VALUE_PTR_MULTI(i, DIP_COLOR_START))->Bits.COLOR_disp_color_start = MTRUE;
            m_rIspRegInfo[i][ERegInfo_WIN_X_MAIN].val          = 0xFFFF0000;
            m_rIspRegInfo[i][ERegInfo_WIN_Y_MAIN].val          = 0xFFFF0000;
            m_rIspRegInfo[i][ERegInfo_DBG_CFG_MAIN].val        = 0x00000700;
            m_rIspRegInfo[i][ERegInfo_INK_DATA_MAIN].val       = 0x020003FF;
            m_rIspRegInfo[i][ERegInfo_INK_DATA_MAIN_CR].val    = 0x00000200;
            m_rIspRegInfo[i][ERegInfo_SAT_HIST_X_CFG_MAIN].val = 0xFFFF0000;
            m_rIspRegInfo[i][ERegInfo_SAT_HIST_Y_CFG_MAIN].val = 0xFFFF0000;
            m_rIspRegInfo[i][ERegInfo_CRC_0].val               = 0x00000003;
            m_rIspRegInfo[i][ERegInfo_CRC_1].val               = 0x1FFF0000;
            m_rIspRegInfo[i][ERegInfo_CRC_2].val               = 0x0FFF0000;
            m_rIspRegInfo[i][ERegInfo_CRC_3].val               = 0x3FFFFFFF;
            m_rIspRegInfo[i][ERegInfo_FRAME_DONE_DEL].val      = 0x00000010;
            m_rIspRegInfo[i][ERegInfo_CM1_EN].val              = 0x00000003;
            m_rIspRegInfo[i][ERegInfo_CM2_EN].val              = 0x00000001;
            reinterpret_cast<COLOR_REG_D1A_COLOR_DIP_COLOR_CROP*>(REG_INFO_VALUE_PTR_MULTI(i, DIP_COLOR_CROP))->Bits.COLOR_crop_h = 2;
            reinterpret_cast<COLOR_REG_D1A_COLOR_DIP_COLOR_CROP*>(REG_INFO_VALUE_PTR_MULTI(i, DIP_COLOR_CROP))->Bits.COLOR_crop_v = 0;
        }
    }

    virtual ~ISP_MGR_COLOR() {}

public:
    static MyType&  getInstance(ESensorDev_T const eSensorDev);

public:

    template <class ISP_xxx_T>
    MyType& put(MUINT8 SubModuleIndex, ISP_xxx_T const& rParam);

    template <class ISP_xxx_T>
    MyType& get(MUINT8 SubModuleIndex, ISP_xxx_T & rParam);

    MBOOL
    isEnable(MUINT8 SubModuleIndex)
    {
        return m_bEnable[SubModuleIndex];
    }

    MBOOL
    isCCTEnable()
    {
        return m_bCCTEnable;
    }

    MVOID
    setEnable(MUINT8 SubModuleIndex, MBOOL bEnable)
    {
        m_bEnable[SubModuleIndex] = bEnable;
    }

    MVOID
    setCCTEnable(MBOOL bEnable)
    {
        m_bCCTEnable = bEnable;
    }

    MBOOL apply_P2(MUINT8 SubModuleIndex, const RAWIspCamInfo& rRawIspCamInfo, dip_a_reg_t* pReg);

} ISP_MGR_COLOR_T;

template <ESensorDev_T const eSensorDev>
class ISP_MGR_COLOR_DEV : public ISP_MGR_COLOR_T
{
public:
    static
    ISP_MGR_COLOR_T&
    getInstance()
    {
        static ISP_MGR_COLOR_DEV<eSensorDev> singleton;
        return singleton;
    }
    virtual MVOID destroyInstance() {}

    ISP_MGR_COLOR_DEV()
        : ISP_MGR_COLOR_T(eSensorDev)
    {}

    virtual ~ISP_MGR_COLOR_DEV() {}

};
#endif