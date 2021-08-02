/*
 * Copyright 2015 The Android Open Source Project
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <utils/Timers.h>
#include <stddef.h>

#include <dlfcn.h>
#include <tz_cross/keyblock.h>

#include <algorithm>
#include <type_traits>
#include <openssl/x509.h>
#include <hardware/keymaster2.h>
#include <keymaster/android_keymaster_messages.h>
#include <keymaster/authorization_set.h>
#include "MTKKeymaster_hal.h"
#include "mtee_keymaster.h"
#include "keymaster_mtee_api.h"

using namespace keymaster;

const size_t kMaximumAttestationChallengeLength = 128;

static const char *libpathkm = "liburee_meta_drmkeyinstall_v2.so";
static void *gKM_Handle = NULL;

typedef int (*PF_get_encrypted_attestation_key)(unsigned int , unsigned char **, unsigned int *);
typedef int (*PF_free_encrypted_attestation_key)(unsigned char *);

static PF_get_encrypted_attestation_key pf_get_encrypted_attestation_key = NULL;
static PF_free_encrypted_attestation_key pf_free_encrypted_attestation_key = NULL;

int init_Km_Api()
{
    if (gKM_Handle == NULL)
    {
        gKM_Handle = dlopen(libpathkm, RTLD_NOW);
        ALOGD("init_Km_Api gKM_Handle = %p", gKM_Handle);
    }
    else
    {
        return 0;
    }
    if (gKM_Handle == NULL)
    {
        ALOGE("init_Km_Api [ERROR]- cannot open %s", gKM_Handle);
        return -1;
    }

    pf_get_encrypted_attestation_key = (PF_get_encrypted_attestation_key)dlsym(gKM_Handle, "get_encrypt_drmkey");
    pf_free_encrypted_attestation_key = (PF_free_encrypted_attestation_key)dlsym(gKM_Handle, "free_encrypt_drmkey");

    return 0;
}

/**
 * Load the encrypted attestation keybox from FS by keymanager.
 */
static int loadEncryptedAttestationKeybox(unsigned char **enc_attestation_keybox, unsigned int *length)
{

    int ret = init_Km_Api();
    if (ret != 0)
    {
        ALOGE("init KM fail");
        return ret;
    }

    ret = pf_get_encrypted_attestation_key(KEYMASTER_ATTEST_ID, enc_attestation_keybox, length);
    if (ret != 0)
    {
        ALOGE("get encrypted attestation key fail : %x", ret);
        return -1;
    }

    if(*enc_attestation_keybox == NULL)
    {
        ALOGE("get encrypted attestation key failed");
        return -1;
    }

    return ret;
}

template <typename RequestType>
static void AddClientAndAppData(const keymaster_blob_t* client_id,
                                const keymaster_blob_t* app_data,
                                RequestType* request) {

    request->additional_params.Clear();
    if (client_id && client_id->data_length != 0)
        request->additional_params.push_back(TAG_APPLICATION_ID, *client_id);
    if (app_data && app_data->data_length != 0)
        request->additional_params.push_back(TAG_APPLICATION_DATA, *app_data);
}

static inline MTKKeymasterHal* convert_device(const keymaster2_device_t* dev) {
    return reinterpret_cast<MTKKeymasterHal*>(const_cast<keymaster2_device_t*>(dev));
}

MTKKeymasterHal::MTKKeymasterHal()
    : impl_(new MTEEKeymaster()), configured_(false) {
}

keymaster2_device_t* MTKKeymasterHal::keymaster_device() {
    return &device_;
}

int MTKKeymasterHal::close_device(hw_device_t* dev) {
    delete reinterpret_cast<MTKKeymasterHal*>(dev);
    return 0;
}

keymaster_error_t MTKKeymasterHal::import_attest_keybox(const keymaster2_device* dev) {

    unsigned char* enckb = NULL;
    unsigned int inlength = 0;
    unsigned char* encrypted_keybox = NULL;

    int ret = loadEncryptedAttestationKeybox(&enckb, &inlength);
    if (ret != 0) {
        ALOGE("Import attest keybox fail");
        return KM_ERROR_UNKNOWN_ERROR;
    }

    if (enckb == NULL || inlength == 0)
    {
        ALOGE("The value of enckb: %p, inlength: %u", enckb, inlength);
        ALOGE("The return value of enckb or inlength is not valid");
        return KM_ERROR_UNKNOWN_ERROR;
    }

    if(enckb != NULL && inlength != 0)
    {
        encrypted_keybox = (unsigned char*)malloc((size_t)inlength);
        memcpy(encrypted_keybox, enckb, (size_t)inlength);
        pf_free_encrypted_attestation_key(enckb);
    }

    keymaster_error_t error = convert_device(dev)->impl_->ParseAttestationKeybox(encrypted_keybox, inlength);
    return error;
}

