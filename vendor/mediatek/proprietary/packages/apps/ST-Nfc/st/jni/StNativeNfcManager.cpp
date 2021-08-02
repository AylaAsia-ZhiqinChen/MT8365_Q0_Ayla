/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <cutils/properties.h>
#include <errno.h>
#include <nativehelper/ScopedLocalRef.h>
#include <nativehelper/ScopedPrimitiveArray.h>
#include <nativehelper/ScopedUtfChars.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include "StHciEventManager.h"
#include "JavaClassConstants.h"
#include "NfcAdaptation.h"
#include "StNfcJni.h"
#include "StNfcTag.h"
#include "PeerToPeer.h"
#include "NfcStExtensions.h"
#include "StCardEmulationEmbedded.h"
#include "PowerSwitch.h"
#include "StRoutingManager.h"
#include "IntervalTimer.h"
#include "SyncEvent.h"
#include "nfc_config.h"

#include "ce_api.h"
#include "nfa_api.h"
#include "nfa_ee_api.h"
#include "nfa_p2p_api.h"
#include "nfc_brcm_defs.h"

#include "rw_api.h"

using android::base::StringPrintf;

extern tNFA_DM_DISC_FREQ_CFG* p_nfa_dm_rf_disc_freq_cfg;  // defined in stack
namespace android {
extern bool gIsTagDeactivating;
extern bool gIsSelectingRfInterface;
extern void nativeNfcTag_doTransceiveStatus(tNFA_STATUS status, uint8_t* buf,
                                            uint32_t buflen);
extern void nativeNfcTag_notifyRfTimeout();
extern void nativeNfcTag_doConnectStatus(jboolean is_connect_ok);
extern void nativeNfcTag_doDeactivateStatus(int status);
extern void nativeNfcTag_doWriteStatus(jboolean is_write_ok);
extern jboolean nativeNfcTag_doDisconnect(JNIEnv*, jobject);
extern void nativeNfcTag_doCheckNdefResult(tNFA_STATUS status,
                                           uint32_t max_size,
                                           uint32_t current_size,
                                           uint8_t flags);
extern void nativeNfcTag_doMakeReadonlyResult(tNFA_STATUS status);
extern void nativeNfcTag_doPresenceCheckResult(tNFA_STATUS status);
extern void nativeNfcTag_formatStatus(bool is_ok);
extern void nativeNfcTag_resetPresenceCheck();
extern void nativeNfcTag_doReadCompleted(tNFA_STATUS status);
extern void nativeNfcTag_setRfInterface(tNFA_INTF_TYPE rfInterface);
extern void nativeNfcTag_abortWaits();
extern void nativeLlcpConnectionlessSocket_abortWait();
extern void nativeNfcTag_registerNdefTypeHandler();
extern void nativeNfcTag_acquireRfInterfaceMutexLock();
extern void nativeNfcTag_releaseRfInterfaceMutexLock();
extern void nativeLlcpConnectionlessSocket_receiveData(uint8_t* data,
                                                       uint32_t len,
                                                       uint32_t remote_sap);
extern void nativeNfcTag_cacheNonNciCardDetection();
extern void nativeNfcTag_handleNonNciCardDetection(
    tNFA_CONN_EVT_DATA* eventData);
extern void nativeNfcTag_handleNonNciMultiCardDetection(
    uint8_t connEvent, tNFA_CONN_EVT_DATA* eventData);

extern bool getReconnectState(void);

extern uint8_t checkTagNtf;
extern uint8_t checkCmdSent;
}  // namespace android

/*****************************************************************************
**
** public variables and functions
**
*****************************************************************************/
bool gActivated = false;
SyncEvent gDeactivatedEvent;
SyncEvent sNfaSetPowerSubState;
bool gNfccConfigControlStatus = false;

namespace android {
jmethodID gCachedNfcManagerNotifyNdefMessageListeners;
jmethodID gCachedNfcManagerNotifyTransactionListeners;
jmethodID gCachedNfcManagerNotifyLlcpLinkActivation;
jmethodID gCachedNfcManagerNotifyLlcpLinkDeactivated;
jmethodID gCachedNfcManagerNotifyLlcpFirstPacketReceived;
jmethodID gCachedNfcManagerNotifyHostEmuActivated;
jmethodID gCachedNfcManagerNotifyHostEmuData;
jmethodID gCachedNfcManagerNotifyHostEmuDeactivated;
jmethodID gCachedNfcManagerNotifyRfFieldActivated;
jmethodID gCachedNfcManagerNotifyRfFieldDeactivated;
jmethodID gCachedNfcManagerNotifyEeUpdated;
jmethodID gCachedNfcManagerNotifyStLogData;
jmethodID gCachedNfcManagerNotifyDefaultRoutesSet;

const char* gNativeP2pDeviceClassName =
    "com/android/nfc/dhimpl/NativeP2pDevice";
const char* gNativeLlcpServiceSocketClassName =
    "com/android/nfc/dhimpl/NativeLlcpServiceSocket";
const char* gNativeLlcpConnectionlessSocketClassName =
    "com/android/nfc/dhimpl/NativeLlcpConnectionlessSocket";
const char* gNativeLlcpSocketClassName =
    "com/android/nfc/dhimpl/NativeLlcpSocket";
const char* gNativeNfcTagClassName = "com/android/nfc/dhimpl/StNativeNfcTag";
const char* gStNativeNfcManagerClassName =
    "com/android/nfc/dhimpl/StNativeNfcManager";
const char* gStNativeNfcSecureElementClassName =
    "com/android/nfc/dhimpl/StNativeNfcSecureElement";
const char* gNativeNfcStExtensionsClassName =
    "com/android/nfc/dhimpl/NativeNfcStExtensions";
void doStartupConfig();
void startStopPolling(bool isStartPolling);
void startRfDiscovery(bool isStart);
bool isDiscoveryStarted();
void pollingChanged(int discoveryEnabled, int pollingEnabled, int p2pEnabled);
}  // namespace android

/*****************************************************************************
**
** private variables and functions
**
*****************************************************************************/
bool nfc_debug_enabled;

// critical section for methods that stop the discovery temporarily, to avoid
// interleaving.
SyncEvent gIsReconfiguringDiscovery;

namespace android {
static jint sLastError = ERROR_BUFFER_TOO_SMALL;
static SyncEvent sNfaEnableEvent;                // event for NFA_Enable()
static SyncEvent sNfaDisableEvent;               // event for NFA_Disable()
static SyncEvent sNfaEnableDisablePollingEvent;  // event for
                                                 // NFA_EnablePolling(),
                                                 // NFA_DisablePolling()
static SyncEvent sNfaSetConfigEvent;             // event for Set_Config....
static SyncEvent sNfaGetConfigEvent;             // event for Get_Config....
static SyncEvent stimer;  // timer to try to enable again  NFA_Enable()
static bool sIsNfaEnabled = false;
static bool sDiscoveryEnabled = false;  // is polling or listening
static bool sPollingEnabled = false;    // is polling for tag?
static bool sIsDisabling = false;
static bool sRfEnabled = false;   // whether RF discovery is enabled
static bool sSeRfActive = false;  // whether RF with SE is likely active
static bool sReaderModeEnabled =
    false;  // whether we're only reading tags, not allowing P2p/card emu
static bool sP2pEnabled = false;
static bool sP2pActive = false;  // whether p2p was last active
static bool sAbortConnlessWait = false;
static jint sLfT3tMax = 0;
static bool sRoutingInitialized = false;

extern bool scoreGenericNtf;

static jint sWalletTechIsMute = -1;

#define CONFIG_UPDATE_TECH_MASK (1 << 1)
#define DEFAULT_TECH_MASK                                                  \
  (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B | NFA_TECHNOLOGY_MASK_F | \
   NFA_TECHNOLOGY_MASK_V | NFA_TECHNOLOGY_MASK_ACTIVE |                    \
   NFA_TECHNOLOGY_MASK_KOVIO)
#define DEFAULT_DISCOVERY_DURATION 500
#define READER_MODE_DISCOVERY_DURATION 200
#define DEFAULT_ENABLE_TIMER 5000;
static uint16_t ENABLE_TIMER;

static void nfaConnectionCallback(uint8_t event, tNFA_CONN_EVT_DATA* eventData);
static void nfaDeviceManagementCallback(uint8_t event,
                                        tNFA_DM_CBACK_DATA* eventData);
static bool isPeerToPeer(tNFA_ACTIVATED& activated);
static bool isListenMode(tNFA_ACTIVATED& activated);
static tNFA_STATUS stopPolling_rfDiscoveryDisabled();
static tNFA_STATUS startPolling_rfDiscoveryDisabled(
    tNFA_TECHNOLOGY_MASK tech_mask);
static void stNfcManager_doSetScreenState(JNIEnv* e, jobject o,
                                          jint screen_state_mask);

/***P2P-Prio Logic for Multiprotocol***/
static uint8_t multiprotocol_flag = 1;
static uint8_t multiprotocol_detected = 0;
void* p2p_prio_logic_multiprotocol(void* arg);
static IntervalTimer multiprotocol_timer;
pthread_t multiprotocol_thread;
void reconfigure_poll_cb(union sigval);
void clear_multiprotocol();
void multiprotocol_clear_flag(union sigval);

static uint16_t sCurrentConfigLen;
static uint8_t sConfig[256];
static int prevScreenState = NFA_SCREEN_STATE_UNKNOWN;
static int NFA_SCREEN_POLLING_TAG_MASK = 0x10;
static bool gIsDtaEnabled = false;
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// Wallet API
static bool nfcManager_SetMuteTech(JNIEnv* e, jobject o, jboolean muteA,
                                   jboolean muteB, jboolean muteF,
                                   jboolean isCommitNeeded);

namespace {
void initializeGlobalDebugEnabledFlag() {
  nfc_debug_enabled =
      (NfcConfig::getUnsigned(NAME_NFC_DEBUG_ENABLED, 1) != 0) ? true : false;

  char valueStr[PROPERTY_VALUE_MAX] = {0};
  int len = property_get("nfc.debug_enabled", valueStr, "");
  if (len > 0) {
    unsigned debug_enabled = 1;
    // let Android property override .conf variable
    sscanf(valueStr, "%u", &debug_enabled);
    nfc_debug_enabled = (debug_enabled == 0) ? false : true;
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: level=%u", __func__, nfc_debug_enabled);
}
}  // namespace

/*******************************************************************************
**
** Function:        getNative
**
** Description:     Get native data
**
** Returns:         Native data structure.
**
*******************************************************************************/
nfc_jni_native_data* getNative(JNIEnv* e, jobject o) {
  static struct nfc_jni_native_data* sCachedNat = NULL;
  if (e) {
    sCachedNat = nfc_jni_get_nat(e, o);
  }
  return sCachedNat;
}

/*******************************************************************************
**
** Function:        handleRfDiscoveryEvent
**
** Description:     Handle RF-discovery events from the stack.
**                  discoveredDevice: Discovered device.
**
** Returns:         None
**
*******************************************************************************/
static void handleRfDiscoveryEvent(tNFC_RESULT_DEVT* discoveredDevice) {
  int thread_ret;
  if (NULL == discoveredDevice) {
    LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: parameter discoveredDevice can not be null error", __func__);
    return;
  }

  if (discoveredDevice->more == NCI_DISCOVER_NTF_MORE) {
    // there is more discovery notification coming

    if (discoveredDevice->protocol != NFC_PROTOCOL_T1T) {
      NfcTag::getInstance().mNumDiscNtf++;
    }
    return;
  }

  if (discoveredDevice->protocol != NFC_PROTOCOL_T1T) {
    NfcTag::getInstance().mNumDiscNtf++;
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Total Notifications - %d ", __func__,
                      NfcTag::getInstance().mNumDiscNtf);

  if (NfcTag::getInstance().mNumDiscNtf > 1) {
    NfcTag::getInstance().mIsMultiProtocolTag = true;
  }
  bool isP2p = NfcTag::getInstance().isP2pDiscovered();
  if (!sReaderModeEnabled && isP2p) {
    // select the peer that supports P2P
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Select peer device", __func__);
    if (multiprotocol_detected == 1) {
      multiprotocol_timer.kill();
    }

    NfcTag::getInstance().selectP2p();
  } else if (sReaderModeEnabled &&
             NfcTag::getInstance().getP2pDetectedButPausedStatus()) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: Reader mode enabled and P2P detected but paused, cancel "
        "multidetection",
        __func__);
    NfcTag::getInstance().mNumDiscNtf = 0x00;
    NfcTag::getInstance().mIsMultiProtocolTag = false;
    NfcTag::getInstance().selectFirstTag();
  } else if (NfcTag::getInstance().mNumDiscNtf == 0x01) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: Only one tag detected, skip multitag detection", __func__);
    NfcTag::getInstance().mNumDiscNtf = 0x00;
    NfcTag::getInstance().mIsMultiProtocolTag = false;
    NfcTag::getInstance().selectFirstTag();
  } else if (!sReaderModeEnabled && multiprotocol_flag) {
    NfcTag::getInstance().mNumDiscNtf = 0x00;
    multiprotocol_flag = 0;
    multiprotocol_detected = 1;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: starting p2p prio logic for multiprotocol tags", __func__);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    thread_ret = pthread_create(&multiprotocol_thread, &attr,
                                p2p_prio_logic_multiprotocol, NULL);
    if (thread_ret != 0)
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: unable to create the thread", __FUNCTION__);
    pthread_attr_destroy(&attr);
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: starting timer for reconfigure default polling callback",
        __func__);
    multiprotocol_timer.set(300, reconfigure_poll_cb);
  } else {
    // select the first of multiple tags that is discovered
    multiprotocol_flag = 1;
    NfcTag::getInstance().mNumDiscNtf--;
    NfcTag::getInstance().selectFirstTag();
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
}

