/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2011
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#ifndef TRUSTZONE_H
#define TRUSTZONE_H

#include "dram_buffer.h"
#include "typedefs.h"

#if CFG_BOOT_ARGUMENT
#define bootarg g_dram_buf->bootarg
#endif

#define ATF_BOOTCFG_MAGIC (0x4D415446) // String MATF in little-endian
#define DEVINFO_SIZE 4
typedef struct {
    u32 atf_magic;
    u32 tee_support;
    u32 tee_entry;
    u64 tee_boot_arg_addr;
    u32 hwuid[4];     // HW Unique id for t-base used
    u32 atf_hrid_size; // Check this atf_hrid_size to read from HRID array
    u32 HRID[8];      // HW random id for t-base used
    u32 atf_log_port;
    u32 atf_log_baudrate;
    u64 atf_log_buf_start;
    u32 atf_log_buf_size;
    u32 atf_irq_num;
    u32 devinfo[DEVINFO_SIZE];
    u64 atf_aee_debug_buf_start;
    u32 atf_aee_debug_buf_size;
    u32 msg_fde_key[4]; /* size of message auth key is 16bytes(128 bits) */
#if CFG_TEE_SUPPORT
    u32 tee_rpmb_size;
#endif
} atf_arg_t, *atf_arg_t_ptr;

/**************************************************************************
 * EXPORTED FUNCTIONS
 **************************************************************************/
void tee_get_secmem_start(u64 *addr);
void tee_get_secmem_size(u32 *size);
void tee_set_entry(u32 addr);
void tee_set_hwuid(u8 *id, u32 size);
int tee_get_hwuid(u8 *id, u32 size);
int  tee_verify_image(u32 *addr);
u32 tee_get_load_addr(u32 maddr);
void trustzone_pre_init(void);
void trustzone_post_init(void);
void trustzone_jump(u64 addr, u32 arg1, u32 arg2);

/**************************************************************************
 * TEE FUNCTIONS
 **************************************************************************/
#define TEE_RESERVED_NAME "tee-reserved"
#define DEVICE_NAME_MAX_LEN 12

typedef struct tee_vendor_device {
    u8 name[DEVICE_NAME_MAX_LEN];
    u32 (*get_reserved_tee_address)(u32 tee_size);
    u32 (*get_reserved_secmem_address)(u32 tee_enty, u32 tee_size);
    void (*set_boot_param)(u32 param_addr, u32 tee_entry,
            u32 tee_size, u64 dram_base, u64 dram_size);
    void (*set_key_param)(u32 param_addr,u8 *hwuid,
            u32 hwid_size,u8 *hrid, u32 hrid_size, u8 * rpmb_key);
} tee_vendor_device_t;

extern tee_vendor_device_t tee_vendor_module;
#define DECLARE_TEE_MODULE(drv_name, tee_addr, secmem_addr, boot_param, key_param) \
    tee_vendor_device_t tee_vendor_module = { \
        .name = drv_name, \
        .get_reserved_tee_address = tee_addr, \
        .get_reserved_secmem_address = secmem_addr, \
        .set_boot_param = boot_param, \
        .set_key_param = key_param, \
    }

#endif /* TRUSTZONE_H */
