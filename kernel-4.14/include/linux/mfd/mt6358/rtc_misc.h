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

#ifndef MTK_RTC_H
#define MTK_RTC_H

#include <linux/rtc.h>

enum rtc_gpio_user_t {
	RTC_GPIO_USER_WIFI = 8,
	RTC_GPIO_USER_GPS = 9,
	RTC_GPIO_USER_BT = 10,
	RTC_GPIO_USER_FM = 11,
	RTC_GPIO_USER_PMIC = 12,
};

#ifdef CONFIG_MT6358_MISC
void rtc_mark_recovery(void);
void rtc_mark_kpoc(void);
void rtc_mark_fast(void);
int set_rtc_spare_fg_value(int val);
int get_rtc_spare_fg_value(void);
int set_rtc_spare0_fg_value(int val);
int get_rtc_spare0_fg_value(void);
void rtc_gpio_enable_32k(enum rtc_gpio_user_t user);
void rtc_gpio_disable_32k(enum rtc_gpio_user_t user);
void rtc_enable_32k1v8_1(void);
void rtc_disable_32k1v8_1(void);
#else
#define rtc_mark_recovey()		do {} while (0)
#define rtc_mark_kpoc()			do {} while (0)
#define rtc_mark_fast()			do {} while (0)
#define set_rtc_spare_fg_value(val)	({ 0; })
#define get_rtc_spare_fg_value()	({ 0; })
#define set_rtc_spare0_fg_value(val)	({ 0; })
#define get_rtc_spare0_fg_value()	({ 0; })
#define rtc_gpio_enable_32k(user)	do {} while (0)
#define rtc_gpio_disable_32k(user)	do {} while (0)
#define rtc_enable_32k1v8_1()		do {} while (0)
#define rtc_disable_32k1v8_1()		do {} while (0)
#endif

#endif
