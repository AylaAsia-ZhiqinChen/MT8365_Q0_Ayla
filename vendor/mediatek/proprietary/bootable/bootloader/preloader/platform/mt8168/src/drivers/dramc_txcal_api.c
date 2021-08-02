/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its
 * licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek
 * Software if you have agreed to and been bound by the applicable license
 * agreement with MediaTek ("License Agreement") and been granted explicit
 * permission to do so within the License Agreement ("Permitted User").
 * If you are not a Permitted User, please cease any access or use of MediaTek
 * Software immediately.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY
 * DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY
 * ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY
 * THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE.
 * MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A
 * PARTICULAR STANDARD OR OPEN FORUM.
 * RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
 * LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/*
 * @file dramc_txcal_api.c
 *  Basic DRAMC calibration API implementation
 */

/* -----------------------------------------------------------------------------
 *  Include files
 * -----------------------------------------------------------------------------
 */
#include "dramc_common.h"
#include "x_hal_io.h"
#include "dramc_api.h"

#define VALUE_2		2
#define VALUE_3		3
#define VALUE_64	64

#define WRITE_LEVELING_MOVD_DQS 1	/* UI */

/* -----------------------------------------------------------------------------
 *  Global variables
 * -----------------------------------------------------------------------------
 */
#if SIMULATION_TX_PERBIT
unsigned short tx_dq_pre_cal_lp4[DQS_NUMBER];
#endif
#if SIMULATION_WRITE_LEVELING
unsigned char wrlevel_done[CHANNEL_NUM] = { 0 };
unsigned char g_need_save_wl = 1;
#endif

#if SIMULATION_WRITE_LEVELING
/* dramc_write_leveling
 *  start Write Leveling Calibration.
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @param  apply           (unsigned char): 0 don't apply the register
 *		we set  1 apply the register we set ,default don't apply.
 *  @retval status          (DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
DRAM_STATUS_T execute_move_dramc_delay(DRAMC_CTX_T *p,
	REG_TRANSFER_T regs[], signed char shift_ui)
{
	signed int high_level_delay, delay_sum;
	unsigned int tmp0p5t, tmp2t;
	unsigned char data_rate_div_shift = 0;
	DRAM_STATUS_T move_result;

	if (get_div_mode(p) == DIV4_MODE)
		data_rate_div_shift = 2;
	else
		data_rate_div_shift = 3;

	tmp0p5t = io_32_read_fld_align(DRAMC_REG_ADDR(regs[0].addr),
		regs[0].fld) & (~(1 << data_rate_div_shift));
	tmp2t = io_32_read_fld_align(DRAMC_REG_ADDR(regs[1].addr),
		regs[1].fld);

	high_level_delay = (tmp2t << data_rate_div_shift) + tmp0p5t;
	delay_sum = (high_level_delay + shift_ui);

	if (delay_sum < 0) {
		tmp0p5t = 0;
		tmp2t = 0;
		move_result = DRAM_FAIL;
	} else {
		tmp2t = delay_sum >> data_rate_div_shift;
		tmp0p5t = delay_sum - (tmp2t << data_rate_div_shift);
		move_result = DRAM_OK;
	}

	io_32_write_fld_align(DRAMC_REG_ADDR(regs[0].addr), tmp0p5t,
		regs[0].fld);
	io_32_write_fld_align(DRAMC_REG_ADDR(regs[1].addr), tmp2t,
		regs[1].fld);

	return move_result;
}

void move_dramc_tx_dqs(DRAMC_CTX_T *p, unsigned char byte_idx,
	signed char shift_ui)
{
	REG_TRANSFER_T transfer_reg[2];

	switch (byte_idx) {
	case 0:
		/*  DQS0 */
		transfer_reg[0].addr = DRAMC_REG_SHU_SELPH_DQS1;
		transfer_reg[0].fld = SHU_SELPH_DQS1_DLY_DQS0;
		transfer_reg[1].addr = DRAMC_REG_SHU_SELPH_DQS0;
		transfer_reg[1].fld = SHU_SELPH_DQS0_TXDLY_DQS0;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;

	case 1:
		/*  DQS1 */
		transfer_reg[0].addr = DRAMC_REG_SHU_SELPH_DQS1;
		transfer_reg[0].fld = SHU_SELPH_DQS1_DLY_DQS1;
		transfer_reg[1].addr = DRAMC_REG_SHU_SELPH_DQS0;
		transfer_reg[1].fld = SHU_SELPH_DQS0_TXDLY_DQS1;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;

	case 2:
		/*  DQS2 */
		transfer_reg[0].addr = DRAMC_REG_SHU_SELPH_DQS1;
		transfer_reg[0].fld = SHU_SELPH_DQS1_DLY_DQS2;
		transfer_reg[1].addr = DRAMC_REG_SHU_SELPH_DQS0;
		transfer_reg[1].fld = SHU_SELPH_DQS0_TXDLY_DQS2;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;

	case 3:
		/*  DQS3 */
		transfer_reg[0].addr = DRAMC_REG_SHU_SELPH_DQS1;
		transfer_reg[0].fld = SHU_SELPH_DQS1_DLY_DQS3;
		transfer_reg[1].addr = DRAMC_REG_SHU_SELPH_DQS0;
		transfer_reg[1].fld = SHU_SELPH_DQS0_TXDLY_DQS3;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;

	default:
		break;
	}
}

void move_dramc_tx_dqs_oen(DRAMC_CTX_T *p, unsigned char byte_idx,
	signed char shift_ui)
{
	REG_TRANSFER_T transfer_reg[2];

	switch (byte_idx) {
	case 0:
		/* DQS_OEN_0 */
		transfer_reg[0].addr = DRAMC_REG_SHU_SELPH_DQS1;
		transfer_reg[0].fld = SHU_SELPH_DQS1_DLY_OEN_DQS0;
		transfer_reg[1].addr = DRAMC_REG_SHU_SELPH_DQS0;
		transfer_reg[1].fld = SHU_SELPH_DQS0_TXDLY_OEN_DQS0;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;

	case 1:
		/* DQS_OEN_1 */
		transfer_reg[0].addr = DRAMC_REG_SHU_SELPH_DQS1;
		transfer_reg[0].fld = SHU_SELPH_DQS1_DLY_OEN_DQS1;
		transfer_reg[1].addr = DRAMC_REG_SHU_SELPH_DQS0;
		transfer_reg[1].fld = SHU_SELPH_DQS0_TXDLY_OEN_DQS1;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;

	case 2:
		/* DQS_OEN_2 */
		transfer_reg[0].addr = DRAMC_REG_SHU_SELPH_DQS1;
		transfer_reg[0].fld = SHU_SELPH_DQS1_DLY_OEN_DQS2;
		transfer_reg[1].addr = DRAMC_REG_SHU_SELPH_DQS0;
		transfer_reg[1].fld = SHU_SELPH_DQS0_TXDLY_OEN_DQS2;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;

	case 3:
		/* DQS_OEN_3 */
		transfer_reg[0].addr = DRAMC_REG_SHU_SELPH_DQS1;
		transfer_reg[0].fld = SHU_SELPH_DQS1_DLY_OEN_DQS3;
		transfer_reg[1].addr = DRAMC_REG_SHU_SELPH_DQS0;
		transfer_reg[1].fld = SHU_SELPH_DQS0_TXDLY_OEN_DQS3;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;

	default:
		break;
	}
}

void move_dramc_tx_dq(DRAMC_CTX_T *p, unsigned char byte_idx,
	signed char shift_ui)
{
	REG_TRANSFER_T transfer_reg[2];

	switch (byte_idx) {
	case 0:
		/* DQM0 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ3;
		transfer_reg[0].fld = SHURK0_SELPH_DQ3_DLY_DQM0;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ1;
		transfer_reg[1].fld = SHURK0_SELPH_DQ1_TXDLY_DQM0;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);

		/* DQ0 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ2;
		transfer_reg[0].fld = SHURK0_SELPH_DQ2_DLY_DQ0;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ0;
		transfer_reg[1].fld = SHURK0_SELPH_DQ0_TXDLY_DQ0;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;

	case 1:
		/* DQM1 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ3;
		transfer_reg[0].fld = SHURK0_SELPH_DQ3_DLY_DQM1;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ1;
		transfer_reg[1].fld = SHURK0_SELPH_DQ1_TXDLY_DQM1;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		/* DQ1 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ2;
		transfer_reg[0].fld = SHURK0_SELPH_DQ2_DLY_DQ1;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ0;
		transfer_reg[1].fld = SHURK0_SELPH_DQ0_TXDLY_DQ1;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;

	case 2:
		/* DQM2 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ3;
		transfer_reg[0].fld = SHURK0_SELPH_DQ3_DLY_DQM2;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ1;
		transfer_reg[1].fld = SHURK0_SELPH_DQ1_TXDLY_DQM2;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		/* DQ2 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ2;
		transfer_reg[0].fld = SHURK0_SELPH_DQ2_DLY_DQ2;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ0;
		transfer_reg[1].fld = SHURK0_SELPH_DQ0_TXDLY_DQ2;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;

	case 3:
		/* DQM3 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ3;
		transfer_reg[0].fld = SHURK0_SELPH_DQ3_DLY_DQM3;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ1;
		transfer_reg[1].fld = SHURK0_SELPH_DQ1_TXDLY_DQM3;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		/* DQ3 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ2;
		transfer_reg[0].fld = SHURK0_SELPH_DQ2_DLY_DQ3;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ0;
		transfer_reg[1].fld = SHURK0_SELPH_DQ0_TXDLY_DQ3;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;
	}
}

void move_dramc_tx_dq_oen(DRAMC_CTX_T *p, unsigned char byte_idx,
	signed char shift_ui)
{
	REG_TRANSFER_T transfer_reg[2];

	switch (byte_idx) {
	case 0:
		/*  DQM_OEN_0 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ3;
		transfer_reg[0].fld = SHURK0_SELPH_DQ3_DLY_OEN_DQM0;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ1;
		transfer_reg[1].fld = SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		/*  DQ_OEN_0 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ2;
		transfer_reg[0].fld = SHURK0_SELPH_DQ2_DLY_OEN_DQ0;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ0;
		transfer_reg[1].fld = SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;

	case 1:
		/*  DQM_OEN_1 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ3;
		transfer_reg[0].fld = SHURK0_SELPH_DQ3_DLY_OEN_DQM1;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ1;
		transfer_reg[1].fld = SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		/*  DQ_OEN_1 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ2;
		transfer_reg[0].fld = SHURK0_SELPH_DQ2_DLY_OEN_DQ1;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ0;
		transfer_reg[1].fld = SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;

	case 2:
		/*  DQM_OEN_2 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ3;
		transfer_reg[0].fld = SHURK0_SELPH_DQ3_DLY_OEN_DQM2;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ1;
		transfer_reg[1].fld = SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		/*  DQ_OEN_2 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ2;
		transfer_reg[0].fld = SHURK0_SELPH_DQ2_DLY_OEN_DQ2;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ0;
		transfer_reg[1].fld = SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;

	case 3:
		/*  DQM_OEN_3 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ3;
		transfer_reg[0].fld = SHURK0_SELPH_DQ3_DLY_OEN_DQM3;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ1;
		transfer_reg[1].fld = SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		/*  DQ_OEN_3 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ2;
		transfer_reg[0].fld = SHURK0_SELPH_DQ2_DLY_OEN_DQ3;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ0;
		transfer_reg[1].fld = SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
		break;
	}
}

void write_leveling_move_dqs_instead_of_clk(DRAMC_CTX_T *p)
{
	unsigned char byte_idx;
	unsigned char backup_rank, ii;

	backup_rank = get_rank(p);

	for (byte_idx = 0; byte_idx < (p->data_width / DQS_BIT_NUM);
		byte_idx++) {
		move_dramc_tx_dqs(p, byte_idx, -WRITE_LEVELING_MOVD_DQS);
		move_dramc_tx_dqs_oen(p, byte_idx, -WRITE_LEVELING_MOVD_DQS);

		for (ii = RANK_0; ii < p->support_rank_num; ii++) {
			set_rank(p, ii);
			move_dramc_tx_dq(p, byte_idx,
				-WRITE_LEVELING_MOVD_DQS);
			move_dramc_tx_dq_oen(p, byte_idx,
				-WRITE_LEVELING_MOVD_DQS);
		}
		set_rank(p, backup_rank);
	}
}

void set_dram_mr_write_leveling_on_off(DRAMC_CTX_T *p, unsigned char on_off)
{
	unsigned int mr_val;
	unsigned int mr_idx;

	if (is_lp4_family(p) || p->dram_type == TYPE_LPDDR3) {
		mr_idx = 0x2;
		mr_val = dram_mr.mr02_value[p->dram_fsp];
	} else if (p->dram_type == TYPE_PCDDR4 ||
		p->dram_type == TYPE_PCDDR3) {
		mr_idx = 0x1;
		mr_val = dram_mr.mr01_value[p->dram_fsp];
	}

	if (on_off)
		mr_val |= (1 << 7);
	else
		mr_val &= ~(1 << 7);

	dramc_mode_reg_write_by_rank(p, p->rank, mr_idx, mr_val);
}

#define WL_CNT_16	0xfc
#define WL_CNT		0xf0

static void dramc_wl_lp4_init(DRAMC_CTX_T *p)
{
	unsigned int value;
	unsigned char wl_dqs_sel;

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG),
		p->rank, RKCFG_TXRANK);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG),
		SET_FLD, RKCFG_TXRANKFIX);

	auto_refresh_switch(p, DISABLE);

	if (p->arfg_write_leveling_init_shif[p->channel][p->rank] == FALSE) {
		write_leveling_move_dqs_instead_of_clk(p);
		p->arfg_write_leveling_init_shif[p->channel][RANK_0] = TRUE;
		p->arfg_write_leveling_init_shif[p->channel][RANK_1] = TRUE;
		p->fg_tx_perbif_init[p->channel][RANK_0] = FALSE;
		p->fg_tx_perbif_init[p->channel][RANK_1] = FALSE;

		show_msg2((INFO, "WriteLevelingMoveDQSInsteadOfCLK\n"));
	}
#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_WRITELEVELING)
	if(p->femmc_Ready==1)
	{
		wl_final_delay[p->channel][0] =p->pSavetimeData->u1WriteLeveling_bypass_Save[p->channel][p->rank][0];
		wl_final_delay[p->channel][1] =p->pSavetimeData->u1WriteLeveling_bypass_Save[p->channel][p->rank][1];
		g_need_save_wl = 0;
		return;
	}
#endif

	/* write leveling mode initialization */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL),
		SET_FLD, DRAMC_PD_CTRL_MIOCKCTRLOFF);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL),
		CLEAR_FLD, DRAMC_PD_CTRL_PHYCLKDYNGEN);

	/*
	* Make CKE fixed at 1
	* (Don't enter power down, Put this before issuing MRS)
	*/
	cke_fix_on_off(p, p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	/* PHY RX Setting for Write Leveling */
	o1_path_on_off(p, ENABLE);

	/* Disable WODT in case it's asserted while issuing MRW */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_WODT),
		SET_FLD, SHU_WODT_WODTFIXOFF);

	/* issue MR2[7] to enable write leveling */
	set_dram_mr_write_leveling_on_off(p, ENABLE);

	/* wait tWLDQSEN (25 nCK / 25ns) after enabling write leveling mode */
	delay_us(1);

	/* Enable Write leveling */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV), 0xa,
		WRITE_LEV_DQSBX_G);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV), SET_FLD,
		WRITE_LEV_WRITE_LEVEL_EN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV), SET_FLD,
		WRITE_LEV_CBTMASKDQSOE);

	/* select DQS */
	if (p->data_width == DATA_WIDTH_32BIT) {
		value = 0xf;
	} else {
		value = 0x3;
	}

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV), value,
		WRITE_LEV_DQS_SEL);

	/* wait tWLMRD (40 nCL / 40 ns) before DQS pulse */
	delay_us(1);

	/* Set DQS output delay to 0 */
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ7),
		CLEAR_FLD, SHU_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ7),
		CLEAR_FLD, SHU_R0_B2_DQ7_RK0_ARPI_PBYTE_B2);
}


static void adjust_clk_dqs_output_delay(DRAMC_CTX_T *p, signed int ii)
{
	if (p->data_width == DATA_WIDTH_16BIT) {
		if (ii <= 0) {
			/* Adjust Clk output delay. */
			io_32_write_fld_align(DRAMC_REG_ADDR
				(DDRPHY_SHU_R0_CA_CMD9), -ii,
				SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK);
		} else {
			/* Adjust DQS output delay. */
			/* PI (TX DQ/DQS adjust at the same time) */
			io_32_write_fld_align(DRAMC_REG_ADDR
				(DDRPHY_SHU_R0_B0_DQ7), ii,
				SHU_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);
			io_32_write_fld_align(DRAMC_REG_ADDR
				(DDRPHY_SHU_R0_B2_DQ7), ii,
				SHU_R0_B2_DQ7_RK0_ARPI_PBYTE_B2);
		}
	} else {
		if (ii <= 0) {
			/* Adjust Clk output delay. */
			io_32_write_fld_align(DRAMC_REG_ADDR
				(DDRPHY_SHU_R0_CA_CMD9), -ii,
				SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK);
		} else {
			/* Adjust DQS output delay. */
			/* PI (TX DQ/DQS adjust at the same time) */
			io_32_write_fld_align_all(DRAMC_REG_ADDR
				(DDRPHY_SHU_R0_B0_DQ7), ii,
				SHU_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);
			io_32_write_fld_align_all(DRAMC_REG_ADDR
				(DDRPHY_SHU_R0_B2_DQ7), ii,
				SHU_R0_B2_DQ7_RK0_ARPI_PBYTE_B2);
		}
	}
}

