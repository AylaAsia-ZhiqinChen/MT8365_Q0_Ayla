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
#include "FreeRTOS.h"
#include <xtensa_rtos.h>
#include <interrupt.h>
#include <task.h>
#include <stdio.h>

uint64_t max_irq_limitm = 0;
uint32_t stamp_count_enterm = 0;
uint32_t stamp_count_exitm = 0;

/* Bitmask of interrupts configured at level n or below*/
extern const unsigned int Xthal_intlevel_andbelow_mask[XTHAL_MAX_INTLEVELS];

struct irq_desc_t irq_desc[MAX_IRQ_NUM];
/* os timer reg value * 77ns  //13M os timer
 * 1  ms: 12987* 1.5ms: 19436
 * 2  ms: 25974* 3  ms: 38961
 */
#define IRQ_DURATION_LIMIT      38960  //3ms

static void mt_ext_init(void);
static int mt_ext_register_isr(uint32_t inner_irq,
				irq_handler_t handler, const char *args);
static void mt_ext_deregister_isr(uint32_t inner_irq);

void hw_isr_dispatch(void *arg);

static uint32_t get_irq_status(uint32_t irq);
static uint32_t get_mask_irq(uint32_t irq);
uint32_t is_in_isr(void)
{
    unsigned int intenable = 0, interrupt = 0, int_level = 0, mask = 0xffffffff;
    __asm__ __volatile__("rsr     %0, intenable" : "=a"(intenable));
    __asm__ __volatile__("rsr     %0, interrupt" : "=a"(interrupt));
    __asm__ __volatile__("rsr     %0, PS" : "=a"(int_level));

    int_level &= 0xf; // PS[0:3] is INTLEVEL
    if (int_level)
        mask = ~Xthal_intlevel_andbelow_mask[int_level - 1];

    return (intenable & interrupt & mask) ? 1 : 0;
}

/** get irq mask status
*
*  @param irq interrupt number
*
*  @returns
*    irq interrupt status
*/
static uint32_t get_irq_status(uint32_t irq)
{
	uint32_t irq_status;
	uint32_t level, in_irq;

	level = GET_INTLEVEL(irq);
	in_irq = GET_INTERRUPT_ID(irq);

	switch(level)
	{
		case 1:
		case 9:
			break;

		default :
			irq_status = drv_reg32(DSP_IRQ_STATUS);
			break;
	}

	irq_status &= (0x1 << in_irq);
	if (irq_status)
		return 1;
	else
		return 0;
}

/** get irq mask status
*
*  @param irq interrupt number
*
*  @returns
*    irq mask status
*/
static uint32_t get_mask_irq(uint32_t irq)
{
	uint32_t irq_mask;
	uint32_t level, in_irq;

	level = GET_INTLEVEL(irq);
	in_irq = GET_INTERRUPT_ID(irq);

	switch(level)
	{
		case 1:
		case 9:
			break;

		default :
			irq_mask = drv_reg32(DSP_IRQ_EN);
			break;
	}

	irq_mask &= (0x1 << in_irq);
	if (irq_mask)
		return 1;
	else
		return 0;

}

static void mt_ext_mask_module(int32_t in_irq)
{
	int32_t mask;

	in_irq &= 0x1f;
	mask = (0x1 << in_irq);
	drv_clr_reg32(DSP_IRQ_EN, mask);
}

static void mt_ext_unmask_module(int32_t in_irq)
{
	int32_t mask;

	in_irq &= 0x1f;
	mask = (0x1 << in_irq);
	drv_set_reg32(DSP_IRQ_EN, mask);
}

/** enable irq
*
*  @param irq interrupt number
*
*  @returns
*    no return
*/
void unmask_irq(uint32_t irq)
{
	uint32_t level, in_irq;

	level = GET_INTLEVEL(irq);
	in_irq = GET_INTERRUPT_ID(irq);
	if (level > MAX_INT_LEVEL) {
		PRINTF("ERR Int level %u\n\r", level);
		return;
	}

	switch (level)
	{
		case 1:
		case 9:
			mt_ext_unmask_module(in_irq);
			break;

		default :
			if (in_irq > MAX_IRQ_NUM) {
				PRINTF("ERR Int irq %u\n\r", in_irq);
				return;
			}
			mt_ext_unmask_module(in_irq);
			xt_ints_on(1 << level);
			break;
	}
}


