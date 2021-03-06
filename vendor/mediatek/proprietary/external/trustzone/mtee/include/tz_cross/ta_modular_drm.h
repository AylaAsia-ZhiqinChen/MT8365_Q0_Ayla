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
 * MediaTek Inc. (C) 2013. All rights reserved.
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

#ifndef __TRUSTZONE_TA_MODULAR_DRM__
#define __TRUSTZONE_TA_MODULAR_DRM__

#define TZ_TA_MODULAR_DRM_UUID   "651d6d29-0cf5-4a0f-b31a-9e8e8cec83a5"

/* Data Structure for Modular DRM TA */
/* You should define data structure used both in REE/TEE here
   N/A for Modular DRM TA */

/* Command for Modular DRM TA */

#define TZCMD_MODULAR_DRM_Initialize               1
#define TZCMD_MODULAR_DRM_Terminate                2
#define TZCMD_MODULAR_DRM_InstallKeybox            3
#define TZCMD_MODULAR_DRM_GetKeyData               4
#define TZCMD_MODULAR_DRM_IsKeyboxValid            5
#define TZCMD_MODULAR_DRM_GetRandom                6
#define TZCMD_MODULAR_DRM_GetDeviceID              7
#define TZCMD_MODULAR_DRM_WrapKeybox               8
#define TZCMD_MODULAR_DRM_OpenSession              9
#define TZCMD_MODULAR_DRM_CloseSession             10
#define TZCMD_MODULAR_DRM_DecryptCTR               11
#define TZCMD_MODULAR_DRM_GenerateDerivedKeys      12
#define TZCMD_MODULAR_DRM_GenerateSignature        13
#define TZCMD_MODULAR_DRM_GenerateNonce            14
#define TZCMD_MODULAR_DRM_LoadKeys                 15
#define TZCMD_MODULAR_DRM_RefreshKeys              16
#define TZCMD_MODULAR_DRM_SelectKey                17
#define TZCMD_MODULAR_DRM_RewrapDeviceRSAKey       18
#define TZCMD_MODULAR_DRM_LoadDeviceRSAKey         19
#define TZCMD_MODULAR_DRM_GenerateRSASignature     20
#define TZCMD_MODULAR_DRM_DeriveKeysFromSessionKey 21
#define TZCMD_MODULAR_DRM_APIVersion               22
#define TZCMD_MODULAR_DRM_SecurityLevel            23
#define TZCMD_MODULAR_DRM_Generic_Encrypt          24
#define TZCMD_MODULAR_DRM_Generic_Decrypt          25
#define TZCMD_MODULAR_DRM_Generic_Sign             26
#define TZCMD_MODULAR_DRM_Generic_Verify           27
#define TZCMD_MODULAR_DRM_GET_RSA_KEY_SIZE         28

#define TZCMD_MODULAR_DRM_TEST                     29
//Disable or enable debug level log in tee
#define TZCMD_MODULAR_DRM_SET_DEBUG_LOG            30

//added by zhitao yan
#define TZCMD_MODULAR_DRM_UpdateUsageTable         31
#define TZCMD_MODULAR_DRM_DeactivateUsageEntry     32
#define TZCMD_MODULAR_DRM_ReportUsage              33
#define TZCMD_MODULAR_DRM_DeleteUsageEntry         34
#define TZCMD_MODULAR_DRM_DeleteUsageTable         35
#define TZCMD_MODULAR_DRM_GetHDCPCapability        36

//add new cmd for V10
#define TZCMD_MODULAR_DRM_CopyBuffer               37
#define TZCMD_MODULAR_DRM_QueryKeyControl          38
#define TZCMD_MODULAR_DRM_LoadTestKeybox           39
#define TZCMD_MODULAR_DRM_LoadTestRSAKey           40
#define TZCMD_MODULAR_DRM_ForceDeleteUsageEntry    41
#define TZCMD_MODULAR_DRM_CheckRPMBAvailability    42
#define TZCMD_MODULAR_DRM_SetRPMBSize              43

//add new cmd for V11
#define TZCMD_MODULAR_DRM_DecryptCENC              44

