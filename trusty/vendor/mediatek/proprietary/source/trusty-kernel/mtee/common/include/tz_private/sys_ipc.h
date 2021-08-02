/*
* Copyright (c) 2015 MediaTek Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files
* (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __MTEE_SYSTEM_IPC_H__
#define __MTEE_SYSTEM_IPC_H__

#include "tz_cross/trustzone.h"
#include "tz_private/system.h"
#include <stddef.h>
#include <kernel/spinlock.h>
#include <kernel/mutex.h>
#include <kernel/semaphore.h>

//#define MTEE_LOCK_DEBUG

/**
 * Spinlock define
 *
 * @param lock    locker.
 */
typedef spin_lock_t MTEE_SPINLOCK;

/**
 * Mutex define
 *
 * @param mutex    mutex id.
 */
typedef mutex_t MTEE_MUTEX;

/**
 * Semaphore define
 *
 * @param semaphore    semaphore id.
 */
typedef semaphore_t MTEE_SEMAPHORE;

/**
 * Wait queue define
 *
 * @param waitqueue    waitqueue id.
 */
typedef uint32_t MTEE_WAITQUEUE;

/**
 * TEE built-in IPC
 *
 * These IPC functions are implemented in TEE.
 */

/* Spinlock
*/

/**
 * Spinlock initializer, used in lock declare.
 *
 * Usage:
 *    MTEE_SPINLOCK mylock = MTEE_SPINLOCK_INIT_UNLOCK;
 */
#define MTEE_SPINLOCK_INIT_UNLOCK    SPIN_LOCK_INITIAL_VALUE

/**
 * Init spinlock, used in code
 *
 * @param lock The lock to init
 * @param init_lock Init lock state.
 */
static inline void MTEE_InitSpinlock(MTEE_SPINLOCK *lock, int init_lock)
{
    *lock = (init_lock) ? 1 : MTEE_SPINLOCK_INIT_UNLOCK;
}

/**
 * Check if the spin lock is locked.
 *
 * @param lock The lock to check
 */
static inline int MTEE_SpinIsLocked(MTEE_SPINLOCK *lock)
{
    return spin_lock_held(lock);
}

/**
 * Grab the spin lock and mask IRQ/FIQ
 *
 * @param lock    The spin lock to take.
 * @param state    Previous IRQ/FIQ state to restore to.
 */
static inline uint32_t MTEE_SpinLockMaskIrq(MTEE_SPINLOCK *lock)
{
    spin_lock_saved_state_t state;
    spin_lock_save(lock, &state, SPIN_LOCK_FLAG_IRQ_FIQ);
    return state;
}

/**
 * Unlock spin lock and restore IRQ to previous state.
 *
 * @param lock    The spin lock to unlock
 * @param state    Previous IRQ state to restore to.
 */
static inline void MTEE_SpinUnlockRestoreIrq(MTEE_SPINLOCK *lock, uint32_t state)
{
    spin_unlock_restore(lock, state, SPIN_LOCK_FLAG_IRQ_FIQ);
}

/**
 * IPC through REE service
 *
 * These IPC functions are implemented through REE service.
 * By REE service, use normal world OS's IPC functions.
 *
 * NOTE: It is simple, but not as secure as built-in solution.
 * It may be used for attacking. The major weapon will be race condition.
 */

/* Mutex
*/
/**
 * Mutex, create
 *
 * @return    A pointer to the the mutex. If fail, return 0.
 */
MTEE_MUTEX *MTEE_CreateMutex (void);

/**
 * Mutex, destory
 *
 * @param mutex    A pointer to the mutex.
 * @return    0 if success. <0 for error.
 */
int MTEE_DestoryMutex (MTEE_MUTEX *mutex);

/**
 * Mutex, lock
 *
 * @param mutex    A pointer to the mutex.
 */
void MTEE_LockMutex (MTEE_MUTEX *mutex);

/**
 * Mutex, unlock
 *
 * @param mutex    A pointer to the mutex.
 */
void MTEE_UnlockMutex (MTEE_MUTEX *mutex);

/**
 * Mutex, try lock
 *
 * @param mutex    A pointer to the mutex.
 * @return    1 if the mutex has been acquired successfully, and 0 on contention.
 */

int MTEE_TryLockMutex (MTEE_MUTEX *mutex);

/**
 * Mutex, querey if it is locked or not
 *
 * @param mutex    A pointer to the mutex.
 * @return    1 if the mutex is locked, 0 if unlocked..
 */
int MTEE_IsLockedMutex (MTEE_MUTEX *mutex);

/* Semaphore
*/
/**
 * Semaphore, create
 *
 * @param val      initial value.
 * @return    A pointer to the the semaphore. If fail, return 0.
 */
MTEE_SEMAPHORE *MTEE_CreateSemaphore (int val);

/**
 * Semaphore, free
 *
 * @param semaphore    A pointer to the semaphore.
 * @return    0 if success. <0 for error.
 */
int MTEE_DestroySemaphore (MTEE_SEMAPHORE *semaphore);

/**
 * Semaphore, down
 *
 * @param semaphore    A pointer to the semaphore.
 */
void MTEE_DownSemaphore (MTEE_SEMAPHORE *semaphore);

/**
 * Semaphore, down with time-out
 *
 * @param semaphore    A pointer to the semaphore.
 * @param timeout    timeout value, in jiffies
 * @return    0 if the @timeout elapsed, and the remaining
 * jiffies if the condition evaluated to true before the timeout elapsed.
 */
int MTEE_DownSemaphore_Timeout (MTEE_SEMAPHORE *semaphore, int timeout);

/**
 * Semaphore, down try lock
 *
 * @param semaphore    A pointer to the semaphore.
 * @return    0 if semaphore is avalibale.
 */
int MTEE_DownTryLockSemaphore (MTEE_SEMAPHORE *semaphore);

/**
 * Semaphore, up
 *
 * @param semaphore    A pointer to the semaphore.
 */
void MTEE_UpSemaphore (MTEE_SEMAPHORE *semaphore);

#if 0
/* Wait queue
*/
/**
 * Wait queue, create
 *
 * @return    A pointer to the waitqueue. If fail, return 0.
 */
MTEE_WAITQUEUE *MTEE_CreateWaitQueue (void);

/**
 * Wait queue, free
 *
 * @param waitqueue    A pointer to the waitqueue.
 * @return    0 if success. <0 for error.
 */
int MTEE_DestroyWaitQueue (MTEE_WAITQUEUE *waitqueue);

/**
 * Wait queue, wait event
 *
 * @param waitqueue    A pointer to the waitqueue.
 * @condition    The event to wait for
 */
void MTEE_WaitEvent (MTEE_WAITQUEUE *waitqueue, int condition);

/**
 * Wait queue, wait event with timeout
 *
 * @param waitqueue    A pointer to the waitqueue.
 * @condition    The event to wait for
 * @timeout    timeout value, in jiffies
 * @return     0 if the @timeout elapsed, and the remaining
 * jiffies if the condition evaluated to true before the timeout elapsed.
 */
int MTEE_WaitEvent_Timeout (MTEE_WAITQUEUE *waitqueue, int condition, int timeout);

/**
 * Wait queue, wake up
 *
 * @param waitqueue    A pointer to the waitqueue.
 */
void MTEE_WakeUpWaitQueue (MTEE_WAITQUEUE *waitqueue);
#endif

#endif /* __MTEE_SYSTEM_IPC_H__ */