keymaster_error_t MTKKeymasterHal::configure(const keymaster2_device_t* dev, const keymaster_key_param_set_t* params) {

    AuthorizationSet params_copy(*params);
    uint32_t os_version;
    uint32_t os_patchlevel;
    if (!params_copy.GetTagValue(TAG_OS_VERSION, &os_version) ||
        !params_copy.GetTagValue(TAG_OS_PATCHLEVEL, &os_patchlevel)) {
        ALOGE("Configuration paramters must contain OS version and patch level");
        return KM_ERROR_INVALID_ARGUMENT;
    }

    keymaster_error_t error = convert_device(dev)->impl_->SetSystemVersion(os_version, os_patchlevel);
    if (error == KM_ERROR_OK)
    {
        convert_device(dev)->configured_ = true;
    }
    return error;
}

keymaster_error_t MTKKeymasterHal::add_rng_entropy(const keymaster2_device_t* dev,
                                                   const uint8_t* data, size_t data_length) {
    if (!dev)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!convert_device(dev)->configured())
        return KM_ERROR_KEYMASTER_NOT_CONFIGURED;

    AddEntropyRequest request;
    request.random_data.Reinitialize(data, data_length);
    AddEntropyResponse response;
    convert_device(dev)->impl_->AddRngEntropy(request, &response);
    if (response.error != KM_ERROR_OK)
        ALOGE("add_rng_entropy failed with %d", response.error);
    return response.error;
}

keymaster_error_t MTKKeymasterHal::generate_key(const keymaster2_device_t* dev,
                                                const keymaster_key_param_set_t* params,
                                                keymaster_key_blob_t* key_blob,
                                                keymaster_key_characteristics_t* characteristics) {
    if (!dev || !params)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!convert_device(dev)->configured())
    {
        ALOGE("The keymaster2 device is not configured.");
        return KM_ERROR_KEYMASTER_NOT_CONFIGURED;
    }

    if (!key_blob)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    GenerateKeyRequest request;
    request.key_description.Reinitialize(*params);
    request.key_description.push_back(TAG_CREATION_DATETIME, java_time(time(NULL)));

    GenerateKeyResponse response;
    convert_device(dev)->impl_->GenerateKey(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    key_blob->key_material_size = response.key_blob.key_material_size;
    uint8_t* tmp = reinterpret_cast<uint8_t*>(malloc(key_blob->key_material_size));
    if (!tmp)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    memcpy(tmp, response.key_blob.key_material, response.key_blob.key_material_size);
    key_blob->key_material = tmp;

    if (characteristics) {
        response.enforced.CopyToParamSet(&characteristics->hw_enforced);
        response.unenforced.CopyToParamSet(&characteristics->sw_enforced);
    }

    return KM_ERROR_OK;
}

keymaster_error_t MTKKeymasterHal::get_key_characteristics(const keymaster2_device_t* dev,
                                                           const keymaster_key_blob_t* key_blob,
                                                           const keymaster_blob_t* client_id,
                                                           const keymaster_blob_t* app_data,
                                                           keymaster_key_characteristics_t* characteristics) {
    if (!dev || !key_blob || !key_blob->key_material)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!convert_device(dev)->configured())
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    if (!characteristics)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    GetKeyCharacteristicsRequest request;
    request.SetKeyMaterial(*key_blob);
    AddClientAndAppData(client_id, app_data, &request);

    GetKeyCharacteristicsResponse response;
    convert_device(dev)->impl_->GetKeyCharacteristics(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    if (characteristics) {
        response.enforced.CopyToParamSet(&characteristics->hw_enforced);
        response.unenforced.CopyToParamSet(&characteristics->sw_enforced);
    }

    return KM_ERROR_OK;
}

