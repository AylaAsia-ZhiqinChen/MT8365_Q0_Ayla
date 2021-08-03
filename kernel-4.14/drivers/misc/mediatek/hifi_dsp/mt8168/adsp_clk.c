/*
 * Copyright (C) 2018 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#include <linux/pm_runtime.h>
#include <linux/clk.h>
#include "adsp_clk.h"
#include "adsp_helper.h"


struct clk *clk_handle[ADSP_CLK_NUM];


int platform_parse_clock(struct device *dev, void *data)
{
	/* Parse CLK */
	clk_handle[CLK_TOP_DSP_SEL] = devm_clk_get(dev, "CLK_DSP_SEL");
	if (IS_ERR(clk_handle[CLK_TOP_DSP_SEL])) {
		dev_err(dev, "clk_get(\"CLK_DSP_SEL\") failed\n");
		return PTR_ERR(clk_handle[CLK_TOP_DSP_SEL]);
	}
	clk_handle[CLK_TOP_CLK26M] = devm_clk_get(dev, "CLK26M_CK");
	if (IS_ERR(clk_handle[CLK_TOP_CLK26M])) {
		dev_err(dev, "clk_get(\"CLK26M_CK\") failed\n");
		return PTR_ERR(clk_handle[CLK_TOP_CLK26M]);
	}
	clk_handle[CLK_TOP_CLK13M] = devm_clk_get(dev, "AD_SYS_26M_D2");
	if (IS_ERR(clk_handle[CLK_TOP_CLK13M])) {
		dev_err(dev, "clk_get(\"AD_SYS_26M_D2\") failed\n");
		return PTR_ERR(clk_handle[CLK_TOP_CLK13M]);
	}
	clk_handle[CLK_TOP_DSPPLL] = devm_clk_get(dev, "DSPPLL_CK");
	if (IS_ERR(clk_handle[CLK_TOP_DSPPLL])) {
		dev_err(dev, "clk_get(\"DSPPLL_CK\") failed\n");
		return PTR_ERR(clk_handle[CLK_TOP_DSPPLL]);
	}
	clk_handle[CLK_TOP_DSPPLL_D2] = devm_clk_get(dev, "DSPPLL_D2");
	if (IS_ERR(clk_handle[CLK_TOP_DSPPLL_D2])) {
		dev_err(dev, "clk_get(\"DSPPLL_D2\") failed\n");
		return PTR_ERR(clk_handle[CLK_TOP_DSPPLL_D2]);
	}
	clk_handle[CLK_TOP_DSPPLL_D4] = devm_clk_get(dev, "DSPPLL_D4");
	if (IS_ERR(clk_handle[CLK_TOP_DSPPLL_D4])) {
		dev_err(dev, "clk_get(\"DSPPLL_D4\") failed\n");
		return PTR_ERR(clk_handle[CLK_TOP_DSPPLL_D4]);
	}
	clk_handle[CLK_TOP_DSPPLL_D8] = devm_clk_get(dev, "DSPPLL_D8");
	if (IS_ERR(clk_handle[CLK_TOP_DSPPLL_D8])) {
		dev_err(dev, "clk_get(\"DSPPLL_D8\") failed\n");
		return PTR_ERR(clk_handle[CLK_TOP_DSPPLL_D8]);
	}
	clk_handle[CLK_TOP_DSP_26M] = devm_clk_get(dev, "PDN_DSP_26M");
	if (IS_ERR(clk_handle[CLK_TOP_DSP_26M])) {
		dev_err(dev, "clk_get(\"PDN_DSP_26M\") failed\n");
		return PTR_ERR(clk_handle[CLK_TOP_DSP_26M]);
	}
	clk_handle[CLK_TOP_DSP_32K] = devm_clk_get(dev, "PDN_DSP_32K");
	if (IS_ERR(clk_handle[CLK_TOP_DSP_32K])) {
		dev_err(dev, "clk_get(\"PDN_DSP_32K\") failed\n");
		return PTR_ERR(clk_handle[CLK_TOP_DSP_32K]);
	}
	clk_handle[CLK_IFR_DSP_UART] = devm_clk_get(dev, "IFR_DSP_UART");
	if (IS_ERR(clk_handle[CLK_IFR_DSP_UART])) {
		dev_err(dev, "clk_get(\"IFR_DSP_UART\") failed\n");
		return PTR_ERR(clk_handle[CLK_IFR_DSP_UART]);
	}

	return 0;
}

