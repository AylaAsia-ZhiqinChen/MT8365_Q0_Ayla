/*
 * Copyright (C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */


#include <assert.h>
#include <compiler.h>
#include <debug.h>
#include <err.h>
#include <list.h>
#include <trace.h>

#include <kernel/thread.h>
#include <lk/init.h>
#include <sys/types.h>

#include <lib/trusty/handle.h>
#include <kernel/vm.h>
#include <lib/trusty/ipc.h>
#include <lib/trusty/ipc_msg.h>
#include <lib/trusty/uuid.h>

#include <mtktimer.h>

#define LOCAL_TRACE  0

static uint32_t *SEJ_VA_BASE;

typedef void (*ipc_event_handler_proc_t) (handle_t *handle, uint32_t evt, void *ctx);

typedef struct ipc_event_handler {
	ipc_event_handler_proc_t proc;
	void *priv;
} ipc_event_handler_t;

struct mtktimer_chan_ctx {
        ipc_event_handler_t evt_handler;
        handle_t *chan;
        uuid_t uuid;
};

static void mtktimer_port_handler(handle_t *handle, uint32_t event, void *priv);
static void mtktimer_chan_handler(handle_t *handle, uint32_t event, void *priv);

static ipc_event_handler_t mtktimer_port_evt_handler = {
        .proc = mtktimer_port_handler,
};

static handle_t *mtktimer_port;
static thread_t *mtktimer_thread;
static handle_list_t mtktimer_handles = HANDLE_LIST_INITIAL_VALUE(mtktimer_handles);

void init_mtktimer(void)
{
	status_t err;

	err = vmm_alloc_physical(vmm_get_kernel_aspace(), "mtktimer",
			PAGE_SIZE, (void **)&SEJ_VA_BASE, PAGE_SIZE_SHIFT, SEJ_BASE,
			0, ARCH_MMU_FLAG_UNCACHED_DEVICE);
	if (err)
		dprintf(CRITICAL, "%s: failed %d\n", __func__, err);

	*SEJ_VA_CON &= 0xFFFF7FFF;
	*SEJ_APXGPT1_PRESCALE |= 0x10;
	*SEJ_APXGPT1_CON = 0x00000032;
	*SEJ_APXGPT1_CON = 0x00000031;
}

int get_mtktimer(struct mtktimer_chan_ctx *ctx, struct mtktimer_msg *hdr)
{
	uint64_t lo, hi1, hi2;
	do {
		hi1 = *SEJ_APXGPT1_COUNTER_1;
		lo = *SEJ_APXGPT1_COUNTER_0;
		hi2 = *SEJ_APXGPT1_COUNTER_1;
	} while (hi1 != hi2);

	hdr->timer = (hi1 << 32) | lo;
	hdr->timer /= 32;  /* 32KHz */
	hdr->cmd = GET_MTKTIMER_DONE;

	iovec_kern_t iovs = {
        .base = hdr,
        .len = sizeof(*hdr),
    };
    ipc_msg_kern_t msg = {
            .iov = &iovs,
            .num_iov = 1,
            .handles = NULL,
            .num_handles = 0,
    };

    return ipc_send_msg(ctx->chan, &msg);
}

static int mtktimer_chan_handle_msg(struct mtktimer_chan_ctx *ctx)
{
        int rc;
        ipc_msg_info_t mi;
        struct mtktimer_msg hdr;

        DEBUG_ASSERT(ctx);

        /* get message */
        rc = ipc_get_msg(ctx->chan, &mi);
        if (rc) {
                LTRACEF("ipc_get_msg failed (%d) for chan %p\n",
                        rc, ctx->chan);
                return rc;
        }

        /* check message size */
        if (mi.len != sizeof(hdr)) {
                LTRACEF("unexpected msg size (%d) for chan %p\n",
                        mi.len, ctx->chan);
                return ERR_BAD_LEN;
        }

        /* read incomming request */
        iovec_kern_t iovs = {
                .base = &hdr,
                .len = sizeof(hdr),
        };
        ipc_msg_kern_t msg = {
                .iov = &iovs,
                .num_iov = 1,
                .handles = NULL,
                .num_handles = 0,
        };

        rc = ipc_read_msg(ctx->chan, mi.id, 0, &msg);
        if (rc < 0) {
                LTRACEF("failed (%d) to read the msg (%d) for chan %p\n",
                        rc, mi.len, ctx->chan);
                return rc;
        }
        if ((size_t)rc != mi.len) {
                LTRACEF("partial message read (%d vs. %d) for chan %p\n",
                        rc, mi.len, ctx->chan);
                return ERR_TOO_BIG;
        }

        /* retire an original message */
        ipc_put_msg(ctx->chan, mi.id);

        /* handle it */
        switch (hdr.cmd) {
        case GET_MTKTIMER:
                rc = get_mtktimer(ctx, &hdr);
                break;

        default:
                LTRACEF("Unsupported request\n");
                rc = ERR_NOT_SUPPORTED;
                break;
        }

        return rc;
}

