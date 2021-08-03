/*
 * Copyright (C) 2016 MediaTek Inc.
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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/jiffies.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/input.h>
#include <linux/sched.h>
#include <linux/sched/rt.h>
#include <linux/kthread.h>
#include <linux/uaccess.h>
#include <linux/regulator/consumer.h>

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_address.h>
#endif

#include "mtk_gpufreq.h"
#include "mt-plat/upmu_common.h"
#include "mt-plat/sync_write.h"
#include "mt-plat/mtk_pmic_wrap.h"
#include "mt-plat/mtk_chip.h"
#include "mach/mtk_fhreg.h"
#include "mach/mtk_freqhopping.h"
#include "mach/mtk_thermal.h"
#include "mt8167/include/mach/upmu_sw.h"

/* #define BRING_UP */
#define DRIVER_NOT_READY	-1
/* #define STATIC_PWR_READY2USE */

/* Define low battery voltage support */
/* By Power team comment, we don't need it in MT8167*/
/* #define MT_GPUFREQ_LOW_BATT_VOLT_PROTECT */

/* Define low battery volume support */
/* By Power team comment, we don't need it in MT8167*/
/* #define MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT */

/* Define oc support */
/* By Power team comment, we don't need it in MT8167*/
/* #define MT_GPUFREQ_OC_PROTECT */

/* Define for dynamic power table update */
#define MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE

/* there is no PBM feature in mt8167 */
#define DISABLE_PBM_FEATURE

/* Define for random test */
/* #define MT_GPU_DVFS_RANDOM_TEST */

/* Define for SRAM debugging */
#ifdef CONFIG_MTK_RAM_CONSOLE
#define MT_GPUFREQ_AEE_RR_REC
#endif

/* Define how to set up VGPU by PMIC_WRAP/PMIC/external IC */
/* mt8167 doesn't the setting */
/* #define VGPU_SET_BY_PMIC_WRAP */
#define VGPU_SET_BY_PMIC
/* #define VGPU_SET_BY_EXTIC */

#ifdef VGPU_SET_BY_EXTIC
#include "fan53555.h"
#endif

/* Define register write function */
#define mt_gpufreq_reg_write(val, addr) \
	mt_reg_sync_writel((val), ((void *)addr))

/* Operate Point Definition */
#define GPUOP(frequency, voltage, index) \
{                      \
	.freq = frequency, \
	.volt = voltage,   \
	.idx  = index,     \
}

/* GPU DVFS OPP table setting. KHz */
#define GPU_DVFS_FREQ0		(598000)
#define GPU_DVFS_FREQ1		(500500)
#define GPU_DVFS_FREQ2		(430000)
#define GPU_DVFS_FREQ3		(390000)
#define GPU_DVFS_FREQ4		(299000)
#define GPU_DVFS_FREQ5		(253500)

#define GPUFREQ_LAST_FREQ_LEVEL	(GPU_DVFS_FREQ5)

/* mV x 100 */
#define GPU_DVFS_VOLT0	(130000)
#define GPU_DVFS_VOLT1	(125000)
#define GPU_DVFS_VOLT2	(115000)

#define GPU_DVFS_PTPOD_DISABLE_VOLT	GPU_DVFS_VOLT2

#define UNIVPLL_FREQ	GPU_DVFS_FREQ3

/* PMIC settle time (us), should not be changed */
#ifdef VGPU_SET_BY_PMIC

#define PMIC_MIN_VGPU GPU_DVFS_VOLT2
#define PMIC_MAX_VGPU GPU_DVFS_VOLT0

#define PMIC_CMD_DELAY_TIME	 5
#define MIN_PMIC_SETTLE_TIME	25
#define PMIC_VOLT_UP_SETTLE_TIME(old_volt, new_volt) \
	(((((new_volt) - (old_volt)) + 1250-1) / 1250) + PMIC_CMD_DELAY_TIME)
#define PMIC_VOLT_DOWN_SETTLE_TIME(old_volt, new_volt) \
	(((((old_volt) - (new_volt)) * 2) / 625) + PMIC_CMD_DELAY_TIME)
#define PMIC_VOLT_ON_OFF_DELAY_US	   (200)
#define INVALID_SLEW_RATE	(0)

/* register val -> mV */
#define GPU_VOLT_TO_MV(volt)            ((((volt)*625)/100+700)*100)

#define PMIC_BUCK_VGPU_VOSEL_ON		MT6351_PMIC_BUCK_VGPU_VOSEL_ON
#define PMIC_ADDR_VGPU_VOSEL_ON		MT6351_PMIC_BUCK_VGPU_VOSEL_ON_ADDR
#define PMIC_ADDR_VGPU_VOSEL_ON_MASK	MT6351_PMIC_BUCK_VGPU_VOSEL_ON_MASK
#define PMIC_ADDR_VGPU_VOSEL_ON_SHIFT	MT6351_PMIC_BUCK_VGPU_VOSEL_ON_SHIFT
#define PMIC_ADDR_VGPU_VOSEL_CTRL	MT6351_PMIC_BUCK_VGPU_VOSEL_CTRL_ADDR
#define PMIC_ADDR_VGPU_VOSEL_CTRL_MASK	MT6351_PMIC_BUCK_VGPU_VOSEL_CTRL_MASK
#define PMIC_ADDR_VGPU_VOSEL_CTRL_SHIFT	MT6351_PMIC_BUCK_VGPU_VOSEL_CTRL_SHIFT
#define PMIC_ADDR_VGPU_EN		MT6351_PMIC_BUCK_VGPU_EN_ADDR
#define PMIC_ADDR_VGPU_EN_MASK		MT6351_PMIC_BUCK_VGPU_EN_MASK
#define PMIC_ADDR_VGPU_EN_SHIFT		MT6351_PMIC_BUCK_VGPU_EN_SHIFT
#define PMIC_ADDR_VGPU_EN_CTRL		MT6351_PMIC_BUCK_VGPU_EN_CTRL_ADDR
#define PMIC_ADDR_VGPU_EN_CTRL_MASK	MT6351_PMIC_BUCK_VGPU_EN_CTRL_MASK
#define PMIC_ADDR_VGPU_EN_CTRL_SHIFT	MT6351_PMIC_BUCK_VGPU_EN_CTRL_SHIFT

#elif defined(VGPU_SET_BY_EXTIC)

#define GPU_LDO_BASE			0x10001000
#define EXTIC_VSEL0			0x0		/* [0] */
#define EXTIC_BUCK_EN0_MASK		0x1
#define EXTIC_BUCK_EN0_SHIFT		0x7
#define EXTIC_VSEL1			0x1	/* [0] */
#define EXTIC_BUCK_EN1_MASK		0x1
#define EXTIC_BUCK_EN1_SHIFT		0x7
#define EXTIC_VGPU_CTRL			0x2
#define EXTIC_VGPU_SLEW_MASK		0x7
#define EXTIC_VGPU_SLEW_SHIFT		0x4

#define EXTIC_VOLT_ON_OFF_DELAY_US		350
#define EXTIC_VOLT_STEP			12826 /* 12.826mV per step */
#define EXTIC_SLEW_STEP			100	/* 10.000mV per step */
/* us */
#define EXTIC_VOLT_UP_SETTLE_TIME(old_volt, new_volt, slew_rate) \
(((((new_volt)-(old_volt))*EXTIC_SLEW_STEP)/EXTIC_VOLT_STEP)/(slew_rate))
/* us */
#define EXTIC_VOLT_DOWN_SETTLE_TIME(old_volt, new_volt, slew_rate) \
(((((old_volt)-(new_volt))*EXTIC_SLEW_STEP)/EXTIC_VOLT_STEP)/(slew_rate))

#endif

/* efuse */
#define GPUFREQ_EFUSE_INDEX      (4)
#define EFUSE_MFG_SPD_BOND_SHIFT (22)
#define EFUSE_MFG_SPD_BOND_MASK  (0x3)

/* LOG */
#define TAG	 "[Power/gpufreq] "

#define gpufreq_pr_err(fmt, args...) pr_err(TAG""fmt, ##args)
#define gpufreq_pr_warn(fmt, args...) pr_warn(TAG""fmt, ##args)
#define gpufreq_pr_info(fmt, args...) pr_info(TAG""fmt, ##args)
#define gpufreq_pr_debug(fmt, args...) pr_debug(TAG""fmt, ##args)

#ifdef CONFIG_HAS_EARLYSUSPEND
static struct early_suspend mt_early_suspend_handler = {
	.level = EARLY_SUSPEND_LEVEL_DISABLE_FB + 200,
	.suspend = NULL,
	.resume = NULL,
};
#endif

static sampler_func g_pFreqSampler;
static sampler_func g_pVoltSampler;

static gpufreq_power_limit_notify g_power_limit_notify;
static gpufreq_ptpod_update_notify g_ptpod_update_notify;

/* GPU DVFS OPP Table */
static struct mt_gpufreq_table_info mt_opp_tbl_0[] = {
	GPUOP(GPU_DVFS_FREQ3, GPU_DVFS_VOLT2, 3),
	GPUOP(GPU_DVFS_FREQ4, GPU_DVFS_VOLT2, 4),
	GPUOP(GPU_DVFS_FREQ5, GPU_DVFS_VOLT2, 5),
};

static struct mt_gpufreq_table_info mt_opp_tbl_1[] = {
	GPUOP(GPU_DVFS_FREQ1, GPU_DVFS_VOLT1, 1),
	GPUOP(GPU_DVFS_FREQ3, GPU_DVFS_VOLT2, 3),
	GPUOP(GPU_DVFS_FREQ4, GPU_DVFS_VOLT2, 4),
	GPUOP(GPU_DVFS_FREQ5, GPU_DVFS_VOLT2, 5),
};

static struct mt_gpufreq_table_info mt_opp_tbl_2[] = {
	GPUOP(GPU_DVFS_FREQ0, GPU_DVFS_VOLT0, 0),
	GPUOP(GPU_DVFS_FREQ1, GPU_DVFS_VOLT1, 1),
	GPUOP(GPU_DVFS_FREQ3, GPU_DVFS_VOLT2, 3),
	GPUOP(GPU_DVFS_FREQ5, GPU_DVFS_VOLT2, 5),
};

/* PTPOD enable/disable GPU power doamin */
static gpufreq_mfgclock_notify g_mfgclock_enable_notify;
static gpufreq_mfgclock_notify g_mfgclock_disable_notify;

/* AEE (SRAM debug) */
#ifdef MT_GPUFREQ_AEE_RR_REC
enum gpu_dvfs_state {
	GPU_DVFS_IS_DOING_DVFS = 0,
	GPU_DVFS_IS_VGPU_ENABLED,
};

static void _mt_gpufreq_aee_init(void)
{
	aee_rr_rec_gpu_dvfs_vgpu(0xFF);
	aee_rr_rec_gpu_dvfs_oppidx(0xFF);
	aee_rr_rec_gpu_dvfs_status(0xFC);
}
#endif

/* Enable GPU DVFS count */
static int g_dvfs_disable_count;

/* initial value, 396.5MHz */
static unsigned int g_cur_gpu_freq = GPU_DVFS_FREQ5;
/* initial value, 1.15v */
static unsigned int g_cur_gpu_volt = GPU_DVFS_VOLT2;
static unsigned int g_cur_gpu_idx = 0xFF;
static unsigned int g_cur_gpu_OPPidx = 0xFF;

static unsigned int g_cur_freq_init_keep;

/* In default settiing,
 * freq_table[0] is max frequency, freq_table[num-1] is min frequency
 */
static unsigned int g_gpufreq_max_id;

/* If not limited, it should be set to freq_table[0] (MAX frequency) */
static unsigned int g_limited_max_id;
static unsigned int g_limited_min_id;

static bool mt_gpufreq_ready;
static bool mt_gpufreq_debug;
static bool mt_gpufreq_pause;
static bool mt_keep_max_frequency_state;
static bool mt_keep_opp_frequency_state;
static unsigned int mt_keep_opp_frequency;
static unsigned int mt_keep_opp_index;
static bool mt_fixed_freq_volt_state;
static unsigned int mt_fixed_frequency;
static unsigned int mt_fixed_voltage;

static unsigned int mt_volt_enable;
static unsigned int mt_volt_enable_state;
static bool g_limited_thermal_ignore_state;

#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
static bool g_limited_low_batt_volt_ignore_state;
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT
static bool g_limited_low_batt_volume_ignore_state;
#endif

#ifdef MT_GPUFREQ_OC_PROTECT
static bool g_limited_oc_ignore_state;
#endif

static bool mt_opp_max_frequency_state;
static unsigned int mt_opp_max_frequency;
static unsigned int mt_opp_max_index;

static unsigned int mt_dvfs_table_type;

static DEFINE_MUTEX(mt_gpufreq_lock);
static DEFINE_MUTEX(mt_gpufreq_power_lock);

