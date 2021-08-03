/*
 * Copyright (C) 2016 MediaTek Inc.
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

#ifndef _VPU_DVFS_H_
#define _VPU_DVFS_H_


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* |    opp_index   |   vpu frequency    |        power            */
/* ----------------------------------------------------------------*/
/* |      0         |   624 MHz          |        466 mA           */
/* ----------------------------------------------------------------*/
/* |      1         |   457 MHz          |        342 mA           */
/* ----------------------------------------------------------------*/
/* |      2         |   400 MHz          |        230 mA           */
/* ----------------------------------------------------------------*/
/* |      3         |   364 MHz          |        209 mA           */
/* ----------------------------------------------------------------*/
/* |      4         |   312 MHz          |        155 mA           */
/* ----------------------------------------------------------------*/
/* |      5         |   273 MHz          |        136 mA           */
/* ----------------------------------------------------------------*/
/* |      6         | 136.5 MHz          |         68 mA           */
/* ----------------------------------------------------------------*/
/* |      7         |    26 MHz          |         13 mA           */
/* ----------------------------------------------------------------*/
/* ++++++++++++++++++++++++++++++++++*/

enum VPU_OPP_INDEX {
	VPU_OPP_0 = 0,
	VPU_OPP_1 = 1,
	VPU_OPP_2 = 2,
	VPU_OPP_3 = 3,
	VPU_OPP_4 = 4,
	VPU_OPP_5 = 5,
	VPU_OPP_6 = 6,
	VPU_OPP_7 = 7,
	VPU_OPP_NUM
};

struct VPU_OPP_INFO {
	enum VPU_OPP_INDEX opp_index;
	int power;	/*mW*/
};

extern struct VPU_OPP_INFO vpu_power_table[VPU_OPP_NUM];
extern int32_t vpu_thermal_en_throttle_cb(uint8_t vcore_opp, uint8_t vpu_opp);
extern int32_t vpu_thermal_dis_throttle_cb(void);
extern int get_vpu_opp(void);
extern int get_vpu_platform_floor_opp(void);
extern int get_vpu_ceiling_opp(int core);
extern int get_vpu_opp_to_freq(uint8_t step);
void vpu_enable_mtcmos(void);
void vpu_disable_mtcmos(void);
int get_vpu_init_done(void);


#endif
