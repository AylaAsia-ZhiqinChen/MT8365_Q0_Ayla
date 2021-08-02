/*
* Copyright (c) 2019 MediaTek Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files
* (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifdef BUILD_LK
#include <string.h>
#include <platform/mt_gpio.h>
#include <platform/mt_pmic.h>
#include <platform/sync_write.h>
#else
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/of_gpio.h>
#include <asm-generic/gpio.h>

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#endif

#endif
#include "lcm_drv.h"

#ifdef GPIO_LCM_RST
#define GPIO_LCD_RST          GPIO_LCM_RST
#else
#define GPIO_LCD_RST          (GPIO20)
#endif

#ifdef GPIO_LCM_STB
#define GPIO_LCD_STB          GPIO_LCM_STB
#else
#define GPIO_LCD_STB          (GPIO21)
#endif

#ifdef GPIO_LCM_PWR_EN
#define GPIO_LCD_PWR_EN       GPIO_LCM_PWR_EN
#else
#define GPIO_LCD_PWR_EN       (GPIO68)
#endif

/* --------------------------------------------------------------- */
/*  Local Constants */
/* --------------------------------------------------------------- */
#define FRAME_WIDTH     (800)
#define FRAME_HEIGHT    (1280)

#define REGFLAG_DELAY         0xFE
#define REGFLAG_END_OF_TABLE  0x00

#define LCM_ID_JD9364		0x9364

/* --------------------------------------------------------------- */
/*  Local Variables */
/* --------------------------------------------------------------- */
extern void mdelay(unsigned long msec);

static LCM_UTIL_FUNCS lcm_util = {
	.set_reset_pin = NULL,
	.udelay = NULL,
	.mdelay = NULL,
};

#define SET_RESET_PIN(v)		(lcm_util.set_reset_pin((v)))

#define UDELAY(n)				(lcm_util.udelay(n))
#define MDELAY(n)				(lcm_util.mdelay(n))

/* --------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------- */
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
		 (lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update))
#define dsi_set_cmdq(pdata, queue_size, force_update) \
		 (lcm_util.dsi_set_cmdq(pdata, queue_size, force_update))
#define wrtie_cmd(cmd) \
		 (lcm_util.dsi_write_cmd(cmd))
#define write_regs(addr, pdata, byte_nums) \
		 (lcm_util.dsi_write_regs(addr, pdata, byte_nums))
#define read_reg \
		 (lcm_util.dsi_read_reg())
#define read_reg_v2(cmd, buffer, buffer_size) \
		 (lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size))

