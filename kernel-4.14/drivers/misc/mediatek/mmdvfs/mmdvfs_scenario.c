// SPDX-License-Identifier: GPL-2.0
/*
 * MediaTek MMDVFS driver.
 *
 * Copyright (c) 2018 MediaTek Inc.
 */

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "mmdvfs_mgr.h"
#include "mmdvfs_pmqos.h"
#include "mmdvfs_scenario.h"

#define SF_HWC_PIXEL_MAX_NORMAL  (1920 * 1080 * 7)
struct MTK_SMI_BWC_MM_INFO g_smi_bwc_mm_info = {
	0, 0, {0, 0}, {0, 0}, {0, 0}, {0, 0}, 0, 0, 0,
	SF_HWC_PIXEL_MAX_NORMAL
};

static s32 table[SMI_BWC_SCEN_CNT];
static spinlock_t lock;
static int prev_scen = -1;
s32 mmdvfs_config_scenario_concurrency(struct MTK_SMI_BWC_CONF *config)
{
	int i, scen;
	unsigned int concurrency = 0;

	if (!config) {
		pr_notice("struct MTK_SMI_BWC_CONF config no such address\n");
		return -ENXIO;
	}
	if (config->scen < 0 || config->scen >= SMI_BWC_SCEN_CNT) {
		pr_notice("Invalid config scnenario=%d, SMI_BWC_SCEN_CNT=%u\n",
			config->scen, SMI_BWC_SCEN_CNT);
		return -EINVAL;
	}
	/* table and concurrency of scenario */
	spin_lock(&lock);
	scen = config->scen;
	if (!config->b_on) {
		if (table[scen] <= 0)
			pr_notice("%s(%d) OFF not in pairs=%d\n",
				smi_bwc_scen_name_get(scen), scen,
				table[scen]);
		else
			table[scen] -= 1;
	} else
		table[scen] += 1;

	for (i = SMI_BWC_SCEN_CNT - 1; i >= 0; i--)
		if (table[i])
			break;

	prev_scen = (i > 0 ? (enum MTK_SMI_BWC_SCEN)i : 0);
	spin_unlock(&lock);

	if (config->b_on)
		mmdvfs_notify_scenario_enter(scen);
	else
		mmdvfs_notify_scenario_exit(scen);

	for (i = 0; i < SMI_BWC_SCEN_CNT; i++)
		concurrency |= (table[i] ? 1 : 0) << i;
	mmdvfs_notify_scenario_concurrency(concurrency);

	return 0;
}
EXPORT_SYMBOL_GPL(mmdvfs_config_scenario_concurrency);

void mmdvfs_scen_init(struct device *dev)
{
	mmdvfs_init(dev, &g_smi_bwc_mm_info);
	spin_lock_init(&lock);
}
EXPORT_SYMBOL_GPL(mmdvfs_scen_init);

#ifdef SMI_PARAM_DISABLE_MMDVFS
static unsigned int disable_mmdvfs_scen = SMI_PARAM_DISABLE_MMDVFS;
#else
static unsigned int disable_mmdvfs_scen;
#endif
int is_mmdvfs_scen_disabled(void)
{
	return disable_mmdvfs_scen;
}
EXPORT_SYMBOL_GPL(is_mmdvfs_scen_disabled);

static unsigned int mmdvfs_scen_log_mask = 1 << MMDVFS_SCEN_COUNT;
int mmdvfs_scen_log_mask_get(void)
{
	return mmdvfs_scen_log_mask;
}

static unsigned int mmdvfs_debug_level;
int mmdvfs_debug_level_get(void)
{
	return mmdvfs_debug_level;
}

#ifdef SMI_PARAM_DISABLE_FREQ_MUX
static unsigned int disable_freq_mux = SMI_PARAM_DISABLE_FREQ_MUX;
#else
static unsigned int disable_freq_mux = 1;
#endif
int is_mmdvfs_freq_mux_disabled(void)
{
	return disable_freq_mux;
}

#ifdef SMI_PARAM_DISABLE_FREQ_HOPPING
static unsigned int disable_freq_hopping = SMI_PARAM_DISABLE_FREQ_HOPPING;
#else
static unsigned int disable_freq_hopping = 1;
#endif
int is_mmdvfs_freq_hopping_disabled(void)
{
	return disable_freq_hopping;
}

#ifdef SMI_PARAM_DISABLE_FORCE_MMSYS_MAX_CLK
static unsigned int force_max_mmsys_clk =
	!(SMI_PARAM_DISABLE_FORCE_MMSYS_MAX_CLK);
#else
static unsigned int force_max_mmsys_clk;
#endif
int is_force_max_mmsys_clk(void)
{
	return force_max_mmsys_clk;
}

#ifdef SMI_PARAM_FORCE_MMSYS_CLKS_ALWAYS_ON
static unsigned int force_always_on_mm_clks_mask =
	SMI_PARAM_FORCE_MMSYS_CLKS_ALWAYS_ON;
#else
static unsigned int force_always_on_mm_clks_mask = 1;
#endif
int force_always_on_mm_clks(void)
{
	return force_always_on_mm_clks_mask;
}

static unsigned int clk_mux_mask = 0xFFFF;
int get_mmdvfs_clk_mux_mask(void)
{
	return clk_mux_mask;
}


module_param(mmdvfs_scen_log_mask, uint, 0644);
module_param(mmdvfs_debug_level, uint, 0644);
module_param(disable_mmdvfs_scen, uint, 0644);
module_param(disable_freq_mux, uint, 0644);
module_param(disable_freq_hopping, uint, 0644);
module_param(force_max_mmsys_clk, uint, 0644);
module_param(clk_mux_mask, uint, 0644);
