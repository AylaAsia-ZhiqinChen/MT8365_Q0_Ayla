/*
 * Copyright (c) 2013-2018, Google, Inc. All rights reserved
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

#ifndef __LIB_TRUSTY_HANDLE_H
#define __LIB_TRUSTY_HANDLE_H

#include <list.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#include <kernel/event.h>
#include <kernel/mutex.h>

#include <refcount.h>

/* bitmask */
enum {
	IPC_HANDLE_POLL_NONE	= 0x0,
	IPC_HANDLE_POLL_READY	= 0x1,
	IPC_HANDLE_POLL_ERROR	= 0x2,
	IPC_HANDLE_POLL_HUP	= 0x4,
	IPC_HANDLE_POLL_MSG	= 0x8,
	IPC_HANDLE_POLL_SEND_UNBLOCKED = 0x10,
};

enum handle_flags {
	HANDLE_FLAG_NO_SEND = (1U << 0),
};

struct handle_ops;

typedef struct handle {
	refcount_t		refcnt;
	uint32_t		flags;

	struct handle_ops	*ops;

	/* pointer to a wait queue on which threads wait for events for this
	 * handle.
	 */
	event_t			*wait_event;
	spin_lock_t		slock;

	struct list_node	hlist_node;
	struct list_node	waiter_list;

	void			*cookie;
} handle_t;

struct handle_waiter {
	struct list_node node;
	void (*notify_proc)(struct handle_waiter *);
};

struct handle_event_waiter {
	struct handle_waiter waiter;
	struct event event;
};

static void handle_event_waiter_notify(struct handle_waiter *hw)
{
	struct handle_event_waiter *hew;
	hew = containerof(hw, struct handle_event_waiter, waiter);
	event_signal(&hew->event, false);
}

#define HANDLE_EVENT_WAITER_INITIAL_VALUE(ew)              \
{                                                          \
	.waiter = {                                        \
		.node = LIST_INITIAL_CLEARED_VALUE,        \
		.notify_proc = handle_event_waiter_notify  \
	},                                                 \
	.event = EVENT_INITIAL_VALUE((ew).event, false,    \
	                              EVENT_FLAG_AUTOUNSIGNAL), \
}

/**
 * struct handle_ref - struct representing handle reference
 * @set_node:   list node used with set_list of handle_set struct
 * @ready_node: list node used with ready_list of handle_set struct
 * @uctx_node:  list node used with ref_lists of uctx struct
 * @waiter:     used with waiter_list of handle struct
 * @parent:     pointer to parent handle set if any
 * @handle:     pointer to underlying handle struct
 * @id:         corresponds to handle field in uevent struct
 * @emask:      event mask
 * @cookie:     corresponds to cookie field in uevent struct
 */
struct handle_ref {
	struct list_node set_node;
	struct list_node ready_node;
	struct list_node uctx_node;
	struct handle_waiter waiter;
	struct handle *parent;
	struct handle *handle;
	uint32_t id;
	uint32_t emask;
	void    *cookie;
};

struct handle_ops {
	uint32_t (*poll)(handle_t *handle, uint32_t emask, bool finalize);
	void (*shutdown)(handle_t *handle);
	void (*destroy)(handle_t *handle);
};

typedef struct handle_list {
	struct list_node	handles;
	mutex_t			lock;
	event_t			*wait_event;
} handle_list_t;

#define HANDLE_LIST_INITIAL_VALUE(hs) \
{ \
	.handles	= LIST_INITIAL_VALUE((hs).handles), \
	.lock		= MUTEX_INITIAL_VALUE((hs).lock), \
}

/* handle management */
void handle_init_etc(handle_t *handle, struct handle_ops *ops, uint32_t flags);

static inline void handle_init(handle_t *handle, struct handle_ops *ops)
{
	handle_init_etc(handle, ops, 0);
}
void handle_close(handle_t *handle);

void handle_incref(handle_t *handle);
void handle_decref(handle_t *handle);

void handle_add_waiter(struct handle *h, struct handle_waiter *w);
void handle_del_waiter(struct handle *h, struct handle_waiter *w);

int handle_wait(handle_t *handle, uint32_t *handle_event, lk_time_t timeout);
int handle_ref_wait(const struct handle_ref *in, struct handle_ref *out,
                    lk_time_t timeout);
void handle_notify(handle_t *handle);
void handle_notify_waiters_locked(handle_t *handle);

static inline void handle_set_cookie(handle_t *handle, void *cookie)
{
	handle->cookie = cookie;
}

static inline void *handle_get_cookie(handle_t *handle)
{
	return handle->cookie;
}

void handle_list_init(handle_list_t *hlist);
void handle_list_add(handle_list_t *hlist, handle_t *handle);
void handle_list_del(handle_list_t *hlist, handle_t *handle);
void handle_list_delete_all(handle_list_t *hlist);
int handle_list_wait(handle_list_t *hlist, handle_t **handle_ptr,
		     uint32_t *event_ptr, lk_time_t timeout);

static inline bool handle_is_sendable(struct handle *h)
{
	return !(h->flags & HANDLE_FLAG_NO_SEND);
}

#endif
