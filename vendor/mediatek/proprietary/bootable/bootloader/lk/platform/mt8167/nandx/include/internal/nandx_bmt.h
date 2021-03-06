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
#ifndef __NANDX_BMT_H__
#define __NANDX_BMT_H__

#include "nandx_util.h"
#include "nandx_info.h"

#define DATA_BAD_BLK        0xffff

#define DATA_MAX_BMT_COUNT      0x400
#define DATA_BMT_VERSION        (1)

struct data_bmt_entry {
	u16 bad_index;      /* bad block index */
	u8 flag;        /* mapping block index in the replace pool */
};

struct data_bmt_struct {
	struct data_bmt_entry entry[DATA_MAX_BMT_COUNT];
	unsigned int version;
	unsigned int bad_count; /* bad block count */
	unsigned int start_block;   /* data partition start block addr */
	unsigned int end_block; /* data partition start block addr */
	unsigned int checksum;
};

enum UPDATE_REASON {
	UPDATE_ERASE_FAIL,
	UPDATE_WRITE_FAIL,
	UPDATE_UNMAPPED_BLOCK,
	UPDATE_REMOVE_ENTRY,
	UPDATE_INIT_WRITE,
	UPDATE_REASON_COUNT,
	INIT_BAD_BLK,
	FTL_MARK_BAD = 64,
};

int nandx_bmt_init(struct nandx_chip_info *dev_info, u32 block_num,
                   bool rebuild);
void nandx_bmt_reset(void);
void nandx_bmt_exit(void);
int nandx_bmt_remark(u32 *blocks, int count, enum UPDATE_REASON reason);
int nandx_bmt_block_isbad(u32 block);
int nandx_bmt_block_markbad(u32 block);
u32 nandx_bmt_update(u32 bad_block, enum UPDATE_REASON reason);
void nandx_bmt_update_oob(u32 block, u8 *oob);
u32 nandx_bmt_get_mapped_block(u32 block);
int nandx_bmt_get_data_bmt(struct data_bmt_struct *data_bmt);
int nandx_bmt_init_data_bmt(u32 start_block, u32 end_block);

#endif              /* #ifndef __NANDX_BMT_H__ */
