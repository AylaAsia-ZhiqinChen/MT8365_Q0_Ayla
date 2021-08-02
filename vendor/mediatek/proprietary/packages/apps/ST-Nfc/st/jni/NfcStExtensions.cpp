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
#include <nativehelper/ScopedLocalRef.h>
#include <nativehelper/ScopedPrimitiveArray.h>
#include <cutils/properties.h>

#include "JavaClassConstants.h"

#include "NfcStExtensions.h"
#include "StSecureElement.h"
#include "StRoutingManager.h"
#include "PeerToPeer.h"
#include "StCardEmulationEmbedded.h"
#include "NfcAdaptation.h"
#include "nfc_config.h"
#include "StNfcJni.h"
#include <stdint.h>

/*****************************************************************************
 **
 ** public variables
 **
 *****************************************************************************/
using android::base::StringPrintf;
extern bool nfc_debug_enabled;
extern SyncEvent gIsReconfiguringDiscovery;

#define DEFAULT_TECH_MASK                                                  \
  (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B | NFA_TECHNOLOGY_MASK_F | \
   NFA_TECHNOLOGY_MASK_V | NFA_TECHNOLOGY_MASK_ACTIVE |                    \
   NFA_TECHNOLOGY_MASK_KOVIO)

namespace android {
extern void startRfDiscovery(bool isStart);
extern void pollingChanged(int discoveryEnabled, int pollingEnabled,
                           int p2pEnabled);
/* NCI 2.0 - Begin */
extern bool isDiscoveryStarted();
/* NCI 2.0 - End */
}  // namespace android

//////////////////////////////////////////////
//////////////////////////////////////////////

NfcStExtensions NfcStExtensions::sStExtensions;
const char* NfcStExtensions::APP_NAME = "nfc_st_ext";

int NfcStExtensions::sRfDynParamSet;

/*******************************************************************************
 **
 ** Function:        NfcStExtensions
 **
 ** Description:     Initialize member variables.
 **
 ** Returns:         None
 **
 *******************************************************************************/
NfcStExtensions::NfcStExtensions() {
  mIdMgmtInfo.added = false;
  mIdMgmtInfo.created = false;
  mIdMgmtInfo.opened = false;
  mNfaStExtHciHandle = NFA_HANDLE_INVALID;
  memset(&mPipesInfo, 0, sizeof(mPipesInfo));
  mWaitingForDmEvent = false;
  mRfConfig.modeBitmap = 0;
  memset(mRfConfig.techArray, 0, sizeof(mRfConfig.techArray));
  //  mDefaultOffHostRoute = UICC_HOST_ID;
  mDefaultIsoTechRoute = NfcConfig::getUnsigned(NAME_DEFAULT_ROUTE, 0x02);
  mIsP2pPaused = false;
  mCreatedPipeId = 0xFF;
  /* NCI 2.0 - Begin */
  memset(&mCustomerData, 0, sizeof(mCustomerData));
  /* NCI 2.0 - End */
}

/*******************************************************************************
 **
 ** Function:        ~NfcStExtensions
 **
 ** Description:     Release all resources.
 **
 ** Returns:         None
 **
 *******************************************************************************/
NfcStExtensions::~NfcStExtensions() {}

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
void NfcStExtensions::initialize(nfc_jni_native_data* native) {
  static const char fn[] = "NfcStExtensions::initialize";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", fn);

  unsigned long num;
  tNFA_STATUS nfaStat;
  mNativeData = native;

  mIsWaitingEvent.crcInfo = false;
  mIsWaitingEvent.pipeInfo = false;
  mIsWaitingEvent.pipeList = false;
  mIsWaitingEvent.vdcMeasRslt = false;

  mRfConfig.modeBitmap = 0;

  mIsP2pPaused = false;
  sRfDynParamSet = 0;

  ///////////////////////////////////////////////////////////
  // Reading all tech configurations from configuration file
  //////////////////////////////////////////////////////////

  memset(mRfConfig.techArray, 0, sizeof(mRfConfig.techArray));

  //////////////
  // Reader mode
  //////////////

  num = NfcConfig::getUnsigned(NAME_POLLING_TECH_MASK, DEFAULT_TECH_MASK);

  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: tag polling tech mask=0x%lX", fn, num);

  if (num) {  // Poll mode
    /* NCI 2.0 - Begin */
    mRfConfig.modeBitmap |= (0x1 << READER_IDX);
    /* NCI 2.0 - End */
    mRfConfig.techArray[READER_IDX] = num;

    // P2P poll mode
    /* NCI 2.0 - Begin */
    if ((num & (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_F |
                NFA_TECHNOLOGY_MASK_ACTIVE)) !=
        0) {  // Check if some tech for p2p may be here
      mRfConfig.modeBitmap |= (0x1 << P2P_POLL_IDX);
      /* NCI 2.0 - End */
      mRfConfig.techArray[P2P_POLL_IDX] = num & 0xC5;
    }
  }

  //////////////
  // Listen mode
  //////////////

  // UICC tech?
  //  num = NfcConfig::getUnsigned("UICC_LISTEN_TECH_MASK", 0x00);
  //  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
  //          "%s: Requested UICC listen mask: 0x%lX", fn, num);
  //
  //  if (num != 0) {
  /* NCI 2.0 - Begin */
  mRfConfig.modeBitmap |= (0x1 << CE_IDX);
  /* NCI 2.0 - End */
  mRfConfig.techArray[CE_IDX] = HCE_TECH_MASK;  // Allow tech A/B
  //  }

  // HCE CE type?
  //  if (mHceCapable) { // CE mode
  //    /* NCI 2.0 - Begin */
  //    mRfConfig.modeBitmap |= (0x1 << CE_IDX);
  //    mRfConfig.techArray[CE_IDX] |= StRoutingManager::getInstance()
  //            .getHceIsoDepMask(); //LISTEN_A and LISTEN_B
  //    /* NCI 2.0 - End */
  //  }

  /////////////
  // P2P listen
  /////////////
  num = NfcConfig::getUnsigned("P2P_LISTEN_TECH_MASK", 0x6F);
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: tag polling tech mask=0x%lX", fn, num);

  if (num) {  // P2P listen mode
    /* NCI 2.0 - Begin */
    mRfConfig.modeBitmap |= (0x1 << P2P_LISTEN_IDX);
    /* NCI 2.0 - End */
    mRfConfig.techArray[P2P_LISTEN_IDX] = num;
  }

  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: mRfConfig.techArray[READER_IDX] = 0x%X", fn,
                      mRfConfig.techArray[READER_IDX]);
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: mRfConfig.techArray[CE_IDX] = 0x%X", fn,
                      mRfConfig.techArray[CE_IDX]);
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: mRfConfig.techArray[P2P_LISTEN_IDX] = 0x%X", fn,
                      mRfConfig.techArray[P2P_LISTEN_IDX]);
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: mRfConfig.techArray[P2P_POLL_IDX] = 0x%X", fn,
                      mRfConfig.techArray[P2P_POLL_IDX]);
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: mRfConfig.modeBitmap = 0x%X", fn, mRfConfig.modeBitmap);

  mVdcMeasConfig.isRfFieldOn = false;

  // Reading trace configuration
  //	num = NfcConfig::getUnsigned("P2P_LISTEN_TECH_MASK", 0x6F);
  //    if (GetNumValue(NAME_APPL_TRACE_LEVEL, &num, sizeof(num))) {
  //        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Core stack
  //        trace level=0x%X", fn, num);
  //    } else {
  //        num = 0;
  //        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Core stack
  //        trace level could not read=0x%X", fn, num);
  //    }
  //
  //    if (num) {
  //        mTraceConfig.coreStack = num;
  //        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Stored Core
  //        stack trace level: %lu", fn, mTraceConfig.coreStack);
  //    }
  //
  //    if (GetNumValue("PROTOCOL_TRACE_LEVEL", &num, sizeof(num))) {
  //        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: HAL trace
  //        level=0x%X", fn, num);
  //    } else {
  //        num = 0;
  //        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: HAL trace level
  //        could not read=0x%X", fn, num);
  //    }
  //
  //    if (num) {
  //        mTraceConfig.hal = num;
  //        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Stored HAL
  //        trace level: %lu", fn, mTraceConfig.hal);
  //    }

  // Register NfcStExtensions for HCI callbacks
  SyncEventGuard guard(mNfaHciRegisterEvent);

  nfaStat = NFA_HciRegister(const_cast<char*>(APP_NAME), nfaHciCallback, TRUE);
  if (nfaStat != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: fail hci register; error=0x%X", fn,
                               nfaStat);
    return;
  }
  mNfaHciRegisterEvent.wait();

  getFwInfo();

  num = NfcConfig::getUnsigned("CE_ON_SCREEN_OFF_STATE", 0x00);
  mDesiredScreenOffPowerState = (uint8_t)num;

  // uint32_t value = getNfcSystemProp("nfc.st.ce_on_screen_off");
  // if (value != DUMMY_SYTEM_PROP_VALUE) {
  //  mDesiredScreenOffPowerState = value;
  //}

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: Selected Screen Off state 0x%X", fn, mDesiredScreenOffPowerState);

  num = NfcConfig::getUnsigned("CE_ON_SWITCH_OFF_STATE", 0x00);
  mCeOnSwitchOffState = (uint8_t)num;

  // value = getNfcSystemProp("nfc.st.ce_on_Switch_off");
  // if (value != DUMMY_SYTEM_PROP_VALUE) {
  //  mCeOnSwitchOffState = value;

  //  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
  //          "%s: user selected Switch Off state 0x%X", fn,
  //          mCeOnSwitchOffState);
  //}
  updateSwitchOffMode();

  mDynEnabled =
      (NfcConfig::getUnsigned("RF_PARAMS_AUTO_SWITCH", 0) == 1) ? true : false;
  mDynErrThreshold =
      NfcConfig::getUnsigned("RF_PARAMS_AUTO_SWITCH_ERR_THRESHOLD", 3);
  mDynT1Threshold =
      NfcConfig::getUnsigned("RF_PARAMS_AUTO_SWITCH_T1_THRESHOLD", 800);
  mDynT2Threshold =
      NfcConfig::getUnsigned("RF_PARAMS_AUTO_SWITCH_T2_THRESHOLD", 500);
  {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: try register VS callback", fn);
    nfaStat = NFA_RegVSCback(true, StVsLogsCallback);
    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail to register; error=0x%X", fn,
                                 nfaStat);
      return;
    }
  }

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
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
void NfcStExtensions::finalize() {
  mIdMgmtInfo.added = false;
  mIdMgmtInfo.created = false;
  mIdMgmtInfo.opened = false;

  {
    tNFA_STATUS nfaStat;
    DLOG_IF(INFO, nfc_debug_enabled) << "try unregister VS callback";
    nfaStat = NFA_RegVSCback(false, StVsLogsCallback);
    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << "fail to register; error=" << nfaStat;
      return;
    }
  }

  mNfaStExtHciHandle = NFA_HANDLE_INVALID;
}

/*******************************************************************************
 **
 ** Function:        abortWaits
 **
 ** Description:     Release all resources.
 **
 ** Returns:         None
 **
 *******************************************************************************/
void NfcStExtensions::abortWaits() {
  static const char fn[] = "NfcStExtensions::abortWaits";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", fn);
  {
    SyncEventGuard g(mNfaHciCreatePipeEvent);
    mNfaHciCreatePipeEvent.notifyOne();
  }
  {
    SyncEventGuard g(mNfaHciOpenPipeEvent);
    mNfaHciOpenPipeEvent.notifyOne();
  }
  {
    SyncEventGuard g(mNfaHciGetRegRspEvent);
    mNfaHciGetRegRspEvent.notifyOne();
  }
  {
    SyncEventGuard g(mHciRspRcvdEvent);
    mHciRspRcvdEvent.notifyOne();
  }
  {
    SyncEventGuard g(mNfaHciEventRcvdEvent);
    mNfaHciEventRcvdEvent.notifyOne();
  }
}

/*******************************************************************************
 **
 ** Function:        getHostName
 **
 ** Description:     Asks for host list on Admin gate (HCI commands)
 **
 ** Returns:         None.
 **
 *******************************************************************************/
/* NCI 2.0 - Begin */
static const char* getHostName(int host_id) {
  static const char fn[] = "getHostName";
  static const char* host = "Unknown Host";

  switch (host_id) {
    case 0:
      host = "Device Host";
      break;
    case 1:
      host = "UICC";
      break;
    case 2:
      host = "eSE";
      break;
    default:
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Invalid host id!!!", fn);
      break;
  }

  return host;
}
/* NCI 2.0 - End */

/*******************************************************************************
 **
 ** Function:        getPipesInfo
 **
 ** Description:     Asks for host list on Admin gate (HCI commands)
 **
 ** Returns:         None.
 **
 *******************************************************************************/
