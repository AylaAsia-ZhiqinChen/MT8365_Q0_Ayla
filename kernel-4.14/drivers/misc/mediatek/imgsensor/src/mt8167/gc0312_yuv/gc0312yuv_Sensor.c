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

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   gc0312yuv_Sensor.c
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 *   Image sensor driver function
 *   V1.2.3
 *
 * Author:
 * -------
 *   lanking zhou
 *
 *=============================================================
 *	       HISTORY
 * Below this line, this part is controlled by GCoreinc. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 *
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by GCoreinc. DO NOT MODIFY!!
 *=============================================================
 ******************************************************************************/
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/atomic.h>

#include "kd_camera_hw.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"

#include "gc0312yuv_Sensor.h"

#undef GC0312YUV_DEBUG
/* #define GC0312YUV_DEBUG */
#ifdef GC0312YUV_DEBUG
#define SENSORDB pr_info
#else
#define SENSORDB(x, ...)
#endif


kal_uint16 GC0312_write_cmos_sensor(kal_uint8 addr, kal_uint8 para)
{
	char puSendCmd[2] = { (char)(addr & 0xFF), (char)(para & 0xFF) };

	iWriteRegI2C(puSendCmd, 2, GC0312_WRITE_ID);
	return 0;

}

kal_uint16 GC0312_read_cmos_sensor(kal_uint8 addr)
{
	kal_uint16 get_byte = 0;
	char puSendCmd = { (char)(addr & 0xFF) };

	iReadRegI2C(&puSendCmd, 1, (u8 *) &get_byte, 1, GC0312_WRITE_ID);

	return get_byte;
}


/*
 **************************************************
 * // Adapter for Winmo typedef
 ***************************************************
 */
#define WINMO_USE 0

#define Sleep(ms) mdelay(ms)
#define RETAILMSG(x, ...)
#define TEXT

kal_bool GC0312_MPEG4_encode_mode = KAL_FALSE;
kal_uint16 GC0312_dummy_pixels = 0, GC0312_dummy_lines = 0;
kal_bool GC0312_MODE_CAPTURE = KAL_FALSE;
kal_bool GC0312_NIGHT_MODE = KAL_FALSE;

kal_uint32 GC0312_isp_master_clock;
static kal_uint32 GC0312_g_fPV_PCLK = 26;

kal_uint8 GC0312_sensor_write_I2C_address = GC0312_WRITE_ID;
kal_uint8 GC0312_sensor_read_I2C_address = GC0312_READ_ID;

UINT8 GC0312PixelClockDivider;

MSDK_SENSOR_CONFIG_STRUCT GC0312SensorConfigData;

static DEFINE_SPINLOCK(imgsensor_drv_lock);

#define GC0312_SET_PAGE0	GC0312_write_cmos_sensor(0xfe, 0x00)
#define GC0312_SET_PAGE1	GC0312_write_cmos_sensor(0xfe, 0x01)


/*************************************************************************
 * FUNCTION
 *	GC0312_SetShutter
 *
 * DESCRIPTION
 *	This function set e-shutter of GC0312 to change exposure time.
 *
 * PARAMETERS
 *   iShutter : exposured lines
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
void GC0312_Set_Shutter(kal_uint16 iShutter)
{
}				/* Set_GC0312_Shutter */


/*************************************************************************
 * FUNCTION
 *	GC0312_read_Shutter
 *
 * DESCRIPTION
 *	This function read e-shutter of GC0312 .
 *
 * PARAMETERS
 *  None
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
kal_uint16 GC0312_Read_Shutter(void)
{
	kal_uint8 temp_reg1, temp_reg2;
	kal_uint16 shutter;

	temp_reg1 = GC0312_read_cmos_sensor(0x04);
	temp_reg2 = GC0312_read_cmos_sensor(0x03);

	shutter = (temp_reg1 & 0xFF) | (temp_reg2 << 8);

	return shutter;
}				/* GC0312_read_shutter */


/*************************************************************************
 * FUNCTION
 *	GC0312_write_reg
 *
 * DESCRIPTION
 *	This function set the register of GC0312.
 *
 * PARAMETERS
 *	addr : the register index of GC0312
 *  para : setting parameter of the specified register of GC0312
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
void GC0312_write_reg(kal_uint32 addr, kal_uint32 para)
{
	GC0312_write_cmos_sensor(addr, para);
}				/* GC0312_write_reg() */


/*************************************************************************
 * FUNCTION
 *	GC0312_read_cmos_sensor
 *
 * DESCRIPTION
 *	This function read parameter of specified register from GC0312.
 *
 * PARAMETERS
 *	addr : the register index of GC0312
 *
 * RETURNS
 *	the data that read from GC0312
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
kal_uint32 GC0312_read_reg(kal_uint32 addr)
{
	return GC0312_read_cmos_sensor(addr);
}				/* OV7670_read_reg() */


/*
 ************************************************************************
 * FUNCTION
 *	GC0312_awb_enable
 *
 * DESCRIPTION
 *	This function enable or disable the awb (Auto White Balance).
 *
 * PARAMETERS
 *	1. kal_bool : KAL_TRUE - enable awb, KAL_FALSE - disable awb.
 *
 * RETURNS
 *	kal_bool : It means set awb right or not.
 *
 ************************************************************************
 */
static void GC0312_awb_enable(kal_bool enalbe)
{
	kal_uint16 temp_AWB_reg = 0;

	temp_AWB_reg = GC0312_read_cmos_sensor(0x42);

	if (enalbe)
		GC0312_write_cmos_sensor(0x42, (temp_AWB_reg | 0x02));
	else
		GC0312_write_cmos_sensor(0x42, (temp_AWB_reg & (~0x02)));
}


/*
 ************************************************************************
 * FUNCTION
 *	GC0312_GAMMA_Select
 *
 * DESCRIPTION
 *	This function is served for FAE to select the appropriate GAMMA curve.
 *
 * PARAMETERS
 *	None
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 ************************************************************************
 */
