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

#include <FreeRTOS.h>
#include <timers.h>
#include <task.h>
#include "wakelock.h"
#include <mt_printf.h>

/*
 * Driver API introduction
 * ==========
 * A driver/application can use the wakelock api by adding a wakelock variable
 * to prevent from tinysys sleeping and calling wake_lock_init.
 *
 * For instance:
 * #include <wakelock.h>
 * wakelock_t wakelock
 *
 * init() {
 *  wake_lock_init(&task_monitor_wakelock,"wak_1");
 * }
 *
 * When the driver determines that it needs to run (usually in an interrupt
 * handler) it calls wake_lock:
 *   wake_lock(&wakelock);
 *
 * When it no longer needs to run it calls wake_unlock:
 *   wake_unlock(&wakelock);
 *
 * It can also call wake_lock_timeout to release the wakelock after a delay:
 *   wake_lock_timeout(&wakelock, ms);
 *
 * In ISR, it should call _FromISR:
 *   wake_lock_FromISR(&wakelock);
 *   wake_unlock_FromISR(&wakelock);
 *   wake_lock_timeout_FromISR(&wakelock, ms);
 *
 * When the new expired time is less than previous wake_lock expired time, the
 * change will not be applied
 * ex.
 * wake_lock_timeout(&wakelock, 10000);
 * wake_lock_timeout(&wakelock, 5000); <--- not applied, since the new expiration < current one
 */

/* define WAKELOCK_DEBUG 1 to print out wakelock debug log*/
#ifndef WAKELOCK_DEBUG
#define WAKELOCK_DEBUG (0)
#endif

#ifndef tmrNO_DELAY
#define tmrNO_DELAY     ( TickType_t ) 0U
#endif

static List_t xWakelocksList;

/**
 * @brief wake lock list init, called before other fuctions
 */
void wakelocks_init(void)
{
	/*initialize a wakelock list which stores the locks */
	vListInitialise(&xWakelocksList);
}

/**
 * @brief wakelock unlock callback function
 * @param[in] xTimer is the timer descriptor
 */
static void wake_unlock_timer_callback(TimerHandle_t xTimer)
{
	wakelock_t *lock;

	/* retrieve the lock structure */
	lock = (wakelock_t *) pvTimerGetTimerID(xTimer);
	wake_unlock(lock);
}

/**
 * @brief specific wake lock structure de init, recycle the timer resource
 * @param[in] lock is the lock structure
 */
void wake_lock_deinit(wakelock_t * lock)
{

	/* Ensure the lock is valid, NULL: assert */
	configASSERT(lock);
	configASSERT(lock->xTimer);
	/* unlock the wakelock to prevent lock holding long time */
	wake_unlock(lock);
	/* Clean the timer resource */
	xTimerDelete(lock->xTimer, RETRY_TIMEOUT);
	lock->xTimer = NULL;
}

/**
 * @brief specific wake lock structure init, called before other operation fuction
 * @param[in] lock is the lock structure
 * @param[in] name is the lock name
 */
void wake_lock_init(wakelock_t * lock, char *name)
{
	UBaseType_t x;
	TimerHandle_t xTimer;

	/* Ensure the lock is valid, NULL: assert */
	configASSERT(lock);

	/* Store the task name in the lock. */
	for (x = 0; x < (UBaseType_t) configMAX_LOCK_NAME_LEN; x++) {
		lock->pcLockName[x] = name[x];

		/* Don't copy all configMAX_LOCK_NAME_LEN if the string is shorter than
		   configMAX_LOCK_NAME_LEN characters just in case the memory after the
		   string is not accessible (extremely unlikely). */
		if (name[x] == 0x00)
			break;
	}
	/* initial value */
	lock->xExpiredTick = 0;
	/* initialize lock structure: make sure the item is not on a list. */
	vListInitialiseItem(&(lock->xGenericListItem));
	listSET_LIST_ITEM_OWNER(&(lock->xGenericListItem), lock);
	xTimer = xTimerCreate(	/* Just a text name, not used by the RTOS kernel. */
				name,
				/* The timer period in ticks, default value:portMAX_DELAY. */
				portMAX_DELAY,
				/* The timers will auto-reload themselves
				   when they expire. */
				pdFALSE,
				/* The ID is used to store lock structure address,
				 * it is used to get real lock structure when timer expire*/
				/*
				 * @param pvTimerID An identifier that is assigned to the timer being created.
				 * Typically this would be used in the timer callback function to identify which
				 * timer expired when the same callback function is assigned to more than one
				 * timer.
				 */
				(void *) lock,
				/* Each timer calls the same callback when it expires. */
				wake_unlock_timer_callback);
	if (xTimer == NULL) {
		/* The timer was not created. malloc fail? */
		configASSERT(0);
	} else {
		/* stop the timer because it's not use yet */
		xTimerStop(xTimer, RETRY_TIMEOUT);
		/* store the timer on lock structure */
		lock->xTimer = xTimer;
	}
	/*reset HW reg = 0 */
}

