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

#include "nandx_util.h"
#include "nandx_errno.h"
#include "cust_part.h"
#include "nandx_pmt.h"
#include "nandx_ops.h"

static u32 part_seccfg_offset;
static u32 part_seccfg_length;
static u32 part_secro_offset;
static u32 part_secro_length;

int pmt_init(void)
{
    part_t *part = cust_part_tbl();
    struct pmt_handler *handler;
    struct pt_resident *pt;
    struct nandx_chip_info *info;
    u32 i, bmt_block;
    u64 offset, size;
    int ret;

    handler = nandx_get_pmt_handler();
    pt = handler->pmt;
    if (pt == NULL)
        return -ENODEV;
    info = handler->info;

    /* assume bmt is always at the last of NAND */
    bmt_block = nandx_calculate_bmt_num(info);

    for (i = 0; i < handler->part_num; i++) {
        part->name = pt->name;
        if (i == (handler->part_num - 1)) {
            offset = (u64)info->block_size *
                     (info->block_num - bmt_block);
            size = (u64)bmt_block * info->block_size;
        } else {
            offset = pt->offset;
            size = pt->size;
        }
        part->size = size;
        part->blks = size / info->page_size;
        part->startblk = offset / info->page_size;
        part->flags = 0;
        part->part_attr = (unsigned long)pt->ext.attr;
        if (pt->ext.type == REGION_LOW_PAGE)
            part->type = TYPE_LOW;
        else
            part->type = TYPE_FULL;

        /* setup seccfg and secro partition info */
        if (!strcmp(pt->name, PART_SECCFG)) {
            part_seccfg_offset = offset;
            part_seccfg_length = size;
        }
        if (!strcmp(pt->name, PART_SEC_RO)) {
            part_secro_offset = offset;
            part_secro_length = size;
        }

        pt++;
        part++;
    }
}

u32 get_seccfg_offset(void)
{
    return part_seccfg_offset;
}

u32 get_seccfg_size(void)
{
    return part_seccfg_length;
}

u32 get_secro_offset(void)
{
    return part_secro_offset;
}

u32 get_secro_length(void)
{
    return part_secro_length;
}

part_t *pmt_get_part(part_t *part, int index)
{
    /*
     * This really looks like a stupid function to transform part to part.
     * I will keep it here anyway. Maybe delete it later.
     */
    return part;
}