void GC0312GammaSelect(kal_uint32 GammaLvl)
{
	switch (GammaLvl) {
	case GC0312_RGB_Gamma_m1:	/* smallest gamma curve */
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0xbf, 0x06);
		GC0312_write_cmos_sensor(0xc0, 0x12);
		GC0312_write_cmos_sensor(0xc1, 0x22);
		GC0312_write_cmos_sensor(0xc2, 0x35);
		GC0312_write_cmos_sensor(0xc3, 0x4b);
		GC0312_write_cmos_sensor(0xc4, 0x5f);
		GC0312_write_cmos_sensor(0xc5, 0x72);
		GC0312_write_cmos_sensor(0xc6, 0x8d);
		GC0312_write_cmos_sensor(0xc7, 0xa4);
		GC0312_write_cmos_sensor(0xc8, 0xb8);
		GC0312_write_cmos_sensor(0xc9, 0xc8);
		GC0312_write_cmos_sensor(0xca, 0xd4);
		GC0312_write_cmos_sensor(0xcb, 0xde);
		GC0312_write_cmos_sensor(0xcc, 0xe6);
		GC0312_write_cmos_sensor(0xcd, 0xf1);
		GC0312_write_cmos_sensor(0xce, 0xf8);
		GC0312_write_cmos_sensor(0xcf, 0xfd);
		break;
	case GC0312_RGB_Gamma_m2:
		GC0312_write_cmos_sensor(0xBF, 0x08);
		GC0312_write_cmos_sensor(0xc0, 0x0F);
		GC0312_write_cmos_sensor(0xc1, 0x21);
		GC0312_write_cmos_sensor(0xc2, 0x32);
		GC0312_write_cmos_sensor(0xc3, 0x43);
		GC0312_write_cmos_sensor(0xc4, 0x50);
		GC0312_write_cmos_sensor(0xc5, 0x5E);
		GC0312_write_cmos_sensor(0xc6, 0x78);
		GC0312_write_cmos_sensor(0xc7, 0x90);
		GC0312_write_cmos_sensor(0xc8, 0xA6);
		GC0312_write_cmos_sensor(0xc9, 0xB9);
		GC0312_write_cmos_sensor(0xcA, 0xC9);
		GC0312_write_cmos_sensor(0xcB, 0xD6);
		GC0312_write_cmos_sensor(0xcC, 0xE0);
		GC0312_write_cmos_sensor(0xcD, 0xEE);
		GC0312_write_cmos_sensor(0xcE, 0xF8);
		GC0312_write_cmos_sensor(0xcF, 0xFF);
		break;

	case GC0312_RGB_Gamma_m3:
		GC0312_write_cmos_sensor(0xbf, 0x0b);
		GC0312_write_cmos_sensor(0xc0, 0x17);
		GC0312_write_cmos_sensor(0xc1, 0x2a);
		GC0312_write_cmos_sensor(0xc2, 0x41);
		GC0312_write_cmos_sensor(0xc3, 0x54);
		GC0312_write_cmos_sensor(0xc4, 0x66);
		GC0312_write_cmos_sensor(0xc5, 0x74);
		GC0312_write_cmos_sensor(0xc6, 0x8c);
		GC0312_write_cmos_sensor(0xc7, 0xa3);
		GC0312_write_cmos_sensor(0xc8, 0xb5);
		GC0312_write_cmos_sensor(0xc9, 0xc4);
		GC0312_write_cmos_sensor(0xca, 0xd0);
		GC0312_write_cmos_sensor(0xcb, 0xdb);
		GC0312_write_cmos_sensor(0xcc, 0xe5);
		GC0312_write_cmos_sensor(0xcd, 0xf0);
		GC0312_write_cmos_sensor(0xce, 0xf7);
		GC0312_write_cmos_sensor(0xcf, 0xff);
		break;

	case GC0312_RGB_Gamma_m4:
		GC0312_write_cmos_sensor(0xBF, 0x0E);
		GC0312_write_cmos_sensor(0xc0, 0x1C);
		GC0312_write_cmos_sensor(0xc1, 0x34);
		GC0312_write_cmos_sensor(0xc2, 0x48);
		GC0312_write_cmos_sensor(0xc3, 0x5A);
		GC0312_write_cmos_sensor(0xc4, 0x6B);
		GC0312_write_cmos_sensor(0xc5, 0x7B);
		GC0312_write_cmos_sensor(0xc6, 0x95);
		GC0312_write_cmos_sensor(0xc7, 0xAB);
		GC0312_write_cmos_sensor(0xc8, 0xBF);
		GC0312_write_cmos_sensor(0xc9, 0xCE);
		GC0312_write_cmos_sensor(0xcA, 0xD9);
		GC0312_write_cmos_sensor(0xcB, 0xE4);
		GC0312_write_cmos_sensor(0xcC, 0xEC);
		GC0312_write_cmos_sensor(0xcD, 0xF7);
		GC0312_write_cmos_sensor(0xcE, 0xFD);
		GC0312_write_cmos_sensor(0xcF, 0xFF);
		break;

	case GC0312_RGB_Gamma_m5:
		GC0312_write_cmos_sensor(0xBF, 0x10);
		GC0312_write_cmos_sensor(0xc0, 0x20);
		GC0312_write_cmos_sensor(0xc1, 0x38);
		GC0312_write_cmos_sensor(0xc2, 0x4E);
		GC0312_write_cmos_sensor(0xc3, 0x63);
		GC0312_write_cmos_sensor(0xc4, 0x76);
		GC0312_write_cmos_sensor(0xc5, 0x87);
		GC0312_write_cmos_sensor(0xc6, 0xA2);
		GC0312_write_cmos_sensor(0xc7, 0xB8);
		GC0312_write_cmos_sensor(0xc8, 0xCA);
		GC0312_write_cmos_sensor(0xc9, 0xD8);
		GC0312_write_cmos_sensor(0xcA, 0xE3);
		GC0312_write_cmos_sensor(0xcB, 0xEB);
		GC0312_write_cmos_sensor(0xcC, 0xF0);
		GC0312_write_cmos_sensor(0xcD, 0xF8);
		GC0312_write_cmos_sensor(0xcE, 0xFD);
		GC0312_write_cmos_sensor(0xcF, 0xFF);
		break;

	case GC0312_RGB_Gamma_m6:	/* largest gamma curve */
		GC0312_write_cmos_sensor(0xBF, 0x14);
		GC0312_write_cmos_sensor(0xc0, 0x28);
		GC0312_write_cmos_sensor(0xc1, 0x44);
		GC0312_write_cmos_sensor(0xc2, 0x5D);
		GC0312_write_cmos_sensor(0xc3, 0x72);
		GC0312_write_cmos_sensor(0xc4, 0x86);
		GC0312_write_cmos_sensor(0xc5, 0x95);
		GC0312_write_cmos_sensor(0xc6, 0xB1);
		GC0312_write_cmos_sensor(0xc7, 0xC6);
		GC0312_write_cmos_sensor(0xc8, 0xD5);
		GC0312_write_cmos_sensor(0xc9, 0xE1);
		GC0312_write_cmos_sensor(0xcA, 0xEA);
		GC0312_write_cmos_sensor(0xcB, 0xF1);
		GC0312_write_cmos_sensor(0xcC, 0xF5);
		GC0312_write_cmos_sensor(0xcD, 0xFB);
		GC0312_write_cmos_sensor(0xcE, 0xFE);
		GC0312_write_cmos_sensor(0xcF, 0xFF);
		break;
	case GC0312_RGB_Gamma_night:	/* Gamma for night mode */
		GC0312_write_cmos_sensor(0xBF, 0x0B);
		GC0312_write_cmos_sensor(0xc0, 0x16);
		GC0312_write_cmos_sensor(0xc1, 0x29);
		GC0312_write_cmos_sensor(0xc2, 0x3C);
		GC0312_write_cmos_sensor(0xc3, 0x4F);
		GC0312_write_cmos_sensor(0xc4, 0x5F);
		GC0312_write_cmos_sensor(0xc5, 0x6F);
		GC0312_write_cmos_sensor(0xc6, 0x8A);
		GC0312_write_cmos_sensor(0xc7, 0x9F);
		GC0312_write_cmos_sensor(0xc8, 0xB4);
		GC0312_write_cmos_sensor(0xc9, 0xC6);
		GC0312_write_cmos_sensor(0xcA, 0xD3);
		GC0312_write_cmos_sensor(0xcB, 0xDD);
		GC0312_write_cmos_sensor(0xcC, 0xE5);
		GC0312_write_cmos_sensor(0xcD, 0xF1);
		GC0312_write_cmos_sensor(0xcE, 0xFA);
		GC0312_write_cmos_sensor(0xcF, 0xFF);
		break;
	default:
		/* GC0312_RGB_Gamma_m3 */
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0xbf, 0x0b);
		GC0312_write_cmos_sensor(0xc0, 0x17);
		GC0312_write_cmos_sensor(0xc1, 0x2a);
		GC0312_write_cmos_sensor(0xc2, 0x41);
		GC0312_write_cmos_sensor(0xc3, 0x54);
		GC0312_write_cmos_sensor(0xc4, 0x66);
		GC0312_write_cmos_sensor(0xc5, 0x74);
		GC0312_write_cmos_sensor(0xc6, 0x8c);
		GC0312_write_cmos_sensor(0xc7, 0xa3);
		GC0312_write_cmos_sensor(0xc8, 0xb5);
		GC0312_write_cmos_sensor(0xc9, 0xc4);
		GC0312_write_cmos_sensor(0xca, 0xd0);
		GC0312_write_cmos_sensor(0xcb, 0xdb);
		GC0312_write_cmos_sensor(0xcc, 0xe5);
		GC0312_write_cmos_sensor(0xcd, 0xf0);
		GC0312_write_cmos_sensor(0xce, 0xf7);
		GC0312_write_cmos_sensor(0xcf, 0xff);
		break;
	}
}


/*
 ************************************************************************
 * FUNCTION
 *	GC0312_config_window
 *
 * DESCRIPTION
 *	This function config the hardware window of GC0312 for getting specified
 *  data of that window.
 *
 * PARAMETERS
 *	start_x : start column of the interested window
 *  start_y : start row of the interested window
 *  width  : column widht of the itnerested window
 *  height : row depth of the itnerested window
 *
 * RETURNS
 *	the data that read from GC0312
 *
 * GLOBALS AFFECTED
 *
 ************************************************************************
 */
void GC0312_config_window(kal_uint16 startx,
	kal_uint16 starty, kal_uint16 width, kal_uint16 height)
{
}				/* GC0312_config_window */


/*************************************************************************
 * FUNCTION
 *	GC0312_SetGain
 *
 * DESCRIPTION
 *	This function is to set global gain to sensor.
 *
 * PARAMETERS
 *   iGain : sensor global gain(base: 0x40)
 *
 * RETURNS
 *	the actually gain set to sensor.
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
kal_uint16 GC0312_SetGain(kal_uint16 iGain)
{
	return iGain;
}


/*************************************************************************
 * FUNCTION
 *	GC0312_NightMode
 *
 * DESCRIPTION
 *	This function night mode of GC0312.
 *
 * PARAMETERS
 *	bEnable: KAL_TRUE -> enable night mode, otherwise, disable night mode
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
void GC0312NightMode(kal_bool bEnable)
{
	if (bEnable) {
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x3c, 0x30);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		/* GC0312GammaSelect(GC0312_RGB_Gamma_night); */
		GC0312_NIGHT_MODE = KAL_TRUE;
	} else {
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x3c, 0x20);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		/* GC0312GammaSelect(GC0312_RGB_Gamma_m3); */
		GC0312_NIGHT_MODE = KAL_FALSE;
	}
}				/* GC0312_NightMode */

/*
 ************************************************************************
 * FUNCTION
 *	GC0312_Sensor_Init
 *
 * DESCRIPTION
 *	This function apply all of the initial setting to sensor.
 *
 * PARAMETERS
 *	None
 *
 * RETURNS
 *	None
 *
 ************************************************************************
 */
