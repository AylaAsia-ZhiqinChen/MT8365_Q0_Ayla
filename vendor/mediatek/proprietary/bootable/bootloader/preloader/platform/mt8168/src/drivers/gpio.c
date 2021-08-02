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
 * gpio.c - MTK Linux GPIO Device Driver
 * 
 * Copyright 2008-2009 MediaTek Co.,Ltd.
 * 
 * DESCRIPTION:
 *     This file provid the other drivers GPIO relative functions
 *
 ******************************************************************************/

#include <typedefs.h>
#include <gpio.h>
#include <platform.h>
//#include <pmic_wrap_init.h>
//autogen
#include <gpio_cfg.h>

//#include <cust_power.h>
/******************************************************************************
 MACRO Definition
******************************************************************************/
//#define  GIO_SLFTEST
#define GPIO_DEVICE "mt-gpio"
#define VERSION     "$Revision$"
/*---------------------------------------------------------------------------*/
#define GPIO_WR32(addr, data)   __raw_writel(data, addr)
#define GPIO_RD32(addr)         __raw_readl(addr)
#define GPIO_SET_BITS(BIT,REG)   ((*(volatile u32*)(REG)) = (u32)(BIT))
#define GPIO_CLR_BITS(BIT,REG)   ((*(volatile u32*)(REG)) &= ~((u32)(BIT)))
//S32 pwrap_read( U32  adr, U32 *rdata ){return 0;}
//S32 pwrap_write( U32  adr, U32  wdata ){return 0;}
#if PRELOADER_SUPPORT_EXT_GPIO
#define GPIOEXT_WR(addr, data)   pwrap_write((u32)addr, data)
#define GPIOEXT_RD(addr)         ({ \
		u32 ext_data; \
		int ret; \
		ret = pwrap_read((u32)addr,&ext_data); \
		(ret != 0)?-1:ext_data;})
#define GPIOEXT_SET_BITS(BIT,REG)   (GPIOEXT_WR(REG, (u32)(BIT)))
#define GPIOEXT_CLR_BITS(BIT,REG)    ({ \
		u32 ext_data; \
		int ret; \
		ret = GPIOEXT_RD(REG);\
		ext_data = ret;\
		(ret < 0)?-1:(GPIOEXT_WR(REG,ext_data & ~((u32)(BIT))))})
#define GPIOEXT_BASE        (0xC000) 			//PMIC GPIO base.
//#define RG_USBDL_EN_CTL_REG  (0x502)
#endif
/*---------------------------------------------------------------------------*/
#define TRUE                   1
#define FALSE                  0
/*---------------------------------------------------------------------------*/
#define MAX_GPIO_REG_BITS      16
#define MAX_GPIO_MODE_PER_REG  5
#define GPIO_MODE_BITS         3
/*---------------------------------------------------------------------------*/
#define GPIOTAG                "[GPIO] "
#define GPIOLOG(fmt, arg...)   //printf(GPIOTAG fmt, ##arg)
#define GPIOMSG(fmt, arg...)   //printf(fmt, ##arg)
#define GPIOERR(fmt, arg...)   //printf(GPIOTAG "%5d: "fmt, __LINE__, ##arg)
#define GPIOFUC(fmt, arg...)   //printf(GPIOTAG "%s\n", __FUNCTION__)
#define GIO_INVALID_OBJ(ptr)   ((ptr) != gpio_obj)
/******************************************************************************

/************************
R0:bit0/bit4/bit8/bit12
R1:bit1/bit5/bit9/bit13
PUPD:bit2/bit6/bit10/bit14;0=pull-up   1=pull-down
************************/
#define MSDC0_MIN_PIN GPIO14
#define MSDC0_MAX_PIN GPIO17
#define MSDC1_MIN_PIN GPIO21
#define MSDC1_MAX_PIN GPIO23
#define MSDC2_MIN_PIN GPIO40
#define MSDC2_MAX_PIN GPIO43
#define MSDC3_MIN_PIN GPIO68
#define MSDC3_MAX_PIN GPIO73
#define MSDC4_MIN_PIN GPIO104
#define MSDC4_MAX_PIN GPIO120

#define MSDC0_PIN_NUM (MSDC0_MAX_PIN - MSDC0_MIN_PIN + 1)
#define MSDC1_PIN_NUM (MSDC1_MAX_PIN - MSDC1_MIN_PIN + 1)
#define MSDC2_PIN_NUM (MSDC2_MAX_PIN - MSDC2_MIN_PIN + 1)
#define MSDC3_PIN_NUM (MSDC3_MAX_PIN - MSDC3_MIN_PIN + 1)
#define MSDC4_PIN_NUM (MSDC4_MAX_PIN - MSDC4_MIN_PIN + 1)

static const struct msdc_pupd msdc_pupd_spec[]=
{
    /*msdc0 pin*/
    {0xE50, 12},
    {0xE60, 0},
    {0xE60, 4},
    {0xE60, 8},
    /*msdc1 pin*/
    {0xE60, 12},
    {0xE70, 0},
	{0xE70, 4},
    /*msdc2 pin*/
    {0xE80, 0},
	{0xE80, 4},
	{0xE90, 0},
	{0xE90, 4},
    /*msdc3 pin*/
    {0xE50, 8},
    {0xE50, 4},
    {0xE40, 4},
    {0xE40, 8},
	{0xE40, 12},
    {0xE50, 0},
    /*msdc4 pin*/
    {0xE40, 0},
	{0xE30, 12},
    {0xE20, 12},
    {0xE30, 0},
    {0xE30, 4},
    {0xE30, 8},
    {0xE10, 12},
    {0xE10, 8},
    {0xE10, 4},
    {0xE10, 0},
    {0xE20, 8},
    {0xE20, 0},
    {0xE20, 4},
    {0xE00, 12},
    {0xE00, 8},
	{0xE00, 4},
	{0xE00, 0},
};
/*---------------------------------------*/

