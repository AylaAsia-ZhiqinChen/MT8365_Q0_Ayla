
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
 * @file dramc_cbt_api.c
 *  Basic DRAMC calibration API implementation
 */

/* -----------------------------------------------------------------------------
 *  Include files
 * -----------------------------------------------------------------------------
 */
#include "dramc_common.h"
#include "x_hal_io.h"
#include "dramc_api.h"
/* -----------------------------------------------------------------------------
 *  Global variables
 * -----------------------------------------------------------------------------
 */
unsigned char cbt_eye_scan_flag = FALSE;
unsigned char cbt_eye_scan_only_higheset_freq_flag = 1;

unsigned char cbt_vref_range_sel;
unsigned char cbt_vref_range_begin;
unsigned char cbt_vref_range_end;
unsigned char cbt_vref_range_step;

signed int ca_train_cmd_delay[CHANNEL_NUM][RANK_MAX] ={0};
unsigned int ca_train_cs_delay[CHANNEL_NUM][RANK_MAX] = {0};
signed int ca_train_clk_delay[CHANNEL_NUM][RANK_MAX] = {0};

signed char first_ca_pass[RANK_MAX][CATRAINING_NUM_MAX];
signed char last_ca_pass[RANK_MAX][CATRAINING_NUM_MAX];

#if (CONFIG_EYESCAN_LOG == 1)
U32 eye_irange, eye_vref;
#endif
extern unsigned char gCBT_EYE_Scan_flag;
extern unsigned char gCBT_EYE_Scan_only_higheset_freq_flag;

#define CBT_WORKAROUND_B0_B1_SWAP		0

#if (FOR_DV_SIMULATION_USED == 1)
#define CA_STEP		8
#define CS_STEP		4
#else
#define CA_STEP		1
#define CS_STEP		1
#endif

/* CA perbit delaycell calculate */
static void cbt_cal_delay_cell_perbit(DRAMC_CTX_T *p,
	signed short center_min, unsigned char delay_cell_ofst[],
	PASS_WIN_DATA_T final_win_per_ca[])
{
	unsigned char ca_idx;
	unsigned char ca_max;

	if (is_lp4_family(p))
		ca_max = CATRAINING_NUM_LP4;
	else
		ca_max = CATRAINING_NUM_LP3;

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_CBT)
	if(p->femmc_Ready==1)
	{
		for (ca_idx = 0; ca_idx < ca_max; ca_idx++)
		{
			delay_cell_ofst[ca_idx] = p->pSavetimeData->u1CBTCA_PerBit_DelayLine_Save[p->channel][p->rank][ca_idx];
			show_msg((INFO,
					"delay_cell_ofst[%d]=%d cells\n",
					ca_idx, delay_cell_ofst[ca_idx]));
		}
	}
	else
#endif
	{
		/* calculate delay cell perbit */
		for (ca_idx = 0; ca_idx < ca_max; ca_idx++) {
			unsigned char u1PIDiff =
				final_win_per_ca[ca_idx].win_center -
				center_min;
			if (p->delay_cell_timex100 != 0) {
				delay_cell_ofst[ca_idx] = (u1PIDiff * 100000000 /
					(p->frequency * 64)) / p->delay_cell_timex100;
				show_msg((INFO,
					"delay_cell_ofst[%d]=%d cells (%d PI)\n",
					ca_idx, delay_cell_ofst[ca_idx],
					u1PIDiff));
			} else {
				show_msg((INFO, "Error: Cell time %s is 0\n",
					"(p->delay_cell_timex100)"));
				memset((void *)delay_cell_ofst, 0, ca_max);
				break;
			}
		}
#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_CBT)
		for (ca_idx = 0; ca_idx < ca_max; ca_idx++)
			p->pSavetimeData->u1CBTCA_PerBit_DelayLine_Save[p->channel][p->rank][ca_idx] = delay_cell_ofst[ca_idx];
#endif
	}
}

static unsigned char cbt_enable_perbit_adjust(DRAMC_CTX_T *p)
{
	unsigned char ret = 0;

	/* If high freq && delay_cell is calibrated */
	if ((p->frequency >= PERBIT_THRESHOLD_FREQ) &&
		(p->delay_cell_timex100 != 0))
		ret = 1;

	return ret;
}

void cbt_pos_cal(DRAMC_CTX_T *p, signed char *ca_macro_delay,
	unsigned char *ca_center_diff)
{
    unsigned char rank_idx, die_idx, ca_idx, macro_idx, macro_mum;
    signed char intersect_min_bymacro[2], intersect_max_bymacro[2];
    signed char intersect_min_bybit[CATRAINING_NUM_MAX], intersect_max_bybit[CATRAINING_NUM_MAX];
    signed char ca_center_min[2];//, s1CACenter[CATRAINING_NUM_MAX];
    signed char temp_first_pass, temp_last_pass;
    unsigned char ca_bit_num = 0;
    signed char ca_center[CATRAINING_NUM_MAX];

    if (is_lp4_family(p))
        ca_bit_num = 6;
    else
        ca_bit_num = 10;

    show_msg((INFO, "[CATrainingPosCal] consider %d rank data\n", p->rank +1));

    for(macro_idx = 0; macro_idx < 2; macro_idx++) {
        intersect_min_bymacro[macro_idx] = -127; // 127
        intersect_max_bymacro[macro_idx] = 127; //-127
        ca_center_min[macro_idx] = 0x7f;
    }

    for(ca_idx = 0; ca_idx < ca_bit_num; ca_idx++) {
        intersect_min_bybit[ca_idx] = -127; // 127
        intersect_max_bybit[ca_idx] = 127; //-127

        if (is_lp4_family(p)) {
            macro_idx = 0;
        } else {
            if(ca_idx == 0 || ca_idx == 1) {
                macro_idx = 0;
            } else
                macro_idx = 1;
        }

        for (rank_idx = RANK_0; rank_idx < (p->rank + 1); rank_idx++)
        {
                temp_first_pass = first_ca_pass[rank_idx][ca_idx];
                temp_last_pass = last_ca_pass[rank_idx][ca_idx];
                //mcSHOW_DBG_MSG(("RK%d, Die%d ,CA%d,iFirstCAPass = %d, iLastCAPass=%d \n",u1RankIdx, u1DieIdx, u1CAIdx, s1TempFirstPass, s1TempLastPass));

                // Intersection by Macro
                if (temp_first_pass > intersect_min_bymacro[macro_idx])
                    intersect_min_bymacro[macro_idx] = temp_first_pass;

                if (temp_last_pass < intersect_max_bymacro[macro_idx])
                    intersect_max_bymacro[macro_idx] = temp_last_pass;

                // intersection by CA bit
                if (temp_first_pass > intersect_min_bybit[ca_idx])
                    intersect_min_bybit[ca_idx] = temp_first_pass;

                if (temp_last_pass < intersect_max_bybit[ca_idx])
                    intersect_max_bybit[ca_idx] = temp_last_pass;
        }

        ca_center[ca_idx] = (intersect_min_bybit[ca_idx] + intersect_max_bybit[ca_idx]) / 2;

        if (ca_center[ca_idx] < ca_center_min[macro_idx])
            ca_center_min[macro_idx] = ca_center[ca_idx];
    }

    // If CA perbit, choose min CA PI of all bits.
    // If CA perbyte, choose middle position of intersenction range of all bits.
    if (is_lp4_family(p))
        macro_mum = 1;
    else //LP3
        macro_mum = 2;

    // CA perbit enable
    if (cbt_enable_perbit_adjust(p))
    {
        for (macro_idx = 0; macro_idx < macro_mum; macro_idx++)
        {
            #if LP3_CATRAING_SHIFT_CLK_PI
            if(is_lp4_family(p))
            #endif
            {
                if (ca_center_min[macro_idx] < 0) //don't move clk
                {
                    //mcSHOW_DBG_MSG(("warning : Macro%d minimum CA PI delay is %d(<0) and changed to 0\n", u1MacroIdx, s1CACenter_min[u1MacroIdx]));
                    ca_center_min[macro_idx] = 0;
                }
            }

            ca_macro_delay[macro_idx] = ca_center_min[macro_idx];
        }

        show_msg((INFO, "delaycelltimex100 = %d/100 ps\n", p->delay_cell_timex100));

        for (ca_idx = 0; ca_idx < ca_bit_num; ca_idx++) {
            if (is_lp4_family(p))
                macro_idx = 0;
            else {
                if(ca_idx == 0 || ca_idx == 1)
                    macro_idx = 0;
                else
                    macro_idx = 1;
            }

            #if LP3_CATRAING_SHIFT_CLK_PI
            if (is_lp4_family(p) && ca_center[ca_idx] < 0) //don't move clk
            #else
            if (ca_center[ca_idx] < 0) //don't move clk
            #endif
            {
                ca_center[ca_idx] = 0;
                ca_center_diff[ca_idx] = 0;
            } else {
                ca_center_diff[ca_idx] = ca_center[ca_idx] - ca_center_min[macro_idx];
            }
            show_msg((INFO, "[CA %d] Center %d (%d~%d) winsize %d\n",
				ca_idx, ca_center[ca_idx],
				intersect_min_bybit[ca_idx],
				intersect_max_bybit[ca_idx],
				(intersect_max_bybit[ca_idx] - intersect_min_bybit[ca_idx])));
            show_msg((INFO, "Diff = %d PI ", ca_center_diff[ca_idx]));

            ca_center_diff[ca_idx] = (ca_center_diff[ca_idx]*100000000 /
				(p->frequency << 6)) / p->delay_cell_timex100;
            show_msg((INFO, "(%d cell)", ca_center_diff[ca_idx]));

            if (ca_center_diff[ca_idx] > 15) {
                show_msg((INFO, "[WARNING] CA%d delay cell %d > 15, adjust to 15 cell",
					ca_idx, ca_center_diff[ca_idx]));
                ca_center_diff[ca_idx] = 15;
            }
            show_msg((INFO, "\n"));
        }

        for (macro_idx = 0; macro_idx < macro_mum; macro_idx++) {
            show_msg((INFO, "CA PerBit enable=%d, Macro%d, CA PI delay=%d\n",
				cbt_enable_perbit_adjust(p), macro_idx, ca_macro_delay[macro_idx]));
        }
    }
    else { //CA perbyte
        for (macro_idx = 0; macro_idx < macro_mum; macro_idx++) {
            ca_macro_delay[macro_idx] = (intersect_min_bymacro[macro_idx] +
				intersect_max_bymacro[macro_idx]) / 2;
            if (ca_macro_delay[macro_idx] < 0) { //don't move clk
                //mcSHOW_DBG_MSG(("warning : CA PI delay is %d(<0) and changed to 0\n", ps1CAMacroDelay[u1MacroIdx]));
                ca_macro_delay[macro_idx] = 0;
            }
            show_msg((INFO, "CA PerBit enable=%d, Macro%d, CA PI delay=%d (%d~%d)\n",
                cbt_enable_perbit_adjust(p), macro_idx, ca_macro_delay[macro_idx],
                intersect_min_bymacro[macro_idx], intersect_max_bymacro[macro_idx]));
        }
    }
}
#if (SUPPORT_TYPE_LPDDR4 && SIMULATION_CBT)

#define ENABLE_CBT_DIV4_WA		(LPDDR4_DIV4_MODE_EN)