static unsigned char wl_check_lp4(DRAMC_CTX_T *p, signed int begin,
	signed int end, unsigned char step, unsigned char sample_count)
{
	signed int ii;
	unsigned char i;
	unsigned int dq_o1 = 0;
	unsigned char sample_status[DQS_NUMBER], dq_o1_perbyte[DQS_NUMBER],
		dq_o1_index[DQS_NUMBER];

	/* DQ mapping */
	for (i = 0; i < (p->data_width / DQS_BIT_NUM); i++)
		dq_o1_index[i] = i * 8;

	/* Initialize sw parameters */
	for (ii = 0; ii < (signed int) (p->data_width / DQS_BIT_NUM); ii++) {
		sample_status[ii] = 0;
		wl_final_delay[p->channel][ii] = 0;
	}

	for (ii = begin; ii < end; ii += step) {
		adjust_clk_dqs_output_delay(p, ii);

		/* Trigger DQS pulse */
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV),
			SET_FLD, WRITE_LEV_DQS_WLEV);
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV),
			CLEAR_FLD, WRITE_LEV_DQS_WLEV);

		/* wait tWLO (7.5ns / 20ns) before output (DDR3 / LPDDR3) */
		delay_us(1);

		/* Read DQ_O1 from register */
		dq_o1 = io32_read_4b(DRAMC_REG_ADDR(DDRPHY_MISC_DQO1));

		/* Since the O1 result is located in DDRPHY side,
		 * if Swap between bytes, shall make the bits that driven
		 * by the same DQS appear in 1byte.
		 * PINMUX. REVIEW
		 */
		if (p->en_4bit_mux) {
			unsigned char * lpddr_4bitmux_byte_mapping;
			unsigned char bit_idx;
			unsigned int dq_o1_tmp = 0;

			show_msg2((INFO,
				"WL 4bit pinmux. Read dq_o1 = 0x%x\n", dq_o1));

			lpddr_4bitmux_byte_mapping =
				dramc_get_4bitmux_byte_mapping(p);
			if (lpddr_4bitmux_byte_mapping != NULL) {
				for (bit_idx = 0; bit_idx < p->data_width; bit_idx++) {
					dq_o1_tmp |= ((dq_o1 >> bit_idx) & 0x1) <<
						lpddr_4bitmux_byte_mapping[bit_idx];
				}

				dq_o1 = dq_o1_tmp;
			}
			show_msg2((INFO,
				"WL 4bit pinmux. Processed dq_o1 = 0x%x\n", dq_o1));
		}
		show_msg2((INFO, "%d    ", ii));

		for (i = 0; i < (p->data_width / DQS_BIT_NUM); i++) {
			dq_o1_perbyte[i] = (unsigned char)
				((dq_o1 >> dq_o1_index[i]) & 0xff);

			show_msg2((INFO, "%x   ", dq_o1_perbyte[i]));

#if SUPPORT_TYPE_LPDDR4
			if ((sample_status[i] == 0) && (dq_o1_perbyte[i] == 0))
				sample_status[i] = 1;
			else if ((sample_status[i] >= 1) &&
				(dq_o1_perbyte[i] == 0))
				sample_status[i] = 1;
			else if ((sample_status[i] >= 1) &&
				(dq_o1_perbyte[i] == 0xFF))
				sample_status[i]++;
                        else if ((sample_status[i] >= 1) &&
                                (dq_o1_perbyte[i] != 0))
                                show_msg0((INFO, "Warning! WL dq_o1_perbyte[%d] = 0x%x\n", i, dq_o1_perbyte[i]));
#else
			if ((sample_status[i] == 0) && (dq_o1_perbyte[i] == 0))
				sample_status[i] = 1;
			else if ((sample_status[i] >= 1) &&
				(dq_o1_perbyte[i] == 0))
				sample_status[i] = 1;
			else if ((sample_status[i] >= 1) &&
				(dq_o1_perbyte[i] != 0))
				sample_status[i]++;
#endif
			/* result not found of byte yet */
			if (((sample_count & (0x01 << i)) == 0)
				&& ((sample_status[i] == 8) ||
				((ii == end - 1) && (sample_status[i] > 1)))) {
				wl_final_delay[p->channel][i] =
					ii - (sample_status[i] - 2)*step;
				sample_count |= (0x01 << i);
			}
		}

		show_msg2((INFO, "\n"));

#if !DQS_DUTY_MEASURE_WITH_WRITE_LEVELING
		if (sample_count == BYTE_MAX)
			break;	/*  all byte found, early break. */
#endif
	}
	show_msg2((INFO, "pass bytecount = 0x%x (0xff: all bytes pass)\n\n",
		sample_count));
	return sample_count;
}

static void wl_adjust_clk_ca_lp4(DRAMC_CTX_T *p)
{
	signed int clock_delay_max;
	unsigned int value;
	signed int diff;
	unsigned char i;

	/* Initialize sw parameters */
	clock_delay_max = MAX_TX_DQSDLY_TAPS;

	for (i = 0; i < (p->data_width / DQS_BIT_NUM); i++) {
		if (clock_delay_max >
			wl_final_delay[p->channel][i]) {
			clock_delay_max = wl_final_delay[p->channel][i];
		}
	}

	if (clock_delay_max > 0)
		clock_delay_max = 0;
	else
		clock_delay_max = -clock_delay_max;

	print_calibration_basic_info(p);

	show_msg((INFO, "WL Clk dly = %d, CA clk dly = %d\n",
		clock_delay_max, ca_train_clk_delay[p->channel][p->rank]));

	/* Adjust Clk & CA if needed */
	/* cc notes: since SW will only adjust DQS delay for WL,
	 * clock_delay_max will always be 0. So even for non-lp4 type,
	 * the following code will execute 'else' branch. Seems OK.
	 */
	if (ca_train_clk_delay[p->channel][p->rank] < clock_delay_max) {
		diff = clock_delay_max -
			ca_train_clk_delay[p->channel][p->rank];
		show_msg((INFO, "CA adjust %d taps\n", diff));

		/* Write shift value into CA output delay. */
		value = ca_train_cmd_delay[p->channel][p->rank];
		value += diff;
		io_32_write_fld_align(DRAMC_REG_ADDR
			(DDRPHY_SHU_R0_CA_CMD9), value,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD);

		show_msg((INFO,
			"Update CA PI Dly Macro0 = %dn", value));

		/* Write shift value into CS output delay. */
		value = ca_train_cs_delay[p->channel][p->rank];
		value += diff;
		io_32_write_fld_align(DRAMC_REG_ADDR
			(DDRPHY_SHU_R0_CA_CMD9), value,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CS);
		show_msg((INFO, "Update CS Dly = %d\n", value));
	} else {
		show_msg((INFO, "No need to update CA/CS dly %s",
			"(CLK dly smaller than CA training)\n"));
		clock_delay_max = ca_train_clk_delay[p->channel][p->rank];
	}

	/* Write max center value into Clk output delay. */
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD9),
		clock_delay_max, SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK);

	show_msg((INFO, "Final Clk output dly = %d\n", clock_delay_max));

	for (i = 0; i < (p->data_width / DQS_BIT_NUM); i++) {
		wl_final_delay[p->channel][i] += clock_delay_max;
		show_msg((INFO, "[Write Leveling]DQS%d dly: %d\n", i,
			wl_final_delay[p->channel][i]));
#if (CONFIG_FOR_HQA_TEST_USED == 1)
		hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT1,
			"WriteLeveling_DQS", i, wl_final_delay[p->channel][i], NULL);
#endif
	}
}

static void wl_set_values_lp4(DRAMC_CTX_T *p)
{
	signed int wrlevel_dq_delay[DQS_NUMBER] = {0, 0, 0, 0};
	unsigned char dramc_byte[DQS_NUMBER];
	unsigned char i;

	for (i = 0; i < (p->data_width / DQS_BIT_NUM); i++) {
		dramc_byte[i] = mapping_phy_byte_to_dramc(p, i);
	}

	/* set to best values for  DQS */
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ7),
		wl_final_delay[p->channel][dramc_byte[0]],
		SHU_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ7),
		wl_final_delay[p->channel][dramc_byte[1]],
		SHU_R0_B2_DQ7_RK0_ARPI_PBYTE_B2);

	if (p->data_width == DATA_WIDTH_32BIT) {
		io_32_write_fld_align(DDRPHY_SHU_R0_B0_DQ7 +
			(CHANNEL_B << POS_BANK_NUM),
			wl_final_delay[p->channel][dramc_byte[2]],
			SHU_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);
		io_32_write_fld_align(DDRPHY_SHU_R0_B2_DQ7 +
			(CHANNEL_B << POS_BANK_NUM),
			wl_final_delay[p->channel][dramc_byte[3]],
			SHU_R0_B2_DQ7_RK0_ARPI_PBYTE_B2);
	}

	for (i = 0; i < (p->data_width / DQS_BIT_NUM); i++) {
		wrlevel_dq_delay[i] = wl_final_delay[p->channel][i] + 0x10;
		/* ARPI_DQ_B* is 6 bits, max 0x40 */
		if (wrlevel_dq_delay[i] >= 0x40) {

			wrlevel_dq_delay[i] -= 0x40;
			move_dramc_tx_dq(p, i, 2);
			move_dramc_tx_dq_oen(p, i, 2);
		}
	}

	/* set to best values for  DQM, DQ */
	io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ7),
		p_fld(wrlevel_dq_delay[dramc_byte[0]],
		SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
		p_fld(wrlevel_dq_delay[dramc_byte[0]],
		SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));

	io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ7),
		p_fld(wrlevel_dq_delay[dramc_byte[1]],
		SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2) |
		p_fld(wrlevel_dq_delay[dramc_byte[1]],
		SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2));

	if (p->data_width == DATA_WIDTH_32BIT) {
		io_32_write_fld_multi(DDRPHY_SHU_R0_B0_DQ7 +
			(CHANNEL_B << POS_BANK_NUM),
			p_fld(wrlevel_dq_delay[dramc_byte[2]],
			SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
			p_fld(wrlevel_dq_delay[dramc_byte[2]],
			SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));

		io_32_write_fld_multi(DDRPHY_SHU_R0_B2_DQ7 +
			(CHANNEL_B << POS_BANK_NUM),
			p_fld(wrlevel_dq_delay[dramc_byte[3]],
			SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2) |
			p_fld(wrlevel_dq_delay[dramc_byte[3]],
			SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2));
	}
}

DRAM_STATUS_T dramc_write_leveling(DRAMC_CTX_T *p)
{
	/* Note that below procedure is based on "ODT off" */
	DRAM_STATUS_T result = DRAM_FAIL;
	unsigned char sample_count;
	signed int begin, end;
	unsigned char step;

	show_msg_with_timestamp((INFO, "start Write Leveling\n"));

	/* error handling */
	if (!p) {
		show_err("context NULL\n");
		return DRAM_FAIL;
	}

	unsigned int reg_backup_address[] = {
		(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0)),
		(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL)),
		(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL)),
		(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV)),
		(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL)),
		(DRAMC_REG_ADDR(DRAMC_REG_SHU_WODT)),
		(DRAMC_REG_ADDR(DRAMC_REG_RKCFG)),
	};

	wrlevel_done[p->channel] = 0;

	/*  backup mode settings */
	dramc_backup_registers(p, reg_backup_address,
		sizeof(reg_backup_address) / sizeof(unsigned int));

	/* Proceed write leveling... */
	dramc_wl_lp4_init(p);
	if (g_need_save_wl == 0 ){
		sample_count = 0xff;
		set_calibration_result(p, DRAM_CALIBRATION_WRITE_LEVEL, DRAM_OK);
	} else {
		/*
		* used for WL done status
		* each bit of sample_cnt represents one-byte WL status
		* 1: done or N/A. 0: NOK
		*/
		if (p->data_width == DATA_WIDTH_16BIT)
			sample_count = WL_CNT_16;
		else
			sample_count = WL_CNT;

		show_msg3((INFO, "\n[Write Leveling]\n"));
		show_msg2((INFO, "delay  byte0  byte1\n"));

		begin = WRITE_LEVELING_MOVD_DQS * 32 - MAX_CLK_PI_DELAY - 1;
		begin = begin + WL_OFFSET;
		end = begin + WL_RANGE;
#if (FOR_DV_SIMULATION_USED == 1)
		step = 2;
#else
		step = WL_STEP;
#endif

		sample_count = wl_check_lp4(p, begin, end, step, sample_count);
	}

	if (sample_count == BYTE_MAX) { /* all bytes are done */
		wrlevel_done[p->channel] = 1;
		result = DRAM_OK;
	} else {
		result = DRAM_FAIL;
	}
	set_calibration_result(p, DRAM_CALIBRATION_WRITE_LEVEL, result);

	wl_adjust_clk_ca_lp4(p);

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_WRITELEVELING)
	if(p->femmc_Ready==0)
	{
		p->pSavetimeData->u1WriteLeveling_bypass_Save[p->channel][p->rank][0]=wl_final_delay[p->channel][0];
		p->pSavetimeData->u1WriteLeveling_bypass_Save[p->channel][p->rank][1]=wl_final_delay[p->channel][1];
	}
#endif

	/*
	* write leveling done, mode settings recovery if necessary
	* recover mode registers : issue MR2[7] to disable write leveling
	*/
	set_dram_mr_write_leveling_on_off(p, DISABLE);

	/*  restore registers. */
	dramc_restore_registers(p, reg_backup_address,
		sizeof(reg_backup_address) / sizeof(unsigned int));

	/* Disable DQ_O1, SELO1ASO=0 for power saving */
	o1_path_on_off(p, 0);

	wl_set_values_lp4(p);

	dramc_rank_swap(p, RANK_0);

	show_msg3((INFO, "[DramcWriteLeveling] Done\n\n"));

	return result;
}
#endif /* SIMULATION_WRITE_LEVELING */

#if SIMULATION_TX_PERBIT
/* dramc_tx_window_perbit_cal (v2)
 *  TX DQS per bit SW calibration.
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @param  apply           (unsigned char): 0 don't apply the register we set
 *			1 apply the register we set ,default don't apply.
 *  @retval status          (DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
#define ENABLE_64_PI_TO_UI 1
#if ENABLE_64_PI_TO_UI /* 1 PI = tCK/64, total 128 PI, 1UI = 64 PI */
#define TX_DQ_UI_TO_PI_TAP         64
#else /* 1 PI = tCK/64, total 128 PI, 1UI = 32 PI */
#define TX_DQ_UI_TO_PI_TAP         32
#endif

static void tx_win_transfer_delay_to_uipi(DRAMC_CTX_T *p, unsigned short delay,
	unsigned char adjust_pi_to_center, unsigned char *ui_large_dq,
	unsigned char *ui_small_dq, unsigned char *u_pi,
	unsigned char *pu1_u1_large_dqoe, unsigned char *pu1_u1_small_dqoe)
{
	unsigned char small_ui_to_large, pi;
	unsigned short tmp_value;

	/* in LP4, 8 small UI =  1 large UI */
	if (get_div_mode(p) == DIV4_MODE)
		small_ui_to_large = 2;
	else
		small_ui_to_large = 3;

	if (u_pi != NULL) {
		pi = delay & (TX_DQ_UI_TO_PI_TAP - 1);
		*u_pi = pi;
	}

	tmp_value = (delay / TX_DQ_UI_TO_PI_TAP) << 1;

	if (adjust_pi_to_center && (u_pi != NULL)) {
		if (pi < 10) {
			pi += (TX_DQ_UI_TO_PI_TAP) >> 1;
			tmp_value--;
		} else if (pi > TX_DQ_UI_TO_PI_TAP - 10) {
			pi -= (TX_DQ_UI_TO_PI_TAP) >> 1;
			tmp_value++;
		}

		*u_pi = pi;
	}

	*ui_small_dq = tmp_value -
		((tmp_value >> small_ui_to_large) << small_ui_to_large);
	*ui_large_dq = (tmp_value >> small_ui_to_large);

	/* calculate DQ OE according to DQ UI */
	tmp_value -= TX_DQ_OE_SHIFT_LP4;

	*pu1_u1_small_dqoe = tmp_value -
		((tmp_value >> small_ui_to_large) << small_ui_to_large);
	*pu1_u1_large_dqoe = (tmp_value >> small_ui_to_large);
}

