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
 * MediaTek Inc. (C) 2016. All rights reserved.
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
 * MTK PMIC Wrapper Driver
 *
 * Copyright 2016 MediaTek Co.,Ltd.
 *
 * DESCRIPTION:
 *     This file provides API for other drivers to access PMIC registers
 *
 ******************************************************************************/

#include <pmic_wrap_init.h>
#if (PMIC_WRAP_PRELOADER)
#elif (PMIC_WRAP_LK)
#elif (PMIC_WRAP_KERNEL)
#elif (PMIC_WRAP_CTP)
#include <gpio.h>
#include <upmu_hw.h>
#else
### Compile error, check SW ENV define
#endif


/************* marco    ******************************************************/
#if (PMIC_WRAP_PRELOADER)
#elif (PMIC_WRAP_LK)
#elif (PMIC_WRAP_KERNEL)
#elif (PMIC_WRAP_SCP)
#elif (PMIC_WRAP_CTP)
#else
### Compile error, check SW ENV define
#endif

#ifdef PMIC_WRAP_NO_PMIC
#if !(PMIC_WRAP_KERNEL)
signed int pwrap_wacs2(unsigned int  write, unsigned int  adr, unsigned int  wdata, unsigned int *rdata)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_read(unsigned int adr, unsigned int *rdata)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_write(unsigned int adr, unsigned int wdata)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}
#endif
signed int pwrap_wacs2_read(unsigned int  adr, unsigned int *rdata)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

/* Provide PMIC write API */
signed int pwrap_wacs2_write(unsigned int  adr, unsigned int  wdata)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_read_nochk(unsigned int adr, unsigned int *rdata)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_write_nochk(unsigned int adr, unsigned int wdata)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

/*
 *pmic_wrap init,init wrap interface
 *
 */
signed int pwrap_init(void)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

signed int pwrap_init_preloader(void)
{
	PWRAPLOG("[PMIC_WRAP]There is no PMIC real chip, PMIC_WRAP do Nothing.\n");
	return 0;
}

#else /* #ifdef PMIC_WRAP_NO_PMIC */
/*********************start ---internal API***********************************/
static int _pwrap_timeout_ns(unsigned long long start_time_ns, unsigned long long timeout_time_ns);
static unsigned long long _pwrap_get_current_time(void);
static unsigned long long _pwrap_time2ns(unsigned long long time_us);
static signed int _pwrap_reset_spislv(void);
static signed int _pwrap_init_dio(unsigned int dio_en);
static signed int _pwrap_init_cipher(void);
static signed int _pwrap_init_reg_clock(unsigned int regck_sel);
static void _pwrap_enable(void);
static void _pwrap_starve_set(void);
static signed int _pwrap_wacs2_nochk(unsigned int write, unsigned int adr, unsigned int wdata, unsigned int *rdata);
static signed int pwrap_wacs2_hal(unsigned int write, unsigned int adr, unsigned int wdata, unsigned int *rdata);
/*********************test API************************************************/
static inline void pwrap_dump_ap_register(void);
static unsigned int pwrap_write_test(void);
static unsigned int pwrap_read_test(void);
signed int pwrap_wacs2_read(unsigned int  adr, unsigned int *rdata);
signed int pwrap_wacs2_write(unsigned int  adr, unsigned int  wdata);
/************* end--internal API**********************************************/
/*********************** external API for pmic_wrap user ************************/
signed int pwrap_wacs2_read(unsigned int  adr, unsigned int *rdata)
{
	pwrap_wacs2(0, adr, 0, rdata);
	return 0;
}

/* Provide PMIC write API */
signed int pwrap_wacs2_write(unsigned int  adr, unsigned int  wdata)
{
#ifdef CONFIG_MTK_TINYSYS_SSPM_SUPPORT
	unsigned int flag;

	flag = WRITE_CMD | (1 << WRITE_PMIC);
	pwrap_wacs2_ipi(adr, wdata, flag);
#else
	pwrap_wacs2(1, adr, wdata, 0);
#endif
	return 0;
}

signed int pwrap_read(unsigned int adr, unsigned int *rdata)
{
	return pwrap_wacs2(0,adr,0,rdata);
}

signed int pwrap_write(unsigned int adr, unsigned int  wdata)
{
	return pwrap_wacs2(1,adr,wdata,0);
}
/******************************************************************************
 *wrapper timeout
 *****************************************************************************/
/*use the same API name with kernel driver
 *however,the timeout API in uboot use tick instead of ns
 */

#ifdef PWRAP_TIMEOUT
static unsigned long long _pwrap_get_current_time(void)
{
	return gpt4_get_current_tick();
}

static int _pwrap_timeout_ns(unsigned long long start_time_ns, unsigned long long timeout_time_ns)
{
	return gpt4_timeout_tick(start_time_ns, timeout_time_ns);
}

static unsigned long long _pwrap_time2ns(unsigned long long time_us)
{
	return gpt4_time2tick_us(time_us);
}

#else
static unsigned long long _pwrap_get_current_time(void)
{
	return 0;
}
static int _pwrap_timeout_ns(unsigned long long start_time_ns, unsigned long long elapse_time)
{
	return 0;
}

static unsigned long long _pwrap_time2ns(unsigned long long time_us)
{
	return 0;
}

#endif

/* ##################################################################### */
/* define macro and inline function (for do while loop) */
/* ##################################################################### */
typedef unsigned int(*loop_condition_fp) (unsigned int);    /* define a function pointer */

static inline unsigned int wait_for_fsm_idle(unsigned int x)
{
	return GET_WACS2_FSM(x) != WACS_FSM_IDLE;
}

static inline unsigned int wait_for_fsm_vldclr(unsigned int x)
{
	return GET_WACS2_FSM(x) != WACS_FSM_WFVLDCLR;
}

static inline unsigned int wait_for_sync(unsigned int x)
{
	return GET_SYNC_IDLE2(x) != WACS_SYNC_IDLE;
}

static inline unsigned int wait_for_idle_and_sync(unsigned int x)
{
	return (GET_WACS2_FSM(x) != WACS_FSM_IDLE) || (GET_SYNC_IDLE2(x) != WACS_SYNC_IDLE);
}

static inline unsigned int wait_for_wrap_idle(unsigned int x)
{
	return (GET_WRAP_FSM(x) != 0x0) || (GET_WRAP_CH_DLE_RESTCNT(x) != 0x0);
}

static inline unsigned int wait_for_wrap_state_idle(unsigned int x)
{
	return GET_WRAP_AG_DLE_RESTCNT(x) != 0;
}

static inline unsigned int wait_for_man_idle_and_noreq(unsigned int x)
{
	return (GET_MAN_REQ(x) != MAN_FSM_NO_REQ) || (GET_MAN_FSM(x) != MAN_FSM_IDLE);
}

static inline unsigned int wait_for_man_vldclr(unsigned int x)
{
	return GET_MAN_FSM(x) != MAN_FSM_WFVLDCLR;
}

static inline unsigned int wait_for_cipher_ready(unsigned int x)
{
	return x != 3;
}