#if ENABLE_CBT_DIV4_WA
void cbt_div4_workaound(DRAMC_CTX_T *p, DIV_MODE_T mode_switch_to)
{
	unsigned char div4_en;
	unsigned char div_mctl;

	if (mode_switch_to == DIV4_MODE) {
		div4_en = 1;
		div_mctl = 0;
	} else {
		div4_en = 0;
		div_mctl = 2;
	}

	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI2, CLEAR_FLD,
		B2_DLL_ARPI2_RG_ARDLL_PHDET_EN_B2);
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI2, CLEAR_FLD,
		B1_DLL_ARPI2_RG_ARDLL_PHDET_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI2, CLEAR_FLD,
		B0_DLL_ARPI2_RG_ARDLL_PHDET_EN_B0);
	io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI2, CLEAR_FLD,
		CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);

	delay_us(1);

	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DRAMC_REG_SHU_CONF0),
		p_fld(!div4_en, SHU_CONF0_FREQDIV4) |
		p_fld(div4_en, SHU_CONF0_FDIV2) |
		p_fld(!div4_en, SHU_CONF0_DM64BITEN) |
		p_fld(div4_en, SHU_CONF0_WDATRGO));

	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DQ6),
		div4_en, SHU_B0_DQ6_RG_TX_ARDQ_SER_MODE_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_SHU_B1_DQ6),
		div4_en, SHU_B1_DQ6_RG_TX_ARDQ_SER_MODE_B1);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DQ6),
		div4_en, SHU_B2_DQ6_RG_TX_ARDQ_SER_MODE_B2);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_SHU_CA_CMD6),
		div4_en, SHU_CA_CMD6_RG_TX_ARCMD_SER_MODE);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_SHU_CA_DLL0), div_mctl,
		SHU_CA_DLL0_RG_ARDLL_DIV_MCTL_CA);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_SHU_B0_DLL0), div_mctl,
		SHU_B0_DLL0_RG_ARDLL_DIV_MCTL_B0);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_SHU_B1_DLL0), div_mctl,
		SHU_B1_DLL0_RG_ARDLL_DIV_MCTL_B1);
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_SHU_B2_DLL0), div_mctl,
		SHU_B2_DLL0_RG_ARDLL_DIV_MCTL_B2);

	io_32_write_fld_align(DDRPHY_CA_DLL_ARPI2, SET_FLD,
		CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);
	delay_us(1);
	io_32_write_fld_align_all(DDRPHY_CA_DLL_ARPI2, SET_FLD,
		CA_DLL_ARPI2_RG_ARDLL_PHDET_EN_CA);
	io_32_write_fld_align_all(DDRPHY_B2_DLL_ARPI2, SET_FLD,
		B2_DLL_ARPI2_RG_ARDLL_PHDET_EN_B2);
	io_32_write_fld_align_all(DDRPHY_B1_DLL_ARPI2, SET_FLD,
		B1_DLL_ARPI2_RG_ARDLL_PHDET_EN_B1);
	io_32_write_fld_align_all(DDRPHY_B0_DLL_ARPI2, SET_FLD,
		B0_DLL_ARPI2_RG_ARDLL_PHDET_EN_B0);
	delay_us(1);
}
#endif

unsigned char get_cbt_vref_pin_mux_revert_value(DRAMC_CTX_T *p,
	unsigned char vref_level);

/* cbt_lp4: cmd_bus_training_lp4
 *  start the calibrate the skew between
 *	(1) Clk pin and CAx pins.
 *	(2) Clk and CS pin
 *	(3) Vref(ca) driving
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @retval status          (DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
void cbt_switch_freq(DRAMC_CTX_T *p, unsigned char freq)
{
#if DUAL_FREQ_K
	if (freq == CBT_LOW_FREQ)
		cbt_dramc_dfs_direct_jump(p, DRAM_DFS_SHUFFLE_MAX - 1);
	else
		cbt_dramc_dfs_direct_jump(p, DRAM_DFS_SHUFFLE_1);
#endif
}

static void set_dram_mrcbt_on_off(DRAMC_CTX_T *p, unsigned char on_off,
	unsigned char op_fsp)
{
	if (on_off) {
		/* MR13 OP[0]=1, enable CBT */
		dram_mr.mr13_value[op_fsp] |= 0x1;

		/*
		* op[7] = !(p->dram_fsp),
		* dram will switch to another FSP_OP automatically
		*/
		if (op_fsp)
			dram_mr.mr13_value[op_fsp] &= 0x7f; /* OP[7] =0; */
		else
			dram_mr.mr13_value[op_fsp] |= 0x80; /* OP[7] =1; */
		if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1) {
			io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV),
				SET_FLD, WRITE_LEV_BYTEMODECBTEN);
		}
	} else {
		dram_mr.mr13_value[op_fsp] &= 0xfe; /* disable CBT */

		if (op_fsp)
			dram_mr.mr13_value[op_fsp] |= 0x80; /* OP[7] =1; */
		else
			dram_mr.mr13_value[op_fsp] &= 0x7f; /* OP[7] =0; */
	}

#if (CBT_K_RANK1_USE_METHOD == 1)
	dramc_mode_reg_write_by_rank(p, p->rank, MR13,
		dram_mr.mr13_value[op_fsp]);
#else
	dramc_mode_reg_write_by_rank(p, RANK_0, MR13,
		dram_mr.mr13_value[op_fsp]);
#endif
}

void CBTEntry(DRAMC_CTX_T *p, unsigned char operating_fsp,
	unsigned short operation_frequency)
{
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL),
		p_fld(CLEAR_FLD, DRAMC_PD_CTRL_PHYCLKDYNGEN) |
		p_fld(CLEAR_FLD, DRAMC_PD_CTRL_DCMEN) |
		p_fld(0x3, DRAMC_PD_CTRL_DQIEN_BUFFEN_OPT)); /* cc add to enable DQ-RX */

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_STBCAL), CLEAR_FLD,
		STBCAL_DQSIENCG_NORMAL_EN);

	/* Step 0.0 CKE go high (Release R_DMCKEFIXOFF, R_DMCKEFIXON=1) */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL),
		SET_FLD, DRAMC_PD_CTRL_MIOCKCTRLOFF);	/* MIOCKCTRLOFF=1 */

	cke_fix_on_off(p, !p->rank, CKE_FIXOFF, CKE_WRITE_TO_ONE_CHANNEL);
	cke_fix_on_off(p, p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	/* Step 0: MRW MR13 OP[0]=1 to enable CBT */
	set_dram_mrcbt_on_off(p, ENABLE, operating_fsp);

	/*
	* Step 0.1: before CKE low, Let DQS=0 by R_DMwrite_level_en=1,
	* spec: DQS_t has to retain a low level during tDQSCKE period
	*/
	if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV),
			SET_FLD, WRITE_LEV_WRITE_LEVEL_EN);
	}

	delay_us(1);

	/* Step 1.0: let CKE go low */
	cke_fix_on_off(p, p->rank, CKE_FIXOFF, CKE_WRITE_TO_ONE_CHANNEL);

	/* Step 1.1 : let IO to O1 path valid */
	if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE) {
		/*
		* Let R_DMFIXDQIEN1=1 (byte1), 0xd8[13]
		* ==> Note: Do not enable again.
		* Currently set in o1_path_on_off
		*/

		/* Let DDRPHY RG_RX_ARDQ_SMT_EN_B1=1 (byte1) */
		o1_path_on_off(p, ENABLE);
	}
	/* Step 1.1 : let IO to O1 path valid */
	if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1) {
		/*
		* Let R_DMFIXDQIEN1=1 (byte1), 0xd8[13]
		* ==> Note: Do not enable again.
		* Currently set in o1_path_on_off
		*/

		/* Let DDRPHY RG_RX_ARDQ_SMT_EN_B1=1 (byte1) */
		o1_path_on_off(p, ENABLE);
	}
}

void cbt_exit(DRAMC_CTX_T *p, unsigned char operating_fsp,
	unsigned char operation_frequency)
{
	/* Workaround for DIV4 mode CBT */
#if ENABLE_CBT_DIV4_WA
	if (get_div_mode(p) == DIV4_MODE) {
		cbt_div4_workaound(p, DIV4_MODE);
		show_msg((INFO, "[CBT] Workaoround for DIV4. Switch back to DIV4\n"));
	}
#endif

	if ((p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE) ||
		(p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1)) {
		/*
		* Step 1: CKE go high
		* (Release R_DMCKEFIXOFF, R_DMCKEFIXON=1)
		*/
		cke_fix_on_off(p, p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

		/* Step 2:wait tCATX, wait tFC */
		delay_us(1);

		/*
		* Step 3: MRW to command bus training exit
		* (MR13 OP[0]=0 to disable CBT)
		*/
		set_dram_mrcbt_on_off(p, DISABLE, operating_fsp);
		cke_fix_on_off(p, !p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);
	}
	/*
	* Step 4:
	* Disable O1 path output
	*/
	if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE) {
		/* Let DDRPHY RG_RX_ARDQ_SMT_EN_B1=0 */
		o1_path_on_off(p, DISABLE);

		/*
		* Let FIXDQIEN1=0 ==> Note: Do not enable again.
		* Moved into o1_path_on_off
		*/
	}

		/* Step 1.1 : let IO to O1 path valid */
	if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1) {
		/*
		* Let R_DMFIXDQIEN1=1 (byte1), 0xd8[13]
		* ==> Note: Do not enable again.
		* Currently set in o1_path_on_off
		*/

		/* Let DDRPHY RG_RX_ARDQ_SMT_EN_B1=1 (byte1) */
		o1_path_on_off(p, DISABLE);
		//Disable Byte mode CBT enable bit
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV), CLEAR_FLD,
			WRITE_LEV_BYTEMODECBTEN);
	}
}

static void cbt_set_fsp(DRAMC_CTX_T *p, unsigned char operating_fsp,
	unsigned char final_set_flag)
{
	if (operating_fsp == FSP_0) {/* OP[6], fsp_wr=0, OP[7] =0; */
		dram_mr.mr13_value[operating_fsp] &= ~(SET_FLD << 6);
		dram_mr.mr13_value[operating_fsp] &= 0x7f;
	} else {/* OP[6], fsp_wr=1, OP[7] =0; */
#if DUAL_FREQ_K
		 if (final_set_flag == 0) {
			dram_mr.mr13_value[operating_fsp] |= (SET_FLD << 6);
			dram_mr.mr13_value[operating_fsp] &= 0x7f;
		} else {
			dram_mr.mr13_value[operating_fsp] |= (SET_FLD << 6);
			dram_mr.mr13_value[operating_fsp] |= 0x80;
		}
#else
		dram_mr.mr13_value[operating_fsp] |= (SET_FLD << 6);
		dram_mr.mr13_value[operating_fsp] |= 0x80;
#endif
	}

	dramc_mode_reg_write_by_rank(p, p->rank, MR13,
		dram_mr.mr13_value[operating_fsp]);
}

static void cbt_set_vref(DRAMC_CTX_T *p, unsigned int vref_level,
	unsigned char operating_fsp, unsigned char final_set_flag)
{
	unsigned int dbg_value;
	unsigned char vref_value_pinmux;

	vref_value_pinmux =
		(get_cbt_vref_pin_mux_revert_value(p, vref_level) & 0x3f);

	if ((p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE) &&
		(final_set_flag == DISABLE)) {
		dram_mr.mr12_value[p->channel][p->rank][operating_fsp] =
		    ((cbt_vref_range_sel & 0x1) << 6) | vref_value_pinmux;

		/* MR12, bit[25:20]=OP[5:0]  bit 26=OP[6] */
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV),
			vref_level, WRITE_LEV_DMVREFCA);

		/* If only B23 used, Select DQS1 (which is mapped to
		 * PHY PAD_DQS2) to send VREFca. Else, shall select
		 * DQS0.
		 */
	#if CBT_WORKAROUND_B0_B1_SWAP
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_PADCTRL),
			SET_FLD, PADCTRL_DRAMOEN);
	#endif

		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV),
			SET_FLD, WRITE_LEV_DQS_SEL);

		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV),
			0xa, WRITE_LEV_DQSBX_G);

		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV),
			SET_FLD, WRITE_LEV_DQS_WLEV);
		delay_us(1);
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV),
			CLEAR_FLD, WRITE_LEV_DQS_WLEV);

	#if CBT_WORKAROUND_B0_B1_SWAP
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_PADCTRL),
			CLEAR_FLD, PADCTRL_DRAMOEN);
	#endif

	} else {
		dbg_value = (((cbt_vref_range_sel & 0x1) << 6) |
			(vref_level & 0x3f));
		dram_mr.mr12_value[p->channel][p->rank][operating_fsp] =
			dbg_value;
		show_msg2((INFO, "dbg_value = 0x%x\n", dbg_value));

		dramc_mode_reg_write_by_rank(p, p->rank, MR12, dbg_value);
	}

	/* wait tVREF_LONG */
	delay_us(1);
}

