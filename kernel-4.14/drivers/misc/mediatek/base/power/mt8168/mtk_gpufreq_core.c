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

/**
 * @file	mtk_gpufreq_core
 * @brief   Driver for GPU-DVFS
 */
/**
 * ===============================================
 * SECTION : Include files
 * ===============================================
 */
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/uaccess.h>
#include <linux/pm_qos.h>
#include <linux/random.h>
#include <linux/pm_runtime.h>
#include <linux/clk-provider.h>

#include "mtk_gpufreq.h"
#include "mtk_gpufreq_core.h"
#include "mtk_devinfo.h"
#include "upmu_common.h"
#include "upmu_sw.h"
#include "upmu_hw.h"

#ifdef CONFIG_MTK_QOS_SUPPORT
#include "helio-dvfsrc-opp-mt8168.h"
#endif
#ifdef CONFIG_THERMAL
#include "mtk_thermal.h"
#endif
#ifdef MT_GPUFREQ_STATIC_PWR_READY2USE
#include "mtk_static_power.h"
#include "mtk_static_power_mt8168.h"
#endif
#ifdef MTK_GPU_LOG
#include "mtk_gpu_log.h"
#endif
/**
 * ===============================================
 * SECTION : Local functions declaration
 * ===============================================
 */
static int __mt_gpufreq_pdrv_probe(struct platform_device *pdev);
static int __mt_gpufreq_pdrv_remove(struct platform_device *pdev);

static void __mt_gpufreq_set(unsigned int idx_old, unsigned int idx_new,
			     unsigned int freq_old, unsigned int freq_new,
			     unsigned int volt_old, unsigned int volt_new,
			     unsigned int vsram_volt_old,
			     unsigned int vsram_volt_new);
static void __mt_gpufreq_set_fixed_volt(int fixed_volt);
static void __mt_gpufreq_set_fixed_freq(int fixed_freq);
static void __mt_gpufreq_bucks_enable(void);
static void __mt_gpufreq_bucks_disable(void);
static void __mt_gpufreq_vgpu_set_mode(unsigned int mode);
static unsigned int __mt_gpufreq_get_cur_volt(void);
static unsigned int __mt_gpufreq_get_cur_freq(void);
static unsigned int __mt_gpufreq_get_cur_vsram_volt(void);
static int __mt_gpufreq_get_opp_idx_by_volt(unsigned int volt);
static unsigned int __mt_gpufreq_get_vsram_by_target_volt(unsigned int volt);
static unsigned int __mt_gpufreq_get_limited_freq_by_power(unsigned int
							   limited_power);
static enum g_post_divider_power_enum
__mt_gpufreq_get_post_divider_power(unsigned int freq, unsigned int efuse);
static void __mt_gpufreq_switch_to_clksrc(enum g_clock_source_enum clksrc);
static void __mt_gpufreq_kick_pbm(int enable);
static void __mt_gpufreq_clock_switch(unsigned int freq_new);
static void __mt_gpufreq_volt_switch(unsigned int volt_old,
				     unsigned int volt_new,
				     unsigned int vsram_volt_old,
				     unsigned int vsram_volt_new);
static void __mt_gpufreq_volt_switch_without_vsram_volt(unsigned int volt_old,
							unsigned int volt_new);

#ifdef MT_GPUFREQ_BATT_OC_PROTECT
static void __mt_gpufreq_batt_oc_protect(unsigned int limited_idx);
#endif

#ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT
static void __mt_gpufreq_batt_percent_protect(unsigned int limited_index);
#endif

#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
static void __mt_gpufreq_low_batt_protect(unsigned int limited_index);
#endif

#ifdef MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE
static void __mt_update_gpufreqs_power_table(void);
#endif

static void __mt_gpufreq_update_max_limited_idx(void);
static unsigned int __mt_gpufreq_calculate_dds(unsigned int freq_khz,
					       enum g_post_divider_power_enum
					       post_divider_power);
static void __mt_gpufreq_setup_opp_power_table(int num);
static void __mt_gpufreq_cal_sb_opp_index(void);
static void __mt_gpufreq_vsram_gpu_volt_switch(enum g_volt_switch_enum
					       switch_way,
					       unsigned int sfchg_rate,
					       unsigned int volt_old,
					       unsigned int volt_new);
static void __mt_gpufreq_vgpu_volt_switch(enum g_volt_switch_enum switch_way,
					  unsigned int sfchg_rate,
					  unsigned int volt_old,
					  unsigned int volt_new);

/**
 * ===============================================
 * SECTION : Local variables definition
 * ===============================================
 */

static struct mt_gpufreq_power_table_info *g_power_table;
static struct g_opp_table_info *g_opp_table;
static struct g_opp_table_info *g_opp_table_default;
static struct g_pmic_info *g_pmic;
static struct g_clk_info *g_clk;
static struct platform_device *g_gpufreq_pdev_p;


static unsigned int g_ptpod_opp_idx_num;
static unsigned int *g_ptpod_opp_idx_table;
static unsigned int g_ptpod_opp_idx_table_segment[] = {
	0, 2
};

static struct g_opp_table_info g_opp_table_segment[] = {
	GPUOP(SEG_GPU_DVFS_FREQ0, SEG_GPU_DVFS_VOLT0, SEG_GPU_DVFS_VSRAM0),
	GPUOP(SEG_GPU_DVFS_FREQ1, SEG_GPU_DVFS_VOLT1, SEG_GPU_DVFS_VSRAM0),
	GPUOP(SEG_GPU_DVFS_FREQ2, SEG_GPU_DVFS_VOLT2, SEG_GPU_DVFS_VSRAM0),
};

static const struct of_device_id g_gpufreq_of_match[] = {
	{.compatible = "mediatek,mt8168-gpufreq"},
	{ /* sentinel */ }
};

static struct platform_driver g_gpufreq_pdrv = {
	.probe = __mt_gpufreq_pdrv_probe,
	.remove = __mt_gpufreq_pdrv_remove,
	.driver = {
		   .name = "gpufreq",
		   .owner = THIS_MODULE,
		   .of_match_table = g_gpufreq_of_match,
		   },
};

static bool g_parking;
static bool g_DVFS_is_paused_by_ptpod;
static bool g_volt_enable_state;
static bool g_keep_opp_freq_state;
static bool g_dvfs_enable;
static bool g_opp_stress_test_state;
static bool g_fixed_freq_volt_state;
static bool g_pbm_limited_ignore_state;
static bool g_thermal_ptc_limit_ignr_state;
static unsigned int g_efuse_id;
static unsigned int g_segment_id;
static unsigned int g_max_opp_idx_num;
static unsigned int g_segment_max_opp_idx;
static unsigned int g_enable_aging_test;
static unsigned int g_cur_opp_freq;
static unsigned int g_cur_opp_volt;
static unsigned int g_cur_opp_vsram_volt;
static unsigned int g_cur_opp_idx;
static unsigned int g_keep_opp_freq;
static unsigned int g_keep_opp_freq_idx;
static unsigned int g_fixed_vsram_volt;
static unsigned int g_fixed_vsram_volt_threshold;
static unsigned int g_fixed_freq;
static unsigned int g_fixed_volt;
static unsigned int g_max_limited_idx;
static unsigned int g_pbm_limited_power;
static unsigned int g_thermal_protect_power;
static unsigned int g_vgpu_sfchg_rrate;
static unsigned int g_vgpu_sfchg_frate;
static unsigned int g_vsram_sfchg_rrate;
static unsigned int g_vsram_sfchg_frate;
static unsigned int g_DVFS_off_by_ptpod_idx;
static int g_opp_sb_idx_up[NUM_OF_OPP_IDX] = { 0 };
static int g_opp_sb_idx_down[NUM_OF_OPP_IDX] = { 0 };

#ifdef MT_GPUFREQ_BATT_OC_PROTECT
static bool g_batt_oc_limited_ignore_state;
static unsigned int g_batt_oc_level;
static unsigned int g_batt_oc_limited_idx;
static unsigned int g_batt_oc_limited_idx_lvl_0;
static unsigned int g_batt_oc_limited_idx_lvl_1;
#endif				/* ifdef MT_GPUFREQ_BATT_OC_PROTECT */
#ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT
static bool g_batt_percent_limit_ignr_state;
static unsigned int g_batt_percent_level;
static unsigned int g_batt_percent_limited_idx;
static unsigned int g_batt_percent_limited_idx_lvl_0;
static unsigned int g_batt_percent_limited_idx_lvl_1;
#endif				/* ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT */
#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
static bool g_low_batt_limited_ignore_state;
static unsigned int g_low_battery_level;
static unsigned int g_low_batt_limited_idx;
static unsigned int g_low_batt_limited_idx_lvl_0;
static unsigned int g_low_batt_limited_idx_lvl_2;
#endif				/* ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT */
static enum g_post_divider_power_enum g_cur_post_divider_power;
static DEFINE_MUTEX(mt_gpufreq_lock);
static DEFINE_MUTEX(mt_gpufreq_power_lock);
static unsigned int g_limited_idx_array[NUMBER_OF_LIMITED_IDX] = { 0 };
static bool g_limited_ignore_array[NUMBER_OF_LIMITED_IDX] = { false };

static void __iomem *g_apmixed_base;

#ifdef MTK_GPU_LOG
GED_LOG_BUF_HANDLE _mtk_gpu_log_hnd;
#endif

static int g_clock_on;

module_param(g_enable_aging_test, uint, 0644);

/**
 * ===============================================
 * SECTION : API definition
 * ===============================================
 */

/*
 * API : handle frequency change request
 */
unsigned int mt_gpufreq_target(unsigned int request_idx, bool is_real_idx)
{
	int i;
	unsigned int target_freq;
	unsigned int target_volt;
	unsigned int target_vsram;
	unsigned int target_idx;
	int need_skip;

	need_skip = 0;
	mutex_lock(&mt_gpufreq_lock);

	if (!g_dvfs_enable) {
		gpufreq_pr_debug("@%s: gpu dvfs is disable (skipped)\n",
				 __func__);
		mutex_unlock(&mt_gpufreq_lock);
		return 0;
	}

	request_idx += g_segment_max_opp_idx;

	if (request_idx > (g_max_opp_idx_num - 1)) {
		gpufreq_pr_debug("@%s: OPP index (%d) is out of range\n",
				 __func__, request_idx);
		mutex_unlock(&mt_gpufreq_lock);
		return -1;
	}

	/* look up for the target OPP table */
	target_idx = request_idx;
	target_freq = g_opp_table[target_idx].gpufreq_khz;
	target_volt = g_opp_table[target_idx].gpufreq_volt;
	target_vsram = g_opp_table[target_idx].gpufreq_vsram;

	gpufreq_pr_debug
	    ("@%s: receive request index: %d, freq: %d, volt: %d, vsram: %d\n",
	     __func__, target_idx, target_freq, target_volt, target_vsram);

	/* generate random opp idx for stress test */
	if (g_opp_stress_test_state) {
		get_random_bytes(&target_idx, sizeof(target_idx));
		target_idx = target_idx % g_max_opp_idx_num;

		if (target_idx < g_segment_max_opp_idx)
			target_idx = g_segment_max_opp_idx;
		gpufreq_pr_debug("@%s: OPP stress test index: %d\n",
				 __func__, target_idx);
	}

	/* OPP freq is limited by Thermal/Power/PBM */
	if (g_max_limited_idx != g_max_opp_idx_num) {
		if (target_freq > g_opp_table[g_max_limited_idx].gpufreq_khz) {
			gpufreq_pr_debug("@%s: tgt_idx:%d limit to: %d Therm/Power/PBM\n",
				__func__, target_idx, g_max_limited_idx);
			target_idx = g_max_limited_idx;
		}
	}

	/* If /proc/gpufreq/gpufreq_opp_freq fix OPP freq */
	if (g_keep_opp_freq_state) {
		gpufreq_pr_debug("@%s: taget_idx: %d limited to keep_opp_idx: %d\n",
			__func__, target_idx, g_keep_opp_freq_idx);
		target_idx = g_keep_opp_freq_idx;
	}

	/* keep at max freq when PTPOD is initializing */
	if (g_DVFS_is_paused_by_ptpod) {
		gpufreq_pr_debug("@%s: target_idx: %d limited to by_ptpod_idx: %d\n",
			__func__, target_idx, g_keep_opp_freq_idx);
		target_idx = g_DVFS_off_by_ptpod_idx;
	}

	/* If /proc/gpufreq/gpufreq_fixed_freq_volt fix freq and volt */
	if (g_fixed_freq_volt_state) {
		target_freq = g_fixed_freq;
		target_volt = g_fixed_volt;
		target_vsram =
		    __mt_gpufreq_get_vsram_by_target_volt(target_volt);

		target_idx = g_segment_max_opp_idx;
		gpufreq_pr_debug("@%s: fixed freq = %d, volt = %d, vsram = %d\n",
			__func__,
			target_freq, target_volt, target_vsram);
	} else {
		target_freq = g_opp_table[target_idx].gpufreq_khz;
		target_volt = g_opp_table[target_idx].gpufreq_volt;
		target_vsram = g_opp_table[target_idx].gpufreq_vsram;
	}
	g_cur_opp_freq = __mt_gpufreq_get_cur_freq();
	if (g_cur_opp_freq > target_freq
		&& ((g_cur_opp_freq - target_freq) < 50))
		need_skip = 1;
	else if (g_cur_opp_freq > target_freq
		&& ((g_cur_opp_freq - target_freq) >= 50))
		need_skip = 0;
	else if (g_cur_opp_freq < target_freq
		&& ((target_freq - g_cur_opp_freq) >= 50))
		need_skip = 0;
	else if (g_cur_opp_freq < target_freq
		&& ((target_freq - g_cur_opp_freq) < 50))
		need_skip = 1;

	/* target freq == current freq && target volt == current volt, skip */
	if ((need_skip == 1) && g_cur_opp_volt == target_volt) {
		gpufreq_pr_debug("@%s: Freq: %d ---> %d (skipped)\n", __func__,
				 g_cur_opp_freq, target_freq);
		mutex_unlock(&mt_gpufreq_lock);
		return 0;
	}

	/* set to the target frequency and voltage */
	if (g_enable_aging_test) {
		if (target_volt <= 75000)
			target_volt -= 1250;
		else
			target_volt -= 1875;
	}

	/* set to the target frequency and voltage */
	__mt_gpufreq_set(g_cur_opp_idx, target_idx,
			 g_cur_opp_freq, target_freq,
			 g_cur_opp_volt, target_volt,
			 g_cur_opp_vsram_volt, target_vsram);

#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_oppidx(target_idx);
#endif

	g_cur_opp_idx = target_idx;

	mutex_unlock(&mt_gpufreq_lock);

	return 0;
}

