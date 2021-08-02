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

#include <adsp_excep.h>
#ifdef CFG_IPC_SUPPORT
#include <adsp_ipi.h>
#endif
#ifdef CFG_TRAX_SUPPORT
#include <adsp_trax.h>
#endif
#include <driver_api.h>
#include <FreeRTOS.h>
#include <mt_reg_base.h>
#include "types.h"

#include <stdio.h>
#include <task.h>
#include <unwind.h>
#ifdef CFG_WDT_SUPPORT
#include <wdt.h>
#endif
#include <xtensa_api.h>
#include <xtensa/corebits.h>

#ifdef CFG_TRAX_SUPPORT
extern  trax_context context;
#endif

struct adsp_excep_desc
{
    int id;
    const char *desc;
};

static const struct adsp_excep_desc excep_desc[] =
{
    {EXCCAUSE_ILLEGAL,                  "Illegal Instruction "},
    {EXCCAUSE_INSTR_ERROR,              "Instruction Fetch Error"},
    {EXCCAUSE_LOAD_STORE_ERROR,         "Load Store Error"},
    {EXCCAUSE_DIVIDE_BY_ZERO,           "Integer Divide by Zero"},
    {EXCCAUSE_PRIVILEGED,               "Privileged Instruction"},
    {EXCCAUSE_UNALIGNED,                "Unaligned Load or Store"},
    {EXCCAUSE_INSTR_DATA_ERROR,         "PIF Data Error on Instruction Fetch"},
    {EXCCAUSE_LOAD_STORE_DATA_ERROR,    "PIF Data Error on Load or Store"},
    {EXCCAUSE_INSTR_ADDR_ERROR,         "PIF Address Error on Instruction Fetch"},
    {EXCCAUSE_LOAD_STORE_ADDR_ERROR,    "PIF Address Error on Load or Store"},
    {EXCCAUSE_ITLB_MISS,                "ITLB Miss"},
    {EXCCAUSE_ITLB_MULTIHIT,            "ITLB Multihit"},
    {EXCCAUSE_INSTR_RING,               "Ring Privilege Violation on Instruction Fetch"},
    {EXCCAUSE_INSTR_PROHIBITED,         "Cache Attribute does not allow Instruction Fetch"},
    {EXCCAUSE_DTLB_MISS,                "DTLB Miss"},
    {EXCCAUSE_DTLB_MULTIHIT,            "DTLB Multihit"},
    {EXCCAUSE_LOAD_STORE_RING,          "Ring Privilege Violation on Load or Store"},
    {EXCCAUSE_LOAD_PROHIBITED,          "Cache Attribute does not allow Load"},
    {EXCCAUSE_STORE_PROHIBITED,         "Cache Attribute does not allow Store"}
};

static unsigned int last_pc = 0x0;
static unsigned int current_pc = 0x0;

static _Unwind_Reason_Code trace_func(struct _Unwind_Context *ctx, void *d)
{
    current_pc = _Unwind_GetIP(ctx);
    if (current_pc && (current_pc != last_pc))
    {
        PRINTF_E("  0x%08x\n", current_pc);
        last_pc = current_pc;
    }
    return _URC_NO_REASON;
}

void print_backtrace(void)
{
    int depth = 0;

    _Unwind_Backtrace(&trace_func, &depth);
}

const char* get_excep_desc(int excep_id)
{
    int i;
    int cnt = ARRAY_SIZE(excep_desc);

    for (i = 0; i < cnt; i++)
        if (excep_desc[i].id == excep_id)
            return excep_desc[i].desc;

    return NULL;
}


void* adsp_excep_handler(XtExcFrame *frame)
{
    const char *cause = NULL;

    PRINTF_E("Oops...\n");
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
        PRINTF_E("Task: %s\n", pcTaskGetTaskName(NULL));

    cause = get_excep_desc(frame->exccause);
    PRINTF_E("Exception Cause: %ld (%s)\n", frame->exccause,
        cause != NULL ? cause : "UNKNOWN");
    PRINTF_E("Exception Virtual Address: 0x%lx\n", frame->excvaddr);
    PRINTF_E("Exception PC: 0x%x\n", XT_RSR_EPC1());
    PRINTF_E("backtrace:\n");
    print_backtrace();

    // dump trax here
#ifdef CFG_TRAX_SUPPORT
    int len = 0;
    len = trax_save(&context);
    if (len < 0) {
        PRINTF_E("trax_save error!\n");
    }
    else {
        /* send save done ipi and content length back */
        enum ipi_id trax_id = IPI_TRAX_DONE;
        while (adsp_ipi_send(trax_id, &len, sizeof(len),
                             0, IPI_ADSP2AP) != DONE);
    }
#endif
    taskDISABLE_INTERRUPTS();
#ifdef CFG_WDT_SUPPORT
    /* watchdog irq only mode */
    mtk_wdt_irq_trigger();
#endif

    while (1) {
        __asm(
            "memw;\n"
            "WAITI 15;\n"
        );
    }
}

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void vAssertCalled(char *file, unsigned int line)
{
    printf("Assertion failed in %s:%d\n", file, line);

    __asm__ (
        "movi.n  a7,0\n"
        "quos    a6,a6,a7\n"
    );
}


/** init exception handler
*  @returns
*    no return
*/
NORMAL_SECTION_FUNC void adsp_excep_init(void)
{
    int i;
    int cnt = ARRAY_SIZE(excep_desc);

    for (i = 0; i < cnt; i++) {
        xt_set_exception_handler(excep_desc[i].id, (xt_exc_handler)adsp_excep_handler);
    }
}
