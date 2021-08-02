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
#include <string.h>
#include <trace.h>

#include <interface/hwkey/hwkey.h>
#include <platform/teeargs.h>

#include "mtcrypto_priv.h"

#define LOCAL_TRACE  0

#define HWKEY_SRV_NAME          HWKEY_PORT

#define HWKEY_MAX_PAYLOAD_SIZE  2048

#define HWKEY_DEV_KEY_SIZE      32

#define MAX_STR_SIZE 128

struct hwkey_chan_ctx {
	ipc_event_handler_t evt_handler;
	handle_t *chan;
	uuid_t uuid;
};

struct hwkey_keyslot {
	const char *key_id;
	const uuid_t *uuid;
	uint32_t (*handler)(uint8_t **key, size_t *key_len);
};

/****************************************************************************/

static void hwkey_port_handler(handle_t *handle, uint32_t event, void *priv);
static void hwkey_chan_handler(handle_t *handle, uint32_t event, void *priv);

static handle_t *hwkey_port;

static ipc_event_handler_t hwkey_port_evt_handler = {
	.proc = hwkey_port_handler,
};

static uint8_t req_data[HWKEY_MAX_PAYLOAD_SIZE+1];
static uint8_t key_data[HWKEY_MAX_PAYLOAD_SIZE];

/****************************************************************************/

__WEAK uint32_t plat_derive_masterkey_v1(uint8_t *salt, size_t salt_len,
			      uint8_t *mk, size_t mk_len)
{
	return HWKEY_ERR_NOT_IMPLEMENTED;
}

__WEAK uint32_t plat_derive_masterkey_v1_hacc(uint8_t *salt, 
	size_t salt_len, uint8_t *mk, size_t mk_len)
{
	return HWKEY_ERR_NOT_IMPLEMENTED;
}

__WEAK uint32_t plat_hkdf_sha256(uint8_t *salt, uint32_t salt_len,
			      uint8_t *ikm_data, uint32_t ikm_len,
			      uint8_t *info_data, uint32_t info_len,
			      uint8_t *key_data, uint32_t key_len)
{
	return HWKEY_ERR_NOT_IMPLEMENTED;
}


/*
 * Derive key V1
 */
static uint32_t derive_key_v1(const uuid_t *uuid,
			      uint8_t *ikm_data, size_t ikm_len,
			      uint8_t *key_data, size_t *key_len)
{
	uint8_t master_key[HWKEY_DEV_KEY_SIZE];
	uint32_t ret = HWKEY_ERR_NOT_IMPLEMENTED;
	uint8_t temp_salt[HWKEY_DEV_KEY_SIZE];//TODO: To be modified by Tablet
	size_t temp_salt_len = HWKEY_DEV_KEY_SIZE;//TODO: To be modified by Tablet

	ASSERT(ikm_data);
	ASSERT(key_data);
	ASSERT(key_len);

	if (!ikm_len)
		return HWKEY_ERR_BAD_LEN;

#if defined(CRYPTO_MODULE_HACC)
	ret = plat_derive_masterkey_v1_hacc(temp_salt, temp_salt_len, master_key, HWKEY_DEV_KEY_SIZE);
#else /* use GCPU */
	ret = plat_derive_masterkey_v1(temp_salt, temp_salt_len, master_key, HWKEY_DEV_KEY_SIZE);
#endif

	if(ret == HWKEY_NO_ERROR)
	{
#if 0
		TRACEF("==== uuid dump uuid_len=%u====\n", sizeof(uuid));
		for (size_t i = 0; i < sizeof(uuid); i+=4)
			TRACEF("0x%x 0x%x 0x%x 0x%x\n", uuid[i], uuid[i+1], uuid[i+2], uuid[i+3]);

		TRACEF("==== ikm dump ikm_len=%u ====\n", ikm_len);
		for (size_t i = 0; i < ikm_len; i+=4)
			TRACEF("0x%x 0x%x 0x%x 0x%x\n", ikm_data[i], ikm_data[i+1], ikm_data[i+2], ikm_data[i+3]);

		TRACEF("==== master_key dump key_len=%u ====\n", *key_len);
		for (size_t i = 0; i < HWKEY_DEV_KEY_SIZE; i+=4)
			TRACEF("0x%x 0x%x 0x%x 0x%x\n", master_key[i], master_key[i+1], master_key[i+2], master_key[i+3]);
#endif

		ret = plat_hkdf_sha256(master_key, *key_len,
				(uint8_t *)uuid, sizeof(uuid),
				ikm_data, ikm_len,
				key_data, ikm_len);

		*key_len  = ikm_len;

#if 0
		TRACEF("==== key_data dump key_len=%u ====\n", *key_len);
		for (size_t i = 0; i < ikm_len; i+=4)
			TRACEF("0x%x 0x%x 0x%x 0x%x\n", key_data[i], key_data[i+1], key_data[i+2], key_data[i+3]);
#endif

	}

	return ret;
}

