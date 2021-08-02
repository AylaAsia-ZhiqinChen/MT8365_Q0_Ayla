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
 * @file dramc_calibration_api.c
 *  Basic DRAMC calibration API implementation
 */

/* -----------------------------------------------------------------------------
 *  Include files
 * -----------------------------------------------------------------------------
 */
#include "dramc_common.h"
#include "x_hal_io.h"
#include "dramc_api.h"

#define OSC_CNT_NUM	2

#define DQSOSCTHRD_INC 0x06
#define DQSOSCTHRD_DEC 0x04

#define PRD_INT	0x3ff
#define DQS_OSC_INT	0x1ff

#define IMPEDANCE_MAX	31

/* Definitions to make IMPCAL_VREF_SEL function more readable */
#define IMPCAL_STAGE_DRVP     1
#define IMPCAL_STAGE_DRVN     2
#define IMPCAL_STAGE_TRACKING 3

/* LP4 IMP_VREF_SEL ============================== */
#define IMP_LP4_VREF_SEL               0x1b

/* LP4X IMP_VREF_SEL ============================== */
#define IMP_LP4X_TERM_VREF_SEL     0x1b

/* LP4X IMP_VREF_SEL w/o term ==== */
#define IMP_DRVP_LP4X_UNTERM_VREF_SEL   0x1a
#define IMP_DRVN_LP4X_UNTERM_VREF_SEL   0x16
#define IMP_TRACK_LP4X_UNTERM_VREF_SEL  0x1a

/* LP4P IMP_VREF_SEL ============================== */
#define IMP_DRVP_LP4P_VREF_SEL        0x13
#define IMP_DRVN_LP4P_VREF_SEL        0xf
#define IMP_TRACK_LP4P_VREF_SEL       0x13

#define IMP_LP3_VREF_SEL		0x2b
#define IMP_DDR4_VREF_SEL		0x16
/* -----------------------------------------------------------------------------
 *  Global variables
 * -----------------------------------------------------------------------------
 */
MR_SET_VALUE_T dram_mr;

/* DQSOSCTHRD_INC & _DEC are 12 bits */
unsigned short dqsosc_thrd_inc[CHANNEL_NUM][RANK_MAX];
unsigned short dqsosc_thrd_dec[CHANNEL_NUM][RANK_MAX];
unsigned short dqsosc[CHANNEL_NUM][RANK_MAX];
unsigned short dqs_osc_cnt_val[CHANNEL_NUM][RANK_MAX][OSC_CNT_NUM];

signed int wl_final_delay[CHANNEL_NUM][DQS_NUMBER];
#ifdef ENABLE_MIOCK_JMETER
unsigned char num_dlycell_per_t[CHANNEL_NUM] = { 49 };
#endif
unsigned char num_dlycell_per_t_all[DRAM_DFS_SHUFFLE_MAX][CHANNEL_NUM];
unsigned short delay_cell_ps[CHANNEL_NUM];
unsigned short delay_cell_ps_all[DRAM_DFS_SHUFFLE_MAX][CHANNEL_NUM];
unsigned int vcore_value[DRAM_DFS_SHUFFLE_MAX];

unsigned int dramc_imp_result[TERM_OPTION_NUM][TERM_TYPE_NUM] = {
	{0, 0, 0, 0}, {0, 0, 0, 0} };

signed char final_k_dqs_clk_delay_cell[DQS_NUMBER];

/* dramc_new_duty_calibration backup register value */
void init_global_variables_by_condition(DRAMC_CTX_T *p)
{
	unsigned char ch_idx, rank_idx, fsp_idx;
	unsigned char vref_unterm;

	if (p->dram_type == TYPE_LPDDR4)
		vref_unterm = 0x4d;
	else
		vref_unterm = 0x5d;

	dram_mr.mr01_value[FSP_0] = MR01_FSP0_INIT;
	dram_mr.mr01_value[FSP_1] = MR01_FSP1_INIT;

	dram_mr.mr13_value[FSP_0] = MR13_FSP0_INIT;
	dram_mr.mr13_value[FSP_1] = MR13_FSP1_INIT;

	for (fsp_idx = 0; fsp_idx < FSP_MAX; fsp_idx++) {
		dram_mr.mr02_value[fsp_idx] = MR02_INIT;
		dram_mr.mr03_value[fsp_idx] = MR03_INIT;
	}

	for (ch_idx = 0; ch_idx < CHANNEL_NUM; ch_idx++) {
		for (rank_idx = 0; rank_idx < RANK_MAX; rank_idx++) {
			for (fsp_idx = 0; fsp_idx < FSP_MAX; fsp_idx++) {
				/*
				* MR14 default value, LP4 default 0x4d,
				* LP4X 0x5d
				*/
				dram_mr.mr14_value[ch_idx][rank_idx][fsp_idx] =
					(fsp_idx == FSP_0) ?
					vref_unterm : MR14_FSP1_INIT;

				/* CA is untermed for all freq */
				dram_mr.mr12_value[ch_idx][rank_idx][fsp_idx] =
					vref_unterm;
			}

			dram_mr.mr23_value[ch_idx][rank_idx] = MR23_INIT;
			dqsosc_thrd_inc[ch_idx][rank_idx] = DQSOSCTHRD_INC;
			dqsosc_thrd_dec[ch_idx][rank_idx] = DQSOSCTHRD_DEC;
		}

	#if SIMULATION_WRITE_LEVELING
		wrlevel_done[ch_idx] = 0;
	#endif
		num_dlycell_per_t[ch_idx] = 49;
	#if GATING_ADJUST_TXDLY_FOR_TRACKING
		tx_dly_cal_min[ch_idx] = BYTE_MAX;
		tx_dly_cal_max[ch_idx] = 0;
	#endif
	}

	memset(rx_dqs_duty_offset, CLEAR_FLD, sizeof(rx_dqs_duty_offset));
}

void set_channel_number(DRAMC_CTX_T *p)
{
#if 0  /* Fix to single Physic channel number */
	#if (CHANNEL_NUM == 4)
	p->support_channel_num = CHANNEL_NUM;	/*4 channel*/
	#elif (CHANNEL_NUM == 2)
	p->support_channel_num = CHANNEL_DUAL;
	#else
	p->support_channel_num = CHANNEL_SINGLE;
	#endif
#endif
}

void set_rank_number(DRAMC_CTX_T *p)
{
#if FOR_DV_SIMULATION_USED
	p->support_rank_num = RANK_SINGLE;
#else
	/* Nothing to do. Decided by CTX?? */
#endif
}

void set_phy_2_channel_mapping(DRAMC_CTX_T *p, unsigned char channel)
{
	p->channel = channel;
}

unsigned char get_phy_2_channel_mapping(DRAMC_CTX_T *p)
{
	return p->channel;
}

void set_rank(DRAMC_CTX_T *p, unsigned char rank)
{
	p->rank = rank;
}

unsigned char get_rank(DRAMC_CTX_T *p)
{
	return p->rank;
}

/*
 * get_dram_cbt_mode
 * Due to current HW design (both ranks share the same set of ACTiming regs),
 * mixed mode LP4 now uses byte mode ACTiming settings. This means most
 * calibration steps should use byte mode code flow.
 * Note: The below items must have per-rank settings (Don't use this function)
 * 1. CBT training 2. TX tracking
 */
DRAM_CBT_MODE_T get_dram_cbt_mode(DRAMC_CTX_T *p)
{
	if (p->dram_cbt_mode[RANK_0] == CBT_NORMAL_MODE
		&& p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE)
		return CBT_NORMAL_MODE;
	else	/* For Mixed mode & Byte mode LP4 */
		return CBT_BYTE_MODE1;
}

#if PRINT_CALIBRATION_SUMMARY
void set_calibration_result(DRAMC_CTX_T *p, unsigned char cal_type,
	unsigned char result)
{
	p->cal_execute_flag[p->channel][p->rank] |= (1 << cal_type);
	if (result == DRAM_OK)
		p->cal_result_flag[p->channel][p->rank] &= (~(1 << cal_type));
	else
		p->cal_result_flag[p->channel][p->rank] |= (1 << cal_type);
}

const char *calib_status_name[DRAM_CALIBRATION_MAX] = {
	"ZQ Calibration",
	"SW Impedance",
	"CA Training",
	"Write leveling",
	"RX DQS gating",
	"RX DATLAT",
	"RX DQ/DQS(RDDQC)",
	"RX DQ/DQS(Engine)",
	"TX DQ/DQS",
};
void print_calibration_result2(DRAMC_CTX_T *p,
	unsigned char ch_idx, unsigned char rank_idx)
{
	unsigned char cal_idx;
	unsigned int cal_result_all, cal_execute_all;
	unsigned char cal_result, cal_execute;
	unsigned char calibration_fail = 0;

	cal_execute_all =
		p->cal_execute_flag[ch_idx][rank_idx];
	cal_result_all = p->cal_result_flag[ch_idx][rank_idx];
	show_msg((INFO, "CH %d, Rank %d\n", ch_idx,
		rank_idx));

	for (cal_idx = 0; cal_idx < DRAM_CALIBRATION_MAX; cal_idx++) {
		cal_execute = (unsigned char)
			((cal_execute_all >> cal_idx) & 0x1);
		cal_result = (unsigned char)
			((cal_result_all >> cal_idx) & 0x1);

		/* excuted and fail */
		if (cal_execute == 1 && cal_result == 1) {
			calibration_fail = 1;
			show_msg((INFO, "%s: %s\n",
				calib_status_name[cal_idx],
				((cal_result == 0) ? "OK" : "Fail")));
		}
	}

	if (calibration_fail == 0)
		show_msg((INFO, "All Pass.\n"));
	show_msg((INFO, "\n"));
}

void print_calibration_result(DRAMC_CTX_T *p)
{
	unsigned char ch_idx, rank_idx;

	show_msg((INFO, "\n\n[Calibration Summary] Freqency %d\n",
		p->frequency));

	for (ch_idx = 0; ch_idx < p->support_channel_num; ch_idx++)
		for (rank_idx = 0; rank_idx < p->support_rank_num; rank_idx++)
			print_calibration_result2(p, ch_idx, rank_idx);

	memset(p->cal_result_flag, CLEAR_FLD, sizeof(p->cal_result_flag));
	memset(p->cal_execute_flag, CLEAR_FLD, sizeof(p->cal_execute_flag));
}
#endif

void print_calibration_basic_info(DRAMC_CTX_T *p)
{
#ifndef PARALLEL_CH_CAL
	show_msg((INFO,
		"==========================================================\n"
		"Dram Type= %d, Freq= %u, FreqGroup= %u, CH_%d, rank %d\n"
		"fsp= %d, odt_onoff= %d, Byte mode= %d, divmode = %d, is_emcp = %d\n"
		"==========================================================\n",
		p->dram_type, p->frequency, p->freqGroup, p->channel,
		p->rank, p->dram_fsp, p->odt_onoff,
		p->dram_cbt_mode[p->rank], get_div_mode(p), p->is_emcp));
#endif
}

void print_calibration_basic_info_diag(DRAMC_CTX_T *p)
{
	show_diag((CRITICAL, "\n==========================================================\n"));
	show_diag((CRITICAL, "Dram Type= %d, Freq= %u, FreqGroup= %u, CH_%d, rank %d\n",
		p->dram_type, p->frequency, p->freqGroup, p->channel, p->rank));
	show_diag((CRITICAL, "fsp= %d, odt_onoff= %d, Byte mode= %d, divmode = %d\n",
		p->dram_fsp, p->odt_onoff, p->dram_cbt_mode[p->rank], get_div_mode(p)));
	show_diag((CRITICAL, "==========================================================\n"));
}

void apply_config_after_calibration(DRAMC_CTX_T *p)
{
	unsigned int shu_index;

	if (p->dram_type == TYPE_PCDDR4) {
		io_32_write_fld_align_all(DRAMC_REG_DRAMC_PD_CTRL, SET_FLD,
			DRAMC_PD_CTRL_PHYCLKDYNGEN);
	}
	/* PHY RX Settings */
	io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL4, 0x11400000,
		MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);
	io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL7, 0x20,
		MISC_CG_CTRL7_R_PHY_MCK_CG_CTRL_2);

	/* Burst mode settings are removed from here due to
	*  1. Set in update_initial_settings_lp4
	*  2. DQS Gating ensures new burst mode is switched when to done
	*  (or doesn't switch gatingMode at all, depending on
	*  "LP4_GATING_OLD_BURST_MODE")
	*/

	io_32_write_fld_align_all(DDRPHY_CA_CMD6, CLEAR_FLD,
		CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN);
	/* DA mode */
	io_32_write_fld_align_all(DDRPHY_B0_DQ6, CLEAR_FLD,
		B0_DQ6_RG_RX_ARDQ_BIAS_PS_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ6, CLEAR_FLD,
		B1_DQ6_RG_RX_ARDQ_BIAS_PS_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ6, CLEAR_FLD,
		B2_DQ6_RG_RX_ARDQ_BIAS_PS_B2);
	io_32_write_fld_align_all(DDRPHY_CA_CMD6, CLEAR_FLD,
		CA_CMD6_RG_RX_ARCMD_BIAS_PS);

	io_32_write_fld_align_all(DDRPHY_B0_DQ6, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ6, SET_FLD,
		B1_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ6, SET_FLD,
		B2_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B2);

	io_32_write_fld_align_all(DDRPHY_CA_CMD6, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_RPRE_TOG_EN);

	/* IMPCAL Settings */
	/* RG_RIMP_BIAS_EN and RG_RIMP_VREF_EN move to IMPPDP and IMPPDN */
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL),
		p_fld(CLEAR_FLD, IMPCAL_IMPCAL_IMPPDP) |
		p_fld(CLEAR_FLD, IMPCAL_IMPCAL_IMPPDN));

	/*
	* MR1 op[7]=0 already be setted at end of gating calibration,
	* no need to set here again
	* Prevent M_CK OFF because of hardware auto-sync
	*/
	io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL0, CLEAR_FLD,
		Fld(4, 0));

	/*DFS- fix Gating Tracking settings*/
	io_32_write_fld_align_all(DDRPHY_MISC_CTRL0, CLEAR_FLD,
		MISC_CTRL0_R_STBENCMP_DIV4CK_EN);
	io_32_write_fld_align_all(DDRPHY_MISC_CTRL1, CLEAR_FLD,
		MISC_CTRL1_R_DMSTBENCMP_RK_OPT);

	/* TODO: Disable MR4 MR18/MR19, TxHWTracking, Dummy RD */
	/* MR4 Disable */
	io_32_write_fld_align_all(DRAMC_REG_SPCMDCTRL, SET_FLD,
		SPCMDCTRL_REFRDIS);
	/* MR18, MR19 Disable */
	io_32_write_fld_align_all(DRAMC_REG_DQSOSCR, SET_FLD,
		DQSOSCR_DQSOSCRDIS);
	for(shu_index = DRAM_DFS_SHUFFLE_1; shu_index < DRAM_DFS_SHUFFLE_MAX; shu_index++)
		io_32_write_fld_align_all(DRAMC_REG_SHU_SCINTV * shu_index, SET_FLD,
			SHU_SCINTV_DQSOSCENDIS);

	io_32_write_fld_multi_all(DRAMC_REG_DUMMY_RD,
		p_fld(CLEAR_FLD, DUMMY_RD_DUMMY_RD_EN)
		| p_fld(SET_FLD, DUMMY_RD_SREF_DMYRD_EN)
		| p_fld(CLEAR_FLD, DUMMY_RD_DQSG_DMYRD_EN)
		| p_fld(CLEAR_FLD, DUMMY_RD_DMY_RD_DBG));
	if (p->dram_type == TYPE_PCDDR3 || p->dram_type == TYPE_LPDDR3) {
		io_32_write_fld_align_all(DRAMC_REG_DUMMY_RD,
			SET_FLD, DUMMY_RD_SREF_DMYRD_EN);
	}
	/* CKE dynamic */
	cke_fix_on_off(p, CKE_WRITE_TO_ALL_RANK, CKE_DYNAMIC, CKE_WRITE_TO_ALL_CHANNEL);
	/* Enable  HW MIOCK control to make CLK dynamic */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL),
		CLEAR_FLD, DRAMC_PD_CTRL_MIOCKCTRLOFF);

	/* close eyescan to save power */
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_EYESCAN),
		p_fld(CLEAR_FLD, EYESCAN_EYESCAN_DQS_SYNC_EN)
		| p_fld(CLEAR_FLD, EYESCAN_EYESCAN_NEW_DQ_SYNC_EN)
		| p_fld(CLEAR_FLD, EYESCAN_EYESCAN_DQ_SYNC_EN));

	/* TESTAGENT2 */
	/* Rank selection is controlled by Test Agent */
	io_32_write_fld_align_all(DRAMC_REG_TEST2_4, 4,
		TEST2_4_TESTAGENTRKSEL);

	/* cc add after MP compare */
	io_32_write_fld_align(DDRPHY_B0_DQ6, CLEAR_FLD,
		B0_DQ6_RG_RX_ARDQ_O1_SEL_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ6, CLEAR_FLD,
		B1_DQ6_RG_RX_ARDQ_O1_SEL_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ6, CLEAR_FLD,
		B2_DQ6_RG_RX_ARDQ_O1_SEL_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD6, CLEAR_FLD,
		CA_CMD6_RG_RX_ARCMD_O1_SEL);

	/* After calibration, auto refresh shall be enabled */
	io_32_write_fld_align_all(DRAMC_REG_REFCTRL0, CLEAR_FLD,
		REFCTRL0_REFDIS);

	if (p->dram_type == TYPE_PCDDR4 || p->dram_type == TYPE_LPDDR3) {
		io_32_write_fld_multi_all(DDRPHY_MISC_CG_CTRL5,
			p_fld(0x1, MISC_CG_CTRL5_R_DQ2_DLY_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_CA_DLY_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ0_DLY_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ1_DLY_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ2_PI_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_CA_PI_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ0_PI_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ1_PI_DCM_EN));
	}
}

void reset_delay_chain_before_calibration(DRAMC_CTX_T *p)
{
	unsigned char rank_idx, rank_idx_bak;

	rank_idx_bak = get_rank(p);

	/* Set LP4 Rank0/1 CA/TX delay chain to 0 */
	/*
	 * CA0~9 per bit delay line -> CHA_CA0 CHA_CA3 CHA_B0_DQ6
	 * CHA_B0_DQ7 CHA_B0_DQ2 CHA_B0_DQ5 CHA_B0_DQ4 CHA_B0_DQ1
	 * CHA_B0_DQ0 CHA_B0_DQ3
	 */
	for (rank_idx = RANK_0; rank_idx < p->support_rank_num; rank_idx++) {
		set_rank(p, rank_idx);

		io_32_write_fld_multi_all(DDRPHY_SHU_R0_CA_CMD0,
			p_fld(CLEAR_FLD, SHU_R0_CA_CMD0_RK0_TX_ARCA5_DLY) |
			p_fld(CLEAR_FLD, SHU_R0_CA_CMD0_RK0_TX_ARCA4_DLY) |
			p_fld(CLEAR_FLD, SHU_R0_CA_CMD0_RK0_TX_ARCA3_DLY) |
			p_fld(CLEAR_FLD, SHU_R0_CA_CMD0_RK0_TX_ARCA2_DLY) |
			p_fld(CLEAR_FLD, SHU_R0_CA_CMD0_RK0_TX_ARCA1_DLY) |
			p_fld(CLEAR_FLD, SHU_R0_CA_CMD0_RK0_TX_ARCA0_DLY));
		io_32_write_fld_multi_all(DDRPHY_SHU_R0_B0_DQ0,
			p_fld(CLEAR_FLD, SHU_R0_B0_DQ0_RK0_TX_ARDQ7_DLY_B0) |
			p_fld(CLEAR_FLD, SHU_R0_B0_DQ0_RK0_TX_ARDQ6_DLY_B0) |
			p_fld(CLEAR_FLD, SHU_R0_B0_DQ0_RK0_TX_ARDQ5_DLY_B0) |
			p_fld(CLEAR_FLD, SHU_R0_B0_DQ0_RK0_TX_ARDQ4_DLY_B0) |
			p_fld(CLEAR_FLD, SHU_R0_B0_DQ0_RK0_TX_ARDQ3_DLY_B0) |
			p_fld(CLEAR_FLD, SHU_R0_B0_DQ0_RK0_TX_ARDQ2_DLY_B0) |
			p_fld(CLEAR_FLD, SHU_R0_B0_DQ0_RK0_TX_ARDQ1_DLY_B0) |
			p_fld(CLEAR_FLD, SHU_R0_B0_DQ0_RK0_TX_ARDQ0_DLY_B0));
		io_32_write_fld_multi_all(DDRPHY_SHU_R0_B1_DQ0,
			p_fld(CLEAR_FLD, SHU_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1) |
			p_fld(CLEAR_FLD, SHU_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1) |
			p_fld(CLEAR_FLD, SHU_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1) |
			p_fld(CLEAR_FLD, SHU_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1) |
			p_fld(CLEAR_FLD, SHU_R0_B1_DQ0_RK0_TX_ARDQ3_DLY_B1) |
			p_fld(CLEAR_FLD, SHU_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1) |
			p_fld(CLEAR_FLD, SHU_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1) |
			p_fld(CLEAR_FLD, SHU_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1));
		io_32_write_fld_align_all(DDRPHY_SHU_R0_B0_DQ1, CLEAR_FLD,
			SHU_R0_B0_DQ1_RK0_TX_ARDQM0_DLY_B0);
		io_32_write_fld_align_all(DDRPHY_SHU_R0_B1_DQ1, CLEAR_FLD,
			SHU_R0_B1_DQ1_RK0_TX_ARDQM0_DLY_B1);
	}

	set_rank(p, rank_idx_bak);
}

static void apply_config_before_calibration1(DRAMC_CTX_T *p)
{
	unsigned char shu_index;

	/* Clk free run */
	enable_dramc_phy_dcm(p, 0);

	reset_delay_chain_before_calibration(p);

	/* MR4 refresh cnt set to 0x1ff (2ms update) */
	io_32_write_fld_align_all(DRAMC_REG_SHU_CONF3, 0x1ff,
		SHU_CONF3_REFRCNT);

	/*
	* The counter for Read MR4 cannot be reset after SREF
	* if DRAMC no power down.
	*/

	/* ---- ZQ CS init -------- */
	/*
	* ZQ Calibration Time, unit: 38.46ns, tZQCAL min is 1 us.
	* need to set larger than 0x1b
	*/
	io_32_write_fld_align_all(DRAMC_REG_SHU_SCINTV,
		0x1f, SHU_SCINTV_TZQLAT);
	if (p->dram_type == TYPE_PCDDR3) {
		io_32_write_fld_align_all(DRAMC_REG_SHU_SCINTV,
			0x0, SHU_SCINTV_TZQLAT);
	} else if (p->dram_type == TYPE_LPDDR3) {
		io_32_write_fld_align_all(DRAMC_REG_SHU_SCINTV,
			0x1b, SHU_SCINTV_TZQLAT);
	}
	for (shu_index = DRAM_DFS_SHUFFLE_1; shu_index < DRAM_DFS_SHUFFLE_MAX;
		shu_index++) {
		/* Every refresh number to issue ZQCS commands */
		io_32_write_fld_align_all(
			DRAMC_REG_SHU_CONF3 + SHU_GRP_DRAMC_OFFSET * shu_index,
			0x1ff, SHU_CONF3_ZQCSCNT);
	}
	/*
	* HW send ZQ command for both rank,
	* disable it due to some dram only have 1 ZQpin for two rank.
	*/
	io_32_write_fld_align_all(DRAMC_REG_DRAMCTRL,
		CLEAR_FLD, DRAMCTRL_ZQCALL);

	/* Dual channel ZQCS interlace,  0: disable, 1: enable */
	if (p->support_channel_num == CHANNEL_SINGLE) {
		/* single channel, ZQCSDUAL=0, ZQCSMASK=0 */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_ZQCS),
			p_fld(CLEAR_FLD, ZQCS_ZQCSDUAL) |
			p_fld(CLEAR_FLD, ZQCS_ZQCSMASK));
	} else if (p->support_channel_num == CHANNEL_DUAL) {
#ifdef ZQCS_ENABLE_LP4
		io_32_write_fld_multi_all(DRAMC_REG_ZQCS,
			p_fld(1, ZQCS_ZQCSDUAL) |
			p_fld(0, ZQCS_ZQCSMASK_OPT) |
			p_fld(0, ZQCS_ZQMASK_CGAR) |
			p_fld(0, ZQCS_ZQCS_MASK_SEL_CGAR));

		/* DRAMC CHA(CHN0):ZQCSMASK=1, DRAMC CHB(CHN1):ZQCSMASK=0.
		 *  ZQCSMASK setting: (Ch A, Ch B) = (1,0) or (0,1)
		 * if CHA.ZQCSMASK=1, and then set CHA.ZQCALDISB=1 first, else set CHB.ZQCALDISB=1 first
		 */
		io_32_write_fld_align(DRAMC_REG_ZQCS + (CHANNEL_A<< POS_BANK_NUM), 0, ZQCS_ZQCSMASK);
		io_32_write_fld_align(DRAMC_REG_ZQCS + SHIFT_TO_CHB_ADDR, 1, ZQCS_ZQCSMASK);

		io_32_write_fld_align_all(DRAMC_REG_ZQCS, 0, ZQCS_ZQCS_MASK_SEL);
#endif
	}

	/* Disable LP3 HW ZQ, LP3 ZQCSDISB=0 */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL),
		CLEAR_FLD, SPCMDCTRL_ZQCSDISB);
	/* Disable LP4 HW ZQ,LP4 ZQCALDISB=0 */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL),
		CLEAR_FLD, SPCMDCTRL_ZQCALDISB);
	/* ---- End of ZQ CS init ----- */

}

