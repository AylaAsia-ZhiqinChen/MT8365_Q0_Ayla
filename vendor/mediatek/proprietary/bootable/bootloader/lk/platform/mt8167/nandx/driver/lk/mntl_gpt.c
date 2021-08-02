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

#include <platform/boot_mode.h>
#include <platform/mmc_core.h>
#include <mntl.h>
#include <mntl_types.h>
#include <mntl_gpt.h>
#include <debug.h>
#include <printf.h>

struct mntl_part_name_alias {
	const char *alias_name;
	const char *name;
};

static char *mntl_gpt_entry;
static char mntl_gpt_uuid[64];
static GuidPartitionTableHeader_t pgpt_header;

static struct mntl_part_name_alias mntl_gpt_name[] = {
	{"system", "mntlblk_d1"},
	{"vendor", "mntlblk_d2"}
};

inline static int mntl_sector_size(void)
{
	return 4096;
}

static int mntl_get_gpt_entry(struct mntl_handler *mntl, int sector_size)
{
	char *ptr = NULL;
	int ret;
	struct mntl_scatter_list list[2];
	uint64_t len = 2 * sector_size;

	dprintf(CRITICAL, "len %llu\n", len);
	ptr = (char *)malloc(len);
	if (ptr == NULL) {
		dprintf(CRITICAL, "Malloc fail\n");
		return 1;
	}

	list[0].address = ptr;
	list[0].size = len;
	list[1].size = 0;
	ret = mntl_read(mntl, 0, &list);
	if (ret < 0) {
		dprintf(CRITICAL, "Read sector fail\n");
		free(ptr);
		return 1;
	}

	memcpy(&pgpt_header, &ptr[sector_size], sizeof(pgpt_header));
	free(ptr);

	dprintf(CRITICAL, "PGPT:\n");
	dprintf(CRITICAL, "Header CRC=%u\n",
	        (unsigned int)pgpt_header.HeaderCRC32);
	dprintf(CRITICAL, "Header Size=%d\n", pgpt_header.HeaderSize);
	dprintf(CRITICAL, "Current LBA=%llu\n", pgpt_header.MyLBA);
	dprintf(CRITICAL, "Backup LBA=%llu\n", pgpt_header.AlternateLBA);
	dprintf(CRITICAL, "First usable LBA=%llu\n",
	        pgpt_header.FirstUsableLBA);
	dprintf(CRITICAL, "Last usable LBA=%llu\n",
	        pgpt_header.LastUsableLBA);
	dprintf(CRITICAL, "Starting PE LBA=%llu\n",
	        pgpt_header.PartitionEntryLBA);
	dprintf(CRITICAL, "Number of PE=%d\n",
	        pgpt_header.NumberOfPartitionEntries);
	dprintf(CRITICAL, "Size of PE=%d\n",
	        pgpt_header.SizeOfPartitionEntry);
	dprintf(CRITICAL, "PE CRC=%u\n",
	        pgpt_header.PartitionEntryArrayCRC32);

	len =
	    ((uint64_t) pgpt_header.NumberOfPartitionEntries *
	     (uint64_t) pgpt_header.SizeOfPartitionEntry);

	/* align multiple of sector_size */
	len = (len / sector_size + 1) * sector_size;
	if ((len >> 32) > 0) {
		dprintf(CRITICAL,
		        "Not support buffer size larger than 32-bit\n");
		return 1;
	}

	mntl_gpt_entry = (char *)malloc(len);
	if (mntl_gpt_entry == NULL) {
		dprintf(CRITICAL, "Malloc fail\n");
		return 1;
	}

	list[0].address = mntl_gpt_entry;
	list[0].size = len;
	list[1].size = 0;
	ret = mntl_read(mntl, pgpt_header.PartitionEntryLBA, &list);

	if (ret < 0) {
		dprintf(CRITICAL, "Read PE fail\n");
		free(mntl_gpt_entry);
		mntl_gpt_entry = NULL;
		return 1;
	}

	return 0;
}

