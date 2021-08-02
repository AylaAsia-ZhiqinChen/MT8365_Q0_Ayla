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

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V port.
 *----------------------------------------------------------*/

/* Scheduler includes. */

#ifdef __clang__
#pragma clang diagnostic ignored "-Wuninitialized"
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "encoding.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#ifdef CFG_XGPT_SUPPORT
#include <xgpt.h>
#endif
#include "peripheral.h"
#include "irq.h"
#include "mt_printf.h"

/* Contains context when starting scheduler, save all 31 registers */
#ifdef __gracefulExit
BaseType_t xStartContext[F_reg+ACC_reg+UAM_reg+X_reg] = {0};
#endif

unsigned int __kstack_ext_reg = __kstack_ext_reg_value;
#ifdef __track_irq
int IRQ = 0;
#endif /* __track_irq */
/*
 * Handler for timer interrupt
 */
void vPortSysTickHandler( void );

/*
 * Setup the timer to generate the tick interrupts.
 */
void vPortSetupTimer( void );

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError( void );

/*-----------------------------------------------------------*/

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
#if configUSE_TICKLESS_IDLE == 1
	static uint32_t xMaximumPossibleSuppressedTicks = 0x7ffff;
#endif /* configUSE_TICKLESS_IDLE */

/*-----------------------------------------------------------*/
#if !defined(MRV)
/* Sets the next timer interrupt
 * Reads previous timer compare register, and adds tickrate */
static void prvSetNextTimerInterrupt(void)
{
    volatile uint64_t * mtime      = (uint64_t*) (CLINT_BASE_ADDR + CLINT_MTIME);
    volatile uint64_t * timecmp    = (uint64_t*) (CLINT_BASE_ADDR + CLINT_MTIMECMP);

    *timecmp = *mtime+(configTICK_CLOCK_HZ / configTICK_RATE_HZ);
}
#endif
/*-----------------------------------------------------------*/

/* Sets and enable the timer interrupt */
void vPortSetupTimer(void)
{
#ifndef CFG_ESL_BUILD
    vic_enable();
    platform_set_cpu_tick(DELAY_TIMER_1MS_TICK);
#else
    vic_enable();
    timer_disable();
    timer_clear_status();
    timer_set_current_value(0x4000); /* write manually in this timer version */
    timer_set_reload_value(0x4000);
    timer_enable();
#endif
    // Enable the Machine-External bit in MIE
    set_csr(mie, MIP_MEIP);

    // Enable the Machine-Timer bit in MIE
    set_csr(mie, MIP_MTIP);

    // Enable the Software interrupt bit in MIE
    set_csr(mie, MIP_MSIP);

    // Enable interrupts in general.
    //set_csr(mstatus, MSTATUS_MIE);
}
/*-----------------------------------------------------------*/

#pragma GCC diagnostic push  // require GCC 4.6
#pragma GCC diagnostic ignored "-Wmissing-prototypes"
void prvTaskExitError_pre( void )
{
    prvTaskExitError();
}
#pragma GCC diagnostic pop   // require GCC 4.6

void prvTaskExitError( void )
{
	/* A function that implements a task must not exit or attempt to return to
	its caller as there is nothing to return to.  If a task wants to exit it
	should instead call vTaskDelete( NULL ).

	Artificially force an assert() to be triggered if configASSERT() is
	defined, then stop here so application writers can catch the error. */
	configASSERT( xGetCriticalNesting() == ~0UL );
	portDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Clear current interrupt mask and set given mask */
void vPortClearInterruptMask(int mask)
{
	__asm volatile("csrw mie, %0"::"r"(mask));
}
/*-----------------------------------------------------------*/

/* Set interrupt mask and return current interrupt enable register */
int vPortSetInterruptMask(void)
{
	int ret;
	__asm volatile("csrr %0,mie":"=r"(ret));
	__asm volatile("csrc mie,%0"::"r"(0xff030888));
	return ret;
}
/*-----------------------------------------------------------*/


/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
	/* Simulate the stack frame as it would be created by a context switch
	interrupt. */

	register int *tp asm("x3");

	memset(pxTopOfStack-F_reg_ctx-ACC_reg_ctx-UAM_reg_ctx-X_reg_ctx, 0x0, (F_reg_ctx+ACC_reg_ctx+UAM_reg_ctx+X_reg_ctx) * 4);
	pxTopOfStack-= (F_reg_ctx+ACC_reg_ctx+UAM_reg_ctx);

#if defined(FF) || defined(FACC) || defined(FUAM)
	pxTopOfStack = ( StackType_t * ) ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) & ( ~( ( portPOINTER_SIZE_TYPE ) portBYTE_ALIGNMENT_MASK ) ) );
