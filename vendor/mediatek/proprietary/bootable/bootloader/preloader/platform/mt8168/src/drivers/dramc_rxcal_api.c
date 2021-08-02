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
 * @file dramc_crxcal_api.c
 *  Basic DRAMC calibration API implementation
 */

/* -----------------------------------------------------------------------------
 *  Include files
 * -----------------------------------------------------------------------------
 */
#include "dramc_common.h"
#include "x_hal_io.h"
#include "dramc_api.h"

#define VALUE_5555	0x5555

/* -----------------------------------------------------------------------------
 *  Global variables
 * -----------------------------------------------------------------------------
 */
#if SIMULATION_RX_PERBIT
unsigned char rx_eye_scan_flag = FALSE;
unsigned char rx_eye_scan_only_higheset_freq_flag = 1;
static unsigned short rx_window_sum[CHANNEL_NUM];
static unsigned short rx_perbit_win_min_max = 0;
static unsigned short rx_perbit_win_min_max_idx = 0;
short rx_dqs_duty_offset[CHANNEL_NUM][DQS_NUMBER][EDGE_NUMBER];
#endif
unsigned char final_rx_vref_dq[CHANNEL_NUM][RANK_MAX];
#if SIMULATION_DATLAT
static unsigned char rx_datlat_result[CHANNEL_NUM][RANK_MAX];
#endif

#if (CONFIG_FOR_HQA_TEST_USED == 1) || (CONFIG_EYESCAN_LOG == 1)
static U32 g_use_test_engine;
#endif

#if (CONFIG_EYESCAN_LOG == 1)
static U32 g_rx_vref;
static U32 g_delay_step;
#endif
extern unsigned char gRX_EYE_Scan_flag;

#if (ENABLE_READ_DBI == 1)
static void set_dram_mode_reg_for_read_dbi(DRAMC_CTX_T *p,
	DRAM_FAST_SWITH_POINT_T fsp, DRAM_DBI_MODE_T on_off)
{
	dram_mr.mr03_value[fsp] &= 0xbfU;
	dram_mr.mr03_value[fsp] |= ((U8)on_off << 6U);
	dramc_mode_reg_write_by_rank(p, p->rank, MR03, dram_mr.mr03_value[fsp]);
}

void enable_dram_mode_reg_for_read_dbi_after_cal(DRAMC_CTX_T *p)
{
	U32 ch_idx, rk_idx;
	U32 ch_backup, rk_backup;
	DRAM_FAST_SWITH_POINT_T fsp_idx;

	ch_backup = p->channel;
	rk_backup = p->rank;

	for (ch_idx = CHANNEL_A; ch_idx < p->support_channel_num; ch_idx++) {
		set_phy_2_channel_mapping(p, ch_idx);

		for (rk_idx = RANK_0; rk_idx < p->support_rank_num; rk_idx++) {
			set_rank(p, rk_idx);
			for (fsp_idx = FSP_0; fsp_idx < FSP_MAX; fsp_idx++) {
				if (fsp_idx == FSP_0) {
					dram_mr.mr13_value[fsp_idx] &= ~0x40;
				} else {
					dram_mr.mr13_value[fsp_idx] |= 0x40;
				}
				dramc_mode_reg_write_by_rank(p, rk_idx, MR13,
					dram_mr.mr13_value[fsp_idx]);
				set_dram_mode_reg_for_read_dbi(p, fsp_idx,
					p->dbi_r_onoff[fsp_idx]);
			}
		}
	}

	set_rank(p, rk_backup);
	set_phy_2_channel_mapping(p, ch_backup);
}
#endif

#if SIMULATION_RX_OFFSET
static unsigned char rx_input_buffer_delay_exchange(signed char offset)
{
	unsigned char ret;

	if (offset < 0) {
		ret = 0x8 | (-offset);
	} else {
		ret = offset;
	}

	return ret;
}

static void rx_set_dq_dqm_offset(DRAMC_CTX_T *p, unsigned char dqm_offset[],
	unsigned char dq_offset[])
{
	unsigned char bit_idx, byte_idx;
	unsigned int dq_offset_set[DQS_NUMBER] = {0};

	for (byte_idx = 0; byte_idx < p->data_width/DQS_BIT_NUM;
		byte_idx += 2) {
		unsigned char bit_start;
		DRAM_CHANNEL_T backup_channel;

		backup_channel = p->channel;
		if (p->data_width == DATA_WIDTH_32BIT)
			p->channel = CHANNEL_B;

		bit_start = byte_idx * DQS_BIT_NUM;
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ1),
			dqm_offset[byte_idx], B0_DQ1_RG_RX_ARDQM0_OFFC_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B1_DQ1),
			dqm_offset[byte_idx + 1], B1_DQ1_RG_RX_ARDQM0_OFFC_B1);

		for (bit_idx = bit_start; bit_idx < bit_start + DQS_BIT_NUM;
			bit_idx++) {
			unsigned char shift;

			shift = bit_idx - bit_start;

			dq_offset_set[byte_idx] |=
				dq_offset[bit_idx] << (shift * 4);
			dq_offset_set[byte_idx+1] |=
				dq_offset[bit_idx + 8] << (shift * 4);
		}

		io32_write_4b(DRAMC_REG_ADDR(DDRPHY_B0_DQ0),
			dq_offset_set[byte_idx]);
		io32_write_4b(DRAMC_REG_ADDR(DDRPHY_B1_DQ0),
			dq_offset_set[byte_idx + 1]);

		p->channel = backup_channel;
	}
}

DRAM_STATUS_T dramc_rx_input_buffer_offset_cal(DRAMC_CTX_T *p)
{
	signed char offset;
	unsigned char dqm_offset[DQS_NUMBER];
	unsigned char dq_offset[DQ_DATA_WIDTH];
	unsigned char dq_flag_change[DQ_DATA_WIDTH];
	unsigned char dqm_flag_change[DQS_NUMBER];
	unsigned int dq_result;
	unsigned int dqm_result;
	unsigned char bit_idx, byte_idx, finish_count;

	unsigned int reg_backup_address[] = {
		DDRPHY_B0_DQ3,
		DDRPHY_B1_DQ3,
		DDRPHY_B0_DQ5,
		DDRPHY_B1_DQ5,
		DDRPHY_B0_DQ6,
		DDRPHY_B1_DQ6,

		DDRPHY_B0_DQ3 + (1 << POS_BANK_NUM),
		DDRPHY_B1_DQ3 + (1 << POS_BANK_NUM),
		DDRPHY_B0_DQ5 + (1 << POS_BANK_NUM),
		DDRPHY_B1_DQ5 + (1 << POS_BANK_NUM),
		DDRPHY_B0_DQ6 + (1 << POS_BANK_NUM),
		DDRPHY_B1_DQ6 + (1 << POS_BANK_NUM),
	};

	show_msg_with_timestamp((INFO, "start Rx InputBuffer calibration\n"));

	dramc_backup_registers(p, reg_backup_address,
		sizeof(reg_backup_address) / sizeof(unsigned int));

	io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B0_DQ6),
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_PS_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B0));
	io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B1_DQ6),
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_PS_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B1));

	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), SET_FLD,
		B0_DQ5_RG_RX_ARDQ_VREF_EN_B0);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B1_DQ5), SET_FLD,
		B1_DQ5_RG_RX_ARDQ_VREF_EN_B1);

	delay_us(1);

	io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B0_DQ3),
		p_fld(1, B0_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B0) |
		p_fld(1, B0_DQ3_RG_RX_ARDQ_OFFC_EN_B0));
	io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B1_DQ3),
		p_fld(1, B1_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B1) |
		p_fld(1, B1_DQ3_RG_RX_ARDQ_OFFC_EN_B1));

	finish_count = 0;
	for (bit_idx = 0; bit_idx < DQ_DATA_WIDTH; bit_idx++) {
		dq_flag_change[bit_idx] = 0x7f;
	}

	for (byte_idx = 0; byte_idx < DQS_NUMBER; byte_idx++) {
		dqm_flag_change[byte_idx] = 0x7f;
	}

	for (offset = -7; offset < 8; offset++)
	{
		unsigned char convert_offset;

		convert_offset = rx_input_buffer_delay_exchange(offset);

		show_msg2((INFO, "offset = %2d, convert_offset = %2d,",
			offset, convert_offset));

		/* Adjust dq & dqm offset value */
		memset(dqm_offset, convert_offset, DQS_NUMBER);
		memset(dq_offset, convert_offset, DQ_DATA_WIDTH);
		rx_set_dq_dqm_offset(p, dqm_offset, dq_offset);

		delay_us(1);

		/* Check Offset Flag */
		dq_result = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_PHY_RGS0),
			MISC_PHY_RGS0_RGS_ARDQ_OFFSET_FLAG);
		dqm_result = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_PHY_RGS0),
			MISC_PHY_RGS0_RGS_ARDQM0_OFFSET_FLAG);
		dqm_result |= io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_PHY_RGS0),
			MISC_PHY_RGS0_RGS_ARDQM1_OFFSET_FLAG) << 1;

		if (p->data_width == DATA_WIDTH_32BIT) {
			unsigned int b23_result;

			b23_result = io_32_read_fld_align(DDRPHY_MISC_PHY_RGS0 +
				(CHANNEL_B << POS_BANK_NUM),
				MISC_PHY_RGS0_RGS_ARDQ_OFFSET_FLAG);
			dqm_result |= io_32_read_fld_align(DDRPHY_MISC_PHY_RGS0 +
				(CHANNEL_B << POS_BANK_NUM),
				MISC_PHY_RGS0_RGS_ARDQM0_OFFSET_FLAG) << 2;
			dqm_result |= io_32_read_fld_align(DDRPHY_MISC_PHY_RGS0 +
				(CHANNEL_B << POS_BANK_NUM),
				MISC_PHY_RGS0_RGS_ARDQM1_OFFSET_FLAG) << 3;

			dq_result |= (b23_result << 16);
		}
		show_msg2((INFO, "dq_result = 0x%x\n", dq_result));
		show_msg2((INFO, "dqm_result = 0x%x\n", dqm_result));

		for (bit_idx = 0; bit_idx < p->data_width; bit_idx++) {
			if (dq_flag_change[bit_idx] == 0x7f) {
				unsigned char result;

				result = (dq_result >> bit_idx) & 0x1;

				if (result == 0) {
					dq_flag_change[bit_idx] = convert_offset;
					finish_count++;
				}
			}
		}

		for (byte_idx = 0; byte_idx < (p->data_width/DQS_BIT_NUM);
			byte_idx++) {
			if (dqm_flag_change[byte_idx] == 0x7f) {
				if ((dqm_result & (1 << byte_idx)) == 0) {
					dqm_flag_change[byte_idx]= convert_offset;
					finish_count++;
				}
			}
		}

		if (finish_count == (p->data_width + p->data_width/DQS_BIT_NUM)) {
			show_msg2((INFO, "%d bits finished. Early break\n",
				finish_count));
			break;
		}
	}

	/* log the result */
	show_msg2((INFO, "Final offset result:\n"));
	for (byte_idx = 0; byte_idx < (p->data_width/DQS_BIT_NUM); byte_idx++) {
		unsigned char bit_start;

		show_msg2((INFO, "DQM %d: %d\n", byte_idx,
			dqm_flag_change[byte_idx]));

		bit_start = byte_idx * DQS_BIT_NUM;
		for (bit_idx = bit_start; bit_idx < (bit_start + DQS_BIT_NUM);
			bit_idx++) {
			show_msg2((INFO, "DQ %d: %d; ", bit_idx,
				dq_flag_change[bit_idx]));

			if ((bit_idx % 4) == 3)
				show_msg2((INFO, "\n"));
		}
		show_msg2((INFO, "\n\n"));
	}

	dramc_restore_registers(p, reg_backup_address,
		sizeof(reg_backup_address) / sizeof(unsigned int));

	rx_set_dq_dqm_offset(p, dqm_flag_change, dq_flag_change);

	show_msg2((INFO, "[RxInputBuffer calibration] Done\n"));
	return DRAM_OK;
}
#endif

