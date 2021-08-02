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
#include <errno.h>
#include <malloc.h>
#include <nativehelper/ScopedLocalRef.h>
#include <nativehelper/ScopedPrimitiveArray.h>
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <string>
#include "IntervalTimer.h"
#include "JavaClassConstants.h"
#include "Mutex.h"
#include "StNfcJni.h"
#include "StNfcTag.h"

#include "ndef_utils.h"
#include "nfa_api.h"
#include "nfa_rw_api.h"
#include "nfc_brcm_defs.h"
#include "rw_api.h"

using android::base::StringPrintf;

namespace android {
extern nfc_jni_native_data* getNative(JNIEnv* e, jobject o);
extern bool nfcManager_isNfcActive();
}  // namespace android

extern bool gActivated;
extern SyncEvent gDeactivatedEvent;
extern bool nfc_debug_enabled;

/*****************************************************************************
**
** public variables and functions
**
*****************************************************************************/
namespace android {
bool gIsTagDeactivating = false;  // flag for nfa callback indicating we are
                                  // deactivating for RF interface switch
bool gIsSelectingRfInterface = false;  // flag for nfa callback indicating we
                                       // are selecting for RF interface switch
}  // namespace android

/*****************************************************************************
**
** private variables and functions
**
*****************************************************************************/
namespace android {

// Pre-defined tag type values. These must match the values in
// framework Ndef.java for Google public NFC API.
#define NDEF_UNKNOWN_TYPE (-1)
#define NDEF_TYPE1_TAG 1
#define NDEF_TYPE2_TAG 2
#define NDEF_TYPE3_TAG 3
#define NDEF_TYPE4_TAG 4
#define NDEF_MIFARE_CLASSIC_TAG 101

#define STATUS_CODE_TARGET_LOST 146  // this error code comes from the service

static uint32_t sCheckNdefCurrentSize = 0;
static tNFA_STATUS sCheckNdefStatus =
    0;  // whether tag already contains a NDEF message
static bool sCheckNdefCapable = false;  // whether tag has NDEF capability
static tNFA_HANDLE sNdefTypeHandlerHandle = NFA_HANDLE_INVALID;
static tNFA_INTF_TYPE sCurrentRfInterface = NFA_INTERFACE_ISO_DEP;
static std::basic_string<uint8_t> sRxDataBuffer;
static tNFA_STATUS sRxDataStatus = NFA_STATUS_OK;
static bool sWaitingForTransceive = false;
static bool sTransceiveRfTimeout = false;
static Mutex sRfInterfaceMutex;
static uint32_t sReadDataLen = 0;
static uint8_t* sReadData = NULL;
static bool sIsReadingNdefMessage = false;
static SyncEvent sReadEvent;
static sem_t sWriteSem;
static sem_t sFormatSem;
static SyncEvent sTransceiveEvent;
static SyncEvent sReconnectEvent;
static sem_t sCheckNdefSem;
static SyncEvent sPresenceCheckEvent;
static sem_t sMakeReadonlySem;
static IntervalTimer sSwitchBackTimer;  // timer used to tell us to switch back
                                        // to ISO_DEP frame interface
static jboolean sWriteOk = JNI_FALSE;
static jboolean sWriteWaitingForComplete = JNI_FALSE;
static bool sFormatOk = false;
static bool sNeedToSwitchRf = false;
static jboolean sConnectOk = JNI_FALSE;
static jboolean sConnectWaitingForComplete = JNI_FALSE;
static bool sGotDeactivate = false;
static uint32_t sCheckNdefMaxSize = 0;
static bool sCheckNdefCardReadOnly = false;
static jboolean sCheckNdefWaitingForComplete = JNI_FALSE;
static bool sIsTagPresent = true;
static tNFA_STATUS sMakeReadonlyStatus = NFA_STATUS_FAILED;
static jboolean sMakeReadonlyWaitingForComplete = JNI_FALSE;
static int sCurrentConnectedTargetType = TARGET_TYPE_UNKNOWN;
static int sCurrentConnectedTargetProtocol = NFC_PROTOCOL_UNKNOWN;
static int sIsoDepPresCheckCnt = 0;
static bool sIsoDepPresCheckAlternate = false;

static int reSelect(tNFA_INTF_TYPE rfInterface, bool fSwitchIfNeeded);
static bool switchRfInterface(tNFA_INTF_TYPE rfInterface);

// Chinese transit card support.
uint8_t checkTagNtf = 0;
uint8_t checkCmdSent = 0;
bool scoreGenericNtf = false;
bool cardWithoutSleep = false;
static uint16_t NON_NCI_CARD_TIMER_OFFSET = 700;
static IntervalTimer sNonNciCardDetectionTimer;
static IntervalTimer sNonNciMultiCardDetectionTimer;
struct sNonNciCard {
  bool chinaTransp_Card;
  bool Changan_Card;
  uint8_t sProtocolType;
  uint8_t srfInterfaceType;
  uint32_t uidlen;
  uint8_t uid[12];
} sNonNciCard_t;
static int sCurrentConnectedHandle;
static bool sReconnectFlag = false;

void nativeNfcTag_cacheNonNciCardDetection();
void nativeNfcTag_handleNonNciCardDetection(tNFA_CONN_EVT_DATA* eventData);
void nativeNfcTag_handleNonNciMultiCardDetection(uint8_t connEvent,
                                                 tNFA_CONN_EVT_DATA* eventData);
static void nonNciCardTimerProc(union sigval);
static bool sIsCheckingNDef = false;

/*******************************************************************************
**
** Function:        nativeNfcTag_abortWaits
**
** Description:     Unblock all thread synchronization objects.
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_abortWaits() {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  {
    SyncEventGuard g(sReadEvent);
    sReadEvent.notifyOne();
  }
  sem_post(&sWriteSem);
  sem_post(&sFormatSem);
  {
    SyncEventGuard g(sTransceiveEvent);
    sTransceiveEvent.notifyOne();
  }
  {
    SyncEventGuard g(sReconnectEvent);
    sReconnectEvent.notifyOne();
  }

  sCheckNdefStatus = NFA_STATUS_FAILED;

  sem_post(&sCheckNdefSem);
  {
    SyncEventGuard guard(sPresenceCheckEvent);
    sPresenceCheckEvent.notifyOne();
  }
  sem_post(&sMakeReadonlySem);
  sCurrentRfInterface = NFA_INTERFACE_ISO_DEP;
  sCurrentConnectedTargetType = TARGET_TYPE_UNKNOWN;
  sCurrentConnectedTargetProtocol = NFC_PROTOCOL_UNKNOWN;

  sIsoDepPresCheckCnt = 0;
  sIsoDepPresCheckAlternate = false;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doReadCompleted
**
** Description:     Receive the completion status of read operation.  Called by
**                  NFA_READ_CPLT_EVT.
**                  status: Status of operation.
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_doReadCompleted(tNFA_STATUS status) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: status=0x%X; is reading=%u", __func__, status,
                      sIsReadingNdefMessage);

  if (sIsReadingNdefMessage == false)
    return;  // not reading NDEF message right now, so just return

  if (status != NFA_STATUS_OK) {
    sReadDataLen = 0;
    if (sReadData) free(sReadData);
    sReadData = NULL;
  }
  SyncEventGuard g(sReadEvent);
  sReadEvent.notifyOne();
}

/*******************************************************************************
**
** Function:        nativeNfcTag_setRfInterface
**
** Description:     Set rf interface.
**
** Returns:         void
**
*******************************************************************************/
void nativeNfcTag_setRfInterface(tNFA_INTF_TYPE rfInterface) {
  sCurrentRfInterface = rfInterface;
}

/*******************************************************************************
**
** Function:        ndefHandlerCallback
**
** Description:     Receive NDEF-message related events from stack.
**                  event: Event code.
**                  p_data: Event data.
**
** Returns:         None
**
*******************************************************************************/
static void ndefHandlerCallback(tNFA_NDEF_EVT event,
                                tNFA_NDEF_EVT_DATA* eventData) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: event=%u, eventData=%p", __func__, event, eventData);

  switch (event) {
    case NFA_NDEF_REGISTER_EVT: {
      tNFA_NDEF_REGISTER& ndef_reg = eventData->ndef_reg;
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_NDEF_REGISTER_EVT; status=0x%X; h=0x%X",
                          __func__, ndef_reg.status, ndef_reg.ndef_type_handle);
      sNdefTypeHandlerHandle = ndef_reg.ndef_type_handle;
    } break;

