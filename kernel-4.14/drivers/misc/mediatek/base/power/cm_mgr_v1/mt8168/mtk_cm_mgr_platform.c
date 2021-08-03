/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

/* system includes */
#include <linux/cpufreq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cpu.h>
#include <linux/cpufreq.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/hrtimer.h>
#include <linux/sched/rt.h>
#include <linux/atomic.h>
#include <linux/clk.h>
#include <linux/ktime.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/bitops.h>
#include <linux/uaccess.h>
#include <linux/sched/clock.h>
#include <linux/seq_file.h>
#include <linux/types.h>
#include <linux/suspend.h>
#include <linux/topology.h>
#include <linux/math64.h>
#include <mt-plat/sync_write.h>
#include <mt-plat/mtk_io.h>
#include <mt-plat/aee.h>
#include <trace/events/mtk_events.h>
#include <mt-plat/met_drv.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <mtk_cm_mgr.h>

#define CREATE_TRACE_POINTS
#include "mtk_cm_mgr_platform_events.h"

#include <linux/fb.h>
#include <linux/notifier.h>

#include <linux/pm_qos.h>
#include "helio-dvfsrc.h"
#ifdef USE_IDLE_NOTIFY
#include "mtk_idle.h"
#endif /* USE_IDLE_NOTIFY */

#if defined(CONFIG_MTK_CPU_FREQ) || defined(CONFIG_CPU_FREQ)
#include <mtk_cpufreq_api.h>
#endif /* CONFIG_MTK_CPU_FREQ */

