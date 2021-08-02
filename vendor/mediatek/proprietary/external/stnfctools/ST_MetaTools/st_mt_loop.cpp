/******************************************************************************
 *
 *  Copyright (C) 2016 ST Microelectronics S.A.
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

#include "st_mtktools.h"

// This function returns an error code to the CLI.
static int reply_failure(int rsp_code) {
#define SEND_ERROR(msg_type, field_name)                                     \
  {                                                                          \
    struct mtk_msg_##msg_type resp;                                          \
    memset(&resp, 0, sizeof(resp));                                          \
    resp.hdr.cmd_le = le_to_h_32(MTK_NFC_##msg_type);                        \
    resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr)); \
    if (sizeof(resp.field_name) == 4)                                        \
      resp.field_name = le_to_h_32(MTK_RESULT_FAIL);                         \
    if (sizeof(resp.field_name) == 2)                                        \
      resp.field_name = le_to_h_16(MTK_RESULT_FAIL);                         \
    if (sizeof(resp.field_name) == 1) resp.field_name = MTK_RESULT_FAIL;     \
    return st_mt_server_send((struct mtk_msg_hdr *)&resp);                   \
  }

  LOG(ERROR) << StringPrintf("Sending failure message, cmd %d", rsp_code);

  switch (rsp_code) {
    case MTK_NFC_EM_ALS_READER_MODE_RSP: {
      SEND_ERROR(EM_ALS_READER_MODE_RSP, result_le);
    } break;

    case MTK_NFC_EM_ALS_READER_MODE_OPT_RSP: {
      SEND_ERROR(EM_ALS_READER_MODE_OPT_RSP, result_le);
    } break;

    case MTK_NFC_EM_ALS_P2P_MODE_RSP: {
      SEND_ERROR(EM_ALS_P2P_MODE_RSP, result_le);
    } break;

    case MTK_NFC_EM_ALS_CARD_MODE_RSP: {
      SEND_ERROR(EM_ALS_CARD_MODE_RSP, result_le);
    } break;

    case MTK_NFC_EM_POLLING_MODE_RSP: {
      SEND_ERROR(EM_POLLING_MODE_RSP, result_le);
    } break;

    case MTK_NFC_EM_TX_CARRIER_ALS_ON_RSP: {
      SEND_ERROR(EM_TX_CARRIER_ALS_ON_RSP, result_le);
    } break;

    case MTK_NFC_EM_VIRTUAL_CARD_RSP: {
      SEND_ERROR(EM_VIRTUAL_CARD_RSP, result_le);
    } break;

      //      case MTK_NFC_EM_PNFC_CMD_RSP:
      //      {
      //
      //      }
      //      break;

    case MTK_NFC_TESTMODE_SETTING_RSP: {
      SEND_ERROR(TESTMODE_SETTING_RSP, result);
    } break;

    case MTK_EM_LOOPBACK_TEST_RSP: {
      SEND_ERROR(EM_LOOPBACK_TEST_RSP, result);
    } break;

    case MTK_NFC_SW_VERSION_RESPONSE: {
      // No error code possible in protocol
      // just send an all 0 response
      struct mtk_msg_SW_VERSION_RESPONSE resp;
      memset(&resp, 0, sizeof(resp));
      resp.hdr.cmd_le = le_to_h_32(MTK_NFC_SW_VERSION_RESPONSE);
      resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));
      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    case MTK_NFC_FM_SWP_TEST_RSP: {
      SEND_ERROR(FM_SWP_TEST_RSP, result_le);
    } break;

      //      case MTK_NFC_FM_READ_UID_TEST_RSP:
      //      {
      //
      //      }
      //      break;

    case MTK_NFC_FM_READ_DEP_TEST_RSP: {
      SEND_ERROR(FM_READ_DEP_TEST_RSP, result_le);
    } break;

    case MTK_NFC_FM_CARD_MODE_TEST_RSP: {
      SEND_ERROR(FM_CARD_MODE_TEST_RSP, result_le);
    } break;

    case MTK_NFC_FM_VIRTUAL_CARD_RSP: {
      SEND_ERROR(FM_VIRTUAL_CARD_RSP, result_le);
    } break;

    case MTK_NFC_FM_ANTENNA_TEST_RSP: {
      SEND_ERROR(FM_ANTENNA_TEST_RSP, result_le);
    } break;

    case MTK_NFC_META_GET_SELIST_RSP: {
      SEND_ERROR(META_GET_SELIST_RSP, result_le);
    } break;

    default:
      LOG(ERROR) << StringPrintf("(Internal error) Invalid response code: %d",
                                 rsp_code);
      return 2;
  }

  return 0;
}

// Read a ndef message and write into a message structure for CLI
static void ndef_to_climsg(struct ndef_msg *msg,
                           struct mtk_ndef_read_payload *climsg) {
  LOG(INFO) << StringPrintf("%s: Read: type: %d", __FUNCTION__, msg->type);
  LOG(INFO) << StringPrintf("%s: Read: lang: %d", __FUNCTION__, msg->lang);
  LOG(INFO) << StringPrintf("%s: Read: flag: %02hhx", __FUNCTION__,
                            msg->record_flag);
  LOG(INFO) << StringPrintf("%s: Read: tnf : %02hhx", __FUNCTION__,
                            msg->record_tnf);
  for (int i = 0; i < (int)msg->len; i++) {
    LOG(INFO) << StringPrintf("%s: Read: data[%d/%d]: %02hhx  '%c'",
                              __FUNCTION__, i, msg->len, msg->data[i],
                              msg->data[i]);
  }
  memset(climsg, 0, sizeof(*climsg));
  // fill the information read from the tag.
  climsg->type_le = le_to_h_32(msg->type);
  switch (msg->lang) {
    case ndef_msg::NDEF_LN_DE:
      climsg->lang[0] = 'D';
      climsg->lang[1] = 'E';
      break;
    case ndef_msg::NDEF_LN_EN:
      climsg->lang[0] = 'E';
      climsg->lang[1] = 'N';
      break;
    case ndef_msg::NDEF_LN_FR:
      climsg->lang[0] = 'F';
      climsg->lang[1] = 'R';
      break;
    default:
      climsg->lang[0] = '?';
      climsg->lang[1] = '?';
      break;
  }
  climsg->recflag = msg->record_flag;
  memcpy(climsg->recid, msg->record_id, sizeof(msg->record_id));
  climsg->rectnf = msg->record_tnf;
  climsg->datalen_le = le_to_h_32(msg->len);
  if (msg->data) {
    memcpy(climsg->data, msg->data,
           msg->len < NDEF_DATA_LEN ? msg->len : NDEF_DATA_LEN);
    free(msg->data);
  }
}

// Ths function is called by the main thread holding the mutex when an NFC even
// has occurred.
static void reader_cb(enum rdr_ntf_type t, enum rdr_ntf_data d, int uidlen,
                      uint8_t *uid, uint8_t discotype, uint8_t datarate) {
  LOG(INFO) << StringPrintf("%s: received %d, %d", __FUNCTION__, t, d);

  if (t == RDR_NTF_FAIL) {
    LOG(ERROR) << StringPrintf("%s: Received RDR_NTF_FAIL", __FUNCTION__);

    if (st_mt_g_state->state == ST_ALS_READER) {
      struct mtk_msg_EM_ALS_READER_MODE_NTF resp;

      // send ntf
      memset(&resp, 0, sizeof(resp));
      resp.hdr.cmd_le = le_to_h_32(MTK_NFC_EM_ALS_READER_MODE_NTF);
      resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));
      resp.result_le = le_to_h_32(1 /* FAIL */);
      (void)st_mt_server_send((struct mtk_msg_hdr *)&resp);
    }
    if (st_mt_g_state->state == ST_POLLING) {
      struct mtk_msg_EM_POLLING_MODE_NTF resp;

      // send ntf
      memset(&resp, 0, sizeof(resp));
      resp.hdr.cmd_le = le_to_h_32(MTK_NFC_EM_POLLING_MODE_NTF);
      resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));
      resp.type_le = le_to_h_32(EM_ENABLE_FUNC_READER_MODE);
      resp.u.reader.result_le = le_to_h_32(1 /* FAIL */);
      (void)st_mt_server_send((struct mtk_msg_hdr *)&resp);
    }
    return;
  }

  if ((st_mt_g_state->state == ST_ALS_READER) && (t == RDR_NTF_CONNECT)) {
    struct mtk_msg_EM_ALS_READER_MODE_NTF resp;

    // send ntf
    memset(&resp, 0, sizeof(resp));
    resp.hdr.cmd_le = le_to_h_32(MTK_NFC_EM_ALS_READER_MODE_NTF);
    resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));

    resp.is_ndef_le = le_to_h_32(d);
    resp.uidlen_le = le_to_h_32(uidlen);
    if (uidlen) {
      memcpy(resp.uid, uid,
             (size_t)uidlen < sizeof(resp.uid) ? uidlen : sizeof(resp.uid));
    }
    resp.discovery_type = discotype;
    resp.bitrate = datarate;
    resp.result_le = le_to_h_32(t);
    (void)st_mt_server_send((struct mtk_msg_hdr *)&resp);

    // move state machine
    st_mt_event_chstate(ST_ALS_READER_TAG_CONNECTED);

  } else if ((st_mt_g_state->state == ST_ALS_READER_TAG_CONNECTED) &&
             (t == RDR_NTF_DISCONNECT)) {
    struct mtk_msg_EM_ALS_READER_MODE_NTF resp;

    // send ntf
    memset(&resp, 0, sizeof(resp));
    resp.hdr.cmd_le = le_to_h_32(MTK_NFC_EM_ALS_READER_MODE_NTF);
    resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));

    resp.is_ndef_le = le_to_h_32(d);
    resp.uidlen_le = le_to_h_32(uidlen);
    if (uidlen) {
      memcpy(resp.uid, uid,
             (size_t)uidlen < sizeof(resp.uid) ? uidlen : sizeof(resp.uid));
    }
    resp.result_le = le_to_h_32(t);
    (void)st_mt_server_send((struct mtk_msg_hdr *)&resp);

    // move state machine
    st_mt_event_chstate(ST_ALS_READER);

  } else if ((st_mt_g_state->state == ST_POLLING) && (t == RDR_NTF_CONNECT)) {
    struct mtk_msg_EM_POLLING_MODE_NTF resp;

    // send ntf
    memset(&resp, 0, sizeof(resp));
    resp.hdr.cmd_le = le_to_h_32(MTK_NFC_EM_POLLING_MODE_NTF);
    resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));
    resp.type_le = le_to_h_32(EM_ENABLE_FUNC_READER_MODE);

    resp.u.reader.is_ndef_le = le_to_h_32(d);
    resp.u.reader.uidlen_le = le_to_h_32(uidlen);
    if (uidlen) {
      memcpy(resp.u.reader.uid, uid,
             (size_t)uidlen < sizeof(resp.u.reader.uid)
                 ? uidlen
                 : sizeof(resp.u.reader.uid));
    }
    resp.u.reader.discovery_type = discotype;
    resp.u.reader.bitrate = datarate;
    resp.u.reader.result_le = le_to_h_32(t);
    (void)st_mt_server_send((struct mtk_msg_hdr *)&resp);

    // move state machine
    st_mt_event_chstate(ST_POLLING_TAG_CONNECTED);

  } else if ((st_mt_g_state->state == ST_POLLING_TAG_CONNECTED) &&
             (t == RDR_NTF_DISCONNECT)) {
    struct mtk_msg_EM_POLLING_MODE_NTF resp;

    // send ntf
    memset(&resp, 0, sizeof(resp));
    resp.hdr.cmd_le = le_to_h_32(MTK_NFC_EM_POLLING_MODE_NTF);
    resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));
    resp.type_le = le_to_h_32(EM_ENABLE_FUNC_READER_MODE);

    resp.u.reader.is_ndef_le = le_to_h_32(d);
    resp.u.reader.uidlen_le = le_to_h_32(uidlen);
    if (uidlen) {
      memcpy(resp.u.reader.uid, uid,
             (size_t)uidlen < sizeof(resp.u.reader.uid)
                 ? uidlen
                 : sizeof(resp.u.reader.uid));
    }
    resp.u.reader.result_le = le_to_h_32(t);
    (void)st_mt_server_send((struct mtk_msg_hdr *)&resp);

    // move state machine
    st_mt_event_chstate(ST_POLLING);

  } else {
    LOG(ERROR) << StringPrintf("%s: Received unexpected combination: %d, %d",
                               __FUNCTION__, st_mt_g_state->state, t);
  }
}

