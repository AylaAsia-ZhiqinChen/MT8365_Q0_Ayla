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

#include <linux/platform_device.h>
#include <linux/of_platform.h>

#include "vpu_cmn.h"
#include "vpu_reg.h"
#include "vpu_if.h"

int vpu_set_internal_ram(struct platform_device *pdev, bool enable)
{
	struct vpu_device *vpu_device;
	int ret = 0;

	vpu_device = platform_get_drvdata(pdev);
	if (!vpu_device)
		return -ESRCH;

	if (enable) {
		ret = vpu_get_power(vpu_device->vpu_core[0], false);
		if (ret)
			LOG_ERR("[vpu]fail to get power!\n");
	} else
		vpu_put_power(vpu_device->vpu_core[0], VPT_ENQUE_ON);

	return ret;
}
EXPORT_SYMBOL_GPL(vpu_set_internal_ram);

struct platform_device *vpu_get_plat_device(void)
{
	struct device_node *vpu_node;
	struct platform_device *vpu_pdev;

	vpu_node = of_find_compatible_node(NULL, NULL, "mediatek,mt8168-vpu");
	if (!vpu_node) {
		LOG_ERR("can't get vpu node\n");
		return NULL;
	}

	vpu_pdev = of_find_device_by_node(vpu_node);
	if (WARN_ON(!vpu_pdev)) {
		LOG_ERR("vpu pdev failed\n");
		of_node_put(vpu_node);
		return NULL;
	}

	return vpu_pdev;
}
EXPORT_SYMBOL_GPL(vpu_get_plat_device);

