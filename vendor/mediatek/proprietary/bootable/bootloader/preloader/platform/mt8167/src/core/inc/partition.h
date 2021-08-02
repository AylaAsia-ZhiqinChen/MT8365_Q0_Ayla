/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
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

#ifndef PARTITION_H
#define PARTITION_H

#include "typedefs.h"
#include "blkdev.h"

#define PART_HEADER_DEFAULT_ADDR    (0xFFFFFFFF)
#define LOAD_ADDR_MODE_BACKWARD     (0x00000000)
#define PART_MAGIC          0x58881688
#define EXT_MAGIC           0x58891689

#define PART_FLAG_NONE              0
#define PART_FLAG_LEFT              0x1
#define PART_FLAG_END               0x2

#define KB  (1024)
#define MB  (1024 * KB)
#define GB  (1024 * MB)

#define PART_PRELOADER "PRELOADER"

#define PART_UBOOT "UBOOT"
#define PART_BOOTIMG "BOOTIMG"
#define PART_RECOVERY "RECOVERY"

#define PART_LOGO "LOGO"
#define PART_CACHE "CACHE"

#define PART_SECURE "SECURE"
#define PART_SECSTATIC "SECSTATIC"
#define PART_ANDSYSIMG "ANDSYSIMG"
#define PART_USER "USER"

#define PART_TEE1 "TEE1"
#define PART_TEE2 "TEE2"
#define PART_SECCFG "SECCFG"
#define PART_SEC_RO "SEC_RO"

#define PART_SIZE_SECCFG			(0)
#define PART_OFFSET_SECCFG			(0)
#define PART_OFFSET_SEC_RO			(0)
#define PART_SIZE_BMTPOOL			(0)

#ifndef RAND_START_ADDR
#define RAND_START_ADDR   1024
#endif

#if defined(MTK_MLC_NAND_SUPPORT) || defined(MTK_TLC_NAND_SUPPORT)
enum partition_type
{
	TYPE_LOW,
	TYPE_FULL,
	TYPE_SLC,
	TYPE_TLC,
};

#define NAND_FLASH_SLC   (0x0000)
#define NAND_FLASH_MLC   (0x0001)
#define NAND_FLASH_TLC   (0x0002)
#define NAND_FLASH_MLC_HYBER	(0x0003)
#define NAND_FLASH_MASK   (0x00FF)

#endif

typedef union
{
    struct
    {
        unsigned int magic;     /* partition magic */
        unsigned int dsize;     /* partition data size */
        char name[32];          /* partition name */
        unsigned int maddr;     /* partition memory address */
        unsigned int mode;      /* memory addressing mode */
        /* extension */
        unsigned int ext_magic; /* always EXT_MAGIC */
        unsigned int hdr_sz;
        unsigned int hdr_ver;
        unsigned int img_type;
        unsigned int img_list_end;
        unsigned int align_sz;
        unsigned int dsize_extend;
        unsigned int maddr_extend;
    } info;
    unsigned char data[512];
} part_hdr_t;

#define PART_ATTR_LEGACY_BIOS_BOOTABLE  (0x00000004UL)  /* bit2 = active bit for OTA */

#ifdef MTK_EMMC_SUPPORT

#define PART_META_INFO_NAMELEN  64
#define PART_META_INFO_UUIDLEN  16

struct part_meta_info {
    u8 name[PART_META_INFO_NAMELEN];
    u8 uuid[PART_META_INFO_UUIDLEN];
};

typedef struct {
    unsigned long start_sect;
    unsigned long nr_sects;
    unsigned long part_attr;
    unsigned int part_id;
    struct part_meta_info *info;
} part_t;

#else

typedef struct
{
    unsigned char *name;        /* partition name */
    unsigned long startblk;     /* partition start blk */
    unsigned long size;         /* partition size */
    unsigned long blks;         /* partition blks */
    unsigned long part_attr;
    unsigned long flags;        /* partition flags */
    unsigned int part_id;       // enum partition_type type;
#if defined(MTK_MLC_NAND_SUPPORT) || defined(MTK_TLC_NAND_SUPPORT)
    enum partition_type type;   // for MLC
    u16 nand_type;
 #endif
} part_t;

#endif

#define PART_MAX_COUNT             40
#define PART_HEADER_MEMADDR        (0xFFFFFFFF)

extern u32 get_seccfg_offset();
extern u32 get_seccfg_size();
extern u32 get_secro_offset();
extern u32 get_secro_length();

extern int part_init(void);
extern part_t *part_get(char *name);
extern int part_load(blkdev_t *bdev, part_t *part, u32 *addr, u32 offset, u32 *size);
extern void part_dump(void);

extern part_t *get_part(char *name);
extern void put_part(part_t *part);
extern int part_load_raw_part(blkdev_t *bdev, part_t *part, u32 *addr, u32 offset, u32 *size);
extern unsigned long mt_part_get_part_active_bit(part_t *part);

#endif /* PARTITION_H */
