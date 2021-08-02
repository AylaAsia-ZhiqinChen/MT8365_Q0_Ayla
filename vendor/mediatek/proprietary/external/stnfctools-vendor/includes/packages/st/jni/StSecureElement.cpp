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
#include <android-base/stringprintf.h>
#include <base/logging.h>
#include <nativehelper/ScopedLocalRef.h>
#include <nativehelper/ScopedPrimitiveArray.h>

#include "JavaClassConstants.h"
#include "StSecureElement.h"
#include "Mutex.h"
#include "StNfcJni.h"
#include "nfc_config.h"
#include "NfcStExtensions.h"

#define PIPE_CREATED_AND_OPENED_STATE 0x06

/*****************************************************************************
**
** public variables
**
*****************************************************************************/
using android::base::StringPrintf;
extern bool nfc_debug_enabled;

Mutex gMutex;
Mutex gMutexEE;

namespace android {
extern void startRfDiscovery(bool isStart);
}

//////////////////////////////////////////////
//////////////////////////////////////////////

StSecureElement StSecureElement::sSecElem;
const char* StSecureElement::APP_NAME = "nfc_jni";

const uint8_t StSecureElement::mSeMASK[2] = {UICC_MASK, ESE_MASK};
/*******************************************************************************
**
** Function:        StSecureElement
**
** Description:     Initialize member variables.
**
** Returns:         None
**
*******************************************************************************/
StSecureElement::StSecureElement()
    : mbNewEE(true),  // by default we start w/thinking there are new EE
      mMaxNbSe(0) {
  mSeActivationBitmap = 0;
  mNfaHciHandle = NFA_HANDLE_INVALID;
  mNativeData = NULL;
  mIsInit = false;
  mIsPiping = false;
  mUiccListenMask = 0;
  mNumActivatedEe = 0;
  mNumEePresent = 0;
  mActualNumEe = 0;
  mEeModeSet = 0;
  memset(&mEeInfo, 0, sizeof(mEeInfo));
  memset(mResponseData, 0, sizeof(mResponseData));
  memset(mAidForEmptySelect, 0, sizeof(mAidForEmptySelect));
  memset(&mSeInfo, 0, sizeof(mSeInfo));
  memset(&mAdpuPipeInfo, 0, sizeof(mAdpuPipeInfo));
  memset(&mAtrInfo, 0, sizeof(mAtrInfo));
}

/*******************************************************************************
**
** Function:        ~StSecureElement
**
** Description:     Release all resources.
**
** Returns:         None
**
*******************************************************************************/
StSecureElement::~StSecureElement() {}

/*******************************************************************************
**
** Function:        getInstance
**
** Description:     Get the StSecureElement singleton object.
**
** Returns:         StSecureElement object.
**
*******************************************************************************/
StSecureElement& StSecureElement::getInstance() { return sSecElem; }

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
bool StSecureElement::initialize(nfc_jni_native_data* native) {
  static const char fn[] = "StSecureElement::initialize";
  tNFA_STATUS nfaStat;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  mUiccListenMask = NfcConfig::getUnsigned("UICC_LISTEN_TECH_MASK", 0x00);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: Requested UICC listen mask: 0x%X", fn, mUiccListenMask);

  mMaxNbSe = NfcConfig::getUnsigned("MAX_NUMBER_OF_SE", 0x05);
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: Maximum number of SE supported by NFCC: 0x%X", fn, mMaxNbSe);

  mNumActivatedEe = 0;
  mNfaHciHandle = NFA_HANDLE_INVALID;

  mNativeData = native;
  mActualNumEe = MAX_NUM_EE;
  mbNewEE = true;
  memset(mEeInfo, 0, sizeof(mEeInfo));

  mSeInfo.info[UICC_IDX].id = UICC_ID;
  mSeInfo.info[ESE_IDX].id = ESE_ID;

  // Get Fresh EE info.
  if (!getEeInfo()) return (false);

  {
    SyncEventGuard guard(mHciRegisterEvent);

    nfaStat =
        NFA_HciRegister(const_cast<char*>(APP_NAME), nfaHciCallback, TRUE);
    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail hci register; error=0x%X", fn,
                                 nfaStat);
      return (false);
    }
    mHciRegisterEvent.wait();

    mEseApduPipeId = 0x10;  // dummy, don't care at this point

    // Add static pipe to accept notify pipe created from NFCC at eSE initial
    // activation
    nfaStat = NFA_HciAllocGate(mNfaHciHandle, 0xf0);

    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "%s: fail create static pipe for APDU gate; error=0x%X", fn, nfaStat);
      return (false);
    }
  }

  mIsInit = true;
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
  return (true);
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
void StSecureElement::finalize() {
  static const char fn[] = "StSecureElement::finalize";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  mNfaHciHandle = NFA_HANDLE_INVALID;
  mNativeData = NULL;
  mIsInit = false;
  mActualNumEe = 0;
  mNumEePresent = 0;
  mNumActivatedEe = 0;
  mIsPiping = false;
  memset(mEeInfo, 0, sizeof(mEeInfo));
  sSecElem.mSeInfo.info[UICC_IDX].connected = false;
  sSecElem.mSeInfo.info[ESE_IDX].connected = false;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
}

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
bool StSecureElement::getEeInfo() {
  static const char fn[] = "StSecureElement::getEeInfo";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter; mbNewEE=%d, mActualNumEe=%d", fn, mbNewEE, mActualNumEe);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;

  // If mbNewEE is true then there is new EE info.
  if (mbNewEE) {
    mActualNumEe = MAX_NUM_EE;
    gMutexEE.lock();
    if ((nfaStat = NFA_EeGetInfo(&mActualNumEe, mEeInfo)) != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf("%s: fail get info; error=0x%X", fn, nfaStat);
      mActualNumEe = 0;
      gMutexEE.unlock();
    } else {
      gMutexEE.unlock();
      mbNewEE = false;

      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: num EEs discovered: %u", fn, mActualNumEe);
      if (mActualNumEe != 0) {
        sSecElem.mSeInfo.info[UICC_IDX].connected = false;
        sSecElem.mSeInfo.info[ESE_IDX].connected = false;
        for (uint8_t xx = 0; xx < mActualNumEe; xx++) {
          /* NCI 2.0 - Begin */
          if (mEeInfo[xx].num_interface == 0) /* NCI 2.0 - End */
            mNumEePresent++;

          /* NCI 2.0 - Begin */
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: NFCEE[%u] Handle: 0x%04x  Status: %s  Nb protocol: %u: "
              "(0x%02x)  Num TLVs: %u",
              fn, xx, mEeInfo[xx].ee_handle,
              eeStatusToString(mEeInfo[xx].ee_status),
              mEeInfo[xx].num_interface, mEeInfo[xx].ee_interface[0],
              mEeInfo[xx].num_tlvs);
          /* NCI 2.0 - End */
          for (int yy = 0; yy < mEeInfo[xx].num_tlvs; yy++) {
            DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
                "%s: EE[%u] TLV[%u]  Tag: 0x%02x  Len: %u  Values[]: 0x%02x  "
                "0x%02x  0x%02x ...",
                fn, xx, yy, mEeInfo[xx].ee_tlv[yy].tag,
                mEeInfo[xx].ee_tlv[yy].len, mEeInfo[xx].ee_tlv[yy].info[0],
                mEeInfo[xx].ee_tlv[yy].info[1], mEeInfo[xx].ee_tlv[yy].info[2]);
          }

          switch (mEeInfo[xx].ee_handle & 0xFF) {
            case 0x81:
            case 0x83:
            case 0x85:
              if (mEeInfo[xx].ee_status == NFA_EE_STATUS_ACTIVE) {
                sSecElem.mSeInfo.info[UICC_IDX].nfceeId =
                    (mEeInfo[xx].ee_handle & 0xFF);
                sSecElem.mSeInfo.info[UICC_IDX].connected = true;
                DLOG_IF(INFO, nfc_debug_enabled)
                    << StringPrintf("%s: Active UICC is at slot 0x%x", fn,
                                    sSecElem.mSeInfo.info[UICC_IDX].nfceeId);
              }

              break;
            case 0x82:
            case 0x84:
            case 0x86:
              if (mEeInfo[xx].ee_status == NFA_EE_STATUS_ACTIVE) {
                sSecElem.mSeInfo.info[ESE_IDX].nfceeId =
                    (mEeInfo[xx].ee_handle & 0xFF);
                sSecElem.mSeInfo.info[ESE_IDX].connected = true;
                DLOG_IF(INFO, nfc_debug_enabled)
                    << StringPrintf("%s: Active eSE is at slot 0x%x", fn,
                                    sSecElem.mSeInfo.info[ESE_IDX].nfceeId);
              }
              break;
            default:
              DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
                  "%s: Unexpected handle: 0x%x", fn, mEeInfo[xx].ee_handle);
          }
        }  // end for
      }
    }
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit; mActualNumEe=%d, mNumEePresent=%d", fn,
                      mActualNumEe, mNumEePresent);
  return (mActualNumEe != 0);
}

