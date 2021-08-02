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

//#include <platform/mt_partition.h>
#include <mt_partition.h>
part_t partition_layout[PART_MAX_COUNT];

struct part_name_map g_part_name_map[PART_MAX_COUNT] = {
	{"preloader",   "PRELOADER",    "raw data", 0,  0,  0},
	{"mbr", "MBR",  "raw data", 1,  0,  0},
	{"ebr1",    "EBR1", "raw data", 2,  0,  0},
	{"pro_info",    "PRO_INFO", "raw data", 3,  0,  0},
	{"nvram",   "NVRAM",    "raw data", 4,  0,  0},
	{"protect_f",   "PROTECT_F",    "ext4", 5,  0,  0},
	{"protect_s",   "PROTECT_S",    "ext4", 6,  0,  0},
	{"persist",   "PERSIST",    "ext4", 7,  0,  0},
	{"seccfg",  "SECCFG",   "raw data", 8,  0,  0},
	{"uboot",   "UBOOT",    "raw data", 9,  1,  1},
	{"mnb",		"MNB",	"raw data",	10,	1,	1},
	{"boot",    "BOOTIMG",  "raw data", 11,  1,  1},
	{"recovery",    "RECOVERY", "raw data", 12, 1,  1},
	{"secro",  "SEC_RO",    "ext4", 13, 0,  0},
	{"misc",    "MISC", "raw data", 14, 0,  0},
	{"logo",    "LOGO", "raw data", 15, 0,  0},
	{"dtbo",    "DTBO", "raw data", 16, 0,  0},
	{"ebr2",    "EBR2", "raw data", 17, 0,  0},
	{"custom",  "CUSTOM",   "ext4", 18, 0,  0},
	{"expdb",   "EXPDB",   "raw data", 19, 0,  0},
	{"tee1",   "TEE1",   "raw data", 20, 0,  0},
	{"tee2",   "TEE2",   "raw data", 21, 0,  0},
	{"system",  "MNTL",    "ext4", 22, 1,  1},
	{"vendor",  "MNTL",    "ext4", 23, 1,  1},
	{"cache",   "MNTL",    "ext4", 24, 1,  1},
	{"userdata",    "MNTL", "ext4", 25, 1,  1},
	{"fat", "FAT",  "fat",  26, 0,  0},
	{"otp", "OTP",  "raw data",  27, 0,  0},
	{"bmtpool", "BMTPOOL",  "raw data",  28, 0,  0},
};
