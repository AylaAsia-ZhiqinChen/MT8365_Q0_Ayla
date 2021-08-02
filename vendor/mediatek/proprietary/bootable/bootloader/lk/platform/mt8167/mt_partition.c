/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2015. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*/

#include <mt_partition.h>
#include <stdint.h>
#include <platform/errno.h>
#include "pmt.h"
#include <string.h>
#include <malloc.h>
#include <printf.h>
#include <target.h>
#ifdef MTK_EMMC_SUPPORT
#include <platform/mmc_core.h>
#endif

#define ERRMSG(...)
#ifdef MTK_EMMC_SUPPORT
#define PMT
#else
#define PMT
#endif

#ifdef PMT
part_t tempart;
pt_resident lastest_part[PART_MAX_COUNT];
#endif

static part_dev_t *mt_part_dev;
extern void part_init_pmt(unsigned long totalblks, part_dev_t *dev);
//extern int mmc_do_erase(int dev_num,u64 start_addr,u64 len);

extern part_t partition_layout[];
//#ifdef MTK_MLC_NAND_SUPPORT
//__attribute__ ((aligned(64))) static uchar mt_part_buf[32768];
//#else
static uchar *mt_part_buf;
//#endif


void mt_part_init(unsigned long totalblks)
{
	part_t *part = &partition_layout[0];
	unsigned long lastblk;
	unsigned int last_part_id;

	if (!totalblks) {
		return;
	}

	/* updater the number of blks of first part. */
	if (totalblks <= part->blknum) {
		part->blknum = totalblks;
	}

	totalblks -= part->blknum;
	lastblk = part->startblk + part->blknum;
#if defined(MTK_MLC_NAND_SUPPORT)
	if (part->type == TYPE_LOW)
		lastblk = part->startblk + part->blknum*2;
#endif
#if defined(MTK_TLC_NAND_SUPPORT)
	if ((part->type == TYPE_SLC) || (part->type == TYPE_LOW))
		lastblk = part->startblk + part->blknum*3;
#endif
	last_part_id = part->part_id;

	while (totalblks) {
		part++;
		if (!part->name) {
			break;
		}

		if (part->flags & PART_FLAG_LEFT || totalblks <= part->blknum) {
			part->blknum = totalblks;
		}

		if (part->part_id == last_part_id) {
			part->startblk = lastblk;
		}
		totalblks -= part->blknum;
		lastblk = part->startblk + part->blknum;
#if defined(MTK_MLC_NAND_SUPPORT)
		if (part->type == TYPE_LOW)
			lastblk = part->startblk + part->blknum*2;
#endif
#if defined(MTK_TLC_NAND_SUPPORT)
		if ((part->type == TYPE_SLC) || (part->type == TYPE_LOW))
			lastblk = part->startblk + part->blknum*3;
#endif

		last_part_id = part->part_id;
#if defined(MTK_MLC_NAND_SUPPORT) || defined(MTK_TLC_NAND_SUPPORT)
		printf ("[%s] part->startblk %x\n", __FUNCTION__, part->startblk);
#endif
	}
}

part_t* mt_part_get_partition(char *name)
{
	int index = 0;
	printf("[%s] %s\n", __FUNCTION__, name);
	if (!strcmp(name, "para")) {
		name = "MISC";
	} else if (!strcmp(name, "logo")) {
		name = "LOGO";
	} else if (!strcmp(name, "seccfg")) {
		name = "SECCFG";
	} else if (!strcmp(name, "secro")) {
		name = "SEC_RO";
	} else if (!strcmp(name, "boot")) {
		name = "BOOTIMG";
	}
	printf("[%s] rename %s\n", __FUNCTION__, name);

	part_t *part = &partition_layout[0];

	while (part->name) {
		if (!strcmp(name, (char *)part->name)) {
#ifdef PMT
			tempart.name = part->name;
			//when download get partitin used new,orther wise used latest
			tempart.startblk = BLK_NUM(lastest_part[index].offset);
			tempart.blknum = BLK_NUM(lastest_part[index].size);
			tempart.flags = part->flags;
			tempart.part_id = lastest_part[index].part_id;
			printf("[%s] %x\n", __FUNCTION__, tempart.startblk);
			return &tempart;
#endif
			return part;
		}
		index++;
		part++;
	}
	return NULL;
}

