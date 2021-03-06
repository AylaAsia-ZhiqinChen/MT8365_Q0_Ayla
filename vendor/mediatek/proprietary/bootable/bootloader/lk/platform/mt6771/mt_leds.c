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

#include <platform/mt_reg_base.h>
#include <platform/mt_typedefs.h>

#include <platform/mt_pwm.h>
#include <platform/mt_gpio.h>
#include <platform/mt_leds.h>
#include <platform/mt_i2c.h>

#include <platform/mt_pmic.h>
#include <platform/upmu_common.h>
#include <platform/upmu_hw.h>
#include <platform/mt_pmic_wrap_init.h>
#include <platform/mt_gpt.h>
#include <platform/primary_display.h>
#include <platform/ddp_bls.h>
#include <libfdt.h>
#include <lk_builtin_dtb.h>

struct cust_mt65xx_led *get_cust_led_dtsi(void);
struct cust_mt65xx_led pled_dtsi[MT65XX_LED_TYPE_TOTAL];


char *leds_name[MT65XX_LED_TYPE_TOTAL] = {
	"red",
	"green",
	"blue",
	"jogball-backlight",
	"keyboard-backlight",
	"button-backlight",
	"lcd-backlight",
};

char *leds_node[MT65XX_LED_TYPE_TOTAL] = {
	"/odm/led@0",
	"/odm/led@1",
	"/odm/led@2",
	"/odm/led@3",
	"/odm/led@4",
	"/odm/led@5",
	"/odm/led@6",
};

/**
 * If the property is not found or the value is not set,
 * return default value 0.
 */
unsigned int led_fdt_getprop_u32(const void *fdt, int nodeoffset,
                                 const char *name)
{
	const void *data = NULL;
	int len = 0;

	data = fdt_getprop(fdt, nodeoffset, name, &len);
	if (len > 0 && data)
		return fdt32_to_cpu(*(unsigned int *)data);
	else
		return 0;
}

/**
 * If the property is not found or the value is not set,
 * use default value 0.
 */
void led_fdt_getprop_char_array(const void *fdt, int nodeoffset,
                                const char *name, char *out_value)
{
	const void *data = NULL;
	int len = 0;

	data = fdt_getprop(fdt, nodeoffset, name, &len);
	if (len > 0 && data)
		memcpy(out_value, data, len);
	else
		memset(out_value, 0, len);
}
/****************************************************************************
 * DEBUG MACROS
 ***************************************************************************/
int debug_enable = 1;
#define LEDS_DEBUG(format, args...) do{ \
        if(debug_enable) \
        {\
            dprintf(CRITICAL,format,##args);\
        }\
    }while(0)
#define LEDS_INFO LEDS_DEBUG
/****************************************************************************
 * structures
 ***************************************************************************/
static int g_lastlevel[MT65XX_LED_TYPE_TOTAL] = {-1, -1, -1, -1, -1, -1, -1};
int backlight_PWM_div = CLK_DIV1;

// Use Old Mode of PWM to suppoort 256 backlight level
#define BACKLIGHT_LEVEL_PWM_256_SUPPORT 256
extern unsigned int Cust_GetBacklightLevelSupport_byPWM(void);

/****************************************************************************
 * function prototypes
 ***************************************************************************/

/* internal functions */
static int brightness_set_pwm(int pwm_num, enum led_brightness level,struct PWM_config *config_data);
static int led_set_pwm(int pwm_num, enum led_brightness level);
static int brightness_set_pmic(enum mt65xx_led_pmic pmic_type, enum led_brightness level);
//static int brightness_set_gpio(int gpio_num, enum led_brightness level);
static int mt65xx_led_set_cust(struct cust_mt65xx_led *cust, int level);
/****************************************************************************
 * global variables
 ***************************************************************************/

/****************************************************************************
 * internal functions
 ***************************************************************************/
