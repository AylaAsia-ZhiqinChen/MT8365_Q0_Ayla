#ifndef __PMIC_WRAP_INIT_H__
#define __PMIC_WRAP_INIT_H__

/****** SW ENV define *************************************/
#define PMIC_WRAP_SSPM           1
#define PMIC_WRAP_KERNEL         0

/* #define PMIC_WRAP_DEBUG */
#define PMIC_WRAP_SUPPORT
/****** For BringUp. if BringUp doesn't had PMIC, need open this ***********/
#undef PMIC_WRAP_NO_PMIC

/******  SW ENV header define *****************************/
#if (PMIC_WRAP_KERNEL)
#ifndef CONFIG_OF
#include <mach/mtk_reg_base.h>
#include <mach/mtk_irq.h>
#endif
#include "mt-plat/sync_write.h"
#elif (PMIC_WRAP_SSPM)
#include <stdio.h>
#include <debug.h>
#include <FreeRTOS.h>
#include <task.h>
#include "driver_api.h"
#include "tinysys_reg.h"
#include <ispeed.h>
#include "pmic_wrap_platform.h"
#else
### Compile error, check SW ENV define
#endif

/*******************start ---external API********************************/
#if (PMIC_WRAP_SSPM)
extern void vPmicWrapTask(void *pvParameters);
extern signed int pwrap_sspm_read(unsigned int adr, unsigned int *rdata);
extern signed int pwrap_sspm_write(unsigned int adr, unsigned int  wdata);
extern signed int pwrap_wacs2_read(unsigned int adr, unsigned int *rdata);
extern signed int pwrap_wacs2_write(unsigned int adr, unsigned int wdata);
#endif
extern signed int pmic_wrap_init(void);

/******  DEBUG marco define *******************************/
#define PWRAPTAG                "[PWRAP] "
#if (PMIC_WRAP_KERNEL)
#ifdef PMIC_WRAP_DEBUG
#define PWRAPDEB(fmt, arg...)   pr_debug(PWRAPTAG "cpuid=%d," fmt, raw_smp_processor_id(), ##arg)
#define PWRAPFUC(fmt, arg...)   pr_debug(PWRAPTAG "cpuid=%d,%s\n", raw_smp_processor_id(), __func__)
#endif
#define PWRAPLOG(fmt, arg...)   pr_debug(PWRAPTAG fmt, ##arg)
#define PWRAPERR(fmt, arg...)   pr_err(PWRAPTAG "ERROR,line=%d " fmt, __LINE__, ##arg)
#define PWRAPREG(fmt, arg...)   pr_debug(PWRAPTAG fmt, ##arg)
#elif (PMIC_WRAP_SSPM)
#ifdef PMIC_WRAP_DEBUG
#define PWRAPFUC(fmt, arg...)   PRINTF_D(PWRAPTAG "%s\n", __FUNCTION__)
#define PWRAPLOG(fmt, arg...)   PRINTF_D(PWRAPTAG fmt, ##arg)
#else
#define PWRAPFUC(fmt, arg...)   /*PRINTF_D(PWRAPTAG "%s\n", __FUNCTION__)*/
#define PWRAPLOG(fmt, arg...)   /*PRINTF_D(PWRAPTAG fmt, ##arg)*/
#endif
#define PWRAPERR(fmt, arg...)   PRINTF_E(PWRAPTAG "ERROR, line=%d " fmt, __LINE__, ##arg)
#else
### Compile error, check SW ENV define
#endif

/**********************************************************/
#define DEFAULT_VALUE_READ_TEST                 (0x5aa5)
#define PWRAP_WRITE_TEST_VALUE                  (0x1234)

#if (PMIC_WRAP_SSPM)
#define UINT32  unsigned int
#define UINT32P unsigned int *
#endif /* end #if (PMIC_WRAP_SSPM) */
/**********************************************************/

#define ENABLE          (1)
#define DISABLE         (0)
#define DISABLE_ALL     (0)

/* HIPRIS_ARB */
#define MDINF       (1 << 0)
#define C2KINF          (1 << 1)
#define MD_DVFSINF      (1 << 2)
#define WACS1           (1 << 3)
#define WACS0           (1 << 4)
#define DVFSINF         (1 << 5)
#define WACS3           (1 << 6)
#define WACSMD32        (1 << 7)
#define DCXO_CONNINF    (1 << 8)
#define DCXO_NFCINF     (1 << 9)
#define MD_ADCINF       (1 << 10)
#define STAUPD          (1 << 11)
#define GPSINF          (1 << 12)
#define WACS2           (1 << 13)/* 0x2000 */
#define WACSP2P         (1 << 14)

