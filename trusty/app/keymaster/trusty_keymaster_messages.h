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

#ifndef TRUSTY_APP_KEYMASTER_TRUSTY_KEYMASTER_MESSAGES_H_
#define TRUSTY_APP_KEYMASTER_TRUSTY_KEYMASTER_MESSAGES_H_

#include <keymaster/android_keymaster_messages.h>

namespace keymaster {

/**
 * Generic struct for Keymaster requests which hold a single raw buffer.
 */
struct RawBufferRequest : public KeymasterMessage {
    explicit RawBufferRequest(int32_t ver = MAX_MESSAGE_VERSION)
            : KeymasterMessage(ver) {}

    size_t SerializedSize() const override { return data.SerializedSize(); }
    uint8_t* Serialize(uint8_t* buf, const uint8_t* end) const override {
        return data.Serialize(buf, end);
    }
    bool Deserialize(const uint8_t** buf_ptr, const uint8_t* end) override {
        return data.Deserialize(buf_ptr, end);
    }

    Buffer data;
};

/**
 * Generic struct for Keymaster responses which hold a single raw buffer.
 */
struct RawBufferResponse : public KeymasterResponse {
    explicit RawBufferResponse(int32_t ver = MAX_MESSAGE_VERSION)
            : KeymasterResponse(ver) {}

    size_t NonErrorSerializedSize() const override {
        return data.SerializedSize();
    }
    uint8_t* NonErrorSerialize(uint8_t* buf,
                               const uint8_t* end) const override {
        return data.Serialize(buf, end);
    }
    bool NonErrorDeserialize(const uint8_t** buf_ptr,
                             const uint8_t* end) override {
        return data.Deserialize(buf_ptr, end);
    }

    Buffer data;
};

/**
 * Generic struct for Keymaster responses which have no specialized response
 * data.
 */
struct NoResponse : public KeymasterResponse {
    explicit NoResponse(int32_t ver = MAX_MESSAGE_VERSION)
            : KeymasterResponse(ver) {}

    size_t NonErrorSerializedSize() const override { return 0; }
    uint8_t* NonErrorSerialize(uint8_t* buf,
                               const uint8_t* end) const override {
        return buf;
    }
    bool NonErrorDeserialize(const uint8_t** buf_ptr,
                             const uint8_t* end) override {
        return true;
    }
};

struct NoRequest : public KeymasterMessage {
    explicit NoRequest(int32_t ver = MAX_MESSAGE_VERSION)
            : KeymasterMessage(ver) {}

    size_t SerializedSize() const override { return 0; }
    uint8_t* Serialize(uint8_t* buf, const uint8_t* end) const override {
        return buf;
    }
    bool Deserialize(const uint8_t** buf_ptr, const uint8_t* end) override {
        return true;
    }
};

struct SetBootParamsRequest : public KeymasterMessage {
    explicit SetBootParamsRequest(int32_t ver = MAX_MESSAGE_VERSION)
            : KeymasterMessage(ver) {}

    size_t SerializedSize() const override {
        return (sizeof(os_version) + sizeof(os_patchlevel) +
                sizeof(device_locked) + sizeof(verified_boot_state) +
                verified_boot_key.SerializedSize() +
                verified_boot_hash.SerializedSize());
    }
    uint8_t* Serialize(uint8_t* buf, const uint8_t* end) const override {
        buf = append_uint32_to_buf(buf, end, os_version);
        buf = append_uint32_to_buf(buf, end, os_patchlevel);
        buf = append_uint32_to_buf(buf, end, device_locked);
        buf = append_uint32_to_buf(buf, end, verified_boot_state);
        buf = verified_boot_key.Serialize(buf, end);
        return verified_boot_hash.Serialize(buf, end);
    }
    bool Deserialize(const uint8_t** buf_ptr, const uint8_t* end) override {
        return copy_uint32_from_buf(buf_ptr, end, &os_version) &&
               copy_uint32_from_buf(buf_ptr, end, &os_patchlevel) &&
               copy_uint32_from_buf(buf_ptr, end, &device_locked) &&
               copy_uint32_from_buf(buf_ptr, end, &verified_boot_state) &&
               verified_boot_key.Deserialize(buf_ptr, end) &&
               verified_boot_hash.Deserialize(buf_ptr, end);
    }

