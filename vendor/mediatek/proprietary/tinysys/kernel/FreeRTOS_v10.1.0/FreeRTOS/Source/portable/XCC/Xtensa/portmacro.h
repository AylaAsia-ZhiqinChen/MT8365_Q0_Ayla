/*
    FreeRTOS V8.2.0 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ASSEMBLER__

#include <stdint.h>

#include <xtensa/tie/xt_core.h>
#include <xtensa/hal.h>
#include <xtensa/config/core.h>
#include <xtensa/config/system.h>   /* required for XSHAL_CLIB */
#include <xtensa/xtruntime.h>

//#include "xtensa_context.h"

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */

#define portCHAR        int8_t
#define portFLOAT       float
#define portDOUBLE      double
#define portLONG        int32_t
#define portSHORT       int16_t
#define portSTACK_TYPE  uint8_t
#define portBASE_TYPE   int

typedef portSTACK_TYPE          StackType_t;
typedef portBASE_TYPE           BaseType_t;
typedef unsigned portBASE_TYPE  UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
typedef uint16_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffff
#else
typedef uint32_t TickType_t;
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#define portTICK_TYPE_IS_ATOMIC 1
#endif
/*-----------------------------------------------------------*/

// portbenchmark
#include "portbenchmark.h"

/* Critical section management. NW-TODO: replace XTOS_SET_INTLEVEL with more efficient version, if any? */
// These cannot be nested. They should be used with a lot of care and cannot be called from interrupt level.
#define portDISABLE_INTERRUPTS()      do { XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL); portbenchmarkINTERRUPT_DISABLE(); } while (0)
#define portENABLE_INTERRUPTS()       do { portbenchmarkINTERRUPT_RESTORE(0); XTOS_SET_INTLEVEL(0); } while (0)
#define portASSERT_IF_IN_ISR()        configASSERT(!is_in_isr());

// These can be nested
#define portCRITICAL_NESTING_IN_TCB 1  // For now, let FreeRTOS' (tasks.c) manage critical nesting
extern UBaseType_t uxCriticalNesting;
void vTaskEnterCritical(void);
void vTaskExitCritical(void);
extern uint32_t is_in_isr(void);
#define portENTER_CRITICAL()        do { vTaskEnterCritical(); uxCriticalNesting++; } while(0)
#define portEXIT_CRITICAL()         do { uxCriticalNesting--; vTaskExitCritical(); } while(0)
// Cleaner and preferred solution allows nested interrupts disabling and restoring via local registers or stack.
// They can be called from interrupts too.
static inline unsigned portENTER_CRITICAL_NESTED() { unsigned state = XTOS_SET_INTLEVEL(XCHAL_EXCM_LEVEL); portbenchmarkINTERRUPT_DISABLE(); return state; }
#define portEXIT_CRITICAL_NESTED(state)   do { portbenchmarkINTERRUPT_RESTORE(state); XTOS_RESTORE_JUST_INTLEVEL(state); } while (0)

// These FreeRTOS versions are similar to the nested versions above
#define portSET_INTERRUPT_MASK_FROM_ISR()            portENTER_CRITICAL_NESTED()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(state)     portEXIT_CRITICAL_NESTED(state)

/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH            ( -1 )
#define portTICK_PERIOD_MS          ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT          128
#define portNOP()                   XT_NOP()
/*-----------------------------------------------------------*/

/* Fine resolution time */
#define portGET_RUN_TIME_COUNTER_VALUE()  xthal_get_ccount()

/* Kernel utilities. */
void vPortYield(void);
void _frxt_setup_switch(void);
#define portYIELD()                 vPortYield()
#define portYIELD_FROM_ISR(xSwitchRequired)     if( xSwitchRequired != pdFALSE ) _frxt_setup_switch()
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

extern void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime);
#define portSUPPRESS_TICKS_AND_SLEEP(xExpectedIdleTime) vPortSuppressTicksAndSleep(xExpectedIdleTime)

// When coprocessors are defined, we to maintain a pointer to coprocessors area.
// We currently use a hack: redefine field xMPU_SETTINGS in TCB block as a structure that can hold:
// MPU wrappers, coprocessor area pointer, trace code structure, and more if needed.
// The field is normally used for memory protection. FreeRTOS should create another general purpose field.
typedef struct {
    #if XCHAL_CP_NUM > 0
    volatile StackType_t* coproc_area; // Pointer to coprocessor save area; MUST BE FIRST
    #endif

#if portUSING_MPU_WRAPPERS
    // Define here mpu_settings, which is port dependent
    int mpu_setting; // Just a dummy example here; MPU not ported to Xtensa yet
#endif

#if configUSE_TRACE_FACILITY_2
    struct {
        // Cf. porttraceStamp()
        int taskstamp;        /* Stamp from inside task to see where we are */
        int taskstampcount;   /* A counter usually incremented when we restart the task's loop */
    } porttrace;
#endif
} xMPU_SETTINGS;

// Main hack to use MPU_wrappers even when no MPU is defined (warning: mpu_setting should not be accessed; otherwise move this above xMPU_SETTINGS)
#if (XCHAL_CP_NUM > 0 || configUSE_TRACE_FACILITY_2) && !portUSING_MPU_WRAPPERS   // If MPU wrappers not used, we still need to allocate coproc area
#undef portUSING_MPU_WRAPPERS
#define portUSING_MPU_WRAPPERS 1   // Enable it to allocate coproc area
#define MPU_WRAPPERS_H             // Override mpu_wrapper.h to disable unwanted code
#define PRIVILEGED_FUNCTION
#define PRIVILEGED_DATA
#endif

// porttrace
#if configUSE_TRACE_FACILITY_2
#include "porttrace.h"
#endif

// configASSERT_2 if requested
#if configASSERT_2
#include <stdio.h>
void exit(int);
#define configASSERT( x )   if (!(x)) { porttracePrint(-1); printf("\nAssertion failed in %s:%d\n", __FILE__, __LINE__); exit(-1); }
#endif


/* C library support -- currently only NEWLIB is supported. */

/* To enable thread-safe C library support, XT_USE_THREAD_SAFE_CLIB must be
   defined to be > 0 somewhere above or on the command line. */

#if (XT_USE_THREAD_SAFE_CLIB > 0u) && (XSHAL_CLIB == XTHAL_CLIB_NEWLIB)
extern void vPortClibInit(void);

// This C library cleanup is not currently done by FreeRTOS when deleting a task
#include <stdio.h>
#define portCLEAN_UP_TCB(pxTCB)   vPortCleanUpTcbClib(&((pxTCB)->xNewLib_reent))
static inline void vPortCleanUpTcbClib(struct _reent *ptr)
{
    FILE *fp = &(ptr->__sf[0]);
    int i;
    for (i = 0; i < 3; ++i, ++fp) {
        fp->_close = NULL;
    }
}
#endif // C library support

#if configUSE_TICKLESS_IDLE == 1
void check_tickless_flag (uint32_t tick_compensation);
#endif  /* configUSE_TICKLESS_IDLE == 1 */

#endif // __ASSEMBLER__

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