void dramc_tx_set_vref(DRAMC_CTX_T *p, unsigned char vref_range,
	unsigned char vref_value)
{
	unsigned char temp_op_value = ((vref_value & 0x3f) | (vref_range << 6));

	dram_mr.mr14_value[p->channel][p->rank][p->dram_fsp] = temp_op_value;
	dramc_mode_reg_write_by_rank(p, p->rank, MR14, temp_op_value);
}

static unsigned short find_smallest_dq_byte_delay(DRAMC_CTX_T *p)
{
	unsigned char byte_idx;
	unsigned int reg_value_txdly, reg_value_dly;
	unsigned short temp_virtual_delay, smallest_virtual_delay = WORD_MAX;
	unsigned char dq_ui_large_bak[DQS_NUMBER], dq_ui_small_bak[DQS_NUMBER];
	unsigned char mck2ui;

	if (get_div_mode(p) == DIV8_MODE) {
		mck2ui = 0x3;
	} else {
		mck2ui = 0x2;
	}

	reg_value_txdly =
		io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ0));
	reg_value_dly =
		io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ2));

	/*  find smallest DQ byte delay */
	for (byte_idx = 0; byte_idx < (p->data_width / DQS_BIT_NUM);
	     byte_idx++) {
		dq_ui_large_bak[byte_idx] =
			(reg_value_txdly >> (byte_idx * 4)) & 0x7;
		dq_ui_small_bak[byte_idx] =
			(reg_value_dly >> (byte_idx * 4)) & 0x7;

		temp_virtual_delay = (dq_ui_large_bak[byte_idx] << mck2ui)
			+ dq_ui_small_bak[byte_idx];
		if (temp_virtual_delay < smallest_virtual_delay)
			smallest_virtual_delay = temp_virtual_delay;

		show_msg((INFO, "Original DQ_B%d (%d %d) =%d, OEN = %d\n",
			byte_idx, dq_ui_large_bak[byte_idx],
			dq_ui_small_bak[byte_idx],
			temp_virtual_delay,
			temp_virtual_delay - TX_DQ_OE_SHIFT_LP4));
	}
	     return smallest_virtual_delay;
}

static void set_dq_oen_rg(DRAMC_CTX_T *p, unsigned char dq_oen_large[],
	unsigned char dq_oen_small[])
{
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ0),
		p_fld(dq_oen_large[0], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) |
		p_fld(dq_oen_large[1], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) |
		p_fld(dq_oen_large[2], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) |
		p_fld(dq_oen_large[3], SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3));

	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ1),
		p_fld(dq_oen_large[0], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) |
		p_fld(dq_oen_large[1], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) |
		p_fld(dq_oen_large[2], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) |
		p_fld(dq_oen_large[3], SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3));

	/* DLY_DQ[2:0] */
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ2),
		p_fld(dq_oen_small[0], SHURK0_SELPH_DQ2_DLY_OEN_DQ0) |
		p_fld(dq_oen_small[1], SHURK0_SELPH_DQ2_DLY_OEN_DQ1) |
		p_fld(dq_oen_small[2], SHURK0_SELPH_DQ2_DLY_OEN_DQ2) |
		p_fld(dq_oen_small[3], SHURK0_SELPH_DQ2_DLY_OEN_DQ3));

	/* DLY_DQM[2:0] */
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ3),
		p_fld(dq_oen_small[0], SHURK0_SELPH_DQ3_DLY_OEN_DQM0) |
		p_fld(dq_oen_small[1], SHURK0_SELPH_DQ3_DLY_OEN_DQM1) |
		p_fld(dq_oen_small[2], SHURK0_SELPH_DQ3_DLY_OEN_DQM2) |
		p_fld(dq_oen_small[3], SHURK0_SELPH_DQ3_DLY_OEN_DQM3));
}

void dramc_tx_oe_calibration(DRAMC_CTX_T *p)
{
	unsigned char byte_idx, begin[DQS_NUMBER] = { 0 },
		end[DQS_NUMBER] = {BYTE_MAX, BYTE_MAX, BYTE_MAX, BYTE_MAX},
		best_step[DQS_NUMBER];
	unsigned int err_value;
	unsigned short delay, smallest_virtual_delay = WORD_MAX;
	unsigned short dqoen_delay_begin, dqoen_delay_end;
	unsigned char dq_oen_ui_large[DQS_NUMBER], dq_oen_ui_small[DQS_NUMBER];
	unsigned char mck2ui, msk;
	unsigned char step;

	show_msg_with_timestamp((INFO, "\n[DramC_TX_OE_Calibration] TA2\n"));

#if FOR_DV_SIMULATION_USED
	step = 4;
#else
	step = 1;
#endif

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION)
	if(p->femmc_Ready==1)
	{
		for(byte_idx = 0; byte_idx < (p->data_width / DQS_BIT_NUM); byte_idx++)
		{
			dq_oen_ui_large[byte_idx]= p->pSavetimeData->u1TX_OE_DQ_MCK[p->channel][p->rank][byte_idx];
			dq_oen_ui_small[byte_idx]= p->pSavetimeData->u1TX_OE_DQ_UI[p->channel][p->rank][byte_idx];
			show_msg((INFO, "Final TX OE(2T, 0.5T) = (%d, %d)\n",
				dq_oen_ui_large[byte_idx],
				dq_oen_ui_small[byte_idx]));
		}
	}
	else
#endif
	{
		smallest_virtual_delay = find_smallest_dq_byte_delay(p);

		dramc_engine2_init(p, p->test2_1, p->test2_2, TEST_AUDIO_PATTERN, 0);

		if (smallest_virtual_delay >= 7)
			dqoen_delay_begin = smallest_virtual_delay - 7;
		else
			dqoen_delay_begin = 0;

		dqoen_delay_end = dqoen_delay_begin + 10;

		if (get_div_mode(p) == DIV4_MODE) {
			mck2ui = 2;
			msk = 3;
		} else {
			mck2ui = 3;
			msk = 7;
		}

		for (delay = dqoen_delay_begin; delay <= dqoen_delay_end; delay++) {
			for (byte_idx = 0; byte_idx < (p->data_width / DQS_BIT_NUM);
				byte_idx++) {


				dq_oen_ui_large[byte_idx] = (delay >> mck2ui);
				dq_oen_ui_small[byte_idx] = delay & msk;
			}

			set_dq_oen_rg(p, dq_oen_ui_large, dq_oen_ui_small);

			err_value = dramc_engine2_run(p, TE_OP_WRITE_READ_CHECK,
				TEST_AUDIO_PATTERN);

			/* 3 */
			for (byte_idx = 0; byte_idx < (p->data_width / DQS_BIT_NUM);
				byte_idx++) {
				if (((err_value >> (byte_idx << 3)) & 0xff) == 0) {
					if (begin[byte_idx] == 0)
						begin[byte_idx] = 1;
					end[byte_idx] = delay;
				}
			}

			show_msg2((INFO,
				"TAP=%d, err_value=0x%x, End_B0=%d End_B1=%d\n",
				delay, err_value, end[0], end[1]));

			if (((err_value & 0xffff) != 0) && (begin[0] == 1) &&
				(begin[1] == 1))
				break; /* early break; */
		}

		dramc_engine2_end(p);

		/* 4 */
		for (byte_idx = 0; byte_idx < (p->data_width / DQS_BIT_NUM);
			byte_idx++) {
			if (end[byte_idx] == BYTE_MAX) {
				best_step[byte_idx] =
					smallest_virtual_delay - TX_DQ_OE_SHIFT_LP4;
				show_err2("Byte %d no TX OE taps pass", byte_idx);
				show_err(", calibration fail!\n");
			} else { /* window is larger htan 3 */
				best_step[byte_idx] = end[byte_idx] - 3;
			}
			show_msg((INFO, "Byte%d end_step=%d  best_step=%d ",
				byte_idx, end[byte_idx],
				best_step[byte_idx]));

			dq_oen_ui_large[byte_idx] = (best_step[byte_idx] >> mck2ui);
			dq_oen_ui_small[byte_idx] = best_step[byte_idx] & msk;
			show_msg((INFO, "Final TX OE(2T, 0.5T) = (%d, %d)\n",
				dq_oen_ui_large[byte_idx],
				dq_oen_ui_small[byte_idx]));
		}
		show_msg3((INFO, "\n"));
	}
#if 0
	if (p->channel == CHANNEL_B) {
		unsigned char dqs_i;

		for (dqs_i = 2; dqs_i < DQS_NUMBER; dqs_i++) {
			unsigned char dqs_map;

			dqs_map = (dqs_i == 2) ? 1 : 0;

			dq_oen_ui_large[dqs_i] =
				dq_oen_ui_large[dqs_map];
			dq_oen_ui_small[dqs_i] =
				dq_oen_ui_small[dqs_map];
		}
	}
#endif

	set_dq_oen_rg(p, dq_oen_ui_large, dq_oen_ui_small);
#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION)
	if(p->femmc_Ready==0)
	{
		for(byte_idx = 0; byte_idx < (p->data_width / DQS_BIT_NUM);
			byte_idx++)
		{
			p->pSavetimeData->u1TX_OE_DQ_MCK[p->channel][p->rank][byte_idx] = dq_oen_ui_large[byte_idx];
			p->pSavetimeData->u1TX_OE_DQ_UI[p->channel][p->rank][byte_idx] = dq_oen_ui_small[byte_idx];
		}
	}
#endif
}

static unsigned short find_smallest_dqs_delay(DRAMC_CTX_T *p)
{
	unsigned int reg_value_txdly, reg_value_dly;
	unsigned char byte_idx;
	unsigned char dq_ui_large_bak[DQS_NUMBER], dq_ui_small_bak[DQS_NUMBER];
	unsigned short temp_virtual_delay, smallest_virtual_delay = WORD_MAX;
	unsigned char mck2ui;

	reg_value_txdly =
		io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS0));
	reg_value_dly =
		io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS1));

	if (get_div_mode(p) == DIV4_MODE)
		mck2ui = VALUE_2;
	else
		mck2ui = VALUE_3;

	/*  find smallest DQS delay */
	for (byte_idx = 0; byte_idx < (p->data_width / DQS_BIT_NUM);
	     byte_idx++) {
		dq_ui_large_bak[byte_idx] =
			(reg_value_txdly >> (byte_idx << 2)) & 0x7;
		dq_ui_small_bak[byte_idx] =
			(reg_value_dly >> (byte_idx << 2)) & 0x7;

		/* LP4 : Virtual Delay = 256 * MCK + 32*UI + PI; */
		temp_virtual_delay =
		    (((dq_ui_large_bak[byte_idx] << mck2ui) +
		      dq_ui_small_bak[byte_idx]) << 5) +
		    wl_final_delay[p->channel][byte_idx];

		if (temp_virtual_delay < smallest_virtual_delay)
			smallest_virtual_delay = temp_virtual_delay;
	}
	return smallest_virtual_delay;
}

static void set_txdq_vref(DRAMC_CTX_T *p, unsigned short vref_range,
	unsigned short vref_level)
{
	if (is_lp4_family(p)) {
		/*  SET tx Vref (DQ) here */
		dram_mr.mr14_value[p->channel][p->rank][p->dram_fsp] =
			(vref_level | (vref_range << 6));
		dramc_mode_reg_write_by_rank(p, p->rank, MR14,
			vref_level | (vref_range << 6));
	} else if (p->dram_type == TYPE_PCDDR4) {
		dramc_mode_reg_write_by_rank(p, p->rank, MR06,
			dram_mr.mr06_value[p->dram_fsp] | 0x80);
		delay_us(100);

		dramc_mode_reg_write_by_rank(p, p->rank, MR06,
			dram_mr.mr06_value[p->dram_fsp] | 0x80 | vref_level | (vref_range << 6));
		delay_us(100);

		dramc_mode_reg_write_by_rank(p, p->rank, MR06,
			dram_mr.mr06_value[p->dram_fsp] | vref_level | (vref_range << 6));
		delay_us(100);
	}
}

static void set_txdq_delay_lp4(DRAMC_CTX_T *p, unsigned char delay1,
	unsigned char delay2, unsigned char delay3, unsigned char delay4,
	unsigned char type)
{
	if (type == 0) { /* TXDLY_DQ , TXDLY_OEN_DQ */
		io_32_write_fld_multi(
			DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ0),
			p_fld(delay1, SHURK0_SELPH_DQ0_TXDLY_DQ0)
			| p_fld(delay1, SHURK0_SELPH_DQ0_TXDLY_DQ1)
			| p_fld(delay1, SHURK0_SELPH_DQ0_TXDLY_DQ2)
			| p_fld(delay1, SHURK0_SELPH_DQ0_TXDLY_DQ3)
			| p_fld(delay2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0)
			| p_fld(delay2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1)
			| p_fld(delay2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2)
			| p_fld(delay2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3));

		/* DLY_DQ[2:0] */
		io_32_write_fld_multi(
		DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ2),
			p_fld(delay3, SHURK0_SELPH_DQ2_DLY_DQ0)
			| p_fld(delay3, SHURK0_SELPH_DQ2_DLY_DQ1)
			| p_fld(delay3, SHURK0_SELPH_DQ2_DLY_DQ2)
			| p_fld(delay3, SHURK0_SELPH_DQ2_DLY_DQ3)
			| p_fld(delay4, SHURK0_SELPH_DQ2_DLY_OEN_DQ0)
			| p_fld(delay4, SHURK0_SELPH_DQ2_DLY_OEN_DQ1)
			| p_fld(delay4, SHURK0_SELPH_DQ2_DLY_OEN_DQ2)
			| p_fld(delay4, SHURK0_SELPH_DQ2_DLY_OEN_DQ3));
	} else if (type == 1) { /* TXDLY_DQM , TXDLY_OEN_DQM */
		io_32_write_fld_multi(
			DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ1),
			p_fld(delay1, SHURK0_SELPH_DQ1_TXDLY_DQM0)
			| p_fld(delay1, SHURK0_SELPH_DQ1_TXDLY_DQM1)
			| p_fld(delay1, SHURK0_SELPH_DQ1_TXDLY_DQM2)
			| p_fld(delay1, SHURK0_SELPH_DQ1_TXDLY_DQM3)
			| p_fld(delay2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0)
			| p_fld(delay2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1)
			| p_fld(delay2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2)
			| p_fld(delay2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3));

		/* DLY_DQM[2:0] */
		io_32_write_fld_multi(
			DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ3),
			p_fld(delay3, SHURK0_SELPH_DQ3_DLY_DQM0)
			| p_fld(delay3, SHURK0_SELPH_DQ3_DLY_DQM1)
			| p_fld(delay3, SHURK0_SELPH_DQ3_DLY_DQM2)
			| p_fld(delay3, SHURK0_SELPH_DQ3_DLY_DQM3)
			| p_fld(delay4, SHURK0_SELPH_DQ3_DLY_OEN_DQM0)
			| p_fld(delay4, SHURK0_SELPH_DQ3_DLY_OEN_DQM1)
			| p_fld(delay4, SHURK0_SELPH_DQ3_DLY_OEN_DQM2)
			| p_fld(delay4, SHURK0_SELPH_DQ3_DLY_OEN_DQM3));
	}

}

