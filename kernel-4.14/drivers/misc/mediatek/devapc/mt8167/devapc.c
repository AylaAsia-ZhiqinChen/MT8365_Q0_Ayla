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
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/types.h>

/* CCF */
#include <linux/clk.h>

#include "mtk_io.h"
#include "sync_write.h"
#include "devapc.h"

static struct clk *dapc_clk;
static struct cdev *g_devapc_ctrl;
static unsigned int devapc_irq;
static void __iomem *devapc_ao_base;
static void __iomem *devapc_pd_base;


/*****************************************************************************
 * FUNCTION DEFINITION
 *****************************************************************************/
static int devapc_ioremap(void);

/**************************************************************************
 * STATIC FUNCTION
 **************************************************************************/

static int devapc_ioremap(void)
{
	struct device_node *node = NULL;
	/*IO remap*/
	node = of_find_compatible_node(NULL, NULL, "mediatek,mt8167-devapc_ao");
	if (node) {
		devapc_ao_base = of_iomap(node, 0);
		pr_debug("[DEVAPC] AO_ADDRESS %p\n", devapc_ao_base);
	} else {
		pr_debug("[DEVAPC] can't find DAPC_AO compatible node\n");
		return -1;
	}

	node = of_find_compatible_node(NULL, NULL, "mediatek,mt8167-devapc");
	if (node) {
		devapc_pd_base = of_iomap(node, 0);
		devapc_irq = irq_of_parse_and_map(node, 0);

		pr_debug("[DEVAPC] PD_ADDRESS %p,", devapc_pd_base);
		pr_debug(" IRD: %d\n", devapc_irq);
	} else {
		pr_debug("[DEVAPC] can't find DAPC_PD compatible node\n");
		return -1;
	}

	return 0;
}

static int devapc_probe(struct platform_device *dev)
{
	pr_debug("[DEVAPC] module probe.\n");
	/*IO remap*/
	devapc_ioremap();

	/* turn on clock once */
	dapc_clk = devm_clk_get(&dev->dev, "main");
	if (IS_ERR(dapc_clk)) {
		pr_debug("[DEVAPC] cannot get dapc clock.\n");
		return PTR_ERR(dapc_clk);
	}
	clk_prepare_enable(dapc_clk);

	return 0;
}


static int devapc_remove(struct platform_device *dev)
{
	return 0;
}

static int devapc_suspend(struct platform_device *dev, pm_message_t state)
{
	return 0;
}

static int devapc_resume(struct platform_device *dev)
{
#ifdef CONFIG_MTK_IN_HOUSE_TEE_SUPPORT
	pr_debug("[DEVAPC] Enable TEE...module resume.\n");
#endif
	return 0;
}

#ifndef CONFIG_OF
struct platform_device devapc_device = {
	.name = "mt8167-devapc",
	.id = -1,
};
#endif

static const struct of_device_id mt_dapc_of_match[] = {
	{ .compatible = "mediatek,mt8167-devapc", },
	{/* sentinel */},
};

MODULE_DEVICE_TABLE(of, mt_dapc_of_match);

static struct platform_driver devapc_driver = {
	.probe = devapc_probe,
	.remove = devapc_remove,
	.suspend = devapc_suspend,
	.resume = devapc_resume,
	.driver = {
	.name = "mt8167-devapc",
	.owner = THIS_MODULE,
#ifdef CONFIG_OF
	.of_match_table = mt_dapc_of_match,
#endif
	},
};

/*
 * devapc_init: module init function.
 */
static int __init devapc_init(void)
{
	int ret;

	pr_debug("[DEVAPC] module init.\n");
#ifndef CONFIG_OF
	ret = platform_device_register(&devapc_device);
	if (ret) {
		pr_debug("[DEVAPC] Unable to do device register(%d)\n", ret);
		return ret;
	}
#endif

	ret = platform_driver_register(&devapc_driver);
	if (ret) {
		pr_debug("[DEVAPC] Unable to register driver (%d)\n", ret);
#ifndef CONFIG_OF
		platform_device_unregister(&devapc_device);
#endif
		return ret;
	}

	g_devapc_ctrl = cdev_alloc();
	if (!g_devapc_ctrl) {
		pr_debug("[DEVAPC] Failed to add devapc device! (%d)\n", ret);
		platform_driver_unregister(&devapc_driver);
#ifndef CONFIG_OF
		platform_device_unregister(&devapc_device);
#endif
		return ret;
	}
	g_devapc_ctrl->owner = THIS_MODULE;

	return 0;
}

/*
 * devapc_exit: module exit function.
 */
static void __exit devapc_exit(void)
{
	pr_debug("[DEVAPC] DEVAPC module exit\n");

}

late_initcall(devapc_init);
module_exit(devapc_exit);
MODULE_LICENSE("GPL");