#if SIMULATION_RX_PERBIT
/* dramc_rx_window_perbit_cal (v2 version)
 *  start the rx dqs perbit sw calibration.
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @retval status          (DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
/* default RX vref is 0xe=14 */
#if SUPPORT_TYPE_LPDDR4
static void dramc_rx_win_rddqc_init_lp4(DRAMC_CTX_T *p)
{
	//unsigned char *lpddr_phy_mapping;
	//unsigned short temp_value = 0;
	unsigned char mr_golden_mr15_golden_value = 0;
	unsigned char mr_golden_mr20_golden_value = 0;
	int i;

	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ7), CLEAR_FLD,
		SHU_B0_DQ7_R_DMDQMDBI_SHU_B0);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B1_DQ7), CLEAR_FLD,
		SHU_B1_DQ7_R_DMDQMDBI_SHU_B1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ7), CLEAR_FLD,
		SHU_B2_DQ7_R_DMDQMDBI_SHU_B2);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), get_rank(p),
		MRS_MRSRK);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION), SET_FLD,
		MPC_OPTION_MPCRKEN);

#if 0
	lpddr_phy_mapping =
		(unsigned char *)lpddr4_phy_mapping_pop[p->channel];

	for (i = 0; i < 16; i++) {
		temp_value |= ((VALUE_5555 >> i) & 0x1) << lpddr_phy_mapping[i];
	}

	mr_golden_mr15_golden_value =
		(unsigned char) temp_value & BYTE_MAX;
	mr_golden_mr20_golden_value =
		(unsigned char) (temp_value >> 8) & BYTE_MAX;
#else
	mr_golden_mr15_golden_value = VALUE_5555 & BYTE_MAX;
	mr_golden_mr20_golden_value = (VALUE_5555 >> 8) & BYTE_MAX;
#endif

	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_MR_GOLDEN),
		p_fld(mr_golden_mr15_golden_value,
		MR_GOLDEN_MR15_GOLDEN) |
		p_fld(mr_golden_mr20_golden_value,
		MR_GOLDEN_MR20_GOLDEN));
}

/* Issue "RD DQ Calibration"
 * 1. RDDQCEN = 1 for RDDQC
 * 2. RDDQCDIS = 1 to stop RDDQC burst
 * 3. Wait rddqc_response = 1
 * 4. Read compare result
 * 5. RDDQCEN = 0
 * 6. RDDQCDIS = 0 (Stops RDDQC request)
 */
static unsigned int dramc_rx_win_rddqc_run_lp4(DRAMC_CTX_T *p)
{
	unsigned int result, response;
	unsigned int time_cnt = TIME_OUT_CNT;

	/*
	* Issue RD DQ calibration
	*  to stop RDDQC burst
	* Wait rddqc_response=1
	*/
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL), SET_FLD,
		SPCMDCTRL_RDDQCDIS);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD),
		SET_FLD, SPCMD_RDDQCEN);
	do {
		response = io_32_read_fld_align(DRAMC_REG_ADDR
			(DRAMC_REG_SPCMDRESP), SPCMDRESP_RDDQC_RESPONSE);
		time_cnt--;
		delay_us(1);
	} while ((response == 0) && (time_cnt > 0));

	if (time_cnt == 0)
		show_msg((INFO, "[RxWinRDDQC] Resp fail (time out)\n"));

	result = io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_RDQC_CMP));

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), CLEAR_FLD,
		SPCMD_RDDQCEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL), CLEAR_FLD,
		SPCMDCTRL_RDDQCDIS);

	return result;
}

static void dramc_rx_win_rddqc_end_lp4(DRAMC_CTX_T *p)
{
	/*  Recover MPC Rank */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), CLEAR_FLD,
		MRS_MRSRK);
}
#endif

#if SUPPORT_TYPE_PCDDR4
#define RDDQC_DQ_GOLDEN			0xaa

void  dramc_rx_win_rddqc_init_ddr4(DRAMC_CTX_T *p)
{
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 0,
		DRAMC_PD_CTRL_PHYCLKDYNGEN);
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
		p_fld(SET_FLD, CKECTRL_CKEFIXON) |
		p_fld(CLEAR_FLD, CKECTRL_CKE1FIXON));
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ7), 0,
		SHU_B0_DQ7_R_DMDQMDBI_SHU_B0);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B1_DQ7), 0,
		SHU_B1_DQ7_R_DMDQMDBI_SHU_B1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL),
		1, SPCMDCTRL_RDDQCDIS);

	/* Enable MPR. Serial mode, from MPR0 (default: 01010101B) */
	dramc_mode_reg_write(p, 3, 0x4);
}

#define GET_BIT(_v, _n)		(((_v) >> (_n)) & 0x1)
static unsigned int dramc_rx_win_rddqc_run_ddr4(DRAMC_CTX_T *p)
{
	unsigned int mrrdata0, mrrdata1, mrrdata2, mrrdata3;
	unsigned int  result, response;
	unsigned int bit_idx;
	unsigned int timecnt = TIME_OUT_CNT;

	/* Read MPR0 */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
		0, MRS_MRSBA);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD),
		1, SPCMD_MRREN);

	do {
		response = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP),
			SPCMDRESP_MRR_RESPONSE);
		timecnt --;
		delay_us(1);
	} while ((response == 0) && (timecnt > 0));

	if (timecnt == 0) {
		show_msg2((INFO, "[DramcRxWinRDDQC] response fail (time out)\n"));
		return WORD_MAX;
	}

	mrrdata0 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRRDATA0),
		MRRDATA0_MRR_DATA0);
	mrrdata1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRRDATA1),
		MRRDATA1_MRR_DATA1);
	mrrdata2 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRRDATA2),
		MRRDATA2_MRR_DATA2);
	mrrdata3 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRRDATA3),
		MRRDATA3_MRR_DATA3);

	//show_msg2((INFO, "[DramcRxWinRDDQC] MRRDATA0~3: %x %x %x %x\n", mrrdata0, mrrdata1, mrrdata2, mrrdata3));

	result = 0;

	/* Check per-bit result */
	for (bit_idx = 0; bit_idx < p->data_width; bit_idx++) {
		unsigned char dq_data;

		dq_data = GET_BIT(mrrdata0, bit_idx) << 0 |
			GET_BIT(mrrdata0, bit_idx+16) << 1 |
			GET_BIT(mrrdata1, bit_idx) << 2 |
			GET_BIT(mrrdata1, bit_idx+16) << 3 |
			GET_BIT(mrrdata2, bit_idx) << 4 |
			GET_BIT(mrrdata2, bit_idx+16) << 5 |
			GET_BIT(mrrdata3, bit_idx) << 6 |
			GET_BIT(mrrdata3, bit_idx+16) << 7;

		if (dq_data != RDDQC_DQ_GOLDEN) {
			result |= (1 << bit_idx);
		}
	}

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), 0, SPCMD_MRREN);

	return result;
}

void  dramc_rx_win_rddqc_end_ddr4(DRAMC_CTX_T *p)
{
	/*
	 * Recover Read DBI
	 * io_32_write_fld_align((DDRPHY_SHU_B0_DQ7),  p->DBI_R_onoff, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0);
	 * io_32_write_fld_align((DDRPHY_SHU_B1_DQ7),  p->DBI_R_onoff, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1);
	 */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL), CLEAR_FLD,
		SPCMDCTRL_RDDQCDIS);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), 0, MRS_MPCRK);
	dramc_mode_reg_write(p, 3, 0x0);
}
#endif

static inline void dramc_rx_win_rddqc_init(DRAMC_CTX_T *p)
{
	if (is_lp4_family(p)) {
	#if SUPPORT_TYPE_LPDDR4
		dramc_rx_win_rddqc_init_lp4(p);
	#endif
	} else if (p->dram_type == TYPE_PCDDR4) {
	#if SUPPORT_TYPE_PCDDR4
		dramc_rx_win_rddqc_init_ddr4(p);
	#endif
	}
}

/* Issue "RD DQ Calibration"
 * 1. RDDQCEN = 1 for RDDQC
 * 2. RDDQCDIS = 1 to stop RDDQC burst
 * 3. Wait rddqc_response = 1
 * 4. Read compare result
 * 5. RDDQCEN = 0
 * 6. RDDQCDIS = 0 (Stops RDDQC request)
 */
static inline unsigned int dramc_rx_win_rddqc_run(DRAMC_CTX_T *p)
{
	if (is_lp4_family(p)) {
	#if SUPPORT_TYPE_LPDDR4
		return dramc_rx_win_rddqc_run_lp4(p);
	#endif
	} else if (p->dram_type == TYPE_PCDDR4) {
	#if SUPPORT_TYPE_PCDDR4
		return dramc_rx_win_rddqc_run_ddr4(p);
	#endif
	}
}


static inline void dramc_rx_win_rddqc_end(DRAMC_CTX_T *p)
{
	if (is_lp4_family(p)) {
	#if SUPPORT_TYPE_LPDDR4
		dramc_rx_win_rddqc_end_lp4(p);
	#endif
	} else if (p->dram_type == TYPE_PCDDR4) {
	#if SUPPORT_TYPE_PCDDR4
		dramc_rx_win_rddqc_end_ddr4(p);
	#endif
	}
}