void* p2p_prio_logic_multiprotocol(void* arg) {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  tNFA_TECHNOLOGY_MASK tech_mask = 0x00;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  /* Do not need if it is already in screen off state */
  //    if (!(getScreenState() & (NFA_SCREEN_STATE_OFF_LOCKED |
  //    NFA_SCREEN_STATE_OFF_UNLOCKED)))
  //    {
  /* Stop polling */
  if (sRfEnabled) {
    startRfDiscovery(false);
  }

  {
    SyncEventGuard guard(sNfaEnableDisablePollingEvent);
    status = NFA_DisablePolling();
    if (status == NFA_STATUS_OK) {
      sNfaEnableDisablePollingEvent.wait();
    } else
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: Failed to disable polling; error=0x%X", __func__, status);
  }

  if (multiprotocol_detected) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: configure polling to tech F only", __func__);
    tech_mask = NFA_TECHNOLOGY_MASK_F;
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: re-configure polling to default", __func__);

    tech_mask =
        NfcConfig::getUnsigned(NAME_POLLING_TECH_MASK, DEFAULT_TECH_MASK);
  }

  {
    SyncEventGuard guard(sNfaEnableDisablePollingEvent);
    status = NFA_EnablePolling(tech_mask);
    if (status == NFA_STATUS_OK) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: wait for enable event", __func__);
      sNfaEnableDisablePollingEvent.wait();
    } else {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: fail enable polling; error=0x%X", __func__, status);
    }
  }

  /* start polling */
  if (!sRfEnabled) {
    startRfDiscovery(true);
  }
  //    }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __FUNCTION__);
  return NULL;
}

void reconfigure_poll_cb(union sigval) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s - Prio_Logic_multiprotocol timer expire", __func__);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s - CallBack Reconfiguring the POLL to Default", __func__);
  clear_multiprotocol();
  multiprotocol_timer.set(300, multiprotocol_clear_flag);
}

void clear_multiprotocol() {
  int thread_ret;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s - clear_multiprotocol", __func__);
  multiprotocol_detected = 0;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  thread_ret = pthread_create(&multiprotocol_thread, &attr,
                              p2p_prio_logic_multiprotocol, NULL);
  if (thread_ret != 0)
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("unable to create the thread");
  pthread_attr_destroy(&attr);
}

