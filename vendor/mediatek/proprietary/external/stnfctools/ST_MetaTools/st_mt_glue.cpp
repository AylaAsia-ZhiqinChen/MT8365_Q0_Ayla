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

// this file controls the NFA stack for the purpose of MTK tools tests only.

#include "st_mtktools.h"
#include "NfcAdaptation.h"
#include "SyncEvent.h"

/////////////////////////////////////

#define CLF_ID 0x00
#define ID_MGMT_GATE_ID 0x05
#define LOOPBACK_GATE_ID 0x04

#define VERSION_SW_REG_IDX 0x01
#define VERSION_HW_REG_IDX 0x03

#ifndef NCI_PARAM_ID_PACM_BIT_RATE
#define NCI_PARAM_ID_PACM_BIT_RATE 0x68
#endif  // NCI_PARAM_ID_PACM_BIT_RATE

#define SET_NCIPARAM(param_id, param_val)                                 \
  {                                                                       \
    uint8_t parameter = param_id;                                         \
    uint8_t value[] = {param_val};                                        \
    LOG(INFO) << StringPrintf("%s: Setting " #param_id " to " #param_val, \
                              __FUNCTION__);                              \
    sNfaSetConfigEvent.start(true);                                       \
    stat = NFA_SetConfig(parameter, sizeof(value), &value[0]);            \
    if (stat == NFA_STATUS_OK) {                                          \
      sNfaSetConfigEvent.wait();                                          \
    } else {                                                              \
      sNfaSetConfigEvent.end();                                           \
      LOG(ERROR) << StringPrintf("%s: Error setting " #param_id           \
                                 " to " #param_val ": %x",                \
                                 __FUNCTION__, stat);                     \
      return 1;                                                           \
    }                                                                     \
    sNfaSetConfigEvent.end();                                             \
  }

