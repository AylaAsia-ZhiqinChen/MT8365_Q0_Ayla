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

#include "nandx_util.h"
#include "nandx_errno.h"
#include "nandx_bmt.h"
#include "nandx_info.h"
#include "nandx_core.h"
#include "nandx_ops.h"
#include "mntl_ops.h"

struct nandx_ops *ops_table;
struct mtk_nand_data_info *data_info;

static inline bool block_page_num_is_valid(struct mtk_nand_chip_info *info,
        u32 block, u32 page);

static int open_block_delete(struct open_block *open, int block);

static bool should_close_block(void);

static bool is_ewrite_block(struct mtk_nand_chip_info *info,
                            struct worklist_ctrl *list_ctrl, u32 block);

/*********  MTK Nand Driver Related Functnions *********/

/* mtk_isbad_block
 * Mark specific block as bad block, update bad block list and bad block table
 * @block: block address to markbad
 */
bool mtk_isbad_block(u32 block)
{
	struct mtk_nand_chip_bbt_info *chip_bbt;
	u32 i;

	chip_bbt = &data_info->chip_bbt;
	for (i = 0; i < chip_bbt->bad_block_num; i++) {
		if (block == chip_bbt->bad_block_table[i]) {
			pr_debug("Check block:0x%x is bad\n", block);
			return TRUE;
		}
	}

	return FALSE;
}

static bool is_slc_block(struct mtk_nand_chip_info *info, u32 block)
{
	int index, bit;

	if (info->types != MTK_NAND_FLASH_TLC)
		return block >= info->data_block_num;

	index = block / 32;
	bit = block % 32;

	if ((info->block_type_bitmap[index] >> bit) & 1)
		return FALSE;

	return TRUE;
}

static int mtk_nand_read_pages(struct mtk_nand_chip_info *info,
                               u8 *data_buffer, u8 *oob_buffer,
                               u32 block, u32 page, u32 offset, u32 size)
{
	int fdm_size;
	static u8 *fdm_buf;
#ifdef MTK_FORCE_READ_FULL_PAGE
	static u8 *page_buf;
#endif
	u32 page_addr, page_size, oob_size;
	u32 sect_num, start_sect;
	u32 col_addr, sect_read;
	u32 mode = 0;
	int ret = 0;
	struct nandx_ops ops;

	nandx_get_device(FL_READING);

	page_addr =
	    (block + data_info->bmt.start_block) * info->data_page_num + page;

	/* For log area access */
	if (is_slc_block(info, block)) {
		mode |= MODE_SLC;
		page_size = info->log_page_size;
		oob_size = info->log_oob_size;
	} else {
		page_size = info->data_page_size;
		oob_size = info->log_oob_size;
	}

	if (fdm_buf == NULL) {
		fdm_buf = mem_alloc(1, 1024);
		if (fdm_buf == NULL) {
			ret = -ENOMEM;
			goto exit;
		}
	}

	if (size < page_size) {
		/* Sector read case */
		sect_num = page_size / (1 << info->sector_size_shift);
		start_sect = offset / (1 << info->sector_size_shift);
		col_addr = start_sect * (1 << info->sector_size_shift);
		sect_read = size / (1 << info->sector_size_shift);
		fdm_size = oob_size / sect_num;
#ifdef MTK_FORCE_READ_FULL_PAGE
		if (page_buf == NULL) {
			page_buf = mem_alloc(1, page_size);
			if (page_buf == NULL) {
				ret = -ENOMEM;
				goto exit;
			}
		}
		ops.row = page_addr;
		ops.col = 0;
		ops.len = page_size;
		ops.data = page_buf;
		ops.oob = fdm_buf;
		ret = nandx_core_read(&ops, 1, mode);
		memcpy(data_buffer, page_buf + offset, size);
		memcpy(oob_buffer, fdm_buf + start_sect * fdm_size,
		       sect_read * fdm_size);
#else
		ops.row = page_addr;
		ops.col = col_addr;
		ops.len = size;
		ops.data = data_buffer;
		ops.oob = fdm_buf;
		ret = nandx_core_read(&ops, 1, mode);
		memcpy(oob_buffer, fdm_buf, sect_read * fdm_size);
#endif
	} else {
		ops.row = page_addr;
		ops.col = 0;
		ops.len = page_size;
		ops.data = data_buffer;
		ops.oob = fdm_buf;
		ret = nandx_core_read(&ops, 1, mode);
		memcpy(oob_buffer, fdm_buf, oob_size);
	}

	if (ret < 0) {
		ret = ops.status;
		if (ret == -ENANDFLIPS)
			pr_err("bitflip @ b:%d p:%d\n", block, page);
	} else {
		ret = 0;
	}

exit:
	if (ret)
		pr_err("read err:%d, b:%d p:%d off:%d size:%d\n",
		       ret, block, page, offset, size);

	nandx_release_device();

	return ret;

}

static int mtk_nand_read_multi_pages(struct mtk_nand_chip_info *info,
                                     int page_num,
                                     struct mtk_nand_chip_read_param *param)
{
	u32 mode = 0, ppb;
	int i, ret;
	struct nandx_ops *ops;

	nandx_get_device(FL_READING);

	ops = alloc_ops_table(page_num);
	if (!ops) {
		ret = -ENOMEM;
		goto err;
	}

	ppb = info->data_page_num;
	if (is_slc_block(info, param[0].block)) {
		mode |= MODE_SLC;
		ppb = info->log_page_num;
	}
	if (info->plane_num > 1)
		mode |= MODE_MULTI;

	for (i = 0; i < page_num; i++) {
		ops[i].row = param[i].block * ppb + param[i].page;
		ops[i].col = param[i].offset;
		ops[i].len = param[i].size;
		ops[i].data = param[i].data_buffer;
		ops[i].oob = param[i].oob_buffer;
	}

	ret = nandx_core_read(ops, page_num, mode);

err:
	nandx_release_device();
	return ret;
}

static inline struct list_node *get_list_item_prev(struct list_node *head,
        struct list_node *cur)
{
	struct list_node *item;

	item = head;
	while ((item->next != cur) && (item->next != NULL))
		item = item->next;

	if (item->next != cur) {
		pr_err("check no node cur:%p\n", cur);
		return NULL;
	}

	return item;
}

static inline struct list_node *get_list_tail(struct list_node *head)
{
	struct list_node *item;

	item = head;
	while (item->next != NULL)
		item = item->next;

	return item;
}

static inline struct nand_work *get_list_work(struct list_node *node)
{
	return containerof(node, struct nand_work, list);
}