    case NFA_NDEF_DATA_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_NDEF_DATA_EVT; data_len = %u", __func__,
                          eventData->ndef_data.len);
      sReadDataLen = eventData->ndef_data.len;
      sReadData = (uint8_t*)malloc(sReadDataLen);
      memcpy(sReadData, eventData->ndef_data.p_data, eventData->ndef_data.len);
    } break;

    default:
      LOG(ERROR) << StringPrintf("%s: Unknown event %u ????", __func__, event);
      break;
  }
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doRead
**
** Description:     Read the NDEF message on the tag.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         NDEF message.
**
*******************************************************************************/
static jbyteArray nativeNfcTag_doRead(JNIEnv* e, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  tNFA_STATUS status = NFA_STATUS_FAILED;
  jbyteArray buf = NULL;

  sReadDataLen = 0;
  if (sReadData != NULL) {
    free(sReadData);
    sReadData = NULL;
  }

  if (sCheckNdefCurrentSize > 0) {
    {
      SyncEventGuard g(sReadEvent);
      sIsReadingNdefMessage = true;
      status = NFA_RwReadNDef();
      sReadEvent.wait();  // wait for NFA_READ_CPLT_EVT
    }
    sIsReadingNdefMessage = false;

    if (sReadDataLen > 0)  // if stack actually read data from the tag
    {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: read %u bytes", __func__, sReadDataLen);
      buf = e->NewByteArray(sReadDataLen);
      e->SetByteArrayRegion(buf, 0, sReadDataLen, (jbyte*)sReadData);
    }
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: create empty buffer", __func__);
    sReadDataLen = 0;
    sReadData = (uint8_t*)malloc(1);
    buf = e->NewByteArray(sReadDataLen);
    e->SetByteArrayRegion(buf, 0, sReadDataLen, (jbyte*)sReadData);
  }

  if (sReadData) {
    free(sReadData);
    sReadData = NULL;
  }
  sReadDataLen = 0;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return buf;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doWriteStatus
**
** Description:     Receive the completion status of write operation.  Called
**                  by NFA_WRITE_CPLT_EVT.
**                  isWriteOk: Status of operation.
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_doWriteStatus(jboolean isWriteOk) {
  if (sWriteWaitingForComplete != JNI_FALSE) {
    sWriteWaitingForComplete = JNI_FALSE;
    sWriteOk = isWriteOk;
    sem_post(&sWriteSem);
  }
}

/*******************************************************************************
**
** Function:        nonNciCardTimerProc
**
** Description:     CallBack timer for Non nci card detection.
**
**
**
** Returns:         None
**
*******************************************************************************/
void nonNciCardTimerProc(union sigval) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter ", __func__);
  (void)memset(&sNonNciCard_t, 0, sizeof(sNonNciCard));
  scoreGenericNtf = false;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_cacheChinaBeijingCardDetection
**
** Description:     Store the  China Beijing Card detection parameters
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_cacheNonNciCardDetection() {
  NfcTag& natTag = NfcTag::getInstance();
  static uint32_t cardDetectTimeout = 0;
  static uint8_t* uid;
  scoreGenericNtf = true;
  NfcTag::getInstance().getTypeATagUID(&uid, &sNonNciCard_t.uidlen);
  (void)memcpy(sNonNciCard_t.uid, uid, sNonNciCard_t.uidlen);
  sNonNciCard_t.sProtocolType =
      natTag.mTechLibNfcTypes[sCurrentConnectedHandle];
  sNonNciCard_t.srfInterfaceType = sCurrentRfInterface;
  // cardDetectTimeout =  NON_NCI_CARD_TIMER_OFFSET +
  // android::getrfDiscoveryDuration();
  cardDetectTimeout = NON_NCI_CARD_TIMER_OFFSET + 500;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: cardDetectTimeout = %d",
                                                   __func__, cardDetectTimeout);
  sNonNciCardDetectionTimer.set(cardDetectTimeout, nonNciCardTimerProc);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: sNonNciCard_t.sProtocolType=0x%x sNonNciCard_t.srfInterfaceType "
      "=0x%x ",
      __func__, sNonNciCard_t.sProtocolType, sNonNciCard_t.srfInterfaceType);
}

/*******************************************************************************
**
** Function:        nativeNfcTag_handleChinaBeijingCardDetection
**
** Description:     China Beijing Card activation
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_handleNonNciCardDetection(tNFA_CONN_EVT_DATA* eventData) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter ", __func__);
  sNonNciCardDetectionTimer.kill();
  static uint32_t tempUidLen = 0x00;
  static uint8_t* tempUid;
  NfcTag::getInstance().getTypeATagUID(&tempUid, &tempUidLen);
  if ((eventData->activated.activate_ntf.intf_param.type ==
       sNonNciCard_t.srfInterfaceType) &&
      (eventData->activated.activate_ntf.protocol ==
       sNonNciCard_t.sProtocolType)) {
    if ((tempUidLen == sNonNciCard_t.uidlen) &&
        (memcmp(tempUid, sNonNciCard_t.uid, tempUidLen) == 0x00)) {
      sNonNciCard_t.chinaTransp_Card = true;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s:  sNonNciCard_t.chinaTransp_Card = true", __func__);
    }
  } else if ((sNonNciCard_t.srfInterfaceType == NFC_INTERFACE_FRAME) &&
             (eventData->activated.activate_ntf.protocol ==
              sNonNciCard_t.sProtocolType)) {
    if ((tempUidLen == sNonNciCard_t.uidlen) &&
        (memcmp(tempUid, sNonNciCard_t.uid, tempUidLen) == 0x00)) {
      sNonNciCard_t.Changan_Card = true;
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s:   sNonNciCard_t.Changan_Card = true", __func__);
    }
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: eventData->activated.activate_ntf.protocol =0x%x "
      "eventData->activated.activate_ntf.intf_param.type =0x%x",
      __func__, eventData->activated.activate_ntf.protocol,
      eventData->activated.activate_ntf.intf_param.type);
}

/*******************************************************************************
**
** Function:        nativeNfcTag_handleChinaMultiCardDetection
**
** Description:     Multiprotocol Card activation
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_handleNonNciMultiCardDetection(
    uint8_t connEvent, tNFA_CONN_EVT_DATA* eventData) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter ", __func__);
  if (NfcTag::getInstance().mNumDiscNtf) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: check_tag_ntf = %d, check_cmd_sent = %d", __func__,
                        checkTagNtf, checkCmdSent);
    if (checkTagNtf == 0) {
      NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
      if (NfcTag::getInstance().checkNextValidProtocol() != -1) {
        NFA_Deactivate(true);
        checkCmdSent = 1;
        sNonNciMultiCardDetectionTimer.set(NON_NCI_CARD_TIMER_OFFSET,
                                           nonNciCardTimerProc);
      }
    } else if (checkTagNtf == 1) {
      NfcTag::getInstance().mNumDiscNtf = 0;
      checkTagNtf = 0;
      checkCmdSent = 0;
      NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
    }
  } else {
    NfcTag::getInstance().connectionEventHandler(connEvent, eventData);
  }
}

/*******************************************************************************
**
** Function:        switchBackTimerProc
**
** Description:     Callback function for interval timer.
**
** Returns:         None
**
*******************************************************************************/
static void switchBackTimerProc(union sigval) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  switchRfInterface(NFA_INTERFACE_ISO_DEP);
}