static unsigned int mt_gpufreqs_num;
static struct mt_gpufreq_table_info *mt_gpufreqs;
static struct mt_gpufreq_table_info *mt_gpufreqs_default;
static struct mt_gpufreq_power_table_info *mt_gpufreqs_power;
static struct mtk_gpu_power_info *mt_gpufreqs_power_info;

static struct mt_gpufreq_pmic_t *mt_gpufreq_pmic;

static bool mt_ptpod_disable;
static int mt_ptpod_disable_idx;

static void mt_gpufreq_clock_switch(unsigned int freq_new);
static void mt_gpufreq_volt_switch(
	unsigned int volt_old,
	unsigned int volt_new);
static void mt_gpufreq_set(
	unsigned int freq_old,
	unsigned int freq_new,
	unsigned int volt_old,
	unsigned int volt_new);
static unsigned int _mt_gpufreq_get_cur_volt(void);
static unsigned int _mt_gpufreq_get_cur_freq(void);
static void _mt_gpufreq_kick_pbm(int enable);

#ifndef DISABLE_PBM_FEATURE
static bool g_limited_pbm_ignore_state;
/* PBM limit power */
static unsigned int mt_pbm_limited_gpu_power;
/* Limited frequency index for PBM */
static unsigned int mt_pbm_limited_index;
#define GPU_OFF_SETTLE_TIME_MS		(100)
struct delayed_work notify_pbm_gpuoff_work;
#endif

int __attribute__ ((weak))
get_immediate_gpu_wrap(void)
{
	gpufreq_pr_debug("%s doesn't exist", __func__);
	return 0;
}

/* Check GPU DVFS efuse */
static unsigned int mt_gpufreq_get_dvfs_table_type(void)
{
	unsigned int gpu_speed_bounding = 0;
	unsigned int type = 0;

	gpu_speed_bounding = (get_devinfo_with_index(GPUFREQ_EFUSE_INDEX) >>
		EFUSE_MFG_SPD_BOND_SHIFT) & EFUSE_MFG_SPD_BOND_MASK;
	gpufreq_pr_info("GPU frequency bounding from efuse = %x\n",
		gpu_speed_bounding);

	/* No efuse or free run,
	 * use clock-frequency from device tree to determine GPU table type!
	 */
	if (gpu_speed_bounding == 0) {
		static const struct of_device_id gpu_ids[] = {
			{.compatible = "mediatek,mt8167-clark"},
			{ /* sentinel */ }
		};
		struct device_node *node;
		unsigned int gpu_speed = 0;

		node = of_find_matching_node(NULL, gpu_ids);
		if (!node) {
			gpufreq_pr_err("@%s: find GPU node failed\n", __func__);
			gpu_speed = 400000;	/* default speed */
		} else {
			if (!of_property_read_u32(node,
					"clock-frequency", &gpu_speed))
				gpu_speed = gpu_speed / 1000;	/* KHz */
			else {
				gpufreq_pr_err("@%s: missing property, use default level\n",
					__func__);
				gpu_speed = 400000;	/* default speed */
			}
		}

		gpufreq_pr_info("GPU clock-frequency from DT = %d KHz\n",
			gpu_speed);

		if (gpu_speed >= GPU_DVFS_FREQ0)
			type = 2;	/* 600M */
		else if (gpu_speed >= GPU_DVFS_FREQ1)
			type = 1;	/* 500M */

		return type;
	}

	switch (gpu_speed_bounding) {
	case 1:
		type = 2;	/* 600M */
		break;
	case 2:
		type = 1;	/* 500M */
		break;
	default:
		type = 0;	/* 400M */
	}

	return type;
}

/* Power table calculation */
static void mt_gpufreq_power_calculation(
	unsigned int idx,
	unsigned int freq,
	unsigned int volt,
	unsigned int temp)
{
#define GPU_ACT_REF_POWER		845		/* mW  */
#define GPU_ACT_REF_FREQ		850000	/* KHz */
#define GPU_ACT_REF_VOLT		90000	/* mV x 100 */

	unsigned int p_total = 0, p_dynamic = 0, ref_freq = 0, ref_volt = 0;
	int p_leakage = 0;

	p_dynamic = GPU_ACT_REF_POWER;
	ref_freq = GPU_ACT_REF_FREQ;
	ref_volt = GPU_ACT_REF_VOLT;

	p_dynamic = p_dynamic *
		((freq*100)/ref_freq) *
		((volt*100)/ref_volt) *
		((volt*100)/ref_volt) /
		(100*100*100);

#ifdef STATIC_PWR_READY2USE
	p_leakage = mt_spower_get_leakage(MT_SPOWER_GPU, (volt / 100), temp);
	if (!mt_volt_enable_state || p_leakage < 0)
		p_leakage = 0;
#else
	p_leakage = 71;
#endif

	p_total = p_dynamic + p_leakage;
	mt_gpufreqs_power[idx].power = p_total;

	gpufreq_pr_debug("%d: p_dynamic = %d, p_leakage = %d, p_total = %d, temp = %d\n",
		idx, p_dynamic, p_leakage, p_total, temp);
}

/* Random seed generated for test */
#ifdef MT_GPU_DVFS_RANDOM_TEST
static int mt_gpufreq_idx_get(int num)
{
	int random = 0, mult = 0, idx;

	random = jiffies & 0xF;

	while (1) {
		if ((mult * num) >= random) {
			idx = (mult * num) - random;
			break;
		}

		mult++;
	}

	return idx;
}
#endif

/* Set frequency and voltage at driver probe function */
static void mt_gpufreq_set_initial(void)
{
	mutex_lock(&mt_gpufreq_lock);

#ifdef MT_GPUFREQ_AEE_RR_REC
	aee_rr_rec_gpu_dvfs_status(aee_rr_curr_gpu_dvfs_status() |
		(1 << GPU_DVFS_IS_DOING_DVFS));
#endif

	g_cur_gpu_OPPidx = mt_gpufreqs_num - 1;
	g_cur_gpu_freq = mt_gpufreqs[g_cur_gpu_OPPidx].freq;
	g_cur_gpu_volt = mt_gpufreqs[g_cur_gpu_OPPidx].volt;
	g_cur_gpu_idx = mt_gpufreqs[g_cur_gpu_OPPidx].idx;

#ifdef MT_GPUFREQ_AEE_RR_REC
	aee_rr_rec_gpu_dvfs_oppidx(g_cur_gpu_OPPidx);
	aee_rr_rec_gpu_dvfs_status(aee_rr_curr_gpu_dvfs_status() &
		~(1 << GPU_DVFS_IS_DOING_DVFS));
#endif

	mutex_unlock(&mt_gpufreq_lock);
}

#ifndef DISABLE_PBM_FEATURE
static void mt_gpufreq_notify_pbm_gpuoff(struct work_struct *work)
{
	mutex_lock(&mt_gpufreq_lock);

	if (!mt_volt_enable_state)
		_mt_gpufreq_kick_pbm(0);

	mutex_unlock(&mt_gpufreq_lock);
}
#endif

/* Set VGPU enable/disable when GPU clock be switched on/off */
unsigned int mt_gpufreq_voltage_enable_set(unsigned int enable)
{
	/* mt8167, gpu uses vcore, the enable/disable is always on */
	if (mt_gpufreq_ready == false) {
		gpufreq_pr_warn("@%s: GPU DVFS not ready!\n", __func__);
		return DRIVER_NOT_READY;
	}

	if (mt_ptpod_disable == true) {
		if (enable == 0) {
			gpufreq_pr_debug("mt_ptpod_disable == true\n");
			return DRIVER_NOT_READY;
		}
	}
	mt_volt_enable_state = enable;

	return 0;
}
EXPORT_SYMBOL(mt_gpufreq_voltage_enable_set);

/* DVFS enable API for PTPOD */
void mt_gpufreq_enable_by_ptpod(void)
{
	if (mt_gpufreq_ready == false) {
		gpufreq_pr_warn("@%s: GPU DVFS not ready!\n", __func__);
		return;
	}

	mt_gpufreq_voltage_enable_set(0);

#ifdef MTK_GPU_SPM
	if (mt_ptpod_disable)
		mtk_gpu_spm_resume();
#endif

	mt_ptpod_disable = false;
	gpufreq_pr_debug("mt_gpufreq enabled by ptpod\n");

	if (g_mfgclock_disable_notify)
		g_mfgclock_disable_notify();
	else
		gpufreq_pr_err("@%s: no callback!\n", __func__);

	/* pmic auto mode:
	 * the variance of voltage is wide but saves more power.
	 */
	regulator_set_mode(mt_gpufreq_pmic->reg_vgpu, REGULATOR_MODE_NORMAL);
	if (regulator_get_mode(mt_gpufreq_pmic->reg_vgpu) !=
		REGULATOR_MODE_NORMAL)
		gpufreq_pr_err("Vgpu should be REGULATOR_MODE_NORMAL(%d), but mode = %d\n",
			REGULATOR_MODE_NORMAL,
			regulator_get_mode(mt_gpufreq_pmic->reg_vgpu));
}
EXPORT_SYMBOL(mt_gpufreq_enable_by_ptpod);

/* DVFS disable API for PTPOD */
void mt_gpufreq_disable_by_ptpod(void)
{
	int i = 0, target_idx = 0;

	if (mt_gpufreq_ready == false) {
		gpufreq_pr_warn("@%s: GPU DVFS not ready!\n", __func__);
		return;
	}

#ifdef MTK_GPU_SPM
	mtk_gpu_spm_pause();

	g_cur_gpu_volt = _mt_gpufreq_get_cur_volt();
	g_cur_gpu_freq = _mt_gpufreq_get_cur_freq();
#endif

	mt_ptpod_disable = true;
	gpufreq_pr_debug("mt_gpufreq disabled by ptpod\n");

	for (i = 0; i < mt_gpufreqs_num; i++) {
		/* VBoot = 0.85v for PTPOD */
		target_idx = i;
		if (mt_gpufreqs_default[i].volt <= GPU_DVFS_PTPOD_DISABLE_VOLT)
			break;
	}

	if (g_mfgclock_enable_notify)
		g_mfgclock_enable_notify();
	else
		gpufreq_pr_err("@%s: no callback!\n", __func__);

	/* pmic PWM mode:
	 * the variance of voltage is narrow but consumes more power.
	 */
	regulator_set_mode(mt_gpufreq_pmic->reg_vgpu, REGULATOR_MODE_FAST);

	if (regulator_get_mode(mt_gpufreq_pmic->reg_vgpu) !=
		REGULATOR_MODE_FAST)
		gpufreq_pr_err("Vgpu should be REGULATOR_MODE_FAST(%d), but mode = %d\n",
			REGULATOR_MODE_FAST,
			regulator_get_mode(mt_gpufreq_pmic->reg_vgpu));

	mt_ptpod_disable_idx = target_idx;

	mt_gpufreq_voltage_enable_set(1);
	mt_gpufreq_target(target_idx);
}
EXPORT_SYMBOL(mt_gpufreq_disable_by_ptpod);

bool mt_gpufreq_IsPowerOn(void)
{
	return (mt_volt_enable_state == 1);
}
EXPORT_SYMBOL(mt_gpufreq_IsPowerOn);

/* API to switch back default voltage setting for GPU PTPOD disabled */
void mt_gpufreq_restore_default_volt(void)
{
	int i;

	if (mt_gpufreq_ready == false) {
		gpufreq_pr_warn("@%s: GPU DVFS not ready!\n", __func__);
		return;
	}

	mutex_lock(&mt_gpufreq_lock);

	for (i = 0; i < mt_gpufreqs_num; i++) {
		mt_gpufreqs[i].volt = mt_gpufreqs_default[i].volt;
		gpufreq_pr_debug("@%s: mt_gpufreqs[%d].volt = %x\n",
			__func__, i, mt_gpufreqs[i].volt);
	}

#ifndef MTK_GPU_SPM
	mt_gpufreq_volt_switch(
		g_cur_gpu_volt, mt_gpufreqs[g_cur_gpu_OPPidx].volt);
#endif

	g_cur_gpu_volt = mt_gpufreqs[g_cur_gpu_OPPidx].volt;

	mutex_unlock(&mt_gpufreq_lock);
}
EXPORT_SYMBOL(mt_gpufreq_restore_default_volt);

