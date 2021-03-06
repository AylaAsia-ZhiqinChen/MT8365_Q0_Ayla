/******************************************************************************
 *
 *  Copyright 2004-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  This file contains functions for managing the SCO connection used in AG.
 *
 ******************************************************************************/

#include <cstddef>

#include "bt_common.h"
#include "bta_ag_api.h"
#include "bta_ag_int.h"
#include "bta_api.h"
#include "btm_api.h"
#include "device/include/controller.h"
#include "device/include/esco_parameters.h"
#include "osi/include/osi.h"
#include "stack/include/btu.h"
#include "utl.h"
#include "bta_dm_api.h"

/** M: New Feature for setting volume support to property. @{ */
#include <cutils/properties.h>
/** @ } */

/* Codec negotiation timeout */
#ifndef BTA_AG_CODEC_NEGOTIATION_ORI_TIMEOUT_MS
#define BTA_AG_CODEC_NEGOTIATION_ORI_TIMEOUT_MS (3 * 1000) /* 3 seconds */
#endif

/* Codec negotiation timeout */
#ifndef BTA_AG_CODEC_NEGOTIATION_TIMEOUT_MS
/** M: Change the codec negotiation timeout from 3s to 15s @{ */
#define BTA_AG_CODEC_NEGOTIATION_TIMEOUT_MS (15 * 1000) /* 15 seconds */
/** @} */
#endif

#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
#include "mediatek/include/interop_mtk.h"
#endif

static bool sco_allowed = true;
static RawAddress active_device_addr = {};

/** M: Modify for SCO retry @{ */
static bool retry_with_sco_only = false; /* indicator to try with SCO only when eSCO fails */
/** @} */

/* sco events */
enum {
  BTA_AG_SCO_LISTEN_E,       /* listen request */
  BTA_AG_SCO_OPEN_E,         /* open request */
  BTA_AG_SCO_XFER_E,         /* transfer request */
  BTA_AG_SCO_CN_DONE_E, /* codec negotiation done */
  BTA_AG_SCO_REOPEN_E,  /* Retry with other codec when failed */
  BTA_AG_SCO_CLOSE_E,      /* close request */
  BTA_AG_SCO_SHUTDOWN_E,   /* shutdown request */
  BTA_AG_SCO_CONN_OPEN_E,  /* sco open */
  BTA_AG_SCO_CONN_CLOSE_E, /* sco closed */
};

#define CASE_RETURN_STR(const) \
  case const:                  \
    return #const;

static const char* bta_ag_sco_evt_str(uint8_t event) {
  switch (event) {
    CASE_RETURN_STR(BTA_AG_SCO_LISTEN_E)
    CASE_RETURN_STR(BTA_AG_SCO_OPEN_E)
    CASE_RETURN_STR(BTA_AG_SCO_XFER_E)
    CASE_RETURN_STR(BTA_AG_SCO_CN_DONE_E)
    CASE_RETURN_STR(BTA_AG_SCO_REOPEN_E)
    CASE_RETURN_STR(BTA_AG_SCO_CLOSE_E)
    CASE_RETURN_STR(BTA_AG_SCO_SHUTDOWN_E)
    CASE_RETURN_STR(BTA_AG_SCO_CONN_OPEN_E)
    CASE_RETURN_STR(BTA_AG_SCO_CONN_CLOSE_E)
    default:
      return "Unknown SCO Event";
  }
}

static const char* bta_ag_sco_state_str(uint8_t state) {
  switch (state) {
    CASE_RETURN_STR(BTA_AG_SCO_SHUTDOWN_ST)
    CASE_RETURN_STR(BTA_AG_SCO_LISTEN_ST)
    CASE_RETURN_STR(BTA_AG_SCO_CODEC_ST)
    CASE_RETURN_STR(BTA_AG_SCO_OPENING_ST)
    CASE_RETURN_STR(BTA_AG_SCO_OPEN_CL_ST)
    CASE_RETURN_STR(BTA_AG_SCO_OPEN_XFER_ST)
    CASE_RETURN_STR(BTA_AG_SCO_OPEN_ST)
    CASE_RETURN_STR(BTA_AG_SCO_CLOSING_ST)
    CASE_RETURN_STR(BTA_AG_SCO_CLOSE_OP_ST)
    CASE_RETURN_STR(BTA_AG_SCO_CLOSE_XFER_ST)
    CASE_RETURN_STR(BTA_AG_SCO_SHUTTING_ST)
    default:
      return "Unknown SCO State";
  }
}

/**
 * Check if bd_addr is the current active device.
 *
 * @param bd_addr target device address
 * @return True if bd_addr is the current active device, False otherwise or if
 * no active device is set (i.e. active_device_addr is empty)
 */
bool bta_ag_sco_is_active_device(const RawAddress& bd_addr) {
  return !active_device_addr.IsEmpty() && active_device_addr == bd_addr;
}

static void bta_ag_create_pending_sco(tBTA_AG_SCB* p_scb, bool is_local);

/*******************************************************************************
 *
 * Function         bta_ag_sco_conn_cback
 *
 * Description      BTM SCO connection callback.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
static void bta_ag_sco_conn_cback(uint16_t sco_idx) {
  uint16_t handle;
  tBTA_AG_SCB* p_scb;

  /* match callback to scb; first check current sco scb */
  if (bta_ag_cb.sco.p_curr_scb != nullptr && bta_ag_cb.sco.p_curr_scb->in_use) {
    handle = bta_ag_scb_to_idx(bta_ag_cb.sco.p_curr_scb);
  }
  /* then check for scb connected to this peer */
  else {
    /* Check if SLC is up */
    handle = bta_ag_idx_by_bdaddr(BTM_ReadScoBdAddr(sco_idx));
    p_scb = bta_ag_scb_by_idx(handle);
    if (p_scb && !p_scb->svc_conn) handle = 0;
  }

  if (handle != 0) {
    do_in_main_thread(FROM_HERE,
                      base::Bind(&bta_ag_sm_execute_by_handle, handle,
                                 BTA_AG_SCO_OPEN_EVT, tBTA_AG_DATA::kEmpty));
  } else {
    /* no match found; disconnect sco, init sco variables */
    bta_ag_cb.sco.p_curr_scb = nullptr;
    bta_ag_cb.sco.state = BTA_AG_SCO_SHUTDOWN_ST;
    BTM_RemoveSco(sco_idx);
  }
}

