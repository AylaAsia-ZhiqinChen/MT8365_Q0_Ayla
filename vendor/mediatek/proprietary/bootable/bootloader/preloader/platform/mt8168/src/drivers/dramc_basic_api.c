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

/** @file dramc_basic_api.c
 *  Basic DRAMC API implementation
 */

/* Include files */
#include "dramc_common.h"
#include "x_hal_io.h"

/* Global variables */
unsigned char print_mode_reg_write = FALSE;
unsigned char phy_pll_en[CHANNEL_MAX] = {TRUE, TRUE};

#define SHUFFLE_ADDR_NUM_DRAMC 3
#define SHUFFLE_ADDR_NUM_DDRPHY 17

#define REG_BACKUP_NUMBER 128

unsigned int reg_backup_vlaue[REG_BACKUP_NUMBER];

unsigned char enable_self_wakeup = FALSE;

typedef struct _SHUFFLE_REG_ADDR {
	unsigned int start_addr;
	unsigned int end_addr;
} SHUFFLE_REG_ADDR;

const SHUFFLE_REG_ADDR shuffle_reg_table_dramc[SHUFFLE_ADDR_NUM_DRAMC] = {
	{DRAMC_AO_BASE+(0x0800), DRAMC_AO_BASE+(0x08E8) },
	{DRAMC_AO_BASE+(0x0A00), DRAMC_AO_BASE+(0x0A50) },
	{DRAMC_AO_BASE+(0x0B00), DRAMC_AO_BASE+(0x0B50) },
};

const SHUFFLE_REG_ADDR shuffle_reg_table_ddrphy[SHUFFLE_ADDR_NUM_DDRPHY] = {
	{DDRPHY_BASE_ADDR+(0x0C00), DDRPHY_BASE_ADDR+(0x0C3C) }, //B0
	{DDRPHY_BASE_ADDR+(0x0C80), DDRPHY_BASE_ADDR+(0x0CBC) }, //B1
	{DDRPHY_BASE_ADDR+(0x0D00), DDRPHY_BASE_ADDR+(0x0D3C) }, //CA
	{DDRPHY_BASE_ADDR+(0x0D80), DDRPHY_BASE_ADDR+(0x0DBC) }, //PHYPLL
	{DDRPHY_BASE_ADDR+(0x0DD0), DDRPHY_BASE_ADDR+(0x0DF4) }, //CLRPLL

	{DDRPHY_BASE_ADDR+(0x0E00), DDRPHY_BASE_ADDR+(0x0E2C) },//R0
	{DDRPHY_BASE_ADDR+(0x0E50), DDRPHY_BASE_ADDR+(0x0E7C) },
	{DDRPHY_BASE_ADDR+(0x0EA0), DDRPHY_BASE_ADDR+(0x0ECC) },

	{DDRPHY_BASE_ADDR+(0x0F00), DDRPHY_BASE_ADDR+(0x0F2C) },//R1
	{DDRPHY_BASE_ADDR+(0x0F50), DDRPHY_BASE_ADDR+(0x0F7C) },
	{DDRPHY_BASE_ADDR+(0x0FA0), DDRPHY_BASE_ADDR+(0x0FCC) },

	{DDRPHY_BASE_ADDR+(0x1000), DDRPHY_BASE_ADDR+(0x102C) },//R2
	{DDRPHY_BASE_ADDR+(0x1050), DDRPHY_BASE_ADDR+(0x107C) },
	{DDRPHY_BASE_ADDR+(0x10A0), DDRPHY_BASE_ADDR+(0x10CC) },

	{DDRPHY_BASE_ADDR+(0x2C00), DDRPHY_BASE_ADDR+(0x2C3C) }, //B2
	{DDRPHY_BASE_ADDR+(0x2E00), DDRPHY_BASE_ADDR+(0x2E2C) }, //B2 R0
	{DDRPHY_BASE_ADDR+(0x2F00), DDRPHY_BASE_ADDR+(0x2F1C) }, //B2 R1
};

/*
 * Struct indicating all register fields mentioned in "multi rank CKE control"
 */
typedef struct {
	unsigned char cke_2_rank : 1;
	unsigned char cke_2_rank_opt : 1;
	unsigned char cke_2_rank_opt2 : 1;
	unsigned char cke_2_rank_opt3 : 1;
	unsigned char cke_2_rank_opt5 : 1;
	unsigned char cke_2_rank_opt6 : 1;
	unsigned char cke_2_rank_opt7 : 1;
	unsigned char cke_2_rank_opt8 : 1;
	unsigned char cke_timer_sel : 1;
	unsigned char fast_wake : 1;
	unsigned char fast_wake_2 : 1;
	unsigned char fast_wake_sel : 1;
	unsigned char cke_wake_sel : 1;
	unsigned char clk_witrfc : 1;
} CKE_CTRL_T;

void dramc_print_rxdqdqs_status(DRAMC_CTX_T *p, unsigned char channel);
void dramc_print_imp_tracking_status(DRAMC_CTX_T *p, unsigned char channel);
void set_shuffle_frequency(DRAMC_CTX_T *p,
	DRAM_DFS_SHUFFLE_TYPE_T whitch_shuffle, unsigned short frequency);
void dramc_hw_gating_debug_on_off(DRAMC_CTX_T *p, unsigned char on_off);
void dramc_mode_reg_write_by_rank(DRAMC_CTX_T *p, unsigned char rank,
	unsigned char mr_Idx, unsigned short value);

void dramc_broadcast_on_off(unsigned int on_off)
{
#if (HW_BROADCAST_ON == 1)
#if (FOR_DV_SIMULATION_USED == 1)
	if (on_off == DRAMC_BROADCAST_ON) {
		broadcast_on();
	} else {
		broadcast_off();
	}
#else
    if ((on_off == DRAMC_BROADCAST_ON) && (get_ddr_type() != TYPE_PCDDR3 && get_ddr_type() != TYPE_LPDDR3))
		*((volatile unsigned int *)(DRAMC_WBR)) = 0x0000001F; //TBD
	else
		*((volatile unsigned int *)(DRAMC_WBR)) = 0x00000000;
#endif
#endif
}

unsigned int get_dramc_broadcast(void)
{

unsigned uWBR;

#if (HW_BROADCAST_ON == 1)
#if (FOR_DV_SIMULATION_USED == 1)
	broadcast_off();
#else
       uWBR =*((volatile unsigned int *)(DRAMC_WBR));
       if (uWBR == 0)
	    return DRAMC_BROADCAST_OFF;
	else
	    return DRAMC_BROADCAST_ON;
#endif
#else
	return 0;
#endif
}
void set_freq_sel_by_table(DRAMC_CTX_T *p, DRAM_DFS_FREQUENCY_TABLE_T *table)
{
	p->dfs_table = table;
	ddr_phy_freq_sel(p, table->freq_sel);
	set_vcore_by_freq(p);
}

DIV_MODE_T get_div_mode(DRAMC_CTX_T *p)
{
	return p->dfs_table->divmode;
}

unsigned char is_lp4_family(DRAMC_CTX_T *p)
{
	if ((p->dram_type == TYPE_LPDDR4) ||
		(p->dram_type == TYPE_LPDDR4X) ||
		(p->dram_type == TYPE_LPDDR4P))
		return 1;
	else
		return 0;
}

unsigned char is_need_do_fast_k(DRAMC_CTX_T *p)
{
	if ((p->dram_type == TYPE_LPDDR4) ||
		(p->dram_type == TYPE_LPDDR4X) ||
		(p->dram_type == TYPE_LPDDR4P) ||
		(p->dram_type == TYPE_PCDDR4))
		return 1;
	else
		return 0;
}

unsigned char is_byte_swapped(DRAMC_CTX_T *p)
{
	if ((p->dram_type == TYPE_LPDDR4) ||
		((p->dram_type == TYPE_PCDDR3) &&
		(p->data_width == DATA_WIDTH_16BIT))) {
		return 1;
	} else {
		return 0;
	}
}
/*
 * To support frequencies not on the ACTimingTable, we use read/write latency
 * settings as the next highest freq listed in ACTimingTable. Use this API
 * to set which freq group the target freq belongs to.
 * (Set's DRAMC_CTX_T's p->freqGroup)
 * Currently is designed to be called from ddr_phy_freq_sel()
 */
void set_freq_group(DRAMC_CTX_T *p)
{
	/*
	 * Below listed conditions represent freqs that exist in ACTimingTable
	 * -> Should cover freqGroup settings for all real freq values
	 */
	if (p->frequency <= DDR1200_FREQ)
		p->freqGroup = DDR1200_FREQ;
	else if (p->frequency <= DDR1333_FREQ)
		p->freqGroup = DDR1333_FREQ;
	else if (p->frequency <= DDR1600_FREQ)
		p->freqGroup = DDR1600_FREQ;
	else if (p->frequency <= DDR1866_FREQ)
		p->freqGroup = DDR1866_FREQ;
	else if (p->frequency <= DDR2400_FREQ)
		p->freqGroup = DDR2400_FREQ;
	else if (p->frequency <= DDR2666_FREQ)
		p->freqGroup = DDR2666_FREQ;
	else if (p->frequency <= DDR3200_FREQ)
		p->freqGroup = DDR3200_FREQ;

	show_msg3((INFO, "[setFreqGroup] p->frequency: %u, freqGroup: %u\n",
		p->frequency, p->freqGroup));
}

static void set_rank_info_to_conf(DRAMC_CTX_T *p)
{
	show_msg3((INFO, "Rank info: Single rank Configurantion\n"));
}

void global_option_init(DRAMC_CTX_T *p)
{
	unsigned int idx;

	set_channel_number(p);
	set_rank_info_to_conf(p);
	set_rank_number(p);
	set_mrr_pinmux_mapping(p);
	init_global_variables_by_condition(p);

	for (idx = DRAM_DFS_SHUFFLE_1; idx < DRAM_DFS_SHUFFLE_MAX; idx++) {
		if (is_lp4_family(p)) {
		#if SUPPORT_TYPE_LPDDR4
			set_shuffle_frequency(p, idx, freq_tbl_lp4[idx].frequency);
		#endif
		} else if (p->dram_type == TYPE_LPDDR3) {
		#if SUPPORT_TYPE_LPDDR3
			set_shuffle_frequency(p, idx, freq_tbl_lp3[idx].frequency);
		#endif
		} else {
			set_shuffle_frequency(p, idx, p->frequency);
			break;
		}
	}
	memset(phy_pll_en, TRUE, sizeof(phy_pll_en));
#if (CONFIG_FOR_HQA_TEST_USED == 1)
	HQA_measure_message_reset_all_data(p);
#endif

}

void auto_refresh_cke_off(DRAMC_CTX_T *p)
{
	unsigned int backup_broadcast;

	backup_broadcast = get_dramc_broadcast();

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);

	show_msg((INFO, "auto_refresh_cke_off AutoREF OFF\n"));
	io_32_write_fld_align_all(DRAMC_REG_REFCTRL0,
		SET_FLD, REFCTRL0_REFDIS);
	delay_us(3);
	show_msg((INFO, "ddr_phy_pll_setting-CKEOFF\n"));
	cke_fix_on_off(p, CKE_WRITE_TO_ALL_RANK, CKE_FIXOFF, CKE_WRITE_TO_ALL_CHANNEL);
	delay_us(1);

	dramc_broadcast_on_off(backup_broadcast);
}

unsigned short dfs_get_highest_freq(DRAMC_CTX_T * p)
{
    if (is_lp4_family(p))
		return LP4_HIGHEST_FREQ;
	else if (p->dram_type == TYPE_LPDDR3)
		return LP3_HIGHEST_FREQ;
	else if (p->dram_type == TYPE_PCDDR4)
		return DDR4_HIGHEST_FREQ;
	else
		return DDR3_HIGHEST_FREQ;
}


void cke_fix_on_off(DRAMC_CTX_T *p, unsigned char rank, CKE_FIX_OPTION option,
		CKE_FIX_CHANNEL write_channel_num)
{
	unsigned char cke_on, cke_off;
	/* if CKE is dynamic, set both CKE fix On and Off as 0 */
	if (option == CKE_DYNAMIC) {
		/*
		 * After CKE FIX on/off,
		 * CKE should be returned to dynamic (control by HW)
		 */
		cke_on = cke_off = 0;
	} else {
	/*
	 * if CKE fix on is set as 1,
	 * CKE fix off should also be set as 0; vice versa
	 */
		cke_on = option;
		cke_off = (1 - option);
	}

	/* write register to all channel */
	if (write_channel_num == CKE_WRITE_TO_ALL_CHANNEL) {
		if (rank == RANK_0) {
			io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
				p_fld(cke_off, CKECTRL_CKEFIXOFF) |
				p_fld(cke_on, CKECTRL_CKEFIXON));
		} else if (rank == RANK_1) {
			io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
				p_fld(cke_off, CKECTRL_CKE1FIXOFF) |
				p_fld(cke_on, CKECTRL_CKE1FIXON));
		} else if (rank == CKE_WRITE_TO_ALL_RANK) {
			io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
				p_fld(cke_off, CKECTRL_CKEFIXOFF) |
				p_fld(cke_on, CKECTRL_CKEFIXON));

			if (p->support_rank_num > 1) {
				io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
					p_fld(cke_off, CKECTRL_CKE1FIXOFF) |
					p_fld(cke_on, CKECTRL_CKE1FIXON));
			}
			else
			{
				io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
					p_fld(0, CKECTRL_CKE1FIXOFF) |
					p_fld(0, CKECTRL_CKE1FIXON));
			}
		}
	} else {
		if (rank == RANK_0) {
			io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
				p_fld(cke_off, CKECTRL_CKEFIXOFF) |
				p_fld(cke_on, CKECTRL_CKEFIXON));
		} else if (rank == RANK_1) {
			io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
				p_fld(cke_off, CKECTRL_CKE1FIXOFF) |
				p_fld(cke_on, CKECTRL_CKE1FIXON));
		} else if (rank == CKE_WRITE_TO_ALL_RANK) {
			io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
				p_fld(cke_off, CKECTRL_CKEFIXOFF) |
				p_fld(cke_on, CKECTRL_CKEFIXON));

			if (p->support_rank_num > 1) {
				io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
					p_fld(cke_off, CKECTRL_CKE1FIXOFF) |
					p_fld(cke_on, CKECTRL_CKE1FIXON));
			}
			else
			{
				io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
					p_fld(0, CKECTRL_CKE1FIXOFF) |
					p_fld(0, CKECTRL_CKE1FIXON));
			}
		}
	}
}

/*
 * rx_dqs_isi_pulse_cg() - API for "RX DQS ISI pulse CG function"
 * 0: disable, 1: enable
 * 1. RG_*_RPRE_TOG_EN (16nm APHY):
 * B0_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B0, B1_DQ6_RG_RX_ARDQ_RPRE_TOG_EN_B1
 * 2. RG_RX_*RDQS*_DQSSTB_CG_EN*(10nm APHY):
 * B0_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B0, B1_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B1
 * Supports setting current channel only,
 * add function to set "all channels" in the future
 */
void rx_dqs_isi_pulse_cg(DRAMC_CTX_T *p, unsigned char on_off)
{
	show_msg2((INFO, "CH%u RX DQS ISI pulse CG: %u (0:disable, 1:enable)\n",
		p->channel, on_off));

	if (p->data_width == DATA_WIDTH_16BIT) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B0_DQ8), on_off,
			B0_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B0);
		io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B2_DQ8), on_off,
			B2_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B2);
	} else {
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ8), on_off,
			B0_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B0);
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ8), on_off,
			B2_DQ8_RG_RX_ARDQS_DQSSTB_CG_EN_B2);
	}
}

