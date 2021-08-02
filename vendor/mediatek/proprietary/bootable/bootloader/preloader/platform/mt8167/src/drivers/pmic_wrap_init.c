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

#include <timer.h>
#include <typedefs.h>
#include "pmic_wrap_init.h"

#define PWRAPTAG                "[PWRAP] "
#define pwrap_log(fmt, arg ...)   printf(PWRAPTAG fmt, ## arg)
#define pwrap_err(fmt, arg ...)   printf(PWRAPTAG "ERROR,line=%d" fmt, \
					__LINE__, ## arg)

static S32 pwrap_read_nochk(U32 adr, U32 *rdata);
static S32 pwrap_write_nochk(U32 adr, U32 wdata);

/* define macro and inline function (for do while loop) */

typedef U32 (*loop_condition_fp)(U32);

static inline U32 wait_for_fsm_vldclr(U32 x)
{
	return ((x >> RDATA_WACS_FSM_SHIFT) & RDATA_WACS_FSM_MASK) !=
		WACS_FSM_WFVLDCLR;
}

static inline U32 wait_for_sync(U32 x)
{
	return ((x >> RDATA_SYNC_IDLE_SHIFT) & RDATA_SYNC_IDLE_MASK) !=
		WACS_SYNC_IDLE;
}

static inline U32 wait_for_idle_and_sync(U32 x)
{
	return ((((x >> RDATA_WACS_FSM_SHIFT) & RDATA_WACS_FSM_MASK) !=
		WACS_FSM_IDLE) || (((x >> RDATA_SYNC_IDLE_SHIFT) &
		RDATA_SYNC_IDLE_MASK)!= WACS_SYNC_IDLE));
}

static inline U32 wait_for_cipher_ready(U32 x)
{
	return x != 3;
}

static U64 _pwrap_get_current_time(void)
{
  return gpt4_get_current_tick();
}

static BOOL _pwrap_timeout_ns(U64 start_time, U64 elapse_time)
{
  return gpt4_timeout_tick(start_time, elapse_time);
}

static U64 _pwrap_time2ns(U64 time_us)
{
  return gpt4_time2tick_us(time_us);
}

static inline U32 wait_for_state_idle(U32 timeout_us, void *wacs_register,
				      void *wacs_vldclr_register,
				      U32 *read_reg)
{
	U32 reg_rdata;
	U64 start_time_ns = 0, timeout_ns = 0;

	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(timeout_us);

	do {
		reg_rdata = read32((wacs_register));
		/* if last read command timeout,clear vldclr bit
		   read command state machine:FSM_REQ-->wfdle-->WFVLDCLR;
		   write:FSM_REQ-->idle */
		switch (((reg_rdata >> RDATA_WACS_FSM_SHIFT) &
			RDATA_WACS_FSM_MASK)) {
		case WACS_FSM_WFVLDCLR:
			write32(wacs_vldclr_register, 1);
			pwrap_err("WACS_FSM = PMIC_WRAP_WACS_VLDCLR\n");
			break;
		case WACS_FSM_WFDLE:
			pwrap_err("WACS_FSM = WACS_FSM_WFDLE\n");
			break;
		case WACS_FSM_REQ:
			pwrap_err("WACS_FSM = WACS_FSM_REQ\n");
			break;
		default:
			break;
		}

		if (_pwrap_timeout_ns(start_time_ns, timeout_ns))
			return E_PWR_WAIT_IDLE_TIMEOUT;

	} while (((reg_rdata >> RDATA_WACS_FSM_SHIFT) & RDATA_WACS_FSM_MASK) !=
		 WACS_FSM_IDLE);        /* IDLE State */
	if (read_reg)
		*read_reg = reg_rdata;
	return 0;
}

static inline U32 wait_for_state_ready(loop_condition_fp fp, U32 timeout_us,
				       void *wacs_register, U32 *read_reg)
{
	U32 reg_rdata;
	U64 start_time_ns = 0, timeout_ns = 0;

	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(timeout_us);

	do {
		reg_rdata = read32((wacs_register));

		if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
			pwrap_err("timeout when waiting for idle\n");
			return E_PWR_WAIT_IDLE_TIMEOUT;
		}
	} while (fp(reg_rdata));        /* IDLE State */
	if (read_reg)
		*read_reg = reg_rdata;
	return 0;
}

