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

/** @file dramc_mr_api.c
 *  Basic DRAMC API implementation
 */

/* Include files */
#include "dramc_common.h"
#include "x_hal_io.h"

#if SUPPORT_TYPE_LPDDR4
/* cc note that for LP4, AD_O1 is re-orgnized as DQ_O1 to DRAMC,
 * which match the order of dram die. So no need to consider pinmux for
 * MRR and CBT
 */
const unsigned char lpddr4_mrr_mapping_pop[CHANNEL_NUM][16] = {
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

static void mr_init_lp4_fsp0(DRAMC_CTX_T *p)
{
	unsigned char mr_fsp = FSP_0;
	unsigned char mr_11_value;
	unsigned char mr_22_value;

	/*
	 * FSP_0:
	 * 1. For un-term freqs
	 * 2. Assumes "data rate < DDR2667" are un-term
	 */
	mr_fsp = FSP_0;
	show_msg2((INFO, "\tFsp%d\n", mr_fsp));

	dram_mr.mr13_value[mr_fsp] = MR13_FSP0_INIT;
	dramc_mode_reg_write(p, MR13, dram_mr.mr13_value[mr_fsp]);

	/* MR12 use previous value, VREF-CA */
	dramc_mode_reg_write(p, MR12,
		dram_mr.mr12_value[p->channel][p->rank][mr_fsp]);

	dramc_mode_reg_write(p, MR01, dram_mr.mr01_value[mr_fsp]);

	/* MR2 set Read/Write Latency */
	if (p->freqGroup <= DDR1600_FREQ)
		dram_mr.mr02_value[mr_fsp] = MR02_1600;
	else if (p->freqGroup <= DDR2666_FREQ)
		dram_mr.mr02_value[mr_fsp] = MR02_2666;
	else if (p->freqGroup <= DDR3200_FREQ)
		dram_mr.mr02_value[mr_fsp] = MR02_3200;
	else if (p->freqGroup <= DDR3733_FREQ)
		dram_mr.mr02_value[mr_fsp] = MR02_3733;
	else /* 3733 < freqGroup, 4266 */
		dram_mr.mr02_value[mr_fsp] = MR02_4266;
	dramc_mode_reg_write(p, MR02, dram_mr.mr02_value[mr_fsp]);

	mr_11_value = MR11_ODT_DIS;
	dramc_mode_reg_write(p, MR11, mr_11_value);

	if (p->dram_type == TYPE_LPDDR4) {
		/* SOC-ODT, ODTE-CK, ODTE-CS, Disable ODTD-CA */
		mr_22_value = MR22_20;
	} else {	/* TYPE_LPDDR4x, LP4P */
		/* SOC-ODT, ODTE-CK, ODTE-CS, Disable ODTD-CA */
		mr_22_value = MR22_38;
	}
	dramc_mode_reg_write(p, MR22, mr_22_value);

	/* MR14 use previous value, MR14 VREF-DQ */
	dramc_mode_reg_write(p, MR14,
		dram_mr.mr14_value[p->channel][p->rank][mr_fsp]);

	/*
	 * MR3 set write-DBI and read-DBI
	 * (Disabled during calibration, enabled after K)
	 */
	dram_mr.mr03_value[mr_fsp] = (dram_mr.mr03_value[mr_fsp] & 0x3f);

	if ((p->dram_type == TYPE_LPDDR4X) || (p->dram_type == TYPE_LPDDR4P))
		dram_mr.mr03_value[mr_fsp] &= 0xfe;

	dramc_mode_reg_write(p, MR03, dram_mr.mr03_value[mr_fsp]);
}

static void mr_init_lp4_fsp1(DRAMC_CTX_T *p)
{
	unsigned char mr_fsp = FSP_0;
	unsigned char mr_11_value;
	unsigned char mr_22_value;

	/*
	 * FSP_1:
	 * 1. For term freqs
	 * 2. Assumes "data rate >= DDR2667" are terminated
	 */
	mr_fsp = FSP_1;
	show_msg2((INFO, "\tFsp%d\n", mr_fsp));

	dram_mr.mr13_value[mr_fsp] = MR13_FSP1_SET;
	dramc_mode_reg_write(p, MR13, dram_mr.mr13_value[mr_fsp]);

	/* MR12 use previous value,VREF-CA */
	dramc_mode_reg_write(p, MR12,
		dram_mr.mr12_value[p->channel][p->rank][mr_fsp]);

	dramc_mode_reg_write(p, MR01, dram_mr.mr01_value[mr_fsp]);

	/* MR2 set Read/Write Latency */
	if (p->freqGroup <= DDR1600_FREQ)
		dram_mr.mr02_value[mr_fsp] = MR02_1600;
	else if (p->freqGroup <= DDR2666_FREQ)
		dram_mr.mr02_value[mr_fsp] = MR02_2666;
	else if (p->freqGroup <= DDR3200_FREQ)
		dram_mr.mr02_value[mr_fsp] = MR02_3200;
	else if (p->freqGroup <= DDR3733_FREQ)
		dram_mr.mr02_value[mr_fsp] = MR02_3733;
	else /* 3733 < freqGroup, 4266 */
		dram_mr.mr02_value[mr_fsp] = MR02_4266;

	dramc_mode_reg_write(p, MR02, dram_mr.mr02_value[mr_fsp]);

	mr_11_value = MR11_ODT_80;
	dramc_mode_reg_write(p, MR11, mr_11_value);

	if (p->odt_onoff == ODT_OFF) {
		if (p->dram_type == TYPE_LPDDR4)
			mr_22_value = MR22_20;
		else /* TYPE_LPDDR4x, LP4P */
			mr_22_value = MR22_38;
	} else {
		if (p->dram_type == TYPE_LPDDR4)
			mr_22_value = MR22_24;
		else /* TYPE_LPDDR4x, LP4P */
			mr_22_value = MR22_3C;
	}
	dramc_mode_reg_write(p, MR22, mr_22_value);

	/* MR14 use previous value, VREF-DQ */
	if (p->odt_onoff == ODT_OFF)
		dramc_mode_reg_write(p, MR14,
			dram_mr.mr14_value[p->channel][p->rank][FSP_0]);

	else
		dramc_mode_reg_write(p, MR14,
			dram_mr.mr14_value[p->channel][p->rank][mr_fsp]);

	/*
	 * MR3 set write-DBI and read-DBI
	 * (Disabled during calibration, enabled after K)
	 */
	dram_mr.mr03_value[mr_fsp] = (dram_mr.mr03_value[mr_fsp] & 0x3f);

	if ((p->dram_type == TYPE_LPDDR4X) || (p->dram_type == TYPE_LPDDR4P))
		dram_mr.mr03_value[mr_fsp] &= 0xfe;

	dramc_mode_reg_write(p, MR03, dram_mr.mr03_value[mr_fsp]);
}

static void mode_reg_init_lp4(DRAMC_CTX_T *p)
{
	unsigned int rank_idx;
	unsigned char mr_fsp = FSP_0;
	unsigned char channel_idx;
	unsigned char operating_fsp = p->dram_fsp;
	unsigned char backup_channel, backup_rank;

	backup_channel = p->channel;
	backup_rank = get_rank(p);

	if ((operating_fsp == FSP_1) &&
		(get_pre_miock_jmeter_hqa_used_flag() == 0)) {
		#if SIMULATION_CBT
			cbt_switch_freq(p, CBT_LOW_FREQ);
		#endif
	}

	for (channel_idx = 0;
		(unsigned char)channel_idx <
		(unsigned char)(p->support_channel_num);
		channel_idx++) {
		set_phy_2_channel_mapping(p, channel_idx);

		for (rank_idx = 0;
			rank_idx < (unsigned int)(p->support_rank_num);
			rank_idx++) {
			set_rank(p, rank_idx);

			show_msg((INFO, "[ModeRegInit_LP4] CH%u RK%u\n",
				channel_idx, rank_idx));

			io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
				rank_idx, MRS_MRSRK);

			/*
			 * Note:MR37 for LP4P should be set
			 * before any Mode register.
			 * MR37 is not shadow register,
			 * just need to set by channel and rank.
			 * No need to set by FSP
			 */
			if (p->dram_type == TYPE_LPDDR4P)
				dramc_mode_reg_write(p, MR37, 0x1);

				/*
			 * ZQ calobration should be done
			 * before CBT calibration by switching to low frequency
			 */
		#if SIMULATION_LP4_ZQ
			dramc_zq_calibration(p);
		#endif

			/*
			 * FSP_0:
			 * 1. For un-term freqs
			 * 2. Assumes "data rate < DDR2667" are un-term
			 */
			 mr_init_lp4_fsp0(p);

			/*
			 * FSP_1: 1. For term freqs
			 * 2. Assumes "data rate >= DDR2667"
			 * are terminated
			 */
			 mr_init_lp4_fsp1(p);

			/* freq < 1333 is assumed to be odt_off -> uses FSP_0 */
			if (p->frequency < DDR2666_FREQ) {
				mr_fsp = FSP_0;
				dram_mr.mr13_value[mr_fsp] = MR13_FSP0_INIT;
				dramc_mode_reg_write(p, MR13,
					dram_mr.mr13_value[mr_fsp]);
			} else {
				mr_fsp = FSP_1;
				dram_mr.mr13_value[mr_fsp] = MR13_FSP1_INIT;
				dramc_mode_reg_write(p, MR13,
					dram_mr.mr13_value[mr_fsp]);
			}

		}

		/*
		 * Auto-MRW related register write
		 * (Used during HW DVFS frequency switch flow)
		 *
		 * VRCG seems to be enabled / disabled even when switching
		 * to same FSP(but different freq) to simplify HW DVFS flow
		 * 1. MR13 OP[3] =
		 * 1:Enable "high current mode" to reduce the settling time
		 * when changing FSP(freq) during operation
		 */
		io_32_write_fld_multi(
			DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR13),
			p_fld(dram_mr.mr13_value[mr_fsp] | (SET_FLD << 3),
			SHU_HWSET_MR13_HWSET_MR13_OP) |
			p_fld(13, SHU_HWSET_MR13_HWSET_MR13_MRSMA));
		/*
		 * 2. MR13 OP[3] =
		 * 1:Enable "high current mode"
		 * after FSP(freq) switch operation
		 */
		io_32_write_fld_multi(
			DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_VRCG),
			p_fld(dram_mr.mr13_value[mr_fsp] | (SET_FLD << 3),
			SHU_HWSET_VRCG_HWSET_VRCG_OP) |
			p_fld(13, SHU_HWSET_VRCG_HWSET_VRCG_MRSMA));
		/* 3. MR2:Set RL/WL after FSP(freq) switch */
		io_32_write_fld_multi(
			DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR2),
			p_fld(dram_mr.mr02_value[mr_fsp],
			SHU_HWSET_MR2_HWSET_MR2_OP) |
			p_fld(2, SHU_HWSET_MR2_HWSET_MR2_MRSMA));
	}

	set_phy_2_channel_mapping(p, backup_channel);
	set_rank(p, backup_rank);

	if ((operating_fsp == FSP_1) &&
		(get_pre_miock_jmeter_hqa_used_flag() == 0)) {
		#if SIMULATION_CBT
			cbt_switch_freq(p, CBT_HIGH_FREQ);
		#endif
	}
}