/*[QW]: for lp3/ddr3/ddr4*/
void dvfs_settings(DRAMC_CTX_T *p)
{
#if DUAL_FREQ_K
	/* DVFS_SM freq: 0: 52Mhz 1:104Mhz */
	unsigned char dvfs_52m_104m_sel = 0;
	unsigned char dll_idle = 0x30; /* DDR800 */
	/*
	 * DVFS_SM LP4: dll_idle 90MCK/19.5ns, LP3: 70MCK
	 * Below values are pre-calculated for each freq and
	 * dram type specifically
	 * for 52M DVFS_SM mode (104M should multiply values by 2)
	 * Since this register value is related to dram's operating freq
	 *  -> Each freq must use specific pre-calculated value
	 * (Couldn't use formula to calculate this value
	 * because floating point numbers are not supported via preloader)
	 */
	if (is_lp4_family(p)) {
		switch (p->frequency) {
		case DDR4266_FREQ:
			dll_idle = 0x9;
			break;
		case DDR3733_FREQ:
			dll_idle = 0xa;
			break;
		case DDR3200_FREQ:
			dll_idle = 0x18;
			break;
		case DDR2666_FREQ:
			dll_idle = 0x1a;
			break;
		case DDR1600_FREQ:
			dll_idle = 0x30;
			break;
		case DDR1333_FREQ:
			dll_idle = 0x39;
			break;
		case DDR1200_FREQ:
			dll_idle = 0x3f;
			break;
		default:
			show_err("dll_idle err!\n");
			/*
			* Set to a large value,
			* but should check with designer for actual settings
			*/
			dll_idle = 0x3F;
			break;
		}
	} else if (p->dram_type == TYPE_LPDDR3) {
		switch (p->frequency) {
		case DDR1600_FREQ:
			dll_idle = 0x30;
			break;
		case DDR1333_FREQ:
			dll_idle = 0x39;
			break;
		case DDR1200_FREQ:
			dll_idle = 0x3f;
			break;
		default:
			show_err("dll_idle err!\n");
			/*
			* Set to a large value,
			* but should check with designer for actual settings
			*/
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
	/* 52M_104M_SEL */
	/* 104M clock */
	if (dvfs_52m_104m_sel == 1) {
		/*
		 * If DVFS_SM uses 104M mode,
		 * dll_idle value should multiply by 2
		 */
		dll_idle = (dll_idle * 109 / 52);
	}

	/* DVFS debug enable - MRR_STATUS2_DVFS_STATE */
	io_32_write_fld_align_all(DRAMC_REG_DVFSDLL, SET_FLD,
		DVFSDLL_R_DDRPHY_SHUFFLE_DEBUG_ENABLE);
	/* Set DVFS_SM's clk */
	io_32_write_fld_align_all(DDRPHY_DVFS_EMI_CLK, dvfs_52m_104m_sel,
		DVFS_EMI_CLK_RG_52M_104M_SEL);
	/*
	io_32_write_fld_align_all(DRAMC_REG_SHUCTRL2, dll_idle,
		SHUCTRL2_R_DLL_IDLE);
	*/
#if 1 /* Under debug. Use tmp setting */
	io_32_write_fld_align_all(DRAMC_REG_SHU_DVFSCTL, dll_idle,
		SHU_DVFSCTL_R_DLL_IDLE);
#else
	io_32_write_fld_align_all(DRAMC_REG_SHU_DVFSCTL, 0x2f,
		SHU_DVFSCTL_R_DLL_IDLE);
#endif

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

	if (is_lp4_family(p)) {
		io_32_write_fld_align_all(DDRPHY_DVFS_EMI_CLK, 1,
			DVFS_EMI_CLK_RG_DLL_SHUFFLE);
		io_32_write_fld_align_all(DRAMC_REG_DVFSDLL, 1,
			DVFSDLL_RG_DLL_SHUFFLE);
	}
#endif
}

void mpll_init(void)
{
#if (FOR_DV_SIMULATION_USED == 0)
	(*(volatile unsigned int *)(0x1000C34C)) = 0x00000003;
	delay_us(1);
	(*(volatile unsigned int *)(0x1000C34C)) = 0x00000001;
	delay_us(1);
	(*(volatile unsigned int *)(0x1000C340)) = 0x00000181;
	delay_us(1);
	(*(volatile unsigned int *)(0x10000104)) &= ~((1UL<<6) | (1UL<<10));
	(*(volatile unsigned int *)(0x10000104)) |= 1UL<<11;
	delay_us(1);
#else
	show_msg((CRITICAL, "[WARNING] mpll_init() is empty\n"));
#endif
}

void ddr_phy_freq_sel(DRAMC_CTX_T *p, DRAM_PLL_FREQ_SEL_T sel)
{
	p->freq_sel = sel;

	switch (p->freq_sel) {
	case DDR_DDR3200:
		p->frequency = DDR3200_FREQ;
		p->dram_fsp = FSP_1;
		p->odt_onoff = ODT_ON;
		break;

	case DDR_DDR2666:
		p->frequency = DDR2666_FREQ;
		p->dram_fsp = FSP_1;
		p->odt_onoff = ODT_ON;
		break;

	case DDR_DDR2400:
		p->frequency = DDR2400_FREQ;
		p->dram_fsp = FSP_0;
		p->odt_onoff = ODT_OFF;
		break;

	case DDR_DDR1866:
		p->frequency = DDR1866_FREQ;
		p->dram_fsp = FSP_0;
		p->odt_onoff = ODT_OFF;
		break;

	case DDR_DDR1600:
		p->frequency = DDR1600_FREQ;
		p->dram_fsp = FSP_0;
		p->odt_onoff = ODT_OFF;
		break;


	case DDR_DDR1333:
		p->frequency = DDR1333_FREQ;
		p->dram_fsp = FSP_0;
		p->odt_onoff = ODT_OFF;
		break;

	case DDR_DDR1200:
		p->frequency = DDR1200_FREQ;
		p->dram_fsp = FSP_0;
		p->odt_onoff = ODT_OFF;
		break;

	default:
		show_err("ddr_phy_freq_sel: Incorrect freq_sel value!!!\n");
		while (1);
		break;
	}

	set_shuffle_frequency(p, DRAM_DFS_SHUFFLE_1, p->frequency);
	set_freq_group(p);
}

void set_shuffle_frequency(DRAMC_CTX_T *p,
	DRAM_DFS_SHUFFLE_TYPE_T whitch_shuffle, unsigned short frequency)
{
	p->shuffle_frequency[whitch_shuffle] = frequency;
}

unsigned char mapping_phy_byte_to_dramc(DRAMC_CTX_T *p,
	unsigned char phy_byte)
{
	unsigned char dramc_byte;
#if (SUPPORT_TYPE_LPDDR4 == 1)
	if(is_lp4_family( p)) {
		if (p->is_emcp)
			dramc_byte = lpddr4_phy2dramc_byte_mapping_emcp[p->channel][phy_byte];
		else
			dramc_byte = lpddr4_phy2dramc_byte_mapping_dsc[p->channel][phy_byte];
	}
#endif

#if (SUPPORT_TYPE_LPDDR3 == 1)
	if(p->dram_type == TYPE_LPDDR3) {
		if (p->is_emcp)
			dramc_byte = lpddr3_phy2dramc_byte_mapping_emcp[phy_byte];
		else
			dramc_byte = lpddr3_phy2dramc_byte_mapping_dsc[phy_byte];
	}
#endif

#if (SUPPORT_TYPE_PCDDR4 == 1)
	if(p->dram_type == TYPE_PCDDR4)
		dramc_byte = pcddr4_phy2dramc_byte_mapping[p->channel][phy_byte];
#endif

#if (SUPPORT_TYPE_PCDDR3 == 1)
	if(p->dram_type == TYPE_PCDDR3)
		dramc_byte = pcddr3_phy2dramc_byte_mapping[phy_byte];
#endif

	return dramc_byte;
}

/*
 * dramc_init_pre_settings(): Initial register settings
 * (which are required to be set before all calibration flow)
 */
void dramc_init_pre_settings(DRAMC_CTX_T *p)
{
	/* PAD_RRESETB control sequence */
	/*
	 * remove twice dram reset pin pulse
	 * before dram power on sequence flow
	 */
	if (is_lp4_family(p) || (p->dram_type == TYPE_PCDDR4)) {
        io_32_write_fld_multi(DDRPHY_CA_CMD8,
				p_fld(0x0, CA_CMD8_RG_TX_RRESETB_PULL_UP) |
				p_fld(0x0, CA_CMD8_RG_TX_RRESETB_PULL_DN) |
				p_fld(0x1, CA_CMD8_RG_TX_RRESETB_DDR3_SEL) |
				p_fld(0x0, CA_CMD8_RG_TX_RRESETB_DDR4_SEL) |
				p_fld(0xa, CA_CMD8_RG_RRESETB_DRVP) |
				p_fld(0xa, CA_CMD8_RG_RRESETB_DRVN));
        io_32_write_fld_align(DDRPHY_MISC_CTRL1, 0x1, MISC_CTRL1_R_DMRRESETB_I_OPT);

        io_32_write_fld_align(DDRPHY_MISC_CTRL1, 0x0, MISC_CTRL1_R_DMDA_RRESETB_I);
        io_32_write_fld_align(DDRPHY_MISC_CTRL1, 0x1, MISC_CTRL1_R_DMDA_RRESETB_E);

		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_CA_CMD8),
			p_fld(CLEAR_FLD, CA_CMD8_RG_TX_RRESETB_PULL_UP) |
			p_fld(CLEAR_FLD, CA_CMD8_RG_TX_RRESETB_PULL_DN) |
			p_fld(SET_FLD, CA_CMD8_RG_TX_RRESETB_DDR3_SEL) |
			p_fld(CLEAR_FLD, CA_CMD8_RG_TX_RRESETB_DDR4_SEL));
		/* Change to glitch-free path */

		io_32_write_fld_align_all(DDRPHY_MISC_CTRL1, SET_FLD,
			MISC_CTRL1_R_DMDA_RRESETB_E);
	}
}

DRAM_STATUS_T dramc_init(DRAMC_CTX_T *p)
{
	unsigned int save_ch;
	unsigned char dram_cbt_mode;

	show_msg_with_timestamp((INFO, "\n[dramc_init] ====Begin====\n"));

	save_ch = get_phy_2_channel_mapping(p);
	set_phy_2_channel_mapping(p, CHANNEL_A);

	memset(p->arfg_write_leveling_init_shif, FALSE,
		sizeof(p->arfg_write_leveling_init_shif));
	memset(p->fg_tx_perbif_init, FALSE, sizeof(p->fg_tx_perbif_init));

	dramc_setting_init(p);
	ddr_update_ac_timing(p);

#if 0
	save_ch = get_phy_2_channel_mapping(p);
	set_phy_2_channel_mapping(p, CHANNEL_A);
#endif

	if(p->dram_type > 7)
		show_msg((INFO, "DRAM Type out of RSV_DRAM_TYPE!\n"));

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_ARBCTL),
		p->dram_type, ARBCTL_RSV_DRAM_TYPE);


	if (is_lp4_family(p)) {
		/* For DRAM normal, byte and mixed mode */
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

		// Sagy: Keep original setting till OS kernel ready, if ready, remove it
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_ARBCTL),
			(p->dram_cbt_mode[RANK_0]|p->dram_cbt_mode[RANK_1]), ARBCTL_RSV_DRAM_CBT);
	}

	set_phy_2_channel_mapping(p, save_ch);

	show_msg3((INFO, "[dramc_init] ====Done====\n"));
	show_msg3((INFO, "Selected channel = %d\n", p->channel));

	return DRAM_OK;
}

#ifdef DUMMY_READ_FOR_TRACKING
void dramc_dummy_read_address_setting(DRAMC_CTX_T *p)
{
	unsigned char backup_channel = p->channel, backup_rank = p->rank;
	unsigned char channelIdx, rankIdx;
	dram_addr_t dram_addr;

	for (channelIdx = CHANNEL_A; channelIdx < CHANNEL_NUM; channelIdx++) {
		set_phy_2_channel_mapping(p, channelIdx);
		for (rankIdx = RANK_0; rankIdx < p->support_rank_num; rankIdx++) {
			set_rank(p, rankIdx);

			dram_addr.ch = channelIdx;
			dram_addr.rk = rankIdx;

			get_dummy_read_addr(&dram_addr);
			show_msg3((INFO,
				"%s_%d, RK%d, row:0x%x, bk:%d, col:0x%x\n\n",
				"=== dummy read address: CH",
				channelIdx, rankIdx, dram_addr.row,
				dram_addr.bk, dram_addr.col));

			io_32_write_fld_multi(
				DRAMC_REG_ADDR(DRAMC_REG_RK0_DUMMY_RD_ADR),
				p_fld(dram_addr.row,
				RK0_DUMMY_RD_ADR_DMY_RD_RK0_ROW_ADR) |
				p_fld(dram_addr.col,
				RK0_DUMMY_RD_ADR_DMY_RD_RK0_COL_ADR) |
				p_fld(CLEAR_FLD,
				RK0_DUMMY_RD_ADR_DMY_RD_RK0_LEN));
			io_32_write_fld_align(
				DRAMC_REG_ADDR(DRAMC_REG_RK0_DUMMY_RD_BK),
				dram_addr.bk,
				RK0_DUMMY_RD_BK_DMY_RD_RK0_BK);
		}
	}

	set_phy_2_channel_mapping(p, backup_channel);
	set_rank(p, backup_rank);
}

/* cc notes: keep original code since it use better
 * methods to calculate Read address. REVIEW??
 */
void dramc_dummy_read_for_tracking_enable(DRAMC_CTX_T *p)
{
	io_32_write_fld_align_all(DRAMC_REG_DUMMY_RD, p->support_rank_num,
		DUMMY_RD_RANK_NUM);
	/*Dummy read pattern (Better efficiency during rx dly tracking) */
	io32_write_4b(DRAMC_REG_RK0_DUMMY_RD_WDATA0, PATTERN3);
	io32_write_4b(DRAMC_REG_RK0_DUMMY_RD_WDATA1, PATTERN3);
	io32_write_4b(DRAMC_REG_RK0_DUMMY_RD_WDATA2, PATTERN3);
	io32_write_4b(DRAMC_REG_RK0_DUMMY_RD_WDATA3, PATTERN3);

	io32_write_4b(DRAMC_REG_RK1_DUMMY_RD_WDATA0, PATTERN3);
	io32_write_4b(DRAMC_REG_RK1_DUMMY_RD_WDATA1, PATTERN3);
	io32_write_4b(DRAMC_REG_RK1_DUMMY_RD_WDATA2, PATTERN3);
	io32_write_4b(DRAMC_REG_RK1_DUMMY_RD_WDATA3, PATTERN3);

	/* Dummy Read rank selection is controlled by Test Agent */
	io_32_write_fld_align_all(DRAMC_REG_TEST2_4, 4,
		TEST2_4_TESTAGENTRKSEL);

	dramc_dummy_read_address_setting(p);

	/*
	 * DUMMY_RD_RX_TRACK = 1:
	 * During "RX input delay tracking enable" and "DUMMY_RD_EN=1"
	 * Dummy read will force a read command to a certain rank,
	 * ignoring whether or not EMI has executed a read command
	 * to that certain rank in the past 4us.
	 */
	io_32_write_fld_multi_all(DRAMC_REG_DUMMY_RD,
		p_fld(SET_FLD, DUMMY_RD_DMY_RD_RX_TRACK) |
		p_fld(SET_FLD, DUMMY_RD_DUMMY_RD_EN));
}
#endif

#ifdef IMPEDANCE_TRACKING_ENABLE
void dramc_impedance_tracking_enable(DRAMC_CTX_T *p)
{
	io_32_write_fld_align(DDRPHY_MISC_CTRL0, 0x1, MISC_CTRL0_IMPCAL_CHAB_EN);

	io_32_write_fld_multi_all(DRAMC_REG_IMPCAL,
		p_fld(1, IMPCAL_IMPCAL_HW) |
		p_fld(0, IMPCAL_IMPCAL_EN) |
		p_fld(1, IMPCAL_IMPCAL_SWVALUE_EN) |
		p_fld(1, IMPCAL_IMPCAL_NEW_OLD_SL) |
		p_fld(1, IMPCAL_IMPCAL_DRVUPDOPT) |
		p_fld(1, IMPCAL_IMPCAL_CHGDRV_ECO_OPT) |
		p_fld(1, IMPCAL_IMPCAL_SM_ECO_OPT) |
		p_fld(1, IMPCAL_IMPCAL_ECO_OPT) |
		p_fld(1, IMPCAL_DRV_ECO_OPT));

	io_32_write_fld_align_all(DDRPHY_MISC_CTRL0, 0x1, MISC_CTRL0_IMPCAL_LP_ECO_OPT);

	io_32_write_fld_align_all(DRAMC_REG_IMPCAL, 1, IMPCAL_IMPCAL_BYPASS_UP_CA_DRV);

	io_32_write_fld_multi(DRAMC_REG_IMPCAL,
		p_fld(0, IMPCAL_DIS_SUS_CH0_DRV) |
		p_fld(1, IMPCAL_DIS_SUS_CH1_DRV));
	io_32_write_fld_multi(DRAMC_REG_IMPCAL + SHIFT_TO_CHB_ADDR,
		p_fld(1, IMPCAL_DIS_SUS_CH0_DRV) |
		p_fld(0, IMPCAL_DIS_SUS_CH1_DRV));

	/*for imp track update to io*/
	io_32_write_fld_multi_all(DRAMC_REG_REFCTRL0,
		p_fld(1, REFCTRL0_DRVCGWREF) |
		p_fld(1, REFCTRL0_DQDRVSWUPD));
}

#endif

#ifdef IMPEDANCE_HW_SAVING
void dramc_impedance_hw_saving(DRAMC_CTX_T *p)
{
	io_32_write_fld_align_all(DRAMC_REG_IMPCAL, 1, IMPCAL_IMPCAL_HWSAVE_EN);
	io_32_write_fld_align_all(DRAMC_REG_SHU_IMPCAL1, 3, SHU_IMPCAL1_IMPCALCNT);
	if (p->dram_type == TYPE_LPDDR3) {
		io_32_write_fld_align_all(DRAMC_REG_IMPCAL, CLEAR_FLD, IMPCAL_IMPCAL_HWSAVE_EN);
		io_32_write_fld_align_all(DRAMC_REG_SHU_IMPCAL1, CLEAR_FLD, SHU_IMPCAL1_IMPCALCNT);
	}
}
#endif

/*
 * div_round_closest() - to round up to the nearest integer
 * discard four, but treat five as whole (of decimal points)
 */
int div_round_closest(const int n, const int d)
{
	return ((n < 0) ^ (d < 0)) ? ((n - d / 2) / d):((n + d / 2) / d);
}

/* Test tDQSCK_temp Pre-calculation */