static unsigned int cbt_delay_compare(DRAMC_CTX_T *p, signed int delay)
{
	unsigned int result = 0, result0 = 0, ready;
	unsigned int time_cnt;

	time_cnt = TIME_OUT_CNT;

	if (delay < 0) {	/* Set CLK delay */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD9),
			p_fld(CLEAR_FLD,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
			p_fld(-delay,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK) |
			p_fld(-delay,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
	} else {		/* Set CA output delay */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD9),
			p_fld(delay,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
			p_fld(CLEAR_FLD,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK) |
			p_fld(CLEAR_FLD, SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));

	}

	/* Let R_DMTESTCATRAIN=1 to enable HW CAPAT Generator */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_CATRAINING1), SET_FLD,
		CATRAINING1_TESTCATRAIN);

	/* Check CA training compare ready */
	do {
		ready =
		    io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_TCMDO1LAT),
			TCMDO1LAT_CATRAIN_CMP_CPT);
		time_cnt--;
		delay_us(1);
	} while ((ready == 0) && (time_cnt > 0));

	if (time_cnt == 0)
		show_err("[cbt_delay_compare] Resp fail (time out)\n");

	/* Get CA training compare result */
	if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE) {
		result = io_32_read_fld_align(
			DRAMC_REG_ADDR(DRAMC_REG_TCMDO1LAT),
			TCMDO1LAT_CATRAIN_CMP_ERR);
	} else {
		unsigned int dq_o1;

		result0 = io_32_read_fld_align(
			DRAMC_REG_ADDR(DRAMC_REG_TCMDO1LAT),
			TCMDO1LAT_CATRAIN_CMP_ERR0);
		result = io_32_read_fld_align(
			DRAMC_REG_ADDR(DRAMC_REG_TCMDO1LAT),
			TCMDO1LAT_CATRAIN_CMP_ERR);

		dq_o1 = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_DQO1),
			MISC_DQO1_DQO1_RO);
		show_msg((INFO, "dq_o1 = 0x%x, result0 = 0x%x, result = 0x%x\n",
			dq_o1, result0, result));
	}

	/* Let R_DMTESTCATRAIN=0 to disable HW CAPAT Generator */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_CATRAINING1),
		CLEAR_FLD, CATRAINING1_TESTCATRAIN);

	return (result | result0);	/* return pattern compre result */
}

static unsigned int cbt_delay_cs_compare(DRAMC_CTX_T *p, unsigned int delay)
{
	/* unsigned char *lpddr_phy_mapping = NULL; */
	unsigned int result, ready;
	unsigned int time_cnt;
	unsigned int dq_o1;
	unsigned int byte_index;

	time_cnt = TIME_OUT_CNT;

#if 0 /* cc mark */
	lpddr_phy_mapping =
		(unsigned char *)lpddr4_phy_mapping_pop[p->channel];
#endif
	/* Set CS output delay */
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD9), delay,
		SHU_R0_CA_CMD9_RG_RK0_ARPI_CS);

	/* Step 5: toggle CS/CA for CS training by R_DMTCMDEN */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), SET_FLD,
		SPCMD_TCMDEN);
	do {
		ready = io_32_read_fld_align(
			DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP),
			SPCMDRESP_TCMD_RESPONSE);
		time_cnt--;
		delay_us(1);
	} while ((ready == 0) && (time_cnt > 0));

	if (time_cnt == 0)
		show_err("[cbt_delay_cs_compare] Resp fail (time out)\n");

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SPCMD), CLEAR_FLD,
		SPCMD_TCMDEN);

	/*
	* Step 6: check CS training result on DQ[13:8] by O1, DDRPHYCFG 0xF80
	* Expected CA value is h2a (CA pulse width is 6UI, CS pulse is 1UI)
	*/
	dq_o1 = io_32_read_fld_align(DRAMC_REG_ADDR(DDRPHY_MISC_DQO1),
		MISC_DQO1_DQO1_RO);

#if 0 /* cc mark */
	result = 0;
	for (byte_index = 8; byte_index <= 13; byte_index++) {
		result |=
		    (((dq_o1 & (1 << lpddr_phy_mapping[byte_index])) >>
		      (lpddr_phy_mapping[byte_index])) << (byte_index - 8));
	}
#endif
	result = (dq_o1 >> 8) & 0x3f;

	show_msg2((INFO, "CS Dly = %d, DQ_O1 = %x, Result=0x%x\n",
		delay, dq_o1, result));

	return result;	/* return pattern compre result */
}

void dramc_cmd_bus_training_post_process_lp4(DRAMC_CTX_T *p)
{
	signed int cs_Final_clk_elay, cs_Final_cmd_delay, cs_finalcs_delay;
	unsigned char backup_rank, irank;

	/* CBT Rank0/1 must set Clk/CA/CS the same */

	show_msg((INFO,
		"[dramc_cmd_bus_training_post_process] p->frequency=%d\n",
		p->frequency));

	backup_rank = get_rank(p);

	cs_Final_clk_elay =
		(ca_train_clk_delay[p->channel][RANK_0] +
		ca_train_clk_delay[p->channel][RANK_1]) / 2;
		ca_train_clk_delay[p->channel][RANK_0] = cs_Final_clk_elay;
		ca_train_clk_delay[p->channel][RANK_1] = cs_Final_clk_elay;

	cs_Final_cmd_delay =
		(ca_train_cmd_delay[p->channel][RANK_0] +
		ca_train_cmd_delay[p->channel][RANK_1]) / 2;
		ca_train_cmd_delay[p->channel][RANK_0] = cs_Final_cmd_delay;
		ca_train_cmd_delay[p->channel][RANK_1] = cs_Final_cmd_delay;

	cs_finalcs_delay =
		(ca_train_cs_delay[p->channel][RANK_0] +
		ca_train_cs_delay[p->channel][RANK_1]) / 2;
		ca_train_cs_delay[p->channel][RANK_0] = cs_finalcs_delay;
		ca_train_cs_delay[p->channel][RANK_1] = cs_finalcs_delay;

	for (irank = RANK_0; irank <= RANK_1; irank++) {
		set_rank(p, irank);
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD9),
			p_fld(cs_Final_clk_elay,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK) |
			p_fld(cs_Final_cmd_delay,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
			p_fld(cs_finalcs_delay,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
	}

	show_msg((INFO, "Clk Dly = %d\nCmd Dly = %d\nCS Dly = %d\n",
		cs_Final_clk_elay, cs_Final_cmd_delay, cs_finalcs_delay));

	set_rank(p, backup_rank);

}

static void cbt_adjust_cs(DRAMC_CTX_T *p)
{
	signed int first_cs_pass, last_cs_pass, cs_final_delay;
	unsigned int delay, value_read_back, cs_win_size;
	unsigned int backup_rank;
	unsigned int rank_idx;

	backup_rank = get_rank(p);
#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_CBT)
    if(p->femmc_Ready==1)
    {
        cs_final_delay=p->pSavetimeData->u1CBTCsDelay_Save[p->channel][p->rank];
		show_diag((INFO, "[CBT_LP4] CS Dly: %d\n", cs_final_delay));
    }
    else
#endif
	{
		first_cs_pass = -1;
		last_cs_pass = -1;

		for (delay = 0; delay <= MAX_CS_PI_DELAY; delay += CS_STEP) {
			value_read_back = cbt_delay_cs_compare(p, delay);

			if (first_cs_pass == -1) {
				if (value_read_back == 0x2a) { /* compare pass */
					first_cs_pass = delay;
				}
			} else if (last_cs_pass == -1) {
				if (value_read_back != 0x2a) { /* compare fail */
					last_cs_pass = delay - 1;
					break; /* Early break */
				} else if ((delay + CS_STEP) > MAX_CS_PI_DELAY) {
					last_cs_pass = delay;
					break; /* Early break */
				}
			}

			/*  Wait time before output CS pattern to DDR again..  */
			delay_us(1);
		}

		cs_win_size = last_cs_pass - first_cs_pass +
			(last_cs_pass == first_cs_pass ? 0 : 1);

		/*  if winSize >32, CS delay= winSize -32. */
		if (cs_win_size > ((MAX_CS_PI_DELAY + 1) >> 1))
			cs_final_delay = cs_win_size - ((MAX_CS_PI_DELAY + 1) >> 1);
		else
			cs_final_delay = 0;

		ca_train_cs_delay[p->channel][p->rank] = cs_final_delay;
	show_diag((INFO, "[CBT_LP4] CS Dly: %d, CSPass First: %d, Last: %d\n",
			cs_final_delay, first_cs_pass, last_cs_pass));

		if (p->rank == RANK_1) {
			cs_final_delay = (ca_train_cs_delay[p->channel][RANK_0] +
				ca_train_cs_delay[p->channel][RANK_1]) / 2;
		}
	}
	/*
	* Set CS output delay after training
	* p->rank = RANK_0, save to Reg Rank0 and Rank1, p->rank = RANK_1,
	* save to Reg Rank1
	*/
	for (rank_idx = RANK_0; rank_idx < backup_rank + 1; rank_idx++) {
		set_rank(p, rank_idx);

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_CBT)
        if(p->femmc_Ready==0)
        {
            p->pSavetimeData->u1CBTCsDelay_Save[p->channel][p->rank]= cs_final_delay;
        }
#endif
		io_32_write_fld_align_all(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD9),
			cs_final_delay, SHU_R0_CA_CMD9_RG_RK0_ARPI_CS);

		ca_train_cs_delay[p->channel][rank_idx] = cs_final_delay;
	}

	set_rank(p, backup_rank);

}

static void cbt_set_ca_clk_result(DRAMC_CTX_T *p, signed char final_ca_clk)
{
	unsigned int rank_idx;
	unsigned int backup_rank;
	unsigned int cmd_delay, clk_delay;

	backup_rank = get_rank(p);
#if 0
	cmd_delay = ca_train_cmd_delay[p->channel][RANK_0];
	clk_delay = ca_train_clk_delay[p->channel][RANK_0];

	if (backup_rank == RANK_1) {
		cmd_delay += ca_train_cmd_delay[p->channel][RANK_1];
		clk_delay += ca_train_clk_delay[p->channel][RANK_1];

		cmd_delay /= 2;
		clk_delay /= 2;
	}
#endif
	/*
	* Set CA_PI_Delay after trainging
	* p->rank = RANK_0, save to Reg Rank0 and Rank1,
	* p->rank = RANK_1, save to Reg Rank1
	*/
	for (rank_idx = RANK_0; rank_idx < (backup_rank + 1); rank_idx++) {
		set_rank(p, rank_idx);
		if (final_ca_clk < 0) { /* Set CLK delay */
			ca_train_clk_delay[p->channel][p->rank] = 0;
			show_err2("Warning!! Clk Dly = %d, adjust to 0 !!\n",
				-final_ca_clk);
			ca_train_cmd_delay[p->channel][p->rank] = 0;
		} else { /*  Set CA output delay */
			if (final_ca_clk >= 64)
				ca_train_cmd_delay[p->channel][p->rank] = final_ca_clk - 64;
			else
				ca_train_cmd_delay[p->channel][p->rank] = final_ca_clk;
			ca_train_clk_delay[p->channel][p->rank] = 0;
		}
		if (final_ca_clk < 64) {
			io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA7),
				p_fld(0, SHU_SELPH_CA7_DLY_RA0) |
				p_fld(0, SHU_SELPH_CA7_DLY_RA1) |
				p_fld(0, SHU_SELPH_CA7_DLY_RA2) |
				p_fld(0, SHU_SELPH_CA7_DLY_RA3) |
				p_fld(0, SHU_SELPH_CA7_DLY_RA4) |
				p_fld(0, SHU_SELPH_CA7_DLY_RA5));

				// Note: CKE UI must sync CA UI (CA and CKE delay circuit are same) @Lin-Yi
				// To avoid tXP timing margin issue
				io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA5),
					0, SHU_SELPH_CA5_DLY_CKE);
				io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA6),
					0, SHU_SELPH_CA6_DLY_CKE1);
		} else {
				io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA7),
					p_fld(2, SHU_SELPH_CA7_DLY_RA0) |
					p_fld(2, SHU_SELPH_CA7_DLY_RA1) |
					p_fld(2, SHU_SELPH_CA7_DLY_RA2) |
					p_fld(2, SHU_SELPH_CA7_DLY_RA3) |
					p_fld(2, SHU_SELPH_CA7_DLY_RA4) |
					p_fld(2, SHU_SELPH_CA7_DLY_RA5));

				// Note: CKE UI must sync CA UI (CA and CKE delay circuit are same) @Lin-Yi
				// To avoid tXP timing margin issue
				io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA5),
					2, SHU_SELPH_CA5_DLY_CKE);
				io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_SELPH_CA6),
					2, SHU_SELPH_CA6_DLY_CKE1);
		}

		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD9),
			p_fld(ca_train_cmd_delay[p->channel][p->rank], SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
			p_fld(ca_train_clk_delay[p->channel][p->rank], SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK));

		cmd_delay = ca_train_cmd_delay[p->channel][rank_idx];
		clk_delay = ca_train_clk_delay[p->channel][rank_idx];
		show_diag((INFO, "[CBTSetCACLKResult] CH%d, RK%d, CA Dly = %d, CLK dly =%d\n",
			p->channel, rank_idx, cmd_delay, clk_delay));

	
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
        if(p->femmc_Ready==0)
        {
            p->pSavetimeData->u1CBTClkDelay_Save[p->channel][p->rank]= ca_train_clk_delay[p->channel][p->rank];
            p->pSavetimeData->u1CBTCmdDelay_Save[p->channel][p->rank]= ca_train_cmd_delay[p->channel][p->rank];
        }
