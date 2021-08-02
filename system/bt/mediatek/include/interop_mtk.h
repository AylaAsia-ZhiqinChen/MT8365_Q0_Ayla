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

#pragma once

/******************************************************************************
 *
 *  This file contains functions for the MTK defined interop function
 *
 ******************************************************************************/
#include "mdroid_buildcfg.h"
#include "raw_address.h"

#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
typedef enum {
// Added for HOGP/GATT/LE, @start {
  // Disable secure connections
  // This is for pre BT 4.1/2 devices that do not handle secure mode
  // very well.
  INTEROP_MTK_DISABLE_LE_SECURE_CONNECTIONS,

  // Some device like Arc Touch BT Mouse will behave abnormally if their
  // required interval which is less than BTM_BLE_CONN_INT_MIN_LIMIT
  // is rejected
  INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ACCEPT,

  // Some device like Mad/Designer Mouse will request their
  // interval to BTM_BLE_CONN_INT_MIN, this will have low
  // power or unsmoothy issue(ALPS04221340/ALPS04226407)
  // We should adjust their interval to BTM_BLE_CONN_INT_MIN_LIMIT
  INTEROP_MTK_LE_CONN_INT_MIN_LIMIT_ADJUST,

  // Some device like Casio watch request a minor link supervision timeout which
  // can cause the link timeout frequently. So adjust their link supervision timeout
  // to default value
  INTEROP_MTK_LE_CONN_TIMEOUT_ADJUST,

  // Devices like BSMBB09DS request a large slave latency which will slow down
  // the data transmission or break the link during profile establishment. So adjust
  // the slave latency to default value
  INTEROP_MTK_LE_CONN_LATENCY_ADJUST,

  //Some device like ELECOM cannot handle fast connection procedure
  INTEROP_MTK_LE_DISABLE_FAST_CONNECTION,

  // Some device like ELECOM cann't handle the peripheral pereferred
  // connection parameters update procedure
  INTEROP_MTK_LE_DISABLE_PREF_CONN_PARAMS,
// } @end

// Added for A2DP, @start {
  // Some device will delay send A2DP signaling. It will occur A2DP connection
  // conflict.
  // Change accept signaling time out value for above devices.
  INTEROP_MTK_A2DP_CHANGE_ACCEPT_SIGNALLING_TMS,

  // Some special device want perform START cmd itself first
  // If it not send START cmd, will close current link.
  // So for this special device, we need delay send A2DP START cmd
  // which from DUT to receive the special device cmd.
  INTEROP_MTK_A2DP_DELAY_START_CMD,

  // Some device not send START cmd after send SUSPEND cmd by remote device.
  // So in this case, music will not auto started. In order to skip this issue, add
  // blacklist to remove flag BTIF_AV_FLAG_REMOTE_SUSPEND for trigger START by
  // DUT.
  INTEROP_MTK_SKIP_REMOTE_SUSPEND_FLAG,

  // Some BT devices are less compatible in the market, for example,
  // some devices like connect A2DP ASAP, but others are opposite.
  // Add this blacklist to adapter this case.
  INTEROP_MTK_ADVANCED_A2DP_CONNECT,

  // Some BT devices maybe connect us when we are connecting,
  // this maybe caused collision but if we reject it, the connect
  // will fail, so pass it to high layer to handle.
  INTEROP_MTK_ACCEPT_L2C_WHEN_CONNECTING,

  //Some BT device send delay report continuously when a2dp suspend,
  //or send delay report continuously when a2dp start,this cause high current.
  INTEROP_MTK_A2DP_DISABLE_DELAY_REPORT,

  // Add a2dp AAC black list,disable aac codec
  INTEROP_MTK_A2DP_DISABLE_AAC_CODEC,

  //Some BT device has pop sound when doing music pause
  INTEROP_MTK_A2DP_SET_STANDBY_TIME,

  //Some BT devices send START request when DUT start A2DP at the same time,
  //it may cause A2DP couldn't start properly. In order to skip this issue,
  //add blacklist to skip START request of remote SNK.
  INTEROP_MTK_SKIP_REMOTE_START_REQ,

  //Disable sniff mode during a2dp streaming
  INTEROP_MTK_DISABLE_SNIFF_MODE_WHEN_A2DP_START,
// } @end

// Added for HFP, @start {

  // Sony Ericsson HBH-DS205 has some special request for opening
  // the sco time(if create SCO command be sent before a2dp suspend,
  // the BT SCO will no voice output for the voice call), so work around
  // for this device by delay the SCO setup.
  // (Nexus has the same problem with this device).
  INTEROP_MTK_HFP_DEALY_OPEN_SCO,

  // Some device has IOT issue for HFP. Turn off bt during ring call, then
  // answer call and turn on bt, the end call UI display on Carkit;
  // So work around delay open sco to avoid this issue;
  INTEROP_MTK_HFP_DELAY_SCO_IN_ACT,

  // Can't disconnect SCO when we exit sniff mode closed to disconnect
  // SCO in CAR AUDIO
  INTEROP_MTK_HFP_DELAY_DISC_SCO,

  // Some device has IOT issue for HFP 1.7 version.
  // Back to 1.6 for this devices.
  INTEROP_MTK_HFP_17_TO_16,

  // Some device has issue when setup eSCO, so force to
  // use SCO instead.
  INTEROP_MTK_HFP_FORCE_TO_USE_SCO,

  // Some device will no voice when using mSBC codec, so
  // force to use CVSD.
  INTEROP_MTK_HFP_FORCE_TO_USE_CVSD,

  // Some device will have no voice issue, need to delay some time
  // to setup sco after call active.
  INTEROP_MTK_HFP_DELAY_SCO_AFTER_ACTIVE,

  // Host sends SCO request right after CIEV2,2, but FW will send SCO request
  // first ad then CIEV2,2, because LMP has higher priority than ACL-U.
  // Some devices may cannot resolve this order and no voice come out.
  INTEROP_MTK_HFP_DELAY_SCO_FOR_MO_CALL,

  // In-band ringing enabled, host sends SCO request right after CIEV2,1,
  // but FW will send SCO request first ad then CIEV2,1, because LMP has
  // higher priority than ACL-U. Some devices may cannot resolve this order
  // and no voice come out.
  INTEROP_MTK_HFP_DELAY_SCO_FOR_MT_CALL,

  // Some device has no voice issue during second call, so force to
  // send ciev 1,1 after accept second call.
  INTEROP_MTK_HFP_UPDATE_SECOND_CALLSTATE,

  // Some device will no voice when use cind sync call state
  // before slc setup, and need delay setup sco at same time
  INTEROP_MTK_HFP_NO_USE_CIND,

  // The negotiation time extend to 15s is to long
  //
  INTEROP_MTK_HFP_USE_ORIGINAL_TIMEOUT,

  // Recovery SCO state machine
  //
  INTEROP_MTK_HFP_SCO_RECOVERY,

  //Some device needs to create SCO right after receiving ATA command
  INTEROP_MTK_HFP_CREATE_SCO_AFTER_ATA,

  // Some device will change the audio path to speaker shortly when answer active call
  // change to answer the held call, disable to send +CIEV:7,2 command for those devices
  INTEROP_MTK_HFP_NO_REPORT_CIEV_7_2,

  // Some device need to keep active mode when sco open
  INTEROP_MTK_HFP_SCO_OPEN_NOT_DO_SNIFF,

// } @end

// Added for HID, @start {
  // Some HID device will show connected state too slowly
  // Because when his connection had setup, HID host process
  // SDP,when peer device response slowly,it lead show connected slowly
  INTEROP_MTK_HID_DISABLE_SDP,
  // Not do sniif mode for HID profile.
  INTEROP_MTK_HID_NOT_DO_SNIFF_SUBRATING,
  // Partial HID devices (Carkit) support HID service but they won't
  // use it. They will connect L2CAP channels for HID profile, but they
  // may not disconnect these channels by sending disconnect request.
  INTEROP_MTK_HID_PEER_NOT_USE_HID,
// } @end

// Added for PAN/NAP, @start {
  // Ignore car kit NAP capability, and then phone will not try to connect
  // carkit's NAP neteork
  INTEROP_MTK_PAN_NAP_IGNORE_PEER_NAP_CAPABILITY,
// } @end

// Added for OPP/RFCOMM, @start {
  // Some devices can not parse multi AT commands in one rfcomm packet.
  // So, send data separately.
  INTEROP_MTK_FORBID_COMBINE_RFCOMM_DATA,
// } @end

// Added for L2CAP, @start {
 // Some devices cannot work normally as a master if the connection is
 // initiated by themselves
 INTEROP_MTK_ACCEPT_CONN_AS_MASTER,

 // Some device cannot work normally as a slave if the connection is
 // initiated by themselves.
  INTEROP_MTK_ACCEPT_CONN_AS_SLAVE,

 // Not restart ACL link for pending request when link down
 INTEROP_MTK_NOT_RESTART_ACL,
// } @end

// Added for GAP, @start {
  // Disable automatic pairing with headsets/car-kits
  // Some car kits do not react kindly to a failed pairing attempt and
  // do not allow immediate re-pairing. Blacklist these so that the initial
  // pairing attempt makes it to the user instead.
  INTEROP_MTK_DISABLE_AUTO_PAIRING,
  // Some devices reconnect RFCOMM without security procedure,
  // disable security check for temporary
  INTEROP_MTK_DISABLE_SERVICE_SECURITY_CHECK,
  // get services from storage for some devices
  INTEROP_MTK_GET_UUIDS_FROM_STORAGE,
  // M557 mouse seldomly claims SSP is not supported in its extended features,
  // but actually it really wants SSP.
  INTEROP_MTK_FORCED_SSP,
  // Some headset will create SCO ev3, FW can't handle this SCO when being slave
  INTEROP_MTK_ROLE_SWITCH,
// } @end

// } @end

// Added for SDP, @start {
// } @end

// Added for AVRCP, @start {
  // Devices requiring this workaround do not handle Bluetooth Absolute Volume
  // control correctly, leading to undesirable (potentially harmful) volume
  // levels or general lack of controlability.
  INTEROP_MTK_DISABLE_ABSOLUTE_VOLUME,

  // Some device do not update position progress when get DUT paused status.
  // In this case, we will also update the play status to unactive device.
  INTEROP_MTK_UPDATE_PLAY_STATUS_TO_UNACTIVE_DEVICE,

  // Some device support avrcp but not support a2dp.
  // In this case, we do not close avrcp after audio sink sdp failed
  INTEROP_MTK_AUDIO_SINK_SDP_FAIL_NOT_DISC_AVRCP,

  // Some devices do not support avrcp1.4 and 1.5 very good.
  // we need use avrcp1.3 to response
  INTEROP_MTK_AVRCP13_USE,

  // Some device do not support avrcp1.5 very good.
  // we need use avrcp1.4 to response
  INTEROP_MTK_AVRCP14_USE,

  // After AVDTP open, some device need avrcp update playStatus quickly.
  INTEROP_MTK_START_AVRCP_SDP_IOT_DEVICE,

  //Audi carkit pos interval is 5s, when change codec between sbc/aac,
  //carkit pos will forward 3s and rewind to normal pos.So we change
  //the pos interval to 1s.
  INTEROP_MTK_AVRCP_CHANGE_POS_INTERVAL,

  //HZ audio carkit need extra track changed to sync song information.
  INTEROP_MTK_AVRCP_SEND_EXTRA_TRACK_CHANGE,

  // SCO disconnect and A2dp resume, Some carkit will in FM UI
  // We should send track change and ID 0 to it.
  INTEROP_MTK_AVRCP_TRACK_ID_NO_CHANGE,

  //BWM 80957 carkit need track number and total number as 1 on android P.
  INTEROP_MTK_AVRCP_FORCE_TRACK_NUMBER_TO_ONE,

  // CHEVY need track change event when music change to playing
  INTEROP_MTK_AVRCP_SEND_TRACK_WHEN_PLAY,

  // Fixed slower Avrcp connection cause IOT issue
  INTEROP_MTK_START_AVRCP_100_MS,

  // Fixed carkit need no-zero player id iot issue
  INTEROP_MTK_AVRCP_PLAYER_ID_NO_ZERO,

  // QQ music pre/next no song info on IOT carkit
  INTEROP_MTK_AVRCP_FOR_QQ_PLAYER,

  // carkit don't support app settings, but still send related cmds
  INTEROP_MTK_AVRCP_NO_APP_SETTINGS,

  // carkit use audio state will cause UI delay show pause
  INTEROP_MTK_AVRCP_USE_MUSIC_ONLY,

  // carkit need a2dp state to avoid send play key
  INTEROP_MTK_AVRCP_USE_A2DP_ONLY,

  // carkit need forward/rewind status
  INTEROP_MTK_AVRCP_FASTFORWARD_REWIND_STATUS,

  // carkit need change ID to refresh UI
  INTEROP_MTK_AVRCP_USE_FAKE_ID,
// } @end

// Added for Common, @start {
// } @end
} mtk_interop_feature_t;

