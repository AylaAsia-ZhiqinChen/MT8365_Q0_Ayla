/*
 * Copyright (C) 2012 The Android Open Source Project
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

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <regex.h>

#include <fstream>
#include <iostream>

#include <gtest/gtest.h>

#include "keymaster_test_utils.h"
#include "openssl_utils.h"

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "keymaster_test"
#endif

#include <cutils/log.h>
#include <cutils/properties.h>

#include <hardware/keymaster2.h>
#include <hardware/hardware.h>
#include <keymaster/authorization_set.h>

using std::string;

namespace keymaster {
namespace test {

constexpr char kPlatformVersionProp[] = "ro.build.version.release";
constexpr char kPlatformVersionRegex[] = "^([0-9]{1,2})(\\.([0-9]{1,2}))?(\\.([0-9]{1,2}))?";
constexpr size_t kMajorVersionMatch = 1;
constexpr size_t kMinorVersionMatch = 3;
constexpr size_t kSubminorVersionMatch = 5;
constexpr size_t kPlatformVersionMatchCount = kSubminorVersionMatch + 1;

constexpr char kPlatformPatchlevelProp[] = "ro.build.version.security_patch";
constexpr char kPlatformPatchlevelRegex[] = "^([0-9]{4})-([0-9]{2})-[0-9]{2}$";
constexpr size_t kYearMatch = 1;
constexpr size_t kMonthMatch = 2;
constexpr size_t kPlatformPatchlevelMatchCount = kMonthMatch + 1;

uint32_t match_to_uint32(const char* expression, const regmatch_t& match) {
    if (match.rm_so == -1)
        return 0;

    size_t len = match.rm_eo - match.rm_so;
    std::string s(expression + match.rm_so, len);
    return std::stoul(s);
}

uint32_t GetOsVersion(const char* version_str) {
    regex_t regex;
    if (regcomp(&regex, kPlatformVersionRegex, REG_EXTENDED)) {
        ALOGE("Failed to compile version regex! (%s)", kPlatformVersionRegex);
        return 0;
    }

    regmatch_t matches[kPlatformVersionMatchCount];
    if (regexec(&regex, version_str, kPlatformVersionMatchCount, matches, 0 /* flags */)) {
        ALOGI("Platform version string does not match expected format. Using version 0.");
        return 0;
    }

    uint32_t major = match_to_uint32(version_str, matches[kMajorVersionMatch]);
    uint32_t minor = match_to_uint32(version_str, matches[kMinorVersionMatch]);
    uint32_t subminor = match_to_uint32(version_str, matches[kSubminorVersionMatch]);

    return (major * 100 + minor) * 100 + subminor;
}

uint32_t GetOsPatchlevel(const char* patchlevel_str) {
    regex_t regex;
    if (regcomp(&regex, kPlatformPatchlevelRegex, REG_EXTENDED) != 0) {
        ALOGE("Failed to compile platform patchlevel regex! (%s)", kPlatformPatchlevelRegex);
        return 0;
    }

    regmatch_t matches[kPlatformPatchlevelMatchCount];
    if (regexec(&regex, patchlevel_str, kPlatformPatchlevelMatchCount, matches, 0 /* flags */)) {
        ALOGI("Platform patchlevel string does not match expected format. Using patchlevel 0");
        return 0;
    }

    uint32_t year = match_to_uint32(patchlevel_str, matches[kYearMatch]);
    uint32_t month = match_to_uint32(patchlevel_str, matches[kMonthMatch]);

    if (month < 1 || month > 12) {
        ALOGE("Invalid patch month %d", month);
        return 0;
    }
    return year * 100 + month;
}

void GetSystemParams(uint32_t* version, uint32_t* patchlevel) {
    char version_str[PROPERTY_VALUE_MAX];
    property_get(kPlatformVersionProp, version_str, "" /* default */);
    *version = GetOsVersion(version_str);

    char patchlevel_str[PROPERTY_VALUE_MAX];
    property_get(kPlatformPatchlevelProp, patchlevel_str, "" /* default */);
    *patchlevel = GetOsPatchlevel(patchlevel_str);
}

X509* parse_cert_blob(const keymaster_blob_t& blob) {

    const uint8_t* p = blob.data;
    return d2i_X509(nullptr, &p, blob.data_length);
}

bool verify_chain(const keymaster_cert_chain_t& chain) {

    for (size_t i = 0; i < chain.entry_count - 1; ++i) {
        keymaster_blob_t& key_cert_blob = chain.entries[i];
        keymaster_blob_t& signing_cert_blob = chain.entries[i + 1];

        X509_Ptr key_cert(parse_cert_blob(key_cert_blob));
        X509_Ptr signing_cert(parse_cert_blob(signing_cert_blob));
        EXPECT_TRUE(!!key_cert.get() && !!signing_cert.get());
        if (!key_cert.get() || !signing_cert.get())
            return false;

        EVP_PKEY_Ptr signing_pubkey(X509_get_pubkey(signing_cert.get()));
        EXPECT_TRUE(!!signing_pubkey.get());
        if (!signing_pubkey.get())
            return false;

        EXPECT_EQ(1, X509_verify(key_cert.get(), signing_pubkey.get()))
            << "Verification of certificate " << i << " failed";
    }

    return true;
}


