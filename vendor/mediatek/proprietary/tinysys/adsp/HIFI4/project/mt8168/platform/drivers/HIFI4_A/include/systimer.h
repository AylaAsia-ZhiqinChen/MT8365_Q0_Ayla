/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2015. All rights reserved.
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
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __MT_SYSTIMER_H__
#define __MT_SYSTIMER_H__

#include <driver_api.h>
#include <mt_reg_base.h>
#include "main.h"


/**
 * system timer register map
 */
#define CNTCR           (DSP_TIMER_BASE + 0x00)
#define CNTSR           (DSP_TIMER_BASE + 0x04)
#define CNTCV_L         (DSP_TIMER_BASE + 0x08)
#define CNTCV_H         (DSP_TIMER_BASE + 0x0c)
#define CNTWACR         (DSP_TIMER_BASE + 0x10)
#define CNTRACR         (DSP_TIMER_BASE + 0x14)
#define CNTACR_LOCK     (DSP_TIMER_BASE + 0x18)
#define CNTFID0         (DSP_TIMER_BASE + 0x20)
#define CNTFID1         (DSP_TIMER_BASE + 0x24)
#define CNTFID2         (DSP_TIMER_BASE + 0x28)
#define CNTFIDE         (DSP_TIMER_BASE + 0x2c)
#define CNTTVAL0_CON    (DSP_TIMER_BASE + 0x40)
#define CNTTVAL0        (DSP_TIMER_BASE + 0x44)
#define CNTTVAL1_CON    (DSP_TIMER_BASE + 0x48)
#define CNTTVAL1        (DSP_TIMER_BASE + 0x4c)
#define CNTTVAL2_CON    (DSP_TIMER_BASE + 0x50)
#define CNTTVAL2        (DSP_TIMER_BASE + 0x54)
#define CNTTVAL3_CON    (DSP_TIMER_BASE + 0x58)
#define CNTTVAL3        (DSP_TIMER_BASE + 0x5c)

#define CNT_EN          (0x1 << 0)
#define CNTTVAL_EN      (0x1 << 0)
#define CNTIRQ_EN       (0x1 << 1)
#define CNTIRQ_STACLR   (0x1 << 4)
#define CNTMODE_REPEAT  (0x1 << 8)


#ifdef CFG_FPGA
#define DELAY_TIMER_1US_TICK       ((unsigned int)10) // (10MHz)
#define DELAY_TIMER_1MS_TICK       ((unsigned int)10000) // (10MHz)
#else
#define DELAY_TIMER_1US_TICK       ((unsigned int)13) // (13MHz)
#define DELAY_TIMER_1MS_TICK       ((unsigned int)13000) // (13MHz)
#endif
#define TIME_TO_TICK_US(us) ((us)*DELAY_TIMER_1US_TICK)
#define TIME_TO_TICK_MS(ms) ((ms)*DELAY_TIMER_1MS_TICK)

#define SYSTICK_TIMER_IRQ           L1_DSP_TIMER_IRQ0_B


#define MAX_SYSTIMER_COUNT          (0xffffffffUL)

#define SYSTIMER_DISABLE            DRV_ClrReg32(CNTTVAL0_CON, CNTTVAL_EN)
#define SYSTIMER_GET_COUNTER        DRV_Reg32(CNTTVAL0)
#define SYSTIMER_ENABLE             DRV_SetReg32(CNTTVAL0_CON, CNTTVAL_EN)
#define SYSTIMER_ENABLE_IRQ         DRV_SetReg32(CNTTVAL0_CON, CNTIRQ_EN)
#define SYSTIMER_SET_RSTVAL(x)      DRV_WriteReg32(CNTTVAL0, x)

typedef unsigned long long mt_time_t;
typedef void (*platform_timer_callback)(void *arg);

extern int g_systimer_wakeup;


extern void mdelay(unsigned long msec);
extern void udelay(unsigned long usec);
extern void mt_platform_systimer_init(void);
extern int platform_set_periodic_systimer(platform_timer_callback callback,
                                void *arg,
                                mt_time_t interval);
extern unsigned long long read_systimer_stamp_ns(void);
extern uint64_t timer_get_global_timer_tick(void);

#endif /* __MT_SYSTIMER_H__ */

