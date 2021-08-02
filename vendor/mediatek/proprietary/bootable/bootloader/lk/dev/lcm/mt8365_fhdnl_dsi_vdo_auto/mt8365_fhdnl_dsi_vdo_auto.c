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
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/
#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
//	#include <mach/mt_pm_ldo.h>
//  #include <mach/mt_gpio.h>
#endif


/* --------------------------------------------------------------------- */
/*  Local Constants */
/* --------------------------------------------------------------------- */
#define LCM_DSI_CMD_MODE                                    0
#define FRAME_WIDTH                                         (1920)
#define FRAME_HEIGHT                                        (1080)

#define REGFLAG_DELAY                                       0xFC
#define REGFLAG_END_OF_TABLE                                0xFD
#define  LCM_POWER_EXT
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* i2c control start */
extern void *g_fdt;
#define LCM_I2C_MODE    ST_MODE
#define LCM_I2C_SPEED   50

static struct mt_i2c_t tps65132_i2c;
static int _lcm_i2c_write_bytes(kal_uint8 addr, kal_uint8 value)
{
	kal_int32 ret_code = I2C_OK;
	kal_uint8 write_data[2];
	//kal_uint16 len;
	int id = 0, i2c_addr = 0x29;
	write_data[0] = addr;
	write_data[1] = value;

	tps65132_i2c.id = (U16)id;
	tps65132_i2c.addr = (U16)i2c_addr;
	tps65132_i2c.mode = LCM_I2C_MODE;
	tps65132_i2c.speed = LCM_I2C_SPEED;
	
	ret_code = mtk_i2c_write(tps65132_i2c.id, tps65132_i2c.addr,tps65132_i2c.speed,write_data, 2);
	if (ret_code<0)
		dprintf(0, "[LCM][ERROR] %s: %d\n", __func__, ret_code);

	return ret_code;
}

static int _lcm_i2c_read_bytes(kal_uint8 addr)
{
	kal_int32 ret_code = I2C_OK;
	kal_uint8 write_data[1];
	//kal_uint16 len;
	int id = 0, i2c_addr = 0x29;
	write_data[0] = addr;
	//uint8_t *buffer =0;
	
	tps65132_i2c.id = (U16)id;
	tps65132_i2c.addr = (U16)i2c_addr;
	tps65132_i2c.mode = LCM_I2C_MODE;
	tps65132_i2c.speed = LCM_I2C_SPEED;

	ret_code = mtk_i2c_write(tps65132_i2c.id, tps65132_i2c.addr,tps65132_i2c.speed,write_data, 1);
	if (ret_code<0)
		dprintf(0, "[LCM][ERROR] %s: %d\n", __func__, ret_code);

	ret_code = mtk_i2c_read(tps65132_i2c.id, tps65132_i2c.addr,tps65132_i2c.speed,write_data, 1);
	if (ret_code<0)
		dprintf(0, "[LCM][ERROR] %s: %d\n", __func__, ret_code);

	return write_data[0];
}


/* i2c control end */

/* --------------------------------------------------------------------- */
/*  Local Variables */
/* --------------------------------------------------------------------- */
static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)                                    (lcm_util.set_reset_pin((v)))
#define MDELAY(n)                                           (lcm_util.mdelay(n))

/* --------------------------------------------------------------------- */
/*  Local Functions */
/* --------------------------------------------------------------------- */
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)    lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)       lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)                                      lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)                  lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)                                       lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)               lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

static void lcm_init_mipi_to_edp(void);

#ifdef BUILD_LK
#define LCD_DEBUG(fmt)  dprintf(CRITICAL,fmt)
#else
#define LCD_DEBUG(fmt)  printk(fmt)
#endif

#ifdef LCM_POWER_EXT
#define GPIO_LCD_PWR_EXT_PIN      GPIO18
#else
#define GPIO_LCD_PWR_EXT_PIN      (0xFFFFFFFF)
#endif

#define LCM_EDP_POWER
#ifdef LCM_EDP_POWER
#define GPIO_LCD_EDP_POWER_PIN      GPIO116
#else
#define GPIO_LCD_EDP_POWER_PIN      (0xFFFFFFFF)
#endif

#define LCM_EDP_RESET
#ifdef LCM_EDP_RESET
#define GPIO_LCD_EDP_RESET_PIN      GPIO67
#else
#define GPIO_LCD_EDP_RESET_PIN      (0xFFFFFFFF)
#endif
///////EDP SETTING///
#define _eDP_2G7_
enum {
	hfp = 0,
	hs,
	hbp,
	hact,
	htotal,
	vfp,
	vs,
	vbp,
	vact,
	vtotal,
	pclk_10khz
};

u8		Read_DPCD010A = 0x00;

bool	ScrambleMode = 0;

bool	flag_mipi_on = 0;

#ifdef _read_edid_ // read eDP panel EDID

u8		EDID_DATA[128] = { 0 };
u16		EDID_Timing[11] = { 0 };

bool	EDID_Reply = 0;
#endif

enum
{
	_Level0_ = 0,                                               // 27.8 mA  0x83/0x00
	_Level1_,                                                   // 26.2 mA  0x82/0xe0
	_Level2_,                                                   // 24.6 mA  0x82/0xc0
	_Level3_,                                                   // 23 mA    0x82/0xa0
	_Level4_,                                                   // 21.4 mA  0x82/0x80
	_Level5_,                                                   // 18.2 mA  0x82/0x40
	_Level6_,                                                   // 16.6 mA  0x82/0x20
	_Level7_,                                                   // 15mA     0x82/0x00
	_Level8_,                                                   // 12.8mA   0x81/0x00
	_Level9_,                                                   // 11.2mA   0x80/0xe0
	_Level10_,                                                  // 9.6mA    0x80/0xc0
	_Level11_,                                                  // 8mA      0x80/0xa0
	_Level12_,                                                  // 6mA      0x80/0x80
};

