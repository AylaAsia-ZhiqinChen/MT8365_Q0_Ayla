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

/** @file dramc_engine_api.c
 *  Basic DRAMC API implementation
 */

/* Include files */
#include "dramc_common.h"
#include "x_hal_io.h"

/* Global variables */
static unsigned int reg0d0h[CHANNEL_NUM];

void dramc_set_rank_engine2(DRAMC_CTX_T *p, unsigned char rank_sel)
{
	/* LPDDR2_3_ADRDECEN_TARKMODE =0, always rank0 */
	/* ADRDECEN_TARKMODE: rank input selection */
	/*1'b1 select CTO_AGENT1_RANK, 1'b0 rank by address decode*/
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DRAMCTRL), SET_FLD,
		DRAMCTRL_ADRDECEN_TARKMODE);

	/* DUMMY_TESTAGENTRKSEL =0, select rank according
	 * to CATRAIN_TESTAGENTRK
	 * TESTAGENTRKSEL: Test agent access rank mode selection
	 * 2'b00: rank selection by TESTAGENTRK,
	 * 2'b01: rank selection by CTO_AGENT_1_BK_ADR[0]
	 * 2'b10: rank selection by CTO_AGENT1_COL_ADR[3],
	 * 2'b11: rank selection by CTO_AGENT1_COL_ADR[4]
	 */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_TEST2_4), CLEAR_FLD,
		TEST2_4_TESTAGENTRKSEL);

	/*
	 * CATRAIN_TESTAGENTRK = rank_sel
	 * TESTAGENTRK: Specify test agent rank
	 * 2'b00 rank 0, 2'b01 rank 1, 2'b10 rank 2
	 */
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_TEST2_4), rank_sel,
		TEST2_4_TESTAGENTRK);
}

void dramc_engine2_set_pat(DRAMC_CTX_T *p, unsigned char test_audpat,
	unsigned char log_2_loop_count, unsigned char use_len1_flag)
{
	if (test_audpat == TEST_XTALK_PATTERN) {
		/*
		 * TEST_REQ_LEN1=1 is new feature,
		 * hope to make dq bus continuously.
		 * but DV simulation will got problem of compare err
		 * so go back to use old way
		 * TEST_REQ_LEN1=0, R_DMRWOFOEN=1
		 */
		if (use_len1_flag != 0) {
			io_32_write_fld_multi(
				DRAMC_REG_ADDR(DRAMC_REG_TEST2_4),
				p_fld(SET_FLD, TEST2_4_TEST_REQ_LEN1));
		} else {
			io_32_write_fld_multi(
				DRAMC_REG_ADDR(DRAMC_REG_TEST2_4),
				p_fld(CLEAR_FLD, TEST2_4_TEST_REQ_LEN1));
		}
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_PERFCTL0),
			p_fld(SET_FLD, PERFCTL0_RWOFOEN));

		/* select XTALK pattern */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3),
			p_fld(CLEAR_FLD, TEST2_3_TESTAUDPAT) |
			p_fld(log_2_loop_count, TEST2_3_TESTCNT));

		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_4),
			p_fld(SET_FLD, TEST2_4_TESTXTALKPAT) |
			p_fld(CLEAR_FLD, TEST2_4_TESTAUDMODE) |
			p_fld(CLEAR_FLD, TEST2_4_TESTAUDBITINV));

		/* R_DMTESTSSOPAT=0, R_DMTESTSSOXTALKPAT=0 */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_4),
			p_fld(CLEAR_FLD, TEST2_4_TESTSSOPAT) |
			p_fld(CLEAR_FLD, TEST2_4_TESTSSOXTALKPAT));
	} else if (test_audpat == TEST_AUDIO_PATTERN) {
		/*
		 * set AUDINIT=0x11 AUDINC=0x0d AUDBITINV=1 AUDMODE=1
		 * (1:read only(address fix), 0: write/read address change)
		 */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_4),
			p_fld(0x00000011, TEST2_4_TESTAUDINIT) |
			p_fld(0x0000000d, TEST2_4_TESTAUDINC) |
			p_fld(CLEAR_FLD, TEST2_4_TESTXTALKPAT) |
			p_fld(CLEAR_FLD, TEST2_4_TESTAUDMODE) |
			p_fld(SET_FLD, TEST2_4_TESTAUDBITINV));

		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3),
			p_fld(SET_FLD, TEST2_3_TESTAUDPAT) |
			p_fld(log_2_loop_count, TEST2_3_TESTCNT));
	} else {
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3),
			p_fld(CLEAR_FLD, TEST2_3_TESTAUDPAT) |
			p_fld(log_2_loop_count, TEST2_3_TESTCNT));
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_TEST2_4),
			CLEAR_FLD, TEST2_4_TESTXTALKPAT);
	}
}

