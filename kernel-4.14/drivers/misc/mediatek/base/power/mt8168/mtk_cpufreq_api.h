/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef __MTK_CPUFREQ_API_H__
#define __MTK_CPUFREQ_API_H__

#include <mt8168-cpufreq.h>

int search_available_freq_idx(struct mtk_cpu_dvfs *p, unsigned int target_khz,
	unsigned int relation);
unsigned int mt_cpufreq_find_close_freq(unsigned int cluster_id,
	unsigned int freq);
unsigned int mt_cpufreq_get_cur_freq(enum mt_cpu_dvfs_id id);
/* mt_cpufreq_get_cur_freq = mt_cpufreq_get_cur_phy_freq_no_lock */
unsigned int mt_cpufreq_get_cur_freq_idx(enum mt_cpu_dvfs_id id);
unsigned int mt_cpufreq_get_freq_by_idx(enum mt_cpu_dvfs_id id, int idx);
unsigned int mt_cpufreq_get_volt_by_idx(enum mt_cpu_dvfs_id id, int idx);
unsigned int mt_cpufreq_get_cpu_level(void);
unsigned int mt_cpufreq_get_cur_cci_freq_idx(void);

#endif	/* __MTK_CPUFREQ_API_H__ */
