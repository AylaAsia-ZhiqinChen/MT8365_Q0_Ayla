/*******************************************************************************
* Copyright 2018 Intel Corporation
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
//     Intel(R) Integrated Performance Primitives. Cryptography Primitives.
//     Operations over GF(p).
// 
//     Context:
//        ippsGFpGetElementOctString()
// 
*/
#include "owndefs.h"
#include "owncp.h"

#include "pcpgfpstuff.h"
#include "pcpgfpxstuff.h"
#include "pcptool.h"


/*F*
// Name: ippsGFpGetElementOctString
//
// Purpose: Get GF Element to the octet string
//
// Returns:                   Reason:
//    ippStsNullPtrErr           NULL == pGFp
//                               NULL == pStr
//                               NULL == pA
//
//    ippStsContextMatchErr      invalid pGFp->idCtx
//                               invalid pA->idCtx
//                               invalid pR->idCtx
//
//    ippStsOutOfRangeErr        GFPE_ROOM() != GFP_FELEN()
//
//    ippStsSizeErr              !(0<lenA && lenA>=GFP_FELEN32(pGFE))
//
//    ippStsNoErr                no error
//
// Parameters:
//    pA       Pointer to the context of the finite field element.
//    pStr     Pointer to the octet string.
//    strSize  Size of the octet string buffer in bytes.
//    pGFp     Pointer to the context of the finite field.
//
*F*/

IPPFUN(IppStatus, ippsGFpGetElementOctString,(const IppsGFpElement* pA, Ipp8u* pStr, int strSize, IppsGFpState* pGFp))
{
   IPP_BAD_PTR3_RET(pStr, pA, pGFp);
   pGFp = (IppsGFpState*)( IPP_ALIGNED_PTR(pGFp, GFP_ALIGNMENT) );
   IPP_BADARG_RET( !GFP_TEST_ID(pGFp), ippStsContextMatchErr );
   IPP_BADARG_RET( !GFPE_TEST_ID(pA), ippStsContextMatchErr );
   IPP_BADARG_RET( 0>=strSize, ippStsSizeErr );
   {
      gsModEngine* pGFE = GFP_PMA(pGFp);
      IPP_BADARG_RET( GFPE_ROOM(pA)!=GFP_FELEN(pGFE), ippStsOutOfRangeErr);
      {
         gsModEngine* pBasicGFE = cpGFpBasic(pGFE);
         int basicDeg = cpGFpBasicDegreeExtension(pGFE);
         int basicElemLen = GFP_FELEN(pBasicGFE);
         int basicSize = BITS2WORD8_SIZE(BITSIZE_BNU(GFP_MODULUS(pBasicGFE),GFP_FELEN(pBasicGFE)));

         BNU_CHUNK_T* pDataElm = GFPE_DATA(pA);
         int deg, error;
         for(deg=0, error=0; deg<basicDeg && !error; deg++) {
            int size = IPP_MIN(strSize, basicSize);
            error = (NULL == cpGFpGetOctString(pStr, size, pDataElm, pBasicGFE));

            pDataElm += basicElemLen;
            pStr += size;
            strSize -= size;
         }

         return error ? ippStsSizeErr : ippStsNoErr;
      }
   }
}