#if DUAL_FREQ_K
static void write_uipi_to_rg_b0(DRAMC_CTX_T *p)
{
	unsigned int value = 0, value_1 = 0;

	/* Step3: Write UI/PI info to RG */
	/* B0 ========================== */
	/* Shuffle 0 =================== */
	value = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG0),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG1),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_RK0_PRE_TDQSCK1), (value << 3) | value_1,
		RK0_PRE_TDQSCK1_TDQSCK_UIFREQ1_B0R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_DQSIEN),
		SHURK0_DQSIEN_R0DQS0IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK1),
		value, RK0_PRE_TDQSCK1_TDQSCK_PIFREQ1_B0R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG0),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG1),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_RK0_PRE_TDQSCK3), (value << 3) | value_1,
		RK0_PRE_TDQSCK3_TDQSCK_UIFREQ1_P1_B0R0);

	/* Shuffle 1 ====================================== */
	value = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG0 + SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG1 + SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK1),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK1_TDQSCK_UIFREQ2_B0R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_DQSIEN + SHU_GRP_DRAMC_OFFSET),
		SHURK0_DQSIEN_R0DQS0IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK1),
		value, RK0_PRE_TDQSCK1_TDQSCK_PIFREQ2_B0R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG0 + SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG1 + SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK3),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK3_TDQSCK_UIFREQ2_P1_B0R0);

	/* Shuffle 2 ====================================== */
	value =	io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG0 + SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG1+ SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK2),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK2_TDQSCK_UIFREQ3_B0R0);

	value =	io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_DQSIEN + SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_DQSIEN_R0DQS0IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK2),
		value, RK0_PRE_TDQSCK2_TDQSCK_PIFREQ3_B0R0);

	value =	io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG0 + SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG1 + SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK3),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK3_TDQSCK_UIFREQ3_P1_B0R0);

	/* Shuffle 3 ===================================== */
#if 0
	value =	io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG0 + SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG1 + SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK2),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK2_TDQSCK_UIFREQ4_B0R0);

	value =	io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_DQSIEN + SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_DQSIEN_R0DQS0IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK2),
		value, RK0_PRE_TDQSCK2_TDQSCK_PIFREQ4_B0R0);

	value =	io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG0 + SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG1 + SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK3),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK3_TDQSCK_UIFREQ4_P1_B0R0);
#endif
}

static void write_uipi_to_rg_b1(DRAMC_CTX_T *p)
{
	unsigned int value = 0, value_1 = 0;
	/* B1 ========================== */
	/* Shuffle 0 ======================= */
	value =	io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG0),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_SELPH_DQSG1),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_RK0_PRE_TDQSCK4), (value << 3) | value_1,
		RK0_PRE_TDQSCK4_TDQSCK_UIFREQ1_B1R0);

	value =	io_32_read_fld_align(DRAMC_REG_ADDR
		(DRAMC_REG_SHURK0_DQSIEN), SHURK0_DQSIEN_R0DQS1IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK4),
		value, RK0_PRE_TDQSCK4_TDQSCK_PIFREQ1_B1R0);

	value =	io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK6),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK6_TDQSCK_UIFREQ1_P1_B1R0);

	/* Shuffle 1 ====================================== */
	value =	io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK4),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK4_TDQSCK_UIFREQ2_B1R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN +
		SHU_GRP_DRAMC_OFFSET),
		SHURK0_DQSIEN_R0DQS1IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK4),
		value, RK0_PRE_TDQSCK4_TDQSCK_PIFREQ2_B1R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK6),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK6_TDQSCK_UIFREQ2_P1_B1R0);

	/* Shuffle 2 ====================================== */
	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK5),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK5_TDQSCK_UIFREQ3_B1R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_DQSIEN_R0DQS1IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK5),
		value, RK0_PRE_TDQSCK5_TDQSCK_PIFREQ3_B1R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK6),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK6_TDQSCK_UIFREQ3_P1_B1R0);

	/* Shuffle 3 ===================================== */
#if 0
	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK5),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK5_TDQSCK_UIFREQ4_B1R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN +
		SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_DQSIEN_R0DQS1IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK5),
		value, RK0_PRE_TDQSCK5_TDQSCK_PIFREQ4_B1R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK6),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK6_TDQSCK_UIFREQ4_P1_B1R0);
#endif
}

static void write_uipi_to_rg_b2(DRAMC_CTX_T *p)
{
	unsigned int value = 0, value_1 = 0;
	/* B2 ========================== */
	/* Shuffle 0 ====================================== */
	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK7),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK7_TDQSCK_UIFREQ1_B2R0);

	value =	io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN),
		SHURK0_DQSIEN_R0DQS2IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK7),
		value, RK0_PRE_TDQSCK7_TDQSCK_PIFREQ1_B2R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK9),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK9_TDQSCK_UIFREQ1_P1_B2R0);

	/* Shuffle 1 ====================================== */
	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK7),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK7_TDQSCK_UIFREQ2_B2R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN +
		SHU_GRP_DRAMC_OFFSET),
		SHURK0_DQSIEN_R0DQS2IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK7),
		value, RK0_PRE_TDQSCK7_TDQSCK_PIFREQ2_B2R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK9),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK9_TDQSCK_UIFREQ2_P1_B2R0);

	/* Shuffle 2 ===================================== */
	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK8),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK8_TDQSCK_UIFREQ3_B2R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_DQSIEN_R0DQS2IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK8),
		value, RK0_PRE_TDQSCK8_TDQSCK_PIFREQ3_B2R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK9),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK9_TDQSCK_UIFREQ3_P1_B2R0);

	/* Shuffle 3 ====================================== */
#if 0
	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK8),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK8_TDQSCK_UIFREQ4_B2R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN +
		SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_DQSIEN_R0DQS2IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK8),
		value, RK0_PRE_TDQSCK8_TDQSCK_PIFREQ4_B2R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS2_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS2_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK9),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK9_TDQSCK_UIFREQ4_P1_B2R0);
#endif
}

static void write_uipi_to_rg_b3(DRAMC_CTX_T *p)
{
	unsigned int value = 0, value_1 = 0;
	/* B3 ========================== */
	/* Shuffle 0 ====================================== */
	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK10),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK10_TDQSCK_UIFREQ1_B3R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN),
		SHURK0_DQSIEN_R0DQS3IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK10),
		value, RK0_PRE_TDQSCK10_TDQSCK_PIFREQ1_B3R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK12),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK12_TDQSCK_UIFREQ1_P1_B3R0);

	/* Shuffle 1 ===================================== */
	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK10),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK10_TDQSCK_UIFREQ2_B3R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN +
		SHU_GRP_DRAMC_OFFSET), SHURK0_DQSIEN_R0DQS3IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK10),
		value, RK0_PRE_TDQSCK10_TDQSCK_PIFREQ2_B3R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK12),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK12_TDQSCK_UIFREQ2_P1_B3R0);

	/* Shuffle 2 ====================================== */
	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK11),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK11_TDQSCK_UIFREQ3_B3R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_DQSIEN_R0DQS3IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK11),
		value, RK0_PRE_TDQSCK11_TDQSCK_PIFREQ3_B3R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET*2),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK12),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK12_TDQSCK_UIFREQ3_P1_B3R0);

	/* Shuffle 3 ====================================== */
#if 0
	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
	SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK11),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK11_TDQSCK_UIFREQ4_B3R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_DQSIEN +
		SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_DQSIEN_R0DQS3IEN);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK11),
		value, RK0_PRE_TDQSCK11_TDQSCK_PIFREQ4_B3R0);

	value = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG0 +
		SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG0_TX_DLY_DQS3_GATED_P1);
	value_1 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHURK0_SELPH_DQSG1 +
		SHU_GRP_DRAMC_OFFSET*3),
		SHURK0_SELPH_DQSG1_REG_DLY_DQS3_GATED_P1);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RK0_PRE_TDQSCK12),
		(value << 3) | value_1,
		RK0_PRE_TDQSCK12_TDQSCK_UIFREQ4_P1_B3R0);
#endif
}

void dramc_dqs_precalculation_preset(DRAMC_CTX_T *p)
{
	DRAM_CHANNEL_T channel_back = p->channel;
	DRAM_RANK_T rank_back = p->rank;
	unsigned char irank_num = 0, ichannel_num = 0;
	int shuffle_src, shuffle_dst, jump_ratio_index;
	unsigned short jump_ratio[6] = {0};
	memset(jump_ratio, 0, sizeof(jump_ratio));

	/* Calculate jump ratios and save to jump_ratio array */
	jump_ratio_index = 0;
	for (shuffle_src = DRAM_DFS_SHUFFLE_1;
		shuffle_src < HW_REG_SHUFFLE_MAX; shuffle_src++) {
		for (shuffle_dst = DRAM_DFS_SHUFFLE_1;
			shuffle_dst < HW_REG_SHUFFLE_MAX; shuffle_dst++) {
			if (shuffle_src == shuffle_dst)
				continue;
			if (shuffle_src >= DRAM_DFS_SHUFFLE_MAX ||
				shuffle_dst >= DRAM_DFS_SHUFFLE_MAX) {
				jump_ratio_index++;
				continue;
			}
			jump_ratio[jump_ratio_index] =
				div_round_closest(
				(p->shuffle_frequency[shuffle_dst] * 32),
				(p->shuffle_frequency[shuffle_src]));
			show_msg3((INFO,
				 "%s[%d]: %x\tFreq %d -> %d\tDDR%d -> DDR%d\n",
				 "Jump_RATIO ", jump_ratio_index,
				 jump_ratio[jump_ratio_index],
				 shuffle_src+1, shuffle_dst+1,
				 p->shuffle_frequency[shuffle_src]<<1,
				 p->shuffle_frequency[shuffle_dst]<<1));

			jump_ratio_index++;
		}
	}

	for (ichannel_num = 0; ichannel_num < p->support_channel_num;
		ichannel_num++) {
		set_phy_2_channel_mapping(p, ichannel_num);

		show_log("Step1: Set DVFS HW enable\n");
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_PRE_TDQSCK1),
			SET_FLD,	PRE_TDQSCK1_TDQSCK_PRECAL_HW);

		/* Save jumpRatios into corresponding register fields */
		show_log("Step2: Set jump ratio\n");
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_PRE_TDQSCK2),
			p_fld(jump_ratio[0], PRE_TDQSCK2_TDDQSCK_JUMP_RATIO0) |
			p_fld(jump_ratio[1], PRE_TDQSCK2_TDDQSCK_JUMP_RATIO1) |
			p_fld(jump_ratio[2], PRE_TDQSCK2_TDDQSCK_JUMP_RATIO3));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_PRE_TDQSCK3),
			p_fld(jump_ratio[3], PRE_TDQSCK3_TDDQSCK_JUMP_RATIO4) |
			p_fld(jump_ratio[4], PRE_TDQSCK3_TDDQSCK_JUMP_RATIO6) |
			p_fld(jump_ratio[5], PRE_TDQSCK3_TDDQSCK_JUMP_RATIO7));

		for (irank_num = 0; irank_num < 2; irank_num++) {
			set_rank(p, irank_num);

			/* Step3: Write UI/PI info to RG */
			/* B0 ========================== */
			write_uipi_to_rg_b0(p);

			/* B1 ========================== */
			write_uipi_to_rg_b1(p);

			/*
			 * if Project is combo LP3+LP4,
			 * then needs to set B2 and B3
			 */
			/* B2 ========================== */
			write_uipi_to_rg_b2(p);

			/* B3 ========================== */
			write_uipi_to_rg_b3(p);

		}
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_PRE_TDQSCK1),
			p_fld(SET_FLD, PRE_TDQSCK1_TDQSCK_REG_DVFS));
		/* Step4: Set Auto save to RG */
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_PRE_TDQSCK1),
			SET_FLD,	PRE_TDQSCK1_TDQSCK_HW_SW_UP_SEL);
	}
	set_phy_2_channel_mapping(p, channel_back);
	set_rank(p, rank_back);
}
#endif /* DUAL_FREQ_K */

#if (CPU_RW_TEST_AFTER_K)
void dramc_dump_debug_info(DRAMC_CTX_T *p)
{
	unsigned char mpdivInSel, cali_shu_sel, mpdiv_shu_sel;
	DRAM_CHANNEL_T ch;
#if !FIRST_BRING_UP
#ifdef TEMP_SENSOR_ENABLE
	unsigned char refresh_rate;
#endif
#if (CPU_RW_TEST_AFTER_K)
	//DRAM_CHANNEL_T ch;
#endif
#endif
	mpdivInSel = io_32_read_fld_align
		(DDRPHY_MISC_CTRL2, MISC_CTRL2_CLRPLL_SHU_GP);
	cali_shu_sel = io_32_read_fld_align
		(DRAMC_REG_SHUCTRL, SHUCTRL_R_OTHER_SHU_GP);
	mpdiv_shu_sel = io_32_read_fld_align
		(DRAMC_REG_SHUCTRL, SHUCTRL_R_MPDIV_SHU_GP);

	/* Read shuffle selection */
	show_msg((INFO, "\n\n[DumpDebugInfo]\n"
		"\tmpdivInSel %d, cali_shu_sel %d, mpdiv_shu_sel %d\n",
		mpdivInSel, cali_shu_sel, mpdiv_shu_sel));

	/* Read HW gating tracking */
#ifdef HW_GATING
	for (ch = CHANNEL_A;
		(unsigned char)ch < (unsigned char)(p->support_channel_num);
		ch++)
		dramc_print_hw_gating_status(p, ch);
#endif

#if ENABLE_RX_TRACKING_LP4
	for (ch = CHANNEL_A;
		(unsigned char)ch < (unsigned char)(p->support_channel_num);
		ch++)
		dramc_print_rxdqdqs_status(p, ch);
#endif

#ifdef IMPEDANCE_TRACKING_ENABLE
	for (ch = CHANNEL_A;
		(unsigned char)ch < (unsigned char)(p->support_channel_num);
		ch++)
		dramc_print_imp_tracking_status(p, ch);
#endif

#ifdef TEMP_SENSOR_ENABLE
	for (ch = CHANNEL_A;
		(unsigned char)ch < (unsigned char)(p->support_channel_num);
		ch++) {
#if SUPPORT_TYPE_LPDDR4
		refresh_rate = get_mr4_refresh_rate(p, ch);
#endif
		show_err3("[CH%d] MRR(MR4) [10:8]=%x\n", ch, refresh_rate);
	}
#endif
}
#endif

void auto_refresh_switch(DRAMC_CTX_T *p, unsigned char option)
{
	if (option == ENABLE) {
		/* enable autorefresh, REFDIS=0, enable auto refresh */
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0),
			CLEAR_FLD, REFCTRL0_REFDIS);
	} else {
		/* disable autorefresh, REFDIS=1, disable auto refresh */
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0),
			SET_FLD, REFCTRL0_REFDIS);

		/*
		 * because HW will actually disable autorefresh
		 * after refresh_queue empty, so we need to wait quene empty.
		 */
		/* wait refresh_queue_cnt * 3.9us */
		delay_us(io_32_read_fld_align(
			DRAMC_REG_ADDR(DRAMC_REG_MISC_STATUSA),
			MISC_STATUSA_REFRESH_QUEUE_CNT) * 4);
	}
}

#ifdef SPM_LIB_USE
#ifdef SPM_CONTROL_AFTERK
/* cc porting from M */
void transfer_to_spm_control(DRAMC_CTX_T *p)
{
	if (is_lp4_family(p)) {
		io_32_write_fld_align_all(DDRPHY_MISC_SPM_CTRL0,
			0x3ff67f3, MISC_SPM_CTRL0_PHY_SPM_CTL0);
		io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL2, 0xff6f7fe0,
			MISC_SPM_CTRL2_PHY_SPM_CTL2);
		io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL2 + SHIFT_TO_CHB_ADDR,
			0x7f6f7fe0, MISC_SPM_CTRL2_PHY_SPM_CTL2);
	} else {
#if SUPPORT_TYPE_LPDDR3
		io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL0,
			0xfbffefff, MISC_SPM_CTRL0_PHY_SPM_CTL0);
		io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL0 + SHIFT_TO_CHB_ADDR,
			0x03fe75c3, MISC_SPM_CTRL0_PHY_SPM_CTL0);
		io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL2, 0x7fffffef,
			MISC_SPM_CTRL2_PHY_SPM_CTL2);
		io_32_write_fld_align(DDRPHY_MISC_SPM_CTRL2 + SHIFT_TO_CHB_ADDR,
			0x7e4e4000, MISC_SPM_CTRL2_PHY_SPM_CTL2);
#endif
	}
}
#endif

