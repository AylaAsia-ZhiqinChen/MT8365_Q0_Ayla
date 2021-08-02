/*
 *  Copyright (C) 2013 ST Microelectronics S.A.
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
 *  Provide extensions for the ST implementation of the NFC stack
 */

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <errno.h>
#include <malloc.h>
#include <nativehelper/ScopedLocalRef.h>
#include <nativehelper/ScopedPrimitiveArray.h>
#include "JavaClassConstants.h"
#include "StCardEmulationEmbedded.h"
/* NCI 2.0 - Begin */
#include "StRoutingManager.h"
/* NCI 2.0 - End */
#include "NfcAdaptation.h"
#include "config.h"
#include "StNfcJni.h"

/*****************************************************************************
 **
 ** public variables
 **
 *****************************************************************************/
using android::base::StringPrintf;
extern bool nfc_debug_enabled;
extern SyncEvent gIsReconfiguringDiscovery;

namespace android {
extern bool isDiscoveryStarted();
extern void startRfDiscovery(bool isStart);
}  // namespace android

//////////////////////////////////////////////
//////////////////////////////////////////////

StCardEmulationEmbedded StCardEmulationEmbedded::sCardEmulationEmbedded;

/*******************************************************************************
 **
 ** Function:        StCardEmulationEmbedded
 **
 ** Description:     Initialize member variables.
 **
 ** Returns:         None
 **
 *******************************************************************************/
StCardEmulationEmbedded::StCardEmulationEmbedded() {
  mIdConnectCee = CEE_NO_APP;
  mNeedRfRestart = false;
  mNfaEECbStatus = NFA_STATUS_FAILED;
}

/*******************************************************************************
 **
 ** Function:        ~StCardEmulationEmbedded
 **
 ** Description:     Release all resources.
 **
 ** Returns:         None
 **
 *******************************************************************************/
StCardEmulationEmbedded::~StCardEmulationEmbedded() {}

/*******************************************************************************
 **
 ** Function:        initialize
 **
 ** Description:     Initialize all member variables.
 **                  native: Native data.
 **
 ** Returns:         None
 **
 *******************************************************************************/
void StCardEmulationEmbedded::initialize(nfc_jni_native_data* native) {
  static const char fn[] = "StCardEmulationEmbedded::initialize";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", fn);

  mNativeData = native;

  tNFA_STATUS nfaStat;
  {
    SyncEventGuard guard(mEeRegisterEvent);
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: try ee register", fn);
    nfaStat = NFA_EeRegister(nfaEeCallback);
    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail ee register; error=0x%X", fn,
                                 nfaStat);
      return;
    }
    mEeRegisterEvent.wait();
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
}

/*******************************************************************************
 **
 ** Function:        finalize
 **
 ** Description:     Release all resources.
 **
 ** Returns:         None
 **
 *******************************************************************************/
void StCardEmulationEmbedded::finalize() {}

/*******************************************************************************
 **
 ** Function:        abortWaits
 **
 ** Description:     Release all resources.
 **
 ** Returns:         None
 **
 *******************************************************************************/
void StCardEmulationEmbedded::abortWaits() {
  static const char fn[] = "StCardEmulationEmbedded::abortWaits";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", fn);

  {
    SyncEventGuard g(mEeRegisterEvent);
    mEeRegisterEvent.notifyOne();
  }
  {
    SyncEventGuard g(mEeSetModeEvent);
    mEeSetModeEvent.notifyOne();
  }
}

/*******************************************************************************
 **
 ** Function:        getInstance
 **
 ** Description:     Get the StSecureElement singleton object.
 **
 ** Returns:         StSecureElement object.
 **
 *******************************************************************************/
StCardEmulationEmbedded& StCardEmulationEmbedded::getInstance() {
  return sCardEmulationEmbedded;
}

