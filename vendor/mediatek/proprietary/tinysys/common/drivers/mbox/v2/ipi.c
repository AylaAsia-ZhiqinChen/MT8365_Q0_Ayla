/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly
 * prohibited.
 */
/* MediaTek Inc. (C) 2019. All rights reserved.
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
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY
 * ACKNOWLEDGES THAT IT IS RECEIVER\'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY
 * THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK
 * SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO
 * RECEIVER\'S SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN
 * FORUM. RECEIVER\'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK\'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER
 * WILL BE, AT MEDIATEK\'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE
 * AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
 * RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation
 * ("MediaTek Software") have been modified by MediaTek Inc. All revisions are
 * subject to any receiver\'s applicable license agreements with MediaTek Inc.
 */

#include <stdio.h>
#include <string.h>
#include <driver_api.h>
#include "FreeRTOS.h"
#include "ipi_id.h"
#include "ipi.h"


static void ipi_isr_cb(int ipi_id);

/*
 * the function should be implemented by tinysys porting layer
 */
void ipi_notify_receiver(int ipi_id);
void ipi_mdelay(unsigned long ms);

#ifdef IPI_MONITOR
unsigned long long ipi_get_ts(void);

struct ipi_monitor {
	unsigned int has_register:1,	/* 0: ipi unregistered, otherwise 1 */
			state:3,	/* transmission stage for t0~t5 */
			seqno:28;	/* sequence count of the IPI pin used */
	unsigned long long t0;
	unsigned long long t1;
	unsigned long long t2;
	unsigned long long t3;
	unsigned long long t4;
	unsigned long long t5;
};
static struct ipi_monitor ipimon[IPI_COUNT];
static int ipi_lastone;
#endif

SemaphoreHandle_t mutex_ipi_reg;
ipi_pin_t ipi_pin[IPI_COUNT];

void ipi_init(void)
{
	unsigned int i;

	mutex_ipi_reg = xSemaphoreCreateMutex();

	for (i = 0; i < TOTAL_SEND_PIN; i++) {
		mbox_pin_send_table[i].mutex = xSemaphoreCreateMutex();
		mbox_pin_send_table[i].sema_ack = xSemaphoreCreateBinary();	// TODO: remove ~?
		ipi_pin[mbox_pin_send_table[i].ipi_id].s_pin =
		    &mbox_pin_send_table[i];
	}

	for (i = 0; i < TOTAL_RECV_PIN; i++) {
		mbox_pin_recv_table[i].notify = xSemaphoreCreateBinary();
		ipi_pin[mbox_pin_recv_table[i].ipi_id].r_pin =
		    &mbox_pin_recv_table[i];
	}

	mbox_isr_hook(ipi_isr_cb);

#ifdef IPI_MONITOR
	for (i = 0; i < IPI_COUNT; i++) {
		ipimon[i].has_register = 1;
	}
#endif
	// TODO:  gen_table_tag();
}

int ipi_register(int ipi_id, void *cb, void *prdata, void *msg)
{
	struct pin_recv *pin = NULL;

	if ((ipi_id < 0) || (ipi_id >= (int) IPI_COUNT))
		return IPI_ILLEGAL;

	if (msg == NULL)
		return IPI_NO_MSGBUF;

	pin = ipi_pin[ipi_id].r_pin;
	if (pin == NULL)
		return IPI_UNAVAILABLE;

	xSemaphoreTake(mutex_ipi_reg, portMAX_DELAY);

	if (pin->pin_buf != NULL) {
		xSemaphoreGive(mutex_ipi_reg);
		return IPI_DUPLEX;
	}
	pin->mbox_pin_cb = cb;
	pin->pin_buf = msg;
	pin->prdata = prdata;

#ifdef IPI_MONITOR
	ipimon[ipi_id].has_register = 1;
	ipimon[ipi_id].seqno = 0;
	ipimon[ipi_id].state = 0;
#endif

	xSemaphoreGive(mutex_ipi_reg);

	return IPI_ACTION_DONE;
}

static int _mbox_send(struct pin_send *pin, void *data,
		      unsigned int slot_len, unsigned long retry)
{
	int ret;

	if (slot_len > pin->msg_size)
		return IPI_NO_MEMORY;
	else if (slot_len == 0)
		slot_len = pin->msg_size;

	/* Confirm this mbox pin is vacanted before send */
	taskENTER_CRITICAL();
	ret = mbox_check_send_irq(pin->mbox, pin->pin_index);
	taskEXIT_CRITICAL();

	while (retry > 0 && ret != 0) {
		ipi_mdelay(1);
		retry--;
		taskENTER_CRITICAL();
		ret = mbox_check_send_irq(pin->mbox, pin->pin_index);
		taskEXIT_CRITICAL();
	}

	if (ret != 0)
		return IPI_PIN_BUSY;

	// TODO: clz before write

	taskENTER_CRITICAL();
	ret = mbox_write(pin->mbox, pin->offset, data, slot_len * MBOX_SLOT_SIZE);
	taskEXIT_CRITICAL();

	if (ret != MBOX_DONE)
		return IPI_MBOX_ERR;

	ipi_notify_receiver(pin->ipi_id);

	taskENTER_CRITICAL();
	ret = mbox_trigger_irq(pin->mbox, 0x1 << pin->pin_index);
	taskEXIT_CRITICAL();

	if (ret != MBOX_DONE)
		return IPI_MBOX_ERR;

	return IPI_ACTION_DONE;
}