#endif

#if (CONFIG_EYESCAN_LOG == 1)
		gEyeScan_CaliDelay[0] = ca_train_cmd_delay[p->channel][rank_idx] + MAX_CLK_PI_DELAY;
#endif
	}

	set_rank(p, backup_rank);
}

unsigned char get_cbt_vref_pin_mux(DRAMC_CTX_T *p,
	unsigned char vref_level)
{
#if 0 /* cc mark */
	//unsigned char vref_bit, vref_new, vref_org;
	unsigned char vref_bit, vref_org;
	unsigned char vref_new;

	vref_new = 0;
	vref_org = ((cbt_vref_range_sel & 0x1) << 6) | (vref_level & 0x3f);
	for (vref_bit = 0; vref_bit < 8; vref_bit++) {
		if (vref_org & (1 << vref_bit)) {
			vref_new |= (1 <<
				lpddr4_phy_mapping_pop[p->channel][vref_bit]);
		}
	}

	show_msg2((INFO, "=== vref_new: 0x%x --> 0x%x\n", vref_org, vref_new));

	return vref_new;
#else
	return ((cbt_vref_range_sel & 0x1) << 6) | (vref_level & 0x3f);
#endif
}

unsigned char get_cbt_vref_pin_mux_revert_value(DRAMC_CTX_T *p,
	unsigned char vref_level)
{
#if 0 /* cc mark */
	unsigned char vref_bit, vref_new, vref_org;

	vref_new = 0;
	vref_org = vref_level;
	for (vref_bit = 0; vref_bit < 8; vref_bit++) {
		vref_new |= ((vref_org >>
		lpddr4_phy_mapping_pop[p->channel][vref_bit]) & 1) << vref_bit;
	}

	show_msg2((INFO, "=== Revert vref_new: 0x%x --> 0x%x\n",
		vref_org, vref_new));

	return vref_new;
#else
	return vref_level;
#endif
}

static void cbt_enter_lp4(DRAMC_CTX_T *p, unsigned char vref_level)
{
	unsigned char operating_fsp;
	unsigned short operation_frequency;

	operating_fsp = p->dram_fsp;
	operation_frequency = p->frequency;

	if (operating_fsp == FSP_1)
		cbt_switch_freq(p, CBT_LOW_FREQ);

	if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1) {
		cbt_set_fsp(p, operating_fsp, 0);
		cbt_set_vref(p, get_cbt_vref_pin_mux(p, vref_level),
			operating_fsp, 0);
	}
	/*  Step1: Enter Command Bus Training Mode */
	CBTEntry(p, operating_fsp, operation_frequency);
	/* Step 2: wait tCAENT */
	delay_us(1);
	if (operating_fsp == FSP_1)
		cbt_switch_freq(p, CBT_HIGH_FREQ);

	/* Workaround for DIV4 mode CBT */
#if ENABLE_CBT_DIV4_WA
	if (get_div_mode(p) == DIV4_MODE) {
		show_msg((INFO, "[CBT] Workaoround for DIV4. Switch to DIV8\n"));
		cbt_div4_workaound(p, DIV8_MODE);
	}
#endif
}

static void cbt_leave_lp4(DRAMC_CTX_T *p, unsigned short operation_frequency,
	unsigned char operating_fsp, unsigned char final_vref)
{
	/* wait tVREF_LONG */
	delay_us(1);

	if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1) {
		CBTEntry(p, operating_fsp, operation_frequency);
		delay_us(1);

		if (p->dram_fsp == FSP_1)
			cbt_switch_freq(p, CBT_HIGH_FREQ);
#if ENABLE_CBT_DIV4_WA
		if (get_div_mode(p) == DIV4_MODE) {
			show_msg((INFO, "[CBT] Workaoround for DIV4. Switch to DIV8\n"));
			cbt_div4_workaound(p, DIV8_MODE);
		}
#endif

	}

	/* -------------  CS and CLK ---------- */
	cbt_adjust_cs(p);

	/* -------  Going to exit Command bus training(CBT) mode.----------- */
	if (operating_fsp == FSP_1)
		cbt_switch_freq(p, CBT_LOW_FREQ);

	cbt_exit(p, operating_fsp, operation_frequency);

	/*
	* normal mode go MR12 set vref again,
	* set final_set_flag to force to MR12 flow
	*/
	if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE) {
		cbt_set_fsp(p, operating_fsp, 1);
		cbt_set_vref(p, final_vref, operating_fsp, 1);
	}

	if (operating_fsp == FSP_1)
		cbt_switch_freq(p, CBT_HIGH_FREQ);

	show_msg3((INFO, "[CmdBusTrainingLP4] Done\n\n"));

	/* restore MRR pin mux */
	set_mrr_pinmux_mapping(p);

	if (p->rank == RANK_1) {
#if (CBT_K_RANK1_USE_METHOD == 1)
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS),
			CLEAR_FLD, MRS_MRSRK);
		/* use other rank's setting */
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG),
			CLEAR_FLD, RKCFG_TXRANK);
		/*
		* TXRANKFIX should be write after TXRANK
		* or the rank will be fix at rank 1
		*/
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG),
			CLEAR_FLD, RKCFG_TXRANKFIX);
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION),
			SET_FLD,	MPC_OPTION_MPCRKEN);
#else
		dramc_rank_swap(p, RANK_0);
#endif
	}
}

unsigned char cbt_irange_check_lp4(DRAMC_CTX_T *p)
{
	U32 ret;

	if ((gCBT_EYE_Scan_flag == 1U) && ((gCBT_EYE_Scan_only_higheset_freq_flag == 0) ||
		((p->frequency == dfs_get_highest_freq(p)) &&
		(gCBT_EYE_Scan_only_higheset_freq_flag == 1)))) {
		ret = 1U;
	} else {
		ret = 0U;
	}

	show_msg(("gCBT_EYE_SCAN_flag = %d, p->frequency = %d, dfs_get_highest_freq() = %d, ret = %d\n",
		gCBT_EYE_Scan_flag, p->frequency, dfs_get_highest_freq(p), ret));
	return ret;
}

static void _cbt_delayscan_lp4(DRAMC_CTX_T *p, SCAN_WIN_DATA_T *sacn_ptr,
	signed short delay, unsigned int compare_result)
{
	unsigned int ca, temp;

	for (ca = 0; ca < CATRAINING_NUM_LP4; ca++) {
		if ((sacn_ptr->first_ca_pass[ca] != PASS_RANGE_NA)
			&& (sacn_ptr->last_ca_pass[ca] != PASS_RANGE_NA)) {
			continue;
		}

		/* Get Each bit of CA result */
		temp = (compare_result >> ca) & 0x1;

		if (sacn_ptr->first_ca_pass[ca] == PASS_RANGE_NA) {
			if (temp == 0)
				sacn_ptr->first_ca_pass[ca] = delay;
		} else if (sacn_ptr->last_ca_pass[ca] == PASS_RANGE_NA) {
			if (temp == 1) {
				if ((delay - sacn_ptr->first_ca_pass[ca])
					< 8) {
					sacn_ptr->first_ca_pass[ca] =
						PASS_RANGE_NA;
					continue;
				}
				sacn_ptr->last_ca_pass[ca] = (delay - 1);
			} else if ((delay + CA_STEP) > MAX_CA_PI_DELAY) {
				sacn_ptr->last_ca_pass[ca] = delay;
			}

			if (sacn_ptr->last_ca_pass[ca] != PASS_RANGE_NA) {
				sacn_ptr->finish_count++;
				sacn_ptr->ca_win_sum +=
					(sacn_ptr->last_ca_pass[ca] -
					sacn_ptr->first_ca_pass[ca]);
				sacn_ptr->ca_center[ca] =
					(sacn_ptr->last_ca_pass[ca] +
					sacn_ptr->first_ca_pass[ca]) >> 1;
				sacn_ptr->ca_center_sum +=
					sacn_ptr->ca_center[ca];
#if (CONFIG_EYESCAN_LOG == 1)
				if (gEyeScan_index[ca] < EYESCAN_BROKEN_NUM)
				{
					gEyeScan_Min[eye_vref+eye_irange*30][ca][gEyeScan_index[ca]] =
						sacn_ptr->first_ca_pass[ca]+MAX_CLK_PI_DELAY;
					gEyeScan_Max[eye_vref+eye_irange*30][ca][gEyeScan_index[ca]] =
						sacn_ptr->last_ca_pass[ca]+MAX_CLK_PI_DELAY;
					gEyeScan_WinSize[eye_vref+eye_irange*30][ca] =
						(sacn_ptr->last_ca_pass[ca] - sacn_ptr->first_ca_pass[ca]) +
						(sacn_ptr->last_ca_pass[ca] == sacn_ptr->first_ca_pass[ca] ? 0 : 1);

					mcSHOW_DBG_MSG(("u2VrefLevel=%d, u2VrefRange=%d, %d, uiCA=%d, index=%d (%d, %d)==\n",
						eye_vref, eye_irange, eye_vref+eye_irange*30, ca, gEyeScan_index[ca],
						gEyeScan_Min[eye_vref+eye_irange*30][ca][gEyeScan_index[ca]],
						gEyeScan_Max[eye_vref+eye_irange*30][ca][gEyeScan_index[ca]]));
					gEyeScan_index[ca] = gEyeScan_index[ca] + 1;
				}
#endif
			}
		}
	}
}

static void cbt_delayscan_lp4(DRAMC_CTX_T *p, SCAN_WIN_DATA_T *sacn_ptr)
{
	signed short delay;
	unsigned int compare_result;
	signed short start_pi;

#if (CBT_MOVE_CA_INSTEAD_OF_CLK == 1)
	start_pi = 0;
#else
	start_pi = -MAX_CLK_PI_DELAY;
#endif

	for (delay = start_pi; delay <= MAX_CA_PI_DELAY; delay += CA_STEP) {
		compare_result = cbt_delay_compare(p, delay);

		/* wait 1us between each CA pattern */
		delay_us(1);

		show_msg2((INFO, "CBTDelayCACLK Delay= %d, ",
			 delay));
		show_msg2((INFO, "CompareResult 0x%x\n", compare_result));

		_cbt_delayscan_lp4(p, sacn_ptr, delay, compare_result);

		/*
		* Wait tCACD(22clk)
		* before output CA pattern to DDR again
		*/
		delay_us(1);

		if (sacn_ptr->finish_count == CATRAINING_NUM_LP4)
			break;
	}
}

