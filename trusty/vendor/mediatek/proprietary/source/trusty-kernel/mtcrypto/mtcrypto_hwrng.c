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
#include <interface/hwrng//hwrng.h>
#include <string.h>
#include <sys/types.h>
#include <trace.h>

#include "mtcrypto_priv.h"

#define LOCAL_TRACE  0

#define HWRNG_SRV_NAME  HWRNG_PORT

/*
 * Max amount of data that can be returned in a single message.
 * This value could be adjusted depending on driver capabilities
 * trading off memory usage and request handling efficiency.
 */
#define MAX_HWRNG_MSG_SIZE  1024

struct hwrng_chan_ctx {
	ipc_event_handler_t evt_handler;
	struct list_node node;
	handle_t *chan;
	size_t req_size;
	bool send_blocked;
};

static void hwrng_port_handler(handle_t *handle, uint32_t event, void *priv);
static void hwrng_chan_handler(handle_t *handle, uint32_t event, void *priv);

static ipc_event_handler_t hwrng_port_evt_handler = {
	.proc = hwrng_port_handler,
};

static handle_t *hwrng_port;
static struct list_node hwrng_req_list;


/*
 * Close specified HWRNG service channel
 */
static void hwrng_close_chan(struct hwrng_chan_ctx *ctx)
{
	handle_list_del(&mtcrypto_handles, ctx->chan);
	handle_close(ctx->chan);

	if (list_in_list(&ctx->node))
		list_delete(&ctx->node);

	free(ctx);
}

__WEAK uint32_t plat_gen_rng_data(uint8_t *rng_data, size_t rng_len)
{

	return 0;
}

/*
 * Handle HWRNG request queue
 */
static bool hwrng_handle_req_queue(void)
{
	struct hwrng_chan_ctx *ctx;
	struct hwrng_chan_ctx *temp;

	/* service channels */
	bool need_more = false;

	/* for all pending requests */
	list_for_every_entry_safe(&hwrng_req_list, ctx, temp,
				  struct hwrng_chan_ctx, node) {

		if (ctx->send_blocked)
			continue; /* cant service it rignt now */

		size_t len = ctx->req_size;

		if (len > MAX_HWRNG_MSG_SIZE)
			len = MAX_HWRNG_MSG_SIZE;

		/* Get platform trng data */
		uint8_t *rng_data = malloc(len);
		plat_gen_rng_data(rng_data, len);

		/*
		 * Generate 'len'bytes of random data and send it back
		 * to caller.
		 */

		/* send reply */
		int rc = ipc_send_single_buf(ctx->chan, rng_data, len);
		free(rng_data);
		if (rc < 0) {
			if (rc == ERR_NOT_ENOUGH_BUFFER) {
				/* mark it as send_blocked */
				ctx->send_blocked = true;
			} else {
				/* just hw rng request channel */
				LTRACEF("failed (%d) to send_reply\n", rc);
				hwrng_close_chan(ctx);
			}
			continue;
		}

		ctx->req_size -= len;

		if (ctx->req_size == 0) {
			/* remove it from pending list */
			list_delete(&ctx->node);
		} else {
			need_more = true;
		}
	}

	return need_more;
}

/*
 * Check if we need handle request queue
 */
static void hwrng_kick_req_queue(void)
{
	while (hwrng_handle_req_queue()) ;
}

/*
 *  Read and queue HWRNG request message
 */
static int hwrng_chan_handle_msg(struct hwrng_chan_ctx *ctx)
{
	int rc;
	ipc_msg_info_t mi;
	struct hwrng_req req;

	DEBUG_ASSERT(ctx);

	/* get message */
	rc = ipc_get_msg(ctx->chan, &mi);
	if (rc) {
		LTRACEF("ipc_get_msg failed (%d) for chan %p\n",
			rc, ctx->chan);
		return rc;
	}

	/* check message size */
	if (mi.len != sizeof(req)) {
		LTRACEF("unexpected msg size (%d) for chan %p\n",
			mi.len, ctx->chan);
		return ERR_BAD_LEN;
	}

	/* read request */
	ipc_msg_kern_t msg;
	iovec_kern_t   iov;
	ipc_msg_init_one(&msg, &iov, &req, sizeof(req));
	rc = ipc_read_msg(ctx->chan, mi.id, 0, &msg);
	if (rc != sizeof(req)) {
		LTRACEF("failed (%d) to read msg (%d) for chan %p\n",
			rc, mi.id, ctx->chan);
		return rc;
	}

	/* check if we already have request in progress */
	if (list_in_list(&ctx->node)) {
		/* extend it */
		ctx->req_size += req.len;
	} else {
		/* queue it */
		ctx->req_size = req.len;
		list_add_tail(&hwrng_req_list, &ctx->node);
	}

	/* retire original message */
	ipc_put_msg(ctx->chan, mi.id);
	return 0;
}

/*
 *  HWRNG service channel event handler
 */
static void hwrng_chan_handler(handle_t *chan, uint32_t event, void *priv)
{
	struct hwrng_chan_ctx *ctx = priv;

	DEBUG_ASSERT(ctx);
	DEBUG_ASSERT(chan == ctx->chan);

	if (event & IPC_HANDLE_POLL_HUP) {
		hwrng_close_chan(ctx);
	} else {
		if (event & IPC_HANDLE_POLL_SEND_UNBLOCKED) {
			ctx->send_blocked = false;
		}

		if (event & IPC_HANDLE_POLL_MSG) {
			int rc = hwrng_chan_handle_msg(ctx);
			if (rc) {
				hwrng_close_chan(ctx);
			}
		}
	}

	/* kick state machine */
	hwrng_kick_req_queue();
}

/*
 * HWRNG service port event handler
 */
static void hwrng_port_handler(handle_t *port, uint32_t event, void *priv)
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
		struct hwrng_chan_ctx *ctx = calloc(1, sizeof(*ctx));
		if (!ctx) {
			LTRACEF("failed to alloc state for chan %p\n", chan);
			handle_close(chan);
			return;
		}

		/* init channel state */
		ctx->evt_handler.priv = ctx;
		ctx->evt_handler.proc = hwrng_chan_handler;
		ctx->chan = chan;

		/* attach channel handler */
		handle_set_cookie(chan, &ctx->evt_handler);

		/* add to handle tracking list */
		handle_list_add(&mtcrypto_handles, chan);
	}
}

/*
 *  Initialize HWRNG service
 */
status_t mtcrypto_init_hwrng(void)
{
	int rc;

	list_initialize(&hwrng_req_list);

	/* Create HWRNG port */
	rc = ipc_port_create(&zero_uuid, HWRNG_SRV_NAME,
			     1, MAX_HWRNG_MSG_SIZE, IPC_PORT_ALLOW_TA_CONNECT,
			     &hwrng_port);
	if (rc != NO_ERROR) {
		LTRACEF("failed (%d) to create port '%s'\n",
			rc, HWRNG_SRV_NAME);
		return rc;
	}
	handle_set_cookie(hwrng_port, &hwrng_port_evt_handler);

	rc = ipc_port_publish(hwrng_port);
	if (rc != NO_ERROR) {
		LTRACEF("failed (%d) to publish port '%s'\n",
			rc, HWRNG_SRV_NAME);
		return rc;
	}
	handle_list_add(&mtcrypto_handles, hwrng_port);

	LTRACEF("%s created\n", HWRNG_SRV_NAME);

	return NO_ERROR;
}
