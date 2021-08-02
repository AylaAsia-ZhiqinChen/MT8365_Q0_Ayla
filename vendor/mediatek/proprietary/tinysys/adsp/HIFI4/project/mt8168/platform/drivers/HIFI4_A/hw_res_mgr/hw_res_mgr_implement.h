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
#ifndef _HW_RES_MGR_IMPLEMENT_H_
#define _HW_RES_MGR_IMPLEMENT_H_

/* user list */
enum DSP_HW_USER_LIST {
    DSP_HW_USER_TOP_CTRL = 0,
    DSP_HW_USER_VA_TASK,
    DSP_HW_USER_RES_MGR,
    DSP_HW_USER_DYM_PREPROC,
    DSP_HW_USER_NUM,
};

/* resource list */
enum DSP_HW_RES_LIST {
    DSP_HW_RES_SPM_REQ = 0,
    DSP_HW_RES_CLK,
    DSP_HW_RES_ULPLL,
    DSP_HW_RES_AUD26M,
    DSP_HW_RES_NUM,
};

/* List resource from low to high request */
/* 0 is the lowest request */
enum DSP_HW_DSP_CLK {
    DSP_CLK_13M = 0,
    DSP_CLK_26M,
    DSP_CLK_PLL_D_8,
    DSP_CLK_PLL_D_4,
    DSP_CLK_PLL_D_2,
    DSP_CLK_PLL,
};

/* List resource from low to high request */
/* 0 is the lowest request */
enum DSP_SYS_HW_REQ {
    DSP_SYS_HW_NONE = 0,
    DSP_SYS_HW_26M,
    DSP_SYS_HW_INFRA,
    DSP_SYS_HW_PLL,
    DSP_SYS_HW_DRAM,
    DSP_SYS_HW_REQ_NUM,
};

/* List resource from low to high request */
/* 0 is the lowest request */
enum DSP_RES_ULPLL {
    DSP_ULPLL_OFF = 0,
    DSP_ULPLL_ON,
};

enum DSP_RES_AUD26M {
    DSP_AUD_26M_DCXO,
    DSP_AUD_26M_ULPLL,
};

int hw_res_implement_init(void);
int hw_res_implement_uninit(void);

#endif
