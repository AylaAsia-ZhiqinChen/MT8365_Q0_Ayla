
/*
 * Copyright (C) 2018 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <linux/timer.h>
#include <linux/module.h>
#include <linux/pm_qos.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/workqueue.h>

#include "vpu_qos.h"
#include "vpu_cmn.h"
#include "vpu_dbg.h"

#define LINE_TAG LOG_DBG("[vpu][qos] %s %d\n", __func__, __LINE__)

//-----
int vpu_cmd_qos_start(int core)
{
	return 0;
}

int vpu_cmd_qos_end(int core)
{
	return 0;
}

int vpu_qos_counter_start(unsigned int core)
{
	return 0;
}

int vpu_qos_counter_end(unsigned int core)
{
	return 0;
}

int vpu_qos_counter_init(void)
{
	return 0;
}

void vpu_qos_counter_destroy(void)
{

}

