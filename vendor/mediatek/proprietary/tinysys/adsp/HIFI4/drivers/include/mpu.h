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
 * MediaTek Inc. (C) 2019. All rights reserved.
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

#ifndef _H_MPU_H_
#define _H_MPU_H_

#include <stdint.h>
#include <xtensa/config/core.h>

/* MPU status definition */
#define MPU_SUCCESS              0
#define MPU_ENTRIES_EXCEED      -1
#define MPU_MAP_NOT_ALIGNED     -2
#define MPU_BAD_ACCESS_RIGHTS   -3
#define MPU_OUT_OF_ENTRIES      -4
#define MPU_OUT_OF_ORDER        -5
#define MPU_UNSUPPORTED         -6
#define MPU_UNKNOWN_ERROR       -7

/* MPU definition */
#define MAX_NUM_REGIONS         (XCHAL_MPU_ENTRIES)

#define MPU_ENTRY_SET_AS(x, vaddr, valid)	(x).as = \
	(((vaddr) & 0xffffffe0) | ((valid & 0x1)))
#define MPU_ENTRY_SET_AT(x, access, memtype)	(x).at = \
	(((XTHAL_ENCODE_MEMORY_TYPE(memtype)) << 12) | (((access) & 0xf) << 8))

/* ENABLE */
enum {
	MPU_ENTRY_DISABLE = 0UL,
	MPU_ENTRY_ENABLE = 1UL,
};

typedef struct {
	uint32_t vaddr;
	uint32_t size;
	uint32_t access_right;
	uint32_t memory_type;
	uint32_t enable;
} mpu_region_t;

int mpu_init(const mpu_region_t *mpu_regions, int num_regions);
void vRegisterMpuCommands( void );
void dump_mpu_status(void);

#endif /*_H_MPU_H_*/