void dramc_power_on_sequence(DRAMC_CTX_T *p)
{
	/* reset dram = low */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1),
		CLEAR_FLD, MISC_CTRL1_R_DMDA_RRESETB_I);

	/* CKE low */
	cke_fix_on_off(p, CKE_WRITE_TO_ALL_RANK, CKE_FIXOFF, CKE_WRITE_TO_ALL_CHANNEL);
	/* delay tINIT1=200us(min) & tINIT2=10ns(min) */
	delay_us(200);

	/* reset dram = high */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), SET_FLD,
		MISC_CTRL1_R_DMDA_RRESETB_I);

	/* Disable HW MIOCK control to make CLK always on */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL),
		SET_FLD, DRAMC_PD_CTRL_MIOCKCTRLOFF);

	/* tINIT3=2ms(min) */
	delay_ms(2);

	/* CKE high */
	cke_fix_on_off(p, CKE_WRITE_TO_ALL_RANK, CKE_FIXON, CKE_WRITE_TO_ALL_CHANNEL);
	/* tINIT5=2us(min) */
	delay_us(2);
	/* u1PowerOn=1; */
	show_msg3((INFO, "APPLY_LP4_POWER_INIT_SEQUENCE\n"));
}

DRAM_STATUS_T dramc_mr_init_lp4(DRAMC_CTX_T *p)
{
	unsigned int backup_broadcast;

	backup_broadcast = get_dramc_broadcast();
	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	dramc_power_on_sequence(p);

	/* Fix nWR value to 30 (MR01[6:4] = 101B) for DDR3200
	 * Fix nWR value to 34 (MR01[6:4] = 110B) for DDR3733
	 * Other vendors: Use default MR01 for each FSP
	 * (Set in vInitGlobalVariablesByCondition() )
	 */
	/* Clear MR01 OP[6:4] */
	dram_mr.mr01_value[FSP_0] &= 0x8F;
	dram_mr.mr01_value[FSP_1] &= 0x8F;
#if (LP4_HIGHEST_FREQ == DDR3733_FREQ)
	/* Set MR01 OP[6:4] to 110B = 6 */
	dram_mr.mr01_value[FSP_0] |= (0x6 << 4);
	dram_mr.mr01_value[FSP_1] |= (0x6 << 4);
	show_msg3((INFO, "nWR fixed to 34\n"));
#else
	/* Set MR01 OP[6:4] to 101B = 5 */
	dram_mr.mr01_value[FSP_0] |= (0x5 << 4);
	dram_mr.mr01_value[FSP_1] |= (0x5 << 4);
	show_msg3((INFO, "nWR fixed to 30\n"));
#endif

	mode_reg_init_lp4(p);

	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_MRS), RANK_0,
		MRS_MRSRK);

	dramc_broadcast_on_off(backup_broadcast);

	return DRAM_OK;
}