//add new cmd for V13
#define TZCMD_MODULAR_DRM_GET_CERTIFICATE_TYPE      45
#define TZCMD_MODULAR_DRM_CREATE_USAGE_TABLE_HEADER 46
#define TZCMD_MODULAR_DRM_LOAD_USAGE_TABLE_HEADER   47
#define TZCMD_MODULAR_DRM_LOAD_USAGE_ENTRY          48
#define TZCMD_MODULAR_DRM_UPDATE_USAGE_ENTRY        49
#define TZCMD_MODULAR_DRM_SHRINK_USAGE_TABLE_HEADER 50
#define TZCMD_MODULAR_DRM_MOVE_ENTRY                51
#define TZCMD_MODULAR_DRM_COPY_OLD_USAGE_ENTRY      52
#define TZCMD_MODULAR_DRM_DELETE_OLD_USAGE_TABLE    53
#define TZCMD_MODULAR_DRM_CREATE_OLD_USAGE_ENTRY    54
#define TZCMD_MODULAR_DRM_CREATE_NEW_USAGE_ENTRY    55
#define TZCMD_MODULAR_DRM_CREATE_CURRENT_SRM_VERSION    56

//add new cmd for V14
#define TZCMD_MODULAR_DRM_LOAD_ENTITLED_CONTENT_KEYS    57

//add new cmd for V15
#define TZCMD_MODULAR_DRM_SetDecryptHash            58
#define TZCMD_MODULAR_DRM_GetHashErrorCode          59

typedef enum TEE_MTK_MODULAR_DRM_Crypto_Result
{
    TEE_MTK_MODULAR_DRM_Crypto_SUCCESS                              = 0,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_INIT_FAILED                    = 1,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_TERMINATE_FAILED               = 2,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_OPEN_FAILURE                   = 3,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_CLOSE_FAILURE                  = 4,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_ENTER_SECURE_PLAYBACK_FAILED   = 5,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_EXIT_SECURE_PLAYBACK_FAILED    = 6,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_SHORT_BUFFER                   = 7,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_NO_DEVICE_KEY                  = 8,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_NO_ASSET_KEY                   = 9,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_KEYBOX_INVALID                 = 10,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_NO_KEYDATA                     = 11,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_NO_CW                          = 12,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_DECRYPT_FAILED                 = 13,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_WRITE_KEYBOX                   = 14,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_WRAP_KEYBOX                    = 15,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_BAD_MAGIC                      = 16,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_BAD_CRC                        = 17,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_NO_DEVICEID                    = 18,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_RNG_FAILED                     = 19,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_RNG_NOT_SUPPORTED              = 20,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_SETUP                          = 21,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_OPEN_SESSION_FAILED            = 22,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_CLOSE_SESSION_FAILED           = 23,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_INVALID_SESSION                = 24,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_NOT_IMPLEMENTED                = 25,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_NO_CONTENT_KEY                 = 26,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_CONTROL_INVALID                = 27,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_UNKNOWN_FAILURE                = 28,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_INVALID_CONTEXT                = 29,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_SIGNATURE_FAILURE              = 30,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_TOO_MANY_SESSIONS              = 31,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_INVALID_NONCE                  = 32,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_TOO_MANY_KEYS                  = 33,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_DEVICE_NOT_RSA_PROVISIONED     = 34,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_INVALID_RSA_KEY                = 35,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_KEY_EXPIRED                    = 36,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_INSUFFICIENT_RESOURCES         = 37,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_INSUFFICIENT_HDCP              = 38,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_BUFFER_TOO_LARGE               = 39,
    TEE_MTK_MODULAR_DRM_Crypto_WARNING_GENERATION_SKEW              = 40,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_GENERATION_SKEW                = 41,
    TEE_MTK_MODULAR_DRM_Crypto_LOCAL_DISPLAY_ONLY                   = 42,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_ANALOG_OUTPUT                  = 43,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_WRONG_PST                      = 44,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_WRONG_KEYS                     = 45,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_MISSING_MASTER                 = 46,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_LICENSE_INACTIVE               = 47,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_ENTRY_NEEDS_UPDATE             = 48,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_ENTRY_IN_USE                   = 49,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_USAGE_TABLE_UNRECOVERABLE      = 50,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_KEY_NOT_LOADED                 = 51,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_KEY_NOT_ENTITLED               = 52,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_BAD_HASH                       = 53,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_OUTPUT_TOO_LARGE               = 54,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_SESSION_LOST_STATE             = 55,
    TEE_MTK_MODULAR_DRM_Crypto_ERROR_SYSTEM_INVALIDATED             = 56,
} TEE_MTK_MODULAR_DRM_Crypto_Result;

#endif /* __TRUSTZONE_TA_MODULAR_DRM__ */