/* NCI 2.0 - Begin */ bool NfcStExtensions::getPipesInfo() {
  static const char fn[] = "NfcStExtensions::getPipesInfo";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  int i, idx, j, dhIdx;
  bool infoOk = false;
  static const char* host;

  // Need to update HCI host list
  StSecureElement::getInstance().getHostList();

  // Retrieve info for UICC and eSE regardless of input param
  // Rebuild info for all hosts all the time
  for (idx = 1; idx < 3; idx++) {
    mTargetHostId = idx;
    mPipesInfo[idx].nb_pipes = 0xFF;

    host = getHostName(idx);
    // Call only if host is connected
    if (StSecureElement::getInstance().isSEConnected(
            (idx == 1 ? 0x02 : 0xc0)) == true) {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Requesting info for host %s", fn, host);

      mPipesInfo[idx].nb_info_rx = 0;
      uint8_t attr = 8 | 2;
      if (idx == 1) {
        uint8_t nfceeId = StSecureElement::getInstance().getSENfceeId(0x02);

        LOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: Active UICC is at slot 0x%02X", fn, nfceeId);

        // we query the active UICC
        switch (nfceeId) {
          case 0x81:
            attr = 8 | 2;
            break;
          case 0x83:
            attr = 8 | 3;
            break;
          case 0x85:
            attr = 8 | 4;
            break;
        }
      } else {
        // we query the active eSE
        uint8_t nfceeId = StSecureElement::getInstance().getSENfceeId(0x01);

        LOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: Active SE is at slot 0x%02X", fn, nfceeId);

        switch (nfceeId) {
          case 0x82:  // eSE
          case 0x86:  // eUICC-SE
            attr = 8 | 3;
            break;
          case 0x84:  // DHSE
            attr = 8 | 5;
            break;
        }
      }
      uint8_t mActionRequestParam[] = {0x03, attr, 0x82, 0x1, 0x1};
      mIsWaitingEvent.pipeInfo = true;
      SyncEventGuard guard(mVsActionRequestEvent);

      nfaStat = NFA_SendVsCommand(OID_ST_VS_CMD, 5, mActionRequestParam,
                                  nfaVsCbActionRequest);
      if (nfaStat != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf(
            "%s: NFA_SendVsCommand() call failed; error=0x%X", fn, nfaStat);
      } else {
        mVsActionRequestEvent.wait();
        mIsWaitingEvent.pipeInfo = false;
      }

      infoOk = true;
    } else {
      mPipesInfo[idx].nb_pipes = 0;
    }
  }

  // Pipe info for UICC and eSE have been retrieved
  // Rebuild pipe info for DH
  mPipesInfo[0].nb_pipes = 0;
  for (i = 0; i < 2; i++) {
    for (j = 0; j < mPipesInfo[i + 1].nb_pipes; j++) {
      if (mPipesInfo[i + 1].data[j].dest_host == 0x01) {
        dhIdx = mPipesInfo[0].nb_pipes;
        mPipesInfo[0].data[dhIdx].dest_gate =
            mPipesInfo[i + 1].data[j].dest_gate;
        mPipesInfo[0].data[dhIdx].dest_host =
            mPipesInfo[i + 1].data[j].dest_host;
        mPipesInfo[0].data[dhIdx].pipe_id = mPipesInfo[i + 1].data[j].pipe_id;
        mPipesInfo[0].data[dhIdx].pipe_state =
            mPipesInfo[i + 1].data[j].pipe_state;
        mPipesInfo[0].data[dhIdx].source_gate =
            mPipesInfo[i + 1].data[j].source_gate;
        mPipesInfo[0].data[dhIdx].source_host =
            mPipesInfo[i + 1].data[j].source_host;
        mPipesInfo[0].nb_pipes++;
      }
    }
  }

  // Debug, display results
  for (i = 0; i < 3; i++) {
    host = getHostName(i);
    LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: Found %d pipes for %s", fn, mPipesInfo[i].nb_pipes, host);
    for (j = 0; j < mPipesInfo[i].nb_pipes; j++) {
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: Info for pipe 0x%x: source host is 0x%x, destination host is "
          "0x%x, gate is 0x%x, state is 0x%x",
          fn, mPipesInfo[i].data[j].pipe_id, mPipesInfo[i].data[j].source_host,
          mPipesInfo[i].data[j].dest_host, mPipesInfo[i].data[j].source_gate,
          mPipesInfo[i].data[j].pipe_state);
    }
  }

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
  return infoOk;
}
/* NCI 2.0 - End */
/* NCI 2.0 - Begin */
/*******************************************************************************
 **
 ** Function:        callbackVsActionRequest
 **
 ** Description:     Callback for NCI vendor specific cmd.
 **
 ** Returns:         None
 **
 *******************************************************************************/
void NfcStExtensions::nfaVsCbActionRequest(uint8_t oid, uint16_t len,
                                           uint8_t* p_msg) {
  static const char fn[] = "NfcStExtensions::nfaVsCbActionRequest";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: nfaVsCbActionRequest; oid=0x%X; len = %d", fn, oid, len);

  if (sStExtensions.mIsWaitingEvent.pipeInfo == true) {  // Pipes Info
    if (len == 0) {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: PROP_CMD_RSP; No data returned !!!!", fn);
    } else {
      int i = 0, nb_pipes = 0;
      int hostIdx = sStExtensions.mTargetHostId;

      while (i < 16) {
        if (p_msg[12 * i + 12] != 0) {
          sStExtensions.mPipesInfo[hostIdx].data[nb_pipes].source_host =
              p_msg[12 * i + 7];
          sStExtensions.mPipesInfo[hostIdx].data[nb_pipes].source_gate =
              p_msg[12 * i + 8];
          sStExtensions.mPipesInfo[hostIdx].data[nb_pipes].dest_host =
              p_msg[12 * i + 9];
          sStExtensions.mPipesInfo[hostIdx].data[nb_pipes].dest_gate =
              p_msg[12 * i + 10];
          sStExtensions.mPipesInfo[hostIdx].data[nb_pipes].pipe_id =
              p_msg[12 * i + 11];
          sStExtensions.mPipesInfo[hostIdx].data[nb_pipes].pipe_state =
              p_msg[12 * i + 12];

          sStExtensions.mPipesInfo[hostIdx].nb_info_rx++;
          nb_pipes++;
        }

        i++;
      }

      sStExtensions.mPipesInfo[sStExtensions.mTargetHostId].nb_pipes = nb_pipes;
    }
  } else if (sStExtensions.mIsWaitingEvent.getPropConfig == true) {
    if (p_msg[3] == NFA_STATUS_OK) {
      sStExtensions.mPropConfigLen = p_msg[6];

      for (int i = 0; i < len - 7; i++) {
        sStExtensions.mPropConfig.config[i] = p_msg[7 + i];
      }
    } else {
      sStExtensions.mPropConfigLen = 0;
    }
  } else if (sStExtensions.mIsWaitingEvent.sendPropTestCmd == true) {
    if (p_msg[3] == NFA_STATUS_OK) {
      sStExtensions.mPropTestRspLen = p_msg[2] - 1;  // Payload minus status

      // Alocate needed memory if needed (actual data received)
      if (sStExtensions.mPropTestRspLen) {
        sStExtensions.mPropTestRspPtr =
            (uint8_t*)GKI_os_malloc(sStExtensions.mPropTestRspLen);
        memcpy(sStExtensions.mPropTestRspPtr, &(p_msg[4]),
               sStExtensions.mPropTestRspLen);
      }
    } else {
      sStExtensions.mPropTestRspLen = 0;
    }
  }
  sStExtensions.mVsActionRequestEvent.start();
  sStExtensions.mVsActionRequestEvent.notifyOne();
  sStExtensions.mVsActionRequestEvent.end();
}
/* NCI 2.0 - End */

/*******************************************************************************
 **
 ** Function:        getPipeState
 **
 ** Description:     Asks for pipe Id state (HCI commands)
 **
 ** Returns:         None.
 **
 *******************************************************************************/
uint8_t NfcStExtensions::getPipeState(uint8_t pipe_id) {
  static const char fn[] = "NfcStExtensions::getPipeState";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Requested state of pipe 0x%x", fn, pipe_id);
  int i;

  for (i = 0; i < mPipesInfo[DH_IDX].nb_pipes; i++) {
    if (mPipesInfo[DH_IDX].data[i].pipe_id == pipe_id) {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: State of pipe 0x%x is 0x%x", fn, pipe_id,
                          mPipesInfo[DH_IDX].data[i].pipe_state);
      return mPipesInfo[DH_IDX].data[i].pipe_state;
    }
  }

  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Pipe not found in stored data - Not created", fn);
  return 0;
}

/*******************************************************************************
 **
 ** Function:        getPipeIdForGate
 **
 ** Description:     Asks for pipe Id for a given gate Id (HCI commands)
 **
 ** Returns:         None.
 **
 *******************************************************************************/
uint8_t NfcStExtensions::getPipeIdForGate(uint8_t host_id, uint8_t gate_id) {
  static const char fn[] = "NfcStExtensions::getPipeIdForGate";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Requesting pipe id for gate 0x%x on host 0x%x", fn,
                      gate_id, host_id);
  /* NCI 2.0 - Begin */
  int i, idx = 0;
  /* NCI 2.0 - End */

  switch (host_id) {
    case DH_HCI_ID:
      idx = DH_IDX;
      break;
    case ESE_HOST_ID:
      idx = ESE_IDX;
      break;
    case UICC_HOST_ID:
      idx = UICC_IDX;
      break;
  }

  for (i = 0; i < mPipesInfo[idx].nb_pipes; i++) {
    if (mPipesInfo[idx].data[i].source_gate == gate_id) {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Pipe 0x%x belongs to gate 0x%x", fn,
                          mPipesInfo[idx].data[i].pipe_id, gate_id);
      return mPipesInfo[idx].data[i].pipe_id;
    }
  }

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: Gate not found in stored data - No pipe created on that gate", fn);
  return 0xFF;  // Invalid pipe Id
}

/*******************************************************************************
 **
 ** Function:        getHostIdForPipe
 **
 ** Description:     Asks for host Id to which given pipe Id pertains.
 **
 ** Returns:         None.
 **
 *******************************************************************************/
uint8_t NfcStExtensions::getHostIdForPipe(uint8_t pipe_id) {
  static const char fn[] = "NfcStExtensions::getHostIdForPipe";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Requesting Host id for pipe 0x%x", fn, pipe_id);
  int i;

  for (i = 0; i < mPipesInfo[DH_IDX].nb_pipes; i++) {
    if (mPipesInfo[DH_IDX].data[i].pipe_id == pipe_id) {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Pipe 0x%x belongs to host 0x%x", fn, pipe_id,
                          mPipesInfo[DH_IDX].data[i].source_host);
      return mPipesInfo[DH_IDX].data[i].source_host;
    }
  }

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: Host not found in stored data - Pipe not created", fn);
  return 0xFF;  // Invalid host Id
}

/*******************************************************************************
 **
 ** Function:        checkGateForHostId
 **
 ** Description:     Checks if a pipe exists for the given gate and host.
 **                  If yes it returns the pipe_id, if not 0xFF.
 **
 ** Returns:         None.
 **
 *******************************************************************************/
uint8_t NfcStExtensions::checkGateForHostId(uint8_t gate_id, uint8_t host_id) {
  static const char fn[] = "NfcStExtensions::checkGateForHostId";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  int i;

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: Checking if gate 0x%x exists between DH and host 0x%x", fn, gate_id,
      host_id);
  if (host_id == DH_ID) {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Requested host shall not be DH ", fn);
    return 0xFF;
  }

  for (i = 0; i < mPipesInfo[DH_IDX].nb_pipes; i++) {
    if ((mPipesInfo[DH_IDX].data[i].dest_gate == gate_id) &&
        ((mPipesInfo[DH_IDX].data[i].dest_host == host_id) ||
         (mPipesInfo[DH_IDX].data[i].source_host == host_id))) {
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: Found gate 0x%x between DH and host 0x%x - pipe_id = 0x%x", fn,
          gate_id, host_id, mPipesInfo[DH_IDX].data[i].pipe_id);
      return mPipesInfo[DH_IDX].data[i].pipe_id;
    }
  }
  return 0xFF;
}

/*******************************************************************************
 **
 ** Function:        updateSwitchOffMode
 **
 ** Description:     Set the CLF configuration to setup the CE on SwitchOFF if
 *needed.
 **
 ** Returns:         None.
 **
 *******************************************************************************/
/* NCI 2.0 - Begin */
void NfcStExtensions::updateSwitchOffMode() {
  static const char fn[] = "NfcStExtensions::updateSwitchOffMode";

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter mCeOnSwitchOffState = %d", fn, mCeOnSwitchOffState);

  setProprietaryConfigSettings(
      NFCC_CONFIGURATION, 0, 0,
      !mCeOnSwitchOffState);  // real byte number computed in there

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
}
/* NCI 2.0 - End */

/*******************************************************************************
 **
 ** Function:        getInstance
 **
 ** Description:     Get the StSecureElement singleton object.
 **
 ** Returns:         StSecureElement object.
 **
 *******************************************************************************/
NfcStExtensions& NfcStExtensions::getInstance() { return sStExtensions; }

/*******************************************************************************
 **
 ** Function:        prepareGate
 **
 ** Description:     Connect to the secure element.
 **                  e: JVM environment.
 **                  o: Java object.
 **
 ** Returns:         Handle of secure element.  values < 0 represent failure.
 **
 *******************************************************************************/
