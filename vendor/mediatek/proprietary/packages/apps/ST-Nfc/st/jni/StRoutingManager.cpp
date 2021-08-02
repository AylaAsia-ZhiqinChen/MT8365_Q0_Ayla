/*
 * Copyright (C) 2013 The Android Open Source Project
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

/*
 *  Manage the listen-mode routing table.
 */

#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <nativehelper/JNIHelp.h>
#include <nativehelper/ScopedLocalRef.h>

#include "JavaClassConstants.h"
#include "StNfcJni.h"
#include "StRoutingManager.h"
#include "NfcStExtensions.h"

#include "nfa_ce_api.h"
#include "nfa_ee_api.h"
#include "nfc_config.h"

using android::base::StringPrintf;

extern bool gActivated;
extern SyncEvent gDeactivatedEvent;
extern bool nfc_debug_enabled;

const JNINativeMethod StRoutingManager::sMethods[] = {
    {"doGetDefaultRouteDestination", "()I",
     (void*)StRoutingManager::
         com_android_nfc_cardemulation_doGetDefaultRouteDestination},
    {"doGetDefaultOffHostRouteDestination", "()I",
     (void*)StRoutingManager::
         com_android_nfc_cardemulation_doGetDefaultOffHostRouteDestination},
    {"doGetOffHostUiccDestination", "()[B",
     (void*)StRoutingManager::
         com_android_nfc_cardemulation_doGetOffHostUiccDestination},
    {"doGetOffHostEseDestination", "()[B",
     (void*)StRoutingManager::
         com_android_nfc_cardemulation_doGetOffHostEseDestination},
    {"doGetAidMatchingMode", "()I",
     (void*)
         StRoutingManager::com_android_nfc_cardemulation_doGetAidMatchingMode},
    {"doGetDefaultIsoDepRouteDestination", "()I",
     (void*)StRoutingManager::
         com_android_nfc_cardemulation_doGetDefaultIsoDepRouteDestination}};

static const int MAX_NUM_EE = 5;
// SCBR from host works only when App is in foreground
static const uint8_t SYS_CODE_PWR_STATE_HOST = 0x01;
static const uint16_t DEFAULT_SYS_CODE = 0xFEFE;

static const uint8_t AID_ROUTE_QUAL_PREFIX = 0x10;