u8	Swing_Setting1[] = { 0x83, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x82, 0x81, 0x80, 0x80, 0x80, 0x80 };
u8	Swing_Setting2[] = { 0x00, 0xe0, 0xc0, 0xa0, 0x80, 0x40, 0x20, 0x00, 0x00, 0xe0, 0xc0, 0xa0, 0x80 };

u8	Level = _Level7_;                                           // normal

//////////////////////LT8911EXB Config////////////////////////////////
//#define _1920x1200_eDP_Panel_
#define _1080P_eDP_Panel_
//#define _1366x768_eDP_Panel_
//#define _1280x800_eDP_Panel_
//#define _1280x7200_eDP_Panel_

//#define _1600x900_eDP_Panel_
//#define _1920x1200_eDP_Panel_

#define _MIPI_Lane_ 4   // 3 /2 / 1

//#define _eDP_scramble_ // eDP scramble mode

#define _Nvid 0         // 0: 0x0080,default
static int Nvid_Val[] = { 0x0080, 0x0800 };

#ifdef _1080P_eDP_Panel_

#define eDP_lane		2
#define PCR_PLL_PREDIV	0x40

// 根据前端MIPI信号的Timing，修改以下参数：
//According to the timing of the Mipi signal, modify the following parameters:
static int MIPI_Timing[] =
// hfp,	hs,	hbp,	hact,	htotal,	vfp,	vs,	vbp,	vact,	vtotal,	pixel_CLK/10000
//-----|---|------|-------|--------|-----|-----|-----|--------|--------|---------------
{ 88, 44, 148, 1920, 2200, 4, 5, 36, 1080, 1125, 14850 };   // VESA
//  { 48, 32, 80,   1920,   2080,   3,  5,  23,    1080,   1111,   13850 };    // SL156PP36

//#define _6bit_ // eDP panel Color Depth，262K color
#define _8bit_                                              // eDP panel Color Depth，16.7M color

#endif
/* --------------------------------------------------------------------- */
/*  LCM Driver Implementations */
/* --------------------------------------------------------------------- */

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


	params->dsi.mode                    = SYNC_PULSE_VDO_MODE;
	//params->dbi.te_mode = LCM_DBI_TE_MODE_DISABLED;
	//params->dbi.te_edge_polarity = LCM_POLARITY_RISING;


	params->dsi.LANE_NUM                = LCM_FOUR_LANE;

	params->dsi.data_format.color_order     = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq       = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding         = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format          = LCM_DSI_FORMAT_RGB888;

	params->dsi.packet_size = 256;

	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

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


static void lcm_init(void)
{
        lcm_set_gpio_output(GPIO_LCD_PWR_EXT_PIN,GPIO_OUT_ONE);
        MDELAY(120);
   lcm_init_mipi_to_edp();     
}

static unsigned int lcm_compare_id(void)
{

	return 1;

}

/*****************************************************/
void Reset_LT8911EXB( void )
{
	lcm_set_gpio_output(GPIO_LCD_EDP_POWER_PIN,GPIO_OUT_ONE);
	MDELAY( 100 );
	lcm_set_gpio_output(GPIO_LCD_EDP_RESET_PIN,GPIO_OUT_ZERO);
	MDELAY( 100 );
	lcm_set_gpio_output(GPIO_LCD_EDP_RESET_PIN,GPIO_OUT_ONE);
	MDELAY( 100 );
}

void LT8911EX_ChipID( void )                                                        // read Chip ID
{
	_lcm_i2c_write_bytes(0xff, 0x81 );                                         //register bank
	_lcm_i2c_write_bytes(0x08, 0x7f );
#if 1
//#ifdef _uart_debug_
	printf( "LT8911EXB chip ID: %d \n", _lcm_i2c_read_bytes( 0x00 ) );  // 0x17
	printf( "LT8911EXB chip ID: %d \n", _lcm_i2c_read_bytes( 0x01 ) );                      // 0x05
	printf( "LT8911EXB chip ID: %d \n", _lcm_i2c_read_bytes( 0x02 ) );                      // 0xE0
#endif
}

//------------------------------------
void LT8911EXB_MIPI_Video_Timing( void )                                    // ( struct video_timing *video_format )
{
	_lcm_i2c_write_bytes( 0xff, 0xd0 );
	_lcm_i2c_write_bytes( 0x0d, (u8)( MIPI_Timing[vtotal] / 256 ) );
	_lcm_i2c_write_bytes( 0x0e, (u8)( MIPI_Timing[vtotal] % 256 ) );    //vtotal
	_lcm_i2c_write_bytes( 0x0f, (u8)( MIPI_Timing[vact] / 256 ) );
	_lcm_i2c_write_bytes( 0x10, (u8)( MIPI_Timing[vact] % 256 ) );      //vactive

	_lcm_i2c_write_bytes( 0x11, (u8)( MIPI_Timing[htotal] / 256 ) );
	_lcm_i2c_write_bytes( 0x12, (u8)( MIPI_Timing[htotal] % 256 ) );    //htotal
	_lcm_i2c_write_bytes( 0x13, (u8)( MIPI_Timing[hact] / 256 ) );
	_lcm_i2c_write_bytes( 0x14, (u8)( MIPI_Timing[hact] % 256 ) );      //hactive

	_lcm_i2c_write_bytes( 0x15, (u8)( MIPI_Timing[vs] % 256 ) );        //vsa
	_lcm_i2c_write_bytes( 0x16, (u8)( MIPI_Timing[hs] % 256 ) );        //hsa
	_lcm_i2c_write_bytes( 0x17, (u8)( MIPI_Timing[vfp] / 256 ) );
	_lcm_i2c_write_bytes( 0x18, (u8)( MIPI_Timing[vfp] % 256 ) );       //vfp

	_lcm_i2c_write_bytes( 0x19, (u8)( MIPI_Timing[hfp] / 256 ) );
	_lcm_i2c_write_bytes( 0x1a, (u8)( MIPI_Timing[hfp] % 256 ) );       //hfp
}

