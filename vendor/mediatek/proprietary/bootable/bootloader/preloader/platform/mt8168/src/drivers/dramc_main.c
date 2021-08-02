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
 * ==========================================================================
 *   Include Files
 * ==========================================================================
 */
#include "dramc_common.h"
#include "x_hal_io.h"
#if (FOR_DV_SIMULATION_USED == 0)
#include "emi.h"
#endif

#if (FOR_DV_SIMULATION_USED == 1)
#define sys_print(...)
#endif

#define PATTERN_5A5A		0x5a5a
#define TX_SHIFT_OFFSET	8

#if (FOR_DV_SIMULATION_USED == 1)
void assertion_failed(const char *function, const char *file, int line)
{
	print("!!! DEBUG ASSERTION FAILED !!!\n"
		"[Function] %s\n [File] %s\n [Line] %d\n",
		function, file, line);
	while
		(1);
}

#define ASSERT(expr)							\
	do {								\
		if (!(expr))						\
		assertion_failed(__func__, __FILE__, __LINE__);	\
	} while (0)

#endif
/*
 * ==========================================================================
 *   Global Variables
 * ==========================================================================
 */
#if SUPPORT_TYPE_LPDDR4
/* DVFS table */
DRAM_DFS_FREQUENCY_TABLE_T freq_tbl_lp4[DRAM_DFS_SHUFFLE_MAX] = {
#if LPDDR4_DIV4_MODE_EN
	{DDR_DDR1600, DIV4_MODE, DDR1600_FREQ, DRAM_DFS_SHUFFLE_1},
	{DDR_DDR1333, DIV4_MODE, DDR1333_FREQ, DRAM_DFS_SHUFFLE_2},
	{DDR_DDR1200, DIV4_MODE, DDR1200_FREQ, DRAM_DFS_SHUFFLE_3},
	//{DDR_DDR1600, DIV4_MODE, DDR1600_FREQ, DRAM_DFS_SHUFFLE_3},
#else
	{DDR_DDR3200, DIV8_MODE, DDR3200_FREQ, DRAM_DFS_SHUFFLE_1},
	{DDR_DDR2666, DIV8_MODE, DDR2666_FREQ, DRAM_DFS_SHUFFLE_2},
	{DDR_DDR1600, DIV8_MODE, DDR1600_FREQ, DRAM_DFS_SHUFFLE_3}, //[QW] bring up setting
	//{DDR_DDR3200, DIV8_MODE, DDR3200_FREQ, DRAM_DFS_SHUFFLE_3},
	//{DDR_DDR2666, DIV8_MODE, DDR2666_FREQ, DRAM_DFS_SHUFFLE_3},
#endif
};
#endif

#if SUPPORT_TYPE_LPDDR3
DRAM_DFS_FREQUENCY_TABLE_T freq_tbl_lp3[DRAM_DFS_SHUFFLE_MAX] = {
	{DDR_DDR1600, DIV4_MODE, DDR1600_FREQ, DRAM_DFS_SHUFFLE_1},
	{DDR_DDR1333, DIV4_MODE, DDR1333_FREQ, DRAM_DFS_SHUFFLE_2},
	{DDR_DDR1200, DIV4_MODE, DDR1200_FREQ, DRAM_DFS_SHUFFLE_3},
};
#endif
#if SUPPORT_TYPE_PCDDR4
DRAM_DFS_FREQUENCY_TABLE_T freq_tbl_pc4[DRAM_DFS_SHUFFLE_MAX] = {
#if PCDDR4_DIV4_MODE_EN
	{DDR_DDR1600, DIV4_MODE, DDR1600_FREQ, DRAM_DFS_SHUFFLE_1},//[QW] bring up setting
	{DDR_DDR1333, DIV4_MODE, DDR1333_FREQ, DRAM_DFS_SHUFFLE_2},
#else
	//{DDR_DDR3200, DIV8_MODE, DDR3200_FREQ, DRAM_DFS_SHUFFLE_1},
	{DDR_DDR2666, DIV8_MODE, DDR2666_FREQ, DRAM_DFS_SHUFFLE_1},
	{DDR_DDR3200, DIV8_MODE, DDR3200_FREQ, DRAM_DFS_SHUFFLE_2},
#endif
};
#endif
#if SUPPORT_TYPE_PCDDR3
DRAM_DFS_FREQUENCY_TABLE_T freq_tbl_pc3[DRAM_DFS_SHUFFLE_MAX] = {
	{DDR_DDR1333, DIV4_MODE, DDR1333_FREQ, DRAM_DFS_SHUFFLE_1},
	{DDR_DDR1600, DIV4_MODE, DDR1600_FREQ, DRAM_DFS_SHUFFLE_1},
	{DDR_DDR1866, DIV4_MODE, DDR1866_FREQ, DRAM_DFS_SHUFFLE_1},
};
#endif

DRAMC_CTX_T *ps_curr_dram_ctx;

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
   SAVE_TIME_FOR_CALIBRATION_T SavetimeData;
#endif

#if defined(DDR_INIT_TIME_PROFILING) || (__ETT__ && SUPPORT_SAVE_TIME_FOR_CALIBRATION)
DRAMC_CTX_T gTimeProfilingDramCtx;
U8 gtime_profiling_flag = 0;
#endif

