/*******************************************************************************
* Copyright 2017-2018 Intel Corporation
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
//     Cryptography Primitive. Modular Arithmetic Engine. General Functionality
// 
//  Contents:
//        gsPackModEngineCtx()
//
*/

#include "owndefs.h"
#include "owncp.h"
#include "pcpbnumisc.h"
#include "pcpbnuarith.h"
#include "gsmodstuff.h"
#include "pcptool.h"

/*
// Pack/Unpack methods
*/
void gsPackModEngineCtx(const gsModEngine* pCtx, Ipp8u* pBuffer)
{
   gsModEngine* pAlignedBuffer = (gsModEngine*)pBuffer;

   /* max modulus length */
   int modSize = MOD_LEN(pCtx);
   /* size of context (bytes) without cube and pool buffers */
   int ctxSize = sizeof(gsModEngine)
                +sizeof(BNU_CHUNK_T)*(modSize*3);

   CopyBlock(pCtx, pAlignedBuffer, ctxSize);
   MOD_MODULUS(pAlignedBuffer) = (BNU_CHUNK_T*)((Ipp8u*)NULL + IPP_UINT_PTR(MOD_MODULUS(pCtx))-IPP_UINT_PTR(pCtx));
   MOD_MNT_R(pAlignedBuffer)   = (BNU_CHUNK_T*)((Ipp8u*)NULL + IPP_UINT_PTR(MOD_MNT_R(pCtx))-IPP_UINT_PTR(pCtx));
   MOD_MNT_R2(pAlignedBuffer)  = (BNU_CHUNK_T*)((Ipp8u*)NULL + IPP_UINT_PTR(MOD_MNT_R2(pCtx))-IPP_UINT_PTR(pCtx));
}
