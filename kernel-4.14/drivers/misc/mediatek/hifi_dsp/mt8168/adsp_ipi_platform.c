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
#include <linux/stddef.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pm_runtime.h>
#include <linux/pm_wakeup.h>
#include <linux/spinlock.h>
#include "adsp_ipi.h"
#include "adsp_ipi_queue.h"
#include "adsp_ipi_platform.h"
#include "mtk_hifixdsp_common.h"
#include "adsp_reg.h"

char *adsp_core_ids[ADSP_CORE_TOTAL] = {"ADSP CORE 0"};
struct adsp_common_info adsp_info[ADSP_CORE_TOTAL];
static u64 adsp_info_addr[ADSP_CORE_TOTAL];
static int adsp_is_ready[ADSP_CORE_TOTAL];
static struct delayed_work ipi_delayed_work;
static DEFINE_SPINLOCK(adsp_ipi_isr_mutex);


/*
 * trg : target address, host address
 * src : source address, adsp address
 */
void memcpy_from_adsp_by_offset(enum adsp_core_id core_id,
				void *trg, u32 src, int size)
{
	//src is the buffer offset from adsp_info_addr[core_id]
	void *src_addr = (void *)(unsigned long)(adsp_info_addr[core_id] + src);

	memcpy(trg, src_addr, size);
}

/*
 * trg : target address, adsp address
 * src : source address, host address
 */
void memcpy_to_adsp_by_offset(enum adsp_core_id core_id,
				u32 trg, void *src, int size)
{
	//trg is the buffer offset from adsp_info_addr[core_id]
	void *trg_addr = (void *)(unsigned long)(adsp_info_addr[core_id] + trg);

	memcpy(trg_addr, src, size);
}

/*
 * shared memory layout
 * [adsp_common_info] [24 bytes]
 * [send obj(adsp view)] [288 bytes]
 * [rcv obj(adsp view)] [288 bytes]
 */
void init_adsp_common_info(enum adsp_core_id core_id)
{
	if (adsp_info_addr[core_id] == 0) {
		if (core_id == ADSP_CORE_0_ID)
			adsp_info_addr[core_id] =
				adsp_get_shared_dtcm_virt_for_ipc();
		else
			pr_err("invaild adsp_core_id %d\n", core_id);
	}
}

unsigned int update_adsp_commmon_info(enum adsp_core_id core_id)
{
	init_adsp_common_info(core_id);
	memcpy(&adsp_info[core_id],
		(void *)(unsigned long)adsp_info_addr[core_id],
		sizeof(struct adsp_common_info));
	if (adsp_info[core_id].magic != ADSP_COMMON_INFO_MAGIC) {
		pr_err("adsp %d info magic is incorrect!\n", core_id);
		return 0;
	}
	return 1;
}

u32 get_host_to_adsp_status(enum adsp_core_id core_id)
{
	u32 status;
	u32 offset = offsetof(struct adsp_common_info, host_to_adsp_status);

	status = *(u32 *)(unsigned long)(adsp_info_addr[core_id]+offset);
	return status;
}

void set_host_to_adsp_status(enum adsp_core_id core_id, u32 status)
{
	u32 offset = offsetof(struct adsp_common_info, host_to_adsp_status);

	*(u32 *)(unsigned long)(adsp_info_addr[core_id]+offset) = status;
}

u32 get_adsp_to_host_status(enum adsp_core_id core_id)
{
	u32 status;
	u32 offset = offsetof(struct adsp_common_info, adsp_to_host_status);

	status = *(u32 *)(unsigned long)(adsp_info_addr[core_id]+offset);
	pr_err("adsp core %d adsp_to_host_status is %u\n", core_id, status);
	return status;
}

void set_adsp_to_host_status(enum adsp_core_id core_id, u32 status)
{
	u32 offset = offsetof(struct adsp_common_info, adsp_to_host_status);

	*(u32 *)(unsigned long)(adsp_info_addr[core_id]+offset) = status;
}

void clr_adsp_to_host_status(enum adsp_core_id core_id, u32 status)
{
	u32 offset = offsetof(struct adsp_common_info, adsp_to_host_status);
	u32 read_val = 0;

	read_val = *(u32 *)(unsigned long)(adsp_info_addr[core_id]+offset);
	read_val = read_val & (~status);
	*(u32 *)(unsigned long)(adsp_info_addr[core_id]+offset) = read_val;
}