static void _cbt_scan_lp4(DRAMC_CTX_T *p, FINAL_WIN_DATA_T *final_cbt_set_ptr,
	PASS_WIN_DATA_T final_win_per_ca[], SCAN_WIN_DATA_T scan_result)
{
	signed int ca_final_center[CATRAINING_NUM_LP4] = { 0 };
	signed int ck_min = 1000;
	unsigned int ca;

	memset(ca_final_center, 0, sizeof(ca_final_center));

	if (cbt_enable_perbit_adjust(p)) {
		ck_min = 1000;
		for (ca = 0; ca < CATRAINING_NUM_LP4; ca++) {
			ca_final_center[ca] = scan_result.ca_center[ca];
			if (ca_final_center[ca] < ck_min)
				ck_min = ca_final_center[ca];
		}

		if (ck_min < 0) {
			show_msg((INFO,
				"Warning!! smallest%s%d < 0, %s !!\n",
				"CA min center = ", ck_min,
				"then adjust to 0"));
			for (ca = 0; ca < CATRAINING_NUM_LP4; ca++) {
				if (ca_final_center[ca] < 0)
					ca_final_center[ca] = 0;
			}
			ck_min = 0;
		}

		final_cbt_set_ptr->final_ca_clk = ck_min;
	} else {
		final_cbt_set_ptr->final_ca_clk = scan_result.ca_center_sum
			/ CATRAINING_NUM_LP4;
	}

	for (ca = 0; ca < CATRAINING_NUM_LP4; ca++) {
		final_win_per_ca[ca].first_pass =
			scan_result.first_ca_pass[ca];
		final_win_per_ca[ca].last_pass =
			scan_result.last_ca_pass[ca];
		final_win_per_ca[ca].win_center =
			scan_result.ca_center[ca];
		final_win_per_ca[ca].win_size = (scan_result.last_ca_pass[ca] -
			scan_result.first_ca_pass[ca]) +
			(scan_result.last_ca_pass[ca] ==
			scan_result.first_ca_pass[ca] ? 0 : 1);
#if (CONFIG_FOR_HQA_TEST_USED == 1)
		gFinalCBTCA[p->channel][p->rank][ca] = final_win_per_ca[ca].win_size;
#endif
	}
#if (CONFIG_FOR_HQA_REPORT_USED == 1)
	LOGI("\n");
	for (ca = 0U; ca < CATRAINING_NUM_LP4; ca++) {
		hqa_log_message_for_report(p, p->channel, p->rank,
			HQA_REPORT_FORMAT0_2, "CA_Center", ca,
			final_win_per_ca[ca].win_center, "NULL");

		hqa_log_message_for_report(p, p->channel, p->rank,
			HQA_REPORT_FORMAT0_2, "CA_Window", ca,
			final_win_per_ca[ca].win_size, "NULL");
	}
#endif
}

static void cbt_scan_lp4(DRAMC_CTX_T *p, unsigned char operating_fsp,
	unsigned char irange, unsigned char irange_start,
	unsigned char irange_end,	FINAL_WIN_DATA_T *final_cbt_set_ptr,
	PASS_WIN_DATA_T final_win_per_ca[])
{
	unsigned char vref_level;
	unsigned int ca;
	unsigned int min_bit;
	unsigned int min_winsize;
	unsigned int temp_winsize;
	SCAN_WIN_DATA_T scan_result;
	PASS_WIN_DATA_T best_vref_win_per_ca[CATRAINING_NUM_LP4];

	memset(&scan_result, 0x0, sizeof(SCAN_WIN_DATA_T));
	memset(best_vref_win_per_ca, 0, sizeof(best_vref_win_per_ca));

	/*  SW variable initialization */
	final_cbt_set_ptr->ca_win_sum_max = 0;

	for (irange = irange_start; irange <= irange_end; irange++) {
#if (CONFIG_EYESCAN_LOG == 1)
		eye_irange = irange;
#endif
		if (cbt_irange_check_lp4(p)) {
			cbt_vref_range_sel = irange;
			cbt_vref_range_begin = 0;
			cbt_vref_range_end = CBT_VREF_MAX;

		#if (FOR_DV_SIMULATION_USED == 0)
			cbt_vref_range_step = 1;
		#else
			cbt_vref_range_step = cbt_vref_range_end;
		#endif

			if (cbt_vref_range_sel == 1)
				cbt_vref_range_begin = CBT_VREF_RANGE1_BEGIN;
		}

		for (vref_level = cbt_vref_range_begin;
			vref_level <= cbt_vref_range_end;
			vref_level += cbt_vref_range_step) {
#if (CONFIG_EYESCAN_LOG == 1)
			eye_vref = vref_level;
			for (ca = 0; ca < CATRAINING_NUM_LP4; ca++) {
				gEyeScan_DelayCellPI[ca] = 0;
				gEyeScan_index[ca] = 0;
			}
#endif

			if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE) {
				cbt_set_vref(p, get_cbt_vref_pin_mux(p, (cbt_vref_range_sel << 6) | vref_level),//cbt_set_vref(p, get_cbt_vref_pin_mux(p, vref_level),
					operating_fsp, 0);
			} else {
				cbt_enter_lp4(p, vref_level);
			}

			/*
			 * Delay CA output delay to do CA training
			 * in order to get the pass window.
			 * moving CA relative to CK and repeating
			 * until CA is centered on the latching edge of CK
			 *
			 * Note  !!!!!!!!!!!!!!!!!!!!!!!
			 * Assume : Leave clk as the init value
			 * and adjust CA delay only can find out
			 * each CA window including of the left boundary.
			 * If NOT, we may need to off-line
			 * adjust SELPH2_TXDLY_CMD
			 */

				/*  SW variable initialization */
				scan_result.finish_count = 0;
				scan_result.ca_win_sum = 0;
				scan_result.ca_center_sum = 0;
				min_winsize = WORD_MAX;
				min_bit = BYTE_MAX;
			for (ca = 0; ca < CATRAINING_NUM_LP4; ca++) {
				scan_result.last_ca_pass[ca] = PASS_RANGE_NA;
				scan_result.first_ca_pass[ca] = PASS_RANGE_NA;
				scan_result.ca_center[ca] = 0;
			}

			cbt_delayscan_lp4(p, &scan_result);

				for (ca = 0; ca < CATRAINING_NUM_LP4; ca++) {
					temp_winsize = scan_result.last_ca_pass[ca] - scan_result.first_ca_pass[ca]
								+ ((scan_result.last_ca_pass[ca] == scan_result.first_ca_pass[ca])? 0 : 1);

					if (temp_winsize < min_winsize) {
						min_bit = ca;
						min_winsize = temp_winsize;
					}
				}

			/* Restore to default value in case CBT fail */
		#if (CBT_MOVE_CA_INSTEAD_OF_CLK)
			/* set CK/CS pi delay to 0. CA to center */
			io_32_write_fld_multi(DRAMC_REG_ADDR
				(DDRPHY_SHU_R0_CA_CMD9),
				p_fld(0x20,
				SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
				p_fld(CLEAR_FLD,
				SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK) |
				p_fld(CLEAR_FLD,
				SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
		#else
			/* set CA/CK/CS pi delay to 0 */
			io_32_write_fld_multi(DRAMC_REG_ADDR
				(DDRPHY_SHU_R0_CA_CMD9),
				p_fld(CLEAR_FLD,
				SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
				p_fld(CLEAR_FLD,
				SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK) |
				p_fld(CLEAR_FLD,
				SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
		#endif

			if ((min_winsize > final_cbt_set_ptr->ca_perbit_win_min_max)
				|| ((min_winsize == final_cbt_set_ptr->ca_perbit_win_min_max)
				&& (scan_result.ca_win_sum > final_cbt_set_ptr->ca_win_sum_max))) {
				show_diag((INFO, "Better CA Vref found %d, Window Min %d >= %d at CA%d, Window Sum %d > %d\n",
					vref_level, min_winsize, final_cbt_set_ptr->ca_perbit_win_min_max, min_bit, scan_result.ca_win_sum, final_cbt_set_ptr->ca_win_sum_max));
				final_cbt_set_ptr->ca_perbit_win_min_max = min_winsize;
				final_cbt_set_ptr->ca_perbit_win_min_max_idx = min_bit;
				final_cbt_set_ptr->ca_win_sum_max = scan_result.ca_win_sum;
				final_cbt_set_ptr->final_vref = vref_level;
				if (cbt_irange_check_lp4(p))
					final_cbt_set_ptr->final_range =
						cbt_vref_range_sel;

				_cbt_scan_lp4(p, final_cbt_set_ptr,
					final_win_per_ca,  scan_result);

				/*save better vref prebit window*/
				memcpy(best_vref_win_per_ca, final_win_per_ca, sizeof(best_vref_win_per_ca));

			} else {
				show_diag((INFO, "CA Vref %d, Window Min %d <= %d at CA%d, Window Sum %d <= %d\n",
					vref_level, min_winsize, final_cbt_set_ptr->ca_perbit_win_min_max, min_bit, scan_result.ca_win_sum, final_cbt_set_ptr->ca_win_sum_max));
			}

			if (gCBT_EYE_Scan_flag == 0) {
				/* max vref found (95%) , early break; */
				if (scan_result.ca_win_sum <
				    (final_cbt_set_ptr->ca_win_sum_max * 95 / 100)) {
					show_msg2((INFO,
						"\nCBT Vref found, "));
					show_msg2((INFO,
						"early break!\n"));
					break;
				}
			}

			if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1) {
				if (operating_fsp == FSP_1)
					cbt_switch_freq(p, CBT_LOW_FREQ);
				cbt_exit(p, operating_fsp, p->frequency);
				delay_us(1);
			}
		}
	}

	memcpy(final_win_per_ca, best_vref_win_per_ca, sizeof(best_vref_win_per_ca));

}

static const unsigned char
	lpddr4_phy2dramc_ca_mapping_emcp[CHANNEL_NUM][CATRAINING_NUM_LP4] = {
	{
		5, 4, 0, 3, 1, 2,
	},
	{
		0, 3, 4, 1, 5, 2,
	},
};

static const unsigned char
	lpddr4_phy2dramc_ca_mapping_dsc[CHANNEL_NUM][CATRAINING_NUM_LP4] = {
	{
		5, 0, 2, 3, 1, 4,
	},
	{
		5, 0, 2, 3, 1, 4,
	},
};

static void cbt_perbit_adjust_lp4(DRAMC_CTX_T *p,
	unsigned char delay_cell_ofst[])
{
	unsigned char *ca_mapping;
	unsigned char rank_bak, rank_idx;

	ca_mapping = (p->is_emcp) ? lpddr4_phy2dramc_ca_mapping_emcp[p->channel] :
				lpddr4_phy2dramc_ca_mapping_dsc[p->channel];

	/* Set Perbit result */
	rank_bak = get_rank(p);

	for (rank_idx = RANK_0; rank_idx < (rank_bak + 1); rank_idx++) {
		set_rank(p, rank_idx);
		if (cbt_enable_perbit_adjust(p)) {
			io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD0),
				p_fld(delay_cell_ofst[ca_mapping[0]],
				SHU_R0_CA_CMD0_RK0_TX_ARCA0_DLY) |
				p_fld(delay_cell_ofst[ca_mapping[1]],
				SHU_R0_CA_CMD0_RK0_TX_ARCA1_DLY) |
				p_fld(delay_cell_ofst[ca_mapping[2]],
				SHU_R0_CA_CMD0_RK0_TX_ARCA2_DLY) |
				p_fld(delay_cell_ofst[ca_mapping[3]],
				SHU_R0_CA_CMD0_RK0_TX_ARCA3_DLY) |
				p_fld(delay_cell_ofst[ca_mapping[4]],
				SHU_R0_CA_CMD0_RK0_TX_ARCA4_DLY) |
				p_fld(delay_cell_ofst[ca_mapping[5]],
				SHU_R0_CA_CMD0_RK0_TX_ARCA5_DLY));
		}
	}
	set_rank(p, rank_bak);
}
DRAM_STATUS_T cbt_lp4(DRAMC_CTX_T *p)
{
	PASS_WIN_DATA_T final_win_per_ca[CATRAINING_NUM_LP4];
	FINAL_WIN_DATA_T final_cbt_set;
	unsigned int ca;
	unsigned char operating_fsp;
	unsigned short operation_frequency;
	unsigned char irange, irange_start, irange_end;
	signed char final_caclk = 0;
	unsigned char delay_cell_ofst[CATRAINING_NUM_LP4] = {0};
	unsigned int reg_backup_address[] = {
		(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL)),
		(DRAMC_REG_ADDR(DRAMC_REG_STBCAL)),
		(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL)),
		(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV)),
		(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0)),
		(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL)),
		(DRAMC_REG_ADDR(DDRPHY_B1_DQ2)),
	};
