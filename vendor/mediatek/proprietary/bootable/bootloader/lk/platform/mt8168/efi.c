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
#include "printf.h"
#include "malloc.h"
#include "string.h"
#include "lib/crc.h"
#include "platform/mt_typedefs.h"
#include "platform/partition.h"
#include "platform/efi.h"

#ifndef min
#define min(x, y)   (x < y ? x : y)
#endif


static part_t *part_ptr = NULL;

/*
 ********** Definition of Debug Macro **********
 */
#define TAG "[GPT_LK]"
#define BLOCK_SIZE 512
#define ENTRIES_SIZE 0x4000

#define LEVEL_ERR   (0x0001)
#define LEVEL_INFO  (0x0004)

#define DEBUG_LEVEL (LEVEL_ERR | LEVEL_INFO)

#define efi_err(fmt, args...)   \
do {    \
    if (DEBUG_LEVEL & LEVEL_ERR) {  \
        dprintf(CRITICAL, fmt, ##args); \
    }   \
} while (0)

#define efi_info(fmt, args...)  \
do {    \
    if (DEBUG_LEVEL & LEVEL_INFO) {  \
        dprintf(CRITICAL, fmt, ##args);    \
    }   \
} while (0)


/*
 ********** Definition of GPT buffer **********
 */

/*
 ********** Definition of CRC32 Calculation **********
 */
#if 0
static int crc32_table_init = 0;
static u32 crc32_table[256];

static void init_crc32_table(void)
{
    int i, j;
    u32 crc;

    if (crc32_table_init) {
        return;
    }

    for (i = 0; i < 256; i++) {
        crc = i;
        for (j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
        crc32_table[i] = crc;
    }
    crc32_table_init = 1;
}

static u32 crc32(u32 crc, u8 *p, u32 len)
{
    init_crc32_table();

    while (len--) {
        crc ^= *p++;
        crc = (crc >> 8) ^ crc32_table[crc & 255];
    }

    return crc;
}
#endif

static u32 efi_crc32(u8 *p, u32 len)
{
    //return (crc32(~0L, p, len) ^ ~0L);
    //return crc32(0, p, len);  /* from zlib */
    return (crc32_no_comp(~0L, p, len) ^ ~0L);  /* from zlib */
}

static void w2s(u8 *dst, int dst_max, u16 *src, int src_max)
{
    int i = 0;
    int len = min(src_max, dst_max - 1);

    while (i < len) {
        if (!src[i]) {
            break;
        }
        dst[i] = src[i] & 0xFF;
        i++;
    }

    dst[i] = 0;

    return;
}

extern u64 g_emmc_user_size;

static int read_data(u8 *buf, u32 part_id, u64 lba, u64 size)
{
    int err;
    part_dev_t *dev;

    dev = mt_part_get_device();
    if (!dev) {
        efi_err("%sread data, err(no dev)\n", TAG);
        return 1;
    }

    err = dev->read(dev, lba * dev->blkdev->blksz, buf, (int)size, part_id);
    if (err != (int)size) {
        efi_err("%sread data, err(%d)\n", TAG, err);
        return err;
    }

    return 0;
}


const char hex_asc[] = "0123456789abcdef";
#define hex_asc_lo(x) hex_asc[((x)&0x0f)];
#define hex_asc_hi(x) hex_asc[((x)&0xf0)>>4];
static inline char *hex_byte_pack(char *buf, u8 byte)
{
	*buf++ = hex_asc_hi(byte);
	*buf++ = hex_asc_lo(byte);
	return buf;
}

static u8 *string(u8 *buf, u8 *end, const char *s)
{
	int len, i;
	len = strnlen(s, PART_META_INFO_UUIDLEN);
	for (i = 0; i < len; ++i) {
		if (buf < end)
			*buf = *s;
		++buf;
		++s;
	}
	if (buf < end)
		*buf = 0;
	return buf;
}

static u8 *uuid_string(u8 *buf, u8 *args)
{
	/* uuid format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxx, which contains 16*2 + 4('-') + 1 ('\0') = 37 bytes*/
	char uuid[PART_META_INFO_UUIDLEN];
	char *p = uuid;
	int i;
	static const u8 index[16] = {3,2,1,0,5,4,7,6,8,9,10,11,12,13,14,15};
	u8 *end;

	end = buf + PART_META_INFO_UUIDLEN;
	for (i = 0; i < 16; i++) {
		p = hex_byte_pack(p, args[index[i]]);
		switch(i) {
		case 3:
		case 5:
		case 7:
		case 9:
			*p++ = '-';
			break;
		default:
			break;
		}
	}

	*p = 0;
	return string(buf, end, uuid);
}

static inline u8 *efi_guid_unparse(efi_guid_t *guid, u8 *out)
{
	if (NULL == uuid_string(out, guid->b)) {
		efi_err("parse uuid string format fail!\n");
		return NULL;
	}
	return out;
}

static int parse_gpt_header(u32 part_id, u64 header_lba, u8 *header_buf, u8 *entries_buf)
{
    int i;

    int err;
    u32 calc_crc, orig_header_crc;
    u64 entries_real_size, entries_read_size;
    part_dev_t *dev = mt_part_get_device();

    gpt_header *primary_gpt_header = (gpt_header *)header_buf;
    gpt_header *secondary_gpt_header;
    gpt_entry *entries = (gpt_entry *)entries_buf;

    uint64_t ret = 0;
    uint64_t device_density;
    unsigned long long card_size_sec;
    int phy_last_part = 0;

    struct part_meta_info *info;

    err = read_data(header_buf, part_id, header_lba, dev->blkdev->blksz);
    if (err) {
        efi_err("%sread header(part_id=%d,lba=%llx), err(%d)\n",
                TAG, part_id, header_lba, err);
        return err;
    }

    if (primary_gpt_header->signature != GPT_HEADER_SIGNATURE) {
        efi_err("%scheck header, err(signature 0x%llx!=0x%llx)\n",
                TAG, primary_gpt_header->signature, GPT_HEADER_SIGNATURE);
        return 1;
    }

    orig_header_crc = primary_gpt_header->header_crc32;
    primary_gpt_header->header_crc32 = 0;
    calc_crc = efi_crc32((u8 *)primary_gpt_header, primary_gpt_header->header_size);

    if (orig_header_crc != calc_crc) {
        efi_err("%scheck header, err(crc 0x%x!=0x%x(calc))\n",
                TAG, orig_header_crc, calc_crc);
        return 1;
    }

    primary_gpt_header->header_crc32 = orig_header_crc;

    if (primary_gpt_header->my_lba != header_lba) {
        efi_err("%scheck header, err(my_lba 0x%llx!=0x%llx)\n",
                TAG, primary_gpt_header->my_lba, header_lba);
        return 1;
    }

    entries_real_size = (u64)primary_gpt_header->num_partition_entries * primary_gpt_header->sizeof_partition_entry;
    entries_read_size = (u64)((primary_gpt_header->num_partition_entries + 3) / 4) * dev->blkdev->blksz;

    err = read_data(entries_buf, part_id, primary_gpt_header->partition_entry_lba, entries_read_size);
    if (err) {
        efi_err("%sread entries(part_id=%d,lba=%llx), err(%d)\n",
                TAG, part_id, primary_gpt_header->partition_entry_lba, err);
        return err;
    }

    calc_crc = efi_crc32((u8 *)entries, (u32)entries_real_size);

    if (primary_gpt_header->partition_entry_array_crc32 != calc_crc) {
        efi_err("%scheck header, err(entries crc 0x%x!=0x%x(calc))\n",
                TAG, primary_gpt_header->partition_entry_array_crc32, calc_crc);
        return 1;
    }

    /* Parsing partition entries */
    for (i = 0; (u32)i < primary_gpt_header->num_partition_entries; i++) {
        /* break if the partition entry is empty */
        if (!entries[i].starting_lba) {
            break;
        }
        part_ptr[i].start_sect = (unsigned long)entries[i].starting_lba;
        part_ptr[i].nr_sects = (unsigned long)(entries[i].ending_lba - entries[i].starting_lba + 1);
	part_ptr[i].part_attr = (unsigned long)entries[i].attributes;
        part_ptr[i].part_id = dev->blkdev->part_user;
        info = malloc(sizeof(*info));
        if (!info) {
            continue;
        }
        part_ptr[i].info = info;
        if ((entries[i].partition_name[0] & 0xFF00) == 0) {
            w2s(part_ptr[i].info->name, PART_META_INFO_NAMELEN, entries[i].partition_name, GPT_ENTRY_NAME_LEN);
        } else {
            memcpy(part_ptr[i].info->name, entries[i].partition_name, 64);
        }

		if (NULL == efi_guid_unparse(&entries[i].unique_partition_guid, part_ptr[i].info->uuid)) {
			efi_err("efi parse uuid string fail!\n");
			return 1;
		}

		efi_info("%s[%d]name=%s, part_id=%d, start_sect=0x%lx, nr_sects=0x%lx, uuid=%s\n", TAG, i,
			part_ptr[i].info ? (char *)part_ptr[i].info->name : "unknown",
			part_ptr[i].part_id, part_ptr[i].start_sect, part_ptr[i].nr_sects,
			part_ptr[i].info ? (char *)part_ptr[i].info->uuid : "");
    }

	/* check whether to resize userdata partition */
	device_density = g_emmc_user_size;
	card_size_sec = device_density /dev->blkdev->blksz;
	efi_info("%s EMMC_PART_USER size = 0x%llx\n", TAG, device_density);

	if (primary_gpt_header->alternate_lba == (card_size_sec - 1))
		return 0;

	/* Patching primary header */
	primary_gpt_header->alternate_lba = card_size_sec - 1;
	primary_gpt_header->last_usable_lba = ((card_size_sec - 34) & 0xfffffff8) - 1;

	/* Patching secondary header */
	secondary_gpt_header = (gpt_header *)malloc(dev->blkdev->blksz);
	memcpy(secondary_gpt_header, primary_gpt_header, dev->blkdev->blksz);
	secondary_gpt_header->my_lba = card_size_sec - 1;
	secondary_gpt_header->alternate_lba = 1;
	secondary_gpt_header->last_usable_lba = ((card_size_sec - 34) & 0xfffffff8) - 1;
	secondary_gpt_header->partition_entry_lba = card_size_sec - 33;

	/* Find last partition */
	i = 0;
	while (entries[i].starting_lba) {
		if (entries[i].starting_lba >= entries[phy_last_part].starting_lba)
			phy_last_part = i;
		 i++;
	}
	efi_info("%s last partition number is %d\n", TAG, phy_last_part);

	/* Patching last partition */
	entries[phy_last_part].ending_lba = ((card_size_sec - 34) & 0xfffffff8) - 1;

	/* Updating CRC of the Partition entry array in both headers */
	calc_crc = efi_crc32((u8 *)entries, (u32)entries_real_size);
	primary_gpt_header->partition_entry_array_crc32 = calc_crc;
	secondary_gpt_header->partition_entry_array_crc32 = calc_crc;

	/* Clearing CRC fields to calculate */
	primary_gpt_header->header_crc32 = 0;
	calc_crc = efi_crc32((u8 *)primary_gpt_header, primary_gpt_header->header_size);
	primary_gpt_header->header_crc32 = calc_crc;

	secondary_gpt_header->header_crc32 = 0;
	calc_crc = efi_crc32((u8 *)secondary_gpt_header, primary_gpt_header->header_size);
	secondary_gpt_header->header_crc32 = calc_crc;

	/* write primary GPT header */
	ret = emmc_write(EMMC_PART_USER, BLOCK_SIZE, (unsigned int *)primary_gpt_header, BLOCK_SIZE);
	if (ret != BLOCK_SIZE) {
		efi_err("Failed to write primary GPT header\n");
		goto end;
	}

	/* write secondary GPT header */
	ret = emmc_write(EMMC_PART_USER, device_density - BLOCK_SIZE, (unsigned int *)secondary_gpt_header, BLOCK_SIZE);
	if (ret != BLOCK_SIZE) {
		efi_err("Failed to write secondary GPT header\n");
		goto end;
	}

	/* write primary Partition entries */
	ret = emmc_write(EMMC_PART_USER, BLOCK_SIZE * 2, (unsigned int *)entries, ENTRIES_SIZE);
	if (ret != ENTRIES_SIZE) {
		efi_err("Failed to write primary partition entries\n");
		goto end;
	}

	/* write secondary Partition entries */
	ret = emmc_write(EMMC_PART_USER, device_density - (BLOCK_SIZE + ENTRIES_SIZE), (unsigned int *)entries, ENTRIES_SIZE);
	if (ret != ENTRIES_SIZE) {
		efi_err("Failed to write secondary partition entries\n");
		goto end;
	}
	return 0;
end:
	return ret;

}


int read_gpt(part_t *part)
{
    int err;
    u64 last_lba;
    u8 *pgpt_header, *pgpt_entries;
    u8 *sgpt_header, *sgpt_entries;
    part_dev_t *dev = mt_part_get_device();
    u32 part_id = dev->blkdev->part_user;

    part_ptr = part;

    efi_info("%sParsing Primary GPT now...\n", TAG);

    pgpt_header = (u8 *)malloc(dev->blkdev->blksz);
    if (!pgpt_header) {
        efi_err("%smalloc memory(pgpt header), err\n", TAG);
        goto next_try;
    }
    memset(pgpt_header, 0, dev->blkdev->blksz);

    pgpt_entries = (u8 *)malloc(ENTRIES_SIZE);
    if (!pgpt_entries) {
        efi_err("%smalloc memory(pgpt entries), err\n", TAG);
        goto next_try;
    }
    memset(pgpt_entries, 0, ENTRIES_SIZE);

    err = parse_gpt_header(part_id, 1, pgpt_header, pgpt_entries);
    if (!err) {
        goto find;
    }

next_try:
    efi_info("%sParsing Secondary GPT now...\n", TAG);

    sgpt_header = (u8 *)malloc(dev->blkdev->blksz);
    if (!sgpt_header) {
        efi_err("%smalloc memory(sgpt header), err\n", TAG);
        goto next_try;
    }
    memset(sgpt_header, 0, dev->blkdev->blksz);

    sgpt_entries = (u8 *)malloc(ENTRIES_SIZE);
    if (!sgpt_entries) {
        efi_err("%smalloc memory(sgpt entries), err\n", TAG);
        goto next_try;
    }
    memset(sgpt_entries, 0, ENTRIES_SIZE);

    last_lba =  g_emmc_user_size / dev->blkdev->blksz - 1;
    err = parse_gpt_header(part_id, last_lba, sgpt_header, sgpt_entries);
    if (!err) {
        goto find;
    }

    efi_err("%sFailure to find valid GPT.\n", TAG);
    return err;

find:
    efi_info("%sSuccess to find valid GPT.\n", TAG);
    return 0;
}