void multiprotocol_clear_flag(union sigval) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s - multiprotocol_clear_flag", __func__);
  multiprotocol_flag = 1;
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
                                  tNFA_CONN_EVT_DATA* eventData) {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  static uint8_t prev_more_val = 0x00;
  uint8_t cur_more_val = 0x00;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: event= %u", __func__, connEvent);

  switch (connEvent) {
    case NFA_POLL_ENABLED_EVT:  // whether polling successfully started
    {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_POLL_ENABLED_EVT: status = %u", __func__, eventData->status);

      SyncEventGuard guard(sNfaEnableDisablePollingEvent);
      sNfaEnableDisablePollingEvent.notifyOne();
    }
      NfcStExtensions::getInstance().nfaConnectionCallback(connEvent,
                                                           eventData);
      break;

    case NFA_POLL_DISABLED_EVT:  // Listening/Polling stopped
    {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_POLL_DISABLED_EVT: status = %u", __func__,
                          eventData->status);

      SyncEventGuard guard(sNfaEnableDisablePollingEvent);
      sNfaEnableDisablePollingEvent.notifyOne();
    }
      NfcStExtensions::getInstance().nfaConnectionCallback(connEvent,
                                                           eventData);
      break;

    case NFA_RF_DISCOVERY_STARTED_EVT:  // RF Discovery started
    {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_RF_DISCOVERY_STARTED_EVT: status = %u",
                          __func__, eventData->status);

      SyncEventGuard guard(sNfaEnableDisablePollingEvent);
      sNfaEnableDisablePollingEvent.notifyOne();
    } break;

    case NFA_RF_DISCOVERY_STOPPED_EVT:  // RF Discovery stopped event
    {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_RF_DISCOVERY_STOPPED_EVT: status = %u",
                          __func__, eventData->status);

      if (getReconnectState() == true) {
        eventData->deactivated.type = NFA_DEACTIVATE_TYPE_SLEEP;
        NfcTag::getInstance().setDeactivationState(eventData->deactivated);
        if (gIsTagDeactivating) {
          NfcTag::getInstance().setActive(false);
          nativeNfcTag_doDeactivateStatus(0);
        }
      }

      NfcTag::getInstance().connectionEventHandler(connEvent, eventData);

      gActivated = false;

      SyncEventGuard guard(sNfaEnableDisablePollingEvent);
      sNfaEnableDisablePollingEvent.notifyOne();
    } break;

    case NFA_DISC_RESULT_EVT:  // NFC link/protocol discovery notificaiton
    {
      status = eventData->disc_result.status;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DISC_RESULT_EVT: status = 0x%0X", __func__, status);
      cur_more_val = eventData->disc_result.discovery_ntf.more;
      if ((cur_more_val == 0x01) && (prev_more_val != 0x02)) {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: NFA_DISC_RESULT_EVT: Failed", __func__);
        status = NFA_STATUS_FAILED;
      } else {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: NFA_DISC_RESULT_EVT: Success", __func__);
        status = NFA_STATUS_OK;
        prev_more_val = cur_more_val;
      }
      if (gIsSelectingRfInterface) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: NFA_DISC_RESULT_EVT: reSelect function didn't save the "
            "modification",
            __func__);
        if (cur_more_val == 0x00) {
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: NFA_DISC_RESULT_EVT: error, select any one tag", __func__);
          multiprotocol_flag = 0;
        }
      }

      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DISC_RESULT_EVT: status = %d", __func__, status);
      if (status != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: NFA_DISC_RESULT_EVT error: status = %d",
                                   __func__, status);
        NfcTag::getInstance().mNumDiscNtf = 0;
      } else {
        NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
        handleRfDiscoveryEvent(&eventData->disc_result.discovery_ntf);
      }
    } break;

    case NFA_SELECT_RESULT_EVT:  // NFC link/protocol discovery select response
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_SELECT_RESULT_EVT: status = %d, gIsSelectingRfInterface = "
          "%d, "
          "sIsDisabling=%d",
          __func__, eventData->status, gIsSelectingRfInterface, sIsDisabling);

      if (sIsDisabling) break;

      if (eventData->status != NFA_STATUS_OK) {
        if (gIsSelectingRfInterface) {
          nativeNfcTag_cacheNonNciCardDetection();
          nativeNfcTag_doConnectStatus(false);
        }

        NfcTag::getInstance().selectCompleteStatus(false);
        NfcTag::getInstance().mNumDiscNtf = 0x00;
        NfcTag::getInstance().mTechListIndex = 0;

        LOG(ERROR) << StringPrintf(
            "%s: NFA_SELECT_RESULT_EVT error: status = %d", __func__,
            eventData->status);
        NFA_Deactivate(FALSE);
      }
      break;

    case NFA_DEACTIVATE_FAIL_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DEACTIVATE_FAIL_EVT: status = %d", __func__,
                          eventData->status);
      break;

    case NFA_ACTIVATED_EVT:  // NFC link/protocol activated
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_ACTIVATED_EVT: gIsSelectingRfInterface=%d, sIsDisabling=%d",
          __func__, gIsSelectingRfInterface, sIsDisabling);
      NfcTag::getInstance().selectCompleteStatus(true);

      /***P2P-Prio Logic for Multiprotocol***/
      if ((eventData->activated.activate_ntf.protocol ==
           NFA_PROTOCOL_NFC_DEP) &&
          (multiprotocol_detected == 1)) {
        LOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("Prio_Logic_multiprotocol stop timer");
        multiprotocol_timer.kill();
      }

      if ((eventData->activated.activate_ntf.protocol == NFA_PROTOCOL_T3T) &&
          (multiprotocol_detected == 1)) {
        LOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("Prio_Logic_multiprotocol stop timer");
        multiprotocol_timer.kill();
        clear_multiprotocol();
      }
      if ((eventData->activated.activate_ntf.protocol !=
           NFA_PROTOCOL_NFC_DEP) &&
          (!isListenMode(eventData->activated))) {
        nativeNfcTag_setRfInterface(
            (tNFA_INTF_TYPE)eventData->activated.activate_ntf.intf_param.type);
      }
      NfcTag::getInstance().setActive(true);
      if (sIsDisabling || !sIsNfaEnabled) break;
      gActivated = true;

      NfcTag::getInstance().setActivationState();

      if (gIsSelectingRfInterface) {
        nativeNfcTag_doConnectStatus(true);
        if (NfcTag::getInstance().isCashBeeActivated() == true ||
            NfcTag::getInstance().isEzLinkTagActivated() == true) {
          NfcTag::getInstance().connectionEventHandler(NFA_ACTIVATED_UPDATE_EVT,
                                                       eventData);
        }
        break;
      }

      nativeNfcTag_resetPresenceCheck();
      if (!isListenMode(eventData->activated) &&
          (prevScreenState == NFA_SCREEN_STATE_OFF_LOCKED ||
           prevScreenState == NFA_SCREEN_STATE_OFF_UNLOCKED)) {
        NFA_Deactivate(FALSE);
      }

      if (isPeerToPeer(eventData->activated)) {
        if (sReaderModeEnabled) {
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: ignoring peer target in reader mode.", __func__);
          NFA_Deactivate(FALSE);
          break;
        } else if (NfcStExtensions::getInstance().getP2pPausedStatus() ==
                   true) {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: P2P is paused, deactivating", __func__);
          NFA_Deactivate(FALSE);
          break;
        }
        sP2pActive = true;
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: NFA_ACTIVATED_EVT; is p2p", __func__);
        if (NFC_GetNCIVersion() == NCI_VERSION_1_0) {
          // Disable RF field events in case of p2p
          uint8_t nfa_disable_rf_events[] = {0x00};
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: Disabling RF field events", __func__);
          status = NFA_SetConfig(NCI_PARAM_ID_RF_FIELD_INFO,
                                 sizeof(nfa_disable_rf_events),
                                 &nfa_disable_rf_events[0]);
          if (status == NFA_STATUS_OK) {
            DLOG_IF(INFO, nfc_debug_enabled)
                << StringPrintf("%s: Disabled RF field events", __func__);
          } else {
            LOG(ERROR) << StringPrintf("%s: Failed to disable RF field events",
                                       __func__);
          }
          // Workaround : Notify a field OFF to avoid the UI stays stuck on
          // Field ON last event
          {
            struct nfc_jni_native_data* nat = getNative(NULL, NULL);
            JNIEnv* e = NULL;
            ScopedAttach attach(nat->vm, &e);
            if (e == NULL) {
              LOG(ERROR) << StringPrintf("jni env is null");
              return;
            }
            e->CallVoidMethod(
                nat->manager,
                android::gCachedNfcManagerNotifyRfFieldDeactivated);
          }
        }
      } else {
        // NfcTag::getInstance().connectionEventHandler(connEvent, eventData);

        nativeNfcTag_handleNonNciMultiCardDetection(connEvent, eventData);
        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: scoreGenericNtf = 0x%x", __func__, scoreGenericNtf);

        if (scoreGenericNtf == true) {
          if ((eventData->activated.activate_ntf.intf_param.type ==
               NFC_INTERFACE_ISO_DEP) &&
              (eventData->activated.activate_ntf.protocol ==
               NFC_PROTOCOL_ISO_DEP)) {
            nativeNfcTag_handleNonNciCardDetection(eventData);
          }
          scoreGenericNtf = false;
        }
        // We know it is not activating for P2P.  If it activated in
        // listen mode then it is likely for an SE transaction.
        // Send the RF Event.
        //        if (isListenMode(eventData->activated)) {
        //          sSeRfActive = true;
        //        }
      }
      break;

    case NFA_DEACTIVATED_EVT:  // NFC link/protocol deactivated
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DEACTIVATED_EVT   Type: %u, gIsTagDeactivating: %d",
          __func__, eventData->deactivated.type, gIsTagDeactivating);
      if (checkCmdSent == 1 && eventData->deactivated.type == 0) {
        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: NFA_DEACTIVATED_EVT: Setting check flag  to one", __func__);
        checkTagNtf = 1;
      }

      if (true == getReconnectState()) {
        LOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("Reconnect in progress : Do nothing");
        break;
      }

      /* P2P-priority logic for multiprotocol tags */
      if ((multiprotocol_detected == 1) && (sP2pActive == 1)) {
        NfcTag::getInstance().mNumDiscNtf = 0;
        clear_multiprotocol();
        multiprotocol_flag = 1;
      }

      NfcTag::getInstance().setDeactivationState(eventData->deactivated);

      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s - nb of discovered ntf: %d", __func__,
                          NfcTag::getInstance().mNumDiscNtf);

      if (NfcTag::getInstance().mNumDiscNtf) {
        NfcTag::getInstance().mNumDiscNtf--;
        NfcTag::getInstance().selectNextTag();
      }

      if (eventData->deactivated.type != NFA_DEACTIVATE_TYPE_SLEEP) {
        {
          SyncEventGuard g(gDeactivatedEvent);
          gActivated = false;  // guard this variable from multi-threaded access
          gDeactivatedEvent.notifyOne();
        }

        NfcTag::getInstance().mNumDiscNtf = 0;
        NfcTag::getInstance().mTechListIndex = 0;
        nativeNfcTag_resetPresenceCheck();
        NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
        nativeNfcTag_abortWaits();
        NfcTag::getInstance().abort();
        NfcTag::getInstance().mIsMultiProtocolTag = false;
      } else if (gIsTagDeactivating) {
        NfcTag::getInstance().setActive(false);
        nativeNfcTag_doDeactivateStatus(0);
      }

      // If RF is activated for what we think is a Secure Element transaction
      // and it is deactivated to either IDLE or DISCOVERY mode, notify w/event.
      if ((eventData->deactivated.type == NFA_DEACTIVATE_TYPE_IDLE) ||
          (eventData->deactivated.type == NFA_DEACTIVATE_TYPE_DISCOVERY)) {
        if (sSeRfActive) {
          sSeRfActive = false;
        } else if (sP2pActive) {
          sP2pActive = false;
          // Make sure RF field events are re-enabled
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: NFA_DEACTIVATED_EVT; is p2p", __func__);
          if (NFC_GetNCIVersion() == NCI_VERSION_1_0) {
            // Disable RF field events in case of p2p
            uint8_t nfa_enable_rf_events[] = {0x01};

            if (!sIsDisabling && sIsNfaEnabled) {
              DLOG_IF(INFO, nfc_debug_enabled)
                  << StringPrintf("%s: Enabling RF field events", __func__);
              status = NFA_SetConfig(NCI_PARAM_ID_RF_FIELD_INFO,
                                     sizeof(nfa_enable_rf_events),
                                     &nfa_enable_rf_events[0]);
              if (status == NFA_STATUS_OK) {
                DLOG_IF(INFO, nfc_debug_enabled)
                    << StringPrintf("%s: Enabled RF field events", __func__);
              } else {
                LOG(ERROR) << StringPrintf(
                    "%s: Failed to enable RF field events", __func__);
              }
            }
          }
        }
      }

      break;

    case NFA_TLV_DETECT_EVT:  // TLV Detection complete
      status = eventData->tlv_detect.status;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_TLV_DETECT_EVT: status = %d, protocol = %d, num_tlvs = %d, "
          "num_bytes = %d",
          __func__, status, eventData->tlv_detect.protocol,
          eventData->tlv_detect.num_tlvs, eventData->tlv_detect.num_bytes);
      if (status != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: NFA_TLV_DETECT_EVT error: status = %d",
                                   __func__, status);
      }
      break;

    case NFA_NDEF_DETECT_EVT:  // NDEF Detection complete;
      // if status is failure, it means the tag does not contain any or valid
      // NDEF data;  pass the failure status to the NFC Service;
      status = eventData->ndef_detect.status;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_NDEF_DETECT_EVT: status = 0x%X, protocol = %u, "
          "max_size = %u, cur_size = %u, flags = 0x%X",
          __func__, status, eventData->ndef_detect.protocol,
          eventData->ndef_detect.max_size, eventData->ndef_detect.cur_size,
          eventData->ndef_detect.flags);
      NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
      nativeNfcTag_doCheckNdefResult(status, eventData->ndef_detect.max_size,
                                     eventData->ndef_detect.cur_size,
                                     eventData->ndef_detect.flags);
      break;

    case NFA_DATA_EVT:  // Data message received (for non-NDEF reads)
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DATA_EVT: status = 0x%X, len = %d", __func__,
                          eventData->status, eventData->data.len);
      nativeNfcTag_doTransceiveStatus(eventData->status, eventData->data.p_data,
                                      eventData->data.len);
      break;
    case NFA_RW_INTF_ERROR_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFC_RW_INTF_ERROR_EVT", __func__);
      nativeNfcTag_notifyRfTimeout();
      nativeNfcTag_doReadCompleted(NFA_STATUS_TIMEOUT);
      break;
    case NFA_SELECT_CPLT_EVT:  // Select completed
      status = eventData->status;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_SELECT_CPLT_EVT: status = %d", __func__, status);
      if (status != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: NFA_SELECT_CPLT_EVT error: status = %d",
                                   __func__, status);
      }
      break;

    case NFA_READ_CPLT_EVT:  // NDEF-read or tag-specific-read completed
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_READ_CPLT_EVT: status = 0x%X", __func__, eventData->status);
      nativeNfcTag_doReadCompleted(eventData->status);
      NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
      break;

    case NFA_WRITE_CPLT_EVT:  // Write completed
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_WRITE_CPLT_EVT: status = %d", __func__, eventData->status);
      nativeNfcTag_doWriteStatus(eventData->status == NFA_STATUS_OK);
      break;

    case NFA_SET_TAG_RO_EVT:  // Tag set as Read only
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_SET_TAG_RO_EVT: status = %d", __func__, eventData->status);
      nativeNfcTag_doMakeReadonlyResult(eventData->status);
      break;

    case NFA_CE_NDEF_WRITE_START_EVT:  // NDEF write started
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_CE_NDEF_WRITE_START_EVT: status: %d",
                          __func__, eventData->status);

      if (eventData->status != NFA_STATUS_OK)
        LOG(ERROR) << StringPrintf(
            "%s: NFA_CE_NDEF_WRITE_START_EVT error: status = %d", __func__,
            eventData->status);
      break;

    case NFA_CE_NDEF_WRITE_CPLT_EVT:  // NDEF write completed
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: FA_CE_NDEF_WRITE_CPLT_EVT: len = %u", __func__,
                          eventData->ndef_write_cplt.len);
      break;

    case NFA_LLCP_ACTIVATED_EVT:  // LLCP link is activated
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_LLCP_ACTIVATED_EVT: is_initiator: %d  remote_wks: %d, "
          "remote_lsc: %d, remote_link_miu: %d, local_link_miu: %d",
          __func__, eventData->llcp_activated.is_initiator,
          eventData->llcp_activated.remote_wks,
          eventData->llcp_activated.remote_lsc,
          eventData->llcp_activated.remote_link_miu,
          eventData->llcp_activated.local_link_miu);

      PeerToPeer::getInstance().llcpActivatedHandler(getNative(0, 0),
                                                     eventData->llcp_activated);
      break;

    case NFA_LLCP_DEACTIVATED_EVT:  // LLCP link is deactivated
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_LLCP_DEACTIVATED_EVT", __func__);
      PeerToPeer::getInstance().llcpDeactivatedHandler(
          getNative(0, 0), eventData->llcp_deactivated);
      break;
    case NFA_LLCP_FIRST_PACKET_RECEIVED_EVT:  // Received first packet over llcp
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_LLCP_FIRST_PACKET_RECEIVED_EVT", __func__);
      PeerToPeer::getInstance().llcpFirstPacketHandler(getNative(0, 0));
      break;
    case NFA_PRESENCE_CHECK_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_PRESENCE_CHECK_EVT", __func__);
      nativeNfcTag_doPresenceCheckResult(eventData->status);
      break;
    case NFA_FORMAT_CPLT_EVT:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_FORMAT_CPLT_EVT: status=0x%X", __func__, eventData->status);
      nativeNfcTag_formatStatus(eventData->status == NFA_STATUS_OK);
      break;

    case NFA_I93_CMD_CPLT_EVT:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_I93_CMD_CPLT_EVT: status=0x%X", __func__, eventData->status);
      break;

    case NFA_CE_UICC_LISTEN_CONFIGURED_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_CE_UICC_LISTEN_CONFIGURED_EVT : status=0x%X",
                          __func__, eventData->status);
      StSecureElement::getInstance().connectionEventHandler(connEvent,
                                                            eventData);
      NfcStExtensions::getInstance().nfaConnectionCallback(connEvent,
                                                           eventData);
      break;

    case NFA_SET_P2P_LISTEN_TECH_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_SET_P2P_LISTEN_TECH_EVT", __func__);
      PeerToPeer::getInstance().connectionEventHandler(connEvent, eventData);
      NfcStExtensions::getInstance().nfaConnectionCallback(connEvent,
                                                           eventData);
      break;

    case NFA_LISTEN_ENABLED_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_LISTEN_ENABLED_EVT", __func__);
      NfcStExtensions::getInstance().nfaConnectionCallback(connEvent,
                                                           eventData);
      break;

    case NFA_LISTEN_DISABLED_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_LISTEN_DISABLED_EVT", __func__);
      NfcStExtensions::getInstance().nfaConnectionCallback(connEvent,
                                                           eventData);
      break;

    case NFA_P2P_PAUSED_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_P2P_PAUSED_EVT", __func__);
      NfcStExtensions::getInstance().nfaConnectionCallback(connEvent,
                                                           eventData);
      break;

    case NFA_P2P_RESUMED_EVT:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_P2P_RESUMED_EVT", __func__);
      NfcStExtensions::getInstance().nfaConnectionCallback(connEvent,
                                                           eventData);
      break;

    case NFA_CE_DEREGISTERED_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_CE_DEREGISTERED_EVT", __func__);
      NfcStExtensions::getInstance().nfaConnectionCallback(connEvent,
                                                           eventData);
    } break;
    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: unknown event ????", __func__);
      break;
  }
}