/*******************************************************************************
**
** Function:        nativeNfcTag_formatStatus
**
** Description:     Receive the completion status of format operation.  Called
**                  by NFA_FORMAT_CPLT_EVT.
**                  isOk: Status of operation.
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_formatStatus(bool isOk) {
  sFormatOk = isOk;
  sem_post(&sFormatSem);
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doWrite
**
** Description:     Write a NDEF message to the tag.
**                  e: JVM environment.
**                  o: Java object.
**                  buf: Contains a NDEF message.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nativeNfcTag_doWrite(JNIEnv* e, jobject, jbyteArray buf) {
  jboolean result = JNI_FALSE;
  tNFA_STATUS status = 0;
  const int maxBufferSize = 1024;
  uint8_t buffer[maxBufferSize] = {0};
  uint32_t curDataSize = 0;

  ScopedByteArrayRO bytes(e, buf);
  uint8_t* p_data = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(
      &bytes[0]));  // TODO: const-ness API bug in NFA_RwWriteNDef!

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; len = %zu", __func__, bytes.size());

  /* Create the write semaphore */
  if (sem_init(&sWriteSem, 0, 0) == -1) {
    LOG(ERROR) << StringPrintf("%s: semaphore creation failed (errno=0x%08x)",
                               __func__, errno);
    return JNI_FALSE;
  }

  sWriteWaitingForComplete = JNI_TRUE;
  if (sCheckNdefStatus == NFA_STATUS_FAILED) {
    // if tag does not contain a NDEF message
    // and tag is capable of storing NDEF message
    if (sCheckNdefCapable) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: try format", __func__);
      sem_init(&sFormatSem, 0, 0);
      sFormatOk = false;
      status = NFA_RwFormatTag();
      sem_wait(&sFormatSem);
      sem_destroy(&sFormatSem);
      if (sFormatOk == false)  // if format operation failed
        goto TheEnd;
    }
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: try write", __func__);
    status = NFA_RwWriteNDef(p_data, bytes.size());
  } else if (bytes.size() == 0) {
    // if (NXP TagWriter wants to erase tag) then create and write an empty ndef
    // message
    NDEF_MsgInit(buffer, maxBufferSize, &curDataSize);
    status = NDEF_MsgAddRec(buffer, maxBufferSize, &curDataSize, NDEF_TNF_EMPTY,
                            NULL, 0, NULL, 0, NULL, 0);
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: create empty ndef msg; status=%u; size=%u",
                        __func__, status, curDataSize);
    status = NFA_RwWriteNDef(buffer, curDataSize);
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: NFA_RwWriteNDef", __func__);
    status = NFA_RwWriteNDef(p_data, bytes.size());
  }

  if (status != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: write/format error=%d", __func__, status);
    goto TheEnd;
  }

  /* Wait for write completion status */
  sWriteOk = false;
  if (sem_wait(&sWriteSem)) {
    LOG(ERROR) << StringPrintf("%s: wait semaphore (errno=0x%08x)", __func__,
                               errno);
    goto TheEnd;
  }

  result = sWriteOk;

TheEnd:
  /* Destroy semaphore */
  if (sem_destroy(&sWriteSem)) {
    LOG(ERROR) << StringPrintf("%s: failed destroy semaphore (errno=0x%08x)",
                               __func__, errno);
  }
  sWriteWaitingForComplete = JNI_FALSE;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit; result=%d", __func__, result);
  return result;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doConnectStatus
**
** Description:     Receive the completion status of connect operation.
**                  isConnectOk: Status of the operation.
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_doConnectStatus(jboolean isConnectOk) {
  if (sConnectWaitingForComplete != JNI_FALSE) {
    sConnectWaitingForComplete = JNI_FALSE;
    sConnectOk = isConnectOk;
    SyncEventGuard g(sReconnectEvent);
    sReconnectEvent.notifyOne();
  }
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doDeactivateStatus
**
** Description:     Receive the completion status of deactivate operation.
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_doDeactivateStatus(int status) {
  sGotDeactivate = (status == 0);

  SyncEventGuard g(sReconnectEvent);
  sReconnectEvent.notifyOne();
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doConnect
**
** Description:     Connect to the tag in RF field.
**                  e: JVM environment.
**                  o: Java object.
**                  targetHandle: Handle of the tag.
**
** Returns:         Must return NXP status code, which NFC service expects.
**
*******************************************************************************/
static jint nativeNfcTag_doConnect(JNIEnv*, jobject, jint targetHandle) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: targetHandle = %d", __func__, targetHandle);
  int i = targetHandle;
  NfcTag& natTag = NfcTag::getInstance();
  int retCode = NFCSTATUS_SUCCESS;
  sCurrentConnectedTargetType = natTag.mTechList[i];
  sCurrentConnectedTargetProtocol = natTag.mTechLibNfcTypes[i];

  sIsoDepPresCheckCnt = 0;
  sIsoDepPresCheckAlternate = false;

  if (i >= NfcTag::MAX_NUM_TECHNOLOGY) {
    LOG(ERROR) << StringPrintf("%s: Handle not found", __func__);
    retCode = NFCSTATUS_FAILED;
    goto TheEnd;
  }

  sNeedToSwitchRf = false;
  if (natTag.getActivationState() != NfcTag::Active) {
    LOG(ERROR) << StringPrintf("%s: tag already deactivated", __func__);
    retCode = NFCSTATUS_FAILED;
    goto TheEnd;
  }

  sCurrentConnectedHandle = targetHandle;

  if ((sCurrentConnectedTargetProtocol != NFC_PROTOCOL_ISO_DEP) &&
      (sCurrentConnectedTargetProtocol != NFC_PROTOCOL_MIFARE)) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s() Nfc type = %d, do nothing for non ISO_DEP",
                        __func__, sCurrentConnectedTargetProtocol);
    retCode = NFCSTATUS_SUCCESS;
    goto TheEnd;
  }

  if (sCurrentConnectedTargetType == TARGET_TYPE_ISO14443_3A ||
      sCurrentConnectedTargetType == TARGET_TYPE_ISO14443_3B) {
    if (sNonNciCard_t.Changan_Card == true)
      sNeedToSwitchRf = true;
    else if (sCurrentConnectedTargetProtocol == NFC_PROTOCOL_ISO_DEP) {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: switching to tech: %d need to switch rf intf to frame", __func__,
          sCurrentConnectedTargetType);
      retCode = switchRfInterface(NFA_INTERFACE_FRAME) ? NFA_STATUS_OK
                                                       : NFA_STATUS_FAILED;
    }
  } else {
    if (sCurrentConnectedTargetType == TARGET_TYPE_MIFARE_CLASSIC) {
      retCode = switchRfInterface(NFC_INTERFACE_MIFARE) ? NFA_STATUS_OK
                                                        : NFA_STATUS_FAILED;
    } else {
      retCode = switchRfInterface(NFA_INTERFACE_ISO_DEP) ? NFA_STATUS_OK
                                                         : NFA_STATUS_FAILED;
    }
  }

TheEnd:
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit 0x%X", __func__, retCode);
  return retCode;
}

/*******************************************************************************
**
** Function:        setReconnectState
**
** Description:     Connect to the tag in RF field.
**                  e: JVM environment.
**                  o: Java object.
**                  targetHandle: Handle of the tag.
**
** Returns:         Must return NXP status code, which NFC service expects.
**
*******************************************************************************/
void setReconnectState(bool flag) {
  sReconnectFlag = flag;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("setReconnectState = 0x%x", sReconnectFlag);
}

/*******************************************************************************
**
** Function:        getReconnectState
**
** Description:     Connect to the tag in RF field.
**                  e: JVM environment.
**                  o: Java object.
**                  targetHandle: Handle of the tag.
**
** Returns:         Must return NXP status code, which NFC service expects.
**
*******************************************************************************/
bool getReconnectState(void) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("getReconnectState = 0x%x", sReconnectFlag);
  return sReconnectFlag;
}

