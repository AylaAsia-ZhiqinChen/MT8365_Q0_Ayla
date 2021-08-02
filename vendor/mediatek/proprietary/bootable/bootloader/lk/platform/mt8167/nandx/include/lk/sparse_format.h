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

typedef struct sparse_header {
	/* 0xed26ff3a */
	uint32_t magic;
	/* (0x1) - reject images with higher major versions */
	uint16_t major_version;
	/* (0x0) - allow images with higer minor versions */
	uint16_t minor_version;
	/* 28 bytes for first revision of the file format */
	uint16_t file_hdr_sz;
	/* 12 bytes for first revision of the file format */
	uint16_t chunk_hdr_sz;
	/* block size in bytes, must be a multiple of 4 (4096) */
	uint32_t blk_sz;
	/* total blocks in the non-sparse output image */
	uint32_t total_blks;
	/* total chunks in the sparse input image */
	uint32_t total_chunks;
	/* CRC32 checksum of the original data, counting "don't care" */
	uint32_t image_checksum;
	/* as 0. Standard 802.3 polynomial, use a Public Domain */
	/* table implementation */
} sparse_header_t;

#define SPARSE_HEADER_MAGIC 0xed26ff3a

#define CHUNK_TYPE_RAW      0xCAC1
#define CHUNK_TYPE_FILL     0xCAC2
#define CHUNK_TYPE_DONT_CARE    0xCAC3
#define CHUNK_TYPE_CRC      0xCAC4

typedef struct chunk_header {
	/* 0xCAC1 -> raw; 0xCAC2 -> fill; 0xCAC3 -> don't care */
	uint16_t chunk_type;
	uint16_t reserved1;
	/* in blocks in output image */
	uint32_t chunk_sz;
	/* in bytes of chunk input file including chunk header and data */
	uint32_t total_sz;
} chunk_header_t;

/*
 * Following a Raw or Fill chunk is data.
 * For a Raw chunk, it's the data in chunk_sz * blk_sz.
 *  For a Fill chunk, it's 4 bytes of the fill data.
 */