void LT8911EXB_eDP_Video_cfg( void )                                        // ( struct video_timing *video_format )
{
	_lcm_i2c_write_bytes(0xff, 0xa8 );
	_lcm_i2c_write_bytes(0x2d, 0x88 );                                 // MSA from register

#ifdef _Msa_Active_Only_
	_lcm_i2c_write_bytes(0x05, 0x00 );
	_lcm_i2c_write_bytes(0x06, 0x00 );                                 //htotal
	_lcm_i2c_write_bytes(0x07, 0x00 );
	_lcm_i2c_write_bytes(0x08, 0x00 );                                 //h_start

	_lcm_i2c_write_bytes(0x09, 0x00 );
	_lcm_i2c_write_bytes(0x0a, 0x00 );                                 //hsa
	_lcm_i2c_write_bytes(0x0b, (u8)( MIPI_Timing[hact] / 256 ) );
	_lcm_i2c_write_bytes(0x0c, (u8)( MIPI_Timing[hact] % 256 ) );      //hactive

	_lcm_i2c_write_bytes(0x0d, 0x00 );
	_lcm_i2c_write_bytes(0x0e, 0x00 );                                 //vtotal

	_lcm_i2c_write_bytes(0x11, 0x00 );
	_lcm_i2c_write_bytes(0x12, 0x00 );
	_lcm_i2c_write_bytes(0x14, 0x00 );
	_lcm_i2c_write_bytes(0x15, (u8)( MIPI_Timing[vact] / 256 ) );
	_lcm_i2c_write_bytes(0x16, (u8)( MIPI_Timing[vact] % 256 ) );      //vactive

#else

	_lcm_i2c_write_bytes(0x05, (u8)( MIPI_Timing[htotal] / 256 ) );
	_lcm_i2c_write_bytes(0x06, (u8)( MIPI_Timing[htotal] % 256 ) );
	_lcm_i2c_write_bytes(0x07, (u8)( ( MIPI_Timing[hs] + MIPI_Timing[hbp] ) / 256 ) );
	_lcm_i2c_write_bytes(0x08, (u8)( ( MIPI_Timing[hs] + MIPI_Timing[hbp] ) % 256 ) );
	_lcm_i2c_write_bytes(0x09, (u8)( MIPI_Timing[hs] / 256 ) );
	_lcm_i2c_write_bytes(0x0a, (u8)( MIPI_Timing[hs] % 256 ) );
	_lcm_i2c_write_bytes(0x0b, (u8)( MIPI_Timing[hact] / 256 ) );
	_lcm_i2c_write_bytes(0x0c, (u8)( MIPI_Timing[hact] % 256 ) );
	_lcm_i2c_write_bytes(0x0d, (u8)( MIPI_Timing[vtotal] / 256 ) );
	_lcm_i2c_write_bytes(0x0e, (u8)( MIPI_Timing[vtotal] % 256 ) );
	_lcm_i2c_write_bytes(0x11, (u8)( ( MIPI_Timing[vs] + MIPI_Timing[vbp] ) / 256 ) );
	_lcm_i2c_write_bytes(0x12, (u8)( ( MIPI_Timing[vs] + MIPI_Timing[vbp] ) % 256 ) );
	_lcm_i2c_write_bytes(0x14, (u8)( MIPI_Timing[vs] % 256 ) );
	_lcm_i2c_write_bytes(0x15, (u8)( MIPI_Timing[vact] / 256 ) );
	_lcm_i2c_write_bytes(0x16, (u8)( MIPI_Timing[vact] % 256 ) );
#endif
}