#define UNSET_NCIPARAM(param_id)                                               \
  {                                                                            \
    uint8_t parameter = param_id;                                              \
    LOG(INFO) << StringPrintf("%s: Setting " #param_id " to empty",            \
                              __FUNCTION__);                                   \
    sNfaSetConfigEvent.start(true);                                            \
    stat = NFA_SetConfig(parameter, 0, NULL);                                  \
    if (stat == NFA_STATUS_OK) {                                               \
      sNfaSetConfigEvent.wait();                                               \
    } else {                                                                   \
      sNfaSetConfigEvent.end();                                                \
      LOG(ERROR) << StringPrintf(                                              \
          "%s: Error setting " #param_id " to empty: %x", __FUNCTION__, stat); \
      return 1;                                                                \
    }                                                                          \
    sNfaSetConfigEvent.end();                                                  \
  }

#define MAX_NUM_EE 5

/*****************************************************************************
**
** private variables and functions
**
*****************************************************************************/
static SyncEvent sNfaEnableEvent;   // event for NFA_Enable()
static SyncEvent sNfaDisableEvent;  // event for NFA_Disable()
static SyncEvent
    sNfaEnableDisablePollingEvent;  // event for NFA_EnablePolling(),
                                    // NFA_DisablePolling()
static SyncEvent sNfaP2pEvent;  // event for NFA_P2pListenTech, NFA_resumeP2p
static SyncEvent sNfaSetConfigEvent;  // event for Set_Config....
static SyncEvent sNfaGetConfigEvent;  // event for Get_Config....
static SyncEvent stimer;  // timer to try to enable again  NFA_Enable()
static SyncEvent
    sNfaPresenceCheck;  // a presence check is flying, waiting for result.
static bool sPresenceConfirmed = false;  // result of the last presence check.
static SyncEvent sNdefCheck;             // wait for rsult of ndef operation
static tNFA_NDEF_DETECT sNdefStatus;
static tNFA_STATUS sNdefResult;
static SyncEvent sEeCheck;  // wait for rsult of ndef operation
static tNFA_EE_CBACK_DATA sEeStatus;
static bool sEeForcedRouting = false;
static tNFA_HANDLE sActiveEe = 0;  // currently active EE handle
static bool sActiveEeError = false;
static SyncEvent sEeActiveStarted;    // if EE is doing a full activation
static SyncEvent sEeActiveCompleted;  // EE full activation completed
static SyncEvent sHceEvent;           // event for ...
static bool sIsHceEnabled = false;
static bool sIsHceFEnabling = false;
static bool sIsHceFEnabled = false;
static tNFA_HANDLE sHceHandle;
static tNFA_HANDLE sHceFHandle;
static struct ndef_msg sNdefMessage;
static SyncEvent sCeUiccListenCheck;
static tNFA_STATUS sCeResult;
static SyncEvent sNfaVSEvent;  // event for NFA_SendVsCommand()
static tNFA_STATUS sNfaVSStatus;
static uint8_t sNfaVSResp[256];  // copy the first 256 bytes of last reply here
static SyncEvent sNfaHciRegisterEvent;
static tNFA_HANDLE sHciHandle;
static SyncEvent sNfaHciCreatePipeEvent;
static uint8_t sCreatedPipeId;
static SyncEvent sNfaHciListPipeEvent;
static SyncEvent sNfaHciHostListEvent;
static SyncEvent sNfaHciAllocGateEvent;
static SyncEvent sNfaHciOpenPipeEvent;
static SyncEvent sNfaHciClosePipeEvent;
static SyncEvent sNfaHciDeletePipeEvent;
static SyncEvent sNfaHciGetRegRspEvent;
static SyncEvent sNfaHciEventRcvdEvent;
static SyncEvent sHciRspRcvdEvent;
static SyncEvent sHotPlugEvent;
static bool sIsInhibited = false;
static uint16_t sRspSize;
static bool sIsNfaEnabled = false;
static bool sPollingEnabled = false;  // is polling for tag?
static bool sIsDisabling = false;
static bool sRfEnabled = false;  // whether RF discovery is enabled
static bool sIsP2pListening = false;
static bool sP2pActive = false;  // whether p2p was last active
static uint8_t sLlcpDiscoType;
static uint8_t sLlcpBitrate;
static tNFA_HANDLE sP2psHandle;
static int current_rf_merge = 0;

static const uint8_t ESE_ID = 0xC0;
static const uint8_t UICC_ID = 0x02;

static struct gate_info {
  bool added;
  bool created;
  bool open_pending;
  bool close_pending;
  bool opened;
  uint8_t pipe_id;
  uint8_t version_sw[3];
  uint8_t version_hw[3];
} sLoopbackInfo, sIdMgmtInfo;

static uint16_t sCurrentConfigLen;
static uint8_t sConfig[256];

static tNFA_TECHNOLOGY_MASK sPollingMask;
static tNFA_TECHNOLOGY_MASK sListeningMask;
static tNFA_TECHNOLOGY_MASK sP2pListeningMask;

static uint8_t sFwVersion[4];
static uint8_t sHwVersion[2];

static void nfaConnectionCallback(uint8_t event, tNFA_CONN_EVT_DATA *eventData);
static void nfaDeviceManagementCallback(uint8_t event,
                                        tNFA_DM_CBACK_DATA *eventData);
static void deactivate_activeSE();
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

/*******************************************************************************
**
** Function:        nfaDeviceManagementCallback
**
** Description:     Receive device management events from stack.
**                  dmEvent: Device-management event ID.
**                  eventData: Data associated with event ID.
**
** Returns:         None
**
*******************************************************************************/
static void nfaDeviceManagementCallback(uint8_t dmEvent,
                                        tNFA_DM_CBACK_DATA *eventData) {
  LOG(INFO) << StringPrintf("%s: enter; event=0x%X", __FUNCTION__, dmEvent);

  switch (dmEvent) {
    case NFA_DM_ENABLE_EVT: { /* Result of NFA_Enable */
      LOG(INFO) << StringPrintf("%s: NFA_DM_ENABLE_EVT; status=0x%X",
                                __FUNCTION__, eventData->enable.status);
      sNfaEnableEvent.start();
      sIsNfaEnabled = eventData->enable.status == NFA_STATUS_OK;
      sFwVersion[0] = eventData->enable.manu_specific_info[5];
      sFwVersion[1] = eventData->enable.manu_specific_info[4];
      sFwVersion[2] = eventData->enable.manu_specific_info[3];
      sFwVersion[3] = eventData->enable.manu_specific_info[2];
      sHwVersion[0] = eventData->enable.manu_specific_info[1];
      sHwVersion[1] = eventData->enable.manu_specific_info[0];
      sNfaEnableEvent.notifyOne();
      sNfaEnableEvent.end();
    } break;
    case NFA_DM_DISABLE_EVT: { /* Result of NFA_Disable */
      LOG(INFO) << StringPrintf("%s: NFA_DM_DISABLE_EVT", __FUNCTION__);
      sNfaDisableEvent.start();
      sIsNfaEnabled = false;
      sNfaDisableEvent.notifyOne();
      sNfaDisableEvent.end();
    } break;

    case NFA_DM_SET_CONFIG_EVT:  // result of NFA_SetConfig
      LOG(INFO) << StringPrintf("%s: NFA_DM_SET_CONFIG_EVT", __FUNCTION__);
      {
        sNfaSetConfigEvent.start();
        sNfaSetConfigEvent.notifyOne();
        sNfaSetConfigEvent.end();
      }
      break;

    case NFA_DM_GET_CONFIG_EVT: /* Result of NFA_GetConfig */
      LOG(INFO) << StringPrintf("%s: NFA_DM_GET_CONFIG_EVT", __FUNCTION__);
      {
        sNfaGetConfigEvent.start();
        if (eventData->status == NFA_STATUS_OK &&
            eventData->get_config.tlv_size <= sizeof(sConfig)) {
          sCurrentConfigLen = eventData->get_config.tlv_size;
          memcpy(sConfig, eventData->get_config.param_tlvs,
                 eventData->get_config.tlv_size);
        } else {
          LOG(ERROR) << StringPrintf("%s: NFA_DM_GET_CONFIG failed",
                                     __FUNCTION__);
          sCurrentConfigLen = 0;
        }
        sNfaGetConfigEvent.notifyOne();
        sNfaGetConfigEvent.end();
      }
      break;

    case NFA_DM_RF_FIELD_EVT:
      LOG(INFO) << StringPrintf(
          "%s: NFA_DM_RF_FIELD_EVT; status=0x%X; field status=%u", __FUNCTION__,
          eventData->rf_field.status, eventData->rf_field.rf_field_status);
      break;

    case NFA_DM_NFCC_TRANSPORT_ERR_EVT:
    case NFA_DM_NFCC_TIMEOUT_EVT: {
      if (dmEvent == NFA_DM_NFCC_TIMEOUT_EVT) {
        LOG(ERROR) << StringPrintf("%s: NFA_DM_NFCC_TIMEOUT_EVT; abort",
                                   __FUNCTION__);
      } else if (dmEvent == NFA_DM_NFCC_TRANSPORT_ERR_EVT) {
        LOG(ERROR) << StringPrintf("%s: NFA_DM_NFCC_TRANSPORT_ERR_EVT; abort",
                                   __FUNCTION__);
      }

      // NFA_Disable(FALSE);

      LOG(ERROR) << StringPrintf("%s: crash NFC service", __FUNCTION__);
      //////////////////////////////////////////////
      // crash the NFC service process so it can restart automatically
      abort();
      //////////////////////////////////////////////
    } break;

    default:
      LOG(INFO) << StringPrintf("%s: unhandled event", __FUNCTION__);
      break;
  }
}

/*******************************************************************************
**
** Function:        nfaConnectionCallback
**
** Description:     Receive connection-related events from stack.
**                  connEvent: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
static void nfaConnectionCallback(uint8_t connEvent,
                                  tNFA_CONN_EVT_DATA *eventData) {
  tNFA_STATUS status = NFA_STATUS_FAILED;

  int asynchronous = 0;

  LOG(INFO) << StringPrintf("%s: event= %u", __FUNCTION__, connEvent);

  switch (connEvent) {
      //
      // The following events are handled synchronously (i.e. main thread is
      // blocked while waiting for this result).
      //
    case NFA_POLL_ENABLED_EVT: {  // whether polling successfully started
      LOG(INFO) << StringPrintf("%s: NFA_POLL_ENABLED_EVT: status = %u",
                                __FUNCTION__, eventData->status);
      sNfaEnableDisablePollingEvent.start();
      sNfaEnableDisablePollingEvent.notifyOne();
      sNfaEnableDisablePollingEvent.end();
    } break;

    case NFA_POLL_DISABLED_EVT: {  // Listening/Polling stopped
      LOG(INFO) << StringPrintf("%s: NFA_POLL_DISABLED_EVT: status = %u",
                                __FUNCTION__, eventData->status);
      sNfaEnableDisablePollingEvent.start();
      sNfaEnableDisablePollingEvent.notifyOne();
      sNfaEnableDisablePollingEvent.end();
    } break;

    case NFA_LISTEN_ENABLED_EVT:
      LOG(INFO) << StringPrintf("%s: NFA_LISTEN_ENABLED_EVT", __FUNCTION__);
      break;

    case NFA_LISTEN_DISABLED_EVT:
      LOG(INFO) << StringPrintf("%s: NFA_LISTEN_DISABLED_EVT", __FUNCTION__);
      break;

    case NFA_RF_DISCOVERY_STARTED_EVT: {  // RF Discovery started
      LOG(INFO) << StringPrintf("%s: NFA_RF_DISCOVERY_STARTED_EVT: status = %u",
                                __FUNCTION__, eventData->status);
      sNfaEnableDisablePollingEvent.start();
      sNfaEnableDisablePollingEvent.notifyOne();
      sNfaEnableDisablePollingEvent.end();
    } break;

    case NFA_RF_DISCOVERY_STOPPED_EVT: {  // RF Discovery stopped event
      LOG(INFO) << StringPrintf("%s: NFA_RF_DISCOVERY_STOPPED_EVT: status = %u",
                                __FUNCTION__, eventData->status);
      sNfaEnableDisablePollingEvent.start();
      sNfaEnableDisablePollingEvent.notifyOne();
      sNfaEnableDisablePollingEvent.end();
    } break;

    case NFA_SET_P2P_LISTEN_TECH_EVT: {
      LOG(INFO) << StringPrintf("%s: NFA_SET_P2P_LISTEN_TECH_EVT",
                                __FUNCTION__);
      sNfaP2pEvent.start();
      sNfaP2pEvent.notifyOne();
      sNfaP2pEvent.end();
    } break;

    case NFA_P2P_PAUSED_EVT: {
      LOG(INFO) << StringPrintf("%s: NFA_P2P_PAUSED_EVT", __FUNCTION__);
      sNfaP2pEvent.start();
      sNfaP2pEvent.notifyOne();
      sNfaP2pEvent.end();
    } break;

    case NFA_P2P_RESUMED_EVT: {
      LOG(INFO) << StringPrintf("%s: NFA_P2P_RESUMED_EVT", __FUNCTION__);
      sNfaP2pEvent.start();
      sNfaP2pEvent.notifyOne();
      sNfaP2pEvent.end();
    } break;

    case NFA_SELECT_RESULT_EVT:  // NFC link/protocol discovery select response
      LOG(INFO) << StringPrintf(
          "%s: NFA_SELECT_RESULT_EVT: status = %d, sIsDisabling=%d",
          __FUNCTION__, eventData->status, sIsDisabling);

      if (sIsDisabling) {
        break;
      }

      if (eventData->status != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf(
            "%s: NFA_SELECT_RESULT_EVT error: status = %d", __FUNCTION__,
            eventData->status);
        NFA_Deactivate(FALSE);
      }
      break;

    case NFA_DEACTIVATE_FAIL_EVT:
      LOG(INFO) << StringPrintf("%s: NFA_DEACTIVATE_FAIL_EVT: status = %d",
                                __FUNCTION__, eventData->status);
      break;

    case NFA_SELECT_CPLT_EVT:  // Select completed
      status = eventData->status;
      LOG(INFO) << StringPrintf("%s: NFA_SELECT_CPLT_EVT: status = %d",
                                __FUNCTION__, status);
      if (status != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: NFA_SELECT_CPLT_EVT error: status = %d",
                                   __FUNCTION__, status);
      }
      break;

    case NFA_TLV_DETECT_EVT:  // TLV Detection complete
      status = eventData->tlv_detect.status;
      LOG(INFO) << StringPrintf(
          "%s: NFA_TLV_DETECT_EVT: status = %d, protocol = %d, num_tlvs = %d, "
          "num_bytes = %d",
          __FUNCTION__, status, eventData->tlv_detect.protocol,
          eventData->tlv_detect.num_tlvs, eventData->tlv_detect.num_bytes);
      if (status != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: NFA_TLV_DETECT_EVT error: status = %d",
                                   __FUNCTION__, status);
      }
      break;

    case NFA_DATA_EVT:  // Data message received (for non-NDEF reads) -- we don
                        // t use this in tools
      LOG(INFO) << StringPrintf("%s: NFA_DATA_EVT: status = 0x%X, len = %d",
                                __FUNCTION__, eventData->status,
                                eventData->data.len);
      break;

    case NFA_SET_TAG_RO_EVT:  // Tag set as Read only -- we don t use this in
                              // tools
      LOG(INFO) << StringPrintf("%s: NFA_SET_TAG_RO_EVT: status = %d",
                                __FUNCTION__, eventData->status);
      break;

    case NFA_I93_CMD_CPLT_EVT:
      LOG(INFO) << StringPrintf("%s: NFA_I93_CMD_CPLT_EVT: status=0x%X",
                                __FUNCTION__, eventData->status);
      break;

    case NFA_PRESENCE_CHECK_EVT:
      LOG(INFO) << StringPrintf("%s: NFA_PRESENCE_CHECK_EVT", __FUNCTION__);
      {
        sNfaPresenceCheck.start();
        sPresenceConfirmed = (eventData->status == NFA_STATUS_OK);
        sNfaPresenceCheck.notifyOne();
        sNfaPresenceCheck.end();
      }
      break;

    case NFA_NDEF_DETECT_EVT:  // NDEF Detection complete;
      // if status is failure, it means the tag does not contain any or valid
      // NDEF data;
      status = eventData->ndef_detect.status;
      LOG(INFO) << StringPrintf(
          "%s: NFA_NDEF_DETECT_EVT: status = 0x%X, protocol = %u, "
          "max_size = %u, cur_size = %u, flags = 0x%X",
          __FUNCTION__, status, eventData->ndef_detect.protocol,
          eventData->ndef_detect.max_size, eventData->ndef_detect.cur_size,
          eventData->ndef_detect.flags);
      {
        sNdefCheck.start();
        memcpy(&sNdefStatus, &eventData->ndef_detect, sizeof(tNFA_NDEF_DETECT));
        sNdefCheck.notifyOne();
        sNdefCheck.end();
      }
      break;

    case NFA_RW_INTF_ERROR_EVT:
      LOG(ERROR) << StringPrintf("%s: NFC_RW_INTF_ERROR_EVT", __FUNCTION__);
      {
        NFA_Deactivate(FALSE);
        sNdefCheck.start();
        sNdefResult = NFA_STATUS_FAILED;
        sNdefCheck.notifyOne();
        sNdefCheck.end();
      }
      break;
    case NFA_READ_CPLT_EVT:  // NDEF-read or tag-specific-read completed
      LOG(INFO) << StringPrintf("%s: NFA_READ_CPLT_EVT: status = 0x%X",
                                __FUNCTION__, eventData->status);
      if (eventData->status != NFA_STATUS_OK) {
        NFA_Deactivate(FALSE);
        sNdefCheck.start();
        sNdefResult = NFA_STATUS_FAILED;
        sNdefCheck.notifyOne();
        sNdefCheck.end();
      }
      break;

    case NFA_WRITE_CPLT_EVT:  // Write completed
      LOG(INFO) << StringPrintf("%s: NFA_WRITE_CPLT_EVT: status = %d",
                                __FUNCTION__, eventData->status);
      {
        sNdefCheck.start();
        sNdefResult = eventData->status;
        sNdefCheck.notifyOne();
        sNdefCheck.end();
      }
      break;

    case NFA_FORMAT_CPLT_EVT:
      LOG(INFO) << StringPrintf("%s: NFA_FORMAT_CPLT_EVT: status=0x%X",
                                __FUNCTION__, eventData->status);
      {
        sNdefCheck.start();
        sNdefResult = eventData->status;
        sNdefCheck.notifyOne();
        sNdefCheck.end();
      }
      break;

    case NFA_LLCP_FIRST_PACKET_RECEIVED_EVT:  // Received first packet over llcp
      LOG(INFO) << StringPrintf("%s: NFA_LLCP_FIRST_PACKET_RECEIVED_EVT",
                                __FUNCTION__);
      //        PeerToPeer::getInstance().llcpFirstPacketHandler (getNative(0,
      //        0));
      break;

    case NFA_CE_UICC_LISTEN_CONFIGURED_EVT:
      LOG(INFO) << StringPrintf(
          "%s: NFA_CE_UICC_LISTEN_CONFIGURED_EVT : status=0x%X", __FUNCTION__,
          eventData->status);
      {
        sCeUiccListenCheck.start();
        sCeResult = eventData->status;
        sCeUiccListenCheck.notifyOne();
        sCeUiccListenCheck.end();
      }
      break;

      //
      // The following events are handled asynchronously (i.e. main thread will
      // handle these from st_mt_glue_handle_events()).
      //
    case NFA_DISC_RESULT_EVT:  // NFC link/protocol discovery notificaiton
      status = eventData->disc_result.status;
      LOG(INFO) << StringPrintf("%s: NFA_DISC_RESULT_EVT: status = %d",
                                __FUNCTION__, status);
      asynchronous = 1;
      break;

    case NFA_ACTIVATED_EVT:  // NFC link/protocol activated
      LOG(INFO) << StringPrintf(
          "%s: NFA_ACTIVATED_EVT (intf:%d)", __FUNCTION__,
          eventData->activated.activate_ntf.intf_param.type);
      asynchronous = 1;
      break;

    case NFA_DEACTIVATED_EVT:  // NFC link/protocol deactivated
      LOG(INFO) << StringPrintf("%s: NFA_DEACTIVATED_EVT   Type: %u",
                                __FUNCTION__, eventData->deactivated.type);
      sNfaEnableDisablePollingEvent.start();
      if (sRfEnabled) {
        asynchronous =
            1;  // otherwise we are stopping the polling so we just ignore this.
      }
      sNfaEnableDisablePollingEvent.end();
      break;

    case NFA_LLCP_ACTIVATED_EVT:  // LLCP link is activated
      LOG(INFO) << StringPrintf(
          "%s: NFA_LLCP_ACTIVATED_EVT: is_initiator: %d  remote_wks: %d, "
          "remote_lsc: %d, remote_link_miu: %d, local_link_miu: %d",
          __FUNCTION__, eventData->llcp_activated.is_initiator,
          eventData->llcp_activated.remote_wks,
          eventData->llcp_activated.remote_lsc,
          eventData->llcp_activated.remote_link_miu,
          eventData->llcp_activated.local_link_miu);

      //        PeerToPeer::getInstance().llcpActivatedHandler (getNative(0, 0),
      //        eventData->llcp_activated);
      asynchronous = 1;
      break;

    case NFA_LLCP_DEACTIVATED_EVT:  // LLCP link is deactivated
      LOG(INFO) << StringPrintf("%s: NFA_LLCP_DEACTIVATED_EVT", __FUNCTION__);
      //       PeerToPeer::getInstance().llcpDeactivatedHandler (getNative(0,
      //       0), eventData->llcp_deactivated);
      asynchronous = 1;
      break;

////////////////////////////////////////////////
#if 0


        case NFA_CE_NDEF_WRITE_START_EVT: // NDEF write started
            LOG(INFO) << StringPrintf("%s: NFA_CE_NDEF_WRITE_START_EVT: status: %d", __FUNCTION__, eventData->status);

            if (eventData->status != NFA_STATUS_OK) {
                LOG(ERROR) << StringPrintf("%s: NFA_CE_NDEF_WRITE_START_EVT error: status = %d", __FUNCTION__, eventData->status);
            }
            break;

        case NFA_CE_NDEF_WRITE_CPLT_EVT: // NDEF write completed
            LOG(INFO) << StringPrintf("%s: FA_CE_NDEF_WRITE_CPLT_EVT: len = %lu", __FUNCTION__, eventData->ndef_write_cplt.len);
            break;

        case NFA_CE_DEREGISTERED_EVT: {
            LOG(INFO) << StringPrintf("%s: NFA_CE_DEREGISTERED_EVT", __FUNCTION__);
            NfcStExtensions::getInstance().nfaConnectionCallback(connEvent, eventData);
        }
        break;
#endif
    default:
      LOG(ERROR) << StringPrintf("%s: unknown event %hhx ????", __FUNCTION__,
                                 connEvent);
      break;
  }

  if (asynchronous) {
    int ret;
    ret = pthread_mutex_lock(&st_mt_g_state->mtx);
    if (ret != 0) {
      LOG(ERROR) << StringPrintf("%s: Failed to lock mutex ! %s", __FUNCTION__,
                                 strerror(ret));
    }

    memcpy(&st_mt_g_state->CONN_event_data, eventData, sizeof(*eventData));
    st_mt_g_state->CONN_event_code = connEvent;
    st_mt_g_state->has_CONN_event = 1;

    ret = pthread_cond_broadcast(&st_mt_g_state->cond);
    if (ret != 0) {
      LOG(ERROR) << StringPrintf("%s: Failed to signal cond ! %s", __FUNCTION__,
                                 strerror(ret));
    }

    ret = pthread_mutex_unlock(&st_mt_g_state->mtx);
    if (ret != 0) {
      LOG(ERROR) << StringPrintf("%s: Failed to unlock mutex ! %s",
                                 __FUNCTION__, strerror(ret));
    }
  }
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
static void st_mt_glue_p2ps_cb(tNFA_P2P_EVT event, tNFA_P2P_EVT_DATA *p_data) {
  LOG(INFO) << StringPrintf("%s: event = %02hhx", __FUNCTION__, event);

  switch (event) {
    case NFA_P2P_REG_SERVER_EVT:
      LOG(INFO) << StringPrintf("%s: event = NFA_P2P_REG_SERVER_EVT",
                                __FUNCTION__);
      sP2psHandle = p_data->reg_server.server_handle;
      break;

    case NFA_P2P_REG_CLIENT_EVT:
      LOG(INFO) << StringPrintf("%s: event = NFA_P2P_REG_CLIENT_EVT",
                                __FUNCTION__);
      break;

    case NFA_P2P_ACTIVATED_EVT:
      LOG(INFO) << StringPrintf("%s: event = NFA_P2P_ACTIVATED_EVT",
                                __FUNCTION__);
      break;

    case NFA_P2P_DEACTIVATED_EVT:
      LOG(INFO) << StringPrintf("%s: event = NFA_P2P_DEACTIVATED_EVT",
                                __FUNCTION__);
      break;

    case NFA_P2P_CONN_REQ_EVT:
      LOG(INFO) << StringPrintf("%s: event = NFA_P2P_CONN_REQ_EVT",
                                __FUNCTION__);
      break;

    default:
      LOG(INFO) << StringPrintf("%s: event = %02hhx", __FUNCTION__, event);
  }
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
static void st_mt_glue_ee_cb(tNFA_EE_EVT event, tNFA_EE_CBACK_DATA *p_data) {
  LOG(INFO) << StringPrintf("%s: event = %02hhx", __FUNCTION__, event);

#define SHOW_DETAIL_AND_NOTIFY(evt)                                \
  case evt:                                                        \
    LOG(INFO) << StringPrintf("%s: event = " #evt " 1st field %x", \
                              __FUNCTION__, p_data->status);       \
    {                                                              \
      sEeCheck.start();                                            \
      memcpy(&sEeStatus, p_data, sizeof(tNFA_EE_CBACK_DATA));      \
      sEeCheck.notifyOne();                                        \
      sEeCheck.end();                                              \
    }                                                              \
    break

#define SHOW_DETAIL_ONLY(evt)                                      \
  case evt:                                                        \
    LOG(INFO) << StringPrintf("%s: event = " #evt " 1st field %x", \
                              __FUNCTION__, p_data->status);       \
    break

  switch (event) {
    SHOW_DETAIL_AND_NOTIFY(NFA_EE_DISCOVER_EVT);
    SHOW_DETAIL_AND_NOTIFY(NFA_EE_REGISTER_EVT);
    SHOW_DETAIL_AND_NOTIFY(NFA_EE_DEREGISTER_EVT);
    SHOW_DETAIL_AND_NOTIFY(NFA_EE_MODE_SET_EVT);

    case NFA_EE_STATUS_NTF_EVT:
      LOG(INFO) << StringPrintf(
          "%s: event = NFA_EE_STATUS_NTF_EVT 1st field %x", __FUNCTION__,
          p_data->status);
      if (p_data->status_ntf.status == NFCEE_STATUS_INIT_COMPLETED) {
        sEeActiveCompleted.start();
        sEeActiveCompleted.notifyOne();
        sEeActiveCompleted.end();
      } else if (p_data->status_ntf.status == NFCEE_STATUS_INIT_STARTED) {
        sEeActiveStarted.start();
        sEeActiveStarted.notifyOne();
        sEeActiveStarted.end();
      }
      break;

      SHOW_DETAIL_ONLY(NFA_EE_POWER_CTRL_EVT);
      SHOW_DETAIL_AND_NOTIFY(NFA_EE_FORCE_ROUTING_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_ADD_AID_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_REMOVE_AID_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_ADD_SYSCODE_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_REMOVE_SYSCODE_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_REMAINING_SIZE_EVT);
      SHOW_DETAIL_AND_NOTIFY(NFA_EE_SET_TECH_CFG_EVT);
      SHOW_DETAIL_AND_NOTIFY(NFA_EE_CLEAR_TECH_CFG_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_SET_PROTO_CFG_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_CLEAR_PROTO_CFG_EVT);
      SHOW_DETAIL_AND_NOTIFY(NFA_EE_UPDATED_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_CONNECT_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_DATA_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_DISCONNECT_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_NEW_EE_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_ACTION_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_DISCOVER_REQ_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_NO_MEM_ERR_EVT);
      SHOW_DETAIL_ONLY(NFA_EE_NO_CB_ERR_EVT);

    default:
      LOG(INFO) << StringPrintf("%s: event = %02hhx", __FUNCTION__, event);
  }
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

static void st_mt_glue_hci_cb(tNFA_HCI_EVT event,
                              tNFA_HCI_EVT_DATA *eventData) {
  LOG(INFO) << StringPrintf("%s: event=0x%X", __FUNCTION__, event);

  switch (event) {
    case NFA_HCI_REGISTER_EVT: {
      LOG(INFO) << StringPrintf(
          "%s: NFA_HCI_REGISTER_EVT; status=0x%X; handle=0x%X; pipes:%02hhx, "
          "gates:%02hhx",
          __FUNCTION__, eventData->hci_register.status,
          eventData->hci_register.hci_handle, eventData->hci_register.num_pipes,
          eventData->hci_register.num_gates);

      sNfaHciRegisterEvent.start();
      sHciHandle = eventData->hci_register.hci_handle;
      sNfaHciRegisterEvent.notifyOne();
      sNfaHciRegisterEvent.end();
    } break;

    case NFA_HCI_ALLOCATE_GATE_EVT: {
      LOG(INFO) << StringPrintf(
          "%s: NFA_HCI_ALLOCATE_GATE_EVT; status = %d, gate = 0x%x",
          __FUNCTION__, eventData->allocated.status, eventData->allocated.gate);
      sNfaHciAllocGateEvent.start();
      sNfaHciAllocGateEvent.notifyOne();
      sNfaHciAllocGateEvent.end();
    } break;

    case NFA_HCI_DEALLOCATE_GATE_EVT: {
      tNFA_HCI_DEALLOCATE_GATE &deallocated = eventData->deallocated;
      LOG(INFO) << StringPrintf(
          "%s: NFA_HCI_DEALLOCATE_GATE_EVT; status=0x%X; gate=0x%X",
          __FUNCTION__, deallocated.status, deallocated.gate);
    } break;

    case NFA_HCI_CREATE_PIPE_EVT: {
      LOG(INFO) << StringPrintf(
          "%s: NFA_HCI_CREATE_PIPE_EVT; status=0x%X; pipe=0x%X; src gate=0x%X; "
          "dest host=0x%X; dest gate=0x%X",
          __FUNCTION__, eventData->created.status, eventData->created.pipe,
          eventData->created.source_gate, eventData->created.dest_host,
          eventData->created.dest_gate);

      sNfaHciCreatePipeEvent.start();
      if (eventData->created.source_gate == ID_MGMT_GATE_ID) {
        if (eventData->created.status == NFA_STATUS_OK) {
          sIdMgmtInfo.created = true;
          sIdMgmtInfo.pipe_id = eventData->created.pipe;
        }
      } else if (eventData->created.source_gate == LOOPBACK_GATE_ID) {
        if (eventData->created.status == NFA_STATUS_OK) {
          sLoopbackInfo.created = true;
          sLoopbackInfo.pipe_id = eventData->created.pipe;
        }
      } else {
        if (eventData->created.status == NFA_STATUS_OK) {
          sCreatedPipeId = eventData->created.pipe;
        }
      }

      sNfaHciCreatePipeEvent.notifyOne();
      sNfaHciCreatePipeEvent.end();
    } break;

    case NFA_HCI_OPEN_PIPE_EVT: {  // this does not get called :/
      LOG(INFO) << StringPrintf(
          "%s: NFA_HCI_OPEN_PIPE_EVT; status=0x%X; pipe=0x%X", __FUNCTION__,
          eventData->opened.status, eventData->opened.pipe);

      sNfaHciOpenPipeEvent.start();

      if (eventData->opened.pipe == sIdMgmtInfo.pipe_id) {
        if (eventData->opened.status == NFA_HCI_ANY_OK) {
          sIdMgmtInfo.opened = true;
        }
      }
      if (eventData->opened.pipe == sLoopbackInfo.pipe_id) {
        if (eventData->opened.status == NFA_HCI_ANY_OK) {
          sLoopbackInfo.opened = true;
        }
      }
      sNfaHciOpenPipeEvent.notifyOne();
      sNfaHciOpenPipeEvent.end();
    } break;

    case NFA_HCI_RSP_RCVD_EVT: {  // response received from secure element
      tNFA_HCI_RSP_RCVD &rsp_rcvd = eventData->rsp_rcvd;
      LOG(INFO) << StringPrintf(
          "%s: NFA_HCI_RSP_RCVD_EVT; status: 0x%X; code: 0x%X; pipe: 0x%X; "
          "len: %u",
          __FUNCTION__, rsp_rcvd.status, rsp_rcvd.rsp_code, rsp_rcvd.pipe,
          rsp_rcvd.rsp_len);

      if (((rsp_rcvd.pipe == sIdMgmtInfo.pipe_id) &&
           (sIdMgmtInfo.open_pending)) ||
          ((rsp_rcvd.pipe == sLoopbackInfo.pipe_id) &&
           (sLoopbackInfo.open_pending))) {
        sNfaHciOpenPipeEvent.start();
        if (rsp_rcvd.pipe == sIdMgmtInfo.pipe_id) {
          sIdMgmtInfo.opened = true;
        }
        if (rsp_rcvd.pipe == sLoopbackInfo.pipe_id) {
          sLoopbackInfo.opened = true;
        }
        sNfaHciOpenPipeEvent.notifyOne();
        sNfaHciOpenPipeEvent.end();
      } else if (((rsp_rcvd.pipe == sIdMgmtInfo.pipe_id) &&
                  (sIdMgmtInfo.close_pending)) ||
                 ((rsp_rcvd.pipe == sLoopbackInfo.pipe_id) &&
                  (sLoopbackInfo.close_pending))) {
        sNfaHciClosePipeEvent.start();
        if (rsp_rcvd.pipe == sIdMgmtInfo.pipe_id) {
          sIdMgmtInfo.opened = false;
        }
        if (rsp_rcvd.pipe == sLoopbackInfo.pipe_id) {
          sLoopbackInfo.opened = false;
        }
        sNfaHciClosePipeEvent.notifyOne();
        sNfaHciClosePipeEvent.end();
      } else {
        sHciRspRcvdEvent.start();
        if (sCreatedPipeId == rsp_rcvd.pipe) {
#if 0
                    if (rsp_rcvd.rsp_code == NFA_HCI_ANY_OK) {
                        if (sStExtensions.mIsWaitingEvent.propHciRsp == true) { //data reception
                            sStExtensions.mRxHciDataLen = rsp_rcvd.rsp_len;

                            for (i = 0; i < rsp_rcvd.rsp_len; i++) {
                                LOG(INFO) << StringPrintf( "%s: NFA_HCI_RSP_RCVD_EVT; sp_rcvd.rsp_data[%d] = 0x%x", fn, i, rsp_rcvd.rsp_data[i]);

                                sStExtensions.mRxHciData[i] = rsp_rcvd.rsp_data[i];
                            }
                        } else if(sStExtensions.mIsWaitingEvent.IsTestPipeOpened == true) {
                            LOG(INFO) << StringPrintf( "%s: NFA_HCI_RSP_RCVD_EVT; pipe 0x%x is now opened!!", fn, rsp_rcvd.pipe);
                        }
                    }
#endif
        }

        sHciRspRcvdEvent.notifyOne();
        sHciRspRcvdEvent.end();
      }
    } break;

    case NFA_HCI_GET_REG_RSP_EVT: {
      int i;

      LOG(INFO) << StringPrintf(
          "%s: NFA_HCI_GET_REG_RSP_EVT; status: 0x%X; pipe: 0x%X, reg_idx: "
          "0x%X, len: %d",
          __FUNCTION__, eventData->registry.status, eventData->registry.pipe,
          eventData->registry.index, eventData->registry.data_len);

      sNfaHciGetRegRspEvent.start();
      if (eventData->registry.pipe == sIdMgmtInfo.pipe_id) {
        if (eventData->registry.status == NFA_STATUS_OK) {
          if (eventData->registry.index == VERSION_SW_REG_IDX) {
            for (i = 0; i < eventData->registry.data_len; i++) {
              sIdMgmtInfo.version_sw[i] = eventData->registry.reg_data[i];
            }
          } else if (eventData->registry.index == VERSION_HW_REG_IDX) {
            for (i = 0; i < eventData->registry.data_len; i++) {
              sIdMgmtInfo.version_hw[i] = eventData->registry.reg_data[i];
            }
          }
        }
      }

      sNfaHciGetRegRspEvent.notifyOne();
      sNfaHciGetRegRspEvent.end();
    } break;

    case NFA_HCI_EVENT_RCVD_EVT: {
      LOG(INFO) << StringPrintf(
          "%s: NFA_HCI_EVENT_RCVD_EVT; code: 0x%X; pipe: 0x%X; data len: %u",
          __FUNCTION__, eventData->rcvd_evt.evt_code, eventData->rcvd_evt.pipe,
          eventData->rcvd_evt.evt_len);

      if (eventData->rcvd_evt.evt_code == NFA_HCI_EVT_POST_DATA) {
        LOG(INFO) << StringPrintf(
            "%s: NFA_HCI_EVENT_RCVD_EVT; NFA_HCI_EVT_POST_DATA", __FUNCTION__);

        sNfaHciEventRcvdEvent.start();
        sRspSize = eventData->rcvd_evt.evt_len;
        sNfaHciEventRcvdEvent.notifyOne();
        sNfaHciEventRcvdEvent.end();
      } else if (eventData->rcvd_evt.evt_code == NFA_HCI_EVT_TRANSACTION) {
        LOG(INFO) << StringPrintf(
            "%s: NFA_HCI_EVENT_RCVD_EVT; NFA_HCI_EVT_TRANSACTION",
            __FUNCTION__);
        // If we got an AID, notify any listeners
      } else if (eventData->rcvd_evt.evt_code == NFA_HCI_EVT_HOT_PLUG) {
        LOG(INFO) << StringPrintf(
            "%s: NFA_HCI_EVENT_RCVD_EVT; NFA_HCI_EVT_HOT_PLUG", __FUNCTION__);
        sIsInhibited = false;
        sHotPlugEvent.start();
        // HCI_REL13
        if (eventData->rcvd_evt.evt_len == 2) {
          if (eventData->rcvd_evt.p_evt_buf[1] == 0x02) {
            sIsInhibited = true;
          }
        } else {
          sIsInhibited = eventData->rcvd_evt.p_evt_buf[0] & 0x80 ? true : false;
        }
        sHotPlugEvent.notifyOne();
        sHotPlugEvent.end();
      }
    } break;

    case NFA_HCI_HOST_LIST_EVT: {  // HOST_LIST answer
      tNFA_HCI_HOST_LIST &hosts = eventData->hosts;
      LOG(INFO) << StringPrintf(
          "%s: NFA_HCI_HOST_LIST_EVT; status=0x%X; Number of hosts=0x%X",
          __FUNCTION__, hosts.status, hosts.num_hosts);

      if (hosts.status == NFA_STATUS_OK) {
        int host_bitmap = 0, nb_hosts = 0;

        for (int i = 0; i < hosts.num_hosts; i++) {
          LOG(INFO) << StringPrintf(
              "%s: NFA_HCI_HOST_LIST_EVT; host 0x%x in HOST_LIST", __FUNCTION__,
              hosts.host[i]);
          if (hosts.host[i] == UICC_ID) {
            host_bitmap |= (0x1);
            nb_hosts++;
          } else if (hosts.host[i] == ESE_ID) {
            host_bitmap |= (0x2);
            nb_hosts++;
          }
        }
      }

      sNfaHciHostListEvent.start();
      sNfaHciHostListEvent.notifyOne();
      sNfaHciHostListEvent.end();
    } break;

    case NFA_HCI_DELETE_PIPE_EVT: {
      tNFA_HCI_DELETE_PIPE &deleted = eventData->deleted;
      LOG(INFO) << StringPrintf(
          "%s: NFA_HCI_DELETE_PIPE_EVT; status=0x%X; Identity of deleted pipe "
          "= 0x%x",
          __FUNCTION__, deleted.status, deleted.pipe);
      sNfaHciDeletePipeEvent.start();
      sNfaHciDeletePipeEvent.notifyOne();
      sNfaHciDeletePipeEvent.end();
    } break;

    case NFA_HCI_CMD_SENT_EVT: {
      tNFA_HCI_CMD_SENT &cmd_sent = eventData->cmd_sent;
      LOG(INFO) << StringPrintf("%s: NFA_HCI_CMD_SENT_EVT; status=0x%X;",
                                __FUNCTION__, cmd_sent.status);

      if (cmd_sent.status == NFA_STATUS_FAILED) {
        LOG(INFO) << StringPrintf(
            "%s: NFA_HCI_CMD_SENT_EVT; Status Failed!!! - Aborting all waits "
            "-- TODO",
            __FUNCTION__);
        // Abort all waits
#if 0
                sStExtensions.abortWaits();
#endif
      }
    } break;

    case NFA_HCI_GET_GATE_PIPE_LIST_EVT: {
      uint8_t i;
      LOG(INFO) << StringPrintf(
          "%s: NFA_HCI_GET_GATE_PIPE_LIST_EVT; status=0x%X, %02hhx pipes, "
          "%02hhx gates, %02hhx UICC gates",
          __FUNCTION__, eventData->gates_pipes.status,
          eventData->gates_pipes.num_pipes, eventData->gates_pipes.num_gates,
          eventData->gates_pipes.num_uicc_created_pipes);
      sNfaHciListPipeEvent.start();
      for (i = 0; i < eventData->gates_pipes.num_gates; i++) {
        if (eventData->gates_pipes.gate[i] == ID_MGMT_GATE_ID) {
          LOG(INFO) << StringPrintf("%s: ID_MGMT_GATE_ID found", __FUNCTION__);
          sIdMgmtInfo.added = true;
        } else if (eventData->gates_pipes.gate[i] == LOOPBACK_GATE_ID) {
          LOG(INFO) << StringPrintf("%s: ID_MGMT_GATE_ID found", __FUNCTION__);
          sLoopbackInfo.added = true;
        }
      }

      for (i = 0; i < eventData->gates_pipes.num_pipes; i++) {
        LOG(INFO) << StringPrintf(
            "%s: pipe %02hhx (%02hhx) %02hhx:%02hhx:%02hhx", __FUNCTION__,
            eventData->gates_pipes.pipe[i].pipe_id,
            eventData->gates_pipes.pipe[i].pipe_state,
            eventData->gates_pipes.pipe[i].local_gate,
            eventData->gates_pipes.pipe[i].dest_host,
            eventData->gates_pipes.pipe[i].dest_gate);
        if (eventData->gates_pipes.pipe[i].dest_host != 0 /* DH */) {
          continue;
        }
        if (eventData->gates_pipes.pipe[i].local_gate == ID_MGMT_GATE_ID) {
          LOG(INFO) << StringPrintf("%s: pipe with ID_MGMT_GATE_ID found",
                                    __FUNCTION__);
          sIdMgmtInfo.created = true;
          if (eventData->gates_pipes.pipe[i].pipe_state ==
              NFA_HCI_PIPE_OPENED) {
            sIdMgmtInfo.opened = true;
          }
        } else if (eventData->gates_pipes.pipe[i].local_gate ==
                   LOOPBACK_GATE_ID) {
          LOG(INFO) << StringPrintf("%s: pipe with LOOPBACK_GATE_ID found",
                                    __FUNCTION__);
          sLoopbackInfo.created = true;
          if (eventData->gates_pipes.pipe[i].pipe_state ==
              NFA_HCI_PIPE_OPENED) {
            sLoopbackInfo.opened = true;
          }
        }
      }

      sNfaHciListPipeEvent.notifyOne();
      sNfaHciListPipeEvent.end();
    } break;

    case NFA_HCI_SET_REG_RSP_EVT: {
      LOG(INFO) << StringPrintf(
          "%s: NFA_HCI_SET_REG_RSP_EVT; status: 0x%X; pipe: 0x%X, reg_idx: "
          "0x%X, len: %d",
          __FUNCTION__, eventData->registry.status, eventData->registry.pipe,
          eventData->registry.index, eventData->registry.data_len);
    } break;

    case NFA_HCI_EVENT_SENT_EVT:
      LOG(INFO) << StringPrintf("%s: NFA_HCI_EVENT_SENT_EVT; status=0x%X",
                                __FUNCTION__, eventData->evt_sent.status);
      {
        if (eventData->evt_sent.status == NFA_STATUS_FAILED) {
          LOG(INFO) << StringPrintf(
              "%s: NFA_HCI_CMD_SENT_EVT; Status Failed!!! - Aborting all waits "
              "-- TODO",
              __FUNCTION__);
          // Abort all waits
#if 0
                    sStExtensions.abortWaits();
#endif
        }
      }
      break;

    case NFA_HCI_CLOSE_PIPE_EVT: {  // this does not get called :/
      LOG(INFO) << StringPrintf(
          "%s: NFA_HCI_CLOSE_PIPE_EVT; status = %d, pipe = 0x%x", __FUNCTION__,
          eventData->closed.status, eventData->closed.pipe);
      sNfaHciClosePipeEvent.start();
      sNfaHciClosePipeEvent.notifyOne();
      sNfaHciClosePipeEvent.end();
    } break;

    default:
      LOG(ERROR) << StringPrintf(
          "%s: event code=0x%X not handled by this method", __FUNCTION__,
          event);
      break;
  }
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
static void st_mt_glue_nfaVsCb(uint8_t event, uint16_t param_len,
                               uint8_t *p_param) {
  LOG(INFO) << StringPrintf("%s: event = %02hhx, len = %hu, status = %02hhx",
                            __FUNCTION__, event, param_len, *p_param);

  sNfaVSEvent.start();
  sNfaVSStatus = NFA_STATUS_FAILED;
  if (param_len >=
      4) {  // all our proprietary commands have a status following the header.
    sNfaVSStatus = p_param[3];  // B0 = resp; B1 = OID, B2 = len, B3 = status.
                                // B4... = data.
    memcpy(sNfaVSResp, p_param + 4,
           param_len - 4 < sizeof(sNfaVSResp) ? param_len - 4
                                              : sizeof(sNfaVSResp));
  }
  sNfaVSEvent.notifyOne();
  sNfaVSEvent.end();
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
static void hce_cb(uint8_t event, tNFA_CONN_EVT_DATA *eventData) {
  int asynchronous = 0;

  LOG(INFO) << StringPrintf("%s: event= %u", __FUNCTION__, event);

  switch (event) {
    case NFA_CE_REGISTERED_EVT: {
      LOG(INFO) << StringPrintf("%s: NFA_CE_REGISTERED_EVT; status = %d",
                                __FUNCTION__, eventData->ce_registered.status);
      sHceEvent.start();
      if (eventData->ce_registered.status == NFA_STATUS_OK) {
        if (sIsHceFEnabling) {
          sHceFHandle = eventData->ce_registered.handle;
        } else {
          sHceHandle = eventData->ce_registered.handle;
        }
      }
      sHceEvent.notifyOne();
      sHceEvent.end();
    } break;

    case NFA_CE_DEREGISTERED_EVT: {
      LOG(INFO) << StringPrintf("%s: NFA_CE_DEREGISTERED_EVT; status = %d",
                                __FUNCTION__,
                                eventData->ce_deregistered.handle);
    } break;

    case NFA_CE_ACTIVATED_EVT: {
      LOG(INFO) << StringPrintf("%s: NFA_CE_ACTIVATED_EVT; status = %d",
                                __FUNCTION__, eventData->ce_activated.status);
      asynchronous = 1;
    } break;

    case NFA_DEACTIVATED_EVT: {
      LOG(INFO) << StringPrintf("%s: NFA_DEACTIVATED_EVT;", __FUNCTION__);
    } break;

    case NFA_CE_DEACTIVATED_EVT: {
      LOG(INFO) << StringPrintf("%s: NFA_CE_DEACTIVATED_EVT;", __FUNCTION__);
    } break;

    case NFA_CE_DATA_EVT: {
      uint8_t resp[] = {0x6F, 0x00};
      LOG(INFO) << StringPrintf(
          "%s: NFA_CE_DATA_EVT; status = %d, len:%u. Responding 6F00.",
          __FUNCTION__, eventData->ce_data.status, eventData->ce_data.len);
      tNFA_STATUS status = NFA_SendRawFrame(resp, sizeof(resp), 0);
      if (status != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: NFA_SendRawFrame failed %02hhx",
                                   __FUNCTION__, status);
      }
    } break;

    default:
      LOG(ERROR) << StringPrintf(
          "%s: event code=0x%X not handled by this method", __FUNCTION__,
          event);
      break;
  }

  if (asynchronous) {
    int ret;
    ret = pthread_mutex_lock(&st_mt_g_state->mtx);
    if (ret != 0) {
      LOG(ERROR) << StringPrintf("%s: Failed to lock mutex ! %s", __FUNCTION__,
                                 strerror(ret));
    }

    memcpy(&st_mt_g_state->CONN_event_data, eventData, sizeof(*eventData));
    st_mt_g_state->CONN_event_code = event;
    st_mt_g_state->has_CONN_event = 1;

    ret = pthread_cond_broadcast(&st_mt_g_state->cond);
    if (ret != 0) {
      LOG(ERROR) << StringPrintf("%s: Failed to signal cond ! %s", __FUNCTION__,
                                 strerror(ret));
    }

    ret = pthread_mutex_unlock(&st_mt_g_state->mtx);
    if (ret != 0) {
      LOG(ERROR) << StringPrintf("%s: Failed to unlock mutex ! %s",
                                 __FUNCTION__, strerror(ret));
    }
  }
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
static void ndef_cb(tNFA_NDEF_EVT event, tNFA_NDEF_EVT_DATA *p_data) {
  LOG(INFO) << StringPrintf("%s: event= %u", __FUNCTION__, event);

  switch (event) {
    case NFA_NDEF_REGISTER_EVT: {
      if ((sNdefResult = p_data->ndef_reg.status) != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: REgistration of handler failed! (%x)",
                                   __FUNCTION__, p_data->ndef_reg.status);
      }
      sNdefCheck.start();
      sNdefCheck.notifyOne();
      sNdefCheck.end();
    } break;

    case NFA_NDEF_DATA_EVT: {
      LOG(INFO) << StringPrintf("%s: received %d bytes NDEF data, parsing",
                                __FUNCTION__, p_data->ndef_data.len);
      sNdefCheck.start();

      free(sNdefMessage.data);  // just in case.
      memset(&sNdefMessage, 0, sizeof(sNdefMessage));

      sNdefResult = NFA_STATUS_FAILED;

      // parse the buffer into sNdefMessage
      do {
        int cf = 0;
        int sr = 0;
        int il = 0;
        uint8_t *type = NULL;
        int type_len = 0;
        uint8_t *id = NULL;
        int id_len = 0;
        uint8_t *payload = NULL;
        int payload_len = 0;
        int offset = 0;

        if (p_data->ndef_data.len < 3) {
          LOG(ERROR) << StringPrintf("%s: Received NDEF data too short (%d)",
                                     __FUNCTION__, p_data->ndef_data.len);
          break;
        }

        sNdefMessage.type = ndef_msg::NDEF_OTHERS;
        sNdefMessage.record_flag = p_data->ndef_data.p_data[offset];

        cf = sNdefMessage.record_flag & NDEF_CF_MASK;
        sr = sNdefMessage.record_flag & NDEF_SR_MASK;
        il = sNdefMessage.record_flag & NDEF_IL_MASK;

        sNdefMessage.record_tnf =
            p_data->ndef_data.p_data[offset] & NDEF_TNF_MASK;

        if ((sNdefMessage.record_flag & NDEF_MB_MASK) == 0) {
          LOG(ERROR) << StringPrintf(
              "%s: Received NDEF data does not start with MESSAGE_BEGIN",
              __FUNCTION__);
          break;
        }
        if ((sNdefMessage.record_flag & NDEF_ME_MASK) == 0) {
          LOG(INFO) << StringPrintf(
              "%s: Received multi-record message, only handling 1st record",
              __FUNCTION__);
        }

        offset++;
        type_len = (int)p_data->ndef_data.p_data[offset];
        offset++;
        if (sr) {
          payload_len = (int)p_data->ndef_data.p_data[offset];
          offset++;
        } else {
          if (p_data->ndef_data.len < (uint32_t)offset + 4) {
            LOG(ERROR) << StringPrintf(
                "%s: Received NDEF data too short (!SR)(%d)", __FUNCTION__,
                p_data->ndef_data.len);
            break;
          }
          payload_len = (p_data->ndef_data.p_data[offset] << 24) +
                        (p_data->ndef_data.p_data[offset + 1] << 16) +
                        (p_data->ndef_data.p_data[offset + 2] << 8) +
                        (p_data->ndef_data.p_data[offset + 3]);
          offset += 4;
        }
        if (il) {
          id_len = (int)p_data->ndef_data.p_data[offset];
          offset++;
        }

        if (p_data->ndef_data.len <
            (uint32_t)(1 + 1 + (sr ? 1 : 4) + (il ? 1 : 0) + type_len + id_len +
                       payload_len)) {
          LOG(ERROR) << StringPrintf(
              "%s: Received NDEF data too short (%d/(%d+%d+%d))", __FUNCTION__,
              p_data->ndef_data.len, type_len, id_len, payload_len);
          break;
        }

        type = p_data->ndef_data.p_data + offset;
        offset += type_len;
        id = p_data->ndef_data.p_data + offset;
        offset += id_len;
        payload = p_data->ndef_data.p_data + offset;

        LOG(INFO) << StringPrintf("%s: flag:%02hhx, TL=%d, IL=%d, PL=%d",
                                  __FUNCTION__, sNdefMessage.record_flag,
                                  type_len, id_len, payload_len);

        memcpy(sNdefMessage.record_id, id,
               (size_t)id_len < sizeof(sNdefMessage.record_id)
                   ? id_len
                   : sizeof(sNdefMessage.record_id));

        if (sNdefMessage.record_tnf == NDEF_TNF_WKT) {
          // URI ?
          if ((type_len == 1) && (type[0] == 'U')) {
            sNdefMessage.type = ndef_msg::NDEF_URI;
            // advance the payload to be only the next byte (skip the type of
            // URI)
            payload_len--;
            payload++;

          } else
              // Text ?
              if ((type_len == 1) && (type[0] == 'T')) {
            uint8_t status = payload[0];
            sNdefMessage.type = ndef_msg::NDEF_TEXT;

            if ((status & 0x1F) == 2) {
              if ((payload[1] == 'd') && (payload[2] == 'e')) {
                sNdefMessage.lang = ndef_msg::NDEF_LN_DE;
              }
              if ((payload[1] == 'e') && (payload[2] == 'n')) {
                sNdefMessage.lang = ndef_msg::NDEF_LN_EN;
              }
              if ((payload[1] == 'f') && (payload[2] == 'r')) {
                sNdefMessage.lang = ndef_msg::NDEF_LN_FR;
              }
            }

            // advance the payload to be only the readable part
            payload_len -= (1 + (status & 0x1F));
            payload += (1 + (status & 0x1F));
          } else
              // Smartposter ?
              if ((type_len == 2) && (type[0] == 'S') && (type[0] == 'p')) {
            sNdefMessage.type = ndef_msg::NDEF_SP;
          }
        }

        sNdefMessage.data = (uint8_t *)calloc(payload_len + 1, 1);
        if (!sNdefMessage.data) {
          LOG(ERROR) << StringPrintf("%s: Unable to malloc for Payload length",
                                     __FUNCTION__);
          break;
        }
        sNdefMessage.len = payload_len;
        memcpy(sNdefMessage.data, payload, payload_len);

        // parsing was OK
        sNdefResult = NFA_STATUS_OK;
      } while (0);

      sNdefCheck.notifyOne();
      sNdefCheck.end();
    } break;

    default:
      LOG(ERROR) << StringPrintf("%s: unknown event %hhx ????", __FUNCTION__,
                                 event);
  }
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
static bool isListenMode(tNFA_ACTIVATED &activated) {
  return ((activated.activate_ntf.intf_param.type ==
           NFC_INTERFACE_EE_DIRECT_RF)  // a secure element is activated
          || (NFC_DISCOVERY_TYPE_LISTEN_A ==
              activated.activate_ntf.rf_tech_param.mode) ||
          (NFC_DISCOVERY_TYPE_LISTEN_B ==
           activated.activate_ntf.rf_tech_param.mode) ||
          (NFC_DISCOVERY_TYPE_LISTEN_F ==
           activated.activate_ntf.rf_tech_param.mode) ||
          (NFC_DISCOVERY_TYPE_LISTEN_ACTIVE ==
           activated.activate_ntf.rf_tech_param.mode) ||
          (NFC_DISCOVERY_TYPE_LISTEN_ISO15693 ==
           activated.activate_ntf.rf_tech_param.mode) ||
          (NFC_DISCOVERY_TYPE_LISTEN_B_PRIME ==
           activated.activate_ntf.rf_tech_param.mode));
}

static bool isPeerToPeer(tNFA_ACTIVATED &activated) {
  return activated.activate_ntf.protocol == NFA_PROTOCOL_NFC_DEP;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int st_mt_glue_startNFC() {
  tHAL_NFC_ENTRY *halFuncEntries;
  tNFA_STATUS stat = NFA_STATUS_OK;

  if (sIsNfaEnabled) {
    LOG(INFO) << StringPrintf("%s: already enabled", __FUNCTION__);
    return 0;
  }

  LOG(INFO) << StringPrintf("%s: initialize NFC stack", __FUNCTION__);
  NfcAdaptation &theInstance = NfcAdaptation::GetInstance();
  theInstance.Initialize();  // start GKI, NCI task, NFC task

  halFuncEntries = theInstance.GetHalEntryFuncs();
  NFA_Init(halFuncEntries);

  sNfaEnableEvent.start(true);
  stat = NFA_Enable(nfaDeviceManagementCallback, nfaConnectionCallback);
  if (stat == NFA_STATUS_OK) {
    // override log levels here?
    if (!sNfaEnableEvent.wait(15000)) {  // wait for NFA command to finish (and
                                         // notif to be received)
      stat = NFA_STATUS_FAILED;
    }
  }
  sNfaEnableEvent.end();

  if (stat == NFA_STATUS_OK && sIsNfaEnabled == false) {
    uint16_t ENABLE_TIMER = 5000;

    LOG(INFO) << StringPrintf("%s: default ENABLE_TIMER = %d ", __FUNCTION__,
                              ENABLE_TIMER);
    stimer.start();
    if (stimer.wait(ENABLE_TIMER) == false) {  // if timeout occurred
      LOG(INFO) << StringPrintf("%s: timeout waiting for RENABLE DM",
                                __FUNCTION__);
    }
    stimer.end();
    sNfaEnableEvent.start(true);
    stat = NFA_Enable(nfaDeviceManagementCallback, nfaConnectionCallback);
    if (stat == NFA_STATUS_OK)
      if (!sNfaEnableEvent.wait(15000)) {  // wait for NFA command to finish
                                           // (and notif to be received)
        stat = NFA_STATUS_FAILED;
      }
    sNfaEnableEvent.end();
  }

  if (stat != NFA_STATUS_OK || sIsNfaEnabled == false) {
    LOG(ERROR) << StringPrintf("%s: fail nfa enable; error=0x%X", __FUNCTION__,
                               stat);

    theInstance.Finalize();

    return 1;
  }

  // Okay if we are here, the basic initialization was successful. Continue.

  // Do custom NFCA startup configuration.
  {
    // we don t use FIELD information at the moment, so no need to bother.
    uint8_t nfa_field_info[] = {0x00};

    LOG(INFO) << StringPrintf("%s: Configure RF_FIELD_INFO event",
                              __FUNCTION__);
    sNfaSetConfigEvent.start(true);
    stat = NFA_SetConfig(NCI_PARAM_ID_RF_FIELD_INFO, sizeof(nfa_field_info),
                         &nfa_field_info[0]);
    if (stat == NFA_STATUS_OK) {
      sNfaSetConfigEvent.wait();
    }
    sNfaSetConfigEvent.end();
  }

  if (st_mt_glue_setRFMergeMode(1)) {
    LOG(ERROR) << StringPrintf("%s: Unable to set NFCC_CONFIG_CONTROL",
                               __FUNCTION__);
  }

  // Register a handler for all NDEF data
  {
    sNdefCheck.start(true);
    stat = NFA_RegisterNDefTypeHandler(TRUE, NFA_TNF_DEFAULT, NULL, 0, ndef_cb);
    if (stat == NFA_STATUS_OK) {
      sNdefCheck.wait();
    } else {
      LOG(ERROR) << StringPrintf("%s: Unable to register NDEF default cb: %d",
                                 __FUNCTION__, stat);
    }
    sNdefCheck.end();
  }

  // Register a handler for all EE data
  {
    sEeCheck.start(true);
    stat = NFA_EeRegister(st_mt_glue_ee_cb);
    if (stat == NFA_STATUS_OK) {
      sEeCheck.wait();
    } else {
      LOG(ERROR) << StringPrintf("%s: Unable to register EE default cb: %d",
                                 __FUNCTION__, stat);
    }
    sEeCheck.end();
  }

  // Reset information about the HCI gates
  memset(&sLoopbackInfo, 0, sizeof(sLoopbackInfo));
  memset(&sIdMgmtInfo, 0, sizeof(sIdMgmtInfo));

  // Register handler for HCI
  {
    sNfaHciRegisterEvent.start(true);
    stat = NFA_HciRegister((char *)"st_mtktools", st_mt_glue_hci_cb, TRUE);
    if (stat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail hci register; error=0x%X",
                                 __FUNCTION__, stat);
    } else {
      sNfaHciRegisterEvent.wait();
    }
    sNfaHciRegisterEvent.end();
  }

  // Check the current registration in HCI network
  {
    sNfaHciListPipeEvent.start(true);
    stat = NFA_HciGetGateAndPipeList(sHciHandle);
    if (stat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail hci list pipes; error=0x%X",
                                 __FUNCTION__, stat);
    } else {
      sNfaHciListPipeEvent.wait();
    }
    sNfaHciListPipeEvent.end();
  }

  return 0;
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
#define ST_OID 0x02
#define ST_VS_NFC_MODE 0x02

int st_mt_glue_stopNFC() {
  LOG(INFO) << StringPrintf("%s: enter", __FUNCTION__);

  if (!sIsNfaEnabled) {
    LOG(INFO) << StringPrintf("%s: already disabled", __FUNCTION__);
    return 0;
  }

  // first go back to IDLE if it wasn t already
  (void)st_mt_glue_resetmode();

  // // turn off the CLF
  // {
  //   uint8_t mActionRequestParam[] = {ST_VS_NFC_MODE, 0x00};
  //   tNFA_STATUS status;

  //   sNfaVSEvent.start(true);
  //   status = NFA_SendVsCommand(ST_OID, sizeof(mActionRequestParam),
  //                              mActionRequestParam, st_mt_glue_nfaVsCb);

  //   if (status != NFA_STATUS_OK) {
  //     LOG(ERROR) << StringPrintf(
  //         "%s: NFA_SendVsCommand() call failed; error=0x%X", __FUNCTION__,
  //         status);
  //   } else {
  //     sNfaVSEvent.wait(200);

  //     // wait for the RESET */
  //     usleep(50000);
  //   }
  //   sNfaVSEvent.end();
  // }

  // deinit the stack
  sHciHandle = NFA_HANDLE_INVALID;

  {
    sEeCheck.start(true);
    tNFA_STATUS stat = NFA_EeDeregister(st_mt_glue_ee_cb);
    if (stat == NFA_STATUS_OK) {
      sEeCheck.wait();
    } else {
      LOG(ERROR) << StringPrintf("%s: Unable to unregister EE default cb: %d",
                                 __FUNCTION__, stat);
    }
    sEeCheck.end();
  }

  if (sIsNfaEnabled) {
    sNfaDisableEvent.start(true);  // wait for NFA command to finish
    tNFA_STATUS stat = NFA_Disable(TRUE /* graceful */);
    if (stat == NFA_STATUS_OK) {
      LOG(INFO) << StringPrintf("%s: wait for completion", __FUNCTION__);
      sNfaDisableEvent.wait();  // wait for NFA command to finish
    } else {
      LOG(ERROR) << StringPrintf("%s: fail disable; error=0x%X", __FUNCTION__,
                                 stat);
    }
    sNfaDisableEvent.end();  // wait for NFA command to finish
  }

  NfcAdaptation &theInstance = NfcAdaptation::GetInstance();
  theInstance.Finalize();

  LOG(INFO) << StringPrintf("%s: exit", __FUNCTION__);
  return 0;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
int st_mt_glue_setRFMergeMode(int rfmerge) {
  LOG(INFO) << StringPrintf("%s: enter", __FUNCTION__);

  if (!sIsNfaEnabled) {
    LOG(INFO) << StringPrintf("%s: NFC disabled", __FUNCTION__);
    return 1;
  }

  {
    tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
    uint8_t nfcc_config_control[] = {0x00};

    if (rfmerge) {
      nfcc_config_control[0] = 0x01;
    }

    LOG(INFO) << StringPrintf(
        "%s: Configure NFCC_CONTROL_CONFIG parameter to %02hhx", __FUNCTION__,
        nfcc_config_control[0]);
    sNfaSetConfigEvent.start(true);
    nfaStat =
        NFA_SetConfig(NCI_PARAM_ID_NFCC_CONFIG_CONTROL,
                      sizeof(nfcc_config_control), &nfcc_config_control[0]);
    if (nfaStat == NFA_STATUS_OK) {
      sNfaSetConfigEvent.wait();
    }
    sNfaSetConfigEvent.end();
    if (nfaStat != NFA_STATUS_OK) {
      LOG(INFO) << StringPrintf("%s: failed to set NFCC_CONTROL_CONFIG value",
                                __FUNCTION__);
      return 1;
    }
  }

  current_rf_merge = rfmerge;

  LOG(INFO) << StringPrintf("%s: exit", __FUNCTION__);
  return 0;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

// This function is called from the main thread.
int st_mt_glue_handle_events(int prescheck) {
  if (!sIsNfaEnabled) {
    LOG(INFO) << StringPrintf("%s: NFC disabled", __FUNCTION__);
    return 1;
  }

  // handle any asynchronous CONN event
  if (st_mt_g_state->has_CONN_event) {
    tNFA_CONN_EVT_DATA *eventData = &st_mt_g_state->CONN_event_data;

    LOG(INFO) << StringPrintf("%s: Processing event %02hhx, current state %d",
                              __FUNCTION__, st_mt_g_state->CONN_event_code,
                              st_mt_g_state->state);

    switch (st_mt_g_state->CONN_event_code) {
      case NFA_DISC_RESULT_EVT: {
        tNFC_RESULT_DEVT *res =
            &st_mt_g_state->CONN_event_data.disc_result.discovery_ntf;

        if (st_mt_g_state->CONN_event_data.disc_result.status !=
            NFA_STATUS_OK) {
          if (st_mt_g_state->reader_cb) {
            (*st_mt_g_state->reader_cb)(RDR_NTF_FAIL, RDR_NTF_dataFAIL, 0, NULL,
                                        0, 0);
          }
          break;
        }

        if (!st_mt_g_state->disc_has_candidate) {
          // we need to find a suitable candidate; is this one OK ?
          if (res->protocol == NFA_PROTOCOL_NFC_DEP) {
            if (st_mt_g_state->p2p_cb != NULL) {
              st_mt_g_state->disc_has_candidate = 1;
              memcpy(&st_mt_g_state->disc_candidate, res,
                     sizeof(tNFC_RESULT_DEVT));
            }
          } else {
            if (st_mt_g_state->reader_cb != NULL) {
              st_mt_g_state->disc_has_candidate = 1;
              memcpy(&st_mt_g_state->disc_candidate, res,
                     sizeof(tNFC_RESULT_DEVT));
            }
          }
        }
        if (res->more > 1) {
          // more notifications to come, do nothing
          LOG(INFO) << StringPrintf(
              "%s : NFA_DISC_RESULT_EVT -> waiting for further discovery "
              "results",
              __FUNCTION__);
        } else {
          // last one
          LOG(INFO) << StringPrintf(
              "%s : NFA_DISC_RESULT_EVT -> last notif, and candidate = %d",
              __FUNCTION__, st_mt_g_state->disc_has_candidate);
          if (st_mt_g_state->disc_has_candidate) {
            st_mt_g_state->disc_has_candidate = 0;
            // Now select that candidate.
            tNFA_INTF_TYPE rf_type = NFC_INTERFACE_FRAME;
            if (st_mt_g_state->disc_candidate.protocol ==
                NFA_PROTOCOL_ISO_DEP) {
              rf_type = NFC_INTERFACE_ISO_DEP;
            }
            if (st_mt_g_state->disc_candidate.protocol ==
                NFA_PROTOCOL_NFC_DEP) {
              rf_type = NFC_INTERFACE_NFC_DEP;
            }

            tNFA_STATUS stat =
                NFA_Select(st_mt_g_state->disc_candidate.rf_disc_id,
                           st_mt_g_state->disc_candidate.protocol, rf_type);
            if (stat != NFA_STATUS_OK) {
              LOG(ERROR) << StringPrintf("%s: fail select; error=0x%X",
                                         __FUNCTION__, stat);
            }
          }
        }
      } break;

      case NFA_ACTIVATED_EVT: {
        // What type of activation is it ? listen/poll/p2p ?
        if (isPeerToPeer(eventData->activated)) {
          // p2p
          if (st_mt_g_state->p2p_cb == NULL) {
            LOG(ERROR) << StringPrintf(
                "%s: Detected a peer but p2p mode not started, deactivating",
                __FUNCTION__);
            NFA_Deactivate(FALSE);
            break;
          }

          sP2pActive = true;
          sLlcpDiscoType = eventData->activated.activate_ntf.rf_tech_param.mode;
          sLlcpBitrate = eventData->activated.activate_ntf.tx_bitrate;
          if (!sLlcpBitrate) {
            sLlcpBitrate = eventData->activated.activate_ntf.rx_bitrate;
          }
          if (NFC_DISCOVERY_TYPE_POLL_F ==
              eventData->activated.activate_ntf.rf_tech_param.mode) {
            sLlcpBitrate = eventData->activated.activate_ntf.rf_tech_param.param
                               .pf.bit_rate;
          }
          LOG(INFO) << StringPrintf(
              "%s: NFA_ACTIVATED_EVT; is p2p(%02hhx,%02hhx) -- wait for LLCP "
              "activated event",
              __FUNCTION__, sLlcpDiscoType, sLlcpBitrate);

        } else if (isListenMode(eventData->activated)) {
          // CE / HCE
          LOG(INFO) << StringPrintf("%s: Card emulation activated",
                                    __FUNCTION__);
          if (st_mt_g_state->ce_cb != NULL) {
            (*st_mt_g_state->ce_cb)();
          }

        } else {
          // poll mode.
          enum rdr_ntf_type t = RDR_NTF_CONNECT;
          enum rdr_ntf_data d = RDR_NTF_NonNDEF;
          int uidlen = 0;
          uint8_t uid[32];
          uint8_t mode = eventData->activated.activate_ntf.rf_tech_param.mode;
          uint8_t bitrate = 0;

          LOG(INFO) << StringPrintf(
              "%s: handling NFA_ACTIVATED_EVT. "
              "tx:%02hhx,rx:%02hhx,rf_tech:%02hhx",
              __FUNCTION__, eventData->activated.activate_ntf.tx_bitrate,
              eventData->activated.activate_ntf.rx_bitrate, mode);

          bitrate = eventData->activated.activate_ntf.tx_bitrate;
          if (!bitrate) {
            bitrate = eventData->activated.activate_ntf.rx_bitrate;
          }

          if (st_mt_g_state->reader_cb == NULL) {
            LOG(ERROR) << StringPrintf(
                "%s: Detected a tag but reader mode not started, deactivating",
                __FUNCTION__);
            NFA_Deactivate(FALSE);
            break;
          }

          // read UID.
          if (mode == NFC_DISCOVERY_TYPE_POLL_KOVIO) {
            uidlen = eventData->activated.activate_ntf.rf_tech_param.param.pk
                         .uid_len;
            memcpy(uid,
                   eventData->activated.activate_ntf.rf_tech_param.param.pk.uid,
                   (size_t)uidlen < sizeof(uid) ? uidlen : sizeof(uid));
          } else if (NFC_DISCOVERY_TYPE_POLL_A == mode) {
            uidlen = eventData->activated.activate_ntf.rf_tech_param.param.pa
                         .nfcid1_len;
            memcpy(
                uid,
                eventData->activated.activate_ntf.rf_tech_param.param.pa.nfcid1,
                (size_t)uidlen < sizeof(uid) ? uidlen : sizeof(uid));
          } else if (NFC_DISCOVERY_TYPE_POLL_B == mode) {
            uidlen = NFC_NFCID0_MAX_LEN;
            memcpy(
                uid,
                eventData->activated.activate_ntf.rf_tech_param.param.pb.nfcid0,
                (size_t)uidlen < sizeof(uid) ? uidlen : sizeof(uid));
          } else if (NFC_DISCOVERY_TYPE_POLL_F == mode) {
            uidlen = NFC_NFCID2_LEN;
            memcpy(
                uid,
                eventData->activated.activate_ntf.rf_tech_param.param.pf.nfcid2,
                (size_t)uidlen < sizeof(uid) ? uidlen : sizeof(uid));
            bitrate = eventData->activated.activate_ntf.rf_tech_param.param.pf
                          .bit_rate;

            // Special case: if this is NFC Forum Analog test, we do not start
            // presence check, we just reset.
            {
              uint8_t dta_id[NFC_NFCID2_LEN] = {0x02, 0xFE, 0x00, 0x01,
                                                0x02, 0x03, 0x04, 0x05};
              if (!memcmp(
                      dta_id, uid,
                      (size_t)uidlen < sizeof(uid) ? uidlen : sizeof(uid))) {
                LOG(INFO) << StringPrintf(
                    "%s:%d : detected DTA NFCID2, not starting NDEF and "
                    "presence check",
                    __FUNCTION__, __LINE__);
                NFA_Deactivate(FALSE);
                break;
              } else {
                LOG(INFO) << StringPrintf(
                    "%s:%d : standard NFCID2, starting NDEF and presence check",
                    __FUNCTION__, __LINE__);
              }
            }
          } else if (NFC_DISCOVERY_TYPE_POLL_V == mode) {
            int i;
            uidlen = I93_UID_BYTE_LEN;
            for (i = 0; i < I93_UID_BYTE_LEN && (size_t)i < sizeof(uid); i++) {
              uid[i] = eventData->activated.activate_ntf.rf_tech_param.param
                           .pi93.uid[I93_UID_BYTE_LEN - i - 1];
            }
          } else if (NFC_DISCOVERY_TYPE_POLL_ACTIVE == mode) {
            uidlen = eventData->activated.activate_ntf.rf_tech_param.param.acm_p
                         .atr_res_len;
            memcpy(uid,
                   eventData->activated.activate_ntf.rf_tech_param.param.acm_p
                       .atr_res,
                   (size_t)uidlen < sizeof(uid) ? uidlen : sizeof(uid));
          } else {
            LOG(ERROR) << StringPrintf("%s:%d : unhandled tech %02hhx",
                                       __FUNCTION__, __LINE__, mode);
          }

          // test if this is NDEF
          if (eventData->activated.activate_ntf.protocol ==
              NFC_PROTOCOL_MIFARE) {
            LOG(INFO) << StringPrintf("%s: Skipping NDEF detection for MiFARE",
                                      __FUNCTION__);
          } else {
            if (st_mt_g_state->state != ST_FM_READDEP_W84TAG) {
              sNdefCheck.start(true);

              memset(&sNdefStatus, 0, sizeof(sNdefStatus));
              sNdefStatus.status = NFA_RwDetectNDef();
              if (sNdefStatus.status == NFA_STATUS_OK) {
                LOG(INFO) << StringPrintf("%s: wait for NDEF check result",
                                          __FUNCTION__);
                sNdefCheck
                    .wait();  // sNdefStatus gets overwritten with the result
                LOG(INFO) << StringPrintf("%s: received NDEF check result",
                                          __FUNCTION__);
              }

              // if (sNdefStatus.status == NFA_STATUS_OK) {
              //     d = (sNdefStatus.flags & NFA_RW_NDEF_FL_READ_ONLY) ?
              //     RDR_NTF_READ_ONLY : RDR_NTF_NDEF;
              // }
              sNdefCheck.end();
            }
          }

          // call the callback.
          LOG(INFO) << StringPrintf("%s: reader_cb(%d, %d, %02hhx, %02hhx)",
                                    __FUNCTION__, t, d, mode, bitrate);
          (*st_mt_g_state->reader_cb)(t, d, uidlen, uid, mode, bitrate);
        }

      } break;

      case NFA_LLCP_ACTIVATED_EVT: {  // LLCP link is activated
        LOG(INFO) << StringPrintf(
            "%s: NFA_LLCP_ACTIVATED_EVT: is_initiator: %d  remote_wks: %d, "
            "remote_lsc: %d, remote_link_miu: %d, local_link_miu: %d",
            __FUNCTION__, eventData->llcp_activated.is_initiator,
            eventData->llcp_activated.remote_wks,
            eventData->llcp_activated.remote_lsc,
            eventData->llcp_activated.remote_link_miu,
            eventData->llcp_activated.local_link_miu);

        if (st_mt_g_state->p2p_cb == NULL) {
          LOG(ERROR) << StringPrintf(
              "%s: Detected a peer but p2p mode not started, deactivating",
              __FUNCTION__);
          NFA_Deactivate(FALSE);
          break;
        }

        (*st_mt_g_state->p2p_cb)(
            P2P_NTF_LLCP_UP, sLlcpDiscoType, sLlcpBitrate,
            eventData->llcp_activated.is_initiator ? 1 : 0);

      } break;

      case NFA_LLCP_DEACTIVATED_EVT:  // LLCP link is deactivated
        LOG(INFO) << StringPrintf("%s: NFA_LLCP_DEACTIVATED_EVT", __FUNCTION__);
        if (st_mt_g_state->p2p_cb == NULL) {
          LOG(ERROR) << StringPrintf(
              "%s: Detected a peer but p2p mode not started, deactivating",
              __FUNCTION__);
          NFA_Deactivate(FALSE);
          break;
        }

        (*st_mt_g_state->p2p_cb)(P2P_NTF_LLCP_DOWN, 0, 0, 0);

        break;

      case NFA_CE_ACTIVATED_EVT: {
        if (st_mt_g_state->ce_sel != 0) {
          LOG(INFO) << StringPrintf(
              "%s: NFA_CE_ACTIVATED_EVT -- cb already called in principle",
              __FUNCTION__);
        } else {
          LOG(INFO) << StringPrintf("%s: Host Card emulation activated",
                                    __FUNCTION__);
          if (st_mt_g_state->ce_cb != NULL) {
            (*st_mt_g_state->ce_cb)();
          }
        }
      } break;

      case NFA_DEACTIVATED_EVT:
        // We are back to RfDiscover state, just reflect the state accordingly,
        // and notify CLI if needed.
        switch (st_mt_g_state->state) {
          case ST_IDLE:
          case ST_ALS_READER:
          case ST_ALS_P2P:
          case ST_ALS_CE:
          case ST_ALS_HCE:
          case ST_POLLING:
          case ST_TXON:
          case ST_FM_CM_CE_W84READER:
          case ST_FM_CM_HCE_W84READER:
            // just nothing to do
            break;

          case ST_ALS_READER_TAG_CONNECTED:
          case ST_POLLING_TAG_CONNECTED: {
            if (st_mt_g_state->reader_cb) {
              (*st_mt_g_state->reader_cb)(
                  RDR_NTF_DISCONNECT, RDR_NTF_dataDISCONNECT, 0, NULL, 0, 0);
            }
          } break;

          case ST_ALS_P2P_LLCP_UP:
          case ST_POLLING_LLCP_UP: {
            if (st_mt_g_state->p2p_cb) {
              (*st_mt_g_state->p2p_cb)(P2P_NTF_LLCP_DOWN, 0, 0, 0);
            }
          } break;

          default:
            LOG(ERROR) << StringPrintf(
                "%s: NFA_DEACTIVATED_EVT > event not implemented for the "
                "current state",
                __FUNCTION__);
        }
        break;

      default:
        LOG(ERROR) << StringPrintf("%s: async event not implemented yet",
                                   __FUNCTION__);
    }

    st_mt_g_state->has_CONN_event = 0;
  }

  // If we are in a state with a tag present, to a synchronous presence check
  if ((prescheck) && ((st_mt_g_state->state == ST_ALS_READER_TAG_CONNECTED) ||
                      (st_mt_g_state->state == ST_POLLING_TAG_CONNECTED))) {
    // Do a presence check
    sNfaPresenceCheck.start(true);
    if (NFA_STATUS_OK != NFA_RwPresenceCheck(NFA_RW_PRES_CHK_DEFAULT)) {
      LOG(ERROR) << StringPrintf("%s: Failed to do a presence check",
                                 __FUNCTION__);
      NFA_Deactivate(FALSE);
    } else {
      // wait for result
      LOG(INFO) << StringPrintf("%s: wait for presence check result",
                                __FUNCTION__);
      if (!sNfaPresenceCheck.wait(1000)) {
        sPresenceConfirmed = false;
      }

      if (!sPresenceConfirmed) {
        // The tag is not here anymore, send a deactivate request. The state
        // change and notifs will be done once deactivated.
        NFA_Deactivate(FALSE);
      }
    }
    sNfaPresenceCheck.end();
  }

  return 0;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int st_mt_glue_getversions(uint8_t *mwver /* 19 bytes */,
                           uint8_t *fw_ver /* 2 bytes */,
                           uint8_t *hw_ver /* 2 bytes */,
                           uint8_t *chip_ver /* 2 bytes */) {
  if (!sIsNfaEnabled) {
    LOG(INFO) << StringPrintf("%s: NFC disabled", __FUNCTION__);
    return 1;
  }

  // We map the information as we can...
  strncpy((char *)mwver, ST_MTKTOOLS_VERSION, 18);
  fw_ver[0] = sFwVersion[0];
  fw_ver[1] = sFwVersion[1];
  //  hw_ver[0] = sFwVersion[2];
  //  hw_ver[1] = sFwVersion[3];
  //  chip_ver[0] = sHwVersion[0];
  //  chip_ver[1] = sHwVersion[1];
  chip_ver[0] = sFwVersion[2];
  chip_ver[1] = sFwVersion[3];
  hw_ver[0] = sHwVersion[0];
  hw_ver[1] = sHwVersion[1];

  return 0;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int st_mt_glue_resetmode() {
  tNFA_STATUS stat;
  if (!sIsNfaEnabled) {
    LOG(INFO) << StringPrintf("%s: NFC disabled", __FUNCTION__);
    return 1;
  }

  // Stop RF Discovery if it was started
  switch (st_mt_g_state->state) {
    case ST_ALS_READER_TAG_CONNECTED:
    case ST_ALS_P2P_LLCP_UP:
    case ST_POLLING_TAG_CONNECTED:
    case ST_POLLING_LLCP_UP:
    case ST_FM_CM_CE_W84READER:
    case ST_FM_CM_HCE_W84READER:
      // try deactivate
      // (void)NFA_Deactivate(FALSE);

    case ST_ALS_READER:
    case ST_ALS_P2P:
    case ST_ALS_CE:
    case ST_ALS_HCE:
    case ST_POLLING:
    case ST_TM_OPTIONCHECK_W84TAG:
    case ST_FM_READDEP_W84TAG: {
      LOG(INFO) << StringPrintf("Stopping RF discovery (checK:%s)",
                                sRfEnabled ? "true" : "false");
      sNfaEnableDisablePollingEvent.start(true);
      stat = NFA_StopRfDiscovery();
      if (stat == NFA_STATUS_OK) {
        LOG(INFO) << StringPrintf("%s: wait for discovery stopped event",
                                  __FUNCTION__);
        sRfEnabled = false;
        sNfaEnableDisablePollingEvent.wait(300);
      } else {
        LOG(ERROR) << StringPrintf("%s: fail to stop RF Discovery; error=0x%X",
                                   __FUNCTION__, stat);
        sNfaEnableDisablePollingEvent.end();
        return -1;
      }
      sNfaEnableDisablePollingEvent.end();
    } break;

    default:
        // ignore
        ;
  }

  (void)NFA_DisablePolling();
  (void)NFA_DisableListening();

  if (sP2pListeningMask) {
    (void)NFA_P2pDeregister(sP2psHandle);
  }

  UNSET_NCIPARAM(NCI_PARAM_ID_LA_SEL_INFO);
  UNSET_NCIPARAM(NCI_PARAM_ID_LB_SENSB_INFO);
  UNSET_NCIPARAM(NCI_PARAM_ID_LF_PROTOCOL);

  if (sEeForcedRouting) {
    tNFA_STATUS stat = NFA_STATUS_OK;

    sEeCheck.start();
    if ((stat = NFA_EeStopForceRouting()) != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: NFA_EeStopForceRouting; error=0x%X",
                                 __FUNCTION__, stat);
      sEeCheck.end();
      return 1;
    }
    LOG(INFO) << StringPrintf("%s: wait for NFA_EE_FORCE_ROUTING_EVT",
                              __FUNCTION__);
    sEeCheck.wait();
    sEeCheck.end();
    if (sEeStatus.status != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: NFA_EeStopForceRouting cb; error=0x%X",
                                 __FUNCTION__, sEeStatus.status);
    }
    sEeForcedRouting = false;
  }

  if (sActiveEe != 0) {
    tNFA_STATUS stat = NFA_STATUS_OK;

    sCeUiccListenCheck.start(true);
    if ((stat = NFA_CeConfigureUiccListenTech(sActiveEe, 0)) != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "%s: NFA_CeConfigureUiccListenTech; error=0x%X", __FUNCTION__, stat);
    } else {
      // NFA_CE_UICC_LISTEN_CONFIGURED_EVT
      sCeUiccListenCheck.wait();
      if (sCeResult != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf(
            "%s: NFA_CeConfigureUiccListenTech; error=0x%X", __FUNCTION__,
            sCeResult);
      }
    }
    sCeUiccListenCheck.end();

    deactivate_activeSE();
  }

  if (sIsHceEnabled) {
    tNFA_STATUS stat;

    // disable the HCE
    if ((stat = NFA_CeDeregisterAidOnDH(sHceHandle)) != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: NFA_CeDeregisterAidOnDH; error=0x%X",
                                 __FUNCTION__, stat);
    }
    sHceHandle = 0;
    sIsHceEnabled = false;
  }

  if (sIsHceFEnabled) {
    tNFA_STATUS stat;

    // disable the HCE
    if ((stat = NFA_CeDeregisterFelicaSystemCodeOnDH(sHceFHandle)) !=
        NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "%s: NFA_CeDeregisterFelicaSystemCodeOnDH; error=0x%X", __FUNCTION__,
          stat);
    }
    sHceFHandle = 0;
    sIsHceFEnabled = false;
  }

  // move to IDLE
  st_mt_event_chstate(ST_IDLE);

  // Now clean up the state
  memset(&st_mt_g_state->rdr_techs, 0, sizeof(st_mt_g_state->rdr_techs));
  memset(&st_mt_g_state->rdr_a_datarate, 0,
         sizeof(st_mt_g_state->rdr_a_datarate));
  memset(&st_mt_g_state->rdr_b_datarate, 0,
         sizeof(st_mt_g_state->rdr_b_datarate));
  memset(&st_mt_g_state->rdr_v_datarate, 0,
         sizeof(st_mt_g_state->rdr_v_datarate));
  memset(&st_mt_g_state->rdr_v_opts, 0, sizeof(st_mt_g_state->rdr_v_opts));
  memset(&st_mt_g_state->rdr_f_datarate, 0,
         sizeof(st_mt_g_state->rdr_f_datarate));
  memset(&st_mt_g_state->p2p_techs, 0, sizeof(st_mt_g_state->p2p_techs));
  memset(&st_mt_g_state->p2p_a_datarate, 0,
         sizeof(st_mt_g_state->p2p_a_datarate));
  memset(&st_mt_g_state->p2p_f_datarate, 0,
         sizeof(st_mt_g_state->p2p_f_datarate));
  memset(&st_mt_g_state->p2p_opts, 0, sizeof(st_mt_g_state->p2p_opts));
  memset(&st_mt_g_state->ce_sel, 0, sizeof(st_mt_g_state->ce_sel));
  memset(&st_mt_g_state->ce_techs, 0, sizeof(st_mt_g_state->ce_techs));

  st_mt_g_state->reader_cb = NULL;
  st_mt_g_state->p2p_cb = NULL;
  st_mt_g_state->ce_cb = NULL;

  st_mt_g_state->disc_has_candidate = 0;
  st_mt_g_state->has_CONN_event = 0;

  sPollingMask = 0;
  sListeningMask = 0;
  sP2pListeningMask = 0;
  sActiveEe = 0;

  // if the SE was in error state, we need to redo an EE_Discover.
  if (sActiveEeError) {
    sActiveEeError = false;

#ifndef SWP_FAIL_EE_DISCOVER
    st_mt_glue_stopNFC();
    st_mt_glue_startNFC();
#else   //    SWP_FAIL_EE_DISCOVER
    tNFA_STATUS nfaStat;
    sEeCheck.start(true);
    if ((nfaStat = NFA_EeDiscover(st_mt_glue_ee_cb)) != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail get info; error=0x%X", __FUNCTION__,
                                 nfaStat);
    } else {
      if (!sEeCheck.wait(200)) {
        LOG(ERROR) << StringPrintf("%s: fail get info notification",
                                   __FUNCTION__);
      }
    }
    sEeCheck.end();
#endif  //   SWP_FAIL_EE_DISCOVER
  }

  if (!current_rf_merge) {
    // go back to default merge mode
    st_mt_glue_setRFMergeMode(1);
  }

  return 0;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
