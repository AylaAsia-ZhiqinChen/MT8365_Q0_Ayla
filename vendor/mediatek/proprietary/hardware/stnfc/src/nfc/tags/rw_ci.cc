/******************************************************************************
 *
 *  Copyright (C) 2010-2014 Broadcom Corporation
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
 *  This file contains the implementation for Type 4 tag in Reader/Writer
 *  mode.
 *
 ******************************************************************************/
#include <string.h>
#include "bt_types.h"
#include "nfc_target.h"

#include "gki.h"
#include "nfc_api.h"
#include "nfc_int.h"
#include "rw_api.h"
#include "rw_int.h"
#include "tags_int.h"
#include <android-base/stringprintf.h>
#include <base/logging.h>

/* main state */
#define RW_CI_STATE_NOT_ACTIVATED \
  0x00                          /* T4T is not activated                 */
#define RW_CI_STATE_IDLE 0x01   /* waiting for upper layer API          */
#define RW_CI_STATE_ATTRIB 0x02 /* Get ATTRIB and UID                   */
#define RW_CI_STATE_UID 0x03    /* Get ATTRIB and UID                   */
#define RW_CI_STATE_PRESENCE_CHECK \
  0x04 /* checking presence of tag             */

#if (BT_TRACE_VERBOSE == true)
static char* rw_ci_get_state_name(uint8_t state);
#endif

static bool rw_ci_send_to_lower(NFC_HDR* p_c_apdu);

static void rw_ci_handle_error(tNFC_STATUS status, uint8_t sw1, uint8_t sw2);
static void rw_ci_data_cback(uint8_t conn_id, tNFC_CONN_EVT event,
                             tNFC_CONN* p_data);
static void rw_ci_send_uid(void);

using android::base::StringPrintf;
extern bool nfc_debug_enabled;

/*******************************************************************************
**
** Function         rw_ci_send_to_lower
**
** Description      Send C-APDU to lower layer
**
** Returns          true if success
**
*******************************************************************************/
static bool rw_ci_send_to_lower(NFC_HDR* p_c_apdu) {
  //#if (BT_TRACE_PROTOCOL == true)
  //    DispRWT4Tags (p_c_apdu, false);
  //#endif

  if (NFC_SendData(NFC_RF_CONN_ID, p_c_apdu) != NFC_STATUS_OK) {
    LOG(ERROR) << StringPrintf(
        "rw_ci_send_to_lower (): NFC_SendData () failed");
    return false;
  }

  nfc_start_quick_timer(&rw_cb.tcb.ci.timer, NFC_TTYPE_RW_CI_RESPONSE,
                        (RW_CI_TOUT_RESP * QUICK_TIMER_TICKS_PER_SEC) / 1000);

  return true;
}

/*******************************************************************************
**
** Function         rw_ci_handle_error
**
** Description      notify error to application and clean up
**
** Returns          none
**
*******************************************************************************/
static void rw_ci_handle_error(tNFC_STATUS status, uint8_t sw1, uint8_t sw2) {
  tRW_CI_CB* p_ci = &rw_cb.tcb.ci;
  tRW_DATA rw_data;
  tRW_EVENT event = NFC_STATUS_OK;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "rw_ci_handle_error (): status:0x%02X, sw1:0x%02X, sw2:0x%02X, "
      "state:0x%X",
      status, sw1, sw2, p_ci->state);

  nfc_stop_quick_timer(&p_ci->timer);

  if (rw_cb.p_cback) {
    rw_data.status = status;

    //        rw_data.t4t_sw.sw1    = sw1;
    //        rw_data.t4t_sw.sw2    = sw2;

    switch (p_ci->state) {
      case RW_CI_STATE_ATTRIB:
        //            event = RW_T4T_NDEF_DETECT_EVT;
        if (status == NFC_STATUS_TIMEOUT) {
          // Maybe not Chinese Id card, maybe ChengShiTong
          // Report card anyway
          rw_data.status = NFC_STATUS_OK;
          rw_data.ci_info.mbi = 0;
          memset(rw_data.ci_info.uid, 0, sizeof(rw_data.ci_info.uid));
          event = RW_CI_CPLT_EVT;
        } else {
          event = RW_CI_INTF_ERROR_EVT;
        }
        break;

      case RW_CI_STATE_UID:
        //            event = RW_T4T_NDEF_READ_FAIL_EVT;
        if (status == NFC_STATUS_TIMEOUT) {
          event = RW_CI_INTF_ERROR_EVT;
        }
        break;

      case RW_CI_STATE_PRESENCE_CHECK:
        event = RW_CI_PRESENCE_CHECK_EVT;
        rw_data.status = NFC_STATUS_FAILED;
        break;

      default:
        event = RW_CI_MAX_EVT;
        break;
    }

    p_ci->state = RW_CI_STATE_IDLE;

    if (event != RW_CI_MAX_EVT) {
      (*(rw_cb.p_cback))(event, &rw_data);
    }
  } else {
    p_ci->state = RW_CI_STATE_IDLE;
  }
}

