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
#define FRAME_WIDTH		(800)
#define FRAME_HEIGHT		(1280)

#define REGFLAG_DELAY	0xFFE
#define REGFLAG_END_OF_TABLE	0xFFF

#define LCM_ID_ILI9881C		0x9881

/* --------------------------------------------------------------- */
/*  Local Variables */
/* --------------------------------------------------------------- */
extern void mdelay(unsigned long msec);

static LCM_UTIL_FUNCS lcm_util = {
	.set_reset_pin = NULL,
	.udelay = NULL,
	.mdelay = NULL,
};

#define SET_RESET_PIN(v)	(lcm_util.set_reset_pin((v)))

#define UDELAY(n)	(lcm_util.udelay(n))
#define MDELAY(n)	(lcm_util.mdelay(n))

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
{0xFF, 03, {0x98, 0x81, 0x03} },

	/* GIP_1 */
	{0x01, 01, {0x00} },
	{0x02, 01, {0x00} },
	{0x03, 01, {0x73} },
	{0x04, 01, {0x00} },
	{0x05, 01, {0x00} },
	{0x06, 01, {0x08} },
	{0x07, 01, {0x00} },
	{0x08, 01, {0x00} },
	{0x09, 01, {0x00} },
	{0x0A, 01, {0x01} },
	{0x0B, 01, {0x01} },
	{0x0C, 01, {0x00} },
	{0x0D, 01, {0x01} },
	{0x0E, 01, {0x01} },
	{0x0F, 01, {0x00} },
	{0x10, 01, {0x00} },
	{0x11, 01, {0x00} },
	{REGFLAG_DELAY, 5, {} },
	{0x12, 01, {0x00} },
	{0x13, 01, {0x00} },
	{0x14, 01, {0x00} },
	{0x15, 01, {0x00} },
	{0x16, 01, {0x00} },
	{0x17, 01, {0x00} },
	{0x18, 01, {0x00} },
	{0x19, 01, {0x00} },
	{0x1A, 01, {0x00} },
	{0x1B, 01, {0x00} },
	{0x1C, 01, {0x00} },
	{0x1D, 01, {0x00} },
	{0x1E, 01, {0x40} },
	{0x1F, 01, {0xC0} },
	{0x20, 01, {0x06} },
	{0x21, 01, {0x01} },
	{0x22, 01, {0x06} },
	{0x23, 01, {0x01} },
	{0x24, 01, {0x88} },
	{0x25, 01, {0x88} },
	{0x26, 01, {0x00} },
	{0x27, 01, {0x00} },
	{REGFLAG_DELAY, 5, {} },
	{0x28, 01, {0x3B} },
	{0x29, 01, {0x03} },
	{0x2A, 01, {0x00} },
	{0x2B, 01, {0x00} },
	{0x2C, 01, {0x00} },
	{0x2D, 01, {0x00} },
	{0x2E, 01, {0x00} },
	{0x2F, 01, {0x00} },
	{0x30, 01, {0x00} },
	{0x31, 01, {0x00} },
	{0x32, 01, {0x00} },
	{0x33, 01, {0x00} },
	{0x34, 01, {0x00} },/* GPWR1/2 non overlap time 2.62us */
	{0x35, 01, {0x00} },
	{0x36, 01, {0x00} },
	{0x37, 01, {0x00} },
	{0x38, 01, {0x00} },
	{0x39, 01, {0x00} },
	{0x3A, 01, {0x00} },
	{0x3B, 01, {0x00} },
	{0x3C, 01, {0x00} },
	{0x3D, 01, {0x00} },
	{0x3E, 01, {0x00} },
	{0x3F, 01, {0x00} },
	{0x40, 01, {0x00} },
	{0x41, 01, {0x00} },
	{0x42, 01, {0x00} },
	{0x43, 01, {0x00} },
	{0x44, 01, {0x00} },
	{REGFLAG_DELAY, 5, {} },
	/* GIP_2 */
	{0x50, 01, {0x01} },
	{0x51, 01, {0x23} },
	{0x52, 01, {0x45} },
	{0x53, 01, {0x67} },
	{0x54, 01, {0x89} },
	{0x55, 01, {0xAB} },
	{0x56, 01, {0x01} },
	{0x57, 01, {0x23} },
	{0x58, 01, {0x45} },
	{0x59, 01, {0x67} },
	{0x5A, 01, {0x89} },
	{0x5B, 01, {0xAB} },
	{0x5C, 01, {0xCD} },
	{0x5D, 01, {0xEF} },
	{REGFLAG_DELAY, 5, {} },
	/* GIP_3 */
	{0x5E, 01, {0x00} },
	{0x5F, 01, {0x01} },
	{0x60, 01, {0x01} },
	{0x61, 01, {0x06} },
	{0x62, 01, {0x06} },
	{0x63, 01, {0x07} },
	{0x64, 01, {0x07} },
	{0x65, 01, {0x00} },
	{0x66, 01, {0x00} },
	{0x67, 01, {0x02} },
	{0x68, 01, {0x02} },
	{0x69, 01, {0x05} },
	{0x6A, 01, {0x05} },
	{0x6B, 01, {0x02} },
	{0x6C, 01, {0x0D} },
	{REGFLAG_DELAY, 5, {} },
	{0x6D, 01, {0x0D} },
	{0x6E, 01, {0x0C} },
	{0x6F, 01, {0x0C} },
	{0x70, 01, {0x0F} },
	{0x71, 01, {0x0F} },
	{0x72, 01, {0x0E} },
	{0x73, 01, {0x0E} },
	{0x74, 01, {0x02} },
	{0x75, 01, {0x01} },
	{0x76, 01, {0x01} },
	{0x77, 01, {0x06} },
	{0x78, 01, {0x06} },
	{0x79, 01, {0x07} },
	{0x7A, 01, {0x07} },
	{0x7B, 01, {0x00} },
	{0x7C, 01, {0x00} },
	{0x7D, 01, {0x02} },
	{0x7E, 01, {0x02} },
	{0x7F, 01, {0x05} },
	{0x80, 01, {0x05} },
	{0x81, 01, {0x02} },
	{0x82, 01, {0x0D} },
	{0x83, 01, {0x0D} },
	{0x84, 01, {0x0C} },
	{0x85, 01, {0x0C} },
	{0x86, 01, {0x0F} },
	{0x87, 01, {0x0F} },
	{0x88, 01, {0x0E} },
	{0x89, 01, {0x0E} },
	{0x8A, 01, {0x02} },
	{REGFLAG_DELAY, 5, {} },
	/* Page 4 command; */
	{0xFF, 03, {0x98, 0x81, 0x04} },

	{0x3B, 01, {0xC0} },/* ILI4003D sel */
	{0x6C, 01, {0x15} },/* Set VCORE voltage =1.5V */
	{0x6E, 01, {0x10} },/* di_pwr_reg=0 for power mode 2A  VGH clamp 18V */
	{0x6F, 01, {0x33} },/* pumping ratio VGH=5x VGL=-3x */
	{0x8D, 01, {0x1B} },/* VGL clamp -10V */
	{0x87, 01, {0xBA} },/* ESD */
	{0x3A, 01, {0x24} },/* POWER SAVING */
	{0x26, 01, {0x76} },
	{0xB2, 01, {0xD1} },

	{REGFLAG_DELAY, 5, {} },
	{0x00, 01, {0x00} },/* 3lane,4LANE去掉这个代码 */

	/* Page 1 command */
	{0xFF, 03, {0x98, 0x81, 0x01} },
	{0x22, 01, {0x0A} },/* BGR, SS */
	{0x31, 01, {0x00} },/* Zigzag type3 inversion */
	{0x40, 01, {0x53} },/* ILI4003D sel */
	{0x43, 01, {0x66} },
	{0x53, 01, {0x33} },
	{0x50, 01, {0x87} },
	{0x51, 01, {0x82} },
	{0x60, 01, {0x15} },
	{0x61, 01, {0x01} },
	{0x62, 01, {0x0C} },
	{0x63, 01, {0x00} },
	{REGFLAG_DELAY, 5, {} },
	/* Gamma P */
	{0xA0, 01, {0x00} },
	{0xA1, 01, {0x13} },/* VP251 */
	{0xA2, 01, {0x23} },/* VP247 */
	{0xA3, 01, {0x14} },/* VP243 */
	{0xA4, 01, {0x16} },/* VP239 */
	{0xA5, 01, {0x29} },/* VP231 */
	{0xA6, 01, {0x1E} },/* VP219 */
	{0xA7, 01, {0x1D} },/* VP203 */
	{0xA8, 01, {0x86} },/* VP175 */
	{0xA9, 01, {0x1E} },/* VP144 */
	{0xAA, 01, {0x29} },/* VP111 */
	{0xAB, 01, {0x74} },/* VP80 */
	{0xAC, 01, {0x19} },/* VP52 */
	{0xAD, 01, {0x17} },/* VP36 */
	{0xAE, 01, {0x4B} },/* VP24 */
	{0xAF, 01, {0x20} },/* VP16 */
	{0xB0, 01, {0x26} },/* VP12 */
	{0xB1, 01, {0x4C} },/* VP8 */
	{0xB2, 01, {0x5D} },/* VP4 */
	{0xB3, 01, {0x3F} },/* VP0 */
	{REGFLAG_DELAY, 5, {} },
	/* Gamma N */
	{0xC0, 01, {0x00} },/* VN255 GAMMA N */
	{0xC1, 01, {0x13} },/* VN251 */
	{0xC2, 01, {0x23} },/* VN247 */
	{0xC3, 01, {0x14} },/* VN243 */
	{0xC4, 01, {0x16} },/* VN239 */
	{0xC5, 01, {0x29} },/* VN231 */
	{0xC6, 01, {0x1E} },/* VN219 */
	{0xC7, 01, {0x1D} },/* VN203 */
	{0xC8, 01, {0x86} },/* VN175 */
	{0xC9, 01, {0x1E} },/* VN144 */
	{0xCA, 01, {0x29} },/* VN111 */
	{0xCB, 01, {0x74} },/* VN80 */
	{0xCC, 01, {0x19} },/* VN52 */
	{0xCD, 01, {0x17} },/* VN36 */
	{0xCE, 01, {0x4B} },/* VN24 */
	{0xCF, 01, {0x20} },/* VN16 */
	{0xD0, 01, {0x26} },/* VN12 */
	{0xD1, 01, {0x4C} },/* VN8 */
	{0xD2, 01, {0x5D} },/* VN4 */
	{0xD3, 01, {0x3F} },/* VN0 */



	{REGFLAG_DELAY, 5, {} },
	{0xFF, 3, {0x98, 81, 00} },
	{0x35, 1, {0x00} },
	{0x11, 1, {0x00} },
	{REGFLAG_DELAY, 120, {} },
	{0x29, 1, {0x00} },
	{REGFLAG_DELAY, 120, {} },
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

	params->dsi.mode   = BURST_VDO_MODE;

	params->dsi.LANE_NUM = LCM_THREE_LANE;
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	params->dsi.packet_size = 256;

	params->dsi.intermediat_buffer_num = 2;

	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;
    params->dsi.word_count=FRAME_WIDTH*3;

	params->dsi.vertical_sync_active = 6;
	params->dsi.vertical_backporch = 15;
	params->dsi.vertical_frontporch = 16;
	params->dsi.vertical_active_line = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active = 8;
	params->dsi.horizontal_backporch = 48;
	params->dsi.horizontal_frontporch = 52;
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;

	params->dsi.PLL_CLOCK = 287;
}

