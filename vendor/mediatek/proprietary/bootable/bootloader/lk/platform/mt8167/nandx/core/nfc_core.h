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
#ifndef __NFC_CORE_H__
#define __NFC_CORE_H__

#include "nandx_util.h"
#include "nandx_info.h"
#include "nandx_device_info.h"

enum WAIT_TYPE {
	IRQ_WAIT_RB,
	POLL_WAIT_RB,
	POLL_WAIT_TWHR2,
};

/**
 * struct nfc_format - nand info from spec
 * @pagesize: nand page size
 * @oobsize: nand oob size
 * @ecc_strength: spec required ecc strength per 1KB
 */
struct nfc_format {
	u32 page_size;
	u32 oob_size;
	u32 ecc_strength;
};

struct nfc_handler {
	u32 sector_size;
	u32 spare_size;
	u32 fdm_size;
	u32 fdm_ecc_size;
	/* ecc strength per sector_size */
	u32 ecc_strength;
	void (*send_command)(struct nfc_handler *, u8);
	void (*send_address)(struct nfc_handler *, u32, u32, u32, u32);
	int (*write_page)(struct nfc_handler *, u8 *, u8 *);
	void (*write_byte)(struct nfc_handler *, u8);
	int (*read_sectors)(struct nfc_handler *, int, u8 *, u8 *);
	u8 (*read_byte)(struct nfc_handler *);
	int (*change_interface)(struct nfc_handler *, enum INTERFACE_TYPE,
	                        struct nand_timing *, void *);
	int (*change_mode)(struct nfc_handler *, enum OPS_MODE_TYPE, bool,
	                   void *);
	bool (*get_mode)(struct nfc_handler *, enum OPS_MODE_TYPE);
	void (*select_chip)(struct nfc_handler *, int);
	void (*set_format)(struct nfc_handler *, struct nfc_format *);
	void (*enable_randomizer)(struct nfc_handler *, u32, bool);
	void (*disable_randomizer)(struct nfc_handler *);
	int (*wait_busy)(struct nfc_handler *, int, enum WAIT_TYPE);
	int (*calculate_ecc)(struct nfc_handler *, u8 *, u8 *, u32, u8);
	int (*correct_ecc)(struct nfc_handler *, u8 *, u32, u8);
	int (*calibration)(struct nfc_handler *);
	int (*suspend)(struct nfc_handler *);
	int (*resume)(struct nfc_handler *);
};

extern struct nfc_handler *nfc_init(struct nfc_resource *res);
extern void nfc_exit(struct nfc_handler *handler);

#endif