/*******************************************************************************
**
** Function         rw_ci_process_timeout
**
** Description      process timeout event
**
** Returns          none
**
*******************************************************************************/
void rw_ci_process_timeout(TIMER_LIST_ENT* p_tle) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("rw_ci_process_timeout () event=%d", p_tle->event);

  if (p_tle->event == NFC_TTYPE_RW_CI_RESPONSE) {
    rw_ci_handle_error(NFC_STATUS_TIMEOUT, 0, 0);
  } else {
    LOG(ERROR) << StringPrintf("rw_ci_process_timeout () unknown event=%d",
                               p_tle->event);
  }
}

/*******************************************************************************
**
** Function         rw_ci_data_cback
**
** Description      This callback function receives the data from NFCC.
**
** Returns          none
**
*******************************************************************************/
static void rw_ci_data_cback(__attribute__((unused)) uint8_t conn_id,
                             tNFC_CONN_EVT event, tNFC_CONN* p_data) {
  tRW_CI_CB* p_ci = &rw_cb.tcb.ci;
  NFC_HDR* p_r_apdu;
  tRW_DATA rw_data;
  tRW_DATA evt_data;
  uint8_t *p, *p_sw;

#if (BT_TRACE_VERBOSE == true)
  uint8_t begin_state = p_ci->state;
#endif

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("rw_ci_data_cback () event = 0x%X", event);

#ifdef ST21NFC
  // Case data is fragmented, do not stop time until complete frame received
  if (event != NFC_DATA_START_CEVT) {
    nfc_stop_quick_timer(&p_ci->timer);
  }
#else
  nfc_stop_quick_timer(&p_ci->timer);
#endif

  switch (event) {
    case NFC_DEACTIVATE_CEVT:
      NFC_SetStaticRfCback(nullptr);
      p_ci->state = RW_CI_STATE_NOT_ACTIVATED;
      return;

    case NFC_ERROR_CEVT:
      if (p_ci->state == RW_CI_STATE_PRESENCE_CHECK) {
        p_ci->state = RW_CI_STATE_IDLE;
        rw_data.status = NFC_STATUS_FAILED;
        (*(rw_cb.p_cback))(RW_CI_PRESENCE_CHECK_EVT, &rw_data);
      } else if (p_ci->state != RW_CI_STATE_IDLE) {
        rw_ci_handle_error(rw_data.status, 0, 0);
      } else {
        p_ci->state = RW_CI_STATE_IDLE;
        rw_data.status = (tNFC_STATUS)(*(uint8_t*)p_data);
        (*(rw_cb.p_cback))(RW_CI_INTF_ERROR_EVT, &rw_data);
      }
      return;

    case NFC_DATA_CEVT:
      p_r_apdu = (NFC_HDR*)p_data->data.p_data;
      p = (uint8_t*)(p_r_apdu + 1) + p_r_apdu->offset;
      break;

    default:
      return;
  }

    //#if (BT_TRACE_PROTOCOL == true)
    //    if (p_ci->state != RW_CI_STATE_IDLE)
    //        DispRWT4Tags (p_r_apdu, true);
    //#endif

#if (BT_TRACE_VERBOSE == true)
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "RW CI state: <%s (%d)>", rw_ci_get_state_name(p_ci->state), p_ci->state);
#else
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s - RW CI state: %d", __func__, p_ci->state);
#endif

  switch (p_ci->state) {
    case RW_CI_STATE_IDLE:
/* Unexpected R-APDU, it should be raw frame response */
/* forward to upper layer without parsing */
#if (BT_TRACE_VERBOSE == true)
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s - RW CI Raw Frame: Len [0x%X] Status [%s]", __func__,
          p_r_apdu->len, NFC_GetStatusName(p_data->data.status));
#else
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s - RW CI Raw Frame: Len [0x%X] Status [0x%X]",
                          __func__, p_r_apdu->len, p_data->data.status);
