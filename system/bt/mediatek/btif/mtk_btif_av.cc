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
 * * NONINFRINGEMENT.
 * * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO
 * * SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER
 * * EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN
 * * FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE.
 * * MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES
 * * MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR
 * * OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED
 * * HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK
 * * SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE
 * * PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 * *
 * * The following software/firmware and/or related documentation ("MediaTek Software")
 * * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * * applicable license agreements with MediaTek Inc.
 * */

#define LOG_TAG "mtk_btif_av"

#include "mtk_btif_av.h"
#include "btif_av.h"
#include "audio_hal_interface/a2dp_encoding.h"
#include "btif_a2dp_audio_interface.h"
#include "btif_hf.h"

#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
#include "btif_a2dp_control.h"
#include "interop_mtk.h"
#include "osi/include/alarm.h"
#include "osi/include/log.h"
#endif

#if defined(MTK_VND_A2DP_PKT_LEN) && (MTK_VND_A2DP_PKT_LEN == TRUE)
#include "stack/btm/btm_int.h"
#include "mediatek/stack/include/a2dp_sbc_encoder.h"
#endif

/** M: Firmware inform host to adjust a2dp packet length as 2-DH5/3-DH5 by RSSI.
 ** Then host will modify a2dp pacekt length of SBC codec after receive vendor event. @{ */
#if defined(MTK_VND_A2DP_PKT_LEN) && (MTK_VND_A2DP_PKT_LEN == TRUE)
/*******************************************************************************
**
** Function         btif_av_change_pkt_len_callback
**
** Description     Callback function for vendor specific event that to change packet length of a2dp
**
** Returns          void
**
*******************************************************************************/
void btif_av_change_pkt_len_callback(uint8_t len, uint8_t* p) {
  if (4 == len && 0x75 == *p++) {
    uint16_t handle;
    STREAM_TO_UINT16(handle, p);
    tBTM_SEC_DEV_REC* p_dev_rec = NULL;
    p_dev_rec = btm_find_dev_by_handle(handle);
    if (p_dev_rec && p_dev_rec->bd_addr == btif_av_source_active_peer()) {
      //*p = 1: 2M; *p = 0: 3M
      a2dp_sbc_set_3mbps(1 - *p);
    }
  }
}
#endif

#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)

#define BTIF_TIMER_A2DP_DELAY_START_CMD (1 * 1000)
//#define MTK_BTIF_AV_START_STREAM_REQ_EVT 27

extern fixed_queue_t* btu_general_alarm_queue;

static alarm_t* tle_av_delay_start_cmd = NULL;

BtifA2dpDataCallback BtAvServiceDelayTimer::btif_a2dp_data_cb_(nullptr);

/*******************************************************************************
**
** Function         btif_create_av_domain_timer
**
** Description     Call it on enable AV service to create delay timer
**
** Returns          void
**
*******************************************************************************/
void BtAvServiceDelayTimer::btif_create_av_domain_timer() {
  alarm_free(tle_av_delay_start_cmd);
  tle_av_delay_start_cmd = alarm_new("btif_av.tle_av_delay_start_cmd");
}

