/*
 * MediaTek Inc. (C) 2019. All rights reserved.
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

#include "nandx_util.h"
#include "nandx_errno.h"
#include "nandx_info.h"
#include "nandx_platform.h"

/* For NFI GPIO setting */
#undef GPIO_BASE
#define GPIO_BASE   0x10005000
/* NCLE */
#define GPIO_MODE3  (GPIO_BASE + 0x320)
/* NCEB1/NCEB0/NREB */
#define GPIO_MODE4  (GPIO_BASE + 0x330)
/* NRNB/NREB_C/NDQS_C */
#define GPIO_MODE5  (GPIO_BASE + 0x340)
/* NLD7/NLD6/NLD4/NLD3/NLD0 */
#define GPIO_MODE17 (GPIO_BASE + 0x460)
/* NALE/NWEB/NLD1/NLD5/NLD8  NLD8--->NDQS */
#define GPIO_MODE18 (GPIO_BASE + 0x470)
/* NLD2 */
#define GPIO_MODE19 (GPIO_BASE + 0x480)
/* PD, NCEB0/NCEB1/NRNB */
#define GPIO_PULLUP (GPIO_BASE + 0xE60)
/* Drving */
#define GPIO_DRV_MODE0  (GPIO_BASE + 0xD00)
#define GPIO_DRV_MODE6  (GPIO_BASE + 0xD60)
#define GPIO_DRV_MODE7  (GPIO_BASE + 0xD70)
/* TDSEL, no need */
/* RDSEL, no need for 1.8V */
#define GPIO_RDSEL1_EN  (GPIO_BASE + 0xC10)
#define GPIO_RDSEL6_EN  (GPIO_BASE + 0xC60)
#define GPIO_RDSEL7_EN  (GPIO_BASE + 0xC70)

#define EFUSE_M_SW_RES  (0x10009000 + 0x0120)
#define EFUSE_IO_33V    0x00000100
/* 0 : 3.3v (MT8130 default), 1 : 1.8v */
#define EFUSE_IS_IO_33V() \
    ((nreadl((void *)EFUSE_M_SW_RES) & EFUSE_IO_33V) ? false : true)
#define GPIO_CFG_BIT32(reg, field, val) \
    do { \
        u32 tv = nreadl((void *)(reg)); \
        tv &= ~(field); \
        tv |= val; \
        nwritel(tv, (void *)(reg)); \
    } while (0)

static void nandx_platform_set_gpio(void)
{
    /* Enable Pinmux Function setting */
    /* NCLE */
    GPIO_CFG_BIT32(GPIO_MODE3, (0x7 << 12), (0x6 << 12));
    /* NCEB1/NCEB0/NREB */
    GPIO_CFG_BIT32(GPIO_MODE4, (0x1FF << 0), (0x1B6 << 0));
    /* NRNB/NREB_C/NDQS_C */
    GPIO_CFG_BIT32(GPIO_MODE5, (0x1FF << 3), (0x1B1 << 3));
    /* NLD7/NLD6/NLD4/NLD3/NLD0 */
    GPIO_CFG_BIT32(GPIO_MODE17, (0x7FFF << 0), (0x4924 << 0));
    /* NALE/NWEB/NLD1/NLD5/NLD8, NLD8 for NDQS */
    GPIO_CFG_BIT32(GPIO_MODE18, (0x7FFF << 0), (0x4924 << 0));
    /* NLD2 */
    GPIO_CFG_BIT32(GPIO_MODE19, (0x7 << 0), (0x5 << 0));

    /* PULL UP setting */
    /* PD, NCEB0, NCEB1, NRNB */
    GPIO_CFG_BIT32(GPIO_PULLUP, (0xF0FF << 0), (0x1011 << 0));
    /* Driving setting */
    if (EFUSE_IS_IO_33V()) {
        GPIO_CFG_BIT32(GPIO_DRV_MODE0, (0xF << 12), (0x2 << 12));
        GPIO_CFG_BIT32(GPIO_DRV_MODE6, (0xFF << 8), (0x22 << 8));
        GPIO_CFG_BIT32(GPIO_DRV_MODE7, (0xFF << 0), (0x22 << 0));
    } else {
        GPIO_CFG_BIT32(GPIO_DRV_MODE0, (0xF << 12), (0x4 << 12));
        GPIO_CFG_BIT32(GPIO_DRV_MODE6, (0xFF << 8), (0x44 << 8));
        GPIO_CFG_BIT32(GPIO_DRV_MODE7, (0xFF << 0), (0x44 << 0));
    }

    /* TDSEL, No need */

    /* RDSEL, only need for 3.3V */
    if (EFUSE_IS_IO_33V()) {
        GPIO_CFG_BIT32(GPIO_RDSEL1_EN, (0x3F << 6), (0xC << 6));
        GPIO_CFG_BIT32(GPIO_RDSEL6_EN, (0xFFF << 0), (0x30C << 0));
        GPIO_CFG_BIT32(GPIO_RDSEL7_EN, (0xFFF << 0), (0x30C << 0));
    }
}

void nandx_platform_enable_clock(struct platform_data *pdata,
                                 bool high_speed_en, bool ecc_clk_en)
{

}

void nandx_platform_disable_clock(struct platform_data *pdata,
                                  bool high_speed_en, bool ecc_clk_en)
{

}

void nandx_platform_prepare_clock(struct platform_data *pdata,
                                  bool high_speed_en, bool ecc_clk_en)
{

}

void nandx_platform_unprepare_clock(struct platform_data *pdata,
                                    bool high_speed_en, bool ecc_clk_en)
{

}

int nandx_platform_init(struct platform_data *pdata)
{
    nandx_platform_set_gpio();
    return 0;
}

int nandx_platform_power_down(struct platform_data *pdata)
{
}