/*******************************************************************************
 *
 * Function         bta_ag_sco_disc_cback
 *
 * Description      BTM SCO disconnection callback.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
static void bta_ag_sco_disc_cback(uint16_t sco_idx) {
  uint16_t handle = 0;

  APPL_TRACE_DEBUG(
      "bta_ag_sco_disc_cback(): sco_idx: 0x%x  p_cur_scb: 0x%08x  sco.state: "
      "%d",
      sco_idx, bta_ag_cb.sco.p_curr_scb, bta_ag_cb.sco.state);

  APPL_TRACE_DEBUG(
      "bta_ag_sco_disc_cback(): scb[0] addr: 0x%08x  in_use: %u  sco_idx: 0x%x "
      " sco state: %u",
      &bta_ag_cb.scb[0], bta_ag_cb.scb[0].in_use, bta_ag_cb.scb[0].sco_idx,
      bta_ag_cb.scb[0].state);
  APPL_TRACE_DEBUG(
      "bta_ag_sco_disc_cback(): scb[1] addr: 0x%08x  in_use: %u  sco_idx: 0x%x "
      " sco state: %u",
      &bta_ag_cb.scb[1], bta_ag_cb.scb[1].in_use, bta_ag_cb.scb[1].sco_idx,
      bta_ag_cb.scb[1].state);

  /* match callback to scb */
  if (bta_ag_cb.sco.p_curr_scb != nullptr && bta_ag_cb.sco.p_curr_scb->in_use) {
    /* We only care about callbacks for the active SCO */
    if (bta_ag_cb.sco.p_curr_scb->sco_idx != sco_idx) {
      if (bta_ag_cb.sco.p_curr_scb->sco_idx != 0xFFFF) return;
    }
    handle = bta_ag_scb_to_idx(bta_ag_cb.sco.p_curr_scb);
  }

  if (handle != 0) {

    /* Restore settings */
    if (bta_ag_cb.sco.p_curr_scb->inuse_codec == BTA_AG_CODEC_MSBC) {
      /* Bypass vendor specific and voice settings if enhanced eSCO supported */
      if (!(controller_get_interface()
                ->supports_enhanced_setup_synchronous_connection())) {
        BTM_WriteVoiceSettings(BTM_VOICE_SETTING_CVSD);
      }

      /* If SCO open was initiated by AG and failed for mSBC T2, try mSBC T1
       * 'Safe setting' first. If T1 also fails, try CVSD */
      if (bta_ag_sco_is_opening(bta_ag_cb.sco.p_curr_scb)) {
        bta_ag_cb.sco.p_curr_scb->state = BTA_AG_SCO_CODEC_ST;
        if (bta_ag_cb.sco.p_curr_scb->codec_msbc_settings ==
            BTA_AG_SCO_MSBC_SETTINGS_T2) {
          APPL_TRACE_WARNING(
              "%s: eSCO/SCO failed to open, falling back to mSBC T1 settings",
              __func__);
          bta_ag_cb.sco.p_curr_scb->codec_msbc_settings =
              BTA_AG_SCO_MSBC_SETTINGS_T1;
        } else {
          APPL_TRACE_WARNING(
              "%s: eSCO/SCO failed to open, falling back to CVSD", __func__);
          bta_ag_cb.sco.p_curr_scb->codec_fallback = true;
        }
      }
    } else if (bta_ag_sco_is_opening(bta_ag_cb.sco.p_curr_scb)) {
      APPL_TRACE_ERROR("%s: eSCO/SCO failed to open, no more fall back",
                       __func__);
    }

    bta_ag_cb.sco.p_curr_scb->inuse_codec = BTA_AG_CODEC_NONE;

    do_in_main_thread(FROM_HERE,
                      base::Bind(&bta_ag_sm_execute_by_handle, handle,
                                 BTA_AG_SCO_CLOSE_EVT, tBTA_AG_DATA::kEmpty));
  } else {
    /* no match found */
    APPL_TRACE_DEBUG("no scb for ag_sco_disc_cback");

    /* sco could be closed after scb dealloc'ed */
    if (bta_ag_cb.sco.p_curr_scb != nullptr) {
      bta_ag_cb.sco.p_curr_scb->sco_idx = BTM_INVALID_SCO_INDEX;
      bta_ag_cb.sco.p_curr_scb = nullptr;
      bta_ag_cb.sco.state = BTA_AG_SCO_SHUTDOWN_ST;
    }
  }
}

/*******************************************************************************
 *
 * Function         bta_ag_remove_sco
 *
 * Description      Removes the specified SCO from the system.
 *                  If only_active is true, then SCO is only removed if
 *                  connected
 *
 * Returns          bool   - true if SCO removal was started
 *
 ******************************************************************************/
static bool bta_ag_remove_sco(tBTA_AG_SCB* p_scb, bool only_active) {
  if (p_scb->sco_idx != BTM_INVALID_SCO_INDEX) {
    if (!only_active || p_scb->sco_idx == bta_ag_cb.sco.cur_idx) {
      tBTM_STATUS status = BTM_ERR_PROCESSING;

      APPL_TRACE_DEBUG("%s: Switch device %s to Active mode", __func__,
                     p_scb->peer_addr.ToString().c_str());
      bta_dm_pm_active(p_scb->peer_addr);
      status = BTM_RemoveSco(p_scb->sco_idx);
      APPL_TRACE_DEBUG("%s: SCO index 0x%04x, status %d", __func__,
                       p_scb->sco_idx, status);
      if (status == BTM_CMD_STARTED) {
        /* SCO is connected; set current control block */
        bta_ag_cb.sco.p_curr_scb = p_scb;
        return true;
      } else if ((status == BTM_SUCCESS) || (status == BTM_UNKNOWN_ADDR)) {
        /* If no connection reset the SCO handle */
        p_scb->sco_idx = BTM_INVALID_SCO_INDEX;
      }
    }
  }
  return false;
}

/*******************************************************************************
 *
 * Function         bta_ag_esco_connreq_cback
 *
 * Description      BTM eSCO connection requests and eSCO change requests
 *                  Only the connection requests are processed by BTA.
 *
 * Returns          void
 *
 ******************************************************************************/
static void bta_ag_esco_connreq_cback(tBTM_ESCO_EVT event,
                                      tBTM_ESCO_EVT_DATA* p_data) {
  /* Only process connection requests */
  if (event == BTM_ESCO_CONN_REQ_EVT) {
    uint16_t sco_inx = p_data->conn_evt.sco_inx;
    const RawAddress* remote_bda = BTM_ReadScoBdAddr(sco_inx);
    tBTA_AG_SCB* p_scb = bta_ag_scb_by_idx(bta_ag_idx_by_bdaddr(remote_bda));
    if (remote_bda && bta_ag_sco_is_active_device(*remote_bda) && p_scb &&
        p_scb->svc_conn) {
      p_scb->sco_idx = sco_inx;

      /* If no other SCO active, allow this one */
      if (!bta_ag_cb.sco.p_curr_scb) {
        APPL_TRACE_EVENT("%s: Accept Conn Request (sco_inx 0x%04x)", __func__,
                         sco_inx);
        bta_ag_sco_conn_rsp(p_scb, &p_data->conn_evt);

        bta_ag_cb.sco.state = BTA_AG_SCO_OPENING_ST;
        bta_ag_cb.sco.p_curr_scb = p_scb;
        bta_ag_cb.sco.cur_idx = p_scb->sco_idx;
      } else {
        /* Begin a transfer: Close current SCO before responding */
        APPL_TRACE_DEBUG("bta_ag_esco_connreq_cback: Begin XFER");
        bta_ag_cb.sco.p_xfer_scb = p_scb;
        bta_ag_cb.sco.conn_data = p_data->conn_evt;
        bta_ag_cb.sco.state = BTA_AG_SCO_OPEN_XFER_ST;

        if (!bta_ag_remove_sco(bta_ag_cb.sco.p_curr_scb, true)) {
          APPL_TRACE_ERROR(
              "%s: Nothing to remove,so accept Conn Request(sco_inx 0x%04x)",
              __func__, sco_inx);
          bta_ag_cb.sco.p_xfer_scb = nullptr;
          bta_ag_cb.sco.state = BTA_AG_SCO_LISTEN_ST;

          bta_ag_sco_conn_rsp(p_scb, &p_data->conn_evt);
        }
      }
    } else {
      LOG(WARNING) << __func__
                   << ": reject incoming SCO connection, remote_bda="
                   << (remote_bda ? *remote_bda : RawAddress::kEmpty)
                   << ", active_bda=" << active_device_addr << ", current_bda="
                   << (p_scb ? p_scb->peer_addr : RawAddress::kEmpty);
      BTM_EScoConnRsp(p_data->conn_evt.sco_inx, HCI_ERR_HOST_REJECT_RESOURCES,
                      (enh_esco_params_t*)nullptr);
    }
  } else if (event == BTM_ESCO_CHG_EVT) {
    /* Received a change in the esco link */
    APPL_TRACE_EVENT(
        "%s: eSCO change event (inx %d): rtrans %d, "
        "rxlen %d, txlen %d, txint %d",
        __func__, p_data->chg_evt.sco_inx, p_data->chg_evt.retrans_window,
        p_data->chg_evt.rx_pkt_len, p_data->chg_evt.tx_pkt_len,
        p_data->chg_evt.tx_interval);
  }
}