/*******************************************************************************
**
** Function:        nfcManager_initNativeStruc
**
** Description:     Initialize variables.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean stNfcManager_initNativeStruc(JNIEnv* e, jobject o) {
  initializeGlobalDebugEnabledFlag();
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  nfc_jni_native_data* nat =
      (nfc_jni_native_data*)malloc(sizeof(struct nfc_jni_native_data));
  if (nat == NULL) {
    LOG(ERROR) << StringPrintf("%s: fail allocate native data", __func__);
    return JNI_FALSE;
  }

  memset(nat, 0, sizeof(*nat));
  e->GetJavaVM(&(nat->vm));
  nat->env_version = e->GetVersion();
  nat->manager = e->NewGlobalRef(o);

  ScopedLocalRef<jclass> cls(e, e->GetObjectClass(o));
  jfieldID f = e->GetFieldID(cls.get(), "mNative", "J");
  e->SetLongField(o, f, (jlong)nat);

  /* Initialize native cached references */
  gCachedNfcManagerNotifyNdefMessageListeners =
      e->GetMethodID(cls.get(), "notifyNdefMessageListeners",
                     "(Lcom/android/nfc/dhimpl/StNativeNfcTag;)V");
  gCachedNfcManagerNotifyLlcpLinkActivation =
      e->GetMethodID(cls.get(), "notifyLlcpLinkActivation",
                     "(Lcom/android/nfc/dhimpl/NativeP2pDevice;)V");
  gCachedNfcManagerNotifyLlcpLinkDeactivated =
      e->GetMethodID(cls.get(), "notifyLlcpLinkDeactivated",
                     "(Lcom/android/nfc/dhimpl/NativeP2pDevice;)V");
  gCachedNfcManagerNotifyLlcpFirstPacketReceived =
      e->GetMethodID(cls.get(), "notifyLlcpLinkFirstPacketReceived",
                     "(Lcom/android/nfc/dhimpl/NativeP2pDevice;)V");

  gCachedNfcManagerNotifyHostEmuActivated =
      e->GetMethodID(cls.get(), "notifyHostEmuActivated", "(I)V");

  gCachedNfcManagerNotifyHostEmuData =
      e->GetMethodID(cls.get(), "notifyHostEmuData", "(I[B)V");

  gCachedNfcManagerNotifyHostEmuDeactivated =
      e->GetMethodID(cls.get(), "notifyHostEmuDeactivated", "(I)V");

  gCachedNfcManagerNotifyRfFieldActivated =
      e->GetMethodID(cls.get(), "notifyRfFieldActivated", "()V");
  gCachedNfcManagerNotifyRfFieldDeactivated =
      e->GetMethodID(cls.get(), "notifyRfFieldDeactivated", "()V");

  gCachedNfcManagerNotifyTransactionListeners = e->GetMethodID(
      cls.get(), "notifyTransactionListeners", "([B[BLjava/lang/String;)V");

  gCachedNfcManagerNotifyEeUpdated =
      e->GetMethodID(cls.get(), "notifyEeUpdated", "()V");

  gCachedNfcManagerNotifyDefaultRoutesSet =
      e->GetMethodID(cls.get(), "notifyDefaultRoutesSet", "(IIIIII)V");

  if (nfc_jni_cache_object(e, gNativeNfcTagClassName, &(nat->cached_NfcTag)) ==
      -1) {
    LOG(ERROR) << StringPrintf("%s: fail cache NativeNfcTag", __func__);
    return JNI_FALSE;
  }

  if (nfc_jni_cache_object(e, gNativeP2pDeviceClassName,
                           &(nat->cached_P2pDevice)) == -1) {
    LOG(ERROR) << StringPrintf("%s: fail cache NativeP2pDevice", __func__);
    return JNI_FALSE;
  }

  gCachedNfcManagerNotifyStLogData =
      e->GetMethodID(cls.get(), "notifyStLogData", "(I[[B)V");
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return JNI_TRUE;
}

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
void nfaDeviceManagementCallback(uint8_t dmEvent,
                                 tNFA_DM_CBACK_DATA* eventData) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; event=0x%X", __func__, dmEvent);

  switch (dmEvent) {
    case NFA_DM_ENABLE_EVT: /* Result of NFA_Enable */
    {
      SyncEventGuard guard(sNfaEnableEvent);
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DM_ENABLE_EVT; status=0x%X", __func__, eventData->status);
      sIsNfaEnabled = eventData->status == NFA_STATUS_OK;
      sIsDisabling = false;
      sNfaEnableEvent.notifyOne();
      NfcStExtensions::getInstance().setCoreResetNtfInfo(
          eventData->enable.manu_specific_info);
    } break;

    case NFA_DM_DISABLE_EVT: /* Result of NFA_Disable */
    {
      SyncEventGuard guard(sNfaDisableEvent);
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DM_DISABLE_EVT", __func__);
      sIsNfaEnabled = false;
      sIsDisabling = false;
      sNfaDisableEvent.notifyOne();
    } break;

    case NFA_DM_SET_CONFIG_EVT:  // result of NFA_SetConfig
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DM_SET_CONFIG_EVT", __func__);
      {
        SyncEventGuard guard(sNfaSetConfigEvent);
        sNfaSetConfigEvent.notifyOne();
        NfcStExtensions::getInstance().notifyNciConfigCompletion(false, 0,
                                                                 NULL);
      }
      break;

    case NFA_DM_GET_CONFIG_EVT: /* Result of NFA_GetConfig */
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DM_GET_CONFIG_EVT", __func__);
      {
        SyncEventGuard guard(sNfaGetConfigEvent);
        if (eventData->status == NFA_STATUS_OK &&
            eventData->get_config.tlv_size <= sizeof(sConfig)) {
          sCurrentConfigLen = eventData->get_config.tlv_size;
          memcpy(sConfig, eventData->get_config.param_tlvs,
                 eventData->get_config.tlv_size);
        } else {
          LOG(ERROR) << StringPrintf("%s: NFA_DM_GET_CONFIG failed", __func__);
          sCurrentConfigLen = 0;
        }
        sNfaGetConfigEvent.notifyOne();
        NfcStExtensions::getInstance().notifyNciConfigCompletion(
            true, sCurrentConfigLen, sConfig);
      }
      break;

    case NFA_DM_RF_FIELD_EVT:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DM_RF_FIELD_EVT; status=0x%X; field status=%u", __func__,
          eventData->rf_field.status, eventData->rf_field.rf_field_status);
      if (sIsNfaEnabled && !sP2pActive &&
          eventData->rf_field.status == NFA_STATUS_OK) {
        struct nfc_jni_native_data* nat = getNative(NULL, NULL);
        JNIEnv* e = NULL;
        ScopedAttach attach(nat->vm, &e);
        if (e == NULL) {
          LOG(ERROR) << StringPrintf("jni env is null");
          return;
        }
        if (eventData->rf_field.rf_field_status == NFA_DM_RF_FIELD_ON)
          e->CallVoidMethod(nat->manager,
                            android::gCachedNfcManagerNotifyRfFieldActivated);
        else
          e->CallVoidMethod(nat->manager,
                            android::gCachedNfcManagerNotifyRfFieldDeactivated);
      }
      break;

    case NFA_DM_NFCC_TRANSPORT_ERR_EVT:
    case NFA_DM_NFCC_TIMEOUT_EVT: {
      if (dmEvent == NFA_DM_NFCC_TIMEOUT_EVT)
        LOG(ERROR) << StringPrintf("%s: NFA_DM_NFCC_TIMEOUT_EVT; abort",
                                   __func__);
      else if (dmEvent == NFA_DM_NFCC_TRANSPORT_ERR_EVT)
        LOG(ERROR) << StringPrintf("%s: NFA_DM_NFCC_TRANSPORT_ERR_EVT; abort",
                                   __func__);

      nativeNfcTag_abortWaits();
      NfcTag::getInstance().abort();
      sAbortConnlessWait = true;
      nativeLlcpConnectionlessSocket_abortWait();
      {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: aborting  sNfaEnableDisablePollingEvent", __func__);
        SyncEventGuard guard(sNfaEnableDisablePollingEvent);
        sNfaEnableDisablePollingEvent.notifyOne();
      }
      {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: aborting  sNfaEnableEvent", __func__);
        SyncEventGuard guard(sNfaEnableEvent);
        sNfaEnableEvent.notifyOne();
      }
      {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: aborting  sNfaDisableEvent", __func__);
        SyncEventGuard guard(sNfaDisableEvent);
        sNfaDisableEvent.notifyOne();
      }
      sDiscoveryEnabled = false;
      sPollingEnabled = false;
      PowerSwitch::getInstance().abort();

      if (!sIsDisabling && sIsNfaEnabled) {
        //       EXTNS_Close();
        NFA_Disable(FALSE);
        sIsDisabling = true;
      } else {
        sIsNfaEnabled = false;
        sIsDisabling = false;
      }
      PowerSwitch::getInstance().initialize(PowerSwitch::UNKNOWN_LEVEL);
      LOG(ERROR) << StringPrintf("%s: crash NFC service", __func__);
      //////////////////////////////////////////////
      // crash the NFC service process so it can restart automatically
      abort();
      //////////////////////////////////////////////
    } break;

    case NFA_DM_PWR_MODE_CHANGE_EVT:
      PowerSwitch::getInstance().deviceManagementCallback(dmEvent, eventData);
      break;

    case NFA_DM_SET_POWER_SUB_STATE_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_DM_SET_POWER_SUB_STATE_EVT; status=0x%X",
                          __FUNCTION__, eventData->power_sub_state.status);
      SyncEventGuard guard(sNfaSetPowerSubState);
      sNfaSetPowerSubState.notifyOne();
    } break;
    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: unhandled event", __func__);
      break;
  }
}

/*******************************************************************************
**
** Function:        nfcManager_sendRawFrame
**
** Description:     Send a raw frame.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean stNfcManager_sendRawFrame(JNIEnv* e, jobject, jbyteArray data) {
  ScopedByteArrayRO bytes(e, data);
  uint8_t* buf =
      const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));
  size_t bufLen = bytes.size();
  tNFA_STATUS status = NFA_SendRawFrame(buf, bufLen, 0);

  return (status == NFA_STATUS_OK);
}

/*******************************************************************************
**
** Function:        nfcManager_routeAid
**
** Description:     Route an AID to an EE
**                  e: JVM environment.
**                  aid: aid to be added to routing table.
**                  route: aid route location. i.e. DH/eSE/UICC
**                  aidInfo: prefix or suffix aid.
**
** Returns:         True if aid is accpted by NFA Layer.
**
*******************************************************************************/
static jboolean stNfcManager_routeAid(JNIEnv* e, jobject, jbyteArray aid,
                                      jint route, jint aidInfo) {
  uint8_t* buf;
  size_t bufLen;

  if (aid == NULL) {
    buf = NULL;
    bufLen = 0;
  } else {
    ScopedByteArrayRO bytes(e, aid);
    buf = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));
    bufLen = bytes.size();
  }
  return StRoutingManager::getInstance().addAidRouting(buf, bufLen, route,
                                                       aidInfo);
}

/*******************************************************************************
**
** Function:        nfcManager_unrouteAid
**
** Description:     Remove a AID routing
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean stNfcManager_unrouteAid(JNIEnv* e, jobject, jbyteArray aid) {
  uint8_t* buf;
  size_t bufLen;

  if (aid == NULL) {
    buf = NULL;
    bufLen = 0;
  } else {
    ScopedByteArrayRO bytes(e, aid);
    buf = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));
    bufLen = bytes.size();
  }
  bool result = StRoutingManager::getInstance().removeAidRouting(buf, bufLen);
  return result;
}

/*******************************************************************************
**
** Function:        stNfcManager_commitRouting
**
** Description:     Sends the AID routing table to the controller
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean stNfcManager_commitRouting(JNIEnv* e, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << __func__;

  bool wasDiscoveryEnabled = true;

  if (sRfEnabled) {
    /*Update routing table only in Idle state.*/
    gIsReconfiguringDiscovery.start();
    startRfDiscovery(false);
  } else {
    wasDiscoveryEnabled = false;
  }
  jboolean commitStatus = StRoutingManager::getInstance().commitRouting();
  if (wasDiscoveryEnabled) {
    startRfDiscovery(true);
    gIsReconfiguringDiscovery.end();
  }
  return commitStatus;
}

/*******************************************************************************
**
** Function:        nfcManager_doRegisterT3tIdentifier
**
** Description:     Registers LF_T3T_IDENTIFIER for NFC-F.
**                  e: JVM environment.
**                  o: Java object.
**                  t3tIdentifier: LF_T3T_IDENTIFIER value (10 or 18 bytes)
**
** Returns:         Handle retrieve from RoutingManager.
**
*******************************************************************************/
static jint StNfcManager_doRegisterT3tIdentifier(JNIEnv* e, jobject,
                                                 jbyteArray t3tIdentifier) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  ScopedByteArrayRO bytes(e, t3tIdentifier);
  uint8_t* buf =
      const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(&bytes[0]));
  size_t bufLen = bytes.size();
  int handle =
      StRoutingManager::getInstance().registerT3tIdentifier(buf, bufLen);

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: handle=%d", __func__, handle);
  //  if (handle != NFA_HANDLE_INVALID)
  //    StRoutingManager::getInstance().commitRouting();
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);

  return handle;
}

/*******************************************************************************
**
** Function:        nfcManager_doDeregisterT3tIdentifier
**
** Description:     Deregisters LF_T3T_IDENTIFIER for NFC-F.
**                  e: JVM environment.
**                  o: Java object.
**                  handle: Handle retrieve from libnfc-nci.
**
** Returns:         None
**
*******************************************************************************/
static void StNfcManager_doDeregisterT3tIdentifier(JNIEnv*, jobject,
                                                   jint handle) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; handle=%d", __func__, handle);

  StRoutingManager::getInstance().deregisterT3tIdentifier(handle);
  //  StRoutingManager::getInstance().commitRouting();

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
}

/*******************************************************************************
**
** Function:        nfcManager_getLfT3tMax
**
** Description:     Returns LF_T3T_MAX value.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         LF_T3T_MAX value.
**
*******************************************************************************/
static jint StNfcManager_getLfT3tMax(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("LF_T3T_MAX=%d", sLfT3tMax);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);

  return sLfT3tMax;
}