void GC0312_Sensor_Init(void)
{
	GC0312_write_cmos_sensor(0xfe, 0xf0);
	GC0312_write_cmos_sensor(0xfe, 0xf0);
	GC0312_write_cmos_sensor(0xfe, 0x00);
	GC0312_write_cmos_sensor(0xfc, 0x0e);
	GC0312_write_cmos_sensor(0xfc, 0x0e);
	GC0312_write_cmos_sensor(0xf2, 0x07);
	GC0312_write_cmos_sensor(0xf3, 0x00);	/* output_disable */
	GC0312_write_cmos_sensor(0xf7, 0x1b);
	GC0312_write_cmos_sensor(0xf8, 0x04);
	GC0312_write_cmos_sensor(0xf9, 0x0e);
	GC0312_write_cmos_sensor(0xfa, 0x11);

	/* /////////////////////////////////////////////// */
	/* ///////////////  CISCTL reg   ///////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0x00, 0x2f);
	GC0312_write_cmos_sensor(0x01, 0x0f);	/* 06 */
	GC0312_write_cmos_sensor(0x02, 0x04);
	GC0312_write_cmos_sensor(0x03, 0x03);
	GC0312_write_cmos_sensor(0x04, 0x50);
	GC0312_write_cmos_sensor(0x09, 0x00);
	GC0312_write_cmos_sensor(0x0a, 0x00);
	GC0312_write_cmos_sensor(0x0b, 0x00);
	GC0312_write_cmos_sensor(0x0c, 0x04);
	GC0312_write_cmos_sensor(0x0d, 0x01);
	GC0312_write_cmos_sensor(0x0e, 0xe8);
	GC0312_write_cmos_sensor(0x0f, 0x02);
	GC0312_write_cmos_sensor(0x10, 0x88);
	GC0312_write_cmos_sensor(0x16, 0x00);
	GC0312_write_cmos_sensor(0x17, 0x17);
	GC0312_write_cmos_sensor(0x18, 0x1a);
	GC0312_write_cmos_sensor(0x19, 0x14);
	GC0312_write_cmos_sensor(0x1b, 0x48);
	GC0312_write_cmos_sensor(0x1e, 0x6b);
	GC0312_write_cmos_sensor(0x1f, 0x28);
	GC0312_write_cmos_sensor(0x20, 0x8b);
	GC0312_write_cmos_sensor(0x21, 0x49);
	GC0312_write_cmos_sensor(0x22, 0xb0);
	GC0312_write_cmos_sensor(0x23, 0x04);
	GC0312_write_cmos_sensor(0x24, 0x17);
	GC0312_write_cmos_sensor(0x34, 0x20);

	/* /////////////////////////////////////////////// */
	/* //////////////////   BLK       //////////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0x26, 0x23);
	GC0312_write_cmos_sensor(0x28, 0xff);
	GC0312_write_cmos_sensor(0x29, 0x00);
	GC0312_write_cmos_sensor(0x33, 0x10);
	GC0312_write_cmos_sensor(0x37, 0x20);
	GC0312_write_cmos_sensor(0x38, 0x10);
	GC0312_write_cmos_sensor(0x47, 0x80);
	GC0312_write_cmos_sensor(0x4e, 0x66);
	GC0312_write_cmos_sensor(0xa8, 0x02);
	GC0312_write_cmos_sensor(0xa9, 0x80);

	/* /////////////////////////////////////////////// */
	/* ////////////////      ISP reg   /////////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0x40, 0xff);
	GC0312_write_cmos_sensor(0x41, 0x21);
	GC0312_write_cmos_sensor(0x42, 0xcf);
	GC0312_write_cmos_sensor(0x44, 0x02);
	GC0312_write_cmos_sensor(0x45, 0xa8);
	GC0312_write_cmos_sensor(0x46, 0x02);	/* sync */
	GC0312_write_cmos_sensor(0x4a, 0x11);
	GC0312_write_cmos_sensor(0x4b, 0x01);
	GC0312_write_cmos_sensor(0x4c, 0x20);
	GC0312_write_cmos_sensor(0x4d, 0x05);
	GC0312_write_cmos_sensor(0x4f, 0x01);
	GC0312_write_cmos_sensor(0x50, 0x01);
	GC0312_write_cmos_sensor(0x55, 0x01);
	GC0312_write_cmos_sensor(0x56, 0xe0);
	GC0312_write_cmos_sensor(0x57, 0x02);
	GC0312_write_cmos_sensor(0x58, 0x80);

	/* /////////////////////////////////////////////// */
	/* /////////////////   GAIN   //////////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0x70, 0x70);
	GC0312_write_cmos_sensor(0x5a, 0x84);
	GC0312_write_cmos_sensor(0x5b, 0xc9);
	GC0312_write_cmos_sensor(0x5c, 0xed);
	GC0312_write_cmos_sensor(0x77, 0x74);
	GC0312_write_cmos_sensor(0x78, 0x40);
	GC0312_write_cmos_sensor(0x79, 0x5f);

	/* /////////////////////////////////////////////// */
	/* /////////////////   DNDD  ///////////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0x82, 0x14);
	GC0312_write_cmos_sensor(0x83, 0x0b);
	GC0312_write_cmos_sensor(0x89, 0xf0);

	/* /////////////////////////////////////////////// */
	/* ////////////////   EEINTP  //////////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0x8f, 0xaa);
	GC0312_write_cmos_sensor(0x90, 0x8c);
	GC0312_write_cmos_sensor(0x91, 0x90);
	GC0312_write_cmos_sensor(0x92, 0x03);
	GC0312_write_cmos_sensor(0x93, 0x03);
	GC0312_write_cmos_sensor(0x94, 0x05);
	GC0312_write_cmos_sensor(0x95, 0x65);
	GC0312_write_cmos_sensor(0x96, 0xf0);

	/* /////////////////////////////////////////////// */
	/* ///////////////////  ASDE  //////////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0xfe, 0x00);

	GC0312_write_cmos_sensor(0x9a, 0x20);
	GC0312_write_cmos_sensor(0x9b, 0x80);
	GC0312_write_cmos_sensor(0x9c, 0x40);
	GC0312_write_cmos_sensor(0x9d, 0x80);

	GC0312_write_cmos_sensor(0xa1, 0x30);
	GC0312_write_cmos_sensor(0xa2, 0x32);
	GC0312_write_cmos_sensor(0xa4, 0x30);
	GC0312_write_cmos_sensor(0xa5, 0x30);
	GC0312_write_cmos_sensor(0xaa, 0x10);
	GC0312_write_cmos_sensor(0xac, 0x22);

	/* /////////////////////////////////////////////// */
	/* /////////////////   GAMMA   /////////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0xfe, 0x00);	/* default */
	GC0312_write_cmos_sensor(0xbf, 0x08);
	GC0312_write_cmos_sensor(0xc0, 0x16);
	GC0312_write_cmos_sensor(0xc1, 0x28);
	GC0312_write_cmos_sensor(0xc2, 0x41);
	GC0312_write_cmos_sensor(0xc3, 0x5a);
	GC0312_write_cmos_sensor(0xc4, 0x6c);
	GC0312_write_cmos_sensor(0xc5, 0x7a);
	GC0312_write_cmos_sensor(0xc6, 0x96);
	GC0312_write_cmos_sensor(0xc7, 0xac);
	GC0312_write_cmos_sensor(0xc8, 0xbc);
	GC0312_write_cmos_sensor(0xc9, 0xc9);
	GC0312_write_cmos_sensor(0xca, 0xd3);
	GC0312_write_cmos_sensor(0xcb, 0xdd);
	GC0312_write_cmos_sensor(0xcc, 0xe5);
	GC0312_write_cmos_sensor(0xcd, 0xf1);
	GC0312_write_cmos_sensor(0xce, 0xfa);
	GC0312_write_cmos_sensor(0xcf, 0xff);

	/* /////////////////////////////////////////////// */
	/* /////////////////   YCP  ////////////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0xd0, 0x40);
	GC0312_write_cmos_sensor(0xd1, 0x34);
	GC0312_write_cmos_sensor(0xd2, 0x34);
	GC0312_write_cmos_sensor(0xd3, 0x40);
	GC0312_write_cmos_sensor(0xd6, 0xf2);
	GC0312_write_cmos_sensor(0xd7, 0x1b);
	GC0312_write_cmos_sensor(0xd8, 0x18);
	GC0312_write_cmos_sensor(0xdd, 0x03);

	/* /////////////////////////////////////////////// */
	/* //////////////////   AEC   //////////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0xfe, 0x01);
	GC0312_write_cmos_sensor(0x05, 0x30);
	GC0312_write_cmos_sensor(0x06, 0x75);
	GC0312_write_cmos_sensor(0x07, 0x40);
	GC0312_write_cmos_sensor(0x08, 0xb0);
	GC0312_write_cmos_sensor(0x0a, 0xc5);
	GC0312_write_cmos_sensor(0x0b, 0x11);
	GC0312_write_cmos_sensor(0x0c, 0x00);
	GC0312_write_cmos_sensor(0x12, 0x52);
	GC0312_write_cmos_sensor(0x13, 0x38);
	GC0312_write_cmos_sensor(0x18, 0x95);
	GC0312_write_cmos_sensor(0x19, 0x96);
	GC0312_write_cmos_sensor(0x1f, 0x20);
	GC0312_write_cmos_sensor(0x20, 0xc0);	/* 80 */
	GC0312_write_cmos_sensor(0x3e, 0x40);
	GC0312_write_cmos_sensor(0x3f, 0x57);
	GC0312_write_cmos_sensor(0x40, 0x7d);
	GC0312_write_cmos_sensor(0x03, 0x60);
	GC0312_write_cmos_sensor(0x44, 0x02);

	/* /////////////////////////////////////////////// */
	/* //////////////////   AWB   //////////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0x1c, 0x91);
	GC0312_write_cmos_sensor(0x21, 0x15);
	GC0312_write_cmos_sensor(0x50, 0x80);
	GC0312_write_cmos_sensor(0x56, 0x04);
	GC0312_write_cmos_sensor(0x59, 0x08);
	GC0312_write_cmos_sensor(0x5b, 0x02);
	GC0312_write_cmos_sensor(0x61, 0x8d);
	GC0312_write_cmos_sensor(0x62, 0xa7);
	GC0312_write_cmos_sensor(0x63, 0x00);	/* d0 cyrille */
	GC0312_write_cmos_sensor(0x65, 0x06);
	GC0312_write_cmos_sensor(0x66, 0x06);
	GC0312_write_cmos_sensor(0x67, 0x84);
	GC0312_write_cmos_sensor(0x69, 0x08);
	GC0312_write_cmos_sensor(0x6a, 0x25);	/* 50 */
	GC0312_write_cmos_sensor(0x6b, 0x01);
	GC0312_write_cmos_sensor(0x6c, 0x00);
	GC0312_write_cmos_sensor(0x6d, 0x02);
	GC0312_write_cmos_sensor(0x6e, 0xf0);
	GC0312_write_cmos_sensor(0x6f, 0x80);
	GC0312_write_cmos_sensor(0x76, 0x80);
	GC0312_write_cmos_sensor(0x78, 0xaf);
	GC0312_write_cmos_sensor(0x79, 0x75);
	GC0312_write_cmos_sensor(0x7a, 0x40);
	GC0312_write_cmos_sensor(0x7b, 0x50);
	GC0312_write_cmos_sensor(0x7c, 0x0c);

	GC0312_write_cmos_sensor(0xa4, 0xb9);
	GC0312_write_cmos_sensor(0xa5, 0xa0);
	GC0312_write_cmos_sensor(0x90, 0xc9);
	GC0312_write_cmos_sensor(0x91, 0xbe);

	GC0312_write_cmos_sensor(0xa6, 0xb8);
	GC0312_write_cmos_sensor(0xa7, 0x95);
	GC0312_write_cmos_sensor(0x92, 0xe6);
	GC0312_write_cmos_sensor(0x93, 0xca);

	GC0312_write_cmos_sensor(0xa9, 0xbc);
	GC0312_write_cmos_sensor(0xaa, 0x95);
	GC0312_write_cmos_sensor(0x95, 0x23);
	GC0312_write_cmos_sensor(0x96, 0xe7);

	GC0312_write_cmos_sensor(0xab, 0x9d);
	GC0312_write_cmos_sensor(0xac, 0x80);
	GC0312_write_cmos_sensor(0x97, 0x43);
	GC0312_write_cmos_sensor(0x98, 0x24);

	GC0312_write_cmos_sensor(0xae, 0xb7);
	GC0312_write_cmos_sensor(0xaf, 0x9e);
	GC0312_write_cmos_sensor(0x9a, 0x43);
	GC0312_write_cmos_sensor(0x9b, 0x24);

	GC0312_write_cmos_sensor(0xb0, 0xc8);
	GC0312_write_cmos_sensor(0xb1, 0x97);
	GC0312_write_cmos_sensor(0x9c, 0xc4);
	GC0312_write_cmos_sensor(0x9d, 0x44);

	GC0312_write_cmos_sensor(0xb3, 0xb7);
	GC0312_write_cmos_sensor(0xb4, 0x7f);
	GC0312_write_cmos_sensor(0x9f, 0xc7);
	GC0312_write_cmos_sensor(0xa0, 0xc8);

	GC0312_write_cmos_sensor(0xb5, 0x00);
	GC0312_write_cmos_sensor(0xb6, 0x00);
	GC0312_write_cmos_sensor(0xa1, 0x00);
	GC0312_write_cmos_sensor(0xa2, 0x00);

	GC0312_write_cmos_sensor(0x86, 0x60);
	GC0312_write_cmos_sensor(0x87, 0x08);
	GC0312_write_cmos_sensor(0x88, 0x00);
	GC0312_write_cmos_sensor(0x89, 0x00);
	GC0312_write_cmos_sensor(0x8b, 0xde);
	GC0312_write_cmos_sensor(0x8c, 0x80);
	GC0312_write_cmos_sensor(0x8d, 0x00);
	GC0312_write_cmos_sensor(0x8e, 0x00);

	GC0312_write_cmos_sensor(0x94, 0x55);
	GC0312_write_cmos_sensor(0x99, 0xa6);
	GC0312_write_cmos_sensor(0x9e, 0xaa);
	GC0312_write_cmos_sensor(0xa3, 0x0a);
	GC0312_write_cmos_sensor(0x8a, 0x0a);
	GC0312_write_cmos_sensor(0xa8, 0x55);
	GC0312_write_cmos_sensor(0xad, 0x55);
	GC0312_write_cmos_sensor(0xb2, 0x55);
	GC0312_write_cmos_sensor(0xb7, 0x05);
	GC0312_write_cmos_sensor(0x8f, 0x05);

	GC0312_write_cmos_sensor(0xb8, 0xcc);
	GC0312_write_cmos_sensor(0xb9, 0x9a);
	/* /////////////////////////////////////////////// */
	/* //////////////////   CC    //////////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0xfe, 0x01);

	GC0312_write_cmos_sensor(0xd0, 0x38);	/* skin white */
	GC0312_write_cmos_sensor(0xd1, 0xfd);
	GC0312_write_cmos_sensor(0xd2, 0x00);
	GC0312_write_cmos_sensor(0xd3, 0xf0);
	GC0312_write_cmos_sensor(0xd4, 0x40);
	GC0312_write_cmos_sensor(0xd5, 0x18);


/*
 *	GC0312_write_cmos_sensor(0xd0, 0x38);//guodengxiang
 *	GC0312_write_cmos_sensor(0xd1, 0xf8);
 *	GC0312_write_cmos_sensor(0xd2, 0x06);
 *	GC0312_write_cmos_sensor(0xd3, 0xfd);
 *	GC0312_write_cmos_sensor(0xd4, 0x40);
 *	GC0312_write_cmos_sensor(0xd5, 0x00);
 */

	GC0312_write_cmos_sensor(0xd6, 0x30);
	GC0312_write_cmos_sensor(0xd7, 0x00);
	GC0312_write_cmos_sensor(0xd8, 0x0a);
	GC0312_write_cmos_sensor(0xd9, 0x16);
	GC0312_write_cmos_sensor(0xda, 0x39);
	GC0312_write_cmos_sensor(0xdb, 0xf8);

	/* /////////////////////////////////////////////// */
	/* //////////////////   LSC   //////////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0xfe, 0x01);
	GC0312_write_cmos_sensor(0xc1, 0x3c);
	GC0312_write_cmos_sensor(0xc2, 0x50);
	GC0312_write_cmos_sensor(0xc3, 0x00);
	GC0312_write_cmos_sensor(0xc4, 0x40);
	GC0312_write_cmos_sensor(0xc5, 0x30);
	GC0312_write_cmos_sensor(0xc6, 0x30);
	GC0312_write_cmos_sensor(0xc7, 0x10);
	GC0312_write_cmos_sensor(0xc8, 0x00);
	GC0312_write_cmos_sensor(0xc9, 0x00);
	GC0312_write_cmos_sensor(0xdc, 0x20);
	GC0312_write_cmos_sensor(0xdd, 0x10);
	GC0312_write_cmos_sensor(0xdf, 0x00);
	GC0312_write_cmos_sensor(0xde, 0x00);

	/* /////////////////////////////////////////////// */
	/* /////////////////  Histogram  ///////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0x01, 0x10);
	GC0312_write_cmos_sensor(0x0b, 0x31);
	GC0312_write_cmos_sensor(0x0e, 0x50);
	GC0312_write_cmos_sensor(0x0f, 0x0f);
	GC0312_write_cmos_sensor(0x10, 0x6e);
	GC0312_write_cmos_sensor(0x12, 0xa0);
	GC0312_write_cmos_sensor(0x15, 0x60);
	GC0312_write_cmos_sensor(0x16, 0x60);
	GC0312_write_cmos_sensor(0x17, 0xe0);

	/* /////////////////////////////////////////////// */
	/* ////////////  Measure Window    /////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0xcc, 0x0c);
	GC0312_write_cmos_sensor(0xcd, 0x10);
	GC0312_write_cmos_sensor(0xce, 0xa0);
	GC0312_write_cmos_sensor(0xcf, 0xe6);

	/* /////////////////////////////////////////////// */
	/* ///////////////       dark sun   ////////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0x45, 0xf7);
	GC0312_write_cmos_sensor(0x46, 0xff);
	GC0312_write_cmos_sensor(0x47, 0x15);
	GC0312_write_cmos_sensor(0x48, 0x03);
	GC0312_write_cmos_sensor(0x4f, 0x60);

	/* ////////////////banding////////////////////// */
	GC0312_write_cmos_sensor(0xfe, 0x01);
	GC0312_write_cmos_sensor(0x0d, 0xf8); /*new exp 3*/
	GC0312_write_cmos_sensor(0xfe, 0x00);
	GC0312_write_cmos_sensor(0x05, 0x01);/*01*/
	GC0312_write_cmos_sensor(0x06, 0x18);/*89*/
	GC0312_write_cmos_sensor(0x07, 0x00);
	GC0312_write_cmos_sensor(0x08, 0x10);/*22*/
	GC0312_write_cmos_sensor(0xfe, 0x01);
	GC0312_write_cmos_sensor(0x25, 0x00);
	GC0312_write_cmos_sensor(0x26, 0x9a);   /*1step   30*/
	GC0312_write_cmos_sensor(0x27, 0x01);
	GC0312_write_cmos_sensor(0x28, 0x34);   /*2step   30*/
	GC0312_write_cmos_sensor(0x29, 0x02);
	GC0312_write_cmos_sensor(0x2a, 0x68);   /*4step   25*/
	GC0312_write_cmos_sensor(0x2b, 0x04);
	GC0312_write_cmos_sensor(0x2c, 0xd0);   /*5step   12.5*/
	GC0312_write_cmos_sensor(0x2d, 0x06);
	GC0312_write_cmos_sensor(0x2e, 0x04);   /*10step  10*/
	GC0312_write_cmos_sensor(0x2f, 0x09);
	GC0312_write_cmos_sensor(0x30, 0x06);   /*16step  6.25*/ /*a0*/
	GC0312_write_cmos_sensor(0x31, 0x09);                    /*0a*/
	GC0312_write_cmos_sensor(0x32, 0xa0);   /*18step  5.56*/ /*d4*/
	GC0312_write_cmos_sensor(0x33, 0x0a);                    /*0c*/
	GC0312_write_cmos_sensor(0x34, 0x3a);   /*20step  5*/    /*08*/
	GC0312_write_cmos_sensor(0x35, 0x10);   /*2*/            /*08*/
	GC0312_write_cmos_sensor(0x36, 0x10);   /*2*/            /*12*/
	GC0312_write_cmos_sensor(0x37, 0x12);   /*2.25*/
	GC0312_write_cmos_sensor(0x38, 0x14);   /*2.5*/
	GC0312_write_cmos_sensor(0x39, 0x18);   /*3*/
	GC0312_write_cmos_sensor(0x3a, 0x28);   /*5*/
	GC0312_write_cmos_sensor(0x3b, 0x28);   /*5*/

	/* update fps register */
	GC0312_write_cmos_sensor(0x28, 0x34);
	GC0312_write_cmos_sensor(0x29, 0x01);
	GC0312_write_cmos_sensor(0x2a, 0x34);
	GC0312_write_cmos_sensor(0x2b, 0x01);
	GC0312_write_cmos_sensor(0x2c, 0x34);
	GC0312_write_cmos_sensor(0x2d, 0x01);
	GC0312_write_cmos_sensor(0x2e, 0x34);
	GC0312_write_cmos_sensor(0x2f, 0x01);
	GC0312_write_cmos_sensor(0x30, 0x34);
	GC0312_write_cmos_sensor(0x31, 0x01);
	GC0312_write_cmos_sensor(0x32, 0x34);
	GC0312_write_cmos_sensor(0x33, 0x01);
	/* /////////////////////////////////////////////// */
	/* ///////////////////  DVP   //////////////////// */
	/* /////////////////////////////////////////////// */
	GC0312_write_cmos_sensor(0xfe, 0x03);
	GC0312_write_cmos_sensor(0x01, 0x00);
	GC0312_write_cmos_sensor(0x02, 0x00);
	GC0312_write_cmos_sensor(0x10, 0x00);
	GC0312_write_cmos_sensor(0x15, 0x00);
	GC0312_write_cmos_sensor(0xfe, 0x00);
	/* /////////////////OUTPUT////////////////////// */
	GC0312_write_cmos_sensor(0xf3, 0xff);	/* output_enable */

}