void apply_config_before_calibration(DRAMC_CTX_T *p)
{
	unsigned char rank_idx, rank_idx_bak;
	unsigned char shu_index;

	rank_idx_bak = get_rank(p);

	apply_config_before_calibration1(p);

#if ENABLE_TX_TRACKING
	io_32_write_fld_align_all(DRAMC_REG_DQSOSCR, p->dram_cbt_mode[RANK_0],
		DQSOSCR_RK0_BYTE_MODE);
	io_32_write_fld_align_all(DRAMC_REG_DQSOSCR, p->dram_cbt_mode[RANK_1],
		DQSOSCR_RK1_BYTE_MODE);
#endif
	/* Disable write-DBI of DRAMC */
	dramc_write_dbi_on_off(p, DBI_OFF);
	/* Disable read-DBI of DRAMC */
	dramc_read_dbi_on_off(p, DBI_OFF);
	/* disable MR4 read, REFRDIS=1 */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL),
		SET_FLD, SPCMDCTRL_REFRDIS);
	/* MR18, MR19 Disable */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR),
		SET_FLD, DQSOSCR_DQSOSCRDIS);

	for(shu_index = DRAM_DFS_SHUFFLE_1; shu_index < DRAM_DFS_SHUFFLE_MAX; shu_index++)
		io_32_write_fld_align_all(DRAMC_REG_SHU_SCINTV * shu_index, SET_FLD,
			SHU_SCINTV_DQSOSCENDIS);

	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD),
		p_fld(CLEAR_FLD, DUMMY_RD_DUMMY_RD_EN) |
		p_fld(CLEAR_FLD, DUMMY_RD_SREF_DMYRD_EN) |
		p_fld(CLEAR_FLD, DUMMY_RD_DQSG_DMYRD_EN) |
		p_fld(CLEAR_FLD, DUMMY_RD_DMY_RD_DBG));

	/*
	 * Disable HW gating tracking first, 0x1c0[31], need to disable
	 * both UI and PI tracking or the gating delay reg won't be valid.
	 */
	dramc_hw_gating_on_off(p, 0);

	/* ARPI_DQ SW mode mux, TX DQ use 1: PHY Reg 0: DRAMC Reg */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1),
		SET_FLD, MISC_CTRL1_R_DMARPIDQ_SW);

	/* Set to all-bank refresh */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0),
		CLEAR_FLD, REFCTRL0_PBREFEN);

	/* set MRSRK to 0, MPCRKEN always set 1 (Derping) */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_MRS),
		CLEAR_FLD, MRS_MRSRK);
	if (is_lp4_family(p)) {
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION),
			SET_FLD, MPC_OPTION_MPCRKEN);
	} else {
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION),
			CLEAR_FLD, MPC_OPTION_MPCRKEN);
	}
	/* RG mode */
	io_32_write_fld_align_all(DDRPHY_B0_DQ6, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_BIAS_PS_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ6, SET_FLD,
		B1_DQ6_RG_RX_ARDQ_BIAS_PS_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ6, SET_FLD,
		B2_DQ6_RG_RX_ARDQ_BIAS_PS_B2);
	io_32_write_fld_align_all(DDRPHY_CA_CMD6, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_BIAS_PS);
	if (p->dram_type == TYPE_PCDDR4) {
		io_32_write_fld_multi_all(DDRPHY_MISC_CG_CTRL5,
			p_fld(0x0, MISC_CG_CTRL5_R_DQ2_DLY_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_CA_DLY_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ0_DLY_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ1_DLY_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ2_PI_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_CA_PI_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ0_PI_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ1_PI_DCM_EN));
	}

#if ENABLE_RX_TRACKING_LP4
	dramc_rx_input_delay_tracking_init_by_freq(p);
#endif

#ifdef DUMMY_READ_FOR_TRACKING
	io_32_write_fld_align_all(DRAMC_REG_DUMMY_RD, SET_FLD,
		DUMMY_RD_DMY_RD_RX_TRACK);
#endif

    io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL5, 0x0,
    MISC_CG_CTRL5_R_DVS_DIV4_CG_CTRL_B2_13);

}

/* cc notes: From M*/
void dram_phy_reset(DRAMC_CTX_T *p)
{
	if (p->data_width == DATA_WIDTH_32BIT) {
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_DDRCONF0), SET_FLD,
			DDRCONF0_RDATRST);
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), SET_FLD,
			MISC_CTRL1_R_DMPHYRST);

		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD10),
			p_fld(CLEAR_FLD, CA_CMD10_RG_RX_ARCLK_STBEN_RESETB) |
			p_fld(CLEAR_FLD, CA_CMD10_RG_RX_ARCMD_STBEN_RESETB));
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ9),
			p_fld(CLEAR_FLD, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) |
			p_fld(CLEAR_FLD, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0));
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ9),
			p_fld(CLEAR_FLD, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) |
			p_fld(CLEAR_FLD, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1));
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ9),
			p_fld(CLEAR_FLD, B2_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B2) |
			p_fld(CLEAR_FLD, B2_DQ9_RG_RX_ARDQ_STBEN_RESETB_B2));
		delay_us(1);
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ9),
			p_fld(SET_FLD, B2_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B2) |
			p_fld(SET_FLD, B2_DQ9_RG_RX_ARDQ_STBEN_RESETB_B2));
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ9),
			p_fld(SET_FLD, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) |
			p_fld(SET_FLD, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1));
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ9),
			p_fld(SET_FLD, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) |
			p_fld(SET_FLD, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0));

		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD10),
			p_fld(SET_FLD, CA_CMD10_RG_RX_ARCLK_STBEN_RESETB) |
			p_fld(SET_FLD, CA_CMD10_RG_RX_ARCMD_STBEN_RESETB));
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), CLEAR_FLD,
			MISC_CTRL1_R_DMPHYRST);
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_DDRCONF0), CLEAR_FLD,
			DDRCONF0_RDATRST);
	} else  {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DDRCONF0), SET_FLD,
			DDRCONF0_RDATRST);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), SET_FLD,
			MISC_CTRL1_R_DMPHYRST);

		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B0_DQ9),
			p_fld(CLEAR_FLD, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) |
			p_fld(CLEAR_FLD, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B1_DQ9),
			p_fld(CLEAR_FLD, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) |
			p_fld(CLEAR_FLD, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B2_DQ9),
			p_fld(CLEAR_FLD, B2_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B2) |
			p_fld(CLEAR_FLD, B2_DQ9_RG_RX_ARDQ_STBEN_RESETB_B2));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_CA_CMD10),
			p_fld(CLEAR_FLD, CA_CMD10_RG_RX_ARCLK_STBEN_RESETB) |
			p_fld(CLEAR_FLD, CA_CMD10_RG_RX_ARCMD_STBEN_RESETB));
		delay_us(1);
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_CA_CMD10),
			p_fld(SET_FLD, CA_CMD10_RG_RX_ARCLK_STBEN_RESETB) |
			p_fld(SET_FLD, CA_CMD10_RG_RX_ARCMD_STBEN_RESETB));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B2_DQ9),
			p_fld(SET_FLD, B2_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B2) |
			p_fld(SET_FLD, B2_DQ9_RG_RX_ARDQ_STBEN_RESETB_B2));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B1_DQ9),
			p_fld(SET_FLD, B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1) |
			p_fld(SET_FLD, B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B0_DQ9),
			p_fld(SET_FLD, B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0) |
			p_fld(SET_FLD, B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0));

		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), CLEAR_FLD,
			MISC_CTRL1_R_DMPHYRST);
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DDRCONF0), CLEAR_FLD,
			DDRCONF0_RDATRST);
	}
}

DRAM_STATUS_T dramc_rank_swap(DRAMC_CTX_T *p, unsigned char rank)
{
	return DRAM_OK;
}

DRAM_STATUS_T dramc_start_dqsosc(DRAMC_CTX_T *p)
{
	unsigned int response;
	unsigned int time_cnt;

	time_cnt = TIME_OUT_CNT;
	show_msg((INFO, "[DQSOSC]\n"));

	/*
	 * R_DMDQSOSCENEN, 0x1E4[10]=1 for DQSOSC Start
	 * Wait dqsoscen_response=1 (dramc_conf_nao, 0x3b8[29])
	 * R_DMDQSOSCENEN, 0x1E4[10]=0
	 */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), SET_FLD,
		SPCMD_DQSOSCENEN);
	do {
		response =
			io_32_read_fld_align(
			DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP),
			SPCMDRESP_DQSOSCEN_RESPONSE);
		time_cnt--;
		delay_us(1);
	} while ((response == 0) && (time_cnt > 0));

	if (time_cnt == 0)	{
		show_err(("Start fail (time out)\n"));
		return DRAM_FAIL;
	}
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD),
		CLEAR_FLD, SPCMD_DQSOSCENEN);
	return DRAM_OK;
}

DRAM_STATUS_T dramc_dqsosc_auto(DRAMC_CTX_T *p)
{
	unsigned char mr23 = dram_mr.mr23_value[p->channel][p->rank];
	unsigned short mr18, mr19;
	unsigned short dqs_cnt;
	unsigned short dqs_osc[2];
	unsigned int reg_bak[3];
	unsigned char shu_index;

	reg_bak[0] = io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_MRS));
	reg_bak[1] = io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL));
	reg_bak[2] = io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL));

	/* !!R_DMMRSRK(R_DMMPCRKEN=1) specify rank0 or rank1 */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG), CLEAR_FLD,
		RKCFG_DQSOSC2RK);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), get_rank(p),
		MRS_MRSRK);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION), SET_FLD,
		MPC_OPTION_MPCRKEN);

	/*
	 * LPDDR4-3200,     PI resolution = tCK/64 =9.76ps
	 * Only if MR23>=16, then error < PI resolution.
	 * Set MR23 == 0x3f, stop after 63*16 clock
	 */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), get_rank(p),
		MRS_MRSRK);
	dramc_mode_reg_write(p, MR23, mr23);

	for (shu_index = DRAM_DFS_SHUFFLE_1; shu_index < DRAM_DFS_SHUFFLE_MAX;
		shu_index++) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_SCINTV) +
			SHU_GRP_DRAMC_OFFSET * shu_index, SET_FLD,
			SHU_SCINTV_DQSOSCENDIS);
	}

	/* MIOCKCTRLOFF=1 */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL),
		SET_FLD, DRAMC_PD_CTRL_MIOCKCTRLOFF);

	cke_fix_on_off(p, p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	dramc_start_dqsosc(p);
	delay_us(1);
#if ENABLE_TMRRI_NEW_MODE
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), get_rank(p),
		MRS_MRSRK);
#else
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), get_rank(p),
		MRS_MRRRK);
#endif
	dramc_mode_reg_read(p, MR18, &mr18);
	dramc_mode_reg_read(p, MR19, &mr19);

	/* B0 */
	dqs_cnt = (mr18 & 0x00ff) | ((mr19 & 0x00ff) << 8);
	if (dqs_cnt != 0) /* tDQSOSC = 16*MR23*tCK/2*count */
		dqs_osc[0] = mr23 * 16 * 1000000 / (2 * dqs_cnt * p->frequency);
	else
		dqs_osc[0] = 0;

	/* B1 */
	dqs_cnt = (mr18 >> 8) | ((mr19 & 0xff00));
	if (dqs_cnt != 0) /* tDQSOSC = 16*MR23*tCK/2*count */
		dqs_osc[1] = mr23 * 16 * 1000000 / (2 * dqs_cnt * p->frequency);
	else
		dqs_osc[1] = 0;
	show_msg2((INFO,
		"%s%d%s0x%x, (MSB)MR19=0x%x,%s%dps tDQSOscB1=%dps\n",
		"[DQSOSCAuto] RK", get_rank(p), ", (LSB)MR18=", mr18,
		mr19, " tDQSOscB0=", dqs_osc[0], dqs_osc[1]));

	dram_mr.mr18_value[p->channel][p->rank] = mr18;
	dram_mr.mr19_value[p->channel][p->rank] = mr19;
	dqsosc[p->channel][p->rank] = dqs_osc[0];

	if (dqs_osc[1] != 0 && dqs_osc[1] < dqs_osc[0])
		dqsosc[p->channel][p->rank] = dqs_osc[1];

	io32_write_4b(DRAMC_REG_ADDR(DRAMC_REG_MRS), reg_bak[0]);
	io32_write_4b(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), reg_bak[1]);
	io32_write_4b(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), reg_bak[2]);

	/* MR23 should be 0x3F for all case ("HW tracking modify" DVT)
	 * -> Value is already set in during mr23_value array init
	 * (Adjusts MR23 according to freq, since back then _INC _DEC bit's
	 * weren't sufficient)
	 */

	return DRAM_OK;
}

/* Using dqsosc results calculated from dramc_dqsosc_auto
 * -> calculate DQSOSCTHRD_INC, DQSOSCTHRD_DEC
 */
DRAM_STATUS_T dramc_dqsosc_mr23(DRAMC_CTX_T *p)
{
	unsigned char mr23 = dram_mr.mr23_value[p->channel][p->rank];
	unsigned short dqs_osc = dqsosc[p->channel][p->rank];
	unsigned int tck = 1000000 / p->frequency;

	if (dqs_osc != 0) {
	dqsosc_thrd_inc[p->channel][p->rank] =
			(3 * mr23 * tck * tck) / (dqs_osc * dqs_osc * 20);
		dqsosc_thrd_dec[p->channel][p->rank] =
		(mr23 * tck * tck) / (dqs_osc * dqs_osc * 10);
	}
	show_msg((INFO,
		"CH%d_RK%d: MR19=0x%X, MR18=0x%X%s%d, MR23=%u%s%u, DEC=%u\n",
		p->channel, p->rank, dram_mr.mr19_value[p->channel][p->rank],
		dram_mr.mr18_value[p->channel][p->rank], ", DQSOSC=",
		dqsosc[p->channel][p->rank], mr23, ", INC=",
		dqsosc_thrd_inc[p->channel][p->rank],
		dqsosc_thrd_dec[p->channel][p->rank]));
	return DRAM_OK;
}

/* Sets DQSOSC_BASE for specified rank/byte */
DRAM_STATUS_T dramc_dqsosc_set_mr18_mr19(DRAMC_CTX_T *p)
{
	unsigned short dqs_osc_cnt[2];

	dramc_dqsosc_auto(p);

	/* B0 */
	dqs_osc_cnt_val[p->channel][p->rank][0] = dqs_osc_cnt[0] =
		(dram_mr.mr18_value[p->channel][p->rank] & 0x00ff) |
		((dram_mr.mr19_value[p->channel][p->rank] & 0x00ff) << 8);
	/* B1 */
	dqs_osc_cnt_val[p->channel][p->rank][1] = dqs_osc_cnt[1] =
		(dram_mr.mr18_value[p->channel][p->rank] >> 8) |
		((dram_mr.mr19_value[p->channel][p->rank] & 0xff00));

	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSOSC),
		p_fld(dqs_osc_cnt[0], SHURK0_DQSOSC_DQSOSC_BASE_RK0) |
		p_fld(dqs_osc_cnt[1], SHURK0_DQSOSC_DQSOSC_BASE_RK0_B1));

	show_msg((INFO, "CH%d RK%d: MR19=%X, MR18=%X\n", p->channel,
			p->rank, dram_mr.mr19_value[p->channel][p->rank],
			dram_mr.mr18_value[p->channel][p->rank]));
	return DRAM_OK;
}

DRAM_STATUS_T dramc_dqsosc_shu_settings(DRAMC_CTX_T *p)
{
	unsigned short prd_cnt = 0x3FF;
	unsigned int prd_cnt_div = 4;
	unsigned short dqsosc_en_cnt = 0x1FF;
	unsigned short third_inc, third_dec;
	unsigned char filt_pithrd = 0;
	unsigned char w2r_sel = 0;

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_SCINTV),
		0x0, SHU_SCINTV_DQS2DQ_SHU_PITHRD);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_DQSOSC),
		0x0, RK0_DQSOSC_R_DMDQS2DQ_FILT_OPT);

	if (p->frequency <= 400) {
		filt_pithrd = 0x4;
		w2r_sel = 0x2;
	} else if (p->frequency <= 600) {
		filt_pithrd = 0x6;
		w2r_sel = 0x2;
	} else if( p->frequency <= 800) {
		filt_pithrd = 0x7;
		if (get_div_mode(p) == DIV4_MODE)
			w2r_sel = 0x2;
		else
			w2r_sel = 0x5;
	} else if( p->frequency <= 1200) {
		filt_pithrd = 0xb;
		w2r_sel = 0x2;
	} else if( p->frequency <= 1333) {
		filt_pithrd = 0xc;
		w2r_sel = 0x2;
	} else if( p->frequency <= 1600) {
		filt_pithrd = 0xE;
		w2r_sel = 0x2;
	} else if(p->frequency <= 1866) {
		filt_pithrd = 0x12;
		w2r_sel = 0x2;
	} else {
		filt_pithrd = 0x15;
		w2r_sel = 0x2;
	}

	if (get_div_mode(p) == DIV4_MODE)
		prd_cnt_div = 2;

	prd_cnt = (dram_mr.mr23_value[p->channel][RANK_0]/prd_cnt_div)+3;
	if (p->support_rank_num==RANK_DUAL) {
		if(dram_mr.mr23_value[p->channel][RANK_0] > dram_mr.mr23_value[p->channel][RANK_1])
			prd_cnt = (dram_mr.mr23_value[p->channel][RANK_0]/prd_cnt_div)+3;
		else
			prd_cnt = (dram_mr.mr23_value[p->channel][RANK_1]/prd_cnt_div)+3;
	}

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSC_PRD),
		prd_cnt, SHU_DQSOSC_PRD_DQSOSC_PRDCNT);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSCR),
		((p->frequency-1)/100)+1, SHU_DQSOSCR_DQSOSCRCNT);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_SCINTV),
		filt_pithrd, SHU_SCINTV_DQS2DQ_FILT_PITHRD);

	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_WODT),
		p_fld(w2r_sel, SHU_WODT_TXUPD_W2R_SEL) |
		p_fld(0x0, SHU_WODT_TXUPD_SEL));

	third_inc = dqsosc_thrd_inc[p->channel][RANK_0];
	third_dec = dqsosc_thrd_dec[p->channel][RANK_0];
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSCTHRD),
		third_inc, SHU_DQSOSCTHRD_DQSOSCTHRD_INC_RK0);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSCTHRD),
		third_dec, SHU_DQSOSCTHRD_DQSOSCTHRD_DEC_RK0);

	third_inc = dqsosc_thrd_inc[p->channel][RANK_1];
	third_dec = dqsosc_thrd_dec[p->channel][RANK_1];

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSCTHRD),
		(third_inc & 0x0FF), SHU_DQSOSCTHRD_DQSOSCTHRD_INC_RK1_7TO0);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSC_PRD),
		((third_inc & 0xF00) >> 8), SHU_DQSOSC_PRD_DQSOSCTHRD_INC_RK1_11TO8);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSC_PRD),
		third_dec, SHU_DQSOSC_PRD_DQSOSCTHRD_DEC_RK1);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSOSCR2),
		dqsosc_en_cnt, SHU_DQSOSCR2_DQSOSCENCNT);

	return DRAM_OK;
}


#if ENABLE_TX_TRACKING
void dramc_hwdqsosc_set_freq_ratio(DRAMC_CTX_T *p)
{
	/* for SHUFFLE_1 */
#if SUPPORT_TYPE_LPDDR4
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_RK2_DQSOSC),
		p_fld((unsigned short)
		(freq_tbl_lp4[1].frequency * 8 / freq_tbl_lp4[0].frequency),
		RK2_DQSOSC_FREQ_RATIO_TX_0) |
		p_fld((unsigned short)
		(freq_tbl_lp4[2].frequency * 8 / freq_tbl_lp4[0].frequency),
		RK2_DQSOSC_FREQ_RATIO_TX_1));
	/* for SHUFFLE_2 */
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_RK2_DQSOSC),
		p_fld((unsigned short)
		(freq_tbl_lp4[0].frequency * 8 / freq_tbl_lp4[1].frequency),
		RK2_DQSOSC_FREQ_RATIO_TX_3) |
		p_fld((unsigned short)
		(freq_tbl_lp4[2].frequency * 8 / freq_tbl_lp4[1].frequency),
		RK2_DQSOSC_FREQ_RATIO_TX_4));
	/* for SHUFFLE_3 */
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_RK2_DUMMY_RD_BK),
		p_fld((unsigned short)
		(freq_tbl_lp4[0].frequency * 8 / freq_tbl_lp4[2].frequency),
		RK2_DUMMY_RD_BK_FREQ_RATIO_TX_6) |
		p_fld((unsigned short)
		(freq_tbl_lp4[1].frequency * 8 / freq_tbl_lp4[2].frequency),
		RK2_DUMMY_RD_BK_FREQ_RATIO_TX_7));
#endif
	/* for SHUFFLE_4 */
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_PRE_TDQSCK1),
		 p_fld(SET_FLD, PRE_TDQSCK1_SHU_PRELOAD_TX_HW) |
		 p_fld(CLEAR_FLD, PRE_TDQSCK1_SHU_PRELOAD_TX_START) |
		 p_fld(CLEAR_FLD, PRE_TDQSCK1_SW_UP_TX_NOW_CASE));
	if (p->dram_type == TYPE_PCDDR4) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_PRE_TDQSCK1),
			CLEAR_FLD, PRE_TDQSCK1_SHU_PRELOAD_TX_HW);
	}
	show_msg2((INFO, "TX_FREQ_RATIO_0=%d\n",
		io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_RK2_DQSOSC), RK2_DQSOSC_FREQ_RATIO_TX_0)));
	show_msg2((INFO, "TX_FREQ_RATIO_1=%d\n",
		io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_RK2_DQSOSC), RK2_DQSOSC_FREQ_RATIO_TX_1)));
	show_msg2((INFO, "TX_FREQ_RATIO_2=%d\n",
		io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_RK2_DQSOSC), RK2_DQSOSC_FREQ_RATIO_TX_2)));
	show_msg2((INFO, "TX_FREQ_RATIO_3=%d\n",
		io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_RK2_DQSOSC), RK2_DQSOSC_FREQ_RATIO_TX_3)));
	show_msg2((INFO, "TX_FREQ_RATIO_4=%d\n",
		io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_RK2_DQSOSC), RK2_DQSOSC_FREQ_RATIO_TX_4)));
	show_msg2((INFO, "TX_FREQ_RATIO_5=%d\n",
		io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_RK2_DQSOSC), RK2_DQSOSC_FREQ_RATIO_TX_5)));
	show_msg2((INFO, "TX_FREQ_RATIO_6=%d\n",
		io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_RK2_DUMMY_RD_BK),
		RK2_DUMMY_RD_BK_FREQ_RATIO_TX_6)));
	show_msg2((INFO, "TX_FREQ_RATIO_7=%d\n",
		io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_RK2_DUMMY_RD_BK),
		RK2_DUMMY_RD_BK_FREQ_RATIO_TX_7)));
	show_msg2((INFO, "TX_FREQ_RATIO_8=%d\n",
		io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_RK2_DUMMY_RD_BK),
		RK2_DUMMY_RD_BK_FREQ_RATIO_TX_8)));
	show_msg2((INFO, "TX_FREQ_RATIO_9=%d\n",
		io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_RK2_DQSOSC),
		RK2_DQSOSC_FREQ_RATIO_TX_0)));
	show_msg2((INFO, "TX_FREQ_RATIO_9=%d\n",
		io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_PRE_TDQSCK1),
		PRE_TDQSCK1_FREQ_RATIO_TX_9)));
	show_msg2((INFO, "TX_FREQ_RATIO_10=%d\n",
		io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_PRE_TDQSCK1),
		PRE_TDQSCK1_FREQ_RATIO_TX_10)));
	show_msg2((INFO, "TX_FREQ_RATIO_11=%d\n",
		io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_PRE_TDQSCK1),
		PRE_TDQSCK1_FREQ_RATIO_TX_11)));
}


