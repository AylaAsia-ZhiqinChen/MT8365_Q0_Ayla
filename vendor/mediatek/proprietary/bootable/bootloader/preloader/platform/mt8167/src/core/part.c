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

#include "typedefs.h"
#include "platform.h"
#include "blkdev.h"
#include "partition.h"
#include "gfh.h"
#include "dram_buffer.h"
#include "sec_boot.h"
#ifdef MTK_EMMC_SUPPORT
#include "mmc_core.h"
#endif
#if CFG_PMT_SUPPORT
#ifdef MTK_COMBO_NAND_SUPPORT
#include "nandx_pmt.h"
#else
#include "pmt.h"
#endif
#endif

#define MOD "PART"

#define TO_BLKS_ALIGN(size, blksz)  (((size) + (blksz) - 1) / (blksz))

u8 g_cur_part_name[32] = {0};
typedef union {
    part_hdr_t      part_hdr;
    gfh_file_info_t file_info_hdr;
} img_hdr_t;

#define IMG_HDR_BUF_SIZE 512

#define img_hdr_buf (g_dram_buf->img_hdr_buf)
#define part_num (g_dram_buf->part_num)
#define part_info (g_dram_buf->part_info)

u32 g_secure_dram_size = 0;


int part_init(void)
{
    blkdev_t *bdev;
    part_t *part;
    u32 erasesz;
    unsigned long lastblk;
    unsigned int last_part_id;

    part_num = 0;
    memset(part_info, 0x00, sizeof(part_info));

    #ifdef PL_PROFILING
    u32 profiling_time;
    profiling_time = 0;
    #endif
    
    cust_part_init();

    bdev = blkdev_get(CFG_BOOT_DEV);
    part = cust_part_tbl();

    if (!bdev || !part)
        return -1;

    erasesz = bdev->blksz;

    part->blks = TO_BLKS_ALIGN(part->size, erasesz);
    
	lastblk    = part->startblk + part->blks;   
#if defined(MTK_MLC_NAND_SUPPORT) || defined(MTK_TLC_NAND_SUPPORT)
	if(part->nand_type == NAND_FLASH_TLC) {
		if((part->type == TYPE_LOW) || (part->type == TYPE_SLC))
       			lastblk = part->startblk + part->blks*3;
	} else if((part->nand_type == NAND_FLASH_MLC) 
		|| (part->nand_type == NAND_FLASH_MLC_HYBER)) {
		if(part->type == TYPE_LOW)
	       		lastblk = part->startblk + part->blks*2;
	}
    #endif
    
    last_part_id = part->part_id;
	//print("[bean]part->startblk(0x%x),part->blks(0x%x),part->size(0x%x)\n", part->startblk, part->blks, part->size);

    while (1) {
        part++;
        if (!part->name)
            break;
        if (part->startblk == 0 && part->part_id == last_part_id) {
            part->startblk = lastblk;
			//print("[bean1]part->startblk(0x%x)\n", part->startblk);
        }
        part->blks = TO_BLKS_ALIGN(part->size, erasesz);
		lastblk = part->startblk + part->blks;
#if defined(MTK_MLC_NAND_SUPPORT) || defined(MTK_TLC_NAND_SUPPORT)
	if(part->nand_type == NAND_FLASH_TLC) {
		if((part->type == TYPE_LOW) || (part->type == TYPE_SLC))
       			lastblk = part->startblk + part->blks*3;
	} else if((part->nand_type == NAND_FLASH_MLC) 
		|| (part->nand_type == NAND_FLASH_MLC_HYBER)) {
		if(part->type == TYPE_LOW)
	       		lastblk = part->startblk + part->blks*2;
	}
        #endif
        
        last_part_id = part->part_id;
		//print("[bean]part->startblk(0x%x),part->blks(0x%x),part->size(0x%x)\n", part->startblk, part->blks, part->size);
    }
#if CFG_PMT_SUPPORT
    #ifdef PL_PROFILING
    profiling_time = get_timer(0);
    #endif
    pmt_init();
    #ifdef PL_PROFILING
    printf("#T#pmt_init=%d\n", get_timer(profiling_time));
    #endif
#endif
    return 0;
}

