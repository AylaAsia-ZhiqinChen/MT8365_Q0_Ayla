/* Copyright Statement:
 * *
 * * This software/firmware and related documentation ("MediaTek Software") are
 * * protected under relevant copyright laws. The information contained herein
 * * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * * Without the prior written permission of MediaTek inc. and/or its licensors,
 * * any reproduction, modification, use or disclosure of MediaTek Software,
 * * and information contained herein, in whole or in part, shall be strictly prohibited.
 * *
 * * MediaTek Inc. (C) 2016. All rights reserved.
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


#include "stack/sdp/sdpint.h"
#include "stack/include/sdp_api.h"
#include "stack/include/l2c_api.h"

/*******************************************************************************
 *
 * Function         sdpu_get_active_ccb_cid
 *
 * Description      This function checks if any sdp connecting is there for
 *                  same remote and returns cid if its available
 *
 *                  BD_ADDR : Remote address
 *
 * Returns          returns cid if any active sdp connection,else '0'.
 *
 *******************************************************************************/
uint16_t sdpu_get_active_ccb_cid(RawAddress remote_bd_addr) {
  uint16_t xx;
  tCONN_CB* p_ccb;

  /* Look through each connection control block for active sdp on given remote */
  for (xx = 0, p_ccb = sdp_cb.ccb; xx < SDP_MAX_CONNECTIONS; xx++, p_ccb++) {
    if ((p_ccb->con_state == SDP_STATE_CONN_SETUP) ||
        (p_ccb->con_state == SDP_STATE_CFG_SETUP)  ||
        (p_ccb->con_state == SDP_STATE_CONNECTED)) {
      if ((p_ccb->device_address == remote_bd_addr) &&
          (p_ccb->con_flags & SDP_FLAGS_IS_ORIG))
        return (p_ccb->connection_id);
    }
  }

  /* If here, no active sdp channel for this remote */
  return (0);
}

/*******************************************************************************
 *
 * Function         sdpu_process_pend_ccb
 *
 * Description      This function process if any sdp ccb pending for connection
 *
 *                  uint16_t : Remote CID
 *
 * Returns          returns true if any pending ccb ,else false.
 *
 ******************************************************************************/
bool sdpu_process_pend_ccb(uint16_t cid, bool use_cur_chnl) {
  uint16_t xx;
  tCONN_CB* p_ccb;
  uint16_t new_cid;
  bool new_conn = false;

  if (use_cur_chnl) {
    /* Look through each connection control block for active sdp on given remote */
    for (xx = 0, p_ccb = sdp_cb.ccb; xx < SDP_MAX_CONNECTIONS; xx++, p_ccb++) {
      if ((p_ccb->con_state == SDP_STATE_CONN_PEND) &&
          (p_ccb->connection_id == cid) &&
          (p_ccb->con_flags & SDP_FLAGS_IS_ORIG)) {
        p_ccb->con_state = SDP_STATE_CONNECTED;
        sdp_disc_connected(p_ccb);
        return (true);
      }
    }

    /* If here, no pending sdp channel for this remote */
    return (false);
  } else {
    for (xx = 0, p_ccb = sdp_cb.ccb; xx < SDP_MAX_CONNECTIONS; xx++, p_ccb++) {
      if ((p_ccb->con_state == SDP_STATE_CONN_PEND) &&
          (p_ccb->connection_id == cid) &&
          (p_ccb->con_flags & SDP_FLAGS_IS_ORIG)) {
        if (!new_conn) {
          p_ccb->con_state = SDP_STATE_CONN_SETUP;
          new_cid = L2CA_ConnectReq(SDP_PSM, p_ccb->device_address);
          new_conn = true;
        }
        /* Check if L2CAP started the connection process */
        if (new_cid != 0)
          p_ccb->connection_id = new_cid;
        else {
          /* Tell the user if he has a callback */
          if (p_ccb->p_cb)
            (*p_ccb->p_cb)(SDP_CONN_FAILED);
          else if (p_ccb->p_cb2)
            (*p_ccb->p_cb2)(SDP_CONN_FAILED, p_ccb->user_data);
          sdpu_release_ccb(p_ccb);
        }
      }
    }
    return (new_conn && new_cid);
  }
}

/*******************************************************************************
 *
 * Function         sdpu_clear_pend_ccb
 *
 * Description      This function releases if any sdp ccb pending for connection
 *
 *                  uint16_t : Remote CID
 *
 * Returns          returns none.
 *
 ******************************************************************************/
void sdpu_clear_pend_ccb(uint16_t cid) {
  uint16_t xx;
  tCONN_CB* p_ccb;

  /* Look through each connection control block for active sdp on given remote */
  for (xx = 0, p_ccb = sdp_cb.ccb; xx < SDP_MAX_CONNECTIONS; xx++, p_ccb++) {
    if ((p_ccb->con_state == SDP_STATE_CONN_PEND) &&
        (p_ccb->connection_id == cid) &&
        (p_ccb->con_flags & SDP_FLAGS_IS_ORIG)) {
      /* Tell the user if he has a callback */
      if (p_ccb->p_cb)
        (*p_ccb->p_cb)(SDP_CONN_FAILED);
      else if (p_ccb->p_cb2)
        (*p_ccb->p_cb2)(SDP_CONN_FAILED, p_ccb->user_data);
      sdpu_release_ccb(p_ccb);
    }
  }
  return;
}