/*******************************************************************************
 *
 * Function         bta_ag_cback_sco
 *
 * Description      Call application callback function with SCO event.
 *
 *
 * Returns          void
 *
 ******************************************************************************/
static void bta_ag_cback_sco(tBTA_AG_SCB* p_scb, uint8_t event) {
  tBTA_AG_HDR sco = {};
  sco.handle = bta_ag_scb_to_idx(p_scb);
  sco.app_id = p_scb->app_id;
  /* call close cback */
  (*bta_ag_cb.p_cback)(event, (tBTA_AG*)&sco);
}

/*******************************************************************************
 *
 * Function         bta_ag_create_sco
 *
 * Description      Create a SCO connection for a given control block
 *                  p_scb : Pointer to the target AG control block
 *                  is_orig : Whether to initiate or listen for SCO connection
 *
 * Returns          void
 *
 ******************************************************************************/
static void bta_ag_create_sco(tBTA_AG_SCB* p_scb, bool is_orig) {
  APPL_TRACE_DEBUG(
      "%s: BEFORE codec_updated=%d, codec_fallback=%d, "
      "sco_codec=%d, peer_codec=%d, msbc_settings=%d, device=%s",
      __func__, p_scb->codec_updated, p_scb->codec_fallback, p_scb->sco_codec,
      p_scb->peer_codecs, p_scb->codec_msbc_settings,
      p_scb->peer_addr.ToString().c_str());
  tBTA_AG_PEER_CODEC esco_codec = BTA_AG_CODEC_CVSD;

  if (!bta_ag_sco_is_active_device(p_scb->peer_addr)) {
    LOG(WARNING) << __func__ << ": device " << p_scb->peer_addr
                 << " is not active, active_device=" << active_device_addr;
    if (bta_ag_cb.sco.p_curr_scb != nullptr &&
        bta_ag_cb.sco.p_curr_scb->in_use && p_scb == bta_ag_cb.sco.p_curr_scb) {
      do_in_main_thread(
          FROM_HERE, base::Bind(&bta_ag_sm_execute, p_scb, BTA_AG_SCO_CLOSE_EVT,
                                tBTA_AG_DATA::kEmpty));
    }
    return;
  }
  /* Make sure this SCO handle is not already in use */
  if (p_scb->sco_idx != BTM_INVALID_SCO_INDEX) {
    APPL_TRACE_ERROR("%s: device %s, index 0x%04x already in use!", __func__,
                     p_scb->peer_addr.ToString().c_str(), p_scb->sco_idx);
    return;
  }

#if (DISABLE_WBS == FALSE)
  /** M: Modify for SCO retry @{ */
  if ((p_scb->sco_codec == BTA_AG_CODEC_MSBC) && !p_scb->codec_fallback && !retry_with_sco_only)
  /** @} */
    esco_codec = BTA_AG_CODEC_MSBC;
#endif

  if (p_scb->codec_fallback) {
    p_scb->codec_fallback = false;
    /* Force AG to send +BCS for the next audio connection. */
    p_scb->codec_updated = true;
    /* Reset mSBC settings to T2 for the next audio connection */
    p_scb->codec_msbc_settings = BTA_AG_SCO_MSBC_SETTINGS_T2;
  }

  esco_codec_t codec_index = ESCO_CODEC_CVSD;
  /* If WBS included, use CVSD by default, index is 0 for CVSD by
   * initialization. If eSCO codec is mSBC, index is T2 or T1 */
  if (esco_codec == BTA_AG_CODEC_MSBC) {
    if (p_scb->codec_msbc_settings == BTA_AG_SCO_MSBC_SETTINGS_T2) {
      codec_index = ESCO_CODEC_MSBC_T2;
    } else {
      codec_index = ESCO_CODEC_MSBC_T1;
    }
  }

  /* Initialize eSCO parameters */
  enh_esco_params_t params = esco_parameters_for_codec(codec_index);
  /* For CVSD */
  if (esco_codec == BTM_SCO_CODEC_CVSD) {
    /* Use the applicable packet types
      (3-EV3 not allowed due to errata 2363) */
    params.packet_types =
        p_bta_ag_cfg->sco_pkt_types | ESCO_PKT_TYPES_MASK_NO_3_EV3;
    if ((!(p_scb->features & BTA_AG_FEAT_ESCO)) ||
        (!(p_scb->peer_features & BTA_AG_PEER_FEAT_ESCO))) {
      params.max_latency_ms = 10;
      params.retransmission_effort = ESCO_RETRANSMISSION_POWER;
    }
    /** M: Modify for SCO retry @{ */
    if(retry_with_sco_only ==  false) {
      retry_with_sco_only = true;
    } else {
      params.retransmission_effort = ESCO_RETRANSMISSION_OFF;
      retry_with_sco_only = false;
    }
  } else {
    retry_with_sco_only = false;
  }
  /** @} */

  /* If initiating, setup parameters to start SCO/eSCO connection */
  if (is_orig) {
    bta_ag_cb.sco.is_local = true;
    /* Set eSCO Mode */
    BTM_SetEScoMode(&params);
    bta_ag_cb.sco.p_curr_scb = p_scb;
    /* save the current codec as sco_codec can be updated while SCO is open. */
    p_scb->inuse_codec = esco_codec;

    /* tell sys to stop av if any */
    bta_sys_sco_use(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);

    /* Send pending commands to create SCO connection to peer */
    bta_ag_create_pending_sco(p_scb, bta_ag_cb.sco.is_local);
    APPL_TRACE_API("%s: orig %d, inx 0x%04x, pkt types 0x%04x", __func__,
                   is_orig, p_scb->sco_idx, params.packet_types);
  } else {
    /** M: Modify for SCO retry @{ */
    retry_with_sco_only = false;
    /** @} */
    /* Not initiating, go to listen mode */
    tBTM_STATUS status = BTM_CreateSco(
        &p_scb->peer_addr, false, params.packet_types, &p_scb->sco_idx,
        bta_ag_sco_conn_cback, bta_ag_sco_disc_cback);
    if (status == BTM_CMD_STARTED) {
      BTM_RegForEScoEvts(p_scb->sco_idx, bta_ag_esco_connreq_cback);
    }

    APPL_TRACE_API("%s: orig %d, inx 0x%04x, status 0x%x, pkt types 0x%04x",
                   __func__, is_orig, p_scb->sco_idx, status,
                   params.packet_types);
  }
  APPL_TRACE_DEBUG(
      "%s: AFTER codec_updated=%d, codec_fallback=%d, "
      "sco_codec=%d, peer_codec=%d, msbc_settings=%d, device=%s",
      __func__, p_scb->codec_updated, p_scb->codec_fallback, p_scb->sco_codec,
      p_scb->peer_codecs, p_scb->codec_msbc_settings,
      p_scb->peer_addr.ToString().c_str());
}

/*******************************************************************************
 *
 * Function         bta_ag_create_pending_sco
 *
 * Description      This Function is called after the pre-SCO vendor setup is
 *                  done for the BTA to continue and send the HCI Commands for
 *                  creating/accepting SCO connection with peer based on the
 *                  is_local parameter.
 *
 * Returns          void
 *
 ******************************************************************************/