static void set_dq_output_delay_lp4(DRAMC_CTX_T *p, unsigned int value)
{
	unsigned char ii;
	/* every 2bit dq have the same delay register address */
	for (ii = 0; ii < 4; ii++) {
		io32_write_4b(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ2
			+ ii * 4), value);	/* DQ0~DQ7 */
		io32_write_4b(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ2
			+ ii * 4), value);	/* DQ8~DQ15 */
		if (p->data_width == DATA_WIDTH_32BIT) {
			io32_write_4b((DDRPHY_SHU_R0_B0_DQ2 + SHIFT_TO_CHB_ADDR
				+ ii * 4), value);	/* DQ0~DQ7 */
			io32_write_4b((DDRPHY_SHU_R0_B2_DQ2 + SHIFT_TO_CHB_ADDR
				+ ii * 4), value);	/* DQ8~DQ15 */
		}
	}
}

static void set_dqm_output_delay_lp4(DRAMC_CTX_T *p, signed short delay)
{
	/*  Adjust DQM output delay */
	io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ6),
		p_fld(delay, SHU_R0_B0_DQ6_RK0_RX_ARDQM0_R_DLY_B0)
		| p_fld(delay, SHU_R0_B0_DQ6_RK0_RX_ARDQM0_F_DLY_B0));
	io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ6),
		p_fld(delay, SHU_R0_B2_DQ6_RK0_RX_ARDQM0_R_DLY_B2)
		| p_fld(delay, SHU_R0_B2_DQ6_RK0_RX_ARDQM0_F_DLY_B2));

	if (p->data_width == DATA_WIDTH_32BIT) {
		io_32_write_fld_multi((DDRPHY_SHU_R0_B0_DQ6 + SHIFT_TO_CHB_ADDR),
			p_fld(delay, SHU_R0_B0_DQ6_RK0_RX_ARDQM0_R_DLY_B0)
			| p_fld(delay, SHU_R0_B0_DQ6_RK0_RX_ARDQM0_F_DLY_B0));
		io_32_write_fld_multi((DDRPHY_SHU_R0_B2_DQ6 + SHIFT_TO_CHB_ADDR),
			p_fld(delay, SHU_R0_B2_DQ6_RK0_RX_ARDQM0_R_DLY_B2)
			| p_fld(delay, SHU_R0_B2_DQ6_RK0_RX_ARDQM0_F_DLY_B2));
	}
}

static void set_rx_dq_dqs_delay(DRAMC_CTX_T *p, signed short delay)
{
	unsigned int value;

	if (delay <= 0) {
		/*  Set DQS delay */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ6),
			p_fld((-delay + rx_dqs_duty_offset[p->channel][0][0]),
			SHU_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0) |
			p_fld((-delay + rx_dqs_duty_offset[p->channel][0][1]),
			SHU_R0_B0_DQ6_RK0_RX_ARDQS0_F_DLY_B0));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ6),
			p_fld((-delay + rx_dqs_duty_offset[p->channel][1][0]),
			SHU_R0_B2_DQ6_RK0_RX_ARDQS0_R_DLY_B2) |
			p_fld((-delay + rx_dqs_duty_offset[p->channel][1][1]),
			SHU_R0_B2_DQ6_RK0_RX_ARDQS0_F_DLY_B2));

		if (p->data_width == DATA_WIDTH_32BIT) {
			io_32_write_fld_multi((DDRPHY_SHU_R0_B0_DQ6 + SHIFT_TO_CHB_ADDR),
				p_fld((-delay + rx_dqs_duty_offset[p->channel][0][0]),
				SHU_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0) |
				p_fld((-delay + rx_dqs_duty_offset[p->channel][0][1]),
				SHU_R0_B0_DQ6_RK0_RX_ARDQS0_F_DLY_B0));
			io_32_write_fld_multi((DDRPHY_SHU_R0_B2_DQ6 + SHIFT_TO_CHB_ADDR),
				p_fld((-delay + rx_dqs_duty_offset[p->channel][1][0]),
				SHU_R0_B2_DQ6_RK0_RX_ARDQS0_R_DLY_B2) |
				p_fld((-delay + rx_dqs_duty_offset[p->channel][1][1]),
				SHU_R0_B2_DQ6_RK0_RX_ARDQS0_F_DLY_B2));
		}

		dram_phy_reset(p);
	} else {
		/*  Adjust DQM output delay. */
		set_dqm_output_delay_lp4(p, delay);

		dram_phy_reset(p);

		/*  Adjust DQ output delay. */
		value = ((unsigned int) delay) | (((unsigned int) delay) << 8)
			| (((unsigned int) delay) << 16)
			| (((unsigned int) delay) << 24);

		set_dq_output_delay_lp4(p, value);
	}
}

unsigned char rx_eye_scan_flag_lp4(DRAMC_CTX_T *p,
	unsigned char vref_scan_enable)
{
	if ((vref_scan_enable == 0) || ((rx_eye_scan_flag == 1) &&
		(((rx_eye_scan_only_higheset_freq_flag == 1)  &&
		(p->frequency ==  dfs_get_highest_freq(p))) ||
		(rx_eye_scan_only_higheset_freq_flag == 0))))
		return 1;
	else
		return 0;
}

static void set_dq_delay_lp4(DRAMC_CTX_T *p, unsigned char bit_idx,
	signed short delay1, signed short delay2,
	signed short delay3, signed short delay4)
{
	unsigned int bak_channel;

	bak_channel = p->channel;

	/* For data width = 32, shall write the the delay
	 * value to B2, B3, that is virtual CHANNEL_B.
	 */
	if ((p->data_width == DATA_WIDTH_32BIT) &&
		(bit_idx > 15)) {
		p->channel = CHANNEL_B;

		bit_idx -= 16;
	}

	/*  set dq delay */
	io_32_write_fld_multi(DRAMC_REG_ADDR
		(DDRPHY_SHU_R0_B0_DQ2 + bit_idx * 2),
		p_fld(((unsigned int)delay1),
		SHU_R0_B0_DQ2_RK0_RX_ARDQ0_R_DLY_B0) |
		p_fld(((unsigned int)delay1),
		SHU_R0_B0_DQ2_RK0_RX_ARDQ0_F_DLY_B0) |
		p_fld(((unsigned int)delay2),
		SHU_R0_B0_DQ2_RK0_RX_ARDQ1_R_DLY_B0) |
		p_fld(((unsigned int)delay2),
		SHU_R0_B0_DQ2_RK0_RX_ARDQ1_F_DLY_B0));

	io_32_write_fld_multi(DRAMC_REG_ADDR
		(DDRPHY_SHU_R0_B2_DQ2 + bit_idx * 2),
		p_fld(((unsigned int)delay3),
		SHU_R0_B2_DQ2_RK0_RX_ARDQ0_R_DLY_B2) |
		p_fld(((unsigned int)delay3),
		SHU_R0_B2_DQ2_RK0_RX_ARDQ0_F_DLY_B2) |
		p_fld(((unsigned int)delay4),
		SHU_R0_B2_DQ2_RK0_RX_ARDQ1_R_DLY_B2) |
		p_fld(((unsigned int)delay4),
		SHU_R0_B2_DQ2_RK0_RX_ARDQ1_F_DLY_B2));

	p->channel = bak_channel;
}

static unsigned char dramc_rx_window_perbit_ini(DRAMC_CTX_T *p,
	unsigned char use_test_engine)
{
	unsigned char vref_scan_enable;

	print_calibration_basic_info(p);
	if (use_test_engine)
		print_calibration_basic_info_diag(p);
	show_msg3((INFO, "Start DQ dly to find pass range UseTestEngine =%d\n",
		use_test_engine));

	/*  for loop, different Vref, */
	rx_window_sum[p->channel] = 0;
	rx_perbit_win_min_max = 0;
	rx_perbit_win_min_max_idx = 0;
	/* only apply RX Vref Scan for Rank 0 */

	/*(Rank 0 and 1 use the same Vref reg) */
	if ((use_test_engine == 1) && (p->enable_rx_scan_vref == ENABLE) &&
		(((p->rank == RANK_0) && rx_eye_scan_flag == 0) ||
		(rx_eye_scan_flag == 1 &&
		((rx_eye_scan_only_higheset_freq_flag == 1 &&
		p->frequency == dfs_get_highest_freq(p)) ||
		rx_eye_scan_only_higheset_freq_flag == 0))))
		vref_scan_enable = 1;
	else
		vref_scan_enable = 0;

	return vref_scan_enable;
}

static unsigned int dramc_rx_win_check(DRAMC_CTX_T *p,
	PASS_WIN_DATA_T final_bitwin[], PASS_WIN_DATA_T win_per_bit[],
	signed short delay, unsigned int err_value, unsigned int finish_count,
	unsigned char vref_scan_enable, signed short delay_end)
{
	unsigned char bit_idx;
	unsigned int fail_bit = BYTE_MAX;

	/*  check fail bit ,0 ok ,others fail */
	for (bit_idx = 0; bit_idx < p->data_width; bit_idx++) {
		fail_bit = err_value & ((unsigned int)1 << bit_idx);
#if (CONFIG_EYESCAN_LOG == 1)
		if (((p->dram_type == TYPE_PCDDR4)|| is_lp4_family(p)) && !fail_bit) {
			gu1pass_in_this_vref_flag[bit_idx]=1;
			gEyeScan_TotalPassCount[bit_idx] += g_delay_step;
		}
#endif

		if (win_per_bit[bit_idx].first_pass == PASS_RANGE_NA) {
			if (fail_bit == 0)
				win_per_bit[bit_idx].first_pass = delay;
		} else if (win_per_bit[bit_idx].last_pass == PASS_RANGE_NA) {
		#if (FOR_DV_SIMULATION_USED == 1)
			/* cc notes, during Simulation, the compare result
			 * maybe 'X' in waveform, but recognized as '0' by SW,
			 * which causes fake window found. Workaround method...
			 */
			if (delay /*- win_per_bit[bit_idx].first_pass > 32*/> 59) {
				fail_bit = 1;
			}
		#endif

			if (fail_bit != 0)
				win_per_bit[bit_idx].last_pass = (delay - 1);
			else if (delay == delay_end)
				win_per_bit[bit_idx].last_pass = delay;

			if (win_per_bit[bit_idx].last_pass != PASS_RANGE_NA) {
				if ((win_per_bit[bit_idx].last_pass -
					win_per_bit[bit_idx].first_pass)
					>= (final_bitwin[bit_idx].last_pass -
					final_bitwin[bit_idx].first_pass)) {
					if ((win_per_bit[bit_idx].last_pass -
					     win_per_bit[bit_idx].first_pass)
					    > 7)
						finish_count |= (1 << bit_idx);

					/* update bigger window size */
					final_bitwin[bit_idx].first_pass =
					    win_per_bit[bit_idx].first_pass;
					final_bitwin[bit_idx].last_pass =
					    win_per_bit[bit_idx].last_pass;
				}
#if (CONFIG_EYESCAN_LOG == 1)
				if(g_use_test_engine && ((p->dram_type == TYPE_PCDDR4)|| is_lp4_family(p)))
				{
				    if (gEyeScan_index[bit_idx] < EYESCAN_BROKEN_NUM)
				    {
					gEyeScan_Min[g_rx_vref][bit_idx][gEyeScan_index[bit_idx]] =
							final_bitwin[bit_idx].first_pass;
					gEyeScan_Max[g_rx_vref][bit_idx][gEyeScan_index[bit_idx]] =
							final_bitwin[bit_idx].last_pass;
					mcSHOW_DBG_MSG(("u2VrefLevel=%d, u1BitIdx=%d, index=%d (%d, %d)==\n",
							g_rx_vref, bit_idx, gEyeScan_index[bit_idx],
							gEyeScan_Min[g_rx_vref][bit_idx][gEyeScan_index[bit_idx]],
							gEyeScan_Max[g_rx_vref][bit_idx][gEyeScan_index[bit_idx]]));
					gEyeScan_index[bit_idx]=gEyeScan_index[bit_idx]+1;
				    }
				}
#endif

				/* reset tmp window */
				win_per_bit[bit_idx].first_pass =
					PASS_RANGE_NA;
				win_per_bit[bit_idx].last_pass =
					PASS_RANGE_NA;
			}
		}

	#if CALIBRATION_LOG
		if (rx_eye_scan_flag_lp4(p, vref_scan_enable)) {
			if (bit_idx % DQS_BIT_NUM == 0)
				show_msg2((INFO, " "));
			if (fail_bit == 0)
				show_msg2((INFO, "o"));
			else
				show_msg2((INFO, "x"));
		}
	#endif
	}

#if CALIBRATION_LOG
	if (rx_eye_scan_flag_lp4(p, vref_scan_enable)) {
		show_msg2((INFO, " [MSB]\n"));
	}
#endif
	return finish_count;
}

