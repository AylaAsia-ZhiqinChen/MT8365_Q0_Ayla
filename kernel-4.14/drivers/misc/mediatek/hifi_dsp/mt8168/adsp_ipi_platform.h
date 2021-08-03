/*
 * Copyright (C) 2017 MediaTek Inc.
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

#include "adsp_ipi.h"

#define INTERNAL_ADSP_ID (0x81688168)
#define ADSP_COMMON_INFO_MAGIC 0x1D901D90

void memcpy_from_adsp(enum adsp_core_id core_id, void *trg, u32 src, int size);
void memcpy_to_adsp(enum adsp_core_id core_id, u32 trg, void *src, int size);
unsigned int is_adsp_ready(enum adsp_core_id core_id);
unsigned int is_ipi_busy(enum adsp_core_id core_id);
u32 get_adsp_to_host_status(enum adsp_core_id core_id);
irqreturn_t adsp_ipi_irq_handler(int irq, void *dev_id);
int adsp_ipi_platform_init(struct platform_device *pdev);
extern unsigned long adsp_get_shared_dtcm_virt_for_ipc(void);

/* remote adsp info, need access by spi*/
#define IPC_MESSAGE_READY (1<<0)

struct adsp_common_info {
	u32 magic;
	u32 status;
	u32 adsp_send_obj_addr;
	u32 adsp_rcv_obj_addr;
	u32 adsp_to_host_status;
	u32 host_to_adsp_status;
};

extern struct adsp_common_info adsp_info[ADSP_CORE_TOTAL];
extern char *adsp_core_ids[ADSP_CORE_TOTAL];