#define PUPD_OFFSET_TO_REG(msdc_pupd_ctrl) ((VAL_REGS*)(GPIO_BASE + msdc_pupd_ctrl->reg))


const struct msdc_pupd * mt_get_msdc_ctrl(unsigned long pin)
{
    unsigned int idx = 255;
    if ((pin >= MSDC0_MIN_PIN) && (pin <= MSDC0_MAX_PIN)){
        idx = pin - MSDC0_MIN_PIN;
    } else if ((pin >= MSDC1_MIN_PIN) && (pin <= MSDC1_MAX_PIN)){
        idx = MSDC0_PIN_NUM + pin-MSDC1_MIN_PIN;
    } else if ((pin >= MSDC2_MIN_PIN) && (pin <= MSDC2_MAX_PIN)){
        idx = MSDC0_PIN_NUM + MSDC1_PIN_NUM + pin-MSDC2_MIN_PIN;
    } else if ((pin >= MSDC3_MIN_PIN) && (pin <= MSDC3_MAX_PIN)){
        idx = MSDC0_PIN_NUM + MSDC1_PIN_NUM + MSDC2_PIN_NUM + pin-MSDC3_MIN_PIN;
    } else if ((pin >= MSDC4_MIN_PIN) && (pin <= MSDC4_MAX_PIN)){
        idx = MSDC0_PIN_NUM + MSDC1_PIN_NUM + MSDC2_PIN_NUM + MSDC3_PIN_NUM + pin-MSDC4_MIN_PIN;
	} else {
        return NULL;
    }
    return &msdc_pupd_spec[idx];
}

struct mt_gpio_obj {
    GPIO_REGS       *reg;
};
static struct mt_gpio_obj gpio_dat = {
    .reg  = (GPIO_REGS*)(GPIO_BASE),
};
static struct mt_gpio_obj *gpio_obj = &gpio_dat;

#if PRELOADER_SUPPORT_EXT_GPIO
struct mt_gpioext_obj {
	GPIOEXT_REGS	*reg;
};