static unsigned short dramc_rx_window_perbit_cal_process(DRAMC_CTX_T *p,
	unsigned char vref_scan_enable, PASS_WIN_DATA_T final_bitwin[],
	unsigned char use_test_engine, unsigned short vref_begin,
	unsigned short vref_end, unsigned short vref_step,
	signed short delay_begin, signed short delay_end,
	signed short delay_step)
{
	unsigned char bit_idx;
	unsigned int finish_count;
	unsigned short temp_win_sum;
	unsigned int min_bit;
	unsigned int min_winsize;
	PASS_WIN_DATA_T win_per_bit[DQ_DATA_WIDTH];
	PASS_WIN_DATA_T best_vref_win_per_bit[DQ_DATA_WIDTH];
	signed short delay = 0;
	unsigned int err_value;
	unsigned short vref, final_vref = 0xe;

	for (vref = vref_begin; vref <= vref_end; vref += vref_step) {
		if (vref_scan_enable == 1) {
			if (p->dram_type == TYPE_PCDDR4) {
				io_32_write_fld_multi_all(DDRPHY_B0_DQ6,
					p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0) |
					p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_DDR4_SEL_B0));
				io_32_write_fld_multi_all(DDRPHY_B1_DQ6,
					p_fld(CLEAR_FLD, B1_DQ6_RG_RX_ARDQ_DDR3_SEL_B1) |
					p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_DDR4_SEL_B1));
				io_32_write_fld_multi_all(DDRPHY_B2_DQ6,
					p_fld(CLEAR_FLD, B2_DQ6_RG_RX_ARDQ_DDR3_SEL_B2) |
					p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_DDR4_SEL_B2));
				io_32_write_fld_multi_all(DDRPHY_CA_CMD6,
					p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_DDR3_SEL) |
					p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_DDR4_SEL));
			}
			io_32_write_fld_align(
				DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ5), vref,
				SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
			io_32_write_fld_align(
				DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ5), vref,
				SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2);
		} else {
			vref = io_32_read_fld_align(DRAMC_REG_ADDR
				(DDRPHY_SHU_B0_DQ5),
				SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
		}

		min_winsize = WORD_MAX;
		min_bit = BYTE_MAX;
		temp_win_sum = 0;
		finish_count = 0;

		for (bit_idx = 0; bit_idx < p->data_width; bit_idx++) {
			win_per_bit[bit_idx].first_pass =
				(signed short) PASS_RANGE_NA;
			win_per_bit[bit_idx].last_pass =
				(signed short) PASS_RANGE_NA;
			final_bitwin[bit_idx].first_pass =
				(signed short) PASS_RANGE_NA;
			final_bitwin[bit_idx].last_pass =
				(signed short) PASS_RANGE_NA;
#if (CONFIG_EYESCAN_LOG == 1)
			   if ((p->dram_type == TYPE_PCDDR4)|| is_lp4_family(p))
			   {
				   gEyeScan_CaliDelay[bit_idx/8] = 0;
				   gEyeScan_DelayCellPI[bit_idx] = 0;
				   gEyeScan_index[bit_idx] = 0;
				   gu1pass_in_this_vref_flag[bit_idx] = 0;
			   }

			   g_rx_vref = vref;
#endif

		}

		/*  Adjust DQM output delay to 0 */
		set_dqm_output_delay_lp4(p, 0);

		/*  Adjust DQ output delay to 0 */
		set_dq_output_delay_lp4(p, 0);

		for (delay = delay_begin; delay <= delay_end;
			delay += delay_step) {
			set_rx_dq_dqs_delay(p, delay);

			if (use_test_engine) {
				err_value = dramc_engine2_run(p,
					TE_OP_WRITE_READ_CHECK,
					p->test_pattern);
			} else {
				err_value = dramc_rx_win_rddqc_run(p);
			}
#if CALIBRATION_LOG
			if (rx_eye_scan_flag_lp4(p, vref_scan_enable)) {
				show_msg2((INFO, "%d, [0]", delay));
			}
#endif
			/*  check fail bit ,0 ok ,others fail */
			finish_count = dramc_rx_win_check(p, final_bitwin,
				win_per_bit, delay, err_value, finish_count,
				vref_scan_enable, delay_end);

			if (((p->data_width == DATA_WIDTH_16BIT) &&
				(finish_count == WORD_MAX)) ||
				((p->data_width == DATA_WIDTH_32BIT) &&
				(finish_count == LWORD_MAX))) {
				if (use_test_engine)
					set_calibration_result(p,
						DRAM_CALIBRATION_RX_PERBIT,
						DRAM_OK);
				else
					set_calibration_result(p,
						DRAM_CALIBRATION_RX_RDDQC,
						DRAM_OK);

				//if (rx_eye_scan_flag_lp4(p, vref_scan_enable)) {
					break; /* early break */
				//}
			}
		}

		for (bit_idx = 0; bit_idx < p->data_width; bit_idx++) {
			final_bitwin[bit_idx].win_size =
				final_bitwin[bit_idx].last_pass -
				final_bitwin[bit_idx].first_pass +
				(final_bitwin[bit_idx].last_pass
				== final_bitwin[bit_idx].first_pass ? 0 : 1);
			temp_win_sum += final_bitwin[bit_idx].win_size;
			/* Sum of CA Windows for vref selection */
#if (CONFIG_EYESCAN_LOG == 1)
			if ((p->dram_type == TYPE_PCDDR4)|| is_lp4_family(p))
			{
				gEyeScan_WinSize[vref][bit_idx] = final_bitwin[bit_idx].win_size;
			}
#endif
			if (final_bitwin[bit_idx].win_size < min_winsize) {
				min_bit = bit_idx;
				min_winsize = final_bitwin[bit_idx].win_size;
			}
		}

		if ((min_winsize > rx_perbit_win_min_max)
			|| ((min_winsize == rx_perbit_win_min_max)
			&& (temp_win_sum > rx_window_sum[p->channel]))) {
			if (vref_scan_enable) {
				show_diag((INFO, "Better RX Vref found %d, Window Min %d >= %d at DQ%d, Window Sum %d > %d\n",
					vref, min_winsize, rx_perbit_win_min_max, min_bit, temp_win_sum, rx_window_sum[p->channel]));
			} else {
				show_msg3((INFO, "\n%s%d, Window Sum %d > %d\n",
					"Better RX Vref found ", vref, temp_win_sum,
					rx_window_sum[p->channel]));
			}

			rx_perbit_win_min_max = min_winsize;
			rx_perbit_win_min_max_idx = min_bit;
			rx_window_sum[p->channel] = temp_win_sum;
			final_vref = vref;

			for (bit_idx = 0; bit_idx < p->data_width; bit_idx++) {
				final_bitwin[bit_idx].win_center =
					(final_bitwin[bit_idx].last_pass +
					final_bitwin[bit_idx].first_pass) >> 1;
				/* window center of each DQ bit */
#if (CONFIG_FOR_HQA_TEST_USED == 1)
				if(g_use_test_engine == 1) {
					gFinalRXPerbitWin[p->channel][p->rank][bit_idx] =
						final_bitwin[bit_idx].win_size;
				}
#endif

				if (rx_eye_scan_flag_lp4(p, vref_scan_enable)) {
					show_msg((INFO,
						"Bit %d%s%d (%d ~ %d) %d\n",
						bit_idx, ", Center ",
						final_bitwin[bit_idx].
						win_center,
						final_bitwin[bit_idx].
						first_pass,
						final_bitwin[bit_idx].
						last_pass,
						final_bitwin[bit_idx].
						win_size));
				}

			}

			/*save best vref rx perbit window*/
			memcpy(best_vref_win_per_bit, final_bitwin,sizeof(best_vref_win_per_bit));
		} else if (vref_scan_enable) {
			show_diag((INFO, "RX Vref %d, Window Min %d <= %d at DQ%d, Window Sum %d <= %d\n",
				vref, min_winsize, rx_perbit_win_min_max, min_bit, temp_win_sum, rx_window_sum[p->channel]));
		}

#if (CONFIG_EYESCAN_LOG == 1)
		if ((p->dram_type == TYPE_PCDDR4)|| is_lp4_family(p))
		{
			for (bit_idx = 0; bit_idx < p->data_width; bit_idx++)
			{
				if (gu1pass_in_this_vref_flag[bit_idx])
					gEyeScan_ContinueVrefHeight[bit_idx]++;
			}
		}
#endif

		if ((temp_win_sum < (rx_window_sum[p->channel] * 95 / 100))
		    && gRX_EYE_Scan_flag == 0
		    ) {
			show_msg3((INFO, "\nRX Vref found, early break!\n"));
			vref = vref_end;
			break;	/* max vref found (95%), early break; */
		}
	}

	/* return final vref perbit window*/
	if (vref_scan_enable)
		memcpy(final_bitwin, best_vref_win_per_bit, sizeof(best_vref_win_per_bit));

	return final_vref;
}

