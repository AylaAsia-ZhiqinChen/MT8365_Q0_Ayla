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

#pragma once

#include <stdint.h>
#include "mdroid_buildcfg.h"
#include "raw_address.h"

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
extern void btif_av_change_pkt_len_callback(uint8_t len, uint8_t* p);
#endif



#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
#include "uipc.h"

typedef void (*BtifA2dpDataCallback)(tUIPC_CH_ID ch_id, tUIPC_EVENT event);

class BtAvServiceDelayTimer {
 public:
  BtAvServiceDelayTimer() = default;
  ~BtAvServiceDelayTimer() = default;
  static struct tUIPC_STATE* a2dp_uipc;

  /*******************************************************************************
  **
  ** Function         btif_create_av_domain_timer
  **
  ** Description     Call it on enable AV service to create delay timer
  **
  ** Returns          void
  **
  *******************************************************************************/
  static void btif_create_av_domain_timer();

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
  bool btif_av_is_black_peer_for_delay_start(const RawAddress& peer_addr);

  /*******************************************************************************
  **
  ** Function         btif_a2dp_data_init
  **
  ** Description     Static function to set init prameters.
  **
  ** Returns          void
  **
  *******************************************************************************/
  static void btif_a2dp_data_init(BtifA2dpDataCallback cb) {
    BtAvServiceDelayTimer::btif_a2dp_data_cb_ = cb;
  }

 private:
  static void btif_media_av_delay_start_cmd_hdlr(void* data);
  static BtifA2dpDataCallback btif_a2dp_data_cb_;
};

#endif