UINT32 GC0312GetSensorID(UINT32 *sensorID)
{
	int retry = 3;
	/* check if sensor ID correct */
	do {
		*sensorID =
			((GC0312_read_cmos_sensor(0xf0) << 8)
			| GC0312_read_cmos_sensor(0xf1));
		if (*sensorID == GC0312_SENSOR_ID)
			break;
		SENSORDB("Read Sensor ID Fail = 0x%04x\n", *sensorID);
		retry--;
	} while (retry > 0);

	if (*sensorID != GC0312_SENSOR_ID) {
		*sensorID = 0xFFFFFFFF;
		return ERROR_SENSOR_CONNECT_FAIL;
	}
	return ERROR_NONE;
}




/*
 ************************************************************************
 * FUNCTION
 *	GC0312_Write_More_Registers
 *
 * DESCRIPTION
 *	This function is served for FAE to modify the necessary Init Regs.
 *  Do not modify the regs in init_GC0312() directly.
 *
 * PARAMETERS
 *	None
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 ************************************************************************
 */
void GC0312_Write_More_Registers(void)
{
	/* //for FAE to modify the necessary Init Regs.*/

}


/*************************************************************************
 * FUNCTION
 *	GC0312Open
 *
 * DESCRIPTION
 *	This function initialize the registers of CMOS sensor
 *
 * PARAMETERS
 *	None
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
UINT32 GC0312Open(void)
{
	signed char i;
	kal_uint16 sensor_id = 0;

	Sleep(10);


	/* Read sensor ID to adjust I2C is OK? */
	for (i = 0; i < 3; i++) {
		sensor_id =
			((GC0312_read_cmos_sensor(0xf0) << 8)
			| GC0312_read_cmos_sensor(0xf1));
		if (sensor_id != GC0312_SENSOR_ID) {
			SENSORDB("GC0312 Read Sensor ID Fail[open] = 0x%x\n",
				sensor_id);
			return ERROR_SENSOR_CONNECT_FAIL;
		}
	}

	SENSORDB("GC0312_ Sensor Read ID OK \r\n");
	GC0312_Sensor_Init();
	GC0312_Write_More_Registers();

	return ERROR_NONE;
}				/* GC0312Open */


