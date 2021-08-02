/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2011. All rights reserved.
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
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver's
* applicable license agreements with MediaTek Inc.
*/


#ifndef CUST_SEC_CTRL_H
#define CUST_SEC_CTRL_H

#include "typedefs.h"
#include "proj_cfg.h"
#include "keypad.h"

/**************************************************************************
 * [ROM INFO]
 **************************************************************************/
#define PROJECT_NAME                        "CUST"
#define PLATFORM_NAME                       "MT8168"


/**************************************************************************
 * [CRYPTO SEED]
 **************************************************************************/
#define CUSTOM_CRYPTO_SEED_SIZE             (16)
#define CUSTOM_CRYPTO_SEED                  "1A52A367CB12C458"

/**************************************************************************
 * [SML AES KEY CONTROL]
 **************************************************************************/
/* It can be enabled only if SUSBDL is turned on */
/* Please make sure SUSBDL is on before enabling this flag */
//#define SML_AES_KEY_ANTICLONE_EN

/**************************************************************************
 * [S-USBDL]
 **************************************************************************/
/* S-USBDL Attribute */
#define ATTR_SUSBDL_DISABLE                 0x00
#define ATTR_SUSBDL_ENABLE                  0x11
#define ATTR_SUSBDL_ONLY_ENABLE_ON_SCHIP    0x22
/* S-USBDL Control */
#define SEC_USBDL_CFG                       CUSTOM_SUSBDL_CFG

/**************************************************************************
 * [FLASHTOOL SECURE CONFIG for (for both of SLA and NON-SLA mode], 32bits
 * It's not recommended to use 32 bits (v3) mode now. Please use FLASHTOOL_SEC_CFG_64 instead
 **************************************************************************/
//#define FLASHTOOL_SEC_CFG
//#define BYPASS_CHECK_IMAGE_0_NAME           ""
//#define BYPASS_CHECK_IMAGE_0_OFFSET         0x0
//#define BYPASS_CHECK_IMAGE_0_LENGTH         0x0
//#define BYPASS_CHECK_IMAGE_1_NAME           ""
//#define BYPASS_CHECK_IMAGE_1_OFFSET         0x0
//#define BYPASS_CHECK_IMAGE_1_LENGTH         0x0
//#define BYPASS_CHECK_IMAGE_2_NAME           ""
//#define BYPASS_CHECK_IMAGE_2_OFFSET         0x0
//#define BYPASS_CHECK_IMAGE_2_LENGTH         0x0

/**************************************************************************
 * [FLASHTOOL SECURE CONFIG (for both of SLA and NON-SLA mode], 64 bits for v4 sign format
 **************************************************************************/
//#define FLASHTOOL_SEC_CFG_64
#ifdef FLASHTOOL_SEC_CFG_64
#define BYPASS_CHECK_IMAGE_0_NAME           ""
#define BYPASS_CHECK_IMAGE_0_OFFSET         0x0
#define BYPASS_CHECK_IMAGE_1_NAME           ""
#define BYPASS_CHECK_IMAGE_1_OFFSET         0x0
#define BYPASS_CHECK_IMAGE_2_NAME           ""
#define BYPASS_CHECK_IMAGE_2_OFFSET         0x0
#endif
/**************************************************************************
 * [FLASHTOOL FORBIT DOWNLOAD CONFIG (for NSLA mode only)] , 32 bits
 * It's not recommended to use 32 bits (v3) mode now. Please use FLASHTOOL_FORBID_DL_NSLA_CFG_64 instead
 **************************************************************************/
//#define FLASHTOOL_FORBID_DL_NSLA_CFG
//#define FORBID_DL_IMAGE_0_NAME              ""
//#define FORBID_DL_IMAGE_0_OFFSET            0x0
//#define FORBID_DL_IMAGE_0_LENGTH            0x0
//#define FORBID_DL_IMAGE_1_NAME              ""
//#define FORBID_DL_IMAGE_1_OFFSET            0x0
//#define FORBID_DL_IMAGE_1_LENGTH            0x0