    uint32_t os_version;
    uint32_t os_patchlevel;
    uint32_t device_locked;
    keymaster_verified_boot_t verified_boot_state;
    Buffer verified_boot_key;
    Buffer verified_boot_hash;
};

struct SetBootParamsResponse : public NoResponse {};

struct SetAttestationKeyRequest : public KeymasterMessage {
    explicit SetAttestationKeyRequest(int32_t ver = MAX_MESSAGE_VERSION)
            : KeymasterMessage(ver) {}

    size_t SerializedSize() const override {
        return sizeof(uint32_t) + key_data.SerializedSize();
    }
    uint8_t* Serialize(uint8_t* buf, const uint8_t* end) const override {
        buf = append_uint32_to_buf(buf, end, algorithm);
        return key_data.Serialize(buf, end);
    }
    bool Deserialize(const uint8_t** buf_ptr, const uint8_t* end) override {
        return copy_uint32_from_buf(buf_ptr, end, &algorithm) &&
               key_data.Deserialize(buf_ptr, end);
    }

    keymaster_algorithm_t algorithm;
    Buffer key_data;
};

struct SetAttestationKeyResponse : public NoResponse {};

struct AppendAttestationCertChainRequest : public KeymasterMessage {
    explicit AppendAttestationCertChainRequest(
            int32_t ver = MAX_MESSAGE_VERSION)
            : KeymasterMessage(ver) {}

    size_t SerializedSize() const override {
        return sizeof(uint32_t) + cert_data.SerializedSize();
    }
    uint8_t* Serialize(uint8_t* buf, const uint8_t* end) const override {
        buf = append_uint32_to_buf(buf, end, algorithm);
        return cert_data.Serialize(buf, end);
    }
    bool Deserialize(const uint8_t** buf_ptr, const uint8_t* end) override {
        return copy_uint32_from_buf(buf_ptr, end, &algorithm) &&
               cert_data.Deserialize(buf_ptr, end);
    }

    keymaster_algorithm_t algorithm;
    Buffer cert_data;
};

struct AppendAttestationCertChainResponse : public NoResponse {};

/**
 * For Android Things Attestation Provisioning (ATAP), the GetCaRequest message
 * in the protocol are raw opaque messages for the purposes of this IPC call.
 * Since the SetCaResponse message will be very large (> 10k), SetCaResponse is
 * split into *Begin, *Update, and *Finish operations.
 */
struct AtapGetCaRequestRequest : public RawBufferRequest {};
struct AtapGetCaRequestResponse : public RawBufferResponse {};

struct AtapSetCaResponseBeginRequest : public KeymasterMessage {
    explicit AtapSetCaResponseBeginRequest(int32_t ver = MAX_MESSAGE_VERSION)
            : KeymasterMessage(ver) {}

    size_t SerializedSize() const override { return sizeof(uint32_t); }
    uint8_t* Serialize(uint8_t* buf, const uint8_t* end) const override {
        return append_uint32_to_buf(buf, end, ca_response_size);
    }
    bool Deserialize(const uint8_t** buf_ptr, const uint8_t* end) override {
        return copy_uint32_from_buf(buf_ptr, end, &ca_response_size);
    }

    uint32_t ca_response_size;
};
struct AtapSetCaResponseBeginResponse : public NoResponse {};

struct AtapSetCaResponseUpdateRequest : public RawBufferRequest {};
struct AtapSetCaResponseUpdateResponse : public NoResponse {};

struct AtapSetCaResponseFinishRequest : public NoRequest {};
struct AtapSetCaResponseFinishResponse : public NoResponse {};
struct AtapSetProductIdRequest : public RawBufferRequest {};
struct AtapSetProductIdResponse : public NoResponse {};

struct AtapReadUuidRequest : public NoRequest {};
struct AtapReadUuidResponse : public RawBufferResponse {};

}  // namespace keymaster

#endif  // TRUSTY_APP_KEYMASTER_TRUSTY_KEYMASTER_MESSAGES_H_