int NfcStExtensions::prepareGate(uint8_t gate_id) {
  static const char fn[] = "NfcStExtensions::prepareGate";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  int ret = 0;
  tJNI_ID_MGMT_INFO* gateInfo;

  if (gate_id == ID_MGMT_GATE_ID) {
    gateInfo = &mIdMgmtInfo;
  } else if (gate_id == LOOPBACK_GATE_ID) {  // Loopback gate
    gateInfo = &mLoopbackInfo;
  } else {
    return ret;
  }

  if (mNfaStExtHciHandle == NFA_HANDLE_INVALID) {
    return ret;
  }

  if (gateInfo->added == false) {
    // Add static pipe to accept notify pipe created from NFCC at eSE initial
    // activation
    nfaStat = NFA_HciAllocGate(mNfaStExtHciHandle, gate_id);

    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "%s: fail adding static pipe for Id mgmt gate; error=0x%X", fn,
          nfaStat);
      return (ret);
    }

    gateInfo->added = true;
  }

  // Create Identity mgmt pipe, if not yet done
  if (gateInfo->created == false) {
    // Get pipe info
    uint8_t pipe_id, pipe_state;

    /* NCI 2.0 - Begin */ bool status = getPipesInfo();
    /* NCI 2.0 - End */
    if (status == false) {
      return ret;
    }

    pipe_id = getPipeIdForGate(DH_ID, gate_id);

    if (pipe_id != 0xFF) {  // Pipe was found in stored data
      gateInfo->pipe_id = pipe_id;
      pipe_state = getPipeState(pipe_id);
    } else {
      pipe_state = 0;  // Not created
    }

    switch (pipe_state) {
      case 0x00:  // Not created/not opened
        break;

      case 0x02:  // created/ not opened
        gateInfo->created = true;
        break;

      case 0x06:  // Created/opened
        gateInfo->created = true;
        gateInfo->opened = true;
        break;

      default:
        LOG(ERROR) << StringPrintf("%s: Unvalid pipe state returned: 0x%x", fn,
                                   pipe_state);
        return ret;
    }

    if (gateInfo->created == false) {
      SyncEventGuard guard(mNfaHciCreatePipeEvent);
      if ((nfaStat = NFA_HciCreatePipe(mNfaStExtHciHandle, gate_id, CLF_ID,
                                       gate_id)) == NFA_STATUS_OK) {
        mNfaHciCreatePipeEvent.wait();  // wait for NFA_HCI_CREATE_PIPE_EVT
      } else {
        LOG(ERROR) << StringPrintf("%s: NFA_HciCreatePipe failed; error=0x%X",
                                   fn, nfaStat);
        return ret;
      }
    }

    // Open Identity mgmt pipe, if not yet done
    if (gateInfo->opened == false) {
      SyncEventGuard guard(mNfaHciOpenPipeEvent);
      if ((nfaStat = NFA_HciOpenPipe(mNfaStExtHciHandle, gateInfo->pipe_id)) ==
          NFA_STATUS_OK) {
        mNfaHciOpenPipeEvent.wait();  // wait for NFA_HCI_CREATE_PIPE_EVT
      } else {
        LOG(ERROR) << StringPrintf("%s: NFA_HciCreatePipe failed; error=0x%X",
                                   fn, nfaStat);
        return ret;
      }
    }
  }

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
  return 1;
}

/*******************************************************************************
 **
 ** Function:        prepareGateForTest
 **
 ** Description:     prepares a gate for communication between the DH and a
 **                  SWP host.
 **
 ** Returns:
 **
 *******************************************************************************/
int NfcStExtensions::prepareGateForTest(uint8_t gate_id, uint8_t host_id) {
  static const char fn[] = "NfcStExtensions::prepareGateForTest";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  uint8_t pipe_state;
  bool isGateSetup = false;
  bool isPipeCreated = false;
  bool isPipeOpened = false;

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: Preparing gate 0x%x for test on host 0x%x", fn, gate_id, host_id);

  if (mNfaStExtHciHandle == NFA_HANDLE_INVALID) {
    return 0xFF;
  }

  // Check if gate is listed in info retrieved from device mgmt gate (pipe info)
  /* NCI 2.0 - Begin */ bool status = getPipesInfo();
  /* NCI 2.0 - End */

  if (status == false) {
    return 0xFF;
  }

  mCreatedPipeId = checkGateForHostId(gate_id, host_id);
  if (mCreatedPipeId != 0xFF) {  // Pipe exists i.e. gate was already allocated
    isGateSetup = true;

    // Get pipe status
    pipe_state = getPipeState(mCreatedPipeId);

    switch (pipe_state) {
      case 0x00:  // Not created/not opened
        break;

      case 0x02:  // created/ not opened
        isPipeCreated = true;
        break;

      case 0x06:  // Created/opened
        isPipeCreated = true;
        isPipeOpened = true;
        break;

      default:
        LOG(ERROR) << StringPrintf("%s: Unvalid pipe state returned: 0x%x", fn,
                                   pipe_state);
        return 0xFF;
    }
  }

  if (isGateSetup == false) {
    nfaStat = NFA_HciAllocGate(mNfaStExtHciHandle, gate_id);
    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "%s: fail adding static pipe for gate 0x%x; error=0x%X", fn, nfaStat,
          gate_id);
      return (0xFF);
    }
  }

  if (isPipeCreated == false) {
    SyncEventGuard guard(mNfaHciCreatePipeEvent);
    if ((nfaStat = NFA_HciCreatePipe(mNfaStExtHciHandle, gate_id, host_id,
                                     gate_id)) == NFA_STATUS_OK) {
      mNfaHciCreatePipeEvent.wait();  // wait for NFA_HCI_CREATE_PIPE_EVT
    } else {
      LOG(ERROR) << StringPrintf("%s: NFA_HciCreatePipe failed; error=0x%X", fn,
                                 nfaStat);
      return 0xFF;
    }
  }

  if ((isPipeOpened == false) && (mCreatedPipeId != 0xFF)) {
    mIsWaitingEvent.IsTestPipeOpened = true;
    SyncEventGuard guard(mHciRspRcvdEvent);
    if ((nfaStat = NFA_HciOpenPipe(mNfaStExtHciHandle, mCreatedPipeId)) ==
        NFA_STATUS_OK) {
      mHciRspRcvdEvent.wait();  // wait for NFA_HCI_CREATE_PIPE_EVT
    } else {
      LOG(ERROR) << StringPrintf("%s: NFA_HciCreatePipe failed; error=0x%X", fn,
                                 nfaStat);
      mIsWaitingEvent.IsTestPipeOpened = false;
      return 0xFF;
    }
    mIsWaitingEvent.IsTestPipeOpened = false;
  } else {
    LOG(ERROR) << StringPrintf(
        "%s: Pipe already created and opened fpr gate 0x%x", fn, gate_id);
  }

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
  return mCreatedPipeId;
}

/*******************************************************************************
 **
 ** Function:        handleLoopback
 **
 ** Description:     Connect to the secure element.
 **                  e: JVM environment.
 **                  o: Java object.
 **
 ** Returns:         Handle of secure element.  values < 0 represent failure.
 **
 *******************************************************************************/
int NfcStExtensions::handleLoopback() {
  static const char fn[] = "NfcStExtensions::handleLoopback";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  int ret = 0, i;
  uint8_t txData[SIZE_LOOPBACK_DATA], rxData[SIZE_LOOPBACK_DATA];

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  ret = prepareGate(LOOPBACK_GATE_ID);
  if (ret == 0) {
    return 0;  // error
  }

  // Send data to CLF
  for (i = 0; i < SIZE_LOOPBACK_DATA; i++) {
    txData[i] = i;
  }

  SyncEventGuard guard(mNfaHciEventRcvdEvent);
  nfaStat = NFA_HciSendEvent(mNfaStExtHciHandle, mLoopbackInfo.pipe_id,
                             NFA_HCI_EVT_POST_DATA, SIZE_LOOPBACK_DATA, txData,
                             sizeof(rxData), rxData, 0);
  if (nfaStat == NFA_STATUS_OK) {
    mNfaHciEventRcvdEvent.wait();  // wait for NFA_HCI_CREATE_PIPE_EVT
  } else {
    LOG(ERROR) << StringPrintf("%s: NFA_HciSendEvent failed; error=0x%X", fn,
                               nfaStat);
    return ret;
  }

  // Check received data
  if (mRspSize != SIZE_LOOPBACK_DATA) {
    LOG(ERROR) << StringPrintf(
        "%s: Returned length for loopback is wrong: expected %d, received %d",
        fn, SIZE_LOOPBACK_DATA, mRspSize);
    return 0;
  }

  for (i = 0; i < SIZE_LOOPBACK_DATA; i++) {
    if (rxData[i] != txData[i]) {
      LOG(ERROR) << StringPrintf(
          "%s: Error in received data: rxData[%d] = 0x%x, txData[%d] = 0x%x",
          fn, i, rxData[i], i, txData[i]);
      return 0;
    }
  }

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
  return 1;
}

/*******************************************************************************
 **
 ** Function:        nfaHciCallback
 **
 ** Description:     Receive Host Controller Interface-related events from
 *stack.
 **                  event: Event code.
 **                  eventData: Event data.
 **
 ** Returns:         None
 **
 *******************************************************************************/
void NfcStExtensions::nfaHciCallback(tNFA_HCI_EVT event,
                                     tNFA_HCI_EVT_DATA* eventData) {
  static const char fn[] = "NfcStExtensions::nfaHciCallback";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: event=0x%X", fn, event);

  switch (event) {
    case NFA_HCI_REGISTER_EVT: {
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_REGISTER_EVT; status=0x%X; handle=0x%X", fn,
          eventData->hci_register.status, eventData->hci_register.hci_handle);
      SyncEventGuard guard(sStExtensions.mNfaHciRegisterEvent);

      sStExtensions.mNfaStExtHciHandle = eventData->hci_register.hci_handle;
      sStExtensions.mNfaHciRegisterEvent.notifyOne();
    } break;

    case NFA_HCI_CREATE_PIPE_EVT: {
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_CREATE_PIPE_EVT; status=0x%X; pipe=0x%X; src gate=0x%X; "
          "dest host=0x%X; dest gate=0x%X",
          fn, eventData->created.status, eventData->created.pipe,
          eventData->created.source_gate, eventData->created.dest_host,
          eventData->created.dest_gate);

      if (eventData->created.source_gate == ID_MGMT_GATE_ID) {
        if (eventData->created.status == NFA_STATUS_OK) {
          sStExtensions.mIdMgmtInfo.created = true;
          sStExtensions.mIdMgmtInfo.pipe_id = eventData->created.pipe;
        }
      } else if (eventData->created.source_gate == LOOPBACK_GATE_ID) {
        if (eventData->created.status == NFA_STATUS_OK) {
          sStExtensions.mLoopbackInfo.created = true;
          sStExtensions.mLoopbackInfo.pipe_id = eventData->created.pipe;
        }
      } else {
        if (eventData->created.status == NFA_STATUS_OK) {
          sStExtensions.mCreatedPipeId = eventData->created.pipe;
        }
      }

      SyncEventGuard guard(sStExtensions.mNfaHciCreatePipeEvent);
      sStExtensions.mNfaHciCreatePipeEvent.notifyOne();
    } break;

    case NFA_HCI_OPEN_PIPE_EVT: {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_HCI_OPEN_PIPE_EVT; status=0x%X; pipe=0x%X",
                          fn, eventData->opened.status, eventData->opened.pipe);

      if (eventData->opened.pipe == sStExtensions.mIdMgmtInfo.pipe_id) {
        if (eventData->opened.status == NFA_HCI_ANY_OK) {
          sStExtensions.mIdMgmtInfo.opened = true;
        }
        SyncEventGuard guard(sStExtensions.mNfaHciOpenPipeEvent);
        sStExtensions.mNfaHciOpenPipeEvent.notifyOne();
      }
    } break;

    case NFA_HCI_GET_REG_RSP_EVT: {
      int i;

      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_GET_REG_RSP_EVT; status: 0x%X; pipe: 0x%X, reg_idx: "
          "0x%X, len: %d",
          fn, eventData->registry.status, eventData->registry.pipe,
          eventData->registry.index, eventData->registry.data_len);

      if (eventData->registry.pipe == sStExtensions.mIdMgmtInfo.pipe_id) {
        if (eventData->registry.status == NFA_STATUS_OK) {
          if (eventData->registry.index == VERSION_SW_REG_IDX) {
            for (i = 0; i < eventData->registry.data_len; i++) {
              sStExtensions.mIdMgmtInfo.version_sw[i] =
                  eventData->registry.reg_data[i];
            }
          } else if (eventData->registry.index == VERSION_HW_REG_IDX) {
            for (i = 0; i < eventData->registry.data_len; i++) {
              sStExtensions.mIdMgmtInfo.version_hw[i] =
                  eventData->registry.reg_data[i];
            }
          }
        }
      }

      SyncEventGuard guard(sStExtensions.mNfaHciGetRegRspEvent);
      sStExtensions.mNfaHciGetRegRspEvent.notifyOne();
    } break;

    case NFA_HCI_SET_REG_RSP_EVT: {
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_SET_REG_RSP_EVT; status: 0x%X; pipe: 0x%X, reg_idx: "
          "0x%X, len: %d",
          fn, eventData->registry.status, eventData->registry.pipe,
          eventData->registry.index, eventData->registry.data_len);
    } break;

    case NFA_HCI_RSP_RCVD_EVT: {  // response received from secure element
      int i;
      tNFA_HCI_RSP_RCVD& rsp_rcvd = eventData->rsp_rcvd;
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_RSP_RCVD_EVT; status: 0x%X; code: 0x%X; pipe: 0x%X; "
          "len: %u",
          fn, rsp_rcvd.status, rsp_rcvd.rsp_code, rsp_rcvd.pipe,
          rsp_rcvd.rsp_len);
      if (sStExtensions.mCreatedPipeId == rsp_rcvd.pipe) {
        if (rsp_rcvd.rsp_code == NFA_HCI_ANY_OK) {
          if (sStExtensions.mIsWaitingEvent.propHciRsp ==
              true) {  // data reception
            sStExtensions.mRxHciDataLen = rsp_rcvd.rsp_len;

            for (i = 0; i < rsp_rcvd.rsp_len; i++) {
              LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
                  "%s: NFA_HCI_RSP_RCVD_EVT; sp_rcvd.rsp_data[%d] = 0x%x", fn,
                  i, rsp_rcvd.rsp_data[i]);

              sStExtensions.mRxHciData[i] = rsp_rcvd.rsp_data[i];
            }
          } else if (sStExtensions.mIsWaitingEvent.IsTestPipeOpened == true) {
            LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
                "%s: NFA_HCI_RSP_RCVD_EVT; pipe 0x%x is now opened!!", fn,
                rsp_rcvd.pipe);
          }
        }
      }

      SyncEventGuard guard(sStExtensions.mHciRspRcvdEvent);
      sStExtensions.mHciRspRcvdEvent.notifyOne();
    } break;

    case NFA_HCI_CMD_SENT_EVT: {
      tNFA_HCI_CMD_SENT& cmd_sent = eventData->cmd_sent;
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_CMD_SENT_EVT; status=0x%X;", fn, cmd_sent.status);

      if (cmd_sent.status == NFA_STATUS_FAILED) {
        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: NFA_HCI_CMD_SENT_EVT; Status Failed!!! - Aborting all waits",
            fn);
        // Abort all waits
        sStExtensions.abortWaits();
      }
    } break;

    case NFA_HCI_EVENT_SENT_EVT:
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_HCI_EVENT_SENT_EVT; status=0x%X", fn,
                          eventData->evt_sent.status);
      {
        if (eventData->evt_sent.status == NFA_STATUS_FAILED) {
          LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: NFA_HCI_CMD_SENT_EVT; Status Failed!!! - Aborting all waits",
              fn);
          // Abort all waits
          sStExtensions.abortWaits();
        }
      }
      break;

    case NFA_HCI_EVENT_RCVD_EVT: {
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_EVENT_RCVD_EVT; code: 0x%X; pipe: 0x%X; data len: %u",
          fn, eventData->rcvd_evt.evt_code, eventData->rcvd_evt.pipe,
          eventData->rcvd_evt.evt_len);

      if (eventData->rcvd_evt.evt_code == NFA_HCI_EVT_POST_DATA) {
        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: NFA_HCI_EVENT_RCVD_EVT; NFA_HCI_EVT_POST_DATA", fn);
        sStExtensions.mRspSize = eventData->rcvd_evt.evt_len;
        SyncEventGuard guard(sStExtensions.mNfaHciEventRcvdEvent);
        sStExtensions.mNfaHciEventRcvdEvent.notifyOne();
      }
    } break;

    case NFA_HCI_ALLOCATE_GATE_EVT:
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_ALLOCATE_GATE_EVT; status = %d, gate = 0x%x", fn,
          eventData->allocated.status, eventData->allocated.gate);
      break;

    case NFA_HCI_CLOSE_PIPE_EVT: {
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_CLOSE_PIPE_EVT; status = %d, pipe = 0x%x", fn,
          eventData->closed.status, eventData->closed.pipe);
      SyncEventGuard guard(sStExtensions.mNfaHciClosePipeEvent);
      sStExtensions.mNfaHciClosePipeEvent.notifyOne();
    } break;

    default:
      LOG(ERROR) << StringPrintf(
          "%s: event code=0x%X not handled by this method", fn, event);
      break;
  }
}