#define cm_platform_stall_printk(fmt, args...)   \
	do {                                    \
		if (cm_debug_log & 0x40) {                \
			pr_info("CM_MGR:" fmt, ##args); \
		}                                   \
	} while (0)

#define cm_platform_dprintk(fmt, args...)   \
	do {                                    \
		if (cm_debug_log & 0x20) {                \
			pr_info("CM_MGR:" fmt, ##args); \
		}                                   \
	} while (0)

#define cm_platform_printk(fmt, args...) \
	do {                                    \
		if (cm_debug_log & 0x10) {                \
			pr_info("CM_MGR:" fmt, ##args); \
		}                                   \
	} while (0)

struct cm_mgr_met_data {
	unsigned int cm_mgr_power[14];
	unsigned int cm_mgr_count[4];
	unsigned int cm_mgr_opp[6];
	unsigned int cm_mgr_loading[12];
	unsigned int cm_mgr_ratio[12];
	unsigned int cm_mgr_bw;
	unsigned int cm_mgr_valid;
};

/******************** MET BEGIN ********************/
typedef void (*cm_mgr_value_handler_t) (unsigned int cnt, unsigned int *value);

static struct cm_mgr_met_data met_data;
static cm_mgr_value_handler_t cm_mgr_power_dbg_handler;
static cm_mgr_value_handler_t cm_mgr_count_dbg_handler;
static cm_mgr_value_handler_t cm_mgr_opp_dbg_handler;
static cm_mgr_value_handler_t cm_mgr_loading_dbg_handler;
static cm_mgr_value_handler_t cm_mgr_ratio_dbg_handler;
static cm_mgr_value_handler_t cm_mgr_bw_dbg_handler;
static cm_mgr_value_handler_t cm_mgr_valid_dbg_handler;
static int cm_mgr_dram_type_pcddr;

#define CM_MGR_MET_REG_FN_VALUE(name)				\
	void cm_mgr_register_##name(cm_mgr_value_handler_t handler)	\
{								\
	name##_dbg_handler = handler;				\
}								\
EXPORT_SYMBOL(cm_mgr_register_##name)

CM_MGR_MET_REG_FN_VALUE(cm_mgr_power);
CM_MGR_MET_REG_FN_VALUE(cm_mgr_count);
CM_MGR_MET_REG_FN_VALUE(cm_mgr_opp);
CM_MGR_MET_REG_FN_VALUE(cm_mgr_loading);
CM_MGR_MET_REG_FN_VALUE(cm_mgr_ratio);
CM_MGR_MET_REG_FN_VALUE(cm_mgr_bw);
CM_MGR_MET_REG_FN_VALUE(cm_mgr_valid);
/********************* MET END *********************/

void cm_mgr_update_met(void)
{
	int cpu;

	met_data.cm_mgr_power[0] = cpu_power_up_array[0];
	met_data.cm_mgr_power[2] = cpu_power_down_array[0];
	met_data.cm_mgr_power[4] = cpu_power_up[0];
	met_data.cm_mgr_power[6] = cpu_power_down[0];
	met_data.cm_mgr_power[8] = cpu_power_up[0];
	met_data.cm_mgr_power[9] = cpu_power_down[0];
	met_data.cm_mgr_power[10] = (unsigned int)vcore_power_up;
	met_data.cm_mgr_power[11] = (unsigned int)vcore_power_down;
	met_data.cm_mgr_power[12] = v2f[0];

	met_data.cm_mgr_count[0] = count[0];
	met_data.cm_mgr_count[2] = count_ack[0];

	met_data.cm_mgr_opp[0] = vcore_dram_opp;
	met_data.cm_mgr_opp[1] = vcore_dram_opp_cur;
	met_data.cm_mgr_opp[2] = cpu_opp_cur[0];
	met_data.cm_mgr_opp[4] = debounce_times_up;
	met_data.cm_mgr_opp[5] = debounce_times_down;

	met_data.cm_mgr_loading[0] = cm_mgr_abs_load;
	met_data.cm_mgr_loading[1] = cm_mgr_rel_load;
	met_data.cm_mgr_loading[2] = max_load[0];
	for_each_possible_cpu(cpu) {
		if (cpu >= CM_MGR_CPU_COUNT)
			break;
		met_data.cm_mgr_loading[4 + cpu] = cpu_load[cpu];
	}

	met_data.cm_mgr_ratio[0] = ratio_max[0];
	met_data.cm_mgr_ratio[2] = ratio_scale[0];
	met_data.cm_mgr_ratio[4] = ratio[0];
	met_data.cm_mgr_ratio[5] = ratio[1];
	met_data.cm_mgr_ratio[6] = ratio[2];
	met_data.cm_mgr_ratio[7] = ratio[3];

	met_data.cm_mgr_bw = total_bw;

	met_data.cm_mgr_valid = cps_valid;

	if (cm_mgr_power_dbg_handler)
		cm_mgr_power_dbg_handler(ARRAY_SIZE(met_data.cm_mgr_power),
				met_data.cm_mgr_power);
	if (cm_mgr_count_dbg_handler)
		cm_mgr_count_dbg_handler(ARRAY_SIZE(met_data.cm_mgr_count),
				met_data.cm_mgr_count);
	if (cm_mgr_opp_dbg_handler)
		cm_mgr_opp_dbg_handler(ARRAY_SIZE(met_data.cm_mgr_opp),
				met_data.cm_mgr_opp);
	if (cm_mgr_loading_dbg_handler)
		cm_mgr_loading_dbg_handler(ARRAY_SIZE(met_data.cm_mgr_loading),
				met_data.cm_mgr_loading);
	if (cm_mgr_ratio_dbg_handler)
		cm_mgr_ratio_dbg_handler(ARRAY_SIZE(met_data.cm_mgr_ratio),
				met_data.cm_mgr_ratio);
	if (cm_mgr_bw_dbg_handler)
		cm_mgr_bw_dbg_handler(1, &met_data.cm_mgr_bw);
	if (cm_mgr_valid_dbg_handler)
		cm_mgr_valid_dbg_handler(1, &met_data.cm_mgr_valid);
}

#include <linux/cpu_pm.h>
static int cm_mgr_idle_mask;

void __iomem *mcucfg_mp0_counter_base;
void __iomem *MT8168_EMI_BASE;

spinlock_t cm_mgr_cpu_mask_lock;

#define diff_value_overflow(diff, a, b) do { \
	if ((a) >= (b)) \
		diff = (a) - (b); \
	else \
		diff = 0xffffffff - (b) + (a); \
} while (0) \

#define CM_MGR_MAX(a, b) (((a) > (b)) ? (a) : (b))

#define USE_DEBUG_LOG	(1)

struct stall_s {
	unsigned int clustor[CM_MGR_CPU_CLUSTER];
	unsigned long long stall_val[CM_MGR_CPU_COUNT];
	unsigned long long stall_val_diff[CM_MGR_CPU_COUNT];
	unsigned long long time_ns[CM_MGR_CPU_COUNT];
	unsigned long long time_ns_diff[CM_MGR_CPU_COUNT];
	unsigned long long ratio[CM_MGR_CPU_COUNT];
	unsigned int ratio_max[CM_MGR_CPU_COUNT];
	unsigned int cpu;
	unsigned int cpu_count[CM_MGR_CPU_CLUSTER];
};

static struct stall_s stall_all;
static struct stall_s *pstall_all = &stall_all;
static int cm_mgr_idx = -1;

#ifdef USE_DEBUG_LOG
void debug_stall(int cpu)
{
	cm_platform_stall_printk("%s: cpu number %d ################\n",
			__func__, cpu);
	cm_platform_stall_printk("%s: clustor[%d] 0x%08x\n", __func__,
			cpu / CM_MGR_CPU_LIMIT,
			pstall_all->clustor[cpu / CM_MGR_CPU_LIMIT]);
	cm_platform_stall_printk("%s: stall_val[%d] 0x%016llx\n", __func__,
			cpu, pstall_all->stall_val[cpu]);
	cm_platform_stall_printk("%s: stall_val_diff[%d] 0x%016llx\n",
			__func__, cpu, pstall_all->stall_val_diff[cpu]);
	cm_platform_stall_printk("%s: time_ns[%d] 0x%016llx\n", __func__,
			cpu, pstall_all->time_ns[cpu]);
	cm_platform_stall_printk("%s: time_ns_diff[%d] 0x%016llx\n", __func__,
			cpu, pstall_all->time_ns_diff[cpu]);
	cm_platform_stall_printk("%s: ratio[%d] 0x%016llx\n", __func__,
			cpu, pstall_all->ratio[cpu]);
	cm_platform_stall_printk("%s: ratio_max[%d] 0x%08x\n", __func__,
			cpu / CM_MGR_CPU_LIMIT,
			pstall_all->ratio_max[cpu / CM_MGR_CPU_LIMIT]);
	cm_platform_stall_printk("%s: cpu 0x%08x\n",
			__func__, pstall_all->cpu);
	cm_platform_stall_printk("%s: cpu_count[%d] 0x%08x\n",
			__func__, cpu / CM_MGR_CPU_LIMIT,
			pstall_all->cpu_count[cpu / CM_MGR_CPU_LIMIT]);
}

void debug_stall_all(void)
{
	int i;

	for (i = 0; i < CM_MGR_CPU_COUNT; i++)
		debug_stall(i);
}
#endif /* USE_DEBUG_LOG */

static int cm_mgr_check_dram_type(void)
{
#ifdef CONFIG_MTK_DRAMC
	int ddr_type = get_ddr_type();
	int ddr_hz = dram_steps_freq(0);

	if (ddr_type == TYPE_LPDDR4X || ddr_type == TYPE_LPDDR4)
		cm_mgr_idx = CM_MGR_LP4X_2CH_3200;
	else if (ddr_type == TYPE_LPDDR3)
		cm_mgr_idx = CM_MGR_LP3_1CH_1866;
	else {
		cm_mgr_enable = 0;
		cm_mgr_dram_type_pcddr = 1;
	}

	cm_platform_dprintk("#@# %s(%d) ddr_type 0x%x, ddr_hz %d, idx 0x%x\n",
				__func__, __LINE__, ddr_type, ddr_hz,
				cm_mgr_idx);
#else
	cm_mgr_idx = 0;
	cm_platform_dprintk("#@# %s(%d) NO CONFIG_DRAMC!!! set idx to 0x%x\n",
				__func__, __LINE__, cm_mgr_idx);
#endif /* CONFIG_MTK_DRAMC */

#if defined(CONFIG_MTK_TINYSYS_SSPM_SUPPORT) && defined(USE_CM_MGR_AT_SSPM)
	if (cm_mgr_idx >= 0)
		cm_mgr_to_sspm_command(IPI_CM_MGR_DRAM_TYPE, cm_mgr_idx);
#endif /* CONFIG_MTK_TINYSYS_SSPM_SUPPORT */

	return cm_mgr_idx;
};

int cm_mgr_get_idx(void)
{
	if (cm_mgr_idx < 0) {
		cm_platform_dprintk("%s cm_mgr_check_dram_type() = %d\n",
			__func__,
			cm_mgr_check_dram_type());
		return cm_mgr_check_dram_type();
	}

	cm_platform_dprintk("%s cm_mgr_idx() = %d\n", __func__,
				cm_mgr_idx);
	return cm_mgr_idx;
};

int cm_mgr_get_stall_ratio(int cpu)
{
	return pstall_all->ratio[cpu];
}

int cm_mgr_get_max_stall_ratio(int cluster)
{
	return pstall_all->ratio_max[cluster];
}

int cm_mgr_get_cpu_count(int cluster)
{
	return pstall_all->cpu_count[cluster];
}

static ktime_t cm_mgr_init_time;
static int cm_mgr_init_flag;

static unsigned int cm_mgr_read_stall(int cpu)
{
	unsigned int val = 0;
	unsigned long spinlock_save_flags;

	if (cm_mgr_init_flag) {
		if (ktime_ms_delta(ktime_get(), cm_mgr_init_time) <
				CM_MGR_INIT_DELAY_MS)
			return val;
		cm_mgr_init_flag = 0;
	}

	if (!spin_trylock_irqsave(&cm_mgr_cpu_mask_lock, spinlock_save_flags))
		return val;

	if (cpu < CM_MGR_CPU_LIMIT) {
		if (cm_mgr_idle_mask & CLUSTER0_MASK)
			val = cm_mgr_read(MP0_CPU0_STALL_COUNTER + 4 * cpu);
	}
	spin_unlock_irqrestore(&cm_mgr_cpu_mask_lock, spinlock_save_flags);

	return val;
}

int cm_mgr_check_stall_ratio(int mp0, int mp1)
{
	unsigned int i;
	unsigned int clustor;
	unsigned int stall_val_new;
	unsigned long long time_ns_new;

	pstall_all->clustor[0] = mp0;
	pstall_all->cpu = 0;
	for (i = 0; i < CM_MGR_CPU_CLUSTER; i++) {
		pstall_all->ratio_max[i] = 0;
		pstall_all->cpu_count[i] = 0;
	}

	for (i = 0; i < CM_MGR_CPU_COUNT; i++) {
		pstall_all->ratio[i] = 0;
		clustor = i / CM_MGR_CPU_LIMIT;

		stall_val_new = cm_mgr_read_stall(i);

		if (stall_val_new == 0 || stall_val_new == 0xdeadbeef) {
#ifdef USE_DEBUG_LOG
			cm_platform_printk("%s: WARN!!! stall_val_new is 0x%08x\n",
					__func__, stall_val_new);
			debug_stall(i);
#endif /* USE_DEBUG_LOG */
			continue;
		}

		time_ns_new = sched_clock();
		pstall_all->time_ns_diff[i] =
			time_ns_new - pstall_all->time_ns[i];
		pstall_all->time_ns[i] = time_ns_new;
		if (pstall_all->time_ns_diff[i] == 0)
			continue;

		diff_value_overflow(pstall_all->stall_val_diff[i],
				stall_val_new, pstall_all->stall_val[i]);
		pstall_all->stall_val[i] = stall_val_new;

		if (pstall_all->stall_val_diff[i] == 0) {
#ifdef USE_DEBUG_LOG
			cm_platform_dprintk("%s: WARN!!! cpu:%d diff == 0\n",
						__func__, i);
			debug_stall(i);
#endif /* USE_DEBUG_LOG */
			continue;
		}

#ifdef CONFIG_ARM64
		pstall_all->ratio[i] = pstall_all->stall_val_diff[i] * 100000 /
			pstall_all->time_ns_diff[i] /
			pstall_all->clustor[clustor];
#else
		pstall_all->ratio[i] = pstall_all->stall_val_diff[i] * 100000;
		do_div(pstall_all->ratio[i], pstall_all->time_ns_diff[i]);
		do_div(pstall_all->ratio[i], pstall_all->clustor[clustor]);
#endif /* CONFIG_ARM64 */
		if (pstall_all->ratio[i] > 100) {
#ifdef USE_DEBUG_LOG
			cm_platform_printk("%s: WARN!!! cpu:%d ratio > 100\n",
					__func__, i);
			debug_stall(i);
#endif /* USE_DEBUG_LOG */
			pstall_all->ratio[i] = 100;
			/* continue; */
		}

		pstall_all->cpu |= (1 << i);
		pstall_all->cpu_count[clustor]++;
		pstall_all->ratio_max[clustor] =
			CM_MGR_MAX(pstall_all->ratio[i],
					pstall_all->ratio_max[clustor]);
#ifdef USE_DEBUG_LOG
		debug_stall(i);
#endif /* USE_DEBUG_LOG */
	}

#ifdef USE_DEBUG_LOG
	debug_stall_all();
#endif /* USE_DEBUG_LOG */
	return 0;
}

static void init_cpu_stall_counter(int cluster)
{
	unsigned int val;

	cm_mgr_init_time = ktime_get();
	cm_mgr_init_flag = 1;

	if (cluster == 0) {
		val = 0x11000;
		cm_mgr_write(MP0_CPU_STALL_INFO, val);

		/* please check CM_MGR_INIT_DELAY_MS value */
		val = RG_FMETER_EN;
		val |= RG_MP0_AVG_STALL_PERIOD_1MS;
		val |= RG_CPU0_AVG_STALL_RATIO_EN |
			RG_CPU0_STALL_COUNTER_EN |
			RG_CPU0_NON_WFX_COUNTER_EN;
		cm_mgr_write(MP0_CPU0_AVG_STALL_RATIO_CTRL, val);

		val = RG_CPU0_AVG_STALL_RATIO_EN |
			RG_CPU0_STALL_COUNTER_EN |
			RG_CPU0_NON_WFX_COUNTER_EN;
		cm_mgr_write(MP0_CPU1_AVG_STALL_RATIO_CTRL, val);

		val = RG_CPU0_AVG_STALL_RATIO_EN |
			RG_CPU0_STALL_COUNTER_EN |
			RG_CPU0_NON_WFX_COUNTER_EN;
		cm_mgr_write(MP0_CPU2_AVG_STALL_RATIO_CTRL, val);

		val = RG_CPU0_AVG_STALL_RATIO_EN |
			RG_CPU0_STALL_COUNTER_EN |
			RG_CPU0_NON_WFX_COUNTER_EN;
		cm_mgr_write(MP0_CPU3_AVG_STALL_RATIO_CTRL, val);
	}
}

#ifdef CONFIG_CPU_PM
static int cm_mgr_sched_pm_notifier(struct notifier_block *self,
			       unsigned long cmd, void *v)
{
	unsigned int cur_cpu = smp_processor_id();
	unsigned long spinlock_save_flags;

	spin_lock_irqsave(&cm_mgr_cpu_mask_lock, spinlock_save_flags);

	if (cmd == CPU_PM_EXIT) {
		if (((cm_mgr_idle_mask & CLUSTER0_MASK) == 0x0) &&
				(cur_cpu < CM_MGR_CPU_LIMIT))
			init_cpu_stall_counter(0);
		else if (((cm_mgr_idle_mask & CLUSTER1_MASK) == 0x0) &&
				(cur_cpu >= CM_MGR_CPU_LIMIT))
			init_cpu_stall_counter(1);
		cm_mgr_idle_mask |= (1 << cur_cpu);
	} else if (cmd == CPU_PM_ENTER)
		cm_mgr_idle_mask &= ~(1 << cur_cpu);

	spin_unlock_irqrestore(&cm_mgr_cpu_mask_lock, spinlock_save_flags);

	return NOTIFY_OK;
}

static struct notifier_block cm_mgr_sched_pm_notifier_block = {
	.notifier_call = cm_mgr_sched_pm_notifier,
};

static void cm_mgr_sched_pm_init(void)
{
	cpu_pm_register_notifier(&cm_mgr_sched_pm_notifier_block);
}

#else
static inline void cm_mgr_sched_pm_init(void) { }
#endif /* CONFIG_CPU_PM */

#ifdef USE_HOTPLUG_NOTIFY
static int cm_mgr_cpu_callback(struct notifier_block *nfb,
				   unsigned long action, void *hcpu)
{
	unsigned int cur_cpu = (long)hcpu;
	unsigned long spinlock_save_flags;

	spin_lock_irqsave(&cm_mgr_cpu_mask_lock, spinlock_save_flags);

	switch (action) {
	case CPU_ONLINE:
		if (((cm_mgr_idle_mask & CLUSTER0_MASK) == 0x0) &&
				(cur_cpu < CM_MGR_CPU_LIMIT))
			init_cpu_stall_counter(0);
		else if (((cm_mgr_idle_mask & CLUSTER1_MASK) == 0x0) &&
				(cur_cpu >= CM_MGR_CPU_LIMIT))
			init_cpu_stall_counter(1);
		cm_mgr_idle_mask |= (1 << cur_cpu);
		break;
	case CPU_DOWN_PREPARE:
		cm_mgr_idle_mask &= ~(1 << cur_cpu);
		break;
	}

	spin_unlock_irqrestore(&cm_mgr_cpu_mask_lock, spinlock_save_flags);

	return NOTIFY_OK;
}

#define CPU_PRI_PERF 20

static struct notifier_block cm_mgr_cpu_notifier = {
	.notifier_call = cm_mgr_cpu_callback,
	.priority = CPU_PRI_PERF + 1,
};

static void cm_mgr_hotplug_cb_init(void)
{
	register_cpu_notifier(&cm_mgr_cpu_notifier);
}
#endif

static int cm_mgr_fb_notifier_callback(struct notifier_block *self,
		unsigned long event, void *data)
{
	struct fb_event *evdata = data;
	int blank;

	if (event != FB_EVENT_BLANK)
		return 0;

	blank = *(int *)evdata->data;

	switch (blank) {
	case FB_BLANK_UNBLANK:
		cm_platform_dprintk("#@# %s(%d) SCREEN ON\n",
					__func__, __LINE__);
		cm_mgr_blank_status = 0;
#if defined(CONFIG_MTK_TINYSYS_SSPM_SUPPORT) && defined(USE_CM_MGR_AT_SSPM)
		cm_mgr_to_sspm_command(IPI_CM_MGR_BLANK, 0);
#endif /* CONFIG_MTK_TINYSYS_SSPM_SUPPORT */
		break;
	case FB_BLANK_POWERDOWN:
		cm_platform_dprintk("#@# %s(%d) SCREEN OFF\n",
					__func__, __LINE__);
		cm_mgr_blank_status = 1;
		cm_mgr_set_dram_level(0);
#if defined(CONFIG_MTK_TINYSYS_SSPM_SUPPORT) && defined(USE_CM_MGR_AT_SSPM)
		cm_mgr_to_sspm_command(IPI_CM_MGR_BLANK, 1);
#endif /* CONFIG_MTK_TINYSYS_SSPM_SUPPORT */
		break;
	default:
		break;
	}

	return 0;
}

static struct notifier_block cm_mgr_fb_notifier = {
	.notifier_call = cm_mgr_fb_notifier_callback,
};

#ifdef USE_IDLE_NOTIFY
static int cm_mgr_idle_cb(struct notifier_block *nfb,
				  unsigned long id,
				  void *arg)
{
	switch (id) {
	case NOTIFY_SOIDLE_ENTER:
	case NOTIFY_SOIDLE3_ENTER:
		if (get_cur_ddr_opp() != CM_MGR_EMI_OPP)
			check_cm_mgr_status_internal();
		break;
	case NOTIFY_DPIDLE_ENTER:
	case NOTIFY_DPIDLE_LEAVE:
	case NOTIFY_SOIDLE_LEAVE:
	case NOTIFY_SOIDLE3_LEAVE:
	default:
		break;
	}

	return NOTIFY_OK;
}

static struct notifier_block cm_mgr_idle_notify = {
	.notifier_call = cm_mgr_idle_cb,
};
#endif /* USE_IDLE_NOTIFY */

struct timer_list cm_mgr_ratio_timer;
#define CM_MGR_RATIO_TIMER_MS	msecs_to_jiffies(1)

static void cm_mgr_ratio_timer_fn(unsigned long data)
{
	trace_CM_MGR__stall_raio_0(
			(unsigned int)cm_mgr_read(MP0_CPU_AVG_STALL_RATIO));

	cm_mgr_ratio_timer.expires = jiffies + CM_MGR_RATIO_TIMER_MS;
	add_timer(&cm_mgr_ratio_timer);
}

void cm_mgr_ratio_timer_en(int enable)
{
	if (enable) {
		cm_mgr_ratio_timer.expires = jiffies + CM_MGR_RATIO_TIMER_MS;
		add_timer(&cm_mgr_ratio_timer);
	} else {
		del_timer(&cm_mgr_ratio_timer);
	}
}

bool is_8168_dram_type_pcddr(void)
{
	if (cm_mgr_dram_type_pcddr) {
		cm_platform_dprintk("CM manager do not support PCDDR!\n");
		cm_platform_dprintk("%s get_ddr_type() = %d\n",
				     __func__, get_ddr_type());
		return true;
	} else
		return false;
}

static struct pm_qos_request ddr_opp_req;
static int debounce_times_perf_down_local;
static int pm_qos_update_request_status;
void cm_mgr_perf_platform_set_status(int enable)
{
	if (enable) {
		debounce_times_perf_down_local = 0;

		if (cm_mgr_perf_enable == 0)
			return;

		pm_qos_update_request(&ddr_opp_req, DDR_OPP_0);
		pm_qos_update_request_status = enable;
	} else {
		if (++debounce_times_perf_down_local < debounce_times_perf_down)
			return;

		pm_qos_update_request(&ddr_opp_req, DDR_OPP_UNREQ);

		pm_qos_update_request_status = enable;

		debounce_times_perf_down_local = 0;
	}
}

void cm_mgr_perf_platform_set_force_status(int enable)
{
	if (enable) {
		debounce_times_perf_down_local = 0;

		if (cm_mgr_perf_enable == 0)
			return;

		if ((cm_mgr_perf_force_enable == 0) ||
				(pm_qos_update_request_status == 1))
			return;

		pm_qos_update_request(&ddr_opp_req, DDR_OPP_0);
		pm_qos_update_request_status = enable;
	} else {
		if (pm_qos_update_request_status == 0)
			return;

		if ((cm_mgr_perf_force_enable == 0) ||
				(++debounce_times_perf_down_local >=
				 debounce_times_perf_force_down)) {

			pm_qos_update_request(&ddr_opp_req,
				DDR_OPP_UNREQ);

			pm_qos_update_request_status = enable;

			debounce_times_perf_down_local = 0;
		}
	}
}

int cm_mgr_register_init(void)
{
	struct device_node *node;
	int ret;
	const char *buf;

#if defined(CONFIG_MACH_MT8168)
	MT8168_EMI_BASE = mt_cen_emi_base_get();

	node = of_find_compatible_node(NULL, NULL,
			"mediatek,mt8168-mcusys");
#else
	node = of_find_compatible_node(NULL, NULL,
			"mediatek,mcucfg_mp0_counter");
#endif
	if (!node)
		pr_debug("find mcucfg_mp0_counter node failed\n");
	mcucfg_mp0_counter_base = of_iomap(node, 0);
	if (!mcucfg_mp0_counter_base) {
		pr_debug("base mcucfg_mp0_counter_base failed\n");
		return -1;
	}

	if (node) {
		ret = of_property_read_string(node,
				"status", (const char **)&buf);

		if (ret == 0) {
			if (!strcmp(buf, "enable"))
				cm_mgr_enable = 1;
			else
				cm_mgr_enable = 0;
		}
	}

	return 0;
}

/**
 * cpufreq_cm_notifier - notifier callback for cpufreq policy change.
 * @nb:	struct notifier_block * with callback info.
 * @event: value showing cpufreq event for which this function invoked.
 * @data: callback-specific data
 *
 * Callback to hijack the notification on cpufreq policy transition.
 * Every time there is a change in policy, we will intercept and
 * update the cpufreq policy with CM manager constraints.
 *
 * Return: 0 (success)
 */
static int cur_cpufreq;
static int old_cpufreq;

static int mtk_cpufreq_cm_notifier(struct notifier_block *nb,
				    unsigned long event, void *data)
{
	if (event != CPUFREQ_ADJUST)
		return NOTIFY_DONE;

	cur_cpufreq = mt_cpufreq_get_cur_freq(0);

	cm_platform_dprintk("%s, old_cpufreq =%d, cur_cpufreq = %d\n",
		__func__, old_cpufreq, cur_cpufreq);

	if (cur_cpufreq != old_cpufreq)
		check_cm_mgr_status(0, cur_cpufreq);

	old_cpufreq = cur_cpufreq;

	return NOTIFY_OK;
}

/* Notifier for cpufreq policy change */
static struct notifier_block cm_cpufreq_notifier_block = {
	.notifier_call = mtk_cpufreq_cm_notifier,
};

int cm_mgr_platform_init(void)
{
	int r;

	spin_lock_init(&cm_mgr_cpu_mask_lock);

	r = cm_mgr_register_init();
	if (r) {
		pr_debug("FAILED TO CREATE REGISTER(%d)\n", r);
		return r;
	}

	cm_mgr_sched_pm_init();

	r = fb_register_client(&cm_mgr_fb_notifier);
	if (r) {
		pr_debug("FAILED TO REGISTER FB CLIENT (%d)\n", r);
		return r;
	}

#ifdef USE_HOTPLUG_NOTIFY
	cm_mgr_hotplug_cb_init();
#endif

#ifdef USE_IDLE_NOTIFY
	mtk_idle_notifier_register(&cm_mgr_idle_notify);
#endif /* USE_IDLE_NOTIFY */

	init_timer_deferrable(&cm_mgr_ratio_timer);
	cm_mgr_ratio_timer.function = cm_mgr_ratio_timer_fn;
	cm_mgr_ratio_timer.data = 0;

#if defined(CONFIG_MACH_MT8168)
	cpufreq_register_notifier(&cm_cpufreq_notifier_block,
					  CPUFREQ_TRANSITION_NOTIFIER);
#else
	mt_cpufreq_set_governor_freq_registerCB(check_cm_mgr_status);
#endif

	pm_qos_add_request(&ddr_opp_req, PM_QOS_EMI_OPP,
		PM_QOS_EMI_OPP_DEFAULT_VALUE);

#ifdef CM_EMI_API
	cm_emi_mon_init();
#endif
	return r;
}

void cm_mgr_set_dram_level(int level)
{
	static int cm_limit_dram_freq[3] = {1600, 2600, 3200};

	if (level >= cm_max_opp)
		level = cm_max_opp;

	if (level <= cm_min_opp)
		level = cm_min_opp;

	dvfsrc_set_power_model_ddr_request(level);

	cm_platform_printk("%s limit dram freq = %d\n",
		__func__, cm_limit_dram_freq[level]);
}

int cm_mgr_get_dram_opp(void)
{
	static int cm_get_dram_freq[3] = {3200, 2600, 1600};
	int dram_opp_cur = get_cur_ddr_opp();

	cm_platform_printk("%s get_cur_ddr_opp = %d, freq = %d\n",
		__func__, dram_opp_cur, cm_get_dram_freq[dram_opp_cur]);

	if (dram_opp_cur < 0 || dram_opp_cur > CM_MGR_EMI_OPP)
		dram_opp_cur = 0;

	return dram_opp_cur;
}

#ifndef	CM_EMI_API
int cm_mgr_check_bw_status(void)
{
	if (cm_mgr_get_bw() > CM_MGR_BW_VALUE)
		return 1;
	else
		return 0;
}
#endif

void cm_mgr_emi_latency(int enable)
{
}

#ifdef USE_CPU_TO_DRAM_MAP
static int cm_mgr_cpu_opp_to_dram[CM_MGR_CPU_OPP_SIZE] = {
/* star from cpu opp 0 */
	0,
	0,
	0,
	0,
	1,
	1,
	1,
	1,
	1,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
};

static void cm_mgr_process(struct work_struct *work)
{
	pm_qos_update_request(&ddr_opp_req_by_cpu_opp, cm_mgr_cpu_to_dram_opp);
}

void cm_mgr_update_dram_by_cpu_opp(int cpu_opp)
{
	int ret = 0;
	int dram_opp = 0;

	if (!is_dvfsrc_enabled())
		return;

	if ((cpu_opp >= 0) && (cpu_opp < CM_MGR_CPU_OPP_SIZE))
		dram_opp = cm_mgr_cpu_opp_to_dram[cpu_opp];

	if (cm_mgr_cpu_to_dram_opp == dram_opp)
		return;

	cm_mgr_cpu_to_dram_opp = dram_opp;

	ret = schedule_delayed_work(&cm_mgr_work, 1);
}
#endif /* USE_CPU_TO_DRAM_MAP */