/*************************************************************************
 * FUNCTION
 *	GC0312Close
 *
 * DESCRIPTION
 *	This function is to turn off sensor module power.
 *
 * PARAMETERS
 *	None
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
UINT32 GC0312Close(void)
{
	return ERROR_NONE;
}				/* GC0312Close */


/*************************************************************************
 * FUNCTION
 * GC0312Preview
 *
 * DESCRIPTION
 *	This function start the sensor preview.
 *
 * PARAMETERS
 *	*image_window : address pointer of pixel numbers in one period of HSYNC
 *  *sensor_config_data : address pointer of line numbers in one period of VSYNC
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
UINT32 GC0312Preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT
	*image_window, MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	/* MPEG4 Encode Mode */
	if (sensor_config_data->SensorOperationMode
		== MSDK_SENSOR_OPERATION_MODE_VIDEO) {
		RETAILMSG(1, (TEXT("Camera Video preview\r\n")));
		GC0312_MPEG4_encode_mode = KAL_TRUE;

	} else {
		RETAILMSG(1, (TEXT("Camera preview\r\n")));
		GC0312_MPEG4_encode_mode = KAL_FALSE;
	}

	image_window->GrabStartX = IMAGE_SENSOR_VGA_GRAB_PIXELS;
	image_window->GrabStartY = IMAGE_SENSOR_VGA_GRAB_LINES;
	image_window->ExposureWindowWidth = IMAGE_SENSOR_PV_WIDTH;
	image_window->ExposureWindowHeight = IMAGE_SENSOR_PV_HEIGHT;

	/* copy sensor_config_data */
	memcpy(&GC0312SensorConfigData, sensor_config_data,
		sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	return ERROR_NONE;
}				/* GC0312Preview */


/*************************************************************************
 * FUNCTION
 *	GC0312Capture
 *
 * DESCRIPTION
 *	This function setup the CMOS sensor in capture MY_OUTPUT mode
 *
 * PARAMETERS
 *
 * RETURNS
 *	None
 *
 * GLOBALS AFFECTED
 *
 *************************************************************************/
UINT32 GC0312Capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
		     MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	GC0312_MODE_CAPTURE = KAL_TRUE;

	image_window->GrabStartX = IMAGE_SENSOR_VGA_GRAB_PIXELS;
	image_window->GrabStartY = IMAGE_SENSOR_VGA_GRAB_LINES;
	image_window->ExposureWindowWidth = IMAGE_SENSOR_FULL_WIDTH;
	image_window->ExposureWindowHeight = IMAGE_SENSOR_FULL_HEIGHT;

	/* copy sensor_config_data */
	memcpy(&GC0312SensorConfigData, sensor_config_data,
		sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	return ERROR_NONE;
}				/* GC0312_Capture() */