/*******************************************************************************
**
** Function:        StRoutingManager
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
StRoutingManager::StRoutingManager() : mAidRoutingConfigured(false) {
  static const char fn[] = "StRoutingManager::RoutingManager()";

  mDefaultOffHostRoute =
      NfcConfig::getUnsigned(NAME_DEFAULT_OFFHOST_ROUTE, 0x00);

  if (NfcConfig::hasKey(NAME_OFFHOST_ROUTE_UICC)) {
    mOffHostRouteUicc = NfcConfig::getBytes(NAME_OFFHOST_ROUTE_UICC);
  }

  if (NfcConfig::hasKey(NAME_OFFHOST_ROUTE_ESE)) {
    mOffHostRouteEse = NfcConfig::getBytes(NAME_OFFHOST_ROUTE_ESE);
  }

  mDefaultFelicaRoute = NfcConfig::getUnsigned(NAME_DEFAULT_NFCF_ROUTE, 0x00);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: Active SE for Nfc-F is 0x%02X", fn, mDefaultFelicaRoute);

  mDefaultEe = NfcConfig::getUnsigned(NAME_DEFAULT_ROUTE, 0x00);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: default route is 0x%02X", fn, mDefaultEe);

  mResolvedDefaultAidRoute = mDefaultEe;

  mAidMatchingMode =
      NfcConfig::getUnsigned(NAME_AID_MATCHING_MODE, AID_MATCHING_EXACT_ONLY);

  mDefaultSysCodeRoute =
      NfcConfig::getUnsigned(NAME_DEFAULT_SYS_CODE_ROUTE, 0xC0);

  mDefaultSysCodePowerstate =
      NfcConfig::getUnsigned(NAME_DEFAULT_SYS_CODE_PWR_STATE, 0x19);

  mDefaultSysCode = DEFAULT_SYS_CODE;
  if (NfcConfig::hasKey(NAME_DEFAULT_SYS_CODE)) {
    std::vector<uint8_t> pSysCode = NfcConfig::getBytes(NAME_DEFAULT_SYS_CODE);
    if (pSysCode.size() == 0x02) {
      mDefaultSysCode = ((pSysCode[0] << 8) | ((int)pSysCode[1] << 0));
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: DEFAULT_SYS_CODE: 0x%02X", __func__, mDefaultSysCode);
    }
  }

  mOffHostAidRoutingPowerState =
      NfcConfig::getUnsigned(NAME_OFFHOST_AID_ROUTE_PWR_STATE, 0x01);

  mDefaultIsoDepRoute = NfcConfig::getUnsigned(NAME_DEFAULT_ISODEP_ROUTE, 0x0);

  memset(&mEeInfo, 0, sizeof(mEeInfo));
  mReceivedEeInfo = false;
  mSeTechMask = 0x00;
  mIsScbrSupported = false;

  mNfcFOnDhHandle = NFA_HANDLE_INVALID;

  mDeinitializing = false;
  mEeInfoChanged = true;
  mScRoutingConfigured = false;
}

/*******************************************************************************
**
** Function:        StRoutingManager
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
StRoutingManager::~StRoutingManager() {}

/*******************************************************************************
**
** Function:        initialize
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
bool StRoutingManager::initialize(nfc_jni_native_data* native) {
  static const char fn[] = "RoutingManager::initialize()";
  mNativeData = native;
  mRxDataBuffer.clear();

  mUserDefaultOffHostRoute = INVALID_ROUTE_VALUE;
  mUserDefaultIsoDepRoute = INVALID_ROUTE_VALUE;
  mUserDefaultFelicaRoute = INVALID_ROUTE_VALUE;
  mUserDefaultMifareRoute = INVALID_ROUTE_VALUE;
  mUserDefaultScRoute = INVALID_ROUTE_VALUE;
  mUserDefaultAidRoute = INVALID_ROUTE_VALUE;

  mIsInit = true;

  {
    SyncEventGuard guard(mEeRegisterEvent);
    DLOG_IF(INFO, nfc_debug_enabled) << fn << ": try ee register";
    tNFA_STATUS nfaStat = NFA_EeRegister(nfaEeCallback);
    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail ee register; error=0x%X", fn,
                                 nfaStat);
      return false;
    }
    mEeRegisterEvent.wait();
  }

  if ((mDefaultOffHostRoute != 0) || (mDefaultFelicaRoute != 0)) {
    // Wait for EE info if needed
    SyncEventGuard guard(mEeInfoEvent);
    if (!mReceivedEeInfo) {
      LOG(INFO) << fn << "Waiting for EE info";
      mEeInfoEvent.wait();
    }
  }
  mSeTechMask = updateEeTechRouteSetting();

  // Register a wild-card for AIDs routed to the host
  tNFA_STATUS nfaStat = NFA_CeRegisterAidOnDH(NULL, 0, stackCallback);
  if (nfaStat != NFA_STATUS_OK)
    LOG(ERROR) << fn << "Failed to register wildcard AID for DH";

  updateDefaultRoute();
  updateDefaultProtocolRoute();

  return true;
}

/*******************************************************************************
**
** Function:        getInstance
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
StRoutingManager& StRoutingManager::getInstance() {
  static StRoutingManager manager;
  return manager;
}

/*******************************************************************************
**
** Function:        enableRoutingToHost
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::enableRoutingToHost() {
  static const char fn[] = "StRoutingManager::enableRoutingToHost()";
  tNFA_STATUS nfaStat;
  SyncEventGuard guard(mRoutingEvent);

  // Default routing for T3T protocol
  if (!mIsScbrSupported && mDefaultEe == NFC_DH_ID) {
    nfaStat = NFA_EeSetDefaultProtoRouting(NFC_DH_ID, NFA_PROTOCOL_MASK_T3T, 0,
                                           0, 0, 0, 0);
    if (nfaStat == NFA_STATUS_OK)
      mRoutingEvent.wait();
    else
      LOG(ERROR) << fn << "Fail to set default proto routing for T3T";
  }

  // Default routing for IsoDep protocol
  tNFA_PROTOCOL_MASK protoMask = NFA_PROTOCOL_MASK_ISO_DEP;
  if (mDefaultIsoDepRoute == NFC_DH_ID) {
    nfaStat = NFA_EeSetDefaultProtoRouting(
        NFC_DH_ID, protoMask, 0, 0, mSecureNfcEnabled ? 0 : protoMask, 0, 0);
    if (nfaStat == NFA_STATUS_OK)
      mRoutingEvent.wait();
    else
      LOG(ERROR) << fn << "Fail to set default proto routing for IsoDep";
  }

  // Route Nfc-A to host if we don't have a SE
  tNFA_TECHNOLOGY_MASK techMask = NFA_TECHNOLOGY_MASK_A;
  if ((mSeTechMask & NFA_TECHNOLOGY_MASK_A) == 0) {
    nfaStat = NFA_EeSetDefaultTechRouting(
        NFC_DH_ID, techMask, 0, 0, mSecureNfcEnabled ? 0 : techMask, 0, 0);
    if (nfaStat == NFA_STATUS_OK)
      mRoutingEvent.wait();
    else
      LOG(ERROR) << fn << "Fail to set default tech routing for Nfc-A";
  }

  // Route Nfc-F to host if we don't have a SE
  techMask = NFA_TECHNOLOGY_MASK_F;
  if ((mSeTechMask & NFA_TECHNOLOGY_MASK_F) == 0) {
    nfaStat = NFA_EeSetDefaultTechRouting(
        NFC_DH_ID, techMask, 0, 0, mSecureNfcEnabled ? 0 : techMask, 0, 0);
    if (nfaStat == NFA_STATUS_OK)
      mRoutingEvent.wait();
    else
      LOG(ERROR) << fn << "Fail to set default tech routing for Nfc-F";
  }
}

/*******************************************************************************
**
** Function:        disableRoutingToHost
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::disableRoutingToHost() {
  static const char fn[] = "StRoutingManager::disableRoutingToHost()";
  tNFA_STATUS nfaStat;
  SyncEventGuard guard(mRoutingEvent);

  // Default routing for IsoDep protocol
  if (mDefaultIsoDepRoute == NFC_DH_ID) {
    nfaStat =
        NFA_EeClearDefaultProtoRouting(NFC_DH_ID, NFA_PROTOCOL_MASK_ISO_DEP);
    if (nfaStat == NFA_STATUS_OK)
      mRoutingEvent.wait();
    else
      LOG(ERROR) << fn << "Fail to set default proto routing for IsoDep";
  }

  // Default routing for Nfc-A technology if we don't have a SE
  if ((mSeTechMask & NFA_TECHNOLOGY_MASK_A) == 0) {
    nfaStat = NFA_EeClearDefaultTechRouting(NFC_DH_ID, NFA_TECHNOLOGY_MASK_A);
    if (nfaStat == NFA_STATUS_OK)
      mRoutingEvent.wait();
    else
      LOG(ERROR) << fn << "Fail to set default tech routing for Nfc-A";
  }

  // Default routing for Nfc-F technology if we don't have a SE
  if ((mSeTechMask & NFA_TECHNOLOGY_MASK_F) == 0) {
    nfaStat = NFA_EeClearDefaultTechRouting(NFC_DH_ID, NFA_TECHNOLOGY_MASK_F);
    if (nfaStat == NFA_STATUS_OK)
      mRoutingEvent.wait();
    else
      LOG(ERROR) << fn << "Fail to set default tech routing for Nfc-F";
  }

  // Default routing for T3T protocol
  if (!mIsScbrSupported && mDefaultEe == NFC_DH_ID) {
    nfaStat = NFA_EeClearDefaultProtoRouting(NFC_DH_ID, NFA_PROTOCOL_MASK_T3T);
    if (nfaStat == NFA_STATUS_OK)
      mRoutingEvent.wait();
    else
      LOG(ERROR) << fn << "Fail to set default proto routing for T3T";
  }
}

/*******************************************************************************
**
** Function:        addAidRouting
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
bool StRoutingManager::addAidRouting(const uint8_t* aid, uint8_t aidLen,
                                     int route, int aidInfo) {
  static const char fn[] = "StRoutingManager::addAidRouting";
  DLOG_IF(INFO, nfc_debug_enabled) << fn << ": enter - route: " << route;
  uint8_t powerState = 0x01;
  if (!mSecureNfcEnabled) {
    powerState = (route != 0x00) ? mOffHostAidRoutingPowerState : 0x11;
  }

  // Check if this is default AID route
  // Keep track of value
  if (aidLen == 0) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: resolved default AID route is 0x%02X", fn, route);
    mResolvedDefaultAidRoute = route;
    mConnectedDefaultAidRoute = route;
  }

  SyncEventGuard guard(mRoutingEvent);
  mAidRoutingConfigured = false;
  tNFA_STATUS nfaStat =
      NFA_EeAddAidRouting(route, aidLen, (uint8_t*)aid, powerState, aidInfo);
  if (nfaStat == NFA_STATUS_OK) {
    mRoutingEvent.wait();
  }
  if (mAidRoutingConfigured) {
    DLOG_IF(INFO, nfc_debug_enabled) << fn << ": routed AID";
    return true;
  } else {
    LOG(ERROR) << fn << ": failed to route AID";
    return false;
  }
}

/*******************************************************************************
**
** Function:        removeAidRouting
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
bool StRoutingManager::removeAidRouting(const uint8_t* aid, uint8_t aidLen) {
  static const char fn[] = "StRoutingManager::removeAidRouting";
  DLOG_IF(INFO, nfc_debug_enabled) << fn << ": enter";
  SyncEventGuard guard(mRoutingEvent);
  mAidRoutingConfigured = false;
  tNFA_STATUS nfaStat = NFA_EeRemoveAidRouting(aidLen, (uint8_t*)aid);
  if (nfaStat == NFA_STATUS_OK) {
    mRoutingEvent.wait();
  }
  if (mAidRoutingConfigured) {
    DLOG_IF(INFO, nfc_debug_enabled) << fn << ": removed AID";
    return true;
  } else {
    LOG(ERROR) << fn << ": failed to remove AID";
    return false;
  }
}

/*******************************************************************************
**
** Function:        commitRouting
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
bool StRoutingManager::commitRouting() {
  static const char fn[] = "StRoutingManager::commitRouting";
  tNFA_STATUS nfaStat = 0;
  DLOG_IF(INFO, nfc_debug_enabled) << fn;

  if (mEeInfoChanged) {
    // Update verything as some NFCEE might have been connected/
    // disconnect
    NFA_EeClearRoutingTable(mScRoutingConfigured ? false : true);

    if (mAidRoutingConfigured == false) {
      clearAidTable();
    }

    updateFullRoutes();

    mEeInfoChanged = false;
  }

  {
    SyncEventGuard guard(mEeUpdateEvent);
    nfaStat = NFA_EeUpdateNow();
    if (nfaStat == NFA_STATUS_OK) {
      mEeUpdateEvent.wait();  // wait for NFA_EE_UPDATED_EVT
    }
  }

  mAidRoutingConfigured = false;
  return (nfaStat == NFA_STATUS_OK);
}

/*******************************************************************************
**
** Function:        forceRouting
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::forceRouting(uint8_t nfceeid) {
  static const char fn[] = "StRoutingManager:forceRouting";
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s : nfceeid = 0x%X", fn, nfceeid);

  uint8_t config = 0x09;
  if (nfceeid != 0x00) {
    config = 0x3b;
  }

  uint8_t activeNfceeId =
      StSecureElement::getInstance().getActiveNfcee(nfceeid);

  SyncEventGuard guard(mEeForceRoutingEvent);
  if ((nfaStat = NFA_EeForceRouting(activeNfceeId, config)) == NFA_STATUS_OK) {
    mEeForceRoutingEvent.wait();  //   wait for NFA_EE_FORCE_ROUTING_EVT
  } else {
    LOG(ERROR) << StringPrintf("Failed to force routing");
  }
}

/*******************************************************************************
**
** Function:        stopforceRouting
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::stopforceRouting() {
  static const char fn[] = "StRoutingManager:stopforceRouting";
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", fn);

  SyncEventGuard guard(mEeForceRoutingEvent);
  if ((nfaStat = NFA_EeStopForceRouting()) == NFA_STATUS_OK) {
    mEeForceRoutingEvent.wait();  //   wait for NFA_EE_FORCE_ROUTING_EVT
  } else {
    LOG(ERROR) << StringPrintf("Failed to force routing");
  }
}

/*******************************************************************************
**
** Function:        NfceeDiscover
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::NfceeDiscover() {
  static const char fn[] = "StRoutingManager:NfceeDiscover";
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s : enter", fn);
  SyncEventGuard guard(mEeDiscoverEvent);
  if ((nfaStat = NFA_EeDiscover(nfaEeCallback)) == NFA_STATUS_OK) {
    mEeDiscoverEvent.wait(500);  //   wait for NFA_EE_FORCE_ROUTING_EVT
  } else {
    LOG(ERROR) << StringPrintf("Failed to discover NFCEEs");
  }
}

/*******************************************************************************
**
** Function:        onNfccShutdown
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::onNfccShutdown() {
  static const char fn[] = "StRoutingManager:onNfccShutdown";
  if (mDefaultOffHostRoute == 0x00 && mDefaultFelicaRoute == 0x00) return;

  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  uint8_t actualNumEe = MAX_NUM_EE;
  tNFA_EE_INFO eeInfo[MAX_NUM_EE];
  mDeinitializing = true;

  memset(&eeInfo, 0, sizeof(eeInfo));
  if ((nfaStat = NFA_EeGetInfo(&actualNumEe, eeInfo)) != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: fail get info; error=0x%X", fn, nfaStat);
    return;
  }
  if (actualNumEe != 0) {
    for (uint8_t xx = 0; xx < actualNumEe; xx++) {
      bool bIsOffHostEEPresent =
          (NFC_GetNCIVersion() < NCI_VERSION_2_0)
              ? (eeInfo[xx].num_interface != 0)
              : (eeInfo[xx].ee_interface[0] !=
                 NCI_NFCEE_INTERFACE_HCI_ACCESS) &&
                    (eeInfo[xx].ee_status == NFA_EE_STATUS_ACTIVE);
      if (bIsOffHostEEPresent) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: Handle: 0x%04x Change Status Active to Inactive", fn,
            eeInfo[xx].ee_handle);
        SyncEventGuard guard(mEeSetModeEvent);
        if ((nfaStat = NFA_EeModeSet(eeInfo[xx].ee_handle,
                                     NFA_EE_MD_DEACTIVATE)) == NFA_STATUS_OK) {
          mEeSetModeEvent.wait();  // wait for NFA_EE_MODE_SET_EVT
        } else {
          LOG(ERROR) << fn << "Failed to set EE inactive";
        }
      }
    }
  } else {
    DLOG_IF(INFO, nfc_debug_enabled) << fn << ": No active EEs found";
  }
}

/*******************************************************************************
**
** Function:        notifyActivated
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::notifyActivated(uint8_t technology) {
  JNIEnv* e = NULL;
  ScopedAttach attach(mNativeData->vm, &e);
  if (e == NULL) {
    LOG(ERROR) << "jni env is null";
    return;
  }

  e->CallVoidMethod(mNativeData->manager,
                    android::gCachedNfcManagerNotifyHostEmuActivated,
                    (int)technology);
  if (e->ExceptionCheck()) {
    e->ExceptionClear();
    LOG(ERROR) << "fail notify";
  }
}

/*******************************************************************************
**
** Function:        notifyDeactivated
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::notifyDeactivated(uint8_t technology) {
  mRxDataBuffer.clear();
  JNIEnv* e = NULL;
  ScopedAttach attach(mNativeData->vm, &e);
  if (e == NULL) {
    LOG(ERROR) << "jni env is null";
    return;
  }

  e->CallVoidMethod(mNativeData->manager,
                    android::gCachedNfcManagerNotifyHostEmuDeactivated,
                    (int)technology);
  if (e->ExceptionCheck()) {
    e->ExceptionClear();
    LOG(ERROR) << StringPrintf("fail notify");
  }
}

void StRoutingManager::notifyDefaultRouteSet(int aidRoute, int mifareRoute,
                                             int isoDepRoute, int felicaRoute,
                                             int abTechRoute, int scRoute) {
  JNIEnv* e = NULL;
  ScopedAttach attach(mNativeData->vm, &e);
  if (e == NULL) {
    LOG(ERROR) << "jni env is null";
    return;
  }

  e->CallVoidMethod(mNativeData->manager,
                    android::gCachedNfcManagerNotifyDefaultRoutesSet,
                    (int)aidRoute, (int)mifareRoute, (int)isoDepRoute,
                    (int)felicaRoute, (int)abTechRoute, (int)scRoute);
  if (e->ExceptionCheck()) {
    e->ExceptionClear();
    LOG(ERROR) << StringPrintf("fail notify");
  }
}

/*******************************************************************************
**
** Function:        handleData
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::handleData(uint8_t technology, const uint8_t* data,
                                  uint32_t dataLen, tNFA_STATUS status) {
  if (status == NFC_STATUS_CONTINUE) {
    if (dataLen > 0) {
      mRxDataBuffer.insert(mRxDataBuffer.end(), &data[0],
                           &data[dataLen]);  // append data; more to come
    }
    return;  // expect another NFA_CE_DATA_EVT to come
  } else if (status == NFA_STATUS_OK) {
    if (dataLen > 0) {
      mRxDataBuffer.insert(mRxDataBuffer.end(), &data[0],
                           &data[dataLen]);  // append data
    }
    // entire data packet has been received; no more NFA_CE_DATA_EVT
  } else if (status == NFA_STATUS_FAILED) {
    LOG(ERROR) << "RoutingManager::handleData: read data fail";
    goto TheEnd;
  }

  {
    JNIEnv* e = NULL;
    ScopedAttach attach(mNativeData->vm, &e);
    if (e == NULL) {
      LOG(ERROR) << "jni env is null";
      goto TheEnd;
    }

    ScopedLocalRef<jobject> dataJavaArray(
        e, e->NewByteArray(mRxDataBuffer.size()));
    if (dataJavaArray.get() == NULL) {
      LOG(ERROR) << "fail allocate array";
      goto TheEnd;
    }

    e->SetByteArrayRegion((jbyteArray)dataJavaArray.get(), 0,
                          mRxDataBuffer.size(), (jbyte*)(&mRxDataBuffer[0]));
    if (e->ExceptionCheck()) {
      e->ExceptionClear();
      LOG(ERROR) << "fail fill array";
      goto TheEnd;
    }

    e->CallVoidMethod(mNativeData->manager,
                      android::gCachedNfcManagerNotifyHostEmuData,
                      (int)technology, dataJavaArray.get());
    if (e->ExceptionCheck()) {
      e->ExceptionClear();
      LOG(ERROR) << "fail notify";
    }
  }
TheEnd:
  mRxDataBuffer.clear();
}

/*******************************************************************************
**
** Function:        notifyEeUpdated
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::notifyEeUpdated() {
  JNIEnv* e = NULL;
  ScopedAttach attach(mNativeData->vm, &e);
  if (e == NULL) {
    LOG(ERROR) << "jni env is null";
    return;
  }

  e->CallVoidMethod(mNativeData->manager,
                    android::gCachedNfcManagerNotifyEeUpdated);
  if (e->ExceptionCheck()) {
    e->ExceptionClear();
    LOG(ERROR) << "fail notify";
  }
}

/*******************************************************************************
**
** Function:        stackCallback
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::stackCallback(uint8_t event,
                                     tNFA_CONN_EVT_DATA* eventData) {
  static const char fn[] = "StRoutingManager::stackCallback";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: event=0x%X", fn, event);
  StRoutingManager& StRoutingManager = StRoutingManager::getInstance();

  switch (event) {
    case NFA_CE_REGISTERED_EVT: {
      tNFA_CE_REGISTERED& ce_registered = eventData->ce_registered;
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_CE_REGISTERED_EVT; status=0x%X; h=0x%X", fn,
                          ce_registered.status, ce_registered.handle);
      NfcStExtensions::getInstance().nfaConnectionCallback(event, eventData);
    } break;

    case NFA_CE_DEREGISTERED_EVT: {
      tNFA_CE_DEREGISTERED& ce_deregistered = eventData->ce_deregistered;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_CE_DEREGISTERED_EVT; h=0x%X", fn, ce_deregistered.handle);
      NfcStExtensions::getInstance().nfaConnectionCallback(event, eventData);
    } break;

    case NFA_CE_ACTIVATED_EVT: {
      StRoutingManager.notifyActivated((NFA_TECHNOLOGY_MASK_A));
    } break;

    case NFA_DEACTIVATED_EVT:
    case NFA_CE_DEACTIVATED_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_DEACTIVATED_EVT, NFA_CE_DEACTIVATED_EVT", fn);
      StRoutingManager.notifyDeactivated((NFA_TECHNOLOGY_MASK_A));
      SyncEventGuard g(gDeactivatedEvent);
      gActivated = false;  // guard this variable from multi-threaded access
      gDeactivatedEvent.notifyOne();
    } break;

    case NFA_CE_DATA_EVT: {
      tNFA_CE_DATA& ce_data = eventData->ce_data;
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_CE_DATA_EVT; stat=0x%X; h=0x%X; data len=%u",
                          fn, ce_data.status, ce_data.handle, ce_data.len);
      getInstance().handleData(NFA_TECHNOLOGY_MASK_A, ce_data.p_data,
                               ce_data.len, ce_data.status);
    } break;
  }
}

/*******************************************************************************
**
** Function:        updateRoutingTable
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::updateRoutingTable() {
  static const char fn[] = "StRoutingManager::updateRoutingTable()";
  DLOG_IF(INFO, nfc_debug_enabled) << fn;
  mSeTechMask = updateEeTechRouteSetting();
  updateDefaultProtocolRoute();
  updateDefaultRoute();
}

/*******************************************************************************
**
** Function:        updateDefaultProtocolRoute
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::updateDefaultProtocolRoute() {
  static const char fn[] = "StRoutingManager::updateDefaultProtocolRoute()";

  DLOG_IF(INFO, nfc_debug_enabled) << fn;

  // Default Routing for ISO-DEP
  tNFA_PROTOCOL_MASK protoMask = NFA_PROTOCOL_MASK_ISO_DEP;
  tNFA_STATUS nfaStat;

  /***************************************************/
  /*************** Check ISO-DEP route ***************/
  /***************************************************/
  int isoDepRoute = mDefaultIsoDepRoute;
  // Check if user has changed the defaukt routes through the NfcAdapterSettings
  // APIs
  if (mUserDefaultIsoDepRoute != INVALID_ROUTE_VALUE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn
        << StringPrintf(" - User modified ISO-DEP route = 0x%02X",
                        mUserDefaultIsoDepRoute);
    isoDepRoute = mUserDefaultIsoDepRoute;
  }
  int connectedIsoDepRoute =
      StSecureElement::getInstance().getConnectedNfceeId(isoDepRoute);

  /*********************************************************/
  if (connectedIsoDepRoute != NFC_DH_ID) {
    nfaStat = NFA_EeClearDefaultProtoRouting(connectedIsoDepRoute, protoMask);
    nfaStat = NFA_EeSetDefaultProtoRouting(
        connectedIsoDepRoute, protoMask, mSecureNfcEnabled ? 0 : protoMask, 0,
        mSecureNfcEnabled ? 0 : protoMask, mSecureNfcEnabled ? 0 : protoMask,
        mSecureNfcEnabled ? 0 : protoMask);
  } else {
    nfaStat = NFA_EeClearDefaultProtoRouting(NFC_DH_ID, protoMask);
    nfaStat = NFA_EeSetDefaultProtoRouting(
        NFC_DH_ID, protoMask, 0, 0, mSecureNfcEnabled ? 0 : protoMask, 0, 0);
  }
  if (nfaStat == NFA_STATUS_OK)
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn << ": Succeed to register default ISO-DEP route";
  else
    LOG(ERROR) << fn << ": failed to register default ISO-DEP route";

  // Default routing for T3T protocol
  if (!mIsScbrSupported) {
    SyncEventGuard guard(mRoutingEvent);
    tNFA_PROTOCOL_MASK protoMask = NFA_PROTOCOL_MASK_T3T;
    if (mDefaultEe == NFC_DH_ID) {
      nfaStat =
          NFA_EeSetDefaultProtoRouting(NFC_DH_ID, protoMask, 0, 0, 0, 0, 0);
    } else {
      nfaStat = NFA_EeClearDefaultProtoRouting(mDefaultEe, protoMask);
      nfaStat = NFA_EeSetDefaultProtoRouting(
          mDefaultEe, protoMask, 0, 0, mSecureNfcEnabled ? 0 : protoMask,
          mSecureNfcEnabled ? 0 : protoMask, mSecureNfcEnabled ? 0 : protoMask);
    }
    if (nfaStat == NFA_STATUS_OK)
      mRoutingEvent.wait();
    else
      LOG(ERROR) << fn << "Fail to set default proto routing for T3T";
  }
}