/*
 * enable Clock Gating
 */
void mt_gpufreq_enable_CG(void)
{
	mutex_lock(&mt_gpufreq_lock);

#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_status(0xB0 |
				   (aee_rr_curr_gpu_dvfs_status() & 0x0F));
#endif

	if (clk_prepare_enable(g_clk->subsys_mfg_cg))
		gpufreq_pr_err("@%s: failed when enable subsys-mfg-cg\n",
			       __func__);
	if (clk_prepare_enable(g_clk->subsys_mfg_mbist_diag))
		gpufreq_pr_err("@%s: failed when enable subsys_mfg_mbist_diag\n",
			       __func__);

#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_status(0xC0 |
				   (aee_rr_curr_gpu_dvfs_status() & 0x0F));
#endif

	gpufreq_pr_debug("@%s: enable CG done, mfg_cg_count[%d]\n",
		__func__,
		__clk_get_enable_count(g_clk->subsys_mfg_cg));

	g_clock_on = 1;

	mutex_unlock(&mt_gpufreq_lock);

}

/*
 * disable Clock Gating
 */
void mt_gpufreq_disable_CG(void)
{
	mutex_lock(&mt_gpufreq_lock);

	g_clock_on = 0;

#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_status(0xD0 |
				   (aee_rr_curr_gpu_dvfs_status() & 0x0F));
#endif

	clk_disable_unprepare(g_clk->subsys_mfg_mbist_diag);
	clk_disable_unprepare(g_clk->subsys_mfg_cg);

#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_status(0xE0 |
				   (aee_rr_curr_gpu_dvfs_status() & 0x0F));
#endif

	gpufreq_pr_debug("@%s: disable CG done mfg_cg_count[%d]\n",
		__func__,
		__clk_get_enable_count(g_clk->subsys_mfg_cg));

	mutex_unlock(&mt_gpufreq_lock);

}

/*
 * enable MTCMOS
 */
void mt_gpufreq_enable_MTCMOS(void)
{
	mutex_lock(&mt_gpufreq_lock);

#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_status(0x70 |
				   (aee_rr_curr_gpu_dvfs_status() & 0x0F));
#endif
	/* Resume mfg power domain */
	if (pm_runtime_get_sync(&g_gpufreq_pdev_p->dev))
		gpufreq_pr_err("@%s: failed when enable mfg mtmos\n",
			       __func__);

#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_status(0x80 |
				   (aee_rr_curr_gpu_dvfs_status() & 0x0F));
#endif
	gpufreq_pr_debug("@%s: enable MTCMOS done\n", __func__);

	mutex_unlock(&mt_gpufreq_lock);
}

/*
 * disable MTCMOS
 */
void mt_gpufreq_disable_MTCMOS(void)
{
	mutex_lock(&mt_gpufreq_lock);

#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_status(0x90 |
				   (aee_rr_curr_gpu_dvfs_status() & 0x0F));
#endif

	pm_runtime_put_sync(&g_gpufreq_pdev_p->dev);


#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_status(0xA0 |
				   (aee_rr_curr_gpu_dvfs_status() & 0x0F));
#endif
	gpufreq_pr_debug("@%s: disable MTCMOS done\n", __func__);

	mutex_unlock(&mt_gpufreq_lock);
}

/*
 * API : GPU voltage on/off setting
 * 0 : off
 * 1 : on
 */
unsigned int mt_gpufreq_voltage_enable_set(unsigned int enable)
{
	mutex_lock(&mt_gpufreq_lock);

	if (g_DVFS_is_paused_by_ptpod && enable == 0) {
		gpufreq_pr_info("@%s: DVFS is paused by PTPOD\n", __func__);
		mutex_unlock(&mt_gpufreq_lock);
		return -1;
	}

	if (enable == 1) {
		/* Turn on GPU Bucks */
		__mt_gpufreq_bucks_enable();
		g_volt_enable_state = true;
		__mt_gpufreq_kick_pbm(1);
		gpufreq_pr_debug("@%s: VGPU/VSRAM_GPU is on\n", __func__);
	} else if (enable == 0) {
		/* Turn off GPU Bucks */
		__mt_gpufreq_bucks_disable();
		g_volt_enable_state = false;
		__mt_gpufreq_kick_pbm(0);
		gpufreq_pr_debug("@%s: VGPU/VSRAM_GPU is off\n", __func__);
	}

	gpufreq_pr_debug("@%s: enable = %d, g_volt_enable_state = %d\n",
			 __func__, enable, g_volt_enable_state);

	mutex_unlock(&mt_gpufreq_lock);

	return 0;
}

/*
 * API : enable DVFS for PTPOD initializing
 */
void mt_gpufreq_enable_by_ptpod(void)
{
#ifdef USE_STAND_ALONE_VGPU
	/* Set GPU Buck to leave PWM mode */
	__mt_gpufreq_vgpu_set_mode(REGULATOR_MODE_NORMAL);
#endif

	/* Freerun GPU DVFS */
	g_DVFS_is_paused_by_ptpod = false;

	/* Turn off GPU MTCMOS */
	mt_gpufreq_disable_MTCMOS();

#if 0
	pr_info("[GPU DVFS] flavor name: %s\n",
		CONFIG_BUILD_ARM64_DTB_OVERLAY_IMAGE_NAMES);
	if ((strstr(CONFIG_BUILD_ARM64_DTB_OVERLAY_IMAGE_NAMES,
		    "k68v1_64_aging") != NULL)) {
		pr_info("[GPU_DVFS]: AGING flavor !!!\n");
		g_enable_aging_test = 1;
	}
#endif

	gpufreq_pr_debug("@%s: DVFS is enabled by ptpod\n", __func__);
}

/*
 * API : disable DVFS for PTPOD initializing
 */
void mt_gpufreq_disable_by_ptpod(void)
{
	int i = 0;
	int target_idx = g_segment_max_opp_idx;

	/* Turn on GPU PMIC Buck */
	mt_gpufreq_voltage_enable_set(1);

	/* Turn on GPU MTCMOS */
	mt_gpufreq_enable_MTCMOS();

	/* Pause GPU DVFS */
	g_DVFS_is_paused_by_ptpod = true;

	/* Fix GPU @ 0.8V */
	for (i = 0; i < g_max_opp_idx_num; i++) {
		if (g_opp_table_default[i].gpufreq_volt <=
		    GPU_DVFS_PTPOD_DISABLE_VOLT) {
			target_idx = i;
			break;
		}
	}
	g_DVFS_off_by_ptpod_idx = (unsigned int)target_idx;
	mt_gpufreq_target(target_idx, true);

#ifdef USE_STAND_ALONE_VGPU
	/* Set GPU Buck to enter PWM mode */
	__mt_gpufreq_vgpu_set_mode(REGULATOR_MODE_FAST);
#endif

	gpufreq_pr_debug("@%s: DVFS is disabled by ptpod\n", __func__);
}

/*
 * API : update OPP and switch back to default voltage setting
 */
void mt_gpufreq_restore_default_volt(void)
{
	int i;

	mutex_lock(&mt_gpufreq_lock);

	gpufreq_pr_debug("@%s: restore OPP table to default voltage\n",
			 __func__);

	for (i = 0; i < g_max_opp_idx_num; i++) {
		g_opp_table[i].gpufreq_volt =
		    g_opp_table_default[i].gpufreq_volt;
		gpufreq_pr_debug("@%s: g_opp_table[%d].gpufreq_volt = %d\n",
				 __func__, i, g_opp_table[i].gpufreq_volt);
	}

	__mt_gpufreq_cal_sb_opp_index();

	__mt_gpufreq_volt_switch_without_vsram_volt(g_cur_opp_volt,
			g_opp_table[g_cur_opp_idx].gpufreq_volt);

	g_cur_opp_volt = g_opp_table[g_cur_opp_idx].gpufreq_volt;
	g_cur_opp_vsram_volt = g_opp_table[g_cur_opp_idx].gpufreq_vsram;

	mutex_unlock(&mt_gpufreq_lock);
}

/*
 * API : update OPP and set voltage because PTPOD
 * modified voltage table by PMIC wrapper
 */
unsigned int mt_gpufreq_update_volt(unsigned int pmic_volt[],
				    unsigned int array_size)
{
	int i;
	int target_idx;

	mutex_lock(&mt_gpufreq_lock);

	gpufreq_pr_debug("@%s: update OPP table to given voltage\n", __func__);

	for (i = 0; i < array_size; i++) {
		target_idx = mt_gpufreq_get_ori_opp_idx(i);
		g_opp_table[target_idx].gpufreq_volt = pmic_volt[i];
		g_opp_table[target_idx].gpufreq_vsram =
		    __mt_gpufreq_get_vsram_by_target_volt(pmic_volt[i]);

		if (i < array_size - 1) {
			/* interpolation for opps not for ptpod */
			int larger = pmic_volt[i];
			int smaller = pmic_volt[i + 1];
			int interpolation;

			if (target_idx == 20) {
				/* After opp 20, 2 opps need intepolation */
				interpolation = ((larger << 1) + smaller) / 3;
				g_opp_table[target_idx + 1].gpufreq_volt
				    = VOLT_NORMALIZATION(interpolation);
				g_opp_table[target_idx + 1].gpufreq_vsram
				    = __mt_gpufreq_get_vsram_by_target_volt
				    (g_opp_table[target_idx + 1].gpufreq_volt);

				interpolation = (larger + (smaller << 1)) / 3;
				g_opp_table[target_idx + 2].gpufreq_volt
				    = VOLT_NORMALIZATION(interpolation);
				g_opp_table[target_idx + 2].gpufreq_vsram
				    = __mt_gpufreq_get_vsram_by_target_volt
				    (g_opp_table[target_idx + 2].gpufreq_volt);
			} else {
				interpolation = (larger + smaller) >> 1;
				g_opp_table[target_idx + 1].gpufreq_volt
				    = VOLT_NORMALIZATION(interpolation);
				g_opp_table[target_idx + 1].gpufreq_vsram
				    = __mt_gpufreq_get_vsram_by_target_volt
				    (g_opp_table[target_idx + 1].gpufreq_volt);
			}
		}

		gpufreq_pr_debug("@%s: g_opp_table[%d].gpufreq_volt = %d\n",
				 __func__, i, g_opp_table[i].gpufreq_volt);
	}

	__mt_gpufreq_cal_sb_opp_index();

	/* update volt if powered */
	if (g_volt_enable_state) {
		__mt_gpufreq_volt_switch_without_vsram_volt(g_cur_opp_volt,
							    g_opp_table
							    [g_cur_opp_idx].
							    gpufreq_volt);
	}

	g_cur_opp_volt = g_opp_table[g_cur_opp_idx].gpufreq_volt;
	g_cur_opp_vsram_volt = g_opp_table[g_cur_opp_idx].gpufreq_vsram;

	mutex_unlock(&mt_gpufreq_lock);

	return 0;
}

/* API : get OPP table index number */
unsigned int mt_gpufreq_get_dvfs_table_num(void)
{
	return g_max_opp_idx_num - g_segment_max_opp_idx;
}

/* API : get frequency via OPP table index */
unsigned int mt_gpufreq_get_freq_by_idx(unsigned int idx)
{
	idx += g_segment_max_opp_idx;
	if (idx < g_max_opp_idx_num)
		return g_opp_table[idx].gpufreq_khz;
	else
		return 0;
}

/* API : get voltage via OPP table index */
unsigned int mt_gpufreq_get_volt_by_idx(unsigned int idx)
{
	idx += g_segment_max_opp_idx;
	if (idx < g_max_opp_idx_num)
		return g_opp_table[idx].gpufreq_volt;
	else
		return 0;
}

/* API: get opp idx in original opp tables. */
/* This is usually for ptpod use. */
unsigned int mt_gpufreq_get_ori_opp_idx(unsigned int idx)
{
	if (idx < g_ptpod_opp_idx_num && idx >= 0)
		return g_ptpod_opp_idx_table[idx];
	else
		return idx;

}

/* API : get max power on power table */
unsigned int mt_gpufreq_get_max_power(void)
{
	return (!g_power_table) ? 0
	    : g_power_table[g_segment_max_opp_idx].gpufreq_power;
}

/* API : get min power on power table */
unsigned int mt_gpufreq_get_min_power(void)
{
	return (!g_power_table) ? 0
	    : g_power_table[g_max_opp_idx_num - 1].gpufreq_power;
}

