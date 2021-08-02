/*
 * Customer ID=13943; Build=0x75f5e; Copyright (c) 2009-2010 Tensilica Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef	_SETJMP_H
#define	_SETJMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <xtensa/config/system.h>

#if __XTENSA_WINDOWED_ABI__

/* The jmp_buf structure for Xtensa windowed ABI holds the following
   (where "proc" is the procedure that calls setjmp): 4-12 registers
   from the window of proc, the 4 words from the save area at proc's $sp
   (in case a subsequent alloca in proc moves $sp), and the return
   address within proc.  Everything else is saved on the stack in the
   normal save areas.  The jmp_buf structure is:
  
        struct jmp_buf {
            int regs[12];
            int save[4];
            void *return_address;
        }
  
   See the setjmp code for details.  */

#define _JBLEN          17      /* 12 + 4 + 1 */

#else /* __XTENSA_CALL0_ABI__ */

#define _JBLEN          6       /* a0, a1, a12, a13, a14, a15 */

#endif /* __XTENSA_CALL0_ABI__ */

typedef struct __jmp_buf
{
  int buf[_JBLEN];
} jmp_buf[1];

typedef struct __sigjmp_buf
{
  int buf[_JBLEN];
} sigjmp_buf[1];

void longjmp(jmp_buf, int);
void siglongjmp(sigjmp_buf, int);
void _longjmp(jmp_buf, int);

int setjmp(jmp_buf);
int sigsetjmp(sigjmp_buf, int);
int _setjmp(jmp_buf);
#define setjmp(env) setjmp(env)

#ifdef __cplusplus
}
#endif
#endif