static unsigned char move_dq_delay_lp4(DRAMC_CTX_T *p, unsigned short delay,
	DRAM_TX_PER_BIT_CALIBRATION_TYTE_T cal_type,
	unsigned char vref_scan_enable, unsigned char dq_ui_small_reg_value)
{
	unsigned char dq_pi, dq_ui_small, dq_ui_large,
		dq_oen_ui_small, dq_oen_ui_large;

	tx_win_transfer_delay_to_uipi(p, delay, 0,
		&dq_ui_large, &dq_ui_small,
		&dq_pi, &dq_oen_ui_large, &dq_oen_ui_small);

	if (cal_type == TX_DQ_DQS_MOVE_DQ_ONLY
	    || cal_type == TX_DQ_DQS_MOVE_DQ_DQM) {
		/* TXDLY_DQ , TXDLY_OEN_DQ */
		if (dq_ui_small_reg_value != dq_ui_small) {
			set_txdq_delay_lp4(p, dq_ui_large,
				dq_oen_ui_large, dq_ui_small,
				dq_oen_ui_small, 0);
		}
	}

	if (cal_type == TX_DQ_DQS_MOVE_DQM_ONLY
	    || cal_type == TX_DQ_DQS_MOVE_DQ_DQM) {
		/* TXDLY_DQM , TXDLY_OEN_DQM */
		if (dq_ui_small_reg_value != dq_ui_small) {
			set_txdq_delay_lp4(p, dq_ui_large,
				dq_oen_ui_large, dq_ui_small,
				dq_oen_ui_small, 1);
		}
	}

	dq_ui_small_reg_value = dq_ui_small;

	/* set to registers, PI DQ (per byte) */

	/*  update TX DQ PI delay, for rank 0 need to take care rank 1 and 2 */
	if (cal_type == TX_DQ_DQS_MOVE_DQ_ONLY
	    || cal_type == TX_DQ_DQS_MOVE_DQ_DQM) {
		io_32_write_fld_align(DRAMC_REG_ADDR
			(DDRPHY_SHU_R0_B0_DQ7),
			dq_pi, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR
			(DDRPHY_SHU_R0_B2_DQ7),
			dq_pi, SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2);

		if (p->data_width == DATA_WIDTH_32BIT) {
			io_32_write_fld_align((DDRPHY_SHU_R0_B0_DQ7 +
				SHIFT_TO_CHB_ADDR),
				dq_pi, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0);
			io_32_write_fld_align((DDRPHY_SHU_R0_B2_DQ7  +
				SHIFT_TO_CHB_ADDR),
				dq_pi, SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2);
		}
	}

	if (cal_type == TX_DQ_DQS_MOVE_DQM_ONLY
	    || cal_type == TX_DQ_DQS_MOVE_DQ_DQM) {
		io_32_write_fld_align(DRAMC_REG_ADDR
			(DDRPHY_SHU_R0_B0_DQ7),
			dq_pi, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR
			(DDRPHY_SHU_R0_B2_DQ7),
			dq_pi, SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2);

		if (p->data_width == DATA_WIDTH_32BIT) {
			io_32_write_fld_align((DDRPHY_SHU_R0_B0_DQ7 +
				SHIFT_TO_CHB_ADDR),
				dq_pi, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0);
			io_32_write_fld_align((DDRPHY_SHU_R0_B2_DQ7  +
				SHIFT_TO_CHB_ADDR),
				dq_pi, SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2);
		}
	}
	if (vref_scan_enable == 0) {
#if !SUPPORT_SAVE_TIME_FOR_CALIBRATION && CALIBRATION_LOG
		show_msg2((INFO,
			"Delay=%d |%d %d %d| ",
			delay, dq_ui_large, dq_ui_small, dq_pi));
#endif
	}
	return dq_ui_small_reg_value;
}

static void txdly_dq_set(DRAMC_CTX_T *p,
	DRAM_TX_PER_BIT_CALIBRATION_TYTE_T cal_type,
	TX_DLY_T *dq_ptr)
{
	/* TXDLY_DQ , TXDLY_OEN_DQ */
	if ((cal_type == TX_DQ_DQS_MOVE_DQ_ONLY)
		|| (cal_type == TX_DQ_DQS_MOVE_DQ_DQM)) {
		io_32_write_fld_multi(DRAMC_REG_ADDR
			(DRAMC_REG_SHURK0_SELPH_DQ0),
			p_fld(dq_ptr->dq_final_ui_large[0],
			SHURK0_SELPH_DQ0_TXDLY_DQ0) |
			p_fld(dq_ptr->dq_final_ui_large[1],
			SHURK0_SELPH_DQ0_TXDLY_DQ1) |
			p_fld(dq_ptr->dq_final_ui_large[2],
			SHURK0_SELPH_DQ0_TXDLY_DQ2) |
			p_fld(dq_ptr->dq_final_ui_large[3],
			SHURK0_SELPH_DQ0_TXDLY_DQ3) |
			p_fld(dq_ptr->dq_final_oen_ui_large[0],
			SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) |
			p_fld(dq_ptr->dq_final_oen_ui_large[1],
			SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) |
			p_fld(dq_ptr->dq_final_oen_ui_large[2],
			SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) |
			p_fld(dq_ptr->dq_final_oen_ui_large[3],
			SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3));

		/*  DLY_DQ[2:0] */
		io_32_write_fld_multi(DRAMC_REG_ADDR
			(DRAMC_REG_SHURK0_SELPH_DQ2),
			p_fld(dq_ptr->dq_final_ui_small[0],
			SHURK0_SELPH_DQ2_DLY_DQ0) |
			p_fld(dq_ptr->dq_final_ui_small[1],
			SHURK0_SELPH_DQ2_DLY_DQ1) |
			p_fld(dq_ptr->dq_final_ui_small[2],
			SHURK0_SELPH_DQ2_DLY_DQ2) |
			p_fld(dq_ptr->dq_final_ui_small[3],
			SHURK0_SELPH_DQ2_DLY_DQ3) |
			p_fld(dq_ptr->dq_final_oen_ui_small[0],
			SHURK0_SELPH_DQ2_DLY_OEN_DQ0) |
			p_fld(dq_ptr->dq_final_oen_ui_small[1],
			SHURK0_SELPH_DQ2_DLY_OEN_DQ1) |
			p_fld(dq_ptr->dq_final_oen_ui_small[2],
			SHURK0_SELPH_DQ2_DLY_OEN_DQ2) |
			p_fld(dq_ptr->dq_final_oen_ui_small[3],
			SHURK0_SELPH_DQ2_DLY_OEN_DQ3));
	}
}

static void txdly_dqm_set(DRAMC_CTX_T *p,
	DRAM_TX_PER_BIT_CALIBRATION_TYTE_T cal_type,
	TX_FINAL_DLY_T *dqm_ptr, unsigned char vref_scan_enable)
{
#if TX_K_DQM_WITH_WDBI
	if ((cal_type == TX_DQ_DQS_MOVE_DQM_ONLY)
		|| (cal_type == TX_DQ_DQS_MOVE_DQ_DQM)
		|| ((cal_type == TX_DQ_DQS_MOVE_DQ_ONLY)
		&& vref_scan_enable))
#endif
	{
		/* TXDLY_DQM , TXDLY_OEN_DQM */
		io_32_write_fld_multi(DRAMC_REG_ADDR
			(DRAMC_REG_SHURK0_SELPH_DQ1),
			p_fld(dqm_ptr->dq_final_dqm_ui_large[0],
			SHURK0_SELPH_DQ1_TXDLY_DQM0) |
			p_fld(dqm_ptr->dq_final_dqm_ui_large[1],
			SHURK0_SELPH_DQ1_TXDLY_DQM1) |
			p_fld(dqm_ptr->dq_final_dqm_ui_large[2],
			SHURK0_SELPH_DQ1_TXDLY_DQM2) |
			p_fld(dqm_ptr->dq_final_dqm_ui_large[3],
			SHURK0_SELPH_DQ1_TXDLY_DQM3) |
			p_fld(dqm_ptr->dq_final_dqm_oen_ui_large[0],
			SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) |
			p_fld(dqm_ptr->dq_final_dqm_oen_ui_large[1],
			SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) |
			p_fld(dqm_ptr->dq_final_dqm_oen_ui_large[2],
			SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) |
			p_fld(dqm_ptr->dq_final_dqm_oen_ui_large[3],
			SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3));

		/*  DLY_DQM[2:0] */
		io_32_write_fld_multi(DRAMC_REG_ADDR
			(DRAMC_REG_SHURK0_SELPH_DQ3),
			p_fld(dqm_ptr->dq_final_dqm_ui_small[0],
			SHURK0_SELPH_DQ3_DLY_DQM0) |
			p_fld(dqm_ptr->dq_final_dqm_ui_small[1],
			SHURK0_SELPH_DQ3_DLY_DQM1) |
			p_fld(dqm_ptr->dq_final_dqm_ui_small[2],
			SHURK0_SELPH_DQ3_DLY_DQM2) |
			p_fld(dqm_ptr->dq_final_dqm_ui_small[3],
			SHURK0_SELPH_DQ3_DLY_DQM3) |
			p_fld(dqm_ptr->dq_final_dqm_oen_ui_small[0],
			SHURK0_SELPH_DQ3_DLY_OEN_DQM0) |
			p_fld(dqm_ptr->dq_final_dqm_oen_ui_small[1],
			SHURK0_SELPH_DQ3_DLY_OEN_DQM1) |
			p_fld(dqm_ptr->dq_final_dqm_oen_ui_small[2],
			SHURK0_SELPH_DQ3_DLY_OEN_DQM2) |
			p_fld(dqm_ptr->dq_final_dqm_oen_ui_small[3],
			SHURK0_SELPH_DQ3_DLY_OEN_DQM3));
	}
}


static void txdly_dqpi_set(DRAMC_CTX_T *p, unsigned char vref_scan_enable,
	DRAM_TX_PER_BIT_CALIBRATION_TYTE_T cal_type,
	TX_FINAL_DLY_T *dqm_ptr, TX_DLY_T *dq_ptr)
{
	unsigned char byte_idx;
	unsigned int bak_channel;
	unsigned char dramc_byte[DQS_NUMBER];
	bak_channel = p->channel;

	for (byte_idx = 0; byte_idx < p->data_width/DQS_BIT_NUM; byte_idx++)
		dramc_byte[byte_idx] = mapping_phy_byte_to_dramc(p, byte_idx);

	for (byte_idx = 0; byte_idx < p->data_width/DQS_BIT_NUM;
			byte_idx += 2) {
		if ((p->data_width == DATA_WIDTH_32BIT) &&
			(byte_idx > 1)){
			p->channel = CHANNEL_B; /* Write to B23 reg */
		}

#if TX_K_DQM_WITH_WDBI
		if (cal_type == TX_DQ_DQS_MOVE_DQ_ONLY
			|| cal_type == TX_DQ_DQS_MOVE_DQ_DQM) {
			io_32_write_fld_align(DRAMC_REG_ADDR
				(DDRPHY_SHU_R0_B0_DQ7),
				dq_ptr->dq_final_pi[dramc_byte[byte_idx]],
				SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0);
			io_32_write_fld_align(DRAMC_REG_ADDR
				(DDRPHY_SHU_R0_B2_DQ7),
				dq_ptr->dq_final_pi[dramc_byte[byte_idx+1]],
				SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2);
		}

		if (cal_type == TX_DQ_DQS_MOVE_DQM_ONLY
			|| cal_type == TX_DQ_DQS_MOVE_DQ_DQM
			|| (cal_type == TX_DQ_DQS_MOVE_DQ_ONLY
			&& vref_scan_enable)) {
			io_32_write_fld_align(DRAMC_REG_ADDR
				(DDRPHY_SHU_R0_B0_DQ7),
				dqm_ptr->dq_final_dqm_pi[dramc_byte[byte_idx]],
				SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0);
			io_32_write_fld_align(DRAMC_REG_ADDR
				(DDRPHY_SHU_R0_B2_DQ7),
				dqm_ptr->dq_final_dqm_pi[dramc_byte[byte_idx+1]],
				SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2);
		}
#else /* !TX_K_DQM_WITH_WDBI */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ7),
			p_fld(dq_ptr->dq_final_pi[dramc_byte[byte_idx]],
			SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0) |
			p_fld(dqm_ptr->dq_final_dqm_pi[dramc_byte[byte_idx]],
			SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0));

		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ7),
			p_fld(dq_ptr->dq_final_pi[dramc_byte[byte_idx+1]],
			SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2) |
			p_fld(dqm_ptr->dq_final_dqm_pi[dramc_byte[byte_idx+1]],
			SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2));
#endif
	}

	p->channel = bak_channel;
}

#if ENABLE_TX_TRACKING
static void txdly_dqpi_txtracking_set(DRAMC_CTX_T *p,
	unsigned char vref_scan_enable,
	DRAM_TX_PER_BIT_CALIBRATION_TYTE_T cal_type,
	TX_FINAL_DLY_T *dqm_ptr, TX_DLY_T *dq_ptr)
{
	unsigned char byte_idx;
	unsigned char dramc_byte[DQS_NUMBER];

	for (byte_idx = 0; byte_idx < p->data_width/DQS_BIT_NUM; byte_idx++)
		dramc_byte[byte_idx] = mapping_phy_byte_to_dramc(p, byte_idx);

#if TX_K_DQM_WITH_WDBI
	if (cal_type == TX_DQ_DQS_MOVE_DQ_ONLY
		|| cal_type == TX_DQ_DQS_MOVE_DQM_ONLY)
#else
	if (cal_type == TX_DQ_DQS_MOVE_DQ_ONLY)
#endif
	{
		/* make a copy to dramc reg for TX DQ tracking used */
#if TX_K_DQM_WITH_WDBI
		if (cal_type == TX_DQ_DQS_MOVE_DQ_ONLY) {
			io_32_write_fld_multi(DRAMC_REG_ADDR
				(DRAMC_REG_SHURK0_PI),
				p_fld(dq_ptr->dq_final_pi[dramc_byte[0]],
				SHURK0_PI_RK0_ARPI_DQ_B0)
				| p_fld(dq_ptr->dq_final_pi[dramc_byte[1]],
				SHURK0_PI_RK0_ARPI_DQ_B1));

			/*  Source DQ */
			io_32_write_fld_multi(DRAMC_REG_ADDR
				(DRAMC_REG_SHURK0_DQS2DQ_CAL1),
				p_fld(dq_ptr->dq_final_pi[dramc_byte[1]],
				SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1)
				| p_fld(dq_ptr->dq_final_pi[dramc_byte[0]],
				SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0));
			/*  Target DQ */
			io_32_write_fld_multi(DRAMC_REG_ADDR
				(DRAMC_REG_SHURK0_DQS2DQ_CAL2),
				p_fld(dq_ptr->dq_final_pi[dramc_byte[1]],
				SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1)
				| p_fld(dq_ptr->dq_final_pi[dramc_byte[0]],
				SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0));
		}

		if (cal_type == TX_DQ_DQS_MOVE_DQM_ONLY
			|| (cal_type == TX_DQ_DQS_MOVE_DQ_ONLY
			&& vref_scan_enable)) {
			io_32_write_fld_multi(DRAMC_REG_ADDR
				(DRAMC_REG_SHURK0_PI),
				p_fld(dqm_ptr->dq_final_dqm_pi[dramc_byte[0]],
				SHURK0_PI_RK0_ARPI_DQM_B0)
				| p_fld(dqm_ptr->dq_final_dqm_pi[dramc_byte[1]],
				SHURK0_PI_RK0_ARPI_DQM_B1));

			/*  Target DQM */
			io_32_write_fld_multi(DRAMC_REG_ADDR
				(DRAMC_REG_SHURK0_DQS2DQ_CAL5),
				p_fld(dqm_ptr->dq_final_dqm_pi[dramc_byte[1]],
				SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1)
				| p_fld(dqm_ptr->dq_final_dqm_pi[dramc_byte[0]],
				SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0));
		}
#else /* !TX_K_DQM_WITH_WDBI */

		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_PI),
			p_fld(dq_ptr->dq_final_pi[dramc_byte[0]],
			SHURK0_PI_RK0_ARPI_DQ_B0)
			| p_fld(dq_ptr->dq_final_pi[dramc_byte[1]],
			SHURK0_PI_RK0_ARPI_DQ_B1)
			| p_fld(dqm_ptr->dq_final_dqm_pi[dramc_byte[0]],
			SHURK0_PI_RK0_ARPI_DQM_B0)
			| p_fld(dqm_ptr->dq_final_dqm_pi[dramc_byte[1]],
			SHURK0_PI_RK0_ARPI_DQM_B1));

		/*  Source DQ */
		io_32_write_fld_multi(DRAMC_REG_ADDR
			(DRAMC_REG_SHURK0_DQS2DQ_CAL1),
			p_fld(dq_ptr->dq_final_pi[dramc_byte[1]],
			SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ1)
			| p_fld(dq_ptr->dq_final_pi[dramc_byte[0]],
			SHURK0_DQS2DQ_CAL1_BOOT_ORIG_UI_RK0_DQ0));
		/*  Target DQ */
		io_32_write_fld_multi(DRAMC_REG_ADDR
			(DRAMC_REG_SHURK0_DQS2DQ_CAL2),
			p_fld(dq_ptr->dq_final_pi[dramc_byte[1]],
			SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ1)
			| p_fld(dq_ptr->dq_final_pi[dramc_byte[0]],
			SHURK0_DQS2DQ_CAL2_BOOT_TARG_UI_RK0_DQ0));

		/*  Target DQM */
		io_32_write_fld_multi(DRAMC_REG_ADDR
			(DRAMC_REG_SHURK0_DQS2DQ_CAL5),
			p_fld(dqm_ptr->dq_final_dqm_pi[dramc_byte[1]],
			SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM1)
			| p_fld(dqm_ptr->dq_final_dqm_pi[dramc_byte[0]],
			SHURK0_DQS2DQ_CAL5_BOOT_TARG_UI_RK0_DQM0));

#endif
	}
}
#endif /* End ENABLE_TX_TRACKING */