class KeymasterTest : public Keymaster2Test,
                      public ::testing::Test {
public:
    static void SetUpTestCase() {

    }

    static void TearDownTestCase() {

    }
};

class RsaAttestationTest : public KeymasterTest,
                           public ::testing::WithParamInterface<uint32_t> {
};


class EcAttestationTest : public KeymasterTest,
                          public ::testing::WithParamInterface<uint32_t> {
};

INSTANTIATE_TEST_CASE_P(MTEEKeymaster, RsaAttestationTest, ::testing::Values(512U, 1024U, 2048U, 3072U, 4096U));

TEST_P(RsaAttestationTest, RsaSuccess) {

    uint32_t version, patchlevel;
    GetSystemParams(&version, &patchlevel);
    AuthorizationSet params(AuthorizationSetBuilder()
                                    .Authorization(keymaster::TAG_OS_VERSION, version)
                                    .Authorization(keymaster::TAG_OS_PATCHLEVEL, patchlevel));
    ASSERT_EQ(KM_ERROR_OK, Configure(params));

    uint32_t rsa_key_size = GetParam();
    ASSERT_EQ(KM_ERROR_OK, GenerateKey(AuthorizationSetBuilder()
                                            .RsaSigningKey(rsa_key_size, 65537)
                                            .Digest(KM_DIGEST_SHA_2_256)
                                            .Padding(KM_PAD_RSA_PSS)
                                            .Authorization(TAG_NO_AUTH_REQUIRED)
                                            .Authorization(TAG_ACTIVE_DATETIME, 1000)))
                << "Should generate an RSA key with " << GetParam() << " bit modulus size";

    keymaster_cert_chain_t cert_chain;
    EXPECT_EQ(KM_ERROR_OK, AttestKey("challenge", &cert_chain));
    EXPECT_LE(3U, cert_chain.entry_count);
    EXPECT_TRUE(verify_chain(cert_chain));
}


INSTANTIATE_TEST_CASE_P(MTEEKeymaster, EcAttestationTest, ::testing::Values(224U, 256U, 384U, 521U));

TEST_P(EcAttestationTest, EcSuccess) {

    uint32_t version, patchlevel;
    GetSystemParams(&version, &patchlevel);
    AuthorizationSet params(AuthorizationSetBuilder()
                                    .Authorization(keymaster::TAG_OS_VERSION, version)
                                    .Authorization(keymaster::TAG_OS_PATCHLEVEL, patchlevel));
    ASSERT_EQ(KM_ERROR_OK, Configure(params));

    uint32_t ecdsa_key_size = GetParam();

    ASSERT_EQ(KM_ERROR_OK, GenerateKey(AuthorizationSetBuilder()
                                        .EcdsaSigningKey(ecdsa_key_size)
                                        .Digest(KM_DIGEST_SHA_2_256)
                                        .Authorization(TAG_NO_AUTH_REQUIRED)
                                        .Authorization(TAG_ACTIVE_DATETIME, 1000)))
                << "Should generate an EC key with " << GetParam() << " field size";

    keymaster_cert_chain_t cert_chain;
    EXPECT_EQ(KM_ERROR_OK, AttestKey("challenge", &cert_chain));
    EXPECT_LE(3U, cert_chain.entry_count);
    EXPECT_TRUE(verify_chain(cert_chain));
}


typedef KeymasterTest GetKeyCharacteristics;

TEST_F(GetKeyCharacteristics, SimpleRsa) {

    uint32_t version, patchlevel;
    GetSystemParams(&version, &patchlevel);
    AuthorizationSet params(AuthorizationSetBuilder()
                                    .Authorization(keymaster::TAG_OS_VERSION, version)
                                    .Authorization(keymaster::TAG_OS_PATCHLEVEL, patchlevel));

    ASSERT_EQ(KM_ERROR_OK, Configure(params));

    ASSERT_EQ(KM_ERROR_OK, GenerateKey(AuthorizationSetBuilder()
                                        .RsaSigningKey(256, 3)
                                        .Digest(KM_DIGEST_NONE)
                                        .Padding(KM_PAD_NONE)));

    AuthorizationSet original(sw_enforced());

    ASSERT_EQ(KM_ERROR_OK, GetCharacteristics());
    EXPECT_EQ(original, sw_enforced());
}

typedef KeymasterTest SigningOperationsTest;