/* API : get static leakage power */
unsigned int mt_gpufreq_get_leakage_mw(void)
{
	int temp = 0;
#ifdef MT_GPUFREQ_STATIC_PWR_READY2USE
	unsigned int cur_vcore = __mt_gpufreq_get_cur_volt() / 100;
	int leak_power;
#endif				/* ifdef MT_GPUFREQ_STATIC_PWR_READY2USE */

#ifdef CONFIG_THERMAL
	temp = get_immediate_gpu_wrap() / 1000;
#else
	temp = 40;
#endif				/* ifdef CONFIG_THERMAL */

#ifdef MT_GPUFREQ_STATIC_PWR_READY2USE
	leak_power = mt_spower_get_leakage(MTK_SPOWER_GPU, cur_vcore, temp);
	if (g_volt_enable_state && leak_power > 0)
		return leak_power;
	else
		return 0;
#else
	return 130;
#endif				/* ifdef MT_GPUFREQ_STATIC_PWR_READY2USE */
}

/*
 * API : get current Thermal/Power/PBM limited OPP table index
 */
unsigned int mt_gpufreq_get_thermal_limit_index(void)
{
	return g_max_limited_idx - g_segment_max_opp_idx;
}

/*
 * API : get current Thermal/Power/PBM limited OPP table frequency
 */
unsigned int mt_gpufreq_get_thermal_limit_freq(void)
{
	return g_opp_table[g_max_limited_idx].gpufreq_khz;
}
EXPORT_SYMBOL(mt_gpufreq_get_thermal_limit_freq);

/*
 * API : get current OPP table conditional index
 */
unsigned int mt_gpufreq_get_cur_freq_index(void)
{
	return g_cur_opp_idx - g_segment_max_opp_idx;
}

/*
 * API : get current OPP table frequency
 */
unsigned int mt_gpufreq_get_cur_freq(void)
{
	return g_cur_opp_freq;
}
EXPORT_SYMBOL(mt_gpufreq_get_cur_freq);

/*
 * API : get current voltage
 */
unsigned int mt_gpufreq_get_cur_volt(void)
{
	return __mt_gpufreq_get_cur_volt();
}
EXPORT_SYMBOL(mt_gpufreq_get_cur_volt);

/* API : get Thermal/Power/PBM limited OPP table index */
int mt_gpufreq_get_cur_ceiling_idx(void)
{
	return (int)mt_gpufreq_get_thermal_limit_index();
}

#ifdef MT_GPUFREQ_BATT_OC_PROTECT
/*
 * API : Over Currents(OC) Callback
 */
void mt_gpufreq_batt_oc_callback(BATTERY_OC_LEVEL battery_oc_level)
{
	if (g_batt_oc_limited_ignore_state) {
		gpufreq_pr_debug("@%s: ignore Over Currents(OC) protection\n",
				 __func__);
		return;
	}

	gpufreq_pr_debug("@%s: battery_oc_level = %d\n", __func__,
			 battery_oc_level);

	g_batt_oc_level = battery_oc_level;

	if (battery_oc_level == BATTERY_OC_LEVEL_1) {
		if (g_batt_oc_limited_idx != g_batt_oc_limited_idx_lvl_1) {
			g_batt_oc_limited_idx = g_batt_oc_limited_idx_lvl_1;
		__mt_gpufreq_batt_oc_protect(g_batt_oc_limited_idx_lvl_1);
		/* Limit */
		}
	} else {
		if (g_batt_oc_limited_idx != g_batt_oc_limited_idx_lvl_0) {
			g_batt_oc_limited_idx = g_batt_oc_limited_idx_lvl_0;
	__mt_gpufreq_batt_oc_protect(g_batt_oc_limited_idx_lvl_0);
	/* Unlimit */
		}
	}
}
#endif /* ifdef MT_GPUFREQ_BATT_OC_PROTECT */

#ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT
/*
 * API : Battery Percentage Callback
 */
void mt_gpufreq_batt_percent_callback(BATTERY_PERCENT_LEVEL
				      battery_percent_level)
{
	if (g_thermal_ptc_limit_ignr_state) {
		gpufreq_pr_debug("@%s: ignore Battery Percentage protection\n",
				 __func__);
		return;
	}

	gpufreq_pr_debug("@%s: battery_percent_level = %d\n", __func__,
			 battery_percent_level);

	g_batt_percent_level = battery_percent_level;

	/* BATTERY_PERCENT_LEVEL_1: <= 15%, BATTERY_PERCENT_LEVEL_0: >15% */
	if (battery_percent_level == BATTERY_PERCENT_LEVEL_1) {
		if (g_batt_percent_limited_idx !=
		    g_batt_percent_limited_idx_lvl_1) {
			g_batt_percent_limited_idx =
			    g_batt_percent_limited_idx_lvl_1;
			__mt_gpufreq_batt_percent_protect
			    (g_batt_percent_limited_idx_lvl_1);
		}
	} else {
		if (g_batt_percent_limited_idx !=
		    g_batt_percent_limited_idx_lvl_0) {
			g_batt_percent_limited_idx =
			    g_batt_percent_limited_idx_lvl_0;
	__mt_gpufreq_batt_percent_protect(g_batt_percent_limited_idx_lvl_0);
		/* Unlimit */
		}
	}
}
#endif	/* ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT */

#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
/*
 * API : Low Battery Volume Callback
 */
void mt_gpufreq_low_batt_callback(LOW_BATTERY_LEVEL low_battery_level)
{
	if (g_low_batt_limited_ignore_state) {
		gpufreq_pr_debug("@%s: ignore Low Battery Volume protection\n",
				 __func__);
		return;
	}

	gpufreq_pr_debug("@%s: low_battery_level = %d\n", __func__,
			 low_battery_level);

	g_low_battery_level = low_battery_level;

	/*
	 * 3.25V HW issue int and is_low_battery = 1
	 * 3.10V HW issue int and is_low_battery = 2
	 */

	if (low_battery_level == LOW_BATTERY_LEVEL_2) {
		if (g_low_batt_limited_idx != g_low_batt_limited_idx_lvl_2) {
			g_low_batt_limited_idx = g_low_batt_limited_idx_lvl_2;
			__mt_gpufreq_low_batt_protect
			    (g_low_batt_limited_idx_lvl_2);
		}
	} else {
		if (g_low_batt_limited_idx != g_low_batt_limited_idx_lvl_0) {
			g_low_batt_limited_idx = g_low_batt_limited_idx_lvl_0;
		__mt_gpufreq_low_batt_protect(g_low_batt_limited_idx_lvl_0);
			/* Unlimit */
		}
	}
}
#endif	/* ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT */

/*
 * API : set limited OPP table index for Thermal protection
 */
void mt_gpufreq_thermal_protect(unsigned int limited_power)
{
	int i = -1;
	unsigned int limited_freq;

	mutex_lock(&mt_gpufreq_power_lock);

	if (g_thermal_ptc_limit_ignr_state) {
		gpufreq_pr_debug("@%s: ignore Thermal protection\n", __func__);
		mutex_unlock(&mt_gpufreq_power_lock);
		return;
	}

	if (limited_power == g_thermal_protect_power) {
		gpufreq_pr_debug
		    ("@%s: limited_power(%d mW) not changed, skip it\n",
		     __func__, limited_power);
		mutex_unlock(&mt_gpufreq_power_lock);
		return;
	}

	g_thermal_protect_power = limited_power;

#ifdef MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE
	__mt_update_gpufreqs_power_table();
#endif	/* ifdef MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE */

	if (limited_power == 0) {
		g_limited_idx_array[IDX_THERMAL_PROTECT_LIMITED]
		    = g_segment_max_opp_idx;
		__mt_gpufreq_update_max_limited_idx();
	} else {
		limited_freq =
		    __mt_gpufreq_get_limited_freq_by_power(limited_power);
		for (i = g_segment_max_opp_idx; i < g_max_opp_idx_num; i++) {
			if (g_opp_table[i].gpufreq_khz <= limited_freq) {
				g_limited_idx_array[IDX_THERMAL_PROTECT_LIMITED]
				    = i;
				__mt_gpufreq_update_max_limited_idx();
				if (g_cur_opp_freq > g_opp_table[i].gpufreq_khz)
					mt_gpufreq_target(i, true);
				break;
			}
		}
	}

	gpufreq_pr_debug("@%s: limited power index = %d, limited power = %d\n",
			 __func__, i, limited_power);

	mutex_unlock(&mt_gpufreq_power_lock);
}

/* API : set limited OPP table index by PBM */
void mt_gpufreq_set_power_limit_by_pbm(unsigned int limited_power)
{
	int i = -1;
	unsigned int limited_freq;

	mutex_lock(&mt_gpufreq_power_lock);

	if (g_pbm_limited_ignore_state) {
		gpufreq_pr_debug("@%s: ignore PBM Power limited\n", __func__);
		mutex_unlock(&mt_gpufreq_power_lock);
		return;
	}

	if (limited_power == g_pbm_limited_power) {
		gpufreq_pr_debug
		    ("@%s: limited_power(%d mW) not changed, skip it\n",
		     __func__, limited_power);
		mutex_unlock(&mt_gpufreq_power_lock);
		return;
	}

	g_pbm_limited_power = limited_power;

	if (limited_power == 0) {
		g_limited_idx_array[IDX_PBM_LIMITED] = g_segment_max_opp_idx;
		__mt_gpufreq_update_max_limited_idx();
	} else {
		limited_freq =
		    __mt_gpufreq_get_limited_freq_by_power(limited_power);
		for (i = g_segment_max_opp_idx; i < g_max_opp_idx_num; i++) {
			if (g_opp_table[i].gpufreq_khz <= limited_freq) {
				g_limited_idx_array[IDX_PBM_LIMITED] = i;
				__mt_gpufreq_update_max_limited_idx();
				break;
			}
		}
	}

	gpufreq_pr_debug("@%s: limited power index = %d, limited_power = %d\n",
			 __func__, i, limited_power);

	mutex_unlock(&mt_gpufreq_power_lock);
}

/*
 * API : register GPU power limited notifiction callback
 */
void mt_gpufreq_power_limit_notify_registerCB(gpufreq_power_limit_notify pCB)
{
	/* legacy */
}


/**
 * ===============================================
 * SECTION : PROCFS interface for debugging
 * ===============================================
 */

#ifdef CONFIG_PROC_FS
/*
 * PROCFS : show OPP table
 */
static int mt_gpufreq_opp_dump_proc_show(struct seq_file *m, void *v)
{
	int i;

	for (i = g_segment_max_opp_idx; i < g_max_opp_idx_num; i++) {
		seq_printf(m, "[%d] ", i - g_segment_max_opp_idx);
		seq_printf(m, "freq = %d, ", g_opp_table[i].gpufreq_khz);
		if (g_enable_aging_test) {
			if (g_opp_table[i].gpufreq_volt <= 75000) {
				seq_printf(m, "volt = %d, ",
					   g_opp_table[i].gpufreq_volt - 1250);
			} else {
				seq_printf(m, "volt = %d, ",
					   g_opp_table[i].gpufreq_volt - 1875);
			}
		} else {
			seq_printf(m, "volt = %d, ",
				   g_opp_table[i].gpufreq_volt);
		}
		seq_printf(m, "vsram = %d, ", g_opp_table[i].gpufreq_vsram);
		seq_printf(m, "gpufreq_power = %d\n",
			   g_power_table[i].gpufreq_power);
	}

	return 0;
}

/*
 * PROCFS : show OPP power table
 */
static int mt_gpufreq_power_dump_proc_show(struct seq_file *m, void *v)
{
	int i;

	for (i = g_segment_max_opp_idx; i < g_max_opp_idx_num; i++) {
		seq_printf(m, "[%d] ", i - g_segment_max_opp_idx);
		seq_printf(m, "freq = %d, ", g_power_table[i].gpufreq_khz);
		seq_printf(m, "volt = %d, ", g_power_table[i].gpufreq_volt);
		seq_printf(m, "power = %d\n", g_power_table[i].gpufreq_power);
	}

	return 0;
}


/*
 * PROCFS : show springboard table
 */
static int mt_gpufreq_sb_idx_proc_show(struct seq_file *m, void *v)
{
	int i;

	for (i = g_segment_max_opp_idx; i < g_max_opp_idx_num; i++) {
		seq_printf(m, "[%d] ", i - g_segment_max_opp_idx);
		seq_printf(m, "g_opp_sb_idx_up = %d, ",
			   g_opp_sb_idx_up[i] - g_segment_max_opp_idx >= 0
			   ? g_opp_sb_idx_up[i] - g_segment_max_opp_idx : 0);
		seq_printf(m, "g_opp_sb_idx_down = %d\n",
			   g_opp_sb_idx_down[i] - g_segment_max_opp_idx >= 0
			   ? g_opp_sb_idx_down[i] - g_segment_max_opp_idx : 0);
	}

	return 0;
}

/*
 * PROCFS : show important variables for debugging
 */