int adsp_enable_clock(struct device *dev)
{
	int ret = 0;

	ret = clk_prepare_enable(clk_handle[CLK_TOP_DSP_SEL]);
	if (ret) {
		dev_err(dev, "%s clk_prepare_enable(CLK_DSP_SEL) fail %d\n",
		       __func__, ret);
		return ret;
	}
	ret = clk_prepare_enable(clk_handle[CLK_TOP_DSP_26M]);
	if (ret) {
		dev_err(dev, "%s clk_prepare_enable(PDN_DSP_26M) fail %d\n",
		       __func__, ret);
		return ret;
	}
	ret = clk_prepare_enable(clk_handle[CLK_TOP_DSP_32K]);
	if (ret) {
		dev_err(dev, "%s clk_prepare_enable(PDN_DSP_32K) fail %d\n",
		       __func__, ret);
		return ret;
	}
	ret = clk_prepare_enable(clk_handle[CLK_IFR_DSP_UART]);
	if (ret) {
		dev_err(dev, "%s clk_prepare_enable(IFR_DSP_UART) fail %d\n",
		       __func__, ret);
		return ret;
	}

	return ret;
}

void adsp_disable_clock(struct device *dev)
{
	clk_disable_unprepare(clk_handle[CLK_IFR_DSP_UART]);
	clk_disable_unprepare(clk_handle[CLK_TOP_DSP_32K]);
	clk_disable_unprepare(clk_handle[CLK_TOP_DSP_26M]);
	clk_disable_unprepare(clk_handle[CLK_TOP_DSP_SEL]);
}

int adsp_set_dsppll_rate(enum ADSP_PLL_FREQ freq)
{
	int ret = 0;

	ret = clk_prepare_enable(clk_handle[CLK_TOP_DSPPLL]);
	if (ret) {
		pr_err("[ADSP] %s clk_prepare_enable(DSPPLL_CK) fail %d\n",
		       __func__, ret);
		return ret;
	}
	ret = clk_set_rate(clk_handle[CLK_TOP_DSPPLL], freq);
	clk_disable_unprepare(clk_handle[CLK_TOP_DSPPLL]);

	pr_notice("[ADSP] set DSPPLL freq = %d done.\n", freq);
	return ret;
}

int adsp_set_top_mux(int enable, enum ADSP_CLK_ID clk)
{
	int ret = 0;

	pr_debug("%s(%d, %x)\n", __func__, enable, clk);

	if (enable) {
		switch (clk) {
		case CLK_TOP_CLK26M:
			ret = clk_set_parent(clk_handle[CLK_TOP_DSP_SEL],
					     clk_handle[CLK_TOP_CLK26M]);
			break;
		case CLK_TOP_CLK13M:
			ret = clk_set_parent(clk_handle[CLK_TOP_DSP_SEL],
					     clk_handle[CLK_TOP_CLK13M]);
			break;
		case CLK_TOP_DSPPLL:
			ret = clk_set_parent(clk_handle[CLK_TOP_DSP_SEL],
					     clk_handle[CLK_TOP_DSPPLL]);
			break;
		case CLK_TOP_DSPPLL_D2:
			ret = clk_set_parent(clk_handle[CLK_TOP_DSP_SEL],
					     clk_handle[CLK_TOP_DSPPLL_D2]);
			break;
		case CLK_TOP_DSPPLL_D4:
			ret = clk_set_parent(clk_handle[CLK_TOP_DSP_SEL],
					     clk_handle[CLK_TOP_DSPPLL_D4]);
				break;
		case CLK_TOP_DSPPLL_D8:
			ret = clk_set_parent(clk_handle[CLK_TOP_DSP_SEL],
					     clk_handle[CLK_TOP_DSPPLL_D8]);
			break;
		default:
			ret = -1;
			break;
		}
		if (ret) {
			pr_err("[ADSP] %s clk_set_parent(CLK_DSP_SEL-%x) fail %d\n",
			      __func__, clk, ret);
			goto TAIL;
		}
	}

TAIL:
	return ret;
}

int adsp_default_clk_init(struct device *dev, int enable)
{
	int ret = 0;

	pr_debug("+%s (%x)\n", __func__, enable);

	if (enable) {
		ret = adsp_set_top_mux(1, CLK_TOP_CLK26M);
		if (ret) {
			dev_err(dev, "failed to adsp_set_top_mux: %d\n", ret);
			goto TAIL;
		}
		ret = adsp_enable_clock(dev);
		if (ret) {
			dev_err(dev, "failed to adsp_enable_clock: %d\n", ret);
			goto TAIL;
		}
	} else
		adsp_disable_clock(dev);

	pr_debug("-%s (%x)\n", __func__, enable);

TAIL:
	return ret;
}

int adsp_pm_register_early(struct device *dev)
{
	int ret;

	/* Fix DSPPLL switch-issue before pm_runtime_enable */
	ret = adsp_set_top_mux(1, CLK_TOP_CLK26M);
	if (ret)
		goto TAIL;
	pm_runtime_enable(dev);
TAIL:
	return ret;
}

void adsp_pm_unregister_last(struct device *dev)
{
	pm_runtime_disable(dev);
}

int adsp_power_enable(struct device *dev)
{
	int ret;

	ret = pm_runtime_get_sync(dev);

	return ret;
}

void adsp_power_disable(struct device *dev)
{
	pm_runtime_put_sync(dev);
}