/* Set voltage because PTP-OD modified voltage table by PMIC wrapper */
unsigned int mt_gpufreq_update_volt(
	unsigned int pmic_volt[], unsigned int array_size)
{
	int i;
	unsigned int volt = 0;

	if (mt_gpufreq_ready == false) {
		gpufreq_pr_warn("@%s: GPU DVFS not ready!\n", __func__);
		return DRIVER_NOT_READY;
	}

	if (array_size > mt_gpufreqs_num) {
		gpufreq_pr_err("@%s: array_size = %d, Over-Boundary!\n",
			__func__, array_size);
		return 0;
	}

	mutex_lock(&mt_gpufreq_lock);

	for (i = 0; i < array_size; i++) {
		volt = GPU_VOLT_TO_MV(pmic_volt[i]);

		/* between 950mv~1300mv */
		if ((volt > 95000) && (volt < 130000)) {
			mt_gpufreqs[i].volt = volt;
			gpufreq_pr_debug("@%s: mt_gpufreqs[%d].volt = %x\n",
				__func__, i, mt_gpufreqs[i].volt);
		} else
			gpufreq_pr_err("@%s: index[%d]._volt = %x Over-Boundary\n",
				__func__, i, volt);
	}

#ifndef MTK_GPU_SPM
	mt_gpufreq_volt_switch(
		g_cur_gpu_volt, mt_gpufreqs[g_cur_gpu_OPPidx].volt);
#endif

	g_cur_gpu_volt = mt_gpufreqs[g_cur_gpu_OPPidx].volt;
	if (g_ptpod_update_notify != NULL)
		g_ptpod_update_notify();

	mutex_unlock(&mt_gpufreq_lock);
	return 0;
}
EXPORT_SYMBOL(mt_gpufreq_update_volt);

unsigned int mt_gpufreq_get_dvfs_table_num(void)
{
	return mt_gpufreqs_num;
}
EXPORT_SYMBOL(mt_gpufreq_get_dvfs_table_num);

unsigned int mt_gpufreq_get_freq_by_idx(unsigned int idx)
{
	if (mt_gpufreq_ready == false) {
		gpufreq_pr_warn("@%s: GPU DVFS not ready!\n", __func__);
		return DRIVER_NOT_READY;
	}

	if (idx < mt_gpufreqs_num) {
		gpufreq_pr_debug("@%s: idx = %d, frequency= %d\n",
			__func__, idx, mt_gpufreqs[idx].freq);
		return mt_gpufreqs[idx].freq;
	}

	gpufreq_pr_debug("@%s: idx = %d, NOT found! return 0!\n",
		__func__, idx);
	return 0;
}
EXPORT_SYMBOL(mt_gpufreq_get_freq_by_idx);

unsigned int mt_gpufreq_get_volt_by_idx(unsigned int idx)
{
	if (mt_gpufreq_ready == false) {
		gpufreq_pr_warn("@%s: GPU DVFS not ready!\n", __func__);
		return DRIVER_NOT_READY;
	}

	if (idx < mt_gpufreqs_num) {
		gpufreq_pr_debug("@%s: idx = %d, voltage= %d\n",
			__func__, idx, mt_gpufreqs[idx].volt);
		return mt_gpufreqs[idx].volt;
	}

	gpufreq_pr_debug("@%s: idx = %d, NOT found! return 0!\n",
		__func__, idx);
	return 0;
}
EXPORT_SYMBOL(mt_gpufreq_get_volt_by_idx);

#ifdef MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE
static void mt_update_gpufreqs_power_table(void)
{
	int i = 0, temp = 0;
	unsigned int freq = 0, volt = 0;

	if (mt_gpufreq_ready == false) {
		gpufreq_pr_warn("@%s: GPU DVFS not ready\n", __func__);
		return;
	}

#ifdef CONFIG_THERMAL
	temp = get_immediate_gpu_wrap() / 1000;
#else
	temp = 40;
#endif

	gpufreq_pr_debug("@%s, temp = %d\n", __func__, temp);

	mutex_lock(&mt_gpufreq_lock);

	if ((temp >= -20) && (temp <= 125)) {
		for (i = 0; i < mt_gpufreqs_num; i++) {
			freq = mt_gpufreqs_power[i].freq;
			volt = mt_gpufreqs_power[i].volt;

			mt_gpufreq_power_calculation(i, freq, volt, temp);

			gpufreq_pr_debug("update mt_gpufreqs_power[%d].freq = %d\n",
				i, mt_gpufreqs_power[i].freq);
			gpufreq_pr_debug("update mt_gpufreqs_power[%d].volt = %d\n",
				i, mt_gpufreqs_power[i].volt);
			gpufreq_pr_debug("update mt_gpufreqs_power[%d].power = %d\n",
				i, mt_gpufreqs_power[i].power);
		}
	} else
		gpufreq_pr_err("@%s: temp < 0 or temp > 125, NOT update power table!\n",
			__func__);

	mutex_unlock(&mt_gpufreq_lock);
}
#endif

static void mt_setup_gpufreqs_power_table(int num)
{
	int i = 0, temp = 0;

	mt_gpufreqs_power =
		kcalloc(
			num,
			sizeof(struct mt_gpufreq_power_table_info),
			GFP_KERNEL);
	if (mt_gpufreqs_power == NULL)
		return;

	mt_gpufreqs_power_info =
		kcalloc(
			num,
			sizeof(struct mtk_gpu_power_info),
			GFP_KERNEL);
	if (mt_gpufreqs_power_info == NULL) {
		kfree(mt_gpufreqs_power);
		return;
	}

#ifdef CONFIG_THERMAL
	temp = get_immediate_gpu_wrap() / 1000;
#else
	temp = 40;
#endif

	gpufreq_pr_debug("@%s: temp = %d\n", __func__, temp);

	if ((temp < -20) || (temp > 125)) {
		gpufreq_pr_debug("@%s: temp < 0 or temp > 125!\n", __func__);
		temp = 65;
	}

	for (i = 0; i < num; i++) {
		/* fill-in freq and volt in power table */
		mt_gpufreqs_power[i].freq = mt_gpufreqs[i].freq;
		mt_gpufreqs_power[i].volt = mt_gpufreqs[i].volt;

		/* CJ, Fix? need the mt_gpufreq_power_calculation? */
		mt_gpufreq_power_calculation(
			i,
			mt_gpufreqs_power[i].freq,
			mt_gpufreqs_power[i].volt,
			temp);

		mt_gpufreqs_power_info[i].gpufreq_khz =
			mt_gpufreqs_power[i].freq;
		mt_gpufreqs_power_info[i].gpufreq_power =
			mt_gpufreqs_power[i].power;
		gpufreq_pr_debug("mt_gpufreqs_power[%d].freq = %u\n",
			i, mt_gpufreqs_power[i].freq);
		gpufreq_pr_debug("mt_gpufreqs_power[%d].volt = %u\n",
			i, mt_gpufreqs_power[i].volt);
		gpufreq_pr_debug("mt_gpufreqs_power[%d].power = %u\n",
			i, mt_gpufreqs_power[i].power);
	}

#ifdef CONFIG_THERMAL
	mtk_gpufreq_register(mt_gpufreqs_power_info, num);
#endif
}

/* Register frequency table to gpufreq subsystem */
static int mt_setup_gpufreqs_table(struct mt_gpufreq_table_info *freqs, int num)
{
	int i = 0;

	mt_gpufreqs = kzalloc((num) * sizeof(*freqs), GFP_KERNEL);
	if (mt_gpufreqs == NULL)
		return -ENOMEM;

	mt_gpufreqs_default = kzalloc((num) * sizeof(*freqs), GFP_KERNEL);
	if (mt_gpufreqs_default == NULL) {
		kfree(mt_gpufreqs);
		return -ENOMEM;
	}

	for (i = 0; i < num; i++) {
		mt_gpufreqs[i].freq = freqs[i].freq;
		mt_gpufreqs[i].volt = freqs[i].volt;
		mt_gpufreqs[i].idx = freqs[i].idx;

		mt_gpufreqs_default[i].freq = freqs[i].freq;
		mt_gpufreqs_default[i].volt = freqs[i].volt;
		mt_gpufreqs_default[i].idx = freqs[i].idx;

		gpufreq_pr_debug("freqs[%d].freq = %u\n", i, freqs[i].freq);
		gpufreq_pr_debug("freqs[%d].volt = %u\n", i, freqs[i].volt);
		gpufreq_pr_debug("freqs[%d].idx = %u\n", i, freqs[i].idx);
	}

	mt_gpufreqs_num = num;

	g_limited_max_id = 0;
	g_limited_min_id = mt_gpufreqs_num - 1;

	gpufreq_pr_debug("@%s: g_cur_gpu_freq = %d, g_cur_gpu_volt = %d\n",
		__func__, g_cur_gpu_freq, g_cur_gpu_volt);

	mt_setup_gpufreqs_power_table(num);

	return 0;
}

/* Check if maximum frequency is needed */
static int mt_gpufreq_keep_max_freq(
	unsigned int freq_old, unsigned int freq_new)
{
	if (mt_keep_max_frequency_state == true)
		return 1;

	return 0;
}

/* Set GPU DVFS status */
int mt_gpufreq_state_set(int enabled)
{
	if (enabled) {
		if (!mt_gpufreq_pause) {
			gpufreq_pr_debug("gpufreq already enabled\n");
			return 0;
		}

		/* Enable GPU DVFS */
		g_dvfs_disable_count--;
		gpufreq_pr_debug("enable GPU DVFS: g_dvfs_disable_count = %d\n",
			g_dvfs_disable_count);

		/* Enable DVFS if no any module still disable it */
		if (g_dvfs_disable_count <= 0)
			mt_gpufreq_pause = false;
		else
			gpufreq_pr_warn("someone still disable gpufreq, cannot enable it\n");
	} else {
		/* Disable GPU DVFS */
		g_dvfs_disable_count++;
		gpufreq_pr_debug("disable GPU DVFS: g_dvfs_disable_count = %d\n",
			g_dvfs_disable_count);

		if (mt_gpufreq_pause) {
			gpufreq_pr_debug("gpufreq already disabled\n");
			return 0;
		}

		mt_gpufreq_pause = true;
	}

	return 0;
}
EXPORT_SYMBOL(mt_gpufreq_state_set);

static unsigned int mt_gpufreq_dds_calc(
	unsigned int freq_khz, enum post_div_order_enum post_div_order)
{
	unsigned int dds = 0;

	dds = (((freq_khz * 4) / 1000) * 0x4000) / 26;

	gpufreq_pr_debug("@%s: request freq = %d, div_order = %d, dds = %x\n",
		__func__, freq_khz, post_div_order, dds);

	return dds;
}

static void mt_gpufreq_clock_switch(unsigned int freq_new)
{
	unsigned int dds;

	if (freq_new == g_cur_gpu_freq)
		return;

	dds = mt_gpufreq_dds_calc(freq_new, POST_DIV4);

#ifdef CONFIG_MTK_FREQ_HOPPING
	mt_dfs_mmpll(dds);
#endif

	g_cur_gpu_freq = freq_new;

	if (g_pFreqSampler != NULL)
		g_pFreqSampler(freq_new);

	gpufreq_pr_debug("mt_gpu_clock_switch, freq_new = %d (KHz)\n",
		freq_new);

	if (g_pFreqSampler != NULL)
		g_pFreqSampler(freq_new);
}

static void mt_gpufreq_volt_switch(unsigned int volt_old, unsigned int volt_new)
{
	/* unsigned int delay_unit_us = 100; */

	gpufreq_pr_debug("@%s: volt_new = %d\n", __func__, volt_new);

/* #ifdef VGPU_SET_BY_PMIC */
#if 0
	if (volt_new > volt_old)
		regulator_set_voltage(
			mt_gpufreq_pmic->reg_vgpu,
			volt_new*10,
			(PMIC_MAX_VGPU*10) + 125);
	else
		regulator_set_voltage(
			mt_gpufreq_pmic->reg_vgpu,
			volt_new*10,
			volt_old*10);

	udelay(delay_unit_us);
#endif

	if (g_pVoltSampler != NULL)
		g_pVoltSampler(volt_new);
}

static unsigned int _mt_gpufreq_get_cur_freq(void)
{
	return g_cur_gpu_freq;
}

static unsigned int _mt_gpufreq_get_cur_volt(void)
{
	unsigned int gpu_volt = 0;

#if defined(VGPU_SET_BY_PMIC)
	/* WARRNING: regulator_get_voltage prints uV */
	gpu_volt = regulator_get_voltage(mt_gpufreq_pmic->reg_vgpu) / 10;
	gpufreq_pr_debug("gpu_dvfs_get_cur_volt:[PMIC] volt = %d\n", gpu_volt);
#else
	gpufreq_pr_debug("gpu_dvfs_get_cur_volt:[WARN] no tran value\n");
#endif

	return gpu_volt;
}

