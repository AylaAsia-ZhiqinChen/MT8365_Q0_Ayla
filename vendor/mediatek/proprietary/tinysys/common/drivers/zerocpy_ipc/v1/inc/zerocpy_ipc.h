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
/* MediaTek Inc. (C) 2019. All rights reserved.
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

#include "FreeRTOS.h"
#include "task.h"
#include <tinysys_reg.h>

#ifdef bool
#undef bool
#endif
#define bool uint32_t

#ifdef TRUE
#undef TRUE
#endif
#define TRUE 1

#ifdef FALSE
#undef FALSE
#endif
#define FALSE 0

#define IPC_SENDER	1
#define IPC_RECEIVER 	0
typedef void (*zerocpy_ipc_handler_t) (int ipc_id);

typedef struct {
	uint32_t rd;
	uint32_t wb;
	uint32_t elems[];
} z_rbuffer;

typedef struct {
	TaskHandle_t task;
	zerocpy_ipc_handler_t handler[2];
	int rbuff_size;
	z_rbuffer *rbuff[2];
} zerocpy_id_info;

typedef struct ipc_chan {
	void (*zerocpy_ipc_reg_set) (void);
	void (*zerocpy_ipc_reg_clr) (void);
	int zerocpy_ids;	/* id counts */
	uint32_t rbuff_size;
	z_rbuffer *rbuff[2];
	zerocpy_id_info *id_info;
} ipc_chan_st;

#define C2C_RBUFFER_ST_SIZE	(sizeof(z_rbuffer)/sizeof(uint32_t))
#define RINGBUFFER_DECL(name, numitems)	uint32_t name##_store[2] [numitems + C2C_RBUFFER_ST_SIZE] __attribute__((section (".sync")));

void c2c_ipc_plat_init(void);
void c2c_ipc_channel_init(ipc_chan_st * ipcc);
void *zerocpy_ipc_get_buff_ptr(ipc_chan_st * icc, int id);
int zerocpy_ipc_send(ipc_chan_st * icc, int id, void *buff_ptr);
void zerocpy_ipc_register(ipc_chan_st * icc, int id,
			  zerocpy_ipc_handler_t handler);
void zerocpy_ipc_receive(ipc_chan_st * icc, int id);

#define SYNC_DECLARE_OS_ALIGNMENT(nam, size, struct_type) uint8_t _##nam##_store [size] __attribute__((aligned(4),section(".sync"))); struct_type *nam = (struct_type *)_##nam##_store
#define SYNC_ATOMIC_BITSET_DECL(nam, numbits)    SYNC_DECLARE_OS_ALIGNMENT(nam, ATOMICBIT_SZ(numbits), struct atomicbit_set)

void buff_in_use_initial(void);
uint32_t buff_in_use(void *ptr, uint32_t use);
uint32_t is_buff_in_use(void *ptr);