struct LCM_setting_table {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initial_setting[] = {
	{0xE0, 1, {0x00} },
	{0xE1, 1, {0x93} },
	{0xE2, 1, {0x65} },
	{0xE3, 1, {0xF8} },
	{0x80, 1, {0x02} },
	{0xE0, 1, {0x04} },
	{0x2D, 1, {0x03} },
	{0xE0, 1, {0x01} },
	{0x00, 1, {0x00} },
	{0x01, 1, {0x5C} },
	{0x03, 1, {0x00} },
	{0x04, 1, {0x68} },
	{0x17, 1, {0x00} },
	{0x18, 1, {0xD7} },
	{0x19, 1, {0x01} },
	{0x1A, 1, {0x00} },
	{0x1B, 1, {0xD7} },
	{0x1C, 1, {0x01} },
	{0x1F, 1, {0x6B} },
	{0x20, 1, {0x29} },
	{0x21, 1, {0x29} },
	{0x22, 1, {0x7E} },
	{0x37, 1, {0x59} },
	{0x35, 1, {0x28} },
	{0x38, 1, {0x05} },
	{0x39, 1, {0x08} },
	{0x3A, 1, {0x12} },
	{0x3C, 1, {0x7E} },
	{0x3D, 1, {0xFF} },
	{0x3E, 1, {0xFF} },
	{0x3F, 1, {0x7F} },
	{0x40, 1, {0x06} },
	{0x41, 1, {0xA0} },
	{0x43, 1, {0x1E} },
	{0x44, 1, {0x0B} },
	{0x55, 1, {0x01} },
	{0x56, 1, {0x01} },
	{0x57, 1, {0x69} },
	{0x58, 1, {0x0A} },
	{0x59, 1, {0x2A} },
	{0x5A, 1, {0x29} },
	{0x5B, 1, {0x15} },
	{0x5C, 1, {0x16} },
	{0x5D, 1, {0x64} },
	{0x5E, 1, {0x50} },
	{0x5F, 1, {0x43} },
	{0x60, 1, {0x38} },
	{0x61, 1, {0x36} },
	{0x62, 1, {0x29} },
	{0x63, 1, {0x2F} },
	{0x64, 1, {0x1B} },
	{0x65, 1, {0x35} },
	{0x66, 1, {0x35} },
	{0x67, 1, {0x35} },
	{0x68, 1, {0x53} },
	{0x69, 1, {0x41} },
	{0x6A, 1, {0x47} },
	{0x6B, 1, {0x38} },
	{0x6C, 1, {0x34} },
	{0x6D, 1, {0x27} },
	{0x6E, 1, {0x19} },
	{0x6F, 1, {0x12} },
	{0x70, 1, {0x64} },
	{0x71, 1, {0x50} },
	{0x72, 1, {0x43} },
	{0x73, 1, {0x38} },
	{0x74, 1, {0x36} },
	{0x75, 1, {0x29} },
	{0x76, 1, {0x2F} },
	{0x77, 1, {0x1B} },
	{0x78, 1, {0x35} },
	{0x79, 1, {0x35} },
	{0x7A, 1, {0x35} },
	{0x7B, 1, {0x53} },
	{0x7C, 1, {0x41} },
	{0x7D, 1, {0x47} },
	{0x7E, 1, {0x38} },
	{0x7F, 1, {0x34} },
	{0x80, 1, {0x27} },
	{0x81, 1, {0x19} },
	{0x82, 1, {0x12} },
	{0xE0, 1, {0x02} },
	{0x00, 1, {0x53} },
	{0x01, 1, {0x5F} },
	{0x02, 1, {0x5F} },
	{0x03, 1, {0x5F} },
	{0x04, 1, {0x5F} },
	{0x05, 1, {0x51} },
	{0x06, 1, {0x77} },
	{0x07, 1, {0x57} },
	{0x08, 1, {0x4F} },
	{0x09, 1, {0x5F} },
	{0x0A, 1, {0x4D} },
	{0x0B, 1, {0x4B} },
	{0x0C, 1, {0x5F} },
	{0x0D, 1, {0x49} },
	{0x0E, 1, {0x47} },
	{0x0F, 1, {0x45} },
	{0x10, 1, {0x41} },
	{0x11, 1, {0x5F} },
	{0x12, 1, {0x5F} },
	{0x13, 1, {0x5F} },
	{0x14, 1, {0x5F} },
	{0x15, 1, {0x5F} },
	{0x16, 1, {0x52} },
	{0x17, 1, {0x5F} },
	{0x18, 1, {0x5F} },
	{0x19, 1, {0x5F} },
	{0x1A, 1, {0x5F} },
	{0x1B, 1, {0x50} },
	{0x1C, 1, {0x77} },
	{0x1D, 1, {0x57} },
	{0x1E, 1, {0x4E} },
	{0x1F, 1, {0x5F} },
	{0x20, 1, {0x4C} },
	{0x21, 1, {0x4A} },
	{0x22, 1, {0x5F} },
	{0x23, 1, {0x48} },
	{0x24, 1, {0x46} },
	{0x25, 1, {0x44} },
	{0x26, 1, {0x40} },
	{0x27, 1, {0x5F} },
	{0x28, 1, {0x5F} },
	{0x29, 1, {0x5F} },
	{0x2A, 1, {0x5F} },
	{0x2B, 1, {0x5F} },
	{0x2C, 1, {0x12} },
	{0x2D, 1, {0x1F} },
	{0x2E, 1, {0x1F} },
	{0x2F, 1, {0x1F} },
	{0x30, 1, {0x1F} },
	{0x31, 1, {0x00} },
	{0x32, 1, {0x17} },
	{0x33, 1, {0x17} },
	{0x34, 1, {0x04} },
	{0x35, 1, {0x1F} },
	{0x36, 1, {0x06} },
	{0x37, 1, {0x08} },
	{0x38, 1, {0x1F} },
	{0x39, 1, {0x0A} },
	{0x3A, 1, {0x0C} },
	{0x3B, 1, {0x0E} },
	{0x3C, 1, {0x10} },
	{0x3D, 1, {0x1F} },
	{0x3E, 1, {0x1F} },
	{0x3F, 1, {0x1F} },
	{0x40, 1, {0x1F} },
	{0x41, 1, {0x1F} },
	{0x42, 1, {0x13} },
	{0x43, 1, {0x1F} },
	{0x44, 1, {0x1F} },
	{0x45, 1, {0x1F} },
	{0x46, 1, {0x1F} },
	{0x47, 1, {0x01} },
	{0x48, 1, {0x17} },
	{0x49, 1, {0x17} },
	{0x4A, 1, {0x05} },
	{0x4B, 1, {0x1F} },
	{0x4C, 1, {0x07} },
	{0x4D, 1, {0x09} },
	{0x4E, 1, {0x1F} },
	{0x4F, 1, {0x0B} },
	{0x50, 1, {0x0D} },
	{0x51, 1, {0x0F} },
	{0x52, 1, {0x11} },
	{0x53, 1, {0x1F} },
	{0x54, 1, {0x1F} },
	{0x55, 1, {0x1F} },
	{0x56, 1, {0x1F} },
	{0x57, 1, {0x1F} },
	{0x58, 1, {0x00} },
	{0x59, 1, {0x00} },
	{0x5A, 1, {0x00} },
	{0x5B, 1, {0x10} },
	{0x5C, 1, {0x04} },
	{0x5D, 1, {0x40} },
	{0x5E, 1, {0x00} },
	{0x5F, 1, {0x00} },
	{0x60, 1, {0x40} },
	{0x61, 1, {0x03} },
	{0x62, 1, {0x04} },
	{0x63, 1, {0x70} },
	{0x64, 1, {0x70} },
	{0x65, 1, {0x75} },
	{0x66, 1, {0x0B} },
	{0x67, 1, {0xB4} },
	{0x68, 1, {0x06} },
	{0x69, 1, {0x70} },
	{0x6A, 1, {0x70} },
	{0x6B, 1, {0x04} },
	{0x6C, 1, {0x00} },
	{0x6D, 1, {0x04} },
	{0x6E, 1, {0x00} },
	{0x6F, 1, {0x88} },
	{0x70, 1, {0x00} },
	{0x71, 1, {0x00} },
	{0x72, 1, {0x06} },
	{0x73, 1, {0x7B} },
	{0x74, 1, {0x00} },
	{0x75, 1, {0xBC} },
	{0x76, 1, {0x00} },
	{0x77, 1, {0x05} },
	{0x78, 1, {0x2A} },
	{0x79, 1, {0x00} },
	{0x7A, 1, {0x00} },
	{0x7B, 1, {0x00} },
	{0x7C, 1, {0x00} },
	{0x7D, 1, {0x03} },
	{0x7E, 1, {0x7B} },
	{0xE0, 1, {0x04} },
	{0x0E, 1, {0x48} },
	{0x2B, 1, {0x2B} },
	{0x2E, 1, {0x44} },
	{0xE0, 1, {0x00} },
	{0xE6, 1, {0x02} },
	{0xE7, 1, {0x02} },
	{0x11, 0, {0x00} },
	{REGFLAG_DELAY, 120, {} },
	{0x29, 0, {0x00} },
	{REGFLAG_DELAY, 20, {} },
	{0x35, 0, {0x00} },
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};

static void push_table(struct LCM_setting_table *table, unsigned int count,
			unsigned char force_update)
{
	unsigned int i;

	for (i = 0; i < count; i++) {

		unsigned int cmd;

		cmd = table[i].cmd;
		switch (cmd) {

		case REGFLAG_DELAY:
			MDELAY(table[i].count);
			break;

		case REGFLAG_END_OF_TABLE:
			break;

		default:
			dsi_set_cmdq_V2(cmd, table[i].count,
				table[i].para_list, force_update);
		}
	}
}

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
	if (GPIO == 0xFFFFFFFF) {
		printf("[LK/LCM] GPIO not defined\n");
		return;
	}