keymaster_error_t MTKKeymasterHal::import_key(const keymaster2_device_t* dev,
                                              const keymaster_key_param_set_t* params,
                                              keymaster_key_format_t key_format,
                                              const keymaster_blob_t* key_data,
                                              keymaster_key_blob_t* key_blob,
                                              keymaster_key_characteristics_t* characteristics) {
    if (!params || !key_data)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!key_blob)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    if (!convert_device(dev)->configured())
        return KM_ERROR_KEYMASTER_NOT_CONFIGURED;

    ImportKeyRequest request;
    request.key_description.Reinitialize(*params);
    request.key_description.push_back(TAG_CREATION_DATETIME, java_time(time(NULL)));

    request.key_format = key_format;
    request.SetKeyMaterial(key_data->data, key_data->data_length);

    ImportKeyResponse response;
    convert_device(dev)->impl_->ImportKey(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    key_blob->key_material_size = response.key_blob.key_material_size;
    key_blob->key_material = reinterpret_cast<uint8_t*>(malloc(key_blob->key_material_size));
    if (!key_blob->key_material)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    memcpy(const_cast<uint8_t*>(key_blob->key_material), response.key_blob.key_material,
           response.key_blob.key_material_size);

    if (characteristics) {
        response.enforced.CopyToParamSet(&characteristics->hw_enforced);
        response.unenforced.CopyToParamSet(&characteristics->sw_enforced);
    }

    return KM_ERROR_OK;
}

keymaster_error_t MTKKeymasterHal::export_key(const keymaster2_device_t* dev,
                                              keymaster_key_format_t export_format,
                                              const keymaster_key_blob_t* key_to_export,
                                              const keymaster_blob_t* client_id,
                                              const keymaster_blob_t* app_data,
                                              keymaster_blob_t* export_data) {

    if (!key_to_export || !key_to_export->key_material)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!export_data)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    if (!convert_device(dev)->configured())
        return KM_ERROR_KEYMASTER_NOT_CONFIGURED;

    export_data->data = nullptr;
    export_data->data_length = 0;

    ExportKeyRequest request;
    request.key_format = export_format;
    request.SetKeyMaterial(*key_to_export);
    AddClientAndAppData(client_id, app_data, &request);

    ExportKeyResponse response;
    convert_device(dev)->impl_->ExportKey(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    export_data->data_length = response.key_data_length;
    uint8_t* tmp = reinterpret_cast<uint8_t*>(malloc(export_data->data_length));
    if (!tmp)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    memcpy(tmp, response.key_data, export_data->data_length);
    export_data->data = tmp;
    return KM_ERROR_OK;
}

keymaster_error_t MTKKeymasterHal::attest_key(const keymaster2_device_t* dev,
                                              const keymaster_key_blob_t* key_to_attest,
                                              const keymaster_key_param_set_t* attest_params,
                                              keymaster_cert_chain_t* cert_chain) {

    if (!dev || !key_to_attest || !attest_params || !cert_chain)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!convert_device(dev)->configured())
        return KM_ERROR_KEYMASTER_NOT_CONFIGURED;

    keymaster_error_t error = import_attest_keybox(dev);
    if (error != KM_ERROR_OK)
    {
        ALOGE("Key attestation init failed");
        return error;
    }

    cert_chain->entry_count = 0;
    cert_chain->entries = nullptr;

    AttestKeyRequest request;
    request.SetKeyMaterial(*key_to_attest);
    request.attest_params.Reinitialize(*attest_params);

    keymaster_blob_t attestation_challenge = {};
    request.attest_params.GetTagValue(TAG_ATTESTATION_CHALLENGE, &attestation_challenge);
    if (attestation_challenge.data_length > kMaximumAttestationChallengeLength) {
        ALOGE("%zu-byte attestation challenge; only %zu bytes allowed", attestation_challenge.data_length, kMaximumAttestationChallengeLength);
        return KM_ERROR_INVALID_INPUT_LENGTH;
    }

    AttestKeyResponse response;
    convert_device(dev)->impl_->AttestKey(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    // Allocate and clear storage for cert_chain.
    keymaster_cert_chain_t& rsp_chain = response.certificate_chain;
    cert_chain->entries = reinterpret_cast<keymaster_blob_t*>(malloc(rsp_chain.entry_count * sizeof(*cert_chain->entries)));
    if (!cert_chain->entries)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;

    cert_chain->entry_count = rsp_chain.entry_count;
    for (keymaster_blob_t& entry : array_range(cert_chain->entries, cert_chain->entry_count))
        entry = {};

    // Copy cert_chain contents
    size_t i = 0;
    for (keymaster_blob_t& entry : array_range(rsp_chain.entries, rsp_chain.entry_count)) {
        cert_chain->entries[i].data = reinterpret_cast<uint8_t*>(malloc(entry.data_length));
        if (!cert_chain->entries[i].data) {
            keymaster_free_cert_chain(cert_chain);
            return KM_ERROR_MEMORY_ALLOCATION_FAILED;
        }
        cert_chain->entries[i].data_length = entry.data_length;
        memcpy(const_cast<uint8_t*>(cert_chain->entries[i].data), entry.data, entry.data_length);
        ++i;
    }

    return KM_ERROR_OK;
}

