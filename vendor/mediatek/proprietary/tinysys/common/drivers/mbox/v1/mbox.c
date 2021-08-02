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

#include <stdio.h>
#include <debug.h>
#include <FreeRTOS.h>
#include <task.h>
#include <driver_api.h>
#include <tinysys_reg.h>
#include "irq.h"
#include "mbox.h"

#define MBOX_OFS(x)         ((unsigned int) (x) << 2)
#define MBOX_IN_IRQ(x)      (MBOX0_IN_IRQ + MBOX_OFS(x))
#define MBOX_OUT_IRQ(x)     (MBOX0_OUT_IRQ + MBOX_OFS(x))
#define MBOX_BASE(x)        (MBOX0_BASE + MBOX_OFS(x))
#define MBOX_IS64D(x, r)    (((r) >> (x)) & 0x1)

static unsigned int mbox_cnt;
static unsigned int mbox_init_done;
static ipi_isr ipi_isr_callback;

unsigned int mbox_isr(void *data);

static unsigned int _mbox_remap(unsigned int mbox, unsigned int base)
{
    unsigned int is64d;

    is64d = MBOX_IS64D(mbox, DRV_Reg32(MBOX8_7B_BASE));

    if (is64d == 1)
        return ((base >> 8) & 0xFFF) << 1;
    else
        return ((base >> 7) & 0x1FFF) << 0;
}

static unsigned int _mbox_base(unsigned int mbox)
{
    unsigned int reg, is64d;

    reg = DRV_Reg32(MBOX_BASE(mbox));
    is64d = MBOX_IS64D(mbox, DRV_Reg32(MBOX8_7B_BASE));

    if (is64d == 1) {
        return ((reg >> 1) & 0xFFF) << 8;
    } else {
        return ((reg << 0) & 0x1FFF) << 7;
    }
}

static void _mbox_send(unsigned int mbox, unsigned int reg)
{
    DRV_WriteReg32(MBOX_OUT_IRQ(mbox), reg);
}

static void _mbox_clr(unsigned int mbox, unsigned int reg)
{
    DRV_WriteReg32(MBOX_IN_IRQ(mbox), reg);
}

unsigned int mbox_init(unsigned int mode, unsigned int *base_addr, unsigned int count, ipi_isr ipi_isr_cb)
{
    unsigned int reg, base, mbox, addr;

    DRV_WriteReg32(MBOX8_7B_BASE, mode);
    reg = DRV_Reg32(MBOX8_7B_BASE);

    if (count > MBOX_NUM)
        count = MBOX_NUM;

    if (ipi_isr_cb == NULL)
        return 0;

    taskENTER_CRITICAL();

    for (mbox = 0; mbox < count; mbox++) {
        if ((reg & 0x1) != 0)
            addr = base_addr[mbox] & (MBOX_8BYTE - 1);
        else
            addr = base_addr[mbox] & (MBOX_4BYTE - 1);

        if (addr != 0) {
            PRINTF_E("mbox %u init error: base addr 0x%08x\n", mbox, base_addr[mbox]);
            continue;
        }

        base = _mbox_remap(mbox, base_addr[mbox]);
        DRV_WriteReg32(MBOX_BASE(mbox), base);
        PRINTF_D("mbox %u base=0x%x (0x%x)\n", mbox, base, base_addr[mbox]);

        reg >>= 1;
    }

    for (mbox = 0; mbox < count; mbox++) {
        base = DRV_Reg32(MBOX_BASE(mbox));

        if (base == 0)
            break;

        if (intc_irq_request(INTC_IRQ_MBOX0 + mbox, INTC_GRP_2, INTC_POL_HIGH, mbox_isr, (void *)mbox) != 0)
            break;
    }

    mbox_cnt = mbox;
    ipi_isr_callback = ipi_isr_cb;
    mbox_init_done = 1;

    taskEXIT_CRITICAL();

    return mbox_cnt;
}

unsigned int mbox_get_base(unsigned int mbox)
{
    if (mbox_init_done)
        return _mbox_base(mbox);
    else
        return 0;
}

