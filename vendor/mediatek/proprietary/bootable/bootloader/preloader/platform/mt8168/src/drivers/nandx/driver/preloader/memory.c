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
#include "dram_buffer.h"

#define MEM_MAP_NUM     100

struct buffer_map {
    void *start;
    u32 used;
    u32 size;
};

static bool need_init = true;
static struct buffer_map *mem_map;

static void mem_init(void)
{
    int i;
    size_t buffer_start = (size_t)g_dram_buf->nandx_buf;
    size_t buffer_len = NANDX_BUF_SIZE;

    buffer_len = buffer_len - sizeof(struct buffer_map) * MEM_MAP_NUM;
    mem_map = (struct buffer_map *)(buffer_start + buffer_len);
    mem_map[0].start = (void *)buffer_start;
    mem_map[0].used = 0;
    mem_map[0].size = buffer_len;

    for (i = 1; i < MEM_MAP_NUM; i++) {
        mem_map[i].start = NULL;
        mem_map[i].used = 0;
        mem_map[i].size = 0;
    }
}

void *mem_alloc(u32 count, u32 size)
{
    int i, j;
    u32 alloc_size = count * size;

    if (need_init) {
        need_init = false;
        mem_init();
    }

    for (i = 0; i < MEM_MAP_NUM; i++) {
        if (mem_map[i].size < alloc_size || mem_map[i].used != 0)
            continue;

        mem_map[i].used = 1;
        for (j = 0; j < MEM_MAP_NUM; j++) {
            if (mem_map[j].used != 0 || mem_map[j].size != 0)
                continue;

            mem_map[j].start = mem_map[i].start + alloc_size;
            mem_map[j].size = mem_map[i].size - alloc_size;
            mem_map[i].size = alloc_size;

            return mem_map[i].start;
        }
        break;
    }

    if (i == MEM_MAP_NUM)
        return NULL;

    return mem_map[i].start;
}

void mem_free(void *mem)
{
    int i, j, k;
    void *end;

    if (!mem)
        return;

    for (i = 0; i < MEM_MAP_NUM; i++) {
        if (mem_map[i].start != mem || mem_map[i].used != 1)
            continue;

        mem_map[i].used = 0;
        for (j = 0; j < MEM_MAP_NUM; j++) {
            end = mem_map[i].start + mem_map[i].size;
            if (end != mem_map[j].start || mem_map[j].used != 0)
                continue;

            mem_map[j].start = NULL;
            mem_map[i].size += mem_map[j].size;
            mem_map[j].size = 0;
            break;
        }

        for (k = 0; k < MEM_MAP_NUM; k++) {
            end = mem_map[k].start + mem_map[k].size;
            if (end != mem_map[i].start || mem_map[k].used != 0)
                continue;

            mem_map[i].start = NULL;
            mem_map[k].size += mem_map[i].size;
            mem_map[i].size = 0;
            break;
        }
        break;
    }
}
