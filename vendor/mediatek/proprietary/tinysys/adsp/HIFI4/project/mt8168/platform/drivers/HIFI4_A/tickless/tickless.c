/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2018. All rights reserved.
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

#include <FreeRTOS.h>
#include <systimer.h>
#include <task.h>
#include <xtensa/tie/xt_interrupt.h>

#if ( configUSE_TICKLESS_IDLE == 1 )

#define TIMER_COUNT_FOR_ONE_TICK    (DELAY_TIMER_1MS_TICK * portTICK_RATE_MS)
#define MAX_IDLE_TICKS              (MAX_SYSTIMER_COUNT / TIMER_COUNT_FOR_ONE_TICK)

/*
 * The number of SysTimer increments that make up one tick period.
 */
static uint32_t ulTimerCountsForOneTick = TIMER_COUNT_FOR_ONE_TICK;

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 32 bit resolution of the SysTimer.
 */
static uint32_t xMaximumPossibleSuppressedTicks = MAX_IDLE_TICKS;

/*
 * Compensate for the CPU cycles that pass while the SysTimer is stopped
 * (low power functionality only).
 * Which should be assumed later by measurement or estimation???
 * 'Big Data Analysis'
 */
static uint32_t ulStoppedTimerCompensation1 = 26;
static uint32_t ulStoppedTimerCompensation2 = 13;



void vPreSleepProcessing( TickType_t x )
{
}

void vPostSleepProcessing( TickType_t x )
{
}

void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
    uint32_t ulReloadValue;
    uint32_t ulRealSleepCount;
    uint32_t ulCompleteTickPeriods;
    uint32_t ulStopCount;
    unsigned long long val_start;
    unsigned long long val_end;
    unsigned long long elapsed_detla;
    unsigned long long residual;
    int systimer_wakeup;
    TickType_t xModifiableIdleTime;

    portDISABLE_INTERRUPTS();
    if (eTaskConfirmSleepModeStatus() == eAbortSleep)
    {
        portENABLE_INTERRUPTS();
        return;
    }

    /* Clear TICKLESS_STATUS_REG first */
    DRV_WriteReg32(TICKLESS_STATUS_REG, (0x00));

    if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks)
        xExpectedIdleTime = xMaximumPossibleSuppressedTicks;

    /* Calc new reload value for systimer0 */
    SYSTIMER_DISABLE;
    ulStopCount = SYSTIMER_GET_COUNTER;
    val_start = timer_get_global_timer_tick();

    xModifiableIdleTime = xExpectedIdleTime - 1;
    ulRealSleepCount = ulTimerCountsForOneTick * xModifiableIdleTime;
    ulRealSleepCount -= ulStoppedTimerCompensation1;
    ulReloadValue = ulRealSleepCount + ulStopCount;

#ifdef CFG_TICKLESS_DEBUG
    PRINTF_D("Tickless\n");
#endif
    SYSTIMER_ENABLE;
    SYSTIMER_SET_RSTVAL(ulReloadValue);
    SYSTIMER_ENABLE_IRQ;

    g_systimer_wakeup = 0;
    DRV_SetReg32(TICKLESS_STATUS_REG, (0x1 << 0) | (0x1 << 1));

    configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
    if (xModifiableIdleTime > 0)
    {
        XT_MEMW();
        XT_WAITI(0);
    }

    portDISABLE_INTERRUPTS();
    configPOST_SLEEP_PROCESSING(xExpectedIdleTime);

    if (g_systimer_wakeup == 1)
    {
        systimer_wakeup = 1;
        ulCompleteTickPeriods = xModifiableIdleTime;
        g_systimer_wakeup = 0;
    }
    else
    {
        systimer_wakeup = 0;
        SYSTIMER_DISABLE;
        val_end = timer_get_global_timer_tick();
        elapsed_detla = val_end - val_start + (ulTimerCountsForOneTick - ulStopCount);
        ulCompleteTickPeriods = elapsed_detla / ulTimerCountsForOneTick;
        residual = (ulCompleteTickPeriods + 1) * ulTimerCountsForOneTick - elapsed_detla;
        residual = (residual > ulStoppedTimerCompensation2) \
                        ? (residual - ulStoppedTimerCompensation2) : 1;
        SYSTIMER_ENABLE;
        SYSTIMER_SET_RSTVAL(residual);
        SYSTIMER_ENABLE_IRQ;
    }
    portENABLE_INTERRUPTS();

    taskENTER_CRITICAL();
    vTaskStepTick(ulCompleteTickPeriods);
    taskEXIT_CRITICAL();

    if (systimer_wakeup)
    {
        DRV_ClrReg32(TICKLESS_STATUS_REG, (0x1 << 0));
    #ifdef CFG_TICKLESS_DEBUG
        PRINTF_D("wakeup by systimer\n");
    #endif
    }
    else
    {
        DRV_ClrReg32(TICKLESS_STATUS_REG, (0x1 << 1));
    #ifdef CFG_TICKLESS_DEBUG
        PRINTF_D("wakeup by others\n");
    #endif
    }
}
#endif /* configUSE_TICKLESS_IDLE */