static void bta_ag_create_pending_sco(tBTA_AG_SCB* p_scb, bool is_local) {
  tBTA_AG_PEER_CODEC esco_codec = p_scb->inuse_codec;
  enh_esco_params_t params = {};
  bta_ag_cb.sco.p_curr_scb = p_scb;
  bta_ag_cb.sco.cur_idx = p_scb->sco_idx;

  /* Local device requested SCO connection to peer */
  if (is_local) {
    if (esco_codec == BTA_AG_CODEC_MSBC) {
      if (p_scb->codec_msbc_settings == BTA_AG_SCO_MSBC_SETTINGS_T2) {
        params = esco_parameters_for_codec(ESCO_CODEC_MSBC_T2);
      } else
        params = esco_parameters_for_codec(ESCO_CODEC_MSBC_T1);
    } else {
      params = esco_parameters_for_codec(ESCO_CODEC_CVSD);
      if ((!(p_scb->features & BTA_AG_FEAT_ESCO)) ||
          (!(p_scb->peer_features & BTA_AG_PEER_FEAT_ESCO))) {
        params.max_latency_ms = 10;
        params.retransmission_effort = ESCO_RETRANSMISSION_POWER;
      }
    }

    /* Bypass voice settings if enhanced SCO setup command is supported */
    if (!(controller_get_interface()
              ->supports_enhanced_setup_synchronous_connection())) {
      if (esco_codec == BTA_AG_CODEC_MSBC) {
        BTM_WriteVoiceSettings(BTM_VOICE_SETTING_TRANS);
      } else {
        BTM_WriteVoiceSettings(BTM_VOICE_SETTING_CVSD);
      }
    }

    if (BTM_CreateSco(&p_scb->peer_addr, true, params.packet_types,
                      &p_scb->sco_idx, bta_ag_sco_conn_cback,
                      bta_ag_sco_disc_cback) == BTM_CMD_STARTED) {
      /* Initiating the connection, set the current sco handle */
      bta_ag_cb.sco.cur_idx = p_scb->sco_idx;
    }
    APPL_TRACE_DEBUG("%s: initiated SCO connection", __func__);
  } else {
    /* Local device accepted SCO connection from peer */
    params = esco_parameters_for_codec(ESCO_CODEC_CVSD);
    if ((!(p_scb->features & BTA_AG_FEAT_ESCO)) ||
        (!(p_scb->peer_features & BTA_AG_PEER_FEAT_ESCO))) {
      params.max_latency_ms = 10;
      params.retransmission_effort = ESCO_RETRANSMISSION_POWER;
    }

    BTM_EScoConnRsp(p_scb->sco_idx, HCI_SUCCESS, &params);
    APPL_TRACE_DEBUG("%s: listening for SCO connection", __func__);
  }
}

/*******************************************************************************
 *
 * Function         bta_ag_codec_negotiation_timer_cback
 *
 * Description
 *
 *
 * Returns          void
 *
 ******************************************************************************/
static void bta_ag_codec_negotiation_timer_cback(void* data) {
  APPL_TRACE_DEBUG("%s", __func__);
  tBTA_AG_SCB* p_scb = (tBTA_AG_SCB*)data;

  /* Announce that codec negotiation failed. */
  bta_ag_sco_codec_nego(p_scb, false);

  /* call app callback */
  bta_ag_cback_sco(p_scb, BTA_AG_AUDIO_CLOSE_EVT);
}

/*******************************************************************************
 *
 * Function         bta_ag_codec_negotiate
 *
 * Description      Initiate codec negotiation by sending AT command.
 *                  If not necessary, skip negotiation.
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_ag_codec_negotiate(tBTA_AG_SCB* p_scb) {
  APPL_TRACE_DEBUG("%s", __func__);
  bta_ag_cb.sco.p_curr_scb = p_scb;
  uint8_t* p_rem_feat = BTM_ReadRemoteFeatures(p_scb->peer_addr);
  bool sdp_wbs_support = p_scb->peer_sdp_features & BTA_AG_FEAT_WBS_SUPPORT;

  if (p_rem_feat == nullptr) {
    LOG(WARNING) << __func__
                 << ": Fail to read remote feature, skip codec negotiation";
    bta_ag_sco_codec_nego(p_scb, false);
    return;
  }

  // Workaround for misbehaving HFs, which indicate which one is not support on
  // Transparent Synchronous Data in Remote Supported Features, WBS in SDP and
  // and Codec Negotiation in BRSF. Fluoride will assume CVSD codec by default.
  // In Sony XAV AX100 car kit and Sony MW600 Headset case, which indicate
  // Transparent Synchronous Data and WBS support, but no codec negotiation
  // support, using mSBC codec can result background noise or no audio.
  // In Skullcandy JIB case, which indicate WBS and codec negotiation support,
  // but no Transparent Synchronous Data support, using mSBC codec can result
  // SCO setup fail by Firmware reject.
  if (!HCI_LMP_TRANSPNT_SUPPORTED(p_rem_feat) || !sdp_wbs_support ||
      !(p_scb->peer_features & BTA_AG_PEER_FEAT_CODEC)) {
    p_scb->sco_codec = UUID_CODEC_CVSD;
  }

  if ((p_scb->codec_updated || p_scb->codec_fallback) &&
      (p_scb->peer_features & BTA_AG_PEER_FEAT_CODEC)) {
    /* Change the power mode to Active until SCO open is completed. */
    bta_sys_busy(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);

    /** M: Force to use SCO. @{ */
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
    uint8_t lmp_ver = 0;
    uint16_t lmp_subver = 0;
    uint16_t mfct_set = 0;
    bool is_whitelist = false;

    BTM_ReadRemoteVersion(p_scb->peer_addr, &lmp_ver,
            &mfct_set, &lmp_subver);
    //Realtek Semiconductor Corporation(93)
    //Airoha Technology Corp. (148)
    if (mfct_set == 93 || mfct_set == 148)
    {
      APPL_TRACE_DEBUG("need use eSco");
    }

    //Xradio (0x063d)
    if( (lmp_ver == 7) && (mfct_set == 1597) && ((lmp_subver == 0x0d04) || (lmp_subver == 0x0d07)|| (lmp_subver == 0x0d03)) )
    {
        is_whitelist = true;
        APPL_TRACE_DEBUG("need use eSco for Xradio device");
    }

    //RDA Microelectronics (97)
    if( (lmp_ver == 7) && (mfct_set == 97) && (lmp_subver == 20) )
    {
        is_whitelist = true;
        APPL_TRACE_DEBUG("need use eSco for RDA device (i-ball)");
    }

    if ((interop_mtk_match_addr_name(INTEROP_MTK_HFP_FORCE_TO_USE_SCO,
        &(p_scb->peer_addr)) ||interop_mtk_match_addr_name(
        INTEROP_MTK_HFP_FORCE_TO_USE_CVSD, &(p_scb->peer_addr)))
        && (mfct_set != 93 && mfct_set != 148)
        && !is_whitelist) {
      p_scb->sco_codec = BTA_AG_CODEC_CVSD;
    }
#endif
    /** @} */

    /* Send +BCS to the peer */
    bta_ag_send_bcs(p_scb);

/** M: Use the original negotiation timeout @{ */
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
    if (interop_mtk_match_addr_name(INTEROP_MTK_HFP_USE_ORIGINAL_TIMEOUT,
            &(p_scb->peer_addr)) ||interop_mtk_match_addr_name(
            INTEROP_MTK_HFP_USE_ORIGINAL_TIMEOUT, &(p_scb->peer_addr))){
        APPL_TRACE_DEBUG("Set Negotiation timeout be 3s");
        /* Start timer to handle timeout */
        alarm_set_on_mloop(p_scb->codec_negotiation_timer,
        BTA_AG_CODEC_NEGOTIATION_ORI_TIMEOUT_MS,
        bta_ag_codec_negotiation_timer_cback, p_scb);
    }else
