/* Copyright Statement:
 * *
 * * This software/firmware and related documentation ("MediaTek Software") are
 * * protected under relevant copyright laws. The information contained herein
 * * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * * Without the prior written permission of MediaTek inc. and/or its licensors,
 * * any reproduction, modification, use or disclosure of MediaTek Software,
 * * and information contained herein, in whole or in part, shall be strictly
 * * prohibited.
 * *
 * * MediaTek Inc. (C) 2016. All rights reserved.
 * *
 * * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER
 * * AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING
 * * THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE
 * * RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED
 * * IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY
 * * MEDIATEK SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM
 * * TO A PARTICULAR STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE
 * * REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE
 * * MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO
 * * REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE
 * * LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH
 * * MEDIATEK SOFTWARE AT ISSUE.
 * *
 * * The following software/firmware and/or related documentation ("MediaTek
 * * Software")
 * * have been modified by MediaTek Inc. All revisions are subject to any
 * * receiver's applicable license agreements with MediaTek Inc.
 * */

/******************************************************************************
 *
 *  This file contains functions for the MTK defined interop function
 *
 ******************************************************************************/
#define LOG_TAG "bt_device_interop_mtk"

#include "interop_mtk.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
#include <base/logging.h>

#include "btif_config.h"
#include "btm_api.h"
#include "interop_database_mtk.h"
#include "osi/include/log.h"

#ifndef CASE_RETURN_STR
#define CASE_RETURN_STR(const) \
  case const:                  \
    return #const;
#endif

