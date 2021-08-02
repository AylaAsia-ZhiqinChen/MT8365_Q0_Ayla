/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2019. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

#include <xtensa/config/core.h>
#include <xtensa/hal.h>

#include "main.h"
#include "interrupt.h"
#include <driver_api.h>
#include "auto_version.h"

#ifdef CFG_MTK_HEAP_SUPPORT
#include <mtk_heap.h>
#endif
#ifdef CFG_UART_SUPPORT
#include <mt_uart.h>
#endif
#include <adsp_excep.h>
#ifdef CFG_LOGGER_SUPPORT
#include <logger.h>
#endif
#ifdef CFG_WDT_SUPPORT
#include "wdt.h"
#endif
#ifdef CFG_VCORE_DVFS_SUPPORT
#include <dvfs.h>
#endif
#ifdef CFG_DMA_SUPPORT
#include <dma.h>
#endif
#ifdef CFG_TRAX_SUPPORT
#include <adsp_trax.h>
#endif
#ifdef CFG_CACHE_SUPPORT
#include "hal_cache.h"
#endif
#ifdef CFG_WAKELOCK_SUPPORT
#include "wakelock.h"
#endif
#ifdef CFG_AP_AWAKE_SUPPORT
#include "ap_awake.h"
#endif
#ifdef CFG_SYSTIMER_SUPPORT
#include "systimer.h"
#endif

/* Global variables ---------------------------------------------------------*/


/* Private functions ---------------------------------------------------------*/
static void prvSetupHardware(void);

/* Extern functions ---------------------------------------------------------*/
extern void adsp_ipi_init(void);
#if defined(CFG_AUDIO_DEMO)
extern void audio_process_init(void);
#endif
BaseType_t xPortSysTickHandler( void );
#ifdef CFG_MTK_PDCT_SUPPORT
extern int pdct_f0(void);
#endif

#ifdef CFG_XTENSA_ERRATUM_572
static inline void disable_l1_loop_buffer(void)
{
    uint32_t memctl;

    /* Erratum 572: Processor may incorrectly fetch instructions from
     *          Zero Overhead Loop Buffer for a single iteration loop
     * Solution: Turn off the Zero Overhead Loop Buffer by disabling the
     *          loop buffer at run time
     * https://support.tensilica.com/Support/html/Docs/Errata_SFDC/errata.asp?id=572&fam=RG&Diamond=&rev=&go=T
     */
    memctl = XT_RSR_MEMCTL();
    memctl &= ~((uint32_t)1);
    XT_WSR_MEMCTL(memctl);
}
#endif

#ifdef CFG_SYSTIMER_SUPPORT
NORMAL_SECTION_FUNC void vPortSetupTimerInterrupt( void )
{
    platform_set_periodic_systimer((void *)xPortSysTickHandler, NULL, portTICK_PERIOD_MS);
}
#endif

/**
 * PROHIBIT using any uninitialized global variable (bss section).
 * __memmap_init() is invoked by _start in crt1_board.S
 */
void __memmap_init(void)
{
#ifdef CFG_XTENSA_ERRATUM_572
    disable_l1_loop_buffer();
#endif
#ifdef CFG_MPU_SUPPORT
    mpu_load_default_region_config();
#endif
}

/**
 * board_init() is invoked by _start in crt1_board.S
 */
void board_init(void)
{
#ifdef CFG_LOGGER_SUPPORT
    /* logger_init must before uart init */
    logger_init();
#endif
#ifdef CFG_UART_SUPPORT
    /* init UART before any printf function all */
    uart_early_init(UART_LOG_PORT, UART_LOG_BAUD_RATE, UART_SRC_CLK);
#endif
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    /* start to use UART */
    PRINTF_E("FreeRTOS %s %x, %s\n\r", tskKERNEL_VERSION_NUMBER,
             XCHAL_INTLEVEL1_MASK, BUILT_TIME);

    /* init irq handler, need init before irq using */
    irq_init();

    /* init exception handler */
    adsp_excep_init();

#ifdef CFG_UART_SUPPORT
    /* register UART interrupt function for CLI input */
    uart_register_irq();
#endif
#ifdef CFG_MPU_SUPPORT
    mpu_init_low_power_region_config();
#endif
#ifdef CFG_MTK_HEAP_SUPPORT
    /* Init mtk memory management, then can use MTK_pvPortMalloc()
     * and MTK_vPortFree */
    mtk_heap_init();
#endif

#ifdef CFG_MTK_PDCT_SUPPORT
    pdct_f0();
#endif

    /* Configure the hardware, after this point, the UART can be used*/
    prvSetupHardware();

    platform_init();

#ifdef CFG_WAKELOCK_SUPPORT
    wakelocks_init();
#endif  /* CFG_WAKELOCK_SUPPORT */
#ifdef CFG_AP_AWAKE_SUPPORT
    ap_awake_init();
#endif  /* CFG_AP_AWAKE_SUPPORT */

#ifdef CFG_TRAX_SUPPORT
    // if trax feature enable
    adsp_trax_init();
#endif
#ifdef CFG_WDT_SUPPORT
    /*put wdt init just before insterrupt enabled in vTaskStartScheduler()*/
    mtk_wdt_init();
#endif
#ifdef CFG_IRQ_MONITOR_SUPPORT
    set_irq_limit(1);
#endif
    PRINTF_E("Scheduler start...\n");

#if defined(CFG_AUDIO_DEMO)
    audio_process_init();
#endif

    /* Start the scheduler. After this point, the interrupt is enabled*/
    vTaskStartScheduler();
    /* If all is well, the scheduler will now be running, and the following line
      will never be reached.  If the following line does execute, then there was
      insufficient FreeRTOS heap memory available for the idle and/or timer tasks
      to be created.  See the memory management section on the FreeRTOS web site
      for more details. */
    for (;;);
}

