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
#include "driver_api.h"
#include "timer.h"
#include "ostimer.h"
#include "irq.h"
#include <debug.h>
#include <FreeRTOS.h>
#include <task.h>
#ifdef CFG_ONDIEMET_SUPPORT
#include "ondiemet.h"
#endif

#define OSTIMER_CNT_EN      (1 << 0)
#define OSTIMER_CNT_IRQ_EN  (1 << 0)
#define OSTIMER_CNT_IRQ_ST  (1 << 4)
#define OSTIMER_CNT_IRQ_CLR (1 << 5)
#define OSTIMER_CLK_EN      (1 << 1)
#ifdef CFG_FPGA
#define OSTIMER_VAL_MSEC(x) (5662310 * ((x) / 1000))
#else
#define OSTIMER_VAL_MSEC(x) (27262976 * ((x) / 1000))
#endif
#define SYS_COUNT_TO_NS 76 //13MHz
#define MAX_TICK_COUNT 0xFFFFFFFF
struct clock_data {
    unsigned long long epoch_ns;
    unsigned long long epoch_cyc;
    unsigned int mult;
    unsigned int shift;
    unsigned int slave_mult;
    unsigned int slave_shift;
    bool insuspend;
};
static struct clock_data cd = {
    .epoch_ns = 0,
    .epoch_cyc = 0,
#ifdef CFG_FPGA
    /* 6000000's mult and shift*/
    .mult = 349525333,
    .shift = 21,
#else
    /*
     * AP System Timer uses 13MHz while AP is awake.
     * Use 13000000's mult and shift.
     */
    .mult = 161319385,
    .shift = 21,
#endif
    /*
     * AP System Timer has auto compensation mechanism in 32KHz zone
     * during system suspend, thus use 13000000's mult and shift as well.
     */
    .slave_mult = 161319385,
    .slave_shift = 21,
    .insuspend = 0,
};

/*
 * ostimer_read_isr (legacy version)
 *
 * Read low/high twice to cover boundary hw issue.
 */
unsigned long long ostimer_read_isr(bool isr_mode)
{
    unsigned long long val;
    unsigned long high_1, high_2, low_1, low_2;

    /*
     * must ensure atomicity from reading ostimer to calculating "val"
     * to prevent timestamp drift issue in print log.
     */
    if (0 == isr_mode)
        taskENTER_CRITICAL();

    low_1 = DRV_Reg32(OSTIMER_CUR_LOW);
    high_1 = DRV_Reg32(OSTIMER_CUR_HIGH);
    low_2 = DRV_Reg32(OSTIMER_CUR_LOW);
    high_2 = DRV_Reg32(OSTIMER_CUR_HIGH);

    if (low_2 < low_1) {
        /*
         * boundary condition
         */
        high_1 = high_2;
        low_1 = low_2;
    }

    val = (((unsigned long long)high_1 << 32) & 0xFFFFFFFF00000000) |
            ((unsigned long long)low_1 & 0x00000000FFFFFFFF);

    if (0 == isr_mode)
        taskEXIT_CRITICAL();

    return val;
}

unsigned long long ostimer_read(void)
{
    return ostimer_read_isr(0);
}
unsigned long long ostimer_get_ns(void)
{
    unsigned long long val, val_ns;
    val = ostimer_read();
    val_ns = val * SYS_COUNT_TO_NS;
    return val_ns;
}
unsigned long ostimer_low_cur_to_ns(void)
{
    return (unsigned long)(DRV_Reg32(OSTIMER_CUR_LOW) * SYS_COUNT_TO_NS);
}
static inline unsigned long long cyc_to_ns(unsigned long long cyc, unsigned int mult, unsigned int shift)
{
    return (cyc * mult) >> shift;
}
static unsigned long long __common_sched_clock(unsigned long long base_ts, unsigned long long base_clk, bool isr_mode,
        unsigned long long* ret_clk)
{
    unsigned long long epoch_ns;
    unsigned long long epoch_cyc;
    unsigned long long cyc;
    if (0 == isr_mode)
        taskENTER_CRITICAL();
    if (0 == base_ts || 0 == base_clk) {
        epoch_cyc = cd.epoch_cyc;
        epoch_ns = cd.epoch_ns;
    } else {
        epoch_cyc = base_clk;
        epoch_ns = base_ts;
    }
    if (0 == isr_mode)
        taskEXIT_CRITICAL();
    cyc = ostimer_read_isr(isr_mode);
    if (ret_clk)
        *ret_clk = cyc;
    cyc = cyc - epoch_cyc;
    return epoch_ns + cyc_to_ns(cyc, cd.mult, cd.shift);
}
#ifdef CFG_ONDIEMET_SUPPORT
unsigned long long met_sched_clock(unsigned long long base_ts, unsigned long long base_clk, bool isr_mode,
                                   unsigned long long* ret_clk)
{
    return __common_sched_clock(base_ts, base_clk, isr_mode, ret_clk);
}
#endif
static unsigned long long __sched_clock(bool isr_mode, unsigned long long* ret_clk)
{
    if (cd.insuspend) {
        /* switch to MD32 internal timer */
        return cd.epoch_ns;
    }
    return __common_sched_clock(0, 0, isr_mode, ret_clk);
}
/*
 * update_sched_clock:
 * to update timestamp, it will always be updated @ AP suspend
 */
