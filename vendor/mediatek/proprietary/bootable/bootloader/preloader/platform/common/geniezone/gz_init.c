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
 * MediaTek Inc. (C) 2016. All rights reserved.
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
#include "download.h"
#include "meta.h"
#include "sec.h"
#include "partition_api.h"
#include "dram_buffer.h"
#include "wdt.h"
#include "emi_mpu_mt.h"
#if CFG_ATF_SUPPORT
#include "tz_init.h"
#endif

#include <partition_active.h>
#include <gz_init.h>
#include <gz_atag.h>
#if CFG_GZ_REMAP
#include <gz_remap.h>
#endif
#include <gz_remap_mt.h>
#include "mt_rtc_hw.h"
#if CFG_GZ_PWRAP_ENABLE
#include "pmic_wrap_init.h"
#include "reg_PMIC_WRAP.h"
#endif
#include "log_store_pl.h"

/*============================================================================*/
/* DEBUG MACROS                                                               */
/*============================================================================*/
#define GZ_DEBUG
#ifdef GZ_DEBUG
#define DBG_MSG(str, ...) do {pal_log_debug(str, ##__VA_ARGS__);} while(0)
#define DBG_INFO(str, ...) do {pal_log_info(str, ##__VA_ARGS__);} while(0)
#define DBG_ERR(str, ...) do {pal_log_err(str, ##__VA_ARGS__);} while(0)
#else
#define DBG_MSG(str, ...) do {} while(0)
#define DBG_INFO(str, ...) do {pal_log_info(str, ##__VA_ARGS__);} while(0)
#define DBG_ERR(str, ...) do {pal_log_err(str, ##__VA_ARGS__);} while(0)
#endif

/*============================================================================*/
/* CONSTAND DEFINITIONS                                                       */
/*============================================================================*/
#define MOD "GZINIT"
#define MBLOCK_RELEASE mblock_create

/*============================================================================*/
/* GLOBAL VARIABLES                                                           */
/*============================================================================*/
#define bootarg g_dram_buf->bootarg
extern unsigned int g_uart;

/*============================================================================*/
/* INTERNAL VARIABLES                                                         */
/*============================================================================*/
u32 gz_start_addr = 0;
u32 gz_md_shm_addr = 0;
u32 gz_md_shm_size = 0;
u32 build_variant = 0; /* 1:user; 2:userdebug; 3:eng */

#if CFG_GZ_SECURE_DSP
u64 gz_sdsp1_fw_addr = 0;
u32 gz_sdsp1_fw_size = 0;
u64 gz_sdsp2_fw_addr = 0;
u32 gz_sdsp2_fw_size = 0;
#endif

#if CFG_GZ_SAPU_MTEE_SHM
u64 sapu_mtee_shm_addr = 0;
u32 sapu_mtee_shm_size = 0;
#endif

#if CFG_GZ_TEE_STATIC_SHM
u64 gz_tee_static_shm_pa = 0;
u32 gz_tee_static_shm_sz = 0;
static u32 gz_tee_static_shm_entry_cnt = 0;
static u64 gz_tee_static_shm_current_pa = 0;
#endif

/* 17 PAGES (for logging before Linux) + 33 PAGES (for logging after Linux)*/
#define GZ_LOG_BUFFER_SIZE (50 * 4096)
u32 gz_log_addr = 0;

/*============================================================================*/
/* ENUMERATIONS                                                               */
/*============================================================================*/
typedef enum {
    GZ_OK = 0,
    GZ_PART_LOAD_FAIL = 1,
    GZ_PART_INVALID_ADDR = 2,
    GZ_PART_INVALID_SIZE = 3,
    GZ_PART_DECRYPT_FAIL = 4,
    GZ_PART_NOT_FOUND = 5,
    GZ_PART_HEADER_NOT_FOUND = 6,
    GZ_BOOT_DISABLE = 7,
    GZ_BOOT_DEV_NOT_FOUND = 8,
    GZ_RESERVE_MEM_FAIL = 9,
    GZ_GET_ACTIVE_PART_FAIL = 10,
} E_GZ_FUNC_RET;

static u32 get_boot_configs(void)
{
    u32 val = 0;

    if (!is_booting_el2())
        val |= EL2_BOOT_DISABLE;

#if CFG_GZ_REMAP
    val |= EL2_REMAP_ENABLE;
#endif

    DBG_INFO("[%s] GZ CONFIGS = 0x%x\n", MOD, val);
    return val;
}

static u32 get_build_variant(void)
{
    /* default treat it as user load */
    u32 variant = 1;

#ifdef TARGET_BUILD_VARIANT_USER
    variant = 1;
#endif

#ifdef TARGET_BUILD_VARIANT_USERDEBUG
    variant = 2;
#endif

#ifdef TARGET_BUILD_VARIANT_ENG
    variant = 3;
#endif

    return variant;
}

