/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly
 * prohibited.
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
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY
 * ACKNOWLEDGES THAT IT IS RECEIVER\'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY
 * THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK
 * SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO
 * RECEIVER\'S SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN
 * FORUM. RECEIVER\'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK\'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER
 * WILL BE, AT MEDIATEK\'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT
 * ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER
 * TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation
 * ("MediaTek Software") have been modified by MediaTek Inc. All revisions are
 * subject to any receiver\'s applicable license agreements with MediaTek Inc.
 */

#ifndef _IRQ_H_
#define _IRQ_H_

#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "irq_grp.h"
#include "intc.h"
#include "tinysys_reg.h"
/* Need to check MD register operation api */
#include "driver_api.h"


#define WORD_LEN                    32
#define INTC_WORD(irq)              ((irq) >> 5)
#define INTC_BIT(irq)               ((irq) & 0x1F)
#define INTC_WORD_OFS(word)         ((word) << 2)
#define INTC_GROUP_OFS(grp)         ((grp) << INTC_GRP_GAP)

#define INTC_IRQ_RAW_STA(word)      (INTC_IRQ_RAW_STA0 + INTC_WORD_OFS(word))
#define INTC_IRQ_STA(word)          (INTC_IRQ_STA0 + INTC_WORD_OFS(word))
#define INTC_IRQ_EN(word)           (INTC_IRQ_EN0 + INTC_WORD_OFS(word))
#define INTC_IRQ_WAKE_EN(word)      (INTC_IRQ_WAKE_EN0 + INTC_WORD_OFS(word))
#define INTC_IRQ_POL(word)          (INTC_IRQ_POL0 + INTC_WORD_OFS(word))
#define INTC_IRQ_GRP(grp, word)     (INTC_IRQ_GRP0_0 + INTC_GROUP_OFS(grp)\
				     + INTC_WORD_OFS(word))
#define INTC_IRQ_GRP_STA(grp, word) (INTC_IRQ_GRP0_STA0 + INTC_GROUP_OFS(grp)\
				     + INTC_WORD_OFS(word))

typedef unsigned int (*irq_handler_t)(void *);

struct intc_flags {
	unsigned int en[INTC_GRP_LEN];
	unsigned int grp[INTC_GRP_NUM][INTC_GRP_LEN];
};

void intc_irq_enable_by_wakeup(void);
int intc_irq_mask(struct intc_flags *flags);
int intc_irq_restore(struct intc_flags *flags);
int intc_irq_grp_mask(struct intc_flags *flags);
int intc_irq_grp_restore(struct intc_flags *flags);
int intc_irq_enable_from_ISR(struct INTC_IRQ *irq);
int intc_irq_enable(struct INTC_IRQ *irq);
int intc_irq_disable_from_ISR(struct INTC_IRQ *irq);
int intc_irq_disable(struct INTC_IRQ *irq);
int intc_irq_wakeup_set(struct INTC_IRQ *irq, unsigned int wake_src);
void intc_irq_clr_wakeup_all(void);
void intc_init(void);
int intc_irq_request(struct INTC_IRQ *irq, irq_handler_t handler,
		void *userdata);
void intc_isr_dispatch(unsigned int group);
extern UBaseType_t xGetCriticalNesting(void);
bool _is_in_isr(void);
bool _is_in_isr_cs(void);
uint32_t intc_get_mask_irq(struct INTC_IRQ *irq);

#ifdef CFG_IRQ_DEBUG_SUPPORT
void intc_wakeup_source_count(uint32_t wakeup_src);
void intc_irq_status_dump(void);
uint32_t irq_total_times(void);
void set_max_cs_limit(uint64_t limit_time);
void disable_cs_limit(void);
uint64_t get_max_cs_duration_time(void);
uint64_t get_max_cs_limit(void);
void set_irq_limit(uint32_t irq_limit_time);
#endif

#endif