/***********************************************************/
void LT8911EXB_init( void )
{
	u8	i;
	u8	pcr_pll_postdiv;
	u8	pcr_m;
	u16 Temp16;

	/* init */
	_lcm_i2c_write_bytes(0xff, 0x81 ); // Change Reg bank
	_lcm_i2c_write_bytes(0x08, 0x7f ); // i2c over aux issue
	_lcm_i2c_write_bytes(0x49, 0xff ); // enable 0x87xx

	_lcm_i2c_write_bytes(0xff, 0x82 ); // Change Reg bank
	_lcm_i2c_write_bytes(0x5a, 0x0e ); // GPIO test output

	//for power consumption//
	_lcm_i2c_write_bytes(0xff, 0x81 );
	_lcm_i2c_write_bytes(0x05, 0x06 );
	_lcm_i2c_write_bytes(0x43, 0x00 );
	_lcm_i2c_write_bytes(0x44, 0x1f );
	_lcm_i2c_write_bytes(0x45, 0xf7 );
	_lcm_i2c_write_bytes(0x46, 0xf6 );
	_lcm_i2c_write_bytes(0x49, 0x7f );

	_lcm_i2c_write_bytes(0xff, 0x82 );
	_lcm_i2c_write_bytes(0x12, 0x33 );

	/* mipi Rx analog */
	_lcm_i2c_write_bytes(0xff, 0x82 ); // Change Reg bank
	_lcm_i2c_write_bytes(0x32, 0x51 );
	_lcm_i2c_write_bytes(0x35, 0x22 ); //EQ current
	_lcm_i2c_write_bytes(0x3a, 0x77 ); //EQ 12.5db
	_lcm_i2c_write_bytes(0x3b, 0x77 ); //EQ 12.5db

	_lcm_i2c_write_bytes(0x4c, 0x0c );
	_lcm_i2c_write_bytes(0x4d, 0x00 );

	/* dessc_pcr  pll analog */
	_lcm_i2c_write_bytes(0xff, 0x82 ); // Change Reg bank
	_lcm_i2c_write_bytes(0x6a, 0x40 );
	_lcm_i2c_write_bytes(0x6b, PCR_PLL_PREDIV );

	Temp16 = MIPI_Timing[pclk_10khz];

	if( MIPI_Timing[pclk_10khz] < 8800 )
	{
		_lcm_i2c_write_bytes(0x6e, 0x82 ); //0x44:pre-div = 2 ,pixel_clk=44~ 88MHz
		pcr_pll_postdiv = 0x08;
	}else
	{
		_lcm_i2c_write_bytes(0x6e, 0x81 ); //0x40:pre-div = 1, pixel_clk =88~176MHz
		pcr_pll_postdiv = 0x04;
	}

	pcr_m = (u8)( Temp16 * pcr_pll_postdiv / 25 / 100 );

	/* dessc pll digital */
	_lcm_i2c_write_bytes(0xff, 0x85 );     // Change Reg bank
	_lcm_i2c_write_bytes(0xa9, 0x31 );
	_lcm_i2c_write_bytes(0xaa, 0x17 );
	_lcm_i2c_write_bytes(0xab, 0xba );
	_lcm_i2c_write_bytes(0xac, 0xe1 );
	_lcm_i2c_write_bytes(0xad, 0x47 );
	_lcm_i2c_write_bytes(0xae, 0x01 );
	_lcm_i2c_write_bytes(0xae, 0x11 );

	/* Digital Top */
	_lcm_i2c_write_bytes(0xff, 0x85 );             // Change Reg bank
	_lcm_i2c_write_bytes(0xc0, 0x01 );             //select mipi Rx
#ifdef _6bit_
	_lcm_i2c_write_bytes(0xb0, 0xd0 );             //enable dither
#else
	_lcm_i2c_write_bytes(0xb0, 0x00 );             // disable dither
#endif

	/* mipi Rx Digital */
	_lcm_i2c_write_bytes(0xff, 0xd0 );             // Change Reg bank
	_lcm_i2c_write_bytes(0x00, _MIPI_Lane_ % 4 );  // 0: 4 Lane / 1: 1 Lane / 2 : 2 Lane / 3: 3 Lane
	_lcm_i2c_write_bytes(0x02, 0x08 );             //settle
	_lcm_i2c_write_bytes(0x08, 0x00 );
//	_lcm_i2c_write_bytes(0x0a, 0x12 );               //pcr mode

	_lcm_i2c_write_bytes(0x0c, 0x80 );             //fifo position
	_lcm_i2c_write_bytes(0x1c, 0x80 );             //fifo position

	//	hs mode:MIPI行采样；vs mode:MIPI帧采样
	_lcm_i2c_write_bytes(0x24, 0x70 );             // 0x30  [3:0]  line limit	  //pcr mode( de hs vs)

	_lcm_i2c_write_bytes(0x31, 0x0a );

	/*stage1 hs mode*/
	_lcm_i2c_write_bytes(0x25, 0x90 );             // 0x80		   // line limit
	_lcm_i2c_write_bytes(0x2a, 0x3a );             // 0x04		   // step in limit
	_lcm_i2c_write_bytes(0x21, 0x4f );             // hs_step
	_lcm_i2c_write_bytes(0x22, 0xff );

	/*stage2 de mode*/
	_lcm_i2c_write_bytes(0x0a, 0x02 );             //de adjust pre line
	_lcm_i2c_write_bytes(0x38, 0x02 );             //de_threshold 1
	_lcm_i2c_write_bytes(0x39, 0x04 );             //de_threshold 2
	_lcm_i2c_write_bytes(0x3a, 0x08 );             //de_threshold 3
	_lcm_i2c_write_bytes(0x3b, 0x10 );             //de_threshold 4

	_lcm_i2c_write_bytes(0x3f, 0x04 );             //de_step 1
	_lcm_i2c_write_bytes(0x40, 0x08 );             //de_step 2
	_lcm_i2c_write_bytes(0x41, 0x10 );             //de_step 3
	_lcm_i2c_write_bytes(0x42, 0x60 );             //de_step 4

	/*stage2 hs mode*/
	_lcm_i2c_write_bytes(0x1e, 0x0a );             // 0x11
	_lcm_i2c_write_bytes(0x23, 0xf0 );             // 0x80			   //

	_lcm_i2c_write_bytes(0x2b, 0x80 );             // 0xa0

#ifdef _Test_Pattern_
	_lcm_i2c_write_bytes(0x26, ( pcr_m | 0x80 ) );
#else

	_lcm_i2c_write_bytes(0x26, pcr_m );

//	_lcm_i2c_write_bytes(0x27, Read_0xD095 );
//	_lcm_i2c_write_bytes(0x28, Read_0xD096 );
#endif

	LT8911EXB_MIPI_Video_Timing( );         //defualt setting is 1080P

	_lcm_i2c_write_bytes(0xff, 0x81 ); // Change Reg bank
	_lcm_i2c_write_bytes(0x03, 0x7b ); //PCR reset
	_lcm_i2c_write_bytes(0x03, 0xff );

#ifdef _eDP_2G7_
	_lcm_i2c_write_bytes(0xff, 0x87 );
	_lcm_i2c_write_bytes(0x19, 0x31 );
	_lcm_i2c_write_bytes(0x1a, 0x36 ); // sync m
	_lcm_i2c_write_bytes(0x1b, 0x00 ); // sync_k [7:0]
	_lcm_i2c_write_bytes(0x1c, 0x00 ); // sync_k [13:8]

	// txpll Analog
	_lcm_i2c_write_bytes(0xff, 0x82 );
//	_lcm_i2c_write_bytes(0x01, 0x18 );// default : 0x18
	_lcm_i2c_write_bytes(0x02, 0x42 );
	_lcm_i2c_write_bytes(0x03, 0x00 ); // txpll en = 0
	_lcm_i2c_write_bytes(0x03, 0x01 ); // txpll en = 1
//	_lcm_i2c_write_bytes(0x04, 0x3a );// default : 0x3A

	_lcm_i2c_write_bytes(0xff, 0x87 );
	_lcm_i2c_write_bytes(0x0c, 0x10 ); // cal en = 0

	_lcm_i2c_write_bytes(0xff, 0x81 );
	_lcm_i2c_write_bytes(0x09, 0xfc );
	_lcm_i2c_write_bytes(0x09, 0xfd );

	_lcm_i2c_write_bytes(0xff, 0x87 );
	_lcm_i2c_write_bytes(0x0c, 0x11 ); // cal en = 1

	// ssc
	_lcm_i2c_write_bytes(0xff, 0x87 );
	_lcm_i2c_write_bytes(0x13, 0x83 );
	_lcm_i2c_write_bytes(0x14, 0x41 );
	_lcm_i2c_write_bytes(0x16, 0x0a );
	_lcm_i2c_write_bytes(0x18, 0x0a );
	_lcm_i2c_write_bytes(0x19, 0x33 );
#endif

#ifdef _eDP_1G62_
	_lcm_i2c_write_bytes(0xff, 0x87 );
	_lcm_i2c_write_bytes(0x19, 0x31 );
	_lcm_i2c_write_bytes(0x1a, 0x20 ); // sync m
	_lcm_i2c_write_bytes(0x1b, 0x19 ); // sync_k [7:0]
	_lcm_i2c_write_bytes(0x1c, 0x99 ); // sync_k [13:8]

	// txpll Analog
	_lcm_i2c_write_bytes(0xff, 0x82 );
	//	_lcm_i2c_write_bytes(0x01, 0x18 );// default : 0x18
	_lcm_i2c_write_bytes(0x02, 0x42 );
	_lcm_i2c_write_bytes(0x03, 0x00 ); // txpll en = 0
	_lcm_i2c_write_bytes(0x03, 0x01 ); // txpll en = 1
	//	_lcm_i2c_write_bytes(0x04, 0x3a );// default : 0x3A

	_lcm_i2c_write_bytes(0xff, 0x87 );
	_lcm_i2c_write_bytes(0x0c, 0x10 ); // cal en = 0

	_lcm_i2c_write_bytes(0xff, 0x81 );
	_lcm_i2c_write_bytes(0x09, 0xfc );
	_lcm_i2c_write_bytes(0x09, 0xfd );

	_lcm_i2c_write_bytes(0xff, 0x87 );
	_lcm_i2c_write_bytes(0x0c, 0x11 ); // cal en = 1

	//ssc
	_lcm_i2c_write_bytes(0xff, 0x87 );
	_lcm_i2c_write_bytes(0x13, 0x83 );
	_lcm_i2c_write_bytes(0x14, 0x41 );
	_lcm_i2c_write_bytes(0x16, 0x0a );
	_lcm_i2c_write_bytes(0x18, 0x0a );
	_lcm_i2c_write_bytes(0x19, 0x33 );
#endif

	_lcm_i2c_write_bytes(0xff, 0x87 );
#if 0
	for( i = 0; i < 5; i++ ) //Check Tx PLL
	{
		MDELAY( 5 );
		if( _lcm_i2c_read_bytes( 0x37 ) & 0x02 )
		{
			Debug_Printf( "\r\nLT8911 tx pll locked" );
			break;
		}else
		{
			Debug_Printf( "\r\nLT8911 tx pll unlocked" );
			_lcm_i2c_write_bytes(0xff, 0x81 );
			_lcm_i2c_write_bytes(0x09, 0xfc );
			_lcm_i2c_write_bytes(0x09, 0xfd );

			_lcm_i2c_write_bytes(0xff, 0x87 );
			_lcm_i2c_write_bytes(0x0c, 0x10 );
			_lcm_i2c_write_bytes(0x0c, 0x11 );
		}
	}
#endif
	// AUX reset
	_lcm_i2c_write_bytes(0xff, 0x81 ); // Change Reg bank
	_lcm_i2c_write_bytes(0x07, 0xfe );
	_lcm_i2c_write_bytes(0x07, 0xff );
	_lcm_i2c_write_bytes(0x0a, 0xfc );
	_lcm_i2c_write_bytes(0x0a, 0xfe );

	/* tx phy */
	_lcm_i2c_write_bytes(0xff, 0x82 ); // Change Reg bank
	_lcm_i2c_write_bytes(0x11, 0x00 );
	_lcm_i2c_write_bytes(0x13, 0x10 );
	_lcm_i2c_write_bytes(0x14, 0x0c );
	_lcm_i2c_write_bytes(0x14, 0x08 );
	_lcm_i2c_write_bytes(0x13, 0x20 );

	_lcm_i2c_write_bytes(0xff, 0x82 ); // Change Reg bank
	_lcm_i2c_write_bytes(0x0e, 0x35 );
//	_lcm_i2c_write_bytes(0x12, 0xff );
//	_lcm_i2c_write_bytes(0xff, 0x80 );
//	_lcm_i2c_write_bytes(0x40, 0x22 );

	/*eDP Tx Digital */
	_lcm_i2c_write_bytes(0xff, 0xa8 ); // Change Reg bank

#ifdef _Test_Pattern_

	_lcm_i2c_write_bytes(0x24, 0x50 ); // bit2 ~ bit 0 : test panttern image mode
	_lcm_i2c_write_bytes(0x25, 0x70 ); // bit6 ~ bit 4 : test Pattern color
	_lcm_i2c_write_bytes(0x27, 0x50 ); //0x50:Pattern; 0x10:mipi video

//	_lcm_i2c_write_bytes(0x2d, 0x00 ); //  pure color setting
//	_lcm_i2c_write_bytes(0x2d, 0x84 ); // black color
	_lcm_i2c_write_bytes(0x2d, 0x88 ); //  block

#else
	_lcm_i2c_write_bytes(0x27, 0x10 ); //0x50:Pattern; 0x10:mipi video
#endif

#ifdef _6bit_
	_lcm_i2c_write_bytes(0x17, 0x00 );
	_lcm_i2c_write_bytes(0x18, 0x00 );
#else
	// _8bit_
	_lcm_i2c_write_bytes(0x17, 0x10 );
	_lcm_i2c_write_bytes(0x18, 0x20 );
#endif

	/* nvid */
	_lcm_i2c_write_bytes(0xff, 0xa0 );                             // Change Reg bank
	_lcm_i2c_write_bytes(0x00, (u8)( Nvid_Val[_Nvid] / 256 ) );    // 0x08
	_lcm_i2c_write_bytes(0x01, (u8)( Nvid_Val[_Nvid] % 256 ) );    // 0x00
}