/*******************************************************************************
**
** Function:        updateDefaultRoute
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::updateDefaultRoute() {
  static const char fn[] = "StRoutingManager::updateDefaultRoute";

  DLOG_IF(INFO, nfc_debug_enabled) << fn;

  if (NFC_GetNCIVersion() != NCI_VERSION_2_0) return;

  /***************************************************/
  /*************** Check ISO-DEP route ***************/
  /***************************************************/
  int isoDepRoute = mDefaultIsoDepRoute;
  // Check if user has changed the defaukt routes through the NfcAdapterSettings
  // APIs
  if (mUserDefaultIsoDepRoute != INVALID_ROUTE_VALUE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn
        << StringPrintf(" - User modified ISO-DEP route = 0x%02X",
                        mUserDefaultIsoDepRoute);
    isoDepRoute = mUserDefaultIsoDepRoute;
  }
  int connectedIsoDepRoute =
      StSecureElement::getInstance().getConnectedNfceeId(isoDepRoute);

  /***************************************************/
  /***************** Check SC route ******************/
  /***************************************************/
  int scRoute = mDefaultSysCodeRoute;
  if (mUserDefaultScRoute != INVALID_ROUTE_VALUE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn
        << StringPrintf(" - User modified System Code route = 0x%02X",
                        mUserDefaultScRoute);
    scRoute = mUserDefaultScRoute;
  }
  int connectedDefaultSysCodeRoute =
      StSecureElement::getInstance().getConnectedNfceeId(scRoute);

  /***************************************************/
  /************ Check default AID route **************/
  /***************************************************/
  int aidRoute = mDefaultEe;
  if (mUserDefaultAidRoute != INVALID_ROUTE_VALUE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn
        << StringPrintf(" - User modified default AID route = 0x%02X",
                        mUserDefaultAidRoute);
    aidRoute = mUserDefaultAidRoute;
  }
  int connectedDefaultEe =
      StSecureElement::getInstance().getConnectedNfceeId(aidRoute);

  /****************************************************************/
  // Register System Code for routing
  SyncEventGuard guard(mRoutingEvent);
  tNFA_STATUS nfaStat = NFA_EeAddSystemCodeRouting(
      //      mDefaultSysCode, mDefaultSysCodeRoute,
      mDefaultSysCode, connectedDefaultSysCodeRoute,
      mSecureNfcEnabled ? 0x01 : mDefaultSysCodePowerstate);
  if (nfaStat == NFA_STATUS_NOT_SUPPORTED) {
    mIsScbrSupported = false;
    LOG(ERROR) << fn << ": SCBR not supported";
  } else if (nfaStat == NFA_STATUS_OK) {
    mIsScbrSupported = true;
    mRoutingEvent.wait();
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn << ": Succeed to register system code";
  } else {
    LOG(ERROR) << fn << ": Fail to register system code";
  }

  //  if (mDefaultEe != mDefaultIsoDepRoute) {
  if (connectedDefaultEe != connectedIsoDepRoute) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: mDefaultEe: 0x%02x", fn, mDefaultEe);

    uint8_t powerState = 0x01;
    if (!mSecureNfcEnabled)
      //      powerState = (mDefaultEe != 0x00) ? mOffHostAidRoutingPowerState :
      //      0x11;
      powerState =
          (connectedDefaultEe != 0x00) ? mOffHostAidRoutingPowerState : 0x11;

    //    nfaStat = NFA_EeAddAidRouting(mDefaultEe, 0, NULL, powerState,
    nfaStat = NFA_EeAddAidRouting(connectedDefaultEe, 0, NULL, powerState,
                                  AID_ROUTE_QUAL_PREFIX);
    if (nfaStat == NFA_STATUS_OK)
      DLOG_IF(INFO, nfc_debug_enabled)
          << fn << ": Succeed to register zero length AID";
    else
      LOG(ERROR) << fn << ": failed to register zero length AID";
  }
}