/* MUX SEL */
#define WRAPPER_MODE    (0)
#define MANUAL_MODE     (1)

/* macro for MAN_RDATA  FSM */
#define MAN_FSM_NO_REQ             (0x00)
#define MAN_FSM_IDLE               (0x00)
#define MAN_FSM_REQ                (0x02)
#define MAN_FSM_WFDLE              (0x04) /* wait for dle,wait for read data done, */
#define MAN_FSM_WFVLDCLR           (0x06)

/* macro for WACS_FSM */
#define WACS_FSM_IDLE               (0x00)
#define WACS_FSM_REQ                (0x02) /* request in process */
#define WACS_FSM_WFDLE              (0x04) /* wait for dle,wait for read data done, */
#define WACS_FSM_WFVLDCLR           (0x06) /* finish read data , wait for valid flag clearing */
#define WACS_INIT_DONE              (0x01)
#define WACS_SYNC_IDLE              (0x01)
#define WACS_SYNC_BUSY              (0x00)

/**** timeout time, unit :us ***********/
#define TIMEOUT_RESET           (0xFF)
#define TIMEOUT_READ            (0xFF)
#define TIMEOUT_WAIT_IDLE       (0xFF)

/*-----macro for manual commnd ---------------------------------*/
#define OP_WR    (0x1)
#define OP_RD    (0x0)
#define OP_CSH   (0x0)
#define OP_CSL   (0x1)
#define OP_CK    (0x2)
#define OP_OUTS  (0x8)
#define OP_OUTD  (0x9)
#define OP_OUTQ  (0xA)
#define OP_INS   (0xC)
#define OP_INS0  (0xD)
#define OP_IND   (0xE)
#define OP_INQ   (0xF)
#define OP_OS2IS (0x10)
#define OP_OS2ID (0x11)
#define OP_OS2IQ (0x12)
#define OP_OD2IS (0x13)
#define OP_OD2ID (0x14)
#define OP_OD2IQ (0x15)
#define OP_OQ2IS (0x16)
#define OP_OQ2ID (0x17)
#define OP_OQ2IQ (0x18)
#define OP_OSNIS (0x19)
#define OP_ODNID (0x1A)

/******************Error handle *****************************/
#define E_PWR_INVALID_ARG               (1)
#define E_PWR_INVALID_RW                (2)
#define E_PWR_INVALID_ADDR              (3)
#define E_PWR_INVALID_WDAT              (4)
#define E_PWR_INVALID_OP_MANUAL         (5)
#define E_PWR_NOT_IDLE_STATE            (6)
#define E_PWR_NOT_INIT_DONE             (7)
#define E_PWR_NOT_INIT_DONE_READ        (8)
#define E_PWR_WAIT_IDLE_TIMEOUT         (9)
#define E_PWR_WAIT_IDLE_TIMEOUT_READ    (10)
#define E_PWR_INIT_SIDLY_FAIL           (11)
#define E_PWR_RESET_TIMEOUT             (12)
#define E_PWR_TIMEOUT                   (13)
#define E_PWR_INIT_RESET_SPI            (20)
#define E_PWR_INIT_SIDLY                (21)
#define E_PWR_INIT_REG_CLOCK            (22)
#define E_PWR_INIT_ENABLE_PMIC          (23)
#define E_PWR_INIT_DIO                  (24)
#define E_PWR_INIT_CIPHER               (25)
#define E_PWR_INIT_WRITE_TEST           (26)
#define E_PWR_INIT_ENABLE_CRC           (27)
#define E_PWR_INIT_ENABLE_DEWRAP        (28)
#define E_PWR_READ_TEST_FAIL            (30)
#define E_PWR_WRITE_TEST_FAIL           (31)
#define E_PWR_SWITCH_DIO                (32)

/*-----macro for read/write register -------------------------------------*/
#if (PMIC_WRAP_SSPM)
#define WRAP_RD32_WACS2(addr)            (ispeed_single_read(addr))
#define WRAP_WR32_WACS2(addr, data)       (ispeed_single_write(addr, data))
#endif
/**************** end ---external API***********************************/

#endif /*__PMIC_WRAP_INIT_H__*/
