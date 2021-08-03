/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef __MTK_CM_MGR_PLATFORM_H__
#define __MTK_CM_MGR_PLATFORM_H__

#ifdef CONFIG_MTK_DRAMC
#include <mtk_dramc.h>
#endif /* CONFIG_MTK_DRAMC */

#ifdef CONFIG_PM_DEVFREQ
#include "helio-dvfsrc-opp-mt8168.h"
#include "helio-dvfsrc-mt8168.h"
#include "helio-dvfsrc-v4/helio-dvfsrc-opp.h"
#include "helio-dvfsrc-v4/helio-dvfsrc.h"
#endif

#define PER_CPU_STALL_RATIO
#define LIGHT_LOAD
/* #define USE_AVG_PMU */
#define DEBUG_CM_MGR
#define USE_TIMER_CHECK
/* #define USE_IDLE_NOTIFY */
#define USE_NEW_CPU_OPP
/* #define USE_CM_MGR_AT_SSPM */
#define USE_SINGLE_CLUSTER
/* #define USE_HOTPLUG_NOTIFY */
#define CM_EMI_API

#define CM_MGR_EMI_OPP	2
#define CM_MGR_LOWER_OPP 8
#define CM_MGR_CPU_CLUSTER 1
#define CM_MGR_CPU_COUNT 4
#define CM_MGR_CPU_LIMIT 4
#define CM_CPUFREQ_MAX_OPP  ((2 * CM_MGR_LOWER_OPP) - 1)

#define CLUSTER0_MASK   0x0f
#define CLUSTER1_MASK   0xf0

#define VCORE_ARRAY_SIZE CM_MGR_EMI_OPP
#define CM_MGR_CPU_ARRAY_SIZE (CM_MGR_CPU_CLUSTER * CM_MGR_EMI_OPP)
#define RATIO_COUNT (100 / 5 - 1)
#define IS_UP 1
#define IS_DOWN 0
#define USE_TIMER_CHECK_TIME msecs_to_jiffies(100)
#define CM_MGR_INIT_DELAY_MS 1
#define CM_DRAM_EMI_BW_OFFSET (750)

enum {
	CM_MGR_LP4X_2CH_3200 = 0,
	CM_MGR_LP3_1CH_1866,
	CM_MGR_MAX,
};

extern void __iomem *mcucfg_mp0_counter_base;
extern void __iomem *MT8168_EMI_BASE;
extern void __iomem *mt_cen_emi_base_get(void);
extern unsigned int cpu_power_up_array[CM_MGR_CPU_CLUSTER];
extern unsigned int cpu_power_down_array[CM_MGR_CPU_CLUSTER];
extern unsigned int cpu_power_up[CM_MGR_CPU_CLUSTER];
extern unsigned int cpu_power_down[CM_MGR_CPU_CLUSTER];
extern unsigned int v2f[CM_MGR_CPU_CLUSTER];
extern int vcore_power_up;
extern int vcore_power_down;
extern int cpu_opp_cur[CM_MGR_CPU_CLUSTER];
extern int ratio_max[CM_MGR_CPU_CLUSTER];
extern int ratio[CM_MGR_CPU_COUNT];
extern int count[CM_MGR_CPU_CLUSTER];
extern int count_ack[CM_MGR_CPU_CLUSTER];
extern int vcore_dram_opp;
extern int vcore_dram_opp_cur;
extern int cm_mgr_abs_load;
extern int cm_mgr_rel_load;
extern int total_bw;
extern int cps_valid;
extern int debounce_times_up;
extern int debounce_times_down;
extern int ratio_scale[CM_MGR_CPU_CLUSTER];
extern int max_load[CM_MGR_CPU_CLUSTER];
extern int cpu_load[NR_CPUS];
extern int loading_acc[NR_CPUS];
extern int loading_cnt;
extern int cm_debug_log;
extern int cm_min_opp;
extern int cm_max_opp;
extern void cm_mgr_update_met(void);
#ifndef	CM_EMI_API
extern int cm_mgr_check_bw_status(void);
#endif
extern int cm_mgr_get_idx(void);
extern int cm_mgr_get_stall_ratio(int cpu);
extern int cm_mgr_get_cpu_count(int cluster);
extern int cm_mgr_get_max_stall_ratio(int cluster);
extern int cm_mgr_check_stall_ratio(int mp0, int mp1);
extern void cm_mgr_ratio_timer_en(int enable);
extern void cm_mgr_perf_platform_set_status(int enable);
extern void cm_mgr_perf_platform_set_force_status(int enable);
extern int cm_mgr_register_init(void);
extern int cm_mgr_platform_init(void);
extern void cm_mgr_set_dram_level(int level);
#ifdef USE_CPU_TO_DRAM_MAP
extern void cm_mgr_update_dram_by_cpu_opp(int cpu_opp);
#endif
extern int cm_mgr_get_dram_opp(void);
extern void cm_mgr_emi_latency(int enable);
extern void cm_emi_mon_init(void);
extern void cm_emi_mon_restart(void);
extern bool is_8168_dram_type_pcddr(void);
#endif	/* __MTK_CM_MGR_PLATFORM_H__ */
