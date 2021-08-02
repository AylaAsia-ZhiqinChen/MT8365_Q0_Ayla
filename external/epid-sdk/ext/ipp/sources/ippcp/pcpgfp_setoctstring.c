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
//     Internal operations over prime GF(p).
//
//     Context:
//        cpGFpSetOctString
//
*/
#include "owncp.h"

#include "pcpbn.h"
#include "pcpgfpstuff.h"

//tbcd: temporary excluded: #include <assert.h>

BNU_CHUNK_T* cpGFpSetOctString(BNU_CHUNK_T* pElm, const Ipp8u* pStr, int strSize, gsModEngine* pGFE)
{
   int elemLen = GFP_FELEN(pGFE);

   if((int)(elemLen*sizeof(BNU_CHUNK_T)) < strSize)
      return NULL;

   {
      BNU_CHUNK_T* pTmp = cpGFpGetPool(1, pGFE);
      //tbcd: temporary excluded: assert(pTmp !=NULL);
      {
         int nsTmp = cpFromOctStr_BNU(pTmp, pStr, strSize);
         BNU_CHUNK_T* ret = cpGFpSet(pElm, pTmp, nsTmp, pGFE);

         cpGFpReleasePool(1, pGFE);
         return ret==NULL? NULL : pElm;
      }
   }
}