keymaster_error_t MTKKeymasterHal::upgrade_key(const keymaster2_device_t* dev,
                                               const keymaster_key_blob_t* key_to_upgrade,
                                               const keymaster_key_param_set_t* upgrade_params,
                                               keymaster_key_blob_t* upgraded_key) {

    if (!dev || !key_to_upgrade || !upgrade_params)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!upgraded_key)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    if (!convert_device(dev)->configured())
        return KM_ERROR_KEYMASTER_NOT_CONFIGURED;

    UpgradeKeyRequest request;
    request.SetKeyMaterial(*key_to_upgrade);
    request.upgrade_params.Reinitialize(*upgrade_params);

    UpgradeKeyResponse response;
    convert_device(dev)->impl_->UpgradeKey(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    upgraded_key->key_material_size = response.upgraded_key.key_material_size;
    uint8_t* tmp = reinterpret_cast<uint8_t*>(malloc(upgraded_key->key_material_size));
    if (!tmp)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;

    memcpy(tmp, response.upgraded_key.key_material, response.upgraded_key.key_material_size);
    upgraded_key->key_material = tmp;

    return KM_ERROR_OK;
}

keymaster_error_t MTKKeymasterHal::delete_key(const keymaster2_device_t* dev,
                                              const keymaster_key_blob_t* key_blob) {
    if (dev == NULL || key_blob == NULL) {
        return KM_ERROR_UNEXPECTED_NULL_POINTER;
    }

    return convert_device(dev)->impl_->DeleteKey(key_blob);
}

keymaster_error_t MTKKeymasterHal::delete_all_keys(const keymaster2_device_t* dev) {
    if (dev == NULL) {
        return KM_ERROR_UNEXPECTED_NULL_POINTER;
    }

    return convert_device(dev)->impl_->DeleteAllKeys();
}

keymaster_error_t MTKKeymasterHal::begin(const keymaster2_device_t* dev,
                                         keymaster_purpose_t purpose,
                                         const keymaster_key_blob_t* key,
                                         const keymaster_key_param_set_t* in_params,
                                         keymaster_key_param_set_t* out_params,
                                         keymaster_operation_handle_t* operation_handle) {

    if (!key || !key->key_material)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!convert_device(dev)->configured())
        return KM_ERROR_KEYMASTER_NOT_CONFIGURED;

    if (!operation_handle)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    if(out_params) {
        out_params->params = nullptr;
        out_params->length = 0;
    }

    BeginOperationRequest request;
    request.purpose = purpose;
    request.SetKeyMaterial(*key);
    request.additional_params.Reinitialize(*in_params);

    BeginOperationResponse response;
    convert_device(dev)->impl_->BeginOperation(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    if (response.output_params.size() > 0 && out_params)
        response.output_params.CopyToParamSet(out_params);

    *operation_handle = response.op_handle;
    return KM_ERROR_OK;
}

keymaster_error_t MTKKeymasterHal::update(const keymaster2_device_t* dev,
                                          keymaster_operation_handle_t operation_handle,
                                          const keymaster_key_param_set_t* in_params,
                                          const keymaster_blob_t* input, size_t* input_consumed,
                                          keymaster_key_param_set_t* out_params,
                                          keymaster_blob_t* output) {

    if (!input)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!input_consumed)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    if (!convert_device(dev)->configured())
        return KM_ERROR_KEYMASTER_NOT_CONFIGURED;

    if (out_params) {
        out_params->params = nullptr;
        out_params->length = 0;
    }

    if (output) {
        output->data = nullptr;
        output->data_length = 0;
    }

    UpdateOperationRequest request;
    request.op_handle = operation_handle;
    if (input)
        request.input.Reinitialize(input->data, input->data_length);
    if (in_params)
        request.additional_params.Reinitialize(*in_params);

    UpdateOperationResponse response;
    convert_device(dev)->impl_->UpdateOperation(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    if (response.output_params.size() > 0 && out_params)
        response.output_params.CopyToParamSet(out_params);

    *input_consumed = response.input_consumed;

    if (output) {
        output->data_length = response.output.available_read();
        uint8_t* tmp = reinterpret_cast<uint8_t*>(malloc(output->data_length));
        if (!tmp)
            return KM_ERROR_MEMORY_ALLOCATION_FAILED;
        memcpy(tmp, response.output.peek_read(), output->data_length);
        output->data = tmp;
    }

    return KM_ERROR_OK;
}