static S32 pwrap_wacs(U32 write, U32 adr, U32 wdata, U32 *rdata, U32 init_check)
{
	U32 reg_rdata = 0;
	U32 wacs_write = 0;
	U32 wacs_adr = 0;
	U32 wacs_cmd = 0;
	U32 return_value = 0;

	if (init_check) {
		reg_rdata = read32(&mt8167_pwrap->wacs2_rdata);
		/* Prevent someone to used pwrap before pwrap init */
		if (((reg_rdata >> RDATA_INIT_DONE_SHIFT) &
		    RDATA_INIT_DONE_MASK) != WACS_INIT_DONE) {
			pwrap_err("initialization isn't finished \n");
			return E_PWR_NOT_INIT_DONE;
		}
	}
	reg_rdata = 0;
	/* Check IDLE in advance */
	return_value = wait_for_state_idle(TIMEOUT_WAIT_IDLE_US,
					   &mt8167_pwrap->wacs2_rdata,
					   &mt8167_pwrap->wacs2_vldclr,
					   0);
	if (return_value != 0) {
		pwrap_err("wait_for_fsm_idle fail,return_value=%d\n",
			  return_value);
		return E_PWR_WAIT_IDLE_TIMEOUT;
	}
	wacs_write = write << 31;
	wacs_adr = (adr >> 1) << 16;
	wacs_cmd = wacs_write | wacs_adr | wdata;

	write32(&mt8167_pwrap->wacs2_cmd, wacs_cmd);
	if (write == 0) {
		if (NULL == rdata) {
			pwrap_err("rdata is a NULL pointer\n");
			return E_PWR_INVALID_ARG;
		}
		return_value = wait_for_state_ready(wait_for_fsm_vldclr,
						    TIMEOUT_READ_US,
						    &mt8167_pwrap->wacs2_rdata,
						    &reg_rdata);
		if (return_value != 0) {
			pwrap_err("wait_for_fsm_vldclr fail,return_value=%d\n",
				  return_value);
			return E_PWR_WAIT_IDLE_TIMEOUT_READ;
		}
		*rdata = ((reg_rdata >> RDATA_WACS_RDATA_SHIFT)
			  & RDATA_WACS_RDATA_MASK);
		write32(&mt8167_pwrap->wacs2_vldclr, 1);
	}

	return 0;
}

/* external API for pmic_wrap user */

S32 pwrap_wacs2(U32 write, U32 adr, U32 wdata, U32 *rdata)
{
	return pwrap_wacs(write, adr, wdata, rdata, 1);
}

S32 pwrap_read(U32 adr, U32 *rdata)
{
	return pwrap_wacs(0, adr, 0, rdata, 1);
}

S32 pwrap_write(U32 adr, U32 wdata)
{
	return pwrap_wacs(1, adr, wdata, 0, 1);
}

static S32 pwrap_read_nochk(U32 adr, U32 *rdata)
{
	return pwrap_wacs(0, adr, 0, rdata, 0);
}

static S32 pwrap_write_nochk(U32 adr, U32 wdata)
{
	return pwrap_wacs(1, adr, wdata, 0, 0);
}

/* call it in pwrap_init,mustn't check init done */
static S32 pwrap_init_dio(U32 dio_en)
{
	U32 rdata = 0;
	U32 return_value = 0;

	pwrap_write_nochk(MT6392_DEW_DIO_EN, dio_en);

	/* Check IDLE in advance */
	return_value =
	wait_for_state_ready(wait_for_idle_and_sync,
			     TIMEOUT_WAIT_IDLE_US,
			     &mt8167_pwrap->wacs2_rdata,
			     0);
	if (return_value != 0) {
		pwrap_err("%s fail,return_value=%x\n", __func__, return_value);
		return return_value;
	}
	write32(&mt8167_pwrap->dio_en, dio_en);
	/* Read Test */
	pwrap_read_nochk(MT6392_DEW_READ_TEST, &rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		pwrap_err("fail,dio_en = %x, READ_TEST rdata=%x\n", dio_en,
			  rdata);
		return E_PWR_READ_TEST_FAIL;
	}

	return 0;
}

