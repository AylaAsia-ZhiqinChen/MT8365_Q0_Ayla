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

#ifndef _IRQ_H_
#define _IRQ_H_

#include <stdio.h>
#include "intc.h"

#define INTC_GRP_NUM        15
#define INTC_GRP_HIGHEST    INTC_GRP_0


enum INTC_POL {
    INTC_POL_HIGH = 0x0,
    INTC_POL_LOW = 0x1,
    INTC_POL_NUM,
};

enum INTC_GROUP {
    INTC_GRP_0 = 0x0,
    INTC_GRP_1,
    INTC_GRP_2,
    INTC_GRP_3,
    INTC_GRP_4,
    INTC_GRP_5,
    INTC_GRP_6,
    INTC_GRP_7,
    INTC_GRP_8,
    INTC_GRP_9,
    INTC_GRP_10,
    INTC_GRP_11,
    INTC_GRP_12,
    INTC_GRP_13,
    INTC_GRP_14,
};


struct intc_flags {
    unsigned int en[INTC_GRP_LEN];
#if !INTC_IRQ_MASK_BY_EN
    unsigned int grp[INTC_GRP_NUM][INTC_GRP_LEN];
#endif
};

typedef unsigned int (*irq_handler_t)(void *);

int intc_irq_mask(struct intc_flags *flags);
int intc_irq_restore(struct intc_flags *flags);
int intc_irq_enable_from_ISR(unsigned int irq, unsigned int group, unsigned int pol);
int intc_irq_enable(unsigned int irq, unsigned int group, unsigned int pol);
int intc_irq_disable_from_ISR(unsigned int irq);
int intc_irq_disable(unsigned int irq);
int intc_irq_wakeup_set(unsigned int irq, unsigned int wake_src);
void intc_init(void);
int intc_irq_request(unsigned int irq, unsigned int group, unsigned int pol, irq_handler_t handler, void *userdata);

bool is_in_isr(void);

int _EXFUN(printfFromISR, (const char *__restrict, ...)
           _ATTRIBUTE((__format__(__printf__, 1, 2))));
#endif