static u32 get_remap_domain(void)
{
#if CFG_GZ_REMAP
    return GZ_REMAP_VMDOMAIN_GZ;
#else
    return 0x0;
#endif
}

static u64 get_ddr_remap_offset(void)
{
#if CFG_GZ_REMAP
    u64 offset = gz_remap_ddr_get_offset(GZ_REMAP_VMID_GZ);
    return (offset & 0x8000000000000000ULL) ? 0x0LL : offset;
#else
    return 0x0LL;
#endif
}

static u64 get_io_remap_offset(void)
{
#if CFG_GZ_REMAP
    u64 offset = gz_remap_io_get_offset(GZ_REMAP_VMID_GZ);
    return (offset & 0x8000000000000000ULL) ? 0x0LL : offset;
#else
    return 0x0LL;
#endif
}

static u64 get_sec_io_remap_offset(void)
{
#if CFG_GZ_REMAP
    u64 offset = gz_remap_sec_io_get_offset(GZ_REMAP_VMID_GZ);
    return (offset & 0x8000000000000000ULL) ? 0x0LL : offset;
#else
    return 0x0LL;
#endif
}

static u32 get_load_offset_without_remap(void)
{
    u32 gz_load_offset = (gz_start_addr - GZ_KERNEL_LOAD_OFFSET);

    DBG_INFO("[%s] GZ exec load offset: 0x%x\n", MOD, gz_load_offset);
    return gz_load_offset;
}

/* Vendor customization.
 * 1: disable gz booting
 * 0: enable gz booting
 */
static int gz_booting_disable_cfg(void)
{
    /* default is to enable EL2 booting */
    return 0;
}

static void gz_set_boot_disabled(void)
{
    gz_start_addr = EL2_BOOTING_DISABLED;
    gz_md_shm_addr = 0;
    gz_md_shm_size = 0;
#if CFG_GZ_SECURE_DSP
    gz_sdsp1_fw_addr = 0;
    gz_sdsp1_fw_size = 0;
    gz_sdsp2_fw_addr = 0;
    gz_sdsp2_fw_size = 0;
#endif

#if CFG_GZ_SAPU_MTEE_SHM
    sapu_mtee_shm_addr = 0;
    sapu_mtee_shm_size = 0;
#endif

#if CFG_GZ_TEE_STATIC_SHM
    gz_tee_static_shm_pa = 0;
    gz_tee_static_shm_current_pa = 0;
#endif
    gz_log_addr = 0;
}

#define img_hdr_buf (g_dram_buf->img_hdr_buf)
#define CMP_EQ(a,b) (a==b)
static int gz_part_existence_check(void)
{
    ssize_t read_len;
    ssize_t try_len = sizeof(part_hdr_t);

    read_len = partition_read("gz", 0, (uint8_t *)img_hdr_buf, try_len);
    if (CMP_EQ(read_len, try_len))
        return GZ_OK;

    DBG_ERR("[%s] invalid 'gz' part length: %d\n", MOD, read_len);

    gz_set_boot_disabled();
    return GZ_PART_HEADER_NOT_FOUND;
}

static int gz_reserve_memory(void)
{
    gz_start_addr = ((u32)mblock_reserve_ext(&bootarg.mblock_info,
        GZ_DRAM_SIZE, GZ_ALIGNMENT, GZ_ADDR_MAX, 0, "gz"));
    if(!gz_start_addr) {
        return GZ_RESERVE_MEM_FAIL;
    }

    DBG_INFO("[%s] allocate gz ram 0x%x\n", MOD, gz_start_addr);
    return GZ_OK;
}

static int gz_md_shm_reserve(u32 *reserved_addr, u32 *reserved_size)
{
    u32 start_addr = 0;

    start_addr = mblock_reserve_ext(&bootarg.mblock_info, GZ_MD_SHM_SIZE,
        GZ_MD_SHM_ALIGNMENT, GZ_MD_SHM_ADDR_MAX, 0, "gz-md-shm");
    if (!start_addr) {
        return GZ_RESERVE_MEM_FAIL;
    }

    *reserved_addr = start_addr;
    *reserved_size = GZ_MD_SHM_SIZE;

    DBG_INFO("[%s] allocate gz-md shared memory 0x%x\n", MOD, start_addr);
    return GZ_OK;
}

#if CFG_GZ_TEE_STATIC_SHM
static int gz_tee_static_shm_reserve(u64 *reserved_addr, u32 *reserved_size)
{
    u32 start_addr = 0;

    start_addr = mblock_reserve_ext(&bootarg.mblock_info, GZ_TEE_STATIC_SHM_SIZE,
        GZ_TEE_STATIC_SHM_ALIGNMENT, GZ_TEE_STATIC_SHM_ADDR_MAX, 0, "gz-tee-static-shm");
    if (!start_addr) {
        return GZ_RESERVE_MEM_FAIL;
    }

    *reserved_addr = start_addr;
    *reserved_size = GZ_TEE_STATIC_SHM_SIZE;

    gz_tee_static_shm_current_pa = start_addr + GZ_TEE_STATIC_SHM_HEADER_SIZE;
    memset((void *)start_addr, 0x0, GZ_TEE_STATIC_SHM_HEADER_SIZE);	// just clear for header

    DBG_INFO("[%s] allocate gz-tee shared memory 0x%x\n", MOD, start_addr);
    return GZ_OK;
}