void update_sched_clock(void)
{
    unsigned long long cyc;
    unsigned long long ns;
    if (cd.insuspend)
        return;
    /* AP suspend in task mode, so call pass non-isr mode to read_sched_clock */
    cyc = ostimer_read_isr(0);
    ns = cd.epoch_ns + cyc_to_ns(cyc - cd.epoch_cyc,
                                 cd.mult, cd.shift);
    taskENTER_CRITICAL();
    cd.epoch_ns = ns;
    cd.epoch_cyc = cyc;
    taskEXIT_CRITICAL();
    return;
}
void ts_apmcu_sync(unsigned long long ns, unsigned long long clk)
{
    taskENTER_CRITICAL();
    cd.epoch_ns = ns;
    cd.epoch_cyc = clk;
    taskEXIT_CRITICAL();
}
long long ts_gpt_diff(unsigned long long gpt1, unsigned long long gpt2)
{
    if (gpt1 > gpt2)
        return (long long)cyc_to_ns(gpt1 - gpt2, cd.mult, cd.shift);
    else
        return -(long long)cyc_to_ns(gpt2 - gpt1, cd.mult, cd.shift);
}
void ts_suspend_handle(unsigned long long ap_ts, unsigned long long ap_clk)
{
    unsigned long long cyc = 0;
#ifdef CFG_ONDIEMET_SUPPORT
    if (met_running_state())
        met_apmcu_suspend_handler(ap_ts, ap_clk);
#endif
    /*update latest 13m clock*/
    update_sched_clock();
    cd.insuspend = 1;
    /* dump suspend debug message */
    /* clock source switch to 32k*/
    cyc = ostimer_read();
    taskENTER_CRITICAL();
    cd.epoch_cyc = cyc;
    taskEXIT_CRITICAL();
}
void ts_resume_handle(unsigned long long ap_ts, unsigned long long ap_clk)
{
#ifdef CFG_ONDIEMET_SUPPORT
    if (met_running_state())
        met_apmcu_resume_handler(ap_ts, ap_clk);
#endif
    cd.insuspend = 0;
    /* clock source switch to 13m*/
    ts_apmcu_sync(ap_ts, ap_clk);
    /* dump resume debug message */
}
static int __ts_apmcu_time(struct sys_time_t* ap_ts, bool isr_mode)
{
    unsigned long long clk;
    if (cd.insuspend) {
        /*suspend handle*/
        ap_ts->ts = __sched_clock(isr_mode, NULL);
        clk = ostimer_read_isr(isr_mode);
        /*update timestamp by OStimer clock*/
        ap_ts->off_ns = cyc_to_ns(clk - cd.epoch_cyc,
                                  cd.slave_mult, cd.slave_shift);
        ap_ts->clk = clk;
        ap_ts->insuspend = 1;
    } else {
        ap_ts->ts = __sched_clock(isr_mode, &clk);
        ap_ts->clk = clk;
        ap_ts->off_ns = 0;
        ap_ts->insuspend = 0;
    }
    return ap_ts->insuspend;
}
/*
 * ts_apmcu_time()/ts_apmcu_time_isr():
 * return 1: apmcu in suspend state
 * return 0: apmcu in operation state
 *
 * if ap in suspend mode:
 * .ts = ap's latest suspend timestamp
 * .off_ns = nano second offset start from latest suspend timestamp
 * .insuspend = 1
 *
 * if ap in operation mode:
 * .ts = timestamp align with ap
 * .off_ns = 0
 * .insuspend = 0
 */