/*******************************************************************************
**
** Function:        nfcManager_doInitialize
**
** Description:     Turn on NFC.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean stNfcManager_doInitialize(JNIEnv* e, jobject o) {
  initializeGlobalDebugEnabledFlag();
  tNFA_STATUS stat = NFA_STATUS_OK;
  int num;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  PowerSwitch& powerSwitch = PowerSwitch::getInstance();

  if (sIsNfaEnabled) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: already enabled", __func__);
    goto TheEnd;
  }

  NfcTag::getInstance().mNfcDisableinProgress = false;

  powerSwitch.initialize(PowerSwitch::FULL_POWER);

  {
    NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
    theInstance.Initialize();  // start GKI, NCI task, NFC task

    {
      SyncEventGuard guard(sNfaEnableEvent);
      tHAL_NFC_ENTRY* halFuncEntries = theInstance.GetHalEntryFuncs();

      NFA_Init(halFuncEntries);

      stat = NFA_Enable(nfaDeviceManagementCallback, nfaConnectionCallback);
      if (stat == NFA_STATUS_OK) {
        sNfaEnableEvent.wait();  // wait for NFA command to finish
      }
    }

    if (stat == NFA_STATUS_OK && sIsNfaEnabled == false) {
      num = NfcConfig::getUnsigned("RE_ENABLE_TIMER", 500);
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: ENABLE_TIMER = %d ", __func__, ENABLE_TIMER);

      SyncEventGuard guard2(stimer);
      if (stimer.wait(ENABLE_TIMER) == false)  // if timeout occurred
      {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: timeout waiting for RENABLE DM", __func__);
      }
      {
        SyncEventGuard guard(sNfaEnableEvent);
        stat = NFA_Enable(nfaDeviceManagementCallback, nfaConnectionCallback);
        sNfaEnableEvent.wait();  // wait for NFA command to finish
      }
    }

    if (stat == NFA_STATUS_OK) {
      // sIsNfaEnabled indicates whether stack started successfully
      if (sIsNfaEnabled) {
        StSecureElement::getInstance().initialize(getNative(e, o));
        StCardEmulationEmbedded::getInstance().initialize(getNative(e, o));
        sRoutingInitialized =
            StRoutingManager::getInstance().initialize(getNative(e, o));

        NfcStExtensions::getInstance().initialize(getNative(e, o));
        nativeNfcTag_registerNdefTypeHandler();
        NfcTag::getInstance().initialize(getNative(e, o));
        PeerToPeer::getInstance().initialize();
        PeerToPeer::getInstance().handleNfcOnOff(true);
        StHciEventManager::getInstance().initialize(getNative(e, o));
        //        NFA_SetMuteTech(false,false,false); // init global val
        sWalletTechIsMute = 0;

        /////////////////////////////////////////////////////////////////////////////////
        // Add extra configuration here (work-arounds, etc.)

        if (gIsDtaEnabled == true) {
          uint8_t configData = 0;
          configData = 0x01; /* Poll NFC-DEP : Highest Available Bit Rates */
          NFA_SetConfig(NCI_PARAM_ID_BITR_NFC_DEP, sizeof(uint8_t),
                        &configData);
          configData = 0x0B; /* Listen NFC-DEP : Waiting Time */
          NFA_SetConfig(NFC_PMID_WT, sizeof(uint8_t), &configData);
          configData = 0x0F; /* Specific Parameters for NFC-DEP RF Interface */
          NFA_SetConfig(NCI_PARAM_ID_NFC_DEP_OP, sizeof(uint8_t), &configData);
        }

        struct nfc_jni_native_data* nat = getNative(e, o);
        if (nat) {
          nat->tech_mask =
              NfcConfig::getUnsigned(NAME_POLLING_TECH_MASK, DEFAULT_TECH_MASK);
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: tag polling tech mask=0x%X", __func__, nat->tech_mask);
        }

        // if this value exists, set polling interval.
        nat->discovery_duration = NfcConfig::getUnsigned(
            NAME_NFA_DM_DISC_DURATION_POLL, DEFAULT_DISCOVERY_DURATION);

        NFA_SetRfDiscoveryDuration(nat->discovery_duration);

        // get LF_T3T_MAX
        {
          SyncEventGuard guard(sNfaGetConfigEvent);
          tNFA_PMID configParam[1] = {NCI_PARAM_ID_LF_T3T_MAX};
          stat = NFA_GetConfig(1, configParam);
          if (stat == NFA_STATUS_OK) {
            sNfaGetConfigEvent.wait();
            if (sCurrentConfigLen >= 4 ||
                sConfig[1] == NCI_PARAM_ID_LF_T3T_MAX) {
              DLOG_IF(INFO, nfc_debug_enabled)
                  << StringPrintf("%s: lfT3tMax=%d", __func__, sConfig[3]);
              sLfT3tMax = sConfig[3];
            }
          }
        }

        // force update for power sub state at start
        // Will be updated by upper layer at boot (EnableInternal)
        prevScreenState = NFA_SCREEN_STATE_UNKNOWN;

        // Do custom NFCA startup configuration.
        doStartupConfig();
        goto TheEnd;
      }
    }

    LOG(ERROR) << StringPrintf("%s: fail nfa enable; error=0x%X", __func__,
                               stat);

    if (sIsNfaEnabled) stat = NFA_Disable(FALSE /* ungraceful */);

    theInstance.Finalize();
  }

TheEnd:
  if (sIsNfaEnabled)
    PowerSwitch::getInstance().setLevel(PowerSwitch::LOW_POWER);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return sIsNfaEnabled ? JNI_TRUE : JNI_FALSE;
}

static void stNfcManager_doEnableDtaMode(JNIEnv*, jobject) {
  gIsDtaEnabled = true;
}

static void stNfcManager_doDisableDtaMode(JNIEnv*, jobject) {
  gIsDtaEnabled = false;
}

static void stNfcManager_doFactoryReset(JNIEnv*, jobject) {
  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  theInstance.FactoryReset();
}

static void stNfcManager_doShutdown(JNIEnv*, jobject) {
  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  theInstance.DeviceShutdown();
}

static void stNfcManager_configNfccConfigControl(bool flag) {
  // configure NFCC_CONFIG_CONTROL- NFCC allowed to manage RF configuration.
  if (NFC_GetNCIVersion() != NCI_VERSION_1_0) {
    uint8_t nfa_set_config[] = {0x00};

    nfa_set_config[0] = (flag == true ? 1 : 0);

    gNfccConfigControlStatus = flag;

    SyncEventGuard guard(sNfaSetConfigEvent);

    tNFA_STATUS status =
        NFA_SetConfig(NCI_PARAM_ID_NFCC_CONFIG_CONTROL, sizeof(nfa_set_config),
                      &nfa_set_config[0]);

    if (status != NFA_STATUS_OK) {
      LOG(ERROR) << __func__ << ": Failed to configure NFCC_CONFIG_CONTROL";
    } else {
      sNfaSetConfigEvent.wait();
    }
  }
}
/*******************************************************************************
**
** Function:        stNfcManager_enableDiscovery
**
** Description:     Start polling and listening for devices.
**                  e: JVM environment.
**                  o: Java object.
**                  technologies_mask: the bitmask of technologies for which to
*enable discovery
**                  enable_lptd: whether to enable low power polling (default:
*false)
**
** Returns:         None
**
*******************************************************************************/
static void stNfcManager_enableDiscovery(
    JNIEnv* e, jobject o, jint technologies_mask, jboolean enable_lptd,
    jboolean reader_mode, jboolean enable_host_routing, jboolean enable_p2p,
    jboolean restart) {
  tNFA_TECHNOLOGY_MASK tech_mask = DEFAULT_TECH_MASK;
  struct nfc_jni_native_data* nat = getNative(e, o);

  if (technologies_mask == -1 && nat)
    tech_mask = (tNFA_TECHNOLOGY_MASK)nat->tech_mask;
  else if (technologies_mask != -1)
    tech_mask = (tNFA_TECHNOLOGY_MASK)technologies_mask;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter; tech_mask = %02x, enable_host_routing: %d, "
      "enable_p2p: %d, restart: %d",
      __func__, tech_mask, enable_host_routing, enable_p2p, restart);

  if (sDiscoveryEnabled && !restart) {
    LOG(ERROR) << StringPrintf("%s: already discovering", __func__);
    return;
  }

  PowerSwitch::getInstance().setLevel(PowerSwitch::FULL_POWER);

  if (sRfEnabled) {
    // Stop RF discovery to reconfigure
    startRfDiscovery(false);
  }

  // Check polling configuration
  if (tech_mask != 0) {
    stopPolling_rfDiscoveryDisabled();
    startPolling_rfDiscoveryDisabled(tech_mask);

    // Start P2P listening if tag polling was enabled
    if (sPollingEnabled) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Enable p2pListening", __func__);

      if (enable_p2p && !sP2pEnabled) {
        sP2pEnabled = true;
        PeerToPeer::getInstance().enableP2pListening(true);
        NFA_ResumeP2p();
      } else if (!enable_p2p && sP2pEnabled) {
        sP2pEnabled = false;
        PeerToPeer::getInstance().enableP2pListening(false);
        NFA_PauseP2p();
      }

      if (reader_mode && !sReaderModeEnabled) {
        sReaderModeEnabled = true;
        NFA_DisableListening();

        // configure NFCC_CONFIG_CONTROL- NFCC not allowed to manage RF
        // configuration.
        stNfcManager_configNfccConfigControl(false);

        NFA_SetRfDiscoveryDuration(READER_MODE_DISCOVERY_DURATION);
      } else if (!reader_mode && sReaderModeEnabled) {
        struct nfc_jni_native_data* nat = getNative(e, o);
        sReaderModeEnabled = false;
        NFA_EnableListening();

        // configure NFCC_CONFIG_CONTROL- NFCC allowed to manage RF
        // configuration.
        if (gNfccConfigControlStatus == false) {
          stNfcManager_configNfccConfigControl(true);
        }

        NFA_SetRfDiscoveryDuration(nat->discovery_duration);
      }
    }
  } else {
    // No technologies configured, stop polling
    stopPolling_rfDiscoveryDisabled();
  }

  // Check listen configuration
  if (enable_host_routing) {
    StRoutingManager::getInstance().enableRoutingToHost();
    StRoutingManager::getInstance().commitRouting();
  } else {
    StRoutingManager::getInstance().disableRoutingToHost();
    StRoutingManager::getInstance().commitRouting();
  }
  // Actually start discovery.
  startRfDiscovery(true);
  sDiscoveryEnabled = true;

  PowerSwitch::getInstance().setModeOn(PowerSwitch::DISCOVERY);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
}

/*******************************************************************************
**
** Function:        nfcManager_disableDiscovery
**
** Description:     Stop polling and listening for devices.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         None
**
*******************************************************************************/
void stNfcManager_disableDiscovery(JNIEnv* e, jobject o) {
  tNFA_STATUS status = NFA_STATUS_OK;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  if (sDiscoveryEnabled == false) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: already disabled", __func__);
    goto TheEnd;
  }

  // Stop RF Discovery.
  startRfDiscovery(false);

  if (sPollingEnabled) status = stopPolling_rfDiscoveryDisabled();

  PeerToPeer::getInstance().enableP2pListening(false);
  sP2pEnabled = false;
  sDiscoveryEnabled = false;
  // if nothing is active after this, then tell the controller to power down
  if (!PowerSwitch::getInstance().setModeOff(PowerSwitch::DISCOVERY))
    PowerSwitch::getInstance().setLevel(PowerSwitch::LOW_POWER);
TheEnd:
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
}

/*******************************************************************************
**
** Function:        stNfcManager_doCreateLlcpServiceSocket
**
** Description:     Create a new LLCP server socket.
**                  e: JVM environment.
**                  o: Java object.
**                  nSap: Service access point.
**                  sn: Service name
**                  miu: Maximum information unit.
**                  rw: Receive window size.
**                  linearBufferLength: Max buffer size.
**
** Returns:         NativeLlcpServiceSocket Java object.
**
*******************************************************************************/
static jobject stNfcManager_doCreateLlcpServiceSocket(JNIEnv* e, jobject,
                                                      jint nSap, jstring sn,
                                                      jint miu, jint rw,
                                                      jint linearBufferLength) {
  PeerToPeer::tJNI_HANDLE jniHandle =
      PeerToPeer::getInstance().getNewJniHandle();

  ScopedUtfChars serviceName(e, sn);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter: sap=%i; name=%s; miu=%i; rw=%i; buffLen=%i", __func__, nSap,
      serviceName.c_str(), miu, rw, linearBufferLength);

  /* Create new NativeLlcpServiceSocket object */
  jobject serviceSocket = NULL;
  if (nfc_jni_cache_object_local(e, gNativeLlcpServiceSocketClassName,
                                 &(serviceSocket)) == -1) {
    LOG(ERROR) << StringPrintf("%s: Llcp socket object creation error",
                               __func__);
    return NULL;
  }

  /* Get NativeLlcpServiceSocket class object */
  ScopedLocalRef<jclass> clsNativeLlcpServiceSocket(
      e, e->GetObjectClass(serviceSocket));
  if (e->ExceptionCheck()) {
    e->ExceptionClear();
    LOG(ERROR) << StringPrintf("%s: Llcp Socket get object class error",
                               __func__);
    return NULL;
  }

  if (!PeerToPeer::getInstance().registerServer(jniHandle,
                                                serviceName.c_str())) {
    LOG(ERROR) << StringPrintf("%s: RegisterServer error", __func__);
    return NULL;
  }

  jfieldID f;

  /* Set socket handle to be the same as the NfaHandle*/
  f = e->GetFieldID(clsNativeLlcpServiceSocket.get(), "mHandle", "I");
  e->SetIntField(serviceSocket, f, (jint)jniHandle);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: socket Handle = 0x%X", __func__, jniHandle);

  /* Set socket linear buffer length */
  f = e->GetFieldID(clsNativeLlcpServiceSocket.get(),
                    "mLocalLinearBufferLength", "I");
  e->SetIntField(serviceSocket, f, (jint)linearBufferLength);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: buffer length = %d", __func__, linearBufferLength);

  /* Set socket MIU */
  f = e->GetFieldID(clsNativeLlcpServiceSocket.get(), "mLocalMiu", "I");
  e->SetIntField(serviceSocket, f, (jint)miu);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: MIU = %d", __func__, miu);

  /* Set socket RW */
  f = e->GetFieldID(clsNativeLlcpServiceSocket.get(), "mLocalRw", "I");
  e->SetIntField(serviceSocket, f, (jint)rw);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s:  RW = %d", __func__, rw);

  sLastError = 0;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return serviceSocket;
}