static void tx_tracking_mode_setting(DRAMC_CTX_T *p, unsigned char mode)
{
	unsigned int shu_index;

	/* enable DQSOSC HW mode */
	for (shu_index = DRAM_DFS_SHUFFLE_1; shu_index < DRAM_DFS_SHUFFLE_MAX;
		shu_index++) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_SCINTV) +
			SHU_GRP_DRAMC_OFFSET * shu_index,
			mode, SHU_SCINTV_DQSOSCENDIS);
	}
}

void dramc_hw_dqsosc(DRAMC_CTX_T *p)
{
	DRAM_RANK_T rank_bak = get_rank(p);
	DRAM_CHANNEL_T ch_bak = p->channel;

	dramc_hwdqsosc_set_freq_ratio(p);

	/* DQSOSC MPC command violation */
#if ENABLE_TMRRI_NEW_MODE
	if (p->dram_type == TYPE_PCDDR4) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION),
			CLEAR_FLD, MPC_OPTION_MPC_BLOCKALE_OPT);
	} else {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION),
			SET_FLD, MPC_OPTION_MPC_BLOCKALE_OPT);
	}
#else
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION),
		CLEAR_FLD, MPC_OPTION_MPC_BLOCKALE_OPT);
#endif

	/* DQS2DQ UI/PI setting controlled by HW */
#if ENABLE_SW_TX_TRACKING
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), SET_FLD,
		MISC_CTRL1_R_DMARPIDQ_SW);
#else
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), CLEAR_FLD,
		MISC_CTRL1_R_DMARPIDQ_SW);

	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ9),
		CLEAR_FLD, B0_DQ9_R_DMARPIDQ_SW_B0);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B1_DQ9),
		CLEAR_FLD, B1_DQ9_R_DMARPIDQ_SW_B1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ9),
		CLEAR_FLD, B2_DQ9_R_DMARPIDQ_SW_B2);
#endif
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), SET_FLD,
		DQSOSCR_ARUIDQ_SW);

	/*
	 * Set dqsosc oscillator run time by MRW
	 * write RK0 MR23
	 * Enable HW read MR18/MR19 for each rank
	 */
#if ENABLE_SW_TX_TRACKING
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), SET_FLD,
		DQSOSCR_DQSOSCRDIS);
#else
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), CLEAR_FLD,
		DQSOSCR_DQSOSCRDIS);
#endif

	set_rank(p, RANK_0);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_DQSOSC),
		SET_FLD, RK0_DQSOSC_DQSOSCR_RK0EN);

	if (p->support_rank_num == RANK_DUAL) {
		set_rank(p, RANK_1);
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK1_DQSOSC),
			SET_FLD, RK1_DQSOSC_DQSOSCR_RK1EN);
	} else {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DRSCTRL),
			SET_FLD, DRSCTRL_DRSCLR_RK0_EN);
	}

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), SET_FLD,
		DQSOSCR_DQSOSC_CALEN);
	if (p->dram_type == TYPE_PCDDR4) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DQSOSCR), CLEAR_FLD,
			DQSOSCR_DQSOSC_CALEN);
	}
#if ENABLE_SW_TX_TRACKING
	tx_tracking_mode_setting(p, SET_FLD);
#else
	tx_tracking_mode_setting(p, CLEAR_FLD);
#endif

	set_rank(p, rank_bak);
	set_phy_2_channel_mapping(p, ch_bak);
}
#endif

#if ENABLE_RX_TRACKING_LP4
void dramc_rx_input_delay_tracking_init_common(DRAMC_CTX_T *p)
{
	U8 ii, backup_rank;

	backup_rank = get_rank(p);

	//Enable RX_FIFO macro DIV4 clock CG
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CG_CTRL1),
		0xffffffff, MISC_CG_CTRL1_R_DVS_DIV4_CG_CTRL);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CG_CTRL5),
		0x1ff, MISC_CG_CTRL5_R_DVS_DIV4_CG_CTRL_B2);


	//DVS mode to RG mode
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_R0_B0_RXDVS2),
		0x0, R0_B0_RXDVS2_R_RK0_DVS_MODE_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_R0_B2_RXDVS2),
		0x0, R0_B2_RXDVS2_R_RK0_DVS_MODE_B2);

	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_R1_B0_RXDVS2),
		0x0, R1_B0_RXDVS2_R_RK1_DVS_MODE_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_R1_B2_RXDVS2),
		0x0, R1_B2_RXDVS2_R_RK1_DVS_MODE_B2);

	//Tracking lead/lag counter >> Rx DLY adjustment fixed to 1
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B0_RXDVS0),
		0x0, B0_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B2_RXDVS0),
		0x0, B2_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B2);

	//DQIEN pre-state option to block update for RX ASVA  1-2
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B0_RXDVS0),
		0x1, B0_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B2_RXDVS0),
		0x1, B2_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B2);

	//Turn off F_DLY individual calibration option (CTO_AGENT_RDAT cannot separate DR/DF error)
	//tracking rising and update rising/falling together
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_R0_B0_RXDVS2),
		0x0, R0_B0_RXDVS2_R_RK0_DVS_FDLY_MODE_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_R0_B2_RXDVS2),
		0x0, R0_B2_RXDVS2_R_RK0_DVS_FDLY_MODE_B2);

	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_R1_B0_RXDVS2),
		0x0, R1_B0_RXDVS2_R_RK1_DVS_FDLY_MODE_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_R1_B2_RXDVS2),
		0x0, R1_B2_RXDVS2_R_RK1_DVS_FDLY_MODE_B2);

	//DQ/DQM/DQS DLY MAX/MIN value under Tracking mode
	for(ii=RANK_0; ii<RANK_MAX; ii++) {
		set_rank(p, ii);

			/* Byte 0 */
		/* DQS, DQ, DQM (DQ, DQM are tied together now) -> controlled using DQM MAX_MIN */
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_R0_B0_RXDVS7),
			p_fld(0x0, R0_B0_RXDVS7_RG_RK0_ARDQ_MIN_DLY_B0) |
			p_fld(0x3f, R0_B0_RXDVS7_RG_RK0_ARDQ_MAX_DLY_B0) |
			p_fld(0x0, R0_B0_RXDVS7_RG_RK0_ARDQS0_MIN_DLY_B0) |
			p_fld(0x7f, R0_B0_RXDVS7_RG_RK0_ARDQS0_MAX_DLY_B0));

		/* Byte 1 */
		/* DQS, DQ, DQM (DQ, DQM are tied together now) -> controlled using DQM MAX_MIN */
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_R0_B2_RXDVS7),
			p_fld(0x0, R0_B2_RXDVS7_RG_RK0_ARDQ_MIN_DLY_B2) |
			p_fld(0x3f, R0_B2_RXDVS7_RG_RK0_ARDQ_MAX_DLY_B2) |
			p_fld(0x0, R0_B2_RXDVS7_RG_RK0_ARDQS0_MIN_DLY_B2) |
			p_fld(0x7f, R0_B2_RXDVS7_RG_RK0_ARDQS0_MAX_DLY_B2));

		//Threshold for LEAD/LAG filter
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_R0_B0_RXDVS1),
			p_fld(0x2, R0_B0_RXDVS1_R_RK0_B0_DVS_TH_LEAD) |
			p_fld(0x2, R0_B0_RXDVS1_R_RK0_B0_DVS_TH_LAG));
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_R0_B2_RXDVS1),
			p_fld(0x2, R0_B2_RXDVS1_R_RK0_B2_DVS_TH_LEAD) |
			p_fld(0x2, R0_B2_RXDVS1_R_RK0_B2_DVS_TH_LAG));

		//DQ/DQS Rx DLY adjustment for tracking mode
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_R0_B0_RXDVS2),
			p_fld(0x0, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_DQ_SCALE_B0) |
			p_fld(0x0, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_DQS_SCALE_B0));
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_R0_B2_RXDVS2),
			p_fld(0x0, R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_DQ_SCALE_B2) |
			p_fld(0x0, R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_DQS_SCALE_B2));

		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_R0_B0_RXDVS2),
			p_fld(0x0, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_DQ_SCALE_B0) |
			p_fld(0x0, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_DQS_SCALE_B0));
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_R0_B2_RXDVS2),
			p_fld(0x0, R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_DQ_SCALE_B2) |
			p_fld(0x0, R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_DQS_SCALE_B2));
	}

	set_rank(p, backup_rank);
}

#endif

void dramc_rx_input_delay_tracking_init_by_freq(DRAMC_CTX_T *p)
{
	unsigned char dvs_delay;
	/* Monitor window size setting */
	/* DDRPHY.SHU*_B*_DQ5.RG_RX_ARDQS0_DVS_DLY_B*  (suggested value from A-PHY owner)
	 * 		 Speed	 Voltage	 DVS_DLY
	 *======================================
	 *SHU1 	 3200	 0.8V		 3
	 *SHU2 	 2667	 0.8V-0.7V	 4
	 *SHU3 	 1600	 0.7V-0.65V  5
	 */

	if (p->freqGroup == DDR3200_FREQ)
		dvs_delay = 3;
	else if (p->freqGroup == DDR2666_FREQ)
		dvs_delay = 4;
	else if (p->freqGroup == DDR1600_FREQ)
		dvs_delay = 5;
	else
		dvs_delay = 5;

	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ5),
		dvs_delay, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ5),
		dvs_delay, SHU_B2_DQ5_RG_RX_ARDQS0_DVS_DLY_B2);

    /* Bia_nco HW design issue: run-time PBYTE flag will lose it's function and become per-bit -> set to 0 */
    io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ7), p_fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0)
                                                            | p_fld(0x0, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0));
    io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ7), p_fld(0x0, SHU_B2_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B2)
                                                            | p_fld(0x0, SHU_B2_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B2));
}

#if ENABLE_RX_TRACKING_LP4
void dramc_rx_input_delay_tracking_hw(DRAMC_CTX_T *p)
{
	DRAM_CHANNEL_T channel_bak = p->channel;
	unsigned char updateDone=0;
	unsigned short u2DVS_TH=0x0;
	unsigned short u2MinDly=0x14;
	unsigned short u2MaxDly=0x30;
	unsigned char ii, backup_rank;

	set_phy_2_channel_mapping(p, CHANNEL_A);
	backup_rank = get_rank(p);

	//Rx DLY tracking setting (Static)
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_B0_RXDVS0),
		p_fld(1, B0_RXDVS0_R_RX_DLY_TRACK_SPM_CTRL_B0) |
		p_fld(0, B0_RXDVS0_R_RX_RANKINCTL_B0)|
		p_fld(1, B0_RXDVS0_R_RX_RANKINSEL_B0));

	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_B2_RXDVS0),
		p_fld(1, B2_RXDVS0_R_RX_DLY_TRACK_SPM_CTRL_B2) |
		p_fld(0, B2_RXDVS0_R_RX_RANKINCTL_B2)|
		p_fld(1, B2_RXDVS0_R_RX_RANKINSEL_B2));

#if 1 //[QW] ???
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ9),
		p_fld(0x1, B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0 |
		p_fld(0, B0_DQ9_R_DMRXDVS_VALID_LAT_B0)));
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ9),
		p_fld(0x1, B2_DQ9_R_DMRXDVS_RDSEL_LAT_B2) |
		p_fld(0, B2_DQ9_R_DMRXDVS_VALID_LAT_B2));
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD10),
		p_fld(0,CA_CMD10_R_DMRXDVS_RDSEL_LAT_CA) |
		p_fld(0, CA_CMD10_R_DMRXDVS_VALID_LAT_CA));
#endif
	/* DMRXTRACK_DQM_B* (rxdly_track SM DQM enable) -> need to be set to 1 if R_DBI is on
	*	They are shuffle regs -> move setting to DramcSetting_Olympus_LP4_ByteMode()
	*/

	//Enable A-PHY DVS LEAD/LAG
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), 0x1,
		B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ5), 0x1,
		B2_DQ5_RG_RX_ARDQS0_DVS_EN_B2);

	//Rx DLY tracking function CG enable
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B0_RXDVS0), 0x1,
		B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B2_RXDVS0), 0x1,
		B2_RXDVS0_R_RX_DLY_TRACK_CG_EN_B2);

	//Rx DLY tracking lead/lag counter enable
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B0_RXDVS0), 0x1,
		B0_RXDVS0_R_RX_DLY_TRACK_ENA_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B2_RXDVS0), 0x1,
		B2_RXDVS0_R_RX_DLY_TRACK_ENA_B2);
	if ((p->dram_type == TYPE_PCDDR4) && (get_div_mode(p) == DIV4_MODE)) {
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_B0_RXDVS0),
			p_fld(0, B0_RXDVS0_R_RX_DLY_TRACK_ENA_B0) |
			p_fld(0, B0_RXDVS0_R_RX_DLY_TRACK_SPM_CTRL_B0)|
			p_fld(0, B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0));
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_B2_RXDVS0),
			p_fld(0, B2_RXDVS0_R_RX_DLY_TRACK_ENA_B2) |
			p_fld(0, B2_RXDVS0_R_RX_DLY_TRACK_SPM_CTRL_B2)|
			p_fld(0, B2_RXDVS0_R_RX_DLY_TRACK_CG_EN_B2));
	}
	//Rx DLY tracking update enable (HW mode)
	for(ii=RANK_0; ii<RANK_MAX; ii++) {
		set_rank(p, ii);

		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_R0_B0_RXDVS2),
			p_fld(2, R0_B0_RXDVS2_R_RK0_DVS_MODE_B0) |
			p_fld(1, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0)|
			p_fld(1, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0));

		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_R0_B2_RXDVS2),
			p_fld(2, R0_B2_RXDVS2_R_RK0_DVS_MODE_B2) |
			p_fld(1, R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B2)|
			p_fld(1, R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B2));
		if ((p->dram_type == TYPE_PCDDR4) && (get_div_mode(p) == DIV4_MODE)) {
			io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_R0_B0_RXDVS2),
				p_fld(0, R0_B0_RXDVS2_R_RK0_DVS_MODE_B0) |
				p_fld(0, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0)|
				p_fld(0, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0));
			io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_R0_B2_RXDVS2),
				p_fld(0, R0_B2_RXDVS2_R_RK0_DVS_MODE_B2) |
				p_fld(0, R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B2)|
				p_fld(0, R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B2));
		}
	}

	set_rank(p, backup_rank);

	set_phy_2_channel_mapping(p, channel_bak);
}

#endif

#if SIMULATION_LP4_ZQ
/*
 * dramc_zq_calibration
 *  start Dram ZQ calibration.
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @retval status          (DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
DRAM_STATUS_T dramc_zq_calibration(DRAMC_CTX_T *p)
{
	unsigned int response;
	unsigned int time_cnt = TIME_OUT_CNT;
	unsigned int reg_backup_address[] = { DRAMC_REG_ADDR(DRAMC_REG_MRS),
		DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL),
		DRAMC_REG_ADDR(DRAMC_REG_CKECTRL) };

	/* Backup rank, CKE fix on/off, HW MIOCK control settings */
	dramc_backup_registers(p, reg_backup_address,
		sizeof(reg_backup_address) / sizeof(unsigned int));

	show_msg3((INFO, "[ZQCalibration]\n"));

	/* Disable HW MIOCK control to make CLK always on */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL),
		SET_FLD,	DRAMC_PD_CTRL_MIOCKCTRLOFF);
	delay_us(1);

	/* it will apply to both rank. */
	cke_fix_on_off(p, p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	/* !!R_DMMRSRK(R_DMMPCRKEN=1) specify rank0 or rank1 */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), get_rank(p),
		MRS_MRSRK);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION), SET_FLD,
		MPC_OPTION_MPCRKEN);

	/* ZQCAL Start
	 * R_DMZQCEN, 0x1E4[4]=1 for ZQCal Start
	 * Wait zqc_response=1 (dramc_conf_nao, 0x3b8[4])
	 * R_DMZQCEN, 0x1E4[4]=0
	 */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD),
		SET_FLD, SPCMD_ZQCEN);
	do {
		response =
			io_32_read_fld_align(
			DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP),
			SPCMDRESP_ZQC_RESPONSE);
		time_cnt--;
		delay_us(1);	/* Wait tZQCAL(min) 1us or wait next polling */

		show_msg2((INFO, "%d- ", time_cnt));
	} while ((response == 0) && (time_cnt > 0));

	if (time_cnt == 0) {	/* time out */
		set_calibration_result(p, DRAM_CALIBRATION_ZQ, DRAM_FAIL);
		show_err("ZQCAL Start fail (time out)\n");
		return DRAM_FAIL;
	}

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD),
		CLEAR_FLD, SPCMD_ZQCEN);

	/* delay tZQCAL */
	delay_us(1);
	time_cnt = TIME_OUT_CNT;

	/*
	 * ZQCAL Latch
	 * R_DMZQLATEN, 0x1E4[6]=1 for ZQCal latch
	 * Wait zqlat_response=1 (dramc_conf_nao, 0x3b8[28])
	 * R_DMZQLATEN, 0x1E4[6]=0
	 */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD),
		SET_FLD, SPCMD_ZQLATEN);
	do {
		response =
			io_32_read_fld_align(DRAMC_REG_ADDR
				(DRAMC_REG_SPCMDRESP),
				SPCMDRESP_ZQLAT_RESPONSE);
		time_cnt--;
		delay_us(1);	/*  Wait tZQLAT 30ns or wait next polling */

		show_msg2((INFO, "%d=", time_cnt));
	} while ((response == 0) && (time_cnt > 0));

	if (time_cnt == 0) {	/* time out */
		set_calibration_result(p, DRAM_CALIBRATION_ZQ, DRAM_FAIL);
		show_err("ZQCAL Latch fail (time out)\n");
		return DRAM_FAIL;
	}
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD),
		CLEAR_FLD, SPCMD_ZQLATEN);

	/* delay tZQLAT */
	delay_us(1);

	/* Restore rank, CKE fix on, HW MIOCK control settings */
	dramc_restore_registers(p, reg_backup_address,
		sizeof(reg_backup_address) / sizeof(unsigned int));

	set_calibration_result(p, DRAM_CALIBRATION_ZQ, DRAM_OK);
	show_msg((INFO, "[DramcZQCalibration] Done\n"));

	return DRAM_OK;
}
#endif

/* From M */
#if SIMULATION_SW_IMPED
void dramc_sw_impedance_save_register(DRAMC_CTX_T *p,
	unsigned char ca_term_option, unsigned char dq_term_option,
	unsigned char save_to_where)
{
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	/* DQ */
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING1 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_DRVP],
			SHU_DRVING1_DQDRVP2) |
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_DRVN],
			SHU_DRVING1_DQDRVN2));
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING2 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_DRVP],
			SHU_DRVING2_DQDRVP1) |
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_DRVN],
			SHU_DRVING2_DQDRVN1) |
		p_fld((!dq_term_option), SHU_DRVING2_DIS_IMPCAL_ODT_EN));
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING3 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_ODTP],
			SHU_DRVING3_DQODTP2) |
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_ODTN],
			SHU_DRVING3_DQODTN2));
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING4 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_ODTP],
			SHU_DRVING4_DQODTP1) |
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_ODTN],
			SHU_DRVING4_DQODTN1));
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING5 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_DRVP],
			SHU_DRVING5_DQDRVP3) |
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_DRVN],
			SHU_DRVING5_DQDRVN3));
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING6 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_ODTP],
			SHU_DRVING6_DQODTP3) |
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_ODTN],
			SHU_DRVING6_DQODTN3));

	/* DQS */
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING1 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_DRVP],
			SHU_DRVING1_DQSDRVP2) |
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_DRVN],
			SHU_DRVING1_DQSDRVN2));
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING1 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_DRVP],
			SHU_DRVING1_DQSDRVP1) |
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_DRVN],
			SHU_DRVING1_DQSDRVN1));
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING3 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_ODTP],
			SHU_DRVING3_DQSODTP2) |
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_ODTN],
			SHU_DRVING3_DQSODTN2));
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING3 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_ODTP],
			SHU_DRVING3_DQSODTP) |
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_ODTN],
			SHU_DRVING3_DQSODTN));
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING5 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_DRVP],
			SHU_DRVING5_DQSDRVP3) |
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_DRVN],
			SHU_DRVING5_DQSDRVN3));
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING6 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_ODTP],
			SHU_DRVING6_DQSODTP3) |
		p_fld(dramc_imp_result[dq_term_option][TERM_TYPE_ODTN],
			SHU_DRVING6_DQSODTN3));

	/* CA */
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING2 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[ca_term_option][TERM_TYPE_DRVP],
			SHU_DRVING2_CMDDRVP2) |
		p_fld(dramc_imp_result[ca_term_option][TERM_TYPE_DRVN],
			SHU_DRVING2_CMDDRVN2));
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING2 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[ca_term_option][TERM_TYPE_DRVP],
			SHU_DRVING2_CMDDRVP1) |
		p_fld(dramc_imp_result[ca_term_option][TERM_TYPE_DRVN],
			SHU_DRVING2_CMDDRVN1));
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING4 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[ca_term_option][TERM_TYPE_ODTP],
			SHU_DRVING4_CMDODTP2) |
		p_fld(dramc_imp_result[ca_term_option][TERM_TYPE_ODTN],
			SHU_DRVING4_CMDODTN2));
	io_32_write_fld_multi_all((DRAMC_REG_SHU_DRVING4 +
		save_to_where * SHU_GRP_DRAMC_OFFSET),
		p_fld(dramc_imp_result[ca_term_option][TERM_TYPE_ODTP],
			SHU_DRVING4_CMDODTP1) |
		p_fld(dramc_imp_result[ca_term_option][TERM_TYPE_ODTN],
			SHU_DRVING4_CMDODTN1));

	io_32_write_fld_align((DDRPHY_SHU_CA_CMD11 +
		save_to_where * SHU_GRP_DDRPHY_OFFSET),
		0x0, SHU_CA_CMD11_RG_TX_ARCKE_DRVP);
	io_32_write_fld_align((DDRPHY_SHU_CA_CMD11 +
		save_to_where * SHU_GRP_DDRPHY_OFFSET),
		0x0, SHU_CA_CMD11_RG_TX_ARCKE_DRVN);

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_align_all((DDRPHY_SHU_CA_CMD3 +
		save_to_where * SHU_GRP_DDRPHY_OFFSET),
		(8 >> 3) & 0x3, SHU_CA_CMD3_RG_TX_ARCMD_PU_PRE);
	io_32_write_fld_align_all((DDRPHY_SHU_CA_CMD0 +
		save_to_where * SHU_GRP_DDRPHY_OFFSET),
		8 & 0x7, SHU_CA_CMD0_RG_TX_ARCLK_DRVN_PRE);
	if (is_lp4_family(p)) {
		io_32_write_fld_align_all((DDRPHY_SHU_CA_DLL1 +
			save_to_where * SHU_GRP_DDRPHY_OFFSET),
			9, RG_ARCMD_REV_BIT_1208_TX_CKE_DRVN);
	} else if (p->dram_type == TYPE_PCDDR3) {
		io_32_write_fld_align_all((DDRPHY_SHU_CA_DLL1 +
			save_to_where * SHU_GRP_DDRPHY_OFFSET),
			7, RG_ARCMD_REV_BIT_1208_TX_CKE_DRVN);
	} else if (p->dram_type == TYPE_LPDDR3) {
		io_32_write_fld_align_all((DDRPHY_SHU_CA_DLL1 +
			save_to_where * SHU_GRP_DDRPHY_OFFSET),
			0xe, RG_ARCMD_REV_BIT_1208_TX_CKE_DRVN);
	}
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);
}

