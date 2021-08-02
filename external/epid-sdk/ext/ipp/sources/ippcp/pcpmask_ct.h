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
// 
//  Purpose:
//     Cryptography Primitive.
//     Constant time Mask operations
// 
// 
*/

#if !defined(_PCP_MASK_CT_H)
#define _PCP_MASK_CT_H

#include "owncp.h"
#include "pcpbnuimpl.h"

/*
// The following functions test particular conditions
// and returns either 0 or 0xffffffff.
//
// The result is suitable for boolean and masked operations.
//
// Inspite of operation below are using BNU_CHUNK_T operand(s) it can be applied to Ipp32u, Ipp32s, Ipp16u, Ipp16s, Ipp8u and Ipp8s too.
// For example, if
//    Ipp32u uns_int;
//    Ipp32s sgn_int;
//    Ipp8u  uns_char;
//    Ipp8s  sgn_char;
// then
//    cpIs_msb_ct((Ipp32s)uns_int)     tests 31 bit of uns_int
//    cpIs_msb_ct(        sgn_int)     tests 31 bit of sgn_int
//    cpIs_msb_ct((Ipp8u)uns_char)     tests  7 bit of uns_char
//    cpIs_msb_ct(       sgn_char)     tests  7 bit of sgn_char
*/

/* tests if MSB(a)==1 */
__INLINE BNU_CHUNK_T cpIsMsb_ct(BNU_CHUNK_T a)
{
   return (BNU_CHUNK_T)0 - (a >> (sizeof(a) * 8 - 1));
}

/* tests if LSB(a)==1 */
__INLINE BNU_CHUNK_T cpIsLsb_ct(BNU_CHUNK_T a)
{
   return (BNU_CHUNK_T)0 - (a & 1);
}

/* tests if a is odd */
__INLINE BNU_CHUNK_T cpIsOdd_ct(BNU_CHUNK_T a)
{
   return cpIsLsb_ct(a);
}

/* tests if a is even */
__INLINE BNU_CHUNK_T cpIsEven_ct(BNU_CHUNK_T a)
{
   return ~cpIsLsb_ct(a);
}

/* tests if a==0 */
__INLINE BNU_CHUNK_T cpIsZero_ct(BNU_CHUNK_T a)
{
   return cpIsMsb_ct(~a & (a - 1));
}

/* tests if a==b */
__INLINE BNU_CHUNK_T cpIsEqu_ct(BNU_CHUNK_T a, BNU_CHUNK_T b)
{
   return cpIsZero_ct(a ^ b);
}

/* replace under mask: dst[] = replaceFlag? src[] : dst[] */
__INLINE void cpMaskedReplace_ct(BNU_CHUNK_T* dst, const BNU_CHUNK_T* src, int len, BNU_CHUNK_T replaceMask)
{
   BNU_CHUNK_T dstMask = ~replaceMask;
   int n;
   for(n=0; n<len; n++)
      dst[n] = (src[n] & replaceMask) ^  (dst[n] & dstMask);
}

/* copy under mask: dst[] = src1[] & mask) ^ src2[] & ~mask  */
__INLINE void cpMaskedCopyBNU_ct(BNU_CHUNK_T* dst, BNU_CHUNK_T mask, const BNU_CHUNK_T* src1, const BNU_CHUNK_T* src2, int len)
{
   int i;
   for(i=0; i<(len); i++)
      dst[i] = (src1[i] & mask) ^ (src2[i] & ~mask);
}

/* test if GF elmement is equal to x chunk */
__INLINE BNU_CHUNK_T cpIsGFpElemEquChunk_ct(const BNU_CHUNK_T* pE, int nsE, BNU_CHUNK_T x)
{
   int i;
   BNU_CHUNK_T accum = pE[0] ^ x;
   for (i = 1; i < nsE; i++) {
    accum |= pE[i];
   }
   return cpIsZero_ct(accum);
}

#define GFPE_IS_ZERO_CT(a,size)  cpIsGFpElemEquChunk_ct((a),(size), 0)

#endif /* _PCP_MASK_CT_H */
