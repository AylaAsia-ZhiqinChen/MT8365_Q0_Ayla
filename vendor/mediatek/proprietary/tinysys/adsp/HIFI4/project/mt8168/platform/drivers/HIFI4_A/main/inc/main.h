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
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"


/*
 * Be sure to note that the TINY ADDR is shared with 1K IPI shared buffer.
 * Please see the macro ' ADSP_IPI_SHARED_SIZE' = 0x0400,
 * |---IPI 768B (adsp_info) ---+---TINY 256B (debug buffer) ---|
 */
#define ADSP_TINY_DEBUG_SIZE (0x0100)
#define ADSP_TINY_DEBUG_ADDR (CFG_HIFI4_DTCM_ADDRESS + CFG_HIFI4_DTCM_SIZE - ADSP_TINY_DEBUG_SIZE)

#define ADSP_IPI_SHARED_SIZE (0x0400)
#define ADSP_IPI_SHARED_ADDR (CFG_HIFI4_DTCM_ADDRESS + CFG_HIFI4_DTCM_SIZE - ADSP_IPI_SHARED_SIZE)

#define ADSP_LOGGER_BUF_SIZE (0x0400)
#define ADSP_LOGGER_BUF_ADDR (CFG_HIFI4_DTCM_ADDRESS + CFG_HIFI4_DTCM_SIZE - ADSP_IPI_SHARED_SIZE - ADSP_LOGGER_BUF_SIZE)


/* Exported macro ------------------------------------------------------------*/
#if 1
//#define INTERNAL_FUNCTION

/*
 * Place function into DRAM section
 */
#define NORMAL_SECTION_FUNC      __attribute__((section(".dram.text"), rodata_section(".dram.rodata")))
#define NORMAL_SECTION_DATA      __attribute__((section(".dram.data")))
#define NORMAL_SECTION_BSS       __attribute__((section(".dram.bss")))
#define NORMAL_SECTION_RODATA    __attribute__((section(".dram.rodata")))
#endif


/* Exported functions ------------------------------------------------------- */
extern void platform_init(void);
#ifdef CFG_MPU_SUPPORT
extern int mpu_init_low_power_region_config(void);
extern int mpu_load_default_region_config(void);
extern int mpu_load_low_power_region_config(void);
#endif
#ifdef CFG_MTK_HEAP_SUPPORT
extern void mtk_heap_init(void);
#endif

#endif /* __MAIN_H */