UINT32 GC0312GetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT
	*pSensorResolution)
{
	spin_lock(&imgsensor_drv_lock);
	pSensorResolution->SensorFullWidth = IMAGE_SENSOR_FULL_WIDTH;
	pSensorResolution->SensorFullHeight = IMAGE_SENSOR_FULL_HEIGHT;
	pSensorResolution->SensorPreviewWidth = IMAGE_SENSOR_PV_WIDTH;
	pSensorResolution->SensorPreviewHeight = IMAGE_SENSOR_PV_HEIGHT;
	pSensorResolution->SensorVideoWidth = IMAGE_SENSOR_PV_WIDTH;
	pSensorResolution->SensorVideoHeight = IMAGE_SENSOR_PV_HEIGHT;
	spin_unlock(&imgsensor_drv_lock);
	return ERROR_NONE;
}				/* GC0312GetResolution() */


UINT32 GC0312GetInfo(enum MSDK_SCENARIO_ID_ENUM ScenarioId,
		     MSDK_SENSOR_INFO_STRUCT *pSensorInfo,
		     MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
	spin_lock(&imgsensor_drv_lock);
	pSensorInfo->SensorPreviewResolutionX = IMAGE_SENSOR_PV_WIDTH;
	pSensorInfo->SensorPreviewResolutionY = IMAGE_SENSOR_PV_HEIGHT;
	pSensorInfo->SensorFullResolutionX = IMAGE_SENSOR_FULL_WIDTH;
	pSensorInfo->SensorFullResolutionY = IMAGE_SENSOR_FULL_HEIGHT;

	pSensorInfo->SensorCameraPreviewFrameRate = 30;
	pSensorInfo->SensorVideoFrameRate = 30;
	pSensorInfo->SensorStillCaptureFrameRate = 10;
	pSensorInfo->SensorWebCamCaptureFrameRate = 15;
	pSensorInfo->SensorResetActiveHigh = FALSE;
	pSensorInfo->SensorResetDelayCount = 1;
	pSensorInfo->SensorOutputDataFormat = SENSOR_OUTPUT_FORMAT_YUYV;
	pSensorInfo->SensorClockPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	pSensorInfo->SensorInterruptDelayLines = 1;
	pSensorInfo->SensroInterfaceType = SENSOR_INTERFACE_TYPE_PARALLEL;
	pSensorInfo->CaptureDelayFrame = 2;
	pSensorInfo->PreviewDelayFrame = 2;
	pSensorInfo->VideoDelayFrame = 4;
	pSensorInfo->SensorMasterClockSwitch = 0;
	pSensorInfo->SensorDrivingCurrent = ISP_DRIVING_2MA;

	switch (ScenarioId) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
	default:
		pSensorInfo->SensorClockFreq = 24;
		pSensorInfo->SensorClockDividCount = 3;
		pSensorInfo->SensorClockRisingCount = 0;
		pSensorInfo->SensorClockFallingCount = 2;
		pSensorInfo->SensorPixelClockCount = 3;
		pSensorInfo->SensorDataLatchCount = 2;
		pSensorInfo->SensorGrabStartX = IMAGE_SENSOR_VGA_GRAB_PIXELS;
		pSensorInfo->SensorGrabStartY = IMAGE_SENSOR_VGA_GRAB_LINES;
		break;
	}
	GC0312PixelClockDivider = pSensorInfo->SensorPixelClockCount;
	memcpy(pSensorConfigData, &GC0312SensorConfigData,
		sizeof(MSDK_SENSOR_CONFIG_STRUCT));
	spin_unlock(&imgsensor_drv_lock);

	return ERROR_NONE;
}				/* GC0312GetInfo() */


UINT32 GC0312Control(enum MSDK_SCENARIO_ID_ENUM ScenarioId,
		     MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow,
		     MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData)
{
	switch (ScenarioId) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
	default:
		GC0312Preview(pImageWindow, pSensorConfigData);
		break;
	}


	return TRUE;
}				/* GC0312Control() */

BOOL GC0312_set_Edge(UINT16 para)
{

	switch (para) {
	case ISP_EDGE_MIDDLE:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0x95, 0x44);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	case ISP_EDGE_HIGH:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0x95, 0x99);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	case ISP_EDGE_LOW:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0x95, 0x00);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

BOOL GC0312_set_HUE(UINT16 para)
{

	switch (para) {
	case ISP_HUE_MIDDLE:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0x7a, 0x80);
		GC0312_write_cmos_sensor(0x7b, 0x80);
		GC0312_write_cmos_sensor(0x7c, 0x80);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	case ISP_HUE_HIGH:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0x7a, 0x88);
		GC0312_write_cmos_sensor(0x7b, 0x88);
		GC0312_write_cmos_sensor(0x7c, 0x80);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	case ISP_HUE_LOW:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0x7a, 0x80);
		GC0312_write_cmos_sensor(0x7b, 0x80);
		GC0312_write_cmos_sensor(0x7c, 0x88);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;


	default:
		return FALSE;
	}

	return TRUE;
}

BOOL GC0312_set_Brightness(UINT16 para)
{

	switch (para) {
	case ISP_BRIGHT_MIDDLE:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0xd5, 0x00);
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x13, 0x37);  /*39  20160406*/
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	case ISP_BRIGHT_HIGH:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0xd5, 0x10);
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x13, 0x60);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	case ISP_BRIGHT_LOW:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0xd5, 0xf8);
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x13, 0x30);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

BOOL GC0312_set_Contrast(UINT16 para)
{

	switch (para) {
	case ISP_CONTRAST_MIDDLE:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0xd3, 0x40);  /*42  20160406*/
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	case ISP_CONTRAST_HIGH:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0xd3, 0x50);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	case ISP_CONTRAST_LOW:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0xd3, 0x30);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

BOOL GC0312_set_Saturation(UINT16 para)
{

	switch (para) {
	case ISP_SAT_MIDDLE:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0xd1, 0x2c);  /*30  20160406*/
		GC0312_write_cmos_sensor(0xd2, 0x28);  /*2a  20160406*/
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	case ISP_SAT_HIGH:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0xd1, 0x40);
		GC0312_write_cmos_sensor(0xd2, 0x40);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	case ISP_SAT_LOW:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0xd1, 0x20);
		GC0312_write_cmos_sensor(0xd2, 0x20);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

