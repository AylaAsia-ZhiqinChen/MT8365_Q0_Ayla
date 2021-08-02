/*
 * Copyright (C) 2017 The Android Open Source Project
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

/**
 * This app tests the API in app/keymaster/secure_storage.h. To run this test,
 * include keymaster/storage_test in TRUSTY_ALL_USER_TASKS, and it will be start
 * once an RPMB proxy becomes available.
 *
 * *** IMPORTANT ***
 * This test will delete all existing attestation data stored in RPMB. It would
 * also delete the product id stored in RPMB. Don't run this test on a permanent
 * attribute fused device since this test would put the device into an
 * inconsistent state.
 */

#include <assert.h>
#include <err.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define typeof(x) __typeof__(x)
#include <lib/rng/trusty_rng.h>
#include <lib/storage/storage.h>
#include <trusty_unittest.h>

#include <UniquePtr.h>
#include <keymaster/android_keymaster_utils.h>
#include "../secure_storage.h"

#define DATA_SIZE 1000
#define CHAIN_LENGTH 3

#define EXPECT_ALL_OK()  \
    if (!_all_ok) {      \
        goto test_abort; \
    }

#define LOG_TAG "km_storage_test"

#define ASSERT_EQ(e, a)      \
    do {                     \
        EXPECT_EQ(e, a, ""); \
        EXPECT_ALL_OK();     \
    } while (0)

#define ASSERT_NE(e, a)      \
    do {                     \
        EXPECT_NE(e, a, ""); \
        EXPECT_ALL_OK();     \
    } while (0)

using keymaster::AttestationKeySlot;
using keymaster::CertificateChainDelete;
using keymaster::DeleteAllAttestationData;
using keymaster::DeleteProductId;
using keymaster::KeymasterKeyBlob;
using keymaster::kProductIdSize;
using keymaster::ReadProductId;
using keymaster::SetProductId;

uint8_t* NewRandBuf(uint32_t size) {
    UniquePtr<uint8_t[]> buf(new uint8_t[size]);
    if (!buf.get()) {
        return nullptr;
    }
    if (trusty_rng_secure_rand(buf.get(), size) != 0) {
        return nullptr;
    }
    return buf.release();
}

void TestKeyStorage(AttestationKeySlot key_slot) {
    keymaster_error_t error = KM_ERROR_OK;
    UniquePtr<uint8_t[]> write_key;
    KeymasterKeyBlob key_blob;
    bool key_exists = false;

    TEST_BEGIN(__func__);

    write_key.reset(NewRandBuf(DATA_SIZE));
    ASSERT_NE(nullptr, write_key.get());

    error = WriteKeyToStorage(key_slot, write_key.get(), DATA_SIZE);
    ASSERT_EQ(KM_ERROR_OK, error);

    key_blob = ReadKeyFromStorage(key_slot, &error);
    ASSERT_EQ(KM_ERROR_OK, error);
    ASSERT_EQ(DATA_SIZE, key_blob.key_material_size);
    ASSERT_NE(nullptr, key_blob.key_material);
    ASSERT_EQ(0, memcmp(write_key.get(), key_blob.key_material, DATA_SIZE));

    error = AttestationKeyExists(AttestationKeySlot::kRsa, &key_exists);
    ASSERT_EQ(KM_ERROR_OK, error);
    ASSERT_EQ(true, key_exists);

test_abort:
    TEST_END;
}

void TestCertChainStorage(AttestationKeySlot key_slot, bool chain_exists) {
    keymaster_error_t error = KM_ERROR_OK;
    UniquePtr<uint8_t[]> write_cert[CHAIN_LENGTH];
    unsigned int i = 0;
    uint32_t cert_chain_length;
    UniquePtr<keymaster_cert_chain_t, CertificateChainDelete> chain;

    TEST_BEGIN(__func__);

    for (i = 0; i < CHAIN_LENGTH; ++i) {
        write_cert[i].reset(NewRandBuf(DATA_SIZE));
        ASSERT_NE(nullptr, write_cert[i].get());

        error = WriteCertToStorage(key_slot, write_cert[i].get(), DATA_SIZE, i);
        ASSERT_EQ(KM_ERROR_OK, error);

        error = ReadCertChainLength(key_slot, &cert_chain_length);
        ASSERT_EQ(KM_ERROR_OK, error);
        if (chain_exists) {
            ASSERT_EQ(3, cert_chain_length);
        } else {
            ASSERT_EQ(i + 1, cert_chain_length);
        }
    }

    chain.reset(new keymaster_cert_chain_t);
    ASSERT_NE(nullptr, chain.get());
    error = ReadCertChainFromStorage(key_slot, chain.get());
    ASSERT_EQ(KM_ERROR_OK, error);
    ASSERT_EQ(CHAIN_LENGTH, chain.get()->entry_count);
    for (i = 0; i < CHAIN_LENGTH; ++i) {
        ASSERT_EQ(DATA_SIZE, chain.get()->entries[i].data_length);
        ASSERT_EQ(0, memcmp(write_cert[i].get(), chain.get()->entries[i].data,
                            DATA_SIZE));
    }

test_abort:
    TEST_END;
}