static void rx_check_delay(DRAMC_CTX_T *p,
	PASS_WIN_DATA_T final_bitwin[], RX_DELAY_SET_PERBIT_T *rx_delay, unsigned char vref_scan_enable)
{
	unsigned char bit_idx, byte_idx;
	unsigned char bit_first, bit_last;
	unsigned short tmp_dqm_sum;
	/*
	* 3
	* As per byte, check max DQS delay in 8-bit.
	* Except for the bit of max DQS delay,
	* delay DQ to fulfill setup time = hold time
	*/
	for (byte_idx = 0; byte_idx < (p->data_width / DQS_BIT_NUM);
	     byte_idx++) {
		tmp_dqm_sum = 0;

		bit_first = DQS_BIT_NUM * byte_idx;
		bit_last = DQS_BIT_NUM * byte_idx + DQS_BIT_NUM - 1;
		rx_delay->dqs_dly_perbyte[byte_idx] = MAX_RX_DQSDLY_TAPS;

		for (bit_idx = bit_first; bit_idx <= bit_last; bit_idx++) {
			/* find out max Center value */
			if (final_bitwin[bit_idx].win_center <
				rx_delay->dqs_dly_perbyte[byte_idx]) {
				rx_delay->dqs_dly_perbyte[byte_idx] =
					final_bitwin[bit_idx].win_center;
			}
		}
		/* Delay DQS=0, Delay DQ only */
		if (rx_delay->dqs_dly_perbyte[byte_idx] > 0) {
			rx_delay->dqs_dly_perbyte[byte_idx] = 0;
		} else {	/* Need to delay DQS */
			rx_delay->dqs_dly_perbyte[byte_idx] = -
				rx_delay->dqs_dly_perbyte[byte_idx];
		}

		/*
		* we delay DQ or DQS to let DQS sample the middle of
		* rx pass window for all the 8 bits,
		*/
		for (bit_idx = bit_first; bit_idx <= bit_last; bit_idx++) {
			final_bitwin[bit_idx].best_dqdly =
				rx_delay->dqs_dly_perbyte[byte_idx] +
				final_bitwin[bit_idx].win_center;
			tmp_dqm_sum += final_bitwin[bit_idx].best_dqdly;
#if (CONFIG_EYESCAN_LOG == 1)
			if ((p->dram_type == TYPE_PCDDR4)|| is_lp4_family(p)) {
				gEyeScan_DelayCellPI[bit_idx] = final_bitwin[bit_idx].best_dqdly;
			}
#endif

		}

		/*  calculate DQM as average of 8 DQ delay */
		rx_delay->dqm_dly_perbyte[byte_idx] =
			tmp_dqm_sum / DQS_BIT_NUM;
	}

	print_calibration_basic_info(p);

	if (vref_scan_enable) {
		show_diag((INFO, "%s%d%s%d\nDQM Delay:\nDQM0 = %d, DQM1 = %d\n",
				"DQS Delay:\nDQS0 = ", rx_delay->dqs_dly_perbyte[0],
				", DQS1 = ", rx_delay->dqs_dly_perbyte[1],
				rx_delay->dqm_dly_perbyte[0],
				rx_delay->dqm_dly_perbyte[1]));
		show_diag((INFO, "DQ Delay:\n"));
	} else {
		show_msg((INFO, "%s%d%s%d\nDQM Delay:\nDQM0 = %d, DQM1 = %d\n",
				"DQS Delay:\nDQS0 = ", rx_delay->dqs_dly_perbyte[0],
				", DQS1 = ", rx_delay->dqs_dly_perbyte[1],
				rx_delay->dqm_dly_perbyte[0],
				rx_delay->dqm_dly_perbyte[1]));
		show_msg((INFO, "DQ Delay:\n"));
	}
}

/* Process 4bit pinmux. PINMUX. REVIEW.
 * Note that currently only LPDDR3 uses 4bit mux.
 */
static void dramc_rx_win_process_pinmux(DRAMC_CTX_T *p,
	PASS_WIN_DATA_T win_per_bit[], RX_DELAY_SET_PERBIT_T *rx_delay)
{
	unsigned char *lpddr_4bitmux_byte_mapping;
	unsigned char idx;
	unsigned char idx_mapping;
	unsigned short dq_swap;

	if (p->en_4bit_mux == DISABLE)
		return;

	lpddr_4bitmux_byte_mapping = dramc_get_4bitmux_byte_mapping(p);
	if (lpddr_4bitmux_byte_mapping == NULL)
		return; /* do nothing */

	for (idx = 0; idx < 8; idx++) {
		idx_mapping = lpddr_4bitmux_byte_mapping[idx];

		/* swap */
		dq_swap = win_per_bit[idx].best_dqdly;
		win_per_bit[idx].best_dqdly =
			win_per_bit[idx_mapping].best_dqdly;
		win_per_bit[idx_mapping].best_dqdly = dq_swap;

		if (p->data_width == DATA_WIDTH_32BIT) {
			unsigned char idx_b23;

			idx_b23 = idx + 16;
			idx_mapping = lpddr_4bitmux_byte_mapping[idx_b23];

			dq_swap = win_per_bit[idx_b23].best_dqdly;
			win_per_bit[idx_b23].best_dqdly =
				win_per_bit[idx_mapping].best_dqdly;
			win_per_bit[idx_mapping].best_dqdly = dq_swap;
		}
	}
}

#define RX_B2_ADDR_OFST	0x2000
DRAM_STATUS_T dramc_rx_window_perbit_cal(DRAMC_CTX_T *p,
	unsigned char use_test_engine)
{
	unsigned char bit_idx, byte_idx, dqs_idx;
	signed short delay_begin = 0, delay_end, delay_step = 1;
	PASS_WIN_DATA_T final_win_per_bit[DQ_DATA_WIDTH];
	RX_DELAY_SET_PERBIT_T rx_delay;
	unsigned char vref_scan_enable;
	unsigned short final_vref = 0xe;
	unsigned short vref_begin, vref_end, vref_step;
#if (CONFIG_EYESCAN_LOG == 1)
	U32 vref_idx, ii;
	g_use_test_engine = use_test_engine;
#endif

	show_msg_with_timestamp((INFO, "start Rx DQ window cal\n"));

	if (!p) {
		show_err("context NULL\n");
		return DRAM_FAIL;
	}
	memset(&rx_delay, 0x0, sizeof(RX_DELAY_SET_PERBIT_T));
	/*
	 * 1.delay DQ ,find the pass widnow (left boundary).
	 * 2.delay DQS find the pass window (right boundary).
	 * 3.Find the best DQ / DQS to satify the middle value of
	 *   the overall pass window per bit
	 * 4.Set DQS delay to the max per byte, delay DQ to de-skew
	 */
	vref_scan_enable = dramc_rx_window_perbit_ini(p, use_test_engine);

#if (CONFIG_EYESCAN_LOG == 1)
	vref_scan_enable = 1;
#endif

	if (vref_scan_enable) { /* 3200, 3733, 4266 */
#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_VREF_CAL)
		if(p->femmc_Ready==1) {
			vref_begin = p->pSavetimeData->u1RxWinPerbitVref_Save[p->channel];
			vref_end = vref_begin;
			vref_step = 1;
		}
		else
#endif
		{
		if (p->odt_onoff) {
			vref_begin = RX_VREF_RANGE_BEGIN;
		} else { /* 1600 */
			if (rx_eye_scan_flag == 0 && gRX_EYE_Scan_flag == 0)
				vref_begin = RX_VREF_RANGE_BEGIN_ODTOFF;
			else
				vref_begin = 0;
		}
			vref_end = RX_VREF_RANGE_END;
			vref_step = RX_VREF_RANGE_STEP;
		}
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), SET_FLD,
			B0_DQ5_RG_RX_ARDQ_VREF_EN_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ5), SET_FLD,
			B2_DQ5_RG_RX_ARDQ_VREF_EN_B2);
	} else {
		vref_begin = 0;
		vref_end = 0;
		vref_step = 1;
	}
	if (vref_scan_enable) {
		show_diag((INFO, "Rx Vref Scan = %d (%d~%d)\n", vref_scan_enable,
			vref_begin, vref_end));
	} else {
		show_msg3((INFO, "Rx Vref Scan = %d (%d~%d)\n", vref_scan_enable,
			vref_begin, vref_end));
	}

	if (p->frequency >= DDR2666_FREQ)
		delay_begin = -MAX_RX_DQSDLY_TAPS_2666;
	else if (p->frequency >= DDR1600_FREQ)
		delay_begin = -MAX_RX_DQSDLY_TAPS_1600;
	else
		delay_begin = -MAX_RX_DQSDLY_TAPS;

	delay_end = MAX_RX_DQDLY_TAPS;
	if (vref_scan_enable) {
		show_diag((INFO, "x-axis: bit #, y-axis: DQ dly (%d~%d)\n",
			delay_begin, delay_end));
	} else {
		show_msg3((INFO, "x-axis: bit #, y-axis: DQ dly (%d~%d)\n",
			delay_begin, delay_end));
	}

#if (FOR_DV_SIMULATION_USED == 0)
	delay_step = 1;
#else
	if (p->frequency <= DDR1600_FREQ)
		delay_step = 4;
	else
		delay_step = 2;
#endif

#if (CONFIG_EYESCAN_LOG == 1)
	g_delay_step = delay_step;

	if ((p->dram_type == TYPE_PCDDR4)|| is_lp4_family(p))
	{
		for(vref_idx = 0; vref_idx<RX_VREF_RANGE_END+1;vref_idx++)
		{
			for (bit_idx = 0; bit_idx < DQ_DATA_WIDTH; bit_idx++)
			{
				for(ii=0; ii<EYESCAN_BROKEN_NUM; ii++)
				{
					gEyeScan_Min[vref_idx][bit_idx][ii] = EYESCAN_DATA_INVALID;
					gEyeScan_Max[vref_idx][bit_idx][ii] = EYESCAN_DATA_INVALID;

					gEyeScan_ContinueVrefHeight[bit_idx] = 0;
					gEyeScan_TotalPassCount[bit_idx] = 0;
				}
			}
		}
	}