BOOL GC0312_set_param_wb(UINT16 para)
{

	switch (para) {
	case AWB_MODE_OFF:

		break;

	case AWB_MODE_AUTO:
		GC0312_awb_enable(KAL_TRUE);
		break;

	case AWB_MODE_CLOUDY_DAYLIGHT:	/* cloudy */
		GC0312_awb_enable(KAL_FALSE);
		GC0312_write_cmos_sensor(0x77, 0x8c);	/* WB_manual_gain */
		GC0312_write_cmos_sensor(0x78, 0x50);
		GC0312_write_cmos_sensor(0x79, 0x40);
		break;

	case AWB_MODE_DAYLIGHT:	/* sunny */
		GC0312_awb_enable(KAL_FALSE);
		GC0312_write_cmos_sensor(0x77, 0x74);
		GC0312_write_cmos_sensor(0x78, 0x52);
		GC0312_write_cmos_sensor(0x79, 0x40);
		break;

	case AWB_MODE_INCANDESCENT:	/* office */
		GC0312_awb_enable(KAL_FALSE);
		GC0312_write_cmos_sensor(0x77, 0x48);
		GC0312_write_cmos_sensor(0x78, 0x40);
		GC0312_write_cmos_sensor(0x79, 0x5c);
		break;

	case AWB_MODE_TUNGSTEN:	/* home */
		GC0312_awb_enable(KAL_FALSE);
		GC0312_write_cmos_sensor(0x77, 0x40);
		GC0312_write_cmos_sensor(0x78, 0x54);
		GC0312_write_cmos_sensor(0x79, 0x70);
		break;

	case AWB_MODE_FLUORESCENT:
		GC0312_awb_enable(KAL_FALSE);
		GC0312_write_cmos_sensor(0x77, 0x40);
		GC0312_write_cmos_sensor(0x78, 0x42);
		GC0312_write_cmos_sensor(0x79, 0x50);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}				/* GC0312_set_param_wb */


BOOL GC0312_set_param_effect(UINT16 para)
{
	kal_uint32 ret = KAL_TRUE;

	switch (para) {
	case MEFFECT_OFF:
		GC0312_write_cmos_sensor(0x43, 0x00);
		break;

	case MEFFECT_SEPIA:
		GC0312_write_cmos_sensor(0x43, 0x02);
		GC0312_write_cmos_sensor(0xda, 0xd0);
		GC0312_write_cmos_sensor(0xdb, 0x28);
		break;

	case MEFFECT_NEGATIVE:
		GC0312_write_cmos_sensor(0x43, 0x01);
		break;

	case MEFFECT_SEPIAGREEN:
		GC0312_write_cmos_sensor(0x43, 0x02);
		GC0312_write_cmos_sensor(0xda, 0xc0);
		GC0312_write_cmos_sensor(0xdb, 0xc0);
		break;

	case MEFFECT_SEPIABLUE:
		GC0312_write_cmos_sensor(0x43, 0x02);
		GC0312_write_cmos_sensor(0xda, 0x50);
		GC0312_write_cmos_sensor(0xdb, 0xe0);
		break;

	case MEFFECT_MONO:
		GC0312_write_cmos_sensor(0x43, 0x02);
		GC0312_write_cmos_sensor(0xda, 0x00);
		GC0312_write_cmos_sensor(0xdb, 0x00);
		break;
	default:
		ret = FALSE;
	}

	return ret;

}				/* GC0312_set_param_effect */


BOOL GC0312_set_param_banding(UINT16 para)
{
	switch (para) {
	case AE_FLICKER_MODE_50HZ:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0x05, 0x02);
		GC0312_write_cmos_sensor(0x06, 0xd1);	/* HB */
		GC0312_write_cmos_sensor(0x07, 0x00);
		GC0312_write_cmos_sensor(0x08, 0x22);	/* VB */

		GC0312_write_cmos_sensor(0xfe, 0x01);
		/* anti-flicker step [11:8] */
		GC0312_write_cmos_sensor(0x25, 0x00);
		/* anti-flicker step [7:0] */
		GC0312_write_cmos_sensor(0x26, 0x6a);

		/* exp level 0  20fps */
		GC0312_write_cmos_sensor(0x27, 0x02);
		GC0312_write_cmos_sensor(0x28, 0x12);
		/* exp level 1  12.50fps */
		GC0312_write_cmos_sensor(0x29, 0x03);
		GC0312_write_cmos_sensor(0x2a, 0x50);
		/* 7.14fps */
		GC0312_write_cmos_sensor(0x2b, 0x05);
		GC0312_write_cmos_sensor(0x2c, 0xcc);
		/* exp level 3  5.55fps */
		GC0312_write_cmos_sensor(0x2d, 0x07);
		GC0312_write_cmos_sensor(0x2e, 0x74);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	case AE_FLICKER_MODE_60HZ:
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0x05, 0x02);
		GC0312_write_cmos_sensor(0x06, 0x60);
		GC0312_write_cmos_sensor(0x07, 0x00);
		GC0312_write_cmos_sensor(0x08, 0x58);

		GC0312_write_cmos_sensor(0xfe, 0x01);
		/* anti-flicker step [11:8] */
		GC0312_write_cmos_sensor(0x25, 0x00);
		/* anti-flicker step [7:0] */
		GC0312_write_cmos_sensor(0x26, 0x60);

		/* exp level 0  20fps */
		GC0312_write_cmos_sensor(0x27, 0x02);
		GC0312_write_cmos_sensor(0x28, 0x40);
		/* exp level 1  13fps */
		GC0312_write_cmos_sensor(0x29, 0x03);
		GC0312_write_cmos_sensor(0x2a, 0x60);
		/* exp level 2  7.5fps */
		GC0312_write_cmos_sensor(0x2b, 0x06);
		GC0312_write_cmos_sensor(0x2c, 0x00);
		/* exp level 3  5.45fps */
		GC0312_write_cmos_sensor(0x2d, 0x08);
		GC0312_write_cmos_sensor(0x2e, 0x40);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}				/* GC0312_set_param_banding */


BOOL GC0312_set_param_exposure(UINT16 para)
{


	switch (para) {
	case AE_EV_COMP_n30:
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x13, 0x20);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;
	case AE_EV_COMP_n20:
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x13, 0x28);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	case AE_EV_COMP_n10:
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x13, 0x30);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;



	case AE_EV_COMP_00:
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x13, 0x38);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;



	case AE_EV_COMP_10:
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x13, 0x40);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;

	case AE_EV_COMP_20:
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x13, 0x48);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;
	case AE_EV_COMP_30:
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x13, 0x50);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}				/* GC0312_set_param_exposure */


UINT32 GC0312YUVSetVideoMode(UINT16 u2FrameRate)	/* lanking add */
{

	SENSORDB("Enter %s, u2FrameRate = %d\n", __func__, u2FrameRate);

	if (u2FrameRate == 30) {
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x0d, 0xf8);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0x05, 0x01);
		GC0312_write_cmos_sensor(0x06, 0x18);
		GC0312_write_cmos_sensor(0x07, 0x00);
		GC0312_write_cmos_sensor(0x08, 0x10);
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x25, 0x00);
		GC0312_write_cmos_sensor(0x26, 0x9a);
		GC0312_write_cmos_sensor(0x27, 0x01);
		GC0312_write_cmos_sensor(0x28, 0x34);   /*30fps*/
		GC0312_write_cmos_sensor(0x29, 0x01);
		GC0312_write_cmos_sensor(0x2a, 0x34);   /*30fps*/
		GC0312_write_cmos_sensor(0x2b, 0x01);
		GC0312_write_cmos_sensor(0x2c, 0x34);   /*30fps*/
		GC0312_write_cmos_sensor(0x2d, 0x01);
		GC0312_write_cmos_sensor(0x2e, 0x34);   /*30fos*/
		GC0312_write_cmos_sensor(0x2f, 0x01);
		GC0312_write_cmos_sensor(0x30, 0x34);   /*30fps*/
		GC0312_write_cmos_sensor(0x31, 0x01);
		GC0312_write_cmos_sensor(0x32, 0x34);   /*30fps*/
		GC0312_write_cmos_sensor(0x33, 0x01);
		GC0312_write_cmos_sensor(0x34, 0x34);
		GC0312_write_cmos_sensor(0x35, 0x0d);
		GC0312_write_cmos_sensor(0x36, 0x14);
		GC0312_write_cmos_sensor(0x37, 0x18);
		GC0312_write_cmos_sensor(0x38, 0x20);
		GC0312_write_cmos_sensor(0x39, 0x30);
		GC0312_write_cmos_sensor(0x3a, 0x30);
		GC0312_write_cmos_sensor(0x3b, 0x30);
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x13, 0x35);
		GC0312_write_cmos_sensor(0xfe, 0x00);
	} else if (u2FrameRate == 20) {
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x0d, 0xf8);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0x05, 0x02);
		GC0312_write_cmos_sensor(0x06, 0xd1);
		GC0312_write_cmos_sensor(0x07, 0x00);
		GC0312_write_cmos_sensor(0x08, 0x22);
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x25, 0x00);
		GC0312_write_cmos_sensor(0x26, 0x6a);
		GC0312_write_cmos_sensor(0x27, 0x01);
		GC0312_write_cmos_sensor(0x28, 0xa8);   /*20fps*/
		GC0312_write_cmos_sensor(0x29, 0x01);
		GC0312_write_cmos_sensor(0x2a, 0xa8);   /*20fps*/
		GC0312_write_cmos_sensor(0x2b, 0x01);
		GC0312_write_cmos_sensor(0x2c, 0xa8);   /*20fps*/
		GC0312_write_cmos_sensor(0x2d, 0x01);
		GC0312_write_cmos_sensor(0x2e, 0xa8);   /*20fps*/
		GC0312_write_cmos_sensor(0x2f, 0x01);
		GC0312_write_cmos_sensor(0x30, 0xa8);   /*20fps*/
		GC0312_write_cmos_sensor(0x31, 0x01);
		GC0312_write_cmos_sensor(0x32, 0xa8);
		GC0312_write_cmos_sensor(0x33, 0x01);
		GC0312_write_cmos_sensor(0x34, 0xa8);
		GC0312_write_cmos_sensor(0x35, 0x0d);
		GC0312_write_cmos_sensor(0x36, 0x14);
		GC0312_write_cmos_sensor(0x37, 0x18);
		GC0312_write_cmos_sensor(0x38, 0x20);
		GC0312_write_cmos_sensor(0x39, 0x30);
		GC0312_write_cmos_sensor(0x3a, 0x30);
		GC0312_write_cmos_sensor(0x3b, 0x30);
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x13, 0x35);
		GC0312_write_cmos_sensor(0xfe, 0x00);
	} else if (u2FrameRate == 15) {
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x0d, 0xf8);
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0x05, 0x04);
		GC0312_write_cmos_sensor(0x06, 0xe6);
		GC0312_write_cmos_sensor(0x07, 0x00);
		GC0312_write_cmos_sensor(0x08, 0x10);
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x25, 0x00);
		GC0312_write_cmos_sensor(0x26, 0x4d);
		GC0312_write_cmos_sensor(0x27, 0x01);
		GC0312_write_cmos_sensor(0x28, 0x34);   /*15fps*/
		GC0312_write_cmos_sensor(0x29, 0x01);
		GC0312_write_cmos_sensor(0x2a, 0x34);   /*15ps*/
		GC0312_write_cmos_sensor(0x2b, 0x01);
		GC0312_write_cmos_sensor(0x2c, 0x34);   /*15fps*/
		GC0312_write_cmos_sensor(0x2d, 0x01);
		GC0312_write_cmos_sensor(0x2e, 0x34);   /*15fps*/
		GC0312_write_cmos_sensor(0x2f, 0x01);
		GC0312_write_cmos_sensor(0x30, 0x34);   /*15fps*/
		GC0312_write_cmos_sensor(0x31, 0x01);
		GC0312_write_cmos_sensor(0x32, 0x34);   /*15fps*/
		GC0312_write_cmos_sensor(0x33, 0x01);
		GC0312_write_cmos_sensor(0x34, 0x34);
		GC0312_write_cmos_sensor(0x35, 0x0d);
		GC0312_write_cmos_sensor(0x36, 0x14);
		GC0312_write_cmos_sensor(0x37, 0x18);
		GC0312_write_cmos_sensor(0x38, 0x20);
		GC0312_write_cmos_sensor(0x39, 0x30);
		GC0312_write_cmos_sensor(0x3a, 0x30);
		GC0312_write_cmos_sensor(0x3b, 0x30);
		GC0312_write_cmos_sensor(0xfe, 0x01);
		GC0312_write_cmos_sensor(0x13, 0x35);
		GC0312_write_cmos_sensor(0xfe, 0x00);
	} else {
		SENSORDB("Wrong frame rate setting\n");
	}

	return TRUE;

}


