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

/* Definitions for the MATHFUN_V4 TIE package */

/* Do not modify. This is automatically generated.*/

#ifndef _XTENSA_MATHFUN_V4_HEADER
#define _XTENSA_MATHFUN_V4_HEADER

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

extern unsigned _TIE_MATHFUN_V4_LOG10_OP(unsigned x);
extern unsigned _TIE_MATHFUN_V4_POWER10_OP(unsigned x);
extern unsigned _TIE_MATHFUN_V4_SIGMOID_OP(unsigned x, unsigned q);
extern unsigned _TIE_MATHFUN_V4_TANH_OP(unsigned x);
extern unsigned _TIE_MATHFUN_V4_LN_OP(unsigned x);
extern unsigned _TIE_MATHFUN_V4_EXP_OP(unsigned x);
extern unsigned _TIE_MATHFUN_V4_EXPACC_OP(unsigned x);
extern unsigned _TIE_MATHFUN_V4_RECIP_OP(unsigned x);
extern unsigned _TIE_MATHFUN_V4_LOG2_OP(unsigned x);
extern unsigned _TIE_MATHFUN_V4_MFP1STATUS(void);
extern unsigned _TIE_MATHFUN_V4_MFP2STATUS(void);
extern unsigned _TIE_MATHFUN_V4_COS_OP(unsigned x);
extern unsigned _TIE_MATHFUN_V4_SIN_OP(unsigned x);
extern unsigned _TIE_MATHFUN_V4_SQRT_OP(unsigned x);
extern unsigned _TIE_MATHFUN_V4_RUR_qi(void);
extern void _TIE_MATHFUN_V4_WUR_qi(unsigned v);
extern unsigned _TIE_MATHFUN_V4_RUR_qo(void);
extern void _TIE_MATHFUN_V4_WUR_qo(unsigned v);
extern unsigned _TIE_MATHFUN_V4_RUR_qo_trig(void);
extern void _TIE_MATHFUN_V4_WUR_qo_trig(unsigned v);
extern unsigned _TIE_MATHFUN_V4_RUR_accreg_0(void);
extern void _TIE_MATHFUN_V4_WUR_accreg_0(unsigned v);
extern unsigned _TIE_MATHFUN_V4_RUR_accreg_1(void);
extern void _TIE_MATHFUN_V4_WUR_accreg_1(unsigned v);
extern short _TIE_MATHFUN_V4_SIGMOID(int x, signed char q);
extern int _TIE_MATHFUN_V4_LOG2(int x);
extern int _TIE_MATHFUN_V4_LOG10(int x);
extern int _TIE_MATHFUN_V4_POWER10(int x);
extern int _TIE_MATHFUN_V4_LN(int x);
extern int _TIE_MATHFUN_V4_EXPACC(int x);
extern int _TIE_MATHFUN_V4_EXP(int x);
extern short _TIE_MATHFUN_V4_COS(int x);
extern short _TIE_MATHFUN_V4_SIN(int x);
extern short _TIE_MATHFUN_V4_TANH(int x);
extern unsigned short _TIE_MATHFUN_V4_SQRT(int x);
extern int _TIE_MATHFUN_V4_RECIPROCAL(int x);

#endif /*__ASSEMBLER__*/
#endif /*_NOCLANGUAGE*/
#endif /*_ASMLANGUAGE*/

