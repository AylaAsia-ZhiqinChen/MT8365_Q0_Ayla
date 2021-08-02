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
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kmsetkey.h>
#include <cryptolib.h>
#include <attest_keybox.h>
#include <hardware/keymaster_defs.h>
#include <interface/keymaster/keymaster.h>

#include <uapi/err.h>
#include <trusty_ipc.h>


#define LOG_TAG "KM_SETKEY"
#define TLOGE(fmt, ...) \
	fprintf(stderr, "%s: %d: " fmt, LOG_TAG, __LINE__,  ## __VA_ARGS__)

static uint32_t key_len = 0, key_offset = 0;
static uint8_t *key_buf = NULL, *out_key_buf = NULL;

static long set_key_len(handle_t chan, struct kmsetkey_msg *req, size_t req_len)
{
	long rc = NO_ERROR;

	struct kmsetkey_msg rsp = {
		.cmd = req->cmd,
	};

	iovec_t iov = {
		.base = &rsp,
		.len = sizeof(rsp),
	};
	ipc_msg_t msg = {
		.iov = &iov,
		.num_iov = 1,
		.handles = NULL,
		.num_handles = 0,
	};

	if (key_len != 0 || key_buf != NULL || out_key_buf != NULL) {
		TLOGE("%s: dirty state is detected!\n", __FUNCTION__);
		rc = -1;
		goto exit;
	}

	if (req_len != sizeof(struct kmsetkey_msg) + sizeof(uint32_t)) {
		TLOGE("%s: msg size is not correct (%u) for cmd (%u)\n", __FUNCTION__, req_len, req->cmd);
		rc = -2;
		goto exit;
	}

	memcpy(&key_len, req->payload, sizeof(uint32_t));
	if (key_len == 0) {
		TLOGE("%s: key length is zero!\n", __FUNCTION__);
		rc = -3;
		goto exit;
	}

	key_buf = malloc(key_len);
	if (key_buf == NULL) {
		TLOGE("%s: malloc input buffer failed!\n", __FUNCTION__);
		key_len = 0;
		rc = -4;
		goto exit;
	}

	out_key_buf = malloc(key_len);
	if (out_key_buf == NULL) {
		TLOGE("%s: malloc output buffer failed!\n", __FUNCTION__);
		free(key_buf);
		key_buf = NULL;
		key_len = 0;
		rc = -5;
		goto exit;
	}

	key_offset = 0;

	rsp.cmd |= (RESP_FLAG | DONE_FLAG);
exit:
	if (send_msg(chan, &msg) < 0)
		TLOGE("%s: failed to send_msg\n", __FUNCTION__);

	return rc < 0 ? rc : NO_ERROR;
}

static long send_key_buf(handle_t chan, struct kmsetkey_msg *req, size_t req_len)
{
	long rc = NO_ERROR;

	size_t payload_len = req_len - sizeof(struct kmsetkey_msg);
	struct kmsetkey_msg rsp = {
		.cmd = req->cmd,
	};

	iovec_t iov = {
		.base = &rsp,
		.len = sizeof(rsp),
	};
	ipc_msg_t msg = {
		.iov = &iov,
		.num_iov = 1,
		.handles = NULL,
		.num_handles = 0,
	};

	if (key_len == 0 || key_buf == NULL || out_key_buf == NULL) {
		TLOGE("%s: dirty state is detected!\n", __FUNCTION__);
		rc = -1;
		goto exit;
	}

	if (payload_len == 0 || payload_len > key_len || key_offset + payload_len > key_len) {
		TLOGE("%s: buffer length is not correct (%u) for cmd (%u)\n", __FUNCTION__, payload_len, req->cmd);
		rc = -2;
		goto exit;
	}

	memcpy(key_buf + key_offset, req->payload, payload_len);
	key_offset += payload_len;
	if (key_offset == key_len)
		rsp.cmd |= DONE_FLAG;

	rsp.cmd |= RESP_FLAG;
exit:
	if (send_msg(chan, &msg) < 0)
		TLOGE("%s: failed to send_msg\n", __FUNCTION__);

	return rc < 0 ? rc : NO_ERROR;
}

static long send_req(handle_t session, uint8_t *req, size_t req_len)
{
	iovec_t tx_iov = {
		.base = req,
		.len = req_len,
	};
	ipc_msg_t tx_msg = {
		.iov = &tx_iov,
		.num_iov = 1,
		.handles = NULL,
		.num_handles = 0,
	};

	long rc = send_msg(session, &tx_msg);
	if (rc < 0) {
		TLOGE("%s: failed (%ld) to send_msg\n", __FUNCTION__, rc);
		return rc;
	}

	if ((size_t)rc != req_len) {
		TLOGE("%s: msg invalid size (%zu != %zu)", __FUNCTION__, (size_t)rc, req_len);
		return ERR_IO;
	}

	return NO_ERROR;
}

static long await_response(handle_t session, struct ipc_msg_info *inf)
{
	uevent_t uevt;
	long rc = wait(session, &uevt, -1);
	if (rc != NO_ERROR) {
		TLOGE("%s: interrupted waiting for response (%ld)\n", __FUNCTION__, rc);
		return rc;
	}

	rc = get_msg(session, inf);
	if (rc != NO_ERROR)
		TLOGE("%s: failed to get_msg (%ld)\n", __FUNCTION__, rc);

	return rc;
}

static long read_response(handle_t session, struct ipc_msg_info *inf, uint8_t *rsp, size_t rsp_len)
{
	iovec_t rx_iov = {
		.base = rsp,
		.len = rsp_len,
	};
	struct ipc_msg rx_msg = {
		.iov = &rx_iov,
		.num_iov = 1,
		.handles = NULL,
		.num_handles = 0,
	};

	long rc = read_msg(session, inf->id, 0, &rx_msg);
	if (rc < 0) {
		TLOGE("%s: failed (%ld) to read_msg\n", __FUNCTION__, rc);
		put_msg(session, inf->id);
		return rc;
	}

	if ((size_t)rc != rsp_len) {
		TLOGE("%s: msg invalid size (%zu != %zu)", __FUNCTION__, (size_t)rc, rsp_len);
		put_msg(session, inf->id);
		return ERR_IO;
	}

	rc = put_msg(session, inf->id);
	if (rc != NO_ERROR) {
		TLOGE("%s: failed (%ld) to put_msg\n", __FUNCTION__, rc);
		return rc;
	}

	return NO_ERROR;
}

static long set_key_impl(uint32_t cmd, uint8_t *buf, uint32_t buf_len)
{
	long rc = NO_ERROR;

	handle_t session;
	keymaster_error_t error;
	uint32_t hdr_len = sizeof(struct keymaster_message) + (buf_len > sizeof(keymaster_error_t) ? buf_len : sizeof(keymaster_error_t));
	struct keymaster_message *hdr = malloc(hdr_len);
	if (hdr == NULL) {
		TLOGE("%s: Allocate header failed!\n", __FUNCTION__);
		return ERR_NO_MEMORY;
	}

	rc = connect(KEYMASTER_SECURE_PORT, IPC_CONNECT_WAIT_FOR_PORT);
	if (rc < 0) {
		TLOGE("%s: Open session failed (%ld)!\n", __FUNCTION__, rc);
		goto exit0;
	}

	session = (handle_t)rc;
	hdr->cmd = cmd;
	memcpy(hdr->payload, buf, buf_len);
	rc = send_req(session, (uint8_t *)hdr, hdr_len);
	if (rc != NO_ERROR) {
		TLOGE("%s: failed (%ld) to send_req for cmd (%u)\n", __FUNCTION__, rc, cmd);
		goto exit1;
	}

	struct ipc_msg_info msg_inf;
	rc = await_response(session, &msg_inf);
	if (rc != NO_ERROR) {
		TLOGE("%s: failed (%ld) to await_response for cmd (%u)\n", __FUNCTION__, rc, cmd);
		goto exit1;
	}

	hdr_len = msg_inf.len;
	if (hdr_len != sizeof(struct keymaster_message) + sizeof(keymaster_error_t)) {
		TLOGE("%s: invalid response size (%zu != %zu)", __FUNCTION__, hdr_len, sizeof(struct keymaster_message) + sizeof(keymaster_error_t));
		put_msg(session, msg_inf.id);
		rc = ERR_BAD_LEN;
		goto exit1;
	}

	rc = read_response(session, &msg_inf, (uint8_t *)hdr, hdr_len);
	if (rc != NO_ERROR) {
		TLOGE("%s: failed (%ld) to read_response for cmd (%u)\n", __FUNCTION__, rc, cmd);
		goto exit1;
	}
	if (hdr->cmd != (cmd | KM_STOP_BIT | KM_RESP_BIT)) {
		TLOGE("%s: invalid response (0x%x) for cmd (%u)\n", __FUNCTION__, hdr->cmd, cmd);
		rc = ERR_NOT_VALID;
		goto exit1;
	}

	memcpy(&error, hdr->payload, sizeof(keymaster_error_t));
	if (error != KM_ERROR_OK) {
		TLOGE("%s: response KM error (%d) for cmd (%u)\n", __FUNCTION__, error, cmd);
		rc = error;
	}
exit1:
	if (close(session) != NO_ERROR)
		TLOGE("%s: Close session failed!\n", __FUNCTION__);
exit0:
	free(hdr);
	return rc < 0 ? rc : NO_ERROR;
}

static long set_key(uint8_t *buf, uint32_t buf_len)
{
	long rc = NO_ERROR;
	uint32_t payload_len, ptr_len, certchain_len, i, j;
	const uint32_t km_algo[2] = {KM_ALGORITHM_EC, KM_ALGORITHM_RSA};
	uint8_t *payload, *ptr;

	payload = malloc(MAX_MSG_SIZE);
	if (payload == NULL) {
		TLOGE("%s: Allocation payload fail!\n", __FUNCTION__);
		return ERR_NO_MEMORY;
	}

	ptr = buf + sizeof(uint32_t); //bypass uint32_t keybox_version;
	for (i = 0; i < sizeof(km_algo)/sizeof(uint32_t); ++i) {
		memcpy(&ptr_len, ptr, sizeof(uint32_t));
		payload_len = sizeof(uint32_t) + ptr_len;
		memcpy(payload, &km_algo[i], sizeof(uint32_t));
		memcpy(payload + sizeof(uint32_t), ptr + sizeof(uint32_t), ptr_len);

		TLOGE("%s: KM_SET_ATTESTATION_KEY: %u\n", __FUNCTION__, payload_len);
		rc = set_key_impl(KM_SET_ATTESTATION_KEY, payload, payload_len);
		if (rc != NO_ERROR) {
			TLOGE("%s: failed (%ld) to set_key_impl for cmd (%u)\n", __FUNCTION__, rc, KM_SET_ATTESTATION_KEY);
			goto exit;
		}

		ptr += sizeof(uint32_t) + ptr_len;
		memcpy(&certchain_len, ptr, sizeof(uint32_t));
		ptr += sizeof(uint32_t);

		for (j = 0; j < certchain_len; ++j) {
			memcpy(&ptr_len, ptr, sizeof(uint32_t));
			payload_len = sizeof(uint32_t) + ptr_len;
			memcpy(payload, &km_algo[i], sizeof(uint32_t));
			memcpy(payload + sizeof(uint32_t), ptr + sizeof(uint32_t), ptr_len);
			ptr += sizeof(uint32_t) + ptr_len;

			TLOGE("%s: KM_APPEND_ATTESTATION_CERT_CHAIN: %u\n", __FUNCTION__, payload_len);
			rc = set_key_impl(KM_APPEND_ATTESTATION_CERT_CHAIN, payload, payload_len);
			if (rc != NO_ERROR) {
				TLOGE("%s: failed (%ld) to set_key_impl for cmd (%u)\n", __FUNCTION__, rc, KM_APPEND_ATTESTATION_CERT_CHAIN);
				goto exit;
			}
		}
	}

	TLOGE("%s: Attestation key injection is success!\n", __FUNCTION__);
exit:
	free(payload);
	return rc < 0 ? rc : NO_ERROR;
}

static long handle_key(handle_t chan, struct kmsetkey_msg *req, size_t req_len)
{
	long rc = NO_ERROR;;
	struct kmsetkey_msg rsp = {
		.cmd = req->cmd,
	};

	iovec_t iov = {
		.base = &rsp,
		.len = sizeof(rsp),
	};
	ipc_msg_t msg = {
		.iov = &iov,
		.num_iov = 1,
		.handles = NULL,
		.num_handles = 0,
	};

	if (key_len == 0 || key_buf == NULL || out_key_buf == NULL) {
		TLOGE("%s: dirty state is detected!\n", __FUNCTION__);
		rc = ERR_NOT_VALID;
		goto exit;
	}

	if (key_offset != key_len) {
		TLOGE("%s: buffer is not complete!\n", __FUNCTION__);
		rc = ERR_NOT_READY;
		goto exit;
	}

	rc = decrypt_keybox(key_buf, key_offset, out_key_buf, &key_offset);
	if (rc != NO_ERROR) {
		TLOGE("%s: dec key returned (%ld)\n", __FUNCTION__, rc);
		goto exit;
	}

	rc = set_key(out_key_buf, key_offset);
	if (rc != NO_ERROR) {
		TLOGE("%s: set key returned (%ld)\n", __FUNCTION__, rc);
		goto exit;
	}

	rsp.cmd |= (RESP_FLAG | DONE_FLAG);

	memset(out_key_buf, 0, key_len);
	memset(key_buf, 0, key_len);
	free(out_key_buf);
	free(key_buf);
	out_key_buf = NULL;
	key_buf = NULL;
	key_len = 0;
exit:
	send_msg(chan, &msg);
	return rc < 0 ? rc : NO_ERROR;
}

static long handle_msg(handle_t chan)
{
	/* get message info */
	ipc_msg_info_t msg_inf;

	long rc = get_msg(chan, &msg_inf);
	if (rc == ERR_NO_MSG)
		return NO_ERROR; /* no new messages */

	// fatal error
	if (rc != NO_ERROR) {
		TLOGE("%s: failed (%ld) to get_msg for chan (%d), closing connection\n", __FUNCTION__, rc, chan);
		return rc;
	}

	if (msg_inf.len < sizeof(struct kmsetkey_msg)) {
		TLOGE("%s: unexpected msg size (%u) for chan (%d)\n", __FUNCTION__, msg_inf.len, chan);
		put_msg(chan, msg_inf.id);
		return ERR_NOT_VALID;
	}

	struct kmsetkey_msg *req = malloc(msg_inf.len);

	/* read msg content */
	iovec_t iov = {
		.base = req,
		.len = msg_inf.len,
	};
	ipc_msg_t msg = {
		.iov = &iov,
		.num_iov = 1,
		.handles = NULL,
		.num_handles = 0,
	};

	rc = read_msg(chan, msg_inf.id, 0, &msg);

	if (rc < 0) {
		TLOGE("%s: failed to read msg (%ld) for chan (%d)\n", __FUNCTION__, rc, chan);
		goto exit;
	}

	if ((size_t)rc != msg_inf.len) {
		TLOGE("%s: invalid message of size (%zu) for chan (%d)\n", __FUNCTION__, (size_t)rc, chan);
		rc = ERR_NOT_VALID;
		goto exit;
	}

	switch (req->cmd) {
	case KEY_LEN:
		rc = set_key_len(chan, req, msg_inf.len);
		break;
	case KEY_BUF:
		rc = send_key_buf(chan, req, msg_inf.len);
		break;
	case SET_KEY:
		rc = handle_key(chan, req, msg_inf.len);
		break;
	default:
		rc = ERR_NOT_SUPPORTED;
		break;
	}

	if (rc != NO_ERROR)
		TLOGE("%s: unable (%ld) to handle request\n", __FUNCTION__, rc);

exit:
	free(req);
	put_msg(chan, msg_inf.id);
	return rc;
}

static void kmsetkey_handle_port(uevent_t *ev)
{
	if ((ev->event & IPC_HANDLE_POLL_ERROR) ||
		(ev->event & IPC_HANDLE_POLL_HUP) ||
		(ev->event & IPC_HANDLE_POLL_MSG) ||
		(ev->event & IPC_HANDLE_POLL_SEND_UNBLOCKED)) {
		/* should never happen with port handles */
		TLOGE("%s: error event (0x%x) for port (%d)\n", __FUNCTION__, ev->event, ev->handle);
		abort();
	}

	uuid_t peer_uuid;
	if (ev->event & IPC_HANDLE_POLL_READY) {
		/* incoming connection: accept it */
		int rc = accept(ev->handle, &peer_uuid);
		if (rc < 0) {
			TLOGE("%s: failed (%d) to accept on port %d\n", __FUNCTION__, rc, ev->handle);
			return;
		}
	}
}

static void kmsetkey_handle_channel(uevent_t *ev)
{
	if ((ev->event & IPC_HANDLE_POLL_ERROR) ||
		(ev->event & IPC_HANDLE_POLL_READY)) {
		/* close it as it is in an error state */
		TLOGE("%s: error event (0x%x) for chan (%d)\n", __FUNCTION__, ev->event, ev->handle);
		close(ev->handle);
		return;
	}

	handle_t chan = ev->handle;

	if (ev->event & IPC_HANDLE_POLL_MSG) {
		long rc = handle_msg(chan);
		if (rc != NO_ERROR) {
			/* report an error and close channel */
			TLOGE("%s: failed (%ld) to handle event on channel %d\n", __FUNCTION__, rc, ev->handle);
			close(chan);
			return;
		}
	}

	if (ev->event & IPC_HANDLE_POLL_HUP) {
		/* closed by peer. */
		close(chan);
		return;
	}

}

int main(void)
{
	int rc;
	uevent_t event;

	rc = port_create(KMSETKEY_PORT, 1, MAX_MSG_SIZE, IPC_PORT_ALLOW_NS_CONNECT);
	if (rc < 0) {
		TLOGE("%s: Failed (%d) to create port %s\n", __FUNCTION__, rc, KMSETKEY_PORT);
	}

	handle_t port = (handle_t)rc;

	/* enter main event loop */
	while (true) {
		event.handle = INVALID_IPC_HANDLE;
		event.event  = 0;
		event.cookie = NULL;

		rc = wait_any(&event, -1);
		if (rc < 0) {
			TLOGE("%s: wait_any failed (%d)\n", __FUNCTION__, rc);
			break;
		}

		if (rc == NO_ERROR) { /* got an event */
			if (event.handle == port) {
				kmsetkey_handle_port(&event);
			} else {
				kmsetkey_handle_channel(&event);
			}
		} else {
			TLOGE("%s: wait_any returned (%d)\n", __FUNCTION__, rc);
			break;
		}
	}

	return 0;
}
