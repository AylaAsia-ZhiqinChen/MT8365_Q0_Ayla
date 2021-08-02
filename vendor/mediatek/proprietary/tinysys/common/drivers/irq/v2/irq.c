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


#define WORD_LEN                    32
#define INTC_WORD(irq)              ((irq) >> 5)
#define INTC_BIT(irq)               ((irq) & 0x1F)
#define INTC_WORD_OFS(word)         ((word) << 2)
#define INTC_GROUP_OFS(grp)         ((grp) << 4)

#define INTC_IRQ_RAW_STA(word)      (INTC_IRQ_RAW_STA0 + INTC_WORD_OFS(word))
#define INTC_IRQ_STA(word)          (INTC_IRQ_STA0 + INTC_WORD_OFS(word))
#define INTC_IRQ_EN(word)           (INTC_IRQ_EN0 + INTC_WORD_OFS(word))
#define INTC_IRQ_WAKE_EN(word)      (INTC_IRQ_WAKE_EN0 + INTC_WORD_OFS(word))
#define INTC_IRQ_POL(word)          (INTC_IRQ_POL0 + INTC_WORD_OFS(word))
#define INTC_IRQ_GRP(grp, word)     (INTC_IRQ_GRP0_0 + INTC_GROUP_OFS(grp) + INTC_WORD_OFS(word))
#define INTC_IRQ_GRP_STA(grp, word) (INTC_IRQ_GRP0_STA0 + INTC_GROUP_OFS(grp) + INTC_WORD_OFS(word))


struct irq_descr_t {
    irq_handler_t handler;
    void *userdata;
};

static struct irq_status_t {
    int group;
    int irq;
} irq_status;

static struct irq_descr_t intc_irq_desc[INTC_IRQ_NUM];

int intc_irq_mask(struct intc_flags *flags)
{
    unsigned int word, reg;
#if !INTC_IRQ_MASK_BY_EN
    unsigned int grp;
#endif
    if (flags == NULL)
        return -1;

    for (word = 0; word < INTC_GRP_LEN; word++) {
#if !INTC_IRQ_MASK_BY_EN
        for (grp = 0;grp < INTC_GRP_NUM;grp++) {
            reg = INTC_IRQ_GRP(grp, word);
            flags->grp[grp][word] = DRV_Reg32(reg);
            DRV_WriteReg32(reg, 0x0);
        }
#endif
        reg = INTC_IRQ_EN(word);
        flags->en[word] = DRV_Reg32(reg);
        DRV_WriteReg32(reg, 0x0);
    }

    return 0;
}

int intc_irq_restore(struct intc_flags *flags)
{
    unsigned int word, reg;
#if !INTC_IRQ_MASK_BY_EN
        unsigned int grp;
#endif

    if (flags == NULL)
        return -1;

    for (word = 0; word < INTC_GRP_LEN; word++) {
        reg = INTC_IRQ_EN(word);
        DRV_WriteReg32(reg, flags->en[word]);
#if !INTC_IRQ_MASK_BY_EN
        for (grp = 0;grp < INTC_GRP_NUM;grp++) {
            reg = INTC_IRQ_GRP(grp, word);
            DRV_WriteReg32(reg, flags->grp[grp][word]);
        }
#endif
    }

    return 0;
}

static int _intc_irq_enable(unsigned int irq, unsigned int group, unsigned int pol)
{
    unsigned int word, mask, reg, val;

    if (irq >= INTC_IRQ_NUM)
        return -1;
    if (group >= INTC_GRP_NUM)
        return -1;
    if (pol >= INTC_POL_NUM)
        return -1;

    word = INTC_WORD(irq);
    mask = (1 << INTC_BIT(irq));

    // disable interrupt
    reg = INTC_IRQ_EN(word);
    DRV_ClrReg32(reg, mask);

    // set polarity
    reg = INTC_IRQ_POL(word);

    if (pol == INTC_POL_HIGH)
        DRV_ClrReg32(reg, mask);
    else
        DRV_SetReg32(reg, mask);

    // set group
    reg = INTC_IRQ_GRP(group, word);
    DRV_SetReg32(reg, mask);

    // enable interrupt
    reg = INTC_IRQ_EN(word);
    DRV_SetReg32(reg, mask);

    return 0;
}