/* mipi should be ready before configuring below video check setting*/
/***********************************************************/

/***********************************************************/
void LT8911EXB_read_edid( void )
{
#ifdef _read_edid_
	u8 reg, i, j;
//	bool	aux_reply, aux_ack, aux_nack, aux_defer;
	_lcm_i2c_write_bytes(0xff, 0xac );
	_lcm_i2c_write_bytes(0x00, 0x20 ); //Soft Link train
	_lcm_i2c_write_bytes(0xff, 0xa6 );
	_lcm_i2c_write_bytes(0x2a, 0x01 );

	/*set edid offset addr*/
	_lcm_i2c_write_bytes(0x2b, 0x40 ); //CMD
	_lcm_i2c_write_bytes(0x2b, 0x00 ); //addr[15:8]
	_lcm_i2c_write_bytes(0x2b, 0x50 ); //addr[7:0]
	_lcm_i2c_write_bytes(0x2b, 0x00 ); //data lenth
	_lcm_i2c_write_bytes(0x2b, 0x00 ); //data lenth
	_lcm_i2c_write_bytes(0x2c, 0x00 ); //start Aux read edid

#ifdef _uart_debug_
	Debug_Printf( "\r\n" );
	Debug_Printf( "\r\nRead eDP EDID......" );
#endif

	MDELAY( 20 );                         //more than 10ms
	reg = _lcm_i2c_read_bytes( 0x25 );
	if( ( reg & 0x0f ) == 0x0c )
	{
		for( j = 0; j < 8; j++ )
		{
			if( j == 7 )
			{
				_lcm_i2c_write_bytes(0x2b, 0x10 ); //MOT
			}else
			{
				_lcm_i2c_write_bytes(0x2b, 0x50 );
			}

			_lcm_i2c_write_bytes(0x2b, 0x00 );
			_lcm_i2c_write_bytes(0x2b, 0x50 );
			_lcm_i2c_write_bytes(0x2b, 0x0f );
			_lcm_i2c_write_bytes(0x2c, 0x00 ); //start Aux read edid
			MDELAY( 50 );                         //more than 50ms

			if( _lcm_i2c_read_bytes( 0x39 ) == 0x31 )
			{
				_lcm_i2c_read_bytes( 0x2b );
				for( i = 0; i < 16; i++ )
				{
					EDID_DATA[j * 16 + i] = _lcm_i2c_read_bytes( 0x2b );
				}

				EDID_Reply = 1;
			}else
			{
				EDID_Reply = 0;
#ifdef _uart_debug_
				Debug_Printf( "\r\nno_reply" );
				Debug_Printf( "\r\n" );
#endif
				//		print("\r\n*************End***************");
				return;
			}
		}

#ifdef _uart_debug_

		for( i = 0; i < 128; i++ ) //print edid data
		{
			if( ( i % 16 ) == 0 )
			{
				Debug_Printf( "\r\n" );
			}
			Debug_DispStrNum( ", ", EDID_DATA[i] );
		}

		Debug_Printf( "\r\n" );
		Debug_Printf( "\r\neDP Timing = { H_FP / H_pluse / H_BP / H_act / H_tol / V_FP / V_pluse / V_BP / V_act / V_tol / D_CLK };" );
		Debug_Printf( "\r\neDP Timing = { " );
		EDID_Timing[hfp] = ( ( EDID_DATA[0x41] & 0xC0 ) * 4 + EDID_DATA[0x3e] );
		Debug_DispNum( (u32)EDID_Timing[hfp] );         // HFB
		Debug_Printf( ", " );

		EDID_Timing[hs] = ( ( EDID_DATA[0x41] & 0x30 ) * 16 + EDID_DATA[0x3f] );
		Debug_DispNum( (u32)EDID_Timing[hs] );          // Hsync Wid
		Debug_Printf( ", " );

		EDID_Timing[hbp] = ( ( ( EDID_DATA[0x3a] & 0x0f ) * 0x100 + EDID_DATA[0x39] ) - ( ( EDID_DATA[0x41] & 0x30 ) * 16 + EDID_DATA[0x3f] ) - ( ( EDID_DATA[0x41] & 0xC0 ) * 4 + EDID_DATA[0x3e] ) );
		Debug_DispNum( (u32)EDID_Timing[hbp] );         // HBP
		Debug_Printf( ", " );

		EDID_Timing[hact] = ( ( EDID_DATA[0x3a] & 0xf0 ) * 16 + EDID_DATA[0x38] );
		Debug_DispNum( (u32)EDID_Timing[hact] );        // H active
		Debug_Printf( ", " );

		EDID_Timing[htotal] = ( ( EDID_DATA[0x3a] & 0xf0 ) * 16 + EDID_DATA[0x38] + ( ( EDID_DATA[0x3a] & 0x0f ) * 0x100 + EDID_DATA[0x39] ) );
		Debug_DispNum( (u32)EDID_Timing[htotal] );      // H total
		Debug_Printf( ", " );

		EDID_Timing[vfp] = ( ( EDID_DATA[0x41] & 0x0c ) * 4 + ( EDID_DATA[0x40] & 0xf0 ) / 16 );
		Debug_DispNum( (u32)EDID_Timing[vfp] );         // VFB
		Debug_Printf( ", " );

		EDID_Timing[vs] = ( ( EDID_DATA[0x41] & 0x03 ) * 16 + EDID_DATA[0x40] & 0x0f );
		Debug_DispNum( (u32)EDID_Timing[vs] );          // Vsync Wid
		Debug_Printf( ", " );

		EDID_Timing[vbp] = ( ( ( EDID_DATA[0x3d] & 0x03 ) * 0x100 + EDID_DATA[0x3c] ) - ( ( EDID_DATA[0x41] & 0x03 ) * 16 + EDID_DATA[0x40] & 0x0f ) - ( ( EDID_DATA[0x41] & 0x0c ) * 4 + ( EDID_DATA[0x40] & 0xf0 ) / 16 ) );
		Debug_DispNum( (u32)EDID_Timing[vbp] );         // VBP
		Debug_Printf( ", " );

		EDID_Timing[vact] = ( ( EDID_DATA[0x3d] & 0xf0 ) * 16 + EDID_DATA[0x3b] );
		Debug_DispNum( (u32)EDID_Timing[vact] );        // V active
		Debug_Printf( ", " );

		EDID_Timing[vtotal] = ( ( EDID_DATA[0x3d] & 0xf0 ) * 16 + EDID_DATA[0x3b] + ( ( EDID_DATA[0x3d] & 0x03 ) * 0x100 + EDID_DATA[0x3c] ) );
		Debug_DispNum( (u32)EDID_Timing[vtotal] );      // V total
		Debug_Printf( ", " );

		EDID_Timing[pclk_10khz] = ( EDID_DATA[0x37] * 0x100 + EDID_DATA[0x36] );
		Debug_DispNum( (u32)EDID_Timing[pclk_10khz] );  // CLK
		Debug_Printf( " };" );
		Debug_Printf( "\r\n" );
#endif
	}

	return;

#endif
}

