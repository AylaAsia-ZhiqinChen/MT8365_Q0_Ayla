/*
 * Copyright (C) 2018 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#ifndef ADSP_CLK_H
#define ADSP_CLK_H


enum ADSP_PLL_FREQ {
	ADSPPLL_FREQ_600MHZ     = 600000000,
	ADSPPLL_FREQ_400MHZ     = 400000000,
	ADSPPLL_FREQ_312MHZ     = 312000000
};

enum ADSP_CLK_ID {
	CLK_TOP_DSP_SEL,
	CLK_TOP_CLK26M,
	CLK_TOP_CLK13M,
	CLK_TOP_DSPPLL,
	CLK_TOP_DSPPLL_D2,
	CLK_TOP_DSPPLL_D4,
	CLK_TOP_DSPPLL_D8,
	CLK_TOP_DSP_26M,
	CLK_TOP_DSP_32K,
	CLK_IFR_DSP_UART,
	ADSP_CLK_NUM
};

extern int platform_parse_clock(struct device *dev, void *data);
extern int adsp_default_clk_init(struct device *dev, int enable);
extern int adsp_pm_register_early(struct device *dev);
extern void adsp_pm_unregister_last(struct device *dev);
extern int adsp_power_enable(struct device *dev);
extern void adsp_power_disable(struct device *dev);

#endif /* ADSP_CLK_H */