static int gz_alloc_gz_tee_static_shm(const char *name, u32 sz)
{
    gz_tee_static_shm_t *ptr;
    u32 sz_page_align = (sz+GZ_PAGE_SIZE-1)&(~(GZ_PAGE_SIZE-1));

    if (!gz_tee_static_shm_current_pa) return -1;
    if ( (gz_tee_static_shm_current_pa+sz_page_align) > (gz_tee_static_shm_pa+gz_tee_static_shm_sz) )
        return -1;
    if ( strlen(name)>=MAX_GZ_TEE_STATIC_SHM_NAME ) return -1;
    DBG_INFO("%s %s %u\n", __func__, name, strlen(name));

    ptr = (gz_tee_static_shm_t *)((u32)gz_tee_static_shm_pa+sizeof(gz_tee_static_shm_t)*gz_tee_static_shm_entry_cnt);
    if ( gz_tee_static_shm_entry_cnt < MAX_GZ_TEE_STATIC_SHM_ENTRY ) {
        ptr->magic = MAGIC_NUMBER;
        sz_page_align = (sz+GZ_PAGE_SIZE-1)&(~(GZ_PAGE_SIZE-1));
        ptr->sz = sz_page_align;
        ptr->pa = gz_tee_static_shm_current_pa;
        memcpy(&ptr->name, name, strlen(name));
        gz_tee_static_shm_current_pa = gz_tee_static_shm_current_pa + sz_page_align;
        gz_tee_static_shm_entry_cnt++;
        return 0;
    }
    return -1;
}
#endif

#if CFG_GZ_SECURE_DSP
static int gz_sdsp_fw_reserve(u64 *reserved_addr, u32 *reserved_size, char index)
{
    u64 start_addr = 0;
    int tmp;
    char my_str[]="gz-sdsp?-fw";

    my_str[7] = index;

    start_addr = mblock_reserve_ext(&bootarg.mblock_info, GZ_SDSP_FW_SIZE,
        GZ_SDSP_FW_ALIGNMENT, GZ_SDSP_FW_ADDR_MAX, 0, my_str);
    if (!start_addr) {
        return GZ_RESERVE_MEM_FAIL;
    }

    *reserved_addr = start_addr;
    *reserved_size = GZ_SDSP_FW_SIZE;

    DBG_INFO("[%s] allocate %s 0x%x\n", MOD, my_str, start_addr);
    return GZ_OK;
}
#endif

#if CFG_GZ_SAPU_MTEE_SHM
static int sapu_mtee_shm_reserve(u64 *reserved_addr, u32 *reserved_size)
{
    u64 start_addr = 0;

    start_addr = mblock_reserve_ext(&bootarg.mblock_info, SAPU_MTEE_SHM_SIZE,
        SAPU_MTEE_SHM_ALIGNMENT, SAPU_MTEE_SHM_ADDR_MAX, 0, "sapu-mtee-shm");
    if (!start_addr) {
        return GZ_RESERVE_MEM_FAIL;
    }

    *reserved_addr = start_addr;
    *reserved_size = SAPU_MTEE_SHM_SIZE;

    DBG_INFO("[%s] allocate gz-sdsp-shm 0x%x\n", MOD, start_addr);
    return GZ_OK;
}
#endif

unsigned int is_booting_el2(void)
{
    if (gz_start_addr == EL2_BOOTING_DISABLED)
        return 0;

    return 1; /* booting el2 */
}

#if CFG_NEBULA_VM_SUPPORT
static inline u32 get_vm_load_addr(void)
{
    return gz_start_addr + (GZ_DRAM_SIZE - VM_DRAM_SIZE);
}

#if CFG_NEBULA_LOAD_IN_PART2
static int load_vm_from_part2(blkdev_t *bdev, const char *active_part_name)
{
    u32 offset = 0;
    int ret;
    u32 size;
    part_t *part;
    u32 vm_load_addr = get_vm_load_addr();

    DBG_INFO("[%s] load VM to addr=0x%x from part=%s (p2)\n", MOD,
             vm_load_addr, active_part_name);

    part = part_get((char *)active_part_name);
    if (!part) {
        DBG_ERR("[%s] load part %s failed!\n", MOD, active_part_name);
        return GZ_GET_ACTIVE_PART_FAIL;
    }

    ret = part_load(bdev, part, &vm_load_addr, offset, &size);
    if (ret) {
        DBG_ERR("[%s] load VM failed at part=%s, ret=%d (p2)\n", MOD,
                part->info->name, ret);
        return ret;
    }

#if CFG_GZ_NEED_DESCRAMBLE
    ret = descramble(vm_load_addr, size);
    if (ret) {
        DBG_ERR("%s descramble VM failed, ret=%d (p2)\n", MOD, ret);
        return ret;
    }
#endif

    return ret;
}
#endif

