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

#include <base/logging.h>
#include <android-base/stringprintf.h>

#include <nativehelper/ScopedLocalRef.h>
#include <nativehelper/ScopedPrimitiveArray.h>
#include <cutils/properties.h>

#include "nfa_api.h"

#include "NfcStDtaExtensions.h"
#include "StNfcDtaJni.h"

using android::base::StringPrintf;
extern bool nfc_debug_enabled;

/*****************************************************************************
 **
 ** public variables
 **
 *****************************************************************************/

namespace android {}

typedef struct tagStDTAservice {
  void *dtaHdl;
  bool dtaRunning;

} TstDTAservice;

char g_initParms[] = "DTAIF=DTA_TCIF_NFCFORUM IXIT=DEFAULT";

static void dtaCallback(void *context, TStateDta state, char *data,
                        uint32_t length);

//////////////////////////////////////////////
//////////////////////////////////////////////

NfcStDtaExtensions NfcStDtaExtensions::sStDtaExtensions;

const char *NfcStDtaExtensions::APP_NAME = "nfc_st_dta_ext";

/*******************************************************************************
 **
 ** Function:        NfcStDtaExtensions
 **
 ** Description:     Initialize member variables.
 **
 ** Returns:         None
 **
 *******************************************************************************/
NfcStDtaExtensions::NfcStDtaExtensions() {}

/*******************************************************************************
 **
 ** Function:        ~NfcStDtaExtensions
 **
 ** Description:     Release all resources.
 **
 ** Returns:         None
 **
 *******************************************************************************/
NfcStDtaExtensions::~NfcStDtaExtensions() {}

/*******************************************************************************
 **
 ** Function:        getInstance
 **
 ** Description:     Get the StSecureElement singleton object.
 **
 ** Returns:         StSecureElement object.
 **
 *******************************************************************************/
NfcStDtaExtensions &NfcStDtaExtensions::getInstance() {
  return sStDtaExtensions;
}

bool NfcStDtaExtensions::initialize() {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  dta_info.dta_mode = DTA_MODE_DP;

  return true;
}

bool NfcStDtaExtensions::deinitialize() {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  return true;
}

void NfcStDtaExtensions::setPatternNb(uint32_t patternNb) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; patternNb=0x%u", __func__, patternNb);
  dta_info.pattern_nb = patternNb;
}

void NfcStDtaExtensions::setCrVersion(uint8_t ver) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; CR version=%d", __func__, ver);
  dta_info.cr_version = ver;
}

void NfcStDtaExtensions::setConnectionDevicesLimit(uint8_t cdlA, uint8_t cdlB,
                                                   uint8_t cdlF, uint8_t cdlV) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; CDL_A=%d, CDL_B=%d, CDL_F=%d, CDL_V=%d",
                      __func__, cdlA, cdlB, cdlF, cdlV);
  dta_info.cdl_A = cdlA;
  dta_info.cdl_B = cdlB;
  dta_info.cdl_F = cdlF;
  dta_info.cdl_V = cdlV;
}

void NfcStDtaExtensions::setListenNfcaUidMode(uint8_t mode) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; Nfc-A UID mode=%u", __func__, mode);
  dta_info.nfca_uid_gen_mode = mode;
}

void NfcStDtaExtensions::setT4atNfcdepPrio(uint8_t prio) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  if ((prio & NFA_PROTOCOL_MASK_NFC_DEP) == NFA_PROTOCOL_MASK_NFC_DEP) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: NFC-DEP priority over T4AT", __func__);
    dta_info.t4at_nfcdep_prio = 0;
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: T4AT priority over NFC-DEP", __func__);
    dta_info.t4at_nfcdep_prio = 1;
  }
}

void NfcStDtaExtensions::setFsdFscExtension(bool ext) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  (ext == false) ? DLOG_IF(INFO, nfc_debug_enabled)
                       << StringPrintf("%s: enter; RF frame 256B", __func__)
                 : DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
                       "%s: enter; RF frame extension 4096B", __func__);
  dta_info.ext_rf_frame = ext;
}