static void tx_cal_delay_cell_perbit(DRAMC_CTX_T *p, unsigned char byte,
	unsigned short center_min[], unsigned char delay_cell_ofst[],
	PASS_WIN_DATA_T final_win_per_bit[])
{
	unsigned char bit_idx;

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_TX_PER_BIT_DELAY_CELL)
		if(p->femmc_Ready==1 && (p->Bypass_TXWINDOW))
		{
			for (bit_idx = 0; bit_idx < DQS_BIT_NUM; bit_idx++)
			{
				unsigned char u1BitTemp = byte * DQS_BIT_NUM + bit_idx;
				delay_cell_ofst[u1BitTemp] = p->pSavetimeData->u1TX_PerBit_DelayLine_Save[p->channel][p->rank][u1BitTemp];
				show_msg((INFO,
						"delay_cell_ofst[%d]=%d cells\n",
						u1BitTemp, delay_cell_ofst[u1BitTemp]));
			}
		}
		else
#endif
		{
			/* calculate delay cell perbit */
			for (bit_idx = 0; bit_idx < DQS_BIT_NUM; bit_idx++) {
				unsigned char u1BitTemp = byte * DQS_BIT_NUM + bit_idx;
				unsigned char u1PIDiff =
					final_win_per_bit[u1BitTemp].win_center -
					center_min[byte];
				if (p->delay_cell_timex100 != 0) {
					delay_cell_ofst[u1BitTemp] = (u1PIDiff * 100000000 /
						(p->frequency * 64)) / p->delay_cell_timex100;
					show_diag((INFO,
						"delay_cell_ofst[%d]=%d cells (%d PI)\n",
						u1BitTemp, delay_cell_ofst[u1BitTemp],
						u1PIDiff));
					if(delay_cell_ofst[u1BitTemp] > 15) {
						show_err3("[WARNING] TX DQ%d delay cell %d >15, adjust to 15 cell\n", bit_idx, delay_cell_ofst[u1BitTemp]);
						delay_cell_ofst[u1BitTemp] = 15;
					}

				} else {
					show_msg((INFO, "Error: Cell time %s is 0\n",
						"(p->delay_cell_timex100)"));
					break;
				}
			}
#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_TX_PER_BIT_DELAY_CELL)
			for (bit_idx = 0; bit_idx < DQS_BIT_NUM; bit_idx++)
			{
				unsigned char u1BitTemp = byte * DQS_BIT_NUM + bit_idx;
				p->pSavetimeData->u1TX_PerBit_DelayLine_Save[p->channel][p->rank][u1BitTemp] = delay_cell_ofst[u1BitTemp];
			}
#endif
		}
}

static unsigned int tx_win_per_bit_cal(DRAMC_CTX_T *p,
	unsigned int finish_count, unsigned char bit_idx,
	unsigned int fail_bit, unsigned short delay,
	unsigned short dq_delay_end, PASS_WIN_DATA_T win_per_bit[],
	PASS_WIN_DATA_T vrefwin_per_bit[])
{
	if (win_per_bit[bit_idx].first_pass ==
		PASS_RANGE_NA) {
		if (fail_bit == 0)
			win_per_bit[bit_idx].first_pass = delay;
	} else if (win_per_bit[bit_idx].last_pass ==
		PASS_RANGE_NA) {
		if (fail_bit != 0)
			win_per_bit[bit_idx].last_pass = (delay - 1);
		else if (delay == dq_delay_end)
			win_per_bit[bit_idx].last_pass = delay;

		if (win_per_bit[bit_idx].last_pass != PASS_RANGE_NA) {
			if ((win_per_bit[bit_idx].last_pass -
				win_per_bit[bit_idx].first_pass) >=
				(vrefwin_per_bit[bit_idx].last_pass -
				vrefwin_per_bit[bit_idx].first_pass)) {
				if ((vrefwin_per_bit[bit_idx].last_pass !=
					PASS_RANGE_NA) && (vrefwin_per_bit
					[bit_idx].last_pass - vrefwin_per_bit
					[bit_idx].first_pass) > 0) {
					show_msg2((INFO, "Bit[%d%s%d > %d%s",
						bit_idx,
						"] Bigger window update ",
						(win_per_bit[bit_idx].last_pass
						- win_per_bit[bit_idx].
						first_pass), (vrefwin_per_bit
						[bit_idx].last_pass -
						vrefwin_per_bit[bit_idx].
						first_pass),
						", window broken?\n"));

				}
				/* if window size bigger than 7,
				* consider as real pass window.
				* If not, don't update finish counte
				* and won't do early break;
				*/
				if ((win_per_bit[bit_idx].last_pass -
					win_per_bit[bit_idx].first_pass) > 7)
					finish_count |= (1 << bit_idx);

				/* update bigger window size */
				vrefwin_per_bit[bit_idx].first_pass =
					win_per_bit[bit_idx].first_pass;
				vrefwin_per_bit[bit_idx].last_pass =
					win_per_bit[bit_idx].last_pass;
			}
			/* reset tmp window */
			win_per_bit[bit_idx].first_pass = PASS_RANGE_NA;
			win_per_bit[bit_idx].last_pass = PASS_RANGE_NA;
		}
	}
return finish_count;
}

static void tx_move_dq(DRAMC_CTX_T *p, unsigned char vref_scan_enable,
	DRAM_TX_PER_BIT_CALIBRATION_TYTE_T cal_type,
	unsigned short dq_delay_begin, unsigned short dq_delay_end,
	PASS_WIN_DATA_T win_per_bit[], PASS_WIN_DATA_T vrefwin_per_bit[])
{
	unsigned char bit_idx;
	unsigned short delay;
	unsigned char dq_ui_small_reg_value;
	unsigned int finish_count;
	unsigned int err_value, fail_bit;
	unsigned short step;

	/*  initialize parameters */
	finish_count = 0;
	dq_ui_small_reg_value = BYTE_MAX;
#if (FOR_DV_SIMULATION_USED == 0)
	step = 1;
#else
	step = 4;
#endif

	for (bit_idx = 0; bit_idx < p->data_width; bit_idx++) {
		win_per_bit[bit_idx].first_pass =
			(signed short) PASS_RANGE_NA;
		win_per_bit[bit_idx].last_pass =
			(signed short) PASS_RANGE_NA;
		vrefwin_per_bit[bit_idx].first_pass =
			(signed short) PASS_RANGE_NA;
		vrefwin_per_bit[bit_idx].last_pass =
			(signed short) PASS_RANGE_NA;
	}

	/*
	 * Move DQ delay ,  1 PI = tCK/64, total 128 PI, 1UI = 32 PI
	 * For data rate 3200, max tDQS2DQ is 2.56UI (82 PI)
	 * For data rate 4266, max tDQS2DQ is 3.41UI (109 PI)
	 */
	for (delay = dq_delay_begin; delay < dq_delay_end; delay += step) {
		dq_ui_small_reg_value = move_dq_delay_lp4(p, delay, cal_type,
			vref_scan_enable, dq_ui_small_reg_value);

		/* audio +xtalk pattern */
		err_value = 0;
		/* cc notes: for Simulation, too time costy...
		 * So only use XTALK.
		 */
	#if (FOR_DV_SIMULATION_USED == 0)
		dramc_engine2_set_pat(p, TEST_AUDIO_PATTERN, 0, 0);
		err_value = dramc_engine2_run(p, TE_OP_WRITE_READ_CHECK,
			TEST_AUDIO_PATTERN);
	#endif
		dramc_engine2_set_pat(p, TEST_XTALK_PATTERN, 0, 0);
		err_value |= dramc_engine2_run(p, TE_OP_WRITE_READ_CHECK,
			TEST_XTALK_PATTERN);
#if CALIBRATION_LOG
		show_msg2((INFO, "0x%x [0]", err_value));
#endif
		/* check fail bit ,0 ok ,others fail */
		for (bit_idx = 0; bit_idx < p->data_width; bit_idx++) {
			fail_bit = err_value & ((unsigned int) 1 << bit_idx);
#if CALIBRATION_LOG
			if (bit_idx % DQS_BIT_NUM == 0)
				show_msg2((INFO, " "));
			if (fail_bit == 0)
				show_msg2((INFO, "o"));
			else
				show_msg2((INFO, "x"));
#endif
			finish_count = tx_win_per_bit_cal(p, finish_count,
				bit_idx, fail_bit, delay, dq_delay_end,
				win_per_bit, vrefwin_per_bit);
		}
#if CALIBRATION_LOG
		show_msg2((INFO, " [MSB]\n"));
#endif
		/*
		 * if all bits widnow found and all bits turns to fail again,
		 * early break;
		*/
		if (((p->data_width == DATA_WIDTH_16BIT) &&
			 (finish_count == WORD_MAX)) ||
			((p->data_width == DATA_WIDTH_32BIT) &&
			 (finish_count == LWORD_MAX))) {
			set_calibration_result(p, DRAM_CALIBRATION_TX_PERBIT,
				DRAM_OK);
				break;
		}
	}
}

static void tx_set_delay_cell(DRAMC_CTX_T *p,
	unsigned char delay_cell_ofst[])
{
	unsigned char *lpddr_4bitmux_byte_mapping;
	unsigned int b0_reg, b1_reg, b2_reg, b3_reg;

	unsigned char *dq_mapping;

	dq_mapping = dramc_get_phy2dramc_dq_mapping(p);

	b0_reg = DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ0);
	b1_reg = DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ0);

	if (p->data_width == DATA_WIDTH_32BIT) {
		b2_reg = DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ0 +
			(CHANNEL_B << POS_BANK_NUM));
		b3_reg = DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ0 +
			(CHANNEL_B << POS_BANK_NUM));
	}

	io_32_write_fld_multi(b0_reg,
		p_fld(delay_cell_ofst[dq_mapping[7]],
		SHU_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0) |
		p_fld(delay_cell_ofst[dq_mapping[6]],
		SHU_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0) |
		p_fld(delay_cell_ofst[dq_mapping[5]],
		SHU_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0) |
		p_fld(delay_cell_ofst[dq_mapping[4]],
		SHU_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0) |
		p_fld(delay_cell_ofst[dq_mapping[3]],
		SHU_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0) |
		p_fld(delay_cell_ofst[dq_mapping[2]],
		SHU_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0) |
		p_fld(delay_cell_ofst[dq_mapping[1]],
		SHU_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0) |
		p_fld(delay_cell_ofst[dq_mapping[0]],
		SHU_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0));
	io_32_write_fld_multi(b1_reg,
		p_fld(delay_cell_ofst[dq_mapping[15]],
		SHU_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1) |
		p_fld(delay_cell_ofst[dq_mapping[14]],
		SHU_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1) |
		p_fld(delay_cell_ofst[dq_mapping[13]],
		SHU_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1) |
		p_fld(delay_cell_ofst[dq_mapping[12]],
		SHU_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1) |
		p_fld(delay_cell_ofst[dq_mapping[11]],
		SHU_R0_B1_DQ0_RK0_TX_ARDQ3_DLY_B1) |
		p_fld(delay_cell_ofst[dq_mapping[10]],
		SHU_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1) |
		p_fld(delay_cell_ofst[dq_mapping[9]],
		SHU_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1) |
		p_fld(delay_cell_ofst[dq_mapping[8]],
		SHU_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1));

	/* Set PHY B23 */
	if (p->data_width == DATA_WIDTH_32BIT) {
		io_32_write_fld_multi(b2_reg,
			p_fld(delay_cell_ofst[dq_mapping[23]],
			SHU_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0) |
			p_fld(delay_cell_ofst[dq_mapping[22]],
			SHU_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0) |
			p_fld(delay_cell_ofst[dq_mapping[21]],
			SHU_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0) |
			p_fld(delay_cell_ofst[dq_mapping[20]],
			SHU_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0) |
			p_fld(delay_cell_ofst[dq_mapping[19]],
			SHU_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0) |
			p_fld(delay_cell_ofst[dq_mapping[18]],
			SHU_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0) |
			p_fld(delay_cell_ofst[dq_mapping[17]],
			SHU_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0) |
			p_fld(delay_cell_ofst[dq_mapping[16]],
			SHU_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0));
		io_32_write_fld_multi(b3_reg,
			p_fld(delay_cell_ofst[dq_mapping[31]],
			SHU_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1) |
			p_fld(delay_cell_ofst[dq_mapping[30]],
			SHU_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1) |
			p_fld(delay_cell_ofst[dq_mapping[29]],
			SHU_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1) |
			p_fld(delay_cell_ofst[dq_mapping[28]],
			SHU_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1) |
			p_fld(delay_cell_ofst[dq_mapping[27]],
			SHU_R0_B1_DQ0_RK0_TX_ARDQ3_DLY_B1) |
			p_fld(delay_cell_ofst[dq_mapping[26]],
			SHU_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1) |
			p_fld(delay_cell_ofst[dq_mapping[25]],
			SHU_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1) |
			p_fld(delay_cell_ofst[dq_mapping[24]],
			SHU_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1));
	}
}

DRAM_STATUS_T dramc_tx_window_perbit_cal(DRAMC_CTX_T *p,
	DRAM_TX_PER_BIT_CALIBRATION_TYTE_T cal_type,
	unsigned char vref_scan_enable)
{
	unsigned char ucindex, bit_idx, byte, u1BitTemp;
	unsigned char ii, backup_rank;
	PASS_WIN_DATA_T win_per_bit[DQ_DATA_WIDTH],
		vrefwin_per_bit[DQ_DATA_WIDTH],
		final_win_per_bit[DQ_DATA_WIDTH];
#if TX_K_DQM_WITH_WDBI
	unsigned char mck2ui;
#endif
	unsigned short smallest_virtual_delay = WORD_MAX;
	unsigned short delay, dq_delay_begin = 0, dq_delay_end = 0,
		tx_dq_pre_cal_lp4_samll;
	TX_DLY_T final_dq_dqm;
	TX_FINAL_DLY_T dq_final_dqm;

	show_msg_with_timestamp((INFO, "start Tx DQ window cal\n"));

	/* TX_DQM_CALC_MAX_MIN_CENTER */
	unsigned short center_min[DQS_NUMBER], center_max[DQS_NUMBER];
	unsigned char enable_delay_cell = 0;
	unsigned char delay_cell_ofst[DQ_DATA_WIDTH];
	unsigned short vref_range, vref, final_vref = 0xd;
	unsigned short vref_begin, final_range = 0, vref_end, vref_step;
	unsigned short temp_win_sum, tx_window_sum;
	unsigned short tx_perbit_win_min_max = 0;
	unsigned short tx_perbit_win_min_max_idx = 0;
	unsigned int min_bit;
	unsigned int min_winsize;
	unsigned char enable_full_eye_scan = 0;

	if (!p) {
		show_err("context NULL\n");
		return DRAM_FAIL;
	}
	print_calibration_basic_info(p);
	if (vref_scan_enable)
		print_calibration_basic_info_diag(p);
	memset(&final_dq_dqm, 0x0, sizeof(TX_DLY_T));

	backup_rank = get_rank(p);

	/* Set TX delay chain to 0 */
#if TX_K_DQM_WITH_WDBI
	if (cal_type != TX_DQ_DQS_MOVE_DQM_ONLY)
#endif
	{
		io32_write_4b(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ0), 0);
		io32_write_4b(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ0), 0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ1),
			CLEAR_FLD, SHU_R0_B0_DQ1_RK0_TX_ARDQM0_DLY_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ1),
			CLEAR_FLD, SHU_R0_B2_DQ1_RK0_TX_ARDQM0_DLY_B2);
	}

#if TX_K_DQM_WITH_WDBI
	mck2ui = VALUE_3;