static int bldr_load_vm_part(blkdev_t *bdev, part_t *part, u32 part_offset)
{
    u32 size;
    int ret;
    u32 vm_load_addr = get_vm_load_addr();

#if CFG_NEBULA_LOAD_IN_PART2
    char gz2_part_name[PART_NAME_BUF_SZ] = {0};

    memset(gz2_part_name, 0x0, PART_NAME_BUF_SZ);
    memcpy(gz2_part_name, "gz", strlen("gz"));

    ret = partition_get_active(gz2_part_name, PART_NAME_BUF_SZ, GET_INACTIVE);
    if (ret) {
        DBG_ERR("[%s] get inactive part name failed!\n", MOD);
        return GZ_PART_LOAD_FAIL;
    }

    return load_vm_from_part2(bdev, gz2_part_name);
#endif

    DBG_INFO("[%s] load VM to addr=0x%x (p1)\n", MOD, vm_load_addr);

    ret = part_load(bdev, part, &vm_load_addr, part_offset, &size);
    if (ret) {
        DBG_ERR("[%s] load VM failed at part=%s, ret=%d (p1)\n", MOD,
                part->info->name);
        return ret;
    }

#if CFG_GZ_NEED_DESCRAMBLE
    ret = descramble(vm_load_addr, size);
    if (ret) {
        DBG_ERR("[%s] descramble VM failed, ret=%d (p1)\n", MOD, ret);
        return ret;
    }
#endif
    return GZ_OK;
}
#endif

int bldr_load_gz_part(blkdev_t *bdev, const char *part_name)
{
    static u32 offset = 0;
    u32 size;
    int ret;
    part_t *part = part_get((char *)part_name);
    u32 addr = gz_get_load_addr(0);

    if (!is_booting_el2()) {
        DBG_INFO("[%s] EL2_BOOTING_DISABLED, skip load gz %s\n", MOD, __func__);
        return GZ_OK;
    }

    if (!part) {
        DBG_ERR("[%s] get part %s failed!\n", MOD, part_name);
        return GZ_PART_LOAD_FAIL;
    }

    ret = part_load(bdev, part, &addr, offset, &size);
    if (ret) {
        DBG_ERR("[%s] %s part. ATF load fail\n", MOD, part_name);
        return GZ_PART_LOAD_FAIL;
    }
    /* check if target address defined by image is the same as mblock reserved address */
    if (addr != gz_start_addr) {
            DBG_ERR("[%s] %s part. error: target addr = 0x%x but reserved addr = 0x%x\n",
            MOD, part_name, addr, gz_start_addr);
        return GZ_PART_INVALID_ADDR;
    }
    /* check if the size of GZ image loaded exceeds the reserved size */
    if (size > GZ_DRAM_SIZE) {
        DBG_ERR("[%s] %s part. error: image size = 0x%x but reserved size = 0x%x\n",
            MOD, part_name, size, GZ_DRAM_SIZE);
        return GZ_PART_INVALID_SIZE;
    }

#if CFG_GZ_NEED_DESCRAMBLE
    ret = descramble(gz_start_addr, size);
    if (ret) {
        DBG_ERR("[%s] error descramble: ret = %d\n", MOD, ret);
        return GZ_PART_DECRYPT_FAIL;
    }
#endif

#if CFG_NEBULA_VM_SUPPORT
    return bldr_load_vm_part(bdev, part, (sizeof(part_hdr_t) + size));
#else
    return GZ_OK;
#endif
}

u32 gz_config_info_atag(boot_tag *tags)
{
    if (!is_booting_el2())
        return tags;

    tags->hdr.size = boot_tag_size(boot_tag_gz_info);
    tags->hdr.tag  = BOOT_TAG_GZ_INFO;
    tags->u.gz_info.gz_configs = get_boot_configs();
    tags->u.gz_info.lk_addr = CFG_UBOOT_MEMADDR;
    tags->u.gz_info.build_variant= build_variant;

    return boot_tag_next(tags);
}

