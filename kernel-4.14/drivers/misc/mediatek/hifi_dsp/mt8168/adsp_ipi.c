/*
 * Copyright (C) 2011-2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/module.h>       /* needed by all modules */
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/interrupt.h>
#include <mt-plat/sync_write.h>

#include "adsp_ipi.h"
#include "adsp_ipi_queue.h"
#include "adsp_ipi_platform.h"
#include "mtk_hifixdsp_common.h"

/*
 * =============================================================================
 *                     MACRO
 * =============================================================================
 */
#define PRINT_THRESHOLD 10000

/*
 * =============================================================================
 *                     struct def
 * =============================================================================
 */
struct adsp_ipi_desc {
	void (*handler)(int id, void *data, unsigned int len);
#if ADSP_IPI_STAMP_SUPPORT
#define ADSP_IPI_ID_STAMP_SIZE 5
	/* recv_flag[recv_count] recv_timestamp[Timestamp] */
	unsigned long long recv_timestamp[ADSP_IPI_ID_STAMP_SIZE];
	/* recv_flag[recv_count] recv_timestamp[Timestamp] */
	unsigned long long handler_timestamp[ADSP_IPI_ID_STAMP_SIZE];
	/* send_flag[send_count] send_timestamp[Timestamp] */
	unsigned long long send_timestamp[ADSP_IPI_ID_STAMP_SIZE];
	unsigned int recv_flag[ADSP_IPI_ID_STAMP_SIZE];
	unsigned int send_flag[ADSP_IPI_ID_STAMP_SIZE];
#endif
	unsigned int recv_count;
	unsigned int success_count;
	unsigned int busy_count;
	unsigned int error_count;
	const char *name;
};

struct adsp_share_obj {
	enum adsp_ipi_id id;
	unsigned int len;
	unsigned char reserve[8];
	unsigned char share_buf[SHARE_BUF_SIZE - 16];
};

/*
 * =============================================================================
 *                     private global members
 * =============================================================================
 */
enum adsp_ipi_id adsp_ipi_id_record;
enum adsp_ipi_id adsp_ipi_mutex_owner[ADSP_CORE_TOTAL];
enum adsp_ipi_id adsp_ipi_owner[ADSP_CORE_TOTAL];

unsigned int adsp_ipi_id_record_count;
unsigned int adsp_to_ap_ipi_count;
unsigned int ap_to_adsp_ipi_count;
unsigned int is_from_wdt;

struct adsp_ipi_desc adsp_ipi_desc[ADSP_NR_IPI];
struct adsp_share_obj *adsp_send_obj[ADSP_CORE_TOTAL];
struct adsp_share_obj *adsp_rcv_obj[ADSP_CORE_TOTAL];
struct mutex adsp_ipi_mutex[ADSP_CORE_TOTAL];

/*
 * =============================================================================
 *                     private function declaration
 * =============================================================================
 */
static enum adsp_ipi_status send_msg_by_emi(
	enum adsp_ipi_id id, void *buf,
	unsigned int  len, unsigned int wait,
	enum adsp_core_id core_id
);

static enum adsp_ipi_status adsp_ipi_send_internal(
	int id, void *buf, unsigned int  len
);


/*
 * find an ipi handler and invoke it
 */
