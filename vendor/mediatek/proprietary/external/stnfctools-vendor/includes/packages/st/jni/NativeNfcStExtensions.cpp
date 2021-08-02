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

//#include <string.h>
#include <dlfcn.h>
#include "NfcStExtensions.h"
#include "StNfcJni.h"

#include "StCardEmulationEmbedded.h"

using android::base::StringPrintf;
extern bool nfc_debug_enabled;

//#include <cutils/properties.h>
//#include <limits.h>
/* NCI 2.0 - Begin */
#define IS_64BIT (sizeof(void *) == 8)
/* NCI 2.0 - End */

namespace android {
/* NCI 2.0 - Begin */
const char *JNIVersion = "JNI version 00.12.16.10";
/* NCI 2.0 - End */

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_getFirmwareVersion
**
** Description:     Connect to the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jbyteArray nativeNfcStExtensions_getFirmwareVersion(JNIEnv *e, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  // jbyteArray result = NULL;
  uint8_t version_sw[NfcStExtensions::getInstance().FW_VERSION_SIZE];
  /* NCI 2.0 - Begin */
  memset(version_sw, 0, sizeof version_sw);
  /* NCI 2.0 - End */
  int ret;
  jbyteArray result;

  ret = NfcStExtensions::getInstance().getFirmwareVersion(version_sw);

  {
    // copy results back to java
    result = e->NewByteArray(NfcStExtensions::getInstance().FW_VERSION_SIZE);
    if (result != NULL) {
      e->SetByteArrayRegion(result, 0,
                            NfcStExtensions::getInstance().FW_VERSION_SIZE,
                            (jbyte *)version_sw);
    }
  }

  return result;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_getHWVersion
**
** Description:     Connect to the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jbyteArray nativeNfcStExtensions_getHWVersion(JNIEnv *e, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  jbyteArray result = NULL;
  uint8_t version_hw[NfcStExtensions::getInstance().HW_VERSION_SIZE];
  /* NCI 2.0 - Begin */
  memset(version_hw, 0, sizeof version_hw);
  /* NCI 2.0 - End */
  int ret;

  ret = NfcStExtensions::getInstance().getHWVersion(version_hw);

  {
    // copy results back to java
    result = e->NewByteArray(NfcStExtensions::getInstance().HW_VERSION_SIZE);
    if (result != NULL) {
      e->SetByteArrayRegion(result, 0,
                            NfcStExtensions::getInstance().HW_VERSION_SIZE,
                            (jbyte *)version_hw);
    }
  }

  return result;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_getCRCConfiguration
**
** Description:     Connect to the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jbyteArray nativeNfcStExtensions_getCRCConfiguration(JNIEnv *e,
                                                            jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  jbyteArray result = NULL;
  uint8_t crc_config[NfcStExtensions::getInstance().CRC_CONFIG_SIZE];
  /* NCI 2.0 - Begin */
  memset(crc_config, 0, sizeof crc_config);
  /* NCI 2.0 - End */
  int ret;

  ret = NfcStExtensions::getInstance().getCRCConfiguration(crc_config);

  // if (ret)
  {
    // copy results back to java
    result = e->NewByteArray(NfcStExtensions::getInstance().CRC_CONFIG_SIZE);
    if (result != NULL) {
      e->SetByteArrayRegion(result, 0,
                            NfcStExtensions::getInstance().CRC_CONFIG_SIZE,
                            (jbyte *)crc_config);
    }
  }

  return result;
}

/* NCI 2.0 - Begin */
/*******************************************************************************
**
** Function:        nativeNfcStExtensions_getCustomerData
**
** Description:     Connect to the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jbyteArray nativeNfcStExtensions_getCustomerData(JNIEnv *e, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  uint8_t customerData[8];
  memset(customerData, 0, sizeof(customerData));
  int ret;
  jbyteArray result;

  ret = NfcStExtensions::getInstance().getCustomerData(customerData);

  {
    // copy results back to java
    result = e->NewByteArray(sizeof(customerData));
    if (result != NULL) {
      e->SetByteArrayRegion(result, 0, sizeof(customerData),
                            (jbyte *)customerData);
    }
  }

  return result;
}
/* NCI 2.0 - End */

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_getSWVersion
**
** Description:     Connect to the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jbyteArray nativeNfcStExtensions_getSWVersion(JNIEnv *e, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  jbyteArray result = NULL;
  void *handle = 0;
  int len = 0, res_len = 0;
  const char *separator = "+";

  // char hal_path[PATH_MAX];
  //    char hal_name[NAME_MAX];
  char lib_path[PATH_MAX];
  if (IS_64BIT) {
    // strcpy (hal_path,"/vendor/lib64/");
    strcpy(lib_path, "/system/lib64/");
  } else {
    //   strcpy (hal_path,"/vendor/lib/");
    strcpy(lib_path, "/system/lib/");
  }

  /* if (property_get("ro.product.board", hal_name, NULL) == 0)
   {
       DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: ro.product.board
   not defined, cannot find hal library", __func__);
   }
   else*/
  //{

  // strcat (hal_path,"hw/nfc_nci.st21nfc.default.so");
  // DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: loading hal library
  // %s", __func__, hal_path); handle = dlopen(hal_path, RTLD_LAZY); if
  // (!handle) { LOG(ERROR) << StringPrintf("%s: failed to load the library",
  // __func__); return NULL;
  //}
  //}
  char **temp;  // = (char**) dlsym(handle, "halVersion");

  //    len = strlen(*temp);
  //   char * hal_version = (char *) malloc(len+1);
  //   memcpy(hal_version, *temp, (len+1));
  //   dlclose(handle);
  //   DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: HAL version is %s",
  //   __func__, hal_version); res_len += len;

  // Get Core stack version - idx 1
  strcat(lib_path, "libstnfc-nci.so");
  handle = dlopen(lib_path, RTLD_LAZY);
  if (!handle) {
    LOG(ERROR) << StringPrintf("%s: failed to load the library", __func__);
    // free(hal_version);
    return NULL;
  }
  temp = (char **)dlsym(handle, "coreStackVersion");

  len = strlen(*temp);
  char *core_stack_version = (char *)malloc(len + 1);
  memcpy(core_stack_version, *temp, (len + 1));
  dlclose(handle);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: Core stack version is %s", __func__, core_stack_version);
  res_len += len;

  // Get NfcService/JNI version -idx 2 => JNIVersion in this file
  len = strlen(JNIVersion);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: JNI version is %s;", __func__, JNIVersion);
  res_len += len;

  // Android version - idx 3

  handle = dlopen(lib_path, RTLD_LAZY);
  if (!handle) {
    LOG(ERROR) << StringPrintf("%s: failed to load the library", __func__);
    // free(hal_version);
    free(core_stack_version);
    return NULL;
  }
  temp = (char **)dlsym(handle, "androidVersion");

  len = strlen(*temp);
  char *android_version = (char *)malloc(len + 1);
  memcpy(android_version, *temp, (len + 1));
  dlclose(handle);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Android version is %s", __func__, android_version);
  res_len += len;

  // Framework version is handled in framework -idx 4

  // copy results back to java
  char *sw_version = (char *)malloc(res_len + 3 + 1);  // 3 separators, final \0
  // DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: Found length is %d",
  // __func__, (len+3));
  if (sw_version && core_stack_version && android_version) {
    //       strcpy(sw_version, hal_version);
    //      strcat(sw_version, separator);
    strcat(sw_version, core_stack_version);
    strcat(sw_version, separator);
    strcat(sw_version, JNIVersion);
    strcat(sw_version, separator);
    strcat(sw_version, android_version);

    result = e->NewByteArray(res_len + 3 + 1);
    if (result != NULL) {
      e->SetByteArrayRegion(result, 0, (res_len + 3 + 1), (jbyte *)sw_version);
    }
  }

  /* NCI 2.0 - Begin */
  free(sw_version);
  free(core_stack_version);
  free(android_version);
  // free(hal_version);
  /* NCI 2.0 - End */

  return result;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_isUiccConnected
**
** Description:     Check if the UICC is connected.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jboolean nativeNfcStExtensions_isUiccConnected(JNIEnv *e, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  jboolean result;

  result = NfcStExtensions::getInstance().isSEConnected(0x02);

  return result;
}
/* NCI 2.0 - Begin */
/*******************************************************************************
**
** Function:        nativeNfcStExtensions_iseSEConnected
**
** Description:     Check if the eSE is connected.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jboolean nativeNfcStExtensions_iseSEConnected(JNIEnv *e, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  jboolean result;

  result = NfcStExtensions::getInstance().isSEConnected(0xC0);
  return result;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_isSEConnected
**
** Description:     Check if the given host HostID is connected.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jboolean nativeNfcStExtensions_isSEConnected(JNIEnv *e, jobject,
                                                    jint HostID) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  jboolean result;

  result = NfcStExtensions::getInstance().isSEConnected(HostID);

  return result;
}
/* NCI 2.0 - End */

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_startLoopback
**
** Description:     Connect to the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jint nativeNfcStExtensions_loopback(JNIEnv *e, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  jint ret;

  ret = NfcStExtensions::getInstance().handleLoopback();
  return ret;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_getHceUserProp
**
** Description:     Connect to the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jint nativeNfcStExtensions_getHceUserProp(JNIEnv *e, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  uint32_t res = true;

  res = NfcStExtensions::getInstance().getHceUserProp();
  return res;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_setRfConfiguration
**
** Description:     Connect to the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static void nativeNfcStExtensions_setRfConfiguration(JNIEnv *e, jobject,
                                                     jint modeBitmap,
                                                     jbyteArray techArray) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  ScopedByteArrayRW bytes(e, techArray);
  NfcStExtensions::getInstance().setRfConfiguration(
      modeBitmap, reinterpret_cast<uint8_t *>(&bytes[0]));
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_getRfConfiguration
**
** Description:     Connect to the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static int nativeNfcStExtensions_getRfConfiguration(JNIEnv *e, jobject,
                                                    jbyteArray techArray) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  uint8_t outputArray[NfcStExtensions::getInstance().RF_CONFIG_ARRAY_SIZE];
  int modeBitmap;

  modeBitmap = NfcStExtensions::getInstance().getRfConfiguration(outputArray);

  e->SetByteArrayRegion(techArray, 0,
                        NfcStExtensions::getInstance().RF_CONFIG_ARRAY_SIZE,
                        (jbyte *)outputArray);

  return modeBitmap;
}

static void nativeNfcStExtensions_setRfBitmap(JNIEnv *e, jobject,
                                              jint modeBitmap) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  NfcStExtensions::getInstance().setRfBitmap(modeBitmap);
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_updatePipesInfo
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
static jint nativeNfcStExtensions_updatePipesInfo(JNIEnv *e, jobject o) {
  int pipe_id;
  jint host_id;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  /* NCI 2.0 - Begin */
  bool res = NfcStExtensions::getInstance().getPipesInfo();
  /* NCI 2.0 - End */

  if (res == true) {
    pipe_id = NfcStExtensions::getInstance().getPipeIdForGate(
        NfcStExtensions::getInstance().DH_HCI_ID, 0x41);

    if (pipe_id != 0xFF) {
      host_id = NfcStExtensions::getInstance().getHostIdForPipe(pipe_id);
      if (host_id != 0xFF) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit:", __func__);
        return host_id;
      }
    }
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit:", __func__);
  return 0xFF;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_setDefaultOffHostRoute
**
** Description:     Get the technology / protocol routing table.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static void nativeNfcStExtensions_setDefaultOffHostRoute(JNIEnv *e, jobject,
                                                         jint route) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  NfcStExtensions::getInstance().setDefaultOffHostRoute(route);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit:", __func__);
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_getDefaultOffHostRoute
**
** Description:     Get the technology / protocol routing table.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jint nativeNfcStExtensions_getDefaultOffHostRoute(JNIEnv *e, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  jint route;

  route = NfcStExtensions::getInstance().getDefaultOffHostRoute();

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit:", __func__);
  return route;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_getProprietaryConfigSettings
**
** Description:     Get the Proprietary Config Settings.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
/* NCI 2.0 - Begin */
static jboolean nativeNfcStExtensions_getProprietaryConfigSettings(
    JNIEnv *e, jobject, jint SubSetId, jint byteNb, jint bitNb)
/* NCI 2.0 - End */
{
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  bool status = false;

  /* NCI 2.0 - Begin */
  status = NfcStExtensions::getInstance().getProprietaryConfigSettings(
      SubSetId, byteNb, bitNb);
  /* NCI 2.0 - End */

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit:", __func__);
  return status;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_setProprietaryConfigSettings
**
** Description:     Set the Proprietary Config Settings.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
/* NCI 2.0 - Begin */
static void nativeNfcStExtensions_setProprietaryConfigSettings(
    JNIEnv *e, jobject, jint SubSetId, jint byteNb, jint bitNb, jboolean status)
/* NCI 2.0 - End */
{
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  /* NCI 2.0 - Begin */
  NfcStExtensions::getInstance().setProprietaryConfigSettings(SubSetId, byteNb,
                                                              bitNb, status);
  /* NCI 2.0 - End */

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit:", __func__);
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_getPipesList
**
** Description:     Get the list and status of pipes for a given host.
**                  e: JVM environment.
**                  o: Java object.
**                  host_id: Host to investigate
**
** Returns:         Java object containing Pipes information
**
*******************************************************************************/
static jint nativeNfcStExtensions_getPipesList(JNIEnv *e, jobject, jint host_id,
                                               jbyteArray list) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  int i, nb_pipes = 0, host_idx;

  /* NCI 2.0 - Begin */
  NfcStExtensions::getInstance().getPipesInfo();
  /* NCI 2.0 - End */

  if (host_id == 1) {
    host_idx = NfcStExtensions::getInstance().DH_IDX;
  } else if (host_id == 2) {
    host_idx = NfcStExtensions::getInstance().UICC_IDX;
  } else if (host_id == 192) {
    host_idx = NfcStExtensions::getInstance().ESE_IDX;
  } else {
    return 0xff;
  }

  nb_pipes = NfcStExtensions::getInstance().mPipesInfo[host_idx].nb_pipes;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: nb_pipes: %d", __func__, nb_pipes);

  uint8_t listArray[nb_pipes];

  for (i = 0; i < nb_pipes; i++) {
    listArray[i] =
        NfcStExtensions::getInstance().mPipesInfo[host_idx].data[i].pipe_id;
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: listArray[%d]= 0x%x", __func__, i, listArray[i]);
  }

  e->SetByteArrayRegion(list, 0, nb_pipes, (jbyte *)listArray);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit:", __func__);
  return nb_pipes;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_getPipeInfo
**
** Description:     Get the list and status of pipes for a given host.
**                  e: JVM environment.
**                  o: Java object.
**                  host_id: Host to investigate
**
** Returns:         Java object containing Pipes information
**
*******************************************************************************/
static void nativeNfcStExtensions_getPipeInfo(JNIEnv *e, jobject, jint host_id,
                                              jint pipe_id, jbyteArray info) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  int i, nb_pipes = 0, host_idx;

  if (host_id == 1) {
    host_idx = NfcStExtensions::getInstance().DH_IDX;
  } else if (host_id == 2) {
    host_idx = NfcStExtensions::getInstance().UICC_IDX;
  } else if (host_id == 192) {
    host_idx = NfcStExtensions::getInstance().ESE_IDX;
  } else {
    return;
  }
  nb_pipes = NfcStExtensions::getInstance().mPipesInfo[host_idx].nb_pipes;

  if (nb_pipes == 0xFF) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: getPipesInfo() was not called!!;", __func__);
    return;
  }

  uint8_t infoArray[4] = {0xFF, 0xFF, 0xFF, 0xFF};

  for (i = 0; i < nb_pipes; i++) {
    if (NfcStExtensions::getInstance().mPipesInfo[host_idx].data[i].pipe_id ==
        pipe_id) {
      infoArray[0] = NfcStExtensions::getInstance()
                         .mPipesInfo[host_idx]
                         .data[i]
                         .pipe_state;
      infoArray[1] = NfcStExtensions::getInstance()
                         .mPipesInfo[host_idx]
                         .data[i]
                         .source_host;
      infoArray[2] = NfcStExtensions::getInstance()
                         .mPipesInfo[host_idx]
                         .data[i]
                         .source_gate;
      infoArray[3] =
          NfcStExtensions::getInstance().mPipesInfo[host_idx].data[i].dest_host;
      break;
    }
  }

  e->SetByteArrayRegion(info, 0, 4, (jbyte *)infoArray);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit:", __func__);
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_getAtr
**
** Description:     Get the list and status of pipes for a given host.
**                  e: JVM environment.
**                  o: Java object.
**                  host_id: Host to investigate
**
** Returns:         Java object containing Pipes information
**
*******************************************************************************/
static jbyteArray nativeNfcStExtensions_getAtr(JNIEnv *e, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  int length;
  uint8_t data[30];
  jbyteArray atr = NULL;

  length = NfcStExtensions::getInstance().getATR(data);

  atr = e->NewByteArray(length);
  e->SetByteArrayRegion(atr, 0, length, (jbyte *)data);

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit:", __func__);
  return atr;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_EnableSE
**
** Description:     Connect/disconnect the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jboolean nativeNfcStExtensions_EnableSE(JNIEnv *e, jobject, jint se_id,
                                               jboolean enable) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  jboolean result;

  result = NfcStExtensions::getInstance().EnableSE(se_id, enable);

  return result;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_connectEE
**
** Description:     Connect/disconnect the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jboolean nativeNfcStExtensions_connectEE(JNIEnv *e, jobject,
                                                jint cee_id) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  jboolean result;

  result = StCardEmulationEmbedded::getInstance().connect(cee_id);

  return result;
}

/*********************************************************************************
** Function:        nativeNfcStExtensions_transceiveEE
**
** Description:     Get the list and status of pipes for a given host.
**                  e: JVM environment.
**                  o: Java object.
**                  host_id: Host to investigate
**
** Returns:         Java object containing Pipes information
**
*******************************************************************************/
static jbyteArray nativeNfcStExtensions_transceiveEE(JNIEnv *e, jobject,
                                                     jint cee_id,
                                                     jbyteArray data_cmd) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  uint8_t recvBuffer[1024];
  uint16_t recvBufferActualSize = 0;
  ScopedByteArrayRW bytes(e, data_cmd);
  bool rslt = false;

  rslt = StCardEmulationEmbedded::getInstance().transceive(
      cee_id, bytes.size(), reinterpret_cast<uint8_t *>(&bytes[0]),
      recvBufferActualSize, recvBuffer);

  jbyteArray result = e->NewByteArray(recvBufferActualSize);
  if (result != NULL) {
    e->SetByteArrayRegion(result, 0, recvBufferActualSize, (jbyte *)recvBuffer);
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: exit: received data length =%d", __func__, recvBufferActualSize);
  return result;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_disconnectEE
**
** Description:     Connect/disconnect the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jboolean nativeNfcStExtensions_disconnectEE(JNIEnv *e, jobject,
                                                   jint cee_id) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  jboolean result;

  result = StCardEmulationEmbedded::getInstance().disconnect(cee_id);

  return result;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_connectGate
**
** Description:     Connect/disconnect the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jint nativeNfcStExtensions_connectGate(JNIEnv *e, jobject, jint host_id,
                                              jint gate_id) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  jint result;

  result = NfcStExtensions::getInstance().connectGate(host_id, gate_id);

  return result;
}

/*********************************************************************************
** Function:        nativeNfcStExtensions_transceive
**
** Description:     Get the list and status of pipes for a given host.
**                  e: JVM environment.
**                  o: Java object.
**                  host_id: Host to investigate
**
** Returns:         Java object containing Pipes information
**
*******************************************************************************/
static jbyteArray nativeNfcStExtensions_transceive(JNIEnv *e, jobject,
                                                   jint pipe_id, jint hciCmd,
                                                   jbyteArray data_cmd) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  uint8_t recvBuffer[1024];
  int32_t recvBufferActualSize = 0;
  ScopedByteArrayRW bytes(e, data_cmd);
  bool rslt = false;

  rslt = NfcStExtensions::getInstance().transceive(
      pipe_id, hciCmd, bytes.size(), reinterpret_cast<uint8_t *>(&bytes[0]),
      recvBufferActualSize, recvBuffer);

  jbyteArray result = e->NewByteArray(recvBufferActualSize);
  if (result != NULL) {
    e->SetByteArrayRegion(result, 0, recvBufferActualSize, (jbyte *)recvBuffer);
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: exit: received data length =%d", __func__, recvBufferActualSize);
  return result;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_disconnectGate
**
** Description:     Connect/disconnect the secure element.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static void nativeNfcStExtensions_disconnectGate(JNIEnv *e, jobject,
                                                 jint pipe_id) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  NfcStExtensions::getInstance().disconnectGate(pipe_id);
}

/* NCI 2.0 - Begin */
/*******************************************************************************
**
** Function:        nativeNfcStExtensions_setNciConfig
**
** Description:     Set a NCI parameter.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static void nativeNfcStExtensions_setNciConfig(JNIEnv *e, jobject,
                                               jint param_id, jbyteArray param,
                                               jint length) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  ScopedByteArrayRW bytes(e, param);
  NfcStExtensions::getInstance().setNciConfig(
      param_id, reinterpret_cast<uint8_t *>(&bytes[0]), length);
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_getNciConfig
**
** Description:     Set a NCI parameter.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static jbyteArray nativeNfcStExtensions_getNciConfig(JNIEnv *e, jobject,
                                                     jint param_id) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  uint16_t recvBufferActualSize = 0;
  uint8_t recvBuffer[256];

  NfcStExtensions::getInstance().getNciConfig(param_id, recvBuffer,
                                              recvBufferActualSize);

  jbyteArray result = e->NewByteArray(recvBufferActualSize);
  if (result != NULL) {
    e->SetByteArrayRegion(result, 0, recvBufferActualSize, (jbyte *)recvBuffer);
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: exit: received data length =%d", __func__, recvBufferActualSize);
  return result;
}

/*******************************************************************************
**
** Function:        nativeNfcStExtensions_getAvailableHciHostList
**
** Description:     Set a NCI parameter.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Handle of secure element.  values < 0 represent failure.
**
*******************************************************************************/
static int nativeNfcStExtensions_getAvailableHciHostList(JNIEnv *e, jobject,
                                                         jbyteArray nfceeId,
                                                         jbyteArray conInfo) {
  uint8_t nfceeIdArray[3];
  uint8_t conInfoArray[3];
  int num = 0;

  num = NfcStExtensions::getInstance().getAvailableHciHostList(nfceeIdArray,
                                                               conInfoArray);

  for (int i = 0; i < num; i++) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: nfceeIdArray[%d] - 0x%02x", __FUNCTION__, i, nfceeIdArray[i]);
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: conInfoArray[%d] - 0x%02x", __FUNCTION__, i, conInfoArray[i]);
  }

  e->SetByteArrayRegion(nfceeId, 0, num, (jbyte *)nfceeIdArray);
  e->SetByteArrayRegion(conInfo, 0, num, (jbyte *)conInfoArray);
  return num;
}

static void nativeNfcStExtensions_sendPropSetConfig(JNIEnv *e, jobject,
                                                    jint subSetId,
                                                    jint configId,
                                                    jbyteArray param,
                                                    jint length) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  ScopedByteArrayRW bytes(e, param);
  NfcStExtensions::getInstance().sendPropSetConfig(
      subSetId, configId, reinterpret_cast<uint8_t *>(&bytes[0]), length);
}

static jbyteArray nativeNfcStExtensions_sendPropGetConfig(JNIEnv *e, jobject,
                                                          jint subSetId,
                                                          jint configId) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);

  uint16_t recvBufferActualSize = 0;
  uint8_t recvBuffer[256];

  NfcStExtensions::getInstance().sendPropGetConfig(
      subSetId, configId, recvBuffer, recvBufferActualSize);

  jbyteArray result = e->NewByteArray(recvBufferActualSize);
  if (result != NULL) {
    e->SetByteArrayRegion(result, 0, recvBufferActualSize, (jbyte *)recvBuffer);
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: exit: received data length =%hu", __func__, recvBufferActualSize);
  return result;
}

static jbyteArray nativeNfcStExtensions_sendPropTestCmd(JNIEnv *e, jobject,
                                                        jint subCode,
                                                        jbyteArray paramTx,
                                                        jint lengthTx) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter;", __func__);
  uint16_t recvBufferActualSize = 0;
  uint8_t recvBuffer[256];

  ScopedByteArrayRW bytes(e, paramTx);
  NfcStExtensions::getInstance().sendPropTestCmd(
      subCode, reinterpret_cast<uint8_t *>(&bytes[0]), lengthTx, recvBuffer,
      recvBufferActualSize);

  jbyteArray result = e->NewByteArray(recvBufferActualSize);
  if (result != NULL) {
    e->SetByteArrayRegion(result, 0, recvBufferActualSize, (jbyte *)recvBuffer);
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: exit: received data length =%d", __func__, recvBufferActualSize);
  return result;
}
/* NCI 2.0 - End */

/*****************************************************************************
**
** Description:     JNI functions
**
*****************************************************************************/
static JNINativeMethod gMethods[] = {
    {"getFirmwareVersion", "()[B",
     (void *)nativeNfcStExtensions_getFirmwareVersion},
    {"getCRCConfiguration", "()[B",
     (void *)nativeNfcStExtensions_getCRCConfiguration},
    {"getHWVersion", "()[B", (void *)nativeNfcStExtensions_getHWVersion},
    {"getSWVersion", "()[B", (void *)nativeNfcStExtensions_getSWVersion},
    {"isUiccConnected", "()Z", (void *)nativeNfcStExtensions_isUiccConnected},
    /* NCI 2.0 - Begin */
    {"iseSEConnected", "()Z", (void *)nativeNfcStExtensions_iseSEConnected},
    {"isSEConnected", "(I)Z", (void *)nativeNfcStExtensions_isSEConnected},
    /* NCI 2.0 - End */
    {"loopback", "()I", (void *)nativeNfcStExtensions_loopback},
    {"getHceUserProp", "()I", (void *)nativeNfcStExtensions_getHceUserProp},
    {"setRfConfiguration", "(I[B)V",
     (void *)nativeNfcStExtensions_setRfConfiguration},
    {"getRfConfiguration", "([B)I",
     (void *)nativeNfcStExtensions_getRfConfiguration},
    {"setRfBitmap", "(I)V", (void *)nativeNfcStExtensions_setRfBitmap},
    {"updatePipesInfo", "()I", (void *)nativeNfcStExtensions_updatePipesInfo},
    {"setDefaultOffHostRoute", "(I)V",
     (void *)nativeNfcStExtensions_setDefaultOffHostRoute},
    {"getDefaultOffHostRoute", "()I",
     (void *)nativeNfcStExtensions_getDefaultOffHostRoute},
    /* NCI 2.0 - Begin */
    {"getProprietaryConfigSettings", "(III)Z",
     (void *)nativeNfcStExtensions_getProprietaryConfigSettings},
    {"setProprietaryConfigSettings", "(IIIZ)V",
     (void *)nativeNfcStExtensions_setProprietaryConfigSettings},
    /* NCI 2.0 - End */
    {"getPipesList", "(I[B)I", (void *)nativeNfcStExtensions_getPipesList},
    {"getPipeInfo", "(II[B)V", (void *)nativeNfcStExtensions_getPipeInfo},
    {"getATR", "()[B", (void *)nativeNfcStExtensions_getAtr},
    {"EnableSE", "(IZ)Z", (void *)nativeNfcStExtensions_EnableSE},
    {"connectEE", "(I)Z", (void *)nativeNfcStExtensions_connectEE},
    {"transceiveEE", "(I[B)[B", (void *)nativeNfcStExtensions_transceiveEE},
    {"disconnectEE", "(I)Z", (void *)nativeNfcStExtensions_disconnectEE},
    {"connectGate", "(II)I", (void *)nativeNfcStExtensions_connectGate},
    {"transceive", "(II[B)[B", (void *)nativeNfcStExtensions_transceive},
    {"disconnectGate", "(I)V", (void *)nativeNfcStExtensions_disconnectGate},
    /* NCI 2.0 - Begin */
    {"setNciConfig", "(I[BI)V", (void *)nativeNfcStExtensions_setNciConfig},
    {"getNciConfig", "(I)[B", (void *)nativeNfcStExtensions_getNciConfig},
    {"getAvailableHciHostList", "([B[B)I",
     (void *)nativeNfcStExtensions_getAvailableHciHostList},
    {"sendPropSetConfig", "(II[BI)V",
     (void *)nativeNfcStExtensions_sendPropSetConfig},
    {"sendPropGetConfig", "(II)[B",
     (void *)nativeNfcStExtensions_sendPropGetConfig},
    {"sendPropTestCmd", "(I[BI)[B",
     (void *)nativeNfcStExtensions_sendPropTestCmd},
    {"getCustomerData", "()[B", (void *)nativeNfcStExtensions_getCustomerData},
    /* NCI 2.0 - End */
};

/*******************************************************************************
**
** Function:        register_com_android_nfc_NativeNfcStExtensions
**
** Description:     Register JNI functions with Java Virtual Machine.
**                  e: Environment of JVM.
**
** Returns:         Status of registration.
**
*******************************************************************************/
int register_com_android_nfc_NativeNfcStExtensions(JNIEnv *e) {
  return jniRegisterNativeMethods(e, gNativeNfcStExtensionsClassName, gMethods,
                                  NELEM(gMethods));
}

}  // namespace android
