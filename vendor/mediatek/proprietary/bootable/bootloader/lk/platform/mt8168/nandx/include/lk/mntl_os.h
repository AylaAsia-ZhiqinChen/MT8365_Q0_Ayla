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

/* MNTL os porting layer header file */

#ifndef __MNTL_OS_LAYER_H__
#define __MNTL_OS_LAYER_H__

#include <mntl_types.h>
#include <errno.h>

/*
 * level used with os_print
 *   ERROR will be printed in all build/cases
 *   DEBUG only available when build witn MNTL_DEBUG
 */
#define MNTL_PR_ERROR   0
#define MNTL_PR_WARN    1
#define MNTL_PR_INFO    2
#define MNTL_PR_DEBUG   3

/*
 * allocate memory and memset zero, see calloc
 * @nmemb:  Number of element to allocate
 * @size:   Size of each element
 */
void *os_calloc(mntl_size nmemb, mntl_size size);

/*
 * Free a buffer allocated by os_calloc
 * @buf:  Buffer to free. os_free will just return if it is NULL.
 */
void os_free(void *buf);

/*
 * print message
 * @level: level of this message, see MNTL_PR_*
 * @fmt:   printf format to print.
 *
 * The message might be only going internal buffer or completely dropped
 * based on level and system build.
 */
void os_print(int level, const char *fmt, ...);

/*
 * format string
 * @buf:   output buffer
 * @size:  size of the buffer.
 * @fmt:   printf format to print.
 *
 * see standard C snprintf
 */
extern __attribute__ ((format(printf, 3, 4)))
int os_snprintf(char *buf, int size, const char *fmt, ...);

/* see memcpy */
void *os_memcpy(void *dest, const void *src, mntl_size n);

/* see memset */
void *os_memset(void *dest, int s, mntl_size n);

/* opaque handle for fast lock */
typedef void *mntl_lock;

/* create a lock */
mntl_lock os_lock_create(void);

/* destroy lock */
int os_lock_destroy(mntl_lock lock);

/* Acquire the lock. This is fast lock, expect the lock to be released soon */
int os_lock(mntl_lock lock);

/* Release the lock */
int os_unlock(mntl_lock lock);

/* Abort the system. Should only be used when debug. */
void os_abort(void);

/* Get current system timestemp in ns */
unsigned long long os_current_time(void);

/* SPOH: Power cut test */
int os_mvg_enabled(void);
int mvg_set_current_case(const char *gname, const char *cname);
int mvg_case_exit(const char *gname, const char *cname);

#endif              /* __MNTL_OS_LAYER_H__ */