#endif
      if (rw_cb.p_cback) {
        rw_data.raw_frame.status = p_data->data.status;
        rw_data.raw_frame.p_data = p_r_apdu;
        // rw_data.raw_frame.p_data->len -= 1; //Last byte is status
        (*(rw_cb.p_cback))(RW_CI_RAW_FRAME_EVT, &rw_data);
        p_r_apdu = nullptr;
      } else {
        GKI_freebuf(p_r_apdu);
      }
      break;

    case RW_CI_STATE_ATTRIB:
      memcpy(p_ci->attrib_res, p, sizeof(p_ci->attrib_res));
      GKI_freebuf(p_r_apdu);
      rw_ci_send_uid();
      break;

    case RW_CI_STATE_UID: {
      uint16_t status_words;

      p_r_apdu->len -= 1;  // Last byte is status
      p_sw = (uint8_t*)(p_r_apdu + 1) + p_r_apdu->offset;
      p_sw += (p_r_apdu->len - T4T_RSP_STATUS_WORDS_SIZE);
      BE_STREAM_TO_UINT16(status_words, p_sw);

      if (status_words == T4T_RSP_CMD_CMPLTED) {
        memcpy(p_ci->uid, p, sizeof(p_ci->uid));
      }

      GKI_freebuf(p_r_apdu);

      p_ci->state = RW_CI_STATE_IDLE;

      evt_data.status = NFC_STATUS_OK;
      evt_data.ci_info.mbi = p_ci->attrib_res[0] & 0xF;
      memcpy(evt_data.ci_info.uid, p_ci->uid, sizeof(p_ci->uid));
      (*rw_cb.p_cback)(RW_CI_CPLT_EVT, (tRW_DATA*)&evt_data);
    } break;

    case RW_CI_STATE_PRESENCE_CHECK:
      /* if any response, send presence check with ok */
      rw_data.status = NFC_STATUS_OK;
      p_ci->state = RW_CI_STATE_IDLE;
      (*(rw_cb.p_cback))(RW_CI_PRESENCE_CHECK_EVT, &rw_data);
      GKI_freebuf(p_r_apdu);
      break;

    default:
      LOG(ERROR) << StringPrintf("rw_ci_data_cback (): invalid state=%d",
                                 p_ci->state);
      GKI_freebuf(p_r_apdu);
      break;
  }

#if (BT_TRACE_VERBOSE == true)
  if (begin_state != p_ci->state) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s - RW CI state changed:<%s> -> <%s>", __func__,
        rw_ci_get_state_name(begin_state), rw_ci_get_state_name(p_ci->state));
  }
#endif
}

/*******************************************************************************
**
** Function         rw_ci_select
**
** Description      This function send Select command for Chinese Id card.
**
** Returns          NFC_STATUS_OK if success
**
*******************************************************************************/
tNFC_STATUS rw_ci_select(void) {
  tRW_CI_CB* p_ci = &rw_cb.tcb.ci;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("rw_ci_select ()");

  NFC_SetStaticRfCback(rw_ci_data_cback);

  p_ci->state = RW_CI_STATE_IDLE;

  return NFC_STATUS_OK;
}

/*****************************************************************************
**
** Function         RW_CiPresenceCheck
**
** Description
**      Check if the tag is still in the field.
**
**      The RW_CI_PRESENCE_CHECK_EVT w/ status is used to indicate presence
**      or non-presence.
**      option is RW_T4T_CHK_EMPTY_I_BLOCK, use empty I block for presence
*check.
**
** Returns
**      NFC_STATUS_OK, if raw data frame sent
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_CiPresenceCheck(void) {
  tNFC_STATUS retval = NFC_STATUS_OK;
  tRW_DATA evt_data;
  bool status;
  NFC_HDR* p_data;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("RW_CiPresenceCheck () ");

  /* If RW_SelectTagType was not called (no conn_callback) return failure */
  if (!rw_cb.p_cback) {
    retval = NFC_STATUS_FAILED;
  }
  /* If command is pending, assume tag is still present */
  else if (rw_cb.tcb.ci.state != RW_CI_STATE_IDLE) {
    evt_data.status = NFC_STATUS_OK;
    (*rw_cb.p_cback)(RW_CI_PRESENCE_CHECK_EVT, &evt_data);
  } else {
    status = false;
    /* use empty I block for presence check */
    if ((p_data = (NFC_HDR*)GKI_getbuf(NCI_MSG_OFFSET_SIZE +
                                       NCI_DATA_HDR_SIZE)) != nullptr) {
      p_data->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
      p = (uint8_t*)(p_data + 1) + p_data->offset;
      UINT8_TO_STREAM(p, 0xB2);
      p_data->len = 1;
      if (NFC_SendData(NFC_RF_CONN_ID, (NFC_HDR*)p_data) == NFC_STATUS_OK)
        status = true;
    }

    if (status == true) {
      rw_cb.tcb.ci.state = RW_CI_STATE_PRESENCE_CHECK;
    } else {
      retval = NFC_STATUS_NO_BUFFERS;
    }
  }

  return (retval);
}