#endif

	pxTopOfStack--;						/* register mstatus */
#if defined(P_MODE_0) // task/RTOS in machine mode
	*pxTopOfStack = MSTATUS_MPP | MSTATUS_MPIE;
#elif defined(P_MODE_1) // task/RTOS in user mode
	*pxTopOfStack = MSTATUS_MPIE;
#endif /* P_MODE_0 */

#ifdef configExtension_F
	*pxTopOfStack |= MSTATUS_FS_C;
#endif /* configExtension_F */
#ifdef configExtension_ACC
	*pxTopOfStack |= MSTATUS_ACS_C;
#endif /* configExtension_ACC */
#ifdef configExtension_UAM
	*pxTopOfStack |= MSTATUS_UAS_C;
#endif /* configExtension_UAM */

	pxTopOfStack--;
	*pxTopOfStack = (portSTACK_TYPE)pxCode;			/* Start address */
	pxTopOfStack -= 22;
	*pxTopOfStack = (portSTACK_TYPE)pvParameters;		/* Register a0 */
	pxTopOfStack -= 6;
	*pxTopOfStack = (portSTACK_TYPE)tp; 			/* Register thread pointer */
	pxTopOfStack -= 3;
	*pxTopOfStack = (portSTACK_TYPE)prvTaskExitError;	/* Register ra */

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

UBaseType_t GetPendedTicks(void);
#ifndef CFG_ESL_BUILD
extern void timer_cpu_tick_irq_ack(void);
#endif

/******************************************************************************
* The SysTick runs at the lowest interrupt priority, so when this interrupt
* executes all interrupts must be unmasked.  There is therefore no need to
* save and then restore the interrupt mask value as its value is already
* known.
******************************************************************************/
void vPortSysTickHandler( void )
{
#ifdef CFG_XGPT_SUPPORT
    unsigned long long ullTime_ns;
    TickType_t ulTimerTick, ulSystemTick, ulDelta;
#endif

#if !defined(MRV)
    prvSetNextTimerInterrupt();
#endif

#ifdef CFG_XGPT_SUPPORT
    ullTime_ns = read_xgpt_stamp_ns();
    ulTimerTick = (TickType_t)(ullTime_ns / 1000000);
    ulSystemTick = xTaskGetTickCount() + (TickType_t)GetPendedTicks();

    /* 0x10000000 is a randomly chosen value.
     * Any value big enough to detect overflow can be used.
     */
    ulDelta = ulTimerTick - ulSystemTick;
    if (ulDelta > 0x10000000) {
        /* Supress system tick count from incrementing. */
        //PRINTF_E("SysTick: %u, TimerTick: %u\n", ulSystemTick, ulTimerTick);
    } else
#endif
    {
	/* Increment the RTOS tick. */
	if( xTaskIncrementTick() != pdFALSE )
	{
		vTaskSwitchContext();
	}
    }
#ifndef CFG_ESL_BUILD
    timer_cpu_tick_irq_ack();
#endif
}
/*-----------------------------------------------------------*/
extern int g_sleep_flag;

#ifdef CFG_XGPT_SUPPORT
#if configUSE_TICKLESS_IDLE == 1
static void sleep_wakeup_timer_fun(void *arg)
{
    /* null, only for wakeup */
    (void)*arg;
}
#endif
#endif
#define WAKEUP_TIMER1 16

#if configUSE_TICKLESS_IDLE == 1

