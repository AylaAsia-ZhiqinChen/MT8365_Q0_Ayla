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

#include "typedefs.h"
#include "platform.h"
#include "dram_buffer.h"
#include "nand_core.h"
#include "nandx_bmt.h"
#include "nandx_pmt.h"
#include "partition.h"
#include "nandx_util.h"
#include "nandx_errno.h"
#include "nandx_info.h"
#include "nandx_ops.h"
#include "nandx_core.h"
#include "nandx.h"

static blkdev_t nand_bdev;
struct nandx_core *ncore;

static int ops_prepare(long long addr, char *buf, struct nandx_ops *ops,
                       int count, long long *map_addr)
{
    u8 *oob;
    bool raw_part;
    int i, sec_num, block_page_num;
    int start, len, offset = 0, part_id;
    u32 page_size, block_size;
    u32 mode = 0, row_addr, block, map_block;
    struct pt_resident *pt;

    page_size = ncore->info->page_size;
    block_size = ncore->info->block_size;
    block_page_num = block_size / page_size;

    pt = nandx_pmt_get_partition(addr);
    raw_part = nandx_pmt_is_raw_partition(pt);

    if (raw_part)
        mode |= MODE_SLC;

    ops[0].row = nandx_ops_addr_transfer(ncore, addr, &block, &map_block);

    *map_addr = addr;
    if (block != map_block)
        *map_addr = ops[0].row * page_size;

    for (i = 0; i < count; i++) {
        if (i > 0)
            ops[i].row = ops[i - 1].row + block_page_num;
        ops[i].col = 0;
        ops[i].len = page_size;
        ops[i].data = buf + i * page_size;
        ops[i].oob = NULL;
    }

    return mode;
}

static int nand_read_data(long long addr, char *buf, bool do_multi,
                          long long *map_addr)
{
    int ret;
    u32 mode;
    int plane_num = 1;
    struct nandx_ops ops[2];

    if (do_multi)
        plane_num = ncore->info->plane_num;
    mode = ops_prepare(addr, buf, ops, plane_num, map_addr);
    ret = nandx_core_read(ops, plane_num, mode);
    if (ret == -ENANDFLIPS)
        ret = 0;
    return ret;
}

static int nand_write_data(long long addr, char *buf, bool do_multi,
                           long long *map_addr)
{
    u32 mode;
    int plane_num = 1;
    struct nandx_ops ops[2];

    if (do_multi)
        plane_num = ncore->info->plane_num;
    mode = ops_prepare(addr, buf, ops, plane_num, map_addr);
    return nandx_core_write(ops, plane_num, mode);
}

int nand_erase(long long addr)
{
    return nandx_ops_erase_block(ncore, addr);
}

bool nand_erase_data(long long offs, long long limit, size_t size)
{
    int ret;

    ret = nandx_ops_erase(ncore, offs, limit, size);
    return ret < 0 ? false : true;
}

static int nand_bread(blkdev_t *bdev, u32 blknr, u32 blks, u8 *buf,
                      u32 part_id)
{
    int ret;
    u32 i;
    long long offset, new_offset;

    offset = (u64)blknr * bdev->blksz;
    for (i = 0; i < blks; i++) {
        ret = nand_read_data(offset, buf, DO_SINGLE_PLANE_OPS,
                             &new_offset);
        if (ret < 0)
            return ret;
        offset = new_offset + bdev->blksz;
        buf += bdev->blksz;
    }

    return 0;
}

static int nand_bwrite(blkdev_t *bdev, u32 blknr, u32 blks, u8 *buf,
                       u32 part_id)
{
    int ret;
    u32 i;
    long long offset, new_offset;

    offset = (u64)blknr * bdev->blksz;
    for (i = 0; i < blks; i++) {
        ret = nand_write_data(offset, buf, DO_SINGLE_PLANE_OPS,
                              &new_offset);
        if (ret < 0)
            return ret;
        offset = new_offset + bdev->blksz;
        buf += bdev->blksz;
    }

    return 0;
}

bool randomizer_is_support(enum IC_VER ver)
{
    u32 reg, enable;

    if (ver == NANDX_MT8167) {
        /* randomizer efuse register address */
        reg = nreadl((void *)0x10009020);
        enable = 0x00001000;
    }

    return (reg & enable) ? true : false;
}

int nand_init_device(void)
{
    u32 mode = 0;

    nandx_lock_init();

    mode = MODE_ECC;
    ncore = nandx_device_init(mode);
    if (!ncore)
        return -EIO;

    memset(&nand_bdev, 0, sizeof(blkdev_t));
    nand_bdev.blksz = ncore->info->page_size;
    nand_bdev.erasesz = ncore->info->block_size;
    nand_bdev.blks = ncore->info->block_size * ncore->info->block_num;
    nand_bdev.bread = nand_bread;
    nand_bdev.bwrite = nand_bwrite;
    nand_bdev.blkbuf = (u8 *)g_dram_buf->storage_buffer;
    nand_bdev.type = BOOTDEV_NAND;
    blkdev_register(&nand_bdev);

    return 0;
}

u32 nand_get_device_id(u8 *id, u32 len)
{
    return 0;
}
