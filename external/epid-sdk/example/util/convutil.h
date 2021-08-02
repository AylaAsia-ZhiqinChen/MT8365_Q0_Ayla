/*############################################################################
  # Copyright 2016-2017 Intel Corporation
  #
  # Licensed under the Apache License, Version 2.0 (the "License");
  # you may not use this file except in compliance with the License.
  # You may obtain a copy of the License at
  #
  #     http://www.apache.org/licenses/LICENSE-2.0
  #
  # Unless required by applicable law or agreed to in writing, software
  # distributed under the License is distributed on an "AS IS" BASIS,
  # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  # See the License for the specific language governing permissions and
  # limitations under the License.
  ############################################################################*/

/*!
 * \file
 * \brief Conversion utilities interface.
 */
#ifndef EXAMPLE_UTIL_CONVUTIL_H_
#define EXAMPLE_UTIL_CONVUTIL_H_

#include <stddef.h>
#include "epid/common/bitsupplier.h"
#include "epid/common/file_parser.h"
#include "epid/common/types.h"
#include "util/stdtypes.h"
#ifdef TPM_TSS
#include "epid/member/tpm_member.h"
#else
#include "epid/member/software_member.h"
#endif

/// Implementation specific configuration parameters.
typedef struct MemberParams MemberParams;

/// convert a hash algorithm to a string
/*!
  \param[in] alg a hash algorithm
  \returns string representing the algorithm
*/
char const* HashAlgToString(HashAlg alg);

/// convert a string to a hash algorithm
/*!
  \param[in] str a string
  \param[out] alg a hash algorithm
  \retval true string represents a hash algorithm
  \retval false string does not represent a hash algorithm
*/
bool StringToHashAlg(char const* str, HashAlg* alg);

/// convert an Intel(R) EPID version to a string
/*!
\param[in] version an Intel(R) EPID version
\returns string representing the version
*/
char const* EpidVersionToString(EpidVersion version);

/// convert a string to an Intel(R) EPID version
/*!
\param[in] str a string
\param[out] version an Intel(R) EPID version
\retval true string represents an Intel(R) EPID version
\retval false string does not represent an Intel(R) EPID version
*/
bool StringToEpidVersion(char const* str, EpidVersion* version);

/// convert an Intel(R) EPID file type to a string
/*!
\param[in] type an Intel(R) EPID file type
\returns string representing the algorithm
*/
char const* EpidFileTypeToString(EpidFileType type);

/// convert a string to an Intel(R) EPID file type
/*!
\param[in] str a string
\param[out] type an Intel(R) EPID file type
\retval true string represents an Intel(R) EPID file type
\retval false string does not represent an Intel(R) EPID file type
*/
bool StringToEpidFileType(char const* str, EpidFileType* type);

/// set MemberParams to encapsulate different structure of Memparams
/// in case of TPM mode and none TPM mode
/*!
\param[in] rnd_func a Intel(R) EPID BitSupplier
\param[in] rnd_param a random parameters for BitSupplier
\param[in] f is Intel(R) EPID FpElemStr
\param[out] params is Intel(R) EPID MemberParams
\returns void
*/
void SetMemberParams(BitSupplier rnd_func, void* rnd_param, FpElemStr* f,
                     MemberParams* params);

#endif  // EXAMPLE_UTIL_CONVUTIL_H_