/*******************************************************************************
**
** Function:        nfaEeCallback
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StCardEmulationEmbedded::nfaEeCallback(tNFA_EE_EVT event,
                                            tNFA_EE_CBACK_DATA* eventData) {
  static const char fn[] = "StCardEmulationEmbedded::nfaEeCallback";

  StCardEmulationEmbedded& StCardEmulationEmbedded =
      StCardEmulationEmbedded::getInstance();

  switch (event) {
    case NFA_EE_REGISTER_EVT: {
      SyncEventGuard guard(StCardEmulationEmbedded.mEeRegisterEvent);
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_REGISTER_EVT; status=%u", fn, eventData->ee_register);
      StCardEmulationEmbedded.mNfaEECbStatus = eventData->ee_register;
      StCardEmulationEmbedded.mEeRegisterEvent.notifyOne();
    } break;

    case NFA_EE_MODE_SET_EVT: {
      if (eventData->mode_set.ee_handle & 0x80) {
        // HCI NFCEE, exit
        break;
      }
      SyncEventGuard guard(StCardEmulationEmbedded.mEeSetModeEvent);
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_MODE_SET_EVT; status: 0x%04X  handle: 0x%04X", fn,
          eventData->mode_set.status, eventData->mode_set.ee_handle);

      if (eventData->mode_set.status != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf(
            "%s: NFA_EE_MODE_SET_EVT; status: 0x%04X  error !!!", fn,
            eventData->mode_set.status);
      }
      StCardEmulationEmbedded.mNfaEECbStatus = eventData->mode_set.status;
      StCardEmulationEmbedded.mEeSetModeEvent.notifyOne();
    } break;

    case NFA_EE_DISCOVER_REQ_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_DISCOVER_REQ_EVT; status=0x%X; num ee=%u", __func__,
          eventData->discover_req.status, eventData->discover_req.num_ee);

      StCardEmulationEmbedded.mNfaEECbStatus = eventData->discover_req.status;
    } break;

    case NFA_EE_CONNECT_EVT: {
      SyncEventGuard guard(StCardEmulationEmbedded.mEeCreateConnEvent);
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_CONNECT_EVT; status=%u, handle=0x%02X, interface=0x%02X",
          fn, eventData->connect.status, eventData->connect.ee_handle,
          eventData->connect.ee_interface);

      if (eventData->connect.status == NFA_STATUS_OK) {
        sCardEmulationEmbedded.mIdConnectCee =
            eventData->connect.ee_handle & 0xFF;
      }

      StCardEmulationEmbedded.mNfaEECbStatus = eventData->connect.status;

      StCardEmulationEmbedded.mEeCreateConnEvent.notifyOne();
    } break;

    case NFA_EE_DATA_EVT: {
      SyncEventGuard guard(StCardEmulationEmbedded.mEeDataEvent);
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_EE_DATA_EVT; handle=0x%02X, length=0x%02X",
                          fn, eventData->data.handle, eventData->data.len);

      for (int i = 0; i < eventData->data.len; i++) {
        StCardEmulationEmbedded.mResponseData[i] = eventData->data.p_buf[i];
      }

      StCardEmulationEmbedded.mResponseDataLength = eventData->data.len;

      StCardEmulationEmbedded.mEeDataEvent.notifyOne();
    } break;

    case NFA_EE_DISCONNECT_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_EE_DISCONNECT_EVT; handle=0x%02X", fn,
                          eventData->data.handle);
      // mEeDisconnEvent
      SyncEventGuard guard(StCardEmulationEmbedded.mEeDisconnEvent);
      StCardEmulationEmbedded.mEeDisconnEvent.notifyOne();
    } break;

    case NFA_EE_SET_TECH_CFG_EVT:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_SET_TECH_CFG_EVT; status=0x%X", fn, eventData->status);
      break;
    case NFA_EE_SET_PROTO_CFG_EVT:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_SET_PROTO_CFG_EVT; status=0x%X", fn, eventData->status);
      break;
    case NFA_EE_UPDATED_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_EE_UPDATED_EVT", fn);
      break;
    }
    case NFA_EE_REMOVE_AID_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_REMOVE_AID_EVT  status=%u", fn, eventData->status);
    } break;

      break;

      // Events not processed by this object
    case NFA_EE_ADD_SYSCODE_EVT:
    case NFA_EE_POWER_CTRL_EVT:
    case NFA_EE_STATUS_NTF_EVT:
    case NFA_EE_DEREGISTER_EVT:
    case NFA_EE_CLEAR_PROTO_CFG_EVT:
    case NFA_EE_ADD_AID_EVT:
    case NFA_EE_ACTION_EVT:
      break;

    default:
      LOG(ERROR) << StringPrintf("%s: unknown event=%u ????", fn, event);
      break;
  }
}

/*******************************************************************************
 **
 ** Function:        enable
 **
 ** Description:     Release all resources.
 **
 ** Returns:         None
 **
 *******************************************************************************/