/*******************************************************************************
**
** Function:        reSelect
**
** Description:     Deactivates the tag and re-selects it with the specified
**                  rf interface.
**
** Returns:         status code, 0 on success, 1 on failure,
**                  146 (defined in service) on tag lost
**
*******************************************************************************/
static int reSelect(tNFA_INTF_TYPE rfInterface, bool fSwitchIfNeeded) {
  int handle = sCurrentConnectedHandle;
  int rVal = 1;
  tNFA_STATUS status = NFA_STATUS_FAILED;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; rf intf = %d, current intf = %d", __func__,
                      rfInterface, sCurrentRfInterface);

  sRfInterfaceMutex.lock();

  if (fSwitchIfNeeded && (rfInterface == sCurrentRfInterface)) {
    // already in the requested interface
    sRfInterfaceMutex.unlock();
    return 0;  // success
  }

  NfcTag& natTag = NfcTag::getInstance();
  natTag.setReselect(TRUE);

  uint8_t retry_cnt = 1;

  do {
    // if tag has shutdown, abort this method
    if (natTag.isNdefDetectionTimedOut()) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: ndef detection timeout; break", __func__);
      rVal = STATUS_CODE_TARGET_LOST;
      break;
    }

    if (!retry_cnt &&
        (natTag.mTechLibNfcTypes[handle] != NFA_PROTOCOL_MIFARE)) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Cashbee detected", __func__);
      natTag.mCashbeeDetected = true;
    }

    {
      SyncEventGuard g(sReconnectEvent);
      gIsTagDeactivating = true;
      sGotDeactivate = false;
      setReconnectState(false);
      NFA_SetReconnectState(true);

      if (natTag.isCashBeeActivated() == true ||
          natTag.isEzLinkTagActivated() == true ||
          sNonNciCard_t.chinaTransp_Card == true) {
        setReconnectState(true);
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: deactivate to Idle", __func__);
        if (NFA_STATUS_OK != (status = NFA_StopRfDiscovery())) {
          LOG(ERROR) << StringPrintf("%s: deactivate failed, status = %d",
                                     __func__, status);
          break;
        }
      } else {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: deactivate to sleep", __func__);
        if (NFA_STATUS_OK !=
            (status = NFA_Deactivate(TRUE)))  // deactivate to sleep state
        {
          LOG(ERROR) << StringPrintf("%s: deactivate failed, status = %d",
                                     __func__, status);
          break;
        }
      }
      if (sReconnectEvent.wait(1000) == false)  // if timeout occurred
      {
        LOG(ERROR) << StringPrintf("%s: timeout waiting for deactivate",
                                   __func__);

        break;
      }
    }

    if (natTag.getActivationState() == NfcTag::Idle) {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s:tag in idle state", __func__);
      if (natTag.mActivationParams_t.mTechLibNfcTypes == NFC_PROTOCOL_ISO_DEP) {
        if (natTag.mActivationParams_t.mTechParams ==
            NFC_DISCOVERY_TYPE_POLL_A) {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: Cashbee detected", __func__);
          natTag.mCashbeeDetected = true;
        } else if (natTag.mActivationParams_t.mTechParams ==
                   NFC_DISCOVERY_TYPE_POLL_B) {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: mEzLinkTypeTag detected", __func__);

          natTag.mEzLinkTypeTag = true;
        }
      }
    }

    if (!(natTag.isCashBeeActivated() == true ||
          natTag.isEzLinkTagActivated() == true ||
          sNonNciCard_t.chinaTransp_Card == true)) {
      if (natTag.getActivationState() != NfcTag::Sleep) {
        LOG(ERROR) << StringPrintf("%s: tag is not in sleep", __func__);
        rVal = STATUS_CODE_TARGET_LOST;
        if (!retry_cnt)
          break;
        else
          continue;
      }
    } else {
      setReconnectState(false);
    }

    gIsTagDeactivating = false;

    {
      SyncEventGuard g2(sReconnectEvent);
      gIsSelectingRfInterface = true;
      sConnectWaitingForComplete = JNI_TRUE;
      if (natTag.isCashBeeActivated() == true ||
          natTag.isEzLinkTagActivated() == true ||
          sNonNciCard_t.chinaTransp_Card == true) {
        setReconnectState(true);
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: Start discovery", __func__);
        if (NFA_STATUS_OK != (status = NFA_StartRfDiscovery())) {
          LOG(ERROR) << StringPrintf("%s: deactivate failed, status = %d",
                                     __func__, status);
          break;
        }
      } else {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: select interface %u", __func__, rfInterface);
        if (NFA_STATUS_OK !=
            (status =
                 NFA_Select(natTag.mTechHandles[handle],
                            natTag.mTechLibNfcTypes[handle], rfInterface))) {
          LOG(ERROR) << StringPrintf(
              "%s: NFA_Select failed, status = %d -- fallback deactivate to "
              "idle",
              __func__, status);
          status = NFA_Deactivate(false);
          if (status != NFA_STATUS_OK) {
            LOG(ERROR) << StringPrintf("%s: deactivate failed, status = %d",
                                       __func__, status);
          }
          break;
        }
      }
      sConnectOk = false;
      if (sReconnectEvent.wait(1000) == false)  // if timeout occured
      {
        LOG(ERROR) << StringPrintf("%s: timeout waiting for select", __func__);
        if (!(natTag.isCashBeeActivated() == true ||
              natTag.isEzLinkTagActivated() == true ||
              sNonNciCard_t.chinaTransp_Card == true)) {
          status = NFA_Deactivate(FALSE);
          if (status != NFA_STATUS_OK)
            LOG(ERROR) << StringPrintf("%s: deactivate failed; error=0x%X",
                                       __func__, status);
        }
        break;
      }
    }

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: select completed; sConnectOk=%d", __func__, sConnectOk);
    if (NfcTag::getInstance().getActivationState() != NfcTag::Active) {
      LOG(ERROR) << StringPrintf("%s: tag is not active", __func__);
      rVal = STATUS_CODE_TARGET_LOST;
      if ((!retry_cnt) ||
          (natTag.mTechLibNfcTypes[handle] == NFA_PROTOCOL_MIFARE))
        break;
    }
    if (natTag.isEzLinkTagActivated() == true) {
      natTag.mEzLinkTypeTag = false;
    }
    if (natTag.isCashBeeActivated() == true) {
      natTag.mCashbeeDetected = false;
    }
    if (sConnectOk) {
      rVal = 0;  // success
      sCurrentRfInterface = rfInterface;
      break;
    } else {
      rVal = 1;
    }
  } while (retry_cnt--);

  setReconnectState(false);
  NFA_SetReconnectState(false);
  sConnectWaitingForComplete = JNI_FALSE;
  gIsTagDeactivating = false;
  gIsSelectingRfInterface = false;
  sRfInterfaceMutex.unlock();
  natTag.setReselect(FALSE);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit; status=%d", __func__, rVal);
  return rVal;
}

/*******************************************************************************
**
** Function:        switchRfInterface
**
** Description:     Switch controller's RF interface to frame, ISO-DEP, or
*NFC-DEP.
**                  rfInterface: Type of RF interface.
**
** Returns:         True if ok.
**
*******************************************************************************/
static bool switchRfInterface(tNFA_INTF_TYPE rfInterface) {
  NfcTag& natTag = NfcTag::getInstance();

  if ((sCurrentConnectedTargetProtocol != NFC_PROTOCOL_ISO_DEP) &&
      (sCurrentConnectedTargetProtocol != NFC_PROTOCOL_MIFARE)) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: protocol: %d not ISO_DEP nor MFC, do nothing",
                        __func__, natTag.mTechLibNfcTypes[0]);
    return true;
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: new rf intf = %d, cur rf intf = %d", __func__,
                      rfInterface, sCurrentRfInterface);
  bool rVal = true;
  if (rfInterface != sCurrentRfInterface) {
    if (0 == reSelect(rfInterface, true)) {
      sCurrentRfInterface = rfInterface;
      rVal = true;
    } else {
      rVal = false;
    }
  }

  return rVal;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doReconnect
