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

#include <mt_partition.h>
#include "partition_define.h"

#include "nandx_util.h"
#include "nandx_errno.h"
#include "nandx_info.h"
#include "nandx_bmt.h"
#include "nandx_pmt.h"
#include "nandx_core.h"
#include "nandx_ops.h"
#include "nandx.h"
#ifdef MNTL_SUPPORT
#include "mntl.h"
#include "mntl_ops.h"

struct mntl_handler *mntl;
#endif
#define DL_NOT_FOUND 2
#define DL_PASS 0
#define DL_FAIL 1

/*download status v1 and old version for emmc*/
#define FORMAT_START    "FORMAT_START"
#define FORMAT_DONE      "FORMAT_DONE"
#define BL_START             "BL_START"
#define BL_DONE              "BL_DONE"
#define DL_START            "DL_START"
#define DL_DONE             "DL_DONE"
#define DL_ERROR           "DL_ERROR"
#define DL_CK_DONE       "DL_CK_DONE"
#define DL_CK_ERROR      "DL_CK_ERROR"

/* Add for Get DL information */
#define PRE_SCAN_BLOCK_NUM 20

#define MAX_LOAD_SECTIONS       40
#define DL_MAGIC "DOWNLOAD INFORMATION!!"
#define DL_INFO_VER_V1  "V1.0"

#define DL_MAGIC_NUM_COUNT 32
#define DL_MAGIC_OFFSET 24
#define DL_IMG_NAME_LENGTH 16
#define DL_CUSTOM_INFO_SIZE (128)

struct IMG_DL_INFO {
	char image_name[DL_IMG_NAME_LENGTH];
};

struct CHECKSUM_INFO_V1 {
	unsigned int image_index;
	unsigned int pc_checksum;
	unsigned int da_checksum;
	char checksum_status[8];
};

struct DL_STATUS_V1 {
	char magic_num[DL_MAGIC_OFFSET];
	char version[DL_MAGIC_NUM_COUNT - DL_MAGIC_OFFSET];
	struct CHECKSUM_INFO_V1 part_info[MAX_LOAD_SECTIONS];
	char ram_checksum[16];
	char download_status[16];
	struct IMG_DL_INFO img_dl_info[MAX_LOAD_SECTIONS];
};

struct nandx_core *ncore;

unsigned long g_nand_size;

static int nand_part_write(part_dev_t *dev, uchar *src, u64 dst, int size,
                           u32 part_id)
{
	int ret;

	ret = nandx_ops_write(ncore, dst, size, src, DO_SINGLE_PLANE_OPS);
	if (ret < 0)
		return ret;

	return (int)size;
}

static int nand_part_read(part_dev_t *dev, u64 source, uchar *dst, int size,
                          u32 part_id)
{
	int ret;

	ret = nandx_ops_read(ncore, source, size, dst, DO_SINGLE_PLANE_OPS);
	if (ret < 0)
		return ret;

	return (int)size;
}

bool randomizer_is_support(enum IC_VER ver)
{
	u32 reg = 0, enable = 0;

	if (ver == NANDX_MT8168) {
		/* randomizer efuse register address */
		reg = nreadl((void *)0x10009020);
		enable = 0x00001000;
	} else {
		NANDX_ASSERT(0);
	}

	return (reg & enable) ? true : false;
}

int nand_erase(u64 offset, u64 size)
{
	u64 limit;

	limit = ncore->info->block_num * ncore->info->block_size;
	return nandx_ops_erase(ncore, offset, limit, size);
}

int nand_img_read(u64 source, uchar *dst, int size)
{

	int ret;

	ret = nandx_ops_read(ncore, source, size, dst, DO_SINGLE_PLANE_OPS);
	if (ret < 0)
		return ret;

	return size;
}

int nand_write_img(u64 from, void *data, u32 size,
                   u32 partition_size, int img_type)
{
	bool ret, bIsRaw;
	u32 left_sz;
	u32 write_size, block_size;
	u64 addr, partition_end;
	struct pt_resident *pt;

	pr_info("[nand_wite_img]write to addr %llx,img size %x\n",
	        from, size);

	addr = from;
	partition_end = addr + partition_size;
	pt = nandx_pmt_get_partition(addr);
	bIsRaw = nandx_pmt_is_raw_partition(pt);
	if (!bIsRaw) {
		if (!nandx_pmt_blk_is_slc(addr))
			block_size = ncore->info->block_size;
		else
			block_size = ncore->info->slc_block_size;
	} else {
		block_size = ncore->info->slc_block_size;
	}

	if (addr % block_size || partition_size % block_size) {
		pr_err("%s: addr or size is not block size alignment\n",
		       __func__);
		return -1;
	}

	if (size > partition_size) {
		pr_err("%s: img size %x exceed partition size\n",
		       __func__, size);
		return -1;
	}

	left_sz = size;
	while (left_sz > 0) {
		if ((addr + left_sz) > partition_end) {
			pr_err("%s: to addr %llx, img size %x invalid\n",
			       __func__, addr, size);
			return -1;
		}

		if ((addr % block_size) == 0) {
			if (nandx_ops_erase_block(ncore, addr) < 0) {
				pr_warn("[ERASE] erase 0x%llx fail\n", addr);
				nandx_ops_mark_bad(addr / block_size,
				                   UPDATE_ERASE_FAIL);
				addr += block_size;
				/* erase fail, skip this block */
				continue;
			}
		}

		/* write by block */
		write_size = MIN(block_size, left_sz);
		ret = nandx_ops_write(ncore, addr, write_size, data,
		                      DO_SINGLE_PLANE_OPS);
		if (ret < 0) {
			pr_warn("%s: write fail at 0x%llx\n", __func__, addr);
			nandx_ops_erase_block(ncore, addr);
			nandx_ops_mark_bad(addr / block_size,
			                   UPDATE_WRITE_FAIL);
			addr += block_size;
			continue;
		}

		left_sz -= write_size;
		data += write_size;
		addr += write_size;
	}

	return 0;
}

