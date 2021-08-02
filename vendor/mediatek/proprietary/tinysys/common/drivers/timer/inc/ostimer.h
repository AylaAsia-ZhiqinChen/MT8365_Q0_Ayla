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

#ifndef _OSTIMER_H_
#define _OSTIMER_H_

#include "mbox_pin.h"

/*
 * Shared MBOX: AP write, SSPM read
 * Unit for each offset: 4 bytes
 */

#define TIMESYNC_MBOX                   SHAREMBOX_NO_MCDI
#define TIMESYNC_MBOX_OFFSET_BASE       SHAREMBOX_OFFSET_TIMESTAMP

#define TIMESYNC_MBOX_TICK_H            (TIMESYNC_MBOX_OFFSET_BASE + 0)
#define TIMESYNC_MBOX_TICK_L            (TIMESYNC_MBOX_OFFSET_BASE + 1)
#define TIMESYNC_MBOX_TS_H              (TIMESYNC_MBOX_OFFSET_BASE + 2)
#define TIMESYNC_MBOX_TS_L              (TIMESYNC_MBOX_OFFSET_BASE + 3)
#define TIMESYNC_MBOX_DEBUG_TS_H        (TIMESYNC_MBOX_OFFSET_BASE + 4)
#define TIMESYNC_MBOX_DEBUG_TS_L        (TIMESYNC_MBOX_OFFSET_BASE + 5)

#define TIMESYNC_MAX_VER                (0x7)
#define TIMESYNC_HEADER_FREEZE_OFS      (31)
#define TIMESYNC_HEADER_FREEZE          (1U << TIMESYNC_HEADER_FREEZE_OFS)
#define TIMESYNC_HEADER_VER_OFS         (28)
#define TIMESYNC_HEADER_VER_MASK        (TIMESYNC_MAX_VER << TIMESYNC_HEADER_VER_OFS)
#define TIMESYNC_HEADER_MASK            (TIMESYNC_HEADER_FREEZE | TIMESYNC_HEADER_VER_MASK)

#define OSTIMER_CLK_DIV1                (1 << 0) /* 26MHz */
#define OSTIMER_CLK_DIV2                (1 << 1) /* 26/2 MHz */
#define OSTIMER_CLK_DIV3                (1 << 2) /* 26/4 MHz */
#define OSTIMER_CLK_DIV4                (0) /* 32KHz */

#define OSTIMER_CLK_DIV_SHIFT           4

enum {
    OSTIMER_DISABLE = 0,
    OSTIMER_ENABLE = 1,
};

struct sys_time_t {
    unsigned long long ts;
    unsigned long long off_ns;
    unsigned long long clk;
    bool insuspend;
};

unsigned long long ostimer_get_ns(void);
unsigned long      ostimer_low_cur_to_us(void);
unsigned long long ostimer_read(void);
unsigned long long ostimer_read_isr(bool isr_mode);

int                ts_apmcu_time(struct sys_time_t* ap_ts);
int                ts_apmcu_time_isr(struct sys_time_t* ap_ts);
void               ts_apmcu_sync(unsigned long long ts, unsigned long long gpt);
long long          ts_gpt_diff(unsigned long long gpt1, unsigned long long gpt2);
void               ts_resume_handle(unsigned long long ap_ts, unsigned long long ap_clk);
void               ts_suspend_handle(unsigned long long ap_ts, unsigned long long ap_clk);
void               ts_timesync_verify(void);
void               ts_timesync_print_base(void);
void               ts_update_sched_clock(void);

void               mdelay(unsigned int msec);
#ifndef CFG_INTERNAL_TIMER_SUPPORT
void               udelay(unsigned int usec);
#else
void               udelay(x) mdelay(x)
#endif

#ifdef CFG_ONDIEMET_SUPPORT
unsigned long long met_sched_clock(unsigned long long base_ts, unsigned long long base_clk, bool isr_mode,
                                   unsigned long long* ret_clk);
#endif
#endif