static inline unsigned int wait_for_stdupd_idle(unsigned int x)
{
	return GET_STAUPD_FSM(x) != 0x0;
}

/**************used at _pwrap_wacs2_nochk*************************************/
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
static inline unsigned int wait_for_state_ready_init(loop_condition_fp fp, unsigned int timeout_us,
        void *wacs_register, unsigned int *read_reg)
#else
static inline unsigned int wait_for_state_ready_init(loop_condition_fp fp, unsigned int timeout_us,
        volatile unsigned int *wacs_register, unsigned int *read_reg)
#endif
{
	unsigned long long start_time_ns = 0, timeout_ns = 0;
	unsigned int reg_rdata = 0x0;

	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(timeout_us);

	do {
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
			PWRAPERR("ready_init timeout\n");
			return E_PWR_WAIT_IDLE_TIMEOUT;
		}
		reg_rdata = WRAP_RD32(wacs_register);
	} while (fp(reg_rdata));

	if (read_reg)
		*read_reg = reg_rdata;

	return 0;
}

#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
static inline unsigned int wait_for_state_idle(loop_condition_fp fp, unsigned int timeout_us, void *wacs_register,
        void *wacs_vldclr_register, unsigned int *read_reg)
#else
static inline unsigned int wait_for_state_idle(loop_condition_fp fp, unsigned int timeout_us,
        volatile unsigned int *wacs_register, volatile unsigned int *wacs_vldclr_register, unsigned int *read_reg)
#endif
{
	unsigned long long start_time_ns = 0, timeout_ns = 0;
	unsigned int reg_rdata;

	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(timeout_us);

	do {
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
			PWRAPERR("state_idle timeout\n");
			pwrap_dump_ap_register();
			return E_PWR_WAIT_IDLE_TIMEOUT;
		}
		reg_rdata = WRAP_RD32(wacs_register);
		if (GET_WACS2_INIT_DONE2(reg_rdata) != WACS_INIT_DONE) {
			PWRAPERR("init isn't finished\n");
			return E_PWR_NOT_INIT_DONE;
		}
		switch (GET_WACS2_FSM(reg_rdata)) {
			case WACS_FSM_WFVLDCLR:
				WRAP_WR32(wacs_vldclr_register, 1);
				PWRAPERR("WACS_FSM = VLDCLR\n");
				break;
			case WACS_FSM_WFDLE:
				PWRAPERR("WACS_FSM = WFDLE\n");
				break;
			case WACS_FSM_REQ:
				PWRAPERR("WACS_FSM = REQ\n");
				break;
			default:
				break;
		}
	} while (fp(reg_rdata));
	if (read_reg)
		*read_reg = reg_rdata;

	return 0;
}

/**************used at pwrap_wacs2********************************************/
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
static inline unsigned int wait_for_state_ready(loop_condition_fp fp, unsigned int timeout_us, void *wacs_register,
        unsigned int *read_reg)
#else
static inline unsigned int wait_for_state_ready(loop_condition_fp fp, unsigned int timeout_us,
        volatile unsigned int *wacs_register, unsigned int *read_reg)
#endif
{
	unsigned long long start_time_ns = 0, timeout_ns = 0;
	unsigned int reg_rdata;

	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(timeout_us);

	do {
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
			PWRAPERR("state_ready timeout\n");
			pwrap_dump_ap_register();
			return E_PWR_WAIT_IDLE_TIMEOUT;
		}
		reg_rdata = WRAP_RD32(wacs_register);
		if (GET_WACS2_INIT_DONE2(reg_rdata) != WACS_INIT_DONE) {
			PWRAPERR("init isn't finished\n");
			return E_PWR_NOT_INIT_DONE;
		}
	} while (fp(reg_rdata));
	if (read_reg)
		*read_reg = reg_rdata;

	return 0;
}

/******************************************************
 * Function : pwrap_wacs2()
 * Description :
 * Parameter :
 * Return :
 ******************************************************/
signed int pwrap_wacs2(unsigned int write, unsigned int adr, unsigned int wdata, unsigned int *rdata)
{
	unsigned int reg_rdata = 0;
	unsigned int wacs_write = 0;
	unsigned int wacs_adr = 0;
	unsigned int wacs_cmd = 0;
	unsigned int return_value = 0;

	/* Check argument validation */
	if ((write & ~(0x1)) != 0)
		return E_PWR_INVALID_RW;
	if ((adr & ~(0xffff)) != 0)
		return E_PWR_INVALID_ADDR;
	if ((wdata & ~(0xffff)) != 0)
		return E_PWR_INVALID_WDAT;

	/* Check IDLE & INIT_DONE in advance */
	return_value =
	    wait_for_state_idle(wait_for_fsm_idle, TIMEOUT_WAIT_IDLE, PMIC_WRAP_WACS2_RDATA,
	                        PMIC_WRAP_WACS2_VLDCLR, 0);
	if (return_value != 0) {
		PWRAPERR("fsm_idle fail,ret=%d\n", return_value);
		goto FAIL;
	}
	wacs_write = write << 31;
	wacs_adr = (adr >> 1) << 16;
	wacs_cmd = wacs_write | wacs_adr | wdata;

	WRAP_WR32(PMIC_WRAP_WACS2_CMD, wacs_cmd);
	if (write == 0) {
		if (rdata == NULL) {
			PWRAPERR("rdata NULL\n");
			return_value = E_PWR_INVALID_ARG;
			goto FAIL;
		}
		return_value =
		    wait_for_state_ready(wait_for_fsm_vldclr, TIMEOUT_READ, PMIC_WRAP_WACS2_RDATA,
		                         &reg_rdata);
		if (return_value != 0) {
			PWRAPERR("fsm_vldclr fail,ret=%d\n", return_value);
			return_value += 1;
			goto FAIL;
		}
		*rdata = GET_WACS2_RDATA(reg_rdata);
		WRAP_WR32(PMIC_WRAP_WACS2_VLDCLR, 1);
	}

FAIL:
	if (return_value != 0) {
		PWRAPERR("pwrap_wacs2_hal fail,ret=%d\n", return_value);
		PWRAPERR("BUG_ON\n");
	}

	return return_value;
}


/*********************internal API for pwrap_init***************************/

/**********************************
 * Function : _pwrap_wacs2_nochk()
 * Description :
 * Parameter :
 * Return :
 ***********************************/
signed int pwrap_read_nochk(unsigned int adr, unsigned int *rdata)
{
	return _pwrap_wacs2_nochk(0, adr, 0, rdata);
}

signed int pwrap_write_nochk(unsigned int adr, unsigned int wdata)
{
	return _pwrap_wacs2_nochk(1, adr, wdata, 0);
}