#ifdef TEMP_SENSOR_ENABLE
#ifdef SUPPORT_TYPE_LPDDR4
unsigned char get_mr4_refresh_rate(DRAMC_CTX_T *p, DRAM_CHANNEL_T channel)
{
	unsigned char refresh_rate;

	refresh_rate =
		(unsigned char)io_32_read_fld_align((DRAMC_REG_MISC_STATUSA) +
		(channel << POS_BANK_NUM), MISC_STATUSA_REFRESH_RATE);

	return refresh_rate;
}
#endif
#endif /* TEMP_SENSOR_ENABLE */

void set_mr13_vrcg_to_normal_operation(DRAMC_CTX_T *p)
{
	DRAM_CHANNEL_T ch_back = get_phy_2_channel_mapping(p);
	unsigned char shuffle_idx = 0;
	unsigned char ch_idx = CHANNEL_A;
	unsigned char rank_idx = 0;
	unsigned char mr_fsp = (p->odt_onoff == ODT_OFF) ? FSP_0 : FSP_1;
	unsigned int value = 0;

	for (ch_idx = CHANNEL_A; ch_idx < p->support_channel_num; ch_idx++) {
		set_phy_2_channel_mapping(p, ch_idx);
		/* To DRAM: MR13[3] = 0 */
		for (rank_idx = 0; rank_idx < p->support_rank_num;
			rank_idx++) {
			dramc_mode_reg_write_by_rank(p, rank_idx, MR13,
				dram_mr.mr13_value[mr_fsp] & ~(SET_FLD << 3));
		}
		/*DVFS MRW*/
		for (shuffle_idx = 0; shuffle_idx < DRAM_DFS_SHUFFLE_MAX;
			shuffle_idx++) {
			value = io_32_read_fld_align(DRAMC_REG_ADDR
				(DRAMC_REG_SHU_HWSET_VRCG) +
				SHU_GRP_DRAMC_OFFSET * shuffle_idx,
				SHU_HWSET_VRCG_HWSET_VRCG_OP);
			io_32_write_fld_align(DRAMC_REG_ADDR
				(DRAMC_REG_SHU_HWSET_VRCG) +
				SHU_GRP_DRAMC_OFFSET * shuffle_idx,
				value & ~(SET_FLD << 3),
				SHU_HWSET_VRCG_HWSET_VRCG_OP);
		}
	}
	set_phy_2_channel_mapping(p, (unsigned char)ch_back);
}