void TestCertStorageInvalid(AttestationKeySlot key_slot) {
    keymaster_error_t error = KM_ERROR_OK;
    UniquePtr<uint8_t[]> write_cert;
    uint32_t cert_chain_length;

    TEST_BEGIN(__func__);

    // Clear existing certificate chain
    error = DeleteCertChain(key_slot);
    error = ReadCertChainLength(key_slot, &cert_chain_length);
    ASSERT_EQ(KM_ERROR_OK, error);
    ASSERT_EQ(0, cert_chain_length);

    // Try to write to index (chain_length + 1)
    write_cert.reset(NewRandBuf(DATA_SIZE));
    ASSERT_NE(nullptr, write_cert.get());
    error = WriteCertToStorage(key_slot, write_cert.get(), DATA_SIZE, 1);
    ASSERT_EQ(KM_ERROR_INVALID_ARGUMENT, error);

    // Verify that cert chain length didn't change
    error = ReadCertChainLength(key_slot, &cert_chain_length);
    ASSERT_EQ(KM_ERROR_OK, error);
    ASSERT_EQ(0, cert_chain_length);

test_abort:
    TEST_END;
}

void DeleteAttestationData() {
    keymaster_error_t error = KM_ERROR_OK;
    uint32_t cert_chain_length;
    bool key_exists;

    TEST_BEGIN(__func__);

    error = DeleteAllAttestationData();
    ASSERT_EQ(KM_ERROR_OK, error);

    error = ReadCertChainLength(AttestationKeySlot::kRsa, &cert_chain_length);
    ASSERT_EQ(KM_ERROR_OK, error);
    ASSERT_EQ(0, cert_chain_length);
    error = ReadCertChainLength(AttestationKeySlot::kEcdsa, &cert_chain_length);
    ASSERT_EQ(KM_ERROR_OK, error);
    ASSERT_EQ(0, cert_chain_length);

    error = AttestationKeyExists(AttestationKeySlot::kRsa, &key_exists);
    ASSERT_EQ(KM_ERROR_OK, error);
    ASSERT_EQ(false, key_exists);
    error = AttestationKeyExists(AttestationKeySlot::kEcdsa, &key_exists);
    ASSERT_EQ(KM_ERROR_OK, error);
    ASSERT_EQ(false, key_exists);

test_abort:
    TEST_END;
}

void TestProductIdStorage() {
    keymaster_error_t error = KM_ERROR_OK;
    UniquePtr<uint8_t[]> write_productid;
    UniquePtr<uint8_t[]> read_productid(new uint8_t[kProductIdSize]);

    TEST_BEGIN(__func__);

    error = DeleteProductId();
    ASSERT_EQ(KM_ERROR_OK, error);

    write_productid.reset(NewRandBuf(kProductIdSize));
    ASSERT_NE(nullptr, write_productid.get());

    error = SetProductId((const uint8_t*)write_productid.get());
    ASSERT_EQ(KM_ERROR_OK, error);

    error = ReadProductId(read_productid.get());
    ASSERT_EQ(KM_ERROR_OK, error);
    ASSERT_NE(nullptr, read_productid.get());
    ASSERT_EQ(0, memcmp(write_productid.get(), read_productid.get(),
                        kProductIdSize));

    error = DeleteProductId();
    ASSERT_EQ(KM_ERROR_OK, error);

test_abort:
    TEST_END;
}

void TestProductIdStoragePreventOverwrite() {
    keymaster_error_t error = KM_ERROR_OK;
    UniquePtr<uint8_t[]> write_productid;
    UniquePtr<uint8_t[]> overwrite_productid;
    UniquePtr<uint8_t[]> read_productid(new uint8_t[kProductIdSize]);

    TEST_BEGIN(__func__);

    error = DeleteProductId();
    ASSERT_EQ(KM_ERROR_OK, error);

    write_productid.reset(NewRandBuf(kProductIdSize));
    ASSERT_NE(nullptr, write_productid.get());

    error = SetProductId((const uint8_t*)write_productid.get());
    ASSERT_EQ(KM_ERROR_OK, error);

    overwrite_productid.reset(NewRandBuf(kProductIdSize));
    error = SetProductId((const uint8_t*)write_productid.get());
    ASSERT_EQ(KM_ERROR_INVALID_ARGUMENT, error);

    error = ReadProductId(read_productid.get());
    ASSERT_EQ(KM_ERROR_OK, error);
    ASSERT_NE(nullptr, read_productid.get());
    ASSERT_EQ(0, memcmp(write_productid.get(), read_productid.get(),
                        kProductIdSize));

    error = DeleteProductId();
    ASSERT_EQ(KM_ERROR_OK, error);

test_abort:
    TEST_END;
}

int main(void) {

    TLOGI("km_storage_test: running all\n");

    DeleteAttestationData();

    TestKeyStorage(AttestationKeySlot::kRsa);
    TestKeyStorage(AttestationKeySlot::kEcdsa);
    TestCertChainStorage(AttestationKeySlot::kRsa, false);
    TestCertChainStorage(AttestationKeySlot::kEcdsa, false);

    // Rewriting keys should work
    TestKeyStorage(AttestationKeySlot::kRsa);
    TestCertChainStorage(AttestationKeySlot::kRsa, true);

    TestCertStorageInvalid(AttestationKeySlot::kRsa);

    TestProductIdStorage();

#ifndef KEYMASTER_DEBUG
    TestProductIdStoragePreventOverwrite();
#endif

    DeleteAttestationData();

    TLOGI("km_storage_test: complete!\n");
    return 0;
}
