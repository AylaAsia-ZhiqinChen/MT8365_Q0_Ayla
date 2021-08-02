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
/* MediaTek Inc. (C) 2018. All rights reserved.
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

#ifndef _LOGGER_H_
#define _LOGGER_H_

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "timers.h"
#include <string.h>
#include <stdio.h>
#include <driver_api.h>
#ifdef CFG_XGPT_SUPPORT
#include <xgpt.h>
#endif

#include <mt_logger.h>

#define PLT_LOG_ENABLE         0x504C5402  /* magic */
#ifndef DRAM_BUF_LEN
#define DRAM_BUF_LEN           0x200000    /* default setup to 2MB */
#endif

#define LOGGER_DRAM_OFF_TIME   50000	   /* 50 seconds */
#define LOGGER_WAIT_TIMEOUT_NS 1000000000  /* 1 second   */

#define LOGGER_ON_BIT          (1<<0)	   /* bit0 = 1, logger is on, else off */
#define LOGGER_DRAM_ON_BIT     (1<<1)	   /* bit1 = 1, logger_dram_use is on, else off */
#define LOGGER_ON_CTRL_BIT     (1<<8)	   /* bit8 = 1, enable function (logger/logger dram use) */
#define LOGGER_OFF_CTRL_BIT    (0<<8)	   /* bit8 = 0, disable function */
#define LOGGER_ON              (LOGGER_ON_CTRL_BIT | LOGGER_ON_BIT)	  /* let logger on */
#define LOGGER_OFF             (LOGGER_OFF_CTRL_BIT | LOGGER_ON_BIT)	  /* let logger off */
#define LOGGER_DRAM_ON         (LOGGER_ON_CTRL_BIT | LOGGER_DRAM_ON_BIT)  /* let logger dram use on */
#define LOGGER_DRAM_OFF        (LOGGER_OFF_CTRL_BIT | LOGGER_DRAM_ON_BIT) /* let logger dram use off */

/*
 * Members may need alignment for cache line,
 * so use _XXXX_XXX_S_ to redefine these structures
 * Otherwise, default as below
 */
#ifndef _BUFFER_INFO_S_
struct buffer_info_s {
	unsigned int r_pos;
	unsigned int w_pos;
};
#endif

#ifndef _DRAM_CTRL_S_
struct dram_ctrl_s {
	unsigned int base;
	unsigned int size;
	unsigned int enable;
	unsigned int info_ofs;
	unsigned int buff_ofs;
	unsigned int buff_size;
};
#endif

/*
 * This structure is used to record logger info,
 * and these info need to sync with AP-side
 */
typedef struct {
	unsigned int log_dram_addr;
	unsigned int log_buf_addr;
	unsigned int log_start_addr;
	unsigned int log_end_addr;
	unsigned int log_buf_maxlen;
} LAST_LOG_INFO;

#if LOGGER_BUFFERABLE
extern unsigned int logger_w_pos;
extern unsigned int logger_r_pos;
extern unsigned char logger_buf[BUF_LEN];
#endif

extern void logger_init(void);
extern void logger_puts(const char *str, int length);
extern void logger_update(const char *str, unsigned int src_len);
extern unsigned int logger_dram_register(unsigned int dram_ctrl_phy, unsigned int limit);
extern unsigned int logger_update_enable(unsigned int enable);
extern unsigned int logger_update_wakeup_ap(unsigned int enable);

#endif