static const char *eeStatusToString(uint8_t status) {
  switch (status) {
    case NFC_NFCEE_STATUS_ACTIVE:
      return ("Connected/Active");
    case NFC_NFCEE_STATUS_INACTIVE:
      return ("Connected/Inactive");
    case NFC_NFCEE_STATUS_UNRESPONSIVE:
      return ("Unresponsive");
  }
  return ("?? Unknown ??");
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
static uint8_t get_SEID_for_se(int se) {
  tNFA_EE_INFO mEeInfo[MAX_NUM_EE];
  uint8_t mActualNumEe =
      MAX_NUM_EE;             // actual number of EE's reported by the stack
  uint8_t mNumEePresent = 0;  // actual number of usable EE's
  uint8_t se_id = 0x00;
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;

  if ((nfaStat = NFA_EeGetInfo(&mActualNumEe, mEeInfo)) != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: fail get info; error=0x%X", __func__,
                               nfaStat);
    return 0;
  } else {
    LOG(INFO) << StringPrintf("%s: num EEs discovered: %u", __FUNCTION__,
                              mActualNumEe);
    if (mActualNumEe != 0) {
      for (uint8_t xx = 0; xx < mActualNumEe; xx++) {
        if (mEeInfo[xx].num_interface == 0) /* NCI 2.0 - End */
          mNumEePresent++;

        LOG(INFO) << StringPrintf(
            "%s: NFCEE[%u] Handle: 0x%04x  Status: %s  Nb protocol: %u: "
            "(0x%02x)  Num TLVs: %u",
            __func__, xx, mEeInfo[xx].ee_handle,
            eeStatusToString(mEeInfo[xx].ee_status), mEeInfo[xx].num_interface,
            mEeInfo[xx].ee_interface[0], mEeInfo[xx].num_tlvs);

        switch (se) {
          case st_mt_state::CE_UICC1:
            if ((mEeInfo[xx].ee_handle & 0xFF) == 0x81) se_id = 0x81;
            break;
          case st_mt_state::CE_UICC2:
            if ((mEeInfo[xx].ee_handle & 0xFF) == 0x83) se_id = 0x83;
            if ((mEeInfo[xx].ee_handle & 0xFF) == 0x85) se_id = 0x85;
            break;
          case st_mt_state::CE_eSE:
          case 3:
            if ((mEeInfo[xx].ee_handle & 0xFF) == 0x82) se_id = 0x82;
            if ((mEeInfo[xx].ee_handle & 0xFF) == 0x86) se_id = 0x86;
            if ((mEeInfo[xx].ee_handle & 0xFF) == 0x84) se_id = 0x84;
            break;
        }
        for (int yy = 0; yy < (int)mEeInfo[xx].num_tlvs; yy++) {
          LOG(INFO) << StringPrintf(
              "%s: EE[%u] TLV[%u]  Tag: 0x%02x  Len: %u  Values[]: 0x%02x  "
              "0x%02x  0x%02x ...",
              __func__, xx, yy, mEeInfo[xx].ee_tlv[yy].tag,
              mEeInfo[xx].ee_tlv[yy].len, mEeInfo[xx].ee_tlv[yy].info[0],
              mEeInfo[xx].ee_tlv[yy].info[1], mEeInfo[xx].ee_tlv[yy].info[2]);
        }
      }
    }
  }
  LOG(INFO) << StringPrintf("%s: mActualNumEe=%d, mNumEePresent=%d, ret:%hhx",
                            __FUNCTION__, mActualNumEe, mNumEePresent, se_id);
  return se_id;
}

