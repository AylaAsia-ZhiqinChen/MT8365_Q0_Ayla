// Customer ID=13943; Build=0x75f5e; Copyright (c) 2017 Cadence Design Systems, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

/* Definitions for the xt_booleans TIE package */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_booleans_HEADER
#define _XTENSA_xt_booleans_HEADER

#ifdef __XTENSA__
#ifdef __XCC__

#ifndef _ASMLANGUAGE
#ifndef _NOCLANGUAGE
#ifndef __ASSEMBLER__

#include <xtensa/tie/xt_core.h>
typedef _TIE_xtbool xtbool;
typedef _TIE_xtbool2 xtbool2;
typedef _TIE_xtbool4 xtbool4;
typedef _TIE_xtbool8 xtbool8;
typedef _TIE_xtbool16 xtbool16;

/*
 * The following prototypes describe intrinsic functions
 * corresponding to TIE instructions.  Some TIE instructions
 * may produce multiple results (designated as "out" operands
 * in the iclass section) or may have operands used as both
 * inputs and outputs (designated as "inout").  However, the C
 * and C++ languages do not provide syntax that can express
 * the in/out/inout constraints of TIE intrinsics.
 * Nevertheless, the compiler understands these constraints
 * and will check that the intrinsic functions are used
 * correctly.  To improve the readability of these prototypes,
 * the "out" and "inout" parameters are marked accordingly
 * with comments.
 */

extern xtbool _TIE_xt_booleans_XORB(xtbool bs, xtbool bt);
extern xtbool _TIE_xt_booleans_ORBC(xtbool bs, xtbool bt);
extern xtbool _TIE_xt_booleans_ORB(xtbool bs, xtbool bt);
extern xtbool _TIE_xt_booleans_ANDBC(xtbool bs, xtbool bt);
extern xtbool _TIE_xt_booleans_ANDB(xtbool bs, xtbool bt);
extern xtbool _TIE_xt_booleans_ALL4(xtbool4 bs4);
extern xtbool _TIE_xt_booleans_ANY4(xtbool4 bs4);
extern xtbool _TIE_xt_booleans_ALL8(xtbool8 bs8);
extern xtbool _TIE_xt_booleans_ANY8(xtbool8 bs8);
extern void _TIE_xt_booleans_MOVT(unsigned arr /*inout*/, unsigned ars, xtbool bt);
extern void _TIE_xt_booleans_MOVF(unsigned arr /*inout*/, unsigned ars, xtbool bt);

#endif /*__ASSEMBLER__*/
#endif /*_NOCLANGUAGE*/
#endif /*_ASMLANGUAGE*/

#define XT_XORB _TIE_xt_booleans_XORB
#define XT_ORBC _TIE_xt_booleans_ORBC
#define XT_ORB _TIE_xt_booleans_ORB
#define XT_ANDBC _TIE_xt_booleans_ANDBC
#define XT_ANDB _TIE_xt_booleans_ANDB
#define XT_ALL4 _TIE_xt_booleans_ALL4
#define XT_ANY4 _TIE_xt_booleans_ANY4
#define XT_ALL8 _TIE_xt_booleans_ALL8
#define XT_ANY8 _TIE_xt_booleans_ANY8
#define XT_MOVT _TIE_xt_booleans_MOVT
#define XT_MOVF _TIE_xt_booleans_MOVF

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_booleans_HEADER */
