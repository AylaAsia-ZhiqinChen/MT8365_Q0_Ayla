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

#include "tz_private/system.h"
#include <kernel/thread.h>
#include <malloc.h>
#include <list.h>
#include <assert.h>

typedef struct _thread_wrap_handle {
	struct list_node list;
	MTEE_ThreadFunc func;
	void *user_data;
	void *ret;
	MTEE_THREAD_HANDLE mtee_h;
	thread_t *t;
} thread_wrap_handle;

static int _thread_wrap(void* arg)
{
	thread_wrap_handle *h = (thread_wrap_handle *)arg;
	h->ret = h->func(h->user_data);
	return 0;
}

/* scheduler lock */
static spin_lock_t mtee_thread_lock = SPIN_LOCK_INITIAL_VALUE;
#define MTEE_THREAD_LOCK(state) spin_lock_saved_state_t state; spin_lock_irqsave(&mtee_thread_lock, state)
#define MTEE_THREAD_UNLOCK(state) spin_unlock_irqrestore(&mtee_thread_lock, state)

static struct list_node mtee_thread_list = LIST_INITIAL_VALUE(mtee_thread_list);

static thread_wrap_handle* get_thread_by_mtee_handle(MTEE_THREAD_HANDLE mtee_h)
{
	thread_wrap_handle *h = NULL;

	MTEE_THREAD_LOCK(state);

	list_for_every_entry(&mtee_thread_list, h, thread_wrap_handle, list) {
		if (h->mtee_h == mtee_h)
			break;
	}
	MTEE_THREAD_UNLOCK(state);

	return h;
}

TZ_RESULT MTEE_CreateThread (MTEE_THREAD_HANDLE *handle, MTEE_ThreadFunc fuc, void *user_data, char *name)
{
	thread_wrap_handle *h;

	h = malloc(sizeof(thread_wrap_handle));
	if (!h) {
		return TZ_RESULT_ERROR_OUT_OF_MEMORY;
	}

	h->mtee_h = *handle = (MTEE_THREAD_HANDLE)(&(h->mtee_h));

	h->func = fuc;
	h->user_data = user_data;
	h->t = thread_create(name, _thread_wrap, h, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
	if (!(h->t)) {
		free(h);
		return TZ_RESULT_ERROR_GENERIC;
	}

	MTEE_THREAD_LOCK(state);
	list_add_head(&mtee_thread_list, &(h->list));
	MTEE_THREAD_UNLOCK(state);

	thread_resume(h->t);

	return TZ_RESULT_SUCCESS;
}

TZ_RESULT MTEE_JoinThread (MTEE_THREAD_HANDLE handle, void **result)
{
	thread_wrap_handle *h;
	int r;

	h = get_thread_by_mtee_handle(handle);
	ASSERT(h != NULL);

	thread_join(h->t, &r, INFINITE_TIME);
	*result = h->ret;

	MTEE_THREAD_LOCK(state);
	list_delete(&(h->list));
	MTEE_THREAD_UNLOCK(state);

	free(h);
	return TZ_RESULT_SUCCESS;
}