static void _mt_gpufreq_kick_pbm(int enable)
{
#ifndef DISABLE_PBM_FEATURE
	int i;
	int tmp_idx = -1;
	unsigned int found = 0;
	unsigned int power;
	unsigned int cur_volt = _mt_gpufreq_get_cur_volt();
	unsigned int cur_freq = _mt_gpufreq_get_cur_freq();

	if (enable) {
		for (i = 0; i < mt_gpufreqs_num; i++) {
			if (mt_gpufreqs_power[i].freq == cur_freq) {
				tmp_idx = i;

				if (mt_gpufreqs_power[i].volt == cur_volt) {
					power = mt_gpufreqs_power[i].power;
					found = 1;
					kicker_pbm_by_gpu(
						true, power, cur_volt/100);
					gpufreq_pr_debug("@%s: request GPU power = %d, cur_volt = %d, cur_freq = %d\n",
						__func__,
						power, cur_volt/100, cur_freq);
					return;
				}
			}
		}

		if (!found) {
			gpufreq_pr_debug("@%s: tmp_idx = %d\n",
				__func__, tmp_idx);

			if (tmp_idx != -1 && tmp_idx < mt_gpufreqs_num) {
				/* found corresponding power budget */
				power = mt_gpufreqs_power[tmp_idx].power;
				kicker_pbm_by_gpu(true, power, cur_volt / 100);
				gpufreq_pr_debug("@%s: request GPU power = %d, cur_volt = %d, cur_freq = %d\n",
					__func__,
					power, cur_volt/100, cur_freq);
			} else {
				gpufreq_pr_warn("@%s: Cannot found request power in power table!\n",
					__func__);
				gpufreq_pr_warn("cur_freq = %dKHz, cur_volt = %dmV\n",
					cur_freq, cur_volt / 100);
			}
		}
	} else
		kicker_pbm_by_gpu(false, 0, cur_volt / 100);
#endif
}

/***********************************************************
 * Frequency ramp up and ramp down handler
 *
 * [note]
 * 1. frequency ramp up need to wait voltage settle
 * 2. frequency ramp down do not need to wait voltage settle
 ************************************************************/
static void mt_gpufreq_set(
	unsigned int freq_old,
	unsigned int freq_new,
	unsigned int volt_old,
	unsigned int volt_new)
{
	gpufreq_pr_debug("@%s:old volt=%d, new volt=%d, old freq=%d, new freq=%d",
		__func__, volt_old, volt_new, freq_old, freq_new);

	if (freq_new > freq_old) {
		/* if(volt_old != volt_new) */
		/* { */
		mt_gpufreq_volt_switch(volt_old, volt_new);
		/* } */

		mt_gpufreq_clock_switch(freq_new);
	} else {
		mt_gpufreq_clock_switch(freq_new);

		/* if(volt_old != volt_new) */
		/* { */
		mt_gpufreq_volt_switch(volt_old, volt_new);
		/* } */
	}

	g_cur_gpu_freq = freq_new;
	g_cur_gpu_volt = volt_new;

	_mt_gpufreq_kick_pbm(1);
}

/*************************************************
 * gpufreq target callback function
 *
 * [note]
 * 1. handle frequency change request
 * 2. call mt_gpufreq_set to set target frequency
 **************************************************/
unsigned int mt_gpufreq_target(unsigned int idx)
{
	unsigned int target_freq, target_volt, target_idx, target_OPPidx;

#ifdef MT_GPUFREQ_PERFORMANCE_TEST
	return 0;
#endif

	mutex_lock(&mt_gpufreq_lock);

	if (mt_gpufreq_pause == true) {
		gpufreq_pr_warn("GPU DVFS pause!\n");
		mutex_unlock(&mt_gpufreq_lock);
		return DRIVER_NOT_READY;
	}

	if (mt_gpufreq_ready == false) {
		gpufreq_pr_warn("GPU DVFS not ready!\n");
		mutex_unlock(&mt_gpufreq_lock);
		return DRIVER_NOT_READY;
	}

	if (mt_volt_enable_state == 0) {
		gpufreq_pr_debug("mt_volt_enable_state == 0! return\n");
		mutex_unlock(&mt_gpufreq_lock);
		return DRIVER_NOT_READY;
	}
#ifdef MT_GPU_DVFS_RANDOM_TEST
	idx = mt_gpufreq_idx_get(5);
	gpufreq_pr_debug("@%s: random test index is %d !\n",
		__func__, idx);
#endif

	if (idx > (mt_gpufreqs_num - 1)) {
		mutex_unlock(&mt_gpufreq_lock);
		gpufreq_pr_err("@%s: idx out of range! idx = %d\n",
			__func__, idx);
		return -1;
	}

	/* look up for the target GPU OPP */
	target_freq = mt_gpufreqs[idx].freq;
	target_volt = mt_gpufreqs[idx].volt;
	target_idx = mt_gpufreqs[idx].idx;
	target_OPPidx = idx;

	gpufreq_pr_debug("@%s: begin, receive freq: %d, OPPidx: %d\n",
		__func__,
		target_freq,
		target_OPPidx);

	/* Check if need to keep max frequency */
	if (mt_gpufreq_keep_max_freq(g_cur_gpu_freq, target_freq)) {
		target_freq = mt_gpufreqs[g_gpufreq_max_id].freq;
		target_volt = mt_gpufreqs[g_gpufreq_max_id].volt;
		target_idx = mt_gpufreqs[g_gpufreq_max_id].idx;
		target_OPPidx = g_gpufreq_max_id;

		gpufreq_pr_debug("Keep MAX frequency %d !\n", target_freq);
	}

	/* If /proc command keep opp frequency. */
	if (mt_keep_opp_frequency_state == true) {
		target_freq = mt_gpufreqs[mt_keep_opp_index].freq;
		target_volt = mt_gpufreqs[mt_keep_opp_index].volt;
		target_idx = mt_gpufreqs[mt_keep_opp_index].idx;
		target_OPPidx = mt_keep_opp_index;

		gpufreq_pr_debug("Keep opp! opp frequency %d, opp voltage %d, opp idx %d\n",
			target_freq, target_volt, target_OPPidx);
	}

	/* If /proc command keep opp max frequency. */
	if (mt_opp_max_frequency_state == true) {
		if (target_freq > mt_opp_max_frequency) {
			target_freq = mt_gpufreqs[mt_opp_max_index].freq;
			target_volt = mt_gpufreqs[mt_opp_max_index].volt;
			target_idx = mt_gpufreqs[mt_opp_max_index].idx;
			target_OPPidx = mt_opp_max_index;

			gpufreq_pr_debug("opp max freq! opp max frequency %d, opp max voltage %d, opp max idx %d\n",
				target_freq, target_volt, target_OPPidx);
		}
	}

	/* PBM limit */
#ifndef DISABLE_PBM_FEATURE
	if (mt_pbm_limited_index != 0) {
		if (target_freq > mt_gpufreqs[mt_pbm_limited_index].freq) {
			/* target_freq > limited_freq, need to adjust */
			target_freq = mt_gpufreqs[mt_pbm_limited_index].freq;
			target_volt = mt_gpufreqs[mt_pbm_limited_index].volt;
			target_OPPidx = mt_pbm_limited_index;

			gpufreq_pr_debug("Limit! Thermal/Power limit gpu frequency %d\n",
				mt_gpufreqs[mt_pbm_limited_index].freq);
		}
	}
#endif

	/* Thermal/Power limit */
	if (g_limited_max_id != 0) {
		if (target_freq > mt_gpufreqs[g_limited_max_id].freq) {
			/* target_freq > limited_freq, need to adjust */
			target_freq = mt_gpufreqs[g_limited_max_id].freq;
			target_volt = mt_gpufreqs[g_limited_max_id].volt;
			target_idx = mt_gpufreqs[g_limited_max_id].idx;
			target_OPPidx = g_limited_max_id;

			gpufreq_pr_debug("Limit! Thermal/Power limit gpu frequency %d\n",
				mt_gpufreqs[g_limited_max_id].freq);
		}
	}

	/* DVFS keep at max freq when PTPOD initial */
	if (mt_ptpod_disable == true) {
		target_freq = mt_gpufreqs[mt_ptpod_disable_idx].freq;
		target_volt = GPU_DVFS_PTPOD_DISABLE_VOLT;
		target_idx = mt_gpufreqs[mt_ptpod_disable_idx].idx;
		target_OPPidx = mt_ptpod_disable_idx;

		gpufreq_pr_debug("PTPOD disable dvfs, mt_ptpod_disable_idx = %d\n",
			mt_ptpod_disable_idx);
	}

	/* If /proc command fix the frequency. */
	if (mt_fixed_freq_volt_state == true) {
		target_freq = mt_fixed_frequency;
		target_volt = mt_fixed_voltage;
		target_idx = 0;
		target_OPPidx = 0;

		gpufreq_pr_debug("Fixed! fixed frequency %d, fixed voltage %d\n",
			target_freq, target_volt);
	}

	/* target frequency == current frequency, skip it */
	if (g_cur_gpu_freq == target_freq && g_cur_gpu_volt == target_volt) {
		mutex_unlock(&mt_gpufreq_lock);
		gpufreq_pr_debug("GPU frequency from %d KHz to %d KHz (skipped) due to same frequency\n",
			g_cur_gpu_freq, target_freq);
		return 0;
	}

	gpufreq_pr_debug("GPU current frequency %d KHz, target frequency %d KHz\n",
		g_cur_gpu_freq, target_freq);

#ifdef MT_GPUFREQ_AEE_RR_REC
	aee_rr_rec_gpu_dvfs_status(aee_rr_curr_gpu_dvfs_status() |
		(1 << GPU_DVFS_IS_DOING_DVFS));
	aee_rr_rec_gpu_dvfs_oppidx(target_OPPidx);
#endif

	/* set to the target frequency */
	mt_gpufreq_set(
		g_cur_gpu_freq,
		target_freq,
		g_cur_gpu_volt,
		target_volt);

	g_cur_gpu_idx = target_idx;
	g_cur_gpu_OPPidx = target_OPPidx;

#ifdef MT_GPUFREQ_AEE_RR_REC
	aee_rr_rec_gpu_dvfs_status(aee_rr_curr_gpu_dvfs_status() &
		~(1 << GPU_DVFS_IS_DOING_DVFS));
#endif

	mutex_unlock(&mt_gpufreq_lock);
	return 0;
}
EXPORT_SYMBOL(mt_gpufreq_target);

/* POWER LIMIT RELATED */
enum {
	IDX_THERMAL_LIMITED,
	IDX_LOW_BATT_VOLT_LIMITED,
	IDX_LOW_BATT_VOLUME_LIMITED,
	IDX_OC_LIMITED,
	NR_IDX_POWER_LIMITED,
};

/* NO need to throttle when OC */
#ifdef MT_GPUFREQ_OC_PROTECT
static unsigned int mt_oc_level;

/* no need to throttle when OC */
#define MT_GPUFREQ_OC_LIMIT_FREQ_1 GPU_DVFS_FREQ4
/* unlimit frequency, index = 0. */
static unsigned int mt_oc_limited_index_0;
static unsigned int mt_oc_limited_index_1;
/* Limited frequency index for oc */
static unsigned int mt_oc_limited_index;
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT
static unsigned int mt_low_battery_volume;

#define MT_GPUFREQ_LOW_BATT_VOLUME_LIMIT_FREQ_1 GPU_DVFS_FREQ0
/* unlimit frequency, index = 0. */
static unsigned int mt_low_bat_volume_limited_index_0;
static unsigned int mt_low_bat_volume_limited_index_1;
/* Limited frequency index for low battery volume */
static unsigned int mt_low_batt_volume_limited_index;
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
static unsigned int mt_low_battery_level;

/* no need to throttle when LV1 */
#define MT_GPUFREQ_LOW_BATT_VOLT_LIMIT_FREQ_1	 GPU_DVFS_FREQ0
#define MT_GPUFREQ_LOW_BATT_VOLT_LIMIT_FREQ_2	 GPU_DVFS_FREQ4
/* unlimit frequency, index = 0. */
static unsigned int mt_low_bat_volt_limited_index_0;
static unsigned int mt_low_bat_volt_limited_index_1;
static unsigned int mt_low_bat_volt_limited_index_2;
/* Limited frequency index for low battery voltage */
static unsigned int mt_low_batt_volt_limited_index;
#endif

/* thermal limit power */
static unsigned int mt_thermal_limited_gpu_power;
/* thermal limited freq */
static unsigned int mt_prev_thermal_limited_freq;
/* limit frequency index array */
static unsigned int mt_power_limidx_array[NR_IDX_POWER_LIMITED] = { 0 };

/* Frequency adjust interface for thermal protect */
static int mt_gpufreq_power_throttle_protect(void)
{
	int ret = 0;
	int i = 0;
	unsigned int limited_index = 0;

	/* Check lowest frequency in all limitation */
	for (i = 0; i < NR_IDX_POWER_LIMITED; i++) {
		if (mt_power_limidx_array[i] != 0 && limited_index == 0)
			limited_index = mt_power_limidx_array[i];
		else if (mt_power_limidx_array[i] != 0 && limited_index != 0) {
			if (mt_power_limidx_array[i] > limited_index)
				limited_index = mt_power_limidx_array[i];
		}
	}

	g_limited_max_id = limited_index;

	if (g_power_limit_notify != NULL)
		g_power_limit_notify(g_limited_max_id);

	return ret;
}