static struct mt_gpioext_obj gpioext_dat = {
	.reg = (GPIOEXT_REGS*)(GPIOEXT_BASE),
};
static struct mt_gpioext_obj *gpioext_obj = &gpioext_dat;
#endif
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_dir_chip(u32 pin, u32 dir)
{
    u32 pos;
    u32 bit;
    struct mt_gpio_obj *obj = gpio_obj;

    if (!obj)
        return -ERACCESS;

#if PRELOADER_SUPPORT_EXT_GPIO
	if (pin >= GPIO_EXTEND_START)
		return -ERINVAL;
#else
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;
#endif

    if (dir >= GPIO_DIR_MAX)
        return -ERINVAL;

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    if (dir == GPIO_DIR_IN)
        GPIO_SET_BITS((1L << bit), &obj->reg->dir[pos].rst);
    else
        GPIO_SET_BITS((1L << bit), &obj->reg->dir[pos].set);
    return RSUCCESS;

}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_dir_chip(u32 pin)
{
    u32 pos;
    u32 bit;
    u32 reg;
    struct mt_gpio_obj *obj = gpio_obj;

    if (!obj)
        return -ERACCESS;

#if PRELOADER_SUPPORT_EXT_GPIO
	if (pin >= GPIO_EXTEND_START)
		return -ERINVAL;
#else
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;
#endif

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIO_RD32(&obj->reg->dir[pos].val);
    return (((reg & (1L << bit)) != 0)? 1: 0);
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_pull_enable_chip(u32 pin, u32 enable)
{
    u32 pos;
    u32 bit;

    struct mt_gpio_obj *obj = gpio_obj;
    const struct msdc_pupd * msdc_pupd_ctrl;

    if (!obj)
        return -ERACCESS;

#if PRELOADER_SUPPORT_EXT_GPIO
	if (pin >= GPIO_EXTEND_START)
		return -ERINVAL;
#else
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;
#endif

    if (enable >= GPIO_PULL_EN_MAX)
        return -ERINVAL;

    /*for special msdc pupd*/
    if(NULL != (msdc_pupd_ctrl = mt_get_msdc_ctrl(pin)))
    {
        if (enable == GPIO_PULL_DISABLE){
            GPIO_SET_BITS((1L << (msdc_pupd_ctrl->bit)), &(PUPD_OFFSET_TO_REG(msdc_pupd_ctrl)->rst));
            GPIO_SET_BITS((1L << (msdc_pupd_ctrl->bit + 1)), &(PUPD_OFFSET_TO_REG(msdc_pupd_ctrl)->rst));
        }
        else {
            GPIO_SET_BITS((1L << (msdc_pupd_ctrl->bit)), &((PUPD_OFFSET_TO_REG(msdc_pupd_ctrl))->set));
            GPIO_SET_BITS((1L << (msdc_pupd_ctrl->bit + 1)), &((PUPD_OFFSET_TO_REG(msdc_pupd_ctrl))->rst));
        }
        return RSUCCESS;
    }

		pos = pin / MAX_GPIO_REG_BITS;
		bit = pin % MAX_GPIO_REG_BITS;

		if (enable == GPIO_PULL_DISABLE)
			GPIO_SET_BITS((1L << bit), &obj->reg->pullen[pos].rst);
		else
			GPIO_SET_BITS((1L << bit), &obj->reg->pullen[pos].set);
	return RSUCCESS;

}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_pull_enable_chip(u32 pin)
{
	u32 pos;
	u32 bit;
	u32 reg=0;
	struct mt_gpio_obj *obj = gpio_obj;
	const struct msdc_pupd *msdc_pupd_ctrl;
	unsigned long data=0;

	if (!obj)
		return -ERACCESS;

#if PRELOADER_SUPPORT_EXT_GPIO
	if (pin >= GPIO_EXTEND_START)
		return -ERINVAL;
#else
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;
#endif

	/*for special msdc pupd*/
	if(NULL != (msdc_pupd_ctrl = mt_get_msdc_ctrl(pin)))
	{
		data = GPIO_RD32(&PUPD_OFFSET_TO_REG(msdc_pupd_ctrl)->val);
		return (((data & (3L << (msdc_pupd_ctrl->bit))) == 0)? 0: 1);
	}


	pos = pin / MAX_GPIO_REG_BITS;
	bit = pin % MAX_GPIO_REG_BITS;

	reg = GPIO_RD32(&obj->reg->pullen[pos].val);
	return (((reg & (1L << bit)) != 0)? 1: 0);
}

/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_pull_select_chip(u32 pin, u32 select)
{
	u32 pos;
	u32 bit;
	struct mt_gpio_obj *obj = gpio_obj;
	const struct msdc_pupd *msdc_pupd_ctrl;

	if (!obj)
		return -ERACCESS;

#if PRELOADER_SUPPORT_EXT_GPIO
	if (pin >= GPIO_EXTEND_START)
		return -ERINVAL;
#else
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;
#endif

	if (select >= GPIO_PULL_MAX)
		return -ERINVAL;

	/*for special msdc pupd*/
	if(NULL != (msdc_pupd_ctrl = mt_get_msdc_ctrl(pin)))
	{
		if (select == GPIO_PULL_DOWN){
			GPIO_SET_BITS((1L << msdc_pupd_ctrl->bit + 2), &(PUPD_OFFSET_TO_REG(msdc_pupd_ctrl))->set);
		}
		else {
			GPIO_SET_BITS((1L << msdc_pupd_ctrl->bit + 2), &(PUPD_OFFSET_TO_REG(msdc_pupd_ctrl))->rst);
		}
		return RSUCCESS;
	}

	pos = pin / MAX_GPIO_REG_BITS;
	bit = pin % MAX_GPIO_REG_BITS;

	if (select == GPIO_PULL_DOWN)
		GPIO_SET_BITS((1L << bit), &obj->reg->pullsel[pos].rst);
	else
		GPIO_SET_BITS((1L << bit), &obj->reg->pullsel[pos].set);
	return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_pull_select_chip(u32 pin)
{
    u32 pos;
    u32 bit;
    u32 reg=0;
    struct mt_gpio_obj *obj = gpio_obj;
    const struct msdc_pupd *msdc_pupd_ctrl;
    unsigned long data=0;

    if (!obj)
        return -ERACCESS;

#if PRELOADER_SUPPORT_EXT_GPIO
	if (pin >= GPIO_EXTEND_START)
		return -ERINVAL;
#else
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;
#endif

    /*for special msdc pupd*/
    if(NULL != (msdc_pupd_ctrl = mt_get_msdc_ctrl(pin)))
    {
        data = GPIO_RD32(&((PUPD_OFFSET_TO_REG(msdc_pupd_ctrl))->val));
        return (((data & (1L << msdc_pupd_ctrl->bit + 2)) != 0)? 0: 1);
    }

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIO_RD32(&obj->reg->pullsel[pos].val);

    return (((reg & (1L << bit)) != 0)? 1: 0);

}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_out_chip(u32 pin, u32 output)
{

    u32 pos;
    u32 bit;
    struct mt_gpio_obj *obj = gpio_obj;

    if (!obj)
        return -ERACCESS;

#if PRELOADER_SUPPORT_EXT_GPIO
	if (pin >= GPIO_EXTEND_START)
		return -ERINVAL;
#else
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;
#endif

    if (output >= GPIO_OUT_MAX)
        return -ERINVAL;

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    if (output == GPIO_OUT_ZERO)
        GPIO_SET_BITS((1L << bit), &obj->reg->dout[pos].rst);
    else
        GPIO_SET_BITS((1L << bit), &obj->reg->dout[pos].set);
    return RSUCCESS;


}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_out_chip(u32 pin)
{
    u32 pos;
    u32 bit;
    u32 reg;
    struct mt_gpio_obj *obj = gpio_obj;

    if (!obj)
        return -ERACCESS;

#if PRELOADER_SUPPORT_EXT_GPIO
	if (pin >= GPIO_EXTEND_START)
		return -ERINVAL;
#else
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;
#endif

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIO_RD32(&obj->reg->dout[pos].val);
    return (((reg & (1L << bit)) != 0)? 1: 0);
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_in_chip(u32 pin)
{
    u32 pos;
    u32 bit;
    u32 reg;
    struct mt_gpio_obj *obj = gpio_obj;

    if (!obj)
        return -ERACCESS;

#if PRELOADER_SUPPORT_EXT_GPIO
	if (pin >= GPIO_EXTEND_START)
		return -ERINVAL;
#else
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;
#endif

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIO_RD32(&obj->reg->din[pos].val);
    return (((reg & (1L << bit)) != 0)? 1: 0);
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_mode_chip(u32 pin, u32 mode)
{
    u32 pos;
    u32 bit;
    u32 reg;
    u32 mask = (1L << GPIO_MODE_BITS) - 1;
    struct mt_gpio_obj *obj = gpio_obj;

    if (!obj)
        return -ERACCESS;

#if PRELOADER_SUPPORT_EXT_GPIO
	if (pin >= GPIO_EXTEND_START)
		return -ERINVAL;
#else
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;
#endif

    if (mode >= GPIO_MODE_MAX)
        return -ERINVAL;

	pos = pin / MAX_GPIO_MODE_PER_REG;
	bit = pin % MAX_GPIO_MODE_PER_REG;


	reg = GPIO_RD32(&obj->reg->mode[pos].val);

	reg &= ~(mask << (GPIO_MODE_BITS*bit));
	reg |= (mode << (GPIO_MODE_BITS*bit));

	GPIO_WR32(&obj->reg->mode[pos].val, reg);

    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_mode_chip(u32 pin)
{
    u32 pos;
    u32 bit;
    u32 reg;
    u32 mask = (1L << GPIO_MODE_BITS) - 1;
    struct mt_gpio_obj *obj = gpio_obj;

    if (!obj)
        return -ERACCESS;

#if PRELOADER_SUPPORT_EXT_GPIO
	if (pin >= GPIO_EXTEND_START)
		return -ERINVAL;
#else
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;
#endif

	pos = pin / MAX_GPIO_MODE_PER_REG;
	bit = pin % MAX_GPIO_MODE_PER_REG;

	reg = GPIO_RD32(&obj->reg->mode[pos].val);

	return ((reg >> (GPIO_MODE_BITS*bit)) & mask);
}
/*---------------------------------------------------------------------------*/
#if PRELOADER_SUPPORT_EXT_GPIO
s32 mt_set_gpio_dir_ext(u32 pin, u32 dir)
{
    u32 pos;
    u32 bit;
	int ret=0;
    struct mt_gpioext_obj *obj = gpioext_obj;

    if (!obj)
        return -ERACCESS;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;

    if (dir >= GPIO_DIR_MAX)
        return -ERINVAL;

    pin -= GPIO_EXTEND_START;
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    if (dir == GPIO_DIR_IN)
        ret=GPIOEXT_SET_BITS((1L << bit), &obj->reg->dir[pos].rst);
    else
        ret=GPIOEXT_SET_BITS((1L << bit), &obj->reg->dir[pos].set);
	if(ret!=0) return -ERWRAPPER;
    return RSUCCESS;

}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_dir_ext(u32 pin)
{
    u32 pos;
    u32 bit;
    s64 reg;
    struct mt_gpioext_obj *obj = gpioext_obj;

    if (!obj)
        return -ERACCESS;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;

	pin -= GPIO_EXTEND_START;
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIOEXT_RD(&obj->reg->dir[pos].val);
    if(reg < 0) return -ERWRAPPER;
    return (((reg & (1L << bit)) != 0)? 1: 0);
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_pull_enable_ext(u32 pin, u32 enable)
{
    u32 pos;
    u32 bit;
	int ret=0;
    struct mt_gpioext_obj *obj = gpioext_obj;

    if (!obj)
        return -ERACCESS;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;

    if (enable >= GPIO_PULL_EN_MAX)
        return -ERINVAL;

	pin -= GPIO_EXTEND_START;
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    if (enable == GPIO_PULL_DISABLE)
        ret=GPIOEXT_SET_BITS((1L << bit), &obj->reg->pullen[pos].rst);
    else
        ret=GPIOEXT_SET_BITS((1L << bit), &obj->reg->pullen[pos].set);
	if(ret!=0) return -ERWRAPPER;
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_pull_enable_ext(u32 pin)
{
    u32 pos;
    u32 bit;
    s64 reg;
    struct mt_gpioext_obj *obj = gpioext_obj;

    if (!obj)
        return -ERACCESS;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;

	pin -= GPIO_EXTEND_START;    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIOEXT_RD(&obj->reg->pullen[pos].val);
    if(reg < 0) return -ERWRAPPER;
    return (((reg & (1L << bit)) != 0)? 1: 0);        
}
s32 mt_set_gpio_pull_select_ext(u32 pin, u32 select)
{
    u32 pos;
    u32 bit;
	int ret=0;
    struct mt_gpioext_obj *obj = gpioext_obj;

    if (!obj)
        return -ERACCESS;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;
    
    if (select >= GPIO_PULL_MAX)
        return -ERINVAL;

	pin -= GPIO_EXTEND_START;    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;
    
    if (select == GPIO_PULL_DOWN)
        ret=GPIOEXT_SET_BITS((1L << bit), &obj->reg->pullsel[pos].rst);
    else
        ret=GPIOEXT_SET_BITS((1L << bit), &obj->reg->pullsel[pos].set);
	if(ret!=0) return -ERWRAPPER;
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_pull_select_ext(u32 pin)
{
    u32 pos;
    u32 bit;
    s64 reg;
    struct mt_gpioext_obj *obj = gpioext_obj;

    if (!obj)
        return -ERACCESS;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;
    
	pin -= GPIO_EXTEND_START;    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIOEXT_RD(&obj->reg->pullsel[pos].val);
    if(reg < 0) return -ERWRAPPER;
    return (((reg & (1L << bit)) != 0)? 1: 0);        
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_inversion_ext(u32 pin, u32 enable)
{
    u32 pos;
    u32 bit;
	int ret = 0;
    struct mt_gpioext_obj *obj = gpioext_obj;

    if (!obj)
        return -ERACCESS;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;

    if (enable >= GPIO_DATA_INV_MAX)
        return -ERINVAL;

	pin -= GPIO_EXTEND_START;    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;
    
    if (enable == GPIO_DATA_UNINV)
        ret=GPIOEXT_SET_BITS((1L << bit), &obj->reg->dinv[pos].rst);
    else
        ret=GPIOEXT_SET_BITS((1L << bit), &obj->reg->dinv[pos].set);
	if(ret!=0) return -ERWRAPPER;
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_inversion_ext(u32 pin)
{
    u32 pos;
    u32 bit;
    s64 reg;
    struct mt_gpioext_obj *obj = gpioext_obj;

    if (!obj)
        return -ERACCESS;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;
    
	pin -= GPIO_EXTEND_START;    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIOEXT_RD(&obj->reg->dinv[pos].val);
    if(reg < 0) return -ERWRAPPER;
    return (((reg & (1L << bit)) != 0)? 1: 0);
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_out_ext(u32 pin, u32 output)
{
    u32 pos;
    u32 bit;
	int ret = 0;
    struct mt_gpioext_obj *obj = gpioext_obj;

    if (!obj)
        return -ERACCESS;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;

    if (output >= GPIO_OUT_MAX)
        return -ERINVAL;
    
	pin -= GPIO_EXTEND_START;    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;
    
    if (output == GPIO_OUT_ZERO)
        ret=GPIOEXT_SET_BITS((1L << bit), &obj->reg->dout[pos].rst);
    else
        ret=GPIOEXT_SET_BITS((1L << bit), &obj->reg->dout[pos].set);
	if(ret!=0) return -ERWRAPPER;
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_out_ext(u32 pin)
{
    u32 pos;
    u32 bit;
    s64 reg;
    struct mt_gpioext_obj *obj = gpioext_obj;

    if (!obj)
        return -ERACCESS;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;
    
	pin -= GPIO_EXTEND_START;    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIOEXT_RD(&obj->reg->dout[pos].val);
    if(reg < 0) return -ERWRAPPER;
    return (((reg & (1L << bit)) != 0)? 1: 0);        
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_in_ext(u32 pin)
{
    u32 pos;
    u32 bit;
    s64 reg;
    struct mt_gpioext_obj *obj = gpioext_obj;

    if (!obj)
        return -ERACCESS;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;
    
	pin -= GPIO_EXTEND_START;    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIOEXT_RD(&obj->reg->din[pos].val);
    if(reg < 0) return -ERWRAPPER;
    return (((reg & (1L << bit)) != 0)? 1: 0);        
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_mode_ext(u32 pin, u32 mode)
{
    u32 pos;
    u32 bit;
    s64 reg;
	int ret=0;
    S32 data;
    u32 mask = (1L << GPIO_MODE_BITS) - 1;    
    struct mt_gpioext_obj *obj = gpioext_obj;

    if (!obj)
        return -ERACCESS;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;

    if (mode >= GPIO_MODE_MAX)
        return -ERINVAL;
    
	pin -= GPIO_EXTEND_START;    
    pos = pin / MAX_GPIO_MODE_PER_REG;
    bit = pin % MAX_GPIO_MODE_PER_REG;
   
    reg = GPIOEXT_RD(&obj->reg->mode[pos].val);
    if(reg < 0) return -ERWRAPPER;

    reg &= ~(mask << (GPIO_MODE_BITS*bit));
    reg |= (mode << (GPIO_MODE_BITS*bit));
    
    ret = GPIOEXT_WR(&obj->reg->mode[pos].val, reg);
    if(ret!=0) return -ERWRAPPER;
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_mode_ext(u32 pin)
{
    u32 pos;
    u32 bit;
    s64 reg;
    u32 mask = (1L << GPIO_MODE_BITS) - 1;    
    struct mt_gpioext_obj *obj = gpioext_obj;

    if (!obj)
        return -ERACCESS;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;
    
	pin -= GPIO_EXTEND_START;    
    pos = pin / MAX_GPIO_MODE_PER_REG;
    bit = pin % MAX_GPIO_MODE_PER_REG;

    reg = GPIOEXT_RD(&obj->reg->mode[pos].val);
    if(reg < 0) return -ERWRAPPER;
    
    return ((reg >> (GPIO_MODE_BITS*bit)) & mask);
}
#endif

/*set driving-------------------------------------------------------------------*/
struct mtk_drv_group_desc {
	unsigned char min_drv;
	unsigned char max_drv;
	unsigned char low_bit;
	unsigned char high_bit;
	unsigned char step;
};
struct mtk_pin_drv_grp {
	unsigned short pin;
	unsigned short offset;
	unsigned char bit;
	unsigned char grp;
};

#define MTK_PIN_DRV_GRP(_pin, _offset, _bit, _grp)	\
	{	\
		.pin = _pin,	\
		.offset = _offset,	\
		.bit = _bit,	\
		.grp = _grp,	\
	}

#define MTK_DRV_GRP(_min, _max, _low, _high, _step)	\
	{	\
		.min_drv = _min,	\
		.max_drv = _max,	\
		.low_bit = _low,	\
		.high_bit = _high,	\
		.step = _step,		\
	}

static const struct mtk_drv_group_desc mt8168_drv_grp[] =  {
	/* 0E4E8SR 4/8/12/16 */
	MTK_DRV_GRP(4, 16, 1, 2, 4),
	/* 0E2E4SR  2/4/6/8 */
	MTK_DRV_GRP(2, 8, 1, 2, 2),
	/* E8E4E2  2/4/6/8/10/12/14/16 */
	MTK_DRV_GRP(2, 16, 0, 2, 2)
};
#define DRV_GROUP_MAX 3

static const struct mtk_pin_drv_grp mt8168_pin_drv[] = {
	MTK_PIN_DRV_GRP(0, 0xd00, 0, 0),
	MTK_PIN_DRV_GRP(1, 0xd00, 0, 0),
	MTK_PIN_DRV_GRP(2, 0xd00, 0, 0),
	MTK_PIN_DRV_GRP(3, 0xd00, 0, 0),
	MTK_PIN_DRV_GRP(4, 0xd00, 0, 0),
	
	MTK_PIN_DRV_GRP(5, 0xd00, 4, 0),
	MTK_PIN_DRV_GRP(6, 0xd00, 4, 0),
	MTK_PIN_DRV_GRP(7, 0xd00, 4, 0),
	MTK_PIN_DRV_GRP(8, 0xd00, 4, 0),
	MTK_PIN_DRV_GRP(9, 0xd00, 4, 0),
	MTK_PIN_DRV_GRP(10, 0xd00, 4, 0),
	
	MTK_PIN_DRV_GRP(11, 0xd00, 8, 0),
	MTK_PIN_DRV_GRP(12, 0xd00, 8, 0),
	MTK_PIN_DRV_GRP(13, 0xd00, 8, 0),
	
	MTK_PIN_DRV_GRP(14, 0xd00, 12, 2),
	MTK_PIN_DRV_GRP(15, 0xd00, 12, 2),
	MTK_PIN_DRV_GRP(16, 0xd00, 12, 2),
	MTK_PIN_DRV_GRP(17, 0xd00, 12, 2),
	
	MTK_PIN_DRV_GRP(18, 0xd10, 0, 0),
	MTK_PIN_DRV_GRP(19, 0xd10, 0, 0),
	MTK_PIN_DRV_GRP(20, 0xd10, 0, 0),
	
	MTK_PIN_DRV_GRP(21, 0xd00, 12, 2),
	MTK_PIN_DRV_GRP(22, 0xd00, 12, 2),
	MTK_PIN_DRV_GRP(23, 0xd00, 12, 2),
	
	MTK_PIN_DRV_GRP(24, 0xd00, 8, 0),
	MTK_PIN_DRV_GRP(25, 0xd00, 8, 0),
	
	MTK_PIN_DRV_GRP(26, 0xd10, 4, 1),
	MTK_PIN_DRV_GRP(27, 0xd10, 4, 1),
	MTK_PIN_DRV_GRP(28, 0xd10, 4, 1),
	MTK_PIN_DRV_GRP(29, 0xd10, 4, 1),
	MTK_PIN_DRV_GRP(30, 0xd10, 4, 1),
	
	MTK_PIN_DRV_GRP(31, 0xd10, 8, 1),
	MTK_PIN_DRV_GRP(32, 0xd10, 8, 1),
	MTK_PIN_DRV_GRP(33, 0xd10, 8, 1),
	
	MTK_PIN_DRV_GRP(34, 0xd10, 12, 0),
	MTK_PIN_DRV_GRP(35, 0xd10, 12, 0),
	
	MTK_PIN_DRV_GRP(36, 0xd20, 0, 0),
	MTK_PIN_DRV_GRP(37, 0xd20, 0, 0),
	MTK_PIN_DRV_GRP(38, 0xd20, 0, 0),
	MTK_PIN_DRV_GRP(39, 0xd20, 0, 0),
	
	MTK_PIN_DRV_GRP(40, 0xd20, 4, 1),
	
	MTK_PIN_DRV_GRP(41, 0xd20, 8, 1),
	MTK_PIN_DRV_GRP(42, 0xd20, 8, 1),
	MTK_PIN_DRV_GRP(43, 0xd20, 8, 1),
	
	MTK_PIN_DRV_GRP(44, 0xd20, 12, 1),
	MTK_PIN_DRV_GRP(45, 0xd20, 12, 1),
	MTK_PIN_DRV_GRP(46, 0xd20, 12, 1),
	MTK_PIN_DRV_GRP(47, 0xd20, 12, 1),
	
	MTK_PIN_DRV_GRP(48, 0xd30, 0, 1),
	MTK_PIN_DRV_GRP(49, 0xd30, 0, 1),
	MTK_PIN_DRV_GRP(50, 0xd30, 0, 1),
	MTK_PIN_DRV_GRP(51, 0xd30, 0, 1),
	
	MTK_PIN_DRV_GRP(54, 0xd30, 8, 1),
	
	MTK_PIN_DRV_GRP(55, 0xd30, 12, 1),
	MTK_PIN_DRV_GRP(56, 0xd30, 12, 1),
	MTK_PIN_DRV_GRP(57, 0xd30, 12, 1),

	MTK_PIN_DRV_GRP(62, 0xd40, 8, 1),
	MTK_PIN_DRV_GRP(63, 0xd40, 8, 1),
	MTK_PIN_DRV_GRP(64, 0xd40, 8, 1),
	MTK_PIN_DRV_GRP(65, 0xd40, 8, 1),
	MTK_PIN_DRV_GRP(66, 0xd40, 8, 1),
	MTK_PIN_DRV_GRP(67, 0xd40, 8, 1),
	
	MTK_PIN_DRV_GRP(68, 0xd40, 12, 2),
	
	MTK_PIN_DRV_GRP(69, 0xd50, 0, 2),
	
	MTK_PIN_DRV_GRP(70, 0xd50, 4, 2),
	MTK_PIN_DRV_GRP(71, 0xd50, 4, 2),
	MTK_PIN_DRV_GRP(72, 0xd50, 4, 2),
	MTK_PIN_DRV_GRP(73, 0xd50, 4, 2),
	
	MTK_PIN_DRV_GRP(100, 0xd50, 8, 1),
	MTK_PIN_DRV_GRP(101, 0xd50, 8, 1),
	MTK_PIN_DRV_GRP(102, 0xd50, 8, 1),
	MTK_PIN_DRV_GRP(103, 0xd50, 8, 1),
	
	MTK_PIN_DRV_GRP(104, 0xd50, 12, 2),
	
	MTK_PIN_DRV_GRP(105, 0xd60, 0, 2),
	
	MTK_PIN_DRV_GRP(106, 0xd60, 4, 2),
	MTK_PIN_DRV_GRP(107, 0xd60, 4, 2),
	MTK_PIN_DRV_GRP(108, 0xd60, 4, 2),
	MTK_PIN_DRV_GRP(109, 0xd60, 4, 2),
	
	MTK_PIN_DRV_GRP(110, 0xd70, 0, 2),
	MTK_PIN_DRV_GRP(111, 0xd70, 0, 2),
	MTK_PIN_DRV_GRP(112, 0xd70, 0, 2),
	MTK_PIN_DRV_GRP(113, 0xd70, 0, 2),
	
	MTK_PIN_DRV_GRP(114, 0xd70, 4, 2),
	
	MTK_PIN_DRV_GRP(115, 0xd60, 12, 2),
	
	MTK_PIN_DRV_GRP(116, 0xd60, 8, 2),
	
	MTK_PIN_DRV_GRP(117, 0xd70, 0, 2),
	MTK_PIN_DRV_GRP(118, 0xd70, 0, 2),
	MTK_PIN_DRV_GRP(119, 0xd70, 0, 2),
	MTK_PIN_DRV_GRP(120, 0xd70, 0, 2),
};


static const struct mtk_pin_drv_grp *mt_find_pin_drv_grp_by_pin(unsigned long pin) 
{
	int i;

	if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;
	
	for (i = 0; i < MAX_GPIO_PIN; i++) {
		const struct mtk_pin_drv_grp *pin_drv =	((struct mtk_pin_drv_grp *)mt8168_pin_drv) + i;
		if (pin == pin_drv->pin)
			return pin_drv;
	}
	return NULL;
}

void mt_gpio_set_driving(u32 pin, unsigned char driving)
{
	unsigned int val;
	unsigned int bits, mask, shift;
	const struct mtk_drv_group_desc *drv_grp;
	const struct mtk_pin_drv_grp *pin_drv;
    struct mt_gpio_obj *obj = gpio_obj;

	if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;
	
	pin_drv = mt_find_pin_drv_grp_by_pin(pin);
	if (!pin_drv || pin_drv->grp >= DRV_GROUP_MAX)
		return -ERINVAL;

	drv_grp = ((struct mtk_drv_group_desc *)mt8168_drv_grp) + pin_drv->grp;
	
	if (driving >= drv_grp->min_drv && driving <= drv_grp->max_drv
		&& !(driving % drv_grp->step)) {		
		val = driving / drv_grp->step - 1;
		bits = drv_grp->high_bit - drv_grp->low_bit + 1;
		//mask = BIT(bits) - 1;
		mask = (1UL << (bits)) -1;
		shift = pin_drv->bit + drv_grp->low_bit;;
		mask <<= shift;
		val <<= shift;
		GPIO_WR32(GPIO_BASE+pin_drv->offset,((GPIO_RD32(GPIO_BASE+pin_drv->offset))&(~mask)));
		GPIO_WR32(GPIO_BASE+pin_drv->offset,((GPIO_RD32(GPIO_BASE+pin_drv->offset))|(val)));
		return RSUCCESS;
	}
	return -ERINVAL;
}
/*set driving-------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
void mt_gpio_pin_decrypt(u32 *cipher)
{
	//just for debug, find out who used pin number directly
	if((*cipher & (0x80000000)) == 0){
		GPIOERR("Pin %d decrypt warning! \n",*cipher);	
		//dump_stack();
		//return;
	}

	//GPIOERR("Pin magic number is %x\n",*cipher);
	*cipher &= ~(0x80000000);
	return;
}

//set GPIO function in fact
/*---------------------------------------------------------------------------*/

s32 mt_set_gpio_dir(u32 pin, u32 dir)
{
	mt_gpio_pin_decrypt(&pin);
	
#if PRELOADER_SUPPORT_EXT_GPIO
	return (pin >= GPIO_EXTEND_START) ? mt_set_gpio_dir_ext(pin,dir): mt_set_gpio_dir_chip(pin,dir);
#else
	return mt_set_gpio_dir_chip(pin,dir);
#endif
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_dir(u32 pin)
{    
	mt_gpio_pin_decrypt(&pin);

#if PRELOADER_SUPPORT_EXT_GPIO
	return (pin >= GPIO_EXTEND_START) ? mt_get_gpio_dir_ext(pin): mt_get_gpio_dir_chip(pin);
#else
	return mt_get_gpio_dir_chip(pin);
#endif
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_pull_enable(u32 pin, u32 enable)
{
	mt_gpio_pin_decrypt(&pin);

#if PRELOADER_SUPPORT_EXT_GPIO
	return (pin >= GPIO_EXTEND_START) ? mt_set_gpio_pull_enable_ext(pin,enable): mt_set_gpio_pull_enable_chip(pin,enable);
#else
	return mt_set_gpio_pull_enable_chip(pin,enable);
#endif
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_pull_enable(u32 pin)
{
	mt_gpio_pin_decrypt(&pin);
#if PRELOADER_SUPPORT_EXT_GPIO
	return (pin >= GPIO_EXTEND_START) ? mt_get_gpio_pull_enable_ext(pin): mt_get_gpio_pull_enable_chip(pin);
#else
	return mt_get_gpio_pull_enable_chip(pin);
#endif
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_pull_select(u32 pin, u32 select)
{
	mt_gpio_pin_decrypt(&pin);
	
#if PRELOADER_SUPPORT_EXT_GPIO
	return (pin >= GPIO_EXTEND_START) ? mt_set_gpio_pull_select_ext(pin,select): mt_set_gpio_pull_select_chip(pin,select);
#else
	return mt_set_gpio_pull_select_chip(pin,select);
#endif
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_pull_select(u32 pin)
{
	mt_gpio_pin_decrypt(&pin);
#if PRELOADER_SUPPORT_EXT_GPIO
	return (pin >= GPIO_EXTEND_START) ? mt_get_gpio_pull_select_ext(pin): mt_get_gpio_pull_select_chip(pin);
#else
	return mt_get_gpio_pull_select_chip(pin);
#endif
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_inversion(u32 pin, u32 enable)
{
#if PRELOADER_SUPPORT_EXT_GPIO
	mt_gpio_pin_decrypt(&pin);
	if(pin >= GPIO_EXTEND_START){
		return mt_set_gpio_inversion_ext(pin,enable);
	}
#endif
	return -ERACCESS;
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_inversion(u32 pin)
{
#if PRELOADER_SUPPORT_EXT_GPIO
	mt_gpio_pin_decrypt(&pin);
	if(pin >= GPIO_EXTEND_START){
		return mt_get_gpio_inversion_ext(pin);
	}
#endif
	return -ERACCESS;
}
/*---------------------------------------------------------------------------*/

s32 mt_set_gpio_out(u32 pin, u32 output)
{
	mt_gpio_pin_decrypt(&pin);
#if PRELOADER_SUPPORT_EXT_GPIO
	return (pin >= GPIO_EXTEND_START) ? mt_set_gpio_out_ext(pin,output): mt_set_gpio_out_chip(pin,output);
#else
	return mt_set_gpio_out_chip(pin,output);
#endif
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_out(u32 pin)
{
	mt_gpio_pin_decrypt(&pin);
	
#if PRELOADER_SUPPORT_EXT_GPIO
	return (pin >= GPIO_EXTEND_START) ? mt_get_gpio_out_ext(pin): mt_get_gpio_out_chip(pin);
#else
	return mt_get_gpio_out_chip(pin);
#endif
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_in(u32 pin)
{
	mt_gpio_pin_decrypt(&pin);
#if PRELOADER_SUPPORT_EXT_GPIO
	return (pin >= GPIO_EXTEND_START) ? mt_get_gpio_in_ext(pin): mt_get_gpio_in_chip(pin);
#else
	return mt_get_gpio_in_chip(pin);
#endif
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_mode(u32 pin, u32 mode)
{
	mt_gpio_pin_decrypt(&pin);
#if PRELOADER_SUPPORT_EXT_GPIO
	return (pin >= GPIO_EXTEND_START) ? mt_set_gpio_mode_ext(pin,mode): mt_set_gpio_mode_chip(pin,mode);
#else
	return mt_set_gpio_mode_chip(pin,mode);
#endif
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_mode(u32 pin)
{
	mt_gpio_pin_decrypt(&pin);
#if PRELOADER_SUPPORT_EXT_GPIO
	return (pin >= GPIO_EXTEND_START) ? mt_get_gpio_mode_ext(pin): mt_get_gpio_mode_chip(pin);
#else
	return mt_get_gpio_mode_chip(pin);
#endif
}

void mt_gpio_init(void)
{
#ifdef DUMMY_AP
	mt_gpio_set_default();
#endif

#ifdef TINY
	mt_gpio_set_default();
#endif
}