/***********************************************************/
void LT8911EX_link_train( void )
{
	_lcm_i2c_write_bytes(0xff, 0x85 );

//#ifdef _eDP_scramble_
	if( ScrambleMode )
	{
		_lcm_i2c_write_bytes(0xa1, 0x82 ); // eDP scramble mode;

		/* Aux operater init */


/*
   _lcm_i2c_write_bytes(0xff, 0xac );
   _lcm_i2c_write_bytes(0x00, 0x20 ); //Soft Link train
   _lcm_i2c_write_bytes(0xff, 0xa6 );
   _lcm_i2c_write_bytes(0x2a, 0x01 );

   DpcdWrite( 0x010a, 0x01 );
   MDELAY( 10 );
   DpcdWrite( 0x0102, 0x00 );
   MDELAY( 10 );
   DpcdWrite( 0x010a, 0x01 );

   MDELAY( 200 );
   //*/
	}
//#else
	else
	{
		_lcm_i2c_write_bytes(0xa1, 0x02 ); // DP scramble mode;
	}
//#endif

	/* Aux setup */
	_lcm_i2c_write_bytes(0xff, 0xac );
	_lcm_i2c_write_bytes(0x00, 0x60 );     //Soft Link train
	_lcm_i2c_write_bytes(0xff, 0xa6 );
	_lcm_i2c_write_bytes(0x2a, 0x00 );

	_lcm_i2c_write_bytes(0xff, 0x81 );
	_lcm_i2c_write_bytes(0x07, 0xfe );
	_lcm_i2c_write_bytes(0x07, 0xff );
	_lcm_i2c_write_bytes(0x0a, 0xfc );
	_lcm_i2c_write_bytes(0x0a, 0xfe );

	/* link train */

	_lcm_i2c_write_bytes(0xff, 0x85 );
	_lcm_i2c_write_bytes(0x1a, eDP_lane );

	_lcm_i2c_write_bytes(0xff, 0xac );
	_lcm_i2c_write_bytes(0x00, 0x64 );
	_lcm_i2c_write_bytes(0x01, 0x0a );
	_lcm_i2c_write_bytes(0x0c, 0x85 );
	_lcm_i2c_write_bytes(0x0c, 0xc5 );
//	MDELAY( 500 );
}