#ifdef MT_GPUFREQ_OC_PROTECT
/* GPU frequency adjust interface for oc protect */
static void mt_gpufreq_oc_protect(unsigned int limited_index)
{
	mutex_lock(&mt_gpufreq_power_lock);

	gpufreq_pr_debug("@%s: limited_index = %d\n", __func__, limited_index);

	mt_power_limidx_array[IDX_OC_LIMITED] = limited_index;
	mt_gpufreq_power_throttle_protect();

	mutex_unlock(&mt_gpufreq_power_lock);
}

void mt_gpufreq_oc_callback(enum BATTERY_OC_LEVEL oc_level)
{
	gpufreq_pr_debug("@%s: oc_level = %d\n", __func__, oc_level);

	if (mt_gpufreq_ready == false) {
		gpufreq_pr_warn("@%s: GPU DVFS not ready!\n", __func__);
		return;
	}

	if (g_limited_oc_ignore_state == true) {
		gpufreq_pr_debug("g_limited_oc_ignore_state == true!\n");
		return;
	}

	mt_oc_level = oc_level;

	/* BATTERY_OC_LEVEL_1: >= 5.5A  */
	if (oc_level == BATTERY_OC_LEVEL_1) {
		if (mt_oc_limited_index != mt_oc_limited_index_1) {
			mt_oc_limited_index = mt_oc_limited_index_1;
			/* Limit GPU 396.5Mhz */
			mt_gpufreq_oc_protect(mt_oc_limited_index_1);
		}
	}
	/* unlimit gpu */
	else {
		if (mt_oc_limited_index != mt_oc_limited_index_0) {
			mt_oc_limited_index = mt_oc_limited_index_0;
			mt_gpufreq_oc_protect(mt_oc_limited_index_0);
		}
	}
}
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT
/* GPU frequency adjust interface for low bat_volume protect */
static void mt_gpufreq_low_batt_volume_protect(unsigned int limited_index)
{
	mutex_lock(&mt_gpufreq_power_lock);

	gpufreq_pr_debug("@%s: limited_index = %d\n", __func__, limited_index);

	mt_power_limidx_array[IDX_LOW_BATT_VOLUME_LIMITED] = limited_index;
	mt_gpufreq_power_throttle_protect();

	mutex_unlock(&mt_gpufreq_power_lock);
}

void mt_gpufreq_low_batt_volume_callback(
	enum BATTERY_PERCENT_LEVEL low_battery_volume)
{
	gpufreq_pr_debug("@%s: low_battery_volume = %d\n",
		__func__, low_battery_volume);

	if (mt_gpufreq_ready == false) {
		gpufreq_pr_warn("@%s: GPU DVFS not ready!\n", __func__);
		return;
	}

	if (g_limited_low_batt_volume_ignore_state == true) {
		gpufreq_pr_debug("g_limited_low_batt_volume_ignore_state == true!\n");
		return;
	}

	mt_low_battery_volume = low_battery_volume;

	/* LOW_BATTERY_VOLUME_1: <= 15%, LOW_BATTERY_VOLUME_0: >15% */
	if (low_battery_volume == BATTERY_PERCENT_LEVEL_1) {
		if (mt_low_batt_volume_limited_index !=
			mt_low_bat_volume_limited_index_1) {
			mt_low_batt_volume_limited_index =
				mt_low_bat_volume_limited_index_1;

			/* Unlimited */
			mt_gpufreq_low_batt_volume_protect(
				mt_low_bat_volume_limited_index_1);
		}
	}
	/* unlimit gpu */
	else {
		if (mt_low_batt_volume_limited_index !=
			mt_low_bat_volume_limited_index_0) {
			mt_low_batt_volume_limited_index =
				mt_low_bat_volume_limited_index_0;
			mt_gpufreq_low_batt_volume_protect(
				mt_low_bat_volume_limited_index_0);
		}
	}
}
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
/* GPU frequency adjust interface for low bat_volt protect */
static void mt_gpufreq_low_batt_volt_protect(unsigned int limited_index)
{
	mutex_lock(&mt_gpufreq_power_lock);

	gpufreq_pr_debug("@%s: limited_index = %d\n", __func__, limited_index);

	mt_power_limidx_array[IDX_LOW_BATT_VOLT_LIMITED] = limited_index;
	mt_gpufreq_power_throttle_protect();

	mutex_unlock(&mt_gpufreq_power_lock);
}

void mt_gpufreq_low_batt_volt_callback(enum LOW_BATTERY_LEVEL low_battery_level)
{
	gpufreq_pr_debug("@%s: low_battery_level = %d\n",
		__func__, low_battery_level);

	if (mt_gpufreq_ready == false) {
		gpufreq_pr_warn("@%s: GPU DVFS not ready!\n", __func__);
		return;
	}

	if (g_limited_low_batt_volt_ignore_state == true) {
		gpufreq_pr_debug("g_limited_low_batt_volt_ignore_state == true!\n");
		return;
	}

	mt_low_battery_level = low_battery_level;

	/* is_low_battery=1:need limit HW, is_low_battery=0:no limit */
	/* 3.25V HW issue int and is_low_battery=1,
	 * 3.0V HW issue int and is_low_battery=2,
	 * 3.5V HW issue int and is_low_battery=0
	 */

	/* no need to throttle when LV1 */
	if (low_battery_level == LOW_BATTERY_LEVEL_1) {
		if (mt_low_batt_volt_limited_index !=
				mt_low_bat_volt_limited_index_1) {
			mt_low_batt_volt_limited_index =
				mt_low_bat_volt_limited_index_1;
			/* Limit GPU 416Mhz */
			mt_gpufreq_low_batt_volt_protect(
				mt_low_bat_volt_limited_index_1);
		}
	} else if (low_battery_level == LOW_BATTERY_LEVEL_2) {
		if (mt_low_batt_volt_limited_index !=
				mt_low_bat_volt_limited_index_2) {
			mt_low_batt_volt_limited_index =
				mt_low_bat_volt_limited_index_2;
			/* Limit GPU 400Mhz */
			mt_gpufreq_low_batt_volt_protect(
				mt_low_bat_volt_limited_index_2);
		}
	} else {
		/* unlimit gpu */
		if (mt_low_batt_volt_limited_index !=
				mt_low_bat_volt_limited_index_0) {
			mt_low_batt_volt_limited_index =
				mt_low_bat_volt_limited_index_0;
			/* Unlimit */
			mt_gpufreq_low_batt_volt_protect(
				mt_low_bat_volt_limited_index_0);
		}
	}
}
#endif

/* Frequency adjust interface for thermal protect */
static unsigned int _mt_gpufreq_get_limited_freq(unsigned int limited_power)
{
	int i = 0;
	unsigned int limited_freq = 0;
	unsigned int found = 0;

	for (i = 0; i < mt_gpufreqs_num; i++) {
		if (mt_gpufreqs_power[i].power <= limited_power) {
			limited_freq = mt_gpufreqs_power[i].freq;
			found = 1;
			break;
		}
	}

	/* not found */
	if (!found)
		limited_freq = mt_gpufreqs_power[mt_gpufreqs_num-1].freq;

	gpufreq_pr_debug("@%s: limited_freq = %d\n", __func__, limited_freq);

	return limited_freq;
}

void mt_gpufreq_thermal_protect(unsigned int limited_power)
{
	int i = 0;
	unsigned int limited_freq = 0;

	mutex_lock(&mt_gpufreq_power_lock);

	if (mt_gpufreq_ready == false) {
		gpufreq_pr_warn("@%s: GPU DVFS not ready!\n", __func__);
		mutex_unlock(&mt_gpufreq_power_lock);
		return;
	}

	if (mt_gpufreqs_num == 0) {
		gpufreq_pr_warn("@%s: mt_gpufreqs_num == 0!\n", __func__);
		mutex_unlock(&mt_gpufreq_power_lock);
		return;
	}

	if (g_limited_thermal_ignore_state == true) {
		gpufreq_pr_debug("g_limited_thermal_ignore_state == true!\n");
		mutex_unlock(&mt_gpufreq_power_lock);
		return;
	}

	mt_thermal_limited_gpu_power = limited_power;

#ifdef MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE
	mt_update_gpufreqs_power_table();
#endif

	if (limited_power == 0)
		mt_power_limidx_array[IDX_THERMAL_LIMITED] = 0;
	else {
		limited_freq = _mt_gpufreq_get_limited_freq(limited_power);

		for (i = 0; i < mt_gpufreqs_num; i++) {
			if (mt_gpufreqs[i].freq <= limited_freq) {
				mt_power_limidx_array[IDX_THERMAL_LIMITED] = i;
				break;
			}
		}
	}

	if (mt_prev_thermal_limited_freq != limited_freq) {
		mt_prev_thermal_limited_freq = limited_freq;
		mt_gpufreq_power_throttle_protect();
		if (limited_freq < GPU_DVFS_FREQ5)
			gpufreq_pr_debug("@%s: p %u f %u i %u\n",
				__func__,
				limited_power,
				limited_freq,
				mt_power_limidx_array[IDX_THERMAL_LIMITED]);
	}

	mutex_unlock(&mt_gpufreq_power_lock);
}
EXPORT_SYMBOL(mt_gpufreq_thermal_protect);

/* For thermal to update power budget */
unsigned int mt_gpufreq_get_max_power(void)
{
	if (!mt_gpufreqs_power)
		return 0;

	return mt_gpufreqs_power[0].power;
}

/* For thermal to update power budget */
unsigned int mt_gpufreq_get_min_power(void)
{
	if (!mt_gpufreqs_power)
		return 0;

	return mt_gpufreqs_power[mt_gpufreqs_num - 1].power;
}

void mt_gpufreq_set_power_limit_by_pbm(unsigned int limited_power)
{
#ifndef DISABLE_PBM_FEATURE
	int i = 0;
	unsigned int limited_freq = 0;

	mutex_lock(&mt_gpufreq_power_lock);

	if (mt_gpufreq_ready == false) {
		gpufreq_pr_warn("@%s: GPU DVFS not ready!\n", __func__);
		mutex_unlock(&mt_gpufreq_power_lock);
		return;
	}

	if (mt_gpufreqs_num == 0) {
		gpufreq_pr_warn("@%s: mt_gpufreqs_num == 0!\n", __func__);
		mutex_unlock(&mt_gpufreq_power_lock);
		return;
	}

	if (g_limited_pbm_ignore_state == true) {
		gpufreq_pr_debug("g_limited_pbm_ignore_state == true!\n");
		mutex_unlock(&mt_gpufreq_power_lock);
		return;
	}

	if (limited_power == mt_pbm_limited_gpu_power) {
		gpufreq_pr_debug("limited_power(%d mW) not changed, skip it!\n",
			limited_power);
		mutex_unlock(&mt_gpufreq_power_lock);
		return;
	}

	mt_pbm_limited_gpu_power = limited_power;

	gpufreq_pr_debug("@%s: limited_power = %d\n", __func__, limited_power);

#ifdef MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE
	/* TODO: need to check overhead? */
	mt_update_gpufreqs_power_table();
#endif

	if (limited_power == 0)
		mt_pbm_limited_index = 0;
	else {
		limited_freq = _mt_gpufreq_get_limited_freq(limited_power);

		for (i = 0; i < mt_gpufreqs_num; i++) {
			if (mt_gpufreqs[i].freq <= limited_freq) {
				mt_pbm_limited_index = i;
				break;
			}
		}
	}

	gpufreq_pr_debug("PBM limit frequency upper bound to id = %d\n",
		mt_pbm_limited_index);

	if (g_power_limit_notify != NULL)
		g_power_limit_notify(mt_pbm_limited_index);

	mutex_unlock(&mt_gpufreq_power_lock);
#endif
}

#if 0
unsigned int mt_gpufreq_get_leakage_mw(void)
{
#ifndef DISABLE_PBM_FEATURE
	int temp = 0;

#ifdef STATIC_PWR_READY2USE
	unsigned int cur_vcore = _mt_gpufreq_get_cur_volt() / 100;
	int leak_power;
#endif

#ifdef CONFIG_THERMAL
	temp = get_immediate_gpu_wrap() / 1000;
#else
	temp = 40;
#endif

#ifdef STATIC_PWR_READY2USE
	leak_power = mt_spower_get_leakage(MT_SPOWER_GPU, cur_vcore, temp);
	if (mt_volt_enable_state && leak_power > 0)
		return leak_power;
	else
		return 0;
#else
	return 130;
#endif

#else /* DISABLE_PBM_FEATURE */
	return 0;
#endif
}
#endif

/* Return current GPU thermal limit index */
unsigned int mt_gpufreq_get_thermal_limit_index(void)
{
	gpufreq_pr_debug("current GPU thermal limit index is %d\n",
		g_limited_max_id);
	return g_limited_max_id;
}
EXPORT_SYMBOL(mt_gpufreq_get_thermal_limit_index);