/*
 * imp_cal_vref_sel
 *  Set IMP_VREF_SEL for DRVP, DRVN, Run-time/Tracking
 *  (Refer to "IMPCAL Settings" document register "RG_RIMP_VREF_SEL" settings)
 *  @param p	Pointer of context created by DramcCtxCreate.
 *  @param  term_option	(unsigned char): pass term_option (odt_on/off) for LP4X
 *  @param  imp_cal_stage	(unsigned char): During DRVP, DRVN,
 *	un-time/tracking stages
 *	some vref_sel values are different
 */
/* Refer to "IMPCAL Settings" document register "RG_RIMP_VREF_SEL" settings */
static void imp_cal_vref_sel(DRAMC_CTX_T *p, unsigned char term_option,
	unsigned char stage)
{
	unsigned char vref = 0;

	if (p->dram_type == TYPE_LPDDR4) {
		vref = IMP_LP4_VREF_SEL;
	} else if (p->dram_type == TYPE_LPDDR4X) {
		if (term_option == TERM) {
			vref = IMP_LP4X_TERM_VREF_SEL;
		} else {
			if (stage == IMPCAL_STAGE_DRVP)
				vref = IMP_DRVP_LP4X_UNTERM_VREF_SEL;
			else if (stage == IMPCAL_STAGE_DRVN)
				vref = IMP_DRVN_LP4X_UNTERM_VREF_SEL;
			else
				vref = IMP_TRACK_LP4X_UNTERM_VREF_SEL;
		}
	} else if (p->dram_type == TYPE_LPDDR4P) {
		if (stage == IMPCAL_STAGE_DRVP)
			vref = IMP_DRVP_LP4P_VREF_SEL;
		else if (stage == IMPCAL_STAGE_DRVN)
			vref = IMP_DRVN_LP4P_VREF_SEL;
		else
			vref = IMP_TRACK_LP4P_VREF_SEL;
	} else if ((p->dram_type == TYPE_LPDDR3) || (p->dram_type == TYPE_PCDDR3)) {
		vref = IMP_LP3_VREF_SEL;
	} else if (p->dram_type == TYPE_PCDDR4) {
		if (stage == IMPCAL_STAGE_DRVP)
			vref = 0x37;
		else if (stage == IMPCAL_STAGE_DRVN)
			vref = 0x2b;
		else
			vref = 0x2b;
	} else {
		show_msg2((INFO, "Un-recognized DRAM type!!!\n"));
	}

	show_msg2((INFO, "[vImpCalVrefSel] IMP_VREF_SEL 0x%x, IMPCAL stage:%u, term_option:%u\n",
		vref, stage, term_option));
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD11),
		vref, SHU_CA_CMD11_RG_RIMP_VREF_SEL);
}

static void dramc_sw_impedance_cal_init(DRAMC_CTX_T *p)
{
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_MISC_SPM_CTRL1),
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B0) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B1) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B2) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_CA));
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_SPM_CTRL2),
		CLEAR_FLD, MISC_SPM_CTRL2_PHY_SPM_CTL2);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_SPM_CTRL0),
		CLEAR_FLD, MISC_SPM_CTRL0_PHY_SPM_CTL0);

	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL),
		CLEAR_FLD, IMPCAL_IMPCAL_HW);

	io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_MISC_IMP_CTRL1),
		p_fld(CLEAR_FLD, MISC_IMP_CTRL1_RG_RIMP_PRE_EN) |
		p_fld(SET_FLD, MISC_IMP_CTRL1_RG_RIMP_BIAS_EN) |
		p_fld(SET_FLD, MISC_IMP_CTRL1_RG_RIMP_VREF_EN));
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL),
		p_fld(CLEAR_FLD, IMPCAL_IMPCAL_CALI_ENN) |
		p_fld(SET_FLD, IMPCAL_IMPCAL_IMPPDP) |
		p_fld(SET_FLD, IMPCAL_IMPCAL_IMPPDN));

	io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_PLL3),
		p_fld(CLEAR_FLD, PLL3_RG_RPHYPLL_TSTOD_EN) |
		p_fld(CLEAR_FLD, PLL3_RG_RPHYPLL_TSTCK_EN));

	if (is_lp4_family(p)/* || (p->dram_type == TYPE_PCDDR4)*/) {
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_MISC_IMP_CTRL0),
			p_fld(SET_FLD, MISC_IMP_CTRL0_RG_IMP_EN) |
			p_fld(CLEAR_FLD, MISC_IMP_CTRL0_RG_RIMP_DDR3_SEL) |
			p_fld(SET_FLD, MISC_IMP_CTRL0_RG_RIMP_DDR4_SEL));
	} else {
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_MISC_IMP_CTRL0),
			p_fld(0x1, MISC_IMP_CTRL0_RG_IMP_EN) |
			p_fld(SET_FLD, MISC_IMP_CTRL0_RG_RIMP_DDR3_SEL) |
			p_fld(CLEAR_FLD, MISC_IMP_CTRL0_RG_RIMP_DDR4_SEL));
	}

	show_msg2((INFO, "MISC_IMP_CTRL1: 0x%X = 0x%X\n",
		DDRPHY_MISC_IMP_CTRL1,
		io32_read_4b(DDRPHY_MISC_IMP_CTRL1)));
	show_msg2((INFO, "MISC_IMP_CTR0: 0x%X = 0x%X\n",
		DDRPHY_MISC_IMP_CTRL0,
		io32_read_4b(DDRPHY_MISC_IMP_CTRL0)));

	delay_us(1);
}

static unsigned int dramc_sw_impedance_drvp(DRAMC_CTX_T *p)
{
	unsigned char reg_tmp_value;
	unsigned int impx_drv, imp_cal_result;
	unsigned int drvp_result = BYTE_MAX;

	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL),
		p_fld(1, IMPCAL_IMPCAL_CALI_EN) |
		p_fld(1, IMPCAL_IMPCAL_CALI_ENP) |
		p_fld(0, IMPCAL_IMPCAL_CALI_ENN));

	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_IMPCAL1),
		p_fld(0, SHU_IMPCAL1_IMPDRVN) |
		p_fld(0, SHU_IMPCAL1_IMPDRVP));

	if (p->dram_type == TYPE_LPDDR4X || p->dram_type == TYPE_LPDDR4P)
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD11),
			0x3, SHU_CA_CMD11_RG_RIMP_REV);
	else /* cc notes: need REVIEW */
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD11),
			0x1, SHU_CA_CMD11_RG_RIMP_REV);

	for (impx_drv = 0; impx_drv < 32; impx_drv++) {
		if (impx_drv == 16)
			impx_drv = 29;

		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_IMPCAL1),
			impx_drv, SHU_IMPCAL1_IMPDRVP);
		delay_us(1);
		imp_cal_result = io_32_read_fld_align(
			DRAMC_REG_ADDR(DDRPHY_MISC_PHY_RGS_CMD),
			MISC_PHY_RGS_CMD_RGS_RIMPCALOUT);
		show_msg2((INFO, "1. OCD DRVP = %d CALOUT = %d\n",
			impx_drv, imp_cal_result));

		if ((imp_cal_result == 1) && (drvp_result == 0xff)) {
			drvp_result = impx_drv;
			show_msg2((INFO, "1. OCD DRVP calibration OK! DRVP = %d\n\n",
				drvp_result));
			break;
		}
	}

	return drvp_result;
}

static unsigned int dramc_sw_impedance_drvn(DRAMC_CTX_T *p)
{
	unsigned int impx_drv, imp_cal_result;
	unsigned int drvn_result = BYTE_MAX;


	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL),
		0, IMPCAL_IMPCAL_CALI_ENP);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_IMPCAL1),
		0, SHU_IMPCAL1_IMPDRVN);

	for (impx_drv = 0; impx_drv < 32; impx_drv++) {
		if (impx_drv == 16)
			impx_drv = 29;

		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_IMPCAL1),
			impx_drv, SHU_IMPCAL1_IMPDRVN);
		delay_us(1);
		imp_cal_result = io_32_read_fld_align(
			DRAMC_REG_ADDR(DDRPHY_MISC_PHY_RGS_CMD),
			MISC_PHY_RGS_CMD_RGS_RIMPCALOUT);
		show_msg2((INFO, "3. OCD DRVN = %d, CALOUT = %d\n",
			impx_drv, imp_cal_result));

		if ((imp_cal_result == 0) && (drvn_result == 0xff)) {
			drvn_result = impx_drv;
			show_msg2((INFO, "3. OCD DRVN calibration OK! DRVN = %d\n\n",
				drvn_result));
			break;
		}
	}

	return drvn_result;
}

static unsigned short round_operation(unsigned short A,
	unsigned short B)
{
	unsigned short temp;

	if (B == 0) {
		return 0xffff;
	}

	temp = A / B;

	if ((A - temp*B) >= ((temp + 1)*B - A)) {
		return (temp + 1);
	} else {
		return temp;
	}
}

static void dramc_sw_impedance_calc(DRAMC_CTX_T *p,
	unsigned char ocdp, unsigned char ocdn, unsigned char term_option)
{
	unsigned char term_type;
	unsigned char drvn;

	if (ocdp == BYTE_MAX || ocdn == BYTE_MAX) {
		ocdp = OCDP_DEFAULT;
		ocdn = OCDN_DEFALUT;
	}

	if (is_lp4_family(p)) {
		drvn = 14;

		if ((p->dram_type == TYPE_LPDDR4X || p->dram_type == TYPE_LPDDR4P) &&
			(term_option == UNTERM)) {
			dramc_imp_result[term_option][TERM_TYPE_DRVP] = ocdp;
			dramc_imp_result[term_option][TERM_TYPE_DRVN] = ocdn;
			dramc_imp_result[term_option][TERM_TYPE_ODTP] = 0x0;
			dramc_imp_result[term_option][TERM_TYPE_ODTN] = 0xf;
		} else {
			dramc_imp_result[term_option][TERM_TYPE_DRVP] =
				ocdp <= 3 ? (ocdp * 3) : ocdp;
			dramc_imp_result[term_option][TERM_TYPE_DRVN] =
				drvn;
			dramc_imp_result[term_option][TERM_TYPE_ODTP] = 0x0;
			dramc_imp_result[term_option][TERM_TYPE_ODTN] =
				ocdn <= 3 ? (ocdn * 3) : ocdn;
		}
	} else {
		drvn = ocdn;

		dramc_imp_result[term_option][TERM_TYPE_DRVP] =
			ocdp <= 3 ? (ocdp * 3) : ocdp;
		dramc_imp_result[term_option][TERM_TYPE_DRVN] =
			drvn <= 3 ? (drvn * 3) : drvn;
		if (p->dram_type == TYPE_PCDDR4) {
			/* For DDR4, odtp = drvp * 0.66 */
			ocdp = ((ocdp + 2) * 66 + 99) / 100;
			ocdp -= 2;
			dramc_imp_result[term_option][TERM_TYPE_ODTP] = ocdp;
			dramc_imp_result[term_option][TERM_TYPE_ODTN] = 0;
		} else {
			dramc_imp_result[term_option][TERM_TYPE_ODTP] = 0x0;
			dramc_imp_result[term_option][TERM_TYPE_ODTN] =
				ocdn <= 3 ? (ocdn * 3) : ocdn;
		}
	}
}

DRAM_STATUS_T dramc_sw_impedance_cal(DRAMC_CTX_T *p,
	unsigned char term_option)
{
	unsigned int drvp_result, drvn_result;
	unsigned char byteidx;
	unsigned char bk_broadcast;

	unsigned int reg_backup_address[] = {
		(DRAMC_REG_ADDR(DDRPHY_MISC_IMP_CTRL0)),
		(DRAMC_REG_ADDR(DDRPHY_MISC_IMP_CTRL1)),
		(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL)),
	};

	bk_broadcast = get_dramc_broadcast();
	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);

	dramc_backup_registers(p, reg_backup_address,
		sizeof(reg_backup_address) / sizeof(unsigned int));

	dramc_sw_impedance_cal_init(p);

	show_msg2((INFO, "======= K DRVP=====================\n"));
	imp_cal_vref_sel(p, term_option, IMPCAL_STAGE_DRVP);
	drvp_result = dramc_sw_impedance_drvp(p);
	if (drvp_result == BYTE_MAX)
		show_msg2((INFO, "OCD DRVP calibration FAIL\n"));

	show_msg2((INFO, "======= K DRVN=====================\n"));
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_IMPCAL1),
		p_fld(drvp_result, SHU_IMPCAL1_IMPDRVP));
	imp_cal_vref_sel(p, term_option, IMPCAL_STAGE_DRVN);
	if (p->dram_type == TYPE_LPDDR4 || p->dram_type == TYPE_LPDDR4X) {
		if (term_option == TERM)
			io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL),
				SET_FLD, IMPCAL_IMPCAL_CALI_ENN);
	}

	drvn_result = dramc_sw_impedance_drvn(p);
	if (drvn_result == BYTE_MAX)
		show_msg2((INFO, 
"OCD DRVN calibration FAIL\n"));

	dramc_sw_impedance_calc(p, drvp_result, drvn_result,
		term_option);

#if (BYPASS_IMP && SUPPORT_SAVE_TIME_FOR_CALIBRATION && RUNTIME_SHMOO_RELEATED_FUNCTION)
	{
		U8 u1drv;
		{
			for (u1drv=0; u1drv<4; u1drv++)
			{
				if(p->femmc_Ready==0)
					p->pSavetimeData->u1SwImpedanceResule[term_option][u1drv] = dramc_imp_result[term_option][u1drv];
				else
					dramc_imp_result[term_option][u1drv] = p->pSavetimeData->u1SwImpedanceResule[term_option][u1drv];
			}
		}
	}
#endif
	show_diag((INFO, "%s: OCDP %x, OCDN %x, ODTP %x, ODTN %x\n",
		"Final Impdance Cal Result: ",
		dramc_imp_result[term_option][TERM_TYPE_DRVP],
		dramc_imp_result[term_option][TERM_TYPE_DRVN],
		dramc_imp_result[term_option][TERM_TYPE_ODTP],
		dramc_imp_result[term_option][TERM_TYPE_ODTN]));
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
	show_msg2((INFO, "[HQALOG]Impedance_Cal %x,%x,%x,%x\n",
		dramc_imp_result[term_option][TERM_TYPE_DRVP],
		dramc_imp_result[term_option][TERM_TYPE_DRVN],
		dramc_imp_result[term_option][TERM_TYPE_ODTP],
		dramc_imp_result[term_option][TERM_TYPE_ODTN]));
#endif
	imp_cal_vref_sel(p, term_option, IMPCAL_STAGE_TRACKING);

	dramc_restore_registers(p, reg_backup_address,
		sizeof(reg_backup_address) / sizeof(unsigned int));

	dramc_broadcast_on_off(bk_broadcast);

	return DRAM_OK;
}

#endif /* SIMULATION_SW_IMPED */

void dramc_update_impedance_term_2un_term(DRAMC_CTX_T *p)
{
	dramc_imp_result[ODT_OFF][TERM_TYPE_ODTP] =
		dramc_imp_result[ODT_ON][TERM_TYPE_ODTP];
	dramc_imp_result[ODT_OFF][TERM_TYPE_ODTN] =
		dramc_imp_result[ODT_ON][TERM_TYPE_ODTN];
}

void o1_path_on_off(DRAMC_CTX_T *p, unsigned char on_off)
{
	unsigned char fix_dqien = 0;

	if (p->data_width == DATA_WIDTH_16BIT) {
		fix_dqien = (on_off == ENABLE) ? 0x3 : 0;
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_PADCTRL), fix_dqien,
			PADCTRL_FIXDQIEN);


		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), on_off,
			B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ5), on_off,
			B2_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B2);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ3), on_off,
			B0_DQ3_RG_RX_ARDQ_SMT_EN_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ3), on_off,
			B2_DQ3_RG_RX_ARDQ_SMT_EN_B2);
	} else {
		fix_dqien = (on_off == ENABLE) ? 0xf : 0;
		io_32_write_fld_align(DRAMC_REG_PADCTRL, fix_dqien,
			PADCTRL_FIXDQIEN);

		io_32_write_fld_align_all(DDRPHY_B0_DQ5, on_off,
			B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0);
		io_32_write_fld_align_all(DDRPHY_B2_DQ5, on_off,
			B2_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B2);
		io_32_write_fld_align_all(DDRPHY_B0_DQ3, on_off,
			B0_DQ3_RG_RX_ARDQ_SMT_EN_B0);
		io_32_write_fld_align_all(DDRPHY_B2_DQ3, on_off,
			B2_DQ3_RG_RX_ARDQ_SMT_EN_B2);
	}
	delay_us(1);
}

#if SUPPORT_TYPE_LPDDR4
/*  LPDDR DQ -> PHY DQ mapping
 *  Use LPDDR DQ bit as index to find PHY DQ bit position
 */
#if 0 /* cc mark */
const unsigned char lpddr4_phy_mapping_pop[CHANNEL_NUM][16] = {
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 14, 15
	},
#if (CHANNEL_NUM == 2)
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 14, 15
	},
#endif
};
#endif

/* For Perbit adjust mapping relationship. Use PAD dq as index to find dramc DQ */
const unsigned char lpddr4_phy2dramc_dq_mapping_emcp[CHANNEL_NUM][16] = {
	{
		13, 14, 15, 12, 11, 10, 8, 9,
		6, 3, 2, 1, 7, 4, 5, 0,
	},
#if (CHANNEL_NUM == 2)
	{
		12, 15, 13, 11, 14, 10, 8, 9,
		6, 3, 2, 1, 7, 4, 5, 0,
	},
#endif
};

const unsigned char lpddr4_phy2dramc_dq_mapping_dsc[CHANNEL_NUM][16] = {
	{
		0, 1, 7, 6, 2, 3, 5, 4,
		11, 10, 9, 8, 15, 14, 13, 12
	},
#if (CHANNEL_NUM == 2)
	{
		6, 7, 0, 2, 1, 3, 5, 4,
		11, 10, 9, 8, 15, 14, 13, 12,
	},
#endif
};

/* Use PAD dq as index to find dramc DQ byte */
const unsigned char lpddr4_phy2dramc_byte_mapping_emcp[CHANNEL_NUM][2] = {
	{1, 0},
#if (CHANNEL_NUM == 2)
	{1, 0},
#endif
};

const unsigned char lpddr4_phy2dramc_byte_mapping_dsc[CHANNEL_NUM][2] = {
	{0, 1},
#if (CHANNEL_NUM == 2)
	{0, 1},
#endif
};

#endif

#if SUPPORT_TYPE_LPDDR3
/* LPDDR3 DQ (as index) -> PHY pad */
const unsigned char lpddr3_phy_mapping_pop[CHANNEL_NUM][32] = {
#if 0 /* PINMUX not applied */
	/* CH-A */
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31,
	},
#else /* PINMUX applied */
	/* CH-A */
	/*{
		2, 3, 0, 1, 4, 5, 6, 7,
		23, 22, 21, 19, 20, 16, 18, 17,
		15, 14, 12, 10, 11, 13, 8, 9,
		25, 24, 29, 27, 26, 28, 30, 31,
	},*/
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31,
	},
#endif
};

const unsigned char lpddr3_4bitmux_byte_mapping[DATA_WIDTH_32BIT] = {
	8, 9, 10, 11, 4, 5, 6, 7,
	0, 1, 2, 3, 12, 13, 14, 15,
	24, 25, 26, 27, 20, 21, 22, 23,
	16, 17, 18, 19, 28, 29, 30, 31,
};

const unsigned char lpddr3_phy2dramc_dq_mapping_emcp[32] = {
		2, 3, 0, 1, 4, 5, 6, 7,
		22, 23, 19, 20, 18, 21, 17, 16,
		13, 15, 14, 11, 12, 10, 9, 8,
		25, 24, 29, 27, 26, 28, 30, 31,
};

const unsigned char lpddr3_phy2dramc_dq_mapping_dsc[32] = {
		19, 23, 18, 22, 2, 3, 7, 6,
		21, 17, 20, 16, 5, 4, 1, 0,
		25, 29, 28, 13, 24, 12, 8, 9,
		26, 30, 27, 31, 11, 10, 14, 15,
};

/* Use PAD dq as index to find dramc DQ byte */
const unsigned char lpddr3_phy2dramc_byte_mapping_emcp[4] = {
	0, 2, 1, 3
};

const unsigned char lpddr3_phy2dramc_byte_mapping_dsc[4] = {
	0, 2, 1, 3
};

#endif

#if SUPPORT_TYPE_PCDDR4
/*  LPDDR DQ -> PHY DQ mapping
 *  Use LPDDR DQ bit as index to find PHY DQ bit position
 */
/* For Perbit adjust mapping relationship. Use PAD dq as index to find dramc DQ */
const unsigned char pcddr4_phy2dramc_dq_mapping[CHANNEL_NUM][16] = {
	{
		12, 14, 8, 10, 0, 4, 2, 6,
		7, 3, 5, 1, 15, 13, 11, 9,
	},
#if (CHANNEL_NUM == 2)
	{
		3, 7, 5, 9, 1, 11, 13, 15,
		8, 10, 12, 14, 6, 2, 4, 0,
	},
#endif
};

/* Use PAD dq as index to find dramc DQ byte */
const unsigned char pcddr4_phy2dramc_byte_mapping[CHANNEL_NUM][2] = {
	{0, 1},
#if (CHANNEL_NUM == 2)
	{1, 0},
#endif
};

#endif

#if SUPPORT_TYPE_PCDDR3
/*  LPDDR DQ -> PHY DQ mapping
 *  Use LPDDR DQ bit as index to find PHY DQ bit position
 */
/* For Perbit adjust mapping relationship. Use PAD dq as index to find dramc DQ */
const unsigned char pcddr3_phy2dramc_dq_mapping_x16[DATA_WIDTH_32BIT] = {
		13, 11, 15, 9, 0, 2, 6, 4,
		7, 5, 1, 3, 8, 10, 14, 12,
		21, 23, 17, 28, 19, 30, 26, 24,
		31, 25, 29, 27, 20, 22, 18, 16,
};

const unsigned char pcddr3_phy2dramc_dq_mapping_x8[DATA_WIDTH_32BIT] = {
		3, 1, 5, 7, 4, 0, 2, 6,
		12, 14, 10, 8, 13, 15, 11, 9,
		24, 26, 30, 25, 28, 27, 31, 29,
		21, 23, 19, 17, 18, 16, 22, 20,
};

/* Use PAD dq as index to find dramc DQ byte */
const unsigned char pcddr3_phy2dramc_byte_mapping[DQS_NUMBER] = {
	0, 1, 3, 2
};

#endif
unsigned char *dramc_get_4bitmux_byte_mapping(DRAMC_CTX_T *p)
{
	unsigned char *ret;

	ret = NULL;

	/* Only LP3 support 4bit mux in 8168 */
#if (SUPPORT_TYPE_LPDDR3== 1)
	ret = (p->dram_type == TYPE_LPDDR3) ?
		lpddr3_4bitmux_byte_mapping : NULL;
#endif

	return ret;
}

unsigned char *dramc_get_phy2dramc_dq_mapping(DRAMC_CTX_T *p)
{
	unsigned char *ret;

	ret = NULL;

#if (SUPPORT_TYPE_LPDDR4 == 1)
	if(is_lp4_family(p)) {
		if (p->is_emcp)
			ret = lpddr4_phy2dramc_dq_mapping_emcp[p->channel];
		else
			ret = lpddr4_phy2dramc_dq_mapping_dsc[p->channel];

	}
#endif

#if (SUPPORT_TYPE_LPDDR3 == 1)
	if(p->dram_type == TYPE_LPDDR3){
		if (p->is_emcp)
			ret = lpddr3_phy2dramc_dq_mapping_emcp;
		else
			ret = lpddr3_phy2dramc_dq_mapping_dsc;
	}
#endif

#if (SUPPORT_TYPE_PCDDR4 == 1)
	if(p->dram_type == TYPE_PCDDR4)
		ret = pcddr4_phy2dramc_dq_mapping[p->channel];
#endif

#if (SUPPORT_TYPE_PCDDR3 == 1)
	if(p->dram_type == TYPE_PCDDR3) {
		if (p->pinmux_type == PINMUX_TYPE_PC3_X16X2 ||
			p->pinmux_type == PINMUX_TYPE_PC3_X16)
			ret = pcddr3_phy2dramc_dq_mapping_x16;
		else
			ret = pcddr3_phy2dramc_dq_mapping_x8;
	}
#endif

	return ret;
}

/* dramc_miock_jmeter
 *  start MIOCK jitter meter.
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @param block_no         (unsigned char): block 0 or 1.
 *  @retval status          (DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */

#ifdef ENABLE_MIOCK_JMETER
static void set_enable_miock_jmeter_rg(DRAMC_CTX_T *p)
{

	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_CA_DLL_ARPI2),
		CLEAR_FLD, CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B0_DLL_ARPI2),
		CLEAR_FLD, B0_DLL_ARPI2_RG_ARDLL_PHDET_EN_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B1_DLL_ARPI2),
		CLEAR_FLD, B1_DLL_ARPI2_RG_ARDLL_PHDET_EN_B1);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B2_DLL_ARPI2),
		CLEAR_FLD, B2_DLL_ARPI2_RG_ARDLL_PHDET_EN_B2);

	if (is_lp4_family(p) || (p->dram_type == TYPE_PCDDR4)) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), CLEAR_FLD,
			MISC_CTRL1_R_DMDQSIENCG_EN);

		/*
		 * Bypass DQS glitch-free mode
		 * RG_RX_*RDQ_EYE_DLY_DQS_BYPASS_B**
		 */
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ6), SET_FLD,
			B0_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B1_DQ6), SET_FLD,
			B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ6), SET_FLD,
			B2_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B2);

		/*
		 * Enable DQ eye scan
		 * RG_??_RX_EYE_SCAN_EN
		 * RG_??_RX_VREF_EN
		 * RG_??_RX_SMT_EN
		 */
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_EYESCAN), SET_FLD,
			EYESCAN_RG_RX_EYE_SCAN_EN);
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_EYESCAN),
			p_fld(SET_FLD, EYESCAN_EYESCAN_DQS_SYNC_EN) |
			p_fld(SET_FLD, EYESCAN_EYESCAN_NEW_DQ_SYNC_EN) |
			p_fld(SET_FLD, EYESCAN_EYESCAN_DQ_SYNC_EN));
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), SET_FLD,
			B0_DQ5_RG_RX_ARDQ_EYE_EN_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B1_DQ5), SET_FLD,
			B1_DQ5_RG_RX_ARDQ_EYE_EN_B1);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ5), SET_FLD,
			B2_DQ5_RG_RX_ARDQ_EYE_EN_B2);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), SET_FLD,
			B0_DQ5_RG_RX_ARDQ_VREF_EN_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B1_DQ5), SET_FLD,
			B1_DQ5_RG_RX_ARDQ_VREF_EN_B1);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ5), SET_FLD,
			B2_DQ5_RG_RX_ARDQ_VREF_EN_B2);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ3), SET_FLD,
			B0_DQ3_RG_RX_ARDQ_SMT_EN_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B1_DQ3), SET_FLD,
			B1_DQ3_RG_RX_ARDQ_SMT_EN_B1);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B1_DQ3), SET_FLD,
			B2_DQ3_RG_RX_ARDQ_SMT_EN_B2);

		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ6), SET_FLD,
			B0_DQ6_RG_RX_ARDQ_JM_SEL_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B1_DQ6), SET_FLD,
			B1_DQ6_RG_RX_ARDQ_JM_SEL_B1);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ6), SET_FLD,
			B2_DQ6_RG_RX_ARDQ_JM_SEL_B2);
	} else {
#if 1	/* SUPPORT_TYPE_LPDDR3 */
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), CLEAR_FLD,
			MISC_CTRL1_R_DMDQSIENCG_EN);

		/*
		 * Bypass DQS glitch-free mode
		 * RG_RX_*RDQ_EYE_DLY_DQS_BYPASS_B**
		 */
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ6), SET_FLD,
			B0_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B0);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ6), SET_FLD,
			B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ6), SET_FLD,
			B2_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B2);

		/*
		 * Enable DQ eye scan
		 * RG_??_RX_EYE_SCAN_EN
		 * RG_??_RX_VREF_EN
		 * RG_??_RX_SMT_EN
		 */
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_EYESCAN), SET_FLD,
			EYESCAN_RG_RX_EYE_SCAN_EN);
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_EYESCAN),
			p_fld(SET_FLD, EYESCAN_EYESCAN_DQS_SYNC_EN) |
			p_fld(SET_FLD, EYESCAN_EYESCAN_NEW_DQ_SYNC_EN) |
			p_fld(SET_FLD, EYESCAN_EYESCAN_DQ_SYNC_EN));
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), SET_FLD,
			B0_DQ5_RG_RX_ARDQ_EYE_EN_B0);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ5), SET_FLD,
			B1_DQ5_RG_RX_ARDQ_EYE_EN_B1);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ5), SET_FLD,
			B2_DQ5_RG_RX_ARDQ_EYE_EN_B2);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), SET_FLD,
			B0_DQ5_RG_RX_ARDQ_VREF_EN_B0);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ5), SET_FLD,
			B1_DQ5_RG_RX_ARDQ_VREF_EN_B1);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ5), SET_FLD,
			B2_DQ5_RG_RX_ARDQ_VREF_EN_B2);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ3), SET_FLD,
			B0_DQ3_RG_RX_ARDQ_SMT_EN_B0);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ3), SET_FLD,
			B1_DQ3_RG_RX_ARDQ_SMT_EN_B1);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ3), SET_FLD,
			B2_DQ3_RG_RX_ARDQ_SMT_EN_B2);

		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ6), SET_FLD,
			B0_DQ6_RG_RX_ARDQ_JM_SEL_B0);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ6), SET_FLD,
			B1_DQ6_RG_RX_ARDQ_JM_SEL_B1);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ6), SET_FLD,
			B2_DQ6_RG_RX_ARDQ_JM_SEL_B2);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD6), SET_FLD,
			CA_CMD6_RG_RX_ARCMD_JM_SEL);
#endif

#if 0	/* SUPPORT_TYPE_LPDDR3 */
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ6), SET_FLD,
			B0_DQ6_RG_RX_ARDQ_JM_SEL_B0);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ6), SET_FLD,
			B1_DQ6_RG_RX_ARDQ_JM_SEL_B1);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ6), SET_FLD,
			B2_DQ6_RG_RX_ARDQ_JM_SEL_B2);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD6), SET_FLD,
			CA_CMD6_RG_RX_ARCMD_JM_SEL);
#endif
	}

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_EYESCAN), SET_FLD,
		EYESCAN_RG_RX_MIOCK_JIT_EN);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_EYESCAN), CLEAR_FLD,
		EYESCAN_RG_RX_EYE_SCAN_EN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_STBCAL1), CLEAR_FLD,
		STBCAL1_DQSERRCNT_DIS);
}

static unsigned char check_counter_jmeter(DRAMC_CTX_T *p,
	unsigned char delay, unsigned char sel_clk)
{
	unsigned char fgcurrent_value;
	unsigned int sample_cnt, ones_cnt[DQS_NUMBER];

	if (p->dram_type != TYPE_LPDDR3 && p->dram_type != TYPE_PCDDR3) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ4), delay,
			B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ4), delay,
			B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B1_DQ4), delay,
			B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B1_DQ4), delay,
			B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ4), delay,
			B2_DQ4_RG_RX_ARDQS_EYE_R_DLY_B2);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ4), delay,
			B2_DQ4_RG_RX_ARDQS_EYE_F_DLY_B2);
	} else {
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ4),
			delay, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ4),
			delay, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ4),
			delay, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ4),
			delay, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ4),
			delay, B2_DQ4_RG_RX_ARDQS_EYE_R_DLY_B2);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ4),
			delay, B2_DQ4_RG_RX_ARDQS_EYE_F_DLY_B2);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD4),
			delay, CA_CMD4_RG_RX_ARCLK_EYE_R_DLY);
		io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD4),
			delay, CA_CMD4_RG_RX_ARCLK_EYE_F_DLY);
	}

	dram_phy_reset(p);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_EYESCAN), 1,
		EYESCAN_REG_SW_RST);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_EYESCAN), 0,
		EYESCAN_REG_SW_RST);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_EYESCAN), 1,
		EYESCAN_RG_RX_EYE_SCAN_EN);

	delay_us(10);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_EYESCAN), 0,
		EYESCAN_RG_RX_EYE_SCAN_EN);

	sample_cnt = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_TOGGLE_CNT),
		TOGGLE_CNT_TOGGLE_CNT);
	ones_cnt[0] = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DQS0_ERR_CNT),
		DQS0_ERR_CNT_DQS0_ERR_CNT);
	ones_cnt[1] = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DQS1_ERR_CNT),
		DQS1_ERR_CNT_DQS1_ERR_CNT);
	ones_cnt[2] = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DQS2_ERR_CNT),
		DQS2_ERR_CNT_DQS2_ERR_CNT);
	ones_cnt[3] = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DQS3_ERR_CNT),
		DQS3_ERR_CNT_DQS3_ERR_CNT);

	show_msg2((INFO, "%d : %d, %d, %d, %d, %d\n",
		delay, sample_cnt, ones_cnt[0], ones_cnt[1], ones_cnt[2], ones_cnt[3]));

	/* change to boolean value */
	if (ones_cnt[0] < (sample_cnt / 2))
		fgcurrent_value = 0;
	else
		fgcurrent_value = 1;

	return fgcurrent_value;
}

unsigned short ca_delay_cell_time_lp4(DRAMC_CTX_T *p)
{
	unsigned int pll5_addr, pll8_addr, ca_cmd6;
	unsigned short real_freq;
	unsigned int sdm_pcw;
	unsigned int prediv;
	unsigned int posdiv;
	unsigned int vco_freq;
	unsigned int data_rate;
	unsigned int ckdiv4;

	//shulevel = io_32_read_fld_align(DRAMC_REG_SHUSTATUS,
	//		SHUSTATUS_SHUFFLE_LEVEL);
	pll5_addr = DDRPHY_SHU_PLL5;//cc mark + SHU_GRP_DDRPHY_OFFSET * shulevel;
	pll8_addr = DDRPHY_SHU_PLL8;// + SHU_GRP_DDRPHY_OFFSET * shulevel;
	ca_cmd6 = DDRPHY_SHU_CA_CMD6;// + SHU_GRP_DDRPHY_OFFSET * shulevel;
	sdm_pcw = io_32_read_fld_align(pll5_addr,
		SHU_PLL5_RG_RPHYPLL_SDM_PCW);
	prediv = io_32_read_fld_align(pll8_addr,
		SHU_PLL8_RG_RPHYPLL_PREDIV);
	posdiv = io_32_read_fld_align(pll8_addr,
		SHU_PLL8_RG_RPHYPLL_POSDIV);
	ckdiv4 = io_32_read_fld_align(ca_cmd6,
		SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA);
	vco_freq = ((52 >> prediv)*(sdm_pcw >> 8)) >> posdiv;
	data_rate = vco_freq >> ckdiv4;
	real_freq = data_rate >> 1;

	show_msg2((INFO, "prediv %d, posdiv %d, vco_freq %d, "
		"data_rate %d, real_freq %d\n",
		prediv, posdiv, vco_freq, data_rate, real_freq));

	return real_freq;
}

DRAM_STATUS_T dramc_miock_jmeter(DRAMC_CTX_T *p)
{
	unsigned char search_state, dqs_dly, fgcurrent_value, fginitial_value,
		start_period = 0, middle_period = 0, end_period = 0;
	unsigned char rxgatingpi, rxgatingpi_start, rxgatingpi_end;
	unsigned short real_freq, real_period;
	unsigned char step;

	if (!p)	{
		show_err("context NULL\n");
		return DRAM_FAIL;
	}
	delay_cell_ps[p->channel] = 0;

#if (FOR_DV_SIMULATION_USED == 0)
	step = 1;
#else
	step = 4;
#endif

	if (get_div_mode(p) == DIV8_MODE)
		rxgatingpi_start = 12;
	else
		rxgatingpi_start = 8;

	rxgatingpi_end = 63;

	unsigned int regbackupaddress[] = {
		(DRAMC_REG_ADDR(DRAMC_REG_EYESCAN)),
		(DRAMC_REG_ADDR(DRAMC_REG_STBCAL1)),
		(DRAMC_REG_ADDR(DDRPHY_B0_DQ6)),
		(DRAMC_REG_ADDR(DDRPHY_B1_DQ6)),
		(DRAMC_REG_ADDR(DDRPHY_B2_DQ6)),
		(DRAMC_REG_ADDR(DDRPHY_B0_DQ5)),
		(DRAMC_REG_ADDR(DDRPHY_B1_DQ5)),
		(DRAMC_REG_ADDR(DDRPHY_B2_DQ5)),
		(DRAMC_REG_ADDR(DDRPHY_B0_DQ3)),
		(DRAMC_REG_ADDR(DDRPHY_B1_DQ3)),
		(DRAMC_REG_ADDR(DDRPHY_B2_DQ3)),
		(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ7)),
		(DRAMC_REG_ADDR(DDRPHY_SHU_B1_DQ7)),
		(DRAMC_REG_ADDR(DDRPHY_B0_DQ4)),
		(DRAMC_REG_ADDR(DDRPHY_B1_DQ4)),
		(DRAMC_REG_ADDR(DDRPHY_B2_DQ4)),
		(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1)),
		(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN)),
		(DRAMC_REG_ADDR(DRAMC_REG_SHURK1_DQSIEN)),
		((DDRPHY_CA_DLL_ARPI2)),
		((DDRPHY_B0_DLL_ARPI2)),
		((DDRPHY_B1_DLL_ARPI2)),
		((DDRPHY_B2_DLL_ARPI2)),
		((DDRPHY_CA_DLL_ARPI2) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_B0_DLL_ARPI2) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_B1_DLL_ARPI2) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_B2_DLL_ARPI2) + SHIFT_TO_CHB_ADDR),
	};

#if SUPPORT_TYPE_LPDDR3 || SUPPORT_TYPE_PCDDR3
	unsigned int regbackupaddress_lp3[] = {
		(DRAMC_REG_ADDR(DRAMC_REG_EYESCAN)),
		(DRAMC_REG_ADDR(DRAMC_REG_STBCAL1)),
		(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN)),
		(DRAMC_REG_ADDR(DRAMC_REG_SHURK1_DQSIEN)),

		(DRAMC_REG_ADDR(DDRPHY_B0_DQ6)),
		(DRAMC_REG_ADDR(DDRPHY_B1_DQ6)),
		(DRAMC_REG_ADDR(DDRPHY_B2_DQ6)),
		(DRAMC_REG_ADDR(DDRPHY_B0_DQ5)),
		(DRAMC_REG_ADDR(DDRPHY_B1_DQ5)),
		(DRAMC_REG_ADDR(DDRPHY_B2_DQ5)),
		(DRAMC_REG_ADDR(DDRPHY_B0_DQ3)),
		(DRAMC_REG_ADDR(DDRPHY_B1_DQ3)),
		(DRAMC_REG_ADDR(DDRPHY_B2_DQ3)),
		(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ7)),
		(DRAMC_REG_ADDR(DDRPHY_SHU_B1_DQ7)),
		(DRAMC_REG_ADDR(DDRPHY_B0_DQ4)),
		(DRAMC_REG_ADDR(DDRPHY_B1_DQ4)),
		(DRAMC_REG_ADDR(DDRPHY_B2_DQ4)),
		(DRAMC_REG_ADDR(DDRPHY_CA_CMD4)),
		(DRAMC_REG_ADDR(DDRPHY_CA_CMD6)),
		(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1)),

		(DRAMC_REG_ADDR(DDRPHY_B0_DQ6) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_B1_DQ6) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_B2_DQ6) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_B0_DQ5) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_B1_DQ5) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_B2_DQ5) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_B0_DQ3) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_B1_DQ3) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_B2_DQ3) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ7) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_SHU_B1_DQ7) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_B0_DQ4) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_B1_DQ4) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_B2_DQ4) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_CA_CMD4) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_CA_CMD6) + SHIFT_TO_CHB_ADDR),
		(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1) + SHIFT_TO_CHB_ADDR),

		((DDRPHY_CA_DLL_ARPI2)),
		((DDRPHY_B0_DLL_ARPI2)),
		((DDRPHY_B1_DLL_ARPI2)),
		((DDRPHY_B2_DLL_ARPI2)),
		((DDRPHY_CA_DLL_ARPI2) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_B0_DLL_ARPI2) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_B1_DLL_ARPI2) + SHIFT_TO_CHB_ADDR),
		((DDRPHY_B2_DLL_ARPI2) + SHIFT_TO_CHB_ADDR),
	};
#endif

	if (p->dram_type == TYPE_LPDDR3 || p->dram_type == TYPE_PCDDR3) {
	#if SUPPORT_TYPE_LPDDR3 || SUPPORT_TYPE_PCDDR3
		dramc_backup_registers(p, regbackupaddress_lp3,
			sizeof (regbackupaddress_lp3) / sizeof (unsigned int));
	#endif
	} else {
		dramc_backup_registers(p, regbackupaddress,
			sizeof (regbackupaddress) / sizeof (unsigned int));
	}

	set_enable_miock_jmeter_rg(p);

	for (rxgatingpi = rxgatingpi_start; rxgatingpi < rxgatingpi_end; rxgatingpi += 4) {
		show_msg2((INFO, "\n[DramcMiockJmeter] rxgatingpi = %d\n", rxgatingpi));

		search_state = 0;

		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN),
			p_fld(rxgatingpi, SHURK0_DQSIEN_R0DQS0IEN) |
			p_fld(rxgatingpi, SHURK0_DQSIEN_R0DQS1IEN) |
			p_fld(rxgatingpi, SHURK0_DQSIEN_R0DQS2IEN) |
			p_fld(rxgatingpi, SHURK0_DQSIEN_R0DQS3IEN));

		/* to see 1T(H,L) or 1T(L,H) from delaycell=0 to 127 */
		for (dqs_dly = 0; dqs_dly < 128; dqs_dly += step) {

			fgcurrent_value = check_counter_jmeter(p, dqs_dly, FALSE);

			/* more than 1T data */
			if (search_state == 0) {
				/* record initial value at the beginning */
				fginitial_value = fgcurrent_value;
				search_state = 1;
			} else if (search_state == 1) {
				/*  check if change value */
				if (fgcurrent_value != fginitial_value) {
					/* start of the period */
					fginitial_value = fgcurrent_value;
					start_period = dqs_dly;
					search_state = 2;
				}
			} else if (search_state == 2) {
				/*  check if change value */
				if (fgcurrent_value != fginitial_value) {
					fginitial_value = fgcurrent_value;
					middle_period = dqs_dly;
					search_state = 3;
				}
			} else if (search_state == 3) {
				/*  check if change value */
				if (fgcurrent_value != fginitial_value) {
					/* end of the period, break the loop */
					end_period = dqs_dly;
					search_state = 4;
					break;
				}
			} else { /* nothing */
			}
		}

		if ((search_state == 3) || (search_state == 4))
			break;
	}

	if (p->dram_type == TYPE_LPDDR3 || p->dram_type == TYPE_PCDDR3) {
#if SUPPORT_TYPE_LPDDR3 || SUPPORT_TYPE_PCDDR3
		dramc_restore_registers(p, regbackupaddress_lp3,
			sizeof (regbackupaddress_lp3) / sizeof (unsigned int));
#endif
	} else {
		dramc_restore_registers(p, regbackupaddress,
			sizeof (regbackupaddress) / sizeof (unsigned int));
	}

	if (search_state != 4) {
		if (search_state != 3) {
			show_msg((INFO, "\n\tMIOCK jitter meter - ch=%d\n",
				p->channel));
			show_msg((INFO,
				"\tLess than 0.5T data."));
			show_msg((INFO,
				"Cannot calculate delay cell time\n\n"));
			return DRAM_FAIL;
		}

		/* Calculate 1 delay cell = ? ps */
		num_dlycell_per_t[p->channel] =
			(middle_period - start_period) * 2;
	} else {
		/* Calculate 1 delay cell = ? ps */
		num_dlycell_per_t[p->channel] = (end_period - start_period);
	}

	show_msg((INFO, "\n\tMIOCK jitter meter\tch=%d\n\n ",
		p->channel));
	if (search_state == 4) {
		show_msg((INFO, " 1T = (%d-%d) = %d dly cells\n",
			end_period, start_period,
			num_dlycell_per_t[p->channel]));
	} else {
		show_msg((INFO, " 1T = (%d-%d)*2 = %d dly cells\n",
			middle_period, start_period,
			num_dlycell_per_t[p->channel]));
	}

	real_freq = ca_delay_cell_time_lp4(p);
	real_period = (unsigned short) (1000000 / real_freq);
	/* calculate delay cell time */

	if (num_dlycell_per_t[p->channel] == 0) {
		delay_cell_ps[p->channel] = 0;
	} else {
		delay_cell_ps[p->channel] =
			real_period * 100 / num_dlycell_per_t[p->channel];
	}

	show_msg((INFO, "Clock freq = %d MHz, ", real_freq));
	show_msg((INFO, "period = %d ps, 1 dly cell = %d/100 ps\n",
		real_period, delay_cell_ps[p->channel]));
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
	show_msg((INFO, "[HQALOG]dly cell = %d \n", delay_cell_ps[p->channel]));
#endif

	return DRAM_OK;
}
#endif

#ifdef ENABLE_MIOCK_JMETER
/*
 * "picoseconds per delay cell" depends on Vcore only
  * (frequency doesn't matter)
 * 1. Retrieve current freq's vcore voltage using pmic API
 * 2. Perform delay cell time calculation
 * (Bypass if shuffle vcore value is the same as before)
 */
static void get_vcore_delay_cell_time(DRAMC_CTX_T *p, unsigned char shuffleIdx)
{
	unsigned int channel_i;

	dramc_miock_jmeter(p);

	for (channel_i = CHANNEL_A; channel_i < CHANNEL_NUM;
		channel_i++) {
		num_dlycell_per_t_all[shuffleIdx][channel_i] =
			num_dlycell_per_t[p->channel];
		delay_cell_ps_all[shuffleIdx][channel_i] =
			delay_cell_ps[p->channel];
	}
}

unsigned char get_hqa_shuffle_idx(DRAMC_CTX_T *p)
{
	unsigned char shuffle_idx = 0;
	DRAM_DFS_FREQUENCY_TABLE_T *pfreq_tbl;

	if (is_lp4_family(p)) {
	#if SUPPORT_TYPE_LPDDR4
		pfreq_tbl = freq_tbl_lp4;
	#endif
	} else if (p->dram_type == TYPE_LPDDR3) {
	#if SUPPORT_TYPE_LPDDR3
		pfreq_tbl = freq_tbl_lp3;
	#endif
	} else {
		return DRAM_DFS_SHUFFLE_1;
	}

	/* Retrieve shuffle number from gFreqTbl */
	for (shuffle_idx = 0; shuffle_idx < DRAM_DFS_SHUFFLE_MAX;
		shuffle_idx++) {
		if (pfreq_tbl[shuffle_idx].frequency == p->frequency)
			break;
	}

	if (shuffle_idx == DRAM_DFS_SHUFFLE_MAX)
		show_log("shuffle num err!\n");
	else
		show_msg3((INFO, "shuffleIdx %d\n", shuffle_idx));

	return shuffle_idx;

	return DRAM_DFS_SHUFFLE_1;
}

void dramc_miock_jmeter_hqa(DRAMC_CTX_T *p)
{
	/* do MiockJitterMeter@DDR2667 */
	unsigned int channel_idx;
	unsigned char shuffle_idx;

	show_msg((INFO, "[MiockJmeterHQA]\n"));

	shuffle_idx = get_hqa_shuffle_idx(p);
	get_vcore_delay_cell_time(p, shuffle_idx);

	/* Use highest freq's delay cell time measure results as reference */
	p->ucnum_dlycell_perT = num_dlycell_per_t_all[shuffle_idx][p->channel];
	p->delay_cell_timex100 = delay_cell_ps_all[shuffle_idx][p->channel];

	show_msg3((INFO, "DelayCellTimex100 CH_%d, (VCORE=%d, cell=%d)\n",
		p->channel, vcore_value[shuffle_idx], p->delay_cell_timex100));
}

#endif /* #ifdef ENABLE_MIOCK_JMETER */

