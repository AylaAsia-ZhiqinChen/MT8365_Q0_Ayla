#include "keymaster_test_utils.h"
#include <cutils/log.h> 

#include <keymaster/android_keymaster_utils.h>

#include <openssl/rand.h>

using std::string;

bool operator==(const keymaster_key_param_t& a, const keymaster_key_param_t& b) {
    if (a.tag != b.tag) {
        return false;
    }

    switch (keymaster_tag_get_type(a.tag)) {
        case KM_INVALID:
            return true;
        case KM_UINT_REP:
        case KM_UINT:
            return a.integer == b.integer;
        case KM_ENUM_REP:
        case KM_ENUM:
            return a.enumerated == b.enumerated;
        case KM_ULONG:
        case KM_ULONG_REP:
            return a.long_integer == b.long_integer;
        case KM_DATE:
            return a.date_time == b.date_time;
        case KM_BOOL:
            return a.boolean == b.boolean;
        case KM_BIGNUM:
        case KM_BYTES:
            if ((a.blob.data == NULL || b.blob.data == NULL) && a.blob.data != b.blob.data)
                return false;
            return a.blob.data_length == b.blob.data_length &&
                   (memcmp(a.blob.data, b.blob.data, a.blob.data_length) == 0);
    }

    return false;
}

namespace keymaster {

bool operator==(const AuthorizationSet& a, const AuthorizationSet& b) {
    if (a.size() != b.size())
        return false;

    for (size_t i = 0; i < a.size(); ++i)
        if (!(a[i] == b[i]))
            return false;
    return true;
}

bool operator!=(const AuthorizationSet& a, const AuthorizationSet& b) {
    return !(a == b);
}

namespace test {

Keymaster2Test::Keymaster2Test() : op_handle_(OP_HANDLE_SENTINEL) {
    memset(&characteristics_, 0, sizeof(characteristics_));
    blob_.key_material = nullptr;
    RAND_seed("foobar", 6);
    blob_.key_material_size= 0;
    CreateDevice(&device_);
}

Keymaster2Test::~Keymaster2Test() {
    FreeCharacteristics();
    FreeKeyBlob();
    device_->common.close(reinterpret_cast<hw_device_t*>(device_));
}

int Keymaster2Test::CreateDevice(keymaster2_device_t **dev) {

    ALOGI("Creating hardware keymaster2 device.\n");
    const hw_module_t* mod;
    int rc;
    rc = hw_get_module_by_class(KEYSTORE_HARDWARE_MODULE_ID, NULL, &mod);
    if (rc) {
        ALOGE("could not find any keystore module");
        return -1;
    }

    rc = mod->methods->open(mod, KEYSTORE_KEYMASTER, reinterpret_cast<struct hw_device_t**>(dev));
    if (rc) {
        ALOGE("could not open keymaster device in %s (%s)",
                        KEYSTORE_KEYMASTER, strerror(-rc));
        return -1;
    }
        return 0;
}

keymaster2_device_t* Keymaster2Test::device() {
    return device_;
}

keymaster_error_t Keymaster2Test::Configure(const AuthorizationSet& config_params) {
    return device()->configure(device(), &config_params);
}

keymaster_error_t Keymaster2Test::GenerateKey(const AuthorizationSetBuilder& builder) {
    AuthorizationSet params(builder.build());
    params.push_back(UserAuthParams());
    params.push_back(ClientParams());

    return device()->generate_key(device(), &params, &blob_, &characteristics_);
}

keymaster_error_t Keymaster2Test::ImportKey(const AuthorizationSetBuilder& builder,
                                            keymaster_key_format_t format,
                                            const string& key_material) {
    AuthorizationSet params(builder.build());
    params.push_back(UserAuthParams());
    params.push_back(ClientParams());

    keymaster_blob_t key = {reinterpret_cast<const uint8_t*>(key_material.c_str()),
                            key_material.length()};
    return device()->import_key(device(), &params, format, &key, &blob_, &characteristics_);
}

AuthorizationSet Keymaster2Test::UserAuthParams() {
    AuthorizationSet set;
    set.push_back(TAG_USER_ID, 7);
    set.push_back(TAG_USER_AUTH_TYPE, HW_AUTH_PASSWORD);
    set.push_back(TAG_AUTH_TIMEOUT, 300);
    return set;
}

AuthorizationSet Keymaster2Test::ClientParams() {
    AuthorizationSet set;
    set.push_back(TAG_APPLICATION_ID, "app_id", 6);
    return set;
}

keymaster_error_t Keymaster2Test::BeginOperation(keymaster_purpose_t purpose) {
    AuthorizationSet in_params(client_params());
    keymaster_key_param_set_t out_params;
    keymaster_error_t error = device()->begin(device(), purpose, &blob_, &in_params, &out_params, &op_handle_);

    EXPECT_EQ(0U, out_params.length);
    EXPECT_TRUE(out_params.params == nullptr);
    return error;
}

keymaster_error_t Keymaster2Test::BeginOperation(keymaster_purpose_t purpose,
                                                 const AuthorizationSet& input_set,
                                                 AuthorizationSet* output_set) {
    keymaster_key_param_set_t out_params;
    keymaster_error_t error = device()->begin(device(), purpose, &blob_, &input_set, &out_params, &op_handle_);

    if (error == KM_ERROR_OK) {
        if (output_set) {
            output_set->Reinitialize(out_params);
        } else {
            EXPECT_EQ(0U, out_params.length);
            EXPECT_TRUE(out_params.params == nullptr);
        }
        keymaster_free_param_set(&out_params);
    }
    return error;
}

keymaster_error_t Keymaster2Test::UpdateOperation(const string& message, string *output,
                                                  size_t *input_consumed) {
    EXPECT_NE(op_handle_, OP_HANDLE_SENTINEL);
    keymaster_blob_t input = {reinterpret_cast<const uint8_t*>(message.c_str()), message.length()};
    keymaster_blob_t out_tmp;

    keymaster_key_param_set_t out_params;
    keymaster_error_t error = device()->update(device(), op_handle_, nullptr /* params */, &input,
                                               input_consumed, &out_params, &out_tmp);
    if (error == KM_ERROR_OK && out_tmp.data)
        output->append(reinterpret_cast<const char*>(out_tmp.data), out_tmp.data_length);
    free(const_cast<uint8_t*>(out_tmp.data));
    return error;
}

keymaster_error_t Keymaster2Test::UpdateOperation(const AuthorizationSet& additional_params,
                                                  const string& message,
                                                  AuthorizationSet* output_params, string* output,
                                                  size_t* input_consumed) {
    EXPECT_NE(op_handle_, OP_HANDLE_SENTINEL);
    keymaster_blob_t input = {reinterpret_cast<const uint8_t*>(message.c_str()), message.length()};
    keymaster_blob_t out_tmp;
    keymaster_key_param_set_t out_params;
    keymaster_error_t error = device()->update(device(), op_handle_, &additional_params, &input,
                                               input_consumed, &out_params, &out_tmp);

    if (error == KM_ERROR_OK && out_tmp.data)
        output->append(reinterpret_cast<const char*>(out_tmp.data), out_tmp.data_length);
    free((void*)out_tmp.data);
    if (output_params)
        output_params->Reinitialize(out_params);
    keymaster_free_param_set(&out_params);
    return error;
}

keymaster_error_t Keymaster2Test::FinishOperation(string* output) {
    return FinishOperation("", "", output);
}

keymaster_error_t Keymaster2Test::FinishOperation(const string& input, const string& signature,
                                                  string* output) {
    AuthorizationSet additional_params;
    AuthorizationSet output_params;
    return FinishOperation(additional_params, input, signature, &output_params, output);
}

keymaster_error_t Keymaster2Test::FinishOperation(const AuthorizationSet& additional_params,
                                                  const string& input, const string& signature,
                                                  AuthorizationSet* output_params, string* output) {
    keymaster_blob_t inp = {reinterpret_cast<const uint8_t*>(input.c_str()), input.length()};
    keymaster_blob_t sig = {reinterpret_cast<const uint8_t*>(signature.c_str()), signature.length()};

    keymaster_blob_t out_tmp;
    keymaster_key_param_set_t out_params;
    keymaster_error_t error = device()->finish(device(), op_handle_, &additional_params, &inp, &sig,
                                               &out_params, &out_tmp);

    if (error != KM_ERROR_OK) {
        EXPECT_TRUE(out_tmp.data == nullptr);
        EXPECT_TRUE(out_params.params == nullptr);
        return error;
    }

    if (out_tmp.data)
        output->append(reinterpret_cast<const char*>(out_tmp.data), out_tmp.data_length);
    free((void*)out_tmp.data);

    if (output_params)
        output_params->Reinitialize(out_params);
    keymaster_free_param_set(&out_params);
    return error;
}

keymaster_error_t Keymaster2Test::AbortOperation() {
    return device()->abort(device(), op_handle_);
}

keymaster_error_t Keymaster2Test::AttestKey(const string& attest_challenge,
                                            keymaster_cert_chain_t* cert_chain) {
    AuthorizationSet attest_params;
    attest_params.push_back(UserAuthParams());
    attest_params.push_back(ClientParams());
    attest_params.push_back(TAG_ATTESTATION_CHALLENGE, attest_challenge.data(), attest_challenge.length());

    return device()->attest_key(device(), &blob_, &attest_params, cert_chain);
}

keymaster_error_t Keymaster2Test::UpgradeKey(const AuthorizationSet& upgrade_params) {
    keymaster_key_blob_t upgraded_blob;
    keymaster_error_t error = device()->upgrade_key(device(), &blob_, &upgrade_params, &upgraded_blob);

    if (error == KM_ERROR_OK) {
        FreeKeyBlob();
        blob_ = upgraded_blob;
    }
    return error;
}

string Keymaster2Test::ProcessMessage(keymaster_purpose_t purpose, const string& message) {
    EXPECT_EQ(KM_ERROR_OK, BeginOperation(purpose, client_params(), NULL /* output_params */));

    string result;
    EXPECT_EQ(KM_ERROR_OK, FinishOperation(message, "" /* signature */, &result));
    return result;
}

string Keymaster2Test::ProcessMessage(keymaster_purpose_t purpose, const string& message,
                                      const AuthorizationSet& begin_params,
                                      const AuthorizationSet& update_params,
                                      AuthorizationSet* begin_out_params) {
    EXPECT_EQ(KM_ERROR_OK, BeginOperation(purpose, begin_params, begin_out_params));

    string result;
    EXPECT_EQ(KM_ERROR_OK, FinishOperation(update_params, message,  "" /* signature */, &result));
    return result;
}

string Keymaster2Test::ProcessMessage(keymaster_purpose_t purpose, const string& message,
                                      const string& signature, const AuthorizationSet& begin_params,
                                      const AuthorizationSet& update_params,
                                      AuthorizationSet* output_params) {
    EXPECT_EQ(KM_ERROR_OK, BeginOperation(purpose, begin_params, output_params));

    string result;
    EXPECT_EQ(KM_ERROR_OK, FinishOperation(update_params, message, signature, &result));
    return result;
}

string Keymaster2Test::ProcessMessage(keymaster_purpose_t purpose, const string& message,
                                      const string& signature) {
    EXPECT_EQ(KM_ERROR_OK, BeginOperation(purpose, client_params(), NULL /* output_params */));

    string result;
    EXPECT_EQ(KM_ERROR_OK, FinishOperation(message, signature, &result));
    return result;
}

void Keymaster2Test::SignMessage(const string& message, string* signature,
                                 keymaster_digest_t digest) {
    AuthorizationSet input_params(AuthorizationSet(client_params_, array_length(client_params_)));
    input_params.push_back(TAG_DIGEST, digest);
    AuthorizationSet update_params;
    AuthorizationSet output_params;
    *signature = ProcessMessage(KM_PURPOSE_SIGN, message, input_params, update_params, &output_params);
    EXPECT_GT(signature->size(), 0U);
}

void Keymaster2Test::SignMessage(const string& message, string* signature,
                                 keymaster_digest_t digest, keymaster_padding_t padding) {
    AuthorizationSet input_params(AuthorizationSet(client_params_, array_length(client_params_)));
    input_params.push_back(TAG_DIGEST, digest);
    input_params.push_back(TAG_PADDING, padding);
    AuthorizationSet update_params;
    AuthorizationSet output_params;
    *signature = ProcessMessage(KM_PURPOSE_SIGN, message, input_params, update_params, &output_params);
    EXPECT_GT(signature->size(), 0U);
}

keymaster_error_t Keymaster2Test::GetCharacteristics() {
    FreeCharacteristics();
    return device()->get_key_characteristics(device(), &blob_, &client_id_, NULL /* app_data */,
                                             &characteristics_);
}

keymaster_error_t Keymaster2Test::ExportKey(keymaster_key_format_t format, string* export_data) {
    keymaster_blob_t export_tmp;
    keymaster_error_t error = device()->export_key(device(), format, &blob_, &client_id_,
                                                   NULL /* app_data */, &export_tmp);

    if (error != KM_ERROR_OK)
        return error;

    *export_data = string(reinterpret_cast<const char*>(export_tmp.data), export_tmp.data_length);
    free((void*)export_tmp.data);
    return error;
}

AuthorizationSet Keymaster2Test::hw_enforced() {
    return AuthorizationSet(characteristics_.hw_enforced);
}

AuthorizationSet Keymaster2Test::sw_enforced() {
    return AuthorizationSet(characteristics_.sw_enforced);
}

void Keymaster2Test::FreeCharacteristics() {
    keymaster_free_characteristics(&characteristics_);
}

void Keymaster2Test::FreeKeyBlob() {
    free(const_cast<uint8_t*>(blob_.key_material));
    blob_.key_material = NULL;
}

}   // namespace test
}   // namespace keymaster
