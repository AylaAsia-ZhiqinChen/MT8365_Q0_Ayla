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
 * WILL BE, AT MEDIATEK\'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE
 * AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
 * RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation
 * ("MediaTek Software") have been modified by MediaTek Inc. All revisions are
 * subject to any receiver\'s applicable license agreements with MediaTek Inc.
 */

#include "irq.h"
#include "task.h"
#include "mt_printf.h"
#ifdef CFG_XGPT_SUPPORT
#include <xgpt.h>
#endif
#include <tinysys_reg.h>

struct irq_descr_t {
	irq_handler_t handler;
	void *userdata;
#ifdef CFG_IRQ_DEBUG_SUPPORT
	uint32_t irq_count;
	uint32_t wakeup_count;
	uint64_t last_enter;
	uint64_t last_exit;
	uint64_t max_duration;
#endif
};

static struct irq_status_t {
	int group;
	int id;
} irq_status;

static struct irq_descr_t intc_irq_desc[INTC_IRQ_NUM];
static uint32_t stamp_count_enter;
static uint32_t stamp_count_exit;

#ifdef CFG_IRQ_DEBUG_SUPPORT
void __real_vTaskEnterCritical(void);
void __real_vTaskExitCritical(void);
static uint64_t start_cs_time;
static uint64_t end_cs_time;
static uint64_t max_cs_duration_time;
static uint64_t cs_duration_time;
static uint64_t max_cs_limit;
static uint64_t max_irq_limit;
extern UBaseType_t uxCriticalNesting;
extern uint32_t tinysys_current_ipi_id;
extern uint32_t tinysys_max_duration_ipc_id;
#endif

extern void vic_reset_priority(void);

/* os timer reg value * 77ns
 * 1  ms: 12987
 * 1.5ms: 19436
 * 2  ms: 25974
 * 3  ms: 38961
 */
#define IRQ_DURATION_LIMIT      38960	/* 3ms */

void intc_irq_enable_by_wakeup(void)
{
	unsigned int reg_en, reg_wk, word, mask;
	int irq = 0;

	for (irq = 0; irq < INTC_IRQ_NUM; irq++) {
		word = INTC_WORD(irq);
		mask = (1 << INTC_BIT(irq));

		reg_en = INTC_IRQ_EN(word);
		reg_wk = INTC_IRQ_WAKE_EN(word);
		if (DRV_Reg32(reg_wk) & mask)
			DRV_SetReg32(reg_en, mask);
	}

}

int intc_irq_mask(struct intc_flags *flags)
{
	unsigned int word, reg_en;

	if (flags == NULL)
		return -1;

	for (word = 0; word < INTC_GRP_LEN; word++) {
		reg_en = INTC_IRQ_EN(word);
		flags->en[word] = DRV_Reg32(reg_en);
		DRV_WriteReg32(reg_en, 0x0);
	}

	return 0;
}

int intc_irq_restore(struct intc_flags *flags)
{
	unsigned int word, reg_en;

	if (flags == NULL)
		return -1;

	for (word = 0; word < INTC_GRP_LEN; word++) {
		reg_en = INTC_IRQ_EN(word);
		DRV_WriteReg32(reg_en, flags->en[word]);
	}

	return 0;
}

int intc_irq_grp_mask(struct intc_flags *flags)
{
	unsigned int word, grp, reg_grp;

	if (flags == NULL)
		return -1;

	for (word = 0; word < INTC_GRP_LEN; word++) {
		for (grp = 0; grp < INTC_GRP_NUM; grp++) {
			reg_grp = INTC_IRQ_GRP(grp, word);
			flags->grp[grp][word] = DRV_Reg32(reg_grp);
			DRV_WriteReg32(reg_grp, 0x0);
		}
	}

	return 0;
}