// Ths function is called by the main thread holding the mutex when an NFC even
// has occurred.
static void reader_cb_testmode(enum rdr_ntf_type t, enum rdr_ntf_data d,
                               int uidlen, uint8_t *uid, uint8_t discotype,
                               uint8_t datarate) {
  struct mtk_msg_TESTMODE_SETTING_RSP resp;

  (void)(d);
  (void)(uidlen);
  (void)(uid);
  (void)(discotype);
  (void)(datarate);

  LOG(INFO) << StringPrintf("%s: enter", __FUNCTION__);

  if (t == RDR_NTF_DISCONNECT) {
    LOG(INFO) << StringPrintf("%s: received RDR_NTF_DISCONNECT", __FUNCTION__);
    return;  // we have nothing to do.
  }

  if (t == RDR_NTF_FAIL) {
    LOG(INFO) << StringPrintf(
        "%s: received RDR_NTF_FAIL, returning error to CLI", __FUNCTION__);
    (void)reply_failure(MTK_NFC_TESTMODE_SETTING_RSP);
    (void)st_mt_glue_resetmode();
    return;
  }

  // We received RDR_NTF_CONNECT => send success to CLI
  memset(&resp, 0, sizeof(resp));
  resp.hdr.cmd_le = le_to_h_32(MTK_NFC_TESTMODE_SETTING_RSP);
  resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));
  resp.result = MTK_RESULT_SUCCESS;
  (void)st_mt_server_send((struct mtk_msg_hdr *)&resp);

  // Stop the polling and go back to IDLE
  (void)st_mt_glue_resetmode();
}

// Ths function is called by the main thread holding the mutex when an NFC even
// has occurred.
static void reader_cb_readdep(enum rdr_ntf_type t, enum rdr_ntf_data d,
                              int uidlen, uint8_t *uid, uint8_t discotype,
                              uint8_t datarate) {
  struct mtk_msg_FM_READ_DEP_TEST_RSP resp;

  (void)(uidlen);
  (void)(uid);
  (void)(discotype);
  (void)(datarate);

  LOG(INFO) << StringPrintf("%s: enter", __FUNCTION__);

  if (t == RDR_NTF_DISCONNECT) {
    LOG(INFO) << StringPrintf("%s: received RDR_NTF_DISCONNECT", __FUNCTION__);
    return;  // we have nothing to do.
  }

  if (t == RDR_NTF_FAIL) {
    LOG(INFO) << StringPrintf(
        "%s: received RDR_NTF_FAIL, returning error to CLI", __FUNCTION__);
    (void)reply_failure(MTK_NFC_FM_READ_DEP_TEST_RSP);
    (void)st_mt_glue_resetmode();
    return;
  }

  // We received RDR_NTF_CONNECT

  memset(&resp, 0, sizeof(resp));
  resp.hdr.cmd_le = le_to_h_32(MTK_NFC_FM_READ_DEP_TEST_RSP);
  resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));

  if (d != RDR_NTF_NonNDEF) {
    // it shall be NDEF or READONLY here.
    struct ndef_msg m;
    LOG(INFO) << StringPrintf("%s: Reading NDEF message", __FUNCTION__);
    if (0 == st_mt_glue_ndef_read(&m)) {
      ndef_to_climsg(&m, &resp.payload.read);
    }
  }

  // Send this result to the CLI
  LOG(INFO) << StringPrintf("%s: sending response to CLI", __FUNCTION__);
  resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
  (void)st_mt_server_send((struct mtk_msg_hdr *)&resp);

  // Stop the polling and go back to IDLE
  (void)st_mt_glue_resetmode();
}

// Ths function is called by the main thread holding the mutex when an NFC even
// has occurred.
static void p2p_cb(enum p2p_ntf_type t, uint8_t discotype, uint8_t datarate,
                   uint8_t role) {
  LOG(INFO) << StringPrintf("%s: received %d, in state %d", __FUNCTION__, t,
                            st_mt_g_state->state);

  switch (st_mt_g_state->state) {
    case ST_ALS_P2P:
    case ST_ALS_P2P_LLCP_UP: {
      struct mtk_msg_EM_ALS_P2P_MODE_NTF resp;

      // send ntf
      memset(&resp, 0, sizeof(resp));
      resp.hdr.cmd_le = le_to_h_32(MTK_NFC_EM_ALS_P2P_MODE_NTF);
      resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));

      resp.status_le = le_to_h_32(t);
      resp.discovery_type = discotype;
      resp.bitrate = datarate;
      resp.role = role;

      LOG(INFO) << StringPrintf(
          "%s: Sending EM_ALS_P2P_MODE_NTF status=%d disco=%02hhx br=%02hhx "
          "role=%02hhx",
          __FUNCTION__, resp.status_le, resp.discovery_type, resp.bitrate,
          resp.role);
      (void)st_mt_server_send((struct mtk_msg_hdr *)&resp);

      // move state machine
      st_mt_event_chstate((t == P2P_NTF_LLCP_UP) ? ST_ALS_P2P_LLCP_UP
                                                 : ST_ALS_P2P);
    } break;

    case ST_POLLING:
    case ST_POLLING_LLCP_UP: {
      struct mtk_msg_EM_POLLING_MODE_NTF resp;

      // send ntf
      memset(&resp, 0, sizeof(resp));
      resp.hdr.cmd_le = le_to_h_32(MTK_NFC_EM_POLLING_MODE_NTF);
      resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));
      resp.type_le = le_to_h_32(EM_ENABLE_FUNC_P2P_MODE);

      resp.u.p2p.status_le = le_to_h_32(t);
      resp.u.p2p.discovery_type = discotype;
      resp.u.p2p.bitrate = datarate;
      resp.u.p2p.role = role;

      (void)st_mt_server_send((struct mtk_msg_hdr *)&resp);

      // move state machine
      st_mt_event_chstate((t == P2P_NTF_LLCP_UP) ? ST_POLLING_LLCP_UP
                                                 : ST_POLLING);
    } break;

    default:
      LOG(ERROR) << StringPrintf(
          "%s: Unexpected state %d to receive this, ignored", __FUNCTION__,
          st_mt_g_state->state);
  }
}

// Ths function is called by the main thread holding the mutex when an NFC even
// has occurred.
static void ce_cb_fm() {
  LOG(INFO) << StringPrintf("%s: enter", __FUNCTION__);
  // Stop the discovery and go back to IDLE
  (void)st_mt_glue_resetmode();
}

// Ths function is called by the main thread holding the mutex when an NFC even
// has occurred.
static void hce_cb_fm() {
  LOG(INFO) << StringPrintf("%s: enter", __FUNCTION__);
  // Stop the discovery and go back to IDLE
  (void)st_mt_glue_resetmode();
}