/*******************************************************************************
**
** Function:        nfcManager_doGetLastError
**
** Description:     Get the last error code.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Last error code.
**
*******************************************************************************/
static jint stNfcManager_doGetLastError(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: last error=%i", __func__, sLastError);
  return sLastError;
}

/*******************************************************************************
**
** Function:        nfcManager_doDeinitialize
**
** Description:     Turn off NFC.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean stNfcManager_doDeinitialize(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  sIsDisabling = true;

  StRoutingManager::getInstance().onNfccShutdown();
  StSecureElement::getInstance().finalize();
  PowerSwitch::getInstance().initialize(PowerSwitch::UNKNOWN_LEVEL);
  StHciEventManager::getInstance().finalize();
  NfcStExtensions::getInstance().finalize();

  if (sIsNfaEnabled) {
    SyncEventGuard guard(sNfaDisableEvent);
    if (multiprotocol_detected == 1) {
      multiprotocol_timer.kill();
    }

    tNFA_STATUS stat = NFA_Disable(TRUE /* graceful */);
    if (stat == NFA_STATUS_OK) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: wait for completion", __func__);
      sNfaDisableEvent.wait();  // wait for NFA command to finish
      PeerToPeer::getInstance().handleNfcOnOff(false);
    } else {
      LOG(ERROR) << StringPrintf("%s: fail disable; error=0x%X", __func__,
                                 stat);
    }
  }
  NfcTag::getInstance().mNfcDisableinProgress = true;
  nativeNfcTag_abortWaits();
  NfcTag::getInstance().abort();
  sAbortConnlessWait = true;
  nativeLlcpConnectionlessSocket_abortWait();
  sIsNfaEnabled = false;
  sDiscoveryEnabled = false;
  sPollingEnabled = false;
  sIsDisabling = false;
  sP2pEnabled = false;
  gActivated = false;
  sRfEnabled = false;
  sLfT3tMax = 0;

  {
    // unblock NFA_EnablePolling() and NFA_DisablePolling()
    SyncEventGuard guard(sNfaEnableDisablePollingEvent);
    sNfaEnableDisablePollingEvent.notifyOne();
  }

  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  theInstance.Finalize();
  // NFA_SetMuteTech(false, false, false);  // clear global val

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return JNI_TRUE;
}

/*******************************************************************************
**
** Function:        nfcManager_doCreateLlcpSocket
**
** Description:     Create a LLCP connection-oriented socket.
**                  e: JVM environment.
**                  o: Java object.
**                  nSap: Service access point.
**                  miu: Maximum information unit.
**                  rw: Receive window size.
**                  linearBufferLength: Max buffer size.
**
** Returns:         NativeLlcpSocket Java object.
**
*******************************************************************************/
static jobject stNfcManager_doCreateLlcpSocket(JNIEnv* e, jobject, jint nSap,
                                               jint miu, jint rw,
                                               jint linearBufferLength) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; sap=%d; miu=%d; rw=%d; buffer len=%d",
                      __func__, nSap, miu, rw, linearBufferLength);

  PeerToPeer::tJNI_HANDLE jniHandle =
      PeerToPeer::getInstance().getNewJniHandle();
  PeerToPeer::getInstance().createClient(jniHandle, miu, rw);

  /* Create new NativeLlcpSocket object */
  jobject clientSocket = NULL;
  if (nfc_jni_cache_object_local(e, gNativeLlcpSocketClassName,
                                 &(clientSocket)) == -1) {
    LOG(ERROR) << StringPrintf("%s: fail Llcp socket creation", __func__);
    return clientSocket;
  }

  /* Get NativeConnectionless class object */
  ScopedLocalRef<jclass> clsNativeLlcpSocket(e,
                                             e->GetObjectClass(clientSocket));
  if (e->ExceptionCheck()) {
    e->ExceptionClear();
    LOG(ERROR) << StringPrintf("%s: fail get class object", __func__);
    return clientSocket;
  }

  jfieldID f;

  /* Set socket SAP */
  f = e->GetFieldID(clsNativeLlcpSocket.get(), "mSap", "I");
  e->SetIntField(clientSocket, f, (jint)nSap);

  /* Set socket handle */
  f = e->GetFieldID(clsNativeLlcpSocket.get(), "mHandle", "I");
  e->SetIntField(clientSocket, f, (jint)jniHandle);

  /* Set socket MIU */
  f = e->GetFieldID(clsNativeLlcpSocket.get(), "mLocalMiu", "I");
  e->SetIntField(clientSocket, f, (jint)miu);

  /* Set socket RW */
  f = e->GetFieldID(clsNativeLlcpSocket.get(), "mLocalRw", "I");
  e->SetIntField(clientSocket, f, (jint)rw);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return clientSocket;
}

/*******************************************************************************
**
** Function:        stNfcManager_doCreateLlcpConnectionlessSocket
**
** Description:     Create a connection-less socket.
**                  e: JVM environment.
**                  o: Java object.
**                  nSap: Service access point.
**                  sn: Service name.
**
** Returns:         NativeLlcpConnectionlessSocket Java object.
**
*******************************************************************************/
static jobject stNfcManager_doCreateLlcpConnectionlessSocket(JNIEnv*, jobject,
                                                             jint nSap,
                                                             jstring /*sn*/) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: nSap=0x%X", __func__, nSap);
  return NULL;
}

/*******************************************************************************
**
** Function:        isPeerToPeer
**
** Description:     Whether the activation data indicates the peer supports
*NFC-DEP.
**                  activated: Activation data.
**
** Returns:         True if the peer supports NFC-DEP.
**
*******************************************************************************/
static bool isPeerToPeer(tNFA_ACTIVATED& activated) {
  return activated.activate_ntf.protocol == NFA_PROTOCOL_NFC_DEP;
}

/*******************************************************************************
**
** Function:        isListenMode
**
** Description:     Indicates whether the activation data indicates it is
**                  listen mode.
**
** Returns:         True if this listen mode.
**
*******************************************************************************/
static bool isListenMode(tNFA_ACTIVATED& activated) {
  return (
      (NFC_DISCOVERY_TYPE_LISTEN_A ==
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
       activated.activate_ntf.rf_tech_param.mode) ||
      (NFC_INTERFACE_EE_DIRECT_RF == activated.activate_ntf.intf_param.type));
}

/*******************************************************************************
**
** Function:        stNfcManager_doCheckLlcp
**
** Description:     Not used.
**
** Returns:         True
**
*******************************************************************************/
static jboolean stNfcManager_doCheckLlcp(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  return JNI_TRUE;
}

/*******************************************************************************
**
** Function:        nfcManager_doActivateLlcp
**
** Description:     Not used.
**
** Returns:         True
**
*******************************************************************************/
static jboolean stNfcManager_doActivateLlcp(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  return JNI_TRUE;
}

/*******************************************************************************
**
** Function:        nfcManager_doAbort
**
** Description:     Not used.
**
** Returns:         None
**
*******************************************************************************/
static void stNfcManager_doAbort(JNIEnv* e, jobject, jstring msg) {
  ScopedUtfChars message = {e, msg};
  e->FatalError(message.c_str());
  abort();  // <-- Unreachable
}

/*******************************************************************************
**
** Function:        nfcManager_setObserverMode
**
** Description:     Enable or disable the observer mode
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         None.
**
*******************************************************************************/
static jboolean nfcManager_setObserverMode(JNIEnv* e, jobject o,
                                           jboolean enabled) {
  bool wasStopped = false;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("Enter :%s  e:%d", __func__, enabled);

  // Set the observer mode enabled or disabled
  uint8_t param[1];
  param[0] = (enabled == JNI_TRUE ? 0x01 : 0x00);

  if (sRfEnabled) {
    // Stop RF Discovery if we were polling
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: stop discovery reconfiguring", __func__);
    gIsReconfiguringDiscovery.start();
    startRfDiscovery(false);
    wasStopped = true;
  }

  tNFA_STATUS status =
      NFA_SetConfig(NCI_PARAM_ID_PROP_OBSERVER_MODE, sizeof(param), &param[0]);

  if (wasStopped) {
    // start discovery
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: reconfigured start discovery", __func__);
    startRfDiscovery(true);
    gIsReconfiguringDiscovery.end();
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);

  return (status == NFA_STATUS_OK) ? JNI_TRUE : JNI_FALSE;
}

/*******************************************************************************
**
** Function:        nfcManager_enableStLog
**
** Description:     Enable or disable the collection of firmware logs
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         None.
**
*******************************************************************************/
static void nfcManager_enableStLog(JNIEnv* e, jobject o, jboolean enabled) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("Enter :%s  e:%d", __func__, enabled);

  NfcStExtensions::getInstance().StLogManagerEnable(enabled);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
}

/*******************************************************************************
**
** Function:        nfcManager_SetMuteTech
**
** Description:     listen mode configuration
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         None.
**
*******************************************************************************/
static bool nfcManager_SetMuteTech(JNIEnv* e, jobject o, jboolean muteA,
                                   jboolean muteB, jboolean muteF,
                                   jboolean isCommitNeeded) {
  bool wasStopped = false;
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("Enter :%s  a:%d b:%d f:%d, isCommitNeeded: %d", __func__,
                      muteA, muteB, muteF, isCommitNeeded);

  jint newMask = (muteA ? 1 : 0) | (muteB ? 2 : 0) | (muteF ? 4 : 0);

  if (sWalletTechIsMute != newMask) {
    if (sRfEnabled) {
      // Stop RF Discovery if we were polling
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: stop discovery reconfiguring", __func__);
      gIsReconfiguringDiscovery.start();
      startRfDiscovery(false);
      wasStopped = true;
    }

    NFA_SetMuteTech(muteA, muteB, muteF);

    if (isCommitNeeded) {
      StRoutingManager::getInstance().updateRoutingTable();
      StRoutingManager::getInstance().commitRouting();
    }

    if (wasStopped) {
      // start discovery
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: reconfigured start discovery", __func__);
      startRfDiscovery(true);
      gIsReconfiguringDiscovery.end();
    }

    sWalletTechIsMute = newMask;
  }
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: success", __func__);
  return true;
}

/*******************************************************************************
**
** Function:        nfcManager_rotateRfParameters
**
** Description:     Change dynamic RF parameters
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         None.
**
*******************************************************************************/
static jboolean nfcManager_rotateRfParameters(JNIEnv* e, jobject o,
                                              jboolean reset) {
  bool res;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("Enter :%s  r:%d", __func__, reset);

  res = NfcStExtensions::rotateRfParameters((bool)reset);

  return res ? JNI_TRUE : JNI_FALSE;
}

/*******************************************************************************
**
** Function:        stNfcManager_doDownload
**
** Description:     Download firmware patch files.  Do not turn on NFC.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean stNfcManager_doDownload(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  bool result = JNI_FALSE;
  theInstance.Initialize();  // start GKI, NCI task, NFC task
  result = theInstance.DownloadFirmware();
  theInstance.Finalize();
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return result;
}

/*******************************************************************************
**
** Function:        stNfcManager_doResetTimeouts
**
** Description:     Not used.
**
** Returns:         None
**
*******************************************************************************/
static void stNfcManager_doResetTimeouts(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  NfcTag::getInstance().resetAllTransceiveTimeouts();
}

/*******************************************************************************
**
** Function:        stNfcManager_doSetTimeout
**
** Description:     Set timeout value.
**                  e: JVM environment.
**                  o: Java object.
**                  tech: technology ID.
**                  timeout: Timeout value.
**
** Returns:         True if ok.
**
*******************************************************************************/
static bool stNfcManager_doSetTimeout(JNIEnv*, jobject, jint tech,
                                      jint timeout) {
  if (timeout <= 0) {
    LOG(ERROR) << StringPrintf("%s: Timeout must be positive.", __func__);
    return false;
  }
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: tech=%d, timeout=%d", __func__, tech, timeout);
  NfcTag::getInstance().setTransceiveTimeout(tech, timeout);
  return true;
}

/*******************************************************************************
**
** Function:        stNfcManager_doGetTimeout
**
** Description:     Get timeout value.
**                  e: JVM environment.
**                  o: Java object.
**                  tech: technology ID.
**
** Returns:         Timeout value.
**
*******************************************************************************/
static jint stNfcManager_doGetTimeout(JNIEnv*, jobject, jint tech) {
  int timeout = NfcTag::getInstance().getTransceiveTimeout(tech);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: tech=%d, timeout=%d", __func__, tech, timeout);
  return timeout;
}

