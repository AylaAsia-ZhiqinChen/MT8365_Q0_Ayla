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

#ifndef _VPU_IF_H_
#define _VPU_IF_H_

struct platform_device *vpu_get_plat_device(void);
int vpu_set_internal_ram(struct platform_device *pdev, bool enable);

#endif
