/*
 * MediaTek Inc. (C) 2018. All rights reserved.
 *
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */
#ifndef __NANDX_PLATFORM_H__
#define __NANDX_PLATFORM_H__

#include "nandx_util.h"
#include "nandx_info.h"

#ifdef MT8167
#define NANDX_PLATFORM  NANDX_MT8167
#else
#define NANDX_PLATFORM  NANDX_NONE
#endif

static inline enum IC_VER nandx_get_chip_version(void)
{
	if (NANDX_PLATFORM == NANDX_NONE)
		NANDX_ASSERT(0);

	return NANDX_PLATFORM;
}

static inline struct nfc_resource *nandx_get_nfc_resource(enum IC_VER ver)
{
	extern struct nfc_resource nandx_resource[];

	if (ver == NANDX_MT8167)
		return &nandx_resource[NANDX_MT8167];

	NANDX_ASSERT(0);

	return NULL;
}

void nandx_platform_enable_clock(struct platform_data *data,
                                 bool high_speed_en, bool ecc_clk_en);
void nandx_platform_disable_clock(struct platform_data *data,
                                  bool high_speed_en, bool ecc_clk_en);
void nandx_platform_prepare_clock(struct platform_data *data,
                                  bool high_speed_en, bool ecc_clk_en);
void nandx_platform_unprepare_clock(struct platform_data *data,
                                    bool high_speed_en, bool ecc_clk_en);
int nandx_platform_init(struct platform_data *pdata);
int nandx_platform_power_on(struct platform_data *pdata);
int nandx_platform_power_down(struct platform_data *pdata);

#endif              /* __NANDX_PLATFORM_H__ */
