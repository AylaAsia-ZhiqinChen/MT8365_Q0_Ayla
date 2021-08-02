/*
 *  Copyright (C) 2018 ST Microelectronics S.A.
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
#include "StNfcDtaJni.h"

#include "dtaJniApi.h"

class NfcStDtaExtensions {
 public:
  static NfcStDtaExtensions& getInstance();

  bool initialize();
  bool deinitialize();

  uint32_t enableDiscovery(uint8_t, uint8_t, uint8_t, bool, bool, uint8_t,
                           uint8_t);
  bool disableDiscovery();

  void setPatternNb(uint32_t);
  void setCrVersion(uint8_t);
  void setConnectionDevicesLimit(uint8_t, uint8_t, uint8_t, uint8_t);
  void setListenNfcaUidMode(uint8_t);
  void setT4atNfcdepPrio(uint8_t);
  void setFsdFscExtension(bool);
  void setLlcpMode(uint32_t);
  void setSnepMode(uint8_t, uint8_t, uint8_t, uint8_t, bool);

 private:
  static NfcStDtaExtensions sStDtaExtensions;
  static const char* APP_NAME;

  tJNI_DTA_INFO dta_info;

  /*******************************************************************************
  **
  ** Function:        NfcStDtaExtensions
  **
  ** Description:     Initialize member variables.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  NfcStDtaExtensions();

  /*******************************************************************************
  **
  ** Function:        ~NfcStDtaExtensions
  **
  ** Description:     Release all resources.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  ~NfcStDtaExtensions();
};