/*******************************************************************************
 **
 ** Function:        getHostList
 **
 ** Description:     Asks for host list on Admin gate (HCI commands)
 **
 ** Returns:         None.
 **
 *******************************************************************************/
void StSecureElement::getHostList() {
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  static const char fn[] = "StSecureElement::getHostList";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  // Get HOST_LIST
  SyncEventGuard guard(mHostListEvent);
  if ((nfaStat = NFA_HciGetHostList(mNfaHciHandle)) == NFA_STATUS_OK) {
    mHostListEvent.wait();  // wait for NFA_HCI_HOST_LIST_EVT
  } else
    LOG(ERROR) << StringPrintf("%s: NFA_HciGetHostList failed; error=0x%X", fn,
                               nfaStat);
}

/*******************************************************************************
 **
 ** Function:        isSEConnected
 **
 ** Description:     Retrieves information on se from HOST LIST reading
 **
 ** Parameters:      se_id: the HCI id (0x02 or 0xC0)
 **                    or NFCEE ID (0x81 or 0x83 etc)
 **
 ** Returns:         true if the SE is active on HCI, false otherwise.
 **
 *******************************************************************************/
bool StSecureElement::isSEConnected(int se_id) {
  static const char fn[] = "StSecureElement::isSEConnected";
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter (0x%x)", fn, se_id);
  uint8_t i;

  /* NCI 2.0 - Begin */
  for (i = 0; i < (sizeof(mSeInfo.info) / sizeof(mSeInfo.info[0])); i++)
  /* NCI 2.0 - End */
  {
    if ((mSeInfo.info[i].id == se_id) || (mSeInfo.info[i].nfceeId == se_id)) {
      return mSeInfo.info[i].connected;
    }
  }

  return false;
}

/*******************************************************************************
 **
 ** Function:        getApduSettings
 **
 ** Description:
 **
 ** Returns:         True if ok.
 **
 *******************************************************************************/
bool StSecureElement::getApduSettings(void) {
  static const char fn[] = "StSecureElement::getApduSettings";
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;

  // getHostList();

  if (mSeInfo.info[ESE_IDX].connected == true) {
    mAdpuPipeInfo.created = false;
    mAdpuPipeInfo.opened = false;

    // Check if we have Prop APDU in pipe list
    // Check if ADPU gate if OK => GET_INFO/PIPE_INFO en device mgmt gate
    mEseApduGateId = ESE_ADPU_GATE_ID;
    mEseApduPipeId = NfcStExtensions::getInstance().getPipeIdForGate(
        NfcStExtensions::getInstance().DH_HCI_ID, mEseApduGateId);

// If no pipe was found for prop APDU
// Try rel12 APDU gate
#if 0
        if (mEseApduPipeId == 0xFF)
        {
            mEseApduGateId = 0x30;
            // Check if ADPU gate if OK => GET_INFO/PIPE_INFO en device mgmt gate
            mEseApduPipeId = NfcStExtensions::getInstance().getPipeIdForGate(NfcStExtensions::getInstance().DH_HCI_ID, mEseApduGateId);

            if (mEseApduPipeId == 0xFF)
            {
                //Try and create rel12 APDU pipe to eSE
                nfaStat = NFA_HciAllocGate(mNfaHciHandle, mEseApduGateId);

                if (nfaStat != NFA_STATUS_OK)
                {
                  LOG(ERROR) << StringPrintf("%s: Couldn't allocate APDU gate %02X; error=0x%X", fn, mEseApduGateId, nfaStat);

                    return;
                }

                {
                    //Send create pipe command
                    SyncEventGuard guard(mCreatePipeEvent);
                    if ((nfaStat = NFA_HciCreatePipe (mNfaHciHandle,
                            mEseApduGateId,
                                    0xc0,
                                    mEseApduGateId)) == NFA_STATUS_OK)
                    {
                        mCreatePipeEvent.wait(); //wait for NFA_EE_MODE_SET_EVT
                    }
                }

                //Pipe was created, open it
                if (mAdpuPipeInfo.created == true)
                {
                    SyncEventGuard guard(mOpenPipeEvent);
                    if ((nfaStat = NFA_HciOpenPipe(mNfaHciHandle, mEseApduPipeId)) == NFA_STATUS_OK)
                    {
                        mOpenPipeEvent.wait(); //wait for NFA_EE_MODE_SET_EVT
                    }
                }
            }
        }
#endif
  } else {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: eSE is not connected!!", fn);
    return false;
  }

  // If we found prop APDU in pipe list, use it
  if ((mEseApduPipeId != 0xFF) && (mEseApduGateId == ESE_ADPU_GATE_ID)) {
    // Add static pipe to accept notify pipe created from NFCC at eSE initial
    // activation

    nfaStat = NFA_HciAllocGate(mNfaHciHandle, ESE_ADPU_GATE_ID);

    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "%s: Couldn't allocate APDU gate %02X; error=0x%X", fn,
          mEseApduGateId, nfaStat);

      return false;
    }

    // Add static pipe
    nfaStat = NFA_HciAddStaticPipe(mNfaHciHandle, 0xc0, ESE_ADPU_GATE_ID,
                                   mEseApduPipeId);
    if (nfaStat != NFA_STATUS_OK) {
      LOG(ERROR) << StringPrintf(
          "%s: Couldn't add static pipe for APDU gate; error=0x%X", fn,
          nfaStat);

      return false;
    }
  }

  if (mAdpuPipeInfo.created == false) {
    uint8_t state = NfcStExtensions::getInstance().getPipeState(mEseApduPipeId);

    if (state == PIPE_CREATED_AND_OPENED_STATE) {
      mAdpuPipeInfo.created = true;
      mAdpuPipeInfo.opened = true;
    }
  }

  return true;
}

/*******************************************************************************
**
** Function:        deactivate
**
** Description:     Turn off the secure element.
**                  seID: ID of secure element; 0xF3 or 0xF4.
**
** Returns:         True if ok.
**
*******************************************************************************/
bool StSecureElement::deactivate(jint seID) {
  static const char fn[] = "StSecureElement::deactivate";
  bool retval = false;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; seID=0x%X", fn, seID);

  if (!mIsInit) {
    LOG(ERROR) << StringPrintf("%s: not init", fn);
    goto TheEnd;
  }

  if (mIsPiping) {
    LOG(ERROR) << StringPrintf("%s: still busy", fn);
    goto TheEnd;
  }
  retval = true;

TheEnd:
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit; ok=%u", fn, retval);
  return retval;
}