keymaster_error_t MTKKeymasterHal::finish(const keymaster2_device_t* dev,
                                          keymaster_operation_handle_t operation_handle,
                                          const keymaster_key_param_set_t* params,
                                          const keymaster_blob_t* input,
                                          const keymaster_blob_t* signature,
                                          keymaster_key_param_set_t* out_params,
                                          keymaster_blob_t* output) {

    if (!dev)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!convert_device(dev)->configured())
        return KM_ERROR_KEYMASTER_NOT_CONFIGURED;

    if (out_params) {
        out_params->params = nullptr;
        out_params->length = 0;
    }

    if (output) {
        output->data = nullptr;
        output->data_length = 0;
    }

    FinishOperationRequest request;
    request.op_handle = operation_handle;
    if (signature && signature->data_length > 0)
        request.signature.Reinitialize(signature->data, signature->data_length);

    if (input && input->data_length > 0)
    request.input.Reinitialize(input->data, input->data_length);

    if (params)
        request.additional_params.Reinitialize(*params);

    FinishOperationResponse response;
    convert_device(dev)->impl_->FinishOperation(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    if (response.output_params.size() > 0) {
    if (out_params)
            response.output_params.CopyToParamSet(out_params);
    else
        return KM_ERROR_OUTPUT_PARAMETER_NULL;
    }

    if(output) {
        output->data_length = response.output.available_read();
        uint8_t* tmp = reinterpret_cast<uint8_t*>(malloc(output->data_length));
        if (!tmp)
            return KM_ERROR_MEMORY_ALLOCATION_FAILED;
        memcpy(tmp, response.output.peek_read(), output->data_length);
        output->data = tmp;
    }

    return KM_ERROR_OK;
}

keymaster_error_t MTKKeymasterHal::abort(const keymaster2_device_t* dev,
                                         keymaster_operation_handle_t operation_handle) {

    if (!dev)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!convert_device(dev)->configured())
        return KM_ERROR_KEYMASTER_NOT_CONFIGURED;

    AbortOperationRequest request;
    request.op_handle = operation_handle;
    AbortOperationResponse response;
    convert_device(dev)->impl_->AbortOperation(request, &response);
    return response.error;
}

int MTKKeymasterHal::open_keymaster_hal(const struct hw_module_t* hw_module,
                                        const char* name,
                                        struct hw_device_t** hw_device_out) {
    ALOGI("open keymaster device");

    if (memcmp(name, KEYSTORE_KEYMASTER, strlen(KEYSTORE_KEYMASTER)) != 0)
        return -EINVAL;

    MTKKeymasterHal *inst = new MTKKeymasterHal();
    keymaster2_device_t *keymaster_device = inst->keymaster_device();

    memset(keymaster_device, 0, sizeof(keymaster2_device_t));

    keymaster_device->common.tag = HARDWARE_DEVICE_TAG;
    keymaster_device->common.version = 1;
    keymaster_device->common.module = const_cast<hw_module_t*>(hw_module);
    keymaster_device->common.close = &close_device;

    keymaster_device->flags = KEYMASTER_BLOBS_ARE_STANDALONE | KEYMASTER_SUPPORTS_EC;

    keymaster_device->configure = configure;
    keymaster_device->add_rng_entropy = add_rng_entropy;
    keymaster_device->generate_key = generate_key;
    keymaster_device->get_key_characteristics = get_key_characteristics;
    keymaster_device->import_key = import_key;
    keymaster_device->export_key = export_key;
    keymaster_device->attest_key = attest_key;
    keymaster_device->upgrade_key = upgrade_key;
    keymaster_device->delete_key = delete_key;
    keymaster_device->delete_all_keys = delete_all_keys;
    keymaster_device->begin = begin;
    keymaster_device->update = update;
    keymaster_device->finish = finish;
    keymaster_device->abort = abort;

    keymaster_device->context = NULL;

    *hw_device_out = &keymaster_device->common;

    ALOGI("open keymaster device.. done");
    return 0;
}

static struct hw_module_methods_t keymaster_hal_methods = {
    .open = MTKKeymasterHal::open_keymaster_hal
};

__attribute__((visibility("default")))struct keystore_module HAL_MODULE_INFO_SYM = {
    .common =
    {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = KEYMASTER_MODULE_API_VERSION_2_0,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = KEYSTORE_HARDWARE_MODULE_ID,
        .name = "Keymaster MTEE based HAL",
        .author = "Mediatek",
        .methods = &keymaster_hal_methods,
        .dso = 0,
        .reserved = {},
    }
};