TEST_F(SigningOperationsTest, RsaSuccess) {

    uint32_t version, patchlevel;
    GetSystemParams(&version, &patchlevel);
    AuthorizationSet params(AuthorizationSetBuilder()
                                    .Authorization(keymaster::TAG_OS_VERSION, version)
                                    .Authorization(keymaster::TAG_OS_PATCHLEVEL, patchlevel));
    ASSERT_EQ(KM_ERROR_OK, Configure(params));

    ASSERT_EQ(KM_ERROR_OK, GenerateKey(AuthorizationSetBuilder()
                                        .RsaSigningKey(256, 3)
                                        .Digest(KM_DIGEST_NONE)
                                        .Padding(KM_PAD_NONE)));
    string message = "12345678901234567890123456789012";
    string signature;
    SignMessage(message, &signature, KM_DIGEST_NONE, KM_PAD_NONE);
}

TEST_F(SigningOperationsTest, RsaPssSha256Success) {

    uint32_t version, patchlevel;
    GetSystemParams(&version, &patchlevel);
    AuthorizationSet params(AuthorizationSetBuilder()
                                    .Authorization(keymaster::TAG_OS_VERSION, version)
                                    .Authorization(keymaster::TAG_OS_PATCHLEVEL, patchlevel));
    ASSERT_EQ(KM_ERROR_OK, Configure(params));

    ASSERT_EQ(KM_ERROR_OK, GenerateKey(AuthorizationSetBuilder()
                                        .RsaSigningKey(768, 3)
                                        .Digest(KM_DIGEST_SHA_2_256)
                                        .Padding(KM_PAD_RSA_PSS)));
    // Use large message, which won't work without digesting.
    string message(1024, 'a');
    string signature;
    SignMessage(message, &signature, KM_DIGEST_SHA_2_256, KM_PAD_RSA_PSS);
}

TEST_F(SigningOperationsTest, RsaPaddingNoneDoesNotAllowOther) {

    uint32_t version, patchlevel;
    GetSystemParams(&version, &patchlevel);
    AuthorizationSet params(AuthorizationSetBuilder()
                                    .Authorization(keymaster::TAG_OS_VERSION, version)
                                    .Authorization(keymaster::TAG_OS_PATCHLEVEL, patchlevel));
    ASSERT_EQ(KM_ERROR_OK, Configure(params));

    ASSERT_EQ(KM_ERROR_OK, GenerateKey(AuthorizationSetBuilder()
                                        .RsaSigningKey(512, 3)
                                        .Digest(KM_DIGEST_NONE)
                                        .Padding(KM_PAD_NONE)));
    string message = "12345678901234567890123456789012";
    string signature;

    AuthorizationSet begin_params(client_params());
    begin_params.push_back(TAG_DIGEST, KM_DIGEST_NONE);
    begin_params.push_back(TAG_PADDING, KM_PAD_RSA_PKCS1_1_5_SIGN);
    EXPECT_EQ(KM_ERROR_INCOMPATIBLE_PADDING_MODE, BeginOperation(KM_PURPOSE_SIGN, begin_params));
}

TEST_F(SigningOperationsTest, RsaPkcs1Sha256Success) {

    uint32_t version, patchlevel;
    GetSystemParams(&version, &patchlevel);
    AuthorizationSet params(AuthorizationSetBuilder()
                                    .Authorization(keymaster::TAG_OS_VERSION, version)
                                    .Authorization(keymaster::TAG_OS_PATCHLEVEL, patchlevel));
    ASSERT_EQ(KM_ERROR_OK, Configure(params));

    ASSERT_EQ(KM_ERROR_OK, GenerateKey(AuthorizationSetBuilder()
                                        .RsaSigningKey(512, 3)
                                        .Digest(KM_DIGEST_SHA_2_256)
                                        .Padding(KM_PAD_RSA_PKCS1_1_5_SIGN)));
    string message(1024, 'a');
    string signature;
    SignMessage(message, &signature, KM_DIGEST_SHA_2_256, KM_PAD_RSA_PKCS1_1_5_SIGN);
}

TEST_F(SigningOperationsTest, AesEcbSign) {

    uint32_t version, patchlevel;
    GetSystemParams(&version, &patchlevel);
    AuthorizationSet params(AuthorizationSetBuilder()
                                    .Authorization(keymaster::TAG_OS_VERSION, version)
                                    .Authorization(keymaster::TAG_OS_PATCHLEVEL, patchlevel));
    ASSERT_EQ(KM_ERROR_OK, Configure(params));

    ASSERT_EQ(KM_ERROR_OK,
              GenerateKey(AuthorizationSetBuilder().AesEncryptionKey(128).Authorization(TAG_BLOCK_MODE, KM_MODE_ECB)));
    ASSERT_EQ(KM_ERROR_UNSUPPORTED_PURPOSE, BeginOperation(KM_PURPOSE_SIGN));
    ASSERT_EQ(KM_ERROR_UNSUPPORTED_PURPOSE, BeginOperation(KM_PURPOSE_VERIFY));

}

}   // namespace test
}   // namespace keymaster
