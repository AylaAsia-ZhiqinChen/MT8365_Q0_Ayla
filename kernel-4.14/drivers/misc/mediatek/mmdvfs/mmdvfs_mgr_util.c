// SPDX-License-Identifier: GPL-2.0
/*
 * MediaTek MMDVFS driver.
 *
 * Copyright (c) 2018 MediaTek Inc.
 */

#include <linux/uaccess.h>
#include "mmdvfs_mgr.h"
#include "mmdvfs_internal.h"

static mmdvfs_state_change_cb quick_state_change_cbs[MMDVFS_SCEN_COUNT];

enum mmdvfs_lcd_size_enum mmdvfs_get_lcd_resolution(void)
{
	enum mmdvfs_lcd_size_enum result = MMDVFS_LCD_SIZE_HD;
	long lcd_resolution = 0;
	long lcd_w = 0;
	long lcd_h = 0;
	int convert_err = -EINVAL;

#if defined(CONFIG_LCM_WIDTH) && defined(CONFIG_LCM_HEIGHT)
	convert_err = kstrtoul(CONFIG_LCM_WIDTH, 10, &lcd_w);
	if (!convert_err)
		convert_err = kstrtoul(CONFIG_LCM_HEIGHT, 10, &lcd_h);
#endif	/* CONFIG_LCM_WIDTH, CONFIG_LCM_HEIGHT */

	if (convert_err) {
#if !defined(CONFIG_FPGA_EARLY_PORTING) && defined(CONFIG_MTK_FB)
		lcd_w = DISP_GetScreenWidth();
		lcd_h = DISP_GetScreenHeight();
#else
		MMDVFSMSG(
			"unable to get resolution, query API is unavailable\n");
#endif
	}

	lcd_resolution = lcd_w * lcd_h;

	if (lcd_resolution <= MMDVFS_DISPLAY_SIZE_HD)
		result = MMDVFS_LCD_SIZE_HD;
	else if (lcd_resolution <= MMDVFS_DISPLAY_SIZE_FHD)
		result = MMDVFS_LCD_SIZE_FHD;
	else
		result = MMDVFS_LCD_SIZE_WQHD;

	return result;
}

void mmdvfs_internal_handle_state_change(
	struct mmdvfs_state_change_event *event)
{
	int i = 0;

	for (i = 0; i < MMDVFS_SCEN_COUNT; i++) {
		mmdvfs_state_change_cb func = quick_state_change_cbs[i];

		if (func != NULL)
			func(event);
	}
}
