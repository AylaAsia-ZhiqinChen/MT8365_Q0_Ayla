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

/*
 *  Communicate with secure elements that are attached to the NFC
 *  controller.
 */
#pragma once
#include "SyncEvent.h"
#include "DataQueue.h"
#include "StNfcJni.h"

#include "nfa_ee_api.h"
#include "nfa_hci_api.h"
#include "nfa_hci_defs.h"
#include "nfa_ce_api.h"

typedef struct {
  bool opened;
  bool created;
} tJNI_ADPU_PIPE_INFO;

typedef struct {
  uint8_t id;
  /* NCI 2.0 - Begin */
  uint8_t nfceeId;
  /* NCI 2.0 - End */
  bool connected;
} tSE;

typedef struct {
  tSE info[2];
} tJNI_SE_INFO;

typedef struct {
  uint8_t data[30];
  int length;
} tJNI_ATR_INFO;

class StSecureElement {
 public:
  tJNI_ATR_INFO mAtrInfo;
  static const uint8_t mSeMASK[2];
  tJNI_SE_INFO mSeInfo;

  static const tNFA_HANDLE EE_HANDLE_HCI_NETWORK = 0x480;
  static const uint8_t ESE_ID = 0xC0;

  uint8_t getActiveNfcee(uint8_t defaultNfceeId);
  uint8_t getConnectedNfceeId(uint8_t id);

  /*******************************************************************************
   **
   ** Function:        getHostList
   **
   ** Description:     Asks for host list on Admin gate (HCI commands)
   **
   ** Returns:         None.
   **
   *******************************************************************************/
  void getHostList();

  /*******************************************************************************
   **
   ** Function:        isSEConnected
   **
   ** Description:     Retrieves information on given SE Id from HOST LIST
   *reading
   **
   ** Returns:         None.
   **
   *******************************************************************************/
  bool isSEConnected(int se_id);

  /*******************************************************************************
  **
  ** Function:        getInstance
  **
  ** Description:     Get the StSecureElement singleton object.
  **
  ** Returns:         StSecureElement object.
  **
  *******************************************************************************/
  static StSecureElement& getInstance();

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
  bool initialize(nfc_jni_native_data* native);

  /*******************************************************************************
  **
  ** Function:        finalize
  **
  ** Description:     Release all resources.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  void finalize();

  /*******************************************************************************
  **
  ** Function:        deactivate
  **
  ** Description:     Turn off the secure element.
  **                  seID: ID of secure element.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool deactivate(jint seID);

  /*******************************************************************************
  **
  ** Function:        connectEE
  **
  ** Description:     Connect to the execution environment.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool connectEE();

  /*******************************************************************************
  **
  ** Function:        disconnectEE
  **
  ** Description:     Disconnect from the execution environment.
  **                  seID: ID of secure element.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool disconnectEE(jint seID);

  /*******************************************************************************
  **
  ** Function:        transceive
  **
  ** Description:     Send data to the secure element; read it's response.
  **                  xmitBuffer: Data to transmit.
  **                  xmitBufferSize: Length of data.
  **                  recvBuffer: Buffer to receive response.
  **                  recvBufferMaxSize: Maximum size of buffer.
  **                  recvBufferActualSize: Actual length of response.
  **                  timeoutMillisec: timeout in millisecond
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  bool transceive(uint8_t* xmitBuffer, int32_t xmitBufferSize,
                  uint8_t* recvBuffer, int32_t recvBufferMaxSize,
                  int32_t& recvBufferActualSize, int32_t timeoutMillisec);

  void notifyModeSet(tNFA_EE_MODE_SET modeSet);
  void notifyEeStatus(tNFA_HANDLE eeHandle, uint8_t status);
  void notifyPowerCtrlRsp();
  void endOfApduTransfer();

  /*******************************************************************************
  **
  ** Function:        connectionEventHandler
  **
  ** Description:     Receive card-emulation related events from stack.
  **                  event: Event code.
  **                  eventData: Event data.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  void connectionEventHandler(uint8_t event, tNFA_CONN_EVT_DATA* eventData);

  /*******************************************************************************
  **
  ** Function:        isBusy
  **
  ** Description:     Whether NFC controller is routing listen-mode events or a
  *pipe is connected.
  **
  ** Returns:         True if either case is true.
  **
  *******************************************************************************/
  bool isBusy();

  /*******************************************************************************
  **
  ** Function:        EnableSE
  **
  ** Description:     Turn on the secure element.
  **                  seID: ID of secure element; 0xF3 or 0xF4.
  **                  enable : enable (0x1) or disable (0x0) the se.
  **
  ** Returns:         True if ok.
  **
  *******************************************************************************/
  /* NCI 2.0 - Begin */
  bool EnableSE(int seID, bool enable);
  /* NCI 2.0 - End */

  void SeActivationLock();
  void SeActivationUnlock();

  /* NCI 2.0 - Begin */
  int retrieveHostList(uint8_t* ptrHostList, uint8_t* ptrInfo);
  void resetEEInfo();

  uint8_t getSENfceeId(uint8_t host_id);
  /* NCI 2.0 - End */

  static const uint8_t UICC_IDX = 0x00;
  static const uint8_t ESE_IDX = 0x01;

 private:
  static const unsigned int MAX_RESPONSE_SIZE = 1024;
  //    enum RouteSelection {NoRoute, DefaultRoute, SecElemRoute};
  enum RxEvtType { EvtTransmitData, EvtWtxRequest };
  /* NCI 2.0 - Begin */
  static const int MAX_NUM_EE = 4;  // max number of EE's
  /* NCI 2.0 - End */
  static StSecureElement sSecElem;
  static const char* APP_NAME;

