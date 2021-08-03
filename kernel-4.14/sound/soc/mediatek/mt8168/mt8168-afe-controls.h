/*
 * mt8168-afe-controls.h  --  Mediatek 8168 audio controls
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

#ifndef _MT8168_AFE_CONTROLS_H_
#define _MT8168_AFE_CONTROLS_H_

struct snd_soc_platform;

int mt8168_afe_add_controls(struct snd_soc_platform *platform);

#endif