static struct list_node *seek_list_item(struct list_node *cur, u32 offset)
{
	struct list_node *item;
	int i;

	i = 0;
	item = cur;
	while (i < offset) {
		item = item->next;
		i++;
	}
	return item;
}

static inline int add_list_node(struct worklist_ctrl *list_ctrl,
                                struct list_node *node)
{
	struct list_node *tail;
	struct mtk_nand_chip_operation *ops;
	int ret = 0;

	ops = &get_list_work(node)->ops;
	if (ops->types == MTK_NAND_OP_WRITE &&
	        is_ewrite_block(ops->info, list_ctrl, ops->block)) {
		ops->callback(ops->info, ops->data_buffer, ops->oob_buffer,
		              ops->block, ops->page, -ENANDWRITE,
		              ops->userdata);
		ret = -ENANDWRITE;
		goto OUT;
	}
	tail = get_list_tail(&list_ctrl->head);
	tail->next = node;
	list_ctrl->total_num++;

OUT:

	return ret;
}

static inline int get_list_work_cnt(struct worklist_ctrl *list_ctrl)
{
	int cnt = 0;

	cnt = list_ctrl->total_num;

	return cnt;
}

static bool is_multi_plane(struct mtk_nand_chip_info *minfo)
{
	struct nandx_chip_info *info;

	info = get_chip_info();

	return (info->plane_num > 1) ? true : false;
}

static bool is_multi_read_support(struct mtk_nand_chip_info *info)
{
	return is_multi_plane(info);
}

static bool are_on_diff_planes(u32 block0, u32 block1)
{
	return (block0 + block1) & 0x1;
}

static bool can_multi_plane(u32 block0, u32 page0, u32 block1, u32 page1)
{

	return ((block0 + block1) & 0x1) && (page0 == page1);
}

static bool can_ops_multi_plane(struct mtk_nand_chip_operation *ops0,
                                struct mtk_nand_chip_operation *ops1)
{
	struct mtk_nand_chip_info *info = ops0->info;

	if (!is_multi_plane(info))
		return false;

	return can_multi_plane(ops0->block, ops0->page, ops1->block,
	                       ops1->page);
}

static void call_multi_work_callback(struct list_node *cur, int count,
                                     int status)
{
	int i;
	struct list_node *item;
	struct nand_work *work;
	struct mtk_nand_chip_operation *ops;

	item = cur;
	for (i = 0; i < count; i++) {
		if (item == NULL) {
			pr_err("%s: NULL item\n", __func__);
			return;
		}
		work = get_list_work(item);
		ops = &work->ops;
		ops->callback(ops->info, ops->data_buffer,
		              ops->oob_buffer, ops->block,
		              ops->page, status, ops->userdata);
		if (i >= count)
			break;
		item = item->next;
	}
}

static void call_tlc_page_group_callback(struct list_node *base_node,
        int start, int end, bool multi_plane,
        int status)
{
	struct list_node *item;
	int i;
	int count = multi_plane ? 2 : 1;

	item = seek_list_item(base_node, start * count);
	for (i = 0; i < end - start + 1; i++) {
		call_multi_work_callback(item, count, status);

		if (i >= end - start)
			break;
		if (count == 1)
			item = item->next;
		else
			item = item->next->next;
	}
}

static u32 callback_and_free_ewrite(struct worklist_ctrl *list_ctrl,
                                    int ecount)
{
	int *ewrite;
	int i;
	struct list_node *item;
	struct nand_work *work;
	struct mtk_nand_chip_operation *ops;

	if (ecount != 1 && ecount != 2) {
		pr_err("ecount:%d\n", ecount);
		return 0;
	}
	ewrite = list_ctrl->ewrite;
	item = list_ctrl->head.next;
	i = 0;
	while (item != NULL) {
		work = get_list_work(item);
		ops = &work->ops;
		if (i < ecount)
			ewrite[i] = ops->block;
		else if (ops->block != ewrite[i % ecount])
			break;
		ops->callback(ops->info, ops->data_buffer, ops->oob_buffer,
		              ops->block, ops->page, -ENANDWRITE,
		              ops->userdata);
		list_ctrl->total_num--;
		item = item->next;
		mem_free(work);
		i++;
	}
	list_ctrl->head.next = item;
	return i;
}

static bool is_ewrite_block(struct mtk_nand_chip_info *info,
                            struct worklist_ctrl *list_ctrl, u32 block)
{
	int i;

	for (i = 0; i < info->plane_num; i++)
		if (block == list_ctrl->ewrite[i])
			return true;

	return false;
}

static struct list_node *free_multi_work(struct worklist_ctrl *list_ctrl,
        struct list_node *head,
        struct list_node *start_node,
        int count)
{
	int i;
	struct list_node *item, *prev, *next;
	struct nand_work *work;

	item = start_node;

	prev = get_list_item_prev(head, start_node);
	if (prev == NULL) {
		pr_err("prev is NULL!!\n");
		return NULL;
	}
	for (i = 0; i < count; i++) {
		if (item == NULL) {
			pr_err("%s: NULL item\n", __func__);
			return NULL;
		}
		work = get_list_work(item);
		next = item->next;
		list_ctrl->total_num--;
		mem_free(work);
		item = next;
	}
	prev->next = item;
	return item;
}

static struct list_node *free_multi_write_work(struct worklist_ctrl
        *list_ctrl,
        struct list_node *start_node,
        int count)
{
	struct list_node *head, *next;

	head = &list_ctrl->head;
	next = free_multi_work(list_ctrl, head, start_node, count);
	return next;
}

static struct list_node *free_multi_erase_work(struct worklist_ctrl
        *list_ctrl,
        struct list_node *start_node,
        int count)
{
	struct list_node *head, *next;

	head = &list_ctrl->head;
	next = free_multi_work(list_ctrl, head, start_node, count);
	return next;
}

static struct list_node *free_tlc_page_group_work(struct worklist_ctrl
        *list_ctrl,
        struct list_node *base_node,
        int start, int end,
        bool multi_plane)
{
	struct list_node *start_node;
	int i;
	int count = multi_plane ? 2 : 1;

	start_node = seek_list_item(base_node, start * count);
	for (i = 0; i < end - start + 1; i++) {
		start_node =
		    free_multi_write_work(list_ctrl, start_node, count);
	}
	return start_node;
}