/*******************************************************************************
**
** Function:        connectEE
**
** Description:     Connect to the execution environment.
**
** Returns:         True if ok.
**
*******************************************************************************/
bool StSecureElement::connectEE() {
  static const char fn[] = "StSecureElement::connectEE";
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  bool retVal = false;
  //  tNFA_HANDLE eeHandle = mActiveeSEHandle;

  if (!mIsInit) {
    LOG(ERROR) << StringPrintf("%s: not init", fn);
    return (false);
  }

  uint8_t nfceeId = getSENfceeId(ESE_ID);
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter, Active eSE ID(nfceeId): 0x%04x", fn, nfceeId);

  /* NCI 2.0 - Begin */
  // Check if and which APDU gate is available
  if (getApduSettings() == false) {
    return false;
  }
  /* NCI 2.0 - End */

  // Check if APDU gate on eSE has been created/opened
  if (mAdpuPipeInfo.created && mAdpuPipeInfo.opened) {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: APDU pipe is up and running", fn);
    retVal = true;

    /* NCI 2.0 - Begin */
    if (mEseApduGateId == ESE_ADPU_GATE_ID) {
      /* NCI 2.0 - End */
      // Sent EVT_SE_SOFT_RESET to eSE => no answer awaited
      nfaStat =
          NFA_HciSendEvent(mNfaHciHandle, mEseApduPipeId, EVT_SE_SOFT_RESET, 0,
                           NULL, sizeof(mResponseData), mResponseData, 0);
      if (nfaStat != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf(
            "%s: failed transmitting EVT_SE_SOFT_RESET to eSE; error=0x%X", fn,
            nfaStat);
        retVal = false;
        goto TheEnd;
      }

      // Get ATR bytes
      SyncEventGuard guard(mApduReadAtrEvent);

      nfaStat =
          NFA_HciGetRegistry(mNfaHciHandle, mEseApduPipeId, ESE_ATR_REG_IDX);
      if (nfaStat != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf(
            "%s: failed getting ATR bytes from APDU gate; error=0x%X", fn,
            nfaStat);
        retVal = false;
        goto TheEnd;
      }
      mApduReadAtrEvent.wait();  // Wait for ATR value

      mTxWaitingTime = DEFAULT_TX_WAITING_TIME;

      if (mBwi != INVALID_BWI) {
        /* NCI 2.0 - Begin */
        mTxWaitingTime = (0x1 << mBwi) * 200;  // in ms
        /* NCI 2.0 - End */
        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: Waiting time value for transceiving on APDU gate is %d ms", fn,
            mTxWaitingTime);
      } else {
        LOG(ERROR) << StringPrintf(
            "%s: Error in getting Waiting time value for transceiving on APDU "
            "gate, use default (2000 ms)",
            fn);
      }

      /* NCI 2.0 - Begin */
      {
        // Send NFCEE_POWER_AND_LINK_CNTRL_CMD to ensure NFCC won't put eSE to
        // sleep
        SyncEventGuard guard(mPowerCtrlEvent);
        /* NCI 2.0 - Begin */
        nfaStat = NFA_EePowerCtrl(nfceeId, 0x03);
        /* NCI 2.0 - End */
        // mPowerCtrlEvent.wait();
        if (mPowerCtrlEvent.wait(500) == false)  // wait for NFA_EE_MODE_SET_EVT
        {
          LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: timeout waiting for NFA_EE_POWER_CTRL_EVT", fn);
        }
      }
    } else  // rel12 APDU gate
    {
      // Read server APDU gate registry
      SyncEventGuard guard(mApduReadAtrEvent);

      nfaStat = NFA_HciGetRegistry(mNfaHciHandle, mEseApduPipeId, 0x02);
      if (nfaStat != NFA_STATUS_OK) {
        LOG(ERROR) << StringPrintf(
            "%s: failed getting ATR bytes from APDU gate; error=0x%X", fn,
            nfaStat);
        retVal = false;
        goto TheEnd;
      }
      mApduReadAtrEvent.wait();  // Wait for WTX value
    }
    /* NCI 2.0 - End */
  } else {
    LOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: APDU pipe has not been created", fn);
    retVal = false;
  }

TheEnd:
  mIsPiping = retVal;

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: exit", fn);
  return retVal;
}

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
bool StSecureElement::disconnectEE(jint seID) {
  static const char fn[] = "StSecureElement::disconnectEE";
  tNFA_HANDLE eeHandle = seID;

  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: seID=0x%X; handle=0x%04x", fn, seID, eeHandle);

  // Sent EVT_END_OF_APDU_TRANSFER to eSE => no answer awaited
  /* NCI 2.0 - Begin */
  endOfApduTransfer();
  /* NCI 2.0 - End */

  // Re-enable RF discovery
  // Note that it only effactuates the current configuration,
  // so if polling/listening were configured OFF (forex because
  // the screen was off), they will stay OFF with this call.
  // android::startRfDiscovery(true);

  mIsPiping = false;
  return true;
}

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
**                  timeoutMillisec: timeout in millisecond.
**
** Returns:         True if ok.
**
*******************************************************************************/
bool StSecureElement::transceive(uint8_t* xmitBuffer, int32_t xmitBufferSize,
                                 uint8_t* recvBuffer, int32_t recvBufferMaxSize,
                                 int32_t& recvBufferActualSize,
                                 int32_t timeoutMillisec) {
  static const char fn[] = "StSecureElement::transceive";
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  bool isSuccess = false;
  bool waitOk = false;
  uint8_t newSelectCmd[NCI_MAX_AID_LEN + 10];

  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter; xmitBufferSize=%d; recvBufferMaxSize=%d; timeout=%d", fn,
      xmitBufferSize, recvBufferMaxSize, timeoutMillisec);

  // Check if we need to replace an "empty" SELECT command.
  // 1. Has there been a AID configured, and
  // 2. Is that AID a valid length (i.e 16 bytes max), and
  // 3. Is the APDU at least 4 bytes (for header), and
  // 4. Is INS == 0xA4 (SELECT command), and
  // 5. Is P1 == 0x04 (SELECT by AID), and
  // 6. Is the APDU len 4 or 5 bytes.
  //
  // Note, the length of the configured AID is in the first
  //   byte, and AID starts from the 2nd byte.
  if (mAidForEmptySelect[0]                          // 1
      && (mAidForEmptySelect[0] <= NCI_MAX_AID_LEN)  // 2
      && (xmitBufferSize >= 4)                       // 3
      && (xmitBuffer[1] == 0xA4)                     // 4
      && (xmitBuffer[2] == 0x04)                     // 5
      && (xmitBufferSize <= 5))                      // 6
  {
    uint8_t idx = 0;

    // Copy APDU command header from the input buffer.
    memcpy(&newSelectCmd[0], &xmitBuffer[0], 4);
    idx = 4;

    // Set the Lc value to length of the new AID
    newSelectCmd[idx++] = mAidForEmptySelect[0];

    // Copy the AID
    memcpy(&newSelectCmd[idx], &mAidForEmptySelect[1], mAidForEmptySelect[0]);
    idx += mAidForEmptySelect[0];

    // If there is an Le (5th byte of APDU), add it to the end.
    if (xmitBufferSize == 5) newSelectCmd[idx++] = xmitBuffer[4];

    // Point to the new APDU
    xmitBuffer = &newSelectCmd[0];
    xmitBufferSize = idx;

    LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: Empty AID SELECT cmd detected, substituting AID from config file, "
        "new length=%d",
        fn, idx);
  }

  {
    SyncEventGuard guard(mTransceiveEvent);
    mActualResponseSize = 0;
    memset(mResponseData, 0, sizeof(mResponseData));
    nfaStat = NFA_HciSendEvent(mNfaHciHandle, mEseApduPipeId, EVT_TRANSMIT_DATA,
                               xmitBufferSize, xmitBuffer,
                               sizeof(mResponseData), mResponseData, 0);
    if (nfaStat == NFA_STATUS_OK) {
      do  // Wait for a EVT_TRANSMIT_DATA or timeout to exit. If rx
          // EVT_WTX_REQUEST before timeout, restart timer.
      {
        waitOk = mTransceiveEvent.wait(mTxWaitingTime);
        if (waitOk == false)  // timeout occurs
        {
          /* NCI 2.0 - Begin */
          LOG(ERROR) << StringPrintf(
              "%s: APDU gate, EVT_TRANSMIT_DATA wait response timeout", fn);
          /* NCI 2.0 - End */
          goto TheEnd;
        }
      } while (mRxEvtType != EvtTransmitData);
    } else {
      LOG(ERROR) << StringPrintf("%s: fail send data; error=0x%X", fn, nfaStat);
      goto TheEnd;
    }
  }

  if (mActualResponseSize > recvBufferMaxSize)
    recvBufferActualSize = recvBufferMaxSize;
  else
    recvBufferActualSize = mActualResponseSize;

  memcpy(recvBuffer, mResponseData, recvBufferActualSize);
  isSuccess = true;

