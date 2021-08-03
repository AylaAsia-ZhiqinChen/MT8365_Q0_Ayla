/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/cpumask.h>
#include <linux/cpu.h>
#include <mt-plat/mtk_io.h>
#include <mt-plat/sync_write.h>
#include <mt-plat/mtk_secure_api.h>

#include <mtk_dcm_internal.h>

static short dcm_cpu_cluster_stat;


unsigned int all_dcm_type =
		(ARMCORE_DCM_TYPE | MCUSYS_DCM_TYPE | RGU_DCM_TYPE
		| GIC_SYNC_DCM_TYPE | INFRA_DCM_TYPE
		| DDRPHY_DCM_TYPE | EMI_DCM_TYPE | DRAMC_DCM_TYPE
		);
unsigned int init_dcm_type =
		(ARMCORE_DCM_TYPE | MCUSYS_DCM_TYPE | RGU_DCM_TYPE
		| GIC_SYNC_DCM_TYPE | INFRA_DCM_TYPE
		);

#if defined(__KERNEL__) && defined(CONFIG_OF)
/* TODO: Fix base addresses. */
unsigned long dcm_infracfg_ao_base;
unsigned long dcm_mcucfg_base;
unsigned long dcm_mcucfg_phys_base;
unsigned long dcm_dramc0_ao_base;
unsigned long dcm_dramc1_ao_base;
unsigned long dcm_ddrphy0_ao_base;
unsigned long dcm_ddrphy1_ao_base;
unsigned long dcm_chn0_emi_base;
unsigned long dcm_chn1_emi_base;
unsigned long dcm_emi_base;

#define INFRACFG_AO_NODE "mediatek,mt8168-infracfg"
#define MCUCFG_NODE "mediatek,mt8168-mcusys"
#endif /* #if defined(__KERNEL__) && defined(CONFIG_OF) */

short is_dcm_bringup(void)
{
#ifdef DCM_BRINGUP
	dcm_pr_info("%s: skipped for bring up\n", __func__);
	return 1;
#else
	return 0;
#endif
}

#ifdef CONFIG_OF
/* TODO: Fix base addresses. */
int mt_dcm_dts_map(void)
{
	struct device_node *node;
	struct resource r;

	/* infracfg_ao */
	node = of_find_compatible_node(NULL, NULL, INFRACFG_AO_NODE);
	if (!node) {
		dcm_pr_info("error: cannot find node %s\n", INFRACFG_AO_NODE);
		return -1;
	}
	dcm_infracfg_ao_base = (unsigned long)of_iomap(node, 0);
	if (!dcm_infracfg_ao_base) {
		dcm_pr_info("error: cannot iomap %s\n", INFRACFG_AO_NODE);
		return -1;
	}

	/* mcucfg */
	node = of_find_compatible_node(NULL, NULL, MCUCFG_NODE);
	if (!node) {
		dcm_pr_info("error: cannot find node %s\n", MCUCFG_NODE);
		return -1;
	}
	if (of_address_to_resource(node, 0, &r)) {
		dcm_pr_info("error: cannot get phys addr %s\n", MCUCFG_NODE);
		return -1;
	}
	dcm_mcucfg_phys_base = r.start;
	dcm_mcucfg_base = (unsigned long)of_iomap(node, 0);
	if (!dcm_mcucfg_base) {
		dcm_pr_info("error: cannot iomap %s\n", MCUCFG_NODE);
		return -1;
	}

	/* dram related */
	/* dramc0_ao */
	dcm_dramc0_ao_base = (unsigned long)mt_dramc_chn_base_get(0);
	if (!dcm_dramc0_ao_base) {
		dcm_pr_info("error: cannot iomap %s\n", "dramc0_ao");
		return -1;
	}

	/* dramc1_ao */
	dcm_dramc1_ao_base = (unsigned long)mt_dramc_chn_base_get(1);
	if (!dcm_dramc1_ao_base) {
		dcm_pr_info("error: cannot iomap %s\n", "dramc1_ao");
		return -1;
	}

	/* ddrphy0_ao */
	dcm_ddrphy0_ao_base = (unsigned long)mt_ddrphy_chn_base_get(0);
	if (!dcm_ddrphy0_ao_base) {
		dcm_pr_info("error: cannot iomap %s\n", "ddrphy0_ao");
		return -1;
	}

	/* ddrphy1_ao */
	dcm_ddrphy1_ao_base = (unsigned long)mt_ddrphy_chn_base_get(1);
	if (!dcm_ddrphy1_ao_base) {
		dcm_pr_info("error: cannot iomap %s\n", "ddrphy1_ao");
		return -1;
	}

	dcm_chn0_emi_base = (unsigned long)mt_chn_emi_base_get(0);
	if (!dcm_chn0_emi_base) {
		dcm_pr_info("error: cannot iomap %s\n", "chn0_emi");
		return -1;
	}

	dcm_chn1_emi_base = (unsigned long)mt_chn_emi_base_get(1);
	if (!dcm_chn1_emi_base) {
		dcm_pr_info("error: cannot iomap %s\n", "chn1_emi");
		return -1;
	}

	/* emi */
	dcm_emi_base = (unsigned long)mt_cen_emi_base_get();
	if (!dcm_emi_base) {
		dcm_pr_info("error: cannot iomap %s\n", "emi");
		return -1;
	}

	return 0;
}
#else
int mt_dcm_dts_map(void)
{

	return 0;
}
#endif /* #ifdef CONFIG_PM */