	mt_set_gpio_mode(GPIO, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO, (output > 0) ? GPIO_OUT_ONE : GPIO_OUT_ZERO);
}

/* --------------------------------------------------------------- */
/*  LCM Driver Implementations */
/* --------------------------------------------------------------- */
static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	params->dsi.mode   = SYNC_EVENT_VDO_MODE;

	params->dsi.LANE_NUM                = LCM_THREE_LANE;
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	params->dsi.packet_size = 256;

	params->dsi.intermediat_buffer_num = 0;

	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

	params->dsi.vertical_sync_active		= 4;
	params->dsi.vertical_backporch			= 10;
	params->dsi.vertical_frontporch			= 30;
	params->dsi.vertical_active_line		= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active		= 20;
	params->dsi.horizontal_backporch		= 43;
	params->dsi.horizontal_frontporch		= 43;
	params->dsi.horizontal_active_pixel		= FRAME_WIDTH;

	params->dsi.PLL_CLOCK = 200;
	params->dsi.clk_lp_per_line_enable = 1;

	params->dsi.ssc_disable = 1;
	params->dsi.cont_clock= 0;
	params->dsi.DA_HS_EXIT = 1;
	params->dsi.CLK_ZERO = 16;
	params->dsi.HS_ZERO = 9;
	params->dsi.HS_TRAIL = 5;
	params->dsi.CLK_TRAIL = 5;
	params->dsi.CLK_HS_POST = 8;
	params->dsi.CLK_HS_EXIT = 6;

	params->dsi.TA_GO = 8;
	params->dsi.TA_GET = 10;

	params->physical_width = 108;
	params->physical_height = 172;
}

