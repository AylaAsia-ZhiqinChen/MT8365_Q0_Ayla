/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2015. All rights reserved.
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
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef _DMA_H_
#define _DMA_H_
#include <tinysys_reg.h>
#include "semphr.h"
#include "mt_printf.h"
#include <mt_dma.h>

/* max transaction size*/
#define GDMA_REG_BANK_SIZE      (0x50)
#define GDMA_START (0x0)
#define MAX_DMA_TRAN_SIZE 262136 //0xFFFF -> 4 * 65534

/* burst */
#define DMA_CON_BURST_SINGLE    (0x00000000)
#define DMA_CON_BURST_4BEAT     (0x00000002)
#define DMA_CON_BURST_8BEAT     (0x00000003)
#define DMA_CON_BURST_16BEAT    (0x00000004)

#ifndef DMA_CON_BURST_BEAT
#define DMA_CON_BURST_BEAT      DMA_CON_BURST_SINGLE
#endif

/* size*/
#define DMA_CON_SIZE_1BYTE   (0x00000000)
#define DMA_CON_SIZE_2BYTE   (0x00000001)
#define DMA_CON_SIZE_4BYTE   (0x00000002)

/*
 * Register Definition
 */
#define DMA_BASE g_dma_base
#define DMA_BASE_CH(n)      (DMA_BASE + 0x0100 * (n + 1))
#define DMA_GLOBAL_INT_FLAG (DMA_BASE + 0x0000)
#define DMA_GLBSTA   DMA_BASE

/*
 * General DMA channel register mapping:
 */
#define DMA_SRC(base)       (base + 0x0000)
#define DMA_DST(base)       (base + 0x0004)
#define DMA_WPPT(base)      (base + 0x0008)
#define DMA_WPTO(base)      (base + 0x000C)
#define DMA_COUNT(base)     (base + 0x0010)
#define DMA_CON(base)       (base + 0x0014)
#define DMA_START(base)     (base + 0x0018)
#define DMA_INSTSTA(base)   (base + 0x001C)
#define DMA_ACKINT(base)    (base + 0x0020)
#define DMA_RLCT(base)      (base + 0x0024)
#define DMA_LIMITER(base)   (base + 0x0028)
#define DMA_PGMADDR(base)   (base + 0x002C)
#define DMA_WRPTR(base)     (base + 0x0030)
#define DMA_RDPTR(base)     (base + 0x0034)
#define DMA_FFCNT(base)     (base + 0x0038)
#define DMA_FFSTA(base)     (base + 0x003C)
#define DMA_ALTLEN(base)    (base + 0x0040)
#define DMA_FFSIZE(base)    (base + 0x0044)

#define DMA_CON_SIZE  (0)
#define DMA_CON_DINC  (2)
#define DMA_CON_SINC  (3)
#define DMA_CON_BURST (8)
#define DMA_CON_ITEN (15)

/*
 * Register Setting
 */
#define DMA_GLBSTA_RUN(ch)      (0x00000001 << ((ch*2)))
#define DMA_GLBSTA_IT(ch)       (0x00000002 << ((ch*2)))

#define DMA_CON_DIR             (0x00000001)
#define DMA_CON_FPEN            (0x00000002)	/* Use fix pattern. */
#define DMA_CON_SLOW_EN         (0x00000004)
#define DMA_CON_DFIX            (0x00000008)
#define DMA_CON_SFIX            (0x00000010)
#define DMA_CON_WPEN            (0x00020000)
#define DMA_CON_WPSD            (0x00010000)
#define DMA_CON_WSIZE_1BYTE     (0x00000000)
#define DMA_CON_WSIZE_2BYTE     (0x01000000)
#define DMA_CON_WSIZE_4BYTE     (0x02000000)
#define DMA_CON_RSIZE_1BYTE     (0x00000000)
#define DMA_CON_RSIZE_2BYTE     (0x10000000)
#define DMA_CON_RSIZE_4BYTE     (0x20000000)
#define DMA_CON_BURST_MASK      (0x00070000)
#define DMA_CON_SLOW_OFFSET     (5)
#define DMA_CON_SLOW_MAX_MASK   (0x000003FF)

#define DMA_START_BIT           (0x00008000)
#define DMA_START_CLR_BIT       (0x00008000)
#define DMA_ACK_BIT             (0x00008000)
#define DMA_INT_EN_BIT          (0x00000001)

/*
 * Register Limitation
 */
#define MAX_TRANSFER_LEN        (0x0000FFFF)
#define MAX_SLOW_DOWN_CNTER     (0x000000FF)

/* DMA result value */
typedef enum {
	DMA_RESULT_RUNNING = 1,
	DMA_RESULT_DONE = 0,
	DMA_RESULT_NO_FREE_CH = -1,
	DMA_RESULT_ERROR = -2,
	DMA_RESULT_INVALID_CH = -3,
	DMA_RESULT_CH_FREE = -4
} DMA_RESULT;

/* define DMA ISR callback function's prototype */
typedef void (*DMA_ISR_CALLBACK) (void *);

typedef enum {
	DMA_FALSE = 0,
	DMA_TRUE
} DMA_BOOL;

typedef enum {
	ALL = 0,
	SRC,
	DST,
	SRC_AND_DST
} DMA_CONF_FLAG;

typedef enum {
	ASYNC = 0,
	SYNC
} DMA_SYNC;

/* define GDMA configurations */
struct mt_gdma_conf {
	unsigned int count;
	unsigned int size_per_count;
	int iten;
	unsigned int burst;
	int dinc;
	int sinc;
	unsigned int limiter;
	unsigned int src;
	unsigned int dst;
	int wpen;
	int wpsd;
	unsigned int wplen;
	unsigned int wpto;
	int mode;
	int dir;
	void (*isr_cb) (void *);
	void *data;
	DMA_SYNC flag;
};

/*
 * DMA channel struct
 */
struct dma_channel_t {
	int8_t dma_id;
	int32_t in_use;
	uint64_t last_enter;
	uint64_t last_exit;
	uint64_t max_duration;
	uint32_t ch_usage_count;
	SemaphoreHandle_t xDMASemaphore;
	void (*isr_cb) (void *);
	void *data;
	DMA_SYNC flag;
};

int32_t mt_start_gdma(int32_t channel);
int32_t mt_config_gdma(int32_t channel, struct mt_gdma_conf *config,
		       DMA_CONF_FLAG flag);
int32_t mt_polling_gdma(int32_t channel, uint32_t timeout);
int32_t mt_stop_gdma(int32_t channel);
int32_t mt_dump_gdma(int32_t channel);
int32_t mt_check_dma_channel(int32_t chan);
int32_t mt_free_gdma(int32_t chan);
int32_t mt_req_gdma(int32_t chan);
void mt_reset_gdma_conf(const uint32_t iChannel);
int32_t mt_request_dma_channel(int8_t dma_id);
void mt_dump_dma_struct(void);
DMA_RESULT dma_transaction(uint32_t dst_addr, uint32_t src_addr, uint32_t len,
			   int8_t dma_id, int32_t ch, void (*isr_cb) (void *),
			   DMA_SYNC flag);
int32_t mt_init_dma(void);
void mt_cleanup_dma_operations(void);

void get_emi_semaphore(void);
void release_emi_semaphore(void);

#if DMA_KEEP_AWAKE
void dma_wake_lock_init(void);
void dma_wake_lock(void);
void dma_wake_unlock(void);
#endif

#endif /* !_DMA_H_ */