bool StCardEmulationEmbedded::enable(uint8_t ceeId, bool enable) {
  static const char fn[] = "StCardEmulationEmbedded::enable";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", fn);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  uint8_t numEE = MAX_NUM_EE;
  int i;
  tNFA_EE_INFO& eeItem = mEeInfo[0];
  bool found_ee = false;
  bool status = false;
  /* NCI 2.0 - Begin */
  uint8_t NdefAid[7] = {0xd2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};
  /* NCI 2.0 - End */

  /* NCI 2.0 - Begin */
  if (enable == true) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: enter; Enabling CEE 0x%x", fn, ceeId);
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: enter; Disabling CEE 0x%x", fn, ceeId);
  }
  /* NCI 2.0 - End */

  // Check if requested CEE was reported by CLF
  if ((nfaStat = NFA_EeGetInfo(&numEE, mEeInfo)) != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: fail get info; error=0x%X", fn, nfaStat);
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: found 0x%x EE", fn, numEE);

    for (i = 0; i < numEE; i++) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: mEeInfo[i].ee_handle = 0x%x;", fn, mEeInfo[i].ee_handle);
      if (mEeInfo[i].ee_handle == (ceeId | NFA_HANDLE_GROUP_EE)) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: found matching entry for CEE ID 0x%x", fn, ceeId);
        // Requested NFCEE Id was reported by CLF
        eeItem = mEeInfo[i];
        found_ee = true;
        break;
      }
    }

    if (found_ee) {  // found
      // Enable NFCEE if not yet enabled
      /* NCI 2.0 - Begin */
      if (((eeItem.ee_status != NFA_EE_STATUS_ACTIVE) && (enable == true)) ||
          ((eeItem.ee_status == NFA_EE_STATUS_ACTIVE) && (enable == false))) {
        // Updating routing table without NDEF AID: needs to be done while
        // NDEF-NFCEE still active
        /* NCI 2.0 - Begin */
        if (enable == false) {
          StRoutingManager::getInstance().removeAidRouting(NdefAid,
                                                           sizeof(NdefAid));
          StRoutingManager::getInstance().commitRouting();
        }
        /* NCI 2.0 - End */

        {
          SyncEventGuard guard(mEeSetModeEvent);
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: set EE mode activate; h=0x%X", fn, eeItem.ee_handle);
          if ((nfaStat = NFA_EeModeSet(eeItem.ee_handle, enable)) ==
              NFA_STATUS_OK) {
            mEeSetModeEvent.wait();  // wait for NFA_EE_MODE_SET_EVT
          } else {
            LOG(ERROR) << StringPrintf("%s: NFA_EeModeSet failed; error=0x%X",
                                       fn, nfaStat);
            goto TheEnd;
          }
          /* NCI 2.0 - End */
        }

        /* NCI 2.0 - Begin */
        // Now wait NFCEE_MODE_SET_NTF
        {
          SyncEventGuard guard(mEeSetModeNtfEvent);
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: Waiting for NFCEE_MODE_SET_NTF; h=0x%X", fn,
                              eeItem.ee_handle);

          if (mEeSetModeNtfEvent.wait(500) ==
              false) {  // wait for NFA_EE_MODE_SET_EVT
            DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
                "%s: timeout waiting for NFCEE_MODE_SET_NTF", fn);
            goto TheEnd;
          }
        }
        /* NCI 2.0 - End */

        if (mNfaEECbStatus != NFA_STATUS_OK) {
          LOG(ERROR) << StringPrintf(
              "%s: Error in NCI_NFCEE_MODE_SET_RSP status (0x%x) - aborting "
              "procedure",
              fn, mNfaEECbStatus);
          goto TheEnd;
        } else {
          status = true;
        }

        /* NCI 2.0 - Begin */
        // Updating routing table with NDEF AID: Needs to be done when
        // NDEF-NFCEE is active
        if (enable == true) {
          StRoutingManager::getInstance().addAidRouting(
              NdefAid, sizeof(NdefAid), 0x10, 0x00);
          StRoutingManager::getInstance().commitRouting();
        }
        /* NCI 2.0 - End */
      } else {
        status = true;  // already activated or deactivated
      }
    }
  }

