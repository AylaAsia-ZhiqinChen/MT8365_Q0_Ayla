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
#ifndef __NANDX_CORE_H__
#define __NANDX_CORE_H__

#include "nandx_info.h"

/*
 * now we just use multi operation in mntl,
 * so set single plane in pl & lk & mtd.
 */
#define DO_SINGLE_PLANE_OPS false

struct nandx_chip_dev;
struct wl_order_program;
typedef int (*order_program_cb) (struct nandx_chip_dev *,
                                 struct wl_order_program *,
                                 struct nandx_ops *, int, bool, bool);

struct wl_order_program {
	u32 wl_num;
	u32 total_wl_num;
	struct nandx_ops **ops_list;
	order_program_cb order_program_func;
};

struct nandx_core {
	struct nandx_chip_info *info;
	struct nandx_chip_dev *chip;
	struct wl_order_program program;
	struct platform_data *pdata;
};

struct nandx_ops *alloc_ops_table(int count);
void free_ops_table(struct nandx_ops *ops_table);
int nandx_core_read(struct nandx_ops *ops_table, int count, u32 mode);
int nandx_core_write(struct nandx_ops *ops_table, int count, u32 mode);
int nandx_core_erase(u32 *rows, int count, u32 mode);
bool nandx_core_is_bad(u32 row);
int nandx_core_mark_bad(u32 row);

int nandx_core_suspend(void);
int nandx_core_resume(void);
struct nandx_core *nandx_core_init(struct platform_data *pdata, u32 mode);
int nandx_core_exit(void);
void nandx_core_free(void);
struct nandx_chip_info *get_chip_info(void);
struct nandx_core *get_nandx_core(void);

#endif              /* __NANDX_CORE_H__ */