/*******************************************************************************
 **
 ** Function:        setCoreResetNtfInfo
 **
 ** Description:     Connect to the secure element.
 **                  e: JVM environment.
 **                  o: Java object.
 **
 ** Returns:         Handle of secure element.  values < 0 represent failure.
 **
 *******************************************************************************/
/* NCI 2.0 - Begin */
void NfcStExtensions::setCoreResetNtfInfo(uint8_t* ptr_manu_info) {
  static const char fn[] = "NfcStExtensions::setCoreResetNtfInfo";

  mFwInfo = (ptr_manu_info[2] << 24) | (ptr_manu_info[3] << 16) |
            (ptr_manu_info[4] << 8) | ptr_manu_info[5];
  mHwInfo = (ptr_manu_info[0] << 8) | ptr_manu_info[1];
  memcpy(mCustomerData, &ptr_manu_info[17], sizeof(mCustomerData));

  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s - FW Info = %08X, HW Info = %04X, CustData=%02X%02X",
                      fn, mFwInfo, mHwInfo, mCustomerData[6], mCustomerData[7]);
}
/* NCI 2.0 - End */

/*******************************************************************************
 **
 ** Function:        getFirmwareVersion
 **
 ** Description:     Connect to the secure element.
 **                  e: JVM environment.
 **                  o: Java object.
 **
 ** Returns:         Handle of secure element.  values < 0 represent failure.
 **
 *******************************************************************************/
int NfcStExtensions::getFirmwareVersion(uint8_t* fwVersion) {
  static const char fn[] = "NfcStExtensions::getFirmwareVersion";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  /* NCI 2.0 - Begin */
  int ret = 1;

  fwVersion[0] = mFwInfo >> 24;
  fwVersion[1] = mFwInfo >> 16;
  fwVersion[2] = mFwInfo >> 8;
  fwVersion[3] = mFwInfo;
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: FW Version = %02X.%02X.%02X%02X", fn, fwVersion[0],
                      fwVersion[1], fwVersion[2], fwVersion[3]);
  /* NCI 2.0 - End */

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
  return ret;
}

/* NCI 2.0 - Begin */
/*******************************************************************************
 **
 ** Function:        getCustomerData
 **
 ** Description:     Connect to the secure element.
 **                  e: JVM environment.
 **                  o: Java object.
 **
 ** Returns:         Handle of secure element.  values < 0 represent failure.
 **
 *******************************************************************************/
int NfcStExtensions::getCustomerData(uint8_t* customerData) {
  static const char fn[] = "NfcStExtensions::getCustomerData";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  int ret = 1;

  memcpy(customerData, mCustomerData, sizeof(mCustomerData));

  return ret;
}
/* NCI 2.0 - End */

/*******************************************************************************
 **
 ** Function:        getHWVersion
 **
 ** Description:     Connect to the secure element.
 **                  e: JVM environment.
 **                  o: Java object.
 **
 ** Returns:         Handle of secure element.  values < 0 represent failure.
 **
 *******************************************************************************/
int NfcStExtensions::getHWVersion(uint8_t* hwVersion) {
  static const char fn[] = "NfcStExtensions::getHWVersion";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  int ret = 1;

  hwVersion[0] = mHwInfo >> 8;
  hwVersion[1] = mHwInfo;

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: HW Version = %02X%02X ", fn, hwVersion[0], hwVersion[1]);
  /* NCI 2.0 - End */

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
  return ret;
}

/*******************************************************************************
 **
 ** Function:        getCRCConfiguration
 **
 ** Description:     Connect to the secure element.
 **                  e: JVM environment.
 **                  o: Java object.
 **
 ** Returns:         Handle of secure element.  values < 0 represent failure.
 **
 *******************************************************************************/
int NfcStExtensions::getCRCConfiguration(uint8_t* crcConfig) {
  int ret = 0;
  static const char fn[] = "NfcStExtensions::getCRCConfiguration";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);

  return ret;
}

/*******************************************************************************
 **
 ** Function:        isSEConnected
 **
 ** Description:     Connect to the secure element.
 **                  e: JVM environment.
 **                  o: Java object.
 **
 ** Returns:         Handle of secure element.  values < 0 represent failure.
 **
 *******************************************************************************/
bool NfcStExtensions::isSEConnected(int se_id) {
  static const char fn[] = "NfcStExtensions::isSEConnected";
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter, requesting info for SE id 0x%x", fn, se_id);
  bool result = false;

  StSecureElement::getInstance().getHostList();
  result = StSecureElement::getInstance().isSEConnected(se_id);

  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: SE id 0x%x connected is %d", fn, se_id, result);

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
  return result;
}

/*******************************************************************************
 **
 ** Function:        getFwInfo
 **
 ** Description:     Connect to the secure element.
 **                  e: JVM environment.
 **                  o: Java object.
 **
 ** Returns:         Handle of secure element.  values < 0 represent failure.
 **
 *******************************************************************************/
void NfcStExtensions::getFwInfo() {
  static const char fn[] = "NfcStExtensions::getFwInfo";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  uint8_t hw_version[HW_VERSION_SIZE];
  uint8_t bitmap = 0x0;

  if (getFirmwareVersion(mFwVersion)) {
    bitmap = 0x1;
  }

  if (getHWVersion(hw_version)) {
    bitmap |= 0x2;
  }

  if (bitmap & 0x1) {
    /* NCI 2.0 - Begin */
    LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: FW version is %02x.%02x.%02x%02X", fn, mFwVersion[0],
        mFwVersion[1], mFwVersion[2], mFwVersion[3]);
    /* NCI 2.0 - End */
  }

  if (bitmap & 0x2) {
    /* NCI 2.0 - Begin */
    LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: HW version is %02x%02x", fn, hw_version[0], hw_version[1]);
    /* NCI 2.0 - End */
  }

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
}

/*******************************************************************************
 **
 ** Function:        setNfcSystemProp
 **
 ** Description:     Remove AID from local table.
 **
 ** Returns:
 **
 *******************************************************************************/
void NfcStExtensions::setNfcSystemProp(const char* key_id,
                                       const char* key_value) {
  static const char fn[] = "NfcStExtensions::setNfcSystemProp";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter - key_id = %s, key_value = %s", fn, key_id, key_value);

  property_set(key_id, key_value);
}

/*******************************************************************************
 **
 ** Function:        getHceUserProp
 **
 ** Description:     Remove AID from local table.
 **
 ** Returns:
 **
 *******************************************************************************/
int NfcStExtensions::getHceUserProp() {
  static const char fn[] = "NfcStExtensions::getHceUserProp";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  // uint32_t value = getNfcSystemProp("nfc.st.hce_capable");
  // if (value != DUMMY_SYTEM_PROP_VALUE) {
  //  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
  //          "%s: User selected HCE status is 0x%X", fn, value);
  //} else {
  //  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
  //          "%s: User did not set the HCE yet", fn);
  //}

  // return value;
  return DUMMY_SYTEM_PROP_VALUE;
}

/*******************************************************************************
 **
 ** Function:        getNfcSystemProp
 **
 ** Description:     Retrieve given system property.
 **
 ** Returns:
 **
 *******************************************************************************/
uint32_t NfcStExtensions::getNfcSystemProp(const char* key_id) {
  char valueStr[PROPERTY_VALUE_MAX] = {0};
  static const char fn[] = "NfcStExtensions::getNfcSystemProp";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  unsigned long num = DUMMY_SYTEM_PROP_VALUE;  // prop not set

  int len = property_get(key_id, valueStr, "");

  if (len > 0) {
    sscanf(valueStr, "%lu", &num);
  }

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s - Read property %s - len = %d - value = %lu", fn, key_id, len, num);

  return num;
}

/*******************************************************************************
 **
 ** Function:        setRfConfiguration
 **
 ** Description:     Remove AID from local table.
 **
 ** Returns:
 **
 *******************************************************************************/