#if defined(MTK_MT6370_PMU_BLED_SUPPORT) // for mt6370 backlight support
struct mt6370_bled_config_t {
	u8 addr;
	u8 val;
};

static struct mt6370_bled_config_t mt6370_bled_settings[8] = {
	{.addr = 0xA1, .val = 0x89},
	{.addr = 0xA2, .val = 0x04},
	{.addr = 0xA3, .val = 0x00},
	{.addr = 0xA4, .val = 0x00},
	{.addr = 0xA5, .val = 0x00},
	{.addr = 0xA6, .val = 0x00},
	{.addr = 0xA7, .val = 0x00},
	{.addr = 0xA0, .val = 0x42}
};

// 6370 default pwm setting, used when dts not found
static struct mt6370_bled_config_t mt6370_bled_settings_default_pwm[8] = {
	{.addr = 0xA1, .val = 0xed},
	{.addr = 0xA2, .val = 0xb4},
	{.addr = 0xA3, .val = 0x30},
	{.addr = 0xA4, .val = 0x07},
	{.addr = 0xA5, .val = 0x3f},
	{.addr = 0xA6, .val = 0x00},
	{.addr = 0xA7, .val = 0x08},
	{.addr = 0xA0, .val = 0x7e}
};

static struct mt_i2c_t mt6370_bled_i2c = {
	.id = I2C5,
	.addr = 0x34,
	.mode = HS_MODE,
	.speed = 3400,
	.pushpull = true,
};

static void mt6370_i2c_write_byte(struct mt_i2c_t *i2c, u8 cmd, u8 data)
{
	unsigned int ret = I2C_OK;
	unsigned char write_buf[2] = {cmd, data};

	ret = i2c_write(i2c, write_buf, 2);
	if (ret != I2C_OK)
		LEDS_DEBUG("[LEDS] %s: I2CW[0x%02X] = 0x%02X failed, code = %d\n", __func__, cmd, data, ret);
}

static unsigned int led_fdt_getprop_bool(const void *fdt, int nodeoffset,
						const char *name)
{
	const void *data = NULL;
	int len = 0;

	data = fdt_getprop(fdt, nodeoffset, name, &len);
	if (data)
		return true;
	else
		return false;
}

#define MT6370_BLED_EN  (0x40)
#define MT6370_BLED_EXTEN (0x80)
#define MT6370_BLED_CHANENSHFT 2
#define MT6370_BLED_MAPLINEAR (0x02)
#define MT6370_BLED_OVPSHFT (5)
#define MT6370_BLED_OCPSHFT (1)
#define MT6370_BLED_PWMSHIFT (7)
#define MT6370_BLED_PWMDSHFT (5)
#define MT6370_BLED_PWMFSHFT (3)
#define MT6370_BLFLRAMP_SHFT (3)
#define MT6370_BLED_RAMPTSHFT (4)

