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

#ifndef _VPU_PM_H_
#define _VPU_PM_H_

#include "vpu_cmn.h"

int vpu_init_pm(struct vpu_core *vpu_core);
void vpu_release_pm(struct vpu_core *vpu_core);

void vpu_pw_on(struct vpu_pm *pm);
void vpu_pw_off(struct vpu_pm *pm);
void vpu_clock_enable(struct vpu_pm *pm);
void vpu_clock_disable(struct vpu_pm *pm);
int vpu_sel_src(struct vpu_pm *pm, uint8_t step);

#endif /* _VPU_PM_H_ */