static u32 do_mlc_multi_plane_write(struct mtk_nand_chip_info *info,
                                    struct worklist_ctrl *list_ctrl,
                                    struct list_node *start_node, int count)
{
	u32 ret = 0, mode = 0;
	int i, status = 0, odd = 0;
	struct list_node *item;
	struct nand_work *work;
	struct mtk_nand_chip_operation *ops;
	u32 page = 0, block0 = 0, block1 = 0, block_num;
	bool multi_plane = false;

	nandx_get_device(FL_WRITING);

	if (count != info->max_keep_pages && count != info->plane_num) {
		pr_err("%s: count:%d max:%d(or %d)\n",
		       __func__, count, info->max_keep_pages,
		       info->plane_num);
		goto err;
	}

	if (start_node == NULL) {
		pr_err("%s: start node is NULL!\n", __func__);
		goto err;
	}

	item = start_node;
	ret = count;

	if (should_close_block()) {
		work = get_list_work(item);
		page = work->ops.page;
		block0 = work->ops.block;
		if (is_multi_plane(info))
			block1 = get_list_work(item->next)->ops.block;
	}

	multi_plane = is_multi_plane(info);
	for (i = 0; i < count; i++) {
		work = get_list_work(item);
		ops = &work->ops;
		item = item->next;
		block_num = ops->block + data_info->bmt.start_block;
		if (i == 0)
			mode |= is_slc_block(info, ops->block) ? MODE_SLC : 0;

		if (multi_plane) {
			if (!(i & 1))
				odd = block_num & 1;
			else if ((block_num & 1) == odd)
				multi_plane = false;
		}

		ops_table[i].row =
		    block_num * info->data_page_num + ops->page;
		ops_table[i].col = 0;
		ops_table[i].len = info->data_page_size;
		ops_table[i].data = ops->data_buffer;
		ops_table[i].oob = ops->oob_buffer;
	}

	if (multi_plane)
		mode |= MODE_MULTI;

	status = nandx_core_write(ops_table, count, mode);

	item = start_node;

	if (status == -ENANDWRITE) {
		ret = callback_and_free_ewrite(list_ctrl, count);
	} else {
		call_multi_work_callback(item, count, status);
		free_multi_write_work(list_ctrl, item, count);
	}

	if (should_close_block() &&
	        (status || (page == (info->data_page_num - 1)))) {
		open_block_delete(&data_info->open, block0);
		if (is_multi_plane(info))
			open_block_delete(&data_info->open, block1);
	}

err:
	nandx_release_device();
	return ret;
}

static u32 do_tlc_write(struct mtk_nand_chip_info *info,
                        struct worklist_ctrl *list_ctrl,
                        struct list_node *start_node, int count)
{
	u32 ret = 0, mode = 0;
	int i, status = 0, odd = 0;
	struct list_node *item;
	u32 block0, block1 = 0, block_num;
	struct nand_work *work;
	struct mtk_nand_chip_operation *ops;
	bool multi_plane = false, last_page = false;

	nandx_get_device(FL_WRITING);

	if (count != info->max_keep_pages && count != info->plane_num) {
		pr_err("%s: count:%d max:%d(or %d)\n",
		       __func__, count, info->max_keep_pages,
		       info->plane_num);
		goto err;
	}

	if (start_node == NULL) {
		pr_err("%s: start node is NULL!\n", __func__);
		goto err;
	}

	work = get_list_work(start_node);
	ops = &work->ops;
	mode |= is_slc_block(info, ops->block) ? MODE_SLC : 0;

	ret = count;
	item = start_node;
	multi_plane = is_multi_plane(info);

	for (i = 0; i < count; i++) {
		work = get_list_work(item);
		ops = &work->ops;
		item = item->next;
		block_num = ops->block + data_info->bmt.start_block;
		if (multi_plane) {
			if (!(i & 1))
				odd = block_num & 1;
			else if ((block_num & 1) == odd)
				multi_plane = false;
		}

		ops_table[i].row =
		    block_num * info->data_page_num + ops->page;
		ops_table[i].col = 0;
		ops_table[i].len = info->data_page_size;
		ops_table[i].data = ops->data_buffer;
		ops_table[i].oob = ops->oob_buffer;
	}

	if (multi_plane)
		mode |= MODE_MULTI;

	status = nandx_core_write(ops_table, count, mode);

	item = start_node;

	if (status == -ENANDWRITE) {
		ret =
		    callback_and_free_ewrite(list_ctrl, multi_plane ? 2 : 1);
	} else if (mode & MODE_SLC) {
		call_multi_work_callback(item, count, status);
		free_multi_write_work(list_ctrl, item, count);
	} else {
		call_tlc_page_group_callback(item, 0, 2, multi_plane, status);

		if (multi_plane)
			work = get_list_work(seek_list_item(item, 8 * 2));
		else
			work = get_list_work(seek_list_item(item, 8));

		item = free_tlc_page_group_work(list_ctrl, item, 0, 2,
		                                multi_plane);
		ops = &work->ops;

		if (ops->page == (info->data_page_num - 1)) {
			/* 3~5 */
			call_tlc_page_group_callback(item, 0, 2,
			                             multi_plane, status);
			/* 6~8 */
			call_tlc_page_group_callback(item, 3, 5,
			                             multi_plane, status);
			/* 3~5 */
			item = free_tlc_page_group_work(list_ctrl, item,
			                                0, 2, multi_plane);
			/* 6~8 */
			item = free_tlc_page_group_work(list_ctrl, item,
			                                0, 2, multi_plane);
			last_page = true;
		}
	}

	if (should_close_block() && (status || last_page)) {
		open_block_delete(&data_info->open, block0);
		if (multi_plane)
			open_block_delete(&data_info->open, block1);
	}

err:
	nandx_release_device();
	return ret;
}

static u32 complete_erase_count(struct mtk_nand_chip_info *info,
                                struct worklist_ctrl *list_ctrl, int total)
{
	struct list_node *head;
	struct nand_work *work0, *work1;
	bool is2p;

	if (!total)
		return 0;

	if (!is_multi_plane(info))
		return 1;

	/* multi-plane */
	head = &list_ctrl->head;
	if (total < info->plane_num) {
		work0 = get_list_work(seek_list_item(head, total));
		return (!work0->ops.more) ? total : 0;
	}

	/* total >= plane_num */
	work0 = get_list_work(head->next);
	if (!head->next->next)
		pr_err("%s: second NULL, total:%d, work0->block:%d\n",
		       __func__, total, work0->ops.block);
	work1 = get_list_work(head->next->next);
	is2p = are_on_diff_planes(work0->ops.block, work1->ops.block);
	return is2p ? info->plane_num : 1;
}