static signed int _pwrap_wacs2_nochk(unsigned int write, unsigned int adr, unsigned int wdata, unsigned int *rdata)
{
	unsigned int reg_rdata = 0x0;
	unsigned int wacs_write = 0x0;
	unsigned int wacs_adr = 0x0;
	unsigned int wacs_cmd = 0x0;
	unsigned int return_value = 0x0;

	/* Check argument validation */
	if ((write & ~(0x1)) != 0)
		return E_PWR_INVALID_RW;
	if ((adr & ~(0xffff)) != 0)
		return E_PWR_INVALID_ADDR;
	if ((wdata & ~(0xffff)) != 0)
		return E_PWR_INVALID_WDAT;

	/* Check IDLE */
	return_value =
	    wait_for_state_ready_init(wait_for_fsm_idle, TIMEOUT_WAIT_IDLE, PMIC_WRAP_WACS2_RDATA, 0);
	if (return_value != 0) {
		PWRAPERR("write fail,ret=%x\n", return_value);
		return return_value;
	}

	wacs_write = write << 31;
	wacs_adr = (adr >> 1) << 16;
	wacs_cmd = wacs_write | wacs_adr | wdata;
	WRAP_WR32(PMIC_WRAP_WACS2_CMD, wacs_cmd);

	if (write == 0) {
		if (rdata == NULL) {
			PWRAPERR("rdata NULL\n");
			return_value = E_PWR_INVALID_ARG;
			return return_value;
		}
		return_value =
		    wait_for_state_ready_init(wait_for_fsm_vldclr, TIMEOUT_READ,
		                              PMIC_WRAP_WACS2_RDATA, &reg_rdata);
		if (return_value != 0) {
			PWRAPERR("fsm_vldclr fail,ret=%d\n", return_value);
			return_value += 1;
			return return_value;
		}
		*rdata = GET_WACS2_RDATA(reg_rdata);
		WRAP_WR32(PMIC_WRAP_WACS2_VLDCLR, 1);
	}

	return 0;
}

static void __pwrap_soft_reset(void)
{
	PWRAPLOG("start reset wrapper\n");
	WRAP_WR32(INFRA_GLOBALCON_RST2_SET, 0x1);
	WRAP_WR32(INFRA_GLOBALCON_RST2_CLR, 0x1);
}

static void __pwrap_spi_clk_set(void)
{
	PWRAPLOG("pwrap_spictl reset ok\n");

	/* sys_ck cg enable, turn off clock */
	WRAP_WR32(MODULE_SW_CG_0_SET, 0x0000000f);
	/* turn off clock */
	/* 0x100010C0 MODULE_SW_CG_3_SET [15:12] */
	WRAP_WR32(MODULE_SW_CG_3_SET, 0x7000);

	/* Disable Clock Source Control By SPM */
	PWRAPLOG("=====PMICW_CLOCK_CTRL===== (Write before): %x\n", WRAP_RD32(PMICW_CLOCK_CTRL));
	WRAP_WR32(PMICW_CLOCK_CTRL, (WRAP_RD32(PMICW_CLOCK_CTRL) & ~(0x1 << 2)));
	PWRAPLOG("=====PMICW_CLOCK_CTRL===== (Write after): %x\n", WRAP_RD32(PMICW_CLOCK_CTRL));

	/* toggle PMIC_WRAP and pwrap_spictl reset */
	__pwrap_soft_reset();

	/*sys_ck cg enable, turn on clock*/
	WRAP_WR32(MODULE_SW_CG_0_CLR, 0x0000000f);
	/* turn on clock*/
	WRAP_WR32(MODULE_SW_CG_3_CLR, 0x7000);
	PWRAPLOG("spi clk set ....\n");
}

/************************************************
 * Function : _pwrap_init_dio()
 * Description :call it in pwrap_init,mustn't check init done
 * Parameter :
 * Return :
 ************************************************/
static signed int _pwrap_init_dio(unsigned int dio_en)
{
	unsigned int rdata = 0x0;
	unsigned int ret = 0;

	WRAP_WR32(PMIC_WRAP_HPRIO_ARB_EN, 0x4); /* ONLY WACS2 */

	/* wait for WRAP_FSM idle */
	do {
		rdata = WRAP_RD32(PMIC_WRAP_WRAP_STA);
	} while((GET_WRAP_FSM(rdata) != 0x0) || (GET_WRAP_CH_DLE_RESTCNT(rdata) != 0x0));

	ret = pwrap_write_nochk(PMIC_DEW_DIO_EN_ADDR, dio_en);
	if (ret != 0)
		PWRAPERR("_pwrap_init_dio, write PMIC_DEW_DIO_EN_ADDR fail, ret = 0x%x\n", ret);
#ifdef DUAL_PMICS
	pwrap_write_nochk(EXT_DEW_DIO_EN, dio_en);
#endif

	do {
		rdata = WRAP_RD32(PMIC_WRAP_WACS2_RDATA);
	} while ((GET_WACS2_FSM(rdata) != WACS_FSM_IDLE) || (GET_SYNC_IDLE2(rdata) != WACS_SYNC_IDLE));

#ifndef DUAL_PMICS
	WRAP_WR32(PMIC_WRAP_DIO_EN, dio_en);
#else
	WRAP_WR32(PMIC_WRAP_DIO_EN, 0x2 | dio_en);
#endif

	return 0;
}

/***************************************************
 * Function : _pwrap_init_cipher()
 * Description :
 * Parameter :
 * Return :
 ****************************************************/
static signed int _pwrap_init_cipher(void)
{
	unsigned int rdata = 0;
	unsigned int return_value = 0;
	unsigned int start_time_ns = 0, timeout_ns = 0;

	WRAP_WR32(PMIC_WRAP_CIPHER_SWRST, 1);
	WRAP_WR32(PMIC_WRAP_CIPHER_SWRST, 0);
	WRAP_WR32(PMIC_WRAP_CIPHER_KEY_SEL, 1);
	WRAP_WR32(PMIC_WRAP_CIPHER_IV_SEL, 2);
	WRAP_WR32(PMIC_WRAP_CIPHER_EN, 1);
	/* Config CIPHER @ PMIC */
	pwrap_write_nochk(PMIC_DEW_CIPHER_SWRST_ADDR, 0x1);
	pwrap_write_nochk(PMIC_DEW_CIPHER_SWRST_ADDR, 0x0);
	pwrap_write_nochk(PMIC_DEW_CIPHER_KEY_SEL_ADDR, 0x1);
	pwrap_write_nochk(PMIC_DEW_CIPHER_IV_SEL_ADDR,  0x2);
	pwrap_write_nochk(PMIC_DEW_CIPHER_EN_ADDR,  0x1);
	PWRAPLOG("[_pwrap_init_cipher]Config CIPHER of PMIC 0 ok\n");

	/* wait for cipher data ready@AP */
	return_value = wait_for_state_ready_init(wait_for_cipher_ready, TIMEOUT_WAIT_IDLE, PMIC_WRAP_CIPHER_RDY, 0);
	if (return_value != 0) {
		PWRAPERR("cipher fail,ret=%x\n", return_value);
		return return_value;
	}
	PWRAPLOG("wait for cipher 0 and 1 to be ready ok\n");

	/* wait for cipher 0 data ready@PMIC */
	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(0xFFFFFF);
	do {
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns))
			PWRAPERR("cipher 0 data\n");

		pwrap_read_nochk(PMIC_DEW_CIPHER_RDY_ADDR, &rdata);
	} while (rdata != 0x1); /* cipher_ready */

	return_value = pwrap_write_nochk(PMIC_DEW_CIPHER_MODE_ADDR, 0x1);
	if (return_value != 0) {
		PWRAPERR("CIPHER_MODE fail,ret=%x\n", return_value);
		return return_value;
	}
	PWRAPLOG("wait for cipher 0 data ready ok\n");

	/* wait for cipher mode idle */
	return_value = wait_for_state_ready_init(wait_for_idle_and_sync, TIMEOUT_WAIT_IDLE, PMIC_WRAP_WACS2_RDATA, 0);
	if (return_value != 0) {
		PWRAPERR("cipher mode idle fail,ret=%x\n", return_value);
		return return_value;
	}
	WRAP_WR32(PMIC_WRAP_CIPHER_MODE, 1);

	/* Read Test */
	pwrap_read_nochk(PMIC_DEW_READ_TEST_ADDR, &rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		PWRAPERR("cipher,err=%x, rdata=%x\n", 1, rdata);
		return E_PWR_READ_TEST_FAIL;
	}

	return 0;
}