// returns 0 on success, 1 on warning, 2 on fatal error
// mutex is already locked when this is being called.
static int process_client_message(struct mtk_msg_hdr *m) {
  uint32_t cmd = le_to_h_32(m->cmd_le);
  uint32_t len = le_to_h_32(m->len_le);

  LOG(INFO) << StringPrintf("Processing command: %d", cmd);

#define DECLARE_RESPONSE_AND_CHECK(req_type, rsp_type)                         \
  struct mtk_msg_##req_type *req = (struct mtk_msg_##req_type *)m;             \
  struct mtk_msg_##rsp_type resp;                                              \
  (void)(req);                                                                 \
  memset(&resp, 0, sizeof(resp));                                              \
  resp.hdr.cmd_le = le_to_h_32(MTK_NFC_##rsp_type);                            \
  resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));     \
  LOG(INFO) << StringPrintf("Received " #req_type);                            \
  if (len != sizeof(struct mtk_msg_##req_type) - sizeof(struct mtk_msg_hdr)) { \
    LOG(ERROR) << StringPrintf(                                                \
        "Received message has unexpected length (%d != %zd)", len,             \
        sizeof(struct mtk_msg_##req_type) - sizeof(struct mtk_msg_hdr));       \
    return reply_failure(MTK_NFC_##rsp_type);                                  \
  }

  switch (cmd) {
    case MTK_NFC_EM_START_CMD: {
      LOG(INFO) << StringPrintf(
          "Received MTK_NFC_EM_START_CMD, initializing NFC stack");
      return st_mt_glue_startNFC();
      // note : we are not sending an answer to this command.
    } break;

    case MTK_NFC_EM_ALS_READER_MODE_REQ: {
      struct mtk_msg_EM_ALS_READER_MODE_REQ dummy,
          *req = (struct mtk_msg_EM_ALS_READER_MODE_REQ *)m;
      struct mtk_msg_EM_ALS_READER_MODE_RSP resp;
      memset(&resp, 0, sizeof(resp));
      resp.hdr.cmd_le = le_to_h_32(MTK_NFC_EM_ALS_READER_MODE_RSP);
      resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));
      LOG(INFO) << StringPrintf("Received EM_ALS_READER_MODE_REQ");
      if (len != sizeof(struct mtk_msg_EM_ALS_READER_MODE_REQ) -
                     sizeof(struct mtk_msg_hdr)) {
        if (len == sizeof(struct mtk_msg_EM_ALS_READER_MODE_REQ_legacy) -
                       sizeof(struct mtk_msg_hdr)) {
          LOG(INFO) << StringPrintf(
              "Received EM_ALS_READER_MODE_REQ without type_v_coding_le");
          struct mtk_msg_EM_ALS_READER_MODE_REQ_legacy *oreq =
              (struct mtk_msg_EM_ALS_READER_MODE_REQ_legacy *)m;
          dummy.action_le = oreq->action_le;
          dummy.type_le = oreq->type_le;
          dummy.type_a_rate_le = oreq->type_a_rate_le;
          dummy.type_b_rate_le = oreq->type_b_rate_le;
          dummy.type_v_rate_le = oreq->type_v_rate_le;
          dummy.type_f_rate_le = oreq->type_f_rate_le;
          dummy.type_v_subcar_le = oreq->type_v_subcar_le;
          dummy.type_v_coding_le = 0;
          req = &dummy;
        } else {
          LOG(ERROR) << StringPrintf(
              "Received message has unexpected length (%d != %zd)", len,
              sizeof(struct mtk_msg_EM_ALS_READER_MODE_REQ) -
                  sizeof(struct mtk_msg_hdr));
          return reply_failure(MTK_NFC_EM_ALS_READER_MODE_RSP);
        }
      }

      // Check we are in a coherent state for this command.
      if (((le_to_h_32(req->action_le) == NFC_EM_ACT_START) &&
           (st_mt_g_state->state != ST_IDLE)) ||
          ((le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) &&
           (st_mt_g_state->state != ST_ALS_READER) &&
           (st_mt_g_state->state != ST_ALS_READER_TAG_CONNECTED))) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command", __FUNCTION__,
            __LINE__, st_mt_g_state->state);
        (void)st_mt_glue_resetmode();  // To resync the UI. It happens if EM
                                       // went to background and back.
        return reply_failure(MTK_NFC_EM_ALS_READER_MODE_RSP);
      }

      // if it is a request to stop the reader mode, do it.
      if (le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) {
        if (st_mt_glue_resetmode() != 0) {
          LOG(ERROR) << StringPrintf(
              "Fatal error while stopping the polling. Exiting");
          reply_failure(MTK_NFC_EM_ALS_READER_MODE_RSP);
          return 2;
        }
        resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
        return st_mt_server_send((struct mtk_msg_hdr *)&resp);
      }

      // otherwise, it was a request to start the polling.
      LOG(INFO) << StringPrintf(
          "Received: type=%x, drA=%x, drB=%x", le_to_h_32(req->type_le),
          le_to_h_32(req->type_a_rate_le), le_to_h_32(req->type_b_rate_le));

      // set the polling parameters in the state.
      st_mt_g_state->rdr_techs.a =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_A ? 1 : 0;
      st_mt_g_state->rdr_techs.b =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_B ? 1 : 0;
      st_mt_g_state->rdr_techs.f =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_F ? 1 : 0;
      st_mt_g_state->rdr_techs.v =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_V ? 1 : 0;
      st_mt_g_state->rdr_techs.kovio =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_KOVIO ? 1 : 0;

      st_mt_g_state->rdr_a_datarate.bps106 =
          le_to_h_32(req->type_a_rate_le) & EM_ALS_READER_M_SPDRATE_106 ? 1 : 0;
      st_mt_g_state->rdr_a_datarate.bps212 =
          le_to_h_32(req->type_a_rate_le) & EM_ALS_READER_M_SPDRATE_212 ? 1 : 0;
      st_mt_g_state->rdr_a_datarate.bps424 =
          le_to_h_32(req->type_a_rate_le) & EM_ALS_READER_M_SPDRATE_424 ? 1 : 0;
      st_mt_g_state->rdr_a_datarate.bps848 =
          le_to_h_32(req->type_a_rate_le) & EM_ALS_READER_M_SPDRATE_848 ? 1 : 0;

      st_mt_g_state->rdr_b_datarate.bps106 =
          le_to_h_32(req->type_b_rate_le) & EM_ALS_READER_M_SPDRATE_106 ? 1 : 0;
      st_mt_g_state->rdr_b_datarate.bps212 =
          le_to_h_32(req->type_b_rate_le) & EM_ALS_READER_M_SPDRATE_212 ? 1 : 0;
      st_mt_g_state->rdr_b_datarate.bps424 =
          le_to_h_32(req->type_b_rate_le) & EM_ALS_READER_M_SPDRATE_424 ? 1 : 0;
      st_mt_g_state->rdr_b_datarate.bps848 =
          le_to_h_32(req->type_b_rate_le) & EM_ALS_READER_M_SPDRATE_848 ? 1 : 0;

      st_mt_g_state->rdr_v_datarate.bps662 =
          le_to_h_32(req->type_b_rate_le) & EM_ALS_READER_M_SPDRATE_662 ? 1 : 0;
      st_mt_g_state->rdr_v_datarate.bps2648 =
          le_to_h_32(req->type_b_rate_le) & EM_ALS_READER_M_SPDRATE_2648 ? 1
                                                                         : 0;

      st_mt_g_state->rdr_f_datarate.bps212 =
          le_to_h_32(req->type_f_rate_le) & EM_ALS_READER_M_SPDRATE_212 ? 1 : 0;
      st_mt_g_state->rdr_f_datarate.bps424 =
          le_to_h_32(req->type_f_rate_le) & EM_ALS_READER_M_SPDRATE_424 ? 1 : 0;

      st_mt_g_state->rdr_v_opts.dual_subcarrier =
          le_to_h_32(req->type_v_subcar_le) ? 1 : 0;
      st_mt_g_state->rdr_v_opts.coding_1of256 =
          le_to_h_32(req->type_v_coding_le) ? 1 : 0;

      // start the polling
      if (st_mt_glue_startmode(0, reader_cb, NULL, 0, NULL) != 0) {
        LOG(ERROR) << StringPrintf(
            "Error while starting the polling. Continueing");
        reply_failure(MTK_NFC_EM_ALS_READER_MODE_RSP);
        return 1;
      }

      // update the state machine
      st_mt_event_chstate(ST_ALS_READER);

      // return success code to CLI
      resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    case MTK_NFC_EM_ALS_READER_MODE_OPT_REQ: {
      DECLARE_RESPONSE_AND_CHECK(EM_ALS_READER_MODE_OPT_REQ,
                                 EM_ALS_READER_MODE_OPT_RSP);

      // Check we are in a coherent state for this command.
      if ((st_mt_g_state->state != ST_ALS_READER_TAG_CONNECTED) &&
          (st_mt_g_state->state != ST_POLLING_TAG_CONNECTED)) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command", __FUNCTION__,
            __LINE__, st_mt_g_state->state);
        (void)st_mt_glue_resetmode();  // To resync the UI. It happens if EM
                                       // went to background and back.
        return reply_failure(MTK_NFC_EM_ALS_READER_MODE_OPT_RSP);
      }

      // We are connected to a tag, so let s proceed.
      switch (le_to_h_32(req->action_le)) {
        case NFC_EM_OPT_ACT_FORMAT: {
          // synchronous
          if (st_mt_glue_ndef_format() != 0) {
            return reply_failure(MTK_NFC_EM_ALS_READER_MODE_OPT_RSP);
          }
          // nothing else to do
        } break;

        case NFC_EM_OPT_ACT_READ: {
          struct ndef_msg ndef;
          // synchronous
          if (st_mt_glue_ndef_read(&ndef) != 0) {
            return reply_failure(MTK_NFC_EM_ALS_READER_MODE_OPT_RSP);
          }
          // fill the information read from the tag.
          ndef_to_climsg(&ndef, &resp.payload.read);
        } break;

        case NFC_EM_OPT_ACT_WRITE: {
          // prepare the ndef from the request
          struct ndef_msg ndef;
          memset(&ndef, 0, sizeof(ndef));
          ndef.type = static_cast<ndef_msg::e_type>(
              le_to_h_32(req->payload.write.type_le));
          ndef.lang = static_cast<ndef_msg::e_lang>(
              le_to_h_32(req->payload.write.lang_le));
          ndef.len = le_to_h_32(req->payload.write.datalen_le);
          ndef.data = req->payload.write.data;

          // synchronous
          if (st_mt_glue_ndef_write(&ndef) != 0) {
            return reply_failure(MTK_NFC_EM_ALS_READER_MODE_OPT_RSP);
          }
          // nothing else to do
        } break;

        default:
          LOG(ERROR) << StringPrintf("%s:%d - Unsupported command value",
                                     __FUNCTION__, __LINE__);
      }

      // return success code to CLI
      resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    case MTK_NFC_EM_ALS_P2P_MODE_REQ: {
      DECLARE_RESPONSE_AND_CHECK(EM_ALS_P2P_MODE_REQ, EM_ALS_P2P_MODE_RSP);

      // Check we are in a coherent state for this command.
      if (((le_to_h_32(req->action_le) == NFC_EM_ACT_START) &&
           (st_mt_g_state->state != ST_IDLE)) ||
          ((le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) &&
           (st_mt_g_state->state != ST_ALS_P2P) &&
           (st_mt_g_state->state != ST_ALS_P2P_LLCP_UP))) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command", __FUNCTION__,
            __LINE__, st_mt_g_state->state);
        (void)st_mt_glue_resetmode();  // To resync the UI. It happens if EM
                                       // went to background and back.
        return reply_failure(MTK_NFC_EM_ALS_P2P_MODE_RSP);
      }

      // if it is a request to stop the p2p mode, do it.
      if (le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) {
        if (st_mt_glue_resetmode() != 0) {
          LOG(ERROR) << StringPrintf(
              "Fatal error while stopping the polling. Exiting");
          reply_failure(MTK_NFC_EM_ALS_P2P_MODE_RSP);
          return 2;
        }
        resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
        return st_mt_server_send((struct mtk_msg_hdr *)&resp);
      }

      // otherwise, it was a request to start the polling.
      LOG(INFO) << StringPrintf(
          "Received: type=%x, drA=%x, drF=%x", le_to_h_32(req->type_le),
          le_to_h_32(req->type_a_rate_le), le_to_h_32(req->type_f_rate_le));

      // set the polling parameters in the state.
      st_mt_g_state->p2p_techs.a =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_A ? 1 : 0;
      st_mt_g_state->p2p_techs.f =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_F ? 1 : 0;

      st_mt_g_state->p2p_a_datarate.bps106 =
          le_to_h_32(req->type_a_rate_le) & EM_ALS_READER_M_SPDRATE_106 ? 1 : 0;
      st_mt_g_state->p2p_a_datarate.bps212 =
          le_to_h_32(req->type_a_rate_le) & EM_ALS_READER_M_SPDRATE_212 ? 1 : 0;
      st_mt_g_state->p2p_a_datarate.bps424 =
          le_to_h_32(req->type_a_rate_le) & EM_ALS_READER_M_SPDRATE_424 ? 1 : 0;
      st_mt_g_state->p2p_a_datarate.bps848 =
          le_to_h_32(req->type_a_rate_le) & EM_ALS_READER_M_SPDRATE_848 ? 1 : 0;

      st_mt_g_state->p2p_f_datarate.bps212 =
          le_to_h_32(req->type_f_rate_le) & EM_ALS_READER_M_SPDRATE_212 ? 1 : 0;
      st_mt_g_state->p2p_f_datarate.bps424 =
          le_to_h_32(req->type_f_rate_le) & EM_ALS_READER_M_SPDRATE_424 ? 1 : 0;

      st_mt_g_state->p2p_opts.active =
          le_to_h_32(req->mode_le) & EM_P2P_MODE_ACTIVE_MODE ? 1 : 0;
      st_mt_g_state->p2p_opts.passive =
          le_to_h_32(req->mode_le) & EM_P2P_MODE_PASSIVE_MODE ? 1 : 0;
      st_mt_g_state->p2p_opts.initiator =
          le_to_h_32(req->role_le) & EM_P2P_ROLE_INITIATOR_MODE ? 1 : 0;
      st_mt_g_state->p2p_opts.target =
          le_to_h_32(req->role_le) & EM_P2P_ROLE_TARGET_MODE ? 1 : 0;
      st_mt_g_state->p2p_opts.disable_CE =
          le_to_h_32(req->disable_ce_le) ? 1 : 0;

      // start the polling
      if (st_mt_glue_startmode(0, NULL, p2p_cb, 0, NULL) != 0) {
        LOG(ERROR) << StringPrintf(
            "Error while starting the polling. Continueing");
        reply_failure(MTK_NFC_EM_ALS_P2P_MODE_RSP);
        return 1;
      }

      // update the state machine
      st_mt_event_chstate(ST_ALS_P2P);

      // return success code to CLI
      resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    case MTK_NFC_EM_ALS_CARD_MODE_REQ: {
      DECLARE_RESPONSE_AND_CHECK(EM_ALS_CARD_MODE_REQ, EM_ALS_CARD_MODE_RSP);

      // Check we are in a coherent state for this command.
      if (((le_to_h_32(req->action_le) == NFC_EM_ACT_START) &&
           (st_mt_g_state->state != ST_IDLE)) ||
          ((le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) &&
           (st_mt_g_state->state != ST_ALS_CE))) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command", __FUNCTION__,
            __LINE__, st_mt_g_state->state);
        (void)st_mt_glue_resetmode();  // To resync the UI. It happens if EM
                                       // went to background and back.
        return reply_failure(MTK_NFC_EM_ALS_CARD_MODE_RSP);
      }

      // if it is a request to stop the CE mode, do it.
      if (le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) {
        if (st_mt_glue_resetmode() != 0) {
          LOG(ERROR) << StringPrintf(
              "Fatal error while stopping the listening. Exiting");
          reply_failure(MTK_NFC_EM_ALS_CARD_MODE_RSP);
          return 2;
        }
        resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
        return st_mt_server_send((struct mtk_msg_hdr *)&resp);
      }

      // otherwise, it was a request to start the listening.
      LOG(INFO) << StringPrintf(
          "Received: type=%x, swio=%x, hce=%x", le_to_h_32(req->type_le),
          le_to_h_32(req->swionr_le), le_to_h_32(req->hce_le));

      // set the listening parameters in the state.
      st_mt_g_state->ce_sel = static_cast<st_mt_state::e_ce_sel>(
          le_to_h_32(req->swionr_le));  // will be 1, 2, or 3.

      st_mt_g_state->ce_techs.a =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_A ? 1 : 0;
      st_mt_g_state->ce_techs.b =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_B ? 1 : 0;
      st_mt_g_state->ce_techs.f =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_F ? 1 : 0;
      st_mt_g_state->ce_techs.bprime =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_Bprime ? 1 : 0;

      // if some modes are disabled, we need to remove merge mode
      if (!st_mt_g_state->ce_techs.a || !st_mt_g_state->ce_techs.b ||
          !st_mt_g_state->ce_techs.f) {
        if (st_mt_glue_setRFMergeMode(0) != 0) {
          LOG(ERROR) << StringPrintf(
              "Error while disabling RF Merge mode. Continueing");
          reply_failure(MTK_NFC_EM_ALS_CARD_MODE_RSP);
          return 1;
        }
      }

      // start the listening
      if (st_mt_glue_startmode(0, NULL, NULL, 1, NULL) != 0) {
        LOG(ERROR) << StringPrintf(
            "Error while starting the listening. Continueing");
        reply_failure(MTK_NFC_EM_ALS_CARD_MODE_RSP);
        return 1;
      }

      // update the state machine
      st_mt_event_chstate(ST_ALS_CE);

      // return success code to CLI
      resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    case MTK_NFC_EM_POLLING_MODE_REQ: {
      int do_polling = 0;
      int do_p2p = 0;
      int do_ce = 0;
      //  DECLARE_RESPONSE_AND_CHECK(EM_POLLING_MODE_REQ, EM_POLLING_MODE_RSP);
      struct mtk_msg_EM_POLLING_MODE_REQ dummy,
          *req = (struct mtk_msg_EM_POLLING_MODE_REQ *)m;
      struct mtk_msg_EM_POLLING_MODE_RSP resp;
      memset(&resp, 0, sizeof(resp));
      resp.hdr.cmd_le = le_to_h_32(MTK_NFC_EM_POLLING_MODE_RSP);
      resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));
      LOG(INFO) << StringPrintf("Received EM_POLLING_MODE_REQ");
      if (len != sizeof(struct mtk_msg_EM_POLLING_MODE_REQ) -
                     sizeof(struct mtk_msg_hdr)) {
        if (len == sizeof(struct mtk_msg_EM_POLLING_MODE_REQ_legacy) -
                       sizeof(struct mtk_msg_hdr)) {
          LOG(INFO) << StringPrintf(
              "Received EM_POLLING_MODE_REQ without type_v_coding_le");
          struct mtk_msg_EM_POLLING_MODE_REQ_legacy *oreq =
              (struct mtk_msg_EM_POLLING_MODE_REQ_legacy *)m;
          dummy.action_le = oreq->action_le;
          dummy.phase_le = oreq->phase_le;
          dummy.period_le = oreq->period_le;
          dummy.function_le = oreq->function_le;
          memcpy(&dummy.p2p, &oreq->p2p, sizeof(dummy.p2p));
          memcpy(&dummy.ce, &oreq->ce, sizeof(dummy.ce));
          dummy.reader.type_le = oreq->reader.type_le;
          dummy.reader.type_a_rate_le = oreq->reader.type_a_rate_le;
          dummy.reader.type_b_rate_le = oreq->reader.type_b_rate_le;
          dummy.reader.type_v_rate_le = oreq->reader.type_v_rate_le;
          dummy.reader.type_f_rate_le = oreq->reader.type_f_rate_le;
          dummy.reader.type_v_subcar_le = oreq->reader.type_v_subcar_le;
          dummy.reader.type_v_coding_le = 0;
          req = &dummy;
        } else {
          LOG(ERROR) << StringPrintf(
              "Received message has unexpected length (%d != %zd)", len,
              sizeof(struct mtk_msg_EM_POLLING_MODE_REQ) -
                  sizeof(struct mtk_msg_hdr));
          return reply_failure(MTK_NFC_EM_POLLING_MODE_RSP);
        }
      }

      // Check we are in a coherent state for this command.
      if (((le_to_h_32(req->action_le) == NFC_EM_ACT_START) &&
           (st_mt_g_state->state != ST_IDLE)) ||
          ((le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) &&
           (st_mt_g_state->state != ST_POLLING) &&
           (st_mt_g_state->state != ST_POLLING_TAG_CONNECTED) &&
           (st_mt_g_state->state != ST_POLLING_LLCP_UP))) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command", __FUNCTION__,
            __LINE__, st_mt_g_state->state);
        (void)st_mt_glue_resetmode();  // To resync the UI. It happens if EM
                                       // went to background and back.
        return reply_failure(MTK_NFC_EM_POLLING_MODE_RSP);
      }

      // if it is a request to stop the polling mode, do it.
      if (le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) {
        if (st_mt_glue_resetmode() != 0) {
          LOG(ERROR) << StringPrintf(
              "Fatal error while stopping the polling. Exiting");
          reply_failure(MTK_NFC_EM_POLLING_MODE_RSP);
          return 2;
        }
        resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
        return st_mt_server_send((struct mtk_msg_hdr *)&resp);
      }

      // otherwise, it was a request to start the polling.
      LOG(INFO) << StringPrintf("Received: poll period=%x, func=%x",
                                le_to_h_32(req->period_le),
                                le_to_h_32(req->function_le));

      if (le_to_h_32(req->function_le) & EM_ENABLE_FUNC_READER_MODE) {
        do_polling = 1;

        // set the polling parameters in the state.
        st_mt_g_state->rdr_techs.a =
            le_to_h_32(req->reader.type_le) & EM_ALS_READER_M_TYPE_A ? 1 : 0;
        st_mt_g_state->rdr_techs.b =
            le_to_h_32(req->reader.type_le) & EM_ALS_READER_M_TYPE_B ? 1 : 0;
        st_mt_g_state->rdr_techs.f =
            le_to_h_32(req->reader.type_le) & EM_ALS_READER_M_TYPE_F ? 1 : 0;
        st_mt_g_state->rdr_techs.v =
            le_to_h_32(req->reader.type_le) & EM_ALS_READER_M_TYPE_V ? 1 : 0;
        st_mt_g_state->rdr_techs.kovio =
            le_to_h_32(req->reader.type_le) & EM_ALS_READER_M_TYPE_KOVIO ? 1
                                                                         : 0;

        st_mt_g_state->rdr_a_datarate.bps106 =
            le_to_h_32(req->reader.type_a_rate_le) & EM_ALS_READER_M_SPDRATE_106
                ? 1
                : 0;
        st_mt_g_state->rdr_a_datarate.bps212 =
            le_to_h_32(req->reader.type_a_rate_le) & EM_ALS_READER_M_SPDRATE_212
                ? 1
                : 0;
        st_mt_g_state->rdr_a_datarate.bps424 =
            le_to_h_32(req->reader.type_a_rate_le) & EM_ALS_READER_M_SPDRATE_424
                ? 1
                : 0;
        st_mt_g_state->rdr_a_datarate.bps848 =
            le_to_h_32(req->reader.type_a_rate_le) & EM_ALS_READER_M_SPDRATE_848
                ? 1
                : 0;

        st_mt_g_state->rdr_b_datarate.bps106 =
            le_to_h_32(req->reader.type_b_rate_le) & EM_ALS_READER_M_SPDRATE_106
                ? 1
                : 0;
        st_mt_g_state->rdr_b_datarate.bps212 =
            le_to_h_32(req->reader.type_b_rate_le) & EM_ALS_READER_M_SPDRATE_212
                ? 1
                : 0;
        st_mt_g_state->rdr_b_datarate.bps424 =
            le_to_h_32(req->reader.type_b_rate_le) & EM_ALS_READER_M_SPDRATE_424
                ? 1
                : 0;
        st_mt_g_state->rdr_b_datarate.bps848 =
            le_to_h_32(req->reader.type_b_rate_le) & EM_ALS_READER_M_SPDRATE_848
                ? 1
                : 0;

        st_mt_g_state->rdr_v_datarate.bps662 =
            le_to_h_32(req->reader.type_b_rate_le) & EM_ALS_READER_M_SPDRATE_662
                ? 1
                : 0;
        st_mt_g_state->rdr_v_datarate.bps2648 =
            le_to_h_32(req->reader.type_b_rate_le) &
                    EM_ALS_READER_M_SPDRATE_2648
                ? 1
                : 0;

        st_mt_g_state->rdr_f_datarate.bps212 =
            le_to_h_32(req->reader.type_f_rate_le) & EM_ALS_READER_M_SPDRATE_212
                ? 1
                : 0;
        st_mt_g_state->rdr_f_datarate.bps424 =
            le_to_h_32(req->reader.type_f_rate_le) & EM_ALS_READER_M_SPDRATE_424
                ? 1
                : 0;

        st_mt_g_state->rdr_v_opts.dual_subcarrier =
            le_to_h_32(req->reader.type_v_subcar_le) ? 1 : 0;
        st_mt_g_state->rdr_v_opts.coding_1of256 =
            le_to_h_32(req->reader.type_v_coding_le) ? 1 : 0;
      }

      if (le_to_h_32(req->function_le) & EM_ENABLE_FUNC_P2P_MODE) {
        do_p2p = 1;

        // set the polling parameters in the state.
        st_mt_g_state->p2p_techs.a =
            le_to_h_32(req->p2p.type_le) & EM_ALS_READER_M_TYPE_A ? 1 : 0;
        st_mt_g_state->p2p_techs.f =
            le_to_h_32(req->p2p.type_le) & EM_ALS_READER_M_TYPE_F ? 1 : 0;

        st_mt_g_state->p2p_a_datarate.bps106 =
            le_to_h_32(req->p2p.type_a_rate_le) & EM_ALS_READER_M_SPDRATE_106
                ? 1
                : 0;
        st_mt_g_state->p2p_a_datarate.bps212 =
            le_to_h_32(req->p2p.type_a_rate_le) & EM_ALS_READER_M_SPDRATE_212
                ? 1
                : 0;
        st_mt_g_state->p2p_a_datarate.bps424 =
            le_to_h_32(req->p2p.type_a_rate_le) & EM_ALS_READER_M_SPDRATE_424
                ? 1
                : 0;
        st_mt_g_state->p2p_a_datarate.bps848 =
            le_to_h_32(req->p2p.type_a_rate_le) & EM_ALS_READER_M_SPDRATE_848
                ? 1
                : 0;

        st_mt_g_state->p2p_f_datarate.bps212 =
            le_to_h_32(req->p2p.type_f_rate_le) & EM_ALS_READER_M_SPDRATE_212
                ? 1
                : 0;
        st_mt_g_state->p2p_f_datarate.bps424 =
            le_to_h_32(req->p2p.type_f_rate_le) & EM_ALS_READER_M_SPDRATE_424
                ? 1
                : 0;

        st_mt_g_state->p2p_opts.active =
            le_to_h_32(req->p2p.mode_le) & EM_P2P_MODE_ACTIVE_MODE ? 1 : 0;
        st_mt_g_state->p2p_opts.passive =
            le_to_h_32(req->p2p.mode_le) & EM_P2P_MODE_PASSIVE_MODE ? 1 : 0;
        st_mt_g_state->p2p_opts.initiator =
            le_to_h_32(req->p2p.role_le) & EM_P2P_ROLE_INITIATOR_MODE ? 1 : 0;
        st_mt_g_state->p2p_opts.target =
            le_to_h_32(req->p2p.role_le) & EM_P2P_ROLE_TARGET_MODE ? 1 : 0;
        st_mt_g_state->p2p_opts.disable_CE =
            le_to_h_32(req->p2p.disable_ce_le) ? 1 : 0;
      }

      if (le_to_h_32(req->function_le) & EM_ENABLE_FUNC_CARD_MODE) {
        do_ce = 1;

        // set the listening parameters in the state.
        st_mt_g_state->ce_sel = static_cast<st_mt_state::e_ce_sel>(
            le_to_h_32(req->ce.swionr_le));  // will be 1, 2, or 3.

        st_mt_g_state->ce_techs.a =
            le_to_h_32(req->ce.type_le) & EM_ALS_READER_M_TYPE_A ? 1 : 0;
        st_mt_g_state->ce_techs.b =
            le_to_h_32(req->ce.type_le) & EM_ALS_READER_M_TYPE_B ? 1 : 0;
        st_mt_g_state->ce_techs.f =
            le_to_h_32(req->ce.type_le) & EM_ALS_READER_M_TYPE_F ? 1 : 0;
        st_mt_g_state->ce_techs.bprime =
            le_to_h_32(req->ce.type_le) & EM_ALS_READER_M_TYPE_Bprime ? 1 : 0;

        // if some modes are disabled, we need to remove merge mode
        if (!st_mt_g_state->ce_techs.a || !st_mt_g_state->ce_techs.b ||
            !st_mt_g_state->ce_techs.f) {
          if (st_mt_glue_setRFMergeMode(0) != 0) {
            LOG(ERROR) << StringPrintf(
                "Error while disabling RF Merge mode. Continueing");
            reply_failure(MTK_NFC_EM_POLLING_MODE_RSP);
            return 1;
          }
        }
      }

      // start the polling
      if (st_mt_glue_startmode(le_to_h_32(req->period_le),
                               do_polling ? reader_cb : NULL,
                               do_p2p ? p2p_cb : NULL, do_ce, NULL) != 0) {
        LOG(ERROR) << StringPrintf(
            "Error while starting the polling. Continueing");
        reply_failure(MTK_NFC_EM_POLLING_MODE_RSP);
        return 1;
      }

      // update the state machine
      st_mt_event_chstate(ST_POLLING);

      // return success code to CLI
      resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    case MTK_NFC_EM_TX_CARRIER_ALS_ON_REQ: {
      DECLARE_RESPONSE_AND_CHECK(EM_TX_CARRIER_ALS_ON_REQ,
                                 EM_TX_CARRIER_ALS_ON_RSP);

      // Check we are in a coherent state for this command.
      if (((le_to_h_32(req->action_le) == NFC_EM_ACT_START) &&
           (st_mt_g_state->state != ST_IDLE)) ||
          ((le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) &&
           (st_mt_g_state->state != ST_TXON))) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command", __FUNCTION__,
            __LINE__, st_mt_g_state->state);
        (void)st_mt_glue_resetmode();  // To resync the UI. It happens if EM
                                       // went to background and back.
        return reply_failure(MTK_NFC_EM_TX_CARRIER_ALS_ON_RSP);
      }

      // Proceed.
      if (st_mt_glue_test_TXON(le_to_h_32(req->action_le) ==
                               NFC_EM_ACT_START) != 0) {
        LOG(ERROR) << StringPrintf(
            "Fatal error while changing TX ON state. Exiting");
        reply_failure(MTK_NFC_EM_TX_CARRIER_ALS_ON_RSP);
        return 2;
      }

      // update the state machine
      if (le_to_h_32(req->action_le) == NFC_EM_ACT_START) {
        st_mt_event_chstate(ST_TXON);
      } else {
        st_mt_event_chstate(ST_IDLE);

        // The chip seems to handle this not so well... Let s reset it here.
        (void)st_mt_glue_stopNFC();
        (void)st_mt_glue_startNFC();
      }

      resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    case MTK_NFC_EM_VIRTUAL_CARD_REQ: {
      DECLARE_RESPONSE_AND_CHECK(EM_VIRTUAL_CARD_REQ, EM_VIRTUAL_CARD_RSP);

      // Check we are in a coherent state for this command.
      if (((le_to_h_32(req->action_le) == NFC_EM_ACT_START) &&
           (st_mt_g_state->state != ST_IDLE)) ||
          ((le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) &&
           (st_mt_g_state->state != ST_ALS_HCE))) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command", __FUNCTION__,
            __LINE__, st_mt_g_state->state);
        (void)st_mt_glue_resetmode();  // To resync the UI. It happens if EM
                                       // went to background and back.
        return reply_failure(MTK_NFC_EM_VIRTUAL_CARD_RSP);
      }

      // if it is a request to stop the CE mode, do it.
      if (le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) {
        if (st_mt_glue_resetmode() != 0) {
          LOG(ERROR) << StringPrintf(
              "Fatal error while stopping the listening. Exiting");
          reply_failure(MTK_NFC_EM_VIRTUAL_CARD_RSP);
          return 2;
        }
        resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
        return st_mt_server_send((struct mtk_msg_hdr *)&resp);
      }

      // otherwise, it was a request to start the listening.
      LOG(INFO) << StringPrintf("Received: type=%x, brF=%x",
                                le_to_h_32(req->type_le),
                                le_to_h_32(req->type_f_rate_le));

      // set the listening parameters in the state.
      st_mt_g_state->ce_sel = st_mt_state::HCE;  // 0.

      st_mt_g_state->ce_techs.a =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_A ? 1 : 0;
      st_mt_g_state->ce_techs.b =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_B ? 1 : 0;
      st_mt_g_state->ce_techs.f =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_F ? 1 : 0;
      st_mt_g_state->ce_techs.f212 =
          le_to_h_32(req->type_f_rate_le) & EM_ALS_READER_M_SPDRATE_212 ? 1 : 0;
      st_mt_g_state->ce_techs.f424 =
          le_to_h_32(req->type_f_rate_le) & EM_ALS_READER_M_SPDRATE_424 ? 1 : 0;
      st_mt_g_state->ce_techs.bprime =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_Bprime ? 1 : 0;

      // start the listening
      if (st_mt_glue_startmode(0, NULL, NULL, 1, NULL) != 0) {
        LOG(ERROR) << StringPrintf(
            "Error while starting the hce listening. Continueing");
        reply_failure(MTK_NFC_EM_VIRTUAL_CARD_RSP);
        return 1;
      }

      // update the state machine
      st_mt_event_chstate(ST_ALS_HCE);

      // return success code to CLI
      resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    case MTK_NFC_EM_STOP_CMD: {
      LOG(INFO) << StringPrintf(
          "Received MTK_NFC_EM_STOP_CMD, deinitializing NFC stack and "
          "quitting");
      (void)st_mt_glue_stopNFC();
      st_mt_g_state->exiting = 1;
      return 0;
    } break;

    case MTK_NFC_TESTMODE_SETTING_REQ: {
      DECLARE_RESPONSE_AND_CHECK(TESTMODE_SETTING_REQ, TESTMODE_SETTING_RSP);

      // Check we are in a coherent state for this command.
      if (((le_to_h_32(req->autocheck_le) == 1) &&
           (st_mt_g_state->state != ST_IDLE)) ||
          ((le_to_h_32(req->autocheck_le) == 0) &&
           (st_mt_g_state->state != ST_TM_OPTIONCHECK_W84TAG))) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command", __FUNCTION__,
            __LINE__, st_mt_g_state->state);
        (void)st_mt_glue_resetmode();  // To resync the UI. It happens if EM
                                       // went to background and back.
        return reply_failure(MTK_NFC_TESTMODE_SETTING_RSP);
      }

      // if it is a request to stop the reader mode, do it. It happens if a
      // previous req has not timeout yet.
      if (le_to_h_32(req->autocheck_le) == 0) {
        if (st_mt_glue_resetmode() != 0) {
          LOG(ERROR) << StringPrintf(
              "Fatal error while stopping the polling. Exiting");
          reply_failure(MTK_NFC_TESTMODE_SETTING_RSP);
          return 2;
        }
        resp.result = MTK_RESULT_SUCCESS;
        return st_mt_server_send((struct mtk_msg_hdr *)&resp);
      }

      // otherwise, it was a request to start the polling. We poll for all.

      // set the polling parameters in the state.
      st_mt_g_state->rdr_techs.a = 1;
      st_mt_g_state->rdr_techs.b = 1;
      st_mt_g_state->rdr_techs.f = 1;
      st_mt_g_state->rdr_techs.v = 1;
      st_mt_g_state->rdr_techs.kovio = 1;

      st_mt_g_state->rdr_a_datarate.bps106 = 1;
      st_mt_g_state->rdr_a_datarate.bps212 = 1;
      st_mt_g_state->rdr_a_datarate.bps424 = 1;
      st_mt_g_state->rdr_a_datarate.bps848 = 1;

      st_mt_g_state->rdr_b_datarate.bps106 = 1;
      st_mt_g_state->rdr_b_datarate.bps212 = 1;
      st_mt_g_state->rdr_b_datarate.bps424 = 1;
      st_mt_g_state->rdr_b_datarate.bps848 = 1;

      st_mt_g_state->rdr_v_datarate.bps662 = 1;
      st_mt_g_state->rdr_v_datarate.bps2648 = 1;

      st_mt_g_state->rdr_f_datarate.bps212 = 1;
      st_mt_g_state->rdr_f_datarate.bps424 = 1;

      // start the polling
      if (st_mt_glue_startmode(0, reader_cb_testmode, NULL, 0, NULL) != 0) {
        LOG(ERROR) << StringPrintf(
            "Error while starting the polling. Continueing");
        reply_failure(MTK_NFC_TESTMODE_SETTING_RSP);
        return 1;
      }

      // update the state machine
      st_mt_event_chstate(ST_TM_OPTIONCHECK_W84TAG);

      return 0;  // response is sent when tag is detected.
    } break;

    case MTK_NFC_EM_LOOPBACK_TEST_REQ: {
      int r;
      DECLARE_RESPONSE_AND_CHECK(EM_LOOPBACK_TEST_REQ, EM_LOOPBACK_TEST_RSP);

      // We handle synchronously so we actually never can receive a STOP request
      // at appropriate timing.
      if (st_mt_g_state->state != ST_IDLE) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command", __FUNCTION__,
            __LINE__, st_mt_g_state->state);
        (void)st_mt_glue_resetmode();  // To resync the UI. It happens if EM
                                       // went to background and back.
        return reply_failure(MTK_NFC_EM_LOOPBACK_TEST_RSP);
      }

      // Proceed.
      if (req->action == NFC_EM_ACT_START) {
        r = st_mt_glue_test_loopback();
        if (r < 0) {
          LOG(ERROR) << StringPrintf(
              "Fatal error while testing looback. Exiting");
          reply_failure(MTK_NFC_EM_LOOPBACK_TEST_RSP);
          return 2;
        }
        if (!st_mt_g_state->is_em) {
          r = 1 - r;
        }
      } else {
        r = 1 - st_mt_g_state->is_em;  // STOP expects 1 in META but 0 in EM .
      }

      resp.result = r;
      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    case MTK_NFC_SW_VERSION_QUERY: {
      DECLARE_RESPONSE_AND_CHECK(SW_VERSION_QUERY, SW_VERSION_RESPONSE);

      // allow in any state.
      if (st_mt_glue_getversions(resp.mw_ver, resp.fw_ver, resp.hw_ver,
                                 resp.chip_ver) != 0) {
        LOG(ERROR) << StringPrintf("Failed to retrieve versions");
        return reply_failure(MTK_NFC_SW_VERSION_RESPONSE);
      }

      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    case MTK_NFC_EM_DEACTIVATE_CMD: {
      LOG(INFO) << StringPrintf("Received MTK_NFC_EM_DEACTIVATE_CMD");
      return st_mt_glue_resetmode();
    } break;

    case MTK_NFC_FM_SWP_TEST_REQ: {
      int r;
      struct mtk_msg_FM_SWP_TEST_REQ dummy,
          *req = (struct mtk_msg_FM_SWP_TEST_REQ *)m;
      struct mtk_msg_FM_SWP_TEST_RSP resp;
      memset(&resp, 0, sizeof(resp));
      resp.hdr.cmd_le = le_to_h_32(MTK_NFC_FM_SWP_TEST_RSP);
      resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));
      LOG(INFO) << StringPrintf("Received FM_SWP_TEST_REQ");
      if (len !=
          sizeof(struct mtk_msg_FM_SWP_TEST_REQ) - sizeof(struct mtk_msg_hdr)) {
        if (len != sizeof(struct mtk_msg_FM_SWP_TEST_REQ) -
                       sizeof(struct mtk_msg_hdr) - 4) {
          LOG(ERROR) << StringPrintf(
              "Received message has unexpected length (%d != %zd)", len,
              sizeof(struct mtk_msg_FM_SWP_TEST_REQ) -
                  sizeof(struct mtk_msg_hdr));
          return reply_failure(MTK_NFC_FM_SWP_TEST_RSP);
        }
        // we received a message without "opt". This is sent from EM or META
        dummy.action_le = req->action_le;
        LOG(INFO) << StringPrintf(
            "Received FM_SWP_TEST_REQ legacy format, hardcode test to UICC1");
        dummy.opt.swionr_le = le_to_h_32(1);  // hardcode to UICC1.
        req = &dummy;
      } else {
        // we received the "opt", sent from FM or AT
        // the "action" is always 1 here (and there is no "STOP")
        // replace with 0.
        req->action_le = le_to_h_32(NFC_EM_ACT_START);
      }

      if (st_mt_g_state->state != ST_IDLE) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command, reset mode first",
            __FUNCTION__, __LINE__, st_mt_g_state->state);
        if (st_mt_glue_resetmode() != 0) {
          return reply_failure(MTK_NFC_FM_SWP_TEST_RSP);
        }
      }

      // Proceed.
      if (le_to_h_32(req->action_le) == NFC_EM_ACT_START) {
        r = 1;
        LOG(INFO) << StringPrintf("%s: tests requested %x", __FUNCTION__,
                                  le_to_h_32(req->opt.swionr_le));
        for (int i = 0; i < 3; i++) {
          if (le_to_h_32(req->opt.swionr_le) & (1 << i)) {
            r = st_mt_glue_test_swp(i + 1);
            if (r < 0) {
              LOG(ERROR) << StringPrintf(
                  "Fatal error while testing SWP (%d). Exiting", i + 1);
              reply_failure(MTK_NFC_FM_SWP_TEST_RSP);
              return 2;  // anyway if this test fails, everything else would
                         // fail too...
            }
            if (r != 0) {
              break;
            }
          }
        }
      } else {  // we always say success to STOP requests
        LOG(INFO) << StringPrintf("%s: sending success to STOP action",
                                  __FUNCTION__);
        r = 0;
      }

      if (r) {
        // Somehow, after a failed activation, it seems we cannot activate
        // properly. Reset to clear this.
        (void)st_mt_glue_stopNFC();
        (void)st_mt_glue_startNFC();
      }

      resp.result_le = le_to_h_32(r);
      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    case MTK_NFC_FM_READ_DEP_TEST_REQ: {
      struct mtk_msg_FM_READ_DEP_TEST_REQ dummy,
          *req = (struct mtk_msg_FM_READ_DEP_TEST_REQ *)m;
      struct mtk_msg_FM_READ_DEP_TEST_RSP resp;
      memset(&resp, 0, sizeof(resp));
      resp.hdr.cmd_le = le_to_h_32(MTK_NFC_FM_READ_DEP_TEST_RSP);
      resp.hdr.len_le = le_to_h_32(sizeof(resp) - sizeof(struct mtk_msg_hdr));
      LOG(INFO) << StringPrintf("Received FM_READ_DEP_TEST_REQ");
      if (len != sizeof(struct mtk_msg_FM_READ_DEP_TEST_REQ) -
                     sizeof(struct mtk_msg_hdr)) {
        if (len == sizeof(struct mtk_msg_FM_READ_DEP_TEST_REQ_legacy) -
                       sizeof(struct mtk_msg_hdr)) {
          LOG(INFO) << StringPrintf(
              "Received FM_READ_DEP_TEST_REQ without type_v_coding_le");
          struct mtk_msg_FM_READ_DEP_TEST_REQ_legacy *oreq =
              (struct mtk_msg_FM_READ_DEP_TEST_REQ_legacy *)m;
          dummy.action_le = oreq->action_le;
          dummy.type_le = oreq->type_le;
          dummy.type_a_rate_le = oreq->type_a_rate_le;
          dummy.type_b_rate_le = oreq->type_b_rate_le;
          dummy.type_v_rate_le = oreq->type_v_rate_le;
          dummy.type_f_rate_le = oreq->type_f_rate_le;
          dummy.type_v_subcar_le = oreq->type_v_subcar_le;
          dummy.type_v_coding_le = 0;
          req = &dummy;
        } else {
          LOG(ERROR) << StringPrintf(
              "Received message has unexpected length (%d != %zd)", len,
              sizeof(struct mtk_msg_FM_READ_DEP_TEST_REQ) -
                  sizeof(struct mtk_msg_hdr));
          return reply_failure(MTK_NFC_FM_READ_DEP_TEST_RSP);
        }
      }

      // Check we are in a coherent state for this command.
      if (((le_to_h_32(req->action_le) == NFC_EM_ACT_START) &&
           (st_mt_g_state->state != ST_IDLE)) ||
          ((le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) &&
           (st_mt_g_state->state != ST_FM_READDEP_W84TAG))) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command, reset mode first",
            __FUNCTION__, __LINE__, st_mt_g_state->state);
        if (st_mt_glue_resetmode() != 0) {
          return reply_failure(MTK_NFC_FM_READ_DEP_TEST_RSP);
        }
      }

      // if it is a request to stop the reader mode, do it.
      if (le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) {
        if (st_mt_glue_resetmode() != 0) {
          LOG(ERROR) << StringPrintf(
              "Fatal error while stopping the polling. Exiting");
          reply_failure(MTK_NFC_FM_READ_DEP_TEST_RSP);
          return 2;
        }
        resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
        return st_mt_server_send((struct mtk_msg_hdr *)&resp);
      }

      // otherwise, it was a request to start the polling.
      LOG(INFO) << StringPrintf("Received (READDEP): type=%x, drA=%x, drB=%x",
                                le_to_h_32(req->type_le),
                                le_to_h_32(req->type_a_rate_le),
                                le_to_h_32(req->type_b_rate_le));

      // set the polling parameters in the state.
      st_mt_g_state->rdr_techs.a =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_A ? 1 : 0;
      st_mt_g_state->rdr_techs.b =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_B ? 1 : 0;
      st_mt_g_state->rdr_techs.f =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_F ? 1 : 0;
      st_mt_g_state->rdr_techs.v =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_V ? 1 : 0;
      st_mt_g_state->rdr_techs.kovio =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_KOVIO ? 1 : 0;

      st_mt_g_state->rdr_a_datarate.bps106 =
          le_to_h_32(req->type_a_rate_le) & EM_ALS_READER_M_SPDRATE_106 ? 1 : 0;
      st_mt_g_state->rdr_a_datarate.bps212 =
          le_to_h_32(req->type_a_rate_le) & EM_ALS_READER_M_SPDRATE_212 ? 1 : 0;
      st_mt_g_state->rdr_a_datarate.bps424 =
          le_to_h_32(req->type_a_rate_le) & EM_ALS_READER_M_SPDRATE_424 ? 1 : 0;
      st_mt_g_state->rdr_a_datarate.bps848 =
          le_to_h_32(req->type_a_rate_le) & EM_ALS_READER_M_SPDRATE_848 ? 1 : 0;

      st_mt_g_state->rdr_b_datarate.bps106 =
          le_to_h_32(req->type_b_rate_le) & EM_ALS_READER_M_SPDRATE_106 ? 1 : 0;
      st_mt_g_state->rdr_b_datarate.bps212 =
          le_to_h_32(req->type_b_rate_le) & EM_ALS_READER_M_SPDRATE_212 ? 1 : 0;
      st_mt_g_state->rdr_b_datarate.bps424 =
          le_to_h_32(req->type_b_rate_le) & EM_ALS_READER_M_SPDRATE_424 ? 1 : 0;
      st_mt_g_state->rdr_b_datarate.bps848 =
          le_to_h_32(req->type_b_rate_le) & EM_ALS_READER_M_SPDRATE_848 ? 1 : 0;

      st_mt_g_state->rdr_v_datarate.bps662 =
          le_to_h_32(req->type_b_rate_le) & EM_ALS_READER_M_SPDRATE_662 ? 1 : 0;
      st_mt_g_state->rdr_v_datarate.bps2648 =
          le_to_h_32(req->type_b_rate_le) & EM_ALS_READER_M_SPDRATE_2648 ? 1
                                                                         : 0;

      st_mt_g_state->rdr_f_datarate.bps212 =
          le_to_h_32(req->type_f_rate_le) & EM_ALS_READER_M_SPDRATE_212 ? 1 : 0;
      st_mt_g_state->rdr_f_datarate.bps424 =
          le_to_h_32(req->type_f_rate_le) & EM_ALS_READER_M_SPDRATE_424 ? 1 : 0;

      st_mt_g_state->rdr_v_opts.dual_subcarrier =
          le_to_h_32(req->type_v_subcar_le) ? 1 : 0;
      st_mt_g_state->rdr_v_opts.coding_1of256 =
          le_to_h_32(req->type_v_coding_le) ? 1 : 0;

      // start the polling
      if (st_mt_glue_startmode(0, reader_cb_readdep, NULL, 0, NULL) != 0) {
        LOG(ERROR) << StringPrintf(
            "Error while starting the polling. Continueing");
        reply_failure(MTK_NFC_FM_READ_DEP_TEST_RSP);
        return 1;
      }

      // update the state machine
      st_mt_event_chstate(ST_FM_READDEP_W84TAG);

      // Response will be returned by callback.
      return 0;
    } break;

    case MTK_NFC_FM_CARD_MODE_TEST_REQ: {
      DECLARE_RESPONSE_AND_CHECK(FM_CARD_MODE_TEST_REQ, FM_CARD_MODE_TEST_RSP);

      // Check we are in a coherent state for this command.
      if (((le_to_h_32(req->action_le) == NFC_EM_ACT_START) &&
           (st_mt_g_state->state != ST_IDLE)) ||
          ((le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) &&
           (st_mt_g_state->state != ST_FM_CM_CE_W84READER))) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command, reset mode first",
            __FUNCTION__, __LINE__, st_mt_g_state->state);
        if (st_mt_glue_resetmode() != 0) {
          return reply_failure(MTK_NFC_FM_CARD_MODE_TEST_RSP);
        }
      }

      // if it is a request to stop the CE mode, do it.
      if (le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) {
        LOG(INFO) << StringPrintf(
            "Received MTK_NFC_FM_CARD_MODE_TEST_REQ(STOP)");

        if (st_mt_glue_resetmode() != 0) {
          LOG(ERROR) << StringPrintf(
              "Fatal error while stopping the listening. Exiting");
          reply_failure(MTK_NFC_FM_CARD_MODE_TEST_RSP);
          return 2;
        }
        resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
        return st_mt_server_send((struct mtk_msg_hdr *)&resp);
      }

      // otherwise, it was a request to start the listening.
      LOG(INFO) << StringPrintf(
          "Received: START type=%x, swio=%x, hce=%x", le_to_h_32(req->type_le),
          le_to_h_32(req->swionr_le), le_to_h_32(req->hce_le));

      // set the listening parameters in the state.
      st_mt_g_state->ce_sel = static_cast<st_mt_state::e_ce_sel>(
          le_to_h_32(req->swionr_le));  // will be 1, 2, or 3.

      st_mt_g_state->ce_techs.a =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_A ? 1 : 0;
      st_mt_g_state->ce_techs.b =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_B ? 1 : 0;
      st_mt_g_state->ce_techs.f =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_F ? 1 : 0;
      st_mt_g_state->ce_techs.bprime =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_Bprime ? 1 : 0;

      // start the listening
      if (st_mt_glue_startmode(0, NULL, NULL, 1, ce_cb_fm) != 0) {
        LOG(ERROR) << StringPrintf(
            "Error while starting the listening. Continueing");
        reply_failure(MTK_NFC_FM_CARD_MODE_TEST_RSP);
        return 1;
      }

      // update the state machine
      st_mt_event_chstate(ST_FM_CM_CE_W84READER);

      // return success code to CLI now
      resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    case MTK_NFC_FM_VIRTUAL_CARD_REQ: {
      DECLARE_RESPONSE_AND_CHECK(FM_VIRTUAL_CARD_REQ, FM_VIRTUAL_CARD_RSP);

      // Check we are in a coherent state for this command.
      if (((le_to_h_32(req->action_le) == NFC_EM_ACT_START) &&
           (st_mt_g_state->state != ST_IDLE)) ||
          ((le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) &&
           (st_mt_g_state->state != ST_FM_CM_HCE_W84READER))) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command, reset mode first",
            __FUNCTION__, __LINE__, st_mt_g_state->state);
        if (st_mt_glue_resetmode() != 0) {
          return reply_failure(MTK_NFC_FM_VIRTUAL_CARD_RSP);
        }
      }

      // if it is a request to stop the CE mode, do it.
      if (le_to_h_32(req->action_le) == NFC_EM_ACT_STOP) {
        if (st_mt_glue_resetmode() != 0) {
          LOG(ERROR) << StringPrintf(
              "Fatal error while stopping the listening. Exiting");
          reply_failure(MTK_NFC_FM_VIRTUAL_CARD_RSP);
          return 2;
        }
        resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
        return st_mt_server_send((struct mtk_msg_hdr *)&resp);
      }

      // otherwise, it was a request to start the listening.
      LOG(INFO) << StringPrintf("Received FM: type=%x, brF=%x",
                                le_to_h_32(req->type_le),
                                le_to_h_32(req->type_f_rate_le));

      // set the listening parameters in the state.
      st_mt_g_state->ce_sel = st_mt_state::HCE;  // 0.

      st_mt_g_state->ce_techs.a =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_A ? 1 : 0;
      st_mt_g_state->ce_techs.b =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_B ? 1 : 0;
      st_mt_g_state->ce_techs.f =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_F ? 1 : 0;
      st_mt_g_state->ce_techs.f212 =
          le_to_h_32(req->type_f_rate_le) & EM_ALS_READER_M_SPDRATE_212 ? 1 : 0;
      st_mt_g_state->ce_techs.f424 =
          le_to_h_32(req->type_f_rate_le) & EM_ALS_READER_M_SPDRATE_424 ? 1 : 0;
      st_mt_g_state->ce_techs.bprime =
          le_to_h_32(req->type_le) & EM_ALS_READER_M_TYPE_Bprime ? 1 : 0;

      // start the listening
      if (st_mt_glue_startmode(0, NULL, NULL, 1, hce_cb_fm) != 0) {
        LOG(ERROR) << StringPrintf(
            "Error while starting the hce listening. Continueing");
        reply_failure(MTK_NFC_FM_VIRTUAL_CARD_RSP);
        return 1;
      }

      // update the state machine
      st_mt_event_chstate(ST_FM_CM_HCE_W84READER);

      // return success code to CLI immediatly. Mode will be reset on successful
      // read or on timeout.
      resp.result_le = le_to_h_32(MTK_RESULT_SUCCESS);
      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    case MTK_NFC_FM_ANTENNA_TEST_REQ: {
      int r;
      DECLARE_RESPONSE_AND_CHECK(FM_ANTENNA_TEST_REQ, FM_ANTENNA_TEST_RSP);

      if (st_mt_g_state->state != ST_IDLE) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command, reset mode first",
            __FUNCTION__, __LINE__, st_mt_g_state->state);
        if (st_mt_glue_resetmode() != 0) {
          return reply_failure(MTK_NFC_FM_ANTENNA_TEST_RSP);
        }
      }

      // Proceed.
      r = st_mt_glue_test_antenna_DIAG_MEAS(
          &resp.Diff_Amplitude, &resp.Diff_Phase, &resp.RFO1_Amplitude,
          &resp.RFO1_Phase, &resp.RFO2_Amplitude, &resp.RFO2_Phase, &resp.VDDRF,
          0x10,  // Measure at 3.6V. This requires VBAT >= 3.8V
          125    // measure 125 times.
      );
      if (r != 0) {
        LOG(ERROR) << StringPrintf(
            "Fatal error while testing antenna. Exiting");
        reply_failure(MTK_NFC_FM_ANTENNA_TEST_RSP);
        return 2;
      }

      resp.result_le = le_to_h_32(r);
      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    case MTK_NFC_META_GET_SELIST_REQ: {
      int r = 0;
      DECLARE_RESPONSE_AND_CHECK(META_GET_SELIST_REQ, META_GET_SELIST_RSP);

      if (st_mt_g_state->state != ST_IDLE) {
        LOG(ERROR) << StringPrintf(
            "%s:%d : Invalid state (%d) for this command", __FUNCTION__,
            __LINE__, st_mt_g_state->state);
        return reply_failure(MTK_NFC_META_GET_SELIST_RSP);
      }

      // Proceed.
      LOG(INFO) << StringPrintf("%s: Hard-coded list of SE at the moment.",
                                __FUNCTION__);
      resp.SeCount = 3;
      // UICC 1
      resp.SE[0].seid_le = le_to_h_32(1);
      resp.SE[0].status_le = le_to_h_32(ENABLE);
      resp.SE[0].type_le = le_to_h_32(UICC);
      resp.SE[0].connecttype_le = le_to_h_32(SE_CONTACTLESS);
      resp.SE[0].lowpowermode_le = le_to_h_32(ENABLE);
      resp.SE[0].pbf_le = le_to_h_32(DISABLE);
      // UICC 2
      resp.SE[1].seid_le = le_to_h_32(2);
      resp.SE[1].status_le = le_to_h_32(ENABLE);
      resp.SE[1].type_le = le_to_h_32(UICC);
      resp.SE[1].connecttype_le = le_to_h_32(SE_CONTACTLESS);
      resp.SE[1].lowpowermode_le = le_to_h_32(ENABLE);
      resp.SE[1].pbf_le = le_to_h_32(DISABLE);
      // eSE
      resp.SE[2].seid_le = le_to_h_32(3);
      resp.SE[2].status_le = le_to_h_32(ENABLE);
      resp.SE[2].type_le = le_to_h_32(EMBEDDED_SE);
      resp.SE[2].connecttype_le = le_to_h_32(SE_ALL);
      resp.SE[2].lowpowermode_le = le_to_h_32(ENABLE);
      resp.SE[2].pbf_le = le_to_h_32(DISABLE);
      //

      resp.result_le = le_to_h_32(r);
      return st_mt_server_send((struct mtk_msg_hdr *)&resp);
    } break;

    default:
      LOG(ERROR) << StringPrintf(
          "Command %d not implemented, simply ignoring and not responding",
          cmd);
      return 1;
  }

  return 2;
}

