/*******************************************************************************
* Copyright 2016-2018 Intel Corporation
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
//     Security Hash Standard
//     Generalized Functionality
// 
//  Contents:
//        ippsHashDuplicate_rmf()
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcphash_rmf.h"
#include "pcptool.h"

/*F*
//    Name: ippsHashDuplicate_rmf
//
// Purpose: Clone Hash context.
//
// Returns:                Reason:
//    ippStsNullPtrErr        pSrcState == NULL
//                            pDstState == NULL
//    ippStsContextMatchErr   pSrcState->idCtx != idCtxHash
//                            pDstState->idCtx != idCtxHash
//    ippStsNoErr             no errors
//
// Parameters:
//    pSrcState  pointer to the source Hash context
//    pDstState  pointer to the target Hash context
//
// Note:
//    pDstState may to be uninitialized by ippsHashInit_rmf()
//
*F*/
IPPFUN(IppStatus, ippsHashDuplicate_rmf,(const IppsHashState_rmf* pSrcState, IppsHashState_rmf* pDstState))
{
   /* test state pointers */
   IPP_BAD_PTR2_RET(pSrcState, pDstState);
   pSrcState = (IppsHashState_rmf*)( IPP_ALIGNED_PTR(pSrcState, HASH_ALIGNMENT) );
   pDstState = (IppsHashState_rmf*)( IPP_ALIGNED_PTR(pDstState, HASH_ALIGNMENT) );
   /* test states ID */
   IPP_BADARG_RET(idCtxHash !=HASH_CTX_ID(pSrcState), ippStsContextMatchErr);

   /* copy state */
   CopyBlock(pSrcState, pDstState, sizeof(IppsHashState_rmf));
   return ippStsNoErr;
}