part_t *part_get(char *name)
{
    int index = 0;
    part_t *part = cust_part_tbl();

	if (!strcmp(name, "SECURE")) {
        name = "SECCFG"; 
	} else if (!strcmp(name, "SECSTATIC")) {
        name = "SEC_RO"; 
	} else if (!strcmp(name, "ANDSYSIMG")) {
        name = "ANDROID"; 
	} else if (!strcmp(name, "USER")) {
        name = "USRDATA"; 
    }

	if (!strcmp(name, "lk")) {
        name = "UBOOT";
	} else if (!strcmp(name, "lk2")) {
	name = "UBOOT2";
	} else if (!strcmp(name, "tee1")) {
        name = "TEE1"; 
	} else if (!strcmp(name, "tee2")) {
        name = "TEE2"; 
	} else if (!strcmp(name, "boot")) {
        name = "BOOTIMG"; 
    } else if (!strcmp(name, "secro")) {
        name = "SEC_RO";
    }else if (!strcmp(name, "seccfg")) {
        name = "SECCFG";
    }else if (!strcmp(name, "logo")) {
        name = "LOGO";
    }
	//do more name mapping here  xiaolei
	
    /* this api corrupts if length of name is > 32 */
    memcpy(g_cur_part_name, name, 32);

    while (part->name && index < PART_MAX_COUNT) {
        if (!strcmp(name, part->name)) {
        #if CFG_PMT_SUPPORT
		//print("[bean] pmt_get_part \n");
            return pmt_get_part(part, index);
        #else
            return part;
        #endif
        }
        part++; index++;
    }
    return NULL;
}

void put_part(part_t *part)
{
    return;
}

/* return > 0 if active bit exist, return = 0 if active bit is not existed*/
unsigned long mt_part_get_active_bit(part_t *part)
{
	return (part->part_attr & PART_ATTR_LEGACY_BIOS_BOOTABLE);
}

#if CFG_TRUSTONIC_TEE_SUPPORT
int part_is_TEE(part_t *part)
{
    int isTEE = 0;

    if (0 == strcmp(part->name, PART_TEE1))
	isTEE = 1;

    if (0 == strcmp(part->name, PART_TEE2))
	isTEE = 1;

    return isTEE;
}
#endif