#if (FOR_DV_SIMULATION_USED == 0)
DRAMC_CTX_T DramCtx_LPDDR4 = {
#if (CHANNEL_NUM == 4)
	.support_channel_num = CHANNEL_FOURTH,
#else
	.support_channel_num = CHANNEL_DUAL,
#endif
	.channel = CHANNEL_A,
	.support_rank_num = RANK_SINGLE,
	.rank = RANK_0,
	.freq_sel = DDR_DDR1600,
	.shu_type = DRAM_DFS_SHUFFLE_1,
	.dfs_table = NULL,
	.dram_type = TYPE_LPDDR4,
	.pinmux_type = PINMUX_TYPE_LP4_DSC_X16,
	.dram_fsp = FSP_0,
	.odt_onoff = ODT_OFF,
	.dram_cbt_mode = {CBT_NORMAL_MODE, CBT_NORMAL_MODE},
#if ENABLE_READ_DBI
	.dbi_r_onoff		= {DBI_OFF, DBI_ON},
#else
	.dbi_r_onoff		= {DBI_OFF, DBI_OFF},
#endif
#if ENABLE_WRITE_DBI
	.dbi_w_onoff = {DBI_OFF, DBI_ON},
#else
	.dbi_w_onoff = {DBI_OFF, DBI_OFF},
#endif
	.data_width = DATA_WIDTH_16BIT,
	.test2_1 = DEFAULT_TEST2_1_CAL,
	.test2_2 = DEFAULT_TEST2_2_CAL,
	.test_pattern = TEST_XTALK_PATTERN,
	.frequency = DDR1600_FREQ,
	.freqGroup = DDR1600_FREQ,
	.shuffle_frequency = {0},
	.vendor_id= 0x88,
	.revision_id = 0,
	.density = 0,
	.ranksize = {0, 0},
	.ucnum_dlycell_perT = 0,
	.delay_cell_timex100 = 0,
	.enable_cbt_scan_vref	= ENABLE,
	.enable_rx_scan_vref	= ENABLE,
	.enable_tx_scan_vref	= ENABLE,
	.ssc_en = DISABLE, /* SSC */
	.en_4bit_mux = DISABLE, /* 4BIT MUX */
#if PRINT_CALIBRATION_SUMMARY
	.cal_result_flag = { {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	.cal_execute_flag = { {0, 0}, {0, 0}, {0, 0}, {0, 0} },
#endif
	.arfg_write_leveling_init_shif = { {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	.fg_tx_perbif_init = { {FALSE, FALSE}, {FALSE, FALSE}, {FALSE, FALSE}, {FALSE, FALSE} },
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	.femmc_Ready = FALSE, //femmc_Ready
	.Bypass_TXWINDOW = 0,
	.Bypass_RXWINDOW = 0,
	.Bypass_RDDQC = 0,
	.pSavetimeData =  &SavetimeData,
#endif
	.is_emcp = FALSE, /* Shall change according to real chip config */
};
#else /* For DV simulation used context */
DRAMC_CTX_T DramCtx_LPDDR4 = {
	.support_channel_num 	= CHANNEL_SINGLE,
	.channel		= CHANNEL_A,
	.support_rank_num	= RANK_SINGLE,
	.rank			= RANK_0,
	.freq_sel		= DDR_DDR1600,
	.shu_type		= DRAM_DFS_SHUFFLE_1,
	.dfs_table		= NULL,
	.dram_type		= TYPE_LPDDR4,
	.pinmux_type		= PINMUX_TYPE_LP4_DSC_X16,
	.dram_fsp		= FSP_0,
	.odt_onoff		= ODT_OFF,
	.dram_cbt_mode		= {CBT_NORMAL_MODE, CBT_NORMAL_MODE},
#if ENABLE_READ_DBI
	.dbi_r_onoff		= {DBI_OFF, DBI_ON},
#else
	.dbi_r_onoff		= {DBI_OFF, DBI_OFF},
#endif
#if ENABLE_WRITE_DBI
	.dbi_w_onoff		= {DBI_OFF, DBI_ON},
#else
	.dbi_w_onoff		= {DBI_OFF, DBI_OFF},
#endif
	.data_width		= DATA_WIDTH_16BIT,
	.test2_1		= DEFAULT_TEST2_1_CAL,
	.test2_2		= DEFAULT_TEST2_2_CAL,
	.test_pattern		= TEST_XTALK_PATTERN,
	.frequency		= DDR1600_FREQ,
	.freqGroup		= DDR1600_FREQ,
	.shuffle_frequency	= {0},
	.vendor_id		= 0x88,
	.revision_id		= 0,
	.density		= 0,
	.ranksize		= {0, 0},
	.ucnum_dlycell_perT	= 0,
	.delay_cell_timex100	= 0,
	.enable_cbt_scan_vref	= DISABLE,
	.enable_rx_scan_vref	= DISABLE,
	.enable_tx_scan_vref	= DISABLE,
	.ssc_en			= DISABLE, /* SSC */
	.en_4bit_mux		= DISABLE, /* 4BIT MUX */
#if PRINT_CALIBRATION_SUMMARY
	.cal_result_flag	= { {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	.cal_execute_flag	= { {0, 0}, {0, 0}, {0, 0}, {0, 0} },
#endif
	.arfg_write_leveling_init_shif	= { {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	.fg_tx_perbif_init	= { {FALSE, FALSE}, {FALSE, FALSE}, {FALSE, FALSE}, {FALSE, FALSE} },
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	.femmc_Ready = FALSE, //femmc_Ready
	.Bypass_TXWINDOW = 0,
	.Bypass_RXWINDOW = 0,
	.Bypass_RDDQC = 0,
	.pSavetimeData =  &SavetimeData,
#endif
	.is_emcp		= FALSE,
};
#endif

DRAMC_CTX_T DramCtx_PCDDR3 = {
	CHANNEL_SINGLE,
	CHANNEL_A,
	RANK_DUAL,
	RANK_0,
	DDR_DDR1600,
	DRAM_DFS_SHUFFLE_1,
	NULL,
	TYPE_PCDDR3,
	PINMUX_TYPE_PC3_X16X2,
	FSP_0,
	ODT_OFF,
	{CBT_NORMAL_MODE, CBT_NORMAL_MODE},
	{DBI_OFF, DBI_OFF},
#if ENABLE_WRITE_DBI
	{DBI_OFF, DBI_ON},
#else
	{DBI_OFF, DBI_OFF},
#endif
#if SUPPORT_PCDDR3_32BIT
	DATA_WIDTH_32BIT,
#else
	DATA_WIDTH_16BIT,
#endif
	DEFAULT_TEST2_1_CAL,
	DEFAULT_TEST2_2_CAL,
	TEST_XTALK_PATTERN,
	DDR2400_FREQ,
	DDR2400_FREQ,
	{0},
	0x88,
	0,
	0,
	{0, 0},
	0,
	0,
	DISABLE,
	DISABLE,
	DISABLE,
	DISABLE, /* SSC */
	DISABLE, /* 4BIT MUX */
#if PRINT_CALIBRATION_SUMMARY
	{ {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	{ {0, 0}, {0, 0}, {0, 0}, {0, 0} },
#endif
	{ {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	{ {FALSE, FALSE}, {FALSE, FALSE}, {FALSE, FALSE}, {FALSE, FALSE} },
	FALSE,
};

DRAMC_CTX_T DramCtx_LPDDR3 = {
	CHANNEL_SINGLE,
	CHANNEL_A,
	RANK_SINGLE,
	RANK_0,
	DDR_DDR1600,
	DRAM_DFS_SHUFFLE_1,
	NULL,
	TYPE_LPDDR3,
	PINMUX_TYPE_LP3_DSC,
	FSP_0,
	ODT_OFF,
	{CBT_NORMAL_MODE, CBT_NORMAL_MODE},
	{DBI_OFF, DBI_OFF},
#if ENABLE_WRITE_DBI
	{DBI_OFF, DBI_ON},
#else
	{DBI_OFF, DBI_OFF},
#endif
	DATA_WIDTH_32BIT,
	DEFAULT_TEST2_1_CAL,
	DEFAULT_TEST2_2_CAL,
	TEST_XTALK_PATTERN,
	DDR2400_FREQ,
	DDR2400_FREQ,
	{0},
	0x88,
	0,
	0,
	{0, 0},
	0,
	0,
	DISABLE,
	DISABLE,
	DISABLE,
	DISABLE, /* SSC */
	DISABLE, /* 4BIT MUX */
#if PRINT_CALIBRATION_SUMMARY
	{ {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	{ {0, 0}, {0, 0}, {0, 0}, {0, 0} },
#endif
	{ {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	{ {FALSE, FALSE}, {FALSE, FALSE}, {FALSE, FALSE}, {FALSE, FALSE} },
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	 FALSE, //femmc_Ready
	 0,
	 0,
	 0,
	 &SavetimeData,
#endif
	FALSE,
};

DRAMC_CTX_T DramCtx_PCDDR4 = {
	.support_channel_num = CHANNEL_DUAL,
	.channel = CHANNEL_A,
	.support_rank_num = RANK_SINGLE,
	.rank = RANK_0,
	.freq_sel = DDR_DDR1600,
	.shu_type = DRAM_DFS_SHUFFLE_1,
	.dfs_table = NULL,
	.dram_type = TYPE_PCDDR4,
	.pinmux_type = PINMUX_TYPE_PC4_X16,
	.dram_fsp = FSP_0,
	.odt_onoff = ODT_ON,
	.dram_cbt_mode = {CBT_NORMAL_MODE, CBT_NORMAL_MODE},
	.dbi_r_onoff = {DBI_OFF, DBI_OFF},
#if ENABLE_WRITE_DBI
	.dbi_w_onoff = {DBI_OFF, DBI_ON},
#else
	.dbi_w_onoff = {DBI_OFF, DBI_OFF},
#endif
	.data_width = DATA_WIDTH_16BIT,
	.test2_1 = DEFAULT_TEST2_1_CAL,
	.test2_2 = DEFAULT_TEST2_2_CAL,
	.test_pattern = TEST_XTALK_PATTERN,
	.frequency = DDR2400_FREQ,
	.freqGroup = DDR2400_FREQ,
	.shuffle_frequency = {0},
	.vendor_id = 0x88,
	.revision_id = 0,
	.density = 0,
	.ranksize = {0, 0},
	.ucnum_dlycell_perT = 0,
	.delay_cell_timex100 = 0,
	.enable_cbt_scan_vref = DISABLE,
	.enable_rx_scan_vref = ENABLE,
	.enable_tx_scan_vref = ENABLE,
	.ssc_en = DISABLE, /* SSC */
	.en_4bit_mux = DISABLE, /* 4BIT MUX */
#if PRINT_CALIBRATION_SUMMARY
	.cal_result_flag = { {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	.cal_execute_flag = { {0, 0}, {0, 0}, {0, 0}, {0, 0} },
#endif
	.arfg_write_leveling_init_shif = { {0, 0}, {0, 0}, {0, 0}, {0, 0} },
	.fg_tx_perbif_init = { {FALSE, FALSE}, {FALSE, FALSE}, {FALSE, FALSE}, {FALSE, FALSE} },
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	.femmc_Ready = FALSE, //femmc_Ready
	.Bypass_TXWINDOW = 0,
	.Bypass_RXWINDOW = 0,
	.Bypass_RDDQC = 0,
	.pSavetimeData =  &SavetimeData,
#endif
	.is_emcp = TRUE, /*pc4 donot care */
};

DRAMC_CTX_T *dramc_ctx_lp4 = &DramCtx_LPDDR4;
DRAMC_CTX_T *dramc_ctx_lp3 = &DramCtx_LPDDR3;
DRAMC_CTX_T *dramc_ctx_ddr4 = &DramCtx_PCDDR4;
DRAMC_CTX_T *dramc_ctx_ddr3 = &DramCtx_PCDDR3;

#define DUMP_AO_REG		0

unsigned char gfirst_init_flag = FALSE;
unsigned char get_mdl_used_flag_value = FALSE;
#ifdef ENABLE_MIOCK_JMETER
unsigned char pre_miock_jmeter_hqa_used_flag = FALSE;
#endif

#if CPU_RW_TEST_AFTER_K
void mem_test_address_calculation(DRAMC_CTX_T *p, unsigned int src_addr,
	unsigned int *pu4_dest)
{
	*pu4_dest = src_addr + p->ranksize[RANK_0];
}

#define new_addr(cnt, addr)	*(volatile unsigned int *)(cnt + addr)
DRAM_STATUS_T dram_cpu_read_write_test_after_calibration(DRAMC_CTX_T *p)
{
    unsigned char dump_info = 0, rank_idx;
    unsigned int len, count, fixed_addr, rank_addr[RANK_MAX];
    unsigned int pass_count, err_count;
    static unsigned int test_loop = 0;
    static unsigned int fail_loop = 0;
    static unsigned int flag = 0;
    unsigned int temp;

    flag = !flag;
    test_loop++;
    len = WORD_MAX;

    rank_addr[0] = DDR_BASE;

    mem_test_address_calculation(p, DDR_BASE, &rank_addr[1]);

    for (rank_idx = 0; rank_idx < p->support_rank_num; rank_idx++) {
        dump_info = 0;
        err_count = 0;
        pass_count = 0;

        fixed_addr = rank_addr[rank_idx];
    #ifdef DRAM_FULLSCAN
        len = p->ranksize[rank_idx];
    #endif

        for (count = 0; count < len; count += 4) {
            temp = flag ? count + (PATTERN_5A5A << 16): PATTERN_5A5A +(count<<16);
            new_addr(count, fixed_addr) = temp;

        #ifdef DRAM_FULLSCAN
            if (count % 0x1000000 == 0)
                info("[MEM_TEST] Set bytes offset: 0x%x\n", count);
        #endif
        }

        for (count = 0; count < len; count += 4) {
            temp = flag ? count + (PATTERN_5A5A << 16): PATTERN_5A5A +(count<<16);
            if (new_addr(count, fixed_addr) != temp){
                    err_count++;
                    if(err_count < 10)
                        error("[MEM_TEST]Fail: [%x]=%x, exp=%x\n", count+fixed_addr, new_addr(count, fixed_addr), temp);
                    else
                        break;
            } else {
                pass_count++;
            }

        #ifdef DRAM_FULLSCAN
            if (count % 0x1000000 == 0)
                info("[MEM_TEST] Check bytes offset: 0x%x\n", count);
        #endif
        }

        if (err_count) {
            show_err2("[MEM_TEST] Rank %d Fail.",
                rank_idx);
            dump_info = 1;
        } else {
            show_msg3((INFO, "[MEM_TEST] Rank %d OK.",
                rank_idx));
        }
        show_msg3((INFO,
            "(FixedAddr 0x%X, Pass count =%d, Fail count =%d, loop=%d)\n",
            fixed_addr, pass_count, err_count, test_loop));
    }

    if(err_count) {
        fail_loop++;
        if(fail_loop > 5)
            while(1);
    }

    if (dump_info)
        dramc_dump_debug_info(p);

    show_msg((INFO, "PHY AO 0x%x = 0x%x\n", 0x510,
        io32_read_4b(DRAMC_REG_ADDR(DDRPHY_MISC_STBERR_RK0_R))));

    return err_count ? DRAM_FAIL : DRAM_OK;
}
#endif

void set_mdl_used_flag(unsigned char value)
{
	get_mdl_used_flag_value = value;
}

unsigned char get_mdl_used_flag(void)
{
	return get_mdl_used_flag_value;
}

#if TX_K_DQM_WITH_WDBI
void switch_write_dbi_settings(DRAMC_CTX_T *p, unsigned char on_off)
{
	signed char tx_shift_ui;

	tx_shift_ui = (on_off) ? -TX_SHIFT_OFFSET : TX_SHIFT_OFFSET;
	/* Tx DQ/DQM -1 MCK for write DBI ON */
	dramc_write_minus_1mck_for_write_dbi(p, tx_shift_ui);

	set_dram_mode_reg_for_write_dbi_on_off(p, on_off);
	dramc_write_dbi_on_off(p, on_off);
}
#endif

void dump_ddrphy_reg(unsigned char channel)
{
	unsigned int base, addr, len;

	if (channel == CHANNEL_A)
		base = Channel_A_PHY_AO_BASE_ADDRESS;
	else
		base = Channel_B_PHY_AO_BASE_ADDRESS;

	len = 0x3000;

	show_msg((INFO, "=============== Dump PHY REG ============\n"));
	for (addr = base; addr < (base + len); addr += 4) {
		show_msg((INFO, "Reg(0x%xh) Address 0x%X = 0x%X\n",
			(addr - base)/4, addr, *((volatile unsigned int *)addr)));
		delay_us(1);
	}
}

void dump_dramc_reg(unsigned char channel)
{
	unsigned int base, addr, len;

	if (channel == CHANNEL_A)
		base = Channel_A_DRAMC_AO_BASE_ADDRESS;
	else
		base = Channel_B_DRAMC_AO_BASE_ADDRESS;

	len = 0x1000;

	show_msg((INFO, "=============== Dump DRAMC REG ============\n"));
	for (addr = base; addr < (base + len); addr += 4) {
		show_msg((INFO, "Reg(0x%xh) Address 0x%X = 0x%X\n",
			(addr - base)/4, addr, *((volatile unsigned int *)addr)));
		delay_us(1);
	}
}

static void __do_dramc_calibration(DRAMC_CTX_T *p)
{
	unsigned char rank_max;
	signed char rank_idx;

#if TX_K_DQM_WITH_WDBI
	unsigned char tx_vref_dq;
#endif
#if TX_K_DQM_WITH_WDBI
	unsigned char tx_vref_dqm, tx_final_vref = BYTE_MAX, dqm_spec_result;
#endif
	rank_max = (p->support_rank_num == RANK_DUAL) ? (RANK_MAX) : RANK_1;

	for (rank_idx = RANK_0; rank_idx < rank_max; rank_idx++) {
		set_rank(p, rank_idx);
	#if SIMULATION_WRITE_LEVELING
		dramc_write_leveling((DRAMC_CTX_T *) p);
	#endif
		/*
		 * when doing gating, RX and TX calibration,
		 * auto refresh should be enable
		 */

		auto_refresh_switch(p, ENABLE);

	//dump_ddrphy_reg(p->channel);
	//dump_dramc_reg(p->channel);

	#if SIMULATION_GATING
		dramc_rx_dqs_gating_cal(p);
	#endif

	#if SIMULATION_RX_PERBIT
		if (is_lp4_family(p) || p->dram_type == TYPE_PCDDR4)
			dramc_rx_window_perbit_cal((DRAMC_CTX_T *) p, DISABLE);
		else
			dramc_rx_window_perbit_cal((DRAMC_CTX_T *) p, ENABLE);
	#endif

#if SIMULATION_TX_PERBIT
#if TX_K_DQM_WITH_WDBI
		dramc_write_dbi_on_off(p, DBI_OFF);
#endif
		dramc_tx_window_perbit_cal((DRAMC_CTX_T *) p,
			TX_DQ_DQS_MOVE_DQ_DQM, FALSE);

#if TX_K_DQM_WITH_WDBI
		tx_vref_dq = dramc_tx_window_perbit_cal((DRAMC_CTX_T *) p,
			TX_DQ_DQS_MOVE_DQ_ONLY, p->enable_tx_scan_vref);
#else
		dramc_tx_window_perbit_cal((DRAMC_CTX_T *) p,
			TX_DQ_DQS_MOVE_DQ_ONLY, p->enable_tx_scan_vref);
#endif

#if TX_K_DQM_WITH_WDBI
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
        if(p->Bypass_TXWINDOW==0) //if bypass TX K, DQM will be calculate form DQ. (no need to K DQM)
#endif
		{
			if ((p->dbi_w_onoff[p->dram_fsp]) == DBI_ON) {
			show_msg((INFO, "[TX_K_DQM_WITH_WDBI] K DQM with DBI_ON "));
			show_msg((INFO, "and check DQM window spec.\n\n"));
				switch_write_dbi_settings(p, DBI_ON);
				dqm_spec_result =
					dramc_tx_window_perbit_cal((DRAMC_CTX_T *) p,
					TX_DQ_DQS_MOVE_DQM_ONLY, FALSE);

				if (dqm_spec_result == TX_DQM_WINDOW_SPEC_IN) {
					switch_write_dbi_settings(p, DBI_OFF);
				} else { /* Scan DQM + Vref */
				show_msg((INFO, "K DQM and Vref with DBI_ON\n\n"));
					tx_vref_dqm = dramc_tx_window_perbit_cal(
						(DRAMC_CTX_T *) p,
						TX_DQ_DQS_MOVE_DQM_ONLY, TRUE);

					tx_final_vref = (tx_vref_dq + tx_vref_dqm) >> 1;

					dramc_tx_set_vref(p, 0, tx_final_vref);
					show_msg((INFO, "tx_vref_dq=%d,", tx_vref_dq));
					show_msg((INFO,
						" tx_vref_dqm %d, Set MR14=%d\n\n",
						 tx_vref_dqm, tx_final_vref));

				show_msg((INFO, "Scan DQM\n\n"));
					dramc_tx_window_perbit_cal((DRAMC_CTX_T *) p,
						TX_DQ_DQS_MOVE_DQM_ONLY, FALSE);

					/* Write DBI off + Scan DQ */
				show_msg((INFO, "Write DBI off + Scan DQ\n\n"));
					switch_write_dbi_settings(p, DBI_OFF);
					dramc_tx_window_perbit_cal((DRAMC_CTX_T *) p,
						TX_DQ_DQS_MOVE_DQ_ONLY, FALSE);
				}
			}
		}
#endif
#endif /* SIMULATION_TX_PERBIT */
#if (CONFIG_EYESCAN_LOG == 1)
	Dramc_K_TX_EyeScan_Log(p);
	print_EYESCAN_LOG_message(p, 2); //draw TX eyescan
#endif

	#if SIMULATION_DATLAT
		dramc_rxdatlat_cal((DRAMC_CTX_T *) p);
	#endif

	#if SIMULATION_RX_PERBIT
		if (is_lp4_family(p) || p->dram_type == TYPE_PCDDR4)
			dramc_rx_window_perbit_cal((DRAMC_CTX_T *) p, ENABLE);
#if (CONFIG_EYESCAN_LOG == 1)
		print_EYESCAN_LOG_message(p, 1); //draw RX eyescan
#endif
	#endif

		/* cc notes, for non-lp4 type, calibrate TX OE may cause
		 * model enter Error state which will lead to simulation
		 * FAIL.
		 * For chip, this issue may not exist, that is SW can try
		 * to calibrate TX OE for all types.
		 */
		if (is_lp4_family(p))
			dramc_tx_oe_calibration(p);
		/*
		* After gating, Rx and Tx calibration,
		* auto refresh should be disable
		*/
		auto_refresh_switch(p, DISABLE);
	#if ENABLE_TX_TRACKING
		if (is_lp4_family(p)) {
			dramc_dqsosc_auto(p);
			dramc_dqsosc_mr23(p);
			dramc_dqsosc_set_mr18_mr19(p);
		}
	#endif

	}
}

static void do_dramc_calibration(DRAMC_CTX_T *p)
{
	unsigned char rank_max;
	signed char rank_idx;

#if GATING_ADJUST_TXDLY_FOR_TRACKING
	if (((p->dram_type == TYPE_PCDDR4) && (p->frequency < DDR2666_FREQ)) ||
		(p->dram_type == TYPE_PCDDR3))
		show_log("GATING_ADJUST_TXDLY_FOR_TRACKING: disabled\n");/* PCDDR4 needn't TXDLY TRACKING. */
	else
		dramc_rxdqs_gating_pre_process(p);
#endif

	rank_max = (p->support_rank_num == RANK_DUAL) ? (RANK_MAX) : RANK_1;

#if SIMULATION_CBT
	if (is_lp4_family(p) || (p->dram_type == TYPE_LPDDR3)) {
		for (rank_idx = RANK_0; rank_idx < rank_max; rank_idx++) {
			set_rank(p, rank_idx);
			cmd_bus_training(p);
#if (CONFIG_EYESCAN_LOG == 1)
		print_EYESCAN_LOG_message(p, 0); //draw CBT eyescan
#endif
		}
	}
	set_rank(p, RANK_0);
#endif

#if DUAL_FREQ_K
	no_parking_on_clrpll(p);
#endif

	__do_dramc_calibration(p);

	set_rank(p, RANK_0);

#if ENABLE_TX_TRACKING
	if (is_lp4_family(p))
		dramc_dqsosc_shu_settings(p);
#endif

#if GATING_ADJUST_TXDLY_FOR_TRACKING
	if (((p->dram_type == TYPE_PCDDR4) && (p->frequency < DDR2666_FREQ)) ||
		(p->dram_type == TYPE_PCDDR3))
		show_log("GATING_ADJUST_TXDLY_FOR_TRACKING: disabled\n");/* PCDDR4 needn't TXDLY TRACKING. */
	else
		dramc_rxdqs_gating_post_process(p);
#endif

	if (p->support_rank_num == RANK_DUAL)
		dramc_dual_rank_rxdatlat_cal(p);

}

static void dram_calibration_single_channel(DRAMC_CTX_T *p)
{
	do_dramc_calibration(p);
}

static void dram_calibration_all_channel(DRAMC_CTX_T *p)
{
#if (ENABLE_WRITE_DBI)
	unsigned char rank_idx;
#endif
	unsigned char channel_idx;

	cke_fix_on_off(p, CKE_WRITE_TO_ALL_RANK, CKE_DYNAMIC, CKE_WRITE_TO_ALL_CHANNEL);
	for (channel_idx = CHANNEL_A; channel_idx < p->support_channel_num;
		channel_idx++) {
		/*
		 * when switching channel,
		 * must update PHY to Channel Mapping
		 */
		show_msg3((INFO, "cal_all_channel: cal CH%d\n", channel_idx));

		set_phy_2_channel_mapping(p, channel_idx);
		cke_fix_on_off(p, CKE_WRITE_TO_ALL_RANK, CKE_FIXON, CKE_WRITE_TO_ONE_CHANNEL);
		dram_calibration_single_channel(p);
	}

	set_phy_2_channel_mapping(p, CHANNEL_A);

#if PRINT_CALIBRATION_SUMMARY
	print_calibration_result(p);
#endif

#if ENABLE_READ_DBI
	enable_dram_mode_reg_for_read_dbi_after_cal(p);

	set_phy_2_channel_mapping(p, CHANNEL_A);
	set_rank(p, RANK_0);
	dramc_read_dbi_on_off(p->dbi_r_onoff[p->dram_fsp]);
#endif

#if ENABLE_WRITE_DBI
	for (channel_idx = CHANNEL_A; channel_idx < p->support_channel_num;
		channel_idx++) {
		set_phy_2_channel_mapping(p, channel_idx);

		for (rank_idx = RANK_0;
			rank_idx < p->support_rank_num; rank_idx++) {
			set_rank(p, rank_idx);

			dramc_write_minus_1mck_for_write_dbi(p, -8);

			set_dram_mode_reg_for_write_dbi_on_off(p,
				p->dbi_w_onoff[p->dram_fsp]);
		}
		set_rank(p, RANK_0);
	}
	set_phy_2_channel_mapping(p, CHANNEL_A);

	dramc_write_dbi_on_off(p, p->dbi_w_onoff[p->dram_fsp]);

	apply_write_dbi_power_improve(p, ENABLE);
#endif

#if TX_PICG_NEW_MODE
	tx_picg_setting(p);
#endif
}

int get_dram_info_after_cal_by_mrr(DRAMC_CTX_T *p,
    DRAM_INFO_BY_MRR_T *dram_info)
{
    unsigned char ch_idx, rank_idx;
    unsigned short mr8, density, io_width;
    unsigned long long size = 0;
    unsigned long long channel_size = 0;

    show_msg((INFO, "[get_dram_info_after_cal_by_mrr]\n"));
    set_phy_2_channel_mapping(p, CHANNEL_A);

    dramc_mrr_by_rank(p, RANK_0, MR05, &(p->vendor_id));
    show_msg((INFO, "Vendor %x.\n", p->vendor_id));

    dramc_mrr_by_rank(p, RANK_0, MR06, &(p->revision_id));
    show_msg((INFO, "Revision %x.\n", p->revision_id));

    if (dram_info != NULL) {
        dram_info->mr5_vendor_id = p->vendor_id;
        dram_info->mr6_vevision_id = p->revision_id;

        for (ch_idx = 0; ch_idx < (p->support_channel_num); ch_idx++)
            for (rank_idx = 0; rank_idx < RANK_MAX; rank_idx++)
                dram_info->mr8_density[ch_idx][rank_idx] = 0;
    }
    for (rank_idx = 0; rank_idx < (p->support_rank_num); rank_idx++) {
        size = 0;
        for (ch_idx = 0; ch_idx < (p->support_channel_num); ch_idx++) {
            set_phy_2_channel_mapping(p, ch_idx);
            dramc_mrr_by_rank(p, rank_idx, 8, &mr8);
            show_msg((INFO, "MR8 %x\n", mr8));

            density = (mr8 >> 2) & 0xf;
            io_width = (mr8 >> 6) & 0x3;
            if (is_lp4_family(p)) {
                switch (density) {
                case channel_density_2Gb:
                    channel_size = 0x10000000;
                    break;
                case channel_density_3Gb:
                    channel_size = 0x18000000;
                    break;
                case channel_density_4Gb:
                    channel_size = 0x20000000;
                    break;
                case channel_density_6Gb:
                    channel_size = 0x30000000;
                    break;
                case channel_density_8Gb:
                    channel_size = 0x40000000;
                    break;
                case channel_density_12Gb:
                    channel_size = 0x60000000;
                    break;
                case channel_density_16Gb:
                    channel_size = 0x80000000;
                    break;
                default:
                    channel_size = 0;
                    break;
                }
            } else if (p->dram_type == TYPE_LPDDR3) {
                switch (density) {
                case 0x4:       // 1Gb
                    channel_size = 0x08000000;
                    break;
                case 0x5:       // 2Gb
                    channel_size = 0x10000000;
                    break;
                case 0x6:       // 4Gb
                    channel_size = 0x20000000;
                    break;
                case 0xE:       // 6Gb
                    channel_size = 0x30000000;
                    break;
                case 0x7:       // 8Gb
                    channel_size = 0x40000000;
                    break;
                case 0xD:       // 12Gb
                    channel_size = 0x60000000;
                    break;
                case 0x8:       // 16Gb
                    channel_size = 0x80000000;
                    break;
                case 0x9:       // 32Gb
                    channel_size = 0x100000000;
                    break;
                default:
                    channel_size = 0;
                    break;
                }
                if (io_width == 1)  // x16
                    channel_size *= 2;
            }
            size += channel_size;
            p->density = density;
            show_msg0((INFO,
                "CH%d, RK%d, Rank Size: 0x%llx.\n",
                ch_idx, rank_idx, channel_size));
        }
        p->ranksize[rank_idx] = size;
        if (dram_info != NULL)
            dram_info->mr8_density[ch_idx][rank_idx] = size;
    }

    set_phy_2_channel_mapping(p, CHANNEL_A);

    return DRAM_OK;
}

#if ENABLE_RANK_NUMBER_AUTO_DETECTION
void dram_rank_number_detection(DRAMC_CTX_T *p)
{
    unsigned char rank_bak;

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_WRITELEVELING)
	if(p->femmc_Ready==1)
	{
		p->support_rank_num = p->pSavetimeData->u1RankNum - 1;
		show_msg((INFO, "[RankNumberDetection] %d\n", p->support_rank_num));
		return;
	}
#endif

    rank_bak = get_rank(p);  // backup current rank setting

    set_phy_2_channel_mapping(p, CHANNEL_A); // when switching channel, must update PHY to Channel Mapping
    set_rank(p, RANK_1);

    if(dramc_write_leveling((DRAMC_CTX_T *) p) == DRAM_OK)//Dram will be reset when finish write leveling
    {
        p->support_rank_num = RANK_DUAL;
    }
    else
    {
        p->support_rank_num = RANK_SINGLE;
    }
    show_msg((INFO, "[RankNumberDetection] %d\n", p->support_rank_num));

    set_rank(p, rank_bak);  // restore rank setting

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_WRITELEVELING)
	if(p->femmc_Ready==0)
	{
		p->pSavetimeData->u1RankNum = p->support_rank_num + 1;
	}
#endif
	if (p->support_rank_num == RANK_DUAL)
		dfs_init_for_calibration(p);  // Restore setting after rank dection (especially DQ= DQS+16)
}
#endif

#ifdef ENABLE_MIOCK_JMETER
void set_pre_miock_jmeter_hqa_used_flag(unsigned char value)
{
	pre_miock_jmeter_hqa_used_flag = value;
}

unsigned char get_pre_miock_jmeter_hqa_used_flag(void)
{
	return pre_miock_jmeter_hqa_used_flag;
}
extern unsigned char num_dlycell_per_t_all[DRAM_DFS_SHUFFLE_MAX][CHANNEL_NUM];
extern unsigned short delay_cell_ps_all[DRAM_DFS_SHUFFLE_MAX][CHANNEL_NUM];

void pre_miock_jmeter_hqa_used(DRAMC_CTX_T *p)
{
	unsigned int backup_freq_sel, backup_channel;
	unsigned int channel_idx;

	backup_freq_sel = p->freq_sel;
	backup_channel = p->channel;

	show_msg3((INFO, "[JMETER_HQA]\n"));

#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_JME)
		if(p->femmc_Ready==1)
		{
			for(channel_idx=CHANNEL_A; channel_idx<p->support_channel_num; channel_idx++)
			{
				//for (shuffleIdx = DRAM_DFS_SHUFFLE_1; shuffleIdx < DRAM_DFS_SHUFFLE_MAX; shuffleIdx++)
				{
					num_dlycell_per_t_all[p->shu_type][channel_idx] = p->pSavetimeData->ucnum_dlycell_perT;
					delay_cell_ps_all[p->shu_type][channel_idx] = p->pSavetimeData->u2DelayCellTimex100;
				}
			}

			p->ucnum_dlycell_perT = p->pSavetimeData->ucnum_dlycell_perT;
			p->delay_cell_timex100 = p->pSavetimeData->u2DelayCellTimex100;
			show_msg3((INFO, "DelayCellTimex100 CH_%d, (cell=%d)\n", p->channel, p->delay_cell_timex100));
			return;
		}
#endif
	set_pre_miock_jmeter_hqa_used_flag(1);

	set_phy_2_channel_mapping(p, CHANNEL_A);
	dramc_miock_jmeter_hqa(p);
#if (SUPPORT_SAVE_TIME_FOR_CALIBRATION && BYPASS_JME)
		if(p->femmc_Ready==0)
		{
        #if 0
			for(channel_idx=CHANNEL_A; channel_idx<p->support_channel_num; channel_idx++)
			{
				for (shuffleIdx = DRAM_DFS_SHUFFLE_1; shuffleIdx < DRAM_DFS_SHUFFLE_MAX; shuffleIdx++)
				{
					p->pSavetimeData->ucg_num_dlycell_perT_all[channel_idx] = ucg_num_dlycell_perT_all[p->shu_type][channel_idx];
					p->pSavetimeData->u2gdelay_cell_ps_all[channel_idx] = u2gdelay_cell_ps_all[p->shu_type][channel_idx];
				}
			}
        #endif
			p->pSavetimeData->ucnum_dlycell_perT = p->ucnum_dlycell_perT;
			p->pSavetimeData->u2DelayCellTimex100 = p->delay_cell_timex100;
		}
#endif
	set_phy_2_channel_mapping(p, backup_channel);

	set_pre_miock_jmeter_hqa_used_flag(0);

	p->freq_sel = backup_freq_sel;
}
#endif
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
#if !EMMC_READY
u32 g_dram_save_time_init_done[DRAM_DFS_SHUFFLE_MAX]={0};
SAVE_TIME_FOR_CALIBRATION_T SaveTimeDataByShuffle[DRAM_DFS_SHUFFLE_MAX];
#endif

DRAM_STATUS_T DramcSave_Time_For_Cal_End(DRAMC_CTX_T *p)
{
    if(!is_need_do_fast_k(p))
        return DRAM_FAIL;

    if(is_need_do_fast_k(p) && (p->femmc_Ready==0))
    {
        #if EMMC_READY
        write_offline_dram_calibration_data(p->shu_type, p->pSavetimeData);
        mcSHOW_DBG_MSG(("[FAST_K] Save calibration result to emmc\n"));
        #else
        g_dram_save_time_init_done[p->shu_type] =1;
        memcpy(&(SaveTimeDataByShuffle[p->shu_type]), p->pSavetimeData, sizeof(SAVE_TIME_FOR_CALIBRATION_T));
        mcSHOW_DBG_MSG(("[FAST_K] Save calibration result to SW memory\n"));
        #endif
    }
    else
    {
        mcSHOW_DBG_MSG(("[FAST_K] Bypass saving calibration result to emmc\n"));
    }

	return DRAM_OK;
}

DRAM_STATUS_T DramcSave_Time_For_Cal_Init(DRAMC_CTX_T *p)
{
    if(!is_need_do_fast_k(p))
        return DRAM_FAIL;

    // Parepare fask k data
    #if EMMC_READY
    // scy: only need to read emmc one time for each boot-up
    //if (g_dram_save_time_init_done == 1)
    //    return DRAM_OK;
    //else
    //    g_dram_save_time_init_done = 1;
    if(read_offline_dram_calibration_data(p->shu_type, p->pSavetimeData)<0)
    {
        p->femmc_Ready=0;
        memset(p->pSavetimeData, 0, sizeof(SAVE_TIME_FOR_CALIBRATION_T));
    }
    else
    {
        p->femmc_Ready=1;
    }

	//get_offline_dram_calibration_data(p);

    #else //EMMC is not avaliable, load off-line data

    if(g_dram_save_time_init_done[p->shu_type] ==0)
    {
        p->femmc_Ready=0;
        memset(p->pSavetimeData, 0, sizeof(SAVE_TIME_FOR_CALIBRATION_T));
    }
    else
    {
        memcpy(p->pSavetimeData, &(SaveTimeDataByShuffle[p->shu_type]), sizeof(SAVE_TIME_FOR_CALIBRATION_T));
        p->femmc_Ready=1;
    }
    #endif

    if(p->femmc_Ready==1)
    {
#if SUPPORT_TYPE_PCDDR4
        if(p->frequency <= DDR2666_FREQ)
#else
        if(p->frequency < DDR2666_FREQ)
#endif
        {   // freq < 1600, TX and RX tracking are disable. Therefore, bypass calibration.
            p->Bypass_RDDQC=1;
            p->Bypass_RXWINDOW=1;
            p->Bypass_TXWINDOW=1;
        }
        else
        {
            p->Bypass_RDDQC=0;
            p->Bypass_RXWINDOW=0;
            p->Bypass_TXWINDOW=0;
        }

#if RUNTIME_SHMOO_RELEATED_FUNCTION
        p->Bypass_RDDQC=1;
        p->Bypass_RXWINDOW=1;
        p->Bypass_TXWINDOW=1;
#endif
    }

#if EMMC_READY
    mcSHOW_DBG_MSG(("[FAST_K] DramcSave_Time_For_Cal_Init SHU%d, femmc_Ready=%d\n", p->shu_type, p->femmc_Ready));
#else
    mcSHOW_DBG_MSG(("[FAST_K] DramcSave_Time_For_Cal_Init SHU%d, Init_done=%d, femmc_Ready=%d\n", p->shu_type, g_dram_save_time_init_done[p->shu_type], p->femmc_Ready));
#endif
    mcSHOW_DBG_MSG(("[FAST_K] Bypass_RDDQC %d, Bypass_RXWINDOW=%d, Bypass_TXWINDOW=%d\n", p->Bypass_RDDQC, p->Bypass_RXWINDOW, p->Bypass_TXWINDOW));

    return DRAM_OK;
}

void dram_last_exception_detection(DRAMC_CTX_T * p)
{
	DRAM_CHANNEL_T bk_ch, chn;
	unsigned int wdt_dbg_signal, ret = 0;
	unsigned char backup_broadcast;

	if(!is_need_do_fast_k(p))
		return;

	if ((g_boot_reason == BR_POWER_KEY) || (g_boot_reason == BR_USB)
			|| mtk_wdt_is_pmic_full_reset() ){
		print("[dramc]cold boot\n");
		return;
	}

#ifndef DDR_RESERVE_MODE
	if (g_boot_reason == BR_UNKNOWN) {
		print("[dramc]kernel panic\n");
		clean_dram_calibration_data();
		return;
	}
#endif

	/*check last dramc status*/
	backup_broadcast = get_dramc_broadcast();
	dramc_broadcast_on_off(DRAMC_BROADCAST_OFF);

	bk_ch = get_phy_2_channel_mapping(p);
	for (chn = CHANNEL_A; chn < p->support_channel_num; chn++) {
		set_phy_2_channel_mapping(p, chn);
		wdt_dbg_signal = io32_read_4b(DRAMC_REG_ADDR(DRAMC_REG_WDT_DBG_SIGNAL));
		print("[dramc]wdt_dbg_signal[%d]=0x%x\n", chn, wdt_dbg_signal);
		if (wdt_dbg_signal & ((1<<10) | (3<<14))) {
				ret |= (1 << chn);
		}
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DDRCONF0), SET_FLD, DDRCONF0_WDT_DBG_RST);
		udelay(1);
		io_32_write_fld_align(DRAMC_REG_ADDR(DRAMC_REG_DDRCONF0), CLEAR_FLD, DDRCONF0_WDT_DBG_RST);
	}
	set_phy_2_channel_mapping(p, bk_ch);
	dramc_broadcast_on_off(backup_broadcast);

	if(ret) {
		print("[dramc]found last dramc  error\n");
		clean_dram_calibration_data();
	}
}

#endif

static int dram_calibration_main(DRAMC_CTX_T *p,
	DRAM_INFO_BY_MRR_T *dram_info)
{
	if ((get_mdl_used_flag()) == GET_MDL_USED) {
		set_phy_2_channel_mapping(p, CHANNEL_A);
		dram_calibration_single_channel(p);
		get_dram_info_after_cal_by_mrr(p, dram_info);
		return DRAM_OK;
	}
	dram_calibration_all_channel(p);

	if (is_lp4_family(p)) {
		get_dram_info_after_cal_by_mrr(p, NULL);
		dramc_ac_timing_optimize(p);
	} else if (p->dram_type == TYPE_LPDDR3)
	    get_dram_info_after_cal_by_mrr(p, NULL);

#if DUMP_AO_REG
	unsigned char channel;
	show_msg((INFO, "Dump register for DDR%d\n", p->frequency << 1));
	for (channel = CHANNEL_A; channel < CHANNEL_NUM; channel++) {
		dump_ddrphy_reg(channel);
		dump_dramc_reg(channel);
	}
#endif

#if DUAL_FREQ_K
	if (is_lp4_family(p)) {
#if SUPPORT_TYPE_LPDDR4
		dramc_save_to_shuffle_reg(p, DRAM_DFS_SHUFFLE_1, DRAM_DFS_SHUFFLE_3);
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
        DramcSave_Time_For_Cal_End(p);
#endif

		/* Second Freq */
		set_freq_sel_by_table(p, &freq_tbl_lp4[DRAM_DFS_SHUFFLE_2]);
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
		p->shu_type = DRAM_DFS_SHUFFLE_2;
        DramcSave_Time_For_Cal_Init(p);
#endif
		dfs_init_for_calibration(p);
		dram_calibration_all_channel(p);
		dramc_ac_timing_optimize(p);
#if DUMP_AO_REG
		show_msg((INFO, "Dump register for DDR%d\n", p->frequency << 1));
		for (channel = CHANNEL_A; channel < CHANNEL_NUM; channel++) {
			dump_ddrphy_reg(channel);
			dump_dramc_reg(channel);
		}
#endif
		dramc_save_to_shuffle_reg(p, DRAM_DFS_SHUFFLE_1, DRAM_DFS_SHUFFLE_2);
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
        DramcSave_Time_For_Cal_End(p);
#endif

		/* Third Freq */
		set_freq_sel_by_table(p, &freq_tbl_lp4[DRAM_DFS_SHUFFLE_1]);
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
		p->shu_type = DRAM_DFS_SHUFFLE_1;
        DramcSave_Time_For_Cal_Init(p);
#endif
		dfs_init_for_calibration(p);
		dram_calibration_all_channel(p);
		dramc_ac_timing_optimize(p);
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
        DramcSave_Time_For_Cal_End(p);
#endif
#endif
	}else if(p->dram_type == TYPE_LPDDR3){
#if SUPPORT_TYPE_LPDDR3
		dramc_save_to_shuffle_reg(p, DRAM_DFS_SHUFFLE_1, DRAM_DFS_SHUFFLE_3);

		/* Second Freq */
		set_freq_sel_by_table(p, &freq_tbl_lp3[DRAM_DFS_SHUFFLE_2]);
		dfs_init_for_calibration(p);
		dram_calibration_all_channel(p);
		dramc_save_to_shuffle_reg(p, DRAM_DFS_SHUFFLE_1, DRAM_DFS_SHUFFLE_2);

		/* Third Freq */
		set_freq_sel_by_table(p, &freq_tbl_lp3[DRAM_DFS_SHUFFLE_1]);
		dfs_init_for_calibration(p);
		dram_calibration_all_channel(p);
#endif
	} else {
#if SUPPORT_TYPE_PCDDR3 || SUPPORT_TYPE_PCDDR4
		dramc_save_to_shuffle_reg(p, DRAM_DFS_SHUFFLE_1, DRAM_DFS_SHUFFLE_2);
		dramc_save_to_shuffle_reg(p, DRAM_DFS_SHUFFLE_1, DRAM_DFS_SHUFFLE_3);
#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
		if(p->dram_type == TYPE_PCDDR4)
			DramcSave_Time_For_Cal_End(p);
#endif
#endif
	}
#else

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION && SUPPORT_TYPE_LPDDR4
    DramcSave_Time_For_Cal_End(p);
#endif

#endif
	apply_config_after_calibration(p);

	return DRAM_OK;
}

static void dram_check(DRAMC_CTX_T *p)
{
	DRAM_STATUS_T ret = DRAM_OK;

#if DRAMC_MODEREG_CHECK
	dramc_mode_reg_check(p);
#endif

#if CPU_RW_TEST_AFTER_K
	show_msg2((INFO,
	"\n[MEM_TEST] 02: After DFS, before run time config\n"));
	ret |= dram_cpu_read_write_test_after_calibration(p);
#endif

#if TA2_RW_TEST_AFTER_K
	show_msg((INFO, "\n[TA2_TEST] freq %d\n", p->frequency));
	ret |= ta2_test_run_time_hw(p);
#endif

	show_msg3((INFO, "\n\nSettings after calibration\n\n"));
	dramc_run_time_config(p);

#if CPU_RW_TEST_AFTER_K
	show_msg((INFO, "\n[MEM_TEST] 03: After run time config\n"));
	ret |= dram_cpu_read_write_test_after_calibration(p);
#endif

#if TA2_RW_TEST_AFTER_K
	show_msg((INFO, "\n[TA2_TEST] freq %d\n", p->frequency));
	ret |= ta2_test_run_time_hw(p);
#endif

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	if((ret != DRAM_OK) && is_need_do_fast_k(p)) {
		show_diag((INFO, "erase fastK para because of dram fail\n"));
		clean_dram_calibration_data();
	}
#endif

}

#ifdef DRAM_SLT
static unsigned int slt_result[CHANNEL_NUM][RANK_MAX] = {0};
void dram_slt_set(DRAMC_CTX_T *p, unsigned char cal_type,
	unsigned char result)
{
	if(result == DRAM_FAIL)
		slt_result[p->channel][p->rank] |= (1<< cal_type);
}
void dram_slt_check(DRAMC_CTX_T *p)
{
	unsigned int result;
	unsigned int ch, rk;

	result = 0;
	for(ch=CHANNEL_A; ch<p->support_channel_num; ch++){
		for(rk=RANK_0; rk<p->support_rank_num; rk++){
			if(slt_result[ch][rk]){
				show_msg0((INFO, "[DRAM SLT Fail] %d/%d: %x\n", ch, rk, slt_result[ch][rk]));
				result = 1;
			}
		}
	}

	if(result) {
		show_msg0((INFO, "[DRAM SLT Fail]\n"));
		while(1);
	} else {
		show_msg0((INFO, "[DRAM SLT Pass]\n"));
	}

}
#endif


extern unsigned int pin_mux_setting;

static void dump_dramc_ctx_t(DRAMC_CTX_T *p)
{
	show_msg3((INFO, "---------- DRAMC_CTX_T ------------\n"));;
	show_msg3((INFO, "support_channel_num: %d\n", p->support_channel_num));
	show_msg3((INFO, "channel: %d\n", p->channel));
	show_msg3((INFO, "support_rank_num: %d\n", p->support_rank_num));
	show_msg3((INFO, "rank: %d\n", p->rank));
	show_msg3((INFO, "freq_sel: %d\n", p->freq_sel));
	show_msg3((INFO, "shu_type: %d\n", p->shu_type));
	show_msg3((INFO, "dram_type: %d\n", p->dram_type));
	show_msg3((INFO, "dram_fsp: %d\n", p->dram_fsp));
	show_msg3((INFO, "odt_onoff: %d\n", p->odt_onoff));
	show_msg3((INFO, "cbt_mode: %d\n", p->dram_cbt_mode[p->rank]));
	show_msg3((INFO, "dbi_r_onoff: %d\n", p->dbi_r_onoff[p->dram_fsp]));
	show_msg3((INFO, "dbi_w_onoff: %d\n", p->dbi_w_onoff[p->dram_fsp]));
	show_msg3((INFO, "data_width: %d\n", p->data_width));
	show_msg3((INFO, "frequency: %d\n", p->frequency));
	show_msg3((INFO, "freqGroup: %d\n", p->freqGroup));
		if(is_lp4_family(p))
	show_msg3((INFO, "eMCP setting: %d\n", pin_mux_setting));
	show_msg3((INFO, "-----------------------------------\n"));
}

DRAM_ADC_SIZE_T get_dram_size_by_adc(void)
{
    int ret = 0, data[4] = {0,0,0,0}, voltage = 0, rawvalue = 0;
    DRAM_ADC_SIZE_T size_lev;

    ret = IMM_GetOneChannelValue(1,  data, &rawvalue);

    if (ret == 0)
    {
        voltage = rawvalue * 1500 / 4096;
        if (voltage < 180)
            size_lev = DRAM_ADC_SIZE_LEVEL_0;
        else if ((voltage >= 180) && (voltage < 540))
            size_lev = DRAM_ADC_SIZE_LEVEL_1;
        else if ((voltage >= 540) && (voltage < 900))
            size_lev = DRAM_ADC_SIZE_LEVEL_2;
        else if ((voltage >= 900) && (voltage < 1260))
            size_lev = DRAM_ADC_SIZE_LEVEL_3;
        else if ((voltage > 1260) && (voltage <= 1450))
            size_lev = DRAM_ADC_SIZE_LEVEL_4;
        else
            size_lev = DRAM_ADC_SIZE_MAX;
    }
    else
    {
        size_lev = DRAM_ADC_SIZE_MAX;
        print("get raw value error %d \n", ret);
    }

    mcSHOW_DBG_MSG(("get dram size from AUXADC1 %d %d\n", voltage, size_lev));

    return size_lev;
}

U8 mt_get_dram_comsetting_type(EMI_SETTINGS *emi_set)
{
	U8 u1dramcom_type = DRAM_COMSETTING_DEFAULT;

	if(emi_set->PIN_MUX_TYPE == PIN_MUX_TYPE_DDR3X16X2)
	{//DDR3
		if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 31))
		{//x32
			u1dramcom_type = DRAM_COMSETTING_DDR3_X32;
		}
		else if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 30))
		{//x16
			u1dramcom_type = DRAM_COMSETTING_DDR3_X16;
		}
		else if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 29))
		{//x8

		}
		else if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 28))
		{//ASYM
			u1dramcom_type = DRAM_COMSETTING_DDR3_ASYM;
		}
	}
	else if(emi_set->PIN_MUX_TYPE == PIN_MUX_TYPE_DDR4X16X2)
	{//DDR4
		if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 31))
		{//x32
			u1dramcom_type = DRAM_COMSETTING_DDR4_X32;
		}
		else if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 30))
		{//x16
			u1dramcom_type = DRAM_COMSETTING_DDR4_X16;
		}
		else if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 29))
		{//x8

		}
		else if(emi_set->iLPDDR3_MODE_REG_5 & (0x1 << 28))
		{//ASYM
			u1dramcom_type = DRAM_COMSETTING_DDR4_ASYM;
		}
	}
	else if(emi_set->PIN_MUX_TYPE == PIN_MUX_TYPE_LPDDR3_178BALL)
	{//LP3_178
		u1dramcom_type = DRAM_COMSETTING_LP3_178BALL;
	}
	else if(emi_set->PIN_MUX_TYPE == PIN_MUX_TYPE_LPDDR3_168BALL)
	{//LP3_168
		u1dramcom_type = DRAM_COMSETTING_LP3_168BALL;
	}
	else if(emi_set->PIN_MUX_TYPE == PIN_MUX_TYPE_DDR3X8)
	{//DDR3x8
		u1dramcom_type = DRAM_COMSETTING_DDR3_X32;
	}
	else if(emi_set->PIN_MUX_TYPE == PIN_MUX_TYPE_DDR3X4)
	{//DDR3x4
		u1dramcom_type = DRAM_COMSETTING_DDR3_X4;
	}

	return u1dramcom_type;
}