void transfer_pll_to_spm_control(DRAMC_CTX_T *p)
{
	unsigned char shu_level;

	shu_level = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHUSTATUS),
		SHUSTATUS_SHUFFLE_LEVEL);

	/* TINFO="DRAM:enter SW DVFS" */
	/* TINFO="DRAM:SPM presetting for pinmux" */
	/* ! set SPM project code and enable clock enable */
	io_32_write_fld_multi(SPM_POWERON_CONFIG_EN,
		p_fld(0xb16, POWERON_CONFIG_EN_PROJECT_CODE) |
		p_fld(SET_FLD, POWERON_CONFIG_EN_BCLK_CG_EN));

	/* ! set SPM pinmux */
	io_32_write_fld_multi(SPM_PCM_PWR_IO_EN,
		p_fld(CLEAR_FLD, PCM_PWR_IO_EN_RG_PCM_PWR_IO_EN) |
		p_fld(CLEAR_FLD, PCM_PWR_IO_EN_RG_RF_SYNC_EN));

	if (is_lp4_family(p))
		io_32_write_fld_align(SPM_DRAMC_DPY_CLK_SW_CON_SEL, 0x00ffffff,
		DRAMC_DPY_CLK_SW_CON_SEL_FULL);
	else
		io_32_write_fld_align(SPM_DRAMC_DPY_CLK_SW_CON_SEL, 0xaaffffff,
			DRAMC_DPY_CLK_SW_CON_SEL_FULL);
	io_32_write_fld_align(SPM_DRAMC_DPY_CLK_SW_CON_SEL2, 0xffffffff,
		DRAMC_DPY_CLK_SW_CON_SEL2_FULL);

	/*
	 * ! set  sc_dpy_2nd_dll_en, sc_dpy_dll_en, sc_dpy_dll_ck_en ,
	 * sc_dpy_vref_en , sc_phypll_en = 1
	 */
	io_32_write_fld_multi(SPM_POWER_ON_VAL0,
		p_fld(SET_FLD, SPM_POWER_ON_VAL0_SC_DPY_2ND_DLL_EN_PCM) |
		p_fld(SET_FLD, SPM_POWER_ON_VAL0_SC_DPY_DLL_EN_PCM) |
		p_fld(SET_FLD, SPM_POWER_ON_VAL0_SC_DPY_DLL_CK_EN_PCM) |
		p_fld(SET_FLD, SPM_POWER_ON_VAL0_SC_DPY_VREF_EN_PCM) |
		p_fld(SET_FLD, SPM_POWER_ON_VAL0_SC_PHYPLL_EN_PCM));

	if (shu_level == 1) {
		show_msg((INFO, "CLRPLL\n"));
		io_32_write_fld_align(SPM_POWER_ON_VAL0, CLEAR_FLD,
			SPM_POWER_ON_VAL0_SC_PHYPLL_MODE_SW_PCM);
		io_32_write_fld_align(SPM_POWER_ON_VAL0, SET_FLD,
			SPM_POWER_ON_VAL0_SC_PHYPLL2_MODE_SW_PCM);
		io_32_write_fld_align(SPM_DRAMC_DPY_CLK_SW_CON2, CLEAR_FLD,
			DRAMC_DPY_CLK_SW_CON2_SW_PHYPLL_MODE_SW);
		io_32_write_fld_align(SPM_DRAMC_DPY_CLK_SW_CON2, SET_FLD,
			DRAMC_DPY_CLK_SW_CON2_SW_PHYPLL2_MODE_SW);
	} else {
		show_msg((INFO, "PHYPLL\n"));
		io_32_write_fld_align(SPM_POWER_ON_VAL0, CLEAR_FLD,
			SPM_POWER_ON_VAL0_SC_PHYPLL2_MODE_SW_PCM);
		io_32_write_fld_align(SPM_POWER_ON_VAL0, SET_FLD,
			SPM_POWER_ON_VAL0_SC_PHYPLL_MODE_SW_PCM);
		io_32_write_fld_align(SPM_DRAMC_DPY_CLK_SW_CON2, CLEAR_FLD,
			DRAMC_DPY_CLK_SW_CON2_SW_PHYPLL2_MODE_SW);
		io_32_write_fld_align(SPM_DRAMC_DPY_CLK_SW_CON2, SET_FLD,
			DRAMC_DPY_CLK_SW_CON2_SW_PHYPLL_MODE_SW);
	}

	delay_us(1);

	io_32_write_fld_align_all(DDRPHY_PLL1, CLEAR_FLD, PLL1_RG_RPHYPLL_EN);
	io_32_write_fld_align_all(DDRPHY_PLL2, CLEAR_FLD, PLL2_RG_RCLRPLL_EN);

#if ENABLE_RX_TRACKING_LP4
	if (p->frequency >= DDR3200_FREQ)
		io_32_write_fld_align(SPM_POWER_ON_VAL0, SET_FLD,
		SPM_POWER_ON_VAL0_SC_DPHY_RXDLY_TRACK_EN);
#endif
}

void spm_pinmux_setting(DRAMC_CTX_T *p)
{
	/* ! set SPM project code and enable clock enable */
	io_32_write_fld_multi(SPM_POWERON_CONFIG_EN,
		p_fld(0xb16, POWERON_CONFIG_EN_PROJECT_CODE) |
		p_fld(SET_FLD, POWERON_CONFIG_EN_BCLK_CG_EN));

	/* ! set SPM pinmux */
	io_32_write_fld_multi(SPM_PCM_PWR_IO_EN,
		p_fld(CLEAR_FLD, PCM_PWR_IO_EN_RG_PCM_PWR_IO_EN) |
		p_fld(CLEAR_FLD, PCM_PWR_IO_EN_RG_RF_SYNC_EN));

	if (is_lp4_family(p)) //from m
		io_32_write_fld_align(SPM_DRAMC_DPY_CLK_SW_CON_SEL, 0x00ffffff,	DRAMC_DPY_CLK_SW_CON_SEL_FULL);
	else
		io_32_write_fld_align(SPM_DRAMC_DPY_CLK_SW_CON_SEL, 0xaaffffff,	DRAMC_DPY_CLK_SW_CON_SEL_FULL);

	io_32_write_fld_align(SPM_DRAMC_DPY_CLK_SW_CON_SEL2, 0xffffffff, DRAMC_DPY_CLK_SW_CON_SEL2_FULL);
}
#endif

#if SUPPORT_TYPE_LPDDR4
/* LP4 2CH */
void lp4_enable_dramc_phydcm_2ch(DRAMC_CTX_T *p, unsigned char en)//Should refer to "vSetChannelNumber"
{
	unsigned int shu_index = 0;
	unsigned char odt_on = 0;

	if (en) {
		io_32_write_fld_multi_all(DDRPHY_MISC_CG_CTRL0,
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_INFRA_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_RX_COMB1_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_RX_COMB0_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_RX_CMD_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_COMB1_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_COMB0_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_CMD_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_COMB_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_PHY_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_DRAMC_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_EMI_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_DRAMC_CHB_CK_OFF));
		for (shu_index = DRAM_DFS_SHUFFLE_1; shu_index <
			DRAM_DFS_SHUFFLE_MAX; shu_index++) {

			if (p->dfs_table->frequency >= LP4_MIDDLE_FREQ)
				odt_on = 1;
			else
				odt_on = 0;

			io_32_write_fld_multi_all(DDRPHY_SHU_B0_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMSTBEN_SYNC_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
				p_fld(!odt_on, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0));
			io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMSTBEN_SYNC_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
				p_fld(!odt_on, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1));
			io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_B2_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRANK_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMSTBEN_SYNC_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRXDLY_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_RMRX_TOPHY_CG_IG_B2) |
				p_fld(!odt_on, SHU_B2_DQ8_R_RMRODTEN_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B2));
			io_32_write_fld_multi_all(DDRPHY_SHU_CA_CMD8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_CHG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_FLAG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_FLAG_SYNC_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMSTBEN_SYNC_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDLY_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDVS_RDSEL_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_RMRX_TOPHY_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_RMRODTEN_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_RXDLY_PIPE_CG_IG_CA));//19th
		}
		io_32_write_fld_multi_all(DDRPHY_MISC_CG_CTRL5,
			p_fld(0x1, MISC_CG_CTRL5_R_CA_DLY_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ0_DLY_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ1_DLY_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ2_DLY_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_CA_PI_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ0_PI_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ1_PI_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ2_PI_DCM_EN));

	} else {
		io_32_write_fld_multi_all(DDRPHY_MISC_CG_CTRL0,
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_INFRA_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_RX_COMB1_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_RX_COMB0_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_RX_CMD_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_COMB1_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_COMB0_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_CMD_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_COMB_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_PHY_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_DRAMC_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_EMI_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_DRAMC_CHB_CK_OFF));

		for (shu_index = DRAM_DFS_SHUFFLE_1; shu_index <
			DRAM_DFS_SHUFFLE_MAX; shu_index++) {
			io_32_write_fld_multi_all(DDRPHY_SHU_B0_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x1, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMSTBEN_SYNC_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0));//19th
			io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x1, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMSTBEN_SYNC_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1));//19th
			io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x1, SHU_B2_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMRANK_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMSTBEN_SYNC_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMRXDLY_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_RMRX_TOPHY_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_RMRODTEN_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B2));//19th
			io_32_write_fld_multi_all(DDRPHY_SHU_CA_CMD8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_CHG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_FLAG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_FLAG_SYNC_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMSTBEN_SYNC_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDLY_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDVS_RDSEL_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_RMRX_TOPHY_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_RMRODTEN_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_RXDLY_PIPE_CG_IG_CA));//19th
		}
		io_32_write_fld_multi_all(DDRPHY_MISC_CG_CTRL5,
			p_fld(0x0, MISC_CG_CTRL5_R_CA_DLY_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ0_DLY_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ1_DLY_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ2_DLY_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_CA_PI_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ0_PI_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ1_PI_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ2_PI_DCM_EN));
	}

	io_32_write_fld_multi_all(DDRPHY_B0_DLL_ARPI2,
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCK_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCTL_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_FB_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQS_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQM_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQ_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
	io_32_write_fld_multi_all(DDRPHY_B0_DLL_ARPI3,
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_MCTL_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_FB_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQS_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQM_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQ_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B0));
	io_32_write_fld_multi_all(DDRPHY_B1_DLL_ARPI2,
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCK_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCTL_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_FB_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQS_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQM_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQ_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));
	io_32_write_fld_multi_all(DDRPHY_B1_DLL_ARPI3,
		p_fld(0x1, B1_DLL_ARPI3_RG_ARPI_MCTL_EN_B1) |
		p_fld(0x1, B1_DLL_ARPI3_RG_ARPI_FB_EN_B1) |
		p_fld(0x1, B1_DLL_ARPI3_RG_ARPI_DQS_EN_B1) |
		p_fld(0x1, B1_DLL_ARPI3_RG_ARPI_DQM_EN_B1) |
		p_fld(0x1, B1_DLL_ARPI3_RG_ARPI_DQ_EN_B1) |
		p_fld(0x1, B1_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B1));
	io_32_write_fld_multi_all(DDRPHY_B2_DLL_ARPI2,
		p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_MCK_B2) |
		p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B2) |
		p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_MCTL_B2) |
		p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_FB_B2) |
		p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_DQS_B2) |
		p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_DQM_B2) |
		p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_DQ_B2) |
		p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B2) |
		p_fld(0x0, B2_DLL_ARPI2_RG_ARPI_MPDIV_CG_B2));
	io_32_write_fld_multi_all(DDRPHY_B2_DLL_ARPI3,
		p_fld(0x1, B2_DLL_ARPI3_RG_ARPI_MCTL_EN_B2) |
		p_fld(0x1, B2_DLL_ARPI3_RG_ARPI_FB_EN_B2) |
		p_fld(0x1, B2_DLL_ARPI3_RG_ARPI_DQS_EN_B2) |
		p_fld(0x1, B2_DLL_ARPI3_RG_ARPI_DQM_EN_B2) |
		p_fld(0x1, B2_DLL_ARPI3_RG_ARPI_DQ_EN_B2) |
		p_fld(0x1, B2_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B2));
	io_32_write_fld_multi_all(DDRPHY_CA_DLL_ARPI2,
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_FB_CA) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CS) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CLK) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CMD) |
		p_fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));
	io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI3,
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_FB_EN_CA) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_CS_EN) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_CLK_EN) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_CMD_EN) |
		p_fld(0x0, CA_DLL_ARPI3_RG_ARPI_CLKIEN_EN));
	io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI3 + SHIFT_TO_CHB_ADDR,
		p_fld(0x0, CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_FB_EN_CA) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_CS_EN) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_CLK_EN) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_CMD_EN) |
		p_fld(0x0, CA_DLL_ARPI3_RG_ARPI_CLKIEN_EN));

	return;
}
#endif

#if SUPPORT_TYPE_LPDDR3
/* LP3 */
void lp3_enable_dramc_phydcm(DRAMC_CTX_T *p, unsigned char en)//Should refer to "vSetChannelNumber"
{
	unsigned int shu_index = 0;

	if (en) {
		io_32_write_fld_multi(DDRPHY_MISC_CG_CTRL0,
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_INFRA_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_RX_COMB1_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_RX_COMB0_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_RX_CMD_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_COMB1_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_COMB0_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_CMD_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_COMB_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_PHY_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_DRAMC_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_EMI_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_DRAMC_CHB_CK_OFF));

		io_32_write_fld_multi(SHIFT_TO_CHB_ADDR + DDRPHY_MISC_CG_CTRL0,
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_INFRA_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_RX_COMB1_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_RX_COMB0_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_RX_CMD_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_COMB1_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_COMB0_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_CMD_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_COMB_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_PHY_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_DRAMC_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_EMI_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_DRAMC_CHB_CK_OFF));

		for (shu_index = DRAM_DFS_SHUFFLE_1; shu_index <
			DRAM_DFS_SHUFFLE_MAX; shu_index++) {
			io_32_write_fld_multi(DDRPHY_SHU_B0_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMSTBEN_SYNC_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0));
			io_32_write_fld_multi(SHIFT_TO_CHB_ADDR + DDRPHY_SHU_B0_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMSTBEN_SYNC_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0));
			io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMSTBEN_SYNC_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) |
				p_fld(0x0, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1));
			io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_B2_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRANK_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMSTBEN_SYNC_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRXDLY_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_RMRX_TOPHY_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_RMRODTEN_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B2));
			io_32_write_fld_multi(DDRPHY_SHU_CA_CMD8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_CHG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_FLAG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_FLAG_SYNC_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMSTBEN_SYNC_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDLY_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDVS_RDSEL_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_RMRX_TOPHY_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_RMRODTEN_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_RXDLY_PIPE_CG_IG_CA));

			io_32_write_fld_multi(SHIFT_TO_CHB_ADDR + DDRPHY_SHU_CA_CMD8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_CHG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_FLAG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_FLAG_SYNC_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMSTBEN_SYNC_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDLY_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDVS_RDSEL_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_RMRX_TOPHY_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_RMRODTEN_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_RXDLY_PIPE_CG_IG_CA));
		}

		io_32_write_fld_multi(DDRPHY_MISC_CG_CTRL5,
			p_fld(0x1, MISC_CG_CTRL5_R_CA_DLY_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ0_DLY_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ1_DLY_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_CA_PI_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ0_PI_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ1_PI_DCM_EN));
		io_32_write_fld_multi(SHIFT_TO_CHB_ADDR + DDRPHY_MISC_CG_CTRL5,
			p_fld(0x1, MISC_CG_CTRL5_R_CA_DLY_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ0_DLY_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ1_DLY_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_CA_PI_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ0_PI_DCM_EN) |
			p_fld(0x1, MISC_CG_CTRL5_R_DQ1_PI_DCM_EN));
	} else {
		io_32_write_fld_multi_all(DDRPHY_MISC_CG_CTRL0,
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_INFRA_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_RX_COMB1_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_RX_COMB0_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_RX_CMD_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_COMB1_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_COMB0_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_CMD_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_COMB_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_PHY_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_DRAMC_OFF_DISABLE) |
			p_fld(0x1, MISC_CG_CTRL0_RG_CG_EMI_OFF_DISABLE) |
			p_fld(0x0, MISC_CG_CTRL0_RG_CG_DRAMC_CHB_CK_OFF));
		for (shu_index = DRAM_DFS_SHUFFLE_1; shu_index <
			DRAM_DFS_SHUFFLE_MAX; shu_index++) {
			io_32_write_fld_multi(DDRPHY_SHU_B0_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMSTBEN_SYNC_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) |
				p_fld(0x0, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0));
			io_32_write_fld_multi(SHIFT_TO_CHB_ADDR + DDRPHY_SHU_B0_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x1, SHU_B0_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMRANK_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMSTBEN_SYNC_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMRXDLY_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_RMRX_TOPHY_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_RMRODTEN_CG_IG_B0) |
				p_fld(0x1, SHU_B0_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B0));
			io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x1, SHU_B1_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMRANK_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMSTBEN_SYNC_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMRXDLY_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_RMRX_TOPHY_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_RMRODTEN_CG_IG_B1) |
				p_fld(0x1, SHU_B1_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B1));
			io_32_write_fld_multi(DDRPHY_SHU_B2_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_B2_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRANK_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMSTBEN_SYNC_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRXDLY_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_RMRX_TOPHY_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_RMRODTEN_CG_IG_B2) |
				p_fld(0x0, SHU_B2_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B2));
			io_32_write_fld_multi(SHIFT_TO_CHB_ADDR + DDRPHY_SHU_B2_DQ8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x1, SHU_B2_DQ8_R_DMRANK_CHG_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMRANK_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMDQSIEN_FLAG_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMDQSIEN_FLAG_SYNC_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMSTBEN_SYNC_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMRXDLY_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMRXDVS_RDSEL_PIPE_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_RMRX_TOPHY_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_RMRODTEN_CG_IG_B2) |
				p_fld(0x1, SHU_B2_DQ8_R_DMRANK_RXDLY_PIPE_CG_IG_B2));
			io_32_write_fld_multi(DDRPHY_SHU_CA_CMD8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_CHG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_FLAG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMDQSIEN_FLAG_SYNC_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMSTBEN_SYNC_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDLY_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRXDVS_RDSEL_PIPE_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_RMRX_TOPHY_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_RMRODTEN_CG_IG_CA) |
				p_fld(0x0, SHU_CA_CMD8_R_DMRANK_RXDLY_PIPE_CG_IG_CA));
			io_32_write_fld_multi(SHIFT_TO_CHB_ADDR + DDRPHY_SHU_CA_CMD8 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x1, SHU_CA_CMD8_R_DMRANK_CHG_PIPE_CG_IG_CA) |
				p_fld(0x1, SHU_CA_CMD8_R_DMRANK_PIPE_CG_IG_CA) |
				p_fld(0x1, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_TOG_PIPE_CG_IG_CA) |
				p_fld(0x1, SHU_CA_CMD8_R_DMDQSIEN_RDSEL_PIPE_CG_IG_CA) |
				p_fld(0x1, SHU_CA_CMD8_R_DMDQSIEN_FLAG_PIPE_CG_IG_CA) |
				p_fld(0x1, SHU_CA_CMD8_R_DMDQSIEN_FLAG_SYNC_CG_IG_CA) |
				p_fld(0x1, SHU_CA_CMD8_R_DMSTBEN_SYNC_CG_IG_CA) |
				p_fld(0x1, SHU_CA_CMD8_R_DMRXDLY_CG_IG_CA) |
				p_fld(0x1, SHU_CA_CMD8_R_DMRXDVS_RDSEL_TOG_PIPE_CG_IG_CA) |
				p_fld(0x1, SHU_CA_CMD8_R_DMRXDVS_RDSEL_PIPE_CG_IG_CA) |
				p_fld(0x1, SHU_CA_CMD8_R_RMRX_TOPHY_CG_IG_CA) |
				p_fld(0x1, SHU_CA_CMD8_R_RMRODTEN_CG_IG_CA) |
				p_fld(0x1, SHU_CA_CMD8_R_DMRANK_RXDLY_PIPE_CG_IG_CA));
	}
		io_32_write_fld_multi_all(DDRPHY_MISC_CG_CTRL5,
			p_fld(0x0, MISC_CG_CTRL5_R_CA_PI_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ0_PI_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ1_PI_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_CA_DLY_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ0_DLY_DCM_EN) |
			p_fld(0x0, MISC_CG_CTRL5_R_DQ1_DLY_DCM_EN));
	}

	io_32_write_fld_multi(DDRPHY_B0_DLL_ARPI2,
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCK_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCTL_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_FB_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQS_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQM_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQ_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
	io_32_write_fld_multi(DDRPHY_B0_DLL_ARPI2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCK_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_MCTL_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_FB_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQS_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQM_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQ_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0) |
		p_fld(0x0, B0_DLL_ARPI2_RG_ARPI_MPDIV_CG_B0));
	io_32_write_fld_multi(DDRPHY_B0_DLL_ARPI3,
		p_fld(0x0, B0_DLL_ARPI3_RG_ARPI_MCTL_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_FB_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQS_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQM_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQ_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B0));
	io_32_write_fld_multi(DDRPHY_B0_DLL_ARPI3 + SHIFT_TO_CHB_ADDR,
		p_fld(0x0, B0_DLL_ARPI3_RG_ARPI_MCTL_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_FB_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQS_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQM_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQ_EN_B0) |
		p_fld(0x1, B0_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B0));
	io_32_write_fld_multi_all(DDRPHY_B1_DLL_ARPI2,
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCK_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_MCTL_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_FB_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQS_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQM_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQ_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1) |
		p_fld(0x0, B1_DLL_ARPI2_RG_ARPI_MPDIV_CG_B1));
	io_32_write_fld_multi_all(DDRPHY_B1_DLL_ARPI3,
		p_fld(0x0, B1_DLL_ARPI3_RG_ARPI_MCTL_EN_B1) |
		p_fld(0x1, B1_DLL_ARPI3_RG_ARPI_FB_EN_B1) |
		p_fld(0x1, B1_DLL_ARPI3_RG_ARPI_DQS_EN_B1) |
		p_fld(0x1, B1_DLL_ARPI3_RG_ARPI_DQM_EN_B1) |
		p_fld(0x1, B1_DLL_ARPI3_RG_ARPI_DQ_EN_B1) |
		p_fld(0x1, B1_DLL_ARPI3_RG_ARPI_DQSIEN_EN_B1));
	io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI2,
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_FB_CA) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CS) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CLK) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CMD) |
		p_fld(0x1, CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));
	io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI2 + SHIFT_TO_CHB_ADDR,
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCK_CA) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCK_FB2DLL_CA) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_MCTL_CA) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_FB_CA) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CS) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CLK) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CMD) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_CG_CLKIEN) |
		p_fld(0x0, CA_DLL_ARPI2_RG_ARPI_MPDIV_CG_CA));
	io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI3,
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_FB_EN_CA) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_CS_EN) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_CLK_EN) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_CMD_EN) |
		p_fld(0x0, CA_DLL_ARPI3_RG_ARPI_CLKIEN_EN));
	io_32_write_fld_multi(DDRPHY_CA_DLL_ARPI3 + SHIFT_TO_CHB_ADDR,
		p_fld(0x0, CA_DLL_ARPI3_RG_ARPI_MCTL_EN_CA) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_FB_EN_CA) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_CS_EN) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_CLK_EN) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_CMD_EN) |
		p_fld(0x1, CA_DLL_ARPI3_RG_ARPI_CLKIEN_EN));

	return;
}
#endif

