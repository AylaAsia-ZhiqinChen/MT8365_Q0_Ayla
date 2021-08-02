/*
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
#include <driver_api.h>
#include <interrupt.h>
#include <FreeRTOS.h>
#include <systimer.h>


struct timer_device {
    unsigned int id;
    unsigned int base_addr;
    unsigned int irq_id;
};

static platform_timer_callback tick_timer_callback;
static void *tick_timer_callback_arg;
static unsigned long long ostimer_init_cycle;
static unsigned long long periodic_interval;
int g_systimer_wakeup = 0;
//static uint64_t previous_systimer_int_time;


/*
 * prototype
 */
static void __timer_enable_irq(struct timer_device *dev);
static void __timer_ack_irq(struct timer_device *dev);
static void __timer_enable(struct timer_device *dev);
static void __timer_disable(struct timer_device *dev);
static void __timer_set_rstval(struct timer_device *dev, unsigned int val);
/* get ostimer counter */
static void get_ostimer_counter_high_low(unsigned int *high, unsigned int *low);
static unsigned long long get_boot_time_ns(void);
static void tick_systimer_irq(void *arg);
static void tick_systimer_irq_handle(void);

//===========================================================================
// delay function
//===========================================================================
static int check_timeout_tick(unsigned long long start_tick,
                              unsigned long timeout_tick);
static unsigned long time2tick_us(unsigned int time_us);
static unsigned long time2tick_ms(unsigned int time_ms);

//===========================================================================
// busy wait
//===========================================================================
static void busy_wait_us(unsigned int timeout_us);
static void busy_wait_ms(unsigned int timeout_ms);


//===========================================================================

static void __timer_enable_irq(struct timer_device *dev)
{
    DRV_WriteReg32(CNTTVAL0_CON, CNTIRQ_EN | CNTTVAL_EN);
}

static void __timer_ack_irq(struct timer_device *dev)
{
    /* systimer must clear IRQ-ENABLE bit first */
    DRV_WriteReg32(CNTTVAL0_CON, (CNTTVAL_EN | CNTIRQ_STACLR));
}

static void __timer_enable(struct timer_device *dev)
{
    DRV_WriteReg32(CNTTVAL0_CON, CNTTVAL_EN);
}

static void __timer_disable(struct timer_device *dev)
{
    DRV_ClrReg32(CNTTVAL0_CON, CNTTVAL_EN);
}

static void __timer_set_rstval(struct timer_device *dev, unsigned int val)
{
    DRV_WriteReg32(CNTTVAL0, val);
}


/* get ostimer counter from Hifi4dsp poweron */
uint64_t timer_get_global_timer_tick(void)
{
    unsigned long high = 0, low = 0;
    UBaseType_t uxSavedInterruptStatus;

    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    /*must first read low, and read high later*/
    low = DRV_Reg32(CNTCV_L);
    high = DRV_Reg32(CNTCV_H);
    portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus);

    return (uint64_t)low | (uint64_t)high << 32;
}

/* get ostimer counter */
static void get_ostimer_counter_high_low(unsigned int *high, unsigned int *low)
{
    UBaseType_t uxSavedInterruptStatus;
    uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
    *low = DRV_Reg32(CNTCV_L);
    *high = DRV_Reg32(CNTCV_H);
    portCLEAR_INTERRUPT_MASK_FROM_ISR(uxSavedInterruptStatus);
}

/* get ostimer timestamp from Hifi4dsp bootup */
static unsigned long long get_boot_time_ns(void)
{
    /* multiplier and shifter for 13MHz global timer */
#define OSTIMER_TIMER_MULT  (161319385)
#define OSTIMER_TIMER_SHIFT (21)
    unsigned int high = 0, low = 0;
    unsigned long long new_high = 0, new_low = 0;
    unsigned long long cycle = 0;
    unsigned long long timestamp = 0;

    get_ostimer_counter_high_low(&high, &low);
    new_high = high;
    new_low = low;
    cycle = ((new_high << 32) & 0xFFFFFFFF00000000ULL) |
                (new_low & 0x00000000FFFFFFFFULL);
    cycle = cycle - ostimer_init_cycle;
    new_high = (cycle >> 32) & 0x00000000FFFFFFFFULL;
    new_low = cycle & 0x00000000FFFFFFFFULL;
    timestamp = (((unsigned long long)new_high * OSTIMER_TIMER_MULT) << 11) +
                (((unsigned long long)new_low * OSTIMER_TIMER_MULT) >> OSTIMER_TIMER_SHIFT);
    return timestamp;
}

unsigned long long read_systimer_stamp_ns(void)
{
    if (ostimer_init_cycle == 0)
        ostimer_init_cycle = timer_get_global_timer_tick();
    return get_boot_time_ns();
}


