/*
 * MediaTek Inc. (C) 2018. All rights reserved.
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

#include <arch/ops.h>
#include <malloc.h>
#include <platform/mt_typedefs.h>
#include <platform/mt_gpt.h>
#include <printf.h>
#include <stdio.h>
#include <string.h>
#include <kernel/event.h>
#include <platform/mt_reg_base.h>

#define wait_queue_head_t int

#ifndef NULL
#define NULL (void *)0
#endif

#ifndef WARN_ON
#define WARN_ON(condition) \
do { \
    if (condition) \
        dprintf(CRITICAL, "[Nandx LK WARN_ON] %s: line %d\n", \
            __func__, __LINE__); \
} while (0)
#endif

#define nwritel(value, reg) DRV_WriteReg32(reg, value)
#define nwritew(value, reg) DRV_WriteReg16(reg, value)
#define nwriteb(value, reg) DRV_WriteReg8(reg, value)
#define nreadl(reg) DRV_Reg32(reg)
#define nreadw(reg) DRV_Reg16(reg)
#define nreadb(reg) DRV_Reg8(reg)

#define TASK_UNINTERRUPTIBLE 0
#define current (0)
#define init_waitqueue_head(q)
#define DECLARE_WAITQUEUE(name, tsk)    int name = tsk
#define add_wait_queue(q, wait)     ((q) = (wait))
#define set_current_state(state_value)
#define schedule()
static inline void remove_wait_queue(int *wq, int *wait)
{
}

static inline void wake_up(int *wq)
{
}

#define BIT(a)          (1 << (a))
#define GENMASK(h, l) \
    (((~0UL) << (l)) & (~0UL >> ((sizeof(unsigned long) * 8) - 1 - (h))))
#define round_up(x, y)      ((((x) - 1) | ((y) - 1)) + 1)
#define round_down(x, y)    ((x) & ~((y) - 1))
#define container_of(ptr, type, member) \
    ({const __typeof__(((type *)0)->member) * __mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type, member)); })
#define MAX_ERRNO   4096
#define ERR_PTR(errno)  ((void *)((long)errno))
#define PTR_ERR(ptr)    ((long)(ptr))
#define IS_ERR(ptr) ((unsigned long)(ptr) > (unsigned long)-MAX_ERRNO)

#define nand_lock_create()  NULL
#define nand_lock_destroy(lock)
static inline void nand_lock(void *lock)
{
}

static inline void nand_unlock(void *lock)
{
}

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
    dprintf(CRITICAL, "[Nandx LK Error]" fmt, ##__VA_ARGS__);
#define pr_warn(fmt, ...) \
    dprintf(ALWAYS, "[Nandx LK Warn]" fmt, ##__VA_ARGS__);
#define pr_info(fmt, ...) \
    dprintf(INFO, "[Nandx LK Info]" fmt, ##__VA_ARGS__);
#define pr_debug(fmt, ...) \
    dprintf(INFO, "[Nandx LK Debug]" fmt, ##__VA_ARGS__);

static inline void *mem_alloc(u32 count, u32 size)
{
	return calloc(count, size);
}

static inline void mem_free(void *mem)
{
	if (mem)
		free(mem);
}

static inline void *nand_event_create(void)
{
	event_t *event;

	event = mem_alloc(1, sizeof(event_t));
	event_init(event, false, EVENT_FLAG_AUTOUNSIGNAL);
	return (void *)event;
}

static inline void nand_event_destroy(void *event)
{
	if (event)
		mem_free(event);
}

static inline void nand_event_complete(void *event)
{
	event_signal(event, 0);
}

static inline void nand_event_init(void *event)
{
}

/*
 * timeout value is millisecond
 * return non-zero for complete, else timout
 */
static inline int nand_event_wait_complete(void *event, u32 timeout)
{
	return !event_wait_timeout(event, 100);
}

static inline u64 get_current_time_us(void)
{
	return gpt4_tick2time_us(gpt4_get_current_tick());
}

static inline u32 nand_dma_map(void *dev, void *buf, u64 len,
                               enum NDMA_OPERATION op)
{
	arch_clean_invalidate_cache_range((addr_t)buf, (size_t)len);
	return (u32)buf;
}

static inline void nand_dma_unmap(void *dev, void *buf, u32 addr, u64 len,
                                  enum NDMA_OPERATION op)
{
	arch_clean_invalidate_cache_range((addr_t)buf, (size_t)len);
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