static int pwrap_init_cipher(void)
{
	U32 rdata = 0;
	U32 return_value = 0;
	U32 start_time_ns=0, timeout_ns=0;

	/* Config cipher mode @AP */
	write32(&mt8167_pwrap->cipher_swrst, 0x1);
	write32(&mt8167_pwrap->cipher_swrst, 0x0);
	write32(&mt8167_pwrap->cipher_key_sel, 0x1);
	write32(&mt8167_pwrap->cipher_iv_sel, 0x2);
	write32(&mt8167_pwrap->cipher_en, 0x1);

	/* Config cipher mode @PMIC */
	pwrap_write_nochk(MT6392_DEW_CIPHER_SWRST,   0x1);
	pwrap_write_nochk(MT6392_DEW_CIPHER_SWRST,   0x0);
	pwrap_write_nochk(MT6392_DEW_CIPHER_KEY_SEL, 0x1);
	pwrap_write_nochk(MT6392_DEW_CIPHER_IV_SEL,  0x2);
	pwrap_write_nochk(MT6392_DEW_CIPHER_EN, 0x1);

	/* wait for cipher data ready@AP */
	return_value = wait_for_state_ready(wait_for_cipher_ready,
			     TIMEOUT_WAIT_IDLE_US,
			     &mt8167_pwrap->cipher_rdy,
			     0);
	if (return_value != 0) {
		pwrap_err("%s fail,return_value=%x\n", __func__, return_value);
		return return_value;
	}

	/* wait for cipher data ready@PMIC */
	start_time_ns = _pwrap_get_current_time();
	timeout_ns = _pwrap_time2ns(0xFFFFFF);
	do
	{
		if (_pwrap_timeout_ns(start_time_ns, timeout_ns))
		{
			pwrap_err("timeout when waiting for cipher data ready@PMIC\n");
			return E_PWR_WAIT_IDLE_TIMEOUT;
		}
		pwrap_read_nochk(MT6392_DEW_CIPHER_RDY,&rdata);
	} while( rdata != 0x1 ); /* cipher_ready */

	pwrap_write_nochk(MT6392_DEW_CIPHER_MODE, 0x1);

	/* wait for cipher mode idle */
	return_value = wait_for_state_ready(wait_for_idle_and_sync,
			     TIMEOUT_WAIT_IDLE_US,
			     &mt8167_pwrap->wacs2_rdata,
			     0);
	if (return_value != 0) {
		pwrap_err("%s fail,return_value=%x\n", __func__, return_value);
		return return_value;
	}

	write32(&mt8167_pwrap->cipher_mode, 0x1);

	/* Read Test */
	pwrap_read_nochk(MT6392_DEW_READ_TEST, &rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		pwrap_err("%s fail, READ_TEST rdata=%x\n", rdata);
		return E_PWR_READ_TEST_FAIL;
	}

	return 0;
}

/*
 * pwrap_init_sidly - configure serial input delay
 *
 * This configures the serial input delay. We can configure 0, 2, 4 or 6ns
 * delay. Do a read test with all possible values and chose the best delay.
 */
static S32 pwrap_init_sidly(void)
{
	U32 rdata = 0;
	U32 i;
	U32 pass = 0;
	U32 sidly = 0;

	for (i = 0; i < 4; i++) {
		write32(&mt8167_pwrap->sidly, i);
		pwrap_wacs(0, MT6392_DEW_READ_TEST, 0, &rdata, 0);
		if (rdata == DEFAULT_VALUE_READ_TEST)
			pass |= 1 << i;
	}

	/*
	 * Config SIDLY according to results
	 * Pass range should be continuously or return failed
	 */
	switch (pass) {
	/* only 1 pass, choose it */
	case 1 << 0:
		sidly = 0;
		break;
	case 1 << 1:
		sidly = 1;
		break;
	case 1 << 2:
		sidly = 2;
		break;
	case 1 << 3:
		sidly = 3;
		break;
	/* two pass, choose the one on SIDLY boundary */
	case (1 << 0) | (1 << 1):
		sidly = 0;
		break;
	case (1 << 1) | (1 << 2): /* no boundary, choose smaller one */
		sidly = 1;
		break;
	case (1 << 2) | (1 << 3):
		sidly = 3;
		break;
	/* three pass, choose the middle one */
	case (1 << 0) | (1 << 1) | (1 << 2):
		sidly = 1;
		break;
	case (1 << 1) | (1 << 2) | (1 << 3):
		sidly = 2;
		break;
	/* four pass, choose the smaller middle one */
	case (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3):
		sidly = 1;
		break;
	/* pass range not continuous, should not happen */
	default:
		printf(PWRAPTAG "ERROR sidly pass range not continuous\n");
	}

	write32(&mt8167_pwrap->sidly, sidly);

	return 0;
}

