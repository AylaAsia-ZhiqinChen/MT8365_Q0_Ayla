/*
 * Copyright (C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#ifndef __MTKTIMER_H__
#define __MTKTIMER_H__

#include <sys/types.h>

#define SEJ_BASE		0x10008000
#define SEJ_VA_CON		((volatile uint32_t *)((uintptr_t)SEJ_VA_BASE + 0x0000))
#define SEJ_APXGPT1_CON		((volatile uint32_t *)((uintptr_t)SEJ_VA_BASE + 0x0220))
#define SEJ_APXGPT1_PRESCALE	((volatile uint32_t *)((uintptr_t)SEJ_VA_BASE + 0x0224))
#define SEJ_APXGPT1_COUNTER_0	((volatile uint32_t *)((uintptr_t)SEJ_VA_BASE + 0x0228))
#define SEJ_APXGPT1_COUNTER_1	((volatile uint32_t *)((uintptr_t)SEJ_VA_BASE + 0x022C))

#define MTKTIMER_PORT		"com.mediatek.mtktimer"
#define MAX_MSG_SIZE		1024

enum mtktimer_cmd {
	GET_MTKTIMER = 0,
	GET_MTKTIMER_DONE = 1,
};

struct mtktimer_msg {
	uint32_t cmd;
	uint64_t timer;
};

#endif