// Some states are waiting for an NFC event before unblocking the server. We add
// a timeout behavior here. returns 0 on success, 1 on warning, 2 on fatal error
static int handle_timeouts() {
  // The following states have a timeout:
  if ((st_mt_g_state->state == ST_TM_OPTIONCHECK_W84TAG) ||
      (st_mt_g_state->state == ST_FM_READDEP_W84TAG) ||
      (st_mt_g_state->state == ST_FM_CM_CE_W84READER) ||
      (st_mt_g_state->state == ST_FM_CM_HCE_W84READER)) {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
      LOG(ERROR) << StringPrintf("clock_gettime failed: %d (%s)", errno,
                                 strerror(errno));
    }
    // Timeout is set to 60-ish seconds
    if (ts.tv_sec > (st_mt_g_state->since.tv_sec + W84_TIMEOUT)) {
      int resptype = 0;
      LOG(INFO) << StringPrintf(
          "Timeout in state %d, going back to idle and sending error",
          st_mt_g_state->state);

      // Return corresponding failure.
      if (st_mt_g_state->state == ST_TM_OPTIONCHECK_W84TAG) {
        resptype = MTK_NFC_TESTMODE_SETTING_RSP;
      }
      if (st_mt_g_state->state == ST_FM_READDEP_W84TAG) {
        resptype = MTK_NFC_FM_READ_DEP_TEST_RSP;
      }
      if (st_mt_g_state->state == ST_FM_CM_CE_W84READER) {
        resptype = 0;
      }
      if (st_mt_g_state->state == ST_FM_CM_HCE_W84READER) {
        resptype = 0;
      }

      if (st_mt_glue_resetmode() != 0) {
        return 2;
      }

      if (resptype) {
        return reply_failure(resptype);
      }
    }
  }

  return 0;
}

