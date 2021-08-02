#ifndef _GPIO_H_
#define _GPIO_H_

/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
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

/******************************************************************************
 * mt6516_gpio.h - The file is the gpio header file !
 *
 * Copyright 2008-2009 MediaTek Co.,Ltd.
 *
 * DESCRIPTION: -
 *     This file expose an in-kernel gpio modules to other device drivers
 *
 * modification history
 * ----------------------------------------
 * 01a, 08 oct 2008, Koshi,Chiu written
 * ----------------------------------------
 ******************************************************************************/
//#include <typedefs.h>
#include <cust_gpio_usage.h>
//#include <gpio_const.h>
#include <kernel/mutex.h>
#include <debug.h>
#include <platform/mt_typedefs.h>
//#include <platform/gpio_const.h>

#define PRELOADER_SUPPORT_EXT_GPIO 0

/*----------------------------------------------------------------------------*/
//  Error Code No.
#define RSUCCESS    0
#define ERACCESS    1
#define ERINVAL     2
#define ERWRAPPER   3
/*----------------------------------------------------------------------------*/

/******************************************************************************
* Enumeration for GPIO pin
******************************************************************************/
#define GPIO_MODE_BITS                 3
#define MAX_GPIO_MODE_PER_REG          8
#define MAX_GPIO_REG_BITS              32
#define MAX_GPIO_PIN                   144

typedef enum GPIO_PIN {
	GPIO_UNSUPPORTED = -1,

	GPIO0  , GPIO1  , GPIO2  , GPIO3  , GPIO4  , GPIO5  , GPIO6  , GPIO7  ,
	GPIO8  , GPIO9  , GPIO10 , GPIO11 , GPIO12 , GPIO13 , GPIO14 , GPIO15 ,
	GPIO16 , GPIO17 , GPIO18 , GPIO19 , GPIO20 , GPIO21 , GPIO22 , GPIO23 ,
	GPIO24 , GPIO25 , GPIO26 , GPIO27 , GPIO28 , GPIO29 , GPIO30 , GPIO31 ,
	GPIO32 , GPIO33 , GPIO34 , GPIO35 , GPIO36 , GPIO37 , GPIO38 , GPIO39 ,
	GPIO40 , GPIO41 , GPIO42 , GPIO43 , GPIO44 , GPIO45 , GPIO46 , GPIO47 ,
	GPIO48 , GPIO49 , GPIO50 , GPIO51 , GPIO52 , GPIO53 , GPIO54 , GPIO55 ,
	GPIO56 , GPIO57 , GPIO58 , GPIO59 , GPIO60 , GPIO61 , GPIO62 , GPIO63 ,
	GPIO64 , GPIO65 , GPIO66 , GPIO67 , GPIO68 , GPIO69 , GPIO70 , GPIO71 ,
	GPIO72 , GPIO73 , GPIO74 , GPIO75 , GPIO76 , GPIO77 , GPIO78 , GPIO79 ,
	GPIO80 , GPIO81 , GPIO82 , GPIO83 , GPIO84 , GPIO85 , GPIO86 , GPIO87 ,
	GPIO88 , GPIO89 , GPIO90 , GPIO91 , GPIO92 , GPIO93 , GPIO94 , GPIO95 ,
	GPIO96 , GPIO97 , GPIO98 , GPIO99 , GPIO100, GPIO101, GPIO102, GPIO103,
	GPIO104, GPIO105, GPIO106, GPIO107, GPIO108, GPIO109, GPIO110, GPIO111,
	GPIO112, GPIO113, GPIO114, GPIO115, GPIO116, GPIO117, GPIO118, GPIO119,
	GPIO120, GPIO121, GPIO122, GPIO123, GPIO124, GPIO125, GPIO126, GPIO127,
	GPIO128, GPIO129, GPIO130, GPIO131, GPIO132, GPIO133, GPIO134, GPIO135,
	GPIO136, GPIO137, GPIO138, GPIO139, GPIO140, GPIO141, GPIO142, GPIO143,
	GPIO144,
	MT_GPIO_BASE_MAX
} GPIO_PIN;

#define MAX_GPIO_PIN    (MT_GPIO_BASE_MAX)