void NfcStExtensions::setRfConfiguration(int modeBitmap, uint8_t* techArray) {
  static const char fn[] = "NfcStExtensions::setRfCOnfiguration";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  bool mustRestartDiscovery = false;

  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: modeBitmap = 0x%x", fn, modeBitmap);
  for (int i = 0; i < RF_CONFIG_ARRAY_SIZE; i++) {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: techArray[%d] = 0x%x", fn, i, techArray[i]);
  }

  if (android::isDiscoveryStarted()) {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Stopping RF discovery", fn);
    // Stop RF discovery
    gIsReconfiguringDiscovery.start();
    android::startRfDiscovery(false);
    mustRestartDiscovery = true;
    android::pollingChanged(-1, 0, 0);
  }

  // Clean technos that are not selected.
  mWaitingForDmEvent = true;

  // Polling
  {
    if (((mRfConfig.techArray[READER_IDX]) &&
         (mRfConfig.modeBitmap & (0x1 << READER_IDX))) ||
        ((mRfConfig.techArray[P2P_POLL_IDX]) &&
         (mRfConfig.modeBitmap & (0x1 << P2P_LISTEN_IDX)))) {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Cleaning polling tech", fn);
      SyncEventGuard guard(mNfaDmEvent);
      nfaStat = NFA_DisablePolling();
      if (nfaStat == NFA_STATUS_OK) {
        mNfaDmEvent.wait();  // wait for NFA_POLL_DISABLED_EVT
        android::pollingChanged(0, -1, 0);
      } else {
        LOG(ERROR) << StringPrintf("%s: Failed to disable polling; error=0x%X",
                                   __func__, nfaStat);
      }
    }
  }

  // P2P listen
  {
    if ((mRfConfig.techArray[P2P_LISTEN_IDX]) &&
        (mRfConfig.modeBitmap & (0x1 << P2P_LISTEN_IDX))) {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Cleaning p2p listen tech", fn);
    }

    PeerToPeer::getInstance().enableP2pListening(false);
    android::pollingChanged(0, 0, -1);
  }

  // Listen
  {
    if ((mRfConfig.techArray[CE_IDX]) &&
        (mRfConfig.modeBitmap & (0x1 << CE_IDX))) {
      {
        LOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: Cleaning listen tech", fn);
        SyncEventGuard guard(mNfaDmEvent);
        if ((nfaStat = NFA_DisableListening()) == NFA_STATUS_OK) {
          mNfaDmEvent.wait();  // wait for NFA_LISTEN_DISABLED_EVT
        } else {
          LOG(ERROR) << StringPrintf(
              "%s: NFA_DisableListening() failed; error=0x%X", fn, nfaStat);
          return;
        }
      }

      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: De-register CE on DH tech", fn);
      {
        SyncEventGuard guard(mNfaDmEvent);
        if ((nfaStat = NFA_CeDeregisterAidOnDH(NFA_HANDLE_GROUP_CE | 0x1)) ==
            NFA_STATUS_OK) {
          mNfaDmEvent.wait();  // wait for NFA_LISTEN_DISABLED_EVT
        } else {
          LOG(ERROR) << StringPrintf(
              "%s: NFA_CeDeregisterAidOnDH() failed; error=0x%X", fn, nfaStat);
          return;
        }
      }
    }
  }

  // Reprogram RF_DISCOVER_CMD

  // Record Rf Config
  mRfConfig.modeBitmap = modeBitmap;
  /* NCI 2.0 - Begin */
  memcpy(mRfConfig.techArray, techArray, sizeof(mRfConfig.techArray));
  /* NCI 2.0 - End */

  ////////////////////
  // Parse modeBitmap
  ////////////////////
  // Check if any EE is active in which case we need to start listening.
  uint8_t activeUiccNfceeId = 0xFF;  // No default
#define MAX_NUM_EE 5
  uint8_t hostId[MAX_NUM_EE];
  uint8_t status[MAX_NUM_EE];
  int i;

  /* Initialize the array */
  memset(status, NFC_NFCEE_STATUS_INACTIVE, sizeof(status));

  NfcStExtensions::getInstance().getAvailableHciHostList(hostId, status);

  // Only one host active at the same time
  for (i = 0; i < MAX_NUM_EE; i++) {
    if (status[i] == NFC_NFCEE_STATUS_ACTIVE) {
      activeUiccNfceeId = hostId[i];

      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s - Found active SE: 0x%02X", fn, activeUiccNfceeId);
      break;
    }
  }

  // program poll, including P2P poll
  if ((modeBitmap & (0x1 << READER_IDX)) ||
      (modeBitmap & (0x1 << P2P_LISTEN_IDX))) {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Reprogram polling", fn);

    /* NCI 2.0 - Begin */
    uint8_t techMask = 0;

    if (modeBitmap & (0x1 << READER_IDX)) {
      techMask = techArray[READER_IDX];
    }
    if (modeBitmap & (0x1 << P2P_LISTEN_IDX)) {
      techMask |= techArray[P2P_POLL_IDX];
    }
    /* NCI 2.0 - End */

    SyncEventGuard guard(mNfaDmEvent);
    if ((nfaStat = NFA_EnablePolling(techMask)) == NFA_STATUS_OK) {
      mNfaDmEvent.wait();  // wait for NFA_POLL_ENABLED_EVT
      android::pollingChanged(0, 1, 0);
    } else {
      LOG(ERROR) << StringPrintf("%s: NFA_EnablePolling() failed; error=0x%X",
                                 fn, nfaStat);
      return;
    }
  }

  // program listen
  if (modeBitmap & (0x1 << CE_IDX)) {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Reprogram listen mode", fn);
    {
      if (techArray[CE_IDX] != 0) {
        if ((nfaStat = NFA_CeSetIsoDepListenTech(techArray[CE_IDX])) !=
            NFA_STATUS_OK) {  // nothing ot wait here
          LOG(ERROR) << StringPrintf(
              "%s: NFA_CeSetIsoDepListenTech() failed; error=0x%X", fn,
              nfaStat);
          return;
        }

        // Re- register CE on DH
        LOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: Re-register CE on DH", fn);
        {
          // Register
          SyncEventGuard guard(mNfaDmEvent);
          if ((nfaStat = NFA_CeRegisterAidOnDH(
                   NULL, 0, StRoutingManager::getInstance().stackCallback)) ==
              NFA_STATUS_OK) {
            mNfaDmEvent.wait();  // wait for NFA_CE_REGISTERED_EVT
          } else {
            LOG(ERROR) << StringPrintf(
                "%s: NFA_CeRegisterAidOnDH() failed; error=0x%X", fn, nfaStat);
            return;
          }
        }

        LOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: Re-enable listen mode", fn);
        {
          SyncEventGuard guard(mNfaDmEvent);
          if ((nfaStat = NFA_EnableListening()) == NFA_STATUS_OK) {
            mNfaDmEvent.wait();  // wait for NFA_LISTEN_ENABLED_EVT
          } else {
            LOG(ERROR) << StringPrintf(
                "%s: NFA_EnableListening() failed; error=0x%X", fn, nfaStat);
            return;
          }
        }
      } else {
        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: No listen mode techno to program/re-enable", fn);
      }
    }
  }

  if (modeBitmap & (0x1 << P2P_LISTEN_IDX)) {  // program p2p listen
    {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Reprogram p2p listen", fn);
      PeerToPeer::getInstance().setP2pListenMask(techArray[P2P_LISTEN_IDX]);
      PeerToPeer::getInstance().enableP2pListening(true);
    }
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Re-enable listen mode", fn);
    {
      SyncEventGuard guard(mNfaDmEvent);
      if ((nfaStat = NFA_EnableListening()) == NFA_STATUS_OK) {
        mNfaDmEvent.wait();  // wait for NFA_LISTEN_ENABLED_EVT
      } else {
        LOG(ERROR) << StringPrintf(
            "%s: NFA_EnableListening() failed; error=0x%X", fn, nfaStat);
        return;
      }
    }
    {
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Re-enable p2p", fn);
      SyncEventGuard guard(mNfaDmEvent);
      if ((nfaStat = NFA_ResumeP2p()) == NFA_STATUS_OK) {
        mNfaDmEvent.wait();  // wait for NFA_P2P_RESUMED_EVT
      } else {
        LOG(ERROR) << StringPrintf("%s: NFA_ResumeP2p() failed; error=0x%X", fn,
                                   nfaStat);
        return;
      }

      setP2pPausedStatus(false);
      android::pollingChanged(0, 0, 1);
    }
  } else {  // Disable p2p
    SyncEventGuard guard(mNfaDmEvent);
    if ((nfaStat = NFA_PauseP2p()) == NFA_STATUS_OK) {
      mNfaDmEvent.wait();  // wait for NFA_P2P_PAUSED_EVT
    } else {
      LOG(ERROR) << StringPrintf("%s: NFA_ResumeP2p() failed; error=0x%X", fn,
                                 nfaStat);
      return;
    }

    setP2pPausedStatus(true);
    android::pollingChanged(0, 0, -1);
  }

  mWaitingForDmEvent = false;

  if (mustRestartDiscovery) {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Restarting RF discovery", fn);
    // Stop RF discovery
    android::startRfDiscovery(true);
    android::pollingChanged(1, 0, 0);
    gIsReconfiguringDiscovery.end();
  }

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
}

/*******************************************************************************
 **
 ** Function:        getRfConfiguration
 **
 ** Description:     Receive connection-related events from stack.
 **                  connEvent: Event code.
 **                  eventData: Event data.
 **
 ** Returns:         None
 **
 *******************************************************************************/
int NfcStExtensions::getRfConfiguration(uint8_t* techArray) {
  static const char fn[] = "NfcStExtensions::getRfConfiguration";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  int i;

  memcpy(techArray, mRfConfig.techArray, sizeof(mRfConfig.techArray));

  for (i = 0; i < RF_CONFIG_ARRAY_SIZE; i++) {
    LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: mRfConfig.techArray[%d] = 0x%x", fn, i, mRfConfig.techArray[i]);
  }

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: mRfConfig.modeBitmap = 0x%x", fn, mRfConfig.modeBitmap);

  return mRfConfig.modeBitmap;
}

void NfcStExtensions::setRfBitmap(int modeBitmap) {
  static const char fn[] = "NfcStExtensions::setRfBitmap()";
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter - modeBitmap: %02X", fn, modeBitmap);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;

  // Clean techno
  mWaitingForDmEvent = true;

  // Polling
  {
    if (((modeBitmap & (0x1 << READER_IDX)) == 0) ||
        ((modeBitmap & (0x1 << P2P_POLL_IDX)) == 0)) {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Cleaning polling tech", fn);
      SyncEventGuard guard(mNfaDmEvent);
      nfaStat = NFA_DisablePolling();
      if (nfaStat == NFA_STATUS_OK) {
        mNfaDmEvent.wait();  // wait for NFA_POLL_DISABLED_EVT
        android::pollingChanged(0, -1, 0);
      } else {
        LOG(ERROR) << StringPrintf("%s: Failed to disable polling; error=0x%X",
                                   __func__, nfaStat);
      }
    } else {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Not cleaning polling tech", fn);
    }
  }

  // P2P listen
  {
    if ((modeBitmap & (0x1 << P2P_LISTEN_IDX)) == 0) {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Cleaning p2p listen tech", fn);
      PeerToPeer::getInstance().enableP2pListening(false);
      android::pollingChanged(0, 0, -1);
    } else {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Not cleaning p2p listen tech", fn);
    }
  }

  // Listen
  {
    if ((modeBitmap & (0x1 << CE_IDX)) == 0) {
      {
        LOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: Cleaning listen tech", fn);
        SyncEventGuard guard(mNfaDmEvent);
        if ((nfaStat = NFA_DisableListening()) == NFA_STATUS_OK) {
          mNfaDmEvent.wait();  // wait for NFA_LISTEN_DISABLED_EVT
        } else {
          LOG(ERROR) << StringPrintf(
              "%s: NFA_DisableListening() failed; error=0x%X", fn, nfaStat);
        }
      }

      {  // deregister CEonDH
        LOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: De-register CE on DH tech", fn);
        SyncEventGuard guard(mNfaDmEvent);
        if ((nfaStat = NFA_CeDeregisterAidOnDH(NFA_HANDLE_GROUP_CE | 0x1)) ==
            NFA_STATUS_OK) {
          mNfaDmEvent.wait();  // wait for NFA_LISTEN_DISABLED_EVT
        } else {
          LOG(ERROR) << StringPrintf(
              "%s: NFA_CeDeregisterAidOnDH() failed; error=0x%X", fn, nfaStat);
        }
      }
    } else {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Not cleaning listen tech", fn);
    }
  }

  // Record Rf Config
  mRfConfig.modeBitmap = modeBitmap;

  mWaitingForDmEvent = false;
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
void NfcStExtensions::nfaConnectionCallback(uint8_t connEvent,
                                            tNFA_CONN_EVT_DATA* eventData) {
  static const char fn[] = "NfcStExtensions::nfaConnectionCallback";

  if (sStExtensions.mWaitingForDmEvent == false) {
    return;
  }
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: event= %u", fn, connEvent);

  switch (connEvent) {
    case NFA_POLL_ENABLED_EVT: {  // whether polling successfully started
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_POLL_ENABLED_EVT: status = %u", fn, eventData->status);
    } break;

    case NFA_POLL_DISABLED_EVT: {  // Listening/Polling stopped
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_POLL_DISABLED_EVT: status = %u", fn, eventData->status);
    } break;

    case NFA_SET_P2P_LISTEN_TECH_EVT: {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_SET_P2P_LISTEN_TECH_EVT", fn);
    } break;

    case NFA_LISTEN_DISABLED_EVT: {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_LISTEN_DISABLED_EVT", fn);

    } break;

    case NFA_LISTEN_ENABLED_EVT: {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_LISTEN_ENABLED_EVT", __func__);
    } break;

    case NFA_P2P_PAUSED_EVT: {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_P2P_PAUSED_EVT", fn);
    } break;

    case NFA_P2P_RESUMED_EVT: {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_P2P_RESUMED_EVT", fn);
    } break;

    case NFA_CE_DEREGISTERED_EVT: {
      tNFA_CE_DEREGISTERED& ce_deregistered = eventData->ce_deregistered;
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_CE_DEREGISTERED_EVT; h=0x%X", fn, ce_deregistered.handle);
    } break;

    case NFA_CE_REGISTERED_EVT: {
      tNFA_CE_REGISTERED& ce_registered = eventData->ce_registered;
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_CE_REGISTERED_EVT; status=0x%X; h=0x%X", fn,
                          ce_registered.status, ce_registered.handle);
    } break;

    case NFA_CE_UICC_LISTEN_CONFIGURED_EVT: {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_CE_UICC_LISTEN_CONFIGURED_EVT", fn);
    } break;

    default:
      LOG(ERROR) << StringPrintf("%s: Event not handled here !!!", fn);
      return;
  }

  SyncEventGuard guard(sStExtensions.mNfaDmEvent);
  sStExtensions.mNfaDmEvent.notifyOne();
}

/*******************************************************************************
 **
 ** Function:        notifyTransactionListenersOfAidSelection
 **
 ** Description:     Notify the NFC service about a transaction event from
 *secure element.
 **                  aid: Buffer contains application ID.
 **                  aidLen: Length of application ID.
 **                  host_id: host which has selected the AID.
 **
 ** Returns:         None
 **
 *******************************************************************************/