/**************************************************************************
 * [FLASHTOOL FORBIT DOWNLOAD CONFIG (for NSLA mode only)], 64 bits for v4 sign format
 **************************************************************************/
//#define FLASHTOOL_FORBID_DL_NSLA_CFG_64
#ifdef FLASHTOOL_FORBID_DL_NSLA_CFG_64
#define FORBID_DL_IMAGE_0_NAME              ""
#define FORBID_DL_IMAGE_0_OFFSET            0x0
#define FORBID_DL_IMAGE_1_NAME              ""
#define FORBID_DL_IMAGE_1_OFFSET            0x0
#endif

#define SEC_USBDL_WITHOUT_SLA_ENABLE

#ifdef SEC_USBDL_WITHOUT_SLA_ENABLE
//#define USBDL_DETECT_VIA_KEY
/* if com port wait key is enabled, define the key*/
#ifdef USBDL_DETECT_VIA_KEY
#define COM_WAIT_KEY    KPD_DL_KEY3
#endif
//#define USBDL_DETECT_VIA_AT_COMMAND
#endif

/**************************************************************************
 * [S-BOOT]
 **************************************************************************/
/* S-BOOT Attribute */
#define ATTR_SBOOT_DISABLE                  0x00
#define ATTR_SBOOT_ENABLE                   0x11
#define ATTR_SBOOT_ONLY_ENABLE_ON_SCHIP     0x22
/* S-BOOT Control */
#define SEC_BOOT_CFG                        CUSTOM_SBOOT_CFG

/* Customized Secure Boot */
//#define CUSTOMIZED_SECURE_PARTITION_SUPPORT
#ifdef CUSTOMIZED_SECURE_PARTITION_SUPPORT
#define SBOOT_CUST_PART1    ""
#define SBOOT_CUST_PART2    ""
#endif

/* For Custom Partition Verification*/
#define VERIFY_PART_CUST                   (FALSE)
#define VERIFY_PART_CUST_NAME              ""

/* 
    RSA2048 public key for verifying mtee image
    It should be the same as AUTH_PARAM_N in alps\mediatek\custom\mt8168_evb\trustzone\TRUSTZONE_IMG_PROTECT_CFG.ini
*/
#define MTEE_IMG_VFY_PUBK_SZ 256