void enable_dramc_phy_dcm(DRAMC_CTX_T *p, unsigned char en)
{
#if SUPPORT_TYPE_LPDDR4 || SUPPORT_TYPE_LPDDR3
	unsigned int broadcast_bkp = get_dramc_broadcast();
	unsigned int shu_index = 0;

	if(!(is_lp4_family(p) || p->dram_type == TYPE_LPDDR3)) {
		return;
	}

	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);

	io_32_write_fld_align_all(DDRPHY_MISC_CTRL0, 0x1, MISC_CTRL0_R_DMSHU_PHYDCM_FORCEOFF);

	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI1, 0x0, B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0_REG_OPT);
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI1, 0x0, B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1_REG_OPT);
	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI1, 0x0, B2_DLL_ARPI1_RG_ARPISM_MCK_SEL_B2_REG_OPT);
	io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI1, 0x0, CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA_REG_OPT);

	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI1, 0x1, B0_DLL_ARPI1_RG_ARPISM_MCK_SEL_B0);
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI1, 0x1, B1_DLL_ARPI1_RG_ARPISM_MCK_SEL_B1);
	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI1, 0x1, B2_DLL_ARPI1_RG_ARPISM_MCK_SEL_B2);
	io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI1, 0x1, CA_DLL_ARPI1_RG_ARPISM_MCK_SEL_CA);


	for(shu_index = DRAM_DFS_SHUFFLE_1; shu_index < DRAM_DFS_SHUFFLE_MAX;
		shu_index++) {
		io_32_write_fld_align_all(DDRPHY_SHU_B0_DLL0 +
			SHU_GRP_DDRPHY_OFFSET * shu_index, 0x1,
			SHU_B0_DLL0_RG_ARPISM_MCK_SEL_B0_SHU);
		io_32_write_fld_align_all(DDRPHY_SHU_B1_DLL0 +
			SHU_GRP_DDRPHY_OFFSET * shu_index, 0x1,
			SHU_B1_DLL0_RG_ARPISM_MCK_SEL_B1_SHU);
		io_32_write_fld_align_all(DDRPHY_SHU_B2_DLL0 +
			SHU_GRP_DDRPHY_OFFSET * shu_index, 0x1,
			SHU_B2_DLL0_RG_ARPISM_MCK_SEL_B2_SHU);
		io_32_write_fld_align_all(DDRPHY_SHU_CA_DLL0 +
			SHU_GRP_DDRPHY_OFFSET * shu_index, 0x1,
			SHU_CA_DLL0_RG_ARPISM_MCK_SEL_CA_SHU);
	}

	if(en) {
		io_32_write_fld_multi_all(DRAMC_REG_DRAMC_PD_CTRL,
			p_fld(0x1, DRAMC_PD_CTRL_DCMENNOTRFC) |
			p_fld(0x1, DRAMC_PD_CTRL_COMBCLKCTRL) |
			p_fld(0x1, DRAMC_PD_CTRL_PHYCLKDYNGEN) |
			p_fld(0x1, DRAMC_PD_CTRL_DCMEN) |
			p_fld(0x0, DRAMC_PD_CTRL_COMBPHY_CLKENSAME) |
			p_fld(0x1, DRAMC_PD_CTRL_DCMEN2));
		if (p->dram_type == TYPE_LPDDR3) {
			io_32_write_fld_align_all(DRAMC_REG_DRAMC_PD_CTRL,
				CLEAR_FLD, DRAMC_PD_CTRL_DCMENNOTRFC);
		}
		io32_write_4b_all(DDRPHY_MISC_CG_CTRL2, 0x806003BE);
		io32_write_4b_all(DDRPHY_MISC_CG_CTRL2, 0x806003BF);
		io32_write_4b_all(DDRPHY_MISC_CG_CTRL2, 0x806003BE);

		io_32_write_fld_multi_all(DDRPHY_MISC_CTRL3,
			p_fld(0x0, MISC_CTRL3_R_DDRPHY_COMB_CG_IG) |
			p_fld(0x0, MISC_CTRL3_R_DDRPHY_RX_PIPE_CG_IG));
		io_32_write_fld_multi_all(DDRPHY_MISC_CTRL4,
			p_fld(0x1, MISC_CTRL4_R_OPT2_CG_CS) |
			p_fld(0x1, MISC_CTRL4_R_OPT2_CG_CLK) |
			p_fld(0x1, MISC_CTRL4_R_OPT2_CG_CMD));
		for(shu_index = DRAM_DFS_SHUFFLE_1; shu_index < DRAM_DFS_SHUFFLE_MAX;
			shu_index++) {
			io_32_write_fld_multi_all(DDRPHY_SHU_B0_DQ7 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x1, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0) |
				p_fld(0x1, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0) |
				p_fld(0x1, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0));
			io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ7 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x1, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1) |
				p_fld(0x1, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1) |
				p_fld(0x1, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1));
			io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ7 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x1, SHU_B2_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B2) |
				p_fld(0x1, SHU_B2_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B2) |
				p_fld(0x1, SHU_B2_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B2));
			io_32_write_fld_multi_all(DDRPHY_SHU_CA_CMD7 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x1, SHU_CA_CMD7_R_DMTX_ARPI_CG_CS_NEW) |
				p_fld(0x1, SHU_CA_CMD7_R_DMTX_ARPI_CG_CLK_NEW) |
				p_fld(0x1, SHU_CA_CMD7_R_DMTX_ARPI_CG_CMD_NEW));
}
	} else {
		io_32_write_fld_multi_all( DRAMC_REG_DRAMC_PD_CTRL,
			p_fld(0x0, DRAMC_PD_CTRL_DCMENNOTRFC) |
			p_fld(0x0, DRAMC_PD_CTRL_COMBCLKCTRL) |
			p_fld(0x0, DRAMC_PD_CTRL_PHYCLKDYNGEN) |
			p_fld(0x0, DRAMC_PD_CTRL_DCMEN) |
			p_fld(0x1, DRAMC_PD_CTRL_COMBPHY_CLKENSAME) |
			p_fld(0x0, DRAMC_PD_CTRL_DCMEN2));

		io32_write_4b_all(DDRPHY_MISC_CG_CTRL2, 0x8060037E);
		io32_write_4b_all(DDRPHY_MISC_CG_CTRL2, 0x8060037F);
		io32_write_4b_all(DDRPHY_MISC_CG_CTRL2, 0x8060037E);

		io_32_write_fld_multi_all(DDRPHY_MISC_CTRL3,
			p_fld(0x1, MISC_CTRL3_R_DDRPHY_COMB_CG_IG) |
			p_fld(0x1, MISC_CTRL3_R_DDRPHY_RX_PIPE_CG_IG));
		io_32_write_fld_multi_all(DDRPHY_MISC_CTRL4,
			p_fld(0x0, MISC_CTRL4_R_OPT2_CG_CS) |
			p_fld(0x0, MISC_CTRL4_R_OPT2_CG_CLK) |
			p_fld(0x0, MISC_CTRL4_R_OPT2_CG_CMD));
		for(shu_index = DRAM_DFS_SHUFFLE_1; shu_index < DRAM_DFS_SHUFFLE_MAX;
			shu_index++) {
			io_32_write_fld_multi_all(DDRPHY_SHU_B0_DQ7 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B0) |
				p_fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B0) |
				p_fld(0x0, SHU_B0_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B0));
			io_32_write_fld_multi_all(DDRPHY_SHU_B1_DQ7 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B1) |
				p_fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B1) |
				p_fld(0x0, SHU_B1_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B1));
			io_32_write_fld_multi_all(DDRPHY_SHU_B2_DQ7 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_B2_DQ7_R_DMTX_ARPI_CG_DQM_NEW_B2) |
				p_fld(0x0, SHU_B2_DQ7_R_DMTX_ARPI_CG_DQS_NEW_B2) |
				p_fld(0x0, SHU_B2_DQ7_R_DMTX_ARPI_CG_DQ_NEW_B2));
			io_32_write_fld_multi_all(DDRPHY_SHU_CA_CMD7 +
				SHU_GRP_DDRPHY_OFFSET * shu_index,
				p_fld(0x0, SHU_CA_CMD7_R_DMTX_ARPI_CG_CS_NEW) |
				p_fld(0x0, SHU_CA_CMD7_R_DMTX_ARPI_CG_CLK_NEW) |
				p_fld(0x0, SHU_CA_CMD7_R_DMTX_ARPI_CG_CMD_NEW));
		}
	}

	if(is_lp4_family(p)) {
	#if SUPPORT_TYPE_LPDDR4
		lp4_enable_dramc_phydcm_2ch(p, en);
	#endif
	} else {
	#if SUPPORT_TYPE_LPDDR3
		lp3_enable_dramc_phydcm(p, en);
	#endif
	}
	dramc_broadcast_on_off(broadcast_bkp);
#endif
}

void dummy_read_for_dqs_gating_retry(DRAMC_CTX_T *p, unsigned char en)
{
	unsigned int shu_index;
	unsigned int target_addr;
	if (en == 1)
	{
		for (shu_index = DRAM_DFS_SHUFFLE_1; shu_index <
			DRAM_DFS_SHUFFLE_MAX; shu_index++) {
			target_addr = DRAMC_REG_SHU_DQSG_RETRY +
				SHU_GRP_DRAMC_OFFSET * shu_index;

			io_32_write_fld_multi_all(target_addr,
				p_fld(0, SHU_DQSG_RETRY_R_RETRY_ROUND_NUM) |
				p_fld(1, SHU_DQSG_RETRY_R_XSR_RETRY_SPM_MODE) |
				p_fld(0, SHU_DQSG_RETRY_R_XSR_DQSG_RETRY_EN) |
				p_fld(0, SHU_DQSG_RETRY_R_RETRY_USE_BURST_MDOE) |
				p_fld(0, SHU_DQSG_RETRY_R_RDY_SEL_DLE) |
				p_fld(0, SHU_DQSG_RETRY_R_DDR1866_PLUS) |
				p_fld(0, SHU_DQSG_RETRY_R_DQSG_RETRY_SW_EN));
		}
		io_32_write_fld_align_all(DRAMC_REG_TEST2_4, 4,
			TEST2_4_TESTAGENTRKSEL);
		io_32_write_fld_multi_all(DRAMC_REG_DUMMY_RD,
			p_fld(1, DUMMY_RD_DQSG_DMYRD_EN) |
			p_fld(p->support_rank_num, DUMMY_RD_RANK_NUM) |
			p_fld(1, DUMMY_RD_DUMMY_RD_SW));
		io_32_write_fld_align_all(DRAMC_REG_RK1_DUMMY_RD_ADR, 0,
			RK1_DUMMY_RD_ADR_DMY_RD_RK1_LEN);
		io_32_write_fld_align_all(DRAMC_REG_RK0_DUMMY_RD_ADR, 0,
			RK0_DUMMY_RD_ADR_DMY_RD_RK0_LEN);
	} else {
		for (shu_index = DRAM_DFS_SHUFFLE_1; shu_index <
			DRAM_DFS_SHUFFLE_MAX; shu_index++) {
			target_addr = DRAMC_REG_SHU_DQSG_RETRY +
				SHU_GRP_DRAMC_OFFSET * shu_index;
			io_32_write_fld_multi_all(target_addr,
			p_fld(0, SHU_DQSG_RETRY_R_XSR_RETRY_SPM_MODE) |
			p_fld(0, SHU_DQSG_RETRY_R_XSR_DQSG_RETRY_EN) |
			p_fld(0, SHU_DQSG_RETRY_R_DQSG_RETRY_SW_EN));
	}
}

}

void hw_save_for_sr(DRAMC_CTX_T *p)
{
	io_32_write_fld_multi_all(DRAMC_REG_RSTMASK,
		p_fld(CLEAR_FLD, RSTMASK_GT_SYNC_MASK) |
		p_fld(CLEAR_FLD, RSTMASK_GT_SYNC_MASK_FOR_PHY));
	io_32_write_fld_align_all(DRAMC_REG_REFCTRL1, SET_FLD,
		REFCTRL1_SLEFREF_AUTOSAVE_EN);
	io_32_write_fld_multi_all(DRAMC_REG_SREFCTRL,
		p_fld(SET_FLD, SREFCTRL_SREF2_OPTION) |
		p_fld(CLEAR_FLD, SREFCTRL_SREF3_OPTION));
}