// void NfcStExtensions::notifyTransactionListenersOfAidSelection (const
// uint8_t* aidBuffer, uint8_t aidBufferLen, uint8_t host_id)
//{
//    static const char fn [] =
//    "NfcStExtensions::notifyTransactionListenersOfAid"; LOG_IF(INFO,
//    nfc_debug_enabled) << StringPrintf ("%s: enter; aid len=%u", fn,
//    aidBufferLen);
//
//    if (aidBufferLen == 0) {
//        return;
//    }
//
//    JNIEnv* e = NULL;
//    ScopedAttach attach(mNativeData->vm, &e);
//    if (e == NULL) {
//        LOG(ERROR) << StringPrintf ("%s: jni env is null", fn);
//        return;
//    }
//
//    const uint16_t tlvMaxLen = aidBufferLen + 10;
//    uint8_t* tlv = new uint8_t [tlvMaxLen];
//    if (tlv == NULL) {
//        LOG(ERROR) << StringPrintf ("%s: fail allocate tlv", fn);
//        return;
//    }
//
//    memcpy (tlv, aidBuffer, aidBufferLen);
//    uint16_t tlvActualLen = aidBufferLen;
//
//    ScopedLocalRef<jobject> tlvJavaArray(e, e->NewByteArray(tlvActualLen));
//    if (tlvJavaArray.get() == NULL) {
//        LOG(ERROR) << StringPrintf ("%s: fail allocate array", fn);
//        goto TheEnd;
//    }
//
//    e->SetByteArrayRegion ((jbyteArray)tlvJavaArray.get(), 0, tlvActualLen,
//    (jbyte *)tlv); if (e->ExceptionCheck()) {
//        e->ExceptionClear();
//        LOG(ERROR) << StringPrintf ("%s: fail fill array", fn);
//        goto TheEnd;
//    }
//
//    e->CallVoidMethod (mNativeData->manager,
//    android::gCachedNfcManagerNotifyAidSelectionReceived,
//    tlvJavaArray.get(),(jint)host_id); if (e->ExceptionCheck()) {
//        e->ExceptionClear();
//        LOG(ERROR) << StringPrintf ("%s: fail notify", fn);
//        goto TheEnd;
//    }
//
// TheEnd:
//    delete [] tlv;
//    LOG_IF(INFO, nfc_debug_enabled) << StringPrintf ("%s: exit", fn);
//}
/*******************************************************************************
 **
 ** Function:        doVdcMeasurement
 **
 ** Description:     Receive connection-related events from stack.
 **                  connEvent: Event code.
 **                  eventData: Event data.
 **
 ** Returns:         None
 **
 *******************************************************************************/
// int NfcStExtensions::doVdcMeasurement(bool isFieldOn) {
//  static const char fn[] = "NfcStExtensions::doVdcMeasurement";
//  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
//  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
//  uint8_t dataVdc[] = { 0x00, 0x00, 0x00, 0x00 };
//  uint8_t dataRfFieldOn[] = { 0x01, 0x00, 0x00, 0x00, 0x00 };
//  uint8_t dataRfFieldOff[] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
//  uint8_t* dataRfFieldConfig;
//
//  bool isRfFieldStart = false;
//  bool isRfFieldStop = false;
//  bool isVdcMeasOn = false;
//
//  if (mVdcMeasConfig.isRfFieldOn == false) { //no RF field
//    if (isFieldOn == true) {
//      //Start RF field gen and VDC meas
//      isRfFieldStart = true;
//      dataRfFieldConfig = dataRfFieldOn;
//      isVdcMeasOn = true;
//      mVdcMeasConfig.isRfFieldOn = true;
//    } else { //false
//      //Only perform VDC meas, no field gen needed
//      isVdcMeasOn = true;
//    }
//  } else { //true - RF field is generated
//    if (isFieldOn == true) {
//      //Start RF field gen and VDC meas
//      isVdcMeasOn = true;
//      mVdcMeasConfig.isRfFieldOn = true;
//    } else { //false
//      // Stop generating RF field
//      isRfFieldStop = true;
//      dataRfFieldConfig = dataRfFieldOff;
//      mVdcMeasConfig.isRfFieldOn = false;
//    }
//  }
//
//  // If we reach this point, answer was rx, good or wrong
//  mIsWaitingEvent.vdcMeasRslt = false;
//
//  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
//
//  return mVdcMeasConfig.measValue;
//}

/*******************************************************************************
 **
 ** Function:        setDefaultOffHostRoute
 **
 ** Description:     Set Default Off Host Route for HCE
 **
 ** Returns:
 **
 *******************************************************************************/
void NfcStExtensions::setDefaultOffHostRoute(int route) {
  static const char fn[] = "NfcStExtensions::setDefaultOffHostRoute";
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter - route = 0x%x", fn, route);

  mDefaultIsoTechRoute = route;
}

/*******************************************************************************
 **
 ** Function:        getDefaultOffHostRoute
 **
 ** Description:     Get Default Off Host Route for HCE
 **
 ** Returns:
 **
 *******************************************************************************/
int NfcStExtensions::getDefaultOffHostRoute() {
  static const char fn[] = "NfcStExtensions::getDefaultOffHostRoute";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter - mDefaultIsoTechRoute = 0x%x", fn, mDefaultIsoTechRoute);

  return mDefaultIsoTechRoute;
}

/*******************************************************************************
 **
 ** Function:        getProprietaryConfigSettings
 **
 ** Description:     Get a particular setting from a Proprietary Configuration
 **                  settings register.
 **
 ** Returns:
 **
 *******************************************************************************/
/* NCI 2.0 - Begin */ bool NfcStExtensions::getProprietaryConfigSettings(
    int prop_config_id, int byteNb, int bitNb) {
  static const char fn[] = "NfcStExtensions::getProprietaryConfigSettings";

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter - byteNb = 0x%x, bitNb = 0x%x", fn, byteNb, bitNb);

  bool status = false;
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;

  uint8_t mActionRequestParam[] = {0x03, 0x00, (uint8_t)prop_config_id, 0x01,
                                   0x0};

  mIsWaitingEvent.getPropConfig = true;

  SyncEventGuard guard(mVsActionRequestEvent);

  nfaStat = NFA_SendVsCommand(OID_ST_VS_CMD, 5, mActionRequestParam,
                              nfaVsCbActionRequest);
  if (nfaStat != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf(
        "%s: NFA_SendVsCommand() call failed; error=0x%X", fn, nfaStat);
  } else {
    mVsActionRequestEvent.wait();
    mIsWaitingEvent.getPropConfig = false;
  }

  mIsWaitingEvent.getPropConfig = false;

  status = ((mPropConfig.config[byteNb] & (0x1 << bitNb)) ? true : false);

  return status;
}

/*******************************************************************************
 **
 ** Function:        setProprietaryConfigSettings
 **
 ** Description:     Set a particular setting in a Proprietary Configuration
 **                  settings register.
 ** Returns:
 **
 *******************************************************************************/
void NfcStExtensions::setProprietaryConfigSettings(int prop_config_id,
                                                   int byteNb, int bitNb,
                                                   bool status) {
  static const char fn[] = "NfcStExtensions::setProprietaryConfigSettings";

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter - byteNb = 0x%x, bitNb = 0x%x, ValueToSet = %d", fn, byteNb,
      bitNb, status);

  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  bool currentStatus;
  uint8_t* setPropConfig;
  bool mustRestartDiscovery = false;

  // Before Updating configuration, read it to update content of
  // mNfccConfig.config[]
  currentStatus = getProprietaryConfigSettings(
      prop_config_id, byteNb, bitNb);  // real byte number computed in there

  if (currentStatus == status) {  // Not change needed
    LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: Current status is equal to requested status - Exit", fn);
    return;
  }

  if (mPropConfigLen == 0) {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: No parameters returned - Exit", fn);
    return;
  }

  if (android::isDiscoveryStarted()) {
    // Stop RF discovery
    mustRestartDiscovery = true;
    gIsReconfiguringDiscovery.start();
    android::startRfDiscovery(false);
  }

  setPropConfig = (uint8_t*)GKI_os_malloc(mPropConfigLen + 6);

  setPropConfig[0] = 0x04;
  setPropConfig[1] = 0x00;
  setPropConfig[2] = prop_config_id;
  setPropConfig[3] = 0x01;
  setPropConfig[4] = 0x00;
  setPropConfig[5] = mPropConfigLen;

  memcpy(setPropConfig + 6, &mPropConfig.config[0], mPropConfigLen);

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: Current value in register is 0x%x", fn, mPropConfig.config[byteNb]);
  if (status == true) {
    setPropConfig[byteNb + 6] = mPropConfig.config[byteNb] | ((0x1 << bitNb));
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Requesting Value - value to set is 0x%x", fn,
                        setPropConfig[byteNb + 6]);
  } else {
    setPropConfig[byteNb + 6] = mPropConfig.config[byteNb] & ~(0x1 << bitNb);
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Requesting Value - value to set is 0x%x", fn,
                        setPropConfig[byteNb + 6]);
  }

  {
    mIsWaitingEvent.setPropConfig = true;

    SyncEventGuard guard(mVsActionRequestEvent);

    nfaStat = NFA_SendVsCommand(OID_ST_VS_CMD, setPropConfig[5] + 6,
                                setPropConfig, nfaVsCbActionRequest);
    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "%s: NFA_SendVsCommand() call failed; error=0x%X", fn, nfaStat);
    } else {
      mVsActionRequestEvent.wait();
      mIsWaitingEvent.setPropConfig = false;
    }

    mIsWaitingEvent.setPropConfig = false;
  }

  GKI_os_free(setPropConfig);

  if (mustRestartDiscovery) {
    // Start RF discovery
    android::startRfDiscovery(true);
    gIsReconfiguringDiscovery.end();
  }
}
/* NCI 2.0 - End */

/*******************************************************************************
 **
 ** Function:        setP2pPausedStatus
 **
 ** Description:     sets the variable mIsP2pPaused (true, p2p is paused,
 **                  false, p2p is not paused)
 **
 ** Returns:         None
 **
 *******************************************************************************/
void NfcStExtensions::setP2pPausedStatus(bool status) {
  static const char fn[] = "NfcStExtensions::setP2pPausedStatus";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter; current status is mIsP2pPaused = %d, new status is %d", fn,
      mIsP2pPaused, status);

  mIsP2pPaused = status;
}

/*******************************************************************************
 **
 ** Function:        getP2pPausedStatus
 **
 ** Description:     gets the variable mIsP2pPaused
 **
 ** Returns:         (true, p2p is paused,
 **                  false, p2p is not paused)
 **
 *******************************************************************************/
bool NfcStExtensions::getP2pPausedStatus() {
  static const char fn[] = "NfcStExtensions::getP2pPausedStatus";
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; mIsP2pPaused = %d", fn, mIsP2pPaused);

  return mIsP2pPaused;
}

/*******************************************************************************
 **
 ** Function:        getATR
 **
 ** Description:     get the ATR read by the StSecureElement at eSE
 *connection.
 **                  Is part of ST Extensions.
 **
 ** Returns:         None.
 **
 *******************************************************************************/
int NfcStExtensions::getATR(uint8_t* atr) {
  static const char fn[] = "NfcStExtensions::getATR";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", fn);
  int i, length = 0xff;

  length = StSecureElement::getInstance().mAtrInfo.length;
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: ATR length = %d;", fn, length);

  for (i = 0; i < length; i++) {
    *(atr + i) = StSecureElement::getInstance().mAtrInfo.data[i];
  }

  return length;
}
/*******************************************************************************
 **
 ** Function:        EnableSE
 **
 ** Description:     Connect/disconnect  the secure element.
 **                  e: JVM environment.
 **                  o: Java object.
 **
 ** Returns:         Handle of secure element.  values < 0 represent failure.
 **
 *******************************************************************************/
bool NfcStExtensions::EnableSE(int se_id, bool enable) {
  static const char fn[] = "NfcStExtensions::EnableSE";
  bool result = false;

  if ((se_id & 0x80) != 0) {  // HCI-NFCEE
    result = StSecureElement::getInstance().EnableSE(se_id, enable);
  } else {  // NDEF NFCEE
    result = StCardEmulationEmbedded::getInstance().enable(se_id, enable);
  }

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);

  return result;
}

/*******************************************************************************
 **
 ** Function:        connectGate
 **
 ** Description:     Connect/disconnect  the secure element.
 **                  e: JVM environment.
 **                  o: Java object.
 **
 ** Returns:         Handle of secure element.  values < 0 represent failure.
 **
 *******************************************************************************/
int NfcStExtensions::connectGate(int gate_id, int host_id) {
  static const char fn[] = "NfcStExtensions::connectGate";
  int pipe_id = 0xFF;

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  pipe_id = prepareGateForTest(gate_id, host_id);

  if (pipe_id != 0xFF) {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Allocated pipe 0x%x for gate 0x%x on host 0x%x",
                        fn, pipe_id, gate_id, host_id);
  } else {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Pipe creation failed for gate 0x%x on host 0x%x",
                        fn, gate_id, host_id);
  }

  return pipe_id;
}

/*******************************************************************************
 **
 ** Function:        transceive
 **
 ** Description:     Connect/disconnect  the secure element.
 **                  e: JVM environment.
 **                  o: Java object.
 **
 ** Returns:         Handle of secure element.  values < 0 represent failure.
 **
 *******************************************************************************/
bool NfcStExtensions::transceive(uint8_t pipeId, uint8_t hciCmd,
                                 uint16_t txBufferLength, uint8_t* txBuffer,
                                 int32_t& recvBufferActualSize,
                                 uint8_t* rxBuffer) {
  static const char fn[] = "NfcStExtensions::transceive";
  bool waitOk = false;
  int i;
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;

  mSentHciCmd = hciCmd;
  mIsWaitingEvent.propHciRsp = true;
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  {
    SyncEventGuard guard(mHciRspRcvdEvent);
    nfaStat = NFA_HciSendCommand(mNfaStExtHciHandle, pipeId, hciCmd,
                                 txBufferLength, txBuffer);
    if (nfaStat == NFA_STATUS_OK) {
      waitOk = mHciRspRcvdEvent.wait(1000);
      if (waitOk == false) {  // timeout occurs
        LOG(ERROR) << StringPrintf("%s: wait response timeout", fn);
      }
    } else {
      LOG(ERROR) << StringPrintf("%s: fail send data; error=0x%X", fn, nfaStat);
      mIsWaitingEvent.propHciRsp = false;
      return false;
    }
  }

  mIsWaitingEvent.propHciRsp = false;

  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: received data of length 0x%02X", fn, mRxHciDataLen);

  recvBufferActualSize = mRxHciDataLen;
  for (i = 0; i < mRxHciDataLen; i++) {
    rxBuffer[i] = mRxHciData[i];
  }

  return (nfaStat == NFA_STATUS_OK ? true : false);
}