#endif
    /** @} */

    {
    /* Start timer to handle timeout */
    alarm_set_on_mloop(p_scb->codec_negotiation_timer,
                       BTA_AG_CODEC_NEGOTIATION_TIMEOUT_MS,
                       bta_ag_codec_negotiation_timer_cback, p_scb);
    }
  } else {
    /* use same codec type as previous SCO connection, skip codec negotiation */
    APPL_TRACE_DEBUG(
        "use same codec type as previous SCO connection,skip codec "
        "negotiation");
    bta_ag_sco_codec_nego(p_scb, true);
  }
}

/*******************************************************************************
 *
 * Function         bta_ag_sco_event
 *
 * Description
 *
 *
 * Returns          void
 *
 ******************************************************************************/
static void bta_ag_sco_event(tBTA_AG_SCB* p_scb, uint8_t event) {
  tBTA_AG_SCO_CB* p_sco = &bta_ag_cb.sco;
  uint8_t previous_state = p_sco->state;
  APPL_TRACE_EVENT("%s: index=0x%04x, device=%s, state=%s[%d], event=%s[%d]",
                   __func__, p_scb->sco_idx,
                   p_scb->peer_addr.ToString().c_str(),
                   bta_ag_sco_state_str(p_sco->state), p_sco->state,
                   bta_ag_sco_evt_str(event), event);

  switch (p_sco->state) {
    case BTA_AG_SCO_SHUTDOWN_ST:
      switch (event) {
        case BTA_AG_SCO_LISTEN_E:
          /* create sco listen connection */
          bta_ag_create_sco(p_scb, false);
          p_sco->state = BTA_AG_SCO_LISTEN_ST;
          break;

        default:
          APPL_TRACE_WARNING(
              "%s: BTA_AG_SCO_SHUTDOWN_ST: Ignoring event %s[%d]", __func__,
              bta_ag_sco_evt_str(event), event);
          break;
      }
      break;

    case BTA_AG_SCO_LISTEN_ST:
      switch (event) {
        case BTA_AG_SCO_LISTEN_E:
          /* create sco listen connection (Additional channel) */
          bta_ag_create_sco(p_scb, false);
          break;

        case BTA_AG_SCO_OPEN_E:
          /* remove listening connection */
          bta_ag_remove_sco(p_scb, false);

#if (DISABLE_WBS == FALSE)
          /* start codec negotiation */
          p_sco->state = BTA_AG_SCO_CODEC_ST;
          bta_ag_codec_negotiate(p_scb);
#else
          bta_ag_create_sco(p_scb, true);
          p_sco->state = BTA_AG_SCO_OPENING_ST;
#endif
          break;

        case BTA_AG_SCO_SHUTDOWN_E:
          /* remove listening connection */
          bta_ag_remove_sco(p_scb, false);

          if (p_scb == p_sco->p_curr_scb) p_sco->p_curr_scb = nullptr;

          /* If last SCO instance then finish shutting down */
          if (!bta_ag_other_scb_open(p_scb)) {
            p_sco->state = BTA_AG_SCO_SHUTDOWN_ST;
          }
          break;

        case BTA_AG_SCO_CLOSE_E:
          /* remove listening connection */
          /* Ignore the event. Keep listening SCO for the active SLC
           */
          APPL_TRACE_WARNING("%s: BTA_AG_SCO_LISTEN_ST: Ignoring event %s[%d]",
                             __func__, bta_ag_sco_evt_str(event), event);
          break;

        case BTA_AG_SCO_CONN_CLOSE_E:
          /* sco failed; create sco listen connection */
          bta_ag_create_sco(p_scb, false);
          p_sco->state = BTA_AG_SCO_LISTEN_ST;
          break;

        default:
          APPL_TRACE_WARNING("%s: BTA_AG_SCO_LISTEN_ST: Ignoring event %s[%d]",
                             __func__, bta_ag_sco_evt_str(event), event);
          break;
      }
      break;

    case BTA_AG_SCO_CODEC_ST:
      switch (event) {
        case BTA_AG_SCO_LISTEN_E:
          /* create sco listen connection (Additional channel) */
          bta_ag_create_sco(p_scb, false);
          break;

        case BTA_AG_SCO_CN_DONE_E:
          /* create sco connection to peer */
          bta_ag_create_sco(p_scb, true);
          p_sco->state = BTA_AG_SCO_OPENING_ST;
          break;

        case BTA_AG_SCO_XFER_E:
          /* save xfer scb */
          p_sco->p_xfer_scb = p_scb;
          p_sco->state = BTA_AG_SCO_CLOSE_XFER_ST;
          break;

        case BTA_AG_SCO_SHUTDOWN_E:
          /* remove listening connection */
          bta_ag_remove_sco(p_scb, false);

          if (p_scb == p_sco->p_curr_scb) p_sco->p_curr_scb = nullptr;

          /* If last SCO instance then finish shutting down */
          if (!bta_ag_other_scb_open(p_scb)) {
            p_sco->state = BTA_AG_SCO_SHUTDOWN_ST;
          }
        /** M: Modify for codec negotiation failed @{ */
        else
        {
            APPL_TRACE_ERROR("%s: Failed for index 0x%04x, device %s", __func__,
                     p_scb->sco_idx, p_scb->peer_addr.ToString().c_str());
            /* sco open is not started yet. just go back to listening */
            p_sco->state = BTA_AG_SCO_LISTEN_ST;
        }
        /** @} */
          break;

        case BTA_AG_SCO_CLOSE_E:
          /* sco open is not started yet. just go back to listening */
          p_sco->state = BTA_AG_SCO_LISTEN_ST;
          break;

        case BTA_AG_SCO_CONN_CLOSE_E:
          /* sco failed; create sco listen connection */
          bta_ag_create_sco(p_scb, false);
          p_sco->state = BTA_AG_SCO_LISTEN_ST;
          break;

        /** M: Recovery the sco state machine @{ */
#if defined(MTK_INTEROP_EXTENSION) && (MTK_INTEROP_EXTENSION == TRUE)
        case BTA_AG_SCO_OPEN_E:
            if (interop_mtk_match_addr_name(INTEROP_MTK_HFP_SCO_RECOVERY,
                    &(p_scb->peer_addr)) ||interop_mtk_match_addr_name(
                    INTEROP_MTK_HFP_SCO_RECOVERY, &(p_scb->peer_addr))){
                APPL_TRACE_DEBUG("Restart codec negotiation");
                bta_ag_codec_negotiate(p_scb);
                break;
            }
#endif
        /** @} */

        default:
          APPL_TRACE_WARNING("%s: BTA_AG_SCO_CODEC_ST: Ignoring event %s[%d]",
                             __func__, bta_ag_sco_evt_str(event), event);
          break;
      }
      break;

    case BTA_AG_SCO_OPENING_ST:
      switch (event) {
        case BTA_AG_SCO_LISTEN_E:
          /* second headset has now joined */
          /* create sco listen connection (Additional channel) */
          if (p_scb != p_sco->p_curr_scb) {
            bta_ag_create_sco(p_scb, false);
          }
          break;

#if (DISABLE_WBS == FALSE)
        case BTA_AG_SCO_REOPEN_E:
          /* start codec negotiation */
          p_sco->state = BTA_AG_SCO_CODEC_ST;
          bta_ag_codec_negotiate(p_scb);
          break;
#endif

        case BTA_AG_SCO_XFER_E:
          /* save xfer scb */
          p_sco->p_xfer_scb = p_scb;
          p_sco->state = BTA_AG_SCO_CLOSE_XFER_ST;
          break;

        case BTA_AG_SCO_CLOSE_E:
          p_sco->state = BTA_AG_SCO_OPEN_CL_ST;
          break;

        case BTA_AG_SCO_SHUTDOWN_E:
          /* If not opening scb, just close it */
          if (p_scb != p_sco->p_curr_scb) {
            /* remove listening connection */
            bta_ag_remove_sco(p_scb, false);
          } else {
            /** M: Bug Fix for ag sco state machine @{ */
            if (!bta_ag_other_scb_open(p_scb))
              p_sco->state = BTA_AG_SCO_SHUTDOWN_ST;
            else
              p_sco->state = BTA_AG_SCO_LISTEN_ST;
            /** @} */
          }
          break;

        case BTA_AG_SCO_CONN_OPEN_E:
          p_sco->state = BTA_AG_SCO_OPEN_ST;
          break;

        case BTA_AG_SCO_CONN_CLOSE_E:
          /* sco failed; create sco listen connection */
          bta_ag_create_sco(p_scb, false);
          p_sco->state = BTA_AG_SCO_LISTEN_ST;
          break;

        default:
          APPL_TRACE_WARNING("%s: BTA_AG_SCO_OPENING_ST: Ignoring event %s[%d]",
                             __func__, bta_ag_sco_evt_str(event), event);
          break;
      }
      break;

    case BTA_AG_SCO_OPEN_CL_ST:
      switch (event) {
        case BTA_AG_SCO_XFER_E:
          /* save xfer scb */
          p_sco->p_xfer_scb = p_scb;

          p_sco->state = BTA_AG_SCO_CLOSE_XFER_ST;
          break;

        case BTA_AG_SCO_OPEN_E:
          p_sco->state = BTA_AG_SCO_OPENING_ST;
          break;

        case BTA_AG_SCO_SHUTDOWN_E:
          /* If not opening scb, just close it */
          if (p_scb != p_sco->p_curr_scb) {
            /* remove listening connection */
            bta_ag_remove_sco(p_scb, false);
          } else
            p_sco->state = BTA_AG_SCO_SHUTTING_ST;

          break;

        case BTA_AG_SCO_CONN_OPEN_E:
          /* close sco connection */
          bta_ag_remove_sco(p_scb, true);

          p_sco->state = BTA_AG_SCO_CLOSING_ST;
          break;

        case BTA_AG_SCO_CONN_CLOSE_E:
          /* sco failed; create sco listen connection */

          p_sco->state = BTA_AG_SCO_LISTEN_ST;
          break;

        default:
          APPL_TRACE_WARNING("%s: BTA_AG_SCO_OPEN_CL_ST: Ignoring event %s[%d]",
                             __func__, bta_ag_sco_evt_str(event), event);
          break;
      }
      break;

    case BTA_AG_SCO_OPEN_XFER_ST:
      switch (event) {
        case BTA_AG_SCO_CLOSE_E:
          /* close sco connection */
          bta_ag_remove_sco(p_scb, true);

          p_sco->state = BTA_AG_SCO_CLOSING_ST;
          break;

        case BTA_AG_SCO_SHUTDOWN_E:
          /* remove all connection */
          bta_ag_remove_sco(p_scb, false);
          p_sco->state = BTA_AG_SCO_SHUTTING_ST;

          break;

        case BTA_AG_SCO_CONN_CLOSE_E:
          /* closed sco; place in listen mode and
             accept the transferred connection */
          bta_ag_create_sco(p_scb, false); /* Back into listen mode */

          /* Accept sco connection with xfer scb */
          bta_ag_sco_conn_rsp(p_sco->p_xfer_scb, &p_sco->conn_data);
          p_sco->state = BTA_AG_SCO_OPENING_ST;
          p_sco->p_curr_scb = p_sco->p_xfer_scb;
          p_sco->cur_idx = p_sco->p_xfer_scb->sco_idx;
          p_sco->p_xfer_scb = nullptr;
          break;

        default:
          APPL_TRACE_WARNING(
              "%s: BTA_AG_SCO_OPEN_XFER_ST: Ignoring event %s[%d]", __func__,
              bta_ag_sco_evt_str(event), event);
          break;
      }
      break;

    case BTA_AG_SCO_OPEN_ST:
      switch (event) {
        case BTA_AG_SCO_LISTEN_E:
          /* second headset has now joined */
          /* create sco listen connection (Additional channel) */
          if (p_scb != p_sco->p_curr_scb) {
            bta_ag_create_sco(p_scb, false);
          }
          break;

        case BTA_AG_SCO_XFER_E:
          /* close current sco connection */
          bta_ag_remove_sco(p_sco->p_curr_scb, true);

          /* save xfer scb */
          p_sco->p_xfer_scb = p_scb;

          p_sco->state = BTA_AG_SCO_CLOSE_XFER_ST;
          break;

        case BTA_AG_SCO_CLOSE_E:
          /* close sco connection if active */
          if (bta_ag_remove_sco(p_scb, true)) {
            p_sco->state = BTA_AG_SCO_CLOSING_ST;
          }
          break;

        case BTA_AG_SCO_SHUTDOWN_E:
          /* remove all listening connections */
          bta_ag_remove_sco(p_scb, false);

          /* If SCO was active on this scb, close it */
          if (p_scb == p_sco->p_curr_scb) {
            p_sco->state = BTA_AG_SCO_SHUTTING_ST;
          }
          break;

        case BTA_AG_SCO_CONN_CLOSE_E:
          /* peer closed sco; create sco listen connection */
          bta_ag_create_sco(p_scb, false);
          p_sco->state = BTA_AG_SCO_LISTEN_ST;
          break;

        default:
          APPL_TRACE_WARNING("%s: BTA_AG_SCO_OPEN_ST: Ignoring event %s[%d]",
                             __func__, bta_ag_sco_evt_str(event), event);
          break;
      }
      break;

    case BTA_AG_SCO_CLOSING_ST:
      switch (event) {
        case BTA_AG_SCO_LISTEN_E:
          /* create sco listen connection (Additional channel) */
          if (p_scb != p_sco->p_curr_scb) {
            bta_ag_create_sco(p_scb, false);
          }
          break;

        case BTA_AG_SCO_OPEN_E:
          p_sco->state = BTA_AG_SCO_CLOSE_OP_ST;
          break;

        case BTA_AG_SCO_XFER_E:
          /* save xfer scb */
          p_sco->p_xfer_scb = p_scb;

          p_sco->state = BTA_AG_SCO_CLOSE_XFER_ST;
          break;

        case BTA_AG_SCO_SHUTDOWN_E:
          /* If not closing scb, just close it */
          if (p_scb != p_sco->p_curr_scb) {
            /* remove listening connection */
            bta_ag_remove_sco(p_scb, false);
          } else
            p_sco->state = BTA_AG_SCO_SHUTTING_ST;

          break;

        case BTA_AG_SCO_CONN_CLOSE_E:
          /* peer closed sco; create sco listen connection */
          bta_ag_create_sco(p_scb, false);

          p_sco->state = BTA_AG_SCO_LISTEN_ST;
          break;

        default:
          APPL_TRACE_WARNING("%s: BTA_AG_SCO_CLOSING_ST: Ignoring event %s[%d]",
                             __func__, bta_ag_sco_evt_str(event), event);
          break;
      }
      break;

    case BTA_AG_SCO_CLOSE_OP_ST:
      switch (event) {
        case BTA_AG_SCO_CLOSE_E:
          p_sco->state = BTA_AG_SCO_CLOSING_ST;
          break;

        case BTA_AG_SCO_SHUTDOWN_E:
          p_sco->state = BTA_AG_SCO_SHUTTING_ST;
          break;

        case BTA_AG_SCO_CONN_CLOSE_E:
          /* start codec negotiation */
          p_sco->state = BTA_AG_SCO_CODEC_ST;
          bta_ag_codec_negotiate(p_scb);
          break;

        case BTA_AG_SCO_LISTEN_E:
          /* create sco listen connection (Additional channel) */
          if (p_scb != p_sco->p_curr_scb) {
            bta_ag_create_sco(p_scb, false);
          }
          break;

        default:
          APPL_TRACE_WARNING(
              "%s: BTA_AG_SCO_CLOSE_OP_ST: Ignoring event %s[%d]", __func__,
              bta_ag_sco_evt_str(event), event);
          break;
      }
      break;

    case BTA_AG_SCO_CLOSE_XFER_ST:
      switch (event) {
        case BTA_AG_SCO_CONN_OPEN_E:
          /* close sco connection so headset can be transferred
             Probably entered this state from "opening state" */
          bta_ag_remove_sco(p_scb, true);
          break;

        case BTA_AG_SCO_CLOSE_E:
          /* clear xfer scb */
          p_sco->p_xfer_scb = nullptr;

          p_sco->state = BTA_AG_SCO_CLOSING_ST;
          break;

        case BTA_AG_SCO_SHUTDOWN_E:
          /* clear xfer scb */
          p_sco->p_xfer_scb = nullptr;

          p_sco->state = BTA_AG_SCO_SHUTTING_ST;
          break;

        case BTA_AG_SCO_CONN_CLOSE_E: {
          /* closed sco; place old sco in listen mode,
             take current sco out of listen, and
             create originating sco for current */
          bta_ag_create_sco(p_scb, false);
          bta_ag_remove_sco(p_sco->p_xfer_scb, false);

#if (DISABLE_WBS == FALSE)
          /* start codec negotiation */
          p_sco->state = BTA_AG_SCO_CODEC_ST;
          tBTA_AG_SCB* p_cn_scb = p_sco->p_xfer_scb;
          p_sco->p_xfer_scb = nullptr;
          bta_ag_codec_negotiate(p_cn_scb);
#else
          /* create sco connection to peer */
          bta_ag_create_sco(p_sco->p_xfer_scb, true);
          p_sco->p_xfer_scb = nullptr;
          p_sco->state = BTA_AG_SCO_OPENING_ST;
#endif
          break;
        }

        default:
          APPL_TRACE_WARNING(
              "%s: BTA_AG_SCO_CLOSE_XFER_ST: Ignoring event %s[%d]", __func__,
              bta_ag_sco_evt_str(event), event);
          break;
      }
      break;

    case BTA_AG_SCO_SHUTTING_ST:
      switch (event) {
        case BTA_AG_SCO_CONN_OPEN_E:
          /* close sco connection; wait for conn close event */
          bta_ag_remove_sco(p_scb, true);
          break;

        case BTA_AG_SCO_CONN_CLOSE_E:
          /* If last SCO instance then finish shutting down */
          if (!bta_ag_other_scb_open(p_scb)) {
            p_sco->state = BTA_AG_SCO_SHUTDOWN_ST;
          } else /* Other instance is still listening */
          {
            p_sco->state = BTA_AG_SCO_LISTEN_ST;
          }

          /* If SCO closed for other HS which is not being disconnected,
             then create listen sco connection for it as scb still open */
          if (bta_ag_scb_open(p_scb)) {
            bta_ag_create_sco(p_scb, false);
            p_sco->state = BTA_AG_SCO_LISTEN_ST;
          }

          if (p_scb == p_sco->p_curr_scb) {
            p_sco->p_curr_scb->sco_idx = BTM_INVALID_SCO_INDEX;
            p_sco->p_curr_scb = nullptr;
          }
          break;

        case BTA_AG_SCO_LISTEN_E:
          /* create sco listen connection (Additional channel) */
          if (p_scb != p_sco->p_curr_scb) {
            bta_ag_create_sco(p_scb, false);
          }
          break;

        case BTA_AG_SCO_SHUTDOWN_E:
          if (!bta_ag_other_scb_open(p_scb)) {
            p_sco->state = BTA_AG_SCO_SHUTDOWN_ST;
          } else /* Other instance is still listening */
          {
            p_sco->state = BTA_AG_SCO_LISTEN_ST;
          }

          if (p_scb == p_sco->p_curr_scb) {
            p_sco->p_curr_scb->sco_idx = BTM_INVALID_SCO_INDEX;
            p_sco->p_curr_scb = nullptr;
          }
          break;

        default:
          APPL_TRACE_WARNING(
              "%s: BTA_AG_SCO_SHUTTING_ST: Ignoring event %s[%d]", __func__,
              bta_ag_sco_evt_str(event), event);
          break;
      }
      break;

    default:
      break;
  }
  if (p_sco->state != previous_state) {
    APPL_TRACE_EVENT(
        "%s: SCO_state_change: [%s(0x%02x)]->[%s(0x%02x)] "
        "after event [%s(0x%02x)]",
        __func__, bta_ag_sco_state_str(previous_state), previous_state,
        bta_ag_sco_state_str(p_sco->state), p_sco->state,
        bta_ag_sco_evt_str(event), event);
  }
}

