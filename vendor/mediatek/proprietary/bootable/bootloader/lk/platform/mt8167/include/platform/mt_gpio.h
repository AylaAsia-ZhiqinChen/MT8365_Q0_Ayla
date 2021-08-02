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

typedef enum GPIO_PIN {
	GPIO_UNSUPPORTED = -1,
	GPIO0 ,
	GPIO1  , GPIO2  , GPIO3  , GPIO4  , GPIO5  , GPIO6  , GPIO7  ,
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
	GPIO120, GPIO121, GPIO122, GPIO123, GPIO124,
#if PRELOADER_SUPPORT_EXT_GPIO
	GPIOEXT0, GPIOEXT1, GPIOEXT2, GPIOEXT3, GPIOEXT4, GPIOEXT5, GPIOEXT6,
#endif
	MT_GPIO_BASE_MAX
} GPIO_PIN;

#define MAX_GPIO_PIN    (MT_GPIO_BASE_MAX)

#if PRELOADER_SUPPORT_EXT_GPIO
#define GPIO_EXTEND_START GPIOEXT0
#if 0//mark to avoid other module set gpio incorrectly, find problem in build phase
typedef enum GPIO_PIN_EXT {
	GPIO135 = GPIO_EXTEND_START,
	GPIO136, GPIO137, GPIO138, GPIO139,
	GPIO140, GPIO141, GPIO142, GPIO143, GPIO144,
	GPIO145, GPIO146, GPIO147, GPIO148, GPIO149,
	GPIO150, GPIO151, GPIO152, GPIO153, GPIO154,
	GPIO155, GPIO156, GPIO157, GPIO158, GPIO159,
	GPIO160, GPIO161, GPIO162, GPIO163, GPIO164,
	GPIO165, GPIO166, GPIO167, GPIO168, GPIO169,
	GPIO170, GPIO171, GPIO172, GPIO173, GPIO174,
	GPIO175
} GPIO_PIN_EXT;
#endif
#endif

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
	GPIO_PULL_DISABLE = 0,
	GPIO_PULL_ENABLE  = 1,

	GPIO_PULL_EN_MAX,
	GPIO_PULL_EN_DEFAULT = GPIO_PULL_ENABLE,
} GPIO_PULL_EN;
/*----------------------------------------------------------------------------*/
/* GPIO PULL-UP/PULL-DOWN*/
typedef enum {
	GPIO_PULL_UNSUPPORTED = -1,
	GPIO_PULL_DOWN  = 0,
	GPIO_PULL_UP    = 1,

	GPIO_PULL_MAX,
	GPIO_PULL_DEFAULT = GPIO_PULL_DOWN
} GPIO_PULL;
/*----------------------------------------------------------------------------*/
typedef enum {
	GPIO_DATA_INV_UNSUPPORTED = -1,
	GPIO_DATA_UNINV = 0,
	GPIO_DATA_INV   = 1,

	GPIO_DATA_INV_MAX,
	GPIO_DATA_INV_DEFAULT = GPIO_DATA_UNINV
} GPIO_INVERSION;
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
/* GPIO INPUT */
typedef enum {
	GPIO_IN_UNSUPPORTED = -1,
	GPIO_IN_ZERO = 0,
	GPIO_IN_ONE  = 1,

	GPIO_IN_MAX,
} GPIO_IN;

