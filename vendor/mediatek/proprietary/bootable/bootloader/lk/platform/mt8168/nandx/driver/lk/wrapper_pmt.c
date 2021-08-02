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
#include "nandx_pmt.h"
#include <mt_partition.h>
#include "nandx_ops.h"
#ifdef MNTL_SUPPORT
#include "mntl_ops.h"
#endif

void part_init_pmt(unsigned long totalblks, part_dev_t *dev)
{
	part_t *part = &partition_layout[0];
	struct pmt_handler *handler;
	struct nandx_chip_info *info;
	struct pt_resident *pt;
	u32 i, bmt_block;
	u64 offset, size;

	handler = nandx_get_pmt_handler();
	pt = handler->pmt;
	if (pt == NULL)
		return;

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
		part->nr_sects = size / BLK_SIZE;
		part->start_sect = offset / BLK_SIZE;
		part->flags = 0;
		if (pt->ext.type == REGION_LOW_PAGE)
			part->type = TYPE_LOW;
		else
			part->type = TYPE_FULL;

		pt++;
		part++;
	}
}

u64 part_get_startaddress(u64 byte_address, int *idx)
{
	struct pmt_handler *handler;
	struct pt_resident *pt, *pt_temp;
	u32 i;

	handler = nandx_get_pmt_handler();

	pt_temp = handler->pmt;
	pt = nandx_pmt_get_partition(byte_address);

	for (i = 0; i < handler->part_num; i++) {
		if (!strcmp(pt->name, pt_temp->name)) {
			*idx = i;
			break;
		}
	}

	return nandx_pmt_get_start_address(pt);
}

bool raw_partition(u32 index)
{
	struct pmt_handler *handler;
	struct pt_resident *pt;

	handler = nandx_get_pmt_handler();

	pt = (struct pt_resident *)(handler->pmt + index);

	return nandx_pmt_is_raw_partition(pt);
}

/* temp part for mt_part_get_partition */
part_t tempart;

part_t *mt_part_get_partition(char *name)
{
	struct pmt_handler *handler;
	struct nandx_chip_info *info;
	part_t *part = &partition_layout[0];
	char r_name[MAX_PARTITION_NAME_LEN];
	u32 slc_blk_size;

	handler = nandx_get_pmt_handler();
	info = handler->info;
	slc_blk_size = info->block_size / info->wl_page_num;

	strcpy(r_name, name);

	if (!strcmp(name, "para"))
		strcpy(r_name, "MISC");
	else if (!strcmp(name, "logo"))
		strcpy(r_name, "LOGO");
	else if (!strcmp(name, "seccfg"))
		strcpy(r_name, "SECCFG");
	else if (!strcmp(name, "secro"))
		strcpy(r_name, "SEC_RO");
	else if (!strcmp(name, "boot"))
		strcpy(r_name, "BOOTIMG");
	else if (!strcmp(name, "preloader"))
		strcpy(r_name, "PRELOADER");
	else if (!strcmp(name, "recovery"))
		strcpy(r_name, "RECOVERY");
	else if (!strcmp(name, "mnb"))
		strcpy(r_name, "MNB");
	else if (!strcmp(name, "MNTL"))
		strcpy(r_name, "USRDATA");

	while (part->name) {
		if (!strcmp(r_name, (char *)part->name)) {
			memcpy(&tempart, part, sizeof(part_t));
			/* special case for security access preloader */
			if (!strcmp(r_name, "PRELOADER"))
				tempart.start_sect = BLK_NUM(slc_blk_size);
			return &tempart;
		}
		part++;
	}

	pr_err("%s: not found part [%s]\n", __func__, name);
	return NULL;
}

void mt_part_dump(void)
{
	part_t *part = &partition_layout[0];
	u32 sectors;

	pr_info("Part Info.\n");
	while (part->name) {
		sectors = part->start_sect + part->nr_sects;
		pr_info("[0x%.8lx-0x%.8x] (%.8ld blocks): \"%s\"\n",
		        part->start_sect * BLK_SIZE,
		        sectors * BLK_SIZE - 1, part->nr_sects, part->name);
		part++;
	}
}

