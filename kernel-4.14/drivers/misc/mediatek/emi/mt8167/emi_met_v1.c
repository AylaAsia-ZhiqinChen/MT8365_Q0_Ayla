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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/printk.h>

#include <mt-plat/mtk_io.h>
#include "emi_met_v1.h"

#define EMI_NOT_FINE_NODE	-1
#define EMI_NOT_IOMAP		-2

static void __iomem *CEN_EMI_BASE;

/*
 * emi_met_init: module init function.
 */
static int __init emi_met_init(void)
{
	int ret = 0;
	struct device_node *node;

	node = of_find_compatible_node(NULL, NULL, "mediatek,mt8167-emi");
	if (!node) {
		pr_info("[EMI] cannot find node mediatek,mt8167-emi\n");
		return EMI_NOT_FINE_NODE;
	}

	CEN_EMI_BASE = of_iomap(node, 0);
	if (!CEN_EMI_BASE) {
		pr_info("[EMI] cannot iomap mediatek,mt8167-emi");
		return EMI_NOT_IOMAP;
	}

	pr_info("[EMI] get CEN_EMI_BASE @ %px\n", mt_cen_emi_base_get());
	return ret;
}

/*
 * emi_ctrl_exit: module exit function.
 */
static void __exit emi_met_exit(void)
{
}

postcore_initcall(emi_met_init);
module_exit(emi_met_exit);



void __iomem *mt_cen_emi_base_get(void)
{
	return CEN_EMI_BASE;
}
EXPORT_SYMBOL(mt_cen_emi_base_get);


