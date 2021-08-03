/*
 * Copyright (c) 2016 MediaTek Inc.
 * Author: Fish Wu <fish.wu@mediatek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/clk.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/pm_runtime.h>
#include <soc/mediatek/smi.h>
#include "vpu_pm.h"

int vpu_init_pm(struct vpu_core *vpu_core)
{
	struct device *dev;
	struct vpu_pm *pm;
	int ret = 0;

	pm = &vpu_core->pm;
	memset(pm, 0, sizeof(struct vpu_pm));

	pm->dev = vpu_core->dev;
	dev = vpu_core->dev;

#if defined(CONFIG_COMMON_CLK_MT8168)
	pm->apu_sel = devm_clk_get(dev, "apu_sel");
	if (IS_ERR(pm->apu_sel)) {
		LOG_ERR("devm_clk_get apu_sel fail");
		ret = PTR_ERR(pm->apu_sel);
	}

	pm->apu_if_sel = devm_clk_get(dev,
				 "apu_if_sel");
	if (IS_ERR(pm->apu_if_sel)) {
		LOG_ERR("devm_clk_get apu_if_sel fail");
		ret = PTR_ERR(pm->apu_if_sel);
	}

	pm->clk26m_ck = devm_clk_get(dev, "clk26m_ck");
	if (IS_ERR(pm->clk26m_ck)) {
		LOG_ERR("devm_clk_get clk26m_ck fail");
		ret = PTR_ERR(pm->clk26m_ck);
	}

	pm->univpll_d2 = devm_clk_get(dev, "univpll_d2");
	if (IS_ERR(pm->univpll_d2)) {
		LOG_ERR("devm_clk_get univpll_d2 fail");
		ret = PTR_ERR(pm->univpll_d2);
	}

	pm->apupll_ck = devm_clk_get(dev, "apupll_ck");
	if (IS_ERR(pm->apupll_ck)) {
		LOG_ERR("devm_clk_get apupll_ck fail");
		ret = PTR_ERR(pm->apupll_ck);
	}

	pm->syspll_d3 = devm_clk_get(dev, "syspll_d3");
	if (IS_ERR(pm->syspll_d3)) {
		LOG_ERR("devm_clk_get syspll_d3 fail");
		ret = PTR_ERR(pm->syspll_d3);
	}

	pm->univpll1_d2 = devm_clk_get(dev, "univpll1_d2");
	if (IS_ERR(pm->univpll1_d2)) {
		LOG_ERR("devm_clk_get univpll1_d2 fail");
		ret = PTR_ERR(pm->univpll1_d2);
	}

	pm->syspll1_d2 = devm_clk_get(dev, "syspll1_d2");
	if (IS_ERR(pm->syspll1_d2)) {
		LOG_ERR("devm_clk_get syspll1_d2 fail");
		ret = PTR_ERR(pm->syspll1_d2);
	}

	pm->syspll1_d4 = devm_clk_get(dev, "syspll1_d4");
	if (IS_ERR(pm->syspll1_d4)) {
		LOG_ERR("devm_clk_get syspll1_d4 fail");
		ret = PTR_ERR(pm->syspll1_d4);
	}

	pm->ifr_apu_axi = devm_clk_get(dev, "ifr_apu_axi");
	if (IS_ERR(pm->ifr_apu_axi)) {
		LOG_ERR("devm_clk_get ifr_apu_axi fail");
		ret = PTR_ERR(pm->ifr_apu_axi);
	}

	pm->smi_cam = devm_clk_get(dev, "smi_cam");
	if (IS_ERR(pm->smi_cam)) {
		LOG_ERR("devm_clk_get smi_cam fail");
		ret = PTR_ERR(pm->smi_cam);
	}

	pm->apu_ipu_ck = devm_clk_get(dev, "apu_ipu_ck");
	if (IS_ERR(pm->apu_ipu_ck)) {
		LOG_ERR("devm_clk_get apu_ipu_ck fail");
		ret = PTR_ERR(pm->apu_ipu_ck);
	}

	pm->apu_axi = devm_clk_get(dev, "apu_axi");
	if (IS_ERR(pm->apu_axi)) {
		LOG_ERR("devm_clk_get apu_axi fail");
		ret = PTR_ERR(pm->apu_axi);
	}

	pm->apu_jtag = devm_clk_get(dev, "apu_jtag");
	if (IS_ERR(pm->apu_jtag)) {
		LOG_ERR("devm_clk_get apu_jtag fail");
		ret = PTR_ERR(pm->apu_jtag);
	}

	pm->apu_if_ck = devm_clk_get(dev, "apu_if_ck");
	if (IS_ERR(pm->apu_if_ck)) {
		LOG_ERR("devm_clk_get apu_if_ck fail");
		ret = PTR_ERR(pm->apu_if_ck);
	}

	pm->apu_edma = devm_clk_get(dev, "apu_edma");
	if (IS_ERR(pm->apu_edma)) {
		LOG_ERR("devm_clk_get apu_edma fail");
		ret = PTR_ERR(pm->apu_edma);
	}

	pm->apu_ahb = devm_clk_get(dev, "apu_ahb");
	if (IS_ERR(pm->apu_ahb)) {
		LOG_ERR("devm_clk_get apu_ahb fail");
		ret = PTR_ERR(pm->apu_ahb);
	}
#endif

#if defined(CONFIG_COMMON_CLK_MT8168)
	pm_runtime_enable(dev);
#endif

	return ret;
}

void vpu_release_pm(struct vpu_core *vpu_core)
{
#if defined(CONFIG_COMMON_CLK_MT8168)
	if (vpu_core->dev)
		pm_runtime_disable(vpu_core->dev);
#endif
}

void vpu_pw_on(struct vpu_pm *pm)
{
#if defined(CONFIG_COMMON_CLK_MT8168)
	int ret;

	if (pm->dev) {
		ret = pm_runtime_get_sync(pm->dev);
		if (ret < 0)
			LOG_ERR("pm_runtime_get_sync fail %d", ret);
	}
#endif
	vpu_clock_enable(pm);
}

void vpu_pw_off(struct vpu_pm *pm)
{
	int ret;

	vpu_clock_disable(pm);

#if defined(CONFIG_COMMON_CLK_MT8168)

	if (pm->dev) {
		ret = pm_runtime_put_sync(pm->dev);
		if (ret < 0)
			LOG_ERR("pm_runtime_put_sync fail %d", ret);
	}
#endif
}

void vpu_clock_enable(struct vpu_pm *pm)
{
	int ret;
#if defined(CONFIG_COMMON_CLK_MT8168)
	ret = clk_prepare_enable(pm->ifr_apu_axi);
	if (ret)
		LOG_ERR("clk_enable ifr_apu_axi fail %d", ret);

	ret = clk_prepare_enable(pm->smi_cam);
	if (ret)
		LOG_ERR("clk_enable smi_cam fail %d", ret);

	ret = clk_prepare_enable(pm->apu_ipu_ck);
	if (ret)
		LOG_ERR("clk_enable apu_ipu_ck fail %d", ret);

	ret = clk_prepare_enable(pm->apu_axi);
	if (ret)
		LOG_ERR("clk_enable apu_axi fail %d", ret);

	ret = clk_prepare_enable(pm->apu_jtag);
	if (ret)
		LOG_ERR("clk_enable apu_jtag fail %d", ret);

	ret = clk_prepare_enable(pm->apu_if_ck);
	if (ret)
		LOG_ERR("clk_enable apu_if_ck fail %d", ret);

	ret = clk_prepare_enable(pm->apu_edma);
	if (ret)
		LOG_ERR("clk_enable apu_edma fail %d", ret);

	ret = clk_prepare_enable(pm->apu_ahb);
	if (ret)
		LOG_ERR("clk_enable apu_ahb fail %d", ret);
#endif
}

void vpu_clock_disable(struct vpu_pm *pm)
{
#if defined(CONFIG_COMMON_CLK_MT8168)
	clk_disable_unprepare(pm->ifr_apu_axi);
	clk_disable_unprepare(pm->smi_cam);

	clk_disable_unprepare(pm->apu_ipu_ck);
	clk_disable_unprepare(pm->apu_axi);
	clk_disable_unprepare(pm->apu_jtag);
	clk_disable_unprepare(pm->apu_if_ck);
	clk_disable_unprepare(pm->apu_edma);
	clk_disable_unprepare(pm->apu_ahb);
#endif
}

int vpu_sel_src(struct vpu_pm *pm, uint8_t step)
{
	struct clk *clk_src;
	int ret = 0;

	LOG_INF("vpu scc(%d)", step);

	switch (step) {
	case 0:	/* 624MHz */
		clk_src = pm->univpll_d2;
		break;
	case 1:	/* 457MHz */
		clk_src = pm->apupll_ck;
		break;
	case 2:	/* 400MHz */
		clk_src = pm->apupll_ck;
		break;
	case 3:	/* 364MHz */
		clk_src = pm->syspll_d3;
		break;
	case 4:	/* 312MHz */
		clk_src = pm->univpll1_d2;
		break;
	case 5:	/* 273MHz */
		clk_src = pm->syspll1_d2;
		break;
	case 6:	/* 136MHz */
		clk_src = pm->syspll1_d4;
		break;
	case 7:	/* 26MHz */
		clk_src = pm->clk26m_ck;
		break;
	default:
		LOG_ERR("wrong freq step(%d)", step);
		return -EINVAL;
	}

	ret = clk_prepare_enable(clk_src);
	if (ret)
		LOG_ERR("clk_enable clk_src:%d fail %d", step, ret);

	if (step == 1)	/* 457Hz */
		clk_set_rate(clk_src, 457000000);

	if (step == 2)	/* 400Hz */
		clk_set_rate(clk_src, 400000000);

	ret = clk_set_parent(pm->apu_sel, clk_src);
	if (ret)
		LOG_ERR("clk_set_parent fail %d", ret);

	ret = clk_set_parent(pm->apu_if_sel, clk_src);
	if (ret)
		LOG_ERR("clk_set_parent fail %d", ret);

	clk_disable_unprepare(clk_src);

	return ret;
}