TheEnd:
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: exit; isSuccess: %d; recvBufferActualSize: %d", fn,
                      isSuccess, recvBufferActualSize);
  return (isSuccess);
}

/*******************************************************************************
**
** Function:        notifyModeSet
**
** Description:     Called from RtouingManager to indicate the
*NFCEE_MODE_SET_RSP was rx
**
** Returns:         None
**
*******************************************************************************/
/* NCI 2.0 - Begin */
void StSecureElement::notifyModeSet(tNFA_EE_MODE_SET modeSet) {
  static const char* fn = "StSecureElement::notifyModeSet";
  tNFA_EE_INFO* pEE = sSecElem.findEeByHandle(modeSet.ee_handle);

  /* NCI 2.0 - Begin */
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: NFA_EE_MODE_SET_EVT; modeSet.status = 0x%02x, modeSet.ee_handle = "
      "0x%X, modeSet.ee_status = 0x%02X",
      fn, modeSet.status, modeSet.ee_handle, modeSet.ee_status);
  /* NCI 2.0 - End */

  if (pEE) {
    SyncEventGuard guard(sSecElem.mEeSetModeEvent);
    sSecElem.mEeSetModeEvent.notifyOne();
  } else {
    LOG(ERROR) << StringPrintf("%s: NFA_EE_MODE_SET_EVT; EE: 0x%04x not found.",
                               fn, modeSet.ee_handle);
  }
}

/*******************************************************************************
**
** Function:        notifyPowerCtrlRsp
**
** Description:
**
** Returns:         None
**
*******************************************************************************/
void StSecureElement::notifyPowerCtrlRsp() {
  static const char* fn = "StSecureElement::notifyPowerCtrlRsp";
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: NFA_EE_POWER_CTRL_EVT; ", fn);
  SyncEventGuard guard(sSecElem.mPowerCtrlEvent);
  sSecElem.mPowerCtrlEvent.notifyOne();
}

/*******************************************************************************
**
** Function:        notifyEeStatus
**
** Description:     Notify about the NFCEE status whether the init aws started
*or
**                  completed
**
** Returns:         None
**
*******************************************************************************/
/* NCI 2.0 - Begin */
void StSecureElement::notifyEeStatus(tNFA_HANDLE eeHandle, uint8_t status) {
  static const char* fn = "StSecureElement::notifyEeStatus";
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; ee status =%d", fn, status);
  tNFA_EE_INFO* pEE = sSecElem.findEeByHandle(eeHandle);

  //  if (eeHandle == ESE_HANDLE) mActiveeSEHandle = ESE_HANDLE;

  if (status == NFCEE_STATUS_INIT_STARTED) {
    SyncEventGuard guard(sSecElem.mSeActivationEvent);
    /* the SE starting an initialization, we will need to wait for this to
     * complete */
    mSeActivationBitmap |= SE_INITIALIZING;
    sSecElem.mSeActivationEvent.notifyOne();
  }

  if (status == NFCEE_STATUS_INIT_COMPLETED) {
    SyncEventGuard guard(sSecElem.mSeActivationEvent);
    mSeActivationBitmap &= ~SE_INITIALIZING;
    sSecElem.mSeActivationEvent.notifyOne();
  }

  if (status == NFCEE_STATUS_ERROR) {
    LOG(ERROR) << StringPrintf("%s: EE ID %x unrecoverable error", fn,
                               eeHandle & 0xFF);
    if (pEE) {
      pEE->ee_status = NFA_EE_STATUS_UNRESPONSIVE;
    } else {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: pEE is NULL", fn);
    }
  }
}

/*******************************************************************************
**
** Function         abortWaits
**
** Description      Returns number of secure elements we know about.
**
** Returns          Number of secure elements we know about.
**
*******************************************************************************/
void StSecureElement::abortWaits() {
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s", __func__);
  {
    SyncEventGuard g(mHciRegisterEvent);
    mHciRegisterEvent.notifyOne();
  }
  {
    SyncEventGuard g(mEeSetModeEvent);
    mEeSetModeEvent.notifyOne();
  }
  {
    SyncEventGuard g(mUiccListenEvent);
    mUiccListenEvent.notifyOne();
  }
  {
    SyncEventGuard g(mHotPlugEvent);
    mHotPlugEvent.notifyOne();
  }
  {
    SyncEventGuard g(mHostListEvent);
    mHostListEvent.notifyOne();
  }
}