int intc_irq_enable_from_ISR(unsigned int irq, unsigned int group, unsigned int pol)
{
    unsigned int ulSavedInterruptMask;
    int ret;

    ulSavedInterruptMask = portSET_INTERRUPT_MASK_FROM_ISR();

    ret = _intc_irq_enable(irq, group, pol);

    portCLEAR_INTERRUPT_MASK_FROM_ISR(ulSavedInterruptMask);

    return ret;
}

int intc_irq_enable(unsigned int irq, unsigned int group, unsigned int pol)
{
    int ret;

    taskENTER_CRITICAL();

    ret = _intc_irq_enable(irq, group, pol);

    taskEXIT_CRITICAL();

    return ret;
}

static int _intc_irq_disable(unsigned int irq)
{
#if !INTC_IRQ_MASK_BY_EN
    unsigned int grp;
#endif
    unsigned int word, mask, reg, val;

    if (irq >= INTC_IRQ_NUM)
        return -1;

    word = INTC_WORD(irq);
    mask = (1 << INTC_BIT(irq));

    // remove wakeup src
    reg = INTC_IRQ_WAKE_EN(word);
    DRV_ClrReg32(reg, mask);

    // disable interrupt
    reg = INTC_IRQ_EN(word);
    DRV_ClrReg32(reg, mask);

#if !INTC_IRQ_MASK_BY_EN

    for (grp = 0;grp < INTC_GRP_NUM;grp++) {
        reg = INTC_IRQ_GRP(grp, word);
        DRV_ClrReg32(reg, mask);
    }
#endif

    return 0;
}

int intc_irq_disable_from_ISR(unsigned int irq)
{
    unsigned int ulSavedInterruptMask;
    int ret;

    ulSavedInterruptMask = portSET_INTERRUPT_MASK_FROM_ISR();

    ret = _intc_irq_disable(irq);

    portCLEAR_INTERRUPT_MASK_FROM_ISR(ulSavedInterruptMask);

    return ret;
}

int intc_irq_disable(unsigned int irq)
{
    int ret;

    taskENTER_CRITICAL();

    ret = _intc_irq_disable(irq);

    taskEXIT_CRITICAL();

    return ret;
}

static int _intc_irq_remove_from_group(unsigned int group, unsigned int irq)
{
    unsigned int mask, word, val;

    if (irq >= INTC_IRQ_NUM)
        return -1;
    if (group >= INTC_GRP_NUM)
        return -1;

    word = INTC_WORD(irq);
    mask = (1 << INTC_BIT(irq));

    DRV_ClrReg32(INTC_IRQ_GRP(group, word), mask);

    return 0;
}

int intc_irq_wakeup_set(unsigned int irq, unsigned int wake_src)
{
    unsigned int reg, val, word, mask;
#if !INTC_IRQ_MASK_BY_EN
    unsigned int grp;
#endif

    if (irq >= INTC_IRQ_NUM)
        return -1;

    taskENTER_CRITICAL();

    word = INTC_WORD(irq);
    mask = (1 << INTC_BIT(irq));

    if (wake_src) {
        val = DRV_Reg32(INTC_IRQ_EN(word));
        if (!(val & mask))
            return -1;

#if !INTC_IRQ_MASK_BY_EN
        for (grp = 0;grp < INTC_GRP_NUM;grp++) {
            val = DRV_Reg32(INTC_IRQ_GRP(grp, word));
            if (val & mask)
                break;
        }

        if (grp == INTC_GRP_NUM)
            return -1;
#endif
    }

    reg = INTC_IRQ_WAKE_EN(word);

    if (wake_src)
        DRV_SetReg32(reg, mask);
    else
        DRV_ClrReg32(reg, mask);

    taskEXIT_CRITICAL();

    return 0;
}

static int _intc_get_pending_grp(void)
{
    unsigned int val, group;

    val = DRV_Reg32(INTC_IRQ_OUT);

    if (val == 0)
        return -1;

    for (group = 0; group < INTC_GRP_NUM; group++) {
        if (val & 0x1)
            break;

        val >>= 1;
    }

    return group;
}

static void _intc_clr_grp(unsigned int group)
{
    unsigned int val;

    val = (1 << group);

    DRV_WriteReg32(INTC_IRQ_CLR_TRG, val);
}

