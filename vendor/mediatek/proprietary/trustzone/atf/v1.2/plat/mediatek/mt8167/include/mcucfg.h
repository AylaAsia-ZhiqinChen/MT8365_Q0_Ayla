/*
 * Copyright (c) 2014-2015, ARM Limited and Contributors. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of ARM nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific
 * prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __MCUCFG_H__
#define __MCUCFG_H__

#include <mt8167_def.h>
#include <stdint.h>

struct mt8167_mcucfg_regs {
	uint32_t mp0_ca7l_cache_config;
	struct {
		uint32_t mem_delsel0;
		uint32_t mem_delsel1;
	} mp0_cpu[4];
	uint32_t mp0_cache_mem_delsel0;
	uint32_t mp0_cache_mem_delsel1;
	uint32_t mp0_axi_config;
	uint32_t mp0_misc_config[2];
	struct {
		uint32_t rv_addr_lw;
		uint32_t rv_addr_hw;
	} mp0_rv_addr[4];
	uint32_t mp0_ca7l_cfg_dis;
	uint32_t mp0_ca7l_clken_ctrl;
	uint32_t mp0_ca7l_rst_ctrl;
	uint32_t mp0_ca7l_misc_config;
	uint32_t mp0_ca7l_dbg_pwr_ctrl;
	uint32_t mp0_rw_rsvd0;
	uint32_t mp0_rw_rsvd1;
	uint32_t mp0_ro_rsvd;
	uint32_t reserved0_0[100];
	uint32_t mp1_cpucfg;
	uint32_t mp1_miscdbg;
	uint32_t reserved0_1[13];
	uint32_t mp1_rst_ctl;
	uint32_t mp1_clkenm_div;
	uint32_t reserved0_2[7];
	uint32_t mp1_config_res;
	uint32_t reserved0_3[13];
	struct {
		uint32_t rv_addr_lw;
		uint32_t rv_addr_hw;
	} mp1_rv_addr[2];
	uint32_t reserved0_4[84];
	uint32_t mp0_rst_status;		/* 0x400 */
	uint32_t mp0_dbg_ctrl;
	uint32_t mp0_dbg_flag;
	uint32_t mp0_ca7l_ir_mon;
	struct {
		uint32_t pc_lw;
		uint32_t pc_hw;
		uint32_t fp_arch32;
		uint32_t sp_arch32;
		uint32_t fp_arch64_lw;
		uint32_t fp_arch64_hw;
		uint32_t sp_arch64_lw;
		uint32_t sp_arch64_hw;
	} mp0_dbg_core[4];
	uint32_t dfd_ctrl;
	uint32_t dfd_cnt_l;
	uint32_t dfd_cnt_h;
	uint32_t misccfg_mp0_rw_rsvd;
	uint32_t misccfg_sec_vio_status0;
	uint32_t misccfg_sec_vio_status1;
	uint32_t reserved1[22];
	uint32_t misccfg_rw_rsvd;		/* 0x500 */
	uint32_t mcusys_dbg_mon_sel_a;
	uint32_t mcusys_dbg_mon;
	uint32_t reserved2[61];
	uint32_t mcusys_config_a;		/* 0x600 */
	uint32_t mcusys_config1_a;
	uint32_t mcusys_gic_peribase_a;
	uint32_t reserved3;
	uint32_t sec_range0_start;		/* 0x610 */
	uint32_t sec_range0_end;
	uint32_t sec_range_enable;
	uint32_t reserved4;
	uint32_t int_pol_ctl[8];		/* 0x620 */
	uint32_t aclken_div;			/* 0x640 */
	uint32_t pclken_div;
	uint32_t l2c_sram_ctrl;
	uint32_t armpll_jit_ctrl;
	uint32_t cci_addrmap;			/* 0x650 */
	uint32_t cci_config;
	uint32_t cci_periphbase;
	uint32_t cci_nevntcntovfl;
	uint32_t cci_clk_ctrl;			/* 0x660 */
	uint32_t cci_acel_s1_ctrl;
	uint32_t bus_fabric_dcm_ctrl;
	uint32_t reserved5;
	uint32_t xgpt_ctl;			/* 0x670 */
	uint32_t xgpt_idx;
	uint32_t ptpod2_ctl0;
	uint32_t ptpod2_ctl1;
	uint32_t mcusys_revid;
	uint32_t mcusys_rw_rsvd0;
	uint32_t mcusys_rw_rsvd1;
};

static struct mt8167_mcucfg_regs *const mt8167_mcucfg = (void *)MCUCFG_BASE;

/* cpu boot mode */
enum {
	MP0_CPUCFG_64BIT_SHIFT = 12,
	MP1_CPUCFG_64BIT_SHIFT = 28,
	MP0_CPUCFG_64BIT = 0xf << MP0_CPUCFG_64BIT_SHIFT,
	MP1_CPUCFG_64BIT = 0xf << MP1_CPUCFG_64BIT_SHIFT
};

/* scu related */
enum {
	MP0_ACINACTM_SHIFT = 4,
	MP1_ACINACTM_SHIFT = 0,
	MP0_ACINACTM = 1 << MP0_ACINACTM_SHIFT,
	MP1_ACINACTM = 1 << MP1_ACINACTM_SHIFT
};

