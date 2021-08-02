/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
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
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER\'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER\'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER\'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK\'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK\'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
 * applicable license agreements with MediaTek Inc.
 */

#ifndef _MBOX_H_
#define _MBOX_H_

#define MBOX_NUM            5
#define MBOX_SLOT_SIZE      4 // 4 bytes

#define MBOX_SEC_4B         ".mbox4"
#define MBOX_SEC_8B         ".mbox8"

enum MBOX_SIZE {
    MBOX_4BYTE = 0x80,
    MBOX_8BYTE = 0x100,
};

enum MBOX_STATUS {
    MBOX_DONE,
    MBOX_FAIL,
};

/* MD32 need initial value to put variable to a specific section, or data will be placed in bss force */
#define MBOX_BASE_4B(x)     unsigned char x[MBOX_4BYTE] __attribute__ ((section (MBOX_SEC_4B))) = {0}
#define MBOX_BASE_8B(x)     unsigned char x[MBOX_8BYTE] __attribute__ ((section (MBOX_SEC_8B))) = {0}

typedef void (*ipi_isr)(unsigned int mbox, unsigned int base, unsigned int irq);

unsigned int mbox_init(unsigned int mode, unsigned int *base_addr, unsigned int count, ipi_isr ipi_isr_cb);
unsigned int mbox_get_base(unsigned int mbox);
unsigned int mbox_size(unsigned int mbox);
int mbox_read(unsigned int mbox, unsigned int slot, void *data, unsigned int len);
int mbox_write(unsigned int mbox, unsigned int slot, void *data, unsigned int len);
int mbox_send(unsigned int mbox, unsigned int slot, unsigned int irq, void *data, unsigned int len);
int mbox_polling(unsigned int mbox, unsigned irq, unsigned int slot, unsigned int *retdata, unsigned int retries);
#endif