static void _pwrap_InitStaUpd(void)
{

#ifndef DUAL_PMICS
	WRAP_WR32(PMIC_WRAP_STAUPD_GRPEN, 0xf5);
#else
	WRAP_WR32(PMIC_WRAP_STAUPD_GRPEN, 0xff);
#endif

#ifdef PMIC_WRAP_CRC_SUPPORT
	/* CRC */
#ifndef DUAL_PMICS
	pwrap_write_nochk(PMIC_DEW_CRC_EN_ADDR, 0x1);
	WRAP_WR32(PMIC_WRAP_CRC_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_SIG_ADR, PMIC_DEW_CRC_VAL_ADDR);
#else
	pwrap_write_nochk(PMIC_DEW_CRC_EN_ADDR, 0x1);
	pwrap_write_nochk(EXT_DEW_CRC_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_CRC_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_SIG_ADR, (PMIC_EXT_DEW_CRC_VAL_ADDR << 16 | PMIC_DEW_CRC_VAL_ADDR));
#endif
#else
	/* Signature */
#ifndef DUAL_PMICS
	WRAP_WR32(PMIC_WRAP_SIG_MODE, 0x1);
	WRAP_WR32(PMIC_WRAP_SIG_ADR, PMIC_DEW_CRC_VAL_ADDR);
	WRAP_WR32(PMIC_WRAP_SIG_VALUE, 0x83);
#else
	WRAP_WR32(PMIC_WRAP_SIG_MODE, 0x3);
	WRAP_WR32(PMIC_WRAP_SIG_ADR, (PMIC_EXT_DEW_CRC_VAL_ADDR << 16) | PMIC_DEW_CRC_VAL_ADDR);
	WRAP_WR32(PMIC_WRAP_SIG_VALUE, (0x83 << 16) | 0x83);
#endif
#endif /* end of crc */

	WRAP_WR32(PMIC_WRAP_EINT_STA0_ADR, PMIC_CPU_INT_STA_ADDR);
#ifdef DUAL_PMICS
	WRAP_WR32(PMIC_WRAP_EINT_STA1_ADR, EXT_INT_STA);
#endif

	/* MD ADC Interface */
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_LATEST_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_WP_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_0_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_1_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_2_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_3_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_4_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_5_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_6_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_7_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_8_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_9_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_10_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_11_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_12_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_13_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_14_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_15_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_16_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_17_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_18_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_19_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_20_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_21_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_22_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_23_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_24_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_25_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_26_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_27_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_28_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_29_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_30_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);
	WRAP_WR32(PMIC_WRAP_MD_AUXADC_RDATA_31_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_MDRT_ADDR << 16) + PMIC_AUXADC_ADC_OUT_MDRT_ADDR);

	WRAP_WR32(PMIC_WRAP_INT_GPS_AUXADC_CMD_ADDR, (PMIC_AUXADC_RQST_DCXO_BY_GPS_ADDR << 16) + PMIC_AUXADC_RQST_CH7_ADDR);
	WRAP_WR32(PMIC_WRAP_INT_GPS_AUXADC_CMD, (0x0400 << 16) + 0x0080);
	WRAP_WR32(PMIC_WRAP_INT_GPS_AUXADC_RDATA_ADDR, (PMIC_AUXADC_ADC_OUT_DCXO_BY_GPS_ADDR << 16) + PMIC_AUXADC_ADC_OUT_CH7_BY_AP_ADDR);

	WRAP_WR32(PMIC_WRAP_EXT_GPS_AUXADC_RDATA_ADDR, PMIC_AUXADC_ADC_OUT_MDRT_ADDR);

}

static void _pwrap_starve_set(void)
{
	WRAP_WR32(PMIC_WRAP_HARB_HPRIO, 0xf);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_0, 0x400);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_1, 0x402);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_2, 0x402);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_3, 0x40e);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_4, 0x402);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_5, 0x427);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_6, 0x427);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_7, 0x4a4);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_8, 0x413);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_9, 0x417);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_10, 0x417);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_11, 0x47b);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_12, 0x47b);
	WRAP_WR32(PMIC_WRAP_STARV_COUNTER_13, 0x45b);
}

static void _pwrap_enable(void)
{
#if (MTK_PLATFORM_MT6357)
	WRAP_WR32(PMIC_WRAP_HPRIO_ARB_EN, 0x3fd35);
#endif
	WRAP_WR32(PMIC_WRAP_WACS0_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_WACS2_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_WACS_P2P_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_WACS_MD32_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_STAUPD_CTRL, 0x5); /* 100us */
	WRAP_WR32(PMIC_WRAP_WDT_UNIT, 0xf);
	WRAP_WR32(PMIC_WRAP_WDT_SRC_EN_0, 0xffffffff);
	WRAP_WR32(PMIC_WRAP_WDT_SRC_EN_1, 0xffffffff);
	WRAP_WR32(PMIC_WRAP_TIMER_CTRL, 0x3);
	WRAP_WR32(PMIC_WRAP_INT0_EN, 0x00000007);
	WRAP_WR32(PMIC_WRAP_INT1_EN, 0xffffd800); /* disable Matching interrupt for bit 13 */
}

/************************************************
 * Function : _pwrap_init_sistrobe()
 * scription : Initialize SI_CK_CON and SIDLY
 * Parameter :
 * Return :
 ************************************************/
