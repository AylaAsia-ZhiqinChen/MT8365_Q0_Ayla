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
#include "mbox_platform.h"
#include "mbox_common_type.h"
#include "mbox_common.h"
#include <driver_api.h>
#include "FreeRTOS.h"
#include <mt_printf.h>


/*
 * function for platform implementation
 */
unsigned int find_mbox_num(unsigned int irq_num);
int set_mbox_64d(unsigned int mbox, unsigned int is64d, void *data);
int register_irq_handler(unsigned int (*mbox_isr) (void *data), void *prdata);
void mbox_isr_pre_cb(void);
void mbox_isr_post_cb(void);


unsigned int TOTAL_RECV_PIN;
unsigned int TOTAL_SEND_PIN;
static ipi_hook ipi_isr_callback;

void init_pin_number(void);

/*
 * read data from mbox/share memory include ipi msg header, this function must in critical context
 */
int mbox_write_hd(unsigned int mbox, unsigned int slot, void *msg)
{
	unsigned int base, slot_ofs, size;
	struct mbox_info *mtable;
	struct mtk_ipi_msg *ipimsg;
	int len;

	if(mbox >= IPI_MBOX_TOTAL || !msg)
		return MBOX_PARA_ERROR;

	mtable = &(mbox_table[mbox]);
	base = mtable->base;
	slot_ofs = slot * MBOX_SLOT_SIZE;
	size = mtable->slot;
	ipimsg = (struct mtk_ipi_msg *)msg;
	len = ipimsg->ipihd.len;

	if (slot > size)
		return MBOX_PARA_ERROR;
	/* ipi header */
	memcpy((void *)(base + slot_ofs), ipimsg, sizeof(struct mtk_ipi_msg_hd));
	/* ipi payload */
	memcpy((void *)(base + slot_ofs + sizeof(struct mtk_ipi_msg_hd)), ipimsg->data, len);

	return MBOX_DONE;
}


/*
 * read data from mbox/share memory include ipi msg header, this function must in critical context
 */
int mbox_read_hd(unsigned int mbox, unsigned int slot, void *dest)
{
	unsigned int base, slot_ofs, size;
	struct mbox_info *mtable;
	struct mtk_ipi_msg_hd *ipihead;

	if(mbox >= IPI_MBOX_TOTAL)
		return MBOX_PARA_ERROR;

	mtable = &(mbox_table[mbox]);
	base = mtable->base;
	slot_ofs = slot * MBOX_SLOT_SIZE;
	size = mtable->slot;
	ipihead = (struct mtk_ipi_msg_hd *)(base + slot_ofs);

	if (slot > size)
		return MBOX_PARA_ERROR;

	if (dest != NULL)
		memcpy(dest, (void *)(base + slot_ofs + sizeof(struct mtk_ipi_msg_hd)), ipihead->len);

	return MBOX_DONE;
}

/* write data to mbox/share memory, this function must in critical context
 *
 */

int mbox_write(unsigned int mbox, unsigned int slot, void *data,
	       unsigned int len)
{
	unsigned int base, slot_ofs, size;
	struct mbox_info *mtable;

	if (mbox >= IPI_MBOX_TOTAL)
		return MBOX_PARA_ERROR;

	mtable = &(mbox_table[mbox]);
	base = mtable->base;
	slot_ofs = slot * MBOX_SLOT_SIZE;
	size = mtable->slot;

	if (slot > size)
		return MBOX_PARA_ERROR;

	if (data != NULL && len > 0)
		memcpy((void *) (base + slot_ofs), data, len);

	return MBOX_DONE;
}

/* read data to mbox/share memory, this function must in critical context
 *
 */
int mbox_read(unsigned int mbox, unsigned int slot, void *data,
	      unsigned int len)
{
	unsigned int base, slot_ofs, size;
	struct mbox_info *mtable;

	if (mbox >= IPI_MBOX_TOTAL)
		return MBOX_PARA_ERROR;

	mtable = &(mbox_table[mbox]);
	base = mtable->base;
	slot_ofs = slot * MBOX_SLOT_SIZE;
	size = mtable->slot;

	if (slot > size)
		return MBOX_PARA_ERROR;

	if (data != NULL && len > 0)
		memcpy(data, (void *) (base + slot_ofs), len);

	return MBOX_DONE;
}

/* clear mbox/share memory irq, with read/write function must in critical context
 *
 */
int mbox_clr_irq(unsigned int mbox, unsigned int irq)
{
	struct mbox_info *mtable;

	if (mbox >= IPI_MBOX_TOTAL)
		return MBOX_PARA_ERROR;

	mtable = &(mbox_table[mbox]);
	DRV_WriteReg32(mtable->clr_irq_reg, irq);

	return MBOX_DONE;
}

