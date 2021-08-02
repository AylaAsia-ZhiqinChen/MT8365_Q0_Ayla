/*
 * MediaTek Inc. (C) 2019. All rights reserved.
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

#include "nandx_errno.h"
#include "nandx_device_info.h"
#include "nandx_chip.h"
#include "nandx_chip_common.h"

static int nandx_bad_block_read(struct nandx_chip *chip, u32 row, bool oob)
{
	int ret = NAND_OK;
	u32 locate, sector_num;
	u8 *data;
	struct nfc_handler *nfc = chip->nfc;
	struct nandx_device_info *dev_info = chip->dev_info;

	data = mem_alloc(1, dev_info->page_size + dev_info->spare_size);
	NANDX_ASSERT(data);

	sector_num = dev_info->page_size / nfc->sector_size;

	nandx_chip_read_page(chip, row);
	nandx_chip_random_output(chip, row, 0);
	ret =
	    nandx_chip_read_data(chip, sector_num, data,
	                         data + dev_info->page_size);
	if (ret == -ENANDREAD) {
		ret = -ENANDBAD;
		goto out;
	}

	if (oob) {
		sector_num =
		    dev_info->page_size / (nfc->sector_size + nfc->fdm_size);
		locate = dev_info->page_size - nfc->fdm_size * sector_num;
		if (data[locate] != 0xff)
			ret = -ENANDBAD;
	} else {
		if (data[0] != 0xff)
			ret = -ENANDBAD;
	}

out:
	mem_free(data);

	return ret;
}

static int nandx_bad_block_slc_program(struct nandx_chip *chip, u32 row)
{
	int status;

	chip->slc_ops->entry(chip);
	nandx_chip_program_page(chip, row, NULL, NULL);
	status = nandx_chip_read_status(chip);
	chip->slc_ops->exit(chip);

	return STATUS_SLC_FAIL(status) ? -ENANDBAD : NAND_OK;
}

static int nandx_bad_block_read_upper_page(struct nandx_chip *chip, u32 row)
{
	int i, ret = -ENANDBAD;
	u8 *data;

	data = mem_alloc(1, chip->nfc->sector_size);

	/* upper page */
	row += 2;
	nandx_chip_read_page(chip, row);
	nandx_chip_random_output(chip, row, 0);
	nandx_chip_read_data(chip, 1, data, NULL);

	for (i = 0; i < 12; i++) {
		if (data[i] != 0x00) {
			ret = NAND_OK;
			break;
		}
	}

	mem_free(data);
	return ret;
}

int nandx_bad_block_check(struct nandx_chip *chip, u32 row,
                          enum BAD_BLOCK_TYPE type)
{
	int page_per_block;

	page_per_block =
	    chip->dev_info->block_size / chip->dev_info->page_size;
	row = row / page_per_block * page_per_block;

	switch (type) {
		case BAD_BLOCK_READ_OOB:
			return nandx_bad_block_read(chip, row, true);

		case BAD_BLOCK_READ_PAGE:
			return nandx_bad_block_read(chip, row, false);

		case BAD_BLOCK_SLC_PROGRAM:
			return nandx_bad_block_slc_program(chip, row);

		case BAD_BLOCK_READ_UPPER_PAGE:
			return nandx_bad_block_read_upper_page(chip, row);

		default:
			break;
	}

	return NAND_OK;
}