#ifdef CONFIG_MTK_QOS_SUPPORT
static int g_cur_vcore_opp = VCORE_OPP_0;
#endif
static int mt_gpufreq_var_dump_proc_show(struct seq_file *m, void *v)
{
	int i;
	unsigned int gpu_loading = 0;

	mtk_get_gpu_loading(&gpu_loading);

	seq_printf(m, "g_cur_opp_idx = %d, g_cur_opp_freq = %d, g_cur_opp_volt = %d, g_cur_opp_vsram_volt = %d\n",
		   g_cur_opp_idx - g_segment_max_opp_idx,
		   g_opp_table[g_cur_opp_idx].gpufreq_khz,
		   g_cur_opp_volt, g_cur_opp_vsram_volt);

	seq_printf(m, "MFGPLL freq = %d, real volt = %d,real vsram_volt = %d\n",
		    __mt_gpufreq_get_cur_freq(),
		   __mt_gpufreq_get_cur_volt(),
		   __mt_gpufreq_get_cur_vsram_volt());
#ifdef CONFIG_MTK_QOS_SUPPORT
	seq_printf(m, "current vcore opp = %d\n", g_cur_vcore_opp);
#endif
	seq_printf(m, "g_segment_id = %d\n", g_segment_id);

	seq_printf(m, "g_dvfs_enable = %d\n", g_dvfs_enable);
	seq_printf(m, "g_volt_enable_state = %d\n", g_volt_enable_state);
	seq_printf(m, "g_opp_stress_test_state = %d\n",
		   g_opp_stress_test_state);
	seq_printf(m, "g_DVFS_off_by_ptpod_idx = %d\n",
		   g_DVFS_off_by_ptpod_idx - g_segment_max_opp_idx);
	seq_printf(m, "g_max_limited_idx = %d\n",
		   g_max_limited_idx - g_segment_max_opp_idx);

	seq_printf(m, "gpu_loading = %d\n", gpu_loading);

	for (i = 0; i < NUMBER_OF_LIMITED_IDX; i++)
		seq_printf(m, "g_limited_idx_array[%d] = %d\n",
			   i, g_limited_idx_array[i] - g_segment_max_opp_idx);

	return 0;
}

/*
 * PROCFS : show current opp stress test state
 */
static int mt_gpufreq_opp_stress_test_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "g_opp_stress_test_state = %d\n",
		   g_opp_stress_test_state);
	return 0;
}

/*
 * PROCFS : opp stress test message setting
 * 0 : disable
 * 1 : enable
 */
static ssize_t mt_gpufreq_opp_stress_test_proc_write(struct file *file,
						     const char __user *buffer,
						     size_t count,
						     loff_t *data)
{
	char buf[64];
	unsigned int len = 0;
	unsigned int value = 0;
	int ret = -EFAULT;

	len = (count < (sizeof(buf) - 1)) ? count : (sizeof(buf) - 1);

	if (copy_from_user(buf, buffer, len))
		goto out;

	buf[len] = '\0';

	if (!kstrtouint(buf, 10, &value)) {
		if (!value || !(value - 1)) {
			ret = 0;
			g_opp_stress_test_state = value;
		}
	}

out:
	return (ret < 0) ? ret : count;
}

/*
 * PROCFS : show Thermal/Power/PBM limited ignore state
 * 0 : consider
 * 1 : ignore
 */
static int mt_gpufreq_power_limited_proc_show(struct seq_file *m, void *v)
{
	seq_puts(m, "GPU-DVFS power limited state ....\n");
#ifdef MT_GPUFREQ_BATT_OC_PROTECT
	seq_printf(m, "g_batt_oc_limited_ignore_state = %d\n",
		   g_batt_oc_limited_ignore_state);
#endif				/* ifdef MT_GPUFREQ_BATT_OC_PROTECT */
#ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT
	seq_printf(m, "g_batt_percent_limited_ignore_state = %d\n",
		   g_thermal_ptc_limit_ignr_state);
#endif				/* ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT */
#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
	seq_printf(m, "g_low_batt_limited_ignore_state = %d\n",
		   g_low_batt_limited_ignore_state);
#endif				/* ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT */
	seq_printf(m, "g_thermal_protect_limited_ignore_state = %d\n",
		   g_thermal_ptc_limit_ignr_state);
	seq_printf(m, "g_pbm_limited_ignore_state = %d\n",
		   g_pbm_limited_ignore_state);
	return 0;
}

/*
 * PROCFS : ignore state or power value setting for Thermal/Power/PBM limit
 */
static ssize_t mt_gpufreq_power_limited_proc_write(struct file *file,
						   const char __user *buffer,
						   size_t count, loff_t *data)
{
	char buf[64];
	unsigned int len = 0;
	int ret = -EFAULT;
	unsigned int i;
	unsigned int size;
	unsigned int value = 0;
	static const char *const array[] = {
#ifdef MT_GPUFREQ_BATT_OC_PROTECT
		"ignore_batt_oc",
#endif				/* ifdef MT_GPUFREQ_BATT_OC_PROTECT */
#ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT
		"ignore_batt_percent",
#endif				/* ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT */
#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
		"ignore_low_batt",
#endif				/* ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT */
		"ignore_thermal_protect",
		"ignore_pbm_limited",
		"pbm_limited_power",
		"thermal_protect_power",
	};

	len = (count < (sizeof(buf) - 1)) ? count : (sizeof(buf) - 1);

	if (copy_from_user(buf, buffer, len))
		goto out;

	buf[len] = '\0';

	size = ARRAY_SIZE(array);

	for (i = 0; i < size; i++) {
		if (strncmp(array[i], buf,
			MIN(strlen(array[i]), count)) == 0) {
			char cond_buf[64];

			snprintf(cond_buf, sizeof(cond_buf), "%s %%u",
				 array[i]);
			if (sscanf(buf, cond_buf, &value) == 1) {
				ret = 0;
				if (strncmp(array[i], "pbm_limited_power",
				     strlen(array[i])) == 0) {
					mt_gpufreq_set_power_limit_by_pbm
					    (value);
				} else if (strncmp(array[i],
					"thermal_protect_power",
					 strlen(array[i])) == 0) {
					mt_gpufreq_thermal_protect(value);
				}
#ifdef MT_GPUFREQ_BATT_OC_PROTECT
				else if (strncmp(array[i], "ignore_batt_oc",
					  strlen(array[i])) == 0) {
					if (!value || !(value - 1)) {
						g_batt_oc_limited_ignore_state
						= (value) ? true : false;
						g_limited_ignore_array
						    [IDX_BATT_OC_LIMITED] =
					    g_batt_oc_limited_ignore_state;
					}
				}
#endif				/* ifdef MT_GPUFREQ_BATT_OC_PROTECT */
#ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT
				else if (strncmp(array[i],
					"ignore_batt_percent",
					  strlen(array[i])) == 0) {
					if (!value || !(value - 1)) {
						g_batt_percent_limit_ignr_state
						    = (value) ? true : false;
						g_limited_ignore_array
						    [IDX_BATT_PERCENT_LIMITED] =
					    g_batt_percent_limit_ignr_state;
					}
				}
#endif				/* ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT */
#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
				else if (strncmp(array[i],
					"ignore_low_batt",
					  strlen(array[i])) == 0) {
					if (!value || !(value - 1)) {
						g_low_batt_limited_ignore_state
						    = (value) ? true : false;
						g_limited_ignore_array
						    [IDX_LOW_BATT_LIMITED] =
					    g_low_batt_limited_ignore_state;
					}
				}
#endif				/* ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT */
				else if (strncmp(array[i],
					"ignore_thermal_protect",
					  strlen(array[i])) == 0) {
					if (!value || !(value - 1)) {
						g_thermal_ptc_limit_ignr_state
						    = (value) ? true : false;
						g_limited_ignore_array
						   [IDX_THERMAL_PROTECT_LIMITED]
				 = g_thermal_ptc_limit_ignr_state;
					}
				} else if (strncmp(array[i],
						"ignore_pbm_limited",
					 strlen(array[i])) == 0) {
					if (!value || !(value - 1)) {
						g_pbm_limited_ignore_state =
						    (value) ? true : false;
						g_limited_ignore_array
						    [IDX_PBM_LIMITED] =
						    g_pbm_limited_ignore_state;
					}
				}
				break;
			}
		}
	}

out:
	return (ret < 0) ? ret : count;
}

/*
 * PROCFS : show current keeping OPP frequency state
 */
static int mt_gpufreq_opp_freq_proc_show(struct seq_file *m, void *v)
{
	if (g_keep_opp_freq_state) {
		seq_puts(m, "Keeping OPP frequency is enabled\n");
		seq_printf(m, "[%d] ",
			   g_keep_opp_freq_idx - g_segment_max_opp_idx);
		seq_printf(m, "freq = %d, ",
			   g_opp_table[g_keep_opp_freq_idx].gpufreq_khz);
		seq_printf(m, "volt = %d, ",
			   g_opp_table[g_keep_opp_freq_idx].gpufreq_volt);
		seq_printf(m, "vsram = %d\n",
			   g_opp_table[g_keep_opp_freq_idx].gpufreq_vsram);
	} else
		seq_puts(m, "Keeping OPP frequency is disabled\n");

	return 0;
}

/*
 * PROCFS : keeping OPP frequency setting
 * 0 : free run
 * 1 : keep OPP frequency
 */
static ssize_t mt_gpufreq_opp_freq_proc_write(struct file *file,
					      const char __user *buffer,
					      size_t count, loff_t *data)
{
	char buf[64];
	unsigned int len = 0;
	unsigned int value = 0;
	unsigned int i = 0;
	int ret = -EFAULT;

	len = (count < (sizeof(buf) - 1)) ? count : (sizeof(buf) - 1);

	if (copy_from_user(buf, buffer, len))
		goto out;

	buf[len] = '\0';

	if (kstrtouint(buf, 10, &value) == 0) {
		if (value == 0) {
			g_keep_opp_freq_state = false;
		} else {
			for (i = g_segment_max_opp_idx;
			     i < g_max_opp_idx_num; i++) {
				if (value == g_opp_table[i].gpufreq_khz) {
					ret = 0;
					g_keep_opp_freq_idx = i;
					g_keep_opp_freq_state = true;
					g_keep_opp_freq = value;
					mt_gpufreq_target(i, true);
					break;
				}
			}
		}
	}

out:
	return (ret < 0) ? ret : count;
}

/*
 * PROCFS : show current fixed freq & volt state
 */
static int mt_gpufreq_fixed_freq_volt_proc_show(struct seq_file *m, void *v)
{
	if (g_fixed_freq_volt_state) {
		seq_puts(m, "GPU-DVFS fixed freq & volt is enabled\n");
		seq_printf(m, "g_fixed_freq = %d\n", g_fixed_freq);
		seq_printf(m, "g_fixed_volt = %d\n", g_fixed_volt);
	} else
		seq_puts(m, "GPU-DVFS fixed freq & volt is disabled\n");

	return 0;
}

/*
 * PROCFS : fixed freq & volt state setting
 */
static ssize_t mt_gpufreq_fixed_freq_volt_proc_write(struct file *file,
						     const char __user *buffer,
						     size_t count,
						     loff_t *data)
{
	char buf[64];
	unsigned int len = 0;
	int ret = -EFAULT;
	unsigned int fixed_freq = 0;
	unsigned int fixed_volt = 0;

	len = (count < (sizeof(buf) - 1)) ? count : (sizeof(buf) - 1);

	if (copy_from_user(buf, buffer, len))
		goto out;

	buf[len] = '\0';

	if (sscanf(buf, "%d %d", &fixed_freq, &fixed_volt) == 2) {
		ret = 0;
		if ((fixed_freq == 0) && (fixed_volt == 0)) {
			g_fixed_freq_volt_state = false;
			g_fixed_freq = 0;
			g_fixed_volt = 0;
		} else {
			g_cur_opp_freq = __mt_gpufreq_get_cur_freq();
			fixed_volt = VOLT_NORMALIZATION(fixed_volt);
			if (fixed_freq > g_cur_opp_freq) {
				__mt_gpufreq_set_fixed_volt(fixed_volt);
				__mt_gpufreq_set_fixed_freq(fixed_freq);
			} else {
				__mt_gpufreq_set_fixed_freq(fixed_freq);
				__mt_gpufreq_set_fixed_volt(fixed_volt);
			}
			g_fixed_freq_volt_state = true;
		}
	}

out:
	return (ret < 0) ? ret : count;
}

/*
 * PROCFS : set gpufreq dvfs status
 * 0 : disable, skip freq setting
 * 1 : enable
 */
static ssize_t mt_gpufreq_dvfs_enable_proc_write(struct file *file,
						 const char __user *buffer,
						 size_t count, loff_t *data)
{
	char buf[64];
	unsigned int len = 0;
	unsigned int value = 0;
	unsigned int i = 0;
	int ret = -EFAULT;

	len = (count < (sizeof(buf) - 1)) ? count : (sizeof(buf) - 1);

	if (copy_from_user(buf, buffer, len))
		goto out;

	buf[len] = '\0';

	if (kstrtouint(buf, 10, &value) == 0) {
		if (value == 0)
			g_dvfs_enable = false;
		else
			g_dvfs_enable = true;
	}

out:
	return (ret < 0) ? ret : count;
}

static int mt_gpufreq_dvfs_enable_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "g_dvfs_enable = %d\n", g_dvfs_enable);
	return 0;
}


/*
 * PROCFS : initialization
 */