unsigned int mbox_size(unsigned int mbox)
{
    unsigned int reg;

    if (mbox >= mbox_cnt)
        return 0;

    reg = DRV_Reg32(MBOX8_7B_BASE);

    if ((reg & (0x1 << mbox)) == 0)
        return MBOX_4BYTE / MBOX_SLOT_SIZE;
    else
        return MBOX_8BYTE / MBOX_SLOT_SIZE;
}

int mbox_send(unsigned int mbox, unsigned int slot, unsigned int irq, void *data, unsigned int len)
{
    unsigned int reg, base, size, slot_ofs;

    if (mbox >= mbox_cnt)
        return -1;
/*
    reg = DRV_Reg32(MBOX_IN_IRQ(mbox));
    if(reg != 0x0)
        return -1;
*/
    /* See if the busy bit is 1, return error if 1 */
    reg = DRV_Reg32(MBOX_OUT_IRQ(mbox));
    if(reg & (0x1 << irq)) {
        //PRINTF_E("mbox %d irq %d busy\n", mbox, irq);
        return -1;
    }

    size = mbox_size(mbox);
    slot_ofs = MBOX_SLOT_SIZE * slot;

    if (slot > size || (slot + len) > size)
        return -1;

    taskENTER_CRITICAL();

    base = _mbox_base(mbox);

    if (data != NULL && len > 0)
        memcpy((void *)(base + slot_ofs), data, len * MBOX_SLOT_SIZE);

    _mbox_send(mbox, 0x1 << irq);

    taskEXIT_CRITICAL();

    return 0;
}

int mbox_read(unsigned int mbox, unsigned int slot, void *data, unsigned int len)
{
    unsigned int reg, base, size, slot_ofs;

    if (mbox >= mbox_cnt) {
        return -1;
    }

    size = mbox_size(mbox);
    slot_ofs = MBOX_SLOT_SIZE * slot;

    if (slot > size || (slot + len) > size) {
        return -1;
    }

    //taskENTER_CRITICAL();

    base = _mbox_base(mbox);

    if (data != NULL && len > 0)
        memcpy(data, (void *)(base + slot_ofs), len * MBOX_SLOT_SIZE);

    //taskEXIT_CRITICAL();

    return 0;
}

int mbox_write(unsigned int mbox, unsigned int slot, void *data, unsigned int len)
{
    unsigned int reg, base, size, slot_ofs;

    if (mbox >= mbox_cnt)
        return -1;

    size = mbox_size(mbox);
    slot_ofs = MBOX_SLOT_SIZE * slot;

    if (slot > size || (slot + len) > size)
        return -1;

    //taskENTER_CRITICAL();

    base = _mbox_base(mbox);

    if (data != NULL && len > 0)
        memcpy((void *)(base + slot_ofs), data, len * MBOX_SLOT_SIZE);

    //taskEXIT_CRITICAL();

    return 0;
}

int mbox_polling(unsigned int mbox, unsigned irq, unsigned int slot, unsigned int *retdata, unsigned int retries)
{
    unsigned int reg;
    int ret = -1;

    if (mbox >= mbox_cnt || retries == 0)
        return ret;

    irq = 0x1 << irq;

    taskENTER_CRITICAL();

    do {
        reg = DRV_Reg32(MBOX_IN_IRQ(mbox));
    } while ( !(reg & irq) && --retries > 0);

    if ((reg & irq) != 0) {
        if (retdata)
            memcpy(retdata, (void *)(_mbox_base(mbox) + (slot * MBOX_SLOT_SIZE)), MBOX_SLOT_SIZE);

        _mbox_clr(mbox, irq);
        ret = 0;
    }

    taskEXIT_CRITICAL();

    return ret;
}

unsigned int mbox_isr(void *data)
{
    unsigned int mbox, irq, base;

    mbox = (unsigned int) data;

    if (mbox >= MBOX_NUM)
        return 1;

    irq = DRV_Reg32(MBOX_IN_IRQ(mbox));
    base = _mbox_base(mbox);

#ifdef DVT_TEST
    PRINTF_D_ISR("Receive MBOX%d: 0x%x\n", mbox, irq);
#else
    if (mbox < mbox_cnt && ipi_isr_callback)
        ipi_isr_callback(mbox, base, irq);
#endif
    _mbox_clr(mbox, irq);

    return 0;
}
