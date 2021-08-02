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

#ifndef __ATTEST_KEYBOX_H__
#define __ATTEST_KEYBOX_H__

#include <stdint.h>


#define SZ_AES_KEY 16
#define SZ_AES_IV 16

#define SZ_ENCKB_PUB_KEY 256


#define OFFSET_ENCKB_HEADER 0
#define SZ_ENCKB_HEADER 4
#define OFFSET_ENCKB_HEADER_RESERVED (OFFSET_ENCKB_HEADER+SZ_ENCKB_HEADER)
#define SZ_ENCKB_HEADER_RESERVED 64
#define OFFSET_ENCKB_HEADER_SIZE (OFFSET_ENCKB_HEADER_RESERVED+SZ_ENCKB_HEADER_RESERVED)
#define SZ_ENCKB_HEADER_SIZE 4
#define OFFSET_ENCKB_HEADER_KEYCOUNT (OFFSET_ENCKB_HEADER_SIZE+SZ_ENCKB_HEADER_SIZE)
#define SZ_ENCKB_HEADER_KEYCOUNT 4
#define OFFSET_ENCKB_HEADER_HASEKKB (OFFSET_ENCKB_HEADER_KEYCOUNT+SZ_ENCKB_HEADER_KEYCOUNT)
#define SZ_ENCKB_HEADER_HASEKKB 4
#define OFFSET_ENCKB_HEADER_EKKB (OFFSET_ENCKB_HEADER_HASEKKB+SZ_ENCKB_HEADER_HASEKKB)
#define SZ_ENCKB_HEADER_EKKB 256
#define SZ_ENCKB_TOTAL_HEADER (OFFSET_ENCKB_HEADER_EKKB+SZ_ENCKB_HEADER_EKKB)
#define SZ_ENCKB_SIG 256

typedef struct {
	uint8_t magicNumber[4]; // now will use "EncS"
	uint8_t reserved[64];
	uint32_t totalSize;
	uint32_t keyCount;
	uint32_t isKkbInKb;
	uint8_t encryptKkb[256];
} ENCRYPT_KEY_BLOCK_HEADER_T;

typedef struct {
	unsigned char encryptKeyBlockSignature[256];
} ENCRYPT_KEY_BLOCK_TAIL_T;


#define SZ_DRMKEY_ID 4
#define SZ_DRMKEY_TYPE 4
#define SZ_DRMKEY_SIZE 4
#define SZ_DRMKEY_ENC_SIZE 4
#define SZ_DRMKEY_CLEAR_IV 16
#define SZ_DRMKEY_RESERVED 48
#define SZ_DRMKEY_HEADER_SIZE (SZ_DRMKEY_ID+SZ_DRMKEY_TYPE+SZ_DRMKEY_SIZE+SZ_DRMKEY_ENC_SIZE+SZ_DRMKEY_CLEAR_IV+SZ_DRMKEY_RESERVED)
#define SZ_DRMKEY_SIG 256

enum DRMKeyID
{
	WIDEVINE_ID = 0,
	MARLIN_ID,
	HDCP_1X_TX_ID,
	HDCP_2X_V1_TX_ID,
	HDCP_2X_V1_RX_ID,
	HDCP_2X_V2_TX_ID,
	HDCP_2X_V2_RX_ID,
	PLAYREADY_BGROUPCERT_ID,
	PLAYREADY_ZGPRIV_ID,
	PLAYREADY_KEYFILE_ID,
	KEYMASTER_ATTESTATION_KEY,
	DEVICE_RSA_KEYPAIR,
	LEK_ID,
	GOOGLE_VOUCHER_ID,
	DAP_ID,
	DRM_KEY_MAX,
	DRM_SP_EKKB = 0xFFFF,
	DRMKeyID_MAX = 0xFFFFFFFF // ensure 4 byte size
};

enum DRMKeyType
{
	KB_TYPE_MIX = 0,
	KB_TYPE_PM,
	KB_TYPE_PD,
	KB_TYPE_MAX,
	KB_TYPE_SPECIAL_DEV_GEN = 0x100,
	KB_TYPE_SPECIAL_OTA = 0x200,
	KB_TYPE_SPECIAL_PLAIN_CONTENT=0x300,
	KB_TYPE_SPECIAL_MAX,
	DRMKeyType_MAX = 0xFFFFFFFF // ensure 4 byte size
};

typedef struct {
	uint32_t drmKeyID;
	uint32_t drmKeyType;
	uint32_t drmKeySize;
	uint32_t encDrmKeySize;
	uint8_t clearIV[16];
	uint8_t reserved[48];
	uint8_t *drmKey;
	uint8_t *drmKeySignature; //16 bytes
} ENCRYPT_DRM_KEY_T;

typedef struct {
	uint32_t drmKeyID;
	uint32_t drmKeyType;
	uint32_t drmKeySize;
	uint8_t clearIV[16];
	uint8_t reserved[48];
	uint8_t *drmKey;
} CLEAR_DRM_KEY_T;


typedef struct attest_keybox {
	uint32_t keybox_version;
	struct {
		// ECDSA
		uint32_t ecAttestKeyLen;
		uint8_t *ecAttestKey;
		uint32_t ecAttestCertChainLen;  // L: number of EC cert chain, including root cert
		uint32_t ecAttestCert0Len;
		uint8_t *ecAttestCert0;
		uint32_t ecAttestCert1Len;
		uint8_t *ecAttestCert1;
		uint32_t ecAttestRootCertLen;
		uint8_t *ecAttestRootCert;
	} ec;
	struct {
		// RSA
		uint32_t rsaAttestKeyLen;
		uint8_t *rsaAttestKey;
		uint32_t rsaAttestCertChainLen;  // L: number of RSA cert chain, including root cert
		uint32_t rsaAttestCert0Len;
		uint8_t *rsaAttestCert0;
		uint32_t rsaAttestCert1Len;
		uint8_t *rsaAttestCert1;
		uint32_t rsaAttestRootCertLen;
		uint8_t *rsaAttestRootCert;
	} rsa;
} attest_keybox_t;

#if 0
typedef struct attest_keybox {
	uint32_t keybox_version;
	struct ec {
		// ECDSA
		uint32_t ecAttestKeyLen;
		uint8_t *ecAttestKey;
		uint32_t ecAttestCertChainLen;  // L: number of EC cert chain, including root cert
		uint32_t ecAttestCert1Len;
		uint8_t *ecAttestCert1;
		uint32_t ecAttestCert2Len;
		uint8_t *ecAttestCert2;
		...
		uint32_t ecAttestCert(L-1)Len;
		uint8_t *ecAttestCert(L-1);
		uint32_t ecAttestRootCertLen;
		uint8_t *ecAttestRootCert;
	};
	struct rsa {
		// RSA
		uint32_t rsaAttestKeyLen;
		uint8_t *rsaAttestKey;
		uint32_t rsaAttestCertChainLen;  // L: number of RSA cert chain, including root cert
		uint32_t rsaAttestCert1Len;
		uint8_t *rsaAttestCert1;
		uint32_t rsaAttestCert2Len;
		uint8_t *rsaAttestCert2;
		...
		uint32_t rsaAttestCert(L-1)Len;
		uint8_t *rsaAttestCert(L-1);
		uint32_t rsaAttestRootCertLen;
		uint8_t *rsaAttestRootCert;
	};
} attest_keybox_t;
#endif

#endif