static int _intc_get_pending_irq(unsigned int group)
{
    int word, irq;
    unsigned int reg, group_sta, irq_sta;

    if (group >= INTC_GRP_NUM)
        return -1;

#ifdef INTC_GPR_MCDI
    if (group == INTC_GPR_MCDI) {
        word = INTC_GRP_LEN - 1;
        reg = INTC_IRQ_STA(word);
        irq_sta = DRV_Reg32(reg);

        if (!irq_sta)
            return -1;
    } else
#endif
    {
        for (word = (INTC_GRP_LEN - 1); word >= 0; word--) {
            reg = INTC_IRQ_GRP_STA(group, word);
            group_sta = DRV_Reg32(reg);

            if (group_sta == 0)
                continue;

            reg = INTC_IRQ_STA(word);
            irq_sta = DRV_Reg32(reg);

            irq_sta &= group_sta;

            if (irq_sta != 0)
                break;
        }

        if (word < 0)
            return -1;
    }

    irq = (int)(WORD_LEN - 1 - clz(irq_sta));
    irq += WORD_LEN * word;

    return irq;
}

static void _intc_irq_status_record(int irq, int group)
{
    irq_status.group = group;
    irq_status.irq = irq;

}

void intc_init(void)
{
    unsigned int word, group;

    taskENTER_CRITICAL();

    for (word = 0; word < INTC_GRP_LEN; word++) {
        DRV_WriteReg32(INTC_IRQ_EN(word), 0x0);
        DRV_WriteReg32(INTC_IRQ_WAKE_EN(word), 0x0);
        DRV_WriteReg32(INTC_IRQ_POL(word), 0x0);
    }

    for (group = 0; group < INTC_GRP_NUM; group++) {
        for (word = 0; word < INTC_GRP_LEN; word++) {
            DRV_WriteReg32(INTC_IRQ_GRP(group, word), 0x0);
        }
    }

    _intc_irq_status_record(-1, -1);

    taskEXIT_CRITICAL();
}

int intc_irq_request(unsigned int irq, unsigned int group, unsigned int pol, irq_handler_t handler, void *userdata)
{
    struct irq_descr_t *irq_descr;

    if (irq >= INTC_IRQ_NUM)
        return -1;
    if (group >= INTC_GRP_NUM)
        return -1;
    if (pol >= INTC_POL_NUM)
        return -1;

    irq_descr = &intc_irq_desc[irq];

    taskENTER_CRITICAL();

    if (irq_descr->handler)
        goto fail;

    irq_descr->handler = handler;
    irq_descr->userdata = userdata;

    if (intc_irq_enable(irq, group, pol) != 0) {
        irq_descr->handler = NULL;
        goto fail;
    }

    taskEXIT_CRITICAL();
    return 0;

fail:
    taskEXIT_CRITICAL();
    return -1;
}

void intc_isr_dispatch(unsigned int group)
{
    unsigned int ret;
    unsigned long ulSavedInterruptMask;
    int irq;
    irq_handler_t handler;
    void *userdata;
    unsigned long isr_start, isr_end, isr_duration;

    ulSavedInterruptMask = portSET_INTERRUPT_MASK_FROM_ISR();

    if (group >= INTC_GRP_NUM) {
        portCLEAR_INTERRUPT_MASK_FROM_ISR_AND_LEAVE(ulSavedInterruptMask);
        return ;
    }

    if ((irq = _intc_get_pending_irq(group)) >= 0) {
        handler = intc_irq_desc[irq].handler;
        userdata = intc_irq_desc[irq].userdata;

        if (handler != NULL) {
            _intc_irq_status_record(irq, group);
#ifdef ISR_DURATION_LIMIT_NS
            isr_start = ostimer_low_cur_to_ns();
#endif

            ret = handler(userdata);

#ifdef ISR_DURATION_LIMIT_NS
            isr_end = ostimer_low_cur_to_ns();

            if (isr_end >= isr_start) {
                isr_duration = isr_end - isr_start;
            } else {
                isr_duration = (0xffffffffUL) - isr_start + isr_end;
            }

            if (isr_duration > ISR_DURATION_LIMIT_NS) {
                PRINTF_E_ISR("ISR#%d over realtime: %lu us!\n", irq, isr_duration/1000);
                /* configASSERT(0); */
            }
#endif
        } else {
            PRINTF_E_ISR("No IRQ#%d handler\n", irq);
        }
    }

    _intc_irq_status_record(-1, -1);
    _intc_clr_grp(group);
    vic_reset_priority();

    portCLEAR_INTERRUPT_MASK_FROM_ISR_AND_LEAVE(ulSavedInterruptMask);
}

extern UBaseType_t xGetCriticalNesting();

bool is_in_isr(void)
{
    return (irq_status.irq >= 0) || ( xGetCriticalNesting() > 0);
}
