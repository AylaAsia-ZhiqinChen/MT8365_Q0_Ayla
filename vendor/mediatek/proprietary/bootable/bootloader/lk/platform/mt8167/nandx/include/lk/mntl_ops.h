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

#ifndef __MNTL_OPS_H__
#define __MNTL_OPS_H__

#include "mtk_nand_chip.h"
#include "nandx_bmt.h"

/**********  PMT Related ***********/

#define FTL_PARTITION_NAME  "USRDATA"

struct nand_ftl_partition_info {
	unsigned int start_block;   /* Number of data blocks */
	unsigned int total_block;   /* Number of block */
	unsigned int slc_ratio; /* FTL SLC ratio here */
	unsigned int slc_block; /* FTL SLC ratio here */
};

enum operation_types {
	MTK_NAND_OP_READ = 0,
	MTK_NAND_OP_WRITE,
	MTK_NAND_OP_ERASE,
};

#define containerof(ptr, type, member) \
    ((type *)((unsigned long)(ptr) - __builtin_offsetof(type, member)))

struct mtk_nand_chip_operation {
	struct mtk_nand_chip_info *info;    /* Data info */
	enum operation_types types;
	/* Operation type, 0: Read, 1: write, 2:Erase */
	int block;
	int page;
	int offset;
	int size;
	bool more;
	unsigned char *data_buffer;
	unsigned char *oob_buffer;
	mtk_nand_callback_func callback;
	void *userdata;
};

struct nand_work {
	struct list_node list;
	struct mtk_nand_chip_operation ops;
};

enum worklist_type {
	LIST_ERASE = 0,
	LIST_SLC_WRITE,
	LIST_NS_WRITE,      /* none slc write list: mlc or tlc */
};

struct worklist_ctrl;

typedef unsigned int (*get_ready_count) (struct mtk_nand_chip_info *info,
        struct worklist_ctrl *list_ctrl,
        int total);

typedef unsigned int (*process_list_data) (struct mtk_nand_chip_info *info,
        struct worklist_ctrl *list_ctrl,
        int count);

struct worklist_ctrl {
	enum worklist_type type;
	struct list_node head;
	int total_num;
	/* last write error block list, the num is plane_num */
	int *ewrite;
	get_ready_count get_ready_count_func;
	process_list_data process_data_func;
};

struct err_para {
	int rate;
	int count;      /*max count */
	int block;
	int page;
};
struct sim_err {
	struct err_para erase_fail;
	struct err_para write_fail;
	struct err_para read_fail;
	struct err_para bitflip_fail;
	struct err_para bad_block;
};

struct open_block {
	int max;
	int *array;
};

struct mtk_nand_data_info {
	struct data_bmt_struct bmt;
	struct mtk_nand_chip_bbt_info chip_bbt;
	struct mtk_nand_chip_info chip_info;
	struct nand_ftl_partition_info partition_info;

	struct worklist_ctrl elist_ctrl;
	struct worklist_ctrl swlist_ctrl;
	struct worklist_ctrl wlist_ctrl;

	struct sim_err err;
	struct open_block open;
};

enum TLC_MULTI_PROG_MODE {
	MULTI_BLOCK = 0,
	BLOCK0_ONLY,
	BLOCK1_ONLY,
};

int nandx_mntl_ops_init(void);
int nandx_mntl_data_info_alloc(void);
void nandx_mntl_data_info_free(void);
u32 get_ftl_row_addr(struct mtk_nand_chip_info *info,
                     unsigned int block, unsigned int page);
extern int mntl_update_part_tab(struct mtk_nand_chip_info *info,
                                int num, u32 *blk);
extern int get_data_partition_info(struct nand_ftl_partition_info *info,
                                   struct mtk_nand_chip_info *cinfo);
#endif              /* __MNTL_OPS_H__ */