PROC_FOPS_RW(gpufreq_opp_stress_test);
PROC_FOPS_RW(gpufreq_power_limited);
PROC_FOPS_RO(gpufreq_opp_dump);
PROC_FOPS_RO(gpufreq_power_dump);
PROC_FOPS_RW(gpufreq_opp_freq);
PROC_FOPS_RO(gpufreq_var_dump);
PROC_FOPS_RW(gpufreq_fixed_freq_volt);
PROC_FOPS_RO(gpufreq_sb_idx);
PROC_FOPS_RW(gpufreq_dvfs_enable);
static int __mt_gpufreq_create_procfs(void)
{
	struct proc_dir_entry *dir = NULL;
	int i;

	struct pentry {
		const char *name;
		const struct file_operations *fops;
	};

	const struct pentry entries[] = {
		PROC_ENTRY(gpufreq_opp_stress_test),
		PROC_ENTRY(gpufreq_power_limited),
		PROC_ENTRY(gpufreq_opp_dump),
		PROC_ENTRY(gpufreq_power_dump),
		PROC_ENTRY(gpufreq_opp_freq),
		PROC_ENTRY(gpufreq_var_dump),
		PROC_ENTRY(gpufreq_fixed_freq_volt),
		PROC_ENTRY(gpufreq_sb_idx),
		PROC_ENTRY(gpufreq_dvfs_enable),
	};

	dir = proc_mkdir("gpufreq", NULL);
	if (!dir) {
		gpufreq_pr_err("@%s: fail to create /proc/gpufreq\n", __func__);
		return -ENOMEM;
	}

	for (i = 0; i < ARRAY_SIZE(entries); i++) {
		if (!proc_create
		    (entries[i].name, 0664, dir,
		     entries[i].fops))
			gpufreq_pr_err("@%s: create /proc/gpufreq/%s failed\n",
				       __func__, entries[i].name);
	}

	return 0;
}
#endif				/* ifdef CONFIG_PROC_FS */

/**
 * ===============================================
 * SECTION : Local functions definition
 * ===============================================
 */

/*
 * calculate springboard opp index to avoid buck variation,
 * the voltage between VGPU and VSRAM_GPU must be in 100mV ~ 250mV
 * (Vgpu +8%~-6.25%, Vgpu_sram +-47mv)
 */
static void __mt_gpufreq_cal_sb_opp_index(void)
{
	int i, j, diff;
	int min_vsram_idx = g_max_opp_idx_num - 1;

	/* find 0.85 index */
	for (i = 0; i < g_max_opp_idx_num; i++) {
		if (g_opp_table[i].gpufreq_vsram == SEG_GPU_DVFS_VSRAM1) {
			min_vsram_idx = i;
			break;
		}
	}

	gpufreq_pr_debug("@%s: min_vsram_idx: %d\n", __func__, min_vsram_idx);

	/* build up table */
	for (i = 0; i < g_max_opp_idx_num; i++) {
		g_opp_sb_idx_up[i] = min_vsram_idx;
		for (j = 0; j <= min_vsram_idx; j++) {
			diff = g_opp_table[i].gpufreq_volt + BUCK_DIFF_MAX;
			if (g_opp_table[j].gpufreq_vsram <= diff) {
				g_opp_sb_idx_up[i] = j;
				break;
			}
		}
		gpufreq_pr_debug("@%s: g_opp_sb_idx_up[%d]: %d\n",
				 __func__, i, g_opp_sb_idx_up[i]);
	}

	/* build down table */
	for (i = 0; i < g_max_opp_idx_num; i++) {
		if (i >= min_vsram_idx)
			g_opp_sb_idx_down[i] = g_max_opp_idx_num - 1;
		else {
			for (j = g_max_opp_idx_num - 1; j >= 0; j--) {
				diff =
				    g_opp_table[i].gpufreq_vsram -
				    BUCK_DIFF_MAX;
				if (g_opp_table[j].gpufreq_volt >= diff) {
					g_opp_sb_idx_down[i] = j;
					break;
				}
			}
		}

		gpufreq_pr_debug("@%s: g_opp_sb_idx_down[%d]: %d\n",
				 __func__, i, g_opp_sb_idx_down[i]);
	}
}

/*
 * switch VGPU voltage via VCORE
 */

static void __mt_gpufreq_vcore_volt_switch(unsigned int volt_target)
{
#ifdef CONFIG_MTK_QOS_SUPPORT
	if (volt_target > 70000) {
		pm_qos_update_request(&g_pmic->pm_vgpu, VCORE_OPP_0);
		pm_qos_update_request(&g_pmic->pm_v_emi_gpu, DDR_OPP_0);
		g_cur_vcore_opp = VCORE_OPP_0;
	} else if (volt_target > 65000) {
		pm_qos_update_request(&g_pmic->pm_vgpu, VCORE_OPP_1);
		pm_qos_update_request(&g_pmic->pm_v_emi_gpu, DDR_OPP_1);
		g_cur_vcore_opp = VCORE_OPP_1;
	} else if (volt_target > 0) {
		pm_qos_update_request(&g_pmic->pm_vgpu, VCORE_OPP_2);
		pm_qos_update_request(&g_pmic->pm_v_emi_gpu, DDR_OPP_2);
		g_cur_vcore_opp = VCORE_OPP_2;
	} else {			/* UNREQUEST */
		pm_qos_update_request(&g_pmic->pm_vgpu, VCORE_OPP_UNREQ);
		pm_qos_update_request(&g_pmic->pm_v_emi_gpu, DDR_OPP_UNREQ);
	}
	gpufreq_pr_debug("@%s:volt_target = %d, cur_v = %d\n",
		__func__, volt_target, mt_gpufreq_get_cur_volt());
#endif
}

/*
 * frequency ramp up/down handler
 * - frequency ramp up need to wait voltage settle
 * - frequency ramp down do not need to wait voltage settle
 */
static void __mt_gpufreq_set(unsigned int idx_old, unsigned int idx_new,
			     unsigned int freq_old, unsigned int freq_new,
			     unsigned int volt_old, unsigned int volt_new,
			     unsigned int vsram_volt_old,
			     unsigned int vsram_volt_new)
{
	unsigned int sb_idx = 0;

	gpufreq_pr_debug("@%s: begin idx:%d->%d,f:%d->%d,v:%d->%d,vram:%d->%d\n",
	     __func__, idx_old, idx_new, freq_old,
	     freq_new, volt_old, volt_new,
	     vsram_volt_old, vsram_volt_new);

	if (freq_new > freq_old) {
		while (idx_old != idx_new) {
			sb_idx = g_opp_sb_idx_up[idx_old] < idx_new
			    ? idx_new : g_opp_sb_idx_up[idx_old];
#ifdef USE_STAND_ALONE_VGPU
			__mt_gpufreq_volt_switch(volt_old,
					 g_opp_table[sb_idx].gpufreq_volt,
					 vsram_volt_old,
					 g_opp_table[sb_idx].gpufreq_vsram);
#else
			__mt_gpufreq_vcore_volt_switch(volt_new);
#endif

			gpufreq_pr_debug("@%s:idx:v_swih:%d->%d,v:%d->%d,ram_v:%d->%d\n",
			     __func__, idx_old, sb_idx, volt_old,
			     g_opp_table[sb_idx].gpufreq_volt, vsram_volt_old,
			     g_opp_table[sb_idx].gpufreq_vsram);

			idx_old = sb_idx;
		}
		__mt_gpufreq_clock_switch(freq_new);
	} else {
		__mt_gpufreq_clock_switch(freq_new);
		while (idx_old != idx_new) {
			sb_idx = g_opp_sb_idx_down[idx_old] > idx_new
			    ? idx_new : g_opp_sb_idx_down[idx_old];
#ifdef USE_STAND_ALONE_VGPU
			__mt_gpufreq_volt_switch(volt_old,
					g_opp_table[sb_idx].gpufreq_volt,
					vsram_volt_old,
					g_opp_table[sb_idx].gpufreq_vsram);
#else
			__mt_gpufreq_vcore_volt_switch(volt_new);
#endif

			gpufreq_pr_debug("@%s:idx:v_swih:%d->%d,v:%d->%d,ram_v:%d->%d\n",
			     __func__, idx_old, sb_idx, volt_old,
			     g_opp_table[sb_idx].gpufreq_volt, vsram_volt_old,
			     g_opp_table[sb_idx].gpufreq_vsram);

			idx_old = sb_idx;
			idx_old = sb_idx;
		}
	}

	g_cur_opp_freq = freq_new;
	g_cur_opp_volt = volt_new;
	g_cur_opp_vsram_volt = vsram_volt_new;

	gpufreq_pr_debug("@%s:donefreq: %d->%d, volt: %d->%d,ram_volt: %d->%d\n",
	     __func__, freq_old, __mt_gpufreq_get_cur_freq(),
	     volt_old, volt_new,
	     vsram_volt_old, vsram_volt_new);

	__mt_gpufreq_kick_pbm(1);
}

/*
 * switch clock(frequency) via PLL
 */
static void __mt_gpufreq_clock_switch(unsigned int freq_new)
{
	unsigned int cur_volt;
	unsigned int cur_freq;
	unsigned long target_freq;
	int ret = -99;

	cur_volt = __mt_gpufreq_get_cur_volt();
	cur_freq = __mt_gpufreq_get_cur_freq();
	target_freq = freq_new * 1000;

	gpufreq_pr_debug("@%s: rqst c_volt= %d, c_freq= %d, target[%ld]\n",
		__func__, cur_volt, cur_freq, target_freq);

	__mt_gpufreq_switch_to_clksrc(CLOCK_SUB);

	ret = clk_set_rate(g_clk->clk_pll_src, target_freq);
	if (ret)
		gpufreq_pr_info("@%s:failed set c_freq= %d,target[%ld] erro[%d]\n",
			__func__, cur_freq, target_freq, ret);
	udelay(10);
	__mt_gpufreq_switch_to_clksrc(CLOCK_MAIN);
}

/*
 * switch to target clock source
 */
static void __mt_gpufreq_switch_to_clksrc(enum g_clock_source_enum clksrc)
{
	int ret;

	ret = clk_prepare_enable(g_clk->clk_mux);
	if (ret)
		gpufreq_pr_debug("@%s: enable clk_mux(TOP_MUX_MFG)failed,ret = %d\n",
		     __func__, ret);

	if (clksrc == CLOCK_MAIN) {
		clk_set_parent(g_clk->clk_mux, g_clk->clk_main_parent);
		gpufreq_pr_debug("@%s: switch to main clock source done, freq[%ld]\n",
				 __func__, clk_get_rate(g_clk->clk_mux));
	} else if (clksrc == CLOCK_SUB) {
		clk_set_parent(g_clk->clk_mux, g_clk->clk_sub_parent);
		gpufreq_pr_debug("@%s: switch to sub clock source done , freq[%ld]\n",
				 __func__, clk_get_rate(g_clk->clk_mux));
	} else {
		gpufreq_pr_debug("@%s: clock source index is not valid, clksrc = %d\n",
		     __func__, clksrc);
	}

	clk_disable_unprepare(g_clk->clk_mux);
}

/*
 * switch voltage and vsram via PMIC
 */
static void __mt_gpufreq_volt_switch_without_vsram_volt(unsigned int volt_old,
							unsigned int volt_new)
{
#if 0
	unsigned int vsram_volt_new, vsram_volt_old;

	volt_new = VOLT_NORMALIZATION(volt_new);

	gpufreq_pr_debug("@%s: volt_new = %d, volt_old = %d\n", __func__,
			 volt_new, volt_old);

	vsram_volt_new = __mt_gpufreq_get_vsram_by_target_volt(volt_new);
	vsram_volt_old = __mt_gpufreq_get_vsram_by_target_volt(volt_old);

	__mt_gpufreq_volt_switch(volt_old, volt_new, vsram_volt_old,
				 vsram_volt_new);
#endif
}

/*
 * switch voltage and vsram via PMIC
 */
static void __mt_gpufreq_volt_switch(unsigned int volt_old,
				     unsigned int volt_new,
				     unsigned int vsram_volt_old,
				     unsigned int vsram_volt_new)
{
#if 0
	gpufreq_pr_debug("@%s: v_new = %d,v_old= %d, ram_v_n= %d,ram_v_o=%d\n",
		__func__, volt_new, volt_old, vsram_volt_new, vsram_volt_old);

	if (volt_new > volt_old) {
		if (vsram_volt_new > vsram_volt_old) {
#ifdef USE_STAND_ALONE_VSRAM
			__mt_gpufreq_vsram_gpu_volt_switch(VOLT_RISING,
							   g_vsram_sfchg_rrate,
							   vsram_volt_old,
							   vsram_volt_new);
#endif
		}
#ifdef USE_STAND_ALONE_VGPU
		__mt_gpufreq_vgpu_volt_switch(VOLT_RISING, g_vgpu_sfchg_rrate,
					      volt_old, volt_new);
		gpufreq_pr_debug("@%s: [RISING] vgpu_volt= %d, vsram_gpu_volt= %d\n",
		     __func__, regulator_get_voltage(g_pmic->reg_vgpu),
		     regulator_get_voltage(g_pmic->reg_vsram_gpu));
#endif
	} else if (volt_new < volt_old) {
#ifdef USE_STAND_ALONE_VGPU
		__mt_gpufreq_vgpu_volt_switch(VOLT_FALLING, g_vgpu_sfchg_frate,
					      volt_old, volt_new);
#endif
		if (vsram_volt_new < vsram_volt_old) {
#ifdef USE_STAND_ALONE_VSRAM
			__mt_gpufreq_vsram_gpu_volt_switch(VOLT_FALLING,
							   g_vsram_sfchg_frate,
							   vsram_volt_old,
							   vsram_volt_new);
		}
		gpufreq_pr_debug("@%s: [FALLING] vgpu_volt = %d, vsram_gpu_volt = %d\n",
		     __func__, regulator_get_voltage(g_pmic->reg_vgpu),
		     regulator_get_voltage(g_pmic->reg_vsram_gpu));
#endif
	}
#endif
}

/*
 * switch VSRAM_GPU voltage via PMIC
 */