int intc_irq_grp_restore(struct intc_flags *flags)
{
	unsigned int word, grp, reg_grp;

	if (flags == NULL)
		return -1;

	for (word = 0; word < INTC_GRP_LEN; word++) {
		for (grp = 0; grp < INTC_GRP_NUM; grp++) {
			reg_grp = INTC_IRQ_GRP(grp, word);
			DRV_WriteReg32(reg_grp, flags->grp[grp][word]);
		}
	}

	return 0;
}

static int _intc_irq_enable(unsigned int id, unsigned int group,
			    unsigned int pol)
{
	unsigned int word, mask, reg;

	if (id >= INTC_IRQ_NUM)
		return -1;
	if (group >= INTC_GRP_NUM)
		return -1;
	if (pol >= INTC_POL_NUM)
		return -1;

	word = INTC_WORD(id);
	mask = (1 << INTC_BIT(id));

	/* disable interrupt */
	reg = INTC_IRQ_EN(word);
	DRV_ClrReg32(reg, mask);

	/* set polarity */
	reg = INTC_IRQ_POL(word);

	if (pol == INTC_POL_HIGH)
		DRV_ClrReg32(reg, mask);
	else
		DRV_SetReg32(reg, mask);

	/* set group */
	reg = INTC_IRQ_GRP(group, word);
	DRV_SetReg32(reg, mask);

	/* enable interrupt */
	reg = INTC_IRQ_EN(word);
	DRV_SetReg32(reg, mask);

	return 0;
}

int intc_irq_enable_from_ISR(struct INTC_IRQ *irq)
{
	unsigned int ulSavedInterruptMask;
	int ret;

	ulSavedInterruptMask = portSET_INTERRUPT_MASK_FROM_ISR();
	ret = _intc_irq_enable(irq->id, irq->group, irq->pol);
	portCLEAR_INTERRUPT_MASK_FROM_ISR(ulSavedInterruptMask);

	return ret;
}

int intc_irq_enable(struct INTC_IRQ *irq)
{
	int ret;

	taskENTER_CRITICAL();
	ret = _intc_irq_enable(irq->id, irq->group, irq->pol);
	taskEXIT_CRITICAL();

	return ret;
}

static int _intc_irq_disable(unsigned int id)
{
	unsigned int word, mask, reg;

	if (id >= INTC_IRQ_NUM)
		return -1;

	word = INTC_WORD(id);
	mask = (1 << INTC_BIT(id));

	/* remove wakeup src */
	reg = INTC_IRQ_WAKE_EN(word);
	DRV_ClrReg32(reg, mask);

	/* disable interrupt */
	reg = INTC_IRQ_EN(word);
	DRV_ClrReg32(reg, mask);

	return 0;
}

int intc_irq_disable_from_ISR(struct INTC_IRQ *irq)
{
	unsigned int ulSavedInterruptMask;
	int ret;

	ulSavedInterruptMask = portSET_INTERRUPT_MASK_FROM_ISR();
	ret = _intc_irq_disable(irq->id);
	portCLEAR_INTERRUPT_MASK_FROM_ISR(ulSavedInterruptMask);

	return ret;
}

int intc_irq_disable(struct INTC_IRQ *irq)
{
	int ret;

	taskENTER_CRITICAL();
	ret = _intc_irq_disable(irq->id);
	taskEXIT_CRITICAL();

	return ret;
}

int intc_irq_wakeup_set(struct INTC_IRQ *irq, unsigned int wake_src)
{
	unsigned int reg, word, mask;

	if (irq->id >= INTC_IRQ_NUM)
		return -1;

	taskENTER_CRITICAL();

	word = INTC_WORD(irq->id);
	mask = (1 << INTC_BIT(irq->id));

	reg = INTC_IRQ_WAKE_EN(word);

	if (wake_src)
		DRV_SetReg32(reg, mask);
	else
		DRV_ClrReg32(reg, mask);

	taskEXIT_CRITICAL();

	return 0;
}

