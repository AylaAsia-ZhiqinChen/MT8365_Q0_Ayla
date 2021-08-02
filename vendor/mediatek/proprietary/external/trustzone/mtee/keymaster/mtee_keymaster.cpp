/*
 * Copyright 2014 The Android Open Source Project
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
#include <string.h>
#include <cstddef>
#include "mtee_keymaster.h"
#include "keymaster_mtee_api.h"

namespace keymaster {

const uint8_t MAJOR_VER = 1;
const uint8_t MINOR_VER = 1;
const uint8_t SUBMINOR_VER = 0;

keymaster_error_t MTEEKeymaster::SetSystemVersion(uint32_t os_version, uint32_t os_patchlevel) {

    keymaster_error_t ret;
    size_t length = sizeof(uint32_t) * 2;
    uint8_t *buffer = (uint8_t*)malloc(length);
    if (buffer == NULL)
    {
        ALOGE("SetSystemVersion failed");
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    memset(buffer, 0, length);
    memcpy(buffer, &os_version, sizeof(uint32_t));
    memcpy(buffer + sizeof(uint32_t), &os_patchlevel, sizeof(uint32_t));

    ret = km_customized_api_call(TZCMD_KEYMASTER_SET_SYSTEM_VERSION, buffer, length);

    if (buffer != NULL)
        free(buffer);

    return ret;
}

keymaster_error_t MTEEKeymaster::ParseAttestationKeybox(unsigned char *enckb, unsigned int inlength) {

    keymaster_error_t ret;
    size_t length = (size_t)inlength;

    ret = km_customized_api_call(TZCMD_KEYMASTER_KEY_ATTESTATION_INITIALIZE, (uint8_t*)enckb, length);

    return ret;
}

void MTEEKeymaster::AddRngEntropy(const AddEntropyRequest& request,
                                  AddEntropyResponse* response) {

    km_mtee_request(TZCMD_KEYMASTER_ADD_RNG_ENTROPY, request, response);
    ALOGI("AddRngEntropy:%d", response->error);
}

void MTEEKeymaster::GenerateKey(const GenerateKeyRequest& request,
                                GenerateKeyResponse* response) {

    km_mtee_request(TZCMD_KEYMASTER_GEN_KEY, request, response);
    ALOGI("GenerateKey:%d", response->error);
}

void MTEEKeymaster::GetKeyCharacteristics(const GetKeyCharacteristicsRequest& request,
                                          GetKeyCharacteristicsResponse* response) {

    km_mtee_request(TZCMD_KEYMASTER_GET_KEY_CHARACTERISTICS, request, response);
    ALOGI("GetKeyCharacteristics:%d", response->error);
}

void MTEEKeymaster::BeginOperation(const BeginOperationRequest& request,
                                   BeginOperationResponse* response) {

    km_mtee_request(TZCMD_KEYMASTER_BEGIN, request, response);
    ALOGI("BeginOperation:%d", response->error);
}

void MTEEKeymaster::UpdateOperation(const UpdateOperationRequest& request,
                                    UpdateOperationResponse* response) {

    km_mtee_request(TZCMD_KEYMASTER_UPDATE, request, response);
    ALOGI("UpdateOperation:%d", response->error);
}

void MTEEKeymaster::FinishOperation(const FinishOperationRequest& request,
                                    FinishOperationResponse* response) {

    km_mtee_request(TZCMD_KEYMASTER_FINISH, request, response);
    ALOGI("FinishOperation:%d", response->error);
}

void MTEEKeymaster::AbortOperation(const AbortOperationRequest& request,
                                   AbortOperationResponse* response) {

    km_mtee_request(TZCMD_KEYMASTER_ABORT, request, response);
    ALOGI("AbortOperation:%d", response->error);
}

void MTEEKeymaster::ExportKey(const ExportKeyRequest& request, ExportKeyResponse* response) {

    km_mtee_request(TZCMD_KEYMASTER_EXPORT_KEY, request, response);
    ALOGI("ExportKey:%d", response->error);
}

void MTEEKeymaster::AttestKey(const AttestKeyRequest& request, AttestKeyResponse* response) {

    km_mtee_request(TZCMD_KEYMASTER_ATTEST_KEY, request, response);
    ALOGI("AttestKey:%d", response->error);
}

void MTEEKeymaster::UpgradeKey(const UpgradeKeyRequest& request, UpgradeKeyResponse* response) {

    km_mtee_request(TZCMD_KEYMASTER_UPGRADE_KEY, request, response);
    ALOGI("UpgradeKey:%d", response->error);
}

void MTEEKeymaster::ImportKey(const ImportKeyRequest& request, ImportKeyResponse* response) {

    km_mtee_request(TZCMD_KEYMASTER_IMPORT_KEY, request, response);
    ALOGD("ImportKey:%d", response->error);
}

keymaster_error_t MTEEKeymaster::DeleteKey(const keymaster_key_blob_t* key_blob) {
    //km_mtee_request(TZCMD_KEYMASTER_DELETE_KEY, request, response);
    //response->error = KM_ERROR_OK;
    //ALOGI("DeleteKey:%d", response->error);
    return KM_ERROR_UNIMPLEMENTED;
}

keymaster_error_t MTEEKeymaster::DeleteAllKeys() {
    //km_mtee_request(TZCMD_KEYMASTER_DELETE_ALL_KEYS, request, response);
    //response->error = KM_ERROR_OK;
    //ALOGI("DeleteAllKeys:%d", response->error);
    return KM_ERROR_UNIMPLEMENTED;
}
}  // namespace keymaster