static const char* interop_feature_string_(
    const mtk_interop_feature_t feature) {
  switch (feature) {
// Added for HOGP/GATT/LE, @start {
    CASE_RETURN_STR(INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS)
    CASE_RETURN_STR(INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ACCEPT)
    CASE_RETURN_STR(INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ADJUST)
    CASE_RETURN_STR(INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST)
    CASE_RETURN_STR(INTEROP_MTK_LE_CONN_LATENCY_ADJUST)
    CASE_RETURN_STR(INTEROP_MTK_LE_DISABLE_FAST_CONNECTION)
    CASE_RETURN_STR(INTEROP_MTK_LE_DISABLE_PREF_CONN_PARAMS)
// } @end

// Added for A2DP, @start {
    CASE_RETURN_STR(INTEROP_MTK_A2DP_CHANGE_ACCEPT_SIGNALLING_TMS)
    CASE_RETURN_STR(INTEROP_MTK_A2DP_DELAY_START_CMD)
    CASE_RETURN_STR(INTEROP_MTK_SKIP_REMOTE_SUSPEND_FLAG)
    CASE_RETURN_STR(INTEROP_MTK_ADVANCED_A2DP_CONNECT)
    CASE_RETURN_STR(INTEROP_MTK_ACCEPT_L2C_WHEN_CONNECTING)
    CASE_RETURN_STR(INTEROP_MTK_A2DP_DISABLE_DELAY_REPORT)
    CASE_RETURN_STR(INTEROP_MTK_A2DP_DISABLE_AAC_CODEC)
    CASE_RETURN_STR(INTEROP_MTK_A2DP_SET_STANDBY_TIME)
    CASE_RETURN_STR(INTEROP_MTK_SKIP_REMOTE_START_REQ)
    CASE_RETURN_STR(INTEROP_MTK_DISABLE_SNIFF_MODE_WHEN_A2DP_START)
// } @end

// Added for HFP, @start {
    /** M: Add for HFP @{ */
    CASE_RETURN_STR(INTEROP_MTK_HFP_DEALY_OPEN_SCO)
    CASE_RETURN_STR(INTEROP_MTK_HFP_DELAY_SCO_IN_ACT)
    CASE_RETURN_STR(INTEROP_MTK_HFP_DELAY_DISC_SCO)
    CASE_RETURN_STR(INTEROP_MTK_HFP_17_TO_16)
    CASE_RETURN_STR(INTEROP_MTK_HFP_FORCE_TO_USE_SCO)
    CASE_RETURN_STR(INTEROP_MTK_HFP_FORCE_TO_USE_CVSD)
    CASE_RETURN_STR(INTEROP_MTK_HFP_DELAY_SCO_AFTER_ACTIVE)
    CASE_RETURN_STR(INTEROP_MTK_HFP_DELAY_SCO_FOR_MO_CALL)
    CASE_RETURN_STR(INTEROP_MTK_HFP_DELAY_SCO_FOR_MT_CALL)
    CASE_RETURN_STR(INTEROP_MTK_HFP_UPDATE_SECOND_CALLSTATE)
    CASE_RETURN_STR(INTEROP_MTK_HFP_NO_USE_CIND)
    CASE_RETURN_STR(INTEROP_MTK_HFP_USE_ORIGINAL_TIMEOUT)
    CASE_RETURN_STR(INTEROP_MTK_HFP_SCO_RECOVERY)
    CASE_RETURN_STR(INTEROP_MTK_HFP_CREATE_SCO_AFTER_ATA)
    CASE_RETURN_STR(INTEROP_MTK_HFP_NO_REPORT_CIEV_7_2)
    CASE_RETURN_STR(INTEROP_MTK_HFP_SCO_OPEN_NOT_DO_SNIFF)
    /** @} */
// } @end

// Added for HID, @start {
    CASE_RETURN_STR(INTEROP_MTK_HID_DISABLE_SDP)
    CASE_RETURN_STR(INTEROP_MTK_HID_NOT_DO_SNIFF_SUBRATING)
    CASE_RETURN_STR(INTEROP_MTK_HID_PEER_NOT_USE_HID)
// } @end

// Added for PAN/NAP, @start {
    CASE_RETURN_STR(INTEROP_MTK_PAN_NAP_IGNORE_PEER_NAP_CAPABILITY)
// } @end

// Added for OPP/RFCOMM, @start {
  CASE_RETURN_STR(INTEROP_MTK_FORBID_COMBINE_RFCOMM_DATA)
// } @end

// Added for L2CAP, @start {
    CASE_RETURN_STR(INTEROP_MTK_ACCEPT_CONN_AS_MASTER)
    CASE_RETURN_STR(INTEROP_MTK_ACCEPT_CONN_AS_SLAVE)
    CASE_RETURN_STR(INTEROP_MTK_NOT_RESTART_ACL)
// } @end

// Added for GAP, @start {
  CASE_RETURN_STR(INTEROP_MTK_DISABLE_AUTO_PAIRING)
  CASE_RETURN_STR(INTEROP_MTK_DISABLE_SERVICE_SECURITY_CHECK)
  CASE_RETURN_STR(INTEROP_MTK_GET_UUIDS_FROM_STORAGE)
  CASE_RETURN_STR(INTEROP_MTK_FORCED_SSP)
  CASE_RETURN_STR(INTEROP_MTK_ROLE_SWITCH)
// } @end

// Added for SDP, @start {
// } @end

// Added for AVRCP, @start {
  CASE_RETURN_STR(INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME)
  CASE_RETURN_STR(INTEROP_MTK_AUDIO_SINK_SDP_FAIL_NOT_DISC_AVRCP)
  CASE_RETURN_STR(INTEROP_MTK_UPDATE_PLAY_STATUS_TO_UNACTIVE_DEVICE)
  CASE_RETURN_STR(INTEROP_MTK_AVRCP13_USE)
  CASE_RETURN_STR(INTEROP_MTK_AVRCP14_USE)
  CASE_RETURN_STR(INTEROP_MTK_START_AVRCP_SDP_IOT_DEVICE)
  CASE_RETURN_STR(INTEROP_MTK_AVRCP_CHANGE_POS_INTERVAL)
  CASE_RETURN_STR(INTEROP_MTK_AVRCP_SEND_EXTRA_TRACK_CHANGE)
  CASE_RETURN_STR(INTEROP_MTK_AVRCP_TRACK_ID_NO_CHANGE)
  CASE_RETURN_STR(INTEROP_MTK_AVRCP_FORCE_TRACK_NUMBER_TO_ONE)
  CASE_RETURN_STR(INTEROP_MTK_AVRCP_SEND_TRACK_WHEN_PLAY)
  CASE_RETURN_STR(INTEROP_MTK_START_AVRCP_100_MS)
  CASE_RETURN_STR(INTEROP_MTK_AVRCP_PLAYER_ID_NO_ZERO)
  CASE_RETURN_STR(INTEROP_MTK_AVRCP_FOR_QQ_PLAYER)
  CASE_RETURN_STR(INTEROP_MTK_AVRCP_NO_APP_SETTINGS)
  CASE_RETURN_STR(INTEROP_MTK_AVRCP_USE_MUSIC_ONLY)
  CASE_RETURN_STR(INTEROP_MTK_AVRCP_USE_A2DP_ONLY)
  CASE_RETURN_STR(INTEROP_MTK_AVRCP_FASTFORWARD_REWIND_STATUS)
  CASE_RETURN_STR(INTEROP_MTK_AVRCP_USE_FAKE_ID)
// } @end

// Added for Common, @start {
// } @end
  }
}