static void __mt_gpufreq_vsram_gpu_volt_switch(
			enum g_volt_switch_enum switch_way,
			unsigned int sfchg_rate,
			unsigned int volt_old,
			unsigned int volt_new)
{

#ifdef USE_STAND_ALONE_VSRAM
	unsigned int max_diff, steps;

	max_diff =
	    (switch_way ==
	     VOLT_RISING) ? (volt_new - volt_old) : (volt_old - volt_new);

	if (max_diff == 0)
		return;

	steps = (max_diff / DELAY_FACTOR) + 1;

	regulator_set_voltage(g_pmic->reg_vsram_gpu, volt_new * 10,
			      VSRAM_GPU_MAX_VOLT * 10 + 125);
	udelay(steps * sfchg_rate + 52);

	gpufreq_pr_debug("@%s: udelay us(%d) = steps(%d) * sfchg_rate(%d)\n",
			 __func__, steps * sfchg_rate, steps, sfchg_rate);
#endif
}

/*
 * switch VGPU voltage via PMIC
 */
static void __mt_gpufreq_vgpu_volt_switch(enum g_volt_switch_enum switch_way,
					  unsigned int sfchg_rate,
					  unsigned int volt_old,
					  unsigned int volt_new)
{
#ifdef USE_STAND_ALONE_VGPU
	unsigned int max_diff, steps;

	max_diff =
	    (switch_way ==
	     VOLT_RISING) ? (volt_new - volt_old) : (volt_old - volt_new);

	if (max_diff == 0)
		return;

	steps = (max_diff / DELAY_FACTOR) + 1;

	regulator_set_voltage(g_pmic->reg_vgpu, volt_new * 10,
			      VGPU_MAX_VOLT * 10 + 125);
	udelay(steps * sfchg_rate + 52);

	gpufreq_pr_debug("@%s: udelay us(%d) = steps(%d) * sfchg_rate(%d)\n",
			 __func__, steps * sfchg_rate, steps, sfchg_rate);
#endif
}

/*
 * enable bucks (VGPU and VSRAM_GPU)
 */
static void __mt_gpufreq_bucks_enable(void)
{
	int ret;
#ifdef USE_STAND_ALONE_VSRAM
#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_status(0x10 |
				   (aee_rr_curr_gpu_dvfs_status() & 0x0F));
#endif

	if (regulator_is_enabled(g_pmic->reg_vsram_gpu) == 0) {
		ret = regulator_enable(g_pmic->reg_vsram_gpu);
		if (ret) {
			gpufreq_pr_err("@%s: enable VSRAM_GPU failed, ret = %d\n",
			     __func__, ret);
			return;
		}
	}
#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_status(0x20 |
				   (aee_rr_curr_gpu_dvfs_status() & 0x0F));
#endif
#endif
#ifdef USE_STAND_ALONE_VGPU
	if (regulator_is_enabled(g_pmic->reg_vgpu) == 0) {
		ret = regulator_enable(g_pmic->reg_vgpu);
		if (ret) {
			gpufreq_pr_err("@%s: enable VGPU failed, ret = %d\n",
				       __func__, ret);
			return;
		}
	}
#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_status(0x30 |
				   (aee_rr_curr_gpu_dvfs_status() & 0x0F));
#endif
	gpufreq_pr_debug("@%s: bucks is enabled\n", __func__);
#else
	__mt_gpufreq_vcore_volt_switch(g_cur_opp_volt);
#endif

}

/*
 * disable bucks (VGPU and VSRAM_GPU)
 */
static void __mt_gpufreq_bucks_disable(void)
{
	int ret;
#ifdef USE_STAND_ALONE_VGPU
#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_status(0x40 |
				   (aee_rr_curr_gpu_dvfs_status() & 0x0F));
#endif

	if (regulator_is_enabled(g_pmic->reg_vgpu) > 0) {
		ret = regulator_disable(g_pmic->reg_vgpu);
		if (ret) {
			gpufreq_pr_err("@%s: disable VGPU failed, ret = %d\n",
				       __func__, ret);
			return;
		}
	}
#else
	__mt_gpufreq_vcore_volt_switch(0);
#endif

#ifdef USE_STAND_ALONE_VSRAM
#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_status(0x50 |
				   (aee_rr_curr_gpu_dvfs_status() & 0x0F));
#endif

	if (regulator_is_enabled(g_pmic->reg_vsram_gpu) > 0) {
		ret = regulator_disable(g_pmic->reg_vsram_gpu);
		if (ret) {
			gpufreq_pr_err
			    ("@%s: disable VSRAM_GPU failed, ret = %d\n",
			     __func__, ret);
			return;
		}
	}
#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_status(0x60 |
				   (aee_rr_curr_gpu_dvfs_status() & 0x0F));
#endif
	gpufreq_pr_debug("@%s: bucks is disabled\n", __func__);
#endif
}

/*
 * set AUTO_MODE or PWM_MODE to PMIC(VGPU)
 * REGULATOR_MODE_FAST: PWM Mode
 * REGULATOR_MODE_NORMAL: Auto Mode
 */
static void __mt_gpufreq_vgpu_set_mode(unsigned int mode)
{
	int ret;

	ret = regulator_set_mode(g_pmic->reg_vgpu, mode);
	if (ret == 0)
		gpufreq_pr_debug("@%s:set AUT_M(%d)PWM_M(%d)2PMIC(VGPU),mode = %d\n",
			__func__, REGULATOR_MODE_NORMAL,
			REGULATOR_MODE_FAST, mode);
	else
		gpufreq_pr_err("@%s: failed to configure mode, ret= %d, mode = %d\n",
			__func__, ret, mode);
}

/*
 * set fixed frequency for PROCFS: fixed_freq_volt
 */
static void __mt_gpufreq_set_fixed_freq(int fixed_freq)
{
	gpufreq_pr_debug("@%s: before, g_fixed_freq = %d, g_fixed_volt = %d\n",
			 __func__, g_fixed_freq, g_fixed_volt);
	g_fixed_freq = fixed_freq;
	g_fixed_volt = g_cur_opp_volt;
	gpufreq_pr_debug("@%s: now, g_fixed_freq = %d, g_fixed_volt = %d\n",
			 __func__, g_fixed_freq, g_fixed_volt);
	__mt_gpufreq_clock_switch(g_fixed_freq);
	g_cur_opp_freq = g_fixed_freq;
}

/*
 * set fixed voltage for PROCFS: fixed_freq_volt
 */
static void __mt_gpufreq_set_fixed_volt(int fixed_volt)
{
	gpufreq_pr_debug("@%s: before, g_fixed_freq = %d, g_fixed_volt = %d\n",
			 __func__, g_fixed_freq, g_fixed_volt);
	g_fixed_freq = g_cur_opp_freq;
	g_fixed_volt = fixed_volt;
	gpufreq_pr_debug("@%s: now, g_fixed_freq = %d, g_fixed_volt = %d\n",
			 __func__, g_fixed_freq, g_fixed_volt);
#ifdef USE_STAND_ALONE_VGPU
	__mt_gpufreq_volt_switch_without_vsram_volt(g_cur_opp_volt,
						    g_fixed_volt);
#else
	regulator_set_voltage(g_pmic->reg_vcore, g_fixed_volt * 10,
			      g_fixed_volt * 10 + 125);
#endif

	g_cur_opp_volt = g_fixed_volt;

	g_cur_opp_vsram_volt =
	    __mt_gpufreq_get_vsram_by_target_volt(g_fixed_volt);
}

/*
 * dds calculation for clock switching
 */
static unsigned int __mt_gpufreq_calculate_dds(unsigned int freq_khz,
					       enum g_post_divider_power_enum
					       post_divider_power)
{
	unsigned int dds = 0;

	gpufreq_pr_debug("@%s: request freq = %d, post_divider = %d\n",
			 __func__, freq_khz, (1 << post_divider_power));

	/* [MT8168] dds is GPUPLL_CON1[21:0] */
	if ((freq_khz >= POST_DIV_8_MIN_FREQ)
	    && (freq_khz <= POST_DIV_2_MAX_FREQ)) {
		dds =
		    (((freq_khz / TO_MHz_HEAD *
		       (1 << post_divider_power)) << DDS_SHIFT)
		     / GPUPLL_FIN + ROUNDING_VALUE) / TO_MHz_TAIL;
	} else {
		gpufreq_pr_err("@%s: out of range, freq_khz = %d\n", __func__,
			       freq_khz);
	}

	return dds;
}

/* power calculation for power table */
static void __mt_gpufreq_calculate_power(unsigned int idx, unsigned int freq,
					 unsigned int volt, unsigned int temp)
{
	unsigned int p_total = 0;
	unsigned int p_dynamic = 0;
	unsigned int ref_freq = 0;
	unsigned int ref_volt = 0;
	int p_leakage = 0;

	p_dynamic = GPU_ACT_REF_POWER;
	ref_freq = GPU_ACT_REF_FREQ;
	ref_volt = GPU_ACT_REF_VOLT;

	p_dynamic = p_dynamic *
	    ((freq * 100) / ref_freq) *
	    ((volt * 100) / ref_volt) * ((volt * 100) / ref_volt) / (100 * 100 *
								     100);

#ifdef MT_GPUFREQ_STATIC_PWR_READY2USE
	p_leakage = mt_spower_get_leakage(MTK_SPOWER_GPU, (volt / 100), temp);
	if (!g_volt_enable_state || p_leakage < 0)
		p_leakage = 0;
#else
	p_leakage = 71;
#endif				/* ifdef MT_GPUFREQ_STATIC_PWR_READY2USE */

	p_total = p_dynamic + p_leakage;

	gpufreq_pr_debug("@%s:idx=%d,dymic= %d,leakage= %d,total= %d,temp= %d\n",
		__func__, idx, p_dynamic, p_leakage, p_total, temp);

	g_power_table[idx].gpufreq_power = p_total;
}

/*
 * VGPU slew rate calculation
 * false : falling rate
 * true : rising rate
 */
static unsigned int __calculate_vgpu_sfchg_rate(bool isRising)
{
	unsigned int sfchg_rate_vgpu;

	/* [MT6358] RG_BUCK_VGPU_SFCHG_RRATE and RG_BUCK_VGPU_SFCHG_FRATE
	 * Rising soft change rate
	 * Ref clock = 26MHz (0.038us)
	 * Step = ( code + 1 ) * 0.038 us
	 */

	if (isRising) {
		/* sfchg_rate_reg is 19, (19+1)*0.038 = 0.76us */
		sfchg_rate_vgpu = 1;
	} else {
		/* sfchg_rate_reg is 39, (39+1)*0.038 = 1.52us */
		sfchg_rate_vgpu = 2;
	}

	gpufreq_pr_debug("@%s: isRising = %d, sfchg_rate_vgpu = %d\n",
			 __func__, isRising, sfchg_rate_vgpu);

	return sfchg_rate_vgpu;
}

/*
 * VSRAM slew rate calculation
 * false : falling rate
 * true : rising rate
 */
static unsigned int __calculate_vsram_sfchg_rate(bool isRising)
{
	unsigned int sfchg_rate_vsram;

	/* [MT6358] RG_LDO_VSRAM_GPU_SFCHG_RRATE RG_LDO_VSRAM_GPU_SFCHG_FRATE
	 *    7'd4 : 0.19us
	 *    7'd8 : 0.34us
	 *    7'd11 : 0.46us
	 *    7'd17 : 0.69us
	 *    7'd23 : 0.92us
	 *    7'd25 : 1us
	 */

	/* sfchg_rate_reg is 7 for rising, (7+1)*0.038 = 0.304us */
	/* sfchg_rate_reg is 15 for falling, (15+1)*0.038 = 0.608us */
	sfchg_rate_vsram = 1;

	gpufreq_pr_debug("@%s: isRising = %d, sfchg_rate_vsram = %d\n",
			 __func__, isRising, sfchg_rate_vsram);

	return sfchg_rate_vsram;
}

/*
 * get post divider value
 * - VCO needs proper post divider value to get corresponding dds value
 *  to adjust PLL value.
 * - e.g: In Vinson, VCO range is 2.0GHz - 4.0GHz, required frequency is 900MHz,
 * so post divider could be 2(X), 4(3600/4), 8(X), 16(X).
 * - It may have special requiremt by DE in different efuse value
 * - e.g: In O_L_Y_M_P_U_S, efuse value(3'b001), VCO range is 1.5GHz - 3.8GHz,
 *  required frequency
 * range is 375MHz - 900MHz, It can only use post divider 4, no post divider 2.
 */
static enum g_post_divider_power_enum
__mt_gpufreq_get_post_divider_power(unsigned int freq, unsigned int efuse)
{
	/* [MT6768] but /MT8168 just 3 idx
	 *    VCO range: 1.5GHz - 3.8GHz by divider 1/2/4/8/16,
	 *    PLL range: 125MHz - 3.8GHz,
	 *    | VCO MAX | VCO MIN | POSTDIV | PLL OUT MAX | PLL OUT MIN |
	 *    |  3800   |  1500   |    1    |   3800MHz   |  1500MHz    | (X)
	 *    |  3800   |  1500   |    2    |   1900MHz   |   750MHz    | (X)
	 *    |  3800   |  1500   |    4    |   950MHz    |   375MHz    | (O)
	 *    |  3800   |  1500   |    8    |   475MHz    |   187.5MHz  | (O)
	 *    |  3800   |  2000   |   16    |   237.5MHz  |   125MHz    | (X)
	 */
	enum g_post_divider_power_enum post_divider_power = POST_DIV4;

	if (freq > POST_DIV_4_MAX_FREQ)
		post_divider_power = POST_DIV2;
	else if (freq < POST_DIV_4_MIN_FREQ)
		post_divider_power = POST_DIV8;

	if (g_cur_post_divider_power != post_divider_power) {
		g_parking = true;
		g_cur_post_divider_power = post_divider_power;
	}

	gpufreq_pr_debug("@%s:freq= %d,post_divider_power= %d,g_parking= %d\n",
		__func__, freq, post_divider_power, g_parking);

	return post_divider_power;
}