// return 0 if activated, !0 otherwise.
static int activate_SEID(uint8_t se_id) {
  tNFA_STATUS stat = NFA_STATUS_OK;
  int success = 0;

  sHotPlugEvent.start(true);
  sHotPlugEvent.end();
  sEeActiveStarted.start(true);
  sEeActiveStarted.end();
  sEeActiveCompleted.start(true);
  sEeActiveCompleted.end();
  sEeCheck.start(true);

  if ((stat = NFA_EeModeSet(NFA_HANDLE_GROUP_EE | se_id, true)) !=
      NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: NFA_EeModeSet; error=0x%X", __FUNCTION__,
                               stat);
    sEeCheck.end();
    return 1;
  }

  if (!sEeCheck.wait(2500)) {  // NFA_EE_MODE_SET_EVT
    LOG(INFO) << StringPrintf("%s: timeout waiting for NFCEE_MODE_SET_NTF",
                              __FUNCTION__);
    sEeCheck.end();
    return 1;
  }
  if (sEeStatus.mode_set.status != NFA_STATUS_OK) {
    LOG(INFO) << StringPrintf("%s: MODESET event status %x", __FUNCTION__,
                              sEeStatus.mode_set.status);
    sEeCheck.end();
    sActiveEeError = true;  // We were not able to talk with the SE.
    return 1;
  }
  sActiveEe = sEeStatus.mode_set.ee_handle;
  LOG(INFO) << StringPrintf("%s: MODESET %x, EE mode: %d, hdl=%hx",
                            __FUNCTION__, sEeStatus.mode_set.status,
                            sEeStatus.mode_set.ee_status,
                            sEeStatus.mode_set.ee_handle);
  if (sEeStatus.mode_set.ee_status == NFA_EE_STATUS_ACTIVE) {
    success = 1;
  } else {
    sActiveEeError = true;  // We were not able to talk with the SE.
  }

  sEeCheck.end();

  if (success) {
    int got_hp = 0;
    sHotPlugEvent.start();
    if (sHotPlugEvent.wait(200)) {
      // got HOT PLUG event
      got_hp = 1;
    } else {
      LOG(INFO) << StringPrintf("%s: No hot plug received", __FUNCTION__);
    }

    sHotPlugEvent.end();
    if (got_hp && !sIsInhibited) {
      LOG(INFO) << StringPrintf(
          "%s: SE is not inhibited, waiting to check it does not clear all "
          "pipes anyway",
          __FUNCTION__);
      sEeActiveStarted.start();
      if (sEeActiveStarted.wait(300) == true) {
        LOG(INFO) << StringPrintf(
            "%s: SE is initializing, deal with it like an initial "
            "activation",
            __FUNCTION__);
        sIsInhibited = true;
      }
      sEeActiveStarted.end();
    }

    if (got_hp && sIsInhibited) {
      // if this was inhibited == initial activation, then we need to wait for
      // the complete registration of the SIM.
      LOG(INFO) << StringPrintf(
          "%s: Was inhibited, wait up to 3sec for full initial activation "
          "sequence",
          __FUNCTION__);
      sEeActiveCompleted.start();
      sEeActiveCompleted.wait(3000);
      sEeActiveCompleted.end();
    }

    if ((stat = NFA_HciGetHostList(sHciHandle)) == NFA_STATUS_OK) {
      sNfaHciHostListEvent.start();
      sNfaHciHostListEvent.wait(300);
      sNfaHciHostListEvent.end();
    } else {
      LOG(ERROR) << StringPrintf("%s: NFA_HciGetHostList failed; error=0x%X",
                                 __FUNCTION__, stat);
    }

    {
      uint8_t attr = 0x08;
      switch (sActiveEe & 0xFF) {
        case 0x81:
          attr |= 2;
          break;  // UICC1
        case 0x82:
          attr |= 3;
          break;  // eSE
        case 0x83:
          attr |= 3;
          break;  // UICC2 (UICC_B)
        case 0x84:
          attr |= 5;
          break;  // DHSE
        case 0x85:
          attr |= 4;
          break;  // UICC2 (UICC_C)
        case 0x86:
          attr |= 3;
          break;  // eSE (eUICC)
        default:
          LOG(ERROR) << StringPrintf("%s: attr not supported yet: %x",
                                     __FUNCTION__, sActiveEe);
      }

      if (attr != 0x08) {
        uint8_t mActionRequestParam[] = {0x03, attr, 0x82, 0x1, 0x1};

        sNfaVSEvent.start(true);
        stat = NFA_SendVsCommand(ST_OID, 5, mActionRequestParam,
                                 st_mt_glue_nfaVsCb);

        if (stat != NFA_STATUS_OK) {
          LOG(ERROR) << StringPrintf(
              "%s: NFA_SendVsCommand() call failed; error=0x%X", __FUNCTION__,
              stat);
          sNfaVSEvent.end();
        } else {
          sNfaVSEvent.wait();

          if (sNfaVSStatus != NFA_STATUS_OK) {
            LOG(ERROR) << StringPrintf(
                "%s: NFA_SendVsCommand() answer with status%X", __FUNCTION__,
                sNfaVSStatus);
          } else {
            int i = 0;
            while (i < 15) {
              if (sNfaVSResp[4 + 12 * i + 5] & 0x01) {
                LOG(INFO) << StringPrintf(
                    "%s: pipe %hhx, src hid %hhx gate %xxh -> dst hid %hhx "
                    "gate %hhx, %s",
                    __FUNCTION__, sNfaVSResp[4 + 12 * i + 4],
                    sNfaVSResp[4 + 12 * i + 0], sNfaVSResp[4 + 12 * i + 1],
                    sNfaVSResp[4 + 12 * i + 2], sNfaVSResp[4 + 12 * i + 3],
                    sNfaVSResp[4 + 12 * i + 5] & 0x02 ? "opened"
                                                      : " not opened");
              }
              i++;
            }
          }
          sNfaVSEvent.end();
        }
      }
    }
  }

  return (success ? 0 : 1);
}