int mntl_gpt_init(struct mntl_handler *mntl)
{
	GuidPartitionEntry_t *pe;
	int sector_size = 0;
	int i = 0, ret;

	/* mntl_debug_flags = 0x7fffffff; */
	dprintf(CRITICAL, "Read gpt start!\n");

	/* get sector size */
	sector_size = mntl_sector_size();
	if (sector_size == 0) {
		dprintf(CRITICAL, "Get sector size fail!\n");
		return 1;
	}

	/* read partition entry */
	ret = mntl_get_gpt_entry(mntl, sector_size);
	if (ret > 0) {
		dprintf(CRITICAL, "Get partition entry fail!\n");
		return 1;
	}
	dprintf(CRITICAL, "First usable LBA=%llu\n",
	        pgpt_header.FirstUsableLBA);
	pe = (GuidPartitionEntry_t *) mntl_gpt_entry;
	for (i = 0; i < pgpt_header.NumberOfPartitionEntries; i++, pe++) {
		unsigned int j;
		char name[37];

		dprintf(CRITICAL, "Partition Entry %d\n", i + 1);
		dprintf(CRITICAL, "\tFirst LBA=%llu\n", pe->StartingLBA);
		dprintf(CRITICAL, "\tLast LBA=%llu\n", pe->EndingLBA);
		for (j = 0; j < 72 / sizeof(efi_char16_t); j++)
			name[j] = (uint16_t) pe->PartitionName[j];

		name[j] = 0;
		dprintf(CRITICAL, "\tName=%s\n", name);
	}
	return 0;
}

static GuidPartitionEntry_t *mntl_get_gpt_entry_by_name(const char *part_name)
{

	GuidPartitionEntry_t *pe;
	char r_name[32], name[37];
	int i, j, size;

	size = sizeof(mntl_gpt_name) / sizeof(mntl_gpt_name[0]);
	for (i = 0; i < size; i++) {
		if (!strcmp(mntl_gpt_name[i].alias_name, part_name)) {
			strcpy(r_name, mntl_gpt_name[i].name);
			break;
		}
	}

	if (i == size)
		strcpy(r_name, part_name);

	pe = (GuidPartitionEntry_t *) mntl_gpt_entry;

	for (i = 0; i < pgpt_header.NumberOfPartitionEntries; i++, pe++) {
		for (j = 0; j < 72 / sizeof(efi_char16_t); j++)
			name[j] = (uint16_t) pe->PartitionName[j];

		name[j] = 0;
		dprintf(CRITICAL, "\tName=%s\n", name);

		if (!strcmp(name, r_name)) {
			dprintf(CRITICAL, "Partition Entry %d\n", i + 1);
			return pe;
		}
	}

	dprintf(CRITICAL, "Cannot find partition %s\n", part_name);

	return NULL;
}

int mntl_read_gpt(const char *part_name, uint64_t *StartLBA, uint64_t *EndLBA)
{
	GuidPartitionEntry_t *pe;

	pe =  mntl_get_gpt_entry_by_name(part_name);
	if (!pe)
		return 1;

	dprintf(CRITICAL, "\tFirst LBA=%llu\n", pe->StartingLBA);
	dprintf(CRITICAL, "\tLast LBA=%llu\n", pe->EndingLBA);
	*StartLBA = pe->StartingLBA;
	*EndLBA = pe->EndingLBA;

	return 0;
}

char *mntl_gpt_uuid_by_name(const char *part_name)
{
	GuidPartitionEntry_t *pe;
	efi_guid_t *uuid = NULL;
	int i;

	pe =  mntl_get_gpt_entry_by_name(part_name);
	if (!pe)
		return NULL;

	uuid = &pe->UniquePartitionGuid;

	sprintf(mntl_gpt_uuid,"%08x-%04x-%04x-%02x%02x-",
				uuid->time_low,
				uuid->time_mid,
				uuid->time_hi_and_version,
				uuid->clock_seq_hi_and_reserved,
				uuid->clock_seq_low);

	for (i = 0; i < 6; i++)
		sprintf(mntl_gpt_uuid + i * 2 + 24, "%02x", uuid->node[i]);

	dprintf(CRITICAL, "Mntl-%s uuid: %s\n", part_name, mntl_gpt_uuid);

	return mntl_gpt_uuid;
}