u32 gz_config_boot_atag(boot_tag *tags)
{
    blkdev_t *bootdev;
    u32 rpmb_total_size;

    int ret;

    if (!is_booting_el2())
        return tags;

    tags->hdr.size = boot_tag_size(boot_tag_gz_param);
    tags->hdr.tag  = BOOT_TAG_GZ_PARAM;
    tags->u.gz_param.modemMteeShareMemPA = gz_md_shm_addr;
    tags->u.gz_param.modemMteeShareMemSize = gz_md_shm_size;

    ret = tee_get_hwuid(tags->u.gz_param.hwuid, ME_IDENTITY_LEN);
    if (ret != 0) {
        DBG_ERR("[%s] gz hwuid fail, ret=%d\n", MOD, ret);
    }
#if 0
    pal_log_info("gz hwuid done\n");
    pal_log_info("0x%x 0x%x 0x%x 0x%x\n",
        tags->u.gz_param.hwuid[0], tags->u.gz_param.hwuid[1],
        tags->u.gz_param.hwuid[2], tags->u.gz_param.hwuid[3]);
    pal_log_info("0x%x 0x%x 0x%x 0x%x\n",
        tags->u.gz_param.hwuid[4], tags->u.gz_param.hwuid[5],
        tags->u.gz_param.hwuid[6], tags->u.gz_param.hwuid[7]);
    pal_log_info("0x%x 0x%x 0x%x 0x%x\n",
        tags->u.gz_param.hwuid[8], tags->u.gz_param.hwuid[9],
        tags->u.gz_param.hwuid[10], tags->u.gz_param.hwuid[11]);
    pal_log_info("0x%x 0x%x 0x%x 0x%x\n",
        tags->u.gz_param.hwuid[12], tags->u.gz_param.hwuid[13],
        tags->u.gz_param.hwuid[14], tags->u.gz_param.hwuid[15]);
#endif

    ret = rpmb_get_key(tags->u.gz_param.rpmb_key, RPMB_KEY_SIZE);
    if (ret != 0) {
        DBG_ERR("[%s] gz rpmb_key fail, ret=%d\n", MOD, ret);
    }

#if !defined(BOOTDEV_SDMMC_UFS_COMBO)
#if (CFG_BOOT_DEV == BOOTDEV_UFS)
    rpmb_total_size = (u32)ufs_rpmb_get_lu_size();
#elif (CFG_BOOT_DEV == BOOTDEV_SDMMC)
    rpmb_total_size = (u32)mmc_rpmb_get_size();
#endif /* CFG_BOOT_DEV */
#else
#if (CFG_BOOT_DEV == BOOTDEV_SDMMC) || (CFG_BOOT_DEV == BOOTDEV_UFS)
    bootdev = blkdev_get(CFG_BOOT_DEV);
    rpmb_total_size =
        (bootdev == NULL) ? 0 :
        (bootdev->type == BOOTDEV_UFS) ? ufs_rpmb_get_lu_size() :
        (bootdev->type == BOOTDEV_SDMMC) ? mmc_rpmb_get_size() : 0;
#endif /* CFG_BOOT_DEV */
#endif /* BOOTDEV_SDMMC_UFS_COMBO */
    /* By-default the bottom part of RPMB is reserved for GZ */
    if (rpmb_total_size == 0 || rpmb_total_size < GZ_RPMB_SIZE) {
        DBG_ERR("[%s] gz insufficient RPMB size, need %u but total=%u\n",
                GZ_RPMB_SIZE, rpmb_total_size);
    } else {
        tags->u.gz_param.rpmb_base = rpmb_total_size - GZ_RPMB_SIZE;
        tags->u.gz_param.rpmb_size = GZ_RPMB_SIZE;
    }
#if 0
    pal_log_info("gz rpmb_key done(partial value)\n");
    pal_log_info("0x%x 0x%x 0x%x 0x%x\n",
        tags->u.gz_param.rpmb_key[0], tags->u.gz_param.rpmb_key[1],
        tags->u.gz_param.rpmb_key[2], tags->u.gz_param.rpmb_key[3]);
    pal_log_info("0x%x 0x%x 0x%x 0x%x\n",
        tags->u.gz_param.rpmb_key[4], tags->u.gz_param.rpmb_key[5],
        tags->u.gz_param.rpmb_key[6], tags->u.gz_param.rpmb_key[7]);
    pal_log_info("0x%x 0x%x 0x%x 0x%x\n",
        tags->u.gz_param.rpmb_key[8], tags->u.gz_param.rpmb_key[9],
        tags->u.gz_param.rpmb_key[10], tags->u.gz_param.rpmb_key[11]);
    pal_log_info("0x%x 0x%x 0x%x 0x%x\n",
        tags->u.gz_param.rpmb_key[12], tags->u.gz_param.rpmb_key[13],
        tags->u.gz_param.rpmb_key[14], tags->u.gz_param.rpmb_key[15]);
#endif

    return boot_tag_next(tags);
}

