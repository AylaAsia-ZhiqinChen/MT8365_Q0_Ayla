/*
 * Copyright (C) 2017 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#ifndef _MT_GPUFREQ_CORE_H_
#define _MT_GPUFREQ_CORE_H_

/**************************************************
 * MT8168 : GPU DVFS OPP table Setting 3 idx
 **************************************************/
#define SEG_GPU_DVFS_FREQ0			(800000)	/* KHz */
#define SEG_GPU_DVFS_FREQ1			(560000)	/* KHz */
#define SEG_GPU_DVFS_FREQ2			(450000)	/* KHz */


#define SEG_GPU_DVFS_VOLT0			(80000)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT1			(70000)		/* mV x 100 */
#define SEG_GPU_DVFS_VOLT2			(65000)		/* mV x 100 */


#define SEG_GPU_DVFS_VSRAM0			(90000)	/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM1			(80000)	/* mV x 100 */
#define SEG_GPU_DVFS_VSRAM2			(80000)	/* mV x 100 */


/**************************************************
 * PMIC Setting
 **************************************************/
#define VGPU_MAX_VOLT		(SEG_GPU_DVFS_VOLT0)
#define VSRAM_GPU_MAX_VOLT	(SEG_GPU_DVFS_VSRAM0)
#define DELAY_FACTOR		(625)
#define BUCK_DIFF_MAX		(25000)		/* mV x 100 */
#define BUCK_DIFF_MIN		(10000)		/* mV x 100 */
#define NUM_OF_OPP_IDX		(32)

/**************************************************
 * efuse Setting
 **************************************************/
#define GPUFREQ_EFUSE_INDEX			(8)
#define EFUSE_MFG_SPD_BOND_SHIFT		(8)
#define EFUSE_MFG_SPD_BOND_MASK			(0xF)
#define FUNC_CODE_EFUSE_INDEX			(22)

/**************************************************
 * Clock Setting
 **************************************************/
#define POST_DIV_2_MAX_FREQ			(1900000)
#define POST_DIV_2_MIN_FREQ			(750000)
#define POST_DIV_4_MAX_FREQ			(950000)
#define POST_DIV_4_MIN_FREQ			(375000)
#define POST_DIV_8_MAX_FREQ			(475000)
#define POST_DIV_8_MIN_FREQ			(187500)
#define POST_DIV_16_MAX_FREQ			(237500)
#define POST_DIV_16_MIN_FREQ			(93750)
#define POST_DIV_MASK				(0x70000000)
#define POST_DIV_SHIFT				(24)
#define TO_MHz_HEAD				(100)
#define TO_MHz_TAIL				(10)
#define ROUNDING_VALUE				(5)
#define DDS_SHIFT				(14)
#define GPUPLL_FIN				(26)
#define GPUPLL_CON1				(g_apmixed_base + 0x21C)

/**************************************************
 * Reference Power Setting
 **************************************************/
#define GPU_ACT_REF_POWER			(1285)		/* mW  */
#define GPU_ACT_REF_FREQ			(900000)	/* KHz */
#define GPU_ACT_REF_VOLT			(90000)		/* mV x 100 */
#define GPU_DVFS_PTPOD_DISABLE_VOLT	(80000)		/* mV x 100 */
#define GPU_DVFS_PTPOD_DISABLE_VSRAM_VOLT (90000)

/**************************************************
 * Log Setting
 **************************************************/