NORMAL_SECTION_FUNC void mt_platform_systimer_init(void)
{
    if (ostimer_init_cycle == 0)
        ostimer_init_cycle = timer_get_global_timer_tick();
    PRINTF_I("[systimer] ostimer_init_cycle = 0x%llX at %dMHz.\n", ostimer_init_cycle, DELAY_TIMER_1US_TICK);
}

static void tick_systimer_irq(void *arg)
{
    /*diff with before*/
    if (tick_timer_callback != NULL) {
        return tick_timer_callback(tick_timer_callback_arg);
    }
}

static void tick_systimer_irq_handle(void)
{
    struct timer_device *dev = NULL;
    unsigned int new_val = periodic_interval;

    /* clear irq status bit */
    __timer_ack_irq(dev);

    /*
     * --->>> Tick Compensation <<<---
     * Excuting time before setting a new value also reduces the delta,
     * unsigned int delta = ???
     */
#if 0
    unsigned int delta = 30;
    uint64_t current_int_time;
    uint64_t two_int_time_diff;
    current_int_time = timer_get_global_timer_tick();
    two_int_time_diff = current_int_time - previous_systimer_int_time;
    previous_systimer_int_time = current_int_time;
    if (two_int_time_diff < 2 * DELAY_TIMER_1MS_TICK
        && two_int_time_diff > DELAY_TIMER_1MS_TICK)
        new_val = (periodic_interval * 2) - two_int_time_diff - delta;
#endif
    /* program next event for one-shot timer */
    //__timer_enable(dev);
    __timer_set_rstval(dev, new_val);
    __timer_enable_irq(dev);

    /* set flag for tickless check */
    g_systimer_wakeup = 1;

    /* handler: xPortSysTickHandler() */
    tick_systimer_irq(NULL);
}

NORMAL_SECTION_FUNC
int platform_set_periodic_systimer(platform_timer_callback callback,
                                void *arg,
                                mt_time_t interval)
{
    struct timer_device *dev;
    unsigned long long tmp_64 = 0;
    unsigned long long interval_tmp = (unsigned long long)interval;

    tick_timer_callback = callback;
    tick_timer_callback_arg = arg;

    /* calculate timeout interval for periodic schedule */
    tmp_64 = time2tick_ms(interval_tmp);
    periodic_interval = tmp_64;

    dev = NULL;
    __timer_disable(dev);

    request_irq(SYSTICK_TIMER_IRQ, tick_systimer_irq_handle, "tick_timer");
    __timer_enable(dev);

    if (interval >= 1) {
        __timer_set_rstval(dev, (unsigned int)tmp_64);    //0.3ms(sw)+0.7ms(hw wake)
    }
    else {
        __timer_set_rstval(dev, 1);
    }
    __timer_enable_irq(dev);

    return 0;
}


//===========================================================================
// delay function
//===========================================================================
static int check_timeout_tick(unsigned long long start_tick,
                              unsigned long timeout_tick)
{
    unsigned long long cur_tick;
    unsigned long long elapse_tick;

    // get current tick
    cur_tick = timer_get_global_timer_tick();

    // check elapse time, down counter
    elapse_tick = cur_tick - start_tick;

    // check if timeout
    if ((unsigned long long)timeout_tick <= elapse_tick) {
        // timeout
        return 1;
    }

    return 0;
}

static unsigned long time2tick_us(unsigned int time_us)
{
    return TIME_TO_TICK_US(time_us);
}

static unsigned long time2tick_ms(unsigned int time_ms)
{
    return TIME_TO_TICK_MS(time_ms);
}

static void busy_wait_us(unsigned int timeout_us)
{
    unsigned long timeout_tick;
    unsigned long long start_tick;

    // get timeout tick
    timeout_tick = time2tick_us(timeout_us);
    start_tick = timer_get_global_timer_tick();

    // wait for timeout
    while (!check_timeout_tick(start_tick, timeout_tick));
}

static void busy_wait_ms(unsigned int timeout_ms)
{
    unsigned long timeout_tick;
    unsigned long long start_tick;

    // get timeout tick
    timeout_tick = time2tick_ms(timeout_ms);
    start_tick = timer_get_global_timer_tick();

    // wait for timeout
    while (!check_timeout_tick(start_tick, timeout_tick));
}

//===========================================================================
// busy wait
//===========================================================================

/* delay msec mseconds */
void mdelay(unsigned long msec)
{
    busy_wait_ms(msec);
}

/* delay usec useconds */
void udelay(unsigned long usec)
{
    busy_wait_us(usec);
}

