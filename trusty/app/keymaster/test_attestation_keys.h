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

#ifndef TRUSTY_APP_KEYMASTER_TEST_ATTESTATION_KEYS_H_
#define TRUSTY_APP_KEYMASTER_TEST_ATTESTATION_KEYS_H_

namespace keymaster {

/**
 * On success, writes address of a software attestation key to |key| and writes
 * the length to |key_length|. Caller does not take ownership of software keys
 */
keymaster_error_t GetSoftwareAttestationKey(keymaster_algorithm_t algorithm,
                                            const uint8_t** key,
                                            uint32_t* key_length);

/**
 * On success, allocates and copies software attestation certificate chain to
 * |chain|. Caller takes ownership of all memory allocated for |chain|.
 */
keymaster_error_t GetSoftwareAttestationChain(keymaster_algorithm_t algorithm,
                                              keymaster_cert_chain_t* chain);

}  // namespace keymaster

#endif  // TRUSTY_APP_KEYMASTER_TEST_ATTESTATION_KEYS_H_