/**
 * @brief to hold wakelock at specific time from task
 * @param[in] lock is the lock structure
 * @param[in] timeout is the time value which the lock will be held
 * @return pdPASS, wakelock lock successfuly
 * @return pdFAIL, it is failed to lock wakelock
 */
static BaseType_t wake_lock_interval(wakelock_t * lock, long timeout_ms)
{

	TickType_t xNewExpiredTick;
	BaseType_t xReturn = pdPASS;

	/* Ensure the lock is valid */
	configASSERT(lock);
	configASSERT(lock->xTimer);
	if (timeout_ms < 0) {
#if WAKELOCK_DEBUG
		PRINTF_D("[%s] lock id:%x, interval:%d is not valid\n", __func__, lock, timeout_ms);
#endif
		return pdFAIL;
	}
#if WAKELOCK_DEBUG
	PRINTF_D("[%s] lock id:%x, interval:%d\n", __func__, lock, timeout_ms);
#endif

	/*use critical section protect xWakelocksList from other task/isr accessing */
	taskENTER_CRITICAL();

	if (timeout_ms == 0) {
		if (xTimerIsTimerActive(lock->xTimer) != pdFALSE) {
			xReturn = xTimerStop(lock->xTimer, tmrNO_DELAY);
			if (xReturn == pdFAIL) {
				PRINTF_D("[%s] fail to stop timer\n", __func__);
				goto _exit;
			}
		}
	} else {
		xNewExpiredTick =
		    xTaskGetTickCount() + (timeout_ms / portTICK_PERIOD_MS);
		/* change:    when the new expired time > original one
		 * no change: when the new expired time <= original one*/
		if (xNewExpiredTick > lock->xExpiredTick) {
#if WAKELOCK_DEBUG
			PRINTF_D
			    ("[%s] lock id:%x, new expired tick:%d, changed\n", __func__, lock, xNewExpiredTick);
#endif
			lock->xExpiredTick = xNewExpiredTick;
			/*once the period is changed, the timer is active */
			xReturn =
			    xTimerChangePeriod(lock->xTimer,
					       timeout_ms / portTICK_PERIOD_MS,
					       tmrNO_DELAY);
			if (xReturn == pdFAIL) {
				PRINTF_D("[%s] fail to change period\n", __func__);
				goto _exit;
			}
		}
	}

	if (listLIST_ITEM_CONTAINER(&(lock->xGenericListItem)) != NULL) {
		/* wakelock already in the list */
	} else {
		vListInsertEnd(&xWakelocksList, &(lock->xGenericListItem));
	}

      _exit:
	/* leave the critical section */
	taskEXIT_CRITICAL();

	return xReturn;

}

BaseType_t wake_lock(wakelock_t * lock)
{
	return wake_lock_interval(lock, 0);
}

BaseType_t wake_lock_timeout(wakelock_t * lock, long timeout_ms)
{
	return wake_lock_interval(lock, timeout_ms);
}

/**
 * @brief to hold wakelock at specific time from ISR
 * @param[in] lock is the lock structure
 * @param[in] timeout_ms is the time value which the lock will be held
 * @return pdPASS, wakelock lock successfuly
 * @return pdFAIL, it is failed to lock wakelock
 */
static BaseType_t wake_lock_interval_FromISR(wakelock_t * lock, long timeout_ms)
{
	TickType_t xNewExpiredTick;

	/* Ensure the lock is valid */
	configASSERT(lock);
	configASSERT(lock->xTimer);
	if (timeout_ms < 0) {
#if WAKELOCK_DEBUG
		PRINTF_D("[%s] lock id:%x, interval:%d is not valid\n", __func__, lock, timeout_ms);
#endif
		return pdFAIL;
	}
#if WAKELOCK_DEBUG
	PRINTF_D("[%s] lock id:%x, interval:%d\n", __func__, lock, timeout_ms);
#endif
	/*it's in ISR, no need to protect xWakelocksList from other task accessing */
	if (listLIST_ITEM_CONTAINER(&(lock->xGenericListItem)) != NULL) {
		/* item exist */
	} else {
		vListInsertEnd(&xWakelocksList, &(lock->xGenericListItem));
	}

	if (timeout_ms == 0)
		xTimerStopFromISR(lock->xTimer, NULL);
	else {
		xNewExpiredTick =
		    xTaskGetTickCount() + (timeout_ms / portTICK_PERIOD_MS);
		/* change:    when the new expired time is > original one
		 * no change: when the new expired time is <= original one*/
		if (xNewExpiredTick > lock->xExpiredTick) {
#if WAKELOCK_DEBUG
			PRINTF_D
			    ("[%s] lock id:%x, new expired tick:%d, changed\n", __func__, lock, xNewExpiredTick);
#endif
			lock->xExpiredTick = xNewExpiredTick;
			/*once the period is changed, the timer is active */
			xTimerChangePeriodFromISR(lock->xTimer, timeout_ms
						  / portTICK_PERIOD_MS, NULL);
		}
	}

	return pdPASS;
}

