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

#ifndef SECLIB_ERROR_H
#define SECLIB_ERROR_H

/* CRYPTO */
#define ERR_CRYPTO_INIT_FAIL                    0x1000
#define ERR_CRYPTO_DEINIT_FAIL                  0x1001
#define ERR_CRYPTO_MODE_INVALID                 0x1002
#define ERR_CRYPTO_KEY_INVALID                  0x1003
#define ERR_CRYPTO_DATA_UNALIGNED               0x1004
#define ERR_CRYPTO_SEED_LEN_ERROR               0x1005

/* AUTH */
#define ERR_AUTH_IMAGE_VERIFY_FAIL              0x2000
#define ERR_DA_IMAGE_SIG_VERIFY_FAIL            0x2001
#define ERR_DA_IMAGE_NO_MEM_FAIL                0x2002
#define ERR_DA_INIT_KEY_FAIL                    0x2003
#define ERR_IMG_INIT_KEY_FAIL                   0x2004
#define ERR_HASH_IMAGE_FAIL                     0x2005
#define ERR_DA_RELOCATE_SIZE_NOT_ENOUGH         0x2006

/* LIB */
#define ERR_LIB_SEC_CFG_NOT_EXIST               0x3000
#define ERR_LIB_VER_INVALID                     0x3001
#define ERR_LIB_SEC_CFG_ERASE_FAIL              0x3002
#define ERR_LIB_SEC_CFG_CANNOT_WRITE            0x3003
#define ERR_LIB_SEC_CFG_END_PATTERN_NOT_EXIST   0x3004
#define ERR_LIB_SEC_CFG_STATUS_INVALID          0x3005
#define ERR_LIB_SEC_CFG_READ_SIZE_NOT_ENOUGH    0x3006
#define ERR_LIB_SEC_CFG_RSA_KEY_INIT_FAIL       0x3007

/* SECURE REGION CHECK */
#define ERR_REGION_INVALID_INCLUDE              0x7000
#define ERR_REGION_INVALID_OVERLAP              0x7001
#define ERR_REGION_INVALID_OVERFLOW             0x7002
#define ERR_DA_INVALID_LOCATION                 0x7003
#define ERR_DA_INVALID_LENGTH                   0x7004

#endif