#if (defined(ENABLE_DUTY_SCAN) && defined(ENABLE_MIOCK_JMETER))
static void set_enable_duty_scan_rg(DRAMC_CTX_T *p)
{
	/* MCK4X CG */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), CLEAR_FLD,
		MISC_CTRL1_R_DMDQSIENCG_EN);

	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL),
		CLEAR_FLD, DRAMC_PD_CTRL_DCMEN);

	/*
	* Enable DQ eye scan
	* RG_??_RX_EYE_SCAN_EN
	* RG_??_RX_VREF_EN
	* RG_??_RX_SMT_EN
	*/
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_PLL1), SET_FLD,
		PLL1_RG_RX_EYE_SCAN_EN);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD3), SET_FLD,
		CA_CMD3_RG_RX_ARCMD_SMT_EN);
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD2),
		p_fld(SET_FLD, CA_CMD2_RG_TX_ARCLK_JM_EN) |
		p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCLK_JM_SEL)); /* LPBK_CLK */

	/* Enable MIOCK jitter meter mode ( RG_RX_MIOCK_JIT_EN=1) */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_PLL1), SET_FLD,
		PLL1_RG_RX_MIOCK_JIT_EN);

	/* Disable DQ eye scan (b'1), for counter clear */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_PLL1), CLEAR_FLD,
		PLL1_RG_RX_EYE_SCAN_EN);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_PLL1), CLEAR_FLD,
		PLL1_R_DMDQSERRCNT_DIS);
}

static void set_clk_duty_code(DRAMC_CTX_T *p, signed char duty_code)
{
	if (duty_code < 0) {
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD2),
			p_fld(SET_FLD, CA_CMD2_RG_TX_ARCLK_MCK4X_DLY_EN) |
			p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCLK_MCK4XB_DLY_EN));
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD10),
			p_fld(-duty_code, SHU_R0_CA_CMD10_RG_RK0_TX_ARCLK_DLY) |
			p_fld(0x0, SHU_R0_CA_CMD10_RG_RK0_TX_ARCLKB_DLY));
	} else {
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD2),
			p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCLK_MCK4X_DLY_EN) |
			p_fld(SET_FLD, CA_CMD2_RG_TX_ARCLK_MCK4XB_DLY_EN));
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD10),
			p_fld(0x0, SHU_R0_CA_CMD10_RG_RK0_TX_ARCLK_DLY) |
			p_fld(duty_code, SHU_R0_CA_CMD10_RG_RK0_TX_ARCLKB_DLY));
	}
}

DRAM_STATUS_T dramc_clk_duty_scan(DRAMC_CTX_T *p)
{
	unsigned char search_state, clk_dly, fgcurrent_value, fginitial_value,
		start_period = 0, middle_period = 0, end_period = 0;
	unsigned short real_freq, real_period;
	unsigned short high_pulse_width, period_width;
	unsigned int duty_cycle;
	unsigned char rx_gating_pi = 0;
	signed char duty_code, best_code;
	unsigned short diff, min_diff;
	unsigned char step;
	unsigned int reg_backup_address[] = {
		(DRAMC_REG_DRAMC_PD_CTRL),
		((DDRPHY_PLL1)),
		((DDRPHY_PLL2)),
		((DDRPHY_PLL4)),

		(DDRPHY_CA_CMD2),
		(DDRPHY_CA_CMD3),
		(DDRPHY_MISC_CTRL1),
	};

	rx_gating_pi = 0x10;

#if (FOR_DV_SIMULATION_USED == 0)
	step = 1;
#else
	step = 4;
#endif

	dramc_backup_registers(p, reg_backup_address,
		sizeof(reg_backup_address) / sizeof(unsigned int));

	set_enable_duty_scan_rg(p);

	min_diff = 1000;
	for (duty_code = -7; duty_code < 8; duty_code++) {
		/* Adjust clock duty */
		set_clk_duty_code(p, duty_code);

		search_state = 0;

		/* to see 1T(H,L) or 1T(L,H) from delaycell=0 to 127 */
		for (clk_dly = 0; clk_dly < 128; clk_dly += step) {

			fgcurrent_value = check_counter_jmeter(p, clk_dly, TRUE);

			/* more than 1T data */
			if (search_state == 0) {
				/* record initial value at the beginning */
				fginitial_value = fgcurrent_value;
				search_state = 1;
			} else if (search_state == 1) {
				/*  check if change value */
				if (fgcurrent_value != fginitial_value) {
					/* start of the period */
					fginitial_value = fgcurrent_value;
					start_period = clk_dly;
					search_state = 2;
				}
			} else if (search_state == 2) {
				/*  check if change value */
				if (fgcurrent_value != fginitial_value) {
					fginitial_value = fgcurrent_value;
					middle_period = clk_dly;
					search_state = 3;
				}
			} else if (search_state == 3) {
				/*  check if change value */
				if (fgcurrent_value != fginitial_value) {
					/* end of the period, break the loop */
					end_period = clk_dly;
					search_state = 4;
					break;
				}
			} else { /* nothing */
			}
		}

		if (search_state != 4) {
			show_msg2((INFO, "Cannot find 1T to calculate duty\n"));
			break;
		} else {
			if (fgcurrent_value == 1) {
				/* The end is a rising edge */
				high_pulse_width = middle_period - start_period;
			} else {
				/* Falling edge ending */
				high_pulse_width = end_period - middle_period;
			}

			period_width = end_period - start_period;
			duty_cycle = (high_pulse_width * 1000) / period_width;
			show_msg2((INFO, "The duty cycle with duty_code %d is %d.%d\n",
				duty_code, duty_cycle/10,
				duty_cycle - (duty_cycle/10)*10));
		}

		if (duty_cycle > 500) {
			diff = duty_cycle - 500;
		} else {
			diff = 500 - duty_cycle;
		}

		if (diff < min_diff) {
			min_diff = diff;
			best_code = duty_code;
			show_msg2((INFO, "Current best code = %d\n", best_code));
		}
	}

	/* Set best code to RG */
	set_clk_duty_code(p, best_code);

	/* restore to orignal value */
	dramc_restore_registers(p, reg_backup_address,
		sizeof(reg_backup_address) / sizeof(unsigned int));

	return DRAM_OK;
}

#endif

void dramc_write_dbi_on_off(DRAMC_CTX_T *p, unsigned char onoff)
{
	/*  DRAMC Write-DBI On/Off */
	io_32_write_fld_align_all(DRAMC_REG_SHU_WODT, onoff,
		SHU_WODT_DBIWR);
	show_msg((INFO, "DramC Write-DBI %s\n",
		((onoff == DBI_ON) ? "on" : "off")));
}

void dramc_read_dbi_on_off(DRAMC_CTX_T *p, unsigned char onoff)
{
	/*  DRAMC Read-DBI On/Off */
	io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ7, onoff,
		SHU_B0_DQ7_R_DMDQMDBI_SHU_B0);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ7, onoff,
		SHU_B1_DQ7_R_DMDQMDBI_SHU_B1);
	show_msg((INFO, "DramC Read-DBI %s\n",
		((onoff == DBI_ON) ? "on" : "off")));
}

#if ENABLE_WRITE_DBI || TX_K_DQM_WITH_WDBI
void dramc_write_minus_1mck_for_write_dbi(DRAMC_CTX_T *p,
	signed char shift_ui)
{
	REG_TRANSFER_T transfer_reg[2];
	if (p->dbi_w_onoff[p->dram_fsp]) {
		/* DQ0 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ2;
		transfer_reg[0].fld = SHURK0_SELPH_DQ2_DLY_DQ0;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ0;
		transfer_reg[1].fld = SHURK0_SELPH_DQ0_TXDLY_DQ0;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);

		/* DQ1 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ2;
		transfer_reg[0].fld = SHURK0_SELPH_DQ2_DLY_DQ1;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ0;
		transfer_reg[1].fld = SHURK0_SELPH_DQ0_TXDLY_DQ1;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);

		/* DQM0 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ3;
		transfer_reg[0].fld = SHURK0_SELPH_DQ3_DLY_DQM0;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ1;
		transfer_reg[1].fld = SHURK0_SELPH_DQ1_TXDLY_DQM0;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);

		/* DQM1 */
		transfer_reg[0].addr = DRAMC_REG_SHURK0_SELPH_DQ3;
		transfer_reg[0].fld = SHURK0_SELPH_DQ3_DLY_DQM1;
		transfer_reg[1].addr = DRAMC_REG_SHURK0_SELPH_DQ1;
		transfer_reg[1].fld = SHURK0_SELPH_DQ1_TXDLY_DQM1;
		execute_move_dramc_delay(p, transfer_reg, shift_ui);
	}
}

void set_dram_mode_reg_for_write_dbi_on_off(DRAMC_CTX_T *p,
	unsigned char onoff)
{
	/* DRAM MR3[7] write-DBI On/Off */
	dram_mr.mr03_value[p->dram_fsp] =
		((dram_mr.mr03_value[p->dram_fsp] & 0x7F) | (onoff << 7));
	dramc_mode_reg_write_by_rank(p, p->rank, MR03,
		dram_mr.mr03_value[p->dram_fsp]);
}
#endif

#if ENABLE_WRITE_DBI
void apply_write_dbi_power_improve(DRAMC_CTX_T *p, unsigned char onoff)
{
}
#endif

#if ENABLE_DUTY_SCAN_V2
#define DutyPrintAllLog         0
#define DutyPrintCalibrationLog 1

#define DUTY_OFFSET_START -8
#define DUTY_OFFSET_END 8

#define CLOCK_PI_START 0
#define CLOCK_PI_END 63
#define CLOCK_PI_STEP 2

#define ClockDutyFailLowerBound 4500    // 45%
#define ClockDutyFailUpperBound 5500    // 55%
#define ClockDutyMiddleBound    5000    // 50%

void DramcClockDutySetClkDelayCell(DRAMC_CTX_T *p, unsigned char u1RankIdx, signed char scDutyDelay, unsigned char use_rev_bit)
{
    unsigned char u1ShuffleIdx = 0;
    unsigned int save_offset;
    unsigned char ucDelay, ucDelayB;
    unsigned char ucRev_Bit0=0, ucRev_Bit1=0;

//    show_msg2((INFO, "CH%d, Final CLK duty delay cell = %d\n", p->channel, scDutyDelay));

    if (scDutyDelay<0)
    {
        ucDelay = -scDutyDelay;
        ucDelayB = 0;

        if (use_rev_bit)
        {
            ucRev_Bit0 = 1;
            ucRev_Bit1 = 0;
        }
    }
    else if (scDutyDelay>0)
    {
        ucDelay = 0;
        ucDelayB= scDutyDelay;

        if (use_rev_bit)
        {
            ucRev_Bit0 = 0;
            ucRev_Bit1 = 1;
        }
    }
    else
    {
        ucDelay = 0;
        ucDelayB= 0;

        if (use_rev_bit)
        {
            ucRev_Bit0 = 0;
            ucRev_Bit1 = 0;
        }
    }

#if DUTY_SCAN_V2_ONLY_K_HIGHEST_FREQ
    for(u1ShuffleIdx = 0; u1ShuffleIdx<DRAM_DFS_SHUFFLE_MAX; u1ShuffleIdx++)
#endif
    {
        save_offset = u1ShuffleIdx * SHU_GRP_DDRPHY_OFFSET + u1RankIdx*0x100;
        io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD1) + save_offset, p_fld(ucDelay, SHU_R0_CA_CMD1_RK0_TX_ARCLK_DLY) | p_fld(ucDelay, SHU_R0_CA_CMD1_RK0_TX_ARCLKB_DLY));
        io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD0) + save_offset, p_fld(ucDelayB, SHU_R0_CA_CMD0_RK0_TX_ARCLK_DLYB) | p_fld(ucDelayB, SHU_R0_CA_CMD0_RK0_TX_ARCLKB_DLYB));

        save_offset = u1ShuffleIdx * SHU_GRP_DDRPHY_OFFSET;
        io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD3), p_fld(ucRev_Bit0, SHU_CA_CMD3_RG_TX_ARCMD_PU_BIT0) | p_fld(ucRev_Bit1, SHU_CA_CMD3_RG_TX_ARCMD_PU_BIT1));
    }
}

void DQSDutyScan_SetDqsDelayCell(DRAMC_CTX_T *p, signed char *scDutyDelay, unsigned char use_rev_bit)
{
    unsigned char u1ShuffleIdx = 0, u1DQSIdx, u1RankIdx = 0;
    unsigned int save_offset;
    unsigned char u1Delay[2], u1DelayB[2];
    unsigned char ucRev_Bit0[2]={0,0}, ucRev_Bit1[2]={0,0};

//    show_msg2((INFO, "CH%d, Final DQS0 duty delay cell = %d\n", p->channel, scDutyDelay[0]));
//    show_msg2((INFO, "CH%d, Final DQS1 duty delay cell = %d\n", p->channel, scDutyDelay[1]));

    for(u1DQSIdx=0; u1DQSIdx<2; u1DQSIdx++)
    {
        if(scDutyDelay[u1DQSIdx] <0)
        {
            u1Delay[u1DQSIdx]  = -(scDutyDelay[u1DQSIdx]);
            u1DelayB[u1DQSIdx]  =0;

            if (use_rev_bit)
            {
                ucRev_Bit0[u1DQSIdx] = 1;
                ucRev_Bit1[u1DQSIdx] = 0;
            }
        }
        else if(scDutyDelay[u1DQSIdx] >0)
        {
            u1Delay[u1DQSIdx]  = 0;
            u1DelayB[u1DQSIdx]  = scDutyDelay[u1DQSIdx];

            if (use_rev_bit)
            {
                ucRev_Bit0[u1DQSIdx] = 0;
                ucRev_Bit1[u1DQSIdx] = 1;
            }
        }
        else
        {
            u1Delay[u1DQSIdx]  = 0;
            u1DelayB[u1DQSIdx]  =0;

            if (use_rev_bit)
            {
                ucRev_Bit0[u1DQSIdx] = 0;
                ucRev_Bit1[u1DQSIdx] = 0;
            }
        }
    }

#if DUTY_SCAN_V2_ONLY_K_HIGHEST_FREQ
    for(u1ShuffleIdx = 0; u1ShuffleIdx<DRAM_DFS_SHUFFLE_MAX; u1ShuffleIdx++)
#endif
    {
        for(u1RankIdx = 0; u1RankIdx<RANK_MAX; u1RankIdx++)
        {
            for(u1DQSIdx = 0; u1DQSIdx<2; u1DQSIdx++)
            {
                save_offset = u1ShuffleIdx * SHU_GRP_DDRPHY_OFFSET + u1RankIdx*0x100;
                io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ1 + u1DQSIdx*0x2000) + save_offset, p_fld(u1Delay[u1DQSIdx], SHU_R0_B0_DQ1_RK0_TX_ARDQS0_DLY_B0) | p_fld(u1Delay[u1DQSIdx], SHU_R0_B0_DQ1_RK0_TX_ARDQS0B_DLY_B0));
                io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ1 + u1DQSIdx*0x2000) + save_offset, p_fld(u1DelayB[u1DQSIdx], SHU_R0_B0_DQ1_RK0_TX_ARDQS0_DLYB_B0) | p_fld(u1DelayB[u1DQSIdx], SHU_R0_B0_DQ1_RK0_TX_ARDQS0B_DLYB_B0));

                save_offset = u1ShuffleIdx * SHU_GRP_DDRPHY_OFFSET;
                io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DLL1 + u1DQSIdx*0x2000) + save_offset, p_fld(ucRev_Bit0[u1DQSIdx], RG_ARDQ_REV_BIT_00_DQS_MCK4X_DLY_EN) | p_fld(ucRev_Bit1[u1DQSIdx], RG_ARDQ_REV_BIT_01_DQS_MCK4XB_DLY_EN));
            }
        }
    }
}

// offset is not related to DQ/DQM/DQS
// we have a circuit to measure duty, But this circuit is not very accurate
// so we need to K offset of this circuit first
// After we got this offset, then we can use it to measure duty
// this offset can measure DQ/DQS/DQM, and every byte has this circuit, too.
// B0/B2/CA all have one circuit.
// CA's circuit can measure CLK duty
// B0/B2's can measure DQ/DQM/DQS duty
signed char DutyScan_Offset_Convert(unsigned char val)
{
    unsigned char calibration_sequence[15]={0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};

    return ((signed char)(calibration_sequence[val]>8 ? 0-(calibration_sequence[val]&0x7) : calibration_sequence[val]));

}
#define SHIFT_OFFSET_B0_TO_B2	(0x2000)

void DutyScan_Offset_Calibration(DRAMC_CTX_T *p)
{
    unsigned char calibration_sequence[15]={0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};
    unsigned char i, read_val_b0, read_val_b1, read_val_ca;
    unsigned char cal_i_b0=0xff, cal_i_b1=0xff, cal_i_ca=0xff;

#if VENDER_JV_LOG
        vPrintCalibrationBasicInfo_ForJV(p);
#else
        print_calibration_basic_info(p);
#endif

#if DutyPrintCalibrationLog
    show_msg((INFO, "[Duty_Offset_Calibration]\n\n"));
#endif

    //B0
    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B0_DQ6), p_fld(1, B0_DQ6_RG_RX_ARDQ_LPBK_EN_B0) | p_fld(0, B0_DQ6_RG_RX_ARDQ_DDR4_SEL_B0) | p_fld(1, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0));
    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DLL1), p_fld(1, RG_ARDQ_REV_BIT_20_DATA_SWAP_EN) | p_fld(2, RG_ARDQ_REV_BIT_2221_DATA_SWAP));
//    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ3), p_fld(1, SHU_B0_DQ3_DQ_REV_B0_BIT_06) | p_fld(0, SHU_B0_DQ3_DQ_REV_B0_BIT_05) | p_fld(1, SHU_B0_DQ3_DQ_REV_B0_BIT_04));
    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ5), p_fld(0, SHU_B0_DQ5_RG_RX_ARDQ_VREF_BYPASS_B0) | p_fld(0xB, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0));
    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ0), 1, SHU_B0_DQ0_RG_TX_ARDQS0_DRVP_PRE_B0_BIT1);
    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ0), 0, B0_DQ0_RG_RX_ARDQ2_OFFC_B0);
    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), p_fld(1, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0) | p_fld(0x1, B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0));
    delay_us(1);
    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ0), 1, SHU_B0_DQ0_RG_TX_ARDQS0_DRVP_PRE_B0_BIT2);

    //B2
    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B2_DQ6), p_fld(1, B2_DQ6_RG_RX_ARDQ_LPBK_EN_B2) | p_fld(0, B2_DQ6_RG_RX_ARDQ_DDR4_SEL_B2) | p_fld(1, B2_DQ6_RG_RX_ARDQ_DDR3_SEL_B2));
    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DLL1), p_fld(1, RG_ARDQ_REV_BIT_20_DATA_SWAP_EN) | p_fld(2, RG_ARDQ_REV_BIT_2221_DATA_SWAP));
//    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ3), p_fld(1, SHU_B2_DQ3_DQ_REV_B2_BIT_06) | p_fld(0, SHU_B2_DQ3_DQ_REV_B2_BIT_05) | p_fld(1, SHU_B2_DQ3_DQ_REV_B2_BIT_04));
    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ5), p_fld(0, SHU_B2_DQ5_RG_RX_ARDQ_VREF_BYPASS_B2) | p_fld(0xB, SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2));
    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ0), 1, SHU_B2_DQ0_RG_TX_ARDQS0_DRVP_PRE_B2_BIT1);
    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ0), 0, B2_DQ0_RG_RX_ARDQ2_OFFC_B2);
    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B2_DQ5), p_fld(1, B2_DQ5_RG_RX_ARDQ_VREF_EN_B2) | p_fld(0x1, B2_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B2));
    delay_us(1);
    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ0), 1, SHU_B2_DQ0_RG_TX_ARDQS0_DRVP_PRE_B2_BIT2);

    //CA
    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_CA_CMD6), p_fld(1, CA_CMD6_RG_RX_ARCMD_LPBK_EN) | p_fld(0, CA_CMD6_RG_RX_ARCMD_DDR4_SEL) | p_fld(1, CA_CMD6_RG_RX_ARCMD_DDR3_SEL));
    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_CA_DLL1), p_fld(1, RG_ARCMD_REV_BIT_20_DATA_SWAP_EN) | p_fld(2, RG_ARCMD_REV_BIT_2221_DATA_SWAP));
//    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD3), p_fld(1, SHU_CA_CMD3_ARCMD_REV_BIT_06) | p_fld(0, SHU_CA_CMD3_ARCMD_REV_BIT_05) | p_fld(1, SHU_CA_CMD3_ARCMD_REV_BIT_04));
    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD5), p_fld(0, SHU_CA_CMD5_RG_RX_ARCMD_VREF_BYPASS) | p_fld(0xB, SHU_CA_CMD5_RG_RX_ARCMD_VREF_SEL));
    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD0), 1, SHU_CA_CMD0_RG_TX_ARCLK_DRVP_PRE_BIT1);
    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_CA_CMD0), 0, CA_CMD0_RG_RX_ARCA2_OFFC);
    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_CA_CMD5), p_fld(1, CA_CMD5_RG_RX_ARCMD_VREF_EN) | p_fld(0x1, CA_CMD5_RG_RX_ARCMD_EYE_VREF_EN));
    delay_us(1);
    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD0), 1, SHU_CA_CMD0_RG_TX_ARCLK_DRVP_PRE_BIT2);

    delay_us(1);

    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ0), 1, SHU_B0_DQ0_RG_TX_ARDQS0_DRVP_PRE_B0_BIT0);
    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ0), 1, SHU_B2_DQ0_RG_TX_ARDQS0_DRVP_PRE_B2_BIT0);
    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD0), 1, SHU_CA_CMD0_RG_TX_ARCLK_DRVP_PRE_BIT0);

#if DutyPrintCalibrationLog
    show_msg((INFO, "\tB0\tB2\tCA\n"));
    show_msg((INFO, "===========================\n"));
#endif

    for(i=0; i<15; i++)
    {
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ0), calibration_sequence[i], B0_DQ0_RG_RX_ARDQ2_OFFC_B0);
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ0), calibration_sequence[i], B2_DQ0_RG_RX_ARDQ2_OFFC_B2);
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_CA_CMD0), calibration_sequence[i], CA_CMD0_RG_RX_ARCA2_OFFC);

        delay_us(1);

        read_val_b0 = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_AD_RX_DQ_O1), MISC_AD_RX_DQ_O1_AD_RX_ARDQ_O1_B0_BIT2);
		read_val_b1 = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_AD_RX_DQ_O1_2), MISC_AD_RX_DQ_O1_AD_RX_ARDQ_O1_B0_BIT2);
        read_val_ca = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_AD_RX_CMD_O1), MISC_AD_RX_CMD_O1_AD_RX_ARCA2_O1);

#if DutyPrintCalibrationLog
        show_msg((INFO, "%d\t%d\t%d\t%d\n", DutyScan_Offset_Convert(i), read_val_b0, read_val_b1, read_val_ca));
#endif

        if (read_val_b0 == 0 && cal_i_b0==0xff)
        {
            cal_i_b0 = i;
        }

        if (read_val_b1 == 0 && cal_i_b1==0xff)
        {
            cal_i_b1 = i;
        }

        if (read_val_ca == 0 && cal_i_ca==0xff)
        {
            cal_i_ca = i;
        }
    }

    if (cal_i_b0==0 || cal_i_b1==0 || cal_i_ca==0)
    {
#if DutyPrintCalibrationLog
        show_msg((INFO, "offset calibration i=-7 and AD_RX_*RDQ_O1_B*<2>/AD_RX_*RCA2_O1 ==0 !!\n"));
#endif
#if __ETT__
        while(1);
#endif
    }
    else
    if ((read_val_b0==1 && cal_i_b0==0xff) || (read_val_b1==1 && cal_i_b1==0xff) || (read_val_ca==1 && cal_i_ca==0xff))
    {
#if DutyPrintCalibrationLog
        show_msg((INFO, "offset calibration i=7 and AD_RX_*RDQ_O1_B*<2>/AD_RX_*RCA2_O1 ==1 !!\n"));
#endif
#if __ETT__
        while(1);
#endif

    }
    else
    {
#if DutyPrintCalibrationLog
        show_msg((INFO, "===========================\n"));
        show_msg((INFO, "\tB0:%d\tB2:%d\tCA:%d\n",DutyScan_Offset_Convert(cal_i_b0),DutyScan_Offset_Convert(cal_i_b1),DutyScan_Offset_Convert(cal_i_ca)));
#endif
    }

    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ0), 0, SHU_B0_DQ0_RG_TX_ARDQS0_DRVP_PRE_B0_BIT0);
    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ0), 0, SHU_B2_DQ0_RG_TX_ARDQS0_DRVP_PRE_B2_BIT0);
    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD0), 0, SHU_CA_CMD0_RG_TX_ARCLK_DRVP_PRE_BIT0);

    if (cal_i_b0!=0xff) io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ0), calibration_sequence[cal_i_b0], B0_DQ0_RG_RX_ARDQ2_OFFC_B0);
    if (cal_i_b1!=0xff) io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ0), calibration_sequence[cal_i_b1], B2_DQ0_RG_RX_ARDQ2_OFFC_B2);
    if (cal_i_ca!=0xff) io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_CA_CMD0), calibration_sequence[cal_i_ca], CA_CMD0_RG_RX_ARCA2_OFFC);

    return;
}