bt_status_t btmtk_get_device_name(const RawAddress* addr, BD_NAME bd_name) {
  if (addr) {
    std::string addrstr = addr->ToString();
    const char* bdstr = addrstr.c_str();

    int length = BD_NAME_LEN;
    bool ret = btif_config_get_str(bdstr, "Name",
                                   reinterpret_cast<char*>(bd_name), &length);
    return ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
  }
  return BT_STATUS_FAIL;
}

bool interop_mtk_match_addr(const mtk_interop_feature_t feature,
                            const RawAddress* addr) {
  CHECK(addr);

  const size_t db_size =
      sizeof(mtk_interop_addr_db) / sizeof(mtk_interop_addr_entry_t);
  const mtk_interop_addr_entry_t* db = mtk_interop_addr_db;
  for (size_t i = 0; i != db_size; ++i) {
    if (feature == db[i].feature &&
        memcmp(addr, &db[i].addr, db[i].length) == 0) {
      LOG_WARN(LOG_TAG, "%s() Device %s is a match for interop workaround %s.",
               __func__, addr->ToString().c_str(),
               interop_feature_string_(feature));
      return true;
    }
  }
  return false;
}

bool interop_mtk_match_name(const mtk_interop_feature_t feature,
                            const char* name) {
  if (name != NULL && name[0] != '\0') {
    const size_t db_size =
        sizeof(mtk_interop_name_db) / sizeof(mtk_interop_name_entry_t);
    const mtk_interop_name_entry_t* db = mtk_interop_name_db;
    for (size_t i = 0; i != db_size; ++i) {
      if (feature == db[i].feature && strlen(name) >= db[i].length &&
          strncmp(name, db[i].name, db[i].length) == 0) {
        LOG_WARN(LOG_TAG,
                 "%s() Device %s is a match for interop workaround %s.",
                 __func__, name, interop_feature_string_(feature));
        return true;
      }
    }
    return false;
  }
  return false;
}

bool interop_mtk_match_name(const mtk_interop_feature_t feature,
                            const RawAddress* addr) {
  CHECK(addr);

  BD_NAME remote_name = {0};
  // Get device name from config
  btmtk_get_device_name(addr, remote_name);

  if (remote_name[0] != '\0')
    return interop_mtk_match_name(feature,
                                  reinterpret_cast<char*>(remote_name));
  return false;
}

bool interop_mtk_match_addr_name(const mtk_interop_feature_t feature,
                                 const RawAddress* addr) {
  if (interop_mtk_match_addr(feature, addr)) return true;

  return interop_mtk_match_name(feature, addr);
}
#endif