static S32 pwrap_reset_spislv(void)
{
	U32 ret = 0;
	U32 return_value = 0;

	write32(&mt8167_pwrap->hiprio_arb_en, 0);
	write32(&mt8167_pwrap->wrap_en, 0);
	write32(&mt8167_pwrap->mux_sel, 1);
	write32(&mt8167_pwrap->man_en, 1);
	write32(&mt8167_pwrap->dio_en, 0);

	write32(&mt8167_pwrap->man_cmd, (OP_WR << 13) | (OP_CSL << 8));
	/* to reset counter */
	write32(&mt8167_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));
	write32(&mt8167_pwrap->man_cmd, (OP_WR << 13) | (OP_CSH << 8));
	/*
	 * In order to pull CSN signal to PMIC,
	 * PMIC will count it then reset spi slave
	*/
	write32(&mt8167_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));
	write32(&mt8167_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));
	write32(&mt8167_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));
	write32(&mt8167_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));

	return_value = wait_for_state_ready(wait_for_sync,
					    TIMEOUT_WAIT_IDLE_US,
					    &mt8167_pwrap->wacs2_rdata, 0);
	if (return_value != 0) {
		pwrap_err("%s fail,return_value=%x\n", __func__, return_value);
		ret = E_PWR_TIMEOUT;
	}

	write32(&mt8167_pwrap->man_en, 0);
	write32(&mt8167_pwrap->mux_sel, 0);

	return ret;
}

static S32 pwrap_init_reg_clock(enum pmic_regck regck_sel)
{
	U32 wdata = 0;
	U32 rdata = 0;

	/* Set reg clk freq */
	wdata = 0x3;
	pwrap_write_nochk(MT6392_TOP_CKCON1_CLR, wdata);
	pwrap_read_nochk(MT6392_TOP_CKCON1,  &rdata);

	if(rdata & 0x3 != 0)
	{
		pwrap_err("pwrap_init_reg_clock,rdata=%x\n", rdata);
		return E_PWR_INIT_REG_CLOCK;
	}

	pwrap_write_nochk(MT6392_DEW_RDDMY_NO, 0x8);
	/* Config SPI Waveform according to reg clk */
	switch (regck_sel) {
	case REG_CLOCK_18MHZ:
		write32(&mt8167_pwrap->rddmy, 0xc);
		write32(&mt8167_pwrap->cshext_write, 0x0);
		write32(&mt8167_pwrap->cshext_read, 0x4);
		write32(&mt8167_pwrap->cslext_start, 0x0);
		write32(&mt8167_pwrap->cslext_end, 0x4);
		break;
	case REG_CLOCK_26MHZ:
		write32(&mt8167_pwrap->rddmy, 0x8);
		write32(&mt8167_pwrap->cshext_write, 0x5);
		write32(&mt8167_pwrap->cshext_read, 0x0);
		write32(&mt8167_pwrap->cslext_start, 0x2);
		write32(&mt8167_pwrap->cslext_end, 0x2);
		break;
	default:
		write32(&mt8167_pwrap->rddmy, 0xf);
		write32(&mt8167_pwrap->cshext_write, 0xf);
		write32(&mt8167_pwrap->cshext_read, 0xf);
		write32(&mt8167_pwrap->cslext_start, 0xf);
		write32(&mt8167_pwrap->cslext_end, 0xf);
		break;
	}

	return 0;
}