/*******************************************************************************
**
** Function:        updateEeTechRouteSetting
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
tNFA_TECHNOLOGY_MASK StRoutingManager::updateEeTechRouteSetting() {
  static const char fn[] = "StRoutingManager::updateEeTechRouteSetting()";
  tNFA_TECHNOLOGY_MASK allSeTechMask = 0x00;

  if (mDefaultOffHostRoute == 0 && mDefaultFelicaRoute == 0)
    return allSeTechMask;

  /***************************************************/
  /*************** Check Off Host route **************/
  /***************************************************/
  int offHostRoute = mDefaultOffHostRoute;
  // Check if user has changed the defaukt routes through the NfcAdapterSettings
  // APIs
  if (mUserDefaultOffHostRoute != INVALID_ROUTE_VALUE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn
        << StringPrintf(" - User modified OffHost AB route = 0x%02X",
                        mUserDefaultOffHostRoute);
    offHostRoute = mUserDefaultOffHostRoute;
  }
  int connectedDefaultOffHostRoute =
      StSecureElement::getInstance().getConnectedNfceeId(offHostRoute);

  /***************************************************/
  /**************** Check Felica route ***************/
  /***************************************************/
  int felicaRoute = mDefaultFelicaRoute;
  // Check if user has changed the defaukt routes through the NfcAdapterSettings
  // APIs
  if (mDefaultFelicaRoute != INVALID_ROUTE_VALUE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn
        << StringPrintf(" - User modified Felica route = 0x%02X",
                        mDefaultFelicaRoute);
    felicaRoute = mDefaultFelicaRoute;
  }
  int connectedDefaultFelicaRoute =
      StSecureElement::getInstance().getConnectedNfceeId(felicaRoute);

  /****************************************************************/
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Number of EE is %d", fn, mEeInfo.num_ee);

  tNFA_STATUS nfaStat;
  for (uint8_t i = 0; i < mEeInfo.num_ee; i++) {
    tNFA_HANDLE eeHandle = mEeInfo.ee_disc_info[i].ee_handle;
    tNFA_TECHNOLOGY_MASK seTechMask = 0;

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s   EE[%u] Handle: 0x%04x  techA: 0x%02x  techB: "
        "0x%02x  techF: 0x%02x  techBprime: 0x%02x",
        fn, i, eeHandle, mEeInfo.ee_disc_info[i].la_protocol,
        mEeInfo.ee_disc_info[i].lb_protocol,
        mEeInfo.ee_disc_info[i].lf_protocol,
        mEeInfo.ee_disc_info[i].lbp_protocol);

    if ((connectedDefaultOffHostRoute != 0) &&
        (eeHandle == (connectedDefaultOffHostRoute | NFA_HANDLE_GROUP_EE))) {
      if (mEeInfo.ee_disc_info[i].la_protocol != 0)
        seTechMask |= NFA_TECHNOLOGY_MASK_A;
      if (mEeInfo.ee_disc_info[i].lb_protocol != 0)
        seTechMask |= NFA_TECHNOLOGY_MASK_B;
    }

    if ((connectedDefaultFelicaRoute != 0) &&
        (eeHandle == (connectedDefaultFelicaRoute | NFA_HANDLE_GROUP_EE))) {
      if (mEeInfo.ee_disc_info[i].lf_protocol != 0)
        seTechMask |= NFA_TECHNOLOGY_MASK_F;
    }

    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: seTechMask[%u]=0x%02x", fn, i, seTechMask);
    if (seTechMask != 0x00) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "Configuring tech mask 0x%02x on EE 0x%04x", seTechMask, eeHandle);

      //      nfaStat = NFA_CeConfigureUiccListenTech(eeHandle, seTechMask);
      //      if (nfaStat != NFA_STATUS_OK)
      //        LOG(ERROR) << fn << "Failed to configure UICC listen
      //        technologies.";

      // clear previous before setting new power state
      nfaStat = NFA_EeClearDefaultTechRouting(eeHandle, seTechMask);
      if (nfaStat != NFA_STATUS_OK)
        LOG(ERROR) << fn << "Failed to clear EE technology routing.";

      nfaStat = NFA_EeSetDefaultTechRouting(
          eeHandle, seTechMask, mSecureNfcEnabled ? 0 : seTechMask, 0,
          mSecureNfcEnabled ? 0 : seTechMask,
          mSecureNfcEnabled ? 0 : seTechMask,
          mSecureNfcEnabled ? 0 : seTechMask);
      if (nfaStat != NFA_STATUS_OK)
        LOG(ERROR) << fn << "Failed to configure UICC technology routing.";

      allSeTechMask |= seTechMask;
    }
  }

  // Clear DH technology route on NFC-A
  if ((allSeTechMask & NFA_TECHNOLOGY_MASK_A) != 0) {
    nfaStat = NFA_EeClearDefaultTechRouting(NFC_DH_ID, NFA_TECHNOLOGY_MASK_A);
    if (nfaStat != NFA_STATUS_OK)
      LOG(ERROR) << "Failed to clear DH technology routing on NFC-A.";
  }

  // Clear DH technology route on NFC-F
  if ((allSeTechMask & NFA_TECHNOLOGY_MASK_F) != 0) {
    nfaStat = NFA_EeClearDefaultTechRouting(NFC_DH_ID, NFA_TECHNOLOGY_MASK_F);
    if (nfaStat != NFA_STATUS_OK)
      LOG(ERROR) << "Failed to clear DH technology routing on NFC-F.";
  }
  return allSeTechMask;
}

