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
#include <string.h>
#include <stdio.h>
#include <platform/errno.h>
#include "pmt.h"
#include <platform/mtk_nand.h>
#include <target.h>


#ifdef PMT

pt_resident lastest_part[PART_MAX_COUNT];
extern part_t partition_layout[];

static pt_info pi;

#if defined(MTK_TLC_NAND_SUPPORT)
extern u32 slc_ratio;
extern u32 sys_slc_ratio;
extern u32 usr_slc_ratio;
#endif
#define PMT_POOL_SIZE   (2)


//extern const struct MLC_feature_set feature_set[];

__attribute__((aligned(64))) unsigned char page_buf[32768 + 4096];
//unsigned char backup_buf[8192];


//BLK_SIZE is 512, block_size is from flash is 128K
static u32 block_size;
static u32 page_size;
extern flashdev_info devinfo;
extern struct NAND_CMD g_kCMD;
extern u64 total_size;
static bool init_pmt_done = FALSE;
part_t tempart;

u64 part_get_startaddress(u64 byte_address, int *idx)
{
	int index = 0;
	if (TRUE == init_pmt_done) {
		while (index < PART_MAX_COUNT) {
			if (lastest_part[index].offset > byte_address || lastest_part[index].size == 0) {
				*idx = index - 1;
				return lastest_part[index-1].offset;
			}
			index++;
			if (!strcmp(lastest_part[index-1].name,"BMTPOOL"))
				break;
		}
	}
	*idx = index - 1;
#if defined(MTK_TLC_NAND_SUPPORT)
	if (FALSE == init_pmt_done) {
		*idx = 0;
	}
#endif
	return byte_address;
}

bool raw_partition(u32 index)
{
	if ((partition_layout[index].type == TYPE_LOW)
#if defined(MTK_TLC_NAND_SUPPORT)
	        || (partition_layout[index].type == TYPE_SLC))
#else
	   )
#endif
		return TRUE;
	return FALSE;
}

unsigned long get_part_size(char* name)
{
	int i =0;
	for (i = 0; i < PART_MAX_COUNT; i++) {
		if (!strcmp(name,lastest_part[i].name))
			return lastest_part[i].size;
	}
	return 0;
}

unsigned long get_part_offset(char* name)
{
	int i =0;
	for (i = 0; i < PART_MAX_COUNT; i++) {
		if (!strcmp(name,lastest_part[i].name))
			return lastest_part[i].offset;
	}
	return 0;
}

void get_part_tab_from_complier(void)
{
	int index = 0;
	printf("get_pt_from_complier \n");
	while (partition_layout[index].flags != PART_FLAG_END) {
		memcpy(lastest_part[index].name,partition_layout[index].name, MAX_PARTITION_NAME_LEN);
		lastest_part[index].size = (u64)partition_layout[index].nr_sects * BLK_SIZE ;
		lastest_part[index].offset = (u64)partition_layout[index].start_sect * BLK_SIZE;
		if (lastest_part[index].size == 0) {
			lastest_part[index].size = total_size - lastest_part[index].offset;
		}
		lastest_part[index].mask_flags = partition_layout[index].flags;  //this flag in kernel should be fufilled even though in flash is 0.
		printf("get_ptr  %s %016llx %016llx\n",lastest_part[index].name,lastest_part[index].offset,lastest_part[index].size);
		index++;
	}
}

