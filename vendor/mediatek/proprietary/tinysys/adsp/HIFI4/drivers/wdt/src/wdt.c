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

#include <wdt.h>
#include "FreeRTOS.h" /* log */
#include "main.h"
#include <driver_api.h>
#include <mt_reg_base.h>
#ifdef CFG_XGPT_SUPPORT
#include <mt_gpt.h>
#endif
#include "task.h"
#include "cli.h"
#include <stdlib.h>

#define WDT_MAX_TIMEOUT		31
static unsigned int wdt_timeout = WDT_MAX_TIMEOUT;
static unsigned int interval = 20;
static unsigned int margin = 10;
TaskHandle_t kick_wdt_handler = NULL;

void mtk_wdt_disable(void)
{
	unsigned int tmp;
	tmp = DRV_Reg32(ADSP_WDT_MODE);
	tmp &= ~WDT_ENABLE;
	tmp |= WDT_MODE_KEY;
	DRV_WriteReg32(ADSP_WDT_MODE, tmp);
	PRINTF_D("[watchdog] watchdog disable mode:0x%x\n", DRV_Reg32(ADSP_WDT_MODE));
}

void mtk_wdt_enable(void)
{
	unsigned int tmp;
	/* irq only mode, low level trigger */
	tmp = DRV_Reg32(ADSP_WDT_MODE);
	tmp |= (WDT_IRQ | WDT_EXTEN | WDT_ENABLE | WDT_IRQ_LVL_EN | WDT_MODE_KEY);
	tmp &= ~(WDT_EXTPOL | WDT_DUAL);
	DRV_WriteReg32(ADSP_WDT_MODE, tmp);
	PRINTF_D("[watchdog] watchdog enable mode:0x%x\n", DRV_Reg32(ADSP_WDT_MODE));
}

void mtk_wdt_restart(void)
{
	DRV_WriteReg32(ADSP_WDT_RESTART, ADSP_WDT_RESTART_RELOAD);
	PRINTF_D("[watchdog]: kick Ex WDT\n");
}

void mtk_wdt_reset(void)
{
	unsigned int tmp;
	tmp = DRV_Reg32(ADSP_WDT_MODE);
	tmp |= (WDT_EXTEN | WDT_MODE_KEY);
	tmp &= ~(WDT_EXTPOL | WDT_DUAL | WDT_IRQ | WDT_ENABLE);
	DRV_WriteReg32(ADSP_WDT_MODE, tmp);
	PRINTF_D("[watchdog]: watchdog reset WDT_MODE:0x%x\n", DRV_Reg32(ADSP_WDT_MODE));
	while(1)
		DRV_WriteReg32(ADSP_WDT_SWRST, ADSP_WDT_SWRST_KEY);
}

void mtk_wdt_irq_trigger(void)
{
	unsigned int tmp;
	tmp = DRV_Reg32(ADSP_WDT_MODE);
	/* low level trigger */
	tmp |= (WDT_IRQ | WDT_IRQ_LVL_EN | WDT_MODE_KEY);
	tmp &= ~(WDT_EXTPOL | WDT_DUAL | WDT_ENABLE | WDT_EXTEN);
	DRV_WriteReg32(ADSP_WDT_MODE, tmp);
	PRINTF_D("[watchdog]: watchdog irq trigger WDT_MODE:0x%x\n", DRV_Reg32(ADSP_WDT_MODE));
	DRV_WriteReg32(ADSP_WDT_SWRST, ADSP_WDT_SWRST_KEY);
}

void mtk_wdt_restart_interval(unsigned long long interval)
{

}

int mtk_wdt_set_time_out_value(unsigned int value)
{
	unsigned int tmp;
	tmp = WDT_LENGTH_TIMEOUT(value << 6) | ADSP_WDT_LENGTH_KEY;
	DRV_WriteReg32(ADSP_WDT_LENGTH, tmp);

	mtk_wdt_restart();

	PRINTF_D("[watchdog] set watchdog timeout value:0x%x\n", DRV_Reg32(ADSP_WDT_LENGTH));
	return 0;
}

