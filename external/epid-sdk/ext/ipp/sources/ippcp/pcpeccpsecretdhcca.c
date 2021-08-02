/*******************************************************************************
* Copyright 2003-2018 Intel Corporation
* All Rights Reserved.
*
* If this  software was obtained  under the  Intel Simplified  Software License,
* the following terms apply:
*
* The source code,  information  and material  ("Material") contained  herein is
* owned by Intel Corporation or its  suppliers or licensors,  and  title to such
* Material remains with Intel  Corporation or its  suppliers or  licensors.  The
* Material  contains  proprietary  information  of  Intel or  its suppliers  and
* licensors.  The Material is protected by  worldwide copyright  laws and treaty
* provisions.  No part  of  the  Material   may  be  used,  copied,  reproduced,
* modified, published,  uploaded, posted, transmitted,  distributed or disclosed
* in any way without Intel's prior express written permission.  No license under
* any patent,  copyright or other  intellectual property rights  in the Material
* is granted to  or  conferred  upon  you,  either   expressly,  by implication,
* inducement,  estoppel  or  otherwise.  Any  license   under such  intellectual
* property rights must be express and approved by Intel in writing.
*
* Unless otherwise agreed by Intel in writing,  you may not remove or alter this
* notice or  any  other  notice   embedded  in  Materials  by  Intel  or Intel's
* suppliers or licensors in any way.
*
*
* If this  software  was obtained  under the  Apache License,  Version  2.0 (the
* "License"), the following terms apply:
*
* You may  not use this  file except  in compliance  with  the License.  You may
* obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
*
*
* Unless  required  by   applicable  law  or  agreed  to  in  writing,  software
* distributed under the License  is distributed  on an  "AS IS"  BASIS,  WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
* See the   License  for the   specific  language   governing   permissions  and
* limitations under the License.
*******************************************************************************/

/* 
// 
//  Purpose:
//     Cryptography Primitive.
//     EC over Prime Finite Field (EC Key Ecxhange, Diffie-Hellman version with cofactor)
// 
//  Contents:
//     ippsECCPSharedSecretDHC()
// 
// 
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcpeccp.h"


/*F*
//    Name: ippsECCPSharedSecretDHC
//
// Purpose: Shared Secret Value Derivation
//          (Diffie-Hellman version with cofactor).
//
// Returns:                   Reason:
//    ippStsNullPtrErr           NULL == pEC
//                               NULL == pPrivateA
//                               NULL == pPublicB
//                               NULL == pShare
//
//    ippStsContextMatchErr      illegal pEC->idCtx
//                               illegal pPrivateA->idCtx
//                               illegal pPublicB->idCtx
//                               illegal pShare->idCtx
//
//    ippStsRangeErr             not enough room for share key
//
//    ippStsShareKeyErr          (infinity) => z
//
//    ippStsNoErr                no errors
//
// Parameters:
//    pPrivateA   pointer to own   private key
//    pPublicB    pointer to alien public  key
//    pShare      pointer to the shared secret value
//    pEC        pointer to the ECCP context
//
*F*/
IPPFUN(IppStatus, ippsECCPSharedSecretDHC,(const IppsBigNumState* pPrivateA,
                                           const IppsECCPPointState* pPublicB,
                                           IppsBigNumState* pShare,
                                           IppsECCPState* pEC))
{
   IPP_BAD_PTR1_RET(pEC);
   /* use aligned EC context */
   pEC = (IppsGFpECState*)( IPP_ALIGNED_PTR(pEC, ECGFP_ALIGNMENT) );
   IPP_BADARG_RET(!ECP_TEST_ID(pEC), ippStsContextMatchErr);

   return ippsGFpECSharedSecretDHC(pPrivateA, pPublicB, pShare, pEC, (Ipp8u*)ECP_SBUFFER(pEC));
}
