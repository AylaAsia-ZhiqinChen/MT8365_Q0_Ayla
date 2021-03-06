/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef _MT_GPUFREQ_H
#define _MT_GPUFREQ_H

#include <linux/module.h>
#include <linux/clk.h>

#define MAX_VCO_VALUE	4000000
#define MIN_VCO_VALUE	2000000

#define DIV4_MAX_FREQ	994500
#define DIV4_MIN_FREQ	500500
#define DIV8_MAX_FREQ   397800
#define DIV8_MIN_FREQ   250000
#define DIV16_MAX_FREQ  248625
#define DIV16_MIN_FREQ  125215

#define TO_MHz_HEAD    100
#define TO_MHz_TAIL    10
#define ROUNDING_VALUE 5
#define DDS_SHIFT      14
#define POST_DIV_SHIFT 28
#define POST_DIV_MASK  0x70000000
#define GPUPLL_FIN     26

enum post_div_order_enum {
	POST_DIV2 = 1,
	POST_DIV4,
	POST_DIV8,
	POST_DIV16,
};

struct mt_gpufreq_table_info {
	unsigned int freq;
	unsigned int volt;
	unsigned int idx;
};

struct mt_gpufreq_power_table_info {
	unsigned int freq;
	unsigned int volt;
	unsigned int power;
};

struct mt_gpufreq_clk_t {
	/* main clock for mfg setting */
	struct clk *clk_mux;
	/* substitution clock for mfg transient mux setting */
	struct clk *clk_main_parent;
	/* substitution clock for mfg transient parent setting */
	struct clk *clk_sub_parent;
};

struct mt_gpufreq_pmic_t {
	struct regulator *reg_vgpu;		/* vgpu regulator */
};


/* Extern function */
extern int mt_gpufreq_state_set(int enabled);
extern void mt_gpufreq_thermal_protect(unsigned int limited_power);
extern unsigned int mt_gpufreq_get_cur_freq_index(void);
extern unsigned int mt_gpufreq_get_cur_freq(void);
extern unsigned int mt_gpufreq_get_cur_volt(void);
extern unsigned int mt_gpufreq_get_dvfs_table_num(void);
extern unsigned int mt_gpufreq_target(unsigned int idx);
extern unsigned int mt_gpufreq_voltage_enable_set(unsigned int enable);
extern unsigned int mt_gpufreq_update_volt(
	unsigned int pmic_volt[], unsigned int array_size);
extern unsigned int mt_gpufreq_get_freq_by_idx(unsigned int idx);
extern unsigned int mt_gpufreq_get_volt_by_idx(unsigned int idx);
extern void mt_gpufreq_thermal_protect(unsigned int limited_power);
extern void mt_gpufreq_restore_default_volt(void);
extern void mt_gpufreq_enable_by_ptpod(void);
extern void mt_gpufreq_disable_by_ptpod(void);
extern unsigned int mt_gpufreq_get_max_power(void);
extern unsigned int mt_gpufreq_get_min_power(void);
extern unsigned int mt_gpufreq_get_thermal_limit_index(void);
extern unsigned int mt_gpufreq_get_thermal_limit_freq(void);
extern void mt_gpufreq_set_power_limit_by_pbm(unsigned int limited_power);

extern unsigned int mt_get_mfgclk_freq(void);	/* Freq Meter API */
extern u32 get_devinfo_with_index(u32 index);
extern int mt_gpufreq_fan53555_init(void);
extern void aee_rr_rec_gpu_dvfs_vgpu(u8 val);
extern void aee_rr_rec_gpu_dvfs_oppidx(u8 val);
extern void aee_rr_rec_gpu_dvfs_status(u8 val);
extern u8 aee_rr_curr_gpu_dvfs_status(void);

/* Power limit notification */
typedef void (*gpufreq_power_limit_notify)(unsigned int);
extern void mt_gpufreq_power_limit_notify_registerCB(
	gpufreq_power_limit_notify pCB);

/* Input boost notification */
typedef void (*gpufreq_input_boost_notify)(unsigned int);
extern void mt_gpufreq_input_boost_notify_registerCB(
	gpufreq_input_boost_notify pCB);

/* Update voltage notification */
typedef void (*gpufreq_ptpod_update_notify)(void);
extern void mt_gpufreq_update_volt_registerCB(gpufreq_ptpod_update_notify pCB);

/* Profiling purpose */
typedef void (*sampler_func)(unsigned int);
extern void mt_gpufreq_setfreq_registerCB(sampler_func pCB);
extern void mt_gpufreq_setvolt_registerCB(sampler_func pCB);

extern void switch_mfg_clk(int src);

/* PTPOD enable/disable GPU power doamin */
typedef void (*gpufreq_mfgclock_notify)(void);
extern void mt_gpufreq_mfgclock_notify_registerCB(
	gpufreq_mfgclock_notify pEnableCB,
	gpufreq_mfgclock_notify pDisableCB);
extern bool mt_gpucore_ready(void);

#ifdef MTK_GPU_SPM
void mtk_gpu_spm_fix_by_idx(unsigned int idx);
void mtk_gpu_spm_reset_fix(void);
void mtk_gpu_spm_pause(void);
void mtk_gpu_spm_resume(void);
#endif

#endif
