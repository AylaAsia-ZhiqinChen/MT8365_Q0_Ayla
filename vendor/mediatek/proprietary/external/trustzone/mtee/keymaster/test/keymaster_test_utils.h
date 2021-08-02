#ifndef KEYMASTER_TEST_UTILS_H_
#define KEYMASTER_TEST_UTILS_H_

/*
 * Utilities used to help with testing.
 */

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include <hardware/keymaster2.h>

#include <keymaster/authorization_set.h>

bool operator==(const keymaster_key_param_t& a, const keymaster_key_param_t& b);

namespace keymaster {

bool operator==(const AuthorizationSet& a, const AuthorizationSet& b);
bool operator!=(const AuthorizationSet& a, const AuthorizationSet& b);

namespace test {

const uint64_t OP_HANDLE_SENTINEL = 0xFFFFFFFFFFFFFFFF;
class Keymaster2Test {
  public:
    Keymaster2Test();
    ~Keymaster2Test();

    keymaster2_device_t* device();

    keymaster_error_t Configure(const AuthorizationSet& config_params);

    keymaster_error_t GenerateKey(const AuthorizationSetBuilder& builder);

    keymaster_error_t ImportKey(const AuthorizationSetBuilder& builder,
                                keymaster_key_format_t format, const std::string& key_material);

    keymaster_error_t ExportKey(keymaster_key_format_t format, std::string* export_data);

    keymaster_error_t GetCharacteristics();

    keymaster_error_t BeginOperation(keymaster_purpose_t purpose);

    keymaster_error_t BeginOperation(keymaster_purpose_t purpose, const AuthorizationSet& input_set,
                                     AuthorizationSet* output_set = NULL);

    keymaster_error_t UpdateOperation(const std::string& message, std::string* output,
                                      size_t* input_consumed);

    keymaster_error_t UpdateOperation(const AuthorizationSet& additional_params,
                                      const std::string& message, AuthorizationSet* output_params,
                                      std::string* output, size_t* input_consumed);

    keymaster_error_t FinishOperation(std::string* output);
    keymaster_error_t FinishOperation(const std::string& input, const std::string& signature,
                                      std::string* output);
    keymaster_error_t FinishOperation(const AuthorizationSet& additional_params,
                                      const std::string& input, const std::string& signature,
                                      std::string* output) {
        return FinishOperation(additional_params, input, signature, nullptr /*output_params */, output);
    }
    keymaster_error_t FinishOperation(const AuthorizationSet& additional_params,
                                      const std::string& input, const std::string& signature,
                                      AuthorizationSet* output_params, std::string* output);

    keymaster_error_t AbortOperation();

    keymaster_error_t AttestKey(const std::string& attest_challenge, keymaster_cert_chain_t* chain);

    keymaster_error_t UpgradeKey(const AuthorizationSet& upgrade_params);

    keymaster_error_t GetVersion(uint8_t* major, uint8_t* minor, uint8_t* subminor);

    std::string ProcessMessage(keymaster_purpose_t purpose, const std::string& message);
    std::string ProcessMessage(keymaster_purpose_t purpose, const std::string& message,
                               const AuthorizationSet& begin_params,
                               const AuthorizationSet& update_params,
                               AuthorizationSet* output_params = NULL);
    std::string ProcessMessage(keymaster_purpose_t purpose, const std::string& message,
                               const std::string& signature, const AuthorizationSet& begin_params,
                               const AuthorizationSet& update_params,
                               AuthorizationSet* output_params = NULL);
    std::string ProcessMessage(keymaster_purpose_t purpose, const std::string& message,
                               const std::string& signature);

    void SignMessage(const std::string& message, std::string* signature, keymaster_digest_t digest);
    void SignMessage(const std::string& message, std::string* signature, keymaster_digest_t digest,
                     keymaster_padding_t padding);

    AuthorizationSet UserAuthParams();
    AuthorizationSet ClientParams();

    AuthorizationSet hw_enforced();
    AuthorizationSet sw_enforced();

    void FreeCharacteristics();
    void FreeKeyBlob();

    void set_key_blob(const uint8_t* key, size_t key_length) {
        FreeKeyBlob();
        blob_.key_material = key;
        blob_.key_material_size = key_length;
    }

    AuthorizationSet client_params() {
        return AuthorizationSet(client_params_, sizeof(client_params_) / sizeof(client_params_[0]));
    }

 protected:
    int CreateDevice(keymaster2_device_t **dev);

 private:
    keymaster2_device_t* device_;
    keymaster_blob_t client_id_ = {.data = reinterpret_cast<const uint8_t*>("app_id"),
                                   .data_length = 6
                                  };
    keymaster_key_param_t client_params_[1] = {Authorization(TAG_APPLICATION_ID, client_id_.data, client_id_.data_length)};

    uint64_t op_handle_;

    keymaster_key_blob_t blob_;
    keymaster_key_characteristics_t characteristics_;
};

}   // namespace test
}   // namespace keymaster

#endif