static void mt6370_bled_parsing_dts(void *fdt)
{
	int offset, sub_offset;
	uint32_t val;

#if defined(USE_DTB_NO_DWS)
	offset = fdt_path_offset(get_lk_overlayed_dtb(), "/mt6370_pmu_dts");
	sub_offset = fdt_subnode_offset(get_lk_overlayed_dtb(), offset, "bled");
	if (offset >= 0 && sub_offset >= 0) {
		val = led_fdt_getprop_bool(get_lk_overlayed_dtb(), sub_offset, "mt,ext_en_pin");
		LEDS_INFO("[LEDS] mt,ext_en_pin val = %d\n", val);
		if (val) {
			mt6370_bled_settings[7].val &= ~(MT6370_BLED_EN | MT6370_BLED_EXTEN);
			mt6370_bled_settings[7].val |= MT6370_BLED_EXTEN;
		}
		val = led_fdt_getprop_u32(get_lk_overlayed_dtb(), sub_offset, "mt,chan_en");
		LEDS_INFO("[LEDS] mt,chan_en val = %d\n", val);
		if (val)
			mt6370_bled_settings[7].val |= (val << MT6370_BLED_CHANENSHFT);
		val = led_fdt_getprop_bool(get_lk_overlayed_dtb(), sub_offset, "mt,map_linear");
		LEDS_INFO("[LEDS] mt,map_linear = %d\n", val);
		if (!val)
			mt6370_bled_settings[7].val &= ~(MT6370_BLED_MAPLINEAR);
		val = led_fdt_getprop_u32(get_lk_overlayed_dtb(), sub_offset, "mt,bl_ovp_level");
		LEDS_INFO("[LEDS] mt,bl_ovp_level = %d\n", val);
		if (val)
			mt6370_bled_settings[0].val |= (val << MT6370_BLED_OVPSHFT);
		val = led_fdt_getprop_u32(get_lk_overlayed_dtb(), sub_offset, "mt,bl_ocp_level");
		LEDS_INFO("[LEDS] mt,bl_ocp_level = %d\n", val);
		if (val)
			mt6370_bled_settings[0].val |= (val << MT6370_BLED_OCPSHFT);
		val = led_fdt_getprop_bool(get_lk_overlayed_dtb(), sub_offset, "mt,use_pwm");
		LEDS_INFO("[LEDS] mt,use_pwm = %d\n", val);
		if (val)
			mt6370_bled_settings[1].val |= (val << MT6370_BLED_PWMSHIFT);
		val = led_fdt_getprop_u32(get_lk_overlayed_dtb(), sub_offset, "mt,pwm_fsample");
		LEDS_INFO("[LEDS] mt,pwm_fsample = %d\n", val);
		if (val)
			mt6370_bled_settings[1].val |= (val << MT6370_BLED_PWMFSHFT);
		val = led_fdt_getprop_u32(get_lk_overlayed_dtb(), sub_offset, "mt,pwm_deglitch");
		LEDS_INFO("[LEDS] mt,pwm_deglitch = %d\n", val);
		if (val)
			mt6370_bled_settings[1].val |= (val << MT6370_BLED_PWMDSHFT);
		val = led_fdt_getprop_u32(get_lk_overlayed_dtb(), sub_offset, "mt,bled_ramptime");
		LEDS_INFO("[LEDS] mt,bled_ramptime = %d\n", val);
		if (val)
			mt6370_bled_settings[2].val |= (val << MT6370_BLED_RAMPTSHFT);
		val = led_fdt_getprop_u32(get_lk_overlayed_dtb(), sub_offset, "mt,bled_flash_ramp");
		LEDS_INFO("[LEDS] mt,bled_flash_ramp = %d\n", val);
		if (val)
			mt6370_bled_settings[6].val |= (val << MT6370_BLFLRAMP_SHFT);
		val = led_fdt_getprop_u32(get_lk_overlayed_dtb(), sub_offset, "mt,max_bled_brightness");
		LEDS_INFO("[LEDS] mt,max_bled_brightness = %d\n", val);
		if (val) {
			val = ((u64)val * 255) >> 8;
			mt6370_bled_settings[3].val |= (val & 0x7);
			mt6370_bled_settings[4].val |= ((val >> 3) & 0xff);
		}
	}
	else
#endif
	{
		memcpy(mt6370_bled_settings, mt6370_bled_settings_default_pwm, sizeof(mt6370_bled_settings));
		LEDS_INFO("[LEDS] cannot read settings from dts, using default setting\n");
	}
}

static void mt6370_init_bled()
{
	unsigned int i;

	LEDS_INFO("[LEDS]LK: %s\n", __func__);
	mt6370_bled_parsing_dts(get_lk_overlayed_dtb());
	for (i = 0; i < sizeof(mt6370_bled_settings) / sizeof(struct mt6370_bled_config_t); i++) {
		LEDS_INFO("[LEDS] set addr 0x%02X = 0x%02x\n", mt6370_bled_settings[i].addr, mt6370_bled_settings[i].val);
		mt6370_i2c_write_byte(&mt6370_bled_i2c, mt6370_bled_settings[i].addr, mt6370_bled_settings[i].val);
	}
}
#endif