#ifdef MTK_EMMC_SUPPORT
void mt_part_dump(void)
{
	part_t *part = &partition_layout[0];

	printf("\nPart Info from compiler.(1blk=%dB):\n", BLK_SIZE);
	printf("\nPart Info.(1blk=%dB):\n", BLK_SIZE);
	while (part->name) {
		printf("[0x%016llx-0x%016llx] (%.8ld blocks): \"%s\"\n",
		       (u64)part->startblk * BLK_SIZE,
		       (u64)(part->startblk + part->blknum) * BLK_SIZE - 1,
		       part->blknum, part->name);
		part++;
	}
	printf("\n");
}

int mt_part_generic_read(part_dev_t *dev, u64 src, uchar *dst, int size, unsigned int part_id)
{
	int dev_id = dev->id;
	uchar *buf = &mt_part_buf[0];
	block_dev_desc_t *blkdev = dev->blkdev;
	u64 end, part_start, part_end, part_len, aligned_start, aligned_end;
	ulong blknr, blkcnt;

	if (!blkdev) {
		ERRMSG("No block device registered\n");
		return -ENODEV;
	}

	if (size == 0) {
		return 0;
	}

	end = src + size;

	part_start    = src &  ((u64)BLK_SIZE - 1);
	part_end      = end &  ((u64)BLK_SIZE - 1);
	aligned_start = src & ~((u64)BLK_SIZE - 1);
	aligned_end   = end & ~((u64)BLK_SIZE - 1);

	if (part_start) {
		blknr = aligned_start >> BLK_BITS;
		part_len = BLK_SIZE - part_start;
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1) {
			return -EIO;
		}
		memcpy(dst, buf + part_start, part_len);
		dst += part_len;
		src += part_len;
	}

	aligned_start = src & ~((u64)BLK_SIZE - 1);
	blknr  = aligned_start >> BLK_BITS;
	blkcnt = (aligned_end - aligned_start) >> BLK_BITS;

	if ((blkdev->block_read(dev_id, blknr, blkcnt, (unsigned long *)(dst), part_id)) != blkcnt) {
		return -EIO;
	}

	src += (blkcnt << BLK_BITS);
	dst += (blkcnt << BLK_BITS);

	if (part_end && src < end) {
		blknr = aligned_end >> BLK_BITS;
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1) {
			return -EIO;
		}
		memcpy(dst, buf, part_end);
	}
	return size;
}

