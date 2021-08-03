/*
 * mt8168-afe-utils.h  --  Mediatek 8168 audio utility
 *
 * Copyright (c) 2018 MediaTek Inc.
 * Author: Jia Zeng <jia.zeng@mediatek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _MT8168_AFE_UTILS_H_
#define _MT8168_AFE_UTILS_H_

struct mtk_base_afe;
struct clk;

int mt8168_afe_init_audio_clk(struct mtk_base_afe *afe);

int mt8168_afe_enable_clk(struct mtk_base_afe *afe, struct clk *clk);

void mt8168_afe_disable_clk(struct mtk_base_afe *afe, struct clk *clk);

int mt8168_afe_set_clk_rate(struct mtk_base_afe *afe, struct clk *clk,
	unsigned int rate);

int mt8168_afe_set_clk_parent(struct mtk_base_afe *afe, struct clk *clk,
	struct clk *parent);

int mt8168_afe_enable_top_cg(struct mtk_base_afe *afe, unsigned int cg_type);

int mt8168_afe_disable_top_cg(struct mtk_base_afe *afe, unsigned int cg_type);

int mt8168_afe_enable_main_clk(struct mtk_base_afe *afe);

int mt8168_afe_disable_main_clk(struct mtk_base_afe *afe);

int mt8168_afe_emi_clk_on(struct mtk_base_afe *afe);

int mt8168_afe_emi_clk_off(struct mtk_base_afe *afe);

int mt8168_afe_enable_afe_on(struct mtk_base_afe *afe);

int mt8168_afe_disable_afe_on(struct mtk_base_afe *afe);

int mt8168_afe_enable_apll_tuner_cfg(struct mtk_base_afe *afe,
	unsigned int apll);

int mt8168_afe_disable_apll_tuner_cfg(struct mtk_base_afe *afe,
	unsigned int apll);

int mt8168_afe_enable_apll_associated_cfg(struct mtk_base_afe *afe,
	unsigned int apll);

int mt8168_afe_disable_apll_associated_cfg(struct mtk_base_afe *afe,
	unsigned int apll);
#endif