u32 gz_config_platform_atag(boot_tag *tags)
{
    int ret;

    COMPILE_ASSERT(GZ_PLAT_TAG_SIZE == sizeof(struct boot_tag_gz_platform));

    if (!is_booting_el2())
        return tags;

    tags->hdr.size = boot_tag_size(boot_tag_gz_platform);
    tags->hdr.tag  = BOOT_TAG_GZ_PLAT;

    tags->u.gz_plat.reg_base.uart = g_uart;

    /* Switch log port to UART2 while uart meta connected */
    if (g_boot_mode == META_BOOT && g_meta_com_type == META_UART_COM)
        tags->u.gz_plat.reg_base.uart = UART2;

    tags->u.gz_plat.reg_base.cpuxgpt = CPUXGPT_BASE;
    tags->u.gz_plat.reg_base.gicd = GICD_BASE;
    tags->u.gz_plat.reg_base.gicr = GICR_BASE;
#if CFG_GZ_PWRAP_ENABLE && defined(PWRAP_BASE)
    tags->u.gz_plat.reg_base.pwrap = PWRAP_BASE;
#else
    tags->u.gz_plat.reg_base.pwrap = 0x0;
#endif
    tags->u.gz_plat.reg_base.rtc = RTC_SEC_BASE;
    tags->u.gz_plat.reg_base.mcucfg = MCUCFG_BASE;
#if CFG_NEBULA_VM_SUPPORT
    tags->u.gz_plat.reserve_mem_size = (GZ_DRAM_SIZE - VM_DRAM_SIZE);
    tags->u.gz_plat.vm_mem_size = VM_DRAM_SIZE;
#else
    tags->u.gz_plat.reserve_mem_size = GZ_DRAM_SIZE;
    tags->u.gz_plat.vm_mem_size = 0x0;
#endif
    tags->u.gz_plat.remap.offset_ddr = get_ddr_remap_offset();
    tags->u.gz_plat.remap.offset_io = get_io_remap_offset();
    tags->u.gz_plat.remap.offset_sec_io = get_sec_io_remap_offset();
    tags->u.gz_plat.exec_start_offset = get_ddr_remap_offset();
    tags->u.gz_plat.exec_start_offset += get_load_offset_without_remap();
    tags->u.gz_plat.flags = 0x0LL;
    tags->u.gz_plat.flags |= GZ_PLAT_FLAGS_ERASE_ATAG;
#if CFG_GZ_REMAP
    tags->u.gz_plat.flags |= GZ_PLAT_FLAGS_REMAP_ENABLE;
#endif
    tags->u.gz_plat.flags |= GZ_PLAT_FLAGS_GIC_V3;
#if CFG_GZ_PWRAP_ENABLE
    tags->u.gz_plat.flags |= GZ_PLAT_FLAGS_PWRAP_EN;
#endif
    tags->u.gz_plat.plat_ver.hw_code = (u16)DRV_Reg32(APHW_CODE);
    tags->u.gz_plat.plat_ver.hw_sub_code = (u16)DRV_Reg32(APHW_SUBCODE);

    tags->u.gz_plat.dram_size_1mb_cnt = (u32)(platform_memory_size() >> 20);
    tags->u.gz_plat.log_addr = gz_log_addr;
    tags->u.gz_plat.log_size = (gz_log_addr ? GZ_LOG_BUFFER_SIZE : 0);

    DBG_INFO("[%s] gz uart: 0x%x\n", MOD, tags->u.gz_plat.reg_base.uart);
    DBG_INFO("[%s] gz platform flags: 0x%llx\n", MOD, tags->u.gz_plat.flags);
    return boot_tag_next(tags);
}

#if CFG_GZ_PWRAP_ENABLE
u32 gz_config_pwrap_atag(boot_tag *tags)
{
    int ret;
    unsigned int x;

    COMPILE_ASSERT(GZ_PWRAP_TAG_SIZE == sizeof(struct boot_tag_gz_pwrap));

    if (!is_booting_el2())
        return tags;

    tags->hdr.size = boot_tag_size(boot_tag_gz_pwrap);
    tags->hdr.tag  = BOOT_TAG_GZ_PWRAP;

    tags->u.gz_pwrap.wacs1_init_done = PMIC_WRAP_INIT_DONE1;
    tags->u.gz_pwrap.wacs1_cmd = PMIC_WRAP_WACS1_CMD;
    tags->u.gz_pwrap.wacs1_rdata = PMIC_WRAP_WACS1_RDATA;
    tags->u.gz_pwrap.wacs1_vldclr = PMIC_WRAP_WACS1_VLDCLR;
    tags->u.gz_pwrap.get_wacs1_init_done1_shift = GET_WACS1_INIT_DONE1_SHIFT;
    tags->u.gz_pwrap.get_sys_idle1_shift = GET_SYS_IDLE1_SHIFT;

    DBG_INFO("[%s] pwrap wacs1_init_done: 0x%x\n", MOD, tags->u.gz_pwrap.wacs1_init_done);
    DBG_INFO("[%s] pwrap wacs1_cmd: 0x%x\n", MOD, tags->u.gz_pwrap.wacs1_cmd);
    DBG_INFO("[%s] pwrap wacs1_rdata: 0x%x\n", MOD, tags->u.gz_pwrap.wacs1_rdata);
    DBG_INFO("[%s] pwrap wacs1_vldclr: 0x%x\n", MOD, tags->u.gz_pwrap.wacs1_vldclr);
    DBG_INFO("[%s] pwrap get_wacs1_init_done1_shift: %d\n", MOD, tags->u.gz_pwrap.get_wacs1_init_done1_shift);
    DBG_INFO("[%s] pwrap get_sys_idle1_shift: %d\n", MOD, tags->u.gz_pwrap.get_sys_idle1_shift);
    return boot_tag_next(tags);
}
#endif

