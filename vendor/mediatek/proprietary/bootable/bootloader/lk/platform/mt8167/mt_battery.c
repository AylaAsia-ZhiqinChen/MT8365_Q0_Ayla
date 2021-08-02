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

#include <target/board.h>
#ifdef MTK_KERNEL_POWER_OFF_CHARGING
#define CFG_POWER_CHARGING
#endif
#ifdef CFG_POWER_CHARGING
#include <platform/mt_typedefs.h>
#include <platform/mt_reg_base.h>
#include <platform/mt_pmic.h>
#include <platform/upmu_hw.h>
#include <platform/upmu_common.h>
#include <platform/boot_mode.h>
#include <platform/mt_gpt.h>
#include <platform/mt_rtc.h>
//#include <platform/mt_disp_drv.h>
//#include <platform/mtk_wdt.h>
//#include <platform/mtk_key.h>
//#include <platform/mt_logo.h>
#include <platform/mt_leds.h>
#include <printf.h>
#include <sys/types.h>
#include <target/cust_battery.h>

#if defined(MTK_NCP1854_SUPPORT)
#include <platform/ncp1854.h>
#endif

#if defined(MTK_BQ24196_SUPPORT)
#include <platform/bq24196.h>
#endif

#if defined(MTK_BQ24296_SUPPORT)
#include <platform/bq24296.h>
#endif

#undef printf


/*****************************************************************************
 *  Type define
 ****************************************************************************/
#if defined(CUST_BATTERY_LOWVOL_THRESOLD)
#define BATTERY_LOWVOL_THRESOLD CUST_BATTERY_LOWVOL_THRESOLD
#else
#define BATTERY_LOWVOL_THRESOLD             3450
#endif
#define	BATTERY_CHARGING_VOLTAGE_RAISE	150

/*****************************************************************************
 *  Global Variable
 ****************************************************************************/
bool g_boot_reason_change = false;
int fix_coverity = 0;
CHARGER_TYPE CHR_Type_num = CHARGER_UNKNOWN;

#if defined(STD_AC_LARGE_CURRENT)
int g_std_ac_large_current_en=1;
#else
int g_std_ac_large_current_en=0;
#endif

/*****************************************************************************
 *  Externl Variable
 ****************************************************************************/
extern bool g_boot_menu;
extern void mtk_wdt_restart(void);

int get_bat_volt(int times)
{
	int bat_vol;
#if defined(SWCHR_POWER_PATH)
	bat_vol = get_i_sense_volt(times);
#else
	bat_vol = get_bat_sense_volt(times);
#endif
	return bat_vol;
}

void kick_charger_wdt(void)
{
	//upmu_set_rg_chrwdt_td(0x0);           // CHRWDT_TD, 4s
	mt6392_upmu_set_rg_chrwdt_td(0x3);           // CHRWDT_TD, 32s for keep charging for lk to kernel
	mt6392_upmu_set_rg_chrwdt_wr(1);              // CHRWDT_WR
	mt6392_upmu_set_rg_chrwdt_int_en(1);         // CHRWDT_INT_EN
	mt6392_upmu_set_rg_chrwdt_en(1);             // CHRWDT_EN
	mt6392_upmu_set_rg_chrwdt_flag_wr(1);        // CHRWDT_WR

}

kal_bool is_low_battery(kal_int32  val)
{
	static UINT8 g_bat_low = 0xFF;
	int battery_lowvol_threshold = BATTERY_LOWVOL_THRESOLD;

	//low battery only justice once in lk
	if (0xFF != g_bat_low)
		return g_bat_low;
	else
		g_bat_low = FALSE;

#if defined(SWCHR_POWER_PATH)
	if (0 == val)
		val = get_i_sense_volt(1);
#else
	if (0 == val)
		val = get_bat_sense_volt(1);
#endif

	if (g_boot_mode == KERNEL_POWER_OFF_CHARGING_BOOT && upmu_is_chr_det() == KAL_TRUE) {
		battery_lowvol_threshold += BATTERY_CHARGING_VOLTAGE_RAISE;
		printf("%s, charging, low battery threshold is %d mV\n", __FUNCTION__, battery_lowvol_threshold);
	}

	if (val < battery_lowvol_threshold) {
		printf("%s, TRUE\n", __FUNCTION__);
		g_bat_low = 0x1;
	}

	if (FALSE == g_bat_low)
		printf("%s, FALSE\n", __FUNCTION__);

	return g_bat_low;
}

void pchr_turn_on_charging(kal_bool bEnable)
{
	//force leave USBDL mode
	mt6392_upmu_set_rg_usbdl_rst(1);

	kick_charger_wdt();

	if (bEnable) {
		if (CHR_Type_num == STANDARD_CHARGER || CHR_Type_num == CHARGING_HOST)
			mt6392_upmu_set_rg_cs_vth(0x08);		// CS_VTH, 800mA
		else
			mt6392_upmu_set_rg_cs_vth(0xC);		// CS_VTH, 450mA
	}
	mt6392_upmu_set_rg_csdac_en(bEnable);	// CSDAC_EN
	mt6392_upmu_set_rg_chr_en(bEnable); 	// CHR_EN

#if defined(MTK_NCP1854_SUPPORT)
	ncp1854_hw_init();
	ncp1854_charging_enable(bEnable);
	ncp1854_dump_register();
#endif

#if defined(MTK_BQ24196_SUPPORT)
	bq24196_hw_init();
	bq24196_charging_enable(bEnable);
	bq24196_dump_register();
#endif

#if defined(MTK_BQ24296_SUPPORT)
	bq24296_hw_init();
	bq24296_turn_on_charging();
	bq24296_dump_register();
#endif
}

