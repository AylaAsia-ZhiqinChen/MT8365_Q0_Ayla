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
#include "FreeRTOS.h"
#include "tinysys_reg.h"
#include "driver_api.h"
#include "debug.h"
#include "irq.h"
#include "wdt.h"
#include "ostimer.h"
#include "ispeed.h"
#ifdef CFG_COREDUMP_SUPPORT
#include <coredump.h>
#endif
#ifdef CFG_PCTRACE_SUPPORT
#include <pctrace.h>
#endif

/* #define WDT_RESET_TEST */

#ifdef _WDT_DBG
struct ctx_info_s {
    unsigned int r1;
    unsigned int r2;
    unsigned int r3;
    unsigned int r4;
    unsigned int r5;
    unsigned int r6;
    unsigned int r7;
    unsigned int r8;
    unsigned int r9;
    unsigned int r10;
    unsigned int r11;
    unsigned int r12;
    unsigned int r13;
    unsigned int r15;
    unsigned int sr;
    unsigned int ipc;
    unsigned int isr;
    unsigned int lf;
    unsigned int ls0;
    unsigned int le0;
    unsigned int lc0;
    unsigned int ls1;
    unsigned int le1;
    unsigned int lc1;
    unsigned int ls2;
    unsigned int le2;
    unsigned int lc2;
    unsigned int a0l;
    unsigned int a0h;
};
#endif

/* #define SSPM_WDT_TEST */
/* #define SSPM_WDT_IRQ_MODE */

#define WDT_TIMEOUT_MS_TO_TICK(ms) (((ms) * 32768) / 1000)

#define WDT_MAX_TIMEOUT_VAL        (0xFFFFF)

/*
 * Default SSPM WDT timeout value: 30 seconds
 *
 * Set timeout value the same as AP WDT to avoid incorrect
 * AP RGU WDT_STA confusing debugging flow.
 */
#define WDT_DEFAULT_TIMEOUT_MS     (30000)

#define WDT_EN                     ((unsigned int) 1 << 31)
#define KICK_WDT                   (0x1)

static unsigned long long wdt_time_old = 0;
static unsigned long long wdt_time_new = 0;
static unsigned long long wdt_time_k_old = 0;
static unsigned long long wdt_time_k_new = 0;
static unsigned int val_d;

static bool wdt_dump = 0;
static unsigned int wdt_cfg = 0xabcdabcd;

static struct sspm_record_reg_t {
    unsigned int mbox_in[5];
    unsigned int mbox_out[5];
    unsigned int res_ctrl;
} sspm_record_reg;