/*----------------------------------------------------------------------------*/
/* GPIO POWER*/
typedef enum {
	GPIO_VIO28 = 0,
	GPIO_VIO18 = 1,

	GPIO_VIO_MAX,
} GPIO_POWER;
/*----------------------------------------------------------------------------*/
typedef struct {
	unsigned short val;
	unsigned short _align1;
	unsigned short set;
	unsigned short _align2;
	unsigned short rst;
	unsigned short _align3[3];
} VAL_REGS;
/*----------------------------------------------------------------------------*/
typedef struct {
	VAL_REGS dir[8];         /*0x0000 ~ 0x0070: 8*16   bytes*/
	u8       rsv00[128];     /*0x0080 ~ 0x00FF: 8*16  bytes*/
	VAL_REGS dout[8];            /*0x0100 ~ 0x0170: 8*16   bytes*/
	u8       rsv01[128];     /*0x0180 ~ 0x01FF: 8*16  bytes*/
	VAL_REGS din[8];         /*0x0200 ~ 0x0270: 8*16   bytes*/
	u8       rsv02[128];     /*0x0280 ~ 0x02FF: 8*16  bytes*/
	VAL_REGS mode[25];       /*0x0300 ~ 0x0480: 25*16   bytes*/
	u8       rsv03[112];     /*0x0490 ~ 0x04FF: 7*16  bytes*/
	VAL_REGS pullen[8];      /*0x0500 ~ 0x0570: 8*16   bytes*/
	u8       rsv04[128];     /*0x0580 ~ 0x05FF: 8*16  bytes*/
	VAL_REGS pullsel[8];     /*0x0600 ~ 0x0670: 8*16   bytes*/
	u8       rsv05[640];     /*0x0680 ~ 0x08FF: 40*16     bytes*/
	VAL_REGS ies[4];         /*0x0900 ~ 0x0930: 4*16   bytes*/
	u8       rsv06[64];      /*0x0940 ~ 0x097F: 4*16  bytes*/
	VAL_REGS misc;           /*0x0980 ~ 0x098F: 1*16   bytes*/
	VAL_REGS conn;           /*0x0990 ~ 0x099F: 1*16   bytes*/
	VAL_REGS ag;             /*0x09A0 ~ 0x09AF: 1*16   bytes*/
	VAL_REGS ags;            /*0x09B0 ~ 0x09BF: 1*16   bytes*/
	u8       rsv07[64];      /*0x09C0 ~ 0x09FF: 4*16  bytes*/
	VAL_REGS smt[4];         /*0x0A00 ~ 0x0A30: 4*16   bytes*/
	u8       rsv08[192];     /*0x0A40 ~ 0x0AFF: 12*16  bytes*/
	VAL_REGS tdsel[8];       /*0x0B00 ~ 0x0B70:  8*16   bytes*/
	u8       rsv09[128];     /*0x0B80 ~ 0x0BFF: 8*16  bytes*/
	VAL_REGS rdsel[8];       /*0x0C00 ~ 0x0C70:  8*16   bytes*/
	u8       rsv010[128];        /*0x0C80 ~ 0x0CFF: 8*16  bytes*/
	VAL_REGS drv_mode[8];        /*0x0D00 ~ 0x0D70:  8*16   bytes*/
	u8       rsv011[128];        /*0x0D80 ~ 0x0DFF: 8*16  bytes*/
	VAL_REGS pupd_ctrl[10];  /*0x0E00 ~ 0x0E90:  10*16   bytes*/
	u8       rsv012[96];     /*0x0D80 ~ 0x0DFF: 6*16  bytes*/
	VAL_REGS flash_mac_ctrl0;    /*0x0F00 ~ 0x0F0F:  1*16   bytes*/
	VAL_REGS flash_mac_ctrl1;    /*0x0F10 ~ 0x0F1F:  1*16   bytes*/
	VAL_REGS dbg_sel;        /*0x0F20 ~ 0x0F2F:  1*16   bytes*/
	VAL_REGS msdc_gpio_sel0;     /*0x0F30 ~ 0x0F3F:  1*16   bytes*/
	VAL_REGS msdc_gpio_sel1;     /*0x0F40 ~ 0x0F4F:  1*16   bytes*/
	VAL_REGS gpio_bank;      /*0x0F50 ~ 0x0F5F:  1*16   bytes*/
	VAL_REGS gpio_tm;        /*0x0F60 ~ 0x0F6F:  1*16   bytes*/
	VAL_REGS bias_ctrl0;     /*0x0F70 ~ 0x0F7F:  1*16   bytes*/
	VAL_REGS bias_ctrl1;     /*0x0F80 ~ 0x0F8F:  1*16   bytes*/
	VAL_REGS bias_ctrl2;     /*0x0F90 ~ 0x0F9F:  1*16   bytes*/
	u8       rsv013[96];     /*0x0FA0 ~ 0x0FFF: 6*16  bytes*/
} GPIO_REGS;
/*----------------------------------------------------------------------------*/
#if PRELOADER_SUPPORT_EXT_GPIO
typedef struct {
	unsigned short val;
	unsigned short set;
	unsigned short rst;
	unsigned short _align;
} EXT_VAL_REGS;
/*----------------------------------------------------------------------------*/
typedef struct {
	EXT_VAL_REGS    dir[1];            /*0x0000 ~ 0x001F: 32 bytes*/
	EXT_VAL_REGS    pullen[1];         /*0x0020 ~ 0x003F: 32 bytes*/
	EXT_VAL_REGS    pullsel[1];        /*0x0040 ~ 0x005F: 32 bytes*/
	EXT_VAL_REGS    dinv[1];           /*0x0060 ~ 0x007F: 32 bytes*/
	EXT_VAL_REGS    dout[1];           /*0x0080 ~ 0x009F: 32 bytes*/
	EXT_VAL_REGS    din[1];            /*0x00A0 ~ 0x00BF: 32 bytes*/
	EXT_VAL_REGS    mode[2];          /*0x00C0 ~ 0x0CF: 16 bytes*/
} GPIOEXT_REGS;
#endif
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
