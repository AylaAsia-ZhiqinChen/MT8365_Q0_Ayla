/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef MT_MT8168_CPUFREQ_H
#define MT_MT8168_CPUFREQ_H
#include "mtk_power_throttle.h"

enum mt_cpu_dvfs_id {
	MT_CPU_DVFS_L,
	NR_MT_CPU_DVFS,
};

enum cpu_level {
	CPU_LEVEL_0 = 0,
	CPU_LEVEL_1,
	NUM_CPU_LEVEL,
};

struct mtk_cpu_dvfs {
	const char *name;
	const enum mt_cpu_dvfs_id id;
	unsigned int cpu_id;	                /* for cpufreq */
	unsigned int cpu_level;

	/* opp (freq) table */
	struct mt_cpu_freq_info *opp_tbl;       /* OPP table */
	int nr_opp_tbl;                         /* size for OPP table */
	int idx_opp_tbl;                        /* current OPP idx */
	int idx_normal_max_opp;                 /* idx for normal max OPP */
};

#define cpu_dvfs_get_freq_by_idx(p, idx) (p->opp_tbl[idx].cpufreq_khz)
#define cpu_dvfs_is_available(p) (p->opp_tbl)
#define cpu_dvfs_get_volt_by_idx(p, idx) (p->opp_tbl[idx].cpufreq_volt)
#define cpu_dvfs_get_cur_freq(p) (p->opp_tbl[p->idx_opp_tbl].cpufreq_khz)
extern struct mtk_cpu_dvfs *id_to_cpu_dvfs(enum mt_cpu_dvfs_id id);
extern u32 get_devinfo_with_index(u32 index);
unsigned int _mt_cpufreq_get_cpu_level(void);

#endif /*MT_MT8168_CPUFREQ_H*/