/****************************************************************************/

#define RPMB_SS_AUTH_KEY_SIZE    32
#define RPMB_SS_AUTH_KEY_ID      "com.android.trusty.storage_auth.rpmb"

/* Secure storage service app uuid */
static const uuid_t ss_uuid =
	{ 0xcea8706d, 0x6cb4, 0x49f3, \
		{ 0xb9, 0x94, 0x29, 0xe0, 0xe4, 0x78, 0xbd, 0x29 }};

__WEAK uint32_t plat_get_rpmb_ss_auth_key(uint8_t *salt,
	size_t salt_len, uint8_t **key, size_t *key_len)
{
	return HWKEY_ERR_NOT_IMPLEMENTED;
}

extern uint8_t tee_rpmb_key[RPMB_KEY_SIZE];

/*
 * Generate RPMB Secure Storage Authentication key
 */
static uint32_t get_rpmb_ss_auth_key(uint8_t **key, size_t *key_len)
{

#if WITH_DXCC_RPMB_KEY
	LTRACEF("%s: WITH_DXCC_RPMB_KEY\n", __func__);
	*key = tee_rpmb_key;
	*key_len = RPMB_SS_AUTH_KEY_SIZE;
	return HWKEY_NO_ERROR;
#else
	uint8_t temp_salt[HWKEY_DEV_KEY_SIZE];//TODO: To be modified by Tablet
	size_t temp_salt_len = HWKEY_DEV_KEY_SIZE;//TODO: To be modified by Tablet
	return plat_get_rpmb_ss_auth_key(temp_salt, temp_salt_len, key, key_len);
#endif
}


#if WITH_HWCRYPTO_UNITTEST
/*
 *  Support for hwcrypto unittest keys should be only enabled
 *  to test hwcrypto related APIs
 */

/* UUID of HWCRYPTO_UNITTEST application */
static const uuid_t hwcrypto_unittest_uuid = \
	{ 0xab742471, 0xd6e6, 0x4806, \
		{ 0x85, 0xf6, 0x05, 0x55, 0xb0, 0x24, 0xf4, 0xda }};

static uint8_t _unittest_key32[32] = "unittestkeyslotunittestkeyslotun";
static uint32_t get_unittest_key32(uint8_t **key, size_t *key_len)
{
	ASSERT(key);
	ASSERT(key_len);

	/* just return predefined key */
	*key = _unittest_key32;
	*key_len = 32;

	return HWKEY_NO_ERROR;
}
#endif /* WITH_HWCRYPTO_UNITTEST */

/*
 *  List of keys slots that hwkey service supports
 */