/*******************************************************************************
 **
 ** Function:        disconnectGate
 **
 ** Description:     Connect/disconnect  the secure element.
 **                  e: JVM environment.
 **                  o: Java object.
 **
 ** Returns:         Handle of secure element.  values < 0 represent failure.
 **
 *******************************************************************************/
void NfcStExtensions::disconnectGate(uint8_t pipeId) {
  static const char fn[] = "NfcStExtensions::disconnectGate";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;

  SyncEventGuard guard(mNfaHciEventRcvdEvent);
  nfaStat = NFA_HciClosePipe(mNfaStExtHciHandle, pipeId);
  if (nfaStat == NFA_STATUS_OK) {
    mNfaHciEventRcvdEvent.wait();  // wait for NFA_HCI_CLOSE_PIPE_EVT
  } else {
    LOG(ERROR) << StringPrintf("%s: error during NFA method call; error=0x%X",
                               fn, nfaStat);
  }
}

/*******************************************************************************
 **
 ** Function:        setNfccPowerMode
 **
 ** Description:     Set the host power mode to the NFCC.
 **                  transport : Transport action to perform :
 **                  			0x0 : Keep the NCI DH connected.
 **                  			0x4 : Disconnect transport interface..
 **                  powermode: Host power mode
 **
 ** Returns:
 **
 *******************************************************************************/
// void NfcStExtensions::setNfccPowerMode(int transport, int powermode) {
//  static const char fn[] = "NfcStExtensions::setNfccPowerMode";
//  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
//  uint8_t mCmdData[] = { (uint8_t) transport, (uint8_t) powermode };
//
//
//  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
//}

/*******************************************************************************
 **
 ** Function:        checkListenAPassiveOnSE
 **
 ** Description:     Set the host power mode to the NFCC.
 **                  transport : Transport action to perform :
 **                             0x0 : Keep the NCI DH connected.
 **                             0x4 : Disconnect transport interface..
 **                  powermode: Host power mode
 **
 ** Returns:
 **
 *******************************************************************************/

#define SE_CONNECTED_MASK 0x01
#define A_CARD_RF_GATE_MASK 0x02
#define B_CARD_RF_GATE_MASK 0x04

bool NfcStExtensions::checkListenAPassiveOnSE() {
  static const char fn[] = "NfcStExtensions::checkListenAPassiveOnSE";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  int i;
  uint8_t seList[2] = {0x02, 0xc0};
  uint8_t techArray[4];
  uint8_t bitmap = 0;

  // Need to be called only once
  /* NCI 2.0 - Begin */
  getPipesInfo();
  /* NCI 2.0 - End */

  for (i = 0; i < 2; i++) {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Checking if %02X is connected", fn, seList[i]);
    if (isSEConnected(seList[i]) == true) {
      bitmap |= SE_CONNECTED_MASK;
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: %02X is connected, check presence of A Card RF gate/pipe", fn,
          seList[i]);

      // Check if pipe for A Card RF gate was created on that SE
      if (getPipeIdForGate(seList[i], 0x23) != 0xFF) {
        bitmap |= A_CARD_RF_GATE_MASK;
      }
      // Check if pipe for ABCard RF gate was created on that SE
      if (getPipeIdForGate(seList[i], 0x21) != 0xFF) {
        bitmap |= B_CARD_RF_GATE_MASK;
      }
    }
  }

  // No A Card RF gate/pipe found, remove Listen A passive mode from P2P
  // polling tech
  if (bitmap == (SE_CONNECTED_MASK | B_CARD_RF_GATE_MASK)) {
    LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: Removing Listen A passive from RF configuration", fn);
    memcpy(techArray, mRfConfig.techArray, sizeof(mRfConfig.techArray));
    techArray[P2P_LISTEN_IDX] &= ~NFA_TECHNOLOGY_MASK_A;
    setRfConfiguration(mRfConfig.modeBitmap, techArray);

    return true;
  }

  return false;
}

/* NCI 2.0 - Begin */
/*******************************************************************************
 **
 ** Function:        setNciConfig
 **
 ** Description:     Set a NCI parameter throught the NFA_SetConfig API.
 **                  param_id : The param id
 **                  param : Aid table
 **                  length : length of the parameter payload
 **
 ** Returns:         void
 **
 *******************************************************************************/
void NfcStExtensions::setNciConfig(int param_id, uint8_t* param, int length) {
  static const char fn[] = "NfcStExtensions::setNciConfig";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  bool mustRestartDiscovery = false;

  if (android::isDiscoveryStarted()) {
    // Stop RF discovery
    gIsReconfiguringDiscovery.start();
    android::startRfDiscovery(false);
    mustRestartDiscovery = true;
  }

  SyncEventGuard guard(mNfaConfigEvent);
  nfaStat = NFA_SetConfig(param_id, length, param);
  if (nfaStat == NFA_STATUS_OK) {
    mNfaConfigEvent.wait();  // wait for NFA_DM_SET_CONFIG_EVT
  } else {
    LOG(ERROR) << StringPrintf("%s: NFA_SetConfig() failed; error=0x%X", fn,
                               nfaStat);
  }

  if (mustRestartDiscovery) {
    // Start RF discovery
    android::startRfDiscovery(true);
    gIsReconfiguringDiscovery.end();
  }
}

/*******************************************************************************
 **
 ** Function:        GetNciConfig
 **
 ** Description:     Set a NCI parameter throught the NFA_SetConfig API.
 **                  param_id : The param id
 **                  param : Aid table
 **                  length : length of the parameter payload
 **
 ** Returns:         void
 **
 *******************************************************************************/
void NfcStExtensions::getNciConfig(int param_id, uint8_t* param,
                                   uint16_t& length) {
  static const char fn[] = "NfcStExtensions::getNciConfig";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  tNFA_PMID data[1] = {0x00};
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;

  data[0] = param_id;

  SyncEventGuard guard(mNfaConfigEvent);
  nfaStat = NFA_GetConfig(0x01, data);
  if (nfaStat == NFA_STATUS_OK) {
    mNfaConfigEvent.wait();  // wait for NFA_DM_GET_CONFIG_EVT
  } else {
    LOG(ERROR) << StringPrintf("%s: NFA_GetConfig() failed; error=0x%X", fn,
                               nfaStat);
  }

  length = mNfaConfigLength;

  // Check status
  if (length >= 4) {
    length = mNfaConfigPtr[2];
    // Return only from first byte of value
    memcpy(param, (mNfaConfigPtr + 3), length);
  }
}

/*******************************************************************************
 **
 ** Function:        notifyNciConfigCompletion
 **
 ** Description:     Set a NCI parameter throught the NFA_SetConfig API.
 **                  param_id : The param id
 **                  param : Aid table
 **                  length : length of the parameter payload
 **
 ** Returns:         void
 **
 *******************************************************************************/
void NfcStExtensions::notifyNciConfigCompletion(bool isGet, uint16_t length,
                                                uint8_t* param) {
  static const char fn[] = "NfcStExtensions::notifyNciConfigCompletion";
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: isGet = %d, length = 0x%02X", fn, isGet, length);

  if (isGet) {
    sStExtensions.mNfaConfigLength = length;
    sStExtensions.mNfaConfigPtr = param;
  }

  SyncEventGuard guard(sStExtensions.mNfaConfigEvent);
  sStExtensions.mNfaConfigEvent.notifyOne();
}

/*******************************************************************************
 **
 ** Function:        sendPropSetConfig
 **
 ** Description:
 **
 ** Returns:         void
 **
 *******************************************************************************/
void NfcStExtensions::sendPropSetConfig(int subSetId, int configId,
                                        uint8_t* param, uint32_t length) {
  static const char fn[] = "NfcStExtensions::sendPropSetConfig";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  uint8_t* setPropConfig;
  bool mustRestartDiscovery = false;

  if (android::isDiscoveryStarted()) {
    // Stop RF discovery
    gIsReconfiguringDiscovery.start();
    android::startRfDiscovery(false);
    mustRestartDiscovery = true;
  }

  setPropConfig = (uint8_t*)GKI_os_malloc(length + 6);

  setPropConfig[0] = 0x04;
  setPropConfig[1] = 0x00;
  setPropConfig[2] = subSetId;
  setPropConfig[3] = 0x01;
  setPropConfig[4] = configId;
  setPropConfig[5] = length;

  memcpy(setPropConfig + 6, param, length);

  {
    mIsWaitingEvent.setPropConfig = true;

    SyncEventGuard guard(mVsActionRequestEvent);

    nfaStat = NFA_SendVsCommand(OID_ST_VS_CMD, setPropConfig[5] + 6,
                                setPropConfig, nfaVsCbActionRequest);
    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "%s: NFA_SendVsCommand() call failed; error=0x%X", fn, nfaStat);
    } else {
      mVsActionRequestEvent.wait();
      mIsWaitingEvent.setPropConfig = false;
    }

    mIsWaitingEvent.setPropConfig = false;
  }

  GKI_os_free(setPropConfig);

  if (subSetId == 0x10) {
    ApplyPropRFConfig();
  }

  if (mustRestartDiscovery) {
    // Start RF discovery
    android::startRfDiscovery(true);
    gIsReconfiguringDiscovery.end();
  }
}

/*******************************************************************************
 **
 ** Function:        sendPropGetConfig
 **
 ** Description:     Get a NFCC propretary configuraion.
 **                  subSetId : Configuration Sub-Set ID
 **                  configId : Parameter ID
 **                  param : NFCC configuration returned value
 **                  length : length of the param payload
 **
 ** Returns:         void
 **
 *******************************************************************************/
void NfcStExtensions::sendPropGetConfig(int subSetId, int configId,
                                        uint8_t* param, uint16_t& length) {
  static const char fn[] = "NfcStExtensions::sendPropGetConfig";
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter (subSetId=0x%x)", fn, subSetId);

  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  uint8_t getPropConfig[5];
  bool mustRestartDiscovery = false;

  if (android::isDiscoveryStarted()) {
    // Stop RF discovery
    gIsReconfiguringDiscovery.start();
    android::startRfDiscovery(false);
    mustRestartDiscovery = true;
  }

  getPropConfig[0] = 0x03;
  getPropConfig[1] = 0x00;
  getPropConfig[2] = subSetId;
  getPropConfig[3] = 0x01;
  getPropConfig[4] = configId;

  mIsWaitingEvent.getPropConfig = true;

  SyncEventGuard guard(mVsActionRequestEvent);

  nfaStat =
      NFA_SendVsCommand(OID_ST_VS_CMD, 5, getPropConfig, nfaVsCbActionRequest);
  if (nfaStat != NFA_STATUS_OK) {
    LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: NFA_SendVsCommand() call failed; error=0x%X", fn, nfaStat);
  } else {
    mVsActionRequestEvent.wait();
    mIsWaitingEvent.getPropConfig = false;

    length = mPropConfigLen;
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: data received, length=%d", fn, length);
    memcpy(param, (mPropConfig.config), length);
  }
  mIsWaitingEvent.getPropConfig = false;

  if (mustRestartDiscovery) {
    // Start RF discovery
    android::startRfDiscovery(true);
    gIsReconfiguringDiscovery.end();
  }
}

/*******************************************************************************
 **
 ** Function:        sendPropTestCmd
 **
 ** Description:
 **
 ** Returns:         void
 **
 *******************************************************************************/
void NfcStExtensions::sendPropTestCmd(int subCode, uint8_t* paramTx,
                                      uint16_t lengthTx, uint8_t* paramRx,
                                      uint16_t& lengthRx) {
  static const char fn[] = "NfcStExtensions::sendPropTestCmd";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  uint8_t* sendTestCmd;

  // Transmission of PROP_TEST_XX_CMD is not allowed in RFST_DISCOVERY state
  //    if (android::isDiscoveryStarted())
  //    {
  //        //Stop RF discovery
  //        android::startRfDiscovery(false);
  //    }

  mPropTestRspPtr = NULL;
  mPropTestRspLen = 0;

  sendTestCmd = (uint8_t*)GKI_os_malloc(lengthTx + 1);

  sendTestCmd[0] = subCode;

  memcpy(sendTestCmd + 1, paramTx, lengthTx);

  {
    mIsWaitingEvent.sendPropTestCmd = true;

    SyncEventGuard guard(mVsActionRequestEvent);

    nfaStat = NFA_SendVsCommand(OID_ST_TEST_CMD, (lengthTx + 1), sendTestCmd,
                                nfaVsCbActionRequest);
    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "%s: NFA_SendVsCommand() call failed; error=0x%X", fn, nfaStat);
    } else {
      mVsActionRequestEvent.wait();
    }

    mIsWaitingEvent.sendPropTestCmd = false;
  }

  GKI_os_free(sendTestCmd);

  lengthRx = mPropTestRspLen;

  // Check status
  if (mPropTestRspLen > 0) {
    // Return only from first byte of value
    memcpy(paramRx, mPropTestRspPtr, mPropTestRspLen);
  }

  // Release memory
  if (mPropTestRspPtr != NULL) {
    GKI_os_free(mPropTestRspPtr);
  }
}

/*******************************************************************************
 **
 ** Function:        getAvailableHciHostList
 **
 ** Description:      Get the available NFCEE id and their status
 **
 ** Returns:         void
 **
 *******************************************************************************/