static int brightness_mapto64(int level)
{
	if (level < 30)
		return (level >> 1) + 7;
	else if (level <= 120)
		return (level >> 2) + 14;
	else if (level <= 160)
		return level / 5 + 20;
	else
		return (level >> 3) + 33;
}
unsigned int brightness_mapping(unsigned int level)
{
	unsigned int mapped_level;

	mapped_level = level;

	return mapped_level;
}

static int brightness_set_pwm(int pwm_num, enum led_brightness level,struct PWM_config *config_data)
{
	struct pwm_spec_config pwm_setting;
	unsigned int BacklightLevelSupport = Cust_GetBacklightLevelSupport_byPWM();
	pwm_setting.pwm_no = pwm_num;
	if (BacklightLevelSupport == BACKLIGHT_LEVEL_PWM_256_SUPPORT)
		pwm_setting.mode = PWM_MODE_OLD;
	else
		pwm_setting.mode = PWM_MODE_FIFO; // New mode fifo and periodical mode

	pwm_setting.pmic_pad = config_data->pmic_pad;
	if (config_data->div) {
		pwm_setting.clk_div = config_data->div;
		backlight_PWM_div = config_data->div;
	} else
		pwm_setting.clk_div = CLK_DIV1;

	if (BacklightLevelSupport== BACKLIGHT_LEVEL_PWM_256_SUPPORT) {
		if (config_data->clock_source) {
			pwm_setting.clk_src = PWM_CLK_OLD_MODE_BLOCK;
		} else {
			pwm_setting.clk_src = PWM_CLK_OLD_MODE_32K; // actually. it's block/1625 = 26M/1625 = 16KHz @ MT6571
		}

		pwm_setting.pwm_mode.PWM_MODE_OLD_REGS.IDLE_VALUE = 0;
		pwm_setting.pwm_mode.PWM_MODE_OLD_REGS.GUARD_VALUE = 0;
		pwm_setting.pwm_mode.PWM_MODE_OLD_REGS.GDURATION = 0;
		pwm_setting.pwm_mode.PWM_MODE_OLD_REGS.WAVE_NUM = 0;
		pwm_setting.pwm_mode.PWM_MODE_OLD_REGS.DATA_WIDTH = 255; // 256 level
		pwm_setting.pwm_mode.PWM_MODE_OLD_REGS.THRESH = level;

		LEDS_DEBUG("[LEDS][%d] LK: backlight_set_pwm:duty is %d/%d\n", BacklightLevelSupport, level, pwm_setting.pwm_mode.PWM_MODE_OLD_REGS.DATA_WIDTH);
		LEDS_DEBUG("[LEDS][%d] LK: backlight_set_pwm:clk_src/div is %d%d\n", BacklightLevelSupport, pwm_setting.clk_src, pwm_setting.clk_div);
		if (level >0 && level < 256) {
			pwm_set_spec_config(&pwm_setting);
			LEDS_DEBUG("[LEDS][%d] LK: backlight_set_pwm: old mode: thres/data_width is %d/%d\n", BacklightLevelSupport, pwm_setting.pwm_mode.PWM_MODE_OLD_REGS.THRESH, pwm_setting.pwm_mode.PWM_MODE_OLD_REGS.DATA_WIDTH);
		} else {
			LEDS_DEBUG("[LEDS][%d] LK: Error level in backlight\n", BacklightLevelSupport);
			mt_pwm_disable(pwm_setting.pwm_no, config_data->pmic_pad);
		}
		return 0;

	} else {
		if (config_data->clock_source) {
			pwm_setting.clk_src = PWM_CLK_NEW_MODE_BLOCK;
		} else {
			pwm_setting.clk_src = PWM_CLK_NEW_MODE_BLOCK_DIV_BY_1625;
		}

		if (config_data->High_duration && config_data->low_duration) {
			pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.HDURATION = config_data->High_duration;
			pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.LDURATION = pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.HDURATION;
		} else {
			pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.HDURATION = 4;
			pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.LDURATION = 4;
		}

		pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.IDLE_VALUE = 0;
		pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.GUARD_VALUE = 0;
		pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.STOP_BITPOS_VALUE = 31;
		pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.GDURATION = (pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.HDURATION + 1) * 32 - 1;
		pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.WAVE_NUM = 0;

		LEDS_DEBUG("[LEDS] LK: backlight_set_pwm:duty is %d\n", level);
		LEDS_DEBUG("[LEDS] LK: backlight_set_pwm:clk_src/div/high/low is %d%d%d%d\n", pwm_setting.clk_src, pwm_setting.clk_div, pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.HDURATION, pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.LDURATION);

		if (level > 0 && level <= 32) {
			pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.GUARD_VALUE = 0;
			pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.SEND_DATA0 = (1 << level) - 1;
			pwm_set_spec_config(&pwm_setting);
		} else if (level > 32 && level <= 64) {
			pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.GUARD_VALUE = 1;
			level -= 32;
			pwm_setting.pwm_mode.PWM_MODE_FIFO_REGS.SEND_DATA0 = (1 << level) - 1 ;
			pwm_set_spec_config(&pwm_setting);
		} else {
			LEDS_DEBUG("[LEDS] LK: Error level in backlight\n");
			mt_pwm_disable(pwm_setting.pwm_no, config_data->pmic_pad);
		}

		return 0;
	}
}