static const struct hwkey_keyslot _slots[] = {
	{
		.uuid = &ss_uuid,
		.key_id = RPMB_SS_AUTH_KEY_ID,
		.handler = get_rpmb_ss_auth_key,
	},
#if WITH_HWCRYPTO_UNITTEST
	{
		.uuid = &hwcrypto_unittest_uuid,
		.key_id = "com.android.trusty.hwcrypto.unittest.key32",
		.handler = get_unittest_key32,
	},
#endif
};

/****************************************************************************/

/*
 * Close specified HWRNG service channel
 */
static void hwkey_close_chan(struct hwkey_chan_ctx *ctx)
{
	handle_list_del(&mtcrypto_handles, ctx->chan);
	handle_close(ctx->chan);

	free(ctx);
}

/*
 * Send response message
 */
static int hwkey_send_rsp(struct hwkey_chan_ctx *ctx, struct hwkey_msg *rsp,
			  uint8_t *rsp_data, size_t rsp_data_len)
{
	ASSERT(ctx);
	ASSERT(rsp);

	iovec_kern_t iovs[2] = {
		{
			.base = rsp,
			.len = sizeof(*rsp),
		},
		{
			.base = rsp_data,
			.len  = rsp_data_len,
		},
	};
	ipc_msg_kern_t msg = {
		.iov = iovs,
		.num_iov = countof(iovs),
	};
	return ipc_send_msg(ctx->chan, &msg);
}

/*
 * Handle get key slot command
 */
static int hwkey_handle_get_keyslot_cmd(struct hwkey_chan_ctx *ctx,
					struct hwkey_msg *hdr,
					const char *slot_id)
{
	size_t key_len = 0;
	uint8_t *key_data = NULL;
	uint32_t status = HWKEY_ERR_NOT_FOUND;

	/* for all key slots */
	for (uint i = 0; i < countof(_slots); i++) {
		/* check key id */
		if (strncmp(_slots[i].key_id, slot_id, MAX_STR_SIZE))
			continue;

		/* Check if the caller is allowed to get that key */
		if (memcmp(&ctx->uuid, _slots[i].uuid, sizeof(uuid_t)) == 0) {
			DEBUG_ASSERT(_slots[i].handler);
			status = _slots[i].handler(&key_data, &key_len);
			break;
		}
	}

	hdr->cmd = HWKEY_GET_KEYSLOT | HWKEY_RESP_BIT;
	hdr->status = status;
	return hwkey_send_rsp(ctx, hdr, key_data, key_len);
}

/*
 * Handle Derive key cmd
 */
static int hwkey_handle_derive_key_cmd(struct hwkey_chan_ctx *ctx,
				       struct hwkey_msg *hdr,
				       uint8_t *ikm_data, size_t ikm_len,
				       uint8_t *key_data)
{
	size_t key_len = 0;

	/* check requested key derivation function */
	if (hdr->arg1 == HWKEY_KDF_VERSION_BEST)
		hdr->arg1 = HWKEY_KDF_VERSION_1; /* we only support V1 */

	switch (hdr->arg1) {
		case HWKEY_KDF_VERSION_1:
			hdr->status = derive_key_v1(&ctx->uuid,
						    ikm_data, ikm_len,
						    key_data, &key_len);
		break;

		default:
			LTRACEF("%u is unsupported KDF function\n", hdr->arg1);
			hdr->status = HWKEY_ERR_NOT_IMPLEMENTED;
	}

	hdr->cmd = HWKEY_DERIVE | HWKEY_RESP_BIT;
	return hwkey_send_rsp(ctx, hdr, key_data, key_len);
}

/*
 *  Read and queue HWKEY request message
 */