BaseType_t wake_lock_FromISR(wakelock_t * lock)
{
	return wake_lock_interval_FromISR(lock, 0);
}

BaseType_t wake_lock_timeout_FromISR(wakelock_t * lock, long timeout_ms)
{
	return wake_lock_interval_FromISR(lock, timeout_ms);
}

/**
 * @brief to released wakelock from ISR
 * @param[in] lock is the wakelock will be released
 */
BaseType_t wake_unlock_FromISR(wakelock_t * lock)
{
	/* Ensure the lock is valid */
	configASSERT(lock);
	configASSERT(lock->xTimer);
#if WAKELOCK_DEBUG
	PRINTF_D("[%s] lock id:%x\n", __func__, lock);
#endif

	/*it's in ISR, no need to protect xWakelocksList from other task accessing */
	if (listLIST_ITEM_CONTAINER(&(lock->xGenericListItem)) != NULL) {
		/* the lock is on the list */
		/* stop the timer if it is active */
		xTimerStopFromISR(lock->xTimer, NULL);

		(void) uxListRemove(&(lock->xGenericListItem));
	} else {
		/*the lock is not held, do nothing */
	}
	lock->xExpiredTick = 0;

	return pdPASS;
}

/**
 * @brief to released wakelock from task
 * @param[in] lock is the wakelock will be released
 */
BaseType_t wake_unlock(wakelock_t * lock)
{
	BaseType_t xReturn = pdPASS;

	/* Ensure the lock is valid */
	configASSERT(lock);
	configASSERT(lock->xTimer);
#if WAKELOCK_DEBUG
	PRINTF_D("[%s] lock id:%x\n", __func__, lock);
#endif

	/*enter critical section to protect xWakelocksList from other task/isr accessing */
	taskENTER_CRITICAL();
	/* stop the timer if it is active */
	if (xTimerIsTimerActive(lock->xTimer) != pdFALSE) {
		xReturn = xTimerStop(lock->xTimer, tmrNO_DELAY);
		if (xReturn == pdFAIL) {
			PRINTF_D("[%s] fail to stop timer\n", __func__);
			goto _exit;
		}
	}

	if (listLIST_ITEM_CONTAINER(&(lock->xGenericListItem)) != NULL) {
		/* the lock is on the list */
		(void) uxListRemove(&(lock->xGenericListItem));
	} else {
		/*the lock is not held, do nothing */
	}
	lock->xExpiredTick = 0;

      _exit:
	/* leave critical section */
	taskEXIT_CRITICAL();

	return xReturn;
}

/**
 * @brief to check the wakelock list is empty
 * @return pdTRUE wake lock list is empty, it is ok to sleep
 * @return pdFALSE wake lock list has items, it cannot sleep
 */
BaseType_t wake_lock_is_empty(void)
{
	if (listLIST_IS_EMPTY(&xWakelocksList) != pdFALSE)
		return pdTRUE;
	else
		return pdFALSE;
}

/**
 * @brief to dump the wakelock list, for debug
 */
void wake_lock_list_dump(void)
{
	volatile wakelock_t *pxNextLock, *pxFirstLock;
	List_t *pxList;

	pxList = &xWakelocksList;
	taskENTER_CRITICAL();
	if (listCURRENT_LIST_LENGTH(pxList) > (UBaseType_t) 0) {
		listGET_OWNER_OF_NEXT_ENTRY(pxFirstLock, pxList);
		do {
			listGET_OWNER_OF_NEXT_ENTRY(pxNextLock, pxList);
			PRINTF_D("wakelock:%s \n", pxNextLock->pcLockName);
		} while (pxFirstLock != pxNextLock);
	}
	taskEXIT_CRITICAL();
}