static int led_set_pwm(int pwm_num, enum led_brightness level)
{
	struct pwm_spec_config pwm_setting;
	pwm_setting.pwm_no = pwm_num;
	pwm_setting.clk_div = CLK_DIV1;

	pwm_setting.pwm_mode.PWM_MODE_OLD_REGS.DATA_WIDTH = 50;

	// We won't choose 32K to be the clock src of old mode because of system performance.
	// The setting here will be clock src = 26MHz, CLKSEL = 26M/1625 (i.e. 16K)
	pwm_setting.clk_src = PWM_CLK_OLD_MODE_32K;

	if (level) {
		pwm_setting.pwm_mode.PWM_MODE_OLD_REGS.THRESH = 15;
	} else {
		pwm_setting.pwm_mode.PWM_MODE_OLD_REGS.THRESH = 0;
	}
	LEDS_INFO("[LEDS]LK: brightness_set_pwm: level=%d, clk=%d \n\r", level, pwm_setting.clk_src);
	pwm_set_spec_config(&pwm_setting);

	return 0;
}

static int brightness_set_pmic(enum mt65xx_led_pmic pmic_type, enum led_brightness level)
{
	LEDS_INFO("[LEDS]LK: PMIC Type: %d, Level: %d\n", pmic_type, level);
#if defined(PMIC_CHIP_MT6355)
	LEDS_INFO("[LEDS]LK: MT6355 driver not ready");
#else
#if 0 /* Fix build error for PMIC MT6358 */
	// Note that: the ISINK for Backlight is not support anymore
	if (pmic_type == MT65XX_LED_PMIC_NLED_ISINK0) {
		pmic_set_register_value(PMIC_RG_DRV_32K_CK_PDN,0x0); // Disable power down
		pmic_set_register_value(PMIC_RG_DRV_ISINK0_CK_PDN,0);
		pmic_set_register_value(PMIC_RG_DRV_ISINK0_CK_CKSEL,0);
		pmic_set_register_value(PMIC_ISINK_CH0_MODE,ISINK_PWM_MODE);
		pmic_set_register_value(PMIC_ISINK_CH0_STEP,ISINK_3);//16mA
		pmic_set_register_value(PMIC_ISINK_DIM0_DUTY,15);
		pmic_set_register_value(PMIC_ISINK_DIM0_FSEL,ISINK_1KHZ);//1KHz
		if (level) {
			pmic_set_register_value(PMIC_RG_DRV_32K_CK_PDN,0x0); // Disable power down
			pmic_set_register_value(PMIC_ISINK_CH0_EN,0x1); // Turn on ISINK Channel 0

		} else {
			pmic_set_register_value(PMIC_ISINK_CH0_EN,0x0); // Turn off ISINK Channel 0
		}
		return 0;
	} else if (pmic_type == MT65XX_LED_PMIC_NLED_ISINK1) {
		pmic_set_register_value(PMIC_RG_DRV_32K_CK_PDN,0x0); // Disable power down
		pmic_set_register_value(PMIC_RG_DRV_ISINK1_CK_PDN,0);
		pmic_set_register_value(PMIC_RG_DRV_ISINK1_CK_CKSEL,0);
		pmic_set_register_value(PMIC_ISINK_CH1_MODE,ISINK_PWM_MODE);
		pmic_set_register_value(PMIC_ISINK_CH1_STEP,ISINK_3);//16mA
		pmic_set_register_value(PMIC_ISINK_DIM1_DUTY,15);
		pmic_set_register_value(PMIC_ISINK_DIM1_FSEL,ISINK_1KHZ);//1KHz
		if (level) {
			pmic_set_register_value(PMIC_RG_DRV_32K_CK_PDN,0x0); // Disable power down
			pmic_set_register_value(PMIC_ISINK_CH1_EN,0x1); // Turn on ISINK Channel 0

		} else {
			pmic_set_register_value(PMIC_ISINK_CH1_EN,0x0); // Turn off ISINK Channel 0
		}
		return 0;
	} else if (pmic_type == MT65XX_LED_PMIC_NLED_ISINK2) {
		pmic_set_register_value(PMIC_RG_DRV_32K_CK_PDN, 0x0);   /* Disable power down */
		pmic_set_register_value(PMIC_RG_DRV_ISINK4_CK_PDN, 0);
		pmic_set_register_value(PMIC_RG_DRV_ISINK4_CK_CKSEL, 0);
		pmic_set_register_value(PMIC_ISINK_CH4_MODE, ISINK_PWM_MODE);
		pmic_set_register_value(PMIC_ISINK_CH4_STEP, ISINK_3);  /* 16mA */
		pmic_set_register_value(PMIC_ISINK_DIM4_DUTY, 15);
		pmic_set_register_value(PMIC_ISINK_DIM4_FSEL, ISINK_1KHZ);  /* 1KHz */
		if (level) {
			pmic_set_register_value(PMIC_RG_DRV_32K_CK_PDN, 0x0);   /* Disable power down */
			pmic_set_register_value(PMIC_ISINK_CH4_EN, 0x1);    /* Turn on ISINK Channel 4 */
		} else {
			pmic_set_register_value(PMIC_ISINK_CH4_EN, 0x0);    /* Turn off ISINK Channel 4 */
		}
		return 0;
	} else if (pmic_type == MT65XX_LED_PMIC_NLED_ISINK3) {
		pmic_set_register_value(PMIC_RG_DRV_32K_CK_PDN, 0x0);   /* Disable power down */
		pmic_set_register_value(PMIC_RG_DRV_ISINK5_CK_PDN, 0);
		pmic_set_register_value(PMIC_RG_DRV_ISINK5_CK_CKSEL, 0);
		pmic_set_register_value(PMIC_ISINK_CH5_MODE, ISINK_PWM_MODE);
		pmic_set_register_value(PMIC_ISINK_CH5_STEP, ISINK_3);  /* 16mA */
		pmic_set_register_value(PMIC_ISINK_DIM5_DUTY, 15);
		pmic_set_register_value(PMIC_ISINK_DIM5_FSEL, ISINK_1KHZ);  /* 1KHz */
		if (level) {
			pmic_set_register_value(PMIC_RG_DRV_32K_CK_PDN, 0x0);   /* Disable power down */
			pmic_set_register_value(PMIC_ISINK_CH5_EN, 0x1);    /* Turn on ISINK Channel 5 */
		} else {
			pmic_set_register_value(PMIC_ISINK_CH5_EN, 0x0);    /* Turn off ISINK Channel 5 */
		}
		return 0;
	}
#endif
#endif
	return -1;
}

