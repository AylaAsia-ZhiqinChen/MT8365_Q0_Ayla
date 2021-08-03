/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef __MTK_CM_MGR_PLATFORM_DATA_H__
#define __MTK_CM_MGR_PLATFORM_DATA_H__

#ifndef PROC_FOPS_RW
#define PROC_FOPS_RW(name)					\
	static int name ## _proc_open(struct inode *inode,	\
		struct file *file)				\
	{							\
		return single_open(file, name ## _proc_show,	\
			PDE_DATA(inode));			\
	}							\
	static const struct file_operations name ## _proc_fops = {	\
		.owner		  = THIS_MODULE,			\
		.open		   = name ## _proc_open,		\
		.read		   = seq_read,				\
		.llseek		 = seq_lseek,				\
		.release		= single_release,		\
		.write		  = name ## _proc_write,		\
	}
#endif /* PROC_FOPS_RW */

#ifndef PROC_FOPS_RO
#define PROC_FOPS_RO(name)					\
	static int name ## _proc_open(struct inode *inode,	\
		struct file *file)				\
	{							\
		return single_open(file, name ## _proc_show,	\
			PDE_DATA(inode));			\
	}							\
	static const struct file_operations name ## _proc_fops = {	\
		.owner		  = THIS_MODULE,			\
		.open		   = name ## _proc_open,		\
		.read		   = seq_read,				\
		.llseek		 = seq_lseek,				\
		.release		= single_release,		\
	}
#endif /* PROC_FOPS_RO */

#ifndef PROC_ENTRY
#define PROC_ENTRY(name)	{__stringify(name), &name ## _proc_fops}
#endif /* PROC_ENTRY */

int light_load_cps = 1000;
static int cm_mgr_loop_count;
static int cm_mgr_dram_level;
static int cm_mgr_loop;
static int total_bw_value;
int cm_mgr_emi_demand_check;
int cpu_power_ratio_up[CM_MGR_EMI_OPP] = {100, 100};
int cpu_power_ratio_down[CM_MGR_EMI_OPP] = {100, 100};
int vcore_power_ratio_up[CM_MGR_EMI_OPP] = {100, 100};
int vcore_power_ratio_down[CM_MGR_EMI_OPP] = {100, 100};
int debounce_times_up_adb[CM_MGR_EMI_OPP] = {0, 3};
int debounce_times_down_adb[CM_MGR_EMI_OPP] = {0, 3};
int debounce_times_reset_adb;
int debounce_times_perf_down = 50;
int debounce_times_perf_force_down = 100;
static int update;
static int emi_latency = 1;
static int update_v2f_table = 1;
static int cm_mgr_opp_enable = 1;
int cm_mgr_enable = 1;
#if defined(CONFIG_MTK_TINYSYS_SSPM_SUPPORT) && defined(USE_CM_MGR_AT_SSPM)
int cm_mgr_sspm_enable = 1;
#endif /* CONFIG_MTK_TINYSYS_SSPM_SUPPORT */
#ifdef USE_TIMER_CHECK
int cm_mgr_timer_enable = 1;
#endif /* USE_TIMER_CHECK */
int cm_mgr_ratio_timer_enable;
int cm_mgr_disable_fb = 1;
int cm_mgr_blank_status;
int cm_mgr_perf_enable = 1;
int cm_mgr_perf_timer_enable;
int cm_mgr_perf_force_enable;
int cm_mgr_loading_level;
int cm_mgr_loading_enable;

/* LP4 */
static int vcore_power_gain_0[][VCORE_ARRAY_SIZE] = {
	{19,  96},
	{236, 321},
	{351, 415},
	{411, 467},
	{454, 512},
	{468, 536},
	{472, 555},
	{475, 575},
	{477, 591},
	{478, 606},
	{480, 622},
	{482, 638},
	{484, 653},
	{484, 669},
	{484, 685},
	{484, 700},
	{484, 716},
	{484, 732},
	{484, 747},
	{484, 763},
};

/* LP3 */
static int vcore_power_gain_1[][VCORE_ARRAY_SIZE] = {
	{36, 176},
	{109, 460},
	{123, 548},
	{120, 586},
	{127, 614},
	{144, 648},
	{154, 681},
	{168, 704},
	{179, 727},
	{179, 751},
	{179, 774},
	{179, 797},
};