/*******************************************************************************
**
** Function:        nfaHciCallback
**
** Description:     Receive Host Controller Interface-related events from stack.
**                  event: Event code.
**                  eventData: Event data.
**
** Returns:         None
**
*******************************************************************************/
void StSecureElement::nfaHciCallback(tNFA_HCI_EVT event,
                                     tNFA_HCI_EVT_DATA* eventData) {
  static const char fn[] = "StSecureElement::nfaHciCallback";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: event=0x%X", fn, event);

  switch (event) {
    case NFA_HCI_REGISTER_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_REGISTER_EVT; status=0x%X; handle=0x%X", fn,
          eventData->hci_register.status, eventData->hci_register.hci_handle);
      SyncEventGuard guard(sSecElem.mHciRegisterEvent);
      sSecElem.mNfaHciHandle = eventData->hci_register.hci_handle;
      sSecElem.mHciRegisterEvent.notifyOne();
    } break;

    case NFA_HCI_ALLOCATE_GATE_EVT: {
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_ALLOCATE_GATE_EVT; status=0x%X; gate=0x%X", fn,
          eventData->status, eventData->allocated.gate);
    } break;

    case NFA_HCI_CREATE_PIPE_EVT: {
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_CREATE_PIPE_EVT; status=0x%X; pipe=0x%X; src gate=0x%X; "
          "dest host=0x%X; dest gate=0x%X",
          fn, eventData->created.status, eventData->created.pipe,
          eventData->created.source_gate, eventData->created.dest_host,
          eventData->created.dest_gate);

      if ((eventData->created.dest_host == ESE_ID) &&
          (eventData->created.dest_gate == ESE_ADPU_GATE_ID)) {
        sSecElem.mAdpuPipeInfo.created = true;
      }
      /* NCI 2.0 - Begin */
      else if ((eventData->created.dest_host == ESE_ID) &&
               (eventData->created.dest_gate == 0x30)) {
        if (eventData->created.status == NFC_STATUS_OK) {
          sSecElem.mAdpuPipeInfo.created = true;
          sSecElem.mEseApduPipeId = eventData->created.pipe;
        }

        SyncEventGuard guard(sSecElem.mCreatePipeEvent);
        sSecElem.mCreatePipeEvent.notifyOne();
      }
      /* NCI 2.0 - End */
    } break;

    case NFA_HCI_OPEN_PIPE_EVT: {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: NFA_HCI_OPEN_PIPE_EVT; status=0x%X; pipe=0x%X",
                          fn, eventData->opened.status, eventData->opened.pipe);

      if ((eventData->opened.status == NFA_STATUS_OK) &&
          (eventData->opened.pipe ==
           sSecElem.mEseApduPipeId))  // ADPU pipe opened
      {
        sSecElem.mAdpuPipeInfo.opened = true;
      }
    } break;

    case NFA_HCI_RSP_RCVD_EVT:  // response received from secure element
    {
      tNFA_HCI_RSP_RCVD& rsp_rcvd = eventData->rsp_rcvd;
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_RSP_RCVD_EVT; status: 0x%X; code: 0x%X; pipe: 0x%X; "
          "len: %u",
          fn, rsp_rcvd.status, rsp_rcvd.rsp_code, rsp_rcvd.pipe,
          rsp_rcvd.rsp_len);
    } break;

    case NFA_HCI_GET_REG_RSP_EVT:
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_GET_REG_RSP_EVT; status: 0x%X; pipe: 0x%X, len: %d", fn,
          eventData->registry.status, eventData->registry.pipe,
          eventData->registry.data_len);
      if (eventData->registry.pipe == sSecElem.mEseApduPipeId) {
        /* NCI 2.0 - Begin */
        if (sSecElem.mEseApduGateId == ESE_ADPU_GATE_ID) {
          /* NCI 2.0 - End */
          sSecElem.mAtrInfo.length = eventData->registry.data_len;

          for (int g = 0; g < eventData->registry.data_len; g++) {
            LOG_IF(INFO, nfc_debug_enabled)
                << StringPrintf("%s: eventData->registry.reg_data[%d] = 0x%x",
                                fn, g, eventData->registry.reg_data[g]);
            sSecElem.mAtrInfo.data[g] = eventData->registry.reg_data[g];
          }

          // Try and get the BWI value in ATR response
          {
            uint8_t bwi_idx = 1, level = 1, nb_bit_set;

            sSecElem.mBwi = INVALID_BWI;

            while (level != 3)  // BWI is 4MSB of TB3
            {
              nb_bit_set = 0;
              for (int i = 0; i < 4; i++)  // Check bitmap (T0 or TDi)
              {
                if (eventData->registry.reg_data[bwi_idx] & (0x10 << i)) {
                  nb_bit_set++;
                }
              }

              // Check that there is a bitmap for next level until level 3, i.e
              // there is a TDi
              if (eventData->registry.reg_data[bwi_idx] & 0x80) {
                level++;
              } else
                // No next level(TDi, i={1, 2}), exit
                break;

              bwi_idx += nb_bit_set;
            }

            if (level == 3)  // Level 3 reached
            {
              nb_bit_set = 0;
              // Check if TA3 is here, is so, go next byte
              if (eventData->registry.reg_data[bwi_idx] & 0x10) {
                nb_bit_set++;
              }
              // TB3 here? BWI is in there. Update mBwi value.
              if (eventData->registry.reg_data[bwi_idx] & 0x20) {
                nb_bit_set++;
                bwi_idx += nb_bit_set;
                sSecElem.mBwi =
                    (eventData->registry.reg_data[bwi_idx] & 0xF0) >> 4;
              }
            }
          }
          /* NCI 2.0 - Begin */
        } else  // rel12 APDU gate
        {
          if (eventData->registry.status == NFC_STATUS_OK) {
            sSecElem.mTxWaitingTime = eventData->registry.reg_data[0];
          } else {
            sSecElem.mTxWaitingTime =
                1000;  // ms, default value in server APDU gate registry
          }
        }
        /* NCI 2.0 - End */

        // Now notify function of ATR reception
        SyncEventGuard guard(sSecElem.mApduReadAtrEvent);
        sSecElem.mApduReadAtrEvent.notifyOne();
      }
      break;

    case NFA_HCI_EVENT_RCVD_EVT:
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_EVENT_RCVD_EVT; code: 0x%X; pipe: 0x%X; data len: %u",
          fn, eventData->rcvd_evt.evt_code, eventData->rcvd_evt.pipe,
          eventData->rcvd_evt.evt_len);
      if (eventData->rcvd_evt.evt_code == NFA_HCI_EVT_POST_DATA) {
        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: NFA_HCI_EVENT_RCVD_EVT; NFA_HCI_EVT_POST_DATA", fn);
      } else if (eventData->rcvd_evt.evt_code ==
                 NFA_HCI_EVT_HOT_PLUG)  // If HOT_PLUG
      {
        sSecElem.mSeActivationBitmap |= sSecElem.HOT_PLUG_MASK;

        {
          uint8_t inhibited = 0;
          uint8_t seId;

          // HCI_REL13
          if (eventData->rcvd_evt.evt_len == 2) {
            if (eventData->rcvd_evt.p_evt_buf[1] == 0x02) {
              inhibited = 1;
            }
            seId = eventData->rcvd_evt.p_evt_buf[0];
          } else {
            inhibited = eventData->rcvd_evt.p_evt_buf[0] &
                        sSecElem.HOT_PLUG_INFO_INHIBITED_MASK;
            seId =
                ((eventData->rcvd_evt.p_evt_buf[0] & 0x1) == 0) ? 0x02 : 0xC0;
          }

          if (inhibited) {
            sSecElem.mSeActivationBitmap |= sSecElem.SE_INHIBITED_MASK;
          }

          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: NFA_HCI_EVENT_RCVD_EVT; (NFA_HCI_EVT_HOT_PLUG: seId = "
              "0x%02X, inhibited = %d);",
              fn, seId, inhibited);
        }
        /* NCI 2.0 - End */

        SyncEventGuard guard(sSecElem.mHotPlugEvent);
        sSecElem.mHotPlugEvent.notifyOne();

      } else if ((eventData->rcvd_evt.evt_code == EVT_WTX_REQUEST) &&
                 (eventData->rcvd_evt.pipe == sSecElem.mEseApduPipeId)) {
        // Received EVT_WTR_REQUEST
        LOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: NFA_HCI_EVENT_RCVD_EVT; EVT_WTX_REQUEST", fn);
        sSecElem.mRxEvtType = EvtWtxRequest;
        SyncEventGuard guard(sSecElem.mTransceiveEvent);
        sSecElem.mTransceiveEvent.notifyOne();
      } else if ((eventData->rcvd_evt.pipe == sSecElem.mEseApduPipeId) &&
                 (eventData->rcvd_evt.evt_code == EVT_TRANSMIT_DATA)) {
        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: NFA_HCI_EVENT_RCVD_EVT; data from APDU pipe", fn);
        SyncEventGuard guard(sSecElem.mTransceiveEvent);
        sSecElem.mActualResponseSize =
            (eventData->rcvd_evt.evt_len > MAX_RESPONSE_SIZE)
                ? MAX_RESPONSE_SIZE
                : eventData->rcvd_evt.evt_len;
        sSecElem.mRxEvtType = EvtTransmitData;

        for (int i = 0; i < eventData->rcvd_evt.evt_len; i++) {
          sSecElem.mResponseData[i] = eventData->rcvd_evt.p_evt_buf[i];
        }

        sSecElem.mTransceiveEvent.notifyOne();
      }
      /* NCI 2.0 - Begin */
      else if ((eventData->rcvd_evt.pipe == sSecElem.mEseApduPipeId) &&
               (eventData->rcvd_evt.evt_code == EVT_ATR)) {
        LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: NFA_HCI_EVENT_RCVD_EVT; ATR from APDU pipe (rel12)", fn);

        sSecElem.mAtrInfo.length = eventData->rcvd_evt.evt_len;

        for (int g = 0; g < eventData->rcvd_evt.evt_len; g++) {
          LOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: eventData->rcvd_evt.p_evt_buf[%d] = 0x%x",
                              fn, g, eventData->rcvd_evt.p_evt_buf[g]);
          sSecElem.mAtrInfo.data[g] = eventData->rcvd_evt.p_evt_buf[g];
        }
      }
      /* NCI 2.0 - End */
      break;

    case NFA_HCI_HOST_LIST_EVT:  // HOST_LIST answer
    {
      tNFA_HCI_HOST_LIST& hosts = eventData->hosts;
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_HOST_LIST_EVT; status=0x%X; Number of hosts=0x%X", fn,
          hosts.status, hosts.num_hosts);

      if (hosts.status == NFA_STATUS_OK) {
        sSecElem.mSeInfo.info[UICC_IDX].connected = false;
        sSecElem.mSeInfo.info[ESE_IDX].connected = false;

        for (int i = 0; i < hosts.num_hosts; i++) {
          DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
              "%s: NFA_HCI_HOST_LIST_EVT; host 0x%x in HOST_LIST", fn,
              hosts.host[i]);

          if (hosts.host[i] == UICC_ID) {
            sSecElem.mSeInfo.info[UICC_IDX].connected = true;
            DLOG_IF(INFO, nfc_debug_enabled)
                << StringPrintf("%s: NFA_HCI_HOST_LIST_EVT; connected: 0x%x",
                                fn, sSecElem.mSeInfo.info[UICC_IDX].nfceeId);
          } else if (hosts.host[i] == ESE_ID) {
            sSecElem.mSeInfo.info[ESE_IDX].connected = true;
            DLOG_IF(INFO, nfc_debug_enabled)
                << StringPrintf("%s: NFA_HCI_HOST_LIST_EVT; connected: 0x%x",
                                fn, sSecElem.mSeInfo.info[ESE_IDX].nfceeId);
          }
        }
      }

      SyncEventGuard guard(sSecElem.mHostListEvent);
      sSecElem.mHostListEvent.notifyOne();
      break;
    }
    case NFA_HCI_DELETE_PIPE_EVT:  // NOTIFY_PIPE_DELETED_CMD or
                                   // NOTIFY_ALL_PIPE_CLEARED_CMD
    {
      tNFA_HCI_DELETE_PIPE& deleted = eventData->deleted;
      LOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: NFA_HCI_DELETE_PIPE_EVT; status=0x%X; Identity of deleted "
          "pipe "
          "= 0x%x",
          fn, deleted.status, deleted.pipe);

      if (deleted.pipe == sSecElem.mEseApduPipeId) {
        sSecElem.mAdpuPipeInfo.created = false;
        sSecElem.mAdpuPipeInfo.opened = false;
      }
      break;
    }

    case NFA_HCI_ADD_STATIC_PIPE_EVT:
    case NFA_HCI_EVENT_SENT_EVT:
      break;

    default:
      LOG(ERROR) << StringPrintf("%s: unknown event code=0x%X ????", fn, event);
      break;
  }
}

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
tNFA_EE_INFO* StSecureElement::findEeByHandle(tNFA_HANDLE eeHandle) {
  for (uint8_t xx = 0; xx < mActualNumEe; xx++) {
    if (mEeInfo[xx].ee_handle == eeHandle) return (&mEeInfo[xx]);
  }
  return (NULL);
}

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
const char* StSecureElement::eeStatusToString(uint8_t status) {
  switch (status) {
    case NFC_NFCEE_STATUS_ACTIVE:
      return ("Connected/Active");
    case NFC_NFCEE_STATUS_INACTIVE:
      return ("Connected/Inactive");
    /* NCI 2.0 - Begin */
    case NFC_NFCEE_STATUS_UNRESPONSIVE:
      return ("Unresponsive");
      /* NCI 2.0 - End */
  }
  return ("?? Unknown ??");
}

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
void StSecureElement::connectionEventHandler(
    uint8_t event, tNFA_CONN_EVT_DATA* /*eventData*/) {
  switch (event) {
    case NFA_CE_UICC_LISTEN_CONFIGURED_EVT: {
      SyncEventGuard guard(mUiccListenEvent);
      mUiccListenEvent.notifyOne();
    } break;
  }
}