DRAM_DRAM_TYPE_T get_LP4_type_by_adc(void)
{
	int ret = 0, data[4] = {0,0,0,0}, voltage = 0, rawvalue = 0;

	ret = IMM_GetOneChannelValue(1,  data, &rawvalue);
	if (ret == 0) {
		voltage = rawvalue * 1500 / 4096;

		if ((voltage >= 0) && (voltage < 540)) {
			show_msg3((INFO, "\n LPDDR4 from AUX_IN \n"));
			return TYPE_LPDDR4;
		} else {
			show_msg3((INFO, "\n LPDDR4X from AUX_IN \n"));
			return TYPE_LPDDR4X;
		}
	} else {
		print("get raw value error %d \n", ret);
		return TYPE_MAX;
	}
}

DRAM_DRAM_TYPE_T get_ddr_type(void)
{
	return ps_curr_dram_ctx->dram_type;
}

DRAM_COM_SETTINGS DRAM_ComSetting_DDR3X16[DRAM_ADC_SIZE_MAX] =
{
	//512MB need verify
	{0xA050A056, 0x0000A052, 0x0000A052, 0x04210000, 0x00000003},
	//256MB need verify
	{0xA050A056, 0x0000A052, 0x0000A052, 0x04210000, 0x00000003},
	//128MB need verify
	{0xA050A056, 0x0000A052, 0x0000A052, 0x04210000, 0x00000003},
	//1GB need verify
	{0xA050A056, 0x0000A052, 0x0000A052, 0x04210000, 0x00000003},
	//Reserved
	{0xA050A056, 0x0000A052, 0x0000A052, 0x04210000, 0x00000003},
};