#endif
	smallest_virtual_delay = find_smallest_dqs_delay(p);

	/*
	* (1)LP4 will calibration DQM at the first time, K full range,
	* and then rember the TX position.
	* (2)The sencod time will calibrate DQ+Vref, reference TX postion of (1)
	*/
	if (cal_type == TX_DQ_DQS_MOVE_DQ_DQM) {
		dq_delay_begin = smallest_virtual_delay;
		dq_delay_end = dq_delay_begin + BYTE_MAX;
	} else {	/* (cal_type==TX_DQ_DQS_MOVE_DQ_ONLY) */
		if (tx_dq_pre_cal_lp4[0] < tx_dq_pre_cal_lp4[1])
			tx_dq_pre_cal_lp4_samll = tx_dq_pre_cal_lp4[0];
		else
			tx_dq_pre_cal_lp4_samll = tx_dq_pre_cal_lp4[1];

		if (tx_dq_pre_cal_lp4_samll > 24)
			dq_delay_begin = tx_dq_pre_cal_lp4_samll - 24;
		else
			dq_delay_begin = 0;

#if TX_K_DQM_WITH_WDBI
		if (cal_type == TX_DQ_DQS_MOVE_DQM_ONLY) {
			/* DBI on, calibration range -1MCK */
			dq_delay_begin -= (1 << (mck2ui + 5));
		}
#endif
		dq_delay_end = dq_delay_begin + VALUE_64;
	}

	if (vref_scan_enable) {
		if (is_lp4_family(p)) {
#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_VREF_CAL)
			if(p->femmc_Ready==1) {
				vref_begin = p->pSavetimeData->u1TxWindowPerbitVref_Save[p->channel][p->rank];
				vref_end = vref_begin;
				vref_step = 1;
			}
			else
#endif
			{
				if (p->odt_onoff == ODT_OFF) {
					if (p->dram_type == TYPE_LPDDR4) {
						/* range 1 */
						vref_begin = TX_VREF_RANGE_BEGIN1;
						vref_end = TX_VREF_RANGE_END1;
					} else {
						/* range 1 */
						vref_begin = TX_VREF_RANGE_BEGIN2;
						vref_end = TX_VREF_RANGE_END2;
					}
					vref_step = 1;
				} else {
					/* range 0 */
					vref_begin = TX_VREF_RANGE_BEGIN;
					vref_end = TX_VREF_RANGE_END;
					vref_step = TX_VREF_RANGE_STEP;
				}
			}
		} else if (p->dram_type == TYPE_PCDDR4) {
			vref_begin = 0x0;
			vref_end = 0x32;
			vref_step = 2;
		}
	} else {
		vref_begin = 0;
		vref_end = 0;
		vref_step = 1;
	}

	final_range = vref_range = (!(p->odt_onoff));

	tx_window_sum = 0;
	set_calibration_result(p, DRAM_CALIBRATION_TX_PERBIT, DRAM_FAIL);
	if (vref_scan_enable) {
		show_diag((INFO,
			"[TxWindowPerbitCal] cal_type=%d, VrefScanEnable %d (%d~%d)\n"
			"Begin, DQ Scan Range %d~%d\n", cal_type, vref_scan_enable,
			vref_begin, vref_end, dq_delay_begin, dq_delay_end));
	} else {
		show_msg3((INFO,
			"[TxWindowPerbitCal] cal_type=%d, VrefScanEnable %d (%d~%d)\n"
			"Begin, DQ Scan Range %d~%d\n", cal_type, vref_scan_enable,
			vref_begin, vref_end, dq_delay_begin, dq_delay_end));
	}

	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), SET_FLD,
		MISC_CTRL1_R_DMARPIDQ_SW);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), SET_FLD,
		DQSOSCR_ARUIDQ_SW);

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
    if(p->femmc_Ready==1 && (p->Bypass_TXWINDOW))
    {
    	tx_perbit_win_min_max = WORD_MAX;
        for (byte = 0; byte < (p->data_width / DQS_BIT_NUM);
				byte++)
        {
            center_min[byte]=p->pSavetimeData->u1TxCenter_min_Save[p->channel][p->rank][byte];
            center_max[byte]=p->pSavetimeData->u1TxCenter_max_Save[p->channel][p->rank][byte];
            for (bit_idx=0; bit_idx<DQS_BIT_NUMBER; bit_idx++)
            {
                u1BitTemp = byte*DQS_BIT_NUMBER+bit_idx;
                final_win_per_bit[u1BitTemp].first_pass= p->pSavetimeData->u1Txfirst_pass_Save[p->channel][p->rank][u1BitTemp];
                final_win_per_bit[u1BitTemp].last_pass= p->pSavetimeData->u1Txlast_pass_Save[p->channel][p->rank][u1BitTemp];
                final_win_per_bit[u1BitTemp].win_center= p->pSavetimeData->u1Txwin_center_Save[p->channel][p->rank][u1BitTemp];
                final_win_per_bit[u1BitTemp].win_size= (final_win_per_bit[u1BitTemp].last_pass - final_win_per_bit[u1BitTemp].first_pass +
                                                        (final_win_per_bit[u1BitTemp].last_pass == final_win_per_bit[u1BitTemp].first_pass ? 0 : 1));
                if (final_win_per_bit[u1BitTemp].win_size < tx_perbit_win_min_max)
                {
                    tx_perbit_win_min_max = final_win_per_bit[u1BitTemp].win_size;
                    tx_perbit_win_min_max_idx = u1BitTemp;
                }
                tx_window_sum += final_win_per_bit[u1BitTemp].win_size;
            }
		}
		show_msg((INFO, "[FAST_K] Bypass TxWindowPerbitCal\n"));
		final_vref = p->pSavetimeData->u1TxWindowPerbitVref_Save[p->channel][p->rank];
        set_calibration_result(p, DRAM_CALIBRATION_TX_PERBIT, DRAM_OK);
    }
    else
#endif
	{
		dramc_engine2_init(p, p->test2_1, p->test2_2, p->test_pattern, 0);

		for (vref = vref_begin; vref <= vref_end; vref += vref_step) {
			/*  SET tx Vref (DQ) here */
			if (vref_scan_enable)
				set_txdq_vref(p, vref_range, vref);
			else
				show_msg3((INFO, "\n\tTX Vref Scan disable\n"));

			/*  initialize parameters */
			temp_win_sum = 0;

			tx_move_dq(p, vref_scan_enable, cal_type, dq_delay_begin,
				dq_delay_end, win_per_bit, vrefwin_per_bit);

			min_winsize = WORD_MAX;
			min_bit = BYTE_MAX;
			for (bit_idx = 0; bit_idx < p->data_width; bit_idx++) {
				vrefwin_per_bit[bit_idx].win_size =
					vrefwin_per_bit[bit_idx].last_pass -
					vrefwin_per_bit[bit_idx].first_pass +
					(vrefwin_per_bit[bit_idx].last_pass ==
					vrefwin_per_bit[bit_idx].first_pass ? 0 : 1);

				if (vrefwin_per_bit[bit_idx].win_size < min_winsize) {
				min_bit = bit_idx;
				min_winsize = vrefwin_per_bit[bit_idx].win_size;
				}
				/* Sum of CA Windows for vref selection */
				temp_win_sum += vrefwin_per_bit[bit_idx].win_size;
			}
			show_msg((INFO, "TXVref=%d, MinBit=%d, winsize=%d\n", vref,
				min_bit, min_winsize));
			if ((min_winsize > tx_perbit_win_min_max)
				|| ((min_winsize == tx_perbit_win_min_max)
				&& (temp_win_sum > tx_window_sum))) {
			if (cal_type == TX_DQ_DQS_MOVE_DQ_ONLY) {
				show_diag((INFO, "Better TX Vref found %d, Window Min %d >= %d at DQ%d, Window Sum %d > %d\n",
					 vref, min_winsize, tx_perbit_win_min_max, min_bit, temp_win_sum, tx_window_sum));
			}

				tx_perbit_win_min_max = min_winsize;
				tx_perbit_win_min_max_idx = min_bit;
				tx_window_sum = temp_win_sum;
				final_range = vref_range;
				final_vref = vref;

				/* Calculate the center of DQ pass window */
				/* Record center sum of each byte */
				for (byte = 0; byte < (p->data_width / DQS_BIT_NUM);
					byte++) {
					/* TX_DQM_CALC_MAX_MIN_CENTER */
					center_min[byte] = WORD_MAX;
					center_max[byte] = 0;

					for (bit_idx = 0; bit_idx < DQS_BIT_NUM;
						bit_idx++) {
						ucindex = byte * DQS_BIT_NUM + bit_idx;
						final_win_per_bit[ucindex].first_pass =
							vrefwin_per_bit[ucindex].
							first_pass;
						final_win_per_bit[ucindex].last_pass =
							vrefwin_per_bit[ucindex].
							last_pass;
						final_win_per_bit[ucindex].win_size =
							vrefwin_per_bit[ucindex].
							win_size;
						final_win_per_bit[ucindex].win_center =
							(final_win_per_bit[ucindex].
							first_pass + final_win_per_bit
							[ucindex].last_pass) >> 1;

						if (final_win_per_bit[ucindex].
							win_center < center_min[byte])
							center_min[byte] =
								final_win_per_bit
								[ucindex].win_center;

						if (final_win_per_bit[ucindex].
							win_center > center_max[byte])
							center_max[byte] =
								final_win_per_bit
								[ucindex].win_center;

					}
				}
		} else if (cal_type == TX_DQ_DQS_MOVE_DQ_ONLY) {
			show_diag((INFO, "TX Vref %d, Window Min %d <= %d at DQ%d, Window Sum %d <= %d\n",
				 vref, min_winsize, tx_perbit_win_min_max, min_bit, temp_win_sum, tx_window_sum));
		}

			if ((temp_win_sum < (tx_window_sum * 95 / 100))
				&& vref_scan_enable && enable_full_eye_scan == 0) {
				show_msg3((INFO,
					"\nTX Vref found, early break! %d< %d\n",
					temp_win_sum, (tx_window_sum * 95 / 100)));
				break;	/* max vref found (95%) , early break; */
			}
		}
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	if(p->femmc_Ready==0)//save firtst run pass value
	{
		for (byte = 0; byte < (p->data_width / DQS_BIT_NUM);
			byte++)
		{
			if(cal_type == TX_DQ_DQS_MOVE_DQ_ONLY) // && u1VrefScanEnable==0
			{
				p->pSavetimeData->u1TxCenter_min_Save[p->channel][p->rank][byte]=center_min[byte];
				p->pSavetimeData->u1TxCenter_max_Save[p->channel][p->rank][byte]=center_max[byte];
				for (bit_idx=0; bit_idx<DQS_BIT_NUMBER; bit_idx++)
				{
					u1BitTemp = byte*DQS_BIT_NUMBER+bit_idx;
					p->pSavetimeData->u1Txfirst_pass_Save[p->channel][p->rank][u1BitTemp] = final_win_per_bit[u1BitTemp].first_pass;
					p->pSavetimeData->u1Txlast_pass_Save[p->channel][p->rank][u1BitTemp] = final_win_per_bit[u1BitTemp].last_pass;
					p->pSavetimeData->u1Txwin_center_Save[p->channel][p->rank][u1BitTemp] = final_win_per_bit[u1BitTemp].win_center;
				}
			}
		}
		// save first run Vref value
		if (cal_type == TX_DQ_DQS_MOVE_DQ_ONLY)
			p->pSavetimeData->u1TxWindowPerbitVref_Save[p->channel][p->rank]=final_vref;
	}
#endif

		dramc_engine2_end(p);
	}

#if (CONFIG_FOR_HQA_TEST_USED == 1)
	if((is_lp4_family(p) && cal_type == TX_DQ_DQS_MOVE_DQ_ONLY &&
		(vref_scan_enable == 0)) ||
		(is_lp4_family(p) && cal_type == TX_DQ_DQS_MOVE_DQ_DQM)) {
		for (bit_idx = 0; bit_idx < p->data_width; bit_idx++) {
			gFinalTXPerbitWin[p->channel][p->rank][bit_idx] =
				vrefwin_per_bit[bit_idx].win_size;
		}
	}
		// LP4 DQ time domain || LP3 DQ_DQM time domain
		if((is_lp4_family(p) && (cal_type == TX_DQ_DQS_MOVE_DQ_ONLY)))
		{
			gFinalTXPerbitWin_min_max[p->channel][p->rank] = tx_perbit_win_min_max;
		}
#endif

		if (vref_scan_enable && (cal_type == TX_DQ_DQS_MOVE_DQ_ONLY)) {
			for (bit_idx = 0; bit_idx < DQS_BIT_NUM; bit_idx++) {
				show_diag((INFO, "[CH%d][RK%d][%d][TX] Bit%d (%d~%d) %d %d,   Bit%d (%d~%d) %d %d,\n", \
							p->channel, p->rank, p->frequency*2,\
							bit_idx, final_win_per_bit[bit_idx].first_pass, final_win_per_bit[bit_idx].last_pass, final_win_per_bit[bit_idx].win_size, final_win_per_bit[bit_idx].win_center, \
							bit_idx+8, final_win_per_bit[bit_idx+8].first_pass, final_win_per_bit[bit_idx+8].last_pass, final_win_per_bit[bit_idx+8].win_size, final_win_per_bit[bit_idx+8].win_center));
			}
			show_msg0((INFO, "\n[CH%d][RK%d][%d][TX] Best Vref %d, Window Min %d at DQ%d, Window Sum %d\n",\
				 p->channel, p->rank, p->frequency*2,\
				 final_vref, tx_perbit_win_min_max, tx_perbit_win_min_max_idx, tx_window_sum));

			if(tx_perbit_win_min_max < TX_WIN_CRITERIA){
				show_msg0((INFO, "TX margin fail@DQ%d, %dPI<%dPI\n",
					tx_perbit_win_min_max_idx, tx_perbit_win_min_max, TX_WIN_CRITERIA));
#ifdef DRAM_SLT
				dram_slt_set(p, DRAM_CALIBRATION_TX_PERBIT, DRAM_FAIL);
#endif
			}
		}

	/*
	* first freq 800(LP4-1600) doesn't support jitter meter(data < 1T),
	* therefore, don't use delay cell
	*/
	if ((cal_type == TX_DQ_DQS_MOVE_DQ_ONLY)
		&& (p->frequency >= PERBIT_THRESHOLD_FREQ)
		&& (p->delay_cell_timex100 != 0)) {
		enable_delay_cell = 1;
		show_diag((INFO, "%sDelayCellTimex100 =%d/100 ps\n",
			"[TX_PER_BIT_DELAY_CELL] ", p->delay_cell_timex100));
	}
	/* Calculate the center of DQ pass window */
	/* average the center delay */
	for (byte = 0; byte < (p->data_width / DQS_BIT_NUM); byte++) {
		if (enable_delay_cell == 0) {
			delay = ((center_min[byte] + center_max[byte]) >> 1);
			tx_dq_pre_cal_lp4[byte] = delay;
		} else {	/*  if(cal_type == TX_DQ_DQS_MOVE_DQ_ONLY) */
			delay = center_min[byte];
			tx_dq_pre_cal_lp4[byte] = ((center_min[byte] +
				center_max[byte]) >> 1);

			/* calculate delay cell perbit */
			tx_cal_delay_cell_perbit(p, byte, center_min,
				delay_cell_ofst, final_win_per_bit);
		}

		tx_win_transfer_delay_to_uipi(p, delay, 1,
			&final_dq_dqm.dq_final_ui_large[byte],
			&final_dq_dqm.dq_final_ui_small[byte],
			&final_dq_dqm.dq_final_pi[byte],
			&final_dq_dqm.dq_final_oen_ui_large[byte],
			&final_dq_dqm.dq_final_oen_ui_small[byte]);

		tx_win_transfer_delay_to_uipi(p, tx_dq_pre_cal_lp4[byte], 1,
			&dq_final_dqm.dq_final_dqm_ui_large[byte],
			&dq_final_dqm.dq_final_dqm_ui_small[byte],
			&dq_final_dqm.dq_final_dqm_pi[byte],
			&dq_final_dqm.dq_final_dqm_oen_ui_large[byte],
			&dq_final_dqm.dq_final_dqm_oen_ui_small[byte]);

		if (vref_scan_enable) {
			show_diag((INFO, "Byte%d, DQ PI dly=%d, DQM PI dly= %d\n",
				byte, delay, tx_dq_pre_cal_lp4[byte]));
			show_diag((INFO, "%s=(%d ,%d, %d)\n",
				"Final DQ PI dly(LargeUI, SmallUI, PI) ",
				final_dq_dqm.dq_final_ui_large[byte],
				final_dq_dqm.dq_final_ui_small[byte],
				final_dq_dqm.dq_final_pi[byte]));
			show_diag((INFO, "%s =(%d ,%d, %d)\n\n",
				"OEN DQ PI dly(LargeUI, SmallUI, PI)",
				final_dq_dqm.dq_final_oen_ui_large[byte],
				final_dq_dqm.dq_final_oen_ui_small[byte],
				final_dq_dqm.dq_final_pi[byte]));
		} else {
			show_msg((INFO, "Byte%d, DQ PI dly=%d, DQM PI dly= %d\n",
				byte, delay, tx_dq_pre_cal_lp4[byte]));
			show_msg((INFO, "%s=(%d ,%d, %d)\n",
				"Final DQ PI dly(LargeUI, SmallUI, PI) ",
				final_dq_dqm.dq_final_ui_large[byte],
				final_dq_dqm.dq_final_ui_small[byte],
				final_dq_dqm.dq_final_pi[byte]));
			show_msg((INFO, "%s =(%d ,%d, %d)\n\n",
				"OEN DQ PI dly(LargeUI, SmallUI, PI)",
				final_dq_dqm.dq_final_oen_ui_large[byte],
				final_dq_dqm.dq_final_oen_ui_small[byte],
				final_dq_dqm.dq_final_pi[byte]));
		}
	}

	for (ii = p->rank; ii < p->support_rank_num; ii++) {
		set_rank(p, ii);

		txdly_dq_set(p, cal_type, &final_dq_dqm);
		txdly_dqm_set(p, cal_type, &dq_final_dqm, vref_scan_enable);
		txdly_dqpi_set(p, vref_scan_enable, cal_type, &dq_final_dqm,
			&final_dq_dqm);
#if TX_K_DQM_WITH_WDBI
		if (cal_type == TX_DQ_DQS_MOVE_DQ_ONLY
			|| cal_type == TX_DQ_DQS_MOVE_DQ_DQM)
#endif
		{
			if (enable_delay_cell)
				tx_set_delay_cell(p, delay_cell_ofst);
		}

#if ENABLE_TX_TRACKING
		txdly_dqpi_txtracking_set(p,  vref_scan_enable, cal_type,
			&dq_final_dqm, &final_dq_dqm);
#endif
	}

	set_rank(p, backup_rank);

	if (vref_scan_enable) {
		set_txdq_vref(p, final_range, final_vref);
		gFinalTXVrefDQRange[p->channel][p->rank] = final_range;
		show_diag((INFO, "Final TX Range %d Vref %d\n",
			final_range, final_vref));
		gFinalTXVrefDQ[p->channel][p->rank] = (U8) final_vref;
	}