/* trigger mbox/share memory irq, with read/write function must in critical context
 *
 */
int mbox_trigger_irq(unsigned int mbox, unsigned int irq)
{
	struct mbox_info *mtable;

	if (mbox >= IPI_MBOX_TOTAL)
		return MBOX_PARA_ERROR;

	mtable = &(mbox_table[mbox]);
	DRV_WriteReg32(mtable->set_irq_reg, irq);

	return MBOX_DONE;
}

/*
 * check mbox 32bits set irq reg status
 * with read/write function must in critical context
 * @return irq status 0: not triggered , other: irq triggered
 */
unsigned int mbox_check_send_irq(unsigned int mbox, unsigned int pin_index)
{
	struct mbox_info *mtable;
	unsigned int reg, irq_state;

	if (mbox >= IPI_MBOX_TOTAL)
		return 0;

	irq_state = 0;
	mtable = &(mbox_table[mbox]);
	if (mtable->send_status_reg)
		reg = DRV_Reg32(mtable->send_status_reg);
	else
		reg = DRV_Reg32(mtable->set_irq_reg);
	irq_state = (reg & (0x1 << pin_index));

	return irq_state;
}

/*
 * check mbox 32bits clr irq reg status
 * with read/write function must in critical context
 * @return irq status 0: not triggered , other: irq triggered
 */
static unsigned int mbox_read_recv_irq(unsigned int mbox)
{
	struct mbox_info *mtable;
	unsigned int reg;

	if (mbox >= IPI_MBOX_TOTAL)
		return 0;

	mtable = &(mbox_table[mbox]);
	if (mtable->recv_status_reg)
		reg = DRV_Reg32(mtable->recv_status_reg);
	else
		reg = DRV_Reg32(mtable->clr_irq_reg);

	return reg;
}

/* set mbox/share memory base address to init register
 *
 */
static int mbox_set_base_reg(unsigned int mbox, unsigned int addr)
{
	struct mbox_info *mtable;

	if (mbox >= IPI_MBOX_TOTAL)
		return MBOX_PARA_ERROR;

	mtable = &(mbox_table[mbox]);
	DRV_WriteReg32(mtable->init_base_reg, addr);

	return MBOX_DONE;
}

/* set mbox/share memory base address, task context
 *
 */
int mbox_set_base_addr(unsigned int mbox, unsigned int addr, unsigned int opt)
{
	struct mbox_info *mtable;
	int ret;

	if (mbox >= IPI_MBOX_TOTAL)
		return MBOX_PARA_ERROR;

	ret = MBOX_DONE;
	if (opt == MBOX_OPT_DIRECT || opt == MBOX_OPT_QUEUE_DIR)
		ret = mbox_set_base_reg(mbox, addr);

	if (ret != MBOX_DONE)
		return ret;

	mtable = &(mbox_table[mbox]);
	mtable->base = addr;

	return MBOX_DONE;
}

/*
 * mbox_cb_register, register callback function
 *
 */
#if 0
int mbox_cb_register(unsigned int pin_offset,
		     void (*mbox_pin_cb) (unsigned int ipi_id, void *data),
		     void *prdata)
{
	struct pin_recv *ptable;

	taskENTER_CRITICAL();
	ptable = &(mbox_pin_recv_table[pin_offset]);
	ptable->mbox_pin_cb = mbox_pin_cb;
	ptable->prdata = prdata;
	taskEXIT_CRITICAL();

	return MBOX_DONE;
}
#endif

/*
 * mbox/share memory isr, in isr context
 */
