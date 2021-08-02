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

#pragma once
#include "SyncEvent.h"
#include "DataQueue.h"
#include "StNfcJni.h"

#include "nfa_ee_api.h"
#include "nfa_hci_api.h"
#include "nfa_hci_defs.h"
#include "nfa_ce_api.h"

class StCardEmulationEmbedded {
 public:
  /*******************************************************************************
   **
   ** Function:        finalize
   **
   ** Description:     Cleans-ip variables before end of operation
   **
   ** Returns:         None.
   **
   *******************************************************************************/
  void finalize();

  /*******************************************************************************
   **
   ** Function:        abortWaits
   **
   ** Description:     Called if an error occurs during the transmission of a
   **                  command to the NFA HCI. All waiting events are then
   **                  aborted.
   **
   ** Returns:         None.
   **
   *******************************************************************************/
  void abortWaits();

  /*******************************************************************************
  **
  ** Function:        getInstance
  **
  ** Description:     Get the SecureElement singleton object.
  **
  ** Returns:         SecureElement object.
  **
  *******************************************************************************/
  static StCardEmulationEmbedded& getInstance();

  /*******************************************************************************
  **
  ** Function:        initialize
  **
  ** Description:     Initialize all member variables.
  **                  native: Native data.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  void initialize(nfc_jni_native_data* native);

  bool enable(uint8_t ceeId, bool enable);

  bool connect(uint8_t ceeId);

  bool transceive(uint8_t ceeId, uint16_t tx_data_len, uint8_t* tx_data,
                  uint16_t& rx_data_size, uint8_t* rx_data);

  bool disconnect(uint8_t ceeId);

 private:
  static StCardEmulationEmbedded sCardEmulationEmbedded;
  nfc_jni_native_data* mNativeData;

  SyncEvent mEeRegisterEvent;
  SyncEvent mEeSetModeEvent;
  /* NCI 2.0 - Begin */
  SyncEvent mEeSetModeNtfEvent;
  /* NCI 2.0 - End */
  SyncEvent mEeCreateConnEvent;
  SyncEvent mEeDataEvent;
  SyncEvent mEeDisconnEvent;

  static const uint8_t CEE_T4T_APP = 0xE1;
  static const uint8_t CEE_ISO_APP = 0xE2;
  static const uint8_t CEE_NO_APP = 0xFF;
  static const uint8_t MAX_NUM_EE = 6;  // HCI, UICC, eSE, uSD, T4T, ISO
  static const uint16_t MAX_RESPONSE_SIZE = 1024;

  uint8_t mIdConnectCee;
  tNFA_EE_INFO mEeInfo[MAX_NUM_EE];  // actual size stored in mActualNumEe
  uint8_t mResponseData[MAX_RESPONSE_SIZE];
  uint16_t mResponseDataLength;

  bool mNeedRfRestart;
  tNFA_STATUS mNfaEECbStatus;

  /*******************************************************************************
  **
  ** Function:        StCardEmulationEmbedded
  **
  ** Description:     Initialize member variables.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  StCardEmulationEmbedded();

  /*******************************************************************************
  **
  ** Function:        ~StCardEmulationEmbedded
  **
  ** Description:     Release all resources.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  ~StCardEmulationEmbedded();

  static void nfaEeCallback(tNFA_EE_EVT event, tNFA_EE_CBACK_DATA* eventData);
};
