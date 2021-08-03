/*
 * Copyright (C) 2018 MediaTek Inc.
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

#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>
#include "adsp_helper.h"
#include "mtk_hifixdsp_common.h"
#include "adsp_reg.h"
#include "mtk_chip.h"

#define DRV_NAME		"mtk-dsp_wdt"

#define DSP_WDT_MODE_EN			(1U << 0)
#define DSP_WDT_MODE_EXT_POL_LOW	(0U << 1)
#define DSP_WDT_MODE_EXT_POL_HIGH	(1U << 1)
#define DSP_WDT_MODE_EXRST_EN		(1U << 2)
#define DSP_WDT_MODE_IRQ_EN		(1U << 3)
#define DSP_WDT_MODE_IRQ_LVL		(1U << 5)
#define DSP_WDT_MODE_DUAL_EN		(1U << 6)
#define DSP_WDT_MODE_KEY		0x22000000U

struct mtk_dsp_wdt_dev {
	void __iomem *dsp_wdt_base;
	unsigned int dsp_wdt_irq_id;
	struct workqueue_struct *dsp_wdt_queue;
	struct work_struct dsp_wdt_work;
};

static struct mtk_dsp_wdt_dev *mtk_dsp_wdt;

BLOCKING_NOTIFIER_HEAD(wdt_notifier_list);


static void dump_adsp_log_buf(void *buf)
{
	int i;
	char tmp_buf[1024];
	char *log_buf = NULL;
	u32 log_size = 0;
	u32 tmp_log_start, tmp_log_size;
	struct hifi4dsp_log_ctrl *log_ctrl;

	log_ctrl = (struct hifi4dsp_log_ctrl *)buf;

	if (log_ctrl->magic != DSP_LOG_BUF_MAGIC) {
		pr_info("ADSP log buf is invalid!\n");
		return;
	}

	if (log_ctrl->full)
		log_size = log_ctrl->size;
	else
		log_size = log_ctrl->offset;

	log_buf = (char *)buf + sizeof(struct hifi4dsp_log_ctrl);

	pr_info("==========================\n");
	pr_info("=== dump dsp log start ===\n");
	pr_info("==========================\n");
	for (i = 0, tmp_log_start = 0; i < log_size; i++) {
		if (log_buf[i] == '\n' || i == log_size - 1) {
			tmp_log_size = i - tmp_log_start + 1;
			strncpy(tmp_buf, log_buf + tmp_log_start, tmp_log_size);
			tmp_buf[tmp_log_size] = '\0';
			pr_info("%s", tmp_buf);
			tmp_log_start = i + 1;
		}
	}
	pr_info("==========================\n");
	pr_info("=== dump dsp log end  ====\n");
	pr_info("==========================\n");
}

static int
dbg_notify_show_adsp_log(struct notifier_block *this,
				 unsigned long code, void *unused)
{
	struct adsp_chip_info *adsp = get_adsp_chip_data();

	if (adsp && adsp->adsp_bootup_done)
		dump_adsp_log_buf(adsp->shared_dtcm);
	else
		pr_info("ADSP A is not ready!\n");

	return NOTIFY_DONE;
}

static struct notifier_block dbg_show_log_notifier = {
	.notifier_call  = dbg_notify_show_adsp_log,
	.priority       = 100,
};

static irqreturn_t mtk_dsp_wdt_isr(int irq, void *dev_id)
{
	u32 reg;

	/*clear irq */
	reg = readl(DSP_WDT_MODE);
	reg |= DSP_WDT_MODE_KEY;
	writel(reg, DSP_WDT_MODE);

	queue_work(mtk_dsp_wdt->dsp_wdt_queue, &mtk_dsp_wdt->dsp_wdt_work);

	return IRQ_HANDLED;
}

void dsp_wdt_work_handler(struct work_struct *unused)
{
	pr_notice("[%s] ADSP happens exception!\n", __func__);

	blocking_notifier_call_chain(&wdt_notifier_list, 0, NULL);
}

void adsp_wdt_stop(void)
{
	uint32_t reg;

	reg = readl(DSP_WDT_MODE);
	reg &= ~DSP_WDT_MODE_EN;
	reg |= DSP_WDT_MODE_KEY;
	writel(reg, DSP_WDT_MODE);
	pr_notice("[%s] wdt mode:0x%x\n", __func__, readl(DSP_WDT_MODE));
}

int adsp_wdt_device_init(struct platform_device *pdev)
{
	int err;

	mtk_dsp_wdt = devm_kzalloc(&pdev->dev,
				sizeof(*mtk_dsp_wdt), GFP_KERNEL);
	if (!mtk_dsp_wdt)
		return -ENOMEM;

	mtk_dsp_wdt->dsp_wdt_irq_id = platform_get_irq(pdev, 1);
	if (mtk_dsp_wdt->dsp_wdt_irq_id == -ENXIO) {
		pr_err("dsp wdt get IRQ ID failed\n");
		return -EINVAL;
	}

	mtk_dsp_wdt->dsp_wdt_queue =
		create_singlethread_workqueue("dsp_wdt_kworker");
	INIT_WORK(&mtk_dsp_wdt->dsp_wdt_work, dsp_wdt_work_handler);

	/* 8168E2 IC ID:0xCA01 */
	if (mt_get_chip_hw_ver() == 0xCA01) {
		err = request_irq(mtk_dsp_wdt->dsp_wdt_irq_id,
			mtk_dsp_wdt_isr, IRQF_TRIGGER_NONE,
			DRV_NAME, mtk_dsp_wdt);
		if (err != 0) {
			pr_err("%s : failed to request irq (%d)\n",
				__func__, err);
			return err;
		}
	}

	register_adsp_wdt_notifier(&dbg_show_log_notifier);

	return 0;
}

int adsp_wdt_device_remove(struct platform_device *pdev)
{
	destroy_workqueue(mtk_dsp_wdt->dsp_wdt_queue);
	unregister_adsp_wdt_notifier(&dbg_show_log_notifier);
	devm_kfree(&pdev->dev, mtk_dsp_wdt);
	mtk_dsp_wdt = NULL;
	return 0;
}

int register_adsp_wdt_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_register(&wdt_notifier_list, nb);
}

int unregister_adsp_wdt_notifier(struct notifier_block *nb)
{
	return blocking_notifier_chain_unregister(&wdt_notifier_list, nb);
}