static signed int _pwrap_init_sistrobe(int dual_si_sample_settings)
{
	unsigned int rdata;
	int si_en_sel, si_ck_sel, si_dly, si_sample_ctrl, clk_edge_no, i;
	int found, result_faulty = 0;
	int test_data[30] = {0x6996, 0x9669, 0x6996, 0x9669, 0x6996, 0x9669, 0x6996, 0x9669, 0x6996, 0x9669,
	                     0x5AA5, 0xA55A, 0x5AA5, 0xA55A, 0x5AA5, 0xA55A, 0x5AA5, 0xA55A, 0x5AA5, 0xA55A,
	                     0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27
	                    };

	/* TINFO = "[DrvPWRAP_InitSiStrobe] SI Strobe Calibration For PMIC 0............" */
	/* TINFO = "[DrvPWRAP_InitSiStrobe] Scan For The First Valid Sampling Clock Edge......" */
	found = 0;
	for (si_en_sel = 0; si_en_sel < 8; si_en_sel++) {
		for (si_ck_sel = 0; si_ck_sel < 2; si_ck_sel++) {
			si_sample_ctrl = (si_en_sel << 6) | (si_ck_sel << 5);
			WRAP_WR32(PMIC_WRAP_SI_SAMPLE_CTRL, si_sample_ctrl);

			pwrap_read_nochk(PMIC_DEW_READ_TEST_ADDR, &rdata);
			if (rdata == DEFAULT_VALUE_READ_TEST) {
				PWRAPLOG("[DrvPWRAP_InitSiStrobe]The First Valid Sampling Clock Edge Is Found !!!\n");
				PWRAPCRI("si_en_sel = %x, si_ck_sel = %x, si_sample_ctrl = %x, rdata = %x, Pass\n",
				         si_en_sel, si_ck_sel, si_sample_ctrl, rdata);
				found = 1;
				break;
			}
			PWRAPCRI("si_en_sel = %x, si_ck_sel = %x, si_sample_ctrl = %x, rdata = %x\n",
			         si_en_sel, si_ck_sel, si_sample_ctrl, rdata);
		}
		if (found == 1)
			break;
	}
	if (found == 0) {
		result_faulty |= 0x1;
		PWRAPERR("result_faulty = %d\n", result_faulty);
	}
	if ((si_en_sel == 7) && (si_ck_sel == 1)) {
		result_faulty |= 0x2;
		PWRAPERR("result_faulty = %d (Sampling Clock is last Setting, can not Use)\n", result_faulty);
	}
	/* TINFO = "[DrvPWRAP_InitSiStrobe] Search For The Data Boundary......" */
	for (si_dly = 0; si_dly < 10; si_dly++) {
		pwrap_write_nochk(PMIC_RG_SPI_DLY_SEL_ADDR, si_dly);

		found = 0;
#ifndef SPEED_UP_PWRAP_INIT
		for (i = 0; i < 30; i++)
#else
		for (i = 0; i < 1; i++)
#endif
		{
			pwrap_write_nochk(PMIC_DEW_WRITE_TEST_ADDR, test_data[i]);
			pwrap_read_nochk(PMIC_DEW_WRITE_TEST_ADDR, &rdata);
			if ((rdata & 0x7fff) != (test_data[i] & 0x7fff)) {
				PWRAPCRI("InitSiStrobe (%x, %x, %x) Data Boundary Is Found !!\n",
				         si_dly, si_dly, rdata);
				found = 1;
				break;
			}
		}
		if (found == 1)
			break;
		PWRAPLOG("si_dly = %x, *RG_SPI_CON2 = %x, rdata = %x\n",
		         si_dly, si_dly, rdata);
	}

	/* TINFO = "[DrvPWRAP_InitSiStrobe] Change The Sampling Clock Edge To The Next One." */
	/* si_sample_ctrl = (((si_en_sel << 1) | si_ck_sel) + 1) << 2;*/
	si_sample_ctrl = si_sample_ctrl + 0x20;
	WRAP_WR32(PMIC_WRAP_SI_SAMPLE_CTRL, si_sample_ctrl);
	if (si_dly == 10) {
		PWRAPLOG("SI Strobe Calibration For PMIC 0 Done, (%x, si_dly%x)\n", si_sample_ctrl, si_dly);
		si_dly--;
	}
	PWRAPCRI("SI Strobe Calibration For PMIC 0 Done, (%x, %x)\n", si_sample_ctrl, si_dly);

#if PMIC_WRAP_ULPOSC_CAL
	/* SI Strobe Calibration For ULPOSC Clock */
	/* TINFO = "[DrvPWRAP_InitSiStrobe] SI Strobe Calibration For ULPOSC Clock For PMIC 0............" */
	si_en_sel = (WRAP_RD32(PMIC_WRAP_SI_SAMPLE_CTRL) << 23) >> 29;
	si_ck_sel = (WRAP_RD32(PMIC_WRAP_SI_SAMPLE_CTRL) << 26) >> 31;
	clk_edge_no = (((si_en_sel * 2 + si_ck_sel) * 100) * CLK_26M_PRD / CLK_ULPOSC_PRD + 50) / 100;
	/* TINFO = "[DrvPWRAP_InitSiStrobe] The Sampling Clock Edge For ULPOSC Clock For PMIC 0 Is Chosen As Clock Edge %d.", clk_edge_no */
	si_en_sel = clk_edge_no / 2;
	si_ck_sel = clk_edge_no % 2;
	si_sample_ctrl = (1 << 19) | (si_en_sel << 6) | (si_ck_sel << 5);
	WRAP_WR32(PMIC_WRAP_SI_SAMPLE_CTRL_ULPOSC, si_sample_ctrl);
	/* TINFO = "[DrvPWRAP_InitSiStrobe] SI Strobe Calibration For ULPOSC Clock For PMIC 0 Done, *PMIC_WRAP_SI_SAMPLE_CTRL_ULPOSC = %x", si_sample_ctrl */
#endif /* end of #if PMIC_WRAP_ULPOSC_CAL */

	if (result_faulty != 0)
		return result_faulty;

	/* Read Test */
	pwrap_read_nochk(PMIC_DEW_READ_TEST_ADDR, &rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		PWRAPERR("_pwrap_init_sistrobe Read Test Failed, rdata = %x, exp = 0x5aa5\n", rdata);
		return 0x10;
	}
	PWRAPLOG("_pwrap_init_sistrobe Read Test ok\n");

	return 0;
}

static int __pwrap_InitSPISLV(void)
{
	pwrap_write_nochk(PMIC_RG_SRCLKEN_IN0_FILTER_EN_ADDR, 0xf0); /* turn on IO filter function */
	pwrap_write_nochk(PMIC_RG_SMT_SPI_CLK_ADDR, 0xf); /* turn on IO SMT function to improve noise immunity */
	pwrap_write_nochk(PMIC_GPIO_PULLEN0_CLR_ADDR, 0xf0); /* turn off IO pull function for power saving */
	pwrap_write_nochk(PMIC_RG_SLP_RW_EN_ADDR, 0x1); /* turn off IO pull function for power saving */
	pwrap_write_nochk(PMIC_RG_OCTL_SPI_CLK_ADDR, 0x8888); /* set IO driving strength to 4 mA */
#ifdef DUAL_PMICS
	pwrap_write_nochk(EXT_FILTER_CON0, 0xf); /* turn on IO filter function */
	pwrap_write_nochk(EXT_SMT_CON1, 0xf); /* turn on IO SMT function to improve noise immunity */
	pwrap_write_nochk(EXT_RG_SPI_CON, 0x1); /* turn off IO pull function for power saving */
	pwrap_write_nochk(EXT_DRV_CON1, 0x8888); /* set IO driving strength to 4 mA */
#endif

	return 0;
}

/******************************************************
 * Function : _pwrap_reset_spislv()
 * Description :
 * Parameter :
 * Return :
 ******************************************************/