#endif
	/*
	 * default set result fail.
	 * When window found, update the result as oK
	*/
	if (use_test_engine) {
		set_calibration_result(p, DRAM_CALIBRATION_RX_PERBIT,
			DRAM_FAIL);
		dramc_engine2_init(p, p->test2_1, p->test2_2,
			p->test_pattern, 0);
	} else {
		set_calibration_result(p, DRAM_CALIBRATION_RX_RDDQC,
			DRAM_FAIL);
		dramc_rx_win_rddqc_init(p);
	}
#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION)
	if(p->femmc_Ready==1 && (p->Bypass_RDDQC || p->Bypass_RXWINDOW))
	{
		if(vref_scan_enable)
		{
			// load RX Vref from eMMC
	#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_VREF_CAL)
			//u2VrefBegin = p->pSavetimeData->u1RxWinPerbitVref_Save[p->channel];
			//u2VrefEnd=u2VrefBegin+1;
			final_vref = p->pSavetimeData->u1RxWinPerbitVref_Save[p->channel];
	#endif
		}
		if(use_test_engine)
			set_calibration_result(p, DRAM_CALIBRATION_RX_PERBIT, DRAM_OK);
		else
			set_calibration_result(p, DRAM_CALIBRATION_RX_RDDQC, DRAM_OK);
	}
	else
#endif
    {
		final_vref = dramc_rx_window_perbit_cal_process(p, vref_scan_enable,
			final_win_per_bit, use_test_engine, vref_begin,
			vref_end, vref_step, delay_begin, delay_end, delay_step);
	}

	if (use_test_engine)
		dramc_engine2_end(p);
	else
		dramc_rx_win_rddqc_end(p);

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_VREF_CAL)
	if(p->femmc_Ready==0)
	{
		p->pSavetimeData->u1RxWinPerbitVref_Save[p->channel]=final_vref;
	}
#endif

	/* Set RX Final Vref Here */
	if (vref_scan_enable == 1) {
#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION)
		if ((p->dram_type == TYPE_PCDDR4) && (p->femmc_Ready==1)) {
			io_32_write_fld_multi_all(DDRPHY_B0_DQ6,
				p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0) |
				p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_DDR4_SEL_B0));
			io_32_write_fld_multi_all(DDRPHY_B1_DQ6,
				p_fld(CLEAR_FLD, B1_DQ6_RG_RX_ARDQ_DDR3_SEL_B1) |
				p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_DDR4_SEL_B1));
			io_32_write_fld_multi_all(DDRPHY_B2_DQ6,
				p_fld(CLEAR_FLD, B2_DQ6_RG_RX_ARDQ_DDR3_SEL_B2) |
				p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_DDR4_SEL_B2));
			io_32_write_fld_multi_all(DDRPHY_CA_CMD6,
				p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_DDR3_SEL) |
				p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_DDR4_SEL));
		}
#endif
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ5),
			final_vref, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ5),
			final_vref, SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2);

		show_diag((INFO,
			"\nFinal RX Vref %d, apply to both rank0 and 1\n",
			final_vref));

		final_rx_vref_dq[p->channel][p->rank] =
			(unsigned char) final_vref;
	}
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	if(p->femmc_Ready==1 && (p->Bypass_RDDQC || p->Bypass_RXWINDOW))
	{
		show_msg((INFO, "[FAST_K] Bypass RxWindowPerbitCal\n"));
		// load RX DQS and DQM delay from eMMC
		for (byte_idx=0; byte_idx<(p->data_width / DQS_BIT_NUM); byte_idx++)
		{
			rx_delay.dqs_dly_perbyte[byte_idx]= p->pSavetimeData->u1RxWinPerbit_DQS[p->channel][p->rank][byte_idx];
			rx_delay.dqm_dly_perbyte[byte_idx]= p->pSavetimeData->u1RxWinPerbit_DQM[p->channel][p->rank][byte_idx];
		}
		// load RX DQ delay from eMMC
		rx_window_sum[p->channel] = 0;
		rx_perbit_win_min_max = WORD_MAX;
		for (bit_idx=0; bit_idx<16; bit_idx++)
		{
			final_win_per_bit[bit_idx].best_dqdly= p->pSavetimeData->u1RxWinPerbit_DQ[p->channel][p->rank][bit_idx];
			final_win_per_bit[bit_idx].first_pass= p->pSavetimeData->u1RxWinPerbitDQ_firsbypass_Save[p->channel][p->rank][bit_idx];
			final_win_per_bit[bit_idx].last_pass= p->pSavetimeData->u1RxWinPerbitDQ_lastbypass_Save[p->channel][p->rank][bit_idx];
			final_win_per_bit[bit_idx].win_center= (final_win_per_bit[bit_idx].first_pass + final_win_per_bit[bit_idx].last_pass) >> 1;
			final_win_per_bit[bit_idx].win_size= final_win_per_bit[bit_idx].last_pass - final_win_per_bit[bit_idx].first_pass +
			                                     ((final_win_per_bit[bit_idx].first_pass == final_win_per_bit[bit_idx].last_pass) ? 0 : 1);
            if (final_win_per_bit[bit_idx].win_size < rx_perbit_win_min_max)
            {
                rx_perbit_win_min_max = final_win_per_bit[bit_idx].win_size;
                rx_perbit_win_min_max_idx = bit_idx;
            }
			rx_window_sum[p->channel] += final_win_per_bit[bit_idx].win_size;
		}
	}
	else
	{
		rx_check_delay(p, final_win_per_bit, &rx_delay, vref_scan_enable);
		// save RX DQS and DQM delay to eMMC
		for (byte_idx=0; byte_idx<(p->data_width / DQS_BIT_NUM); byte_idx++)
		{
			p->pSavetimeData->u1RxWinPerbit_DQS[p->channel][p->rank][byte_idx]=(U8)rx_delay.dqs_dly_perbyte[byte_idx];
			p->pSavetimeData->u1RxWinPerbit_DQM[p->channel][p->rank][byte_idx]=(U8)rx_delay.dqm_dly_perbyte[byte_idx];
		}
		// save RX DQ delay to eMMC
		for (bit_idx=0; bit_idx<16; bit_idx++)
		{
			p->pSavetimeData->u1RxWinPerbit_DQ[p->channel][p->rank][bit_idx] = final_win_per_bit[bit_idx].best_dqdly;
			p->pSavetimeData->u1RxWinPerbitDQ_firsbypass_Save[p->channel][p->rank][bit_idx] = final_win_per_bit[bit_idx].first_pass;
			p->pSavetimeData->u1RxWinPerbitDQ_lastbypass_Save[p->channel][p->rank][bit_idx] = final_win_per_bit[bit_idx].last_pass;
		}
	}
#else
	rx_check_delay(p, final_win_per_bit, &rx_delay, vref_scan_enable);
#endif

	//RX final window info
	if (vref_scan_enable) {
		for (bit_idx = 0; bit_idx < p->data_width; bit_idx++) {
			show_diag((INFO, "[CH%d][RK%d][%d][RX] Bit%d (%d ~ %d) %d %d\n",\
				p->channel, p->rank, p->frequency*2,\
				bit_idx, final_win_per_bit[bit_idx].first_pass, final_win_per_bit[bit_idx].last_pass,\
				final_win_per_bit[bit_idx].win_center, final_win_per_bit[bit_idx].win_size));
		}
		show_msg0((INFO, "\n[CH%d][RK%d][%d][RX] Best Vref %d, Window Min %d at DQ%d, Window Sum %d\n",\
			p->channel, p->rank, p->frequency*2,\
			final_vref, rx_perbit_win_min_max, rx_perbit_win_min_max_idx, rx_window_sum[p->channel]));

#ifdef ENABLE_MIOCK_JMETER
		if (p->frequency >= PERBIT_THRESHOLD_FREQ) {
			if(((rx_perbit_win_min_max*100)/(p->ucnum_dlycell_perT>>1)) < RX_WIN_CRITERIA) {
					show_msg0((INFO, "RX margin fail@DQ%d, 0.%dUI<0.%dUI\n",
							rx_perbit_win_min_max_idx, ((rx_perbit_win_min_max*100)/(p->ucnum_dlycell_perT>>1)), RX_WIN_CRITERIA));
#ifdef DRAM_SLT
					dram_slt_set(p, DRAM_CALIBRATION_RX_PERBIT, DRAM_FAIL);
#endif
			}
		}
#endif
	}

#if (CONFIG_FOR_HQA_REPORT_USED == 1)
	for (dqs_idx = 0U; dqs_idx < (p->data_width / DQS_BIT_NUM); dqs_idx++) {
		U32 bit_first, bit_last;

		bit_first = dqs_idx * DQS_BIT_NUM;
		bit_last = bit_first + DQS_BIT_NUM - 1;

		hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0,
			"RX_Window_Center_DQS", dqs_idx, rx_delay.dqs_dly_perbyte[dqs_idx], NULL);
		hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT0,
			"RX_Window_Center_DQM", dqs_idx, rx_delay.dqm_dly_perbyte[dqs_idx], NULL);
		for (bit_idx = bit_first; bit_idx <= bit_last; bit_idx++) {
			hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT1,
				"RX_Window_Center_DQ", bit_idx, final_win_per_bit[bit_idx].win_center,
				NULL);
		}
	}
#endif
	dramc_rx_win_process_pinmux(p, final_win_per_bit, &rx_delay);

	for (bit_idx = 0; bit_idx < p->data_width; bit_idx = bit_idx + 4) {
		if (vref_scan_enable) {
			show_diag((INFO,
				"DQ%d =%d, DQ%d =%d, DQ%d =%d, DQ%d =%d\n",
				bit_idx, final_win_per_bit[bit_idx].best_dqdly,
				bit_idx + 1, final_win_per_bit[bit_idx + 1].best_dqdly,
				bit_idx + 2, final_win_per_bit[bit_idx + 2].best_dqdly,
				bit_idx + 3, final_win_per_bit[bit_idx + 3].best_dqdly));
		} else {
			show_msg((INFO,
				"DQ%d =%d, DQ%d =%d, DQ%d =%d, DQ%d =%d\n",
				bit_idx, final_win_per_bit[bit_idx].best_dqdly,
				bit_idx + 1, final_win_per_bit[bit_idx + 1].best_dqdly,
				bit_idx + 2, final_win_per_bit[bit_idx + 2].best_dqdly,
				bit_idx + 3, final_win_per_bit[bit_idx + 3].best_dqdly));
		}
	}
	show_msg2((INFO, "\n"));

	rx_dqs_duty_offset[p->channel][0][0] =
		rx_dqs_duty_offset[p->channel][0][1] =
		rx_dqs_duty_offset[p->channel][1][0] =
		rx_dqs_duty_offset[p->channel][1][1] = 0;

	/* billow: DV simulation workaround to avoid Rx fake pass */