static void mtktimer_close_chan(struct mtktimer_chan_ctx *ctx)
{
        handle_list_del(&mtktimer_handles, ctx->chan);
        handle_close(ctx->chan);

        free(ctx);
}

static void mtktimer_chan_handler(handle_t *chan, uint32_t event, void *priv)
{
        struct mtktimer_chan_ctx *ctx = priv;

        DEBUG_ASSERT(ctx);
        DEBUG_ASSERT(chan == ctx->chan);

        if (event & IPC_HANDLE_POLL_HUP) {
                mtktimer_close_chan(ctx);
        } else {
                if (event & IPC_HANDLE_POLL_MSG) {
                        int rc = mtktimer_chan_handle_msg(ctx);
                        if (rc) {
                                mtktimer_close_chan(ctx);
                        }
                }
        }
}

static void mtktimer_port_handler(handle_t *port, uint32_t event, void *priv)
{
        const uuid_t *peer_uuid;

        if (event & IPC_HANDLE_POLL_READY) {
                handle_t *chan;

                /* incoming connection: accept it */
                int rc = ipc_port_accept(port, &chan, &peer_uuid);
                if (rc < 0) {
                        LTRACEF("failed (%d) to accept on port %p\n",
                                 rc, port);
                        return;
                }

                /* allocate state */
                struct mtktimer_chan_ctx *ctx = calloc(1, sizeof(*ctx));
                if (!ctx) {
                        LTRACEF("failed to alloc state for chan %p\n", chan);
                        handle_close(chan);
                        return;
                }

                /* init channel state */
                ctx->evt_handler.priv = ctx;
                ctx->evt_handler.proc = mtktimer_chan_handler;
                ctx->chan = chan;
                ctx->uuid = *peer_uuid;

                /* attach channel handler */
                handle_set_cookie(chan, &ctx->evt_handler);

                /* add to handle tracking list */
                handle_list_add(&mtktimer_handles, chan);
        }
}

static status_t init_mtktimer_ipc(void)
{
	int rc;

	rc = ipc_port_create(&zero_uuid, MTKTIMER_PORT, 1, MAX_MSG_SIZE, IPC_PORT_ALLOW_TA_CONNECT, &mtktimer_port);
	if (rc != NO_ERROR) {
		TRACEF("failed (%d) to create port '%s'\n", rc, MTKTIMER_PORT);
		return rc;
	}
	handle_set_cookie(mtktimer_port, &mtktimer_port_evt_handler);

	rc = ipc_port_publish(mtktimer_port);
	if (rc != NO_ERROR) {
		TRACEF("failed (%d) to publish port '%s'\n", rc, MTKTIMER_PORT);
		return rc;
	}
	handle_list_add(&mtktimer_handles, mtktimer_port);

	TRACEF("%s created\n", MTKTIMER_PORT);
	return NO_ERROR;
}

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

	handle_list_del(&mtktimer_handles, handle);
	handle_close(handle);
}

static int mtktimer_ipc_thread(void *arg)
{
	int ret;
	handle_t *handle;
	uint32_t  event;

	LTRACEF("enter\n");

	for (;;) {
		/* wait for incoming messgages */
		ret = handle_list_wait(&mtktimer_handles, &handle,
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

static void init_mtktimer_service(uint level)
{
	init_mtktimer();
	init_mtktimer_ipc();

	mtktimer_thread = thread_create("mtktimer_service", mtktimer_ipc_thread,
					NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
	if (!mtktimer_thread) {
		TRACEF("WARNING: failed for start mtktimer thread\n");
		return;
	}

	thread_detach_and_resume(mtktimer_thread);
}

LK_INIT_HOOK(mtktimer_service, init_mtktimer_service, LK_INIT_LEVEL_APPS);