/* Return current GPU thermal limit frequency */
unsigned int mt_gpufreq_get_thermal_limit_freq(void)
{
	gpufreq_pr_debug("current GPU thermal limit freq is %d MHz\n",
		mt_gpufreqs[g_limited_max_id].freq / 1000);
	return mt_gpufreqs[g_limited_max_id].freq;
}
EXPORT_SYMBOL(mt_gpufreq_get_thermal_limit_freq);

/* Return current GPU frequency index */
unsigned int mt_gpufreq_get_cur_freq_index(void)
{
	gpufreq_pr_debug("current GPU frequency OPP index is %d\n",
		g_cur_gpu_OPPidx);
	return g_cur_gpu_OPPidx;
}
EXPORT_SYMBOL(mt_gpufreq_get_cur_freq_index);

/* Return current GPU frequency */
unsigned int mt_gpufreq_get_cur_freq(void)
{
#ifdef MTK_GPU_SPM
	return _mt_gpufreq_get_cur_freq();
#else
	gpufreq_pr_debug("current GPU frequency is %d MHz\n",
		g_cur_gpu_freq / 1000);
	return g_cur_gpu_freq;
#endif
}
EXPORT_SYMBOL(mt_gpufreq_get_cur_freq);

/* Return current GPU voltage */
unsigned int mt_gpufreq_get_cur_volt(void)
{
	return _mt_gpufreq_get_cur_volt();
}
EXPORT_SYMBOL(mt_gpufreq_get_cur_volt);

/* register/unregister GPU power limit notifiction CB */
void mt_gpufreq_power_limit_notify_registerCB(gpufreq_power_limit_notify pCB)
{
	g_power_limit_notify = pCB;
}
EXPORT_SYMBOL(mt_gpufreq_power_limit_notify_registerCB);

/* register/unregister ptpod update GPU volt CB */
void mt_gpufreq_update_volt_registerCB(gpufreq_ptpod_update_notify pCB)
{
	g_ptpod_update_notify = pCB;
}
EXPORT_SYMBOL(mt_gpufreq_update_volt_registerCB);

/* register/unregister set GPU freq CB */
void mt_gpufreq_setfreq_registerCB(sampler_func pCB)
{
	g_pFreqSampler = pCB;
}
EXPORT_SYMBOL(mt_gpufreq_setfreq_registerCB);

/* register/unregister set GPU volt CB */
void mt_gpufreq_setvolt_registerCB(sampler_func pCB)
{
	g_pVoltSampler = pCB;
}
EXPORT_SYMBOL(mt_gpufreq_setvolt_registerCB);

/* For ptpod used to open gpu external/internal power */
void mt_gpufreq_mfgclock_notify_registerCB(
	gpufreq_mfgclock_notify pEnableCB,
	gpufreq_mfgclock_notify pDisableCB)
{
	g_mfgclock_enable_notify = pEnableCB;
	g_mfgclock_disable_notify = pDisableCB;
}
EXPORT_SYMBOL(mt_gpufreq_mfgclock_notify_registerCB);

#ifdef CONFIG_HAS_EARLYSUSPEND
/* Early suspend callback function */
void mt_gpufreq_early_suspend(struct early_suspend *h)
{
	/* mt_gpufreq_state_set(0); */
}

/* Late resume callback function */
void mt_gpufreq_late_resume(struct early_suspend *h)
{
	/* mt_gpufreq_check_freq_and_set_pll(); */
	/* mt_gpufreq_state_set(1); */
}
#endif

static int mt_gpufreq_pm_restore_early(struct device *dev)
{
	int i = 0;
	int found = 0;

	g_cur_gpu_freq = _mt_gpufreq_get_cur_freq();

	for (i = 0; i < mt_gpufreqs_num; i++) {
		if (g_cur_gpu_freq == mt_gpufreqs[i].freq) {
			g_cur_gpu_idx = mt_gpufreqs[i].idx;
			g_cur_gpu_volt = mt_gpufreqs[i].volt;
			g_cur_gpu_OPPidx = i;
			found = 1;

			gpufreq_pr_debug("match g_cur_gpu_OPPidx: %d\n",
				g_cur_gpu_OPPidx);
			break;
		}
	}

	if (found == 0) {
		g_cur_gpu_idx = mt_gpufreqs[0].idx;
		g_cur_gpu_volt = mt_gpufreqs[0].volt;
		g_cur_gpu_OPPidx = 0;

		gpufreq_pr_err("gpu freq not found, set parameter to max freq\n");
	}

	gpufreq_pr_debug("GPU freq SW/HW: %d/%d\n",
		g_cur_gpu_freq, _mt_gpufreq_get_cur_freq());
	gpufreq_pr_debug("g_cur_gpu_OPPidx: %d\n",
		g_cur_gpu_OPPidx);

	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id mt_gpufreq_of_match[] = {
	{.compatible = "mediatek,mt8167-gpufreq",},
	{ },
};
#endif
MODULE_DEVICE_TABLE(of, mt_gpufreq_of_match);
static int mt_gpufreq_pdrv_probe(struct platform_device *pdev)
{
#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
	int i = 0;
#endif

#ifdef CONFIG_OF
	struct device_node *node;

	node = of_find_matching_node(NULL, mt_gpufreq_of_match);
	if (!node)
		gpufreq_pr_err("@%s: find GPU node failed\n", __func__);

	/* alloc PMIC regulator */
	mt_gpufreq_pmic = kzalloc(sizeof(struct mt_gpufreq_pmic_t), GFP_KERNEL);
	if (mt_gpufreq_pmic == NULL)
		return -ENOMEM;

	mt_gpufreq_pmic->reg_vgpu = regulator_get(&pdev->dev, "reg-vgpu");
	if (IS_ERR(mt_gpufreq_pmic->reg_vgpu)) {
		dev_err(&pdev->dev, "cannot get reg-vgpu\n");
		return PTR_ERR(mt_gpufreq_pmic->reg_vgpu);
	}
#endif

	mt_dvfs_table_type = mt_gpufreq_get_dvfs_table_type();

#ifdef CONFIG_HAS_EARLYSUSPEND
	mt_early_suspend_handler.suspend = mt_gpufreq_early_suspend;
	mt_early_suspend_handler.resume = mt_gpufreq_late_resume;
	register_early_suspend(&mt_early_suspend_handler);
#endif

	/* Initial leackage power usage */
#ifdef STATIC_PWR_READY2USE
	mt_spower_init();
#endif

	/* Initial SRAM debugging ptr */
#ifdef MT_GPUFREQ_AEE_RR_REC
	_mt_gpufreq_aee_init();
#endif

	/* Setup gpufreq table */
	gpufreq_pr_debug("setup gpufreqs table\n");

	switch (mt_dvfs_table_type) {
	case 0:		/* 400MHz */
		mt_setup_gpufreqs_table(mt_opp_tbl_0, ARRAY_SIZE(mt_opp_tbl_0));
		break;
	case 1:		/* 500MHz */
		mt_setup_gpufreqs_table(mt_opp_tbl_1, ARRAY_SIZE(mt_opp_tbl_1));
		break;
	case 2:		/* 600MHz */
		mt_setup_gpufreqs_table(mt_opp_tbl_2, ARRAY_SIZE(mt_opp_tbl_2));
		break;
	default:	/* 400MHz */
		mt_setup_gpufreqs_table(mt_opp_tbl_0, ARRAY_SIZE(mt_opp_tbl_0));
		break;
	}

	/* Setup PMIC init value */
#ifdef VGPU_SET_BY_PMIC
	gpufreq_pr_debug("VGPU Enabled (%d) %d mV\n",
		regulator_is_enabled(mt_gpufreq_pmic->reg_vgpu),
		_mt_gpufreq_get_cur_volt());

#ifdef MT_GPUFREQ_AEE_RR_REC
	aee_rr_rec_gpu_dvfs_status(aee_rr_curr_gpu_dvfs_status() |
		(1 << GPU_DVFS_IS_VGPU_ENABLED));
#endif
	mt_volt_enable_state = 1;
#endif

	/* Setup initial frequency */
	mt_gpufreq_set_initial();

	g_cur_freq_init_keep = g_cur_gpu_freq;

	gpufreq_pr_debug("GPU current frequency = %dKHz\n",
		_mt_gpufreq_get_cur_freq());
	gpufreq_pr_debug("Current Vcore = %dmV\n",
		_mt_gpufreq_get_cur_volt() / 100);
	gpufreq_pr_debug("g_cur_gpu_freq = %d, g_cur_gpu_volt = %d\n",
		g_cur_gpu_freq, g_cur_gpu_volt);
	gpufreq_pr_debug("g_cur_gpu_idx = %d, g_cur_gpu_OPPidx = %d\n",
		g_cur_gpu_idx, g_cur_gpu_OPPidx);

	mt_gpufreq_ready = true;

#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
	for (i = 0; i < mt_gpufreqs_num; i++) {
		if (mt_gpufreqs[i].freq ==
			MT_GPUFREQ_LOW_BATT_VOLT_LIMIT_FREQ_1) {
			mt_low_bat_volt_limited_index_1 = i;
			break;
		}
	}

	for (i = 0; i < mt_gpufreqs_num; i++) {
		if (mt_gpufreqs[i].freq ==
			MT_GPUFREQ_LOW_BATT_VOLT_LIMIT_FREQ_2) {
			mt_low_bat_volt_limited_index_2 = i;
			break;
		}
	}

	/*
	 * register_low_battery_notify(
	 * &mt_gpufreq_low_batt_volt_callback, LOW_BATTERY_PRIO_GPU);
	 */
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT
	for (i = 0; i < mt_gpufreqs_num; i++) {
		if (mt_gpufreqs[i].freq ==
			MT_GPUFREQ_LOW_BATT_VOLUME_LIMIT_FREQ_1) {
			mt_low_bat_volume_limited_index_1 = i;
			break;
		}
	}

	/* register_battery_percent_notify(
	 *	&mt_gpufreq_low_batt_volume_callback, BATTERY_PERCENT_PRIO_GPU);
	 */
#endif

#ifdef MT_GPUFREQ_OC_PROTECT
	for (i = 0; i < mt_gpufreqs_num; i++) {
		if (mt_gpufreqs[i].freq == MT_GPUFREQ_OC_LIMIT_FREQ_1) {
			mt_oc_limited_index_1 = i;
			break;
		}
	}

	/*
	 * register_battery_oc_notify(
	 *	&mt_gpufreq_oc_callback, BATTERY_OC_PRIO_GPU);
	 */
#endif

#ifndef DISABLE_PBM_FEATURE
	INIT_DEFERRABLE_WORK(&notify_pbm_gpuoff_work,
		mt_gpufreq_notify_pbm_gpuoff);
#endif

	return 0;
}

/* This function should never be called */
static int mt_gpufreq_pdrv_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct dev_pm_ops mt_gpufreq_pm_ops = {
	.suspend = NULL,
	.resume = NULL,
	.restore_early = mt_gpufreq_pm_restore_early,
};
static struct platform_driver mt_gpufreq_pdrv = {
	.probe = mt_gpufreq_pdrv_probe,
	.remove = mt_gpufreq_pdrv_remove,
	.driver = {
		   .name = "gpufreq",
		   .pm = &mt_gpufreq_pm_ops,
		   .owner = THIS_MODULE,
#ifdef CONFIG_OF
		   .of_match_table = mt_gpufreq_of_match,
#endif
		   },
};

#ifdef CONFIG_PROC_FS
/* Show current debug status */
static int mt_gpufreq_debug_proc_show(struct seq_file *m, void *v)
{
	if (mt_gpufreq_debug)
		seq_puts(m, "gpufreq debug enabled\n");
	else
		seq_puts(m, "gpufreq debug disabled\n");

	return 0;
}

/* Enable debug message */
static ssize_t mt_gpufreq_debug_proc_write(
	struct file *file,
	const char __user *buffer,
	size_t count,
	loff_t *data)
{
	char desc[32];
	int len = 0;
	int debug = 0;

	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
		return 0;

	desc[len] = '\0';

	if (kstrtoint(desc, 0, &debug) == 0) {
		if (debug == 0)
			mt_gpufreq_debug = 0;
		else if (debug == 1)
			mt_gpufreq_debug = 1;
		else
			gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: disable, 1: enable]\n");
	} else
		gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: disable, 1: enable]\n");

	return count;
}

#ifdef MT_GPUFREQ_OC_PROTECT
/* Show current limited by low batt volume */
static int mt_gpufreq_limited_oc_ignore_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "g_limited_max_id = %d, g_limited_oc_ignore_state = %d\n",
		g_limited_max_id, g_limited_oc_ignore_state);
	return 0;
}