void dramc_engine2_check_complete(DRAMC_CTX_T *p, unsigned char status)
{
	unsigned int loop_count = 0;

	while ((io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_TESTRPT)) & status)
		!= status) {
		delay_us(CMP_CPT_POLLING_PERIOD);
		loop_count++;
		if (loop_count > MAX_CMP_CPT_WAIT_LOOP) {
			show_msg((INFO, "TA2 check complete time out\n"));
			break;
		}
	}
}

unsigned int dramc_engine2_compare(DRAMC_CTX_T *p, DRAM_TE_OP_T wr)
{
	unsigned int result = 0xffffffff;
	unsigned int loop_count;
	unsigned char status = 1;	/* RK0 */

	loop_count = io_32_read_fld_align(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3),
		TEST2_3_TESTCNT);
	if (loop_count == 1)
		status = 3;	/* RK0/1 */

	if (wr == TE_OP_WRITE_READ_CHECK) {
		/* read data compare ready check */
		dramc_engine2_check_complete(p, status);

		/* disable write */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3),
			p_fld(CLEAR_FLD, TEST2_3_TEST2W) |
			p_fld(CLEAR_FLD, TEST2_3_TEST2R) |
			p_fld(CLEAR_FLD, TEST2_3_TEST1));

		delay_us(1);

		/* enable read */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3),
			p_fld(CLEAR_FLD, TEST2_3_TEST2W) |
			p_fld(SET_FLD, TEST2_3_TEST2R) |
			p_fld(CLEAR_FLD, TEST2_3_TEST1));
	}
	/* 5 */
	/* read data compare ready check */
	dramc_engine2_check_complete(p, status);

	/* delay 10ns after ready check from DE suggestion (1ms here) */

	result = (io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_TESTRPT)) >> 4) &
		status;	/* CMP_ERR_RK0/1 */

	return result;
}

/*
 * DramcEngine2
 *  start the self test engine 2 inside dramc to test dram w/r.
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @param  wr              (DRAM_TE_OP_T): TE operation
 *  @param  test2_1         (unsigned int): 28bits, base address[27:0].
 *  @param  test2_2         (unsigned int): 28bits, offset address[27:0].
 *			(unit is 16-byte, i.e: 0x100 is 0x1000).
 *  @param  loopforever     (signed short): 0 read\write one time , then exit
 *                                >0 enable eingie2, after "loopforever"
 *			second , write log and exit
 *                                -1 loop forever to read\write, every "period"
 *			seconds , check result , only when we
 *			find error, write log and exit
 *                                -2 loop forever to read\write, every "period"
 *			seconds , write log , only when we find
 *			error, write log and exit
 *                                -3 just enable loop forever , then exit
 *  @param period	(unsigned char):it is valid only when loopforever <0;
 *			period should greater than 0
 *  @param log_2_loop_count	(unsigned char):
 *			test loop number of test agent2 loop
 *			number =2^(log_2_loop_count) , 0 one time
 *  @retval status          (unsigned int): return the value of DM_CMP_ERR  ,
 *			0 is ok , others mean  error
 */
DRAM_STATUS_T dramc_engine2_init(DRAMC_CTX_T *p, unsigned int test2_1,
	unsigned int test2_2, unsigned char test_audpat,
	unsigned char log_2_loop_count)
{
	unsigned char use_len1_flag;

	/* error handling */
	if (!p) {
		show_err("context is NULL\n");
		return DRAM_FAIL;
	}
	/* check loop number validness */
	if (log_2_loop_count > 15) {
		show_err("wrong param: log_2_loop_count > 15\n");
		return DRAM_FAIL;
	}

	use_len1_flag = (test_audpat & 0x80) >> 7;
	test_audpat = test_audpat & 0x7f;

	dramc_set_rank_engine2(p, p->rank);

	reg0d0h[p->channel] =
		io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD));
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD),
		p_fld(CLEAR_FLD, DUMMY_RD_DQSG_DMYRD_EN) |
		p_fld(CLEAR_FLD, DUMMY_RD_DQSG_DMYWR_EN) |
		p_fld(CLEAR_FLD, DUMMY_RD_DUMMY_RD_EN) |
		p_fld(CLEAR_FLD, DUMMY_RD_SREF_DMYRD_EN) |
		p_fld(CLEAR_FLD, DUMMY_RD_DMY_RD_DBG) |
		p_fld(CLEAR_FLD, DUMMY_RD_DMY_WR_DBG));
	/* must close dummy read when do test agent */