static u32 complete_slc_write_count(struct mtk_nand_chip_info *info,
                                    struct worklist_ctrl *list_ctrl,
                                    int total)
{
	struct list_node *head;
	struct nand_work *work0, *work1;
	bool multi_op;

	if (!total)
		return 0;

	if (!is_multi_plane(info))
		return 1;

	/* multi-plane */
	if (total < info->plane_num)
		return 0;

	/* total >= plane_num */
	head = &list_ctrl->head;
	work0 = get_list_work(head->next);
	work1 = get_list_work(head->next->next);
	multi_op = can_ops_multi_plane(&work0->ops, &work1->ops);
	return multi_op ? 2 : 1;
}

static u32 complete_write_count(struct mtk_nand_chip_info *info,
                                struct worklist_ctrl *list_ctrl, int total)
{
	struct list_node *head;
	struct nand_work *work0, *work1;
	bool multi_op;

	if (!total)
		return 0;

	head = &list_ctrl->head;
	work0 = get_list_work(head->next);
	if (!is_slc_block(info, work0->ops.block))
		goto non_slc;

	if (!is_multi_plane(info))
		return 1;

	if (total < info->plane_num)
		return 0;

	work1 = get_list_work(head->next->next);
	if (!is_slc_block(info, work1->ops.block)) {
		pr_err("%s: block0 %d is slc, but block1 %d is not\n",
		       __func__, work0->ops.block, work1->ops.block);
		return 0;
	}
	multi_op = can_ops_multi_plane(&work0->ops, &work1->ops);
	return multi_op ? 2 : 1;

non_slc:
	return (total >= info->max_keep_pages) ? info->max_keep_pages : 0;
}

static int init_list_ctrl(struct mtk_nand_chip_info *info,
                          struct worklist_ctrl *list_ctrl,
                          enum worklist_type type,
                          get_ready_count get_ready_count_func,
                          process_list_data process_data_func)
{
	int i;

	list_ctrl->type = type;
	list_ctrl->total_num = 0;
	list_ctrl->head.next = NULL;
	list_ctrl->get_ready_count_func = get_ready_count_func;
	list_ctrl->process_data_func = process_data_func;

	list_ctrl->ewrite = mem_alloc(info->plane_num, sizeof(int));
	for (i = 0; i < info->plane_num; i++)
		list_ctrl->ewrite[i] = -1;

	return 0;
}

static inline bool block_num_is_valid(struct mtk_nand_chip_info *info,
                                      u32 block)
{
	return (block >= 0 && block <
	        (info->data_block_num + info->log_block_num));
}

static inline bool block_page_num_is_valid(struct mtk_nand_chip_info *info,
        u32 block, u32 page)
{
	if (!block_num_is_valid(info, block))
		return false;

	if (page < 0)
		return false;

	if ((!is_slc_block(info, block) &&
	        page < info->data_page_num) ||
	        (is_slc_block(info, block) && page < info->log_page_num))
		return true;
	else
		return false;
}

static int open_block_init(struct open_block *open)
{
	struct mtk_nand_chip_info *info = &data_info->chip_info;
	int i;

	open->max = info->plane_num;
	open->array = mem_alloc(open->max, sizeof(int));
	if (open->array == NULL)
		return -1;

	for (i = 0; i < open->max; i++)
		open->array[i] = -1;

	return 0;
}

static int open_block_count(const struct open_block *open)
{
	int i, count = 0;

	for (i = 0; i < open->max; i++) {
		if (open->array[i] != -1)
			count++;
	}

	return count;
}

static int open_block_delete(struct open_block *open, int block)
{
	int i, count;
	int ret;

	count = open_block_count(open);

	for (i = 0; i < open->max; i++) {
		if (open->array[i] == block) {
			open->array[i] = -1;
			if (count == 1) {
				pr_debug("pm_relax\n");
			}
			ret = 0;
			break;
		}
	}

	if (i == open->max)
		ret = -1;

	if (ret)
		pr_err("fail!!!\n");

	return ret;
}

static bool should_close_block(void)
{
	return false;
}

static u32 mtk_nand_do_erase(struct mtk_nand_chip_info *info,
                             struct worklist_ctrl *list_ctrl, int count)
{
	struct list_node *item;
	int i, status, op_cnt, ret = 0;
	int plane_num, odd = 0;
	u32 *rows;
	struct nand_work *work;
	struct mtk_nand_chip_operation *ops;
	u32 block_num = 0, mode = 0;
	bool multi_plane = false;

	nandx_get_device(FL_ERASING);

	i = 0;
	item = list_ctrl->head.next;

	if (!count)
		goto err;

	if (count != 1 && count % info->plane_num) {
		pr_err("count:%d\n", count);
		goto err;
	}

	if (item == NULL) {
		pr_err("head next null, count:%d\n", count);
		goto err;
	}

	rows = mem_alloc(1, sizeof(u32) * count);
	if (!rows) {
		goto err;
	}

	ret = count;
	multi_plane = is_multi_plane(info);
	while ((i < count) && item) {
		work = get_list_work(item);
		ops = &work->ops;
		block_num = ops->block + data_info->bmt.start_block;
		rows[i] = block_num * info->data_page_num + ops->page;
		pr_debug("%s: ops_block %d, page %d, row %d\n",
		         __func__, ops->block, ops->page, rows[i]);
		item = item->next;
		if (multi_plane) {
			if (!(i & 1))
				odd = ops->block & 1;
			else if ((ops->block & 1) == odd)
				multi_plane = false;
		}
		i++;
	}

	/* check whether is slc block */
	block_num -= data_info->bmt.start_block;
	if (is_slc_block(info, block_num))
		mode |= MODE_SLC;
	/* check whether support multi-plane */
	if (i & 1)
		multi_plane = false;
	else if (multi_plane)
		mode |= MODE_MULTI;

	status = nandx_core_erase(rows, count, mode);

	i = 0;
	item = list_ctrl->head.next;
	plane_num = multi_plane ? info->plane_num : 1;
	while ((i < count) && item) {
		op_cnt = ((count - i) > plane_num) ? plane_num :
		         (count - i);
		pr_debug(" i:%d, op_cnt:%d\n", i, op_cnt);
		call_multi_work_callback(item, op_cnt, status);
		item = free_multi_erase_work(list_ctrl, item, op_cnt);
		i += op_cnt;
	}

	mem_free(rows);
	pr_debug("mtk_nand_do_erase done\n");

err:
	nandx_release_device();
	return ret;
}