UINT32 GC0312YUVSensorSetting(enum FEATURE_ID iCmd, UINT16 iPara)
{
	switch (iCmd) {
	case FID_ISP_EDGE:
		GC0312_set_Edge(iPara);
		break;
	case FID_ISP_HUE:
		GC0312_set_HUE(iPara);
		break;
	case FID_ISP_BRIGHT:
		GC0312_set_Brightness(iPara);
		break;
	case FID_ISP_CONTRAST:
		GC0312_set_Contrast(iPara);
		break;
	case FID_ISP_SAT:
		GC0312_set_Saturation(iPara);
		break;
	case FID_AWB_MODE:
		GC0312_set_param_wb(iPara);
		break;
	case FID_COLOR_EFFECT:
		GC0312_set_param_effect(iPara);
		break;
	case FID_AE_EV:
		GC0312_set_param_exposure(iPara);
		break;
	case FID_AE_FLICKER:
		GC0312_set_param_banding(iPara);
		break;
	case FID_SCENE_MODE:
		GC0312NightMode(iPara);
		break;
	default:
		break;
	}
	return TRUE;
}				/* GC0312YUVSensorSetting */

UINT32 GC0312_SetTestPatternMode(kal_bool bEnable)
{
	SENSORDB("bEnable: %d", bEnable);
	if (bEnable) {
		GC0312_write_cmos_sensor(0xfe, 0x00);
		GC0312_write_cmos_sensor(0xfa, 0x32);
		GC0312_write_cmos_sensor(0x40, 0x08);
		GC0312_write_cmos_sensor(0x41, 0x00);
		GC0312_write_cmos_sensor(0x42, 0x00);
		GC0312_write_cmos_sensor(0x4f, 0x00);
		GC0312_write_cmos_sensor(0x18, 0x06);
		GC0312_write_cmos_sensor(0x77, 0x40);
		GC0312_write_cmos_sensor(0x78, 0x40);
		GC0312_write_cmos_sensor(0x79, 0x40);
		GC0312_write_cmos_sensor(0xd0, 0x40);
		GC0312_write_cmos_sensor(0xdd, 0x00);
		GC0312_write_cmos_sensor(0x71, 0x40);
		GC0312_write_cmos_sensor(0x72, 0x40);
		GC0312_write_cmos_sensor(0x03, 0x00);
		GC0312_write_cmos_sensor(0x04, 0x00);
		GC0312_write_cmos_sensor(0x4c, 0x01);
	} else {
	}

	return ERROR_NONE;
}

#define GC0312_TEST_PATTERN_CHECKSUM (0xa8ccfa09)

UINT32 GC0312FeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId,
			    UINT8 *pFeaturePara, UINT32 *pFeatureParaLen)
{
	UINT16 *pFeatureReturnPara16 = (UINT16 *) pFeaturePara;
	UINT16 *pFeatureData16 = (UINT16 *) pFeaturePara;
	UINT32 *pFeatureReturnPara32 = (UINT32 *) pFeaturePara;
	UINT32 *pFeatureData32 = (UINT32 *) pFeaturePara;
	unsigned long long *pFeatureData = (unsigned long long *)pFeaturePara;
	MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData =
		(MSDK_SENSOR_CONFIG_STRUCT *) pFeaturePara;
	MSDK_SENSOR_REG_INFO_STRUCT *pSensorRegData =
		(MSDK_SENSOR_REG_INFO_STRUCT *) pFeaturePara;

	RETAILMSG(1, (_T("gaiyang %s FeatureId=%d\r\n"), __func__, FeatureId));

	switch (FeatureId) {
	case SENSOR_FEATURE_GET_RESOLUTION:
		*pFeatureReturnPara16++ = IMAGE_SENSOR_FULL_WIDTH;
		*pFeatureReturnPara16 = IMAGE_SENSOR_FULL_HEIGHT;
		*pFeatureParaLen = 4;
		break;
	case SENSOR_FEATURE_GET_PERIOD:
		*pFeatureReturnPara16++ =
			(VGA_PERIOD_PIXEL_NUMS) + GC0312_dummy_pixels;
		*pFeatureReturnPara16 =
			(VGA_PERIOD_LINE_NUMS) + GC0312_dummy_lines;
		*pFeatureParaLen = 4;
		break;
	case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
		*pFeatureReturnPara32 = GC0312_g_fPV_PCLK;
		*pFeatureParaLen = 4;
		break;
	case SENSOR_FEATURE_SET_ESHUTTER:
		break;
	case SENSOR_FEATURE_SET_NIGHTMODE:
		/* GC0312NightMode((BOOL) *pFeatureData); */
		break;
	case SENSOR_FEATURE_SET_GAIN:
	case SENSOR_FEATURE_SET_FLASHLIGHT:
		break;
	case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
		GC0312_isp_master_clock = *pFeatureData32;
		break;
	case SENSOR_FEATURE_SET_REGISTER:
		GC0312_write_cmos_sensor(pSensorRegData->RegAddr,
			pSensorRegData->RegData);
		break;
	case SENSOR_FEATURE_GET_REGISTER:
		pSensorRegData->RegData =
			GC0312_read_cmos_sensor(pSensorRegData->RegAddr);
		break;
	case SENSOR_FEATURE_GET_CONFIG_PARA:
		memcpy(pSensorConfigData, &GC0312SensorConfigData,
		       sizeof(MSDK_SENSOR_CONFIG_STRUCT));
		*pFeatureParaLen = sizeof(MSDK_SENSOR_CONFIG_STRUCT);
		break;
	case SENSOR_FEATURE_SET_CCT_REGISTER:
	case SENSOR_FEATURE_GET_CCT_REGISTER:
	case SENSOR_FEATURE_SET_ENG_REGISTER:
	case SENSOR_FEATURE_GET_ENG_REGISTER:
	case SENSOR_FEATURE_GET_REGISTER_DEFAULT:
	case SENSOR_FEATURE_CAMERA_PARA_TO_SENSOR:
	case SENSOR_FEATURE_SENSOR_TO_CAMERA_PARA:
	case SENSOR_FEATURE_GET_GROUP_COUNT:
	case SENSOR_FEATURE_GET_GROUP_INFO:
	case SENSOR_FEATURE_GET_ITEM_INFO:
	case SENSOR_FEATURE_SET_ITEM_INFO:
	case SENSOR_FEATURE_GET_ENG_INFO:
		break;
	case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
		/* get the lens driver ID from EEPROM or
		 * just return LENS_DRIVER_ID_DO_NOT_CARE
		 */
		/* if EEPROM does not exist in camera module. */
		*pFeatureReturnPara32 = LENS_DRIVER_ID_DO_NOT_CARE;
		*pFeatureParaLen = 4;
		break;
	case SENSOR_FEATURE_SET_YUV_CMD:
		GC0312YUVSensorSetting((enum FEATURE_ID) *pFeatureData,
			*(pFeatureData + 1));
		break;
	case SENSOR_FEATURE_SET_VIDEO_MODE:	/* lanking */
		GC0312YUVSetVideoMode(*pFeatureData16);
		break;
	case SENSOR_FEATURE_CHECK_SENSOR_ID:
		GC0312GetSensorID(pFeatureData32);
		break;
	case SENSOR_FEATURE_SET_TEST_PATTERN:
		GC0312_SetTestPatternMode((BOOL) *pFeatureData);
		break;
		/* for factory mode auto testing */
	case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE:
		*pFeatureReturnPara32 = GC0312_TEST_PATTERN_CHECKSUM;
		*pFeatureParaLen = 4;
		break;
	default:
		break;
	}
	return ERROR_NONE;
}				/* GC0312FeatureControl() */


struct SENSOR_FUNCTION_STRUCT SensorFuncGC0312YUV = {
	GC0312Open,
	GC0312GetInfo,
	GC0312GetResolution,
	GC0312FeatureControl,
	GC0312Control,
	GC0312Close
};


UINT32 GC0312_YUV_SensorInit(struct SENSOR_FUNCTION_STRUCT **pfFunc)
{
	/* To Do : Check Sensor status here */
	if (pfFunc != NULL)
		*pfFunc = &SensorFuncGC0312YUV;
	return ERROR_NONE;
}				/* SensorInit() */