#define MTEE_IMG_VFY_PUBK \
    0xda, 0x44, 0x1f, 0x33, 0xe1, 0xe2, 0xe0, 0xcd, \
    0x2f, 0x1c, 0x77, 0xe7, 0x69, 0x77, 0xed, 0x99, \
    0x22, 0x6c, 0xd5, 0x8b, 0x30, 0x2b, 0x16, 0xb5, \
    0xc7, 0xd8, 0xf8, 0x5d, 0x64, 0x59, 0x01, 0x79, \
    0x4f, 0x13, 0xbe, 0x53, 0xfb, 0xa5, 0x08, 0x73, \
    0x7a, 0xed, 0xd1, 0x10, 0xff, 0xc6, 0x2d, 0xdb, \
    0xe0, 0x41, 0x42, 0xcf, 0x65, 0x72, 0x99, 0x9b, \
    0x33, 0x9d, 0x3e, 0xf6, 0xa2, 0xd3, 0xa1, 0x18, \
    0x6f, 0x15, 0xb5, 0x4d, 0xe9, 0xde, 0x98, 0xee, \
    0xfd, 0x68, 0x05, 0x5f, 0x1c, 0x71, 0xb0, 0x78, \
    0x40, 0x14, 0xf1, 0x0f, 0x4b, 0xd2, 0xbe, 0x9f, \
    0x2e, 0x9b, 0x37, 0xf3, 0xfa, 0x97, 0x48, 0x00, \
    0xd6, 0xa7, 0xe3, 0xaf, 0xde, 0x9d, 0x62, 0xc0, \
    0x9a, 0x3a, 0x87, 0x52, 0x16, 0x73, 0x77, 0x5a, \
    0xcd, 0x45, 0xf9, 0x62, 0xff, 0x53, 0x09, 0x32, \
    0xb9, 0x8a, 0x54, 0x72, 0xc0, 0xa6, 0x78, 0x36, \
    0xfb, 0x0f, 0x43, 0x6e, 0x42, 0x86, 0x63, 0x8a, \
    0x75, 0xcf, 0x37, 0x6b, 0x81, 0x22, 0x13, 0xd6, \
    0xc0, 0x3f, 0x24, 0x65, 0xa8, 0x69, 0x89, 0x0f, \
    0x87, 0x09, 0xb5, 0x31, 0x13, 0x69, 0x50, 0x4c, \
    0xce, 0xc2, 0x2b, 0xd3, 0x36, 0xd3, 0x6a, 0xe0, \
    0x2a, 0x5b, 0x62, 0x58, 0x5c, 0xe2, 0xed, 0x7e, \
    0x9d, 0xca, 0x2b, 0x33, 0x2a, 0xc5, 0x97, 0x3b, \
    0xf2, 0x5c, 0x02, 0x1b, 0xa0, 0x31, 0x4e, 0xb9, \
    0x5d, 0x4c, 0xcc, 0x30, 0x2b, 0xd8, 0xcd, 0x0e, \
    0x26, 0x44, 0x47, 0xa2, 0xe6, 0x66, 0x67, 0x01, \
    0xb3, 0xae, 0x8f, 0x42, 0x60, 0x5d, 0xe8, 0x39, \
    0x57, 0x85, 0x2e, 0x49, 0x0b, 0x40, 0x72, 0x5e, \
    0x76, 0x44, 0x53, 0x0f, 0x6b, 0x55, 0xb3, 0x3e, \
    0x4d, 0x63, 0xf0, 0xde, 0x1a, 0x6f, 0x18, 0xa0, \
    0xb9, 0x82, 0x49, 0xc2, 0xe2, 0xb0, 0x71, 0xb9, \
    0x8f, 0xa8, 0x56, 0x0d, 0x31, 0x3a, 0x3d, 0x5d

/**************************************************************************
 * [DEFINITION CHECK]
 **************************************************************************/
#ifdef SML_AES_KEY_ANTICLONE_EN
#ifndef SECRO_IMG_ANTICLONE_EN
#error "SML_AES_KEY_ANTICLONE_EN is defined. Should also enable SECRO_IMG_ANTICLONE_EN"
#endif
#endif

#if MTK_SECURITY_SW_SUPPORT
#ifndef SEC_USBDL_CFG
#error "MTK_SECURITY_SW_SUPPORT is NOT disabled. Should define SEC_USBDL_CFG "
#endif
#endif

#if MTK_SECURITY_SW_SUPPORT
#ifndef SEC_BOOT_CFG
#error "MTK_SECURITY_SW_SUPPORT is NOT disabled. Should define SEC_BOOT_CFG"
#endif
#endif

#if MTK_SECURITY_SW_SUPPORT
#ifndef SEC_USBDL_WITHOUT_SLA_ENABLE
#error "MTK_SECURITY_SW_SUPPORT is NOT disabled. Should define SEC_USBDL_WITHOUT_SLA_ENABLE"
#endif
#endif

#ifdef USBDL_DETECT_VIA_KEY
#ifndef SEC_USBDL_WITHOUT_SLA_ENABLE
#error "USBDL_DETECT_VIA_KEY can only be enabled when SEC_USBDL_WITHOUT_SLA_ENABLE is enabled"
#endif
#ifndef COM_WAIT_KEY
#error "COM_WAIT_KEY is not defined!!"
#endif
#endif

#ifdef USBDL_DETECT_VIA_AT_COMMAND
#ifndef SEC_USBDL_WITHOUT_SLA_ENABLE
#error "USBDL_DETECT_VIA_AT_COMMAND can only be enabled when SEC_USBDL_WITHOUT_SLA_ENABLE is enabled"
#endif
#endif


#endif   /* CUST_SEC_CTRL_H */
