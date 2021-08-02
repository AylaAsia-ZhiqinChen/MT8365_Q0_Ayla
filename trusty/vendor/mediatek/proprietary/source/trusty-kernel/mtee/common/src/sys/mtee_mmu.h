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

#ifndef __MTEE_SYS_MMU_H__
#define __MTEE_SYS_MMU_H__

#ifdef __aarch64__

#define DESC_TABLE_NS_BIT  (1l<<63)
#define DESC_TABLE_AP_SHIFT (61)
#define DESC_TABLE_UXN_BIT (1l<<60)
#define DESC_TABLE_PXN_BIT (1l<<59)
#define DESC_VALID_BIT     (1l)
#define DESC_TABLE_BLOCK   (0l<<1)
#define DESC_TABLE_PAGE    (1l<<1)
#define DESC_UXN_BIT       (1l<<54)
#define DESC_PXN_BIT       (1l<<53)
#define DESC_CONTIG_BIT    (1l<<52)
#define DESC_ACCESS_BIT    (1l<<10)
#define DESC_NOSHR_BIT     (0x0l<<8)
#define DESC_INNSHR_BIT    (0x3l<<8)
#define DESC_INSHR_BIT     (0x3l<<8)
#define DESC_ACEL1RW_BIT   (0x0l<<6)
#define DESC_ACEL1RO_BIT   (0x2l<<6)
#define DESC_ACEL0RW_BIT   (0x1l<<6)
#define DESC_ACEL0RO_BIT   (0x3l<<6)
#define DESC_NS_BIT        (1l<<5)
#define DESC_ATTRIDX(i)    ((i & 0x7)<<2)

        // Share, Read/write at PL1, Normal Memory, Inner/outer cachable, write-back/write-allocate
#define PROT_SECURE_MEMORY       (DESC_VALID_BIT|DESC_ACCESS_BIT|DESC_INNSHR_BIT|DESC_ACEL1RW_BIT|DESC_ATTRIDX(4))
        // Share, Read-only at PL1, Normal Memory, Inner/outer cachable, write-back/write-allocate
#define PROT_SECURE_MEMORY_RO    (DESC_VALID_BIT|DESC_ACCESS_BIT|DESC_INNSHR_BIT|DESC_ACEL1RO_BIT|DESC_ATTRIDX(4))
        // +NS/XN
#define PROT_NORMAL_MEMORY       ((PROT_SECURE_MEMORY)|DESC_NS_BIT|DESC_UXN_BIT|DESC_PXN_BIT)
        // +NS/XN
#define PROT_NORMAL_MEMORY_RO    ((PROT_SECURE_MEMORY_RO)|DESC_NS_BIT|DESC_UXN_BIT|DESC_PXN_BIT)
        // Share, Read/write at PL1, Device, non-cachable.
#define PROT_SECURE_DEVICE       (DESC_VALID_BIT|DESC_UXN_BIT|DESC_PXN_BIT|DESC_ACCESS_BIT|DESC_INNSHR_BIT|DESC_ACEL1RW_BIT|DESC_ATTRIDX(0))
        // +NS
#define PROT_NORMAL_DEVICE       ((PROT_SECURE_DEVICE)|DESC_NS_BIT)
        // Clear virtual memory mapping
#define PROT_NO_ACCESS           0

#else
        // Share, Read/write at PL1, Normal Memory, Inner/outer cachable, write-back/write-allocate
#define PROT_SECURE_MEMORY       ((1<<16)|(1<<10)|(1<<12)|(3<<2)|(1<<5))
        // Share, Read-only at PL1, Normal Memory, Inner/outer cachable, write-back/write-allocate
#define PROT_SECURE_MEMORY_RO    ((1<<16)|(1<<15)|(1<<10)|(1<<12)|(3<<2)|(1<<5))
        // +NS/Client/XN
#define PROT_NORMAL_MEMORY       ((PROT_SECURE_MEMORY)|(8<<5)|(1<<4)|(1<<19))
        // +NS/Client/XN/RO
#define PROT_NORMAL_MEMORY_RO    ((PROT_SECURE_MEMORY_RO)|(8<<5)|(1<<4)|(1<<19))
        // Share, Read/write at PL1, Device, non-cachable, XN.
#define PROT_SECURE_DEVICE       ((1<<16)|(1<<10)|(0<<12)|(1<<2)|(1<<4)|(1<<5))
        // +NS/Client/XN
#define PROT_NORMAL_DEVICE       ((PROT_SECURE_DEVICE)|(8<<5)|(1<<4)|(1<<19))
        // Clear virtual memory mapping
#define PROT_NO_ACCESS           0

#endif /* __aarch64__ */

#define SZ_1K              (0x400)
#define SZ_4K              (0x1000)
#define SZ_64K             (0x10000)
#define SZ_128K            (0x20000)
#define SZ_256K            (0x40000)
#define SZ_1M              (0x100000)
#define SZ_2M              (0x200000)
#define SZ_4M              (0x400000)
#define SZ_16M             (0x1000000)


struct MTEE_MMUMaps {
    void *virt_addr;
    unsigned long phys_addr;
    unsigned long prot;
    unsigned long size;
};

int MTEE_SetMMUTable(void *virt_addr, unsigned long phys_addr, unsigned long size, unsigned long prot);
int MTEE_SetMMUMaps(const struct MTEE_MMUMaps *maps, int num);

#endif /* __MTEE_SYS_MMU_H__ */
