/*
 * Copyright (C) 2019 MediaTek Inc.
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

#ifndef _UPMU_HW_H_
#define _UPMU_HW_H_

#ifdef CONFIG_MTK_PMIC_CHIP_MT6357
#include "upmu_hw_mt6357.h"
#endif

#ifdef CONFIG_MTK_PMIC_CHIP_MT6390
#include "upmu_hw_mt6390.h"
#endif

#endif		/* _UPMU_HW_H_ */