int dcm_set_stall_wr_del_sel(unsigned int mp0, unsigned int mp1)
{
	/* not support */
	return 0;
}

unsigned int sync_dcm_convert_freq2div(unsigned int freq)
{
	unsigned int div = 0, min_freq = SYNC_DCM_CLK_MIN_FREQ;

	if (freq < min_freq)
		return 0;

	/* max divided ratio =
	 * Floor (CPU Frequency / (4 or 5) * system timer Frequency)
	 */
	div = (freq / min_freq) - 1;
	if (div > SYNC_DCM_MAX_DIV_VAL)
		return SYNC_DCM_MAX_DIV_VAL;

	return div;
}

int sync_dcm_set_cci_div(unsigned int cci)
{
	if (!dcm_initiated)
		return -1;

	/*
	 * 1. set xxx_sync_dcm_div first
	 * 2. set xxx_sync_dcm_tog from 0 to 1 for making sure it is toggled
	 */
	reg_write(MCUCFG_SYNC_DCM_CCI_REG,
		  aor(reg_read(MCUCFG_SYNC_DCM_CCI_REG),
		      ~MCUCFG_SYNC_DCM_SEL_CCI_MASK,
		      cci << MCUCFG_SYNC_DCM_SEL_CCI));
	reg_write(MCUCFG_SYNC_DCM_CCI_REG,
		aor(reg_read(MCUCFG_SYNC_DCM_CCI_REG),
		~MCUCFG_SYNC_DCM_CCI_TOGMASK,
		MCUCFG_SYNC_DCM_CCI_TOG0));
	reg_write(MCUCFG_SYNC_DCM_CCI_REG,
		aor(reg_read(MCUCFG_SYNC_DCM_CCI_REG),
		~MCUCFG_SYNC_DCM_CCI_TOGMASK,
		MCUCFG_SYNC_DCM_CCI_TOG1));
#ifdef __KERNEL__
	dcm_pr_dbg("%s: MCUCFG_SYNC_DCM_CCI_REG=0x%08x, cci_div_sel=%u/%u\n",
#else
	dcm_pr_dbg("%s: MCUCFG_SYNC_DCM_CCI_REG=0x%X, cci_div_sel=%u/%u\n",
#endif
		 __func__, reg_read(MCUCFG_SYNC_DCM_CCI_REG),
		 (and(reg_read(MCUCFG_SYNC_DCM_CCI_REG),
		      MCUCFG_SYNC_DCM_SEL_CCI_MASK) >> MCUCFG_SYNC_DCM_SEL_CCI),
		 cci);

	return 0;
}

int sync_dcm_set_cci_freq(unsigned int cci)
{
	dcm_pr_dbg("%s: cci=%u\n", __func__, cci);
	sync_dcm_set_cci_div(sync_dcm_convert_freq2div(cci));

	return 0;
}

int sync_dcm_set_mp0_div(unsigned int mp0)
{
	return 0;
}

int sync_dcm_set_mp0_freq(unsigned int mp0)
{
	return 0;
}

int sync_dcm_set_mp1_div(unsigned int mp1)
{
	return 0;
}

int sync_dcm_set_mp1_freq(unsigned int mp1)
{
	return 0;
}

int sync_dcm_set_mp2_div(unsigned int mp2)
{
	return 0;
}

int sync_dcm_set_mp2_freq(unsigned int mp2)
{
	return 0;
}

/* unit of frequency is MHz */
int sync_dcm_set_cpu_freq(unsigned int cci, unsigned int mp0,
			unsigned int mp1, unsigned int mp2)
{
	sync_dcm_set_cci_freq(cci);
	sync_dcm_set_mp0_freq(mp0);
	sync_dcm_set_mp1_freq(mp1);
	sync_dcm_set_mp2_freq(mp2);

	return 0;
}

int sync_dcm_set_cpu_div(unsigned int cci, unsigned int mp0,
			unsigned int mp1, unsigned int mp2)
{
	sync_dcm_set_cci_div(cci);
	sync_dcm_set_mp0_div(mp0);
	sync_dcm_set_mp1_div(mp1);
	sync_dcm_set_mp2_div(mp2);

	return 0;
}

/*****************************************
 * following is implementation per DCM module.
 * 1. per-DCM function is 1-argu with ON/OFF/MODE option.
 *****************************************/

int dcm_mcusys_preset(int on)
{
	return 0;
}

int dcm_infra_preset(int on)
{
	return 0;
}

int dcm_mcusys(int on)
{
	dcm_mcu_misccfg_adb400_dcm(on);
	dcm_mcu_misccfg_bus_clock_dcm(on);
	dcm_mcu_misccfg_bus_fabric_dcm(on);
	dcm_mcu_misccfg_l2_shared_dcm(on);
	dcm_mcu_misccfg_mcu_misc_dcm(on);

	return 0;
}

int dcm_infra(int on)
{
	dcm_infracfg_ao_dcm_infrabus_group(on);
	dcm_infracfg_ao_dcm_mem_group(on);
	dcm_infracfg_ao_dcm_peribus_group(on);
	dcm_infracfg_ao_dcm_ssusb_group(on);

	return 0;
}

int dcm_dramc_ao(int on)
{
	dcm_dramc_ch0_top1_dcm_dramc_group(on);
	dcm_dramc_ch1_top1_dcm_dramc_group(on);

	return 0;
}

int dcm_ddrphy(int on)
{
	dcm_dramc_ch0_top0_ddrphy(on);
	dcm_dramc_ch1_top0_ddrphy(on);

	return 0;
}

int dcm_emi(int on)
{
	dcm_chn0_emi_dcm_emi_group(on);
	dcm_chn1_emi_dcm_emi_group(on);

	return 0;
}

int dcm_armcore(int mode)
{
	dcm_mcu_misccfg_bus_arm_pll_divider_dcm(mode);

	return 0;
}

int dcm_gic_sync(int on)
{
	dcm_mcu_misccfg_gic_sync_dcm(on);

	return 0;
}

int dcm_rgu(int on)
{
	dcm_mp0_cpucfg_mp0_rgu_dcm(on);

	return 0;
}

struct DCM dcm_array[NR_DCM_TYPE] = {
	{
		.typeid = MCUSYS_DCM_TYPE,
		.name = "MCUSYS_DCM",
		.func = (DCM_FUNC) dcm_mcusys,
		.preset_func = (DCM_PRESET_FUNC) dcm_mcusys_preset,
		.current_state = MCUSYS_DCM_ON,
		.default_state = MCUSYS_DCM_ON,
		.disable_refcnt = 0,
	},
	{
		.typeid = INFRA_DCM_TYPE,
		.name = "INFRA_DCM",
		.func = (DCM_FUNC) dcm_infra,
		.preset_func = (DCM_PRESET_FUNC) dcm_infra_preset,
		.current_state = INFRA_DCM_ON,
		.default_state = INFRA_DCM_ON,
		.disable_refcnt = 0,
	},
	{
		.typeid = DRAMC_DCM_TYPE,
		.name = "DRAMC_DCM",
		.func = (DCM_FUNC) dcm_dramc_ao,
		.current_state = DRAMC_AO_DCM_ON,
		.default_state = DRAMC_AO_DCM_ON,
		.disable_refcnt = 0,
	},
	{
		.typeid = DDRPHY_DCM_TYPE,
		.name = "DDRPHY_DCM",
		.func = (DCM_FUNC) dcm_ddrphy,
		.current_state = DDRPHY_DCM_ON,
		.default_state = DDRPHY_DCM_ON,
		.disable_refcnt = 0,
	},
	{
		.typeid = EMI_DCM_TYPE,
		.name = "EMI_DCM",
		.func = (DCM_FUNC) dcm_emi,
		.current_state = EMI_DCM_ON,
		.default_state = EMI_DCM_ON,
		.disable_refcnt = 0,
	},
	{
		.typeid = ARMCORE_DCM_TYPE,
		.name = "ARMCORE_DCM",
		.func = (DCM_FUNC) dcm_armcore,
		.current_state = ARMCORE_DCM_MODE1,
		.default_state = ARMCORE_DCM_MODE1,
		.disable_refcnt = 0,
	},
	{
		.typeid = GIC_SYNC_DCM_TYPE,
		.name = "GIC_SYNC_DCM",
		.func = (DCM_FUNC) dcm_gic_sync,
		.current_state = GIC_SYNC_DCM_ON,
		.default_state = GIC_SYNC_DCM_ON,
		.disable_refcnt = 0,
	},
	{
		.typeid = RGU_DCM_TYPE,
		.name = "RGU_CORE_DCM",
		.func = (DCM_FUNC) dcm_rgu,
		.current_state = RGU_DCM_ON,
		.default_state = RGU_DCM_ON,
		.disable_refcnt = 0,
	},
};

void dcm_dump_regs(void)
{
	dcm_pr_info("\n******** dcm dump register *********\n");
	/* infra_ao */
	REG_DUMP(INFRA_BUS_DCM_CTRL);
	REG_DUMP(PERI_BUS_DCM_CTRL);
	REG_DUMP(MEM_DCM_CTRL);
	REG_DUMP(DFS_MEM_DCM_CTRL);
	REG_DUMP(P2P_RX_CLK_ON);
	/* mcusys */
	REG_DUMP(MP0_CPUCFG_MP0_RGU_DCM_CONFIG);
	REG_DUMP(L2C_SRAM_CTRL);
	REG_DUMP(CCI_CLK_CTRL);
	REG_DUMP(BUS_FABRIC_DCM_CTRL);
	REG_DUMP(MCU_MISC_DCM_CTRL);
	REG_DUMP(CCI_ADB400_DCM_CONFIG);
	REG_DUMP(SYNC_DCM_CONFIG);
	REG_DUMP(MP_GIC_RGU_SYNC_DCM);
	REG_DUMP(BUS_PLL_DIVIDER_CFG);
	/* Not support
	 * REG_DUMP(MP1_CPUCFG_MP1_RGU_DCM_CONFIG);
	 * REG_DUMP(SYNC_DCM_CLUSTER_CONFIG);
	 * REG_DUMP(MP0_PLL_DIVIDER_CFG);
	 * REG_DUMP(MP1_PLL_DIVIDER_CFG);
	 * REG_DUMP(MCSIA_DCM_EN);
	 */
	/* dramc/ddrphy/emi */
	//REG_DUMP(DRAMC_CH0_TOP0_MISC_CG_CTRL0);
	//REG_DUMP(DRAMC_CH0_TOP0_MISC_CG_CTRL2);
	//REG_DUMP(DRAMC_CH0_TOP0_MISC_CTRL3);
	//REG_DUMP(DRAMC_CH0_TOP1_DRAMC_PD_CTRL);
	//REG_DUMP(DRAMC_CH0_TOP1_CLKAR);
	REG_DUMP(CHN0_EMI_CHN_EMI_CONB);
	//REG_DUMP(DRAMC_CH1_TOP0_MISC_CG_CTRL0);
	//REG_DUMP(DRAMC_CH1_TOP0_MISC_CG_CTRL2);
	//REG_DUMP(DRAMC_CH1_TOP0_MISC_CTRL3);
	//REG_DUMP(DRAMC_CH1_TOP1_DRAMC_PD_CTRL);
	//REG_DUMP(DRAMC_CH1_TOP1_CLKAR);
	REG_DUMP(CHN1_EMI_CHN_EMI_CONB);
}

#if 0 /* Add API to mtk_secure_api.h if necessary. */
int dcm_smc_get_cnt(int type_id)
{
	return dcm_smc_read_cnt(type_id);
}

void dcm_smc_msg_send(unsigned int msg)
{
	dcm_smc_msg(msg);
}
#endif

short dcm_get_cpu_cluster_stat(void)
{
	return dcm_cpu_cluster_stat;
}