//-------------------------------------------

//*
void LT8911EX_link_train_result( void )
{
	u8 i, reg;
	_lcm_i2c_write_bytes(0xff, 0xac );
	for( i = 0; i < 10; i++ )
	{
		reg = _lcm_i2c_read_bytes( 0x82 );
		//  Debug_DispStrNum( "\r\n0x82 = ", reg );
		if( reg & 0x20 )
		{
			if( ( reg & 0x1f ) == 0x1e )
			{
				Debug_DispStrNum( "\r\nLink train success, 0x82 = ", reg );
			} else
			{
				Debug_DispStrNum( "\r\nLink train fail, 0x82 = ", reg );
			}

			Debug_DispStrNum( "\r\npanel link rate: ", _lcm_i2c_read_bytes( 0x83 ) );
			Debug_DispStrNum( "\r\npanel link count: ", _lcm_i2c_read_bytes( 0x84 ) );
			return;
		}else
		{
			print( "\r\nlink trian on going..." );
		}
		MDELAY( 100 );
	}
}

void LT8911EX_TxSwingPreSet( void )
{
	_lcm_i2c_write_bytes(0xFF, 0x82 );
	_lcm_i2c_write_bytes(0x22, Swing_Setting1[Level] );    //lane 0 tap0
	_lcm_i2c_write_bytes(0x23, Swing_Setting2[Level] );
	_lcm_i2c_write_bytes(0x24, 0x80 );                     //lane 0 tap1
	_lcm_i2c_write_bytes(0x25, 0x00 );
	_lcm_i2c_write_bytes(0x26, Swing_Setting1[Level] );    //lane 1 tap0
	_lcm_i2c_write_bytes(0x27, Swing_Setting2[Level] );
	_lcm_i2c_write_bytes(0x28, 0x80 );                     //lane 1 tap1
	_lcm_i2c_write_bytes(0x29, 0x00 );
}