#ifdef DIAG // TX Vref scan at center
	if (vref_scan_enable) {
        // u2WinVrefPerm_x stands for the per-mil offset based on the Final Vref per-mil
        // u2WinVrefPerm_x / MR_VREF_STEP_PERM stands for the step offset based on the Final Vref
        // _L means low-bond, _H means high-bond
        u16 u2WinVrefPerm_L, u2WinVrefPerm_H;
        u16 u2FinalTXVrefConf, u2FinalTXVrefPerm, u2TestTXVrefPerm;
        u16 u2TestTXVrefPerm_L_Start, u2TestTXVrefPerm_L_End, u2TestTXVrefPerm_H_Start, u2TestTXVrefPerm_H_End;
		u32 u4err_value;

        dramc_mrr_by_rank(p, p->rank, 14, &u2FinalTXVrefConf); // MR14 is read-write able
        u2FinalTXVrefPerm = CONVERT_VREF_CONF_TO_PERM(u2FinalTXVrefConf);

        u2TestTXVrefPerm_L_Start = (u2FinalTXVrefPerm - TX_VREF_CHECK_WIN_PERM_START > MR_VREF_RANGE0_MIN_PERM) ? (u2FinalTXVrefPerm - TX_VREF_CHECK_WIN_PERM_START) : MR_VREF_RANGE0_MIN_PERM;
        u2TestTXVrefPerm_L_End = (u2FinalTXVrefPerm - TX_VREF_CHECK_WIN_PERM_END > MR_VREF_RANGE0_MIN_PERM) ? (u2FinalTXVrefPerm - TX_VREF_CHECK_WIN_PERM_END) : MR_VREF_RANGE0_MIN_PERM;
        u2TestTXVrefPerm_H_Start = (u2FinalTXVrefPerm + TX_VREF_CHECK_WIN_PERM_START < MR_VREF_RANGE1_MAX_PERM) ? (u2FinalTXVrefPerm + TX_VREF_CHECK_WIN_PERM_START) : MR_VREF_RANGE1_MAX_PERM;
        u2TestTXVrefPerm_H_End = (u2FinalTXVrefPerm + TX_VREF_CHECK_WIN_PERM_END < MR_VREF_RANGE1_MAX_PERM) ? (u2FinalTXVrefPerm + TX_VREF_CHECK_WIN_PERM_END) : MR_VREF_RANGE1_MAX_PERM;

        // Keep the delay settings (x axis), reduce/increase Vref (y axis) and check pass/fail to get margin on Y axis
        // TX will scan the neighbor 4 steps around the pass criteria. Refer to JEDEC, we need to cover 140mV for TX Vref.
        // 140mV means 70mV on each side (up/bottom), i.e. 70/4.4=15.9 -> 16 steps
        // we will check 12~20 steps on both direction, verified by AUDIO & XTALK pattern
        // then BFT will 100% detect the mainboards with insufficient Vref window height and filter them out for FA
        //
        // Noted, the loop here is count by per-mil, 0.4% per step
        u2WinVrefPerm_L = u2FinalTXVrefPerm - u2TestTXVrefPerm_L_End;
        for (u2TestTXVrefPerm = u2TestTXVrefPerm_L_Start; u2TestTXVrefPerm > u2TestTXVrefPerm_L_End; u2TestTXVrefPerm -= MR_VREF_STEP_PERM)
        {
            dramc_mode_reg_write_by_rank(p, p->rank, 14, CONVERT_VREF_PERM_TO_CONF(u2TestTXVrefPerm));
            DRAMC_ENGINE_DO_TX_TEST(p, u4err_value);
            if (u4err_value != 0)
            {
                show_diag((INFO, "TX Vref L fail at perm %d, conf 0x%x\n", u2TestTXVrefPerm, CONVERT_VREF_PERM_TO_CONF(u2TestTXVrefPerm)));
                u2WinVrefPerm_L = u2FinalTXVrefPerm - u2TestTXVrefPerm - MR_VREF_STEP_PERM;
                break;
            }
        }
        u2WinVrefPerm_H = u2TestTXVrefPerm_H_End - u2FinalTXVrefPerm;
        for (u2TestTXVrefPerm = u2TestTXVrefPerm_H_Start; u2TestTXVrefPerm < u2TestTXVrefPerm_H_End; u2TestTXVrefPerm += MR_VREF_STEP_PERM)
        {
            dramc_mode_reg_write_by_rank(p, p->rank, 14, CONVERT_VREF_PERM_TO_CONF(u2TestTXVrefPerm));
            DRAMC_ENGINE_DO_TX_TEST(p, u4err_value);
            if (u4err_value != 0)
            {
                show_diag((INFO, "TX Vref H fail at perm %d, conf 0x%x\n", u2TestTXVrefPerm, CONVERT_VREF_PERM_TO_CONF(u2TestTXVrefPerm)));
                u2WinVrefPerm_H = u2TestTXVrefPerm - u2FinalTXVrefPerm - MR_VREF_STEP_PERM;
                break;
            }
        }

        // set Vref back to the Final and dump the Vref window check result
        dramc_mode_reg_write_by_rank(p, p->rank, 14, u2FinalTXVrefConf);
        show_diag((INFO, "[%dMHz][CH%d][RK%d] TX Vref Final %d, Vref Window at center, offset based on Final Vref (-%d~%d)\n", p->frequency, p->channel, p->rank,
            u2FinalTXVrefConf & 0x3F, u2WinVrefPerm_L / MR_VREF_STEP_PERM, u2WinVrefPerm_H / MR_VREF_STEP_PERM));
	}
#endif
	show_msg3((INFO, "[TxWindowPerbitCal] Done\n"));

	if (cal_type == TX_DQ_DQS_MOVE_DQM_ONLY && vref_scan_enable == 0)
		return TX_DQM_WINDOW_SPEC_IN;

	return DRAM_OK;
}

#if TX_PICG_NEW_MODE
void tx_picg_setting(DRAMC_CTX_T * p)
{
	unsigned int dqs_oen_final, dq_oen_final;
	unsigned short dqs_oen_mck[2], dqs_oen_ui[2], dqs_oen_delay[2];
	unsigned short dq_oen_mck[2], dq_oen_ui[2], dq_oen_delay[2];
	unsigned short comb_tx_sel[2];
	unsigned short shift_div[2];
	unsigned short comb_tx_picg_cnt;
	unsigned char ch_idx, rank_idx;
	unsigned char rank_bak = get_rank(p);
	unsigned char ch_bak = get_phy_2_channel_mapping(p);

	if (get_div_mode(p) == DIV4_MODE) {
		shift_div[0] = 2;
		shift_div[1] = 0;
#ifndef EIGER_E2
		comb_tx_picg_cnt = 8;
#else
		comb_tx_picg_cnt = 9;
#endif
#if TX_OE_EXTEND
		comb_tx_picg_cnt++;
#endif
	} else {
		shift_div[0] = 10;
		shift_div[1] = 6;
		comb_tx_picg_cnt = 7;
#if TX_OE_EXTEND
		comb_tx_picg_cnt++;
#endif
	}

	for (ch_idx=0; ch_idx<p->support_channel_num; ch_idx++) {
		set_phy_2_channel_mapping(p, ch_idx);

		dqs_oen_mck[0] = io_32_read_fld_align(
			DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS0),
			SHU_SELPH_DQS0_TXDLY_OEN_DQS0);
		dqs_oen_ui[0] = io_32_read_fld_align(
			DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS1),
			SHU_SELPH_DQS1_DLY_OEN_DQS0);
		dqs_oen_delay[0] = (dqs_oen_mck[0] <<3) + dqs_oen_ui[0];

		dqs_oen_mck[1] = io_32_read_fld_align(
			DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS0),
			SHU_SELPH_DQS0_TXDLY_OEN_DQS1);
		dqs_oen_ui[1] = io_32_read_fld_align(
			DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_DQS1),
			SHU_SELPH_DQS1_DLY_OEN_DQS1);
		dqs_oen_delay[1] = (dqs_oen_mck[1] <<3) + dqs_oen_ui[1];

		dqs_oen_final = (dqs_oen_delay[0] > dqs_oen_delay[1]) ?
			dqs_oen_delay[1] : dqs_oen_delay[0];

		comb_tx_sel[0] = (dqs_oen_final - shift_div[0])>>3;

		if (get_div_mode(p) == DIV4_MODE)
			comb_tx_sel[1] = 0;
		else
			comb_tx_sel[1] = (dqs_oen_final - shift_div[1])>>3;

#ifndef EIGER_E2
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_APHY_TX_PICG_CTRL),
			p_fld(1, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_OPT) |
			p_fld(1, SHU_APHY_TX_PICG_CTRL_APHYPI_CG_CK_OPT) |
			p_fld(0xc, SHU_APHY_TX_PICG_CTRL_APHYPI_CG_CK_SEL) |
			p_fld(1, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_OPT) |
			p_fld(comb_tx_sel[0], SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_SEL_P0) |
			p_fld(comb_tx_sel[1], SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_SEL_P1) |
			p_fld(comb_tx_picg_cnt, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_PICG_CNT));
		if (p->dram_type == TYPE_PCDDR3) {
			io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_APHY_TX_PICG_CTRL),
				p_fld(0, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_OPT) |
				p_fld(0, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_PICG_CNT));
		}
#else
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_APHY_TX_PICG_CTRL),
			p_fld(1, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_OPT) |
			p_fld(comb_tx_sel[0], SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P0) |
			p_fld(comb_tx_sel[1], SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQS_SEL_P1) |
			p_fld(comb_tx_picg_cnt, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_PICG_CNT));

		for (rank_idx=RANK_0; rank_idx<p->support_rank_num; rank_idx++) {
			set_rank(p, rank_idx);

			dq_oen_mck[0] = io_32_read_fld_align(
				DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ0),
				SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0);
			dq_oen_ui[0] = io_32_read_fld_align(
				DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ2),
				SHURK0_SELPH_DQ2_DLY_OEN_DQ0);
			dq_oen_delay[0] = (dq_oen_mck[0] <<3) + dq_oen_ui[0];

			dq_oen_mck[1] = io_32_read_fld_align(
				DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ0),
				SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1);
			dq_oen_ui[1] = io_32_read_fld_align(
				DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ2),
				SHURK0_SELPH_DQ2_DLY_OEN_DQ1);
			dq_oen_delay[1] = (dq_oen_mck[1] <<3) + dq_oen_ui[1];

			/* Reset Shift_Div if is DVI8_MODE */
			if (get_div_mode(p) == DIV8_MODE) {
				shift_div[0] = 8;
				shift_div[1] = 4;
			} else  {
				shift_div[0] = 0;
			}

			dqs_oen_final = (dq_oen_delay[0] > dq_oen_delay[1]) ?
				dq_oen_delay[1] : dq_oen_delay[0];

			comb_tx_sel[0] = (dqs_oen_final - shift_div[0])>>3;

			if (get_div_mode(p) == DIV4_MODE)
				comb_tx_sel[1] = 0;
			else
				comb_tx_sel[1] = (dqs_oen_final - shift_div[1])>>3;

			if (rank_idx == RANK_0) {
				io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_APHY_TX_PICG_CTRL),
					p_fld(comb_tx_sel[0], SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK0_SEL_P0) |
					p_fld(comb_tx_sel[1], SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_COMB_TX_DQ_RK0_SEL_P1));
			} else {
				io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_WODT),
					p_fld(comb_tx_sel[0], SHU_WODT_DDRPHY_CLK_EN_COMB_TX_DQ_RK1_SEL_P0) |
					p_fld(comb_tx_sel[1], SHU_WODT_DDRPHY_CLK_EN_COMB_TX_DQ_RK1_SEL_P1));
			}
		}
		set_rank(p, rank_bak);
#endif
	}
	set_phy_2_channel_mapping(p, ch_bak);
}
#endif /* TX_PICG_NEW_MODE */

