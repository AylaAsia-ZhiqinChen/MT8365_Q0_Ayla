/*
 * Copyright (C) 2012 The Android Open Source Project
 * Copyright (C) 2014 ST Microelectronics S.A.
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
#include "JavaClassConstants.h"
#include "StSecureElement.h"
#include "StNfcJni.h"
#include "NfcStExtensions.h"
#include "StNfcTag.h"
#include "nfc_config.h"

using android::base::StringPrintf;

extern bool nfc_debug_enabled;

namespace android {

extern void com_android_nfc_stNfcManager_disableDiscovery(JNIEnv* e, jobject o);
extern void com_android_nfc_stNfcManager_enableDiscovery(JNIEnv* e, jobject o,
                                                         jint mode);

// These must match the EE_ERROR_ types in NfcService.java
static const int EE_ERROR_INIT = -3;

// for APDU gate with NFC off
static bool sSeEnabledForApduGate = false;

/*******************************************************************************
**
** Function:        stNativeNfcStSecureElement_doOpenSecureElementConnection
**
** Description:     Connect to the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jint stNativeNfcSecureElement_doOpenSecureElementConnection(JNIEnv*,
                                                                   jobject) {
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  bool stat = true;
  jint secElemHandle = EE_ERROR_INIT;
  StSecureElement& se = StSecureElement::getInstance();
  sSeEnabledForApduGate = false;
  se.SeActivationLock();

  // if controller is not routing AND there is no pipe connected,
  // then turn on the sec elem
  if (!se.isBusy() && (se.getSENfceeId(se.ESE_ID) != 0x0)) {
    /* NCI 2.0 - Begin */
    // stat = se.activate(0);
    uint8_t nfceeId = se.getSENfceeId(se.ESE_ID);
    bool seIsConnected = se.isSEConnected((int)nfceeId);
    if (!seIsConnected) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: going to enable %02hhx", __func__, nfceeId);
      stat = se.EnableSE(nfceeId, true);
      if (stat == true) {
        sSeEnabledForApduGate = true;
      }
    } else {
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: not enabling %02hhx, already connected", __func__, nfceeId);
    }
    /* NCI 2.0 - End */
  }

  /* NCI 2.0 - Begin */
  if (stat == true)
  /* NCI 2.0 - End */
  {
    // establish a pipe to sec elem
    stat = se.connectEE();
    if (stat) {
      /* NCI 2.0 - Begin */
      secElemHandle = se.EE_HANDLE_HCI_NETWORK;
      /* NCI 2.0 - End */
    } else {
      se.deactivate(0);
    }
  }

  se.SeActivationUnlock();
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit; return handle=0x%X", __func__, secElemHandle);
  return secElemHandle;
}

/*******************************************************************************
**
** Function: stNativeNfcStSecureElement_doDisconnectSecureElementConnection
**
** Description:     Disconnect from the secure element.
**                  e: JVM environment.
**                  o: Java object.
**                  handle: Handle of secure element.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean stNativeNfcSecureElement_doDisconnectSecureElementConnection(
    JNIEnv*, jobject, jint handle) {
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; handle=0x%04x", __func__, handle);
  bool stat = false;

  StSecureElement& se = StSecureElement::getInstance();
  se.SeActivationLock();
  stat = se.disconnectEE(handle);

  // if controller is not routing AND there is no pipe connected,
  // then turn off the sec elem
  if (sSeEnabledForApduGate) {
    uint8_t nfceeId = se.getSENfceeId(se.ESE_ID);
    (void)se.EnableSE(nfceeId, false);
    sSeEnabledForApduGate = false;
  }
  se.SeActivationUnlock();
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return stat ? JNI_TRUE : JNI_FALSE;
}

/*******************************************************************************
**
** Function:        stNativeNfcSecureElement_doTransceive
**
** Description:     Send data to the secure element; retrieve response.
**                  e: JVM environment.
**                  o: Java object.
**                  handle: Secure element's handle.
**                  data: Data to send.
**
** Returns:         Buffer of received data.
**
*******************************************************************************/
static jbyteArray stNativeNfcSecureElement_doTransceive(JNIEnv* e, jobject,
                                                        jint handle,
                                                        jbyteArray data) {
  const int32_t recvBufferMaxSize = 1024;
  uint8_t recvBuffer[recvBufferMaxSize];
  int32_t recvBufferActualSize = 0;
  int timeout = NfcTag::getInstance().getTransceiveTimeout(
      TARGET_TYPE_ISO14443_4);  // NFC service expects JNI to use ISO-DEP's
                                // timeout
  ScopedByteArrayRW bytes(e, data);

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter; handle=0x%X; buf len=%zu", __func__, handle, bytes.size());
  StSecureElement::getInstance().transceive(
      reinterpret_cast<uint8_t*>(&bytes[0]), bytes.size(), recvBuffer,
      recvBufferMaxSize, recvBufferActualSize, timeout);

  // copy results back to java
  jbyteArray result = e->NewByteArray(recvBufferActualSize);
  if (result != NULL) {
    e->SetByteArrayRegion(result, 0, recvBufferActualSize, (jbyte*)recvBuffer);
  }

  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit: recv len=%d", __func__, recvBufferActualSize);
  return result;
}

/*****************************************************************************
**
** Description:     JNI functions
**
*****************************************************************************/
static JNINativeMethod gMethods[] = {
    {"doNativeOpenSecureElementConnection", "()I",
     (void*)stNativeNfcSecureElement_doOpenSecureElementConnection},
    {"doNativeDisconnectSecureElementConnection", "(I)Z",
     (void*)stNativeNfcSecureElement_doDisconnectSecureElementConnection},
    {"doTransceive", "(I[B)[B", (void*)stNativeNfcSecureElement_doTransceive}};

/*******************************************************************************
**
** Function:        register_com_android_nfc_stNativeNfcSecureElement
**
** Description:     Regisgter JNI functions with Java Virtual Machine.
**                  e: Environment of JVM.
**
** Returns:         Status of registration.
**
*******************************************************************************/
int register_com_android_nfc_stNativeNfcSecureElement(JNIEnv* e) {
  return jniRegisterNativeMethods(e, gStNativeNfcSecureElementClassName,
                                  gMethods, NELEM(gMethods));
}

}  // namespace android