bool find_mirror_pt_from_bottom(u64 *start_addr, part_dev_t *dev)
{
	int mpt_locate;
	u64 mpt_start_addr;
	u64 current_start_addr=0;
	//unsigned int part_id;
	char pmt_spare[4];
	mpt_start_addr = total_size+block_size;
	//mpt_start_addr=MPT_LOCATION*block_size-page_size;
	for (mpt_locate = (block_size / page_size); mpt_locate > 0; mpt_locate--) {
		memset(pmt_spare, 0xFF, PT_SIG_SIZE);

		current_start_addr = mpt_start_addr + mpt_locate * page_size;
		if (!dev->read(dev, current_start_addr, page_buf, page_size, NAND_PART_USER)) {
			printf("find_mirror read  failed %x %x \n",current_start_addr,mpt_locate);
		}
		memcpy(&page_buf[page_size],g_kCMD.au1OOB,16);
		memcpy(pmt_spare,&page_buf[page_size] ,PT_SIG_SIZE);
		//need enhance must be the larget sequnce number
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#if defined(MTK_TLC_NAND_SUPPORT)
		if (is_valid_mpt(page_buf)) {
#else
		if (is_valid_mpt(page_buf) && is_valid_mpt(&pmt_spare)) {
#endif
			//if no pt, pt.has space is 0;
#if defined(MTK_TLC_NAND_SUPPORT)
			slc_ratio = *((u32 *)page_buf + 1);//slc ratio
			sys_slc_ratio = (slc_ratio >> 16)&0xFF;
			usr_slc_ratio = (slc_ratio)&0xFF;
			printf("[lk] slc ratio %d\n",slc_ratio);
#endif
			pi.sequencenumber = page_buf[PT_SIG_SIZE+page_size];
			printf("find_mirror find valid pt at %x sq %x \n",current_start_addr, pi.sequencenumber);
			break;
		} else {
			continue;
		}
	}
	if (mpt_locate == 0) {
		printf("no valid mirror page\n");
		pi.sequencenumber =  0;
		return FALSE;
	} else {
		*start_addr = current_start_addr;
		return TRUE;
	}
}

int load_exist_part_tab(u8 *buf, part_dev_t *dev)
{
	u64 pt_start_addr;
	u64 pt_cur_addr;
	u32 pt_locate;
	//unsigned int part_id;
	int reval=DM_ERR_OK;
	u64 mirror_address;
	char pmt_spare[PT_SIG_SIZE];

#if defined(MTK_TLC_NAND_SUPPORT)
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && (devinfo.tlcControl.normaltlc))
		block_size = devinfo.blocksize * 1024 / 3; // SLC MODE
	else
#endif
		block_size = devinfo.blocksize * 1024;
	page_size = devinfo.pagesize;

	//page_buf = malloc(page_size);

	pt_start_addr = total_size;
	printf("load_pt from 0x%llx \n",pt_start_addr);
	//pt_start_addr=PT_LOCATION*block_size;
	for (pt_locate=0; pt_locate < (block_size / page_size); pt_locate++) {
		pt_cur_addr = pt_start_addr+pt_locate*page_size;
		memset(pmt_spare,0xFF,PT_SIG_SIZE);

		if (!dev->read(dev, pt_cur_addr, page_buf, page_size, NAND_PART_USER)) {
			printf("load_pt read pt failded: %llx\n", pt_cur_addr);
		}

		memcpy(&page_buf[page_size],g_kCMD.au1OOB,16);

		memcpy(pmt_spare,&page_buf[page_size] ,PT_SIG_SIZE); //skip bad block flag
#if defined(MTK_TLC_NAND_SUPPORT)
		if (is_valid_pt(page_buf)) {
#else
		if (is_valid_pt(page_buf) && is_valid_pt(pmt_spare)) {
#endif
#if defined(MTK_TLC_NAND_SUPPORT)
			slc_ratio = *((u32 *)page_buf + 1);//slc ratio
			sys_slc_ratio = (slc_ratio >> 16)&0xFF;
			usr_slc_ratio = (slc_ratio)&0xFF;
			printf("[lk] slc ratio %d\n",slc_ratio);
#endif
			pi.sequencenumber = page_buf[PT_SIG_SIZE+page_size];
			printf("load_pt find valid pt at %llx sq %x \n",pt_start_addr, pi.sequencenumber);
			break;
		} else {
			continue;
		}
	}
	//for test
	//pt_locate==(block_size/page_size);
	if (pt_locate == (block_size/page_size)) {
		//first download or download is not compelte after erase or can not download last time
		printf("load_pt find pt failed \n");
		pi.pt_has_space = 0; //or before download pt power lost

		if (!find_mirror_pt_from_bottom(&mirror_address,dev)) {
			printf("First time download \n");
			reval=ERR_NO_EXIST;
			return reval;
		} else {
			//used the last valid mirror pt, at lease one is valid.
			dev->read(dev,mirror_address, page_buf,page_size, NAND_PART_USER);
		}
	}
	memcpy(buf,&page_buf[PT_SIG_SIZE],sizeof(lastest_part));

	return reval;
}

int part_get_mtd_index(char *name)
{
	int i;
	part_t *part = &partition_layout[0];
	for (i=0; i<PART_MAX_COUNT; i++,part++) {
		printf("part_get_mtd_index %s\n", part->name);
		if (!strncasecmp(name, part->name, strlen(name)))
			return i;
	}
	return -1;
}

void part_init_pmt(unsigned long totalblks, part_dev_t *dev)
{
	part_t *part = &partition_layout[0];
	unsigned long lastblk;
	int retval = 0;
	int i = 0;
	printf("mt8167_part_init_pmt \n");
	if (!totalblks) {
		return;
	}

	part->nr_sects = totalblks;
#if 0
	/* updater the number of blks of first part. */
	if (totalblks <= part->blknum) {
		part->blknum = totalblks;
	}

	totalblks -= part->blknum;
#ifdef MTK_MLC_NAND_SUPPORT
	if (part->type == TYPE_LOW)
		lastblk = part->startblk + part->blknum*2;
	else
#endif
		lastblk = part->startblk + part->blknum;

	while (totalblks) {
		part++;
		if (!part->name) {
			break;
		}

		if (part->flags & PART_FLAG_LEFT || totalblks <= part->blknum) {
			part->blknum = totalblks;
		}
		part->startblk = lastblk;
		totalblks -= part->blknum;
#ifdef MTK_MLC_NAND_SUPPORT
		if (part->type == TYPE_LOW)
			lastblk = part->startblk + part->blknum*2;
		else
#endif
			lastblk = part->startblk + part->blknum;
	}
#endif
	memset(&pi, 0xFF, sizeof(pi));
	memset(&lastest_part, 0, PART_MAX_COUNT * sizeof(pt_resident));
	retval = load_exist_part_tab((u8 *)&lastest_part, dev);
	if (retval == ERR_NO_EXIST) {
		//first run preloader before dowload
		//and valid mirror last download or first download
		printf("no pt \n");
		get_part_tab_from_complier(); //get from complier
	} else {
		printf("Find pt \n");
		for (i=0; i<PART_MAX_COUNT; i++,part++) {
			if (!strcmp(lastest_part[i-1].name,"BMTPOOL"))
				break;
			part->name = lastest_part[i].name;
			//part->size= lastest_part[i].size;
			part->nr_sects = lastest_part[i].size / BLK_SIZE;
			part->start_sect = lastest_part[i].offset / BLK_SIZE;
			part->flags = PART_FLAG_NONE;

			if (lastest_part[i].ext.type == REGION_LOW_PAGE) {
				part->type = TYPE_LOW;
			}
#if defined(MTK_TLC_NAND_SUPPORT)
			else if (lastest_part[i].ext.type == REGION_SLC_MODE) {
				part->type = TYPE_SLC;
			} else if (lastest_part[i].ext.type == REGION_TLC_MODE) {
				part->type = TYPE_TLC;
			}
#endif
			else {
				part->type = TYPE_FULL;
			}

			if (!strcmp(lastest_part[i].name,"BMTPOOL")) {
				lastest_part[i].offset = total_size + PMT_POOL_SIZE * (devinfo.blocksize * 1024);
#if defined(MTK_TLC_NAND_SUPPORT)
				if (devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC) {
					if (devinfo.tlcControl.normaltlc) {
						part->type = TYPE_SLC;
					} else
						part->type = TYPE_TLC;
				}
#endif
			}

			printf("partition %s size %016llx %016llx \n",lastest_part[i].name,lastest_part[i].offset,lastest_part[i].size);
		}
	}
	init_pmt_done = TRUE;
}

void mt_part_init(unsigned long totalblks)
{
	part_t *part = &partition_layout[0];
	unsigned long lastblk;
	unsigned int last_part_id;

	if (!totalblks) {
		return;
	}

	/* updater the number of blks of first part. */
	if (totalblks <= part->nr_sects) {
		part->nr_sects = totalblks;
	}

	totalblks -= part->start_sect;
#ifdef MTK_TLC_NAND_SUPPORT
	if (part->type == TYPE_LOW)
		lastblk = part->start_sect + part->nr_sects*3;
	else
#endif
		lastblk = part->start_sect + part->nr_sects;
	last_part_id = part->part_id;

	while (totalblks) {
		part++;
		if (!part->name) {
			break;
		}

		if (part->flags & PART_FLAG_LEFT || totalblks <= part->nr_sects) {
			part->nr_sects = totalblks;
		}

		if (part->part_id == last_part_id) {
			part->start_sect = lastblk;
		}
		totalblks -= part->nr_sects;
#ifdef MTK_TLC_NAND_SUPPORT
		if (part->type == TYPE_LOW)
			lastblk = part->start_sect + part->nr_sects*3;
		else
#endif
			lastblk = part->start_sect + part->nr_sects;;
		last_part_id = part->part_id;
#ifdef MTK_TLC_NAND_SUPPORT
		printf ("[%s] part->startblk %x\n", __FUNCTION__, part->start_sect);
#endif
	}
}

part_t* mt_part_get_partition(char *name)
{
	int index = 0;
	u32 blocksize = (devinfo.blocksize * 1024);
	char r_name[32];

	/* for tlc raw partition*/
	blocksize /= 3;

	strcpy(r_name, name);

	printf("[%s] %s\n", __FUNCTION__, name);
	if (!strcmp(name, "para")) {
		strcpy(r_name, "MISC");
	} else if (!strcmp(name, "logo")) {
		strcpy(r_name, "LOGO");
	} else if (!strcmp(name, "seccfg")) {
		strcpy(r_name, "SECCFG");
	} else if (!strcmp(name, "secro")) {
		strcpy(r_name, "SEC_RO");
	} else if (!strcmp(name, "boot")) {
		strcpy(r_name, "BOOTIMG");
	} else if (!strcmp(name, "preloader")) {
		strcpy(r_name, "PRELOADER");
	} else if (!strcmp(name, "recovery")) {
		strcpy(r_name, "RECOVERY");
	}

	printf("[%s] rename %s\n", __FUNCTION__, r_name);

	part_t *part = &partition_layout[0];

	while (part->name) {
		if (!strcmp(r_name, (char *)part->name)) {
#ifdef PMT
			tempart.name = part->name;
			//when download get partitin used new,orther wise used latest
			if (!strcmp(r_name, "PRELOADER"))
				tempart.start_sect = BLK_NUM(lastest_part[index].offset + blocksize);
			else
				tempart.start_sect = BLK_NUM(lastest_part[index].offset);
			tempart.nr_sects = BLK_NUM(lastest_part[index].size);
			tempart.flags = part->flags;
			tempart.part_id = lastest_part[index].part_id;
			printf("[%s] 0x%x\n", __FUNCTION__, tempart.start_sect);
			return &tempart;
#endif
			return part;
		}
		index++;
		part++;
	}
	printf("warning!!! not found this partition!");
	return NULL;
}



void mt_part_dump(void)
{
	part_t *part = &partition_layout[0];

	printf("\nPart Info from compiler.(1blk=%dB):\n", BLK_SIZE);
	printf("\nPart Info.(1blk=%dB):\n", BLK_SIZE);
	while (part->name) {
		printf("[0x%.8x-0x%.8x] (%.8ld blocks): \"%s\"\n",
		       part->start_sect * BLK_SIZE,
		       (part->start_sect + part->nr_sects) * BLK_SIZE - 1,
		       part->nr_sects, part->name);
		part++;
	}
	printf("\n");
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
	printf("[%s] %s\n", __FUNCTION__, name);


	if (!strcmp(name, "userdata")) {
		dprintf(CRITICAL, "NAND project does not support FTL2.0 partition operation in LK, refuse operation of 'userdata' partition\n");
		return -1;
	}

	for (index = 0; index < PART_MAX_COUNT; index++) {
		if ((!strcmp(name, g_part_name_map[index].fb_name))  ||(!strcmp(name, g_part_name_map[index].r_name))) {
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
	return (u64)p->start_sect * BLK_SIZE;
}

u64 partition_get_size(int index)
{
	part_t *p = mt_part_get_partition(g_part_name_map[index].r_name);
	if (p == NULL) {
		return -1;
	}
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
unsigned long long partition_reserve_size(void)
{
	unsigned long long size = 0;

	return size;
}

#if defined(MTK_TLC_NAND_SUPPORT)
extern u32 system_block_count;
u64 OFFSET(u32 block) //return logical address
{
	u64 offset;
	u32 idx;
	u64 start_address;
	bool raw_part;
	u64 real_address = (u64)block * (devinfo.blocksize * 1024);
	u32 total_blk_num; // total block number in FS partition
	u32 slc_blk_num;

	start_address = part_get_startaddress(real_address, &idx);
	if (raw_partition(idx))
		raw_part = TRUE;
	else
		raw_part = FALSE;

	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && devinfo.tlcControl.normaltlc) {
		if (raw_part)
			offset = start_address + (real_address - start_address) / 3;
		else {
			total_blk_num = (lastest_part[idx + 1].offset - lastest_part[idx].offset) / (devinfo.blocksize * 1024);
			if (!strcmp(lastest_part[idx].name,"ANDROID"))
				slc_blk_num = total_blk_num * sys_slc_ratio / 100;
			else {
				total_blk_num -= 2;//pmt block number
				slc_blk_num = total_blk_num * usr_slc_ratio / 100;
			}
			if (slc_blk_num % 3)
				slc_blk_num += (3 - (slc_blk_num % 3)); // slc block number must be 3 aligned
			if (block < (system_block_count - PMT_POOL_SIZE - slc_blk_num)) {
				offset = (((u64)block) * (devinfo.blocksize * 1024));
			} else {
				offset = (system_block_count - PMT_POOL_SIZE - slc_blk_num) * (devinfo.blocksize * 1024) + (block - (system_block_count - PMT_POOL_SIZE - slc_blk_num)) * (devinfo.blocksize * 1024) / 3;
			}
		}
	} else {
		offset = (((u64)block) * (devinfo.blocksize * 1024));
	}

	return offset;
}

void mtk_slc_blk_addr(u64 addr, u32* blk_num, u32* page_in_block)
{
	u64 start_address;
	u32 idx;
	u32 total_blk_num; // total block number in FS partition
	u32 slc_blk_num;
	u64 offset;
	u32 block_size = (devinfo.blocksize * 1024);

	start_address = part_get_startaddress(addr,&idx);
	if (raw_partition(idx)) { // raw partition just has SLC mode block
		*blk_num = (u32)((u32)(start_address / block_size) + (u32)((addr-start_address) / (block_size / 3)));
		*page_in_block = ((u32)((addr-start_address) / devinfo.pagesize) % ((block_size/devinfo.pagesize)/3));
		*page_in_block *= 3;
	} else {
		if ((addr < lastest_part[idx + 1].offset) && (addr >= (lastest_part[idx + 1].offset - PMT_POOL_SIZE * block_size))) {
			//for pmt update
			*blk_num = (u32)(addr / block_size);
			*page_in_block = ((u32)(addr / devinfo.pagesize) % ((block_size/devinfo.pagesize)/3));
			*page_in_block *= 3;
		} else {
			total_blk_num = (lastest_part[idx + 1].offset - lastest_part[idx].offset) / (devinfo.blocksize * 1024);
			if (!strcmp(lastest_part[idx].name,"ANDROID"))
				slc_blk_num = total_blk_num * sys_slc_ratio / 100;
			else {
				total_blk_num -= 2;//pmt block number
				slc_blk_num = total_blk_num * usr_slc_ratio / 100;
			}
			//total_blk_num -= 2;//pmt block number
			//slc_blk_num = total_blk_num * slc_ratio / 100;
			if (slc_blk_num % 3)
				slc_blk_num += (3 - (slc_blk_num % 3)); // slc block number must be 3 aligned

			offset = start_address + (u64)(devinfo.blocksize * 1024) * (total_blk_num - slc_blk_num);

			if (offset <= addr) {
				*blk_num = (u32)((u32)(offset / block_size) + (u32)((addr-offset) / (block_size / 3)));
				*page_in_block = ((u32)((addr-offset) / devinfo.pagesize) % ((block_size/devinfo.pagesize)/3));
				*page_in_block *= 3;
			} else {
				dprintf(INFO,"[xiaolei] error :this is not slc mode block\n");
				while (1);
			}
		}
	}
}

bool mtk_block_istlc(u64 addr)
{
	u64 start_address;
	u32 idx;
	u32 total_blk_num; // total block number in FS partition
	u32 slc_blk_num;
	u64 offset;

	start_address = part_get_startaddress(addr,&idx);
	if (raw_partition(idx)) { // raw partition just has SLC mode block
		return FALSE;
	} else {
		if (devinfo.tlcControl.normaltlc) {
			total_blk_num = (lastest_part[idx + 1].offset - lastest_part[idx].offset) / (devinfo.blocksize * 1024);
			if (!strcmp(lastest_part[idx].name,"ANDROID"))
				slc_blk_num = total_blk_num * sys_slc_ratio / 100;
			else {
				total_blk_num -= 2;//pmt block number
				slc_blk_num = total_blk_num * usr_slc_ratio / 100;
			}
			//slc_blk_num = total_blk_num * slc_ratio / 100;
			if (slc_blk_num % 3)
				slc_blk_num += (3 - (slc_blk_num % 3)); // slc block number must be 3 aligned

			offset = start_address + (u64)(devinfo.blocksize * 1024) * (total_blk_num - slc_blk_num);

			if (offset <= addr)
				return FALSE;
			else
				return TRUE;
		} else //micron tlc
			return TRUE;
	}
}

void mtk_pmt_reset(void)
{
	u32 index;
	if ((devinfo.NAND_FLASH_TYPE == NAND_FLASH_TLC)
	        && devinfo.tlcControl.normaltlc) {
		for (index = 0; index < PART_MAX_COUNT; index++) { //must have this step
			partition_layout[index].type = TYPE_SLC; //default SLC MODE
			lastest_part[index].offset = 0; //default 0
		}
	}
}

bool mtk_nand_IsBMTPOOL(u64 logical_address)
{
	u64 start_address;
	u32 idx;

	start_address = part_get_startaddress(logical_address,&idx);

	if ((!strcmp(lastest_part[idx].name,"BMTPOOL")) || (!init_pmt_done))
		return TRUE;
	else
		return FALSE;
}

#endif


#endif