#if (CONFIG_EYESCAN_LOG == 1)
	unsigned int vref_idx, ii;

	if (is_lp4_family(p))
	{
		for(vref_idx=0; vref_idx<VREF_TOTAL_NUM_WITH_RANGE;vref_idx++)
		{
			for (ca = 0; ca < CATRAINING_NUM_LP4; ca++)
			{
				for(ii=0; ii<EYESCAN_BROKEN_NUM; ii++)
				{
					gEyeScan_Min[vref_idx][ca][ii] = EYESCAN_DATA_INVALID;
					gEyeScan_Max[vref_idx][ca][ii] = EYESCAN_DATA_INVALID;
				}
			}
		}
	}
#endif

	show_msg_with_timestamp((INFO, "start CBT\n"));

	memset(&final_win_per_ca, 0x0, sizeof(PASS_WIN_DATA_T));
	memset(&final_cbt_set, 0x0, sizeof(FINAL_WIN_DATA_T));

	/* Back up dramC register */
	dramc_backup_registers(p, reg_backup_address,
		sizeof(reg_backup_address) / sizeof(unsigned int));

#if CBT_WORKAROUND_B0_B1_SWAP
	io_32_write_fld_align(DRAMC_REG_ADDR(DDRPHY_B1_DQ2),
		SET_FLD, B1_DQ2_RG_TX_ARDQ_OE_DIS_B1);
#endif

	/* When doing CA training, should make sure that
	 * auto refresh is disable.
	 */
	auto_refresh_switch(p, DISABLE);

#if (CBT_K_RANK1_USE_METHOD == 1)
	if (p->rank == RANK_1) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), p->rank,
			MRS_MRSRK);
		/* use other rank's setting */
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG),
			p->rank, RKCFG_TXRANK);
		/*
		* TXRANKFIX should be write
		* after TXRANK or the rank will be fix at rank 1
		*/
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG),
			SET_FLD, RKCFG_TXRANKFIX);
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MPC_OPTION),
			CLEAR_FLD, MPC_OPTION_MPCRKEN);
	}
#else
	if (p->rank == RANK_1) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), RANK_0,
			MRS_MRSRK);
		dramc_rank_swap(p, RANK_1);
	}
#endif

	/*  SW variable initialization */
	final_cbt_set.final_vref =
		dram_mr.mr12_value[p->channel][p->rank][p->dram_fsp] & 0x3f;
	irange_start = irange_end = 1;
	operating_fsp = p->dram_fsp;
	operation_frequency = p->frequency;

	print_calibration_basic_info(p);
	print_calibration_basic_info_diag(p);

	/* set CBT range, verify range and setp */
	cbt_vref_range_sel = 1;	/* MR12,OP[6] */
	cbt_vref_range_step = 2;

	if (p->enable_cbt_scan_vref == DISABLE) {
		cbt_vref_range_begin =
			(dram_mr.mr12_value[p->channel][p->rank][p->dram_fsp]
			& 0x3f);
		cbt_vref_range_end = cbt_vref_range_begin;
	} else {
		if (p->dram_type == TYPE_LPDDR4) {
#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION)
			if (p->femmc_Ready==1) {
				/* even for Fast K, we'll still do calibration at the final Vref */
				cbt_vref_range_begin = p->pSavetimeData->u1CBTVref_Save[p->channel][p->rank];
				cbt_vref_range_end = cbt_vref_range_begin;
			} else
#endif
			{
				/* range 1, 300/1100(VDDQ) = 27.2% */
				cbt_vref_range_begin = CBT_VREF_BEGIN;
				cbt_vref_range_end = CBT_VREF_END;
			}
		} else {
			/* range 1, 290/600(VDDQ)=48.3% */
			cbt_vref_range_begin = CBT_VREF_BEGIN_X;
			cbt_vref_range_end = CBT_VREF_END_X;
		}
	}

	show_msg((INFO, "=== vref scan from %d to %d\n",
		cbt_vref_range_begin, cbt_vref_range_end));

	if (p->dram_cbt_mode[p->rank] == CBT_NORMAL_MODE)
		cbt_enter_lp4(p, 0);

	if (cbt_irange_check_lp4(p)) {
		irange_start = 0;
		irange_end = 1;
	}

	cbt_scan_lp4(p, operating_fsp, irange, irange_start, irange_end,
		&final_cbt_set, final_win_per_ca);
#if 0
	if (cbt_enable_perbit_adjust(p)) {
		cbt_cal_delay_cell_perbit(p, final_cbt_set.final_ca_clk,
			delay_cell_ofst, final_win_per_ca);
	}
#endif
	if (cbt_irange_check_lp4(p))
		cbt_vref_range_sel = final_cbt_set.final_range;

	for (ca = 0; ca < CATRAINING_NUM_LP4; ca++) {
		first_ca_pass[p->rank][ca] = final_win_per_ca[ca].first_pass;
		last_ca_pass[p->rank][ca] = final_win_per_ca[ca].last_pass;
		//show_msg((INFO, "[CA %d] Center %d (%d~%d) winsize %d\n",
			//ca, final_win_per_ca[ca].win_center,
			//final_win_per_ca[ca].first_pass,
			//final_win_per_ca[ca].last_pass,
			//final_win_per_ca[ca].win_size));
	}
	cbt_pos_cal(p, &final_caclk, delay_cell_ofst);

 #if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_VREF_CAL)
	if(p->femmc_Ready==1)
	{
		final_cbt_set.final_vref = p->pSavetimeData->u1CBTVref_Save[p->channel][p->rank];
	}
#endif

	/*
	 * Set Vref after trainging, normol mode go DQ pin set vref,
	 * don't set final_set_flag here
	 */
	if (p->dram_cbt_mode[p->rank] == CBT_BYTE_MODE1) {
		if (p->dram_fsp == FSP_1)
			cbt_switch_freq(p, CBT_LOW_FREQ);
		cbt_set_fsp(p, operating_fsp, 1);
		cbt_set_vref(p, get_cbt_vref_pin_mux(p, final_cbt_set.final_vref),
			operating_fsp, 1);
	} else {
		cbt_set_vref(p, get_cbt_vref_pin_mux(p, final_cbt_set.final_vref),
			operating_fsp, 0);
	}

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	if(p->femmc_Ready==0)
	{
		p->pSavetimeData->u1CBTVref_Save[p->channel][p->rank]= final_cbt_set.final_vref;
	}
#endif

	show_diag((INFO, "[CBT_LP4] Vref(ca) range %d: %d\n",
		cbt_vref_range_sel, final_cbt_set.final_vref));

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_CBT)
	if(p->femmc_Ready==1)
	{
		ca_train_clk_delay[p->channel][p->rank]=p->pSavetimeData->u1CBTClkDelay_Save[p->channel][p->rank];
		ca_train_cmd_delay[p->channel][p->rank]=p->pSavetimeData->u1CBTCmdDelay_Save[p->channel][p->rank];
		final_cbt_set.final_ca_clk = ca_train_cmd_delay[p->channel][p->rank];
        set_calibration_result(p, DRAM_CALIBRATION_CA_TRAIN, DRAM_OK);
	}
	else
#endif
	{
		/* Set CA_PI_Delay after trainging */
		if (final_cbt_set.final_ca_clk < 0) { /* Set CLK delay */
			ca_train_clk_delay[p->channel][p->rank] = 0;
			show_err2("Warning!! Clk Dly = %d, adjust to 0 !!\n",
				-final_cbt_set.final_ca_clk);
			ca_train_cmd_delay[p->channel][p->rank] = 0;
		} else { /*  Set CA output delay */
			ca_train_clk_delay[p->channel][p->rank] = 0;
			ca_train_cmd_delay[p->channel][p->rank] =
				final_cbt_set.final_ca_clk;
		}
	}

	/*CA perbit window @final vref*/
	for (ca = 0; ca < CATRAINING_NUM_LP4; ca++) {
		show_diag((INFO, "[CH%d][RK%d][%d][CBT]CA%d (%d~%d) %d %d\n",
			p->channel,
			p->rank,
			p->frequency*2,
			ca,
			final_win_per_ca[ca].first_pass,
			final_win_per_ca[ca].last_pass,
			final_win_per_ca[ca].win_center,
			final_win_per_ca[ca].win_size));
	}
	show_msg0((INFO, "\n[CH%d][RK%d][%d][CBT] Best CA Vref %d, Window Min %d at CA%d, Window Sum %d\n",
		 p->channel, p->rank, p->frequency*2,
		final_cbt_set.final_vref,
		final_cbt_set.ca_perbit_win_min_max,
		final_cbt_set.ca_perbit_win_min_max_idx,
		final_cbt_set.ca_win_sum_max));

	cbt_set_ca_clk_result(p, final_caclk);
	cbt_perbit_adjust_lp4(p, delay_cell_ofst);
#if ((CONFIG_EYESCAN_LOG == 1) || (CONFIG_FOR_HQA_TEST_USED == 1))
		gFinalCBTVrefDQ[p->channel][p->rank] = final_cbt_set.final_vref;
#endif

	show_diag((INFO, "Average CA Dly: %d\n", final_cbt_set.final_ca_clk));

	cbt_leave_lp4(p, operation_frequency, operating_fsp,
		final_cbt_set.final_vref);

	/* Restore setting registers */
	dramc_restore_registers(p, reg_backup_address,
		sizeof(reg_backup_address) / sizeof(unsigned int));

	return DRAM_OK;
}
#endif /* SIMUILATION_LP4_CBT */

#if (SUPPORT_TYPE_LPDDR3 && SIMULATION_CBT)
static unsigned int cbt_delay_compare_lp3(DRAMC_CTX_T *p, signed int delay)
{
	unsigned int result = 0;

	if (delay < 0) { /* Set CLK delay */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD9),
			p_fld(CLEAR_FLD,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
			p_fld(-delay,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK) |
			p_fld(-delay,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B1_DQ7),
			p_fld(0x0, SHU_R0_B1_DQ7_RK0_ARPI_PBYTE_B1) |
			p_fld(0x0, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
			p_fld(0x0, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	} else { /* Set CA output delay */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD9),
			p_fld(delay,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
			p_fld(CLEAR_FLD,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK) |
			p_fld(CLEAR_FLD,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B1_DQ7),
			p_fld(delay, SHU_R0_B1_DQ7_RK0_ARPI_PBYTE_B1) |
			p_fld(delay, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
			p_fld(delay, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	}

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_CATRAINING1), SET_FLD,
		CATRAINING1_CATRAINEN);

	delay_us(1);

	/* Get CA training compare result */
	result = io32_read_4b(DDRPHY_MISC_DQO1) & 0xffff;
	result |=
		io32_read_4b(DDRPHY_MISC_DQO1 + (1 << POS_BANK_NUM)) &
		0xffff0000;

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_CATRAINING1),
		CLEAR_FLD, CATRAINING1_CATRAINEN);

	return result; /* return pattern compare result */
}

