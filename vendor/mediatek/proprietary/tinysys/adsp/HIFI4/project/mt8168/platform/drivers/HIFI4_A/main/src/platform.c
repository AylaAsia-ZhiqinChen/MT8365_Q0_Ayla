/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2017. All rights reserved.
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
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/

#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#ifdef CFG_MPU_SUPPORT
#include "mpu.h"
#endif
#ifdef CFG_CLI_SUPPORT
#include "cli.h"
#endif
#ifdef CFG_MTK_HEAP_SUPPORT
#include <mtk_heap.h>
#endif
#include "types.h"
#include "main.h"
#include "driver_api.h"
#include "interrupt.h"
#include "mt_reg_base.h"
#include "systimer.h"
#ifdef CFG_AUDIO_SUPPORT
#include "audio.h"
#endif


#ifdef CFG_TASK_MONITOR
static int task_monitor_log = 1;
static unsigned int ix = 0;
static char list_buffer[512];
#define mainCHECK_DELAY    ( ( portTickType) 40000 / portTICK_RATE_MS )

static void vTaskMonitor(void *pvParameters)
{
    portTickType xLastExecutionTime, xDelayTime;

    xLastExecutionTime = xTaskGetTickCount();
    xDelayTime = mainCHECK_DELAY;

    do {
        if (task_monitor_log == 1) {
            vTaskList(list_buffer);
            PRINTF_D("xLastExecutionTime: %d\n", xLastExecutionTime);
            PRINTF_D("[%d]Heap:free/total size:%d/%d\n", ix, xPortGetFreeHeapSize(),
                     configTOTAL_HEAP_SIZE);
            PRINTF_D("Task Status:\n\r%s", list_buffer);
            PRINTF_D("max dur.:%llu,limit:%llu\n", get_max_cs_duration_time(),
                     get_max_cs_limit());
#ifdef CFG_IPI_STAMP_SUPPORT
            ipi_status_dump();
#endif
        }
        ix ++;
        vTaskDelayUntil(&xLastExecutionTime, xDelayTime);
    }
    while (1);
}
#endif


NORMAL_SECTION_FUNC void platform_init(void)
{
    PRINTF_D("in %s\n", __func__);
#ifdef CFG_HIFI4_DUAL_CORE
    mt_poweron_dsp1();
#endif

#ifdef CFG_TASK_MONITOR
    xTaskCreate(vTaskMonitor, "TMon", 2 * configMINIMAL_STACK_SIZE, (void *)4, 0, NULL);
#endif

#ifdef CFG_CLI_SUPPORT
    vUARTCommandConsoleStart(2 * configMINIMAL_STACK_SIZE, (tskIDLE_PRIORITY + 1));
    vRegisterOSCLICommands();
#if defined(CFG_MPU_SUPPORT) && defined(CFG_MPU_DEBUG)
    dump_mpu_status();
    vRegisterMpuCommands();
#endif
#endif

#ifdef CFG_AUDIO_SUPPORT
    audio_init();
#endif

    irq_status_dump();
    return;
}


#ifdef CFG_MPU_SUPPORT
/*
 * mpu regions, must sort by vaddr,
 * other regions will be no access.
 */
