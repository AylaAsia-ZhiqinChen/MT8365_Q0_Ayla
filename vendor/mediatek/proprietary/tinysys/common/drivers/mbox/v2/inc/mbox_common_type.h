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

#ifndef _MBOX_COMMON_TYPE_H_
#define _MBOX_COMMON_TYPE_H_
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/*
 * mailbox slot size defination
 */
#define MBOX_SLOT_SIZE 4	/* mailbox 1 slot = 4 bytes */

/*
 * mbox pin structure, this is for send defination,
 * ipi=endpoint=pin
 * mbox     : (mbox number)mbox number of the pin, up to 16
 * offset   : (slot)msg offset in share memory, 4 bytes alignment, up to 1024*4 KB
 * send_opt : (opt)send opt, 0:send ,1: send for response
 * lock_opt : (opt)option 0: mutex, 1: busy wait
 * msg_size : (slot)message size in words, 4 bytes alignment
 * pin_index  : (bit offset)pin index in the mbox
 * ipi_id     : (ipi_id)ipi_id in the mbox
 * mutex      : (mutex)mutex for remote response
 * sema_ack   : (sema_ack)completion for remote response
 */
struct pin_send {
	unsigned int mbox:4,
		     offset:20,
		     send_opt:2,
		     lock_opt:2;
	unsigned int msg_size;
	unsigned int pin_index;
	unsigned int ipi_id;
	SemaphoreHandle_t mutex;
	SemaphoreHandle_t sema_ack;
};

/*
 * mbox pin structure, this is for receive defination,
 * ipi=endpoint=pin
 * mbox     : (mbox number)mbox number of the pin, up to 16
 * offset   : (slot)msg offset in share memory, 4 bytes alignment, up to 1024*4 KB
 * recv_opt : (opt)recv option,  0:receive ,1: response
 * lock_opt : (opt)option 0: mutex, 1: busy wait
 * buf_full_opt : (opt)buffer option 0:drop, 1:assert, 2:overwrite
 * cb_ctx_opt : (opt)callback option 0:isr context, 1:process context
 * msg_size   : (slot)msg used slots in the mbox, 4 bytes alignment
 * pin_index  : (bit offset)pin index in the mbox
 * ipi_id     : (ipi_id)ipi_id in the mbox
 * notify     : (completion)notify process
 * mbox_pin_cb: (cb)cb function
 * pin_buf : (void*)buffer point
 * prdata  : (void*)private data
 */
struct pin_recv {
	unsigned int mbox:4,
		     offset:20,
		     recv_opt:2,
		     lock_opt:2,
		     buf_full_opt:2,
		     cb_ctx_opt:2;
	unsigned int msg_size;
	unsigned int pin_index;
	unsigned int ipi_id;
	SemaphoreHandle_t notify;
	void (*mbox_pin_cb) (unsigned int id, void *prdata, void *data, unsigned int len);
	void *pin_buf;
	void *prdata;
};

/*
 * mailbox/share memory callback function context, 0:in isr context, 1:in task context
 */
enum {
	MBOX_CB_IN_IRQ = 0,
	MBOX_CB_IN_TASK = 1,
};
/*
 * mailbox/share memory user buffer full action, 0:drop, 1:assert, 2:overwrite
 */
enum {
	MBOX_BUF_FULL_DROP = 0,
	MBOX_BUF_FULL_ASSERT = 1,
	MBOX_BUF_FULL_OVERWRITE = 2,
	MBOX_BUF_COPY_DONE = 3,
	MBOX_BUF_FULL_RET = 4,
};

/*
 * mbox recv action defination
 * 0:for recv, 1:for response
 */
enum MBOX_RECV_OPT {
	MBOX_RECV = 0,
	MBOX_RESPONSE = 1,
};

/*
 * mailbox options
 * 0: mbox
 * 1: share memory
 * 2: mbox with queue
 * 3: share memory with queue
 */
enum {
	MBOX_OPT_DIRECT    = 0,
	MBOX_OPT_SMEM      = 1,
	MBOX_OPT_QUEUE_DIR = 2,
	MBOX_OPT_QUEUE_SMEM = 3,
};
/*
 * mailbox/share memory options, 0:32d, 1:64d
 */
enum {
	MBOX_OPT_32_SLOT = 0,
	MBOX_OPT_64_SLOT = 1,
};

/*
 * mbox information
 *
 * irq_num:identity of mbox irq
 * id     :mbox id
 * is64d  :mbox is64d status, 0:32d, 1: 64d
 * slot   :how many slots that mbox used
 * opt    :option for tx mode, 0:mbox, 1:share memory, 2: queue mode
 * base   :mbox base address
 * set_irq_reg  : mbox set irq register
 * clr_irq_reg  : mbox clear irq register
 * init_base_reg: mbox initialize register
 * mbox_mutex   : mutex of mbox
 */
struct mbox_info {
	unsigned int id:30,
		     is64d:2;
	unsigned int slot;
	unsigned int opt;
	unsigned int base;
	unsigned int set_irq_reg;
	unsigned int clr_irq_reg;
	unsigned int init_base_reg;
	unsigned int send_status_reg;
	unsigned int recv_status_reg;
	SemaphoreHandle_t mbox_mutex;
};

/*
 * mailbox/share memory return value defination
 */
enum {
	MBOX_CONFIG_ERROR = -3,
	MBOX_IRQ_ERROR = -2,
	MBOX_PARA_ERROR = -1,
	MBOX_DONE = 0,
	MBOX_PIN_BUSY = 1,
};


/*
 * mtk ipi message header
 *
 * id       :message id
 * len      :data length in byte
 * options  :options
 * reserved :reserved
 */
struct mtk_ipi_msg_hd {
	uint32_t id;
	uint32_t len;
	uint32_t options;
	uint32_t reserved;
};

struct mtk_ipi_msg {
	struct mtk_ipi_msg_hd ipihd;
	void *data;
};

#endif