/* Only for LP4 */
#if DRAMC_MODEREG_CHECK
void dramc_mode_reg_check2(DRAMC_CTX_T *p, unsigned char ch,
	unsigned char mr_fsp, unsigned char rank)
{

	unsigned char operating_fsp = p->dram_fsp;
	unsigned char ii, mr[] = {MR04, MR05, MR12, MR14, MR18, MR19};
	unsigned short mr_value = 0, value = 0;
	unsigned char match = 0;

	if (mr_fsp == FSP_1)
		dram_mr.mr13_value[operating_fsp] |= 0x40;
	else
		dram_mr.mr13_value[operating_fsp] &= ~0x40;

	dramc_mode_reg_write_by_rank(p, rank, MR13,
		dram_mr.mr13_value[operating_fsp]);

	for (ii = 0; ii < sizeof(mr); ii++) {
		dramc_mrr_by_rank(p, rank, mr[ii], &value);
		value &= 0xff;

		if ((mr[ii] == MR12) || (mr[ii] == MR14)) {
			if (mr[ii] == MR12)
				mr_value =
					dram_mr.mr12_value[ch][rank][mr_fsp];
			else if (mr[ii] == MR14)
				mr_value =
					dram_mr.mr14_value[ch][rank][mr_fsp];

			match = (value == mr_value) ? 1 : 0;
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
			hqa_log_message_for_report(p, ch, rank, HQA_REPORT_FORMAT1,
				(mr_fsp == (uint32_t)FSP_0) ? "DramcModeReg_Read_FSP_0_MR" :
				"DramcModeReg_Read_FSP_1_MR", mr[ii], (int32_t)value, NULL);
#endif

			show_msg2((INFO,
				"\t\tMR%d = 0x%x (global = 0x%x)\t%smatch\n",
				mr[ii], value, mr_value,
				(match == 1) ? "" : "mis"));
			if (match != 1) {
				show_msg((INFO,
					"\t\tMR%d=0x%x (global=0x%x)\t%s\n",
					mr[ii], value, mr_value, "mismatch"));
			}
		} else {
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
			hqa_log_message_for_report(p, ch, rank, HQA_REPORT_FORMAT1,
				(mr_fsp == (uint32_t)FSP_0) ? "DramcModeReg_Read_FSP_0_MR" :
				"DramcModeReg_Read_FSP_1_MR", mr[ii], (int32_t)value, NULL);
#endif

			show_msg2((INFO, "\t\tMR%d = 0x%x\n", mr[ii], value));
		}
	}
}


void dramc_mode_reg_check(DRAMC_CTX_T *p)
{
	unsigned char backup_channel;
	unsigned char ch;
	unsigned char mr_fsp;
	unsigned char rank;

	show_msg((INFO, "\n\n[DramcModeReg_Check] Freq_%d, FSP_%d\n",
		p->frequency, p->dram_fsp));

	backup_channel = get_phy_2_channel_mapping(p);

#if DUAL_FREQ_K
	for (mr_fsp = FSP_0; mr_fsp <= (p->dram_fsp); mr_fsp++) {
#else
	for (mr_fsp = (p->dram_fsp); mr_fsp <= (p->dram_fsp); mr_fsp++) {
#endif
		for (ch = 0; ch < (p->support_channel_num); ch++) {
			set_phy_2_channel_mapping(p, ch);
			for (rank = 0; rank <
				(unsigned int)(p->support_rank_num); rank++) {
				show_msg2((INFO, "FSP_%d, CH_%d, RK%d\n",
					mr_fsp, ch, rank));
				dramc_mode_reg_check2(p, ch, mr_fsp, rank);
			}
		}
	}

	set_phy_2_channel_mapping(p, backup_channel);
}
#endif /* DRAMC_MODEREG_CHECK */
#endif /* SUPPORT_TYPE_LPDDR4 */

#if SUPPORT_TYPE_PCDDR3
#define DDR_PC3_MR1		(0x1 << 6) /* set RTT_NOM = 120ohm */

void dramc_power_on_sequence_ddr3(DRAMC_CTX_T *p)
{
	/* reset dram = low */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1),
		SET_FLD, MISC_CTRL1_R_DMRRESETB_I_OPT);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1),
		CLEAR_FLD, MISC_CTRL1_R_DMDA_RRESETB_I);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1),
		SET_FLD, MISC_CTRL1_R_DMDA_RRESETB_E);
	/* CKE low */
	cke_fix_on_off(p, CKE_WRITE_TO_ALL_RANK, CKE_FIXOFF, CKE_WRITE_TO_ALL_CHANNEL);
	/* delay tINIT1=200us(min) & tINIT2=10ns(min) */
	delay_us(200);

	/* reset dram = high */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), SET_FLD,
		MISC_CTRL1_R_DMDA_RRESETB_I);

	/* Disable HW MIOCK control to make CLK always on */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL),
		SET_FLD, DRAMC_PD_CTRL_MIOCKCTRLOFF);

	/* tINIT3=2ms(min) */
	delay_us(500);

	/* CKE high */
	cke_fix_on_off(p, CKE_WRITE_TO_ALL_RANK, CKE_FIXON, CKE_WRITE_TO_ALL_CHANNEL);
	/* tINIT5=2us(min) */
	delay_us(2);
	/* u1PowerOn=1; */
	show_msg3((INFO, "APPLY_DDR3_POWER_INIT_SEQUENCE\n"));
}