/*******************************************************************************
**
** Function:        updateFullRoutes
**
** Description:     Reprograms the ISO-DEP, SC and A/F tech routing (if no
**                  NFCEE supports them)
**                  This is done on NFCEE state changes (notifyEeUpdated())
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::updateFullRoutes() {
  static const char fn[] = "StRoutingManager::updateFullRoutes()";
  tNFA_STATUS nfaStat;

  DLOG_IF(INFO, nfc_debug_enabled) << fn;

  /***************************************************/
  /************* Check default AID route *************/
  /***************************************************/
  int aidRoute = mResolvedDefaultAidRoute;
  if (mUserDefaultAidRoute != INVALID_ROUTE_VALUE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn
        << StringPrintf(" - User modified default AID route = 0x%02X",
                        mUserDefaultAidRoute);
    aidRoute = mUserDefaultAidRoute;
  }
  int connectedDefaultEe =
      StSecureElement::getInstance().getConnectedNfceeId(aidRoute);

  DLOG_IF(INFO, nfc_debug_enabled)
      << fn
      << StringPrintf(" - Default AID route = 0x%02X", connectedDefaultEe);

  /***************************************************/
  /**************** Check ISO-DEP route **************/
  /***************************************************/
  int isoDepRoute = mDefaultIsoDepRoute;
  // Check if user has changed the defaukt routes through the NfcAdapterSettings
  // APIs
  if (mUserDefaultIsoDepRoute != INVALID_ROUTE_VALUE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn
        << StringPrintf(" - User modified ISO-DEP route = 0x%02X",
                        mUserDefaultIsoDepRoute);
    isoDepRoute = mUserDefaultIsoDepRoute;
  }
  int connectedIsoDepRoute =
      StSecureElement::getInstance().getConnectedNfceeId(isoDepRoute);

  DLOG_IF(INFO, nfc_debug_enabled)
      << fn
      << StringPrintf(" - Default ISO-DEP route = 0x%02X",
                      connectedIsoDepRoute);

  /***************************************************/
  /****************** Check SC route *****************/
  /***************************************************/
  int scRoute = mDefaultSysCodeRoute;
  if (mUserDefaultScRoute != INVALID_ROUTE_VALUE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn
        << StringPrintf(" - User modified System Code route = 0x%02X",
                        mUserDefaultScRoute);
    scRoute = mUserDefaultScRoute;
  }
  int connectedDefaultSysCodeRoute =
      StSecureElement::getInstance().getConnectedNfceeId(scRoute);

  DLOG_IF(INFO, nfc_debug_enabled)
      << fn
      << StringPrintf(" - Default System Code route = 0x%02X",
                      connectedDefaultSysCodeRoute);

  /***************************************************/
  /*************** Check Off Host route **************/
  /***************************************************/
  int offHostRoute = mDefaultOffHostRoute;
  // Check if user has changed the defaukt routes through the NfcAdapterSettings
  // APIs
  if (mUserDefaultOffHostRoute != INVALID_ROUTE_VALUE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn
        << StringPrintf(" - User modified OffHost route = 0x%02X",
                        mUserDefaultOffHostRoute);
    offHostRoute = mUserDefaultOffHostRoute;
  }
  int connectedDefaultOffHostRoute =
      StSecureElement::getInstance().getConnectedNfceeId(offHostRoute);

  DLOG_IF(INFO, nfc_debug_enabled)
      << fn
      << StringPrintf(" - Default Tech A/B route = 0x%02X",
                      connectedDefaultOffHostRoute);

  /***************************************************/
  /*************** Check Felica route ****************/
  /***************************************************/
  int felicaRoute = mDefaultFelicaRoute;
  // Check if user has changed the defaukt routes through the NfcAdapterSettings
  // APIs
  if (mUserDefaultFelicaRoute != INVALID_ROUTE_VALUE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn
        << StringPrintf(" - User modified Felica route = 0x%02X",
                        mUserDefaultFelicaRoute);
    felicaRoute = mUserDefaultFelicaRoute;
  }
  int connectedDefaultFelicaRoute =
      StSecureElement::getInstance().getConnectedNfceeId(felicaRoute);

  DLOG_IF(INFO, nfc_debug_enabled)
      << fn
      << StringPrintf(" - Default Felica route = 0x%02X",
                      connectedDefaultFelicaRoute);

  /***************************************************/
  /*************** Check MIFARE route ****************/
  /***************************************************/
  int connectedMifareRoute = INVALID_ROUTE_VALUE;
  if (mUserDefaultMifareRoute != INVALID_ROUTE_VALUE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn
        << StringPrintf(" - User set a MIFARE route = 0x%02X",
                        mUserDefaultMifareRoute);
    connectedMifareRoute = StSecureElement::getInstance().getConnectedNfceeId(
        mUserDefaultMifareRoute);
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << fn
      << StringPrintf(" - Default MIFARE route = 0x%02X", connectedMifareRoute);

  /**************************************************************/
  /**************** Update default AID routes *******************/
  /**************************************************************/

  // If there was no AID updated
  if (mAidRoutingConfigured == false) {
    if (connectedDefaultEe != connectedIsoDepRoute) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: Default AID route was not set and is different from ISO-DEP "
          "route - "
          "connectedDefaultEe: 0x%02x",
          fn, connectedDefaultEe);

      uint8_t powerState = 0x01;
      if (!mSecureNfcEnabled)
        powerState =
            (connectedDefaultEe != 0x00) ? mOffHostAidRoutingPowerState : 0x11;

      nfaStat = NFA_EeAddAidRouting(connectedDefaultEe, 0, NULL, powerState,
                                    AID_ROUTE_QUAL_PREFIX);
      if (nfaStat == NFA_STATUS_OK)
        DLOG_IF(INFO, nfc_debug_enabled)
            << fn << ": Succeed to register zero length AID";
      else
        LOG(ERROR) << fn << ": failed to register zero length AID";
    }
  }

  /**************************************************************/
  /***************** Update protocol routes *********************/
  /**************************************************************/

  // Default Routing for ISO-DEP
  tNFA_PROTOCOL_MASK protoMask = NFA_PROTOCOL_MASK_ISO_DEP;

  if (connectedIsoDepRoute != NFC_DH_ID) {
    nfaStat = NFA_EeSetDefaultProtoRouting(
        connectedIsoDepRoute, protoMask, mSecureNfcEnabled ? 0 : protoMask, 0,
        mSecureNfcEnabled ? 0 : protoMask, mSecureNfcEnabled ? 0 : protoMask,
        mSecureNfcEnabled ? 0 : protoMask);
  } else {
    nfaStat = NFA_EeSetDefaultProtoRouting(
        NFC_DH_ID, protoMask, 0, 0, mSecureNfcEnabled ? 0 : protoMask, 0, 0);
  }
  if (nfaStat == NFA_STATUS_OK)
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn << ": Succeed to register default ISO-DEP route";
  else
    LOG(ERROR) << fn << ": failed to register default ISO-DEP route";

  /**************************************************************/
  /**************** Update System code route ********************/
  /**************************************************************/

  // Check if default host has changed while HCE-F is ON
  // in this case need to reprog the default SC route
  // please note that this should only happen during tests
  int reconfHceFNeeded = false;
  if ((connectedDefaultSysCodeRoute != mConnectedDefaultScRoute) &&
      (mScRoutingConfigured == true)) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << fn << ": Default system code route has changed while HCE-F is on";
    // Register System Code for routing
    SyncEventGuard guard(mRoutingEvent);
    tNFA_STATUS nfaStat = NFA_EeRemoveSystemCodeRouting(mDefaultSysCode);
    if (nfaStat == NFA_STATUS_NOT_SUPPORTED) {
      mIsScbrSupported = false;
      LOG(ERROR) << fn << ": SCBR not supported";
    } else if (nfaStat == NFA_STATUS_OK) {
      mIsScbrSupported = true;
      mRoutingEvent.wait();
      DLOG_IF(INFO, nfc_debug_enabled)
          << fn << ": Succeed to de-register system code";

      reconfHceFNeeded = true;
    } else {
      LOG(ERROR) << fn << ": Fail to deregister system code";
    }
  }

  if ((mScRoutingConfigured == false) || (reconfHceFNeeded)) {
    // Register System Code for routing
    SyncEventGuard guard(mRoutingEvent);
    tNFA_STATUS nfaStat = NFA_EeAddSystemCodeRouting(
        mDefaultSysCode, connectedDefaultSysCodeRoute,
        mSecureNfcEnabled ? 0x01 : mDefaultSysCodePowerstate);
    if (nfaStat == NFA_STATUS_NOT_SUPPORTED) {
      mIsScbrSupported = false;
      LOG(ERROR) << fn << ": SCBR not supported";
    } else if (nfaStat == NFA_STATUS_OK) {
      mIsScbrSupported = true;
      mRoutingEvent.wait();
      DLOG_IF(INFO, nfc_debug_enabled)
          << fn << ": Succeed to register system code";
    } else {
      LOG(ERROR) << fn << ": Fail to register system code";
    }
  }

  /**************************************************************/
  /******************** Update techs route **********************/
  /**************************************************************/

  mSeTechMask = 0;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Number of EE is %d", fn, mEeInfo.num_ee);

  for (uint8_t i = 0; i < mEeInfo.num_ee; i++) {
    tNFA_HANDLE eeHandle = mEeInfo.ee_disc_info[i].ee_handle;
    tNFA_TECHNOLOGY_MASK seTechMask = 0;

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s   EE[%u] Handle: 0x%04x  techA: 0x%02x  techB: "
        "0x%02x  techF: 0x%02x  techBprime: 0x%02x",
        fn, i, eeHandle, mEeInfo.ee_disc_info[i].la_protocol,
        mEeInfo.ee_disc_info[i].lb_protocol,
        mEeInfo.ee_disc_info[i].lf_protocol,
        mEeInfo.ee_disc_info[i].lbp_protocol);

    /*** Mifare routing/Tech A ***/
    if ((connectedMifareRoute != INVALID_ROUTE_VALUE) &&
        (eeHandle == (connectedMifareRoute | NFA_HANDLE_GROUP_EE))) {
      if ((mEeInfo.ee_disc_info[i].la_protocol & NFA_PROTOCOL_MASK_T2T) != 0) {
        seTechMask |= NFA_TECHNOLOGY_MASK_A;
      }
    }

    if ((connectedDefaultOffHostRoute != 0) &&
        (eeHandle == (connectedDefaultOffHostRoute | NFA_HANDLE_GROUP_EE))) {
      // If MIFARE was not routed, and this
      if ((mEeInfo.ee_disc_info[i].la_protocol != 0) && (seTechMask == 0))
        seTechMask |= NFA_TECHNOLOGY_MASK_A;
      if (mEeInfo.ee_disc_info[i].lb_protocol != 0)
        seTechMask |= NFA_TECHNOLOGY_MASK_B;
    }

    if ((connectedDefaultFelicaRoute != 0) &&
        (eeHandle == (connectedDefaultFelicaRoute | NFA_HANDLE_GROUP_EE))) {
      if (mEeInfo.ee_disc_info[i].lf_protocol != 0)
        seTechMask |= NFA_TECHNOLOGY_MASK_F;
    }

    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: seTechMask[%u]=0x%02x", fn, i, seTechMask);
    if (seTechMask != 0x00) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "Configuring tech mask 0x%02x on EE 0x%04x", seTechMask, eeHandle);

      nfaStat = NFA_EeSetDefaultTechRouting(
          eeHandle, seTechMask, mSecureNfcEnabled ? 0 : seTechMask, 0,
          mSecureNfcEnabled ? 0 : seTechMask,
          mSecureNfcEnabled ? 0 : seTechMask,
          mSecureNfcEnabled ? 0 : seTechMask);
      if (nfaStat != NFA_STATUS_OK)
        LOG(ERROR) << fn << "Failed to configure UICC technology routing.";

      mSeTechMask |= seTechMask;
    }
  }

  /**************************************************************/
  /****************** Update A/F tech route *********************/
  /************ to DH is not supported by UICC ******************/
  /**************************************************************/
  {
    SyncEventGuard guard(mRoutingEvent);

    // Route Nfc-A to host if we don't have a SE
    tNFA_TECHNOLOGY_MASK techMask = NFA_TECHNOLOGY_MASK_A;
    if ((NfcStExtensions::HCE_TECH_MASK & NFA_TECHNOLOGY_MASK_A) &&
        ((mSeTechMask & NFA_TECHNOLOGY_MASK_A) == 0)) {
      nfaStat = NFA_EeSetDefaultTechRouting(
          NFC_DH_ID, techMask, 0, 0, mSecureNfcEnabled ? 0 : techMask, 0, 0);
      if (nfaStat == NFA_STATUS_OK)
        mRoutingEvent.wait();
      else
        LOG(ERROR) << fn << "Fail to set default tech routing for Nfc-A";
    }

    //    tNFA_TECHNOLOGY_MASK techMask = NFA_TECHNOLOGY_MASK_B;
    techMask = NFA_TECHNOLOGY_MASK_B;
    if ((NfcStExtensions::HCE_TECH_MASK & NFA_TECHNOLOGY_MASK_B) &&
        ((mSeTechMask & NFA_TECHNOLOGY_MASK_B) == 0)) {
      nfaStat = NFA_EeSetDefaultTechRouting(
          NFC_DH_ID, techMask, 0, 0, mSecureNfcEnabled ? 0 : techMask, 0, 0);
      if (nfaStat == NFA_STATUS_OK)
        mRoutingEvent.wait();
      else
        LOG(ERROR) << fn << "Fail to set default tech routing for Nfc-B";
    }

    // Route Nfc-F to host if we don't have a SE
    techMask = NFA_TECHNOLOGY_MASK_F;
    if ((mSeTechMask & NFA_TECHNOLOGY_MASK_F) == 0) {
      nfaStat = NFA_EeSetDefaultTechRouting(
          NFC_DH_ID, techMask, 0, 0, mSecureNfcEnabled ? 0 : techMask, 0, 0);
      if (nfaStat == NFA_STATUS_OK)
        mRoutingEvent.wait();
      else
        LOG(ERROR) << fn << "Fail to set default tech routing for Nfc-F";
    }
  }

  mConnectedDefaultOffHostRoute = connectedDefaultOffHostRoute;
  mConnectedDefaultIsoDepRoute = connectedIsoDepRoute;
  mConnectedDefaultFelicaRoute = connectedDefaultFelicaRoute;
  mConnectedDefaultMifareRoute = connectedMifareRoute;
  mConnectedDefaultScRoute = connectedDefaultSysCodeRoute;
  mConnectedDefaultAidRoute = connectedDefaultEe;
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
void StRoutingManager::nfaEeCallback(tNFA_EE_EVT event,
                                     tNFA_EE_CBACK_DATA* eventData) {
  static const char fn[] = "StRoutingManager::nfaEeCallback";
  StSecureElement& se = StSecureElement::getInstance();
  StRoutingManager& routingManager = StRoutingManager::getInstance();
  if (eventData) routingManager.mCbEventData = *eventData;

  switch (event) {
    case NFA_EE_REGISTER_EVT: {
      SyncEventGuard guard(routingManager.mEeRegisterEvent);
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_REGISTER_EVT; status=%u", fn, eventData->ee_register);
      routingManager.mEeRegisterEvent.notifyOne();
    } break;

    case NFA_EE_DEREGISTER_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_DEREGISTER_EVT; status=0x%X", fn, eventData->status);
      routingManager.mReceivedEeInfo = false;
      routingManager.mDeinitializing = false;
    } break;

    case NFA_EE_MODE_SET_EVT: {
      se.notifyModeSet((eventData->mode_set));
      SyncEventGuard guard(routingManager.mEeSetModeEvent);
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_MODE_SET_EVT; status: 0x%04X  handle: 0x%04X  ", fn,
          eventData->mode_set.status, eventData->mode_set.ee_handle);
      routingManager.mEeSetModeEvent.notifyOne();
    } break;

    case NFA_EE_SET_TECH_CFG_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_SET_TECH_CFG_EVT; status=0x%X", fn, eventData->status);
      SyncEventGuard guard(routingManager.mRoutingEvent);
      routingManager.mRoutingEvent.notifyOne();
    } break;

    case NFA_EE_CLEAR_TECH_CFG_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_CLEAR_TECH_CFG_EVT; status=0x%X", fn, eventData->status);
      SyncEventGuard guard(routingManager.mRoutingEvent);
      routingManager.mRoutingEvent.notifyOne();
    } break;

    case NFA_EE_SET_PROTO_CFG_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_SET_PROTO_CFG_EVT; status=0x%X", fn, eventData->status);
      SyncEventGuard guard(routingManager.mRoutingEvent);
      routingManager.mRoutingEvent.notifyOne();
    } break;

    case NFA_EE_CLEAR_PROTO_CFG_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_CLEAR_PROTO_CFG_EVT; status=0x%X", fn, eventData->status);
      SyncEventGuard guard(routingManager.mRoutingEvent);
      routingManager.mRoutingEvent.notifyOne();
    } break;

    case NFA_EE_STATUS_NTF_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_STATUS_NTF_EVT; status: 0x%04X  nfcee_id: 0x%04X", fn,
          eventData->status_ntf.status,
          ((tNFA_EE_STATUS_NTF*)eventData)->nfcee_id);

      se.notifyEeStatus(
          ((tNFA_EE_STATUS_NTF*)eventData)->nfcee_id | NFA_HANDLE_GROUP_EE,
          eventData->status_ntf.status);
    } break;

    case NFA_EE_POWER_CTRL_EVT: {
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_POWER_CTRL_EVT; status: 0x%04X  nfcee_id: 0x%04X", fn,
          eventData->power_ctrl_rsp.status, eventData->power_ctrl_rsp.nfcee_id);
      se.notifyPowerCtrlRsp();
    } break;

    case NFA_EE_ACTION_EVT: {
      tNFA_EE_ACTION& action = eventData->action;
      if (action.trigger == NFC_EE_TRIG_SELECT)
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: NFA_EE_ACTION_EVT; h=0x%X; trigger=select (0x%X)", fn,
            action.ee_handle, action.trigger);
      else if (action.trigger == NFC_EE_TRIG_APP_INIT) {
        tNFC_APP_INIT& app_init = action.param.app_init;
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: NFA_EE_ACTION_EVT; h=0x%X; trigger=app-init "
            "(0x%X); aid len=%u; data len=%u",
            fn, action.ee_handle, action.trigger, app_init.len_aid,
            app_init.len_data);
      } else if (action.trigger == NFC_EE_TRIG_RF_PROTOCOL)
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: NFA_EE_ACTION_EVT; h=0x%X; trigger=rf protocol (0x%X)", fn,
            action.ee_handle, action.trigger);
      else if (action.trigger == NFC_EE_TRIG_RF_TECHNOLOGY)
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: NFA_EE_ACTION_EVT; h=0x%X; trigger=rf tech (0x%X)", fn,
            action.ee_handle, action.trigger);
      else
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: NFA_EE_ACTION_EVT; h=0x%X; unknown trigger (0x%X)", fn,
            action.ee_handle, action.trigger);
    } break;

    case NFA_EE_DISCOVER_REQ_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_DISCOVER_REQ_EVT; status=0x%X; num ee=%u", __func__,
          eventData->discover_req.status, eventData->discover_req.num_ee);
      SyncEventGuard guard(routingManager.mEeInfoEvent);
      memcpy(&routingManager.mEeInfo, &eventData->discover_req,
             sizeof(routingManager.mEeInfo));
      if (routingManager.mReceivedEeInfo && !routingManager.mDeinitializing) {
        routingManager.mEeInfoChanged = true;
        routingManager.notifyEeUpdated();
      }
      routingManager.mReceivedEeInfo = true;
      routingManager.mEeInfoEvent.notifyOne();
    } break;

    case NFA_EE_NO_CB_ERR_EVT:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_NO_CB_ERR_EVT  status=%u", fn, eventData->status);
      break;

    case NFA_EE_ADD_AID_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_ADD_AID_EVT  status=%u", fn, eventData->status);
      SyncEventGuard guard(routingManager.mRoutingEvent);
      routingManager.mAidRoutingConfigured =
          (eventData->status == NFA_STATUS_OK);
      routingManager.mRoutingEvent.notifyOne();
    } break;

    case NFA_EE_ADD_SYSCODE_EVT: {
      SyncEventGuard guard(routingManager.mRoutingEvent);
      routingManager.mRoutingEvent.notifyOne();
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_ADD_SYSCODE_EVT  status=%u", fn, eventData->status);
    } break;

    case NFA_EE_REMOVE_SYSCODE_EVT: {
      SyncEventGuard guard(routingManager.mRoutingEvent);
      routingManager.mRoutingEvent.notifyOne();
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_REMOVE_SYSCODE_EVT  status=%u", fn, eventData->status);
    } break;

    case NFA_EE_REMOVE_AID_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_REMOVE_AID_EVT  status=%u", fn, eventData->status);
      SyncEventGuard guard(routingManager.mRoutingEvent);
      routingManager.mAidRoutingConfigured =
          (eventData->status == NFA_STATUS_OK);
      routingManager.mRoutingEvent.notifyOne();
    } break;

    case NFA_EE_NEW_EE_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_EE_NEW_EE_EVT  h=0x%X; status=%u", fn,
          eventData->new_ee.ee_handle, eventData->new_ee.ee_status);
    } break;

    case NFA_EE_UPDATED_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_EE_UPDATED_EVT", fn);
      SyncEventGuard guard(routingManager.mEeUpdateEvent);
      routingManager.mEeUpdateEvent.notifyOne();
      routingManager.notifyDefaultRouteSet(
          routingManager.mConnectedDefaultAidRoute,
          routingManager.mConnectedDefaultMifareRoute,
          routingManager.mConnectedDefaultIsoDepRoute,
          routingManager.mConnectedDefaultFelicaRoute,
          routingManager.mConnectedDefaultOffHostRoute,
          routingManager.mConnectedDefaultScRoute);
    } break;

    case NFA_EE_FORCE_ROUTING_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_EE_FORCE_ROUTING_EVT", fn);
      SyncEventGuard guard(routingManager.mEeForceRoutingEvent);
      routingManager.mEeForceRoutingEvent.notifyOne();
    } break;

    case NFA_EE_DISCOVER_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_EE_DISCOVER_EVT", fn);
      SyncEventGuard guard(routingManager.mEeDiscoverEvent);
      routingManager.mEeDiscoverEvent.notifyOne();
    } break;
    default:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: unknown event=%u ????", fn, event);
      break;
  }
}