/*
 * cke_rank_ctrl
 *  Control CKE toggle mode (toggle both ranks 1. at the same time
 * (CKE_RANK_DEPENDENT) 2. individually (CKE_RANK_INDEPENDENT))
 *  Note: Sets CKE toggle mode for all channels
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @param cke_ctrl_mode      Indicates
 */

void cke_rank_ctrl(DRAMC_CTX_T *p, CKE_CTRL_MODE_T cke_ctrl_mode)
{
	unsigned int shu_index;

	/* CKE_Rank dependent/independent mode register setting values */
	CKE_CTRL_T CKE_Mode;
	CKE_CTRL_T CKE_Rank_Independent = { .cke_2_rank = 0,
		.cke_2_rank_opt = 0, .cke_2_rank_opt2 = 1,
		.cke_2_rank_opt3 = 0, .cke_2_rank_opt5 = 0,
		.cke_2_rank_opt6 = 0, .cke_2_rank_opt7 = 1,
		.cke_2_rank_opt8 = 0, .cke_timer_sel = 0,
		.fast_wake = 1, .fast_wake_2 = 1, .fast_wake_sel = 1,
		.cke_wake_sel = 0, .clk_witrfc = 0};
	CKE_CTRL_T CKE_Rank_Dependent = { .cke_2_rank = 1,
		.cke_2_rank_opt = 0, .cke_2_rank_opt2 = 1,
		.cke_2_rank_opt3 = 0, .cke_2_rank_opt5 = 0,
		.cke_2_rank_opt6 = 0, .cke_2_rank_opt7 = 0,
		.cke_2_rank_opt8 = 0, .cke_timer_sel = 1,
		.fast_wake = 1, .fast_wake_2 = 0, .fast_wake_sel = 0,
		.cke_wake_sel = 0, .clk_witrfc = 0};

	/* Select CKE control mode */
	CKE_Mode = (cke_ctrl_mode == CKE_RANK_INDEPENDENT) ?
		CKE_Rank_Independent : CKE_Rank_Dependent;

	/* Apply CKE control mode register settings */
	io_32_write_fld_multi_all(DRAMC_REG_RKCFG,
		p_fld(CKE_Mode.cke_2_rank, RKCFG_CKE2RANK) |
		p_fld(CKE_Mode.cke_2_rank_opt, RKCFG_CKE2RANK_OPT) |
		p_fld(CKE_Mode.cke_2_rank_opt2, RKCFG_CKE2RANK_OPT2));

	for (shu_index = DRAM_DFS_SHUFFLE_1; shu_index < DRAM_DFS_SHUFFLE_MAX;
		shu_index++) {
		io_32_write_fld_multi_all(DRAMC_REG_SHU_CONF2 +
			SHU_GRP_DRAMC_OFFSET * shu_index,
			p_fld(CKE_Mode.fast_wake, SHU_CONF2_FASTWAKE) |
			p_fld(CKE_Mode.fast_wake_2, SHU_CONF2_FASTWAKE2));
	}

	io_32_write_fld_align_all(DRAMC_REG_DRAMCTRL, CKE_Mode.clk_witrfc,
		DRAMCTRL_CLKWITRFC);
}

void set_cke_2_rank_independent(DRAMC_CTX_T *p)
{
	/* Newly added CKE control mode API */
	cke_rank_ctrl(p, CKE_RANK_INDEPENDENT);
}

	/*
	* If dramc enter SREF and power down,
	* all configure need to sync 2T again after exit SREF.
	* If Psel is 1, clock will be free run at the periof
	* of 2T to let conf be applied.
	* If Psel is 0, Clock will be gated
	*/
void clk_free_run_for_dramc_psel(DRAMC_CTX_T *p)
{
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_CLKAR), CLEAR_FLD,
		CLKAR_PSELAR);
}

#if PA_IMPROVEMENT_FOR_DRAMC_ACTIVE_POWER
void dramc_pa_improve(DRAMC_CTX_T *p)
{
	/* TODO: change shu_cnt to actual shuffle num define */
	unsigned char shu_idx = 0, shu_cnt = 3;
	/*
	 * For SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG,
	 * SHU_ODTCTRL_RODTEN_SELPH_CG_IG shuffle regs
	 */
	unsigned int target_addr = 0;

	io_32_write_fld_multi_all(DRAMC_REG_CLKAR,
		p_fld(CLEAR_FLD, CLKAR_REQQUE_PACG_DIS) |
		p_fld(CLEAR_FLD, CLKAR_SELPH_CMD_CG_DIS));
	/*
	 * Dummy_RD_PA_OPT should be set to 1,
	 * or else some functions would fail (YH Tsai)
	 * Already set to 1 in in UpdateInitialSettings(),
	 * so comment out set to 0 here
	 */
	io_32_write_fld_multi_all(DRAMC_REG_SREFCTRL,
		p_fld(CLEAR_FLD, SREFCTRL_SCSM_CGAR) |
		p_fld(CLEAR_FLD, SREFCTRL_SCARB_SM_CGAR) |
		p_fld(CLEAR_FLD, SREFCTRL_RDDQSOSC_CGAR) |
		p_fld(CLEAR_FLD, SREFCTRL_HMRRSEL_CGAR));
	io_32_write_fld_align_all(DRAMC_REG_ZQCS, CLEAR_FLD,
		ZQCS_ZQCS_MASK_SEL_CGAR);
	io_32_write_fld_align_all(DRAMC_REG_PRE_TDQSCK1, CLEAR_FLD,
		PRE_TDQSCK1_TXUIPI_CAL_CGAR);

	/* AE1/AE2/WE2/M17 */
	io_32_write_fld_align_all(DRAMC_REG_ZQCS, CLEAR_FLD,
		ZQCS_ZQCS_MASK_SEL_CGAR);
	/* AE1/AE2/WE2/M17 */
	io_32_write_fld_align_all(DRAMC_REG_PRE_TDQSCK1, CLEAR_FLD,
		PRE_TDQSCK1_TXUIPI_CAL_CGAR);

	/*
	 * Below loop sets SHU*_ODTCTRL_RODTENSTB_SELPH_CG_IG,
	 * SHU*_ODTCTRL_RODTEN_SELPH_CG_IG (wei-jen)
	 */
	for (shu_idx = DRAM_DFS_SHUFFLE_1; shu_idx < shu_cnt; shu_idx++) {
		target_addr = DRAMC_REG_SHU_ODTCTRL +
			SHU_GRP_DRAMC_OFFSET * shu_idx;
		io_32_write_fld_multi_all(target_addr,
			p_fld(CLEAR_FLD, SHU_ODTCTRL_RODTENSTB_SELPH_CG_IG) |
			p_fld(CLEAR_FLD, SHU_ODTCTRL_RODTEN_SELPH_CG_IG));
	}
}
#endif

void dramc_run_time_config(DRAMC_CTX_T *p)
{
	show_log("[dramc_run_time_config]\n");


#if (RX_PICG_NEW_MODE || TX_PICG_NEW_MODE)
	picg_new_mode_enable(p);
#endif

#if (FOR_DV_SIMULATION_USED == 0)
	transfer_pll_to_spm_control(p);

#if ENABLE_TX_TRACKING
	if (is_lp4_family(p)) {
		unsigned char backup_channel = p->channel;
		unsigned char channelIdx;

		for (channelIdx = CHANNEL_A;
			(unsigned char)channelIdx <
			(unsigned char)(p->support_channel_num);
			channelIdx++) {
			set_phy_2_channel_mapping(p, channelIdx);
			dramc_hw_dqsosc(p);
		}

		set_phy_2_channel_mapping(p, backup_channel);
		show_log("TX_TRACKING: ON\n");
	} else
		show_log("TX_TRACKING: OFF\n");
#else
	show_log("TX_TRACKING: OFF\n");
#endif

#if ENABLE_RX_TRACKING_LP4
	if ((p->dram_type == TYPE_PCDDR3) || (p->dram_type == TYPE_LPDDR3)) {
		show_log("RX_TRACKING: OFF\n");
	} else {
		dramc_rx_input_delay_tracking_init_common(p);
		dramc_rx_input_delay_tracking_hw(p);
		show_log("RX_TRACKING: ON\n");
	}
#else
	show_log("RX_TRACKING: OFF\n");
#endif

/* HW gating - Disabled by default(in preloader) to save power  */
#ifdef HW_GATING
	if (((p->dram_type == TYPE_PCDDR4) && (p->frequency < DDR2666_FREQ)) ||
		(p->dram_type == TYPE_PCDDR3))
		show_log("HW_GATING: OFF\n");/* PCDDR4 HW gating only enable above 2667. */
	else {
		dramc_hw_gating_init(p); /* HW gating initial before RunTime config. */
		dramc_hw_gating_on_off(p, 1); /* Enable HW gating tracking */
		show_log("HW_GATING: ON\n");
	}
#else
	show_log("HW_GATING: OFF\n");
#endif



	show_log("HW_GATING DBG: OFF\n");
	dramc_hw_gating_debug_on_off(p, DISABLE);

#ifdef DUMMY_READ_FOR_TRACKING
	if (p->frequency >= DDR3200_FREQ) {
		dramc_dummy_read_for_tracking_enable(p);
		show_log("DUMMY_READ_FOR_TRACKING: ON\n");
	} else {
		show_log("DUMMY_READ_FOR_TRACKING: OFF\n");
	}
#else
	show_log("DUMMY_READ_FOR_TRACKING: OFF\n");
#endif

	if (is_lp4_family(p)) {
#ifdef ZQCS_ENABLE_LP4
		/* ZQCSMASK setting: (Ch A, Ch B) = (1, 0) or (0, 1) */
		/*
		 * if CHA.ZQCSMASK=1, and then set CHA.ZQCALDISB=1 first,
		 * else set CHB.ZQCALDISB=1 first
		 */
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL),
			SET_FLD, SPCMDCTRL_ZQCALDISB);
		show_log("ZQCS_ENABLE_LP4: ON\n");
#else
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL),
			CLEAR_FLD, SPCMDCTRL_ZQCALDISB);
		show_log("ZQCS_ENABLE_LP4: OFF\n");
#endif
	} else {
#ifdef ZQCS_ENABLE_LP3
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL),
			SET_FLD, SPCMDCTRL_ZQCSDISB);
		show_log("ZQCS_ENABLE_LP3: ON\n");
#else
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL),
			CLEAR_FLD, SPCMDCTRL_ZQCSDISB);
		show_log("ZQCS_ENABLE_LP3: OFF\n");
#endif
	}

#if APPLY_LOWPOWER_GOLDEN_SETTINGS
	enable_dramc_phy_dcm(p, 1);
	show_log("LOWPOWER_GOLDEN_SETTINGS(DCM): ON\n");
#else
	enable_dramc_phy_dcm(p, 0);
	show_log("LOWPOWER_GOLDEN_SETTINGS(DCM): OFF\n");
#endif

	if (is_lp4_family(p)) {
		dummy_read_for_dqs_gating_retry(p, 0);
		show_log("DUMMY_READ_FOR_DQS_GATING_RETRY: OFF\n");
	}

#ifdef SPM_CONTROL_AFTERK
	transfer_to_spm_control(p);
#endif

#ifdef IMPEDANCE_TRACKING_ENABLE
	if (is_lp4_family(p)) {
		dramc_impedance_tracking_enable(p);
		show_log("IMPEDANCE_TRACKING: ON\n");
	}
#ifdef IMPEDANCE_HW_SAVING
	dramc_impedance_hw_saving(p);
#endif
#else
	show_log("IMPEDANCE_TRACKING: OFF\n");
#endif

#ifdef TEMP_SENSOR_ENABLE
	if (is_lp4_family(p)) {
		if(p->support_rank_num == RANK_SINGLE)
			io_32_write_fld_align_all(DRAMC_REG_SPCMDCTRL,
				CLEAR_FLD, SPCMDCTRL_HMR4_TOG_OPT);

	/* Enable HW-MRR4:SPCMDCTRL_REFRDIS=0 */
	io_32_write_fld_align_all(DRAMC_REG_SPCMDCTRL, CLEAR_FLD,
		SPCMDCTRL_REFRDIS);

	/* HW-MRR4 don't block normal EMI request: SPCMDCTRL_REFR_BLOCKEN=0 */
	io_32_write_fld_align_all(DRAMC_REG_SPCMDCTRL, CLEAR_FLD,
		SPCMDCTRL_REFR_BLOCKEN);

	/* DRAMC will consider tMRR ac-timing: HW_MRR_FUN_TMRR_ENA =1 */
	io_32_write_fld_align_all(DRAMC_REG_HW_MRR_FUN, SET_FLD,
		HW_MRR_FUN_TMRR_ENA);

	/* Set HW-MRR4 command in queue to high priority: MRR_HW_HIPRI = 1 */
	io_32_write_fld_align_all(DRAMC_REG_HW_MRR_FUN, SET_FLD,
		HW_MRR_FUN_MRR_HW_HIPRI);
	} else if (p->dram_type == TYPE_LPDDR3) {
		io_32_write_fld_align_all(DRAMC_REG_SPCMDCTRL, CLEAR_FLD,
			SPCMDCTRL_REFRDIS);
	}
	show_log("TEMP_SENSOR: ON\n");
#else
	io_32_write_fld_align_all(DRAMC_REG_SPCMDCTRL, SET_FLD,
		SPCMDCTRL_REFRDIS);
	show_log("TEMP_SENSOR: OFF\n");
#endif

#if 1
	if (p->dram_type != TYPE_PCDDR4 && p->dram_type != TYPE_PCDDR3) {
		io_32_write_fld_align_all(DRAMC_REG_REFCTRL0, SET_FLD,
			REFCTRL0_PBREFEN);
		show_log("PER_BANK_REFRESH: ON\n");
	}

	hw_save_for_sr(p);
	show_log("HW_SAVE_FOR_SR: ON\n");

	set_cke_2_rank_independent(p);
	show_log("SET_CKE_2_RANK_INDEPENDENT_RUN_TIME: ON\n");

	clk_free_run_for_dramc_psel(p);
	show_log("CLK_FREE_FUN_FOR_DRAMC_PSEL: ON\n");
#endif

#if PA_IMPROVEMENT_FOR_DRAMC_ACTIVE_POWER
	dramc_pa_improve(p);
	show_log("PA_IMPROVEMENT_FOR_DRAMC_ACTIVE_POWER: ON\n");
#else
	show_log("PA_IMPROVEMENT_FOR_DRAMC_ACTIVE_POWER: OFF\n");
#endif

#if ENABLE_RODT_TRACKING
	show_log("Read ODT Tracking: ON\n");
#endif

#if DUAL_FREQ_K
	show_log("DQS Precalculation for DVFS: ");
	if (is_lp4_family(p)) {
		show_log("ON\n");
		dramc_dqs_precalculation_preset(p);
		show_log("=========================\n");
	}
#endif

#if ENABLE_DRS
	if (p->support_rank_num == RANK_DUAL)
		dramc_drs(p, 1);
	else
		dramc_drs(p, 0);
#endif

	/* Set VRCG{MR13[3]} to 0 both to DRAM and DVFS */
#if SUPPORT_TYPE_LPDDR4
	if (is_lp4_family(p))
		set_mr13_vrcg_to_normal_operation(p);
#endif

#endif
}

