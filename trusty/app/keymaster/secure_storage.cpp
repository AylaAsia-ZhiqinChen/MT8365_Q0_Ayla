/*
 * Copyright 2017 The Android Open Source Project
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

#include "secure_storage.h"

#include <errno.h>
#include <stdio.h>
#include <uapi/err.h>

#include <lib/storage/storage.h>

#include <keymaster/UniquePtr.h>
#include "trusty_keymaster_context.h"
#include "trusty_logger.h"

namespace keymaster {

namespace {

// Name of the attestation key file is kAttestKeyPrefix.%algorithm, where
// algorithm is either "ec" or "rsa".
const char* kAttestKeyPrefix = "AttestKey.";

// Name of the attestation certificate file is
// kAttestCertPrefix.%algorithm.%index, where index is the index within the
// certificate chain.
const char* kAttestCertPrefix = "AttestCert.";

const char* kAttestUuidFileName = "AttestUuid";
const char* kProductIdFileName = "ProductId";

// Maximum file name size.
static const int kStorageIdLengthMax = 64;

// RAII wrapper for storage_session_t
class StorageSession {
public:
    StorageSession() {
        error_ = storage_open_session(&handle_, STORAGE_CLIENT_TP_PORT);
        if (error_ < 0) {
            LOG_E("Error: [%d] opening storage session", error_);
        }
    }
    ~StorageSession() {
        if (error_ < 0) {
            return;
        }
        storage_close_session(handle_);
        error_ = -EINVAL;
    }

    int error() const { return error_; }
    storage_session_t handle() { return handle_; }

private:
    storage_session_t handle_ = 0;
    int error_ = -EINVAL;
};

// RAII wrapper for file_handle_t
class FileHandle {
public:
    FileHandle(const char* filename, int flags) {
        if (session_.error() == 0) {
            error_ = storage_open_file(session_.handle(), &handle_,
                                       const_cast<char*>(filename), flags, 0);
        } else {
            error_ = session_.error();
        }
    }
    ~FileHandle() {
        if (error_ != 0) {
            return;
        }
        storage_close_file(handle_);
        error_ = -EINVAL;
    }
    int error() const { return error_; }
    file_handle_t handle() { return handle_; }

private:
    StorageSession session_;
    int error_ = -EINVAL;
    file_handle_t handle_ = 0;
};

bool SecureStorageWrite(const char* filename, const void* data, uint32_t size) {
    FileHandle file(filename,
                    STORAGE_FILE_OPEN_CREATE | STORAGE_FILE_OPEN_TRUNCATE);
    if (file.error() < 0) {
        return false;
    }
    int rc = storage_write(file.handle(), 0, data, size, STORAGE_OP_COMPLETE);
    if (rc < 0) {
        LOG_E("Error: [%d] writing storage object '%s'", rc, filename);
        return false;
    }
    if (static_cast<uint32_t>(rc) < size) {
        LOG_E("Error: invalid object size [%d] from '%s'", rc, filename);
        return false;
    }
    return true;
}

bool SecureStorageRead(const char* filename, void* data, uint32_t size) {
    FileHandle file(filename, STORAGE_FILE_OPEN_CREATE);
    if (file.error() < 0) {
        return false;
    }
    int rc = storage_read(file.handle(), 0, data, size);
    if (rc < 0) {
        LOG_E("Error: [%d] reading storage object '%s'", rc, filename);
        return false;
    }
    if (static_cast<uint32_t>(rc) < size) {
        LOG_E("Error: invalid object size [%d] from '%s'", rc, filename);
        return false;
    }
    return true;
}

bool SecureStorageGetFileSize(const char* filename, uint64_t* size) {
    FileHandle file(filename, STORAGE_FILE_OPEN_CREATE);
    if (file.error() < 0) {
        return false;
    }
    int rc = storage_get_file_size(file.handle(), size);
    if (rc < 0) {
        LOG_E("Error: [%d] reading storage object '%s'", rc, filename);
        return false;
    }
    return true;
}

bool SecureStorageDeleteFile(const char* filename) {
    StorageSession session;
    if (session.error() < 0) {
        return false;
    }
    int rc = storage_delete_file(session.handle(), filename,
                                 STORAGE_OP_COMPLETE);
    if (rc < 0 && rc != ERR_NOT_FOUND) {
        LOG_E("Error: [%d] deleting storage object '%s'", rc, filename);
        return false;
    }
    return true;
}

const char* GetKeySlotStr(AttestationKeySlot key_slot) {
    switch (key_slot) {
    case AttestationKeySlot::kRsa:
        return "rsa";
    case AttestationKeySlot::kEcdsa:
        return "ec";
    case AttestationKeySlot::kEddsa:
        return "ed";
    case AttestationKeySlot::kEpid:
        return "epid";
    case AttestationKeySlot::kClaimable0:
        return "c0";
    case AttestationKeySlot::kSomRsa:
        return "s_rsa";
    case AttestationKeySlot::kSomEcdsa:
        return "s_ec";
    case AttestationKeySlot::kSomEddsa:
        return "s_ed";
    case AttestationKeySlot::kSomEpid:
        return "s_epid";
    default:
        return "";
    }
}

}  //  unnamed namespace

keymaster_error_t WriteKeyToStorage(AttestationKeySlot key_slot,
                                    const uint8_t* key,
                                    uint32_t key_size) {
    UniquePtr<char[]> key_file(new char[kStorageIdLengthMax]);
    snprintf(key_file.get(), kStorageIdLengthMax, "%s.%s", kAttestKeyPrefix,
             GetKeySlotStr(key_slot));
    if (!SecureStorageWrite(key_file.get(), key, key_size)) {
        return KM_ERROR_UNKNOWN_ERROR;
    }

    return KM_ERROR_OK;
}

KeymasterKeyBlob ReadKeyFromStorage(AttestationKeySlot key_slot,
                                    keymaster_error_t* error) {
    UniquePtr<char[]> key_file(new char[kStorageIdLengthMax]);
    snprintf(key_file.get(), kStorageIdLengthMax, "%s.%s", kAttestKeyPrefix,
             GetKeySlotStr(key_slot));

    uint64_t key_size_64;
    if (!SecureStorageGetFileSize(key_file.get(), &key_size_64) ||
        key_size_64 == 0) {
        if (error)
            *error = KM_ERROR_UNKNOWN_ERROR;
        return {};
    }

    KeymasterKeyBlob result(key_size_64);
    if (result.key_material == nullptr) {
        if (error)
            *error = KM_ERROR_MEMORY_ALLOCATION_FAILED;
        return {};
    }
    if (!SecureStorageRead(key_file.get(), result.writable_data(),
                           result.key_material_size)) {
        if (error)
            *error = KM_ERROR_UNKNOWN_ERROR;
        return {};
    }
    if (error)
        *error = KM_ERROR_OK;
    return result;
}

keymaster_error_t AttestationKeyExists(AttestationKeySlot key_slot,
                                       bool* exists) {
    UniquePtr<char[]> key_file(new char[kStorageIdLengthMax]);
    snprintf(key_file.get(), kStorageIdLengthMax, "%s.%s", kAttestKeyPrefix,
             GetKeySlotStr(key_slot));
    uint64_t size;
    if (!SecureStorageGetFileSize(key_file.get(), &size)) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    *exists = size > 0;
    return KM_ERROR_OK;
}

keymaster_error_t WriteCertToStorage(AttestationKeySlot key_slot,
                                     const uint8_t* cert,
                                     uint32_t cert_size,
                                     uint32_t index) {
    UniquePtr<char[]> cert_file(new char[kStorageIdLengthMax]);
    uint32_t cert_chain_length;
    bool update_cert_chain_length = false;
    if (ReadCertChainLength(key_slot, &cert_chain_length) != KM_ERROR_OK) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    if (index > cert_chain_length) {
        return KM_ERROR_INVALID_ARGUMENT;
    } else if (index == cert_chain_length) {
        cert_chain_length += 1;
        update_cert_chain_length = true;
    }

    snprintf(cert_file.get(), kStorageIdLengthMax, "%s.%s.%d",
             kAttestCertPrefix, GetKeySlotStr(key_slot), index);
    if (!SecureStorageWrite(cert_file.get(), cert, cert_size)) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    if (update_cert_chain_length &&
        WriteCertChainLength(key_slot, cert_chain_length) != KM_ERROR_OK) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    return KM_ERROR_OK;
}

keymaster_error_t ReadCertChainFromStorage(AttestationKeySlot key_slot,
                                           keymaster_cert_chain_t* cert_chain) {
    UniquePtr<char[]> cert_file(new char[kStorageIdLengthMax]);
    uint32_t cert_chain_length;
    uint64_t cert_size;

    if (ReadCertChainLength(key_slot, &cert_chain_length) != KM_ERROR_OK ||
        cert_chain_length == 0) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    cert_chain->entry_count = cert_chain_length;
    cert_chain->entries = new keymaster_blob_t[cert_chain_length];
    if (!cert_chain->entries) {
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    memset(cert_chain->entries, 0,
           sizeof(cert_chain->entries[0]) * cert_chain_length);

    // Read |cert_chain_length| certs from storage
    for (size_t i = 0; i < cert_chain_length; i++) {
        snprintf(cert_file.get(), kStorageIdLengthMax, "%s.%s.%d",
                 kAttestCertPrefix, GetKeySlotStr(key_slot), i);
        if (!SecureStorageGetFileSize(cert_file.get(), &cert_size) ||
            cert_size == 0) {
            return KM_ERROR_UNKNOWN_ERROR;
        }
        UniquePtr<uint8_t[]> cert_data(new uint8_t[cert_size]);
        if (!cert_data.get()) {
            return KM_ERROR_MEMORY_ALLOCATION_FAILED;
        }
        if (!SecureStorageRead(cert_file.get(), cert_data.get(), cert_size)) {
            return KM_ERROR_UNKNOWN_ERROR;
        }
        cert_chain->entries[i].data_length = static_cast<uint32_t>(cert_size);
        cert_chain->entries[i].data = cert_data.release();
    }
    return KM_ERROR_OK;
}

keymaster_error_t WriteCertChainLength(AttestationKeySlot key_slot,
                                       uint32_t cert_chain_length) {
    UniquePtr<char[]> cert_chain_length_file(new char[kStorageIdLengthMax]);
    snprintf(cert_chain_length_file.get(), kStorageIdLengthMax, "%s.%s.length",
             kAttestKeyPrefix, GetKeySlotStr(key_slot));
    if (cert_chain_length > kMaxCertChainLength) {
        return KM_ERROR_UNKNOWN_ERROR;
    }

    uint32_t current_cert_chain_length = 0;
    if (ReadCertChainLength(key_slot, &current_cert_chain_length) !=
                KM_ERROR_OK ||
        cert_chain_length > current_cert_chain_length + 1) {
        LOG_E("Error: Cannot increase certificate chain length by more than 1.",
              0);
        return KM_ERROR_INVALID_ARGUMENT;
    }
    if (!SecureStorageWrite(cert_chain_length_file.get(), &cert_chain_length,
                            sizeof(uint32_t))) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    return KM_ERROR_OK;
}

keymaster_error_t ReadCertChainLength(AttestationKeySlot key_slot,
                                      uint32_t* cert_chain_length) {
    UniquePtr<char[]> cert_chain_length_file(new char[kStorageIdLengthMax]);
    snprintf(cert_chain_length_file.get(), kStorageIdLengthMax, "%s.%s.length",
             kAttestKeyPrefix, GetKeySlotStr(key_slot));

    uint64_t file_size;
    if (!SecureStorageGetFileSize(cert_chain_length_file.get(), &file_size)) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    if (file_size == 0) {
        *cert_chain_length = 0;
        return KM_ERROR_OK;
    }

    if (!SecureStorageRead(cert_chain_length_file.get(), cert_chain_length,
                           sizeof(uint32_t)) ||
        *cert_chain_length > kMaxCertChainLength) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    return KM_ERROR_OK;
}

keymaster_error_t WriteAttestationUuid(
        const uint8_t attestation_uuid[kAttestationUuidSize]) {
    if (!SecureStorageWrite(kAttestUuidFileName, attestation_uuid,
                            kAttestationUuidSize)) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    return KM_ERROR_OK;
}

keymaster_error_t ReadAttestationUuid(
        uint8_t attestation_uuid[kAttestationUuidSize]) {
    uint64_t size;
    if (!SecureStorageGetFileSize(kAttestUuidFileName, &size)) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    if (size < kAttestationUuidSize) {
        memset(attestation_uuid, '\0', kAttestationUuidSize);
        return KM_ERROR_OK;
    }
    if (!SecureStorageRead(kAttestUuidFileName, attestation_uuid,
                           kAttestationUuidSize)) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    return KM_ERROR_OK;
}

keymaster_error_t DeleteKey(AttestationKeySlot key_slot) {
    UniquePtr<char[]> key_file(new char[kStorageIdLengthMax]);
    snprintf(key_file.get(), kStorageIdLengthMax, "%s.%s", kAttestKeyPrefix,
             GetKeySlotStr(key_slot));
    if (!SecureStorageDeleteFile(key_file.get())) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    return KM_ERROR_OK;
}

keymaster_error_t DeleteCertChain(AttestationKeySlot key_slot) {
    UniquePtr<char[]> cert_file(new char[kStorageIdLengthMax]);
    uint32_t cert_chain_length;

    if (ReadCertChainLength(key_slot, &cert_chain_length) != KM_ERROR_OK) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    for (size_t i = 0; i < cert_chain_length; ++i) {
        snprintf(cert_file.get(), kStorageIdLengthMax, "%s.%s.%d",
                 kAttestCertPrefix, GetKeySlotStr(key_slot), i);
        if (!SecureStorageDeleteFile(cert_file.get())) {
            return KM_ERROR_UNKNOWN_ERROR;
        }
    }
    if (WriteCertChainLength(key_slot, 0) != KM_ERROR_OK) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    return KM_ERROR_OK;
}

keymaster_error_t SetProductId(const uint8_t product_id[kProductIdSize]) {
    uint64_t product_id_size;
    if (!SecureStorageGetFileSize(kProductIdFileName, &product_id_size)) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
#ifndef KEYMASTER_DEBUG
    if (product_id_size != 0) {
        LOG_E("Error: Product ID already set!\n", 0);
        return KM_ERROR_INVALID_ARGUMENT;
    }
#endif /* KEYMASTER_DEBUG */
    if (!SecureStorageWrite(kProductIdFileName, product_id, kProductIdSize)) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    return KM_ERROR_OK;
}