static int mt65xx_led_set_cust(struct cust_mt65xx_led *cust, int level)
{
	unsigned int BacklightLevelSupport = Cust_GetBacklightLevelSupport_byPWM();
	if (level > LED_FULL)
		level = LED_FULL;
	else if (level < 0)
		level = 0;

	switch (cust->mode) {

		case MT65XX_LED_MODE_PWM:
			if (level == 0) {
				//LEDS_INFO("[LEDS]LK: mt65xx_leds_set_cust: enter mt_pwm_disable()\n");
				mt_pwm_disable(cust->data, cust->config_data.pmic_pad);
				return 1;
			}
			if (strcmp(cust->name,"lcd-backlight") == 0) {
				if (BacklightLevelSupport == BACKLIGHT_LEVEL_PWM_256_SUPPORT)
					level = brightness_mapping(level);
				else
					level = brightness_mapto64(level);
				return brightness_set_pwm(cust->data, level,&cust->config_data);
			} else {
				return led_set_pwm(cust->data, level);
			}

		case MT65XX_LED_MODE_GPIO:
			return ((cust_brightness_set)(cust->data))(level);
		case MT65XX_LED_MODE_PMIC:
			return brightness_set_pmic(cust->data, level);
		case MT65XX_LED_MODE_CUST_LCM:
			return ((cust_brightness_set)(cust->data))(level);
		case MT65XX_LED_MODE_CUST_BLS_PWM:
			return ((cust_brightness_set)(cust->data))(level);
		case MT65XX_LED_MODE_NONE:
		default:
			break;
	}
	return -1;
}

