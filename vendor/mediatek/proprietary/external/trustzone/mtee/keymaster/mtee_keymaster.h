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

#ifndef SYSTEM_KEYMASTER_MTEE_KEYMASTER_H_
#define SYSTEM_KEYMASTER_MTEE_KEYMASTER_H_

#include <keymaster/android_keymaster_messages.h>
#include <keymaster/authorization_set.h>

namespace keymaster {

//class Key;
//class KeyFactory;
//class OperationTable;

class MTEEKeymaster {
  public:
    MTEEKeymaster() {};
    virtual ~MTEEKeymaster() {};

    // Add for configure
    keymaster_error_t SetSystemVersion(uint32_t os_version, uint32_t os_patchlevel);
    keymaster_error_t ParseAttestationKeybox(unsigned char* enckb, unsigned int inlength);

    void AddRngEntropy(const AddEntropyRequest& request, AddEntropyResponse* response);
    void GenerateKey(const GenerateKeyRequest& request, GenerateKeyResponse* response);
    void GetKeyCharacteristics(const GetKeyCharacteristicsRequest& request,
                               GetKeyCharacteristicsResponse* response);
    void ImportKey(const ImportKeyRequest& request, ImportKeyResponse* response);
    void ExportKey(const ExportKeyRequest& request, ExportKeyResponse* response);
    void AttestKey(const AttestKeyRequest& request, AttestKeyResponse* response);
    void UpgradeKey(const UpgradeKeyRequest& request, UpgradeKeyResponse* response);
    keymaster_error_t DeleteKey(const keymaster_key_blob_t* key_blob);
    keymaster_error_t DeleteAllKeys();
    void BeginOperation(const BeginOperationRequest& request, BeginOperationResponse* response);
    void UpdateOperation(const UpdateOperationRequest& request, UpdateOperationResponse* response);
    void FinishOperation(const FinishOperationRequest& request, FinishOperationResponse* response);
    void AbortOperation(const AbortOperationRequest& request, AbortOperationResponse* response);
    void GetVersion(const GetVersionRequest& request, GetVersionResponse* response);
};

}  // namespace keymaster

#endif  //  SYSTEM_KEYMASTER_MTEE_KEYMASTER_H_