void dramc_mr_init_ddr3(DRAMC_CTX_T *p)
{
	unsigned char backup_channel, backup_rank;
	unsigned int rank_idx;
	unsigned char channel_idx;

	backup_channel = p->channel;
	backup_rank = p->rank;
	dramc_power_on_sequence_ddr3(p);

	switch (p->frequency) {
	case DDR1066_FREQ:
		dram_mr.mr02_value[p->dram_fsp] = 0x08;
		dram_mr.mr03_value[p->dram_fsp] = 0x0;
		dram_mr.mr01_value[p->dram_fsp] = DDR_PC3_MR1;
		dram_mr.mr00_value[p->dram_fsp] = 0x1940;
		break;

	case DDR1333_FREQ:
		dram_mr.mr02_value[p->dram_fsp] = 0x10;
		dram_mr.mr03_value[p->dram_fsp] = 0x0;
		dram_mr.mr01_value[p->dram_fsp] = DDR_PC3_MR1;
		dram_mr.mr00_value[p->dram_fsp] = 0x1b50;
		break;

	case DDR1600_FREQ:
		dram_mr.mr02_value[p->dram_fsp] = 0x18;
		dram_mr.mr03_value[p->dram_fsp] = 0x0;
		dram_mr.mr01_value[p->dram_fsp] = DDR_PC3_MR1;
		dram_mr.mr00_value[p->dram_fsp] = 0x1d70;
		break;

	default:
	case DDR1866_FREQ:
		dram_mr.mr02_value[p->dram_fsp] = 0x20;
		dram_mr.mr03_value[p->dram_fsp] = 0x0;
		dram_mr.mr01_value[p->dram_fsp] = DDR_PC3_MR1;
		dram_mr.mr00_value[p->dram_fsp] = 0x1170;
		break;
	}

	for (channel_idx = 0;
		(unsigned char)channel_idx <
		(unsigned char)(p->support_channel_num);
		channel_idx++) {
		set_phy_2_channel_mapping(p, channel_idx);

		for (rank_idx = 0;
			rank_idx < (unsigned int)(p->support_rank_num);
			rank_idx++) {
			set_rank(p, rank_idx);
			show_msg((INFO, "[ModeRegInit_DDR3] CH%u RK%u\n",
				channel_idx, rank_idx));
			io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
				rank_idx, MRS_MRSRK);

			dramc_mode_reg_write(p, 0x2, dram_mr.mr02_value[p->dram_fsp]);
			dramc_mode_reg_write(p, 0x3, dram_mr.mr03_value[p->dram_fsp]);
			dramc_mode_reg_write(p, 0x1, dram_mr.mr01_value[p->dram_fsp]);
			dramc_mode_reg_write(p, 0x0, dram_mr.mr00_value[p->dram_fsp]);

			io_32_write_fld_align_all(DRAMC_REG_MRS,0x400, MRS_MRSMA);
			io_32_write_fld_align_all(DRAMC_REG_SPCMD, 1, SPCMD_ZQCEN);
			delay_us(1);
			io_32_write_fld_align_all(DRAMC_REG_SPCMD, 0, SPCMD_ZQCEN);
		}
	}
	set_phy_2_channel_mapping(p, backup_channel);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
				backup_rank, MRS_MRSRK);
	set_rank(p, backup_rank);
}
#endif

#if SUPPORT_TYPE_PCDDR4
#define DDR_PC4_TCCD	3
#define DDR_PC4_MR1		(0x001 | (6 << 8)) /* RTT_NOM = 80ohm */

void dramc_power_on_sequence_ddr4(DRAMC_CTX_T *p)
{
	/* reset dram = low */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1),
		CLEAR_FLD, MISC_CTRL1_R_DMDA_RRESETB_I);

	/* CKE low */
	cke_fix_on_off(p, CKE_WRITE_TO_ALL_RANK, CKE_FIXOFF, CKE_WRITE_TO_ALL_CHANNEL);
	/* delay tINIT1=200us(min) & tINIT2=10ns(min) */
	delay_us(200);

	/* reset dram = high */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), SET_FLD,
		MISC_CTRL1_R_DMDA_RRESETB_I);

	/* Disable HW MIOCK control to make CLK always on */
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL),
		SET_FLD, DRAMC_PD_CTRL_MIOCKCTRLOFF);

	/* tINIT3=2ms(min) */
	delay_us(500);

	/* CKE high */
	cke_fix_on_off(p, CKE_WRITE_TO_ALL_RANK, CKE_FIXON, CKE_WRITE_TO_ALL_CHANNEL);
	/* tINIT5=2us(min) */
	delay_us(2);
	/* u1PowerOn=1; */
	show_msg3((INFO, "APPLY_DDR4_POWER_INIT_SEQUENCE\n"));
}