DRAM_COM_SETTINGS DRAM_ComSetting_DDR3X32[DRAM_ADC_SIZE_MAX] =
{
	//512MB+512MB
	{0xA050A056, 0x0000A052, 0x0000A052, 0x04210000, 0x00000003},
	//256MB+256MB need verify
	{0xA050A056, 0x0000A052, 0x0000A052, 0x04210000, 0x00000003},
	//128MB+128MB need verify
	{0xA050A056, 0x0000A052, 0x0000A052, 0x04210000, 0x00000003},
	//1GB+1GB need verify
	{0xF050F056, 0x0000F052, 0x0000F052, 0x04210000, 0x00000003},
	//Reserved
	{0xA050A056, 0x0000A052, 0x0000A052, 0x04210000, 0x00000003},
};

DRAM_COM_SETTINGS DRAM_ComSetting_DDR4X16[DRAM_ADC_SIZE_MAX] =
{
	//1GB
	{0xf050f054, 0x0444f050, 0x0444f050, 0x00421000, 0x44440003},
	//512MB need verify
	{0xf050f154, 0x0444f050, 0x0444f050, 0x04210000, 0x44440003},
	//Reserved
	{0xf050f154, 0x0444f050, 0x0444f050, 0x04210000, 0x44440003},
	//Reserved
	{0xf050f154, 0x0444f050, 0x0444f050, 0x04210000, 0x44440003},
	//2GB
	{0xf150c054, 0x0444c054, 0x0444c054, 0x04210000, 0x48480013},
};