int partition_get_index(const char *name)
{
	int index;

	for (index = 0; index < PART_MAX_COUNT; index++) {
		if ((!strcmp(name, g_part_name_map[index].fb_name)) ||
		        (!strcmp(name, g_part_name_map[index].r_name))) {
			return g_part_name_map[index].partition_idx;
		}
	}

	pr_err("get partition[%s] index failed\n", name);

	return -1;
}

unsigned int partition_get_region(int index)
{
	part_t *p = mt_part_get_partition(g_part_name_map[index].r_name);

	if (p == NULL)
		return -1;

	return p->part_id;
}

u64 partition_get_offset(int index)
{
	part_t *p = mt_part_get_partition(g_part_name_map[index].r_name);

	if (p == NULL)
		return -1;

	return (u64)p->start_sect * BLK_SIZE;
}

u64 partition_get_size(int index)
{
	part_t *p = mt_part_get_partition(g_part_name_map[index].r_name);

	if (p == NULL)
		return -1;

	return (u64)p->nr_sects * BLK_SIZE;
}

int partition_get_type(int index, char **p_type)
{
	*p_type = g_part_name_map[index].partition_type;

	return 0;
}

int partition_get_name(int index, char **p_name)
{
	*p_name = g_part_name_map[index].fb_name;

	return 0;
}

int is_support_erase(int index)
{
	return g_part_name_map[index].is_support_erase;
}

int is_support_flash(int index)
{
	return g_part_name_map[index].is_support_dl;
}

/* return > 0 if active bit exist, return = 0 if active bit is not existed*/
unsigned long mt_part_get_part_active_bit(part_t *part)
{
	return 0;
}

int partition_get_active_bit_by_name(const char *name)
{
	return 0;
}

u8 *partition_get_uuid_by_name(const char *name)
{
#ifdef MNTL_SUPPORT
	return mntl_gpt_uuid_by_name(name);
#else
	return NULL;
#endif
}

#ifdef MNTL_SUPPORT
int is_mntl_partition(char *part_name)
{
	int index = -1;

	index = partition_get_index(part_name);
	if (index == -1) {
		pr_err("partition get index fail\n");
		return 0;
	}
	return !(strncmp("ext4", g_part_name_map[index].partition_type, 4));
}

int get_data_partition_info(struct nand_ftl_partition_info *info,
                            struct mtk_nand_chip_info *cinfo)
{
	struct pmt_handler *handler;
	struct nandx_chip_info *dev;
	struct pt_resident *pt;
	u32 i;

	handler = nandx_get_pmt_handler();
	pt = handler->pmt;
	dev = handler->info;
	cinfo->block_type_bitmap = handler->block_bitmap;

	for (i = 0; i < handler->part_num; i++) {
		if (!strcmp(pt->name, FTL_PARTITION_NAME)) {
			info->total_block = (u32)div_down(pt->size,
			                                  dev->block_size);

			info->start_block = (u32)div_down(pt->offset,
			                                  dev->block_size);

			info->slc_ratio = 8 /*handler->usr_slc_ratio */;
			return 0;
		}
		pt++;
	}

	return -1;
}

static void update_block_bitmap(struct mtk_nand_chip_info *info, int num,
                                u32 *blk)
{
	struct pmt_handler *handler;
	int i, byte, bit;

	handler = nandx_get_pmt_handler();

	for (i = 0; i < num; i++) {
		byte = *blk / 32;
		bit = *blk % 32;
		info->block_type_bitmap[byte] |= (1 << bit);
		blk++;
	}
}

int mntl_update_part_tab(struct mtk_nand_chip_info *info, int num, u32 *blk)
{
	int ret;

	if (num <= 0 || blk == NULL)
		return -EINVAL;

	update_block_bitmap(info, num, blk);

	ret = nandx_pmt_update();

	return ret;
}

#endif