static int mt_part_generic_write(part_dev_t *dev, uchar *src, u64 dst, int size, unsigned int part_id)
{
	int dev_id = dev->id;
	uchar *buf = &mt_part_buf[0];
	block_dev_desc_t *blkdev = dev->blkdev;
	u64 end, part_start, part_end, part_len, aligned_start, aligned_end;
	ulong blknr, blkcnt;

	if (!blkdev) {
		ERRMSG("No block device registered\n");
		return -ENODEV;
	}

	if (size == 0) {
		return 0;
	}

	end = dst + size;

	part_start    = dst &  ((u64)BLK_SIZE - 1);
	part_end      = end &  ((u64)BLK_SIZE - 1);
	aligned_start = dst & ~((u64)BLK_SIZE - 1);
	aligned_end   = end & ~((u64)BLK_SIZE - 1);

	if (part_start) {
		blknr = aligned_start >> BLK_BITS;
		part_len = BLK_SIZE - part_start;
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1) {
			return -EIO;
		}
		memcpy(buf + part_start, src, part_len);
		if ((blkdev->block_write(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1) {
			return -EIO;
		}
		dst += part_len;
		src += part_len;
	}

	aligned_start = dst & ~((u64)BLK_SIZE - 1);
	blknr  = aligned_start >> BLK_BITS;
	blkcnt = (aligned_end - aligned_start) >> BLK_BITS;

	if ((blkdev->block_write(dev_id, blknr, blkcnt, (unsigned long *)(src), part_id)) != blkcnt)
		return -EIO;

	src += (blkcnt << BLK_BITS);
	dst += (blkcnt << BLK_BITS);

	if (part_end && dst < end) {
		blknr = aligned_end >> BLK_BITS;
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1) {
			return -EIO;
		}
		memcpy(buf, src, part_end);
		if ((blkdev->block_write(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1) {
			return -EIO;
		}
	}
	return size;
}

int mt_part_register_device(part_dev_t *dev)
{
	printf("[mt_part_register_device]\n");
	if (!mt_part_dev) {
		if (!dev->read) {
			dev->read = mt_part_generic_read;
		}
		if (!dev->write) {
			dev->write = mt_part_generic_write;
		}
		mt_part_dev = dev;

		mt_part_buf = (uchar*)malloc(BLK_SIZE * 2);
		printf("[mt_part_register_device]malloc %d : %x\n",(BLK_SIZE * 2), mt_part_buf);

#ifdef PMT
		part_init_pmt((unsigned long)dev->blkdev->lba, dev);
#else
		mt_part_init((unsigned long)dev->blkdev->lba);
#endif
	}
	return 0;
}

#else

void mt_part_dump(void)
{
	part_t *part = &partition_layout[0];

	printf("\nPart Info from compiler.(1blk=%dB):\n", BLK_SIZE);
	printf("\nPart Info.(1blk=%dB):\n", BLK_SIZE);
	while (part->name) {
		printf("[0x%.8x-0x%.8x] (%.8ld blocks): \"%s\"\n",
		       part->startblk * BLK_SIZE,
		       (part->startblk + part->blknum) * BLK_SIZE - 1,
		       part->blknum, part->name);
		part++;
	}
	printf("\n");
}

int mt_part_generic_read(part_dev_t *dev, u64 src, uchar *dst, u32 size, unsigned int part_id)
{
	int dev_id = dev->id;
	uchar *buf = &mt_part_buf[0];
	block_dev_desc_t *blkdev = dev->blkdev;
	u64 end, part_start, part_end, part_len, aligned_start, aligned_end;
	ulong blknr, blkcnt;

	if (!blkdev) {
		ERRMSG("No block device registered\n");
		return -ENODEV;
	}

	if (size == 0) {
		return 0;
	}

	end = src + size;

	part_start    = src &  ((u64)BLK_SIZE - 1);
	part_end      = end &  ((u64)BLK_SIZE - 1);
	aligned_start = src & ~((u64)BLK_SIZE - 1);
	aligned_end   = end & ~((u64)BLK_SIZE - 1);

	if (part_start) {
		blknr = (ulong)(aligned_start >> BLK_BITS);
		part_len = (u64)BLK_SIZE - part_start;
		if (part_len>(u64)size) {
			part_len = (u64)size;
		}
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf, part_id))!= 1) {
			return -EIO;
		}
		memcpy(dst, buf + part_start, part_len);
		dst += part_len;
		src += part_len;
	}

	aligned_start = src & ~((u64)BLK_SIZE - 1);
	blknr  = (ulong)(aligned_start >> BLK_BITS);
	blkcnt = (ulong)((aligned_end - aligned_start) >> BLK_BITS);

	if (blkcnt!=0) {
		if ((blkdev->block_read(dev_id, blknr, blkcnt, (unsigned long *)(dst), part_id)) != blkcnt) {
			return -EIO;
		}
	}

	src += ((u64)blkcnt << BLK_BITS);
	dst += ((u64)blkcnt << BLK_BITS);

	if (part_end && src < end) {
		blknr = (ulong)(aligned_end >> BLK_BITS);
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1) {
			return -EIO;
		}
		memcpy(dst, buf, part_end);
	}
	return size;
}

