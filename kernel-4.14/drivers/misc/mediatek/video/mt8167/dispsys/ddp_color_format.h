/*
 * Copyright (C) 2016 MediaTek Inc.
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

#ifndef __H_DDP_COLOR_FORMAT__
#define __H_DDP_COLOR_FORMAT__

#include "ddp_info.h"

int fmt_bpp(enum DP_COLOR_ENUM fmt);
int fmt_swap(enum DP_COLOR_ENUM fmt);
int fmt_color_space(enum DP_COLOR_ENUM fmt);
int fmt_is_yuv422(enum DP_COLOR_ENUM fmt);
int fmt_is_yuv420(enum DP_COLOR_ENUM fmt);
int fmt_hw_value(enum DP_COLOR_ENUM fmt);
char *fmt_string(enum DP_COLOR_ENUM fmt);
enum DP_COLOR_ENUM fmt_type(int unique, int swap);

#endif