static signed int _pwrap_reset_spislv(void)
{
	unsigned int ret = 0;
	unsigned int return_value = 0;

	WRAP_WR32(PMIC_WRAP_HPRIO_ARB_EN, DISABLE_ALL);
	WRAP_WR32(PMIC_WRAP_WRAP_EN, 0x0);
	WRAP_WR32(PMIC_WRAP_MUX_SEL, MANUAL_MODE);
	WRAP_WR32(PMIC_WRAP_MAN_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_DIO_EN, 0x0);

	WRAP_WR32(PMIC_WRAP_MAN_CMD, (OP_WR << 13) | (OP_CSL << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD, (OP_WR << 13) | (OP_OUTS << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD, (OP_WR << 13) | (OP_CSH << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD, (OP_WR << 13) | (OP_OUTS << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD, (OP_WR << 13) | (OP_OUTS << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD, (OP_WR << 13) | (OP_OUTS << 8));
	WRAP_WR32(PMIC_WRAP_MAN_CMD, (OP_WR << 13) | (OP_OUTS << 8));
	return_value =
	    wait_for_state_ready_init(wait_for_sync, TIMEOUT_WAIT_IDLE, PMIC_WRAP_WACS2_RDATA, 0);

	if (return_value != 0) {
		PWRAPERR("reset_spislv fail,ret=%x\n", return_value);
		ret = E_PWR_TIMEOUT;
		goto timeout;
	}

	WRAP_WR32(PMIC_WRAP_MAN_EN, 0x0);
	WRAP_WR32(PMIC_WRAP_MUX_SEL, WRAPPER_MODE);

timeout:
	WRAP_WR32(PMIC_WRAP_MAN_EN, 0x0);
	WRAP_WR32(PMIC_WRAP_MUX_SEL, WRAPPER_MODE);
	return ret;
}

static signed int _pwrap_init_reg_clock(unsigned int regck_sel)
{
	unsigned int rdata;

	WRAP_WR32(PMIC_WRAP_EXT_CK_WRITE, 0x1);
#ifndef SLV_CLK_1M
#ifndef DUAL_PMICS
	/* Set Read Dummy Cycle Number (Slave Clock is 18MHz) */
	_pwrap_wacs2_nochk(1, PMIC_DEW_RDDMY_NO_ADDR, 0x8, &rdata);
	WRAP_WR32(PMIC_WRAP_RDDMY, 0x8);
	PWRAPLOG("NO_SLV_CLK_1M Set Read Dummy Cycle\n");
#else
	_pwrap_wacs2_nochk(1, PMIC_DEW_RDDMY_NO_ADDR, 0x8, &rdata);
	_pwrap_wacs2_nochk(1, EXT_DEW_RDDMY_NO, 0x8, &rdata);
	WRAP_WR32(PMIC_WRAP_RDDMY, 0x0808);
	PWRAPLOG("NO_SLV_CLK_1M Set Read Dummy Cycle dual_pmics\n");
#endif
#else
#ifndef DUAL_PMICS
	/* Set Read Dummy Cycle Number (Slave Clock is 1MHz) */
	_pwrap_wacs2_nochk(1, PMIC_DEW_RDDMY_NO_ADDR, 0x68, &rdata);
	WRAP_WR32(PMIC_WRAP_RDDMY, 0x68);
	PWRAPLOG("SLV_CLK_1M Set Read Dummy Cycle\n");
#else
	_pwrap_wacs2_nochk(1, PMIC_DEW_RDDMY_NO_ADDR, 0x68, &rdata);
	_pwrap_wacs2_nochk(1, EXT_DEW_RDDMY_NO, 0x68, &rdata);
	WRAP_WR32(PMIC_WRAP_RDDMY, 0x6868);
	PWRAPLOG("SLV_CLK_1M Set Read Dummy Cycle dual_pmics\n");
#endif
#endif

	/* Config SPI Waveform according to reg clk */
	if (regck_sel == 1) { /* Slave Clock is 18MHz */
		/* wait data written into register => 4T_PMIC:
		 * CSHEXT_WRITE_START+EXT_CK+CSHEXT_WRITE_END+CSLEXT_START
		 */
		WRAP_WR32(PMIC_WRAP_CSHEXT_WRITE, 0x0);
		WRAP_WR32(PMIC_WRAP_CSHEXT_READ, 0x0);
		WRAP_WR32(PMIC_WRAP_CSLEXT_WRITE, 0x0);
		WRAP_WR32(PMIC_WRAP_CSLEXT_READ, 0x0200);
	} else { /*Safe Mode*/
		WRAP_WR32(PMIC_WRAP_CSHEXT_WRITE, 0x0f0f);
		WRAP_WR32(PMIC_WRAP_CSHEXT_READ, 0x0f0f);
		WRAP_WR32(PMIC_WRAP_CSLEXT_WRITE, 0x0f0f);
		WRAP_WR32(PMIC_WRAP_CSLEXT_READ, 0x0f0f);
	}

	return 0;
}

static int _pwrap_wacs2_write_test(int pmic_no)
{
	unsigned int rdata;

	if (pmic_no == 0) {
		pwrap_write_nochk(PMIC_DEW_WRITE_TEST_ADDR, 0xa55a);
		pwrap_read_nochk(PMIC_DEW_WRITE_TEST_ADDR, &rdata);
		if (rdata != 0xa55a) {
			PWRAPERR("Error: w_rdata = %x, exp = 0xa55a\n", rdata);
			return E_PWR_WRITE_TEST_FAIL;
		}
	}

#ifdef DUAL_PMICS
	if (pmic_no == 1) {
		pwrap_write_nochk(EXT_DEW_WRITE_TEST, 0xa55a);
		pwrap_read_nochk(EXT_DEW_WRITE_TEST, &rdata);
		if (rdata != 0xa55a) {
			PWRAPERR("Error: ext_w_rdata = %x, exp = 0xa55a\n", rdata);
			return E_PWR_WRITE_TEST_FAIL;
		}
	}
#endif

	return 0;
}

static unsigned int pwrap_read_test(void)
{
	unsigned int rdata = 0;
	unsigned int return_value = 0;
	/* Read Test */
	return_value = pwrap_wacs2_read(PMIC_DEW_READ_TEST_ADDR, &rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		PWRAPERR("Error: r_rdata=0x%x, exp=0x5aa5,ret=0x%x\n", rdata, return_value);
		return E_PWR_READ_TEST_FAIL;
	}
	else {
		PWRAPCRI("Read Test pass, return_value=0x%x\n", return_value);
	}

	return 0;
}
static unsigned int pwrap_write_test(void)
{
	unsigned int rdata = 0;
	unsigned int sub_return = 0;
	unsigned int sub_return1 = 0;

	/* Write test using WACS2 */
	PWRAPLOG("start pwrap_write\n");
	sub_return = pwrap_wacs2_write(PMIC_DEW_WRITE_TEST_ADDR, DEFAULT_VALUE_READ_TEST);
	PWRAPLOG("after pwrap_write\n");
	sub_return1 = pwrap_wacs2_read(PMIC_DEW_WRITE_TEST_ADDR, &rdata);
	if ((rdata != DEFAULT_VALUE_READ_TEST) || (sub_return != 0) || (sub_return1 != 0)) {
		PWRAPERR("Error: w_rdata=0x%x,exp=0xa55a,sub_return=0x%x,sub_return1=0x%x\n", rdata, sub_return,
		         sub_return1);
		return E_PWR_INIT_WRITE_TEST;
	}
	else {
		PWRAPCRI("Write Test pass\n");
	}

	return 0;
}
static void pwrap_ut(unsigned int ut_test)
{
	unsigned int sub_return = 0;
	switch (ut_test) {
		case 1:
			pwrap_write_test();
			break;
		case 2:
			pwrap_read_test();
			break;
		case 3:
#ifdef CONFIG_MTK_TINYSYS_SSPM_SUPPORT
			pwrap_wacs2_ipi(0x10010000 + 0xD8, 0xffffffff, (WRITE_CMD | WRITE_PMIC_WRAP));
			break;
#endif
		case 4:
			sub_return = pwrap_write_nochk(PMIC_DEW_WRITE_TEST_ADDR, 0x1234);
			sub_return = pwrap_write_nochk(PMIC_DEW_WRITE_TEST_ADDR, 0x4321);
			sub_return = pwrap_write_nochk(PMIC_DEW_WRITE_TEST_ADDR, 0xF0F0);
			break;
		default:
			PWRAPLOG("default test.\n");
			break;
	}
}

signed int pwrap_init(void)
{
	signed int sub_return = 0;
	unsigned int rdata = 0x0;

	PWRAPLOG("pwrap_init start!!!!!!!!!!!!!\n");

	/* Set SoC SPI IO Driving Strength to 4 mA */
	WRAP_WR32(0x10005720, (WRAP_RD32(0x10005720) & ~(0xF << 16)) |  (0x1 << 16));

	__pwrap_spi_clk_set();

	PWRAPLOG("__pwrap_spi_clk_set ok\n");

	/* Enable DCM */
	PWRAPLOG("Not need to enable DCM\n");

	/* Reset SPISLV */
	sub_return = _pwrap_reset_spislv();
	if (sub_return != 0) {
		PWRAPERR("reset_spislv fail,ret=%x\n", sub_return);
		return E_PWR_INIT_RESET_SPI;
	}
	PWRAPLOG("Reset SPISLV ok\n");

	/* Enable WRAP */
	WRAP_WR32(PMIC_WRAP_WRAP_EN, 0x1);
	PWRAPLOG("Enable WRAP ok\n");

	/* Enable WACS2 */
	WRAP_WR32(PMIC_WRAP_WACS2_EN, 0x1);
	WRAP_WR32(PMIC_WRAP_HPRIO_ARB_EN, 0x4); /* ONLY WACS2 */

	PWRAPLOG("Enable WACS2 ok\n");

	/* SPI Waveform Configuration. 0:safe mode, 1:18MHz */
	sub_return = _pwrap_init_reg_clock(1);
	if (sub_return != 0) {
		PWRAPERR("init_reg_clock fail,ret=%x\n", sub_return);
		return E_PWR_INIT_REG_CLOCK;
	}
	PWRAPLOG("_pwrap_init_reg_clock ok\n");

	/* SPI Slave Configuration */
	sub_return = __pwrap_InitSPISLV();
	if (sub_return != 0) {
		PWRAPERR("InitSPISLV Failed, ret = %x\n", sub_return);
		return -1;
	}

	/* Enable DIO mode */
	sub_return = _pwrap_init_dio(1);
	if (sub_return != 0) {
		PWRAPERR("dio test error, err=%x, ret=%x\n", 0x11, sub_return);
		return E_PWR_INIT_DIO;
	}
	PWRAPLOG("_pwrap_init_dio ok\n");

	/* Input data calibration flow; */
	sub_return = _pwrap_init_sistrobe(0);
	if (sub_return != 0) {
		PWRAPERR("InitSiStrobe fail, ret=%x\n", sub_return);
		return E_PWR_INIT_SIDLY;
	}
	PWRAPLOG("_pwrap_init_sistrobe ok\n");

#if 0
	/* Enable Encryption */
	sub_return = _pwrap_init_cipher();
	if (sub_return != 0) {
		PWRAPERR("Encryption fail, ret=%x\n", sub_return);
		return E_PWR_INIT_CIPHER;
	}
	PWRAPLOG("_pwrap_init_cipher ok\n");
#endif

	/*  Write test using WACS2.  check Write test default value */
	sub_return = _pwrap_wacs2_write_test(0);
	if (sub_return != 0) {
		PWRAPERR("write test 0 fail\n");
		return E_PWR_INIT_WRITE_TEST;
	}
	PWRAPLOG("_pwrap_wacs2_write_test ok\n");

#ifdef DUAL_PMICS
	sub_return = _pwrap_wacs2_write_test(1);
	if (sub_return != 0) {
		PWRAPERR("write test 1 fail\n");
		return E_PWR_INIT_WRITE_TEST;
	}
	PWRAPLOG("_pwrap_wacs2_write_test dual ok\n");
#endif

	/* Status update function initialization
	* 1. Signature Checking using CRC (CRC 0 only)
	* 2. EINT update
	* 3. Read back Auxadc thermal data for GPS
	*/
	_pwrap_InitStaUpd();
	PWRAPLOG("_pwrap_InitStaUpd ok\n");

	/* PMIC_WRAP starvation setting */
	_pwrap_starve_set();
	PWRAPLOG("_pwrap_starve_set ok\n");

	/* PMIC_WRAP enables */
	_pwrap_enable();
	PWRAPLOG("_pwrap_enable ok\n");

	/* Initialization Done */
	WRAP_WR32(PMIC_WRAP_INIT_DONE0, 0x1);
	WRAP_WR32(PMIC_WRAP_INIT_DONE2, 0x1);
	WRAP_WR32(PMIC_WRAP_INIT_DONE_P2P, 0x1);
	WRAP_WR32(PMIC_WRAP_INIT_DONE_MD32, 0x1);

	PWRAPLOG("pwrap_init Done!!!!!!!!!\n");

	/* WACS2 UT */
	pwrap_ut(2);
	pwrap_ut(1);
	PWRAPLOG("channel pass\n\r");

	/* Read Last three command */
	pwrap_read_nochk(PMIC_RECORD_CMD0_ADDR, &rdata);
	PWRAPCRI("RECORD_CMD0:  0x%x (Last one command addr)\n", (rdata & 0x3fff));
	pwrap_read_nochk(PMIC_RECORD_WDATA0_ADDR, &rdata);
	PWRAPCRI("RECORD_WDATA0:0x%x (Last one command wdata)\n", rdata);
	pwrap_read_nochk(PMIC_RECORD_CMD1_ADDR, &rdata);
	PWRAPCRI("RECORD_CMD1:  0x%x (Last second command addr)\n", (rdata & 0x3fff));
	pwrap_read_nochk(PMIC_RECORD_WDATA1_ADDR, &rdata);
	PWRAPCRI("RECORD_WDATA1:0x%x (Last second command wdata)\n", rdata);
	pwrap_read_nochk(PMIC_RECORD_CMD2_ADDR, &rdata);
	PWRAPCRI("RECORD_CMD2:  0x%x (Last third command addr)\n", (rdata & 0x3fff));
	pwrap_read_nochk(PMIC_RECORD_WDATA2_ADDR, &rdata);
	PWRAPCRI("RECORD_WDATA2:0x%x (Last third command wdata)\n", rdata);

	/* Enable Command Recording */
	sub_return = pwrap_write_nochk(PMIC_RG_SPI_RSV_ADDR, 0x3);
	if (sub_return != 0)
		PWRAPERR("enable spi debug fail, ret=%x\n", sub_return);
	PWRAPLOG("enable spi debug ok\n");

	/* Clear Last three record command */
	sub_return = pwrap_write_nochk(PMIC_RG_SPI_RECORD_CLR_ADDR, 0x1);
	if (sub_return != 0)
		PWRAPERR("clear record command fail, ret=%x\n", sub_return);
	sub_return = pwrap_write_nochk(PMIC_RG_SPI_RECORD_CLR_ADDR, 0x0);
	if (sub_return != 0)
		PWRAPERR("clear record command fail, ret=%x\n", sub_return);
	PWRAPLOG("clear record command ok\n\r");

	/* WRAP RG monitor */
#ifdef PMIC_WRAP_MATCH_SUPPORT
	/* enable matching mode */
	PWRAPCRI("PMIC_WRAP_MONITOR_MODE = Matching Mode\n");

	/* enable Matching interrupt for bit 13 */
	WRAP_WR32(PMIC_WRAP_INT1_EN, WRAP_RD32(PMIC_WRAP_INT1_EN) | 0x2000);
	PWRAPCRI("PMIC_WRAP_INT1_EN = 0x%x\n", WRAP_RD32(PMIC_WRAP_INT1_EN));

	WRAP_WR32(PMIC_WRAP_MONITOR_CTRL_1, 0x1ffff);
	WRAP_WR32(PMIC_WRAP_MONITOR_CTRL_2, 0xffff8089); /* 0x8089 = monitor write addr 0x89 << 1 = 0x112 */
	WRAP_WR32(PMIC_WRAP_MONITOR_CTRL_3, 0x01e00100); /* 0x0100 = monitor bit 5~8 when 0x4 -> 0x8 */
	WRAP_WR32(PMIC_WRAP_MONITOR_CTRL_0, 0x8); /* clear log */
	WRAP_WR32(PMIC_WRAP_MONITOR_CTRL_0, 0x5); /* Matching mode and Stop recording after interrupt trigger */
	PWRAPCRI("PMIC_WRAP_MONITOR_CTRL_0 = 0x%x\n", WRAP_RD32(PMIC_WRAP_MONITOR_CTRL_0));
	PWRAPCRI("PMIC_WRAP_MONITOR_CTRL_1 = 0x%x\n", WRAP_RD32(PMIC_WRAP_MONITOR_CTRL_1));
	PWRAPCRI("PMIC_WRAP_MONITOR_CTRL_2 = 0x%x\n", WRAP_RD32(PMIC_WRAP_MONITOR_CTRL_2));
	PWRAPCRI("PMIC_WRAP_MONITOR_CTRL_3 = 0x%x\n", WRAP_RD32(PMIC_WRAP_MONITOR_CTRL_3));
#else
	/* enable logging mode */
	PWRAPLOG("PMIC_WRAP_MONITOR_MODE = Logging Mode\n");
	WRAP_WR32(PMIC_WRAP_MONITOR_CTRL_1, 0x1feff); /* disable monitor MD HW channel (CH8) */
	WRAP_WR32(PMIC_WRAP_MONITOR_CTRL_2, 0x0); /* record all addr (mask+addr) */
	WRAP_WR32(PMIC_WRAP_MONITOR_CTRL_3, 0x0); /* record all wdata (mask+addr) */
	WRAP_WR32(PMIC_WRAP_MONITOR_CTRL_0, 0x8); /* clear log */
	WRAP_WR32(PMIC_WRAP_MONITOR_CTRL_0, 0x1); /* reenable (Matching mode and Continue recording after interrupt trigger) */
	PWRAPLOG("PMIC_WRAP_MONITOR_CTRL_0 = 0x%x\n", WRAP_RD32(PMIC_WRAP_MONITOR_CTRL_0));
	PWRAPLOG("PMIC_WRAP_MONITOR_CTRL_1 = 0x%x\n", WRAP_RD32(PMIC_WRAP_MONITOR_CTRL_1));
	PWRAPLOG("PMIC_WRAP_MONITOR_CTRL_2 = 0x%x\n", WRAP_RD32(PMIC_WRAP_MONITOR_CTRL_2));
	PWRAPLOG("PMIC_WRAP_MONITOR_CTRL_3 = 0x%x\n", WRAP_RD32(PMIC_WRAP_MONITOR_CTRL_3));
#endif

	return 0;
}

/*-------------------pwrap debug---------------------*/
static inline void pwrap_dump_ap_register(void)
{
	unsigned int i = 0;
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
	unsigned int *reg_addr;
#else
	unsigned int reg_addr;
#endif
	unsigned int reg_value = 0;

	PWRAPERR("dump reg\n");
	for (i = 0; i <= PMIC_WRAP_REG_RANGE; i++) {
#if (PMIC_WRAP_KERNEL) || (PMIC_WRAP_CTP)
		reg_addr = (unsigned int *) (PMIC_WRAP_BASE + i * 4);
		reg_value = WRAP_RD32(((unsigned int *) (PMIC_WRAP_BASE + i * 4)));
		PWRAPERR("addr:0x%p = 0x%x\n", reg_addr, reg_value);
#else
		reg_addr = (PMIC_WRAP_BASE + i * 4);
		reg_value = WRAP_RD32(reg_addr);
		PWRAPERR("addr:0x%x = 0x%x\n", reg_addr, reg_value);
#endif
	}
}

void pwrap_dump_all_register(void)
{
	pwrap_dump_ap_register();
}

static int is_pwrap_init_done(void)
{
	int ret = 0;

	ret = WRAP_RD32(PMIC_WRAP_INIT_DONE2);
	PWRAPLOG("is_pwrap_init_done %d\n", ret);
	if ((ret & 0x1) == 1)
		return 0;
	return -1;
}

signed int pwrap_init_preloader(void)
{
	unsigned int pwrap_ret = 0, i = 0;

	PWRAPFUC();
	if (0 == is_pwrap_init_done()) {
		PWRAPLOG("[PMIC_WRAP]wrap_init already init, do nothing\n");
		return 0;
	}
	for (i = 0; i < 3; i++) {
		pwrap_ret = pwrap_init();
		if (pwrap_ret != 0) {
			PWRAPERR("init fail, ret=%x.\n\r",pwrap_ret);
			if (i >= 2)
				ASSERT(0);
		} else {
			PWRAPCRI("init pass, ret=%x.\n\r",pwrap_ret);
			break;
		}
	}

	return 0;
}

#endif /*endif PMIC_WRAP_NO_PMIC */
