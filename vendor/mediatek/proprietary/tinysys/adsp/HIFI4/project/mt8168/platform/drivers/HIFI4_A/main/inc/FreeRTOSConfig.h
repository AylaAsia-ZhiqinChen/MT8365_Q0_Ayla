/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H


/* Required for configuration-dependent settings */
#include "xtensa_config.h"
#include <mt_printf.h>
/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * Note that the default heap size is deliberately kept small so that
 * the build is more likely to succeed for configurations with limited
 * memory.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *----------------------------------------------------------*/

#include <portmacro.h>

#define configUSE_PREEMPTION            1
#define configUSE_IDLE_HOOK              1

#define configUSE_TICK_HOOK             1
#ifndef configUSE_TICKLESS_IDLE
#define configUSE_TICKLESS_IDLE          1
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP 5
#endif

#ifndef __ASSEMBLER__
#ifndef configPRE_SLEEP_PROCESSING
extern void vPreSleepProcessing( TickType_t x );
#define configPRE_SLEEP_PROCESSING( x )   vPreSleepProcessing( x )
#endif
#ifndef configPOST_SLEEP_PROCESSING
extern void vPostSleepProcessing( TickType_t x );
#define configPOST_SLEEP_PROCESSING( x )  vPostSleepProcessing( x )
#endif

#ifdef CFG_MALLOC_TRACE
extern void vTraceMALLOC( void *pvAddress, size_t uiSize );
#define traceMALLOC( pvAddress, uiSize )    vTraceMALLOC( pvAddress, uiSize )
extern void vTraceFREE( void *pvAddress, size_t uiSize );
#define traceFREE( pvAddress, uiSize )      vTraceFREE( pvAddress, uiSize )
#endif
#endif

#ifdef CFG_FPGA
#define configCPU_CLOCK_HZ               ( 10000000 )
#define configTICK_RATE_HZ               ( 100 )
#else
#define configCPU_CLOCK_HZ               ( 400000000 )
#define configTICK_RATE_HZ               ( 1000 )
#endif

/* This has impact on speed of search for highest priority */
#define configMAX_PRIORITIES             ( 5 )

/* Minimal stack size. This may need to be increased for your application */
/* NOTE: The FreeRTOS demos may not work reliably with stack size < 4KB.  */
/* The Xtensa-specific examples should be fine with XT_STACK_MIN_SIZE.    */
/* NOTE: the size is defined in terms of StackType_t units not bytes */
#if !(defined XT_STACK_MIN_SIZE)
#error XT_STACK_MIN_SIZE not defined, did you include xtensa_config.h ?
#endif

#define configMINIMAL_STACK_SIZE        (1024 / sizeof(StackType_t)) //(XT_STACK_MIN_SIZE > 4096 ? XT_STACK_MIN_SIZE : 4096)

/* The Xtensa port uses a separate interrupt stack. Adjust the stack size */
/* to suit the needs of your specific application.                        */
/* NOTE: the size is defined in bytes. */
#ifndef configISR_STACK_SIZE
#define configISR_STACK_SIZE            2048
#endif

/* Minimal heap size to make sure examples can run on memory limited
   configs. Adjust this to suit your system. */
#ifndef configTOTAL_HEAP_SIZE
#define configTOTAL_HEAP_SIZE           ( ( size_t ) (256 * 1024) )
#endif

#define configMAX_TASK_NAME_LEN         ( 16 )
#define configUSE_TRACE_FACILITY        1       /* Used by vTaskList in main.c */
#define configUSE_TRACE_FACILITY_2      0       /* Provided by Xtensa port patch */
#define configBENCHMARK                 0       /* Provided by Xtensa port patch */
#define configUSE_16_BIT_TICKS          0
#define configIDLE_SHOULD_YIELD         1

#define configUSE_MUTEXES               1
//#define configQUEUE_REGISTRY_SIZE        8
#define configCHECK_FOR_STACK_OVERFLOW  2
#define configUSE_RECURSIVE_MUTEXES      1
#define configUSE_MALLOC_FAILED_HOOK     1
#define configUSE_APPLICATION_TASK_TAG   0
#define configUSE_COUNTING_SEMAPHORES   1
#define configGENERATE_RUN_TIME_STATS    0
#define configUSE_QUEUE_SETS                1

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES           0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Test FreeRTOS timers (with timer task) and more. */
/* Some files don't compile if this flag is disabled */
#define configUSE_TIMERS                    1
#define configTIMER_TASK_PRIORITY           2
#define configTIMER_QUEUE_LENGTH            10
#define configTIMER_TASK_STACK_DEPTH        (configMINIMAL_STACK_SIZE)
/* Set the following definitions to 1 to include the API function, or zero
   to exclude the API function. */

#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           1
#define INCLUDE_vTaskDelete                 1
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                1
#define INCLUDE_vTaskDelayUntil             1
#define INCLUDE_vTaskDelay                  1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1   /* Used by vTaskList in main.c */
#define INCLUDE_pcTaskGetTaskName               1
#define INCLUDE_xTimerPendFunctionCall          1

/* The priority at which the tick interrupt runs.  This should probably be
   kept at 1. */
#define configKERNEL_INTERRUPT_PRIORITY     1

/* The maximum interrupt priority from which FreeRTOS.org API functions can
   be called.  Only API functions that end in ...FromISR() can be used within
   interrupts. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    XCHAL_EXCM_LEVEL
/* XT_USE_THREAD_SAFE_CLIB is defined in xtensa_config.h and can be
   overridden from the compiler/make command line. The small test*/

#define configXT_SIMULATOR                  0   /* Simulator mode */
#define configXT_BOARD                      1   /* Board mode */

#if (!defined XT_INTEXC_HOOKS)
#define configXT_INTEXC_HOOKS               1   /* Exception hooks used by certain tests */
#endif

#if configUSE_TRACE_FACILITY_2
#define configASSERT_2                      1   /* Specific to Xtensa port */
#endif

#ifndef __ASSEMBLER__
#ifdef CFG_ASSERT_SUPPORT
extern void vAssertCalled(char *file, unsigned int line);
#define configASSERT( x ) do {	\
	if( ( x ) == 0) {	\
		vAssertCalled(__FILE__, __LINE__);	\
	}	\
} while(0)
#endif
#endif


/* The size of the global output buffer that is available for use when there
are multiple command interpreters running at once (for example, one on a UART
and one on TCP/IP).  This is done to prevent an output buffer being defined by
each implementation - which would waste RAM.  In this case, there is only one
command interpreter running. */
#define configCOMMAND_INT_MAX_OUTPUT_SIZE 512

#define DEBUGLEVEL 4

#define kal_xTaskCreate(x...) xTaskCreate(x)
#define kal_taskENTER_CRITICAL() taskENTER_CRITICAL()
#define kal_taskEXIT_CRITICAL() taskEXIT_CRITICAL()
#define kal_vTaskSuspendAll() vTaskSuspendAll()
#define kal_xTaskResumeAll() xTaskResumeAll()
#define kal_pvPortMalloc(x) pvPortMalloc(x)
#define kal_vPortFree(x) vPortFree(x)
#endif /* FREERTOS_CONFIG_H */