/*******************************************************************************
**
** Function:        isBusy
**
** Description:     Whether controller is routing listen-mode events to
**                  secure elements or a pipe is connected.
**
** Returns:         True if either case is true.
**
*******************************************************************************/
bool StSecureElement::isBusy() {
  bool retval = mIsPiping;
  LOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("SecureElement::isBusy: %u", retval);
  return retval;
}

/*******************************************************************************
**
** Function:        SeActivationLock
**
** Description:     Whether controller is routing listen-mode events to
**                  secure elements or a pipe is connected.
**
** Returns:         True if either case is true.
**
*******************************************************************************/
void StSecureElement::SeActivationLock() {
  static const char fn[] = "StSecureElement::SeActivationLock";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  gMutex.lock();
}

/*******************************************************************************
**
** Function:        SeActivationLock
**
** Description:     Whether controller is routing listen-mode events to
**                  secure elements or a pipe is connected.
**
** Returns:         True if either case is true.
**
*******************************************************************************/
void StSecureElement::SeActivationUnlock() {
  static const char fn[] = "StSecureElement::SeActivationUnlock";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  gMutex.unlock();
}

/*******************************************************************************
**
** Function:        EnableDisableSE
**
** Description:     Turn on the secure element.
**                  seID: ID of secure element; 0xF3 or 0xF4.
**                  enable : enable (0x1) or disable (0x0) the se.
**
** Returns:         True if ok.
**
*******************************************************************************/
/* NCI 2.0 - Begin */
bool StSecureElement::EnableSE(int seID, bool enable)
/* NCI 2.0 - End */
{
  static const char fn[] = "StSecureElement::EnableSE";
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  int index;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: enter; seID=0x%X enable = %d", fn, seID, enable);
  SeActivationLock();
  /* NCI 2.0 - Begin */
  for (index = 0; index < mActualNumEe; index++)
  /* NCI 2.0 - End */
  {
    tNFA_EE_INFO& eeItem = mEeInfo[index];

    /* NCI 2.0 - Begin */
    DLOG_IF(INFO, nfc_debug_enabled)
        << StringPrintf("%s: handle h=0x%X  id = 0x%X", fn,
                        (eeItem.ee_handle & ~NFA_HANDLE_GROUP_EE), seID);
    /* NCI 2.0 - End */

    /* NCI 2.0 - Begin */
    if (((eeItem.ee_handle & ~NFA_HANDLE_GROUP_EE) == seID)) {
      /* NCI 2.0 - Begin */
      if (eeItem.ee_status == NFA_EE_STATUS_UNRESPONSIVE) {
        nfaStat = NFA_STATUS_FAILED;
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: seId 0x%X is unresponsive (not connected)", fn,
                            (eeItem.ee_handle & ~NFA_HANDLE_GROUP_EE));
        break;
      }
      /* NCI 2.0 - End */

      if (((enable == true) && (eeItem.ee_status != NFA_EE_STATUS_ACTIVE)) ||
          ((enable == false) && (eeItem.ee_status == NFA_EE_STATUS_ACTIVE))) {
        /* NCI 2.0 - Begin */
        nfaStat = handleEnableSESeq(&mEeInfo[index], enable);
        /* NCI 2.0 - End */
        break;
        // getEeInfo();
      } else {
        nfaStat = NFA_STATUS_OK;
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: seId 0x%X already in the desired state", fn,
                            (eeItem.ee_handle & ~NFA_HANDLE_GROUP_EE));
        break;
      }
    }
  }

  getEeInfo();

  // mEEInfo has been updated, so index might have changed.
  for (index = 0; index < mActualNumEe; index++) {
    if (((mEeInfo[index].ee_handle & ~NFA_HANDLE_GROUP_EE) == seID)) break;
  }
  if (index == mActualNumEe) {
    nfaStat = NFA_STATUS_FAILED;
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: seId 0x%X is removed or unknown, return failure", fn, seID);
  } else {
    if ((mEeInfo[index].ee_status == NFA_EE_STATUS_ACTIVE) &&
        (enable == true) && (nfaStat == NFA_STATUS_OK)) {
      NfcStExtensions::getInstance().getPipesInfo();
    }
  }
  /* NCI 2.0 - End */
  SeActivationUnlock();
  /* NCI 2.0 - Begin */
  return (nfaStat == NFA_STATUS_OK ? true : false);
  /* NCI 2.0 - End */
}