void dramc_cmd_bus_training_post_process_lp3(DRAMC_CTX_T *p)
{
	signed int cs_Final_clk_elay, cs_Final_cmd_delay, cs_finalcs_delay;
	unsigned char backup_rank, irank;

	/* CBT Rank0/1 must set Clk/CA/CS the same */

	show_msg((INFO,
		"[dramc_cmd_bus_training_post_process] p->frequency=%d\n",
		p->frequency));

	backup_rank = get_rank(p);

	cs_Final_clk_elay =
		(ca_train_clk_delay[p->channel][RANK_0] +
		ca_train_clk_delay[p->channel][RANK_1]) / 2;
		ca_train_clk_delay[p->channel][RANK_0] = cs_Final_clk_elay;
		ca_train_clk_delay[p->channel][RANK_1] = cs_Final_clk_elay;

	cs_Final_cmd_delay =
		(ca_train_cmd_delay[p->channel][RANK_0] +
		ca_train_cmd_delay[p->channel][RANK_1]) / 2;
		ca_train_cmd_delay[p->channel][RANK_0] = cs_Final_cmd_delay;
		ca_train_cmd_delay[p->channel][RANK_1] = cs_Final_cmd_delay;

	cs_finalcs_delay =
		(ca_train_cs_delay[p->channel][RANK_0] +
		ca_train_cs_delay[p->channel][RANK_1]) / 2;
		ca_train_cs_delay[p->channel][RANK_0] = cs_finalcs_delay;
		ca_train_cs_delay[p->channel][RANK_1] = cs_finalcs_delay;

	for (irank = RANK_0; irank <= RANK_1; irank++) {
		set_rank(p, irank);
		io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD9),
			p_fld(cs_Final_clk_elay,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK) |
			p_fld(cs_Final_cmd_delay,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
			p_fld(cs_finalcs_delay,
			SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
		io_32_write_fld_multi(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B1_DQ7),
			p_fld(cs_Final_cmd_delay, SHU_R0_B1_DQ7_RK0_ARPI_PBYTE_B1) |
			p_fld(cs_Final_cmd_delay, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));
	}

	show_msg((INFO, "Clk Dly = %d\nCmd Dly = %d\nCS Dly = %d\n",
		cs_Final_clk_elay, cs_Final_cmd_delay, cs_finalcs_delay));

	set_rank(p, backup_rank);

}

static void cbt_set_cbt_result_lp3(DRAMC_CTX_T *p)
{
	/*
	* Set CA_PI_Delay after trainging
	* p->rank = RANK_0, save to Reg Rank0 and Rank1,
	* p->rank = RANK_1, save to Reg Rank1
	*/
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD9),
		p_fld(ca_train_cmd_delay[p->channel][p->rank],
		SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
		p_fld(ca_train_clk_delay[p->channel][p->rank],
		SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK) |
		p_fld(ca_train_cs_delay[p->channel][p->rank],
		SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B1_DQ7),
		p_fld(ca_train_cmd_delay[p->channel][p->rank],
		SHU_R0_B1_DQ7_RK0_ARPI_PBYTE_B1) |
		p_fld(ca_train_cmd_delay[p->channel][p->rank],
		SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
		p_fld(ca_train_cmd_delay[p->channel][p->rank],
		SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));


	show_msg((INFO,
		"[CBTSetCACLKResult] CA Dly = %d, CLK dly =%d, CS dly = %d\n",
		ca_train_cmd_delay[p->channel][p->rank],
		ca_train_clk_delay[p->channel][p->rank],
		ca_train_cs_delay[p->channel][p->rank]));
}

static void cbt_enter_lp3(DRAMC_CTX_T *p, unsigned char cal_bit4_9,
	unsigned int ca_pattern)
{
	if(p->rank == RANK_1) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), p->rank, MRS_MRSRK);
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_RKCFG),
			p_fld(0, RKCFG_CS0FORCE) | p_fld(0, RKCFG_CS2RANK));
	}
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_CATRAINING1), 0,
		CATRAINING1_CATRAINCSEXT);

	cke_fix_on_off(p, p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	/* Let CA be stable prior to issue CS */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_CATRAINING1),
		1, CATRAINING1_CATRAINMRS);

	if (cal_bit4_9) {
		show_msg2((INFO, "[CBT] Calibration BIT4, BIT9\n"));

		/* Write to MR48 */
		dramc_mode_reg_write_by_rank(p, p->rank, 48, 0xc0);
	} else {
		show_msg2((INFO, "[CBT] Calibration BIT1~3, BIT5~8\n"));

		/* Write to MR41 */
		dramc_mode_reg_write_by_rank(p, p->rank, 41, 0xa4);
	}

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_CATRAINING1),
		0, CATRAINING1_CATRAINMRS);

	delay_us(1);

	/* CKE low */
	cke_fix_on_off(p, p->rank, CKE_FIXOFF, CKE_WRITE_TO_ONE_CHANNEL);

	io32_write_4b(DRAMC_REG_ADDR(DRAMC_REG_CATRAINING2), ca_pattern);

	delay_us(1);

	if(p->rank == RANK_1) {
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_RKCFG),
			p_fld(1, RKCFG_CS0FORCE) | p_fld(1, RKCFG_CS2RANK));
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG), 1, RKCFG_TXRANK);
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG), 1, RKCFG_TXRANKFIX);
	}

	return DRAM_OK;
}


static void cbt_leave_lp3(DRAMC_CTX_T *p)
{
	if(p->rank == RANK_1) {
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_MRS), RANK_0, MRS_MRSRK);
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_RKCFG),
			p_fld(0, RKCFG_CS0FORCE) | p_fld(0, RKCFG_CS2RANK));
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG), 0, RKCFG_TXRANK);
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_RKCFG), 0, RKCFG_TXRANKFIX);
	}

	cke_fix_on_off(p, p->rank, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);

	delay_us(1);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_CATRAINING1), 0,
		CATRAINING1_CATRAINCSEXT);

	/* Hold the CA bus stable for at least one cycle. */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_CATRAINING1), 1,
		CATRAINING1_CATRAINMRS);

	dramc_mode_reg_write_by_rank(p, p->rank, 42, 0xa8);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_CATRAINING1), 0,
		CATRAINING1_CATRAINMRS);

	/* CS extent disable */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_CATRAINING1), 0,
		CATRAINING1_CATRAINCSEXT);
}

static void cbt_delayscan_lp3(DRAMC_CTX_T *p, SCAN_WIN_DATA_T *sacn_ptr,
	signed short delay, unsigned int compare_result,
	unsigned char cal_bit4_9, unsigned int golden_pattern)
{
	unsigned char *lpddr_phy_mapping;
	unsigned short rst_rising, rst_falling;
	unsigned short pat_rising, pat_falling;
	unsigned char ca;
	unsigned char ca_idx;
	unsigned char fail;

	lpddr_phy_mapping = lpddr3_phy_mapping_pop;

	for (ca = 0; ca < CATRAINING_NUM_LP3; ca++) {
		if (cal_bit4_9 && (ca != 4) && (ca != 9)) {
		#if CALIBRATION_LOG
			show_msg2((INFO, "-"));
		#endif
			continue;
		}

		if (!cal_bit4_9 && ((ca == 4) || (ca == 9))) {
		#if CALIBRATION_LOG
			show_msg2((INFO, "-"));
		#endif
			continue;
		}

		/* Get the result of each CA bit */
		switch (ca) {
		case 4:
			rst_rising = !!(compare_result &
				(1 << lpddr_phy_mapping[0]));
			rst_falling = !!(compare_result &
				(1 << lpddr_phy_mapping[1]));
			break;
		case 9:
			rst_rising = !!(compare_result &
				(1 << lpddr_phy_mapping[8]));
			rst_falling = !!(compare_result &
				(1 << lpddr_phy_mapping[9]));
			break;
		default:
			ca_idx = (ca < 4) ? (ca << 1) : ((ca - 1) << 1);

			rst_rising = !!(compare_result &
				(1 << lpddr_phy_mapping[ca_idx]));
			rst_falling = !!(compare_result
				& (1 << lpddr_phy_mapping[ca_idx+1]));
			break;
		}

		/* Value of golden pattern */
		pat_rising = !!(golden_pattern & (1 << (ca << 1)));
		pat_falling = !!(golden_pattern & (1 << ((ca << 1) + 1)));

		fail = 1;
		if ((pat_rising == rst_rising) &&
			(pat_falling == rst_falling)) {
			fail = 0;
		}

	#if CALIBRATION_LOG
		if (fail == 0)
			show_msg2((INFO, "o"));
		else
			show_msg2((INFO, "x"));
	#endif
		if (sacn_ptr->first_ca_pass[ca] == PASS_RANGE_NA) {
			if (fail == 0)
				sacn_ptr->first_ca_pass[ca] = delay;
		} else if (sacn_ptr->last_ca_pass[ca] == PASS_RANGE_NA) {
			if (fail == 1) {
				if ((delay - sacn_ptr->first_ca_pass[ca])
					< 8) {
					sacn_ptr->first_ca_pass[ca] =
						PASS_RANGE_NA;
					continue;
				}
				sacn_ptr->last_ca_pass[ca] = (delay - 1);
			} else if (delay == MAX_CA_PI_DELAY) {
				sacn_ptr->last_ca_pass[ca] = delay;
			}

			if (sacn_ptr->last_ca_pass[ca] != PASS_RANGE_NA) {
				sacn_ptr->finish_count++;
				sacn_ptr->ca_win_sum +=
					(sacn_ptr->last_ca_pass[ca] -
					sacn_ptr->first_ca_pass[ca]);
				sacn_ptr->ca_center[ca] =
					(sacn_ptr->last_ca_pass[ca] +
					sacn_ptr->first_ca_pass[ca]) >> 1;
				sacn_ptr->ca_center_sum +=
					sacn_ptr->ca_center[ca];
			}
		}
	}

#if CALIBRATION_LOG
	show_msg2((INFO, " [MSB]\n"));
#endif
}

static void _cbt_scan_lp3(DRAMC_CTX_T *p, FINAL_WIN_DATA_T *final_cbt_set_ptr,
	PASS_WIN_DATA_T final_win_per_ca[], SCAN_WIN_DATA_T *scan_result)
{
	signed int ca_final_center[CATRAINING_NUM_LP3] = { 0 };
	signed int ck_min = 1000;
	unsigned int ca;

	memset(ca_final_center, 0, sizeof(ca_final_center));

	/* cc notes: For High Freq, may need per-bit ajust. Currently
	 * not implemented since the highest freqency is 1866.
	 * Instead, using the average value.
	 */
	if (cbt_enable_perbit_adjust(p)) {
		ck_min = 1000;
		for (ca = 0; ca < CATRAINING_NUM_LP3; ca++) {
			ca_final_center[ca] = scan_result->ca_center[ca];
			if (ca_final_center[ca] < ck_min)
				ck_min = ca_final_center[ca];
		}

		if (ck_min < 0) {
			show_msg((INFO,
				"Warning!! smallest%s%d < 0, %s !!\n",
				"CA min center = ", ck_min,
				"then adjust to 0"));
			for (ca = 0; ca < CATRAINING_NUM_LP3; ca++) {
				if (ca_final_center[ca] < 0)
					ca_final_center[ca] = 0;
			}
			ck_min = 0;
		}

		final_cbt_set_ptr->final_ca_clk = ck_min;
	} else {
		final_cbt_set_ptr->final_ca_clk = scan_result->ca_center_sum
			/ CATRAINING_NUM_LP3;
	}

	for (ca = 0; ca < CATRAINING_NUM_LP3; ca++) {
		final_win_per_ca[ca].first_pass =
			scan_result->first_ca_pass[ca];
		final_win_per_ca[ca].last_pass =
			scan_result->last_ca_pass[ca];
		final_win_per_ca[ca].win_center =
			scan_result->ca_center[ca];
		final_win_per_ca[ca].win_size =
			(scan_result->last_ca_pass[ca] -
			scan_result->first_ca_pass[ca]) +
			(scan_result->last_ca_pass[ca] ==
			scan_result->first_ca_pass[ca] ? 0 : 1);
	}
}

