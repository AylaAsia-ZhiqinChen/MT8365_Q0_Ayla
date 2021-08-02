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

#include <tinysys_reg.h>
#include <driver_api.h>
#include "timer.h"
#include "irq.h"
#include <FreeRTOS.h>
#include <task.h>

#define TIMER_CNT_EN        (1 << 0)
#define TIMER_CNT_26MCLK    (1 << 4)
#define TIMER_CNT_IRQ_EN    (1 << 0)
#define TIMER_CNT_IRQ_ST    (1 << 4)
#define TIMER_CNT_IRQ_CLR   (1 << 5)

#define _26MHZ_CLK_

#ifdef CFG_FPGA

#ifdef _26MHZ_CLK_
#define TIMER_VAL_100USEC(x) (6000000 * (x) / 10000)
#else
#define TIMER_VAL_100USEC(x) (11718 * (x) / 10000) // FPGA
#endif

#else

#ifdef _26MHZ_CLK_
#define TIMER_VAL_100USEC(x) (26000000 * (x) / 10000)
#else
#define TIMER_VAL_100USEC(x) (32768 * (x) / 10000)
#endif

#endif

#define TIMER_OFS(x)        (x << 4)
#define TIMER_EN(x)         (TIMER0_EN + TIMER_OFS(x))
#define TIMER_RESET_VAL(x)  (TIMER0_RESET_VAL + TIMER_OFS(x))
#define TIMER_CUR_VAL(x)    (TIMER0_CUR_VAL + TIMER_OFS(x))
#define TIMER_IRQ_ACK(x)    (TIMER0_IRQ_ACK + TIMER_OFS(x))
#define GPT_COUNT_TO_NS     30000 //32KHz

static int _timer_enable(unsigned int timer, unsigned int enable)
{
    if (timer >= TIMER_NUM)
        return -1;

    if (enable == TIMER_ENABLE)
        DRV_SetReg32(TIMER_EN(timer), TIMER_CNT_EN);
    else
        DRV_ClrReg32(TIMER_EN(timer), TIMER_CNT_EN);

    return 0;
}

int timer_enable(unsigned int timer)
{
    return _timer_enable(timer, TIMER_ENABLE);
}

int timer_disable(unsigned int timer)
{
    return _timer_enable(timer, TIMER_DISABLE);
}

int timer_set(unsigned int timer, unsigned int interval_100us, unsigned int irq)
{
    unsigned int val;

    if (timer_disable(timer) != 0)
        return -1;
#ifdef _26MHZ_CLK_
    DRV_SetReg32(TIMER_EN(timer), TIMER_CNT_26MCLK);
#endif
    val = TIMER_VAL_100USEC(interval_100us);
    DRV_WriteReg32(TIMER_RESET_VAL(timer), val);

    val = (irq) ? TIMER_CNT_IRQ_EN : 0;
    DRV_SetReg32(TIMER_IRQ_ACK(timer), val);

    return 0;
}

unsigned int timer_read(unsigned int timer)
{
    if (timer >= TIMER_NUM)
        return 0;

    return DRV_Reg32(TIMER_CUR_VAL(timer));
}

int timer_irq_is_assert(unsigned int timer)
{
    unsigned int val;

    if (timer >= TIMER_NUM)
        return 0;

    val = DRV_Reg32(TIMER_IRQ_ACK(timer));

    return (val & TIMER_CNT_IRQ_ST) ? 1 : 0;
}

void timer_irq_ack(unsigned int timer)
{
    unsigned int val;

    if (timer >= TIMER_NUM)
        return;

    do {
        DRV_SetReg32(TIMER_IRQ_ACK(timer), TIMER_CNT_IRQ_CLR);

        val = DRV_Reg32(TIMER_IRQ_ACK(timer));
    } while (val & TIMER_CNT_IRQ_ST);
}