unsigned int mbox_isr(void *data)
{
	unsigned int mbox, irq_status, cb_opt;
	struct pin_recv *pin_recv;
	struct mbox_info *minfo = (struct mbox_info *) data;
	unsigned int irq_temp;
	int ret;
	unsigned int i;


	//mbox id   (0~IPI_MBOX_TOTAL)
	if (minfo == NULL)
		mbox = find_mbox_num(0);
	else
		mbox = find_mbox_num(minfo->id);

	minfo = &(mbox_table[mbox]);
	ret = MBOX_DONE;

	if (mbox >= IPI_MBOX_TOTAL) {
		return 0;
	}
	//get irq status
	irq_temp = 0x0;
	irq_status = mbox_read_recv_irq(mbox);

	mbox_isr_pre_cb();

	//execute all receive pin handler
	for (i = 0; i < TOTAL_RECV_PIN; i++) {
		pin_recv = &(mbox_pin_recv_table[i]);
		if (pin_recv->mbox != mbox)
			continue;

		if (((0x1 << pin_recv->pin_index) & irq_status) > 0x0) {
			cb_opt = pin_recv->cb_ctx_opt;
			irq_temp = irq_temp | (0x1 << pin_recv->pin_index);
			/*check user buf */
			if (pin_recv->pin_buf == NULL) {
				//assert
			}
			if (minfo->opt == MBOX_OPT_QUEUE_SMEM || minfo->opt == MBOX_OPT_QUEUE_DIR) {
				//queue mode
				struct mtk_ipi_msg_hd *ipihead;
				ipihead = (struct mtk_ipi_msg_hd *)(minfo->base + (pin_recv->offset * MBOX_SLOT_SIZE));
				ret = mbox_read_hd(mbox, pin_recv->offset, pin_recv->pin_buf);
				if (pin_recv->recv_opt == MBOX_RECV
					&& cb_opt == MBOX_CB_IN_IRQ
					&& pin_recv->mbox_pin_cb != NULL
					&& ret == MBOX_DONE) {
					pin_recv->mbox_pin_cb(ipihead->id, pin_recv->prdata, pin_recv->pin_buf ,(unsigned int)ipihead->len);
				}
			} else {
				//direct mode
				ret = mbox_read(mbox, pin_recv->offset, pin_recv->pin_buf, pin_recv->msg_size * MBOX_SLOT_SIZE);
				if (pin_recv->recv_opt == MBOX_RECV
					&& cb_opt == MBOX_CB_IN_IRQ
					&& pin_recv->mbox_pin_cb != NULL
					&& ret == MBOX_DONE) {
					pin_recv->mbox_pin_cb(pin_recv->ipi_id, pin_recv->prdata, pin_recv->pin_buf ,pin_recv->msg_size * MBOX_SLOT_SIZE);
				}
			}

			//notify task
			if (ret == MBOX_DONE && ipi_isr_callback)
				ipi_isr_callback(pin_recv->ipi_id);
		}
	}

	mbox_isr_post_cb();

	//clear irq status
	mbox_clr_irq(mbox, irq_temp);
	return 0;
}

/*
 * mbox isr for sync. layer
 */
void mbox_isr_hook(ipi_hook isr_cb)
{
	ipi_isr_callback = isr_cb;
}

/*
 * mbox init function
 */
int mbox_init(unsigned int mbox, unsigned int is64d, unsigned int opt,
	      unsigned int base_addr, void *prdata)
{
	struct mbox_info *mtable;
	int ret;

	if (mbox >= IPI_MBOX_TOTAL)
		return MBOX_PARA_ERROR;

	mtable = &(mbox_table[mbox]);
	mtable->id = mbox;
	mtable->is64d = is64d;
	mtable->opt = opt;
	//init_pin_number
	init_pin_number();

	ret = MBOX_DONE;
	if (opt == MBOX_OPT_DIRECT || opt == MBOX_OPT_QUEUE_DIR) {
		//init mbox is64d option
		ret = set_mbox_64d(mbox, is64d, prdata);
	}
	if (ret != MBOX_DONE)
		return ret;
	//init semaphore?
	//TODO

	//set base addr
	ret = mbox_set_base_addr(mbox, base_addr, opt);
	if (ret != MBOX_DONE)
		return ret;

	//hook irq handler
	ret = register_irq_handler(mbox_isr, prdata);

	return ret;
}

/*
 *mbox information dump
 */
void mbox_info_dump(void)
{
	struct pin_recv *pin_recv;
	struct pin_send *pin_send;
	unsigned int i;


	PRINTF_D("[MBOX]recv count=%u send count=%u"
				, TOTAL_RECV_PIN
				, TOTAL_SEND_PIN);


	for (i = 0; i < TOTAL_RECV_PIN; i++) {
		pin_recv = &(mbox_pin_recv_table[i]);
		PRINTF_D("[MBOX]recv pin:%d mbox=%u offset=%u recv_opt=%u cb_ctx_opt=%u msg_size=%u pin_index=%u chan_id=%u"
				, i
				, pin_recv->mbox
				, pin_recv->offset
				, pin_recv->recv_opt
				, pin_recv->cb_ctx_opt
				, pin_recv->msg_size
				, pin_recv->pin_index
				, pin_recv->ipi_id);
	}

	for (i = 0; i < TOTAL_SEND_PIN; i++) {
		pin_send = &(mbox_pin_send_table[i]);
		PRINTF_D("[MBOX]send pin:%d mbox=%u offset=%u send_opt=%u msg_size=%u pin_index=%u chan_id=%u"
				, i
				, pin_send->mbox
				, pin_send->offset
				, pin_send->send_opt
				, pin_send->msg_size
				, pin_send->pin_index
				, pin_send->ipi_id);
	}
}
