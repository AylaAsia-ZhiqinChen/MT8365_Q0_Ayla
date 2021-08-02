/*
* Copyright (c) 2015 MediaTek Inc.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files
* (the "Software"), to deal in the Software without restriction,
* including without limitation the rights to use, copy, modify, merge,
* publish, distribute, sublicense, and/or sell copies of the Software,
* and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "platform/tz_utils.h"

#ifndef __raw_readw
static inline u16 __raw_readw(const volatile void __iomem *addr)
{
    return *(const volatile u16 __force *) addr;
}
#endif

#ifndef __raw_readl
static inline u32 __raw_readl(const volatile void __iomem *addr)
{
    return *(const volatile u32 __force *) addr;
}
#endif

#ifndef __raw_writew
static inline void __raw_writew(u16 b, volatile void __iomem *addr)
{
    *(volatile u16 __force *) addr = b;
}
#endif

#ifndef __raw_writel
static inline void __raw_writel(u32 b, volatile void __iomem *addr)
{
    *(volatile u32 __force *) addr = b;
}
#endif

static unsigned int tz_uffs(unsigned int x)
{
    return __builtin_ffs(x);
}

void tz_set_field(volatile u32 *reg, u32 field, u32 val)
{
    u32 tv = (u32)*reg;
    tv &= ~(field);
    tv |= ((val) << (tz_uffs((unsigned int)field) - 1));
    *reg = tv;
}

void tz_get_field(volatile u32 *reg, u32 field, u32 *val)
{
    u32 tv = (u32)*reg;
    *val = ((tv & (field)) >> (tz_uffs((unsigned int)field) - 1));
}

int tz_get_limit_cpunr(void)
{
#ifdef MTEE_CPUNR
    return MTEE_CPUNR;
#else
    return 0;
#endif
}