void adsp_ipi_handler(enum adsp_core_id core_id)
{
#if ADSP_IPI_STAMP_SUPPORT
	unsigned int flag = 0;
#endif
	enum adsp_ipi_id adsp_ipi_id;

	pr_debug("[ADSP] A ipi handler, id=%d\n", core_id);

	while (is_adsp_ready(core_id) == 0) {
		pr_err("%s: %s not enabled, core_id=%d\n",
		       __func__, adsp_core_ids[core_id], core_id);
		msleep(20);
	}

	memcpy_from_adsp(core_id, adsp_rcv_obj[core_id],
		adsp_info[core_id].adsp_rcv_obj_addr,
		sizeof(struct adsp_share_obj));

	adsp_ipi_id = adsp_rcv_obj[core_id]->id;
	/*pr_debug("adsp A ipi handler %d\n", adsp_ipi_id);*/
	if (adsp_ipi_id >= ADSP_NR_IPI || adsp_ipi_id <= 0) {
		/* ipi id abnormal*/
		pr_debug("[ADSP] A ipi handler id abnormal, id=%d\n",
			adsp_ipi_id);
	} else if (adsp_ipi_desc[adsp_ipi_id].handler) {
		adsp_ipi_desc[adsp_ipi_id].recv_count++;
		adsp_to_ap_ipi_count++;
#if ADSP_IPI_STAMP_SUPPORT
		flag = adsp_ipi_desc[adsp_ipi_id].recv_count %
		       ADSP_IPI_ID_STAMP_SIZE;
		if (flag < ADSP_IPI_ID_STAMP_SIZE) {
			adsp_ipi_desc[adsp_ipi_id].recv_flag[flag] =
				adsp_ipi_desc[adsp_ipi_id].recv_count;
			adsp_ipi_desc[adsp_ipi_id].handler_timestamp[flag] = 0;
			adsp_ipi_desc[adsp_ipi_id].recv_timestamp[flag] =
				arch_counter_get_cntvct();
		}
#endif
		scp_dispatch_ipi_hanlder_to_queue(
			INTERNAL_ADSP_ID,
			adsp_ipi_id,
			adsp_rcv_obj[core_id]->share_buf,
			adsp_rcv_obj[core_id]->len,
			adsp_ipi_desc[adsp_ipi_id].handler);
#if ADSP_IPI_STAMP_SUPPORT
		if (flag < ADSP_IPI_ID_STAMP_SIZE)
			adsp_ipi_desc[adsp_ipi_id].handler_timestamp[flag] =
				arch_counter_get_cntvct();
#endif
	} else {
		/* adsp_ipi_handler is null or ipi id abnormal */
		pr_debug("[ADSP] A ipi handler is null or abnormal, id=%d\n",
			 adsp_ipi_id);
	}

	pr_debug("%s done\n", __func__);
}

/*
 * ipi initialize
 */
int adsp_ipi_init(void)
{
	int i = 0;
#if ADSP_IPI_STAMP_SUPPORT
	int j = 0;
#endif
	scp_ipi_queue_init_ex(INTERNAL_ADSP_ID, adsp_ipi_send_internal);

	for (i = 0; i < ADSP_CORE_TOTAL; i++) {
		size_t share_obj_size = sizeof(struct adsp_share_obj);

		mutex_init(&adsp_ipi_mutex[i]);
		adsp_rcv_obj[i] = kmalloc(share_obj_size, GFP_KERNEL);
		adsp_send_obj[i] = kmalloc(share_obj_size, GFP_KERNEL);
		pr_debug("adsp_rcv_obj[%d] = 0x%p\n", i, adsp_rcv_obj[i]);
		pr_debug("adsp_send_obj[%d] = 0x%p\n", i, adsp_send_obj[i]);
		memset(adsp_send_obj[i], 0, share_obj_size);
		memset(adsp_rcv_obj[i], 0, share_obj_size);
	}

	adsp_to_ap_ipi_count = 0;
	ap_to_adsp_ipi_count = 0;

	for (i = 0; i < ADSP_NR_IPI; i++) {
		adsp_ipi_desc[i].recv_count      = 0;
		adsp_ipi_desc[i].success_count  = 0;
		adsp_ipi_desc[i].busy_count      = 0;
		adsp_ipi_desc[i].error_count    = 0;
#if ADSP_IPI_STAMP_SUPPORT
		for (j = 0; j < ADSP_IPI_ID_STAMP_SIZE; j++) {
			adsp_ipi_desc[i].recv_timestamp[j] = 0;
			adsp_ipi_desc[i].send_timestamp[j] = 0;
			adsp_ipi_desc[i].recv_flag[j] = 0;
			adsp_ipi_desc[i].send_flag[j] = 0;
			adsp_ipi_desc[i].handler_timestamp[j] = 0;
		}
#endif
	}

	return 0;
}

/*
 * API let apps can register an ipi handler to receive IPI
 * @param id:      IPI ID
 * @param handler:  IPI handler
 * @param name:  IPI name
 */
enum adsp_ipi_status adsp_ipi_registration(
	enum adsp_ipi_id id,
	ipi_handler_t ipi_handler,
	const char *name)
{
	if (id < ADSP_NR_IPI) {
		adsp_ipi_desc[id].name = name;

		if (ipi_handler == NULL)
			return ADSP_IPI_ERROR;

		adsp_ipi_desc[id].handler = ipi_handler;
		return ADSP_IPI_DONE;
	} else
		return ADSP_IPI_ERROR;
}
EXPORT_SYMBOL_GPL(adsp_ipi_registration);