#if NON_EXIST_RG
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_TESTCHIP_DMA1),
		CLEAR_FLD, TESTCHIP_DMA1_DMA_LP4MATAB_OPT);
#endif
	/* disable self test engine1 and self test engine2 */
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3),
		p_fld(CLEAR_FLD, TEST2_3_TEST2W) |
		p_fld(CLEAR_FLD, TEST2_3_TEST2R) |
		p_fld(CLEAR_FLD, TEST2_3_TEST1));

	/*
	 * 1.set pattern , base address , offset address
	 * 2.select  ISI pattern or audio pattern or xtalk pattern
	 * 3.set loop number
	 * 4.enable read or write
	 * 5.loop to check DM_CMP_CPT
	 * 6.return CMP_ERR
	 * currently only implement ucengine_status = 1,
	 * others are left for future extension
	 */

	/* 1 */
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_0),
		p_fld(test2_1 >> 24, TEST2_0_TEST2_PAT0) |
		p_fld(test2_2 >> 24, TEST2_0_TEST2_PAT1));

	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_TEST2_1),
		test2_1 & 0x00ffffff, TEST2_1_TEST2_BASE);
	io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_TEST2_2),
		test2_2 & 0x00ffffff, TEST2_2_TEST2_OFF);

	/* 2 & 3 */
	/*
	 * (TESTXTALKPAT, TESTAUDPAT) =
	 * 00 (ISI), 01 (AUD), 10 (XTALK), 11 (UNKNOWN)
	 */
	dramc_engine2_set_pat(p, test_audpat, log_2_loop_count, use_len1_flag);
	return DRAM_OK;
}

unsigned int dramc_engine2_run(DRAMC_CTX_T *p, DRAM_TE_OP_T wr,
	unsigned char test_audpat)
{
	unsigned int result = 0xffffffff;

	/* 4 */
	if (wr == TE_OP_READ_CHECK) {
		if ((test_audpat == 1) || (test_audpat == 2)) {
			/*
			 * if audio pattern, enable read only
			 * (disable write after read), AUDMODE=0x48[15]=0
			 */
			io_32_write_fld_align(
				DRAMC_REG_ADDR(DRAMC_REG_TEST2_4), CLEAR_FLD,
				TEST2_4_TESTAUDMODE);
		}
		/* enable read */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3),
			p_fld(CLEAR_FLD, TEST2_3_TEST2W) |
			p_fld(SET_FLD, TEST2_3_TEST2R) |
			p_fld(CLEAR_FLD, TEST2_3_TEST1));
	} else if (wr == TE_OP_WRITE_READ_CHECK) {
		/* enable write */
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3),
			p_fld(SET_FLD, TEST2_3_TEST2W) |
			p_fld(CLEAR_FLD, TEST2_3_TEST2R) |
			p_fld(CLEAR_FLD, TEST2_3_TEST1));
	}
	dramc_engine2_compare(p, wr);

	/* delay 10ns after ready check from DE suggestion (1ms here) */
	delay_us(1);

	/* 6
	 * return CMP_ERR, 0 is ok , others are fail,
	 * disable test2w or test2r
	 * get result or all result
	*/
	result = (io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_CMP_ERR)));
	/* disable read */
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3),
		p_fld(CLEAR_FLD, TEST2_3_TEST2W) |
		p_fld(CLEAR_FLD, TEST2_3_TEST2R) |
		p_fld(CLEAR_FLD, TEST2_3_TEST1));


	return result;
}

void dramc_engine2_end(DRAMC_CTX_T *p)
{
	io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_4),
		p_fld(CLEAR_FLD, TEST2_4_TEST_REQ_LEN1));
	/* test agent 2 with cmd length = 0 */
	io32_write_4b(DRAMC_REG_ADDR(DRAMC_REG_DUMMY_RD), reg0d0h[p->channel]);
}