#if 0//(FOR_DV_SIMULATION_USED == 1)
	if (p->dram_type == TYPE_PCDDR4 && use_test_engine == 1) {
		for (byte_idx = 0; byte_idx < (p->data_width / DQS_BIT_NUM);
			byte_idx++) {
			rx_delay.dqm_dly_perbyte[byte_idx] += 26;
		}

		for (bit_idx = 0; bit_idx < p->data_width; bit_idx++) {
			final_win_per_bit[bit_idx].best_dqdly += 26;
		}
	}
#endif

	/*  set dqs delay, (dqm delay) */
	for (byte_idx = 0; byte_idx < (p->data_width / DQS_BIT_NUM);
		byte_idx++) {
		unsigned int bak_channel;
		unsigned char byte_sel, byte_ofst;

		bak_channel = p->channel;
		byte_ofst = byte_idx;

		/*
		 * Here we want to set PHY register, since RX compare result is DRAMC View. for PHY,
		 * it shall be mapped to correct byte.
		 */
		byte_sel = mapping_phy_byte_to_dramc(p, byte_idx);

		if ((p->data_width == DATA_WIDTH_32BIT) && (byte_idx > 1)) {
			p->channel = CHANNEL_B;
			byte_ofst -= 2;
		}
		/* Set DQS & DQM delay */
		io_32_write_fld_multi(DRAMC_REG_ADDR
			(DDRPHY_SHU_R0_B0_DQ6 + RX_B2_ADDR_OFST * byte_ofst),
			p_fld(((unsigned int)rx_delay.dqs_dly_perbyte[byte_sel]
			+ rx_dqs_duty_offset[p->channel][byte_sel][0]),
			SHU_R0_B0_DQ6_RK0_RX_ARDQS0_R_DLY_B0) |
			p_fld(((unsigned int)rx_delay.dqs_dly_perbyte[byte_sel]
			+ rx_dqs_duty_offset[p->channel][byte_sel][1]),
			SHU_R0_B0_DQ6_RK0_RX_ARDQS0_F_DLY_B0) |
			p_fld(((unsigned int)rx_delay.dqm_dly_perbyte[byte_sel]),
			SHU_R0_B0_DQ6_RK0_RX_ARDQM0_R_DLY_B0) |
			p_fld(((unsigned int)rx_delay.dqm_dly_perbyte[byte_sel]),
			SHU_R0_B0_DQ6_RK0_RX_ARDQM0_F_DLY_B0));

		p->channel = bak_channel;
	}

	dram_phy_reset(p);

	/*  set dq delay */
	for (bit_idx = 0; bit_idx < DQS_BIT_NUM; bit_idx += 2) {
		unsigned char *mapping = dramc_get_phy2dramc_dq_mapping(p);

		/* Write DQ perbit delay in phy, but the result stored is DRAMC view */
		set_dq_delay_lp4(p, bit_idx,
			final_win_per_bit[mapping[bit_idx]].best_dqdly,
			final_win_per_bit[mapping[bit_idx + 1]].best_dqdly,
			final_win_per_bit[mapping[bit_idx + 8]].best_dqdly,
			final_win_per_bit[mapping[bit_idx + 9]].best_dqdly);

		/* Set B2, B3 */
		if (p->data_width == DATA_WIDTH_32BIT) {
			unsigned int bit_idx_tmp;

			bit_idx_tmp = bit_idx + 16;
			set_dq_delay_lp4(p, bit_idx_tmp,
				final_win_per_bit[mapping[bit_idx_tmp]].best_dqdly,
				final_win_per_bit[mapping[bit_idx_tmp + 1]].best_dqdly,
				final_win_per_bit[mapping[bit_idx_tmp + 8]].best_dqdly,
				final_win_per_bit[mapping[bit_idx_tmp + 9]].best_dqdly);
		}
	}

#ifdef DIAG // RX Vref scan at center
	if (vref_scan_enable) {
        // u2WinVrefConf_x stands for the step offset based on the Final Vref
        // _L means low-bond, _H means high-bond
        u16 u2WinVrefConf_L, u2WinVrefConf_H;
        u16 u2FinalRXVrefConf, u2TestRXVrefConf;
        u16 u2TestRXVrefConf_L_Start, u2TestRXVrefConf_L_End, u2TestRXVrefConf_H_Start, u2TestRXVrefConf_H_End;
		u32 u4err_value;

        u2FinalRXVrefConf = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ5), SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);

        u2TestRXVrefConf_L_Start = (u2FinalRXVrefConf - RX_VREF_CHECK_WIN_CONF_START > RX_VREF_MIN_CONF) ? (u2FinalRXVrefConf - RX_VREF_CHECK_WIN_CONF_START) : RX_VREF_MIN_CONF;
        u2TestRXVrefConf_L_End = (u2FinalRXVrefConf - RX_VREF_CHECK_WIN_CONF_END > RX_VREF_MIN_CONF) ? (u2FinalRXVrefConf - RX_VREF_CHECK_WIN_CONF_END) : RX_VREF_MIN_CONF;
        u2TestRXVrefConf_H_Start = (u2FinalRXVrefConf + RX_VREF_CHECK_WIN_CONF_START < RX_VREF_MAX_CONF) ? (u2FinalRXVrefConf + RX_VREF_CHECK_WIN_CONF_START) : RX_VREF_MAX_CONF;
        u2TestRXVrefConf_H_End = (u2FinalRXVrefConf + RX_VREF_CHECK_WIN_CONF_END < RX_VREF_MAX_CONF) ? (u2FinalRXVrefConf + RX_VREF_CHECK_WIN_CONF_END) : RX_VREF_MAX_CONF;

        // Keep the delay settings (x axis), reduce/increase Vref (y axis) and check pass/fail to get margin on Y axis
        // RX will scan the neighbor 2 steps around the pass criteria. Refer to JEDEC, we need to cover 50mV on each side for RX Vref -> 4~5 steps
        // we will check 3~7 steps on both direction, verified by XTALK pattern
        // then BFT will 100% detect the mainboards with insufficient Vref window height and filter them out for FA
        //
        // Noted, the loop here is count by step, for RX Vref it is 9.3mV or 12.3mV or 14.4mV per step
        u2WinVrefConf_L = u2FinalRXVrefConf - u2TestRXVrefConf_L_End;
        for (u2TestRXVrefConf = u2TestRXVrefConf_L_Start; u2TestRXVrefConf > u2TestRXVrefConf_L_End; u2TestRXVrefConf--)
        {
            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ5), u2TestRXVrefConf, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ5), u2TestRXVrefConf, SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2);
            DRAMC_ENGINE_DO_RX_TEST(p, u4err_value);
            if (u4err_value != 0)
            {
                show_diag((INFO, "RX Vref L fail at conf %d\n", u2TestRXVrefConf));
                dram_phy_reset(p);
                u2WinVrefConf_L = u2FinalRXVrefConf - u2TestRXVrefConf - 1;
                break;
            }
        }
        u2WinVrefConf_H = u2TestRXVrefConf_H_End - u2FinalRXVrefConf;
        for (u2TestRXVrefConf = u2TestRXVrefConf_H_Start; u2TestRXVrefConf < u2TestRXVrefConf_H_End; u2TestRXVrefConf++)
        {
            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ5), u2TestRXVrefConf, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ5), u2TestRXVrefConf, SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2);
            DRAMC_ENGINE_DO_RX_TEST(p, u4err_value);
            if (u4err_value != 0)
            {
                show_diag((INFO, "RX Vref H fail at conf %d\n", u2TestRXVrefConf));
                dram_phy_reset(p);
                u2WinVrefConf_H = u2TestRXVrefConf - u2FinalRXVrefConf - 1;
                break;
            }
        }

        // set Vref back to the Final and dump the Vref window check result
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ5), u2FinalRXVrefConf, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ5), u2FinalRXVrefConf, SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2);
        show_diag((INFO, "[%dMHz][CH%d][RK%d] RX Vref Final %d, Vref Window at center, offset based on Final Vref (-%d~%d)\n", p->frequency, p->channel, p->rank,
            u2FinalRXVrefConf, u2WinVrefConf_L, u2WinVrefConf_H));
	}
#endif