#if defined(YH_SWEEP_IC)
typedef struct _YH_SWEEP_IC_T
{
    unsigned int maxduty;
    unsigned int minduty;
    unsigned int dutydiff;
    unsigned int avgduty;
} YH_SWEEP_IC_T;

YH_SWEEP_IC_T gYH_Sweep_IC_test_result[4][CHANNEL_NUM][DQS_NUMBER];

#define YH_SWEEP_IC_PASS_CRITERIO 1 // 0: FT  1: SLT
void YH_Sweep_IC_Print_Result(DRAMC_CTX_T *p)
{
    unsigned char u1ChannelIdx, u1ByteIdx, k_type;
    unsigned char u1ByteIdxNum;

    //  SLT:
    //        CHB CLK duty max-min j5.3%: FAIL0
    //        NDQS duty max-min j5.8%: FAIL1
    //        NDQDQM maxduty j54.5% or min_duty<45.5% or max-min j5.8%: FAIL2

    show_msg2((INFO, "\n\n YH Sweep IC Print Result =========\n"));

    for(k_type=0; k_type<4; k_type++)
    {

        if (k_type == DutyScan_Calibration_K_CLK) u1ByteIdxNum = 1;
        else u1ByteIdxNum = 2;

        for(u1ChannelIdx=0; u1ChannelIdx<CHANNEL_NUM; u1ChannelIdx++)
                for(u1ByteIdx=0; u1ByteIdx<u1ByteIdxNum; u1ByteIdx++)
                {
                    if (k_type == DutyScan_Calibration_K_CLK && u1ChannelIdx == CHANNEL_B)
                    {
                        show_msg2((INFO, "CH%d CLK max-min Duty %d%% : ",u1ChannelIdx, gYH_Sweep_IC_test_result[k_type][u1ChannelIdx][u1ByteIdx].dutydiff));
#if YH_SWEEP_IC_PASS_CRITERIO
                        if (gYH_Sweep_IC_test_result[k_type][u1ChannelIdx][u1ByteIdx].dutydiff > 530)
#else
                        if (gYH_Sweep_IC_test_result[k_type][u1ChannelIdx][u1ByteIdx].dutydiff > 450)
#endif
                        {
                            show_msg2((INFO, "FAIL0\n"));
                        }
                        else
                        {
                            show_msg2((INFO, "PASS\n"));
                        }
                    }
                    if (k_type == DutyScan_Calibration_K_DQS)
                    {
                        show_msg2((INFO, "CH%d DQS Byte %d max-min Duty %d%% : ",u1ChannelIdx, u1ByteIdx, gYH_Sweep_IC_test_result[k_type][u1ChannelIdx][u1ByteIdx].dutydiff));
#if YH_SWEEP_IC_PASS_CRITERIO
                        if (gYH_Sweep_IC_test_result[k_type][u1ChannelIdx][u1ByteIdx].dutydiff > 580)
#else
                        if (gYH_Sweep_IC_test_result[k_type][u1ChannelIdx][u1ByteIdx].dutydiff > 500)
#endif
                        {
                            show_msg2((INFO, "FAIL1\n"));
                        }
                        else
                        {
                            show_msg2((INFO, "PASS\n"));
                        }
                    }
                    if (k_type == DutyScan_Calibration_K_DQ || k_type == DutyScan_Calibration_K_DQM)
                    {
                        show_msg2((INFO, "CH%d %s Byte %d max Duty %d%%, min Duty %d%% : ",u1ChannelIdx, k_type == DutyScan_Calibration_K_DQ ? "DQ" : "DQM", u1ByteIdx, gYH_Sweep_IC_test_result[k_type][u1ChannelIdx][u1ByteIdx].maxduty, gYH_Sweep_IC_test_result[k_type][u1ChannelIdx][u1ByteIdx].minduty));
#if YH_SWEEP_IC_PASS_CRITERIO
                        if  (gYH_Sweep_IC_test_result[k_type][u1ChannelIdx][u1ByteIdx].minduty < 4550 || gYH_Sweep_IC_test_result[k_type][u1ChannelIdx][u1ByteIdx].maxduty > 5450 || gYH_Sweep_IC_test_result[k_type][u1ChannelIdx][u1ByteIdx].dutydiff > 580)
#else
                        if  (gYH_Sweep_IC_test_result[k_type][u1ChannelIdx][u1ByteIdx].minduty < 4600 || gYH_Sweep_IC_test_result[k_type][u1ChannelIdx][u1ByteIdx].maxduty > 5400 || gYH_Sweep_IC_test_result[k_type][u1ChannelIdx][u1ByteIdx].dutydiff > 500)
#endif
                        {
                            show_msg2((INFO, "FAIL2\n"));
                        }
                        else
                        {
                            show_msg2((INFO, "PASS\n"));
                        }
                    }
                }
    }
}
#endif

signed char gcFinal_K_Duty_clk_delay_cell[DQS_NUMBER];
DRAM_STATUS_T DutyScan_Calibration_Flow(DRAMC_CTX_T *p, unsigned char k_type, unsigned char use_rev_bit)
{
    signed char scinner_duty_ofst, scFinal_clk_delay_cell[DQS_NUMBER]={0,0};
    signed char scinner_duty_ofst_start = 0, scinner_duty_ofst_end = 0;
    int scdqs_dly, s4PICnt, s4PIBegin, s4PIEnd, s4PICnt_mod64;
    signed char i, swap_idx, ucdqs_i, ucdqs_i_count=2;
    unsigned char u1ByteIdx;
    unsigned char ucDelay, ucDelayB;
    unsigned char ucRev_Bit0=0, ucRev_Bit1=0;
    unsigned int u4DutyDiff, u4DutyDiff_Limit=900;

    unsigned char vref_sel_value[2], cal_out_value;
    int duty_value[2];
    int final_duty;

    unsigned int ucperiod_duty_max=0, ucperiod_duty_min=0xffffffff, ucperiod_duty_max_clk_dly=0, ucperiod_duty_min_clk_dly=0;
    unsigned int ucperiod_duty_averige=0, ucFinal_period_duty_averige[DQS_NUMBER]={0,0}, ucmost_approach_50_percent=0xffffffff;
    unsigned int ucFinal_period_duty_max[DQS_NUMBER] = {0,0}, ucFinal_period_duty_min[DQS_NUMBER] = {0,0};
    unsigned int ucFinal_duty_max_clk_dly[DQS_NUMBER]={0},ucFinal_duty_min_clk_dly[DQS_NUMBER]={0};
    unsigned char early_break_count=0;
    unsigned char str_clk_duty[]="CLK", str_dqs_duty[]="DQS", str_dq_duty[]="DQ", str_dqm_duty[]="DQM";
    unsigned char *str_who_am_I=str_clk_duty;

    show_msg2((INFO, "\n[DutyScan_Calibration_Flow] %s Calibration\n", use_rev_bit==0 ? "First" : "Second"));
    show_msg2((INFO, "\n[DutyScan_Calibration_Flow] k_type=%d, use_rev_bit=%d\n", k_type, use_rev_bit));
    /*TINFO="\n[DutyScan_Calibration_Flow] k_type=%d\n", k_type */


    if (k_type == DutyScan_Calibration_K_CLK)
    {
#if 0
        // DQS duty test 3
        //show_msg2((INFO, "\n[*PHDET_EN*=0]\n"));
        show_msg2((INFO, "\n[*PI*RESETB*=0  *PHDET_EN*=0  *PI_RESETB*=1]\n"));
        /*TINFO="\n[*PI*RESETB*=0  *PHDET_EN*=0  *PI_RESETB*=1]\n" */
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_CA_DLL_ARPI0), 0x0, CA_DLL_ARPI0_RG_ARPI_RESETB_CA);
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_CA_DLL_ARPI2), 0x0, CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_CA_DLL_ARPI0), 0x1, CA_DLL_ARPI0_RG_ARPI_RESETB_CA);
#else
#if DutyPrintCalibrationLog
        show_msg((INFO, "\n[  *PHDET_EN*=0  \n"));
#endif
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_CA_DLL_ARPI2), 0x0, CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);
#endif
    }
    else
    {
        // DQS duty test 3
        //show_msg2((INFO, "\n[*PHDET_EN*=0]\n"));
#if DutyPrintCalibrationLog
        show_msg2((INFO, "[*PI*RESETB*=0  *PHDET_EN*=0  *PI_RESETB*=1]\n"));
#endif
        /*TINFO="[*PI*RESETB*=0  *PHDET_EN*=0  *PI_RESETB*=1]\n" */
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DLL_ARPI0), 0x0, B0_DLL_ARPI0_RG_ARPI_RESETB_B0);
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DLL_ARPI0), 0x0, B2_DLL_ARPI0_RG_ARPI_RESETB_B2);

        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DLL_ARPI2), 0x0, B0_DLL_ARPI2_RG_ARDLL_PHDET_EN_B0);
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DLL_ARPI2), 0x0, B2_DLL_ARPI2_RG_ARDLL_PHDET_EN_B2);

        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DLL_ARPI0), 0x1, B0_DLL_ARPI0_RG_ARPI_RESETB_B0);
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DLL_ARPI0), 0x1, B2_DLL_ARPI0_RG_ARPI_RESETB_B2);
    }

    //CLK Source Select (DQ/DQM/DQS/CLK)
    if (k_type == DutyScan_Calibration_K_DQ) // K DQ
    {
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ8), 0, B0_DQ8_RG_TX_ARDQ_CAP_DET_B0);
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DLL1), 0, RG_ARDQ_REV_BIT_06_MCK4X_SEL_DQ1);

        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ8), 0, B2_DQ8_RG_TX_ARDQ_CAP_DET_B2);
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DLL1), 0, RG_ARDQ_REV_BIT_06_MCK4X_SEL_DQ1);

        ucdqs_i_count = 2;
        str_who_am_I = (unsigned char*)str_dq_duty;

        scinner_duty_ofst_start = 0;
        scinner_duty_ofst_end = 0;
    }
    else if (k_type == DutyScan_Calibration_K_DQM) // K DQM
    {
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ8), 0, B0_DQ8_RG_TX_ARDQ_CAP_DET_B0);
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DLL1), 1, RG_ARDQ_REV_BIT_06_MCK4X_SEL_DQ1);

        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ8), 0, B2_DQ8_RG_TX_ARDQ_CAP_DET_B2);
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DLL1), 1, RG_ARDQ_REV_BIT_06_MCK4X_SEL_DQ1);

        ucdqs_i_count = 2;
        str_who_am_I = (unsigned char*)str_dqm_duty;

        scinner_duty_ofst_start = 0;
        scinner_duty_ofst_end = 0;
    }
    else if (k_type == DutyScan_Calibration_K_DQS) // K DQS
    {
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ8), 1, B0_DQ8_RG_TX_ARDQ_CAP_DET_B0);

        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ8), 1, B2_DQ8_RG_TX_ARDQ_CAP_DET_B2);

        ucdqs_i_count = 2;
        str_who_am_I = (unsigned char*)str_dqs_duty;

        scinner_duty_ofst_start = DUTY_OFFSET_START;
        scinner_duty_ofst_end = DUTY_OFFSET_END;

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
        p->pSavetimeData->u1dqs_use_rev_bit = use_rev_bit;
#endif
    }
    else if (k_type == DutyScan_Calibration_K_CLK) // K CLK
    {
        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_CA_CMD9), 1, CA_CMD9_RG_TX_ARCMD_CAP_DET);

        ucdqs_i_count = 1;
        str_who_am_I = (unsigned char*)str_clk_duty;

        scinner_duty_ofst_start = DUTY_OFFSET_START;
        scinner_duty_ofst_end = DUTY_OFFSET_END;

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
        p->pSavetimeData->u1clk_use_rev_bit = use_rev_bit;
#endif
    }

#if defined(YH_SWEEP_IC) || FT_DSIM_USED
        scinner_duty_ofst_start=0;
        scinner_duty_ofst_end=0;
#endif

    if (k_type == DutyScan_Calibration_K_CLK)
    {
        u4DutyDiff_Limit = 530;
    }
    else
    {
        u4DutyDiff_Limit = 924;
    }


    if (k_type == DutyScan_Calibration_K_CLK)
    {
        s4PIBegin = 0;
        s4PIEnd = 0;
    }
    else
    {
        s4PIBegin = CLOCK_PI_START;
        s4PIEnd = CLOCK_PI_END;
    }

    for(ucdqs_i=0; ucdqs_i<ucdqs_i_count; ucdqs_i++)
    {
#if DutyPrintCalibrationLog
        if (k_type == DutyScan_Calibration_K_CLK)
        {
            show_msg2((INFO, "\n[CLK Duty scan]\n"));
            /*TINFO="\n[CLK Duty scan]\n", ucdqs_i */
        }
        else
        {
            show_msg2((INFO, "\n[%s B%d Duty scan]\n", str_who_am_I, ucdqs_i));
            /*TINFO="\n[%s B%d Duty scan]\n", str_who_am_I, ucdqs_i */
        }
#endif

        ucmost_approach_50_percent=0xffffffff;
        early_break_count=0;

        for(scinner_duty_ofst=scinner_duty_ofst_start; scinner_duty_ofst<=scinner_duty_ofst_end; scinner_duty_ofst++)
        {
            ucperiod_duty_max = 0;
            ucperiod_duty_min = 100000;

            if (scinner_duty_ofst<0)
            {
                ucDelay = -scinner_duty_ofst;
                ucDelayB = 0;

                if (use_rev_bit)
                {
                    ucRev_Bit0 = 1;
                    ucRev_Bit1 = 0;
                }
            }
            else if (scinner_duty_ofst>0)
            {
                ucDelay = 0;
                ucDelayB= scinner_duty_ofst;

                if (use_rev_bit)
                {
                    ucRev_Bit0 = 0;
                    ucRev_Bit1 = 1;
                }
            }
            else
            {
                ucDelay = 0;
                ucDelayB= 0;

                if (use_rev_bit)
                {
                    ucRev_Bit0 = 0;
                    ucRev_Bit1 = 0;
                }
            }

            if (k_type == DutyScan_Calibration_K_DQS)
            {
                if (ucdqs_i==0)
                {
                    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ1), p_fld(ucDelay, SHU_R0_B0_DQ1_RK0_TX_ARDQS0_DLY_B0)
                                                                                | p_fld(ucDelay, SHU_R0_B0_DQ1_RK0_TX_ARDQS0B_DLY_B0)
                                                                                | p_fld(ucDelayB, SHU_R0_B0_DQ1_RK0_TX_ARDQS0_DLYB_B0)
                                                                                | p_fld(ucDelayB, SHU_R0_B0_DQ1_RK0_TX_ARDQS0B_DLYB_B0));

                    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DLL1), p_fld(ucRev_Bit0, RG_ARDQ_REV_BIT_00_DQS_MCK4X_DLY_EN)
                                                                            | p_fld(ucRev_Bit1, RG_ARDQ_REV_BIT_01_DQS_MCK4XB_DLY_EN));

                }
                else
                {
                    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ1), p_fld(ucDelay, SHU_R0_B2_DQ1_RK0_TX_ARDQS0_DLY_B2)
                                                                                | p_fld(ucDelay, SHU_R0_B2_DQ1_RK0_TX_ARDQS0B_DLY_B2)
                                                                                | p_fld(ucDelayB, SHU_R0_B2_DQ1_RK0_TX_ARDQS0_DLYB_B2)
                                                                                | p_fld(ucDelayB, SHU_R0_B2_DQ1_RK0_TX_ARDQS0B_DLYB_B2));

                    io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DLL1), p_fld(ucRev_Bit0, RG_ARDQ_REV_BIT_00_DQS_MCK4X_DLY_EN)
                                                                            | p_fld(ucRev_Bit1, RG_ARDQ_REV_BIT_01_DQS_MCK4XB_DLY_EN));
                }

            }

            if (k_type == DutyScan_Calibration_K_CLK)
            {
                io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD1), p_fld(ucDelay, SHU_R0_CA_CMD1_RK0_TX_ARCLK_DLY)
                                                                            | p_fld(ucDelay, SHU_R0_CA_CMD1_RK0_TX_ARCLKB_DLY));
                io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD0), p_fld(ucDelayB, SHU_R0_CA_CMD0_RK0_TX_ARCLK_DLYB)
                                                                            | p_fld(ucDelayB, SHU_R0_CA_CMD0_RK0_TX_ARCLKB_DLYB));

                io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD3), p_fld(ucRev_Bit0, SHU_CA_CMD3_RG_TX_ARCMD_PU_BIT0)
                                                                        | p_fld(ucRev_Bit1, SHU_CA_CMD3_RG_TX_ARCMD_PU_BIT1));
            }

            for(s4PICnt=s4PIBegin; s4PICnt<=s4PIEnd; s4PICnt+=CLOCK_PI_STEP)
            {
                s4PICnt_mod64 = (s4PICnt+64)&0x3f;//s4PICnt_mod64 = (s4PICnt+64)%64;
#if DutyPrintAllLog
                //if(scinner_duty_ofst!=DUTY_OFFSET_START)
                    show_msg2((INFO, "PI= %d\n", s4PICnt_mod64));
#endif

                if (k_type == DutyScan_Calibration_K_DQS)
                {
                    if (ucdqs_i==0)
                    {
                        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ7), s4PICnt_mod64, SHU_R0_B0_DQ7_RK0_ARPI_PBYTE_B0);
                    }
                    else
                    {
                        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ7), s4PICnt_mod64, SHU_R0_B2_DQ7_RK0_ARPI_PBYTE_B2);
                    }
                }
                else
                if (k_type == DutyScan_Calibration_K_CLK)
                {
                    io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD9), s4PICnt_mod64, SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK);
                }
                else
                if (k_type == DutyScan_Calibration_K_DQ)
                {
                    if (ucdqs_i==0)
                    {
                        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ7), s4PICnt_mod64, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0);
                    }
                    else
                    {
                        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ7), s4PICnt_mod64, SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2);
                    }
                }
                else
                if (k_type == DutyScan_Calibration_K_DQM)
                {
                    if (ucdqs_i==0)
                    {
                        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B0_DQ7), s4PICnt_mod64, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0);
                    }
                    else
                    {
                        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B2_DQ7), s4PICnt_mod64, SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2);
                    }
                }

                for(swap_idx=0; swap_idx<2; swap_idx++)
                {
                    if (k_type == DutyScan_Calibration_K_CLK)
                    {
                        if (swap_idx==0)
                        {
                            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_CA_DLL1), 2, RG_ARCMD_REV_BIT_2221_DATA_SWAP);
                        }
                        else
                        {
                            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_CA_DLL1), 3, RG_ARCMD_REV_BIT_2221_DATA_SWAP);
                        }

                        vref_sel_value[swap_idx]= 0;
                        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD5), vref_sel_value[swap_idx]>>1, SHU_CA_CMD5_RG_RX_ARCMD_VREF_SEL);
                        io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD0), (vref_sel_value[swap_idx]&1)==1?0:1, SHU_CA_CMD0_RG_TX_ARCLK_DRVP_PRE_BIT1);
                    }
                    else
                    {
                        if (ucdqs_i==0)
                        {
                            if (swap_idx==0)
                            {
                                io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DLL1), 2, RG_ARDQ_REV_BIT_2221_DATA_SWAP);
                            }
                            else
                            {
                                io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DLL1), 3, RG_ARDQ_REV_BIT_2221_DATA_SWAP);
                            }

                            vref_sel_value[swap_idx]= 0;
                            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ5), vref_sel_value[swap_idx]>>1, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
                            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ0), (vref_sel_value[swap_idx]&1)==1?0:1, SHU_B0_DQ0_RG_TX_ARDQS0_DRVP_PRE_B0_BIT1);
                        }
                        else
                        {
                            if (swap_idx==0)
                            {
                                io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DLL1), 2, RG_ARDQ_REV_BIT_2221_DATA_SWAP);
                            }
                            else
                            {
                                io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DLL1), 3, RG_ARDQ_REV_BIT_2221_DATA_SWAP);
                            }

                            vref_sel_value[swap_idx]= 0;
                            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ5), vref_sel_value[swap_idx]>>1, SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2);
                            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ0), (vref_sel_value[swap_idx]&1)==1?0:1, SHU_B2_DQ0_RG_TX_ARDQS0_DRVP_PRE_B2_BIT1);
                        }
                    }

                    for(i=5; i>=0; i--)
                    {
                        if (k_type == DutyScan_Calibration_K_CLK)
                        {
                            vref_sel_value[swap_idx] |= (1<<i);
                            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD5), vref_sel_value[swap_idx]>>1, SHU_CA_CMD5_RG_RX_ARCMD_VREF_SEL);
                            io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD0), (vref_sel_value[swap_idx]&1)==1?0:1, SHU_CA_CMD0_RG_TX_ARCLK_DRVP_PRE_BIT1);

                            delay_us(1);

                            cal_out_value = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_AD_RX_CMD_O1), MISC_AD_RX_CMD_O1_AD_RX_ARCA2_O1);

                            if (cal_out_value == 0)
                            {
                                vref_sel_value[swap_idx] &= ~(1<<i);
                            }
                        }
                        else
                        {
                            if (ucdqs_i==0)
                            {
                                vref_sel_value[swap_idx] |= (1<<i);
                                io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ5), vref_sel_value[swap_idx]>>1, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
                                io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ0), (vref_sel_value[swap_idx]&1)==1?0:1, SHU_B0_DQ0_RG_TX_ARDQS0_DRVP_PRE_B0_BIT1);

                                delay_us(1);

                                cal_out_value = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_AD_RX_DQ_O1), MISC_AD_RX_DQ_O1_AD_RX_ARDQ_O1_B0_BIT2);
                            }
                            else
                            {
                                vref_sel_value[swap_idx] |= (1<<i);
                                io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ5), vref_sel_value[swap_idx]>>1, SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2);
                                io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ0), (vref_sel_value[swap_idx]&1)==1?0:1, SHU_B0_DQ0_RG_TX_ARDQS0_DRVP_PRE_B0_BIT1);

                                delay_us(1);

                                cal_out_value = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_AD_RX_DQ_O1_2), MISC_AD_RX_DQ_O1_AD_RX_ARDQ_O1_B0_BIT2);
                            }

#if DutyPrintAllLog
                            //show_msg2((INFO, "Fra i=%d vref_sel_value[swap_idx]=%x, cal_out=%d\n",i, vref_sel_value[swap_idx], cal_out_value));
#endif

                            if (cal_out_value == 0)
                            {
                                vref_sel_value[swap_idx] &= ~(1<<i);
                            }
                        }
                    }
                }


                for(swap_idx=0; swap_idx<2; swap_idx++)
                {
                    if (vref_sel_value[swap_idx]<=31)
                    {
                        duty_value[swap_idx] = (vref_sel_value[swap_idx]-23)*69+5050;
                    }
                    else
                    {
                        duty_value[swap_idx] = (vref_sel_value[swap_idx]-32)*55+5600;
                    }
                }

#if DutyPrintAllLog
                show_msg2((INFO, "\t[%d][%d] B%d : Vref_Sel=0x%x, Swap Vref_Sel=0x%x\n", scinner_duty_ofst, s4PICnt, ucdqs_i, vref_sel_value[0], vref_sel_value[1]));
                show_msg2((INFO, "\t[%d][%d] B%d : duty_value=%d, Swap duty_value=%d\n", scinner_duty_ofst, s4PICnt, ucdqs_i, duty_value[0], duty_value[1]));
#endif

                final_duty = 5000+((duty_value[0]-duty_value[1])/2);

                if (final_duty > (int) ucperiod_duty_max)
                {
                    ucperiod_duty_max = final_duty;
                    ucperiod_duty_max_clk_dly = s4PICnt;
                }
                if (final_duty < (int) ucperiod_duty_min)
                {
                    ucperiod_duty_min = final_duty;
                    ucperiod_duty_min_clk_dly = s4PICnt;
                }

#if DutyPrintAllLog
                show_msg2((INFO, "\t[%d][%d] B%d : Final_Duty=%d\n", scinner_duty_ofst, s4PICnt, ucdqs_i, final_duty));
#endif
            }



            ucperiod_duty_averige = (ucperiod_duty_max + ucperiod_duty_min)>>1;