void LT8911EXB_LinkTrainResultCheck( void )
{
#if 1 // def _link_train_enable_
	u8	i;
	u8	val;
	//int ret;

	_lcm_i2c_write_bytes(0xff, 0xac );
	for( i = 0; i < 10; i++ )
	{
		val = _lcm_i2c_read_bytes( 0x82 );
		if( val & 0x20 )
		{
			if( ( val & 0x1f ) == 0x1e )
			{
#ifdef _uart_debug_
				//   printf("\r\nLT8911_LinkTrainResultCheck: edp link train successed: 0x%bx", val);
				Debug_DispStrNum( "\r\nedp link train successed: ", val );
#endif
			}else
			{
#ifdef _uart_debug_
				//printf("\r\nLT8911_LinkTrainResultCheck: edp link train failed: 0x%bx", val);
				Debug_DispStrNum( "\r\nedp link train failed: ", val );
#endif
				_lcm_i2c_write_bytes(0xff, 0xac );
				_lcm_i2c_write_bytes(0x00, 0x00 );
				_lcm_i2c_write_bytes(0x01, 0x0a );
				_lcm_i2c_write_bytes(0x14, 0x80 );
				_lcm_i2c_write_bytes(0x14, 0x81 );
				MDELAY( 50 );
				_lcm_i2c_write_bytes(0x14, 0x84 );
				MDELAY( 50 );
				_lcm_i2c_write_bytes(0x14, 0xc0 );
				//printf("\r\nLT8911_LinkTrainResultCheck: Enable eDP video output while linktrian fail");
			}

#ifdef _uart_debug_

			val = _lcm_i2c_read_bytes( 0x83 );
			//printf("\r\nLT8911_LinkTrainResultCheck: panel link rate: 0x%bx",val);
			Debug_DispStrNum( "\r\npanel link rate: ", val );
			val = _lcm_i2c_read_bytes( 0x84 );
			//printf("\r\nLT8911_LinkTrainResultCheck: panel link count: 0x%bx",val);
			Debug_DispStrNum( "\r\npanel link count: ", val );
#endif
			return;
		}else
		{
			//printf("\r\nLT8911_LinkTrainResultCheck: link trian on going...");
			MDELAY( 100 );
		}
	}
#endif
}

u8 DpcdRead( u32 Address )
{
	/***************************
	   注意大小端的问题!
	   这里默认是大端模式

	   Pay attention to the Big-Endian and Little-Endian!
	   The default mode is Big-Endian here.

	 ****************************/

	u8	DpcdValue  = 0x00;
	u8	AddressH   = 0x0f & ( Address >> 16 );
	u8	AddressM   = 0xff & ( Address >> 8 );
	u8	AddressL   = 0xff & Address;
	u8	reg;

	_lcm_i2c_write_bytes( 0xff, 0xac );
	_lcm_i2c_write_bytes( 0x00, 0x20 );                 //Soft Link train
	_lcm_i2c_write_bytes( 0xff, 0xa6 );
	_lcm_i2c_write_bytes( 0x2a, 0x01 );

	_lcm_i2c_write_bytes( 0xff, 0xa6 );
	_lcm_i2c_write_bytes( 0x2b, ( 0x90 | AddressH ) );  //CMD
	_lcm_i2c_write_bytes( 0x2b, AddressM );             //addr[15:8]
	_lcm_i2c_write_bytes( 0x2b, AddressL );             //addr[7:0]
	_lcm_i2c_write_bytes( 0x2b, 0x00 );                 //data lenth
	_lcm_i2c_write_bytes( 0x2c, 0x00 );                 //start Aux read edid

	MDELAY( 50 );                                         //more than 10ms
	reg = _lcm_i2c_read_bytes( 0x25 );
	if( ( reg & 0x0f ) == 0x0c )
	{
		if( _lcm_i2c_read_bytes( 0x39 ) == 0x22 )
		{
			_lcm_i2c_read_bytes( 0x2b );
			DpcdValue = _lcm_i2c_read_bytes( 0x2b );
		}


		/*
		   else
		   {
		   //	goto no_reply;
		   //	DpcdValue = 0xff;
		   return DpcdValue;
		   }//*/
	}else
	{
		_lcm_i2c_write_bytes( 0xff, 0x81 ); // change bank
		_lcm_i2c_write_bytes( 0x07, 0xfe );
		_lcm_i2c_write_bytes( 0x07, 0xff );
		_lcm_i2c_write_bytes( 0x0a, 0xfc );
		_lcm_i2c_write_bytes( 0x0a, 0xfe );
	}

	return DpcdValue;
}

//------------------------------------------------------------------
/***********************************************************************8/

/* turn on gate ic & control voltage to 5.5V */
static void lcm_init_mipi_to_edp(void)
{
	Reset_LT8911EXB( );     // 先Reset LT8911EXB ,用GPIO 先拉低LT8911EXB的复位脚 100ms左右，再拉高，保持100ms。

	LT8911EX_ChipID( );     // read Chip ID

	LT8911EXB_eDP_Video_cfg( );
	LT8911EXB_init( );

	LT8911EXB_read_edid( ); // for debug

	Read_DPCD010A = DpcdRead( 0x010A ) & 0x01;
#if 1
//#ifdef _uart_debug_
	printf( "%s \n",__func__ );
	printf( "DPCD010Ah:%x \n", Read_DPCD010A );
#endif

	if( Read_DPCD010A )
	{
		ScrambleMode = 1;
	}else
	{
		ScrambleMode = 0;
	}

	LT8911EX_link_train( );

	LT8911EX_TxSwingPreSet( );

	LT8911EXB_LinkTrainResultCheck( );
}

LCM_DRIVER mt8365_fhdnl_dsi_vdo_auto_lcm_drv= {
	.name               = "mt8365_fhdnl_dsi_vdo_auto",
	.set_util_funcs     = lcm_set_util_funcs,
	.get_params         = lcm_get_params,
	.init               = lcm_init,
	.compare_id         = lcm_compare_id,
};