/*
 * get current frequency (KHZ)
 */
static unsigned int __mt_gpufreq_get_cur_freq(void)
{

	unsigned int freq_khz = 0;

	freq_khz = clk_get_rate(g_clk->clk_mux) / 1000;
	gpufreq_pr_debug("@%s: freq = %d KHz\n", __func__, freq_khz);

	return freq_khz;
}

/*
 * get current vsram voltage (mV * 100)
 */
static unsigned int __mt_gpufreq_get_cur_vsram_volt(void)
{
	unsigned int volt = 0;

	/* WARRNING: regulator_get_voltage prints uV */
#ifdef USE_STAND_ALONE_VSRAM
	volt = regulator_get_voltage(g_pmic->reg_vsram_gpu) / 10;

	gpufreq_pr_debug("@%s: volt = %d\n", __func__, volt);
#else
	gpufreq_pr_debug("@%s: volt = 80000 (FIXED VALUE)\n", __func__);
#endif

	return volt;
}

/*
 * get current voltage (mV * 100)
 */
static unsigned int __mt_gpufreq_get_cur_volt(void)
{
	unsigned int volt = 0;

	if (g_volt_enable_state) {
#ifdef USE_STAND_ALONE_VGPU
		/* WARRNING: regulator_get_voltage prints uV */
		volt = regulator_get_voltage(g_pmic->reg_vgpu) / 10;
#else
		volt = regulator_get_voltage(g_pmic->reg_vcore) / 10;
#endif
	}

	gpufreq_pr_debug("@%s: volt = %d\n", __func__, volt);

	return volt;
}

/*
 * get OPP table index by voltage (mV * 100)
 */
static int __mt_gpufreq_get_opp_idx_by_volt(unsigned int volt)
{
	int i = g_max_opp_idx_num - 1;

	while (i >= 0) {
		if (g_opp_table[i--].gpufreq_volt >= volt)
			goto EXIT;
	}

EXIT:
	return i + 1;
}

/*
 * calculate vsram_volt via given volt
 */
static unsigned int __mt_gpufreq_get_vsram_by_target_volt(unsigned int volt)
{
	unsigned int target_vsram;

	if (volt > g_fixed_vsram_volt_threshold)
		target_vsram = volt + 10000;
	else
		target_vsram = g_fixed_vsram_volt;

	return target_vsram;
}

/*
 * get limited frequency by limited power (mW)
 */
static unsigned int __mt_gpufreq_get_limited_freq_by_power(unsigned int
							   limited_power)
{
	int i;
	unsigned int limited_freq;

	limited_freq = g_power_table[g_max_opp_idx_num - 1].gpufreq_khz;

	for (i = g_segment_max_opp_idx; i < g_max_opp_idx_num; i++) {
		if (g_power_table[i].gpufreq_power <= limited_power) {
			limited_freq = g_power_table[i].gpufreq_khz;
			break;
		}
	}

	gpufreq_pr_debug("@%s: limited_freq = %d\n", __func__, limited_freq);

	return limited_freq;
}

#ifdef MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE
/* update OPP power table */
static void __mt_update_gpufreqs_power_table(void)
{
	int i;
	int temp = 0;
	unsigned int freq = 0;
	unsigned int volt = 0;

#ifdef CONFIG_THERMAL
	temp = get_immediate_gpu_wrap() / 1000;
#else
	temp = 40;
#endif				/* ifdef CONFIG_THERMAL */

	gpufreq_pr_debug("@%s: temp = %d\n", __func__, temp);

	mutex_lock(&mt_gpufreq_lock);

	if ((temp >= -20) && (temp <= 125)) {
		for (i = 0; i < g_max_opp_idx_num; i++) {
			freq = g_power_table[i].gpufreq_khz;
			volt = g_power_table[i].gpufreq_volt;

			__mt_gpufreq_calculate_power(i, freq, volt, temp);

			gpufreq_pr_debug("@%s: [%d] freq_khz= %d,volt= %d, pwr = %d\n",
			     __func__, i, g_power_table[i].gpufreq_khz,
			     g_power_table[i].gpufreq_volt,
			     g_power_table[i].gpufreq_power);
		}
	} else {
		gpufreq_pr_err("@%s:temp< -20 or temp> 125,NOT update pwr table!\n",
		     __func__);
	}

	mutex_unlock(&mt_gpufreq_lock);
}
#endif		/* ifdef MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE */

/* update OPP limited index for Thermal/Power/PBM protection */
static void __mt_gpufreq_update_max_limited_idx(void)
{
	int i = 0;
	unsigned int limited_idx = g_segment_max_opp_idx;

	/* Check lowest frequency index in all limitation */
	for (i = 0; i < NUMBER_OF_LIMITED_IDX; i++) {
		if (g_limited_idx_array[i] > limited_idx) {
			if (!g_limited_ignore_array[i])
				limited_idx = g_limited_idx_array[i];
		}
	}

	g_max_limited_idx = limited_idx;

	gpufreq_pr_debug("@%s: g_max_limited_idx = %d\n", __func__,
			 g_max_limited_idx);
}

#ifdef MT_GPUFREQ_BATT_OC_PROTECT
/*
 * limit OPP index for Over Currents (OC) protection
 */
static void __mt_gpufreq_batt_oc_protect(unsigned int limited_idx)
{
	mutex_lock(&mt_gpufreq_power_lock);

	gpufreq_pr_debug("@%s: limited_idx = %d\n", __func__, limited_idx);

	g_limited_idx_array[IDX_BATT_OC_LIMITED] = limited_idx;
	__mt_gpufreq_update_max_limited_idx();

	mutex_unlock(&mt_gpufreq_power_lock);
}
#endif				/* ifdef MT_GPUFREQ_BATT_OC_PROTECT */

#ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT
/*
 * limit OPP index for Battery Percentage protection
 */
static void __mt_gpufreq_batt_percent_protect(unsigned int limited_index)
{
	mutex_lock(&mt_gpufreq_power_lock);

	gpufreq_pr_debug("@%s: limited_index = %d\n", __func__, limited_index);

	g_limited_idx_array[IDX_BATT_PERCENT_LIMITED] = limited_index;
	__mt_gpufreq_update_max_limited_idx();

	mutex_unlock(&mt_gpufreq_power_lock);
}
#endif				/* ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT */

#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
/*
 * limit OPP index for Low Battery Volume protection
 */
static void __mt_gpufreq_low_batt_protect(unsigned int limited_index)
{
	mutex_lock(&mt_gpufreq_power_lock);

	gpufreq_pr_debug("@%s: limited_index = %d\n", __func__, limited_index);

	g_limited_idx_array[IDX_LOW_BATT_LIMITED] = limited_index;
	__mt_gpufreq_update_max_limited_idx();

	mutex_unlock(&mt_gpufreq_power_lock);
}
#endif				/* ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT */

/*
 * kick Power Budget Manager(PBM) when OPP changed
 */
static void __mt_gpufreq_kick_pbm(int enable)
{
	unsigned int power;
	unsigned int cur_freq;
	unsigned int cur_volt;
	unsigned int found = 0;
	int tmp_idx = -1;
	int i;

	cur_freq = __mt_gpufreq_get_cur_freq();
	cur_volt = __mt_gpufreq_get_cur_volt();

	if (enable) {
		for (i = 0; i < g_max_opp_idx_num; i++) {
			if (g_power_table[i].gpufreq_khz == cur_freq) {
		/* record idx since current voltage may not in DVFS table */
				tmp_idx = i;

				if (g_power_table[i].gpufreq_volt == cur_volt) {
					power = g_power_table[i].gpufreq_power;
					found = 1;
#ifdef MTK_PBM_READY
					kicker_pbm_by_gpu(true, power,
							  cur_volt / 100);
#endif
					gpufreq_pr_debug("@%s:rqq pwr= %d,cvlt= %d,cfreq= %d\n",
					     __func__, power, cur_volt * 10,
					     cur_freq);
					return;
				}
			}
		}

		if (!found) {
			gpufreq_pr_debug("@%s: tmp_idx = %d\n",
				__func__, tmp_idx);
			if (tmp_idx != -1 && tmp_idx < g_max_opp_idx_num) {
			/* use freq to found corresponding power budget */
				power = g_power_table[tmp_idx].gpufreq_power;
#ifdef MTK_PBM_READY
				kicker_pbm_by_gpu(true, power, cur_volt / 100);
#endif
			gpufreq_pr_debug("@%s: req pwr= %d,cv= %d uV, cfreq= %d KHz\n",
				__func__,
				power,
				cur_volt * 10,
				cur_freq);
			} else {
				gpufreq_pr_debug("@%s:not found,freq= %d KHz,volt= %d uV\n",
					__func__, cur_freq, cur_volt * 10);
			}
		}
	} else {
#ifdef MTK_PBM_READY
		kicker_pbm_by_gpu(false, 0, cur_volt / 100);
#endif
	}
}

/*
 * (default) OPP table initialization
 */
static void __mt_gpufreq_setup_opp_table(struct g_opp_table_info *freqs,
					 int num)
{
	unsigned int i = 0;

	g_opp_table = kzalloc((num) * sizeof(*freqs), GFP_KERNEL);
	g_opp_table_default = kzalloc((num) * sizeof(*freqs), GFP_KERNEL);

	if (g_opp_table == NULL || g_opp_table_default == NULL)
		return;

	for (i = 0; i < num; i++) {
		g_opp_table[i].gpufreq_khz = freqs[i].gpufreq_khz;
		g_opp_table[i].gpufreq_volt = freqs[i].gpufreq_volt;
		g_opp_table[i].gpufreq_vsram = freqs[i].gpufreq_vsram;

		g_opp_table_default[i].gpufreq_khz = freqs[i].gpufreq_khz;
		g_opp_table_default[i].gpufreq_volt = freqs[i].gpufreq_volt;
		g_opp_table_default[i].gpufreq_vsram = freqs[i].gpufreq_vsram;

		gpufreq_pr_debug("@%s: idx= %u, freq_khz= %u, volt= %u, vsram= %u\n",
			__func__,
			i,
			freqs[i].gpufreq_khz,
			freqs[i].gpufreq_volt,
			freqs[i].gpufreq_vsram);
	}

	/* setup OPP table by device ID */
	if (g_segment_id == MT8168_SEGMENT)
		g_segment_max_opp_idx = 6;
	else if (g_segment_id == MT6767_SEGMENT)
		g_segment_max_opp_idx = 14;
	else
		g_segment_max_opp_idx = 0;

	g_max_opp_idx_num = num;
	g_max_limited_idx = g_segment_max_opp_idx;
	g_DVFS_off_by_ptpod_idx = g_segment_max_opp_idx;

	g_ptpod_opp_idx_table = g_ptpod_opp_idx_table_segment;
	g_ptpod_opp_idx_num = ARRAY_SIZE(g_ptpod_opp_idx_table_segment);

	gpufreq_pr_debug("@%s:g_segment_max_opp_idx= %u,g_max_opp_idx_num= %u\n",
		__func__, g_segment_max_opp_idx, g_max_opp_idx_num);

	__mt_gpufreq_cal_sb_opp_index();
	__mt_gpufreq_setup_opp_power_table(num);
}

/*
 * OPP power table initialization
 */
static void __mt_gpufreq_setup_opp_power_table(int num)
{
	int i = 0;
	int temp = 0;

	g_power_table =
	    kzalloc((num) * sizeof(struct mt_gpufreq_power_table_info),
		    GFP_KERNEL);

	if (g_power_table == NULL)
		return;

#ifdef CONFIG_THERMAL
	temp = get_immediate_gpu_wrap() / 1000;
#else
	temp = 40;
#endif				/* ifdef CONFIG_THERMAL */

	gpufreq_pr_debug("@%s: temp = %d\n", __func__, temp);

	if ((temp < -20) || (temp > 125)) {
		gpufreq_pr_debug("@%s: temp < -20 or temp > 125!\n", __func__);
		temp = 65;
	}

	for (i = 0; i < num; i++) {
		g_power_table[i].gpufreq_khz = g_opp_table[i].gpufreq_khz;
		g_power_table[i].gpufreq_volt = g_opp_table[i].gpufreq_volt;

		__mt_gpufreq_calculate_power(i, g_power_table[i].gpufreq_khz,
					     g_power_table[i].gpufreq_volt,
					     temp);

		gpufreq_pr_debug("@%s: [%d], freq_khz = %u, volt = %u, power = %u\n",
			__func__, i, g_power_table[i].gpufreq_khz,
			g_power_table[i].gpufreq_volt,
			g_power_table[i].gpufreq_power);
	}

#ifdef CONFIG_THERMAL
	mtk_gpufreq_register(g_power_table, num);
#endif				/* ifdef CONFIG_THERMAL */
}

/*
 * I/O remap
 */
static void *__mt_gpufreq_of_ioremap(const char *node_name, int idx)
{
	struct device_node *node;

	node = of_find_compatible_node(NULL, NULL, node_name);

	if (node)
		return of_iomap(node, idx);

	return NULL;
}

/*
 * Set default OPP index at driver probe function
 */