static void deactivate_activeSE() {
  tNFA_STATUS nfaStat = NFA_STATUS_OK;

  sEeCheck.start(true);

  if ((nfaStat = NFA_EeModeSet(sActiveEe, false)) != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: NFA_EeModeSet; error=0x%X", __FUNCTION__,
                               nfaStat);
  } else {
    if (!sEeCheck.wait(2500)) {  // NFA_EE_MODE_SET_EVT
      LOG(INFO) << StringPrintf("%s: MODESET timeout", __FUNCTION__);
      sActiveEeError = true;
    } else if (sEeStatus.mode_set.status == NFA_STATUS_OK) {
      sActiveEe = 0;
      LOG(INFO) << StringPrintf("%s: MODESET %x, EE mode: %d, hdl=%hx",
                                __FUNCTION__, sEeStatus.mode_set.status,
                                sEeStatus.mode_set.ee_status,
                                sEeStatus.mode_set.ee_handle);
    } else {
      sActiveEeError = true;
    }
  }

  sEeCheck.end();

  if (!sActiveEeError) {
    sHotPlugEvent.start();
    (void)sHotPlugEvent.wait(50);
    sHotPlugEvent.end();
  }
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int st_mt_glue_startmode(  // ret value: 0 = success, 1: fail.
    int polling_period,    // use 0 for default.
    void (*reader_cb)(enum rdr_ntf_type t, enum rdr_ntf_data d, int uidlen,
                      uint8_t *uid, uint8_t discotype,
                      uint8_t datarate),  // if ! NULL, starts reader mode and
                                          // calls this cb when notification of
                                          // tag (connect /disconnect /failure)
    void (*p2p_cb)(enum p2p_ntf_type t, uint8_t discotype, uint8_t datarate,
                   uint8_t role),  // if ! NULL, starts p2p mode and calls this
                                   // callback when llcp status changes
    int start_CE,    // if !0, start CE or HCE depending on ce_sel in config.
    void (*ce_cb)()  // if !NULL, will be called when a reader has read the
                     // emulated card (UID was read).
) {
  tNFA_STATUS stat = NFA_STATUS_OK;
  int pi_bitrate_set = 0;
  int pf_bitrate_set = 0;
  int acm_bitrate_set = 0;
  int li_bitrate_set = 0;

  if (!sIsNfaEnabled) {
    LOG(INFO) << StringPrintf("%s: NFC disabled", __FUNCTION__);
    return 1;
  }

  // make sure we start in clean slate
  sPollingMask = 0;
  sListeningMask = 0;
  sP2pListeningMask = 0;

  // Configure the polling period.
  if (polling_period) {
    if (NFA_STATUS_OK != NFA_SetRfDiscoveryDuration(polling_period)) {
      LOG(ERROR) << StringPrintf("Failed to set the polling period to %d",
                                 polling_period);
      return -1;
    }
  }

  // Reader mode requested ?
  if (reader_cb != NULL) {
    // Following parameters are set to be compliant with the polling loop for
    // NFC Forum Analog tests
    SET_NCIPARAM(NCI_PARAM_ID_PF_DEVICES_LIMIT, 0x01);
    SET_NCIPARAM(NCI_PARAM_ID_PA_BAILOUT, 0x01);
    SET_NCIPARAM(NCI_PARAM_ID_PB_BAILOUT, 0x01);
    SET_NCIPARAM(NCI_PARAM_ID_PF_BAILOUT, 0x01);

    // Polling is required.
    if (st_mt_g_state->rdr_techs.a) {
      sPollingMask |= NFA_TECHNOLOGY_MASK_A;
      // set the bitrate for A.
      if (st_mt_g_state->rdr_a_datarate.bps848) {
        SET_NCIPARAM(NCI_PARAM_ID_PI_BIT_RATE, NFC_BIT_RATE_848);
        pi_bitrate_set = 1;
      } else if (st_mt_g_state->rdr_a_datarate.bps424) {
        SET_NCIPARAM(NCI_PARAM_ID_PI_BIT_RATE, NFC_BIT_RATE_424);
        pi_bitrate_set = 1;
      } else if (st_mt_g_state->rdr_a_datarate.bps212) {
        SET_NCIPARAM(NCI_PARAM_ID_PI_BIT_RATE, NFC_BIT_RATE_212);
        pi_bitrate_set = 1;
      } else {
        SET_NCIPARAM(NCI_PARAM_ID_PI_BIT_RATE, NFC_BIT_RATE_106);
        pi_bitrate_set = 1;
      }
    }
    if (st_mt_g_state->rdr_techs.b) {
      sPollingMask |= NFA_TECHNOLOGY_MASK_B;
      if (pi_bitrate_set) {
        LOG(INFO) << StringPrintf(
            "%s: ignoring bitrate setting for ISO B because it was set for ISO "
            "A",
            __FUNCTION__);
      } else {
        if (st_mt_g_state->rdr_b_datarate.bps848) {
          SET_NCIPARAM(NCI_PARAM_ID_PI_BIT_RATE, NFC_BIT_RATE_848);
          pi_bitrate_set = 1;
        } else if (st_mt_g_state->rdr_b_datarate.bps424) {
          SET_NCIPARAM(NCI_PARAM_ID_PI_BIT_RATE, NFC_BIT_RATE_424);
          pi_bitrate_set = 1;
        } else if (st_mt_g_state->rdr_b_datarate.bps212) {
          SET_NCIPARAM(NCI_PARAM_ID_PI_BIT_RATE, NFC_BIT_RATE_212);
          pi_bitrate_set = 1;
        } else {
          SET_NCIPARAM(NCI_PARAM_ID_PI_BIT_RATE, NFC_BIT_RATE_106);
          pi_bitrate_set = 1;
        }
      }
    }
    if (st_mt_g_state->rdr_techs.f) {
      sPollingMask |= NFA_TECHNOLOGY_MASK_F;
      if (st_mt_g_state->rdr_f_datarate.bps424) {
        SET_NCIPARAM(NCI_PARAM_ID_PF_BIT_RATE, NFC_BIT_RATE_424);
        pf_bitrate_set = 1;
      } else {
        SET_NCIPARAM(NCI_PARAM_ID_PF_BIT_RATE, NFC_BIT_RATE_212);
        pf_bitrate_set = 1;
      }
    }
    if (st_mt_g_state->rdr_techs.v) {
      sPollingMask |= NFA_TECHNOLOGY_MASK_V;
      LOG(INFO) << StringPrintf(
          "%s: bitrate selection and options for type V is not supported",
          __FUNCTION__);
    }
    if (st_mt_g_state->rdr_techs.kovio) {
      sPollingMask |= NFA_TECHNOLOGY_MASK_KOVIO;
    }

    st_mt_g_state->reader_cb = reader_cb;
  }

  // P2P mode requested ?
  if (p2p_cb != NULL) {
    // TODO: disable CE flag
    if (st_mt_g_state->p2p_opts.disable_CE) {
      LOG(INFO) << StringPrintf("%s: Disable CE flag is not implemented.",
                                __FUNCTION__);
    }

    if (st_mt_g_state->p2p_techs.a) {
      if (pi_bitrate_set) {
        LOG(INFO) << StringPrintf(
            "%s: ignore P2P bitrate for poll A because it was set for reader "
            "mode",
            __FUNCTION__);
      } else {
        if (st_mt_g_state->p2p_a_datarate.bps848) {
          SET_NCIPARAM(NCI_PARAM_ID_PI_BIT_RATE, NFC_BIT_RATE_848);
          pi_bitrate_set = 1;
        } else if (st_mt_g_state->p2p_a_datarate.bps424) {
          SET_NCIPARAM(NCI_PARAM_ID_PI_BIT_RATE, NFC_BIT_RATE_424);
          pi_bitrate_set = 1;
        } else if (st_mt_g_state->p2p_a_datarate.bps212) {
          SET_NCIPARAM(NCI_PARAM_ID_PI_BIT_RATE, NFC_BIT_RATE_212);
          pi_bitrate_set = 1;
        } else {
          SET_NCIPARAM(NCI_PARAM_ID_PI_BIT_RATE, NFC_BIT_RATE_106);
          pi_bitrate_set = 1;
        }
      }

      if (st_mt_g_state->p2p_a_datarate.bps848) {
        SET_NCIPARAM(NCI_PARAM_ID_LI_BIT_RATE, NFC_BIT_RATE_848);
        li_bitrate_set = 1;
      } else if (st_mt_g_state->p2p_a_datarate.bps424) {
        SET_NCIPARAM(NCI_PARAM_ID_LI_BIT_RATE, NFC_BIT_RATE_424);
        li_bitrate_set = 1;
      } else if (st_mt_g_state->p2p_a_datarate.bps212) {
        SET_NCIPARAM(NCI_PARAM_ID_LI_BIT_RATE, NFC_BIT_RATE_212);
        li_bitrate_set = 1;
      } else {
        SET_NCIPARAM(NCI_PARAM_ID_LI_BIT_RATE, NFC_BIT_RATE_106);
        li_bitrate_set = 1;
      }

      if (st_mt_g_state->p2p_opts.passive) {
        if (st_mt_g_state->p2p_opts.initiator) {
          sPollingMask |= NFA_TECHNOLOGY_MASK_A;
        }
        if (st_mt_g_state->p2p_opts.target) {
          sP2pListeningMask |= NFA_TECHNOLOGY_MASK_A;
        }
      }
    }

    if (st_mt_g_state->p2p_techs.f) {
      if (pf_bitrate_set) {
        LOG(INFO) << StringPrintf(
            "%s: ignore P2P bitrate for poll F because it was set for reader "
            "mode",
            __FUNCTION__);
      } else {
        if (st_mt_g_state->p2p_f_datarate.bps424) {
          SET_NCIPARAM(NCI_PARAM_ID_PF_BIT_RATE, NFC_BIT_RATE_424);
          pf_bitrate_set = 1;
        } else {
          SET_NCIPARAM(NCI_PARAM_ID_PF_BIT_RATE, NFC_BIT_RATE_212);
          pf_bitrate_set = 1;
        }
      }

      LOG(INFO) << StringPrintf(
          "%s: No standard parameter to configure LISTEN F bitrate in NCI 2.0 "
          "draft 20",
          __FUNCTION__);

      if (st_mt_g_state->p2p_opts.passive) {
        if (st_mt_g_state->p2p_opts.initiator) {
          sPollingMask |= NFA_TECHNOLOGY_MASK_F;
        }
        if (st_mt_g_state->p2p_opts.target) {
          sP2pListeningMask |= NFA_TECHNOLOGY_MASK_F;
        }
      }
    }

    if (st_mt_g_state->p2p_opts.active) {
      if (st_mt_g_state->p2p_f_datarate.bps424) {
        SET_NCIPARAM(NCI_PARAM_ID_PACM_BIT_RATE, NFC_BIT_RATE_424);
        acm_bitrate_set = 1;
      } else {
        SET_NCIPARAM(NCI_PARAM_ID_PACM_BIT_RATE, NFC_BIT_RATE_212);
        acm_bitrate_set = 1;
      }

      if (st_mt_g_state->p2p_opts.initiator) {
        sPollingMask |= NFA_TECHNOLOGY_MASK_ACTIVE;
      }
      if (st_mt_g_state->p2p_opts.target) {
        sP2pListeningMask |= NFA_TECHNOLOGY_MASK_ACTIVE;
      }
    }

    st_mt_g_state->p2p_cb = p2p_cb;

    if (sP2pListeningMask) {
      // We need to start a P2P server in order for the stack to configure
      // listening techs.
      if ((stat = NFA_P2pRegisterServer(NFA_P2P_ANY_SAP, NFA_P2P_LLINK_TYPE,
                                        (char *)"st_mtktools",
                                        st_mt_glue_p2ps_cb)) != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: NFA_SnepStartDefaultServer; error=0x%X",
                                   __FUNCTION__, stat);
        return 1;
      }
    }
  }

  // CE mode requested ?
  if (start_CE) {
    if (st_mt_g_state->ce_techs.a) {
      sListeningMask |= NFA_TECHNOLOGY_MASK_A;
    }
    if (st_mt_g_state->ce_techs.b) {
      sListeningMask |= NFA_TECHNOLOGY_MASK_B;
    }
    if (st_mt_g_state->ce_techs.f) {
      sListeningMask |= NFA_TECHNOLOGY_MASK_F;
    }
    if (st_mt_g_state->ce_techs.f212) {
      // sListeningMask |= NFA_TECHNOLOGY_MASK_F;
      LOG(ERROR) << StringPrintf("%s: bitrate selection not implemeted (F 212)",
                                 __FUNCTION__);
    }
    if (st_mt_g_state->ce_techs.f424) {
      // sListeningMask |= NFA_TECHNOLOGY_MASK_F;
      LOG(ERROR) << StringPrintf("%s: bitrate selection not implemeted (F 424)",
                                 __FUNCTION__);
    }

#if 0
        /* we may need to merge in 2 situations:
          - P2P and CE to UICC are requested.
          - or an SE is active and we want HCE or P2P
          */
        if ((st_mt_g_state->ce_sel != st_mt_state::HCE) && (p2p_cb != NULL)) {
            st_mt_glue_setRFMergeMode(1);
        }
        if (((st_mt_g_state->ce_sel == st_mt_state::HCE) || (p2p_cb != NULL)) && (sActiveEe)) {
            st_mt_glue_setRFMergeMode(1);
        }
#endif
    if (st_mt_g_state->ce_sel == st_mt_state::HCE) {
      // workaround because otherwise starting with A only, then stop, then B
      // only will not work.
      if ((sListeningMask & (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B)) !=
          (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B)) {
        SET_NCIPARAM(NCI_PARAM_ID_LA_SEL_INFO, 0x20);
        SET_NCIPARAM(NCI_PARAM_ID_LB_SENSB_INFO, 0x01);
      }
      // ------------------

      // Set the bitrate to max speed
      if (li_bitrate_set) {
        LOG(INFO) << StringPrintf("%s: Listen bitrate already set, ignoring",
                                  __FUNCTION__);
      } else {
        SET_NCIPARAM(NCI_PARAM_ID_LI_BIT_RATE, NFC_BIT_RATE_848);
        //	SET_NCIPARAM(NCI_PARAM_ID_LB_BIT_RATE, NFC_BIT_RATE_848);
        li_bitrate_set = 1;
      }

      // Configure for HCE

      if (sListeningMask & (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B)) {
        // Select the tech to listen to
        if ((stat = NFA_CeSetIsoDepListenTech(
                 sListeningMask & (NFA_TECHNOLOGY_MASK_A |
                                   NFA_TECHNOLOGY_MASK_B))) != NFA_STATUS_OK) {
          LOG(ERROR) << StringPrintf(
              "%s: NFA_CeSetIsoDepListenTech; error=0x%X", __FUNCTION__, stat);
          return 1;
        }

        // Start HCE for ISO-DEP.
        sHceEvent.start(true);
        if ((stat = NFA_CeRegisterAidOnDH(NULL, 0, hce_cb)) != NFA_STATUS_OK) {
          LOG(ERROR) << StringPrintf("%s: NFA_CeRegisterAidOnDH; error=0x%X",
                                     __FUNCTION__, stat);
          sHceEvent.end();
          return 1;
        }
        // NFA_CE_REGISTERED_EVT
        {
          LOG(INFO) << StringPrintf("%s: wait for NFA_CE_REGISTERED_EVT",
                                    __FUNCTION__);
          sHceEvent.wait();
          LOG(INFO) << StringPrintf("%s: NFA_CE_REGISTERED_EVT received",
                                    __FUNCTION__);
          if (sHceHandle == 0) {
            LOG(ERROR) << StringPrintf("%s: NFA_CeRegisterAidOnDH failed",
                                       __FUNCTION__);
            sHceEvent.end();
            return 1;
          }
        }
        sHceEvent.end();
        sIsHceEnabled = true;
      }

      if (sListeningMask & (NFA_TECHNOLOGY_MASK_F)) {
        // Start HCE for T3T.
        int sc = 0x4004;
        uint8_t nfcid2[NCI_RF_F_UID_LEN] = {1, 2, 3, 4, 5, 6, 7, 8};
        ;
        uint8_t t3tPmm[NCI_T3T_PMM_LEN] = {0xFF, 0xFF, 0xFF, 0xFF,
                                           0xFF, 0xFF, 0xFF, 0xFF};

        sHceEvent.start(true);
        sIsHceFEnabling = true;
        if ((stat = NFA_CeRegisterFelicaSystemCodeOnDH(
                 sc, nfcid2, t3tPmm, hce_cb)) != NFA_STATUS_OK) {
          LOG(ERROR) << StringPrintf(
              "%s: NFA_CeRegisterFelicaSystemCodeOnDH; error=0x%X",
              __FUNCTION__, stat);
          sIsHceFEnabling = false;
          sHceEvent.end();
          return 1;
        }
        // NFA_CE_REGISTERED_EVT
        {
          LOG(INFO) << StringPrintf("%s: wait for NFA_CE_REGISTERED_EVT",
                                    __FUNCTION__);
          sHceEvent.wait();
          LOG(INFO) << StringPrintf("%s: NFA_CE_REGISTERED_EVT received",
                                    __FUNCTION__);
          sIsHceFEnabling = false;
          if (sHceFHandle == 0) {
            LOG(ERROR) << StringPrintf(
                "%s: NFA_CeRegisterFelicaSystemCodeOnDH failed", __FUNCTION__);
            sHceEvent.end();
            return 1;
          }
        }
        sHceEvent.end();

        // Add routing entry for type F otherwise CLF will stay mute.
        {
          SyncEventGuard guard(sEeCheck);

          // Set routing for technologies_switch_on
          stat = NFA_EeSetDefaultTechRouting(0x00, NFA_TECHNOLOGY_MASK_F, 0, 0,
                                             0, 0, 0);
          if (stat == NFA_STATUS_OK) {
            sEeCheck.wait();  // wait for NFA_EE_SET_TECH_CFG_EVT
            stat = NFA_EeUpdateNow();
            if (stat == NFA_STATUS_OK) {
              sEeCheck.wait();  // wait for NFA_EE_UPDATED_EVT
            }
          }
        }

        sIsHceFEnabled = true;
      }
    }
    if (st_mt_g_state->ce_sel != st_mt_state::HCE) {
      // ------------------
      LOG(INFO) << StringPrintf("%s: trying to activate EE %d", __FUNCTION__,
                                st_mt_g_state->ce_sel);

      uint8_t se_id = 0;
      int res = 1;  // FAIL

      se_id = get_SEID_for_se(st_mt_g_state->ce_sel);
      if (!se_id) {
        LOG(ERROR) << StringPrintf("%s: swpnr not supported %d", __FUNCTION__,
                                   st_mt_g_state->ce_sel);
        sActiveEeError =
            true;  // We were not able to find this SE, reinit in case.
        (void)st_mt_glue_resetmode();  // deactivate if needed
        return 1;
      }

      res = activate_SEID(se_id);
      if (res != 0) {
        LOG(ERROR) << StringPrintf(
            "%s: EE was not activated, so we cannot set the Card Emulation to "
            "it",
            __FUNCTION__);
        sActiveEeError =
            true;  // We were not able to find this SE, reinit in case.
        (void)st_mt_glue_resetmode();  // deactivate if needed
        return 1;
      }

      LOG(INFO) << StringPrintf("%s: NFA_CeConfigureUiccListenTech, mask %x",
                                __FUNCTION__, sListeningMask);
      // OK the SE is active now.
      sCeUiccListenCheck.start(true);
      if ((stat = NFA_CeConfigureUiccListenTech(sActiveEe, sListeningMask)) !=
          NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf(
            "%s: NFA_CeConfigureUiccListenTech; error=0x%X", __FUNCTION__,
            stat);
        sCeUiccListenCheck.end();
        return 1;
      }
      // NFA_CE_UICC_LISTEN_CONFIGURED_EVT
      {
        if (!sCeUiccListenCheck.wait(2000)) {
          LOG(ERROR) << StringPrintf(
              "%s: NFA_CeConfigureUiccListenTech; timeout", __FUNCTION__);
          sCeResult = NFA_STATUS_FAILED;
        }

        if (sCeResult != NFA_STATUS_OK) {
          LOG(ERROR) << StringPrintf(
              "%s: NFA_CeConfigureUiccListenTech; error=0x%X", __FUNCTION__,
              sCeResult);
          sCeUiccListenCheck.end();
          return 1;
        }
      }
      sCeUiccListenCheck.end();

#if 0  // The below is the default anyway, let the FW deal with non-supported
       // techs set routing config
            LOG(INFO) << StringPrintf ("%s: NFA_EeSetDefaultTechRouting, mask %x", __FUNCTION__, sListeningMask);
            if ((stat = NFA_EeSetDefaultTechRouting(sActiveEe, sListeningMask, 0, 0)) != NFA_STATUS_OK) {
                LOG(ERROR) << StringPrintf ("%s: NFA_EeSetDefaultTechRouting; error=0x%X", __FUNCTION__, stat);
                return 1;
            }
            LOG(INFO) << StringPrintf ("%s: NFA_EeUpdateNow", __FUNCTION__);
            if ((stat = NFA_EeUpdateNow()) != NFA_STATUS_OK) {
                LOG(ERROR) << StringPrintf ("%s: NFA_EeUpdateNow; error=0x%X", __FUNCTION__, stat);
                return 1;
            }

            // NFA_EE_SET_TECH_CFG_EVT
            {
                sEeCheck.start();
                LOG(INFO) << StringPrintf ("%s: wait for NFA_EE_SET_TECH_CFG_EVT", __FUNCTION__);
                sEeCheck.wait ();
                sEeCheck.end();
                if (sEeStatus.set_tech != NFA_STATUS_OK) {
                    LOG(ERROR) << StringPrintf ("%s: NFA_EeSetDefaultTechRouting; error=0x%X", __FUNCTION__, sEeStatus.set_tech);
                    return 1;
                }
            }
#else
      // Use the Force Routing feature so we are very sure nothing goes to the
      // DH.
      {
        sEeCheck.start();
        if ((stat = NFA_EeForceRouting(
                 sActiveEe, 0x39 /* sitched ON only */)) != NFA_STATUS_OK) {
          LOG(ERROR) << StringPrintf("%s: NFA_EeForceRouting; error=0x%X",
                                     __FUNCTION__, stat);
          sEeCheck.end();
          return 1;
        }
        LOG(INFO) << StringPrintf("%s: wait for NFA_EE_FORCE_ROUTING_EVT",
                                  __FUNCTION__);
        sEeCheck.wait();
        sEeCheck.end();
        if (sEeStatus.status != NFA_STATUS_OK) {
          LOG(ERROR) << StringPrintf("%s: NFA_EeForceRouting cb; error=0x%X",
                                     __FUNCTION__, sEeStatus.status);
          return 1;
        }
        sEeForcedRouting = true;
      }

#endif
      LOG(INFO) << StringPrintf(
          "%s: Card emulation set successfully. Ready for RF transactions",
          __FUNCTION__);
    }

    st_mt_g_state->ce_cb = ce_cb;
  }

  if (!sPollingMask && !sListeningMask && !sP2pListeningMask) {
    LOG(ERROR) << StringPrintf(
        "Error: no technology requested, not starting discovery");
    return -1;
  }

  if (sPollingMask) {
    LOG(INFO) << StringPrintf("%s: enable polling, mask %x", __FUNCTION__,
                              sPollingMask);
    sNfaEnableDisablePollingEvent.start(true);
    stat = NFA_EnablePolling(sPollingMask);
    if (stat == NFA_STATUS_OK) {
      LOG(INFO) << StringPrintf("%s: wait for enable event", __FUNCTION__);

      sPollingEnabled = true;
      if (!sNfaEnableDisablePollingEvent.wait(
              500)) {  // wait for NFA_POLL_ENABLED_EVT
        LOG(ERROR) << StringPrintf("%s: fail enable polling; timeout",
                                   __FUNCTION__);
        sNfaEnableDisablePollingEvent.end();
        return -1;
      }
    } else {
      LOG(ERROR) << StringPrintf("%s: fail enable polling; error=0x%X",
                                 __FUNCTION__, stat);
      sNfaEnableDisablePollingEvent.end();
      return -1;
    }
    sNfaEnableDisablePollingEvent.end();
  }

  if (sP2pListeningMask) {
    LOG(INFO) << StringPrintf("%s: enable P2P listening, mask %x", __FUNCTION__,
                              sP2pListeningMask);
    sNfaP2pEvent.start(true);
    stat = NFA_SetP2pListenTech(sP2pListeningMask);
    if (stat == NFA_STATUS_OK) {
      LOG(INFO) << StringPrintf("%s: wait for enable event", __FUNCTION__);
      sNfaP2pEvent.wait();  // wait for NFA_POLL_ENABLED_EVT
      sIsP2pListening = true;
    } else {
      LOG(ERROR) << StringPrintf("%s: fail enable P2P listen techs; error=0x%X",
                                 __FUNCTION__, stat);
      sNfaP2pEvent.end();
      return -1;
    }
    sNfaP2pEvent.end();
  }

  if (p2p_cb != NULL) {
    sNfaP2pEvent.start(true);
    stat = NFA_ResumeP2p();
    if (stat == NFA_STATUS_OK) {
      LOG(INFO) << StringPrintf("%s: wait for resumed event", __FUNCTION__);
      sNfaP2pEvent.wait();
    } else {
      LOG(ERROR) << StringPrintf("%s: fail resume P2P; error=0x%X",
                                 __FUNCTION__, stat);
      sNfaP2pEvent.end();
      return -1;
    }
    sNfaP2pEvent.end();
  }

  if ((st_mt_g_state->ce_sel == st_mt_state::HCE) || sP2pListeningMask) {
    // ------------------
    // workaround because otherwise starting HCE-F then P2P target F does not
    // work.
    if (p2p_cb != NULL) {
      SET_NCIPARAM(NCI_PARAM_ID_LF_PROTOCOL, 0x02);
    }

    LOG(INFO) << StringPrintf("%s: enable listening (%x, %x)", __FUNCTION__,
                              sListeningMask, sP2pListeningMask);
    stat = NFA_EnableListening();  // we ignore the event at the moment.
    if (stat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail enable listening; error=0x%X",
                                 __FUNCTION__, stat);
      return -1;
    }
    usleep(100000);
  }

  // Now start RF Discovery.
  {
    sNfaEnableDisablePollingEvent.start(true);
    stat = NFA_StartRfDiscovery();
    if (stat == NFA_STATUS_OK) {
      LOG(INFO) << StringPrintf("%s: wait for discovery started event",
                                __FUNCTION__);
      sRfEnabled = true;
      if (!sNfaEnableDisablePollingEvent.wait(500)) {
        LOG(ERROR) << StringPrintf("%s: fail to start RF Discovery; timeout",
                                   __FUNCTION__);
        sNfaEnableDisablePollingEvent.end();
        return -1;
      }
    } else {
      LOG(ERROR) << StringPrintf("%s: fail to start RF Discovery; error=0x%X",
                                 __FUNCTION__, stat);
      sNfaEnableDisablePollingEvent.end();
      return -1;
    }
    sNfaEnableDisablePollingEvent.end();
  }

  return 0;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int st_mt_glue_ndef_format() {
  if (!sIsNfaEnabled) {
    LOG(INFO) << StringPrintf("%s: NFC disabled", __FUNCTION__);
    return 1;
  }

  sNdefCheck.start(true);
  if (NFA_STATUS_OK != NFA_RwFormatTag()) {
    LOG(ERROR) << StringPrintf("%s: Tag formatting failed.", __FUNCTION__);
    sNdefCheck.end();
    return -1;
  }

  // Wait for the format event.
  sNdefCheck.wait();
  sNdefCheck.end();

  if (sNdefResult != NFA_STATUS_OK) {
    LOG(INFO) << StringPrintf("%s: Returning error to caller.", __FUNCTION__);
    return -1;
  }

  LOG(INFO) << StringPrintf("%s: Tag formatted successfully.", __FUNCTION__);
  return 0;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int st_mt_glue_ndef_write(struct ndef_msg *msg) {
  uint8_t *rawNdefBuf = NULL;
  uint32_t rawNdefSz = 0;

  if (!sIsNfaEnabled) {
    LOG(INFO) << StringPrintf("%s: NFC disabled", __FUNCTION__);
    return 1;
  }

  // convert the msg to a plain NDEF bytes buffer
  LOG(ERROR) << StringPrintf("%s: type: %d", __FUNCTION__, msg->type);
  LOG(ERROR) << StringPrintf("%s: lang: %d", __FUNCTION__, msg->lang);
  for (int i = 0; (uint32_t)i < msg->len; i++) {
    LOG(ERROR) << StringPrintf("%s: data[%d/%d]: %02hhx", __FUNCTION__, i,
                               msg->len, msg->data[i]);
  }

#if 0
    64293: 01-01 00:38:59.736 D/ST_MTKTools( 2367): Received EM_ALS_READER_MODE_OPT_REQ
64294: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        type: 0
64295: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        lang: 0
64296: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[0/66]: 6d
64297: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[1/66]: 65
64298: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[2/66]: 64
64299: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[3/66]: 69
64300: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[4/66]: 61
64301: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[5/66]: 74
64302: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[6/66]: 65
64303: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[7/66]: 6b
64304: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[8/66]: 2e
64305: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[9/66]: 63
64306: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[10/66]: 6f
64307: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[11/66]: 6d
64308: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[12/66]: 00
64309: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[13/66]: 00
64310: 01-01 00:38:59.736 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[14/66]: 00
64311: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[15/66]: 00
64312: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[16/66]: 00
64313: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[17/66]: 00
64314: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[18/66]: 00
64315: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[19/66]: 00
64316: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[20/66]: 00
64317: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[21/66]: 00
64318: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[22/66]: 00
64319: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[23/66]: 00
64320: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[24/66]: 00
64321: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[25/66]: 00
64322: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[26/66]: 00
64323: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[27/66]: 00
64324: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[28/66]: 00
64325: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[29/66]: 00
64326: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[30/66]: 00
64327: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[31/66]: 00
64328: 01-01 00:38:59.737 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[32/66]: 00
64329: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[33/66]: 00
64330: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[34/66]: 00
64331: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[35/66]: 00
64332: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[36/66]: 00
64333: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[37/66]: 00
64334: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[38/66]: 00
64335: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[39/66]: 00
64336: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[40/66]: 00
64337: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[41/66]: 00
64338: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[42/66]: 00
64339: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[43/66]: 00
64340: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[44/66]: 00
64341: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[45/66]: 00
64342: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[46/66]: 00
64343: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[47/66]: 00
64344: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[48/66]: 00
64345: 01-01 00:38:59.738 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[49/66]: 00
64346: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[50/66]: 00
64347: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[51/66]: 00
64348: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[52/66]: 00
64349: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[53/66]: 00
64350: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[54/66]: 00
64351: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[55/66]: 00
64352: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[56/66]: 00
64353: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[57/66]: 00
64354: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[58/66]: 00
64355: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[59/66]: 00
64356: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[60/66]: 00
64357: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[61/66]: 00
64358: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[62/66]: 00
64359: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[63/66]: 00
64360: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[64/66]: 40
64361: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
        data[65/66]: 00
64362: 01-01 00:38:59.739 E/ST_MTKTools( 2367): st_mt_glue_ndef_write:
NOT IMPLEMENTED YET :
        convert NDEF format
#endif

  LOG(ERROR) << StringPrintf("%s: NOT IMPLEMENTED YET : convert NDEF format",
                             __FUNCTION__);
  return -1;

  // Now write this to the tag.
  sNdefCheck.start(true);
  if (NFA_STATUS_OK != NFA_RwWriteNDef(rawNdefBuf, rawNdefSz)) {
    LOG(ERROR) << StringPrintf(
        "%s: NFA_RwWriteNDef failed, returning error to caller.", __FUNCTION__);
    sNdefCheck.end();
    return -1;
  }

  // Wait for the NFA_RW_WRITE_CPLT_EVT event.
  sNdefCheck.wait();
  sNdefCheck.end();

  if (sNdefResult != NFA_STATUS_OK) {
    LOG(INFO) << StringPrintf("%s: Returning error to caller.", __FUNCTION__);
    return -1;
  }

  free(rawNdefBuf);

  return 0;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

int st_mt_glue_ndef_read(struct ndef_msg *msg) {
  tNFA_STATUS status;

  if (!sIsNfaEnabled) {
    LOG(INFO) << StringPrintf("%s: NFC disabled", __FUNCTION__);
    return 1;
  }

  sNdefCheck.start(true);
  status = NFA_RwReadNDef();
  if (status != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: Unable to read NDEF: %d", __FUNCTION__,
                               status);
    sNdefCheck.end();
    return -1;
  }

  // Wait for the ndef_cb to receive the message.
  sNdefCheck.wait();
  sNdefCheck.end();

  if (sNdefResult != NFA_STATUS_OK) {
    LOG(INFO) << StringPrintf("%s: Returning error to caller.", __FUNCTION__);
    return -1;
  }

  LOG(INFO) << StringPrintf("%s: Returning NDEF message to caller.",
                            __FUNCTION__);
  memcpy(msg, &sNdefMessage, sizeof(struct ndef_msg));
  memset(&sNdefMessage, 0, sizeof(struct ndef_msg));

  return 0;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
int st_mt_glue_test_swp(int swpnr) {
  uint8_t se_id = 0;
  int res = 1;  // FAIL
  LOG(INFO) << StringPrintf("%s: test for %d", __FUNCTION__, swpnr);

  if (!sIsNfaEnabled) {
    LOG(INFO) << StringPrintf("%s: NFC disabled", __FUNCTION__);
    return 1;
  }

  se_id = get_SEID_for_se(swpnr);
  if (!se_id) {
    LOG(ERROR) << StringPrintf("%s: swpnr not supported %d", __FUNCTION__,
                               st_mt_g_state->ce_sel);
    sActiveEeError = true;  // We were not able to find this SE, reinit in case.
  } else if (activate_SEID(se_id) == 0) {
    res = 0;
    deactivate_activeSE();
  } else {
    sActiveEeError = true;  // We were not able to initialize this SE
  }

  if (sActiveEeError == true) {
    (void)st_mt_glue_resetmode();  // deactivate if needed
  }

  LOG(INFO) << StringPrintf("%s: result %d", __FUNCTION__, res);
  return res;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

static int st_mt_glue_prepareGate(uint8_t gate_id) {
  LOG(INFO) << StringPrintf("%s: enter", __FUNCTION__);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  int ret = -1;
  struct gate_info *gateInfo;

  if (gate_id == ID_MGMT_GATE_ID) {
    gateInfo = &sIdMgmtInfo;
  } else if (gate_id == LOOPBACK_GATE_ID) {  // Loopback gate
    gateInfo = &sLoopbackInfo;
  } else {
    return ret;
  }

  if (sHciHandle == NFA_HANDLE_INVALID) {
    return ret;
  }

  if (gateInfo->added == false) {
    LOG(INFO) << StringPrintf("%s: NFA_HciAllocGate 0x%X", __FUNCTION__,
                              gate_id);
    // Add static pipe to accept notify pipe created from NFCC at eSE initial
    // activation
    sNfaHciAllocGateEvent.start(true);
    nfaStat = NFA_HciAllocGate(sHciHandle, gate_id);

    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail adding static gate; error=0x%X",
                                 __FUNCTION__, nfaStat);
      sNfaHciAllocGateEvent.end();
      return (ret);
    }
    sNfaHciAllocGateEvent.wait();  // wait for NFA_HCI_CREATE_PIPE_EVT
    sNfaHciAllocGateEvent.end();

    gateInfo->added = true;
  }

  // Create pipe, if not yet done
  if (gateInfo->created == false) {
    LOG(INFO) << StringPrintf("%s: NFA_HciCreatePipe %02hhx %02hhx %02hhx",
                              __FUNCTION__, gate_id, (uint8_t)CLF_ID, gate_id);
    sNfaHciCreatePipeEvent.start();
    if ((nfaStat = NFA_HciCreatePipe(sHciHandle, gate_id, CLF_ID, gate_id)) ==
        NFA_STATUS_OK) {
      sNfaHciCreatePipeEvent.wait();  // wait for NFA_HCI_CREATE_PIPE_EVT

      if (!gateInfo->created) {
        LOG(INFO) << StringPrintf("%s: pipe was not created.", __FUNCTION__);
        sNfaHciCreatePipeEvent.end();
        return ret;
      }
    } else {
      LOG(ERROR) << StringPrintf("%s: NFA_HciCreatePipe failed; error=0x%X",
                                 __FUNCTION__, nfaStat);
      sNfaHciCreatePipeEvent.end();
      return ret;
    }
    sNfaHciCreatePipeEvent.end();
  }

  // Open pipe, if not yet done
  if (gateInfo->opened == false) {
    gateInfo->open_pending = true;
    sNfaHciOpenPipeEvent.start(true);
    if ((nfaStat = NFA_HciOpenPipe(sHciHandle, gateInfo->pipe_id)) ==
        NFA_STATUS_OK) {
      sNfaHciOpenPipeEvent.wait();  // wait for NFA_HCI_OPEN_PIPE_EVT
      gateInfo->open_pending = false;

      if (!gateInfo->opened) {
        LOG(INFO) << StringPrintf("%s: pipe was not opened.", __FUNCTION__);
        sNfaHciOpenPipeEvent.end();
        return ret;
      }

    } else {
      LOG(ERROR) << StringPrintf("%s: NFA_HciOpenPipe failed; error=0x%X",
                                 __FUNCTION__, nfaStat);
      sNfaHciOpenPipeEvent.end();
      return ret;
    }
    sNfaHciOpenPipeEvent.end();
  }

  LOG(INFO) << StringPrintf("%s: exit", __FUNCTION__);
  return 0;
}

static int st_mt_glue_releaseGate(uint8_t gate_id) {
  LOG(INFO) << StringPrintf("%s: enter", __FUNCTION__);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  int ret = -1;
  struct gate_info *gateInfo;

  if (gate_id == ID_MGMT_GATE_ID) {
    gateInfo = &sIdMgmtInfo;
  } else if (gate_id == LOOPBACK_GATE_ID) {  // Loopback gate
    gateInfo = &sLoopbackInfo;
  } else {
    return ret;
  }

  if (sHciHandle == NFA_HANDLE_INVALID) {
    return ret;
  }

  // close pipe, if it was opened
  if (gateInfo->opened != false) {
    LOG(INFO) << StringPrintf("%s: NFA_HciClosePipe 0x%X", __FUNCTION__,
                              gateInfo->pipe_id);
    gateInfo->close_pending = true;
    sNfaHciClosePipeEvent.start(true);
    if ((nfaStat = NFA_HciClosePipe(sHciHandle, gateInfo->pipe_id)) ==
        NFA_STATUS_OK) {
      sNfaHciClosePipeEvent.wait();  // wait for
      gateInfo->opened = false;
      gateInfo->close_pending = false;
    } else {
      LOG(ERROR) << StringPrintf("%s: NFA_HciClosePipe failed; error=0x%X",
                                 __FUNCTION__, nfaStat);
      sNfaHciClosePipeEvent.end();
      return ret;
    }
    sNfaHciClosePipeEvent.end();
  }

  // delete pipe,
  if (gateInfo->created != false) {
    LOG(INFO) << StringPrintf("%s: NFA_HciDeletePipe 0x%X", __FUNCTION__,
                              gateInfo->pipe_id);
    sNfaHciDeletePipeEvent.start();
    if ((nfaStat = NFA_HciDeletePipe(sHciHandle, gateInfo->pipe_id)) ==
        NFA_STATUS_OK) {
      sNfaHciDeletePipeEvent.wait();  // wait for
      gateInfo->created = false;
      gateInfo->pipe_id = 0x00;
    } else {
      LOG(ERROR) << StringPrintf("%s: NFA_HciDeletePipe failed; error=0x%X",
                                 __FUNCTION__, nfaStat);
      sNfaHciDeletePipeEvent.end();
      return ret;
    }
    sNfaHciDeletePipeEvent.end();
  }

  if (gateInfo->added != false) {
    // try removing the gate but this seems to always fail... not a real issue.
    LOG(INFO) << StringPrintf("%s: NFA_HciDeallocGate 0x%X", __FUNCTION__,
                              gate_id);
    nfaStat = NFA_HciDeallocGate(sHciHandle, gate_id);

    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail removing static gate; error=0x%X",
                                 __FUNCTION__, nfaStat);
      // return (ret);
    }

    gateInfo->added = false;
  }

  LOG(INFO) << StringPrintf("%s: exit", __FUNCTION__);
  return 0;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