static void lcm_init_power(void)
{
	printf("[LK/LCM] %s enter\n", __func__);

	pmic_vsim1_enable(true, 1800);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(1);
	lcm_set_gpio_output(GPIO_LCD_STB, GPIO_OUT_ONE);
	MDELAY(50);

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(50);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(200);
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

	data_array[0] = 0x00043902;
	data_array[1] = 0x018198FF;	// CMD_Page 1
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00013700;	// return one byte
	dsi_set_cmdq(data_array, 1, 1);

	read_reg_v2(0x00, &buffer[0], 1);
	read_reg_v2(0x01, &buffer[1], 1);
	read_reg_v2(0x02, &buffer[2], 1);
	id = (buffer[0] << 8) | buffer[1]; //we only need ID

#ifdef BUILD_LK
	printf("Read ILI9881C ID, buf:0x%02x,0x%02x,0x%02x, ID=0x%X\n", buffer[0], buffer[1], buffer[2], id);
#else
	printk("Read ILI9881C ID, buf:0x%02x,0x%02x,0x%02x, ID=0x%X\n", buffer[0], buffer[1], buffer[2], id);
#endif

	return (LCM_ID_ILI9881C == id) ? 1 : 0;
}

LCM_DRIVER ili9881c_xy_dsi_vdo_tv101wxu_lcm_drv = {
	.name			= "ili9881c_xy_dsi_vdo_tv101wxu",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init_power		= lcm_init_power,
	.compare_id = lcm_compare_id,
	.init           = lcm_init,
};