/*******************************************************************************
**
** Function:        nfcManager_doDump
**
** Description:     Get libnfc-nci dump
**                  e: JVM environment.
**                  obj: Java object.
**                  fdobj: File descriptor to be used
**
** Returns:         Void
**
*******************************************************************************/
static void stNfcManager_doDump(JNIEnv* e, jobject obj, jobject fdobj) {
  int fd = jniGetFDFromFileDescriptor(e, fdobj);
  if (fd < 0) return;

  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  theInstance.Dump(fd);
}

/*******************************************************************************
**
** Function:        nfcManager_doGetNciVersion
**
** Description:     Get libnfc-nci dump
**                  e: JVM environment.
**                  obj: Java object.
**                  fdobj: File descriptor to be used
**
** Returns:         Void
**
*******************************************************************************/
static jint stNfcManager_doGetNciVersion(JNIEnv*, jobject) {
  return NFC_GetNCIVersion();
}

/*******************************************************************************
**
** Function:        stNfcManager_doSetScreenState
**
** Description:     Get libnfc-nci dump
**                  e: JVM environment.
**                  obj: Java object.
**                  fdobj: File descriptor to be used
**
** Returns:         Void
**
*******************************************************************************/
static void stNfcManager_doSetScreenState(JNIEnv* e, jobject o,
                                          jint screen_state_mask) {
  tNFA_STATUS status = NFA_STATUS_OK;
  uint8_t state = (screen_state_mask & NFA_SCREEN_STATE_MASK);
  uint8_t discovry_param =
      NCI_LISTEN_DH_NFCEE_ENABLE_MASK | NCI_POLLING_DH_ENABLE_MASK;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: state = %d prevScreenState= %d, discovry_param = %d",
                      __FUNCTION__, state, prevScreenState, discovry_param);

  if (prevScreenState == state) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "New screen state is same as previous state. No action taken");
    return;
  }

  if (sIsDisabling || !sIsNfaEnabled ||
      (NFC_GetNCIVersion() != NCI_VERSION_2_0)) {
    prevScreenState = state;
    return;
  }
  if (prevScreenState == NFA_SCREEN_STATE_OFF_LOCKED ||
      prevScreenState == NFA_SCREEN_STATE_OFF_UNLOCKED ||
      prevScreenState == NFA_SCREEN_STATE_ON_LOCKED ||
      prevScreenState == NFA_SCREEN_STATE_UNKNOWN) {
    SyncEventGuard guard(sNfaSetPowerSubState);
    status = NFA_SetPowerSubStateForScreenState(state);
    if (status != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail enable SetScreenState; error=0x%X",
                                 __FUNCTION__, status);
      return;
    } else {
      sNfaSetPowerSubState.wait();
    }
  }

  if (state == NFA_SCREEN_STATE_OFF_LOCKED ||
      state == NFA_SCREEN_STATE_OFF_UNLOCKED) {
    // disable poll and enable listen on DH 0x00
    discovry_param =
        NCI_POLLING_DH_DISABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK;
  }

  if (state == NFA_SCREEN_STATE_ON_LOCKED) {
    // disable poll and enable listen on DH 0x00
    discovry_param =
        (screen_state_mask & NFA_SCREEN_POLLING_TAG_MASK)
            ? (NCI_LISTEN_DH_NFCEE_ENABLE_MASK | NCI_POLLING_DH_ENABLE_MASK)
            : (NCI_POLLING_DH_DISABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK);
  }

  if (state == NFA_SCREEN_STATE_ON_UNLOCKED) {
    // enable both poll and listen on DH 0x01
    discovry_param =
        NCI_LISTEN_DH_NFCEE_ENABLE_MASK | NCI_POLLING_DH_ENABLE_MASK;
  }

  SyncEventGuard guard(sNfaSetConfigEvent);
  status = NFA_SetConfig(NCI_PARAM_ID_CON_DISCOVERY_PARAM,
                         NCI_PARAM_LEN_CON_DISCOVERY_PARAM, &discovry_param);
  if (status == NFA_STATUS_OK) {
    sNfaSetConfigEvent.wait();
  } else {
    LOG(ERROR) << StringPrintf("%s: Failed to update CON_DISCOVER_PARAM",
                               __FUNCTION__);
    return;
  }

  if (prevScreenState == NFA_SCREEN_STATE_ON_UNLOCKED) {
    SyncEventGuard guard(sNfaSetPowerSubState);
    status = NFA_SetPowerSubStateForScreenState(state);
    if (status != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail enable SetScreenState; error=0x%X",
                                 __FUNCTION__, status);
    } else {
      sNfaSetPowerSubState.wait();
    }
  }
  if ((state == NFA_SCREEN_STATE_OFF_LOCKED ||
       state == NFA_SCREEN_STATE_OFF_UNLOCKED) &&
      prevScreenState == NFA_SCREEN_STATE_ON_UNLOCKED && (!sP2pActive) &&
      (!sSeRfActive)) {
    // screen turns off, disconnect tag if connected
    nativeNfcTag_doDisconnect(NULL, NULL);
  }

  prevScreenState = state;
}
/*******************************************************************************
**
** Function:        stNfcManager_doSetP2pInitiatorModes
**
** Description:     Set P2P initiator's activation modes.
**                  e: JVM environment.
**                  o: Java object.
**                  modes: Active and/or passive modes.  The values are
*specified
**                          in external/libnfc-nxp/inc/phNfcTypes.h.  See
**                          enum phNfc_eP2PMode_t.
**
** Returns:         None.
**
*******************************************************************************/
static void stNfcManager_doSetP2pInitiatorModes(JNIEnv* e, jobject o,
                                                jint modes) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: modes=0x%X", __func__, modes);
  struct nfc_jni_native_data* nat = getNative(e, o);

  tNFA_TECHNOLOGY_MASK mask = 0;
  if (modes & 0x01) mask |= NFA_TECHNOLOGY_MASK_A;
  if (modes & 0x02) mask |= NFA_TECHNOLOGY_MASK_F;
  if (modes & 0x04) mask |= NFA_TECHNOLOGY_MASK_F;
  if (modes & 0x08) mask |= NFA_TECHNOLOGY_MASK_ACTIVE;
  nat->tech_mask = mask;
}

/*******************************************************************************
**
** Function:        stNfcManager_doSetP2pTargetModes
**
** Description:     Set P2P target's activation modes.
**                  e: JVM environment.
**                  o: Java object.
**                  modes: Active and/or passive modes.
**
** Returns:         None.
**
*******************************************************************************/
static void stNfcManager_doSetP2pTargetModes(JNIEnv*, jobject, jint modes) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: modes=0x%X", __func__, modes);
  // Map in the right modes
  tNFA_TECHNOLOGY_MASK mask = 0;
  if (modes & 0x01) mask |= NFA_TECHNOLOGY_MASK_A;
  if (modes & 0x02) mask |= NFA_TECHNOLOGY_MASK_F;
  if (modes & 0x04) mask |= NFA_TECHNOLOGY_MASK_F;
  if (modes & 0x08) mask |= NFA_TECHNOLOGY_MASK_ACTIVE;

  PeerToPeer::getInstance().setP2pListenMask(mask);
}

static void stNfcManager_doEnableScreenOffSuspend(JNIEnv* e, jobject o) {
  PowerSwitch::getInstance().setScreenOffPowerState(
      PowerSwitch::POWER_STATE_FULL);
}

/*******************************************************************************
**
** Function:        stNfcManager_forceRouting
**
** Description:     Force routing to a NFCEE_ID
**                  e: JVM environment.
**                  o: Java object.
**
**
** Returns:         None.
**
*******************************************************************************/
static void stNfcManager_forceRouting(JNIEnv*, jobject, jint nfceeid) {
  if (sRfEnabled) {
    /*Update routing table only in Idle state.*/
    startRfDiscovery(false);
  }
  StRoutingManager::getInstance().forceRouting(nfceeid);
  startRfDiscovery(true);
}

/*******************************************************************************
**
** Function:        stNfcManager_stopforceRouting
**
** Description:     Stop force routing .
**                  e: JVM environment.
**                  o: Java object.
**
**
** Returns:         None.
**
*******************************************************************************/
static void stNfcManager_stopforceRouting(JNIEnv*, jobject) {
  if (sRfEnabled) {
    /*Update routing table only in Idle state.*/
    startRfDiscovery(false);
  }
  StRoutingManager::getInstance().stopforceRouting();
  startRfDiscovery(true);
}

/*******************************************************************************
**
** Function:        stNfcManager_NfceeDiscover
**
** Description:     Discover NFCEEs.
**                  e: JVM environment.
**                  o: Java object.
**
**
** Returns:         None.
**
*******************************************************************************/
static void stNfcManager_NfceeDiscover(JNIEnv*, jobject) {
  StRoutingManager::getInstance().NfceeDiscover();
  StSecureElement::getInstance().resetEEInfo();
}

/*******************************************************************************
**
** Function:        nfcManager_clearAidTable
**
** Description:     Clean all AIDs in routing table
**                  e: JVM environment.
**                  o: Java object.
**
*******************************************************************************/
static bool stNfcManager_clearAidTable(JNIEnv*, jobject) {
  return StRoutingManager::getInstance().clearAidTable();
}

/*******************************************************************************
**
** Function:        nfcManager_clearAidTable
**
** Description:     Clean all AIDs in routing table
**                  e: JVM environment.
**                  o: Java object.
**
*******************************************************************************/
static void stNfcManager_doDisableScreenOffSuspend(JNIEnv* e, jobject o) {
  PowerSwitch::getInstance().setScreenOffPowerState(
      PowerSwitch::POWER_STATE_OFF);
}

/*******************************************************************************
**
** Function:        nfcManager_getIsoDepMaxTransceiveLength
**
** Description:     Get maximum ISO DEP Transceive Length supported by the NFC
**                  chip. Returns default 261 bytes if the property is not set.
**
** Returns:         max value.
**
*******************************************************************************/
static jint stNfcManager_getIsoDepMaxTransceiveLength(JNIEnv*, jobject) {
  /* Check if extended APDU is supported by the chip.
   * If not, default value is returned.
   * The maximum length of a default IsoDep frame consists of:
   * CLA, INS, P1, P2, LC, LE + 255 payload bytes = 261 bytes
   */
  return NfcConfig::getUnsigned(NAME_ISO_DEP_MAX_TRANSCEIVE, 261);
}

/*******************************************************************************
 **
 ** Function:        nfcManager_getAidTableSize
 ** Description:     Get the maximum supported size for AID routing table.
 **
 **                  e: JVM environment.
 **                  o: Java object.
 **
 *******************************************************************************/
static jint stNfcManager_getAidTableSize(JNIEnv*, jobject) {
  return NFA_GetAidTableSize();
}

/*******************************************************************************
 **
 ** Function:        stNfcManager_doSetNfcSecure
 ** Description:     Get the maximum supported size for AID routing table.
 **
 **                  e: JVM environment.
 **                  o: Java object.
 **
 *******************************************************************************/
static jboolean stNfcManager_doSetNfcSecure(JNIEnv* e, jobject o,
                                            jboolean enable) {
  StRoutingManager& routingManager = StRoutingManager::getInstance();
  routingManager.setNfcSecure(enable);
  bool rfEnabled = sRfEnabled;
  if (sRoutingInitialized) {
    routingManager.disableRoutingToHost();
    if (rfEnabled) startRfDiscovery(false);
    routingManager.updateRoutingTable();
    routingManager.enableRoutingToHost();
    routingManager.commitRouting();
    if (rfEnabled) startRfDiscovery(true);
  }
  return true;
}

/*******************************************************************************
 **
 ** Function:        stNfcManager_setUserDefaultRoutesPref
 ** Description:     Set default routes as set by user through
 *NfcSettingsAdapter
 **                  APIs.
 **
 **                  e: JVM environment.
 **                  o: Java object.
 **
 *******************************************************************************/
static void stNfcManager_setUserDefaultRoutesPref(
    JNIEnv* e, jobject o, jint mifareRoute, jint isoDepRoute, jint felicaRoute,
    jint abTechRoute, jint scRoute, jint aidRoute) {
  StRoutingManager& routingManager = StRoutingManager::getInstance();
  routingManager.setUserDefaultRoutesPref(mifareRoute, isoDepRoute, felicaRoute,
                                          abTechRoute, scRoute, aidRoute);
}

