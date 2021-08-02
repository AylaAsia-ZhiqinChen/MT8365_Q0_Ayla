/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*
* MediaTek Inc. (C) 2018. All rights reserved.
*  Created on: 2018/6/5
*      Author: mtk06743
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
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver\'s
* applicable license agreements with MediaTek Inc.
*/
#ifndef _DSP_INTERRUPT_H_
#define _DSP_INTERRUPT_H_

#include "FreeRTOS.h"
#include <task.h>
#include <xtensa_api.h>
#include <driver_api.h>
#include <mt_reg_base.h>
#include <main.h>


/*
 *
 *	platform irq iniformation
 *
 */
#define IRQ_TYPE_EDGE_RISING		0x00000001
#define IRQ_TYPE_EDGE_FALLING		0x00000002
#define IRQ_TYPE_LEVEL_HIGH			0x00000004
#define IRQ_TYPE_LEVEL_LOW			0x00000008

//int level
#define IRQ_INVALID				0xff
#define IRQ_MASK				0xff
#define EXT_MASK				0x3F0
#define GET_INTERRUPT_ID(n)		(n & 0xff)
#define LEVEL_SHFIT				8
#define LEVEL_MASK				0xf
#define INT_LEVEL(n)			(n << LEVEL_SHFIT)
#define GET_INTLEVEL(irq)		((irq >> LEVEL_SHFIT) & 0xf)
#define IRQ_LEVEL0				0
#define IRQ_LEVEL1				1

/*
	IRQ = 0xcc-dd
		cc = int level
		dd = irq
	LX_MODULEX_IRQX_B	= ( (0xcc << 8 ) | 0xdd )
	All irq dispatch to level0 default
*/
#define INTERRUPT_ID(ID)	(ID)

#define L1_INT_IRQ_B		(INT_LEVEL(1) | IRQ_INVALID)
#define L1_DSP_TIMER_IRQ0_B	(INT_LEVEL(2) | INTERRUPT_ID(0))
#define L1_DSP_TIMER_IRQ1_B	(INT_LEVEL(3) | INTERRUPT_ID(1))
#define L1_DSP_TIMER_IRQ2_B	(INT_LEVEL(4) | INTERRUPT_ID(2))
#define L1_DSP_TIMER_IRQ3_B	(INT_LEVEL(5) | INTERRUPT_ID(3))

#define LX_CQDMA_IRQ0_B 	(INT_LEVEL(1) | INTERRUPT_ID(4))
#define LX_CQDMA_IRQ1_B 	(INT_LEVEL(1) | INTERRUPT_ID(5))
#define LX_CQDMA_IRQ2_B 	(INT_LEVEL(1) | INTERRUPT_ID(6))
#define LX_UART_IRQ_B		(INT_LEVEL(1) | INTERRUPT_ID(7))
#define LX_AFE_IRQ_B		(INT_LEVEL(1) | INTERRUPT_ID(8))
#define LX_MCU_IRQ_B		(INT_LEVEL(1) | INTERRUPT_ID(9))

#define MAX_IRQ_NUM			11
#define MAX_INT_LEVEL		24

#define AUDIO_IRQn          LX_AFE_IRQ_B

/*
 *		irq register information
 *
 */
#define	DSP_IRQ_POL			(DSP_RG_INT_POL_CTL0)
#define	DSP_IRQ_EN			(DSP_RG_INT_EN_CTL0)
#define	DSP_IRQ_LEVEL		(DSP_RG_INT_LV_CTL0)
#define	DSP_IRQ_STATUS		(DSP_RG_INT_STATUS0)


typedef void(*irq_handler_t)(void);

struct irq_desc_t {
    irq_handler_t handler;
    const char  *name;
    uint32_t priority;
    uint32_t irq_count;
    uint32_t wakeup_count;
    uint32_t init_count;
    uint64_t last_enter;
    uint64_t last_exit;
    uint64_t max_duration;
};
#ifdef __cplusplus
extern "C" {
#endif

void irq_init(void);
void request_irq(uint32_t irq, irq_handler_t handler, const char *name);
void free_irq(uint32_t irq);
void unmask_irq(uint32_t irq);
void mask_irq(uint32_t irq);
void irq_status_dump(void);

void set_irq_limit(uint32_t irq_limit_time);
uint64_t get_max_cs_duration_time(void);
uint64_t get_max_cs_limit(void);
void wakeup_source_count(uint32_t wakeup_src);

uint32_t is_in_isr(void) __attribute__((always_inline));

#ifdef __cplusplus
}
#endif
#endif
