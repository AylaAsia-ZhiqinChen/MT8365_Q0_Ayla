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

#pragma once

#include <sys/types.h>
#include <lib/trusty/handle.h>
#include <lib/trusty/ipc.h>
#include <lib/trusty/ipc_msg.h>
#include <lib/trusty/uuid.h>

typedef void (*ipc_event_handler_proc_t) (handle_t *handle, uint32_t evt, void *ctx);

typedef struct ipc_event_handler {
	ipc_event_handler_proc_t proc;
	void *priv;
} ipc_event_handler_t;

extern handle_list_t mtcrypto_handles;

status_t mtcrypto_init_hwrng(void);
status_t mtcrypto_init_hwkey(void);

int ipc_send_single_buf(handle_t *chan, const void *buf, uint32_t len);
int ipc_recv_single_buf(handle_t *chan, void *buf, uint32_t len);

static inline void ipc_msg_init_one(ipc_msg_kern_t *msg, iovec_kern_t *iov,
				    void *base, uint32_t len)
{
	iov->base = base;
	iov->len = len;
	msg->iov = iov;
	msg->num_iov = 1;
	msg->handles = NULL;
	msg->num_handles = 0;
}

static inline void ipc_msg_init_two(ipc_msg_kern_t *msg, iovec_kern_t *iov,
				    void *base0, uint32_t len0,
				    void *base1, uint32_t len1)
{
	iov[0].base = base0;
	iov[0].len  = len0;
	iov[1].base = base1;
	iov[1].len  = len1;
	msg->iov = iov;
	msg->num_iov = 2;
	msg->handles = NULL;
	msg->num_handles = 0;
}