/*******************************************************************************
 *
 * Function         bta_ag_sco_is_open
 *
 * Description      Check if sco is open for this scb.
 *
 *
 * Returns          true if sco open for this scb, false otherwise.
 *
 ******************************************************************************/
bool bta_ag_sco_is_open(tBTA_AG_SCB* p_scb) {
  return ((bta_ag_cb.sco.state == BTA_AG_SCO_OPEN_ST) &&
          (bta_ag_cb.sco.p_curr_scb == p_scb));
}

/*******************************************************************************
 *
 * Function         bta_ag_sco_is_opening
 *
 * Description      Check if sco is in Opening state.
 *
 *
 * Returns          true if sco is in Opening state for this scb, false
 *                  otherwise.
 *
 ******************************************************************************/
bool bta_ag_sco_is_opening(tBTA_AG_SCB* p_scb) {
  return ((bta_ag_cb.sco.state == BTA_AG_SCO_OPENING_ST) &&
          (bta_ag_cb.sco.p_curr_scb == p_scb));
}

/*******************************************************************************
 *
 * Function         bta_ag_sco_listen
 *
 * Description
 *
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_ag_sco_listen(tBTA_AG_SCB* p_scb,
                       UNUSED_ATTR const tBTA_AG_DATA& data) {
  LOG(INFO) << __func__ << ": " << p_scb->peer_addr;
  bta_ag_sco_event(p_scb, BTA_AG_SCO_LISTEN_E);
}

/*******************************************************************************
 *
 * Function         bta_ag_sco_open
 *
 * Description
 *
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_ag_sco_open(tBTA_AG_SCB* p_scb, UNUSED_ATTR const tBTA_AG_DATA& data) {
  if (!sco_allowed) {
    LOG(INFO) << __func__ << ": not opening sco, by policy";
    return;
  }
  /* if another scb using sco, this is a transfer */
  if (bta_ag_cb.sco.p_curr_scb && bta_ag_cb.sco.p_curr_scb != p_scb) {
    LOG(INFO) << __func__ << ": tranfer " << bta_ag_cb.sco.p_curr_scb->peer_addr
              << " -> " << p_scb->peer_addr;
    bta_ag_sco_event(p_scb, BTA_AG_SCO_XFER_E);
  } else {
    /* else it is an open */
    LOG(INFO) << __func__ << ": open " << p_scb->peer_addr;
    bta_ag_sco_event(p_scb, BTA_AG_SCO_OPEN_E);
  }
}