/*
 * API let apps unregister an ipi handler
 * @param id:      IPI ID
 */
enum adsp_ipi_status adsp_ipi_unregistration(enum adsp_ipi_id id)
{
	if (id < ADSP_NR_IPI) {
		adsp_ipi_desc[id].name = "";
		adsp_ipi_desc[id].handler = NULL;
		return ADSP_IPI_DONE;
	} else
		return ADSP_IPI_ERROR;
}
EXPORT_SYMBOL_GPL(adsp_ipi_unregistration);

/*
 * API for apps to send an IPI to adsp
 * @param id:   IPI ID
 * @param buf:  the pointer of data
 * @param len:  data length
 * @param wait: If true, wait (atomically) until data have been gotten by Host
 * @param len:  data length
 */
enum adsp_ipi_status adsp_ipi_send(enum adsp_ipi_id id, void *buf,
				   unsigned int  len, unsigned int wait,
				   enum adsp_core_id core_id)
{
	int retval = 0;

	/* wait at most 5 ms until IPC done */
	if (core_id == ADSP_CORE_0_ID) {
		retval = scp_send_msg_to_queue(
			INTERNAL_ADSP_ID, id, buf, len,
			(wait) ? 5 : 0);
	} else {
		pr_err("Invalid adsp core id %d\n", core_id);
		retval = ADSP_IPI_ERROR;
	}

	return (retval == 0) ? ADSP_IPI_DONE : ADSP_IPI_ERROR;
}
EXPORT_SYMBOL_GPL(adsp_ipi_send);

static enum adsp_ipi_status send_msg_by_emi(enum adsp_ipi_id id, void *buf,
				       unsigned int  len, unsigned int wait,
				       enum adsp_core_id core_id)
{

#if ADSP_IPI_STAMP_SUPPORT
	unsigned long flag = 0;
#endif

	/*avoid adsp log print too much*/
	if (adsp_ipi_id_record == id)
		adsp_ipi_id_record_count++;
	else
		adsp_ipi_id_record_count = 0;

	adsp_ipi_id_record = id;

	if (in_interrupt()) {
		if (wait) {
			pr_err("adsp_ipi_send: cannot use in isr\n");
			adsp_ipi_desc[id].error_count++;
			return ADSP_IPI_ERROR;
		}
	}

	if (is_adsp_ready(core_id) == 0) {
		pr_err("adsp_ipi_send: %s not enabled, id=%d\n",
		       adsp_core_ids[core_id], id);
		adsp_ipi_desc[id].error_count++;
		return ADSP_IPI_ERROR;
	}
	if (len > sizeof(adsp_send_obj[core_id]->share_buf) || buf == NULL) {
		pr_err("adsp_ipi_send: %s buffer error\n",
		       adsp_core_ids[core_id]);
		adsp_ipi_desc[id].error_count++;
		return ADSP_IPI_ERROR;
	}

	if (mutex_trylock(&adsp_ipi_mutex[core_id]) == 0) {
		/*avoid adsp ipi send log print too much*/
		if ((adsp_ipi_id_record_count % PRINT_THRESHOLD == 0) ||
		    (adsp_ipi_id_record_count % PRINT_THRESHOLD == 1)) {
			pr_err("adsp_ipi_send:%s %d mutex_trylock busy,owner=%d\n",
			       adsp_core_ids[core_id], id,
			       adsp_ipi_mutex_owner[core_id]);
		}
		adsp_ipi_desc[id].busy_count++;
		return ADSP_IPI_BUSY;
	}

	/*get adsp ipi mutex owner*/
	adsp_ipi_mutex_owner[core_id] = id;

	if (is_ipi_busy(core_id)) {
		/*avoid adsp ipi send log print too much*/
		if ((adsp_ipi_id_record_count % PRINT_THRESHOLD == 0) ||
		    (adsp_ipi_id_record_count % PRINT_THRESHOLD == 1)) {
			pr_err("adsp_ipi_send: %s %d host to adsp busy, ipi last time = %d\n",
			       adsp_core_ids[core_id], id,
			       adsp_ipi_owner[core_id]);		}

		adsp_ipi_desc[id].busy_count++;
		mutex_unlock(&adsp_ipi_mutex[core_id]);
		return ADSP_IPI_BUSY;
	}
	/*get adsp ipi send owner*/
	adsp_ipi_owner[core_id] = id;