#if 0
static int _intc_irq_remove_from_group(unsigned int group, unsigned int id)
{
	unsigned int mask, word;

	if (id >= INTC_IRQ_NUM)
		return -1;
	if (group >= INTC_GRP_NUM)
		return -1;

	word = INTC_WORD(id);
	mask = (1 << INTC_BIT(id));

	DRV_ClrReg32(INTC_IRQ_GRP(group, word), mask);

	return 0;
}

static int _intc_irq_remove_handler(unsigned int id)
{
	if (id >= INTC_IRQ_NUM)
		return -1;

	intc_irq_desc[id].handler = NULL;

	return 0;
}

static int _intc_get_pending_grp(void)
{
	unsigned int val, group;

	val = DRV_Reg32(INTC_IRQ_OUT);
	if (val == 0)
		return -1;

	for (group = 0; group < INTC_GRP_NUM; group++) {
		if (val & 0x1)
			break;
		val >>= 1;
	}

	return group;
}
#endif

static void _intc_clr_grp(unsigned int group)
{
	unsigned int val;

	val = (1 << group);
	DRV_WriteReg32(INTC_IRQ_CLR_TRG, val);
}

void intc_irq_clr_wakeup_all(void)
{
	unsigned int reg, word;

	taskENTER_CRITICAL();

	for (word = 0; word < INTC_GRP_LEN; word++) {
		reg = INTC_IRQ_WAKE_EN(word);
		DRV_WriteReg32(reg, 0x0);
	}

	taskEXIT_CRITICAL();
}

static int _intc_get_pending_irq(unsigned int group)
{
	int word, id;
	unsigned int reg, group_sta, irq_sta;

	if (group >= INTC_GRP_NUM)
		return -1;

	for (word = (INTC_GRP_LEN - 1); word >= 0; word--) {
		reg = INTC_IRQ_GRP_STA(group, word);
		group_sta = DRV_Reg32(reg);

		if (group_sta == 0)
			continue;

		reg = INTC_IRQ_STA(word);
		irq_sta = DRV_Reg32(reg);

		irq_sta &= group_sta;

		if (irq_sta != 0)
			break;
	}

	if (word < 0)
		return -1;

	id = (int)(WORD_LEN - 1 - clz(irq_sta));
	id += WORD_LEN * word;

	return id;
}

static void _intc_irq_status_record(int id, int group)
{
	irq_status.group = group;
	irq_status.id = id;
}

void intc_init(void)
{
	unsigned int word, group, id;
	taskENTER_CRITICAL();

	stamp_count_enter = 0;
	stamp_count_exit = 0;
#ifdef CFG_IRQ_DEBUG_SUPPORT
	/* Initial static global variables here */
	start_cs_time = 0;
	end_cs_time = 0;
	max_cs_duration_time = 0;
	cs_duration_time = 0;
	max_cs_limit = 0;
	max_irq_limit = 0;
#endif

	for (id = 0; id < INTC_IRQ_NUM; id++) {
		intc_irq_desc[id].handler = NULL;
#ifdef CFG_IRQ_DEBUG_SUPPORT
		intc_irq_desc[id].wakeup_count = 0;
		intc_irq_desc[id].irq_count = 0;
		intc_irq_desc[id].last_enter = 0;
		intc_irq_desc[id].last_exit = 0;
		intc_irq_desc[id].max_duration = 0;
#endif
	}

	for (word = 0; word < INTC_GRP_LEN; word++) {
		DRV_WriteReg32(INTC_IRQ_EN(word), 0x0);
		DRV_WriteReg32(INTC_IRQ_WAKE_EN(word), 0x0);
		DRV_WriteReg32(INTC_IRQ_POL(word), 0x0);
	}

	for (group = 0; group < INTC_GRP_NUM; group++) {
		for (word = 0; word < INTC_GRP_LEN; word++)
			DRV_WriteReg32(INTC_IRQ_GRP(group, word), 0x0);
	}
	/* reset INTC register */
	_intc_irq_status_record(-1, -1);
	taskEXIT_CRITICAL();
}