enum {
	MP1_DIS_RGU0_WAIT_PD_CPUS_L1_ACK_SHIFT = 0,
	MP1_DIS_RGU1_WAIT_PD_CPUS_L1_ACK_SHIFT = 4,
	MP1_DIS_RGU2_WAIT_PD_CPUS_L1_ACK_SHIFT = 8,
	MP1_DIS_RGU3_WAIT_PD_CPUS_L1_ACK_SHIFT = 12,
	MP1_DIS_RGU_NOCPU_WAIT_PD_CPUS_L1_ACK_SHIFT = 16,

	MP1_DIS_RGU0_WAIT_PD_CPUS_L1_ACK =
		0xf << MP1_DIS_RGU0_WAIT_PD_CPUS_L1_ACK_SHIFT,
	MP1_DIS_RGU1_WAIT_PD_CPUS_L1_ACK =
		0xf << MP1_DIS_RGU1_WAIT_PD_CPUS_L1_ACK_SHIFT,
	MP1_DIS_RGU2_WAIT_PD_CPUS_L1_ACK =
		0xf << MP1_DIS_RGU2_WAIT_PD_CPUS_L1_ACK_SHIFT,
	MP1_DIS_RGU3_WAIT_PD_CPUS_L1_ACK =
		0xf << MP1_DIS_RGU3_WAIT_PD_CPUS_L1_ACK_SHIFT,
	MP1_DIS_RGU_NOCPU_WAIT_PD_CPUS_L1_ACK =
		0xf << MP1_DIS_RGU_NOCPU_WAIT_PD_CPUS_L1_ACK_SHIFT
};

enum {
	MP1_AINACTS_SHIFT = 4,
	MP1_AINACTS = 1 << MP1_AINACTS_SHIFT
};

enum {
	MP1_SW_CG_GEN_SHIFT = 12,
	MP1_SW_CG_GEN = 1 << MP1_SW_CG_GEN_SHIFT
};

enum {
	MP1_L2RSTDISABLE_SHIFT = 14,
	MP1_L2RSTDISABLE = 1 << MP1_L2RSTDISABLE_SHIFT
};

#define MP0_MISC_CONFIG_BOOT_ADDR(cpu)  (MCUCFG_BASE + 0x038 + ((cpu) * 8))
#define MP0_MISC_CONFIG3                (MCUCFG_BASE + 0x03C)
#define MP0_CA7_MISC_CONFIG             (MCUCFG_BASE + 0x064)
#define MP0_CPUTOP_SPMC_CTL             (MCUCFG_BASE + 0x15C)
#define MP1_MISC_CONFIG_BOOT_ADDR(cpu)  (MCUCFG_BASE + 0x238 + ((cpu) * 8))
#define MP1_MISC_CONFIG3                (MCUCFG_BASE + 0x23C)
#define MP1_CA7_MISC_CONFIG             (MCUCFG_BASE + 0x264)
#define MP1_CPUTOP_SPMC_CTL             (MCUCFG_BASE + 0x78C)
#define MP1_CPUTOP_SPMC_SRAM_CTL        (MCUCFG_BASE + 0x790)
#define CPUSYS0_CPU0_SPMC_CTL           (MCUCFG_BASE + 0x1C30)
#define CPUSYS0_CPU1_SPMC_CTL           (MCUCFG_BASE + 0x1C34)
#define CPUSYS0_CPU2_SPMC_CTL           (MCUCFG_BASE + 0x1C38)
#define CPUSYS0_CPU3_SPMC_CTL           (MCUCFG_BASE + 0x1C3C)
#define CPUSYS1_CPU0_SPMC_CTL           (MCUCFG_BASE + 0x3C30)
#define CPUSYS1_CPU1_SPMC_CTL           (MCUCFG_BASE + 0x3C34)
#define CPUSYS1_CPU2_SPMC_CTL           (MCUCFG_BASE + 0x3C38)
#define CPUSYS1_CPU3_SPMC_CTL           (MCUCFG_BASE + 0x3C3C)

#define cpu_sw_spark_en			(1<<0)
#define cpu_sw_no_wait_for_q_channel	(1<<1)
#define cpu_sw_fsm_override		(1<<2)
#define cpu_sw_logic_pre1_pdb		(1<<3)
#define cpu_sw_logic_pre2_pdb		(1<<4)
#define cpu_sw_logic_pdb		(1<<5)
#define cpu_sw_iso			(1<<6)
#define cpu_sw_sram_sleepb		(1<<7)
#define cpu_sw_sram_isointb		(1<<8)
#define cpu_sw_clk_dis			(1<<9)
#define cpu_sw_ckiso			(1<<10)
#define cpu_sw_pd			(0x1F<<11)
#define cpu_sw_hot_plug_reset		(1<<16)
#define cpu_sw_powr_on_override_en	(1<<17)
#define cpu_sw_pwr_on			(1<<18)
#define cpu_spark2ldo_allswoff		(1<<19)
#define cpu_pdbo_all_on_ack		(1<<20)
#define cpu_pre2_pdbo_allon_ack		(1<<21)
#define cpu_pre1_pdbo_allon_ack		(1<<22)

/* #define MP0_CA7_MISC_CONFIG          (MCUCFG_BASE + 0x064) */
#define MP0_CPU0_STANDBYWFE		(1<<20)
#define MP0_CPU1_STANDBYWFE		(1<<21)
#define MP0_CPU2_STANDBYWFE		(1<<22)
#define MP0_CPU3_STANDBYWFE		(1<<23)

#endif  /* __MCUCFG_H__ */
