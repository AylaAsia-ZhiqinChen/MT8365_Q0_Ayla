/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2015. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*/
#include "lcm_drv.h"

/* --------------------------------------------------------------------- */
/* Local Constants */
/* --------------------------------------------------------------------- */
#define FRAME_WIDTH  (800)
#define FRAME_HEIGHT (1280)

#ifdef GPIO_LCM_PWR_EN
#define GPIO_LCD_VDD_EN   GPIO_LCM_PWR_EN
#else
#define GPIO_LCD_VDD_EN   0xFFFFFFFF
#endif

#ifdef GPIO_LCM_RST
#define GPIO_LCD_RST      GPIO_LCM_RST
#else
#define GPIO_LCD_RST      0xFFFFFFFF
#endif

#ifdef GPIO_LCM_PWR
#define GPIO_LCD_PWR      GPIO_LCM_PWR
#else
#define GPIO_LCD_PWR      0xFFFFFFFF
#endif

/* --------------------------------------------------------------------- */
/* Local Variables */
/* --------------------------------------------------------------------- */
static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

/* --------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------- */
#ifdef BUILD_LK
static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
	if (GPIO == 0xFFFFFFFF) {
		printf("[LK/LCM] GPIO not defined\n");
		return;
	}
	mt_set_gpio_mode(GPIO, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}
#endif

static void lcm_init_power(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_init_power() enter\n");

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(5);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(5);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(10);

	lcm_set_gpio_output(GPIO_LCD_VDD_EN, GPIO_OUT_ONE);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_PWR, GPIO_OUT_ONE);
	MDELAY(20);

/*	mt6392_upmu_set_rg_vgp2_vosel(3); */
/*	mt6392_upmu_set_rg_vgp2_en(0x1); */
	MDELAY(20);
	printf("[LK/LCM] lcm_init_power() end\n");

#else
	pr_err("[Kernel/LCM] lcm_init_power() enter\n");
#endif
}

/* --------------------------------------------------------------------- */
/* LCM Driver Implementations */
/* --------------------------------------------------------------------- */
static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	printf("[LK/LCM] lcm_get_params() enter\n");

	params->type   = LCM_TYPE_DPI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	params->dpi.format            = LCM_DPI_FORMAT_RGB888;
	params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;

	params->dpi.clk_pol           = LCM_POLARITY_FALLING;
	params->dpi.de_pol            = LCM_POLARITY_RISING;
	params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
	params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

	params->dpi.hsync_pulse_width = 40;
	params->dpi.hsync_back_porch  = 220;
	params->dpi.hsync_front_porch = 110;

	params->dpi.vsync_pulse_width = 5;
	params->dpi.vsync_back_porch  = 20;
	params->dpi.vsync_front_porch = 5;

	params->dpi.width = FRAME_WIDTH;
	params->dpi.height = FRAME_HEIGHT;

	params->dpi.PLL_CLOCK           = 67;
	params->dpi.ssc_disable = 1;
	params->dpi.lvds_tx_en = 1;
}

static void lcm_init_lcm(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_init() enter\n");
#else
	pr_err("[Kernel/LCM] lcm_init() enter\n");
#endif
}

LCM_DRIVER clap070wp03xg_lvds_lcm_drv = {
	.name       = "clap070wp03xg_lvds",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init = lcm_init_lcm,
	.init_power     = lcm_init_power,
};