#if (DUAL_FREQ_K == 1)
void dramc_save_to_shuffle_reg(DRAMC_CTX_T *p, DRAM_DFS_SHUFFLE_TYPE_T src_rg,
		DRAM_DFS_SHUFFLE_TYPE_T dst_rg)
{
	unsigned int i;
	unsigned int idx;
	unsigned int src_addr;
	unsigned int shuffle_addr;
	unsigned int reg_value;
	unsigned int normal_addr;

	show_msg((INFO, "Save freq reg settings into shuffle regs\n"));

	/* DRAMC */
	for (i = 0; i < CHANNEL_NUM; i++) {
		for (idx = 0; idx < SHUFFLE_ADDR_NUM_DRAMC; idx++) {
			normal_addr = shuffle_reg_table_dramc[idx].start_addr +
				(i << POS_BANK_NUM);
			while (normal_addr <=
				shuffle_reg_table_dramc[idx].end_addr +
				(i << POS_BANK_NUM)) {
				src_addr = normal_addr +
					SHU_GRP_DRAMC_OFFSET * src_rg;
				shuffle_addr = normal_addr +
					SHU_GRP_DRAMC_OFFSET * dst_rg;
				reg_value = io32_read_4b(src_addr);
				io32_write_4b(shuffle_addr, reg_value);
				normal_addr += 4;
			}
		}
	}

#if 0
	/* DRAMC-exception-1 */
	for (i = 0; i < CHANNEL_NUM; i++) {
		src_addr = DRAMC_REG_SHUCTRL2 +
			((unsigned int)i << POS_BANK_NUM);
		shuffle_addr = DRAMC_REG_DVFSDLL +
			((unsigned int)i << POS_BANK_NUM);
		reg_value = io_32_read_fld_align(src_addr, SHUCTRL2_R_DLL_IDLE);

		switch (dst_rg) {
		case DRAM_DFS_SHUFFLE_2:
			io_32_write_fld_align(shuffle_addr, reg_value,
				DVFSDLL_DLL_IDLE_SHU2);
			break;
		case DRAM_DFS_SHUFFLE_3:
			io_32_write_fld_align(shuffle_addr, reg_value,
				DVFSDLL_DLL_IDLE_SHU3);
			break;
				default:
			io_32_write_fld_align(src_addr, reg_value,
				SHUCTRL2_R_DLL_IDLE);
		}
	}
#endif

#if 0/* cc mark for reg not defined */
	/* DRAMC-exception-2 */
	for (i = 0; i < CHANNEL_NUM; i++) {
		src_addr = DRAMC_REG_DVFSDLL + (i << POS_BANK_NUM);
		reg_value =
			io_32_read_fld_align(src_addr,
				DVFSDLL_R_BYPASS_1ST_DLL_SHU1);
		switch (dst_rg) {
		case DRAM_DFS_SHUFFLE_2:
			io_32_write_fld_align(src_addr, reg_value,
				DVFSDLL_R_BYPASS_1ST_DLL_SHU2);
			break;
		case DRAM_DFS_SHUFFLE_3:
			io_32_write_fld_align(src_addr, reg_value,
				DVFSDLL_R_BYPASS_1ST_DLL_SHU3);
			break;
		case DRAM_DFS_SHUFFLE_4:
			io_32_write_fld_align(src_addr, reg_value,
				DVFSDLL_R_BYPASS_1ST_DLL_SHU4);
			break;
		default:
			io_32_write_fld_align(src_addr, reg_value,
				DVFSDLL_R_BYPASS_1ST_DLL_SHU1);
		}
	}
#endif

	/* PHY */
	for (i = 0; i < CHANNEL_NUM; i++) {
		for (idx = 0; idx < SHUFFLE_ADDR_NUM_DDRPHY; idx++) {
			normal_addr =
				shuffle_reg_table_ddrphy[idx].start_addr +
				((unsigned int)i << POS_BANK_NUM);
			while (normal_addr <=
				shuffle_reg_table_ddrphy[idx].end_addr +
				((unsigned int)i << POS_BANK_NUM)) {
				src_addr = normal_addr +
					SHU_GRP_DDRPHY_OFFSET * src_rg;
				shuffle_addr = normal_addr +
					SHU_GRP_DDRPHY_OFFSET * dst_rg;
				reg_value = io32_read_4b(src_addr);
				io32_write_4b(shuffle_addr, reg_value);
				normal_addr += 4;
			}
		}
	}

	/* PHY-exception */
	set_shuffle_frequency(p, dst_rg, p->shuffle_frequency[src_rg]);
}

/* cc porting from M */
void cbt_dramc_dfs_direct_jump(DRAMC_CTX_T *p, unsigned char shu_level)
{
	unsigned char shu_ack = 0;
	unsigned char i = 0;
	/* enable another PLL */
	if (phy_pll_en[p->channel]) {
		show_msg2((INFO, "Disable CLRPLL\n"));
		io_32_write_fld_align(DDRPHY_PLL2, CLEAR_FLD, PLL2_RG_RCLRPLL_EN);
	} else {
		show_msg2((INFO, "Disable PHYPLL\n"));
		io_32_write_fld_align(DDRPHY_PLL1, CLEAR_FLD, PLL1_RG_RPHYPLL_EN);
		}

	for (i = 0; i < p->support_channel_num; i++) {
		shu_ack |= (0x1 << i);
	}

	if (phy_pll_en[p->channel]) {
		show_msg3((INFO,
			"DFSDirectJump to CLRPLL, SHU_LEVEL=%d, ACK=%x\n",
			shu_level, shu_ack));
	} else {
		show_msg3((INFO,
			"DFSDirectJump to PHYPLL, SHU_LEVEL=%d, ACK=%x\n",
			shu_level, shu_ack));
	}
#ifdef SPM_LIB_USE
	io_32_write_fld_align(SPM_POWER_ON_VAL0, CLEAR_FLD,	SPM_POWER_ON_VAL0_SC_PHYPLL_SHU_EN_PCM);
	io_32_write_fld_align(SPM_POWER_ON_VAL0, CLEAR_FLD,	SPM_POWER_ON_VAL0_SC_PHYPLL2_SHU_EN_PCM);
	io_32_write_fld_align(SPM_POWER_ON_VAL0, CLEAR_FLD,	SPM_POWER_ON_VAL0_SC_DR_SHU_LEVEL_PCM);
	io_32_write_fld_align(SPM_POWER_ON_VAL0, shu_level,	SPM_POWER_ON_VAL0_SC_DR_SHU_LEVEL_PCM);

	if (phy_pll_en[p->channel]) {
		io_32_write_fld_align(SPM_POWER_ON_VAL0, SET_FLD, SPM_POWER_ON_VAL0_SC_PHYPLL2_SHU_EN_PCM);
		delay_us(1);
		io_32_write_fld_align(DDRPHY_PLL2, SET_FLD, PLL2_RG_RCLRPLL_EN);
		show_msg2((INFO, "Enable CLRPLL\n"));
	} else {
		io_32_write_fld_align(SPM_POWER_ON_VAL0, SET_FLD, SPM_POWER_ON_VAL0_SC_PHYPLL_SHU_EN_PCM);
		delay_us(1);
		io_32_write_fld_align(DDRPHY_PLL1, SET_FLD, PLL1_RG_RPHYPLL_EN);
		show_msg2((INFO, "Enable PHYPLL\n"));
	}

	/*TINFO="DRAM : set tx tracking disable  = 1"*/
	io_32_write_fld_align(SPM_POWER_ON_VAL0, SET_FLD, SPM_POWER_ON_VAL0_SC_TX_TRACKING_DIS);
	delay_us(20);

	/*TINFO="DRAM : set ddrphy_fb_ck_en=1"*/
	io_32_write_fld_align(SPM_POWER_ON_VAL0, SET_FLD, SPM_POWER_ON_VAL0_SC_DDRPHY_FB_CK_EN_PCM);

	show_msg2((INFO, "Disable RX-Tracking\n"));
	io_32_write_fld_align(SPM_POWER_ON_VAL0, CLEAR_FLD,	SPM_POWER_ON_VAL0_SC_DPHY_RXDLY_TRACK_EN);

	show_msg3((INFO, "SHUFFLE Start\n"));
	io_32_write_fld_align(SPM_POWER_ON_VAL0, SET_FLD, SPM_POWER_ON_VAL0_SC_DR_SHU_EN_PCM);
	while ((io_32_read_fld_align(SPM_DRAMC_DPY_CLK_SW_CON, DRAMC_DPY_CLK_SW_CON_SC_DMDRAMCSHU_ACK) & shu_ack) != shu_ack)
		show_msg2((INFO, "\twait shu_en ack = %x.\n", shu_ack));

	io_32_write_fld_align(SPM_POWER_ON_VAL0, CLEAR_FLD,	SPM_POWER_ON_VAL0_SC_DR_SHU_EN_PCM);
	show_msg3((INFO, "SHUFFLE End\n"));

	/*! LP4 shu 0:3200, shu1:1600 (no rx), shu2:3733, shu3:4266 */
	if ((shu_level == 0) && is_lp4_family(p)) {
		show_msg((INFO, "Enable LP4 rx tracking for SHU-0\n"));
		//io_32_write_fld_align(SPM_POWER_ON_VAL0, SET_FLD, SPM_POWER_ON_VAL0_SC_DPHY_RXDLY_TRACK_EN);
	}

#ifdef ENABLE_TX_TRACKING
	/*TINFO="DRAM : clear tx tracking disable  = 0"*/
	io_32_write_fld_align(SPM_POWER_ON_VAL0, CLEAR_FLD,	SPM_POWER_ON_VAL0_SC_TX_TRACKING_DIS);
#endif

	/*TINFO="DRAM : set ddrphy_fb_ck_en=0"*/
	io_32_write_fld_align(SPM_POWER_ON_VAL0, CLEAR_FLD,	SPM_POWER_ON_VAL0_SC_DDRPHY_FB_CK_EN_PCM);
#endif

	if (phy_pll_en[p->channel]) {
		io_32_write_fld_align(DDRPHY_PLL1, CLEAR_FLD, PLL1_RG_RPHYPLL_EN);
	} else {
		/*TINFO="DRAM:set sc_phypll_mode_sw=0" */
		io_32_write_fld_align(DDRPHY_PLL2, CLEAR_FLD, PLL2_RG_RCLRPLL_EN);
	}
	show_msg((INFO, "Shuffle flow complete\n"));

	phy_pll_en[p->channel] = !phy_pll_en[p->channel];

	return;
};
#endif

void no_parking_on_clrpll(DRAMC_CTX_T *p)
{
#if DUAL_FREQ_K
	if (phy_pll_en[p->channel])
		return;	/* already parking on PHYPLL */

	cbt_dramc_dfs_direct_jump(p, DRAM_DFS_SHUFFLE_1);
#endif
}

/* "DUAL_FREQ_K" use this API */
void dfs_init_for_calibration(DRAMC_CTX_T *p)
{
#if CALIBRATION_LOG
	print_mode_reg_write = 1;
#endif
	dramc_init(p);
	print_mode_reg_write = 0;
	apply_config_before_calibration(p);
#ifdef ENABLE_MIOCK_JMETER
	if (p->frequency >= PERBIT_THRESHOLD_FREQ)
		pre_miock_jmeter_hqa_used(p);
#endif
}

void dramc_hw_dqs_gating_tracking_dvt(DRAMC_CTX_T *p)
{
#ifdef HW_GATING
	if(is_lp4_family(p))
	{
#if SUPPORT_TYPE_LPDDR4
#if DramcHWDQSGatingTracking_DVT_FIFO_MODE
	//REFUICHG=0, STB_SHIFT_DTCOUT_IG=0, DQSG_MODE=1, NARROW_IG=0
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL),
		p_fld(1, STBCAL_STB_DQIEN_IG) |
		p_fld(1, STBCAL_PICHGBLOCK_NORD) |
		p_fld(0, STBCAL_REFUICHG) |
		p_fld(0, STBCAL_PHYVALID_IG) |
		p_fld(0, STBCAL_STBSTATE_OPT)|
		p_fld(0, STBCAL_STBDLELAST_FILTER) |
		p_fld(0, STBCAL_STBDLELAST_PULSE) |
		p_fld(0, STBCAL_STBDLELAST_OPT)|
		p_fld(1, STBCAL_PIMASK_RKCHG_OPT));

	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL1),
		p_fld(1, STBCAL1_STBCAL_FILTER)|
		p_fld(1, STBCAL1_STB_FLAGCLR) |
		p_fld(1, STBCAL1_STB_SHIFT_DTCOUT_IG));

	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL0),
		p_fld(1, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
		p_fld(0, MISC_CTRL0_R_DMVALID_DLY)|
		p_fld(0, MISC_CTRL0_R_DMVALID_DLY_OPT) |
		p_fld(0, MISC_CTRL0_R_DMVALID_NARROW_IG) |
		p_fld(0, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));

	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ6), 0,
		B0_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ6), 0,
		B2_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B2);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD6), 0,
		CA_CMD6_RG_RX_ARCMD_DMRANK_OUTSEL);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ6), 0,
		B1_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B1);

#else
	//REFUICHG=0, STB_SHIFT_DTCOUT_IG=0, DQSG_MODE=1, NARROW_IG=0
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL),
		p_fld(1, STBCAL_STB_DQIEN_IG) |
		p_fld(1, STBCAL_PICHGBLOCK_NORD) |
		p_fld(0, STBCAL_REFUICHG) |
		p_fld(0, STBCAL_PHYVALID_IG) |
		p_fld(0, STBCAL_STBSTATE_OPT)|
		p_fld(0, STBCAL_STBDLELAST_FILTER) |
		p_fld(0, STBCAL_STBDLELAST_PULSE) |
		p_fld(0, STBCAL_STBDLELAST_OPT)|
		p_fld(1, STBCAL_PIMASK_RKCHG_OPT));

	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL1),
		p_fld(1, STBCAL1_STBCAL_FILTER)|
		p_fld(1, STBCAL1_STB_FLAGCLR) |
		p_fld(0, STBCAL1_STB_SHIFT_DTCOUT_IG));


	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL0),
		p_fld(0, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
		p_fld(3, MISC_CTRL0_R_DMVALID_DLY)|
		p_fld(1, MISC_CTRL0_R_DMVALID_DLY_OPT) |
		p_fld(0, MISC_CTRL0_R_DMVALID_NARROW_IG) |
		p_fld(0xf, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));

	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ6), 1,
		B0_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ6), 1,
		B2_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B2);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD6), 1,
		CA_CMD6_RG_RX_ARCMD_DMRANK_OUTSEL);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ6), 1,
		B1_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B1);

#endif
#endif /* SUPPORT_TYPE_LPDDR4 */
	} else {

#if SUPPORT_TYPE_LPDDR3
		//Set LP3 as FIFO mode to gain AC timing margin (review by HJ Huang)
#if DramcHWDQSGatingTracking_DVT_LP3_FIFO_MODE
		//REFUICHG=0, STB_SHIFT_DTCOUT_IG=0, DQSG_MODE=1, NARROW_IG=0
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL),
			p_fld(1, STBCAL_STB_DQIEN_IG) |
			p_fld(1, STBCAL_PICHGBLOCK_NORD) |
			p_fld(0, STBCAL_REFUICHG) |
			p_fld(0, STBCAL_PHYVALID_IG) |
			p_fld(0, STBCAL_STBSTATE_OPT)|
			p_fld(0, STBCAL_STBDLELAST_FILTER) |
			p_fld(0, STBCAL_STBDLELAST_PULSE) |
			p_fld(0, STBCAL_STBDLELAST_OPT)|
			p_fld(1, STBCAL_PIMASK_RKCHG_OPT));

		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL1),
			p_fld(1, STBCAL1_STBCAL_FILTER)|
			p_fld(1, STBCAL1_STB_FLAGCLR) |
			p_fld(1, STBCAL1_STB_SHIFT_DTCOUT_IG));

		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL0),
			p_fld(1, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
			p_fld(0, MISC_CTRL0_R_DMVALID_DLY)|
			p_fld(0, MISC_CTRL0_R_DMVALID_DLY_OPT) |
			p_fld(0, MISC_CTRL0_R_DMVALID_NARROW_IG) |
			p_fld(0, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));

		io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL1), 1,
			STBCAL1_STBCNT_MODESEL);

		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ6), 0,
			B0_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B0);
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ6), 0,
			B1_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B1);
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ6), 0,
			B2_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B2);
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD6), 0,
			CA_CMD6_RG_RX_ARCMD_DMRANK_OUTSEL);

#elif DramcHWDQSGatingTracking_DVT_JADE_TRACKING_MODE
		//UPDBYWR=0, REFUICHG=1, STB_SHIFT_DTCOUT_IG=0, DQSG_MODE=1, NARROW_IG=1
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0), 0, REFCTRL0_UPDBYWR);
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL),
			p_fld(0, STBCAL_STB_DQIEN_IG) |
			p_fld(0, STBCAL_PICHGBLOCK_NORD) |
			p_fld(1, STBCAL_REFUICHG) |
			p_fld(0, STBCAL_PHYVALID_IG) |
			p_fld(1, STBCAL_STBSTATE_OPT)|
			p_fld(0, STBCAL_STBDLELAST_FILTER) |
			p_fld(0, STBCAL_STBDLELAST_PULSE) |
			p_fld(0, STBCAL_STBDLELAST_OPT)|
			p_fld(0, STBCAL_PIMASK_RKCHG_OPT));

		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL1),
			p_fld(0, STBCAL1_STBCAL_FILTER)|
			p_fld(1, STBCAL1_STB_FLAGCLR) |
			p_fld(0, STBCAL1_STB_SHIFT_DTCOUT_IG));

		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL0),
			p_fld(0, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
			p_fld(2, MISC_CTRL0_R_DMVALID_DLY)|
			p_fld(1, MISC_CTRL0_R_DMVALID_DLY_OPT) |
			p_fld(1, MISC_CTRL0_R_DMVALID_NARROW_IG) |
			p_fld(0xf, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));

		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ6), 1,
			B0_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B0);
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ6), 1,
			B1_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B1);
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ6), 1,
			B2_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B2);
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD6), 1,
			CA_CMD6_RG_RX_ARCMD_DMRANK_OUTSEL);

#else
		//REFUICHG=0, STB_SHIFT_DTCOUT_IG=0, DQSG_MODE=1, NARROW_IG=0
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL),
			p_fld(1, STBCAL_STB_DQIEN_IG) |
			p_fld(1, STBCAL_PICHGBLOCK_NORD) |
			p_fld(0, STBCAL_REFUICHG) |
			p_fld(0, STBCAL_PHYVALID_IG) |
			p_fld(0, STBCAL_STBSTATE_OPT)|
			p_fld(0, STBCAL_STBDLELAST_FILTER) |
			p_fld(0, STBCAL_STBDLELAST_PULSE) |
			p_fld(0, STBCAL_STBDLELAST_OPT)|
			p_fld(1, STBCAL_PIMASK_RKCHG_OPT));

		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL1),
			p_fld(1, STBCAL1_STBCAL_FILTER)|
			p_fld(1, STBCAL1_STB_FLAGCLR) |
			p_fld(0, STBCAL1_STB_SHIFT_DTCOUT_IG));

		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL0),
			p_fld(0, MISC_CTRL0_R_DMDQSIEN_FIFO_EN) |
			p_fld(5, MISC_CTRL0_R_DMVALID_DLY)|
			p_fld(1, MISC_CTRL0_R_DMVALID_DLY_OPT) |
			p_fld(0, MISC_CTRL0_R_DMVALID_NARROW_IG) |
			p_fld(0xf, MISC_CTRL0_R_DMDQSIEN_SYNCOPT));

		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B0_DQ6), 1,
			B0_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B0);
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B1_DQ6), 1,
			B1_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B1);
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_B2_DQ6), 1,
			B2_DQ6_RG_RX_ARDQ_DMRANK_OUTSEL_B2);
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_CA_CMD6), 1,
			CA_CMD6_RG_RX_ARCMD_DMRANK_OUTSEL);