/*****************************************************************************
**
** JNI functions for android-4.0.1_r1
**
*****************************************************************************/
static JNINativeMethod gMethods[] = {
    {"doDownload", "()Z", (void*)stNfcManager_doDownload},

    {"initializeNativeStructure", "()Z", (void*)stNfcManager_initNativeStruc},

    {"doInitialize", "()Z", (void*)stNfcManager_doInitialize},

    {"doDeinitialize", "()Z", (void*)stNfcManager_doDeinitialize},

    {"sendRawFrame", "([B)Z", (void*)stNfcManager_sendRawFrame},

    {"routeAid", "([BII)Z", (void*)stNfcManager_routeAid},

    {"unrouteAid", "([B)Z", (void*)stNfcManager_unrouteAid},

    {"commitRouting", "()Z", (void*)stNfcManager_commitRouting},

    {"clearAidTable", "()Z", (void*)stNfcManager_clearAidTable},

    {"doRegisterT3tIdentifier", "([B)I",
     (void*)StNfcManager_doRegisterT3tIdentifier},

    {"doDeregisterT3tIdentifier", "(I)V",
     (void*)StNfcManager_doDeregisterT3tIdentifier},

    {"getLfT3tMax", "()I", (void*)StNfcManager_getLfT3tMax},

    {"doEnableDiscovery", "(IZZZZZ)V", (void*)stNfcManager_enableDiscovery},

    {"doCheckLlcp", "()Z", (void*)stNfcManager_doCheckLlcp},

    {"doActivateLlcp", "()Z", (void*)stNfcManager_doActivateLlcp},

    {"doCreateLlcpConnectionlessSocket",
     "(ILjava/lang/String;)Lcom/android/nfc/dhimpl/"
     "NativeLlcpConnectionlessSocket;",
     (void*)stNfcManager_doCreateLlcpConnectionlessSocket},

    {"doCreateLlcpServiceSocket",
     "(ILjava/lang/String;III)Lcom/android/nfc/dhimpl/NativeLlcpServiceSocket;",
     (void*)stNfcManager_doCreateLlcpServiceSocket},

    {"doCreateLlcpSocket", "(IIII)Lcom/android/nfc/dhimpl/NativeLlcpSocket;",
     (void*)stNfcManager_doCreateLlcpSocket},

    {"doGetLastError", "()I", (void*)stNfcManager_doGetLastError},

    {"disableDiscovery", "()V", (void*)stNfcManager_disableDiscovery},

    {"doSetTimeout", "(II)Z", (void*)stNfcManager_doSetTimeout},

    {"doGetTimeout", "(I)I", (void*)stNfcManager_doGetTimeout},

    {"doResetTimeouts", "()V", (void*)stNfcManager_doResetTimeouts},

    {"doAbort", "(Ljava/lang/String;)V", (void*)stNfcManager_doAbort},

    {"doSetP2pInitiatorModes", "(I)V",
     (void*)stNfcManager_doSetP2pInitiatorModes},

    {"doSetP2pTargetModes", "(I)V", (void*)stNfcManager_doSetP2pTargetModes},

    {"doEnableScreenOffSuspend", "()V",
     (void*)stNfcManager_doEnableScreenOffSuspend},

    {"doSetScreenState", "(I)V", (void*)stNfcManager_doSetScreenState},

    {"doDisableScreenOffSuspend", "()V",
     (void*)stNfcManager_doDisableScreenOffSuspend},

    {"doDump", "(Ljava/io/FileDescriptor;)V", (void*)stNfcManager_doDump},
    {"forceRouting", "(I)V", (void*)stNfcManager_forceRouting},
    {"stopforceRouting", "()V", (void*)stNfcManager_stopforceRouting},
    {"NfceeDiscover", "()V", (void*)stNfcManager_NfceeDiscover},

    {"getNciVersion", "()I", (void*)stNfcManager_doGetNciVersion},
    {"doEnableDtaMode", "()V", (void*)stNfcManager_doEnableDtaMode},
    {"doDisableDtaMode", "()V", (void*)stNfcManager_doDisableDtaMode},
    {"doFactoryReset", "()V", (void*)stNfcManager_doFactoryReset},
    {"doShutdown", "()V", (void*)stNfcManager_doShutdown},

    {"getIsoDepMaxTransceiveLength", "()I",
     (void*)stNfcManager_getIsoDepMaxTransceiveLength},

    {"getAidTableSize", "()I", (void*)stNfcManager_getAidTableSize},

    {"doSetMuteTech", "(ZZZZ)Z", (void*)nfcManager_SetMuteTech},

    {"doSetNfcSecure", "(Z)Z", (void*)stNfcManager_doSetNfcSecure},

    {"enableStLog", "(Z)V", (void*)nfcManager_enableStLog},
    {"setObserverMode", "(Z)Z", (void*)nfcManager_setObserverMode},

    {"setUserDefaultRoutesPref", "(IIIIII)V",
     (void*)stNfcManager_setUserDefaultRoutesPref},

    {"rotateRfParameters", "(Z)Z", (void*)nfcManager_rotateRfParameters},
};

/*******************************************************************************
**
** Function:        register_com_android_nfc_NativeNfcManager
**
** Description:     Regisgter JNI functions with Java Virtual Machine.
**                  e: Environment of JVM.
**
** Returns:         Status of registration.
**
*******************************************************************************/
int register_com_android_nfc_stNativeNfcManager(JNIEnv* e) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  PowerSwitch::getInstance().initialize(PowerSwitch::UNKNOWN_LEVEL);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return jniRegisterNativeMethods(e, gStNativeNfcManagerClassName, gMethods,
                                  NELEM(gMethods));
}

/*******************************************************************************
**
** Function:        startRfDiscovery
**
** Description:     Ask stack to start polling and listening for devices.
**                  isStart: Whether to start.
**
** Returns:         None
**
*******************************************************************************/
void startRfDiscovery(bool isStart) {
  tNFA_STATUS status = NFA_STATUS_FAILED;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: is start=%d", __func__, isStart);
  nativeNfcTag_acquireRfInterfaceMutexLock();
  SyncEventGuard guard(sNfaEnableDisablePollingEvent);
  status = isStart ? NFA_StartRfDiscovery() : NFA_StopRfDiscovery();
  if (status == NFA_STATUS_OK) {
    sNfaEnableDisablePollingEvent.wait();  // wait for NFA_RF_DISCOVERY_xxxx_EVT
    sRfEnabled = isStart;
  } else {
    LOG(ERROR) << StringPrintf(
        "%s: Failed to start/stop RF discovery; error=0x%X", __func__, status);
  }
  nativeNfcTag_releaseRfInterfaceMutexLock();
}

/*******************************************************************************
**
** Function:        isDiscoveryStarted
**
** Description:     Indicates whether the discovery is started.
**
** Returns:         True if discovery is started
**
*******************************************************************************/
bool isDiscoveryStarted() { return sRfEnabled; }

/*******************************************************************************
**
** Function:        pollingChanged
**
** Description:     Update internal vars when mode was changed outside.
**
** Returns:         None
**
*******************************************************************************/
void pollingChanged(int discoveryEnabled, int pollingEnabled, int p2pEnabled) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: %d %d %d", __func__, discoveryEnabled, pollingEnabled, p2pEnabled);
  switch (discoveryEnabled) {
    case 1:
      sDiscoveryEnabled = true;
      break;
    case -1:
      sDiscoveryEnabled = false;
      break;
  }
  switch (pollingEnabled) {
    case 1:
      sPollingEnabled = true;
      break;
    case -1:
      sPollingEnabled = false;
      break;
  }
  switch (p2pEnabled) {
    case 1:
      sP2pEnabled = true;
      break;
    case -1:
      sP2pEnabled = false;
      break;
  }
}

/*******************************************************************************
**
** Function:        doStartupConfig
**
** Description:     Configure the NFC controller.
**
** Returns:         None
**
*******************************************************************************/
void doStartupConfig() {
  // configure RF polling frequency for each technology
  static tNFA_DM_DISC_FREQ_CFG nfa_dm_disc_freq_cfg;
  // values in the polling_frequency[] map to members of nfa_dm_disc_freq_cfg
  std::vector<uint8_t> polling_frequency;
  if (NfcConfig::hasKey(NAME_POLL_FREQUENCY))
    polling_frequency = NfcConfig::getBytes(NAME_POLL_FREQUENCY);
  if (polling_frequency.size() == 8) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: polling frequency", __func__);
    memset(&nfa_dm_disc_freq_cfg, 0, sizeof(nfa_dm_disc_freq_cfg));
    nfa_dm_disc_freq_cfg.pa = polling_frequency[0];
    nfa_dm_disc_freq_cfg.pb = polling_frequency[1];
    nfa_dm_disc_freq_cfg.pf = polling_frequency[2];
    nfa_dm_disc_freq_cfg.pi93 = polling_frequency[3];
    nfa_dm_disc_freq_cfg.pbp = polling_frequency[4];
    nfa_dm_disc_freq_cfg.pk = polling_frequency[5];
    nfa_dm_disc_freq_cfg.paa = polling_frequency[6];
    nfa_dm_disc_freq_cfg.pfa = polling_frequency[7];
    p_nfa_dm_rf_disc_freq_cfg = &nfa_dm_disc_freq_cfg;
  }

  {
    uint8_t nfa_field_info[] = {0x01};

    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Configure RF_FIELD_INFO event", __func__);
    SyncEventGuard guard(sNfaSetConfigEvent);
    tNFA_STATUS status = NFA_SetConfig(
        NCI_PARAM_ID_RF_FIELD_INFO, sizeof(nfa_field_info), &nfa_field_info[0]);
    if (status == NFA_STATUS_OK) sNfaSetConfigEvent.wait();
  }

  // configure NFCC_CONFIG_CONTROL- NFCC allowed to manage RF configuration.
  stNfcManager_configNfccConfigControl(true);
}

/*******************************************************************************
**
** Function:        nfcManager_isNfcActive
**
** Description:     Used externaly to determine if NFC is active or not.
**
** Returns:         'true' if the NFC stack is running, else 'false'.
**
*******************************************************************************/
bool nfcManager_isNfcActive() { return sIsNfaEnabled; }

/*******************************************************************************
**
** Function:        startStopPolling
**
** Description:     Start or stop polling.
**                  isStartPolling: true to start polling; false to stop
*polling.
**
** Returns:         None.
**
*******************************************************************************/
void startStopPolling(bool isStartPolling) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; isStart=%u", __func__, isStartPolling);
  gIsReconfiguringDiscovery.start();
  startRfDiscovery(false);

  if (isStartPolling)
    startPolling_rfDiscoveryDisabled(0);
  else
    stopPolling_rfDiscoveryDisabled();

  startRfDiscovery(true);
  gIsReconfiguringDiscovery.end();
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
}

static tNFA_STATUS startPolling_rfDiscoveryDisabled(
    tNFA_TECHNOLOGY_MASK tech_mask) {
  tNFA_STATUS stat = NFA_STATUS_FAILED;

  if (tech_mask == 0)
    tech_mask =
        NfcConfig::getUnsigned(NAME_POLLING_TECH_MASK, DEFAULT_TECH_MASK);

  nativeNfcTag_acquireRfInterfaceMutexLock();
  SyncEventGuard guard(sNfaEnableDisablePollingEvent);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enable polling", __func__);
  stat = NFA_EnablePolling(tech_mask);
  if (stat == NFA_STATUS_OK) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: wait for enable event", __func__);
    sPollingEnabled = true;
    sNfaEnableDisablePollingEvent.wait();  // wait for NFA_POLL_ENABLED_EVT
  } else {
    LOG(ERROR) << StringPrintf("%s: fail enable polling; error=0x%X", __func__,
                               stat);
  }
  nativeNfcTag_releaseRfInterfaceMutexLock();

  return stat;
}

static tNFA_STATUS stopPolling_rfDiscoveryDisabled() {
  tNFA_STATUS stat = NFA_STATUS_FAILED;

  nativeNfcTag_acquireRfInterfaceMutexLock();
  SyncEventGuard guard(sNfaEnableDisablePollingEvent);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: disable polling", __func__);
  stat = NFA_DisablePolling();
  if (stat == NFA_STATUS_OK) {
    sPollingEnabled = false;
    sNfaEnableDisablePollingEvent.wait();  // wait for NFA_POLL_DISABLED_EVT
  } else {
    LOG(ERROR) << StringPrintf("%s: fail disable polling; error=0x%X", __func__,
                               stat);
  }
  nativeNfcTag_releaseRfInterfaceMutexLock();

  return stat;
}

/*******************************************************************************
**
** Function:        setNciConfig
**
** Description:     Start or stop polling.
**                  isStartPolling: true to start polling; false to stop
*polling.
**
** Returns:         None.
**
*******************************************************************************/
void setNciConfig(int param_id, uint8_t* param, int length) {
  tNFA_STATUS stat = NFA_STATUS_OK;
  // Stop RF discovery
  gIsReconfiguringDiscovery.start();
  startRfDiscovery(false);

  SyncEventGuard guard(sNfaSetConfigEvent);
  stat = NFA_SetConfig(param_id, length, param);
  if (stat == NFA_STATUS_OK)
    sNfaSetConfigEvent.wait();
  else
    LOG(ERROR) << StringPrintf("%s: Could not configure NCI param", __func__);

  // Stop RF discovery
  startRfDiscovery(true);
  gIsReconfiguringDiscovery.end();
}
} /* namespace android */
