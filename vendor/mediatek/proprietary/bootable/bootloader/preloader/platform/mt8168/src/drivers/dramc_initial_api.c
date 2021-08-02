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

/** @file dramc_initial_api.c
 *  Basic DRAMC API implementation
 */

 /* Include files */
#include "dramc_common.h"
#include "x_hal_io.h"

static void apply_ssc_setting(DRAMC_CTX_T *p,
	unsigned char percent, unsigned char slope, unsigned char dir)
{
	if (p->ssc_en == ENABLE) {
		unsigned int prd, delta, delta1;

		prd = 13000 / slope;
		delta = (((p->frequency << 18) / 26) *percent) / (prd *100);
		delta1 = delta;
		if (p->ssc_en == ENABLE) {
			io_32_write_fld_multi_all(DDRPHY_SHU_PLL5,
				p_fld(SET_FLD, SHU_PLL5_RG_RPHYPLL_SDM_FRA_EN));
			io_32_write_fld_multi_all(DDRPHY_SHU_PLL7,
			p_fld(SET_FLD, SHU_PLL7_RG_RCLRPLL_SDM_FRA_EN));
		} else {
			io_32_write_fld_multi_all(DDRPHY_SHU_PLL14,
				p_fld(CLEAR_FLD, SHU_PLL5_RG_RPHYPLL_SDM_FRA_EN));
			io_32_write_fld_multi_all(DDRPHY_SHU_PLL7,
			p_fld(CLEAR_FLD, SHU_PLL7_RG_RCLRPLL_SDM_FRA_EN));
		}

		io_32_write_fld_align_all(DDRPHY_SHU_PLL20,
			prd, SHU_PLL20_RG_RCLRPLL_SDM_SSC_PRD);
		io_32_write_fld_align_all(DDRPHY_SHU_PLL20,
			dir, SHU_PLL20_RG_RCLRPLL_SDM_SSC_PH_INIT); /* ssc direction*/
		io_32_write_fld_align_all(DDRPHY_SHU_PLL21,
			delta1, SHU_PLL21_RG_RCLRPLL_SDM_SSC_DELTA1);
		io_32_write_fld_align_all(DDRPHY_SHU_PLL21,
			delta, SHU_PLL21_RG_RCLRPLL_SDM_SSC_DELTA);

		io_32_write_fld_align_all(DDRPHY_SHU_PLL14,
			prd, SHU_PLL14_RG_RPHYPLL_SDM_SSC_PRD);
		io_32_write_fld_align_all(DDRPHY_SHU_PLL14,
			dir, SHU_PLL14_RG_RPHYPLL_SDM_SSC_PH_INIT); /* ssc direction*/
		io_32_write_fld_align_all(DDRPHY_SHU_PLL15,
			delta1, SHU_PLL15_RG_RPHYPLL_SDM_SSC_DELTA1);
		io_32_write_fld_align_all(DDRPHY_SHU_PLL15,
			delta, SHU_PLL15_RG_RPHYPLL_SDM_SSC_DELTA);

		io_32_write_fld_align_all(DDRPHY_SHU_PLL1, CLEAR_FLD,
			SHU_PLL1_RG_RPHYPLLGP_CK_SEL);

		show_msg2((INFO, "[apply_ssc_setting]\n"
			"Percent %d, Slope %d kHz, Dir %d (Down)\n"
			"PRD = (%d), Delta (%d), Delta1 (%d)\n",
			percent, slope, dir,
			prd, delta, delta1));
	}
}

/*----------
 * Note that since B1 is used as CA pad for NON-LP4 type,
 * thus B1's setting shall be the same as CA
 */
static void ssc_enable(DRAMC_CTX_T *p)
{
	if (p->ssc_en == ENABLE) {
		show_msg2((INFO, "Enable SSC\n"));

		io_32_write_fld_align_all(DDRPHY_PLL1, 1,
			PLL1_RG_RPHYPLL_SDM_SSC_EN);

		io_32_write_fld_align_all(DDRPHY_PLL2, 1,
			PLL2_RG_RCLRPLL_SDM_SSC_EN);
	}

	return DRAM_OK;
}

void ddrphy_pll_setting(DRAMC_CTX_T *p)
{
	unsigned char cap_sel;
	unsigned char midpicap_sel;
	unsigned char vth_sel;
	unsigned short sdm_pcw = 0;
	unsigned char ca_dll_mode[2];
	unsigned char channel = CHANNEL_A;
	unsigned char brpi_mctl_en_ca = 0;
#if ENABLE_TMRRI_NEW_MODE
	unsigned char rankidx;
#endif

	vth_sel = 0x2;
	if (p->frequency <= 933)
		cap_sel = 0x3;
	else
		cap_sel = 0x0;


	if (p->frequency <= 933)
		midpicap_sel = 0x2;
	else if (p->frequency <= 1200)
		midpicap_sel = 0x3;
	else if (p->frequency <= 1333)
		midpicap_sel = 0x0;
	else if (p->frequency <= 1600)
		midpicap_sel = 0x3;
	else if (p->frequency <= 1866)
		midpicap_sel = 0x1;
	else
		midpicap_sel = 0x0;

	io_32_write_fld_align_all(DDRPHY_SHU_PLL4, 0x2000,
		SHU_PLL4_RG_RPHYPLL_RESERVED);
	io_32_write_fld_align_all(DDRPHY_SHU_PLL6, 0x2000,
		SHU_PLL6_RG_RCLRPLL_RESERVED);

	if (is_lp4_family(p)) {
	#if DLL_ASYNC_MODE
		brpi_mctl_en_ca = 1;
		ca_dll_mode[CHANNEL_A] = ca_dll_mode[CHANNEL_B] = DLL_MASTER;
		io_32_write_fld_align(DDRPHY_MISC_SHU_OPT + ((U32)CHANNEL_A << POS_BANK_NUM),
			1, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN);
		io_32_write_fld_align(DDRPHY_MISC_SHU_OPT + SHIFT_TO_CHB_ADDR, 1,
			MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN);
		io_32_write_fld_multi(DDRPHY_CKMUX_SEL + ((U32)CHANNEL_A << POS_BANK_NUM),
			p_fld(0, CKMUX_SEL_FMEM_CK_MUX) |
			p_fld(0, CKMUX_SEL_FB_CK_MUX));
		io_32_write_fld_multi(DDRPHY_CKMUX_SEL + SHIFT_TO_CHB_ADDR,
			p_fld(2, CKMUX_SEL_FMEM_CK_MUX) |
			p_fld(2, CKMUX_SEL_FB_CK_MUX));
		io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD0, CLEAR_FLD,
			SHU_CA_CMD0_RG_FB_CK_MUX);
	#else
		ca_dll_mode[CHANNEL_A] = DLL_MASTER;
		ca_dll_mode[CHANNEL_B] = DLL_SLAVE;
		io_32_write_fld_align(DDRPHY_MISC_SHU_OPT + ((U32)CHANNEL_A << POS_BANK_NUM),
			1, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN);
		io_32_write_fld_align(DDRPHY_MISC_SHU_OPT + SHIFT_TO_CHB_ADDR, 2,
			MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN);
		io_32_write_fld_multi(DDRPHY_CKMUX_SEL + ((U32)CHANNEL_A << POS_BANK_NUM),
			p_fld(1, CKMUX_SEL_FMEM_CK_MUX) |
			p_fld(1, CKMUX_SEL_FB_CK_MUX));
		io_32_write_fld_multi(DDRPHY_CKMUX_SEL + SHIFT_TO_CHB_ADDR,
			p_fld(1, CKMUX_SEL_FMEM_CK_MUX) |
			p_fld(1, CKMUX_SEL_FB_CK_MUX));
		io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD0, CLEAR_FLD,
			SHU_CA_CMD0_RG_FB_CK_MUX);
	#endif
	} else {
		ca_dll_mode[CHANNEL_A] = DLL_MASTER;
		ca_dll_mode[CHANNEL_B] = DLL_SLAVE;
		io_32_write_fld_align(DDRPHY_MISC_SHU_OPT + ((U32)CHANNEL_A << POS_BANK_NUM),
			1, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN);
		io_32_write_fld_align(DDRPHY_MISC_SHU_OPT + SHIFT_TO_CHB_ADDR, 2,
			MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN);
		io_32_write_fld_multi(DDRPHY_CKMUX_SEL + ((U32)CHANNEL_A << POS_BANK_NUM),
			p_fld(1, CKMUX_SEL_FMEM_CK_MUX) |
			p_fld(1, CKMUX_SEL_FB_CK_MUX));
		io_32_write_fld_multi(DDRPHY_CKMUX_SEL + SHIFT_TO_CHB_ADDR,
			p_fld(1, CKMUX_SEL_FMEM_CK_MUX) |
			p_fld(1, CKMUX_SEL_FB_CK_MUX));
		io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD0, SET_FLD,
			SHU_CA_CMD0_RG_FB_CK_MUX);
	}

#if ENABLE_DLL_ALL_SLAVE_MODE
	if (p->frequency <= DDR1866_FREQ)
		ca_dll_mode[CHANNEL_A] = ca_dll_mode[CHANNEL_B] = DLL_SLAVE;
#endif

	if (ca_dll_mode[CHANNEL_A] == DLL_SLAVE)
		io_32_write_fld_align_all(DRAMC_REG_SHU_DVFSCTL, 1,
			SHU_DVFSCTL_R_BYPASS_1ST_DLL);
	else
		io_32_write_fld_align_all(DRAMC_REG_SHU_DVFSCTL, 0,
			SHU_DVFSCTL_R_BYPASS_1ST_DLL);

	for (channel = CHANNEL_A; channel <= CHANNEL_B; channel++) {
		if (ca_dll_mode[channel] == DLL_MASTER) {
			io_32_write_fld_multi(DDRPHY_SHU_CA_DLL0 + ((U32)channel << POS_BANK_NUM),
				p_fld(CLEAR_FLD, SHU_CA_DLL0_RG_ARDLL_PHDET_OUT_SEL_CA) |
				p_fld(CLEAR_FLD, SHU_CA_DLL0_RG_ARDLL_PHDET_IN_SWAP_CA) |
				p_fld(0x6, SHU_CA_DLL0_RG_ARDLL_GAIN_CA) |
				p_fld(0x9, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA) |
				p_fld(0x8, SHU_CA_DLL0_RG_ARDLL_P_GAIN_CA) |
				p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHJUMP_EN_CA) |
				p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDIV_CA) |
				p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_FAST_PSJP_CA));
			io_32_write_fld_multi(DDRPHY_SHU_CA_DLL1 + ((U32)channel << POS_BANK_NUM),
				p_fld(SET_FLD, SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA) |
				p_fld(CLEAR_FLD, SHU_CA_DLL1_RG_ARDLL_FASTPJ_CK_SEL_CA));
			io_32_write_fld_align(DDRPHY_SHU_CA_CMD6 + ((U32)channel << POS_BANK_NUM),
				1, RG_ARPI_RESERVE_BIT_01_DLL_FAST_PSJP);
		} else {
			io_32_write_fld_multi(DDRPHY_SHU_CA_DLL0 + ((U32)channel << POS_BANK_NUM),
				p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDET_OUT_SEL_CA) |
				p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDET_IN_SWAP_CA) |
				p_fld(0x7, SHU_CA_DLL0_RG_ARDLL_GAIN_CA) |
				p_fld(0x7, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA) |
				p_fld(0x8, SHU_CA_DLL0_RG_ARDLL_P_GAIN_CA) |
				p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHJUMP_EN_CA) |
				p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDIV_CA) |
				p_fld(CLEAR_FLD, SHU_CA_DLL0_RG_ARDLL_FAST_PSJP_CA));
			io_32_write_fld_multi(DDRPHY_SHU_CA_DLL1 + ((U32)channel << POS_BANK_NUM),
				p_fld(CLEAR_FLD, SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA) |
				p_fld(SET_FLD, SHU_CA_DLL1_RG_ARDLL_FASTPJ_CK_SEL_CA));
			io_32_write_fld_align(DDRPHY_SHU_CA_CMD6 + ((U32)channel << POS_BANK_NUM),
				0, RG_ARPI_RESERVE_BIT_01_DLL_FAST_PSJP);
		}
	}

	unsigned int regbackupaddress[] = {
		(DDRPHY_B0_DQ7),
		(DDRPHY_B1_DQ7),
		(DDRPHY_B2_DQ7),
		(DDRPHY_CA_CMD7),
		(DDRPHY_B0_DQ7 + SHIFT_TO_CHB_ADDR),
		(DDRPHY_B1_DQ7 + SHIFT_TO_CHB_ADDR),
		(DDRPHY_B2_DQ7 + SHIFT_TO_CHB_ADDR),
		(DDRPHY_CA_CMD7 + SHIFT_TO_CHB_ADDR),
	};

	dramc_backup_registers(p, regbackupaddress,
		sizeof (regbackupaddress) / sizeof (U32));
	io_32_write_fld_multi_all(DDRPHY_B0_DQ7,
		p_fld(SET_FLD, B0_DQ7_RG_TX_ARDQ_PULL_DN_B0) |
		p_fld(SET_FLD, B0_DQ7_RG_TX_ARDQM0_PULL_DN_B0) |
		p_fld(SET_FLD, B0_DQ7_RG_TX_ARDQS0_PULL_DN_B0) |
		p_fld(SET_FLD, B0_DQ7_RG_TX_ARDQS0B_PULL_DN_B0));
	io_32_write_fld_multi_all(DDRPHY_B2_DQ7,
		p_fld(SET_FLD, B2_DQ7_RG_TX_ARDQ_PULL_DN_B2) |
		p_fld(SET_FLD, B2_DQ7_RG_TX_ARDQM0_PULL_DN_B2) |
		p_fld(SET_FLD, B2_DQ7_RG_TX_ARDQS0_PULL_DN_B2) |
		p_fld(SET_FLD, B2_DQ7_RG_TX_ARDQS0B_PULL_DN_B2));
	io_32_write_fld_multi_all(DDRPHY_CA_CMD7,
		p_fld(SET_FLD, CA_CMD7_RG_TX_ARCMD_PULL_DN) |
		p_fld(SET_FLD, CA_CMD7_RG_TX_ARCS_PULL_DN) |
		p_fld(SET_FLD, CA_CMD7_RG_TX_ARCLK_PULL_DN) |
		p_fld(SET_FLD, CA_CMD7_RG_TX_ARCLKB_PULL_DN));
	io_32_write_fld_multi_all(DDRPHY_B1_DQ7,
		p_fld(SET_FLD, B1_DQ7_RG_TX_ARDQ_PULL_DN_B1) |
		p_fld(SET_FLD, B1_DQ7_RG_TX_ARDQM0_PULL_DN_B1) |
		p_fld(SET_FLD, B1_DQ7_RG_TX_ARDQS0_PULL_DN_B1) |
		p_fld(SET_FLD, B1_DQ7_RG_TX_ARDQS0B_PULL_DN_B1));

	/*
	 * DMSUS replaced by CA_CMD2_RG_TX_ARCMD_OE_DIS, CMD_OE_DIS(1) will prevent illegal command ouput
	 * And DRAM 1st reset_n pulse will disappear if use CA_CMD2_RG_TX_ARCMD_OE_DIS
	 */
	io_32_write_fld_align_all(DDRPHY_CA_CMD3, SET_FLD,
		CA_CMD3_RG_TX_ARCMD_OE_DIS);

	io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL0, CLEAR_FLD,
		MISC_CG_CTRL0_CLK_MEM_SEL);

#ifdef USE_CLK26M
	io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL0, SET_FLD,
		MISC_CG_CTRL0_RG_DA_RREF_CK_SEL);
#endif

	io_32_write_fld_multi_all(DDRPHY_SHU_B0_DQ6,
		p_fld(CLEAR_FLD, SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0));
	io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ6,
		p_fld(CLEAR_FLD, SHU_B2_DQ6_RG_ARPI_MIDPI_EN_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B2));
	io_32_write_fld_multi_all(DDRPHY_SHU_CA_CMD6,
		p_fld(CLEAR_FLD, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA) |
		p_fld(CLEAR_FLD, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA));
	io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ6,
		p_fld(CLEAR_FLD, SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1));

	io_32_write_fld_multi_all(DDRPHY_PLL4,
		p_fld(CLEAR_FLD, PLL4_RG_RPHYPLL_ADA_MCK8X_EN) |
		p_fld(CLEAR_FLD, PLL4_RG_RPHYPLL_RESETB));

	io_32_write_fld_align_all(DDRPHY_PLL1, CLEAR_FLD,
		PLL1_RG_RPHYPLL_EN);
	io_32_write_fld_align_all(DDRPHY_PLL2, CLEAR_FLD,
		PLL2_RG_RCLRPLL_EN);

	io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI2, CLEAR_FLD,
		CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI2, CLEAR_FLD,
		B1_DLL_ARPI2_RG_ARDLL_PHDET_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI2, CLEAR_FLD,
		B0_DLL_ARPI2_RG_ARDLL_PHDET_EN_B0);
	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI2, CLEAR_FLD,
		B2_DLL_ARPI2_RG_ARDLL_PHDET_EN_B2);

	io_32_write_fld_multi_all(DDRPHY_B0_DLL_ARPI2,
		p_fld(SET_FLD, B0_DLL_ARPI2_RG_ARPI_CG_MCK_B0) |
		p_fld(SET_FLD, B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0) |
		p_fld(SET_FLD, B0_DLL_ARPI2_RG_ARPI_CG_MCTL_B0) |
		p_fld(SET_FLD, B0_DLL_ARPI2_RG_ARPI_CG_FB_B0) |
		p_fld(SET_FLD, B0_DLL_ARPI2_RG_ARPI_CG_DQS_B0) |
		p_fld(SET_FLD, B0_DLL_ARPI2_RG_ARPI_CG_DQM_B0) |
		p_fld(SET_FLD, B0_DLL_ARPI2_RG_ARPI_CG_DQ_B0) |
		p_fld(SET_FLD, B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0) |
		p_fld(SET_FLD, B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
	io_32_write_fld_multi_all(DDRPHY_B1_DLL_ARPI2,
		p_fld(SET_FLD, B1_DLL_ARPI2_RG_ARPI_CG_MCK_B1) |
		p_fld(SET_FLD, B1_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B1) |
		p_fld(SET_FLD, B1_DLL_ARPI2_RG_ARPI_CG_MCTL_B1) |
		p_fld(SET_FLD, B1_DLL_ARPI2_RG_ARPI_CG_FB_B1) |
		p_fld(SET_FLD, B1_DLL_ARPI2_RG_ARPI_CG_DQS_B1) |
		p_fld(SET_FLD, B1_DLL_ARPI2_RG_ARPI_CG_DQM_B1) |
		p_fld(SET_FLD, B1_DLL_ARPI2_RG_ARPI_CG_DQ_B1) |
		p_fld(SET_FLD, B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1) |
		p_fld(SET_FLD, B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));
	io_32_write_fld_multi_all(DDRPHY_B2_DLL_ARPI2,
		p_fld(SET_FLD, B2_DLL_ARPI2_RG_ARPI_CG_MCK_B2) |
		p_fld(SET_FLD, B2_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B2) |
		p_fld(SET_FLD, B2_DLL_ARPI2_RG_ARPI_CG_MCTL_B2) |
		p_fld(SET_FLD, B2_DLL_ARPI2_RG_ARPI_CG_FB_B2) |
		p_fld(SET_FLD, B2_DLL_ARPI2_RG_ARPI_CG_DQS_B2) |
		p_fld(SET_FLD, B2_DLL_ARPI2_RG_ARPI_CG_DQM_B2) |
		p_fld(SET_FLD, B2_DLL_ARPI2_RG_ARPI_CG_DQ_B2) |
		p_fld(SET_FLD, B2_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B2) |
		p_fld(SET_FLD, B2_DLL_ARPI2_RG_ARPI_MPDIV_CG_B2));
	io_32_write_fld_multi_all(DDRPHY_CA_DLL_ARPI2,
		p_fld(SET_FLD, CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA) |
		p_fld(SET_FLD, CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA) |
		p_fld(SET_FLD, CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA) |
		p_fld(SET_FLD, CA_DLL_ARPI2_RG_ARPI_CG_FB_CA) |
		p_fld(SET_FLD, CA_DLL_ARPI2_RG_ARPI_CG_CS) |
		p_fld(SET_FLD, CA_DLL_ARPI2_RG_ARPI_CG_CLK) |
		p_fld(SET_FLD, CA_DLL_ARPI2_RG_ARPI_CG_CMD) |
		p_fld(SET_FLD, CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN) |
		p_fld(SET_FLD, CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));

	io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI0, CLEAR_FLD,
		CA_DLL_ARPI0_RG_ARPI_RESETB_CA);
	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI0, CLEAR_FLD,
		B0_DLL_ARPI0_RG_ARPI_RESETB_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI0, CLEAR_FLD,
		B1_DLL_ARPI0_RG_ARPI_RESETB_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI0, CLEAR_FLD,
		B2_DLL_ARPI0_RG_ARPI_RESETB_B2);

	delay_us(1);

	/*
	 * TODO: PLL/MIDPI Settings
	 * Ref clock should be 20M~30M, if MPLL=52M, Pre-divider should be set to 1
	 */
#ifdef USE_CLK26M
	io_32_write_fld_multi_all(DDRPHY_SHU_PLL8,
		p_fld(CLEAR_FLD, SHU_PLL8_RG_RPHYPLL_POSDIV) |
		p_fld(CLEAR_FLD, SHU_PLL8_RG_RPHYPLL_PREDIV));
	io_32_write_fld_multi_all(DDRPHY_SHU_PLL10,
		p_fld(CLEAR_FLD, SHU_PLL10_RG_RCLRPLL_POSDIV) |
		p_fld(CLEAR_FLD, SHU_PLL10_RG_RCLRPLL_PREDIV));
#else
	io_32_write_fld_multi_all(DDRPHY_SHU_PLL8,
		p_fld(CLEAR_FLD, SHU_PLL8_RG_RPHYPLL_POSDIV) |
		p_fld(SET_FLD, SHU_PLL8_RG_RPHYPLL_PREDIV));
	io_32_write_fld_multi_all(DDRPHY_SHU_PLL10,
		p_fld(CLEAR_FLD, SHU_PLL10_RG_RCLRPLL_POSDIV) |
		p_fld(SET_FLD, SHU_PLL10_RG_RCLRPLL_PREDIV));
#endif
	if (p->frequency == DDR3733_FREQ) {
	#ifdef EN_DDR3733
		sdm_pcw = 0x8f00;
	#else
		sdm_pcw = 0x8a00;
	#endif
	} else if (p->frequency == DDR3200_FREQ || p->frequency == DDR1600_FREQ) {
		sdm_pcw = 0x7a00;
	} else if (p->frequency == DDR2666_FREQ || p->frequency == DDR1333_FREQ) {
		sdm_pcw = 0x6600;
	} else if (p->frequency == DDR2400_FREQ || p->frequency == DDR1200_FREQ) {
		sdm_pcw = 0x5c00;
	} else if (p->frequency == DDR2280_FREQ) {
		sdm_pcw = 0x5700;
	}
#if SUPPORT_TYPE_LPDDR3
	else if (p->frequency == DDR1866_FREQ)
		sdm_pcw = 0x8f00;
	else if (p->frequency == DDR1200_FREQ)
		sdm_pcw = 0x5c00;
	else if (p->frequency == DDR933_FREQ)
		sdm_pcw = 0x4700;
#endif

	/*
	 * SDM_PCW: Feedback divide ratio (8-bit integer + 8-bit fraction)
	 * PLL_SDM_FRA_EN: SDMPLL fractional mode enable (0:Integer mode, 1:Fractional mode)
	 */
	io_32_write_fld_multi_all(DDRPHY_SHU_PLL5,
		p_fld(sdm_pcw, SHU_PLL5_RG_RPHYPLL_SDM_PCW) |
		p_fld(SET_FLD, SHU_PLL5_RG_RPHYPLL_SDM_FRA_EN));
	io_32_write_fld_multi_all(DDRPHY_SHU_PLL7,
		p_fld(sdm_pcw, SHU_PLL7_RG_RCLRPLL_SDM_PCW) |
		p_fld(SET_FLD, SHU_PLL7_RG_RCLRPLL_SDM_FRA_EN));

	if (p->ssc_en == ENABLE)
		apply_ssc_setting(p, 2, 30, 1);

	io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI0, CLEAR_FLD,
		CA_DLL_ARPI0_RG_ARMCTLPLL_CK_SEL_CA);
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI0, CLEAR_FLD,
		B1_DLL_ARPI0_RG_ARMCTLPLL_CK_SEL_B1);
	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI0, CLEAR_FLD,
		B0_DLL_ARPI0_RG_ARMCTLPLL_CK_SEL_B0);
	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI0, CLEAR_FLD,
		B2_DLL_ARPI0_RG_ARMCTLPLL_CK_SEL_B2);

	io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI1, CLEAR_FLD,
		CA_DLL_ARPI1_RG_ARPI_CLKIEN_JUMP_EN);
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI1, CLEAR_FLD,
		B1_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI1, CLEAR_FLD,
		B0_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B0);
	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI1, CLEAR_FLD,
		B2_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B2);

	io_32_write_fld_multi_all(DDRPHY_SHU_B0_DQ6,
		p_fld(vth_sel, SHU_B0_DQ6_RG_ARPI_MIDPI_VTH_SEL_B0) |
		p_fld(cap_sel, SHU_B0_DQ6_RG_ARPI_CAP_SEL_B0) |
		p_fld(midpicap_sel, SHU_B0_DQ6_RG_ARPI_MIDPI_CAP_SEL_B0));
	io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ6,
		p_fld(vth_sel, SHU_B2_DQ6_RG_ARPI_MIDPI_VTH_SEL_B2) |
		p_fld(cap_sel, SHU_B2_DQ6_RG_ARPI_CAP_SEL_B2) |
		p_fld(midpicap_sel, SHU_B2_DQ6_RG_ARPI_MIDPI_CAP_SEL_B2));
	io_32_write_fld_multi_all(DDRPHY_SHU_CA_CMD6,
		p_fld(vth_sel, SHU_CA_CMD6_RG_ARPI_MIDPI_VTH_SEL_CA) |
		p_fld(cap_sel, SHU_CA_CMD6_RG_ARPI_CAP_SEL_CA) |
		p_fld(midpicap_sel, SHU_CA_CMD6_RG_ARPI_MIDPI_CAP_SEL_CA));
	io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ6,
		p_fld(vth_sel, SHU_B1_DQ6_RG_ARPI_MIDPI_VTH_SEL_B1) |
		p_fld(cap_sel, SHU_B1_DQ6_RG_ARPI_CAP_SEL_B1) |
		p_fld(midpicap_sel, SHU_B1_DQ6_RG_ARPI_MIDPI_CAP_SEL_B1));

	io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI0, SET_FLD,
		CA_DLL_ARPI0_RG_ARPI_RESETB_CA);
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI0, SET_FLD,
		B1_DLL_ARPI0_RG_ARPI_RESETB_B1);
	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI0, SET_FLD,
		B0_DLL_ARPI0_RG_ARPI_RESETB_B0);
	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI0, SET_FLD,
		B2_DLL_ARPI0_RG_ARPI_RESETB_B2);
	delay_us(1);

	io_32_write_fld_align_all(DDRPHY_PLL1, SET_FLD, PLL1_RG_RPHYPLL_EN);
	io_32_write_fld_align_all(DDRPHY_PLL2, SET_FLD, PLL2_RG_RCLRPLL_EN);
	delay_us(100);

	io_32_write_fld_multi_all(DDRPHY_PLL4,
		p_fld(SET_FLD, PLL4_RG_RPHYPLL_ADA_MCK8X_EN) |
		p_fld(SET_FLD, PLL4_RG_RPHYPLL_RESETB));

	delay_us(1);

	/*
	* [QW]: as CF request, close B1 power consume
	*/
	if(is_lp4_family(p)) {
			io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI0, SET_FLD, B1_DLL_ARPI0_RG_ARPI_ISO_EN_B1);
			io_32_write_fld_align_all(DDRPHY_MISC_SPM_CTRL1, SET_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B1);
	}

	/*
	 * MIDPI Settings (Olymxxx): DA_*RPI_MIDPI_EN, DA_*RPI_MIDPI_CKDIV4_EN
	 * Justin suggests use frequency > 933 as boundary
	 */
	if (p->frequency > DDR1866_FREQ) {
		io_32_write_fld_multi_all(DDRPHY_SHU_B0_DQ6,
			p_fld(SET_FLD, SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0) |
			p_fld(CLEAR_FLD, SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0));
		io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ6,
			p_fld(SET_FLD, SHU_B2_DQ6_RG_ARPI_MIDPI_EN_B2) |
			p_fld(CLEAR_FLD, SHU_B2_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B2));
		io_32_write_fld_multi_all(DDRPHY_SHU_CA_CMD6,
			p_fld(SET_FLD, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA) |
			p_fld(CLEAR_FLD, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA));
		io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ6,
			p_fld(SET_FLD, SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1) |
			p_fld(CLEAR_FLD, SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1));
	} else {
		io_32_write_fld_multi_all(DDRPHY_SHU_B0_DQ6,
			p_fld(CLEAR_FLD, SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0) |
			p_fld(SET_FLD, SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0));
		io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ6,
			p_fld(CLEAR_FLD, SHU_B2_DQ6_RG_ARPI_MIDPI_EN_B2) |
			p_fld(SET_FLD, SHU_B2_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B2));
		io_32_write_fld_multi_all(DDRPHY_SHU_CA_CMD6,
			p_fld(CLEAR_FLD, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA) |
			p_fld(SET_FLD, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA));
		io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ6,
			p_fld(CLEAR_FLD, SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1) |
			p_fld(SET_FLD, SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1));
	}
	delay_us(1);

	if (is_lp4_family(p)) {
		io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI3,
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_FB_EN_CA) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_CS_EN) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_CLK_EN) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_CMD_EN));
		io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI3 + SHIFT_TO_CHB_ADDR,
			p_fld(brpi_mctl_en_ca, CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_FB_EN_CA) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_CS_EN) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_CLK_EN) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_CMD_EN));
		io_32_write_fld_multi_all(DDRPHY_B0_DLL_ARPI3,
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_FB_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_MCTL_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_DQS_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_DQM_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_DQ_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B0));
		io_32_write_fld_multi_all(DDRPHY_B1_DLL_ARPI3,
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_FB_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_MCTL_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_DQS_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_DQM_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_DQ_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B1));
		io_32_write_fld_multi_all(DDRPHY_B2_DLL_ARPI3,
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_FB_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_MCTL_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_DQS_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_DQM_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_DQ_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B2));
	} else {
		io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI3,
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_FB_EN_CA) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_CS_EN) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_CLK_EN) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_CMD_EN) |
			p_fld(CLEAR_FLD, CA_DLL_ARPI3_RG_ARPI_CLKIEN_EN));
		io_32_write_fld_multi(DDRPHY_B0_DLL_ARPI3,
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_MCTL_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_FB_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_DQS_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_DQM_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_DQ_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B0));
		io_32_write_fld_multi(DDRPHY_B1_DLL_ARPI3,
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_MCTL_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_FB_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_DQS_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_DQM_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_DQ_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B1));
		io_32_write_fld_multi(DDRPHY_B2_DLL_ARPI3,
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_MCTL_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_FB_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_DQS_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_DQM_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_DQ_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B2));
		io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI3 + SHIFT_TO_CHB_ADDR,
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_FB_EN_CA) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_CS_EN) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_CLK_EN) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_CMD_EN) |
			p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_CLKIEN_EN));
		io_32_write_fld_multi(DDRPHY_B0_DLL_ARPI3 + SHIFT_TO_CHB_ADDR,
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_MCTL_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_FB_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_DQS_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_DQM_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_DQ_EN_B0) |
			p_fld(SET_FLD, B0_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B0));
		io_32_write_fld_multi(DDRPHY_B1_DLL_ARPI3 + SHIFT_TO_CHB_ADDR,
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_MCTL_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_FB_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_DQS_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_DQM_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_DQ_EN_B1) |
			p_fld(SET_FLD, B1_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B1));
		io_32_write_fld_multi(DDRPHY_B2_DLL_ARPI3 + SHIFT_TO_CHB_ADDR,
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_MCTL_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_FB_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_DQS_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_DQM_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_DQ_EN_B2) |
			p_fld(SET_FLD, B2_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B2));
	}

	if (p->dram_type == TYPE_PCDDR3) {
		io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI3, SET_FLD,
			CA_DLL_ARPI3_RG_ARPI_CLKIEN_EN);
	}

	io_32_write_fld_multi_all(DDRPHY_CA_DLL_ARPI2,
		p_fld(CLEAR_FLD, CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI2_RG_ARPI_CG_FB_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI2_RG_ARPI_CG_CS) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI2_RG_ARPI_CG_CLK) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI2_RG_ARPI_CG_CMD) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));
	io_32_write_fld_multi_all(DDRPHY_B0_DLL_ARPI2,
		p_fld(CLEAR_FLD, B0_DLL_ARPI2_RG_ARPI_CG_MCK_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI2_RG_ARPI_CG_MCTL_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI2_RG_ARPI_CG_FB_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI2_RG_ARPI_CG_DQS_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI2_RG_ARPI_CG_DQM_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI2_RG_ARPI_CG_DQ_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
	io_32_write_fld_multi_all(DDRPHY_B1_DLL_ARPI2,
		p_fld(CLEAR_FLD, B1_DLL_ARPI2_RG_ARPI_CG_MCK_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI2_RG_ARPI_CG_MCTL_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI2_RG_ARPI_CG_FB_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI2_RG_ARPI_CG_DQS_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI2_RG_ARPI_CG_DQM_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI2_RG_ARPI_CG_DQ_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));
	io_32_write_fld_multi_all(DDRPHY_B2_DLL_ARPI2,
		p_fld(CLEAR_FLD, B2_DLL_ARPI2_RG_ARPI_CG_MCK_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI2_RG_ARPI_CG_MCTL_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI2_RG_ARPI_CG_FB_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI2_RG_ARPI_CG_DQS_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI2_RG_ARPI_CG_DQM_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI2_RG_ARPI_CG_DQ_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI2_RG_ARPI_MPDIV_CG_B2));


	if (is_lp4_family(p)) {
		io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI2, 1,
			CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN);
		io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI2, 0,
			B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0);
		io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI2, 1, /* cc note, B1 as CA?? */
			B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1);
		io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI2, 0,
			B2_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B2);
	} else {
		io_32_write_fld_align(DDRPHY_CA_DLL_ARPI2, 1,
			CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN);
		io_32_write_fld_align(DDRPHY_B0_DLL_ARPI2, 0,
			B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0);
		io_32_write_fld_align(DDRPHY_B1_DLL_ARPI2, 1,
			B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1);
		io_32_write_fld_align(DDRPHY_B2_DLL_ARPI2, 0,
			B2_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B2);
		io_32_write_fld_align(DDRPHY_CA_DLL_ARPI2 + SHIFT_TO_CHB_ADDR, 0,
			CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN);
		io_32_write_fld_align(DDRPHY_B0_DLL_ARPI2 + SHIFT_TO_CHB_ADDR, 0,
			B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0);
		io_32_write_fld_align(DDRPHY_B1_DLL_ARPI2 + SHIFT_TO_CHB_ADDR, 0,
			B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1);
		io_32_write_fld_align(DDRPHY_B2_DLL_ARPI2 + SHIFT_TO_CHB_ADDR, 0,
			B2_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B2);
	}

	delay_us(2);

	io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL0, SET_FLD,
		MISC_CG_CTRL0_CLK_MEM_SEL);
	delay_us(1);

	io_32_write_fld_align(DDRPHY_CA_DLL_ARPI2, SET_FLD,
		CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);
	delay_us(1);
	io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI2, SET_FLD,
		CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);
	delay_us(1);
	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI2, SET_FLD,
		B0_DLL_ARPI2_RG_ARDLL_PHDET_EN_B0);
	delay_us(1);
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI2, SET_FLD,
		B1_DLL_ARPI2_RG_ARDLL_PHDET_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI2, SET_FLD,
		B2_DLL_ARPI2_RG_ARDLL_PHDET_EN_B2);
	delay_us(1);

	{
	#if 0
		show_msg2((INFO, "DDRPhyPLLSetting - DMSUS\n\n"));
		io_32_write_fld_multi_all(DDRPHY_MISC_SPM_CTRL1,
			p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10) |
			p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B0) |
			p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B1) |
			p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_CA));
		io_32_write_fld_align_all(DDRPHY_MISC_SPM_CTRL0, 0xffffffff,
			MISC_SPM_CTRL0_PHY_SPM_CTL0);
		io_32_write_fld_align_all(DDRPHY_MISC_SPM_CTRL2, 0xffffffff,
			MISC_SPM_CTRL2_PHY_SPM_CTL2);
	#else
		/*
		 * DMSUS replaced by CA_CMD2_RG_TX_ARCMD_OE_DIS, CMD_OE_DIS(1) will prevent illegal command ouput
		 * And DRAM 1st reset_n pulse will disappear if use CA_CMD2_RG_TX_ARCMD_OE_DIS
		 */
		io_32_write_fld_align_all(DDRPHY_CA_CMD3, CLEAR_FLD,
			CA_CMD3_RG_TX_ARCMD_OE_DIS);
		dramc_restore_registers(p, regbackupaddress,
			sizeof (regbackupaddress) / sizeof (unsigned int));
	#endif

		show_msg2((INFO, "DDRPhyPLLSetting - CKEON\n\n"));

		cke_fix_on_off(p, CKE_WRITE_TO_ALL_RANK, CKE_DYNAMIC, CKE_WRITE_TO_ALL_CHANNEL);
	}
	if (p->ssc_en == ENABLE)
		ssc_enable(p);
	//cc mark DDRPhyFreqMeter();
}

void ddrphy_rsv_rg_setting(DRAMC_CTX_T *p)
{
	unsigned char hyst_sel = 0;
	unsigned char midpi_cap_sel2 = 0;
	unsigned char lp3_sel = 0;
	unsigned char ser_rst_mode = 1;
	unsigned char tx_read_base_en = 1;
	unsigned char arpi_bit4to10 = 0;
	unsigned char psmux_drv_sel = 0;
	unsigned char bypass_sr = 1;
	unsigned int reserve_ca_cha = 0;
	unsigned int reserve_ca_chb = 0;
#if TX_DQ_PRE_EMPHASIS
	unsigned char drving = 0;
#endif

	if (p->frequency <= 1333)
		hyst_sel = 1;

	if (is_lp4_family(p) && p->frequency < 1333)
		midpi_cap_sel2 = 1;
	else
		midpi_cap_sel2 = 0;

	if (is_lp4_family(p) && p->frequency >= 1200)
		psmux_drv_sel = 1;

	if (p->dram_type == TYPE_LPDDR3)
		lp3_sel = 1;

	if (p->frequency <= 933)
		arpi_bit4to10 = 1;

	if (p->dram_type == TYPE_LPDDR3 || p->dram_type == TYPE_PCDDR3)
	{
		tx_read_base_en = 0;
	#if DramcHWDQSGatingTracking_DVT_JADE_TRACKING_MODE
		ser_rst_mode = 0;
	#endif
	}

	if ((p->dram_type == TYPE_LPDDR3) && (p->frequency <= 800))
	{
		bypass_sr = 0;
	}
	else
	{
		bypass_sr = 1;
	}

	io_32_write_fld_align_all(DDRPHY_SHU_PLL4, 1,
		RG_PLL_RESERVE_BIT_13_PLL_FS_EN);
	io_32_write_fld_align_all(DDRPHY_SHU_PLL6, 1,
		RG_PLL_RESERVE_BIT_13_PLL_FS_EN);

	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD6,
		p_fld(SET_FLD, RG_ARPI_RESERVE_BIT_00_TX_CG_EN) |
		p_fld(1, RG_ARPI_RESERVE_BIT_01_DLL_FAST_PSJP) |
		p_fld(hyst_sel, RG_ARPI_RESERVE_BIT_02_HYST_SEL) |
		p_fld(midpi_cap_sel2, RG_ARPI_RESERVE_BIT_03_MIDPI_CAP_SEL) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_04_8PHASE_XLATCH_FORCE) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_05_PSMUX_XLATCH_FORCE) |
		p_fld(arpi_bit4to10,
		RG_ARPI_RESERVE_BIT_06_PSMUX_XLATCH_FORCEDQS) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_07_SMT_XLATCH_FORCE) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_08_SMT_XLATCH_FORCE_DQS) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_09_BUFGP_XLATCH_FORCE) |
		p_fld(arpi_bit4to10,
		RG_ARPI_RESERVE_BIT_10_BUFGP_XLATCH_FORCE_DQS) |
		p_fld(bypass_sr, RG_ARPI_RESERVE_BIT_11_BYPASS_SR) |
		p_fld(bypass_sr, RG_ARPI_RESERVE_BIT_12_BYPASS_SR_DQS) |
		p_fld(0, RG_ARPI_RESERVE_BIT_13_CG_SYNC_ENB) |
		p_fld(lp3_sel, RG_ARPI_RESERVE_BIT_14_LP3_SEL) |
		p_fld(psmux_drv_sel, RG_ARPI_RESERVE_BIT_15_PSMUX_DRV_SEL));

	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, RG_ARPI_RESERVE_BIT_00_TX_CG_EN) |
		p_fld(0, RG_ARPI_RESERVE_BIT_01_DLL_FAST_PSJP) |
		p_fld(hyst_sel, RG_ARPI_RESERVE_BIT_02_HYST_SEL) |
		p_fld(midpi_cap_sel2, RG_ARPI_RESERVE_BIT_03_MIDPI_CAP_SEL) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_04_8PHASE_XLATCH_FORCE) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_05_PSMUX_XLATCH_FORCE) |
		p_fld(arpi_bit4to10,
		RG_ARPI_RESERVE_BIT_06_PSMUX_XLATCH_FORCEDQS) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_07_SMT_XLATCH_FORCE) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_08_SMT_XLATCH_FORCE_DQS) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_09_BUFGP_XLATCH_FORCE) |
		p_fld(arpi_bit4to10,
		RG_ARPI_RESERVE_BIT_10_BUFGP_XLATCH_FORCE_DQS) |
		p_fld(bypass_sr, RG_ARPI_RESERVE_BIT_11_BYPASS_SR) |
		p_fld(bypass_sr, RG_ARPI_RESERVE_BIT_12_BYPASS_SR_DQS) |
		p_fld(0, RG_ARPI_RESERVE_BIT_13_CG_SYNC_ENB) |
		p_fld(lp3_sel, RG_ARPI_RESERVE_BIT_14_LP3_SEL) |
		p_fld(psmux_drv_sel, RG_ARPI_RESERVE_BIT_15_PSMUX_DRV_SEL));

	io_32_write_fld_multi_all(DDRPHY_SHU_CA_DLL1,
		p_fld(SET_FLD, RG_ARCMD_REV_BIT_00_TX_LSH_DQ_CG_EN) |
		p_fld(CLEAR_FLD, RG_ARCMD_REV_BIT_01_TX_LSH_DQS_CG_EN) |
		p_fld(CLEAR_FLD, RG_ARCMD_REV_BIT_02_TX_LSH_DQM_CG_EN) |
		p_fld(CLEAR_FLD, RG_ARCMD_REV_BIT_03_RX_DQS_GATE_EN_MODE) |
		p_fld(CLEAR_FLD, RG_ARCMD_REV_BIT_04_RX_DQSIEN_RB_DLY) |
		p_fld(ser_rst_mode, RG_ARCMD_REV_BIT_05_RX_SER_RST_MODE) |
		p_fld(SET_FLD, RG_ARCMD_REV_BIT_06_MCK4X_SEL_CKE0) |
		p_fld(SET_FLD, RG_ARCMD_REV_BIT_07_MCK4X_SEL_CKE1) |
		p_fld(0x4, RG_ARCMD_REV_BIT_1208_TX_CKE_DRVN) |
		p_fld(CLEAR_FLD, RG_ARCMD_REV_BIT_13_TX_DDR3_CKE_SEL) |
		p_fld(CLEAR_FLD, RG_ARCMD_REV_BIT_14_TX_DDR4_CKE_SEL) |
		p_fld(CLEAR_FLD, RG_ARCMD_REV_BIT_15_TX_DDR4P_CKE_SEL) |
		p_fld(CLEAR_FLD, RG_ARCMD_REV_BIT_1716_TX_LP4Y_SEL) |
		p_fld(CLEAR_FLD, RG_ARCMD_REV_BIT_18_RX_LP4Y_EN) |
		p_fld(CLEAR_FLD, RG_ARCMD_REV_BIT_19_RX_DQSIEN_FORCE_ON_EN) |
		p_fld(CLEAR_FLD, RG_ARCMD_REV_BIT_20_DATA_SWAP_EN) |
		p_fld(CLEAR_FLD, RG_ARCMD_REV_BIT_2221_DATA_SWAP) |
		p_fld(CLEAR_FLD, RG_ARCMD_REV_BIT_23_NA));

	io_32_write_fld_multi_all(DDRPHY_SHU_B0_DQ6,
		p_fld(SET_FLD, RG_ARPI_RESERVE_BIT_00_TX_CG_EN) |
		p_fld(0, RG_ARPI_RESERVE_BIT_01_DLL_FAST_PSJP) |
		p_fld(hyst_sel, RG_ARPI_RESERVE_BIT_02_HYST_SEL) |
		p_fld(midpi_cap_sel2, RG_ARPI_RESERVE_BIT_03_MIDPI_CAP_SEL) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_04_8PHASE_XLATCH_FORCE) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_05_PSMUX_XLATCH_FORCE) |
		p_fld(arpi_bit4to10,
		RG_ARPI_RESERVE_BIT_06_PSMUX_XLATCH_FORCEDQS) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_07_SMT_XLATCH_FORCE) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_08_SMT_XLATCH_FORCE_DQS) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_09_BUFGP_XLATCH_FORCE) |
		p_fld(arpi_bit4to10,
		RG_ARPI_RESERVE_BIT_10_BUFGP_XLATCH_FORCE_DQS) |
		p_fld(bypass_sr, RG_ARPI_RESERVE_BIT_11_BYPASS_SR) |
		p_fld(bypass_sr, RG_ARPI_RESERVE_BIT_12_BYPASS_SR_DQS) |
		p_fld(0, RG_ARPI_RESERVE_BIT_13_CG_SYNC_ENB) |
		p_fld(lp3_sel, RG_ARPI_RESERVE_BIT_14_LP3_SEL) |
		p_fld(psmux_drv_sel, RG_ARPI_RESERVE_BIT_15_PSMUX_DRV_SEL));

	io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ6,
		p_fld(SET_FLD, RG_ARPI_RESERVE_BIT_00_TX_CG_EN) |
		p_fld(0, RG_ARPI_RESERVE_BIT_01_DLL_FAST_PSJP) |
		p_fld(hyst_sel, RG_ARPI_RESERVE_BIT_02_HYST_SEL) |
		p_fld(midpi_cap_sel2, RG_ARPI_RESERVE_BIT_03_MIDPI_CAP_SEL) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_04_8PHASE_XLATCH_FORCE) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_05_PSMUX_XLATCH_FORCE) |
		p_fld(arpi_bit4to10,
		RG_ARPI_RESERVE_BIT_06_PSMUX_XLATCH_FORCEDQS) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_07_SMT_XLATCH_FORCE) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_08_SMT_XLATCH_FORCE_DQS) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_09_BUFGP_XLATCH_FORCE) |
		p_fld(arpi_bit4to10,
		RG_ARPI_RESERVE_BIT_10_BUFGP_XLATCH_FORCE_DQS) |
		p_fld(bypass_sr, RG_ARPI_RESERVE_BIT_11_BYPASS_SR) |
		p_fld(bypass_sr, RG_ARPI_RESERVE_BIT_12_BYPASS_SR_DQS) |
		p_fld(0, RG_ARPI_RESERVE_BIT_13_CG_SYNC_ENB) |
		p_fld(lp3_sel, RG_ARPI_RESERVE_BIT_14_LP3_SEL) |
		p_fld(psmux_drv_sel, RG_ARPI_RESERVE_BIT_15_PSMUX_DRV_SEL));
	io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ6,
		p_fld(SET_FLD, RG_ARPI_RESERVE_BIT_00_TX_CG_EN) |
		p_fld(0, RG_ARPI_RESERVE_BIT_01_DLL_FAST_PSJP) |
		p_fld(hyst_sel, RG_ARPI_RESERVE_BIT_02_HYST_SEL) |
		p_fld(midpi_cap_sel2, RG_ARPI_RESERVE_BIT_03_MIDPI_CAP_SEL) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_04_8PHASE_XLATCH_FORCE) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_05_PSMUX_XLATCH_FORCE) |
		p_fld(arpi_bit4to10,
		RG_ARPI_RESERVE_BIT_06_PSMUX_XLATCH_FORCEDQS) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_07_SMT_XLATCH_FORCE) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_08_SMT_XLATCH_FORCE_DQS) |
		p_fld(arpi_bit4to10, RG_ARPI_RESERVE_BIT_09_BUFGP_XLATCH_FORCE) |
		p_fld(arpi_bit4to10,
		RG_ARPI_RESERVE_BIT_10_BUFGP_XLATCH_FORCE_DQS) |
		p_fld(bypass_sr, RG_ARPI_RESERVE_BIT_11_BYPASS_SR) |
		p_fld(bypass_sr, RG_ARPI_RESERVE_BIT_12_BYPASS_SR_DQS) |
		p_fld(0, RG_ARPI_RESERVE_BIT_13_CG_SYNC_ENB) |
		p_fld(lp3_sel, RG_ARPI_RESERVE_BIT_14_LP3_SEL) |
		p_fld(psmux_drv_sel, RG_ARPI_RESERVE_BIT_15_PSMUX_DRV_SEL));

	io_32_write_fld_multi_all(DDRPHY_SHU_B0_DLL1,
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_00_DQS_MCK4X_DLY_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_01_DQS_MCK4XB_DLY_EN) |
		p_fld(tx_read_base_en, RG_ARDQ_REV_BIT_02_TX_READ_BASE_EN_DQSB) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_03_RX_DQS_GATE_EN_MODE) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_04_RX_DQSIEN_RB_DLY) |
		p_fld(ser_rst_mode, RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_06_MCK4X_SEL_DQ1) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_07_MCK4X_SEL_DQ5) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_08_TX_ODT_DISABLE) |
		p_fld(tx_read_base_en, RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_1110_DRVN_PRE) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_1312_DRVP_PRE) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_14_TX_PRE_DATA_SEL) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_15_TX_PRE_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_1716_TX_LP4Y_SEL) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_18_RX_LP4Y_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_19_RX_DQSIEN_FORCE_ON_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_20_DATA_SWAP_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_2221_DATA_SWAP) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_23_NA));

	io_32_write_fld_multi_all(DDRPHY_SHU_B1_DLL1,
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_00_DQS_MCK4X_DLY_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_01_DQS_MCK4XB_DLY_EN) |
		p_fld(tx_read_base_en, RG_ARDQ_REV_BIT_02_TX_READ_BASE_EN_DQSB) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_03_RX_DQS_GATE_EN_MODE) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_04_RX_DQSIEN_RB_DLY) |
		p_fld(ser_rst_mode, RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_06_MCK4X_SEL_DQ1) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_07_MCK4X_SEL_DQ5) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_08_TX_ODT_DISABLE) |
		p_fld(tx_read_base_en, RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_1110_DRVN_PRE) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_1312_DRVP_PRE) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_14_TX_PRE_DATA_SEL) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_15_TX_PRE_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_1716_TX_LP4Y_SEL) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_18_RX_LP4Y_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_19_RX_DQSIEN_FORCE_ON_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_20_DATA_SWAP_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_2221_DATA_SWAP) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_23_NA));
	io_32_write_fld_multi_all(DDRPHY_SHU_B2_DLL1,
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_00_DQS_MCK4X_DLY_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_01_DQS_MCK4XB_DLY_EN) |
		p_fld(tx_read_base_en, RG_ARDQ_REV_BIT_02_TX_READ_BASE_EN_DQSB) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_03_RX_DQS_GATE_EN_MODE) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_04_RX_DQSIEN_RB_DLY) |
		p_fld(ser_rst_mode, RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_06_MCK4X_SEL_DQ1) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_07_MCK4X_SEL_DQ5) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_08_TX_ODT_DISABLE) |
		p_fld(tx_read_base_en, RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_1110_DRVN_PRE) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_1312_DRVP_PRE) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_14_TX_PRE_DATA_SEL) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_15_TX_PRE_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_1716_TX_LP4Y_SEL) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_18_RX_LP4Y_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_19_RX_DQSIEN_FORCE_ON_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_20_DATA_SWAP_EN) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_2221_DATA_SWAP) |
		p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_23_NA));

#if TX_DQ_PRE_EMPHASIS
	if ((p->frequency > DDR3200_FREQ) && (dramc_imp_result[ODT_ON][TERM_TYPE_DRVP] > 0)) {
		if (dramc_imp_result[ODT_ON][TERM_TYPE_DRVP] <= 7)
			drving = 1;
		else if (dramc_imp_result[ODT_ON][TERM_TYPE_DRVP] <= 15)
			drving = 2;
		else
			drving = 3;

		io_32_write_fld_multi_all(DDRPHY_SHU_B0_DLL1,
			p_fld(drving, RG_ARDQ_REV_BIT_1110_DRVN_PRE) |
			p_fld(drving, RG_ARDQ_REV_BIT_1312_DRVP_PRE) |
			p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_14_TX_PRE_DATA_SEL) |
			p_fld(SET_FLD, RG_ARDQ_REV_BIT_15_TX_PRE_EN));
		io_32_write_fld_multi_all(DDRPHY_SHU_B1_DLL1,
			p_fld(drving, RG_ARDQ_REV_BIT_1110_DRVN_PRE) |
			p_fld(drving, RG_ARDQ_REV_BIT_1312_DRVP_PRE) |
			p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_14_TX_PRE_DATA_SEL) |
			p_fld(SET_FLD, RG_ARDQ_REV_BIT_15_TX_PRE_EN));
		io_32_write_fld_multi_all(DDRPHY_SHU_B2_DLL1,
			p_fld(drving, RG_ARDQ_REV_BIT_1110_DRVN_PRE) |
			p_fld(drving, RG_ARDQ_REV_BIT_1312_DRVP_PRE) |
			p_fld(CLEAR_FLD, RG_ARDQ_REV_BIT_14_TX_PRE_DATA_SEL) |
			p_fld(SET_FLD, RG_ARDQ_REV_BIT_15_TX_PRE_EN));

		show_msg2((INFO, "[TX_DQ_PRE_EMPHASIS] Set pre - emphasis driving at %d\n",
			drving));
	}
#endif

#if 0

	io_32_write_fld_align(DDRPHY_SHU_B0_DQ3, CLEAR_FLD,
		SHU_B0_DQ3_RG_ARDQ_REV_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ3, CLEAR_FLD,
		SHU_B1_DQ3_RG_ARDQ_REV_B1);

	io_32_write_fld_align(DDRPHY_SHU_B0_DQ3, 0x20,
		SHU_B0_DQ3_RG_ARDQ_REV_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ3, 0x20,
		SHU_B1_DQ3_RG_ARDQ_REV_B1);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD3, 0x09E0,
		SHU_CA_CMD3_RG_ARCMD_REV);

	io_32_write_fld_align(DDRPHY_SHU_CA_CMD3, 0x7e2,
		SHU_CA_CMD3_RG_ARCMD_REV);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD3 + SHIFT_TO_CHB_ADDR, 0x720,
		SHU_CA_CMD3_RG_ARCMD_REV);
#endif

	/* cc change after DE bringup review */
	if (is_lp4_family(p)) {
		unsigned int arpi_rsv_dq;
		unsigned int arpi_rsv_ca_cha, arpi_rsv_ca_chb;

		if (p->frequency >= DDR3200_FREQ) {
			arpi_rsv_dq = 0x9805;
			arpi_rsv_ca_cha = 0x9807;
			arpi_rsv_ca_chb = 0x9805;
		} else if (p->frequency >= DDR2666_FREQ) {
			arpi_rsv_dq = 0x980d;
			arpi_rsv_ca_cha = 0x980f;
			arpi_rsv_ca_chb = 0x980d;
		} else {
			arpi_rsv_dq = 0x47f5;
			arpi_rsv_ca_cha = 0x47f5;
			arpi_rsv_ca_chb = 0x47f5;
		}

		io_32_write_fld_align(DDRPHY_SHU_CA_CMD6,
			arpi_rsv_ca_cha, SHU_CA_CMD6_RG_ARPI_RESERVE_CA);
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD6 + SHIFT_TO_CHB_ADDR,
			arpi_rsv_ca_chb, SHU_CA_CMD6_RG_ARPI_RESERVE_CA);

		io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ6,
			arpi_rsv_dq, SHU_B0_DQ6_RG_ARPI_RESERVE_B0);
		io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ6,
			arpi_rsv_dq, SHU_B1_DQ6_RG_ARPI_RESERVE_B1);
		io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ6,
			arpi_rsv_dq, SHU_B2_DQ6_RG_ARPI_RESERVE_B2);

		/* WL waveform review result: ARDQ_REV_B0/B1/B2/CMD[25] = 1'b1 */
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL2,
			(1 << 1), SHU_CA_DLL2_RG_ARCMD_MSB_REV);
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL2 + SHIFT_TO_CHB_ADDR,
			(1 << 1), SHU_CA_DLL2_RG_ARCMD_MSB_REV);
		io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL2,
			(1 << 1), SHU_B0_DLL2_RG_ARDQ_REV_MSB_B0);
		io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL2,
			(1 << 1), SHU_B1_DLL2_RG_ARDQ_REV_MSB_B1);
		io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL2,
			(1 << 1), SHU_B2_DLL2_RG_ARDQ_REV_MSB_B2);

	}
	if (p->dram_type == TYPE_PCDDR4) {
		if (p->frequency == DDR3200_FREQ) {
			reserve_ca_cha = 0x9805;
			reserve_ca_chb = 0x9807;
		} else if (p->frequency == DDR2666_FREQ) {
			reserve_ca_cha = 0x980d;
			reserve_ca_chb = 0x980f;
		} else {
			reserve_ca_cha = 0x47f5;
			reserve_ca_chb = 0x47f5;
		}
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD6,
			reserve_ca_chb, SHU_CA_CMD6_RG_ARPI_RESERVE_CA);
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD6 + SHIFT_TO_CHB_ADDR,
			reserve_ca_cha, SHU_CA_CMD6_RG_ARPI_RESERVE_CA);

		io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ6,
			reserve_ca_cha, SHU_B0_DQ6_RG_ARPI_RESERVE_B0);
		io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ6,
			reserve_ca_cha, SHU_B1_DQ6_RG_ARPI_RESERVE_B1);
		io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ6,
			reserve_ca_cha, SHU_B2_DQ6_RG_ARPI_RESERVE_B2);

		io_32_write_fld_align(DDRPHY_SHU_CA_DLL2,
			0x2, SHU_CA_DLL2_RG_ARCMD_MSB_REV);
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL2 + SHIFT_TO_CHB_ADDR,
			0x2, SHU_CA_DLL2_RG_ARCMD_MSB_REV);
		io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL2,
			0x2, SHU_B0_DLL2_RG_ARDQ_REV_MSB_B0);
		io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL2,
			0x2, SHU_B1_DLL2_RG_ARDQ_REV_MSB_B1);
		io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL2,
			0x2, SHU_B2_DLL2_RG_ARDQ_REV_MSB_B2);
	}
	if (p->dram_type == TYPE_PCDDR3) {
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD6,
			0x47f5, SHU_CA_CMD6_RG_ARPI_RESERVE_CA);
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD6 + SHIFT_TO_CHB_ADDR,
			0x47f5, SHU_CA_CMD6_RG_ARPI_RESERVE_CA);
		io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ6,
			0x47f5, SHU_B0_DQ6_RG_ARPI_RESERVE_B0);
		io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ6,
			0x47f5, SHU_B1_DQ6_RG_ARPI_RESERVE_B1);
		io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ6,
			0x47f5, SHU_B2_DQ6_RG_ARPI_RESERVE_B2);

		io_32_write_fld_align(DDRPHY_SHU_CA_DLL2,
			0x82, SHU_CA_DLL2_RG_ARCMD_MSB_REV);
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL2 + SHIFT_TO_CHB_ADDR,
			0x82, SHU_CA_DLL2_RG_ARCMD_MSB_REV);
		io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL2,
			0x82, SHU_B0_DLL2_RG_ARDQ_REV_MSB_B0);
		io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL2,
			0x82, SHU_B1_DLL2_RG_ARDQ_REV_MSB_B1);
		io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL2,
			0x82, SHU_B2_DLL2_RG_ARDQ_REV_MSB_B2);
	}
	if (p->dram_type == TYPE_LPDDR3) {
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD6,
			0x47f5, SHU_CA_CMD6_RG_ARPI_RESERVE_CA);
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD6 + SHIFT_TO_CHB_ADDR,
			0x47f5, SHU_CA_CMD6_RG_ARPI_RESERVE_CA);
		io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ6,
			0x47f5, SHU_B0_DQ6_RG_ARPI_RESERVE_B0);
		io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ6,
			0x47f5, SHU_B1_DQ6_RG_ARPI_RESERVE_B1);
		io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ6,
			0x47f5, SHU_B2_DQ6_RG_ARPI_RESERVE_B2);

		io_32_write_fld_align(DDRPHY_SHU_CA_DLL2,
			0x2, SHU_CA_DLL2_RG_ARCMD_MSB_REV);
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL2 + SHIFT_TO_CHB_ADDR,
			0x2, SHU_CA_DLL2_RG_ARCMD_MSB_REV);
		io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL2,
			0x2, SHU_B0_DLL2_RG_ARDQ_REV_MSB_B0);
		io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL2,
			0x2, SHU_B1_DLL2_RG_ARDQ_REV_MSB_B1);
		io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL2,
			0x2, SHU_B2_DLL2_RG_ARDQ_REV_MSB_B2);
	}
}


#if (SUPPORT_TYPE_LPDDR4 || SUPPORT_TYPE_LPDDR3)

#if SUPPORT_TYPE_LPDDR4
static DRAM_STATUS_T update_initial_setting_lp4(DRAMC_CTX_T *p)
{
	unsigned short rxvref_default = 0x8;
	unsigned short channelidx, rankidx;

	if (p->odt_onoff == ODT_ON) {
		io_32_write_fld_align(DRAMC_REG_SHU_ODTCTRL, 1, SHU_ODTCTRL_ROEN);
		io_32_write_fld_align(DDRPHY_SHU_B0_DQ7, SET_FLD,
			SHU_B0_DQ7_R_DMRODTEN_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ7, SET_FLD,
			SHU_B1_DQ7_R_DMRODTEN_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DQ7, SET_FLD,
			SHU_B2_DQ7_R_DMRODTEN_B2);
		io_32_write_fld_multi(DDRPHY_SHU_CA_CMD0,
			p_fld(CLEAR_FLD, SHU_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
			p_fld(CLEAR_FLD, SHU_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	} else {
		io_32_write_fld_align(DRAMC_REG_SHU_ODTCTRL, 0, SHU_ODTCTRL_ROEN);
		io_32_write_fld_align(DDRPHY_SHU_B0_DQ7, CLEAR_FLD,
			SHU_B0_DQ7_R_DMRODTEN_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ7, CLEAR_FLD,
			SHU_B1_DQ7_R_DMRODTEN_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DQ7, CLEAR_FLD,
			SHU_B2_DQ7_R_DMRODTEN_B2);
		io_32_write_fld_multi(DDRPHY_SHU_CA_CMD0,
			p_fld(CLEAR_FLD, SHU_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
			p_fld(CLEAR_FLD, SHU_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	}

	io_32_write_fld_multi(DDRPHY_R0_B0_RXDVS2,
		p_fld(CLEAR_FLD, R0_B0_RXDVS2_R_RK0_DVS_MODE_B0) |
		p_fld(CLEAR_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(CLEAR_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi(DDRPHY_R1_B0_RXDVS2,
		p_fld(CLEAR_FLD, R1_B0_RXDVS2_R_RK1_DVS_MODE_B0) |
		p_fld(CLEAR_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(CLEAR_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS2,
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS2,
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1) |
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R0_B2_RXDVS2,
		p_fld(CLEAR_FLD, R0_B2_RXDVS2_R_RK0_DVS_MODE_B2) |
		p_fld(CLEAR_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(CLEAR_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B2));
	io_32_write_fld_multi(DDRPHY_R1_B2_RXDVS2,
		p_fld(CLEAR_FLD, R1_B2_RXDVS2_R_RK1_DVS_MODE_B2) |
		p_fld(CLEAR_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(CLEAR_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B2));

	io_32_write_fld_align(DDRPHY_SHU_CA_CMD7, CLEAR_FLD,
		SHU_CA_CMD7_R_DMRANKRXDVS_CA);

	io_32_write_fld_align(DDRPHY_CA_CMD3, SET_FLD,
		CA_CMD3_RG_RX_ARCMD_STBENCMP_EN);

	io_32_write_fld_align(DDRPHY_CA_CMD10, SET_FLD,
		CA_CMD10_RG_RX_ARCLK_DQSIENMODE);

	io_32_write_fld_align(DDRPHY_CA_CMD6, 0x2,
		CA_CMD6_RG_RX_ARCMD_BIAS_VREF_SEL);

	io_32_write_fld_multi(DDRPHY_B0_DQ3,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ3,
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B1));
	io_32_write_fld_multi(DDRPHY_B2_DQ3,
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B2));

	io_32_write_fld_multi(DDRPHY_CA_CMD3,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_IN_BUFF_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCLK_IN_BUFF_EN));

	io_32_write_fld_align(DDRPHY_B0_DQ3, CLEAR_FLD,
		B0_DQ3_RG_RX_ARDQ_SMT_EN_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ3, CLEAR_FLD,
		B1_DQ3_RG_RX_ARDQ_SMT_EN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ3, CLEAR_FLD,
		B2_DQ3_RG_RX_ARDQ_SMT_EN_B2);

	io_32_write_fld_align(DDRPHY_B0_DQ5, SET_FLD,
		B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ5, SET_FLD,
		B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ5, SET_FLD,
		B2_DQ5_RG_RX_ARDQS0_DVS_EN_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD5, SET_FLD,
		CA_CMD5_RG_RX_ARCLK_DVS_EN);

	/*
	 * LP4 no need, follow LP3 first.
	 * io_32_write_fld_align(DDRPHY_MISC_VREF_CTRL,
		p_fld(SET_FLD, MISC_VREF_CTRL_RG_RVREF_DDR3_SEL) |
	 * | p_fld(CLEAR_FLD, MISC_VREF_CTRL_RG_RVREF_DDR4_SEL));
	 */

	io_32_write_fld_multi(DDRPHY_CA_CMD6,
		p_fld(CLEAR_FLD, CA_CMD6_RG_TX_ARCMD_DDR3_SEL) |
		p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_DDR3_SEL) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_DDR4_SEL));
	io_32_write_fld_multi(DDRPHY_CA_CMD6,
		p_fld(CLEAR_FLD, CA_CMD6_RG_TX_ARCMD_DDR3_SEL) |
		p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_DDR3_SEL) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_DDR4_SEL));
	io_32_write_fld_multi(DDRPHY_MISC_IMP_CTRL0,
		p_fld(CLEAR_FLD, MISC_IMP_CTRL0_RG_RIMP_DDR3_SEL) |
		p_fld(SET_FLD, MISC_IMP_CTRL0_RG_RIMP_DDR4_SEL));

	io_32_write_fld_align(DDRPHY_B0_DQ6, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_O1_SEL_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ6, SET_FLD,
		B1_DQ6_RG_RX_ARDQ_O1_SEL_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ6, SET_FLD,
		B2_DQ6_RG_RX_ARDQ_O1_SEL_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD6, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_O1_SEL);

	io_32_write_fld_align(DDRPHY_B0_DQ6, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_BIAS_PS_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ6, SET_FLD,
		B1_DQ6_RG_RX_ARDQ_BIAS_PS_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ6, SET_FLD,
		B2_DQ6_RG_RX_ARDQ_BIAS_PS_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD6, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_BIAS_PS);

	io_32_write_fld_align(DDRPHY_CA_CMD6, CLEAR_FLD,
		CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN);

	io_32_write_fld_align(DDRPHY_B0_DQ6, CLEAR_FLD,
		B0_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ6, CLEAR_FLD,
		B1_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ6, CLEAR_FLD,
		B2_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD6, CLEAR_FLD,
		CA_CMD6_RG_TX_ARCMD_ODTEN_EXT_DIS);

	io_32_write_fld_align(DDRPHY_B0_DQ6, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ6, SET_FLD,
		B1_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ6, SET_FLD,
		B2_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD6, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_RPRE_TOG_EN);

	if (p->dram_type == TYPE_LPDDR4) {
		if (p->odt_onoff == ODT_ON)
			rxvref_default = 0xE;
		else
			rxvref_default = 0x16;
	} else if (p->dram_type == TYPE_LPDDR4X) {
		if (p->odt_onoff == ODT_ON)
			rxvref_default = 0xb;
		else
			rxvref_default = 0x16;
	} else {
		rxvref_default = 0x10;
	}

	io_32_write_fld_align(DDRPHY_SHU_B0_DQ5, rxvref_default,
		SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ5, rxvref_default,
		SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ5, rxvref_default,
		SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2);
	io_32_write_fld_align(DDRPHY_B0_DQ5, rxvref_default,
		B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ5, rxvref_default,
		B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ5, rxvref_default,
		B2_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B2);

	if ((p->dram_type == TYPE_LPDDR4X) || (p->dram_type == TYPE_LPDDR4P)) {

		io_32_write_fld_multi(DDRPHY_B0_DQ8,
			p_fld(SET_FLD, B0_DQ8_RG_TX_ARDQ_EN_LP4P_B0) |
			p_fld(SET_FLD, B0_DQ8_RG_TX_ARDQ_EN_CAP_LP4P_B0) |
			p_fld(SET_FLD, B0_DQ8_RG_TX_ARDQ_CAP_DET_B0));
		/*
		 * | p_fld(SET_FLD, B0_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B0) // Field only exists for 10nm APHY
		 * corresponds to B0_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B0 for 16nm APHY
		 */
		io_32_write_fld_multi(DDRPHY_B1_DQ8,
			p_fld(SET_FLD, B1_DQ8_RG_TX_ARDQ_EN_LP4P_B1) |
			p_fld(SET_FLD, B1_DQ8_RG_TX_ARDQ_EN_CAP_LP4P_B1) |
			p_fld(SET_FLD, B1_DQ8_RG_TX_ARDQ_CAP_DET_B1));

		io_32_write_fld_multi(DDRPHY_B2_DQ8,
			p_fld(SET_FLD, B2_DQ8_RG_TX_ARDQ_EN_LP4P_B2) |
			p_fld(SET_FLD, B2_DQ8_RG_TX_ARDQ_EN_CAP_LP4P_B2) |
			p_fld(SET_FLD, B2_DQ8_RG_TX_ARDQ_CAP_DET_B2));
		/*
		 * | p_fld(SET_FLD, B1_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B1) // Field only exists for 10nm APHY
		 * corresponds to B1_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B1 for 16nm APHY
		 */
		io_32_write_fld_multi(DDRPHY_CA_CMD9,
			p_fld(SET_FLD, CA_CMD9_RG_TX_ARCMD_EN_LP4P) |
			p_fld(SET_FLD, CA_CMD9_RG_TX_ARCMD_EN_CAP_LP4P) |
			p_fld(SET_FLD, CA_CMD9_RG_TX_ARCMD_CAP_DET));
	} else {
		io_32_write_fld_multi(DDRPHY_B0_DQ8,
			p_fld(CLEAR_FLD, B0_DQ8_RG_TX_ARDQ_EN_LP4P_B0) |
			p_fld(CLEAR_FLD, B0_DQ8_RG_TX_ARDQ_EN_CAP_LP4P_B0) |
			p_fld(CLEAR_FLD, B0_DQ8_RG_TX_ARDQ_CAP_DET_B0));
		/*
		 * | p_fld(SET_FLD, B0_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B0) // Field only exists for 10nm APHY
		 * corresponds to B0_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B0 for 16nm APHY
		 */
		io_32_write_fld_multi(DDRPHY_B1_DQ8,
			p_fld(CLEAR_FLD, B1_DQ8_RG_TX_ARDQ_EN_LP4P_B1) |
			p_fld(CLEAR_FLD, B1_DQ8_RG_TX_ARDQ_EN_CAP_LP4P_B1) |
			p_fld(CLEAR_FLD, B1_DQ8_RG_TX_ARDQ_CAP_DET_B1));

		io_32_write_fld_multi(DDRPHY_B2_DQ8,
			p_fld(CLEAR_FLD, B2_DQ8_RG_TX_ARDQ_EN_LP4P_B2) |
			p_fld(CLEAR_FLD, B2_DQ8_RG_TX_ARDQ_EN_CAP_LP4P_B2) |
			p_fld(CLEAR_FLD, B2_DQ8_RG_TX_ARDQ_CAP_DET_B2));
		/*
		 * | p_fld(SET_FLD, B1_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B1) // Field only exists for 10nm APHY
		 * corresponds to B1_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B1 for 16nm APHY
		 */
		io_32_write_fld_multi(DDRPHY_CA_CMD9,
			p_fld(CLEAR_FLD, CA_CMD9_RG_TX_ARCMD_EN_LP4P) |
			p_fld(CLEAR_FLD, CA_CMD9_RG_TX_ARCMD_EN_CAP_LP4P) |
			p_fld(CLEAR_FLD, CA_CMD9_RG_TX_ARCMD_CAP_DET));
	}

	io_32_write_fld_align(DDRPHY_CA_CMD8, SET_FLD,
		CA_CMD8_RG_TX_RRESETB_DDR3_SEL);
	io_32_write_fld_align(DDRPHY_CA_CMD8, CLEAR_FLD,
		CA_CMD8_RG_TX_RRESETB_DDR4_SEL);

	io_32_write_fld_align(DRAMC_REG_SHU_MISC, 0x2,
		SHU_MISC_REQQUE_MAXCNT);

	io_32_write_fld_multi(DRAMC_REG_SHU_DQSG,
		p_fld(0x2a, SHU_DQSG_SCINTV) |
		p_fld(SET_FLD, SHU_DQSG_DQSINCTL_PRE_SEL));

	io_32_write_fld_align(DDRPHY_SHU_B0_DQ5, CLEAR_FLD,
		SHU_B0_DQ5_RG_ARPI_FB_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ5, CLEAR_FLD,
		SHU_B1_DQ5_RG_ARPI_FB_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ5, CLEAR_FLD,
		SHU_B2_DQ5_RG_ARPI_FB_B2);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD5, CLEAR_FLD,
		SHU_CA_CMD5_RG_ARPI_FB_CA);

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ6, CLEAR_FLD,
		SHU_B0_DQ6_RG_ARPI_OFFSET_DQSIEN_B0);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ6, CLEAR_FLD,
		SHU_B1_DQ6_RG_ARPI_OFFSET_DQSIEN_B1);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ6, CLEAR_FLD,
		SHU_B2_DQ6_RG_ARPI_OFFSET_DQSIEN_B2);
	io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD6, CLEAR_FLD,
		SHU_CA_CMD6_RG_ARPI_OFFSET_CLKIEN);
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	/*
	 * IMP Tracking Init Settings
	 * Write (DRAMC _BASE+ 0x219) [31:0] = 32'h80080020//DDR3200 default
	 * SHU_IMPCAL1_IMPCAL_CHKCYCLE should > 12.5/MCK, 1:4 mode will disable imp tracking -> don't care
	 */
	io_32_write_fld_multi(DRAMC_REG_SHU_IMPCAL1,
		p_fld(8, SHU_IMPCAL1_IMPCAL_CALICNT) |
		p_fld(0x10, SHU_IMPCAL1_IMPCALCNT) |
		p_fld(4, SHU_IMPCAL1_IMPCAL_CALEN_CYCLE) |
		p_fld((p->frequency*25 / 8000) + 1, SHU_IMPCAL1_IMPCAL_CHKCYCLE));

	io_32_write_fld_multi(DRAMC_REG_SREFCTRL,
		p_fld(SET_FLD, SREFCTRL_SCSM_CGAR) |
		p_fld(SET_FLD, SREFCTRL_SCARB_SM_CGAR) |
		p_fld(SET_FLD, SREFCTRL_RDDQSOSC_CGAR) |
		p_fld(SET_FLD, SREFCTRL_HMRRSEL_CGAR));
	io_32_write_fld_align(DRAMC_REG_PRE_TDQSCK1, SET_FLD,
		PRE_TDQSCK1_TXUIPI_CAL_CGAR);

	io_32_write_fld_align(DRAMC_REG_SHU_MISC, 0xf, SHU_MISC_PREA_INTV);
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ8,
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMSTBEN_SYNC_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0) |
		p_fld(0x7fff, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ8,
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMSTBEN_SYNC_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1) |
		p_fld(0x7fff, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ8,
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRANK_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMSTBEN_SYNC_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRXDLY_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ8_R_DMRXDVS_UPD_FORCE_EN_B2) |
		p_fld(0x7fff, SHU_B2_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B2));

	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD8,
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRANK_CHG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRANK_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_FLAG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_FLAG_SYNC_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMSTBEN_SYNC_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRXDLY_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRXDVS_RDSEL_PIPE_CG_IG_CA) |
		p_fld(CLEAR_FLD, SHU_CA_CMD8_R_DMRXDVS_UPD_FORCE_EN_CA) |
		p_fld(0x7fff, SHU_CA_CMD8_R_DMRXDVS_UPD_FORCE_CYC_CA));
	io_32_write_fld_align(DDRPHY_MISC_CTRL3, SET_FLD,
		MISC_CTRL3_R_DDRPHY_COMB_CG_IG);
	io_32_write_fld_multi(DDRPHY_MISC_CTRL3,
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_CG_MCK_DQ_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_MPDIV_CG_DQ_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_CG_DQS_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_CG_DQ_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_CG_MCK_CA_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_MPDIV_CG_CA_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_CG_CLK_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_CG_CMD_OPT));

	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7,
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXDVS_DQM_FLAGSEL_B2));

	io_32_write_fld_multi(DRAMC_REG_CLKAR,
		p_fld(CLEAR_FLD, CLKAR_DWCLKRUN) |
		p_fld(SET_FLD, CLKAR_SELPH_CMD_CG_DIS) |
		p_fld(0x7FFF, CLKAR_REQQUE_PACG_DIS));

	io_32_write_fld_align(DRAMC_REG_SHU_DQSG_RETRY, CLEAR_FLD,
		SHU_DQSG_RETRY_R_RETRY_PA_DSIABLE);
	io_32_write_fld_align(DRAMC_REG_WRITE_LEV, CLEAR_FLD,
		WRITE_LEV_DDRPHY_COMB_CG_SEL);
	io_32_write_fld_align(DRAMC_REG_DUMMY_RD, SET_FLD,
		DUMMY_RD_DUMMY_RD_PA_OPT);
	io_32_write_fld_multi(DRAMC_REG_STBCAL2,
		p_fld(CLEAR_FLD, STBCAL2_STB_UIDLYCG_IG) |
		p_fld(CLEAR_FLD, STBCAL2_STB_PIDLYCG_IG));
	io_32_write_fld_multi(DRAMC_REG_EYESCAN,
		p_fld(SET_FLD, EYESCAN_EYESCAN_DQS_SYNC_EN) |
		p_fld(CLEAR_FLD, EYESCAN_EYESCAN_NEW_DQ_SYNC_EN) |
		p_fld(CLEAR_FLD, EYESCAN_EYESCAN_DQ_SYNC_EN));
	io_32_write_fld_multi(DRAMC_REG_SHU_ODTCTRL,
		p_fld(SET_FLD, SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG) |
		p_fld(SET_FLD, SHU_ODTCTRL_RODTEN_SELPH_CG_IG));

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL0, CLEAR_FLD,
		SHU_B0_DLL0_RG_ARPISM_MCK_SEL_B0_SHU);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL0, CLEAR_FLD,
		SHU_B1_DLL0_RG_ARPISM_MCK_SEL_B1_SHU);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL0, CLEAR_FLD,
		SHU_B2_DLL0_RG_ARPISM_MCK_SEL_B2_SHU);
	io_32_write_fld_align_all(DDRPHY_SHU_CA_DLL0, CLEAR_FLD,
		SHU_CA_DLL0_RG_ARPISM_MCK_SEL_CA_SHU);
	io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI1, CLEAR_FLD,
		CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA);
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	io_32_write_fld_multi(DRAMC_REG_PERFCTL0,
		p_fld(SET_FLD, PERFCTL0_WRFIFO_OPT) |
		p_fld(CLEAR_FLD, PERFCTL0_REORDEREN) |
		p_fld(SET_FLD, PERFCTL0_RWSPLIT));

#if NON_EXIST_RG
#if FIX_CROSSRK_XRT_05T_OPT
	io_32_write_fld_align(DRAMC_REG_PERFCTL0, CLEAR_FLD,
		PERFCTL0_XRT_05T_OPT);
#else
	io_32_write_fld_align(DRAMC_REG_PERFCTL0, SET_FLD,
		PERFCTL0_XRT_05T_OPT);
#endif
#endif

	io_32_write_fld_align(DRAMC_REG_SREFCTRL, SET_FLD,
		SREFCTRL_SREF2_OPTION);
	io_32_write_fld_align(DRAMC_REG_SHUCTRL1, 0x1a, SHUCTRL1_FC_PRDCNT);

#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	io_32_write_fld_multi(DDRPHY_B0_DQ6,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
	io_32_write_fld_multi(DDRPHY_B2_DQ6,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2));

	io_32_write_fld_multi(DDRPHY_CA_CMD6,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_EN));
	io_32_write_fld_multi(DRAMC_REG_STBCAL2,
		p_fld(SET_FLD, STBCAL2_STB_PICG_EARLY_1T_EN) |
		p_fld(SET_FLD, STBCAL2_DQSIEN_SELPH_BY_RANK_EN));

	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7,
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_EN_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_EN_B2));

#else
	io_32_write_fld_multi(DDRPHY_B0_DQ6,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
	io_32_write_fld_multi(DDRPHY_B2_DQ6,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2));
	io_32_write_fld_multi(DDRPHY_CA_CMD6,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_EN));

	io_32_write_fld_multi(DRAMC_REG_STBCAL2,
		p_fld(SET_FLD, STBCAL2_STB_PICG_EARLY_1T_EN) |
		p_fld(SET_FLD, STBCAL2_DQSIEN_SELPH_BY_RANK_EN));
#endif
#if NON_EXIST_RG
	io_32_write_fld_align(DRAMC_REG_SHU_RODTENSTB,
		SHU_RODTENSTB_RODTEN_MCK_MODESEL);
#endif
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSG), \
		p_fld(9, SHU_DQSG_STB_UPDMASKCYC) | \
		p_fld(1, SHU_DQSG_STB_UPDMASK_EN));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_DQSCAL,
		p_fld(0, SHURK0_DQSCAL_R0DQSIENLLMTEN) |
		p_fld(0, SHURK0_DQSCAL_R0DQSIENHLMTEN));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_DQSCAL,
		p_fld(0, SHURK1_DQSCAL_R1DQSIENLLMTEN) |
		p_fld(0, SHURK1_DQSCAL_R1DQSIENHLMTEN));
	io_32_write_fld_multi(DRAMC_REG_SHU_STBCAL,
		p_fld(1, SHU_STBCAL_DQSG_MODE) |
		p_fld(1, SHU_STBCAL_PICGLAT));

	if (get_div_mode(p) == DIV4_MODE) {
		io_32_write_fld_multi(DRAMC_REG_SHU_STBCAL,
			p_fld(1, SHU_STBCAL_R0_DQSIEN_PICG_TAIL_EXT_LAT) |
			p_fld(1, SHU_STBCAL_DQSIEN_RX_SELPH_OPT) |
			p_fld(1, SHU_STBCAL_R1_DQSIEN_PICG_TAIL_EXT_LAT));
	} else {
		io_32_write_fld_multi(DRAMC_REG_SHU_STBCAL,
			p_fld(0, SHU_STBCAL_R0_DQSIEN_PICG_TAIL_EXT_LAT) |
			p_fld(1, SHU_STBCAL_DQSIEN_RX_SELPH_OPT) |
			p_fld(0, SHU_STBCAL_R1_DQSIEN_PICG_TAIL_EXT_LAT));
	}
#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	io_32_write_fld_align(DDRPHY_B0_DQ9, 0x4,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9, 0x4,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9, 0x4,
		B2_DQ9_R_IN_GATE_EN_LOW_OPT_B2);
#else

	io_32_write_fld_align(DDRPHY_B0_DQ9, 0x7,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9, 0x7,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9, 0x7,
		B2_DQ9_R_IN_GATE_EN_LOW_OPT_B2);
#endif
	io_32_write_fld_align(DDRPHY_CA_CMD10, CLEAR_FLD,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);

	io_32_write_fld_align(DDRPHY_SHU_B0_DQ8, SET_FLD,
		SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ8, SET_FLD,
		SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ8, SET_FLD,
		SHU_B2_DQ8_R_DMRXDLY_CG_IG_B2);

#ifdef DUMMY_READ_FOR_DQS_GATING_RETRY
	if (p->support_rank_num == RANK_SINGLE)
	{
		io_32_write_fld_align(DRAMC_REG_SHU_DQSG_RETRY, 1,
			SHU_DQSG_RETRY_R_RETRY_1RANK);
	}
#endif

	show_msg2((INFO, "Enable WDQS\n"));

	io_32_write_fld_multi(DDRPHY_SHU_B0_DLL1,
		p_fld(1, RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN) |
		p_fld(1, RG_ARDQ_REV_BIT_02_TX_READ_BASE_EN_DQSB) |
		p_fld(!p->odt_onoff, RG_ARDQ_REV_BIT_08_TX_ODT_DISABLE));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DLL1,
		p_fld(1, RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN) |
		p_fld(1, RG_ARDQ_REV_BIT_02_TX_READ_BASE_EN_DQSB) |
		p_fld(!p->odt_onoff, RG_ARDQ_REV_BIT_08_TX_ODT_DISABLE));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DLL1,
		p_fld(1, RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN) |
		p_fld(1, RG_ARDQ_REV_BIT_02_TX_READ_BASE_EN_DQSB) |
		p_fld(!p->odt_onoff, RG_ARDQ_REV_BIT_08_TX_ODT_DISABLE));

	io_32_write_fld_multi(DRAMC_REG_SHU_ODTCTRL,
		p_fld(SET_FLD, SHU_ODTCTRL_RODTE) |
		p_fld(SET_FLD, SHU_ODTCTRL_RODTE2) |
		p_fld(SET_FLD, SHU_ODTCTRL_ROEN));

	io_32_write_fld_align(DDRPHY_SHU_B0_DQ7, SET_FLD,
		SHU_B0_DQ7_R_DMRODTEN_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ7, SET_FLD,
		SHU_B1_DQ7_R_DMRODTEN_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ7, SET_FLD,
		SHU_B2_DQ7_R_DMRODTEN_B2);

	io_32_write_fld_align(DRAMC_REG_DRSCTRL, SET_FLD,
		DRSCTRL_DRS_SELFWAKE_DMYRD_DIS);
	io_32_write_fld_align(DRAMC_REG_REFCTRL0, SET_FLD,
		REFCTRL0_REFNA_OPT);
	io_32_write_fld_align(DRAMC_REG_ZQCS, SET_FLD,
		ZQCS_ZQCS_MASK_SEL_CGAR);
	io_32_write_fld_multi(DRAMC_REG_DUMMY_RD,
		p_fld(SET_FLD, DUMMY_RD_DMYRD_REORDER_DIS) |
		p_fld(CLEAR_FLD, DUMMY_RD_DMYRD_HPRI_DIS));
	io_32_write_fld_align(DRAMC_REG_SHUCTRL2, SET_FLD,
		SHUCTRL2_R_DVFS_SREF_OPT);
	io_32_write_fld_align(DRAMC_REG_SHUCTRL3, 0xb,
		SHUCTRL3_VRCGDIS_PRDCNT);
	io_32_write_fld_align(DDRPHY_MISC_CTRL3, CLEAR_FLD,
		MISC_CTRL3_R_DDRPHY_RX_PIPE_CG_IG);

	/*
	 * ARPISM_MCK_SEL_B0, B1 set to 1 (Joe): "Due to TX_PICG modify register is set to 1,
	 * ARPISM_MCK_SEL_Bx should be 1 to fulfill APHY TX OE spec for low freq (Ex: DDR1600)"
	 */
	io_32_write_fld_multi(DDRPHY_B0_DLL_ARPI1,
		p_fld(SET_FLD, B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0_REG_OPT) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0));
	io_32_write_fld_multi(DDRPHY_B1_DLL_ARPI1,
		p_fld(SET_FLD, B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1_REG_OPT) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1));
	io_32_write_fld_multi(DDRPHY_B2_DLL_ARPI1,
		p_fld(SET_FLD, B2_DLL_ARPI1_RG_ARPISM_MCK_SEL_B2_REG_OPT) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPISM_MCK_SEL_B2));

	io_32_write_fld_align(DDRPHY_CA_DLL_ARPI1, SET_FLD,
		CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA_REG_OPT);
	io_32_write_fld_align(DDRPHY_MISC_CTRL0, CLEAR_FLD,
		MISC_CTRL0_R_DMSHU_PHYDCM_FORCEOFF);
	io_32_write_fld_align(DDRPHY_MISC_RXDVS2, 1,
		MISC_RXDVS2_R_DMRXDVS_SHUFFLE_CTRL_CG_IG);
	io_32_write_fld_align(DRAMC_REG_CLKCTRL, SET_FLD,
		CLKCTRL_SEQCLKRUN3);
	io_32_write_fld_multi(DRAMC_REG_REFCTRL1,
		p_fld(1, REFCTRL1_REF_OVERHEAD_SLOW_REFPB_ENA) |
		p_fld(1, REFCTRL1_SREF_CG_OPT) |
		p_fld(0x5, REFCTRL1_MPENDREF_CNT));
	io_32_write_fld_multi(DRAMC_REG_SHUCTRL,
		p_fld(CLEAR_FLD, SHUCTRL_DVFS_CG_OPT) |
		p_fld(0x3, SHUCTRL_R_DVFS_PICG_MARGIN2) |
		p_fld(0x3, SHUCTRL_R_DVFS_PICG_MARGIN3));
	io_32_write_fld_multi(DRAMC_REG_SHUCTRL2,
		p_fld(SET_FLD, SHUCTRL2_SHORTQ_OPT) |
		p_fld(0x3, SHUCTRL2_R_DVFS_PICG_MARGIN));
	io_32_write_fld_align(DRAMC_REG_STBCAL2, CLEAR_FLD,
		STBCAL2_STB_DBG_EN);
	io_32_write_fld_multi(DRAMC_REG_PRE_TDQSCK1,
		p_fld(CLEAR_FLD, PRE_TDQSCK1_APHY_CG_OPT1) |
		p_fld(CLEAR_FLD, PRE_TDQSCK1_SHU_PRELOAD_TX_HW));

#ifndef FIRST_BRING_UP
	if (dfs_get_highest_freq(p) >= 1866)
#endif
	{
		io_32_write_fld_align(DRAMC_REG_CLKAR, 1, CLKAR_SELPH_4LCG_DIS);
	}

	io_32_write_fld_align(DRAMC_REG_CKECTRL, SET_FLD, CKECTRL_CKEPBDIS);

	io_32_write_fld_multi(DDRPHY_CA_TX_MCK,
		p_fld(CLEAR_FLD, CA_TX_MCK_R_DMRESET_FRPHY_OPT) |
		p_fld(0xa, CA_TX_MCK_R_DMRESETB_DRVP_FRPHY) |
		p_fld(0xa, CA_TX_MCK_R_DMRESETB_DRVN_FRPHY));

	io_32_write_fld_align(DRAMC_REG_CKECTRL, CLEAR_FLD,
		CKECTRL_CKELCKFIX);

	io_32_write_fld_align(DRAMC_REG_SHU_RODTENSTB, CLEAR_FLD,
		SHU_RODTENSTB_RODTENSTB_4BYTE_EN);

	dramc_gating_mode(p, GW_MODE_7UI);

	io_32_write_fld_align(DRAMC_REG_SHU_ODTCTRL, SET_FLD, SHU_ODTCTRL_RODTEN_OPT);
	io_32_write_fld_multi(DRAMC_REG_SHU_RODTENSTB,
		p_fld(0, SHU_RODTENSTB_RODTEN_P1_ENABLE) |
		p_fld(1, SHU_RODTENSTB_RODTENSTB_TRACK_UDFLWCTRL) |
		p_fld(1, SHU_RODTENSTB_RODTENSTB_SELPH_MODE));
	io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, SET_FLD, SHU_STBCAL_DQSIEN_PICG_MODE);

#if ENABLE_RODT_TRACKING
	if(get_div_mode(p) == DIV4_MODE)
		io_32_write_fld_align(DRAMC_REG_SHU_RODTENSTB,
			0x21, SHU_RODTENSTB_RODTENSTB_OFFSET);
	else
		io_32_write_fld_align(DRAMC_REG_SHU_RODTENSTB,
			0x11, SHU_RODTENSTB_RODTENSTB_OFFSET);

	unsigned char readroen;
	readroen = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_ODTCTRL),
		SHU_ODTCTRL_ROEN);
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_RODTENSTB),
		p_fld(0xff, SHU_RODTENSTB_RODTENSTB_EXT) |
		p_fld(readroen, SHU_RODTENSTB_RODTENSTB_TRACK_EN));
#endif

	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, SET_FLD,
		SPCMDCTRL_SPDR_MR4_OPT);

	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ3,
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQ_PDB_PRE_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQ_PU_PRE_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQS0_PDB_PRE_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQS0_PU_PRE_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ3,
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQ_PDB_PRE_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQ_PU_PRE_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQS0_PDB_PRE_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQS0_PU_PRE_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ3,
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQ_PDB_PRE_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQ_PU_PRE_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQS0_PDB_PRE_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQS0_PU_PRE_B2));


	io_32_write_fld_align(DDRPHY_B0_DQ9, SET_FLD,
		B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_R_DMRXFIFO_STBENCMP_EN_B2);



#if CBT_MOVE_CA_INSTEAD_OF_CLK
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA7,
		p_fld(CLEAR_FLD, SHU_SELPH_CA7_DLY_RA0) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA7_DLY_RA1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA7_DLY_RA2) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA7_DLY_RA3) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA7_DLY_RA4) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA7_DLY_RA5));
	io_32_write_fld_align(DRAMC_REG_SHU_SELPH_CA5, CLEAR_FLD,
		SHU_SELPH_CA5_DLY_CKE);
	io_32_write_fld_align(DRAMC_REG_SHU_SELPH_CA6, CLEAR_FLD,
		SHU_SELPH_CA6_DLY_CKE1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD9), 0x20,
		SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R1_CA_CMD9), 0x20,
		SHU_R1_CA_CMD9_RG_RK1_ARPI_CMD);
#endif

	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_CG_CTRL5), 0x0,
		MISC_CG_CTRL5_R_DVS_DIV4_CG_CTRL_B2_13);

	return DRAM_OK;
}

static void dramc_setting_ddr3733(DRAMC_CTX_T *p)
{
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_DQS0,
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS3) |
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS2) |
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS1) |
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS0) |
		p_fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS3) |
		p_fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS2) |
		p_fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS1) |
		p_fld(0x4, SHU_SELPH_DQS0_TXDLY_DQS0));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_DQS1,
		p_fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS3) |
		p_fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS2) |
		p_fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS1) |
		p_fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS0) |
		p_fld(SET_FLD, SHU_SELPH_DQS1_DLY_DQS3) |
		p_fld(SET_FLD, SHU_SELPH_DQS1_DLY_DQS2) |
		p_fld(SET_FLD, SHU_SELPH_DQS1_DLY_DQS1) |
		p_fld(SET_FLD, SHU_SELPH_DQS1_DLY_DQS0));
}

static void dramc_setting_ddr2667(DRAMC_CTX_T *p)
{

#if 0//cc mark
	LegacyPreCalLP4_DDR2667(p);
#endif

	io_32_write_fld_multi(DRAMC_REG_SHU_RANKCTL,
		p_fld(0x4, SHU_RANKCTL_RANKINCTL_PHY) |
		p_fld(0x2, SHU_RANKCTL_RANKINCTL_ROOT1) |
		p_fld(0x2, SHU_RANKCTL_RANKINCTL));
	io_32_write_fld_multi(DRAMC_REG_SHU_CKECTRL,
		p_fld(0x3, SHU_CKECTRL_TCKESRX));

	io_32_write_fld_multi(DRAMC_REG_SHU_ODTCTRL,
		p_fld(SET_FLD, SHU_ODTCTRL_RODTE) |
		p_fld(SET_FLD, SHU_ODTCTRL_RODTE2) |

		p_fld(SET_FLD, SHU_ODTCTRL_ROEN));
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ7, SET_FLD,
		SHU_B0_DQ7_R_DMRODTEN_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ7, SET_FLD,
		SHU_B1_DQ7_R_DMRODTEN_B1);
#if 0//cc mark
	LegacyTxTrackLP4_DDR2667(p);
#endif
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_DQS0,
		p_fld(0x2, SHU_SELPH_DQS0_TXDLY_OEN_DQS3) |
		p_fld(0x2, SHU_SELPH_DQS0_TXDLY_OEN_DQS2) |
		p_fld(0x2, SHU_SELPH_DQS0_TXDLY_OEN_DQS1) |
		p_fld(0x2, SHU_SELPH_DQS0_TXDLY_OEN_DQS0) |
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS3) |
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS2) |
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS1) |
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS0));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_DQS1,
		p_fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS3) |
		p_fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS2) |
		p_fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS1) |
		p_fld(0x6, SHU_SELPH_DQS1_DLY_OEN_DQS0) |
		p_fld(0x1, SHU_SELPH_DQS1_DLY_DQS3) |
		p_fld(0x1, SHU_SELPH_DQS1_DLY_DQS2) |
		p_fld(0x1, SHU_SELPH_DQS1_DLY_DQS1) |
		p_fld(0x1, SHU_SELPH_DQS1_DLY_DQS0));
	if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
		io_32_write_fld_align(DRAMC_REG_SHU_WODT, CLEAR_FLD,
			SHU_WODT_WPST1P5T);
	} else {
		io_32_write_fld_multi(DRAMC_REG_SHU_WODT,
			p_fld(CLEAR_FLD, SHU_WODT_WPST1P5T) |
			p_fld(SET_FLD, SHU_WODT_DBIWR));
	}

	io_32_write_fld_align(DRAMC_REG_SHU_DQS2DQ_TX, 0x7,
		SHU_DQS2DQ_TX_OE2DQ_OFFSET);

	io_32_write_fld_align(DRAMC_REG_SHU_HWSET_MR2, 0x14,
		SHU_HWSET_MR2_HWSET_MR2_OP);
	io_32_write_fld_align(DRAMC_REG_SHU_HWSET_MR13, 0x24,
		SHU_HWSET_MR13_HWSET_MR13_OP);
	io_32_write_fld_align(DRAMC_REG_SHU_HWSET_VRCG, 0x16,
		SHU_HWSET_VRCG_HWSET_VRCG_OP);

	io_32_write_fld_multi(DRAMC_REG_SHURK0_DQSIEN,
		p_fld(0x19, SHURK0_DQSIEN_R0DQS1IEN) |
		p_fld(0x19, SHURK0_DQSIEN_R0DQS0IEN));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_PI,
		p_fld(0x14, SHURK0_PI_RK0_ARPI_DQM_B1) |
		p_fld(0x14, SHURK0_PI_RK0_ARPI_DQM_B0) |
		p_fld(0x14, SHURK0_PI_RK0_ARPI_DQ_B1) |
		p_fld(0x14, SHURK0_PI_RK0_ARPI_DQ_B0));

#if 0//cc mark
	LegacyGatingDlyLP4_DDR2667(p);
#endif

	if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
		io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ0,
			p_fld(0x2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3) |
			p_fld(0x2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) |
			p_fld(0x2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) |
			p_fld(0x2, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) |
			p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ3) |
			p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ2) |
			p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ1) |
			p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ0));
		io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ1,
			p_fld(0x2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3) |
			p_fld(0x2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) |
			p_fld(0x2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) |
			p_fld(0x2, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) |
			p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM3) |
			p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM2) |
			p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM1) |
			p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM0));
	} else {
		io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ0,
			p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3) |
			p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) |
			p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) |
			p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) |
			p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ3) |
			p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ2) |
			p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ1) |
			p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ0));
		io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ1,
			p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3) |
			p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) |
			p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) |
			p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) |
			p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM3) |
			p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM2) |
			p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM1) |
			p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM0));
	}

	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ2,
		p_fld(0x6, SHURK0_SELPH_DQ2_DLY_OEN_DQ3) |
		p_fld(0x6, SHURK0_SELPH_DQ2_DLY_OEN_DQ2) |
		p_fld(0x6, SHURK0_SELPH_DQ2_DLY_OEN_DQ1) |
		p_fld(0x6, SHURK0_SELPH_DQ2_DLY_OEN_DQ0) |
		p_fld(0x2, SHURK0_SELPH_DQ2_DLY_DQ3) |
		p_fld(0x2, SHURK0_SELPH_DQ2_DLY_DQ2) |
		p_fld(0x2, SHURK0_SELPH_DQ2_DLY_DQ1) |
		p_fld(0x2, SHURK0_SELPH_DQ2_DLY_DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ3,
		p_fld(0x6, SHURK0_SELPH_DQ3_DLY_OEN_DQM3) |
		p_fld(0x6, SHURK0_SELPH_DQ3_DLY_OEN_DQM2) |
		p_fld(0x6, SHURK0_SELPH_DQ3_DLY_OEN_DQM1) |
		p_fld(0x6, SHURK0_SELPH_DQ3_DLY_OEN_DQM0) |
		p_fld(0x2, SHURK0_SELPH_DQ3_DLY_DQM3) |
		p_fld(0x2, SHURK0_SELPH_DQ3_DLY_DQM2) |
		p_fld(0x2, SHURK0_SELPH_DQ3_DLY_DQM1) |
		p_fld(0x2, SHURK0_SELPH_DQ3_DLY_DQM0));

	io_32_write_fld_multi(DRAMC_REG_SHURK1_DQSIEN,
		p_fld(0x1f, SHURK1_DQSIEN_R1DQS1IEN) |
		p_fld(0x1f, SHURK1_DQSIEN_R1DQS0IEN));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_PI,
		p_fld(0x14, SHURK1_PI_RK1_ARPI_DQM_B1) |
		p_fld(0x14, SHURK1_PI_RK1_ARPI_DQM_B0) |
		p_fld(0x14, SHURK1_PI_RK1_ARPI_DQ_B1) |
		p_fld(0x14, SHURK1_PI_RK1_ARPI_DQ_B0));
	if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
		io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ0,
			p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3) |
			p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2) |
			p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1) |
			p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0) |
			p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3) |
			p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2) |
			p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1) |
			p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
		io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ1,
			p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3) |
			p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2) |
			p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1) |
			p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0) |
			p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3) |
			p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2) |
			p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1) |
			p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
	} else {
		io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ0,
			p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3) |
			p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2) |
			p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1) |
			p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0) |
			p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3) |
			p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2));
		io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ1,
			p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3) |
			p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2) |
			p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1) |
			p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0) |
			p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3) |
			p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2));
	}
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ2,
		p_fld(CLEAR_FLD, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3) |
		p_fld(CLEAR_FLD, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2) |
		p_fld(CLEAR_FLD, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1) |
		p_fld(CLEAR_FLD, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0) |
		p_fld(0x2, SHURK1_SELPH_DQ2_DLY_R1DQ3) |
		p_fld(0x2, SHURK1_SELPH_DQ2_DLY_R1DQ2) |
		p_fld(0x2, SHURK1_SELPH_DQ2_DLY_R1DQ1) |
		p_fld(0x2, SHURK1_SELPH_DQ2_DLY_R1DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ3,
		p_fld(CLEAR_FLD, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3) |
		p_fld(CLEAR_FLD, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2) |
		p_fld(CLEAR_FLD, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1) |
		p_fld(CLEAR_FLD, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0) |
		p_fld(0x2, SHURK1_SELPH_DQ3_DLY_R1DQM3) |
		p_fld(0x2, SHURK1_SELPH_DQ3_DLY_R1DQM2) |
		p_fld(0x2, SHURK1_SELPH_DQ3_DLY_R1DQM1) |
		p_fld(0x2, SHURK1_SELPH_DQ3_DLY_R1DQM0));
	io_32_write_fld_multi(DRAMC_REG_SHU_DQSG_RETRY,
		p_fld(0x0, SHU_DQSG_RETRY_R_DQSIENLAT) |
		p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_DDR1866_PLUS));

	io_32_write_fld_align(DDRPHY_SHU_B0_DQ5, 0x3,
		SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0);
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU_B0_DQ6,
		p_fld(CLEAR_FLD, SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0) |
	 * | p_fld(SET_FLD, SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0));
	 */
	if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
		io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
			p_fld(CLEAR_FLD, SHU_B0_DQ7_MIDPI_DIV4_ENABLE) |
			p_fld(SET_FLD, SHU_B0_DQ7_MIDPI_ENABLE));
		io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
			p_fld(CLEAR_FLD, SHU_B2_DQ7_MIDPI_DIV4_ENABLE) |
			p_fld(SET_FLD, SHU_B2_DQ7_MIDPI_ENABLE));
	} else {
		io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
			p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) |
			p_fld(SET_FLD, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0) |
			p_fld(CLEAR_FLD, SHU_B0_DQ7_MIDPI_DIV4_ENABLE) |
			p_fld(SET_FLD, SHU_B0_DQ7_MIDPI_ENABLE));
		io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
			p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B2) |
			p_fld(SET_FLD, SHU_B2_DQ7_R_DMDQMDBI_SHU_B2) |
			p_fld(CLEAR_FLD, SHU_B2_DQ7_MIDPI_DIV4_ENABLE) |
			p_fld(SET_FLD, SHU_B2_DQ7_MIDPI_ENABLE));
	}
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ5, 0x3,
		SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1);
	/* cc add. Review the value?? */
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ5, 0x3,
		SHU_B2_DQ5_RG_RX_ARDQS0_DVS_DLY_B2);
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU_B1_DQ6,
		p_fld(CLEAR_FLD, SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1) |
	 * | p_fld(SET_FLD, SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1));
	 */
	if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
	} else {
		io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7,
			p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) |
			p_fld(SET_FLD, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1));
		io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
			p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B2) |
			p_fld(SET_FLD, SHU_B2_DQ7_R_DMDQMDBI_SHU_B2));
	}
	/*
	 * dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	 * io_32_write_fld_multi(DDRPHY_SHU_CA_CMD6,
		p_fld(CLEAR_FLD, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA) |
	 * | p_fld(SET_FLD, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA));
	 * io_32_write_fld_multi(DDRPHY_SHU_CA_CMD6+SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA) |
	 * | p_fld(SET_FLD, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA));
	 * dramc_broadcast_on_off(DRAMC_BROADCAST_ON);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL5, 0x3300, SHU_PLL5_RG_RPHYPLL_SDM_PCW);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL7, 0x3300, SHU_PLL7_RG_RCLRPLL_SDM_PCW);
	 */
	io_32_write_fld_multi(DDRPHY_SHU_R0_B0_DQ7,
		p_fld(0x14, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
		p_fld(0x14, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
	io_32_write_fld_multi(DDRPHY_SHU_R0_B1_DQ7,
		p_fld(0x14, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
		p_fld(0x14, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	io_32_write_fld_multi(DDRPHY_SHU_R0_B2_DQ7,
		p_fld(0x14, SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2) |
		p_fld(0x14, SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B0_DQ7,
		p_fld(0x14, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0) |
		p_fld(0x14, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B1_DQ7,
		p_fld(0x14, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1) |
		p_fld(0x14, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B2_DQ7,
		p_fld(0x14, SHU_R1_B2_DQ7_RK1_ARPI_DQM_B2) |
		p_fld(0x14, SHU_R1_B2_DQ7_RK1_ARPI_DQ_B2));

#if 0 //cc mark
	LegacyRxDly_LP4_DDR2667(p);
#endif
#if 0 //cc mark
	LegacyDlyCellInitLP4_DDR2667(p);
#endif

}

static void dramc_setting_ddr1600(DRAMC_CTX_T *p)
{
	unsigned char reg_txdly_dqs_oen, reg_dly_dqs_oen;
	unsigned char reg_txdly_dqdqm_oen, reg_dly_dqdqm_oen;
	unsigned char reg_txdly_dqs, reg_dly_dqs;
	unsigned char reg_txdly_dqdqm, reg_dly_dqdqm;

	if (get_div_mode(p) == DIV8_MODE) {
		reg_txdly_dqs_oen = 0x1;
		reg_dly_dqs_oen = 0x6;
		reg_txdly_dqs = 0x2;
		reg_dly_dqs = 0x1;

		reg_txdly_dqdqm_oen = 0x1;
		reg_dly_dqdqm_oen = 0x7;
		reg_txdly_dqdqm = 0x2;
		reg_dly_dqdqm = 0x1;
	} else {
		reg_txdly_dqs_oen = 0x3;
		reg_dly_dqs_oen = 0x2;
		reg_txdly_dqs = 0x4;
		reg_dly_dqs = 0x1;

		reg_txdly_dqdqm_oen = 0x3;
		reg_dly_dqdqm_oen = 0x3;
		reg_txdly_dqdqm = 0x4;
		reg_dly_dqdqm = 0x1;
	}
#if 0 //cc mark
	LegacyPreCalLP4_DDR1600(p);
#endif
#if 0 //cc mark
	LegacyTxTrackLP4_DDR1600(p);
#endif

	io_32_write_fld_multi(DRAMC_REG_SHU_RANKCTL,
		p_fld(0x2, SHU_RANKCTL_RANKINCTL_PHY) |
		p_fld(CLEAR_FLD, SHU_RANKCTL_RANKINCTL_ROOT1) |
		p_fld(CLEAR_FLD, SHU_RANKCTL_RANKINCTL));
	io_32_write_fld_multi(DRAMC_REG_SHU_CKECTRL,
		p_fld(0x3, SHU_CKECTRL_TCKESRX));

	io_32_write_fld_multi(DRAMC_REG_SHU_ODTCTRL,
		p_fld(CLEAR_FLD, SHU_ODTCTRL_RODTE) |
		p_fld(CLEAR_FLD, SHU_ODTCTRL_RODTE2) |

#ifdef LOOPBACK_TEST
		p_fld(SET_FLD, SHU_ODTCTRL_ROEN));
#else
		p_fld(CLEAR_FLD, SHU_ODTCTRL_ROEN));
#endif
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ7, CLEAR_FLD,
		SHU_B0_DQ7_R_DMRODTEN_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ7, CLEAR_FLD,
		SHU_B1_DQ7_R_DMRODTEN_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ7, CLEAR_FLD,
		SHU_B2_DQ7_R_DMRODTEN_B2);
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_DQS0,
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS3) |
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS2) |
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS1) |
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS0) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS3) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS2) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS1) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS0));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_DQS1,
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS3) |
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS2) |
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS1) |
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS0) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS3) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS2) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS1) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS0));
	if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
		io_32_write_fld_align(DRAMC_REG_SHU_WODT, CLEAR_FLD,
			SHU_WODT_WPST1P5T);
	} else {
		io_32_write_fld_multi(DRAMC_REG_SHU_WODT,
			p_fld(CLEAR_FLD, SHU_WODT_WPST1P5T) |
			p_fld(CLEAR_FLD, SHU_WODT_DBIWR));
	}

	io_32_write_fld_align(DRAMC_REG_SHU_DQS2DQ_TX, 0x4,
		SHU_DQS2DQ_TX_OE2DQ_OFFSET);

	/*
	 * io_32_write_fld_align(DRAMC_REG_SHU_HWSET_MR2, 0x12, SHU_HWSET_MR2_HWSET_MR2_OP);
	 * io_32_write_fld_align(DRAMC_REG_SHU_HWSET_MR13, 0x8, SHU_HWSET_MR13_HWSET_MR13_OP);
	 * io_32_write_fld_align(DRAMC_REG_SHU_HWSET_VRCG, CLEAR_FLD, SHU_HWSET_VRCG_HWSET_VRCG_OP);
	 */
	io_32_write_fld_multi(DRAMC_REG_SHURK0_DQSIEN,
		p_fld(CLEAR_FLD, SHURK0_DQSIEN_R0DQS1IEN) |
		p_fld(CLEAR_FLD, SHURK0_DQSIEN_R0DQS0IEN));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_PI,
		p_fld(0x1a, SHURK0_PI_RK0_ARPI_DQM_B1) |
		p_fld(0x1a, SHURK0_PI_RK0_ARPI_DQM_B0) |
		p_fld(0x1a, SHURK0_PI_RK0_ARPI_DQ_B1) |
		p_fld(0x1a, SHURK0_PI_RK0_ARPI_DQ_B0));
	if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
		io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ0,
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3) |
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) |
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) |
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ3) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ2) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ1) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ0));
		io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ1,
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3) |
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) |
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) |
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM3) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM2) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM1) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM0));
	} else {
		io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ0,
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3) |
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) |
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) |
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ3) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ2) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ1) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ0));
		io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ1,
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3) |
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) |
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) |
			p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM3) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM2) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM1) |
			p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM0));
	}
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ2,
		p_fld(0x7, SHURK0_SELPH_DQ2_DLY_OEN_DQ3) |
		p_fld(0x7, SHURK0_SELPH_DQ2_DLY_OEN_DQ2) |
		p_fld(0x7, SHURK0_SELPH_DQ2_DLY_OEN_DQ1) |
		p_fld(0x7, SHURK0_SELPH_DQ2_DLY_OEN_DQ0) |
		p_fld(SET_FLD, SHURK0_SELPH_DQ2_DLY_DQ3) |
		p_fld(SET_FLD, SHURK0_SELPH_DQ2_DLY_DQ2) |
		p_fld(SET_FLD, SHURK0_SELPH_DQ2_DLY_DQ1) |
		p_fld(SET_FLD, SHURK0_SELPH_DQ2_DLY_DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ3,
		p_fld(0x7, SHURK0_SELPH_DQ3_DLY_OEN_DQM3) |
		p_fld(0x7, SHURK0_SELPH_DQ3_DLY_OEN_DQM2) |
		p_fld(0x7, SHURK0_SELPH_DQ3_DLY_OEN_DQM1) |
		p_fld(0x7, SHURK0_SELPH_DQ3_DLY_OEN_DQM0) |
		p_fld(SET_FLD, SHURK0_SELPH_DQ3_DLY_DQM3) |
		p_fld(SET_FLD, SHURK0_SELPH_DQ3_DLY_DQM2) |
		p_fld(SET_FLD, SHURK0_SELPH_DQ3_DLY_DQM1) |
		p_fld(SET_FLD, SHURK0_SELPH_DQ3_DLY_DQM0));

	io_32_write_fld_multi(DRAMC_REG_SHURK1_DQSIEN,
		p_fld(CLEAR_FLD, SHURK1_DQSIEN_R1DQS1IEN) |
		p_fld(CLEAR_FLD, SHURK1_DQSIEN_R1DQS0IEN));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_PI,
		p_fld(0x1e, SHURK1_PI_RK1_ARPI_DQM_B1) |
		p_fld(0x1e, SHURK1_PI_RK1_ARPI_DQM_B0) |
		p_fld(0x1e, SHURK1_PI_RK1_ARPI_DQ_B1) |
		p_fld(0x1e, SHURK1_PI_RK1_ARPI_DQ_B0));
#if 0//cc mark
	LegacyGatingDlyLP4_DDR1600(p);
#endif
	if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
		io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ0,
			p_fld(SET_FLD, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3) |
			p_fld(SET_FLD, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2) |
			p_fld(SET_FLD, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1) |
			p_fld(SET_FLD, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0) |
			p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3) |
			p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2) |
			p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1) |
			p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
		io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ1,
			p_fld(SET_FLD, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3) |
			p_fld(SET_FLD, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2) |
			p_fld(SET_FLD, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1) |
			p_fld(SET_FLD, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0) |
			p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3) |
			p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2) |
			p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1) |
			p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
	} else {
		io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ0,
			p_fld(SET_FLD, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3) |
			p_fld(SET_FLD, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2) |
			p_fld(SET_FLD, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1) |
			p_fld(SET_FLD, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0) |
			p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3) |
			p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2));
		io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ1,
			p_fld(SET_FLD, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3) |
			p_fld(SET_FLD, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2) |
			p_fld(SET_FLD, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1) |
			p_fld(SET_FLD, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0) |
			p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3) |
			p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2));
	}
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ2,
		p_fld(0x7, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3) |
		p_fld(0x7, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2) |
		p_fld(0x7, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1) |
		p_fld(0x7, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0) |
		p_fld(SET_FLD, SHURK1_SELPH_DQ2_DLY_R1DQ3) |
		p_fld(SET_FLD, SHURK1_SELPH_DQ2_DLY_R1DQ2) |
		p_fld(SET_FLD, SHURK1_SELPH_DQ2_DLY_R1DQ1) |
		p_fld(SET_FLD, SHURK1_SELPH_DQ2_DLY_R1DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ3,
		p_fld(0x7, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3) |
		p_fld(0x7, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2) |
		p_fld(0x7, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1) |
		p_fld(0x7, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0) |
		p_fld(SET_FLD, SHURK1_SELPH_DQ3_DLY_R1DQM3) |
		p_fld(SET_FLD, SHURK1_SELPH_DQ3_DLY_R1DQM2) |
		p_fld(SET_FLD, SHURK1_SELPH_DQ3_DLY_R1DQM1) |
		p_fld(SET_FLD, SHURK1_SELPH_DQ3_DLY_R1DQM0));
	io_32_write_fld_multi(DRAMC_REG_SHU_DQSG_RETRY,
		p_fld(0x0, SHU_DQSG_RETRY_R_DQSIENLAT) |
		p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_DDR1866_PLUS));

	io_32_write_fld_align(DDRPHY_SHU_B0_DQ5, 0x4,
		SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0);
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU_B0_DQ6,
		p_fld(SET_FLD, SHU_B0_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B0) |
	 * | p_fld(CLEAR_FLD, SHU_B0_DQ6_RG_ARPI_MIDPI_EN_B0));
	 */
	if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
		io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
			p_fld(SET_FLD, SHU_B0_DQ7_MIDPI_DIV4_ENABLE) |
			p_fld(CLEAR_FLD, SHU_B0_DQ7_MIDPI_ENABLE));
		io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
			p_fld(SET_FLD, SHU_B2_DQ7_MIDPI_DIV4_ENABLE) |
			p_fld(CLEAR_FLD, SHU_B2_DQ7_MIDPI_ENABLE));
	}
	else
	{
		io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
			p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) |
			p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMDQMDBI_SHU_B0) |
			p_fld(SET_FLD, SHU_B0_DQ7_MIDPI_DIV4_ENABLE) |
			p_fld(CLEAR_FLD, SHU_B0_DQ7_MIDPI_ENABLE));
		io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
			p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B2) |
			p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMDQMDBI_SHU_B2) |
			p_fld(SET_FLD, SHU_B2_DQ7_MIDPI_DIV4_ENABLE) |
			p_fld(CLEAR_FLD, SHU_B2_DQ7_MIDPI_ENABLE));
	}
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ5, 0x4,
		SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1);

	/* cc note: REVIEW the value */
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ5, 0x4,
		SHU_B2_DQ5_RG_RX_ARDQS0_DVS_DLY_B2);

	/*
	 * io_32_write_fld_multi(DDRPHY_SHU_B1_DQ6,
		p_fld(SET_FLD, SHU_B1_DQ6_RG_ARPI_MIDPI_CKDIV4_EN_B1) |
	 * | p_fld(CLEAR_FLD, SHU_B1_DQ6_RG_ARPI_MIDPI_EN_B1));
	 */
	if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
	} else {
		io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7,
			p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) |
			p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMDQMDBI_SHU_B1));
		io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
			p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B2) |
			p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMDQMDBI_SHU_B2));
	}
	/*
	 * dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	 * io_32_write_fld_multi(DDRPHY_SHU_CA_CMD6,
		p_fld(CLEAR_FLD, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA) |
	 * | p_fld(SET_FLD, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA));
	 * io_32_write_fld_multi(DDRPHY_SHU_CA_CMD6+SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_CA_CMD6_RG_ARPI_MIDPI_CKDIV4_EN_CA) |
	 * | p_fld(SET_FLD, SHU_CA_CMD6_RG_ARPI_MIDPI_EN_CA));
	 * dramc_broadcast_on_off(DRAMC_BROADCAST_ON);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL5, 0x3d00, SHU_PLL5_RG_RPHYPLL_SDM_PCW);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL7, 0x3d00, SHU_PLL7_RG_RCLRPLL_SDM_PCW);
	 */
	io_32_write_fld_multi(DDRPHY_SHU_R0_B0_DQ7,
		p_fld(0x1a, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
		p_fld(0x1a, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
	io_32_write_fld_multi(DDRPHY_SHU_R0_B1_DQ7,
		p_fld(0x1a, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
		p_fld(0x1a, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	io_32_write_fld_multi(DDRPHY_SHU_R0_B2_DQ7,
		p_fld(0x1a, SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2) |
		p_fld(0x1a, SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B0_DQ7,
		p_fld(0x26, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0) |
		p_fld(0x26, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B1_DQ7,
		p_fld(0x26, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1) |
		p_fld(0x26, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B2_DQ7,
		p_fld(0x26, SHU_R1_B2_DQ7_RK1_ARPI_DQM_B2) |
		p_fld(0x26, SHU_R1_B2_DQ7_RK1_ARPI_DQ_B2));
#if 0//cc mark
	LegacyRxDly_LP4_DDR1600(p);
#endif
#if 0//cc mark
	LegacyDlyCellInitLP4_DDR1600(p);
#endif
}

/*[QW]: for lpddr4*/
#if 0 //cc mark to use dvfs_settings( )
void dramc_dvfs_setting(DRAMC_CTX_T *p)
{
	unsigned char dvfs_52m_104m_sel = 0;
	unsigned char dll_idle = 0x30;

	/*
	 * DVFS_SM LP4: dll_idle 90MCK/19.5ns, LP3: 70MCK   (DE: Lynx)
	 * Below values are pre-calculated for each freq and dram type specifically
	 * for 52M DVFS_SM mode (104M should multiply values by 2)
	 * Since this register value is related to dram's operating freq
	 * -> Each freq must use specific pre-calculated value
	 * (Couldn't use formula to calculate this value because floating point numbers are
	 * not supported via preloader)
	 */
	if (is_lp4_family(p)) {
		switch (p->frequency) {
		case 1866:
			dll_idle = 0xa;
			break;
		case 1600:
			dll_idle = 0xc;
			break;
		case 1333:
			dll_idle = 0xd;
			break;
		case 1200:
			dll_idle = 0x10;
			break;
		case 800:
			dll_idle = 0x18;
			break;
		case 400:
			dll_idle = 0x30;
			break;
		default:
			show_err("dll_idle err!\n");
			dll_idle = 0x30;
			break;
		}
	} else {
		switch (p->frequency) {
		case 933:
			dll_idle = 0x8;
			break;
		case 800:
			dll_idle = 0x9;
			break;
		case 600:
			dll_idle = 0xC;
			break;
		case 667:
			dll_idle = 0xC;
			break;
		default:
			show_err("dll_idle err!\n");
			dll_idle = 0xC;
			break;
		}
	}

	dll_idle = (dll_idle << dvfs_52m_104m_sel);

#if NON_EXIST_RG
	io_32_write_fld_align_all(DRAMC_REG_SHUCTRL, SET_FLD,
		SHUCTRL_DVFS_CHB_SEL_B);
#endif

	io_32_write_fld_align_all(DRAMC_REG_DVFSDLL, 1,
		DVFSDLL_R_DDRPHY_SHUFFLE_DEBUG_ENABLE);

	io_32_write_fld_align_all(DDRPHY_DVFS_EMI_CLK, dvfs_52m_104m_sel,
		DVFS_EMI_CLK_RG_52M_104M_SEL);
	io_32_write_fld_align_all(DRAMC_REG_SHUCTRL2, dll_idle,
		SHUCTRL2_R_DLL_IDLE);

#if ENABLE_DVFS_CDC_SYNCHRONIZER_OPTION
	io_32_write_fld_align_all(DRAMC_REG_SHUCTRL2, 1,
		SHUCTRL2_R_DVFS_CDC_OPTION);
	io_32_write_fld_align_all(DRAMC_REG_DVFSDLL, 1,
		DVFSDLL_R_DVFS_SYNC_MODULE_RST_SEL);
	io_32_write_fld_align_all(DRAMC_REG_SHUCTRL2, 1,
		SHUCTRL2_R_CDC_MUX_SEL_OPTION);
#endif

#if ENABLE_BLOCK_APHY_CLOCK_DFS_OPTION
	io_32_write_fld_align_all(DRAMC_REG_DVFSDLL, 1, DVFSDLL_R_DVFS_PICG_POSTPONE);
	io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL6, 1, MISC_CG_CTRL6_ARMCTL_CK_OUT_CG_SEL);
	io_32_write_fld_align_all(DRAMC_REG_SHUCTRL2, 1, SHUCTRL2_SHUFFLE_CHANGE_FREQ_OPT);
#endif

	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL0),
		p_fld(1, MISC_CTRL0_IMPCAL_CDC_ECO_OPT) |
		p_fld(1, MISC_CTRL0_IMPCAL_LP_ECO_OPT) |
		p_fld(1, MISC_CTRL0_IDLE_DCM_CHB_CDC_ECO_OPT));

	io_32_write_fld_align_all(DRAMC_REG_DVFSDLL2,
		0x3, DVFSDLL2_R_DVFS_PICG_MARGIN4_NEW);

#if DVT_8PHASE_UNCERTAINTY_EN
	io_32_write_fld_align_all(DDRPHY_DVFS_EMI_CLK,
		SET_FLD, DVFS_EMI_CLK_R_DDRPHY_SHUFFLE_MUX_ENABLE);
	io_32_write_fld_multi_all(DRAMC_REG_DVFSDLL2,
		p_fld(SET_FLD, DVFSDLL2_R_SHUFFLE_PI_RESET_ENABLE) |
		p_fld(0x3, DVFSDLL2_R_DVFS_MCK8X_MARGIN));
	show_msg((INFO, "[DVT] 8phase Remove Uncertainty Setting Enable\n"));
#endif

	if (is_lp4_family(p)) {
		io_32_write_fld_align_all(DDRPHY_DVFS_EMI_CLK, 1,
			DVFS_EMI_CLK_RG_DLL_SHUFFLE);
		io_32_write_fld_align_all(DRAMC_REG_DVFSDLL, 1,
			DVFSDLL_RG_DLL_SHUFFLE);
	}
}
#endif

static void dramc_setting_lp4(DRAMC_CTX_T *p)
{
	unsigned char cap_sel;
	unsigned char midpicap_sel;
	unsigned char txdly_cmd;

	auto_refresh_cke_off(p);

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);

	io_32_write_fld_multi_all(DDRPHY_CKMUX_SEL,
		p_fld(SET_FLD, CKMUX_SEL_R_PHYCTRLMUX) |
		p_fld(SET_FLD, CKMUX_SEL_R_PHYCTRLDCM));
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, SET_FLD,
		MISC_CG_CTRL0_W_CHG_MEM);

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, CLEAR_FLD,
		MISC_CG_CTRL0_CLK_MEM_SEL);

#if NON_EXIST_RG
	io_32_write_fld_align(DDRPHY_MISC_CTRL0, CLEAR_FLD,
		MISC_CTRL0_R_DMRDSEL_DIV2_OPT);
#endif
	/*
	 * 0 ===LP4_3200_intial_setting_shu1 begin===
	 * Francis : pin mux issue, need to set CHD
	 * TODO: ARDMSUS_10 already set to 0 in SwimpedanceCal(), may be removed here?
	 */
	io_32_write_fld_multi(DDRPHY_MISC_SPM_CTRL1,
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B0) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B1) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B2) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_CA));

	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL2, CLEAR_FLD,
		MISC_SPM_CTRL2_PHY_SPM_CTL2);
	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL0, CLEAR_FLD,
		MISC_SPM_CTRL0_PHY_SPM_CTL0);
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL2, 0x6003bf,
		MISC_CG_CTRL2_RG_MEM_DCM_CTL);
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL4, 0x333f3f00,
		MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL7, 0x63,
		MISC_CG_CTRL7_R_PHY_MCK_CG_CTRL_2);
	io_32_write_fld_multi(DDRPHY_SHU_PLL1,
		p_fld(SET_FLD, SHU_PLL1_R_SHU_AUTO_PLL_MUX) |
		p_fld(0x7, SHU_PLL1_SHU1_PLL1_RFU));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
		p_fld(SET_FLD, SHU_B0_DQ7_MIDPI_ENABLE) |
		p_fld(CLEAR_FLD, SHU_B0_DQ7_MIDPI_DIV4_ENABLE) |
		p_fld(0, SHU_B0_DQ7_R_DMRANKRXDVS_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
		p_fld(SET_FLD, SHU_B2_DQ7_MIDPI_ENABLE) |
		p_fld(CLEAR_FLD, SHU_B2_DQ7_MIDPI_DIV4_ENABLE) |
		p_fld(0, SHU_B2_DQ7_R_DMRANKRXDVS_B2));
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ7, CLEAR_FLD,
		SHU_B1_DQ7_R_DMRANKRXDVS_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ7, CLEAR_FLD,
		SHU_B2_DQ7_R_DMRANKRXDVS_B2);

	io_32_write_fld_align(DDRPHY_MISC_CTRL1, 0x0, MISC_CTRL1_R_DMPINMUX);
	if (p->is_emcp) {
		io_32_write_fld_align(DDRPHY_MISC_CTRL1, SET_FLD,
			MISC_CTRL1_R_DMPINMUX_BIT2);
	}

	/* cc add since this bit default is 1'b1, which will cause CS blocked */
	io_32_write_fld_align(DRAMC_REG_SLP4_TESTMODE, CLEAR_FLD,
		SLP4_TESTMODE_DQM_FIX_LOW);
	io_32_write_fld_multi(DDRPHY_SHU_PLL4,
		p_fld(SET_FLD, SHU_PLL4_RG_RPHYPLL_IBIAS) |
		p_fld(SET_FLD, SHU_PLL4_RG_RPHYPLL_ICHP) |
		p_fld(0x2, SHU_PLL4_RG_RPHYPLL_FS));

	io_32_write_fld_multi(DDRPHY_SHU_PLL6,
		p_fld(SET_FLD, SHU_PLL6_RG_RCLRPLL_IBIAS) |
		p_fld(SET_FLD, SHU_PLL6_RG_RCLRPLL_ICHP) |
		p_fld(0x2, SHU_PLL6_RG_RCLRPLL_FS));
	io_32_write_fld_align(DDRPHY_SHU_PLL14, CLEAR_FLD,
		SHU_PLL14_RG_RPHYPLL_SDM_SSC_PH_INIT);
	io_32_write_fld_align(DDRPHY_SHU_PLL20, CLEAR_FLD,
		SHU_PLL20_RG_RCLRPLL_SDM_SSC_PH_INIT);
	io_32_write_fld_multi(DDRPHY_CA_CMD3,
		p_fld(CLEAR_FLD, CA_CMD3_RG_TX_ARCMD_OE_DIS) |
		p_fld(CLEAR_FLD, CA_CMD3_RG_TX_ARCMD_ODTEN_DIS) |
		p_fld(CLEAR_FLD, CA_CMD3_RG_TX_ARCLK_OE_DIS) |
		p_fld(CLEAR_FLD, CA_CMD3_RG_TX_ARCLK_ODTEN_DIS));
	io_32_write_fld_multi(DDRPHY_B0_DQ2,
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQ_OE_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQ_ODTEN_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQS0_OE_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ2,
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQ_OE_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQ_ODTEN_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQS0_OE_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B1));
	io_32_write_fld_multi(DDRPHY_B2_DQ2,
		p_fld(CLEAR_FLD, B2_DQ2_RG_TX_ARDQ_OE_DIS_B2) |
		p_fld(CLEAR_FLD, B2_DQ2_RG_TX_ARDQ_ODTEN_DIS_B2) |
		p_fld(CLEAR_FLD, B2_DQ2_RG_TX_ARDQS0_OE_DIS_B2) |
		p_fld(CLEAR_FLD, B2_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B2));

	io_32_write_fld_align(DDRPHY_B0_DQ9, SET_FLD,
		B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_R_DMRXDVS_RDSEL_LAT_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD10, CLEAR_FLD,
		CA_CMD10_R_DMRXDVS_RDSEL_LAT_CA);

	/* cc add after MP compare */
#if 1 /* Under debug, use default value for now */
	io_32_write_fld_align(DDRPHY_MISC_VREF_CTRL,
		0xfffe, MISC_VREF_CTRL_PHY_SPM_CTL4);
#endif

	io_32_write_fld_align(DDRPHY_B0_RXDVS0, SET_FLD,
		B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0);
	io_32_write_fld_align(DDRPHY_B1_RXDVS0, SET_FLD,
		B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1);
	io_32_write_fld_align(DDRPHY_B2_RXDVS0, SET_FLD,
		B2_RXDVS0_R_RX_DLY_TRACK_CG_EN_B2);
	io_32_write_fld_align(DDRPHY_B0_RXDVS0, SET_FLD,
		B0_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_RXDVS0, SET_FLD,
		B1_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B1);
	io_32_write_fld_align(DDRPHY_B2_RXDVS0, SET_FLD,
		B2_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B2);
	io_32_write_fld_align(DDRPHY_R0_B0_RXDVS2, SET_FLD,
		R0_B0_RXDVS2_R_RK0_DVS_FDLY_MODE_B0);
	io_32_write_fld_align(DDRPHY_R1_B0_RXDVS2, SET_FLD,
		R1_B0_RXDVS2_R_RK1_DVS_FDLY_MODE_B0);
	io_32_write_fld_align(DDRPHY_R0_B1_RXDVS2, SET_FLD,
		R0_B1_RXDVS2_R_RK0_DVS_FDLY_MODE_B1);
	io_32_write_fld_align(DDRPHY_R1_B1_RXDVS2, SET_FLD,
		R1_B1_RXDVS2_R_RK1_DVS_FDLY_MODE_B1);
	io_32_write_fld_align(DDRPHY_R0_B2_RXDVS2, SET_FLD,
		R0_B2_RXDVS2_R_RK0_DVS_FDLY_MODE_B2);
	io_32_write_fld_align(DDRPHY_R1_B2_RXDVS2, SET_FLD,
		R1_B2_RXDVS2_R_RK1_DVS_FDLY_MODE_B2);
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ5, 0x3,
		SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ5, 0x3,
		SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ5, 0x3,
		SHU_B2_DQ5_RG_RX_ARDQS0_DVS_DLY_B2);
#if 0 //cc mark. Rx delay will depends on calibration value
	LegacyRxDly_LP4_DDR3200(p);
#endif
	io_32_write_fld_multi(DDRPHY_R0_B0_RXDVS1,
		p_fld(0x2, R0_B0_RXDVS1_R_RK0_B0_DVS_TH_LEAD) |
		p_fld(0x2, R0_B0_RXDVS1_R_RK0_B0_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_B0_RXDVS1,
		p_fld(0x2, R1_B0_RXDVS1_R_RK1_B0_DVS_TH_LEAD) |
		p_fld(0x2, R1_B0_RXDVS1_R_RK1_B0_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS1,
		p_fld(0x2, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LEAD) |
		p_fld(0x2, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS1,
		p_fld(0x2, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LEAD) |
		p_fld(0x2, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R0_B2_RXDVS1,
		p_fld(0x2, R0_B2_RXDVS1_R_RK0_B2_DVS_TH_LEAD) |
		p_fld(0x2, R0_B2_RXDVS1_R_RK0_B2_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_B2_RXDVS1,
		p_fld(0x2, R1_B2_RXDVS1_R_RK1_B2_DVS_TH_LEAD) |
		p_fld(0x2, R1_B2_RXDVS1_R_RK1_B2_DVS_TH_LAG));

	io_32_write_fld_multi(DDRPHY_R0_B0_RXDVS2,
		p_fld(0x2, R0_B0_RXDVS2_R_RK0_DVS_MODE_B0) |
		p_fld(SET_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(SET_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi(DDRPHY_R1_B0_RXDVS2,
		p_fld(0x2, R1_B0_RXDVS2_R_RK1_DVS_MODE_B0) |
		p_fld(SET_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(SET_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS2,
		p_fld(0x2, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
		p_fld(SET_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(SET_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS2,
		p_fld(0x2, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1) |
		p_fld(SET_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(SET_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R0_B2_RXDVS2,
		p_fld(0x2, R0_B2_RXDVS2_R_RK0_DVS_MODE_B2) |
		p_fld(SET_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(SET_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B2));
	io_32_write_fld_multi(DDRPHY_R1_B2_RXDVS2,
		p_fld(0x2, R1_B2_RXDVS2_R_RK1_DVS_MODE_B2) |
		p_fld(SET_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(SET_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B2));

	io_32_write_fld_align(DDRPHY_B0_RXDVS0, CLEAR_FLD,
		B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0);
	io_32_write_fld_align(DDRPHY_B1_RXDVS0, CLEAR_FLD,
		B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1);
	io_32_write_fld_align(DDRPHY_B2_RXDVS0, CLEAR_FLD,
		B2_RXDVS0_R_RX_DLY_TRACK_CG_EN_B2);
	io_32_write_fld_align(DDRPHY_B0_DQ9, SET_FLD,
		B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_RG_RX_ARDQ_STBEN_RESETB_B2);
#if 0//cc mark. DlyCell value will depend on calibration values
	LegacyDlyCellInitLP4_DDR3200(p);
#endif
	io_32_write_fld_multi(DDRPHY_SHU_R0_B0_DQ7,
		p_fld(0x1f, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
		p_fld(0x1f, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
	io_32_write_fld_multi(DDRPHY_SHU_R0_B1_DQ7,
		p_fld(0x1f, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
		p_fld(0x1f, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	io_32_write_fld_multi(DDRPHY_SHU_R0_B2_DQ7,
		p_fld(0x1f, SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2) |
		p_fld(0x1f, SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B0_DQ7,
		p_fld(0x1f, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0) |
		p_fld(0x1f, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B1_DQ7,
		p_fld(0x1f, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1) |
		p_fld(0x1f, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B2_DQ7,
		p_fld(0x1f, SHU_R1_B2_DQ7_RK1_ARPI_DQM_B2) |
		p_fld(0x1f, SHU_R1_B2_DQ7_RK1_ARPI_DQ_B2));

	io_32_write_fld_multi(DDRPHY_B0_DQ4,
		p_fld(0x10, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0) |
		p_fld(0x10, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0));
	io_32_write_fld_multi(DDRPHY_B0_DQ5,
		p_fld(CLEAR_FLD, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0) |
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_EYE_SEL_B0) |
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0) |
		p_fld(0xe, B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0) |
		p_fld(0xe, B0_DQ5_B0_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B0_DQ6,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_TX_ARDQ_DDR3_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_TX_ARDQ_DDR4_SEL_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_DDR4_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0));

	io_32_write_fld_multi(DDRPHY_B0_DQ5,
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0) |
		p_fld(0x0e, B0_DQ5_B0_DQ5_RFU));

	io_32_write_fld_multi(DDRPHY_B1_DQ4,
		p_fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1) |
		p_fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ5,
		p_fld(CLEAR_FLD, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1) |
		p_fld(0xe, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1) |
		p_fld(0x0e, B1_DQ5_B1_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1) |
		p_fld(CLEAR_FLD, B1_DQ6_RG_TX_ARDQ_DDR3_SEL_B1) |
		p_fld(CLEAR_FLD, B1_DQ6_RG_RX_ARDQ_DDR3_SEL_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_TX_ARDQ_DDR4_SEL_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_DDR4_SEL_B1) |
		p_fld(CLEAR_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ5,
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) |
		p_fld(0x0e, B1_DQ5_B1_DQ5_RFU));

	io_32_write_fld_multi(DDRPHY_B2_DQ4,
		p_fld(0x10, B2_DQ4_RG_RX_ARDQS_EYE_R_DLY_B2) |
		p_fld(0x10, B2_DQ4_RG_RX_ARDQS_EYE_F_DLY_B2));
	io_32_write_fld_multi(DDRPHY_B2_DQ5,
		p_fld(CLEAR_FLD, B2_DQ5_RG_RX_ARDQ_EYE_EN_B2) |
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_EYE_SEL_B2) |
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_VREF_EN_B2) |
		p_fld(0xe, B2_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B2) |
		p_fld(0x0e, B2_DQ5_B2_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B2_DQ6,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B2) |
		p_fld(CLEAR_FLD, B2_DQ6_RG_TX_ARDQ_DDR3_SEL_B2) |
		p_fld(CLEAR_FLD, B2_DQ6_RG_RX_ARDQ_DDR3_SEL_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_TX_ARDQ_DDR4_SEL_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_DDR4_SEL_B2) |
		p_fld(CLEAR_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2));

	io_32_write_fld_multi(DDRPHY_B2_DQ5,
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B2) |
		p_fld(0x0e, B2_DQ5_B2_DQ5_RFU));

	io_32_write_fld_multi(DDRPHY_CA_CMD3,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_IN_BUFF_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_ARCMD_RESETB) |
		p_fld(SET_FLD, CA_CMD3_RG_TX_ARCMD_EN));
	io_32_write_fld_multi(DDRPHY_CA_CMD6,
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_VREF_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN));

	if (get_div_mode(p) == DIV8_MODE) {
		io_32_write_fld_align(DDRPHY_SHU_B0_DQ6, CLEAR_FLD,
			SHU_B0_DQ6_RG_TX_ARDQ_SER_MODE_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ6, CLEAR_FLD,
			SHU_B1_DQ6_RG_TX_ARDQ_SER_MODE_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DQ6, CLEAR_FLD,
			SHU_B2_DQ6_RG_TX_ARDQ_SER_MODE_B2);
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD6, CLEAR_FLD,
			SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE);

		dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
		io_32_write_fld_align_all(DDRPHY_SHU_CA_DLL0, 0x2,
			SHU_CA_DLL0_RG_ARDLL_DIV_MCTL_CA);
		io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL0, 0x2,
			SHU_B0_DLL0_RG_ARDLL_DIV_MCTL_B0);
		io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL0, 0x2,
			SHU_B1_DLL0_RG_ARDLL_DIV_MCTL_B1);
		io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL0, 0x2,
			SHU_B2_DLL0_RG_ARDLL_DIV_MCTL_B2);
		dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

		io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
			p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRDSEL_DIV2_OPT) |
			p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRDSEL_LOBYTE_OPT));
	} else {
		io_32_write_fld_align(DDRPHY_SHU_B0_DQ6, SET_FLD,
			SHU_B0_DQ6_RG_TX_ARDQ_SER_MODE_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ6, SET_FLD,
			SHU_B1_DQ6_RG_TX_ARDQ_SER_MODE_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DQ6, SET_FLD,
			SHU_B2_DQ6_RG_TX_ARDQ_SER_MODE_B2);
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD6, SET_FLD,
			SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE);

		dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
		io_32_write_fld_align_all(DDRPHY_SHU_CA_DLL0, CLEAR_FLD,
			SHU_CA_DLL0_RG_ARDLL_DIV_MCTL_CA);
		io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL0, CLEAR_FLD,
			SHU_B0_DLL0_RG_ARDLL_DIV_MCTL_B0);
		io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL0, CLEAR_FLD,
			SHU_B1_DLL0_RG_ARDLL_DIV_MCTL_B1);
		io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL0, CLEAR_FLD,
			SHU_B2_DLL0_RG_ARDLL_DIV_MCTL_B2);
		dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

		io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
			p_fld(SET_FLD, SHU_B0_DQ7_R_DMRDSEL_DIV2_OPT) |
			p_fld(SET_FLD, SHU_B0_DQ7_R_DMRDSEL_LOBYTE_OPT));
		io_32_write_fld_multi(DRAMC_REG_SHU_PIPE,
			p_fld(0x1, SHU_PIPE_READ_START_EXTEND1) |
			p_fld(0x1, SHU_PIPE_DLE_LAST_EXTEND1) |
			p_fld(0x1, SHU_PIPE_READ_START_EXTEND2) |
			p_fld(0x1, SHU_PIPE_DLE_LAST_EXTEND2) |
			p_fld(0x0, SHU_PIPE_READ_START_EXTEND3) |
			p_fld(0x0, SHU_PIPE_DLE_LAST_EXTEND3));
	}

	io_32_write_fld_multi(DDRPHY_PLL3,
		p_fld(CLEAR_FLD, PLL3_RG_RPHYPLL_TSTOP_EN) |
		p_fld(CLEAR_FLD, PLL3_RG_RPHYPLL_TST_EN));
	io_32_write_fld_align(DDRPHY_MISC_VREF_CTRL, SET_FLD,
		MISC_VREF_CTRL_RG_RVREF_VREF_EN);
	io_32_write_fld_multi_all(DDRPHY_SHU_MISC0,
		p_fld(CLEAR_FLD, SHU_MISC0_RG_RVREF_SEL_CMD) |
		p_fld(CLEAR_FLD, SHU_MISC0_RG_RVREF_DDR3_SEL) |
		p_fld(SET_FLD, SHU_MISC0_RG_RVREF_DDR4_SEL) |
		p_fld(0x0, SHU_MISC0_RG_RVREF_SEL_DQ));

	io_32_write_fld_align(DDRPHY_B0_DQ3, SET_FLD,
		B0_DQ3_RG_ARDQ_RESETB_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ3, SET_FLD,
		B1_DQ3_RG_ARDQ_RESETB_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ3, SET_FLD,
		B2_DQ3_RG_ARDQ_RESETB_B2);

	delay_us(1);

	io_32_write_fld_multi(DDRPHY_SHU_PLL8,
		p_fld(CLEAR_FLD, SHU_PLL8_RG_RPHYPLL_POSDIV) |
		p_fld(SET_FLD, SHU_PLL8_RG_RPHYPLL_PREDIV));

	delay_us(1);

	io_32_write_fld_multi(DDRPHY_SHU_PLL9,
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_MONCK_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_MONVC_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_LVROD_EN) |
		p_fld(0x1, SHU_PLL9_RG_RPHYPLL_RST_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_PLL11,
		p_fld(CLEAR_FLD, SHU_PLL11_RG_RCLRPLL_MONCK_EN) |
		p_fld(CLEAR_FLD, SHU_PLL11_RG_RCLRPLL_MONVC_EN) |
		p_fld(CLEAR_FLD, SHU_PLL11_RG_RCLRPLL_LVROD_EN) |
		p_fld(0x1, SHU_PLL11_RG_RCLRPLL_RST_DLY));

	delay_us(1);

	io_32_write_fld_multi(DDRPHY_SHU_PLL10,
		p_fld(CLEAR_FLD, SHU_PLL10_RG_RCLRPLL_POSDIV) |
		p_fld(SET_FLD, SHU_PLL10_RG_RCLRPLL_PREDIV));

	delay_us(1);

	io_32_write_fld_multi(DDRPHY_PLL4,
		p_fld(CLEAR_FLD, PLL4_RG_RPHYPLL_AD_MCK8X_EN) |
		p_fld(SET_FLD, PLL4_PLL4_RFU) |
		p_fld(SET_FLD, PLL4_RG_RPHYPLL_MCK8X_SEL));

	delay_us(1);

	io_32_write_fld_align(DDRPHY_SHU_PLL0, 0x3,
		SHU_PLL0_RG_RPHYPLL_TOP_REV);

	delay_us(1);

	io_32_write_fld_align(DDRPHY_CA_DLL_ARPI1, CLEAR_FLD,
		CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA);
	io_32_write_fld_multi(DDRPHY_B0_DQ3,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_STBENCMP_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_TX_ARDQ_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ3,
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_STBENCMP_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_TX_ARDQ_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1));
	io_32_write_fld_multi(DDRPHY_B2_DQ3,
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQ_STBENCMP_EN_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_TX_ARDQ_EN_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQ_SMT_EN_B2));

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_align(DDRPHY_SHU_CA_DLL0, SET_FLD,
		SHU_CA_DLL0_RG_ARPISM_MCK_SEL_CA_SHU);
	io_32_write_fld_align(DDRPHY_SHU_CA_DLL0 + SHIFT_TO_CHB_ADDR,
		SET_FLD, SHU_CA_DLL0_RG_ARPISM_MCK_SEL_CA_SHU);
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	io_32_write_fld_multi(DDRPHY_SHU_B0_DLL0,
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHDET_IN_SWAP_B0) |
		p_fld(0x7, SHU_B0_DLL0_RG_ARDLL_GAIN_B0) |
		p_fld(0x7, SHU_B0_DLL0_RG_ARDLL_IDLECNT_B0) |
		p_fld(0x8, SHU_B0_DLL0_RG_ARDLL_P_GAIN_B0) |
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHJUMP_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHDIV_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DLL0_RG_ARDLL_FAST_PSJP_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DLL0,
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHDET_IN_SWAP_B1) |
		p_fld(0x7, SHU_B1_DLL0_RG_ARDLL_GAIN_B1) |
		p_fld(0x7, SHU_B1_DLL0_RG_ARDLL_IDLECNT_B1) |
		p_fld(0x8, SHU_B1_DLL0_RG_ARDLL_P_GAIN_B1) |
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHJUMP_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHDIV_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DLL0_RG_ARDLL_FAST_PSJP_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DLL0,
		p_fld(SET_FLD, SHU_B2_DLL0_RG_ARDLL_PHDET_IN_SWAP_B2) |
		p_fld(0x7, SHU_B2_DLL0_RG_ARDLL_GAIN_B2) |
		p_fld(0x7, SHU_B2_DLL0_RG_ARDLL_IDLECNT_B2) |
		p_fld(0x8, SHU_B2_DLL0_RG_ARDLL_P_GAIN_B2) |
		p_fld(SET_FLD, SHU_B2_DLL0_RG_ARDLL_PHJUMP_EN_B2) |
		p_fld(SET_FLD, SHU_B2_DLL0_RG_ARDLL_PHDIV_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DLL0_RG_ARDLL_FAST_PSJP_B2));

	io_32_write_fld_align(DDRPHY_SHU_CA_CMD5, CLEAR_FLD,
		SHU_CA_CMD5_RG_RX_ARCMD_VREF_SEL);


	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD0,
		p_fld(SET_FLD, SHU_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
		p_fld(0x4, SHU_CA_CMD0_RG_TX_ARCLK_DRVN_PRE) |
		p_fld(SET_FLD, SHU_CA_CMD0_RG_TX_ARCLK_PRE_EN));

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_multi(DDRPHY_MISC_SHU_OPT,
		p_fld(0x2, MISC_SHU_OPT_R_DQB2_SHU_PHDET_SPM_EN) |
		p_fld(0x1, MISC_SHU_OPT_R_DQB2_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB1_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB0_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN));
	io_32_write_fld_multi(DDRPHY_MISC_SHU_OPT + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, MISC_SHU_OPT_R_DQB2_SHU_PHDET_SPM_EN) |
		p_fld(0x1, MISC_SHU_OPT_R_DQB2_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB1_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB0_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN));

	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	delay_us(9);

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_multi(DDRPHY_SHU_CA_DLL1,
		p_fld(SET_FLD, SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA) |
		p_fld(CLEAR_FLD, SHU_CA_DLL1_RG_ARDLL_FASTPJ_CK_SEL_CA));
	io_32_write_fld_multi(DDRPHY_SHU_CA_DLL1 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA) |
		p_fld(SET_FLD, SHU_CA_DLL1_RG_ARDLL_FASTPJ_CK_SEL_CA));
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	io_32_write_fld_multi(DDRPHY_SHU_B0_DLL1,
		p_fld(CLEAR_FLD, SHU_B0_DLL1_RG_ARDLL_PD_CK_SEL_B0) |
		p_fld(SET_FLD, SHU_B0_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DLL1,
		p_fld(CLEAR_FLD, SHU_B1_DLL1_RG_ARDLL_PD_CK_SEL_B1) |
		p_fld(SET_FLD, SHU_B1_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DLL1,
		p_fld(CLEAR_FLD, SHU_B2_DLL1_RG_ARDLL_PD_CK_SEL_B2) |
		p_fld(SET_FLD, SHU_B2_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B2));

	delay_us(1);

	io_32_write_fld_align(DDRPHY_PLL2, CLEAR_FLD, PLL2_RG_RCLRPLL_EN);

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, 0xf,
		MISC_CG_CTRL0_CLK_MEM_DFS_CFG);

	delay_us(1);

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	ddrphy_rsv_rg_setting(p);
	ddrphy_pll_setting(p);
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

#if ENABLE_TMRRI_NEW_MODE

	io_32_write_fld_align(DRAMC_REG_RKCFG, 0, RKCFG_TXRANK);
	io_32_write_fld_align(DRAMC_REG_RKCFG, 1, RKCFG_TXRANKFIX);
	io_32_write_fld_align(DRAMC_REG_DRSCTRL, CLEAR_FLD,
		DRSCTRL_RK_SCINPUT_OPT);
	io_32_write_fld_multi(DRAMC_REG_DRAMCTRL,
		p_fld(CLEAR_FLD, DRAMCTRL_MRRIOPT) |
		p_fld(CLEAR_FLD, DRAMCTRL_TMRRIBYRK_DIS) |
		p_fld(SET_FLD, DRAMCTRL_TMRRICHKDIS));
	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, SET_FLD,
		SPCMDCTRL_SC_PG_UPD_OPT);
	io_32_write_fld_multi(DRAMC_REG_SPCMDCTRL,
		p_fld(CLEAR_FLD, SPCMDCTRL_SC_PG_MPRW_DIS) |
		p_fld(CLEAR_FLD, SPCMDCTRL_SC_PG_STCMD_AREF_DIS) |
		p_fld(CLEAR_FLD, SPCMDCTRL_SC_PG_OPT2_DIS) |
		p_fld(CLEAR_FLD, SPCMDCTRL_SC_PG_MAN_DIS));
	io_32_write_fld_multi(DRAMC_REG_MPC_OPTION,
		p_fld(SET_FLD, MPC_OPTION_ZQ_BLOCKALE_OPT) |
		p_fld(SET_FLD, MPC_OPTION_MPC_BLOCKALE_OPT2) |
		p_fld(SET_FLD, MPC_OPTION_MPC_BLOCKALE_OPT1) |
		p_fld(SET_FLD, MPC_OPTION_MPC_BLOCKALE_OPT));

	io_32_write_fld_align(DRAMC_REG_RKCFG, 0, RKCFG_TXRANK);
	io_32_write_fld_align(DRAMC_REG_RKCFG, 0, RKCFG_TXRANKFIX);
#else
	io_32_write_fld_align(DRAMC_REG_DRSCTRL, SET_FLD,
		DRSCTRL_RK_SCINPUT_OPT);
	io_32_write_fld_multi(DRAMC_REG_DRAMCTRL,
		p_fld(SET_FLD, DRAMCTRL_MRRIOPT) |
		p_fld(SET_FLD, DRAMCTRL_TMRRIBYRK_DIS) |
		p_fld(CLEAR_FLD, DRAMCTRL_TMRRICHKDIS));
	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, CLEAR_FLD,
		SPCMDCTRL_SC_PG_UPD_OPT);
	io_32_write_fld_multi(DRAMC_REG_SPCMDCTRL,
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_MPRW_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_STCMD_AREF_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_OPT2_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_MAN_DIS));
#endif
	io_32_write_fld_align(DRAMC_REG_CKECTRL, SET_FLD,
		CKECTRL_RUNTIMEMRRCKEFIX);
	io_32_write_fld_align(DRAMC_REG_CKECTRL, CLEAR_FLD,
		CKECTRL_RUNTIMEMRRMIODIS);

	io_32_write_fld_align(DDRPHY_B0_DQ9, SET_FLD,
		B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B2);
	io_32_write_fld_multi(DRAMC_REG_SHURK1_DQSIEN,
		p_fld(0xf, SHURK1_DQSIEN_R1DQS3IEN) |
		p_fld(0xf, SHURK1_DQSIEN_R1DQS2IEN) |
		p_fld(0xf, SHURK1_DQSIEN_R1DQS1IEN) |
		p_fld(0xf, SHURK1_DQSIEN_R1DQS0IEN));
	io_32_write_fld_multi(DRAMC_REG_STBCAL1,
		p_fld(CLEAR_FLD, STBCAL1_DLLFRZ_MON_PBREF_OPT) |
		p_fld(SET_FLD, STBCAL1_STB_FLAGCLR) |
		p_fld(SET_FLD, STBCAL1_STBCNT_MODESEL));
	io_32_write_fld_multi(DRAMC_REG_SHU_DQSG_RETRY,
		p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_RETRY_USE_BURST_MDOE) |
		p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_RDY_SEL_DLE) |
		p_fld(0x0, SHU_DQSG_RETRY_R_DQSIENLAT) |
		p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_RETRY_ONCE));
	io_32_write_fld_multi(DRAMC_REG_SHU_DRVING1,
		p_fld(!p->odt_onoff, SHU_DRVING1_DIS_IMP_ODTN_TRACK) |
		p_fld(!p->odt_onoff, SHU_DRVING1_DIS_IMPCAL_HW));
	io_32_write_fld_align(DRAMC_REG_SHU_DRVING2,
		!p->odt_onoff, SHU_DRVING2_DIS_IMPCAL_ODT_EN);
	io_32_write_fld_multi(DRAMC_REG_SHU_DRVING1,
		p_fld(0xa, SHU_DRVING1_DQSDRVP2) |
		p_fld(0xa, SHU_DRVING1_DQSDRVN2) |
		p_fld(0xa, SHU_DRVING1_DQSDRVP1) |
		p_fld(0xa, SHU_DRVING1_DQSDRVN1) |
		p_fld(0xa, SHU_DRVING1_DQDRVP2) |
		p_fld(0xa, SHU_DRVING1_DQDRVN2));
	io_32_write_fld_multi(DRAMC_REG_SHU_DRVING2,
		p_fld(0xa, SHU_DRVING2_DQDRVP1) |
		p_fld(0xa, SHU_DRVING2_DQDRVN1) |
		p_fld(0xa, SHU_DRVING2_CMDDRVP2) |
		p_fld(0xa, SHU_DRVING2_CMDDRVN2) |
		p_fld(0xa, SHU_DRVING2_CMDDRVP1) |
		p_fld(0xa, SHU_DRVING2_CMDDRVN1));
	io_32_write_fld_multi(DRAMC_REG_SHU_DRVING3,
		p_fld(0xa, SHU_DRVING3_DQSODTP2) |
		p_fld(0xa, SHU_DRVING3_DQSODTN2) |
		p_fld(0xa, SHU_DRVING3_DQSODTP) |
		p_fld(0xa, SHU_DRVING3_DQSODTN) |
		p_fld(0xa, SHU_DRVING3_DQODTP2) |
		p_fld(0xa, SHU_DRVING3_DQODTN2));
	io_32_write_fld_multi(DRAMC_REG_SHU_DRVING4,
		p_fld(0xa, SHU_DRVING4_DQODTP1) |
		p_fld(0xa, SHU_DRVING4_DQODTN1) |
		p_fld(0xa, SHU_DRVING4_CMDODTP2) |
		p_fld(0xa, SHU_DRVING4_CMDODTN2) |
		p_fld(0xa, SHU_DRVING4_CMDODTP1) |
		p_fld(0xa, SHU_DRVING4_CMDODTN1));

	io_32_write_fld_multi(DRAMC_REG_SHUCTRL2,
		p_fld(CLEAR_FLD, SHUCTRL2_HWSET_WLRL) |
		p_fld(SET_FLD, SHUCTRL2_SHU_PERIOD_GO_ZERO_CNT) |
		p_fld(CLEAR_FLD, SHUCTRL2_R_DVFS_OPTION) |
		p_fld(CLEAR_FLD, SHUCTRL2_R_DVFS_PARK_N) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_DLL_CHA) |
		p_fld(0xa, SHUCTRL2_R_DLL_IDLE));
	io_32_write_fld_align(DRAMC_REG_DVFSDLL, SET_FLD,
		DVFSDLL_DLL_LOCK_SHU_EN);
	io_32_write_fld_multi(DRAMC_REG_DDRCONF0,
		p_fld(SET_FLD, DDRCONF0_LPDDR4EN) |
		p_fld(SET_FLD, DDRCONF0_BC4OTF) |
		p_fld(SET_FLD, DDRCONF0_BK8EN));

	io_32_write_fld_multi(DRAMC_REG_STBCAL2,
		p_fld(SET_FLD, STBCAL2_STB_GERR_B01) |
		p_fld(SET_FLD, STBCAL2_STB_GERRSTOP) |
		p_fld(SET_FLD, EYESCAN_EYESCAN_RD_SEL_OPT));
	io_32_write_fld_align(DRAMC_REG_STBCAL2, SET_FLD,
		STBCAL2_STB_GERR_RST);
	io_32_write_fld_align(DRAMC_REG_STBCAL2, CLEAR_FLD,
		STBCAL2_STB_GERR_RST);
	io_32_write_fld_align(DRAMC_REG_CLKAR, SET_FLD, CLKAR_PSELAR);
	io_32_write_fld_align(DDRPHY_B0_DQ9, SET_FLD,
		B0_DQ9_R_DMDQSIEN_RDSEL_LAT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_R_DMDQSIEN_RDSEL_LAT_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_R_DMDQSIEN_RDSEL_LAT_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD10, CLEAR_FLD,
		CA_CMD10_R_DMDQSIEN_RDSEL_LAT_CA);
	if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
		io_32_write_fld_multi(DDRPHY_MISC_CTRL0,
			p_fld(SET_FLD, MISC_CTRL0_R_STBENCMP_DIV4CK_EN) |
			p_fld(SET_FLD, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
			p_fld(SET_FLD, MISC_CTRL0_R_DMSTBEN_OUTSEL) |
			p_fld(0xf, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));
	} else {
		io_32_write_fld_multi(DDRPHY_MISC_CTRL0,
			p_fld(SET_FLD, MISC_CTRL0_R_STBENCMP_DIV4CK_EN) |
			p_fld(SET_FLD, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
			p_fld(SET_FLD, MISC_CTRL0_R_DMSTBEN_OUTSEL) |
			p_fld(0xf, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));
	}

	io_32_write_fld_multi(DDRPHY_MISC_CTRL1,
		p_fld(SET_FLD, MISC_CTRL1_R_DMDQSIENCG_EN) |
		p_fld(SET_FLD, MISC_CTRL1_R_DM_TX_ARCMD_OE) |
		p_fld(SET_FLD, MISC_CTRL1_R_DM_TX_ARCLK_OE));
	io_32_write_fld_align(DDRPHY_B0_RXDVS0, 1,
		B0_RXDVS0_R_HWSAVE_MODE_ENA_B0);
	io_32_write_fld_align(DDRPHY_B1_RXDVS0, 1,
		B1_RXDVS0_R_HWSAVE_MODE_ENA_B1);
	io_32_write_fld_align(DDRPHY_B2_RXDVS0, 1,
		B2_RXDVS0_R_HWSAVE_MODE_ENA_B2);
	io_32_write_fld_align(DDRPHY_CA_RXDVS0, 0,
		CA_RXDVS0_R_HWSAVE_MODE_ENA_CA);

	io_32_write_fld_align(DDRPHY_CA_CMD7, CLEAR_FLD,
		CA_CMD7_RG_TX_ARCMD_PULL_DN);
	io_32_write_fld_align(DDRPHY_CA_CMD7, CLEAR_FLD,
		CA_CMD7_RG_TX_ARCS_PULL_DN);
	io_32_write_fld_align(DDRPHY_B0_DQ7, CLEAR_FLD,
		B0_DQ7_RG_TX_ARDQ_PULL_DN_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ7, CLEAR_FLD,
		B1_DQ7_RG_TX_ARDQ_PULL_DN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ7, CLEAR_FLD,
		B2_DQ7_RG_TX_ARDQ_PULL_DN_B2);

	if (get_div_mode(p) == DIV8_MODE) {
		io_32_write_fld_multi(DRAMC_REG_SHU_CONF0,
			p_fld(0x2, SHU_CONF0_MATYPE) |
			p_fld(SET_FLD, SHU_CONF0_BL4) |
			p_fld(SET_FLD, SHU_CONF0_FREQDIV4) |
			p_fld(CLEAR_FLD, SHU_CONF0_FDIV2) |
			p_fld(SET_FLD, SHU_CONF0_REFTHD) |
			p_fld(SET_FLD, SHU_CONF0_ADVPREEN) |
			p_fld(SET_FLD, SHU_CONF0_DM64BITEN) |
			p_fld(CLEAR_FLD, SHU_CONF0_DUALSCHEN) |
			p_fld(CLEAR_FLD, SHU_CONF0_WDATRGO) |
			p_fld(0x3f, SHU_CONF0_DMPGTIM));
	} else {
		io_32_write_fld_multi(DRAMC_REG_SHU_CONF0,
			p_fld(0x2, SHU_CONF0_MATYPE) |
			p_fld(SET_FLD, SHU_CONF0_BL4) |
			p_fld(CLEAR_FLD, SHU_CONF0_FREQDIV4) |
			p_fld(SET_FLD, SHU_CONF0_FDIV2) |
			p_fld(SET_FLD, SHU_CONF0_REFTHD) |
			p_fld(SET_FLD, SHU_CONF0_ADVPREEN) |
			p_fld(CLEAR_FLD, SHU_CONF0_DM64BITEN) |
			p_fld(CLEAR_FLD, SHU_CONF0_DUALSCHEN) |
			p_fld(SET_FLD, SHU_CONF0_WDATRGO) |
			p_fld(0x3f, SHU_CONF0_DMPGTIM));
	}
	io_32_write_fld_multi(DRAMC_REG_SHU_ODTCTRL,
		p_fld(SET_FLD, SHU_ODTCTRL_RODTE) |
		p_fld(SET_FLD, SHU_ODTCTRL_RODTE2) |
		p_fld(SET_FLD, SHU_ODTCTRL_TWODT) |

		p_fld(SET_FLD, SHU_ODTCTRL_WOEN) |
		p_fld(SET_FLD, SHU_ODTCTRL_ROEN));
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ7, SET_FLD,
		SHU_B0_DQ7_R_DMRODTEN_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ7, SET_FLD,
		SHU_B1_DQ7_R_DMRODTEN_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ7, SET_FLD,
		SHU_B2_DQ7_R_DMRODTEN_B2);
	io_32_write_fld_align(DRAMC_REG_REFCTRL0, 0x5,
		REFCTRL0_REF_PREGATE_CNT);
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA1,
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CS1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_RAS) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CAS) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_WE) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_RESET) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_ODT) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CKE) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CS));

	if (p->frequency < 1333)
		txdly_cmd = 0x7;
	else if (p->frequency < 1600)
		txdly_cmd = 0x8;
	else if (p->frequency < 1866)
		txdly_cmd = 0x9;
	else if (p->frequency < 2133)
		txdly_cmd = 0xA;
	else
		txdly_cmd = 0xB;

	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA2,
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_CKE1) |
		p_fld(txdly_cmd, SHU_SELPH_CA2_TXDLY_CMD) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_BA2) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_BA1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_BA0));

	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA3,
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA7) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA6) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA5) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA4) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA3) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA2) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA0));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA4,
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA15) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA14) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA13) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA12) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA11) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA10) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA9) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA8));
	io_32_write_fld_align(DRAMC_REG_SHU_SELPH_CA5, CLEAR_FLD,
		SHU_SELPH_CA5_DLY_ODT);
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_DQS0,
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS3) |
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS2) |
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS1) |
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_OEN_DQS0) |
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS3) |
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS2) |
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS1) |
		p_fld(0x3, SHU_SELPH_DQS0_TXDLY_DQS0));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_DQS1,
		p_fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS3) |
		p_fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS2) |
		p_fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS1) |
		p_fld(0x2, SHU_SELPH_DQS1_DLY_OEN_DQS0) |
		p_fld(0x5, SHU_SELPH_DQS1_DLY_DQS3) |
		p_fld(0x5, SHU_SELPH_DQS1_DLY_DQS2) |
		p_fld(0x5, SHU_SELPH_DQS1_DLY_DQS1) |
		p_fld(0x5, SHU_SELPH_DQS1_DLY_DQS0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ0,
		p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3) |
		p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) |
		p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) |
		p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) |
		p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ3) |
		p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ2) |
		p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ1) |
		p_fld(0x3, SHURK0_SELPH_DQ0_TXDLY_DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ1,
		p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3) |
		p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) |
		p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) |
		p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) |
		p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM3) |
		p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM2) |
		p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM1) |
		p_fld(0x3, SHURK0_SELPH_DQ1_TXDLY_DQM0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ2,
		p_fld(0x2, SHURK0_SELPH_DQ2_DLY_OEN_DQ3) |
		p_fld(0x2, SHURK0_SELPH_DQ2_DLY_OEN_DQ2) |
		p_fld(0x2, SHURK0_SELPH_DQ2_DLY_OEN_DQ1) |
		p_fld(0x2, SHURK0_SELPH_DQ2_DLY_OEN_DQ0) |
		p_fld(0x6, SHURK0_SELPH_DQ2_DLY_DQ3) |
		p_fld(0x6, SHURK0_SELPH_DQ2_DLY_DQ2) |
		p_fld(0x6, SHURK0_SELPH_DQ2_DLY_DQ1) |
		p_fld(0x6, SHURK0_SELPH_DQ2_DLY_DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ3,
		p_fld(0x2, SHURK0_SELPH_DQ3_DLY_OEN_DQM3) |
		p_fld(0x2, SHURK0_SELPH_DQ3_DLY_OEN_DQM2) |
		p_fld(0x2, SHURK0_SELPH_DQ3_DLY_OEN_DQM1) |
		p_fld(0x2, SHURK0_SELPH_DQ3_DLY_OEN_DQM0) |
		p_fld(0x6, SHURK0_SELPH_DQ3_DLY_DQM3) |
		p_fld(0x6, SHURK0_SELPH_DQ3_DLY_DQM2) |
		p_fld(0x6, SHURK0_SELPH_DQ3_DLY_DQM1) |
		p_fld(0x6, SHURK0_SELPH_DQ3_DLY_DQM0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ0,
		p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3) |
		p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2) |
		p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1) |
		p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0) |
		p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3) |
		p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2) |
		p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1) |
		p_fld(0x3, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ1,
		p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3) |
		p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2) |
		p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1) |
		p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0) |
		p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3) |
		p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2) |
		p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1) |
		p_fld(0x3, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ2,
		p_fld(0x2, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3) |
		p_fld(0x2, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2) |
		p_fld(0x2, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1) |
		p_fld(0x2, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0) |
		p_fld(0x6, SHURK1_SELPH_DQ2_DLY_R1DQ3) |
		p_fld(0x6, SHURK1_SELPH_DQ2_DLY_R1DQ2) |
		p_fld(0x6, SHURK1_SELPH_DQ2_DLY_R1DQ1) |
		p_fld(0x6, SHURK1_SELPH_DQ2_DLY_R1DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ3,
		p_fld(0x2, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3) |
		p_fld(0x2, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2) |
		p_fld(0x2, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1) |
		p_fld(0x2, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0) |
		p_fld(0x6, SHURK1_SELPH_DQ3_DLY_R1DQM3) |
		p_fld(0x6, SHURK1_SELPH_DQ3_DLY_R1DQM2) |
		p_fld(0x6, SHURK1_SELPH_DQ3_DLY_R1DQM1) |
		p_fld(0x6, SHURK1_SELPH_DQ3_DLY_R1DQM0));

	io_32_write_fld_multi(DDRPHY_SHU_R0_B2_DQ7,
		p_fld(0x1a, SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2) |
		p_fld(0x1a, SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2));
	io_32_write_fld_multi(DDRPHY_SHU_R0_B1_DQ7,
		p_fld(0x1a, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
		p_fld(0x1a, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	io_32_write_fld_multi(DDRPHY_SHU_R0_B0_DQ7,
		p_fld(0x1a, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
		p_fld(0x1a, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B2_DQ7,
		p_fld(0x14, SHU_R1_B2_DQ7_RK1_ARPI_DQM_B2) |
		p_fld(0x14, SHU_R1_B2_DQ7_RK1_ARPI_DQ_B2));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B1_DQ7,
		p_fld(0x14, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1) |
		p_fld(0x14, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B0_DQ7,
		p_fld(0x14, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0) |
		p_fld(0x14, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));

	delay_us(1);

	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_RG_RX_ARDQS0_DQSIENMODE_B2);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1);
	io_32_write_fld_align(DDRPHY_B0_DQ9, SET_FLD,
		B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0);

	io_32_write_fld_align(DRAMC_REG_STBCAL, SET_FLD, STBCAL_DQSIENMODE);
	io_32_write_fld_multi(DRAMC_REG_SREFCTRL,
		p_fld(CLEAR_FLD, SREFCTRL_SREF_HW_EN) |
		p_fld(0x8, SREFCTRL_SREFDLY));
	io_32_write_fld_multi(DRAMC_REG_SHU_CKECTRL,
		p_fld(0x3, SHU_CKECTRL_SREF_CK_DLY) |
		p_fld(0x3, SHU_CKECTRL_TCKESRX));

	io_32_write_fld_multi(DRAMC_REG_SHU_PIPE,
		p_fld(SET_FLD, SHU_PIPE_READ_START_EXTEND1) |
		p_fld(SET_FLD, SHU_PIPE_DLE_LAST_EXTEND1) |
		p_fld(SET_FLD, SHU_PIPE_READ_START_EXTEND2) |
		p_fld(SET_FLD, SHU_PIPE_DLE_LAST_EXTEND2) |
		p_fld(SET_FLD, SHU_PIPE_READ_START_EXTEND3) |
		p_fld(SET_FLD, SHU_PIPE_DLE_LAST_EXTEND3));
	io_32_write_fld_multi(DRAMC_REG_CKECTRL,
		p_fld(SET_FLD, CKECTRL_CKEON) |
		p_fld(CLEAR_FLD, CKECTRL_CKETIMER_SEL));
	io_32_write_fld_align(DRAMC_REG_RKCFG, SET_FLD,
		RKCFG_CKE2RANK_OPT2);

	if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
		io_32_write_fld_multi(DRAMC_REG_SHU_CONF2,
			p_fld(SET_FLD, SHU_CONF2_WPRE2T) |
			p_fld(SET_FLD, SHU_CONF2_FASTWAKE2) |
			p_fld(0x7, SHU_CONF2_DCMDLYREF));

		io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, SET_FLD,
			SPCMDCTRL_CLR_EN);

		io_32_write_fld_align(DRAMC_REG_SHUCTRL1, 0x40,
			SHUCTRL1_FC_PRDCNT);
	} else {
		io_32_write_fld_multi(DRAMC_REG_SHU_CONF2,
			p_fld(SET_FLD, SHU_CONF2_WPRE2T) |
			p_fld(SET_FLD, SHU_CONF2_FASTWAKE2) |
			p_fld(0x7, SHU_CONF2_DCMDLYREF));

		io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, SET_FLD,
			SPCMDCTRL_CLR_EN);

		io_32_write_fld_align(DRAMC_REG_SHUCTRL1, 0x40,
			SHUCTRL1_FC_PRDCNT);
	}
	io_32_write_fld_align(DRAMC_REG_SHUCTRL, SET_FLD,
		SHUCTRL_LPSM_BYPASS_B);
	io_32_write_fld_multi(DRAMC_REG_REFCTRL1,
		p_fld(CLEAR_FLD, REFCTRL1_SREF_PRD_OPT) |
		p_fld(CLEAR_FLD, REFCTRL1_PSEL_OPT1) |
		p_fld(CLEAR_FLD, REFCTRL1_PSEL_OPT2) |
		p_fld(CLEAR_FLD, REFCTRL1_PSEL_OPT3));
	/*
	 * io_32_write_fld_align(DDRPHY_SHU_PLL4, 0xfe, SHU_PLL4_RG_RPHYPLL_RESERVED);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL6, 0xfe, SHU_PLL6_RG_RCLRPLL_RESERVED);
	 */
	io_32_write_fld_multi(DRAMC_REG_REFRATRE_FILTER,
		p_fld(SET_FLD, REFRATRE_FILTER_PB2AB_OPT) |
		p_fld(CLEAR_FLD, REFRATRE_FILTER_PB2AB_OPT1));

#if !APPLY_LP4_POWER_INIT_SEQUENCE
	io_32_write_fld_align(DDRPHY_MISC_CTRL1, SET_FLD,
		MISC_CTRL1_R_DMDA_RRESETB_I);
#endif
	io_32_write_fld_align(DRAMC_REG_DRAMCTRL, CLEAR_FLD,
		DRAMCTRL_CLKWITRFC);
	io_32_write_fld_multi(DRAMC_REG_MISCTL0,
		p_fld(SET_FLD, MISCTL0_DM32BIT_RDSEL_OPT) |
		p_fld(SET_FLD, MISCTL0_REFP_ARB_EN2) |
		p_fld(SET_FLD, MISCTL0_PBC_ARB_EN) |
		p_fld(SET_FLD, MISCTL0_REFA_ARB_EN2));

	io_32_write_fld_align(DRAMC_REG_MISCTL0, SET_FLD, MISCTL0_PBC_ARB_EN);
	io_32_write_fld_multi(DRAMC_REG_MISCTL0,
		p_fld(SET_FLD, MISCTL0_PG_WAKEUP_OPT) |
		p_fld(SET_FLD, MISCTL0_PRE_DLE_VLD_OPT));
	io_32_write_fld_multi(DRAMC_REG_PERFCTL0,
		p_fld(SET_FLD, PERFCTL0_MWHPRIEN) |
		p_fld(SET_FLD, PERFCTL0_WFLUSHEN) |
		p_fld(SET_FLD, PERFCTL0_EMILLATEN) |
		p_fld(SET_FLD, PERFCTL0_RWAGEEN) |
		p_fld(SET_FLD, PERFCTL0_RWLLATEN) |
		p_fld(SET_FLD, PERFCTL0_RWHPRIEN) |
		p_fld(SET_FLD, PERFCTL0_RWOFOEN) |
		p_fld(SET_FLD, PERFCTL0_DISRDPHASE1));

	io_32_write_fld_align(DRAMC_REG_ARBCTL, 0x80, ARBCTL_MAXPENDCNT);
	io_32_write_fld_multi(DRAMC_REG_PADCTRL,
		p_fld(SET_FLD, PADCTRL_DQIENLATEBEGIN) |
		p_fld(SET_FLD, PADCTRL_DQIENQKEND));
	io_32_write_fld_align(DRAMC_REG_DRAMC_PD_CTRL, SET_FLD,
		DRAMC_PD_CTRL_DCMREF_OPT);
	io_32_write_fld_align(DRAMC_REG_CLKCTRL, SET_FLD, CLKCTRL_CLK_EN_1);
	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(0x4, REFCTRL0_DISBYREFNUM) |
		p_fld(SET_FLD, REFCTRL0_DLLFRZ));
#if 0
	io_32_write_fld_multi(DRAMC_REG_CATRAINING1,
		p_fld(0xff, CATRAINING1_CATRAIN_INTV) |
		p_fld(CLEAR_FLD, CATRAINING1_CATRAINLAT));
#endif
	io_32_write_fld_multi(DRAMC_REG_SHU_RANKCTL,
		p_fld(0x6, SHU_RANKCTL_RANKINCTL_PHY) |
		p_fld(0x4, SHU_RANKCTL_RANKINCTL_ROOT1) |
		p_fld(0x4, SHU_RANKCTL_RANKINCTL));

	delay_us(2);
#if 0 //cc mark. Gating will depends on calibration value
	LegacyGatingDlyLP4_DDR3200(p);
#endif
	if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
	} else {
		io_32_write_fld_multi(DRAMC_REG_SHURK0_DQSIEN,
			p_fld(0x19, SHURK0_DQSIEN_R0DQS1IEN) |
			p_fld(0x19, SHURK0_DQSIEN_R0DQS0IEN));
		io_32_write_fld_multi(DRAMC_REG_SHURK1_DQSIEN,
			p_fld(CLEAR_FLD, SHURK1_DQSIEN_R1DQS3IEN) |
			p_fld(CLEAR_FLD, SHURK1_DQSIEN_R1DQS2IEN) |
			p_fld(0x1b, SHURK1_DQSIEN_R1DQS1IEN) |
			p_fld(0x1b, SHURK1_DQSIEN_R1DQS0IEN));
	}
	/*
	 * 41536 === over_write_setting_begin ===
	 * 41536 === over_write_setting_end ===
	 */
	io_32_write_fld_align(DRAMC_REG_DRAMCTRL, SET_FLD,
		DRAMCTRL_PREALL_OPTION);
	io_32_write_fld_align(DRAMC_REG_ZQCS, 0x56, ZQCS_ZQCSOP);

	delay_us(1);

	io_32_write_fld_align(DRAMC_REG_SHU_CONF1, SET_FLD,
		SHU_CONF1_TREFBWIG);
	io_32_write_fld_align(DRAMC_REG_SHU_CONF3, 0xff, SHU_CONF3_REFRCNT);
	io_32_write_fld_align(DRAMC_REG_REFCTRL0, SET_FLD,
		REFCTRL0_REFFRERUN);
	io_32_write_fld_align(DRAMC_REG_SREFCTRL, SET_FLD,
		SREFCTRL_SREF_HW_EN);
	io_32_write_fld_align(DRAMC_REG_MPC_OPTION, SET_FLD,
		MPC_OPTION_MPCRKEN);
	io_32_write_fld_align(DRAMC_REG_DRAMC_PD_CTRL, SET_FLD,
		DRAMC_PD_CTRL_PHYCLKDYNGEN);
	io_32_write_fld_align(DRAMC_REG_DRAMC_PD_CTRL, SET_FLD,
		DRAMC_PD_CTRL_DCMEN);
	io_32_write_fld_multi(DRAMC_REG_EYESCAN,
		p_fld(CLEAR_FLD, EYESCAN_RX_DQ_EYE_SEL) |
		p_fld(CLEAR_FLD, EYESCAN_RG_RX_EYE_SCAN_EN));
	io_32_write_fld_multi(DRAMC_REG_STBCAL1,
		p_fld(SET_FLD, STBCAL1_STBCNT_LATCH_EN) |
		p_fld(SET_FLD, STBCAL1_STBCNT_SHU_RST_EN) |
		p_fld(SET_FLD, STBCAL1_STBENCMPEN));
	io_32_write_fld_align(DRAMC_REG_TEST2_1, 0x10000,
		TEST2_1_TEST2_BASE);
#if (FOR_DV_SIMULATION_USED == 1)
	/*
	 * because cmd_len=1 has bug with byte mode, so need to set cmd_len=0, then it will cost more time to do a pattern test
	 * workaround: reduce TEST2_OFF to make less test agent cmd. make lpddr4-1600 can finish in 60us (Mengru)
	 */
	io_32_write_fld_align(DRAMC_REG_TEST2_2, 0x20, TEST2_2_TEST2_OFF);
#else
	io_32_write_fld_align(DRAMC_REG_TEST2_2, 0x400, TEST2_2_TEST2_OFF);
#endif
	io_32_write_fld_multi(DRAMC_REG_TEST2_3,
		p_fld(SET_FLD, TEST2_3_TEST2WREN2_HW_EN) |
		p_fld(0x4, TEST2_3_DQSICALSTP) |
		p_fld(SET_FLD, TEST2_3_TESTAUDPAT));
	io_32_write_fld_align(DRAMC_REG_RSTMASK, CLEAR_FLD,
		RSTMASK_DAT_SYNC_MASK);
	io_32_write_fld_align(DRAMC_REG_RSTMASK, CLEAR_FLD,
		RSTMASK_PHY_SYNC_MASK);

	delay_us(1);

	io_32_write_fld_multi(DRAMC_REG_HW_MRR_FUN,
		p_fld(CLEAR_FLD, HW_MRR_FUN_TRPMRR_EN) |
		p_fld(CLEAR_FLD, HW_MRR_FUN_TRCDMRR_EN) |
		p_fld(CLEAR_FLD, HW_MRR_FUN_TMRR_ENA) |
#if NON_EXIST_RG
		p_fld(0x6, HW_MRR_FUN_MANTMRR) |
#endif
		p_fld(SET_FLD, HW_MRR_FUN_MANTMRR_EN) |
		p_fld(SET_FLD, HW_MRR_FUN_R2MRRHPRICTL) |
		p_fld(SET_FLD, HW_MRR_FUN_MRR_HW_HIPRI) |
		p_fld(SET_FLD, HW_MRR_FUN_TR2MRR_ENA));
	if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
		io_32_write_fld_align(DRAMC_REG_PERFCTL0, SET_FLD,
			PERFCTL0_WRFIO_MODE2);

		io_32_write_fld_multi(DRAMC_REG_PERFCTL0,
			p_fld(SET_FLD, PERFCTL0_REORDEREN) |
			p_fld(CLEAR_FLD, PERFCTL0_REORDER_MODE));
		io_32_write_fld_align(DRAMC_REG_RSTMASK, CLEAR_FLD,
			RSTMASK_GT_SYNC_MASK);
		io_32_write_fld_align(DRAMC_REG_RKCFG, SET_FLD, RKCFG_DQSOSC2RK);
		io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, SET_FLD,
			SPCMDCTRL_REFR_BLOCKEN);
		io_32_write_fld_align(DRAMC_REG_EYESCAN, CLEAR_FLD,
			EYESCAN_RG_RX_MIOCK_JIT_EN);
	} else {
		io_32_write_fld_align(DRAMC_REG_DRAMCTRL, CLEAR_FLD,
			DRAMCTRL_CTOREQ_HPRI_OPT);
		io_32_write_fld_multi(DRAMC_REG_PERFCTL0,
			p_fld(SET_FLD, PERFCTL0_REORDEREN) |
			p_fld(CLEAR_FLD, PERFCTL0_REORDER_MODE));
		io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, SET_FLD,
			SPCMDCTRL_REFR_BLOCKEN);
		io_32_write_fld_align(DRAMC_REG_RSTMASK, CLEAR_FLD,
			RSTMASK_GT_SYNC_MASK);
		io_32_write_fld_align(DRAMC_REG_RKCFG, SET_FLD, RKCFG_DQSOSC2RK);
		io_32_write_fld_align(DRAMC_REG_MPC_OPTION, 1, MPC_OPTION_MPCRKEN);
		io_32_write_fld_align(DRAMC_REG_EYESCAN, CLEAR_FLD,
			EYESCAN_RG_RX_MIOCK_JIT_EN);
		io_32_write_fld_align(DRAMC_REG_SHU_WODT, SET_FLD,
			SHU_WODT_DBIWR);
		io_32_write_fld_align(DDRPHY_SHU_B0_DQ7, SET_FLD,
			SHU_B0_DQ7_R_DMDQMDBI_SHU_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ7, SET_FLD,
			SHU_B1_DQ7_R_DMDQMDBI_SHU_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DQ7, SET_FLD,
			SHU_B2_DQ7_R_DMDQMDBI_SHU_B2);
		io_32_write_fld_align(DRAMC_REG_SHU_RANKCTL, 0x4,
			SHU_RANKCTL_RANKINCTL);
		io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ0,
			p_fld(0x2, SHURK0_SELPH_DQ0_TXDLY_DQ1) |
			p_fld(0x2, SHURK0_SELPH_DQ0_TXDLY_DQ0));
		io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ1,
			p_fld(0x2, SHURK0_SELPH_DQ1_TXDLY_DQM1) |
			p_fld(0x2, SHURK0_SELPH_DQ1_TXDLY_DQM0));
		io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ0,
			p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1) |
			p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
		io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ1,
			p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1) |
			p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
	}
#if ENABLE_TMRRI_NEW_MODE
	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, CLEAR_FLD,
		SPCMDCTRL_REFR_BLOCKEN);
	io_32_write_fld_align(DRAMC_REG_HW_MRR_FUN, SET_FLD,
		HW_MRR_FUN_TMRR_ENA);
#endif
	delay_us(5);

	io_32_write_fld_align(DRAMC_REG_STBCAL1, 0x3,
		STBCAL1_STBCAL_FILTER);
	io_32_write_fld_align(DRAMC_REG_STBCAL1, SET_FLD,
		STBCAL1_STBCAL_FILTER);
	io_32_write_fld_multi(DRAMC_REG_STBCAL,
		p_fld(SET_FLD, STBCAL_STB_DQIEN_IG) |
		p_fld(SET_FLD, STBCAL_PICHGBLOCK_NORD) |
		p_fld(CLEAR_FLD, STBCAL_STBCALEN) |
		p_fld(CLEAR_FLD, STBCAL_STB_SELPHYCALEN) |
		p_fld(SET_FLD, STBCAL_PIMASK_RKCHG_OPT));
	io_32_write_fld_align(DRAMC_REG_STBCAL1, SET_FLD,
		STBCAL1_STB_SHIFT_DTCOUT_IG);
	io_32_write_fld_multi(DRAMC_REG_SHU_DQSG,
		p_fld(0x9, SHU_DQSG_STB_UPDMASKCYC) |
		p_fld(SET_FLD, SHU_DQSG_STB_UPDMASK_EN));
	io_32_write_fld_align(DDRPHY_MISC_CTRL0, CLEAR_FLD,
		MISC_CTRL0_R_DMDQSIEN_SYNCOPT);
	io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, SET_FLD,
		SHU_STBCAL_DQSG_MODE);
	io_32_write_fld_align(DRAMC_REG_STBCAL, SET_FLD,
		STBCAL_SREF_DQSGUPD);

#if ENABLE_RX_TRACKING_LP4
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ7, p->dbi_r_onoff[p->dram_fsp],
		SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ7, p->dbi_r_onoff[p->dram_fsp],
		SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ7, p->dbi_r_onoff[p->dram_fsp],
		SHU_B2_DQ7_R_DMRXTRACK_DQM_EN_B2);
#else
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ7, CLEAR_FLD,
		SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ7, CLEAR_FLD,
		SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ7, CLEAR_FLD,
		SHU_B2_DQ7_R_DMRXTRACK_DQM_EN_B2);
#endif

	io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, SET_FLD,
		SHU_STBCAL_PICGLAT);

	io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, 2,
		SHU_STBCAL_DMSTBLAT);

	io_32_write_fld_multi(DRAMC_REG_REFCTRL1,
		p_fld(SET_FLD, REFCTRL1_REF_QUE_AUTOSAVE_EN) |
		p_fld(SET_FLD, REFCTRL1_SLEFREF_AUTOSAVE_EN));
	io_32_write_fld_multi(DRAMC_REG_DQSOSCR,
		p_fld(SET_FLD, DQSOSCR_SREF_TXPI_RELOAD_OPT) |
		p_fld(SET_FLD, DQSOSCR_SREF_TXUI_RELOAD_OPT));
	io_32_write_fld_multi(DRAMC_REG_RSTMASK,
		p_fld(CLEAR_FLD, RSTMASK_DVFS_SYNC_MASK) |
		p_fld(CLEAR_FLD, RSTMASK_GT_SYNC_MASK_FOR_PHY) |
		p_fld(CLEAR_FLD, RSTMASK_DVFS_SYNC_MASK_FOR_PHY));
	io_32_write_fld_align(DRAMC_REG_RKCFG, SET_FLD, RKCFG_RKMODE);

#if !APPLY_LP4_POWER_INIT_SEQUENCE
	io_32_write_fld_multi(DRAMC_REG_CKECTRL,
		p_fld(SET_FLD, CKECTRL_CKEFIXON) |
		p_fld(SET_FLD, CKECTRL_CKE1FIXON));
#endif

	delay_us(12);

	/*
	 * TODO: DDR3200
	 * if(p->frequency==1600)
	 */
	{

		if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
			io_32_write_fld_multi(DRAMC_REG_SHU_RANKCTL,
				p_fld(0x5, SHU_RANKCTL_RANKINCTL_PHY) |
				p_fld(0x3, SHU_RANKCTL_RANKINCTL_ROOT1) |
				p_fld(0x3, SHU_RANKCTL_RANKINCTL));
		} else {
			io_32_write_fld_multi(DRAMC_REG_SHU_RANKCTL,
				p_fld(0x6, SHU_RANKCTL_RANKINCTL_PHY) |
				p_fld(0x4, SHU_RANKCTL_RANKINCTL_ROOT1));
		}
	#if 0//cc mark. Depends on calibration
		LegacyTxTrackLP4_DDR3200(p);
	#endif
#if NON_EXIST_RG
		io_32_write_fld_align(DRAMC_REG_SHU_WODT, CLEAR_FLD,
			SHU_WODT_WPST2T);
#endif

		if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
			io_32_write_fld_multi(DRAMC_REG_SHURK0_DQSIEN,
				p_fld(0x19, SHURK0_DQSIEN_R0DQS1IEN) |
				p_fld(0x19, SHURK0_DQSIEN_R0DQS0IEN));
		}
		io_32_write_fld_multi(DRAMC_REG_SHURK0_PI,
			p_fld(0x1a, SHURK0_PI_RK0_ARPI_DQM_B1) |
			p_fld(0x1a, SHURK0_PI_RK0_ARPI_DQM_B0) |
			p_fld(0x1a, SHURK0_PI_RK0_ARPI_DQ_B1) |
			p_fld(0x1a, SHURK0_PI_RK0_ARPI_DQ_B0));
		io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ2,
			p_fld(0x4, SHURK0_SELPH_DQ2_DLY_OEN_DQ3) |
			p_fld(0x4, SHURK0_SELPH_DQ2_DLY_OEN_DQ2) |
			p_fld(0x4, SHURK0_SELPH_DQ2_DLY_OEN_DQ1) |
			p_fld(0x4, SHURK0_SELPH_DQ2_DLY_OEN_DQ0));
		io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ3,
			p_fld(0x4, SHURK0_SELPH_DQ3_DLY_OEN_DQM3) |
			p_fld(0x4, SHURK0_SELPH_DQ3_DLY_OEN_DQM2) |
			p_fld(0x4, SHURK0_SELPH_DQ3_DLY_OEN_DQM1) |
			p_fld(0x4, SHURK0_SELPH_DQ3_DLY_OEN_DQM0));
		if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1) {
			io_32_write_fld_multi(DRAMC_REG_SHURK1_DQSIEN,
				p_fld(CLEAR_FLD, SHURK1_DQSIEN_R1DQS3IEN) |
				p_fld(CLEAR_FLD, SHURK1_DQSIEN_R1DQS2IEN) |
				p_fld(0x1b, SHURK1_DQSIEN_R1DQS1IEN) |
				p_fld(0x1b, SHURK1_DQSIEN_R1DQS0IEN));
		}
		io_32_write_fld_multi(DRAMC_REG_SHURK1_PI,
			p_fld(0x14, SHURK1_PI_RK1_ARPI_DQM_B1) |
			p_fld(0x14, SHURK1_PI_RK1_ARPI_DQM_B0) |
			p_fld(0x14, SHURK1_PI_RK1_ARPI_DQ_B1) |
			p_fld(0x14, SHURK1_PI_RK1_ARPI_DQ_B0));
		io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ2,
			p_fld(0x4, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3) |
			p_fld(0x4, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2) |
			p_fld(0x4, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1) |
			p_fld(0x4, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0));
		io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ3,
			p_fld(0x4, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3) |
			p_fld(0x4, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2) |
			p_fld(0x4, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1) |
			p_fld(0x4, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0));
		io_32_write_fld_multi(DRAMC_REG_SHU_DQSG_RETRY,
			p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_RETRY_ROUND_NUM) |
			p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_XSR_RETRY_SPM_MODE) |
			p_fld(0x0, SHU_DQSG_RETRY_R_DQSIENLAT) |
			p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_DDR1866_PLUS));
	#if 0//cc mark
		LegacyPreCalLP4_DDR3200(p);
	#endif

		if (get_dram_cbt_mode(p) == CBT_BYTE_MODE1)	{
			io_32_write_fld_align(DDRPHY_SHU_B0_DQ7, SET_FLD,
				SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0);
			io_32_write_fld_align(DDRPHY_SHU_B1_DQ7, SET_FLD,
				SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1);
			io_32_write_fld_align(DDRPHY_SHU_B2_DQ7, SET_FLD,
				SHU_B2_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B2);
		} else {
			io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
				p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) |
				p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0));
			io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7,
				p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) |
				p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1));
			io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
				p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B2) |
				p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B2));
		}

		io_32_write_fld_align(DRAMC_REG_SHU_DQS2DQ_TX, CLEAR_FLD,
			SHU_DQS2DQ_TX_OE2DQ_OFFSET);
	}

	if (get_div_mode(p) == DIV8_MODE) {
		if (p->freqGroup == DDR3733_FREQ) {
			dramc_setting_ddr3733(p);
		} else if (p->freqGroup == DDR2666_FREQ ||
			p->freqGroup == DDR2400_FREQ) {
			dramc_setting_ddr2667(p);
		} else if (p->freqGroup == DDR1600_FREQ) {
			dramc_setting_ddr1600(p);
		}
	} else {
		dramc_setting_ddr1600(p);
	}

	update_initial_setting_lp4(p);

#if SIMULATION_SW_IMPED
	if (p->dram_type == TYPE_LPDDR4)
		dramc_sw_impedance_save_register(p, ODT_ON, ODT_ON,
			DRAM_DFS_SHUFFLE_1);
	else if (p->dram_type == TYPE_LPDDR4X)
		dramc_sw_impedance_save_register(p, ODT_OFF, p->odt_onoff,
			DRAM_DFS_SHUFFLE_1);
	else if (p->dram_type == TYPE_LPDDR4P)
		dramc_sw_impedance_save_register(p, ODT_OFF, ODT_OFF,
			DRAM_DFS_SHUFFLE_1);
#endif

#ifndef LOOPBACK_TEST
	//DDRPhyFreqMeter();
#endif

#if 0
	io_32_write_fld_multi(DRAMC_REG_MRS,
		p_fld(CLEAR_FLD, MRS_MRSRK) |
		p_fld(0x4, MRS_MRSMA) |
		p_fld(CLEAR_FLD, MRS_MRSOP));
	delay_us(1);
	io_32_write_fld_align(DRAMC_REG_SPCMD, SET_FLD, SPCMD_MRREN);
	io_32_write_fld_align(DRAMC_REG_SPCMD, CLEAR_FLD, SPCMD_MRREN);
#endif

	io_32_write_fld_align(DRAMC_REG_TEST2_4, CLEAR_FLD,
		TEST2_4_TEST_REQ_LEN1);

	io_32_write_fld_align(DRAMC_REG_SHU_CONF3, 0x5, SHU_CONF3_ZQCSCNT);

	delay_us(1);

#if !APPLY_LP4_POWER_INIT_SEQUENCE
	io_32_write_fld_multi(DRAMC_REG_CKECTRL,
		p_fld(CLEAR_FLD, CKECTRL_CKEFIXON) |
		p_fld(CLEAR_FLD, CKECTRL_CKE1FIXON));
#endif
	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(SET_FLD, REFCTRL0_PBREFEN) |
		p_fld(SET_FLD, REFCTRL0_PBREF_DISBYRATE));
	io_32_write_fld_multi(DRAMC_REG_SHUCTRL2,
		p_fld(SET_FLD, SHUCTRL2_MR13_SHU_EN) |
		p_fld(SET_FLD, SHUCTRL2_HWSET_WLRL));
	io_32_write_fld_align(DRAMC_REG_REFCTRL0, SET_FLD, REFCTRL0_REFDIS);

	io_32_write_fld_align(DRAMC_REG_DRAMCTRL, SET_FLD,
		DRAMCTRL_REQQUE_THD_EN);

	io_32_write_fld_multi(DRAMC_REG_DUMMY_RD,
		p_fld(SET_FLD, DUMMY_RD_DQSG_DMYRD_EN) |
		p_fld(p->support_rank_num, DUMMY_RD_RANK_NUM) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_CNT6) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_CNT5) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_CNT3) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_SW));
	io_32_write_fld_align(DRAMC_REG_TEST2_4, 0x4,
		TEST2_4_TESTAGENTRKSEL);
	io_32_write_fld_align(DRAMC_REG_DRAMCTRL, CLEAR_FLD,
		DRAMCTRL_CTOREQ_HPRI_OPT);

	delay_us(1);

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);

	io_32_write_fld_multi(DRAMC_REG_SHUCTRL,
		p_fld(SET_FLD, SHUCTRL_R_DRAMC_CHA) |
		p_fld(CLEAR_FLD, SHUCTRL_SHU_PHYRST_SEL));
	io_32_write_fld_align(DRAMC_REG_SHUCTRL2, SET_FLD,
		SHUCTRL2_R_DVFS_DLL_CHA);

	io_32_write_fld_multi(DRAMC_REG_SHUCTRL + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHUCTRL_R_DRAMC_CHA) |
		p_fld(SET_FLD, SHUCTRL_SHU_PHYRST_SEL));
	io_32_write_fld_align(DRAMC_REG_SHUCTRL2 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, SHUCTRL2_R_DVFS_DLL_CHA);

	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);
	io_32_write_fld_multi(DRAMC_REG_REFRATRE_FILTER,
		p_fld(0x6, REFRATRE_FILTER_REFRATE_FIL7) |
		p_fld(0x5, REFRATRE_FILTER_REFRATE_FIL6) |
		p_fld(0x4, REFRATRE_FILTER_REFRATE_FIL5) |
		p_fld(0x3, REFRATRE_FILTER_REFRATE_FIL4) |
		p_fld(0x3, REFRATRE_FILTER_REFRATE_FIL3) |
		p_fld(0x3, REFRATRE_FILTER_REFRATE_FIL2) |
		p_fld(0x2, REFRATRE_FILTER_REFRATE_FIL1) |
		p_fld(0x1, REFRATRE_FILTER_REFRATE_FIL0));

	io_32_write_fld_multi(DRAMC_REG_SPCMDCTRL,
		p_fld(SET_FLD, SPCMDCTRL_SRFMR4_CNTKEEP_B) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_UPD_OPT));

	if (p->support_rank_num == RANK_DUAL) {
		io_32_write_fld_align(DRAMC_REG_SPCMDCTRL,
			SET_FLD, SPCMDCTRL_HMR4_TOG_OPT);
		io_32_write_fld_align(DRAMC_REG_RKCFG,
			CLEAR_FLD, RKCFG_CS1FORCE0);
	} else {
		io_32_write_fld_align(DRAMC_REG_SPCMDCTRL,
			CLEAR_FLD, SPCMDCTRL_HMR4_TOG_OPT);
		io_32_write_fld_align(DRAMC_REG_RKCFG,
			SET_FLD, RKCFG_CS1FORCE0);
	}

	io_32_write_fld_multi(DRAMC_REG_SHU_PHY_RX_CTRL,
		p_fld(0x2, SHU_PHY_RX_CTRL_RX_IN_BUFF_EN_HEAD) |
		p_fld(CLEAR_FLD, SHU_PHY_RX_CTRL_RX_IN_GATE_EN_HEAD) |
		p_fld(0x2, SHU_PHY_RX_CTRL_RX_IN_GATE_EN_PRE_OFFSET) |
		p_fld(0x2, SHU_PHY_RX_CTRL_RANK_RXDLY_UPD_OFFSET) |
		p_fld(SET_FLD, SHU_PHY_RX_CTRL_RANK_RXDLY_UPDLAT_EN));

	if (get_div_mode(p) == DIV8_MODE) {
		io_32_write_fld_multi(DRAMC_REG_SHU_APHY_TX_PICG_CTRL,
			p_fld(0x5, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_DYN_GATING_SEL) |
			p_fld(0xc, SHU_APHY_TX_PICG_CTRL_APHYPI_CG_CK_SEL));
	} else {
		io_32_write_fld_multi(DRAMC_REG_SHU_APHY_TX_PICG_CTRL,
			p_fld(0x6, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_DYN_GATING_SEL) |
			p_fld(0xc, SHU_APHY_TX_PICG_CTRL_APHYPI_CG_CK_SEL));
	}
	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);


	io_32_write_fld_align_all(DRAMC_REG_DVFSDLL2,
		0x3, DVFSDLL2_R_DVFS_PICG_MARGIN4_NEW);

#if DVT_8PHASE_UNCERTAINTY_EN
	io_32_write_fld_align_all(DDRPHY_DVFS_EMI_CLK,
		SET_FLD, DVFS_EMI_CLK_R_DDRPHY_SHUFFLE_MUX_ENABLE);
	io_32_write_fld_multi_all(DRAMC_REG_DVFSDLL2,
		p_fld(SET_FLD, DVFSDLL2_R_SHUFFLE_PI_RESET_ENABLE) |
		p_fld(0x3, DVFSDLL2_R_DVFS_MCK8X_MARGIN));
	show_msg((INFO, "[DVT] 8phase Remove Uncertainty Setting Enable\n"));
#endif

#if ENABLE_DUTY_SCAN_V2
	DramcNewDutyCalibration(p);
#endif

	dvfs_settings(p);
	dramc_mr_init_lp4(p);
}
#endif

#if SUPPORT_TYPE_LPDDR3
static DRAM_STATUS_T update_initial_setting_lp3(DRAMC_CTX_T *p)
{
	io_32_write_fld_multi_all(DDRPHY_R0_B2_RXDVS2,
		p_fld(CLEAR_FLD, R0_B2_RXDVS2_R_RK0_DVS_MODE_B2) |
		p_fld(CLEAR_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(CLEAR_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B2));
	io_32_write_fld_multi_all(DDRPHY_R1_B2_RXDVS2,
		p_fld(CLEAR_FLD, R1_B2_RXDVS2_R_RK1_DVS_MODE_B2) |
		p_fld(CLEAR_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(CLEAR_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B2));
	io_32_write_fld_multi_all(DDRPHY_R0_B1_RXDVS2,
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi_all(DDRPHY_R1_B1_RXDVS2,
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1) |
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi_all(DDRPHY_R0_B0_RXDVS2,
		p_fld(CLEAR_FLD, R0_B0_RXDVS2_R_RK0_DVS_MODE_B0) |
		p_fld(CLEAR_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(CLEAR_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi_all(DDRPHY_R1_B0_RXDVS2,
		p_fld(CLEAR_FLD, R1_B0_RXDVS2_R_RK1_DVS_MODE_B0) |
		p_fld(CLEAR_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(CLEAR_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi_all(DDRPHY_R0_CA_RXDVS2,
		p_fld(CLEAR_FLD, R0_CA_RXDVS2_R_RK0_DVS_MODE_CA) |
		p_fld(CLEAR_FLD, R0_CA_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_CA) |
		p_fld(CLEAR_FLD, R0_CA_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_CA));
	io_32_write_fld_multi_all(DDRPHY_R1_CA_RXDVS2,
		p_fld(CLEAR_FLD, R1_CA_RXDVS2_R_RK1_DVS_MODE_CA) |
		p_fld(CLEAR_FLD, R1_CA_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_CA) |
		p_fld(CLEAR_FLD, R1_CA_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_CA));
#if 0
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1) |
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
#endif

	io_32_write_fld_multi_all(DDRPHY_B0_DQ6,
		p_fld(SET_FLD, B0_DQ6_RG_TX_ARDQ_DDR3_SEL_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_TX_ARDQ_DDR4_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_DDR4_SEL_B0));
	io_32_write_fld_multi_all(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_TX_ARDQ_DDR3_SEL_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_DDR3_SEL_B1) |
		p_fld(CLEAR_FLD, B1_DQ6_RG_TX_ARDQ_DDR4_SEL_B1) |
		p_fld(CLEAR_FLD, B1_DQ6_RG_RX_ARDQ_DDR4_SEL_B1));
	io_32_write_fld_multi_all(DDRPHY_B2_DQ6,
		p_fld(SET_FLD, B2_DQ6_RG_TX_ARDQ_DDR3_SEL_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_DDR3_SEL_B2) |
		p_fld(CLEAR_FLD, B2_DQ6_RG_TX_ARDQ_DDR4_SEL_B2) |
		p_fld(CLEAR_FLD, B2_DQ6_RG_RX_ARDQ_DDR4_SEL_B2));
	io_32_write_fld_multi_all(DDRPHY_CA_CMD6,
		p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_DDR3_SEL) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_DDR3_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_TX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_DDR4_SEL));

	io_32_write_fld_align_all(DDRPHY_B0_DQ8, CLEAR_FLD,
		B0_DQ8_RG_TX_ARDQ_EN_LP4P_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ8, CLEAR_FLD,
		B1_DQ8_RG_TX_ARDQ_EN_LP4P_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ8, CLEAR_FLD,
		B2_DQ8_RG_TX_ARDQ_EN_LP4P_B2);
	io_32_write_fld_align_all(DDRPHY_CA_CMD9, CLEAR_FLD,
		CA_CMD9_RG_TX_ARCMD_EN_LP4P);

	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), CLEAR_FLD,
		B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ5), CLEAR_FLD,
		B1_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B1);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ5), CLEAR_FLD,
		B2_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B2);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD5), CLEAR_FLD,
		CA_CMD5_RG_RX_ARCMD_EYE_VREF_EN);

	io_32_write_fld_multi_all(DDRPHY_CA_CMD8,
		p_fld(SET_FLD, CA_CMD8_RG_TX_RRESETB_DDR3_SEL) |
		p_fld(CLEAR_FLD, CA_CMD8_RG_TX_RRESETB_DDR4_SEL));

	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), 0xb,
		B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ5), 0xb,
		B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ5), 0xb,
		B2_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B2);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD5), 0xb,
		CA_CMD5_RG_RX_ARCMD_EYE_VREF_SEL);

	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ5), 0xb,
		SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_SHU_B1_DQ5), 0xb,
		SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ5), 0xb,
		SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD5),
		0xb, SHU_CA_CMD5_RG_RX_ARCMD_VREF_SEL);

	if (!(p->is_emcp)) {
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B0_DQ10),
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ0_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ1_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ2_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ3_DQS_SEL_B0));

		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B1_DQ10),
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ4_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ5_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ6_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ7_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ0_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ1_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ2_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ3_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ4_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ5_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ6_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ7_DQS_SEL_B1));

		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B2_DQ10),
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ4_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ5_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ6_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ7_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ4_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ5_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ6_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ7_DQS_SEL_B2));

		io_32_write_fld_multi((DDRPHY_B0_DQ10 + SHIFT_TO_CHB_ADDR),
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ4_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ0_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ1_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ2_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ4_DQS_SEL_B0));
		io_32_write_fld_multi((DDRPHY_B2_DQ10 + SHIFT_TO_CHB_ADDR),
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ4_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ5_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ6_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ7_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ4_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ5_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ6_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ7_DQS_SEL_B2));
	}

	io_32_write_fld_align(DDRPHY_B1_DLL_ARPI0 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B1_DLL_ARPI0_RG_ARPI_ISO_EN_B1);
	io_32_write_fld_align(DDRPHY_CA_DLL_ARPI0 + SHIFT_TO_CHB_ADDR, SET_FLD,
		CA_DLL_ARPI0_RG_ARPI_ISO_EN_CA);

	io_32_write_fld_align(DDRPHY_B0_DQ6, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B0);
	io_32_write_fld_align(DDRPHY_B0_DQ6 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ6, SET_FLD,
		B1_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ6, SET_FLD,
		B2_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B2);

	io_32_write_fld_align(DDRPHY_CA_CMD6, CLEAR_FLD,
		CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN);
	io_32_write_fld_align(DDRPHY_CA_CMD6 + SHIFT_TO_CHB_ADDR, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN);

	io_32_write_fld_align_all(DDRPHY_CA_CMD6, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN);

	io_32_write_fld_align_phy_all(DDRPHY_B0_DQ6, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_O1_SEL_B0);
	io_32_write_fld_align_phy_all(DDRPHY_B1_DQ6, SET_FLD,
		B1_DQ6_RG_RX_ARDQ_O1_SEL_B1);
	io_32_write_fld_align_phy_all(DDRPHY_B2_DQ6, SET_FLD,
		B2_DQ6_RG_RX_ARDQ_O1_SEL_B2);
	io_32_write_fld_align_phy_all(DDRPHY_CA_CMD6, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_O1_SEL);

	io_32_write_fld_align_phy_all(DDRPHY_B0_DQ6, SET_FLD,
		B0_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B0);
	io_32_write_fld_align_phy_all(DDRPHY_B1_DQ6, SET_FLD,
		B1_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B1);
	io_32_write_fld_align_phy_all(DDRPHY_B2_DQ6, SET_FLD,
		B2_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B2);
	io_32_write_fld_align_phy_all(DDRPHY_CA_CMD6, SET_FLD,
		CA_CMD6_RG_TX_ARCMD_ODTEN_EXT_DIS);

	io_32_write_fld_multi(DDRPHY_B0_DQ3,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B0));
	io_32_write_fld_multi(DDRPHY_B0_DQ3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B0));
	io_32_write_fld_multi_all(DDRPHY_B1_DQ3,
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B1));
	io_32_write_fld_multi_all(DDRPHY_B2_DQ3,
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B2));

	io_32_write_fld_multi(DDRPHY_CA_CMD3,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_IN_BUFF_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCLK_IN_BUFF_EN));
	io_32_write_fld_multi(DDRPHY_CA_CMD3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_IN_BUFF_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCLK_IN_BUFF_EN));

	io_32_write_fld_align(DDRPHY_B0_DQ9, CLEAR_FLD,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_CA_CMD10, CLEAR_FLD,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);

	io_32_write_fld_align_all(DDRPHY_B0_DQ6, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ6, SET_FLD,
		B1_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ6, SET_FLD,
		B2_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B2);

	io_32_write_fld_align_all(DDRPHY_CA_CMD6, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_RPRE_TOG_EN);

#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	io_32_write_fld_align(DDRPHY_B1_DQ9, 0x04,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, 0x04,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, 0x04,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, 0x04,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
#else
	io_32_write_fld_align(DDRPHY_B2_DQ9, 0x7,
		B2_DQ9_R_IN_GATE_EN_LOW_OPT_B2);
	io_32_write_fld_align(DDRPHY_B2_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B2_DQ9_R_IN_GATE_EN_LOW_OPT_B2);
	io_32_write_fld_align(DDRPHY_B1_DQ9, 0x7,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, 0x7,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
#endif

	io_32_write_fld_align_all(DRAMC_REG_SHU_MISC, 0x2,
		SHU_MISC_REQQUE_MAXCNT);

	io_32_write_fld_align_all(DRAMC_REG_SHU_DQSG, 0x2a,
		SHU_DQSG_SCINTV);

	io_32_write_fld_multi(DRAMC_REG_SREFCTRL,
		p_fld(SET_FLD, SREFCTRL_SCSM_CGAR) |
		p_fld(SET_FLD, SREFCTRL_SCARB_SM_CGAR) |
		p_fld(SET_FLD, SREFCTRL_RDDQSOSC_CGAR) |
		p_fld(SET_FLD, SREFCTRL_HMRRSEL_CGAR));

	io_32_write_fld_align(DRAMC_REG_PRE_TDQSCK1, SET_FLD,
		PRE_TDQSCK1_TXUIPI_CAL_CGAR);

	io_32_write_fld_align(DRAMC_REG_SHU_MISC, 0xf, SHU_MISC_PREA_INTV);

	io_32_write_fld_multi_all(DDRPHY_SHU_B0_DQ8,
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMSTBEN_SYNC_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0) |
		p_fld(0x7fff, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0));
	io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ8,
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMSTBEN_SYNC_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1) |
		p_fld(0x7fff, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1));
	io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ8,
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRANK_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMSTBEN_SYNC_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRXDLY_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ8_R_DMRXDVS_UPD_FORCE_EN_B2) |
		p_fld(0x7fff, SHU_B2_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B2));

	io_32_write_fld_multi_all(DDRPHY_SHU_CA_CMD8,
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRANK_CHG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRANK_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_FLAG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_FLAG_SYNC_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMSTBEN_SYNC_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRXDLY_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRXDVS_RDSEL_PIPE_CG_IG_CA) |
		p_fld(CLEAR_FLD, SHU_CA_CMD8_R_DMRXDVS_UPD_FORCE_EN_CA) |
		p_fld(0x7fff, SHU_CA_CMD8_R_DMRXDVS_UPD_FORCE_CYC_CA));
	io_32_write_fld_align_all(DDRPHY_MISC_CTRL3, SET_FLD,
		MISC_CTRL3_R_DDRPHY_COMB_CG_IG);

	io_32_write_fld_align_all(DDRPHY_MISC_CTRL0, CLEAR_FLD,
		MISC_CTRL0_R_DMSHU_PHYDCM_FORCEOFF);

	io_32_write_fld_align_all(DDRPHY_MISC_RXDVS2, SET_FLD,
		MISC_RXDVS2_R_DMRXDVS_SHUFFLE_CTRL_CG_IG);

	io_32_write_fld_multi_all(DDRPHY_SHU_B0_DQ7,
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0));
	io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ7,
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1));
	io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ7,
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXDVS_DQM_FLAGSEL_B2));

	io_32_write_fld_multi(DRAMC_REG_CLKAR,
		p_fld(CLEAR_FLD, CLKAR_DWCLKRUN) |
		p_fld(SET_FLD, CLKAR_SELPH_CMD_CG_DIS) |
		p_fld(0x7FFF, CLKAR_REQQUE_PACG_DIS));

	io_32_write_fld_align(DRAMC_REG_SHU_DQSG_RETRY, CLEAR_FLD,
		SHU_DQSG_RETRY_R_RETRY_PA_DSIABLE);
	io_32_write_fld_align(DRAMC_REG_WRITE_LEV, CLEAR_FLD,
		WRITE_LEV_DDRPHY_COMB_CG_SEL);

	io_32_write_fld_multi(DRAMC_REG_DUMMY_RD,
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_PA_OPT) |
		p_fld(SET_FLD, DUMMY_RD_DMYRD_REORDER_DIS) |
		p_fld(CLEAR_FLD, DUMMY_RD_DMYRD_HPRI_DIS) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_CNT6) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_CNT5) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_CNT3) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_SW));

	io_32_write_fld_multi(DRAMC_REG_STBCAL2,
		p_fld(SET_FLD, STBCAL2_STB_UIDLYCG_IG) |
		p_fld(SET_FLD, STBCAL2_STB_PIDLYCG_IG));
	io_32_write_fld_multi(DRAMC_REG_EYESCAN,
		p_fld(SET_FLD, EYESCAN_EYESCAN_DQS_SYNC_EN) |
		p_fld(SET_FLD, EYESCAN_EYESCAN_NEW_DQ_SYNC_EN) |
		p_fld(SET_FLD, EYESCAN_EYESCAN_DQ_SYNC_EN));
	io_32_write_fld_multi(DRAMC_REG_SHU_ODTCTRL,
		p_fld(SET_FLD, SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG) |
		p_fld(SET_FLD, SHU_ODTCTRL_RODTEN_SELPH_CG_IG));
	io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL0, SET_FLD,
		SHU_B0_DLL0_RG_ARPISM_MCK_SEL_B0_SHU);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL0, SET_FLD,
		SHU_B1_DLL0_RG_ARPISM_MCK_SEL_B1_SHU);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL0, SET_FLD,
		SHU_B2_DLL0_RG_ARPISM_MCK_SEL_B2_SHU);
	io_32_write_fld_align_all(DDRPHY_SHU_CA_DLL0, SET_FLD,
		SHU_CA_DLL0_RG_ARPISM_MCK_SEL_CA_SHU);

	io_32_write_fld_multi_all(DRAMC_REG_SHURK0_DQSCAL,
		p_fld(0, SHURK0_DQSCAL_R0DQSIENLLMTEN) |
		p_fld(0, SHURK0_DQSCAL_R0DQSIENHLMTEN));
	io_32_write_fld_multi_all(DRAMC_REG_SHURK1_DQSCAL,
		p_fld(0, SHURK1_DQSCAL_R1DQSIENLLMTEN) |
		p_fld(0, SHURK1_DQSCAL_R1DQSIENHLMTEN));
	io_32_write_fld_multi_all(DRAMC_REG_SHU_STBCAL,
		p_fld(1, SHU_STBCAL_DQSG_MODE) |
		p_fld(1, SHU_STBCAL_PICGLAT));

#if DramcHWDQSGatingTracking_DVT_JADE_TRACKING_MODE

	io32_write_4b_all(DRAMC_REG_SHU_PIPE, 0xFC000000);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ6, 0x38,
		SHU_B2_DQ6_RG_ARPI_OFFSET_DQSIEN_B2);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ6, 0x38,
		SHU_B1_DQ6_RG_ARPI_OFFSET_DQSIEN_B1);
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ6, 0x38,
		SHU_B0_DQ6_RG_ARPI_OFFSET_DQSIEN_B0);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD6, 0x38,
		SHU_CA_CMD6_RG_ARPI_OFFSET_CLKIEN);
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ6 + SHIFT_TO_CHB_ADDR, 0x38,
		SHU_B0_DQ6_RG_ARPI_OFFSET_DQSIEN_B0);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD6 + SHIFT_TO_CHB_ADDR, 0x38,
		SHU_CA_CMD6_RG_ARPI_OFFSET_CLKIEN);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ6 + SHIFT_TO_CHB_ADDR, 0x38,
		SHU_B1_DQ6_RG_ARPI_OFFSET_DQSIEN_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ6 + SHIFT_TO_CHB_ADDR, 0x38,
		SHU_B2_DQ6_RG_ARPI_OFFSET_DQSIEN_B2);

	{
		io_32_write_fld_align(DDRPHY_SHU_B0_DLL1, SET_FLD,
			RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B1_DLL1, SET_FLD,
			RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B2_DLL1, SET_FLD,
			RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL1, SET_FLD,
			RG_ARCMD_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B0_DLL1 + SHIFT_TO_CHB_ADDR,
			SET_FLD, RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B1_DLL1 + SHIFT_TO_CHB_ADDR,
			SET_FLD, RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B2_DLL1 + SHIFT_TO_CHB_ADDR,
			SET_FLD, RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL1 + SHIFT_TO_CHB_ADDR,
			SET_FLD, RG_ARCMD_REV_BIT_05_RX_SER_RST_MODE);
	}

	io_32_write_fld_align_all(DRAMC_REG_SHU_DQSG, CLEAR_FLD,
		SHU_DQSG_STB_UPDMASKCYC);
	io_32_write_fld_align_all(DRAMC_REG_SHU_DQSG, CLEAR_FLD,
		SHU_DQSG_STB_UPDMASK_EN);

	if (p->frequency >= DDR1600_FREQ) {
		io_32_write_fld_align_all(DRAMC_REG_SHU_STBCAL, 3,
			SHU_STBCAL_DMSTBLAT);
		/*
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM2, 0x7, SHU_ACTIM2_TR2W);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_AC_TIME_05T, 0, SHU_AC_TIME_05T_TR2W_05T);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x5, SHU_ACTIM_XRT_XRTW2R);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x6, SHU_ACTIM_XRT_XRTR2W);
		 */
	} else if (p->frequency >= DDR1200_FREQ) {
		io_32_write_fld_align_all(DRAMC_REG_SHU_STBCAL, 2,
			SHU_STBCAL_DMSTBLAT);
		/*
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM2, 0x5, SHU_ACTIM2_TR2W);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_AC_TIME_05T, 0, SHU_AC_TIME_05T_TR2W_05T);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x5, SHU_ACTIM_XRT_XRTW2R);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x6, SHU_ACTIM_XRT_XRTR2W);
		 */
	} else {
		io_32_write_fld_align_all(DRAMC_REG_SHU_STBCAL, 1,
			SHU_STBCAL_DMSTBLAT);
		/*
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM2, 0x4, SHU_ACTIM2_TR2W);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_AC_TIME_05T, 0, SHU_AC_TIME_05T_TR2W_05T);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x5, SHU_ACTIM_XRT_XRTW2R);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x6, SHU_ACTIM_XRT_XRTR2W);
		 */
	}

	/*
	 * For LP3 FFFF corner IC pass LTLV test
	 * Need to check with
	 */
	io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ5, 4,
		SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ5, 4,
		SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ5, 4,
		SHU_B2_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B2);
	io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD5, 4,
		SHU_CA_CMD5_RG_RX_ARCLK_DQSIEN_DLY);

#else
	/*
	 * Valid delay mode debug : monitor window
	 * io_32_write_fld_align(DDRPHY_SHU_B0_DQ5+SHIFT_TO_CHB_ADDR, 7, SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0);
	 * io_32_write_fld_align(DDRPHY_SHU_B1_DQ5+SHIFT_TO_CHB_ADDR, 7, SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1);
	 * io_32_write_fld_align_all(DRAMC_REG_SHU_PIPE, 0xF0000000,PHY_FLD_FULL);
	 */
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSG), \
		p_fld(9, SHU_DQSG_STB_UPDMASKCYC) | \
		p_fld(1, SHU_DQSG_STB_UPDMASK_EN));
#endif

	io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ5, CLEAR_FLD,
		SHU_B0_DQ5_RG_ARPI_FB_B0);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ5, CLEAR_FLD,
		SHU_B1_DQ5_RG_ARPI_FB_B1);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ5, CLEAR_FLD,
		SHU_B2_DQ5_RG_ARPI_FB_B2);
	io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD5, CLEAR_FLD,
		SHU_CA_CMD5_RG_ARPI_FB_CA);
#if 0
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ5 + SHIFT_TO_CHB_ADDR, 0x4,
		SHU_B0_DQ5_RG_ARPI_FB_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ5 + SHIFT_TO_CHB_ADDR, 0x4,
		SHU_B1_DQ5_RG_ARPI_FB_B1);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD5 + SHIFT_TO_CHB_ADDR, 0x4,
		SHU_CA_CMD5_RG_ARPI_FB_CA);
#endif

	io_32_write_fld_multi(DRAMC_REG_DRAMCTRL,
		p_fld(0, DRAMCTRL_FW2R) |
		p_fld(CLEAR_FLD, DRAMCTRL_DYNMWREN) |
		p_fld(CLEAR_FLD, DRAMCTRL_CLKWITRFC) |
		p_fld(CLEAR_FLD, DRAMCTRL_TMRR2WDIS) |
		p_fld(CLEAR_FLD, DRAMCTRL_ADRBIT3DEC) |
		p_fld(CLEAR_FLD, DRAMCTRL_CTOREQ_HPRI_OPT));
	io_32_write_fld_align(DRAMC_REG_MISCTL0, SET_FLD,
		MISCTL0_PBC_ARB_EN);
	io_32_write_fld_align(DRAMC_REG_PERFCTL0, CLEAR_FLD,
		PERFCTL0_WRFIFO_OPT);
	io_32_write_fld_multi(DRAMC_REG_PERFCTL0,
		p_fld(CLEAR_FLD, PERFCTL0_REORDEREN) |
		p_fld(CLEAR_FLD, PERFCTL0_RWHPRICTL) |
		p_fld(SET_FLD, PERFCTL0_EMILLATEN) |
		p_fld(SET_FLD, PERFCTL0_RWAGEEN) |
		p_fld(SET_FLD, PERFCTL0_RWHPRIEN));
	io_32_write_fld_align(DRAMC_REG_SREFCTRL, 0x8, SREFCTRL_SREFDLY);

	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, SET_FLD,
		SPCMDCTRL_REFR_BLOCKEN);
	io_32_write_fld_align(DRAMC_REG_HW_MRR_FUN, CLEAR_FLD,
		HW_MRR_FUN_TMRR_ENA);

	io_32_write_fld_multi(DRAMC_REG_SHUCTRL,
		p_fld(CLEAR_FLD, SHUCTRL_VRCG_EN) |
		p_fld(0x3, SHUCTRL_R_DVFS_PICG_MARGIN2) |
		p_fld(0x3, SHUCTRL_R_DVFS_PICG_MARGIN3));
	io_32_write_fld_multi(DRAMC_REG_SHUCTRL2,
		p_fld(SET_FLD, SHUCTRL2_SHORTQ_OPT) |
		p_fld(0x3, SHUCTRL2_R_DVFS_PICG_MARGIN) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_SREF_OPT));

	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(SET_FLD, REFCTRL0_REFNA_OPT) |
		p_fld(0x4, REFCTRL0_DISBYREFNUM) |
		p_fld(CLEAR_FLD, REFCTRL0_UPDBYWR));
	io_32_write_fld_multi(DDRPHY_B0_DQ6,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0));
	io_32_write_fld_multi(DDRPHY_CA_CMD6,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_EN));

#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	io_32_write_fld_multi(DDRPHY_B0_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
	io_32_write_fld_multi(DDRPHY_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_EN));

	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_EN_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_EN_B2));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7,
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_EN_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_EN_B2));

	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1));
	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_CA_CMD7_R_DMRXRANK_CLK_LAT) |
		p_fld(SET_FLD, SHU_CA_CMD7_R_DMRXRANK_CLK_EN) |
		p_fld(SET_FLD, SHU_CA_CMD7_R_DMRXRANK_CMD_LAT) |
		p_fld(SET_FLD, SHU_CA_CMD7_R_DMRXRANK_CMD_EN));

#else
	io_32_write_fld_align(DRAMC_REG_STBCAL2, SET_FLD,
		STBCAL2_STB_PICG_EARLY_1T_EN);

	io_32_write_fld_multi(DDRPHY_B0_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
	io_32_write_fld_multi(DDRPHY_B2_DQ6,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2));
	io_32_write_fld_multi(DDRPHY_B2_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2));
	io_32_write_fld_multi(DDRPHY_B1_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
	io_32_write_fld_multi(DDRPHY_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_EN));
#endif

	io_32_write_fld_multi_all(DDRPHY_CA_DLL_ARPI1,
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_CS_JUMP_EN) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_CLK_JUMP_EN) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_CMD_JUMP_EN) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_CLKIEN_JUMP_EN) |
		p_fld(SET_FLD, CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA) |
		p_fld(SET_FLD, CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA_REG_OPT));
	io_32_write_fld_multi_all(DDRPHY_B0_DLL_ARPI1,
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_DQS_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_DQM_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_DQSIEN_JUMP_EN_B0) |
		p_fld(SET_FLD, B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0) |
		p_fld(SET_FLD, B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0_REG_OPT));
	io_32_write_fld_multi_all(DDRPHY_B1_DLL_ARPI1,
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_DQS_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_DQM_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_DQSIEN_JUMP_EN_B1) |
		p_fld(SET_FLD, B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1) |
		p_fld(SET_FLD, B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1_REG_OPT));
	io_32_write_fld_multi_all(DDRPHY_B2_DLL_ARPI1,
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_DQS_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_DQM_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_DQSIEN_JUMP_EN_B2) |
		p_fld(SET_FLD, B2_DLL_ARPI1_RG_ARPISM_MCK_SEL_B2) |
		p_fld(SET_FLD, B2_DLL_ARPI1_RG_ARPISM_MCK_SEL_B2_REG_OPT));

	io_32_write_fld_align_all(DDRPHY_B0_DQ5, SET_FLD,
		B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ5, SET_FLD,
		B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ5, SET_FLD,
		B2_DQ5_RG_RX_ARDQS0_DVS_EN_B2);
	io_32_write_fld_align_all(DDRPHY_CA_CMD5, SET_FLD,
		CA_CMD5_RG_RX_ARCLK_DVS_EN);

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL4, 0x13300000,
		MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL4 + SHIFT_TO_CHB_ADDR,
		0x22200a00, MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);

	io_32_write_fld_multi_all(DRAMC_REG_PERFCTL0,
		p_fld(SET_FLD, PERFCTL0_RWSPLIT) |
		p_fld(CLEAR_FLD, PERFCTL0_REORDEREN));

#if NON_EXIST_RG
#if FIX_CROSSRK_XRT_05T_OPT
	io_32_write_fld_align_all(DRAMC_REG_PERFCTL0, CLEAR_FLD,
		PERFCTL0_XRT_05T_OPT);
#else
	io_32_write_fld_align_all(DRAMC_REG_PERFCTL0, SET_FLD,
		PERFCTL0_XRT_05T_OPT);
#endif
#endif
	io_32_write_fld_align_all(DRAMC_REG_CLKCTRL, SET_FLD,
		CLKCTRL_SEQCLKRUN3);
	io_32_write_fld_align_all(DRAMC_REG_STBCAL1, CLEAR_FLD,
		STBCAL1_DLLFRZ_MON_PBREF_OPT);
	io_32_write_fld_align_all(DRAMC_REG_ARBCTL, CLEAR_FLD,
		ARBCTL_DBIWR_IMP_EN );

	io_32_write_fld_align(DDRPHY_SHU_CA_CMD7, CLEAR_FLD,
		SHU_CA_CMD7_R_DMRANKRXDVS_CA);

	io_32_write_fld_align_all(DRAMC_REG_CKECTRL, SET_FLD,
		CKECTRL_CKEPBDIS);

	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ3,
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQ_PDB_PRE_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQ_PU_PRE_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQS0_PDB_PRE_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQS0_PU_PRE_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ3,
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQ_PDB_PRE_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQ_PU_PRE_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQS0_PDB_PRE_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQS0_PU_PRE_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ3,
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQ_PDB_PRE_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQ_PU_PRE_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQS0_PDB_PRE_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQS0_PU_PRE_B2));

	io_32_write_fld_align_all(DRAMC_REG_SPCMDCTRL, SET_FLD,
		SPCMDCTRL_HMR4_TOG_OPT);

	io_32_write_fld_align(DDRPHY_B0_DQ9, SET_FLD,
		B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_R_DMRXFIFO_STBENCMP_EN_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD10, SET_FLD,
		CA_CMD10_R_DMRXFIFO_STBENCMP_EN_CA);

	return DRAM_OK;
}

static void dramc_setting_lp3(DRAMC_CTX_T *p)
{
	unsigned char reg_txdly_dqs, reg_txdly_dqs_oen;
	unsigned char reg_txdly_dqdqm, reg_txdly_dqdqm_oen;
	unsigned char reg_dly_dqs, reg_dly_dqs_oen;
	unsigned char reg_dly_dqdqm, reg_dly_dqdqm_oen;

	if (p->freq_sel == DDR_DDR1600 || p->freq_sel == DDR_DDR1333) {
		reg_txdly_dqs = 0x2;
		reg_txdly_dqs_oen = 0x2;
		reg_dly_dqs = 0x3;
		reg_dly_dqs_oen = 0x1;

		reg_txdly_dqdqm = 0x2;
		reg_txdly_dqdqm_oen = 0x2;
		reg_dly_dqdqm = 0x3;
		reg_dly_dqdqm_oen = 0x1;
	} else if (p->freq_sel == DDR_DDR1200) {
		reg_txdly_dqs = 0x2;
		reg_txdly_dqs_oen = 0x1;
		reg_dly_dqs = 0x1;
		reg_dly_dqs_oen = 0x7;

		reg_txdly_dqdqm = 0x2;
		reg_txdly_dqdqm_oen = 0x1;
		reg_dly_dqdqm = 0x1;
		reg_dly_dqdqm_oen = 0x7;
	}

	auto_refresh_cke_off(p);

	io_32_write_fld_multi_all(DDRPHY_CKMUX_SEL,
		p_fld(SET_FLD, CKMUX_SEL_R_PHYCTRLMUX) |
		p_fld(SET_FLD, CKMUX_SEL_R_PHYCTRLDCM));

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, SET_FLD,
		MISC_CG_CTRL0_W_CHG_MEM);

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, CLEAR_FLD,
		MISC_CG_CTRL0_CLK_MEM_SEL);

#if NON_EXIST_RG
	io_32_write_fld_align_all(DDRPHY_MISC_CTRL0, SET_FLD,
		MISC_CTRL0_R_DMRDSEL_DIV2_OPT);
#endif

	if (0/* p->bDLP3 */) {/* cc note, review this RG!!! */
		io_32_write_fld_align_all(DDRPHY_MISC_CTRL1, SET_FLD,
			MISC_CTRL1_R_DMMUXCA_SEC);
	}

	io_32_write_fld_multi(DDRPHY_MISC_SPM_CTRL1,
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B0) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B1) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B2) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_CA));
	io_32_write_fld_multi(DDRPHY_MISC_SPM_CTRL1 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B0) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B1) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_CA));

	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL0,
		0xfbffefff, MISC_SPM_CTRL0_PHY_SPM_CTL0);
	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL0 + SHIFT_TO_CHB_ADDR,
		0x03fe75c3, MISC_SPM_CTRL0_PHY_SPM_CTL0);

	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL2, 0x7fffffef,
		MISC_SPM_CTRL2_PHY_SPM_CTL2);
	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL2 + SHIFT_TO_CHB_ADDR,
		0x7e4e4000, MISC_SPM_CTRL2_PHY_SPM_CTL2);

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL2, 0x6003bf,
		MISC_CG_CTRL2_RG_MEM_DCM_CTL);
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL4, 0x13300000,
		MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL4 + SHIFT_TO_CHB_ADDR,
		0x222a2a00, MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);
	io_32_write_fld_multi(DDRPHY_SHU_PLL1,
		p_fld(SET_FLD, SHU_PLL1_R_SHU_AUTO_PLL_MUX) |
		p_fld(0x7, SHU_PLL1_SHU1_PLL1_RFU));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_MIDPI_DIV4_ENABLE) |
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRANKRXDVS_B0));
	io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ7,
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRANKRXDVS_B1));
	io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ7,
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_MIDPI_DIV4_ENABLE) |
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRANKRXDVS_B2));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_MIDPI_DIV4_ENABLE) |
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRANKRXDVS_B0));
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRANKRXDVS_B1));
#endif
	/* cc add since this bit default is 1'b1, which will cause CS blocked */
	io_32_write_fld_align(DRAMC_REG_SLP4_TESTMODE, CLEAR_FLD,
		SLP4_TESTMODE_DQM_FIX_LOW);

	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_CA_CMD7_R_DMRXDVS_PBYTE_FLAG_OPT_CA) |
		p_fld(SET_FLD, SHU_CA_CMD7_R_DMRANKRXDVS_CA));
	/*
	 * io_32_write_fld_align(DDRPHY_SHU_PLL4, 0xfe, SHU_PLL4_RG_RPHYPLL_RESERVED);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL6, 0xfe, SHU_PLL6_RG_RCLRPLL_RESERVED);
	 */
	io_32_write_fld_multi_all(DDRPHY_CA_CMD3,
		p_fld(CLEAR_FLD, CA_CMD3_RG_TX_ARCMD_OE_DIS) |
		p_fld(SET_FLD, CA_CMD3_RG_TX_ARCMD_ODTEN_DIS) |
		p_fld(CLEAR_FLD, CA_CMD3_RG_TX_ARCLK_OE_DIS) |
		p_fld(SET_FLD, CA_CMD3_RG_TX_ARCLK_ODTEN_DIS));
	io_32_write_fld_multi_all(DDRPHY_B0_DQ2,
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQ_OE_DIS_B0) |
		p_fld(SET_FLD, B0_DQ2_RG_TX_ARDQ_ODTEN_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQS0_OE_DIS_B0) |
		p_fld(SET_FLD, B0_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B0));
	io_32_write_fld_multi_all(DDRPHY_B1_DQ2,
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQ_OE_DIS_B1) |
		p_fld(SET_FLD, B1_DQ2_RG_TX_ARDQ_ODTEN_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQS0_OE_DIS_B1) |
		p_fld(SET_FLD, B1_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B1));
	io_32_write_fld_multi_all(DDRPHY_B2_DQ2,
		p_fld(CLEAR_FLD, B2_DQ2_RG_TX_ARDQ_OE_DIS_B2) |
		p_fld(SET_FLD, B2_DQ2_RG_TX_ARDQ_ODTEN_DIS_B2) |
		p_fld(CLEAR_FLD, B2_DQ2_RG_TX_ARDQS0_OE_DIS_B2) |
		p_fld(SET_FLD, B2_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B2));

#if 0
	io_32_write_fld_align(DDRPHY_B0_DQ9, CLEAR_FLD,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9, 0x7,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_CA_CMD10, CLEAR_FLD,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
#endif
#if 0
	io_32_write_fld_multi(DDRPHY_CA_CMD2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCMD_OE_DIS) |
		p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCMD_ODTEN_DIS) |
		p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCLK_OE_DIS) |
		p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCLK_ODTEN_DIS));
	io_32_write_fld_multi(DDRPHY_B0_DQ2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQ_OE_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQ_ODTEN_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQS_OE_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQS_ODTEN_DIS_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQ_OE_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQ_ODTEN_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQS_OE_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQS_ODTEN_DIS_B1));
#endif
#if 0
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, 0x7,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
#endif
	if (p->is_emcp) {
		io_32_write_fld_multi_all(DDRPHY_MISC_CTRL1,
			p_fld(0x2, MISC_CTRL1_R_DMPINMUX) |
			p_fld(CLEAR_FLD, MISC_CTRL1_R_DMARPIDQ_SW) |
			p_fld(CLEAR_FLD, MISC_CTRL1_R_DMMCTLPLL_CKSEL));
	} else {
		io_32_write_fld_multi_all(DDRPHY_MISC_CTRL1,
			p_fld(0x1, MISC_CTRL1_R_DMPINMUX) |
			p_fld(CLEAR_FLD, MISC_CTRL1_R_DMARPIDQ_SW) |
			p_fld(CLEAR_FLD, MISC_CTRL1_R_DMMCTLPLL_CKSEL));
	}
#if 0
	io_32_write_fld_multi(DDRPHY_MISC_CTRL1 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, MISC_CTRL1_R_DMPINMUX) |
		p_fld(CLEAR_FLD, MISC_CTRL1_R_DMARPIDQ_SW) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMMCTLPLL_CKSEL));
#endif
	io_32_write_fld_align(DDRPHY_PLL3, CLEAR_FLD,
		PLL3_RG_RPHYPLL_TSTOP_EN);

	io_32_write_fld_align_all(DDRPHY_MISC_VREF_CTRL, SET_FLD,
		MISC_VREF_CTRL_RG_RVREF_VREF_EN);
	io_32_write_fld_align_all(DDRPHY_MISC_VREF_CTRL, 0xfffe,
		MISC_VREF_CTRL_PHY_SPM_CTL4);
	io_32_write_fld_multi_all(DDRPHY_SHU_MISC0,
		p_fld(CLEAR_FLD, SHU_MISC0_RG_RVREF_SEL_CMD) |
		p_fld(SET_FLD, SHU_MISC0_RG_RVREF_DDR3_SEL) |
		p_fld(CLEAR_FLD, SHU_MISC0_RG_RVREF_DDR4_SEL) |
		p_fld(0xb, SHU_MISC0_RG_RVREF_SEL_DQ));
	io_32_write_fld_align_all(DDRPHY_MISC_IMP_CTRL0, SET_FLD,
		MISC_IMP_CTRL0_RG_RIMP_DDR3_SEL);
#if 0
	io_32_write_fld_align(DDRPHY_MISC_VREF_CTRL + SHIFT_TO_CHB_ADDR,
		SET_FLD, MISC_VREF_CTRL_RG_RVREF_VREF_EN);
	io_32_write_fld_multi(DDRPHY_SHU_MISC0 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_MISC0_RG_RVREF_SEL_CMD) |
		p_fld(SET_FLD, SHU_MISC0_RG_RVREF_DDR3_SEL) |
		p_fld(CLEAR_FLD, SHU_MISC0_RG_RVREF_DDR4_SEL) |
		p_fld(0xb, SHU_MISC0_RG_RVREF_SEL_DQ));
	io_32_write_fld_align(DDRPHY_MISC_IMP_CTRL0 + SHIFT_TO_CHB_ADDR,
		SET_FLD, MISC_IMP_CTRL0_RG_RIMP_DDR3_SEL);
#endif
	io_32_write_fld_align(DDRPHY_B2_RXDVS0, SET_FLD,
		B2_RXDVS0_R_RX_DLY_TRACK_CG_EN_B2);
	io_32_write_fld_align(DDRPHY_B1_RXDVS0, SET_FLD,
		B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1);
	io_32_write_fld_multi(DDRPHY_B0_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0) |
		p_fld(CLEAR_FLD, B0_RXDVS0_R_RX_RANKINCTL_B0));
	io_32_write_fld_multi(DDRPHY_CA_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_RXDVS0_R_RX_DLY_TRACK_CG_EN_CA) |
		p_fld(CLEAR_FLD, CA_RXDVS0_R_RX_RANKINCTL_CA));
	io_32_write_fld_multi(DDRPHY_B2_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B2_RXDVS0_R_RX_DLY_TRACK_CG_EN_B2) |
		p_fld(CLEAR_FLD, B2_RXDVS0_R_RX_RANKINCTL_B2));
	io_32_write_fld_multi(DDRPHY_B1_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1) |
		p_fld(CLEAR_FLD, B1_RXDVS0_R_RX_RANKINCTL_B1));

	io_32_write_fld_multi(DDRPHY_B2_RXDVS0,
		p_fld(SET_FLD, B2_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B2) |
		p_fld(SET_FLD, B2_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B2));
	io_32_write_fld_multi(DDRPHY_B1_RXDVS0,
		p_fld(SET_FLD, B1_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B1) |
		p_fld(SET_FLD, B1_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B1));
	io_32_write_fld_multi(DDRPHY_B0_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B0) |
		p_fld(SET_FLD, B0_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B0));
	io_32_write_fld_multi(DDRPHY_B2_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B2_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B2) |
		p_fld(SET_FLD, B2_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B2));
	io_32_write_fld_multi(DDRPHY_B1_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B1) |
		p_fld(SET_FLD, B1_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B1));
	io_32_write_fld_multi(DDRPHY_CA_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_RXDVS0_R_DMRXDVS_CNTCMP_OPT_CA) |
		p_fld(SET_FLD, CA_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_CA));
#if 0
	io_32_write_fld_align(DDRPHY_R0_B1_RXDVS2, SET_FLD,
		R0_B1_RXDVS2_R_RK0_DVS_FDLY_MODE_B1);
	io_32_write_fld_align(DDRPHY_R1_B1_RXDVS2, SET_FLD,
		R1_B1_RXDVS2_R_RK1_DVS_FDLY_MODE_B1);
	io_32_write_fld_align(DDRPHY_R0_B0_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R0_B0_RXDVS2_R_RK0_DVS_FDLY_MODE_B0);
	io_32_write_fld_align(DDRPHY_R1_B0_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R1_B0_RXDVS2_R_RK1_DVS_FDLY_MODE_B0);

	io_32_write_fld_align(DDRPHY_R0_CA_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R0_CA_RXDVS2_R_RK0_DVS_FDLY_MODE_CA);
	io_32_write_fld_align(DDRPHY_R1_CA_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R1_CA_RXDVS2_R_RK1_DVS_FDLY_MODE_CA);
	io_32_write_fld_align(DDRPHY_R0_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R0_B1_RXDVS2_R_RK0_DVS_FDLY_MODE_B1);
	io_32_write_fld_align(DDRPHY_R1_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R1_B1_RXDVS2_R_RK1_DVS_FDLY_MODE_B1);
#endif
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ5, CLEAR_FLD,
		SHU_B2_DQ5_RG_RX_ARDQS0_DVS_DLY_B2);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ5, CLEAR_FLD,
		SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1);
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0));
	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_CA_CMD5_RG_RX_ARCLK_DVS_DLY) |
		p_fld(CLEAR_FLD, SHU_CA_CMD5_RG_RX_ARCMD_VREF_SEL));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B2_DQ5_RG_RX_ARDQS0_DVS_DLY_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1));

#if 0//cc mark LEGACY_RX_DLY
	LegacyRxDly_LP3(p);
#endif
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD2_RG_RK0_RX_ARCA1_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD2_RG_RK0_RX_ARCA1_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD2_RG_RK0_RX_ARCA0_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD2_RG_RK0_RX_ARCA0_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD3 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD3_RG_RK0_RX_ARCA3_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD3_RG_RK0_RX_ARCA3_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD3_RG_RK0_RX_ARCA2_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD3_RG_RK0_RX_ARCA2_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD4_RG_RK0_RX_ARCA5_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD4_RG_RK0_RX_ARCA5_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD4_RG_RK0_RX_ARCA4_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD4_RG_RK0_RX_ARCA4_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE1_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE1_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE0_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE0_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD6_RG_RK0_RX_ARCS0_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD6_RG_RK0_RX_ARCS0_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD6_RG_RK0_RX_ARCKE2_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD6_RG_RK0_RX_ARCKE2_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD7 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD7_RG_RK0_RX_ARCS2_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD7_RG_RK0_RX_ARCS2_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD7_RG_RK0_RX_ARCS1_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD7_RG_RK0_RX_ARCS1_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD8 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD8_RG_RK0_RX_ARCLK_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD8_RG_RK0_RX_ARCLK_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD2_RG_RK1_RX_ARCA1_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD2_RG_RK1_RX_ARCA1_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD2_RG_RK1_RX_ARCA0_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD2_RG_RK1_RX_ARCA0_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD3 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD3_RG_RK1_RX_ARCA3_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD3_RG_RK1_RX_ARCA3_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD3_RG_RK1_RX_ARCA2_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD3_RG_RK1_RX_ARCA2_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD4_RG_RK1_RX_ARCA5_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD4_RG_RK1_RX_ARCA5_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD4_RG_RK1_RX_ARCA4_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD4_RG_RK1_RX_ARCA4_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE1_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE1_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE0_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE0_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD6_RG_RK1_RX_ARCS0_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD6_RG_RK1_RX_ARCS0_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD6_RG_RK1_RX_ARCKE2_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD6_RG_RK1_RX_ARCKE2_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD7 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD7_RG_RK1_RX_ARCS2_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD7_RG_RK1_RX_ARCS2_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD7_RG_RK1_RX_ARCS1_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD7_RG_RK1_RX_ARCS1_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD8 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD8_RG_RK1_RX_ARCLK_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD8_RG_RK1_RX_ARCLK_R_DLY));

#if 0
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS1,
		p_fld(0x4, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LEAD) |
		p_fld(0x4, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS1,
		p_fld(0x4, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LEAD) |
		p_fld(0x4, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R0_B0_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R0_B0_RXDVS1_R_RK0_B0_DVS_TH_LEAD) |
		p_fld(0x4, R0_B0_RXDVS1_R_RK0_B0_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_B0_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R1_B0_RXDVS1_R_RK1_B0_DVS_TH_LEAD) |
		p_fld(0x4, R1_B0_RXDVS1_R_RK1_B0_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R0_CA_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R0_CA_RXDVS1_R_RK0_CA_DVS_TH_LEAD) |
		p_fld(0x4, R0_CA_RXDVS1_R_RK0_CA_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_CA_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R1_CA_RXDVS1_R_RK1_CA_DVS_TH_LEAD) |
		p_fld(0x4, R1_CA_RXDVS1_R_RK1_CA_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LEAD) |
		p_fld(0x4, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LEAD) |
		p_fld(0x4, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LAG));
#endif


	io_32_write_fld_multi_all(DDRPHY_R0_B2_RXDVS2,
		p_fld(0x2, R0_B2_RXDVS2_R_RK0_DVS_MODE_B2) |
		p_fld(SET_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(SET_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B2));
	io_32_write_fld_multi_all(DDRPHY_R1_B2_RXDVS2,
		p_fld(0x2, R1_B2_RXDVS2_R_RK1_DVS_MODE_B2) |
		p_fld(SET_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(SET_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B2));
	io_32_write_fld_multi_all(DDRPHY_R0_B1_RXDVS2,
		p_fld(0x2, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
		p_fld(SET_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(SET_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi_all(DDRPHY_R1_B1_RXDVS2,
		p_fld(0x2, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1) |
		p_fld(SET_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(SET_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi_all(DDRPHY_R0_B0_RXDVS2,
		p_fld(0x2, R0_B0_RXDVS2_R_RK0_DVS_MODE_B0) |
		p_fld(SET_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(SET_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi_all(DDRPHY_R1_B0_RXDVS2,
		p_fld(0x2, R1_B0_RXDVS2_R_RK1_DVS_MODE_B0) |
		p_fld(SET_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(SET_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi_all(DDRPHY_R0_CA_RXDVS2,
		p_fld(0x2, R0_CA_RXDVS2_R_RK0_DVS_MODE_CA) |
		p_fld(SET_FLD, R0_CA_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_CA) |
		p_fld(SET_FLD, R0_CA_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_CA));
	io_32_write_fld_multi_all(DDRPHY_R1_CA_RXDVS2,
		p_fld(0x2, R1_CA_RXDVS2_R_RK1_DVS_MODE_CA) |
		p_fld(SET_FLD, R1_CA_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_CA) |
		p_fld(SET_FLD, R1_CA_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_CA));
#if 0
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
		p_fld(SET_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(SET_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1) |
		p_fld(SET_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(SET_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
#endif
	io_32_write_fld_align_all(DDRPHY_B2_RXDVS0, CLEAR_FLD,
		B2_RXDVS0_R_RX_DLY_TRACK_CG_EN_B2);
	io_32_write_fld_align_all(DDRPHY_B1_RXDVS0, CLEAR_FLD,
		B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1);
	io_32_write_fld_align(DDRPHY_B0_RXDVS0 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0);
	io_32_write_fld_align(DDRPHY_CA_RXDVS0 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		CA_RXDVS0_R_RX_DLY_TRACK_CG_EN_CA);

	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_RG_RX_ARDQ_STBEN_RESETB_B2);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1);
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, SET_FLD,
		CA_CMD10_RG_RX_ARCMD_STBEN_RESETB);
	io_32_write_fld_align(DDRPHY_B2_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B2_DQ9_RG_RX_ARDQ_STBEN_RESETB_B2);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1);
	io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL1, CLEAR_FLD,
		MISC_CG_CTRL1_R_DVS_DIV4_CG_CTRL);

	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD9,
		p_fld(0xf, SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
		p_fld(0, SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK) |
		p_fld(0, SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD9,
		p_fld(0xf, SHU_R1_CA_CMD9_RG_RK1_ARPI_CMD) |
		p_fld(0, SHU_R1_CA_CMD9_RG_RK1_ARPI_CLK) |
		p_fld(0, SHU_R1_CA_CMD9_RG_RK1_ARPI_CS));
	io_32_write_fld_align(DDRPHY_SHU_R0_B1_DQ7,	0xf,
		SHU_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);
	io_32_write_fld_align(DDRPHY_SHU_R1_B1_DQ7,	0xf,
		SHU_R1_B1_DQ7_RK1_ARPI_PBYTE_B1);

#if 0//cc mark LEGACY_DELAY_CELL
	LegacyDlyCellInitLP3(p);
#endif
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x6, SHU_R1_CA_CMD1_RK1_TX_ARCKE2_DLY) |
		p_fld(0x6, SHU_R1_CA_CMD1_RK1_TX_ARCKE1_DLY) |
		p_fld(0x6, SHU_R1_CA_CMD1_RK1_TX_ARCKE0_DLY));

	io_32_write_fld_align(DDRPHY_SHU_R0_B0_DQ7, 0xf,
		SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0);
	io_32_write_fld_align(DDRPHY_SHU_R1_B0_DQ7, 0xf,
		SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0);

	io_32_write_fld_multi_all(DDRPHY_SHU_R0_B2_DQ7,
		p_fld(0xf, SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2) |
		p_fld(0xf, SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2));
	io_32_write_fld_multi_all(DDRPHY_SHU_R1_B2_DQ7,
		p_fld(0xf, SHU_R1_B2_DQ7_RK1_ARPI_DQM_B2) |
		p_fld(0xf, SHU_R1_B2_DQ7_RK1_ARPI_DQ_B2));
	io_32_write_fld_multi_all(DDRPHY_SHU_R0_B1_DQ7,
		p_fld(0xf, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
		p_fld(0xf, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	io_32_write_fld_multi_all(DDRPHY_SHU_R1_B1_DQ7,
		p_fld(0xf, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1) |
		p_fld(0xf, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD9 + SHIFT_TO_CHB_ADDR,
		p_fld(0xf, SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
		p_fld(0x0, SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD9 + SHIFT_TO_CHB_ADDR,
		p_fld(0xf, SHU_R1_CA_CMD9_RG_RK1_ARPI_CMD) |
		p_fld(0x0, SHU_R1_CA_CMD9_RG_RK1_ARPI_CS));
	io_32_write_fld_multi(DDRPHY_SHU_R0_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(0xf, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
		p_fld(0xf, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(0xf, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0) |
		p_fld(0xf, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_R0_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(0xf, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
		p_fld(0xf, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(0xf, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1) |
		p_fld(0xf, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
#endif

	io_32_write_fld_multi(DDRPHY_B2_DQ4,
		p_fld(0x10, B2_DQ4_RG_RX_ARDQS_EYE_R_DLY_B2) |
		p_fld(0x10, B2_DQ4_RG_RX_ARDQS_EYE_F_DLY_B2));
	io_32_write_fld_multi(DDRPHY_B2_DQ5,
		p_fld(CLEAR_FLD, B2_DQ5_RG_RX_ARDQ_EYE_EN_B2) |
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_EYE_SEL_B2) |
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_VREF_EN_B2) |
		p_fld(0x10, B2_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B2) |
		p_fld(0x10, B2_DQ5_B2_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B2_DQ6,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B2) |
		p_fld(CLEAR_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2));
	io_32_write_fld_multi(DDRPHY_B2_DQ5,
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B2) |
		p_fld(CLEAR_FLD, B2_DQ5_B2_DQ5_RFU));

	io_32_write_fld_multi(DDRPHY_B1_DQ4,
		p_fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1) |
		p_fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ5,
		p_fld(CLEAR_FLD, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1) |
		p_fld(0x10, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1) |
		p_fld(0x10, B1_DQ5_B1_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1) |
		p_fld(CLEAR_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ5,
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) |
		p_fld(CLEAR_FLD, B1_DQ5_B1_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_CA_CMD3,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_IN_BUFF_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_ARCMD_RESETB) |
		p_fld(SET_FLD, CA_CMD3_RG_TX_ARCMD_EN));
	io_32_write_fld_multi(DDRPHY_CA_CMD6,
		p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_DDR3_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_TX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_VREF_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN));
#if NON_EXIST_RG
		p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_SER_MODE));
#endif
	/*
	 * ARCMD_DRVP, DRVN , ARCLK_DRVP, DRVN not used anymore
	 * io_32_write_fld_multi(DDRPHY_SHU_CA_CMD1,
		p_fld(SET_FLD, SHU_CA_CMD1_RG_TX_ARCMD_DRVN) |
	 * | p_fld(SET_FLD, SHU_CA_CMD1_RG_TX_ARCMD_DRVP));
	 * io_32_write_fld_multi(DDRPHY_SHU_CA_CMD2,
		p_fld(SET_FLD, SHU_CA_CMD2_RG_TX_ARCLK_DRVN) |
	 * | p_fld(SET_FLD, SHU_CA_CMD2_RG_TX_ARCLK_DRVP));
	 */
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU2_CA_CMD1,
		p_fld(SET_FLD, SHU2_CA_CMD1_RG_TX_ARCMD_DRVN) |
	 * | p_fld(SET_FLD, SHU2_CA_CMD1_RG_TX_ARCMD_DRVP));
	 * io_32_write_fld_multi(DDRPHY_SHU2_CA_CMD2,
		p_fld(SET_FLD, SHU2_CA_CMD2_RG_TX_ARCLK_DRVN) |
	 * | p_fld(SET_FLD, SHU2_CA_CMD2_RG_TX_ARCLK_DRVP));
	 */
	io_32_write_fld_multi(DDRPHY_B0_DQ3,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_ARDQ_RESETB_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_TX_ARDQ_EN_B0));
	io_32_write_fld_multi(DDRPHY_B0_DQ6,
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ1,
		p_fld(SET_FLD, SHU_B0_DQ1_RG_TX_ARDQ_DRVN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ1_RG_TX_ARDQ_DRVP_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ2,
		p_fld(SET_FLD, SHU_B0_DQ2_RG_TX_ARDQS0_DRVN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ2_RG_TX_ARDQS0_DRVP_B0));
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU2_B0_DQ1,
		p_fld(SET_FLD, SHU2_B0_DQ1_RG_TX_ARDQ_DRVN_B0) |
	 * | p_fld(SET_FLD, SHU2_B0_DQ1_RG_TX_ARDQ_DRVP_B0));
	 * io_32_write_fld_multi(DDRPHY_SHU2_B0_DQ2,
		p_fld(SET_FLD, SHU2_B0_DQ2_RG_TX_ARDQS0_DRVN_B0) |
	 * | p_fld(SET_FLD, SHU2_B0_DQ2_RG_TX_ARDQS0_DRVP_B0));
	 */
	io_32_write_fld_multi(DDRPHY_CA_CMD4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x10, CA_CMD4_RG_RX_ARCLK_EYE_R_DLY) |
		p_fld(0x10, CA_CMD4_RG_RX_ARCLK_EYE_F_DLY));
	io_32_write_fld_multi(DDRPHY_CA_CMD5,
		p_fld(CLEAR_FLD, CA_CMD5_RG_RX_ARCMD_EYE_EN));
	io_32_write_fld_multi(DDRPHY_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, CA_CMD5_RG_RX_ARCMD_EYE_EN) |
		p_fld(SET_FLD, CA_CMD5_RG_RX_ARCMD_EYE_SEL) |
		p_fld(SET_FLD, CA_CMD5_RG_RX_ARCMD_VREF_EN) |
		p_fld(0x10, CA_CMD5_RG_RX_ARCMD_EYE_VREF_SEL) |
		p_fld(0x10, CA_CMD5_CA_CMD5_RFU));
	io_32_write_fld_multi(DDRPHY_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_EYE_DLY_DQS_BYPASS) |
		p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_DDR3_SEL) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_DDR3_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_TX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_VREF_SEL) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_EN) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN));
		//cc mark p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_SER_MODE));
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD6, SET_FLD,
		SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		SET_FLD, SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE);

	io_32_write_fld_multi(DDRPHY_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD5_RG_RX_ARCMD_EYE_STBEN_RESETB) |
		p_fld(CLEAR_FLD, CA_CMD5_CA_CMD5_RFU));
	io_32_write_fld_multi(DDRPHY_B0_DQ4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x10, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0) |
		p_fld(0x10, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0));

	io_32_write_fld_multi(DDRPHY_B0_DQ5,
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0));

	io_32_write_fld_multi(DDRPHY_B0_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0) |
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_EYE_SEL_B0) |
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0) |
		p_fld(0x10, B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0) |
		p_fld(0x10, B0_DQ5_B0_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B0_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0));
	io_32_write_fld_multi(DDRPHY_B0_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0) |
		p_fld(CLEAR_FLD, B0_DQ5_B0_DQ5_RFU));

	io_32_write_fld_align(DDRPHY_SHU_B0_DQ6, SET_FLD,
		SHU_B0_DQ6_RG_TX_ARDQ_SER_MODE_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ6, SET_FLD,
		SHU_B1_DQ6_RG_TX_ARDQ_SER_MODE_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ6, SET_FLD,
		SHU_B2_DQ6_RG_TX_ARDQ_SER_MODE_B2);
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ6 + SHIFT_TO_CHB_ADDR, SET_FLD,
		SHU_B0_DQ6_RG_TX_ARDQ_SER_MODE_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ6 + SHIFT_TO_CHB_ADDR, SET_FLD,
		SHU_B1_DQ6_RG_TX_ARDQ_SER_MODE_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ6 + SHIFT_TO_CHB_ADDR, SET_FLD,
		SHU_B2_DQ6_RG_TX_ARDQ_SER_MODE_B2);
	io_32_write_fld_multi(DDRPHY_B2_DQ4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x10, B2_DQ4_RG_RX_ARDQS_EYE_R_DLY_B2) |
		p_fld(0x10, B2_DQ4_RG_RX_ARDQS_EYE_F_DLY_B2));
	io_32_write_fld_multi(DDRPHY_B2_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B2_DQ5_RG_RX_ARDQ_EYE_EN_B2) |
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_EYE_SEL_B2) |
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_VREF_EN_B2) |
		p_fld(0x10, B2_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B2) |
		p_fld(0x10, B2_DQ5_B2_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B2_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B2) |
		p_fld(CLEAR_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2));
	io_32_write_fld_multi(DDRPHY_B2_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B2) |
		p_fld(CLEAR_FLD, B2_DQ5_B2_DQ5_RFU));

	io_32_write_fld_multi(DDRPHY_B1_DQ4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1) |
		p_fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1) |
		p_fld(0x10, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1) |
		p_fld(0x10, B1_DQ5_B1_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B1_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1) |
		p_fld(CLEAR_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) |
		p_fld(CLEAR_FLD, B1_DQ5_B1_DQ5_RFU));

	io_32_write_fld_multi(DDRPHY_PLL4,
		p_fld(CLEAR_FLD, PLL4_RG_RPHYPLL_AD_MCK8X_EN) |
		p_fld(SET_FLD, PLL4_PLL4_RFU) |
		p_fld(SET_FLD, PLL4_RG_RPHYPLL_MCK8X_SEL));

	io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI1,
		p_fld(SET_FLD, CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA) |
		p_fld(SET_FLD, CA_DLL_ARPI1_RG_ARPI_CS_JUMP_EN) |
		p_fld(SET_FLD, CA_DLL_ARPI1_RG_ARPI_CLK_JUMP_EN));
	io_32_write_fld_multi(DDRPHY_B0_DLL_ARPI1,
		p_fld(SET_FLD, B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0) |
		p_fld(SET_FLD, B0_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B0));
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI1, SET_FLD,
		B1_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B1);

	io_32_write_fld_align(DDRPHY_CA_DLL_ARPI1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, CA_DLL_ARPI1_RG_ARPI_CMD_JUMP_EN);
	io_32_write_fld_align(DDRPHY_B0_DLL_ARPI1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, B0_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B0);
#if 0
	io_32_write_fld_align(DDRPHY_B1_DLL_ARPI1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, B1_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B1);
	io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, CA_DLL_ARPI2_RG_ARPI_CG_FB_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));
	io_32_write_fld_multi(DDRPHY_B0_DLL_ARPI2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B0_DLL_ARPI2_RG_ARPI_CG_FB_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
	io_32_write_fld_multi(DDRPHY_B1_DLL_ARPI2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B1_DLL_ARPI2_RG_ARPI_CG_FB_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));
#endif

	io_32_write_fld_multi_all(DDRPHY_CA_CMD3,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_STBENCMP_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_ARCMD_RESETB) |
		p_fld(SET_FLD, CA_CMD3_RG_TX_ARCMD_EN) |
		p_fld(CLEAR_FLD, CA_CMD3_RG_RX_ARCMD_SMT_EN));
	io_32_write_fld_multi_all(DDRPHY_B0_DQ3,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_STBENCMP_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_ARDQ_RESETB_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_TX_ARDQ_EN_B0) |
		p_fld(CLEAR_FLD, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0));
	io_32_write_fld_multi_all(DDRPHY_B1_DQ3,
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_STBENCMP_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_ARDQ_RESETB_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_TX_ARDQ_EN_B1) |
		p_fld(CLEAR_FLD, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1));
	io_32_write_fld_multi_all(DDRPHY_B2_DQ3,
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQ_STBENCMP_EN_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_ARDQ_RESETB_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_TX_ARDQ_EN_B2) |
		p_fld(CLEAR_FLD, B2_DQ3_RG_RX_ARDQ_SMT_EN_B2));
#if 0
	io_32_write_fld_multi(DDRPHY_CA_CMD3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_STBENCMP_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_ARCMD_RESETB) |
		p_fld(SET_FLD, CA_CMD3_RG_TX_ARCMD_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_SMT_EN));
	io_32_write_fld_multi(DDRPHY_B0_DQ3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_STBENCMP_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_ARDQ_RESETB_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_TX_ARDQ_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_STBENCMP_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_ARDQ_RESETB_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_TX_ARDQ_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1));
#endif
	io_32_write_fld_align_all(DDRPHY_SHU_CA_DLL0, CLEAR_FLD,
		SHU_CA_DLL0_RG_ARDLL_DIV_MCTL_CA);
	io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL0, CLEAR_FLD,
		SHU_B0_DLL0_RG_ARDLL_DIV_MCTL_B0);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL0, CLEAR_FLD,
		SHU_B1_DLL0_RG_ARDLL_DIV_MCTL_B1);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL2, CLEAR_FLD,
		SHU_B1_DLL0_RG_ARDLL_DIV_MCTL_B1);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL0, CLEAR_FLD,
		SHU_B2_DLL0_RG_ARDLL_DIV_MCTL_B2);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL2, CLEAR_FLD,
		SHU_B2_DLL0_RG_ARDLL_DIV_MCTL_B2);
#if 0
	io_32_write_fld_align(DDRPHY_CA_DLL_ARPI5 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, CA_DLL_ARPI5_RG_ARDLL_DIV_MCTL_CA);
	io_32_write_fld_align(DDRPHY_B0_DLL_ARPI5 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, B0_DLL_ARPI5_RG_ARDLL_DIV_MCTL_B0);
	io_32_write_fld_align(DDRPHY_B1_DLL_ARPI5 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, B1_DLL_ARPI5_RG_ARDLL_DIV_MCTL_B1);
#endif

	io_32_write_fld_multi(DDRPHY_SHU_CA_DLL0,
		p_fld(0x9, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA) |
		p_fld(0x8, SHU_CA_DLL0_RG_ARDLL_P_GAIN_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHJUMP_EN_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDIV_CA));
	io_32_write_fld_multi_all(DDRPHY_SHU_B0_DLL0,
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHDET_IN_SWAP_B0) |
		p_fld(0x7, SHU_B0_DLL0_RG_ARDLL_GAIN_B0) |
		p_fld(0x7, SHU_B0_DLL0_RG_ARDLL_IDLECNT_B0) |
		p_fld(0x8, SHU_B0_DLL0_RG_ARDLL_P_GAIN_B0) |
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHJUMP_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHDIV_B0));
	io_32_write_fld_multi_all(DDRPHY_SHU_B1_DLL0,
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHDET_IN_SWAP_B1) |
		p_fld(0x7, SHU_B1_DLL0_RG_ARDLL_GAIN_B1) |
		p_fld(0x7, SHU_B1_DLL0_RG_ARDLL_IDLECNT_B1) |
		p_fld(0x8, SHU_B1_DLL0_RG_ARDLL_P_GAIN_B1) |
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHJUMP_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHDIV_B1));
	io_32_write_fld_multi_all(DDRPHY_SHU_B2_DLL0,
		p_fld(SET_FLD, SHU_B2_DLL0_RG_ARDLL_PHDET_IN_SWAP_B2) |
		p_fld(0x7, SHU_B2_DLL0_RG_ARDLL_GAIN_B2) |
		p_fld(0x7, SHU_B2_DLL0_RG_ARDLL_IDLECNT_B2) |
		p_fld(0x8, SHU_B2_DLL0_RG_ARDLL_P_GAIN_B2) |
		p_fld(SET_FLD, SHU_B2_DLL0_RG_ARDLL_PHJUMP_EN_B2) |
		p_fld(SET_FLD, SHU_B2_DLL0_RG_ARDLL_PHDIV_B2));

	io_32_write_fld_multi(DDRPHY_SHU_CA_DLL0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDET_OUT_SEL_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDET_IN_SWAP_CA) |
		p_fld(0x7, SHU_CA_DLL0_RG_ARDLL_GAIN_CA) |
		p_fld(0x7, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA) |
		p_fld(0x8, SHU_CA_DLL0_RG_ARDLL_P_GAIN_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHJUMP_EN_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDIV_CA));
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_B0_DLL0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHDET_IN_SWAP_B0) |
		p_fld(0x7, SHU_B0_DLL0_RG_ARDLL_GAIN_B0) |
		p_fld(0x7, SHU_B0_DLL0_RG_ARDLL_IDLECNT_B0) |
		p_fld(0x8, SHU_B0_DLL0_RG_ARDLL_P_GAIN_B0) |
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHJUMP_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHDIV_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DLL0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHDET_IN_SWAP_B1) |
		p_fld(0x7, SHU_B1_DLL0_RG_ARDLL_GAIN_B1) |
		p_fld(0x7, SHU_B1_DLL0_RG_ARDLL_IDLECNT_B1) |
		p_fld(0x8, SHU_B1_DLL0_RG_ARDLL_P_GAIN_B1) |
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHJUMP_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHDIV_B1));
#endif

	io_32_write_fld_multi_all(DDRPHY_CA_CMD8,
		p_fld(SET_FLD, CA_CMD8_RG_TX_RRESETB_DDR3_SEL) |
		p_fld(0xa, CA_CMD8_RG_RRESETB_DRVN) |
		p_fld(0xa, CA_CMD8_RG_RRESETB_DRVP));
	io_32_write_fld_multi_all(DDRPHY_CA_TX_MCK,
		p_fld(CLEAR_FLD, CA_TX_MCK_R_DMRESET_FRPHY_OPT) |
		p_fld(0xa, CA_TX_MCK_R_DMRESETB_DRVP_FRPHY) |
		p_fld(0xa, CA_TX_MCK_R_DMRESETB_DRVN_FRPHY));
#if 0
	io_32_write_fld_multi(DDRPHY_CA_CMD8 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD8_RG_TX_RRESETB_DDR3_SEL) |
		p_fld(SET_FLD, CA_CMD8_RG_RRESETB_DRVN) |
		p_fld(SET_FLD, CA_CMD8_RG_RRESETB_DRVP));
#endif
	delay_us(1);

	io_32_write_fld_align_all(DDRPHY_SHU_PLL0, 0x3,
		SHU_PLL0_RG_RPHYPLL_TOP_REV);
	/*
	 * io_32_write_fld_align(DDRPHY_SHU2_PLL0, 0x3, SHU2_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU3_PLL0, 0x3, SHU3_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU4_PLL0, 0x3, SHU4_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL0+SHIFT_TO_CHB_ADDR, 0x3, SHU_PLL0_RG_RPHYPLL_TOP_REV); //Use _All() instead
	 * io_32_write_fld_align(DDRPHY_SHU2_PLL0+SHIFT_TO_CHB_ADDR, 0x3, SHU2_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU3_PLL0+SHIFT_TO_CHB_ADDR, 0x3, SHU3_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU4_PLL0+SHIFT_TO_CHB_ADDR, 0x3, SHU4_PLL0_RG_RPHYPLL_TOP_REV);
	 */
	io_32_write_fld_multi_all(DDRPHY_SHU_CA_CMD0,
		p_fld(SET_FLD, SHU_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
		p_fld(CLEAR_FLD, SHU_CA_CMD0_RG_TX_ARCLK_DRVN_PRE) |
		p_fld(SET_FLD, SHU_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU2_CA_CMD0,
		p_fld(SET_FLD, SHU2_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU2_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU2_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 * io_32_write_fld_multi(DDRPHY_SHU3_CA_CMD0,
		p_fld(SET_FLD, SHU3_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU3_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU3_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 * io_32_write_fld_multi(DDRPHY_SHU4_CA_CMD0,
		p_fld(SET_FLD, SHU4_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU4_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU4_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 */
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
		p_fld(0x7, SHU_CA_CMD0_RG_TX_ARCLK_DRVN_PRE) |
		p_fld(SET_FLD, SHU_CA_CMD0_RG_TX_ARCLK_PRE_EN));
#endif
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU2_CA_CMD0+SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU2_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU2_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU2_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 * io_32_write_fld_multi(DDRPHY_SHU3_CA_CMD0+SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU3_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU3_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU3_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 * io_32_write_fld_multi(DDRPHY_SHU4_CA_CMD0+SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU4_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU4_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU4_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 */
	io_32_write_fld_multi(DDRPHY_MISC_SHU_OPT,
		p_fld(SET_FLD, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB2_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB2_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB1_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB0_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN));
	io_32_write_fld_multi(DDRPHY_MISC_SHU_OPT + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB2_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB2_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB1_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB0_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN));

	delay_us(1);

	io_32_write_fld_align_all(DDRPHY_PLL3, CLEAR_FLD,
		PLL3_RG_RPHYPLL_TSTOP_EN);

	io_32_write_fld_multi_all(DDRPHY_SHU_PLL9,
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_MONCK_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_MONVC_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_LVROD_EN) |
		p_fld(SET_FLD, SHU_PLL9_RG_RPHYPLL_RST_DLY));
	io_32_write_fld_multi_all(DDRPHY_SHU_PLL11,
		p_fld(CLEAR_FLD, SHU_PLL11_RG_RCLRPLL_MONCK_EN) |
		p_fld(CLEAR_FLD, SHU_PLL11_RG_RCLRPLL_MONVC_EN) |
		p_fld(CLEAR_FLD, SHU_PLL11_RG_RCLRPLL_LVROD_EN) |
		p_fld(SET_FLD, SHU_PLL11_RG_RCLRPLL_RST_DLY));
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_PLL9 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_MONCK_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_MONVC_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_LVROD_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_RST_DLY));
#endif
#if 0
	io_32_write_fld_align(DDRPHY_SHU_PLL5, CLEAR_FLD,
		SHU_PLL5_RG_RPHYPLL_SDM_FRA_EN);
	io_32_write_fld_align(DDRPHY_SHU_PLL7, CLEAR_FLD,
		SHU_PLL7_RG_RCLRPLL_SDM_FRA_EN);
	io_32_write_fld_align(DDRPHY_SHU_PLL5 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		SHU_PLL5_RG_RPHYPLL_SDM_FRA_EN);
	io_32_write_fld_align(DDRPHY_SHU_PLL7 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		SHU_PLL7_RG_RCLRPLL_SDM_FRA_EN);

	io_32_write_fld_align(DDRPHY_SHU_PLL4 + SHIFT_TO_CHB_ADDR, 0xfe,
		SHU_PLL4_RG_RPHYPLL_RESERVED);
	io_32_write_fld_align(DDRPHY_SHU_PLL6 + SHIFT_TO_CHB_ADDR, 0xfe,
		SHU_PLL6_RG_RCLRPLL_RESERVED);
#endif

	delay_us(1);

	io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL1, SET_FLD,
		SHU_B0_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B0);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL1, SET_FLD,
		SHU_B1_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B1);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL1, SET_FLD,
		SHU_B2_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B2);
	io_32_write_fld_align(DDRPHY_SHU_CA_DLL1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, SHU_CA_DLL1_RG_ARDLL_FASTPJ_CK_SEL_CA);
#if 0
	io_32_write_fld_align(DDRPHY_SHU_B0_DLL1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, SHU_B0_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DLL1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, SHU_B1_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B1);
#endif
	delay_us(1);

	/*
	 * io_32_write_fld_align(DDRPHY_PLL2, CLEAR_FLD, PLL2_RG_RCLRPLL_EN);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL4, 0xff, SHU_PLL4_RG_RPHYPLL_RESERVED);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL6, 0xff, SHU_PLL6_RG_RCLRPLL_RESERVED);
	 */
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0 + SHIFT_TO_CHB_ADDR,
		0xf, MISC_CG_CTRL0_CLK_MEM_DFS_CFG);
	io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL6, 0x1, MISC_CG_CTRL6_FMEM_CK_CG_PINMUX);
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, 0xf,
		MISC_CG_CTRL0_CLK_MEM_DFS_CFG);

	delay_us(1);

	ddrphy_rsv_rg_setting(p);
	ddrphy_pll_setting(p);

	io_32_write_fld_align(DDRPHY_SHU_PLL4, 0x2000, SHU_PLL4_RG_RPHYPLL_RESERVED);
	io_32_write_fld_align(DDRPHY_SHU_PLL6, 0x2000, SHU_PLL6_RG_RCLRPLL_RESERVED);
	io_32_write_fld_align(DDRPHY_SHU_PLL5, SET_FLD,
		SHU_PLL5_RG_RPHYPLL_SDM_FRA_EN);
	io_32_write_fld_align(DDRPHY_SHU_PLL7, SET_FLD,
		SHU_PLL7_RG_RCLRPLL_SDM_FRA_EN);

#if ENABLE_TMRRI_NEW_MODE

	io_32_write_fld_align(DRAMC_REG_RKCFG, 0, RKCFG_TXRANK);
	io_32_write_fld_align(DRAMC_REG_RKCFG, 1, RKCFG_TXRANKFIX);
	io_32_write_fld_align(DRAMC_REG_DRSCTRL, CLEAR_FLD,
		DRSCTRL_RK_SCINPUT_OPT);
	io_32_write_fld_multi(DRAMC_REG_DRAMCTRL,
		p_fld(CLEAR_FLD, DRAMCTRL_MRRIOPT) |
		p_fld(CLEAR_FLD, DRAMCTRL_TMRRIBYRK_DIS) |
		p_fld(SET_FLD, DRAMCTRL_TMRRICHKDIS));

	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, SET_FLD,
		SPCMDCTRL_SC_PG_UPD_OPT);
	io_32_write_fld_multi(DRAMC_REG_SPCMDCTRL,
		p_fld(SET_FLD, SPCMDCTRL_SRFMR4_CNTKEEP_B) |
		p_fld(SET_FLD, SPCMDCTRL_SPDR_MR4_OPT) |
		p_fld(CLEAR_FLD, SPCMDCTRL_SC_PG_MPRW_DIS) |
		p_fld(CLEAR_FLD, SPCMDCTRL_SC_PG_STCMD_AREF_DIS) |
		p_fld(CLEAR_FLD, SPCMDCTRL_SC_PG_OPT2_DIS) |
		p_fld(CLEAR_FLD, SPCMDCTRL_SC_PG_MAN_DIS));

	io_32_write_fld_align(DRAMC_REG_RKCFG, 0, RKCFG_TXRANK);
	io_32_write_fld_align(DRAMC_REG_RKCFG, 0, RKCFG_TXRANKFIX);
#else
	io_32_write_fld_align(DRAMC_REG_DRSCTRL, SET_FLD,
		DRSCTRL_RK_SCINPUT_OPT);
	io_32_write_fld_multi(DRAMC_REG_DRAMCTRL,
		p_fld(SET_FLD, DRAMCTRL_MRRIOPT) |
		p_fld(SET_FLD, DRAMCTRL_TMRRIBYRK_DIS) |
		p_fld(CLEAR_FLD, DRAMCTRL_TMRRICHKDIS));
#if NON_EXIST_RG
	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, CLEAR_FLD,
		SPCMDCTRL_SC_PG_UPD_OPT);
	io_32_write_fld_multi(DRAMC_REG_SPCMDCTRL,
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_MPRW_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_STCMD_AREF_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_OPT2_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_MAN_DIS));
#endif
#endif

	io_32_write_fld_multi(DRAMC_REG_MPC_OPTION,
		p_fld(CLEAR_FLD, MPC_OPTION_ZQ_BLOCKALE_OPT) |
		p_fld(CLEAR_FLD, MPC_OPTION_MPC_BLOCKALE_OPT2) |
		p_fld(CLEAR_FLD, MPC_OPTION_MPC_BLOCKALE_OPT1) |
		p_fld(CLEAR_FLD, MPC_OPTION_MPC_BLOCKALE_OPT));

	io_32_write_fld_align(DRAMC_REG_CKECTRL, SET_FLD,
		CKECTRL_RUNTIMEMRRCKEFIX);
	io_32_write_fld_align(DRAMC_REG_CKECTRL, CLEAR_FLD,
		CKECTRL_RUNTIMEMRRMIODIS);

	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B2);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, SET_FLD,
		CA_CMD10_RG_RX_ARCLK_STBEN_RESETB);
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B2_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B2);
	io_32_write_fld_multi(DDRPHY_MISC_CTRL1,
		p_fld(SET_FLD, MISC_CTRL1_R_DMDA_RRESETB_E) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMDQSIENCG_EN) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMARPIDQ_SW) |
		p_fld(SET_FLD, MISC_CTRL1_R_DM_TX_ARCMD_OE) |
		p_fld(SET_FLD, MISC_CTRL1_R_DM_TX_ARCLK_OE));

	io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ7, SET_FLD,
		SHU_B0_DQ7_R_DMRDSEL_DIV2_OPT);
	io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ7, CLEAR_FLD,
		SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ7, CLEAR_FLD,
		SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ7, CLEAR_FLD,
		SHU_B2_DQ7_R_DMRXTRACK_DQM_EN_B2);
	io_32_write_fld_align_all(DDRPHY_B0_RXDVS0, 1,
		B0_RXDVS0_R_HWSAVE_MODE_ENA_B0);
	io_32_write_fld_align_all(DDRPHY_B1_RXDVS0, 1,
		B1_RXDVS0_R_HWSAVE_MODE_ENA_B1);
	io_32_write_fld_align_all(DDRPHY_B2_RXDVS0, 1,
		B2_RXDVS0_R_HWSAVE_MODE_ENA_B2);
	io_32_write_fld_align_all(DDRPHY_CA_RXDVS0, 1,
		CA_RXDVS0_R_HWSAVE_MODE_ENA_CA);
	io_32_write_fld_multi(DDRPHY_MISC_CTRL1 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, MISC_CTRL1_R_DMDA_RRESETB_E) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMDQSIENCG_EN) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMARPIDQ_SW) |
		p_fld(SET_FLD, MISC_CTRL1_R_DM_TX_ARCMD_OE) |
		p_fld(SET_FLD, MISC_CTRL1_R_DM_TX_ARCLK_OE));
#if 0
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1);
	io_32_write_fld_align(DDRPHY_B0_RXDVS0 + SHIFT_TO_CHB_ADDR, 1,
		B0_RXDVS0_R_HWSAVE_MODE_ENA_B0);
	io_32_write_fld_align(DDRPHY_B1_RXDVS0 + SHIFT_TO_CHB_ADDR, 1,
		B1_RXDVS0_R_HWSAVE_MODE_ENA_B1);
	io_32_write_fld_align(DDRPHY_CA_RXDVS0 + SHIFT_TO_CHB_ADDR, 1,
		CA_RXDVS0_R_HWSAVE_MODE_ENA_CA);
#endif
	io_32_write_fld_multi_all(DDRPHY_CA_CMD8,
		p_fld(CLEAR_FLD, CA_CMD8_RG_TX_RRESETB_PULL_DN) |
		p_fld(CLEAR_FLD, CA_CMD8_RG_TX_RRESETB_DDR3_SEL));

	io_32_write_fld_align(DDRPHY_CA_CMD7, CLEAR_FLD,
		CA_CMD7_RG_TX_ARCMD_PULL_DN);
	io_32_write_fld_align(DDRPHY_B0_DQ7, CLEAR_FLD,
		B0_DQ7_RG_TX_ARDQ_PULL_DN_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ7, CLEAR_FLD,
		B1_DQ7_RG_TX_ARDQ_PULL_DN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ7, CLEAR_FLD,
		B2_DQ7_RG_TX_ARDQ_PULL_DN_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD7 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		CA_CMD7_RG_TX_ARCMD_PULL_DN);
	io_32_write_fld_align(DDRPHY_B0_DQ7 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		B0_DQ7_RG_TX_ARDQ_PULL_DN_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ7 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		B1_DQ7_RG_TX_ARDQ_PULL_DN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ7 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		B2_DQ7_RG_TX_ARDQ_PULL_DN_B2);
	io_32_write_fld_multi(DRAMC_REG_SHU_DQSG_RETRY,
		p_fld(SET_FLD, SHU_DQSG_RETRY_R_RDY_SEL_DLE) |
		p_fld(0x6, SHU_DQSG_RETRY_R_DQSIENLAT) |
		p_fld(SET_FLD, SHU_DQSG_RETRY_R_RETRY_ONCE));
	io_32_write_fld_multi(DRAMC_REG_SHUCTRL2,
		p_fld(CLEAR_FLD, SHUCTRL2_HWSET_WLRL) |
		p_fld(SET_FLD, SHUCTRL2_SHU_PERIOD_GO_ZERO_CNT) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_OPTION) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_PARK_N) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_DLL_CHA) |
		p_fld(0xa, SHUCTRL2_R_DLL_IDLE));
	io_32_write_fld_multi(DRAMC_REG_SHUCTRL2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHUCTRL2_HWSET_WLRL) |
		p_fld(SET_FLD, SHUCTRL2_SHU_PERIOD_GO_ZERO_CNT) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_OPTION) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_PARK_N) |
		p_fld(0xa, SHUCTRL2_R_DLL_IDLE));
	io_32_write_fld_align(DRAMC_REG_DVFSDLL, SET_FLD,
		DVFSDLL_DLL_LOCK_SHU_EN);
	io_32_write_fld_multi(DRAMC_REG_DDRCONF0,
		p_fld(SET_FLD, DDRCONF0_LPDDR3EN) |
#if NON_EXIST_RG
		p_fld(SET_FLD, DDRCONF0_DM64BITEN) |
#endif
		p_fld(CLEAR_FLD, DDRCONF0_BC4OTF) |
		p_fld(SET_FLD, DDRCONF0_BK8EN));
	io_32_write_fld_align(DRAMC_REG_SHU_CONF0, SET_FLD,
		SHU_CONF0_DM64BITEN);
	io_32_write_fld_multi(DRAMC_REG_STBCAL2,
		p_fld(SET_FLD, STBCAL2_STB_GERR_B23) |
		p_fld(SET_FLD, STBCAL2_STB_GERR_B01) |
		p_fld(SET_FLD, STBCAL2_STB_GERRSTOP));
	io_32_write_fld_align(DRAMC_REG_STBCAL2, SET_FLD,
		STBCAL2_STB_GERR_RST);
	io_32_write_fld_align(DRAMC_REG_STBCAL2, CLEAR_FLD,
		STBCAL2_STB_GERR_RST);
	io_32_write_fld_align(DRAMC_REG_SHU_WODT, CLEAR_FLD,
		SHU_WODT_WPST1P5T);
	io_32_write_fld_align(DRAMC_REG_CLKAR, SET_FLD, CLKAR_PSELAR);
	io_32_write_fld_multi(DDRPHY_MISC_CTRL0,
		p_fld(CLEAR_FLD, MISC_CTRL0_R_DQS0IEN_DIV4_CK_CG_CTRL) |
		p_fld(SET_FLD, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
		p_fld(SET_FLD, MISC_CTRL0_R_DMSTBEN_OUTSEL) |
		p_fld(0xf, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));
	io_32_write_fld_multi(DDRPHY_MISC_CTRL0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, MISC_CTRL0_R_CLKIEN_DIV4_CK_CG_CTRL) |
		p_fld(SET_FLD, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
		p_fld(SET_FLD, MISC_CTRL0_R_DMSTBEN_OUTSEL) |
		p_fld(0xf, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));
	io_32_write_fld_multi(DRAMC_REG_SHU_CONF0,
		p_fld(0x2, SHU_CONF0_MATYPE) |
		p_fld(SET_FLD, SHU_CONF0_BL4) |
		p_fld(SET_FLD, SHU_CONF0_FDIV2) |
		p_fld(CLEAR_FLD, SHU_CONF0_DUALSCHEN) |
		p_fld(SET_FLD, SHU_CONF0_REFTHD) |
		p_fld(SET_FLD, SHU_CONF0_RDATDIV2) |
		p_fld(CLEAR_FLD, SHU_CONF0_RDATDIV4) |
		p_fld(SET_FLD, SHU_CONF0_ADVPREEN) |
		p_fld(0x3f, SHU_CONF0_DMPGTIM));
	io_32_write_fld_multi(DRAMC_REG_SHU_ODTCTRL,
		p_fld(0x3, SHU_ODTCTRL_TWODT) |
		p_fld(CLEAR_FLD, SHU_ODTCTRL_WOEN));
	io_32_write_fld_align(DRAMC_REG_REFCTRL0, 0x5,
		REFCTRL0_REF_PREGATE_CNT);
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA1,
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CS1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_RAS) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CAS) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_WE) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_RESET) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_ODT) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CKE) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CS));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA2,
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_CKE1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_CMD) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_BA2) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_BA1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_BA0));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA3,
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA7) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA6) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA5) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA4) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA3) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA2) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA0));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA4,
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA15) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA14) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA13) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA12) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA11) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA10) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA9) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA8));
	io_32_write_fld_align(DRAMC_REG_SHU_SELPH_CA5, CLEAR_FLD,
		SHU_SELPH_CA5_DLY_ODT);
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_DQS0,
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS3) |
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS2) |
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS1) |
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS0) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS3) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS2) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS1) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS0));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_DQS1,
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS3) |
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS2) |
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS1) |
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS0) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS3) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS2) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS1) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ0,
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ3) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ2) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ1) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ1,
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM3) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM2) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM1) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ2,
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ2_DLY_OEN_DQ3) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ2_DLY_OEN_DQ2) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ2_DLY_OEN_DQ1) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ2_DLY_OEN_DQ0) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ2_DLY_DQ3) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ2_DLY_DQ2) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ2_DLY_DQ1) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ2_DLY_DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ3,
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ3_DLY_OEN_DQM3) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ3_DLY_OEN_DQM2) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ3_DLY_OEN_DQM1) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ3_DLY_OEN_DQM0) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ3_DLY_DQM3) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ3_DLY_DQM2) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ3_DLY_DQM1) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ3_DLY_DQM0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ0,
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3) |
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2) |
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1) |
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ1,
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3) |
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2) |
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1) |
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ2,
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3) |
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2) |
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1) |
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ2_DLY_R1DQ3) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ2_DLY_R1DQ2) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ2_DLY_R1DQ1) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ2_DLY_R1DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ3,
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3) |
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2) |
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1) |
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ3_DLY_R1DQM3) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ3_DLY_R1DQM2) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ3_DLY_R1DQM1) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ3_DLY_R1DQM0));
	delay_us(1);

	io_32_write_fld_multi(DRAMC_REG_STBCAL,
		p_fld(CLEAR_FLD, STBCAL_DQSIENMODE_SELPH) |
		p_fld(SET_FLD, STBCAL_STB_DQIEN_IG) |
		p_fld(SET_FLD, STBCAL_PICHGBLOCK_NORD) |
		p_fld(SET_FLD, STBCAL_PIMASK_RKCHG_OPT));
	io_32_write_fld_multi(DRAMC_REG_SHU_DQSG,
		p_fld(0x9, SHU_DQSG_STB_UPDMASKCYC) |
		p_fld(SET_FLD, SHU_DQSG_STB_UPDMASK_EN));
	io_32_write_fld_align(DRAMC_REG_STBCAL, SET_FLD, STBCAL_DQSIENMODE);
	io_32_write_fld_multi(DRAMC_REG_SREFCTRL,
		p_fld(CLEAR_FLD, SREFCTRL_SREF_HW_EN) |
		p_fld(0x8, SREFCTRL_SREFDLY) |
		p_fld(CLEAR_FLD, SREFCTRL_SREF2_OPTION));
	io_32_write_fld_multi(DRAMC_REG_SHU_PIPE,
		p_fld(SET_FLD, SHU_PIPE_READ_START_EXTEND1) |
		p_fld(SET_FLD, SHU_PIPE_DLE_LAST_EXTEND1));

	io_32_write_fld_multi(DRAMC_REG_CKECTRL,
		p_fld(SET_FLD, CKECTRL_CKEON));
#if NON_EXIST_RG
	io_32_write_fld_multi(DRAMC_REG_CKECTRL,
		p_fld(SET_FLD, CKECTRL_CKEON) |
		p_fld(SET_FLD, CKECTRL_CKETIMER_SEL));
	io_32_write_fld_align(DRAMC_REG_RKCFG, SET_FLD,
		RKCFG_CKE2RANK_OPT2);
#endif
	io_32_write_fld_align(DRAMC_REG_SHU_CONF2, 0x7,
		SHU_CONF2_DCMDLYREF);

	io_32_write_fld_multi(DRAMC_REG_SHU_SCINTV,
		p_fld(CLEAR_FLD, SHU_SCINTV_DQS2DQ_SHU_PITHRD) |

		p_fld(CLEAR_FLD, SHU_SCINTV_RDDQC_INTV) |
		p_fld(CLEAR_FLD, SHU_SCINTV_TZQLAT));
#if 0
	io_32_write_fld_multi(DRAMC_REG_SHU_CONF1,
		p_fld(0xc, SHU_CONF1_DATLAT_DSEL_PHY) |
		p_fld(0xc, SHU_CONF1_DATLAT_DSEL) |
		p_fld(0xe, SHU_CONF1_DATLAT));
#endif
	io_32_write_fld_align(DRAMC_REG_SHUCTRL, SET_FLD,
		SHUCTRL_LPSM_BYPASS_B);
	io_32_write_fld_align(DRAMC_REG_REFCTRL1, CLEAR_FLD,
		REFCTRL1_SREF_PRD_OPT);
	/*
	 * io_32_write_fld_align(DDRPHY_SHU_PLL4, 0xfe, SHU_PLL4_RG_RPHYPLL_RESERVED);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL6, 0xfe, SHU_PLL6_RG_RCLRPLL_RESERVED);
	 */
	io_32_write_fld_align(DRAMC_REG_REFRATRE_FILTER, SET_FLD,
		REFRATRE_FILTER_PB2AB_OPT);
	io_32_write_fld_align_all(DDRPHY_MISC_CTRL1, SET_FLD,
		MISC_CTRL1_R_DMDA_RRESETB_I);
	io_32_write_fld_align_all(DDRPHY_MISC_CTRL1,
		SET_FLD, MISC_CTRL1_R_DMRRESETB_I_OPT);
	io_32_write_fld_align(DRAMC_REG_DRAMCTRL, CLEAR_FLD,
		DRAMCTRL_CLKWITRFC);
	io_32_write_fld_multi(DRAMC_REG_MISCTL0,
		p_fld(SET_FLD, MISCTL0_REFP_ARB_EN2) |
		p_fld(SET_FLD, MISCTL0_PBC_ARB_EN) |
		p_fld(SET_FLD, MISCTL0_REFA_ARB_EN2));
	io_32_write_fld_multi(DRAMC_REG_MISCTL0,
		p_fld(SET_FLD, MISCTL0_DM32BIT_RDSEL_OPT) |
		p_fld(SET_FLD, MISCTL0_PG_WAKEUP_OPT) |
		p_fld(SET_FLD, MISCTL0_PBC_ARB_EN));

	io_32_write_fld_multi(DRAMC_REG_PERFCTL0,
		p_fld(SET_FLD, PERFCTL0_MWHPRIEN) |
		p_fld(SET_FLD, PERFCTL0_WFLUSHEN) |
		p_fld(SET_FLD, PERFCTL0_EMILLATEN) |
		p_fld(SET_FLD, PERFCTL0_RWAGEEN) |
		p_fld(SET_FLD, PERFCTL0_RWLLATEN) |
		p_fld(SET_FLD, PERFCTL0_RWHPRIEN) |
		p_fld(SET_FLD, PERFCTL0_RWOFOEN) |
		p_fld(SET_FLD, PERFCTL0_DISRDPHASE1));
#if NON_EXIST_RG
		p_fld(SET_FLD, PERFCTL0_DUALSCHEN));
#endif
	io_32_write_fld_align(DRAMC_REG_ARBCTL, 0x80, ARBCTL_MAXPENDCNT);
	io_32_write_fld_multi(DRAMC_REG_PADCTRL,
		p_fld(SET_FLD, PADCTRL_DQIENLATEBEGIN) |
		p_fld(SET_FLD, PADCTRL_DQIENQKEND));
	io_32_write_fld_align(DRAMC_REG_DRAMC_PD_CTRL, SET_FLD,
		DRAMC_PD_CTRL_DCMREF_OPT);
	io_32_write_fld_align(DRAMC_REG_CLKCTRL, SET_FLD, CLKCTRL_CLK_EN_1);
	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(0x4, REFCTRL0_DISBYREFNUM) |
		p_fld(SET_FLD, REFCTRL0_DLLFRZ));
	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, SET_FLD,
		SPCMDCTRL_CLR_EN);
	io_32_write_fld_align(DRAMC_REG_SHUCTRL1, CLEAR_FLD,
			SHUCTRL1_FC_PRDCNT);
#if 0
	io_32_write_fld_multi(DRAMC_REG_CATRAINING1,
		p_fld(0x4, CATRAINING1_CATRAIN_INTV) |
		p_fld(0x3, CATRAINING1_CATRAINLAT));
#endif
	io_32_write_fld_align(DRAMC_REG_STBCAL, SET_FLD, STBCAL_REFUICHG);
	io_32_write_fld_multi(DRAMC_REG_SHU_RANKCTL,
		p_fld(0x5, SHU_RANKCTL_RANKINCTL_PHY) |
		p_fld(0x2, SHU_RANKCTL_RANKINCTL_ROOT1) |
		p_fld(0x2, SHU_RANKCTL_RANKINCTL));
	io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, SET_FLD,
		SHU_STBCAL_DMSTBLAT);
	/*
	 * io_32_write_fld_align(DRAMC_REG_SHURK0_DQSCTL, 0x4, SHURK0_DQSCTL_DQSINCTL); //DQSINCTL: set in UpdateACTimingReg()
	 * io_32_write_fld_align(DRAMC_REG_SHURK1_DQSCTL, 0x4, SHURK1_DQSCTL_R1DQSINCTL); //DQSINCTL: set in UpdateACTimingReg()
	 */

	delay_us(2);

#if 0//cc mark LEGACY_GATING_DLY
	LegacyGatingDlyLP3(p);
#endif

	io_32_write_fld_multi(DRAMC_REG_SHU_WODT,
		p_fld(SET_FLD, SHU_WODT_DISWODTE) |
		p_fld(SET_FLD, SHU_WODT_WODTFIXOFF) |
		p_fld(0x4, SHU_WODT_DISWODT));

	io_32_write_fld_multi(DRAMC_REG_PHY_RX_INCTL,
		p_fld(SET_FLD, PHY_RX_INCTL_RX_IN_GATE_EN_4BYTE_EN) |
		p_fld(SET_FLD, PHY_RX_INCTL_RX_IN_BUFF_EN_4BYTE_EN));
	/*
	 * io_32_write_fld_align(DRAMC_REG_SHU_CONF3, 0x5, SHU_CONF3_ZQCSCNT);
	 * io_32_write_fld_align(DRAMC_REG_SHU_CONF3, 0xff, SHU_CONF3_REFRCNT);
	 * io_32_write_fld_align(DRAMC_REG_SHU_CONF1, 0xb0, SHU_CONF1_REFBW_FR);
	 */

	io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, SET_FLD,
		SHU_STBCAL_DQSG_MODE);
	io_32_write_fld_multi(DRAMC_REG_SHU_CONF0,
		p_fld(0x5, SHU_CONF0_REFTHD) |
		p_fld(0x15, SHU_CONF0_DMPGTIM));
	/*
	 * io_32_write_fld_align(DRAMC_REG_SHU_CONF1, 0xae, SHU_CONF1_REFBW_FR);
	 * io_32_write_fld_multi(DRAMC_REG_SHU_CONF3,
		p_fld(0x8d, SHU_CONF3_REFRCNT) |
	 * | p_fld(0x1f8, SHU_CONF3_ZQCSCNT));
	 */
	io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, SET_FLD,
		SHU_STBCAL_PICGLAT);

	{

		io_32_write_fld_multi(DRAMC_REG_SHU_CONF0,
			p_fld(SET_FLD, SHU_CONF0_REFTHD) |
			p_fld(0x3f, SHU_CONF0_DMPGTIM));
		io_32_write_fld_multi(DRAMC_REG_SHU_CONF1,
			p_fld(SET_FLD, SHU_CONF1_TREFBWIG) |
			p_fld(CLEAR_FLD, SHU_CONF1_REFBW_FR));
		io_32_write_fld_multi(DRAMC_REG_SHU_CONF3,
			p_fld(0xff, SHU_CONF3_REFRCNT) |
			p_fld(0x5, SHU_CONF3_ZQCSCNT));
		io_32_write_fld_align(DRAMC_REG_SHU_RANKCTL, 0x3,
			SHU_RANKCTL_RANKINCTL_PHY);

#if NON_EXIST_RG
		io_32_write_fld_align(DRAMC_REG_SHU_WODT, CLEAR_FLD,
			SHU_WODT_TWPSTEXT);
#endif
		io_32_write_fld_align(DRAMC_REG_SHU_DQSG, 0xf, SHU_DQSG_SCINTV);
		/*
		 * io_32_write_fld_multi(DRAMC_REG_SHU_HWSET_MR2,
			p_fld(CLEAR_FLD, SHU_HWSET_MR2_HWSET_MR2_OP) |
		 * | p_fld(CLEAR_FLD, SHU_HWSET_MR2_HWSET_MR2_MRSMA));
		 * io_32_write_fld_multi(DRAMC_REG_SHU_HWSET_MR13,
			p_fld(0x1c, SHU_HWSET_MR13_HWSET_MR13_OP) |
		 * | p_fld(0x2, SHU_HWSET_MR13_HWSET_MR13_MRSMA));
		 * io_32_write_fld_multi(DRAMC_REG_SHU_HWSET_VRCG,
			p_fld(CLEAR_FLD, SHU_HWSET_VRCG_HWSET_VRCG_OP) |
		 * | p_fld(CLEAR_FLD, SHU_HWSET_VRCG_HWSET_VRCG_MRSMA));
		 */
		io_32_write_fld_multi(DRAMC_REG_SHU_DQSG_RETRY,
			p_fld(SET_FLD, SHU_DQSG_RETRY_R_RETRY_USE_BURST_MDOE) |
			p_fld(SET_FLD, SHU_DQSG_RETRY_R_RETRY_ROUND_NUM) |
	#ifdef FIRST_BRING_UP
			p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_XSR_RETRY_SPM_MODE) |
	#else
			p_fld(SET_FLD, SHU_DQSG_RETRY_R_XSR_RETRY_SPM_MODE) |
	#endif
			p_fld(0x3, SHU_DQSG_RETRY_R_DQSIENLAT) |
			p_fld(SET_FLD, SHU_DQSG_RETRY_R_DM4BYTE));

		io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ5, CLEAR_FLD,
			SHU_B0_DQ5_RG_ARPI_FB_B0);
		io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ5, CLEAR_FLD,
			SHU_B1_DQ5_RG_ARPI_FB_B1);
		io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ5, CLEAR_FLD,
			SHU_B2_DQ5_RG_ARPI_FB_B2);
		io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD5, CLEAR_FLD,
			SHU_CA_CMD5_RG_ARPI_FB_CA);
		/*
		 * 93918 ===ddrphy_sh_lp3_1866_cha end===
		 * 93918 ===ddrphy_sh_lp3_1866_chb begin===
		 */
	#if 0
		io_32_write_fld_align(DDRPHY_SHU_B0_DQ5 + SHIFT_TO_CHB_ADDR, 0x4,
			SHU_B0_DQ5_RG_ARPI_FB_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ5 + SHIFT_TO_CHB_ADDR, 0x4,
			SHU_B1_DQ5_RG_ARPI_FB_B1);
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD5 + SHIFT_TO_CHB_ADDR, 0x4,
			SHU_CA_CMD5_RG_ARPI_FB_CA);
	#endif

		io_32_write_fld_multi(DDRPHY_SHU_R0_B0_DQ2 + SHIFT_TO_CHB_ADDR,
			p_fld(0xa, SHU_R0_B0_DQ2_RK0_RX_ARDQ1_F_DLY_B0) |
			p_fld(0xa, SHU_R0_B0_DQ2_RK0_RX_ARDQ0_F_DLY_B0));

	}
	/*
	 * else
	 * TODO: overwrite 1866 setting END
	 */

	/*
	 * Since the above initial settings are for LP3_DDR1866, TX related initial delay settings needs to be adjusted
	 * according it's Write Latency difference
	 */
	if (p->freqGroup != DDR1866_FREQ) {
		//cc mark TxCalibrationMoveDQSByDifferentWL(p);
	}

	io_32_write_fld_align(DRAMC_REG_RKCFG, SET_FLD, RKCFG_RKMODE);

	update_initial_setting_lp3(p);

#if SIMULATION_SW_IMPED
	dramc_sw_impedance_save_register(p, ODT_OFF, ODT_OFF,
		DRAM_DFS_SHUFFLE_1);
#endif

#if LP3_CATRAING_SHIFT_CLK_PI
	dramc_mr_init_lp3(p, TRUE);
#else
	dramc_mr_init_lp3(p);
#endif

#if 0
	io_32_write_fld_multi(DRAMC_REG_MRS,
		p_fld(CLEAR_FLD, MRS_MRSRK) |
		p_fld(0x4, MRS_MRSMA) |
		p_fld(CLEAR_FLD, MRS_MRSOP));

	delay_us(1);

	io_32_write_fld_align(DRAMC_REG_SPCMD, SET_FLD, SPCMD_MRREN);

	delay_us(1);
#endif

	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(CLEAR_FLD, REFCTRL0_PBREFEN) |
		p_fld(SET_FLD, REFCTRL0_PBREF_DISBYRATE));
	io_32_write_fld_align(DRAMC_REG_RKCFG, CLEAR_FLD, RKCFG_DQSOSC2RK);
	/*
	 * io_32_write_fld_align(DRAMC_REG_CKECTRL, CLEAR_FLD, CKECTRL_CKEFIXON);
	 * io_32_write_fld_align(DRAMC_REG_HW_MRR_FUN, SET_FLD, HW_MRR_FUN_TMRR_ENA);
	 */
	io_32_write_fld_align(DRAMC_REG_DRAMCTRL, SET_FLD,
		DRAMCTRL_PREALL_OPTION);
	io_32_write_fld_align(DRAMC_REG_ZQCS, 0x56, ZQCS_ZQCSOP);

	delay_us(1);

	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(SET_FLD, REFCTRL0_REFFRERUN) |
		p_fld(SET_FLD, REFCTRL0_REFDIS));
	io_32_write_fld_align(DRAMC_REG_SREFCTRL, SET_FLD,
		SREFCTRL_SREF_HW_EN);
	io_32_write_fld_align(DRAMC_REG_MPC_OPTION, CLEAR_FLD,
		MPC_OPTION_MPCRKEN);
	io_32_write_fld_multi(DRAMC_REG_DRAMC_PD_CTRL,
		p_fld(SET_FLD, DRAMC_PD_CTRL_PHYCLKDYNGEN) |
		p_fld(CLEAR_FLD, DRAMC_PD_CTRL_DCMENNOTRFC) |
		p_fld(SET_FLD, DRAMC_PD_CTRL_DCMEN));
	io_32_write_fld_multi(DRAMC_REG_DRAMC_PD_CTRL + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, DRAMC_PD_CTRL_COMBCLKCTRL) |
		p_fld(CLEAR_FLD, DRAMC_PD_CTRL_DCMENNOTRFC) |
		p_fld(SET_FLD, DRAMC_PD_CTRL_DCMEN));
	io_32_write_fld_multi(DRAMC_REG_EYESCAN,
		p_fld(CLEAR_FLD, EYESCAN_RX_DQ_EYE_SEL) |
		p_fld(CLEAR_FLD, EYESCAN_RG_RX_EYE_SCAN_EN));
	io_32_write_fld_multi(DRAMC_REG_STBCAL1,
		p_fld(SET_FLD, STBCAL1_STBCNT_LATCH_EN) |
		p_fld(SET_FLD, STBCAL1_STBENCMPEN));
	io_32_write_fld_align(DRAMC_REG_TEST2_1, 0x10000,
		TEST2_1_TEST2_BASE);
	io_32_write_fld_align(DRAMC_REG_TEST2_2, 0x400, TEST2_2_TEST2_OFF);
	io_32_write_fld_multi(DRAMC_REG_TEST2_3,
		p_fld(SET_FLD, TEST2_3_TEST2WREN2_HW_EN) |
#if NON_EXIST_RG
		p_fld(0x4, TEST2_3_DQSICALSTP) |
#endif
		p_fld(SET_FLD, TEST2_3_TESTAUDPAT) |
		p_fld(CLEAR_FLD, TEST2_3_TESTADR_SHIFT));
	io_32_write_fld_align(DRAMC_REG_SHUCTRL2, SET_FLD,
		SHUCTRL2_MR13_SHU_EN);
	io_32_write_fld_align(DRAMC_REG_DRAMCTRL, SET_FLD,
		DRAMCTRL_REQQUE_THD_EN);

	io_32_write_fld_multi(DRAMC_REG_SHU_CKECTRL,
		p_fld(0x3, SHU_CKECTRL_SREF_CK_DLY) |
		p_fld(0x3, SHU_CKECTRL_TCKESRX));

	io_32_write_fld_multi(DRAMC_REG_SHU_DRVING1,
		p_fld(SET_FLD, SHU_DRVING1_DIS_IMP_ODTN_TRACK) |
		p_fld(SET_FLD, SHU_DRVING1_DIS_IMPCAL_HW));

	io_32_write_fld_align(DRAMC_REG_DUMMY_RD, p->support_rank_num,
		DUMMY_RD_RANK_NUM);

	io_32_write_fld_align(DRAMC_REG_TEST2_4, 0x4,
		TEST2_4_TESTAGENTRKSEL);
	io_32_write_fld_multi(DRAMC_REG_REFCTRL1,
		p_fld(SET_FLD, REFCTRL1_REF_OVERHEAD_SLOW_REFPB_ENA) |
		p_fld(0x5, REFCTRL1_MPENDREF_CNT) |
		p_fld(SET_FLD, REFCTRL1_REF_QUE_AUTOSAVE_EN) |
		p_fld(SET_FLD, REFCTRL1_SLEFREF_AUTOSAVE_EN));
	io_32_write_fld_multi(DRAMC_REG_DQSOSCR,
		p_fld(CLEAR_FLD, DQSOSCR_SREF_TXPI_RELOAD_OPT) |
		p_fld(CLEAR_FLD, DQSOSCR_SREF_TXUI_RELOAD_OPT));
	io_32_write_fld_multi(DRAMC_REG_RSTMASK,
		p_fld(CLEAR_FLD, RSTMASK_GT_SYNC_MASK) |
		p_fld(CLEAR_FLD, RSTMASK_DVFS_SYNC_MASK) |
		p_fld(CLEAR_FLD, RSTMASK_GT_SYNC_MASK_FOR_PHY) |
		p_fld(CLEAR_FLD, RSTMASK_DVFS_SYNC_MASK_FOR_PHY));
	/*
	 * io_32_write_fld_align(DRAMC_REG_DRAMCTRL, CLEAR_FLD, DRAMCTRL_CTOREQ_HPRI_OPT);
	 * 91074 === DE initial sequence done ===
	 * #ifdef DVFS_Enable
	 */
	io_32_write_fld_align(DRAMC_REG_STBCAL1, 0x8,
		STBCAL1_STBCAL_FILTER);
	io_32_write_fld_multi(DRAMC_REG_STBCAL,
		p_fld(CLEAR_FLD, STBCAL_STBCALEN) |
		p_fld(CLEAR_FLD, STBCAL_STB_SELPHYCALEN) |
		p_fld(CLEAR_FLD, STBCAL_REFUICHG) |
		p_fld(SET_FLD, STBCAL_SREF_DQSGUPD));

	delay_us(1);
	io_32_write_fld_align(DRAMC_REG_DDRCONF0, SET_FLD, DDRCONF0_AG0MWR);
	/*
	 * io_32_write_fld_multi(DRAMC_REG_DRAMCTRL,
		p_fld(SET_FLD, DRAMCTRL_FW2R) |
	 * | p_fld(CLEAR_FLD, DRAMCTRL_DYNMWREN)
	 * | p_fld(SET_FLD, DRAMCTRL_CLKWITRFC)
	 * | p_fld(SET_FLD, DRAMCTRL_ADRBIT3DEC)
	 * | p_fld(SET_FLD, DRAMCTRL_CTOREQ_HPRI_OPT));
	 */
#if 0
	io_32_write_fld_align(DRAMC_REG_MISCTL0, CLEAR_FLD,
		MISCTL0_PBC_ARB_EN);
	io_32_write_fld_multi(DRAMC_REG_PERFCTL0,
		p_fld(SET_FLD, PERFCTL0_REORDEREN) |
		p_fld(SET_FLD, PERFCTL0_RWHPRICTL) |
		p_fld(CLEAR_FLD, PERFCTL0_EMILLATEN) |
		p_fld(CLEAR_FLD, PERFCTL0_RWAGEEN) |
		p_fld(CLEAR_FLD, PERFCTL0_RWHPRIEN));
#endif
	io_32_write_fld_multi(DRAMC_REG_RSTMASK,
		p_fld(CLEAR_FLD, RSTMASK_PHY_SYNC_MASK) |
		p_fld(CLEAR_FLD, RSTMASK_DAT_SYNC_MASK));

#if 0
	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(SET_FLD, REFCTRL0_REFNA_OPT) |
		p_fld(0x2, REFCTRL0_DISBYREFNUM) |
		p_fld(SET_FLD, REFCTRL0_UPDBYWR));
#endif
	io_32_write_fld_multi(DRAMC_REG_REFRATRE_FILTER,
		p_fld(0x6, REFRATRE_FILTER_REFRATE_FIL7) |
		p_fld(0x5, REFRATRE_FILTER_REFRATE_FIL6) |
		p_fld(0x4, REFRATRE_FILTER_REFRATE_FIL5) |
		p_fld(0x3, REFRATRE_FILTER_REFRATE_FIL4) |
		p_fld(0x3, REFRATRE_FILTER_REFRATE_FIL3) |
		p_fld(0x3, REFRATRE_FILTER_REFRATE_FIL2) |
		p_fld(0x2, REFRATRE_FILTER_REFRATE_FIL1) |
		p_fld(0x1, REFRATRE_FILTER_REFRATE_FIL0));
	io_32_write_fld_multi(DRAMC_REG_ZQCS,
		p_fld(CLEAR_FLD, ZQCS_ZQCSDUAL) |
		p_fld(CLEAR_FLD, ZQCS_ZQCSMASK));
	io_32_write_fld_align(DRAMC_REG_HW_MRR_FUN, CLEAR_FLD,
		HW_MRR_FUN_MRR_HW_HIPRI);
	io_32_write_fld_align(DRAMC_REG_DUMMY_RD, SET_FLD,
		DUMMY_RD_SREF_DMYRD_EN);
	io_32_write_fld_align(DRAMC_REG_STBCAL1, 0x2,
		STBCAL1_STBCAL_FILTER);
	io_32_write_fld_multi(DRAMC_REG_SHU_APHY_TX_PICG_CTRL,
		p_fld(SET_FLD, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_EN_OPT) |
		p_fld(0x6, SHU_APHY_TX_PICG_CTRL_DDRPHY_CLK_DYN_GATING_SEL) |
		p_fld(SET_FLD, SHU_APHY_TX_PICG_CTRL_APHYPI_CG_CK_OPT) |
		p_fld(0xd, SHU_APHY_TX_PICG_CTRL_APHYPI_CG_CK_SEL));

#if ENABLE_DVFS_CDC_SYNCHRONIZER_OPTION
	io_32_write_fld_align_all(DRAMC_REG_SHUCTRL2, 1,
		SHUCTRL2_R_DVFS_CDC_OPTION);
	io_32_write_fld_align_all(DRAMC_REG_DVFSDLL, 1,
		DVFSDLL_R_DVFS_SYNC_MODULE_RST_SEL);
	io_32_write_fld_align_all(DRAMC_REG_SHUCTRL2, 1,
		SHUCTRL2_R_CDC_MUX_SEL_OPTION);
#endif

	io_32_write_fld_align(DRAMC_REG_DVFSDLL2, 0x3,
		DVFSDLL2_R_DVFS_PICG_MARGIN4_NEW);

	dvfs_settings(p);
}
#endif


/* LP4 & LP3 is porting from Merlxxx */
DRAM_STATUS_T dramc_setting_lp3_4(DRAMC_CTX_T *p)
{
	unsigned int save_ch, dram_t, highest_freq = 0;
	unsigned char dram_cbt_mode;

	show_msg2((INFO, "\n[DramcInit]\n"));

	set_phy_2_channel_mapping(p, CHANNEL_A);

	if (is_lp4_family(p)) {
#if SUPPORT_TYPE_LPDDR4
		highest_freq = LP4_HIGHEST_FREQ;
		dramc_setting_lp4(p);
#endif
	} else if (p->dram_type == TYPE_LPDDR3) {
#if SUPPORT_TYPE_LPDDR3
		highest_freq = LP3_HIGHEST_FREQ;
		dramc_setting_lp3(p);
#endif
	}

	/*
	 * For kernel api for check LPDDR3/4/4X (Darren), only for fcOlymxxx and fcElbxxx.
	 * For Other chip, please confirm the register is free for SW use.
	 */
	save_ch = get_phy_2_channel_mapping(p);
	set_phy_2_channel_mapping(p, CHANNEL_A);

#if 0 /* cc mark after DE review */
	switch (p->dram_type)
	{
		case TYPE_LPDDR3:
			dram_t = 1;
			break;
		case TYPE_LPDDR4:
			dram_t = 2;
			break;
		case TYPE_LPDDR4X:
			dram_t = 3;
			break;
		case TYPE_LPDDR4P:
			dram_t = 4;
			break;
		default:
			dram_t = 0;
			show_err("Incorrect DRAM Type!\n");
			break;
	}

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_ARBCTL), dram_t,
		ARBCTL_RSV_DRAM_TYPE);
#endif

	if ((p->dram_cbt_mode[RANK_0] == CBT_NORMAL_MODE) &&
		(p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE))
		dram_cbt_mode = CBT_R0_R1_NORMAL;
	else if ((p->dram_cbt_mode[RANK_0] == CBT_BYTE_MODE1) &&
		(p->dram_cbt_mode[RANK_1] == CBT_BYTE_MODE1))
		dram_cbt_mode = CBT_R0_R1_BYTE;
	else if ((p->dram_cbt_mode[RANK_0] == CBT_NORMAL_MODE) &&
		(p->dram_cbt_mode[RANK_1] == CBT_BYTE_MODE1))
		dram_cbt_mode = CBT_R0_NORMAL_R1_BYTE;
	else if ((p->dram_cbt_mode[RANK_0] == CBT_BYTE_MODE1) &&
		(p->dram_cbt_mode[RANK_1] == CBT_NORMAL_MODE))
		dram_cbt_mode = CBT_R0_BYTE_R1_NORMAL;
	else
		dram_cbt_mode = CBT_R0_R1_NORMAL;

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RSTMASK),
		dram_cbt_mode, RSTMASK_RSV_DRAM_CBT_MIXED);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_ARBCTL),
		(p->dram_cbt_mode[RANK_0] | p->dram_cbt_mode[RANK_1]),
		ARBCTL_RSV_DRAM_CBT);

	set_phy_2_channel_mapping(p, save_ch);

	show_msg2((INFO, "[DramcInit] Done\n"));

	return DRAM_OK;
}

#endif /* SUPPORT_TYPE_LPDDR4 */

#if SUPPORT_TYPE_PCDDR3
static DRAM_STATUS_T update_initial_setting_ddr3(DRAMC_CTX_T *p)
{
	io_32_write_fld_multi_all(DDRPHY_R0_B2_RXDVS2,
		p_fld(CLEAR_FLD, R0_B2_RXDVS2_R_RK0_DVS_MODE_B2) |
		p_fld(CLEAR_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(CLEAR_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B2));
	io_32_write_fld_multi_all(DDRPHY_R1_B2_RXDVS2,
		p_fld(CLEAR_FLD, R1_B2_RXDVS2_R_RK1_DVS_MODE_B2) |
		p_fld(CLEAR_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(CLEAR_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B2));
	io_32_write_fld_multi_all(DDRPHY_R0_B1_RXDVS2,
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi_all(DDRPHY_R1_B1_RXDVS2,
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1) |
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R0_B0_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, R0_B0_RXDVS2_R_RK0_DVS_MODE_B0) |
		p_fld(CLEAR_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(CLEAR_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi(DDRPHY_R1_B0_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, R1_B0_RXDVS2_R_RK1_DVS_MODE_B0) |
		p_fld(CLEAR_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(CLEAR_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi(DDRPHY_R0_CA_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, R0_CA_RXDVS2_R_RK0_DVS_MODE_CA) |
		p_fld(CLEAR_FLD, R0_CA_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_CA) |
		p_fld(CLEAR_FLD, R0_CA_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_CA));
	io_32_write_fld_multi(DDRPHY_R1_CA_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, R1_CA_RXDVS2_R_RK1_DVS_MODE_CA) |
		p_fld(CLEAR_FLD, R1_CA_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_CA) |
		p_fld(CLEAR_FLD, R1_CA_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_CA));
#if 0
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1) |
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
#endif

	io_32_write_fld_multi_all(DDRPHY_B0_DQ6,
		p_fld(SET_FLD, B0_DQ6_RG_TX_ARDQ_DDR3_SEL_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_TX_ARDQ_DDR4_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_DDR4_SEL_B0));
	io_32_write_fld_multi_all(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_TX_ARDQ_DDR3_SEL_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_DDR3_SEL_B1) |
		p_fld(CLEAR_FLD, B1_DQ6_RG_TX_ARDQ_DDR4_SEL_B1) |
		p_fld(CLEAR_FLD, B1_DQ6_RG_RX_ARDQ_DDR4_SEL_B1));
	io_32_write_fld_multi_all(DDRPHY_B2_DQ6,
		p_fld(SET_FLD, B2_DQ6_RG_TX_ARDQ_DDR3_SEL_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_DDR3_SEL_B2) |
		p_fld(CLEAR_FLD, B2_DQ6_RG_TX_ARDQ_DDR4_SEL_B2) |
		p_fld(CLEAR_FLD, B2_DQ6_RG_RX_ARDQ_DDR4_SEL_B2));
	io_32_write_fld_multi_all(DDRPHY_CA_CMD6,
		p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_DDR3_SEL) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_DDR3_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_TX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_DDR4_SEL));

	io_32_write_fld_align_all(DDRPHY_B0_DQ8, CLEAR_FLD,
		B0_DQ8_RG_TX_ARDQ_EN_LP4P_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ8, CLEAR_FLD,
		B1_DQ8_RG_TX_ARDQ_EN_LP4P_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ8, CLEAR_FLD,
		B2_DQ8_RG_TX_ARDQ_EN_LP4P_B2);
	io_32_write_fld_align_all(DDRPHY_CA_CMD9, CLEAR_FLD,
		CA_CMD9_RG_TX_ARCMD_EN_LP4P);

	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), CLEAR_FLD,
		B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ5), CLEAR_FLD,
		B1_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B1);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ5), CLEAR_FLD,
		B2_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B2);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD5), CLEAR_FLD,
		CA_CMD5_RG_RX_ARCMD_EYE_VREF_EN);

	io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD0, CLEAR_FLD,
		SHU_CA_CMD0_RG_FB_CK_MUX);
	io_32_write_fld_multi_all(DDRPHY_CA_CMD8,
		p_fld(SET_FLD, CA_CMD8_RG_TX_RRESETB_DDR3_SEL) |
		p_fld(CLEAR_FLD, CA_CMD8_RG_TX_RRESETB_DDR4_SEL));

	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), 0xb,
		B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ5), 0xb,
		B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ5), 0xb,
		B2_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B2);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD5), 0xb,
		CA_CMD5_RG_RX_ARCMD_EYE_VREF_SEL);

	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ5), 0xb,
		SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_SHU_B1_DQ5), 0xb,
		SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ5), 0xb,
		SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2);
	io_32_write_fld_align_phy_all(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD5),
		0xb, SHU_CA_CMD5_RG_RX_ARCMD_VREF_SEL);

	if (p->pinmux_type != PINMUX_TYPE_PC3_X8X4) {
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B0_DQ10),
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ0_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ1_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ2_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ3_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQM_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQM_MCK4X_SEL_B0));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B1_DQ10),
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ4_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ5_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ6_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ7_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ0_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ1_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ2_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ3_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ4_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ5_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ6_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ7_DQS_SEL_B1));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B2_DQ10),
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ0_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ1_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ2_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ3_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQM_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQM_MCK4X_SEL_B2));

		io_32_write_fld_multi((DDRPHY_B0_DQ10 + SHIFT_TO_CHB_ADDR),
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ4_MCK4X_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ0_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ1_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ2_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ4_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQM_DQS_SEL_B0) |
			p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQM_MCK4X_SEL_B0));
		io_32_write_fld_multi((DDRPHY_B1_DQ10 + SHIFT_TO_CHB_ADDR),
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ4_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ5_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ6_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ7_MCK4X_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ0_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ1_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ2_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ3_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ4_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ5_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ6_DQS_SEL_B1) |
			p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ7_DQS_SEL_B1));
		io_32_write_fld_multi((DDRPHY_B2_DQ10 + SHIFT_TO_CHB_ADDR),
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ0_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ1_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ2_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ3_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQM_DQS_SEL_B2) |
			p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQM_MCK4X_SEL_B2));
	}

	io_32_write_fld_align(DDRPHY_B0_DQ6, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B0);
	io_32_write_fld_align(DDRPHY_B0_DQ6 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ6, SET_FLD,
		B1_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ6, SET_FLD,
		B2_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B2);

	io_32_write_fld_align(DDRPHY_CA_CMD6, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN);
	io_32_write_fld_align(DDRPHY_CA_CMD6 + SHIFT_TO_CHB_ADDR, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN);

	io_32_write_fld_align_phy_all(DDRPHY_B0_DQ6, CLEAR_FLD,
		B0_DQ6_RG_RX_ARDQ_O1_SEL_B0);
	io_32_write_fld_align_phy_all(DDRPHY_B1_DQ6, CLEAR_FLD,
		B1_DQ6_RG_RX_ARDQ_O1_SEL_B1);
	io_32_write_fld_align_phy_all(DDRPHY_B2_DQ6, CLEAR_FLD,
		B2_DQ6_RG_RX_ARDQ_O1_SEL_B2);
	io_32_write_fld_align_phy_all(DDRPHY_CA_CMD6, CLEAR_FLD,
		CA_CMD6_RG_RX_ARCMD_O1_SEL);

	io_32_write_fld_align_phy_all(DDRPHY_B0_DQ6, CLEAR_FLD,
		B0_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B0);
	io_32_write_fld_align_phy_all(DDRPHY_B1_DQ6, CLEAR_FLD,
		B1_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B1);
	io_32_write_fld_align_phy_all(DDRPHY_B2_DQ6, CLEAR_FLD,
		B2_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B2);
	io_32_write_fld_align_phy_all(DDRPHY_CA_CMD6, CLEAR_FLD,
		CA_CMD6_RG_TX_ARCMD_ODTEN_EXT_DIS);

	io_32_write_fld_multi(DDRPHY_B0_DQ3,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B0));
	io_32_write_fld_multi(DDRPHY_B0_DQ3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B0));
	io_32_write_fld_multi_all(DDRPHY_B1_DQ3,
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B1));
	io_32_write_fld_multi_all(DDRPHY_B2_DQ3,
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B2));

	io_32_write_fld_multi(DDRPHY_CA_CMD3,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_IN_BUFF_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCLK_IN_BUFF_EN));
	io_32_write_fld_multi(DDRPHY_CA_CMD3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_IN_BUFF_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCLK_IN_BUFF_EN));

	io_32_write_fld_align_all(DDRPHY_B0_DQ6, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ6, SET_FLD,
		B1_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ6, SET_FLD,
		B2_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B2);

	io_32_write_fld_align_all(DDRPHY_CA_CMD6, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_RPRE_TOG_EN);

#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	io_32_write_fld_align(DDRPHY_B0_DQ9, 0x4,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9, 0x4,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9, 0x4,
		B2_DQ9_R_IN_GATE_EN_LOW_OPT_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD10, 0x4,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, 0x4,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, 0x4,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9 + SHIFT_TO_CHB_ADDR, 0x4,
		B2_DQ9_R_IN_GATE_EN_LOW_OPT_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, 0x4,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
#else
	io_32_write_fld_align(DDRPHY_B0_DQ9, 0x7,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9, 0x7,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9, 0x7,
		B2_DQ9_R_IN_GATE_EN_LOW_OPT_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD10, 0x7,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B2_DQ9_R_IN_GATE_EN_LOW_OPT_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, 0x7,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
#endif

	io_32_write_fld_align_all(DRAMC_REG_SHU_MISC, 0x2,
		SHU_MISC_REQQUE_MAXCNT);

	io_32_write_fld_align_all(DRAMC_REG_SHU_DQSG, 0x2a,
		SHU_DQSG_SCINTV);

	io_32_write_fld_multi(DRAMC_REG_SREFCTRL,
		p_fld(CLEAR_FLD, SREFCTRL_SCSM_CGAR) |
		p_fld(CLEAR_FLD, SREFCTRL_SCARB_SM_CGAR) |
		p_fld(CLEAR_FLD, SREFCTRL_RDDQSOSC_CGAR) |
		p_fld(CLEAR_FLD, SREFCTRL_HMRRSEL_CGAR));

	io_32_write_fld_align(DRAMC_REG_PRE_TDQSCK1, CLEAR_FLD,
		PRE_TDQSCK1_TXUIPI_CAL_CGAR);

	io_32_write_fld_align(DRAMC_REG_SHU_MISC, 0xf, SHU_MISC_PREA_INTV);

	io_32_write_fld_multi_all(DDRPHY_SHU_B0_DQ8,
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMSTBEN_SYNC_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0) |
		p_fld(0x7fff, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0));
	io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ8,
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMSTBEN_SYNC_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1) |
		p_fld(0x7fff, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1));
	io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ8,
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRANK_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMSTBEN_SYNC_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRXDLY_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_RMRX_TOPHY_CG_IG_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ8_R_DMRXDVS_UPD_FORCE_EN_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B2) |
		p_fld(0x7fff, SHU_B2_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B2));

	io_32_write_fld_multi_all(DDRPHY_SHU_CA_CMD8,
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRANK_CHG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRANK_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_FLAG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_FLAG_SYNC_CG_IG_CA) |
		p_fld(CLEAR_FLD, SHU_CA_CMD8_R_DMSTBEN_SYNC_CG_IG_CA) |
		p_fld(CLEAR_FLD, SHU_CA_CMD8_R_DMRXDLY_CG_IG_CA) |
		p_fld(CLEAR_FLD, SHU_CA_CMD8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_CA) |
		p_fld(CLEAR_FLD, SHU_CA_CMD8_R_DMRXDVS_RDSEL_PIPE_CG_IG_CA) |
		p_fld(CLEAR_FLD, SHU_CA_CMD8_R_DMRXDVS_UPD_FORCE_EN_CA) |
		p_fld(0x0, SHU_CA_CMD8_R_DMRXDVS_UPD_FORCE_CYC_CA));
	io_32_write_fld_align_all(DDRPHY_MISC_CTRL3, CLEAR_FLD,
		MISC_CTRL3_R_DDRPHY_COMB_CG_IG);
	io_32_write_fld_multi_all(DDRPHY_MISC_CTRL3,
		p_fld(0x3, MISC_CTRL3_ARPI_CG_DQS_OPT) |
		p_fld(0x3, MISC_CTRL3_ARPI_CG_DQ_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_MPDIV_CG_CA_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_CG_CLK_OPT));

	io_32_write_fld_align_all(DDRPHY_MISC_CTRL0, SET_FLD,
		MISC_CTRL0_R_DMSHU_PHYDCM_FORCEOFF);

	io_32_write_fld_align_all(DDRPHY_MISC_RXDVS2, CLEAR_FLD,
		MISC_RXDVS2_R_DMRXDVS_SHUFFLE_CTRL_CG_IG);

	io_32_write_fld_multi_all(DDRPHY_SHU_B0_DQ7,
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0));
	io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ7,
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1));
	io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ7,
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXDVS_DQM_FLAGSEL_B2));

	io_32_write_fld_multi(DRAMC_REG_CLKAR,
		p_fld(CLEAR_FLD, CLKAR_DWCLKRUN) |
		p_fld(CLEAR_FLD, CLKAR_SELPH_CMD_CG_DIS) |
		p_fld(0x7FFF, CLKAR_REQQUE_PACG_DIS));

	io_32_write_fld_align(DRAMC_REG_SHU_DQSG_RETRY, CLEAR_FLD,
		SHU_DQSG_RETRY_R_RETRY_PA_DSIABLE);
	io_32_write_fld_align(DRAMC_REG_WRITE_LEV, CLEAR_FLD,
		WRITE_LEV_DDRPHY_COMB_CG_SEL);

	io_32_write_fld_multi(DRAMC_REG_DUMMY_RD,
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_PA_OPT) |
		p_fld(SET_FLD, DUMMY_RD_DMYRD_REORDER_DIS) |
		p_fld(CLEAR_FLD, DUMMY_RD_DMYRD_HPRI_DIS) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_CNT6) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_CNT5) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_CNT3) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_SW));

	io_32_write_fld_multi(DRAMC_REG_STBCAL2,
		p_fld(CLEAR_FLD, STBCAL2_STB_STBENRST_EARLY_1T_EN) |
		p_fld(CLEAR_FLD, STBCAL2_STB_UIDLYCG_IG) |
		p_fld(CLEAR_FLD, STBCAL2_STB_PIDLYCG_IG));
	io_32_write_fld_multi(DRAMC_REG_EYESCAN,
		p_fld(SET_FLD, EYESCAN_EYESCAN_DQS_SYNC_EN) |
		p_fld(SET_FLD, EYESCAN_EYESCAN_NEW_DQ_SYNC_EN) |
		p_fld(SET_FLD, EYESCAN_EYESCAN_DQ_SYNC_EN));
	io_32_write_fld_multi(DRAMC_REG_SHU_ODTCTRL,
		p_fld(CLEAR_FLD, SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG) |
		p_fld(CLEAR_FLD, SHU_ODTCTRL_RODTEN_SELPH_CG_IG));
	io_32_write_fld_align(DRAMC_REG_SHU_DQSOSC_PRD, CLEAR_FLD,
		SHU_DQSOSC_PRD_DQSOSC_PRDCNT);
	io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL0, SET_FLD,
		SHU_B0_DLL0_RG_ARPISM_MCK_SEL_B0_SHU);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL0, SET_FLD,
		SHU_B1_DLL0_RG_ARPISM_MCK_SEL_B1_SHU);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL0, SET_FLD,
		SHU_B2_DLL0_RG_ARPISM_MCK_SEL_B2_SHU);
	io_32_write_fld_align_all(DDRPHY_SHU_CA_DLL0, SET_FLD,
		SHU_CA_DLL0_RG_ARPISM_MCK_SEL_CA_SHU);

	io_32_write_fld_multi_all(DRAMC_REG_SHURK0_DQSCAL,
		p_fld(0, SHURK0_DQSCAL_R0DQSIENLLMTEN) |
		p_fld(0, SHURK0_DQSCAL_R0DQSIENHLMTEN));
	io_32_write_fld_multi_all(DRAMC_REG_SHURK1_DQSCAL,
		p_fld(0, SHURK1_DQSCAL_R1DQSIENLLMTEN) |
		p_fld(0, SHURK1_DQSCAL_R1DQSIENHLMTEN));
	io_32_write_fld_multi_all(DRAMC_REG_SHU_STBCAL,
		p_fld(1, SHU_STBCAL_DQSG_MODE) |
		p_fld(1, SHU_STBCAL_PICGLAT));

#if DramcHWDQSGatingTracking_DVT_JADE_TRACKING_MODE

	io32_write_4b_all(DRAMC_REG_SHU_PIPE, 0xF0000000);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ6, CLEAR_FLD,
		SHU_B2_DQ6_RG_ARPI_OFFSET_DQSIEN_B2);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ6, CLEAR_FLD,
		SHU_B1_DQ6_RG_ARPI_OFFSET_DQSIEN_B1);
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ6 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		SHU_B0_DQ6_RG_ARPI_OFFSET_DQSIEN_B0);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD6 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		SHU_CA_CMD6_RG_ARPI_OFFSET_CLKIEN);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ6 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		SHU_B1_DQ6_RG_ARPI_OFFSET_DQSIEN_B1);

	{
		io_32_write_fld_align(DDRPHY_SHU_B0_DLL1, SET_FLD,
			RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B1_DLL1, SET_FLD,
			RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B2_DLL1, SET_FLD,
			RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL1, SET_FLD,
			RG_ARCMD_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B0_DLL1 + SHIFT_TO_CHB_ADDR,
			SET_FLD, RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B1_DLL1 + SHIFT_TO_CHB_ADDR,
			SET_FLD, RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B2_DLL1 + SHIFT_TO_CHB_ADDR,
			SET_FLD, RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL1 + SHIFT_TO_CHB_ADDR,
			SET_FLD, RG_ARCMD_REV_BIT_05_RX_SER_RST_MODE);
	}

	io_32_write_fld_align_all(DRAMC_REG_SHU_DQSG, 0x9,
		SHU_DQSG_STB_UPDMASKCYC);
	io_32_write_fld_align_all(DRAMC_REG_SHU_DQSG, SET_FLD,
		SHU_DQSG_STB_UPDMASK_EN);

	if (p->frequency >= DDR1600_FREQ) {
		io_32_write_fld_align_all(DRAMC_REG_SHU_STBCAL, 3,
			SHU_STBCAL_DMSTBLAT);
		/*
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM2, 0x7, SHU_ACTIM2_TR2W);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_AC_TIME_05T, 0, SHU_AC_TIME_05T_TR2W_05T);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x5, SHU_ACTIM_XRT_XRTW2R);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x6, SHU_ACTIM_XRT_XRTR2W);
		 */
	} else if (p->frequency >= DDR1200_FREQ) {
		io_32_write_fld_align_all(DRAMC_REG_SHU_STBCAL, 2,
			SHU_STBCAL_DMSTBLAT);
		/*
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM2, 0x5, SHU_ACTIM2_TR2W);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_AC_TIME_05T, 0, SHU_AC_TIME_05T_TR2W_05T);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x5, SHU_ACTIM_XRT_XRTW2R);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x6, SHU_ACTIM_XRT_XRTR2W);
		 */
	} else {
		io_32_write_fld_align_all(DRAMC_REG_SHU_STBCAL, 1,
			SHU_STBCAL_DMSTBLAT);
		/*
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM2, 0x4, SHU_ACTIM2_TR2W);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_AC_TIME_05T, 0, SHU_AC_TIME_05T_TR2W_05T);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x5, SHU_ACTIM_XRT_XRTW2R);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x6, SHU_ACTIM_XRT_XRTR2W);
		 */
	}

	/*
	 * For LP3 FFFF corner IC pass LTLV test
	 * Need to check with
	 */
	io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ5, CLEAR_FLD,
		SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ5, CLEAR_FLD,
		SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ5, CLEAR_FLD,
		SHU_B2_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B2);
	io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD5, CLEAR_FLD,
		SHU_CA_CMD5_RG_RX_ARCLK_DQSIEN_DLY);

#else
	/*
	 * Valid delay mode debug : monitor window
	 * io_32_write_fld_align(DDRPHY_SHU_B0_DQ5+SHIFT_TO_CHB_ADDR, 7, SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0);
	 * io_32_write_fld_align(DDRPHY_SHU_B1_DQ5+SHIFT_TO_CHB_ADDR, 7, SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1);
	 * io_32_write_fld_align_all(DRAMC_REG_SHU_PIPE, 0xF0000000,PHY_FLD_FULL);
	 */
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSG), \
		p_fld(9, SHU_DQSG_STB_UPDMASKCYC) | \
		p_fld(1, SHU_DQSG_STB_UPDMASK_EN));
#endif

	io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ5, CLEAR_FLD,
		SHU_B0_DQ5_RG_ARPI_FB_B0);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ5, CLEAR_FLD,
		SHU_B1_DQ5_RG_ARPI_FB_B1);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ5, CLEAR_FLD,
		SHU_B2_DQ5_RG_ARPI_FB_B2);
	io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD5, CLEAR_FLD,
		SHU_CA_CMD5_RG_ARPI_FB_CA);
#if 0
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ5 + SHIFT_TO_CHB_ADDR, 0x4,
		SHU_B0_DQ5_RG_ARPI_FB_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ5 + SHIFT_TO_CHB_ADDR, 0x4,
		SHU_B1_DQ5_RG_ARPI_FB_B1);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD5 + SHIFT_TO_CHB_ADDR, 0x4,
		SHU_CA_CMD5_RG_ARPI_FB_CA);
#endif

	io_32_write_fld_multi(DRAMC_REG_DRAMCTRL,
		p_fld(0x3, DRAMCTRL_PREA_RK) |
		p_fld(CLEAR_FLD, DRAMCTRL_FW2R) |
		p_fld(SET_FLD, DRAMCTRL_DYNMWREN) |
		p_fld(CLEAR_FLD, DRAMCTRL_CLKWITRFC) |
		p_fld(CLEAR_FLD, DRAMCTRL_TMRR2WDIS) |
		p_fld(CLEAR_FLD, DRAMCTRL_ADRBIT3DEC) |
		p_fld(CLEAR_FLD, DRAMCTRL_CTOREQ_HPRI_OPT));
	io_32_write_fld_align(DRAMC_REG_MISCTL0, SET_FLD,
		MISCTL0_PBC_ARB_EN);
	io_32_write_fld_align(DRAMC_REG_PERFCTL0, CLEAR_FLD,
		PERFCTL0_WRFIFO_OPT);
	io_32_write_fld_multi(DRAMC_REG_PERFCTL0,
		p_fld(CLEAR_FLD, PERFCTL0_REORDEREN) |
		p_fld(CLEAR_FLD, PERFCTL0_RWHPRICTL) |
		p_fld(SET_FLD, PERFCTL0_EMILLATEN) |
		p_fld(SET_FLD, PERFCTL0_RWAGEEN) |
		p_fld(SET_FLD, PERFCTL0_RWHPRIEN));
	io_32_write_fld_align(DRAMC_REG_SREFCTRL, 0x8, SREFCTRL_SREFDLY);

	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, SET_FLD,
		SPCMDCTRL_REFR_BLOCKEN);
	io_32_write_fld_align(DRAMC_REG_HW_MRR_FUN, CLEAR_FLD,
		HW_MRR_FUN_TMRR_ENA);

	io_32_write_fld_multi(DRAMC_REG_SHUCTRL,
		p_fld(SET_FLD, SHUCTRL_VRCG_EN) |
		p_fld(0x3, SHUCTRL_R_DVFS_PICG_MARGIN2) |
		p_fld(0x3, SHUCTRL_R_DVFS_PICG_MARGIN3));
	io_32_write_fld_multi(DRAMC_REG_SHUCTRL2,
		p_fld(SET_FLD, SHUCTRL2_SHORTQ_OPT) |
		p_fld(0x3, SHUCTRL2_R_DVFS_PICG_MARGIN) |
		p_fld(CLEAR_FLD, SHUCTRL2_R_DVFS_SREF_OPT));

	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(CLEAR_FLD, REFCTRL0_REFNA_OPT) |
		p_fld(0x4, REFCTRL0_DISBYREFNUM) |
		p_fld(CLEAR_FLD, REFCTRL0_UPDBYWR));
	io_32_write_fld_multi(DDRPHY_B0_DQ6,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0));
	io_32_write_fld_multi(DDRPHY_CA_CMD6,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_EN));

#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	io_32_write_fld_multi(DDRPHY_B0_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
	io_32_write_fld_multi(DDRPHY_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_EN));

	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_EN_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_EN_B2));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7,
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_EN_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_EN_B2));

	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1));
	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_CA_CMD7_R_DMRXRANK_CLK_LAT) |
		p_fld(SET_FLD, SHU_CA_CMD7_R_DMRXRANK_CLK_EN) |
		p_fld(SET_FLD, SHU_CA_CMD7_R_DMRXRANK_CMD_LAT) |
		p_fld(SET_FLD, SHU_CA_CMD7_R_DMRXRANK_CMD_EN));

#else
	io_32_write_fld_align(DRAMC_REG_STBCAL2, SET_FLD,
		STBCAL2_STB_PICG_EARLY_1T_EN);

	io_32_write_fld_multi(DDRPHY_B0_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
	io_32_write_fld_multi(DDRPHY_B2_DQ6,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2));
	io_32_write_fld_multi(DDRPHY_B2_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2));
	io_32_write_fld_multi(DDRPHY_B1_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
	io_32_write_fld_multi(DDRPHY_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_EN));
#endif

	io_32_write_fld_multi_all(DDRPHY_CA_DLL_ARPI1,
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_CS_JUMP_EN) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_CLK_JUMP_EN) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_CMD_JUMP_EN) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_CLKIEN_JUMP_EN) |
		p_fld(SET_FLD, CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA_REG_OPT));
	io_32_write_fld_multi_all(DDRPHY_B0_DLL_ARPI1,
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_DQS_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_DQM_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_DQSIEN_JUMP_EN_B0) |
		p_fld(SET_FLD, B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0_REG_OPT));
	io_32_write_fld_multi_all(DDRPHY_B1_DLL_ARPI1,
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_DQS_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_DQM_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_DQSIEN_JUMP_EN_B1) |
		p_fld(SET_FLD, B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1_REG_OPT));
	io_32_write_fld_multi_all(DDRPHY_B2_DLL_ARPI1,
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_DQS_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_DQM_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_DQSIEN_JUMP_EN_B2) |
		p_fld(SET_FLD, B2_DLL_ARPI1_RG_ARPISM_MCK_SEL_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPISM_MCK_SEL_B2_REG_OPT));

	io_32_write_fld_align_all(DDRPHY_B0_DQ5, CLEAR_FLD,
		B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ5, CLEAR_FLD,
		B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ5, CLEAR_FLD,
		B2_DQ5_RG_RX_ARDQS0_DVS_EN_B2);
	io_32_write_fld_align_all(DDRPHY_CA_CMD5, CLEAR_FLD,
		CA_CMD5_RG_RX_ARCLK_DVS_EN);

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL4, 0x13300000,
		MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL4 + SHIFT_TO_CHB_ADDR,
		0x22200a00, MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);

	io_32_write_fld_multi_all(DRAMC_REG_PERFCTL0,
		p_fld(SET_FLD, PERFCTL0_RWSPLIT) |
		p_fld(CLEAR_FLD, PERFCTL0_REORDEREN));

#if NON_EXIST_RG
#if FIX_CROSSRK_XRT_05T_OPT
	io_32_write_fld_align_all(DRAMC_REG_PERFCTL0, CLEAR_FLD,
		PERFCTL0_XRT_05T_OPT);
#else
	io_32_write_fld_align_all(DRAMC_REG_PERFCTL0, SET_FLD,
		PERFCTL0_XRT_05T_OPT);
#endif
#endif
	io_32_write_fld_align_all(DRAMC_REG_CLKCTRL, SET_FLD,
		CLKCTRL_SEQCLKRUN3);
	io_32_write_fld_align_all(DRAMC_REG_STBCAL1, CLEAR_FLD,
		STBCAL1_DLLFRZ_MON_PBREF_OPT);
	io_32_write_fld_align_all(DRAMC_REG_ARBCTL, CLEAR_FLD,
		ARBCTL_DBIWR_IMP_EN );

	io_32_write_fld_align(DDRPHY_SHU_CA_CMD7, CLEAR_FLD,
		SHU_CA_CMD7_R_DMRANKRXDVS_CA);

	io_32_write_fld_align_all(DRAMC_REG_CKECTRL, CLEAR_FLD,
		CKECTRL_CKEPBDIS);

	io_32_write_fld_multi_all(DDRPHY_SHU_B0_DQ3,
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQ_PU_PRE_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQ_PDB_PRE_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQS0_PDB_PRE_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQS0_PU_PRE_B0));
	io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ3,
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQ_PU_PRE_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQ_PDB_PRE_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQS0_PDB_PRE_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQS0_PU_PRE_B1));
	io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ3,
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQ_PU_PRE_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQ_PDB_PRE_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQS0_PDB_PRE_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQS0_PU_PRE_B2));

#if NON_EXIST_RG
	io_32_write_fld_align_all(DRAMC_REG_SPCMDCTRL, CLEAR_FLD,
		SPCMDCTRL_HMR4_TOG_OPT);
#endif

	io_32_write_fld_align_all(DDRPHY_B0_DQ9, SET_FLD,
		B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_R_DMRXFIFO_STBENCMP_EN_B2);
	io_32_write_fld_align_all(DDRPHY_CA_CMD10, SET_FLD,
		CA_CMD10_R_DMRXFIFO_STBENCMP_EN_CA);

	return DRAM_OK;
}

static void dramc_setting_ddr3(DRAMC_CTX_T *p)
{
	unsigned char reg_txdly_dqs, reg_txdly_dqs_oen;
	unsigned char reg_txdly_dqdqm, reg_txdly_dqdqm_oen;
	unsigned char reg_dly_dqs, reg_dly_dqs_oen;
	unsigned char reg_dly_dqdqm, reg_dly_dqdqm_oen;
	unsigned char pinmux;

	if (p->freq_sel == DDR_DDR1333) {
		reg_txdly_dqs = 0x1;
		reg_txdly_dqs_oen = 0x1;
		reg_dly_dqs = 0x3;
		reg_dly_dqs_oen = 0x1;

		reg_txdly_dqdqm = 0x1;
		reg_txdly_dqdqm_oen = 0x1;
		reg_dly_dqdqm = 0x3;
		reg_dly_dqdqm_oen = 0x1;
	} else if (p->freq_sel == DDR_DDR1600) {
		reg_txdly_dqs = 0x2;
		reg_txdly_dqs_oen = 0x1;
		reg_dly_dqs = 0x1;
		reg_dly_dqs_oen = 0x3;

		reg_txdly_dqdqm = 0x2;
		reg_txdly_dqdqm_oen = 0x1;
		reg_dly_dqdqm = 0x1;
		reg_dly_dqdqm_oen = 0x3;
	} else if (p->freq_sel == DDR_DDR1866) {
		reg_txdly_dqs = 0x2;
		reg_txdly_dqs_oen = 0x1;
		reg_dly_dqs = 0x1;
		reg_dly_dqs_oen = 0x7;

		reg_txdly_dqdqm = 0x2;
		reg_txdly_dqdqm_oen = 0x1;
		reg_dly_dqdqm = 0x1;
		reg_dly_dqdqm_oen = 0x7;
	}

	auto_refresh_cke_off(p);

	io_32_write_fld_multi_all(DDRPHY_CKMUX_SEL,
		p_fld(SET_FLD, CKMUX_SEL_R_PHYCTRLMUX) |
		p_fld(SET_FLD, CKMUX_SEL_R_PHYCTRLDCM));

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, SET_FLD,
		MISC_CG_CTRL0_W_CHG_MEM);

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, CLEAR_FLD,
		MISC_CG_CTRL0_CLK_MEM_SEL);
	io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL0, SET_FLD,
		MISC_CG_CTRL0_RG_FREERUN_MCK_CG);

#if NON_EXIST_RG
	io_32_write_fld_align_all(DDRPHY_MISC_CTRL0, SET_FLD,
		MISC_CTRL0_R_DMRDSEL_DIV2_OPT);
#endif

	if (0/* p->bDLP3 */) {/* cc note, review this RG!!! */
		io_32_write_fld_align_all(DDRPHY_MISC_CTRL1, SET_FLD,
			MISC_CTRL1_R_DMMUXCA_SEC);
	}

	io_32_write_fld_multi(DDRPHY_MISC_SPM_CTRL1,
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_PHYPLL_MODE_SW) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B0) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B0) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B1) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B2) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_CA));
	io_32_write_fld_multi(DDRPHY_MISC_SPM_CTRL1 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B0) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B1) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_CA));

	io_32_write_fld_align_all(DDRPHY_MISC_SPM_CTRL0, 0xfbffefff,
		MISC_SPM_CTRL0_PHY_SPM_CTL0);

	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL2, 0xfffeffef,
		MISC_SPM_CTRL2_PHY_SPM_CTL2);
	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL2 + SHIFT_TO_CHB_ADDR, 0x7ffeffef,
		MISC_SPM_CTRL2_PHY_SPM_CTL2);

	io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL2, 0x3e003bf,
		MISC_CG_CTRL2_RG_MEM_DCM_CTL);
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL4, 0x13300000,
		MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL4 + SHIFT_TO_CHB_ADDR,
		0x222a2a00, MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);
	io_32_write_fld_multi_all(DDRPHY_SHU_PLL1,
		p_fld(SET_FLD, SHU_PLL1_RG_RPHYPLLGP_CK_SEL) |
		p_fld(SET_FLD, SHU_PLL1_R_SHU_AUTO_PLL_MUX) |
		p_fld(0x7, SHU_PLL1_SHU1_PLL1_RFU));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_MIDPI_DIV4_ENABLE) |
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRANKRXDVS_B0));
	io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ7,
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRANKRXDVS_B1));
	io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ7,
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_EN_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_EN_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_MIDPI_DIV4_ENABLE) |
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRANKRXDVS_B2));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_MIDPI_DIV4_ENABLE) |
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRANKRXDVS_B0));
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRANKRXDVS_B1));
#endif

	/* cc add since this bit default is 1'b1, which will cause CS blocked */
	io_32_write_fld_align(DRAMC_REG_SLP4_TESTMODE, CLEAR_FLD,
		SLP4_TESTMODE_DQM_FIX_LOW);

	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_CA_CMD7_R_DMRXDVS_PBYTE_FLAG_OPT_CA) |
		p_fld(SET_FLD, SHU_CA_CMD7_R_DMRANKRXDVS_CA));
	/*
	 * io_32_write_fld_align(DDRPHY_SHU_PLL4, 0xfe, SHU_PLL4_RG_RPHYPLL_RESERVED);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL6, 0xfe, SHU_PLL6_RG_RCLRPLL_RESERVED);
	 */
	io_32_write_fld_multi_all(DDRPHY_CA_CMD3,
		p_fld(CLEAR_FLD, CA_CMD3_RG_TX_ARCMD_OE_DIS) |
		p_fld(CLEAR_FLD, CA_CMD3_RG_TX_ARCMD_ODTEN_DIS) |
		p_fld(CLEAR_FLD, CA_CMD3_RG_TX_ARCLK_OE_DIS) |
		p_fld(CLEAR_FLD, CA_CMD3_RG_TX_ARCLK_ODTEN_DIS));
	io_32_write_fld_multi_all(DDRPHY_B0_DQ2,
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQ_OE_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQ_ODTEN_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQS0_OE_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B0));
	io_32_write_fld_multi_all(DDRPHY_B1_DQ2,
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQ_OE_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQ_ODTEN_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQS0_OE_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B1));
	io_32_write_fld_multi_all(DDRPHY_B2_DQ2,
		p_fld(CLEAR_FLD, B2_DQ2_RG_TX_ARDQ_OE_DIS_B2) |
		p_fld(CLEAR_FLD, B2_DQ2_RG_TX_ARDQ_ODTEN_DIS_B2) |
		p_fld(CLEAR_FLD, B2_DQ2_RG_TX_ARDQS0_OE_DIS_B2) |
		p_fld(CLEAR_FLD, B2_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B2));

	io_32_write_fld_align(DDRPHY_B0_DQ9, SET_FLD,
		B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_R_DMRXDVS_RDSEL_LAT_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD10, SET_FLD,
		CA_CMD10_R_DMRXDVS_RDSEL_LAT_CA);
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR,
		SET_FLD, B0_DQ9_R_DMRXDVS_RDSEL_LAT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR,
		SET_FLD, B1_DQ9_R_DMRXDVS_RDSEL_LAT_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9 + SHIFT_TO_CHB_ADDR,
		SET_FLD, B2_DQ9_R_DMRXDVS_RDSEL_LAT_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR,
		SET_FLD, CA_CMD10_R_DMRXDVS_RDSEL_LAT_CA);

	io_32_write_fld_multi_all(DDRPHY_MISC_VREF_CTRL,
		p_fld(0xfffe, MISC_VREF_CTRL_PHY_SPM_CTL4) |
		p_fld(0x3f, MISC_VREF_CTRL_MISC_LP_8X_MUX));

	io_32_write_fld_multi(DDRPHY_MISC_SHU_OPT,
		p_fld(0x2, MISC_SHU_OPT_R_DQB2_SHU_PHDET_SPM_EN) |
		p_fld(0x1, MISC_SHU_OPT_R_DQB2_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x1, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB1_SHU_PHDET_SPM_EN) |
		p_fld(0x1, MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB0_SHU_PHDET_SPM_EN) |
		p_fld(0x1, MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN));
	io_32_write_fld_multi(DDRPHY_MISC_SHU_OPT + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, MISC_SHU_OPT_R_DQB2_SHU_PHDET_SPM_EN) |
		p_fld(0x1, MISC_SHU_OPT_R_DQB2_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB1_SHU_PHDET_SPM_EN) |
		p_fld(0x1, MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB0_SHU_PHDET_SPM_EN) |
		p_fld(0x1, MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN));
#if 0
	io_32_write_fld_align(DDRPHY_B0_DQ9, CLEAR_FLD,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9, 0x7,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_CA_CMD10, CLEAR_FLD,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
#endif
#if 0
	io_32_write_fld_multi(DDRPHY_CA_CMD2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCMD_OE_DIS) |
		p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCMD_ODTEN_DIS) |
		p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCLK_OE_DIS) |
		p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCLK_ODTEN_DIS));
	io_32_write_fld_multi(DDRPHY_B0_DQ2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQ_OE_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQ_ODTEN_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQS_OE_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQS_ODTEN_DIS_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQ_OE_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQ_ODTEN_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQS_OE_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQS_ODTEN_DIS_B1));
#endif
#if 0
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, 0x7,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
#endif
	if (p->pinmux_type == PINMUX_TYPE_PC3_X16X2)
		pinmux = 0x5;
	else if (p->pinmux_type == PINMUX_TYPE_PC3_X8X4)
		pinmux = 0x6;
	else
		pinmux = 0x7; /* 16X1 */

	io_32_write_fld_multi_all(DDRPHY_MISC_CTRL1,
		p_fld(pinmux & 0x3, MISC_CTRL1_R_DMPINMUX) |
		p_fld((pinmux >> 2) & 0x1, MISC_CTRL1_R_DMPINMUX_BIT2) |
		p_fld(CLEAR_FLD, MISC_CTRL1_R_DMARPIDQ_SW) |
		p_fld(CLEAR_FLD, MISC_CTRL1_R_DMMCTLPLL_CKSEL));
#if 0
	io_32_write_fld_multi(DDRPHY_MISC_CTRL1 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, MISC_CTRL1_R_DMPINMUX) |
		p_fld(CLEAR_FLD, MISC_CTRL1_R_DMARPIDQ_SW) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMMCTLPLL_CKSEL));
#endif
	io_32_write_fld_align(DDRPHY_PLL3, CLEAR_FLD,
		PLL3_RG_RPHYPLL_TSTOP_EN);

	io_32_write_fld_align_all(DDRPHY_MISC_VREF_CTRL, SET_FLD,
		MISC_VREF_CTRL_RG_RVREF_VREF_EN);
	io_32_write_fld_multi_all(DDRPHY_SHU_MISC0,
		p_fld(CLEAR_FLD, SHU_MISC0_RG_RVREF_SEL_CMD) |
		p_fld(SET_FLD, SHU_MISC0_RG_RVREF_DDR3_SEL) |
		p_fld(CLEAR_FLD, SHU_MISC0_RG_RVREF_DDR4_SEL) |
		p_fld(0xb, SHU_MISC0_RG_RVREF_SEL_DQ));
	io_32_write_fld_align_all(DDRPHY_MISC_IMP_CTRL0, SET_FLD,
		MISC_IMP_CTRL0_RG_RIMP_DDR3_SEL);
#if 0
	io_32_write_fld_align(DDRPHY_MISC_VREF_CTRL + SHIFT_TO_CHB_ADDR,
		SET_FLD, MISC_VREF_CTRL_RG_RVREF_VREF_EN);
	io_32_write_fld_multi(DDRPHY_SHU_MISC0 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_MISC0_RG_RVREF_SEL_CMD) |
		p_fld(SET_FLD, SHU_MISC0_RG_RVREF_DDR3_SEL) |
		p_fld(CLEAR_FLD, SHU_MISC0_RG_RVREF_DDR4_SEL) |
		p_fld(0xb, SHU_MISC0_RG_RVREF_SEL_DQ));
	io_32_write_fld_align(DDRPHY_MISC_IMP_CTRL0 + SHIFT_TO_CHB_ADDR,
		SET_FLD, MISC_IMP_CTRL0_RG_RIMP_DDR3_SEL);
#endif
	io_32_write_fld_multi(DDRPHY_B0_RXDVS0,
		p_fld(CLEAR_FLD, B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0) |
		p_fld(SET_FLD, B0_RXDVS0_R_RX_RANKINSEL_B0) |
		p_fld(CLEAR_FLD, B0_RXDVS0_R_RX_RANKINCTL_B0));
	io_32_write_fld_multi(DDRPHY_CA_RXDVS0,
		p_fld(CLEAR_FLD, CA_RXDVS0_R_RX_DLY_TRACK_CG_EN_CA) |
		p_fld(CLEAR_FLD, CA_RXDVS0_R_RX_RANKINSEL_CA) |
		p_fld(SET_FLD, CA_RXDVS0_CA_RXDVS0_RFU) |
		p_fld(CLEAR_FLD, CA_RXDVS0_R_RX_RANKINCTL_CA));
	io_32_write_fld_multi(DDRPHY_B2_RXDVS0,
		p_fld(CLEAR_FLD, B2_RXDVS0_R_RX_DLY_TRACK_CG_EN_B2) |
		p_fld(SET_FLD, B2_RXDVS0_R_RX_RANKINSEL_B2) |
		p_fld(CLEAR_FLD, B2_RXDVS0_R_RX_RANKINCTL_B2));
	io_32_write_fld_multi(DDRPHY_B1_RXDVS0,
		p_fld(CLEAR_FLD, B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1) |
		p_fld(SET_FLD, B1_RXDVS0_R_RX_RANKINSEL_B1) |
		p_fld(CLEAR_FLD, B1_RXDVS0_R_RX_RANKINCTL_B1));
	io_32_write_fld_multi(DDRPHY_B0_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0) |
		p_fld(SET_FLD, B0_RXDVS0_R_RX_RANKINSEL_B0) |
		p_fld(CLEAR_FLD, B0_RXDVS0_R_RX_RANKINCTL_B0));
	io_32_write_fld_multi(DDRPHY_CA_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_RXDVS0_R_RX_DLY_TRACK_CG_EN_CA) |
		p_fld(CLEAR_FLD, CA_RXDVS0_R_RX_RANKINSEL_CA) |
		p_fld(SET_FLD, CA_RXDVS0_CA_RXDVS0_RFU) |
		p_fld(CLEAR_FLD, CA_RXDVS0_R_RX_RANKINCTL_CA));
	io_32_write_fld_multi(DDRPHY_B2_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B2_RXDVS0_R_RX_DLY_TRACK_CG_EN_B2) |
		p_fld(SET_FLD, B2_RXDVS0_R_RX_RANKINSEL_B2) |
		p_fld(CLEAR_FLD, B2_RXDVS0_R_RX_RANKINCTL_B2));
	io_32_write_fld_multi(DDRPHY_B1_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1) |
		p_fld(SET_FLD, B1_RXDVS0_R_RX_RANKINSEL_B1) |
		p_fld(CLEAR_FLD, B1_RXDVS0_R_RX_RANKINCTL_B1));

	io_32_write_fld_multi(DDRPHY_B2_RXDVS0,
		p_fld(CLEAR_FLD, B2_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B2) |
		p_fld(SET_FLD, B2_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B2));
	io_32_write_fld_multi(DDRPHY_B1_RXDVS0,
		p_fld(CLEAR_FLD, B1_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B1) |
		p_fld(SET_FLD, B1_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B1));
	io_32_write_fld_multi(DDRPHY_B0_RXDVS0,
		p_fld(CLEAR_FLD, B0_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B0) |
		p_fld(SET_FLD, B0_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B0));
	io_32_write_fld_multi(DDRPHY_B0_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B0_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B0) |
		p_fld(SET_FLD, B0_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B0));
	io_32_write_fld_multi(DDRPHY_B2_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B2_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B2) |
		p_fld(SET_FLD, B2_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B2));
	io_32_write_fld_multi(DDRPHY_B1_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B1_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B1) |
		p_fld(SET_FLD, B1_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B1));
	io_32_write_fld_multi(DDRPHY_CA_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, CA_RXDVS0_R_DMRXDVS_CNTCMP_OPT_CA) |
		p_fld(SET_FLD, CA_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_CA));
#if 0
	io_32_write_fld_align(DDRPHY_R0_B1_RXDVS2, SET_FLD,
		R0_B1_RXDVS2_R_RK0_DVS_FDLY_MODE_B1);
	io_32_write_fld_align(DDRPHY_R1_B1_RXDVS2, SET_FLD,
		R1_B1_RXDVS2_R_RK1_DVS_FDLY_MODE_B1);
	io_32_write_fld_align(DDRPHY_R0_B0_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R0_B0_RXDVS2_R_RK0_DVS_FDLY_MODE_B0);
	io_32_write_fld_align(DDRPHY_R1_B0_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R1_B0_RXDVS2_R_RK1_DVS_FDLY_MODE_B0);

	io_32_write_fld_align(DDRPHY_R0_CA_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R0_CA_RXDVS2_R_RK0_DVS_FDLY_MODE_CA);
	io_32_write_fld_align(DDRPHY_R1_CA_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R1_CA_RXDVS2_R_RK1_DVS_FDLY_MODE_CA);
	io_32_write_fld_align(DDRPHY_R0_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R0_B1_RXDVS2_R_RK0_DVS_FDLY_MODE_B1);
	io_32_write_fld_align(DDRPHY_R1_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R1_B1_RXDVS2_R_RK1_DVS_FDLY_MODE_B1);
#endif
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ5, CLEAR_FLD,
		SHU_B2_DQ5_RG_RX_ARDQS0_DVS_DLY_B2);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ5, CLEAR_FLD,
		SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1);
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0));
	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_CA_CMD5_RG_RX_ARCLK_DVS_DLY) |
		p_fld(CLEAR_FLD, SHU_CA_CMD5_RG_RX_ARCMD_VREF_SEL));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B2_DQ5_RG_RX_ARDQS0_DVS_DLY_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1));

#if 0//cc mark LEGACY_RX_DLY
	LegacyRxDly_LP3(p);
#endif
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD2_RG_RK0_RX_ARCA1_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD2_RG_RK0_RX_ARCA1_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD2_RG_RK0_RX_ARCA0_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD2_RG_RK0_RX_ARCA0_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD3 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD3_RG_RK0_RX_ARCA3_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD3_RG_RK0_RX_ARCA3_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD3_RG_RK0_RX_ARCA2_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD3_RG_RK0_RX_ARCA2_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD4_RG_RK0_RX_ARCA5_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD4_RG_RK0_RX_ARCA5_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD4_RG_RK0_RX_ARCA4_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD4_RG_RK0_RX_ARCA4_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE1_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE1_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE0_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE0_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD6_RG_RK0_RX_ARCS0_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD6_RG_RK0_RX_ARCS0_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD6_RG_RK0_RX_ARCKE2_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD6_RG_RK0_RX_ARCKE2_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD7 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD7_RG_RK0_RX_ARCS2_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD7_RG_RK0_RX_ARCS2_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD7_RG_RK0_RX_ARCS1_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD7_RG_RK0_RX_ARCS1_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD8 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD8_RG_RK0_RX_ARCLK_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD8_RG_RK0_RX_ARCLK_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD2_RG_RK1_RX_ARCA1_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD2_RG_RK1_RX_ARCA1_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD2_RG_RK1_RX_ARCA0_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD2_RG_RK1_RX_ARCA0_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD3 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD3_RG_RK1_RX_ARCA3_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD3_RG_RK1_RX_ARCA3_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD3_RG_RK1_RX_ARCA2_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD3_RG_RK1_RX_ARCA2_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD4_RG_RK1_RX_ARCA5_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD4_RG_RK1_RX_ARCA5_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD4_RG_RK1_RX_ARCA4_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD4_RG_RK1_RX_ARCA4_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE1_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE1_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE0_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE0_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD6_RG_RK1_RX_ARCS0_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD6_RG_RK1_RX_ARCS0_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD6_RG_RK1_RX_ARCKE2_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD6_RG_RK1_RX_ARCKE2_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD7 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD7_RG_RK1_RX_ARCS2_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD7_RG_RK1_RX_ARCS2_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD7_RG_RK1_RX_ARCS1_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD7_RG_RK1_RX_ARCS1_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD8 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD8_RG_RK1_RX_ARCLK_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD8_RG_RK1_RX_ARCLK_R_DLY));

#if 0
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS1,
		p_fld(0x4, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LEAD) |
		p_fld(0x4, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS1,
		p_fld(0x4, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LEAD) |
		p_fld(0x4, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R0_B0_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R0_B0_RXDVS1_R_RK0_B0_DVS_TH_LEAD) |
		p_fld(0x4, R0_B0_RXDVS1_R_RK0_B0_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_B0_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R1_B0_RXDVS1_R_RK1_B0_DVS_TH_LEAD) |
		p_fld(0x4, R1_B0_RXDVS1_R_RK1_B0_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R0_CA_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R0_CA_RXDVS1_R_RK0_CA_DVS_TH_LEAD) |
		p_fld(0x4, R0_CA_RXDVS1_R_RK0_CA_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_CA_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R1_CA_RXDVS1_R_RK1_CA_DVS_TH_LEAD) |
		p_fld(0x4, R1_CA_RXDVS1_R_RK1_CA_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LEAD) |
		p_fld(0x4, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LEAD) |
		p_fld(0x4, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LAG));
#endif
	io_32_write_fld_multi(DRAMC_REG_SHU_DRVING1,
		p_fld(SET_FLD, SHU_DRVING1_DIS_IMP_ODTN_TRACK) |
		p_fld(SET_FLD, SHU_DRVING1_DIS_IMPCAL_HW));

	io_32_write_fld_multi_all(DDRPHY_R0_B2_RXDVS2,
		p_fld(0x2, R0_B2_RXDVS2_R_RK0_DVS_MODE_B2) |
		p_fld(SET_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(SET_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B2));
	io_32_write_fld_multi_all(DDRPHY_R1_B1_RXDVS2,
		p_fld(0x2, R1_B2_RXDVS2_R_RK1_DVS_MODE_B2) |
		p_fld(SET_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(SET_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B2));
	io_32_write_fld_multi_all(DDRPHY_R0_B1_RXDVS2,
		p_fld(0x2, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
		p_fld(SET_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(SET_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi_all(DDRPHY_R1_B1_RXDVS2,
		p_fld(0x2, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1) |
		p_fld(SET_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(SET_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R0_B0_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, R0_B0_RXDVS2_R_RK0_DVS_MODE_B0) |
		p_fld(SET_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(SET_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi(DDRPHY_R1_B0_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, R1_B0_RXDVS2_R_RK1_DVS_MODE_B0) |
		p_fld(SET_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(SET_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi(DDRPHY_R0_CA_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, R0_CA_RXDVS2_R_RK0_DVS_MODE_CA) |
		p_fld(SET_FLD, R0_CA_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_CA) |
		p_fld(SET_FLD, R0_CA_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_CA));
	io_32_write_fld_multi(DDRPHY_R1_CA_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, R1_CA_RXDVS2_R_RK1_DVS_MODE_CA) |
		p_fld(SET_FLD, R1_CA_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_CA) |
		p_fld(SET_FLD, R1_CA_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_CA));
#if 0
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
		p_fld(SET_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(SET_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1) |
		p_fld(SET_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(SET_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
#endif
	io_32_write_fld_align_all(DDRPHY_B2_RXDVS0, CLEAR_FLD,
		B2_RXDVS0_R_RX_DLY_TRACK_CG_EN_B2);
	io_32_write_fld_align_all(DDRPHY_B1_RXDVS0, CLEAR_FLD,
		B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1);
	io_32_write_fld_align(DDRPHY_B0_RXDVS0 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0);
	io_32_write_fld_align(DDRPHY_CA_RXDVS0 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		CA_RXDVS0_R_RX_DLY_TRACK_CG_EN_CA);

	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_RG_RX_ARDQ_STBEN_RESETB_B2);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1);
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, SET_FLD,
		CA_CMD10_RG_RX_ARCMD_STBEN_RESETB);
	io_32_write_fld_align(DDRPHY_B2_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B2_DQ9_RG_RX_ARDQ_STBEN_RESETB_B2);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1);
	io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL1, 0xffffffff,
		MISC_CG_CTRL1_R_DVS_DIV4_CG_CTRL);

	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD9,
		p_fld(0, SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
		p_fld(0, SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK) |
		p_fld(0, SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD9,
		p_fld(0, SHU_R1_CA_CMD9_RG_RK1_ARPI_CMD) |
		p_fld(0, SHU_R1_CA_CMD9_RG_RK1_ARPI_CLK) |
		p_fld(0, SHU_R1_CA_CMD9_RG_RK1_ARPI_CS));
	io_32_write_fld_align(DDRPHY_SHU_R0_B1_DQ7,	0x0,
		SHU_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);

#if 0//cc mark LEGACY_DELAY_CELL
	LegacyDlyCellInitLP3(p);
#endif
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x6, SHU_R1_CA_CMD1_RK1_TX_ARCKE2_DLY) |
		p_fld(0x6, SHU_R1_CA_CMD1_RK1_TX_ARCKE1_DLY) |
		p_fld(0x6, SHU_R1_CA_CMD1_RK1_TX_ARCKE0_DLY));

	io_32_write_fld_align(DDRPHY_SHU_R0_B0_DQ7, 0,
		SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0);
	io_32_write_fld_align(DDRPHY_SHU_R1_B0_DQ7, 0,
		SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0);

	io_32_write_fld_multi_all(DDRPHY_SHU_R0_B2_DQ7,
		p_fld(0, SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2) |
		p_fld(0, SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2));
	io_32_write_fld_multi_all(DDRPHY_SHU_R1_B2_DQ7,
		p_fld(0, SHU_R1_B2_DQ7_RK1_ARPI_DQM_B2) |
		p_fld(0, SHU_R1_B2_DQ7_RK1_ARPI_DQ_B2));
	io_32_write_fld_multi_all(DDRPHY_SHU_R0_B1_DQ7,
		p_fld(0, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
		p_fld(0, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	io_32_write_fld_multi_all(DDRPHY_SHU_R1_B1_DQ7,
		p_fld(0, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1) |
		p_fld(0, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD9 + SHIFT_TO_CHB_ADDR,
		p_fld(0, SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
		p_fld(0, SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD9 + SHIFT_TO_CHB_ADDR,
		p_fld(0, SHU_R1_CA_CMD9_RG_RK1_ARPI_CMD) |
		p_fld(0, SHU_R1_CA_CMD9_RG_RK1_ARPI_CS));
	io_32_write_fld_multi(DDRPHY_SHU_R0_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(0, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
		p_fld(0, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(0, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0) |
		p_fld(0, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_R0_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(0xf, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
		p_fld(0xf, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(0xf, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1) |
		p_fld(0xf, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
#endif

	io_32_write_fld_multi(DDRPHY_B2_DQ4,
		p_fld(0x10, B2_DQ4_RG_RX_ARDQS_EYE_R_DLY_B2) |
		p_fld(0x10, B2_DQ4_RG_RX_ARDQS_EYE_F_DLY_B2));
	io_32_write_fld_multi(DDRPHY_B2_DQ5,
		p_fld(CLEAR_FLD, B2_DQ5_RG_RX_ARDQ_EYE_EN_B2) |
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_EYE_SEL_B2) |
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_VREF_EN_B2) |
		p_fld(0x10, B2_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B2) |
		p_fld(0x10, B2_DQ5_B2_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B2_DQ6,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B2) |
		p_fld(CLEAR_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2));
	io_32_write_fld_multi(DDRPHY_B2_DQ5,
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B2) |
		p_fld(CLEAR_FLD, B2_DQ5_B2_DQ5_RFU));

	io_32_write_fld_multi(DDRPHY_B1_DQ4,
		p_fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1) |
		p_fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ5,
		p_fld(CLEAR_FLD, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1) |
		p_fld(0x10, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1) |
		p_fld(0x10, B1_DQ5_B1_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1) |
		p_fld(CLEAR_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ5,
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) |
		p_fld(CLEAR_FLD, B1_DQ5_B1_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_CA_CMD3,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_IN_BUFF_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_ARCMD_RESETB) |
		p_fld(SET_FLD, CA_CMD3_RG_TX_ARCMD_EN));
	io_32_write_fld_multi(DDRPHY_CA_CMD6,
		p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_DDR3_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_TX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_VREF_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN));
#if NON_EXIST_RG
		p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_SER_MODE));
#endif
	/*
	 * ARCMD_DRVP, DRVN , ARCLK_DRVP, DRVN not used anymore
	 * io_32_write_fld_multi(DDRPHY_SHU_CA_CMD1,
		p_fld(SET_FLD, SHU_CA_CMD1_RG_TX_ARCMD_DRVN) |
	 * | p_fld(SET_FLD, SHU_CA_CMD1_RG_TX_ARCMD_DRVP));
	 * io_32_write_fld_multi(DDRPHY_SHU_CA_CMD2,
		p_fld(SET_FLD, SHU_CA_CMD2_RG_TX_ARCLK_DRVN) |
	 * | p_fld(SET_FLD, SHU_CA_CMD2_RG_TX_ARCLK_DRVP));
	 */
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU2_CA_CMD1,
		p_fld(SET_FLD, SHU2_CA_CMD1_RG_TX_ARCMD_DRVN) |
	 * | p_fld(SET_FLD, SHU2_CA_CMD1_RG_TX_ARCMD_DRVP));
	 * io_32_write_fld_multi(DDRPHY_SHU2_CA_CMD2,
		p_fld(SET_FLD, SHU2_CA_CMD2_RG_TX_ARCLK_DRVN) |
	 * | p_fld(SET_FLD, SHU2_CA_CMD2_RG_TX_ARCLK_DRVP));
	 */
	io_32_write_fld_multi(DDRPHY_B0_DQ3,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_ARDQ_RESETB_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_TX_ARDQ_EN_B0));
	io_32_write_fld_multi(DDRPHY_B0_DQ6,
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ1,
		p_fld(SET_FLD, SHU_B0_DQ1_RG_TX_ARDQ_DRVN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ1_RG_TX_ARDQ_DRVP_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ2,
		p_fld(SET_FLD, SHU_B0_DQ2_RG_TX_ARDQS0_DRVN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ2_RG_TX_ARDQS0_DRVP_B0));
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU2_B0_DQ1,
		p_fld(SET_FLD, SHU2_B0_DQ1_RG_TX_ARDQ_DRVN_B0) |
	 * | p_fld(SET_FLD, SHU2_B0_DQ1_RG_TX_ARDQ_DRVP_B0));
	 * io_32_write_fld_multi(DDRPHY_SHU2_B0_DQ2,
		p_fld(SET_FLD, SHU2_B0_DQ2_RG_TX_ARDQS0_DRVN_B0) |
	 * | p_fld(SET_FLD, SHU2_B0_DQ2_RG_TX_ARDQS0_DRVP_B0));
	 */
	io_32_write_fld_multi(DDRPHY_CA_CMD4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x10, CA_CMD4_RG_RX_ARCLK_EYE_R_DLY) |
		p_fld(0x10, CA_CMD4_RG_RX_ARCLK_EYE_F_DLY));
	io_32_write_fld_multi(DDRPHY_CA_CMD5,
		p_fld(CLEAR_FLD, CA_CMD5_RG_RX_ARCMD_EYE_EN));
	io_32_write_fld_multi(DDRPHY_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, CA_CMD5_RG_RX_ARCMD_EYE_EN) |
		p_fld(SET_FLD, CA_CMD5_RG_RX_ARCMD_EYE_SEL) |
		p_fld(SET_FLD, CA_CMD5_RG_RX_ARCMD_VREF_EN) |
		p_fld(0x10, CA_CMD5_RG_RX_ARCMD_EYE_VREF_SEL) |
		p_fld(0x10, CA_CMD5_CA_CMD5_RFU));
	io_32_write_fld_multi(DDRPHY_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_EYE_DLY_DQS_BYPASS) |
		p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_DDR3_SEL) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_DDR3_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_TX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_VREF_SEL) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_EN) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN));
		//cc mark p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_SER_MODE));
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD6, SET_FLD,
		SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		SET_FLD, SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE);

	io_32_write_fld_multi(DDRPHY_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD5_RG_RX_ARCMD_EYE_STBEN_RESETB) |
		p_fld(CLEAR_FLD, CA_CMD5_CA_CMD5_RFU));
	io_32_write_fld_multi(DDRPHY_B0_DQ4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x10, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0) |
		p_fld(0x10, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0));

	io_32_write_fld_multi(DDRPHY_B0_DQ5,
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0));

	io_32_write_fld_multi(DDRPHY_B0_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0) |
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_EYE_SEL_B0) |
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0) |
		p_fld(0x10, B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0) |
		p_fld(0x10, B0_DQ5_B0_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B0_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0));
	io_32_write_fld_multi(DDRPHY_B0_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0) |
		p_fld(CLEAR_FLD, B0_DQ5_B0_DQ5_RFU));

	io_32_write_fld_align(DDRPHY_SHU_B0_DQ6, SET_FLD,
		SHU_B0_DQ6_RG_TX_ARDQ_SER_MODE_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ6, SET_FLD,

		SHU_B1_DQ6_RG_TX_ARDQ_SER_MODE_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ6, SET_FLD,
		SHU_B2_DQ6_RG_TX_ARDQ_SER_MODE_B2);
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ6 + SHIFT_TO_CHB_ADDR, SET_FLD,
		SHU_B0_DQ6_RG_TX_ARDQ_SER_MODE_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ6 + SHIFT_TO_CHB_ADDR, SET_FLD,
		SHU_B1_DQ6_RG_TX_ARDQ_SER_MODE_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ6 + SHIFT_TO_CHB_ADDR, SET_FLD,
		SHU_B2_DQ6_RG_TX_ARDQ_SER_MODE_B2);
	io_32_write_fld_multi(DDRPHY_B2_DQ4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x10, B2_DQ4_RG_RX_ARDQS_EYE_R_DLY_B2) |
		p_fld(0x10, B2_DQ4_RG_RX_ARDQS_EYE_F_DLY_B2));
	io_32_write_fld_multi(DDRPHY_B2_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B2_DQ5_RG_RX_ARDQ_EYE_EN_B2) |
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_EYE_SEL_B2) |
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_VREF_EN_B2) |
		p_fld(0x10, B2_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B2) |
		p_fld(0x10, B2_DQ5_B2_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B2_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B2) |
		p_fld(CLEAR_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2));
	io_32_write_fld_multi(DDRPHY_B2_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B2) |
		p_fld(CLEAR_FLD, B2_DQ5_B2_DQ5_RFU));

	io_32_write_fld_multi(DDRPHY_B1_DQ4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1) |
		p_fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1) |
		p_fld(0x10, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1) |
		p_fld(0x10, B1_DQ5_B1_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B1_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1) |
		p_fld(CLEAR_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) |
		p_fld(CLEAR_FLD, B1_DQ5_B1_DQ5_RFU));

	io_32_write_fld_multi(DDRPHY_PLL4,
		p_fld(CLEAR_FLD, PLL4_RG_RPHYPLL_AD_MCK8X_EN) |
		p_fld(SET_FLD, PLL4_PLL4_RFU) |
		p_fld(SET_FLD, PLL4_RG_RPHYPLL_MCK8X_SEL));

	io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI1,
		p_fld(SET_FLD, CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA) |
		p_fld(SET_FLD, CA_DLL_ARPI1_RG_ARPI_CS_JUMP_EN) |
		p_fld(SET_FLD, CA_DLL_ARPI1_RG_ARPI_CLK_JUMP_EN));
	io_32_write_fld_multi(DDRPHY_B0_DLL_ARPI1,
		p_fld(SET_FLD, B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0) |
		p_fld(SET_FLD, B0_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B0));
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI1, SET_FLD,
		B1_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B1);

	io_32_write_fld_align(DDRPHY_CA_DLL_ARPI1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, CA_DLL_ARPI1_RG_ARPI_CMD_JUMP_EN);
	io_32_write_fld_align(DDRPHY_B0_DLL_ARPI1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, B0_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B0);
#if 0
	io_32_write_fld_align(DDRPHY_B1_DLL_ARPI1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, B1_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B1);
	io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, CA_DLL_ARPI2_RG_ARPI_CG_FB_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));
	io_32_write_fld_multi(DDRPHY_B0_DLL_ARPI2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B0_DLL_ARPI2_RG_ARPI_CG_FB_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
	io_32_write_fld_multi(DDRPHY_B1_DLL_ARPI2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B1_DLL_ARPI2_RG_ARPI_CG_FB_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));

	io_32_write_fld_multi_all(DDRPHY_CA_DLL_ARPI3,
		p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA) |
		p_fld(SET_FLD, CA_DLL_ARPI3_RG_ARPI_CLKIEN_EN));
	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI3, SET_FLD,
		B0_DLL_ARPI3_RG_ARPI_MCTL_EN_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI3, SET_FLD,
		B1_DLL_ARPI3_RG_ARPI_MCTL_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI3, SET_FLD,
		B2_DLL_ARPI3_RG_ARPI_MCTL_EN_B2);
#endif

	io_32_write_fld_multi_all(DDRPHY_CA_CMD3,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_STBENCMP_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_ARCMD_RESETB) |
		p_fld(SET_FLD, CA_CMD3_RG_TX_ARCMD_EN) |
		p_fld(CLEAR_FLD, CA_CMD3_RG_RX_ARCMD_SMT_EN));
	io_32_write_fld_multi_all(DDRPHY_B0_DQ3,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_STBENCMP_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_ARDQ_RESETB_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_TX_ARDQ_EN_B0) |
		p_fld(CLEAR_FLD, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0));
	io_32_write_fld_multi_all(DDRPHY_B1_DQ3,
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_STBENCMP_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_ARDQ_RESETB_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_TX_ARDQ_EN_B1) |
		p_fld(CLEAR_FLD, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1));
	io_32_write_fld_multi_all(DDRPHY_B2_DQ3,
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQ_STBENCMP_EN_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_ARDQ_RESETB_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_TX_ARDQ_EN_B2) |
		p_fld(CLEAR_FLD, B2_DQ3_RG_RX_ARDQ_SMT_EN_B2));
#if 0
	io_32_write_fld_multi(DDRPHY_CA_CMD3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_STBENCMP_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_ARCMD_RESETB) |
		p_fld(SET_FLD, CA_CMD3_RG_TX_ARCMD_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_SMT_EN));
	io_32_write_fld_multi(DDRPHY_B0_DQ3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_STBENCMP_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_ARDQ_RESETB_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_TX_ARDQ_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_STBENCMP_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_ARDQ_RESETB_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_TX_ARDQ_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1));
#endif
	io_32_write_fld_align_all(DDRPHY_SHU_CA_DLL0, CLEAR_FLD,
		SHU_CA_DLL0_RG_ARDLL_DIV_MCTL_CA);
	io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL0, CLEAR_FLD,
		SHU_B0_DLL0_RG_ARDLL_DIV_MCTL_B0);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL0, CLEAR_FLD,
		SHU_B1_DLL0_RG_ARDLL_DIV_MCTL_B1);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL2, CLEAR_FLD,
		SHU_B1_DLL0_RG_ARDLL_DIV_MCTL_B1);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL0, CLEAR_FLD,
		SHU_B2_DLL0_RG_ARDLL_DIV_MCTL_B2);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL2, CLEAR_FLD,
		SHU_B2_DLL0_RG_ARDLL_DIV_MCTL_B2);
#if 0
	io_32_write_fld_align(DDRPHY_CA_DLL_ARPI5 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, CA_DLL_ARPI5_RG_ARDLL_DIV_MCTL_CA);
	io_32_write_fld_align(DDRPHY_B0_DLL_ARPI5 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, B0_DLL_ARPI5_RG_ARDLL_DIV_MCTL_B0);
	io_32_write_fld_align(DDRPHY_B1_DLL_ARPI5 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, B1_DLL_ARPI5_RG_ARDLL_DIV_MCTL_B1);
#endif

	io_32_write_fld_multi(DDRPHY_SHU_CA_DLL0,
		p_fld(0x9, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA) |
		p_fld(0x8, SHU_CA_DLL0_RG_ARDLL_P_GAIN_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHJUMP_EN_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDIV_CA));
	io_32_write_fld_multi_all(DDRPHY_SHU_B0_DLL0,
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHDET_IN_SWAP_B0) |
		p_fld(0x7, SHU_B0_DLL0_RG_ARDLL_GAIN_B0) |
		p_fld(0x7, SHU_B0_DLL0_RG_ARDLL_IDLECNT_B0) |
		p_fld(0x8, SHU_B0_DLL0_RG_ARDLL_P_GAIN_B0) |
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHJUMP_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHDIV_B0));
	io_32_write_fld_multi_all(DDRPHY_SHU_B1_DLL0,
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHDET_IN_SWAP_B1) |
		p_fld(0x7, SHU_B1_DLL0_RG_ARDLL_GAIN_B1) |
		p_fld(0x7, SHU_B1_DLL0_RG_ARDLL_IDLECNT_B1) |
		p_fld(0x8, SHU_B1_DLL0_RG_ARDLL_P_GAIN_B1) |
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHJUMP_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHDIV_B1));
	io_32_write_fld_multi_all(DDRPHY_SHU_B2_DLL0,
		p_fld(SET_FLD, SHU_B2_DLL0_RG_ARDLL_PHDET_IN_SWAP_B2) |
		p_fld(0x7, SHU_B2_DLL0_RG_ARDLL_GAIN_B2) |
		p_fld(0x7, SHU_B2_DLL0_RG_ARDLL_IDLECNT_B2) |
		p_fld(0x8, SHU_B2_DLL0_RG_ARDLL_P_GAIN_B2) |
		p_fld(SET_FLD, SHU_B2_DLL0_RG_ARDLL_PHJUMP_EN_B2) |
		p_fld(SET_FLD, SHU_B2_DLL0_RG_ARDLL_PHDIV_B2));

	io_32_write_fld_multi(DDRPHY_SHU_CA_DLL0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDET_OUT_SEL_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDET_IN_SWAP_CA) |
		p_fld(0x7, SHU_CA_DLL0_RG_ARDLL_GAIN_CA) |
		p_fld(0x7, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA) |
		p_fld(0x8, SHU_CA_DLL0_RG_ARDLL_P_GAIN_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHJUMP_EN_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDIV_CA));
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_B0_DLL0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHDET_IN_SWAP_B0) |
		p_fld(0x7, SHU_B0_DLL0_RG_ARDLL_GAIN_B0) |
		p_fld(0x7, SHU_B0_DLL0_RG_ARDLL_IDLECNT_B0) |
		p_fld(0x8, SHU_B0_DLL0_RG_ARDLL_P_GAIN_B0) |
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHJUMP_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHDIV_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DLL0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHDET_IN_SWAP_B1) |
		p_fld(0x7, SHU_B1_DLL0_RG_ARDLL_GAIN_B1) |
		p_fld(0x7, SHU_B1_DLL0_RG_ARDLL_IDLECNT_B1) |
		p_fld(0x8, SHU_B1_DLL0_RG_ARDLL_P_GAIN_B1) |
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHJUMP_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHDIV_B1));
#endif

	io_32_write_fld_multi_all(DDRPHY_CA_CMD8,
		p_fld(SET_FLD, CA_CMD8_RG_TX_RRESETB_DDR3_SEL) |
		p_fld(0xa, CA_CMD8_RG_RRESETB_DRVN) |
		p_fld(0xa, CA_CMD8_RG_RRESETB_DRVP));
	io_32_write_fld_multi_all(DDRPHY_CA_TX_MCK,
		p_fld(CLEAR_FLD, CA_TX_MCK_R_DMRESET_FRPHY_OPT) |
		p_fld(0xa, CA_TX_MCK_R_DMRESETB_DRVP_FRPHY) |
		p_fld(0xa, CA_TX_MCK_R_DMRESETB_DRVN_FRPHY));
#if 0
	io_32_write_fld_multi(DDRPHY_CA_CMD8 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD8_RG_TX_RRESETB_DDR3_SEL) |
		p_fld(SET_FLD, CA_CMD8_RG_RRESETB_DRVN) |
		p_fld(SET_FLD, CA_CMD8_RG_RRESETB_DRVP));
#endif
	delay_us(1);

	io_32_write_fld_align_all(DDRPHY_SHU_PLL0, 0x3,
		SHU_PLL0_RG_RPHYPLL_TOP_REV);
	/*
	 * io_32_write_fld_align(DDRPHY_SHU2_PLL0, 0x3, SHU2_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU3_PLL0, 0x3, SHU3_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU4_PLL0, 0x3, SHU4_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL0+SHIFT_TO_CHB_ADDR, 0x3, SHU_PLL0_RG_RPHYPLL_TOP_REV); //Use _All() instead
	 * io_32_write_fld_align(DDRPHY_SHU2_PLL0+SHIFT_TO_CHB_ADDR, 0x3, SHU2_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU3_PLL0+SHIFT_TO_CHB_ADDR, 0x3, SHU3_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU4_PLL0+SHIFT_TO_CHB_ADDR, 0x3, SHU4_PLL0_RG_RPHYPLL_TOP_REV);
	 */
	io_32_write_fld_multi_all(DDRPHY_SHU_CA_CMD0,
		p_fld(CLEAR_FLD, SHU_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
		p_fld(0x7, SHU_CA_CMD0_RG_TX_ARCLK_DRVN_PRE) |
		p_fld(CLEAR_FLD, SHU_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU2_CA_CMD0,
		p_fld(SET_FLD, SHU2_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU2_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU2_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 * io_32_write_fld_multi(DDRPHY_SHU3_CA_CMD0,
		p_fld(SET_FLD, SHU3_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU3_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU3_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 * io_32_write_fld_multi(DDRPHY_SHU4_CA_CMD0,
		p_fld(SET_FLD, SHU4_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU4_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU4_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 */
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
		p_fld(0x7, SHU_CA_CMD0_RG_TX_ARCLK_DRVN_PRE) |
		p_fld(SET_FLD, SHU_CA_CMD0_RG_TX_ARCLK_PRE_EN));
#endif
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU2_CA_CMD0+SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU2_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU2_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU2_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 * io_32_write_fld_multi(DDRPHY_SHU3_CA_CMD0+SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU3_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU3_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU3_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 * io_32_write_fld_multi(DDRPHY_SHU4_CA_CMD0+SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU4_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU4_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU4_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 */
#if 0
	io_32_write_fld_multi_all(DDRPHY_MISC_SHU_OPT,
		p_fld(SET_FLD, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_CA_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB1_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB0_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN));
	io_32_write_fld_multi(DDRPHY_MISC_SHU_OPT + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_CA_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB1_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB0_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN));
#endif

	delay_us(1);

	io_32_write_fld_align_all(DDRPHY_PLL3, CLEAR_FLD,
		PLL3_RG_RPHYPLL_TSTOP_EN);
	io_32_write_fld_align_all(DDRPHY_PLL3, SET_FLD,
		PLL3_RG_RPHYPLL_TST_EN);

	io_32_write_fld_multi_all(DDRPHY_SHU_PLL9,
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_MONCK_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_MONVC_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_LVROD_EN) |
		p_fld(SET_FLD, SHU_PLL9_RG_RPHYPLL_RST_DLY));
	io_32_write_fld_multi_all(DDRPHY_SHU_PLL11,
		p_fld(CLEAR_FLD, SHU_PLL11_RG_RCLRPLL_MONCK_EN) |
		p_fld(CLEAR_FLD, SHU_PLL11_RG_RCLRPLL_MONVC_EN) |
		p_fld(CLEAR_FLD, SHU_PLL11_RG_RCLRPLL_LVROD_EN) |
		p_fld(SET_FLD, SHU_PLL11_RG_RCLRPLL_RST_DLY));
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_PLL9 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_MONCK_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_MONVC_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_LVROD_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_RST_DLY));
#endif

	io_32_write_fld_align(DDRPHY_SHU_PLL5, SET_FLD,
		SHU_PLL5_RG_RPHYPLL_SDM_FRA_EN);
	io_32_write_fld_align(DDRPHY_SHU_PLL7, SET_FLD,
		SHU_PLL7_RG_RCLRPLL_SDM_FRA_EN);
	io_32_write_fld_align(DDRPHY_SHU_PLL5 + SHIFT_TO_CHB_ADDR, SET_FLD,
		SHU_PLL5_RG_RPHYPLL_SDM_FRA_EN);
	io_32_write_fld_align(DDRPHY_SHU_PLL7 + SHIFT_TO_CHB_ADDR, SET_FLD,
		SHU_PLL7_RG_RCLRPLL_SDM_FRA_EN);

	/*
	 * io_32_write_fld_align_all(DDRPHY_SHU_PLL4, 0xfe, SHU_PLL4_RG_RPHYPLL_RESERVED);
	 * io_32_write_fld_align_all(DDRPHY_SHU_PLL6, 0xfe, SHU_PLL6_RG_RCLRPLL_RESERVED);
	 */
#if 0
	io_32_write_fld_align(DDRPHY_SHU_PLL4 + SHIFT_TO_CHB_ADDR, 0xfe,
		SHU_PLL4_RG_RPHYPLL_RESERVED);
	io_32_write_fld_align(DDRPHY_SHU_PLL6 + SHIFT_TO_CHB_ADDR, 0xfe,
		SHU_PLL6_RG_RCLRPLL_RESERVED);
#endif

	delay_us(1);

	io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL1, SET_FLD,
		SHU_B0_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B0);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL1, SET_FLD,
		SHU_B1_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B1);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL1, SET_FLD,
		SHU_B2_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B2);
	io_32_write_fld_align(DDRPHY_SHU_CA_DLL1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, SHU_CA_DLL1_RG_ARDLL_FASTPJ_CK_SEL_CA);
#if 0
	io_32_write_fld_align(DDRPHY_SHU_B0_DLL1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, SHU_B0_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DLL1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, SHU_B1_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B1);
#endif
	delay_us(1);

	/*
	 * io_32_write_fld_align(DDRPHY_PLL2, CLEAR_FLD, PLL2_RG_RCLRPLL_EN);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL4, 0xff, SHU_PLL4_RG_RPHYPLL_RESERVED);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL6, 0xff, SHU_PLL6_RG_RCLRPLL_RESERVED);
	 */
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0 + SHIFT_TO_CHB_ADDR,
		0x0, MISC_CG_CTRL0_CLK_MEM_DFS_CFG);
	io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL6,
		0x1, MISC_CG_CTRL6_FMEM_CK_CG_PINMUX);
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, 0xf,
		MISC_CG_CTRL0_CLK_MEM_DFS_CFG);
	io_32_write_fld_align_all(DDRPHY_MISC_CG_CTRL0, SET_FLD,
		MISC_CG_CTRL0_RG_CG_EMI_OFF_DISABLE);

	delay_us(1);

	ddrphy_rsv_rg_setting(p);
	ddrphy_pll_setting(p);

#if ENABLE_TMRRI_NEW_MODE

	io_32_write_fld_align(DRAMC_REG_RKCFG, 0, RKCFG_TXRANK);
	io_32_write_fld_align(DRAMC_REG_RKCFG, 1, RKCFG_TXRANKFIX);
	io_32_write_fld_multi(DRAMC_REG_DRSCTRL,
		p_fld(3, DRSCTRL_DRS_CNTX) |
		p_fld(CLEAR_FLD, DRSCTRL_RK_SCINPUT_OPT));
	io_32_write_fld_multi(DRAMC_REG_DRAMCTRL,
		p_fld(CLEAR_FLD, DRAMCTRL_MRRIOPT) |
		p_fld(CLEAR_FLD, DRAMCTRL_TMRRIBYRK_DIS) |
		p_fld(SET_FLD, DRAMCTRL_TMRRICHKDIS));
#if NON_EXIST_RG
	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, SET_FLD,
		SPCMDCTRL_SC_PG_UPD_OPT);
	io_32_write_fld_multi(DRAMC_REG_SPCMDCTRL,
		p_fld(CLEAR_FLD, SPCMDCTRL_SC_PG_MPRW_DIS) |
		p_fld(CLEAR_FLD, SPCMDCTRL_SC_PG_STCMD_AREF_DIS) |
		p_fld(CLEAR_FLD, SPCMDCTRL_SC_PG_OPT2_DIS) |
		p_fld(CLEAR_FLD, SPCMDCTRL_SC_PG_MAN_DIS));
#endif
	io_32_write_fld_align(DRAMC_REG_RKCFG, 0, RKCFG_TXRANK);
	io_32_write_fld_align(DRAMC_REG_RKCFG, 0, RKCFG_TXRANKFIX);
#else
	io_32_write_fld_align(DRAMC_REG_DRSCTRL, SET_FLD,
		DRSCTRL_RK_SCINPUT_OPT);
	io_32_write_fld_multi(DRAMC_REG_DRAMCTRL,
		p_fld(SET_FLD, DRAMCTRL_MRRIOPT) |
		p_fld(SET_FLD, DRAMCTRL_TMRRIBYRK_DIS) |
		p_fld(CLEAR_FLD, DRAMCTRL_TMRRICHKDIS));
#if NON_EXIST_RG
	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, CLEAR_FLD,
		SPCMDCTRL_SC_PG_UPD_OPT);
	io_32_write_fld_multi(DRAMC_REG_SPCMDCTRL,
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_MPRW_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_STCMD_AREF_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_OPT2_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_MAN_DIS));
#endif
#endif

	io_32_write_fld_multi(DRAMC_REG_MPC_OPTION,
		p_fld(CLEAR_FLD, MPC_OPTION_ZQ_BLOCKALE_OPT) |
		p_fld(CLEAR_FLD, MPC_OPTION_MPC_BLOCKALE_OPT2) |
		p_fld(CLEAR_FLD, MPC_OPTION_MPC_BLOCKALE_OPT1) |
		p_fld(CLEAR_FLD, MPC_OPTION_MPC_BLOCKALE_OPT));

	io_32_write_fld_align(DRAMC_REG_CKECTRL, CLEAR_FLD,
		CKECTRL_RUNTIMEMRRCKEFIX);
	io_32_write_fld_align(DRAMC_REG_CKECTRL, CLEAR_FLD,
		CKECTRL_RUNTIMEMRRMIODIS);

	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B2);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, SET_FLD,
		CA_CMD10_RG_RX_ARCLK_STBEN_RESETB);
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B2_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B2);
	io_32_write_fld_multi(DDRPHY_MISC_CTRL1,
		p_fld(SET_FLD, MISC_CTRL1_R_DMDA_RRESETB_E) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMDQSIENCG_EN) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMARPIDQ_SW) |
		p_fld(SET_FLD, MISC_CTRL1_R_DM_TX_ARCMD_OE) |
		p_fld(SET_FLD, MISC_CTRL1_R_DM_TX_ARCLK_OE));
	io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ7, SET_FLD,
		SHU_B0_DQ7_R_DMRDSEL_DIV2_OPT);
	io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ7, SET_FLD,
		SHU_B0_DQ7_MIDPI_DIV4_ENABLE);
	io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ7, CLEAR_FLD,
		SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ7, CLEAR_FLD,
		SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ7, CLEAR_FLD,
		SHU_B2_DQ7_R_DMRXTRACK_DQM_EN_B2);
	io_32_write_fld_align_all(DDRPHY_B0_RXDVS0, CLEAR_FLD,
		B0_RXDVS0_R_HWSAVE_MODE_ENA_B0);
	io_32_write_fld_align_all(DDRPHY_B1_RXDVS0, CLEAR_FLD,
		B1_RXDVS0_R_HWSAVE_MODE_ENA_B1);
	io_32_write_fld_align_all(DDRPHY_B2_RXDVS0, CLEAR_FLD,
		B2_RXDVS0_R_HWSAVE_MODE_ENA_B2);
	io_32_write_fld_align_all(DDRPHY_CA_RXDVS0, CLEAR_FLD,
		CA_RXDVS0_R_HWSAVE_MODE_ENA_CA);
	io_32_write_fld_multi(DDRPHY_MISC_CTRL1 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, MISC_CTRL1_R_DMDA_RRESETB_E) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMDQSIENCG_EN) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMARPIDQ_SW) |
		p_fld(SET_FLD, MISC_CTRL1_R_DM_TX_ARCMD_OE) |
		p_fld(SET_FLD, MISC_CTRL1_R_DM_TX_ARCLK_OE));
#if 0
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1);
	io_32_write_fld_align(DDRPHY_B0_RXDVS0 + SHIFT_TO_CHB_ADDR, 1,
		B0_RXDVS0_R_HWSAVE_MODE_ENA_B0);
	io_32_write_fld_align(DDRPHY_B1_RXDVS0 + SHIFT_TO_CHB_ADDR, 1,
		B1_RXDVS0_R_HWSAVE_MODE_ENA_B1);
	io_32_write_fld_align(DDRPHY_CA_RXDVS0 + SHIFT_TO_CHB_ADDR, 1,
		CA_RXDVS0_R_HWSAVE_MODE_ENA_CA);
#endif
	io_32_write_fld_multi_all(DDRPHY_CA_CMD8,
		p_fld(CLEAR_FLD, CA_CMD8_RG_TX_RRESETB_PULL_DN) |
		p_fld(CLEAR_FLD, CA_CMD8_RG_TX_RRESETB_DDR3_SEL));

	io_32_write_fld_align(DDRPHY_CA_CMD7, CLEAR_FLD,
		CA_CMD7_RG_TX_ARCMD_PULL_DN);
	io_32_write_fld_align(DDRPHY_B0_DQ7, CLEAR_FLD,
		B0_DQ7_RG_TX_ARDQ_PULL_DN_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ7, CLEAR_FLD,
		B1_DQ7_RG_TX_ARDQ_PULL_DN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ7, CLEAR_FLD,
		B2_DQ7_RG_TX_ARDQ_PULL_DN_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD7 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		CA_CMD7_RG_TX_ARCMD_PULL_DN);
	io_32_write_fld_align(DDRPHY_B0_DQ7 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		B0_DQ7_RG_TX_ARDQ_PULL_DN_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ7 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		B1_DQ7_RG_TX_ARDQ_PULL_DN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ7 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		B2_DQ7_RG_TX_ARDQ_PULL_DN_B2);
	io_32_write_fld_multi(DRAMC_REG_SHU_DQSG_RETRY,
		p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_RDY_SEL_DLE) |
		p_fld(0x0, SHU_DQSG_RETRY_R_DQSIENLAT) |
		p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_RETRY_ONCE));
	io_32_write_fld_multi(DRAMC_REG_SHUCTRL2,
		p_fld(CLEAR_FLD, SHUCTRL2_HWSET_WLRL) |
		p_fld(SET_FLD, SHUCTRL2_SHU_PERIOD_GO_ZERO_CNT) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_OPTION) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_PARK_N) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_DLL_CHA) |
		p_fld(0xa, SHUCTRL2_R_DLL_IDLE));
	io_32_write_fld_multi(DRAMC_REG_SHUCTRL2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHUCTRL2_HWSET_WLRL) |
		p_fld(SET_FLD, SHUCTRL2_SHU_PERIOD_GO_ZERO_CNT) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_OPTION) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_PARK_N) |
		p_fld(0xa, SHUCTRL2_R_DLL_IDLE));
	io_32_write_fld_align(DRAMC_REG_DVFSDLL, SET_FLD,
		DVFSDLL_DLL_LOCK_SHU_EN);
	io_32_write_fld_multi(DRAMC_REG_DDRCONF0,
		p_fld(SET_FLD, DDRCONF0_DDR3EN) |
#if NON_EXIST_RG
		p_fld(SET_FLD, DDRCONF0_DM64BITEN) |
#endif
		p_fld(SET_FLD, DDRCONF0_DDR34_2TCMDEN) |
		p_fld(SET_FLD, DDRCONF0_BC4OTF_OPT) |
		p_fld(CLEAR_FLD, DDRCONF0_BC4OTF) |
		p_fld(SET_FLD, DDRCONF0_BK8EN));

	io_32_write_fld_multi(DRAMC_REG_STBCAL2,
		p_fld(SET_FLD, STBCAL2_STB_GERR_B23) |
		p_fld(SET_FLD, STBCAL2_STB_GERR_B01) |
		p_fld(SET_FLD, STBCAL2_STB_GERRSTOP));
	io_32_write_fld_align(DRAMC_REG_STBCAL2, SET_FLD,
		STBCAL2_STB_GERR_RST);
	io_32_write_fld_align(DRAMC_REG_STBCAL2, CLEAR_FLD,
		STBCAL2_STB_GERR_RST);
	io_32_write_fld_align(DRAMC_REG_SHU_WODT, CLEAR_FLD,
		SHU_WODT_WPST1P5T);
	io_32_write_fld_align(DRAMC_REG_CLKAR, CLEAR_FLD, CLKAR_PSELAR);
	io_32_write_fld_multi(DDRPHY_MISC_CTRL0,
		p_fld(SET_FLD, MISC_CTRL0_R_DQS0IEN_DIV4_CK_CG_CTRL) |
		p_fld(SET_FLD, MISC_CTRL0_R_DQS1IEN_DIV4_CK_CG_CTRL) |
		p_fld(CLEAR_FLD, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
		p_fld(CLEAR_FLD, MISC_CTRL0_R_DMSTBEN_OUTSEL) |
		p_fld(CLEAR_FLD, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));
	io_32_write_fld_multi(DDRPHY_MISC_CTRL0 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, MISC_CTRL0_R_CLKIEN_DIV4_CK_CG_CTRL) |
		p_fld(SET_FLD, MISC_CTRL0_R_DQS0IEN_DIV4_CK_CG_CTRL) |
		p_fld(SET_FLD, MISC_CTRL0_R_DQS1IEN_DIV4_CK_CG_CTRL) |
		p_fld(CLEAR_FLD, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
		p_fld(CLEAR_FLD, MISC_CTRL0_R_DMSTBEN_OUTSEL) |
		p_fld(CLEAR_FLD, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));
	io_32_write_fld_multi(DRAMC_REG_SHU_CONF0,
		p_fld(0x2, SHU_CONF0_MATYPE) |
		p_fld(SET_FLD, SHU_CONF0_BL4) |
		p_fld(SET_FLD, SHU_CONF0_FDIV2) |
		p_fld(CLEAR_FLD, SHU_CONF0_DUALSCHEN) |
		p_fld(SET_FLD, SHU_CONF0_DM64BITEN) |
		p_fld(SET_FLD, SHU_CONF0_WDATRGO) |
		p_fld(SET_FLD, SHU_CONF0_REFTHD) |
		p_fld(SET_FLD, SHU_CONF0_RDATDIV2) |
		p_fld(CLEAR_FLD, SHU_CONF0_RDATDIV4) |
		p_fld(SET_FLD, SHU_CONF0_ADVPREEN) |
		p_fld(0x3f, SHU_CONF0_DMPGTIM));
	io_32_write_fld_multi(DRAMC_REG_SHU_ODTCTRL,
		p_fld(0x3, SHU_ODTCTRL_TWODT) |
		p_fld(SET_FLD, SHU_ODTCTRL_WOEN));
	io_32_write_fld_align(DRAMC_REG_REFCTRL0, 0x5,
		REFCTRL0_REF_PREGATE_CNT);
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA1,
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CS1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_RAS) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CAS) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_WE) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_RESET) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_ODT) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CKE) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CS));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA2,
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_CKE1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_CMD) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_BA2) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_BA1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_BA0));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA3,
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA7) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA6) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA5) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA4) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA3) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA2) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA0));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA4,
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA15) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA14) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA13) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA12) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA11) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA10) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA9) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA8));
	io_32_write_fld_align(DRAMC_REG_SHU_SELPH_CA5, CLEAR_FLD,
		SHU_SELPH_CA5_DLY_ODT);
/*
	io32_write_4b_all(DRAMC_REG_SHU_SELPH_CA5, 0x11111011);
	io32_write_4b_all(DRAMC_REG_SHU_SELPH_CA6, 0x01000111);
	io32_write_4b_all(DRAMC_REG_SHU_SELPH_CA7, 0x11111111);
	io32_write_4b_all(DRAMC_REG_SHU_SELPH_CA8, 0x11111111);
*/
	/* move CK to CA center */
	io32_write_4b_all(DRAMC_REG_SHU_SELPH_CA5, 0x10001111);
	io32_write_4b_all(DRAMC_REG_SHU_SELPH_CA6, 0x01000000);
	io32_write_4b_all(DRAMC_REG_SHU_SELPH_CA7, 0x00000000);
	io32_write_4b_all(DRAMC_REG_SHU_SELPH_CA8, 0x00000000);

	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_DQS0,
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS3) |
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS2) |
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS1) |
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS0) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS3) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS2) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS1) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS0));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_DQS1,
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS3) |
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS2) |
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS1) |
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS0) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS3) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS2) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS1) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ0,
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ3) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ2) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ1) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ1,
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM3) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM2) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM1) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ2,
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ2_DLY_OEN_DQ3) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ2_DLY_OEN_DQ2) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ2_DLY_OEN_DQ1) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ2_DLY_OEN_DQ0) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ2_DLY_DQ3) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ2_DLY_DQ2) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ2_DLY_DQ1) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ2_DLY_DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ3,
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ3_DLY_OEN_DQM3) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ3_DLY_OEN_DQM2) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ3_DLY_OEN_DQM1) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ3_DLY_OEN_DQM0) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ3_DLY_DQM3) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ3_DLY_DQM2) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ3_DLY_DQM1) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ3_DLY_DQM0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ0,
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3) |
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2) |
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1) |
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ1,
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3) |
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2) |
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1) |
		p_fld(reg_txdly_dqdqm_oen, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1) |
		p_fld(reg_txdly_dqdqm, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ2,
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3) |
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2) |
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1) |
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ2_DLY_R1DQ3) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ2_DLY_R1DQ2) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ2_DLY_R1DQ1) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ2_DLY_R1DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ3,
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3) |
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2) |
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1) |
		p_fld(reg_dly_dqdqm_oen, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ3_DLY_R1DQM3) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ3_DLY_R1DQM2) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ3_DLY_R1DQM1) |
		p_fld(reg_dly_dqdqm, SHURK1_SELPH_DQ3_DLY_R1DQM0));
	delay_us(1);

	io_32_write_fld_multi(DRAMC_REG_STBCAL,
		p_fld(CLEAR_FLD, STBCAL_DQSIENMODE_SELPH) |
		p_fld(SET_FLD, STBCAL_STB_DQIEN_IG) |
		p_fld(SET_FLD, STBCAL_PICHGBLOCK_NORD) |
		p_fld(SET_FLD, STBCAL_PIMASK_RKCHG_OPT));
	io_32_write_fld_multi(DRAMC_REG_SHU_DQSG,
		p_fld(0x9, SHU_DQSG_STB_UPDMASKCYC) |
		p_fld(SET_FLD, SHU_DQSG_STB_UPDMASK_EN));
	io_32_write_fld_align(DRAMC_REG_STBCAL, SET_FLD, STBCAL_DQSIENMODE);
	io_32_write_fld_multi(DRAMC_REG_SREFCTRL,
		p_fld(CLEAR_FLD, SREFCTRL_SREF_HW_EN) |
		p_fld(0x8, SREFCTRL_SREFDLY) |
		p_fld(SET_FLD, SREFCTRL_SREF2_OPTION));
	io_32_write_fld_multi(DRAMC_REG_SHU_PIPE,
		p_fld(SET_FLD, SHU_PIPE_READ_START_EXTEND1) |
		p_fld(SET_FLD, SHU_PIPE_DLE_LAST_EXTEND1));

	io_32_write_fld_multi(DRAMC_REG_CKECTRL,
		p_fld(0x3, CKECTRL_CKELCKCNT) |
		p_fld(SET_FLD, CKECTRL_CKEON));
#if NON_EXIST_RG
	io_32_write_fld_multi(DRAMC_REG_CKECTRL,
		p_fld(SET_FLD, CKECTRL_CKEON) |
		p_fld(SET_FLD, CKECTRL_CKETIMER_SEL));
	io_32_write_fld_align(DRAMC_REG_RKCFG, SET_FLD,
		RKCFG_CKE2RANK_OPT2);
#endif
	io_32_write_fld_align(DRAMC_REG_CKECTRL, SET_FLD,
		CKECTRL_CKETIMER_SEL);
	io_32_write_fld_align(DRAMC_REG_SHU_CONF2, 0x7,
		SHU_CONF2_DCMDLYREF);

	io_32_write_fld_multi(DRAMC_REG_SHU_SCINTV,
		p_fld(CLEAR_FLD, SHU_SCINTV_DQS2DQ_SHU_PITHRD) |
		p_fld(CLEAR_FLD, SHU_SCINTV_DQS2DQ_FILT_PITHRD) |
		p_fld(SET_FLD, SHU_SCINTV_RDDQC_INTV) |
		p_fld(CLEAR_FLD, SHU_SCINTV_TZQLAT));
#if 0
	io_32_write_fld_multi(DRAMC_REG_SHU_CONF1,
		p_fld(0xc, SHU_CONF1_DATLAT_DSEL_PHY) |
		p_fld(0xc, SHU_CONF1_DATLAT_DSEL) |
		p_fld(0xe, SHU_CONF1_DATLAT));
#endif
	io_32_write_fld_align(DRAMC_REG_SHUCTRL, SET_FLD,
		SHUCTRL_LPSM_BYPASS_B);
	io_32_write_fld_align(DRAMC_REG_REFCTRL1, CLEAR_FLD,
		REFCTRL1_SREF_PRD_OPT);
	/*
	 * io_32_write_fld_align(DDRPHY_SHU_PLL4, 0xfe, SHU_PLL4_RG_RPHYPLL_RESERVED);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL6, 0xfe, SHU_PLL6_RG_RCLRPLL_RESERVED);
	 */
	io_32_write_fld_multi_all(DDRPHY_SHU_PLL4,
		p_fld(0x1, SHU_PLL4_RG_RPHYPLL_BP) |
		p_fld(0x1, SHU_PLL4_RG_RPHYPLL_BR) |
		p_fld(0x1, SHU_PLL4_RG_RPHYPLL_BLP) |
		p_fld(0x1, SHU_PLL4_RG_RPHYPLL_IBIAS) |
		p_fld(0x1, SHU_PLL4_RG_RPHYPLL_ICHP) |
		p_fld(0x7, SHU_PLL4_RG_RPHYPLL_BW) |
		p_fld(0x2, SHU_PLL4_RG_RPHYPLL_FS) |
		p_fld(0x2000, SHU_PLL4_RG_RPHYPLL_RESERVED));
	io_32_write_fld_multi_all(DDRPHY_SHU_PLL5,
		p_fld(SET_FLD, SHU_PLL5_RG_RPHYPLL_SDM_FRA_EN) |
		p_fld(SET_FLD, SHU_PLL5_RG_RPHYPLL_SDM_PCW_CHG));
	io_32_write_fld_multi_all(DDRPHY_SHU_PLL6,
		p_fld(0x1, SHU_PLL6_RG_RCLRPLL_BP) |
		p_fld(0x1, SHU_PLL6_RG_RCLRPLL_BR) |
		p_fld(0x1, SHU_PLL6_RG_RCLRPLL_BLP) |
		p_fld(0x1, SHU_PLL6_RG_RCLRPLL_IBIAS) |
		p_fld(0x1, SHU_PLL6_RG_RCLRPLL_ICHP) |
		p_fld(0x7, SHU_PLL6_RG_RCLRPLL_BW) |
		p_fld(0x2, SHU_PLL6_RG_RCLRPLL_FS) |
		p_fld(0x2000, SHU_PLL6_RG_RCLRPLL_RESERVED));
	io_32_write_fld_multi_all(DDRPHY_SHU_PLL7,
		p_fld(SET_FLD, SHU_PLL7_RG_RCLRPLL_SDM_PCW_CHG) |
		p_fld(SET_FLD, SHU_PLL7_RG_RCLRPLL_SDM_FRA_EN));

	io_32_write_fld_align(DRAMC_REG_REFRATRE_FILTER, SET_FLD,
		REFRATRE_FILTER_PB2AB_OPT);
	//io_32_write_fld_align(DDRPHY_MISC_CTRL1, SET_FLD,
		//MISC_CTRL1_R_DMDA_RRESETB_I);
	io_32_write_fld_align(DRAMC_REG_DRAMCTRL, CLEAR_FLD,
		DRAMCTRL_CLKWITRFC);
#if NON_EXIST_RG
	io_32_write_fld_multi(DRAMC_REG_MISCTL0,
		p_fld(SET_FLD, MISCTL0_REFP_ARB_EN2) |
		p_fld(SET_FLD, MISCTL0_PBC_ARB_EN) |
		p_fld(SET_FLD, MISCTL0_REFA_ARB_EN2));
#endif
	io_32_write_fld_multi(DRAMC_REG_MISCTL0,
		p_fld(SET_FLD, MISCTL0_DM32BIT_RDSEL_OPT) |
		p_fld(SET_FLD, MISCTL0_PG_WAKEUP_OPT) |
		p_fld(SET_FLD, MISCTL0_PBC_ARB_EN));

	io_32_write_fld_multi(DRAMC_REG_PERFCTL0,
		p_fld(SET_FLD, PERFCTL0_MWHPRIEN) |

		p_fld(SET_FLD, PERFCTL0_WFLUSHEN) |
		p_fld(SET_FLD, PERFCTL0_EMILLATEN) |
		p_fld(SET_FLD, PERFCTL0_RWAGEEN) |
		p_fld(SET_FLD, PERFCTL0_RWLLATEN) |
		p_fld(SET_FLD, PERFCTL0_RWHPRIEN) |
		p_fld(SET_FLD, PERFCTL0_RWOFOEN) |
		p_fld(SET_FLD, PERFCTL0_DISRDPHASE1));
#if NON_EXIST_RG
		p_fld(SET_FLD, PERFCTL0_DUALSCHEN));
#endif
	io_32_write_fld_align(DRAMC_REG_ARBCTL, 0x80, ARBCTL_MAXPENDCNT);
	io_32_write_fld_multi(DRAMC_REG_PADCTRL,
		p_fld(SET_FLD, PADCTRL_DQIENLATEBEGIN) |
		p_fld(SET_FLD, PADCTRL_DQIENQKEND));
	io_32_write_fld_align(DRAMC_REG_DRAMC_PD_CTRL, SET_FLD,
		DRAMC_PD_CTRL_DCMREF_OPT);
	io_32_write_fld_align(DRAMC_REG_CLKCTRL, SET_FLD, CLKCTRL_CLK_EN_1);
	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(0x4, REFCTRL0_DISBYREFNUM) |
		p_fld(SET_FLD, REFCTRL0_DLLFRZ));
	io_32_write_fld_multi(DRAMC_REG_SPCMDCTRL,
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_MPRW_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_OPT2_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_STCMD_AREF_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SRFMR4_CNTKEEP_B) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_MAN_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_CLR_EN));
#if 0
	io_32_write_fld_multi(DRAMC_REG_CATRAINING1,
		p_fld(0x4, CATRAINING1_CATRAIN_INTV) |
		p_fld(0x3, CATRAINING1_CATRAINLAT));
#endif
	io_32_write_fld_align(DRAMC_REG_STBCAL, SET_FLD, STBCAL_REFUICHG);
	io_32_write_fld_multi(DRAMC_REG_SHU_RANKCTL,
		p_fld(0x3, SHU_RANKCTL_RANKINCTL_PHY) |
		p_fld(0x1, SHU_RANKCTL_RANKINCTL_ROOT1) |
		p_fld(0x1, SHU_RANKCTL_RANKINCTL));
	io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, SET_FLD,
		SHU_STBCAL_DMSTBLAT);
	/*
	 * io_32_write_fld_align(DRAMC_REG_SHURK0_DQSCTL, 0x4, SHURK0_DQSCTL_DQSINCTL); //DQSINCTL: set in UpdateACTimingReg()
	 * io_32_write_fld_align(DRAMC_REG_SHURK1_DQSCTL, 0x4, SHURK1_DQSCTL_R1DQSINCTL); //DQSINCTL: set in UpdateACTimingReg()
	 */

	delay_us(2);

#if 0//cc mark LEGACY_GATING_DLY
	LegacyGatingDlyLP3(p);
#endif

	io_32_write_fld_multi(DRAMC_REG_SHU_WODT,
		p_fld(CLEAR_FLD, SHU_WODT_DISWODTE) |
		p_fld(CLEAR_FLD, SHU_WODT_WODTFIXOFF) |
		p_fld(CLEAR_FLD, SHU_WODT_DISWODT));

	io_32_write_fld_multi(DRAMC_REG_PHY_RX_INCTL,
		p_fld(CLEAR_FLD, PHY_RX_INCTL_RX_IN_GATE_EN_OPT) |
		p_fld(SET_FLD, PHY_RX_INCTL_RX_IN_GATE_EN_4BYTE_EN) |
		p_fld(CLEAR_FLD, PHY_RX_INCTL_RX_IN_BUFF_EN_OPT) |
		p_fld(SET_FLD, PHY_RX_INCTL_RX_IN_BUFF_EN_4BYTE_EN));
	/*
	 * io_32_write_fld_align(DRAMC_REG_SHU_CONF3, 0x5, SHU_CONF3_ZQCSCNT);
	 * io_32_write_fld_align(DRAMC_REG_SHU_CONF3, 0xff, SHU_CONF3_REFRCNT);
	 * io_32_write_fld_align(DRAMC_REG_SHU_CONF1, 0xb0, SHU_CONF1_REFBW_FR);
	 */

	io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, SET_FLD,
		SHU_STBCAL_DQSG_MODE);
	io_32_write_fld_multi(DRAMC_REG_SHU_CONF0,
		p_fld(0x5, SHU_CONF0_REFTHD) |
		p_fld(0x15, SHU_CONF0_DMPGTIM));
	/*
	 * io_32_write_fld_align(DRAMC_REG_SHU_CONF1, 0xae, SHU_CONF1_REFBW_FR);
	 * io_32_write_fld_multi(DRAMC_REG_SHU_CONF3,
		p_fld(0x8d, SHU_CONF3_REFRCNT) |
	 * | p_fld(0x1f8, SHU_CONF3_ZQCSCNT));
	 */
	io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, SET_FLD,
		SHU_STBCAL_PICGLAT);

	{

		io_32_write_fld_multi(DRAMC_REG_SHU_CONF0,
			p_fld(SET_FLD, SHU_CONF0_REFTHD) |
			p_fld(0x3f, SHU_CONF0_DMPGTIM));
		io_32_write_fld_multi(DRAMC_REG_SHU_CONF1,
			p_fld(SET_FLD, SHU_CONF1_TREFBWIG) |
			p_fld(CLEAR_FLD, SHU_CONF1_REFBW_FR));
		io_32_write_fld_multi(DRAMC_REG_SHU_CONF3,
			p_fld(0xff, SHU_CONF3_REFRCNT) |
			p_fld(0x5, SHU_CONF3_ZQCSCNT));

#if NON_EXIST_RG
		io_32_write_fld_align(DRAMC_REG_SHU_WODT, CLEAR_FLD,
			SHU_WODT_TWPSTEXT);
#endif
		io_32_write_fld_align(DRAMC_REG_SHU_DQSG, 0xf, SHU_DQSG_SCINTV);
		/*
		 * io_32_write_fld_multi(DRAMC_REG_SHU_HWSET_MR2,
			p_fld(CLEAR_FLD, SHU_HWSET_MR2_HWSET_MR2_OP) |
		 * | p_fld(CLEAR_FLD, SHU_HWSET_MR2_HWSET_MR2_MRSMA));
		 * io_32_write_fld_multi(DRAMC_REG_SHU_HWSET_MR13,
			p_fld(0x1c, SHU_HWSET_MR13_HWSET_MR13_OP) |
		 * | p_fld(0x2, SHU_HWSET_MR13_HWSET_MR13_MRSMA));
		 * io_32_write_fld_multi(DRAMC_REG_SHU_HWSET_VRCG,
			p_fld(CLEAR_FLD, SHU_HWSET_VRCG_HWSET_VRCG_OP) |
		 * | p_fld(CLEAR_FLD, SHU_HWSET_VRCG_HWSET_VRCG_MRSMA));
		 */
		io_32_write_fld_multi(DRAMC_REG_SHU_DQSG_RETRY,
			p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_RETRY_USE_BURST_MDOE) |
			p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_RETRY_ROUND_NUM) |
	#ifdef FIRST_BRING_UP
			p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_XSR_RETRY_SPM_MODE) |
	#else
			p_fld(SET_FLD, SHU_DQSG_RETRY_R_XSR_RETRY_SPM_MODE) |
	#endif
			p_fld(0x0, SHU_DQSG_RETRY_R_DQSIENLAT) |
			p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_DM4BYTE));

		io_32_write_fld_align_all(DDRPHY_SHU_B0_DQ5, CLEAR_FLD,
			SHU_B0_DQ5_RG_ARPI_FB_B0);
		io_32_write_fld_align_all(DDRPHY_SHU_B1_DQ5, CLEAR_FLD,
			SHU_B1_DQ5_RG_ARPI_FB_B1);
		io_32_write_fld_align_all(DDRPHY_SHU_B2_DQ5, CLEAR_FLD,
			SHU_B2_DQ5_RG_ARPI_FB_B2);
		io_32_write_fld_align_all(DDRPHY_SHU_CA_CMD5, CLEAR_FLD,
			SHU_CA_CMD5_RG_ARPI_FB_CA);
		/*
		 * 93918 ===ddrphy_sh_lp3_1866_cha end===
		 * 93918 ===ddrphy_sh_lp3_1866_chb begin===
		 */
	#if 0
		io_32_write_fld_align(DDRPHY_SHU_B0_DQ5 + SHIFT_TO_CHB_ADDR, 0x4,
			SHU_B0_DQ5_RG_ARPI_FB_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ5 + SHIFT_TO_CHB_ADDR, 0x4,
			SHU_B1_DQ5_RG_ARPI_FB_B1);
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD5 + SHIFT_TO_CHB_ADDR, 0x4,
			SHU_CA_CMD5_RG_ARPI_FB_CA);
	#endif

		io_32_write_fld_multi(DDRPHY_SHU_R0_B0_DQ2 + SHIFT_TO_CHB_ADDR,
			p_fld(0xa, SHU_R0_B0_DQ2_RK0_RX_ARDQ1_F_DLY_B0) |
			p_fld(0xa, SHU_R0_B0_DQ2_RK0_RX_ARDQ0_F_DLY_B0));

	}
	/*
	 * else
	 * TODO: overwrite 1866 setting END
	 */

	/*
	 * Since the above initial settings are for LP3_DDR1866, TX related initial delay settings needs to be adjusted
	 * according it's Write Latency difference
	 */
	if (p->freqGroup != DDR1866_FREQ) {
		//cc mark TxCalibrationMoveDQSByDifferentWL(p);
	}

	io_32_write_fld_align(DRAMC_REG_RKCFG, SET_FLD, RKCFG_RKMODE);

	update_initial_setting_ddr3(p);

#if SIMULATION_SW_IMPED
	dramc_sw_impedance_save_register(p, ODT_OFF, ODT_OFF,
		DRAM_DFS_SHUFFLE_1);
#endif

	dramc_mr_init_ddr3(p);

#if 0
	io_32_write_fld_multi(DRAMC_REG_MRS,
		p_fld(CLEAR_FLD, MRS_MRSRK) |
		p_fld(0x4, MRS_MRSMA) |
		p_fld(CLEAR_FLD, MRS_MRSOP));

	delay_us(1);

	io_32_write_fld_align(DRAMC_REG_SPCMD, SET_FLD, SPCMD_MRREN);

	delay_us(1);
#endif

	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(CLEAR_FLD, REFCTRL0_PBREFEN) |
		p_fld(SET_FLD, REFCTRL0_PBREF_DISBYRATE));
	io_32_write_fld_align(DRAMC_REG_RKCFG, CLEAR_FLD, RKCFG_DQSOSC2RK);
	/*
	 * io_32_write_fld_align(DRAMC_REG_CKECTRL, CLEAR_FLD, CKECTRL_CKEFIXON);
	 * io_32_write_fld_align(DRAMC_REG_HW_MRR_FUN, SET_FLD, HW_MRR_FUN_TMRR_ENA);
	 */
	io_32_write_fld_align(DRAMC_REG_DRAMCTRL, SET_FLD,
		DRAMCTRL_PREALL_OPTION);
	io_32_write_fld_align(DRAMC_REG_ZQCS, 0x56, ZQCS_ZQCSOP);

	delay_us(1);

	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(SET_FLD, REFCTRL0_REFFRERUN) |
		p_fld(SET_FLD, REFCTRL0_REFDIS));
	io_32_write_fld_align(DRAMC_REG_SREFCTRL, SET_FLD,
		SREFCTRL_SREF_HW_EN);
	io_32_write_fld_align(DRAMC_REG_MPC_OPTION, CLEAR_FLD,
		MPC_OPTION_MPCRKEN);
	io_32_write_fld_multi(DRAMC_REG_DRAMC_PD_CTRL,
		p_fld(SET_FLD, DRAMC_PD_CTRL_PHYCLKDYNGEN) |
		p_fld(CLEAR_FLD, DRAMC_PD_CTRL_DCMENNOTRFC) |
		p_fld(SET_FLD, DRAMC_PD_CTRL_DCMEN));
	io_32_write_fld_multi(DRAMC_REG_DRAMC_PD_CTRL + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, DRAMC_PD_CTRL_COMBCLKCTRL) |
		p_fld(CLEAR_FLD, DRAMC_PD_CTRL_DCMENNOTRFC) |
		p_fld(SET_FLD, DRAMC_PD_CTRL_DCMEN));
	io_32_write_fld_multi(DRAMC_REG_EYESCAN,
		p_fld(CLEAR_FLD, EYESCAN_RX_DQ_EYE_SEL) |
		p_fld(CLEAR_FLD, EYESCAN_RG_RX_EYE_SCAN_EN));
	io_32_write_fld_multi(DRAMC_REG_STBCAL1,
		p_fld(SET_FLD, STBCAL1_STBCNT_SHU_RST_EN) |
		p_fld(SET_FLD, STBCAL1_RKUICHG_EN) |
		p_fld(SET_FLD, STBCAL1_STBCNT_MODESEL) |
		p_fld(SET_FLD, STBCAL1_STB_SHIFT_DTCOUT_IG) |
		p_fld(SET_FLD, STBCAL1_STB_FLAGCLR) |
		p_fld(SET_FLD, STBCAL1_STBCNT_LATCH_EN) |
		p_fld(SET_FLD, STBCAL1_STBENCMPEN));
	io_32_write_fld_align(DRAMC_REG_TEST2_1, 0x10000,
		TEST2_1_TEST2_BASE);
	io_32_write_fld_align(DRAMC_REG_TEST2_2, 0x20, TEST2_2_TEST2_OFF);
	io_32_write_fld_multi(DRAMC_REG_TEST2_3,
		p_fld(SET_FLD, TEST2_3_TEST2WREN2_HW_EN) |
#if NON_EXIST_RG
		p_fld(0x4, TEST2_3_DQSICALSTP) |
#endif
		p_fld(SET_FLD, TEST2_3_TESTAUDPAT) |
		p_fld(CLEAR_FLD, TEST2_3_TESTADR_SHIFT));
	io_32_write_fld_align(DRAMC_REG_SHUCTRL2, SET_FLD,
		SHUCTRL2_MR13_SHU_EN);
	io_32_write_fld_align(DRAMC_REG_DRAMCTRL, SET_FLD,
		DRAMCTRL_REQQUE_THD_EN);

	io_32_write_fld_align(DRAMC_REG_SHU_CKECTRL, 0x3,
		SHU_CKECTRL_SREF_CK_DLY);

	io_32_write_fld_align(DRAMC_REG_DUMMY_RD, p->support_rank_num,
		DUMMY_RD_RANK_NUM);

	io_32_write_fld_align(DRAMC_REG_TEST2_4, 0x4,
		TEST2_4_TESTAGENTRKSEL);
	io_32_write_fld_multi(DRAMC_REG_REFCTRL1,
		p_fld(SET_FLD, REFCTRL1_SREF_CG_OPT) |
		p_fld(SET_FLD, REFCTRL1_REF_QUE_AUTOSAVE_EN) |
		p_fld(SET_FLD, REFCTRL1_SLEFREF_AUTOSAVE_EN));
	io_32_write_fld_multi(DRAMC_REG_DQSOSCR,
		p_fld(SET_FLD, DQSOSCR_TXUPD_ABREF_SEL) |
		p_fld(CLEAR_FLD, DQSOSCR_SREF_TXUI_RELOAD_OPT));
	io_32_write_fld_multi(DRAMC_REG_RSTMASK,
		p_fld(CLEAR_FLD, RSTMASK_GT_SYNC_MASK) |
		p_fld(CLEAR_FLD, RSTMASK_DVFS_SYNC_MASK) |
		p_fld(CLEAR_FLD, RSTMASK_GT_SYNC_MASK_FOR_PHY) |
		p_fld(CLEAR_FLD, RSTMASK_DVFS_SYNC_MASK_FOR_PHY));
	/*
	 * io_32_write_fld_align(DRAMC_REG_DRAMCTRL, CLEAR_FLD, DRAMCTRL_CTOREQ_HPRI_OPT);
	 * 91074 === DE initial sequence done ===
	 * #ifdef DVFS_Enable
	 */
	io_32_write_fld_align(DRAMC_REG_STBCAL1, 0x8,
		STBCAL1_STBCAL_FILTER);
	io_32_write_fld_multi(DRAMC_REG_STBCAL,
		p_fld(CLEAR_FLD, STBCAL_STBCALEN) |
		p_fld(CLEAR_FLD, STBCAL_STB_SELPHYCALEN) |
		p_fld(CLEAR_FLD, STBCAL_REFUICHG) |
		p_fld(SET_FLD, STBCAL_SREF_DQSGUPD));

	delay_us(1);
	io_32_write_fld_align(DRAMC_REG_DDRCONF0, CLEAR_FLD, DDRCONF0_AG0MWR);
	/*
	 * io_32_write_fld_multi(DRAMC_REG_DRAMCTRL,
		p_fld(SET_FLD, DRAMCTRL_FW2R) |
	 * | p_fld(CLEAR_FLD, DRAMCTRL_DYNMWREN)
	 * | p_fld(SET_FLD, DRAMCTRL_CLKWITRFC)
	 * | p_fld(SET_FLD, DRAMCTRL_ADRBIT3DEC)
	 * | p_fld(SET_FLD, DRAMCTRL_CTOREQ_HPRI_OPT));
	 */
#if 0
	io_32_write_fld_align(DRAMC_REG_MISCTL0, CLEAR_FLD,
		MISCTL0_PBC_ARB_EN);
	io_32_write_fld_multi(DRAMC_REG_PERFCTL0,
		p_fld(SET_FLD, PERFCTL0_REORDEREN) |
		p_fld(SET_FLD, PERFCTL0_RWHPRICTL) |
		p_fld(CLEAR_FLD, PERFCTL0_EMILLATEN) |
		p_fld(CLEAR_FLD, PERFCTL0_RWAGEEN) |
		p_fld(CLEAR_FLD, PERFCTL0_RWHPRIEN));
#endif
	io_32_write_fld_multi(DRAMC_REG_RSTMASK,
		p_fld(CLEAR_FLD, RSTMASK_PHY_SYNC_MASK) |
		p_fld(CLEAR_FLD, RSTMASK_DAT_SYNC_MASK));

#if 0
	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(SET_FLD, REFCTRL0_REFNA_OPT) |
		p_fld(0x2, REFCTRL0_DISBYREFNUM) |
		p_fld(SET_FLD, REFCTRL0_UPDBYWR));
#endif
	io_32_write_fld_multi(DRAMC_REG_REFRATRE_FILTER,
		p_fld(0x6, REFRATRE_FILTER_REFRATE_FIL7) |
		p_fld(0x5, REFRATRE_FILTER_REFRATE_FIL6) |
		p_fld(0x4, REFRATRE_FILTER_REFRATE_FIL5) |
		p_fld(0x3, REFRATRE_FILTER_REFRATE_FIL4) |
		p_fld(0x3, REFRATRE_FILTER_REFRATE_FIL3) |
		p_fld(0x3, REFRATRE_FILTER_REFRATE_FIL2) |
		p_fld(0x2, REFRATRE_FILTER_REFRATE_FIL1) |
		p_fld(0x1, REFRATRE_FILTER_REFRATE_FIL0));
	io_32_write_fld_multi(DRAMC_REG_ZQCS,
		p_fld(CLEAR_FLD, ZQCS_ZQCSDUAL) |
		p_fld(CLEAR_FLD, ZQCS_ZQCSMASK));
	io_32_write_fld_align(DRAMC_REG_HW_MRR_FUN, CLEAR_FLD,
		HW_MRR_FUN_MRR_HW_HIPRI);
	io_32_write_fld_align(DRAMC_REG_DUMMY_RD, SET_FLD,
		DUMMY_RD_SREF_DMYRD_EN);
	io_32_write_fld_align(DRAMC_REG_STBCAL1, 0x2,
		STBCAL1_STBCAL_FILTER);

	dvfs_settings(p);
}
#endif

#if SUPPORT_TYPE_PCDDR4
static DRAM_STATUS_T update_initial_setting_ddr4(DRAMC_CTX_T *p)
{
	io_32_write_fld_multi(DDRPHY_R0_B2_RXDVS2,
		p_fld(CLEAR_FLD, R0_B2_RXDVS2_R_RK0_DVS_MODE_B2) |
		p_fld(CLEAR_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(CLEAR_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B2));
	io_32_write_fld_multi(DDRPHY_R1_B2_RXDVS2,
		p_fld(CLEAR_FLD, R1_B2_RXDVS2_R_RK1_DVS_MODE_B2) |
		p_fld(CLEAR_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(CLEAR_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B2));
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS2,
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS2,
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1) |
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R0_B0_RXDVS2,
		p_fld(CLEAR_FLD, R0_B0_RXDVS2_R_RK0_DVS_MODE_B0) |
		p_fld(CLEAR_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(CLEAR_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi(DDRPHY_R1_B0_RXDVS2,
		p_fld(CLEAR_FLD, R1_B0_RXDVS2_R_RK1_DVS_MODE_B0) |
		p_fld(CLEAR_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(CLEAR_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi(DDRPHY_R0_CA_RXDVS2,
		p_fld(CLEAR_FLD, R0_CA_RXDVS2_R_RK0_DVS_MODE_CA) |
		p_fld(CLEAR_FLD, R0_CA_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_CA) |
		p_fld(CLEAR_FLD, R0_CA_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_CA));
	io_32_write_fld_multi(DDRPHY_R1_CA_RXDVS2,
		p_fld(CLEAR_FLD, R1_CA_RXDVS2_R_RK1_DVS_MODE_CA) |
		p_fld(CLEAR_FLD, R1_CA_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_CA) |
		p_fld(CLEAR_FLD, R1_CA_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_CA));

#if 0  /*modified before broadcast*/
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(CLEAR_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1) |
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(CLEAR_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
#endif

	io_32_write_fld_multi(DDRPHY_B0_DQ6,
		p_fld(SET_FLD, B0_DQ6_RG_TX_ARDQ_DDR3_SEL_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_TX_ARDQ_DDR4_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_DDR4_SEL_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_TX_ARDQ_DDR3_SEL_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_DDR3_SEL_B1) |
		p_fld(CLEAR_FLD, B1_DQ6_RG_TX_ARDQ_DDR4_SEL_B1) |
		p_fld(CLEAR_FLD, B1_DQ6_RG_RX_ARDQ_DDR4_SEL_B1));
	io_32_write_fld_multi(DDRPHY_B2_DQ6,
		p_fld(SET_FLD, B2_DQ6_RG_TX_ARDQ_DDR3_SEL_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_DDR3_SEL_B2) |
		p_fld(CLEAR_FLD, B2_DQ6_RG_TX_ARDQ_DDR4_SEL_B2) |
		p_fld(CLEAR_FLD, B2_DQ6_RG_RX_ARDQ_DDR4_SEL_B2));
	io_32_write_fld_multi(DDRPHY_CA_CMD6,
		p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_DDR3_SEL) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_DDR3_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_TX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_DDR4_SEL));

	io_32_write_fld_align(DDRPHY_B0_DQ8, CLEAR_FLD,
		B0_DQ8_RG_TX_ARDQ_EN_LP4P_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ8, CLEAR_FLD,
		B1_DQ8_RG_TX_ARDQ_EN_LP4P_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ8, CLEAR_FLD,
		B2_DQ8_RG_TX_ARDQ_EN_LP4P_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD9, CLEAR_FLD,
		CA_CMD9_RG_TX_ARCMD_EN_LP4P);

	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), CLEAR_FLD,
		B0_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B0);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B1_DQ5), CLEAR_FLD,
		B1_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ5), CLEAR_FLD,
		B2_DQ5_RG_RX_ARDQ_EYE_VREF_EN_B2);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_CA_CMD5), CLEAR_FLD,
		CA_CMD5_RG_RX_ARCMD_EYE_VREF_EN);

	io_32_write_fld_align(DDRPHY_SHU_CA_CMD0, CLEAR_FLD,
		SHU_CA_CMD0_RG_FB_CK_MUX);
	io_32_write_fld_align(DDRPHY_CA_CMD10, SET_FLD,
		CA_CMD10_RG_RX_ARCLK_DQSIENMODE);

	io_32_write_fld_multi(DDRPHY_CA_CMD8,
		p_fld(SET_FLD, CA_CMD8_RG_TX_RRESETB_DDR3_SEL) |
		p_fld(CLEAR_FLD, CA_CMD8_RG_TX_RRESETB_DDR4_SEL));

	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ5), 0xb,
		B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B1_DQ5), 0xb,
		B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ5), 0xb,
		B2_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B2);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_CA_CMD5), 0xb,
		CA_CMD5_RG_RX_ARCMD_EYE_VREF_SEL);

	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ5), p->odt_onoff ? 0x16 : 0xb,
		SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B1_DQ5), p->odt_onoff ? 0x16 : 0xb,
		SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ5), p->odt_onoff ? 0x16 : 0xb,
		SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2);
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD5), p->odt_onoff ? 0x16 : 0xb,
		SHU_CA_CMD5_RG_RX_ARCMD_VREF_SEL);

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B0_DQ10),
		p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ0_DQS_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ1_DQS_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ2_DQS_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ3_DQS_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQM_DQS_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQM_MCK4X_SEL_B0));

	io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B1_DQ10),
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ4_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ5_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ6_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ7_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ0_DQS_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ1_DQS_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ2_DQS_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ3_DQS_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ4_DQS_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ5_DQS_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ6_DQS_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ7_DQS_SEL_B1));

	io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_B2_DQ10),
		p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ0_DQS_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ1_DQS_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ2_DQS_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ3_DQS_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQM_DQS_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQM_MCK4X_SEL_B2));

	io_32_write_fld_multi((DDRPHY_B0_DQ10 + SHIFT_TO_CHB_ADDR),
		p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQ4_MCK4X_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ0_DQS_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ1_DQS_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ2_DQS_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQ4_DQS_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_RX_ARDQM_DQS_SEL_B0) |
		p_fld(SET_FLD, B0_DQ10_RG_TX_ARDQM_MCK4X_SEL_B0));

	io_32_write_fld_multi((DDRPHY_B1_DQ10 + SHIFT_TO_CHB_ADDR),
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ4_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ5_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ6_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_TX_ARDQ7_MCK4X_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ0_DQS_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ1_DQS_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ2_DQS_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ3_DQS_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ4_DQS_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ5_DQS_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ6_DQS_SEL_B1) |
		p_fld(SET_FLD, B1_DQ10_RG_RX_ARDQ7_DQS_SEL_B1));

	io_32_write_fld_multi((DDRPHY_B2_DQ10 + SHIFT_TO_CHB_ADDR),
		p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ0_MCK4X_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ1_MCK4X_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ2_MCK4X_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQ3_MCK4X_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ0_DQS_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ1_DQS_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ2_DQS_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQ3_DQS_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_RX_ARDQM_DQS_SEL_B2) |
		p_fld(SET_FLD, B2_DQ10_RG_TX_ARDQM_MCK4X_SEL_B2));
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	io_32_write_fld_align(DDRPHY_B0_DQ6, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B0);
#if 0
	io_32_write_fld_align(DDRPHY_B0_DQ6 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B0);
#endif
	io_32_write_fld_align(DDRPHY_B1_DQ6, SET_FLD,
		B1_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ6, SET_FLD,
		B2_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B2);

	io_32_write_fld_align(DDRPHY_CA_CMD6, CLEAR_FLD,
		CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN);
#if 0
	io_32_write_fld_align(DDRPHY_CA_CMD6 + SHIFT_TO_CHB_ADDR, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN);
#endif

	io_32_write_fld_align(DDRPHY_CA_CMD6, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN);

	io_32_write_fld_align(DDRPHY_B0_DQ6, CLEAR_FLD,
		B0_DQ6_RG_RX_ARDQ_O1_SEL_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ6, CLEAR_FLD,
		B1_DQ6_RG_RX_ARDQ_O1_SEL_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ6, CLEAR_FLD,
		B2_DQ6_RG_RX_ARDQ_O1_SEL_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD6, CLEAR_FLD,
		CA_CMD6_RG_RX_ARCMD_O1_SEL);

	io_32_write_fld_align(DDRPHY_B0_DQ6, CLEAR_FLD,
		B0_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ6, CLEAR_FLD,
		B1_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ6, CLEAR_FLD,
		B2_DQ6_RG_TX_ARDQ_ODTEN_EXT_DIS_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD6, CLEAR_FLD,
		CA_CMD6_RG_TX_ARCMD_ODTEN_EXT_DIS);

	io_32_write_fld_multi(DDRPHY_B0_DQ3,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B0));
#if 0
	io_32_write_fld_multi(DDRPHY_B0_DQ3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B0));
#endif

	io_32_write_fld_multi(DDRPHY_B1_DQ3,
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B1));
	io_32_write_fld_multi(DDRPHY_B2_DQ3,
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQM0_IN_BUFF_EN_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQS0_IN_BUFF_EN_B2));

	io_32_write_fld_multi(DDRPHY_CA_CMD3,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_IN_BUFF_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCLK_IN_BUFF_EN));
#if 0
	io_32_write_fld_multi(DDRPHY_CA_CMD3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_IN_BUFF_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCLK_IN_BUFF_EN));
#endif

	io_32_write_fld_align(DDRPHY_B0_DQ9, CLEAR_FLD,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_CA_CMD10, CLEAR_FLD,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
	io_32_write_fld_multi(DDRPHY_CA_CMD10,
		p_fld(SET_FLD, CA_CMD10_RG_RX_ARCMD_STBEN_RESETB) |
		p_fld(SET_FLD, CA_CMD10_RG_RX_ARCLK_STBEN_RESETB));

	io_32_write_fld_align(DDRPHY_B0_DQ6, SET_FLD,
		B0_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ6, SET_FLD,
		B1_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ6, SET_FLD,
		B2_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD6, SET_FLD,
		CA_CMD6_RG_RX_ARCMD_RPRE_TOG_EN);

#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
	io_32_write_fld_align(DDRPHY_B1_DQ9, 0x04,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
#if 0
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, 0x04,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, 0x04,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, 0x04,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
#endif

#else
	io_32_write_fld_align(DDRPHY_B2_DQ9, 0x7,
		B2_DQ9_R_IN_GATE_EN_LOW_OPT_B2);
#if 0
	io_32_write_fld_align(DDRPHY_B2_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B2_DQ9_R_IN_GATE_EN_LOW_OPT_B2);
#endif
	io_32_write_fld_align(DDRPHY_B1_DQ9, 0x7,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
#if 0
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, 0x7,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
#endif
#endif

	io_32_write_fld_align(DRAMC_REG_SHU_MISC, 0x2,
		SHU_MISC_REQQUE_MAXCNT);

	io_32_write_fld_align(DRAMC_REG_SHU_DQSG, 0x2a,
		SHU_DQSG_SCINTV);

	io_32_write_fld_multi(DRAMC_REG_SREFCTRL,
		p_fld(CLEAR_FLD, SREFCTRL_SCSM_CGAR) |
		p_fld(CLEAR_FLD, SREFCTRL_SCARB_SM_CGAR) |
		p_fld(CLEAR_FLD, SREFCTRL_RDDQSOSC_CGAR) |
		p_fld(CLEAR_FLD, SREFCTRL_HMRRSEL_CGAR));

	io_32_write_fld_align(DRAMC_REG_PRE_TDQSCK1, SET_FLD,
		PRE_TDQSCK1_TXUIPI_CAL_CGAR);

	io_32_write_fld_align(DRAMC_REG_SHU_MISC, 0xf, SHU_MISC_PREA_INTV);

	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ8,
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
		p_fld(SET_FLD, SHU_B0_DQ8_R_DMSTBEN_SYNC_CG_IG_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_EN_B0) |
		p_fld(0x7fff, SHU_B0_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ8,
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
		p_fld(SET_FLD, SHU_B1_DQ8_R_DMSTBEN_SYNC_CG_IG_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_EN_B1) |
		p_fld(0x7fff, SHU_B1_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ8,
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMRANK_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B2) |
		p_fld(SET_FLD, SHU_B2_DQ8_R_DMSTBEN_SYNC_CG_IG_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ8_R_DMRXDLY_CG_IG_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ8_R_DMRXDVS_UPD_FORCE_EN_B2) |
		p_fld(0x7fff, SHU_B2_DQ8_R_DMRXDVS_UPD_FORCE_CYC_B2));

	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD8,
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRANK_CHG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRANK_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_FLAG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMDQSIEN_FLAG_SYNC_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMSTBEN_SYNC_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRXDLY_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_CA) |
		p_fld(SET_FLD, SHU_CA_CMD8_R_DMRXDVS_RDSEL_PIPE_CG_IG_CA) |
		p_fld(CLEAR_FLD, SHU_CA_CMD8_R_DMRXDVS_UPD_FORCE_EN_CA) |
		p_fld(0x7fff, SHU_CA_CMD8_R_DMRXDVS_UPD_FORCE_CYC_CA));

	io_32_write_fld_multi(DDRPHY_MISC_CTRL3,
		p_fld(SET_FLD, MISC_CTRL3_DRAM_CLK_NEW_DQ_EN_SEL) |
		p_fld(CLEAR_FLD, MISC_CTRL3_R_DDRPHY_RX_PIPE_CG_IG) |
		p_fld(CLEAR_FLD, MISC_CTRL3_R_DDRPHY_COMB_CG_IG) |
		p_fld(SET_FLD, MISC_CTRL3_DDRPHY_MCK_MPDIV_CG_DQ_SEL) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_MPDIV_CG_DQ_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_CG_DQS_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_CG_DQ_OPT) |
		p_fld(SET_FLD, MISC_CTRL3_DRAM_CLK_NEW_CA_EN_SEL) |
		p_fld(SET_FLD, MISC_CTRL3_DDRPHY_MCK_MPDIV_CG_CA_SEL) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_CG_MCK_CA_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_MPDIV_CG_CA_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_CG_CLK_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_CG_CMD_OPT) |
		p_fld(CLEAR_FLD, MISC_CTRL3_ARPI_CG_MCK_DQ_OPT));

	io_32_write_fld_align(DDRPHY_MISC_CTRL0, SET_FLD,
		MISC_CTRL0_R_DMSHU_PHYDCM_FORCEOFF);

	io_32_write_fld_align(DDRPHY_MISC_RXDVS2, SET_FLD,
		MISC_RXDVS2_R_DMRXDVS_SHUFFLE_CTRL_CG_IG);

	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXDVS_DQM_FLAGSEL_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7,
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXDVS_DQM_FLAGSEL_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_DQM_EN_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXDVS_DQM_FLAGSEL_B2));

	io_32_write_fld_multi(DRAMC_REG_CLKAR,
		p_fld(CLEAR_FLD, CLKAR_SELPH_CMD_CG_DIS) |
		p_fld(CLEAR_FLD, CLKAR_DWCLKRUN) |
		p_fld(0, CLKAR_REQQUE_PACG_DIS));

	io_32_write_fld_align(DRAMC_REG_SHU_DQSG_RETRY, CLEAR_FLD,
		SHU_DQSG_RETRY_R_RETRY_PA_DSIABLE);
	io_32_write_fld_align(DRAMC_REG_WRITE_LEV, CLEAR_FLD,
		WRITE_LEV_DDRPHY_COMB_CG_SEL);

	io_32_write_fld_multi(DRAMC_REG_DUMMY_RD,
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_PA_OPT) |
		p_fld(SET_FLD, DUMMY_RD_DMYRD_REORDER_DIS) |
		p_fld(CLEAR_FLD, DUMMY_RD_DMYRD_HPRI_DIS) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_CNT6) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_CNT5) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_CNT3) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_SW));

	io_32_write_fld_multi(DRAMC_REG_STBCAL2,
		p_fld(SET_FLD, STBCAL2_STB_UIDLYCG_IG) |
		p_fld(SET_FLD, STBCAL2_STB_PIDLYCG_IG));
	io_32_write_fld_multi(DRAMC_REG_EYESCAN,
		p_fld(SET_FLD, EYESCAN_EYESCAN_DQS_SYNC_EN) |
		p_fld(SET_FLD, EYESCAN_EYESCAN_NEW_DQ_SYNC_EN) |
		p_fld(SET_FLD, EYESCAN_EYESCAN_DQ_SYNC_EN));

	io_32_write_fld_multi(DRAMC_REG_SHU_ODTCTRL,
		p_fld(SET_FLD, SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG) |
		p_fld(CLEAR_FLD, SHU_ODTCTRL_ROEN) |
		p_fld(SET_FLD, SHU_ODTCTRL_RODTE) |
		p_fld(SET_FLD, SHU_ODTCTRL_RODTE2) |
		p_fld(SET_FLD, SHU_ODTCTRL_RODTEN_SELPH_CG_IG));

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL0, SET_FLD,
		SHU_B0_DLL0_RG_ARPISM_MCK_SEL_B0_SHU);
	io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL0, SET_FLD,
		SHU_B1_DLL0_RG_ARPISM_MCK_SEL_B1_SHU);
	io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL0, SET_FLD,
		SHU_B2_DLL0_RG_ARPISM_MCK_SEL_B2_SHU);
	io_32_write_fld_align_all(DDRPHY_SHU_CA_DLL0, SET_FLD,
		SHU_CA_DLL0_RG_ARPISM_MCK_SEL_CA_SHU);
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	io_32_write_fld_multi(DRAMC_REG_SHURK0_DQSCAL,
		p_fld(0, SHURK0_DQSCAL_R0DQSIENLLMTEN) |
		p_fld(0, SHURK0_DQSCAL_R0DQSIENHLMTEN));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_DQSCAL,
		p_fld(0, SHURK1_DQSCAL_R1DQSIENLLMTEN) |
		p_fld(0, SHURK1_DQSCAL_R1DQSIENHLMTEN));
	io_32_write_fld_multi(DRAMC_REG_SHU_STBCAL,
		p_fld(1, SHU_STBCAL_DQSG_MODE) |
		p_fld(1, SHU_STBCAL_PICGLAT));

#if DramcHWDQSGatingTracking_DVT_JADE_TRACKING_MODE

	io32_write_4b(DRAMC_REG_SHU_PIPE, 0xFC000000);
	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ6, CLEAR_FLD,
		SHU_B2_DQ6_RG_ARPI_OFFSET_DQSIEN_B2);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ6, CLEAR_FLD,
		SHU_B1_DQ6_RG_ARPI_OFFSET_DQSIEN_B1);
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ6, CLEAR_FLD,
		SHU_B0_DQ6_RG_ARPI_OFFSET_DQSIEN_B0);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD6, CLEAR_FLD,
		SHU_CA_CMD6_RG_ARPI_OFFSET_CLKIEN);
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ6 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, SHU_B0_DQ6_RG_ARPI_OFFSET_DQSIEN_B0);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, SHU_CA_CMD6_RG_ARPI_OFFSET_CLKIEN);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ6 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, SHU_B1_DQ6_RG_ARPI_OFFSET_DQSIEN_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ6 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, SHU_B2_DQ6_RG_ARPI_OFFSET_DQSIEN_B2);
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	{
#if 0  /*modified before broadcast*/
		io_32_write_fld_align(DDRPHY_SHU_B0_DLL1, SET_FLD,
			RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B1_DLL1, SET_FLD,
			RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B2_DLL1, SET_FLD,
			RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL1, SET_FLD,
			RG_ARCMD_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B0_DLL1 + SHIFT_TO_CHB_ADDR,
			SET_FLD, RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B1_DLL1 + SHIFT_TO_CHB_ADDR,
			SET_FLD, RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_B2_DLL1 + SHIFT_TO_CHB_ADDR,
			SET_FLD, RG_ARDQ_REV_BIT_05_RX_SER_RST_MODE);
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL1 + SHIFT_TO_CHB_ADDR,
			SET_FLD, RG_ARCMD_REV_BIT_05_RX_SER_RST_MODE);

		io_32_write_fld_align(DDRPHY_SHU_B0_DLL1, CLEAR_FLD,
			RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN);
		io_32_write_fld_align(DDRPHY_SHU_B1_DLL1, CLEAR_FLD,
			RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN);
		io_32_write_fld_align(DDRPHY_SHU_B2_DLL1, CLEAR_FLD,
			RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN);
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL1, CLEAR_FLD,
			RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN);
		io_32_write_fld_align(DDRPHY_SHU_B0_DLL1 + SHIFT_TO_CHB_ADDR,
			CLEAR_FLD, RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN);
		io_32_write_fld_align(DDRPHY_SHU_B1_DLL1 + SHIFT_TO_CHB_ADDR,
			CLEAR_FLD, RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN);
		io_32_write_fld_align(DDRPHY_SHU_B2_DLL1 + SHIFT_TO_CHB_ADDR,
			CLEAR_FLD, RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN);
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL1 + SHIFT_TO_CHB_ADDR,
			CLEAR_FLD, RG_ARDQ_REV_BIT_09_TX_READ_BASE_EN);
#endif
		if (p->frequency >= DDR2666_FREQ) {
			io_32_write_fld_align(DDRPHY_SHU_B0_DLL1, 0x224,
				SHU_B0_DLL1_RG_ARDQ_REV_B0);
			io_32_write_fld_align(DDRPHY_SHU_B1_DLL1, 0x224,
				SHU_B1_DLL1_RG_ARDQ_REV_B1);
			io_32_write_fld_align(DDRPHY_SHU_B2_DLL1, 0x224,
				SHU_B2_DLL1_RG_ARDQ_REV_B2);
		} else {
			io_32_write_fld_align(DDRPHY_SHU_B0_DLL1, 0x324,
				SHU_B0_DLL1_RG_ARDQ_REV_B0);
			io_32_write_fld_align(DDRPHY_SHU_B1_DLL1, 0x324,
				SHU_B1_DLL1_RG_ARDQ_REV_B1);
			io_32_write_fld_align(DDRPHY_SHU_B2_DLL1, 0x324,
				SHU_B2_DLL1_RG_ARDQ_REV_B2);
		}

		dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
		io_32_write_fld_align_all(DDRPHY_SHU_CA_DLL1, 0x4e1,
			SHU_CA_DLL1_RG_ARCMD_REV);
		dramc_broadcast_on_off(DRAMC_BROADCAST_ON);
#if 0
		io_32_write_fld_align(DDRPHY_SHU_B0_DLL1 + SHIFT_TO_CHB_ADDR,
			0x324, SHU_B0_DLL1_RG_ARDQ_REV_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DLL1 + SHIFT_TO_CHB_ADDR,
			0x324, SHU_B1_DLL1_RG_ARDQ_REV_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DLL1 + SHIFT_TO_CHB_ADDR,
			0x324, SHU_B2_DLL1_RG_ARDQ_REV_B2);
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL1 + SHIFT_TO_CHB_ADDR,
			0x4e1, SHU_CA_DLL1_RG_ARCMD_REV);
#endif
	}

	io_32_write_fld_align(DRAMC_REG_SHU_DQSG, CLEAR_FLD,
		SHU_DQSG_STB_UPDMASKCYC);
	io_32_write_fld_align(DRAMC_REG_SHU_DQSG, CLEAR_FLD,
		SHU_DQSG_STB_UPDMASK_EN);

	if (p->frequency >= DDR1600_FREQ) {
		io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, 3,
			SHU_STBCAL_DMSTBLAT);
		/*
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM2, 0x7, SHU_ACTIM2_TR2W);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_AC_TIME_05T, 0, SHU_AC_TIME_05T_TR2W_05T);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x5, SHU_ACTIM_XRT_XRTW2R);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x6, SHU_ACTIM_XRT_XRTR2W);
		 */
	} else if (p->frequency >= DDR1200_FREQ) {
		io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, 2,
			SHU_STBCAL_DMSTBLAT);
		/*
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM2, 0x5, SHU_ACTIM2_TR2W);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_AC_TIME_05T, 0, SHU_AC_TIME_05T_TR2W_05T);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x5, SHU_ACTIM_XRT_XRTW2R);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x6, SHU_ACTIM_XRT_XRTR2W);
		 */
	} else {
		io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, 1,
			SHU_STBCAL_DMSTBLAT);
		/*
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM2, 0x4, SHU_ACTIM2_TR2W);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_AC_TIME_05T, 0, SHU_AC_TIME_05T_TR2W_05T);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x5, SHU_ACTIM_XRT_XRTW2R);
		 * io_32_write_fld_align_all(DRAMC_REG_SHU_ACTIM_XRT, 0x6, SHU_ACTIM_XRT_XRTR2W);
		 */
	}

	/*
	 * For LP3 FFFF corner IC pass LTLV test
	 * Need to check with
	 */
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ5, 0,
		SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ5, 0,
		SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ5, 0,
		SHU_B2_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B2);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD5, 0,
		SHU_CA_CMD5_RG_RX_ARCLK_DQSIEN_DLY);

#else
	/*
	 * Valid delay mode debug : monitor window
	 * io_32_write_fld_align(DDRPHY_SHU_B0_DQ5+SHIFT_TO_CHB_ADDR, 7, SHU_B0_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B0);
	 * io_32_write_fld_align(DDRPHY_SHU_B1_DQ5+SHIFT_TO_CHB_ADDR, 7, SHU_B1_DQ5_RG_RX_ARDQS0_DQSIEN_DLY_B1);
	 * io_32_write_fld_align_all(DRAMC_REG_SHU_PIPE, 0xF0000000,PHY_FLD_FULL);
	 */
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_DQSG), \
		p_fld(9, SHU_DQSG_STB_UPDMASKCYC) | \
		p_fld(1, SHU_DQSG_STB_UPDMASK_EN));
#endif

	io_32_write_fld_align(DDRPHY_SHU_B0_DQ5, CLEAR_FLD,
		SHU_B0_DQ5_RG_ARPI_FB_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ5, CLEAR_FLD,
		SHU_B1_DQ5_RG_ARPI_FB_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ5, CLEAR_FLD,
		SHU_B2_DQ5_RG_ARPI_FB_B2);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD5, CLEAR_FLD,
		SHU_CA_CMD5_RG_ARPI_FB_CA);
#if 0
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ5 + SHIFT_TO_CHB_ADDR, 0x4,
		SHU_B0_DQ5_RG_ARPI_FB_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ5 + SHIFT_TO_CHB_ADDR, 0x4,
		SHU_B1_DQ5_RG_ARPI_FB_B1);
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD5 + SHIFT_TO_CHB_ADDR, 0x4,
		SHU_CA_CMD5_RG_ARPI_FB_CA);
#endif

	io_32_write_fld_multi(DRAMC_REG_DRAMCTRL,
		p_fld(0, DRAMCTRL_FW2R) |
		p_fld(SET_FLD, DRAMCTRL_DYNMWREN) |
		p_fld(SET_FLD, DRAMCTRL_CLKWITRFC) |
		p_fld(CLEAR_FLD, DRAMCTRL_TMRR2WDIS) |
		p_fld(CLEAR_FLD, DRAMCTRL_ADRBIT3DEC) |
		p_fld(0x03, DRAMCTRL_PREA_RK) |
		p_fld(CLEAR_FLD, DRAMCTRL_CTOREQ_HPRI_OPT));
	io_32_write_fld_align(DRAMC_REG_MISCTL0, SET_FLD,
		MISCTL0_PBC_ARB_EN);
	io_32_write_fld_align(DRAMC_REG_PERFCTL0, CLEAR_FLD,
		PERFCTL0_WRFIFO_OPT);
	io_32_write_fld_multi(DRAMC_REG_PERFCTL0,
		p_fld(CLEAR_FLD, PERFCTL0_REORDEREN) |
		p_fld(CLEAR_FLD, PERFCTL0_RWHPRICTL) |
		p_fld(SET_FLD, PERFCTL0_EMILLATEN) |
		p_fld(SET_FLD, PERFCTL0_RWAGEEN) |
		p_fld(SET_FLD, PERFCTL0_RWHPRIEN));
	io_32_write_fld_align(DRAMC_REG_SREFCTRL, 0x8, SREFCTRL_SREFDLY);

	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, SET_FLD,
		SPCMDCTRL_REFR_BLOCKEN);
	io_32_write_fld_align(DRAMC_REG_HW_MRR_FUN, CLEAR_FLD,
		HW_MRR_FUN_TMRR_ENA);

	io_32_write_fld_multi(DRAMC_REG_SHUCTRL,
		p_fld(SET_FLD, SHUCTRL_VRCG_EN) |
		p_fld(0x3, SHUCTRL_R_DVFS_PICG_MARGIN2) |
		p_fld(0x3, SHUCTRL_R_DVFS_PICG_MARGIN3));
	io_32_write_fld_multi(DRAMC_REG_SHUCTRL2,
		p_fld(SET_FLD, SHUCTRL2_SHORTQ_OPT) |
		p_fld(0x3, SHUCTRL2_R_DVFS_PICG_MARGIN) |
		p_fld(CLEAR_FLD, SHUCTRL2_R_DVFS_SREF_OPT));

	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(CLEAR_FLD, REFCTRL0_REFNA_OPT) |
		p_fld(0x4, REFCTRL0_DISBYREFNUM) |
		p_fld(CLEAR_FLD, REFCTRL0_UPDBYWR));
	io_32_write_fld_multi(DDRPHY_B0_DQ6,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0));
	io_32_write_fld_multi(DDRPHY_CA_CMD6,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_EN));

#ifdef XRTR2R_PERFORM_ENHANCE_DQSG_RX_DLY
#if 0
	io_32_write_fld_multi(DDRPHY_B0_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0));
#endif
	io_32_write_fld_multi(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
#if 0
	io_32_write_fld_multi(DDRPHY_B1_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
	io_32_write_fld_multi(DDRPHY_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_EN));
#endif

#if NON_EXIST_RG
	io_32_write_fld_align(DRAMC_REG_STBCAL2, SET_FLD,
		STBCAL2_STB_PICG_EARLY_1T_EN);
#endif

	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_EN_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_EN_B2));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7,
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1));
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRXRANK_DQS_LAT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQS_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQ_LAT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXRANK_DQ_EN_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQS_EN_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_LAT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXRANK_DQ_EN_B2));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQS_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_LAT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXRANK_DQ_EN_B1));
	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_CA_CMD7_R_DMRXRANK_CLK_LAT) |
		p_fld(SET_FLD, SHU_CA_CMD7_R_DMRXRANK_CLK_EN) |
		p_fld(SET_FLD, SHU_CA_CMD7_R_DMRXRANK_CMD_LAT) |
		p_fld(SET_FLD, SHU_CA_CMD7_R_DMRXRANK_CMD_EN));
#endif

#else
#if 0
	io_32_write_fld_multi(DDRPHY_B0_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0));
#endif
	io_32_write_fld_multi(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
	io_32_write_fld_multi(DDRPHY_B2_DQ6,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2));
#if 0
	io_32_write_fld_multi(DDRPHY_B2_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2));
	io_32_write_fld_multi(DDRPHY_B1_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1));
	io_32_write_fld_multi(DDRPHY_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_EN));
#endif

#endif

	io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI1,
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_CS_JUMP_EN) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_CLK_JUMP_EN) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_CMD_JUMP_EN) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPI_CLKIEN_JUMP_EN) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA_REG_OPT));
	io_32_write_fld_multi(DDRPHY_B0_DLL_ARPI1,
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_DQS_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_DQM_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPI_DQSIEN_JUMP_EN_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0_REG_OPT));
	io_32_write_fld_multi(DDRPHY_B1_DLL_ARPI1,
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_DQS_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_DQM_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPI_DQSIEN_JUMP_EN_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1_REG_OPT));
	io_32_write_fld_multi(DDRPHY_B2_DLL_ARPI1,
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_MCTL_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_FB_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_DQS_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_DQM_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPI_DQSIEN_JUMP_EN_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPISM_MCK_SEL_B2) |
		p_fld(CLEAR_FLD, B2_DLL_ARPI1_RG_ARPISM_MCK_SEL_B2_REG_OPT));

	io_32_write_fld_align(DDRPHY_B0_DQ5, CLEAR_FLD,
		B0_DQ5_RG_RX_ARDQS0_DVS_EN_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ5, CLEAR_FLD,
		B1_DQ5_RG_RX_ARDQS0_DVS_EN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ5, CLEAR_FLD,
		B2_DQ5_RG_RX_ARDQS0_DVS_EN_B2);
	io_32_write_fld_align(DDRPHY_CA_CMD5, CLEAR_FLD,
		CA_CMD5_RG_RX_ARCLK_DVS_EN);

	if (p->odt_onoff == ODT_ON) {
		io_32_write_fld_align(DRAMC_REG_SHU_ODTCTRL, 1, SHU_ODTCTRL_ROEN);
		io_32_write_fld_align(DDRPHY_SHU_B0_DQ7, SET_FLD,
			SHU_B0_DQ7_R_DMRODTEN_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ7, SET_FLD,
			SHU_B1_DQ7_R_DMRODTEN_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DQ7, SET_FLD,
			SHU_B2_DQ7_R_DMRODTEN_B2);
	} else {
		io_32_write_fld_align(DRAMC_REG_SHU_ODTCTRL, 0, SHU_ODTCTRL_ROEN);
		io_32_write_fld_align(DDRPHY_SHU_B0_DQ7, CLEAR_FLD,
			SHU_B0_DQ7_R_DMRODTEN_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ7, CLEAR_FLD,
			SHU_B1_DQ7_R_DMRODTEN_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DQ7, CLEAR_FLD,
			SHU_B2_DQ7_R_DMRODTEN_B2);
	}

	io_32_write_fld_align(DRAMC_REG_SHU_ODTCTRL, SET_FLD, SHU_ODTCTRL_RODTEN_OPT);
	io_32_write_fld_multi(DRAMC_REG_SHU_RODTENSTB,
		p_fld(0, SHU_RODTENSTB_RODTEN_P1_ENABLE) |
		p_fld(1, SHU_RODTENSTB_RODTENSTB_TRACK_UDFLWCTRL) |
		p_fld(1, SHU_RODTENSTB_RODTENSTB_SELPH_MODE));
	io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, CLEAR_FLD, SHU_STBCAL_DQSIEN_PICG_MODE);
	io_32_write_fld_align(DRAMC_REG_SHU_RODTENSTB, CLEAR_FLD,
		SHU_RODTENSTB_RODTENSTB_4BYTE_EN);
#if ENABLE_RODT_TRACKING
	if(get_div_mode(p) == DIV4_MODE)
		io_32_write_fld_align(DRAMC_REG_SHU_RODTENSTB,
			0x21, SHU_RODTENSTB_RODTENSTB_OFFSET);
	else
		io_32_write_fld_align(DRAMC_REG_SHU_RODTENSTB,
			0x11, SHU_RODTENSTB_RODTENSTB_OFFSET);

	unsigned char readroen;
	readroen = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_ODTCTRL),
		SHU_ODTCTRL_ROEN);
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_RODTENSTB),
		p_fld(0xff, SHU_RODTENSTB_RODTENSTB_EXT) |
		p_fld(readroen, SHU_RODTENSTB_RODTENSTB_TRACK_EN));
#endif


	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL4, 0x13300000,
		MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);
#if 0
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL4 + SHIFT_TO_CHB_ADDR,
		0x22200a00, MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);
#endif

	io_32_write_fld_multi(DRAMC_REG_PERFCTL0,
		p_fld(SET_FLD, PERFCTL0_RWSPLIT) |
		p_fld(CLEAR_FLD, PERFCTL0_REORDEREN));

#if NON_EXIST_RG
#if FIX_CROSSRK_XRT_05T_OPT
	io_32_write_fld_align(DRAMC_REG_PERFCTL0, CLEAR_FLD,
		PERFCTL0_XRT_05T_OPT);
#else
	io_32_write_fld_align(DRAMC_REG_PERFCTL0, SET_FLD,
		PERFCTL0_XRT_05T_OPT);
#endif
#endif
	io_32_write_fld_align(DRAMC_REG_CLKCTRL, SET_FLD,
		CLKCTRL_SEQCLKRUN3);
	io_32_write_fld_align(DRAMC_REG_STBCAL1, CLEAR_FLD,
		STBCAL1_DLLFRZ_MON_PBREF_OPT);
	io_32_write_fld_align(DRAMC_REG_ARBCTL, SET_FLD,
		ARBCTL_DBIWR_IMP_EN );

	io_32_write_fld_align(DDRPHY_SHU_CA_CMD7, CLEAR_FLD,
		SHU_CA_CMD7_R_DMRANKRXDVS_CA);

	io_32_write_fld_align(DRAMC_REG_CKECTRL, CLEAR_FLD,
		CKECTRL_CKEPBDIS);

	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ3,
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQ_PDB_PRE_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQ_PU_PRE_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQS0_PDB_PRE_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ3_RG_TX_ARDQS0_PU_PRE_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ3,
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQ_PDB_PRE_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQ_PU_PRE_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQS0_PDB_PRE_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ3_RG_TX_ARDQS0_PU_PRE_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ3,
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQ_PDB_PRE_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQ_PU_PRE_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQS0_PDB_PRE_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ3_RG_TX_ARDQS0_PU_PRE_B2));

#if NON_EXIST_RG
	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, CLEAR_FLD,
		SPCMDCTRL_HMR4_TOG_OPT);
#endif
	if (get_div_mode(p) == DIV4_MODE) {
		io_32_write_fld_align(DDRPHY_B0_DQ9, CLEAR_FLD,
			B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0);
		io_32_write_fld_align(DDRPHY_B1_DQ9, CLEAR_FLD,
			B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1);
		io_32_write_fld_align(DDRPHY_B2_DQ9, CLEAR_FLD,
			B2_DQ9_R_DMRXFIFO_STBENCMP_EN_B2);
		io_32_write_fld_align(DDRPHY_CA_CMD10, CLEAR_FLD,
			CA_CMD10_R_DMRXFIFO_STBENCMP_EN_CA);
	} else {
		io_32_write_fld_align(DDRPHY_B0_DQ9, SET_FLD,
			B0_DQ9_R_DMRXFIFO_STBENCMP_EN_B0);
		io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
			B1_DQ9_R_DMRXFIFO_STBENCMP_EN_B1);
		io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
			B2_DQ9_R_DMRXFIFO_STBENCMP_EN_B2);
		io_32_write_fld_align(DDRPHY_CA_CMD10, SET_FLD,
			CA_CMD10_R_DMRXFIFO_STBENCMP_EN_CA);
	}

	return DRAM_OK;
}

static void dramc_setting_ddr4(DRAMC_CTX_T *p)
{

	unsigned char reg_txdly_dqs, reg_txdly_dqs_oen;
	unsigned char reg_txdly_dqdqm, reg_txdly_dqdqm_oen;
	unsigned char reg_dly_dqs, reg_dly_dqs_oen;
	unsigned char reg_dly_dqdqm, reg_dly_dqdqm_oen;

	if (p->freq_sel == DDR_DDR3200) {
		reg_txdly_dqs = 0x3;
		reg_txdly_dqs_oen = 0x2;
		reg_dly_dqs = 0x1;
		reg_dly_dqs_oen = 0x7;

		reg_txdly_dqdqm = 0x3;
		reg_txdly_dqdqm_oen = 0x2;
		reg_dly_dqdqm = 0x1;
		reg_dly_dqdqm_oen = 0x7;
	} else if (p->freq_sel == DDR_DDR2666) {
		reg_txdly_dqs = 0x2;
		reg_txdly_dqs_oen = 0x2;
		reg_dly_dqs = 0x5;
		reg_dly_dqs_oen = 0x3;

		reg_txdly_dqdqm = 0x2;
		reg_txdly_dqdqm_oen = 0x2;
		reg_dly_dqdqm = 0x5;
		reg_dly_dqdqm_oen = 0x3;
	} else if (p->freq_sel <= DDR_DDR1600) {
		reg_txdly_dqs = 0x2;
		reg_txdly_dqs_oen = 0x2;
		reg_dly_dqs = 0x3;
		reg_dly_dqs_oen = 0x1;

		reg_txdly_dqdqm = 0x2;
		reg_txdly_dqdqm_oen = 0x2;
		reg_dly_dqdqm = 0x3;
		reg_dly_dqdqm_oen = 0x1;
	}

	auto_refresh_cke_off(p);

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_multi_all(DDRPHY_CKMUX_SEL,
		p_fld(SET_FLD, CKMUX_SEL_R_PHYCTRLMUX) |
		p_fld(SET_FLD, CKMUX_SEL_R_PHYCTRLDCM));
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, SET_FLD,
		MISC_CG_CTRL0_W_CHG_MEM);

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, CLEAR_FLD,
		MISC_CG_CTRL0_CLK_MEM_SEL);

#if NON_EXIST_RG
	io_32_write_fld_align_all(DDRPHY_MISC_CTRL0, SET_FLD,
		MISC_CTRL0_R_DMRDSEL_DIV2_OPT);
#endif

	if (0/* p->bDLP3 */) {/* cc note, review this RG!!! */
		io_32_write_fld_align_all(DDRPHY_MISC_CTRL1, SET_FLD,
			MISC_CTRL1_R_DMMUXCA_SEC);
	}

	io_32_write_fld_multi(DDRPHY_MISC_SPM_CTRL1,
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B0) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B1) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B2) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_CA));
#if 0
	io_32_write_fld_multi(DDRPHY_MISC_SPM_CTRL1 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B0) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_B1) |
		p_fld(CLEAR_FLD, MISC_SPM_CTRL1_RG_ARDMSUS_10_CA));
#endif

	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL0, 0xfbffefff,
		MISC_SPM_CTRL0_PHY_SPM_CTL0);

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL2, 0xfffeffef,
		MISC_SPM_CTRL2_PHY_SPM_CTL2);
	io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL2 + SHIFT_TO_CHB_ADDR, 0x7ffeffef,
		MISC_SPM_CTRL2_PHY_SPM_CTL2);
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL2, 0x6003bf,
		MISC_CG_CTRL2_RG_MEM_DCM_CTL);
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL4, 0x13300000,
		MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);

#if 0
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL4 + SHIFT_TO_CHB_ADDR,
		0x222a2a00, MISC_CG_CTRL4_R_PHY_MCK_CG_CTRL);
#endif
	io_32_write_fld_multi(DDRPHY_SHU_PLL1,
		p_fld(SET_FLD, SHU_PLL1_R_SHU_AUTO_PLL_MUX) |
		p_fld(0x7, SHU_PLL1_SHU1_PLL1_RFU));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRANKRXDVS_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7,
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRANKRXDVS_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ7,
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B2) |
		p_fld(SET_FLD, SHU_B2_DQ7_R_DMRANKRXDVS_B2));
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B0) |
		p_fld(SET_FLD, SHU_B0_DQ7_MIDPI_DIV4_ENABLE) |
		p_fld(SET_FLD, SHU_B0_DQ7_R_DMRANKRXDVS_B0));
#endif
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRXDVS_PBYTE_FLAG_OPT_B1) |
		p_fld(SET_FLD, SHU_B1_DQ7_R_DMRANKRXDVS_B1));
#endif
	/* cc add since this bit default is 1'b1, which will cause CS blocked */
	io_32_write_fld_align(DRAMC_REG_SLP4_TESTMODE, CLEAR_FLD,
		SLP4_TESTMODE_DQM_FIX_LOW);
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD7 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_CA_CMD7_R_DMRXDVS_PBYTE_FLAG_OPT_CA) |
		p_fld(SET_FLD, SHU_CA_CMD7_R_DMRANKRXDVS_CA));
#endif
	/*
	 * io_32_write_fld_align(DDRPHY_SHU_PLL4, 0xfe, SHU_PLL4_RG_RPHYPLL_RESERVED);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL6, 0xfe, SHU_PLL6_RG_RCLRPLL_RESERVED);
	 */
	io_32_write_fld_multi(DDRPHY_CA_CMD3,
		p_fld(CLEAR_FLD, CA_CMD3_RG_TX_ARCMD_OE_DIS) |
		p_fld(CLEAR_FLD, CA_CMD3_RG_TX_ARCMD_ODTEN_DIS) |
		p_fld(CLEAR_FLD, CA_CMD3_RG_TX_ARCLK_OE_DIS) |
		p_fld(CLEAR_FLD, CA_CMD3_RG_TX_ARCLK_ODTEN_DIS));
	io_32_write_fld_multi(DDRPHY_B0_DQ2,
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQ_OE_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQ_ODTEN_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQS0_OE_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ2,
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQ_OE_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQ_ODTEN_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQS0_OE_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B1));
	io_32_write_fld_multi(DDRPHY_B2_DQ2,
		p_fld(CLEAR_FLD, B2_DQ2_RG_TX_ARDQ_OE_DIS_B2) |
		p_fld(CLEAR_FLD, B2_DQ2_RG_TX_ARDQ_ODTEN_DIS_B2) |
		p_fld(CLEAR_FLD, B2_DQ2_RG_TX_ARDQS0_OE_DIS_B2) |
		p_fld(CLEAR_FLD, B2_DQ2_RG_TX_ARDQS0_ODTEN_DIS_B2));

#if 0 /*modified before broadcast*/
	io_32_write_fld_align(DDRPHY_B0_DQ9, CLEAR_FLD,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9, 0x7,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_CA_CMD10, CLEAR_FLD,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
#endif
#if 0
	io_32_write_fld_multi(DDRPHY_CA_CMD2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCMD_OE_DIS) |
		p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCMD_ODTEN_DIS) |
		p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCLK_OE_DIS) |
		p_fld(CLEAR_FLD, CA_CMD2_RG_TX_ARCLK_ODTEN_DIS));
	io_32_write_fld_multi(DDRPHY_B0_DQ2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQ_OE_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQ_ODTEN_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQS_OE_DIS_B0) |
		p_fld(CLEAR_FLD, B0_DQ2_RG_TX_ARDQS_ODTEN_DIS_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQ_OE_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQ_ODTEN_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQS_OE_DIS_B1) |
		p_fld(CLEAR_FLD, B1_DQ2_RG_TX_ARDQS_ODTEN_DIS_B1));
#endif
#if 0
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B0_DQ9_R_IN_GATE_EN_LOW_OPT_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, 0x7,
		B1_DQ9_R_IN_GATE_EN_LOW_OPT_B1);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, 0x7,
		CA_CMD10_R_IN_GATE_EN_LOW_OPT_CA);
#endif
	io_32_write_fld_multi(DDRPHY_MISC_CTRL1,
		p_fld(0x3, MISC_CTRL1_R_DMPINMUX) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMARPIDQ_SW) |
		p_fld(CLEAR_FLD, MISC_CTRL1_R_DMMCTLPLL_CKSEL));
#if 0
	io_32_write_fld_multi(DDRPHY_MISC_CTRL1 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, MISC_CTRL1_R_DMPINMUX) |
		p_fld(CLEAR_FLD, MISC_CTRL1_R_DMARPIDQ_SW) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMMCTLPLL_CKSEL));
#endif
	io_32_write_fld_align(DDRPHY_PLL3, CLEAR_FLD,
		PLL3_RG_RPHYPLL_TSTOP_EN);

	io_32_write_fld_align(DDRPHY_MISC_VREF_CTRL, SET_FLD,
		MISC_VREF_CTRL_RG_RVREF_VREF_EN);
	io_32_write_fld_align(DDRPHY_MISC_VREF_CTRL, 0xfffe,
		MISC_VREF_CTRL_PHY_SPM_CTL4);
	io_32_write_fld_multi(DDRPHY_SHU_MISC0,
		p_fld(CLEAR_FLD, SHU_MISC0_RG_RVREF_SEL_CMD) |
		p_fld(SET_FLD, SHU_MISC0_RG_RVREF_DDR3_SEL) |
		p_fld(CLEAR_FLD, SHU_MISC0_RG_RVREF_DDR4_SEL) |
		p_fld(0xb, SHU_MISC0_RG_RVREF_SEL_DQ));
	io_32_write_fld_align(DDRPHY_MISC_IMP_CTRL0, SET_FLD,
		MISC_IMP_CTRL0_RG_RIMP_DDR3_SEL);
#if 0
	io_32_write_fld_align(DDRPHY_MISC_VREF_CTRL + SHIFT_TO_CHB_ADDR,
		SET_FLD, MISC_VREF_CTRL_RG_RVREF_VREF_EN);
	io_32_write_fld_multi(DDRPHY_SHU_MISC0 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_MISC0_RG_RVREF_SEL_CMD) |
		p_fld(SET_FLD, SHU_MISC0_RG_RVREF_DDR3_SEL) |
		p_fld(CLEAR_FLD, SHU_MISC0_RG_RVREF_DDR4_SEL) |
		p_fld(0xb, SHU_MISC0_RG_RVREF_SEL_DQ));
	io_32_write_fld_align(DDRPHY_MISC_IMP_CTRL0 + SHIFT_TO_CHB_ADDR,
		SET_FLD, MISC_IMP_CTRL0_RG_RIMP_DDR3_SEL);
#endif
	io_32_write_fld_align(DDRPHY_B2_RXDVS0, SET_FLD,
		B2_RXDVS0_R_RX_DLY_TRACK_CG_EN_B2);
	io_32_write_fld_align(DDRPHY_B1_RXDVS0, SET_FLD,
		B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1);
#if 0
	io_32_write_fld_multi(DDRPHY_B0_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0) |
		p_fld(CLEAR_FLD, B0_RXDVS0_R_RX_RANKINCTL_B0));
	io_32_write_fld_multi(DDRPHY_CA_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_RXDVS0_R_RX_DLY_TRACK_CG_EN_CA) |
		p_fld(CLEAR_FLD, CA_RXDVS0_R_RX_RANKINCTL_CA));
	io_32_write_fld_multi(DDRPHY_B2_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B2_RXDVS0_R_RX_DLY_TRACK_CG_EN_B2) |
		p_fld(CLEAR_FLD, B2_RXDVS0_R_RX_RANKINCTL_B2));
	io_32_write_fld_multi(DDRPHY_B1_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1) |
		p_fld(CLEAR_FLD, B1_RXDVS0_R_RX_RANKINCTL_B1));
#endif
	io_32_write_fld_multi(DDRPHY_B2_RXDVS0,
		p_fld(SET_FLD, B2_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B2) |
		p_fld(SET_FLD, B2_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B2));
	io_32_write_fld_multi(DDRPHY_B1_RXDVS0,
		p_fld(SET_FLD, B1_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B1) |
		p_fld(SET_FLD, B1_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B1));
#if 0
	io_32_write_fld_multi(DDRPHY_B0_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B0) |
		p_fld(SET_FLD, B0_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B0));
	io_32_write_fld_multi(DDRPHY_B2_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B2_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B2) |
		p_fld(SET_FLD, B2_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B2));
	io_32_write_fld_multi(DDRPHY_B1_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_RXDVS0_R_DMRXDVS_CNTCMP_OPT_B1) |
		p_fld(SET_FLD, B1_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_B1));
	io_32_write_fld_multi(DDRPHY_CA_RXDVS0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_RXDVS0_R_DMRXDVS_CNTCMP_OPT_CA) |
		p_fld(SET_FLD, CA_RXDVS0_R_DMRXDVS_DQIENPRE_OPT_CA));
#endif

#if 0 /*modified before broadcast*/
	io_32_write_fld_align(DDRPHY_R0_B1_RXDVS2, SET_FLD,
		R0_B1_RXDVS2_R_RK0_DVS_FDLY_MODE_B1);
	io_32_write_fld_align(DDRPHY_R1_B1_RXDVS2, SET_FLD,
		R1_B1_RXDVS2_R_RK1_DVS_FDLY_MODE_B1);
	io_32_write_fld_align(DDRPHY_R0_B0_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R0_B0_RXDVS2_R_RK0_DVS_FDLY_MODE_B0);
	io_32_write_fld_align(DDRPHY_R1_B0_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R1_B0_RXDVS2_R_RK1_DVS_FDLY_MODE_B0);

	io_32_write_fld_align(DDRPHY_R0_CA_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R0_CA_RXDVS2_R_RK0_DVS_FDLY_MODE_CA);
	io_32_write_fld_align(DDRPHY_R1_CA_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R1_CA_RXDVS2_R_RK1_DVS_FDLY_MODE_CA);
	io_32_write_fld_align(DDRPHY_R0_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R0_B1_RXDVS2_R_RK0_DVS_FDLY_MODE_B1);
	io_32_write_fld_align(DDRPHY_R1_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		SET_FLD, R1_B1_RXDVS2_R_RK1_DVS_FDLY_MODE_B1);
#endif
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ5, CLEAR_FLD,
		SHU_B2_DQ5_RG_RX_ARDQS0_DVS_DLY_B2);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ5, CLEAR_FLD,
		SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1);
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ5, CLEAR_FLD,
		SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0);
#if 0

	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(0x5, SHU_B0_DQ5_RG_RX_ARDQS0_DVS_DLY_B0) |
		p_fld(CLEAR_FLD, SHU_B0_DQ5_RG_RX_ARDQ_VREF_SEL_B0));
	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(0x5, SHU_CA_CMD5_RG_RX_ARCLK_DVS_DLY) |
		p_fld(CLEAR_FLD, SHU_CA_CMD5_RG_RX_ARCMD_VREF_SEL));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(0x5, SHU_B2_DQ5_RG_RX_ARDQS0_DVS_DLY_B2) |
		p_fld(CLEAR_FLD, SHU_B2_DQ5_RG_RX_ARDQ_VREF_SEL_B2));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(0x5, SHU_B1_DQ5_RG_RX_ARDQS0_DVS_DLY_B1) |
		p_fld(CLEAR_FLD, SHU_B1_DQ5_RG_RX_ARDQ_VREF_SEL_B1));
#endif

#if 0//cc mark LEGACY_RX_DLY
	LegacyRxDly_LP3(p);
#endif

#if 0
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD2_RG_RK0_RX_ARCA1_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD2_RG_RK0_RX_ARCA1_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD2_RG_RK0_RX_ARCA0_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD2_RG_RK0_RX_ARCA0_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD3 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD3_RG_RK0_RX_ARCA3_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD3_RG_RK0_RX_ARCA3_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD3_RG_RK0_RX_ARCA2_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD3_RG_RK0_RX_ARCA2_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD4_RG_RK0_RX_ARCA5_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD4_RG_RK0_RX_ARCA5_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD4_RG_RK0_RX_ARCA4_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD4_RG_RK0_RX_ARCA4_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE1_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE1_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE0_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD5_RG_RK0_RX_ARCKE0_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD6_RG_RK0_RX_ARCS0_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD6_RG_RK0_RX_ARCS0_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD6_RG_RK0_RX_ARCKE2_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD6_RG_RK0_RX_ARCKE2_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD7 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD7_RG_RK0_RX_ARCS2_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD7_RG_RK0_RX_ARCS2_R_DLY) |
		p_fld(0x12, SHU_R0_CA_CMD7_RG_RK0_RX_ARCS1_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD7_RG_RK0_RX_ARCS1_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD8 + SHIFT_TO_CHB_ADDR,
		p_fld(0x12, SHU_R0_CA_CMD8_RG_RK0_RX_ARCLK_F_DLY) |
		p_fld(0xa, SHU_R0_CA_CMD8_RG_RK0_RX_ARCLK_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD2_RG_RK1_RX_ARCA1_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD2_RG_RK1_RX_ARCA1_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD2_RG_RK1_RX_ARCA0_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD2_RG_RK1_RX_ARCA0_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD3 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD3_RG_RK1_RX_ARCA3_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD3_RG_RK1_RX_ARCA3_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD3_RG_RK1_RX_ARCA2_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD3_RG_RK1_RX_ARCA2_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD4_RG_RK1_RX_ARCA5_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD4_RG_RK1_RX_ARCA5_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD4_RG_RK1_RX_ARCA4_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD4_RG_RK1_RX_ARCA4_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE1_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE1_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE0_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD5_RG_RK1_RX_ARCKE0_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD6_RG_RK1_RX_ARCS0_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD6_RG_RK1_RX_ARCS0_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD6_RG_RK1_RX_ARCKE2_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD6_RG_RK1_RX_ARCKE2_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD7 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD7_RG_RK1_RX_ARCS2_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD7_RG_RK1_RX_ARCS2_R_DLY) |
		p_fld(0x13, SHU_R1_CA_CMD7_RG_RK1_RX_ARCS1_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD7_RG_RK1_RX_ARCS1_R_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD8 + SHIFT_TO_CHB_ADDR,
		p_fld(0x13, SHU_R1_CA_CMD8_RG_RK1_RX_ARCLK_F_DLY) |
		p_fld(0xb, SHU_R1_CA_CMD8_RG_RK1_RX_ARCLK_R_DLY));
#endif

#if 0  /*modified before broadcast*/
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS1,
		p_fld(0x4, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LEAD) |
		p_fld(0x4, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS1,
		p_fld(0x4, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LEAD) |
		p_fld(0x4, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R0_B0_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R0_B0_RXDVS1_R_RK0_B0_DVS_TH_LEAD) |
		p_fld(0x4, R0_B0_RXDVS1_R_RK0_B0_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_B0_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R1_B0_RXDVS1_R_RK1_B0_DVS_TH_LEAD) |
		p_fld(0x4, R1_B0_RXDVS1_R_RK1_B0_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R0_CA_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R0_CA_RXDVS1_R_RK0_CA_DVS_TH_LEAD) |
		p_fld(0x4, R0_CA_RXDVS1_R_RK0_CA_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_CA_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R1_CA_RXDVS1_R_RK1_CA_DVS_TH_LEAD) |
		p_fld(0x4, R1_CA_RXDVS1_R_RK1_CA_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LEAD) |
		p_fld(0x4, R0_B1_RXDVS1_R_RK0_B1_DVS_TH_LAG));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x4, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LEAD) |
		p_fld(0x4, R1_B1_RXDVS1_R_RK1_B1_DVS_TH_LAG));
#endif


	io_32_write_fld_multi(DDRPHY_R0_B2_RXDVS2,
		p_fld(0x2, R0_B2_RXDVS2_R_RK0_DVS_MODE_B2) |
		p_fld(SET_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(SET_FLD, R0_B2_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B2));
	io_32_write_fld_multi(DDRPHY_R1_B2_RXDVS2,
		p_fld(0x2, R1_B2_RXDVS2_R_RK1_DVS_MODE_B2) |
		p_fld(SET_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B2) |
		p_fld(SET_FLD, R1_B2_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B2));
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS2,
		p_fld(0x2, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
		p_fld(SET_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(SET_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS2,
		p_fld(0x2, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1) |
		p_fld(SET_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(SET_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R0_B0_RXDVS2,
		p_fld(0x2, R0_B0_RXDVS2_R_RK0_DVS_MODE_B0) |
		p_fld(SET_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(SET_FLD, R0_B0_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi(DDRPHY_R1_B0_RXDVS2,
		p_fld(0x2, R1_B0_RXDVS2_R_RK1_DVS_MODE_B0) |
		p_fld(SET_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B0) |
		p_fld(SET_FLD, R1_B0_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B0));
	io_32_write_fld_multi(DDRPHY_R0_CA_RXDVS2,
		p_fld(0x2, R0_CA_RXDVS2_R_RK0_DVS_MODE_CA) |
		p_fld(SET_FLD, R0_CA_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_CA) |
		p_fld(SET_FLD, R0_CA_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_CA));
	io_32_write_fld_multi(DDRPHY_R1_CA_RXDVS2,
		p_fld(0x2, R1_CA_RXDVS2_R_RK1_DVS_MODE_CA) |
		p_fld(SET_FLD, R1_CA_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_CA) |
		p_fld(SET_FLD, R1_CA_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_CA));

#if 0 /*modified before broadcast*/
	io_32_write_fld_multi(DDRPHY_R0_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, R0_B1_RXDVS2_R_RK0_DVS_MODE_B1) |
		p_fld(SET_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(SET_FLD, R0_B1_RXDVS2_R_RK0_RX_DLY_FAL_TRACK_GATE_ENA_B1));
	io_32_write_fld_multi(DDRPHY_R1_B1_RXDVS2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, R1_B1_RXDVS2_R_RK1_DVS_MODE_B1) |
		p_fld(SET_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_RIS_TRACK_GATE_ENA_B1) |
		p_fld(SET_FLD, R1_B1_RXDVS2_R_RK1_RX_DLY_FAL_TRACK_GATE_ENA_B1));
#endif
	io_32_write_fld_align(DDRPHY_B2_RXDVS0, CLEAR_FLD,
		B2_RXDVS0_R_RX_DLY_TRACK_CG_EN_B2);
	io_32_write_fld_align(DDRPHY_B1_RXDVS0, CLEAR_FLD,
		B1_RXDVS0_R_RX_DLY_TRACK_CG_EN_B1);
#if 0
	io_32_write_fld_align(DDRPHY_B0_RXDVS0 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		B0_RXDVS0_R_RX_DLY_TRACK_CG_EN_B0);
	io_32_write_fld_align(DDRPHY_CA_RXDVS0 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		CA_RXDVS0_R_RX_DLY_TRACK_CG_EN_CA);
#endif

	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_RG_RX_ARDQ_STBEN_RESETB_B2);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1);
#if 0
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0);
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, SET_FLD,
		CA_CMD10_RG_RX_ARCMD_STBEN_RESETB);
	io_32_write_fld_align(DDRPHY_B2_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B2_DQ9_RG_RX_ARDQ_STBEN_RESETB_B2);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1);
#endif

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL1, CLEAR_FLD,
		MISC_CG_CTRL1_R_DVS_DIV4_CG_CTRL);

	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD9,
		p_fld(0, SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
		p_fld(0, SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK) |
		p_fld(0, SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD9,
		p_fld(0, SHU_R1_CA_CMD9_RG_RK1_ARPI_CMD) |
		p_fld(0, SHU_R1_CA_CMD9_RG_RK1_ARPI_CLK) |
		p_fld(0, SHU_R1_CA_CMD9_RG_RK1_ARPI_CS));
	io_32_write_fld_align(DDRPHY_SHU_R0_B1_DQ7,	0,
		SHU_R0_B1_DQ7_RK0_ARPI_PBYTE_B1);

#if 0//cc mark LEGACY_DELAY_CELL
	LegacyDlyCellInitLP3(p);

	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD1 + SHIFT_TO_CHB_ADDR,
		p_fld(0x6, SHU_R1_CA_CMD1_RK1_TX_ARCKE2_DLY) |
		p_fld(0x6, SHU_R1_CA_CMD1_RK1_TX_ARCKE1_DLY) |
		p_fld(0x6, SHU_R1_CA_CMD1_RK1_TX_ARCKE0_DLY));
#endif

	io_32_write_fld_multi(DDRPHY_SHU_R0_B0_DQ7 ,
		p_fld(0, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
		p_fld(0, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B0_DQ7 ,
		p_fld(0, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0) |
		p_fld(0, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));

	io_32_write_fld_multi(DDRPHY_SHU_R0_B2_DQ7,
		p_fld(0, SHU_R0_B2_DQ7_RK0_ARPI_DQM_B2) |
		p_fld(0, SHU_R0_B2_DQ7_RK0_ARPI_DQ_B2));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B2_DQ7,
		p_fld(0, SHU_R1_B2_DQ7_RK1_ARPI_DQM_B2) |
		p_fld(0, SHU_R1_B2_DQ7_RK1_ARPI_DQ_B2));
	io_32_write_fld_multi(DDRPHY_SHU_R0_B1_DQ7,
		p_fld(0, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
		p_fld(0, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B1_DQ7,
		p_fld(0, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1) |
		p_fld(0, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD9 + SHIFT_TO_CHB_ADDR,
		p_fld(0, SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
		p_fld(0, SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
	io_32_write_fld_multi(DDRPHY_SHU_R1_CA_CMD9 + SHIFT_TO_CHB_ADDR,
		p_fld(0, SHU_R1_CA_CMD9_RG_RK1_ARPI_CMD) |
		p_fld(0, SHU_R1_CA_CMD9_RG_RK1_ARPI_CS));
	io_32_write_fld_multi(DDRPHY_SHU_R0_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(0, SHU_R0_B0_DQ7_RK0_ARPI_DQM_B0) |
		p_fld(0, SHU_R0_B0_DQ7_RK0_ARPI_DQ_B0));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(0, SHU_R1_B0_DQ7_RK1_ARPI_DQM_B0) |
		p_fld(0, SHU_R1_B0_DQ7_RK1_ARPI_DQ_B0));
#endif

#if 0 /*modified before broadcast*/
	io_32_write_fld_multi(DDRPHY_SHU_R0_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(0xf, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
		p_fld(0xf, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	io_32_write_fld_multi(DDRPHY_SHU_R1_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		p_fld(0xf, SHU_R1_B1_DQ7_RK1_ARPI_DQM_B1) |
		p_fld(0xf, SHU_R1_B1_DQ7_RK1_ARPI_DQ_B1));
#endif

	io_32_write_fld_multi(DDRPHY_B2_DQ6,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2));

	io_32_write_fld_multi(DDRPHY_B0_DQ4,
		p_fld(0x10, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0) |
		p_fld(0x10, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ4,
		p_fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1) |
		p_fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1));
	io_32_write_fld_multi(DDRPHY_B2_DQ4,
		p_fld(0x10, B2_DQ4_RG_RX_ARDQS_EYE_R_DLY_B2) |
		p_fld(0x10, B2_DQ4_RG_RX_ARDQS_EYE_F_DLY_B2));
	io_32_write_fld_multi(DDRPHY_CA_CMD4,
		p_fld(0x10, CA_CMD4_RG_RX_ARCLK_EYE_R_DLY) |
		p_fld(0x10, CA_CMD4_RG_RX_ARCLK_EYE_F_DLY));

	io_32_write_fld_multi(DDRPHY_B0_DQ5,
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0) |
		p_fld(CLEAR_FLD, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) |
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0) |
		p_fld(0x0e, B1_DQ5_B1_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B1_DQ5,
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) |
		p_fld(CLEAR_FLD, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1) |
		p_fld(0x0e, B1_DQ5_B1_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B2_DQ5,
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B2) |
		p_fld(CLEAR_FLD, B2_DQ5_RG_RX_ARDQ_EYE_EN_B2) |
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_EYE_SEL_B2) |
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_VREF_EN_B2) |
		p_fld(0x0e, B2_DQ5_B2_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_CA_CMD5,
		p_fld(SET_FLD, CA_CMD5_RG_RX_ARCMD_EYE_STBEN_RESETB) |
		p_fld(CLEAR_FLD, CA_CMD5_RG_RX_ARCMD_EYE_EN) |
		p_fld(SET_FLD, CA_CMD5_RG_RX_ARCMD_EYE_SEL) |
		p_fld(SET_FLD, CA_CMD5_RG_RX_ARCMD_VREF_EN) |
		p_fld(0x0e, CA_CMD5_CA_CMD5_RFU));

	io_32_write_fld_multi(DDRPHY_B1_DQ6,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ5,
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) |
		p_fld(CLEAR_FLD, B1_DQ5_B1_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_CA_CMD3,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_IN_BUFF_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_ARCMD_RESETB) |
		p_fld(SET_FLD, CA_CMD3_RG_TX_ARCMD_EN));
	io_32_write_fld_multi(DDRPHY_CA_CMD6,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_VREF_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_RES_BIAS_EN));
#if NON_EXIST_RG
		p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_SER_MODE));
#endif
	/*
	 * ARCMD_DRVP, DRVN , ARCLK_DRVP, DRVN not used anymore
	 * io_32_write_fld_multi(DDRPHY_SHU_CA_CMD1,
		p_fld(SET_FLD, SHU_CA_CMD1_RG_TX_ARCMD_DRVN) |
	 * | p_fld(SET_FLD, SHU_CA_CMD1_RG_TX_ARCMD_DRVP));
	 * io_32_write_fld_multi(DDRPHY_SHU_CA_CMD2,
		p_fld(SET_FLD, SHU_CA_CMD2_RG_TX_ARCLK_DRVN) |
	 * | p_fld(SET_FLD, SHU_CA_CMD2_RG_TX_ARCLK_DRVP));
	 */
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU2_CA_CMD1,
		p_fld(SET_FLD, SHU2_CA_CMD1_RG_TX_ARCMD_DRVN) |
	 * | p_fld(SET_FLD, SHU2_CA_CMD1_RG_TX_ARCMD_DRVP));
	 * io_32_write_fld_multi(DDRPHY_SHU2_CA_CMD2,
		p_fld(SET_FLD, SHU2_CA_CMD2_RG_TX_ARCLK_DRVN) |
	 * | p_fld(SET_FLD, SHU2_CA_CMD2_RG_TX_ARCLK_DRVP));
	 */
	io_32_write_fld_multi(DDRPHY_B0_DQ3,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_IN_BUFF_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_ARDQ_RESETB_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_TX_ARDQ_EN_B0));
	io_32_write_fld_multi(DDRPHY_B0_DQ6,
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_DDR3_SEL_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0) |
		p_fld(CLEAR_FLD, B0_DQ6_RG_RX_ARDQ_RES_BIAS_EN_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ1,
		p_fld(SET_FLD, SHU_B0_DQ1_RG_TX_ARDQ_DRVN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ1_RG_TX_ARDQ_DRVP_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DQ2,
		p_fld(SET_FLD, SHU_B0_DQ2_RG_TX_ARDQS0_DRVN_B0) |
		p_fld(SET_FLD, SHU_B0_DQ2_RG_TX_ARDQS0_DRVP_B0));
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU2_B0_DQ1,
		p_fld(SET_FLD, SHU2_B0_DQ1_RG_TX_ARDQ_DRVN_B0) |
	 * | p_fld(SET_FLD, SHU2_B0_DQ1_RG_TX_ARDQ_DRVP_B0));
	 * io_32_write_fld_multi(DDRPHY_SHU2_B0_DQ2,
		p_fld(SET_FLD, SHU2_B0_DQ2_RG_TX_ARDQS0_DRVN_B0) |
	 * | p_fld(SET_FLD, SHU2_B0_DQ2_RG_TX_ARDQS0_DRVP_B0));
	 */
#if 0
	io_32_write_fld_multi(DDRPHY_CA_CMD4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x10, CA_CMD4_RG_RX_ARCLK_EYE_R_DLY) |
		p_fld(0x10, CA_CMD4_RG_RX_ARCLK_EYE_F_DLY));
	io_32_write_fld_multi(DDRPHY_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD5_RG_RX_ARCMD_EYE_EN) |
		p_fld(SET_FLD, CA_CMD5_RG_RX_ARCMD_EYE_SEL) |
		p_fld(SET_FLD, CA_CMD5_RG_RX_ARCMD_VREF_EN) |
		p_fld(0x10, CA_CMD5_RG_RX_ARCMD_EYE_VREF_SEL) |
		p_fld(0x10, CA_CMD5_CA_CMD5_RFU));
	io_32_write_fld_multi(DDRPHY_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_EYE_DLY_DQS_BYPASS) |
		p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_DDR3_SEL) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_DDR3_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_TX_ARCMD_DDR4_SEL) |
		p_fld(CLEAR_FLD, CA_CMD6_RG_RX_ARCMD_DDR4_SEL) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_VREF_SEL) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_BIAS_EN) |
		p_fld(SET_FLD, CA_CMD6_RG_RX_ARCMD_OP_BIAS_SW_EN));
#endif
#if 0
		//cc mark p_fld(SET_FLD, CA_CMD6_RG_TX_ARCMD_SER_MODE));
	io_32_write_fld_align(DDRPHY_SHU_CA_CMD6, CLEAR_FLD,
		SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE);

	io_32_write_fld_align(DDRPHY_SHU_CA_CMD6 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE);

	io_32_write_fld_multi(DDRPHY_CA_CMD5 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD5_RG_RX_ARCMD_EYE_STBEN_RESETB) |
		p_fld(CLEAR_FLD, CA_CMD5_CA_CMD5_RFU));
	io_32_write_fld_multi(DDRPHY_B0_DQ4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x10, B0_DQ4_RG_RX_ARDQS_EYE_R_DLY_B0) |
		p_fld(0x10, B0_DQ4_RG_RX_ARDQS_EYE_F_DLY_B0));
#endif

#if 0
	io_32_write_fld_multi(DDRPHY_B0_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0) |
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_EYE_SEL_B0) |
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0) |
		p_fld(0x10, B0_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B0) |
		p_fld(0x10, B0_DQ5_B0_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B0_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_BIAS_EN_B0) |
		p_fld(SET_FLD, B0_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B0));
	io_32_write_fld_multi(DDRPHY_B0_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B0) |
		p_fld(CLEAR_FLD, B0_DQ5_B0_DQ5_RFU));
#endif

	if (get_div_mode(p) == DIV4_MODE) {
		io_32_write_fld_align(DDRPHY_SHU_B0_DQ6, SET_FLD,
			SHU_B0_DQ6_RG_TX_ARDQ_SER_MODE_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ6, SET_FLD,
			SHU_B1_DQ6_RG_TX_ARDQ_SER_MODE_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DQ6, SET_FLD,
			SHU_B2_DQ6_RG_TX_ARDQ_SER_MODE_B2);
#if 0
		io_32_write_fld_align(DDRPHY_SHU_B0_DQ6 + SHIFT_TO_CHB_ADDR, SET_FLD,
			SHU_B0_DQ6_RG_TX_ARDQ_SER_MODE_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ6 + SHIFT_TO_CHB_ADDR, SET_FLD,
			SHU_B1_DQ6_RG_TX_ARDQ_SER_MODE_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DQ6 + SHIFT_TO_CHB_ADDR, SET_FLD,
			SHU_B2_DQ6_RG_TX_ARDQ_SER_MODE_B2);
#endif
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD6, SET_FLD,
			SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE);
#if 0
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD6 + SHIFT_TO_CHB_ADDR,
			SET_FLD, SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE);
#endif
	} else {
		io_32_write_fld_align(DDRPHY_SHU_B0_DQ6, CLEAR_FLD,
			SHU_B0_DQ6_RG_TX_ARDQ_SER_MODE_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ6, CLEAR_FLD,
			SHU_B1_DQ6_RG_TX_ARDQ_SER_MODE_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DQ6, CLEAR_FLD,
			SHU_B2_DQ6_RG_TX_ARDQ_SER_MODE_B2);
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD6, CLEAR_FLD,
			SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE);
#if 0
		io_32_write_fld_align(DDRPHY_SHU_B0_DQ6 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
			SHU_B0_DQ6_RG_TX_ARDQ_SER_MODE_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ6 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
			SHU_B1_DQ6_RG_TX_ARDQ_SER_MODE_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DQ6 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
			SHU_B2_DQ6_RG_TX_ARDQ_SER_MODE_B2);
#endif
	}

#if 0
	io_32_write_fld_multi(DDRPHY_B2_DQ4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x10, B2_DQ4_RG_RX_ARDQS_EYE_R_DLY_B2) |
		p_fld(0x10, B2_DQ4_RG_RX_ARDQS_EYE_F_DLY_B2));
	io_32_write_fld_multi(DDRPHY_B2_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B2_DQ5_RG_RX_ARDQ_EYE_EN_B2) |
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_EYE_SEL_B2) |
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_VREF_EN_B2) |
		p_fld(0x10, B2_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B2) |
		p_fld(0x10, B2_DQ5_B2_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B2_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_BIAS_EN_B2) |
		p_fld(SET_FLD, B2_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B2));
	io_32_write_fld_multi(DDRPHY_B2_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B2_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B2) |
		p_fld(CLEAR_FLD, B2_DQ5_B2_DQ5_RFU));

	io_32_write_fld_multi(DDRPHY_B1_DQ4 + SHIFT_TO_CHB_ADDR,
		p_fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_R_DLY_B1) |
		p_fld(0x10, B1_DQ4_RG_RX_ARDQS_EYE_F_DLY_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_SEL_B1) |
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1) |
		p_fld(0x10, B1_DQ5_RG_RX_ARDQ_EYE_VREF_SEL_B1) |
		p_fld(0x10, B1_DQ5_B1_DQ5_RFU));
	io_32_write_fld_multi(DDRPHY_B1_DQ6 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_VREF_SEL_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_BIAS_EN_B1) |
		p_fld(SET_FLD, B1_DQ6_RG_RX_ARDQ_OP_BIAS_SW_EN_B1));
	io_32_write_fld_multi(DDRPHY_B1_DQ5 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ5_RG_RX_ARDQ_EYE_STBEN_RESETB_B1) |
		p_fld(CLEAR_FLD, B1_DQ5_B1_DQ5_RFU));
#endif

	io_32_write_fld_multi(DDRPHY_PLL4,
		p_fld(CLEAR_FLD, PLL4_RG_RPHYPLL_AD_MCK8X_EN) |
		p_fld(SET_FLD, PLL4_PLL4_RFU) |
		p_fld(SET_FLD, PLL4_RG_RPHYPLL_MCK8X_SEL));

#if 0
	io_32_write_fld_align(DDRPHY_CA_DLL_ARPI1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, CA_DLL_ARPI1_RG_ARPI_CMD_JUMP_EN);
	io_32_write_fld_align(DDRPHY_B0_DLL_ARPI1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, B0_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B0);
#endif

#if 0 /*modified before broadcast*/
	io_32_write_fld_align(DDRPHY_B1_DLL_ARPI1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, B1_DLL_ARPI1_RG_ARPI_DQ_JUMP_EN_B1);
	io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, CA_DLL_ARPI2_RG_ARPI_CG_FB_CA) |
		p_fld(CLEAR_FLD, CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));
	io_32_write_fld_multi(DDRPHY_B0_DLL_ARPI2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B0_DLL_ARPI2_RG_ARPI_CG_FB_B0) |
		p_fld(CLEAR_FLD, B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
	io_32_write_fld_multi(DDRPHY_B1_DLL_ARPI2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, B1_DLL_ARPI2_RG_ARPI_CG_FB_B1) |
		p_fld(CLEAR_FLD, B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));
#endif

	io_32_write_fld_multi(DDRPHY_CA_CMD3,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_STBENCMP_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_ARCMD_RESETB) |
		p_fld(SET_FLD, CA_CMD3_RG_TX_ARCMD_EN) |
		p_fld(CLEAR_FLD, CA_CMD3_RG_RX_ARCMD_SMT_EN));
	io_32_write_fld_multi(DDRPHY_B0_DQ3,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_STBENCMP_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_ARDQ_RESETB_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_TX_ARDQ_EN_B0) |
		p_fld(CLEAR_FLD, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ3,
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_STBENCMP_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_ARDQ_RESETB_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_TX_ARDQ_EN_B1) |
		p_fld(CLEAR_FLD, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1));
	io_32_write_fld_multi(DDRPHY_B2_DQ3,
		p_fld(SET_FLD, B2_DQ3_RG_RX_ARDQ_STBENCMP_EN_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_ARDQ_RESETB_B2) |
		p_fld(SET_FLD, B2_DQ3_RG_TX_ARDQ_EN_B2) |
		p_fld(CLEAR_FLD, B2_DQ3_RG_RX_ARDQ_SMT_EN_B2));
#if 0 /*modified before broadcast*/
	io_32_write_fld_multi(DDRPHY_CA_CMD3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_STBENCMP_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_ARCMD_RESETB) |
		p_fld(SET_FLD, CA_CMD3_RG_TX_ARCMD_EN) |
		p_fld(SET_FLD, CA_CMD3_RG_RX_ARCMD_SMT_EN));
	io_32_write_fld_multi(DDRPHY_B0_DQ3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_STBENCMP_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_ARDQ_RESETB_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_TX_ARDQ_EN_B0) |
		p_fld(SET_FLD, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0));
	io_32_write_fld_multi(DDRPHY_B1_DQ3 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_STBENCMP_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_ARDQ_RESETB_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_TX_ARDQ_EN_B1) |
		p_fld(SET_FLD, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1));
#endif
	if (get_div_mode(p) == DIV4_MODE) {
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL0, CLEAR_FLD,
			SHU_CA_DLL0_RG_ARDLL_DIV_MCTL_CA);
		io_32_write_fld_align(DDRPHY_SHU_B0_DLL0, CLEAR_FLD,
			SHU_B0_DLL0_RG_ARDLL_DIV_MCTL_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DLL0, CLEAR_FLD,
			SHU_B1_DLL0_RG_ARDLL_DIV_MCTL_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DLL0, CLEAR_FLD,
			SHU_B2_DLL0_RG_ARDLL_DIV_MCTL_B2);
	} else {
		io_32_write_fld_align(DDRPHY_SHU_CA_DLL0, 0x2,
			SHU_CA_DLL0_RG_ARDLL_DIV_MCTL_CA);
		io_32_write_fld_align(DDRPHY_SHU_B0_DLL0, 0x2,
			SHU_B0_DLL0_RG_ARDLL_DIV_MCTL_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DLL0, 0x2,
			SHU_B1_DLL0_RG_ARDLL_DIV_MCTL_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DLL0, 0x2,
			SHU_B2_DLL0_RG_ARDLL_DIV_MCTL_B2);
	}
#if 0
	io_32_write_fld_align(DDRPHY_SHU_B1_DLL2, CLEAR_FLD,
		SHU_B1_DLL0_RG_ARDLL_DIV_MCTL_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DLL0, CLEAR_FLD,
		SHU_B2_DLL0_RG_ARDLL_DIV_MCTL_B2);
	io_32_write_fld_align(DDRPHY_SHU_B2_DLL2, CLEAR_FLD,
		SHU_B2_DLL0_RG_ARDLL_DIV_MCTL_B2);
#endif
#if 0 /*modified before broadcast*/
	io_32_write_fld_align(DDRPHY_CA_DLL_ARPI5 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, CA_DLL_ARPI5_RG_ARDLL_DIV_MCTL_CA);
	io_32_write_fld_align(DDRPHY_B0_DLL_ARPI5 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, B0_DLL_ARPI5_RG_ARDLL_DIV_MCTL_B0);
	io_32_write_fld_align(DDRPHY_B1_DLL_ARPI5 + SHIFT_TO_CHB_ADDR,
		CLEAR_FLD, B1_DLL_ARPI5_RG_ARDLL_DIV_MCTL_B1);
#endif

	io_32_write_fld_multi(DDRPHY_SHU_CA_DLL0,
		p_fld(0x9, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA) |
		p_fld(0x8, SHU_CA_DLL0_RG_ARDLL_P_GAIN_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHJUMP_EN_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDIV_CA));
	io_32_write_fld_multi(DDRPHY_SHU_B0_DLL0,
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHDET_IN_SWAP_B0) |
		p_fld(0x7, SHU_B0_DLL0_RG_ARDLL_GAIN_B0) |
		p_fld(0x7, SHU_B0_DLL0_RG_ARDLL_IDLECNT_B0) |
		p_fld(0x8, SHU_B0_DLL0_RG_ARDLL_P_GAIN_B0) |
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHJUMP_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHDIV_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DLL0,
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHDET_IN_SWAP_B1) |
		p_fld(0x7, SHU_B1_DLL0_RG_ARDLL_GAIN_B1) |
		p_fld(0x7, SHU_B1_DLL0_RG_ARDLL_IDLECNT_B1) |
		p_fld(0x8, SHU_B1_DLL0_RG_ARDLL_P_GAIN_B1) |
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHJUMP_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHDIV_B1));
	io_32_write_fld_multi(DDRPHY_SHU_B2_DLL0,
		p_fld(SET_FLD, SHU_B2_DLL0_RG_ARDLL_PHDET_IN_SWAP_B2) |
		p_fld(0x7, SHU_B2_DLL0_RG_ARDLL_GAIN_B2) |
		p_fld(0x7, SHU_B2_DLL0_RG_ARDLL_IDLECNT_B2) |
		p_fld(0x8, SHU_B2_DLL0_RG_ARDLL_P_GAIN_B2) |
		p_fld(SET_FLD, SHU_B2_DLL0_RG_ARDLL_PHJUMP_EN_B2) |
		p_fld(SET_FLD, SHU_B2_DLL0_RG_ARDLL_PHDIV_B2));
#if 0
	io_32_write_fld_multi(DDRPHY_SHU_CA_DLL0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDET_OUT_SEL_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDET_IN_SWAP_CA) |
		p_fld(0x7, SHU_CA_DLL0_RG_ARDLL_GAIN_CA) |
		p_fld(0x7, SHU_CA_DLL0_RG_ARDLL_IDLECNT_CA) |
		p_fld(0x8, SHU_CA_DLL0_RG_ARDLL_P_GAIN_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHJUMP_EN_CA) |
		p_fld(SET_FLD, SHU_CA_DLL0_RG_ARDLL_PHDIV_CA));
#endif

#if 0  /*modified before broadcast*/
	io_32_write_fld_multi(DDRPHY_SHU_B0_DLL0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHDET_IN_SWAP_B0) |
		p_fld(0x7, SHU_B0_DLL0_RG_ARDLL_GAIN_B0) |
		p_fld(0x7, SHU_B0_DLL0_RG_ARDLL_IDLECNT_B0) |
		p_fld(0x8, SHU_B0_DLL0_RG_ARDLL_P_GAIN_B0) |
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHJUMP_EN_B0) |
		p_fld(SET_FLD, SHU_B0_DLL0_RG_ARDLL_PHDIV_B0));
	io_32_write_fld_multi(DDRPHY_SHU_B1_DLL0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHDET_IN_SWAP_B1) |
		p_fld(0x7, SHU_B1_DLL0_RG_ARDLL_GAIN_B1) |
		p_fld(0x7, SHU_B1_DLL0_RG_ARDLL_IDLECNT_B1) |
		p_fld(0x8, SHU_B1_DLL0_RG_ARDLL_P_GAIN_B1) |
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHJUMP_EN_B1) |
		p_fld(SET_FLD, SHU_B1_DLL0_RG_ARDLL_PHDIV_B1));
#endif

	io_32_write_fld_multi(DDRPHY_CA_CMD8,
		p_fld(SET_FLD, CA_CMD8_RG_TX_RRESETB_DDR3_SEL) |
		p_fld(0xa, CA_CMD8_RG_RRESETB_DRVN) |
		p_fld(0xa, CA_CMD8_RG_RRESETB_DRVP));
	io_32_write_fld_multi(DDRPHY_CA_TX_MCK,
		p_fld(CLEAR_FLD, CA_TX_MCK_R_DMRESET_FRPHY_OPT) |
		p_fld(0xa, CA_TX_MCK_R_DMRESETB_DRVP_FRPHY) |
		p_fld(0xa, CA_TX_MCK_R_DMRESETB_DRVN_FRPHY));
#if 0 /*modified before broadcast*/
	io_32_write_fld_multi(DDRPHY_CA_CMD8 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, CA_CMD8_RG_TX_RRESETB_DDR3_SEL) |
		p_fld(SET_FLD, CA_CMD8_RG_RRESETB_DRVN) |
		p_fld(SET_FLD, CA_CMD8_RG_RRESETB_DRVP));
#endif
	delay_us(1);

	io_32_write_fld_align(DDRPHY_SHU_PLL0, 0x3,
		SHU_PLL0_RG_RPHYPLL_TOP_REV);
	/*
	 * io_32_write_fld_align(DDRPHY_SHU2_PLL0, 0x3, SHU2_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU3_PLL0, 0x3, SHU3_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU4_PLL0, 0x3, SHU4_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL0+SHIFT_TO_CHB_ADDR, 0x3, SHU_PLL0_RG_RPHYPLL_TOP_REV); //Use _All() instead
	 * io_32_write_fld_align(DDRPHY_SHU2_PLL0+SHIFT_TO_CHB_ADDR, 0x3, SHU2_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU3_PLL0+SHIFT_TO_CHB_ADDR, 0x3, SHU3_PLL0_RG_RPHYPLL_TOP_REV);
	 * io_32_write_fld_align(DDRPHY_SHU4_PLL0+SHIFT_TO_CHB_ADDR, 0x3, SHU4_PLL0_RG_RPHYPLL_TOP_REV);
	 */
	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD0,
		p_fld(CLEAR_FLD, SHU_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
		p_fld(0x7, SHU_CA_CMD0_RG_TX_ARCLK_DRVN_PRE) |
		p_fld(CLEAR_FLD, SHU_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU2_CA_CMD0,
		p_fld(SET_FLD, SHU2_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU2_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU2_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 * io_32_write_fld_multi(DDRPHY_SHU3_CA_CMD0,
		p_fld(SET_FLD, SHU3_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU3_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU3_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 * io_32_write_fld_multi(DDRPHY_SHU4_CA_CMD0,
		p_fld(SET_FLD, SHU4_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU4_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU4_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 */
#if 0 /*modified before broadcast*/
	io_32_write_fld_multi(DDRPHY_SHU_CA_CMD0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
		p_fld(0x7, SHU_CA_CMD0_RG_TX_ARCLK_DRVN_PRE) |
		p_fld(SET_FLD, SHU_CA_CMD0_RG_TX_ARCLK_PRE_EN));
#endif
	/*
	 * io_32_write_fld_multi(DDRPHY_SHU2_CA_CMD0+SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU2_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU2_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU2_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 * io_32_write_fld_multi(DDRPHY_SHU3_CA_CMD0+SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU3_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU3_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU3_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 * io_32_write_fld_multi(DDRPHY_SHU4_CA_CMD0+SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, SHU4_CA_CMD0_RG_TX_ARCMD_PRE_EN) |
	 * | p_fld(0x7, SHU4_CA_CMD0_RG_TX_ARCLK_DRVN_PRE)
	 * | p_fld(SET_FLD, SHU4_CA_CMD0_RG_TX_ARCLK_PRE_EN));
	 */
	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_multi(DDRPHY_MISC_SHU_OPT,
		p_fld(0x2, MISC_SHU_OPT_R_DQB2_SHU_PHDET_SPM_EN) |
		p_fld(0x1, MISC_SHU_OPT_R_DQB2_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB1_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB0_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN));

	io_32_write_fld_multi(DDRPHY_MISC_SHU_OPT + SHIFT_TO_CHB_ADDR,
		p_fld(0x2, MISC_SHU_OPT_R_DQB2_SHU_PHDET_SPM_EN) |
		p_fld(0x1, MISC_SHU_OPT_R_DQB2_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_CA_SHU_PHDET_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB1_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB1_SHU_PHY_GATING_RESETB_SPM_EN) |
		p_fld(0x2, MISC_SHU_OPT_R_DQB0_SHU_PHDET_SPM_EN) |
		p_fld(SET_FLD, MISC_SHU_OPT_R_DQB0_SHU_PHY_GATING_RESETB_SPM_EN));
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	delay_us(1);

	io_32_write_fld_align(DDRPHY_PLL3, CLEAR_FLD,
		PLL3_RG_RPHYPLL_TSTOP_EN);

	io_32_write_fld_multi(DDRPHY_SHU_PLL9,
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_MONCK_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_MONVC_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_LVROD_EN) |
		p_fld(SET_FLD, SHU_PLL9_RG_RPHYPLL_RST_DLY));
	io_32_write_fld_multi(DDRPHY_SHU_PLL11,
		p_fld(CLEAR_FLD, SHU_PLL11_RG_RCLRPLL_MONCK_EN) |
		p_fld(CLEAR_FLD, SHU_PLL11_RG_RCLRPLL_MONVC_EN) |
		p_fld(CLEAR_FLD, SHU_PLL11_RG_RCLRPLL_LVROD_EN) |
		p_fld(SET_FLD, SHU_PLL11_RG_RCLRPLL_RST_DLY));
#if 0 /*modified before broadcast*/
	io_32_write_fld_multi(DDRPHY_SHU_PLL9 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_MONCK_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_MONVC_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_LVROD_EN) |
		p_fld(CLEAR_FLD, SHU_PLL9_RG_RPHYPLL_RST_DLY));
#endif
#if 0 /*modified before broadcast*/
	io_32_write_fld_align(DDRPHY_SHU_PLL5, CLEAR_FLD,
		SHU_PLL5_RG_RPHYPLL_SDM_FRA_EN);
	io_32_write_fld_align(DDRPHY_SHU_PLL7, CLEAR_FLD,
		SHU_PLL7_RG_RCLRPLL_SDM_FRA_EN);
	io_32_write_fld_align(DDRPHY_SHU_PLL5 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		SHU_PLL5_RG_RPHYPLL_SDM_FRA_EN);
	io_32_write_fld_align(DDRPHY_SHU_PLL7 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		SHU_PLL7_RG_RCLRPLL_SDM_FRA_EN);
#endif
	/*
	 * io_32_write_fld_align_all(DDRPHY_SHU_PLL4, 0xfe, SHU_PLL4_RG_RPHYPLL_RESERVED);
	 * io_32_write_fld_align_all(DDRPHY_SHU_PLL6, 0xfe, SHU_PLL6_RG_RCLRPLL_RESERVED);
	 */
#if 0 /*modified before broadcast*/
	io_32_write_fld_align(DDRPHY_SHU_PLL4 + SHIFT_TO_CHB_ADDR, 0xfe,
		SHU_PLL4_RG_RPHYPLL_RESERVED);
	io_32_write_fld_align(DDRPHY_SHU_PLL6 + SHIFT_TO_CHB_ADDR, 0xfe,
		SHU_PLL6_RG_RCLRPLL_RESERVED);
#endif
	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_multi(DDRPHY_SHU_CA_DLL1,
		p_fld(SET_FLD, SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA) |
		p_fld(CLEAR_FLD, SHU_CA_DLL1_RG_ARDLL_FASTPJ_CK_SEL_CA));
	io_32_write_fld_multi(DDRPHY_SHU_CA_DLL1 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA) |
		p_fld(SET_FLD, SHU_CA_DLL1_RG_ARDLL_FASTPJ_CK_SEL_CA));
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);
	if (get_div_mode(p) == DIV4_MODE) {
		io_32_write_fld_multi(DDRPHY_SHU_CA_DLL1,
			p_fld(CLEAR_FLD, SHU_CA_DLL1_RG_ARDLL_PD_CK_SEL_CA) |
			p_fld(SET_FLD, SHU_CA_DLL1_RG_ARDLL_FASTPJ_CK_SEL_CA));
	}

	delay_us(1);

	io_32_write_fld_align(DDRPHY_SHU_B0_DLL1, SET_FLD,
		SHU_B0_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DLL1, SET_FLD,
		SHU_B1_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DLL1, SET_FLD,
		SHU_B2_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B2);

#if 0 /*modified before broadcast*/
	io_32_write_fld_align(DDRPHY_SHU_B0_DLL1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, SHU_B0_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DLL1 + SHIFT_TO_CHB_ADDR,
		SET_FLD, SHU_B1_DLL1_RG_ARDLL_FASTPJ_CK_SEL_B1);
#endif
	delay_us(1);

	/*
	 * io_32_write_fld_align(DDRPHY_PLL2, CLEAR_FLD, PLL2_RG_RCLRPLL_EN);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL4, 0xff, SHU_PLL4_RG_RPHYPLL_RESERVED);
	 * io_32_write_fld_align(DDRPHY_SHU_PLL6, 0xff, SHU_PLL6_RG_RCLRPLL_RESERVED);
	 */
#if 0
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0 + SHIFT_TO_CHB_ADDR,
		0x100000, MISC_CG_CTRL0_CLK_MEM_DFS_CFG);
#endif

	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, 0xf,
		MISC_CG_CTRL0_CLK_MEM_DFS_CFG);
	io_32_write_fld_align(DDRPHY_MISC_CG_CTRL0, SET_FLD,
		MISC_CG_CTRL0_RG_CG_EMI_OFF_DISABLE);

	delay_us(1);

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	ddrphy_rsv_rg_setting(p);
	ddrphy_pll_setting(p);
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

#if ENABLE_TMRRI_NEW_MODE

	io_32_write_fld_align(DRAMC_REG_RKCFG, 0, RKCFG_TXRANK);
	io_32_write_fld_align(DRAMC_REG_RKCFG, 0, RKCFG_TXRANKFIX);
	io_32_write_fld_align(DRAMC_REG_DRSCTRL, CLEAR_FLD,
		DRSCTRL_RK_SCINPUT_OPT);
	io_32_write_fld_align(DRAMC_REG_DRSCTRL, SET_FLD,
		DRSCTRL_DRSDIS);
	io_32_write_fld_align(DRAMC_REG_DRSCTRL, 0x8,
		DRSCTRL_DRSDLY);
	io_32_write_fld_align(DRAMC_REG_DRSCTRL, SET_FLD,
		DRSCTRL_DRS_SELFWAKE_DMYRD_DIS);
	io_32_write_fld_multi(DRAMC_REG_DRAMCTRL,
		p_fld(CLEAR_FLD, DRAMCTRL_MRRIOPT) |
		p_fld(CLEAR_FLD, DRAMCTRL_TMRRIBYRK_DIS) |
		p_fld(SET_FLD, DRAMCTRL_TMRRICHKDIS));

	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, CLEAR_FLD,
		SPCMDCTRL_SC_PG_UPD_OPT);
	io_32_write_fld_multi(DRAMC_REG_SPCMDCTRL,
		p_fld(SET_FLD, SPCMDCTRL_SRFMR4_CNTKEEP_B) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_MPRW_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_STCMD_AREF_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_OPT2_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_MAN_DIS));

	io_32_write_fld_align(DRAMC_REG_RKCFG, 0, RKCFG_TXRANK);
	io_32_write_fld_align(DRAMC_REG_RKCFG, 0, RKCFG_TXRANKFIX);
#else
	io_32_write_fld_align(DRAMC_REG_DRSCTRL, SET_FLD,
		DRSCTRL_RK_SCINPUT_OPT);
	io_32_write_fld_multi(DRAMC_REG_DRAMCTRL,
		p_fld(SET_FLD, DRAMCTRL_MRRIOPT) |
		p_fld(SET_FLD, DRAMCTRL_TMRRIBYRK_DIS) |
		p_fld(CLEAR_FLD, DRAMCTRL_TMRRICHKDIS));
#if NON_EXIST_RG
	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, CLEAR_FLD,
		SPCMDCTRL_SC_PG_UPD_OPT);
	io_32_write_fld_multi(DRAMC_REG_SPCMDCTRL,
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_MPRW_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_STCMD_AREF_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_OPT2_DIS) |
		p_fld(SET_FLD, SPCMDCTRL_SC_PG_MAN_DIS));
#endif
#endif

	io_32_write_fld_multi(DRAMC_REG_MPC_OPTION,
		p_fld(CLEAR_FLD, MPC_OPTION_ZQ_BLOCKALE_OPT) |
		p_fld(CLEAR_FLD, MPC_OPTION_MPC_BLOCKALE_OPT2) |
		p_fld(CLEAR_FLD, MPC_OPTION_MPC_BLOCKALE_OPT1) |
		p_fld(CLEAR_FLD, MPC_OPTION_MPC_BLOCKALE_OPT));

	io_32_write_fld_align(DRAMC_REG_CKECTRL, CLEAR_FLD,
		CKECTRL_RUNTIMEMRRCKEFIX);
	io_32_write_fld_align(DRAMC_REG_CKECTRL, CLEAR_FLD,
		CKECTRL_RUNTIMEMRRMIODIS);

	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B2);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1);
#if 0
	io_32_write_fld_align(DDRPHY_CA_CMD10 + SHIFT_TO_CHB_ADDR, SET_FLD,
		CA_CMD10_RG_RX_ARCLK_STBEN_RESETB);
	io_32_write_fld_align(DDRPHY_B0_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ9 + SHIFT_TO_CHB_ADDR, SET_FLD,
		B2_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B2);
#endif

	io_32_write_fld_multi(DDRPHY_MISC_CTRL1,
		p_fld(SET_FLD, MISC_CTRL1_R_DMDA_RRESETB_E) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMDQSIENCG_EN) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMARPIDQ_SW) |
		p_fld(SET_FLD, MISC_CTRL1_R_DM_TX_ARCMD_OE) |
		p_fld(SET_FLD, MISC_CTRL1_R_DM_TX_ARCLK_OE));

	if (get_div_mode(p) == DIV4_MODE) {
		io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
			p_fld(CLEAR_FLD, SHU_B0_DQ7_MIDPI_ENABLE) |
			p_fld(SET_FLD, SHU_B0_DQ7_MIDPI_DIV4_ENABLE) |
			p_fld(SET_FLD, SHU_B0_DQ7_R_DMRDSEL_DIV2_OPT) |
			p_fld(SET_FLD, SHU_B0_DQ7_R_DMRDSEL_LOBYTE_OPT));
	} else {
		io_32_write_fld_multi(DDRPHY_SHU_B0_DQ7,
			p_fld(SET_FLD, SHU_B0_DQ7_MIDPI_ENABLE) |
			p_fld(CLEAR_FLD, SHU_B0_DQ7_MIDPI_DIV4_ENABLE) |
			p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRDSEL_DIV2_OPT) |
			p_fld(CLEAR_FLD, SHU_B0_DQ7_R_DMRDSEL_LOBYTE_OPT));
	}
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ7, CLEAR_FLD,
		SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ7, CLEAR_FLD,
		SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1);
	io_32_write_fld_align(DDRPHY_SHU_B2_DQ7, CLEAR_FLD,
		SHU_B2_DQ7_R_DMRXTRACK_DQM_EN_B2);
	io_32_write_fld_align(DDRPHY_B0_RXDVS0, 1,
		B0_RXDVS0_R_HWSAVE_MODE_ENA_B0);
	io_32_write_fld_align(DDRPHY_B1_RXDVS0, 1,
		B1_RXDVS0_R_HWSAVE_MODE_ENA_B1);
	io_32_write_fld_align(DDRPHY_B2_RXDVS0, 1,
		B2_RXDVS0_R_HWSAVE_MODE_ENA_B2);
	io_32_write_fld_align(DDRPHY_CA_RXDVS0, 1,
		CA_RXDVS0_R_HWSAVE_MODE_ENA_CA);
#if 0
	io_32_write_fld_multi(DDRPHY_MISC_CTRL1 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, MISC_CTRL1_R_DMDA_RRESETB_E) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMDQSIENCG_EN) |
		p_fld(SET_FLD, MISC_CTRL1_R_DMARPIDQ_SW) |
		p_fld(SET_FLD, MISC_CTRL1_R_DM_TX_ARCMD_OE) |
		p_fld(SET_FLD, MISC_CTRL1_R_DM_TX_ARCLK_OE));
#endif

#if 0 /*modified before broadcast*/
	io_32_write_fld_align(DDRPHY_SHU_B0_DQ7 + SHIFT_TO_CHB_ADDR,
		SHU_B0_DQ7_R_DMRXTRACK_DQM_EN_B0);
	io_32_write_fld_align(DDRPHY_SHU_B1_DQ7 + SHIFT_TO_CHB_ADDR,
		SHU_B1_DQ7_R_DMRXTRACK_DQM_EN_B1);
	io_32_write_fld_align(DDRPHY_B0_RXDVS0 + SHIFT_TO_CHB_ADDR, 1,
		B0_RXDVS0_R_HWSAVE_MODE_ENA_B0);
	io_32_write_fld_align(DDRPHY_B1_RXDVS0 + SHIFT_TO_CHB_ADDR, 1,
		B1_RXDVS0_R_HWSAVE_MODE_ENA_B1);
	io_32_write_fld_align(DDRPHY_CA_RXDVS0 + SHIFT_TO_CHB_ADDR, 1,
		CA_RXDVS0_R_HWSAVE_MODE_ENA_CA);
#endif
	io_32_write_fld_multi(DDRPHY_CA_CMD8,
		p_fld(CLEAR_FLD, CA_CMD8_RG_TX_RRESETB_PULL_DN) |
		p_fld(CLEAR_FLD, CA_CMD8_RG_TX_RRESETB_DDR3_SEL));

	io_32_write_fld_align(DDRPHY_CA_CMD7, CLEAR_FLD,
		CA_CMD7_RG_TX_ARCMD_PULL_DN);
	io_32_write_fld_align(DDRPHY_B0_DQ7, CLEAR_FLD,
		B0_DQ7_RG_TX_ARDQ_PULL_DN_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ7, CLEAR_FLD,
		B1_DQ7_RG_TX_ARDQ_PULL_DN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ7, CLEAR_FLD,
		B2_DQ7_RG_TX_ARDQ_PULL_DN_B2);
#if 0
	io_32_write_fld_align(DDRPHY_CA_CMD7 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		CA_CMD7_RG_TX_ARCMD_PULL_DN);
	io_32_write_fld_align(DDRPHY_B0_DQ7 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		B0_DQ7_RG_TX_ARDQ_PULL_DN_B0);
	io_32_write_fld_align(DDRPHY_B1_DQ7 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		B1_DQ7_RG_TX_ARDQ_PULL_DN_B1);
	io_32_write_fld_align(DDRPHY_B2_DQ7 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		B2_DQ7_RG_TX_ARDQ_PULL_DN_B2);
#endif

	io_32_write_fld_multi(DRAMC_REG_SHU_DQSG_RETRY,
		p_fld(SET_FLD, SHU_DQSG_RETRY_R_RDY_SEL_DLE) |
		p_fld(0x6, SHU_DQSG_RETRY_R_DQSIENLAT) |
		p_fld(SET_FLD, SHU_DQSG_RETRY_R_RETRY_ONCE));

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_multi(DRAMC_REG_SHUCTRL2,
		p_fld(CLEAR_FLD, SHUCTRL2_HWSET_WLRL) |
		p_fld(SET_FLD, SHUCTRL2_SHU_PERIOD_GO_ZERO_CNT) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_OPTION) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_PARK_N) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_DLL_CHA) |
		p_fld(0xa, SHUCTRL2_R_DLL_IDLE));
	io_32_write_fld_multi(DRAMC_REG_SHUCTRL2 + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, SHUCTRL2_HWSET_WLRL) |
		p_fld(SET_FLD, SHUCTRL2_SHU_PERIOD_GO_ZERO_CNT) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_OPTION) |
		p_fld(SET_FLD, SHUCTRL2_R_DVFS_PARK_N) |
		p_fld(0xa, SHUCTRL2_R_DLL_IDLE));
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	io_32_write_fld_align(DRAMC_REG_DVFSDLL, SET_FLD,
		DVFSDLL_DLL_LOCK_SHU_EN);
	io_32_write_fld_multi(DRAMC_REG_DDRCONF0,
		p_fld(SET_FLD, DDRCONF0_DDR4EN) |
#if NON_EXIST_RG
		p_fld(SET_FLD, DDRCONF0_DM64BITEN) |
#endif
		p_fld(CLEAR_FLD, DDRCONF0_BK8EN) |
		p_fld(SET_FLD, DDRCONF0_BC4OTF_OPT) |
		p_fld(CLEAR_FLD, DDRCONF0_BC4OTF) |
		p_fld(SET_FLD, DDRCONF0_GDDR3RST) |
		p_fld(SET_FLD, DDRCONF0_DDR34_2TCMDEN));

	io_32_write_fld_multi(DRAMC_REG_STBCAL2,
		p_fld(SET_FLD, STBCAL2_STB_GERR_B23) |
		p_fld(SET_FLD, STBCAL2_DQSGCNT_BYP_REF) |
		p_fld(SET_FLD, STBCAL2_STB_PICG_EARLY_1T_EN) |
		p_fld(SET_FLD, STBCAL2_STB_GERR_B01) |
		p_fld(SET_FLD, STBCAL2_STB_GERRSTOP));
	io_32_write_fld_align(DRAMC_REG_STBCAL2, SET_FLD,
		STBCAL2_STB_GERR_RST);
	io_32_write_fld_align(DRAMC_REG_STBCAL2, CLEAR_FLD,
		STBCAL2_STB_GERR_RST);
	io_32_write_fld_align(DRAMC_REG_SHU_WODT, CLEAR_FLD,
		SHU_WODT_WPST1P5T);
	io_32_write_fld_align(DRAMC_REG_CLKAR, CLEAR_FLD, CLKAR_PSELAR);
	io_32_write_fld_multi(DDRPHY_MISC_CTRL0,
		p_fld(SET_FLD, MISC_CTRL0_R_DQS1IEN_DIV4_CK_CG_CTRL) |
		p_fld(SET_FLD, MISC_CTRL0_R_DQS0IEN_DIV4_CK_CG_CTRL) |
		p_fld(SET_FLD, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
		p_fld(SET_FLD, MISC_CTRL0_IMPCAL_LP_ECO_OPT) |
		p_fld(SET_FLD, MISC_CTRL0_R_DMDQSIEN_DEPTH_HALF) |
		p_fld(SET_FLD, MISC_CTRL0_R_DMSTBEN_OUTSEL) |
		p_fld(0, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	io_32_write_fld_align(DDRPHY_MISC_CTRL0, SET_FLD,
		MISC_CTRL0_IMPCAL_CHAB_EN);
	io_32_write_fld_align(DDRPHY_MISC_CTRL0 + SHIFT_TO_CHB_ADDR, CLEAR_FLD,
		MISC_CTRL0_IMPCAL_CHAB_EN);
	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);
#if 0
	io_32_write_fld_multi(DDRPHY_MISC_CTRL0 + SHIFT_TO_CHB_ADDR,
		p_fld(SET_FLD, MISC_CTRL0_R_DQS1IEN_DIV4_CK_CG_CTRL) |
		p_fld(SET_FLD, MISC_CTRL0_R_CLKIEN_DIV4_CK_CG_CTRL) |
		p_fld(SET_FLD, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
		p_fld(SET_FLD, MISC_CTRL0_R_DMDQSIEN_DEPTH_HALF) |
		p_fld(SET_FLD, MISC_CTRL0_R_DMSTBEN_OUTSEL) |
		p_fld(0, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));
#endif
	if (get_div_mode(p) == DIV4_MODE) {
		io_32_write_fld_multi(DRAMC_REG_SHU_CONF0,
			p_fld(0x2, SHU_CONF0_MATYPE) |
			p_fld(SET_FLD, SHU_CONF0_BL4) |
			p_fld(SET_FLD, SHU_CONF0_FDIV2) |
			p_fld(CLEAR_FLD, SHU_CONF0_FREQDIV4) |
			p_fld(CLEAR_FLD, SHU_CONF0_DUALSCHEN) |
			p_fld(CLEAR_FLD, SHU_CONF0_DM64BITEN) |
			p_fld(SET_FLD, SHU_CONF0_WDATRGO) |
			p_fld(SET_FLD, SHU_CONF0_REFTHD) |
			p_fld(0x8, SHU_CONF0_REQQUE_DEPTH) |
			p_fld(CLEAR_FLD, SHU_CONF0_RDATDIV2) |
			p_fld(SET_FLD, SHU_CONF0_RDATDIV4) |
			p_fld(SET_FLD, SHU_CONF0_ADVPREEN) |
			p_fld(0x3f, SHU_CONF0_DMPGTIM));

	} else {
		io_32_write_fld_multi(DRAMC_REG_SHU_CONF0,
			p_fld(0x2, SHU_CONF0_MATYPE) |
			p_fld(SET_FLD, SHU_CONF0_BL4) |
			p_fld(SET_FLD, SHU_CONF0_DM64BITEN) |
			p_fld(CLEAR_FLD, SHU_CONF0_FDIV2) |
			p_fld(SET_FLD, SHU_CONF0_FREQDIV4) |
			p_fld(CLEAR_FLD, SHU_CONF0_DUALSCHEN) |
			p_fld(SET_FLD, SHU_CONF0_WDATRGO) |
			p_fld(SET_FLD, SHU_CONF0_REFTHD) |
			p_fld(0x8, SHU_CONF0_REQQUE_DEPTH) |
			p_fld(CLEAR_FLD, SHU_CONF0_RDATDIV2) |
			p_fld(SET_FLD, SHU_CONF0_RDATDIV4) |
			p_fld(SET_FLD, SHU_CONF0_ADVPREEN) |
			p_fld(0x3f, SHU_CONF0_DMPGTIM));
	}
	if (get_div_mode(p) == DIV4_MODE) {
		if (p->odt_onoff == ODT_ON) {
			io_32_write_fld_multi(DRAMC_REG_SHU_ODTCTRL,
				p_fld(0x3, SHU_ODTCTRL_TWODT) |
				p_fld(0x5, SHU_ODTCTRL_RODT) |
				p_fld(SET_FLD, SHU_ODTCTRL_WOEN));
		} else {
			io_32_write_fld_multi(DRAMC_REG_SHU_ODTCTRL,
				p_fld(0x3, SHU_ODTCTRL_TWODT) |
				p_fld(0x5, SHU_ODTCTRL_RODT) |
				p_fld(CLEAR_FLD, SHU_ODTCTRL_WOEN));
		}
	} else if (p->odt_onoff == ODT_ON){
		io_32_write_fld_multi(DRAMC_REG_SHU_ODTCTRL,
			p_fld(0x3, SHU_ODTCTRL_TWODT) |
			p_fld(SET_FLD, SHU_ODTCTRL_WOEN));
	} else {
		io_32_write_fld_multi(DRAMC_REG_SHU_ODTCTRL,
			p_fld(0x3, SHU_ODTCTRL_TWODT) |
			p_fld(CLEAR_FLD, SHU_ODTCTRL_WOEN));
	}
	io_32_write_fld_align(DRAMC_REG_REFCTRL0, 0x5,
		REFCTRL0_REF_PREGATE_CNT);
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA1,
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CS1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_RAS) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CAS) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_WE) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_RESET) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_ODT) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CKE) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA1_TXDLY_CS));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA2,
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_CKE1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_CMD) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_BA2) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_BA1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA2_TXDLY_BA0));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA3,
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA7) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA6) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA5) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA4) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA3) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA2) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA1) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA3_TXDLY_RA0));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_CA4,
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA15) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA14) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA13) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA12) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA11) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA10) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA9) |
		p_fld(CLEAR_FLD, SHU_SELPH_CA4_TXDLY_RA8));
	io_32_write_fld_align(DRAMC_REG_SHU_SELPH_CA5, CLEAR_FLD,
		SHU_SELPH_CA5_DLY_ODT);

	/*2T mode*/
	io32_write_4b(DRAMC_REG_SHU_SELPH_CA5, 0x10001111);
	io32_write_4b(DRAMC_REG_SHU_SELPH_CA6, 0x01000000);
	io32_write_4b(DRAMC_REG_SHU_SELPH_CA7, 0x00000000);
	io32_write_4b(DRAMC_REG_SHU_SELPH_CA8, 0x00000000);

	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_DQS0,
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS3) |
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS2) |
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS1) |
		p_fld(reg_txdly_dqs_oen, SHU_SELPH_DQS0_TXDLY_OEN_DQS0) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS3) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS2) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS1) |
		p_fld(reg_txdly_dqs, SHU_SELPH_DQS0_TXDLY_DQS0));
	io_32_write_fld_multi(DRAMC_REG_SHU_SELPH_DQS1,
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS3) |
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS2) |
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS1) |
		p_fld(reg_dly_dqs_oen, SHU_SELPH_DQS1_DLY_OEN_DQS0) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS3) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS2) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS1) |
		p_fld(reg_dly_dqs, SHU_SELPH_DQS1_DLY_DQS0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ0,
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ3) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ2) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ1) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ0_TXDLY_OEN_DQ0) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ3) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ2) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ1) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ0_TXDLY_DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ1,
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM3) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM2) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM1) |
		p_fld(reg_txdly_dqdqm_oen, SHURK0_SELPH_DQ1_TXDLY_OEN_DQM0) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM3) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM2) | /* MCK = 8UI */
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM1) |
		p_fld(reg_txdly_dqdqm, SHURK0_SELPH_DQ1_TXDLY_DQM0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ2,
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ2_DLY_OEN_DQ3) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ2_DLY_OEN_DQ2) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ2_DLY_OEN_DQ1) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ2_DLY_OEN_DQ0) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ2_DLY_DQ3) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ2_DLY_DQ2) | /* ui */
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ2_DLY_DQ1) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ2_DLY_DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK0_SELPH_DQ3,
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ3_DLY_OEN_DQM3) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ3_DLY_OEN_DQM2) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ3_DLY_OEN_DQM1) |
		p_fld(reg_dly_dqdqm_oen, SHURK0_SELPH_DQ3_DLY_OEN_DQM0) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ3_DLY_DQM3) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ3_DLY_DQM2) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ3_DLY_DQM1) |
		p_fld(reg_dly_dqdqm, SHURK0_SELPH_DQ3_DLY_DQM0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ0,
		p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ3) |
		p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ2) |
		p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ1) |
		p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1OEN_DQ0) |
		p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1DQ3) |
		p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1DQ2) |
		p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1DQ1) |
		p_fld(0x2, SHURK1_SELPH_DQ0_TX_DLY_R1DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ1,
		p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM3) |
		p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM2) |
		p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM1) |
		p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1OEN_DQM0) |
		p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1DQM3) |
		p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1DQM2) |
		p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1DQM1) |
		p_fld(0x2, SHURK1_SELPH_DQ1_TX_DLY_R1DQM0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ2,
		p_fld(SET_FLD, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ3) |
		p_fld(SET_FLD, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ2) |
		p_fld(SET_FLD, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ1) |
		p_fld(SET_FLD, SHURK1_SELPH_DQ2_DLY_R1OEN_DQ0) |
		p_fld(0x3, SHURK1_SELPH_DQ2_DLY_R1DQ3) |
		p_fld(0x3, SHURK1_SELPH_DQ2_DLY_R1DQ2) |
		p_fld(0x3, SHURK1_SELPH_DQ2_DLY_R1DQ1) |
		p_fld(0x3, SHURK1_SELPH_DQ2_DLY_R1DQ0));
	io_32_write_fld_multi(DRAMC_REG_SHURK1_SELPH_DQ3,
		p_fld(SET_FLD, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM3) |
		p_fld(SET_FLD, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM2) |
		p_fld(SET_FLD, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM1) |
		p_fld(SET_FLD, SHURK1_SELPH_DQ3_DLY_R1OEN_DQM0) |
		p_fld(0x3, SHURK1_SELPH_DQ3_DLY_R1DQM3) |
		p_fld(0x3, SHURK1_SELPH_DQ3_DLY_R1DQM2) |
		p_fld(0x3, SHURK1_SELPH_DQ3_DLY_R1DQM1) |
		p_fld(0x3, SHURK1_SELPH_DQ3_DLY_R1DQM0));
	delay_us(1);

	io_32_write_fld_align(DDRPHY_B2_DQ9, SET_FLD,
		B2_DQ9_RG_RX_ARDQS0_DQSIENMODE_B2);
	io_32_write_fld_align(DDRPHY_B1_DQ9, SET_FLD,
		B1_DQ9_RG_RX_ARDQS0_DQSIENMODE_B1);
	io_32_write_fld_align(DDRPHY_B0_DQ9, SET_FLD,
		B0_DQ9_RG_RX_ARDQS0_DQSIENMODE_B0);

	io_32_write_fld_multi(DRAMC_REG_STBCAL,
		p_fld(SET_FLD, STBCAL_DQSIENMODE_SELPH) |
		p_fld(SET_FLD, STBCAL_STB_DQIEN_IG) |
		p_fld(SET_FLD, STBCAL_PICHGBLOCK_NORD) |
		p_fld(SET_FLD, STBCAL_PIMASK_RKCHG_OPT));
	io_32_write_fld_multi(DRAMC_REG_SHU_DQSG,
		p_fld(0x9, SHU_DQSG_STB_UPDMASKCYC) |
		p_fld(SET_FLD, SHU_DQSG_STB_UPDMASK_EN));
	io_32_write_fld_align(DRAMC_REG_STBCAL, SET_FLD, STBCAL_DQSIENMODE);
	io_32_write_fld_multi(DRAMC_REG_SREFCTRL,
		p_fld(SET_FLD, SREFCTRL_SREF_HW_EN) |
		p_fld(0x8, SREFCTRL_SREFDLY) |
		p_fld(CLEAR_FLD, SREFCTRL_SREF2_OPTION));
	io_32_write_fld_multi(DRAMC_REG_SHU_PIPE,
		p_fld(SET_FLD, SHU_PIPE_READ_START_EXTEND1) |
		p_fld(SET_FLD, SHU_PIPE_DLE_LAST_EXTEND1));

	io_32_write_fld_multi(DRAMC_REG_CKECTRL,
		p_fld(SET_FLD, CKECTRL_CKETIMER_SEL) |
		p_fld(0x4, CKECTRL_CKELCKCNT) |
		p_fld(SET_FLD, CKECTRL_CKEON));
#if NON_EXIST_RG
	io_32_write_fld_multi(DRAMC_REG_CKECTRL,
		p_fld(SET_FLD, CKECTRL_CKEON) |
		p_fld(SET_FLD, CKECTRL_CKETIMER_SEL));
	io_32_write_fld_align(DRAMC_REG_RKCFG, SET_FLD,
		RKCFG_CKE2RANK_OPT2);
#endif
	io_32_write_fld_align(DRAMC_REG_SHU_CONF2, 0x7,
		SHU_CONF2_DCMDLYREF);

	io_32_write_fld_multi(DRAMC_REG_SHU_SCINTV,
		p_fld(CLEAR_FLD, SHU_SCINTV_DQS2DQ_SHU_PITHRD) |
		p_fld(CLEAR_FLD, SHU_SCINTV_DQS2DQ_FILT_PITHRD) |
		p_fld(CLEAR_FLD, SHU_SCINTV_RDDQC_INTV) |
		p_fld(CLEAR_FLD, SHU_SCINTV_TZQLAT));
#if 0 /*modified before broadcast*/
	io_32_write_fld_multi(DRAMC_REG_SHU_CONF1,
		p_fld(0xc, SHU_CONF1_DATLAT_DSEL_PHY) |
		p_fld(0xc, SHU_CONF1_DATLAT_DSEL) |
		p_fld(0xe, SHU_CONF1_DATLAT));
#endif
	io_32_write_fld_align(DRAMC_REG_SHUCTRL, SET_FLD,
		SHUCTRL_LPSM_BYPASS_B);
	io_32_write_fld_align(DRAMC_REG_REFCTRL1, CLEAR_FLD,
		REFCTRL1_SREF_PRD_OPT);

	io_32_write_fld_align(DDRPHY_SHU_PLL5, SET_FLD,
		SHU_PLL5_RG_RPHYPLL_SDM_FRA_EN);
	io_32_write_fld_align(DDRPHY_SHU_PLL7, SET_FLD,
		SHU_PLL7_RG_RCLRPLL_SDM_FRA_EN);
	io_32_write_fld_align(DDRPHY_SHU_PLL4, 0x2000, SHU_PLL4_RG_RPHYPLL_RESERVED);
	io_32_write_fld_align(DDRPHY_SHU_PLL6, 0x2000, SHU_PLL6_RG_RCLRPLL_RESERVED);

	io_32_write_fld_align(DRAMC_REG_REFRATRE_FILTER, SET_FLD,
		REFRATRE_FILTER_PB2AB_OPT);
	io_32_write_fld_align(DDRPHY_MISC_CTRL1, SET_FLD,
		MISC_CTRL1_R_DMDA_RRESETB_I);
	io_32_write_fld_align(DRAMC_REG_DRAMCTRL, SET_FLD,
		DRAMCTRL_CLKWITRFC);
#if NON_EXIST_RG
	io_32_write_fld_multi(DRAMC_REG_MISCTL0,
		p_fld(SET_FLD, MISCTL0_REFP_ARB_EN2) |
		p_fld(SET_FLD, MISCTL0_PBC_ARB_EN) |
		p_fld(SET_FLD, MISCTL0_REFA_ARB_EN2));
#endif
	io_32_write_fld_multi(DRAMC_REG_MISCTL0,
		p_fld(SET_FLD, MISCTL0_DM32BIT_RDSEL_OPT) |
		p_fld(SET_FLD, MISCTL0_PG_WAKEUP_OPT) |
		p_fld(SET_FLD, MISCTL0_PRE_DLE_VLD_OPT) |
		p_fld(SET_FLD, MISCTL0_PBC_ARB_EN));

	io_32_write_fld_multi(DRAMC_REG_PERFCTL0,
		p_fld(SET_FLD, PERFCTL0_MWHPRIEN) |
		p_fld(SET_FLD, PERFCTL0_RWSPLIT) |
		p_fld(SET_FLD, PERFCTL0_WFLUSHEN) |
		p_fld(SET_FLD, PERFCTL0_EMILLATEN) |
		p_fld(SET_FLD, PERFCTL0_RWAGEEN) |
		p_fld(SET_FLD, PERFCTL0_RWLLATEN) |
		p_fld(SET_FLD, PERFCTL0_RWHPRIEN) |
		p_fld(SET_FLD, PERFCTL0_RWOFOEN) |
		p_fld(SET_FLD, PERFCTL0_DISRDPHASE1));
#if NON_EXIST_RG
		p_fld(SET_FLD, PERFCTL0_DUALSCHEN));
#endif
	io_32_write_fld_align(DRAMC_REG_ARBCTL, 0x80, ARBCTL_MAXPENDCNT);
	io_32_write_fld_multi(DRAMC_REG_PADCTRL,
		p_fld(SET_FLD, PADCTRL_DQIENLATEBEGIN) |
		p_fld(SET_FLD, PADCTRL_DQIENQKEND));
	io_32_write_fld_align(DRAMC_REG_DRAMC_PD_CTRL, SET_FLD,
		DRAMC_PD_CTRL_DCMREF_OPT);
	io_32_write_fld_align(DRAMC_REG_CLKCTRL, SET_FLD, CLKCTRL_CLK_EN_1);
	io_32_write_fld_align(DRAMC_REG_CLKCTRL, SET_FLD, CLKCTRL_CLK_EN_0);
	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(0x4, REFCTRL0_DISBYREFNUM) |
		p_fld(SET_FLD, REFCTRL0_DLLFRZ));
	io_32_write_fld_align(DRAMC_REG_SPCMDCTRL, SET_FLD,
		SPCMDCTRL_CLR_EN);
#if 0
	io_32_write_fld_multi(DRAMC_REG_CATRAINING1,
		p_fld(0x4, CATRAINING1_CATRAIN_INTV) |
		p_fld(0x3, CATRAINING1_CATRAINLAT));
#endif
	io_32_write_fld_align(DRAMC_REG_STBCAL, SET_FLD, STBCAL_REFUICHG);
	io_32_write_fld_multi(DRAMC_REG_SHU_RANKCTL,
		p_fld(0x5, SHU_RANKCTL_RANKINCTL_PHY) |
		p_fld(0x2, SHU_RANKCTL_RANKINCTL_ROOT1) |
		p_fld(0x2, SHU_RANKCTL_RANKINCTL));
	io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, SET_FLD,
		SHU_STBCAL_DMSTBLAT);
	/*
	 * io_32_write_fld_align(DRAMC_REG_SHURK0_DQSCTL, 0x4, SHURK0_DQSCTL_DQSINCTL); //DQSINCTL: set in UpdateACTimingReg()
	 * io_32_write_fld_align(DRAMC_REG_SHURK1_DQSCTL, 0x4, SHURK1_DQSCTL_R1DQSINCTL); //DQSINCTL: set in UpdateACTimingReg()
	 */

	delay_us(2);

#if 0//cc mark LEGACY_GATING_DLY
	LegacyGatingDlyLP3(p);
#endif

	io_32_write_fld_multi(DRAMC_REG_SHU_WODT,
		p_fld(CLEAR_FLD, SHU_WODT_DISWODTE) |
		p_fld(CLEAR_FLD, SHU_WODT_WODTFIXOFF) |
		p_fld(CLEAR_FLD, SHU_WODT_DISWODT));

	io_32_write_fld_align(DRAMC_REG_PHY_RX_INCTL,
		CLEAR_FLD, PHY_RX_INCTL_RX_IN_GATE_EN_4BYTE_EN);
	io_32_write_fld_align(DRAMC_REG_PHY_RX_INCTL,
		SET_FLD, PHY_RX_INCTL_RX_IN_GATE_EN_OPT);
	io_32_write_fld_align(DRAMC_REG_PHY_RX_INCTL,
		SET_FLD, PHY_RX_INCTL_RX_IN_BUFF_EN_OPT);
	/*
	 * io_32_write_fld_align(DRAMC_REG_SHU_CONF3, 0x5, SHU_CONF3_ZQCSCNT);
	 * io_32_write_fld_align(DRAMC_REG_SHU_CONF3, 0xff, SHU_CONF3_REFRCNT);
	 * io_32_write_fld_align(DRAMC_REG_SHU_CONF1, 0xb0, SHU_CONF1_REFBW_FR);
	 */

	io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, SET_FLD,
		SHU_STBCAL_DQSG_MODE);
	io_32_write_fld_multi(DRAMC_REG_SHU_CONF0,
		p_fld(0x5, SHU_CONF0_REFTHD) |
		p_fld(0x15, SHU_CONF0_DMPGTIM));
	/*
	 * io_32_write_fld_align(DRAMC_REG_SHU_CONF1, 0xae, SHU_CONF1_REFBW_FR);
	 * io_32_write_fld_multi(DRAMC_REG_SHU_CONF3,
		p_fld(0x8d, SHU_CONF3_REFRCNT) |
	 * | p_fld(0x1f8, SHU_CONF3_ZQCSCNT));
	 */
	io_32_write_fld_align(DRAMC_REG_SHU_STBCAL, SET_FLD,
		SHU_STBCAL_PICGLAT);

	{

		io_32_write_fld_multi(DRAMC_REG_SHU_CONF0,
			p_fld(SET_FLD, SHU_CONF0_REFTHD) |
			p_fld(0x3f, SHU_CONF0_DMPGTIM));
		io_32_write_fld_multi(DRAMC_REG_SHU_CONF1,
			p_fld(SET_FLD, SHU_CONF1_TREFBWIG) |
			p_fld(CLEAR_FLD, SHU_CONF1_REFBW_FR));
		io_32_write_fld_multi(DRAMC_REG_SHU_CONF3,
			p_fld(0xff, SHU_CONF3_REFRCNT) |
			p_fld(0x5, SHU_CONF3_ZQCSCNT));
		io_32_write_fld_align(DRAMC_REG_SHU_RANKCTL, 0x3,
			SHU_RANKCTL_RANKINCTL_PHY);

#if NON_EXIST_RG
		io_32_write_fld_align(DRAMC_REG_SHU_WODT, CLEAR_FLD,
			SHU_WODT_TWPSTEXT);
#endif
		io_32_write_fld_align(DRAMC_REG_SHU_DQSG, 0xf, SHU_DQSG_SCINTV);
		/*
		 * io_32_write_fld_multi(DRAMC_REG_SHU_HWSET_MR2,
			p_fld(CLEAR_FLD, SHU_HWSET_MR2_HWSET_MR2_OP) |
		 * | p_fld(CLEAR_FLD, SHU_HWSET_MR2_HWSET_MR2_MRSMA));
		 * io_32_write_fld_multi(DRAMC_REG_SHU_HWSET_MR13,
			p_fld(0x1c, SHU_HWSET_MR13_HWSET_MR13_OP) |
		 * | p_fld(0x2, SHU_HWSET_MR13_HWSET_MR13_MRSMA));
		 * io_32_write_fld_multi(DRAMC_REG_SHU_HWSET_VRCG,
			p_fld(CLEAR_FLD, SHU_HWSET_VRCG_HWSET_VRCG_OP) |
		 * | p_fld(CLEAR_FLD, SHU_HWSET_VRCG_HWSET_VRCG_MRSMA));
		 */
		io_32_write_fld_multi(DRAMC_REG_SHU_DQSG_RETRY,
			p_fld(SET_FLD, SHU_DQSG_RETRY_R_RETRY_USE_BURST_MDOE) |
			p_fld(SET_FLD, SHU_DQSG_RETRY_R_RETRY_ROUND_NUM) |
	#ifdef FIRST_BRING_UP
			p_fld(CLEAR_FLD, SHU_DQSG_RETRY_R_XSR_RETRY_SPM_MODE) |
	#else
			p_fld(SET_FLD, SHU_DQSG_RETRY_R_XSR_RETRY_SPM_MODE) |
	#endif
			p_fld(0x3, SHU_DQSG_RETRY_R_DQSIENLAT) |
			p_fld(SET_FLD, SHU_DQSG_RETRY_R_DM4BYTE));

		io_32_write_fld_align(DDRPHY_SHU_B0_DQ5, CLEAR_FLD,
			SHU_B0_DQ5_RG_ARPI_FB_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ5, CLEAR_FLD,
			SHU_B1_DQ5_RG_ARPI_FB_B1);
		io_32_write_fld_align(DDRPHY_SHU_B2_DQ5, CLEAR_FLD,
			SHU_B2_DQ5_RG_ARPI_FB_B2);
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD5, CLEAR_FLD,
			SHU_CA_CMD5_RG_ARPI_FB_CA);
		/*
		 * 93918 ===ddrphy_sh_lp3_1866_cha end===
		 * 93918 ===ddrphy_sh_lp3_1866_chb begin===
		 */
	#if 0 /*modified before broadcast*/
		io_32_write_fld_align(DDRPHY_SHU_B0_DQ5 + SHIFT_TO_CHB_ADDR, 0x4,
			SHU_B0_DQ5_RG_ARPI_FB_B0);
		io_32_write_fld_align(DDRPHY_SHU_B1_DQ5 + SHIFT_TO_CHB_ADDR, 0x4,
			SHU_B1_DQ5_RG_ARPI_FB_B1);
		io_32_write_fld_align(DDRPHY_SHU_CA_CMD5 + SHIFT_TO_CHB_ADDR, 0x4,
			SHU_CA_CMD5_RG_ARPI_FB_CA);
	#endif

#if 0
		io_32_write_fld_multi(DDRPHY_SHU_R0_B0_DQ2 + SHIFT_TO_CHB_ADDR,
			p_fld(0xa, SHU_R0_B0_DQ2_RK0_RX_ARDQ1_F_DLY_B0) |
			p_fld(0xa, SHU_R0_B0_DQ2_RK0_RX_ARDQ0_F_DLY_B0));
#endif

	}
	/*
	 * else
	 * TODO: overwrite 1866 setting END
	 */

	/*
	 * Since the above initial settings are for LP3_DDR1866, TX related initial delay settings needs to be adjusted
	 * according it's Write Latency difference
	 */
	if (p->freqGroup != DDR1866_FREQ) {
		//cc mark TxCalibrationMoveDQSByDifferentWL(p);
	}

	io_32_write_fld_align(DRAMC_REG_RKCFG, SET_FLD, RKCFG_RKMODE);

	update_initial_setting_ddr4(p);

#if SIMULATION_SW_IMPED
	dramc_sw_impedance_save_register(p, ODT_OFF, ODT_OFF,
		DRAM_DFS_SHUFFLE_1);
#endif

	dramc_mr_init_ddr4(p);

#if 0 /*modified before broadcast*/
	io_32_write_fld_multi(DRAMC_REG_MRS,
		p_fld(CLEAR_FLD, MRS_MRSRK) |
		p_fld(0x4, MRS_MRSMA) |
		p_fld(CLEAR_FLD, MRS_MRSOP));

	delay_us(1);

	io_32_write_fld_align(DRAMC_REG_SPCMD, SET_FLD, SPCMD_MRREN);

	delay_us(1);
#endif

	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(CLEAR_FLD, REFCTRL0_PBREFEN) |
		p_fld(CLEAR_FLD, REFCTRL0_PBREF_DISBYRATE));
	io_32_write_fld_align(DRAMC_REG_RKCFG, CLEAR_FLD, RKCFG_DQSOSC2RK);
	io_32_write_fld_align(DRAMC_REG_RKCFG, 0x3, RKCFG_RKSIZE);
	/*
	 * io_32_write_fld_align(DRAMC_REG_CKECTRL, CLEAR_FLD, CKECTRL_CKEFIXON);
	 * io_32_write_fld_align(DRAMC_REG_HW_MRR_FUN, SET_FLD, HW_MRR_FUN_TMRR_ENA);
	 */
	io_32_write_fld_align(DRAMC_REG_DRAMCTRL, SET_FLD,
		DRAMCTRL_PREALL_OPTION);
	io_32_write_fld_align(DRAMC_REG_ZQCS, 0x56, ZQCS_ZQCSOP);

	delay_us(1);

	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(SET_FLD, REFCTRL0_REFFRERUN) |
		p_fld(SET_FLD, REFCTRL0_REFDIS));
	io_32_write_fld_align(DRAMC_REG_SREFCTRL, SET_FLD,
		SREFCTRL_SREF_HW_EN);
	io_32_write_fld_align(DRAMC_REG_MPC_OPTION, SET_FLD,
		MPC_OPTION_MPCRKEN);
	io_32_write_fld_multi(DRAMC_REG_DRAMC_PD_CTRL,
		p_fld(SET_FLD, DRAMC_PD_CTRL_PHYCLKDYNGEN) |
		p_fld(SET_FLD, DRAMC_PD_CTRL_COMBCLKCTRL) |
		p_fld(CLEAR_FLD, DRAMC_PD_CTRL_DCMENNOTRFC) |
		p_fld(SET_FLD, DRAMC_PD_CTRL_DCMEN2) |
		p_fld(SET_FLD, DRAMC_PD_CTRL_DCMEN));
#if 0
	io_32_write_fld_multi(DRAMC_REG_DRAMC_PD_CTRL + SHIFT_TO_CHB_ADDR,
		p_fld(CLEAR_FLD, DRAMC_PD_CTRL_COMBCLKCTRL) |
		p_fld(CLEAR_FLD, DRAMC_PD_CTRL_DCMENNOTRFC) |
		p_fld(SET_FLD, DRAMC_PD_CTRL_DCMEN));
#endif
	io_32_write_fld_multi(DRAMC_REG_EYESCAN,
		p_fld(CLEAR_FLD, EYESCAN_RX_DQ_EYE_SEL) |
		p_fld(CLEAR_FLD, EYESCAN_RG_RX_EYE_SCAN_EN));
	io_32_write_fld_multi(DRAMC_REG_STBCAL1,
		p_fld(CLEAR_FLD, STBCAL1_DIS_PI_TRACK_AS_NOT_RD) |
		p_fld(SET_FLD, STBCAL1_STBCNT_MODESEL) |
		p_fld(SET_FLD, STBCAL1_STB_SHIFT_DTCOUT_IG) |
		p_fld(SET_FLD, STBCAL1_STB_FLAGCLR) |
		p_fld(SET_FLD, STBCAL1_STBCNT_LATCH_EN) |
		p_fld(SET_FLD, STBCAL1_STBENCMPEN));
	io_32_write_fld_align(DRAMC_REG_TEST2_1, 0x10000,
		TEST2_1_TEST2_BASE);
	io_32_write_fld_align(DRAMC_REG_TEST2_2, 0x400, TEST2_2_TEST2_OFF);
	io_32_write_fld_multi(DRAMC_REG_TEST2_3,
		p_fld(SET_FLD, TEST2_3_TEST2WREN2_HW_EN) |
#if NON_EXIST_RG
		p_fld(0x4, TEST2_3_DQSICALSTP) |
#endif
		p_fld(SET_FLD, TEST2_3_TESTAUDPAT) |
		p_fld(CLEAR_FLD, TEST2_3_TESTADR_SHIFT));
	io_32_write_fld_align(DRAMC_REG_SHUCTRL2, SET_FLD,
		SHUCTRL2_MR13_SHU_EN);
	io_32_write_fld_align(DRAMC_REG_DRAMCTRL, SET_FLD,
		DRAMCTRL_REQQUE_THD_EN);

	io_32_write_fld_multi(DRAMC_REG_SHU_CKECTRL,
		p_fld(0x3, SHU_CKECTRL_SREF_CK_DLY) |
		p_fld(0x3, SHU_CKECTRL_TCKESRX) |
		p_fld(0x1, SHU_CKECTRL_CMDCKE));

	io_32_write_fld_align(DRAMC_REG_DUMMY_RD, p->support_rank_num,
		DUMMY_RD_RANK_NUM);

	io_32_write_fld_multi(DRAMC_REG_TEST2_4,
		p_fld(0x4, TEST2_4_TESTAGENTRKSEL) |
		p_fld(SET_FLD, TEST2_4_TESTXTALKPAT));
	io_32_write_fld_multi(DRAMC_REG_REFCTRL1,
		p_fld(SET_FLD, REFCTRL1_REF_QUE_AUTOSAVE_EN) |
		p_fld(0x5, REFCTRL1_MPENDREF_CNT) |
		p_fld(SET_FLD, REFCTRL1_SLEFREF_AUTOSAVE_EN));
	io_32_write_fld_multi(DRAMC_REG_RSTMASK,
		p_fld(CLEAR_FLD, RSTMASK_GT_SYNC_MASK) |
		p_fld(CLEAR_FLD, RSTMASK_DVFS_SYNC_MASK) |
		p_fld(CLEAR_FLD, RSTMASK_GT_SYNC_MASK_FOR_PHY) |
		p_fld(CLEAR_FLD, RSTMASK_DVFS_SYNC_MASK_FOR_PHY));
	/*
	 * io_32_write_fld_align(DRAMC_REG_DRAMCTRL, CLEAR_FLD, DRAMCTRL_CTOREQ_HPRI_OPT);
	 * 91074 === DE initial sequence done ===
	 * #ifdef DVFS_Enable
	 */

	io_32_write_fld_multi(DRAMC_REG_STBCAL,
		p_fld(CLEAR_FLD, STBCAL_STBCALEN) |
		p_fld(CLEAR_FLD, STBCAL_STB_SELPHYCALEN) |
		p_fld(CLEAR_FLD, STBCAL_REFUICHG) |
		p_fld(SET_FLD, STBCAL_SREF_DQSGUPD));

	delay_us(1);
	io_32_write_fld_align(DRAMC_REG_DDRCONF0, CLEAR_FLD, DDRCONF0_AG0MWR);
	/*
	 * io_32_write_fld_multi(DRAMC_REG_DRAMCTRL,
		p_fld(SET_FLD, DRAMCTRL_FW2R) |
	 * | p_fld(CLEAR_FLD, DRAMCTRL_DYNMWREN)
	 * | p_fld(SET_FLD, DRAMCTRL_CLKWITRFC)
	 * | p_fld(SET_FLD, DRAMCTRL_ADRBIT3DEC)
	 * | p_fld(SET_FLD, DRAMCTRL_CTOREQ_HPRI_OPT));
	 */
#if 0 /*modified before broadcast*/
	io_32_write_fld_align(DRAMC_REG_MISCTL0, CLEAR_FLD,
		MISCTL0_PBC_ARB_EN);
	io_32_write_fld_multi(DRAMC_REG_PERFCTL0,
		p_fld(SET_FLD, PERFCTL0_REORDEREN) |
		p_fld(SET_FLD, PERFCTL0_RWHPRICTL) |
		p_fld(CLEAR_FLD, PERFCTL0_EMILLATEN) |
		p_fld(CLEAR_FLD, PERFCTL0_RWAGEEN) |
		p_fld(CLEAR_FLD, PERFCTL0_RWHPRIEN));
#endif
	io_32_write_fld_multi(DRAMC_REG_RSTMASK,
		p_fld(CLEAR_FLD, RSTMASK_PHY_SYNC_MASK) |
		p_fld(CLEAR_FLD, RSTMASK_DAT_SYNC_MASK));

#if 0 /*modified before broadcast*/
	io_32_write_fld_multi(DRAMC_REG_REFCTRL0,
		p_fld(SET_FLD, REFCTRL0_REFNA_OPT) |
		p_fld(0x2, REFCTRL0_DISBYREFNUM) |
		p_fld(SET_FLD, REFCTRL0_UPDBYWR));
#endif
	io_32_write_fld_multi(DRAMC_REG_REFRATRE_FILTER,
		p_fld(0x6, REFRATRE_FILTER_REFRATE_FIL7) |
		p_fld(0x5, REFRATRE_FILTER_REFRATE_FIL6) |
		p_fld(0x4, REFRATRE_FILTER_REFRATE_FIL5) |
		p_fld(0x3, REFRATRE_FILTER_REFRATE_FIL4) |
		p_fld(0x3, REFRATRE_FILTER_REFRATE_FIL3) |
		p_fld(0x3, REFRATRE_FILTER_REFRATE_FIL2) |
		p_fld(0x2, REFRATRE_FILTER_REFRATE_FIL1) |
		p_fld(0x1, REFRATRE_FILTER_REFRATE_FIL0));
	io_32_write_fld_multi(DRAMC_REG_ZQCS,
		p_fld(CLEAR_FLD, ZQCS_ZQCSDUAL) |
		p_fld(CLEAR_FLD, ZQCS_ZQCSMASK));
	io_32_write_fld_align(DRAMC_REG_HW_MRR_FUN, CLEAR_FLD,
		HW_MRR_FUN_MRR_HW_HIPRI);
	io_32_write_fld_align(DRAMC_REG_DUMMY_RD, SET_FLD,
		DUMMY_RD_SREF_DMYRD_EN);
	io_32_write_fld_align(DRAMC_REG_STBCAL1, 0x1,
		STBCAL1_STBCAL_FILTER);

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);
	dvfs_settings(p);
}
#endif


void dramc_setting_init(DRAMC_CTX_T *p)
{
	switch (p->dram_type) {
	#if SUPPORT_TYPE_LPDDR4
	case TYPE_LPDDR4P:
	case TYPE_LPDDR4X:
	case TYPE_LPDDR4:
		dramc_setting_lp3_4(p);
		break;
	#endif

	#if SUPPORT_TYPE_LPDDR3
	case TYPE_LPDDR3:
		dramc_setting_lp3_4(p);
		break;
	#endif

	#if SUPPORT_TYPE_PCDDR4
	case TYPE_PCDDR4:
		dramc_setting_ddr4(p);
		break;
	#endif

	#if SUPPORT_TYPE_PCDDR3
	case TYPE_PCDDR3:
		dramc_setting_ddr3(p);
		break;
	#endif

	default:
		show_err("Unrecognized Dram type.\n");
		break;
	}
}