__attribute__((weak)) void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
    uint32_t ulCompleteTickPeriods;
    TickType_t xModifiableIdleTime;
    TickType_t ullAbsCurTick, ullAbsPreTick, ullDiffTick;

    /* Make sure the SysTick reload value does not overflow the counter. */
    if( xExpectedIdleTime > xMaximumPossibleSuppressedTicks )
    {
        xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
    }

    /* Stop the SysTick momentarily.  The time the SysTick is stopped for
     * is accounted for as best it can be, but using the tickless mode will
     * inevitably result in some tiny drift of the time maintained by the
     * kernel with respect to calendar time. */
    platform_cpu_tick_disable();

    /* Enter a critical section but don't use the taskENTER_CRITICAL()
     * method as that will mask interrupts that should exit sleep mode. */
    vic_set_mask(0, 0x0);

    /* If a context switch is pending or a task is waiting for the scheduler
     * to be unsuspended then abandon the low power entry. */
    if( eTaskConfirmSleepModeStatus() == eAbortSleep )
    {
        /* Restart SysTick. */
        platform_cpu_tick_enable();
    } else {
        /* If xExpectedIdleTime less than MCU-wake-up time, it had better
         * loop rather than go to sleep.
         */
        if (xExpectedIdleTime > portMCU_WAKEUP_COST)
        {
            xModifiableIdleTime = xExpectedIdleTime;
            /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
             * set its parameter to 0 to indicate that its implementation contains
             * its own wait for interrupt or wait for event instruction, and so wfi
             * should not be executed again.  However, the original expected idle
             * time variable must remain unmodified, so a copy is taken. */
#ifdef CFG_WDT_SUPPORT
            mtk_wdt_disable();
#endif
#ifdef CFG_XGPT_SUPPORT
            platform_set_periodic_timer(sleep_wakeup_timer_fun, (void *) 0, xModifiableIdleTime);
#endif
#ifdef CFG_VCORE_DVFS_SUPPORT
            configPRE_SLEEP_PROCESSING( xModifiableIdleTime );
#endif
            mrv_dcache_barrier();
            __asm volatile( "wfi" );
            mrv_icache_barrier();

#ifdef CFG_VCORE_DVFS_SUPPORT
            configPOST_SLEEP_PROCESSING( xModifiableIdleTime );
#endif
#ifdef CFG_WDT_SUPPORT
            mtk_wdt_restart();
            mtk_wdt_enable();
#endif

            ullAbsPreTick = xTaskGetTickCountFromISR();
            /* absolute ticks (ms) from start */
            ullAbsCurTick = read_xgpt_stamp_ns() / 1000000;

            /* Time Guard:
             * Somehow xgpt gives reverse time which makes former given
             * time, xTickCount, begger than the latter given time. In this
             * case sync current time to previous time because it'd better
             * not modify the original tick count.
             */
            if (ullAbsPreTick > ullAbsCurTick) {
                /* if time reverse happens, the difference of previous tick and
                 * current tick would be very small, or otherwise current tick
                 * would be the value for overflow.
                 */
                if (ullAbsPreTick - ullAbsCurTick < portTIME_SKEW_FACTOR)
                    ullAbsCurTick = ullAbsPreTick;
            }

            ullDiffTick = ullAbsCurTick - ullAbsPreTick;

            /* mtime < difftime: sleep overhead makes current time exeeding the
             *                   expected time.
             * mtime > difftime: being disturbed by some interrupt events.
             * mtime = difftime: wake up on time, so minus one for ready tasks
             *                   to be selected by xTaskIncrementTick.
             */
            ulCompleteTickPeriods =
                (xModifiableIdleTime < ullDiffTick) ? xModifiableIdleTime :
                (xModifiableIdleTime > ullDiffTick) ? ullDiffTick :
                                                     ullDiffTick - 1;

            /* To step to xNextTaskUnblockTime in one go */
            vTaskStepTick( ulCompleteTickPeriods );

            /* The remain tick count says how many ticks exceed unblock time and
             * there must be some tasks who are needed to be waken up.
             */
            while (ulCompleteTickPeriods < ullDiffTick) {
                ulCompleteTickPeriods++;
                /* The increment of ticks is to only increase the variable,
                 * uxPendedTicks. The value will be taken in the following
                 * function, xTaskResumeAll, and then put ready tasks in ready
                 * queue.
                 */
                xTaskIncrementTick();
            }

            /* Clear interrupt and restart SysTick. */
            timer_cpu_tick_irq_ack();
        }

        /* Restart SysTick for sleep or busy loop cases in the else scope. */
        platform_cpu_tick_enable();
    }

    /* Re-enable interrupts - see comments above the cpsid instruction()
     * above.
     */
    vic_set_mask(0, 0xffff);

}

#endif /* #if configUSE_TICKLESS_IDLE */
/*-----------------------------------------------------------*/