void gz_pre_init()
{
    int ret;

    build_variant = get_build_variant();

    ret = gz_part_existence_check();
    if (ret) {
        DBG_ERR("[%s] gz part check failed: %d!\n", MOD, ret);
        return;
    }

    ret = gz_reserve_memory();
    if (ret) {
        DBG_ERR("[%s] GZ fatal error...\n", MOD);
        ASSERT(0);
    }

#if CFG_GZ_TEE_STATIC_SHM
    ret = gz_tee_static_shm_reserve(&gz_tee_static_shm_pa, &gz_tee_static_shm_sz);
    if (ret) {
        DBG_ERR("[%s] GZ TEE shm fatal error...\n", MOD);
        ASSERT(0);
    }
#endif

#if CFG_GZ_SECURE_DSP
    ret = gz_sdsp_fw_reserve(&gz_sdsp2_fw_addr, &gz_sdsp2_fw_size, 0x32);
    if (ret) {
        DBG_ERR("[%s] GZ DSP2 fw mmeory fatal error...\n", MOD);
        ASSERT(0);
    }
    if ( (gz_sdsp2_fw_addr&0x40000000) != ((gz_sdsp2_fw_addr+(u64)gz_sdsp2_fw_size)&0x40000000) ) {
        DBG_ERR("[%s] gz-sdsp2-fw range fatal error...\n", MOD);
        ASSERT(0);
    }
    ret = gz_sdsp_fw_reserve(&gz_sdsp1_fw_addr, &gz_sdsp1_fw_size, 0x31);
    if (ret) {
        DBG_ERR("[%s] GZ DSP1 fw mmeory fatal error...\n", MOD);
        ASSERT(0);
    }
    if ( (gz_sdsp1_fw_addr&0x40000000) != ((gz_sdsp1_fw_addr+(u64)gz_sdsp1_fw_size)&0x40000000) ) {
        DBG_ERR("[%s] gz-sdsp1-fw range fatal error...\n", MOD);
        ASSERT(0);
    }
    ASSERT(gz_sdsp2_fw_addr==(gz_sdsp1_fw_addr+gz_sdsp1_fw_size));
#endif

#if CFG_GZ_SAPU_MTEE_SHM
    ret = sapu_mtee_shm_reserve(&sapu_mtee_shm_addr, &sapu_mtee_shm_size);
    if (ret) {
        DBG_ERR("[%s] sapu-mtee-shm fatal error...\n", MOD);
        ASSERT(0);
    }
#endif

    gz_log_addr = mblock_reserve_ext(&bootarg.mblock_info, GZ_LOG_BUFFER_SIZE,
				GZ_ALIGNMENT, GZ_ADDR_MAX, 0, "gz-log");
    if (!gz_log_addr) {
        DBG_ERR("[%s] GZ log mmeory fatal error...\n", MOD);
        ASSERT(0);
    }

    ret = gz_md_shm_reserve(&gz_md_shm_addr, &gz_md_shm_size);
    if (ret) {
        DBG_ERR("[%s] GZ MD shared mmeory fatal error...\n", MOD);
        ASSERT(0);
    }
}

