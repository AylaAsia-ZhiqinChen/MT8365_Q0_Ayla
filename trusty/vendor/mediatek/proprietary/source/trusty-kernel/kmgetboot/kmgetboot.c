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
#include <string.h>

#include <kernel/thread.h>
#include <lk/init.h>
#include <sys/types.h>

#include <lib/trusty/handle.h>
#include <lib/trusty/ipc.h>
#include <lib/trusty/ipc_msg.h>
#include <lib/trusty/uuid.h>

#include <kmgetboot.h>


#define LOCAL_TRACE 0

#if 0
#define LOG_TAG "KM_GetBoot"
#define TLOGE(fmt, ...) \
	fprintf(stderr, "%s: %d: " fmt, LOG_TAG, __LINE__,  ## __VA_ARGS__)
#endif

typedef void (*ipc_event_handler_proc_t) (handle_t *handle, uint32_t evt, void *ctx);

typedef struct ipc_event_handler {
	ipc_event_handler_proc_t proc;
	void *priv;
} ipc_event_handler_t;

static handle_t *kmgetboot_port;

struct kmgetboot_chan_ctx {
	ipc_event_handler_t evt_handler;
	handle_t *chan;
	uuid_t uuid;
};

static thread_t *kmgetboot_thread;
static handle_list_t kmgetboot_handles = HANDLE_LIST_INITIAL_VALUE(kmgetboot_handles);

#define MTK_SIP_GET_ROOT_OF_TRUST_AARCH32 0x82000020

void arm32_smc_ret(uint32_t fid, uint32_t *r0, uint32_t *r1, uint32_t *r2, uint32_t *r3)
{
	register uint32_t _r0 __asm__("r0") = fid;
	register uint32_t _r1 __asm__("r1") = 0;
	register uint32_t _r2 __asm__("r2") = 0;
	register uint32_t _r3 __asm__("r3") = 0;

	__asm__ volatile ("smc 0" : "+r"(_r0),
		"+r"(_r1), "+r"(_r2), "+r"(_r3));

	*r0 = _r0;
	*r1 = _r1;
	*r2 = _r2;
	*r3 = _r3;
}

int get_verified_boot(struct kmgetboot_chan_ctx *ctx, ipc_msg_info_t *mi, struct kmgetboot_msg *hdr)
{
	struct boot_param boot = {
		.os_version = 0,
		.os_patchlevel = 0,
		.device_locked = 0,
		.verified_boot_state = 0,
		.verified_boot_key = {0},
	};
	//unsigned char public_key_hash[SHA256_LENGTH] = {0};
	//unsigned int device_lock_state = 0, boot_state = 0, os_version = 0, os_patchlevel = 0;

	uint32_t *p_hash = (uint32_t *)boot.verified_boot_key;
	uint32_t year, month, version_a, version_b, version_c;

	arm32_smc_ret(MTK_SIP_GET_ROOT_OF_TRUST_AARCH32, (p_hash), (p_hash+1), (p_hash+2), (p_hash+3));
	arm32_smc_ret(MTK_SIP_GET_ROOT_OF_TRUST_AARCH32, (p_hash+4), (p_hash+5), (p_hash+6), (p_hash+7));
	arm32_smc_ret(MTK_SIP_GET_ROOT_OF_TRUST_AARCH32, &boot.device_locked, &boot.verified_boot_state, &boot.os_version, &boot.os_patchlevel);

	month = boot.os_version & 0xF;
	boot.os_version >>= 4;
	year = (boot.os_version & 0x7F) + 2000;
	boot.os_version >>= 7;
	version_c = boot.os_version & 0x7F;
	boot.os_version >>= 7;
	version_b = boot.os_version & 0x7F;
	boot.os_version >>= 7;
	version_a = boot.os_version & 0x7F;
	boot.os_version = (version_a % 100) * 10000 + (version_b % 100) * 100 + (version_c % 100);
	boot.os_patchlevel = (year % 10000) * 100 + (month % 100);
#if LOCAL_TRACE
	for (unsigned int i=0; i<SHA256_LENGTH; i+=8)
		LTRACEF("hash [%d-%d] = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
			i, i+7, boot.verified_boot_key[i], boot.verified_boot_key[i+1], boot.verified_boot_key[i+2], boot.verified_boot_key[i+3],
			boot.verified_boot_key[i+4], boot.verified_boot_key[i+5], boot.verified_boot_key[i+6], boot.verified_boot_key[i+7]);
	LTRACEF("device lock state : %d\n", boot.device_locked);
	LTRACEF("verified boot state : %d\n", boot.verified_boot_state);
	LTRACEF("os version : 0x%x\n", boot.os_version);
	LTRACEF("os patchlevel : 0x%x\n", boot.os_patchlevel);
#endif
	hdr->cmd |= (RESP_FLAG | DONE_FLAG);

	iovec_kern_t iov[2] = {
		{
			.base = hdr,
			.len = sizeof(struct kmgetboot_msg),
		},
		{
			.base = &boot,
			.len = sizeof(struct boot_param),
		}
	};
	ipc_msg_kern_t msg = {
		.iov = iov,
		.num_iov = 2,
		.handles = NULL,
		.num_handles = 0,
	};

	return ipc_send_msg(ctx->chan, &msg);
}

#if 0
static uint32_t key_len = 0, in_offset = 0, out_offset = 0;
static uint8_t *in_buf = NULL, *out_buf = NULL;

int set_key_len(struct kmgetboot_chan_ctx *ctx, ipc_msg_info_t *mi, struct kmgetboot_msg *hdr)
{
	int rc = NO_ERROR;
	iovec_kern_t iov = {
		.base = hdr,
		.len = sizeof(struct kmgetboot_msg),
	};
	ipc_msg_kern_t msg = {
		.iov = &iov,
		.num_iov = 1,
		.handles = NULL,
		.num_handles = 0,
	};

	if (key_len != 0 || in_buf != NULL || out_buf != NULL) {
		LTRACEF("dirty state is detected!\n");
		rc = ERR_ALREADY_STARTED;
		goto exit;
	}

	if (mi->len != sizeof(struct kmgetboot_msg) + sizeof(uint32_t)) {
		LTRACEF("msg size is not correct (%d) for cmd (%u)\n", mi->len, hdr->cmd);
		rc = ERR_INVALID_ARGS;
		goto exit;
	}

	memcpy(&key_len, hdr->payload, sizeof(uint32_t));
	if (key_len == 0) {
		LTRACEF("key length is zero!\n");
		rc = ERR_INVALID_ARGS;
		goto exit;
	}

	in_buf = malloc(key_len);
	if (in_buf == NULL) {
		LTRACEF("malloc input buffer failed!\n");
		key_len = 0;
		rc = ERR_NO_MEMORY;
		goto exit;
	}

	out_buf = malloc(key_len);
	if (out_buf == NULL) {
		LTRACEF("malloc output buffer failed!\n");
		key_len = 0;
		free(in_buf);
		rc = ERR_NO_MEMORY;
		goto exit;
	}

	in_offset = 0;
	out_offset = 0;

	hdr->cmd |= (RESP_FLAG | DONE_FLAG);
exit:
	ipc_send_msg(ctx->chan, &msg);

	return rc;
}

int send_key_buf(struct kmgetboot_chan_ctx *ctx, ipc_msg_info_t *mi, struct kmgetboot_msg *hdr)
{
	int rc = NO_ERROR;
	uint32_t buf_len = mi->len - sizeof(struct kmgetboot_msg);
	iovec_kern_t iov = {
		.base = hdr,
		.len = sizeof(struct kmgetboot_msg),
	};
	ipc_msg_kern_t msg = {
		.iov = &iov,
		.num_iov = 1,
		.handles = NULL,
		.num_handles = 0,
	};

	if (key_len == 0 || in_buf == NULL || out_buf == NULL) {
		LTRACEF("dirty state is detected!\n");
		rc = ERR_NOT_READY;
		goto exit;
	}

	if (buf_len == 0 || buf_len > key_len || in_offset + buf_len > key_len) {
		LTRACEF("buffer length is not correct (%u) for cmd (%u)\n", buf_len, hdr->cmd);
		rc = ERR_INVALID_ARGS;
		goto exit;
	}

	memcpy(in_buf + in_offset, hdr->payload, buf_len);
	in_offset += buf_len;
	if (in_offset == key_len)
		hdr->cmd |= DONE_FLAG;

	hdr->cmd |= RESP_FLAG;
exit:
	ipc_send_msg(ctx->chan, &msg);

	return rc;
}

int dec_key_buf(struct kmgetboot_chan_ctx *ctx, ipc_msg_info_t *mi, struct kmgetboot_msg *hdr)
{
	int rc = NO_ERROR;
	iovec_kern_t iov[2] = {
		{
			.base = hdr,
			.len = sizeof(struct kmgetboot_msg),
		},
		{
			.base = &in_offset,
			.len = sizeof(uint32_t),
		}
	};
	ipc_msg_kern_t msg = {
		.iov = iov,
		.num_iov = 1,
		.handles = NULL,
		.num_handles = 0,
	};

	if (key_len == 0 || in_buf == NULL || out_buf == NULL) {
		LTRACEF("dirty state is detected!\n");
		rc = ERR_NOT_READY;
		goto exit;
	}

	if (in_offset != key_len) {
		LTRACEF("buffer is not complete!\n");
		rc = ERR_INVALID_ARGS;
		goto exit;
	}

	//rc = process_encrypt_key_block(in_buf, in_offset, out_buf, &in_offset);
	memcpy(out_buf, in_buf, in_offset);
	if (rc) {
		LTRACEF("process_encrypt_key_block failed (%d)\n", rc);
		rc = ERR_NOT_VALID;
		goto exit;
	}

	if (in_offset == 0) {
		LTRACEF("process_encrypt_key_block outputs buffer of zero byte!\n");
		rc = ERR_NO_MSG;
		goto exit;
	}

	hdr->cmd |= (RESP_FLAG | DONE_FLAG);
	msg.num_iov = 2;
exit:
	ipc_send_msg(ctx->chan, &msg);
	return rc;
}

int read_key_buf(struct kmgetboot_chan_ctx *ctx, ipc_msg_info_t *mi, struct kmgetboot_msg *hdr)
{
	int rc = NO_ERROR;
	uint32_t out_len = in_offset - out_offset < MAX_MSG_SIZE - IPC_MSG_SIZE - sizeof(struct kmgetboot_msg) ? in_offset - out_offset : MAX_MSG_SIZE - IPC_MSG_SIZE - sizeof(struct kmgetboot_msg);
	iovec_kern_t iov[2] = {
		{
			.base = hdr,
			.len = sizeof(struct kmgetboot_msg),
		},
		{
			.base = out_buf + out_offset,
			.len = out_len,
		}
	};
	ipc_msg_kern_t msg = {
		.iov = iov,
		.num_iov = 1,
		.handles = NULL,
		.num_handles = 0,
	};

	if (key_len == 0 || in_buf == NULL || out_buf == NULL) {
		LTRACEF("dirty state is detected!\n");
		rc = ERR_NOT_READY;
		goto exit;
	}

	if (out_len == 0) {
		LTRACEF("output length is not correct (%u) for cmd (%u)\n", out_len, hdr->cmd);
		rc = ERR_NO_MSG;
		goto exit;
	}

	out_offset += out_len;
	if (out_offset == in_offset)
		hdr->cmd |= DONE_FLAG;

	hdr->cmd |= RESP_FLAG;
	msg.num_iov = 2;
exit:
	ipc_send_msg(ctx->chan, &msg);

	if (out_offset == in_offset) {
		memset(in_buf, 0, key_len);
		memset(out_buf, 0, key_len);
		key_len = 0;
		free(in_buf);
		free(out_buf);
		in_buf = NULL;
		out_buf = NULL;
	}

	return rc;
}
#endif

static int kmgetboot_chan_handle_msg(struct kmgetboot_chan_ctx *ctx)
{
	int rc;
	ipc_msg_info_t mi;

	DEBUG_ASSERT(ctx);

	/* get message */
	rc = ipc_get_msg(ctx->chan, &mi);
	if (rc) {
		LTRACEF("ipc_get_msg failed (%d) for chan %p\n", rc, ctx->chan);
		return rc;
	}

	/* check message size */
	if (mi.len < sizeof(struct kmgetboot_msg)) {
		LTRACEF("unexpected msg size (%d) for chan %p\n", mi.len, ctx->chan);
		ipc_put_msg(ctx->chan, mi.id);
		return ERR_BAD_LEN;
	}

	struct kmgetboot_msg *hdr = malloc(mi.len);

	/* read incomming request */
	iovec_kern_t iov = {
		.base = hdr,
		.len = mi.len,
	};
	ipc_msg_kern_t msg = {
		.iov = &iov,
		.num_iov = 1,
		.handles = NULL,
		.num_handles = 0,
	};

	rc = ipc_read_msg(ctx->chan, mi.id, 0, &msg);
	if (rc < 0) {
		LTRACEF("failed (%d) to read the msg (%d) for chan %p\n", rc, mi.len, ctx->chan);
		goto exit;
	}
	if ((size_t)rc != mi.len) {
		LTRACEF("partial message read (%d vs. %d) for chan %p\n", rc, mi.len, ctx->chan);
		rc = ERR_TOO_BIG;
		goto exit;
	}

	/* retire an original message */
	ipc_put_msg(ctx->chan, mi.id);

	/* handle it */
	switch (hdr->cmd) {
	case GET_VERIFIED_BOOT:
		rc = get_verified_boot(ctx, &mi, hdr);
		break;
#if 0
	case SET_KEY_LEN:
		rc = set_key_len(ctx, &mi, hdr);
		break;
	case SEND_KEY_BUF:
		rc = send_key_buf(ctx, &mi, hdr);
		break;
	case DEC_KEY_BUF:
		rc = dec_key_buf(ctx, &mi, hdr);
		break;
	case READ_KEY_BUF:
		rc = read_key_buf(ctx, &mi, hdr);
		break;
#endif
	default:
		LTRACEF("Unsupported request\n");
		rc = ERR_NOT_SUPPORTED;
		break;
	}
exit:
	free(hdr);
	return rc;
}

static void kmgetboot_close_chan(struct kmgetboot_chan_ctx *ctx)
{
	handle_list_del(&kmgetboot_handles, ctx->chan);
	handle_close(ctx->chan);

	free(ctx);
}

static void kmgetboot_chan_handler(handle_t *chan, uint32_t event, void *priv)
{
	struct kmgetboot_chan_ctx *ctx = priv;

	DEBUG_ASSERT(ctx);
	DEBUG_ASSERT(chan == ctx->chan);

	if (event & IPC_HANDLE_POLL_HUP) {
		kmgetboot_close_chan(ctx);
	} else {
		if (event & IPC_HANDLE_POLL_MSG) {
			int rc = kmgetboot_chan_handle_msg(ctx);
			if (rc) {
				kmgetboot_close_chan(ctx);
			}
		}
	}
}

static void kmgetboot_port_handler(handle_t *port, uint32_t event, void *priv)
{
	const uuid_t *peer_uuid;

	if (event & IPC_HANDLE_POLL_READY) {
		handle_t *chan;

		/* incoming connection: accept it */
		int rc = ipc_port_accept(port, &chan, &peer_uuid);
		if (rc < 0) {
			LTRACEF("failed (%d) to accept on port %p\n", rc, port);
			return;
		}

		/* allocate state */
		struct kmgetboot_chan_ctx *ctx = calloc(1, sizeof(*ctx));
		if (!ctx) {
			LTRACEF("failed to alloc state for chan %p\n", chan);
			handle_close(chan);
			return;
		}

		/* init channel state */
		ctx->evt_handler.priv = ctx;
		ctx->evt_handler.proc = kmgetboot_chan_handler;
		ctx->chan = chan;
		ctx->uuid = *peer_uuid;

		/* attach channel handler */
		handle_set_cookie(chan, &ctx->evt_handler);

		/* add to handle tracking list */
		handle_list_add(&kmgetboot_handles, chan);
	}
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

	LTRACEF("no handler for event (0x%x) with handle %p\n", event, handle);

	handle_list_del(&kmgetboot_handles, handle);
	handle_close(handle);
}

static ipc_event_handler_t kmgetboot_port_evt_handler = {
	.proc = kmgetboot_port_handler,
};

static status_t init_kmgetboot_ipc(void)
{
	int rc;

	rc = ipc_port_create(&zero_uuid, KMGETBOOT_PORT, 1, MAX_MSG_SIZE, IPC_PORT_ALLOW_TA_CONNECT, &kmgetboot_port);
	if (rc != NO_ERROR) {
		TRACEF("failed (%d) to create port '%s'\n", rc, KMGETBOOT_PORT);
		return rc;
	}
	handle_set_cookie(kmgetboot_port, &kmgetboot_port_evt_handler);

	rc = ipc_port_publish(kmgetboot_port);
	if (rc != NO_ERROR) {
		TRACEF("failed (%d) to publish port '%s'\n", rc, KMGETBOOT_PORT);
		return rc;
	}
	handle_list_add(&kmgetboot_handles, kmgetboot_port);

	TRACEF("%s created\n", KMGETBOOT_PORT);
	return NO_ERROR;
}

static int kmgetboot_ipc_thread(void)
{
	int ret;
	handle_t *handle;
	uint32_t  event;

	LTRACEF("enter\n");

	for (;;) {
		/* wait for incoming messgages */
		ret = handle_list_wait(&kmgetboot_handles, &handle, &event, INFINITE_TIME);
		if (ret == ERR_NOT_FOUND) {
			/* no handles left */
			LTRACEF("No handles left\n");
			break;
		}
		if (ret < 0) {
			/* only possible if somebody else is waiting
			   on the same handle which should never happen */
			panic("%s: couldn't wait for handle events (%d)\n", __func__, ret);
		}
		dispatch_event(handle, event);
		handle_decref(handle);
	}

	LTRACEF("exiting\n");
	return 0;
}

static void init_kmgetboot_service(uint level)
{
	init_kmgetboot_ipc();

	kmgetboot_thread = thread_create("kmgetboot_service", (thread_start_routine)kmgetboot_ipc_thread,
					NULL, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
	if (!kmgetboot_thread) {
		TRACEF("WARNING: failed for start kmgetboot thread\n");
		return;
	}

	thread_detach_and_resume(kmgetboot_thread);
}

LK_INIT_HOOK(kmgetboot_service, init_kmgetboot_service, LK_INIT_LEVEL_APPS);