/* NCI 2.0 - Begin */
/*******************************************************************************
**
** Function:        handleEnableSESeq
**
** Description:     Turn on the secure element.
**                  seID: ID of secure element; 0xF3 or 0xF4.
**                  enable : enable (0x1) or disable (0x0) the se.
**
** Returns:         True if ok.
**
*******************************************************************************/
tNFA_STATUS StSecureElement::handleEnableSESeq(tNFA_EE_INFO* eeItem,
                                               bool status) {
  static const char fn[] = "StSecureElement::handleEnableSESeq";
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: enter; ee_handle = 0x%X enable = %d", fn, eeItem->ee_handle, status);

  if (status == true) {
    // in case of SWP switch, make sure we are not activating both sides
    int requestSE = eeItem->ee_handle & ~NFA_HANDLE_GROUP_EE;
    if (requestSE == 0x82 || requestSE == 0x84 || requestSE == 0x86) {
      for (int index = 0; index < mActualNumEe; index++) {
        tNFA_EE_INFO& eeOtherItem = mEeInfo[index];
        int otherSE = eeOtherItem.ee_handle & ~NFA_HANDLE_GROUP_EE;
        if ((eeOtherItem.ee_status == NFA_EE_STATUS_ACTIVE) &&
            (otherSE == 0x82 || otherSE == 0x84 || otherSE == 0x86)) {
          LOG(ERROR) << StringPrintf(
              "%s: Tried to activate %x while %x already active", fn, requestSE,
              otherSE);
          return NFA_STATUS_FAILED;
        }
      }
    }
  }

  mEeModeSet = status;
  mbNewEE = true;

  mSeActivationBitmap = 0;

  {  // Wait for NFCEE_MODE_SET_RSP
    SyncEventGuard guard(mEeSetModeEvent);
    /* NCI 2.0 - Begin */
    DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
        "%s: Waiting for NFCEE_MODE_SET_RSP; h=0x%X", fn, eeItem->ee_handle);
    /* NCI 2.0 - End */
    if ((nfaStat = NFA_EeModeSet(eeItem->ee_handle, status)) == NFA_STATUS_OK) {
      mEeSetModeEvent.wait();  // wait for NFA_EE_MODE_SET_EVT
    } else {
      LOG(ERROR) << StringPrintf("%s: NFA_EeModeSet failed; error=0x%X", fn,
                                 nfaStat);
      return NFA_STATUS_FAILED;
    }
  }

  if ((nfaStat == NFA_STATUS_OK) &&
      ((eeItem->ee_status == NFC_NFCEE_STATUS_ACTIVE) ||
       (eeItem->ee_status == NFC_NFCEE_STATUS_INACTIVE))) {
    {
      SyncEventGuard guard2(mHotPlugEvent);
      /* NCI 2.0 - Begin */
      DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
          "%s: Waiting for HCI HOT_PLUG; h=0x%X", fn, eeItem->ee_handle);
      /* NCI 2.0 - End */
      if (mHotPlugEvent.wait(HOT_PLUG_TIMER) == false)  // if timeout occurred
      {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: timeout waiting for HOT PLUG", fn);
      }
    }

    if (mSeActivationBitmap & HOT_PLUG_MASK) {
      SyncEventGuard guard2(mSeActivationEvent);
      if (!(mSeActivationBitmap & SE_INHIBITED_MASK)) {
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: SE is not inhibited, waiting to check it does not clear all "
            "pipes anyway",
            fn);

        if (mSeActivationEvent.wait(300) == true)  // was signaled
        {
          if (mSeActivationBitmap & SE_INITIALIZING) {
            mSeActivationBitmap |= SE_INHIBITED_MASK;
            DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
                "%s: SE is initializing, deal with it like an initial "
                "activation",
                fn);
          }
        }
      }

      if (mSeActivationBitmap & SE_INHIBITED_MASK) {
        /* NCI 2.0 - Begin */
        DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
            "%s: SE is inhibited, waiting for pipe creations; h=0x%X", fn,
            eeItem->ee_handle);
        /* NCI 2.0 - End */
        if (mSeActivationEvent.wait(SE_ACTIVATION_TIMER) ==
            false)  // if timeout occurred
        {
          DLOG_IF(INFO, nfc_debug_enabled)
              << StringPrintf("%s: timeout waiting for SE activation", fn);
          if (mSeActivationBitmap & SE_INITIALIZING) {
            mSeActivationBitmap &= ~SE_INITIALIZING;
            nfaStat = NFA_STATUS_FAILED;
          }
        } else {
          if (mSeActivationBitmap & SE_INITIALIZING) {
            // previous wait was interrupted by the "NFCEE Initialization
            // sequence started" ntf
            DLOG_IF(INFO, nfc_debug_enabled)
                << StringPrintf("%s: Activation started, wait more; h=0x%X", fn,
                                eeItem->ee_handle);
            if (mSeActivationEvent.wait(SE_ACTIVATION_TIMER) ==
                false)  // if timeout occurred
            {
              DLOG_IF(INFO, nfc_debug_enabled)
                  << StringPrintf("%s: timeout waiting for SE activation", fn);
              mSeActivationBitmap &= ~SE_INITIALIZING;
              nfaStat = NFA_STATUS_FAILED;
            } else if (mSeActivationBitmap & SE_INITIALIZING) {
              DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
                  "%s: still in initializing state, but have been triggered",
                  fn);
              mSeActivationBitmap &= ~SE_INITIALIZING;
            }
          }
        }
      }
    } else {
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: No hot plug received", fn);
    }
  }

  // getEeInfo();

  if ((nfaStat == NFA_STATUS_OK) &&
      (eeItem->ee_status == NFC_NFCEE_STATUS_UNRESPONSIVE)) {
    nfaStat = NFA_STATUS_FAILED;
  }

  return nfaStat;
}

/*******************************************************************************
 **
 ** Function:        endOfApduTransfer
 **
 ** Description:     Receive connection-related events from stack.
 **                  connEvent: Event code.
 **                  eventData: Event data.
 **
 ** Returns:         None
 **
 *******************************************************************************/