static u32 mtk_nand_do_slc_write(struct mtk_nand_chip_info *info,
                                 struct worklist_ctrl *list_ctrl, int count)
{
	int i, status, odd = 0, ret = 0;
	struct nand_work *work;
	struct list_node *item;
	struct mtk_nand_chip_operation *ops;
	u32 block_num, mode = 0;
	bool multi_plane = false;

	nandx_get_device(FL_WRITING);

	if (!count)
		goto err;

	if (count != 1 && count != 2) {
		pr_err("count not 1 or 2, count:%d\n", count);
		goto err;
	}

	item = list_ctrl->head.next;
	ret = count;

	multi_plane = is_multi_plane(info);
	if (multi_plane && (count % info->plane_num) > 0)
		multi_plane = false;

	for (i = 0; i < count; i++) {
		work = get_list_work(item);
		ops = &work->ops;
		item = item->next;
		block_num = ops->block + data_info->bmt.start_block;
		if (multi_plane) {
			if (!(i & 1))
				odd = block_num & 1;
			else if ((block_num & 1) == odd)
				multi_plane = false;
		}

		ops_table[i].row =
		    block_num * info->data_page_num + ops->page;
		ops_table[i].col = 0;
		ops_table[i].len = info->log_page_size;
		ops_table[i].data = ops->data_buffer;
		ops_table[i].oob = ops->oob_buffer;
	}

	mode |= MODE_SLC;
	if (multi_plane)
		mode |= MODE_MULTI;

	status = nandx_core_write(ops_table, count, mode);

	if (status == -ENANDWRITE) {
		ret = callback_and_free_ewrite(list_ctrl, count);
		goto err;
	}

	call_multi_work_callback(list_ctrl->head.next, count, status);
	free_multi_write_work(list_ctrl, list_ctrl->head.next, count);

err:
	nandx_release_device();
	return ret;
}

static u32 mtk_nand_do_write(struct mtk_nand_chip_info *info,
                             struct worklist_ctrl *list_ctrl, int count)
{
	struct list_node *node;

	node = list_ctrl->head.next;
	if (info->types == MTK_NAND_FLASH_MLC)
		return do_mlc_multi_plane_write(info, list_ctrl, node, count);

	if (info->types == MTK_NAND_FLASH_TLC)
		return do_tlc_write(info, list_ctrl, node, count);

	pr_err("unhandled work!\n");
	return 0;
}

static int mtk_nand_process_list(struct mtk_nand_chip_info *info,
                                 struct worklist_ctrl *list_ctrl,
                                 int sync_num)
{
	u32 total, ready_cnt;
	u32 process_cnt = 0, left_cnt;

	total = get_list_work_cnt(list_ctrl);

	if (((sync_num > 0) && (total < sync_num)) || (total == 0))
		goto out;

	left_cnt = total - process_cnt;
	do {
		ready_cnt = list_ctrl->get_ready_count_func(info, list_ctrl,
		            left_cnt);
		if (!ready_cnt)
			break;

		process_cnt += list_ctrl->process_data_func(info, list_ctrl,
		               ready_cnt);
		left_cnt = total - process_cnt;
	} while (left_cnt);

	if ((sync_num == -1) && left_cnt) {
		/* only force sync erase and slc write work */
		if (list_ctrl->type == LIST_NS_WRITE)
			goto out;

		left_cnt = total - process_cnt;
		if (left_cnt > 1)
			pr_err("left_cnt:%d\n", left_cnt);
		list_ctrl->process_data_func(info, list_ctrl, 1);
		process_cnt += 1;
	}

out:
	return process_cnt;
}

static void mtk_nand_do_work(void)
{
	struct mtk_nand_chip_info *info = &data_info->chip_info;
	struct worklist_ctrl *elist_ctrl = &data_info->elist_ctrl;
	struct worklist_ctrl *swlist_ctrl = &data_info->swlist_ctrl;
	struct worklist_ctrl *wlist_ctrl = &data_info->wlist_ctrl;

	mtk_nand_process_list(info, elist_ctrl, 0);
	mtk_nand_process_list(info, swlist_ctrl, 0);
	mtk_nand_process_list(info, wlist_ctrl, 0);
}

static void mtk_nand_dump_bbt_info(struct mtk_nand_chip_bbt_info *chip_bbt)
{
	u32 i;

	pr_info("%s: bad_block_num:%d, initial_bad_num:%d\n",
	        __func__, chip_bbt->bad_block_num, chip_bbt->initial_bad_num);
	for (i = 0; i < chip_bbt->bad_block_num; i++)
		pr_info("bad_index:%d\n", chip_bbt->bad_block_table[i]);
}

int mtk_chip_bbt_init(struct data_bmt_struct *data_bmt)
{
	struct mtk_nand_chip_bbt_info *chip_bbt = &data_info->chip_bbt;
	u32 i, initial_bad_num;
	u32 initial_bad, ftl_mark_bad;
	u16 bad_block;

	chip_bbt->bad_block_num = data_bmt->bad_count;
	chip_bbt->initial_bad_num = data_bmt->bad_count;
	if (data_bmt->bad_count > BAD_BLOCK_MAX_NUM) {
		pr_err("bad block count > max(%d)\n", BAD_BLOCK_MAX_NUM);
		return -1;
	}

	initial_bad_num = 0;
	for (i = 0; i < data_bmt->bad_count; i++)
		if (data_bmt->entry[i].flag != FTL_MARK_BAD)
			initial_bad_num++;

	chip_bbt->initial_bad_num = initial_bad_num;
	initial_bad = 0;
	ftl_mark_bad = initial_bad_num;

	for (i = 0; i < data_bmt->bad_count; i++) {
		bad_block = data_bmt->entry[i].bad_index -
		            data_bmt->start_block;
		if (data_bmt->entry[i].flag != FTL_MARK_BAD) {
			chip_bbt->bad_block_table[initial_bad] = bad_block;
			initial_bad++;
		} else {
			chip_bbt->bad_block_table[ftl_mark_bad] = bad_block;
			ftl_mark_bad++;
		}
	}

	for (i = data_bmt->bad_count; i < BAD_BLOCK_MAX_NUM; i++)
		chip_bbt->bad_block_table[i] = 0xffff;

	mtk_nand_dump_bbt_info(chip_bbt);

	return 0;
}