void dramc_mr_init_ddr4(DRAMC_CTX_T *p)
{
	unsigned char backup_channel, backup_rank;
	unsigned int backup_broadcast;
	unsigned int rank_idx;
	unsigned char channel_idx;

	backup_broadcast = get_dramc_broadcast();
	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	dramc_power_on_sequence_ddr4(p);
	backup_channel = p->channel;
	backup_rank = p->rank;
	switch (p->frequency) {
	default:
	case DDR3200_FREQ:
		dram_mr.mr02_value[p->dram_fsp] = 0x38;
		dram_mr.mr03_value[p->dram_fsp] = 0x0;
		dram_mr.mr01_value[p->dram_fsp] = DDR_PC4_MR1;
		dram_mr.mr04_value[p->dram_fsp] = 0x800;
		dram_mr.mr05_value[p->dram_fsp] = 0x400;
		dram_mr.mr06_value[p->dram_fsp] = (0x4 << 10);
		dram_mr.mr00_value[p->dram_fsp] = 0xd54;
		break;

	case DDR2666_FREQ:
		dram_mr.mr02_value[p->dram_fsp] = 0x30;
		dram_mr.mr03_value[p->dram_fsp] = 0x0;
		dram_mr.mr01_value[p->dram_fsp] = DDR_PC4_MR1;
		dram_mr.mr04_value[p->dram_fsp] = 0x800;
		dram_mr.mr05_value[p->dram_fsp] = 0x400;
		dram_mr.mr06_value[p->dram_fsp] = (DDR_PC4_TCCD << 10);
		dram_mr.mr00_value[p->dram_fsp] = 0xb44;
		break;

	case DDR1600_FREQ:
		dram_mr.mr02_value[p->dram_fsp] = 0x0;
		dram_mr.mr03_value[p->dram_fsp] = 0x0;
		dram_mr.mr01_value[p->dram_fsp] = DDR_PC4_MR1;
		dram_mr.mr04_value[p->dram_fsp] = 0x0;
		dram_mr.mr05_value[p->dram_fsp] = 0x400;
		dram_mr.mr06_value[p->dram_fsp] = (0x1 << 10);
		dram_mr.mr00_value[p->dram_fsp] = 0x310;
		break;

	case DDR1333_FREQ:
		dram_mr.mr02_value[p->dram_fsp] = 0x0;
		dram_mr.mr03_value[p->dram_fsp] = 0x0;
		dram_mr.mr01_value[p->dram_fsp] = DDR_PC4_MR1;
		dram_mr.mr04_value[p->dram_fsp] = 0x0;
		dram_mr.mr05_value[p->dram_fsp] = 0x400;
		dram_mr.mr06_value[p->dram_fsp] = 0x0;
		dram_mr.mr00_value[p->dram_fsp] = 0xd10;
		break;
	}

	for (channel_idx = 0;
		(unsigned char)channel_idx <
		(unsigned char)(p->support_channel_num);
		channel_idx++) {
		set_phy_2_channel_mapping(p, channel_idx);

		for (rank_idx = 0;
			rank_idx < (unsigned int)(p->support_rank_num);
			rank_idx++) {
			set_rank(p, rank_idx);

			show_msg((INFO, "[ModeRegInit_DDR4] CH%u RK%u\n",
				channel_idx, rank_idx));
			io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
				rank_idx, MRS_MRSRK);

			dramc_mode_reg_write(p, 2, dram_mr.mr02_value[p->dram_fsp]);
			dramc_mode_reg_write(p, 3, dram_mr.mr03_value[p->dram_fsp]);
			dramc_mode_reg_write(p, 1, dram_mr.mr01_value[p->dram_fsp]);
			dramc_mode_reg_write(p, 4, dram_mr.mr04_value[p->dram_fsp]);
			dramc_mode_reg_write(p, 5, dram_mr.mr05_value[p->dram_fsp]);
			dramc_mode_reg_write(p, 6, 0x080 |
				dram_mr.mr06_value[p->dram_fsp]);
			delay_us(10);
			dramc_mode_reg_write(p, 6, 0x0C8 |
				dram_mr.mr06_value[p->dram_fsp]);
			delay_us(10);
			dramc_mode_reg_write(p, 6, 0x048 |
				dram_mr.mr06_value[p->dram_fsp]);
			delay_us(10);
			dramc_mode_reg_write(p, 0, dram_mr.mr00_value[p->dram_fsp]);
			delay_us(100);

			io_32_write_fld_align_all(DRAMC_REG_MRS,0x400, MRS_MRSMA);
			io_32_write_fld_align_all(DRAMC_REG_SPCMD, 1, SPCMD_ZQCEN);
			delay_us(1);
			io_32_write_fld_align_all(DRAMC_REG_SPCMD, 0, SPCMD_ZQCEN);
		}
	}

	set_phy_2_channel_mapping(p, backup_channel);
	set_rank(p, backup_rank);

	dramc_broadcast_on_off(backup_broadcast);

}
#endif

#if SUPPORT_TYPE_LPDDR3
/* mrr pinmux */
const unsigned char lpddr3_mrr_mapping_pop[CHANNEL_NUM][32] = {
#if 0 /* PINMUX not applied */
	{
		0, 1, 2, 3, 4, 6, 5, 7,
		9, 8, 10, 11, 15, 12, 14, 13
	},
#else /* PINMUX applied */
	/*{
		25, 24, 21, 23, 27, 26, 20, 22,
		8, 10, 4, 6, 11, 9, 7, 5,
		17, 16, 18, 31, 19, 29, 30, 28,
		1, 0, 13, 14, 3, 2, 15, 12,
	},*/
	{
		0, 1, 2, 3, 4, 5, 6, 7,
		8, 9, 10, 11, 12, 13, 14, 15,
		16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31,
	},

#endif
};