S32 pwrap_init_preloader(void)
{
	S32 sub_return = 0;
	S32 sub_return1 = 0;
	U32 rdata = 0x0;
	U32 backup = 0x0;
	u32 cg_mask = 0;

	/* Turn off module clock */
	cg_mask = PWRAP_CG_AP | PWRAP_CG_MD | PWRAP_CG_CONN | PWRAP_CG_26M;
	write32(MODULE_SW_CG_1_SET, cg_mask);

	/* dummy read to add latency (to wait clock turning off) */
	rdata = read32(&mt8167_pwrap->swrst);

	/* Toggle module reset */
	write32(&mt8167_pwrap->swrst, 1);

	write32(&mt8167_pwrap->swrst, 0);

	/* Turn on module clock */
	write32(MODULE_SW_CG_1_CLR, cg_mask);

	/* Turn on module clock dcm (in global_con) */
	write32(MODULE_SW_CG_3_SET, read32(MODULE_SW_CG_3_SET) | PMIC_B_DCM_EN | PMIC_SPI_DCM_EN);

	write32(CLK_CFG_5_CLR, CLK_SPI_CK_26M);

	/* Enable DCM */
	write32(&mt8167_pwrap->dcm_en, 3);
	write32(&mt8167_pwrap->dcm_dbc_prd, 0);

	write32(&mt8167_pwrap->op_type, 0);
	write32(&mt8167_pwrap->msb_first, 1);

	/* Reset SPISLV */
	sub_return = pwrap_reset_spislv();
	if (sub_return != 0) {
		pwrap_err("error,pwrap_reset_spislv fail,sub_return=%x\n",
			  sub_return);
		return E_PWR_INIT_RESET_SPI;
	}
	/* Enable WACS2 */
	write32(&mt8167_pwrap->wrap_en, 1);
	write32(&mt8167_pwrap->hiprio_arb_en, WACS2);
	write32(&mt8167_pwrap->wacs2_en, 1);

	write32(&mt8167_pwrap->rddmy, 0xf);

	/* SIDLY setting */
	sub_return = pwrap_init_sidly();
	if (sub_return != 0) {
		pwrap_err("error,pwrap_init_sidly fail,sub_return=%x\n",
			  sub_return);
		return E_PWR_INIT_SIDLY;
	}
	/*
	 * SPI Waveform Configuration
	 * 18MHz/26MHz/safe mode/
	 */
	sub_return = pwrap_init_reg_clock(REG_CLOCK_26MHZ);
	if (sub_return != 0) {
		pwrap_err("error,pwrap_init_reg_clock fail,sub_return=%x\n",
			  sub_return);
		return E_PWR_INIT_REG_CLOCK;
	}

	/* Enable DIO mode */
	sub_return = pwrap_init_dio(1);
	if (sub_return != 0) {
		pwrap_err("pwrap_init_dio fail, sub_return=%x\n", sub_return);
		return E_PWR_INIT_DIO;
	}

	/* Enable Encryption */
	sub_return = pwrap_init_cipher();
	if (sub_return != 0) {
		pwrap_err("pwrap_init_cipher fail, sub_return=%x\n", sub_return);
		return E_PWR_INIT_CIPHER;
	}

	/*
	 * Write test using WACS2,
	 * make sure the read/write function ready
	 */
	sub_return = pwrap_write_nochk(MT6392_DEW_WRITE_TEST, WRITE_TEST_VALUE);
	sub_return1 = pwrap_read_nochk(MT6392_DEW_WRITE_TEST, &rdata);
	if ((rdata != WRITE_TEST_VALUE) || (sub_return != 0)
	    || (sub_return1 != 0)) {
		pwrap_err("write error, rdata=%x, return=%x, return1=%x\n",
			  rdata, sub_return, sub_return1);
		return E_PWR_INIT_WRITE_TEST;
	}

	/* Signature Checking - Using CRC
	 * should be the last to modify WRITE_TEST
	 */
	sub_return = pwrap_write_nochk(MT6392_DEW_CRC_EN, 0x1);
	if (sub_return != 0) {
		pwrap_err("enable CRC fail,sub_return=%x\n", sub_return);
		return E_PWR_INIT_ENABLE_CRC;
	}
	write32(&mt8167_pwrap->crc_en, 0x1);
	write32(&mt8167_pwrap->sig_mode, 0x0);
	write32(&mt8167_pwrap->sig_adr, read32(&mt8167_pwrap->sig_adr) | MT6392_DEW_CRC_VAL);

	/* PMIC_WRAP enables */
	write32(&mt8167_pwrap->hiprio_arb_en, 0xff);
	write32(&mt8167_pwrap->wacs0_en, 0x1);
	write32(&mt8167_pwrap->wacs1_en, 0x1);
	write32(&mt8167_pwrap->staupd_prd, 0x5);

	/* Initialization Done */
	write32(&mt8167_pwrap->init_done2, 0x1);
	write32(&mt8167_pwrap->init_done0, 0x1);
	write32(&mt8167_pwrap->init_done1, 0x1);

	return 0;
}