static int mtk_chip_info_init(struct mtk_nand_chip_info *chip_info)
{
	struct nandx_chip_info *info;
	u32 page_per_block;

	info = get_chip_info();

	page_per_block = info->block_size / info->page_size;

	chip_info->data_block_num = data_info->partition_info.total_block *
	                            (100 - data_info->partition_info.slc_ratio) / 100;
	chip_info->log_block_num = data_info->partition_info.total_block -
	                           chip_info->data_block_num;

	chip_info->data_page_num = page_per_block;
	chip_info->data_page_size = info->page_size;
	chip_info->data_block_size = info->block_size;
	chip_info->log_page_size = info->page_size;

	chip_info->log_page_num = page_per_block / info->wl_page_num;
	chip_info->log_block_size = info->block_size / info->wl_page_num;

	if (info->plane_num > 1) {
		chip_info->data_block_num >>= 1;
		chip_info->data_block_num <<= 1;
		chip_info->log_block_num >>= 1;
		chip_info->log_block_num <<= 1;
	}

	chip_info->data_oob_size = info->oob_size;
	chip_info->log_oob_size = chip_info->data_oob_size;

	chip_info->slc_ratio = data_info->partition_info.slc_ratio;
	chip_info->start_block = data_info->partition_info.start_block;

	chip_info->sector_size_shift = 10;

	if (info->wl_page_num == 3)
		chip_info->max_keep_pages = (info->plane_num > 1) ? 18 : 9;
	else if (info->wl_page_num == 2)
		chip_info->max_keep_pages = (info->plane_num > 1) ? 4 : 2;
	else
		chip_info->max_keep_pages = (info->plane_num > 1) ? 2 : 1;

	if (info->wl_page_num == 3)
		chip_info->types = MTK_NAND_FLASH_TLC;
	else if (info->wl_page_num == 2)
		chip_info->types = MTK_NAND_FLASH_MLC;
	else
		chip_info->types = MTK_NAND_FLASH_SLC;

	chip_info->plane_mask = 0x1;
	chip_info->plane_num = (info->plane_num > 1) ? 2 : 1;
	chip_info->chip_num = 1;

	if (info->plane_num > 1)
		chip_info->option = MTK_NAND_PLANE_MODE_SUPPORT |
		                    MTK_NAND_MULTI_READ_DIFFERENT_OFFSET;

	/* Todo: export PE cycle from device info!! */
	chip_info->data_pe = 2000;
	chip_info->log_pe = 25000;
	chip_info->info_version = MTK_NAND_CHIP_INFO_VERSION;

	return 0;
}

static void mtk_nand_dump_chip_info(struct mtk_nand_chip_info *chip_info)
{
	pr_debug("mtk_nand_chip_info dump info here\n");
	pr_debug("data_block_num: %d\n", chip_info->data_block_num);
	pr_debug("data_page_num: %d\n", chip_info->data_page_num);
	pr_debug("data_page_size: %d\n", chip_info->data_page_size);
	pr_debug("data_oob_size: %d\n", chip_info->data_oob_size);
	pr_debug("data_block_size: 0x%x\n", chip_info->data_block_size);
	pr_debug("log_block_num: %d\n", chip_info->log_block_num);
	pr_debug("log_page_num: %d\n", chip_info->log_page_num);
	pr_debug("log_page_size: %d\n", chip_info->log_page_size);
	pr_debug("log_block_size: 0x%x\n", chip_info->log_block_size);
	pr_debug("log_oob_size: %d\n", chip_info->log_oob_size);
	pr_debug("slc_ratio: %d\n", chip_info->slc_ratio);
	pr_debug("start_block: %d\n", chip_info->start_block);
	pr_debug("sector_size_shift: %d\n", chip_info->sector_size_shift);
	pr_debug("max_keep_pages: %d\n", chip_info->max_keep_pages);
	pr_debug("types: 0x%x\n", chip_info->types);
	pr_debug("plane_mask: 0x%x\n", chip_info->plane_mask);
	pr_debug("plane_num: %d\n", chip_info->plane_num);
	pr_debug("chip_num: %d\n", chip_info->chip_num);
	pr_debug("option: 0x%x\n", chip_info->option);
}

static void mtk_nand_dump_partition_info(struct nand_ftl_partition_info
        *partition_info)
{
	pr_debug("dump partition info here\n");
	pr_debug("start_block: %d\n", partition_info->start_block);
	pr_debug("total_block: %d\n", partition_info->total_block);
	pr_debug("slc_ratio: %d\n", partition_info->slc_ratio);
	pr_debug("slc_block: %d\n", partition_info->slc_block);
}

static void mtk_nand_dump_bmt_info(struct data_bmt_struct *data_bmt)
{
	u32 i;

	pr_debug("dump bmt info here\n");
	pr_debug("bad_count: %d\n", data_bmt->bad_count);
	pr_debug("start_block: %d\n", data_bmt->start_block);
	pr_debug("end_block: %d\n", data_bmt->end_block);
	pr_debug("version: %d\n", data_bmt->version);

	for (i = 0; i < data_bmt->bad_count; i++) {
		pr_debug("bad_index: %d, flag: %d\n",
		         data_bmt->entry[i].bad_index,
		         data_bmt->entry[i].flag);
	}
}

/* API for nand Wrapper */

/*
 * struct mtk_nand_chip_info *mtk_nand_chip_init(void)
 * Init mntl_chip_info to nand wrapper, after nand driver init.
 * Return: On success, return mtk_nand_chip_info. On error, return error num.
 */
struct mtk_nand_chip_info *mtk_nand_chip_init(void)
{

	if (&data_info->chip_info != NULL)
		return &data_info->chip_info;
	else
		return 0;
}

/*
 * mtk_nand_chip_read_page
 * Only one page data and FDM data read, support partial read.
 *
 * @info: NAND handler
 * @data_buffer/oob_buffer: Null for no need data/oob
 *                                             must contiguous address space.
 * @block/page: The block/page to read data.
 * @offset: data offset to start read, and must be aligned to sector size.
 * @size: data size to read. size <= pagesize
 *             less than page size will partial read, and OOB is only related
 *             sectors, uncompleted refer to whole page.
 * return : 0 on success, On error, return error num.
 */
