/* Copyright Statement:
 * *
 * * This software/firmware and related documentation ("MediaTek Software") are
 * * protected under relevant copyright laws. The information contained herein
 * * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * * Without the prior written permission of MediaTek inc. and/or its licensors,
 * * any reproduction, modification, use or disclosure of MediaTek Software,
 * * and information contained herein, in whole or in part, shall be strictly prohibited.
 * *
 * * MediaTek Inc. (C) 2017. All rights reserved.
 * *
 * * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 * *
 * * The following software/firmware and/or related documentation ("MediaTek Software")
 * * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * * applicable license agreements with MediaTek Inc.
 * */

#pragma once

#if __STDC_VERSION__ < 199901L
#  ifndef FALSE
#    define FALSE 0
#  endif
#  ifndef TRUE
#    define TRUE (!FALSE)
#  endif
#else
#  include <stdbool.h>
#  ifndef FALSE
#    define FALSE  false
#  endif
#  ifndef TRUE
#    define TRUE   true
#  endif
#endif

#define MTK_BRANCH "neptune-trunk-q0.tk "
#define MTK_VER " "
#define MTK_TAG " "
#define MTK_INFO MTK_BRANCH MTK_VER MTK_TAG

/******************************************************************************
 * **
 * ** for FW multiple links 7br + 32ble
 * **
 * ******************************************************************************/
#ifndef MAX_ACL_CONNECTIONS
#define MAX_ACL_CONNECTIONS    39
#endif

#ifndef GATT_MAX_PHY_CHANNEL
#define GATT_MAX_PHY_CHANNEL    MAX_ACL_CONNECTIONS
#endif

#ifndef BTA_GATTC_KNOWN_SR_MAX
#define BTA_GATTC_KNOWN_SR_MAX    GATT_MAX_PHY_CHANNEL
#endif

#ifndef BTA_GATTC_CL_MAX
#define BTA_GATTC_CL_MAX    64
#endif

#ifndef GATT_MAX_SR_PROFILES
#define GATT_MAX_SR_PROFILES    64
#endif

#ifndef GATT_MAX_APPS
#define GATT_MAX_APPS    64
#endif

#ifndef GATT_CL_MAX_LCB
#define GATT_CL_MAX_LCB    64
#endif

#ifndef BTM_BLE_MAX_BG_CONN_DEV_NUM
#define BTM_BLE_MAX_BG_CONN_DEV_NUM    64
#endif

#ifndef GATT_MAX_BG_CONN_DEV
#define GATT_MAX_BG_CONN_DEV    BTM_BLE_MAX_BG_CONN_DEV_NUM
#endif

/******************************************************************************
 * **
 * ** define MTK comp id and MTK controller
 * **
 * ******************************************************************************/
#define LMP_COMPID_MEDIATEK 70

#define BTM_IS_MTK_CONTROLLER()                                 \
      (controller_get_interface()->get_bt_version()->manufacturer == \
       LMP_COMPID_MEDIATEK)

/******************************************************************************
 * **
 * ** define max number of l2cap clients, reconfig to 20
 * **
 * ******************************************************************************/
#ifndef MAX_L2CAP_CLIENTS
#define MAX_L2CAP_CLIENTS 20
#endif

// For HDP PTS case.
// In 64bit project. It occur SDP DB full error by smaller size.
#ifndef BTA_HL_DISC_SIZE
#define BTA_HL_DISC_SIZE 3200
#endif

#ifndef MTK_PTS_HF_TEST
#define MTK_PTS_HF_TEST TRUE //added for hfp(hf role) pts test
#endif

#ifndef MTK_PTS_AV_TEST
#define MTK_PTS_AV_TEST TRUE// added for av pts test
#define MTK_PTS_AV_SEC_CTRL TRUE// to support security control or not

#define MTK_PTS_TEST_IS_ENABLE \
    char values[PROPERTY_VALUE_MAX] = {0}; \
    if ((osi_property_get("persist.bluetooth.pts.enable", values, "off")) && (!strcmp(values, "on")))
#endif


/******************************************************************************
 * **
 * ** Disable connection down timer (in milliseconds)
 * **
 * ******************************************************************************/
#ifndef BTA_DM_DISABLE_CONN_DOWN_TIMER_MS
#define BTA_DM_DISABLE_CONN_DOWN_TIMER_MS 500
#endif

/** M: Bug fix for IOT device a2dp signal response too slowly @{ */
#ifndef BTA_AV_SIG_TOUT
#define BTA_AV_SIG_TOUT 7
#endif
/** @} */

#ifndef BTM_BLE_SCAN_SLOW_INT_1
#define BTM_BLE_SCAN_SLOW_INT_1 96
#endif

#ifndef BTM_BLE_CONN_INT_MIN_DEF
#define BTM_BLE_CONN_INT_MIN_DEF 9
#endif

#ifndef BTM_BLE_CONN_INT_MAX_DEF
#define BTM_BLE_CONN_INT_MAX_DEF 9
#endif

#ifndef BTM_BLE_CONN_INT_MIN_LIMIT
#define BTM_BLE_CONN_INT_MIN_LIMIT 9
#endif

#ifndef MTK_COMMON
#define MTK_COMMON TRUE
#endif

#ifndef BLE_VND_INCLUDED
#define BLE_VND_INCLUDED TRUE
#endif

#ifndef BT_TUNNEL_SUPPORT
#define BT_TUNNEL_SUPPORT FALSE
#endif

#if defined(MTK_COMMON) && (MTK_COMMON == TRUE)
/* AC off / on soon after RC paired/unpaired (as a stress test), the hid
information will not be saved/removed correctly.
Notice: some special paltforms maybe need a lot of time to flush*/
#define FILE_FLUSH_FOR_DEVICE_INFO_LOST FALSE
#endif

#if defined(MTK_COMMON) && (MTK_COMMON == TRUE)
#define PATCH_FOR_NOTIFICATION_MAX TRUE
/* AC off / on soon after RC paired/unpaired (as a stress test), the hid
information will not be saved/removed correctly.
Notice: system("sync") maybe take a lot of time on some special platforms*/
#define FILE_SYNC_FOR_DEVICE_INFO_LOST FALSE
#endif