/** disable irq
*
*  @param irq interrupt number
*
*  @returns
*    no return
*/
void mask_irq(uint32_t irq)
{
	uint32_t level, in_irq;

	level = GET_INTLEVEL(irq);
	in_irq = GET_INTERRUPT_ID(irq);
	if (level > MAX_INT_LEVEL) {
		PRINTF("ERR Int level %u\n\r", level);
		return;
	}

	switch (level)
	{
		case 1:
		case 9:
			mt_ext_mask_module(in_irq);
			break;

		default :
			if (in_irq > MAX_IRQ_NUM) {
				PRINTF("ERR Int irq %u\n\r", in_irq);
				return;
			}
			mt_ext_mask_module(in_irq);
			xt_ints_off(1 << level);
			break;
	}
}

/** register a irq handler
*
*  @param irq interrupt number
*  @param irq interrupt handler
*  @param irq interrupt name
*
*  @returns
*    no return
*/
void request_irq(uint32_t irq, irq_handler_t handler, const char *name)
{
	uint32_t level, in_irq;

	level = GET_INTLEVEL(irq);
	in_irq = GET_INTERRUPT_ID(irq);
	if (level > MAX_INT_LEVEL) {
		PRINTF("ERR Int level %u\n\r", level);
		return;
	}

	switch (level)
	{
		case 1:
		case 9:
			mt_ext_register_isr(in_irq, handler, name);
			mt_ext_unmask_module(in_irq);
			break;

		default :
			if (in_irq > MAX_IRQ_NUM ||	\
				(irq_desc[level].handler)) {
				PRINTF("ERR Int irq %u\n\r", in_irq);
				return;
			}
			xt_ints_off(1 << level);
			mt_ext_mask_module(in_irq);

			irq_desc[level].name = name;
			irq_desc[level].handler = handler;
			irq_desc[level].init_count ++;

			mt_ext_unmask_module(in_irq);
			xt_ints_on(1 << level);
			break;
	}
}

/** free a irq handler
*
*  @param irq interrupt number
*
*  @returns
*    no return
*/
void free_irq(uint32_t irq)
{
	uint32_t level, in_irq;

	level = GET_INTLEVEL(irq);
	in_irq = GET_INTERRUPT_ID(irq);
	if (level > MAX_INT_LEVEL) {
		PRINTF("ERR Int level %u\n\r", level);
		return;
	}

	switch (level)
	{
		case 1:
		case 9:
			mt_ext_deregister_isr(in_irq);
			mt_ext_mask_module(in_irq);
			break;

		default :
			if (in_irq > MAX_IRQ_NUM) {
				PRINTF("ERR Int irq %u\n\r", in_irq);
				return;
			}
			xt_ints_off(1 << level);
			mt_ext_mask_module(in_irq);
			irq_desc[level].name = NULL;
			irq_desc[level].handler = NULL;
			break;
	}
}

/** init irq handler
*  @returns
*    no return
*/
NORMAL_SECTION_FUNC void irq_init(void)
{
	uint32_t in_irq;
	for (in_irq = 0; in_irq < MAX_IRQ_NUM; in_irq++) {
		xt_ints_off(1 << in_irq);                   //sw disable irq
		mt_ext_mask_module(in_irq);		//hw disable irq
		irq_desc[in_irq].name = "";
		irq_desc[in_irq].handler = NULL;
		irq_desc[in_irq].priority = 0;
		irq_desc[in_irq].irq_count = 0;
		irq_desc[in_irq].wakeup_count = 0;
		irq_desc[in_irq].init_count = 0;
		irq_desc[in_irq].last_enter = 0;
		irq_desc[in_irq].last_exit = 0;
		irq_desc[in_irq].max_duration = 0;
		xt_set_interrupt_handler(in_irq, hw_isr_dispatch,
				(void *)in_irq); //default go hw_isr_dispatcher
	}

	mt_ext_init();
}


/** interrupt handler entry & dispatcher
*
*  @param
*
*  @returns
*    no return
*/
void hw_isr_dispatch(void *arg)
{
    uint32_t ulCurrentInterrupt;
    uint64_t duration;

    /* Obtain the number of the currently executing interrupt. */
    ulCurrentInterrupt = (uint32_t)arg;
#ifdef LIANG_UNMARK
    traceISR_ENTER();
#endif
    //PRINTF("Interrupt ID %lu\n", ulCurrentInterrupt);
    if (ulCurrentInterrupt < MAX_IRQ_NUM) {
        if (irq_desc[ulCurrentInterrupt].handler) {
            irq_desc[ulCurrentInterrupt].irq_count ++;
            /* record the last handled interrupt duration, unit: (ns)*/
            stamp_count_enterm++;

            irq_desc[ulCurrentInterrupt].handler();

            stamp_count_exitm++;
            duration = irq_desc[ulCurrentInterrupt].last_exit -
                       irq_desc[ulCurrentInterrupt].last_enter;
            /* handle the xgpt overflow case
            * discard the duration time when exit time < enter time
            * */
            if (irq_desc[ulCurrentInterrupt].last_exit >
                irq_desc[ulCurrentInterrupt].last_enter)
                if (duration > irq_desc[ulCurrentInterrupt].max_duration) {
                    irq_desc[ulCurrentInterrupt].max_duration = duration;
                }
#ifdef CFG_IRQ_MONITOR_SUPPORT
	configASSERT(x)(0);
#endif
        }
        else {
            PRINTF("IRQ ID %u handler null\n\r", ulCurrentInterrupt);
        }
    }
    else {
        PRINTF("ERR IRQ ID %u\n\r", ulCurrentInterrupt);
    }
#ifdef LIANG_UNMARK
    traceISR_EXIT();
#endif
}