#define SIZE_LOOPBACK_DATA 240

int st_mt_glue_test_loopback() {
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  int ret, i;
  uint8_t txData[SIZE_LOOPBACK_DATA], rxData[SIZE_LOOPBACK_DATA];

  LOG(INFO) << StringPrintf("%s: enter", __FUNCTION__);

  ret = st_mt_glue_prepareGate(LOOPBACK_GATE_ID);
  if (ret != 0) {
    return 1;  // error
  }

  // Send data to CLF
  for (i = 0; i < SIZE_LOOPBACK_DATA; i++) {
    txData[i] = i;
  }

  ret = 1;
  sNfaHciEventRcvdEvent.start(true);
  nfaStat =
      NFA_HciSendEvent(sHciHandle, sLoopbackInfo.pipe_id, NFA_HCI_EVT_POST_DATA,
                       SIZE_LOOPBACK_DATA, txData, sizeof(rxData), rxData, 0);
  if (nfaStat == NFA_STATUS_OK) {
    sNfaHciEventRcvdEvent.wait();  // wait for NFA_HCI_EVENT_RCVD_EVT
  } else {
    LOG(ERROR) << StringPrintf("%s: NFA_HciSendEvent failed; error=0x%X",
                               __FUNCTION__, nfaStat);
    sNfaHciEventRcvdEvent.end();
    goto end;
  }
  sNfaHciEventRcvdEvent.end();

  // Check received data
  if (sRspSize != SIZE_LOOPBACK_DATA) {
    LOG(ERROR) << StringPrintf(
        "%s: Returned length for loopback is wrong: expected %d, received %d",
        __FUNCTION__, SIZE_LOOPBACK_DATA, sRspSize);
    goto end;
  }

  for (i = 0; i < SIZE_LOOPBACK_DATA; i++) {
    if (rxData[i] != txData[i]) {
      LOG(ERROR) << StringPrintf(
          "%s: Error in received data: rxData[%d] = 0x%x, txData[%d] = 0x%x",
          __FUNCTION__, i, rxData[i], i, txData[i]);
      goto end;
    }
  }

  LOG(INFO) << StringPrintf("%s: received expected data, test successful",
                            __FUNCTION__);
  ret = 0;

end:

  st_mt_glue_releaseGate(LOOPBACK_GATE_ID);

  LOG(INFO) << StringPrintf("%s: exit (%d)", __FUNCTION__, ret);
  return ret;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

#define ST_OID_TEST 0x03
#define PROP_TEST_FIELD 0xB3
#define PROP_TEST_FIELD_ON 0x01
#define PROP_TEST_FIELD_OFF 0x00

int st_mt_glue_test_TXON(int on) {
  uint8_t param_on[] = {PROP_TEST_FIELD, PROP_TEST_FIELD_ON};
  uint8_t param_off[] = {PROP_TEST_FIELD, PROP_TEST_FIELD_OFF};
  tNFA_STATUS status;

  if (!sIsNfaEnabled) {
    LOG(INFO) << StringPrintf("%s: NFC disabled", __FUNCTION__);
    return 1;
  }

  LOG(INFO) << StringPrintf("%s: Sending TX ON/OFF command (%d)", __FUNCTION__,
                            on);
  sNfaVSEvent.start(true);
  if (on) {
    status = NFA_SendVsCommand(ST_OID_TEST, sizeof(param_on), param_on,
                               st_mt_glue_nfaVsCb);
  } else {
    status = NFA_SendVsCommand(ST_OID_TEST, sizeof(param_off), param_off,
                               st_mt_glue_nfaVsCb);
  }

  if (status != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf(
        "%s: NFA_SendVsCommand() call failed; error=0x%X", __FUNCTION__,
        status);
    sNfaVSEvent.end();
    return 1;
  }

  sNfaVSEvent.wait();
  sNfaVSEvent.end();
  return (sNfaVSStatus == NFA_STATUS_OK) ? 0 : 1;
}

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

#define PROP_TEST_DIAG_MEAS 0xB6

// Antenna test
int st_mt_glue_test_antenna_DIAG_MEAS(
    uint16_t *diff_ampl, uint16_t *diff_phase, uint16_t *rfo1_ampl,
    uint16_t *rfo1_phase, uint16_t *rfo2_ampl, uint16_t *rfo2_phase,
    uint8_t *vddrf,
    uint8_t rege,  // configuration for the measure. b7 must be 0
    int nrloops    // hom many times to measure to have a better average
) {
  uint8_t param_on[] = {PROP_TEST_FIELD, PROP_TEST_FIELD_ON};
  uint8_t param_off[] = {PROP_TEST_FIELD, PROP_TEST_FIELD_OFF};
  uint8_t param_meas[] = {PROP_TEST_DIAG_MEAS, 0};
  tNFA_STATUS status;
  int i;

  uint32_t diff_ampl_total = 0;
  uint32_t diff_phase_total = 0;
  uint32_t rfo1_ampl_total = 0;
  uint32_t rfo1_phase_total = 0;
  uint32_t rfo2_ampl_total = 0;
  uint32_t rfo2_phase_total = 0;
  uint32_t vddrf_total = 0;

  param_meas[1] = rege;
  if (rege & 0x80) {
    LOG(ERROR) << StringPrintf("%s: Store measure in NVM is not supported yet",
                               __func__);
    return 1;
  }

  if (!sIsNfaEnabled) {
    LOG(INFO) << StringPrintf("%s: NFC disabled", __FUNCTION__);
    return 1;
  }

  LOG(INFO) << StringPrintf("%s: Sending TX ON command", __FUNCTION__);
  sNfaVSEvent.start(true);
  status = NFA_SendVsCommand(ST_OID_TEST, sizeof(param_on), param_on,
                             st_mt_glue_nfaVsCb);
  if (status != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf(
        "%s: NFA_SendVsCommand() call failed; error=0x%X", __FUNCTION__,
        status);
    sNfaVSEvent.end();
    return 1;
  }

  sNfaVSEvent.wait();
  sNfaVSEvent.end();
  if (sNfaVSStatus != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: Failed to turn the field ON", __func__);
    return 1;
  }

  // First measure tends to return inconsistent data, so we just skip it.
  {
    LOG(INFO) << StringPrintf("%s: Sending dummy MEAS_DIAG command first",
                              __FUNCTION__);
    sNfaVSEvent.start(true);
    status = NFA_SendVsCommand(ST_OID_TEST, sizeof(param_meas), param_meas,
                               st_mt_glue_nfaVsCb);
    if (status != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "%s: NFA_SendVsCommand() call failed; error=0x%X", __FUNCTION__,
          status);
      sNfaVSEvent.end();
      return 1;
    }
    sNfaVSEvent.wait();
    sNfaVSEvent.end();
    if (sNfaVSStatus != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "%s: Failed to receive antenna diagnostic data", __func__);
      return 1;
    }
  }

  // Measure N times (each time command is sent, the FW measures 8 times
  // already, but we do more measures for more averaged result)
  LOG(INFO) << StringPrintf(
      "%s: \tCSV: diff_ampl, diff_phas, rfo1_ampl, rfo1_phas, rfo2_ampl, "
      "rfo2_phas, vddrf",
      __func__);
  for (i = 0; i < nrloops; i++) {
    LOG(INFO) << StringPrintf("%s: Sending MEAS_DIAG command (#%d)",
                              __FUNCTION__, i);
    sNfaVSEvent.start(true);
    status = NFA_SendVsCommand(ST_OID_TEST, sizeof(param_meas), param_meas,
                               st_mt_glue_nfaVsCb);
    if (status != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "%s: NFA_SendVsCommand() call failed; error=0x%X", __FUNCTION__,
          status);
      sNfaVSEvent.end();
      return 1;
    }
    sNfaVSEvent.wait();
    sNfaVSEvent.end();
    if (sNfaVSStatus != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "%s: Failed to receive antenna diagnostic data", __func__);
      return 1;
    }
    // We got 1 measure in sNfaVSResp
    LOG(INFO) << StringPrintf("%s: \tCSV: %u, %u, %u, %u, %u, %u, %hhu",
                              __func__, (sNfaVSResp[0] << 8) | sNfaVSResp[1],
                              (sNfaVSResp[2] << 8) | sNfaVSResp[3],
                              (sNfaVSResp[4] << 8) | sNfaVSResp[5],
                              (sNfaVSResp[6] << 8) | sNfaVSResp[7],
                              (sNfaVSResp[8] << 8) | sNfaVSResp[9],
                              (sNfaVSResp[10] << 8) | sNfaVSResp[11],
                              sNfaVSResp[12]);

    diff_ampl_total += (sNfaVSResp[0] << 8) | sNfaVSResp[1];
    diff_phase_total += (sNfaVSResp[2] << 8) | sNfaVSResp[3];
    rfo1_ampl_total += (sNfaVSResp[4] << 8) | sNfaVSResp[5];
    rfo1_phase_total += (sNfaVSResp[6] << 8) | sNfaVSResp[7];
    rfo2_ampl_total += (sNfaVSResp[8] << 8) | sNfaVSResp[9];
    rfo2_phase_total += (sNfaVSResp[10] << 8) | sNfaVSResp[11];
    vddrf_total += sNfaVSResp[12];
  }

  LOG(INFO) << StringPrintf("%s: Sending TX OFF command", __FUNCTION__);
  sNfaVSEvent.start(true);
  status = NFA_SendVsCommand(ST_OID_TEST, sizeof(param_off), param_off,
                             st_mt_glue_nfaVsCb);
  if (status != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf(
        "%s: NFA_SendVsCommand() call failed; error=0x%X", __FUNCTION__,
        status);
    sNfaVSEvent.end();
    return 1;
  }

  sNfaVSEvent.wait();
  sNfaVSEvent.end();
  if (sNfaVSStatus != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: Failed to turn the field ON", __func__);
    return 1;
  }

  diff_ampl_total /= nrloops;
  diff_phase_total /= nrloops;
  rfo1_ampl_total /= nrloops;
  rfo1_phase_total /= nrloops;
  rfo2_ampl_total /= nrloops;
  rfo2_phase_total /= nrloops;
  vddrf_total /= nrloops;

  *diff_ampl = (uint16_t)diff_ampl_total;
  *diff_phase = (uint16_t)diff_phase_total;
  *rfo1_ampl = (uint16_t)rfo1_ampl_total;
  *rfo1_phase = (uint16_t)rfo1_phase_total;
  *rfo2_ampl = (uint16_t)rfo2_ampl_total;
  *rfo2_phase = (uint16_t)rfo2_phase_total;
  *vddrf = (uint8_t)vddrf_total;

  return 0;
}

// -----------------------------