/******************************************************************************
 *
 **
 ** Function         interop_mtk_match_addr
 **
 ** Description      Looks up the mtk_interop_addr_db for the specified BD
 **                  address.
 **
 ** Returns          TRUE if matched, else FALSE
 **
 *******************************************************************************/
extern bool interop_mtk_match_addr(const mtk_interop_feature_t feature,
                                   const RawAddress* addr);

/*******************************************************************************
**
** Function         interop_mtk_match_name
**
** Description      Looks up the mtk_interop_name_db for the specified BD
**                   name.
**
** Returns          TRUE if matched, else FALSE
**
*******************************************************************************/
extern bool interop_mtk_match_name(const mtk_interop_feature_t feature,
                                   const char* name);

/*******************************************************************************
**
** Function         interop_mtk_match_name
**
** Description      Looks up the mtk_interop_name_db for the specified BD
**                   address.
**
** Returns          TRUE if matched, else FALSE
**
*******************************************************************************/
extern bool interop_mtk_match_name(const mtk_interop_feature_t feature,
                                   const RawAddress* addr);

/*******************************************************************************
**
** Function         interop_mtk_match_addr_name
**
** Description      Looks up the mtk_interop_addr_db and mtk_interop_name_db
**                  for the specified BD address.
**
** Returns          TRUE if matched, else FALSE
**
*******************************************************************************/
extern bool interop_mtk_match_addr_name(const mtk_interop_feature_t feature,
                                        const RawAddress* addr);
#endif
