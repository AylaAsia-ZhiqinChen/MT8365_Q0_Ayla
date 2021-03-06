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

#include <linux/console.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/spinlock.h>
#include <linux/suspend.h>
#include <linux/time.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/device.h>
#include <mt-plat/aee.h>

#include "mtk_sleep.h"
#include "mtk_spm.h"
#include "mtk_spm_idle.h"
#include "mtk_spm_sleep.h"
#include <mt-plat/sync_write.h>

/* #include <mt-plat/mt_gpio.h> */
#if 0
#include <mach/battery_common.h>
#include <mach/systracker.h>
#endif

#undef ENABLE_AUTO_SUSPEND_TEST

/**************************************
 * only for internal debug
 **************************************/
#ifdef CONFIG_MTK_LDVT
#define SLP_SLEEP_DPIDLE_EN 1
#define SLP_REPLACE_DEF_WAKESRC 1
#define SLP_SUSPEND_LOG_EN 1
#else
#define SLP_SLEEP_DPIDLE_EN 1 /* FM@Suspend */
#define SLP_REPLACE_DEF_WAKESRC 0
#define SLP_SUSPEND_LOG_EN 1
#endif

/**************************************
 * SW code for suspend
 **************************************/
/* #define slp_read(addr)              (*(volatile u32 *)(addr)) */
#define slp_write(addr, val) mt65xx_reg_sync_writel(val, addr)

#undef TAG
#define TAG "[SLP] "

#define slp_pr_err(fmt, args...) pr_err(TAG "" fmt, ##args)
#define slp_pr_warn(fmt, args...) pr_warn(TAG "" fmt, ##args)
#define slp_notice(fmt, args...) pr_notice(TAG "" fmt, ##args)
#define slp_info(fmt, args...) pr_info(TAG "" fmt, ##args)
#define slp_debug(fmt, args...) pr_debug(TAG "" fmt, ##args)