#define GPUFERQ_TAG				"[GPU/DVFS]"
#define gpufreq_pr_err(fmt, args...)	\
	pr_err(GPUFERQ_TAG"[ERROR]"fmt, ##args)
#define gpufreq_pr_warn(fmt, args...)	\
	pr_warn(GPUFERQ_TAG"[WARNING]"fmt, ##args)
#define gpufreq_pr_info(fmt, args...)	\
	pr_info(GPUFERQ_TAG"[INFO]"fmt, ##args)
#define gpufreq_pr_debug(fmt, args...)	\
	pr_debug(GPUFERQ_TAG"[DEBUG]"fmt, ##args)

/**************************************************
 * Condition Setting
 **************************************************/
//#define MT_GPUFREQ_STATIC_PWR_READY2USE
//#define MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
//#define MT_GPUFREQ_BATT_PERCENT_PROTECT /* todo: disable it */
//#define MT_GPUFREQ_BATT_OC_PROTECT
//#define MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE

/**************************************************
 * Battery Over Current Protect
 **************************************************/
#ifdef MT_GPUFREQ_BATT_OC_PROTECT
#define MT_GPUFREQ_BATT_OC_LIMIT_FREQ		(485000)	/* KHz */
#endif

/**************************************************
 * Battery Percentage Protect
 **************************************************/
#ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT
#define MT_GPUFREQ_BATT_PERCENT_LIMIT_FREQ	(485000)	/* KHz */
#endif

/**************************************************
 * Low Battery Volume Protect
 **************************************************/
#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
#define MT_GPUFREQ_LOW_BATT_VOLT_LIMIT_FREQ	(485000)	/* KHz */
#endif

/**************************************************
 * Proc Node Definition
 **************************************************/
#ifdef CONFIG_PROC_FS
#define PROC_FOPS_RW(name)	\
	static int mt_ ## name ## _proc_open \
			(struct inode *inode, struct file *file)	\
	{	\
		return single_open(file, mt_ ## name ## _proc_show, \
			PDE_DATA(inode));	\
	}	\
	static const struct file_operations mt_ ## name ## _proc_fops = { \
		.owner = THIS_MODULE,	\
		.open = mt_ ## name ## _proc_open,	\
		.read = seq_read,	\
		.llseek = seq_lseek,	\
		.release = single_release,	\
		.write = mt_ ## name ## _proc_write,	\
	}
#define PROC_FOPS_RO(name)	\
	static int mt_ ## name ## _proc_open(struct inode *inode, \
			struct file *file)			\
	{	\
		return single_open(file, mt_ ## name ## _proc_show, \
			PDE_DATA(inode));	\
	}	\
	static const struct file_operations mt_ ## name ## _proc_fops =	\
	{	\
		.owner = THIS_MODULE,	\
		.open = mt_ ## name ## _proc_open,	\
		.read = seq_read,	\
		.llseek = seq_lseek,	\
		.release = single_release,	\
	}
#define PROC_ENTRY(name) \
	{__stringify(name), &mt_ ## name ## _proc_fops}
#endif

/**************************************************
 * Operation Definition
 **************************************************/
#define VOLT_NORMALIZATION(volt) \
	((volt % 625) ? (volt - (volt % 625) + 625) : volt)
#ifndef MIN
#define MIN(x, y)				(((x) < (y)) ? (x) : (y))
#endif
#define GPUOP(khz, volt, vsram)	\
	{	\
		.gpufreq_khz = khz,	\
		.gpufreq_volt = volt,	\
		.gpufreq_vsram = vsram,	\
	}

/**************************************************
 * Enumerations
 **************************************************/
enum g_segment_id_enum {
	MT6767_SEGMENT = 1,
	MT8168_SEGMENT,
	MT8168_PRO_SEGMENT,
};
enum g_post_divider_power_enum  {
	POST_DIV2 = 1,
	POST_DIV4,
	POST_DIV8,
	POST_DIV16,
};
enum g_clock_source_enum  {
	CLOCK_MAIN = 0,
	CLOCK_SUB,
};
enum g_limited_idx_enum {
	IDX_THERMAL_PROTECT_LIMITED = 0,
	IDX_LOW_BATT_LIMITED,
	IDX_BATT_PERCENT_LIMITED,
	IDX_BATT_OC_LIMITED,
	IDX_PBM_LIMITED,
	NUMBER_OF_LIMITED_IDX,
};
enum g_volt_switch_enum {
	VOLT_FALLING = 0,
	VOLT_RISING,
};

/**************************************************
 * Structures
 **************************************************/
struct g_opp_table_info {
	unsigned int gpufreq_khz;
	unsigned int gpufreq_volt;
	unsigned int gpufreq_vsram;
};
struct g_clk_info {
/* substitution clock for mfg transient mux setting mfgpll*/
	struct clk *clk_mux;		/* main clock for mfg setting */
	struct clk *clk_pll_src;
	struct clk *clk_main_parent;
	struct clk *clk_sub_parent;
	struct clk *subsys_mfg_cg;	/* clock gating */
	struct clk *subsys_mfg_mbist_diag;	/* clock gating*/
};
struct g_pmic_info {
	struct regulator *reg_vgpu;
	struct regulator *reg_vsram_gpu;
	struct regulator *reg_vcore;
#ifdef CONFIG_MTK_QOS_SUPPORT
	struct pm_qos_request pm_vgpu;
	struct pm_qos_request pm_v_emi_gpu;

#endif
};

/**************************************************
 * External functions declaration
 **************************************************/
extern bool mtk_get_gpu_loading(unsigned int *pLoading);
extern u32 get_devinfo_with_index(u32 index);

#endif /* _MT_GPUFREQ_CORE_H_ */
