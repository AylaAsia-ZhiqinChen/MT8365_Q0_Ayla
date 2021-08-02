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
 * MediaTek Inc. (C) 2018. All rights reserved.
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

#include "pmic_wrap.h"
#include "irq.h"
#if (PMIC_WRAP_SSPM)
unsigned int sspm_wacs2_base;
#endif

/************** For timeout API *********************/
#define PWRAP_NO_TIMEOUT
#ifdef PWRAP_NO_TIMEOUT
static unsigned int _pwrap_get_current_time(void) {
    return 0;
}

static int _pwrap_timeout_ns(unsigned int start_time, unsigned int elapse_time) {
    return 0;
}

static unsigned int _pwrap_time2ns(unsigned int time_us) {
    return 0;
}
#else
static unsigned int _pwrap_get_current_time(void) {
    portTickType tick_in_ns = (1000 / portTICK_RATE_MS) * 1000;
    return (xTaskGetTickCount() * tick_in_ns);

    /* return (xTaskGetTickCount() * (1000 / portTICK_RATE_MS) * 1000); */
}

static int _pwrap_timeout_ns (unsigned int start_time, unsigned int elapse_time) {
    unsigned int cur_time = 0 ;
    portTickType tick_in_ns = (1000 / portTICK_RATE_MS) * 1000;
    cur_time = (xTaskGetTickCount() * tick_in_ns);
    return (cur_time > (start_time + elapse_time));
}

static unsigned int _pwrap_time2ns (unsigned int time_us) {
    return (time_us * 1000);
}

#endif

/****************  channel API *****************/
typedef unsigned int (*loop_condition_fp)(unsigned int);

static inline unsigned int wait_for_fsm_idle(unsigned int x) {
    return (GET_WACS_SSPM_FSM( x ) != 0 );
}
static inline unsigned int wait_for_fsm_vldclr(unsigned int x) {
    return (GET_WACS_SSPM_FSM( x ) != 6);
}

static inline unsigned int  wait_for_state_idle(loop_condition_fp fp, unsigned int timeout_us, void *wacs_register,
            void *wacs_vldclr_register, unsigned int *read_reg) {
    unsigned long long start_time_ns = 0, timeout_ns = 0;
    unsigned int reg_rdata;

    start_time_ns = _pwrap_get_current_time();
    timeout_ns = _pwrap_time2ns(timeout_us);

    do {
        if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
            PWRAPERR("wait_for_idle timeout\n");
            return E_PWR_WAIT_IDLE_TIMEOUT;
        }
        reg_rdata = DRV_Reg32(wacs_register);
        if (GET_WACS_INIT_DONE_SSPM(reg_rdata) != WACS_INIT_DONE) {
            PWRAPERR("init not finish\n");
            return E_PWR_NOT_INIT_DONE;
        }
    } while (fp(reg_rdata));

    if (read_reg)
        *read_reg = reg_rdata;

    return 0;
}

static inline unsigned int wait_for_state_ready(loop_condition_fp fp, unsigned int timeout_us, void *wacs_register,
            unsigned int *read_reg) {

    unsigned long long start_time_ns = 0, timeout_ns = 0;
    unsigned int reg_rdata;

    start_time_ns = _pwrap_get_current_time();
    timeout_ns = _pwrap_time2ns(timeout_us);

    do {
        if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
            PWRAPERR("wait_for_ready timeout\n");
            return E_PWR_WAIT_IDLE_TIMEOUT;
        }
        reg_rdata = DRV_Reg32(wacs_register);
        if (GET_WACS_INIT_DONE_SSPM(reg_rdata) != WACS_INIT_DONE) {
            PWRAPERR("init not finish\n");
            return E_PWR_NOT_INIT_DONE;
        }
    } while (fp(reg_rdata));

    if(read_reg)
        *read_reg = reg_rdata;

    return 0;
}

