/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER\'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER\'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER\'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK\'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK\'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
 * applicable license agreements with MediaTek Inc.
 */

#ifndef _DRIVER_API_H_
#define _DRIVER_API_H_

#include "mt_driver_api.h"

#define ROUND_UP(a,b)   (((a) + ((b)-1)) & ~((b)-1))
#define ROUND_DOWN(a,b) ((a) & (~((b)-1)))

#define writel(addr, val)     *(volatile unsigned int *)(addr) = (val)
#define readl(addr)           *(volatile unsigned int *)(addr)
#define writeb(addr, val)     *(volatile unsigned char *)(addr) = (val)
#define readb(addr)           *(volatile unsigned char *)(addr)

/* I/O */
#define OUTREG32(a,b)   (*(volatile unsigned int *)(a) = (unsigned int)b)
#define INREG32(a)      (*(volatile unsigned int *)(a))

#ifndef DRV_WriteReg32
#define DRV_WriteReg32(addr,data)   ((*(volatile unsigned int *)(addr)) = (unsigned int)(data))
#endif
#ifndef DRV_Reg32
#define DRV_Reg32(addr)             (*(volatile unsigned int *)(addr))
#endif
#ifndef DRV_WriteReg16
#define DRV_WriteReg16(addr,data)   ((*(volatile unsigned short *)(addr)) = (unsigned short)(data))
#endif
#ifndef DRV_Reg16
#define DRV_Reg16(addr)             (*(volatile unsigned short *)(addr))
#endif
#define DRV_WriteReg8(addr,data)    ((*(volatile char *)(addr)) = (char)(data))
#define DRV_Reg8(addr)              (*(volatile char *)(addr))
#define DRV_SetReg32(addr, data)    ((*(volatile unsigned int *)(addr)) |= (unsigned int)(data))
#define DRV_ClrReg32(addr, data)    ((*(volatile unsigned int *)(addr)) &= ~((unsigned int)(data)))
#define DRV_SetReg8(addr, data)    ((*(volatile char *)(addr)) |= (char)(data))
#define DRV_ClrReg8(addr, data)    ((*(volatile char *)(addr)) &= ~((char)(data)))

/* I/O lowcase version */
#define outreg32(a,b)   (*(volatile unsigned int *)(a) = (unsigned int)(b))
#define inreg32(a)      (*(volatile unsigned int *)(a))

#ifndef drv_write_reg32
#define drv_write_reg32(addr,data)   ((*(volatile unsigned int *)(addr)) = (unsigned int)(data))
#endif
#ifndef drv_reg32
#define drv_reg32(addr)             (*(volatile unsigned int *)(addr))
#endif
#define drv_write_reg8(addr,data)    ((*(volatile char *)(addr)) = (char)(data))
#define drv_reg8(addr)              (*(volatile char *)(addr))
#define drv_set_reg32(addr, data)    ((*(volatile unsigned int *)(addr)) |= (unsigned int)(data))
#define drv_clr_reg32(addr, data)    ((*(volatile unsigned int *)(addr)) &= ~((unsigned int)(data)))
#define drv_set_reg8(addr, data)    ((*(volatile char *)(addr)) |= (char)(data))
#define drv_clr_reg8(addr, data)    ((*(volatile char *)(addr)) &= ~((char)(data)))

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

#ifndef bool
typedef unsigned char  bool;
#endif

#ifndef MAXIMUM
#define MAXIMUM(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MINIMUM
#define MINIMUM(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef UPPER_BOUND
#define UPPER_BOUND(in, up) ((in) > (up) ? (up) : (in))
#endif

#ifndef LOWER_BOUND
#define LOWER_BOUND(in, lo) ((in) < (lo) ? (lo) : (in))
#endif

#ifndef BOUNDED
#define BOUNDED(in, up, lo) ((in) > (up) ? (up) : (in) < (lo) ? (lo) : (in))
#endif

#define DSP_CORE0_ID	0
#define DSP_CORE1_ID	1

#endif