void WriteDebugByte(unsigned int ch);

/**
  * @brief  Hardware init
  * @param  None
  * @retval None
  */
NORMAL_SECTION_FUNC static void prvSetupHardware(void)
{
#ifdef CFG_SYSTIMER_SUPPORT
    mt_platform_systimer_init();
#endif

#ifndef CFG_FPGA
#ifdef CFG_CLK_PM_SUPPORT
    mt_pll_init();
#endif
#endif

#ifdef CFG_EINT_SUPPORT
    /* init eint handler */
    mt_eint_init();
#endif

#ifdef CFG_CACHE_SUPPORT
    hal_cache_init();
#endif

#ifdef CFG_IPC_SUPPORT
    adsp_ipi_init();
#endif

#ifdef CFG_DMA_SUPPORT
    /* init DMA */
    mt_init_dma();
#endif
}
/*-----------------------------------------------------------*/


void vApplicationStackOverflowHook(xTaskHandle pxTask, char *pcTaskName)
{
    /* If configCHECK_FOR_STACK_OVERFLOW is set to either 1 or 2 then this
    function will automatically get called if a task overflows its stack. */
    (void) pxTask;
    (void) pcTaskName;
    PRINTF_E("\n\rtask: %s stack overflow\n", pcTaskName);
    for (;;);
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void)
{
    /* If configUSE_MALLOC_FAILED_HOOK is set to 1 then this function will
    be called automatically if a call to pvPortMalloc() fails.  pvPortMalloc()
    is called automatically when a task, queue or semaphore is created. */
    PRINTF_E("\n\rmalloc fail\n\r");
    PRINTF_E("Heap: free/total : %d/%d bytes\n", xPortGetFreeHeapSize(),
             configTOTAL_HEAP_SIZE);
    configASSERT(0);
    for (;;);
}
/*-----------------------------------------------------------*/

void MTK_vApplicationMallocFailedHook( MTK_eMemoryType eMemoryType )
{
    /* If configUSE_MALLOC_FAILED_HOOK is set to 1 then this function will
    be called automatically if a call to MTK_pvPortMalloc() fails. */
    PRINTF_E("\n\r MTK_pvPortMalloc fail, MemoryType: %d\n\r", eMemoryType);
    PRINTF_E("Heap: free/total : %d/%d bytes\n",
            MTK_xPortGetFreeHeapSize(eMemoryType),
            MTK_xGetHeapSize(eMemoryType));
    configASSERT(0);
    for (;;);
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{
    //printf("\n\r Enter IDLE \n\r");
}
/*-----------------------------------------------------------*/

void vApplicationTickHook(void)
{
    //printf("\n\r Enter TickHook \n\r");
}
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
    *ppxTimerTaskTCBBuffer = ( StaticTask_t * ) pvPortMalloc( sizeof( StaticTask_t ) );
    if( *ppxTimerTaskTCBBuffer != NULL )
    {
        *ppxTimerTaskStackBuffer = ( StackType_t * ) pvPortMalloc( ( ( ( size_t ) *pulTimerTaskStackSize ) * sizeof( StackType_t ) ) );
    }
}
/*-----------------------------------------------------------*/

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
    *ppxIdleTaskTCBBuffer = ( StaticTask_t * ) pvPortMalloc( sizeof( StaticTask_t ) );
    if( *ppxIdleTaskTCBBuffer != NULL )
    {
        *ppxIdleTaskStackBuffer = ( StackType_t * ) pvPortMalloc( ( ( ( size_t ) *pulIdleTaskStackSize ) * sizeof( StackType_t ) ) );
    }
}

#endif