static signed int pwrap_sspm(unsigned int write, unsigned int adr, unsigned int wdata, unsigned int *rdata) {
        unsigned int reg_rdata = 0;
        unsigned int wacs_write = 0;
        unsigned int wacs_adr = 0;
        unsigned int wacs_cmd = 0;
        unsigned int ret = 0, in_isr_and_cs = 0;

        /* Check argument validation */
        if ((write & ~(0x1))    != 0)  return E_PWR_INVALID_RW;
        if ((adr   & ~(0xffff)) != 0)  return E_PWR_INVALID_ADDR;
        if ((wdata & ~(0xffff)) != 0)  return E_PWR_INVALID_WDAT;

        /* check C.S. */
        in_isr_and_cs = is_in_isr();
        if(!in_isr_and_cs) {
            taskENTER_CRITICAL();
        }

        /* Check IDLE & INIT_DONE in advance */
        ret = wait_for_state_idle(wait_for_fsm_idle, TIMEOUT_WAIT_IDLE,
            (UINT32P)PMIC_WRAP_WACS_RDATA, (UINT32P)PMIC_WRAP_WACS_VLDCLR, 0);
        if (ret != 0) {
            PWRAPERR("wait_fsm_idle fail,ret=%x\n", ret);
            goto FAIL;
        }
        /* Argument process */
        wacs_write = write << 31;
        wacs_adr = (adr >> 1) << 16;
        wacs_cmd = wacs_write | wacs_adr | wdata;
        DRV_WriteReg32(PMIC_WRAP_WACS_CMD, wacs_cmd);

        if (write == 0) {
            if (NULL == rdata) {
                PWRAPERR("rdata null\n");
                ret= E_PWR_INVALID_ARG;
                goto FAIL;
            }
            ret = wait_for_state_ready(wait_for_fsm_vldclr, TIMEOUT_READ,
                    (UINT32P)PMIC_WRAP_WACS_RDATA, &reg_rdata);
            if (ret != 0) {
                PWRAPERR("wait_fsm_vldclr fail,ret=%x\n", ret);
                ret += 1;
                goto FAIL;
            }

            *rdata = GET_WACS_SSPM_RDATA(reg_rdata);
            DRV_WriteReg32(PMIC_WRAP_WACS_VLDCLR , 1);
        }

    FAIL:
        /* check C.S. */
        if(!in_isr_and_cs) {
            taskEXIT_CRITICAL();
        }
        if (ret != 0) {
            PWRAPERR("pwrap_sspm fail,ret=%x\n", ret);
        }

    return ret;
}


signed int pwrap_sspm_read(unsigned int adr, unsigned int *rdata) {
    return pwrap_sspm(0, adr, 0, rdata);
}

signed int pwrap_sspm_write(unsigned int adr, unsigned int wdata) {
    return pwrap_sspm(1, adr, wdata, 0);
}

/*************************************************/


/*******************wacs2 channel API ******************************/
#if (PMIC_WRAP_SSPM)
static inline unsigned int wait_for_wacs2_fsm_idle(unsigned int x) {
    return (GET_WACS2_FSM(x) != WACS_FSM_IDLE);
}

static inline unsigned int wait_for_wacs2_fsm_vldclr(unsigned int x) {
    return (GET_WACS2_FSM(x) != WACS_FSM_WFVLDCLR);
}

static unsigned int  wait_for_wacs2_state_idle(loop_condition_fp fp, unsigned int timeout_us, unsigned int wacs_register, unsigned int wacs_vldclr_register,
        unsigned int *read_reg) {
    unsigned long long start_time_ns = 0, timeout_ns = 0;
    unsigned int reg_rdata;

    start_time_ns = _pwrap_get_current_time();
    timeout_ns = _pwrap_time2ns(timeout_us);

    do {
        if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
            PWRAPERR("wait_wacs2_idle timeout\n");
            return E_PWR_WAIT_IDLE_TIMEOUT;
        }
        reg_rdata = WRAP_RD32_WACS2(wacs_register);
        if (GET_WACS2_INIT_DONE2(reg_rdata) != WACS_INIT_DONE) {
            PWRAPERR("init not finish\n");
            return E_PWR_NOT_INIT_DONE;
        }
        /* if last read command timeout,clear vldclr bit
         * read command state machine:FSM_REQ-->wfdle-->WFVLDCLR;write:FSM_REQ-->idle */
        switch (GET_WACS2_FSM(reg_rdata)) {
            case WACS_FSM_WFVLDCLR:
                WRAP_WR32_WACS2(wacs_vldclr_register , 1);
                /* PWRAPERR("WACS_FSM = PMIC_WRAP_WACS_VLDCLR\n\r"); */
                break;
            case WACS_FSM_WFDLE:
                /* PWRAPERR("WACS_FSM = WACS_FSM_WFDLE\n\r"); */
                break;
            case WACS_FSM_REQ:
                /* PWRAPERR("WACS_FSM = WACS_FSM_REQ\n\r"); */
                break;
            default:
               break;
        }
    } while (fp(reg_rdata)); /*IDLE State*/

    if (read_reg)
        *read_reg = reg_rdata;

    return 0;
}

