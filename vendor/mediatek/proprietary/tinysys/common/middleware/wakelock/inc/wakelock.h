/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly
 * prohibited.
 */
/* MediaTek Inc. (C) 2019. All rights reserved.
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
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY
 * ACKNOWLEDGES THAT IT IS RECEIVER\'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY
 * THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK
 * SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO
 * RECEIVER\'S SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN
 * FORUM. RECEIVER\'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK\'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER
 * WILL BE, AT MEDIATEK\'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE
 * AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
 * RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation
 * ("MediaTek Software") have been modified by MediaTek Inc. All revisions are
 * subject to any receiver\'s applicable license agreements with MediaTek Inc.
 */

#ifndef _WAKE_LOCK_H_
#define _WAKE_LOCK_H_

#include <timers.h>

#define configMAX_LOCK_NAME_LEN 5
#define RETRY_TIMEOUT ( ( portTickType) 1000 / portTICK_RATE_MS ) /* timeout 1000 ms */

typedef struct wakelock {
	char pcLockName[configMAX_TASK_NAME_LEN];
	ListItem_t xGenericListItem;
	TickType_t xExpiredTick;
	TimerHandle_t xTimer;
} wakelock_t;

void wakelocks_init(void);
void wake_lock_init(wakelock_t * lock, char *name);
void wake_lock_deinit(wakelock_t * lock);
BaseType_t wake_unlock(wakelock_t * lock);
BaseType_t wake_unlock_FromISR(wakelock_t * lock);
BaseType_t wake_lock(wakelock_t * lock);
BaseType_t wake_lock_FromISR(wakelock_t * lock);
BaseType_t wake_lock_timeout(wakelock_t * lock, long timeout);
BaseType_t wake_lock_timeout_FromISR(wakelock_t * lock, long timeout);
void wake_lock_list_dump(void);
BaseType_t wake_lock_is_empty(void);

#endif