  static const uint8_t ESE_ADPU_GATE_ID = 0xF0;
  static const uint8_t EVT_SE_SOFT_RESET = 0x11;
  static const uint8_t EVT_TRANSMIT_DATA = 0x10;
  static const uint8_t ESE_ATR_REG_IDX = 0x01;
  static const uint8_t INVALID_BWI = 0xFF;
  /* NCI 2.0 - Begin */
  static const uint8_t EVT_ATR = 0x12;
  /* NCI 2.0 - End */
  static const uint16_t DEFAULT_TX_WAITING_TIME = 2000;

  static const uint8_t UICC_ID = 0x02;
  static const uint8_t UICC_MASK = 0x1;
  static const uint8_t ESE_MASK = 0x2;

  static const uint8_t ESE_GSMA_ID = 0x01;
  static const uint8_t UICC_GSMA_ID = 0x02;
  static const uint8_t EVT_WTX_REQUEST = 0x11;
  /* NCI 2.0 - Begin */
  static const tNFA_HANDLE ESE_HANDLE1 = 0x482;
  static const tNFA_HANDLE ESE_HANDLE2 = 0x484;
  /* NCI 2.0 - End */

  static const uint8_t HOT_PLUG_MASK = 0x1;
  static const uint8_t SE_INHIBITED_MASK = 0x2;
  static const uint8_t SE_INITIALIZING = 0x4;
  static const uint8_t HOT_PLUG_INFO_INHIBITED_MASK = 0x80;
  static const uint8_t HOT_PLUG_INFO_SE_ID_MASK = 0x1;
  static const uint8_t HOT_PLUG_TIMER = 50;          // ms
  static const uint16_t SE_ACTIVATION_TIMER = 1950;  // ms

  tNFA_HANDLE mNfaHciHandle;  // NFA handle to NFA's HCI component
  nfc_jni_native_data* mNativeData;
  bool mIsInit;             // whether EE is initialized
  uint8_t mActualNumEe;     // actual number of EE's reported by the stack
  uint8_t mNumEePresent;    // actual number of usable EE's
  uint8_t mNumActivatedEe;  // number of activated EE's.
  bool mbNewEE;
  bool mIsPiping;           // is a pipe connected to the controller?
  int mActualResponseSize;  // number of bytes in the response received from
                            // secure element

  tNFA_EE_INFO mEeInfo[MAX_NUM_EE];  // actual size stored in mActualNumEe

  SyncEvent mHciRegisterEvent;
  SyncEvent mEeSetModeEvent;
  SyncEvent mUiccListenEvent;
  SyncEvent mTransceiveEvent;

  SyncEvent mHotPlugEvent;
  SyncEvent mHostListEvent;
  SyncEvent mApduInfoEvent;
  SyncEvent mApduReadAtrEvent;

  /* NCI 2.0 - Begin */
  SyncEvent mPowerCtrlEvent;
  /* NCI 2.0 - End */

  SyncEvent mSeActivationEvent;

  /* NCI 2.0 - Begin */
  SyncEvent mCreatePipeEvent;
  SyncEvent mOpenPipeEvent;
  /* NCI 2.0 - End */

  uint8_t mResponseData[MAX_RESPONSE_SIZE];
  uint8_t mAidForEmptySelect[NCI_MAX_AID_LEN + 1];

  uint8_t mUiccListenMask;
  uint8_t mMaxNbSe;
  tJNI_ADPU_PIPE_INFO mAdpuPipeInfo;

  uint16_t mTxWaitingTime;
  uint8_t mBwi;
  uint8_t mEseApduPipeId;
  /* NCI 2.0 - Begin */
  uint8_t mEseApduGateId;
  /* NCI 2.0 - End */

  uint8_t mRxEvtType;
  bool mEeModeSet;
  uint8_t mSeActivationBitmap;

  /*******************************************************************************
  **
  ** Function:        StSecureElement
  **
  ** Description:     Initialize member variables.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  StSecureElement();

  /*******************************************************************************
  **
  ** Function:        ~StSecureElement
  **
  ** Description:     Release all resources.
  **
  ** Returns:         None
  **
  *******************************************************************************/
  ~StSecureElement();

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
  static void nfaEeCallback(tNFA_EE_EVT event, tNFA_EE_CBACK_DATA* eventData);

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
  static void nfaHciCallback(tNFA_HCI_EVT event, tNFA_HCI_EVT_DATA* eventData);

  /*******************************************************************************
  **
  ** Function:        findEeByHandle
  **
  ** Description:     Find information about an execution environment.
  **                  eeHandle: Handle to execution environment.
  **
  ** Returns:         Information about an execution environment.
  **
  *******************************************************************************/
  tNFA_EE_INFO* findEeByHandle(tNFA_HANDLE eeHandle);

  /*******************************************************************************
  **
  ** Function:        getEeInfo
  **
  ** Description:     Get latest information about execution environments from
  *stack.
  **
  ** Returns:         True if at least 1 EE is available.
  **
  *******************************************************************************/
  bool getEeInfo();

  /*******************************************************************************
  **
  ** Function:        eeStatusToString
  **
  ** Description:     Convert status code to status text.
  **                  status: Status code
  **
  ** Returns:         None
  **
  *******************************************************************************/
  static const char* eeStatusToString(uint8_t status);

  /*******************************************************************************
   **
   ** Function:        abortWaits
   **
   ** Description:
   **
   ** Returns:
   **
   *******************************************************************************/
  void abortWaits();
  bool getApduSettings(void);
  tNFA_STATUS handleEnableSESeq(tNFA_EE_INFO* eeItem, bool status);
};
