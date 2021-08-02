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

#include "FreeRTOS.h"
#include "task.h"
#include "zerocpy_ipc_plat.h"
#include <tinysys_reg.h>
#include <driver_api.h>
#include <mt_printf.h>
#include <irq.h>
#include <intc.h>
#include <encoding.h>

#define CFG_C2C_IPC_DEBUG_WB	0
#define CFG_C2C_IPC_DEBUG_RD	0
#define CFG_C2C_IPC_DEBUG_BU	0

#if CFG_C2C_IPC_DEBUG_WB
#define C2C_IPC_DEBUG_WB(x...) printf(x)
#else
#define C2C_IPC_DEBUG_WB(x...)
#endif
#if CFG_C2C_IPC_DEBUG_RD
#define C2C_IPC_DEBUG_RD(x...) printf(x)
#else
#define C2C_IPC_DEBUG_RD(x...)
#endif

#if CFG_C2C_IPC_DEBUG_BU
#define C2C_IPC_DEBUG_BU(x...) printf(x)
#else
#define C2C_IPC_DEBUG_BU(x...)
#endif

SYNC_ATOMIC_BITSET_DECL(buff_in_use_bitset, BUFF_IN_USE_SIZE);
void *buff_in_use_bitmap[BUFF_IN_USE_SIZE] __attribute__ ((section(".sync")));;

int cb_is_full(z_rbuffer * cb, uint32_t buff_size);
int cb_is_empty(z_rbuffer * cb);
int cb_incr(uint32_t p, uint32_t buff_size);
int cb_write(z_rbuffer * cb, uint32_t buff_size, uint32_t elem);
int cb_read(z_rbuffer * cb, uint32_t buff_size, uint32_t * elem);

int cb_is_full(z_rbuffer * cb, uint32_t buff_size)
{
	return cb->wb == (cb->rd ^ buff_size);	// This inverts the most significant bit of rd before comparison
}

int cb_is_empty(z_rbuffer * cb)
{
	return cb->wb == cb->rd;
}

int cb_incr(uint32_t p, uint32_t buff_size)
{
	return (p + 1) & (2 * buff_size - 1);	// rd and wb pointers incrementation is done modulo 2*size
}

int cb_write(z_rbuffer * cb, uint32_t buff_size, uint32_t elem)
{
	if (cb_is_full(cb, buff_size))	// full, skip
		return pdFALSE;
	cb->elems[cb->wb & (buff_size - 1)] = elem;
	C2C_IPC_DEBUG_WB("cb_write elems[%d] = %x\n",
			 cb->wb & (buff_size - 1), elem);
	cb->wb = cb_incr(cb->wb, buff_size);
	SYNCD();
	return pdTRUE;
}

int cb_read(z_rbuffer * cb, uint32_t buff_size, uint32_t * elem)
{
	if (cb_is_empty(cb)) {	// empty, skip
		C2C_IPC_DEBUG_RD
		    ("cb_is_empty cb->wb(%p) = %d cb->rd(%p) = %d\n", &(cb->wb),
		     cb->wb, &(cb->rd), cb->rd);
		return pdFALSE;
	}

	*elem = cb->elems[cb->rd & (buff_size - 1)];

	C2C_IPC_DEBUG_RD("cb_read elems[%d] = %x\n",
			 cb->rd & (buff_size - 1), *elem);
	cb->rd = cb_incr(cb->rd, buff_size);

	SYNCD();

	C2C_IPC_DEBUG_RD("cb->rd(%p) = %d\n", &(cb->rd), cb->rd);
	return pdTRUE;
}

/*
	return fail when resource not avaliable
*/
int zerocpy_ipc_send(ipc_chan_st * icc, int id, void *buff_ptr)
{
	int ret = pdFALSE;
	int wb_id;

	if (id >= icc->zerocpy_ids)
		return pdFALSE;

	wb_id = c2c_get_plat_wb_id(IPC_SENDER);

	taskENTER_CRITICAL();
	/* write id ring buffer first */
	if (icc->id_info[id].rbuff_size) /* if ID config with buffer_ptr ring buffer */
		if (cb_write(icc->id_info[id].rbuff[wb_id],
			icc->id_info[id].rbuff_size,
			(uint32_t) buff_ptr) == pdFALSE) {
			PRINTF_E("[id:%d].buff full\n", id);
			goto _zerocpy_ipc_send_end;
		}
	if (cb_write(icc->rbuff[wb_id], icc->rbuff_size, (uint32_t) id) ==
	    pdFALSE) {
		PRINTF_E("cb_write full\n");
		goto _zerocpy_ipc_send_end;

	}

	icc->zerocpy_ipc_reg_set();
	ret = pdTRUE;
_zerocpy_ipc_send_end:
	taskEXIT_CRITICAL();
	return ret;
}