**
** Description:     Re-connect to the tag in RF field.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         Status code.
**
*******************************************************************************/
static jint nativeNfcTag_doReconnect(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter (cur:%d)", __func__, sCurrentConnectedTargetProtocol);
  int retCode = NFCSTATUS_SUCCESS;
  NfcTag& natTag = NfcTag::getInstance();

  if (natTag.mNfcDisableinProgress) {
    LOG(ERROR) << StringPrintf("%s: NFC disabling in progress", __func__);
    retCode = NFCSTATUS_FAILED;
    goto TheEnd;
  }
  if (natTag.getActivationState() != NfcTag::Active) {
    LOG(ERROR) << StringPrintf("%s: tag already deactivated", __func__);
    retCode = NFCSTATUS_FAILED;
    goto TheEnd;
  }

  // special case for Kovio
  if (sCurrentConnectedTargetProtocol == TARGET_TYPE_KOVIO_BARCODE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: fake out reconnect for Kovio", __func__);
    goto TheEnd;
  }

  // this is only supported for type 2 or 4 (ISO_DEP) tags
  if (sCurrentConnectedTargetProtocol == NFA_PROTOCOL_ISO_DEP)
    retCode = reSelect(NFA_INTERFACE_ISO_DEP, false);
  else if (sCurrentConnectedTargetProtocol == NFA_PROTOCOL_T2T)
    retCode = reSelect(NFA_INTERFACE_FRAME, false);
  else if (sCurrentConnectedTargetProtocol == NFC_PROTOCOL_MIFARE)
    retCode = reSelect(NFA_INTERFACE_MIFARE, false);

TheEnd:
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit 0x%X", __func__, retCode);
  return retCode;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doHandleReconnect
**
** Description:     Re-connect to the tag in RF field.
**                  e: JVM environment.
**                  o: Java object.
**                  targetHandle: Handle of the tag.
**
** Returns:         Status code.
**
*******************************************************************************/
static jint nativeNfcTag_doHandleReconnect(JNIEnv* e, jobject o,
                                           jint targetHandle) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: targetHandle = %d", __func__, targetHandle);
  if (NfcTag::getInstance().mNfcDisableinProgress)
    return STATUS_CODE_TARGET_LOST;
  return nativeNfcTag_doConnect(e, o, targetHandle);
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doDisconnect
**
** Description:     Deactivate the RF field.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if ok.
**
*******************************************************************************/
jboolean nativeNfcTag_doDisconnect(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  tNFA_STATUS nfaStat = NFA_STATUS_OK;

  NfcTag::getInstance().resetAllTransceiveTimeouts();

  if (sNonNciCard_t.Changan_Card == true ||
      sNonNciCard_t.chinaTransp_Card == true) {
    (void)memset(&sNonNciCard_t, 0, sizeof(sNonNciCard));
    scoreGenericNtf = false;
  }

  if ((NfcTag::getInstance().getActivationState() != NfcTag::Active) &&
      ((NfcTag::getInstance().getActivationState() != NfcTag::Sleep))) {
    LOG(ERROR) << StringPrintf("%s: tag already deactivated", __func__);
    goto TheEnd;
  }

  nfaStat = NFA_Deactivate(FALSE);
  if (nfaStat != NFA_STATUS_OK)
    LOG(ERROR) << StringPrintf("%s: deactivate failed; error=0x%X", __func__,
                               nfaStat);

TheEnd:
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return (nfaStat == NFA_STATUS_OK) ? JNI_TRUE : JNI_FALSE;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doTransceiveStatus
**
** Description:     Receive the completion status of transceive operation.
**                  status: operation status.
**                  buf: Contains tag's response.
**                  bufLen: Length of buffer.
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_doTransceiveStatus(tNFA_STATUS status, uint8_t* buf,
                                     uint32_t bufLen) {
  SyncEventGuard g(sTransceiveEvent);
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: data len=%d", __func__, bufLen);

  if (!sWaitingForTransceive) {
    LOG(ERROR) << StringPrintf("%s: drop data", __func__);
    return;
  }
  sRxDataStatus = status;
  if (sRxDataStatus == NFA_STATUS_OK || sRxDataStatus == NFC_STATUS_CONTINUE)
    sRxDataBuffer.append(buf, bufLen);

  if (sRxDataStatus == NFA_STATUS_OK) sTransceiveEvent.notifyOne();
}

void nativeNfcTag_notifyRfTimeout() {
  SyncEventGuard g(sTransceiveEvent);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: waiting for transceive: %d", __func__, sWaitingForTransceive);
  if (!sWaitingForTransceive) return;

  sTransceiveRfTimeout = true;

  sTransceiveEvent.notifyOne();
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doTransceive
**
** Description:     Send raw data to the tag; receive tag's response.
**                  e: JVM environment.
**                  o: Java object.
**                  raw: Not used.
**                  statusTargetLost: Whether tag responds or times out.
**
** Returns:         Response from tag.
**
*******************************************************************************/
static jbyteArray nativeNfcTag_doTransceive(JNIEnv* e, jobject o,
                                            jbyteArray data, jboolean raw,
                                            jintArray statusTargetLost) {
  int timeout =
      NfcTag::getInstance().getTransceiveTimeout(sCurrentConnectedTargetType);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter; raw=%u; timeout = %d", __func__, raw, timeout);

  bool waitOk = false;
  bool isNack = false;
  jint* targetLost = NULL;
  tNFA_STATUS status;
  bool fNeedToSwitchBack = false;

  if (NfcTag::getInstance().getActivationState() != NfcTag::Active) {
    if (statusTargetLost) {
      targetLost = e->GetIntArrayElements(statusTargetLost, 0);
      if (targetLost)
        *targetLost = 1;  // causes NFC service to throw TagLostException
      e->ReleaseIntArrayElements(statusTargetLost, targetLost, 0);
    }
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: tag not active", __func__);
    return NULL;
  }

  NfcTag& natTag = NfcTag::getInstance();

  // get input buffer and length from java call
  ScopedByteArrayRO bytes(e, data);
  uint8_t* buf = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(
      &bytes[0]));  // TODO: API bug; NFA_SendRawFrame should take const*!
  size_t bufLen = bytes.size();

  if (statusTargetLost) {
    targetLost = e->GetIntArrayElements(statusTargetLost, 0);
    if (targetLost) *targetLost = 0;  // success, tag is still present
  }

  sSwitchBackTimer.kill();
  ScopedLocalRef<jbyteArray> result(e, NULL);
  do {
    {
      if (sNeedToSwitchRf) {
        if (!switchRfInterface(NFA_INTERFACE_FRAME))  // NFA_INTERFACE_ISO_DEP
        {
          break;
        }
        fNeedToSwitchBack = true;
      }
      SyncEventGuard g(sTransceiveEvent);
      sTransceiveRfTimeout = false;
      sWaitingForTransceive = true;
      sRxDataStatus = NFA_STATUS_OK;
      sRxDataBuffer.clear();
      status = NFA_SendRawFrame(buf, bufLen,
                                NFA_DM_DEFAULT_PRESENCE_CHECK_START_DELAY);

      if (status != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf("%s: fail send; error=%d", __func__, status);
        break;
      }
      waitOk = sTransceiveEvent.wait(timeout);
    }

    if (waitOk == false || sTransceiveRfTimeout)  // if timeout occurred
    {
      LOG(ERROR) << StringPrintf("%s: wait response timeout", __func__);
      if (targetLost)
        *targetLost = 1;  // causes NFC service to throw TagLostException
      break;
    }

    if (NfcTag::getInstance().getActivationState() != NfcTag::Active) {
      LOG(ERROR) << StringPrintf("%s: already deactivated", __func__);
      if (targetLost)
        *targetLost = 1;  // causes NFC service to throw TagLostException
      break;
    }

    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: response %zu bytes", __func__, sRxDataBuffer.size());

    if ((natTag.getProtocol() == NFA_PROTOCOL_T2T) &&
        natTag.isT2tNackResponse(sRxDataBuffer.data(), sRxDataBuffer.size())) {
      isNack = true;
    }

    if (sRxDataBuffer.size() > 0) {
      if (isNack) {
        // Some Mifare Ultralight C tags enter the HALT state after it
        // responds with a NACK.  Need to perform a "reconnect" operation
        // to wake it.
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: try reconnect", __func__);
        nativeNfcTag_doReconnect(NULL, NULL);
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: reconnect finish", __func__);
      } else if (sCurrentConnectedTargetProtocol == NFC_PROTOCOL_MIFARE) {
        uint32_t transDataLen = sRxDataBuffer.size();
        uint8_t* transData = (uint8_t*)sRxDataBuffer.data();
        if ((transDataLen == 1) && (transData[0] != 0x00)) {
          /* an error occurred: timeout, unexpected command, etc */
          nativeNfcTag_doReconnect(e, o);
        } else {
          if (transDataLen != 0) {
            result.reset(e->NewByteArray(transDataLen));
          }
          if (result.get() != NULL) {
            e->SetByteArrayRegion(result.get(), 0, transDataLen,
                                  (const jbyte*)transData);
          } else
            LOG(ERROR) << StringPrintf("%s: Failed to allocate java byte array",
                                       __func__);
        }
      } else {
        // marshall data to java for return
        result.reset(e->NewByteArray(sRxDataBuffer.size()));
        if (result.get() != NULL) {
          e->SetByteArrayRegion(result.get(), 0, sRxDataBuffer.size(),
                                (const jbyte*)sRxDataBuffer.data());
        } else
          LOG(ERROR) << StringPrintf("%s: Failed to allocate java byte array",
                                     __func__);
      }  // else a nack is treated as a transceive failure to the upper layers

      sRxDataBuffer.clear();
    }
  } while (0);

  sWaitingForTransceive = false;
  if (targetLost) e->ReleaseIntArrayElements(statusTargetLost, targetLost, 0);

  if (fNeedToSwitchBack) {
    sSwitchBackTimer.set(1500, switchBackTimerProc);
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return result.release();
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doGetNdefType
**
** Description:     Retrieve the type of tag.
**                  e: JVM environment.
**                  o: Java object.
**                  libnfcType: Type of tag represented by JNI.
**                  javaType: Not used.
**
** Returns:         Type of tag represented by NFC Service.
**
*******************************************************************************/
static jint nativeNfcTag_doGetNdefType(JNIEnv*, jobject, jint libnfcType,
                                       jint javaType) {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; libnfc type=%d; java type=%d", __func__,
                      libnfcType, javaType);
  jint ndefType = NDEF_UNKNOWN_TYPE;

  // For NFA, libnfcType is mapped to the protocol value received
  // in the NFA_ACTIVATED_EVT and NFA_DISC_RESULT_EVT event.
  if (NFA_PROTOCOL_T1T == libnfcType) {
    ndefType = NDEF_TYPE1_TAG;
  } else if (NFA_PROTOCOL_T2T == libnfcType) {
    ndefType = NDEF_TYPE2_TAG;
  } else if (NFA_PROTOCOL_T3T == libnfcType) {
    ndefType = NDEF_TYPE3_TAG;
  } else if (NFA_PROTOCOL_ISO_DEP == libnfcType) {
    ndefType = NDEF_TYPE4_TAG;
  } else if (NFC_PROTOCOL_MIFARE == libnfcType) {
    ndefType = NDEF_MIFARE_CLASSIC_TAG;
  } else {
    /* NFA_PROTOCOL_T5T, NFA_PROTOCOL_INVALID and others */
    ndefType = NDEF_UNKNOWN_TYPE;
  }
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit; ndef type=%d", __func__, ndefType);
  return ndefType;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doCheckNdefResult
**
** Description:     Receive the result of checking whether the tag contains a
*NDEF
**                  message.  Called by the NFA_NDEF_DETECT_EVT.
**                  status: Status of the operation.
**                  maxSize: Maximum size of NDEF message.
**                  currentSize: Current size of NDEF message.
**                  flags: Indicate various states.
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_doCheckNdefResult(tNFA_STATUS status, uint32_t maxSize,
                                    uint32_t currentSize, uint8_t flags) {
  // this function's flags parameter is defined using the following macros
  // in nfc/include/rw_api.h;
  //#define RW_NDEF_FL_READ_ONLY  0x01    /* Tag is read only              */
  //#define RW_NDEF_FL_FORMATED   0x02    /* Tag formated for NDEF         */
  //#define RW_NDEF_FL_SUPPORTED  0x04    /* NDEF supported by the tag     */
  //#define RW_NDEF_FL_UNKNOWN    0x08    /* Unable to find if tag is ndef
  // capable/formated/read only */ #define RW_NDEF_FL_FORMATABLE 0x10    /* Tag
  // supports format operation */

  if (!sCheckNdefWaitingForComplete) {
    LOG(ERROR) << StringPrintf("%s: not waiting", __func__);
    return;
  }

  if (flags & RW_NDEF_FL_READ_ONLY)
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: flag read-only", __func__);
  if (flags & RW_NDEF_FL_FORMATED)
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: flag formatted for ndef", __func__);
  if (flags & RW_NDEF_FL_SUPPORTED)
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: flag ndef supported", __func__);
  if (flags & RW_NDEF_FL_UNKNOWN)
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: flag all unknown", __func__);
  if (flags & RW_NDEF_FL_FORMATABLE)
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: flag formattable", __func__);

  sCheckNdefWaitingForComplete = JNI_FALSE;
  sCheckNdefStatus = status;
  if (sCheckNdefStatus != NFA_STATUS_OK &&
      sCheckNdefStatus != NFA_STATUS_TIMEOUT)
    sCheckNdefStatus = NFA_STATUS_FAILED;
  sCheckNdefCapable = false;  // assume tag is NOT ndef capable
  if (sCheckNdefStatus == NFA_STATUS_OK) {
    // NDEF content is on the tag
    sCheckNdefMaxSize = maxSize;
    sCheckNdefCurrentSize = currentSize;
    sCheckNdefCardReadOnly = flags & RW_NDEF_FL_READ_ONLY;
    sCheckNdefCapable = true;
  } else if (sCheckNdefStatus == NFA_STATUS_FAILED) {
    // no NDEF content on the tag
    sCheckNdefMaxSize = 0;
    sCheckNdefCurrentSize = 0;
    sCheckNdefCardReadOnly = flags & RW_NDEF_FL_READ_ONLY;
    if ((flags & RW_NDEF_FL_UNKNOWN) == 0)  // if stack understands the tag
    {
      if (flags & RW_NDEF_FL_SUPPORTED)  // if tag is ndef capable
        sCheckNdefCapable = true;
    }
  } else if (sCheckNdefStatus == NFA_STATUS_TIMEOUT) {
    LOG(ERROR) << StringPrintf("%s: timeout", __func__);

    sCheckNdefMaxSize = 0;
    sCheckNdefCurrentSize = 0;
    sCheckNdefCardReadOnly = false;
  } else {
    LOG(ERROR) << StringPrintf("%s: unknown status=0x%X", __func__, status);
    sCheckNdefMaxSize = 0;
    sCheckNdefCurrentSize = 0;
    sCheckNdefCardReadOnly = false;
  }
  sem_post(&sCheckNdefSem);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doCheckNdef
**
** Description:     Does the tag contain a NDEF message?
**                  e: JVM environment.
**                  o: Java object.
**                  ndefInfo: NDEF info.
**
** Returns:         Status code; 0 is success.
**
*******************************************************************************/
static jint nativeNfcTag_doCheckNdef(JNIEnv* e, jobject o, jintArray ndefInfo) {
  tNFA_STATUS status = NFA_STATUS_FAILED;
  jint* ndef = NULL;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);

  // special case for Kovio
  sIsCheckingNDef = true;
  if (sCurrentConnectedTargetProtocol == TARGET_TYPE_KOVIO_BARCODE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Kovio tag, no NDEF", __func__);
    ndef = e->GetIntArrayElements(ndefInfo, 0);
    ndef[0] = 0;
    ndef[1] = NDEF_MODE_READ_ONLY;
    e->ReleaseIntArrayElements(ndefInfo, ndef, 0);
    sIsCheckingNDef = false;
    return NFA_STATUS_FAILED;
  } else if (sCurrentConnectedTargetProtocol == NFC_PROTOCOL_MIFARE) {
    if (NFCSTATUS_SUCCESS != nativeNfcTag_doReconnect(e, o)) {
      LOG(ERROR) << StringPrintf("%s: Reconnect failed so return error",
                                 __func__);
      sIsCheckingNDef = false;
      return NFA_STATUS_FAILED;
    }
  }

  /* Create the write semaphore */
  if (sem_init(&sCheckNdefSem, 0, 0) == -1) {
    LOG(ERROR) << StringPrintf(
        "%s: Check NDEF semaphore creation failed (errno=0x%08x)", __func__,
        errno);
    sIsCheckingNDef = false;
    return JNI_FALSE;
  }

  if (NfcTag::getInstance().getActivationState() != NfcTag::Active) {
    LOG(ERROR) << StringPrintf("%s: tag already deactivated", __func__);
    goto TheEnd;
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: try NFA_RwDetectNDef", __func__);
  sCheckNdefWaitingForComplete = JNI_TRUE;
  status = NFA_RwDetectNDef();

  if (status != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: NFA_RwDetectNDef failed, status = 0x%X",
                               __func__, status);
    goto TheEnd;
  }

  struct timespec timeout;
  // Get current time
  clock_gettime(CLOCK_REALTIME, &timeout);
  // wait 3s
  timeout.tv_sec += 3;
  /* Wait for check NDEF completion status */
  if (sem_timedwait(&sCheckNdefSem, &timeout)) {
    LOG(ERROR) << StringPrintf(
        "%s: Failed to wait for check NDEF semaphore (errno=0x%08x)", __func__,
        errno);
    goto TheEnd;
  }

  if (sCheckNdefStatus == NFA_STATUS_OK) {
    // stack found a NDEF message on the tag
    ndef = e->GetIntArrayElements(ndefInfo, 0);
    if (NfcTag::getInstance().getProtocol() == NFA_PROTOCOL_T1T)
      ndef[0] = NfcTag::getInstance().getT1tMaxMessageSize();
    else
      ndef[0] = sCheckNdefMaxSize;
    if (sCheckNdefCardReadOnly)
      ndef[1] = NDEF_MODE_READ_ONLY;
    else
      ndef[1] = NDEF_MODE_READ_WRITE;
    e->ReleaseIntArrayElements(ndefInfo, ndef, 0);
    status = NFA_STATUS_OK;
  } else if (sCheckNdefStatus == NFA_STATUS_FAILED) {
    // stack did not find a NDEF message on the tag;
    ndef = e->GetIntArrayElements(ndefInfo, 0);
    if (NfcTag::getInstance().getProtocol() == NFA_PROTOCOL_T1T)
      ndef[0] = NfcTag::getInstance().getT1tMaxMessageSize();
    else
      ndef[0] = sCheckNdefMaxSize;
    if (sCheckNdefCardReadOnly)
      ndef[1] = NDEF_MODE_READ_ONLY;
    else
      ndef[1] = NDEF_MODE_READ_WRITE;
    e->ReleaseIntArrayElements(ndefInfo, ndef, 0);
    status = NFA_STATUS_FAILED;
  } else {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: unknown status 0x%X", __func__, sCheckNdefStatus);
    status = sCheckNdefStatus;
  }

  /* Reconnect Mifare Classic Tag for furture use */
  if (sCurrentConnectedTargetProtocol == NFC_PROTOCOL_MIFARE) {
    if (NFCSTATUS_SUCCESS != nativeNfcTag_doReconnect(e, o)) {
      LOG(ERROR) << StringPrintf("%s: Reconnect failed so return error",
                                 __func__);
      status = NFA_STATUS_FAILED;
    }
  }

TheEnd:
  /* Destroy semaphore */
  if (sem_destroy(&sCheckNdefSem)) {
    LOG(ERROR) << StringPrintf(
        "%s: Failed to destroy check NDEF semaphore (errno=0x%08x)", __func__,
        errno);
  }
  sCheckNdefWaitingForComplete = JNI_FALSE;
  sIsCheckingNDef = false;
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit; status=0x%X", __func__, status);
  return status;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_resetPresenceCheck
**
** Description:     Reset variables related to presence-check.
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_resetPresenceCheck() {
  sIsTagPresent = true;
  sIsoDepPresCheckCnt = 0;
  sIsoDepPresCheckAlternate = false;
  NfcTag::getInstance().mCashbeeDetected = false;
  NfcTag::getInstance().mEzLinkTypeTag = false;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doPresenceCheckResult
**
** Description:     Receive the result of presence-check.
**                  status: Result of presence-check.
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_doPresenceCheckResult(tNFA_STATUS status) {
  SyncEventGuard guard(sPresenceCheckEvent);
  sIsTagPresent = status == NFA_STATUS_OK;
  sPresenceCheckEvent.notifyOne();
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doPresenceCheck
**
** Description:     Check if the tag is in the RF field.
**                  e: JVM environment.
**                  o: Java object.
**
** Returns:         True if tag is in RF field.
**
*******************************************************************************/
static jboolean nativeNfcTag_doPresenceCheck(JNIEnv*, jobject) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  tNFA_STATUS status = NFA_STATUS_OK;
  jboolean isPresent = JNI_FALSE;

  if (NfcTag::getInstance().mNfcDisableinProgress) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s, Nfc disable in progress", __func__);
    return JNI_FALSE;
  }
  if (sIsCheckingNDef == true) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Ndef is being checked", __func__);
    return JNI_TRUE;
  }

  // Special case for Kovio.  The deactivation would have already occurred
  // but was ignored so that normal tag opertions could complete.  Now we
  // want to process as if the deactivate just happened.
  if (sCurrentConnectedTargetProtocol == TARGET_TYPE_KOVIO_BARCODE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: Kovio, force deactivate handling", __func__);
    tNFA_DEACTIVATED deactivated = {NFA_DEACTIVATE_TYPE_IDLE};
    {
      SyncEventGuard g(gDeactivatedEvent);
      gActivated = false;  // guard this variable from multi-threaded access
      gDeactivatedEvent.notifyOne();
    }

    NfcTag::getInstance().setDeactivationState(deactivated);
    nativeNfcTag_resetPresenceCheck();
    NfcTag::getInstance().connectionEventHandler(NFA_DEACTIVATED_EVT, NULL);
    nativeNfcTag_abortWaits();
    NfcTag::getInstance().abort();

    return JNI_FALSE;
  }

  if (nfcManager_isNfcActive() == false) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: NFC is no longer active.", __func__);
    return JNI_FALSE;
  }

  if (!sRfInterfaceMutex.tryLock()) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: tag is being reSelected assume it is present", __func__);
    return JNI_TRUE;
  }

  sRfInterfaceMutex.unlock();

  if (NfcTag::getInstance().isActivated() == false) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: tag already deactivated", __func__);
    return JNI_FALSE;
  }

  {
    SyncEventGuard guard(sPresenceCheckEvent);
    tNFA_RW_PRES_CHK_OPTION method =
        NfcTag::getInstance().getPresenceCheckAlgorithm();

    if (sCurrentConnectedTargetProtocol == NFC_PROTOCOL_ISO_DEP) {
      if (method == NFA_RW_PRES_CHK_ISO_DEP_NAK) {
        sIsoDepPresCheckCnt++;
      }
      if (sIsoDepPresCheckAlternate == true) {
        method = NFA_RW_PRES_CHK_I_BLOCK;
      }
    }

    status = NFA_RwPresenceCheck(method);
    if (status == NFA_STATUS_OK) {
      isPresent = sPresenceCheckEvent.wait(2000);
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s(%d): isPresent = %d", __FUNCTION__, __LINE__, isPresent);
      if (isPresent && (sIsoDepPresCheckCnt == 1) && (sIsTagPresent == false)) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s(%d): Try alternate method in case tag does not support RNAK",
            __FUNCTION__, __LINE__);
        method = NFA_RW_PRES_CHK_I_BLOCK;
        sIsoDepPresCheckAlternate = true;
        status = NFA_RwPresenceCheck(method);
        if (status == NFA_STATUS_OK) {
          isPresent = sPresenceCheckEvent.wait(2000);
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s(%d): isPresent = %d", __FUNCTION__, __LINE__, isPresent);
        }
      }

      isPresent =
          isPresent ? (sIsTagPresent ? JNI_TRUE : JNI_FALSE) : JNI_FALSE;
    }
  }

  if (isPresent == JNI_FALSE) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: tag absent", __func__);

    nativeNfcTag_resetPresenceCheck();
  }

  return isPresent;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doIsNdefFormatable
