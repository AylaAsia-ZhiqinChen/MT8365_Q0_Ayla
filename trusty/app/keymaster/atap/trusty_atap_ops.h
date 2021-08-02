/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef TRUSTY_ATAP_OPS_H_
#define TRUSTY_ATAP_OPS_H_

#include "ops/openssl_ops.h"

namespace keymaster {

// An ops implementation for Trusty. All instances of this class must be created
// on the same thread. This class is intended to be used with libatap and does
// not perform additional verification of data formats. Only use this class with
// sanitized inputs.
class TrustyAtapOps : public atap::OpensslOps {
public:
    TrustyAtapOps();
    ~TrustyAtapOps() override;

    // AtapOpsDelegate methods. Other methods are handled by OpensslOps.
    AtapResult read_product_id(
            uint8_t product_id[ATAP_PRODUCT_ID_LEN]) override;

    AtapResult get_auth_key_type(AtapKeyType* key_type) override;

    AtapResult read_auth_key_cert_chain(AtapCertChain* cert_chain) override;

    AtapResult write_attestation_key(AtapKeyType key_type,
                                     const AtapBlob* key,
                                     const AtapCertChain* cert_chain) override;

    AtapResult read_attestation_public_key(AtapKeyType key_type,
                                           uint8_t pubkey[ATAP_KEY_LEN_MAX],
                                           uint32_t* pubkey_len) override;

    AtapResult read_soc_global_key(
            uint8_t global_key[ATAP_AES_128_KEY_LEN]) override;

    AtapResult write_hex_uuid(const uint8_t uuid[ATAP_HEX_UUID_LEN]) override;

    AtapResult auth_key_sign(const uint8_t* nonce,
                             uint32_t nonce_len,
                             uint8_t sig[ATAP_SIGNATURE_LEN_MAX],
                             uint32_t* sig_len) override;

private:
    bool _auth_key_type_init = false;
    AtapKeyType _auth_key_type;
};

}  // namespace keymaster

#endif /* TRUSTY_ATAP_OPS_H_ */