static void __mt_gpufreq_set_initial(void)
{
	unsigned int cur_volt = 0;
	unsigned int cur_freq = 0;
	unsigned int cur_vsram_volt = 0;
	unsigned int cur_bringp_idx = 2;

	mutex_lock(&mt_gpufreq_lock);

	/* default OPP index */
	if (g_segment_id == MT8168_SEGMENT && g_segment_id == MT6767_SEGMENT)
		g_cur_opp_idx = g_segment_max_opp_idx;
	else
		g_cur_opp_idx = 0;

	gpufreq_pr_debug("@%s: initial opp index = %d\n",
		__func__, g_cur_opp_idx);

	cur_vsram_volt = __mt_gpufreq_get_cur_vsram_volt();
	cur_volt = __mt_gpufreq_get_cur_volt();
	cur_freq = __mt_gpufreq_get_cur_freq();

	__mt_gpufreq_set(cur_bringp_idx, g_cur_opp_idx,
			 cur_freq, g_opp_table[g_cur_opp_idx].gpufreq_khz,
			 cur_volt, g_opp_table[g_cur_opp_idx].gpufreq_volt,
			 cur_vsram_volt,
			 g_opp_table[g_cur_opp_idx].gpufreq_vsram);

	g_cur_opp_freq = g_opp_table[g_cur_opp_idx].gpufreq_khz;
	g_cur_opp_volt = g_opp_table[g_cur_opp_idx].gpufreq_volt;
	g_cur_opp_vsram_volt = g_opp_table[g_cur_opp_idx].gpufreq_vsram;

	mutex_unlock(&mt_gpufreq_lock);
}


static int __mt_gpufreq_init_pmic(struct platform_device *pdev)
{
	g_pmic = kzalloc(sizeof(struct g_pmic_info), GFP_KERNEL);
	if (g_pmic == NULL)
		return -1;

#ifdef USE_STAND_ALONE_VGPU
	g_pmic->reg_vgpu = regulator_get(&pdev->dev, "vgpu");
	if (IS_ERR(g_pmic->reg_vgpu)) {
		gpufreq_pr_err("@%s: cannot get VGPU\n", __func__);
		return PTR_ERR(g_pmic->reg_vgpu);
	}

	g_pmic->reg_vsram_gpu = regulator_get(&pdev->dev, "vsram_gpu");
	if (IS_ERR(g_pmic->reg_vsram_gpu)) {
		gpufreq_pr_err("@%s: cannot get VSRAM_GPU\n", __func__);
		return PTR_ERR(g_pmic->reg_vsram_gpu);
	}

	/* setup PMIC init value */
	g_vgpu_sfchg_rrate = __calculate_vgpu_sfchg_rate(true);
	g_vgpu_sfchg_frate = __calculate_vgpu_sfchg_rate(false);
	g_vsram_sfchg_rrate = __calculate_vsram_sfchg_rate(true);
	g_vsram_sfchg_frate = __calculate_vsram_sfchg_rate(false);

	/* set VSRAM_GPU */
	regulator_set_voltage(g_pmic->reg_vsram_gpu, VSRAM_GPU_MAX_VOLT * 10,
			      VSRAM_GPU_MAX_VOLT * 10 + 125);
	/* set VGPU */
	regulator_set_voltage(g_pmic->reg_vgpu, VGPU_MAX_VOLT * 10,
			      VGPU_MAX_VOLT * 10 + 125);

	/* enable bucks (VGPU && VSRAM_GPU) enforcement */
	if (regulator_enable(g_pmic->reg_vsram_gpu))
		gpufreq_pr_err("@%s: enable VSRAM_GPU failed\n", __func__);
	if (regulator_enable(g_pmic->reg_vgpu))
		gpufreq_pr_err("@%s: enable VGPU failed\n", __func__);
#else
#ifdef CONFIG_MTK_QOS_SUPPORT
	pm_qos_add_request(&g_pmic->pm_vgpu, PM_QOS_VCORE_OPP, VCORE_OPP_0);
	pm_qos_add_request(&g_pmic->pm_v_emi_gpu, PM_QOS_EMI_OPP, DDR_OPP_0);

#endif
	g_pmic->reg_vcore = regulator_get(&pdev->dev, "vcore");
	if (IS_ERR(g_pmic->reg_vcore)) {
		gpufreq_pr_err("@%s: cannot get VCORE\n", __func__);
		return PTR_ERR(g_pmic->reg_vcore);
	}
#endif

	g_volt_enable_state = true;

	return 0;
}


static int __mt_gpufreq_init_clk(struct platform_device *pdev)
{
	g_clk = kzalloc(sizeof(struct g_clk_info), GFP_KERNEL);
	if (g_clk == NULL)
		return -ENOENT;

	g_clk->clk_mux = devm_clk_get(&pdev->dev, "clk_mux");
	if (IS_ERR(g_clk->clk_mux)) {
		gpufreq_pr_err("@%s: cannot get clk_mux\n", __func__);
		return PTR_ERR(g_clk->clk_mux);
	}

	g_clk->clk_pll_src = devm_clk_get(&pdev->dev, "clk_pll_src");
	if (IS_ERR(g_clk->clk_pll_src)) {
		gpufreq_pr_err("@%s: cannot get clk_pll_src\n", __func__);
		return PTR_ERR(g_clk->clk_pll_src);
	}

	g_clk->clk_main_parent = devm_clk_get(&pdev->dev, "clk_main_parent");
	if (IS_ERR(g_clk->clk_main_parent)) {
		gpufreq_pr_err("@%s: cannot get clk_main_parent\n", __func__);
		return PTR_ERR(g_clk->clk_main_parent);
	}

	g_clk->clk_sub_parent = devm_clk_get(&pdev->dev, "clk_sub_parent");
	if (IS_ERR(g_clk->clk_sub_parent)) {
		gpufreq_pr_err("@%s: cannot get clk_sub_parent\n", __func__);
		return PTR_ERR(g_clk->clk_sub_parent);
	}

	g_clk->subsys_mfg_cg = devm_clk_get(&pdev->dev, "subsys_mfg_cg");
	if (IS_ERR(g_clk->subsys_mfg_cg)) {
		gpufreq_pr_err("@%s: cannot get subsys_mfg_cg\n", __func__);
		return PTR_ERR(g_clk->subsys_mfg_cg);
	}

	g_clk->subsys_mfg_mbist_diag =
		devm_clk_get(&pdev->dev, "subsys_mfg_mbist_diag");
	if (IS_ERR(g_clk->subsys_mfg_mbist_diag)) {
		gpufreq_pr_err("@%s: cannot get mfg_mbist\n", __func__);
		return PTR_ERR(g_clk->subsys_mfg_mbist_diag);
	}

	pr_info("[GPU/DVFS]@%s:main_parent 0x%p,mfg_cg 0x%p,mbist_diag0x%p\n",
		__func__, g_clk->clk_main_parent, g_clk->subsys_mfg_cg,
		g_clk->subsys_mfg_mbist_diag);

	return 0;
}

static void __mt_gpufreq_init_efuse(void)
{
	gpufreq_pr_info("@%s: g_efuse_id = 0x%08X, g_segment_id = %d\n",
		__func__, g_efuse_id, g_segment_id);
}

static void __mt_gpufreq_init_others(void)
{
	int i;

#ifdef MT_GPUFREQ_STATIC_PWR_READY2USE
	/* Initial leackage power usage */
	mt_spower_init();
#endif				/* ifdef MT_GPUFREQ_STATIC_PWR_READY2USE */

#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
	g_low_batt_limited_idx_lvl_0 = g_segment_max_opp_idx;
	for (i = g_segment_max_opp_idx; i < g_max_opp_idx_num; i++) {
		if (g_opp_table[i].gpufreq_khz <=
		    MT_GPUFREQ_LOW_BATT_VOLT_LIMIT_FREQ) {
			g_low_batt_limited_idx_lvl_2 = i;
			break;
		}
	}
	register_low_battery_notify(&mt_gpufreq_low_batt_callback,
				    LOW_BATTERY_PRIO_GPU);
#endif				/* ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT */

#ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT
	g_batt_percent_limited_idx_lvl_0 = g_segment_max_opp_idx;
	g_batt_percent_limited_idx_lvl_1 = g_segment_max_opp_idx;
	for (i = g_segment_max_opp_idx; i < g_max_opp_idx_num; i++) {
		if (g_opp_table[i].gpufreq_khz ==
		    MT_GPUFREQ_BATT_PERCENT_LIMIT_FREQ) {
			g_batt_percent_limited_idx_lvl_1 = i;
			break;
		}
	}
	register_battery_percent_notify(&mt_gpufreq_batt_percent_callback,
					BATTERY_PERCENT_PRIO_GPU);
#endif				/* ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT */

#ifdef MT_GPUFREQ_BATT_OC_PROTECT
	g_batt_oc_limited_idx_lvl_0 = g_segment_max_opp_idx;
	for (i = g_segment_max_opp_idx; i < g_max_opp_idx_num; i++) {
		if (g_opp_table[i].gpufreq_khz
			<= MT_GPUFREQ_BATT_OC_LIMIT_FREQ) {
			g_batt_oc_limited_idx_lvl_1 = i;
			break;
		}
	}
	register_battery_oc_notify(&mt_gpufreq_batt_oc_callback,
				   BATTERY_OC_PRIO_GPU);
#endif				/* ifdef MT_GPUFREQ_BATT_OC_PROTECT */

	for (i = 0; i < NUMBER_OF_LIMITED_IDX; i++)
		g_limited_idx_array[i] = g_segment_max_opp_idx;
}

/*
 * gpufreq driver probe
 */
static int __mt_gpufreq_pdrv_probe(struct platform_device *pdev)
{
	struct device_node *node;
	int ret;

	g_opp_stress_test_state = false;
	g_keep_opp_freq_state = false;
	g_dvfs_enable = true;

	node = of_find_matching_node(NULL, g_gpufreq_of_match);
	if (!node)
		gpufreq_pr_err("@%s: find GPU node failed\n", __func__);

	/* alloc PMIC regulator 8168 no need */
	ret = __mt_gpufreq_init_pmic(pdev);
	if (ret)
		return ret;

	/* init clock source and mtcmos */
	ret = __mt_gpufreq_init_clk(pdev);
	if (ret)
		return ret;

	/* check efuse_id and set the corresponding segment_id */
	__mt_gpufreq_init_efuse();

	/* init opp table */
	__mt_gpufreq_setup_opp_table(g_opp_table_segment,
				     ARRAY_SIZE(g_opp_table_segment));

	g_fixed_vsram_volt = SEG_GPU_DVFS_VSRAM2;
	g_fixed_vsram_volt_threshold = SEG_GPU_DVFS_VOLT0;

	/* init APMIXED base address */
	g_apmixed_base =
		__mt_gpufreq_of_ioremap("mediatek,mt8168-apmixedsys", 0);
	if (!g_apmixed_base) {
		gpufreq_pr_err("@%s: APMIXED iomap failed", __func__);
		return -ENOENT;
	}

	/* setup initial frequency */
	__mt_gpufreq_set_initial();
	gpufreq_pr_info("@%s: freq: %d KHz, vgpu: %d uV, vsram_gpu: %d uV\n",
			__func__, mt_gpufreq_get_cur_freq(),
			__mt_gpufreq_get_cur_volt() * 10,
			__mt_gpufreq_get_cur_vsram_volt() * 10);

	gpufreq_pr_info
	    ("@%s: g_cur_freq = %d, g_cur_volt = %d, g_cur_vsram_volt = %d\n",
	     __func__, g_cur_opp_freq, g_cur_opp_volt, g_cur_opp_vsram_volt);

	gpufreq_pr_info("@%s: g_cur_opp_idx = %d\n", __func__, g_cur_opp_idx);
	g_gpufreq_pdev_p = pdev;
	pm_runtime_enable(&pdev->dev);

	__mt_gpufreq_init_others();

	return 0;
}

static int __mt_gpufreq_pdrv_remove(struct platform_device *pdev)
{
	pm_runtime_disable(&pdev->dev);
	return 0;
}


/*
 * register the gpufreq driver
 */
static int __init __mt_gpufreq_init(void)
{
	int ret = 0;

#ifdef MTK_GPU_BRING_UP
	/* Skip driver init in bring up stage */
	return 0;
#endif

#ifdef MTK_GPU_LOG
	mtk_gpu_log_init();
#endif

	gpufreq_pr_debug("@%s: start to initialize gpufreq driver\n", __func__);

#ifdef CONFIG_PROC_FS
	if (__mt_gpufreq_create_procfs())
		goto out;
#endif				/* ifdef CONFIG_PROC_FS */

	/* register platform driver */
	ret = platform_driver_register(&g_gpufreq_pdrv);
	if (ret)
		gpufreq_pr_err("@%s: fail to register gpufreq driver\n",
			       __func__);

out:
#ifdef MT_GPUFREQ_SRAM_DEBUG
	aee_rr_rec_gpu_dvfs_vgpu(0xFF);
	aee_rr_rec_gpu_dvfs_oppidx(0xFF);
	aee_rr_rec_gpu_dvfs_status(0x0);
#endif
	gpufreq_pr_debug("@%s: end to initialize gpufreq driver\n", __func__);

	return ret;
}

/*
 * unregister the gpufreq driver
 */
static void __exit __mt_gpufreq_exit(void)
{
	platform_driver_unregister(&g_gpufreq_pdrv);
}

module_init(__mt_gpufreq_init);
module_exit(__mt_gpufreq_exit);


MODULE_DEVICE_TABLE(of, g_gpufreq_of_match);
MODULE_DESCRIPTION("MediaTek GPU-DVFS driver");
MODULE_LICENSE("GPL");