#if (FOR_DV_SIMULATION_USED == 1)
#define EMI_APB_BASE 0xF0107000
#else
#define EMI_APB_BASE 0x10219000
#endif
void ta2_test_run_time_hw_set_column_num(DRAMC_CTX_T *p)
{
    unsigned char ch = 0, shu_index;
    unsigned int emi_cona, ma_type[CHANNEL_NUM][RANK_MAX], min_ma;
    DRAM_CHANNEL_T e_ori_channel = p->channel;

    min_ma = 0x3;   // ma max: 0x3(reserved)
    for (ch = 0; ch < p->support_channel_num; ch++) {
        p->channel = ch;
        emi_cona = io32_read_4b(EMI_APB_BASE);
        if (p->support_channel_num <= 2) {
            unsigned int temp_ma;

            ma_type[ch][RANK_0] =
                ((emi_cona >> (4 + ch * 16)) & 0x3) + 1;
            ma_type[ch][RANK_1] =
                ((emi_cona >> (6 + ch * 16)) & 0x3) + 1;
            temp_ma = (ma_type[ch][RANK_0] < ma_type[ch][RANK_1])
                        ? ma_type[ch][RANK_0] : ma_type[ch][RANK_1];
            if (temp_ma < min_ma)
                min_ma = temp_ma;
        }
    }
    print("final ma_type:%d\n", min_ma);
    for (shu_index = DRAM_DFS_SHUFFLE_1;
            shu_index < DRAM_DFS_SHUFFLE_MAX; shu_index++)
            io_32_write_fld_align(
                DRAMC_REG_ADDR(DRAMC_REG_SHU_CONF0) +
                SHU_GRP_DRAMC_OFFSET * shu_index,
                min_ma, SHU_CONF0_MATYPE);

    p->channel = e_ori_channel;
}

/* ----------------------------------------------------------------------
 * LP4 RG Address
 * bit:   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 9 8 7 6 5 4 3 2 1 0
 *        1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *        ---------------------------------------------------------------
 * RG:    - - R R R R R R R R R R R R R R R R R|B B B|C C C C C C - - - -
 *            1 1 1 1 1 1 1 9 8 7 6 5 4 3 2 1 0|A A A|9 8 7 6 5 4
 *            6 5 4 3 2 1 0                    |2 1 0|
 * AXI    ---------------------------------------------------------------
 * Addr:  - R R R R R R R R R R R R R R R R|B B B|C C C|C|C C C C C C C -
 *          1 1 1 1 1 1 9 8 7 6 5 4 3 2 1 0|A A A|9 8 7|H|6 5 4 3 2 1 0
 *          5 4 3 2 1 0                    |2 1 0|     |A|
 *
 * ----------------------------------------------------------------------
 * LP3 RG Address
 * bit:   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 9 8 7 6 5 4 3 2 1 0
 *        1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *        ---------------------------------------------------------------
 * RG:    - R R R R R R R R R R R R R R R R|B B B|C C C C C C C C - - - -
 *          1 1 1 1 1 1 9 8 7 6 5 4 3 2 1 0|A A A|9 8 7 6 5 4 3 2
 *          5 4 3 2 1 0                    |2 1 0|
 * AXI    ---------------------------------------------------------------
 * Addr:  - R R R R R R R R R R R R R R R R|B B B|C C C C C C C C C C - -
 *          1 1 1 1 1 1 9 8 7 6 5 4 3 2 1 0|A A A|9 8 7 6 5 4 3 2 1 0
 *          5 4 3 2 1 0                    |2 1 0|
 * ----------------------------------------------------------------------
 */
#define TA2_RANK0_ADDRESS (0x40200000)
void ta2_test_run_time_hw_presetting(DRAMC_CTX_T *p, unsigned int len,
		TA2_RKSEL_TYPE_T rksel_mode)
{
	unsigned int base, offset, addr;

	addr = TA2_RANK0_ADDRESS & 0x1fffffff;
	/* drop bit8, bit0 */
	base = (((addr & ~0x1ff) >> 2) | (addr & 0xff) >> 1) >> 4;
	/* 1:ch_a/b field, 5:0x20 bytes coverage per pattern */
	offset = len >> (1 + 5);

	offset = (offset == 0) ? 1:offset;	/* halt if offset = 0 */

	/* offset = 0x400; */
	/* fill based on RG table for Rank 0 */
	io_32_write_fld_align_all(DRAMC_REG_TEST2_1, base,
		TEST2_1_TEST2_BASE);

	/* 128 bits data length per offest */
	io_32_write_fld_align_all(DRAMC_REG_TEST2_2, offset,
		TEST2_2_TEST2_OFF);
	io_32_write_fld_align_all(DRAMC_REG_TEST2_4, rksel_mode,
		TEST2_4_TESTAGENTRKSEL);

	if (rksel_mode == TA2_RKSEL_XRT) {
		io_32_write_fld_align_all(DRAMC_REG_PERFCTL0, CLEAR_FLD,
			PERFCTL0_RWOFOEN);
		show_msg((INFO, "=== TA2 XRT R2R/W2W\n"));
	} else {
		io_32_write_fld_align_all(DRAMC_REG_PERFCTL0, SET_FLD,
			PERFCTL0_RWOFOEN);
		show_msg((INFO, "=== TA2 HW\n"));
	}

	ta2_test_run_time_hw_set_column_num(p);
}

