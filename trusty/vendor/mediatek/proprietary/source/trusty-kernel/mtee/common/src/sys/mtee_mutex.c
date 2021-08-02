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

#include "tz_private/sys_ipc.h"
#include <malloc.h>
#include <err.h>
#include <kernel/thread.h>

MTEE_MUTEX *MTEE_CreateMutex (void)
{
	MTEE_MUTEX *m = malloc(sizeof(MTEE_MUTEX));
	if (!m) return NULL;

	mutex_init(m);
	return m;
}

int MTEE_DestoryMutex (MTEE_MUTEX *mutex)
{
	mutex_destroy(mutex);
	free(mutex);
	return 0;
}

void MTEE_LockMutex (MTEE_MUTEX *mutex)
{
	mutex_acquire(mutex);
}

void MTEE_UnlockMutex (MTEE_MUTEX *mutex)
{
	mutex_release(mutex);
}

int MTEE_TryLockMutex (MTEE_MUTEX *mutex)
{
	if (mutex_acquire_timeout(mutex, 0) == NO_ERROR)
		return 1;

	return 0;
}
int MTEE_IsLockedMutex (MTEE_MUTEX *mutex)
{
	if (mutex->count <= 0) return 0;
	return 1;
}