static void mtk_kick_wdt_thread(void *pvParameters)
{
	portTickType xLastExecutionTime, xDelayTime;
	xLastExecutionTime = xTaskGetTickCount();
	PRINTF_D("[watchdog] start watchdog kick thread\n");

	for(;;)
	{
		xDelayTime = interval * 1000 / portTICK_RATE_MS;
		mtk_wdt_restart();
		vTaskDelayUntil(&xLastExecutionTime, xDelayTime);
	}
}

#ifdef CFG_CLI_SUPPORT
static int cli_watchdog_enable( char *pcWriteBuffer,
				size_t xWriteBufferLen, const char *pcCommandString )
{
	int interval_len, margin_len;

	if (2 != FreeRTOS_CLIGetNumberOfParameters(pcCommandString))
	{
		PRINTF_D("The number of parameters should be 2\n");
		return 0;
	}

	interval = atoi(FreeRTOS_CLIGetParameter(pcCommandString, 1, &interval_len));
	margin = atoi(FreeRTOS_CLIGetParameter(pcCommandString, 2, &margin_len));

	PRINTF_D("interval:%d margin:%d\n", interval, margin);

	if (interval + margin > 31)
	{
		PRINTF_D("interval add margin should not bigger than 31\n");
		return 0;
	}

	mtk_wdt_set_time_out_value(interval + margin);
	mtk_wdt_enable();

	return 0;
}

static int cli_watchdog_disable( char *pcWriteBuffer,
			size_t xWriteBufferLen, const char *pcCommandString )
{
	mtk_wdt_disable();
	return 0;
}

#if 0
static int cli_watchdog_timeout( char *pcWriteBuffer,
			size_t xWriteBufferLen, const char *pcCommandString )
{
	mtk_wdt_set_time_out_value(5);
	mtk_wdt_enable();
	vTaskSuspend(kick_wdt_handler);
	return 0;
}
#endif

static const CLI_Command_Definition_t cli_command_watchdog_enable =
{
	"wdt_enable",
	"\r\nwdt_enable <interval> <margin>\r\n Enable watchdog\r\n",
	cli_watchdog_enable,
	2
};

static const CLI_Command_Definition_t cli_command_watchdog_disable =
{
	"wdt_disable",
	"\r\nwdt_disable\r\n Disable watchdog\r\n",
	cli_watchdog_disable,
	-1
};

#if 0
static const CLI_Command_Definition_t cli_command_watchdog_timeout_test =
{
	"wdt_timeout",
	"\r\nwdt_timeout_test\r\n Watchdog timeout test\r\n",
	cli_watchdog_timeout,
	-1
};
#endif

NORMAL_SECTION_FUNC static void cli_watchdog_register(void)
{
	FreeRTOS_CLIRegisterCommand(&cli_command_watchdog_enable);
	FreeRTOS_CLIRegisterCommand(&cli_command_watchdog_disable);
	//FreeRTOS_CLIRegisterCommand(&cli_command_watchdog_timeout_test);
}
#endif

NORMAL_SECTION_FUNC void mtk_wdt_init(void)
{
	wdt_timeout = interval + margin;
	mtk_wdt_set_time_out_value(wdt_timeout);
	mtk_wdt_enable();
	PRINTF_D("[watchdog] watchdog init, mode:0x%x\n",
		DRV_Reg32(ADSP_WDT_MODE));

	/* Create wdt kick thread */
	xTaskCreate(mtk_kick_wdt_thread, "wdt_kick",
			configMINIMAL_STACK_SIZE,
			NULL, configMAX_PRIORITIES - 1, &kick_wdt_handler);

#ifdef CFG_CLI_SUPPORT
	cli_watchdog_register();
#endif
}
