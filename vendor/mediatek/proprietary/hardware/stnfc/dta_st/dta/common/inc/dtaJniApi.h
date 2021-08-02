/*  Copyright (C) 2018 ST Microelectronics S.A.
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
 *  Device Test Application for ST21NFCB NFC Controller
 *
 ******************************************************************************/

#if !defined(__DTA_JNI_API_H_)
#define __DTA_JNI_API_H_

#if defined(_WIN32) || defined(_WIN64)
#define DTAAPI WINAPI

#if defined(DTA_EXPORT)
#define DTAEXPORT __declspec(dllexport)
#else
#define DTAEXPORT __declspec(dllimport)
#endif
#else

#define DTAAPI
#define DTAEXPORT

#endif

#if defined(__cplusplus)
extern "C" {
#endif

#define DTA_MODE_DP 0
#define DTA_MODE_LLCP 1
#define DTA_MODE_SNEP 2

typedef enum {
  stDtaIdle = 0,  // 0
  stDtaError,     // 1
  stDtaReady,
  stDtaTcRunning,
  stDtaTcStopped,
  stDtaStackStopped,
  stDtaStackRunning,
  stDtaWarning
} TStateDta;

typedef enum tagDTAStatus {
  dtaStatusSuccess = 0x00,
  dtaStatusFailed = 0x01,
  dtaStatusLowLayerNotFound = 0x02,
  dtaStatusAlreadyInitialized = 0x03,
  dtaStatusLowLayerInitFailed = 0x04,
  dtaStatusInvalidHandle = 0x05,
  dtaStatusInvalidParameter = 0x06,
  dtaStatusAuthenticationFailed = 0x08,
  dtaStatusLowerLayerCommunicationFailed = 0x09,
  dtaStatusNotSupported = 0x0A,
  dtaStatusInvalidState = 0x0B,
  dtaStatusNotEnoughMemory = 0x0C,
  dtaStatusTagMismatch = 0x0D,
  dtaStatusWrongOperationMode = 0x0E,
  dtaStatusNotWritable = 0x0F,
  dtaStatusNoTagConnected = 0x10,
  dtaStatusPending = 0x11,
  dtaStatusBusy = 0x12,
  dtaStatusBufferTooSmall = 0x13,
  dtaStatusBufferOverflow = 0x14,
  dtaStatusInternalError = 0x15,
  dtaStatusTimeout = 0x16,
  dtaStatusMoreDataRequired = 0x17,
  dtaStatusListenAlreadyPending = 0x18,
  dtaStatusAccessDenied = 0x19,
  dtaStatusNotFound = 0x1A,
  dtaStatusBadRequest = 0x1B,
  dtaStatusUnsupportedVersion = 0x1C,
  dtaStatusNoResources = 0x1D,
  dtaStatusOpenFailed = 0x1E,
  dtaStatusNotReady = 0x1F,
  dtaStatusErrorDecodingStream = 0x20,
  dtaStatusReactivationPending = 0x21,
  dtaStatusNfccError = 0x22,
  dtaStatusIxitFileNotFound = 0x23,

  dtaStatusRegistrationRejected = 0x40
} DTASTATUS;

typedef struct {
  void *handle;
  unsigned char dtaRunning;
  unsigned char dta_mode;
  unsigned int pattern_nb;
  unsigned char cr_version;
  unsigned char con_poll_A;
  unsigned char con_poll_B;
  unsigned char con_poll_F;
  unsigned char con_poll_V;
  unsigned char con_poll_ACM;
  unsigned char con_listen_dep_A;
  unsigned char con_listen_dep_F;
  unsigned char con_listen_t4Atp;
  unsigned char con_listen_t4Btp;
  unsigned char con_listen_t3tp;
  unsigned char con_listen_acm;
  unsigned char con_bitr_f;
  unsigned char con_bitr_acm;
  unsigned char nfca_uid_gen_mode;
  unsigned char nfca_uid_size;
  unsigned char cdl_A;
  unsigned char cdl_B;
  unsigned char cdl_F;
  unsigned char cdl_V;
  unsigned char t4at_nfcdep_prio;
  unsigned char ext_rf_frame;
  unsigned int miux_mode;
  unsigned char role;
  unsigned char server_type;
  unsigned char request_type;
  unsigned char data_type;
  unsigned char disc_incorrect_len;

} tJNI_DTA_INFO;

typedef void(DTAAPI *PStDtaCallback)(void *context, TStateDta state, char *data,
                                     unsigned int length);

DTAEXPORT DTASTATUS DTAAPI dtaInitialize(void **hHandle, char *initString,
                                         PStDtaCallback callback, void *ctx);
DTAEXPORT DTASTATUS DTAAPI dtaShutdown(void *hHandle);

#if defined(__cplusplus)
}
#endif

#endif  // __DTA_JNI_API_H_
