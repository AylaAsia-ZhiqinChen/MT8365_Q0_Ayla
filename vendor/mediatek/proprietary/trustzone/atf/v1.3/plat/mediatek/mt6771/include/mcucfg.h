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

#include <platform_def.h>
#include <stdint.h>

#define MP2_MISC_CONFIG_BOOT_ADDR_L(cpu)	(MCUCFG_BASE + 0x2290 + ((cpu) * 8))
#define MP2_MISC_CONFIG_BOOT_ADDR_H(cpu)	(MCUCFG_BASE + 0x2294 + ((cpu) * 8))

#define MP2_CPUCFG			(MCUCFG_BASE + 0x2208)




#define MP0_CPUTOP_SPMC_CTL		(MCUCFG_BASE + 0x788)
#define MP1_CPUTOP_SPMC_CTL		(MCUCFG_BASE + 0x78C)
#define MP1_CPUTOP_SPMC_SRAM_CTL	(MCUCFG_BASE + 0x790)

#define sw_spark_en			(1<<0)
#define sw_no_wait_for_q_channel	(1<<1)
#define sw_fsm_override			(1<<2)
#define sw_logic_pre1_pdb		(1<<3)
#define sw_logic_pre2_pdb		(1<<4)
#define sw_logic_pdb			(1<<5)
#define sw_iso				(1<<6)
#define sw_sram_sleepb			(0x3F<<7)
#define sw_sram_isointb			(1<<13)
#define sw_clk_dis			(1<<14)
#define sw_ckiso			(1<<15)
#define sw_pd				(0x3F<<16)
#define sw_hot_plug_reset		(1<<22)
#define sw_pwr_on_override_en		(1<<23)
#define sw_pwr_on			(1<<24)
#define sw_coq_dis			(1<<25)
#define logic_pdbo_all_off_ack		(1<<26)
#define logic_pdbo_all_on_ack		(1<<27)
#define logic_pre2_pdbo_all_on_ack	(1<<28)
#define logic_pre1_pdbo_all_on_ack	(1<<29)

#define CPUSYSx_CPUx_SPMC_CTL(cluster, cpu)     (MCUCFG_BASE + 0x1c30 + cluster * 0x2000 + cpu * 4)

#define CPUSYS0_CPU0_SPMC_CTL	(MCUCFG_BASE + 0x1c30)
#define CPUSYS0_CPU1_SPMC_CTL	(MCUCFG_BASE + 0x1c34)
#define CPUSYS0_CPU2_SPMC_CTL	(MCUCFG_BASE + 0x1c38)
#define CPUSYS0_CPU3_SPMC_CTL	(MCUCFG_BASE + 0x1c3C)

#define CPUSYS1_CPU0_SPMC_CTL	(MCUCFG_BASE + 0x3c30)
#define CPUSYS1_CPU1_SPMC_CTL	(MCUCFG_BASE + 0x3c34)
#define CPUSYS1_CPU2_SPMC_CTL	(MCUCFG_BASE + 0x3c38)
#define CPUSYS1_CPU3_SPMC_CTL	(MCUCFG_BASE + 0x3c3C)

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


#define SPARK2LDO			(MCUCFG_BASE + 0x2700)
/* APB Module mcucfg */
#define MP0_CA7_CACHE_CONFIG		(MCUCFG_BASE + 0x000)
#define MP0_AXI_CONFIG			(MCUCFG_BASE + 0x02C)
#define MP0_MISC_CONFIG0		(MCUCFG_BASE + 0x030)
#define MP0_MISC_CONFIG2		(MCUCFG_BASE + 0x038)
#define MP0_MISC_CONFIG_BOOT_ADDR(cpu)	(MCUCFG_BASE + 0x038 + ((cpu) * 8))
#define MP0_MISC_CONFIG3		(MCUCFG_BASE + 0x03C)
#define MP0_MISC_CONFIG9		(MCUCFG_BASE + 0x054)
#define MP0_CA7_MISC_CONFIG		(MCUCFG_BASE + 0x064)

#define MP0_RW_RSVD0			(MCUCFG_BASE + 0x06C)


#define MP1_CA7_CACHE_CONFIG		(MCUCFG_BASE + 0x200)
#define MP1_AXI_CONFIG			(MCUCFG_BASE + 0x22C)
#define MP1_MISC_CONFIG0		(MCUCFG_BASE + 0x230)
#define MP1_MISC_CONFIG2		(MCUCFG_BASE + 0x238)
#define MP1_MISC_CONFIG_BOOT_ADDR(cpu)	(MCUCFG_BASE + 0x238 + ((cpu) * 8))
#define MP1_MISC_CONFIG3		(MCUCFG_BASE + 0x23C)
#define MP1_MISC_CONFIG9		(MCUCFG_BASE + 0x254)
#define MP1_CA7_MISC_CONFIG		(MCUCFG_BASE + 0x264)

#define CCI_ADB400_DCM_CONFIG		(MCUCFG_BASE + 0x740)
#define SYNC_DCM_CONFIG			(MCUCFG_BASE + 0x744)

#define L2C_CFG_MP0			(MCUCFG_BASE + 0x7F0)
#define L2C_CFG_MP1			(MCUCFG_BASE + 0x7F4)

