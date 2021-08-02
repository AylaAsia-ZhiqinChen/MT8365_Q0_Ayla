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

#ifndef _MBOX_COMMON_H_
#define _MBOX_COMMON_H_

#include "mbox_common_type.h"
#include "mbox_platform.h"

typedef void (*ipi_hook) (int ipi_id);

int mbox_write_hd(unsigned int mbox, unsigned int slot, void *msg);
int mbox_read_hd(unsigned int mbox, unsigned int slot, void *dest);
int mbox_write(unsigned int mbox, unsigned int slot, void *data,
	       unsigned int len);
int mbox_read(unsigned int mbox, unsigned int slot, void *data,
	      unsigned int len);
int mbox_clr_irq(unsigned int mbox, unsigned int irq);
int mbox_trigger_irq(unsigned int mbox, unsigned int irq);
unsigned int mbox_read_irq_status(unsigned int mbox);
int mbox_set_base_addr(unsigned int mbox, unsigned int addr, unsigned int opt);
unsigned int mbox_get_pin_irq_status(unsigned int mbox, unsigned int irq);
unsigned int mbox_check_send_irq(unsigned int mbox, unsigned int pin_index);
int mbox_init(unsigned int mbox, unsigned int is64d, unsigned int opt,
	      unsigned int base_addr, void *prdata);
unsigned int mbox_isr(void *data);
void mbox_isr_hook(ipi_hook isr_cb);
void mbox_info_dump(void);

extern struct mbox_info mbox_table[];
extern struct pin_send mbox_pin_send_table[];
extern struct pin_recv mbox_pin_recv_table[];
extern unsigned int TOTAL_RECV_PIN;
extern unsigned int TOTAL_SEND_PIN;

#endif
