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

/* Definitions for the xt_timer TIE package */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_xt_timer_HEADER
#define _XTENSA_xt_timer_HEADER

#ifdef __XTENSA__
#ifdef __XCC__

#ifndef _ASMLANGUAGE
#ifndef _NOCLANGUAGE
#ifndef __ASSEMBLER__

#include <xtensa/tie/xt_core.h>

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

extern unsigned _TIE_xt_timer_RSR_CCOUNT(void);
extern void _TIE_xt_timer_WSR_CCOUNT(unsigned art);
extern void _TIE_xt_timer_XSR_CCOUNT(unsigned art /*inout*/);
extern unsigned _TIE_xt_timer_RSR_CCOMPARE0(void);
extern void _TIE_xt_timer_WSR_CCOMPARE0(unsigned art);
extern void _TIE_xt_timer_XSR_CCOMPARE0(unsigned art /*inout*/);
extern unsigned _TIE_xt_timer_RSR_CCOMPARE1(void);
extern void _TIE_xt_timer_WSR_CCOMPARE1(unsigned art);
extern void _TIE_xt_timer_XSR_CCOMPARE1(unsigned art /*inout*/);
extern unsigned _TIE_xt_timer_RSR_CCOMPARE2(void);
extern void _TIE_xt_timer_WSR_CCOMPARE2(unsigned art);
extern void _TIE_xt_timer_XSR_CCOMPARE2(unsigned art /*inout*/);

#endif /*__ASSEMBLER__*/
#endif /*_NOCLANGUAGE*/
#endif /*_ASMLANGUAGE*/

#define XT_RSR_CCOUNT _TIE_xt_timer_RSR_CCOUNT
#define XT_WSR_CCOUNT _TIE_xt_timer_WSR_CCOUNT
#define XT_XSR_CCOUNT _TIE_xt_timer_XSR_CCOUNT
#define XT_RSR_CCOMPARE0 _TIE_xt_timer_RSR_CCOMPARE0
#define XT_WSR_CCOMPARE0 _TIE_xt_timer_WSR_CCOMPARE0
#define XT_XSR_CCOMPARE0 _TIE_xt_timer_XSR_CCOMPARE0
#define XT_RSR_CCOMPARE1 _TIE_xt_timer_RSR_CCOMPARE1
#define XT_WSR_CCOMPARE1 _TIE_xt_timer_WSR_CCOMPARE1
#define XT_XSR_CCOMPARE1 _TIE_xt_timer_XSR_CCOMPARE1
#define XT_RSR_CCOMPARE2 _TIE_xt_timer_RSR_CCOMPARE2
#define XT_WSR_CCOMPARE2 _TIE_xt_timer_WSR_CCOMPARE2
#define XT_XSR_CCOMPARE2 _TIE_xt_timer_XSR_CCOMPARE2

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_xt_timer_HEADER */