// Change the state in the state machine. The mutex must be held by caller.
void st_mt_event_chstate(enum mt_state newstate) {
  LOG(INFO) << StringPrintf("State %d -> %d", st_mt_g_state->state, newstate);
  st_mt_g_state->state = newstate;
  if (clock_gettime(CLOCK_REALTIME, &st_mt_g_state->since) != 0) {
    LOG(ERROR) << StringPrintf("clock_gettime failed: %d (%s)", errno,
                               strerror(errno));
  }
}

void st_mt_event_loop() {
  int ret;

  ret = pthread_mutex_lock(&st_mt_g_state->mtx);
  if (ret != 0) {
    LOG(ERROR) << StringPrintf("Error while locking mutex: %s", strerror(ret));
    return;
  }

  do {
    struct timespec ts;
    int pres_check_timeout = 0;

    // We wake up every 200ms; this is the timer for presence check.
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
      LOG(ERROR) << StringPrintf("clock_gettime failed: %d (%s)", errno,
                                 strerror(errno));
    }
    {
      ts.tv_nsec += 200000000;
      if (ts.tv_nsec >= 1000000000) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000;
      }
    }
    ret =
        pthread_cond_timedwait(&st_mt_g_state->cond, &st_mt_g_state->mtx, &ts);
    if ((ret != 0) && (ret != ETIMEDOUT)) {
      LOG(ERROR) << StringPrintf("Error while waiting for cond: %s",
                                 strerror(ret));
      break;
    }
    if (ret == ETIMEDOUT) {
      pres_check_timeout = 1;
    }

    // events here; only if there is a client, otherwise just continue waiting..
    if (st_mt_g_state->clisock >= 0) {
      ret = handle_timeouts();
      if (ret > 0) {
        LOG(ERROR) << StringPrintf("Problem while processing timeout (%s)",
                                   (ret == 1) ? "continue" : "abord");
        if (ret == 2) {
          break;
        }
      }

      ret = st_mt_glue_handle_events(pres_check_timeout);
      if (ret > 0) {
        LOG(ERROR) << StringPrintf("Problem while processing NFC event (%s)",
                                   (ret == 1) ? "continue" : "abord");
        if (ret == 2) {
          break;
        }
      }

      // now check if we have anything to do -- we are holding the mutex
      if (st_mt_g_state->recv_msg) {
        // pop the message
        struct mtk_msg_hdr *m = st_mt_g_state->recv_msg;
        st_mt_g_state->recv_msg = NULL;

        LOG(INFO) << StringPrintf("%s: picked next message (%d)", __FUNCTION__,
                                  le_to_h_32(m->cmd_le));

        // signal the condition in case the server handler was waiting to push
        // another message
        (void)pthread_cond_broadcast(&st_mt_g_state->cond);

        // process it
        ret = process_client_message(m);
        free(m);
        if (ret > 0) {
          LOG(ERROR) << StringPrintf(
              "Problem while processing a client message (%s)",
              (ret == 1) ? "continue" : "abord");
          if (ret == 2) {
            break;
          }
        }
      }
    }

  } while (st_mt_g_state->exiting == 0);

  (void)pthread_mutex_unlock(&st_mt_g_state->mtx);

  return;
}
