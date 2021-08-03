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

#ifndef __ADSP_IPI_H
#define __ADSP_IPI_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/poll.h>
#include <linux/io.h>
#include <linux/of_fdt.h>
#include <linux/of_reserved_mem.h>
#include <linux/of_irq.h>
#include <linux/of.h>
#include <linux/seq_file.h>
#include <linux/thermal.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>
#include <linux/atomic.h>
#include <linux/irq.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#define SHARE_BUF_SIZE 288
#define ADSP_IPI_STAMP_SUPPORT 0

/* for debug */
/* #define IPI_TEST */

#define IPC_NOTIFY_BY_SPIWR	1
#define IPC_NOTIFY_BY_EINT	2
#define IPC_NOTIFY_BY_POLLING	3
#define IPC_NOTIFY_BY_INT 4
#define IPC_NOTIFY_METHOD (IPC_NOTIFY_BY_INT)


/* adsp Core ID definition*/
enum adsp_core_id {
	ADSP_CORE_0_ID = 0,
	ADSP_CORE_TOTAL = 1,
};

/* adsp ipi ID definition
 * need to sync with ADSP-side
 */
enum adsp_ipi_id {
	ADSP_IPI_WDT = 0,
	ADSP_IPI_TEST1,
	ADSP_IPI_AUDIO,
	ADSP_NR_IPI,
};

enum adsp_ipi_status {
	ADSP_IPI_ERROR = -1,
	ADSP_IPI_DONE,
	ADSP_IPI_BUSY,
};

typedef void (*ipi_handler_t)(
	int id, void *data, unsigned int len
);

extern int adsp_ipi_device_init(struct platform_device *pdev);
extern int adsp_ipi_device_remove(struct platform_device *dev);

extern enum adsp_ipi_status adsp_ipi_registration(
	enum adsp_ipi_id id,
	ipi_handler_t ipi_handler,
	const char *name
);
extern enum adsp_ipi_status adsp_ipi_unregistration(enum adsp_ipi_id id);
extern enum adsp_ipi_status adsp_ipi_send(
	enum adsp_ipi_id id, void *buf,
	unsigned int len, unsigned int wait,
	enum adsp_core_id adsp_id
);

extern void adsp_ipi_handler(enum adsp_core_id core_id);

extern void adsp_ipi_status_dump(void);
extern void adsp_ipi_status_dump_id(enum adsp_ipi_id id);
extern void adsp_ipi_set_wdt_status(void);
extern unsigned int is_from_wdt;
#endif
