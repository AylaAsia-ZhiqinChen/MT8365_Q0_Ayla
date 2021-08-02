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

#include "trusty_keymaster_context.h"
#include "secure_storage.h"

#include <lib/hwkey/hwkey.h>
#include <lib/rng/trusty_rng.h>

#include <keymaster/android_keymaster_utils.h>
#include <keymaster/km_openssl/ec_key_factory.h>
#include <keymaster/km_openssl/rsa_key_factory.h>
#include <keymaster/logger.h>

#include <keymaster/key_blob_utils/auth_encrypted_key_blob.h>
#include <keymaster/key_blob_utils/ocb_utils.h>
#include <keymaster/km_openssl/aes_key.h>
#include <keymaster/km_openssl/asymmetric_key.h>
#include <keymaster/km_openssl/attestation_utils.h>
#include <keymaster/km_openssl/hmac_key.h>
#include <keymaster/km_openssl/openssl_err.h>
#include "test_attestation_keys.h"

#ifdef KEYMASTER_DEBUG
#warning "Compiling with fake Keymaster Root of Trust values! DO NOT SHIP THIS!"
#endif

namespace keymaster {

namespace {
static const int kAesKeySize = 16;
static const int kCallsBetweenRngReseeds = 32;
static const int kRngReseedSize = 64;
static const uint8_t kMasterKeyDerivationData[kAesKeySize] = "KeymasterMaster";

bool UpgradeIntegerTag(keymaster_tag_t tag,
                       uint32_t value,
                       AuthorizationSet* set,
                       bool* set_changed) {
    int index = set->find(tag);
    if (index == -1) {
        keymaster_key_param_t param;
        param.tag = tag;
        param.integer = value;
        set->push_back(param);
        *set_changed = true;
        return true;
    }

    if (set->params[index].integer > value) {
        return false;
    }

    if (set->params[index].integer != value) {
        set->params[index].integer = value;
        *set_changed = true;
    }
    return true;
}

}  // anonymous namespace

TrustyKeymasterContext::TrustyKeymasterContext()
        : enforcement_policy_(this),
          rng_initialized_(false),
          calls_since_reseed_(0) {
    LOG_D("Creating TrustyKeymaster", 0);
    rsa_factory_.reset(new RsaKeyFactory(this));
    ec_factory_.reset(new EcKeyFactory(this));
    aes_factory_.reset(new AesKeyFactory(this, this));
    hmac_factory_.reset(new HmacKeyFactory(this, this));
    verified_boot_key_.Reinitialize("Unbound", 7);
}

const KeyFactory* TrustyKeymasterContext::GetKeyFactory(
        keymaster_algorithm_t algorithm) const {
    switch (algorithm) {
    case KM_ALGORITHM_RSA:
        return rsa_factory_.get();
    case KM_ALGORITHM_EC:
        return ec_factory_.get();
    case KM_ALGORITHM_AES:
        return aes_factory_.get();
    case KM_ALGORITHM_HMAC:
        return hmac_factory_.get();
    default:
        return nullptr;
    }
}

static keymaster_algorithm_t supported_algorithms[] = {
        KM_ALGORITHM_RSA, KM_ALGORITHM_EC, KM_ALGORITHM_AES, KM_ALGORITHM_HMAC};

const keymaster_algorithm_t* TrustyKeymasterContext::GetSupportedAlgorithms(
        size_t* algorithms_count) const {
    *algorithms_count = array_length(supported_algorithms);
    return supported_algorithms;
}

const OperationFactory* TrustyKeymasterContext::GetOperationFactory(
        keymaster_algorithm_t algorithm,
        keymaster_purpose_t purpose) const {
    const KeyFactory* key_factory = GetKeyFactory(algorithm);
    if (!key_factory)
        return nullptr;
    return key_factory->GetOperationFactory(purpose);
}

static keymaster_error_t TranslateAuthorizationSetError(
        AuthorizationSet::Error err) {
    switch (err) {
    case AuthorizationSet::OK:
        return KM_ERROR_OK;
    case AuthorizationSet::ALLOCATION_FAILURE:
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    case AuthorizationSet::MALFORMED_DATA:
        return KM_ERROR_UNKNOWN_ERROR;
    }
    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterContext::SetAuthorizations(
        const AuthorizationSet& key_description,
        keymaster_key_origin_t origin,
        AuthorizationSet* hw_enforced,
        AuthorizationSet* sw_enforced) const {
    sw_enforced->Clear();
    hw_enforced->Clear();

    for (auto& entry : key_description) {
        switch (entry.tag) {
        case KM_TAG_INVALID:
        case KM_TAG_BOOTLOADER_ONLY:
        case KM_TAG_NONCE:
        case KM_TAG_AUTH_TOKEN:
        case KM_TAG_MAC_LENGTH:
        case KM_TAG_ASSOCIATED_DATA:
        case KM_TAG_UNIQUE_ID:
            return KM_ERROR_INVALID_KEY_BLOB;

        case KM_TAG_ROLLBACK_RESISTANT:
        case KM_TAG_APPLICATION_ID:
        case KM_TAG_APPLICATION_DATA:
        case KM_TAG_ALL_APPLICATIONS:
        case KM_TAG_ROOT_OF_TRUST:
        case KM_TAG_ORIGIN:
        case KM_TAG_RESET_SINCE_ID_ROTATION:
        case KM_TAG_ALLOW_WHILE_ON_BODY:
        case KM_TAG_ATTESTATION_CHALLENGE:
        case KM_TAG_OS_VERSION:
        case KM_TAG_OS_PATCHLEVEL:
            // Ignore these.
            break;

        case KM_TAG_PURPOSE:
        case KM_TAG_ALGORITHM:
        case KM_TAG_KEY_SIZE:
        case KM_TAG_RSA_PUBLIC_EXPONENT:
        case KM_TAG_BLOB_USAGE_REQUIREMENTS:
        case KM_TAG_DIGEST:
        case KM_TAG_PADDING:
        case KM_TAG_BLOCK_MODE:
        case KM_TAG_MIN_SECONDS_BETWEEN_OPS:
        case KM_TAG_MAX_USES_PER_BOOT:
        case KM_TAG_USER_SECURE_ID:
        case KM_TAG_NO_AUTH_REQUIRED:
        case KM_TAG_AUTH_TIMEOUT:
        case KM_TAG_CALLER_NONCE:
        case KM_TAG_MIN_MAC_LENGTH:
        case KM_TAG_KDF:
        case KM_TAG_EC_CURVE:
        case KM_TAG_ECIES_SINGLE_HASH_MODE:
            hw_enforced->push_back(entry);
            break;

        case KM_TAG_USER_AUTH_TYPE: {
            keymaster_key_param_t elem = entry;

            // This implementation does support TEE enforced password auth
            elem.enumerated = entry.enumerated & HW_AUTH_PASSWORD;

#if TEE_FINGERPRINT_AUTH_SUPPORTED
            // If HW_AUTH_FINGERPRINT is supported it needs to be included too
            elem.enumerates |= entry.enumerated & HW_AUTH_FINGERPRINT;
#endif
            hw_enforced->push_back(elem);
        } break;

        case KM_TAG_ACTIVE_DATETIME:
        case KM_TAG_ORIGINATION_EXPIRE_DATETIME:
        case KM_TAG_USAGE_EXPIRE_DATETIME:
        case KM_TAG_USER_ID:
        case KM_TAG_ALL_USERS:
        case KM_TAG_CREATION_DATETIME:
        case KM_TAG_INCLUDE_UNIQUE_ID:
        case KM_TAG_EXPORTABLE:

            sw_enforced->push_back(entry);
            break;
        default:
            break;
        }
    }

    hw_enforced->push_back(TAG_ORIGIN, origin);
    // these values will be 0 if not set by bootloader
    hw_enforced->push_back(TAG_OS_VERSION, boot_os_version_);
    hw_enforced->push_back(TAG_OS_PATCHLEVEL, boot_os_patchlevel_);

    if (sw_enforced->is_valid() != AuthorizationSet::OK)
        return TranslateAuthorizationSetError(sw_enforced->is_valid());
    if (hw_enforced->is_valid() != AuthorizationSet::OK)
        return TranslateAuthorizationSetError(hw_enforced->is_valid());
    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterContext::BuildHiddenAuthorizations(
        const AuthorizationSet& input_set,
        AuthorizationSet* hidden) const {
    keymaster_blob_t entry;
    if (input_set.GetTagValue(TAG_APPLICATION_ID, &entry))
        hidden->push_back(TAG_APPLICATION_ID, entry.data, entry.data_length);
    if (input_set.GetTagValue(TAG_APPLICATION_DATA, &entry))
        hidden->push_back(TAG_APPLICATION_DATA, entry.data, entry.data_length);

    // Copy verified boot key, verified boot state, and device lock state to
    // hidden authorization set for binding to key.
    keymaster_key_param_t root_of_trust;
    root_of_trust.tag = KM_TAG_ROOT_OF_TRUST;
    root_of_trust.blob.data = verified_boot_key_.begin();
    root_of_trust.blob.data_length = verified_boot_key_.buffer_size();
    hidden->push_back(root_of_trust);

    root_of_trust.blob.data =
            reinterpret_cast<const uint8_t*>(&verified_boot_state_);
    root_of_trust.blob.data_length = sizeof(verified_boot_state_);
    hidden->push_back(root_of_trust);

    root_of_trust.blob.data = reinterpret_cast<const uint8_t*>(&device_locked_);
    root_of_trust.blob.data_length = sizeof(device_locked_);
    hidden->push_back(root_of_trust);

    return TranslateAuthorizationSetError(hidden->is_valid());
}

keymaster_error_t TrustyKeymasterContext::CreateAuthEncryptedKeyBlob(
        const AuthorizationSet& key_description,
        const KeymasterKeyBlob& key_material,
        const AuthorizationSet& hw_enforced,
        const AuthorizationSet& sw_enforced,
        KeymasterKeyBlob* blob) const {
    AuthorizationSet hidden;
    keymaster_error_t error =
            BuildHiddenAuthorizations(key_description, &hidden);
    if (error != KM_ERROR_OK)
        return error;

    KeymasterKeyBlob master_key;
    error = DeriveMasterKey(&master_key);
    if (error != KM_ERROR_OK)
        return error;

    Buffer nonce(OCB_NONCE_LENGTH);
    Buffer tag(OCB_TAG_LENGTH);
    if (!nonce.peek_write() || !tag.peek_write())
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;

    error = GenerateRandom(nonce.peek_write(), OCB_NONCE_LENGTH);
    if (error != KM_ERROR_OK)
        return error;
    nonce.advance_write(OCB_NONCE_LENGTH);

    KeymasterKeyBlob encrypted_key;
    error = OcbEncryptKey(hw_enforced, sw_enforced, hidden, master_key,
                          key_material, nonce, &encrypted_key, &tag);
    if (error != KM_ERROR_OK)
        return error;

    return SerializeAuthEncryptedBlob(encrypted_key, hw_enforced, sw_enforced,
                                      nonce, tag, blob);
}

keymaster_error_t TrustyKeymasterContext::CreateKeyBlob(
        const AuthorizationSet& key_description,
        keymaster_key_origin_t origin,
        const KeymasterKeyBlob& key_material,
        KeymasterKeyBlob* blob,
        AuthorizationSet* hw_enforced,
        AuthorizationSet* sw_enforced) const {
    keymaster_error_t error = SetAuthorizations(key_description, origin,
                                                hw_enforced, sw_enforced);
    if (error != KM_ERROR_OK)
        return error;

    return CreateAuthEncryptedKeyBlob(key_description, key_material,
                                      *hw_enforced, *sw_enforced, blob);
}

keymaster_error_t TrustyKeymasterContext::UpgradeKeyBlob(
        const KeymasterKeyBlob& key_to_upgrade,
        const AuthorizationSet& upgrade_params,
        KeymasterKeyBlob* upgraded_key) const {
    UniquePtr<Key> key;
    keymaster_error_t error =
            ParseKeyBlob(key_to_upgrade, upgrade_params, &key);
    if (error != KM_ERROR_OK)
        return error;

    bool set_changed = false;

    if (boot_os_version_ == 0) {
        // We need to allow "upgrading" OS version to zero, to support upgrading
        // from proper numbered releases to unnumbered development and preview
        // releases.

        int key_os_version_pos = key->sw_enforced().find(TAG_OS_VERSION);
        if (key_os_version_pos != -1) {
            uint32_t key_os_version =
                    key->sw_enforced()[key_os_version_pos].integer;
            if (key_os_version != 0) {
                key->sw_enforced()[key_os_version_pos].integer =
                        boot_os_version_;
                set_changed = true;
            }
        }
    }

    if (!UpgradeIntegerTag(TAG_OS_VERSION, boot_os_version_,
                           &key->hw_enforced(), &set_changed) ||
        !UpgradeIntegerTag(TAG_OS_PATCHLEVEL, boot_os_patchlevel_,
                           &key->hw_enforced(), &set_changed)) {
        // One of the version fields would have been a downgrade. Not allowed.
        return KM_ERROR_INVALID_ARGUMENT;
    }

    if (!set_changed) {
        // Don't need an upgrade.
        return KM_ERROR_OK;
    }

    return CreateAuthEncryptedKeyBlob(upgrade_params, key->key_material(),
                                      key->hw_enforced(), key->sw_enforced(),
                                      upgraded_key);
}

keymaster_error_t TrustyKeymasterContext::ParseKeyBlob(
        const KeymasterKeyBlob& blob,
        const AuthorizationSet& additional_params,
        UniquePtr<Key>* key) const {
    AuthorizationSet hw_enforced;
    AuthorizationSet sw_enforced;
    KeymasterKeyBlob key_material;
    keymaster_error_t error;

    auto constructKey = [&, this]() mutable -> keymaster_error_t {
        // GetKeyFactory
        if (error != KM_ERROR_OK)
            return error;
        keymaster_algorithm_t algorithm;
        if (!hw_enforced.GetTagValue(TAG_ALGORITHM, &algorithm)) {
            return KM_ERROR_INVALID_ARGUMENT;
        }
        auto factory = GetKeyFactory(algorithm);
        return factory->LoadKey(move(key_material), additional_params,
                                move(hw_enforced), move(sw_enforced), key);
    };

    Buffer nonce, tag;
    KeymasterKeyBlob encrypted_key_material;
    if (!key)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;
    error = DeserializeAuthEncryptedBlob(blob, &encrypted_key_material,
                                         &hw_enforced, &sw_enforced, &nonce,
                                         &tag);
    if (error != KM_ERROR_OK)
        return error;

    if (nonce.available_read() != OCB_NONCE_LENGTH ||
        tag.available_read() != OCB_TAG_LENGTH)
        return KM_ERROR_INVALID_KEY_BLOB;

    KeymasterKeyBlob master_key;
    error = DeriveMasterKey(&master_key);
    if (error != KM_ERROR_OK)
        return error;

    AuthorizationSet hidden;
    error = BuildHiddenAuthorizations(additional_params, &hidden);
    if (error != KM_ERROR_OK)
        return error;

    error = OcbDecryptKey(hw_enforced, sw_enforced, hidden, master_key,
                          encrypted_key_material, nonce, tag, &key_material);
    return constructKey();
}

keymaster_error_t TrustyKeymasterContext::AddRngEntropy(const uint8_t* buf,
                                                        size_t length) const {
    if (trusty_rng_add_entropy(buf, length) != 0)
        return KM_ERROR_UNKNOWN_ERROR;
    return KM_ERROR_OK;
}

bool TrustyKeymasterContext::SeedRngIfNeeded() const {
    if (ShouldReseedRng())
        const_cast<TrustyKeymasterContext*>(this)->ReseedRng();
    return rng_initialized_;
}

bool TrustyKeymasterContext::ShouldReseedRng() const {
    if (!rng_initialized_) {
        LOG_I("RNG not initalized, reseed", 0);
        return true;
    }

    if (++calls_since_reseed_ % kCallsBetweenRngReseeds == 0) {
        LOG_I("Periodic reseed", 0);
        return true;
    }
    return false;
}

bool TrustyKeymasterContext::ReseedRng() {
    UniquePtr<uint8_t[]> rand_seed(new uint8_t[kRngReseedSize]);
    memset(rand_seed.get(), 0, kRngReseedSize);
    if (trusty_rng_hw_rand(rand_seed.get(), kRngReseedSize) != 0) {
        LOG_E("Failed to get bytes from HW RNG", 0);
        return false;
    }
    LOG_I("Reseeding with %d bytes from HW RNG", kRngReseedSize);
    trusty_rng_add_entropy(rand_seed.get(), kRngReseedSize);

    rng_initialized_ = true;
    return true;
}

// Gee wouldn't it be nice if the crypto service headers defined this.
enum DerivationParams {
    DERIVATION_DATA_PARAM = 0,
    OUTPUT_BUFFER_PARAM = 1,
};

keymaster_error_t TrustyKeymasterContext::DeriveMasterKey(
        KeymasterKeyBlob* master_key) const {
    LOG_D("Deriving master key", 0);

    long rc = hwkey_open();
    if (rc < 0) {
        return KM_ERROR_UNKNOWN_ERROR;
    }

    hwkey_session_t session = (hwkey_session_t)rc;

    if (!master_key->Reset(kAesKeySize)) {
        LOG_S("Could not allocate memory for master key buffer", 0);
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    }

    uint32_t kdf_version = HWKEY_KDF_VERSION_1;
    rc = hwkey_derive(session, &kdf_version, kMasterKeyDerivationData,
                      master_key->writable_data(), kAesKeySize);

    if (rc < 0) {
        LOG_S("Error deriving master key: %d", rc);
        return KM_ERROR_UNKNOWN_ERROR;
    }

    hwkey_close(session);
    LOG_I("Key derivation complete", 0);
    return KM_ERROR_OK;
}

bool TrustyKeymasterContext::InitializeAuthTokenKey() {
    if (GenerateRandom(auth_token_key_, kAuthTokenKeySize) != KM_ERROR_OK)
        return false;
    auth_token_key_initialized_ = true;
    return auth_token_key_initialized_;
}

keymaster_error_t TrustyKeymasterContext::GetAuthTokenKey(
        keymaster_key_blob_t* key) const {
    if (!auth_token_key_initialized_ &&
        !const_cast<TrustyKeymasterContext*>(this)->InitializeAuthTokenKey())
        return KM_ERROR_UNKNOWN_ERROR;

    key->key_material = auth_token_key_;
    key->key_material_size = kAuthTokenKeySize;
    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterContext::SetSystemVersion(
        uint32_t os_version,
        uint32_t os_patchlevel) {
    if (!version_info_set_) {
        // Note that version info is now set by Configure, rather than by the
        // bootloader.  This is to ensure that system-only updates can be done,
        // to avoid breaking Project Treble.
        boot_os_version_ = os_version;
        boot_os_patchlevel_ = os_patchlevel;
        version_info_set_ = true;
    }

#ifdef KEYMASTER_DEBUG
    Buffer fake_root_of_trust("000111222333444555666777888999000", 32);
    Buffer verified_boot_hash_none;
    if (!root_of_trust_set_) {
        /* Sets bootloader parameters to what is expected on a 'good' device,
         * will pass attestation CTS tests. FOR DEBUGGING ONLY.
         */
        SetBootParams(os_version, os_patchlevel, fake_root_of_trust,
                      KM_VERIFIED_BOOT_VERIFIED, true, verified_boot_hash_none);
    }
#endif

    return KM_ERROR_OK;
}

void TrustyKeymasterContext::GetSystemVersion(uint32_t* os_version,
                                              uint32_t* os_patchlevel) const {
    *os_version = boot_os_version_;
    *os_patchlevel = boot_os_patchlevel_;
}

keymaster_error_t TrustyKeymasterContext::GetVerifiedBootParams(
        keymaster_blob_t* verified_boot_key,
        keymaster_verified_boot_t* verified_boot_state,
        bool* device_locked) const {
    verified_boot_key->data = verified_boot_key_.begin();
    verified_boot_key->data_length = verified_boot_key_.buffer_size();
    *verified_boot_state = verified_boot_state_;
    *device_locked = device_locked_;
    return KM_ERROR_OK;
}

KeymasterKeyBlob AttestationKey(keymaster_algorithm_t algorithm,
                                keymaster_error_t* error) {
    const uint8_t* key = nullptr;
    uint32_t key_size = 0;
    AttestationKeySlot key_slot;

    switch (algorithm) {
    case KM_ALGORITHM_RSA:
        key_slot = AttestationKeySlot::kRsa;
        break;

    case KM_ALGORITHM_EC:
        key_slot = AttestationKeySlot::kEcdsa;
        break;

    default:
        *error = KM_ERROR_UNSUPPORTED_ALGORITHM;
        return {};
    }

    auto result = ReadKeyFromStorage(key_slot, error);
    if (*error != KM_ERROR_OK) {
        LOG_I("Failed to read attestation key from RPMB, falling back to test key",
              0);
        *error = GetSoftwareAttestationKey(algorithm, &key, &key_size);
        if (*error != KM_ERROR_OK)
            return {};
        result = KeymasterKeyBlob(key, key_size);
        if (!result.key_material)
            *error = KM_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    return result;
}

CertChainPtr AttestationChain(keymaster_algorithm_t algorithm,
                                         keymaster_error_t* error) {
    CertChainPtr chain(new keymaster_cert_chain_t);
    if (!chain.get()) {
        *error = KM_ERROR_MEMORY_ALLOCATION_FAILED;
        return nullptr;
    }
    AttestationKeySlot key_slot;
    switch (algorithm) {
    case KM_ALGORITHM_RSA:
        key_slot = AttestationKeySlot::kRsa;
        break;
    case KM_ALGORITHM_EC:
        key_slot = AttestationKeySlot::kEcdsa;
        break;
    default:
        *error = KM_ERROR_UNSUPPORTED_ALGORITHM;
        return nullptr;
    }
    memset(chain.get(), 0, sizeof(keymaster_cert_chain_t));

    *error = ReadCertChainFromStorage(key_slot, chain.get());
    if (*error != KM_ERROR_OK) {
        LOG_I("Failed to read attestation chain from RPMB, falling back to test chain",
              0);
        *error = GetSoftwareAttestationChain(algorithm, chain.get());
    }
    if (*error != KM_ERROR_OK)
        return nullptr;
    return chain;
}

keymaster_error_t TrustyKeymasterContext::GenerateAttestation(
        const Key& key,
        const AuthorizationSet& attest_params,
        CertChainPtr* cert_chain) const {
    keymaster_error_t error = KM_ERROR_OK;
    keymaster_algorithm_t key_algorithm;
    if (!key.authorizations().GetTagValue(TAG_ALGORITHM, &key_algorithm)) {
        return KM_ERROR_UNKNOWN_ERROR;
    }

    if ((key_algorithm != KM_ALGORITHM_RSA && key_algorithm != KM_ALGORITHM_EC))
        return KM_ERROR_INCOMPATIBLE_ALGORITHM;

    // We have established that the given key has the correct algorithm, and
    // because this is the SoftKeymasterContext we can assume that the Key is an
    // AsymmetricKey. So we can downcast.
    const AsymmetricKey& asymmetric_key =
            static_cast<const AsymmetricKey&>(key);

    auto attestation_chain = AttestationChain(key_algorithm, &error);
    if (error != KM_ERROR_OK)
        return error;

    auto attestation_key = AttestationKey(key_algorithm, &error);
    if (error != KM_ERROR_OK)
        return error;

    return generate_attestation(asymmetric_key, attest_params,
                                *attestation_chain, attestation_key, *this,
                                cert_chain);
}

keymaster_error_t TrustyKeymasterContext::SetBootParams(
        uint32_t /* os_version */,
        uint32_t /* os_patchlevel */,
        const Buffer& verified_boot_key,
        keymaster_verified_boot_t verified_boot_state,
        bool device_locked,
        const Buffer& verified_boot_hash) {
    if (root_of_trust_set_)
        return KM_ERROR_ROOT_OF_TRUST_ALREADY_SET;

    verified_boot_hash_.Reinitialize(verified_boot_hash);
    root_of_trust_set_ = true;

    if (verified_boot_key.buffer_size()) {
        verified_boot_key_.Reinitialize(verified_boot_key);
        verified_boot_state_ = verified_boot_state;
        device_locked_ = device_locked;
    } else {
        // If no verified boot key hash is passed, then verified boot state is
        // considered unverified and unlocked.
        verified_boot_state_ = KM_VERIFIED_BOOT_UNVERIFIED;
        device_locked_ = false;
    }
    return KM_ERROR_OK;
}

keymaster_error_t TrustyKeymasterContext::UnwrapKey(
        const KeymasterKeyBlob& wrapped_key_blob,
        const KeymasterKeyBlob& wrapping_key_blob,
        const AuthorizationSet& wrapping_key_params,
        const KeymasterKeyBlob& masking_key,
        AuthorizationSet* wrapped_key_params,
        keymaster_key_format_t* wrapped_key_format,
        KeymasterKeyBlob* wrapped_key_material) const {
    return KM_ERROR_UNIMPLEMENTED;
}

}  // namespace keymaster