int part_load(blkdev_t *bdev, part_t *part, u32 *addr, u32 offset, u32* size)
{
    int ret;
    img_hdr_t *hdr = img_hdr_buf;
    part_hdr_t *part_hdr = &hdr->part_hdr;
    gfh_file_info_t *file_info_hdr = &hdr->file_info_hdr;
    unsigned int sec_policy_idx = 0;
    unsigned int img_auth_required = 0;
    unsigned int sec_feature_mask = 0;

    /* specify the read offset */
    u64 src = part->startblk * bdev->blksz + offset;
    u32 dsize = 0, maddr = 0, mode =0;
    u32 ms;
	
#ifdef MTK_SECURITY_SW_SUPPORT
    /* get security policy of current partition */
    print("[%s] partition name = %s\n", MOD, g_cur_part_name);
    sec_policy_idx = get_policy_entry_idx(g_cur_part_name);
    img_auth_required = get_vfy_policy(sec_policy_idx);
    print("[%s] img_auth_required = %x\n", MOD, img_auth_required);
#endif
	
	//print("[bean] part->startblk(0x%x) bdev->blksz(0x%x) part->part_id(%d) hdr(0x%x)\n", part->startblk,
	//	bdev->blksz, part->part_id, hdr);

    /* retrieve partition header. */
    if (blkdev_read(bdev, src, sizeof(img_hdr_t), (u8*)hdr, part->part_id) != 0) {
        print("[%s] bdev(%d) read error (%s)\n", MOD, bdev->type, part->name);
        return -1;
    }
    
    if (part_hdr->info.magic == PART_MAGIC) {

        /* load image with partition header */
        part_hdr->info.name[31] = '\0';

        print("[%s] Image with part header\n", MOD);
        print("[%s] name : %s\n", MOD, part_hdr->info.name);
        print("[%s] addr : %xh\n", MOD, part_hdr->info.maddr);
        print("[%s] size : %d\n", MOD, part_hdr->info.dsize);
        print("[%s] magic: %xh\n", MOD, part_hdr->info.magic);
    
        maddr = part_hdr->info.maddr;
        dsize = part_hdr->info.dsize;
        mode = part_hdr->info.mode;
        src += sizeof(part_hdr_t);
	
	memcpy(part_info + part_num, part_hdr, sizeof(part_hdr_t));
	part_num++;
    } else {
        print("[%s] %s image doesn't exist\n", MOD, part->name);
        return -1;
    }

#ifdef MTK_SECURITY_SW_SUPPORT
    ms = get_timer(0);
    if (img_auth_required) {
        sec_malloc_buf_reset();
        #ifdef MTK_SECURITY_ANTI_ROLLBACK
        sec_feature_mask |= SEC_FEATURE_MASK_ANTIROLLBACK;
        #endif
        ret = sec_img_auth_init(g_cur_part_name, part_hdr->info.name, sec_feature_mask);
        if (ret) {
            print("[%s] cert chain vfy fail...\n", MOD);
            ASSERT(0);
        }
        /*add anti roll back hook*/
        #ifdef MTK_SECURITY_ANTI_ROLLBACK
        ret = sec_rollback_check(0);
        if (ret) {
            print("[%s] img ver check fail...\n", MOD);
            ASSERT(0);
        }
        #endif
    }
    ms = get_timer(ms);
    print("[%s] part: %s img: %s cert vfy(%d ms)\n", MOD, g_cur_part_name, part_hdr->info.name, ms);
#endif

    if (maddr == PART_HEADER_MEMADDR) {
        maddr = *addr;
	}
#if CFG_ATF_SUPPORT
	else if (mode == LOAD_ADDR_MODE_BACKWARD) {
			maddr = tee_get_load_addr(maddr);
	}
#endif

    ms = get_timer(0);
    if (0 == (ret = blkdev_read(bdev, src, dsize, (u8*)maddr, part->part_id))){
        if (addr) *addr = maddr;
        if (size) *size = dsize;
	}
    ms = get_timer(ms);

    print("\n[%s] load \"%s\" from 0x%llx (dev) to 0x%x (mem) [%s]\n", MOD, 
        part->name, src, maddr, (ret == 0) ? "SUCCESS" : "FAILED");

    if( ms == 0 )
        ms+=1;
    
    print("[%s] load speed: %dKB/s, %d bytes, %dms\n", MOD, ((dsize / ms) * 1000) / 1024, dsize, ms);

#ifdef MTK_SECURITY_SW_SUPPORT
    ms = get_timer(0);
    if (img_auth_required) {
        unsigned int cert_chain_sz = 0;
        unsigned int padding_sz = 0;
        print("[%s] img vfy...", MOD);
        ret = sec_img_auth(maddr, dsize);
        if (ret) {
            print("fail\n");
            ASSERT(0);
        }
        else {
            print("ok\n");
        }
    }
    ms = get_timer(ms);
    print("[%s] part: %s img: %s vfy(%d ms)\n", MOD, g_cur_part_name, part_hdr->info.name, ms);
	
    ret = sec_get_img_size(g_cur_part_name, part_hdr->info.name, size);
    if (ret)
        return ret;
#endif

    return ret;
}

int part_load_raw_part(blkdev_t *bdev, part_t *part, u32 *addr, u32 offset, u32 *size)
{
    int ret;
    img_hdr_t *hdr = (img_hdr_t *)img_hdr_buf;
    part_hdr_t *part_hdr = &hdr->part_hdr;
    gfh_file_info_t *file_info_hdr = &hdr->file_info_hdr;

    /* specify the read offset */
    u64 src = (u64)(part->startblk * bdev->blksz) + offset;
    u32 dsize = 0, maddr = 0;
    u32 ms;

    ret = 0;
    /* retrieve partition header. */
    if (blkdev_read(bdev, src, *size, (u8*)(*addr), part->part_id) != 0) {
        //print("[%s] bdev(%d) read error (%s)\n", MOD, bdev->type, part->name);
        ret = -1;
    }

    return ret;
}