#if (CONFIG_EYESCAN_LOG == 1)
extern unsigned char gTX_EYE_Scan_flag;
void Dramc_K_TX_EyeScan_Log(DRAMC_CTX_T *p)
{
    U8 byte_tmp, delay_end = 64;
    U8 ucindex, u1BitIdx, u1ByteIdx;
    U8 ii, /*backup_rank, u1PrintWinData,*/ u1vrefidx;
    PASS_WIN_DATA_T WinPerBit[DQ_DATA_WIDTH], VrefWinPerBit[DQ_DATA_WIDTH], FinalWinPerBit[DQ_DATA_WIDTH];
    U16 tx_pi_delay[4], tx_dqm_pi_delay[4],tx_pi_delay1[4], tx_dqm_pi_delay1[4];
    //U16 u2DQDelayBegin;
    U16 uiDelay;
    U16 u2VrefLevel, u2VrefBegin, u2VrefEnd, u2VrefStep, u2VrefRange;
    U32 ucdq_pi, ucdq_ui_small, ucdq_ui_large,ucdq_oen_ui_small, ucdq_oen_ui_large;
    U32 uiFinishCount;
    U16 u2TempWinSum, u2tx_window_sum=0;
    U32 u4err_value, u4fail_bit;
    #if 1//TX_DQM_CALC_MAX_MIN_CENTER
    U16 u2Center_min[DQS_NUMBER],u2Center_max[DQS_NUMBER];
    #endif

    U16 TXPerbitWin_min_max = 0;
    //U32 min_bit;
    U32 min_winsize;

    //U16 u2FinalVref=0xd;
    //U16 u2FinalRange=0;

    U8 EyeScan_index[DQ_DATA_WIDTH];

    U8 backup_u1MR14Value, backup_u1MR06Value;
    U8 u1pass_in_this_vref_flag[DQ_DATA_WIDTH];

    U32 * u4RegBackupAddress[] = {
	DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ0),
	DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ2),
	DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ1),
	DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ3),
	DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ7),
	DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ7),
    };

    if(gTX_EYE_Scan_flag==0) return;

    //backup register value
    dramc_backup_registers(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(U32));

    if (is_lp4_family(p)) {
	backup_u1MR14Value = dram_mr.mr14_value[p->channel][p->rank][p->dram_fsp];
    } else if (p->dram_type == TYPE_PCDDR4) {
	backup_u1MR06Value = dram_mr.mr06_value[p->dram_fsp];
    }

    //set initial values
    for(u1vrefidx=0; u1vrefidx<VREF_TOTAL_NUM_WITH_RANGE;u1vrefidx++)
    {
        for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            for(ii=0; ii<EYESCAN_BROKEN_NUM; ii++)
            {
                gEyeScan_Min[u1vrefidx][u1BitIdx][ii] = EYESCAN_DATA_INVALID;
                gEyeScan_Max[u1vrefidx][u1BitIdx][ii] = EYESCAN_DATA_INVALID;
            }
            gEyeScan_ContinueVrefHeight[u1BitIdx] = 0;
            gEyeScan_TotalPassCount[u1BitIdx] = 0;
        }
    }

    for(u1ByteIdx=0; u1ByteIdx < p->data_width/DQS_BIT_NUM; u1ByteIdx++)
    {
	byte_tmp = mapping_phy_byte_to_dramc(p, u1ByteIdx);
        if (u1ByteIdx == 0)
        {
            tx_pi_delay[u1ByteIdx] = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ0), SHURK0_SELPH_DQ0_TXDLY_DQ0) * 256 +
                          io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ2), SHURK0_SELPH_DQ2_DLY_DQ0) * 32;// +
                        //  io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ7), SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0);

            tx_dqm_pi_delay[u1ByteIdx] = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ1), SHURK0_SELPH_DQ1_TXDLY_DQM0) * 256 +
                              io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ3), SHURK0_SELPH_DQ3_DLY_DQM0) * 32;// +
                          //    io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ7), SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0);
        }
        else
        {
            tx_pi_delay[u1ByteIdx] = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ0), SHURK0_SELPH_DQ0_TXDLY_DQ1) * 256 +
                          io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ2), SHURK0_SELPH_DQ2_DLY_DQ1) * 32;// +
                         // io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ7), SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2);

            tx_dqm_pi_delay[u1ByteIdx] = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ1), SHURK0_SELPH_DQ1_TXDLY_DQM1) * 256 +
                              io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ3), SHURK0_SELPH_DQ3_DLY_DQM1) * 32;// +
                             // io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ7), SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2);
        }

        if (byte_tmp == 0) {
	   tx_pi_delay[u1ByteIdx] += io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ7), SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0);
           tx_dqm_pi_delay[u1ByteIdx] +=  io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ7), SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0);
        } else {
	   tx_pi_delay[u1ByteIdx] += io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ7), SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2);
	   tx_dqm_pi_delay[u1ByteIdx] += io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ7), SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2);
        }
    }

    if (tx_pi_delay[0] < tx_pi_delay[1])
    {
    //    u2DQDelayBegin = tx_pi_delay[0]-32;
    }
    else
    {
     //   u2DQDelayBegin = tx_pi_delay[1]-32;
    }

	/* Simple process for DDR4 byte swap */
    if (p->dram_type == TYPE_PCDDR4) {
	if (tx_pi_delay[0] < tx_pi_delay[1])
	{
	   tx_pi_delay[1] = tx_pi_delay[0];
	   tx_dqm_pi_delay[1] = tx_dqm_pi_delay[0];
	    //    u2DQDelayBegin = tx_pi_delay[0]-32;
	 }
	 else
	 {
	   tx_pi_delay[0] = tx_pi_delay[1];
	   tx_dqm_pi_delay[0] = tx_dqm_pi_delay[1];
	     //   u2DQDelayBegin = tx_pi_delay[1]-32;
	 }
	 delay_end = 128;
    }

   if (is_lp4_family(p))
	u2VrefRange = 0;
   else if (p->dram_type == TYPE_PCDDR4)
	u2VrefRange = 1;

    u2VrefBegin = 0;
    u2VrefEnd = 50;
    u2VrefStep = 1;

    dramc_engine2_init(p, p->test2_1, p->test2_2, p->test_pattern, 0);

    for(u2VrefLevel = u2VrefBegin; u2VrefLevel <= u2VrefEnd; u2VrefLevel += u2VrefStep)
    {
        //set vref
//fra        u1MR14Value[p->channel][p->rank][p->dram_fsp] = (u2VrefLevel | (u2VrefRange<<6));
	if (is_lp4_family(p)) {
		dramc_mode_reg_write_by_rank(p, p->rank, 14, u2VrefLevel | (u2VrefRange<<6));
	} else if (p->dram_type == TYPE_PCDDR4) {
		dramc_mode_reg_write(p, 6, 0x080 |
			dram_mr.mr06_value[p->dram_fsp]);
		delay_us(10);
		dramc_mode_reg_write(p, 6, 0x080 | u2VrefLevel | (u2VrefRange<<6) |
			dram_mr.mr06_value[p->dram_fsp]);
		delay_us(10);
		dramc_mode_reg_write(p, 6, u2VrefLevel | (u2VrefRange<<6) |
			dram_mr.mr06_value[p->dram_fsp]);
		delay_us(10);
		dramc_mode_reg_write(p, 0, dram_mr.mr00_value[p->dram_fsp]);
		delay_us(100);
	}

        // initialize parameters
        uiFinishCount = 0;
        u2TempWinSum =0;

        for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            WinPerBit[u1BitIdx].first_pass = (S16)PASS_RANGE_NA;
            WinPerBit[u1BitIdx].last_pass = (S16)PASS_RANGE_NA;
            VrefWinPerBit[u1BitIdx].first_pass = (S16)PASS_RANGE_NA;
            VrefWinPerBit[u1BitIdx].last_pass = (S16)PASS_RANGE_NA;

            gEyeScan_DelayCellPI[u1BitIdx] = 0;

            EyeScan_index[u1BitIdx] = 0;
            u1pass_in_this_vref_flag[u1BitIdx] = 0;
        }

        for (uiDelay=0; uiDelay<delay_end; uiDelay++)
        {
            tx_win_transfer_delay_to_uipi(p, tx_pi_delay[0]+uiDelay-32, 0,
	    	&ucdq_ui_large, &ucdq_ui_small, &ucdq_pi, &ucdq_oen_ui_large, &ucdq_oen_ui_small);
            io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ0), \
                                             p_fld(ucdq_ui_large, SHURK0_SELPH_DQ0_TXDLY_DQ0) | \
                                             p_fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0));
            io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ2), \
                                             p_fld(ucdq_ui_small, SHURK0_SELPH_DQ2_DLY_DQ0) | \
                                             p_fld(ucdq_oen_ui_small, SHURK0_SELPH_DQ2_DLY_OEN_DQ0));
            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ7), ucdq_pi, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0);

            tx_win_transfer_delay_to_uipi(p, tx_pi_delay[1]+uiDelay-32, 0, &ucdq_ui_large, &ucdq_ui_small, &ucdq_pi, &ucdq_oen_ui_large, &ucdq_oen_ui_small);
            io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ0), \
                                             p_fld(ucdq_ui_large, SHURK0_SELPH_DQ0_TXDLY_DQ1) | \
                                             p_fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1));
            io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ2), \
                                             p_fld(ucdq_ui_small, SHURK0_SELPH_DQ2_DLY_DQ1) | \
                                             p_fld(ucdq_oen_ui_small, SHURK0_SELPH_DQ2_DLY_OEN_DQ1));
            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ7), ucdq_pi, SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2);

            tx_win_transfer_delay_to_uipi(p, tx_dqm_pi_delay[0]+uiDelay-32, 0, &ucdq_ui_large, &ucdq_ui_small, &ucdq_pi, &ucdq_oen_ui_large, &ucdq_oen_ui_small);
            io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ1), \
                                            p_fld(ucdq_ui_large, SHURK0_SELPH_DQ1_TXDLY_DQM0) | \
                                            p_fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0));
           io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ3), \
                                            p_fld(ucdq_ui_small, SHURK0_SELPH_DQ3_DLY_DQM0) | \
                                            p_fld(ucdq_oen_ui_small, SHURK0_SELPH_DQ3_DLY_OEN_DQM0));
            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ7), ucdq_pi, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0);

            tx_win_transfer_delay_to_uipi(p, tx_dqm_pi_delay[1]+uiDelay-32, 0, &ucdq_ui_large, &ucdq_ui_small, &ucdq_pi, &ucdq_oen_ui_large, &ucdq_oen_ui_small);
            io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ1), \
                                             p_fld(ucdq_ui_large, SHURK0_SELPH_DQ1_TXDLY_DQM1) | \
                                             p_fld(ucdq_oen_ui_large, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1));
            io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQ3), \
                                             p_fld(ucdq_ui_small, SHURK0_SELPH_DQ3_DLY_DQM1) | \
                                             p_fld(ucdq_oen_ui_small, SHURK0_SELPH_DQ3_DLY_OEN_DQM1));
            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ7), ucdq_pi, SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2);


             // audio +xtalk pattern
            u4err_value=0;
            dramc_engine2_set_pat(p,TEST_AUDIO_PATTERN, 0,0);
                u4err_value = dramc_engine2_run(p, TE_OP_WRITE_READ_CHECK, TEST_AUDIO_PATTERN);
            dramc_engine2_set_pat(p,TEST_XTALK_PATTERN, 0,0);
                u4err_value |= dramc_engine2_run(p, TE_OP_WRITE_READ_CHECK, TEST_XTALK_PATTERN);

            // check fail bit ,0 ok ,others fail
            for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
            {
                u4fail_bit = u4err_value&((U32)1<<u1BitIdx);

                if (u4fail_bit == 0)
                {
                    gEyeScan_TotalPassCount[u1BitIdx]++;
                }

                if(WinPerBit[u1BitIdx].first_pass== PASS_RANGE_NA)
                {
                    if(u4fail_bit==0) //compare correct: pass
                    {
                        WinPerBit[u1BitIdx].first_pass = uiDelay;
                        u1pass_in_this_vref_flag[u1BitIdx] = 1;
                    }
                }
                else if(WinPerBit[u1BitIdx].last_pass == PASS_RANGE_NA)
                {
                    if(u4fail_bit !=0) //compare error : fail
                    {
                        WinPerBit[u1BitIdx].last_pass  = (uiDelay-1);
                    }
                    else if (uiDelay==64)
                    {
                        WinPerBit[u1BitIdx].last_pass  = uiDelay;
                    }

                    if(WinPerBit[u1BitIdx].last_pass  !=PASS_RANGE_NA)
                    {
                        if((WinPerBit[u1BitIdx].last_pass -WinPerBit[u1BitIdx].first_pass) >= (VrefWinPerBit[u1BitIdx].last_pass -VrefWinPerBit[u1BitIdx].first_pass))
                        {
                            //if window size bigger than 7, consider as real pass window. If not, don't update finish counte and won't do early break;
                            if((WinPerBit[u1BitIdx].last_pass -WinPerBit[u1BitIdx].first_pass) >7)
                                uiFinishCount |= (1<<u1BitIdx);

                            //update bigger window size
                            VrefWinPerBit[u1BitIdx].first_pass = WinPerBit[u1BitIdx].first_pass;
                            VrefWinPerBit[u1BitIdx].last_pass = WinPerBit[u1BitIdx].last_pass;
                        }

#if CONFIG_EYESCAN_LOG
                            if (EyeScan_index[u1BitIdx] < EYESCAN_BROKEN_NUM)
                            {
#if VENDER_JV_LOG || defined(RELEASE)
				if (is_lp4_family(p)) {
					gEyeScan_Min[u2VrefLevel+u2VrefRange*30][u1BitIdx][EyeScan_index[u1BitIdx]] = WinPerBit[u1BitIdx].first_pass;
					gEyeScan_Max[u2VrefLevel+u2VrefRange*30][u1BitIdx][EyeScan_index[u1BitIdx]] = WinPerBit[u1BitIdx].last_pass;
				} else if (p->dram_type == TYPE_PCDDR4) {
					gEyeScan_Min[u2VrefLevel + (1 - u2VrefRange)*23][u1BitIdx][EyeScan_index[u1BitIdx]] = WinPerBit[u1BitIdx].first_pass;
					gEyeScan_Max[u2VrefLevel + (1 - u2VrefRange)*23][u1BitIdx][EyeScan_index[u1BitIdx]] = WinPerBit[u1BitIdx].last_pass;
				}
#else
				if (is_lp4_family(p)) {
					gEyeScan_Min[u2VrefLevel+u2VrefRange*30][u1BitIdx][EyeScan_index[u1BitIdx]] = WinPerBit[u1BitIdx].first_pass + tx_pi_delay[u1BitIdx/8]-32;
					gEyeScan_Max[u2VrefLevel+u2VrefRange*30][u1BitIdx][EyeScan_index[u1BitIdx]] = WinPerBit[u1BitIdx].last_pass + tx_pi_delay[u1BitIdx/8]-32;
				} else if (p->dram_type == TYPE_PCDDR4) {
					gEyeScan_Min[u2VrefLevel + (1 - u2VrefRange)*23][u1BitIdx][EyeScan_index[u1BitIdx]] = WinPerBit[u1BitIdx].first_pass + tx_pi_delay[u1BitIdx/8]-32;
					gEyeScan_Max[u2VrefLevel + (1 - u2VrefRange)*23][u1BitIdx][EyeScan_index[u1BitIdx]] = WinPerBit[u1BitIdx].last_pass + tx_pi_delay[u1BitIdx/8]-32;
				}
#endif
                                EyeScan_index[u1BitIdx]=EyeScan_index[u1BitIdx]+1;
                            }
#endif

                        //reset tmp window
                        WinPerBit[u1BitIdx].first_pass = PASS_RANGE_NA;
                        WinPerBit[u1BitIdx].last_pass = PASS_RANGE_NA;
                    }
                 }
               }
        }

        min_winsize = 0xffff;
        //min_bit = 0xff;
        for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            VrefWinPerBit[u1BitIdx].win_size = VrefWinPerBit[u1BitIdx].last_pass- VrefWinPerBit[u1BitIdx].first_pass +(VrefWinPerBit[u1BitIdx].last_pass==VrefWinPerBit[u1BitIdx].first_pass?0:1);

            if (VrefWinPerBit[u1BitIdx].win_size < min_winsize)
            {
            //    min_bit = u1BitIdx;
                min_winsize = VrefWinPerBit[u1BitIdx].win_size;
            }

            u2TempWinSum += VrefWinPerBit[u1BitIdx].win_size;  //Sum of CA Windows for vref selection

#if (CONFIG_EYESCAN_LOG == 1)
	   if (is_lp4_family(p)) {
		gEyeScan_WinSize[u2VrefLevel+u2VrefRange*30][u1BitIdx] = VrefWinPerBit[u1BitIdx].win_size;
	   } else if (p->dram_type == TYPE_PCDDR4) {
		gEyeScan_WinSize[u2VrefLevel + (1 - u2VrefRange)*23][u1BitIdx] = VrefWinPerBit[u1BitIdx].win_size;
	   }
#endif
        }

        if ((min_winsize > TXPerbitWin_min_max) || ((min_winsize == TXPerbitWin_min_max) && (u2TempWinSum >u2tx_window_sum)))
        {
            TXPerbitWin_min_max = min_winsize;
            u2tx_window_sum =u2TempWinSum;
           // u2FinalRange = u2VrefRange;
           // u2FinalVref = u2VrefLevel;

            //Calculate the center of DQ pass window
            // Record center sum of each byte
            for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUM); u1ByteIdx++)
            {
        #if 1//TX_DQM_CALC_MAX_MIN_CENTER
                u2Center_min[u1ByteIdx] = 0xffff;
                u2Center_max[u1ByteIdx] = 0;
        #endif

                for (u1BitIdx=0; u1BitIdx<DQS_BIT_NUM; u1BitIdx++)
                {
                    ucindex = u1ByteIdx * DQS_BIT_NUM + u1BitIdx;
                    FinalWinPerBit[ucindex].first_pass = VrefWinPerBit[ucindex].first_pass;
                    FinalWinPerBit[ucindex].last_pass =  VrefWinPerBit[ucindex].last_pass;
                    FinalWinPerBit[ucindex].win_size = VrefWinPerBit[ucindex].win_size;
                    FinalWinPerBit[ucindex].win_center = (FinalWinPerBit[ucindex].first_pass + FinalWinPerBit[ucindex].last_pass) >> 1;

                    if(FinalWinPerBit[ucindex].win_center < u2Center_min[u1ByteIdx])
                        u2Center_min[u1ByteIdx] = FinalWinPerBit[ucindex].win_center;

                    if(FinalWinPerBit[ucindex].win_center > u2Center_max[u1ByteIdx])
                        u2Center_max[u1ByteIdx] = FinalWinPerBit[ucindex].win_center;
                }
            }
        }


        if(u2VrefRange == 0 && u2VrefLevel == 50 && is_lp4_family(p))
        {
            u2VrefRange = 1;
            u2VrefLevel = 20;
        } else if (u2VrefRange == 1 && u2VrefLevel == 50 && p->dram_type == TYPE_PCDDR4) {
	    u2VrefRange = 0;
            u2VrefLevel = 27;
        }

        for (u1BitIdx = 0; u1BitIdx < p->data_width; u1BitIdx++)
        {
            if (u1pass_in_this_vref_flag[u1BitIdx]) gEyeScan_ContinueVrefHeight[u1BitIdx]++;  //count pass number of continue vref
        }
    }

    dramc_engine2_end(p);

    //Calculate the center of DQ pass window
    //average the center delay
    for (u1ByteIdx=0; u1ByteIdx<(p->data_width/DQS_BIT_NUM); u1ByteIdx++)
    {
        uiDelay = ((u2Center_min[u1ByteIdx] + u2Center_max[u1ByteIdx])>>1); //(max +min)/2

#if (CONFIG_EYESCAN_LOG == 1)
#if VENDER_JV_LOG || defined(RELEASE)
        gEyeScan_CaliDelay[u1ByteIdx] = uiDelay;
#else
        gEyeScan_CaliDelay[u1ByteIdx] = uiDelay + tx_pi_delay[u1ByteIdx]-32;
#endif
#endif
    }

    //restore to orignal value
    dramc_restore_registers(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(U32));

    if (is_lp4_family(p)) {
	dramc_mode_reg_write_by_rank(p, p->rank, 14, backup_u1MR14Value);
	dram_mr.mr14_value[p->channel][p->rank][p->dram_fsp] = backup_u1MR14Value;
    } else if (p->dram_type == TYPE_PCDDR4) {
	dram_mr.mr06_value[p->dram_fsp] = backup_u1MR06Value;
	dramc_mode_reg_write_by_rank(p, p->rank, MR06,
		dram_mr.mr06_value[p->dram_fsp] | 0x80);
	delay_us(100);

	dramc_mode_reg_write_by_rank(p, p->rank, MR06,
		dram_mr.mr06_value[p->dram_fsp] | 0x80 );
	delay_us(100);

	dramc_mode_reg_write_by_rank(p, p->rank, MR06,
		dram_mr.mr06_value[p->dram_fsp]);
	delay_us(100);

    }

}
#endif
#endif /* SIMULATION_TX_PERBIT */

