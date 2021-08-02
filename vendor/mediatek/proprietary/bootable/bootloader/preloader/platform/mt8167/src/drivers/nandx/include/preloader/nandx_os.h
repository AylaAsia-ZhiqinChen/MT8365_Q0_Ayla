/*
 * MediaTek Inc. (C) 2019. All rights reserved.
 *
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
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

#ifndef __NANDX_OS_H__
#define __NANDX_OS_H__

#include <stddef.h>
#include <timer.h>
#include "typedefs.h"
#include "print.h"
#include "mt8167.h"

#define wait_queue_head_t int

#ifndef NULL
#define NULL 0
#endif

#ifndef WARN_ON
#define WARN_ON(condition) \
do { \
    if (condition) \
        print("[Nandx PL WARN_ON] %s: line %d\n", \
            __func__, __LINE__); \
} while (0)
#endif

#define nwritel(value, reg) __raw_writel(value, reg)
#define nwritew(value, reg) __raw_writew(value, reg)
#define nwriteb(value, reg) __raw_writeb(value, reg)
#define nreadl(reg) __raw_readl(reg)
#define nreadw(reg) __raw_readw(reg)
#define nreadb(reg) __raw_readb(reg)

#define BIT(a)      (1 << (a))
#define GENMASK(h, l) \
    (((~0UL) << (l)) & (~0UL >> ((sizeof(unsigned long) * 8) - 1 - (h))))
#define round_up(x, y)  ((((x) - 1) | ((y) - 1)) + 1)
#define round_down(x, y)    ((x) & ~((y) - 1))
#define container_of(ptr, type, member) \
    ({const __typeof__(((type *)0)->member) * __mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type, member)); })

#define MAX_ERRNO   4096
#define ERR_PTR(errno)  ((void *)((long)errno))
#define PTR_ERR(ptr)    ((long)(ptr))
#define IS_ERR(ptr) ((unsigned long)(ptr) > (unsigned long)-MAX_ERRNO)

#define TASK_UNINTERRUPTIBLE 0
#define current (0)
#define init_waitqueue_head(q)
#define DECLARE_WAITQUEUE(name, tsk)    int name = tsk
#define add_wait_queue(q, wait)     ((q) = (wait))
#define remove_wait_queue(q, wait)  ((q) = 0)
#define set_current_state(state_value)
#define wake_up(x)
#define schedule()

#define nand_lock_create()  NULL
#define nand_lock_destroy(lock)
#define nand_lock(lock)
#define nand_unlock(lock)

#define nand_event_create() NULL
#define nand_event_destroy(event)
#define nand_event_complete(event)
#define nand_event_init(event)
#define nand_event_wait_complete(event, timeout)    true
#define nand_irq_register(irq_id, irq_handler, name, data)  (0)

#define do_div(n, base) \
({                  \
    u32 __base = (base);                \
    u32 __rem;                      \
    __rem = ((u64)(n)) % __base;            \
    (n) = ((u64)(n)) / __base;              \
    __rem;                          \
})

#define pr_err(fmt, ...) \
    print("[Nandx PL Error]" fmt, ##__VA_ARGS__)
#define pr_warn(fmt, ...) \
    print("[Nandx PL Warn]" fmt, ##__VA_ARGS__)
#define pr_info(fmt, ...) \
    print("[Nandx PL Info]" fmt, ##__VA_ARGS__)
#define pr_debug(fmt, ...) \
    print("[Nandx PL Debug]" fmt, ##__VA_ARGS__)

void *mem_alloc(u32 count, u32 size);
void mem_free(void *mem);

static inline u64 get_current_time_us(void)
{
    return get_time_us();
}

static inline u32 nand_dma_map(void *dev, void *buf, u64 len,
                               enum NDMA_OPERATION op)
{
    return (u32)buf;
}

static inline void nand_dma_unmap(void *dev, void *buf, u32 addr, u64 len,
                                  enum NDMA_OPERATION op)
{
    /* empty */
}

static inline bool is_support_mntl(void)
{
#ifdef MNTL_SUPPORT
    return true;
#else
    return false;
#endif
}

static inline bool virt_addr_valid(void *buf)
{
    /* true: physical address, false: logical address */
    return true;
}

#endif              /* __NANDX_OS_H__ */
