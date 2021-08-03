/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef MT_POWERTHROTTLE_H
#define MT_POWERTHROTTLE_H

struct mt_cpu_freq_info {
	unsigned int cpufreq_khz;
	unsigned int cpufreq_volt;  /* mv * 1000 */
	const unsigned int cpufreq_volt_org;    /* mv * 1000 */
};

#define OP(khz, volt) {            \
	.cpufreq_khz = khz,             \
	.cpufreq_volt = volt,           \
	.cpufreq_volt_org = volt,       \
}

extern struct mt_cpu_freq_info opp_tbl_default[];
extern int setup_power_table_tk(void);
extern void dump_power_table(void);
extern void mt_cpufreq_thermal_protect(unsigned int limited_power);
#endif