/* Limited for low batt volume protect */
static ssize_t mt_gpufreq_limited_oc_ignore_proc_write(struct file *file,
	const char __user *buffer, size_t count, loff_t *data)
{
	char desc[32];
	int len = 0;
	unsigned int ignore = 0;

	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
		return 0;

	desc[len] = '\0';

	if (kstrtouint(desc, 0, &ignore) == 0) {
		if (ignore == 1)
			g_limited_oc_ignore_state = true;
		else if (ignore == 0)
			g_limited_oc_ignore_state = false;
		else
			gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");
	} else
		gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");

	return count;
}
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT
/* Show current limited by low batt volume */
static int mt_gpufreq_limited_low_batt_volume_ignore_proc_show(
	struct seq_file *m, void *v)
{
	seq_printf(m, "g_limited_max_id = %d, g_limited_low_batt_volume_ignore_state = %d\n",
		g_limited_max_id, g_limited_low_batt_volume_ignore_state);
	return 0;
}

/* Limited for low batt volume protect */
static ssize_t mt_gpufreq_limited_low_batt_volume_ignore_proc_write(
	struct file *file,
	const char __user *buffer,
	size_t count,
	loff_t *data)
{
	char desc[32];
	int len = 0;
	unsigned int ignore = 0;

	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
		return 0;

	desc[len] = '\0';

	if (kstrtouint(desc, 0, &ignore) == 0) {
		if (ignore == 1)
			g_limited_low_batt_volume_ignore_state = true;
		else if (ignore == 0)
			g_limited_low_batt_volume_ignore_state = false;
		else
			gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");
	} else
		gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");

	return count;
}
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
/* Show current limited by low batt volt */
static int mt_gpufreq_limited_low_batt_volt_ignore_proc_show(
	struct seq_file *m, void *v)
{
	seq_printf(m, "g_limited_max_id = %d, g_limited_low_batt_volt_ignore_state = %d\n",
		g_limited_max_id, g_limited_low_batt_volt_ignore_state);
	return 0;
}

/* Limited for low batt volt protect */
static ssize_t mt_gpufreq_limited_low_batt_volt_ignore_proc_write(
	struct file *file,
	const char __user *buffer,
	size_t count,
	loff_t *data)
{
	char desc[32];
	int len = 0;
	unsigned int ignore = 0;

	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
		return 0;

	desc[len] = '\0';

	if (kstrtouint(desc, 0, &ignore) == 0) {
		if (ignore == 1)
			g_limited_low_batt_volt_ignore_state = true;
		else if (ignore == 0)
			g_limited_low_batt_volt_ignore_state = false;
		else
			gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");
	} else
		gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");

	return count;
}
#endif

/* Show current limited by thermal */
static int mt_gpufreq_limited_thermal_ignore_proc_show(
	struct seq_file *m, void *v)
{
	seq_printf(m, "g_limited_max_id = %d, g_limited_thermal_ignore_state = %d\n",
		g_limited_max_id, g_limited_thermal_ignore_state);
	return 0;
}

/* Limited for thermal protect */
static ssize_t mt_gpufreq_limited_thermal_ignore_proc_write(struct file *file,
	const char __user *buffer, size_t count, loff_t *data)
{
	char desc[32];
	int len = 0;
	unsigned int ignore = 0;

	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
		return 0;

	desc[len] = '\0';

	if (kstrtouint(desc, 0, &ignore) == 0) {
		if (ignore == 1)
			g_limited_thermal_ignore_state = true;
		else if (ignore == 0)
			g_limited_thermal_ignore_state = false;
		else
			gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");
	} else
		gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");

	return count;
}

#ifndef DISABLE_PBM_FEATURE
/* Show current limited by PBM */
static int mt_gpufreq_limited_pbm_ignore_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "g_limited_max_id = %d, g_limited_oc_ignore_state = %d\n",
		g_limited_max_id, g_limited_pbm_ignore_state);
	return 0;
}

/* Limited for low batt volume protect */
static ssize_t mt_gpufreq_limited_pbm_ignore_proc_write(struct file *file,
	const char __user *buffer, size_t count, loff_t *data)
{
	char desc[32];
	int len = 0;
	unsigned int ignore = 0;

	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
		return 0;

	desc[len] = '\0';

	if (kstrtouint(desc, 0, &ignore) == 0) {
		if (ignore == 1)
			g_limited_pbm_ignore_state = true;
		else if (ignore == 0)
			g_limited_pbm_ignore_state = false;
		else
			gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");
	} else
		gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: not ignore, 1: ignore]\n");

	return count;
}
#endif

/* Show current limited power */
static int mt_gpufreq_limited_power_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "g_limited_max_id = %d, limit frequency = %d\n",
		g_limited_max_id, mt_gpufreqs[g_limited_max_id].freq);
	return 0;
}

/* Limited power for thermal protect */
static ssize_t mt_gpufreq_limited_power_proc_write(struct file *file,
	const char __user *buffer, size_t count, loff_t *data)
{
	char desc[32];
	int len = 0;
	unsigned int power = 0;

	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
		return 0;

	desc[len] = '\0';

	if (kstrtouint(desc, 0, &power) == 0)
		mt_gpufreq_thermal_protect(power);
	else
		gpufreq_pr_warn("bad argument!! please provide the maximum limited power\n");

	return count;
}

/* Show current limited power by PBM */
#ifndef DISABLE_PBM_FEATURE
static int mt_gpufreq_limited_by_pbm_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "pbm_limited_power = %d, limit index = %d\n",
		mt_pbm_limited_gpu_power, mt_pbm_limited_index);
	return 0;
}

/* Limited power for thermal protect */
static ssize_t mt_gpufreq_limited_by_pbm_proc_write(struct file *file,
	const char __user *buffer, size_t count, loff_t *data)
{
	char desc[32];
	int len = 0;
	unsigned int power = 0;

	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
		return 0;

	desc[len] = '\0';

	if (kstrtouint(desc, 0, &power) == 0)
		mt_gpufreq_set_power_limit_by_pbm(power);
	else
		gpufreq_pr_warn("bad argument!! please provide the maximum limited power\n");

	return count;
}
#endif

/* Show current GPU DVFS stauts */
static int mt_gpufreq_state_proc_show(struct seq_file *m, void *v)
{
	if (!mt_gpufreq_pause)
		seq_puts(m, "GPU DVFS enabled\n");
	else
		seq_puts(m, "GPU DVFS disabled\n");

	return 0;
}

/* Set GPU DVFS stauts by sysfs interface */
static ssize_t mt_gpufreq_state_proc_write(struct file *file,
	const char __user *buffer, size_t count, loff_t *data)
{
	char desc[32];
	int len = 0;
	int enabled = 0;

	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
		return 0;

	desc[len] = '\0';

	if (kstrtoint(desc, 0, &enabled) == 0) {
		if (enabled == 1) {
			mt_keep_max_frequency_state = false;
			mt_gpufreq_state_set(1);
		} else if (enabled == 0) {
			/* Keep MAX frequency when GPU DVFS disabled. */
			mt_keep_max_frequency_state = true;
			mt_gpufreq_voltage_enable_set(1);
			mt_gpufreq_target(g_gpufreq_max_id);
			mt_gpufreq_state_set(0);
		} else
			gpufreq_pr_warn("bad argument!! argument should be \"1\" or \"0\"\n");
	} else
		gpufreq_pr_warn("bad argument!! argument should be \"1\" or \"0\"\n");

	return count;
}

/* Show GPU OPP table */
static int mt_gpufreq_opp_dump_proc_show(struct seq_file *m, void *v)
{
	int i = 0;

	for (i = 0; i < mt_gpufreqs_num; i++) {
		seq_printf(m, "[%d] ", i);
		seq_printf(m, "freq = %d, ", mt_gpufreqs[i].freq);
		seq_printf(m, "volt = %d, ", mt_gpufreqs[i].volt);
		seq_printf(m, "idx = %d\n", mt_gpufreqs[i].idx);
	}

	return 0;
}

/* Show GPU power table */
static int mt_gpufreq_power_dump_proc_show(struct seq_file *m, void *v)
{
	int i = 0;

	for (i = 0; i < mt_gpufreqs_num; i++) {
		seq_printf(m, "mt_gpufreqs_power[%d].freq = %d\n",
			i, mt_gpufreqs_power[i].freq);
		seq_printf(m, "mt_gpufreqs_power[%d].volt = %d\n",
			i, mt_gpufreqs_power[i].volt);
		seq_printf(m, "mt_gpufreqs_power[%d].power = %d\n",
			i, mt_gpufreqs_power[i].power);
	}

	return 0;
}

/* Show current specific frequency status */
static int mt_gpufreq_opp_freq_proc_show(struct seq_file *m, void *v)
{
	if (mt_keep_opp_frequency_state) {
		seq_puts(m, "gpufreq keep opp frequency enabled\n");
		seq_printf(m, "freq = %d\n",
			mt_gpufreqs[mt_keep_opp_index].freq);
		seq_printf(m, "volt = %d\n",
			mt_gpufreqs[mt_keep_opp_index].volt);
	} else
		seq_puts(m, "gpufreq keep opp frequency disabled\n");
	return 0;
}

/* Enable specific frequency */
static ssize_t mt_gpufreq_opp_freq_proc_write(struct file *file,
	const char __user *buffer, size_t count, loff_t *data)
{
	char desc[32];
	int len = 0;
	int i = 0;
	int fixed_freq = 0;
	int found = 0;

	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
		return 0;

	desc[len] = '\0';

	if (kstrtoint(desc, 0, &fixed_freq) == 0) {
		if (fixed_freq == 0) {
			mt_keep_opp_frequency_state = false;
#ifdef MTK_GPU_SPM
			mtk_gpu_spm_reset_fix();
#endif
		} else {
			for (i = 0; i < mt_gpufreqs_num; i++) {
				if (fixed_freq == mt_gpufreqs[i].freq) {
					mt_keep_opp_index = i;
					found = 1;
					break;
				}
			}

			if (found == 1) {
				mt_keep_opp_frequency_state = true;
				mt_keep_opp_frequency = fixed_freq;

#ifndef MTK_GPU_SPM
				mt_gpufreq_voltage_enable_set(1);
				mt_gpufreq_target(mt_keep_opp_index);
#else
				mtk_gpu_spm_fix_by_idx(mt_keep_opp_index);
#endif
			}

		}
	} else
		gpufreq_pr_warn("bad argument!! please provide the fixed frequency\n");

	return count;
}

/* Show current specific frequency status */
static int mt_gpufreq_opp_max_freq_proc_show(struct seq_file *m, void *v)
{
	if (mt_opp_max_frequency_state) {
		seq_puts(m, "gpufreq opp max frequency enabled\n");
		seq_printf(m, "freq = %d\n",
			mt_gpufreqs[mt_opp_max_index].freq);
		seq_printf(m, "volt = %d\n",
			mt_gpufreqs[mt_opp_max_index].volt);
	} else
		seq_puts(m, "gpufreq opp max frequency disabled\n");

	return 0;
}

/* Enable specific frequency */
static ssize_t mt_gpufreq_opp_max_freq_proc_write(struct file *file,
	const char __user *buffer, size_t count, loff_t *data)
{
	char desc[32];
	int len = 0;
	int i = 0;
	int max_freq = 0;
	int found = 0;

	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
		return 0;

	desc[len] = '\0';

	if (kstrtoint(desc, 0, &max_freq) == 0) {
		if (max_freq == 0) {
			mt_opp_max_frequency_state = false;
		} else {
			for (i = 0; i < mt_gpufreqs_num; i++) {
				if (mt_gpufreqs[i].freq <= max_freq) {
					mt_opp_max_index = i;
					found = 1;
					break;
				}
			}

			if (found == 1) {
				mt_opp_max_frequency_state = true;
				mt_opp_max_frequency =
					mt_gpufreqs[mt_opp_max_index].freq;

				mt_gpufreq_voltage_enable_set(1);
				mt_gpufreq_target(mt_opp_max_index);
			}
		}
	} else
		gpufreq_pr_warn("bad argument!! please provide the maximum limited frequency\n");

	return count;
}

/* Show variable dump */
static int mt_gpufreq_var_dump_proc_show(struct seq_file *m, void *v)
{
	int i = 0;

#ifdef MTK_GPU_SPM
	seq_puts(m, "DVFS_GPU SPM is on\n");
#endif

	seq_printf(m, "g_cur_gpu_freq = %d, g_cur_gpu_volt = %d\n",
		mt_gpufreq_get_cur_freq(), mt_gpufreq_get_cur_volt());
	seq_printf(m, "g_cur_gpu_idx = %d, g_cur_gpu_OPPidx = %d\n",
		g_cur_gpu_idx, g_cur_gpu_OPPidx);
	seq_printf(m, "g_limited_max_id = %d\n",
		g_limited_max_id);

	for (i = 0; i < NR_IDX_POWER_LIMITED; i++)
		seq_printf(m, "mt_power_limidx_array[%d] = %d\n",
			i, mt_power_limidx_array[i]);

	seq_printf(m, "_mt_gpufreq_get_cur_freq = %d\n",
		_mt_gpufreq_get_cur_freq());
	seq_printf(m, "mt_volt_enable_state = %d\n", mt_volt_enable_state);
	seq_printf(m, "mt_dvfs_table_type = %d\n", mt_dvfs_table_type);
	seq_printf(m, "mt_ptpod_disable_idx = %d\n", mt_ptpod_disable_idx);

	return 0;
}

