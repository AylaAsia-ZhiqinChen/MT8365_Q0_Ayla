/*
 * Copyright (C) 2015 MediaTek Inc.
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

#ifndef _KD_CAMERA_HW_H_
#define _KD_CAMERA_HW_H_


#include <linux/types.h>
#include "kd_camera_typedef.h"


#if 0				/*defined CONFIG_MTK_LEGACY */
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>
#include "pmic_drv.h"
/*  */
/* Analog */
#define CAMERA_POWER_VCAM_A         PMIC_APP_MAIN_CAMERA_POWER_A
/* Digital */
#define CAMERA_POWER_VCAM_D         PMIC_APP_MAIN_CAMERA_POWER_D
/* AF */
#define CAMERA_POWER_VCAM_AF        PMIC_APP_MAIN_CAMERA_POWER_AF
/* digital io */
#define CAMERA_POWER_VCAM_IO        PMIC_APP_MAIN_CAMERA_POWER_IO
/* Digital for Sub */
#define SUB_CAMERA_POWER_VCAM_D     PMIC_APP_SUB_CAMERA_POWER_D


/* FIXME, should defined in DCT tool */

/* Main sensor */
    /* Common phone's reset pin uses extension GPIO10 of mt6306 */
    #define CAMERA_CMRST_PIN            GPIO_CAMERA_CMRST_PIN
    #define CAMERA_CMRST_PIN_M_GPIO     GPIO_CAMERA_CMRST_PIN_M_GPIO


#define CAMERA_CMPDN_PIN            GPIO_CAMERA_CMPDN_PIN
#define CAMERA_CMPDN_PIN_M_GPIO     GPIO_CAMERA_CMPDN_PIN_M_GPIO

/* FRONT sensor */
#define CAMERA_CMRST1_PIN           GPIO_CAMERA_CMRST1_PIN
#define CAMERA_CMRST1_PIN_M_GPIO    GPIO_CAMERA_CMRST1_PIN_M_GPIO

#define CAMERA_CMPDN1_PIN           GPIO_CAMERA_CMPDN1_PIN
#define CAMERA_CMPDN1_PIN_M_GPIO    GPIO_CAMERA_CMPDN1_PIN_M_GPIO

/* Define I2C Bus Num */
#define SUPPORT_I2C_BUS_NUM1        0
#define SUPPORT_I2C_BUS_NUM2        0
#else
#define CAMERA_CMRST_PIN            0
#define CAMERA_CMRST_PIN_M_GPIO     0

#define CAMERA_CMPDN_PIN            0
#define CAMERA_CMPDN_PIN_M_GPIO     0

/* FRONT sensor */
#define CAMERA_CMRST1_PIN           0
#define CAMERA_CMRST1_PIN_M_GPIO    0

#define CAMERA_CMPDN1_PIN           0
#define CAMERA_CMPDN1_PIN_M_GPIO    0

#define GPIO_OUT_ONE 1
#define GPIO_OUT_ZERO 0



int mtkcam_gpio_set(int PinIdx, int PwrType, int Val);
int mtkcam_gpio_init(struct platform_device *pdev);

#endif /* End of #if defined CONFIG_MTK_LEGACY */

enum KD_REGULATOR_TYPE_T {
	VCAMA,
	VCAMD,
	VCAMIO,
	VCAMAF,
	SUB_VCAMD,
};

enum CAMPowerType {
	CAMPDN,
	CAMRST,
	CAM1PDN,
	CAM1RST,
	CAMLDO
};

extern bool _hwPowerDown(int PinIdx, enum KD_REGULATOR_TYPE_T PwrType);
extern bool _hwPowerOn(int PinIdx,
	enum KD_REGULATOR_TYPE_T PwrType, int Voltage);

void __attribute__ ((weak)) ISP_MCLK1_EN(BOOL En)
{
	pr_info("%s() is not implemented\n", __func__);
}

/* use GPIO API instead of pinctl*/
extern int CamUseSameRst;

extern unsigned int GPIO_CAM0_RST;
extern unsigned int GPIO_CAM1_RST;

extern unsigned int GPIO_CAM0_PDN;
extern unsigned int GPIO_CAM1_PDN;

#endif