int mtk_nand_chip_read_page(struct mtk_nand_chip_info *info,
                            u8 *data_buffer, u8 *oob_buffer, u32 block,
                            u32 page, u32 offset, u32 size)
{
	int ret = 0;

	if (data_buffer == NULL) {
		pr_err("data_buffer is null\n");
		return -EINVAL;
	}

	if (oob_buffer == NULL) {
		pr_err("oob_buffer is null\n");
		return -EINVAL;
	}

	if ((offset % (1 << info->sector_size_shift) != 0)
	        || (size % (1 << info->sector_size_shift) != 0)) {
		pr_err("offset or size is invalid:offset:%d, size:%d\n",
		       offset, size);
		return -EINVAL;
	}

	if (!block_page_num_is_valid(info, block, page)) {
		pr_err("%s: block or page is invalid:block:%d, page:%d\n",
		       __func__, block, page);
		return -EINVAL;
	}

	if (mtk_isbad_block(block))
		return -ENANDBAD;

	ret = mtk_nand_read_pages(info, data_buffer,
	                          oob_buffer, block, page, offset, size);

	if (ret) {
		pr_err("read err:%d block:%d page:%d offset:%d size:%d\n",
		       ret, block, page, offset, size);
	}

	return ret;
}

/*
 * mtk_nand_chip_read_pages
 * Read multiple pages of data/FDM at once. Support partial read.
 *
 * The driver can choose the number of pages it actually read.
 * Driver only guarantee to read at least one page. Caller must handle
 * unread pages by itself.
 *
 * If there are any error in 2nd or following pages, just return numbers
 * of page read without any error. Driver shouldn't retry/re-read other pages.
 *
 * @info: NAND handler
 * @page_num: the page numbers to read.
 * @param: parameters for each page read
 * return : >0 number of pages read without any error (including ENANDFLIPS)
 *          On first page read error, return error number.
 */
int mtk_nand_chip_read_multi_pages(struct mtk_nand_chip_info *info,
                                   int page_num,
                                   struct mtk_nand_chip_read_param *param)
{
	struct mtk_nand_chip_read_param *p = param;
	int ret;

	if (!page_num) {
		pr_err("page_num is 0, so return 0\n");
		return 0;
	}

	if (page_num == 1 || !is_multi_read_support(info)) {
		ret = mtk_nand_chip_read_page(info, p->data_buffer,
		                              p->oob_buffer, p->block,
		                              p->page, p->offset, p->size);
		return ret ? ret : 1;
	}

	if (!can_multi_plane(p->block, p->page, p[1].block, p[1].page)) {
		ret = mtk_nand_chip_read_page(info, p->data_buffer,
		                              p->oob_buffer, p->block,
		                              p->page, p->offset, p->size);
		return ret ? ret : 1;
	}

	return mtk_nand_read_multi_pages(info, 2, param);
}

/*
 * mtk_nand_chip_write_page
 * write page API. Only one page data write, async mode.
 * Just return 0 and add to write worklist as below:
 *  a) For TLC WL write, NAND handler call nand driver WL write function.
 *  b) For Multi-plane program, if more_page is TRUE,
 *  wait for the next pages write and do multi-plane write on time.
 *  c) For cache  program, driver will depend on more_page flag for TLC program,
 *  can not used for SLC/MLC program.
 * after Nand driver erase/write operation, callback function will be done.
 *
 * @info: NAND handler
 * @data_buffer/oob_buffer: must contiguous address space.
 * @block/page: The block/page to write data.
 * @more_page: for TLC WL write and multi-plane program operation.
 *                        if more_page is true, driver will wait complete
 *                        operation and call driver function.
 * @*callback: callback for wrapper, called after driver finish the operation.
 * @*userdata : for callback function
 * return : 0 on success, On error, return error num casted by ERR_PTR
 */
int mtk_nand_chip_write_page(struct mtk_nand_chip_info *info,
                             u8 *data_buffer, u8 *oob_buffer,
                             u32 block, u32 page, bool more_page,
                             mtk_nand_callback_func callback, void *userdata)
{
	/* Add to worklist here */
	struct worklist_ctrl *list_ctrl;
	struct nand_work *work;
	struct mtk_nand_chip_operation *ops;
	int total_num;
	u32 max_keep_pages;
	int page_num;
	int ret;

	if (data_buffer == NULL) {
		pr_err("data_buffer is null\n");
		return -EINVAL;
	}

	if (oob_buffer == NULL) {
		pr_err("oob_buffer is null\n");
		return -EINVAL;
	}

	if (!block_page_num_is_valid(info, block, page)) {
		pr_err("%s: block or page is invalid:block:%d, page:%d\n",
		       __func__, block, page);
		return -EINVAL;
	}

	list_ctrl = (block < info->data_block_num) ?
	            &data_info->wlist_ctrl : &data_info->swlist_ctrl;
	total_num = get_list_work_cnt(list_ctrl);
	max_keep_pages = is_slc_block(info, block) ?
	                 info->plane_num : info->max_keep_pages;

	while (total_num >= max_keep_pages) {
		mtk_nand_process_list(info, list_ctrl, max_keep_pages);
		total_num = get_list_work_cnt(list_ctrl);
	};

	work = mem_alloc(1, sizeof(struct nand_work));
	if (work == NULL)
		return -ENOMEM;

	work->list.next = NULL;
	ops = &work->ops;

	ops->info = info;
	ops->types = MTK_NAND_OP_WRITE;
	ops->data_buffer = data_buffer;
	ops->oob_buffer = oob_buffer;
	ops->block = block;
	ops->page = page;
	ops->more = more_page;
	ops->callback = callback;
	ops->userdata = userdata;

	ret = add_list_node(list_ctrl, &work->list);
	if (ret)
		return 0;

	page_num = is_slc_block(info, block) ? info->log_page_num :
	           info->data_page_num;
	total_num = get_list_work_cnt(list_ctrl);
	if (total_num >= max_keep_pages || (page == page_num - 1))
		mtk_nand_do_work();

	return 0;
}

/*
 * mtk_nand_chip_erase_block
 * Erase API for nand wrapper, async mode for erase, just return success,
 * put erase operation into write worklist.
 * After Nand driver erase/write operation, callback function will be done.
 * @block: The block to erase
 * @*callback: Callback for wrapper, called after driver finish the operation.
 * @* userdata: for callback function
 * return : 0 on success, On error, return error num casted by ERR_PTR
 */