#ifdef DIAG // CA Vref scan at center
    if (vref_scan_enable)
    {
        // u2WinVrefPerm_x stands for the per-mil offset based on the Final Vref per-mil
        // u2WinVrefPerm_x / MR_VREF_STEP_PERM stands for the step offset based on the Final Vref
        // _L means low-bond, _H means high-bond
        u16 u2WinVrefPerm_L, u2WinVrefPerm_H;
        u16 u2FinalCAVrefConf, u2FinalCAVrefPerm, u2TestCAVrefPerm;
        u16 u2TestCAVrefPerm_L_Start, u2TestCAVrefPerm_L_End, u2TestCAVrefPerm_H_Start, u2TestCAVrefPerm_H_End;
		u32 u4err_value;

        dramc_mrr_by_rank(p, p->rank, 12, &u2FinalCAVrefConf); // MR12 is read-write able
        u2FinalCAVrefPerm = CONVERT_VREF_CONF_TO_PERM(u2FinalCAVrefConf);

        u2TestCAVrefPerm_L_Start = (u2FinalCAVrefPerm - CA_VREF_CHECK_WIN_PERM_START > MR_VREF_RANGE0_MIN_PERM) ? (u2FinalCAVrefPerm - CA_VREF_CHECK_WIN_PERM_START) : MR_VREF_RANGE0_MIN_PERM;
        u2TestCAVrefPerm_L_End = (u2FinalCAVrefPerm - CA_VREF_CHECK_WIN_PERM_END > MR_VREF_RANGE0_MIN_PERM) ? (u2FinalCAVrefPerm - CA_VREF_CHECK_WIN_PERM_END) : MR_VREF_RANGE0_MIN_PERM;
        u2TestCAVrefPerm_H_Start = (u2FinalCAVrefPerm + CA_VREF_CHECK_WIN_PERM_START < MR_VREF_RANGE1_MAX_PERM) ? (u2FinalCAVrefPerm + CA_VREF_CHECK_WIN_PERM_START) : MR_VREF_RANGE1_MAX_PERM;
        u2TestCAVrefPerm_H_End = (u2FinalCAVrefPerm + CA_VREF_CHECK_WIN_PERM_END < MR_VREF_RANGE1_MAX_PERM) ? (u2FinalCAVrefPerm + CA_VREF_CHECK_WIN_PERM_END) : MR_VREF_RANGE1_MAX_PERM;

        // Keep the delay settings (x axis), reduce/increase Vref (y axis) and check pass/fail to get margin on Y axis
        // CA will scan the neighbor 4 steps around the pass criteria. Refer to JEDEC, we need to cover 155mV for CA Vref.
        // 155mV means 77.5mV on each side (up/bottom), i.e. 77.5/4.4=17.6 -> 18 steps
        // we will check 14~22 steps on both direction, verified by AUDIO & XTALK pattern
        // If CA failed, then the MR access will always failed, i.e. memtest will always failed and code will assert in preloader
        // then BFT will 100% detect such kind of mainboards and filter them out for FA
        //
        // Noted, the loop here is count by per-mil, 0.4% per step
        u2WinVrefPerm_L = u2FinalCAVrefPerm - u2TestCAVrefPerm_L_End;
        for (u2TestCAVrefPerm = u2TestCAVrefPerm_L_Start; u2TestCAVrefPerm > u2TestCAVrefPerm_L_End; u2TestCAVrefPerm -= MR_VREF_STEP_PERM)
        {
            dramc_mode_reg_write_by_rank(p, p->rank, 12, CONVERT_VREF_PERM_TO_CONF(u2TestCAVrefPerm));
            // Call TX test pattern for CA Vref here
            DRAMC_ENGINE_DO_TX_TEST(p, u4err_value);
            if (u4err_value != 0)
            {
                show_diag((INFO, "CA Vref L fail at perm %d, conf 0x%x\n", u2TestCAVrefPerm, CONVERT_VREF_PERM_TO_CONF(u2TestCAVrefPerm)));
                u2WinVrefPerm_L = u2FinalCAVrefPerm - u2TestCAVrefPerm - MR_VREF_STEP_PERM;
                break;
            }
        }
        u2WinVrefPerm_H = u2TestCAVrefPerm_H_End - u2FinalCAVrefPerm;
        for (u2TestCAVrefPerm = u2TestCAVrefPerm_H_Start; u2TestCAVrefPerm < u2TestCAVrefPerm_H_End; u2TestCAVrefPerm += MR_VREF_STEP_PERM)
        {
            dramc_mode_reg_write_by_rank(p, p->rank, 12, CONVERT_VREF_PERM_TO_CONF(u2TestCAVrefPerm));
            // Call TX test pattern for CA Vref here
            DRAMC_ENGINE_DO_TX_TEST(p, u4err_value);
            if (u4err_value != 0)
            {
                show_diag((INFO, "CA Vref H fail at perm %d, conf 0x%x\n", u2TestCAVrefPerm, CONVERT_VREF_PERM_TO_CONF(u2TestCAVrefPerm)));
                u2WinVrefPerm_H = u2TestCAVrefPerm - u2FinalCAVrefPerm - MR_VREF_STEP_PERM;
                break;
            }
        }

        // set Vref back to the Final and dump the Vref window check result
        dramc_mode_reg_write_by_rank(p, p->rank, 12, u2FinalCAVrefConf);
        show_diag((INFO, "[%dMHz][CH%d][RK%d] CA Vref Final %d, Vref Window at center, offset based on Final Vref (-%d~%d)\n", p->frequency, p->channel, p->rank,
            u2FinalCAVrefConf & 0x3F, u2WinVrefPerm_L / MR_VREF_STEP_PERM, u2WinVrefPerm_H / MR_VREF_STEP_PERM));
    }
#endif
	show_msg3((INFO, "[DramcRxWindowPerbitCal] Done\n"));

	return DRAM_OK;
}
#endif /* SIMULATION_RX_PERBIT */

#if SIMULATION_DATLAT
static void dle_factor_handler(DRAMC_CTX_T *p, unsigned char curr_val,
	unsigned char pip_num)
{
	unsigned char dlecg_option_ext1 = 0;
	unsigned char dlecg_option_ext2 = 0;
	unsigned char dlecg_option_ext3 = 0;

	if (curr_val < 2)
		curr_val = 2;

	/*	Datlat_dsel = datlat -1, only 1 TX pipe */
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_CONF1),
		p_fld(curr_val, SHU_CONF1_DATLAT) |
		p_fld(curr_val - 4, SHU_CONF1_DATLAT_DSEL) |
		p_fld(curr_val - 4, SHU_CONF1_DATLAT_DSEL_PHY));

	dlecg_option_ext1 = (curr_val>=8) ? (1) : (0);
	dlecg_option_ext2 = (curr_val>=14) ? (1) : (0);
	dlecg_option_ext3 = (curr_val>=19) ? (1) : (0);

	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_PIPE),
		p_fld(dlecg_option_ext1, SHU_PIPE_READ_START_EXTEND1) |
		p_fld(dlecg_option_ext1, SHU_PIPE_DLE_LAST_EXTEND1) |
		p_fld((dlecg_option_ext2), SHU_PIPE_READ_START_EXTEND2) |
		p_fld((dlecg_option_ext2), SHU_PIPE_DLE_LAST_EXTEND2) |
		p_fld((dlecg_option_ext3), SHU_PIPE_READ_START_EXTEND3) |
		p_fld((dlecg_option_ext3), SHU_PIPE_DLE_LAST_EXTEND3));

	dram_phy_reset(p);
}

unsigned char dramc_rxdatlat_scan(DRAMC_CTX_T *p,
	DRAM_DATLAT_CALIBRATION_TYTE_T use_rxtx_scan)
{
	unsigned char ii;
	unsigned int prv_register_080;
	unsigned int err_value = LWORD_MAX;
	unsigned char first, begin, sum, best_step, pipe_num = 0;
	unsigned short datlat_begin;

	/* error handling */
	if (!p) {
		show_err("context NULL\n");
		return DRAM_FAIL;
	}

	show_msg((INFO, "\n[DATLAT]\n"
		"Freq=%d, CH%d RK%d, use_rxtx_scan=%d\n\n",
		p->frequency, p->channel, p->rank, use_rxtx_scan));

	/*  pre-save */
	/*  0x07c[6:4]   DATLAT bit2-bit0 */
	prv_register_080 = io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_SHU_CONF1));

	/*  init best_step to default */
	best_step = (unsigned char) io_32_read_fld_align
		(DRAMC_REG_ADDR(DRAMC_REG_SHU_CONF1), SHU_CONF1_DATLAT);
	show_msg2((INFO, "DATLAT Default: 0x%x\n", best_step));

	/*
	 *  1.set DATLAT 0-15
	 *  2.enable engine1 or engine2
	 *  3.check result  ,3~4 taps pass
	 *  4.set DATLAT 2nd value for optimal
	 */

	/* Initialize */
	first = BYTE_MAX;
	begin = 0;
	sum = 0;

	dramc_engine2_init(p, p->test2_1, p->test2_2, p->test_pattern, 0);
	datlat_begin = best_step - 4;
#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_DATLAT)
	if(p->femmc_Ready==1)
        best_step = p->pSavetimeData->u1RxDatlat_Save[p->channel][p->rank];
    else
#endif
	{
	for (ii = datlat_begin; ii < DATLAT_TAP_NUMBER; ii++) {
		/* 1 */
		dle_factor_handler(p, ii, pipe_num);

		/* 2 */
		if (use_rxtx_scan == fcDATLAT_USE_DEFAULT) {
			err_value = dramc_engine2_run(p,
				TE_OP_WRITE_READ_CHECK, p->test_pattern);
		}
		/* 3 */
		if (err_value == 0) {
			if (begin == 0) { /* first tap which is pass */
				first = ii;
				begin = 1;
			}
			if (begin == 1) {
				sum++;
				if (sum > 4) /* early break. */
					break;
			}
		} else if (err_value != 0 && sum <= 1) {
			first = BYTE_MAX;
			begin = 0;
			sum = 0;
		} else {
			if (begin == 1) {
				/* pass range end */
				begin = BYTE_MAX;
			}
		}

		show_msg2((INFO, "%d, 0x%X, sum=%d\n", ii, err_value, sum));
	}

	dramc_engine2_end(p);

	/* 4 */
	if (sum == 0)
		show_err("no DATLAT taps pass, DATLAT calibration fail!\n");
	else if (sum <= 3)
		best_step = first + (sum >> 1);
	else /* window is larger htan 3 */
		best_step = first + 2;
	}
	rx_datlat_result[p->channel][p->rank] = best_step;
#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_DATLAT)
	 if(p->femmc_Ready==0)
		 p->pSavetimeData->u1RxDatlat_Save[p->channel][p->rank] = best_step;
#endif
#if (CONFIG_FOR_HQA_TEST_USED == 1)
	hqa_log_message_for_report(p, p->channel, p->rank, HQA_REPORT_FORMAT2,
		"DATLAT", 0, best_step, NULL);
#endif

	show_msg((INFO,
		"pattern=%d first_step=%d total pass=%d best_step=%d\n",
		p->test_pattern, first, sum, best_step));

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_DATLAT)
	if(p->femmc_Ready==1)
	{
        dle_factor_handler(p, best_step, pipe_num);
        set_calibration_result(p, DRAM_CALIBRATION_DATLAT, DRAM_OK);
	}
    else
#endif
	{
	if (sum < 4) {
		show_msg2((INFO, "[NOTICE] CH%d, DatlatSum %d\n",
			 p->channel, sum));
	}

	if (sum == 0) {
		show_err("DATLAT calibration fail, write back to default values!\n");
		io32_write_4b(DRAMC_REG_ADDR(DRAMC_REG_SHU_CONF1),
			prv_register_080);
		set_calibration_result(p, DRAM_CALIBRATION_DATLAT, DRAM_FAIL);
	} else {
		dle_factor_handler(p, best_step, pipe_num);
		set_calibration_result(p, DRAM_CALIBRATION_DATLAT, DRAM_OK);
	}
	}
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_PADCTRL),
		p_fld(SET_FLD, PADCTRL_DQIENQKEND) | p_fld(SET_FLD,
		PADCTRL_DQIENLATEBEGIN));

	show_msg3((INFO, "[DramcRxdatlatCal] Done\n"));

	return sum;
}

void dramc_rxdatlat_cal(DRAMC_CTX_T *p)
{
	show_msg_with_timestamp((INFO, "start Rx DatLat cal\n"));
	dramc_rxdatlat_scan(p, fcDATLAT_USE_DEFAULT);
}

DRAM_STATUS_T dramc_dual_rank_rxdatlat_cal(DRAMC_CTX_T *p)
{
	unsigned char final_datlat, datlat0, datlat1;

	datlat0 = rx_datlat_result[p->channel][0];
	datlat1 = rx_datlat_result[p->channel][1];

	if (datlat0 > datlat1)
		final_datlat = datlat0;
	else
		final_datlat = datlat1;

	dle_factor_handler(p, final_datlat, 3);
	show_msg((INFO,
		"[DualRankRxdatlatCal] RK0: %d, RK1: %d, Final_Datlat %d\n",
		datlat0, datlat1, final_datlat));

	return DRAM_OK;

}
#endif /* SIMULATION_DATLAT */