	adsp_send_obj[core_id]->id = id;
	adsp_send_obj[core_id]->len = len;
	memcpy(adsp_send_obj[core_id]->share_buf, buf, len);
	memcpy_to_adsp(core_id, adsp_info[core_id].adsp_send_obj_addr,
		       adsp_send_obj[core_id], sizeof(struct adsp_share_obj));

	dsb(SY);
	/*record timestamp*/
	adsp_ipi_desc[id].success_count++;
	ap_to_adsp_ipi_count++;

#if ADSP_IPI_STAMP_SUPPORT
	flag = adsp_ipi_desc[id].success_count % ADSP_IPI_ID_STAMP_SIZE;
	if (flag < ADSP_IPI_ID_STAMP_SIZE) {
		adsp_ipi_desc[id].send_flag[flag] =
			adsp_ipi_desc[id].success_count;
		adsp_ipi_desc[id].send_timestamp[flag] =
			arch_counter_get_cntvct();
	}
#endif

	if (wait)
		while (is_ipi_busy(core_id))
			;

	/*send host to adsp ipi cpmplete, unlock mutex*/
	mutex_unlock(&adsp_ipi_mutex[core_id]);

	return ADSP_IPI_DONE;
}

static enum adsp_ipi_status adsp_ipi_send_internal(
	int id, void *buf, unsigned int  len
)
{
	return send_msg_by_emi(id, buf, len, 1, ADSP_CORE_0_ID);
}


void adsp_ipi_info_dump(enum adsp_ipi_id ipi_id)
{
#if ADSP_IPI_STAMP_SUPPORT
	int i = 0;
#endif

	pr_debug("%u\t%u\t%u\t%u\t%u\t%s\n\r",
		 ipi_id,
		 adsp_ipi_desc[ipi_id].recv_count,
		 adsp_ipi_desc[ipi_id].success_count,
		 adsp_ipi_desc[ipi_id].busy_count,
		 adsp_ipi_desc[ipi_id].error_count,
		 adsp_ipi_desc[ipi_id].name);
#if ADSP_IPI_STAMP_SUPPORT
	/*time stamp*/
	for (i = 0; i < ADSP_IPI_ID_STAMP_SIZE; i++) {
		if (adsp_ipi_desc[ipi_id].recv_timestamp[i] != 0) {
			pr_debug("adsp->ap recv count:%u, recv time:%llu handler finish time:%llu\n",
				 adsp_ipi_desc[ipi_id].recv_flag[i],
				 adsp_ipi_desc[ipi_id].recv_timestamp[i],
				 adsp_ipi_desc[ipi_id].handler_timestamp[i]);
		}
	}
	for (i = 0; i < ADSP_IPI_ID_STAMP_SIZE; i++) {
		if (adsp_ipi_desc[ipi_id].send_timestamp[i] != 0) {
			pr_debug("ap->adsp send count:%u send time:%llu\n",
				 adsp_ipi_desc[ipi_id].send_flag[i],
				 adsp_ipi_desc[ipi_id].send_timestamp[i]);
		}
	}
#endif

}

void adsp_ipi_status_dump_id(enum adsp_ipi_id ipi_id)
{
	pr_debug("[ADSP]ipi_id\trecv\tsuccess\tbusy\terror\tname\n\r");
	adsp_ipi_info_dump(ipi_id);

}


void adsp_ipi_status_dump(void)
{
	enum adsp_ipi_id ipi_id;

	pr_debug("[ADSP]ipi_id\trecv\tsuccess\tbusy\terror\tname\n\r");
	for (ipi_id = 0; ipi_id < ADSP_NR_IPI; ipi_id++) {
		if (adsp_ipi_desc[ipi_id].recv_count > 0    ||
		    adsp_ipi_desc[ipi_id].success_count > 0 ||
		    adsp_ipi_desc[ipi_id].busy_count > 0    ||
		    adsp_ipi_desc[ipi_id].error_count > 0)
			adsp_ipi_info_dump(ipi_id);
	}
	pr_debug("ap->adsp total=%u adsp->ap total=%u\n\r",
		 ap_to_adsp_ipi_count, adsp_to_ap_ipi_count);
}

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