void dramc_mr_init_lp3(DRAMC_CTX_T *p, unsigned char reset_dram)
{
	unsigned int rankidx, autorefreshbak;

	autorefreshbak = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0),
		REFCTRL0_REFDIS);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1,
		DRAMC_PD_CTRL_MIOCKCTRLOFF);

	delay_us(1);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
		1, CKECTRL_CKEFIXON);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
		1, CKECTRL_CKE1FIXON);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0),
		1, REFCTRL0_REFDIS);

	/*
	 * wait cke to reset timing constraint 'tinit3'
	 * for (i=0; i < 10 ; ++i);
	 */
	delay_us(200); /* tINIT3 timing */

	for (rankidx = 0; rankidx < (U32)(p->support_rank_num); rankidx++) {
		show_msg2((INFO, "DramcModeRegInit_LP for Rank%d\n", rankidx));

		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
			rankidx, MRS_MRSRK);

		switch (p->frequency) {
		default:
		case DDR1866_FREQ:
			dram_mr.mr63_value[p->dram_fsp] = 0x0;
			dram_mr.mr10_value[p->dram_fsp] = 0xff;
			dram_mr.mr01_value[p->dram_fsp] = 0x83;
			dram_mr.mr02_value[p->dram_fsp] = 0x1c;
			break;

		case DDR1600_FREQ:
			dram_mr.mr63_value[p->dram_fsp] = 0x0;
			dram_mr.mr10_value[p->dram_fsp] = 0xff;
			dram_mr.mr01_value[p->dram_fsp] = 0x83;
			dram_mr.mr02_value[p->dram_fsp] = 0x1a;
			break;

		case DDR1333_FREQ:
			dram_mr.mr63_value[p->dram_fsp] = 0x0;
			dram_mr.mr10_value[p->dram_fsp] = 0xff;
			dram_mr.mr01_value[p->dram_fsp] = 0x03;
			dram_mr.mr02_value[p->dram_fsp] = 0x18;
			break;

		case DDR1200_FREQ:
			dram_mr.mr63_value[p->dram_fsp] = 0x0;
			dram_mr.mr10_value[p->dram_fsp] = 0xff;
			dram_mr.mr01_value[p->dram_fsp] = 0x73;
			dram_mr.mr02_value[p->dram_fsp] = 0x07;
			break;
		}
		dramc_mode_reg_write(p, 0x3f, dram_mr.mr63_value[p->dram_fsp]);
		delay_us(10); /* to meet tINIT4 timing */

		dramc_mode_reg_write(p, 0xa, dram_mr.mr10_value[p->dram_fsp]);
		delay_us(2);

		dramc_mode_reg_write(p, 0x1, dram_mr.mr01_value[p->dram_fsp]);
		dramc_mode_reg_write(p, 0x2, dram_mr.mr02_value[p->dram_fsp]);
	}

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), 0, MRS_MRSRK);

	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_MR2),
		p_fld(0, SHU_HWSET_MR2_HWSET_MR2_OP) |
		p_fld(0, SHU_HWSET_MR2_HWSET_MR2_MRSMA));

	io_32_write_fld_multi(DRAMC_REG_SHU_HWSET_MR13,
		p_fld(dram_mr.mr02_value[p->dram_fsp], SHU_HWSET_MR13_HWSET_MR13_OP) |
		p_fld(2, SHU_HWSET_MR13_HWSET_MR13_MRSMA));

	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_HWSET_VRCG),
		p_fld(0, SHU_HWSET_VRCG_HWSET_VRCG_OP) |
		p_fld(0, SHU_HWSET_VRCG_HWSET_VRCG_MRSMA));

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0), autorefreshbak,
		REFCTRL0_REFDIS);

	return DRAM_OK;
}
#endif

void set_mrr_pinmux_mapping(DRAMC_CTX_T *p)
{
#if (SUPPORT_TYPE_LPDDR4 || SUPPORT_TYPE_LPDDR3)
	unsigned char *lpddr_mrr_mapping = NULL;
	unsigned char backup_broadcast, backup_channel;
	DRAM_CHANNEL_T chIdx = CHANNEL_A;

	if(!(is_lp4_family( p) || p->dram_type == TYPE_LPDDR3)){
		return;
	}

	backup_channel = get_phy_2_channel_mapping(p);
	backup_broadcast = get_dramc_broadcast();

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);

	/*
	 * LP4 and LP3: Set each channel's pinmux individually,
	 */
	for (chIdx = CHANNEL_A;
		(unsigned char)chIdx < (unsigned char)(p->support_channel_num);
		chIdx++) {
		set_phy_2_channel_mapping(p, chIdx);

		if (is_lp4_family(p)) {
		#if SUPPORT_TYPE_LPDDR4
			lpddr_mrr_mapping =
				(unsigned char *)lpddr4_mrr_mapping_pop[chIdx];
		#endif
		} else if (p->dram_type == TYPE_LPDDR3) {
		#if SUPPORT_TYPE_LPDDR3
			lpddr_mrr_mapping =
				(unsigned char *)lpddr3_mrr_mapping_pop[chIdx];
		#endif
		}

		/* Set MRR pin mux */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX1),
			p_fld(lpddr_mrr_mapping[0],
				MRR_BIT_MUX1_MRR_BIT0_SEL) |
			p_fld(lpddr_mrr_mapping[1],
				MRR_BIT_MUX1_MRR_BIT1_SEL) |
			p_fld(lpddr_mrr_mapping[2],
				MRR_BIT_MUX1_MRR_BIT2_SEL) |
			p_fld(lpddr_mrr_mapping[3],
				MRR_BIT_MUX1_MRR_BIT3_SEL));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX2),
			p_fld(lpddr_mrr_mapping[4],
				MRR_BIT_MUX2_MRR_BIT4_SEL) |
			p_fld(lpddr_mrr_mapping[5],
				MRR_BIT_MUX2_MRR_BIT5_SEL) |
			p_fld(lpddr_mrr_mapping[6],
				MRR_BIT_MUX2_MRR_BIT6_SEL) |
			p_fld(lpddr_mrr_mapping[7],
				MRR_BIT_MUX2_MRR_BIT7_SEL));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX3),
			p_fld(lpddr_mrr_mapping[8],
				MRR_BIT_MUX3_MRR_BIT8_SEL) |
			p_fld(lpddr_mrr_mapping[9],
				MRR_BIT_MUX3_MRR_BIT9_SEL) |
			p_fld(lpddr_mrr_mapping[10],
				MRR_BIT_MUX3_MRR_BIT10_SEL) |
			p_fld(lpddr_mrr_mapping[11],
				MRR_BIT_MUX3_MRR_BIT11_SEL));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_MRR_BIT_MUX4),
			p_fld(lpddr_mrr_mapping[12],
				MRR_BIT_MUX4_MRR_BIT12_SEL) |
			p_fld(lpddr_mrr_mapping[13],
				MRR_BIT_MUX4_MRR_BIT13_SEL) |
			p_fld(lpddr_mrr_mapping[14],
				MRR_BIT_MUX4_MRR_BIT14_SEL) |
			p_fld(lpddr_mrr_mapping[15],
				MRR_BIT_MUX4_MRR_BIT15_SEL));
	}

	set_phy_2_channel_mapping(p, backup_channel);
	dramc_broadcast_on_off(backup_broadcast);