int NfcStExtensions::getAvailableHciHostList(uint8_t* nfceeId,
                                             uint8_t* conInfo) {
  static const char fn[] = "NfcStExtensions::getAvailableHciHostList";

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  // StSecureElement::getInstance().getHostList();
  return StSecureElement::getInstance().retrieveHostList(nfceeId, conInfo);
}

/*******************************************************************************
 **
 ** Function:        ApplyPropRFConfig
 **
 ** Description:    Force new RF configuration. Only available for
 *CustomA/CustomB.
 **
 ** Returns:         void
 **
 *******************************************************************************/
void NfcStExtensions::ApplyPropRFConfig() {
  static const char fn[] = "NfcStExtensions::ApplyPropRFConfig";
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  SyncEventGuard guard(mVsActionRequestEvent);
  uint8_t mPropApplyRfConfig[] = {0x0A};

  nfaStat = NFA_SendVsCommand(OID_ST_VS_CMD, 1, mPropApplyRfConfig,
                              nfaVsCbActionRequest);
  if (nfaStat != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf(
        "%s: NFA_SendVsCommand() call failed; error=0x%X", fn, nfaStat);
  } else {
    mVsActionRequestEvent.wait();
  }
}

// Compute a difference between 2 timestamps of fw logs and return the result
// in ms. Note that FW clock is not always running so this is only reliable
// during a few secs.
int NfcStExtensions::FwTsDiffToMs(uint32_t fwtsstart, uint32_t fwtsend) {
  uint32_t diff;
  if (fwtsstart <= fwtsend) {
    diff = fwtsend - fwtsstart;
  } else {
    // overflow
    diff = (0xFFFFFFFF - fwtsstart) + fwtsend;
  }
  return (int)((float)diff * 0.00457);
}

/*******************************************************************************
 **
 ** Function:        StHandleLogDataDynParams
 **
 ** Description:    State machine to try and detect failures during anticol.
 **
 ** Returns:         void
 **
 *******************************************************************************/

#define T_firstRx 0x04
#define T_dynParamUsed 0x07
#define T_CERx 0x09
#define T_fieldOn 0x10
#define T_fieldOff 0x11
#define T_fieldLevel 0x18
#define T_CERxError 0x19

typedef void* (*THREADFUNCPTR)(void*);

void NfcStExtensions::StHandleLogDataDynParams(uint8_t format,
                                               uint16_t data_len,
                                               uint8_t* p_data, bool last) {
  static const char fn[] = "NfcStExtensions::StHandleLogDataDynParams";

  if ((format & 0x1) == 0 || data_len < 6) {
    LOG_IF(INFO, nfc_debug_enabled) << fn << ": TLV without timestamp";
    return;
  }

  uint32_t receivedFwts = (p_data[data_len - 4] << 24) |
                          (p_data[data_len - 3] << 16) |
                          (p_data[data_len - 2] << 8) | p_data[data_len - 1];

  switch (mDynFwState) {
    case (DYN_ST_INITIAL): {
      if (p_data[0] == T_fieldLevel) {
        mDynFwErr = 0;
        mDynRotated = 0;
        mDynFwTsT1Started = receivedFwts;
        mDynFwState = DYN_ST_T1_RUNNING;
        mDynFwSubState = DYN_SST_IDLE;
        LOG_IF(INFO, nfc_debug_enabled) << fn << ": Start T1";
      }
    } break;

    case (DYN_ST_T1_RUNNING): {
      switch (mDynFwSubState) {
        case (DYN_SST_IDLE):
          switch (p_data[0]) {
            case T_firstRx:
              // If this is type A, passive mode
              if (p_data[3] == 0x01) {
                // Go to DYN_SST_STARTED
                mDynFwSubState = DYN_SST_STARTED;
                LOG_IF(INFO, nfc_debug_enabled) << fn << ": -> SST_STARTED";
              }
              break;
          }
          break;

        case (DYN_SST_STARTED):
          switch (p_data[0]) {
            case T_dynParamUsed: {
              int fw_cur_set = sRfDynParamSet;
              // check if the set is different from what we programmed.
              if ((format & 0x30) == 0x10) {
                // ST21NFCD
                fw_cur_set = (int)(p_data[2] - 2);
              } else if ((format & 0x30) == 0x20) {
                // ST54J
                fw_cur_set = (int)p_data[2];
              }
              if (fw_cur_set != sRfDynParamSet) {
                LOG_IF(INFO, nfc_debug_enabled)
                    << fn << ": Firmware switched dynamic params";
                mDynFwState = DYN_ST_INITIAL;
                mDynFwTsT1Started = 0;
                mDynFwErr = 0;
              }
            } break;

            case T_fieldLevel: {
              mDynFwErr++;
              LOG_IF(INFO, nfc_debug_enabled)
                  << fn << ": -> SST_IDLE, err=" << mDynFwErr;
              mDynFwSubState = DYN_SST_IDLE;
            } break;

            case T_CERxError: {
              uint8_t errstatus = p_data[4];  // ST21
              if ((format & 0x30) == 0x20) {
                // ST54J
                errstatus = p_data[5];
              }
              if (errstatus != 0x00) {
                // this was an actual error, ignore it
                break;
              }
            }
              // fallback to CERx case if there was no error status.
              // (observer mode)
              U_FALLTHROUGH;
            case T_CERx: {
              LOG_IF(INFO, nfc_debug_enabled)
                  << fn << ": Received data, stop T1";
              mDynFwState = DYN_ST_ACTIVE;
              mDynFwTsT1Started = 0;
            } break;
          }
          break;
      }
    } break;

    case (DYN_ST_ACTIVE): {
      // just wait for T2 expire, even if we receive new T_fieldLevel
    } break;
  }

  // T1 management
  if (last && (mDynFwTsT1Started != 0)) {
    if (FwTsDiffToMs(mDynFwTsT1Started, receivedFwts) > mDynT1Threshold) {
      // T1 elapsed
      LOG_IF(INFO, nfc_debug_enabled) << fn << ": T1 elapsed";
      // restart T1, using the ts of the last event of this ntf is fine.
      mDynFwTsT1Started = receivedFwts;
      // rotate if too many errors received.
      if (mDynFwErr >= mDynErrThreshold) {
        pthread_attr_t pa;
        pthread_t p;
        (void)pthread_attr_init(&pa);
        (void)pthread_attr_setdetachstate(&pa, PTHREAD_CREATE_DETACHED);
        mDynFwErr = 0;
        mDynRotated++;
        LOG_IF(INFO, nfc_debug_enabled)
            << fn << ": Start task to rotate params";
        (void)pthread_create(
            &p, &pa, (THREADFUNCPTR)&NfcStExtensions::rotateRfParameters,
            (void*)false);
      }
    }
  }

  // T2 management
  if (mDynFwTsT2Started != 0) {
    if (last &&
        (FwTsDiffToMs(mDynFwTsT2Started, receivedFwts) > mDynT2Threshold)) {
      // T2 elapsed
      LOG_IF(INFO, nfc_debug_enabled) << fn << ": T2 elapsed";
      mDynFwState = DYN_ST_INITIAL;
      mDynFwTsT1Started = 0;
      mDynFwTsT2Started = 0;
      mDynFwErr = 0;
      if (mDynRotated) {
        pthread_attr_t pa;
        pthread_t p;
        mDynRotated = 0;
        (void)pthread_attr_init(&pa);
        (void)pthread_attr_setdetachstate(&pa, PTHREAD_CREATE_DETACHED);
        LOG_IF(INFO, nfc_debug_enabled) << fn << ": Start task to reset params";
        (void)pthread_create(
            &p, &pa, (THREADFUNCPTR)&NfcStExtensions::rotateRfParameters,
            (void*)true);
      }
    } else if (p_data[0] == T_fieldOn) {
      mDynFwTsT2Started = 0;  // stop T2
    }
  }
  if ((p_data[0] == T_fieldOff) && (mDynFwState != DYN_ST_INITIAL))
    mDynFwTsT2Started = receivedFwts;
}

/*******************************************************************************
**
** Function:        StHandleVsLogData
**
** Description:     H.ndle Vendor-specific logging data
** Returns:         None
**
*******************************************************************************/
void NfcStExtensions::StHandleVsLogData(uint16_t data_len, uint8_t* p_data) {
  static const char fn[] = "NfcStExtensions::StHandleVsLogData";
  int current_tlv_pos = 6;
  int current_tlv_length;
  int idx;
  bool doSendUpper = false;

  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s : data_len=  0x%04X ", fn, data_len);

  for (idx = 0;; ++idx) {
    if (current_tlv_pos + 1 > data_len) break;
    current_tlv_length = p_data[current_tlv_pos + 1] + 2;
    if (current_tlv_pos + current_tlv_length > data_len) break;
    // Parse logs for dynamic parameters mechanism -- FW 1.7+ only
    if (mDynEnabled && (mFwInfo >= 0x01070000)) {
      StHandleLogDataDynParams(
          p_data[3], current_tlv_length, p_data + current_tlv_pos,
          current_tlv_pos + current_tlv_length >= data_len);
    }
    // go to next TLV
    current_tlv_pos = current_tlv_pos + current_tlv_length;
  }  // idx is now the number of TLVs

  // Do we need to send the payload to upper layer?
  {
    SyncEventGuard guard(mVsLogDataEvent);
    doSendUpper = mSendVsLogDataToUpper;
  }
  if (doSendUpper) {
    JNIEnv* e = NULL;
    ScopedAttach attach(mNativeData->vm, &e);
    if (e == NULL) {
      LOG(ERROR) << StringPrintf("jni env is null");
      return;
    }
    ////////////////////////////////////////////////////////////////////////////////
    ScopedLocalRef<jbyteArray> tlv(e, e->NewByteArray(0));
    ScopedLocalRef<jclass> byteArrayClass(e, e->GetObjectClass(tlv.get()));
    ScopedLocalRef<jobjectArray> tlv_list(
        e, e->NewObjectArray(idx, byteArrayClass.get(), 0));

    current_tlv_pos = 6;
    for (idx = 0;; ++idx) {
      if (current_tlv_pos + 1 > data_len) break;
      current_tlv_length = p_data[current_tlv_pos + 1] + 2;
      if (current_tlv_pos + current_tlv_length > data_len) break;
      // Send TLV to upper layer
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s : creating java array for index %d", fn, idx);

      tlv.reset(e->NewByteArray(current_tlv_length));
      e->SetByteArrayRegion(tlv.get(), 0, current_tlv_length,
                            (jbyte*)(p_data + current_tlv_pos));

      e->SetObjectArrayElement(tlv_list.get(), idx, tlv.get());
      // go to next TLV
      current_tlv_pos = current_tlv_pos + current_tlv_length;
    }

    e->CallVoidMethod(mNativeData->manager,
                      android::gCachedNfcManagerNotifyStLogData,
                      (jint)(int)p_data[3], tlv_list.get());
  }
}
/*******************************************************************************
**
** Function:        StVsLogsCallback
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void NfcStExtensions::StVsLogsCallback(tNFC_VS_EVT event, uint16_t data_len,
                                       uint8_t* p_data) {
  static const char fn[] = "NfcStExtensions::StVsLogsCallback";
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s : event = 0x%02X", fn, event);
  if (data_len < 6) {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s : data_len = 0x%X", fn, data_len);
    return;
  }
  if (p_data[1] != 0x02 || p_data[4] != 0x20) {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s : 1:3:4 = 0x%02X:0x%02X:0x%02X", fn, p_data[1],
                        p_data[3], p_data[3]);
    return;  // this is not a firmware log notification
  }
  NfcStExtensions::getInstance().StHandleVsLogData(data_len, p_data);
}
/*******************************************************************************
 **
 ** Function:        StLogManagerEnable
 **
 ** Description:    Enable logging
 **
 ** Returns:         void
 **
 *******************************************************************************/
void NfcStExtensions::StLogManagerEnable(bool enable) {
  static const char fn[] = "StLogManagerEnable";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", fn);

  {
    SyncEventGuard guard(mVsLogDataEvent);
    mSendVsLogDataToUpper = enable;
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
}

/*******************************************************************************
 **
 ** Function:        rotateRfParameters
 **
 ** Description:    Change the default RF settings by rotating in available sets
 **
 ** Returns:         status
 **
 *******************************************************************************/
bool NfcStExtensions::rotateRfParameters(bool reset) {
  bool wasStopped = false;
  uint8_t param[1];
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "Enter :%s  r:%d cur:%d", __func__, reset, sRfDynParamSet);

  // Set the new RF set to use
  if (reset) {
    sRfDynParamSet = 0;
  } else {
    sRfDynParamSet = (++sRfDynParamSet) % 3;
  }

  // Compute the corresponding value for RF_SET_LISTEN_IOT_SEQ
  switch (sRfDynParamSet) {
    case 1:  // rotation 1 : .. 00 10 01
      param[0] = 0x09;
      break;
    case 2:  // rotation 2 : .. 01 00 10
      param[0] = 0x12;
      break;
    case 0:  // default sequence : .. 10 01 00
    default:
      param[0] = 0x24;
      break;
  }

  if (android::isDiscoveryStarted()) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: stop discovery reconfiguring", __func__);
    // Stop RF discovery
    wasStopped = true;
    gIsReconfiguringDiscovery.start();
    android::startRfDiscovery(false);
  }

  tNFA_STATUS status = NFA_SetConfig(NCI_PARAM_ID_PROP_RF_SET_LISTEN_IOT_SEQ,
                                     sizeof(param), &param[0]);

  if (wasStopped) {
    // start discovery
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: reconfigured start discovery", __func__);
    android::startRfDiscovery(true);
    gIsReconfiguringDiscovery.end();
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);

  return (status == NFA_STATUS_OK);
}