keymaster_error_t ReadProductId(uint8_t product_id[kProductIdSize]) {
    uint64_t product_id_size;
    if (!SecureStorageGetFileSize(kProductIdFileName, &product_id_size) ||
        product_id_size != kProductIdSize) {
        return KM_ERROR_UNKNOWN_ERROR;
    }

    if (!SecureStorageRead(kProductIdFileName, product_id, kProductIdSize)) {
        return KM_ERROR_UNKNOWN_ERROR;
    }

    return KM_ERROR_OK;
}

keymaster_error_t DeleteProductId() {
    if (!SecureStorageDeleteFile(kProductIdFileName)) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    return KM_ERROR_OK;
}

static keymaster_error_t DeleteAttestationData(AttestationKeySlot key_slot) {
    if (DeleteKey(key_slot) != KM_ERROR_OK ||
        DeleteCertChain(key_slot) != KM_ERROR_OK) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    return KM_ERROR_OK;
}

keymaster_error_t DeleteAllAttestationData() {
    if (DeleteAttestationData(AttestationKeySlot::kRsa) != KM_ERROR_OK ||
        DeleteAttestationData(AttestationKeySlot::kEcdsa) != KM_ERROR_OK ||
        DeleteAttestationData(AttestationKeySlot::kEddsa) != KM_ERROR_OK ||
        DeleteAttestationData(AttestationKeySlot::kEpid) != KM_ERROR_OK ||
        DeleteAttestationData(AttestationKeySlot::kClaimable0) != KM_ERROR_OK ||
        DeleteAttestationData(AttestationKeySlot::kSomRsa) != KM_ERROR_OK ||
        DeleteAttestationData(AttestationKeySlot::kSomEcdsa) != KM_ERROR_OK ||
        DeleteAttestationData(AttestationKeySlot::kSomEddsa) != KM_ERROR_OK ||
        DeleteAttestationData(AttestationKeySlot::kSomEpid) != KM_ERROR_OK) {
        return KM_ERROR_UNKNOWN_ERROR;
    }
    return KM_ERROR_OK;
}

}  // namespace keymaster
