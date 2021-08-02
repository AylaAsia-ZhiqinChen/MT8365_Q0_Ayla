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

#ifndef __NANDX_CHIP_H__
#define __NANDX_CHIP_H__

#include "nandx_errno.h"
#include "nandx_util.h"
#include "nandx_info.h"
#include "nfc_core.h"

enum PROGRAM_ORDER {
    PROGRAM_ORDER_NONE,
    PROGRAM_ORDER_TLC,
    PROGRAM_ORDER_VTLC_TOSHIBA,
    PROGRAM_ORDER_VTLC_MICRON,
};

struct nandx_chip_dev {
    struct nandx_chip_info info;
    u8 program_order_type;
    int (*read_page)(struct nandx_chip_dev *, struct nandx_ops *);
    int (*cache_read_page)(struct nandx_chip_dev *, struct nandx_ops **,
                           int);
    int (*multi_read_page)(struct nandx_chip_dev *, struct nandx_ops **,
                           int);
    int (*multi_cache_read_page)(struct nandx_chip_dev *,
                                 struct nandx_ops **, int);
    int (*program_page)(struct nandx_chip_dev *, struct nandx_ops **,
                        int);
    int (*cache_program_page)(struct nandx_chip_dev *,
                              struct nandx_ops **, int);
    int (*multi_program_page)(struct nandx_chip_dev *,
                              struct nandx_ops **, int);
    int (*multi_cache_program_page)(struct nandx_chip_dev *,
                                    struct nandx_ops **, int);
    int (*erase)(struct nandx_chip_dev *, u32);
    int (*multi_erase)(struct nandx_chip_dev *, u32 *);
    int (*multi_plane_check)(struct nandx_chip_dev *, u32 *);
    bool (*block_is_bad)(struct nandx_chip_dev *, u32);
    int (*change_mode)(struct nandx_chip_dev *, enum OPS_MODE_TYPE, bool,
                       void *);
    bool (*get_mode)(struct nandx_chip_dev *, enum OPS_MODE_TYPE);
    int (*suspend)(struct nandx_chip_dev *);
    int (*resume)(struct nandx_chip_dev *);
};

struct nandx_chip_dev *nandx_chip_alloc(struct nfc_resource *res);
void nandx_chip_free(struct nandx_chip_dev *chip_dev);

#endif              /* __NANDX_CHIP_H__ */
