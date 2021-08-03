/*
 * Copyright (C) 2018 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#ifndef __ADSP_HELPER_H__
#define __ADSP_HELPER_H__

#include "adsp_reg.h"
#include <linux/platform_device.h>


#define DSP_LOG_BUF_MAGIC          (0x67676F6C)


/*
 * Design const definition with specifi platform.
 * SHARED_DTCM
 * HEAD|---logger(1KB)---+---IPC(1KB)---|TAIL
 */
#define SIZE_SHARED_DTCM_FOR_IPC      (0x0400)  /* 1KB */
#define SIZE_SHARED_DTCM_FOR_LOGGER   (0x0400)  /* 1KB */
#define TOTAL_SIZE_SHARED_DTCM_FROM_TAIL  \
		(SIZE_SHARED_DTCM_FOR_IPC + SIZE_SHARED_DTCM_FOR_LOGGER)

/*
 * CPU to DSP DRAM remap control Register, bit 31:12 valid,
 * need write offset = CPU Dram Address - DSP Dram Base Address
 */
#define DSP_EMI_BASE_ADDR             (0x1000181C)
#define DRAM_PHYS_BASE_FROM_DSP_VIEW  (0x40020000) /* MT8168 DSP view */


enum ADSP_TCM_MODE {
	TCM_MODE1 = 0x0,
	TCM_MODE2 = 0x1,
	TCM_MODE3 = 0x2,
};

struct hifi4dsp_log_ctrl {
	u32 magic;
	u32 start;
	u32 size;
	u32 offset;
	int full;
};


struct adsp_private_data {
	struct device *dev;
	/* maybe add other items */
};

/*
 * Global important adsp data structure.
 */
struct adsp_chip_info {
	phys_addr_t pa_itcm;
	phys_addr_t pa_dtcm;
	phys_addr_t pa_dram; /* adsp dram physical base */
	phys_addr_t pa_cfgreg;
	void __iomem *va_itcm;
	void __iomem *va_dtcm; /* corresponding to pa_dtcm */
	void __iomem *va_dram; /* corresponding to pa_dram */
	void __iomem *va_cfgreg;
	void __iomem *shared_dtcm; /* part of  va_dtcm */
	void __iomem *shared_dram; /* part of  va_dram */
	u32 itcmsize;
	u32 dtcmsize;
	u32 dramsize;
	u32 cfgregsize;
	u32 shared_dtcm_size;
	u32 shared_dram_size;
	phys_addr_t adsp_bootup_addr;
	int adsp_bootup_done;
	struct adsp_private_data *data;
};


/* adsp helper api */
extern int platform_parse_resource(struct platform_device *pdev, void *data);
extern int adsp_shared_base_ioremap(struct platform_device *pdev, void *data);
extern int adsp_wdt_device_init(struct platform_device *pdev);
extern void adsp_wdt_stop(void);
extern int adsp_wdt_device_remove(struct platform_device *pdev);
extern int adsp_must_setting_early(struct device *dev);
extern int adsp_clock_power_on(struct device *dev);
extern int adsp_clock_power_off(struct device *dev);
extern void hifixdsp_boot_sequence(u32 boot_addr);
extern void hifixdsp_shutdown(void);
extern int adsp_misc_setting_after_poweron(void);
extern int adsp_remove_setting_after_shutdown(void);
extern int adsp_shutdown_notify_check(void);
extern void init_adsp_sysram_reserve_mblock(phys_addr_t pbase,
				void __iomem *vbase);
extern void *get_adsp_chip_data(void);
extern void __iomem *get_adsp_reg_base(void);
extern int adsp_create_sys_files(struct device *dev);
extern void adsp_destroy_sys_files(struct device *dev);
extern unsigned int is_from_suspend;

#endif
