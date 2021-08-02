/* 
* Copyright (C) 2015 MediaTek Inc. 
*
* Modification based on code covered by the below mentioned copyright
* and/or permission notice(S). 
*/

/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <assert.h>
#include <compiler.h>
#include <debug.h>
#include <err.h>
#include <list.h>
#include <trace.h>

#include <kernel/thread.h>

#if WITH_HWCRYPTO_UNITTEST
#include <lib/trusty/trusty_app.h>
#endif

#include <lk/init.h>
#include <sys/types.h>

#include "mtcrypto_priv.h"

#define LOCAL_TRACE  0

static thread_t *mtcrypto_thread;
handle_list_t mtcrypto_handles = HANDLE_LIST_INITIAL_VALUE(mtcrypto_handles);

/****************************************************************************/

/*
 * Handle common unexpected port events
 */
static void handle_port_errors(handle_t *handle, uint32_t event)
{
	if ((event & IPC_HANDLE_POLL_ERROR) ||
	    (event & IPC_HANDLE_POLL_HUP) ||
	    (event & IPC_HANDLE_POLL_MSG) ||
	    (event & IPC_HANDLE_POLL_SEND_UNBLOCKED)) {
		/* should never happen with port handles */
		panic("error event (0x%x) for port (%p)\n", event, handle);
	}
}

/*
 * Handle common unexpected chan events
 */
static void handle_chan_errors(handle_t *handle, uint32_t event)
{
	if ((event & IPC_HANDLE_POLL_ERROR) ||
	    (event & IPC_HANDLE_POLL_READY)) {
		/* should never happen with channel handles */
		panic("error event (0x%x) for chan (%p)\n", event, handle);
	}
}

/*
 *  Send single buf message
 */
int ipc_send_single_buf(handle_t *chan, const void *buf, uint32_t len)
{
	iovec_kern_t iov;
	ipc_msg_kern_t msg;

	ipc_msg_init_one(&msg, &iov, (void *)buf, len);
	return ipc_send_msg(chan, &msg);
}

/*
 *  Receive single buf message
 */
int ipc_recv_single_buf(handle_t *chan, void *buf, uint32_t len)
{
	int rc;
	iovec_kern_t iov;
	ipc_msg_kern_t msg;
	ipc_msg_info_t msg_inf;

	rc = ipc_get_msg(chan, &msg_inf);
	if (rc)
		return rc;

	if (msg_inf.len != len) {
		/* unexpected msg size */
		rc = ERR_BAD_LEN;
	} else {
		ipc_msg_init_one(&msg, &iov, buf, len);
		rc = ipc_read_msg(chan, msg_inf.id, 0, &msg);
	}

	ipc_put_msg(chan, msg_inf.id);
	return rc;
}

static void dispatch_event(handle_t *handle, uint32_t event)
{
	if (event == IPC_HANDLE_POLL_NONE) {
		/* not really an event, do nothing */
		return;
	}

	/* handle common errors */
	if (ipc_is_channel(handle))
		handle_chan_errors(handle, event);

	if (ipc_is_port(handle))
		handle_port_errors(handle, event);

	/* check if we have handler */
	struct ipc_event_handler *handler = handle_get_cookie(handle);
	if (handler && handler->proc) {
		/* invoke it */
		handler->proc(handle, event, handler->priv);
		return;
	}

	LTRACEF("no handler for event (0x%x) with handle %p\n",
	         event, handle);

	handle_list_del(&mtcrypto_handles, handle);
	handle_close(handle);
}

static int mtcrypto_ipc_thread(void *arg)
{
	int ret;
	handle_t *handle;
	uint32_t  event;

	LTRACEF("enter\n");

	for (;;) {
		/* wait for incoming messgages */
		ret = handle_list_wait(&mtcrypto_handles, &handle,
				       &event, INFINITE_TIME);
		if (ret == ERR_NOT_FOUND) {
			/* no handles left */
			LTRACEF("No handles left\n");
			break;
		}
		if (ret < 0) {
			/* only possible if somebody else is waiting
			   on the same handle which should never happen */
			panic("%s: couldn't wait for handle events (%d)\n",
			      __func__, ret);
		}
		dispatch_event(handle, event);
		handle_decref(handle);
	}

	LTRACEF("exiting\n");
	return 0;
}

static void init_mtcrypto_service(uint level)
{
	mtcrypto_init_hwrng();
	mtcrypto_init_hwkey();

	mtcrypto_thread = thread_create("mtcrypto_service", mtcrypto_ipc_thread,
					NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
	if (!mtcrypto_thread) {
		TRACEF("WARNING: failed for start mtcrypto thread\n");
		return;
	}

	thread_detach_and_resume(mtcrypto_thread);
}

LK_INIT_HOOK(mtcrypto_service, init_mtcrypto_service, LK_INIT_LEVEL_APPS);

#if WITH_HWCRYPTO_UNITTEST

/* UUID of HWCRYPTO_UNITTEST application */
static uuid_t hwcrypto_unittest_uuid = \
	{ 0xab742471, 0xd6e6, 0x4806, \
		{ 0x85, 0xf6, 0x05, 0x55, 0xb0, 0x24, 0xf4, 0xda }};

static void wait_for_hwcrypto_unittest(uint level)
{
	trusty_app_t *ta = trusty_app_find_by_uuid(&hwcrypto_unittest_uuid);

	if(ta!= NULL && ta->thread != NULL && ta->thread->thread != NULL)
		thread_join(ta->thread->thread, NULL, INFINITE_TIME);
}

LK_INIT_HOOK(wait_for_hwcrypto_unittest, wait_for_hwcrypto_unittest, LK_INIT_LEVEL_LAST - 1);

#endif /* WITH_HWCRYPTO_UNITTEST */
