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

#ifndef _PORTMACRO_H_
#define _PORTMACRO_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stdbool.h>
#include <intc.h>

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */
#ifndef traceISR_ENTER
        #define traceISR_ENTER()
#endif

#ifndef traceISR_EXIT
        #define traceISR_EXIT()
#endif

#ifndef traceINT_OFF
#define traceINT_OFF()
#define traceINT_OFF_FROM_ISR()
#endif

#ifndef traceINT_ON
#define traceINT_ON()
#define traceINT_ON_FROM_ISR()
#endif

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portBASE_TYPE	long

#if __riscv_xlen == 64
	#define portSTACK_TYPE	uint64_t
	#define portPOINTER_SIZE_TYPE	uint64_t
#else
	#define portSTACK_TYPE	uint32_t
	#define portPOINTER_SIZE_TYPE	uint32_t
#endif

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) (1000 / configTICK_RATE_HZ) )

/* for newlib floating point printing, need 8-byte alignment to convert to double precision floating temporalily */
#define portBYTE_ALIGNMENT		16
#if 0
#if __riscv_xlen == 64
	#define portBYTE_ALIGNMENT	8
#else
	#define portBYTE_ALIGNMENT	4
#endif
#endif
#define portCRITICAL_NESTING_IN_TCB					1
/*-----------------------------------------------------------*/

#include "syscalls.h"
/* Scheduler utilities. */
extern void vPortYield( void );
//#define portYIELD()	vPortYield()
#if defined(P_MODE_0) // task/RTOS in machine mode
#define portYIELD()	__asm volatile( "csrrwi x0,0x5c1,1" )
#elif defined(P_MODE_1) // task/RTOS in user mode
#define portYIELD()	syscall(SYS_yield, 0, 0, 0)
#endif /* P_MODE_0 */

#define vPortYieldFromISR() __asm volatile( "csrrwi x0,0x5c1,1" )
#define portEND_SWITCHING_ISR( xSwitchRequired ) if( xSwitchRequired ) vPortYieldFromISR()
#define portYIELD_FROM_ISR( x ) portEND_SWITCHING_ISR( x )
/*-----------------------------------------------------------*/


/* Critical section management. */
extern int vPortSetInterruptMask( void );
extern void vPortClearInterruptMask( int );
extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );

#if defined(P_MODE_0) // task/RTOS in machine mode
#define portDISABLE_INTERRUPTS()				__asm volatile 	( "csrc mie,%0\n"::"r"(0xff030888):);
#define portENABLE_INTERRUPTS()					__asm volatile 	( "csrs mie,%0\n"::"r"(0xff030888):);
#elif defined(P_MODE_1) // task/RTOS in user mode
#define portDISABLE_INTERRUPTS()				syscall(SYS_disable_int, 0, 0, 0)
#define portENABLE_INTERRUPTS()					syscall(SYS_enable_int, 0, 0, 0)
#endif /* P_MODE_0 */

#define portENTER_CRITICAL()					{ if (!is_in_isr()) vTaskEnterCritical(); }
#define portEXIT_CRITICAL()					{ if (!is_in_isr()) vTaskExitCritical(); }
#define portASSERT_IF_IN_ISR()					configASSERT(!is_in_isr());

#define portSET_INTERRUPT_MASK_FROM_ISR()			vPortSetInterruptMask()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedStatusValue )	vPortClearInterruptMask( uxSavedStatusValue )
/* hw disable/enable mie automatically when isr in and out,
 * so no need to concider switching on/off after mask setup */
#define portCLEAR_INTERRUPT_MASK_FROM_ISR_AND_LEAVE( x )        portCLEAR_INTERRUPT_MASK_FROM_ISR( x )
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

/* Tickless idle/low power functionality. */
#ifndef portSUPPRESS_TICKS_AND_SLEEP
        extern void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime );
        #define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime ) vPortSuppressTicksAndSleep( xExpectedIdleTime )
#endif
/*-----------------------------------------------------------*/

#define portNOP() __asm volatile 	( " nop " )

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