/*******************************************************************************
 *
 * Function         bta_ag_sco_close
 *
 * Description
 *
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_ag_sco_close(tBTA_AG_SCB* p_scb,
                      UNUSED_ATTR const tBTA_AG_DATA& data) {
  /* if scb is in use */
  /* sco_idx is not allocated in SCO_CODEC_ST, still need to move to listen
   * state. */
  if ((p_scb->sco_idx != BTM_INVALID_SCO_INDEX) ||
      (bta_ag_cb.sco.state == BTA_AG_SCO_CODEC_ST)) {
    APPL_TRACE_DEBUG("bta_ag_sco_close: sco_inx = %d", p_scb->sco_idx);
    bta_ag_sco_event(p_scb, BTA_AG_SCO_CLOSE_E);
  }
}

/*******************************************************************************
 *
 * Function         bta_ag_sco_codec_nego
 *
 * Description      Handles result of eSCO codec negotiation
 *
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_ag_sco_codec_nego(tBTA_AG_SCB* p_scb, bool result) {
  if (result) {
    /* Subsequent SCO connection will skip codec negotiation */
    APPL_TRACE_DEBUG("%s: Succeeded for index 0x%04x, device %s", __func__,
                     p_scb->sco_idx, p_scb->peer_addr.ToString().c_str());
    p_scb->codec_updated = false;
    bta_ag_sco_event(p_scb, BTA_AG_SCO_CN_DONE_E);
  } else {
    /* codec negotiation failed */
    APPL_TRACE_ERROR("%s: Failed for index 0x%04x, device %s", __func__,
                     p_scb->sco_idx, p_scb->peer_addr.ToString().c_str());
    bta_ag_sco_event(p_scb, BTA_AG_SCO_CLOSE_E);
  }
}