static void lcm_init_power(void)
{
	printf("[LK/LCM] %s enter\n", __func__);

	pmic_vsim1_enable(true, 1800);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(1);
	lcm_set_gpio_output(GPIO_LCD_STB, GPIO_OUT_ONE);
	MDELAY(10);

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(2);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(5);
}

static void lcm_init(void)
{
	printf("[LK/LCM] %s enter\n", __func__);

	push_table(lcm_initial_setting,
		sizeof(lcm_initial_setting) / sizeof(struct LCM_setting_table),
		1);
}

static unsigned int lcm_compare_id(void)
{
    unsigned int id=0;
    unsigned char buffer[3];
    unsigned int array[16];
    unsigned int data_array[16];

	/* NOTE:should reset LCM firstly */
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(20);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(30);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(30);

	data_array[0] = 0x00033700;
	dsi_set_cmdq(data_array, 1, 1);

	read_reg_v2(0x04, buffer, 3);
	id = (buffer[0] << 8) | buffer[1]; //we only need ID

#ifdef BUILD_LK
	printf("Read JD9364 ID, buf:0x%02x ,0x%02x,0x%02x, ID=0x%X\n", buffer[0], buffer[1], buffer[2],id);
#else
	printk("Read JD9364 ID, buf:0x%02x ,0x%02x,0x%02x, ID=0x%X\n", buffer[0], buffer[1], buffer[2],id);
#endif

	return (LCM_ID_JD9364 == id) ? 1 : 0;

}

LCM_DRIVER jd9364_hsx101n31am27a_dsi_vdo_lcm_drv = {
	.name           = "jd9364_hsx101n31am27a_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init_power     = lcm_init_power,
	.compare_id = lcm_compare_id,
	.init           = lcm_init,
};