#define LOG10_OP _TIE_MATHFUN_V4_LOG10_OP
#define POWER10_OP _TIE_MATHFUN_V4_POWER10_OP
#define SIGMOID_OP _TIE_MATHFUN_V4_SIGMOID_OP
#define TANH_OP _TIE_MATHFUN_V4_TANH_OP
#define LN_OP _TIE_MATHFUN_V4_LN_OP
#define EXP_OP _TIE_MATHFUN_V4_EXP_OP
#define EXPACC_OP _TIE_MATHFUN_V4_EXPACC_OP
#define RECIP_OP _TIE_MATHFUN_V4_RECIP_OP
#define LOG2_OP _TIE_MATHFUN_V4_LOG2_OP
#define MFP1STATUS _TIE_MATHFUN_V4_MFP1STATUS
#define MFP2STATUS _TIE_MATHFUN_V4_MFP2STATUS
#define COS_OP _TIE_MATHFUN_V4_COS_OP
#define SIN_OP _TIE_MATHFUN_V4_SIN_OP
#define SQRT_OP _TIE_MATHFUN_V4_SQRT_OP
#define RUR_qi _TIE_MATHFUN_V4_RUR_qi
#define Rqi _TIE_MATHFUN_V4_RUR_qi
#define RUR0 _TIE_MATHFUN_V4_RUR_qi
#define WUR_qi _TIE_MATHFUN_V4_WUR_qi
#define Wqi _TIE_MATHFUN_V4_WUR_qi
#define WUR0 _TIE_MATHFUN_V4_WUR_qi
#define RUR_qo _TIE_MATHFUN_V4_RUR_qo
#define Rqo _TIE_MATHFUN_V4_RUR_qo
#define RUR1 _TIE_MATHFUN_V4_RUR_qo
#define WUR_qo _TIE_MATHFUN_V4_WUR_qo
#define Wqo _TIE_MATHFUN_V4_WUR_qo
#define WUR1 _TIE_MATHFUN_V4_WUR_qo
#define RUR_qo_trig _TIE_MATHFUN_V4_RUR_qo_trig
#define Rqo_trig _TIE_MATHFUN_V4_RUR_qo_trig
#define RUR2 _TIE_MATHFUN_V4_RUR_qo_trig
#define WUR_qo_trig _TIE_MATHFUN_V4_WUR_qo_trig
#define Wqo_trig _TIE_MATHFUN_V4_WUR_qo_trig
#define WUR2 _TIE_MATHFUN_V4_WUR_qo_trig
#define RUR_accreg_0 _TIE_MATHFUN_V4_RUR_accreg_0
#define Raccreg_0 _TIE_MATHFUN_V4_RUR_accreg_0
#define RUR3 _TIE_MATHFUN_V4_RUR_accreg_0
#define WUR_accreg_0 _TIE_MATHFUN_V4_WUR_accreg_0
#define Waccreg_0 _TIE_MATHFUN_V4_WUR_accreg_0
#define WUR3 _TIE_MATHFUN_V4_WUR_accreg_0
#define RUR_accreg_1 _TIE_MATHFUN_V4_RUR_accreg_1
#define Raccreg_1 _TIE_MATHFUN_V4_RUR_accreg_1
#define RUR4 _TIE_MATHFUN_V4_RUR_accreg_1
#define WUR_accreg_1 _TIE_MATHFUN_V4_WUR_accreg_1
#define Waccreg_1 _TIE_MATHFUN_V4_WUR_accreg_1
#define WUR4 _TIE_MATHFUN_V4_WUR_accreg_1
#define SIGMOID _TIE_MATHFUN_V4_SIGMOID
#define LOG2 _TIE_MATHFUN_V4_LOG2
#define LOG10 _TIE_MATHFUN_V4_LOG10
#define POWER10 _TIE_MATHFUN_V4_POWER10
#define LN _TIE_MATHFUN_V4_LN
#define EXPACC _TIE_MATHFUN_V4_EXPACC
#define EXP _TIE_MATHFUN_V4_EXP
#define COS _TIE_MATHFUN_V4_COS
#define SIN _TIE_MATHFUN_V4_SIN
#define TANH _TIE_MATHFUN_V4_TANH
#define SQRT _TIE_MATHFUN_V4_SQRT
#define RECIPROCAL _TIE_MATHFUN_V4_RECIPROCAL

#ifndef RUR
#define RUR(NUM) RUR##NUM()
#endif

#ifndef WUR
#define WUR(VAL, NUM) WUR##NUM(VAL)
#endif

#endif /* __XCC__ */

#endif /* __XTENSA__ */

#endif /* !_XTENSA_MATHFUN_V4_HEADER */