/*******************************************************************************
**
** Function:        registerT3tIdentifier
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
int StRoutingManager::registerT3tIdentifier(uint8_t* t3tId, uint8_t t3tIdLen) {
  static const char fn[] = "RoutingManager::registerT3tIdentifier";

  DLOG_IF(INFO, nfc_debug_enabled)
      << fn << ": Start to register NFC-F system on DH";

  if (t3tIdLen != (2 + NCI_RF_F_UID_LEN + NCI_T3T_PMM_LEN)) {
    LOG(ERROR) << fn << ": Invalid length of T3T Identifier";
    return NFA_HANDLE_INVALID;
  }

  mNfcFOnDhHandle = NFA_HANDLE_INVALID;

  uint16_t systemCode;
  uint8_t nfcid2[NCI_RF_F_UID_LEN];
  uint8_t t3tPmm[NCI_T3T_PMM_LEN];

  systemCode = (((int)t3tId[0] << 8) | ((int)t3tId[1] << 0));
  memcpy(nfcid2, t3tId + 2, NCI_RF_F_UID_LEN);
  memcpy(t3tPmm, t3tId + 10, NCI_T3T_PMM_LEN);
  {
    SyncEventGuard guard(mRoutingEvent);
    tNFA_STATUS nfaStat = NFA_CeRegisterFelicaSystemCodeOnDH(
        systemCode, nfcid2, t3tPmm, nfcFCeCallback);
    if (nfaStat == NFA_STATUS_OK) {
      mRoutingEvent.wait();
    } else {
      LOG(ERROR) << fn << ": Fail to register NFC-F system on DH";
      return NFA_HANDLE_INVALID;
    }
  }
  DLOG_IF(INFO, nfc_debug_enabled)
      << fn << ": Succeed to register NFC-F system on DH";

  // Register System Code for routing
  if (mIsScbrSupported) {
    SyncEventGuard guard(mRoutingEvent);
    tNFA_STATUS nfaStat = NFA_EeAddSystemCodeRouting(systemCode, NCI_DH_ID,
                                                     SYS_CODE_PWR_STATE_HOST);
    if (nfaStat == NFA_STATUS_OK) {
      mRoutingEvent.wait();
    }

    mScRoutingConfigured = true;

    if ((nfaStat != NFA_STATUS_OK) || (mCbEventData.status != NFA_STATUS_OK)) {
      LOG(ERROR) << StringPrintf("%s: Fail to register system code on DH", fn);
      return NFA_HANDLE_INVALID;
    }
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: Succeed to register system code on DH , systemCode = %X", fn,
        systemCode);
    // add handle and system code pair to the map
    mMapScbrHandle.emplace(mNfcFOnDhHandle, systemCode);
  } else {
    LOG(ERROR) << StringPrintf("%s: SCBR Not supported", fn);
  }

  return mNfcFOnDhHandle;
}

/*******************************************************************************
**
** Function:        deregisterT3tIdentifier
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::deregisterT3tIdentifier(int handle) {
  static const char fn[] = "RoutingManager::deregisterT3tIdentifier";

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Start to deregister NFC-F system on DH", fn);
  {
    SyncEventGuard guard(mRoutingEvent);
    tNFA_STATUS nfaStat = NFA_CeDeregisterFelicaSystemCodeOnDH(handle);
    if (nfaStat == NFA_STATUS_OK) {
      mRoutingEvent.wait();
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: Succeeded in deregistering NFC-F system on DH", fn);
    } else {
      LOG(ERROR) << StringPrintf("%s: Fail to deregister NFC-F system on DH",
                                 fn);
    }
  }
  if (mIsScbrSupported) {
    map<int, uint16_t>::iterator it = mMapScbrHandle.find(handle);
    // find system code for given handle
    if (it != mMapScbrHandle.end()) {
      uint16_t systemCode = it->second;
      mMapScbrHandle.erase(handle);
      if (systemCode != 0) {
        SyncEventGuard guard(mRoutingEvent);
        tNFA_STATUS nfaStat = NFA_EeRemoveSystemCodeRouting(systemCode);
        if (nfaStat == NFA_STATUS_OK) {
          mRoutingEvent.wait();
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: Succeeded in deregistering system Code on DH", fn);
        } else {
          LOG(ERROR) << StringPrintf("%s: Fail to deregister system Code on DH",
                                     fn);
        }

        mScRoutingConfigured = false;
      }
    }
  }
}

/*******************************************************************************
**
** Function:        nfcFCeCallback
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::nfcFCeCallback(uint8_t event,
                                      tNFA_CONN_EVT_DATA* eventData) {
  static const char fn[] = "StRoutingManager::nfcFCeCallback";
  StRoutingManager& routingManager = StRoutingManager::getInstance();

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: 0x%x", __func__, event);

  switch (event) {
    case NFA_CE_REGISTERED_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: registerd event notified", fn);
      routingManager.mNfcFOnDhHandle = eventData->ce_registered.handle;
      SyncEventGuard guard(routingManager.mRoutingEvent);
      routingManager.mRoutingEvent.notifyOne();
    } break;
    case NFA_CE_DEREGISTERED_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: deregisterd event notified", fn);
      SyncEventGuard guard(routingManager.mRoutingEvent);
      routingManager.mRoutingEvent.notifyOne();
    } break;
    case NFA_CE_ACTIVATED_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: activated event notified", fn);
      routingManager.notifyActivated(NFA_TECHNOLOGY_MASK_F);
    } break;
    case NFA_CE_DEACTIVATED_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: deactivated event notified", fn);
      routingManager.notifyDeactivated(NFA_TECHNOLOGY_MASK_F);
    } break;
    case NFA_CE_DATA_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: data event notified", fn);
      tNFA_CE_DATA& ce_data = eventData->ce_data;
      routingManager.handleData(NFA_TECHNOLOGY_MASK_F, ce_data.p_data,
                                ce_data.len, ce_data.status);
    } break;
    default: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: unknown event=%u ????", fn, event);
    } break;
  }
}

/*******************************************************************************
**
** Function:        setNfcSecure
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
bool StRoutingManager::setNfcSecure(bool enable) {
  mSecureNfcEnabled = enable;
  DLOG_IF(INFO, true) << "setNfcSecure() " << enable;
  return true;
}

/*******************************************************************************
**
** Function:        setNfcSecure
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::setUserDefaultRoutesPref(int mifareRoute,
                                                int isoDepRoute,
                                                int felicaRoute,
                                                int abTechRoute, int scRoute,
                                                int aidRoute) {
  static const char fn[] = "StRoutingManager::setUserDefaultRoutesPref";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: mifareRoute: 0x%02X, isoDepRoute: 0x%02X, "
      "felicaRoute: 0x%02X, abTechRoute: 0x%02X, scRoute: 0x%02X, "
      "aidRoute: 0x%02X",
      fn, mifareRoute, isoDepRoute, felicaRoute, abTechRoute, scRoute,
      aidRoute);

  mUserDefaultOffHostRoute = abTechRoute;
  mUserDefaultIsoDepRoute = isoDepRoute;
  mUserDefaultFelicaRoute = felicaRoute;
  mUserDefaultMifareRoute = mifareRoute;
  mUserDefaultScRoute = scRoute;
  mUserDefaultAidRoute = aidRoute;

  // Reset value to static one.
  //  If needed, will be updated by call to AddAidRouting() from CE code
  mResolvedDefaultAidRoute = mDefaultEe;

  if (mIsInit == false) {
    // This is to force a full update
    mEeInfoChanged = true;

    //    commitRouting();
  }

  mIsInit = false;
}

/*******************************************************************************
**
** Function:        deinitialize
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StRoutingManager::deinitialize() {
  onNfccShutdown();
  NFA_EeDeregister(nfaEeCallback);
}

/*******************************************************************************
**
** Function:        registerJniFunctions
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
int StRoutingManager::registerJniFunctions(JNIEnv* e) {
  static const char fn[] = "StRoutingManager::registerJniFunctions";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", fn);
  return jniRegisterNativeMethods(
      e, "com/android/nfc/cardemulation/AidRoutingManager", sMethods,
      NELEM(sMethods));
}

/*******************************************************************************
**
** Function:        com_android_nfc_cardemulation_doGetDefaultRouteDestination
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
int StRoutingManager::
    com_android_nfc_cardemulation_doGetDefaultRouteDestination(JNIEnv*) {
  return getInstance().mDefaultEe;
}

/*******************************************************************************
**
** Function: com_android_nfc_cardemulation_doGetDefaultOffHostRouteDestination
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
int StRoutingManager::
    com_android_nfc_cardemulation_doGetDefaultOffHostRouteDestination(JNIEnv*) {
  return getInstance().mDefaultOffHostRoute;
}

/*******************************************************************************
**
** Function: com_android_nfc_cardemulation_doGetOffHostUiccDestination
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
jbyteArray
StRoutingManager::com_android_nfc_cardemulation_doGetOffHostUiccDestination(
    JNIEnv* e) {
  std::vector<uint8_t> uicc = getInstance().mOffHostRouteUicc;
  if (uicc.size() == 0) {
    return NULL;
  }
  CHECK(e);
  jbyteArray uiccJavaArray = e->NewByteArray(uicc.size());
  CHECK(uiccJavaArray);
  e->SetByteArrayRegion(uiccJavaArray, 0, uicc.size(), (jbyte*)&uicc[0]);
  return uiccJavaArray;
}

/*******************************************************************************
**
** Function: com_android_nfc_cardemulation_doGetOffHostEseDestination
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
jbyteArray
StRoutingManager::com_android_nfc_cardemulation_doGetOffHostEseDestination(
    JNIEnv* e) {
  std::vector<uint8_t> ese = getInstance().mOffHostRouteEse;
  if (ese.size() == 0) {
    return NULL;
  }
  CHECK(e);
  jbyteArray eseJavaArray = e->NewByteArray(ese.size());
  CHECK(eseJavaArray);
  e->SetByteArrayRegion(eseJavaArray, 0, ese.size(), (jbyte*)&ese[0]);
  return eseJavaArray;
}

/*******************************************************************************
**
** Function: com_android_nfc_cardemulation_doGetAidMatchingMode
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
int StRoutingManager::com_android_nfc_cardemulation_doGetAidMatchingMode(
    JNIEnv*) {
  return getInstance().mAidMatchingMode;
}

/*******************************************************************************
**
** Function: com_android_nfc_cardemulation_doGetDefaultIsoDepRouteDestination
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
int StRoutingManager::
    com_android_nfc_cardemulation_doGetDefaultIsoDepRouteDestination(JNIEnv*) {
  return getInstance().mDefaultIsoDepRoute;
}

/*******************************************************************************
**
** Function:        clearAidTable
**
** Description:     Receive execution environment-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
bool StRoutingManager::clearAidTable() {
  static const char fn[] = "StRoutingManager::clearAidTable()";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  SyncEventGuard guard(mRoutingEvent);

  tNFA_STATUS nfaStat = NFA_EeRemoveAidRouting(NFA_REMOVE_ALL_AID_LEN,
                                               (uint8_t*)NFA_REMOVE_ALL_AID);

  if (nfaStat == NFA_STATUS_OK) {
    mRoutingEvent.wait();
    mAidRoutingConfigured = false;
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: removed AID", fn);
    return true;
  } else {
    LOG(ERROR) << StringPrintf("%s: failed to remove AID", fn);
    return false;
  }
}