/** interrupt status query function
*
*  @param
*
*  @returns
*    no return
*/
void irq_status_dump(void)
{
    int32_t id;

    PRINTF_E("id\tname\tpriority(HW)\tcount\tlast\tenable\tactive\n\r");
    for (id = 0; id < MAX_IRQ_NUM; id++) {
        if (irq_desc[id].handler) {
            PRINTF_E("%d\t%s\t%d\t%d\t%llu\t%s\t%s\n\r",
                     id,
                     (irq_desc[id].name) ? irq_desc[id].name : "n/a",
                     irq_desc[id].priority,
                     irq_desc[id].irq_count,
                     irq_desc[id].last_exit,
                     get_mask_irq(id) ? "enable" : "disable",
                     get_irq_status(id) ? "enable" : "inactive");
        }
    }
}


/*----------------------------------------------------------------------------
 *
 *	peripheral irq handler
 *---------------------------------------------------------------------------*/
static struct irq_desc_t ext_isr[MAX_IRQ_NUM];

static int mt_ext_register_isr(uint32_t inner_irq, irq_handler_t handler, const char *args)
{
	if(inner_irq >= MAX_IRQ_NUM)
		return -1;

	ext_isr[inner_irq].handler = handler;
	ext_isr[inner_irq].name = args;
	return 0;
}

static void mt_ext_deregister_isr(uint32_t inner_irq)
{
	ext_isr[inner_irq].handler = NULL;
	ext_isr[inner_irq].name = NULL;
}

void mt_set_intlevel(uint32_t irq, int tlevel)
{
	uint32_t intlvl, in_irq;

	intlvl = GET_INTLEVEL(irq);
	in_irq = GET_INTERRUPT_ID(irq);
	if (intlvl > MAX_INT_LEVEL) {
		PRINTF("ERR Int level %u\n\r", intlvl);
		return;
	}

	if (tlevel) {
		xt_ints_off(1 << intlvl);
		drv_set_reg32(DSP_IRQ_LEVEL, 1 << in_irq);
		xt_ints_on(1 << (intlvl + 8));
	} else {
		xt_ints_off(1 << (intlvl + 8));
		drv_clr_reg32(DSP_IRQ_LEVEL, 1 << in_irq);
		xt_ints_on(1 << intlvl);
	}
}

static void  mt_ext_handle_domain_irq(void *args)
{
	int i;
	unsigned int mask;

	mask = drv_reg32(DSP_IRQ_STATUS);
	mask &= EXT_MASK;

	for (i = 0; i < MAX_IRQ_NUM; i++) {
		if ((1 << i) & mask) {
			if (ext_isr[i].handler) {
				ext_isr[i].handler();
			} else {
				PRINTF_E("mask no handler irq: %d\n", i);
				mt_ext_mask_module(i);
			}
		}
	}
}

NORMAL_SECTION_FUNC static void mt_ext_init(void)
{
	uint32_t level;

	level = GET_INTLEVEL(L1_INT_IRQ_B);
	xt_set_interrupt_handler(level, mt_ext_handle_domain_irq, (void *)level);

	xt_ints_on(1 << level);
}

uint64_t max_cs_duration_time = 0;
uint64_t max_cs_limit = 0;
uint64_t max_irq_limit = 0;
uint64_t get_max_cs_duration_time(void)
{
	return max_cs_duration_time;
}

uint64_t get_max_cs_limit(void)
{
	return max_cs_limit;
}

NORMAL_SECTION_FUNC void set_irq_limit(uint32_t irq_limit_time)
{
	if (irq_limit_time == 0) {
		max_irq_limit = 0;
	} else {
		max_irq_limit = IRQ_DURATION_LIMIT;
	}
}
