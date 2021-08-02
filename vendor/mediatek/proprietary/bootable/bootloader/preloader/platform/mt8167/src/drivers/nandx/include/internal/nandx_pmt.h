/*
 * MediaTek Inc. (C) 2019. All rights reserved.
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

#ifndef __NANDX_PMT_H__
#define __NANDX_PMT_H__

#include "nandx_util.h"
#include "nandx_info.h"

#define MAX_PARTITION_NAME_LEN  64

#define REGION_LOW_PAGE     0x004C4F57
#define REGION_FULL_PAGE    0x46554C4C
#define REGION_SLC_MODE     0x00534C43
#define REGION_TLC_MODE     0x00544C43
#define PT_SIG          0x50547633  /* "PTv3" */
#define MPT_SIG         0x4D505433  /* "MPT3" */
#define PT_SIG_SIZE     4
#define PT_TLCRATIO_SIZE    4
#define is_valid_pt(buf)    (!memcmp(buf, "3vTP", 4))
#define is_valid_mpt(buf)   (!memcmp(buf, "3TPM", 4))

#define PMT_POOL_SIZE       (2)
#undef PART_MAX_COUNT
#define PART_MAX_COUNT      (40)

struct extension {
    unsigned int type;
    unsigned int attr;
};

struct pt_resident {
    char name[MAX_PARTITION_NAME_LEN];  /* partition name */
    unsigned long long size;    /* partition size */
    union {
        unsigned long long part_id;
        struct extension ext;
    };
    unsigned long long offset;  /* partition start */
    unsigned long long mask_flags;  /* partition flags */

};

/**
 * struct pmt_handler - partition table handler
 * @info: struct nandx_chip_info which contains nand device info
 * @pmt: partition table buffer
 * @block_bitmap: block type bitmap buffer for mntl
 * @start_blk: start block of partition region
 * @pmt_page: the latest pmt page number in pmt block
 * @part_num: partition number of current pmt
 * @sys_slc_ratio: slc ratio of system partition
 * @usr_slc_ratio: slc ratio of user partition
 */
struct pmt_handler {
    struct nandx_chip_info *info;
    struct pt_resident *pmt;
    u32 *block_bitmap;
    u32 start_blk;
    u32 pmt_page;
    u32 part_num;
    u32 sys_slc_ratio;
    u32 usr_slc_ratio;
};

int nandx_pmt_init(struct nandx_chip_info *info, u32 start_blk);
void nandx_pmt_exit(void);
int nandx_pmt_update(void);
struct pt_resident *nandx_pmt_get_partition(u64 addr);
u64 nandx_pmt_get_start_address(struct pt_resident *pt);
bool nandx_pmt_is_raw_partition(struct pt_resident *pt);
struct pmt_handler *nandx_get_pmt_handler(void);
int nandx_pmt_addr_to_row(u64 addr, u32 *block, u32 *page);
bool nandx_pmt_blk_is_slc(u64 addr);
#endif