int intc_irq_request(struct INTC_IRQ *irq, irq_handler_t handler,
			void *userdata)
{
	struct irq_descr_t *irq_descr;

	if (irq->id >= INTC_IRQ_NUM)
		return -1;
	if (irq->group >= INTC_GRP_NUM)
		return -1;
	if (irq->pol >= INTC_POL_NUM)
		return -1;

	irq_descr = &intc_irq_desc[irq->id];

	taskENTER_CRITICAL();
	if (irq_descr->handler)
		goto fail;

	irq_descr->handler = handler;
	irq_descr->userdata = userdata;

	if (intc_irq_enable(irq) != 0) {
		irq_descr->handler = NULL;
		goto fail;
	}
	taskEXIT_CRITICAL();

	return 0;

fail:
	taskEXIT_CRITICAL();

	return -1;
}

void intc_isr_dispatch(unsigned int group)
{
	unsigned int ret;
	unsigned int ulSavedInterruptMask;
	int id;
	irq_handler_t handler;
	void *userdata;
#ifdef CFG_IRQ_DEBUG_SUPPORT
	uint64_t duration;
#endif

	ulSavedInterruptMask = portSET_INTERRUPT_MASK_FROM_ISR();

	if (group >= INTC_GRP_NUM) {
		portCLEAR_INTERRUPT_MASK_FROM_ISR_AND_LEAVE(ulSavedInterruptMask);
		return;
	}

	id = _intc_get_pending_irq(group);
	if (id >= 0) {
		handler = intc_irq_desc[id].handler;
		userdata = intc_irq_desc[id].userdata;
		if (handler != NULL) {
			_intc_irq_status_record(id, group);
#ifdef CFG_IRQ_DEBUG_SUPPORT
			intc_irq_desc[id].irq_count++;
#ifdef CFG_XGPT_SUPPORT
			intc_irq_desc[id].last_enter = read_xgpt_stamp_ns();
#endif
#endif
			stamp_count_enter++;

			ret = handler(userdata);

			stamp_count_exit++;
#ifdef CFG_IRQ_DEBUG_SUPPORT
#ifdef CFG_XGPT_SUPPORT
			intc_irq_desc[id].last_exit = read_xgpt_stamp_ns();
#endif
			duration = intc_irq_desc[id].last_exit
				   - intc_irq_desc[id].last_enter;

			/* handle the xgpt overflow case
			 * discard the duration time when exit time < enter time
			 */
			if (intc_irq_desc[id].last_exit
			    > intc_irq_desc[id].last_enter
			    && (duration > intc_irq_desc[id].max_duration)) {
				intc_irq_desc[id].max_duration = duration;
#if defined(CFG_XGPT_SUPPORT) && defined(CFG_IRQ_DEBUG_SUPPORT)
				/* monitor irq duration over limit
				 * read_xgpt_stamp_ns() return OS timer
				 */
				if ((duration > max_irq_limit)
				    && (max_irq_limit != 0)) {
					PRINTF_E("ERR IRQ%d: duration %llu > %llu\n", id, max_irq_limit, duration);
#ifdef CFG_IRQ_MONITOR_SUPPORT
					configASSERT(0);
#endif
				}
#endif
			}
#endif
		} else
			PRINTF_E("No IRQ#%d handler\n", id);
	}

	_intc_irq_status_record(-1, -1);
	_intc_clr_grp(group);
	vic_reset_priority();
	portCLEAR_INTERRUPT_MASK_FROM_ISR_AND_LEAVE(ulSavedInterruptMask);
}

bool _is_in_isr(void)
{
	return (irq_status.id >= 0);
}

bool _is_in_isr_cs(void)
{
	return (irq_status.id >= 0 || xGetCriticalNesting() > 0);
}

static uint32_t _intc_get_mask_irq(unsigned int id)
{
	unsigned int word, mask, reg;

	if (id >= INTC_IRQ_NUM)
		return 0;

	word = INTC_WORD(id);
	mask = (1 << INTC_BIT(id));
	reg = INTC_IRQ_EN(word);

	return (DRV_Reg32(reg) & mask);
}