TheEnd:

  return status;
}

/*******************************************************************************
 **
 ** Function:        connect
 **
 ** Description:     Release all resources.
 **
 ** Returns:         None
 **
 *******************************************************************************/
bool StCardEmulationEmbedded::connect(uint8_t ceeId) {
  static const char fn[] = "StCardEmulationEmbedded::connect";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", fn);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  uint8_t numEE;
  int i;
  tNFA_EE_INFO& eeItem = mEeInfo[0];
  bool found_ee = false;
  bool status = false;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; requesting to connect CEE 0x%x", fn, ceeId);

  // Check if requested CEE was reported by CLF
  if ((nfaStat = NFA_EeGetInfo(&numEE, mEeInfo)) != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: fail get info; error=0x%X", fn, nfaStat);
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: found 0x%x EE", fn, numEE);

    for (i = 0; i < numEE; i++) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: mEeInfo[i].ee_handle = 0x%x;", fn, mEeInfo[i].ee_handle);
      if (mEeInfo[i].ee_handle == (ceeId | NFA_HANDLE_GROUP_EE)) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: found matching entry for CEE ID 0x%x", fn, ceeId);
        // Requested NFCEE Id was reported by CLF
        eeItem = mEeInfo[i];
        found_ee = true;
        break;
      }
    }

    if (found_ee) {  // found
      // Check if in RF state idle
      if (android::isDiscoveryStarted()) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: RF state machine is currently in discovery mode - "
            "Deactivating",
            fn);
        gIsReconfiguringDiscovery.start();
        android::startRfDiscovery(false);
        mNeedRfRestart = true;
      }

      // Check if no already one CEE app connected
      if (mIdConnectCee != CEE_NO_APP) {
        LOG(ERROR) << StringPrintf("%s: Already one CEE connect: 0x%x", fn,
                                   mIdConnectCee);
        goto TheEnd;  // error
      }

      // Enable NFCEE if not yet enabled
      if (eeItem.ee_status == NFA_EE_STATUS_INACTIVE) {
        SyncEventGuard guard(mEeSetModeEvent);
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: set EE mode activate; h=0x%X", fn, eeItem.ee_handle);
        if ((nfaStat = NFA_EeModeSet(eeItem.ee_handle, true)) ==
            NFA_STATUS_OK) {
          mEeSetModeEvent.wait();  // wait for NFA_EE_MODE_SET_EVT
        } else {
          LOG(ERROR) << StringPrintf("%s: NFA_EeModeSet failed; error=0x%X", fn,
                                     nfaStat);
          goto TheEnd;
        }

        if (mNfaEECbStatus != NFA_STATUS_OK) {
          LOG(ERROR) << StringPrintf(
              "%s: Error in NCI_NFCEE_MODE_SET_RSP status (0x%x) - aborting "
              "procedure",
              fn, mNfaEECbStatus);
          goto TheEnd;
        }
      }

      // Create logical connection to NFCEE
      {
        SyncEventGuard guard(mEeCreateConnEvent);
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: set EE mode activate; h=0x%X", fn, eeItem.ee_handle);
        if ((nfaStat = NFA_EeConnect(eeItem.ee_handle, eeItem.ee_interface[0],
                                     nfaEeCallback)) == NFA_STATUS_OK) {
          mEeCreateConnEvent.wait();  // wait for NFA_EE_MODE_SET_EVT
        } else {
          LOG(ERROR) << StringPrintf("%s: NFA_EeModeSet failed; error=0x%X", fn,
                                     nfaStat);
          goto TheEnd;
        }

        if (mNfaEECbStatus != NFA_STATUS_OK) {
          LOG(ERROR) << StringPrintf(
              "%s: Error in NCI_CORE_CONN_CREATE_RSP status (0x%x) - aborting "
              "procedure",
              fn, mNfaEECbStatus);
          goto TheEnd;
        }
      }

      if (mIdConnectCee != ceeId) {
        goto TheEnd;  // error
      }
    } else {
      goto TheEnd;  // error
    }
  }

  status = true;