static unsigned int wait_for_wacs2_state_ready(loop_condition_fp fp, unsigned int timeout_us, unsigned int wacs_register, unsigned int *read_reg) {
    unsigned long long start_time_ns = 0, timeout_ns = 0;
    unsigned int reg_rdata;

    start_time_ns = _pwrap_get_current_time();
    timeout_ns = _pwrap_time2ns(timeout_us);

    do {
        if (_pwrap_timeout_ns(start_time_ns, timeout_ns)) {
            PWRAPERR("wait_wacs2_ready timeout\n");
            return E_PWR_WAIT_IDLE_TIMEOUT;
        }
        reg_rdata = DRV_Reg32(wacs_register);
        if (GET_WACS2_INIT_DONE2(reg_rdata) != WACS_INIT_DONE) {
            PWRAPERR("init not finish\n");
            return E_PWR_NOT_INIT_DONE;
        }
    } while (fp(reg_rdata)); /*IDLE State */

    if (read_reg)
        *read_reg = reg_rdata;

    return 0;
}


static signed int pwrap_wacs2(unsigned int write, unsigned int adr, unsigned int wdata, unsigned int *rdata) {
    unsigned int reg_rdata = 0;
    unsigned int wacs_write = 0;
    unsigned int wacs_adr = 0;
    unsigned int wacs_cmd = 0;
    unsigned int ret = 0;
    unsigned int wacs2_rdata_reg;
    unsigned int wacs2_vldclr_reg;
    unsigned int wacs2_cmd_reg, in_isr_and_cs = 0;;

    if (sspm_wacs2_base == 0x0) {
       PWRAPERR("base err\n");
       /** try again, mapping wacs2 address to sspm **/
       while ((sspm_wacs2_base = ispeed_sysreg_remap(PMIC_WRAP_BASE)) == 0) {};
       if (sspm_wacs2_base == 0x0)
           configASSERT(0);
    }
    wacs2_rdata_reg = sspm_wacs2_base + PMIC_WRAP_WACS2_RDATA_OFFSET;
    wacs2_vldclr_reg = sspm_wacs2_base + PMIC_WRAP_WACS2_VLDCLR_OFFSET;
    wacs2_cmd_reg = sspm_wacs2_base + PMIC_WRAP_INIT_DONE2_OFFSET;

    /* Check argument validation */
    if ((write & ~(0x1))    != 0)  return E_PWR_INVALID_RW;
    if ((adr   & ~(0xffff)) != 0)  return E_PWR_INVALID_ADDR;
    if ((wdata & ~(0xffff)) != 0)  return E_PWR_INVALID_WDAT;

    /* check C.S. */
    in_isr_and_cs = is_in_isr();
    if(!in_isr_and_cs) {
        taskENTER_CRITICAL();
    }
    /* Check IDLE & INIT_DONE in advance */
    ret = wait_for_wacs2_state_idle(wait_for_wacs2_fsm_idle, TIMEOUT_WAIT_IDLE, wacs2_rdata_reg, wacs2_vldclr_reg, 0);
    if (ret != 0) {
        PWRAPERR("wacs2_fsm_idle fail,ret=%x\n", ret);
        goto FAIL;
    }

    wacs_write  = write << 31;
    wacs_adr    = (adr >> 1) << 16;
    wacs_cmd = wacs_write | wacs_adr | wdata;
    WRAP_WR32_WACS2(wacs2_cmd_reg,  wacs_cmd);

    if (write == 0) {
        if (rdata == NULL) {
            PWRAPERR("rdata null\n");
            ret = E_PWR_INVALID_ARG;
            goto FAIL;
        }
        ret = wait_for_wacs2_state_ready(wait_for_wacs2_fsm_vldclr, TIMEOUT_READ, wacs2_rdata_reg, &reg_rdata);
        if (ret != 0) {
            PWRAPERR("wacs2_fsm_vldclr fail,ret=%x\n", ret);
            ret+=1;
            goto FAIL;
        }

        *rdata = GET_WACS2_RDATA(reg_rdata);
        WRAP_WR32_WACS2(wacs2_vldclr_reg, 1);
    }

FAIL:
    /* check C.S. */
    if(!in_isr_and_cs) {
        taskEXIT_CRITICAL();
    }
    if (ret != 0) {
        PWRAPERR("pwrap_wacs2 fail,ret=%x\n", ret);
    }

    ispeed_sysreg_unremap(sspm_wacs2_base);

    return ret;
}