DRAM_COM_SETTINGS DRAM_ComSetting_DDR4X32[DRAM_ADC_SIZE_MAX] =
{
	//1GB+1GB
	{0xf050f154, 0x0444f050, 0x0444f050, 0x04210000, 0x44440003},
	//512MB+512MB need verify
	{0xe050e154, 0x0442e050, 0x0442e050, 0x04210000, 0x42420003},
	//1GB+512MB
	{0xf050f154, 0x0444f050, 0x0444f050, 0x04210000, 0x42440003},
	//2GB+1GB need verify
	{0xf050f154, 0x0444f050, 0x0444f050, 0x04210000, 0x44440003},
	//2GB+2GB
	{0xf150c154, 0x0444c054, 0x0444c054, 0x04210000, 0x48480013},
};

void EMI_Comsetting_Config(DRAMC_CTX_T *p)
{
	U8 u1dramcom_type;
	EMI_SETTINGS *emi_set;
	DRAM_ADC_SIZE_T dram_adc_size;
	DRAM_COM_SETTINGS *pDramCom = &DRAM_ComSetting_DDR3X32[0];
	DRAM_DRAM_TYPE_T dram_type;

	if (emi_setting_index == -1)
		emi_set = &default_emi_setting;
	else
		emi_set = &emi_settings[emi_setting_index];

	dram_type = emi_set->type;

	if(dram_type == TYPE_LPDDR3 || is_lp4_family (p))
    {//LP series don't care ADC pin
        return ;
    }
    else
    {//PC3 & PC4 need check ADC pin
        dram_adc_size = get_dram_size_by_adc();
		u1dramcom_type = mt_get_dram_comsetting_type(emi_set);
    }
	if(dram_adc_size == DRAM_ADC_SIZE_MAX)
	{
		mcSHOW_ERR_MSG(("get dram size index illegal!\n"));
		return ;
	}
	switch (u1dramcom_type)
	{
		case DRAM_COMSETTING_DDR3_X4:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR3 x4 "));
		//	pDramCom = &DRAM_ComSetting_DDR3X4[dram_adc_size];
			break;
		case DRAM_COMSETTING_DDR3_X8:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR3 x8 "));
		//	pDramCom = &DRAM_ComSetting_DDR3X8[dram_adc_size];
			break;
		case DRAM_COMSETTING_DDR3_X16:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR3 x16 "));
			pDramCom = &DRAM_ComSetting_DDR3X16[dram_adc_size];
			break;
		case DRAM_COMSETTING_DDR3_X32:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR3 x32 "));
			pDramCom = &DRAM_ComSetting_DDR3X32[dram_adc_size];
			break;
		case DRAM_COMSETTING_DDR4_X16:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR4 x16 "));
			p->support_channel_num = CHANNEL_SINGLE;
			pDramCom = &DRAM_ComSetting_DDR4X16[dram_adc_size];
			break;
		case DRAM_COMSETTING_DDR4_X32:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR4 x32 "));
			pDramCom = &DRAM_ComSetting_DDR4X32[dram_adc_size];
			break;
		case DRAM_COMSETTING_LP3_178BALL:
			mcSHOW_DBG_MSG(("[EMI] Use Common LP3 178BALL x32 "));
			u1dramcom_type = DRAM_COMSETTING_DEFAULT;//from emi gen
			break;
		case DRAM_COMSETTING_LP3_168BALL:
			mcSHOW_DBG_MSG(("[EMI] Use Common LP3 168BALL x32 "));
			u1dramcom_type = DRAM_COMSETTING_DEFAULT;//from emigen
			break;
		case DRAM_COMSETTING_DDR3_ASYM:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR3 ASYM "));
			u1dramcom_type = DRAM_COMSETTING_DEFAULT;//from emigen
			break;
		case DRAM_COMSETTING_DDR4_ASYM:
			mcSHOW_DBG_MSG(("[EMI] Use Common DDR4 ASYM "));
			u1dramcom_type = DRAM_COMSETTING_DEFAULT;//from emigen
			break;
		case DRAM_COMSETTING_DEFAULT:
		default:
			mcSHOW_DBG_MSG(("[EMI] Use default emigen "));
			break;
	}
	mcSHOW_DBG_MSG(("emi settings \r\n"));

	if(u1dramcom_type != DRAM_COMSETTING_DEFAULT)
	{
		emi_set->EMI_CONA_VAL = pDramCom->EMI_CONA_VAL;
		emi_set->EMI_CONF_VAL = pDramCom->EMI_CONF_VAL;
		emi_set->EMI_CONH_VAL = pDramCom->EMI_CONH_VAL;
		emi_set->CHN0_EMI_CONA_VAL = pDramCom->EMI_CHN0_CONA_VAL;
		emi_set->CHN1_EMI_CONA_VAL = pDramCom->EMI_CHN1_CONA_VAL;
	}

	mcSHOW_DBG_MSG(("[EMI] Config emi settings:\n"));
	mcSHOW_DBG_MSG(("EMI_CONA=0x%x,EMI_CONF=0x%x,EMI_CONH=0x%x\n", emi_set->EMI_CONA_VAL, emi_set->EMI_CONF_VAL, emi_set->EMI_CONH_VAL));
	mcSHOW_DBG_MSG(("CHN0_EMI_CONA_VAL=0x%x, CHN1_EMI_CONA_VAL=0x%x\n",emi_set->CHN0_EMI_CONA_VAL, emi_set->CHN1_EMI_CONA_VAL));

}