/* Show current voltage enable status */
static int mt_gpufreq_volt_enable_proc_show(struct seq_file *m, void *v)
{
	if (mt_volt_enable)
		seq_puts(m, "gpufreq voltage enabled\n");
	else
		seq_puts(m, "gpufreq voltage disabled\n");

	return 0;
}

/* Enable specific frequency */
static ssize_t mt_gpufreq_volt_enable_proc_write(struct file *file,
	const char __user *buffer, size_t count, loff_t *data)
{
	char desc[32];
	int len = 0;
	int enable = 0;

	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
		return 0;

	desc[len] = '\0';

	if (kstrtoint(desc, 0, &enable) == 0) {
		if (enable == 0) {
			mt_gpufreq_voltage_enable_set(0);
			mt_volt_enable = false;
		} else if (enable == 1) {
			mt_gpufreq_voltage_enable_set(1);
			mt_volt_enable = true;
		} else
			gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: disable, 1: enable]\n");
	} else
		gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: disable, 1: enable]\n");

	return count;
}

/* Show current specific frequency status */
static int mt_gpufreq_fixed_freq_volt_proc_show(struct seq_file *m, void *v)
{
	if (mt_fixed_freq_volt_state) {
		seq_puts(m, "gpufreq fixed frequency enabled\n");
		seq_printf(m, "fixed freq = %d\n", mt_fixed_frequency);
		seq_printf(m, "fixed volt = %d\n", mt_fixed_voltage);
	} else
		seq_puts(m, "gpufreq fixed frequency disabled\n");

	return 0;
}

/* Enable specific frequency */
static void _mt_gpufreq_fixed_freq(int fixed_freq)
{
	/* freq (KHz) */
	if ((fixed_freq >= GPUFREQ_LAST_FREQ_LEVEL) &&
		(fixed_freq <= GPU_DVFS_FREQ0)) {
		gpufreq_pr_debug("@ %s, mt_gpufreq_clock_switch1 fix frq = %d, fix volt = %d, volt = %d\n",
			__func__,
			mt_fixed_frequency,
			mt_fixed_voltage,
			g_cur_gpu_volt);

		mt_fixed_freq_volt_state = true;
		mt_fixed_frequency = fixed_freq;
		mt_fixed_voltage = g_cur_gpu_volt;
		mt_gpufreq_voltage_enable_set(1);

		gpufreq_pr_debug("@ %s, mt_gpufreq_clock_switch2 fix frq = %d, fix volt = %d, volt = %d\n",
			__func__,
			mt_fixed_frequency,
			mt_fixed_voltage,
			g_cur_gpu_volt);

		mt_gpufreq_clock_switch(mt_fixed_frequency);
		g_cur_gpu_freq = mt_fixed_frequency;
	}
}

static void _mt_gpufreq_fixed_volt(int fixed_volt)
{
	/* volt (mV) */
#ifdef VGPU_SET_BY_PMIC
	if (fixed_volt >= (PMIC_MIN_VGPU / 100) &&
		fixed_volt <= (PMIC_MAX_VGPU / 100)) {
#endif
		gpufreq_pr_debug("@ %s, mt_gpufreq_volt_switch1 fix frq = %d, fix volt = %d, volt = %d\n",
			__func__,
			mt_fixed_frequency,
			mt_fixed_voltage,
			g_cur_gpu_volt);

		mt_fixed_freq_volt_state = true;
		mt_fixed_frequency = g_cur_gpu_freq;
		mt_fixed_voltage = fixed_volt * 100;
		mt_gpufreq_voltage_enable_set(1);

		gpufreq_pr_debug("@ %s, mt_gpufreq_volt_switch2 fix frq = %d, fix volt = %d, volt = %d\n",
			__func__,
			mt_fixed_frequency,
			mt_fixed_voltage,
			g_cur_gpu_volt);

		mt_gpufreq_volt_switch(g_cur_gpu_volt, mt_fixed_voltage);
		g_cur_gpu_volt = mt_fixed_voltage;
	}
}

static ssize_t mt_gpufreq_fixed_freq_volt_proc_write(struct file *file,
	const char __user *buffer, size_t count, loff_t *data)
{
	char desc[32];
	int len = 0;
	int fixed_freq = 0;
	int fixed_volt = 0;

	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
		return 0;

	desc[len] = '\0';

	if (sscanf(desc, "%d %d", &fixed_freq, &fixed_volt) == 2) {
		if ((fixed_freq == 0) && (fixed_volt == 0)) {
			mt_fixed_freq_volt_state = false;
			mt_fixed_frequency = 0;
			mt_fixed_voltage = 0;
#ifdef MTK_GPU_SPM
			mtk_gpu_spm_reset_fix();
#endif
		} else {
			g_cur_gpu_freq = _mt_gpufreq_get_cur_freq();
#ifndef MTK_GPU_SPM
			if (fixed_freq > g_cur_gpu_freq) {
				_mt_gpufreq_fixed_volt(fixed_volt);
				_mt_gpufreq_fixed_freq(fixed_freq);
			} else {
				_mt_gpufreq_fixed_freq(fixed_freq);
				_mt_gpufreq_fixed_volt(fixed_volt);
			}
#else
			if (0) {
				_mt_gpufreq_fixed_volt(fixed_volt);
				_mt_gpufreq_fixed_freq(fixed_freq);
			}

			{
				int i, found;

				for (i = 0; i < mt_gpufreqs_num; i++) {
					if (fixed_freq == mt_gpufreqs[i].freq) {
						mt_keep_opp_index = i;
						found = 1;
						break;
					}
				}
			}

			mt_fixed_frequency = fixed_freq;
			mt_fixed_voltage = fixed_volt * 100;
			mtk_gpu_spm_fix_by_idx(mt_keep_opp_index);
#endif
		}
	} else
		gpufreq_pr_warn("bad argument! should be [enable fixed_freq fixed_volt]\n");

	return count;
}

/* Show lowpower frequency opp enable status */
static int mt_gpufreq_lpt_enable_proc_show(struct seq_file *m, void *v)
{
	seq_puts(m, "not implemented\n");
	return 0;
}

/* Enable lowpower frequency opp */
static ssize_t mt_gpufreq_lpt_enable_proc_write(struct file *file,
	const char __user *buffer, size_t count, loff_t *data)
{
	gpufreq_pr_warn("not implemented\n");

#if 0
	char desc[32];
	int len = 0;
	int enable = 0;

	len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
	if (copy_from_user(desc, buffer, len))
		return 0;

	desc[len] = '\0';

	if (kstrtoint(desc, 0, &enable) == 0) {
		if (enable == 0)
			mt_gpufreq_low_power_test_enable = false;
		else if (enable == 1)
			mt_gpufreq_low_power_test_enable = true;
		else
			gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: disable, 1: enable]\n");
	} else
		gpufreq_pr_warn("bad argument!! should be 0 or 1 [0: disable, 1: enable]\n");
#endif

	return count;
}

#define PROC_FOPS_RW(name) \
	static int mt_ ## name ## _proc_open( \
		struct inode *inode, struct file *file) \
{ \
	return single_open(file, mt_ ## name ## _proc_show, PDE_DATA(inode)); \
} \
static const struct file_operations mt_ ## name ## _proc_fops = { \
	.owner		= THIS_MODULE, \
	.open		= mt_ ## name ## _proc_open, \
	.read		= seq_read, \
	.llseek		= seq_lseek, \
	.release	= single_release, \
	.write		= mt_ ## name ## _proc_write, \
}

#define PROC_FOPS_RO(name) \
	static int mt_ ## name ## _proc_open( \
		struct inode *inode, struct file *file) \
{ \
	return single_open(file, mt_ ## name ## _proc_show, PDE_DATA(inode)); \
} \
static const struct file_operations mt_ ## name ## _proc_fops = { \
	.owner		= THIS_MODULE, \
	.open		= mt_ ## name ## _proc_open, \
	.read		= seq_read, \
	.llseek		= seq_lseek, \
	.release	= single_release, \
}

#define PROC_ENTRY(name) {__stringify(name), &mt_ ## name ## _proc_fops}

PROC_FOPS_RW(gpufreq_debug);
PROC_FOPS_RW(gpufreq_limited_power);

#ifdef MT_GPUFREQ_OC_PROTECT
PROC_FOPS_RW(gpufreq_limited_oc_ignore);
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT
PROC_FOPS_RW(gpufreq_limited_low_batt_volume_ignore);
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
PROC_FOPS_RW(gpufreq_limited_low_batt_volt_ignore);
#endif

PROC_FOPS_RW(gpufreq_limited_thermal_ignore);

#ifndef DISABLE_PBM_FEATURE
PROC_FOPS_RW(gpufreq_limited_pbm_ignore);
PROC_FOPS_RW(gpufreq_limited_by_pbm);
#endif

PROC_FOPS_RW(gpufreq_state);
PROC_FOPS_RO(gpufreq_opp_dump);
PROC_FOPS_RO(gpufreq_power_dump);
PROC_FOPS_RW(gpufreq_opp_freq);
PROC_FOPS_RW(gpufreq_opp_max_freq);
PROC_FOPS_RO(gpufreq_var_dump);
PROC_FOPS_RW(gpufreq_volt_enable);
PROC_FOPS_RW(gpufreq_fixed_freq_volt);
PROC_FOPS_RW(gpufreq_lpt_enable);

static int mt_gpufreq_create_procfs(void)
{
	struct proc_dir_entry *dir = NULL;
	int i;

	struct pentry {
		const char *name;
		const struct file_operations *fops;
	};

	const struct pentry entries[] = {
		PROC_ENTRY(gpufreq_debug),
		PROC_ENTRY(gpufreq_limited_power),
#ifdef MT_GPUFREQ_OC_PROTECT
		PROC_ENTRY(gpufreq_limited_oc_ignore),
#endif
#ifdef MT_GPUFREQ_LOW_BATT_VOLUME_PROTECT
		PROC_ENTRY(gpufreq_limited_low_batt_volume_ignore),
#endif
#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
		PROC_ENTRY(gpufreq_limited_low_batt_volt_ignore),
#endif
		PROC_ENTRY(gpufreq_limited_thermal_ignore),
#ifndef DISABLE_PBM_FEATURE
		PROC_ENTRY(gpufreq_limited_pbm_ignore),
		PROC_ENTRY(gpufreq_limited_by_pbm),
#endif
		PROC_ENTRY(gpufreq_state),
		PROC_ENTRY(gpufreq_opp_dump),
		PROC_ENTRY(gpufreq_power_dump),
		PROC_ENTRY(gpufreq_opp_freq),
		PROC_ENTRY(gpufreq_opp_max_freq),
		PROC_ENTRY(gpufreq_var_dump),
		PROC_ENTRY(gpufreq_volt_enable),
		PROC_ENTRY(gpufreq_fixed_freq_volt),
		PROC_ENTRY(gpufreq_lpt_enable),
	};

	dir = proc_mkdir("gpufreq", NULL);
	if (!dir) {
		gpufreq_pr_err("fail to create /proc/gpufreq\n");
		return -ENOMEM;
	}

	for (i = 0; i < ARRAY_SIZE(entries); i++) {
		/* 0664: S_IRUGO | S_IWUSR | S_IWGRP */
		if (!proc_create(entries[i].name, 0664, dir, entries[i].fops))
			gpufreq_pr_err("create /proc/gpufreq/%s failed\n",
				entries[i].name);
	}

	return 0;
}
#endif /* CONFIG_PROC_FS */

/* Mediatek gpufreq initialization */
static int __init mt_gpufreq_init(void)
{
	int ret = 0;

#ifdef BRING_UP
	/* Skip driver init in bring up stage */
	return 0;
#endif

	gpufreq_pr_debug("@%s\n", __func__);

#ifdef CONFIG_PROC_FS
	if (mt_gpufreq_create_procfs())
		goto out;
#endif

	/* register platform device/driver */
#ifndef CONFIG_OF
	ret = platform_device_register(&mt_gpufreq_pdev);
	if (ret) {
		gpufreq_pr_err("fail to register gpufreq device\n");
		goto out;
	}
#endif

	ret = platform_driver_register(&mt_gpufreq_pdrv);
	if (ret) {
		gpufreq_pr_err("fail to register gpufreq driver\n");

#ifndef CONFIG_OF
		platform_device_unregister(&mt_gpufreq_pdev);
#endif
	}

out:
	return ret;
}

static void __exit mt_gpufreq_exit(void)
{
	platform_driver_unregister(&mt_gpufreq_pdrv);

#ifndef CONFIG_OF
	platform_device_unregister(&mt_gpufreq_pdev);
#endif
}

module_init(mt_gpufreq_init);
module_exit(mt_gpufreq_exit);

MODULE_DESCRIPTION("MediaTek GPU Frequency Scaling driver");
MODULE_LICENSE("GPL");
