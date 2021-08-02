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

MTEE_SEMAPHORE *MTEE_CreateSemaphore (int val)
{
	MTEE_SEMAPHORE *s = malloc(sizeof(MTEE_SEMAPHORE));
	if (!s) return NULL;

	sem_init(s, val);
	return s;
}

int MTEE_DestroySemaphore (MTEE_SEMAPHORE *semaphore)
{
	sem_destroy(semaphore);
	free(semaphore);
	return 0;
}

void MTEE_DownSemaphore (MTEE_SEMAPHORE *semaphore)
{
	sem_wait(semaphore);
}

int MTEE_DownSemaphore_Timeout (MTEE_SEMAPHORE *semaphore, int timeout)
{
	if (sem_timedwait(semaphore, timeout) == ERR_TIMED_OUT ) 
		return 0;
	if ((timeout-1)>0) 
		return timeout-1;
	return 1;
}

int MTEE_DownTryLockSemaphore (MTEE_SEMAPHORE *semaphore)
{
	if (sem_trywait(semaphore) == NO_ERROR)
		return 0;

	return 1;
}

void MTEE_UpSemaphore (MTEE_SEMAPHORE *semaphore)
{
	sem_post(semaphore, true);
}

