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

#include <key.h>
#include <attest_keybox.h>

#include <openssl/engine.h>
#include <openssl/digest.h>
#include <openssl/cipher.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/mem.h>


#define LOG_TAG "KM_SETKEY_LIB"
#define TLOGE(fmt, ...) \
	fprintf(stderr, "%s: %d: " fmt, LOG_TAG, __LINE__,  ## __VA_ARGS__)

static const uint8_t Cpre[] = {
	0xDF, 0xD3, 0x2E, 0x5E, 0x93, 0x91, 0xB8, 0x66, 0xA1, 0xE1, 0x2A, 0x88, 0xEE, 0x8B, 0x83, 0xAA,
	0xAD, 0xC4, 0xB0, 0x7C, 0x96, 0x96, 0x91, 0x9C, 0x27, 0x61, 0xB1, 0x91, 0x7B, 0x69, 0x82, 0x7A,
};

static int32_t sha256(const uint8_t *in_buf, const uint32_t in_len, uint8_t *out_buf, uint32_t *out_len)
{
	int32_t rc;

	if (out_buf == NULL || out_len == NULL || *out_len == 0) {
		TLOGE("%s,%d: NULL pointer or zero length!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	rc = EVP_Digest(in_buf, in_len, out_buf, out_len, EVP_sha256(), NULL);
	if (rc != 1) {
		TLOGE("%s,%d: Digest failed!\n", __FUNCTION__, __LINE__);
		return -2;
	}

	if (*out_len != EVP_MD_size(EVP_sha256())) {
		TLOGE("%s,%d: Wrong digest length: %u!\n", __FUNCTION__, __LINE__, *out_len);
		return -3;
	}

	return 0;
}

static int32_t sha1(const uint8_t *in_buf, const uint32_t in_len, uint8_t *out_buf, uint32_t *out_len)
{
	int32_t rc;

	if (out_buf == NULL || out_len == NULL || *out_len == 0) {
		TLOGE("%s,%d: NULL pointer or zero length!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	rc = EVP_Digest(in_buf, in_len, out_buf, out_len, EVP_sha1(), NULL);
	if (rc != 1) {
		TLOGE("%s,%d: Digest failed!\n", __FUNCTION__, __LINE__);
		return -2;
	}

	if (*out_len != EVP_MD_size(EVP_sha1())) {
		TLOGE("%s,%d: Wrong digest length: %u!\n", __FUNCTION__, __LINE__, *out_len);
		return -3;
	}

	return 0;
}

static int32_t sha1_pkcs1_mgf1(uint8_t *out, uint32_t out_len, const uint8_t *seed, uint32_t seed_len)
{
	int32_t rc = 0;
	EVP_MD_CTX ctx;
	EVP_MD_CTX_init(&ctx);

	uint32_t md_len = EVP_MD_size(EVP_sha1());

	for (uint32_t i = 0; out_len > 0; i++) {
		uint8_t counter[4];
		counter[0] = (uint8_t)(i >> 24);
		counter[1] = (uint8_t)(i >> 16);
		counter[2] = (uint8_t)(i >> 8);
		counter[3] = (uint8_t)i;
		if (!EVP_DigestInit_ex(&ctx, EVP_sha1(), NULL) || !EVP_DigestUpdate(&ctx, seed, seed_len) || !EVP_DigestUpdate(&ctx, counter, sizeof(counter))) {
			rc = -1;
			goto err;
		}

		if (md_len <= out_len) {
			if (!EVP_DigestFinal_ex(&ctx, out, NULL)) {
				rc = -2;
				goto err;
			}
			out += md_len;
			out_len -= md_len;
		} else {
			uint8_t digest[EVP_MAX_MD_SIZE];
			if (!EVP_DigestFinal_ex(&ctx, digest, NULL)) {
				rc = -3;
				goto err;
			}
			if (out_len > 0) {
				memcpy(out, digest, out_len);
				out_len = 0;
			}
		}
	}
err:
	EVP_MD_CTX_cleanup(&ctx);
	return rc < 0 ? rc : 0;
}

static int32_t aes_cbc(const uint8_t *key_buf, const uint8_t *iv_buf, const int32_t enc, const uint8_t *in_buf, const uint32_t in_len, uint8_t *out_buf)
{
	int32_t rc;

	if (key_buf == NULL || iv_buf == NULL || in_buf == NULL || in_len == 0 || out_buf == NULL) {
		TLOGE("%s,%d: NULL pointer or zero length!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	EVP_CIPHER_CTX *cipher_ctx = EVP_CIPHER_CTX_new();
	if (cipher_ctx == NULL) {
		TLOGE("%s,%d: Allocation cipher context failed!\n", __FUNCTION__, __LINE__);
		return -2;
	}

	rc = EVP_CipherInit_ex(cipher_ctx, EVP_aes_128_cbc(), NULL, key_buf, iv_buf, enc);
	if (rc != 1) {
		TLOGE("%s,%d: Initialize cipher failed!\n", __FUNCTION__, __LINE__);
		rc = -3;
		goto exit;
	}

	rc = EVP_Cipher(cipher_ctx, out_buf, in_buf, in_len);
	if (rc != 1) {
		TLOGE("%s,%d: cupher failed!\n", __FUNCTION__, __LINE__);
		rc = -4;
		goto exit;
	}
exit:
	EVP_CIPHER_CTX_free(cipher_ctx);
	return rc < 0 ? rc : 0;
}

static int32_t aes_ecb(const uint8_t *key_buf, const int32_t enc, const uint8_t *in_buf, const uint32_t in_len, uint8_t *out_buf)
{
	int32_t rc;

	if (key_buf == NULL || in_buf == NULL || in_len == 0 || out_buf == NULL) {
		TLOGE("%s,%d: NULL pointer or zero length!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	EVP_CIPHER_CTX *cipher_ctx = EVP_CIPHER_CTX_new();
	if (cipher_ctx == NULL) {
		TLOGE("%s,%d: Allocation cipher context failed!\n", __FUNCTION__, __LINE__);
		return -2;
	}

	rc = EVP_CipherInit_ex(cipher_ctx, EVP_aes_128_ecb(), NULL, key_buf, NULL, enc);
	if (rc != 1) {
		TLOGE("%s,%d: Initialize cipher failed!\n", __FUNCTION__, __LINE__);
		rc = -3;
		goto exit;
	}

	rc = EVP_Cipher(cipher_ctx, out_buf, in_buf, in_len);
	if (rc != 1) {
		TLOGE("%s,%d: cupher failed!\n", __FUNCTION__, __LINE__);
		rc = -4;
		goto exit;
	}
exit:
	EVP_CIPHER_CTX_free(cipher_ctx);
	return rc < 0 ? rc : 0;
}

static int32_t oaep_decode(const uint8_t *in_buf, const uint32_t in_len, uint8_t *out_buf, uint32_t *out_len)
{
	int32_t rc;
	uint32_t md_len = EVP_MD_size(EVP_sha1()), db_len, seed_offset, db_offset, index;
	uint8_t *seed_buf, *db_buf;

	if (in_len < 2*md_len + 2) {
		TLOGE("%s,%d: length error!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if (in_buf[0] != 0x0) {
		TLOGE("%s,%d: format error!\n", __FUNCTION__, __LINE__);
		return -2;
	}

	seed_buf = OPENSSL_malloc(md_len);
	if (seed_buf == NULL) {
		TLOGE("%s,%d: allocation failed!\n", __FUNCTION__, __LINE__);
		return -3;
	}

	db_len = in_len - md_len - 1;
	db_buf = OPENSSL_malloc(db_len);
	if (db_buf == NULL) {
		TLOGE("%s,%d: allocation failed!\n", __FUNCTION__, __LINE__);
		OPENSSL_free(seed_buf);
		return -4;
	}

/*
	ok so it's now in the form
	0x00  || maskedseed || maskedDB 
	1     ||   md_len   || in_len - md_len - 1
*/

	seed_offset = 1;
	db_offset = md_len + 1;

	rc = sha1_pkcs1_mgf1(seed_buf, md_len, in_buf + db_offset, db_len);
	if (rc) {
		TLOGE("%s,%d: hash failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -5;
		goto exit;
	}

	for (index = 0; index < md_len; ++index)
		seed_buf[index] ^= (in_buf + seed_offset)[index];

	rc = sha1_pkcs1_mgf1(db_buf, db_len, seed_buf, md_len);
	if (rc) {
		TLOGE("%s,%d: hash failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -6;
		goto exit;
	}

	for (index = 0; index < db_len; ++index)
		db_buf[index] ^= (in_buf + db_offset)[index];

	/* now DB == phash || PS || 0x01 || M, PS == k - mlen - 2hlen - 2 zeroes */

	rc = sha1(NULL, 0, seed_buf, &md_len);
	if (rc || md_len != EVP_MD_size(EVP_sha1())) {
		TLOGE("%s,%d: hash failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -7;
		goto exit;
	}

	rc = CRYPTO_memcmp(db_buf, seed_buf, md_len);
	if (rc) {
		TLOGE("%s,%d: check error (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -8;
		goto exit;
	}

	for (index = md_len; index < db_len && db_buf[index] == 0x0; ++index);
	if (index == db_len || db_buf[index] != 0x1) {
		TLOGE("%s,%d: check error!\n", __FUNCTION__, __LINE__);
		rc = -9;
		goto exit;
	}

	if (db_len - index - 1 > *out_len) {
		TLOGE("%s,%d: buffer is too small!\n", __FUNCTION__, __LINE__);
		rc = -10;
		goto exit;
	}

	*out_len = db_len - index - 1;
	if (*out_len > 0)
		memcpy(out_buf, db_buf + index + 1, *out_len);
exit:
	OPENSSL_free(db_buf);
	OPENSSL_free(seed_buf);
	return rc < 0 ? rc : 0;
}

static int32_t rsa_oaep_decode(const uint8_t *key_buf, const uint32_t key_len, const uint8_t *in_buf, const uint32_t in_len, uint8_t *out_buf, uint32_t *out_len)
{
	int32_t rc;
	uint32_t tmp_len = in_len;
	uint8_t *tmp_buf = OPENSSL_malloc(tmp_len);
	if (tmp_buf == NULL) {
		TLOGE("%s,%d: Allocation buffer failed!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	RSA *key = RSA_new();
	if (key == NULL) {
		TLOGE("%s,%d: Allocation key failed!\n", __FUNCTION__, __LINE__);
		OPENSSL_free(tmp_buf);
		return -2;
	}

	key->n = BN_bin2bn(key_buf, key_len, NULL);
	if (key->n == NULL) {
		TLOGE("%s,%d: Allocation modulus failed!\n", __FUNCTION__, __LINE__);
		rc = -3;
		goto exit;
	}

	key->e = BN_new();
	if (key->e == NULL) {
		TLOGE("%s,%d: Allocation exponent failed!\n", __FUNCTION__, __LINE__);
		rc = -4;
		goto exit;
	}

	rc = BN_set_u64(key->e, RSA_F4);
	if (rc != 1) {
		TLOGE("%s,%d: Set exponent failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -5;
		goto exit;
	}

	rc = RSA_check_key(key);
	if (rc != 1) {
		TLOGE("%s,%d: Check key failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -6;
		goto exit;
	}

	rc = RSA_encrypt(key, &tmp_len, tmp_buf, tmp_len, in_buf, in_len, RSA_NO_PADDING);
	if (rc != 1 || tmp_len != RSA_size(key)) {
		TLOGE("%s,%d: Encryption failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -7;
		goto exit;
	}

	rc = oaep_decode(tmp_buf, tmp_len, out_buf, out_len);
	if (rc != 0) {
		TLOGE("%s,%d: Decode failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -8;
		goto exit;
	}
exit:
	RSA_free(key);
	OPENSSL_free(tmp_buf);
	return rc < 0 ? rc : 0;
}

static int32_t pss_verify(const RSA *key, const uint8_t *mHash, const uint32_t mHash_len, const uint8_t *EM, const uint32_t sLen)
{
	int32_t rc;
	uint32_t md_len = EVP_MD_size(EVP_sha1()), db_len, em_len, H_offset, index,  MSBits;
	const uint8_t PSS_ZERO[8] = {0};
	uint8_t *DB, H_[EVP_MAX_MD_SIZE];

	MSBits = (BN_num_bits(key->n) - 1) & 0x7;
	em_len = RSA_size(key);
	if (EM[0] & (0xFF << MSBits)) {
		TLOGE("%s,%d: Header error!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if (MSBits == 0) {
		++EM;
		--em_len;
	}

	if (em_len < md_len + 2 || em_len < md_len + sLen + 2) {
		TLOGE("%s,%d: data is too small!\n", __FUNCTION__, __LINE__);
		return -2;
	}

	if (EM[em_len - 1] != 0xbc) {
		TLOGE("%s,%d: Tail error!\n", __FUNCTION__, __LINE__);
		return -3;
	}

	EVP_MD_CTX ctx;
	EVP_MD_CTX_init(&ctx);

	/* EM = DB || H || 0xBC */
	db_len = em_len - md_len - 1;
	DB = OPENSSL_malloc(db_len);
	if (DB == NULL) {
		TLOGE("%s,%d: Allocation fail!\n", __FUNCTION__, __LINE__);
		EVP_MD_CTX_cleanup(&ctx);
		return -4;
	}

	H_offset = db_len;
	rc = sha1_pkcs1_mgf1(DB, db_len, EM + H_offset, md_len);
	if (rc < 0) {
		TLOGE("%s,%d: hash failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -5;
		goto exit;
	}

	for (index = 0; index < db_len; ++index)
		DB[index] ^= EM[index];

	if (MSBits)
		DB[0] &= 0xFF >> (8 - MSBits);

	for (index = 0; index < db_len && DB[index] == 0x0; ++index);
	if (index == db_len || DB[index] != 0x1) {
		TLOGE("%s,%d: check error!\n", __FUNCTION__, __LINE__);
		rc = -6;
		goto exit;
	}

	if (db_len - index - 1 != sLen) {
		TLOGE("%s,%d: check error!\n", __FUNCTION__, __LINE__);
		rc = -7;
		goto exit;
	}

	rc = EVP_DigestInit_ex(&ctx, EVP_sha1(), NULL);
	if (rc != 1) {
		TLOGE("%s,%d: Initialize hash fail (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -8;
		goto exit;
	}

	rc = EVP_DigestUpdate(&ctx, PSS_ZERO, sizeof(PSS_ZERO));
	if (rc != 1) {
		TLOGE("%s,%d: Hash fail (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -9;
		goto exit;
	}

	rc = EVP_DigestUpdate(&ctx, mHash, mHash_len);
	if (rc != 1) {
		TLOGE("%s,%d: Hash fail (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -10;
		goto exit;
	}

	rc = EVP_DigestUpdate(&ctx, DB + index + 1, db_len - index - 1);
	if (rc != 1) {
		TLOGE("%s,%d: Hash fail (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -11;
		goto exit;
	}

	rc = EVP_DigestFinal_ex(&ctx, H_, NULL);
	if (rc != 1) {
		TLOGE("%s,%d: Hash fail (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -12;
		goto exit;
	}

	rc = CRYPTO_memcmp(H_, EM + H_offset, md_len);
	if (rc) {
		TLOGE("%s,%d: check error (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -13;
		goto exit;
	}
exit:
	OPENSSL_free(DB);
	EVP_MD_CTX_cleanup(&ctx);
	return rc < 0 ? rc : 0;
}

static int32_t rsa_pss_verify(const uint8_t *key_buf, const uint32_t key_len, const uint8_t *sig_buf, const uint32_t sig_len, const uint8_t *hash_buf, const uint32_t hash_len)
{
	int32_t rc;
	uint32_t tmp_len = sig_len;
	uint8_t *tmp_buf = OPENSSL_malloc(tmp_len);
	if (tmp_buf == NULL) {
		TLOGE("%s,%d: Allocation buffer failed!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	RSA *key = RSA_new();
	if (key == NULL) {
		TLOGE("%s,%d: Allocation key failed!\n", __FUNCTION__, __LINE__);
		OPENSSL_free(tmp_buf);
		return -2;
	}

	key->n = BN_bin2bn(key_buf, key_len, NULL);
	if (key->n == NULL) {
		TLOGE("%s,%d: Allocation modulus failed!\n", __FUNCTION__, __LINE__);
		rc = -3;
		goto exit;
	}

	key->e = BN_new();
	if (key->e == NULL) {
		TLOGE("%s,%d: Allocation exponent failed!\n", __FUNCTION__, __LINE__);
		rc = -4;
		goto exit;
	}

	rc = BN_set_u64(key->e, RSA_F4);
	if (rc != 1) {
		TLOGE("%s,%d: Set exponent failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -5;
		goto exit;
	}

	rc = RSA_check_key(key);
	if (rc != 1) {
		TLOGE("%s,%d: Check key failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -6;
		goto exit;
	}

	rc = RSA_verify_raw(key, &tmp_len, tmp_buf, tmp_len, sig_buf, sig_len, RSA_NO_PADDING);
	if (rc != 1 || tmp_len != RSA_size(key)) {
		TLOGE("%s,%d: Verify failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -7;
		goto exit;
	}
#if 0
	for (int i =0; i<256; i+=8)
		TLOGE("%s,%d: tmp_buf: %x:%x:%x:%x:%x:%x:%x:%x\n", __FUNCTION__, __LINE__, tmp_buf[i], tmp_buf[i+1], tmp_buf[i+2], tmp_buf[i+3], tmp_buf[i+4], tmp_buf[i+5], tmp_buf[i+6], tmp_buf[i+7]);
#endif

	rc = pss_verify(key, hash_buf, hash_len, tmp_buf, 15);
	if (rc != 0) {
		TLOGE("%s,%d: Decode failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		rc = -8;
		goto exit;
	}
exit:
	RSA_free(key);
	OPENSSL_free(tmp_buf);
	return rc < 0 ? rc : 0;
}

static int32_t decrypt_Kkb_pub(const uint8_t *Pkb, const uint8_t *Cpre, const uint8_t *EKkb_pub, uint8_t *Kkb_pub)
{
	int32_t rc;
	uint8_t hash_buf[EVP_MAX_MD_SIZE] = {0}, iv_buf[16], key_buf[16];
	uint32_t hash_len = EVP_MAX_MD_SIZE, i;

	if (Pkb == NULL || Cpre == NULL || EKkb_pub == NULL || Kkb_pub == NULL) {
		TLOGE("%s,%d: NULL pointer or zero length!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if (Pkb[0] != 0x0) {
		TLOGE("%s,%d: Wrong header in parameter!\n", __FUNCTION__, __LINE__);
		return -2;
	}

	rc = sha256(Pkb + 1, 128, hash_buf, &hash_len);
	if (rc) {
		TLOGE("%s,%d: hash failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		return -3;
	}

	for (i = 0; i < 16; ++i) {
		iv_buf[i] = hash_buf[i] ^ Cpre[i];
		key_buf[i] = hash_buf[i + 16] ^ Cpre[i + 16];
	}

	rc = aes_cbc(key_buf, iv_buf, 0, EKkb_pub, 256, Kkb_pub);
	if (rc) {
		TLOGE("%s,%d: decryption failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		return -4;
	}

	return 0;
}

static int32_t decrypt_Kkb(const uint8_t *Kkb_pub, const uint32_t Kkb_pub_len, const uint8_t *EKkb, const uint32_t EKkb_len, uint8_t *Kkb, uint32_t *Kkb_len)
{
	int32_t rc;

	if (Kkb_pub == NULL || Kkb_pub_len == 0 || EKkb == NULL || EKkb_len == 0 || Kkb == NULL || Kkb_len == NULL || *Kkb_len == 0) {
		TLOGE("%s,%d: NULL pointer or zero length!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if (Kkb_pub[0] != 0x0) {
		TLOGE("%s,%d: Wrong header in parameter!\n", __FUNCTION__, __LINE__);
		return -2;
	}

	rc = rsa_oaep_decode(Kkb_pub, Kkb_pub_len, EKkb, EKkb_len, Kkb, Kkb_len);
	if (rc) {
		TLOGE("%s,%d: decryption failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		return -3;
	}

	return 0;
}

static int32_t verify_keybox(const uint8_t *Kkb_pub, const uint32_t Kkb_pub_len, const uint8_t *keybox, const uint32_t keybox_len)
{
	int32_t rc;
	uint8_t hash_buf[EVP_MAX_MD_SIZE] = {0};
	uint32_t hash_len = EVP_MAX_MD_SIZE;

	if (Kkb_pub == NULL || Kkb_pub_len == 0 || keybox == NULL || keybox_len == 0) {
		TLOGE("%s,%d: NULL pointer or zero length!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	rc = sha256(keybox, keybox_len - SZ_ENCKB_SIG, hash_buf, &hash_len);
	if (rc) {
		TLOGE("%s,%d: Hash failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		return -2;
	}

	rc = rsa_pss_verify(Kkb_pub, Kkb_pub_len, keybox + keybox_len - SZ_ENCKB_SIG, SZ_ENCKB_SIG, hash_buf, hash_len);
	if (rc) {
		TLOGE("%s,%d: verification failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		return -3;
	}

	return 0;
}

static int32_t verify_key(const uint8_t *Kkb_pub, const uint32_t Kkb_pub_len, const uint8_t *key, const uint32_t key_len, const uint8_t *sig, const uint32_t sig_len)
{
	int32_t rc;
	uint8_t hash_buf[EVP_MAX_MD_SIZE] = {0};
	uint32_t hash_len = EVP_MAX_MD_SIZE;

	if (Kkb_pub == NULL || Kkb_pub_len == 0 || key == NULL || key_len == 0) {
		TLOGE("%s,%d: NULL pointer or zero length!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	rc = sha256(key, key_len, hash_buf, &hash_len);
	if (rc) {
		TLOGE("%s,%d: Hash failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		return -2;
	}

	rc = rsa_pss_verify(Kkb_pub, Kkb_pub_len, sig, sig_len, hash_buf, hash_len);
	if (rc) {
		TLOGE("%s,%d: verification failed (%d)!\n", __FUNCTION__, __LINE__, rc);
		return -3;
	}

	return 0;
}

int32_t decrypt_keybox(uint8_t *in_keybox, uint32_t in_len, uint8_t *out_keybox, uint32_t *out_len)
{
	int32_t ret;
	uint32_t total_len, Kkb_len, key_count;
	uint8_t Kkb_pub[SZ_ENCKB_PUB_KEY + 1] = {0}, Kkb[SZ_AES_IV + SZ_AES_KEY] = {0}, enc_iv[SZ_AES_IV], *drm_ptr;

	if (in_keybox == NULL || in_len == 0 || out_keybox == NULL || out_len == NULL) {
		TLOGE("%s,%d: NULL pointer or zero length!\n", __FUNCTION__, __LINE__);
		return -1;
	}

	if (in_keybox[0] != 'E' || in_keybox[1] != 'n' || in_keybox[2] != 'c' || in_keybox[3] != 'S') {
		TLOGE("%s,%d: Magic number error in Encrypt Key block!\n", __FUNCTION__, __LINE__);
		return -2;
	}

	if (in_len < SZ_ENCKB_TOTAL_HEADER + SZ_ENCKB_SIG) {
		TLOGE("%s,%d: input length is too small!\n", __FUNCTION__, __LINE__);
		return -3;
	}

	memcpy(&total_len, in_keybox + OFFSET_ENCKB_HEADER_SIZE, sizeof(uint32_t));
	if (in_len != total_len || *out_len != total_len) {
		TLOGE("%s,%d: input length or output length is not match total length!\n", __FUNCTION__, __LINE__);
		return -4;
	}

	TLOGE("%s,%d: Decrypt Kkb_pub...\n", __FUNCTION__, __LINE__);
	ret = decrypt_Kkb_pub(InputPkb, Cpre, Input_Ekkb_pub, Kkb_pub + 1);
	if (ret != 0) {
		TLOGE("%s,%d: Decrypt Kkb_pub fail (%d)!\n", __FUNCTION__, __LINE__, ret);
		return -5;
	}
#if 0
	TLOGE("%s,%d: kkb_pub: %x\n", __FUNCTION__, __LINE__, Kkb_pub[0]);
	for (int i =1; i<SZ_ENCKB_PUB_KEY + 1; i+=8)
		TLOGE("%s,%d: kkb_pub: %x:%x:%x:%x:%x:%x:%x:%x\n", __FUNCTION__, __LINE__, Kkb_pub[i], Kkb_pub[i+1], Kkb_pub[i+2], Kkb_pub[i+3], Kkb_pub[i+4], Kkb_pub[i+5], Kkb_pub[i+6], Kkb_pub[i+7]);
#endif

	memcpy(&Kkb_len, in_keybox + OFFSET_ENCKB_HEADER_HASEKKB, sizeof(uint32_t));
	if (Kkb_len != 1) {
		TLOGE("%s,%d: Wrong key header (%u)!\n", __FUNCTION__, __LINE__, Kkb_len);
		return -6;
	}

	TLOGE("%s,%d: Decrypt Kkb...\n", __FUNCTION__, __LINE__);
	Kkb_len = SZ_AES_IV + SZ_AES_KEY;
	ret = decrypt_Kkb(Kkb_pub, SZ_ENCKB_PUB_KEY + 1, in_keybox + OFFSET_ENCKB_HEADER_EKKB, SZ_ENCKB_HEADER_EKKB, Kkb, &Kkb_len);
	if (ret != 0) {
		TLOGE("%s,%d: Decrypt Kkb fail (%d)!\n", __FUNCTION__, __LINE__, ret);
		return -7;
	}
#if 0
	for (int i = 0; i<SZ_AES_IV + SZ_AES_KEY; i+=8)
		TLOGE("%s,%d: kkb: %x:%x:%x:%x:%x:%x:%x:%x\n", __FUNCTION__, __LINE__, Kkb[i], Kkb[i+1], Kkb[i+2], Kkb[i+3], Kkb[i+4], Kkb[i+5], Kkb[i+6], Kkb[i+7]);
#endif

	TLOGE("%s,%d: Verify Keybox Signature...\n", __FUNCTION__, __LINE__);
	ret = verify_keybox(Kkb_pub, SZ_ENCKB_PUB_KEY + 1, in_keybox, in_len);
	if (ret != 0) {
		TLOGE("%s,%d: Verify Keybox fail (%d)!\n", __FUNCTION__, __LINE__, ret);
		return -8;
	}

	TLOGE("%s,%d: Process divided drm keys...\n", __FUNCTION__, __LINE__);
	memcpy(&key_count, in_keybox + OFFSET_ENCKB_HEADER_KEYCOUNT, sizeof(uint32_t));
	if (key_count != 1) {
		TLOGE("%s,%d: Wrong key number (%u)!\n", __FUNCTION__, __LINE__, key_count);
		return -9;
	}

	ENCRYPT_DRM_KEY_T encryptDrm;
	drm_ptr = in_keybox + SZ_ENCKB_TOTAL_HEADER;

	memcpy(&encryptDrm, drm_ptr, sizeof(ENCRYPT_DRM_KEY_T));
	encryptDrm.drmKey = (uint8_t *)(&((ENCRYPT_DRM_KEY_T *)drm_ptr)->drmKey);
	encryptDrm.drmKeySignature = encryptDrm.drmKey + encryptDrm.encDrmKeySize;

	memset(enc_iv, 0, SZ_AES_IV);
	ret = memcmp(encryptDrm.clearIV, enc_iv, SZ_DRMKEY_CLEAR_IV);
	if (ret == 0) {
		TLOGE("%s,%d: IV is all zero!\n", __FUNCTION__, __LINE__);
		return -10;
	}
#if 0
	TLOGE("%s,%d: ID = %u, type = %u, size = %u, enc_size = %u\n", __FUNCTION__, __LINE__, encryptDrm.drmKeyID, encryptDrm.drmKeyType, encryptDrm.drmKeySize, encryptDrm.encDrmKeySize);
	for (int i = 0; i<16; ++i) {
		TLOGE("%s,%d: clearIV = %x\n", __FUNCTION__, __LINE__, encryptDrm.clearIV[i]);
	}
	for (int i = 0; i<48; ++i) {
		TLOGE("%s,%d: reserved = %x\n", __FUNCTION__, __LINE__, encryptDrm.reserved[i]);
		//if (encryptDrm.reserved[i] != 0x0)
		//	TLOGE("%s,%d: reserved is not all zero!\n", __FUNCTION__, __LINE__);
	}
#endif
	ret = aes_ecb(encryptDrm.clearIV, 1, Kkb, SZ_AES_IV, enc_iv);
	if (ret != 0) {
		TLOGE("%s,%d: Encrypting iv fail (%d)!\n", __FUNCTION__, __LINE__, ret);
		return -11;
	}

	ret = aes_cbc(Kkb + SZ_AES_IV, enc_iv, 0, encryptDrm.drmKey, encryptDrm.encDrmKeySize, out_keybox);
	if (ret != 0) {
		TLOGE("%s,%d: Decrypting key fail (%d)!\n", __FUNCTION__, __LINE__, ret);
		return -12;
	}

	*out_len = encryptDrm.drmKeySize;
	uint8_t *key_buf = malloc(*out_len + SZ_DRMKEY_ID + SZ_DRMKEY_SIZE);
	if (key_buf == NULL) {
		TLOGE("%s,%d: Allocation fail!\n", __FUNCTION__, __LINE__);
		return -13;
	}

	memcpy(key_buf, &encryptDrm.drmKeyID, SZ_DRMKEY_ID);
	memcpy(key_buf + SZ_DRMKEY_ID, &encryptDrm.drmKeySize, SZ_DRMKEY_SIZE);
	memcpy(key_buf + SZ_DRMKEY_ID + SZ_DRMKEY_SIZE, out_keybox, *out_len);

	ret = verify_key(Kkb_pub, SZ_ENCKB_PUB_KEY + 1, key_buf, *out_len + SZ_DRMKEY_ID + SZ_DRMKEY_SIZE, encryptDrm.drmKeySignature, SZ_DRMKEY_SIG);
	if (ret != 0) {
		TLOGE("%s,%d: Verify Key fail (%d)!\n", __FUNCTION__, __LINE__, ret);
		free(key_buf);
		return -14;
	}

	TLOGE("%s,%d: Decrypting keybox is success!\n", __FUNCTION__, __LINE__);
	free(key_buf);
	return 0;
}
