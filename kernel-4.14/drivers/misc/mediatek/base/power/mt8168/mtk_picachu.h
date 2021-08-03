/*
 * Copyright (C) 2017 MediaTek Inc.
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
#ifndef __MTK_PICACHU_H__
#define  __MTK_PICACHU_H__

//#define PICACHU_ENABLE_LOO
//#define PICACHU_DUBUG

enum picachu_enum {
	PICACHU_CLUSTER_L_VMIN_OPP0 = 0,
#ifdef PICACHU_ENABLE_LOO
	PICACHU_CLUSTER_L_VMIN_OPP8,
#endif
	PICACHU_MAX_NUM
};

struct picachu_data {
	unsigned char valid;
	unsigned char BDES;
	unsigned char MDES;
	unsigned char MTDES;
#ifdef PICACHU_DUBUG
	unsigned int vmin_voltage; /* unit: 10uV*/
	unsigned int vmin_pmic_step;
#endif
};

int get_picachu_calibrate_data(
	enum picachu_enum id,
	struct picachu_data *picachu_data);

#endif
