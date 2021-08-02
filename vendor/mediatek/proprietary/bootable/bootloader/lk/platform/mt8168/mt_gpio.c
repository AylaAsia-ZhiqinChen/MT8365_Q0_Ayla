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
#include <platform/mt_reg_base.h>
#include <platform/gpio_cfg.h>
#include <debug.h>

//#include <typedefs.h>
#include <platform/mt_gpio.h>
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

#define GPIO_BRINGUP

#define GPIO_WR32(addr, data)   DRV_WriteReg32(addr, data)
#define GPIO_RD32(addr)         DRV_Reg32(addr)
#define GPIO_SW_SET_BITS(BIT,REG)   GPIO_WR32(REG,GPIO_RD32(REG) | ((unsigned long)(BIT)))
#define GPIO_SET_BITS(BIT,REG)   ((*(volatile unsigned long*)(REG)) = (unsigned long)(BIT))
#define GPIO_CLR_BITS(BIT,REG)   ((*(volatile unsigned long*)(REG)) &= ~((unsigned long)(BIT)))
/*---------------------------------------------------------------------------*/
#define TRUE                   1
#define FALSE                  0
/*---------------------------------------------------------------------------*/
//#define MAX_GPIO_REG_BITS      16
//#define MAX_GPIO_MODE_PER_REG  5
//#define GPIO_MODE_BITS         3
/*---------------------------------------------------------------------------*/
#define GPIOTAG                "[GPIO] "
#define GPIOLOG(fmt, arg...)   printf(GPIOTAG fmt, ##arg)
#define GPIOMSG(fmt, arg...)   printf(fmt, ##arg)
#define GPIOERR(fmt, arg...)   printf(GPIOTAG "%5d: "fmt, __LINE__, ##arg)
#define GPIOFUC(fmt, arg...)   //printf(GPIOTAG "%s\n", __FUNCTION__)
#define GIO_INVALID_OBJ(ptr)   ((ptr) != gpio_obj)


/******************************************************************************
Enumeration/Structure
******************************************************************************/