/******************************************************************************
* Enumeration for GPIO pin
******************************************************************************/
/* GPIO MODE CONTROL VALUE*/
typedef enum {
	GPIO_MODE_UNSUPPORTED = -1,
	GPIO_MODE_GPIO  = 0,
	GPIO_MODE_00    = 0,
	GPIO_MODE_01    = 1,
	GPIO_MODE_02    = 2,
	GPIO_MODE_03    = 3,
	GPIO_MODE_04    = 4,
	GPIO_MODE_05    = 5,
	GPIO_MODE_06    = 6,
	GPIO_MODE_07    = 7,

	GPIO_MODE_MAX,
	GPIO_MODE_DEFAULT = GPIO_MODE_01,
} GPIO_MODE;
/*----------------------------------------------------------------------------*/
/* GPIO DIRECTION */
typedef enum {
    GPIO_DIR_UNSUPPORTED = -1,
    GPIO_DIR_IN     = 0,
    GPIO_DIR_OUT    = 1,

    GPIO_DIR_MAX,
    GPIO_DIR_DEFAULT = GPIO_DIR_IN,
} GPIO_DIR;
/*----------------------------------------------------------------------------*/
/* GPIO PULL ENABLE*/
typedef enum {
    GPIO_PULL_EN_UNSUPPORTED = -1,
    GPIO_NOPULLUP		= -4,
    GPIO_NOPULLDOWN		= -5,
    GPIO_PULL_DISABLE = 0,
    GPIO_PULL_ENABLE  = 1,

    GPIO_PULL_EN_MAX,
    GPIO_PULL_EN_DEFAULT = GPIO_PULL_ENABLE,
} GPIO_PULL_EN;
/*----------------------------------------------------------------------------*/
/* GPIO SMT*/
typedef enum {
    GPIO_SMT_UNSUPPORTED = -1,
    GPIO_SMT_DISABLE = 0,
    GPIO_SMT_ENABLE  = 1,

    GPIO_SMT_MAX,
    GPIO_SMT_DEFAULT = GPIO_SMT_ENABLE,
} GPIO_SMT;
/*----------------------------------------------------------------------------*/
/* GPIO IES*/
typedef enum {
    GPIO_IES_UNSUPPORTED = -1,
    GPIO_IES_DISABLE = 0,
    GPIO_IES_ENABLE  = 1,

    GPIO_IES_MAX,
    GPIO_IES_DEFAULT = GPIO_IES_ENABLE,
} GPIO_IES;
/*----------------------------------------------------------------------------*/
/* GPIO PULL-UP/PULL-DOWN*/
typedef enum {
    GPIO_PULL_UNSUPPORTED = -1,
    GPIO_PULL_DOWN  = 0,
    GPIO_PULL_UP    = 1,
    GPIO_NO_PULL = 2,

    GPIO_PULL_MAX,
    GPIO_PULL_DEFAULT = GPIO_PULL_DOWN
} GPIO_PULL;
/*----------------------------------------------------------------------------*/
/* GPIO OUTPUT */
typedef enum {
    GPIO_OUT_UNSUPPORTED = -1,
    GPIO_OUT_ZERO = 0,
    GPIO_OUT_ONE  = 1,

    GPIO_OUT_MAX,
    GPIO_OUT_DEFAULT = GPIO_OUT_ZERO,
    GPIO_DATA_OUT_DEFAULT = GPIO_OUT_ZERO,  /*compatible with DCT*/
} GPIO_OUT;

/*----------------------------------------------------------------------------*/
typedef struct {
	unsigned int no     : 16;
	unsigned int mode   : 3;
	unsigned int pullsel: 1;
	unsigned int din    : 1;
	unsigned int dout   : 1;
	unsigned int pullen : 1;
	unsigned int dir    : 1;
	unsigned int dinv   : 1;
	unsigned int _align : 7;
} GPIO_CFG;

/*-------for msdc pupd-----------*/
struct msdc_pupd {
	unsigned short reg;
	unsigned char   bit;
};

/******************************************************************************
* GPIO Driver interface
******************************************************************************/
/*direction*/
s32 mt_set_gpio_dir(u32 pin, u32 dir);
s32 mt_get_gpio_dir(u32 pin);

/*pull enable*/
s32 mt_set_gpio_pull_enable(u32 pin, u32 enable);
s32 mt_get_gpio_pull_enable(u32 pin);
/*pull select*/
s32 mt_set_gpio_pull_select(u32 pin, u32 select);
s32 mt_get_gpio_pull_select(u32 pin);

/*schmitt trigger*/
s32 mt_set_gpio_smt(u32 pin, u32 enable);
s32 mt_get_gpio_smt(u32 pin);

/*IES*/
s32 mt_set_gpio_ies(u32 pin, u32 enable);
s32 mt_get_gpio_ies(u32 pin);

/*input/output*/
s32 mt_set_gpio_out(u32 pin, u32 output);
s32 mt_get_gpio_out(u32 pin);
s32 mt_get_gpio_in(u32 pin);

/*mode control*/
s32 mt_set_gpio_mode(u32 pin, u32 mode);
s32 mt_get_gpio_mode(u32 pin);

S32 mt_set_msdc_r0_r1_value(u32 pin, u32 value);
S32 mt_get_msdc_r0_r1_value(u32 pin);

void mt_gpio_set_driving(u32 pin, unsigned char driving);

void mt_gpio_init(void);
void mt_gpio_set_default(void);
#endif //_GPIO_H_