#endif
}

void dramc_mode_reg_read(DRAMC_CTX_T *p, unsigned char mr_idx,
	unsigned short *value)
{
	unsigned int mr_value;

	if ((is_lp4_family(p)) ||
		(p->dram_type == TYPE_LPDDR3)) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), mr_idx,
			MRS_MRSMA);
	} else {
		if (p->dram_type == TYPE_PCDDR3) {
			io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), mr_idx,
				MRS_MRSBA);
		} else {
			/* DDR4 uses BG[0], BA[1], BA[0] to index MR */
			io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
				mr_idx & 0x3, MRS_MRSBA);
			io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
				(mr_idx >> 2) & 0x1, MRS_MRSBG);
		}
	}

	/* MRR command will be fired when MRREN 0->1 */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), SET_FLD,
		SPCMD_MRREN);

	/* wait MRR command fired. */
	while (io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP),
		SPCMDRESP_MRR_RESPONSE) == 0) {
		delay_us(1);
	}

	/* Read out mode register value */
	mr_value =
		io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRR_STATUS),
		MRR_STATUS_MRR_SW_REG);
	*value = (unsigned short)mr_value;

	/* Set MRREN =0 for next time MRR. */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), CLEAR_FLD,
		SPCMD_MRREN);

	show_msg2((INFO, "Read MR%d =0x%x\n", mr_idx, mr_value));
}

void dramc_mrr_by_rank(DRAMC_CTX_T *p, unsigned char rank,
	unsigned char mr_idx, unsigned short *value_p)
{
	unsigned short value = 0;
	unsigned char rank_bak;
        unsigned char scinput_opt = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DRSCTRL), DRSCTRL_RK_SCINPUT_OPT);

	if (scinput_opt == 0)
	{
		rank_bak = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
			MRS_MRSRK);
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), rank,
			MRS_MRSRK);

		/* Mode reg read */
		dramc_mode_reg_read(p, mr_idx, &value);
		*value_p = value;

		/* Restore rank */
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), rank_bak,
			MRS_MRSRK);
	}
	else
	{
		rank_bak = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
			MRS_MRRRK);
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), rank,
			MRS_MRRRK);

		/* Mode reg read */
		dramc_mode_reg_read(p, mr_idx, &value);
		*value_p = value;

		/* Restore rank */
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), rank_bak,
			MRS_MRRRK);
	}
}

void dramc_mode_reg_write(DRAMC_CTX_T *p, unsigned char mr_idx,
	unsigned short value)
{
	unsigned int counter = 0;
	unsigned int rank = 0;
	unsigned int register_ckectl;

	register_ckectl = io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL));
	rank = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), MRS_MRSRK);

	/* CKE must be fix on when doing MRW */
	cke_fix_on_off(p, rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	if ((is_lp4_family(p)) ||
		(p->dram_type == TYPE_LPDDR3)) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), mr_idx,
			MRS_MRSMA);
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), value & 0xFF,
			MRS_MRSOP);
	} else {
		if (p->dram_type == TYPE_PCDDR3) {
			io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), mr_idx,
				MRS_MRSBA);
		} else {
			/* DDR4 uses BG[0], BA[1], BA[0] to index MR */
			io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
				mr_idx & 0x3, MRS_MRSBA);
			io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
				(mr_idx >> 2) & 0x1, MRS_MRSBG);
		}
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), value,
			MRS_MRSMA);
	}

	/* MRW command will be fired when MRWEN 0->1 */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), SET_FLD,
		SPCMD_MRWEN);

	/* wait MRW command fired. */
	while (io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP),
		SPCMDRESP_MRW_RESPONSE) == 0) {
		counter++;
		show_msg((INFO,
			"wait MRW command Rank%d MR%d =0x%x fired (%d)\n",
			rank, mr_idx, value, counter));
		delay_us(1);
	}

	/* Set MRWEN =0 for next time MRW. */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), CLEAR_FLD,
		SPCMD_MRWEN);

	io32_write_4b(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL), register_ckectl);

	rank = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), MRS_MRSRK);
	if (print_mode_reg_write)
		show_msg((INFO, "Write Rank%d MR%d =0x%x\n", rank,
			mr_idx, value));
	show_mrw((INFO, "[%dMHz] Write Rank%d MR%d =0x%x\n", p->frequency, rank, mr_idx, value));
}

void dramc_mode_reg_write_by_rank(DRAMC_CTX_T *p, unsigned char rank,
	unsigned char mr_idx, unsigned short value)
{
	unsigned int rank_backup;

	rank_backup = io_32_read_fld_align
		(DRAMC_REG_ADDR(DRAMC_REG_MRS), MRS_MRSRK);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
		rank, MRS_MRSRK);

	/* write mode register */
	dramc_mode_reg_write(p, mr_idx, value);

	/* restore rank */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
		rank_backup, MRS_MRSRK);
}