#if DutyPrintCalibrationLog
            if (k_type==DutyScan_Calibration_K_CLK)
            {
                show_msg2((INFO, "[%d] CLK\n",scinner_duty_ofst));
                /*TINFO="[%d] CLK\n",scinner_duty_ofst */
            }
            else
            {
                show_msg2((INFO, "[%d] %s%d\n",scinner_duty_ofst, str_who_am_I, ucdqs_i));
                /*TINFO="[%d] %s%d\n",scinner_duty_ofst, str_who_am_I, ucdqs_i */
            }
#endif

#if DutyPrintCalibrationLog
            show_msg2((INFO, "\tMAX Duty = %d%%(X100), CLK PI=%d\n",ucperiod_duty_max, ucperiod_duty_max_clk_dly));
            /*TINFO="\tMAX Duty = %d%%(X100), CLK PI=%d\n",ucperiod_duty_max, ucperiod_duty_max_clk_dly */
            show_msg2((INFO, "\tMIN Duty = %d%%(X100), CLK PI=%d\n",ucperiod_duty_min, ucperiod_duty_min_clk_dly));
            /*TINFO="\tMIN Duty = %d%%(X100), CLK PI=%d\n",ucperiod_duty_min, ucperiod_duty_min_clk_dly */
            show_msg2((INFO, "\tAVG Duty = %d%%(X100)\n", ucperiod_duty_averige));
            /*TINFO="\tAVG Duty = %d%%(X100)\n", ucperiod_duty_averige */
#endif

            if (ucperiod_duty_averige >= ClockDutyMiddleBound)
            {
                if ((scinner_duty_ofst<=0 && ((ucperiod_duty_averige-ClockDutyMiddleBound+(ucperiod_duty_max-ucperiod_duty_min)/2) <= ucmost_approach_50_percent)) ||
                     (scinner_duty_ofst>0 && ((ucperiod_duty_averige-ClockDutyMiddleBound+(ucperiod_duty_max-ucperiod_duty_min)/2) < ucmost_approach_50_percent)))
                {
                    ucmost_approach_50_percent = ucperiod_duty_averige-ClockDutyMiddleBound+(ucperiod_duty_max-ucperiod_duty_min)/2;
                    scFinal_clk_delay_cell[ucdqs_i] = scinner_duty_ofst;
                    ucFinal_period_duty_averige[ucdqs_i] = ucperiod_duty_averige;
                    ucFinal_period_duty_max[ucdqs_i] = ucperiod_duty_max;
                    ucFinal_period_duty_min[ucdqs_i] = ucperiod_duty_min;
                    ucFinal_duty_max_clk_dly[ucdqs_i] = ucperiod_duty_max_clk_dly;
                    ucFinal_duty_min_clk_dly[ucdqs_i] = ucperiod_duty_min_clk_dly;
#if DutyPrintCalibrationLog
                    show_msg2((INFO, "\t!!! ucmost_approach_50_percent = %d%%(X100) !!!\n",ucmost_approach_50_percent));
                    /*TINFO="!!! ucmost_approach_50_percent = %d%%(X100) !!!\n",ucmost_approach_50_percent */
#endif
                    early_break_count = 0;
                }
                else
                {
                    if (scinner_duty_ofst>0) early_break_count ++;
#if DutyPrintAllLog==0
                    if (early_break_count>=2) break; //early break;
#endif
                }
            }
            else
            {
                if ((scinner_duty_ofst<=0 && ((ClockDutyMiddleBound-ucperiod_duty_averige+(ucperiod_duty_max-ucperiod_duty_min)/2) <= ucmost_approach_50_percent)) ||
                    (scinner_duty_ofst>0 && ((ClockDutyMiddleBound-ucperiod_duty_averige+(ucperiod_duty_max-ucperiod_duty_min)/2) < ucmost_approach_50_percent)))
                {
                    ucmost_approach_50_percent = ClockDutyMiddleBound-ucperiod_duty_averige+(ucperiod_duty_max-ucperiod_duty_min)/2;
                    scFinal_clk_delay_cell[ucdqs_i] = scinner_duty_ofst;
                    ucFinal_period_duty_averige[ucdqs_i] = ucperiod_duty_averige;
                    ucFinal_period_duty_max[ucdqs_i] = ucperiod_duty_max;
                    ucFinal_period_duty_min[ucdqs_i] = ucperiod_duty_min;
                    ucFinal_duty_max_clk_dly[ucdqs_i] = ucperiod_duty_max_clk_dly;
                    ucFinal_duty_min_clk_dly[ucdqs_i] = ucperiod_duty_min_clk_dly;
#if DutyPrintCalibrationLog
                    show_msg2((INFO, "\t!!! ucmost_approach_50_percent = %d%%(X100) !!!\n",ucmost_approach_50_percent));
                    /*TINFO="!!! ucmost_approach_50_percent = %d%%(X100) !!!\n",ucmost_approach_50_percent */
#endif
                    early_break_count = 0;
                }
                else
                {
                    if (scinner_duty_ofst>0) early_break_count ++;
#if DutyPrintAllLog==0
                    if (early_break_count>=2) break; //early break;
#endif
                }
            }

#if DutyPrintCalibrationLog
            show_msg2((INFO, "\n"));
            /*TINFO="\n" */
#endif
        }
    }

    for(ucdqs_i=0; ucdqs_i<ucdqs_i_count; ucdqs_i++)
    {
        //for SLT, use ERR_MSG to force print log
        if (k_type == DutyScan_Calibration_K_CLK)
        {
            show_diag((INFO, "\n==%s ==\n", str_who_am_I, ucdqs_i));
            /*TINFO="\n==%s ==\n", str_who_am_I */
        }
        else
        {
            show_diag((INFO, "\n==%s %d ==\n", str_who_am_I, ucdqs_i));
            /*TINFO="\n==%s %d ==\n", str_who_am_I, ucdqs_i */
        }
        show_diag((INFO, "Final %s duty delay cell = %d\n", str_who_am_I, scFinal_clk_delay_cell[ucdqs_i]));
        /*TINFO="Final %s duty delay cell = %d\n", str_who_am_I, scFinal_clk_delay_cell[ucdqs_i] */
        show_diag((INFO, "[%d] MAX Duty = %d%%(X100), DQS PI = %d\n",scFinal_clk_delay_cell[ucdqs_i], ucFinal_period_duty_max[ucdqs_i], ucFinal_duty_max_clk_dly[ucdqs_i]));
        /*TINFO="[%d] MAX Duty = %d%%(X100), DQS PI = %d\n",scFinal_clk_delay_cell[ucdqs_i], ucFinal_period_duty_max[ucdqs_i], ucFinal_duty_max_clk_dly[ucdqs_i] */
        show_diag((INFO, "[%d] MIN Duty = %d%%(X100), DQS PI = %d\n",scFinal_clk_delay_cell[ucdqs_i], ucFinal_period_duty_min[ucdqs_i], ucFinal_duty_min_clk_dly[ucdqs_i]));
        /*TINFO="[%d] MIN Duty = %d%%(X100), DQS PI = %d\n",scFinal_clk_delay_cell[ucdqs_i], ucFinal_period_duty_min[ucdqs_i], ucFinal_duty_min_clk_dly[ucdqs_i] */
        show_diag((INFO, "[%d] AVG Duty = %d%%(X100)\n", scFinal_clk_delay_cell[ucdqs_i], ucFinal_period_duty_averige[ucdqs_i]));
        /*TINFO="[%d] AVG Duty = %d%%(X100)\n", scFinal_clk_delay_cell[ucdqs_i], ucFinal_period_duty_averige[ucdqs_i] */
    }

#if FT_DSIM_USED
    FT_Duty_Compare_PassFail(p->channel, k_type, ucFinal_period_duty_max[0] , ucFinal_period_duty_min[0],ucFinal_period_duty_max[1] , ucFinal_period_duty_min[1]);
#else
    for(ucdqs_i=0; ucdqs_i<ucdqs_i_count; ucdqs_i++)
    {
        u4DutyDiff = ucFinal_period_duty_max[ucdqs_i] - ucFinal_period_duty_min[ucdqs_i];

#if DQS_DUTY_SLT_CONDITION_TEST
        if (k_type == DutyScan_Calibration_K_CLK || (k_type == DutyScan_Calibration_K_DQS))
        {
            u4DQSDutyDiff_Rec[p->channel][ucdqs_i][u1GlobalTestCnt]=u4DutyDiff;

            u4DQSDutyDutyDly[p->channel][ucdqs_i] = scFinal_clk_delay_cell[ucdqs_i];

            if(u4DutyDiff > u4DQSDutyDiff_Max[p->channel][ucdqs_i])
                u4DQSDutyDiff_Max[p->channel][ucdqs_i] = u4DutyDiff;

            if(u4DutyDiff < u4DQSDutyDiff_Min[p->channel][ucdqs_i])
                u4DQSDutyDiff_Min[p->channel][ucdqs_i] = u4DutyDiff;

            u4DQSDutyDiff_Avrg[p->channel][ucdqs_i]  += u4DutyDiff;
        }
#endif

#if defined(YH_SWEEP_IC)
        gYH_Sweep_IC_test_result[k_type][p->channel][ucdqs_i].maxduty = ucFinal_period_duty_max[ucdqs_i];
        gYH_Sweep_IC_test_result[k_type][p->channel][ucdqs_i].minduty = ucFinal_period_duty_min[ucdqs_i];
        gYH_Sweep_IC_test_result[k_type][p->channel][ucdqs_i].dutydiff = u4DutyDiff;
        gYH_Sweep_IC_test_result[k_type][p->channel][ucdqs_i].avgduty = ucFinal_period_duty_averige[ucdqs_i];
#else
        if ((((k_type == DutyScan_Calibration_K_CLK) || (k_type == DutyScan_Calibration_K_DQS)) && (u4DutyDiff < u4DutyDiff_Limit)) ||
           (((k_type == DutyScan_Calibration_K_DQ) || (k_type == DutyScan_Calibration_K_DQM)) && ((u4DutyDiff < u4DutyDiff_Limit) && (ucFinal_period_duty_averige[ucdqs_i] >= 4550 && ucFinal_period_duty_averige[ucdqs_i] <= 5450))))
        {
            if (k_type == DutyScan_Calibration_K_CLK)
            {
                show_diag((INFO, "\nCH%d %s Duty spec in!! Max-Min= %d%%\n",p->channel, str_who_am_I, u4DutyDiff));
                /*TINFO="\nCH%d %s Duty spec in!! Max-Min= %d%%\n",p->channel, str_who_am_I, u4DutyDiff */
            }
            else
            {
                show_diag((INFO, "\nCH%d %s %d Duty spec in!! Max-Min= %d%%\n",p->channel, str_who_am_I, ucdqs_i, u4DutyDiff));
                /*TINFO="\nCH%d %s %d Duty spec in!! Max-Min= %d%%\n",p->channel, str_who_am_I, ucdqs_i, u4DutyDiff */
            }
        }
        else
        {
            if (k_type == DutyScan_Calibration_K_CLK)
            {
                show_diag((INFO, "\nCH%d %s Duty spec out!! Max-Min= %d%% >%d%%\n", p->channel, str_who_am_I, u4DutyDiff, u4DutyDiff_Limit));
                /*TINFO="\nCH%d %s Duty spec out!! Max-Min= %d%% >8%%\n", p->channel, str_who_am_I, u4DutyDiff */
            }
            else
            {
                show_diag((INFO, "\nCH%d %s %d Duty spec out!! Max-Min= %d%% >%d%%\n", p->channel, str_who_am_I, ucdqs_i, u4DutyDiff, u4DutyDiff_Limit));
                /*TINFO="\nCH%d %s %d Duty spec out!! Max-Min= %d%% >8%%\n", p->channel, str_who_am_I, ucdqs_i, u4DutyDiff */
            }

            #if defined(DRAM_SLT)
                    while(1); //stop here
            #endif

            #if __ETT__

                #if DQS_DUTY_SLT_CONDITION_TEST
                retStatus = DRAM_FAIL;
                #else
                    while(1); //stop here
                #endif

            #endif
        }
#endif
    }

#endif

    if (k_type == DutyScan_Calibration_K_DQS)
    {
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
        if(p->femmc_Ready==0)
        {
            p->pSavetimeData->s1DQSDuty_clk_delay_cell[p->channel][0] = scFinal_clk_delay_cell[0];
            p->pSavetimeData->s1DQSDuty_clk_delay_cell[p->channel][1] = scFinal_clk_delay_cell[1];
        }
#endif

        // backup K DQS final values
        gcFinal_K_Duty_clk_delay_cell[0] = scFinal_clk_delay_cell[0];
        gcFinal_K_Duty_clk_delay_cell[1] = scFinal_clk_delay_cell[1];

        DQSDutyScan_SetDqsDelayCell(p, scFinal_clk_delay_cell, use_rev_bit);

		//according to JC's suggestion @ 2019/07/25
		if (use_rev_bit==1)
		{
			if (scFinal_clk_delay_cell[0] == 8 || scFinal_clk_delay_cell[0] == -8 || scFinal_clk_delay_cell[1] == 8 || scFinal_clk_delay_cell[1] == -8)
			{
				mcSHOW_ERR_MSG(("Warning !! DQS Duty is 8 or -8 !!\n"));
			}
		}
#ifdef FOR_HQA_TEST_USED
        gFinalDQSDuty[p->channel][0] = scFinal_clk_delay_cell[0];
        gFinalDQSDuty[p->channel][1] = scFinal_clk_delay_cell[1];
        gFinalDQSDutyMinMax[p->channel][0][0] = ucFinal_period_duty_min[0];
        gFinalDQSDutyMinMax[p->channel][0][1] = ucFinal_period_duty_max[0];
        gFinalDQSDutyMinMax[p->channel][1][0] = ucFinal_period_duty_min[1];
        gFinalDQSDutyMinMax[p->channel][1][1] = ucFinal_period_duty_max[1];
#endif
    }

    if (k_type == DutyScan_Calibration_K_CLK)
    {
        DramcClockDutySetClkDelayCell(p, RANK_0, scFinal_clk_delay_cell[0], use_rev_bit);
        DramcClockDutySetClkDelayCell(p, RANK_1, scFinal_clk_delay_cell[0], use_rev_bit);

        // backup K CLK final values
        gcFinal_K_Duty_clk_delay_cell[0] = scFinal_clk_delay_cell[0];

		//according to JC's suggestion @ 2019/07/25
		if (use_rev_bit==1)
		{
			if (scFinal_clk_delay_cell[0] == 8 || scFinal_clk_delay_cell[0] == -8)
			{
				mcSHOW_ERR_MSG(("Warning !! CLK Duty is 8 or -8 !!\n"));
			}
		}
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
        if(p->femmc_Ready==0)
        {
            p->pSavetimeData->s1ClockDuty_clk_delay_cell[p->channel][RANK_0] = scFinal_clk_delay_cell[0];
            p->pSavetimeData->s1ClockDuty_clk_delay_cell[p->channel][RANK_1] = scFinal_clk_delay_cell[0];
        }
#endif

#ifdef FOR_HQA_TEST_USED
        gFinalClkDuty[p->channel] = scFinal_clk_delay_cell[0];
        gFinalClkDutyMinMax[p->channel][0] = ucFinal_period_duty_min[0];
        gFinalClkDutyMinMax[p->channel][1] = ucFinal_period_duty_max[0];
#endif
    }

    dram_phy_reset(p);

    show_msg((INFO, "[DutyScan_Calibration_Flow] ====Done====\n"));
    /*TINFO="[DutyScan_Calibration_Flow] ====Done====\n" */

    return DRAM_OK;
}

void DramcNewDutyCalibration(DRAMC_CTX_T *p)
{
    unsigned char u1ChannelIdx, u1backup_channel;

#if(DQS_DUTY_SLT_CONDITION_TEST)
        unsigned short u2TestCnt, u2FailCnt=0, u2TestCntTotal =20; //fra 400;
        unsigned char u1ByteIdx, u1PI_FB;
        unsigned int u4Variance;
#endif
    unsigned char use_rev_bit=0;

    DRAM_STATUS_T u2FailStatusByCh[CHANNEL_NUM]={DRAM_OK,DRAM_OK};

    //backup register value
#if FT_DSIM_USED==0
    unsigned int u4RegBackupAddress[] =
    {
        (DDRPHY_B0_DQ6),
        (DDRPHY_SHU_B0_DLL1),
        (DDRPHY_SHU_B0_DQ5),
        (DDRPHY_SHU_B0_DQ0),
        (DDRPHY_B0_DQ0),
        (DDRPHY_B0_DQ5),
        (DDRPHY_B0_DQ8),
        (DDRPHY_SHU_R0_B0_DQ7),
        (DDRPHY_B0_DLL_ARPI0),
        (DDRPHY_B0_DLL_ARPI2),

        (DDRPHY_B2_DQ6),
        (DDRPHY_SHU_B2_DLL1),
        (DDRPHY_SHU_B2_DQ5),
        (DDRPHY_SHU_B2_DQ0),
        (DDRPHY_B2_DQ0),
        (DDRPHY_B2_DQ5),
        (DDRPHY_B2_DQ8),
        (DDRPHY_SHU_R0_B2_DQ7),
        (DDRPHY_B2_DLL_ARPI0),
        (DDRPHY_B2_DLL_ARPI2),


        (DDRPHY_CA_CMD6),
        (DDRPHY_SHU_CA_DLL1),
        (DDRPHY_SHU_CA_CMD5),
        (DDRPHY_SHU_CA_CMD0),
        (DDRPHY_CA_CMD0),
        (DDRPHY_CA_CMD5),
        (DDRPHY_CA_CMD9),
//        (DDRPHY_SHU_CA_CMD3),
        (DDRPHY_SHU_R0_CA_CMD9),
        (DDRPHY_CA_DLL_ARPI0),
        (DDRPHY_CA_DLL_ARPI2),



        (DDRPHY_B0_DQ6 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_SHU_B0_DLL1 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_SHU_B0_DQ5 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_SHU_B0_DQ0 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_B0_DQ0 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_B0_DQ5 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_B0_DQ8 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_SHU_R0_B0_DQ7 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_B0_DLL_ARPI0 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_B0_DLL_ARPI2 + SHIFT_TO_CHB_ADDR),

        (DDRPHY_B2_DQ6 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_SHU_B2_DLL1 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_SHU_B2_DQ5 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_SHU_B2_DQ0 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_B2_DQ0 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_B2_DQ5 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_B2_DQ8 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_SHU_R0_B2_DQ7 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_B2_DLL_ARPI0 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_B2_DLL_ARPI2 + SHIFT_TO_CHB_ADDR),


        (DDRPHY_CA_CMD6 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_SHU_CA_DLL1 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_SHU_CA_CMD5 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_SHU_CA_CMD0 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_CA_CMD0 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_CA_CMD5 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_CA_CMD9 + SHIFT_TO_CHB_ADDR),
//        (DDRPHY_SHU_CA_CMD3 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_SHU_R0_CA_CMD9 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_CA_DLL_ARPI0 + SHIFT_TO_CHB_ADDR),
        (DDRPHY_CA_DLL_ARPI2 + SHIFT_TO_CHB_ADDR)
    };
#endif

#if 0
#if DUTY_SCAN_V2_ONLY_K_HIGHEST_FREQ
    if((p->frequency == dfs_get_highest_freq(p)) && (get_pre_miock_jmeter_hqa_used_flag()==0))
#else
    if(get_pre_miock_jmeter_hqa_used_flag()==0)
#endif
#endif

    {
        if(is_lp4_family(p))
        {
            unsigned char u1ChannelIdx;
            u1backup_channel = get_phy_2_channel_mapping(p);

            #if SUPPORT_SAVE_TIME_FOR_CALIBRATION
            if(p->femmc_Ready==1)
            {
                for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<p->support_channel_num; u1ChannelIdx++)
                {
                    set_phy_2_channel_mapping(p, u1ChannelIdx);
                    DramcClockDutySetClkDelayCell(p, RANK_0, p->pSavetimeData->s1ClockDuty_clk_delay_cell[p->channel][RANK_0], p->pSavetimeData->u1clk_use_rev_bit);
                    DramcClockDutySetClkDelayCell(p, RANK_1, p->pSavetimeData->s1ClockDuty_clk_delay_cell[p->channel][RANK_1], p->pSavetimeData->u1clk_use_rev_bit);
                    DQSDutyScan_SetDqsDelayCell(p, p->pSavetimeData->s1DQSDuty_clk_delay_cell[p->channel], p->pSavetimeData->u1dqs_use_rev_bit);
					show_msg((INFO, "DUTY%d CLK/DQS0/DQS1: %d %d %d %d\n", p->channel, p->pSavetimeData->s1ClockDuty_clk_delay_cell[p->channel][RANK_0], p->pSavetimeData->s1ClockDuty_clk_delay_cell[p->channel][RANK_1],
							p->pSavetimeData->s1DQSDuty_clk_delay_cell[p->channel][0], p->pSavetimeData->s1DQSDuty_clk_delay_cell[p->channel][1]));
                }
            }
            else
            #endif
            {
                //Clk free run
                enable_dramc_phy_dcm(p, 0);

                for(u1ChannelIdx=CHANNEL_A; u1ChannelIdx<p->support_channel_num; u1ChannelIdx++)
                {
                    set_phy_2_channel_mapping(p, u1ChannelIdx);

                //Fix rank to rank0
                io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG), 0, RKCFG_TXRANK);
                io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG), 1, RKCFG_TXRANKFIX);

                    //backup register value
                    #if FT_DSIM_USED==0
                    dramc_backup_registers(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(unsigned int));
                    #endif

                    DutyScan_Offset_Calibration(p);

                    #if defined(YH_SWEEP_IC)
                        if (p->channel == CHANNEL_B)
                        {
                            u2FailStatusByCh[u1ChannelIdx]= DutyScan_Calibration_Flow(p, DutyScan_Calibration_K_CLK, 0);
                            if (gcFinal_K_Duty_clk_delay_cell[0] == 8 || gcFinal_K_Duty_clk_delay_cell[0] == -8)
                            {
                                u2FailStatusByCh[u1ChannelIdx]= DutyScan_Calibration_Flow(p, DutyScan_Calibration_K_CLK, 1);
                            }
                        }
                    #else
                        u2FailStatusByCh[u1ChannelIdx]= DutyScan_Calibration_Flow(p, DutyScan_Calibration_K_CLK, 0);
                        if (gcFinal_K_Duty_clk_delay_cell[0] == 8 || gcFinal_K_Duty_clk_delay_cell[0] == -8)
                        {
                            u2FailStatusByCh[u1ChannelIdx]= DutyScan_Calibration_Flow(p, DutyScan_Calibration_K_CLK, 1);
                        }
                    #endif

                        u2FailStatusByCh[u1ChannelIdx]= DutyScan_Calibration_Flow(p, DutyScan_Calibration_K_DQS, 0);
                        use_rev_bit=0;
                    if (gcFinal_K_Duty_clk_delay_cell[0] == 8 ||
						gcFinal_K_Duty_clk_delay_cell[0] == -8 ||
						gcFinal_K_Duty_clk_delay_cell[1] == 8 ||
						gcFinal_K_Duty_clk_delay_cell[1] == -8)
                        {
                            u2FailStatusByCh[u1ChannelIdx]= DutyScan_Calibration_Flow(p, DutyScan_Calibration_K_DQS, 1);
                            use_rev_bit=1;
                        }

                    #if defined(YH_SWEEP_IC)
                        u2FailStatusByCh[u1ChannelIdx]|= DutyScan_Calibration_Flow(p, DutyScan_Calibration_K_DQ, 0);
                        u2FailStatusByCh[u1ChannelIdx]|= DutyScan_Calibration_Flow(p, DutyScan_Calibration_K_DQM, 0);
                    #endif
                    #if FT_DSIM_USED==0
                    //restore to orignal value
                    dramc_restore_registers(p, u4RegBackupAddress, sizeof(u4RegBackupAddress)/sizeof(unsigned int));
                    #endif

                    //Set K DQS MCK4X_DLY_EN and MCK4XB_DLY_EN again, this is especially for K DQS because other bit fields need to be restored.
                    DQSDutyScan_SetDqsDelayCell(p, gcFinal_K_Duty_clk_delay_cell, use_rev_bit);

                    io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG), 0, RKCFG_TXRANK);
                    io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG), 0, RKCFG_TXRANKFIX);
                }
            }

            set_phy_2_channel_mapping(p, u1backup_channel);
        }

#if !FT_DSIM_USED
#if defined(YH_SWEEP_IC)
        YH_Sweep_IC_Print_Result(p);
        while(1); //stop here
#endif
#endif
    }
}
#endif