/*******************************************************************************
 *
 * Function         bta_ag_sco_shutdown
 *
 * Description
 *
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_ag_sco_shutdown(tBTA_AG_SCB* p_scb,
                         UNUSED_ATTR const tBTA_AG_DATA& data) {
  bta_ag_sco_event(p_scb, BTA_AG_SCO_SHUTDOWN_E);
}

/*******************************************************************************
 *
 * Function         bta_ag_sco_conn_open
 *
 * Description
 *
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_ag_sco_conn_open(tBTA_AG_SCB* p_scb,
                          UNUSED_ATTR const tBTA_AG_DATA& data) {

  /** M: New Feature for setting volume support to property. @{ */
  if ((p_scb->peer_features & BTA_AG_PEER_FEAT_VOL) == 0) {
    property_set("persist.vendor.bluetooth.hfp.vol", "0");
  } else {
    property_set("persist.vendor.bluetooth.hfp.vol", "1");
  }
  /** @} */

  bta_ag_sco_event(p_scb, BTA_AG_SCO_CONN_OPEN_E);

  bta_sys_sco_open(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);

  /* call app callback */
  bta_ag_cback_sco(p_scb, BTA_AG_AUDIO_OPEN_EVT);

  /** M: Modify for SCO retry @{ */
  retry_with_sco_only = false;
  /** @} */
  /* reset to mSBC T2 settings as the preferred */
  p_scb->codec_msbc_settings = BTA_AG_SCO_MSBC_SETTINGS_T2;
}

/*******************************************************************************
 *
 * Function         bta_ag_sco_conn_close
 *
 * Description
 *
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_ag_sco_conn_close(tBTA_AG_SCB* p_scb,
                           UNUSED_ATTR const tBTA_AG_DATA& data) {
  /* clear current scb */
  bta_ag_cb.sco.p_curr_scb = nullptr;
  p_scb->sco_idx = BTM_INVALID_SCO_INDEX;

  /* codec_fallback is set when AG is initiator and connection failed for mSBC.
   * OR if codec is msbc and T2 settings failed, then retry Safe T1 settings */
  if (p_scb->svc_conn &&
      (p_scb->codec_fallback ||
       (p_scb->sco_codec == BTM_SCO_CODEC_MSBC &&
        p_scb->codec_msbc_settings == BTA_AG_SCO_MSBC_SETTINGS_T1))) {
    bta_ag_sco_event(p_scb, BTA_AG_SCO_REOPEN_E);
  /** M: Modify for SCO retry @{ */
  } else if (p_scb->svc_conn && retry_with_sco_only) {
    bta_ag_create_sco(p_scb, TRUE);
  /** @} */
  } else {
    /* Indicate if the closing of audio is because of transfer */
    bta_ag_sco_event(p_scb, BTA_AG_SCO_CONN_CLOSE_E);

    bta_sys_sco_close(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);

    /* if av got suspended by this call, let it resume. */
    /* In case call stays alive regardless of sco, av should not be affected. */
    if (((p_scb->call_ind == BTA_AG_CALL_INACTIVE) &&
         (p_scb->callsetup_ind == BTA_AG_CALLSETUP_NONE)) ||
        (p_scb->post_sco == BTA_AG_POST_SCO_CALL_END)) {
      bta_sys_sco_unuse(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);
    }

    /* call app callback */
    bta_ag_cback_sco(p_scb, BTA_AG_AUDIO_CLOSE_EVT);
    p_scb->codec_msbc_settings = BTA_AG_SCO_MSBC_SETTINGS_T2;
  }
  /** M: Modify for SCO retry @{ */
  retry_with_sco_only = false;
  /** @} */
}

/*******************************************************************************
 *
 * Function         bta_ag_sco_conn_rsp
 *
 * Description      Process the SCO connection request
 *
 *
 * Returns          void
 *
 ******************************************************************************/
void bta_ag_sco_conn_rsp(tBTA_AG_SCB* p_scb,
                         tBTM_ESCO_CONN_REQ_EVT_DATA* p_data) {
  bta_ag_cb.sco.is_local = false;

  APPL_TRACE_DEBUG("%s: eSCO %d, state %d", __func__,
                   controller_get_interface()
                       ->supports_enhanced_setup_synchronous_connection(),
                   bta_ag_cb.sco.state);

  if (bta_ag_cb.sco.state == BTA_AG_SCO_LISTEN_ST ||
      bta_ag_cb.sco.state == BTA_AG_SCO_CLOSE_XFER_ST ||
      bta_ag_cb.sco.state == BTA_AG_SCO_OPEN_XFER_ST) {
    /* tell sys to stop av if any */
    bta_sys_sco_use(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);
    /* When HS initiated SCO, it cannot be WBS. */
  }

  /* If SCO open was initiated from HS, it must be CVSD */
  p_scb->inuse_codec = BTA_AG_CODEC_NONE;
  /* Send pending commands to create SCO connection to peer */
  bta_ag_create_pending_sco(p_scb, bta_ag_cb.sco.is_local);
}

void bta_ag_set_sco_allowed(bool value) {
  sco_allowed = value;
  APPL_TRACE_DEBUG(sco_allowed ? "sco now allowed" : "sco now not allowed");
}

const RawAddress& bta_ag_get_active_device() { return active_device_addr; }

void bta_clear_active_device() { active_device_addr = RawAddress::kEmpty; }

void bta_ag_api_set_active_device(const RawAddress& new_active_device) {
  if (new_active_device.IsEmpty()) {
    APPL_TRACE_ERROR("%s: empty device", __func__);
    return;
  }
  active_device_addr = new_active_device;
}