void memcpy_from_adsp(enum adsp_core_id core_id, void *trg, u32 src, int size)
{
	memcpy_from_adsp_by_offset(core_id, trg, src, size);
	clr_adsp_to_host_status(core_id, IPC_MESSAGE_READY);
}

static void trigger_host_to_adsp_intertupt(void)
{
	unsigned long flags;

	spin_lock_irqsave(&adsp_ipi_isr_mutex, flags);
	hw_semaphore_get(ADSP_HW_SEMA_IPI, 0);
	*(u32 *)DSP_RG_INT2CIRQ |= (1 << 0); /* trigger CPU->DSP int */
	hw_semaphore_release(ADSP_HW_SEMA_IPI);
	spin_unlock_irqrestore(&adsp_ipi_isr_mutex, flags);
}

void memcpy_to_adsp(enum adsp_core_id core_id, u32 trg, void *src, int size)
{
	memcpy_to_adsp_by_offset(core_id, trg, src, size);
	set_host_to_adsp_status(core_id, IPC_MESSAGE_READY);

	/*
	 *trigger interrupt to notify adsp receive message
	 */
	trigger_host_to_adsp_intertupt();
}

#define IPC_RETRY_LIMIT 5
unsigned int is_ipi_busy(enum adsp_core_id core_id)
{
	u32 status;
	static int retry_cnt;

	status = get_host_to_adsp_status(core_id);
	if (status & IPC_MESSAGE_READY) {
		/*
		 * ipi busy may because interrupt to beethoven is lost
		 * try to send intterupt again in eint case
		 */
		if (retry_cnt >= 0)
			retry_cnt++;
		if (retry_cnt % 5 == 0 && retry_cnt / 5 <= IPC_RETRY_LIMIT) {
			pr_err("interrupt may be lost!\n");
			trigger_host_to_adsp_intertupt();
		}
		if (retry_cnt / 5 > IPC_RETRY_LIMIT)
			retry_cnt = -1;
		udelay(1000);
	} else {
		retry_cnt = 0;
	}
	return (status&IPC_MESSAGE_READY);
}

unsigned int is_adsp_ready(enum adsp_core_id core_id)
{
	if (adsp_is_ready[core_id])
		return 1;

	return adsp_is_ready[core_id] = update_adsp_commmon_info(core_id);
}

/*
 * dispatch adsp irq
 * reset adsp and generate exception if needed
 * @param irq:      irq id
 * @param dev_id:   should be NULL
 */
static struct workqueue_struct *ipi_queue;
static struct work_struct ipi_work;

void adsp_ipi_work_handler(struct work_struct *unused)
{
	adsp_ipi_handler(ADSP_CORE_0_ID);
}
irqreturn_t adsp_ipi_irq_handler(int irq, void *dev_id)
{
	if (is_from_suspend) {
		pr_notice("resuming flow , queue delayed work!\n");
		queue_delayed_work(ipi_queue,
			&ipi_delayed_work,
			msecs_to_jiffies(50));
		is_from_suspend = 0;
	} else {
		queue_work(ipi_queue, &ipi_work);
	}
	spin_lock(&adsp_ipi_isr_mutex);
	hw_semaphore_get(ADSP_HW_SEMA_IPI, 0);
	*(u32 *)DSP_RG_INT2CIRQ |= (1 << 2); /* clear DSP->CPU wakeup int */
	*(u32 *)DSP_RG_INT2CIRQ &= ~(1 << 1); /* clear DSP->CPU int */
	hw_semaphore_release(ADSP_HW_SEMA_IPI);
	spin_unlock(&adsp_ipi_isr_mutex);
	return IRQ_HANDLED;
}

int adsp_ipi_platform_init(struct platform_device *pdev)
{
	spin_lock_init(&adsp_ipi_isr_mutex);
	ipi_queue = create_singlethread_workqueue("ipi_kworker");
	INIT_WORK(&ipi_work, adsp_ipi_work_handler);
	INIT_DELAYED_WORK(&ipi_delayed_work, adsp_ipi_work_handler);

	return 0;
}


