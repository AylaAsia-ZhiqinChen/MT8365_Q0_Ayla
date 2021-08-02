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
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/

#ifndef AUDIO_MEMORY_CONTROL_H
#define AUDIO_MEMORY_CONTROL_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <audio_drv_log.h>
#include <audio_assert.h>

#include <FreeRTOS.h>



/*
 * =============================================================================
 *                     MACRO
 * =============================================================================
 */

#define GET_MAX_VALUE(x, y) ((x) > (y)) ? (x) : (y);
#define GET_MIN_VALUE(x, y) ((x) < (y)) ? (x) : (y);

#define GET_FILENAME() \
    ({ \
        const char *__slash = strrchr(__FILE__, '/'); \
        const char *__filename = (__slash) ? __slash + 1 : __FILE__; \
        __filename; \
    })



/*
 * =============================================================================
 *                     AUDIO_MALLOC & AUDIO_FREE
 * =============================================================================
 */

#define AUDIO_MALLOC(sz) \
    ({ \
        uint32_t __sz_total = 128 + (sz) +  1; \
        uint8_t *__base = NULL; \
        uint8_t *__head = NULL; \
        uint32_t *__p_sz = NULL; \
        void *__ptr = NULL; \
        uint8_t *__tail = NULL; \
        AUD_LOG_VV("AUDIO_MALLOC(%u), heap(%d/%d), \"%s\", %uL\n", \
                   (uint32_t)(sz), \
                   xPortGetFreeHeapSize(), configTOTAL_HEAP_SIZE, \
                   GET_FILENAME(), __LINE__); \
        __base = (uint8_t *)kal_pvPortMalloc(__sz_total); \
        __head = (uint8_t *)(__base + 127); \
        __p_sz = (uint32_t *)(__base + 64); \
        __ptr  = (void *)(__base + 128); \
        __tail  = __base + 128 + (sz); \
        AUD_LOG_V("AUDIO_MALLOC(%u) => %p (%p %p), heap(%d/%d), \"%s\", %uL\n", \
                  (uint32_t)(sz), __ptr, __base, __tail, \
                  xPortGetFreeHeapSize(), configTOTAL_HEAP_SIZE, \
                  GET_FILENAME(), __LINE__); \
        *__head = 0x55; \
        *__p_sz = (sz); \
        *__tail = 0x66; \
        AUDIO_CHECK(__ptr); \
        __ptr; \
    })


#define AUDIO_CHECK(ptr) \
    ({ \
        if (ptr) { \
            uint32_t __sz = 0; \
            uint8_t *__base = NULL; \
            uint8_t *__head = NULL; \
            uint8_t *__tail = NULL; \
            __base = ((uint8_t *)(ptr)) - 128; \
            __head = (uint8_t *)(__base + 127); \
            __sz = *((uint32_t *)(__base + 64)); \
            __tail  = __base + 128 + (__sz); \
            if (*__head != 0x55 || *__tail != 0x66) { \
                AUD_LOG_E("AUDIO_CHECK(%p) fail, (%d %d) %u\n", \
                          (ptr), *__base, *__tail, __sz); \
                AUD_WARNING("memory corruption!!"); \
            } \
        } \
    })


#define AUDIO_FREE(ptr) \
    ({ \
        uint8_t *__base = ((uint8_t *)(ptr)) - 128; \
        AUDIO_CHECK(ptr); \
        kal_vPortFree(__base); \
        AUD_LOG_V("AUDIO_FREE(%p), sz %u, heap (%d/%d), \"%s\", %uL\n", \
                  (ptr), *((uint32_t *)(__base + 64)), \
                  xPortGetFreeHeapSize(), configTOTAL_HEAP_SIZE, \
                  GET_FILENAME(), __LINE__); \
    })



/*
 * =============================================================================
 *                     utilities
 * =============================================================================
 */

#define AUDIO_ALLOC_BUFFER(ptr, len) \
    do { \
        if (ptr != NULL) { \
            AUD_LOG_W("mem leak!! \"%s\", %uL", GET_FILENAME(), __LINE__); \
        } \
        ptr = (void *)AUDIO_MALLOC(len); \
        AUD_ASSERT(ptr != NULL); \
        memset(ptr, 0, len); \
    } while(0)


#define AUDIO_ALLOC_CHAR_BUFFER(ptr, len) \
    do { \
        if (ptr != NULL) { \
            AUD_LOG_W("mem leak!! \"%s\", %uL", GET_FILENAME(), __LINE__); \
        } \
        ptr = (char *)AUDIO_MALLOC(len); \
        AUD_ASSERT(ptr != NULL); \
        memset(ptr, 0, len); \
    } while(0)


#define AUDIO_ALLOC_STRUCT(type, ptr) \
    do { \
        if (ptr != NULL) { \
            AUD_LOG_W("mem leak!! \"%s\", %uL", GET_FILENAME(), __LINE__); \
        } \
        ptr = (type *)AUDIO_MALLOC(sizeof(type)); \
        AUD_ASSERT(ptr != NULL); \
        memset(ptr, 0, sizeof(type)); \
    } while(0)


#define AUDIO_ALLOC_STRUCT_ARRAY(type, num, ptr) \
    do { \
        if (ptr != NULL) { \
            AUD_LOG_W("mem leak!! \"%s\", %uL", GET_FILENAME(), __LINE__); \
        } \
        ptr = (type *)AUDIO_MALLOC(sizeof(type) * num); \
        AUD_ASSERT(ptr != NULL); \
        memset(ptr, 0, sizeof(type) * num); \
    } while(0)


#define AUDIO_FREE_POINTER(ptr) \
    do { \
        if (ptr != NULL) { \
            AUDIO_FREE(ptr); \
            ptr = NULL; \
        } \
    } while(0)




/*
 * =============================================================================
 *                     dynamic memory management
 * =============================================================================
 */

int dynamic_change_buf_size(
    void **ptr,
    uint32_t *current_size,
    uint32_t target_size);



/*
 * =============================================================================
 *                     string
 * =============================================================================
 */

char *strcpy_safe(char *target, char *source, uint32_t target_size);
char *strcat_safe(char *target, char *source, uint32_t target_size);




#endif /* AUDIO_MEMORY_CONTROL_H */