/****************************************************************************
 * external functions
 ***************************************************************************/
int mt65xx_leds_brightness_set(enum mt65xx_led_type type, enum led_brightness level)
{
	struct cust_mt65xx_led *cust_led_list = get_cust_led_dtsi();
	if (cust_led_list == NULL) {
		cust_led_list = get_cust_led_list();
		LEDS_DEBUG("Cannot not get the LED info from device tree. \n");
	}

	if (type >= MT65XX_LED_TYPE_TOTAL)
		return -1;

	if (level > LED_FULL)
		level = LED_FULL;
//	else if (level < 0)  //level cannot < 0
//		level = 0;

	if (g_lastlevel[type] != (int)level) {
		g_lastlevel[type] = level;
		dprintf(CRITICAL,"[LEDS]LK: %s level is %d \n\r", cust_led_list[type].name, level);
		return mt65xx_led_set_cust(&cust_led_list[type], level);
	} else {
		return -1;
	}

}

void leds_battery_full_charging(void)
{
	mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, LED_OFF);
	mt65xx_leds_brightness_set(MT65XX_LED_TYPE_GREEN, LED_FULL);
	mt65xx_leds_brightness_set(MT65XX_LED_TYPE_BLUE, LED_OFF);
}

void leds_battery_low_charging(void)
{
	mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, LED_FULL);
	mt65xx_leds_brightness_set(MT65XX_LED_TYPE_GREEN, LED_OFF);
	mt65xx_leds_brightness_set(MT65XX_LED_TYPE_BLUE, LED_OFF);
}