TheEnd:
  if ((status == false) && (mNeedRfRestart)) {
    android::startRfDiscovery(true);
    mNeedRfRestart = false;
    gIsReconfiguringDiscovery.end();
  }

  return status;
}
/*******************************************************************************
**
** Function:        transceive
**
** Description:     Send data to the NFCEE.
**
** Returns:         True if ok.
**
*******************************************************************************/
bool StCardEmulationEmbedded::transceive(uint8_t ceeId, uint16_t tx_data_len,
                                         uint8_t* tx_data,
                                         uint16_t& rx_data_size,
                                         uint8_t* rx_data) {
  static const char fn[] = "StCardEmulationEmbedded::transceive";
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter; NFCEE ID=%d; data len=%d;", fn, ceeId, tx_data_len);

  if (ceeId != mIdConnectCee) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: enter; Requested NFCEE Id is not connected", fn);
    return false;
  }

  nfaStat = NFA_EeSendData(ceeId, tx_data_len, tx_data);

  if (nfaStat != NFA_STATUS_OK) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: enter; Error when calling NFA_EeSendData()", fn);
    return false;
  }

  // Wait for response, when receiving evt  NFA_EE_DATA_EVT
  SyncEventGuard guard(mEeDataEvent);
  if (mEeDataEvent.wait(500) == false) {  // if timeout occurred
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: timeout waiting for answer to NFCEE data", fn);
    return false;
  }

  // Copy received data to buffer
  memcpy(rx_data, mResponseData, mResponseDataLength);

  rx_data_size = mResponseDataLength;

  return true;
}

/*******************************************************************************
 **
 ** Function:        disconnect
 **
 ** Description:     Release all resources.
 **
 ** Returns:         None
 **
 *******************************************************************************/
bool StCardEmulationEmbedded::disconnect(uint8_t ceeId) {
  static const char fn[] = "StCardEmulationEmbedded::disconnect";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", fn);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  tNFA_HANDLE eeHandle;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", fn);

  if (ceeId != mIdConnectCee) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: enter; Requested NFCEE Id is already disconnected", fn);
    return false;
  }

  eeHandle = ceeId | NFA_HANDLE_GROUP_EE;

  // Close the connection
  {
    SyncEventGuard guard(mEeDisconnEvent);
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: set EE mode activate; h=0x%X", fn, eeHandle);
    if ((nfaStat = NFA_EeDisconnect(eeHandle)) == NFA_STATUS_OK) {
      mEeDisconnEvent.wait();  // wait for NFA_EE_DISCONNECT_EVT
    } else {
      LOG(ERROR) << StringPrintf("%s: NFA_EeDisconnect failed; error=0x%X", fn,
                                 nfaStat);
      return false;
    }
  }
  // Disable the NFCEE
  {
    SyncEventGuard guard(mEeSetModeEvent);
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: set EE mode deactivate; h=0x%X", fn, ceeId);
    if ((nfaStat = NFA_EeModeSet((eeHandle), false)) == NFA_STATUS_OK) {
      mEeSetModeEvent.wait();  // wait for NFA_EE_MODE_SET_EVT
    } else {
      LOG(ERROR) << StringPrintf("%s: NFA_EeModeSet failed; error=0x%X", fn,
                                 nfaStat);
      return false;
    }
  }

  // retore the polling loop if needed
  if (mNeedRfRestart) {
    android::startRfDiscovery(true);
    mNeedRfRestart = false;
    gIsReconfiguringDiscovery.end();
  }

  mIdConnectCee = CEE_NO_APP;

  return true;
}