#ifdef IPI_TEST
#define TEST_BUFFER_LEN 272
static int check_test_buffer(char *buffer, int len, unsigned int excepted)
{
	int i = 0;

	for (i = 0; i < len; i += 4) {
		if (*(unsigned int *)(buffer+i) != excepted)
			return 0;
		excepted += 1;
	}
	return 1;
}

static void fill_test_buffer(char *buffer, int len, unsigned int val)
{
	int i = 0;

	for (i = 0; i < len; i += 4) {
		*(unsigned int *)(buffer+i) = val;
		val += 1;
	}
}

static void adsp_ipi_debug(
	int ipi_id, void *data, unsigned int len
)
{
	char test_buffer[TEST_BUFFER_LEN] = {0};
	static unsigned int excepted = 1;

	if (!check_test_buffer(data, len, excepted))
		pr_err("adsp get unexcepted data %d,excepted %d\n",
			*(unsigned int *)data, excepted);
	else
		pr_err("adsp get excepted data %d\n", excepted);
	excepted += 1;
	fill_test_buffer(test_buffer, TEST_BUFFER_LEN, excepted);
	adsp_ipi_send(ADSP_IPI_TEST1, test_buffer,
		TEST_BUFFER_LEN, 0, ADSP_CORE_0_ID);
	excepted += 1;
}

static int audio_ipi_test_thread(void *data)
{
	unsigned int begin = 0;
	char test_buffer[TEST_BUFFER_LEN] = {0};

	fill_test_buffer(test_buffer, TEST_BUFFER_LEN, begin);
	adsp_ipi_registration(ADSP_IPI_TEST1, adsp_ipi_debug, "IPIDebug");
	msleep(45000);
	pr_err("audio ipi test\n");
	adsp_ipi_send(ADSP_IPI_TEST1, test_buffer,
		TEST_BUFFER_LEN, 1, ADSP_CORE_0_ID);

#if IPC_NOTIFY_METHOD == IPC_NOTIFY_BY_POLLING
	msleep(2000);
	pr_err("ipc polling thread\n");
	while (1) {
		u32 host_status = get_adsp_to_host_status(ADSP_CORE_0_ID);

		if (!(host_status&IPC_MESSAGE_READY))
			continue;
		pr_err("recived msg\n");
		adsp_ipi_handler(ADSP_CORE_0_ID);
	}
#endif
	/*
	 * bootup mission is now accomplished,
	 * the kthead will end self-life.
	 */
	return 0;
}
#endif

void adsp_ipi_set_wdt_status(void)
{
	is_from_wdt = 0;
	pr_info("adsp_ipi set dsp wdt status flase!\n");
}

static int
adsp_ipi_get_wdt_status(struct notifier_block *this,
				 unsigned long code, void *unused)
{
	is_from_wdt = 1;
	pr_info("adsp_ipi get dsp wdt!\n");
	return NOTIFY_DONE;
}

static struct notifier_block get_wdt_status = {
	.notifier_call  = adsp_ipi_get_wdt_status,
	.priority       = 101,
};

int adsp_ipi_device_init(struct platform_device *pdev)
{
#ifdef CONFIG_OF
	int irq_num;

	pr_err("%s", __func__);

	irq_num = platform_get_irq(pdev, 0);
	if (irq_num == -ENXIO) {
		pr_err("Fail to get audio ipi irq number from device tree\n");
		return -EINVAL;
	}
	pr_notice("%s irq num %d.\n", __func__, irq_num);

	/* IRQF_TRIGGER_RISING/IRQF_TRIGGER_FALLING
	 * IRQF_TRIGGER_HIGH/IRQF_TRIGGER_LOW
	 */
	if (request_irq(irq_num, adsp_ipi_irq_handler,
		IRQF_TRIGGER_NONE, "AUDIO IPI", NULL) != 0) {
		pr_err("Fail to request audio ipi irq interrupt!\n");
		return -1;
	}
	enable_irq_wake(irq_num);
#endif

	adsp_ipi_platform_init(pdev);

	adsp_ipi_init();

	pr_err("%s finish", __func__);

#ifdef IPI_TEST
	kthread_run(audio_ipi_test_thread, NULL, "audio ipi test");
#endif
	register_adsp_wdt_notifier(&get_wdt_status);
	return 0;
}

int adsp_ipi_device_remove(struct platform_device *dev)
{
	pr_err("%s", __func__);
	unregister_adsp_wdt_notifier(&get_wdt_status);
	return 0;
}