#define VCORE_POWER_ARRAY_SIZE(name) \
	(sizeof(vcore_power_gain_##name) / \
	 sizeof(unsigned int) / \
	 VCORE_ARRAY_SIZE)

#define VCORE_POWER_GAIN_PTR(name) \
	(&vcore_power_gain_##name[0][0])

static int vcore_power_array_size(int idx)
{
	switch (idx) {
	case 0:
		return VCORE_POWER_ARRAY_SIZE(0);
	case 1:
		return VCORE_POWER_ARRAY_SIZE(1);
	}

	pr_debug("#@# %s(%d) warning value %d\n", __func__, __LINE__, idx);
	return 0;
};

static int *vcore_power_gain_ptr(int idx)
{
	switch (idx) {
	case 0:
		return VCORE_POWER_GAIN_PTR(0);
	case 1:
		return VCORE_POWER_GAIN_PTR(1);
	}

	pr_debug("#@# %s(%d) warning value %d\n", __func__, __LINE__, idx);
	return NULL;
};

static int *vcore_power_gain = VCORE_POWER_GAIN_PTR(0);
#define vcore_power_gain(p, i, j) (*(p + (i) * VCORE_ARRAY_SIZE + (j)))

static unsigned int _v2f_all[][CM_MGR_CPU_CLUSTER] = {
	{280},
	{236},
	{180},
	{151},
	{125},
	{113},
	{102},
	{93},
	{84},
	{76},
	{68},
	{59},
	{50},
	{42},
	{35},
	{29},
};

static unsigned int cpu_power_gain_UpLow0[][CM_MGR_CPU_ARRAY_SIZE] = {
	{3, 2},
	{6, 4},
	{99, 7},
	{97, 94},
	{95, 91},
	{92, 88},
	{90, 85},
	{87, 82},
	{85, 78},
	{83, 75},
	{80, 72},
	{78, 69},
	{76, 66},
	{73, 63},
	{71, 60},
	{69, 57},
	{66, 54},
	{64, 51},
	{61, 47},
	{59, 44},
};

static unsigned int cpu_power_gain_DownLow0[][CM_MGR_CPU_ARRAY_SIZE] = {
	{4, 3},
	{113, 6},
	{112, 107},
	{110, 104},
	{109, 101},
	{108, 99},
	{106, 96},
	{105, 93},
	{103, 90},
	{102, 87},
	{101, 85},
	{99, 82},
	{98, 79},
	{97, 76},
	{95, 73},
	{94, 70},
	{93, 68},
	{91, 65},
	{90, 62},
	{89, 59},
};

static unsigned int cpu_power_gain_UpHigh0[][CM_MGR_CPU_ARRAY_SIZE] = {
	{3, 2},
	{6, 4},
	{9, 7},
	{12, 9},
	{127, 11},
	{123, 13},
	{118, 113},
	{114, 108},
	{109, 103},
	{105, 97},
	{100, 92},
	{96, 87},
	{91, 81},
	{86, 76},
	{82, 71},
	{77, 66},
	{73, 60},
	{68, 55},
	{64, 50},
	{59, 44},
};

static unsigned int cpu_power_gain_DownHigh0[][CM_MGR_CPU_ARRAY_SIZE] = {
	{4, 3},
	{9, 6},
	{13, 9},
	{149, 12},
	{145, 138},
	{141, 133},
	{138, 127},
	{134, 122},
	{130, 117},
	{126, 112},
	{123, 106},
	{119, 101},
	{115, 96},
	{111, 91},
	{108, 85},
	{104, 80},
	{100, 75},
	{96, 70},
	{92, 64},
	{89, 59},
};

static unsigned int cpu_power_gain_UpLow1[][CM_MGR_CPU_ARRAY_SIZE] = {
	{3, 2},
	{6, 4},
	{99, 7},
	{97, 94},
	{95, 91},
	{92, 88},
	{90, 85},
	{87, 82},
	{85, 78},
	{83, 75},
	{80, 72},
	{78, 69},
	{76, 66},
	{73, 63},
	{71, 60},
	{69, 57},
	{66, 54},
	{64, 51},
	{61, 47},
	{59, 44},
};

static unsigned int cpu_power_gain_DownLow1[][CM_MGR_CPU_ARRAY_SIZE] = {
	{4, 3},
	{113, 6},
	{112, 107},
	{110, 104},
	{109, 101},
	{108, 99},
	{106, 96},
	{105, 93},
	{103, 90},
	{102, 87},
	{101, 85},
	{99, 82},
	{98, 79},
	{97, 76},
	{95, 73},
	{94, 70},
	{93, 68},
	{91, 65},
	{90, 62},
	{89, 59},
};

static unsigned int cpu_power_gain_UpHigh1[][CM_MGR_CPU_ARRAY_SIZE] = {
	{1, 1},
	{2, 3},
	{3, 4},
	{4, 6},
	{4, 7},
	{5, 9},
	{6, 10},
	{7, 12},
	{8, 13},
	{9, 90},
	{10, 84},
	{11, 78},
	{11, 72},
	{12, 66},
	{13, 60},
	{44, 54},
	{38, 48},
	{31, 42},
	{24, 36},
	{18, 30},
};

static unsigned int cpu_power_gain_DownHigh1[][CM_MGR_CPU_ARRAY_SIZE] = {
	{1, 2},
	{2, 4},
	{3, 5},
	{4, 7},
	{5, 9},
	{6, 11},
	{7, 12},
	{8, 113},
	{9, 106},
	{10, 100},
	{11, 93},
	{12, 87},
	{13, 81},
	{14, 74},
	{56, 68},
	{49, 61},
	{41, 55},
	{34, 48},
	{27, 42},
	{20, 36},
};

#define cpu_power_gain(p, i, j) (*(p + (i) * CM_MGR_CPU_ARRAY_SIZE + (j)))
#define CPU_POWER_GAIN(a, b, c) \
	(&cpu_power_gain_##a##b##c[0][0])

static unsigned int *_cpu_power_gain_ptr(int isUP, int isLow, int idx)
{
	switch (isUP) {
	case 0:
		switch (isLow) {
		case 0:
			switch (idx) {
			case 0:
				return CPU_POWER_GAIN(Down, High, 0);
			case 1:
				return CPU_POWER_GAIN(Down, High, 1);
			}
			break;
		case 1:
			switch (idx) {
			case 0:
				return CPU_POWER_GAIN(Down, Low, 0);
			case 1:
				return CPU_POWER_GAIN(Down, Low, 1);
			}
			break;
		}
		break;
	case 1:
		switch (isLow) {
		case 0:
			switch (idx) {
			case 0:
				return CPU_POWER_GAIN(Up, High, 0);
			case 1:
				return CPU_POWER_GAIN(Up, High, 1);
			}
			break;
		case 1:
			switch (idx) {
			case 0:
				return CPU_POWER_GAIN(Up, Low, 0);
			case 1:
				return CPU_POWER_GAIN(Up, Low, 1);
			}
			break;
		}
		break;
	}

	pr_debug("#@# %s(%d) warning value %d\n", __func__, __LINE__, idx);
	return NULL;
};

static unsigned int *cpu_power_gain_up = CPU_POWER_GAIN(Up, High, 0);
static unsigned int *cpu_power_gain_down = CPU_POWER_GAIN(Down, High, 0);

#include <mt-plat/mtk_chip.h>

static void cpu_power_gain_ptr(int opp, int tbl, int cluster)
{
#if defined(CONFIG_MACH_MT8168)
	if (opp < CM_MGR_LOWER_OPP) {
#else
	if (opp >= CM_MGR_LOWER_OPP) {
#endif
		switch (tbl) {
		case 0:
			cpu_power_gain_up =
				CPU_POWER_GAIN(Up, Low, 0);
			cpu_power_gain_down =
				CPU_POWER_GAIN(Down, Low, 0);
			break;
		case 1:
			cpu_power_gain_up =
				CPU_POWER_GAIN(Up, Low, 1);
			cpu_power_gain_down =
				CPU_POWER_GAIN(Down, Low, 1);
			break;
		}
	} else {
		switch (tbl) {
		case 0:
			cpu_power_gain_up =
				CPU_POWER_GAIN(Up, High, 0);
			cpu_power_gain_down =
				CPU_POWER_GAIN(Down, High, 0);
			break;
		case 1:
			cpu_power_gain_up =
				CPU_POWER_GAIN(Up, High, 1);
			cpu_power_gain_down =
				CPU_POWER_GAIN(Down, High, 1);
			break;
		}
	}
}

int cpu_power_gain_opp(int bw, int is_up, int opp, int ratio_idx, int idx)
{
	cpu_power_gain_ptr(opp, cm_mgr_get_idx(), idx % CM_MGR_CPU_CLUSTER);

	if (is_up)
		return cpu_power_gain(cpu_power_gain_up, ratio_idx, idx);
	else
		return cpu_power_gain(cpu_power_gain_down, ratio_idx, idx);
}

#endif	/* __MTK_CM_MGR_PLATFORM_DATA_H__ */