static int _ipi_send(struct pin_send *pin, void *data, int len,
			unsigned long retry)
{
	int ret;

	xSemaphoreTake(pin->mutex, portMAX_DELAY);

	ret = _mbox_send(pin, data, len, retry);

#ifdef IPI_MONITOR
	if (ret == IPI_ACTION_DONE) {
		ipimon[pin->ipi_id].seqno++;
		ipimon[pin->ipi_id].t0 = ipi_get_ts();
		ipimon[pin->ipi_id].t4 = 0;
		ipimon[pin->ipi_id].t5 = 0;
		ipimon[pin->ipi_id].state = 0;
	}
#endif

	return ret;
}

static int _ipi_recv(int ipi_id, unsigned long timeout, enum MBOX_RECV_OPT opt)
{
	struct pin_recv *pin = NULL;
	int ret;
#ifdef _BUFFERABLE_IPI
	void *user_data;
#endif

	pin = ipi_pin[ipi_id].r_pin;
	if (pin == NULL)
		return IPI_UNAVAILABLE;

	/* receive the ipi from ISR */
	ret = xSemaphoreTake(pin->notify,
			   (portTickType) (timeout * portTICK_PERIOD_MS));

	if (ret == pdFALSE)
		return IPI_RECV_TIMEOUT;
	else {
		if (opt == MBOX_RECV) {
#ifdef IPI_MONITOR
			ipimon[ipi_id].t2 = ipi_get_ts();
			ipimon[ipi_id].state = 2;
#endif
			/* do user callback */
			if (pin->cb_ctx_opt == MBOX_CB_IN_TASK) {
				pin->mbox_pin_cb(ipi_id, pin->prdata, pin->pin_buf, pin->msg_size);
			}
		}
	}

	return IPI_ACTION_DONE;
}

int ipi_send(int ipi_id, void *data, int len, unsigned long retry_timeout)
{
	struct pin_send *pin;
	int ret;

	pin = ipi_pin[ipi_id].s_pin;
	if (pin == NULL)
		return IPI_UNAVAILABLE;

	ret = _ipi_send(pin, data, len, retry_timeout);

	xSemaphoreGive(pin->mutex);

	return ret;
}

int ipi_recv(int ipi_id)
{
	return _ipi_recv(ipi_id, portMAX_DELAY, MBOX_RECV);
}

int ipi_send_compl(int ipi_id, void *data, int len,
		   unsigned long retry_timeout, unsigned long response_timeout)
{
	struct pin_send *pin;
	int ret;

	pin = ipi_pin[ipi_id].s_pin;
	if (pin == NULL)
		return IPI_UNAVAILABLE;

	/* send the IPI message */
	ret = _ipi_send(pin, data, len, retry_timeout);

	if (ret != IPI_ACTION_DONE) {
		xSemaphoreGive(pin->mutex);
		return ret;
	}

	/* wait for response */
	ret = _ipi_recv(ipi_id, response_timeout, MBOX_RESPONSE);
	xSemaphoreGive(pin->mutex);

#ifdef IPI_MONITOR
	if (ret == IPI_ACTION_DONE) {
		ipimon[ipi_id].t5 = ipi_get_ts();
		ipimon[ipi_id].state = 5;
		ipi_lastone = ipi_id;
	}
#endif

	return ret;
}

int ipi_recv_reply(int ipi_id, void *reply_data, int len)
{
	struct pin_send *pin;
	int ret;

	/* recvice the IPI message */
	ret = ipi_recv(ipi_id);
	if (ret != IPI_ACTION_DONE)
		return ret;

	pin = ipi_pin[ipi_id].s_pin;
	if (pin == NULL)
		return IPI_UNAVAILABLE;

	/* send the response */
	ret = _mbox_send(pin, reply_data, (reply_data == NULL) ? 0 : len, 0);

#ifdef IPI_MONITOR
	if (ret == IPI_ACTION_DONE) {
		ipimon[ipi_id].t3 = ipi_get_ts();
		ipimon[ipi_id].state = 3;
	}
#endif

	return ret;
}

static void ipi_isr_cb(int ipi_id)
{
	static BaseType_t xTaskWoken = 0;
	struct pin_recv *pin = NULL;

	pin = ipi_pin[ipi_id].r_pin;
	if (pin == NULL) {
		PRINTF_E("RECV PIN NOT DEFINED\n");
		return;
	}

	xSemaphoreGiveFromISR(pin->notify, &xTaskWoken);

#ifdef IPI_MONITOR
	if (pin->recv_opt == MBOX_RECV) {
		// recv msg
		ipimon[ipi_id].seqno++;
		ipimon[ipi_id].t1 = ipi_get_ts();
		ipimon[ipi_id].t2 = ipimon[ipi_id].t3 = 0;
		ipimon[ipi_id].state = 1;
	} else {
		// recv response
		ipimon[ipi_id].t4 = ipi_get_ts();
		ipimon[ipi_id].state = 4;
	}
#endif

	portYIELD_FROM_ISR(xTaskWoken);

	return;
}