int mtk_nand_chip_erase_block(struct mtk_nand_chip_info *info,
                              u32 block, u32 more_block,
                              mtk_nand_callback_func callback, void *userdata)
{
	struct worklist_ctrl *list_ctrl;
	struct nand_work *work;
	struct mtk_nand_chip_operation *ops;
	int total_num;

	pr_debug("erase block:%d more_page:%d\n", block, more_block);

	if (!block_num_is_valid(info, block)) {
		pr_err("block num is invalid:block:%d\n", block);
		return -EINVAL;
	}

	list_ctrl = &data_info->elist_ctrl;
	total_num = get_list_work_cnt(list_ctrl);

	while (total_num >= info->max_keep_pages) {
		mtk_nand_process_list(info, list_ctrl, info->max_keep_pages);
		list_ctrl = &data_info->elist_ctrl;
		total_num = get_list_work_cnt(list_ctrl);
	};

	work = mem_alloc(1, sizeof(struct nand_work));
	if (work == NULL)
		return -ENOMEM;

	work->list.next = NULL;
	ops = &work->ops;

	ops->info = info;
	ops->types = MTK_NAND_OP_ERASE;
	ops->block = block;
	ops->page = 0;
	ops->more = more_block;
	ops->data_buffer = NULL;
	ops->oob_buffer = NULL;
	ops->callback = callback;
	ops->userdata = userdata;

	list_ctrl = &data_info->elist_ctrl;

	add_list_node(list_ctrl, &work->list);
	total_num = get_list_work_cnt(list_ctrl);
	if (total_num >= info->plane_num)
		mtk_nand_do_work();

	return 0;
}

/*
 * mtk_nand_chip_sync
 * flush all async worklist to nand driver.
 * return : On success, return 0. On error, return error num
 */
int mtk_nand_chip_sync(struct mtk_nand_chip_info *info)
{
	struct worklist_ctrl *elist_ctrl, *swlist_ctrl, *wlist_ctrl;
	int ret = 0;

	elist_ctrl = &data_info->elist_ctrl;
	swlist_ctrl = &data_info->swlist_ctrl;
	wlist_ctrl = &data_info->wlist_ctrl;

	ret = mtk_nand_process_list(info, elist_ctrl, -1);
	ret |= mtk_nand_process_list(info, swlist_ctrl, -1);
	ret |= mtk_nand_process_list(info, wlist_ctrl, -1);

	return 0;
}

/*
void mtk_nand_chip_set_blk_thread(struct task_struct *thead)
{

}
*/

/*
 * mtk_nand_chip_bmt, bad block table maintained by driver,
 * and read only for wrapper
 * @info: NAND handler
 * Return FTL partition's bad block table for nand wrapper.
 */
const struct mtk_nand_chip_bbt_info *mtk_nand_chip_bmt(struct
        mtk_nand_chip_info
        *info)
{
	if (&data_info->chip_bbt != NULL)
		return &data_info->chip_bbt;

	return 0;
}

/*
 * mtk_chip_mark_bad_block
 * Mark specific block as bad block, update bad block list and bad block table.
 * @block: block address to markbad
 */
void mtk_chip_mark_bad_block(struct mtk_nand_chip_info *info, u32 block)
{
	struct mtk_nand_chip_bbt_info *chip_bbt = &data_info->chip_bbt;
	u32 i;
	bool success;

	pr_err("markbad block:%d\n", block);
	for (i = 0; i < chip_bbt->bad_block_num; i++) {
		if (block == chip_bbt->bad_block_table[i])
			return;
	}
	chip_bbt->bad_block_table[chip_bbt->bad_block_num++] = block;

	success = nandx_bmt_update(block + data_info->bmt.start_block,
	                           FTL_MARK_BAD);
	if (!success)
		pr_err("mark block(%d) as bad fail!!!\n", block);
}

int nandx_mntl_data_info_alloc(void)
{
	data_info = mem_alloc(1, sizeof(struct mtk_nand_data_info));
	if (data_info == NULL)
		return -ENOMEM;

	return 0;
}

void nandx_mntl_data_info_free(void)
{
	mem_free(data_info);
}

int nandx_mntl_ops_init(void)
{
	int ret = 0;
	struct mtk_nand_chip_info *info;

	info = &data_info->chip_info;

	ret = get_data_partition_info(&data_info->partition_info,
	                              &data_info->chip_info);
	if (ret) {
		pr_err("Get FTL partition info failed\n");
		goto err_out;
	}
	mtk_nand_dump_partition_info(&data_info->partition_info);
	dump_block_bit_map(data_info->chip_info.block_type_bitmap);

	ret = nandx_bmt_get_data_bmt(&data_info->bmt);
	if (ret) {
		pr_err("Get FTL bmt info failed\n");
		goto err_out;
	}
	mtk_nand_dump_bmt_info(&data_info->bmt);

	ret = mtk_chip_info_init(info);
	if (ret) {
		pr_err("Get chip info failed\n");
		goto err_out;
	}
	mtk_nand_dump_chip_info(info);

	ret = open_block_init(&data_info->open);
	if (ret)
		goto err_out;

	ret = mtk_chip_bbt_init(&data_info->bmt);
	if (ret) {
		pr_err("Get chip bbt info failed\n");
		goto err_out;
	}

	ops_table = alloc_ops_table(info->max_keep_pages);
	if (!ops_table) {
		pr_err("error:alloc ops table failed!\n");
		goto err_out;
	}

	init_list_ctrl(info, &data_info->elist_ctrl, LIST_ERASE,
	               complete_erase_count, mtk_nand_do_erase);
	init_list_ctrl(info, &data_info->swlist_ctrl, LIST_SLC_WRITE,
	               complete_slc_write_count, mtk_nand_do_slc_write);
	init_list_ctrl(info, &data_info->wlist_ctrl, LIST_NS_WRITE,
	               complete_write_count, mtk_nand_do_write);

	return 0;

err_out:
	return ret;
}

void dump_block_bit_map(u8 *array)
{
	int i, j;
	int num;
	bool check;

	pr_info("dump_block_bit_map\n");
	for (i = 0; i < 500; i++) {
		num = array[i];
		if (num == 0)
			continue;

		for (j = 0; j < 8; j++) {
			if ((num & (1 << j)) == 0)
				continue;

			check = is_slc_block(&data_info->chip_info,
			                     ((i * 8) + j));
			pr_info("block %d is tlc check = %d\n",
			        ((i * 8) + j), check);
		}
	}
}

int mtk_nand_update_block_type(int num, unsigned int *blk)
{
	struct mtk_nand_chip_info *info = &data_info->chip_info;
	u32 row;
	int i, ret;

	nandx_get_device(FL_ERASING);
	for (i = 0; i < num; i++) {
		pr_debug("%s: erase block %d\n", __func__, blk[i]);
		row = (blk[i] + data_info->bmt.start_block) *
		      info->data_page_num;
		ret = nandx_core_erase(&row, 1, MODE_SLC);
		if (ret)
			pr_err("erase block %d failed\n", blk[i]);
	}
	nandx_release_device();

	return mntl_update_part_tab(&data_info->chip_info, num, blk);
}