int gz_de_init(void)
{
    if (gz_booting_disable_cfg()) {
        int ret;
        DBG_INFO("[%s] skip load gz(%s)\n", MOD, __func__);
        if (gz_md_shm_addr!=0 && gz_md_shm_size!=0) {
            ret = MBLOCK_RELEASE(&bootarg.mblock_info, &bootarg.orig_dram_info, (u64)gz_md_shm_addr, (u64)gz_md_shm_size);
        }
        if (ret) {
            DBG_ERR("[%s] GZ MD release shared mmeory fatal error...\n", MOD);
            ASSERT(0);
        }

	if (gz_log_addr != 0) {
		ret = MBLOCK_RELEASE(&bootarg.mblock_info, &bootarg.orig_dram_info,
				(u64)gz_log_addr, (u64)GZ_LOG_BUFFER_SIZE);
		DBG_INFO("[%s] release gz-log addr:0x%x size:0x%x ret %d\n",
			MOD, gz_log_addr, GZ_LOG_BUFFER_SIZE, ret);
	}
	if (ret) {
            DBG_ERR("[%s] GZ log release shared mmeory fatal error...\n", MOD);
            ASSERT(0);
        }
#if CFG_GZ_SECURE_DSP
        if (gz_sdsp1_fw_addr!=0 && gz_sdsp1_fw_size!=0) {
            ret = MBLOCK_RELEASE(&bootarg.mblock_info, &bootarg.orig_dram_info, gz_sdsp1_fw_addr, (u64)gz_sdsp1_fw_size);
        }
        if (ret) {
            DBG_ERR("gz-sdsp1-fw release mmeory fatal error...\n", MOD);
            ASSERT(0);
        }
        if (gz_sdsp2_fw_addr!=0 && gz_sdsp2_fw_size!=0) {
            ret = MBLOCK_RELEASE(&bootarg.mblock_info, &bootarg.orig_dram_info, gz_sdsp2_fw_addr, (u64)gz_sdsp2_fw_size);
        }
        if (ret) {
            DBG_ERR("gz-sdsp2-fw release mmeory fatal error...\n", MOD);
            ASSERT(0);
        }
#endif

#if CFG_GZ_SAPU_MTEE_SHM
        if (sapu_mtee_shm_addr!=0 && sapu_mtee_shm_size!=0) {
            ret = MBLOCK_RELEASE(&bootarg.mblock_info, &bootarg.orig_dram_info, sapu_mtee_shm_addr, (u64)sapu_mtee_shm_size);
        }
        if (ret) {
            DBG_ERR("sapu-mtee-shm release fatal error...\n", MOD);
            ASSERT(0);
        }
#endif

#if CFG_GZ_TEE_STATIC_SHM
        if (gz_tee_static_shm_pa!=0 && gz_tee_static_shm_sz!=0) {
            ret = MBLOCK_RELEASE(&bootarg.mblock_info, &bootarg.orig_dram_info, gz_tee_static_shm_pa, (u64)gz_tee_static_shm_sz);
        }
        if (ret) {
            DBG_ERR("gz-tee-shm release fatal error...\n", MOD);
            ASSERT(0);
        }
#endif

        if (gz_start_addr!=0) {
            ret = MBLOCK_RELEASE(&bootarg.mblock_info, &bootarg.orig_dram_info, (u64)gz_start_addr, GZ_DRAM_SIZE);
        }
        if (ret) {
            DBG_ERR("[%s] GZ release fatal error...\n", MOD);
            ASSERT(0);
        }
        gz_set_boot_disabled();
        return 1;
    }
    return 0;
}

u32 gz_get_sdsp_mem_info(u32 *pa, u32 *size)
{
#if CFG_GZ_SECURE_DSP
    *pa = gz_sdsp1_fw_addr;
    *size = (gz_sdsp1_fw_size + gz_sdsp2_fw_size);
#else
    *pa = 0;
    *size = 0;
#endif
    return 0;
}

u32 gz_get_sapu_shm_info(u32 *pa, u32 *size)
{
#if CFG_GZ_SAPU_MTEE_SHM
    *pa = sapu_mtee_shm_addr;
    *size = sapu_mtee_shm_size;
#else
    *pa = 0;
    *size = 0;
#endif
    return 0;
}


u32 get_gz_tee_static_shm_info(u32 *pa, u32 *size)
{
#if CFG_GZ_TEE_STATIC_SHM
    *pa = gz_tee_static_shm_pa;
    *size = gz_tee_static_shm_sz;
#else
    *pa = 0;
    *size = 0;
#endif
    return 0;
}

void gz_post_init(void)
{
    int rc;
    if (!is_booting_el2())
        return;

#if CFG_GZ_REMAP
    if (gz_remap_init() != GZ_REMAP_SUCCESS) {
        DBG_ERR("[%s] GZ remap initialization fatal error ...\n", MOD);
        ASSERT(0);
    }
    gz_remap_dump_config();
#endif

#if CFG_GZ_TEE_STATIC_SHM
#ifdef GZ_TEE_STATIC_SHM_UT_TEST //UT disable
    rc = gz_alloc_gz_tee_static_shm("for-ut-test1", 4095);
    ASSERT(!rc);
    rc = gz_alloc_gz_tee_static_shm("for-ut-test2", 8191);
    ASSERT(!rc);
#endif
#endif
    ((struct sram_log_header *)SRAM_LOG_ADDR)->gz_log_addr = gz_log_addr;
    ((struct sram_log_header *)SRAM_LOG_ADDR)->gz_log_len = (gz_log_addr ? GZ_LOG_BUFFER_SIZE : 0);
}

u64 gz_get_jump_addr(void)
{
    u64 addr64;

    addr64 = gz_start_addr;
#if CFG_GZ_REMAP
    addr64 += get_ddr_remap_offset();
#endif

    return addr64;
}

u32 gz_get_load_addr(u32 maddr)
{
    DBG_INFO("[%s] GZ load start: 0x%x (maddr: 0x%x)\n", MOD, gz_start_addr,
        maddr);
    return gz_start_addr;
}
