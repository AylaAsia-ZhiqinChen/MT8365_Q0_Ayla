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
/*! \file
*    \brief  Declaration of library functions
*
*    Any definitions in this file will be shared among GLUE Layer and internal Driver Stack.
*/

#ifndef _CONNINFRA_CORE_H_
#define _CONNINFRA_CORE_H_

#include <linux/semaphore.h>
#include <linux/platform_device.h>

#include "osal.h"
#include "msg_thread.h"
#include "conninfra.h"

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef enum _ENUM_DRV_STS_ {
	DRV_STS_POWER_OFF = 0,	/* initial state */
	DRV_STS_POWER_ON = 1,	/* powered on */
	DRV_STS_RESET = 2,
	DRV_STS_MAX
} ENUM_DRV_STS, *P_ENUM_DRV_STS;

struct subsys_drv_inst {
	ENUM_DRV_STS drv_status;	/* Controlled driver status */
	unsigned int rst_state;
	struct sub_drv_ops_cb ops_cb;
	struct msg_thread_ctx msg_ctx;
};


/*
 * state of conninfra
 *
 */
struct conninfra_ctx {
	ENUM_DRV_STS infra_drv_status;

	struct subsys_drv_inst drv_inst[CONNDRV_TYPE_MAX];
	/*struct spinlock infra_lock;*/
	spinlock_t infra_lock;

	OSAL_SLEEPABLE_LOCK rst_lock;
	struct semaphore rst_sema;
	atomic_t rst_state;

	struct semaphore pre_cal_sema;
	atomic_t pre_cal_state;

	struct msg_thread_ctx msg_ctx;
	struct msg_thread_ctx cb_ctx;

	unsigned int hw_ver;
	unsigned int fw_ver;
	unsigned int ip_ver;

	struct osal_op_history cored_op_history;

};

//typedef enum _ENUM_CONNINFRA_CORE_OPID_T {
typedef enum {
	CONNINFRA_OPID_PWR_ON 		= 0,
	CONNINFRA_OPID_PWR_OFF		= 1,
	CONNINFRA_OPID_THERM_CTRL	= 2,
	CONNINFRA_OPID_MAX
} conninfra_core_opid;

/* For the operation which may callback subsys driver */
typedef enum {
	CONNINFRA_CB_OPID_CHIP_RST         = 0,
	CONNINFRA_CB_OPID_PRE_CAL          = 1,
	CONNINFRA_CB_OPID_MAX
} conninfra_core_cb_opid;

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

extern int conninfra_core_init(void);
extern int conninfra_core_deinit(void);

int conninfra_core_power_on(enum consys_drv_type type);
int conninfra_core_power_off(enum consys_drv_type type);

int conninfra_core_lock_rst(void);
int conninfra_core_unlock_rst(void);
int conninfra_core_trg_chip_rst(enum consys_drv_type drv, char *reason);

int conninfra_core_subsys_ops_reg(enum consys_drv_type type,
						struct sub_drv_ops_cb *cb);
int conninfra_core_subsys_ops_unreg(enum consys_drv_type type);

int conninfra_core_pre_cal_start(void);

/* NOTE: NOT thread-safe
 * return value
 * 1 : Yes, 0: NO
 */
int conninfra_core_reg_readable(void);
int conninfra_core_is_consys_reg(phys_addr_t addr);

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif				/* _CONNINFRA_CORE_H_ */