int nand_write_img_ex(u64 addr, void *data,
                      u32 length, u64 total_size,
                      u32 *next_offset, u64 partition_start,
                      u64 partition_size, int img_type)
{
	return 0;
}

int nand_get_dl_info(void)
{
	int ret;
	struct DL_STATUS_V1 dl_info;
	u8 *data_buf, *oob_buf;
	u32 block_size, page_size, oob_size, total_blocks, pages_per_block;
	u32 i, block_i, page_i, block_addr, dl_info_blkAddr;
	u32 page_index[4];
	struct nandx_ops ops;

	block_size = ncore->info->block_size;
	page_size = ncore->info->page_size;
	oob_size = ncore->info->oob_size;
	pages_per_block = block_size / page_size;
	total_blocks = ncore->info->block_num;
	dl_info_blkAddr = 0xFFFFFFFF;

	data_buf = mem_alloc(1, page_size);
	oob_buf = mem_alloc(1, oob_size);
	if (!data_buf || !oob_buf) {
		ret = -1;
		return ret;
	}

	/*
	 * DL information block should program to good block instead of
	 * always at last block.
	 */
	page_index[0] = 0;
	page_index[1] = 1;
	page_index[2] = pages_per_block - 3;
	page_index[3] = pages_per_block - 1;

	block_i = 1;

	nandx_get_device(FL_READING);

	do {
		block_addr = pages_per_block * (total_blocks - block_i);
		for (page_i = 0; page_i < 4; page_i++) {
			ops.row = block_addr + page_index[page_i];
			ops.col = 0;
			ops.len = page_size;
			ops.data = data_buf;
			ops.oob = oob_buf;
			ret = nandx_core_read(&ops, 1, MODE_SLC);
			if (ret < 0) {
				nandx_release_device();
				goto err;
			}
			ret = memcmp((void *)data_buf, DL_MAGIC,
			             sizeof(DL_MAGIC));
			if (!ret) {
				dl_info_blkAddr = block_addr;
				break;
			}
		}

		if (dl_info_blkAddr != 0xFFFFFFFF)
			break;
		block_i++;
	} while (block_i <= PRE_SCAN_BLOCK_NUM);

	nandx_release_device();

	if (dl_info_blkAddr == 0xFFFFFFFF) {
		pr_info("DL INFO NOT FOUND\n");
		ret = DL_NOT_FOUND;
	} else {
		printf("get dl info from 0x%x\n", dl_info_blkAddr);
		memcpy(&dl_info, data_buf, sizeof(dl_info));

		if (!memcmp(dl_info.download_status, DL_DONE,
		            sizeof(DL_DONE)) ||
		        !memcmp(dl_info.download_status, DL_CK_DONE,
		                sizeof(DL_CK_DONE))) {
			pr_info("dl done. status = %s\n",
			        dl_info.download_status);
			pr_info("dram checksum : %s\n", dl_info.ram_checksum);

			for (i = 0; i < PART_MAX_COUNT; i++) {
				if (dl_info.part_info[i].image_index != 0) {
					pr_info
					("image_index:%d, checksum: %s\n",
					 dl_info.part_info[i].image_index,
					 dl_info.part_info[i].
					 checksum_status);
				}
			}
			ret = DL_PASS;
		} else {
			pr_info("dl error. status = %s\n",
			        dl_info.download_status);
			pr_info("dram checksum : %s\n", dl_info.ram_checksum);

			for (i = 0; i < PART_MAX_COUNT; i++) {
				if (dl_info.part_info[i].image_index != 0) {
					pr_info
					("image_index:%d, checksum: %s\n",
					 dl_info.part_info[i].image_index,
					 dl_info.part_info[i].
					 checksum_status);
				}
			}
			ret = DL_FAIL;
		}
	}

err:
	mem_free(data_buf);
	mem_free(oob_buf);
	return ret;
}

u32 mtk_nand_erasesize(void)
{
	return ncore->info->block_size / ncore->info->wl_page_num;
}

int nand_get_alignment(void)
{
	return ncore->info->block_size;
}

u32 mtk_nand_page_size(void)
{
	return ncore->info->page_size;
}

/*
 * empty function for build pass
 * to be deleted in future
 */
void nand_driver_test(void)
{
}

void lk_nand_irq_handler(unsigned int irq)
{
}

void nand_init(void)
{
	u32 mode = 0;
	static part_dev_t dev;
	static block_dev_desc_t blkdev;

	nandx_lock_init();

	mode = MODE_ECC | MODE_DMA;
	ncore = nandx_device_init(mode);
	if (!ncore)
		return;

	g_nand_size = nandx_get_chip_block_num(ncore->info);
	g_nand_size *= ncore->info->block_size;

	dev.id = 0;
	dev.init = 1;
	dev.read = nand_part_read;
	dev.write = nand_part_write;

	/*  for blkdev here */
	dev.blkdev = &blkdev;
	dev.blkdev->blksz = 512;
	dev.blkdev->blk_bits = 9;
	dev.blkdev->priv = ncore;

	mt_part_register_device(&dev);
#ifdef MNTL_SUPPORT
	nandx_mntl_data_info_alloc();
	nandx_mntl_ops_init();

	mntl = mntl_init();
	mntl_gpt_init(mntl);
#endif
}