static void cbt_perbit_adjust_lp3(DRAMC_CTX_T *p,
	unsigned char delay_cell_ofst[])
{
	/* Set per-bit adjust result */
	if (cbt_enable_perbit_adjust(p)) {
		/* CA Pinmux:
		 * CA9->ARDQ1_B1
		 * CA8->ARDQ0_B1
		 * CA7->ARDQS0_B1
		 * CA6->ARCA9
		 * CA5->ARDQ5_B1
		 * CA4->ARDQ7_B1
		 * CA3->ARDQ2_B1
		 * CA2->ARDQ6_B1
		 * CA1->ARCA5
		 * CA0->ARDQ4_B1
		 */
		if (p->is_emcp) {
			io_32_write_fld_align(DDRPHY_SHU_R0_CA_CMD0,
				delay_cell_ofst[1], SHU_R0_CA_CMD0_RK0_TX_ARCA5_DLY);
			io_32_write_fld_align(DDRPHY_SHU_R0_CA_CMD10,
				delay_cell_ofst[6], SHU_R0_CA_CMD10_RK0_TX_ARCA9_DLY);
			io_32_write_fld_multi(DDRPHY_SHU_R0_B1_DQ0,
				p_fld(delay_cell_ofst[8],
					SHU_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1) |
				p_fld(delay_cell_ofst[9],
					SHU_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1) |
				p_fld(delay_cell_ofst[3],
					SHU_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1) |
				p_fld(delay_cell_ofst[0],
					SHU_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1) |
				p_fld(delay_cell_ofst[5],
					SHU_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1) |
				p_fld(delay_cell_ofst[2],
					SHU_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1) |
				p_fld(delay_cell_ofst[4],
					SHU_R0_B1_DQ0_RK0_TX_ARDQ7_DLY_B1));
			io_32_write_fld_align(DDRPHY_SHU_R0_B1_DQ1,
				delay_cell_ofst[7], SHU_R0_B1_DQ1_RK0_TX_ARDQS0_DLY_B1);
		}else {
			io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD10,
				p_fld(delay_cell_ofst[6], SHU_R0_CA_CMD10_RK0_TX_ARCA9_DLY) |
				p_fld(delay_cell_ofst[4], SHU_R0_CA_CMD10_RK0_TX_ARCA10_DLY));
			io_32_write_fld_multi(DDRPHY_SHU_R0_B1_DQ0,
				p_fld(delay_cell_ofst[5],
					SHU_R0_B1_DQ0_RK0_TX_ARDQ0_DLY_B1) |
				p_fld(delay_cell_ofst[7],
					SHU_R0_B1_DQ0_RK0_TX_ARDQ1_DLY_B1) |
				p_fld(delay_cell_ofst[1],
					SHU_R0_B1_DQ0_RK0_TX_ARDQ2_DLY_B1) |
				p_fld(delay_cell_ofst[0],
					SHU_R0_B1_DQ0_RK0_TX_ARDQ4_DLY_B1) |
				p_fld(delay_cell_ofst[9],
					SHU_R0_B1_DQ0_RK0_TX_ARDQ5_DLY_B1) |
				p_fld(delay_cell_ofst[2],
					SHU_R0_B1_DQ0_RK0_TX_ARDQ6_DLY_B1));
			io_32_write_fld_multi(DDRPHY_SHU_R0_B1_DQ1,
				p_fld(delay_cell_ofst[3],
					SHU_R0_B1_DQ1_RK0_TX_ARDQM0_DLY_B1) |
				p_fld(delay_cell_ofst[8],
					SHU_R0_B1_DQ1_RK0_TX_ARDQS0_DLY_B1));
		}
#if 0
		io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD0,
			p_fld(delay_cell_ofst[3],
				SHU_R0_CA_CMD0_RK0_TX_ARCA0_DLY) |
			p_fld(delay_cell_ofst[6],
				SHU_R0_CA_CMD0_RK0_TX_ARCA4_DLY) |
			p_fld(delay_cell_ofst[1],
				SHU_R0_CA_CMD0_RK0_TX_ARCA5_DLY) |
			p_fld(delay_cell_ofst[8],
				SHU_R0_CA_CMD0_RK0_TX_ARCA6_DLY));
		io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD1,
			p_fld(delay_cell_ofst[5],
				SHU_R0_CA_CMD1_RK0_TX_ARCMD0_DLY) |
			p_fld(delay_cell_ofst[0],
				SHU_R0_CA_CMD1_RK0_TX_ARCMD1_DLY));
		io_32_write_fld_multi(DDRPHY_SHU_R0_CA_CMD0 +
				(1 << POS_BANK_NUM),
			p_fld(delay_cell_ofst[7],
				SHU_R0_CA_CMD0_RK0_TX_ARCA3_DLY) |
			p_fld(delay_cell_ofst[9],
				SHU_R0_CA_CMD0_RK0_TX_ARCA6_DLY) |
			p_fld(delay_cell_ofst[2],
				SHU_R0_CA_CMD0_RK0_TX_ARCA7_DLY));
		io_32_write_fld_align(DDRPHY_SHU_R0_CA_CMD1 +
				(1 << POS_BANK_NUM),
			delay_cell_ofst[4], SHU_R0_CA_CMD1_RK0_TX_ARCMD0_DLY);
#endif
	}
}

static void cbt_scan_lp3(DRAMC_CTX_T *p,SCAN_WIN_DATA_T *scan_ptr,
	unsigned char cal_bit4_9, unsigned int ca_golden_pattern)
{
	unsigned int ca;
	signed int delay;
	unsigned int delay_step;
	unsigned int result;
	unsigned int finish_count;

#if FOR_DV_SIMULATION_USED
	delay_step = 4;
#else
	delay_step = 1;
#endif

	/*  SW variable initialization */
	finish_count = scan_ptr->finish_count;
	for (delay = -(MAX_CLK_PI_DELAY/2); delay < MAX_CA_PI_DELAY;
		delay += delay_step) {

		result = cbt_delay_compare_lp3(p, delay);

		show_msg2((INFO, "%2d, 0x%08x | ", delay, result));

		cbt_delayscan_lp3(p, scan_ptr, delay, result,
			cal_bit4_9, ca_golden_pattern);

		if ((cal_bit4_9 &&
			(scan_ptr->finish_count - finish_count == 2)) ||
			(!cal_bit4_9 &&
			(scan_ptr->finish_count - finish_count == 8))) {
			show_msg((INFO, "[CBT] Finish %d bits. Early break\n",
				scan_ptr->finish_count - finish_count));
			break;
		}
		delay_us(1); /* for tCACD consern */
	}
}

DRAM_STATUS_T cbt_lp3(DRAMC_CTX_T *p)
{
	PASS_WIN_DATA_T final_win_per_ca[CATRAINING_NUM_LP3];
	FINAL_WIN_DATA_T final_cbt_set;
	SCAN_WIN_DATA_T scan_cbt_win;
	unsigned int ca;
	unsigned char cal_bit4_9;
	unsigned int ca_golden_pattern;
	unsigned char delay_cell_ofst[CATRAINING_NUM_LP3];

	unsigned int reg_backup_address[] = {
		(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL)),
		(DRAMC_REG_ADDR(DRAMC_REG_STBCAL)),
		(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL)),
		(DRAMC_REG_ADDR(DRAMC_REG_WRITE_LEV)),
		(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM3)),
		(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM4)),
		(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0)),
		(DRAMC_REG_ADDR(DRAMC_REG_SPCMDCTRL)),
	};

	show_msg_with_timestamp((INFO, "start CBT\n"));

	memset(&final_win_per_ca, 0x0,
		sizeof(PASS_WIN_DATA_T) * CATRAINING_NUM_LP3);
	memset(&final_cbt_set, 0x0, sizeof(FINAL_WIN_DATA_T));
	memset(&scan_cbt_win, 0x0, sizeof(SCAN_WIN_DATA_T));
	ca_golden_pattern = CA_GOLDEN_PATTERN;

	for (ca = 0; ca < CATRAINING_NUM_LP3; ca++) {
		scan_cbt_win.last_ca_pass[ca] = PASS_RANGE_NA;
		scan_cbt_win.first_ca_pass[ca] = PASS_RANGE_NA;
	}

	/* Back up dramC register */
	dramc_backup_registers(p, reg_backup_address,
		sizeof(reg_backup_address) / sizeof(unsigned int));

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_REFCTRL0), 1,
		REFCTRL0_REFDIS);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM3), 0,
		SHU_ACTIM3_REFCNT);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_SHU_ACTIM4), 0,
		SHU_ACTIM4_REFCNT_FR_CLK);

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 1,
		DRAMC_PD_CTRL_MIOCKCTRLOFF);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), 0,
		DRAMC_PD_CTRL_PHYCLKDYNGEN);

	o1_path_on_off(p, 1);

	/*
	 * When doing CA training, should make sure that auto refresh is disable
	 */
	auto_refresh_switch(p, DISABLE);

	/* According to Spec., Shall first cal bits excluding bit4 and bit9,
	 * then change DQ mapping and cal bit4 and bit9.
	 */
	cal_bit4_9 = FALSE;
	cbt_enter_lp3(p, cal_bit4_9, ca_golden_pattern);
	cbt_scan_lp3(p, &scan_cbt_win, cal_bit4_9, ca_golden_pattern);

	cal_bit4_9 = TRUE;
	cbt_enter_lp3(p, cal_bit4_9, ca_golden_pattern);
	cbt_scan_lp3(p, &scan_cbt_win, cal_bit4_9, ca_golden_pattern);

	/* set CA/CK/CS pi delay to 0 */
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_SHU_R0_CA_CMD9),
		p_fld(CLEAR_FLD, SHU_R0_CA_CMD9_RG_RK0_ARPI_CMD) |
		p_fld(CLEAR_FLD, SHU_R0_CA_CMD9_RG_RK0_ARPI_CLK) |
		p_fld(CLEAR_FLD, SHU_R0_CA_CMD9_RG_RK0_ARPI_CS));
	io_32_write_fld_multi_all(DRAMC_REG_ADDR(DDRPHY_SHU_R0_B1_DQ7),
		p_fld(CLEAR_FLD, SHU_R0_B1_DQ7_RK0_ARPI_PBYTE_B1) |
		p_fld(CLEAR_FLD, SHU_R0_B1_DQ7_RK0_ARPI_DQM_B1) |
		p_fld(CLEAR_FLD, SHU_R0_B1_DQ7_RK0_ARPI_DQ_B1));

	_cbt_scan_lp3(p, &final_cbt_set,
		final_win_per_ca, &scan_cbt_win);

	for (ca = 0; ca < CATRAINING_NUM_LP3; ca++) {
		show_msg((INFO, "[CA %d] Center %d (%d~%d) winsize %d\n",
			ca, final_win_per_ca[ca].win_center,
			final_win_per_ca[ca].first_pass,
			final_win_per_ca[ca].last_pass,
			final_win_per_ca[ca].win_size));
	}

	/* Used to do per-bit adjust */
	if (cbt_enable_perbit_adjust(p)) {
		cbt_cal_delay_cell_perbit(p, final_cbt_set.final_ca_clk,
			delay_cell_ofst, final_win_per_ca);
	}

	/* Set CA_PI_Delay after trainging */
	if (final_cbt_set.final_ca_clk < 0) {
		/* All adjust to 0 to avoid moving CLK.
		 * May lose some margine.
		 * Shall be evaluated if patch is needed after
		 * IC back.
		 */
		ca_train_clk_delay[p->channel][p->rank] = 0;
		ca_train_cs_delay[p->channel][p->rank] = 0;
		show_err2("Warning!! Clk Dly = %d, adjust to 0 !!\n",
			-final_cbt_set.final_ca_clk);
		ca_train_cmd_delay[p->channel][p->rank] = 0;
	} else {
		/*  Set CA output delay */
		ca_train_clk_delay[p->channel][p->rank] = 0;
		ca_train_cs_delay[p->channel][p->rank] = 0;
		ca_train_cmd_delay[p->channel][p->rank] =
			final_cbt_set.final_ca_clk;
	}

	cbt_set_cbt_result_lp3(p);
	cbt_perbit_adjust_lp3(p, delay_cell_ofst);

	show_msg2((INFO, "Average CA Dly: %d\n", final_cbt_set.final_ca_clk));

	cbt_leave_lp3(p);
	o1_path_on_off(p, 0);

	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_CKECTRL),
		p_fld(0, CKECTRL_CKEFIXOFF) |
		p_fld(0, CKECTRL_CKEFIXON));

	/* Restore setting registers */
	dramc_restore_registers(p, reg_backup_address,
		sizeof(reg_backup_address) / sizeof(unsigned int));

	return DRAM_OK;
}
#endif /* SIMUILATION_LP3_CBT */

#if SIMULATION_CBT
DRAM_STATUS_T cmd_bus_training(DRAMC_CTX_T *p)
{
	if (is_lp4_family(p)) {
	#if SUPPORT_TYPE_LPDDR4
		return cbt_lp4(p);
	#endif
	} else if (p->dram_type == TYPE_LPDDR3) {
	#if SUPPORT_TYPE_LPDDR3
		return cbt_lp3(p);
	#endif
	}

	return DRAM_OK;
}
#endif