static int hwkey_chan_handle_msg(struct hwkey_chan_ctx *ctx)
{
	int rc;
	ipc_msg_info_t mi;
	struct hwkey_msg hdr;
	size_t req_data_len;

	DEBUG_ASSERT(ctx);

	/* get message */
	rc = ipc_get_msg(ctx->chan, &mi);
	if (rc) {
		LTRACEF("ipc_get_msg failed (%d) for chan %p\n",
			rc, ctx->chan);
		return rc;
	}

	/* check message size */
	if (mi.len < sizeof(hdr)) {
		LTRACEF("unexpected msg size (%d) for chan %p\n",
			mi.len, ctx->chan);
		return ERR_BAD_LEN;
	}

	/* read incomming request */
	iovec_kern_t iovs[2] = {
		{
			.base = &hdr,
			.len = sizeof(hdr),
		},
		{
			.base = req_data,
			.len = sizeof(req_data) - 1,
		}
	};
	ipc_msg_kern_t msg = {
		.iov = iovs,
		.num_iov = countof(iovs),
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

	/* calculate payload length */
	req_data_len = mi.len - sizeof(hdr);

	/* handle it */
	switch (hdr.cmd) {
	case HWKEY_GET_KEYSLOT:
		req_data[req_data_len] = 0; /* force zero termination */
		rc = hwkey_handle_get_keyslot_cmd(ctx, &hdr, (const char *)req_data);
		break;

	case HWKEY_DERIVE:
		rc = hwkey_handle_derive_key_cmd(ctx, &hdr, req_data, req_data_len,
						 key_data);
		break;

	default:
		LTRACEF("Unsupported request\n");
		rc = ERR_NOT_SUPPORTED;
		break;
	}

	/* wipe request buffers clean */
	memset(req_data, 0, req_data_len);
	memset(key_data, 0, req_data_len);
	return rc;
}

/*
 *  HWKEY service channel event handler
 */
static void hwkey_chan_handler(handle_t *chan, uint32_t event, void *priv)
{
	struct hwkey_chan_ctx *ctx = priv;

	DEBUG_ASSERT(ctx);
	DEBUG_ASSERT(chan == ctx->chan);

	if (event & IPC_HANDLE_POLL_HUP) {
		hwkey_close_chan(ctx);
	} else {
		if (event & IPC_HANDLE_POLL_MSG) {
			int rc = hwkey_chan_handle_msg(ctx);
			if (rc < 0) {
				hwkey_close_chan(ctx);
			}
		}
	}
}

/*
 * HWKEY service port event handler
 */
static void hwkey_port_handler(handle_t *port, uint32_t event, void *priv)
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
		struct hwkey_chan_ctx *ctx = calloc(1, sizeof(*ctx));
		if (!ctx) {
			LTRACEF("failed to alloc state for chan %p\n", chan);
			handle_close(chan);
			return;
		}

		/* init channel state */
		ctx->evt_handler.priv = ctx;
		ctx->evt_handler.proc = hwkey_chan_handler;
		ctx->chan = chan;
		ctx->uuid = *peer_uuid;

		/* attach channel handler */
		handle_set_cookie(chan, &ctx->evt_handler);

		/* add to handle tracking list */
		handle_list_add(&mtcrypto_handles, chan);
	}
}

/*
 *  Initialize HWKEY service
 */
status_t mtcrypto_init_hwkey(void)
{
	int rc;

	/* Create HWKEY port */
	rc = ipc_port_create(&zero_uuid, HWKEY_SRV_NAME,
			     1,  sizeof(struct hwkey_msg) + HWKEY_MAX_PAYLOAD_SIZE,
			     IPC_PORT_ALLOW_TA_CONNECT,
			     &hwkey_port);
	if (rc != NO_ERROR) {
		LTRACEF("failed (%d) to create port '%s'\n",
			rc, HWKEY_SRV_NAME);
		return rc;
	}
	handle_set_cookie(hwkey_port, &hwkey_port_evt_handler);

	rc = ipc_port_publish(hwkey_port);
	if (rc != NO_ERROR) {
		LTRACEF("failed (%d) to publish port '%s'\n",
			rc, HWKEY_SRV_NAME);
		return rc;
	}
	handle_list_add(&mtcrypto_handles, hwkey_port);

	return NO_ERROR;
}