/*---------------------------------------------------------------------------*/
static s32 mt_set_gpio_dir_chip(u32 pin, u32 dir)
{
	u32 bit;
	u32 reg;
	u32 pos;

	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (dir >= GPIO_DIR_MAX)
		return -ERINVAL;

	bit = DIR_offset[pin].offset;

	reg = GPIO_RD32(DIR_addr[pin].addr);
	if (dir == GPIO_DIR_IN)
		reg &= (~(1 << bit));
	else
		reg |= (1 << bit);

	GPIO_WR32(DIR_addr[pin].addr, reg);

	return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static int mt_get_gpio_dir_chip(unsigned long pin)
{
    unsigned long pos;
    unsigned long bit;
    unsigned long reg;

    if (pin >= MAX_GPIO_PIN)
        return -ERINVAL;

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIO_RD32(DIR_addr[pin].addr);
    return (((reg & (1L << bit)) != 0)? 1: 0);
}
/*---------------------------------------------------------------------------*/

static s32 mt_get_gpio_pull_select_chip(u32 pin)
{
	unsigned long data;
	unsigned long data1;
	unsigned long r0;
	unsigned long r1;
	unsigned long pupd;
	unsigned long pullen;
	unsigned long pullsel;

	if (-1 != PUPD_offset[pin].offset) {
		data = GPIO_RD32(PUPD_addr[pin].addr);
		pupd = data & (1L << (PUPD_offset[pin].offset));
		r0 = data & (1L << (R0_offset[pin].offset));
		r1 = data & (1L << (R0_offset[pin].offset));
		if (r0 ==0 && r1 == 0)
				return GPIO_NO_PULL;	/*High Z(no pull) */
			else if (pupd == 0)
				return GPIO_PULL_UP;	/* pull up */
			else if (pupd == 1)
				return GPIO_PULL_DOWN;	/* pull down */
	}

	if (-1 != PULLEN_offset[pin].offset) {
		data = GPIO_RD32(PULLEN_addr[pin].addr);
		data1 = GPIO_RD32(PULLSEL_addr[pin].addr);
		if (0 == (data & (1L << (PULLEN_offset[pin].offset))))
			return GPIO_NO_PULL;
		else if (0 == (data1 & (1L << (PULLSEL_offset[pin].offset))))
			return GPIO_PULL_DOWN;
		else
			return GPIO_PULL_UP;
		
	}

}

/*---------------------------------------------------------------------------*/
static s32 mt_set_gpio_pull_select_chip(u32 pin, u32 select)
{
	u32 reg = 0;

	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (-1 != PULLSEL_offset[pin].offset) {
		reg = GPIO_RD32(PULLSEL_addr[pin].addr);
		if (select == GPIO_PULL_UP) {
			reg |= (1 << (PULLSEL_offset[pin].offset));
		} else if (select == GPIO_PULL_DOWN) {
			reg &= (~(1 << (PULLSEL_offset[pin].offset)));
		}
		GPIO_WR32(PULLSEL_addr[pin].addr, reg);
	}
	if (-1 != PUPD_offset[pin].offset) {
		reg = GPIO_RD32(PUPD_addr[pin].addr);
		reg &= (~(1 << (R1_offset[pin].offset)));
		reg |= (1 << (R0_offset[pin].offset));
		if (select == GPIO_PULL_UP) {
			reg &= (~(1 << (PUPD_offset[pin].offset)));
		} else if (select == GPIO_PULL_DOWN) {
			reg |= (1 << (PUPD_offset[pin].offset));
		}
		GPIO_WR32(PUPD_addr[pin].addr, reg);
	}
	return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
static s32 mt_set_gpio_pull_enable_chip(u32 pin, u32 enable)
{
	int reg = 0;

	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (-1 != PULLEN_offset[pin].offset) {
		reg = GPIO_RD32(PULLEN_addr[pin].addr);
		if (GPIO_PULL_DISABLE == enable)
			reg &= (~(1 << (PULLEN_offset[pin].offset)));
		else
			reg |= (1 << (PULLEN_offset[pin].offset));
		GPIO_WR32(PULLEN_addr[pin].addr, reg);
	}
	if (-1 != PUPD_offset[pin].offset) {
		reg = GPIO_RD32(PUPD_addr[pin].addr);
		if (GPIO_PULL_DISABLE == enable) {
			reg &= (~(1 << (R1_offset[pin].offset)));
			reg &= (~(1 << (R0_offset[pin].offset)));
		} else {
			reg &= (~(1 << (R1_offset[pin].offset)));
			reg |= (1 << (R0_offset[pin].offset));
		}
		GPIO_WR32(PUPD_addr[pin].addr, reg);
	}
	return RSUCCESS;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static s32 mt_get_gpio_pull_enable_chip(u32 pin)
{
	int res = 0;

	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;
	res = mt_get_gpio_pull_select_chip(pin);

	if (2 == res)
		return 0;	/*disable */
	if (1 == res || 0 == res)
		return 1;	/*enable */
	if (-1 == res)
		return -1;
	return -ERWRAPPER;

}

/*---------------------------------------------------------------------------*/
static s32 mt_set_gpio_smt_chip(u32 pin, u32 enable)
{

	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (SMT_offset[pin].offset == -1) {
		return GPIO_SMT_UNSUPPORTED;
	} else {
		if (enable == GPIO_SMT_DISABLE)
			GPIO_SET_BITS((1L << (SMT_offset[pin].offset)), SMT_addr[pin].addr + 8);
		else
			GPIO_SET_BITS((1L << (SMT_offset[pin].offset)), SMT_addr[pin].addr + 4);
	}

	return RSUCCESS;
}

/*---------------------------------------------------------------------------*/
static s32 mt_get_gpio_smt_chip(u32 pin)
{
	unsigned long data;
	u32 bit = 0;

	bit = SMT_offset[pin].offset;
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (SMT_offset[pin].offset == -1) {
		return GPIO_SMT_UNSUPPORTED;
	} else {
		data = GPIO_RD32(SMT_addr[pin].addr);
		return (((data & (1L << bit)) != 0) ? 1 : 0);
	}
}

/*---------------------------------------------------------------------------*/
static s32 mt_set_gpio_ies_chip(u32 pin, u32 enable)
{
	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (IES_offset[pin].offset == -1) {
		return GPIO_IES_UNSUPPORTED;
	} else {
		if (enable == GPIO_IES_DISABLE)
			GPIO_SET_BITS((1L << (IES_offset[pin].offset)), IES_addr[pin].addr + 8);
		else
			GPIO_SET_BITS((1L << (IES_offset[pin].offset)), IES_addr[pin].addr + 4);
	}

	return RSUCCESS;
}

/*---------------------------------------------------------------------------*/
static s32 mt_get_gpio_ies_chip(u32 pin)
{
	unsigned long data;

	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (IES_offset[pin].offset == -1) {
		return GPIO_IES_UNSUPPORTED;
	} else {
		data = GPIO_RD32(IES_addr[pin].addr);

		return (((data & (1L << (IES_offset[pin].offset))) != 0) ? 1 : 0);
	}
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
static s32 mt_set_gpio_out_chip(u32 pin, u32 output)
{
	u32 bit;
	u32 reg = 0;

	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (output >= GPIO_OUT_MAX)
		return -ERINVAL;

	bit = DATAOUT_offset[pin].offset;

	reg = GPIO_RD32(DATAOUT_addr[pin].addr);
	if (output == GPIO_OUT_ZERO)
		reg &= (~(1 << bit));
	else
		reg |= (1 << bit);
	GPIO_WR32(DATAOUT_addr[pin].addr, reg);

	return RSUCCESS;
}

/*---------------------------------------------------------------------------*/
static s32 mt_get_gpio_out_chip(u32 pin)
{
	u32 pos;
	u32 bit;
	u32 reg;

	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	pos = pin / MAX_GPIO_REG_BITS;
	bit = pin % MAX_GPIO_REG_BITS;

	reg = GPIO_RD32(DATAOUT_addr[pin].addr);
	return (((reg & (1L << bit)) != 0) ? 1 : 0);
}

/*---------------------------------------------------------------------------*/
static s32 mt_get_gpio_in_chip(u32 pin)
{
	u32 bit;
	u32 reg;

	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	bit = DATAIN_offset[pin].offset;
	reg = GPIO_RD32(DATAIN_addr[pin].addr);
	return (((reg & (1L << bit)) != 0) ? 1 : 0);
}
/*---------------------------------------------------------------------------*/
static s32 mt_set_gpio_mode_chip(u32 pin, u32 mode)
{
	u32 bit;
	u32 reg;

	u32 mask = (1L << GPIO_MODE_BITS) - 1;


	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	if (mode >= GPIO_MODE_MAX)
		return -ERINVAL;

	bit = MODE_offset[pin].offset;

	mode = mode & 0x7;
	reg = GPIO_RD32(MODE_addr[pin].addr);
	reg &= (~(mask << bit));
	reg |= (mode << bit);
	GPIO_WR32(MODE_addr[pin].addr, reg);

	return RSUCCESS;
}

/*---------------------------------------------------------------------------*/
static s32 mt_get_gpio_mode_chip(u32 pin)
{
	u32 bit;
	u32 reg;
	u32 mask = (1L << GPIO_MODE_BITS) - 1;

	if (pin >= MAX_GPIO_PIN)
		return -ERINVAL;

	bit = MODE_offset[pin].offset;
	reg = GPIO_RD32(MODE_addr[pin].addr);
	return ((reg >> bit) & mask);
}
/*---------------------------------------------------------------------------*/

void mt_gpio_pin_decrypt(u32 *cipher)
{
	//just for debug, find out who used pin number directly
	if((*cipher & (0x80000000)) == 0){
	}
	*cipher &= ~(0x80000000);
	return;
}

//set GPIO function in fact
/*---------------------------------------------------------------------------*/
int mt_set_gpio_dir(u32 pin, u32 dir)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_set_gpio_dir_chip(pin,dir);
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_dir(u32 pin)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_get_gpio_dir_chip(pin);
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_pull_enable(u32 pin, u32 enable)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_set_gpio_pull_enable_chip(pin,enable);
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_pull_enable(u32 pin)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_get_gpio_pull_enable_chip(pin);
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_pull_select(u32 pin, u32 select)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_set_gpio_pull_select_chip(pin,select);
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_pull_select(u32 pin)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_get_gpio_pull_select_chip(pin);
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_smt(u32 pin, u32 enable)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_set_gpio_smt_chip(pin,enable);
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_smt(u32 pin)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_get_gpio_smt_chip(pin);
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_ies(u32 pin, u32 enable)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_set_gpio_ies_chip(pin,enable);
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_ies(u32 pin)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_get_gpio_ies_chip(pin);
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_out(u32 pin, u32 output)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_set_gpio_out_chip(pin,output);
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_out(u32 pin)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_get_gpio_out_chip(pin);
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_in(u32 pin)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_get_gpio_in_chip(pin);
}
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_mode(u32 pin, u32 mode)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_set_gpio_mode_chip(pin,mode);
}
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_mode(u32 pin)
{
	mt_gpio_pin_decrypt(&pin);

    return mt_get_gpio_mode_chip(pin);
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