void wdt_handler(void *sp)
{
    unsigned int val, idx;

    wdt_cfg = DRV_Reg32(WDT_CFG);

waitdie:
    // if dumped, do nothing and wait ot die
    while (wdt_dump) {};

#ifdef CFG_PCTRACE_SUPPORT
    if (pctrace_enable(PCTRACE_DISABLE) == PCTRACE_DISABLE) {
        /*
            If pctrace is disable, it means the wdt timeout trigged by abort or assert, so do nothing
            FIXME: need another way if not support CFG_PCTRACE_SUPPORT
            */
        goto waitdie;
    }
#endif

#ifdef SSPM_WDT_IRQ_MODE
    // kick AP RGU watchdog to ensure AP WDT will not timeout during this flow
    if (ispeed_sysreg_single_write_from_ISR(MTK_WDT_RESTART, MTK_WDT_RESTART_KEY, 5) != DataMoveDone) {
        PRINTF_E_ISR("kick AP RGU fail\n");
    }
#endif

    wdt_disable();

#ifdef _WDT_DBG
    struct ctx_info_s *ctx;
    ctx = (struct ctx_info_s *) sp;

    PRINTF_E_ISR("REG DUMP:\n");
    PRINTF_E_ISR(" r1: 0x%08x,  r2: 0x%08x,  r3: 0x%08x,  r4: 0x%08x\n", ctx->r1, ctx->r2, ctx->r3, ctx->r4);
    PRINTF_E_ISR(" r5: 0x%08x,  r6: 0x%08x,  r7: 0x%08x,  r8: 0x%08x\n", ctx->r5, ctx->r6, ctx->r7, ctx->r8);
    PRINTF_E_ISR(" r9: 0x%08x, r10: 0x%08x, r11: 0x%08x, r12: 0x%08x\n", ctx->r9, ctx->r10, ctx->r11, ctx->r12);
    PRINTF_E_ISR("r13: 0x%08x, r14: 0x%08x, r15: 0x%08x, sr: 0x%08x\n", ctx->r13, ctx, ctx->r15, ctx->sr);
    PRINTF_E_ISR("ipc: 0x%08x, isr: 0x%08x, lf: 0x%08x \n", ctx->ipc, ctx->isr, ctx->lf);
    PRINTF_E_ISR("ls0: 0x%08x, le0: 0x%08x, lc0: 0x%08x \n", ctx->ls0, ctx->le0, ctx->lc0);
    PRINTF_E_ISR("ls1: 0x%08x, le1: 0x%08x, lc1: 0x%08x \n", ctx->ls1, ctx->le1, ctx->lc1);
    PRINTF_E_ISR("ls2: 0x%08x, le2: 0x%08x, lc2: 0x%08x \n", ctx->ls2, ctx->le2, ctx->lc2);
#endif


#if ( ( INCLUDE_xTaskGetCurrentTaskHandle == 1 ) || ( configUSE_MUTEXES == 1 ) )
    PRINTF_E_ISR("Running: %s\n", pcTaskGetTaskName(xTaskGetCurrentTaskHandle()));
#endif

#ifdef CFG_PCTRACE_SUPPORT
    // query the backtrace until pc = &_debug_isr_wdt_handler
    PRINTF_E_ISR("WDT T.O from 0x%x\n", pctrace_query(0x40, 0));
#endif

    /*
     * Dump WDT configuration in register. Other relative globals are not
     * dumped because they will be packed in sspm exception db.
     */
    PRINTF_E_ISR("WDT CFG -0x%x-\n", wdt_cfg);

#ifdef SYS_RES
    sspm_record_reg.res_ctrl = DRV_Reg32(SYS_RES);
#endif
    for (idx = 0; idx < 5; idx++) {
        sspm_record_reg.mbox_in[idx] = DRV_Reg32(MBOX0_IN_IRQ + ((idx) << 2));
        sspm_record_reg.mbox_out[idx] = DRV_Reg32(MBOX0_OUT_IRQ + ((idx) << 2));
    }

#ifdef CFG_COREDUMP_SUPPORT
    coredump_within_debug_isr(sp);
#endif

    wdt_dump = 1;

#ifdef SSPM_WDT_IRQ_MODE
    // change SSPM WDT to  "reset mode" in AP RGU
    if (ispeed_sysreg_single_read_from_ISR(MTK_WDT_REQ_IRQ_EN, &val, 5) == DataMoveDone) {
        if (ispeed_sysreg_single_write_from_ISR(MTK_WDT_REQ_IRQ_EN, (val & ~MTK_WDT_STATUS_SSPM_RST) | MTK_WDT_REQ_IRQ_KEY, 5) == DataMoveDone) {
            // dummy read to make sure write success
            ispeed_sysreg_single_read_from_ISR(MTK_WDT_REQ_IRQ_EN, &val, 5);

            // PRINTF_E_ISR("RESET SYSTEM: 0x%x\n", val);
            mtk_wdt_set_time_out_value(0x1);
        }
    }
#endif

    goto waitdie;
}

static int mtk_wdt_set_time_out_value(unsigned int value)
{
    if (value > WDT_MAX_TIMEOUT_VAL)
        value = WDT_MAX_TIMEOUT_VAL;

    wdt_disable();
    DRV_WriteReg32(WDT_CFG, WDT_EN | value);

    return 0;
}

static unsigned int mtk_wdt_cal_timeout_val_by_ms(unsigned int ms)
{
    unsigned int val = WDT_TIMEOUT_MS_TO_TICK(ms);

    if (val > WDT_MAX_TIMEOUT_VAL)
        return WDT_MAX_TIMEOUT_VAL;

    return val;
}