void NfcStDtaExtensions::setLlcpMode(uint32_t miux_mode) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  dta_info.dta_mode = DTA_MODE_LLCP;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: MIUXiut mode=%u", __func__, miux_mode);
  dta_info.miux_mode = miux_mode;
}

void NfcStDtaExtensions::setSnepMode(uint8_t role, uint8_t server_type,
                                     uint8_t request_type, uint8_t data_type,
                                     bool disc_incorrect_len) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  dta_info.dta_mode = DTA_MODE_SNEP;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: role=%u, server_type=%u, request_type=%u, data=%u, "
      "disc_incorrect_len=%u",
      __func__, role, server_type, request_type, data_type, disc_incorrect_len);
  dta_info.role = role;
  dta_info.server_type = server_type;
  dta_info.request_type = request_type;
  dta_info.data_type = data_type;
  dta_info.disc_incorrect_len = disc_incorrect_len;
}

uint32_t NfcStDtaExtensions::enableDiscovery(
    uint8_t con_poll, uint8_t con_listen_dep, uint8_t con_listen_t4tp,
    bool con_listen_t3tp, bool con_listen_acm, uint8_t con_bitr_f,
    uint8_t con_bitr_acm) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  // dta_info.con_poll_A = ((con_poll & NFA_TECHNOLOGY_MASK_A) | (con_poll &
  // NFA_TECHNOLOGY_MASK_ACTIVE)) ? 1 : 0;
  dta_info.con_poll_A = (con_poll & NFA_TECHNOLOGY_MASK_A) ? 1 : 0;
  dta_info.con_poll_B = (con_poll & NFA_TECHNOLOGY_MASK_B) ? 1 : 0;
  // dta_info.con_poll_F = ((con_poll & NFA_TECHNOLOGY_MASK_F) | (con_poll &
  // NFA_TECHNOLOGY_MASK_ACTIVE)) ? 1 : 0;
  dta_info.con_poll_F = (con_poll & NFA_TECHNOLOGY_MASK_F) ? 1 : 0;
  dta_info.con_poll_V = (con_poll & NFA_TECHNOLOGY_MASK_V) ? 1 : 0;
  dta_info.con_poll_ACM = (con_poll & NFA_TECHNOLOGY_MASK_ACTIVE) ? 1 : 0;

  dta_info.con_listen_dep_A = (con_listen_dep & NFA_TECHNOLOGY_MASK_A) ? 1 : 0;
  dta_info.con_listen_dep_F = (con_listen_dep & NFA_TECHNOLOGY_MASK_F) ? 1 : 0;
  dta_info.con_listen_t4Atp = (con_listen_t4tp & NFA_TECHNOLOGY_MASK_A) ? 1 : 0;
  dta_info.con_listen_t4Btp = (con_listen_t4tp & NFA_TECHNOLOGY_MASK_B) ? 1 : 0;
  dta_info.con_listen_t3tp = con_listen_t3tp;
  dta_info.con_listen_acm = con_listen_acm;

  dta_info.con_bitr_f = con_bitr_f;
  dta_info.con_bitr_acm = con_bitr_acm;

  return dtaInitialize(&dta_info.handle, (char *)g_initParms, dtaCallback,
                       &dta_info);
}

bool NfcStDtaExtensions::disableDiscovery() {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  uint8_t status;

  status = dtaShutdown(dta_info.handle);
  memset(&dta_info, 0, sizeof(tJNI_DTA_INFO));
  return status;
}

static void dtaCallback(void *context, TStateDta state, char *data,
                        uint32_t length) {
  // TstDTAservice *pService = (TstDTAservice *) context;

  switch (state) {
    case stDtaReady:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: enter; NFC stack started, discovery running", __func__);
      // pService->dtaRunning = true;
      break;

    case stDtaStackStopped:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: enter; NFC stack stopped", __func__);
      // pService->dtaRunning = false;
      break;

    case stDtaError:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: enter; DTA stated error!", __func__);
      break;

    case stDtaTcRunning:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: enter; DTA stated test case running", __func__);
      break;

    case stDtaTcStopped:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: enter; DTA stated test case stopped", __func__);
      break;

    default:
      break;
  }  // switch

}  // dtaCallback