#define TA2_PAT TEST_XTALK_PATTERN
void ta2_test_run_time_pat_setting(DRAMC_CTX_T *p, unsigned char PatSwitch)
{
	static unsigned char pat = TA2_PAT, loop = 1;
	unsigned char ch = 0;
	DRAM_CHANNEL_T e_ori_channel = p->channel;

	if (loop || PatSwitch == TA2_PAT_SWITCH_ON) {
		show_msg((INFO, "TA2 PAT: %s\n",
			(pat == TEST_XTALK_PATTERN) ?
			"XTALK" : (pat == TEST_AUDIO_PATTERN) ?
			"AUDIO" : "ISI"));
		for (ch = CHANNEL_A; ch < p->support_channel_num; ch++) {
			p->channel = ch;
			dramc_engine2_set_pat(p, pat,
				p->support_rank_num - 1, 0);
		}
		p->channel = e_ori_channel;
		if (PatSwitch)
			pat = (pat + 1) % 3;
		else
			loop = 0;
	}
}

void ta2_test_run_time_hw_write(DRAMC_CTX_T *p, U32 enable)
{
	io_32_write_fld_align_all(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3), enable,
		TEST2_3_TEST2W);
}

DRAM_STATUS_T ta2_test_run_time_hw_status(DRAMC_CTX_T *p)
{
	unsigned char ch = 0;
	unsigned char rank_idx = 0;
	unsigned int error_value = 0;
	static unsigned int err_count = FALSE;
	static unsigned int pass_count = FALSE;

	for (ch = 0; ch < p->support_channel_num; ch++) {
		p->channel = ch;
		error_value = dramc_engine2_compare(p, TE_OP_WRITE_READ_CHECK);
		if (error_value&0x3) {	/* RK0 or RK1 test fail */
			show_msg((INFO,
				"%s%s(%d) error_value:0x%x, bit error: 0x%x\n",
				"=== HW", "HW channel", ch, error_value,
				io32_read_4b
				(DRAMC_REG_ADDR(DRAMC_REG_CMP_ERR))));
		}
		for (rank_idx = 0; rank_idx < p->support_rank_num;
				rank_idx++) {
			if (error_value & (1 << rank_idx)) {
				err_count++;
				show_msg((INFO,
					"%s(%d) %s(%d)%s%s%d%s%d%s0x%x\n",
					"HW channel", ch, "Rank", rank_idx,
					", TA2 failed",
					", pass_cnt:", pass_count,
					", err_cnt:", err_count,
					", error_value ",
					error_value));
			} else {
				pass_count++;
				show_msg((INFO,
					"%s(%d) %s(%d)%s%d, err_cnt:%d\n",
					"HW channel", ch, "Rank", rank_idx,
					", TA2 pass, pass_cnt:",
					pass_count, err_count));
			}
		}
		io_32_write_fld_multi(DRAMC_REG_ADDR(DRAMC_REG_TEST2_3),
			p_fld(CLEAR_FLD, TEST2_3_TEST2W) |
			p_fld(CLEAR_FLD, TEST2_3_TEST2R) |
			p_fld(CLEAR_FLD, TEST2_3_TEST1));

	}
	p->channel = CHANNEL_A;

	return err_count? DRAM_FAIL : DRAM_OK;
}

DRAM_STATUS_T ta2_test_run_time_hw(DRAMC_CTX_T *p)
{
	DRAM_STATUS_T ret;
	DRAM_CHANNEL_T channel_bak = p->channel;
	DRAM_RANK_T rank_bak = p->rank;

	ta2_test_run_time_hw_presetting(p, 0x10000, TA2_RKSEL_HW);
	ta2_test_run_time_pat_setting(p, TA2_PAT_SWITCH_ON);

	ta2_test_run_time_hw_write(p, ENABLE);
	ret = ta2_test_run_time_hw_status(p);

	p->channel = channel_bak;
	p->rank = rank_bak;

	return ret;
}

