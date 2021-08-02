/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2018. All rights reserved.
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
 */
#include "hw_res_mgr.h"
#include "audio_rtos_header_group.h"
#include "semphr.h"

#include "mt_printf.h"
#include "systimer.h"
#ifdef CFG_SPM_SUPPORT
#include "spm.h"
#endif
#ifdef CFG_DSP_CLK_SUPPORT
#include "dsp_clk.h"
#endif

#ifdef CFG_SPM_SUPPORT
static inline int spm_res_value_convert(int value)
{
    int ret;
    switch (value) {
    case SPM_NONE:
        ret = SPM_NONE;
        break;
    case DSP_SYS_HW_26M:
        ret = SPM_26M;
        break;
    case DSP_SYS_HW_INFRA:
        ret = SPM_INFRA;
        break;
    case DSP_SYS_HW_PLL:
        ret = SPM_PLL;
        break;
    case DSP_SYS_HW_DRAM:
        ret = SPM_DRAM;
        break;
    default:
        ret = SPM_NONE;
        break;
    }
    return ret;
}
#endif

int spm_res_ops_set(int value)
{
    int ret = 0;
#ifdef CFG_SPM_SUPPORT
    /* convert res manger value to driver map */
    value = spm_res_value_convert(value);

    ret = spm_res_set((uint32_t)value);
    if (ret != 0)
        PRINTF_E("spm res set to %d fail(%d)\n", value, ret);
#endif
    return ret;
}

#ifdef CFG_DSP_CLK_SUPPORT
static inline int dsp_clk_value_convert(int value)
{
    int ret;
    switch (value) {
    case DSP_CLK_13M:
        ret = CLK_DSP_SEL_26M_D_2;
        break;
    case DSP_CLK_26M:
        ret = CLK_DSP_SEL_26M;
        break;
    case DSP_CLK_PLL_D_8:
        ret = CLK_DSP_SEL_DSPPLL_D_8;
        break;
    case DSP_CLK_PLL_D_4:
        ret = CLK_DSP_SEL_DSPPLL_D_4;
        break;
    case DSP_CLK_PLL_D_2:
        ret = CLK_DSP_SEL_DSPPLL_D_2;
        break;
    case DSP_CLK_PLL:
        ret = CLK_DSP_SEL_DSPPLL;
        break;
    default:
        ret = CLK_DSP_SEL_26M;
        break;
    }
    return ret;
}
#endif

int dsp_clk_res_ops_set(int value)
{
    int ret = 0;

#ifdef CFG_DSP_CLK_SUPPORT
    /* convert res manger value to driver map */
    value = dsp_clk_value_convert(value);
    /* for dsp clock setting, need enable infra */
    dsp_hw_res_request(DSP_HW_RES_SPM_REQ, DSP_HW_USER_RES_MGR, DSP_SYS_HW_INFRA);
    if (value > DSP_CLK_26M && (dsppll_get_enabled() == 0))
         clk_dsppll_enable();
    ret = set_mux_sel(MUX_CLK_DSP_SEL, value);
    if (value <= DSP_CLK_26M && (dsppll_get_enabled() == 1))
         clk_dsppll_disable();
    dsp_hw_res_request(DSP_HW_RES_SPM_REQ, DSP_HW_USER_RES_MGR, DSP_SYS_HW_NONE);
#endif
    return ret;
}

#ifdef CFG_DSP_ULPLL_SUPPORT
int dsp_ulpll_res_ops_set(int value)
{
    int ret = 0;
#ifdef CFG_DSP_CLK_SUPPORT
    switch (value) {
    case DSP_ULPLL_OFF:
        if (ulpll_get_enabled())
            clk_ulpll_disable();
        break;
    case DSP_ULPLL_ON:
        if (!ulpll_get_enabled())
            clk_ulpll_enable();
        break;
    default:
        break;
    }
#endif
    return ret;
}

int dsp_aud26m_res_ops_set(int value)
{
    int ret = 0;
#ifdef CFG_DSP_CLK_SUPPORT
    switch (value) {
    case DSP_AUD_26M_DCXO:
        set_mux_aud26m_sel(DCXO_26);
        break;
    case DSP_AUD_26M_ULPLL:
        set_mux_aud26m_sel(ULPLL_26M);
        break;
    default:
        break;
    }
#endif
    return ret;
}
#endif

NORMAL_SECTION_FUNC int hw_res_implement_init(void)
{
    dsp_hw_res_init();
    dsp_hw_res_register_ops(DSP_HW_RES_SPM_REQ, spm_res_ops_set);
    dsp_hw_res_register_ops(DSP_HW_RES_CLK, dsp_clk_res_ops_set);
#ifdef CFG_DSP_ULPLL_SUPPORT
    dsp_hw_res_register_ops(DSP_HW_RES_ULPLL, dsp_ulpll_res_ops_set);
    dsp_hw_res_register_ops(DSP_HW_RES_AUD26M, dsp_aud26m_res_ops_set);
#endif
#ifdef CFG_DSP_CLK_SUPPORT
    /* set DSP PLL value, not fix PLL setting in whole state */
    dsppll_set_freq(CFG_DSP_PLL_VALUE);
#endif
    return 0;
}

NORMAL_SECTION_FUNC int hw_res_implement_uninit(void)
{
    dsp_hw_res_uninit();
    return 0;
}