static int mt_part_generic_write(part_dev_t *dev, uchar *src, u64 dst, u32 size, unsigned int part_id)
{
	int dev_id = dev->id;
	uchar *buf = &mt_part_buf[0];
	block_dev_desc_t *blkdev = dev->blkdev;
	u64 end, part_start, part_end, part_len, aligned_start, aligned_end;
	ulong blknr, blkcnt;

	if (!blkdev) {
		ERRMSG("No block device registered\n");
		return -ENODEV;
	}

	if (size == 0) {
		return 0;
	}

	end = dst + size;

	part_start    = dst &  ((u64)BLK_SIZE - 1);
	part_end      = end &  ((u64)BLK_SIZE - 1);
	aligned_start = dst & ~((u64)BLK_SIZE - 1);
	aligned_end   = end & ~((u64)BLK_SIZE - 1);

	if (part_start) {
		blknr = (ulong)(aligned_start >> BLK_BITS);
		part_len = (u64)BLK_SIZE - part_start;
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1)
			return -EIO;
		memcpy(buf + part_start, src, part_len);
		if ((blkdev->block_write(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1)
			return -EIO;
		dst += part_len;
		src += part_len;
	}

	aligned_start = dst & ~((u64)BLK_SIZE - 1);
	blknr  = (ulong)(aligned_start >> BLK_BITS);
	blkcnt = (ulong)((aligned_end - aligned_start) >> BLK_BITS);

	if ((blkdev->block_write(dev_id, blknr, blkcnt, (unsigned long *)(src), part_id)) != blkcnt) {
		return -EIO;
	}

	src += ((u64)blkcnt << BLK_BITS);
	dst += ((u64)blkcnt << BLK_BITS);

	if (part_end && dst < end) {
		blknr = (u64)(aligned_end >> BLK_BITS);
		if ((blkdev->block_read(dev_id, blknr, 1, (unsigned long*)buf, part_id)) != 1) {
			return -EIO;
		}
		memcpy(buf, src, part_end);
		if ((blkdev->block_write(dev_id, blknr, 1, (unsigned long*)buf, part_id))!= 1) {
			return -EIO;
		}
	}
	return size;
}

int mt_part_register_device(part_dev_t *dev)
{
	printf("[mt_part_register_device]\n");
	if (!mt_part_dev) {
		if (!dev->read) {
			dev->read = mt_part_generic_read;
		}
		if (!dev->write) {
			dev->write = mt_part_generic_write;
		}
		mt_part_dev = dev;

#if !defined(MTK_MLC_NAND_SUPPORT) && !defined(MTK_TLC_NAND_SUPPORT)
		mt_part_buf = (uchar*)malloc(BLK_SIZE * 2);
		printf("[mt_part_register_device]malloc %d : %x\n", (BLK_SIZE * 2), mt_part_buf);
#else
		printf("[mt_part_register_device]malloc %d : %x\n", sizeof(mt_part_buf), mt_part_buf);
#endif

#ifdef PMT
		part_init_pmt(BLK_NUM(1 * GB), dev);
#else
		mt_part_init(BLK_NUM(1 * GB));
#endif
	}

	return 0;
}
#endif

part_dev_t *mt_part_get_device(void)
{
	if (mt_part_dev && !mt_part_dev->init && mt_part_dev->init_dev) {
		mt_part_dev->init_dev(mt_part_dev->id);
		mt_part_dev->init = 1;
	}
	return mt_part_dev;
}

/**/
/*fastboot*/
/**/
unsigned int write_partition(unsigned size, unsigned char *partition)
{
	return 0;
}

int partition_get_index(const char * name)
{
	int index;

	for (index = 0; index < PART_MAX_COUNT; index++) {
		if (!strcmp(name, g_part_name_map[index].fb_name)) {
			printf("[%s]find %s %s index %d\n", __FUNCTION__, name, g_part_name_map[index].r_name, g_part_name_map[index].partition_idx);
			return g_part_name_map[index].partition_idx;
		}
	}

	if (index == PART_MAX_COUNT) {
		return -1;
	}
	return -1;
}

unsigned int partition_get_region(int index)
{
	part_t *p = mt_part_get_partition(g_part_name_map[index].r_name);
	if (p == NULL) {
		return -1;
	}
	return p->part_id;
}

u64 partition_get_offset(int index)
{
	part_t *p = mt_part_get_partition(g_part_name_map[index].r_name);
	if (p == NULL) {
		return -1;
	}
	return (u64)p->startblk * BLK_SIZE;
}

u64 partition_get_size(int index)
{
	part_t *p = mt_part_get_partition(g_part_name_map[index].r_name);
	if (p == NULL) {
		return -1;
	}
	return (u64)p->blknum * BLK_SIZE;
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

int partition_get_active_bit_by_name(char *name) {
	return 0;
}

#ifdef MTK_EMMC_SUPPORT
u64 emmc_write(u32 part_id, u64 offset, void *data, u64 size)
{
	part_dev_t *dev = mt_part_get_device();
	return (u64)dev->write(dev,data,offset,(int)size, part_id);
}

u64 emmc_read(u32 part_id, u64 offset, void *data, u64 size)
{
	part_dev_t *dev = mt_part_get_device();
	return (u64)dev->read(dev,offset,data,(int)size, part_id);
}

int emmc_erase(u32 part_id, u64 offset, u64 size)
{
	return mmc_do_erase(0,offset,size, part_id);
}
#endif
unsigned long long partition_reserve_size(void)
{
	unsigned long long size = 0;
#ifdef MTK_EMMC_SUPPORT
	if (target_is_emmc_boot()) {
#ifdef MTK_EMMC_SUPPORT_OTP
		size += PART_SIZE_OTP;
#endif
		size += PART_SIZE_BMTPOOL * (128 * 1024);
	}
#endif
	return size;
}