**
** Description:     Can tag be formatted to store NDEF message?
**                  e: JVM environment.
**                  o: Java object.
**                  libNfcType: Type of tag.
**                  uidBytes: Tag's unique ID.
**                  pollBytes: Data from activation.
**                  actBytes: Data from activation.
**
** Returns:         True if formattable.
**
*******************************************************************************/
static jboolean nativeNfcTag_doIsNdefFormatable(JNIEnv* e, jobject o,
                                                jint /*libNfcType*/, jbyteArray,
                                                jbyteArray, jbyteArray) {
  jboolean isFormattable = JNI_FALSE;
  tNFC_PROTOCOL protocol = NfcTag::getInstance().getProtocol();
  if (NFA_PROTOCOL_T1T == protocol || NFA_PROTOCOL_T5T == protocol ||
      NFC_PROTOCOL_MIFARE == protocol) {
    isFormattable = JNI_TRUE;
  } else if (NFA_PROTOCOL_T3T == protocol) {
    isFormattable = NfcTag::getInstance().isFelicaLite() ? JNI_TRUE : JNI_FALSE;
  } else if (NFA_PROTOCOL_T2T == protocol) {
    isFormattable = (NfcTag::getInstance().isMifareUltralight() |
                     NfcTag::getInstance().isInfineonMyDMove() |
                     NfcTag::getInstance().isKovioType2Tag())
                        ? JNI_TRUE
                        : JNI_FALSE;
  } else if (NFA_PROTOCOL_ISO_DEP == protocol) {
    /**
     * Determines whether this is a formatable IsoDep tag - currectly only NXP
     * DESFire is supported.
     */
    uint8_t cmd[] = {0x90, 0x60, 0x00, 0x00, 0x00};

    if (NfcTag::getInstance().isMifareDESFire()) {
      /* Identifies as DESfire, use get version cmd to be sure */
      jbyteArray versionCmd = e->NewByteArray(5);
      e->SetByteArrayRegion(versionCmd, 0, 5, (jbyte*)cmd);
      jbyteArray respBytes =
          nativeNfcTag_doTransceive(e, o, versionCmd, JNI_TRUE, NULL);
      if (respBytes != NULL) {
        // Check whether the response matches a typical DESfire
        // response.
        // libNFC even does more advanced checking than we do
        // here, and will only format DESfire's with a certain
        // major/minor sw version and NXP as a manufacturer.
        // We don't want to do such checking here, to avoid
        // having to change code in multiple places.
        // A succesful (wrapped) DESFire getVersion command returns
        // 9 bytes, with byte 7 0x91 and byte 8 having status
        // code 0xAF (these values are fixed and well-known).
        int respLength = e->GetArrayLength(respBytes);
        uint8_t* resp = (uint8_t*)e->GetByteArrayElements(respBytes, NULL);
        if (respLength == 9 && resp[7] == 0x91 && resp[8] == 0xAF) {
          isFormattable = JNI_TRUE;
        }
        e->ReleaseByteArrayElements(respBytes, (jbyte*)resp, JNI_ABORT);
      }
    }
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: is formattable=%u", __func__, isFormattable);
  return isFormattable;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doIsIsoDepNdefFormatable
**
** Description:     Is ISO-DEP tag formattable?
**                  e: JVM environment.
**                  o: Java object.
**                  pollBytes: Data from activation.
**                  actBytes: Data from activation.
**
** Returns:         True if formattable.
**
*******************************************************************************/
static jboolean nativeNfcTag_doIsIsoDepNdefFormatable(JNIEnv* e, jobject o,
                                                      jbyteArray pollBytes,
                                                      jbyteArray actBytes) {
  uint8_t uidFake[] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  jbyteArray uidArray = e->NewByteArray(8);
  e->SetByteArrayRegion(uidArray, 0, 8, (jbyte*)uidFake);
  return nativeNfcTag_doIsNdefFormatable(e, o, 0, uidArray, pollBytes,
                                         actBytes);
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doNdefFormat
**
** Description:     Format a tag so it can store NDEF message.
**                  e: JVM environment.
**                  o: Java object.
**                  key: Not used.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nativeNfcTag_doNdefFormat(JNIEnv* e, jobject o, jbyteArray) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", __func__);
  tNFA_STATUS status = NFA_STATUS_OK;

  // Do not try to format if tag is already deactivated.
  if (NfcTag::getInstance().isActivated() == false) {
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: tag already deactivated(no need to format)", __func__);
    return JNI_FALSE;
  }

  sem_init(&sFormatSem, 0, 0);
  sFormatOk = false;
  status = NFA_RwFormatTag();
  if (status == NFA_STATUS_OK) {
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: wait for completion", __func__);
    sem_wait(&sFormatSem);
    status = sFormatOk ? NFA_STATUS_OK : NFA_STATUS_FAILED;
  } else
    LOG(ERROR) << StringPrintf("%s: error status=%u", __func__, status);
  sem_destroy(&sFormatSem);

  if (sCurrentConnectedTargetProtocol == NFA_PROTOCOL_ISO_DEP) {
    int retCode = NFCSTATUS_SUCCESS;
    retCode = nativeNfcTag_doReconnect(e, o);
  }
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", __func__);
  return (status == NFA_STATUS_OK) ? JNI_TRUE : JNI_FALSE;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doMakeReadonlyResult
**
** Description:     Receive the result of making a tag read-only. Called by the
**                  NFA_SET_TAG_RO_EVT.
**                  status: Status of the operation.
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_doMakeReadonlyResult(tNFA_STATUS status) {
  if (sMakeReadonlyWaitingForComplete != JNI_FALSE) {
    sMakeReadonlyWaitingForComplete = JNI_FALSE;
    sMakeReadonlyStatus = status;

    sem_post(&sMakeReadonlySem);
  }
}

/*******************************************************************************
**
** Function:        nativeNfcTag_doMakeReadonly
**
** Description:     Make the tag read-only.
**                  e: JVM environment.
**                  o: Java object.
**                  key: Key to access the tag.
**
** Returns:         True if ok.
**
*******************************************************************************/
static jboolean nativeNfcTag_doMakeReadonly(JNIEnv* e, jobject o, jbyteArray) {
  jboolean result = JNI_FALSE;
  tNFA_STATUS status;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);

  /* Create the make_readonly semaphore */
  if (sem_init(&sMakeReadonlySem, 0, 0) == -1) {
    LOG(ERROR) << StringPrintf(
        "%s: Make readonly semaphore creation failed (errno=0x%08x)", __func__,
        errno);
    return JNI_FALSE;
  }

  sMakeReadonlyWaitingForComplete = JNI_TRUE;

  // Hard-lock the tag (cannot be reverted)
  status = NFA_RwSetTagReadOnly(TRUE);
  if (status == NFA_STATUS_REJECTED) {
    status = NFA_RwSetTagReadOnly(FALSE);  // try soft lock
    if (status != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail soft lock, status=%d", __func__,
                                 status);
      goto TheEnd;
    }
  } else if (status != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: fail hard lock, status=%d", __func__,
                               status);
    goto TheEnd;
  }

  /* Wait for check NDEF completion status */
  if (sem_wait(&sMakeReadonlySem)) {
    LOG(ERROR) << StringPrintf(
        "%s: Failed to wait for make_readonly semaphore (errno=0x%08x)",
        __func__, errno);
    goto TheEnd;
  }

  if (sMakeReadonlyStatus == NFA_STATUS_OK) {
    result = JNI_TRUE;
  }

TheEnd:
  /* Destroy semaphore */
  if (sem_destroy(&sMakeReadonlySem)) {
    LOG(ERROR) << StringPrintf(
        "%s: Failed to destroy read_only semaphore (errno=0x%08x)", __func__,
        errno);
  }
  sMakeReadonlyWaitingForComplete = JNI_FALSE;
  return result;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_registerNdefTypeHandler
**
** Description:     Register a callback to receive NDEF message from the tag
**                  from the NFA_NDEF_DATA_EVT.
**
** Returns:         None
**
*******************************************************************************/
// register a callback to receive NDEF message from the tag
// from the NFA_NDEF_DATA_EVT;
void nativeNfcTag_registerNdefTypeHandler() {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  sNdefTypeHandlerHandle = NFA_HANDLE_INVALID;
  NFA_RegisterNDefTypeHandler(TRUE, NFA_TNF_DEFAULT, (uint8_t*)"", 0,
                              ndefHandlerCallback);
}

/*******************************************************************************
**
** Function:        nativeNfcTag_deregisterNdefTypeHandler
**
** Description:     No longer need to receive NDEF message from the tag.
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_deregisterNdefTypeHandler() {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  NFA_DeregisterNDefTypeHandler(sNdefTypeHandlerHandle);
  sNdefTypeHandlerHandle = NFA_HANDLE_INVALID;
}

/*******************************************************************************
**
** Function:        nativeNfcTag_acquireRfInterfaceMutexLock
**
** Description:     acquire sRfInterfaceMutex
**
** Returns:         None
**
*******************************************************************************/
void nativeNfcTag_acquireRfInterfaceMutexLock() {
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: try to acquire lock", __func__);
  sRfInterfaceMutex.lock();
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: sRfInterfaceMutex lock", __func__);
}

/*******************************************************************************
**
** Function:       nativeNfcTag_releaseRfInterfaceMutexLock
**
** Description:    release the sRfInterfaceMutex
**
** Returns:        None
**
*******************************************************************************/
void nativeNfcTag_releaseRfInterfaceMutexLock() {
  sRfInterfaceMutex.unlock();
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: sRfInterfaceMutex unlock", __func__);
}

/*****************************************************************************
**
** JNI functions for Android 4.0.3
**
*****************************************************************************/
static JNINativeMethod gMethods[] = {
    {"doConnect", "(I)I", (void*)nativeNfcTag_doConnect},
    {"doDisconnect", "()Z", (void*)nativeNfcTag_doDisconnect},
    {"doReconnect", "()I", (void*)nativeNfcTag_doReconnect},
    {"doHandleReconnect", "(I)I", (void*)nativeNfcTag_doHandleReconnect},
    {"doTransceive", "([BZ[I)[B", (void*)nativeNfcTag_doTransceive},
    {"doGetNdefType", "(II)I", (void*)nativeNfcTag_doGetNdefType},
    {"doCheckNdef", "([I)I", (void*)nativeNfcTag_doCheckNdef},
    {"doRead", "()[B", (void*)nativeNfcTag_doRead},
    {"doWrite", "([B)Z", (void*)nativeNfcTag_doWrite},
    {"doPresenceCheck", "()Z", (void*)nativeNfcTag_doPresenceCheck},
    {"doIsIsoDepNdefFormatable", "([B[B)Z",
     (void*)nativeNfcTag_doIsIsoDepNdefFormatable},
    {"doNdefFormat", "([B)Z", (void*)nativeNfcTag_doNdefFormat},
    {"doMakeReadonly", "([B)Z", (void*)nativeNfcTag_doMakeReadonly},
};

/*******************************************************************************
**
** Function:        register_com_android_nfc_NativeNfcTag
**
** Description:     Regisgter JNI functions with Java Virtual Machine.
**                  e: Environment of JVM.
**
** Returns:         Status of registration.
**
*******************************************************************************/
int register_com_android_nfc_NativeNfcTag(JNIEnv* e) {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  return jniRegisterNativeMethods(e, gNativeNfcTagClassName, gMethods,
                                  NELEM(gMethods));
}

} /* namespace android */