#define MP0_SPMC			(MCUCFG_BASE + 0x788)
#define MP1_SPMC			(MCUCFG_BASE + 0x78C)
#define MP2_AXI_CONFIG			(MCUCFG_BASE + 0x220C)
#define MP2_AXI_CONFIG_acinactm		(1<<0)

#define MPx_AXI_CONFIG_acinactm		(1<<4)

#define MPx_CA7_MISC_CONFIG_standbywfil2	(1<<28)


#define MP0_CPU0_STANDBYWFE		(1<<20)
#define MP0_CPU1_STANDBYWFE		(1<<21)
#define MP0_CPU2_STANDBYWFE		(1<<22)
#define MP0_CPU3_STANDBYWFE		(1<<23)

#define MP1_CPU0_STANDBYWFE		(1<<20)
#define MP1_CPU1_STANDBYWFE		(1<<21)
#define MP1_CPU2_STANDBYWFE		(1<<22)
#define MP1_CPU3_STANDBYWFE		(1<<23)

#define CPUSYS0_SPARKVRETCNTRL	(MCUCFG_BASE+0x1c00)
#define CPUSYS0_SPARKEN		(MCUCFG_BASE+0x1c04)
#define CPUSYS0_AMUXSEL		(MCUCFG_BASE+0x1c08)
#define CPUSYS1_SPARKVRETCNTRL	(MCUCFG_BASE+0x3c00)
#define CPUSYS1_SPARKEN		(MCUCFG_BASE+0x3c04)
#define CPUSYS1_AMUXSEL		(MCUCFG_BASE+0x3c08)

#define MP2_CPUTOP_SPMC_CTL		(MCUCFG_BASE + 0x22A0)
#define MP2_CPUTOP_SPMC_STA		(MCUCFG_BASE + 0x22A4)

#define CPUSYS2_CPU0_SPMC_CTL	(MCUCFG_BASE + 0x2430)
#define CPUSYS2_CPU1_SPMC_CTL	(MCUCFG_BASE + 0x2438)
#define CPUSYS2_CPU0_SPMC_STA	(MCUCFG_BASE + 0x2434)
#define CPUSYS2_CPU1_SPMC_STA	(MCUCFG_BASE + 0x243C)


#define B_SW_HOT_PLUG_RESET		(1<<30)

#define B_SW_PD_OFFSET			(18)
#define B_SW_PD				(0x3f<<B_SW_PD_OFFSET)

#define B_SW_SRAM_SLEEPB_OFFSET		(12)
#define B_SW_SRAM_SLEEPB		(0x3f<<B_SW_SRAM_SLEEPB_OFFSET)

#define B_SW_SRAM_ISOINTB		(1<<9)
#define B_SW_ISO			(1<<8)
#define B_SW_LOGIC_PDB			(1<<7)
#define B_SW_LOGIC_PRE2_PDB		(1<<6)
#define B_SW_LOGIC_PRE1_PDB		(1<<5)
#define B_SW_FSM_OVERRIDE		(1<<4)
#define B_SW_PWR_ON			(1<<3)
#define B_SW_PWR_ON_OVERRIDE_EN		(1<<2)



#define B_FSM_STATE_OUT_OFFSET		(6)
#define B_FSM_STATE_OUT_MASK		(0x1f << B_FSM_STATE_OUT_OFFSET)
#define B_SW_LOGIC_PDBO_ALL_OFF_ACK	(1<<5)
#define B_SW_LOGIC_PDBO_ALL_ON_ACK	(1<<4)
#define B_SW_LOGIC_PRE2_PDBO_ALL_ON_ACK	(1<<3)
#define B_SW_LOGIC_PRE1_PDBO_ALL_ON_ACK	(1<<2)


#define B_FSM_OFF				(0<<B_FSM_STATE_OUT_OFFSET)
#define B_FSM_ON				(1<<B_FSM_STATE_OUT_OFFSET)
#define B_FSM_RET				(2<<B_FSM_STATE_OUT_OFFSET)

/* APB Module infracfg_ao */
#define INFRA_TOPAXI_PROTECTEN_1	(INFRACFG_AO_BASE + 0x250)
#define INFRA_TOPAXI_PROTECTSTA1_1	(INFRACFG_AO_BASE + 0x258)
#define INFRA_TOPAXI_PROTECTEN_1_SET	(INFRACFG_AO_BASE + 0x2A8)
#define INFRA_TOPAXI_PROTECTEN_1_CLR	(INFRACFG_AO_BASE + 0x2AC)

#define IDX_PROTECT_MP0_CACTIVE		10
#define IDX_PROTECT_MP1_CACTIVE		11
#define IDX_PROTECT_ICC0_CACTIVE	12
#define IDX_PROTECT_ICD0_CACTIVE	13
#define IDX_PROTECT_ICC1_CACTIVE	14
#define IDX_PROTECT_ICD1_CACTIVE	15
#define IDX_PROTECT_L2C0_CACTIVE	26
#define IDX_PROTECT_L2C1_CACTIVE	27

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
	MP1_ACINACTM_SHIFT = 4,
	MP2_ACINACTM_SHIFT = 0,
	MP0_ACINACTM = 1 << MP0_ACINACTM_SHIFT,
	MP1_ACINACTM = 1 << MP1_ACINACTM_SHIFT,
	MP2_ACINACTM = 1 << MP2_ACINACTM_SHIFT
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

#endif  /* __MCUCFG_H__ */