void StSecureElement::endOfApduTransfer() {
  static const char fn[] = "StSecureElement::endOfApduTransfer";
  LOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  {
    // Send NFCEE_POWER_AND_LINK_CNTRL_CMD to ensure NFCC won't put eSE to sleep
    SyncEventGuard guard(mPowerCtrlEvent);
    /* NCI 2.0 - Begin */
    uint8_t nfceeId = getSENfceeId(ESE_ID);

    nfaStat = NFA_EePowerCtrl(nfceeId, 0x00);
    /* NCI 2.0 - End */
    // mPowerCtrlEvent.wait();
    if (mPowerCtrlEvent.wait(500) == false)  // wait for NFA_EE_MODE_SET_EVT
    {
      LOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: timeout waiting for NFA_EE_POWER_CTRL_EVT", fn);
    }
  }
  // Sent EVT_END_OF_APDU_TRANSFER to eSE => no answer awaited
}

/*******************************************************************************
 **
 ** Function:        retrieveHostList
 **
 ** Description:    retrieveHostLists from stack.
 **                  connEvent: Event code.
 **                  eventData: Event data.
 **
 ** Returns:         None
 **
 *******************************************************************************/
int StSecureElement::retrieveHostList(uint8_t* ptrHostList, uint8_t* ptrInfo) {
  static const char fn[] = "StSecureElement::retrieveHostList()";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);
  int i, idx = 0;
  uint8_t lActualNumEe = MAX_NUM_EE;

  // getEeInfo();
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;
  gMutexEE.lock();
  if ((nfaStat = NFA_EeGetInfo(&lActualNumEe, mEeInfo)) != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: fail get info; error=0x%X", fn, nfaStat);
    gMutexEE.unlock();
  }
  gMutexEE.unlock();
  for (i = 0; i < MAX_NUM_EE; i++) {
    if (mEeInfo[i].ee_handle & 0x80) {
      if ((mEeInfo[i].ee_handle == ESE_HANDLE1) ||
          (mEeInfo[i].ee_handle == ESE_HANDLE2)) {
        DLOG_IF(INFO, nfc_debug_enabled)
            << StringPrintf("%s: eSE id =  0x%X", fn,
                            mEeInfo[i].ee_handle & ~NFA_HANDLE_GROUP_EE);
        //        mESEHandle = mEeInfo[i].ee_handle;
      }
      ptrHostList[idx] = mEeInfo[i].ee_handle & ~NFA_HANDLE_GROUP_EE;
      ptrInfo[idx] = mEeInfo[i].ee_status;
      idx++;
    }
  }
  return idx;
}

/*******************************************************************************
 **
 ** Function:       getActiveNfcee
 **
 ** Description:    Gets the default NFCEE Id as defined in conf file
 **                 and checks if one in the family (UICC/eSE) is active
 **                 If one is foudn, return this value to be used for
 **                 routing.
 **                 NOTE: only one UICC/eSE active at the same time
 **                 If none active, let CLF use its default routing.
 **
 ** Returns:        Id of active NFCEE Id in family (UICC/eSE)
 **
 *******************************************************************************/
uint8_t StSecureElement::getActiveNfcee(uint8_t defaultNfceeId) {
  static const char fn[] = "StSecureElement::getActiveNfcee()";
  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: checking id 0x%02X", fn, defaultNfceeId);
  uint8_t i, nfceeId = 0x00, mask = 0x80;

  if (defaultNfceeId == 0x00) {
    return nfceeId;
  }

  // getEeInfo();
  tNFA_STATUS nfaStat = NFA_STATUS_FAILED;

  mActualNumEe = MAX_NUM_EE;

  if ((nfaStat = NFA_EeGetInfo(&mActualNumEe, mEeInfo)) != NFA_STATUS_OK) {
    LOG(ERROR) << StringPrintf("%s: fail get info; error=0x%X", fn, nfaStat);
    mActualNumEe = 0;
  }

  switch (defaultNfceeId) {
    case 0x81:
    case 0x83:
    case 0x85:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Looking for active UICC", fn);
      break;

    case 0x82:
    case 0x84:
    case 0x86:
      DLOG_IF(INFO, nfc_debug_enabled)
          << StringPrintf("%s: Looking for active eSE", fn);
      mask = 0x81;
      break;

    default:
      LOG(ERROR) << StringPrintf("%s: Unknown SE, exiting", fn);
      return nfceeId;
  }

  for (i = 0; i < mActualNumEe; i++) {
    if ((mEeInfo[i].ee_handle & mask) == mask) {
      if (mEeInfo[i].ee_status == NFA_EE_STATUS_ACTIVE) {
        nfceeId = mEeInfo[i].ee_handle & ~NFA_HANDLE_GROUP_EE;
        break;
      }
    }
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: Found active Nfcee id =  0x%02X", fn, nfceeId);

  return nfceeId;
}

/*******************************************************************************
 **
 ** Function:        resetEEInfo
 **
 ** Description:    Reset EeInfo and check the updated info from stack.
 **
 ** Returns:         None
 **
 *******************************************************************************/
void StSecureElement::resetEEInfo() {
  static const char fn[] = "StSecureElement::resetEEInfo";
  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf("%s: enter", fn);

  mbNewEE = true;
}

/*******************************************************************************
 **
 ** Function:        getSENfceeId
 **
 ** Description:     Get the NFCEE ID of a HCI host (0x02 or 0xc0)
 **
 ** Returns:         None
 **
 *******************************************************************************/
uint8_t StSecureElement::getSENfceeId(uint8_t host_id) {
  static const char fn[] = "StSecureElement::getSENfceeId";
  uint8_t nfceeId = 0xff;

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: host_id: 0x%02X", fn, host_id);

  if (host_id == UICC_ID) {
    nfceeId = mSeInfo.info[UICC_IDX].nfceeId;
    if (nfceeId == 0x0) {
      // not initialized yet, return default value
      nfceeId = 0x81;
    }
  } else if (host_id == ESE_ID || host_id == ESE_GSMA_ID) {
    nfceeId = mSeInfo.info[ESE_IDX].nfceeId;
    if (nfceeId == 0x0) {
      // not initialized yet, return default value (82 or 86).
      if (getEeInfo()) {
        for (uint8_t xx = 0; xx < mActualNumEe; xx++) {
          switch (mEeInfo[xx].ee_handle & 0xFF) {
            case 0x82:
            case 0x86:
              nfceeId = (mEeInfo[xx].ee_handle & 0xFF);
              break;
          }
        }  // end for
      } else {
        nfceeId = 0x82;
      }
    }
  } else if (host_id == 0) {
    nfceeId = 0;
  }

  DLOG_IF(INFO, nfc_debug_enabled)
      << StringPrintf("%s: host_id: 0x%02X corresponds to nfceeId: 0x%02X", fn,
                      host_id, nfceeId);

  return nfceeId;
}

uint8_t StSecureElement::getConnectedNfceeId(uint8_t id) {
  static const char fn[] = "StSecureElement::getConnectedNfceeId";

  // If DH or unvalid value, use HCE
  uint8_t nciId = 0x00;
  uint8_t i;
  uint8_t nfceeid[3];
  uint8_t conInfo[3];
  uint8_t num = retrieveHostList(nfceeid, conInfo);

  switch (id) {
    case 0x81:
    case 0x83:
    case 0x85:
      for (i = 0; i < num; i++) {
        if ((conInfo[i] == 0) && ((nfceeid[i] & 0x01) != 0)) {
          nciId = nfceeid[i];
          break;
        }
      }
      break;

    case 0x82:
    case 0x84:
    case 0x86:
      for (i = 0; i < num; i++) {
        if ((conInfo[i] == 0) && ((nfceeid[i] & 0x01) == 0)) {
          nciId = nfceeid[i];
          break;
        }
      }
      break;

    default:
      break;
  }

  DLOG_IF(INFO, nfc_debug_enabled) << StringPrintf(
      "%s: requested id: 0x%02X, "
      "corresponding connected nfceeId: 0x%02X",
      fn, id, nciId);

  return (nciId & 0xFF);
}

/* NCI 2.0 - End */