static const mpu_region_t g_adsp_mpu_region_default[] =
{
    {
        .vaddr          = DSP_VER_REG_BASE,
        .size           = DSP_VER_REG_SIZE,
        .access_right   = XTHAL_AR_R,
        .memory_type    = XTHAL_MEM_DEVICE,
        .enable         = MPU_ENTRY_ENABLE,
    },
    {
        .vaddr          = DSP_SYS_REG_BASE,
        .size           = DSP_SYS_REG_SIZE,
        .access_right   = XTHAL_AR_RW,
        .memory_type    = XTHAL_MEM_DEVICE,
        .enable         = MPU_ENTRY_ENABLE,
    },
    {
        .vaddr          = DSP_AUDIO_SRAM_BASE,
        .size           = DSP_AUDIO_SRAM_SIZE,
        .access_right   = XTHAL_AR_RW,
        .memory_type    = XTHAL_MEM_NON_CACHEABLE,
        .enable         = MPU_ENTRY_ENABLE,
    },
    {
        .vaddr          = DSP_REG_REMAP_BASE,
        .size           = DSP_REG_REMAP_SIZE,
        .access_right   = XTHAL_AR_RW,
        .memory_type    = XTHAL_MEM_DEVICE,
        .enable         = MPU_ENTRY_ENABLE,
    },
    {
        .vaddr          = CFG_HIFI4_DTCM_ADDRESS,
        .size           = CFG_HIFI4_DTCM_SIZE,
        .access_right   = XTHAL_AR_RW,
        .memory_type    = XTHAL_MEM_NON_CACHEABLE,
        .enable         = MPU_ENTRY_ENABLE,
    },
    {
        .vaddr          = CFG_HIFI4_ITCM_ADDRESS,
        .size           = CFG_HIFI4_ITCM_SIZE,
        .access_right   = XTHAL_AR_RX,
        .memory_type    = XTHAL_MEM_NON_CACHEABLE,
        .enable         = MPU_ENTRY_ENABLE,
    },
    {
        .vaddr          = CFG_HIFI4_DRAM_ADDRESS,
        .size           = CFG_HIFI4_DRAM_SIZE,
        .access_right   = XTHAL_AR_RWX,
        .memory_type    = XTHAL_MEM_WRITEBACK,
        .enable         = MPU_ENTRY_ENABLE,
    },
    {
        .vaddr          = CFG_HIFI4_DRAM_RESERVE_CACHE_START,
        .size           = CFG_HIFI4_DRAM_RESERVE_CACHE_SIZE,
        .access_right   = XTHAL_AR_RW,
        .memory_type    = XTHAL_MEM_WRITEBACK,
        .enable         = MPU_ENTRY_ENABLE,
    },
    {
        .vaddr          = CFG_HIFI4_DRAM_SHARED_NONCACHE_START,
        .size           = CFG_HIFI4_DRAM_SHARED_NONCACHE_SIZE,
        .access_right   = XTHAL_AR_RW,
        .memory_type    = XTHAL_MEM_NON_CACHEABLE,
        .enable         = MPU_ENTRY_ENABLE,
    },
};

#ifdef CFG_MPU_LOW_POWER_CHECK
static mpu_region_t
    g_adsp_mpu_region_low_power[ARRAY_SIZE(g_adsp_mpu_region_default)];

NORMAL_SECTION_FUNC
int mpu_init_low_power_region_config(void)
{
    const mpu_region_t *region_src = g_adsp_mpu_region_default;
    mpu_region_t *region_dst = g_adsp_mpu_region_low_power;
    int i;

    for (i = 0; i < ARRAY_SIZE(g_adsp_mpu_region_default); i++) {
        memcpy(region_dst, region_src, sizeof(mpu_region_t));

        switch (region_src->vaddr) {
        case CFG_HIFI4_DRAM_ADDRESS:
            /* FALLTHROUGH */
        case CFG_HIFI4_DRAM_RESERVE_CACHE_START:
            /* FALLTHROUGH */
        case CFG_HIFI4_DRAM_SHARED_NONCACHE_START:
            /* deny the access right */
            region_dst->access_right = XTHAL_AR_NONE;
            /* FALLTHROUGH */
        default:
            region_src++;
            region_dst++;
            continue;
        }
    }

    return 0;
}

int mpu_load_low_power_region_config(void)
{
    return mpu_init(g_adsp_mpu_region_low_power,
        ARRAY_SIZE(g_adsp_mpu_region_low_power));
}
#else
NORMAL_SECTION_FUNC
int mpu_init_low_power_region_config(void)
{
    return 0;
}

int mpu_load_low_power_region_config(void)
{
    return 0;
}
#endif

int mpu_load_default_region_config(void)
{
    return mpu_init(g_adsp_mpu_region_default,
            ARRAY_SIZE(g_adsp_mpu_region_default));
}

#endif /* CFG_MPU_SUPPORT */


#ifdef CFG_MTK_HEAP_SUPPORT
/*
 * You can add eMemoryType according to actual needs,
 * but not too much.
 */
NORMAL_SECTION_RODATA static const MTK_HeapRegion_t MTK_xHeapRegions[] =
{
    {
        .pucStartAddress = (uint8_t *)CFG_HIFI4_DRAM_RESERVE_CACHE_START,
        .xSizeInBytes    = CFG_HIFI4_DRAM_RESERVE_CACHE_SIZE,
        .eMemoryType     = MTK_eMemDramNormal,
        .pcName          = "Heap(DramNormal)",
    },
    {
        .pucStartAddress = (uint8_t *)CFG_HIFI4_DRAM_SHARED_NONCACHE_START,
        .xSizeInBytes    = CFG_HIFI4_DRAM_SHARED_NONCACHE_SIZE,
        .eMemoryType     = MTK_eMemDramNormalNC,
        .pcName          = "Heap(DramNormalNC)",
    },
};

NORMAL_SECTION_FUNC void mtk_heap_init(void)
{
    MTK_vPortDefineHeapRegions(MTK_xHeapRegions,
            ARRAY_SIZE(MTK_xHeapRegions));
}

#endif

