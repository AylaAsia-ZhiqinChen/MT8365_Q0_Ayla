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

#include <linux/io.h>
#include <linux/sysfs.h>
#include "adsp_reg.h"
#include "adsp_helper.h"
#include "mtk_hifixdsp_common.h"


static ssize_t adsp_A_reg_status_show(struct device *kobj,
		struct device_attribute *attr, char *buf)
{
	int len = 0;
	struct adsp_chip_info *adsp = get_adsp_chip_data();

	if (adsp && adsp->adsp_bootup_done) {
		len += scnprintf(buf + len, PAGE_SIZE - len,
				"[ADSP] DSP_PDEBUGPC: 0x%08x\n",
				readl(DSP_PDEBUGPC));
		len += scnprintf(buf + len, PAGE_SIZE - len,
				 "[ADSP] DSP_RESERVED_1: 0x%08x\n",
				 readl(DSP_RESERVED_1));
		len += scnprintf(buf + len, PAGE_SIZE - len,
				 "[ADSP] DSP_RESET_SW: 0x%08x\n",
				 readl(DSP_RESET_SW));

		return len;
	} else
		return scnprintf(buf, PAGE_SIZE, "ADSP A is not ready!\n");
}

DEVICE_ATTR(adsp_A_reg_status, 0444, adsp_A_reg_status_show, NULL);

static ssize_t adsp_tiny_dbg_buffer_show(struct device *kobj,
		struct device_attribute *attr, char *buf)
{
/*|---IPC 768B (adsp_info) ---+---TINY 256B (debug buffer) ---|*/
#define ADSP_TINY_DEBUG_SIZE (0x0100)
#define SIZE_SHARED_OFF_FROM_IPC      (0x0300)
	int len = 0;
	int i = 0;
	unsigned long base = adsp_get_shared_dtcm_virt_for_ipc();
	unsigned int *tiny_base;
	struct adsp_chip_info *adsp = get_adsp_chip_data();

	if (adsp && adsp->adsp_bootup_done) {
		tiny_base = (void *)(base + SIZE_SHARED_OFF_FROM_IPC);
		len += scnprintf(buf + len, PAGE_SIZE - len,
				"[ADSP] TINY_DBG_buffer(256Bytes):\n");
		for (i = 0; i < ADSP_TINY_DEBUG_SIZE / sizeof(u32); i++) {
			len += scnprintf(buf + len, PAGE_SIZE - len,
				 "0x%08x ",
				 readl(tiny_base + i));
			if (i != 0 && (i + 1) % 8 == 0)
				len += scnprintf(buf + len, PAGE_SIZE - len,
					"\n");
		}
		return len;
	} else
		return scnprintf(buf, PAGE_SIZE, "ADSP A is not ready!\n");
#undef SIZE_SHARED_OFF_FROM_IPC
#undef ADSP_TINY_DEBUG_SIZE
}

DEVICE_ATTR(adsp_tiny_dbg_show, 0444, adsp_tiny_dbg_buffer_show, NULL);

static ssize_t adsp_A_logger_show(struct device *kobj,
		struct device_attribute *attr, char *buf)
{
	u32 i;
	int len = 0;
	struct adsp_chip_info *adsp = get_adsp_chip_data();
	void __iomem *vbase;
	struct hifi4dsp_log_ctrl log_ctrl;
	u32 log_len;
	char *log_str;

	if (adsp && adsp->adsp_bootup_done) {
		vbase = adsp->shared_dtcm;
		log_ctrl = *(struct hifi4dsp_log_ctrl *)vbase;

		if (log_ctrl.magic != DSP_LOG_BUF_MAGIC)
			return scnprintf(buf, PAGE_SIZE,
				"ADSP log buf is invalid!\n");

		if (log_ctrl.full)
			log_len = log_ctrl.size;
		else
			log_len = log_ctrl.offset;

		log_str = (char *)vbase + sizeof(struct hifi4dsp_log_ctrl);

		len += scnprintf(buf + len, PAGE_SIZE - len,
				"==========================\n");
		len += scnprintf(buf + len, PAGE_SIZE - len,
				"=== dump dsp log start ===\n");
		len += scnprintf(buf + len, PAGE_SIZE - len,
				"==========================\n");

		for (i = 0; i < log_len; i++)
			len += scnprintf(buf + len, PAGE_SIZE - len,
				 "%c", *(log_str + i));

		len += scnprintf(buf + len, PAGE_SIZE - len,
				"\n==========================\n");
		len += scnprintf(buf + len, PAGE_SIZE - len,
				"=== dump dsp log end  ====\n");
		len += scnprintf(buf + len, PAGE_SIZE - len,
				"==========================\n");

		return len;
	} else
		return scnprintf(buf, PAGE_SIZE, "ADSP A is not ready!\n");
}

DEVICE_ATTR(adsp_logger_show, 0444, adsp_A_logger_show, NULL);

static ssize_t adsp_poweron_test(struct device *kobj,
		struct device_attribute *attr, char *buf)
{
	async_load_hifixdsp_bin_and_run(NULL, NULL);
	return 0;
}

DEVICE_ATTR(adsp_poweron, 0444, adsp_poweron_test, NULL);

static ssize_t adsp_shutdown_test(struct device *kobj,
		struct device_attribute *attr, char *buf)
{
	hifixdsp_stop_run();
	return 0;
}

DEVICE_ATTR(adsp_shutdown, 0444, adsp_shutdown_test, NULL);


 /*
  * create /sys files
  * @return:	 0: success, otherwise: fail
  */
int adsp_create_sys_files(struct device *dev)
{
	int ret = 0;

	if (!dev)
		return -EINVAL;

	ret = device_create_file(dev,
			&dev_attr_adsp_A_reg_status);
	if (ret)
		goto TAIL;
	ret = device_create_file(dev,
			&dev_attr_adsp_tiny_dbg_show);
	if (ret)
		goto TAIL;
	ret = device_create_file(dev,
			&dev_attr_adsp_logger_show);
	if (ret)
		goto TAIL;
	ret = device_create_file(dev,
			&dev_attr_adsp_poweron);
	if (ret)
		goto TAIL;
	ret = device_create_file(dev,
			&dev_attr_adsp_shutdown);
	if (ret)
		goto TAIL;

TAIL:
	return ret;
}

void adsp_destroy_sys_files(struct device *dev)
{
	device_remove_file(dev, &dev_attr_adsp_shutdown);
	device_remove_file(dev, &dev_attr_adsp_poweron);
	device_remove_file(dev, &dev_attr_adsp_logger_show);
	device_remove_file(dev, &dev_attr_adsp_tiny_dbg_show);
	device_remove_file(dev, &dev_attr_adsp_A_reg_status);
}

