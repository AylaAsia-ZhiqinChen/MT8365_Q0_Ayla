/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2019. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */
/*
 * MTK PMIF Driver
 *
 * Copyright 2019 MediaTek Co.,Ltd.
 *
 * DESCRIPTION:
 *     This file provides API for other drivers to access PMIC registers
 *
 */

#include <stdio.h>
#include <debug.h>
#include <FreeRTOS.h>
#include <task.h>
#include "driver_api.h"
#include "tinysys_reg.h"
#include <ispeed.h>
#include <spmi.h>
#include <pmif.h>
#include <pmif_sw.h>

#define GET_SWINF_0_FSM(x)       ((x>>1)  & 0x00000007)
#define TIMEOUT_WAIT_IDLE        (0xFF)

static int pmif_spmi_read_cmd(struct pmif *arb, unsigned char opc,
		unsigned char sid, unsigned short addr, unsigned char *buf,
		unsigned short len);
static int pmif_spmi_write_cmd(struct pmif *arb, unsigned char opc,
	unsigned char sid, unsigned short addr, const unsigned char *buf,
	unsigned short len);
#if PMIF_NO_PMIC
static int pmif_spmi_read_cmd(struct pmif *arb, unsigned char opc,
		unsigned char sid, unsigned short addr, unsigned char *buf,
		unsigned short len)
{
	PRINTF_I("%s do Nothing.\n", __func__);
	return 0;
}

static int pmif_spmi_write_cmd(struct pmif *arb, unsigned char opc,
	unsigned char sid, unsigned short addr, const unsigned char *buf,
	unsigned short len)
{
	PRINTF_I("%s do Nothing.\n", __func__);
	return 0;
}

/* init interface */
int pmif_spmi_init(void)
{
	PRINTF_I("%s do Nothing.\n", __func__);
	return 0;
}
#else /* #ifdef PMIF_NO_PMIC */
static struct pmif pmif_spmi_arb[] = {
	{
		.read_cmd = pmif_spmi_read_cmd,
		.write_cmd = pmif_spmi_write_cmd,
		.is_pmif_init_done = is_pmif_spmi_init_done,
	},
};

/* pmif internal API declaration */
static unsigned int pmif_get_current_time(void);
static int pmif_timeout_ns(unsigned int start_time,
	unsigned int elapse_time);
static unsigned int pmif_time2ns(unsigned int time_us);
int pmif_spmi_init(void);

/* pmif timeout */
#if PMIF_TIMEOUT
static unsigned int pmif_get_current_time(void)
{
    portTickType tick_in_ns = (1000 / portTICK_RATE_MS) * 1000;
    return (xTaskGetTickCount() * tick_in_ns);
}

static int pmif_timeout_ns(unsigned int start_time,
	unsigned int elapse_time)
{
    unsigned int cur_time = 0 ;
    portTickType tick_in_ns = (1000 / portTICK_RATE_MS) * 1000;
    cur_time = (xTaskGetTickCount() * tick_in_ns);
    return (cur_time > (start_time + elapse_time));
}

static unsigned int pmif_time2ns(unsigned int time_us)
{
    return (time_us * 1000);
}

#else
static unsigned int pmif_get_current_time(void)
{
	return 0;
}
static int pmif_timeout_ns(unsigned int start_time,
	unsigned int elapse_time)
{
	return 0;
}

static unsigned int pmif_time2ns(unsigned int time_us)
{
	return 0;
}
#endif
static inline unsigned int pmif_check_idle(void) {
	unsigned long long start_time_ns = 0, end_time_ns = 0, timeout_ns = 0;
	unsigned int reg_rdata;

	start_time_ns = pmif_get_current_time();
	timeout_ns = pmif_time2ns(TIMEOUT_WAIT_IDLE);

	do {
		if (pmif_timeout_ns(start_time_ns, timeout_ns)) {
			end_time_ns = pmif_get_current_time();
			PRINTF_E("%s timeout %d %d\n", __func__, start_time_ns,
					end_time_ns - start_time_ns);
			return -3;
	        }
		reg_rdata = DRV_Reg32(PMIF_STA);
	} while(GET_SWINF_0_FSM(reg_rdata) != SWINF_FSM_IDLE);

    return 0;
}