/* 
    read data (not include the image header) from parttion
        input
             bdev      device information
             part      partition information
             addr      destination address in external memory
             offset    partition offset
             pu32_size ptr to request data size (not count image header size)
        output 
             pu32_size ptr actual read data size (not count image header size)
*/
int read_data_from_part(blkdev_t *bdev, part_t *part, u32 addr, u32 offset, u32* pu32_size)
{
    int ret;
    img_hdr_t *hdr = img_hdr_buf;
    part_hdr_t *part_hdr = &hdr->part_hdr;
    gfh_file_info_t *file_info_hdr = &hdr->file_info_hdr;

    /* specify the read offset */
    u64 src = part->startblk * bdev->blksz + offset;
    u32 dsize = 0, maddr = 0;
    u32 ms;

    if( NULL == pu32_size )
    {
        print("[%s] NULL pointer for size\n", MOD);
        return -1;
    }

    if( 0 == *pu32_size )
    {
        print("[%s] zero byte is read\n", MOD);
        return 0;
    }    
  
    /* retrieve partition header. */
    if (blkdev_read(bdev, src, sizeof(img_hdr_t), (u8*)hdr, part->part_id) != 0) {
        print("[%s] bdev(%d) read error (%s)\n", MOD, bdev->type, part->name);
        return -1;
    }
    
    if (part_hdr->info.magic == PART_MAGIC) {

        /* load image with partition header */
        part_hdr->info.name[31] = '\0';

        print("[%s] Image with part header\n", MOD);
        print("[%s] name : %s\n", MOD, part_hdr->info.name);
        print("[%s] addr : %xh\n", MOD, part_hdr->info.maddr);
        print("[%s] size : %d\n", MOD, part_hdr->info.dsize);
        print("[%s] magic: %xh\n", MOD, part_hdr->info.magic);
    
        maddr = part_hdr->info.maddr;
        dsize = part_hdr->info.dsize;
        src += sizeof(part_hdr_t);
	memcpy(part_info + part_num, part_hdr, sizeof(part_hdr_t));
	part_num++;
    } else {
        print("[%s] <ASSERT> %s image doesn't exist\n", MOD, part->name);
        return -1;
    }

    if( dsize > *pu32_size )
    {
        dsize = *pu32_size;
    }

    maddr = addr;

    ms = get_timer(0);
    ret = blkdev_read(bdev, src, dsize, (u8*)maddr, part->part_id);
    ms = get_timer(ms);

    print("\n[%s] load \"%s\" from 0x%llx (dev) to 0x%x (mem) [%s]\n", MOD, 
        part->name, src, maddr, (ret == 0) ? "SUCCESS" : "FAILED");

    if( ms == 0 )
        ms+=1;
    
    print("[%s] load speed: %dKB/s, %d bytes, %dms\n", MOD, ((dsize / ms) * 1000) / 1024, dsize, ms);

    *pu32_size = dsize;
    return ret;
}

int get_part_info(u8 *buf, u32 *maddr, u32 *dsize, u32 *mode, u8 bMsg)
{
    int ret = 0;
    part_hdr_t *part_hdr = buf;

    if (part_hdr->info.magic == PART_MAGIC) {

        /* load image with partition header */
        part_hdr->info.name[31] = '\0';

        if (bMsg) {
            print("[%s] partition hdr (1)\n", MOD);
            print("[%s] Image with part header\n", MOD);
            print("[%s] name : %s\n", MOD, part_hdr->info.name);
            print("[%s] addr : %xh mode : %d\n", MOD, part_hdr->info.maddr, part_hdr->info.mode);
            print("[%s] size : %d\n", MOD, part_hdr->info.dsize);
            print("[%s] magic: %xh\n", MOD, part_hdr->info.magic);
        }
    
        *maddr = part_hdr->info.maddr;
        *dsize = part_hdr->info.dsize;
        *mode = part_hdr->info.mode;
	
    } else {
        if (bMsg) {
            print("[%s] partition hdr (0)\n", MOD);
        }
        return -1;
    }

    return ret;
}

void part_dump(void)
{
    blkdev_t *bdev;
    part_t *part;
    u32 blksz;
    u64 start, end;

    bdev = blkdev_get(CFG_BOOT_DEV);
    part = cust_part_tbl();
    blksz = bdev->blksz;

    print("\n[%s] blksz: %dB\n", MOD, blksz);
    while (part->name) {
        start = (u64)part->startblk * blksz;
        end = (u64)(part->startblk + part->blks) * blksz - 1;
        print("[%s] [0x%llx-0x%llx] \"%s\" (%d blocks) \n", MOD, start, end, 
            part->name, part->blks); 
        part++;
    }
}