/*******************************************************************************
**
** Function         btif_media_av_delay_start_cmd_hdlr
**
** Description      The call back function to ongoing do A2DP_CTRL_CMD_START
**
** Returns          void
**
*******************************************************************************/
void BtAvServiceDelayTimer::btif_media_av_delay_start_cmd_hdlr(void* data) {
  LOG_INFO(LOG_TAG, "%s start ", __func__);
  if (NULL != data) {
    LOG_INFO(LOG_TAG, "%s Stop timer.", __func__);
    alarm_cancel((alarm_t*)data);
  }
  // Don't send START request to stack while we are in a call
  if (!bluetooth::headset::IsCallIdle()) {
    LOG(ERROR) << __func__ << ": call state is busy";
    if (bluetooth::audio::a2dp::is_hal_2_0_enabled()) {
      bluetooth::audio::a2dp::ack_stream_started(A2DP_CTRL_ACK_INCALL_FAILURE);
    } else if (btif_av_is_a2dp_offload_enabled()) {
      btif_a2dp_audio_on_started(A2DP_CTRL_ACK_INCALL_FAILURE);
       } else {
      btif_a2dp_command_ack(A2DP_CTRL_ACK_INCALL_FAILURE);
    }
    return;
  }

  if (btif_av_stream_ready()) {
    if (!bluetooth::audio::a2dp::is_hal_2_0_enabled())
      btif_av_uipc_open_wrapper(UIPC_CH_ID_AV_AUDIO, BtAvServiceDelayTimer::btif_a2dp_data_cb_);
    btif_av_stream_start();
    if (btif_av_get_peer_sep() == AVDT_TSEP_SRC) {
      if (bluetooth::audio::a2dp::is_hal_2_0_enabled()) {
        bluetooth::audio::a2dp::ack_stream_started(A2DP_CTRL_ACK_SUCCESS);
      } else if (btif_av_is_a2dp_offload_enabled()) {
        btif_a2dp_audio_on_started(A2DP_CTRL_ACK_SUCCESS);
      } else {
        btif_a2dp_command_ack(A2DP_CTRL_ACK_SUCCESS);
      }
    }
    return;
  }

  if (btif_av_stream_started_ready()) {
    if (!bluetooth::audio::a2dp::is_hal_2_0_enabled())
      btif_av_uipc_open_wrapper(UIPC_CH_ID_AV_AUDIO, BtAvServiceDelayTimer::btif_a2dp_data_cb_);
    if (bluetooth::audio::a2dp::is_hal_2_0_enabled()) {
      bluetooth::audio::a2dp::ack_stream_started(A2DP_CTRL_ACK_SUCCESS);
    } else if (btif_av_is_a2dp_offload_enabled()) {
      btif_a2dp_audio_on_started(A2DP_CTRL_ACK_SUCCESS);
    } else {
      btif_a2dp_command_ack(A2DP_CTRL_ACK_SUCCESS);
    }
    return;
  }
  LOG_WARN(LOG_TAG,
           "%s: A2DP command A2DP_CTRL_CMD_START while AV stream is not ready",
           __func__);
  if (bluetooth::audio::a2dp::is_hal_2_0_enabled()) {
    bluetooth::audio::a2dp::ack_stream_started(A2DP_CTRL_ACK_FAILURE);
  } else if (btif_av_is_a2dp_offload_enabled()) {
    btif_a2dp_audio_on_started(A2DP_CTRL_ACK_FAILURE);
  } else {
    btif_a2dp_command_ack(A2DP_CTRL_ACK_FAILURE);
  }
  LOG_INFO(LOG_TAG, "%s DONE", __func__);
}

/*******************************************************************************
**
** Function         btif_av_is_black_peer_for_delay_start
**
** Description     Call it to check peer device. Return TRUE and schedule delay
**                      timer when it blacklisted device, else return FALSE
**
** Returns          bool
**
*******************************************************************************/
bool BtAvServiceDelayTimer::btif_av_is_black_peer_for_delay_start(
    const RawAddress& peer_addr) {
  LOG_INFO(LOG_TAG, "%s", __func__);
  if (sizeof(peer_addr) != 0) {
    if (interop_mtk_match_addr_name(INTEROP_MTK_A2DP_DELAY_START_CMD,
                                    (const RawAddress*)&peer_addr)) {
      alarm_set_on_mloop(tle_av_delay_start_cmd,
                         BTIF_TIMER_A2DP_DELAY_START_CMD,
                         btif_media_av_delay_start_cmd_hdlr,
                         tle_av_delay_start_cmd);
      LOG_INFO(LOG_TAG, "%s return true", __func__);
      return true;
    }
  }
  return false;
}

#endif