static inline unsigned int pmif_check_vldclr(void) {
	unsigned long long start_time_ns = 0, end_time_ns = 0, timeout_ns = 0;
	unsigned int reg_rdata;

	start_time_ns = pmif_get_current_time();
	timeout_ns = pmif_time2ns(TIMEOUT_WAIT_IDLE);

	do {
		if (pmif_timeout_ns(start_time_ns, timeout_ns)) {
			end_time_ns = pmif_get_current_time();
			PRINTF_E("%s timeout %d %d\n", __func__, start_time_ns,
					end_time_ns - start_time_ns);
			return -3;
	        }
		reg_rdata = DRV_Reg32(PMIF_STA);
	} while(GET_SWINF_0_FSM(reg_rdata) != SWINF_FSM_WFVLDCLR);

    return 0;
}
static int pmif_spmi_read_cmd(struct pmif *arb, unsigned char opc,
		unsigned char sid, unsigned short addr, unsigned char *buf,
		unsigned short len)
{
	int reg_rdata = 0, write = 0x0;
	unsigned int in_isr_and_cs = 0, ret = 0;
	unsigned char bc = len - 1;

	if ((sid & ~(0xf)) != 0x0)
		return -1;

	if ((addr & ~(0xffff)) != 0x0)
		return -1;

	if ((*buf & ~(0xffff)) != 0x0)
		return -1;

	if ((bc & ~(0x1)) != 0x0)
		return -1;

	/* Check the opcode */
	if (opc >= 0x60 && opc <= 0x7F)
		opc = PMIF_SPMI_CMD_REG;
	else if (opc >= 0x20 && opc <= 0x2F)
		opc = PMIF_SPMI_CMD_EXT_REG;
	else if (opc >= 0x38 && opc <= 0x3F)
		opc = PMIF_SPMI_CMD_EXT_REG_LONG;
	else
		return -1;

	/* check C.S. */
	in_isr_and_cs = is_in_isr();
	if(!in_isr_and_cs) {
		taskENTER_CRITICAL();
	}
	/* Wait for Software Interface FSM state to be IDLE. */
	ret = pmif_check_idle();
	if(ret)
		return ret;

	/* Send the command. */
	DRV_WriteReg32(PMIF_ACC,
		(opc << 30) | (write << 29) | (sid << 24) | (bc << 16) | addr);

	/* Wait for Software Interface FSM state to be WFVLDCLR,
	 *
	 * read the data and clear the valid flag.
	 */
	if(write == 0)
	{
		ret = pmif_check_vldclr();
		if(ret)
			return ret;

		*buf = DRV_Reg32(PMIF_RDATA_31_0);

		DRV_WriteReg32(PMIF_VLD_CLR, 0x1);
	}

	/* check C.S. */
	if(!in_isr_and_cs) {
		taskEXIT_CRITICAL();
	}

	return 0x0;
}

static int pmif_spmi_write_cmd(struct pmif *arb, unsigned char opc,
	unsigned char sid, unsigned short addr, const unsigned char *buf,
	unsigned short len)
{
	int reg_rdata = 0, write = 0x1;
	unsigned int in_isr_and_cs = 0, ret = 0;
	unsigned char bc = len - 1;

	if ((sid & ~(0xf)) != 0x0)
		return -1;

	if ((addr & ~(0xffff)) != 0x0)
		return -1;

	if ((*buf & ~(0xffff)) != 0x0)
		return -1;

	if ((bc & ~(0x1)) != 0x0)
		return -1;

	/* Check the opcode */
	if (opc >= 0x40 && opc <= 0x5F)
		opc = PMIF_SPMI_CMD_REG;
	else if (opc <= 0x0F)
		opc = PMIF_SPMI_CMD_EXT_REG;
	else if (opc >= 0x30 && opc <= 0x37)
		opc = PMIF_SPMI_CMD_EXT_REG_LONG;
	else if (opc >= 0x80)
		opc = PMIF_SPMI_CMD_REG_0;
	else
		return -1;

	/* check C.S. */
	in_isr_and_cs = is_in_isr();
	if(!in_isr_and_cs) {
		taskENTER_CRITICAL();
	}
	/* Wait for Software Interface FSM state to be IDLE. */
	ret = pmif_check_idle();
	if(ret)
		return ret;

	/* Set the write data. */
	if (write == 1)
	{
		DRV_WriteReg32(PMIF_WDATA_31_0, *buf);
	}

	/* Send the command. */
	DRV_WriteReg32(PMIF_ACC,
		(opc << 30) | (write << 29) | (sid << 24) | (bc << 16) | addr);

	/* check C.S. */
	if(!in_isr_and_cs) {
		taskEXIT_CRITICAL();
	}

	return 0x0;
}

struct pmif *get_pmif_controller(int inf, int mstid)
{
	if (inf == PMIF_SPMI)
		return &pmif_spmi_arb[mstid];

	return 0;
}

int is_pmif_spmi_init_done(void)
{
	int ret = 0, reg_data = 0;

	reg_data = DRV_Reg32(PMIF_STA);
	ret = GET_PMIF_INIT_DONE(reg_data);
	if ((ret & 0x1) == 1)
		return 0;

	return -1;
}

int pmif_spmi_init(void) {
	struct pmif *spmi_arb = get_pmif_controller(PMIF_SPMI, SPMI_MASTER_0);
	int ret = 0;

	ret = spmi_init(spmi_arb, SPMI_MASTER_0);
	if(ret) {
		PRINTF_E("%s spmi init fail\r\n", __func__);
		return -1;
	}

	return 0;
}

#endif /* endif PMIF_NO_PMIC */