//enter this function when low battery with charger
void check_bat_protect_status()
{
	kal_int32 bat_val = 0;

#if defined(SWCHR_POWER_PATH)
	bat_val = get_i_sense_volt(1);
#else
	bat_val = get_bat_sense_volt(1);
#endif

	dprintf(CRITICAL, "[%s]: check VBAT=%d mV with %d mV, start charging... \n", __FUNCTION__, bat_val, BATTERY_LOWVOL_THRESOLD);

	if (bat_val < BATTERY_LOWVOL_THRESOLD)
		mt65xx_leds_brightness_set(MT65XX_LED_TYPE_RED, LED_FULL);

	while (bat_val < BATTERY_LOWVOL_THRESOLD + BATTERY_CHARGING_VOLTAGE_RAISE) {
		mtk_wdt_restart();
		if (upmu_is_chr_det() == KAL_FALSE) {
			dprintf(CRITICAL, "[BATTERY] No Charger, Power OFF !\n");
			mt6575_power_off();
			while (1) {
				if (fix_coverity == 1)
					return;
			}
		}

		pchr_turn_on_charging(KAL_TRUE);

		mdelay(5000);

#if defined(SWCHR_POWER_PATH)
		pchr_turn_on_charging(KAL_FALSE);
		mdelay(100);
		bat_val = get_i_sense_volt(1);
#else
		bat_val = get_bat_sense_volt(1);
		dprintf(CRITICAL, "[%s]: check VBAT=%d mV with %d mV, charging... \n", __FUNCTION__, bat_val, BATTERY_LOWVOL_THRESOLD + BATTERY_CHARGING_VOLTAGE_RAISE);
#endif
	}

	dprintf(CRITICAL, "[%s]: check VBAT=%d mV with %d mV, stop charging... \n", __FUNCTION__, bat_val, BATTERY_LOWVOL_THRESOLD + BATTERY_CHARGING_VOLTAGE_RAISE);
}

void mt65xx_bat_init(void)
{
	kal_int32 bat_vol;

	// Low Battery Safety Booting

#if defined(SWCHR_POWER_PATH)
	bat_vol = get_i_sense_volt(1);
#else
	bat_vol = get_bat_sense_volt(1);
#endif

	if (bat_vol == 56789)
		fix_coverity = 1;

	if (upmu_is_chr_det() == KAL_TRUE)
		CHR_Type_num = mt_charger_type_detection();

	pchr_turn_on_charging(KAL_TRUE);
	dprintf(CRITICAL, "[mt65xx_bat_init] check VBAT=%d mV with %d mV\n", bat_vol, BATTERY_LOWVOL_THRESOLD);

	if (g_boot_mode == KERNEL_POWER_OFF_CHARGING_BOOT && (mt6392_upmu_get_pwrkey_deb()==0) ) {
		dprintf(CRITICAL, "[mt65xx_bat_init] KPOC+PWRKEY => change boot mode\n");

		g_boot_reason_change = true;
	}

	rtc_boot_check(false);

#ifndef MTK_DISABLE_POWER_ON_OFF_VOLTAGE_LIMITATION
	if (is_low_battery(bat_vol))
	{
		if (g_boot_mode == KERNEL_POWER_OFF_CHARGING_BOOT && upmu_is_chr_det() == KAL_TRUE) {
			dprintf(CRITICAL, "[%s] Kernel Low Battery Power Off Charging Mode\n", __func__);
			g_boot_mode = LOW_POWER_OFF_CHARGING_BOOT;

			check_bat_protect_status();
			return;
		} else {
			dprintf(CRITICAL, "[BATTERY] battery voltage(%dmV) <= CLV ! Can not Boot Linux Kernel !! \n\r",bat_vol);
#ifndef NO_POWER_OFF
			mt6575_power_off();
#endif
			while (1) {
				dprintf(CRITICAL, "If you see the log, please check with RTC power off API\n\r");
				if (fix_coverity == 1)
					return;
			}
		}
	}
#endif
	return;
}

#else

#include <platform/mt_typedefs.h>
#include <platform/mt_reg_base.h>
#include <printf.h>

void mt65xx_bat_init(void)
{
	dprintf(CRITICAL, "[BATTERY] Skip mt65xx_bat_init !!\n\r");
	dprintf(CRITICAL, "[BATTERY] If you want to enable power off charging, \n\r");
	dprintf(CRITICAL, "[BATTERY] Please #define CFG_POWER_CHARGING!!\n\r");
}

/* dummy function */
int get_bat_volt(int times)
{
	int bat_vol = 0;
	return bat_vol;
}
#endif