uint32_t intc_get_mask_irq(struct INTC_IRQ *irq)
{
	unsigned int ret;

	taskENTER_CRITICAL();
	ret = _intc_get_mask_irq(irq->id);
	taskEXIT_CRITICAL();

	return ret;
}

#ifdef CFG_IRQ_DEBUG_SUPPORT
void intc_wakeup_source_count(uint32_t wakeup_src)
{
	unsigned int id;

	for (id = 0; id < INTC_IRQ_NUM; id++)
		if (wakeup_src & (1 << id))
			intc_irq_desc[id].wakeup_count++;
}

void intc_irq_status_dump(void)
{
	int index;
	unsigned int id, group, word, val, grp_stat;

	PRINTF_E("group\tirq\tcount\tenable\tpending\tactive\t\tlast\n");
	for (group = 0; group < INTC_GRP_NUM; group++) {
		for (word = 0; word < INTC_GRP_LEN; word++) {
			grp_stat = DRV_Reg32(INTC_IRQ_GRP(group, word));
			for (index = 0; index < WORD_LEN; index++) {
				val = grp_stat >> index;
				if (val == 0)
					break;
				else if (val & 0x1) {
					id = index + WORD_LEN * word;
					PRINTF_E("%d\t\t%u\t%u\t%s\t%s\t%s\t%lld\n",
						group,
						id,
						intc_irq_desc[id].irq_count,
						_intc_get_mask_irq(id) ? "enable" : "disable",
						((int)id == _intc_get_pending_irq(group)) ? "yes" : "no",
						(irq_status.id == (int)id && irq_status.group == (int)group) ? "active" : "inactive",
						intc_irq_desc[id].last_exit);
				}
			}
		}
	}
}

uint32_t irq_total_times(void)
{
	uint32_t id;
	uint32_t num = 0;

	for (id = 0; id < INTC_IRQ_NUM; id++)
		num += (int)intc_irq_desc[id].irq_count;

	return num;
}

void set_max_cs_limit(uint64_t limit_time)
{
	if (max_cs_limit == 0)
		max_cs_limit = limit_time;
	else {
		PRINTF_D("set_max_cs_limit(%llu) failed\n", limit_time);
		PRINTF_D("Already set max context switch limit:%llu,\nPlease run disable_cs_limit() to disiable it first\n",
			max_cs_limit);
	}
}

void disable_cs_limit(void)
{
	max_cs_limit = 0;
}

void __wrap_vTaskEnterCritical(void);
void __wrap_vTaskEnterCritical(void)
{
	__real_vTaskEnterCritical();
	if (xGetCriticalNesting() == 1 && max_cs_limit != 0) {
#ifdef CFG_XGPT_SUPPORT
		start_cs_time = read_xgpt_stamp_ns();
#endif
	}
}

void __wrap_vTaskExitCritical(void);
void __wrap_vTaskExitCritical(void)
{
	if ((xGetCriticalNesting() - 1) == 0 && max_cs_limit != 0) {
#ifdef CFG_XGPT_SUPPORT
		end_cs_time = read_xgpt_stamp_ns();
#endif
		if (end_cs_time > start_cs_time) {
			cs_duration_time = end_cs_time - start_cs_time;
			if (cs_duration_time > max_cs_duration_time)
				max_cs_duration_time = cs_duration_time;
			if (max_cs_duration_time > max_cs_limit) {
				PRINTF_D("violate the critical section time limit:%llu>%llu\n",
					max_cs_duration_time, max_cs_limit);
				configASSERT(0);
			}
		}
	}
	__real_vTaskExitCritical();
}

uint64_t get_max_cs_duration_time(void)
{
	return max_cs_duration_time;
}

uint64_t get_max_cs_limit(void)
{
	return max_cs_limit;
}

void set_irq_limit(uint32_t irq_limit_time)
{
	if (irq_limit_time == 0)
		max_irq_limit = 0;
	else
		max_irq_limit = irq_limit_time;
}
#endif