int init_dram(DRAM_DRAM_TYPE_T dram_type,
	DRAM_CBT_MODE_EXTERN_T dram_cbt_mode_extern,
	DRAM_INFO_BY_MRR_T *dram_info, unsigned char get_mdl_used)
{
	DRAMC_CTX_T *p;
	unsigned char backup_broadcast;

	printf("%s:%d: init_dram Starting\n", __func__, __LINE__);

	switch (dram_type) {
	case TYPE_LPDDR4X:
	case TYPE_LPDDR4P:
	case TYPE_LPDDR4:
		ps_curr_dram_ctx = dramc_ctx_lp4;
		break;
	case TYPE_LPDDR3:
		ps_curr_dram_ctx = dramc_ctx_lp3;
		break;
	case TYPE_PCDDR4:
		ps_curr_dram_ctx = dramc_ctx_ddr4;
		break;
	case TYPE_PCDDR3:
		ps_curr_dram_ctx = dramc_ctx_ddr3;
		break;
	default:
		show_err("[Error] Unrecognized type\n");
		break;
	}

#ifdef DRAM_ETT
	dramc_printf(INFO, "ETT Version 0.0.1\n");
	TestMenu();
#endif

	p = ps_curr_dram_ctx;

	/* Over write the dram type in dramc ctx, in case LP4/4X/4P difference */
	p->dram_type = dram_type;

	if((is_lp4_family(p)) && (get_LP4_type_by_adc()==TYPE_LPDDR4X)) {
		ps_curr_dram_ctx->dram_type = TYPE_LPDDR4X;
		p->dram_type = TYPE_LPDDR4X;
	}

	if((is_lp4_family(p)) && (pin_mux_setting != 0 )) {
		p->is_emcp = TRUE;
	}

#if SUPPORT_TYPE_PCDDR3
	if (p->dram_type == TYPE_PCDDR3) {
		if (pin_mux_setting == 4) {
			p->pinmux_type = PINMUX_TYPE_PC3_X8X4;
			show_msg((INFO, "[dram]is DDR3 8x4 PINMUX\n"));
		} else {
			show_msg((INFO, "[dram]is DDR3 16x2 PINMUX\n"));
		}
	}
#endif

	EMI_Comsetting_Config(p);

	dump_dramc_ctx_t(p);

	// set dram voltage
//#if defined(DRAM_HQA) || defined(DRAM_ETT)
	set_dram_voltage(p);
	get_dram_voltage(p, DRAM_VOL_VDD2);
	get_dram_voltage(p, DRAM_VOL_VDDQ);
//#endif

	set_mdl_used_flag(get_mdl_used);

	/* Convert DRAM_CBT_MODE_EXTERN_T to DRAM_CBT_MODE_T */
	switch ((int)dram_cbt_mode_extern) {
	case CBT_R0_R1_NORMAL:
		p->dram_cbt_mode[RANK_0] = CBT_NORMAL_MODE;
		p->dram_cbt_mode[RANK_1] = CBT_NORMAL_MODE;
		break;
	case CBT_R0_R1_BYTE:
		p->dram_cbt_mode[RANK_0] = CBT_BYTE_MODE1;
		p->dram_cbt_mode[RANK_1] = CBT_BYTE_MODE1;
		break;
	case CBT_R0_NORMAL_R1_BYTE:
		p->dram_cbt_mode[RANK_0] = CBT_NORMAL_MODE;
		p->dram_cbt_mode[RANK_1] = CBT_BYTE_MODE1;
		break;
	case CBT_R0_BYTE_R1_NORMAL:
		p->dram_cbt_mode[RANK_0] = CBT_BYTE_MODE1;
		p->dram_cbt_mode[RANK_1] = CBT_NORMAL_MODE;
		break;
	default:
		show_err("Error!Not Support");
		break;
	}

	dramc_broadcast_on_off(DRAMC_BROADCAST_ON);

	if (gfirst_init_flag == 0) {
		mpll_init();
		global_option_init(p);
		gfirst_init_flag = 1;
	}
	backup_broadcast = get_dramc_broadcast();
#if (FOR_DV_SIMULATION_USED == 0)
	emi_init(p);
#endif

#if ENABLE_RANK_NUMBER_AUTO_DETECTION && SUPPORT_TYPE_LPDDR4// only need to do this when DUAL_RANK_ENABLE is 1
	p->support_rank_num = RANK_DUAL;
#else
	p->support_rank_num = get_dram_rank_nr();
#endif

	dramc_broadcast_on_off(backup_broadcast);
	show_msg((INFO, "\n%s %s\n", chip_name, PL_version));
	show_msg((INFO, "dram_type %d, R0/R1 cbt_mode %d/%d\n\n",
		p->dram_type, p->dram_cbt_mode[RANK_0],
		p->dram_cbt_mode[RANK_1]));

	dramc_init_pre_settings(p);

#if DUAL_FREQ_K
	spm_pinmux_setting(p);
#endif

	switch(p->dram_type){
#if SUPPORT_TYPE_LPDDR4
	case TYPE_LPDDR4:
	case TYPE_LPDDR4X:
		set_freq_sel_by_table(p, &freq_tbl_lp4[DRAM_DFS_SHUFFLE_MAX - 1]);
		break;
#endif

#if SUPPORT_TYPE_LPDDR3
	case TYPE_LPDDR3:
		set_freq_sel_by_table(p, &freq_tbl_lp3[DRAM_DFS_SHUFFLE_MAX - 1]);
		break;
#endif

#if SUPPORT_TYPE_PCDDR4
	case TYPE_PCDDR4:
		set_freq_sel_by_table(p, &freq_tbl_pc4[DRAM_DFS_SHUFFLE_1]);
		break;
#endif

#if SUPPORT_TYPE_PCDDR3
	case TYPE_PCDDR3:
		set_freq_sel_by_table(p, &freq_tbl_pc3[DRAM_DFS_SHUFFLE_1]);
		break;
#endif
	default:
		show_err("Error!Not Support Dram Type\n");
		break;
	}

#if SUPPORT_SAVE_TIME_FOR_CALIBRATION
	dram_last_exception_detection(p);

#if DUAL_FREQ_K && SUPPORT_TYPE_LPDDR4
	p->shu_type = DRAM_DFS_SHUFFLE_3;
#else
	p->shu_type = DRAM_DFS_SHUFFLE_1;
#endif

	DramcSave_Time_For_Cal_Init(p);
#endif

	dump_dramc_ctx_t(p);

#if SIMULATION_SW_IMPED
#if SUPPORT_TYPE_LPDDR4
	if (p->dram_type == TYPE_LPDDR4) {
		dramc_sw_impedance_cal(p, TERM);
	} else if (p->dram_type == TYPE_LPDDR4X) {
		dramc_sw_impedance_cal(p, UNTERM);
		dramc_sw_impedance_cal(p, TERM);
	} else { /* TYPE_LPDDR4P/LP3/DDR4/DDR3 */
		dramc_sw_impedance_cal(p, UNTERM);
	}
#else
	if (p->dram_type == TYPE_PCDDR4)
		dramc_sw_impedance_cal(p, UNTERM);
	else
		dramc_sw_impedance_cal(p, p->odt_onoff);
#endif

	dramc_update_impedance_term_2un_term(p);
#endif

	dfs_init_for_calibration(p);

#if ENABLE_RANK_NUMBER_AUTO_DETECTION && SUPPORT_TYPE_LPDDR4// only need to do this when DUAL_RANK_ENABLE is 1
	if (is_lp4_family(p))
	{
		dram_rank_number_detection(p);
	}
#endif

	backup_broadcast = get_dramc_broadcast();
#if (FOR_DV_SIMULATION_USED == 0)
	emi_init2();
#endif
	dramc_broadcast_on_off(backup_broadcast);

	dram_calibration_main(p, dram_info);

	dram_check(p);

#ifdef DRAM_SLT
	dram_slt_check(p);
#endif

#ifndef DRAM_HQA
	// resotre Vcore to 0.8,DVS need stable voltage
	if((p->dram_type == TYPE_PCDDR4)|| (p->dram_type == TYPE_PCDDR3))
		set_vcore_voltage(800000);

	switch_dramc_voltage_to_auto_mode(p);
#endif

#if DUMP_AO_REG
	unsigned char channel;
	show_msg((INFO, "Dump register for DDR%d\n", p->frequency << 1));
	for (channel = CHANNEL_A; channel < CHANNEL_NUM; channel++) {
		dump_ddrphy_reg(channel);
		dump_dramc_reg(channel);
	}
#endif

	return DRAM_OK;
}

#if (FOR_DV_SIMULATION_USED == 1)
/*
 * This function is used to run bring up simulation.
 * It shall not be used in normal flow, but use init_dram() instead.
 * @ps_dramc_ctx: NOT used. Just to meet DPI interface requirement.
 * Spec.:
 * LPDDR4: 2400
 * LPDDR3: 1866
 * PCDDR4: 2667
 * PCDDR3: 1866
 */
void dpi_simulation_dramc(void)
{
	DRAM_DRAM_TYPE_T type;

#if SUPPORT_TYPE_LPDDR4 /* Highest Priority */
	type = TYPE_LPDDR4;
#elif SUPPORT_TYPE_LPDDR3
	type = TYPE_LPDDR3;
#elif SUPPORT_TYPE_PCDDR4
	type = TYPE_PCDDR4;
#else
	type = TYPE_PCDDR3; /* Lowest */
#endif

	show_msg_with_timestamp((CRITICAL, "start dramc calibration\n"));
	init_dram(type, CBT_R0_R1_NORMAL, NULL, NORMAL_USED);
}

#endif
