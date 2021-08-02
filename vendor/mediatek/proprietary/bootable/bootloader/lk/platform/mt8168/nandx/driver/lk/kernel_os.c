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

/* MNTL os porting layer - Linux Kernel implementation */

#include <printf.h>
#include <string.h>
#include <malloc.h>
#include <mntl_os.h>

void *os_calloc(mntl_size nmemb, mntl_size size)
{
	return calloc(nmemb, size);
}

void os_free(void *buf)
{
	free(buf);
}

static const char level_str[][sizeof("KERN_WARNING")] = {
	"KERN_ERR",
	"KERN_WARNING",
	"KERN_INFO",
	"KERN_DEBUG",
};

void os_print(int level, const char *fmt, ...)
{
	char buf[128];
	va_list ap;

	if (level > MNTL_PR_DEBUG)
		level = MNTL_PR_DEBUG;

	if (strlen(fmt) < sizeof(buf) - 8) {
		snprintf(buf, sizeof(buf), "%smntl: %s", level_str[level],
		         fmt);
		fmt = buf;
	}

	va_start(ap, fmt);
	_dvprintf(fmt, ap);
	va_end(ap);
}

int os_snprintf(char *buf, int size, const char *fmt, ...)
{
	va_list ap;
	int ret;

	va_start(ap, fmt);
	ret = vsnprintf(buf, size, fmt, ap);
	va_end(ap);
	return ret;
}

void *os_memcpy(void *dest, const void *src, mntl_size n)
{
	return memcpy(dest, src, n);
}

void *os_memset(void *dest, int s, mntl_size n)
{
	return memset(dest, s, n);
}

mntl_lock os_lock_create(void)
{
	mntl_lock *lock;

	lock = calloc(1, sizeof(mntl_lock));
	return lock;
}

int os_lock_destroy(mntl_lock lock)
{
	free(lock);
	return 0;
}

int os_lock(mntl_lock lock)
{
	return 0;
}

int os_unlock(mntl_lock lock)
{
	return 0;
}

void os_abort(void)
{
	return;
}

unsigned long long os_current_time(void)
{
	return 0;
}

int os_mvg_enabled(void)
{
	return 0;
}

int mvg_set_current_case(const char *gname, const char *cname)
{
	return 0;
}

int mvg_case_exit(const char *gname, const char *cname)
{
	return 0;
}