static int mtk_wdt_is_enabled(void)
{
    unsigned int val;

    val = DRV_Reg32(WDT_CFG);

    if (val & WDT_EN)
        return 1;

    return 0;
}

void wdt_disable(void)
{
    /* WDT_EN = 0, timeout value = default value */
    DRV_WriteReg32(WDT_CFG,
        mtk_wdt_cal_timeout_val_by_ms(WDT_DEFAULT_TIMEOUT_MS));
}

void wdt_enable(void)
{
    DRV_WriteReg32(WDT_CFG,
        WDT_EN | mtk_wdt_cal_timeout_val_by_ms(WDT_DEFAULT_TIMEOUT_MS));
}

void mtk_wdt_reset_test(void)
{
#ifdef WDT_RESET_TEST

    /* set a very short wdt timeout value to trigger sspm wdt timeout */

    struct sys_time_t ap_ts;

    ts_apmcu_time_isr(&ap_ts);

    /*
     * adjust wdt timeout to a very short time after 200 seconds passed
     * in ap to have enough time to grab exception db.
     */

    if (ap_ts.ts > 200000000000) {
        DRV_WriteReg32(WDT_CFG,
            WDT_EN | mtk_wdt_cal_timeout_val_by_ms(500));
    }
#else
    return;
#endif
}

void mtk_wdt_restart(void)
{
    struct sys_time_t ap_ts;
    wdt_time_k_old = wdt_time_k_new;

    val_d = DRV_Reg32(WDT_CFG);
    DRV_WriteReg32(WDT_KICK, KICK_WDT);
    wdt_time_k_new = wdt_time_new;

    mtk_wdt_reset_test();
}

void mtk_wdt_restart_interval(unsigned long long interval)
{
    struct sys_time_t ap_ts;

    ts_apmcu_time_isr(&ap_ts);
    wdt_time_new = ap_ts.ts;

    if ((wdt_time_new - wdt_time_old) > interval) {
        mtk_wdt_restart();
        wdt_time_old = wdt_time_new;
    }
}

#ifdef SSPM_WDT_TEST
/*
 * WDT UT test
 *
 * Usage: Just simply insert this API in main() in Main.c for WDT test.
 */
#define SSPM_WDT_TEST_KICK_STRESS_CNT (500)
#define SSPM_WDT_TEST_TIMEOUT_SECOND  (10)

void wdt_test(void)
{
    int i;
    int ret;

    PRINTF_D("[SSPM][WDT] ====== SSPM WDT Test ======\n");

    wdt_disable();

    mtk_wdt_set_time_out_value(SSPM_WDT_TEST_TIMEOUT_SECOND * 1000 * 32); // enable timer with very short timeout

    /* kick stress test */

    PRINTF_D("\n\n[SSPM][WDT] ## Kick Stress Test ##\n\n");

    for (i = 0; i < SSPM_WDT_TEST_KICK_STRESS_CNT; i++)
        mtk_wdt_restart();

    PRINTF_D("[SSPM][WDT] Kick WDT %d times pass\n", i);
    PRINTF_D("[SSPM][WDT] SSPM_WDT_CFG = %x\n", DRV_Reg32(WDT_CFG));

    /* wdt timeout test */

    PRINTF_D("\n\n[SSPM][WDT] ## Timeout Test ##\n\n");

    PRINTF_D("[SSPM][WDT] WDT shall timeout after %d seconds.\n", SSPM_WDT_TEST_TIMEOUT_SECOND);

    mtk_wdt_set_time_out_value(SSPM_WDT_TEST_TIMEOUT_SECOND * 1000 * 32);

    /*
     * NOTE: Make sure SSPM irq is triggerred in TOPRGU.
     */

    for (i = 0;; i++) {
        mdelay(1000);
        PRINTF_D("[SSPM][WDT] %d seconds passed ...\n", i);
    }
}

#endif /* SSPM_WDT_TEST */

void wdt_init(void)
{
    wdt_disable();
    wdt_enable();
    PRINTF_D("[SSPM][WDT] wdt_init: WDT_CFG = 0x%x\n", DRV_Reg32(WDT_CFG));

    intc_irq_request(&INTC_IRQ_WDT, NULL, (void *) 0);
}
