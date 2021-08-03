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

#ifndef BUILD_LK
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/of_gpio.h>
#include <asm-generic/gpio.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/types.h>
#include <linux/vmalloc.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/regulator/consumer.h>
#include <linux/clk.h>
#endif
#endif
#include "lcm_drv.h"
#include "lcm_util.h"


/* --------------------------------------------------------------------- */
/*  Local Constants */
/* --------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE                                    0
#define FRAME_WIDTH                                         (1920)
#define FRAME_HEIGHT                                        (1080)

#define REGFLAG_DELAY                                       0xFC
#define REGFLAG_END_OF_TABLE                                0xFD

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* --------------------------------------------------------------------- */
/*  Local Variables */
/* --------------------------------------------------------------------- */
static struct LCM_UTIL_FUNCS lcm_util = {0};
#define SET_RESET_PIN(v)     (lcm_util.set_reset_pin((v)))
#define UDELAY(n)            (lcm_util.udelay(n))
#define MDELAY(n)            (lcm_util.mdelay(n))

/* --------------------------------------------------------------------- */
/*  Local Functions */
/* --------------------------------------------------------------------- */
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
	lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)      lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) \
	lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)       lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
	lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#ifdef BUILD_LK
#define LCD_DEBUG(fmt)  dprintf(CRITICAL, fmt)
#else
#define LCD_DEBUG(fmt)  pr_debug(fmt)
#endif



/* --------------------------------------------------------------------- */
/*  LCM Driver Implementations */
/* --------------------------------------------------------------------- */
static void lcm_set_util_funcs(const struct LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(struct LCM_UTIL_FUNCS));
}

static void lcm_get_params(struct LCM_PARAMS *params)
{
	memset(params, 0, sizeof(struct LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;


	params->dsi.mode                    = SYNC_PULSE_VDO_MODE;
	params->dbi.te_mode = LCM_DBI_TE_MODE_DISABLED;
	params->dbi.te_edge_polarity = LCM_POLARITY_RISING;


	params->dsi.LANE_NUM                = LCM_FOUR_LANE;
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	params->dsi.packet_size             = 256;

	params->dsi.PS                      = LCM_PACKED_PS_24BIT_RGB888;

	params->dsi.vertical_sync_active    = 5;
	params->dsi.vertical_backporch      = 36;
	params->dsi.vertical_frontporch     = 4;
	params->dsi.vertical_active_line    = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active  = 44;
	params->dsi.horizontal_backporch    = 148;
	params->dsi.horizontal_frontporch   = 88;
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;
	params->dsi.ssc_disable             = 1; 
        params->dsi.vsync_disable = 1;
	params->dsi.PLL_CLOCK               = 450;

}

static void lcm_init(void)
{

}

static void lcm_suspend(void)
{

}

static void lcm_resume(void)
{

}

static unsigned int lcm_compare_id(void)
{
	return 1;
}

struct LCM_DRIVER mt8365_fhdnl_dsi_vdo_auto_lcm_drv = {
	.name               = "mt8365_fhdnl_dsi_vdo_auto",
	.set_util_funcs     = lcm_set_util_funcs,
	.get_params         = lcm_get_params,
	.init               = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
	.compare_id         = lcm_compare_id,
};