#endif
#endif
}

#endif
}


void dramc_hw_gating_init(DRAMC_CTX_T *p)
{
#ifdef HW_GATING
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL),
		p_fld(CLEAR_FLD, STBCAL_STBCALEN) |
		p_fld(CLEAR_FLD, STBCAL_STBCAL2R) |
		p_fld(CLEAR_FLD, STBCAL_STB_SELPHYCALEN) |
		p_fld(CLEAR_FLD, STBCAL_STBSTATE_OPT) |
		p_fld(CLEAR_FLD, STBCAL_RKCHGMASKDIS) |
		p_fld(CLEAR_FLD, STBCAL_REFUICHG) |
		p_fld(SET_FLD, STBCAL_PICGEN));

	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL),
		p_fld(SET_FLD, STBCAL_DQSIENCG_CHG_EN));
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL), CLEAR_FLD,
		STBCAL_CG_RKEN);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL), SET_FLD,
		STBCAL_DQSIENCG_NORMAL_EN);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_MISC_CTRL1), SET_FLD,
		MISC_CTRL1_R_DMDQSIENCG_EN);

	dramc_hw_dqs_gating_tracking_dvt(p);
#endif
}

void dramc_hw_gating_on_off(DRAMC_CTX_T *p, unsigned char on_off)
{
#ifdef HW_GATING
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_SHUCTRL2),
		p_fld(on_off, SHUCTRL2_R_DVFS_OPTION) |
		p_fld(on_off, SHUCTRL2_R_DVFS_PARK_N));
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL), on_off,
		STBCAL_STBCALEN);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL2), on_off,
		STBCAL2_STB_GERRSTOP);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL), on_off,
		STBCAL_STB_SELPHYCALEN);
#else
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_SHUCTRL2),
		p_fld(0x0, SHUCTRL2_R_DVFS_OPTION) |
		p_fld(0x0, SHUCTRL2_R_DVFS_PARK_N));
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL), 0,
		STBCAL_STBCALEN);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL2), 0,
		STBCAL2_STB_GERRSTOP);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_STBCAL), 0,
		STBCAL_STB_SELPHYCALEN);
#endif
}


void dramc_hw_gating_debug_on_off(DRAMC_CTX_T *p, unsigned char on_off)
{
#ifdef HW_GATING
	io_32_write_fld_multi_all(DRAMC_REG_STBCAL2,
		p_fld(on_off, STBCAL2_STB_DBG_EN_B1) |
		p_fld(on_off, STBCAL2_STB_DBG_EN_B0) |
		p_fld(on_off, STBCAL2_STB_PIDLYCG_IG) |
		p_fld(on_off, STBCAL2_STB_UIDLYCG_IG) |
		p_fld(on_off, STBCAL2_STB_GERRSTOP) |
		p_fld(CLEAR_FLD, STBCAL2_STB_DBG_CG_AO) |
		p_fld(CLEAR_FLD, STBCAL2_STB_DBG_UIPI_UPD_OPT));
#endif
}

#if (CPU_RW_TEST_AFTER_K)
void dramc_print_hw_gating_status(DRAMC_CTX_T *p, unsigned char channel)
{
#ifdef HW_GATING
	unsigned char rank_idx, rank_max;
	unsigned int result_dqs_pi, result_dqs_ui, result_dqs_ui_p1;
	unsigned char dqs_pi[DQS_BIT_NUM], dqs_ui[DQS_BIT_NUM],
		dqs_ui_p1[DQS_BIT_NUM];
	unsigned int backup_value_i[4] = {0, 0, 0, 0};

	backup_value_i[0] = (unsigned int)p->channel;
	set_phy_2_channel_mapping(p, channel);
	backup_value_i[1] = (unsigned int)get_rank(p);

	if (p->support_rank_num == RANK_DUAL)
		rank_max = RANK_MAX;
	else
		rank_max = RANK_1;

	backup_value_i[2] =
		io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3),
		TEST2_3_MANUDLLFRZ);
	backup_value_i[3] =
		io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_STBCAL),
		STBCAL_STBSTATE_OPT);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3), SET_FLD,
		TEST2_3_MANUDLLFRZ);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_STBCAL), SET_FLD,
		STBCAL_STBSTATE_OPT);

	for (rank_idx = 0; rank_idx < rank_max; rank_idx++) {
		set_rank(p, rank_idx);
		show_msg((INFO,	"[DramcHWGatingStatus] Channel=%d, Rank=%d\n",
			p->channel, rank_idx));
		result_dqs_pi =
			io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_RK0_DQSIENDLY));
		dqs_pi[0] = result_dqs_pi & 0xff;
		dqs_pi[1] = (result_dqs_pi >> 8) & 0xff;
		dqs_pi[2] = (result_dqs_pi >> 16) & 0xff;
		dqs_pi[3] = (result_dqs_pi >> 24) & 0xff;

		result_dqs_ui = io32_read_4b
			(DRAMC_REG_ADDR(DRAMC_REG_RK0_DQSIENUIDLY));
		dqs_ui[0] = result_dqs_ui & 0xff;
		dqs_ui[1] = (result_dqs_ui >> 8) & 0xff;
		dqs_ui[2] = (result_dqs_ui >> 16) & 0xff;
		dqs_ui[3] = (result_dqs_ui >> 24) & 0xff;
		show_msg((INFO,
			"%s(%d, %d, %d)\n %s(%d, %d, %d)\n",
			"Byte0(2T, 0.5T, PI) =",
			dqs_ui[0] / 8, dqs_ui[0] % 8, dqs_pi[0],
			"Byte1(2T, 0.5T, PI) =",
			dqs_ui[1] / 8, dqs_ui[1] % 8, dqs_pi[1]));
		show_msg((INFO,
			"%s(%d, %d, %d)\n %s(%d, %d, %d)\n",
			"Byte2(2T, 0.5T, PI) =",
			dqs_ui[2] / 8, dqs_ui[2] % 8, dqs_pi[2],
			 "Byte3(2T, 0.5T, PI) =",
			dqs_ui[3] / 8, dqs_ui[3] % 8, dqs_pi[3]));

		result_dqs_ui_p1 = io32_read_4b
			(DRAMC_REG_ADDR(DRAMC_REG_RK0_DQSIENUIDLY_P1));
		dqs_ui_p1[0] = result_dqs_ui_p1 & 0xff;
		dqs_ui_p1[1] = (result_dqs_ui_p1 >> 8) & 0xff;
		dqs_ui_p1[2] = (result_dqs_ui_p1 >> 16) & 0xff;
		dqs_ui_p1[3] = (result_dqs_ui_p1 >> 24) & 0xff;
		show_msg((INFO,
			"UI_Phase1 (DQS0~3) =(%d, %d, %d, %d)\n\n",
			dqs_ui_p1[0], dqs_ui_p1[1], dqs_ui_p1[2],
			dqs_ui_p1[3]));
	}
	set_rank(p, (unsigned char)backup_value_i[1]);


	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_STBCAL),
		backup_value_i[3], STBCAL_STBSTATE_OPT);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3),
		backup_value_i[2], TEST2_3_MANUDLLFRZ);

	p->channel = (unsigned char)backup_value_i[0];
	set_phy_2_channel_mapping(p, backup_value_i[0]);
#endif
}

void dramc_print_rxdqdqs_status(DRAMC_CTX_T *p, unsigned char channel)
{
	unsigned char rank_idx, rank_max, channel_bak;
	unsigned int result_dqs_pi;
	unsigned int backup_rank;
	unsigned char dqx_b0, dqs_0, dqx_b2, dqs_2;

	channel_bak = p->channel;
	set_phy_2_channel_mapping(p, channel);
	backup_rank = get_rank(p);

	if (p->support_rank_num == RANK_DUAL)
		rank_max = RANK_MAX;
	else
		rank_max = RANK_1;

	for (rank_idx = 0; rank_idx < rank_max; rank_idx++) {
		set_rank(p, rank_idx);
		show_msg((INFO, "[RXDQDQSStatus] CH%d, RK%d\n",
			p->channel, rank_idx));

		if (rank_idx == 0) {
			dqx_b0 = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_DQ_RXDLY_TRRO13), MISC_DQ_RXDLY_TRRO13_DA_RK0_DQX_B0_R_DLY);
			dqs_0 = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_DQ_RXDLY_TRRO13), MISC_DQ_RXDLY_TRRO13_DA_RK0_DQS0_R_DLY);
			dqx_b2 = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_DQ_RXDLY_TRRO15), MISC_DQ_RXDLY_TRRO13_DA_RK0_DQX_B1_R_DLY);
			dqs_2 = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_DQ_RXDLY_TRRO15), MISC_DQ_RXDLY_TRRO13_DA_RK0_DQS1_R_DLY);
		}
		if (rank_idx == 1) {
			dqx_b0 = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_DQ_RXDLY_TRRO14), MISC_DQ_RXDLY_TRRO14_DA_RK1_DQX_B0_R_DLY);
			dqs_0 = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_DQ_RXDLY_TRRO14), MISC_DQ_RXDLY_TRRO14_DA_RK1_DQS0_R_DLY);
			dqx_b2 = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_DQ_RXDLY_TRRO14), MISC_DQ_RXDLY_TRRO14_DA_RK1_DQX_B1_R_DLY);
			dqs_2 = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_DQ_RXDLY_TRRO14), MISC_DQ_RXDLY_TRRO14_DA_RK1_DQS1_R_DLY);
		}

		show_msg((INFO,
			"DQX_B0, DQS0, DQX_B2, DQS2 = (%d, %d, %d, %d)\n\n",
			dqx_b0, dqs_0, dqx_b2, dqs_2));
	}
	set_rank(p, backup_rank);

	p->channel = channel_bak;
	set_phy_2_channel_mapping(p, channel_bak);
}

void dramc_print_imp_tracking_status(DRAMC_CTX_T *p, unsigned char channel)
{
#ifdef IMPEDANCE_TRACKING_ENABLE

	unsigned char channel_bak;
	unsigned char drvp_2, odtn_2, drvp, odtn;

	channel_bak = p->channel;
	set_phy_2_channel_mapping(p, channel);

	show_msg((INFO, "[IMPTrackingStatus] CH=%d\n", p->channel));

	drvp_2 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL_STATUS1),
		IMPCAL_STATUS1_DRVPDQS_SAVE2);
	odtn_2 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL_STATUS1),
		IMPCAL_STATUS1_DRVNDQS_SAVE2);
	drvp = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL_STATUS1),
		IMPCAL_STATUS1_DRVPDQS_SAVE1);
	odtn = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL_STATUS1),
		IMPCAL_STATUS1_DRVNDQS_SAVE1);
	show_msg((INFO, "\tDRVP_2\tODTN_2\tDRVP\tODTN\n"
		"DQS\t%d\t%d\t%d\t%d\n",
		drvp_2, odtn_2, drvp, odtn));

	drvp_2 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL_STATUS1),
		IMPCAL_STATUS1_DRVPDQ_SAVE2);
	odtn_2 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL_STATUS1),
		IMPCAL_STATUS1_DRVNDQ_SAVE2);
	drvp = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL_STATUS2),
		IMPCAL_STATUS2_DRVPDQ_SAVE1);
	odtn = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL_STATUS2),
		IMPCAL_STATUS2_DRVNDQ_SAVE1);
	show_msg((INFO, "DQ\t%d\t%d\t%d\t%d\n",
		drvp_2, odtn_2, drvp, odtn));

	drvp_2 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL_STATUS2),
		IMPCAL_STATUS2_DRVPCMD_SAVE2);
	odtn_2 = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL_STATUS2),
		IMPCAL_STATUS2_DRVNCMD_SAVE2);
	drvp = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL_STATUS2),
		IMPCAL_STATUS2_DRVPCMD_SAVE1);
	odtn = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_IMPCAL_STATUS2),
		IMPCAL_STATUS2_DRVNCMD_SAVE1);
	show_msg((INFO, "CMD\t%d\t%d\t%d\t%d\n",
		drvp_2, odtn_2, drvp, odtn));

	p->channel = channel_bak;
	set_phy_2_channel_mapping(p, channel_bak);
#endif
}
#endif

void dramc_drs(DRAMC_CTX_T *p, unsigned char enable)
{
#if 1 /* cc notes: what for?? */
	/* R_DMDRS_CNTX[6:0](DVT set 0, HQA set 4 or 5) */
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_DRSCTRL),
		p_fld(CLEAR_FLD, DRSCTRL_DRSPB2AB_OPT) |
		p_fld(CLEAR_FLD, DRSCTRL_DRSMON_CLR) |
		p_fld(8, DRSCTRL_DRSDLY) |
		p_fld(CLEAR_FLD, DRSCTRL_DRSACKWAITREF) |
		p_fld(!enable, DRSCTRL_DRSDIS) |
		p_fld(SET_FLD, DRSCTRL_DRSCLR_EN) |
		p_fld(3, DRSCTRL_DRS_CNTX) |
		p_fld(!enable_self_wakeup,
		DRSCTRL_DRS_SELFWAKE_DMYRD_DIS) |
		p_fld(CLEAR_FLD, DRSCTRL_DRSOPT2));

	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD),
		p_fld(SET_FLD, DUMMY_RD_DMYRD_REORDER_DIS) |
		p_fld(SET_FLD, DUMMY_RD_DMYRD_HPRI_DIS));
#endif
}

void picg_new_mode_enable(DRAMC_CTX_T * p)
{
#if (RX_PICG_NEW_MODE || TX_PICG_NEW_MODE)
	if (is_lp4_family(p)) {
		io_32_write_fld_multi_all(DDRPHY_MISC_CTRL4,
			p_fld(1, MISC_CTRL4_R_OPT2_CG_DQSIEN) |
			p_fld(1, MISC_CTRL4_R_OPT2_CG_MCK) |
			p_fld(1, MISC_CTRL4_R_OPT2_MPDIV_CG));
	}
#endif

#if TX_PICG_NEW_MODE
	/* Switch TX PICG to new mode */
	if (is_lp4_family(p)) {
		io_32_write_fld_multi_all(DDRPHY_MISC_CTRL4,
			p_fld(1, MISC_CTRL4_R_OPT2_CG_DQ) |
			p_fld(1, MISC_CTRL4_R_OPT2_CG_DQS) |
			p_fld(1, MISC_CTRL4_R_OPT2_CG_DQM));
	}
#ifndef EIGER_E2
	io_32_write_fld_multi_all(DDRPHY_MISC_CTRL3,
		/* cc notes: NOT-exist p_fld(0, MISC_CTRL3_ARPI_CG_DQSIEN_OPT) | */
		p_fld(0, MISC_CTRL3_ARPI_CG_MCK_DQ_OPT) |
		p_fld(0, MISC_CTRL3_ARPI_MPDIV_CG_DQ_OPT) |
		p_fld(0, MISC_CTRL3_ARPI_CG_DQS_OPT) |
		p_fld(0, MISC_CTRL3_ARPI_CG_DQ_OPT));
#else
	io_32_write_fld_multi_all(DDRPHY_MISC_CTRL3,
		p_fld(0, MISC_CTRL3_ARPI_CG_MCK_DQ_OPT) |
		p_fld(0, MISC_CTRL3_ARPI_MPDIV_CG_DQ_OPT) |
		p_fld(0, MISC_CTRL3_ARPI_CG_DQS_OPT) |
		p_fld(0, MISC_CTRL3_ARPI_CG_DQ_OPT));
#endif
#endif

	show_msg((INFO, "RX_PICG_NEW_MODE: "));
#if RX_PICG_NEW_MODE
	show_msg((INFO, "ON\n"));
#else
	show_msg((INFO, "OFF\n"));
#endif

	show_msg((INFO, "TX_PICG_NEW_MODE: "));
#if TX_PICG_NEW_MODE
	show_msg((INFO, "ON\n"));
#else
	show_msg((INFO, "OFF\n"));
#endif
}

void dramc_backup_registers(DRAMC_CTX_T *p, unsigned int *backup_addr,
	unsigned int backup_num)
{
	unsigned int reg_idx;

	for (reg_idx = 0; reg_idx < backup_num; reg_idx++)
		reg_backup_vlaue[reg_idx] = io32_read_4b(backup_addr[reg_idx]);
}

void dramc_restore_registers(DRAMC_CTX_T *p, unsigned int *restore_addr,
	unsigned int restore_num)
{
	unsigned int reg_idx;

	for (reg_idx = 0; reg_idx < restore_num; reg_idx++) {
		io32_write_4b(restore_addr[reg_idx],
			reg_backup_vlaue[reg_idx]);
	}
}

