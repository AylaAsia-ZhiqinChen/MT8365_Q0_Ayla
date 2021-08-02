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

#include "nandx_chip_common.h"

static u32 hynix_transfer_mapping(u32 page, bool low)
{
	u32 offset;

	if (low) {
		/* high to low */
		if (page < 4)
			return page;

		offset = page % 4;
		if (offset == 2 || offset == 3)
			return page;

		if (page == 4 || page == 5 || page == 254 || page == 255)
			return page - 4;
		else
			return page - 6;
	} else {
		if (page > 251)
			return page;
		if (page == 0 || page == 1)
			return page + 4;
		offset = page % 4;
		if (offset == 0 || offset == 1)
			return page;
		else
			return page + 6;
	}

	return -1;
}

static u32 hynix_transfer_offset(u32 low_page)
{
	if (low_page < 4)
		return low_page;

	return low_page + (low_page & 0xFFFFFFFE) - 2;
}

static u32 sandisk_transfer_mapping(u32 page, bool low)
{
	if (low) {
		/* high to low */
		if (page == 255)
			return page - 2;
		if (page == 0 || 1 == page % 2)
			return page;
		if (page == 2)
			return 0;
		else
			return page - 3;
	} else {
		if (page != 0 && 0 == page % 2)
			return page;
		if (page == 255)
			return page;
		if (page == 0 || page == 253)
			return page + 2;
		else
			return page + 3;
	}

	return -1U;
}

static u32 sandisk_transfer_offset(u32 low_page)
{
	if (low_page == 0)
		return low_page;

	return low_page + low_page - 1;
}

static u32 micron_transfer_mapping(u32 page, bool low)
{
	u32 offset;

	/* micron 256 pages */
	if (low) {
		/* high to low */
		if (page < 4 || page > 251)
			return page;
		offset = page % 4;
		if (offset == 0 || offset == 1)
			return page;
		else
			return page - 6;
	} else {
		if (page == 2 || page == 3 || page > 247)
			return page;
		offset = page % 4;
		if (offset == 0 || offset == 1)
			return page + 6;
		else
			return page;
	}

	return -1;
}

static u32 micron_transfer_offset(u32 low_page)
{
	u32 temp;

	if (low_page < 4)
		return low_page;

	temp = (low_page - 4) & 0xfffffffe;
	if (low_page <= 130)
		return low_page + temp;

	return low_page + temp - 2;
}

static u32 l95b_transfer_mapping(u32 page, bool low)
{
	/* not used, but complex and difficult to implement */
	NANDX_ASSERT(0);
	return -1;
}

static u32 l95b_transfer_offset(u32 low_page)
{
	u32 temp;

	if (low_page < 6)
		return low_page;

	if (low_page == 259)
		return 509;

	temp = (low_page - 6) & 0xfffffffe;

	if (temp == 0)
		return low_page + 1;

	return low_page + temp;
}

struct pair_page_ops *get_pair_page_ops(u32 mode_type)
{
	static struct pair_page_ops ops;

	if (mode_type & MODE_LOWER_PAGE_HYNIX) {
		ops.transfer_mapping = hynix_transfer_mapping;
		ops.transfer_offset = hynix_transfer_offset;
	} else if (mode_type & MODE_LOWER_PAGE_SANDISK) {
		ops.transfer_mapping = sandisk_transfer_mapping;
		ops.transfer_offset = sandisk_transfer_offset;
	} else if (mode_type & MODE_LOWER_PAGE_MICRON) {
		ops.transfer_mapping = micron_transfer_mapping;
		ops.transfer_offset = micron_transfer_offset;
	} else if (mode_type & MODE_LOWER_PAGE_L95B) {
		ops.transfer_mapping = l95b_transfer_mapping;
		ops.transfer_offset = l95b_transfer_offset;
	} else {
		return NULL;
	}

	return &ops;
}

static void slc_mode_feature_micron_entry(struct nandx_chip *chip)
{
	int ret;
	u8 param[4] = { 0x00, 0x01, 0x00, 0x00 };
	u8 backp[4] = { 0xff, 0xff, 0xff, 0xff };

	/* TODO: for 0x91 */
	ret = nandx_chip_set_feature_with_check(chip, 0x91, param, backp, 4);
	NANDX_ASSERT(!ret);
}

static void slc_mode_feature_micron_exit(struct nandx_chip *chip)
{
	int ret;
	u8 param[4] = { 0x02, 0x01, 0x00, 0x00 };
	u8 backp[4] = { 0xff, 0xff, 0xff, 0xff };

	/* TODO: for 0x91 */
	ret = nandx_chip_set_feature_with_check(chip, 0x91, param, backp, 4);
	NANDX_ASSERT(!ret);
}

static void slc_mode_a2_entry(struct nandx_chip *chip)
{
	struct nfc_handler *nfc = chip->nfc;

	nfc->send_command(nfc, 0xa2);
}

static void slc_mode_da_entry(struct nandx_chip *chip)
{
	struct nfc_handler *nfc = chip->nfc;

	nfc->send_command(nfc, 0xda);
}

static void slc_mode_da_exit(struct nandx_chip *chip)
{
	struct nfc_handler *nfc = chip->nfc;

	nfc->send_command(nfc, 0xdf);
}

struct slc_mode_ops *get_slc_mode_ops(u32 mode_type)
{
	static struct slc_mode_ops ops;

	if (mode_type & MODE_SLC_A2) {
		ops.entry = slc_mode_a2_entry;
		ops.exit = NULL;
	} else if (mode_type & MODE_SLC_DA) {
		ops.entry = slc_mode_da_entry;
		ops.exit = slc_mode_da_exit;
	} else if (mode_type & MODE_SLC_FEATURE_MICRON) {
		ops.entry = slc_mode_feature_micron_entry;
		ops.exit = slc_mode_feature_micron_exit;
	} else {
		return NULL;
	}

	return &ops;
}