void leds_battery_medium_charging(void)
{
	mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, LED_FULL);
	mt65xx_leds_brightness_set(MT65XX_LED_TYPE_GREEN, LED_OFF);
	mt65xx_leds_brightness_set(MT65XX_LED_TYPE_BLUE, LED_OFF);
}

void leds_init(void)
{
#if defined(MTK_MT6370_PMU_BLED_SUPPORT) // for mt6370 backlight support
	mt6370_init_bled();
#endif
	LEDS_INFO("[LEDS]LK: leds_init: mt65xx_backlight_off \n\r");
	mt65xx_backlight_off();
}

void leds_deinit(void)
{
	LEDS_INFO("[LEDS]LK: leds_deinit: LEDS off \n\r");
	mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, LED_OFF);
	mt65xx_leds_brightness_set(MT65XX_LED_TYPE_GREEN, LED_OFF);
	mt65xx_leds_brightness_set(MT65XX_LED_TYPE_BLUE, LED_OFF);
}

void mt65xx_backlight_on(void)
{
	enum led_brightness backlight_level = get_cust_led_default_level();
	LEDS_INFO("[LEDS]LK: mt65xx_backlight_on:level =  %d\n\r",backlight_level);
	mt65xx_leds_brightness_set(MT65XX_LED_TYPE_LCD, backlight_level);
}

void mt65xx_backlight_off(void)
{
	LEDS_INFO("[LEDS]LK: mt65xx_backlight_off \n\r");
	mt65xx_leds_brightness_set(MT65XX_LED_TYPE_LCD, LED_OFF);
}

struct cust_mt65xx_led *get_cust_led_dtsi(void)
{
	static bool isDTinited = false;
	int i, offset;
	int pwm_config[5] = {0};

#if defined(USE_DTB_NO_DWS)
	if ( isDTinited == true )
		goto out;
	for (i = 0; i < MT65XX_LED_TYPE_TOTAL; i++) {
		pled_dtsi[i].name = leds_name[i];
		offset = fdt_path_offset(get_lk_overlayed_dtb(), leds_node[i]);
		if (offset < 0) {
			LEDS_DEBUG("[LEDS]LK:Cannot find LED node from dts\n");
			pled_dtsi[i].mode = 0;
			pled_dtsi[i].data = -1;
		} else {
			isDTinited = true;
			pled_dtsi[i].mode = led_fdt_getprop_u32(get_lk_overlayed_dtb(), offset, "led_mode");
			pled_dtsi[i].data = led_fdt_getprop_u32(get_lk_overlayed_dtb(), offset, "data");
			led_fdt_getprop_char_array(get_lk_overlayed_dtb(), offset, "pwm_config", (char *)pwm_config);
			pled_dtsi[i].config_data.clock_source = pwm_config[0];
			pled_dtsi[i].config_data.div = pwm_config[1];
			pled_dtsi[i].config_data.low_duration = pwm_config[2];
			pled_dtsi[i].config_data.High_duration = pwm_config[3];
			pled_dtsi[i].config_data.pmic_pad = pwm_config[4];
			switch (pled_dtsi[i].mode) {
				case MT65XX_LED_MODE_CUST_LCM:
					pled_dtsi[i].data = (long)primary_display_setbacklight;
					LEDS_DEBUG("[LEDS]LK:The backlight hw mode is LCM.\n");
					break;
				case MT65XX_LED_MODE_CUST_BLS_PWM:
					pled_dtsi[i].data = (long)disp_bls_set_backlight;
					LEDS_DEBUG("[LEDS]LK:The backlight hw mode is BLS.\n");
					break;
				default:
					break;
			}
			LEDS_INFO("[LEDS]LK:led[%d] offset is %d,mode is %d,data is %d .\n",    \
			          i,offset,pled_dtsi[i].mode,pled_dtsi[i].data);
		}
	}
#endif
	if ( isDTinited == false )
		return NULL;
out:
	return pled_dtsi;
}
