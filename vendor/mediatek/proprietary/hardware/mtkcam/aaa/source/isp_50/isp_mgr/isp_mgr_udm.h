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
#ifndef _ISP_MGR_UDM_H_
#define _ISP_MGR_UDM_H_

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  UDM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef class ISP_MGR_UDM : public ISP_MGR_BASE_T
{
    typedef ISP_MGR_UDM    MyType;
private:
    MBOOL m_bEnable;
    MBOOL m_bCCTEnable;
    MUINT32 m_u4StartAddr; // for debug purpose: CAM+0x08A0

    enum
    {

       ERegInfo_DIP_X_UDM_INTP_CRS,
       ERegInfo_DIP_X_UDM_INTP_NAT,
       ERegInfo_DIP_X_UDM_INTP_AUG,
       ERegInfo_DIP_X_UDM_LUMA_LUT1,
       ERegInfo_DIP_X_UDM_LUMA_LUT2,
       ERegInfo_DIP_X_UDM_SL_CTL,
       ERegInfo_DIP_X_UDM_HFTD_CTL,
       ERegInfo_DIP_X_UDM_NR_STR,
       ERegInfo_DIP_X_UDM_NR_ACT,
       ERegInfo_DIP_X_UDM_HF_STR,
       ERegInfo_DIP_X_UDM_HF_ACT1,
       ERegInfo_DIP_X_UDM_HF_ACT2,
       ERegInfo_DIP_X_UDM_CLIP,
       ERegInfo_DIP_X_UDM_DSB,
       ERegInfo_DIP_X_UDM_P1_ACT,
       ERegInfo_DIP_X_UDM_LR_RAT,
       ERegInfo_DIP_X_UDM_HFTD_CTL2,
       ERegInfo_DIP_X_UDM_EST_CTL,
       ERegInfo_DIP_X_UDM_INT_CTL,
       ERegInfo_DIP_X_UDM_EE,
       ERegInfo_NUM
    };
    RegInfo_T m_rIspRegInfo[ERegInfo_NUM];

public:
    ISP_MGR_UDM(ESensorDev_T const eSensorDev)
        : ISP_MGR_BASE_T(m_rIspRegInfo, ERegInfo_NUM, m_u4StartAddr, eSensorDev)
        , m_bEnable(MTRUE)
        , m_bCCTEnable(MTRUE)
        , m_u4StartAddr(REG_ADDR_P2(DIP_X_UDM_INTP_CRS))
    {
        // register info addr init
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_INTP_CRS);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_INTP_NAT);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_INTP_AUG);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_LUMA_LUT1);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_LUMA_LUT2);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_SL_CTL);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_HFTD_CTL);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_NR_STR);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_NR_ACT);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_HF_STR);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_HF_ACT1);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_HF_ACT2);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_CLIP);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_DSB);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_P1_ACT);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_LR_RAT);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_HFTD_CTL2);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_EST_CTL);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_INT_CTL);
        INIT_REG_INFO_ADDR_P2(DIP_X_UDM_EE);
    }

    virtual ~ISP_MGR_UDM() {}

public:
    static MyType&  getInstance(ESensorDev_T const eSensorDev);

public: // Interfaces.

    MBOOL
    isEnable()
    {
        return m_bEnable;
    }

    MVOID
    setEnable(MBOOL bEnable)
    {
        m_bEnable = bEnable;
    }

    MBOOL
    isCCTEnable()
    {
        return m_bCCTEnable;
    }

    MVOID
    setCCTEnable(MBOOL bEnable)
    {
        m_bCCTEnable = bEnable;
    }

    template <class ISP_xxx_T>
    MyType& put(ISP_xxx_T const& rParam);

    template <class ISP_xxx_T>
    MBOOL get(ISP_xxx_T & rParam, const dip_x_reg_t* pReg) const;

    MBOOL apply(EIspProfile_T eIspProfile, dip_x_reg_t* pReg);
} ISP_MGR_UDM_T;


#endif