void zerocpy_ipc_register(ipc_chan_st * icc, int id,
			  zerocpy_ipc_handler_t handler)
{
	int wb_id;

	wb_id = c2c_get_plat_wb_id(IPC_RECEIVER);
	if (id < C2C_LAST_ID) {
		icc->id_info[id].handler[wb_id] = handler;
		PRINTF_E("%s[%d]=%p\n", __func__, id, (void *) handler);
	} else
		PRINTF_E("%s[%d] fail\n", __func__, id);
}

uint32_t buff_in_use(void *ptr, uint32_t use)
{
	int bit;

	if (use) {
		bit = atomicbit_find_clear_and_set(buff_in_use_bitset);
		if (bit >= 0) {
			//get unique bit, then fill buf ptr
			buff_in_use_bitmap[bit] = ptr;
			SYNCD();
			C2C_IPC_DEBUG_BU("buff_in_use %p @ %d\n", ptr, bit);
		} else {
			PRINTF_E("buff_in_use full\n");
			return FALSE;
		}
	} else {		/* clear use */
		int found_bit = -1;

		for (bit = 0; bit < BUFF_IN_USE_SIZE; bit++) {
			if (buff_in_use_bitmap[bit] == ptr) {
				found_bit = bit;
				break;
			}
		}
		if (found_bit >= 0) {
			buff_in_use_bitmap[found_bit] = 0;
			atomicbit_clearbit(buff_in_use_bitset, found_bit);	/* must after clear bitmap */
		} else {
			PRINTF_E("buff_in_use %p not found\n", ptr);
			return FALSE;
		}
	}
	return TRUE;
}

uint32_t is_buff_in_use(void *ptr)
{
	int bit;

	for (bit = 0; bit < BUFF_IN_USE_SIZE; bit++) {
		if (buff_in_use_bitmap[bit] == ptr) {	/* */
			if (atomicbit_getbit(buff_in_use_bitset, bit) == TRUE)
				return TRUE;
		}
	}
	return FALSE;
}

void buff_in_use_initial(void)
{
	atomicbit_init(buff_in_use_bitset, BUFF_IN_USE_SIZE);
	memset(buff_in_use_bitmap, 0, sizeof (uint32_t) * BUFF_IN_USE_SIZE);
	SYNCD();
}

void *zerocpy_ipc_get_buff_ptr(ipc_chan_st * icc, int id)
{
	int wb_id;
	uint32_t buff = 0;
	wb_id = c2c_get_plat_wb_id(IPC_RECEIVER);
	if (icc->id_info[id].rbuff_size != 0)
		if (cb_read(icc->id_info[id].rbuff[wb_id],
			icc->id_info[id].rbuff_size,
			&buff) == pdFALSE) {
			C2C_IPC_DEBUG_RD("id:%d buff_ptr empty\n", id);

		}
	return (void *) buff;
}

void zerocpy_ipc_receive(ipc_chan_st * icc, int id)
{
	int wb_id;
	wb_id = c2c_get_plat_wb_id(IPC_RECEIVER);
	if (icc->id_info[id].task == NULL)
		icc->id_info[id].task = xTaskGetCurrentTaskHandle();
	taskENTER_CRITICAL();
	if (cb_is_empty(icc->id_info[id].rbuff[wb_id])) {
		vTaskSuspend(NULL);
		portYIELD_WITHIN_API();
		C2C_IPC_DEBUG_RD("zerocpy_ipc_receive suspend\n");
	} else {
		C2C_IPC_DEBUG_RD("zerocpy_ipc_receive with data\n");
	}
	taskEXIT_CRITICAL();
}

void c2c_zcpy_isr(ipc_chan_st * icc)
{
	int wb_id;
	uint32_t id;
	zerocpy_ipc_handler_t handler;

	wb_id = c2c_get_plat_wb_id(IPC_RECEIVER);
	icc->zerocpy_ipc_reg_clr();
	while (1) {
		if (cb_read(icc->rbuff[wb_id], icc->rbuff_size, &id) == pdFALSE) {
			C2C_IPC_DEBUG_RD("cb_read id empty\n");
			break;
		}
		handler = icc->id_info[id].handler[wb_id];
		if (handler != NULL)
			handler(id);
		else {
			TaskHandle_t task = icc->id_info[id].task;

			if (task) {
				xTaskResumeFromISR(task);
				portYIELD_WITHIN_API();
			}
		}
	}
}