int ts_apmcu_time(struct sys_time_t* ap_ts)
{
    return __ts_apmcu_time(ap_ts, 0);
}
int ts_apmcu_time_isr(struct sys_time_t* ap_ts)
{
    return __ts_apmcu_time(ap_ts, 1);
}
#define GPT_BIT_MASK_L 0x00000000FFFFFFFF
#define GPT_BIT_MASK_H 0xFFFFFFFF00000000
#ifdef _OSTIMER_32kHZ_
#define US_LIMIT 31 /* udelay's parameter limit*/
#define DELAY_TIMER_1US_TICK       ((unsigned int)1)   /*(32KHz)*/
#define DELAY_TIMER_1MS_TICK       ((unsigned int)33) /*(32KHz)*/
/*32KHz: 31us = 1 counter*/
#define TIME_TO_TICK_US(us) ((us)*DELAY_TIMER_1US_TICK)
/*32KHz: 1ms = 33 counter*/
#define TIME_TO_TICK_MS(ms) ((ms)*DELAY_TIMER_1MS_TICK)
#else
#ifdef CFG_FPGA
#define DELAY_TIMER_1US_TICK       ((unsigned int)6)    /*(6MHz)*/
#define DELAY_TIMER_1MS_TICK       ((unsigned int)6000) /*(6MHz)*/
#define TIME_TO_TICK_US(us) ((us)*DELAY_TIMER_1US_TICK)
#define TIME_TO_TICK_MS(ms) ((ms)*DELAY_TIMER_1MS_TICK)
#else
/* Use System Timer (13MHz) as delay timer source */
#define DELAY_TIMER_1US_TICK       ((unsigned int)13)    /*(13MHz)*/
#define DELAY_TIMER_1MS_TICK       ((unsigned int)13000) /*(13MHz)*/
#define TIME_TO_TICK_US(us) ((us)*DELAY_TIMER_1US_TICK)
#define TIME_TO_TICK_MS(ms) ((ms)*DELAY_TIMER_1MS_TICK)
#endif
#endif
static unsigned int read_ostimer_low_cnt(void)
{
    return DRV_Reg32(OSTIMER_CUR_LOW);
}
static int check_timeout_tick(unsigned int start_tick, unsigned int timeout_tick)
{
    unsigned int cur_tick;
    unsigned int elapse_tick;
    cur_tick = read_ostimer_low_cnt();
    if (start_tick <= cur_tick)
        elapse_tick = cur_tick - start_tick;
    else
        elapse_tick = (MAX_TICK_COUNT - start_tick) + cur_tick;
    /*check if timeout*/
    if (timeout_tick <= elapse_tick) {
        return 1;
    }
    return 0;
}
static unsigned int time2tick_us(unsigned int time_us)
{
    return TIME_TO_TICK_US(time_us);
}
static unsigned int time2tick_ms(unsigned int time_ms)
{
    return TIME_TO_TICK_MS(time_ms);
}
//==============================================
// busy wait
//==============================================
static void busy_wait_us(unsigned int timeout_us)
{
    unsigned int start_tick, timeout_tick;
    // get timeout tick
    timeout_tick = time2tick_us(timeout_us);
    start_tick = read_ostimer_low_cnt();
    // wait for timeout
    while (!check_timeout_tick(start_tick, timeout_tick));
}
static void busy_wait_ms(unsigned int timeout_ms)
{
    unsigned int start_tick, timeout_tick;
    // get timeout tick
    timeout_tick = time2tick_ms(timeout_ms);
    start_tick = read_ostimer_low_cnt();
    // wait for timeout
    while (!check_timeout_tick(start_tick, timeout_tick));
}
/* delay msec mseconds, can't delay time bigger than 2.5 min */
void mdelay(unsigned int msec)
{
    busy_wait_ms(msec);
}
/* delay usec useconds, can't delay time bigger than 2.5 min */
void udelay(unsigned int usec)
{
#ifdef _OSTIMER_32kHZ_
    unsigned int usec_t;
    if (usec < US_LIMIT) {
        busy_wait_us(1);
    } else {
        usec_t = usec / 31 + 1;
        busy_wait_us(usec_t);
    }
#else
    busy_wait_us(usec);
#endif
}

/*
 * dummy functions
 *
 * ts_timesync* functions are added since timesync v2 but
 * referenced by sspm common platform code, thus we add
 * dummy body here (timesync v1).
 */
void ts_timesync_print_base(void)
{
    return;
}

void ts_timesync_verify(void)
{
    return;
}