/*****************************************************************************
**
** Function         RW_CiSendAttrib
**
** Description
**       Send the Attrib to the Endpoint.
**
** Returns
**      NFC_STATUS_OK, if raw data frame sent
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
tNFC_STATUS RW_CiSendAttrib(void) {
  NFC_HDR* p_c_apdu;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("RW_CiSendAttrib ()");

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("RW_CiSendAttrib (): Cannot allocate buffer");
    return false;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, 0x1D);
  UINT8_TO_BE_STREAM(p, 0x00);
  UINT8_TO_BE_STREAM(p, 0x00);
  UINT8_TO_BE_STREAM(p, 0x00);
  UINT8_TO_BE_STREAM(p, 0x00);
  UINT8_TO_BE_STREAM(p, 0x00);
  UINT8_TO_BE_STREAM(p, 0x08);
  UINT8_TO_BE_STREAM(p, 0x01);
  UINT8_TO_BE_STREAM(p, 0x08);

  p_c_apdu->len = 9;

  if (!rw_ci_send_to_lower(p_c_apdu)) {
    return false;
  }

  rw_cb.tcb.ci.state = RW_CI_STATE_ATTRIB;

  return true;
}

/*****************************************************************************
**
** Function         RW_CiSendUid
**
** Description
**      Check if the tag is still in the field.
**
**      The RW_CI_PRESENCE_CHECK_EVT w/ status is used to indicate presence
**      or non-presence.
**      option is RW_T4T_CHK_EMPTY_I_BLOCK, use empty I block for presence
*check.
**
** Returns
**      NFC_STATUS_OK, if raw data frame sent
**      NFC_STATUS_NO_BUFFERS: unable to allocate a buffer for this operation
**      NFC_STATUS_FAILED: other error
**
*****************************************************************************/
static void rw_ci_send_uid(void) {
  NFC_HDR* p_c_apdu;
  uint8_t* p;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "RW_CiSendUid () - ATTRIB_RES = %02X", rw_cb.tcb.ci.attrib_res[0]);

  p_c_apdu = (NFC_HDR*)GKI_getpoolbuf(NFC_RW_POOL_ID);

  if (!p_c_apdu) {
    LOG(ERROR) << StringPrintf("RW_CiSendUid (): Cannot allocate buffer");
    return;
  }

  p_c_apdu->offset = NCI_MSG_OFFSET_SIZE + NCI_DATA_HDR_SIZE;
  p = (uint8_t*)(p_c_apdu + 1) + p_c_apdu->offset;

  UINT8_TO_BE_STREAM(p, 0x00);
  UINT8_TO_BE_STREAM(p, 0x36);
  UINT8_TO_BE_STREAM(p, 0x00);
  UINT8_TO_BE_STREAM(p, 0x00);
  UINT8_TO_BE_STREAM(p, rw_cb.tcb.ci.attrib_res[0]);

  p_c_apdu->len = 5;

  if (!rw_ci_send_to_lower(p_c_apdu)) {
    return;
  }

  rw_cb.tcb.ci.state = RW_CI_STATE_UID;

  return;
}

#if (BT_TRACE_VERBOSE == true)
/*******************************************************************************
**
** Function         rw_ci_get_state_name
**
** Description      This function returns the state name.
**
** NOTE             conditionally compiled to save memory.
**
** Returns          pointer to the name
**
*******************************************************************************/
static char* rw_ci_get_state_name(uint8_t state) {
  switch (state) {
    case RW_CI_STATE_NOT_ACTIVATED:
      return ("NOT_ACTIVATED");
    case RW_CI_STATE_IDLE:
      return ("IDLE");
    case RW_CI_STATE_ATTRIB:
      return ("ATTRIB");
    case RW_CI_STATE_UID:
      return ("UID");
    case RW_CI_STATE_PRESENCE_CHECK:
      return ("PRESENCE_CHECK");

    default:
      return ("???? UNKNOWN STATE");
  }
}

#endif
