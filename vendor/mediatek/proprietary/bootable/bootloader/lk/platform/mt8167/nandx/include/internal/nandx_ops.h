/*
 * MediaTek Inc. (C) 2018. All rights reserved.
 *
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
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
#ifndef __OPS_H__
#define __OPS_H__

#include "nandx_util.h"
#include "nandx_core.h"

struct nandx_lock {
	void *lock;
	wait_queue_head_t wq;
	int state;
};

typedef int (*nandx_core_rw_cb) (struct nandx_ops *, int, u32);

int nandx_ops_read(struct nandx_core *dev, long long from,
                   size_t len, u8 *buf, bool do_multi);
int nandx_ops_write(struct nandx_core *dev, long long to,
                    size_t len, u8 *buf, bool do_multi);
int nandx_ops_read_oob(struct nandx_core *dev, long long to, u8 *oob);
int nandx_ops_write_oob(struct nandx_core *dev, long long to, u8 *oob);
int nandx_ops_erase_block(struct nandx_core *dev, long long laddr);
int nandx_ops_erase(struct nandx_core *dev, long long offs,
                    long long limit, size_t size);
int nandx_ops_mark_bad(u32 block, int reason);
int nandx_ops_isbad(long long offs);
u32 nandx_ops_addr_transfer(struct nandx_core *dev, long long laddr,
                            u32 *blk, u32 *map_blk);
u32 nandx_get_chip_block_num(struct nandx_chip_info *info);
u32 nandx_calculate_bmt_num(struct nandx_chip_info *info);
void dump_nand_info(struct nandx_chip_info *info);
bool randomizer_is_support(enum IC_VER ver);
struct nandx_core *nandx_device_init(u32 mode);
int nandx_get_device(int new_state);
void nandx_release_device(void);
void nandx_lock_init(void);
struct nandx_lock *get_nandx_lock(void);

#endif              /* __OPS_H__ */