signed int pwrap_wacs2_read(unsigned int adr, unsigned int *rdata) {
    return pwrap_wacs2(0, adr, 0, rdata);
}

signed int pwrap_wacs2_write(unsigned int adr, unsigned int wdata) {
    return pwrap_wacs2(1, adr, wdata, 0);
}

static signed int pwrap_set_reg(unsigned int adr, unsigned int wdata) {
    int retry = 10;
    int ret;
    unsigned int rdata;

    PWRAPLOG("reg:0x%x,wdata:0x%x\n", adr, wdata);

    ret = ispeed_sysreg_single_write(adr, wdata, retry);
    if (ret < 0)
        PWRAPERR("addr:0x%x fail\n", adr);

    PWRAPLOG("reg[0x%x]:0x%x\n", adr, ispeed_sysreg_single_read(adr, &rdata, retry));
    return 0;
}

/* For wacs2 channel loop deal  */

#endif /* end of #if (PMIC_WRAP_SSPM) */
/******* For channel UT ********/
#ifdef PMIC_WRAP_DEBUG
static unsigned int pwrap_read_test(void) {
    unsigned int rdata = 0;
    unsigned int ret = 0;

    ret = pwrap_sspm_read(DEW_READ_TEST, &rdata);
    if (rdata != DEFAULT_VALUE_READ_TEST) {
        PWRAPERR("Read fail,rdata=0x%x,exp=0x5aa5,ret=0x%x\n", rdata, ret);
        return E_PWR_READ_TEST_FAIL;
    } else {
        PWRAPLOG("Read pass,ret=%x\n", ret);
    }

    return 0;
}

static unsigned int pwrap_write_test(void) {
    unsigned int rdata = 0;
    unsigned int ret = 0;
    unsigned int ret1 = 0;

    ret = pwrap_sspm_write(DEW_WRITE_TEST, PWRAP_WRITE_TEST_VALUE);
    ret1 = pwrap_sspm_read(DEW_WRITE_TEST, &rdata);
    if ((rdata != PWRAP_WRITE_TEST_VALUE) || (ret != 0) || (ret1 != 0)) {
        PWRAPERR("Write fail,rdata=0x%x,exp=0xa55a,ret=0x%x,ret1=0x%x\n",
            rdata, ret, ret1);
        return E_PWR_INIT_WRITE_TEST;
    } else {
        PWRAPLOG("Write pass\n");
    }
    return 0;
}
#endif
signed int pmic_wrap_init(void) {
    /* Init sspm */
//    ispeed_single_write(PMIC_WRAP_WACS_EN, 0x1);
//    ispeed_single_write(PMIC_WRAP_WACS_INIT_DONE, 0x1);

#ifdef PMIC_WRAP_DEBUG
    pwrap_read_test();
    pwrap_write_test();
#endif
    /** mapping wacs2 address to sspm **/
    while ((sspm_wacs2_base = ispeed_sysreg_remap(PMIC_WRAP_BASE)) == 0) {};

    PWRAPLOG("init done\n");

    return 0;
}