#define slp_crit2(fmt, args...)                                                \
	do {                                                                   \
		aee_sram_printk(fmt, ##args);                                  \
		slp_crit(fmt, ##args);                                         \
	} while (0)

/* void systracker_enable(void); */
/* extern void mt_power_gs_dump_suspend(void); */

#ifdef CONFIG_MTK_FPGA
/* TODO: let fpga config build pass */
int __attribute__((weak))
spm_set_sleep_wakesrc(u32 wakesrc, bool enable, bool replace)
{
	return 0;
}

int __attribute__((weak))
spm_set_dpidle_wakesrc(u32 wakesrc, bool enable, bool replace)
{
	return 0;
}

void __attribute__((weak)) spm_output_sleep_option(void)
{
}
#else
/* TODO: let evb config build pass */
int __attribute__((weak))
spm_set_dpidle_wakesrc(u32 wakesrc, bool enable, bool replace)
{
	return 0;
}
__weak unsigned int spm_go_to_sleep_dpidle(u32 spm_flags, u32 spm_data)
{
	return 0;
}
#endif

static DEFINE_SPINLOCK(slp_lock);

static unsigned int slp_wake_reason = WR_NONE;

static bool slp_ck26m_on;
static bool slp_pars_dpd = 1;

static bool slp_chk_golden = 1;
static bool slp_dump_gpio;
static bool slp_dump_regs = 1;
static bool slp_check_mtcmos_pll = 1;

#ifdef ENABLE_AUTO_SUSPEND_TEST
static bool slp_auto_suspend_resume;
static u32 slp_auto_suspend_resume_cnt;
struct wakeup_source *spm_suspend_lock;
static struct hrtimer slp_auto_suspend_resume_hrtimer;
struct task_struct *slp_auto_suspend_resume_thread;
static int slp_auto_suspend_resume_timer_flag;
static DECLARE_WAIT_QUEUE_HEAD(slp_auto_suspend_resume_timer_waiter);
static u32 slp_time = 30;
#endif /* ENABLE_AUTO_SUSPEND_TEST */

static u32 slp_spm_flags = {
#if 1 /* normal suspend */
	SPM_L2_DORMANT_DIS | SPM_VPROC_LOW_DIS
#else /* legacy suspend */
	SPM_L2_DORMANT_DIS | SPM_VPROC_LOW_DIS | SPM_CPU_PDN_DIS |
	SPM_INFRA_PDN_DIS | SPM_DDRPHY_S1_DIS | SPM_26M_DIS
/* SPM_L2_DORMANT_DIS | SPM_VPROC_LOW_DIS | SPM_INFRA_PDN_DIS */
#endif
};

#if SLP_SLEEP_DPIDLE_EN
static u32 slp_spm_deepidle_flags = {
#if 1 /* normal suspend */
	SPM_INFRA_PDN_DIS | SPM_26M_OFF_DIS
/* SPM_BUS26M_DIS | SPM_MPLLOFF_DIS | SPM_FHC_SLEEP_DIS | SPM_26M_DIS */
#else
	SPM_CPU_PDN_DIS | SPM_DDRPHY_S1_DIS | SPM_VPROC_LOW_DIS |
	SPM_INFRA_PDN_DIS | SPM_26M_OFF_DIS
#endif
};
#endif

static u32 slp_spm_data;

#ifdef ENABLE_AUTO_SUSPEND_TEST
static enum hrtimer_restart
slp_auto_suspend_resume_timer_func(struct hrtimer *timer)
{
	slp_crit2("do %s...\n", __func__);

	slp_auto_suspend_resume = 0;
	slp_auto_suspend_resume_cnt = 0;

#if 0
	charging_suspend_enable();
#else
	slp_auto_suspend_resume_timer_flag = 1;
	wake_up_interruptible(&slp_auto_suspend_resume_timer_waiter);
#endif

	return HRTIMER_NORESTART;
}

static int slp_auto_suspend_resume_thread_handler(void *unused)
{
	do {
		wait_event_interruptible(slp_auto_suspend_resume_timer_waiter,
					 slp_auto_suspend_resume_timer_flag !=
						 0);
		slp_auto_suspend_resume_timer_flag = 0;
		charging_suspend_enable();
		slp_crit2(
			"%s... charging_suspend_enable\n", __func__);

	} while (!kthread_should_stop());

	return 0;
}
#endif /* ENABLE_AUTO_SUSPEND_TEST */

/* FIXME: for bring up */
static int slp_suspend_ops_valid(suspend_state_t state)
{
	return state == PM_SUSPEND_MEM;
}

static int slp_suspend_ops_begin(suspend_state_t state)
{
	/* legacy log */
	slp_notice(
		"@@@@@@@@@@@@@@@@@@@@\tChip_pm_begin(%u)(%u)\t@@@@@@@@@@@@@@@@@@@@\n",
		is_cpu_pdn(slp_spm_flags), is_infra_pdn(slp_spm_flags));

	slp_wake_reason = WR_NONE;

	return 0;
}

static int slp_suspend_ops_prepare(void)
{
	/* legacy log */
	slp_notice(
		"@@@@@@@@@@@@@@@@@@@@\tChip_pm_prepare\t@@@@@@@@@@@@@@@@@@@@\n");

#if 0
	if (slp_chk_golden)
		mt_power_gs_dump_suspend();
#endif
	return 0;
}

/* FM@Suspend */
bool __attribute__((weak)) ConditionEnterSuspend(void)
{
	return true;
}

static int slp_suspend_ops_enter(suspend_state_t state)
{
	int ret = 0;
	/* FM@Suspend */
	int fm_radio_is_playing = 0;

	if (ConditionEnterSuspend() == true)
		fm_radio_is_playing = 0;
	else
		fm_radio_is_playing = 1;

	/* legacy log */
	slp_notice(
		"@@@@@@@@@@@@@@@@@@@@\tChip_pm_enter\t@@@@@@@@@@@@@@@@@@@@\n");

#if 0 /* !defined(CONFIG_MTK_FPGA) */
	if (slp_dump_gpio)
		gpio_dump_regs();
#endif

#if 0
	if (slp_dump_regs)
		slp_dump_pm_regs();
#endif

/* todo: need mt_clkmgr.h */
#if 0
	if (slp_check_mtcmos_pll)
		slp_check_pm_mtcmos_pll();
#endif

/* todo: need mt_spm_mtcmos.h */
#if 0
	if (!spm_cpusys0_can_power_down()) {
		slp_pr_err("CANNOT SLEEP DUE TO CPU1~x PON\n");
		/* return -EPERM; */
		ret = -EPERM;
		goto LEAVE_SLEEP;
	}
#endif

	if (is_infra_pdn(slp_spm_flags) && !is_cpu_pdn(slp_spm_flags)) {
		slp_pr_err("CANNOT SLEEP DUE TO INFRA PDN BUT CPU PON\n");
		/* return -EPERM; */
		ret = -EPERM;
		goto LEAVE_SLEEP;
	}

/* only for test */
#if 0
	slp_pasr_en(1, 0x0);
	slp_dpd_en(1);
#endif

#if SLP_SLEEP_DPIDLE_EN
	if ((slp_ck26m_on) || (fm_radio_is_playing))
		slp_wake_reason = spm_go_to_sleep_dpidle(slp_spm_deepidle_flags,
							 slp_spm_data);
	else
#endif
		slp_wake_reason = spm_go_to_sleep(slp_spm_flags, slp_spm_data);

LEAVE_SLEEP:

#ifdef CONFIG_MTK_SYSTRACKER
/* systracker_enable(); */
#endif

	return ret;
}

static void slp_suspend_ops_finish(void)
{
	/* legacy log */
	slp_notice(
		"@@@@@@@@@@@@@@@@@@@@\tChip_pm_finish\t@@@@@@@@@@@@@@@@@@@@\n");
}

static void slp_suspend_ops_end(void)
{
	/* legacy log */
	slp_notice("@@@@@@@@@@@@@@@@@@@@\tChip_pm_end\t@@@@@@@@@@@@@@@@@@@@\n");

#ifdef ENABLE_AUTO_SUSPEND_TEST
	if (slp_auto_suspend_resume == 1) {
		slp_crit2("slp_auto_suspend_resume_cnt = %d\n",
			  slp_auto_suspend_resume_cnt);
		slp_auto_suspend_resume_cnt++;

		if (slp_auto_suspend_resume_cnt > 10) {
			slp_crit2("do spm_usb_resume\n");

			__pm_stay_awake(spm_suspend_lock);
			slp_auto_suspend_resume = 0;
			slp_auto_suspend_resume_cnt = 0;
		}
	}
#endif
}

static const struct platform_suspend_ops slp_suspend_ops = {
	.valid = slp_suspend_ops_valid,
	.begin = slp_suspend_ops_begin,
	.prepare = slp_suspend_ops_prepare,
	.enter = slp_suspend_ops_enter,
	.finish = slp_suspend_ops_finish,
	.end = slp_suspend_ops_end,
};

/*
 * wakesrc : WAKE_SRC_XXX
 * enable  : enable or disable @wakesrc
 * ck26m_on: if true, mean @wakesrc needs 26M to work
 */
int slp_set_wakesrc(u32 wakesrc, bool enable, bool ck26m_on)
{
	int r;
	unsigned long flags;

	slp_notice("wakesrc = 0x%x, enable = %u, ck26m_on = %u\n", wakesrc,
		   enable, ck26m_on);

#if SLP_REPLACE_DEF_WAKESRC
	if (wakesrc & WAKE_SRC_CFG_KEY)
#else
	if (!(wakesrc & WAKE_SRC_CFG_KEY))
#endif
		return -EPERM;

	spin_lock_irqsave(&slp_lock, flags);

#if SLP_REPLACE_DEF_WAKESRC
	if (ck26m_on)
		r = spm_set_dpidle_wakesrc(wakesrc, enable, true);
	else
		r = spm_set_sleep_wakesrc(wakesrc, enable, true);
#else
	if (ck26m_on)
		r = spm_set_dpidle_wakesrc(wakesrc & ~WAKE_SRC_CFG_KEY, enable,
					   false);
	else
		r = spm_set_sleep_wakesrc(wakesrc & ~WAKE_SRC_CFG_KEY, enable,
					  false);
#endif

	if (!r)
		slp_ck26m_on = ck26m_on;
	spin_unlock_irqrestore(&slp_lock, flags);

	return r;
}

unsigned int slp_get_wake_reason(void)
{
	return slp_wake_reason;
}

bool slp_will_infra_pdn(void)
{
	return is_infra_pdn(slp_spm_flags);
}

#if 0
/*
 * en: 1: enable pasr, 0: disable pasr
 * value: pasr setting (RK1, MR17 for RK0)
 */
void slp_pasr_en(bool en, u32 value)
{
	if (slp_pars_dpd) {
		if (en) {
			slp_spm_flags &= ~SPM_PASR_DIS;
			slp_spm_data = value;
		} else {
			slp_spm_flags |= SPM_PASR_DIS;
			slp_spm_data = 0;
		}
	}
}
#endif

#if 0
/*
 * en: 1: enable DPD, 0: disable DPD
 */
void slp_dpd_en(bool en)
{
	if (slp_pars_dpd) {
		if (en)
			slp_spm_flags &= ~SPM_DPD_DIS;
		else
			slp_spm_flags |= SPM_DPD_DIS;
	}
}
#endif

static int __init slp_module_init(void)
{
	spm_output_sleep_option();

	slp_notice(
		"SLEEP_DPIDLE_EN:%d, REPLACE_DEF_WAKESRC:%d, SUSPEND_LOG_EN:%d\n",
		SLP_SLEEP_DPIDLE_EN, SLP_REPLACE_DEF_WAKESRC,
		SLP_SUSPEND_LOG_EN);

#if 1
	suspend_set_ops(&slp_suspend_ops);
#endif

#if SLP_SUSPEND_LOG_EN
	console_suspend_enabled = 0;
#endif

#ifdef ENABLE_AUTO_SUSPEND_TEST
	spm_suspend_lock = wakeup_source_register("spm_wakelock");
#endif
	return 0;
}
arch_initcall(slp_module_init);

#ifdef ENABLE_AUTO_SUSPEND_TEST
void slp_start_auto_suspend_resume_timer(u32 sec)
{
	ktime_t ktime;

	slp_auto_suspend_resume = 1;
	charging_suspend_disable();

	slp_time = sec;
	slp_crit2("%s... init = %d\n", __func__, slp_time);

	ktime = ktime_set(slp_time, 0);

	hrtimer_init(&slp_auto_suspend_resume_hrtimer, CLOCK_MONOTONIC,
		     HRTIMER_MODE_REL);

	slp_auto_suspend_resume_hrtimer.function =
		slp_auto_suspend_resume_timer_func;
	hrtimer_start(&slp_auto_suspend_resume_hrtimer, ktime,
		      HRTIMER_MODE_REL);
}

void slp_create_auto_suspend_resume_thread(void)
{
	slp_auto_suspend_resume_thread =
		kthread_run(slp_auto_suspend_resume_thread_handler, 0,
			    "auto suspend resume");
}

void slp_set_auto_suspend_wakelock(bool lock)
{
	if (lock)
		__pm_stay_awake(spm_suspend_lock);
	else
		__pm_relax(spm_suspend_lock);
}
#endif /* ENABLE_AUTO_SUSPEND_TEST */

module_param(slp_ck26m_on, bool, 0644);
module_param(slp_pars_dpd, bool, 0644);
module_param(slp_spm_flags, uint, 0644);

module_param(slp_chk_golden, bool, 0644);
module_param(slp_dump_gpio, bool, 0644);
module_param(slp_dump_regs, bool, 0644);
module_param(slp_check_mtcmos_pll, bool, 0644);

MODULE_DESCRIPTION("Sleep Driver v0.1");
