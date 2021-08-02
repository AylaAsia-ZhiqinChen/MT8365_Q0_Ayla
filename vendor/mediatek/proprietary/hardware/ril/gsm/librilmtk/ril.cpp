/* //device/libs/telephony/ril.cpp
**
** Copyright 2006, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#define LOG_TAG "RILC"

#include <hardware_legacy/power.h>

#include <telephony/mtk_ril.h>
#include <libmtkrilutils.h>
#include <telephony/ril_cdma_sms.h>
#include <cutils/sockets.h>
#include <cutils/jstring.h>
#include <telephony/record_stream.h>
#include <log/log.h>
#include <utils/SystemClock.h>
#include <pthread.h>
#include <binder/ProcessState.h>
#include <binder/Parcel.h>
#include <cutils/jstring.h>
#include <sys/types.h>
#include <sys/limits.h>
#include <sys/system_properties.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <sys/un.h>
#include <assert.h>
#include <netinet/in.h>
#include <cutils/properties.h>
#include <RilSapSocket.h>
#include <mal.h>

#include <rilc.h>

#ifdef HAVE_AEE_FEATURE
#include "aee.h"
#endif

extern "C" void
RIL_onRequestComplete(RIL_Token t, RIL_Errno e, void *response, size_t responselen);

extern "C" void
RIL_onRequestAck(RIL_Token t);
namespace android {


static RIL_SOCKET_ID s_ril_cntx[] = {
    RIL_SOCKET_1
    , RIL_SOCKET_2
    , RIL_SOCKET_3
    , RIL_SOCKET_4
};

#define PHONE_PROCESS "radio"
#define BLUETOOTH_PROCESS "bluetooth"

#define SOCKET_NAME_RIL "mrild"
#define SOCKET2_NAME_RIL "mrild2"
#define SOCKET3_NAME_RIL "mrild3"
#define SOCKET4_NAME_RIL "mrild4"

#define SOCKET_NAME_RIL_DEBUG "rild-debug"

#define SOCKET_NAME_RIL_OEM "rild-oem"

#define SOCKET_NAME_SAP "msap_uim_socket1"
#define SOCKET2_NAME_SAP "msap_uim_socket2"
#define SOCKET3_NAME_SAP "msap_uim_socket3"
#define SOCKET4_NAME_SAP "msap_uim_socket4"

#define ANDROID_WAKE_LOCK_NAME "radio-interface-gsm"

#define ANDROID_WAKE_LOCK_SECS 0
#define ANDROID_WAKE_LOCK_USECS 200000

#define PROPERTY_RIL_IMPL "gsm.version.ril-impl"

// match with constant in RIL.java
#define MAX_COMMAND_BYTES (20 * 1024)

// Basically: memset buffers that the client library
// shouldn't be using anymore in an attempt to find
// memory usage issues sooner.
#define MEMSET_FREED 1

#define NUM_ELEMS(a)     (sizeof (a) / sizeof (a)[0])

#define MIN(a,b) ((a)<(b) ? (a) : (b))

/* Constants for response types */
#define RESPONSE_SOLICITED 0
#define RESPONSE_UNSOLICITED 1
#define RESPONSE_SOLICITED_ACK 2
#define RESPONSE_SOLICITED_ACK_EXP 3
#define RESPONSE_UNSOLICITED_ACK_EXP 4

/* Negative values for private RIL errno's */
#define RIL_ERRNO_INVALID_RESPONSE (-1)
#define RIL_ERRNO_NO_MEMORY (-12)

// request, response, and unsolicited msg print macro
#define PRINTBUF_SIZE 8096

// Enable verbose logging
#define VDBG 0

// Enable RILC log
#define RILC_LOG 0

#if RILC_LOG
    #define startRequest           sprintf(printBuf, "(")
    #define closeRequest           sprintf(printBuf, "%s)", printBuf)
    #define printRequest(token, req)           \
            RLOGD("[%04d]> %s %s", token, requestToString(req), printBuf)

    #define startResponse           sprintf(printBuf, "%s {", printBuf)
    #define closeResponse           sprintf(printBuf, "%s}", printBuf)
    #define printResponse           RLOGD("%s", printBuf)

    #define clearPrintBuf           printBuf[0] = 0
    #define removeLastChar          printBuf[strlen(printBuf)-1] = 0
    #define appendPrintBuf(x...)    snprintf(printBuf, PRINTBUF_SIZE, x)
#else
    #define startRequest
    #define closeRequest
    #define printRequest(token, req)
    #define startResponse
    #define closeResponse
    #define printResponse
    #define clearPrintBuf
    #define removeLastChar
    #define appendPrintBuf(x...)
#endif

/* move to rilc.h
enum WakeType {DONT_WAKE, WAKE_PARTIAL};

typedef struct {
    int requestNumber;
    void (*dispatchFunction) (Parcel &p, struct RequestInfo *pRI);
    int(*responseFunction) (Parcel &p, void *response, size_t responselen);
} CommandInfo;

typedef struct {
    int requestNumber;
    int (*responseFunction) (Parcel &p, void *response, size_t responselen);
    WakeType wakeType;
} UnsolResponseInfo;

typedef struct RequestInfo {
    int32_t token;      //this is not RIL_Token
    CommandInfo *pCI;
    struct RequestInfo *p_next;
    char cancelled;
    char local;         // responses to local commands do not go back to command process
    RIL_SOCKET_ID socket_id;
    int wasAckSent;    // Indicates whether an ack was sent earlier
} RequestInfo;

typedef struct UserCallbackInfo {
    RIL_TimedCallback p_callback;
    void *userParam;
    struct ril_event event;
    struct UserCallbackInfo *p_next;
} UserCallbackInfo;

extern "C" const char * requestToString(int request);
extern "C" const char * failCauseToString(RIL_Errno);
extern "C" const char * callStateToString(RIL_CallState);
extern "C" const char * radioStateToString(RIL_RadioState);
extern "C" const char * rilSocketIdToString(RIL_SOCKET_ID socket_id);

*/

extern "C"
char rild[MAX_SERVICE_NAME_LENGTH] = SOCKET_NAME_RIL;
/*******************************************************************/

RIL_RadioFunctions s_callbacks = {0, NULL, NULL, NULL, NULL, NULL, NULL};
static int s_registerCalled = 0;

static pthread_t s_tid_dispatch;
static pthread_t s_tid_reader;
static int s_started = 0;

static int s_fdDebug = -1;
static int s_fdDebug_socket2 = -1;

static int s_fdOem = -1;
static int s_fdOem_command = -1;
static int s_EAPSIMAKA_fd = -1;
static int s_SIMLOCK_fd = -1;
static int s_THERMAL_fd = -1;

// External SIM [Start]
static pthread_t s_vsim_dispatch;
static struct ril_event s_vsim_commands_event[SIM_COUNT];
static struct ril_event s_vsim_listen_event[SIM_COUNT];
static SocketListenParam s_ril_vsim_param_socket[SIM_COUNT];
static int s_fdVsim[SIM_COUNT] = {0};
static int s_fdVsim_command[SIM_COUNT] = {0};
pthread_mutex_t s_vsim_lock_mutex;
pthread_mutex_t s_pendingVsimUrcMutex[MAX_SIM_COUNT] = {PTHREAD_MUTEX_INITIALIZER};
static pthread_mutex_t s_writeVsimMutex[SIM_COUNT] = {PTHREAD_MUTEX_INITIALIZER};
extern void cacheVsimUrc(int unsolResponse, const void *data, size_t datalen, RIL_SOCKET_ID id);
extern void sendPendedVsimUrcs(RIL_SOCKET_ID socket_id, int fdCommand);
// External SIM [End]

static int s_fdWakeupRead;
static int s_fdWakeupWrite;

int s_wakelock_count = 0;

static struct ril_event s_wakeupfd_event;

static struct ril_event s_commands_event[SIM_COUNT];
static struct ril_event s_listen_event[SIM_COUNT];
static SocketListenParam s_ril_param_socket[SIM_COUNT];

static pthread_mutex_t s_last_wake_mutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t s_pendingRequestsMutex[SIM_COUNT] = {PTHREAD_MUTEX_INITIALIZER};
static pthread_mutex_t s_writeMutex[SIM_COUNT] = {PTHREAD_MUTEX_INITIALIZER};
static pthread_mutex_t s_wakeLockCountMutex = PTHREAD_MUTEX_INITIALIZER;
static RequestInfo *s_pendingRequests[SIM_COUNT] = {NULL};

static struct ril_event s_wake_timeout_event;
static struct ril_event s_debug_event;
static struct ril_event s_oem_event;

static const struct timeval TIMEVAL_WAKE_TIMEOUT = {ANDROID_WAKE_LOCK_SECS,ANDROID_WAKE_LOCK_USECS};

static pthread_mutex_t s_startupMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t s_startupCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t s_dispatchMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t s_dispatchCond = PTHREAD_COND_INITIALIZER;

static RequestInfo *s_toDispatchHead = NULL;
static RequestInfo *s_toDispatchTail = NULL;

static UserCallbackInfo *s_last_wake_timeout_info = NULL;

static void *s_lastNITZTimeData = NULL;
static size_t s_lastNITZTimeDataSize;

pthread_mutex_t s_pendingUrcMutex[MAX_SIM_COUNT];
int s_fdCommand[MAX_SIM_COUNT];

#if RILC_LOG
    static char printBuf[PRINTBUF_SIZE];
#endif

/*******************************************************************/
static int sendResponse (Parcel &p, RIL_SOCKET_ID socket_id);

static void dispatchVoid (Parcel& p, RequestInfo *pRI);
static void dispatchString (Parcel& p, RequestInfo *pRI);
static void dispatchStrings (Parcel& p, RequestInfo *pRI);
static void dispatchInts (Parcel& p, RequestInfo *pRI);
static void dispatchDial (Parcel& p, RequestInfo *pRI);
static void dispatchEmergencyDial (Parcel& p, RequestInfo *pRI);
static void dispatchRedial (Parcel &p, RequestInfo *pRI);
static void dispatchSIM_IO (Parcel& p, RequestInfo *pRI);
static void dispatchSIM_APDU (Parcel& p, RequestInfo *pRI);
static void dispatchCallForward(Parcel& p, RequestInfo *pRI);
static void dispatchCallForwardEx(Parcel& p, RequestInfo *pRI);
static void dispatchRaw(Parcel& p, RequestInfo *pRI);
static void dispatchSmsWrite (Parcel &p, RequestInfo *pRI);
static void dispatchDataCall (Parcel& p, RequestInfo *pRI);
static void dispatchSetInitialAttachApn (Parcel& p, RequestInfo *pRI);
static void dispatchCdmaSms(Parcel &p, RequestInfo *pRI);
static void dispatchImsSms(Parcel &p, RequestInfo *pRI);
static void dispatchImsCdmaSms(Parcel &p, RequestInfo *pRI, uint8_t retry, int32_t messageRef);
static void dispatchImsGsmSms(Parcel &p, RequestInfo *pRI, uint8_t retry, int32_t messageRef);
static void dispatchCdmaSmsAck(Parcel &p, RequestInfo *pRI);
static void dispatchGsmBrSmsCnf(Parcel &p, RequestInfo *pRI);
static void dispatchCdmaBrSmsCnf(Parcel &p, RequestInfo *pRI);
static void dispatchRilCdmaSmsWriteArgs(Parcel &p, RequestInfo *pRI);
static void dispatchNVReadItem(Parcel &p, RequestInfo *pRI);
static void dispatchNVWriteItem(Parcel &p, RequestInfo *pRI);
static void dispatchUiccSubscripton(Parcel &p, RequestInfo *pRI);
static void dispatchSimAuthentication(Parcel &p, RequestInfo *pRI);
static void dispatchDataProfile(Parcel &p, RequestInfo *pRI);
static void dispatchRadioCapability(Parcel &p, RequestInfo *pRI);
static void dispatchCarrierRestrictions(Parcel &p, RequestInfo *pRI);
static void dispatchFdMode(Parcel &p, RequestInfo *pRI);
static void dispatchNetworkScan(Parcel &p, RequestInfo *pRI);
static int responseInts(Parcel &p, void *response, size_t responselen);
static int responseFailCause(Parcel &p, void *response, size_t responselen);
static int responseStrings(Parcel &p, void *response, size_t responselen);
static int responseString(Parcel &p, void *response, size_t responselen);
static int responseVoid(Parcel &p, void *response, size_t responselen);
static int responseCallList(Parcel &p, void *response, size_t responselen);
static int responseSMS(Parcel &p, void *response, size_t responselen);
static int responseSIM_IO(Parcel &p, void *response, size_t responselen);
static int responseCallForwards(Parcel &p, void *response, size_t responselen);
static int responseCallForwardsEx(Parcel &p, void *response, size_t responselen);
static int responseDataCallList(Parcel &p, void *response, size_t responselen);
static int responseSetupDataCall(Parcel &p, void *response, size_t responselen);
static int responseRaw(Parcel &p, void *response, size_t responselen);
static int responseSsn(Parcel &p, void *response, size_t responselen);
static int responseSimStatus(Parcel &p, void *response, size_t responselen);
static int responseGsmBrSmsCnf(Parcel &p, void *response, size_t responselen);
static int responseCdmaBrSmsCnf(Parcel &p, void *response, size_t responselen);
static int responseCdmaSms(Parcel &p, void *response, size_t responselen);
static int responseCellList(Parcel &p, void *response, size_t responselen);
static int responseCdmaInformationRecords(Parcel &p,void *response, size_t responselen);
static int responseRilSignalStrength(Parcel &p,void *response, size_t responselen);
static int responseCallRing(Parcel &p, void *response, size_t responselen);
static int responseCdmaSignalInfoRecord(Parcel &p,void *response, size_t responselen);
static int responseCdmaCallWaiting(Parcel &p,void *response, size_t responselen);
static int responseSimRefresh(Parcel &p, void *response, size_t responselen);
static int responseCellInfoList(Parcel &p, void *response, size_t responselen);
static int handleSpecialRequestWithArgs(int argCount, char** args);
static int responseHardwareConfig(Parcel &p, void *response, size_t responselen);
static int responseDcRtInfo(Parcel &p, void *response, size_t responselen);
static int responseRadioCapability(Parcel &p, void *response, size_t responselen);
static int responseSSData(Parcel &p, void *response, size_t responselen);
static int responseLceStatus(Parcel &p, void *response, size_t responselen);
static int responseLceData(Parcel &p, void *response, size_t responselen);
static int responseActivityData(Parcel &p, void *response, size_t responselen);
/// M: [C2K] IRAT feature. @{
static int responseIratStateChange(Parcel &p, void *response, size_t responselen);
/// @}
static int responseCarrierRestrictions(Parcel &p, void *response, size_t responselen);
static int responsePhoneCapability(Parcel &p, void *response, size_t responselen);
static int responsePcoData(Parcel &p, void *response, size_t responselen);
// M: [VzW] Data Framework
static int responsePcoDataAfterAttached(Parcel &p, void *response, size_t responselen);

// SMS-START
static int responseGetSmsSimMemStatusCnf(Parcel &p,void *response, size_t responselen);
static void dispatchSmsParams(Parcel &p, RequestInfo *pRI);
static int responseSmsParams(Parcel &p, void *response, size_t responselen);
static int responseEtwsNotification(Parcel &p, void *response, size_t responselen);
// SMS-END
static int responseNetworkScanResult(Parcel &p, void *response, size_t responselen);

// MTK-START: SIM GBA
static void dispatchSimAuth(Parcel &p, RequestInfo *pRI);
// MTK-END

/// M: CC: Call control CRSS related handling
static int responseCrssN(Parcel &p, void *response, size_t responselen);

// MTK-START: SIM OPEN CHANNEL WITH P2
static void dispatchOpenChannelParams(Parcel &p, RequestInfo *pRI);
// MTK-END

static void grabPartialWakeLock();
void releaseWakeLock();
static void wakeTimeoutCallback(void *);
static bool isServiceTypeCfQuery(RIL_SsServiceType serType, RIL_SsRequestType reqType);
static bool isDebuggable();
/// M: eMBMS feature
static int responseLocalEmbmsEnable(Parcel &p, void *response, size_t responselen);
static int responseLocalEmbmsDisable(Parcel &p, void *response, size_t responselen);
static int responseLocalEmbmsNetworkTime(Parcel &p, void *response, size_t responselen);
static void dispatchLocalEmbmsStartSessionInfo(Parcel &p, RequestInfo *pRI);
static void dispatchLocalEmbmsStopSessionInfo(Parcel &p, RequestInfo *pRI);
static int responseLocalEmbmsSessionInfo(Parcel &p, void *response, size_t responselen);
static int responseLocalEmbmsOosNotify(Parcel &p, void *response, size_t responselen);
static int responseLocalEmbmsSaiNotify(Parcel &p, void *response, size_t responselen);
static int responseLocalEmbmsSessionNotify(Parcel &p, void *response, size_t responselen);
static int responseLocalEmbmsGetCoverageState(Parcel &p, void *response, size_t responselen);
/// M: eMBMS end
// PHB START
static void dispatchPhbEntry(Parcel &p, RequestInfo *pRI);
static void dispatchWritePhbEntryExt(Parcel &p, RequestInfo *pRI);
static int responsePhbEntries(Parcel &p, void *response, size_t responselen);
static int responseGetPhbMemStorage(Parcel &p, void *response, size_t responselen);
static int responseReadPhbEntryExt(Parcel &p, void *response, size_t responselen);
// PHB END

/// M: Ims Data Framework {@
static int responseSetupDedicateDataCall(Parcel &p, void *response, size_t responselen);
/// @}

// External SIM [START]
static void dispatchVsimEvent(Parcel &p, RequestInfo *pRI);
static void dispatchVsimOperationEvent(Parcel &p, RequestInfo *pRI);
static int responseVsimOperationEvent(Parcel &p, void *response, size_t responselen);
// External SIM [END]

static void dispatchLinkCapacityReportingCriteria(Parcel &p, RequestInfo *pRI);
static int responseLinkCapacityEstimate(Parcel &p, void *response, size_t responselen);

#ifdef HAVE_AEE_FEATURE
void triggerWarning(char *pErrMsg);
#endif

#ifdef RIL_SHLIB
#if defined(ANDROID_MULTI_SIM)
extern "C" void RIL_onUnsolicitedResponse(int unsolResponse, const void *data,
                                size_t datalen, RIL_SOCKET_ID socket_id);
#else
extern "C" void RIL_onUnsolicitedResponse(int unsolResponse, const void *data,
                                size_t datalen);
#endif
#endif

#if defined(ANDROID_MULTI_SIM)
#define RIL_UNSOL_RESPONSE(a, b, c, d) RIL_onUnsolicitedResponse((a), (b), (c), (d))
#define CALL_ONREQUEST(a, b, c, d, e) s_callbacks.onRequest((a), (b), (c), (d), (e))
#define CALL_ONSTATEREQUEST(a) s_callbacks.onStateRequest(a)
#else
#define RIL_UNSOL_RESPONSE(a, b, c, d) RIL_onUnsolicitedResponse((a), (b), (c))
#define CALL_ONREQUEST(a, b, c, d, e) s_callbacks.onRequest((a), (b), (c), (d))
#define CALL_ONSTATEREQUEST(a) s_callbacks.onStateRequest()
#endif

#ifdef HAVE_AEE_FEATURE
void triggerWarning(char *pErrMsg);
#endif

static UserCallbackInfo * internalRequestTimedCallback
    (RIL_TimedCallback callback, void *param,
        const struct timeval *relativeTime);

/** Index == requestNumber */
CommandInfo s_commands[] = {
#include "telephony/ril_commands.h"
};

UnsolResponseInfo s_unsolResponses[] = {
#include "telephony/ril_unsol_commands.h"
};

char * RIL_getRilSocketName() {
    return rild;
}

#ifdef MTK_RIL

CommandInfo s_mtk_commands[] = {
#include "telephony/mtk_ril_commands.h"
};

UnsolResponseInfo s_mtk_unsolResponses[] = {
#include "telephony/mtk_ril_unsol_commands.h"
};

CommandInfo s_mtk_local_commands[] = {
#include "mtk_ril_local_commands.h"
};

/// M: ril proxy
static UnsolResponseInfo s_mtk_local_unsolResponses[] = {
#include "mtk_ril_local_urc_commands.h"
};

static UserCallbackInfo * internalRequestProxyTimedCallback
(RIL_TimedCallback callback, void *param,
const struct timeval *relativeTime, int proxyId);

const int s_commands_size = (int32_t)NUM_ELEMS(s_commands);

const int s_mtk_commands_size = (int32_t)NUM_ELEMS(s_mtk_commands);

const int s_mtk_local_commands_size = (int32_t)NUM_ELEMS(s_mtk_local_commands);

const int s_unsol_size = (int32_t)NUM_ELEMS(s_unsolResponses);

const int s_mtk_unsol_size = (int32_t)NUM_ELEMS(s_mtk_unsolResponses);

const int s_mtk_local_unsol_size = (int32_t)NUM_ELEMS(s_mtk_local_unsolResponses);
#endif

extern "C"
void RIL_setRilSocketName(const char * s) {
    strncpy(rild, s, MAX_SERVICE_NAME_LENGTH);
}

static char *
strdupReadString(Parcel &p) {
    size_t stringlen;
    const char16_t *s16;

    s16 = p.readString16Inplace(&stringlen);

    return strndup16to8(s16, stringlen);
}

static status_t
readStringFromParcelInplace(Parcel &p, char *str, size_t maxLen) {
    size_t s16Len;
    const char16_t *s16;

    s16 = p.readString16Inplace(&s16Len);
    if (s16 == NULL) {
        return NO_MEMORY;
    }
    size_t strLen = strnlen16to8(s16, s16Len);
    if ((strLen + 1) > maxLen) {
        return NO_MEMORY;
    }
    if (strncpy16to8(str, s16, strLen) == NULL) {
        return NO_MEMORY;
    } else {
        return NO_ERROR;
    }
}

static void writeStringToParcel(Parcel &p, const char *s) {
    char16_t *s16;
    size_t s16_len;
    s16 = strdup8to16(s, &s16_len);
    p.writeString16(s16, s16_len);
    free(s16);
}


static void
memsetString (char *s) {
    if (s != NULL) {
        memset (s, 0, strlen(s));
    }
}

void   nullParcelReleaseFunction (const uint8_t* data, size_t dataSize,
                                    const size_t* objects, size_t objectsSize,
                                        void* cookie) {
    // do nothing -- the data reference lives longer than the Parcel object
}

/**
 * To be called from dispatch thread
 * Issue a single local request for specified SIM,
 * and sent back up to the command process
 */
extern "C"
void issueLocalRequestForResponse(int request, void *data, int len, RIL_SOCKET_ID socket_id) {
    RequestInfo *pRI;
    int ret, i;
    int isInvalidRequest = 0;
    status_t status;

    /* Hook for current context */
    /* pendingRequestsMutextHook refer to &s_pendingRequestsMutex */
    pthread_mutex_t* pendingRequestsMutexHook = &s_pendingRequestsMutex[socket_id];
    /* pendingRequestsHook refer to &s_pendingRequests */
    RequestInfo**    pendingRequestsHook = &s_pendingRequests[socket_id];


    pRI = (RequestInfo *)calloc(1, sizeof(RequestInfo));
    if (pRI == NULL) {
        RLOGE("OOM");
        return;
    }
    pRI->local = 0;
    pRI->socket_id = socket_id;

#ifdef MTK_RIL
    if (request >= RIL_LOCAL_REQUEST_VENDOR_BASE) {
        for (i = 0; i < s_mtk_local_commands_size; i++) {
            if (request == s_mtk_local_commands[i].requestNumber) {
                pRI->pCI = &(s_mtk_local_commands[i]);
                break;
            }
        }
        if (i == s_mtk_local_commands_size) {
            isInvalidRequest = 1;
        }
    } else if (request >= RIL_REQUEST_VENDOR_BASE) {
        for (i = 0; i < s_mtk_commands_size; i++) {
            if (request == s_mtk_commands[i].requestNumber) {
                pRI->pCI = &(s_mtk_commands[i]);
                break;
            }
        }
        if (i == s_mtk_commands_size) {
            isInvalidRequest = 1;
        }
    } else
#endif /* MTK_RIL */
    if (request >= 1) {
        for (i = 0; i < s_commands_size; i++) {
            if (request == s_commands[i].requestNumber) {
                pRI->pCI = &(s_commands[i]);
                break;
            }
        }
        if (i == s_commands_size) {
            isInvalidRequest = 1;
        }
    } else {
        isInvalidRequest = 1;
    }
    if (isInvalidRequest == 1) {
        LOGE("issueLocalRequestForResponse: unsupported request code %d", request);
        // FIXME this should perhaps return a response
        free(pRI);
        return;
    }
    pRI->cid = RIL_CMD_PROXY_1;
    if (socket_id == RIL_SOCKET_2){
        pRI->cid = RIL_CMD2_PROXY_1;
    } else if (socket_id == RIL_SOCKET_3) {
        pRI->cid = RIL_CMD3_PROXY_1;
    } else if (socket_id == RIL_SOCKET_4) {
        pRI->cid = RIL_CMD4_PROXY_1;
    }
    pRI->exec_cid = pRI->cid;
    ret = pthread_mutex_lock(pendingRequestsMutexHook);
    assert (ret == 0);

    pRI->p_next = *pendingRequestsHook;
    *pendingRequestsHook = pRI;

    ret = pthread_mutex_unlock(pendingRequestsMutexHook);
    assert (ret == 0);

    LOGD("C[locl]> %s", requestToString(request));

#ifdef MTK_RIL
    {
        enqueueLocalRequestResponse(pRI, data, len, NULL, socket_id);
    }
#else
    s_callbacks.onRequest(request, data, len, pRI);
#endif
}

static int
processCommandBuffer(void *buffer, size_t buflen, RIL_SOCKET_ID socket_id) {
    Parcel p;
    status_t status;
    int32_t request;
    int32_t token;
    RequestInfo *pRI;
    int ret;
    /* Hook for current context */
    /* pendingRequestsMutextHook refer to &s_pendingRequestsMutex */
    pthread_mutex_t* pendingRequestsMutexHook = &s_pendingRequestsMutex[socket_id];
    /* pendingRequestsHook refer to &s_pendingRequests */
    RequestInfo**    pendingRequestsHook = &s_pendingRequests[socket_id];

    p.setData((uint8_t *) buffer, buflen);

    // status checked at end
    status = p.readInt32(&request);
    status = p.readInt32 (&token);

    RLOGD("SOCKET %s REQUEST: %s length:%zu", rilSocketIdToString(socket_id),
            requestToString(request), buflen);

    if (status != NO_ERROR) {
        RLOGE("invalid request block");
        return 0;
    }

    // Received an Ack for the previous result sent to RIL.java,
    // so release wakelock and exit
    if (request == RIL_RESPONSE_ACKNOWLEDGEMENT) {
        releaseWakeLock();
        return 0;
    }

    pRI = (RequestInfo *)calloc(1, sizeof(RequestInfo));
    if (pRI == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(request));
        return 0;
    }

    pRI->token = token;
    int isInvalidRequest = 0;
    int i = 0;

#ifdef MTK_RIL
    if (request >= RIL_LOCAL_REQUEST_VENDOR_BASE) {
        for (i = 0; i < s_mtk_local_commands_size; i++) {
            if (request == s_mtk_local_commands[i].requestNumber) {
                pRI->pCI = &(s_mtk_local_commands[i]);
                break;
            }
        }
        if (i == s_mtk_local_commands_size) {
            isInvalidRequest = 1;
        }
    } else if (request >= RIL_REQUEST_VENDOR_BASE) {
        for (i = 0; i < s_mtk_commands_size; i++) {
            if (request == s_mtk_commands[i].requestNumber) {
                pRI->pCI = &(s_mtk_commands[i]);
                break;
            }
        }
        if (i == s_mtk_commands_size) {
            isInvalidRequest = 1;
        }
    } else
#endif /* MTK_RIL */
    if (request >= 1) {
        for (i = 0; i < s_commands_size; i++) {
            if (request == s_commands[i].requestNumber) {
                pRI->pCI = &(s_commands[i]);
                break;
            }
        }
        if (i == s_commands_size) {
            isInvalidRequest = 1;
        }
    } else {
        isInvalidRequest = 1;
    }
    if (isInvalidRequest == 1) {
        Parcel pErr;
        RLOGE("unsupported request code %d token %d", request, token);
        // FIXME this should perhaps return a response
        pErr.writeInt32 (RESPONSE_SOLICITED);
        pErr.writeInt32 (token);
        pErr.writeInt32 (RIL_E_GENERIC_FAILURE);
        free(pRI);
        sendResponse(pErr, socket_id);
        return 0;
    }

    pRI->socket_id = socket_id;

    ret = pthread_mutex_lock(pendingRequestsMutexHook);
    assert (ret == 0);

    pRI->p_next = *pendingRequestsHook;
    *pendingRequestsHook = pRI;

    ret = pthread_mutex_unlock(pendingRequestsMutexHook);
    assert (ret == 0);

/*    sLastDispatchedToken = token; */

#ifdef MTK_RIL
    {
        enqueue(pRI, buffer, buflen, NULL, socket_id);
    }
#else
    pRI->pCI->dispatchFunction(p, pRI);
#endif

    return 0;
}

static void
invalidCommandBlock (RequestInfo *pRI) {
    RLOGE("invalid command block for token %d request %s",
                pRI->token, requestToString(pRI->pCI->requestNumber));
}

/** Callee expects NULL */
static void
dispatchVoid (Parcel& p, RequestInfo *pRI) {
    clearPrintBuf;
    printRequest(pRI->token, pRI->pCI->requestNumber);
    CALL_ONREQUEST(pRI->pCI->requestNumber, NULL, 0, pRI, pRI->socket_id);
}

/** Callee expects const char * */
static void
dispatchString (Parcel& p, RequestInfo *pRI) {
    status_t status;
    size_t datalen;
    size_t stringlen;
    char *string8 = NULL;

    string8 = strdupReadString(p);

    startRequest;
    appendPrintBuf("%s%s", printBuf, string8);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, string8,
                       sizeof(char *), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString(string8);
#endif

    free(string8);
    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

/** Callee expects const char ** */
static void
dispatchStrings (Parcel &p, RequestInfo *pRI) {
    int32_t countStrings;
    status_t status;
    size_t datalen;
    char **pStrings;

    status = p.readInt32 (&countStrings);

    if (status != NO_ERROR) {
        goto invalid;
    }

    startRequest;
    if (countStrings == 0) {
        // just some non-null pointer
        pStrings = (char **)calloc(1, sizeof(char *));
        if (pStrings == NULL) {
            RLOGE("Memory allocation failed for request %s",
                    requestToString(pRI->pCI->requestNumber));
            closeRequest;
            return;
        }

        datalen = 0;
    } else if (countStrings < 0) {
        pStrings = NULL;
        datalen = 0;
    } else {
        datalen = sizeof(char *) * countStrings;

        pStrings = (char **)calloc(countStrings, sizeof(char *));
        if (pStrings == NULL) {
            RLOGE("Memory allocation failed for request %s",
                    requestToString(pRI->pCI->requestNumber));
            closeRequest;
            return;
        }

        for (int i = 0 ; i < countStrings ; i++) {
            pStrings[i] = strdupReadString(p);
            appendPrintBuf("%s%s,", printBuf, pStrings[i]);
        }
    }
    removeLastChar;
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, pStrings, datalen, pRI, pRI->socket_id);

    if (pStrings != NULL) {
        for (int i = 0 ; i < countStrings ; i++) {
#ifdef MEMSET_FREED
            memsetString (pStrings[i]);
#endif
            free(pStrings[i]);
        }

#ifdef MEMSET_FREED
        memset(pStrings, 0, datalen);
#endif
        free(pStrings);
    }

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

/** Callee expects const int * */
static void
dispatchInts (Parcel &p, RequestInfo *pRI) {
    int32_t count = 0;
    status_t status = NO_ERROR;
    size_t datalen = 0;
    int *pInts = NULL;

    status = p.readInt32 (&count);

    if (status != NO_ERROR || count <= 0) {
        goto invalid;
    }

    datalen = sizeof(int) * count;
    pInts = (int *)calloc(count, sizeof(int));
    if (pInts == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(pRI->pCI->requestNumber));
        return;
    }

    startRequest;
    for (int i = 0 ; i < count ; i++) {
        int32_t t = 0;

        status = p.readInt32(&t);
        pInts[i] = (int)t;
        appendPrintBuf("%s%d,", printBuf, t);

        if (status != NO_ERROR) {
            free(pInts);
            goto invalid;
        }
   }
   removeLastChar;
   closeRequest;
   printRequest(pRI->token, pRI->pCI->requestNumber);

   CALL_ONREQUEST(pRI->pCI->requestNumber, const_cast<int *>(pInts),
                       datalen, pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memset(pInts, 0, datalen);
#endif
    free(pInts);
    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}


/**
 * Callee expects const RIL_SMS_WriteArgs *
 * Payload is:
 *   int32_t status
 *   String pdu
 */
static void
dispatchSmsWrite (Parcel &p, RequestInfo *pRI) {
    RIL_SMS_WriteArgs args;
    int32_t t;
    status_t status;

    RLOGD("dispatchSmsWrite");
    memset (&args, 0, sizeof(args));

    status = p.readInt32(&t);
    args.status = (int)t;

    args.pdu = strdupReadString(p);

    if (status != NO_ERROR || args.pdu == NULL) {
        goto invalid;
    }

    args.smsc = strdupReadString(p);

    startRequest;
    appendPrintBuf("%s%d,%s,smsc=%s", printBuf, args.status,
        (char*)args.pdu,  (char*)args.smsc);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &args, sizeof(args), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString (args.pdu);
#endif

    free (args.pdu);

#ifdef MEMSET_FREED
    memset(&args, 0, sizeof(args));
#endif

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

/**
 * Callee expects const RIL_Dial *
 * Payload is:
 *   String address
 *   int32_t clir
 */
static void
dispatchDial (Parcel &p, RequestInfo *pRI) {
    RIL_Dial dial;
    RIL_UUS_Info uusInfo;
    int32_t sizeOfDial;
    int32_t t;
    int32_t uusPresent;
    status_t status;

    RLOGD("dispatchDial");
    memset (&dial, 0, sizeof(dial));

    dial.address = strdupReadString(p);

    status = p.readInt32(&t);
    dial.clir = (int)t;

    if (status != NO_ERROR || dial.address == NULL) {
        goto invalid;
    }

    if (s_callbacks.version < 3) { // Remove when partners upgrade to version 3
        uusPresent = 0;
        sizeOfDial = sizeof(dial) - sizeof(RIL_UUS_Info *);
    } else {
        status = p.readInt32(&uusPresent);

        if (status != NO_ERROR) {
            goto invalid;
        }

        if (uusPresent == 0) {
            dial.uusInfo = NULL;
        } else {
            int32_t len;

            memset(&uusInfo, 0, sizeof(RIL_UUS_Info));

            status = p.readInt32(&t);
            uusInfo.uusType = (RIL_UUS_Type) t;

            status = p.readInt32(&t);
            uusInfo.uusDcs = (RIL_UUS_DCS) t;

            status = p.readInt32(&len);
            if (status != NO_ERROR) {
                goto invalid;
            }

            // The java code writes -1 for null arrays
            if (((int) len) == -1) {
                uusInfo.uusData = NULL;
                len = 0;
            } else {
                uusInfo.uusData = (char*) p.readInplace(len);
                // check if the length is invalid
                if (uusInfo.uusData == NULL) {
                    goto invalid;
                }
            }

            uusInfo.uusLength = len;
            dial.uusInfo = &uusInfo;
        }
        sizeOfDial = sizeof(dial);
    }

    startRequest;
    appendPrintBuf("%snum=%s,clir=%d", printBuf, dial.address, dial.clir);
    if (uusPresent) {
        appendPrintBuf("%s,uusType=%d,uusDcs=%d,uusLen=%d", printBuf,
                dial.uusInfo->uusType, dial.uusInfo->uusDcs,
                dial.uusInfo->uusLength);
    }
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &dial, sizeOfDial, pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString (dial.address);
#endif

    free (dial.address);

#ifdef MEMSET_FREED
    memset(&uusInfo, 0, sizeof(RIL_UUS_Info));
    memset(&dial, 0, sizeof(dial));
#endif

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

/**
 * Callee expects const RIL_Emergency_Dial *
 * Payload is:
 *   String address
 *   int32_t clir
 */
static void
dispatchEmergencyDial(Parcel &p, RequestInfo *pRI) {
    RIL_Emergency_Dial emergencyDial;
    RIL_Dial dial;
    RIL_UUS_Info uusInfo;
    int32_t sizeOfEmergencyDial;
    int32_t t;
    int32_t uusPresent;
    status_t status;

    RLOGD("dispatchEmergencyDial");
    memset(&emergencyDial, 0, sizeof(emergencyDial));

    memset(&dial, 0, sizeof(dial));

    dial.address = strdupReadString(p);

    status = p.readInt32(&t);
    dial.clir = (int)t;

    if (status != NO_ERROR || dial.address == NULL) {
        goto invalid;
    }

    status = p.readInt32(&uusPresent);

    if (status != NO_ERROR) {
        goto invalid;
    }

    if (uusPresent == 0) {
        dial.uusInfo = NULL;
    } else {
        int32_t len;

        memset(&uusInfo, 0, sizeof(RIL_UUS_Info));

        status = p.readInt32(&t);
        uusInfo.uusType = (RIL_UUS_Type) t;

        status = p.readInt32(&t);
        uusInfo.uusDcs = (RIL_UUS_DCS) t;

        status = p.readInt32(&len);
        if (status != NO_ERROR) {
            goto invalid;
        }

        // The java code writes -1 for null arrays
        if (((int) len) == -1) {
            uusInfo.uusData = NULL;
            len = 0;
        } else {
            uusInfo.uusData = (char*) p.readInplace(len);
        }

        uusInfo.uusLength = len;
        dial.uusInfo = &uusInfo;
    }

    emergencyDial.dialData = &dial;

    status = p.readInt32(&t);
    emergencyDial.serviceCategory = (EmergencyServiceCategory)t;

    status = p.readInt32(&t);
    emergencyDial.routing = (EmergencyCallRouting)t;

    status = p.readInt32(&t);
    emergencyDial.isTesting = (t != 0);

    sizeOfEmergencyDial = sizeof(emergencyDial);

    startRequest;
    appendPrintBuf("%snum=%s,clir=%d", printBuf, dial.address, dial.clir);
    if (uusPresent) {
        appendPrintBuf("%s,uusType=%d,uusDcs=%d,uusLen=%d", printBuf,
                dial.uusInfo->uusType, dial.uusInfo->uusDcs,
                dial.uusInfo->uusLength);
    }
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &emergencyDial, sizeOfEmergencyDial, pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString(dial.address);
#endif

    free(dial.address);

#ifdef MEMSET_FREED
    memset(&uusInfo, 0, sizeof(RIL_UUS_Info));
    memset(&dial, 0, sizeof(dial));
    memset(&emergencyDial, 0, sizeof(emergencyDial));
#endif

    return;

invalid:
    invalidCommandBlock(pRI);
    if (dial.address != NULL) {
#ifdef MEMSET_FREED
        memsetString(dial.address);
#endif
        free(dial.address);
    }

#ifdef MEMSET_FREED
    memset(&uusInfo, 0, sizeof(RIL_UUS_Info));
    memset(&dial, 0, sizeof(dial));
    memset(&emergencyDial, 0, sizeof(emergencyDial));
#endif
    return;
}


/**
 * Callee expects const RIL_Redial *
 * Payload is:
 *   String address
 *   int32_t clir
 *   RIL_UUS_Info uusInfo
 *   int32 mode
 *   int32 callId
 */
static void
dispatchRedial (Parcel &p, RequestInfo *pRI) {
    RIL_Redial dial;
    RIL_UUS_Info uusInfo;
    int32_t sizeOfDial;
    int32_t t;
    int32_t uusPresent;
    status_t status;

    RLOGD("dispatchRedial");
    memset (&dial, 0, sizeof(dial));

    dial.address = strdupReadString(p);

    status = p.readInt32(&t);
    dial.clir = (int)t;

    if (status != NO_ERROR || dial.address == NULL) {
        goto invalid;
    }

    if (s_callbacks.version < 3) { // Remove when partners upgrade to version 3
        uusPresent = 0;
        sizeOfDial = sizeof(dial) - sizeof(RIL_UUS_Info *);
    } else {
        status = p.readInt32(&uusPresent);

        if (status != NO_ERROR) {
            goto invalid;
        }

        if (uusPresent == 0) {
            dial.uusInfo = NULL;
        } else {
            int32_t len;

            memset(&uusInfo, 0, sizeof(RIL_UUS_Info));

            status = p.readInt32(&t);
            uusInfo.uusType = (RIL_UUS_Type) t;

            status = p.readInt32(&t);
            uusInfo.uusDcs = (RIL_UUS_DCS) t;

            status = p.readInt32(&len);
            if (status != NO_ERROR) {
                goto invalid;
            }

            // The java code writes -1 for null arrays
            if (((int) len) == -1) {
                uusInfo.uusData = NULL;
                len = 0;
            } else {
                uusInfo.uusData = (char*) p.readInplace(len);
                // check if the length is invalid
                if (uusInfo.uusData == NULL) {
                    goto invalid;
                }
            }

            uusInfo.uusLength = len;
            dial.uusInfo = &uusInfo;
        }
        sizeOfDial = sizeof(dial);
    }

    status = p.readInt32(&t);
    dial.mode = (int)t;
    status = p.readInt32(&t);
    dial.callId = (int)t;

    startRequest;
    appendPrintBuf("%snum=%s,clir=%d", printBuf, dial.address, dial.clir);
    if (uusPresent) {
        appendPrintBuf("%s,uusType=%d,uusDcs=%d,uusLen=%d", printBuf,
                dial.uusInfo->uusType, dial.uusInfo->uusDcs,
                dial.uusInfo->uusLength);
    }
    appendPrintBuf("%s,mode=%d,callId=%d", printBuf, dial.mode, dial.callId);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &dial, sizeOfDial, pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString (dial.address);
#endif

    free (dial.address);

#ifdef MEMSET_FREED
    memset(&uusInfo, 0, sizeof(RIL_UUS_Info));
    memset(&dial, 0, sizeof(dial));
#endif

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

/**
 * Callee expects const RIL_SIM_IO *
 * Payload is:
 *   int32_t command
 *   int32_t fileid
 *   String path
 *   int32_t p1, p2, p3
 *   String data
 *   String pin2
 *   String aidPtr
 */
static void
dispatchSIM_IO (Parcel &p, RequestInfo *pRI) {
    union RIL_SIM_IO {
        RIL_SIM_IO_v6 v6;
        RIL_SIM_IO_v5 v5;
    } simIO;

    int32_t t;
    int size;
    status_t status;

#if VDBG
    RLOGD("dispatchSIM_IO");
#endif
    memset (&simIO, 0, sizeof(simIO));

    // note we only check status at the end

    status = p.readInt32(&t);
    simIO.v6.command = (int)t;

    status = p.readInt32(&t);
    simIO.v6.fileid = (int)t;

    simIO.v6.path = strdupReadString(p);

    status = p.readInt32(&t);
    simIO.v6.p1 = (int)t;

    status = p.readInt32(&t);
    simIO.v6.p2 = (int)t;

    status = p.readInt32(&t);
    simIO.v6.p3 = (int)t;

    simIO.v6.data = strdupReadString(p);
    simIO.v6.pin2 = strdupReadString(p);
    simIO.v6.aidPtr = strdupReadString(p);

    startRequest;
    appendPrintBuf("%scmd=0x%X,efid=0x%X,path=%s,%d,%d,%d,%s,pin2=%s,aid=%s", printBuf,
        simIO.v6.command, simIO.v6.fileid, (char*)simIO.v6.path,
        simIO.v6.p1, simIO.v6.p2, simIO.v6.p3,
        (char*)simIO.v6.data,  (char*)simIO.v6.pin2, simIO.v6.aidPtr);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    if (status != NO_ERROR) {
        goto invalid;
    }

    size = (s_callbacks.version < 6) ? sizeof(simIO.v5) : sizeof(simIO.v6);
    CALL_ONREQUEST(pRI->pCI->requestNumber, &simIO, size, pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString (simIO.v6.path);
    memsetString (simIO.v6.data);
    memsetString (simIO.v6.pin2);
    memsetString (simIO.v6.aidPtr);
#endif

    free (simIO.v6.path);
    free (simIO.v6.data);
    free (simIO.v6.pin2);
    free (simIO.v6.aidPtr);

#ifdef MEMSET_FREED
    memset(&simIO, 0, sizeof(simIO));
#endif

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

/**
 * Callee expects const RIL_SIM_APDU *
 * Payload is:
 *   int32_t sessionid
 *   int32_t cla
 *   int32_t instruction
 *   int32_t p1, p2, p3
 *   String data
 */
static void
dispatchSIM_APDU (Parcel &p, RequestInfo *pRI) {
    int32_t t;
    status_t status;
    RIL_SIM_APDU apdu;

#if VDBG
    RLOGD("dispatchSIM_APDU");
#endif
    memset (&apdu, 0, sizeof(RIL_SIM_APDU));

    // Note we only check status at the end. Any single failure leads to
    // subsequent reads filing.
    status = p.readInt32(&t);
    apdu.sessionid = (int)t;

    status = p.readInt32(&t);
    apdu.cla = (int)t;

    status = p.readInt32(&t);
    apdu.instruction = (int)t;

    status = p.readInt32(&t);
    apdu.p1 = (int)t;

    status = p.readInt32(&t);
    apdu.p2 = (int)t;

    status = p.readInt32(&t);
    apdu.p3 = (int)t;

    apdu.data = strdupReadString(p);

    startRequest;
    appendPrintBuf("%ssessionid=%d,cla=%d,ins=%d,p1=%d,p2=%d,p3=%d,data=%s",
        printBuf, apdu.sessionid, apdu.cla, apdu.instruction, apdu.p1, apdu.p2,
        apdu.p3, (char*)apdu.data);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    if (status != NO_ERROR) {
        goto invalid;
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &apdu, sizeof(RIL_SIM_APDU), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString(apdu.data);
#endif
    free(apdu.data);

#ifdef MEMSET_FREED
    memset(&apdu, 0, sizeof(RIL_SIM_APDU));
#endif

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}


/**
 * Callee expects const RIL_CallForwardInfo *
 * Payload is:
 *  int32_t status/action
 *  int32_t reason
 *  int32_t serviceCode
 *  int32_t toa
 *  String number  (0 length -> null)
 *  int32_t timeSeconds
 */
static void
dispatchCallForward(Parcel &p, RequestInfo *pRI) {
    RIL_CallForwardInfo cff;
    int32_t t;
    status_t status;

    RLOGD("dispatchCallForward");
    memset (&cff, 0, sizeof(cff));

    // note we only check status at the end

    status = p.readInt32(&t);
    cff.status = (int)t;

    status = p.readInt32(&t);
    cff.reason = (int)t;

    status = p.readInt32(&t);
    cff.serviceClass = (int)t;

    status = p.readInt32(&t);
    cff.toa = (int)t;

    cff.number = strdupReadString(p);

    status = p.readInt32(&t);
    cff.timeSeconds = (int)t;

    if (status != NO_ERROR) {
        goto invalid;
    }

    // special case: number 0-length fields is null

    if (cff.number != NULL && strlen (cff.number) == 0) {
        free(cff.number);
        cff.number = NULL;
    }

    startRequest;
    appendPrintBuf("%sstat=%d,reason=%d,serv=%d,toa=%d,%s,tout=%d", printBuf,
        cff.status, cff.reason, cff.serviceClass, cff.toa,
        (char*)cff.number, cff.timeSeconds);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &cff, sizeof(cff), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString(cff.number);
#endif

    free (cff.number);

#ifdef MEMSET_FREED
    memset(&cff, 0, sizeof(cff));
#endif

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

/**
 * Callee expects const RIL_CallForwardInfoEx *
 * Payload is:
 *  int32_t status/action
 *  int32_t reason
 *  int32_t serviceCode
 *  int32_t toa
 *  String number  (0 length -> null)
 *  int32_t timeSeconds
 *  String timeSlotBegin  (0 length -> null)
 *  String timeSlotEnd  (0 length -> null)
 */
static void
dispatchCallForwardEx(Parcel &p, RequestInfo *pRI) {
    RIL_CallForwardInfoEx cff;
    int32_t t;
    status_t status;

    RLOGD("dispatchCallForwardEx");
    memset (&cff, 0, sizeof(cff));

    // note we only check status at the end

    status = p.readInt32(&t);
    cff.status = (int)t;

    status = p.readInt32(&t);
    cff.reason = (int)t;

    status = p.readInt32(&t);
    cff.serviceClass = (int)t;

    status = p.readInt32(&t);
    cff.toa = (int)t;

    cff.number = strdupReadString(p);

    status = p.readInt32(&t);
    cff.timeSeconds = (int)t;

    cff.timeSlotBegin = strdupReadString(p);
    cff.timeSlotEnd = strdupReadString(p);

    if (status != NO_ERROR) {
        goto invalid;
    }

    // special case: number 0-length fields is null

    if (cff.number != NULL && strlen (cff.number) == 0) {
        free(cff.number);
        cff.number = NULL;
    }

    if (cff.timeSlotBegin != NULL && strlen (cff.timeSlotBegin) == 0) {
        free(cff.timeSlotBegin);
        cff.timeSlotBegin = NULL;
    }
    if (cff.timeSlotEnd != NULL && strlen (cff.timeSlotEnd) == 0) {
        free(cff.timeSlotEnd);
        cff.timeSlotEnd = NULL;
    }

    startRequest;
    appendPrintBuf("%sstat=%d,reason=%d,serv=%d,toa=%d,%s,tout=%d,timeSlot=%s,%s", printBuf,
        cff.status, cff.reason, cff.serviceClass, cff.toa,
        (char*)cff.number, cff.timeSeconds, cff.timeSlotBegin, cff.timeSlotEnd);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &cff, sizeof(cff), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString(cff.number);
    memsetString(cff.timeSlotBegin);
    memsetString(cff.timeSlotEnd);
#endif

    free (cff.number);
    free (cff.timeSlotBegin);
    free (cff.timeSlotEnd);

#ifdef MEMSET_FREED
    memset(&cff, 0, sizeof(cff));
#endif

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

static void
dispatchRaw(Parcel &p, RequestInfo *pRI) {
    int32_t len;
    status_t status;
    const void *data;

    status = p.readInt32(&len);

    if (status != NO_ERROR) {
        goto invalid;
    }

    // The java code writes -1 for null arrays
    if (((int)len) == -1) {
        data = NULL;
        len = 0;
    }

    data = p.readInplace(len);

    startRequest;
    appendPrintBuf("%sraw_size=%d", printBuf, len);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, const_cast<void *>(data), len, pRI, pRI->socket_id);

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

static status_t
constructCdmaSms(Parcel &p, RequestInfo *pRI, RIL_CDMA_SMS_Message& rcsm) {
    int32_t  t = 0;
    uint8_t ut = 0;
    status_t status = NO_ERROR;
    int32_t digitCount = 0;
    int digitLimit = 0;

    memset(&rcsm, 0, sizeof(rcsm));

    status = p.readInt32(&t);
    rcsm.uTeleserviceID = (int) t;

    status = p.read(&ut,sizeof(ut));
    rcsm.bIsServicePresent = (uint8_t) ut;

    status = p.readInt32(&t);
    rcsm.uServicecategory = (int) t;

    status = p.readInt32(&t);
    rcsm.sAddress.digit_mode = (RIL_CDMA_SMS_DigitMode) t;

    status = p.readInt32(&t);
    rcsm.sAddress.number_mode = (RIL_CDMA_SMS_NumberMode) t;

    status = p.readInt32(&t);
    rcsm.sAddress.number_type = (RIL_CDMA_SMS_NumberType) t;

    status = p.readInt32(&t);
    rcsm.sAddress.number_plan = (RIL_CDMA_SMS_NumberPlan) t;

    status = p.read(&ut,sizeof(ut));
    rcsm.sAddress.number_of_digits= (uint8_t) ut;

    digitLimit= MIN((rcsm.sAddress.number_of_digits), RIL_CDMA_SMS_ADDRESS_MAX);
    for(digitCount =0 ; digitCount < digitLimit; digitCount ++) {
        status = p.read(&ut,sizeof(ut));
        rcsm.sAddress.digits[digitCount] = (uint8_t) ut;
    }

    status = p.readInt32(&t);
    rcsm.sSubAddress.subaddressType = (RIL_CDMA_SMS_SubaddressType) t;

    status = p.read(&ut,sizeof(ut));
    rcsm.sSubAddress.odd = (uint8_t) ut;

    status = p.read(&ut,sizeof(ut));
    rcsm.sSubAddress.number_of_digits = (uint8_t) ut;

    digitLimit= MIN((rcsm.sSubAddress.number_of_digits), RIL_CDMA_SMS_SUBADDRESS_MAX);
    for(digitCount =0 ; digitCount < digitLimit; digitCount ++) {
        status = p.read(&ut,sizeof(ut));
        rcsm.sSubAddress.digits[digitCount] = (uint8_t) ut;
    }

    status = p.readInt32(&t);
    rcsm.uBearerDataLen = (int) t;

    digitLimit= MIN((rcsm.uBearerDataLen), RIL_CDMA_SMS_BEARER_DATA_MAX);
    for(digitCount =0 ; digitCount < digitLimit; digitCount ++) {
        status = p.read(&ut, sizeof(ut));
        rcsm.aBearerData[digitCount] = (uint8_t) ut;
    }

    if (status != NO_ERROR) {
        return status;
    }

    startRequest;
    appendPrintBuf("%suTeleserviceID=%d, bIsServicePresent=%d, uServicecategory=%d, \
            sAddress.digit_mode=%d, sAddress.Number_mode=%d, sAddress.number_type=%d, ",
            printBuf, rcsm.uTeleserviceID,rcsm.bIsServicePresent,rcsm.uServicecategory,
            rcsm.sAddress.digit_mode, rcsm.sAddress.number_mode,rcsm.sAddress.number_type);
    closeRequest;

    printRequest(pRI->token, pRI->pCI->requestNumber);

    return status;
}

static void
dispatchCdmaSms(Parcel &p, RequestInfo *pRI) {
    RIL_CDMA_SMS_Message rcsm;

    RLOGD("dispatchCdmaSms");
    if (NO_ERROR != constructCdmaSms(p, pRI, rcsm)) {
        goto invalid;
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &rcsm, sizeof(rcsm),pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memset(&rcsm, 0, sizeof(rcsm));
#endif

    return;

invalid:
    invalidCommandBlock(pRI);
    return;
}

static void
dispatchImsCdmaSms(Parcel &p, RequestInfo *pRI, uint8_t retry, int32_t messageRef) {
    RIL_IMS_SMS_Message rism;
    RIL_CDMA_SMS_Message rcsm;

    RLOGD("dispatchImsCdmaSms: retry=%d, messageRef=%d", retry, messageRef);

    if (NO_ERROR != constructCdmaSms(p, pRI, rcsm)) {
        goto invalid;
    }
    memset(&rism, 0, sizeof(rism));
    rism.tech = RADIO_TECH_3GPP2;
    rism.retry = retry;
    rism.messageRef = messageRef;
    rism.message.cdmaMessage = &rcsm;

    CALL_ONREQUEST(pRI->pCI->requestNumber, &rism,
            sizeof(RIL_RadioTechnologyFamily)+sizeof(uint8_t)+sizeof(int32_t)
            +sizeof(rcsm),pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memset(&rcsm, 0, sizeof(rcsm));
    memset(&rism, 0, sizeof(rism));
#endif

    return;

invalid:
    invalidCommandBlock(pRI);
    return;
}

static void
dispatchImsGsmSms(Parcel &p, RequestInfo *pRI, uint8_t retry, int32_t messageRef) {
    RIL_IMS_SMS_Message rism;
    int32_t countStrings = 0;
    status_t status = NO_ERROR;
    size_t datalen = 0;
    char **pStrings;
    RLOGD("dispatchImsGsmSms: retry=%d, messageRef=%d", retry, messageRef);

    status = p.readInt32 (&countStrings);

    if (status != NO_ERROR) {
        goto invalid;
    }

    memset(&rism, 0, sizeof(rism));
    rism.tech = RADIO_TECH_3GPP;
    rism.retry = retry;
    rism.messageRef = messageRef;

    startRequest;
    appendPrintBuf("%stech=%d, retry=%d, messageRef=%d, ", printBuf,
                    (int)rism.tech, (int)rism.retry, rism.messageRef);
    if (countStrings == 0) {
        // just some non-null pointer
        pStrings = (char **)calloc(1, sizeof(char *));
        if (pStrings == NULL) {
            RLOGE("Memory allocation failed for request %s",
                    requestToString(pRI->pCI->requestNumber));
            closeRequest;
            return;
        }

        datalen = 0;
    } else if (countStrings < 0) {
        pStrings = NULL;
        datalen = 0;
    } else {
        if ((size_t)countStrings > (INT_MAX/sizeof(char *))) {
            RLOGE("Invalid value of countStrings: \n");
            closeRequest;
            return;
        }
        datalen = sizeof(char *) * countStrings;

        pStrings = (char **)calloc(countStrings, sizeof(char *));
        if (pStrings == NULL) {
            RLOGE("Memory allocation failed for request %s",
                    requestToString(pRI->pCI->requestNumber));
            closeRequest;
            return;
        }

        for (int i = 0 ; i < countStrings ; i++) {
            pStrings[i] = strdupReadString(p);
            appendPrintBuf("%s%s,", printBuf, pStrings[i]);
        }
    }
    removeLastChar;
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    rism.message.gsmMessage = pStrings;
    CALL_ONREQUEST(pRI->pCI->requestNumber, &rism,
            sizeof(RIL_RadioTechnologyFamily)+sizeof(uint8_t)+sizeof(int32_t)
            +datalen, pRI, pRI->socket_id);

    if (pStrings != NULL) {
        for (int i = 0 ; i < countStrings ; i++) {
#ifdef MEMSET_FREED
            memsetString (pStrings[i]);
#endif
            free(pStrings[i]);
        }

#ifdef MEMSET_FREED
        memset(pStrings, 0, datalen);
#endif
        free(pStrings);
    }

#ifdef MEMSET_FREED
    memset(&rism, 0, sizeof(rism));
#endif
    return;
invalid:
    ALOGE("dispatchImsGsmSms invalid block");
    invalidCommandBlock(pRI);
    return;
}

static void
dispatchImsSms(Parcel &p, RequestInfo *pRI) {
    int32_t  t = 0;
    status_t status = p.readInt32(&t);
    RIL_RadioTechnologyFamily format;
    uint8_t retry = 0;
    int32_t messageRef = 0;

    RLOGD("dispatchImsSms");
    if (status != NO_ERROR) {
        goto invalid;
    }
    format = (RIL_RadioTechnologyFamily) t;

    // read retry field
    status = p.read(&retry,sizeof(retry));
    if (status != NO_ERROR) {
        goto invalid;
    }
    // read messageRef field
    status = p.read(&messageRef,sizeof(messageRef));
    if (status != NO_ERROR) {
        goto invalid;
    }

    if (RADIO_TECH_3GPP == format) {
        dispatchImsGsmSms(p, pRI, retry, messageRef);
    } else if (RADIO_TECH_3GPP2 == format) {
        dispatchImsCdmaSms(p, pRI, retry, messageRef);
    } else {
        ALOGE("requestImsSendSMS invalid format value =%d", format);
    }

    return;

invalid:
    invalidCommandBlock(pRI);
    return;
}

static void
dispatchCdmaSmsAck(Parcel &p, RequestInfo *pRI) {
    RIL_CDMA_SMS_Ack rcsa;
    int32_t  t = 0;
    status_t status = NO_ERROR;
    int32_t digitCount = 0;

    RLOGD("dispatchCdmaSmsAck");
    memset(&rcsa, 0, sizeof(rcsa));

    status = p.readInt32(&t);
    rcsa.uErrorClass = (RIL_CDMA_SMS_ErrorClass) t;

    status = p.readInt32(&t);
    rcsa.uSMSCauseCode = (int) t;

    if (status != NO_ERROR) {
        goto invalid;
    }

    startRequest;
    appendPrintBuf("%suErrorClass=%d, uTLStatus=%d, ",
            printBuf, rcsa.uErrorClass, rcsa.uSMSCauseCode);
    closeRequest;

    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &rcsa, sizeof(rcsa),pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memset(&rcsa, 0, sizeof(rcsa));
#endif

    return;

invalid:
    invalidCommandBlock(pRI);
    return;
}

static void
dispatchGsmBrSmsCnf(Parcel &p, RequestInfo *pRI) {
    int32_t t = 0;
    status_t status = NO_ERROR;
    int32_t num = 0;

    status = p.readInt32(&num);
    if (status != NO_ERROR) {
        goto invalid;
    }

    {
        RIL_GSM_BroadcastSmsConfigInfo gsmBci[num];
        RIL_GSM_BroadcastSmsConfigInfo *gsmBciPtrs[num];

        startRequest;
        for (int i = 0 ; i < num ; i++ ) {
            gsmBciPtrs[i] = &gsmBci[i];

            status = p.readInt32(&t);
            gsmBci[i].fromServiceId = (int) t;

            status = p.readInt32(&t);
            gsmBci[i].toServiceId = (int) t;

            status = p.readInt32(&t);
            gsmBci[i].fromCodeScheme = (int) t;

            status = p.readInt32(&t);
            gsmBci[i].toCodeScheme = (int) t;

            status = p.readInt32(&t);
            gsmBci[i].selected = (uint8_t) t;

            appendPrintBuf("%s [%d: fromServiceId=%d, toServiceId =%d, \
                  fromCodeScheme=%d, toCodeScheme=%d, selected =%d]", printBuf, i,
                  gsmBci[i].fromServiceId, gsmBci[i].toServiceId,
                  gsmBci[i].fromCodeScheme, gsmBci[i].toCodeScheme,
                  gsmBci[i].selected);
        }
        closeRequest;

        if (status != NO_ERROR) {
            goto invalid;
        }

        CALL_ONREQUEST(pRI->pCI->requestNumber,
                              gsmBciPtrs,
                              num * sizeof(RIL_GSM_BroadcastSmsConfigInfo *),
                              pRI, pRI->socket_id);

#ifdef MEMSET_FREED
        memset(gsmBci, 0, num * sizeof(RIL_GSM_BroadcastSmsConfigInfo));
        memset(gsmBciPtrs, 0, num * sizeof(RIL_GSM_BroadcastSmsConfigInfo *));
#endif
    }

    return;

invalid:
    invalidCommandBlock(pRI);
    return;
}

static void
dispatchCdmaBrSmsCnf(Parcel &p, RequestInfo *pRI) {
    int32_t t = 0;
    status_t status = NO_ERROR;
    int32_t num = 0;

    status = p.readInt32(&num);
    if (status != NO_ERROR) {
        goto invalid;
    }

    {
        RIL_CDMA_BroadcastSmsConfigInfo cdmaBci[num];
        RIL_CDMA_BroadcastSmsConfigInfo *cdmaBciPtrs[num];

        startRequest;
        for (int i = 0 ; i < num ; i++ ) {
            cdmaBciPtrs[i] = &cdmaBci[i];

            status = p.readInt32(&t);
            cdmaBci[i].service_category = (int) t;

            status = p.readInt32(&t);
            cdmaBci[i].language = (int) t;

            status = p.readInt32(&t);
            cdmaBci[i].selected = (uint8_t) t;

            appendPrintBuf("%s [%d: service_category=%d, language =%d, \
                  entries.bSelected =%d]", printBuf, i, cdmaBci[i].service_category,
                  cdmaBci[i].language, cdmaBci[i].selected);
        }
        closeRequest;

        if (status != NO_ERROR) {
            goto invalid;
        }

        CALL_ONREQUEST(pRI->pCI->requestNumber,
                              cdmaBciPtrs,
                              num * sizeof(RIL_CDMA_BroadcastSmsConfigInfo *),
                              pRI, pRI->socket_id);

#ifdef MEMSET_FREED
        memset(cdmaBci, 0, num * sizeof(RIL_CDMA_BroadcastSmsConfigInfo));
        memset(cdmaBciPtrs, 0, num * sizeof(RIL_CDMA_BroadcastSmsConfigInfo *));
#endif
    }

    return;

invalid:
    invalidCommandBlock(pRI);
    return;
}

static void dispatchRilCdmaSmsWriteArgs(Parcel &p, RequestInfo *pRI) {
    RIL_CDMA_SMS_WriteArgs rcsw;
    int32_t  t = 0;
    uint32_t ut = 0;
    uint8_t  uct = 0;
    status_t status = NO_ERROR;
    int32_t  digitCount = 0;
    int32_t  digitLimit = 0;

    memset(&rcsw, 0, sizeof(rcsw));

    status = p.readInt32(&t);
    rcsw.status = t;

    status = p.readInt32(&t);
    rcsw.message.uTeleserviceID = (int) t;

    status = p.read(&uct,sizeof(uct));
    rcsw.message.bIsServicePresent = (uint8_t) uct;

    status = p.readInt32(&t);
    rcsw.message.uServicecategory = (int) t;

    status = p.readInt32(&t);
    rcsw.message.sAddress.digit_mode = (RIL_CDMA_SMS_DigitMode) t;

    status = p.readInt32(&t);
    rcsw.message.sAddress.number_mode = (RIL_CDMA_SMS_NumberMode) t;

    status = p.readInt32(&t);
    rcsw.message.sAddress.number_type = (RIL_CDMA_SMS_NumberType) t;

    status = p.readInt32(&t);
    rcsw.message.sAddress.number_plan = (RIL_CDMA_SMS_NumberPlan) t;

    status = p.read(&uct,sizeof(uct));
    rcsw.message.sAddress.number_of_digits = (uint8_t) uct;

    digitLimit = MIN((rcsw.message.sAddress.number_of_digits), RIL_CDMA_SMS_ADDRESS_MAX);

    for(digitCount = 0 ; digitCount < digitLimit; digitCount ++) {
        status = p.read(&uct,sizeof(uct));
        rcsw.message.sAddress.digits[digitCount] = (uint8_t) uct;
    }

    status = p.readInt32(&t);
    rcsw.message.sSubAddress.subaddressType = (RIL_CDMA_SMS_SubaddressType) t;

    status = p.read(&uct,sizeof(uct));
    rcsw.message.sSubAddress.odd = (uint8_t) uct;

    status = p.read(&uct,sizeof(uct));
    rcsw.message.sSubAddress.number_of_digits = (uint8_t) uct;

    digitLimit = MIN((rcsw.message.sSubAddress.number_of_digits), RIL_CDMA_SMS_SUBADDRESS_MAX);

    for(digitCount = 0 ; digitCount < digitLimit; digitCount ++) {
        status = p.read(&uct,sizeof(uct));
        rcsw.message.sSubAddress.digits[digitCount] = (uint8_t) uct;
    }

    status = p.readInt32(&t);
    rcsw.message.uBearerDataLen = (int) t;

    digitLimit = MIN((rcsw.message.uBearerDataLen), RIL_CDMA_SMS_BEARER_DATA_MAX);

    for(digitCount = 0 ; digitCount < digitLimit; digitCount ++) {
        status = p.read(&uct, sizeof(uct));
        rcsw.message.aBearerData[digitCount] = (uint8_t) uct;
    }

    if (status != NO_ERROR) {
        goto invalid;
    }

    startRequest;
    appendPrintBuf("%sstatus=%d, message.uTeleserviceID=%d, message.bIsServicePresent=%d, \
            message.uServicecategory=%d, message.sAddress.digit_mode=%d, \
            message.sAddress.number_mode=%d, \
            message.sAddress.number_type=%d, ",
            printBuf, rcsw.status, rcsw.message.uTeleserviceID, rcsw.message.bIsServicePresent,
            rcsw.message.uServicecategory, rcsw.message.sAddress.digit_mode,
            rcsw.message.sAddress.number_mode,
            rcsw.message.sAddress.number_type);
    closeRequest;

    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &rcsw, sizeof(rcsw),pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memset(&rcsw, 0, sizeof(rcsw));
#endif

    return;

invalid:
    invalidCommandBlock(pRI);
    return;

}

// For backwards compatibility in RIL_REQUEST_SETUP_DATA_CALL.
// Version 4 of the RIL interface adds a new PDP type parameter to support
// IPv6 and dual-stack PDP contexts. When dealing with a previous version of
// RIL, remove the parameter from the request.
static void dispatchDataCall(Parcel& p, RequestInfo *pRI) {
    // In RIL v3, REQUEST_SETUP_DATA_CALL takes 6 parameters.
    const int numParamsRilV3 = 6;

    // The first bytes of the RIL parcel contain the request number and the
    // serial number - see processCommandBuffer(). Copy them over too.
    int pos = p.dataPosition();

    int numParams = p.readInt32();
    if (s_callbacks.version < 4 && numParams > numParamsRilV3) {
      Parcel p2;
      p2.appendFrom(&p, 0, pos);
      p2.writeInt32(numParamsRilV3);
      for(int i = 0; i < numParamsRilV3; i++) {
        p2.writeString16(p.readString16());
      }
      p2.setDataPosition(pos);
      dispatchStrings(p2, pRI);
    } else {
      p.setDataPosition(pos);
      dispatchStrings(p, pRI);
    }
}

static void dispatchSetInitialAttachApn(Parcel &p, RequestInfo *pRI)
{
    RIL_InitialAttachApn_v15 pf;
    int32_t  t;
    status_t status;

    memset(&pf, 0, sizeof(pf));

    pf.apn = strdupReadString(p);
    pf.protocol = strdupReadString(p);
    pf.roamingProtocol = strdupReadString(p);
    status = p.readInt32(&t);
    pf.authtype = (int) t;

    pf.username = strdupReadString(p);
    pf.password = strdupReadString(p);

    status = p.readInt32(&t);
    pf.supportedTypesBitmask = (int) t;
    status = p.readInt32(&t);
    pf.bearerBitmask = (int) t;
    status = p.readInt32(&t);
    pf.modemCognitive = (int) t;
    status = p.readInt32(&t);
    pf.mtu = (int) t;

    pf.mvnoType = strdupReadString(p);
    pf.mvnoMatchData = strdupReadString(p);
    status = p.readInt32(&t);
    pf.canHandleIms= (int) t;
    startRequest;
    appendPrintBuf("%sapn=%s, protocol=%s, roamingProtocol=%s, authtype=%d, username=%s, \
password=%s, TypesBitmask=%d, bearerBitmask=%d, modemCognitive=%d, mtu=%d, mvnoType=%s, \
mvnoMatchData=%s, canHandleIms=%d",
            printBuf, pf.apn, pf.protocol, pf.roamingProtocol, pf.authtype, pf.username,
            pf.password, pf.supportedTypesBitmask, pf.bearerBitmask, pf.modemCognitive,
            pf.mtu, pf.mvnoType, pf.mvnoMatchData, pf.canHandleIms);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    if (status != NO_ERROR) {
        goto invalid;
    }
    CALL_ONREQUEST(pRI->pCI->requestNumber, &pf, sizeof(pf), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString(pf.apn);
    memsetString(pf.protocol);
    memsetString(pf.roamingProtocol);
    memsetString(pf.username);
    memsetString(pf.password);
    memsetString(pf.mvnoType);
    memsetString(pf.mvnoMatchData);
#endif

    free(pf.apn);
    free(pf.protocol);
    free(pf.roamingProtocol);
    free(pf.username);
    free(pf.password);
    free(pf.mvnoType);
    free(pf.mvnoMatchData);

#ifdef MEMSET_FREED
    memset(&pf, 0, sizeof(pf));
#endif

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

static void dispatchNVReadItem(Parcel &p, RequestInfo *pRI) {
    RIL_NV_ReadItem nvri;
    int32_t  t;
    status_t status;

    memset(&nvri, 0, sizeof(nvri));

    status = p.readInt32(&t);
    nvri.itemID = (RIL_NV_Item) t;

    if (status != NO_ERROR) {
        goto invalid;
    }

    startRequest;
    appendPrintBuf("%snvri.itemID=%d, ", printBuf, nvri.itemID);
    closeRequest;

    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &nvri, sizeof(nvri), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memset(&nvri, 0, sizeof(nvri));
#endif

    return;

invalid:
    invalidCommandBlock(pRI);
    return;
}

static void dispatchNVWriteItem(Parcel &p, RequestInfo *pRI) {
    RIL_NV_WriteItem nvwi;
    int32_t  t;
    status_t status;

    memset(&nvwi, 0, sizeof(nvwi));

    status = p.readInt32(&t);
    nvwi.itemID = (RIL_NV_Item) t;

    nvwi.value = strdupReadString(p);

    if (status != NO_ERROR || nvwi.value == NULL) {
        goto invalid;
    }

    startRequest;
    appendPrintBuf("%snvwi.itemID=%d, value=%s, ", printBuf, nvwi.itemID,
            nvwi.value);
    closeRequest;

    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &nvwi, sizeof(nvwi), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString(nvwi.value);
#endif

    free(nvwi.value);

#ifdef MEMSET_FREED
    memset(&nvwi, 0, sizeof(nvwi));
#endif

    return;

invalid:
    invalidCommandBlock(pRI);
    return;
}


static void dispatchUiccSubscripton(Parcel &p, RequestInfo *pRI) {
    RIL_SelectUiccSub uicc_sub;
    status_t status;
    int32_t  t;
    memset(&uicc_sub, 0, sizeof(uicc_sub));

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }
    uicc_sub.slot = (int) t;

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }
    uicc_sub.app_index = (int) t;

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }
    uicc_sub.sub_type = (RIL_SubscriptionType) t;

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }
    uicc_sub.act_status = (RIL_UiccSubActStatus) t;

    startRequest;
    appendPrintBuf("slot=%d, app_index=%d, act_status = %d", uicc_sub.slot, uicc_sub.app_index,
            uicc_sub.act_status);
    RLOGD("dispatchUiccSubscription, slot=%d, app_index=%d, act_status = %d", uicc_sub.slot,
            uicc_sub.app_index, uicc_sub.act_status);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &uicc_sub, sizeof(uicc_sub), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memset(&uicc_sub, 0, sizeof(uicc_sub));
#endif
    return;

invalid:
    invalidCommandBlock(pRI);
    return;
}

static void dispatchSimAuthentication(Parcel &p, RequestInfo *pRI)
{
    RIL_SimAuthentication pf;
    int32_t  t;
    status_t status;

    memset(&pf, 0, sizeof(pf));

    status = p.readInt32(&t);
    pf.authContext = (int) t;
    pf.authData = strdupReadString(p);
    pf.aid = strdupReadString(p);

    startRequest;
    appendPrintBuf("authContext=%s, authData=%s, aid=%s", pf.authContext, pf.authData, pf.aid);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    if (status != NO_ERROR) {
        goto invalid;
    }
    CALL_ONREQUEST(pRI->pCI->requestNumber, &pf, sizeof(pf), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString(pf.authData);
    memsetString(pf.aid);
#endif

    free(pf.authData);
    free(pf.aid);

#ifdef MEMSET_FREED
    memset(&pf, 0, sizeof(pf));
#endif

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

static void dispatchDataProfile(Parcel &p, RequestInfo *pRI) {
    int32_t t = 0;
    status_t status;
    int32_t num = 0;

    status = p.readInt32(&num);
    if (status != NO_ERROR || num < 0) {
        goto invalid;
    }

    if (s_callbacks.version <= 14) {
        RIL_DataProfileInfo *dataProfiles =
                (RIL_DataProfileInfo *)calloc(num, sizeof(RIL_DataProfileInfo));
        if (dataProfiles == NULL) {
            RLOGE("Memory allocation failed for request %s",
                    requestToString(pRI->pCI->requestNumber));
            return;
        }
        RIL_DataProfileInfo **dataProfilePtrs =
                (RIL_DataProfileInfo **)calloc(num, sizeof(RIL_DataProfileInfo *));
        if (dataProfilePtrs == NULL) {
            RLOGE("Memory allocation failed for request %s",
                    requestToString(pRI->pCI->requestNumber));
            free(dataProfiles);
            return;
        }

        startRequest;
        for (int i = 0 ; i < num ; i++ ) {
            dataProfilePtrs[i] = &dataProfiles[i];

            status = p.readInt32(&t);
            dataProfiles[i].profileId = (int) t;

            dataProfiles[i].apn = strdupReadString(p);
            dataProfiles[i].protocol = strdupReadString(p);
            status = p.readInt32(&t);
            dataProfiles[i].authType = (int) t;

            dataProfiles[i].user = strdupReadString(p);
            dataProfiles[i].password = strdupReadString(p);

            status = p.readInt32(&t);
            dataProfiles[i].type = (int) t;

            status = p.readInt32(&t);
            dataProfiles[i].maxConnsTime = (int) t;
            status = p.readInt32(&t);
            dataProfiles[i].maxConns = (int) t;
            status = p.readInt32(&t);
            dataProfiles[i].waitTime = (int) t;

            status = p.readInt32(&t);
            dataProfiles[i].enabled = (int) t;

            appendPrintBuf("%s [%d: profileId=%d, apn =%s, protocol =%s, authType =%d, \
                  user =%s, password =%s, type =%d, maxConnsTime =%d, maxConns =%d, \
                  waitTime =%d, enabled =%d]", printBuf, i, dataProfiles[i].profileId,
                  dataProfiles[i].apn, dataProfiles[i].protocol, dataProfiles[i].authType,
                  dataProfiles[i].user, dataProfiles[i].password, dataProfiles[i].type,
                  dataProfiles[i].maxConnsTime, dataProfiles[i].maxConns,
                  dataProfiles[i].waitTime, dataProfiles[i].enabled);
        }
        closeRequest;
        printRequest(pRI->token, pRI->pCI->requestNumber);

        if (status != NO_ERROR) {
            free(dataProfiles);
            free(dataProfilePtrs);
            goto invalid;
        }
        CALL_ONREQUEST(pRI->pCI->requestNumber,
                              dataProfilePtrs,
                              num * sizeof(RIL_DataProfileInfo *),
                              pRI, pRI->socket_id);

#ifdef MEMSET_FREED
        memset(dataProfiles, 0, num * sizeof(RIL_DataProfileInfo));
        memset(dataProfilePtrs, 0, num * sizeof(RIL_DataProfileInfo *));
#endif
        free(dataProfiles);
        free(dataProfilePtrs);
    } else {
        RIL_MtkDataProfileInfo *dataProfiles =
                (RIL_MtkDataProfileInfo *)calloc(num, sizeof(RIL_MtkDataProfileInfo));
        if (dataProfiles == NULL) {
            RLOGE("Memory allocation failed for request %s",
                    requestToString(pRI->pCI->requestNumber));
            return;
        }
        RIL_MtkDataProfileInfo **dataProfilePtrs =
                (RIL_MtkDataProfileInfo **)calloc(num, sizeof(RIL_MtkDataProfileInfo *));
        if (dataProfilePtrs == NULL) {
            RLOGE("Memory allocation failed for request %s",
                    requestToString(pRI->pCI->requestNumber));
            free(dataProfiles);
            return;
        }

        startRequest;
        for (int i = 0 ; i < num ; i++ ) {
            dataProfilePtrs[i] = &dataProfiles[i];

            status = p.readInt32(&t);
            dataProfiles[i].profileId = (int) t;

            dataProfiles[i].apn = strdupReadString(p);
            dataProfiles[i].protocol = strdupReadString(p);
            dataProfiles[i].roamingProtocol = strdupReadString(p);

            status = p.readInt32(&t);
            dataProfiles[i].authType = (int) t;

            dataProfiles[i].user = strdupReadString(p);
            dataProfiles[i].password = strdupReadString(p);

            status = p.readInt32(&t);
            dataProfiles[i].type = (int) t;

            status = p.readInt32(&t);
            dataProfiles[i].maxConnsTime = (int) t;
            status = p.readInt32(&t);
            dataProfiles[i].maxConns = (int) t;
            status = p.readInt32(&t);
            dataProfiles[i].waitTime = (int) t;

            status = p.readInt32(&t);
            dataProfiles[i].enabled = (int) t;

            status = p.readInt32(&t);
            dataProfiles[i].supportedTypesBitmask = (int) t;

            status = p.readInt32(&t);
            dataProfiles[i].bearerBitmask = (int) t;

            status = p.readInt32(&t);
            dataProfiles[i].mtu = (int) t;

            dataProfiles[i].mvnoType = strdupReadString(p);
            dataProfiles[i].mvnoMatchData = strdupReadString(p);

            status = p.readInt32(&t);
            dataProfiles[i].inactiveTimer = (int) t;

            appendPrintBuf("%s [%d: profileId=%d, apn =%s, protocol =%s, roamingProtocol =%s, \
                  authType =%d, user =%s, password =%s, type =%d, maxConnsTime =%d, \
                  maxConns =%d, waitTime =%d, enabled =%d, supportedTypesBitmask =%d, \
                  bearerBitmask = %d, mtu =%d, mvnoType =%s, mvnoMatchData =%s, inactiveTimer =%d]",
                  printBuf, i, dataProfiles[i].profileId,
                  dataProfiles[i].apn, dataProfiles[i].protocol,dataProfiles[i].roamingProtocol,
                  dataProfiles[i].authType, dataProfiles[i].user, dataProfiles[i].password,
                  dataProfiles[i].type, dataProfiles[i].maxConnsTime, dataProfiles[i].maxConns,
                  dataProfiles[i].waitTime, dataProfiles[i].enabled,
                  dataProfiles[i].supportedTypesBitmask, dataProfiles[i].bearerBitmask,
                  dataProfiles[i].mtu, dataProfiles[i].mvnoType, dataProfiles[i].mvnoMatchData,
                  dataProfiles[i].inactiveTimer);
        }
        closeRequest;
        printRequest(pRI->token, pRI->pCI->requestNumber);

        if (status != NO_ERROR) {
            free(dataProfiles);
            free(dataProfilePtrs);
            goto invalid;
        }
        CALL_ONREQUEST(pRI->pCI->requestNumber,
                              dataProfilePtrs,
                              num * sizeof(RIL_DataProfileInfo *),
                              pRI, pRI->socket_id);

#ifdef MEMSET_FREED
        memset(dataProfiles, 0, num * sizeof(RIL_DataProfileInfo_v15));
        memset(dataProfilePtrs, 0, num * sizeof(RIL_DataProfileInfo_v15 *));
#endif
        free(dataProfiles);
        free(dataProfilePtrs);
    }
    return;

invalid:
    invalidCommandBlock(pRI);
    return;
}

static void dispatchRadioCapability(Parcel &p, RequestInfo *pRI){
    RIL_RadioCapability rc;
    int32_t t;
    status_t status;

    memset (&rc, 0, sizeof(RIL_RadioCapability));

    status = p.readInt32(&t);
    rc.version = (int)t;
    if (status != NO_ERROR) {
        goto invalid;
    }

    status = p.readInt32(&t);
    rc.session= (int)t;
    if (status != NO_ERROR) {
        goto invalid;
    }

    status = p.readInt32(&t);
    rc.phase= (int)t;
    if (status != NO_ERROR) {
        goto invalid;
    }

    status = p.readInt32(&t);
    rc.rat = (int)t;
    if (status != NO_ERROR) {
        goto invalid;
    }

    status = readStringFromParcelInplace(p, rc.logicalModemUuid, sizeof(rc.logicalModemUuid));
    if (status != NO_ERROR) {
        goto invalid;
    }

    status = p.readInt32(&t);
    rc.status = (int)t;

    if (status != NO_ERROR) {
        goto invalid;
    }

    startRequest;
    appendPrintBuf("%s [version:%d, session:%d, phase:%d, rat:%d, \
            logicalModemUuid:%s, status:%d", printBuf, rc.version, rc.session,
            rc.phase, rc.rat, rc.logicalModemUuid, rc.session);

    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber,
                &rc,
                sizeof(RIL_RadioCapability),
                pRI, pRI->socket_id);
    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

/**
 * Callee expects const RIL_CarrierRestrictionsWithPriority *
 */
static void dispatchCarrierRestrictions(Parcel &p, RequestInfo *pRI) {
    RIL_CarrierRestrictionsWithPriority cr;
    RIL_Carrier * allowed_carriers = NULL;
    RIL_Carrier * excluded_carriers = NULL;
    int32_t t;
    status_t status;

    memset(&cr, 0, sizeof(RIL_CarrierRestrictionsWithPriority));

    if (s_callbacks.version < 14) {
        RLOGE("Unsuppoted RIL version %d, min version expected %d",
              s_callbacks.version, 14);
        RIL_onRequestComplete(pRI, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
        return;
    }

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }
    allowed_carriers = (RIL_Carrier *)calloc(t, sizeof(RIL_Carrier));
    if (allowed_carriers == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(pRI->pCI->requestNumber));
        goto exit;
    }
    cr.len_allowed_carriers = t;
    cr.allowed_carriers = allowed_carriers;

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }
    excluded_carriers = (RIL_Carrier *)calloc(t, sizeof(RIL_Carrier));
    if (excluded_carriers == NULL) {
        RLOGE("Memory allocation failed for request %s", requestToString(pRI->pCI->requestNumber));
        goto exit;
    }
    cr.len_excluded_carriers = t;
    cr.excluded_carriers = excluded_carriers;

    startRequest;
    appendPrintBuf("%s len_allowed_carriers:%d, len_excluded_carriers:%d,",
                   printBuf, cr.len_allowed_carriers, cr.len_excluded_carriers);

    appendPrintBuf("%s allowed_carriers:", printBuf);
    for (int32_t i = 0; i < cr.len_allowed_carriers; i++) {
        RIL_Carrier *p_cr = allowed_carriers + i;
        p_cr->mcc = strdupReadString(p);
        p_cr->mnc = strdupReadString(p);
        status = p.readInt32(&t);
        p_cr->match_type = static_cast<RIL_CarrierMatchType>(t);
        if (status != NO_ERROR) {
            goto invalid;
        }
        p_cr->match_data = strdupReadString(p);
        appendPrintBuf("%s [%d mcc:%s, mnc:%s, match_type:%d, match_data:%s],",
                       printBuf, i, p_cr->mcc, p_cr->mnc, p_cr->match_type, p_cr->match_data);
    }

    for (int32_t i = 0; i < cr.len_excluded_carriers; i++) {
        RIL_Carrier *p_cr = excluded_carriers + i;
        p_cr->mcc = strdupReadString(p);
        p_cr->mnc = strdupReadString(p);
        status = p.readInt32(&t);
        p_cr->match_type = static_cast<RIL_CarrierMatchType>(t);
        if (status != NO_ERROR) {
            goto invalid;
        }
        p_cr->match_data = strdupReadString(p);
        appendPrintBuf("%s [%d mcc:%s, mnc:%s, match_type:%d, match_data:%s],",
                       printBuf, i, p_cr->mcc, p_cr->mnc, p_cr->match_type, p_cr->match_data);
    }

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }
    cr.allowedCarriersPrioritized = t;

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }
    cr.simLockMultiSimPolicy = t;

    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber,
                &cr,
                sizeof(RIL_CarrierRestrictionsWithPriority),
                pRI, pRI->socket_id);

    goto exit;

invalid:
    invalidCommandBlock(pRI);
    RIL_onRequestComplete(pRI, RIL_E_INVALID_ARGUMENTS, NULL, 0);
exit:
    if (allowed_carriers != NULL) {
        free(allowed_carriers);
    }
    if (excluded_carriers != NULL) {
        free(excluded_carriers);
    }
    return;
}

// MTK-START: SIM GBA
static void dispatchSimAuth(Parcel &p, RequestInfo *pRI) {

    RIL_SimAuthStructure args;
    int32_t t = 0;
    status_t status;

    memset (&args, 0, sizeof(args));

    // Session ID
    status = p.readInt32(&t);
    args.sessionId = (int) t;

    // mode
    status = p.readInt32(&t);
    args.mode = (int) t;

    args.param1 = strdupReadString(p);
    args.param2 = strdupReadString(p);

    if(args.mode == 1) {
        //GBA
        status = p.readInt32(&t);
        args.tag = (int) t;
    }

    if (status != NO_ERROR) {
        goto invalid;
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &args, sizeof(args), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString(args.param1);
    memsetString(args.param2);
#endif
    if (args.param1 != NULL) {
        free(args.param1);
    }
    if (args.param2 != NULL) {
        free(args.param2);
    }

    return;
invalid:
    invalidCommandBlock(pRI);
    return;

}
// MTK-END

// FastDormancy
static void dispatchFdMode(Parcel &p, RequestInfo *pRI) {
    RIL_FdModeStructure args;
    status_t status;
    int t_value = 0;
    memset(&args, 0, sizeof(args));
    status = p.readInt32(&t_value);
    args.paramNumber = t_value;

    /* AT+EFD=<mode>[,<param1>[,<param2>]] */
    /* For all modes: but mode 0 & 1 only has one argument */
    if (args.paramNumber >= 1) {
        status = p.readInt32(&t_value);
        args.mode = t_value;
    }
    /* For mode 2 & 3 */
    if (args.paramNumber >= 2) {
        status = p.readInt32(&t_value);
        args.parameter1 = t_value;
    }
    /* Only mode 2 */
    if (args.paramNumber >=3) {
        status = p.readInt32(&t_value);
        args.parameter2 = t_value;
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &args, sizeof(args), pRI, pRI->socket_id);
}

// External SIM [Start]
static void dispatchVsimEvent(Parcel &p, RequestInfo *pRI) {
    RIL_VsimEvent args;
    int32_t t = 0;
    status_t status;

    memset (&args, 0, sizeof(args));

    // Transcation id
    status = p.readInt32(&t);
    args.transaction_id = (int) t;

    // Event id
    status = p.readInt32(&t);
    args.eventId = (int) t;

    // Sim type
    status = p.readInt32(&t);
    args.sim_type = (int) t;

    startRequest;
    appendPrintBuf("%scmd=0x%X,transaction_id=%d,eventId=%d,sim_type=%d", printBuf,
        args.transaction_id, args.eventId, args.sim_type);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    if (status != NO_ERROR) {
        goto invalid;
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &args, sizeof(args), pRI, pRI->socket_id);

    return;

invalid:
    invalidCommandBlock(pRI);
    return;
}

static void dispatchVsimOperationEvent(Parcel &p, RequestInfo *pRI) {
    RIL_VsimOperationEvent args;
    int32_t t = 0;
    status_t status;

    memset (&args, 0, sizeof(args));

    // Transcation id
    status = p.readInt32(&t);
    args.transaction_id = (int) t;

    // Event id
    status = p.readInt32(&t);
    args.eventId = (int) t;

    // Result
    status = p.readInt32(&t);
    args.result = (int) t;

    // Data length
    status = p.readInt32(&t);
    args.data_length = (int) t;

    // Data array
    args.data = (char *)strdupReadString(p);

    //LOGI ("[dispatchVsimOperationEvent]%d, %d, %d, %d, data:%s, response_addr:%p",
    //        args.transaction_id, args.eventId, args.result, args.data_length, args.data, &args);

    startRequest;
    appendPrintBuf("%scmd=0x%X,transaction_id=%d,eventId=%d,result=%d, data_len=%d", printBuf,
        args.transaction_id, args.eventId, args.result, args.data_length);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    if (status != NO_ERROR) {
        goto invalid;
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &args, sizeof(args), pRI, pRI->socket_id);
    if (args.data != NULL) {
        free(args.data);
        args.data = NULL;
    }
    return;

invalid:
    invalidCommandBlock(pRI);
    if (args.data != NULL) {
        free(args.data);
        args.data = NULL;
    }
    return;
}

static void dispatchNetworkScan(Parcel &p, RequestInfo *pRI) {
    RIL_NetworkScanRequest args;
    int32_t t;
    status_t status;

    memset (&args, 0, sizeof(args));

    status = p.readInt32(&t);
    args.type = (RIL_ScanType) t;

    status = p.readInt32(&t);
    args.interval = (int32_t) t;

    status = p.readInt32(&t);
    args.specifiers_length = (uint32_t) t;

    for (int i = 0 ; i < args.specifiers_length ; i++) {
        status = p.readInt32(&t);
        args.specifiers[i].radio_access_network = (RIL_RadioAccessNetworks) t;

        status = p.readInt32(&t);
        args.specifiers[i].bands_length = (uint32_t) t;

        for (int j = 0 ; j < args.specifiers[i].bands_length ; j++) {
            status = p.readInt32(&t);
            if (args.specifiers[i].radio_access_network == GERAN) {
                args.specifiers[i].bands.geran_bands[j] = (RIL_GeranBands) t;
            } else if (args.specifiers[i].radio_access_network == UTRAN) {
                args.specifiers[i].bands.utran_bands[j] = (RIL_UtranBands) t;
            } else {
                args.specifiers[i].bands.eutran_bands[j] = (RIL_EutranBands) t;
            }
        }

        status = p.readInt32(&t);
        args.specifiers[i].channels_length = (uint32_t) t;

        for (int k = 0 ; k < args.specifiers[i].channels_length ; k++) {
            status = p.readInt32(&t);
            args.specifiers[i].channels[k] = (uint32_t) t;
        }
    }

    startRequest;
    appendPrintBuf("%scmd=0x%X,type=%d,interval=%d,specifiers_length=%d", printBuf,
        args.type, args.interval, args.specifiers_length);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    if (status != NO_ERROR) {
        goto invalid;
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &args, sizeof(args), pRI, pRI->socket_id);

    return;

invalid:
    invalidCommandBlock(pRI);
    return;
}

static int responseVsimOperationEvent(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen != sizeof (RIL_VsimOperationEvent) ) {
        RLOGE("invalid response length was %d expected %d",
                (int)responselen, (int)sizeof (RIL_VsimOperationEvent));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RLOGD("responseVsimOperationEvent: enter");

    RIL_VsimOperationEvent *p_cur = (RIL_VsimOperationEvent *) response;
    p.writeInt32(p_cur->transaction_id);
    p.writeInt32(p_cur->eventId);
    p.writeInt32(p_cur->result);
    p.writeInt32(p_cur->data_length);
    writeStringToParcel(p, p_cur->data);

    RLOGD("responseVsimOperationEvent: data_length=%d", p_cur->data_length);

    startResponse;
    appendPrintBuf("%stransaction_id=%d,eventId=%d,datalen=%d", printBuf, p_cur->transaction_id,
            p_cur->eventId, p_cur->data_length);
    closeResponse;

    return 0;
}
// External SIM [Start]

static void dispatchLinkCapacityReportingCriteria(Parcel &p, RequestInfo *pRI) {
    status_t status;
    int t = 0;
    int num = 0;

    RIL_LinkCapacityReportingCriteria *data = (RIL_LinkCapacityReportingCriteria *) calloc(1,
            sizeof(RIL_LinkCapacityReportingCriteria));
    if (data == NULL) {
        RLOGE("setLinkCapacityReportingCriteria: data memory allocation failed for request %s",
                requestToString(pRI->pCI->requestNumber));
        return;
    }

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }
    data->hysteresisMs = (int)t;

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }
    data->hysteresisDlKbps = (int)t;

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }
    data->hysteresisUlKbps = (int)t;

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }
    num = (int)t;
    data->thresholdDlKbpsNumber = num > MAX_LCE_THRESHOLD_NUMBER ? MAX_LCE_THRESHOLD_NUMBER : num;
    for (int i = 0; i < num; i++) {
        status = p.readInt32(&t);
        if (status != NO_ERROR) {
            goto invalid;
        }
        if (i < MAX_LCE_THRESHOLD_NUMBER) {
            data->thresholdDlKbpsList[i] = (int)t;
        }
    }

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }
    num = (int)t;
    data->thresholdUlKbpsNumber = num > MAX_LCE_THRESHOLD_NUMBER ? MAX_LCE_THRESHOLD_NUMBER : num;
    for (int i = 0; i < num; i++) {
        status = p.readInt32(&t);
        if (status != NO_ERROR) {
            goto invalid;
        }
        if (i < MAX_LCE_THRESHOLD_NUMBER) {
            data->thresholdUlKbpsList[i] = (int)t;
        }
    }

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }
    data->accessNetwork = (int)t;

    CALL_ONREQUEST(pRI->pCI->requestNumber, data, sizeof(RIL_LinkCapacityReportingCriteria), pRI,
            pRI->socket_id);

    free(data);
    return;

invalid:
    RLOGE("setLinkCapacityReportingCriteria: invalidCommandBlock for request %s",
            requestToString(pRI->pCI->requestNumber));
    free(data);
    invalidCommandBlock(pRI);
    return;
}

static int responseLinkCapacityEstimate(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        RLOGE("responseLinkCapacityEstimate: invalid NULL response");
        return RIL_ERRNO_INVALID_RESPONSE;
    }
    if (responselen != sizeof(RIL_LinkCapacityEstimate)) {
        RLOGE("responseLinkCapacityEstimate: invalid response length %d, expecting length: %d",
                (int)responselen, (int)sizeof(RIL_LinkCapacityEstimate));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_LinkCapacityEstimate *p_cur = (RIL_LinkCapacityEstimate *)response;
    p.writeInt32(p_cur->downlinkCapacityKbps);
    p.writeInt32(p_cur->uplinkCapacityKbps);

    startResponse;
        appendPrintBuf("responseLinkCapacityEstimate: downlinkCapacityKbps %d uplinkCapacityKbps %d",
                       p_cur->downlinkCapacityKbps, p_cur->uplinkCapacityKbps);
    closeResponse;

    return 0;
}

static int
blockingWrite(int fd, const void *buffer, size_t len) {
    size_t writeOffset = 0;
    const uint8_t *toWrite;

    toWrite = (const uint8_t *)buffer;

    while (writeOffset < len) {
        ssize_t written;
        do {
            written = write (fd, toWrite + writeOffset,
                                len - writeOffset);
        } while (written < 0 && ((errno == EINTR) || (errno == EAGAIN)));

        if (written >= 0) {
            writeOffset += written;
        } else {   // written < 0
            RLOGE ("RIL Response: unexpected error on write errno:%d", errno);
            close(fd);
            return -1;
        }
    }
#if VDBG
    RLOGE("RIL Response bytes written:%d", writeOffset);
#endif
    return 0;
}

static int
sendResponseRaw (const void *data, size_t dataSize, RIL_SOCKET_ID socket_id) {
    int fd = s_ril_param_socket[socket_id].fdCommand;
    int ret;
    uint32_t header;
    pthread_mutex_t * writeMutexHook = &s_writeMutex[socket_id];

#if VDBG
    RLOGE("Send Response to %s", rilSocketIdToString(socket_id));
#endif

    if (fd < 0) {
        RLOGE("Send Response, but fd is incorrect");
        return -1;
    }

    if (dataSize > MAX_COMMAND_BYTES) {
        RLOGE("RIL: packet larger than %u (%u)",
                MAX_COMMAND_BYTES, (unsigned int )dataSize);

        return -1;
    }

    pthread_mutex_lock(writeMutexHook);

    header = htonl(dataSize);

    ret = blockingWrite(fd, (void *)&header, sizeof(header));

    if (ret < 0) {
        pthread_mutex_unlock(writeMutexHook);
        return ret;
    }

    ret = blockingWrite(fd, data, dataSize);

    if (ret < 0) {
        pthread_mutex_unlock(writeMutexHook);
        return ret;
    }

    pthread_mutex_unlock(writeMutexHook);

    return 0;
}

static int
sendResponse (Parcel &p, RIL_SOCKET_ID socket_id) {
    printResponse;
    return sendResponseRaw(p.data(), p.dataSize(), socket_id);
}

/** response is an int* pointing to an array of ints */

static int
responseInts(Parcel &p, void *response, size_t responselen) {
    int numInts;

    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }
    if (responselen % sizeof(int) != 0) {
        RLOGE("responseInts: invalid response length %d expected multiple of %d\n",
            (int)responselen, (int)sizeof(int));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    int *p_int = (int *) response;

    numInts = responselen / sizeof(int);
    p.writeInt32 (numInts);

    /* each int*/
    startResponse;
    for (int i = 0 ; i < numInts ; i++) {
        appendPrintBuf("%s%d,", printBuf, p_int[i]);
        p.writeInt32(p_int[i]);
    }
    removeLastChar;
    closeResponse;

    return 0;
}

// Response is an int or RIL_LastCallFailCauseInfo.
// Currently, only Shamu plans to use RIL_LastCallFailCauseInfo.
// TODO(yjl): Let all implementations use RIL_LastCallFailCauseInfo.
static int responseFailCause(Parcel &p, void *response, size_t responselen) {
    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen == sizeof(int)) {
      startResponse;
      int *p_int = (int *) response;
      appendPrintBuf("%s%d,", printBuf, p_int[0]);
      p.writeInt32(p_int[0]);
      removeLastChar;
      closeResponse;
    } else if (responselen == sizeof(RIL_LastCallFailCauseInfo)) {
      startResponse;
      RIL_LastCallFailCauseInfo *p_fail_cause_info = (RIL_LastCallFailCauseInfo *) response;
      appendPrintBuf("%s[cause_code=%d,vendor_cause=%s]", printBuf, p_fail_cause_info->cause_code,
                     p_fail_cause_info->vendor_cause);
      p.writeInt32(p_fail_cause_info->cause_code);
      writeStringToParcel(p, p_fail_cause_info->vendor_cause);
      removeLastChar;
      closeResponse;
    } else {
      RLOGE("responseFailCause: invalid response length %d expected an int or "
            "RIL_LastCallFailCauseInfo", (int)responselen);
      return RIL_ERRNO_INVALID_RESPONSE;
    }

    return 0;
}

/** response is a char **, pointing to an array of char *'s
    The parcel will begin with the version */
static int responseStringsWithVersion(int version, Parcel &p, void *response, size_t responselen) {
    p.writeInt32(version);
    return responseStrings(p, response, responselen);
}

/** response is a char **, pointing to an array of char *'s */
static int responseStrings(Parcel &p, void *response, size_t responselen) {
    int numStrings;

    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }
    if (responselen % sizeof(char *) != 0) {
        RLOGE("responseStrings: invalid response length %d expected multiple of %d\n",
            (int)responselen, (int)sizeof(char *));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (response == NULL) {
        p.writeInt32 (0);
    } else {
        char **p_cur = (char **) response;

        numStrings = responselen / sizeof(char *);
        p.writeInt32 (numStrings);

        /* each string*/
        startResponse;
        for (int i = 0 ; i < numStrings ; i++) {
            appendPrintBuf("%s%s,", printBuf, (char*)p_cur[i]);
            writeStringToParcel (p, p_cur[i]);
        }
        removeLastChar;
        closeResponse;
    }
    return 0;
}


/**
 * NULL strings are accepted
 * FIXME currently ignores responselen
 */
static int responseString(Parcel &p, void *response, size_t responselen) {
    /* one string only */
    startResponse;
    appendPrintBuf("%s%s", printBuf, (char*)response);
    closeResponse;

    writeStringToParcel(p, (const char *)response);

    return 0;
}

static int responseVoid(Parcel &p, void *response, size_t responselen) {
    startResponse;
    removeLastChar;
    return 0;
}

static int responseCallList(Parcel &p, void *response, size_t responselen) {
    int num;

    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof (RIL_Call *) != 0) {
        RLOGE("responseCallList: invalid response length %d expected multiple of %d\n",
            (int)responselen, (int)sizeof (RIL_Call *));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    startResponse;
    /* number of call info's */
    num = responselen / sizeof(RIL_Call *);
    p.writeInt32(num);

    for (int i = 0 ; i < num ; i++) {
        RIL_Call *p_cur = ((RIL_Call **) response)[i];
        /* each call info */
        p.writeInt32(p_cur->state);
        p.writeInt32(p_cur->index);
        p.writeInt32(p_cur->toa);
        p.writeInt32(p_cur->isMpty);
        p.writeInt32(p_cur->isMT);
        p.writeInt32(p_cur->als);
        p.writeInt32(p_cur->isVoice);
        p.writeInt32(p_cur->isVoicePrivacy);
        writeStringToParcel(p, p_cur->number);
        p.writeInt32(p_cur->numberPresentation);
        writeStringToParcel(p, p_cur->name);
        p.writeInt32(p_cur->namePresentation);
        // Remove when partners upgrade to version 3
        if ((s_callbacks.version < 3) || (p_cur->uusInfo == NULL || p_cur->uusInfo->uusData == NULL)) {
            p.writeInt32(0); /* UUS Information is absent */
        } else {
            RIL_UUS_Info *uusInfo = p_cur->uusInfo;
            p.writeInt32(1); /* UUS Information is present */
            p.writeInt32(uusInfo->uusType);
            p.writeInt32(uusInfo->uusDcs);
            p.writeInt32(uusInfo->uusLength);
            p.write(uusInfo->uusData, uusInfo->uusLength);
        }
        p.writeInt32(p_cur->speechCodec);
        appendPrintBuf("%s[id=%d,%s,toa=%d,",
            printBuf,
            p_cur->index,
            callStateToString(p_cur->state),
            p_cur->toa);
        appendPrintBuf("%s%s,%s,als=%d,%s,%s,",
            printBuf,
            (p_cur->isMpty)?"conf":"norm",
            (p_cur->isMT)?"mt":"mo",
            p_cur->als,
            (p_cur->isVoice)?"voc":"nonvoc",
            (p_cur->isVoicePrivacy)?"evp":"noevp");
        appendPrintBuf("%s%s,cli=%d,name='%s',%d,%d]",
            printBuf,
            p_cur->number,
            p_cur->numberPresentation,
            p_cur->name,
            p_cur->namePresentation,
            p_cur->speechCodec);
    }
    removeLastChar;
    closeResponse;

    return 0;
}

static int responseSMS(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen != sizeof (RIL_SMS_Response) ) {
        RLOGE("invalid response length %d expected %d",
                (int)responselen, (int)sizeof (RIL_SMS_Response));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_SMS_Response *p_cur = (RIL_SMS_Response *) response;

    p.writeInt32(p_cur->messageRef);
    writeStringToParcel(p, p_cur->ackPDU);
    p.writeInt32(p_cur->errorCode);

    startResponse;
    appendPrintBuf("%s%d,%s,%d", printBuf, p_cur->messageRef,
        (char*)p_cur->ackPDU, p_cur->errorCode);
    closeResponse;

    return 0;
}

static int responseDataCallListV4(Parcel &p, void *response, size_t responselen)
{
    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_Data_Call_Response_v4) != 0) {
        RLOGE("responseDataCallListV4: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_Data_Call_Response_v4));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    // Write version
    p.writeInt32(4);

    int num = responselen / sizeof(RIL_Data_Call_Response_v4);
    p.writeInt32(num);

    RIL_Data_Call_Response_v4 *p_cur = (RIL_Data_Call_Response_v4 *) response;
    startResponse;
    int i;
    for (i = 0; i < num; i++) {
        p.writeInt32(p_cur[i].cid);
        p.writeInt32(p_cur[i].active);
        writeStringToParcel(p, p_cur[i].type);
        // apn is not used, so don't send.
        writeStringToParcel(p, p_cur[i].address);
        appendPrintBuf("%s[cid=%d,%s,%s,%s],", printBuf,
            p_cur[i].cid,
            (p_cur[i].active==0)?"down":"up",
            (char*)p_cur[i].type,
            (char*)p_cur[i].address);
    }
    removeLastChar;
    closeResponse;

    return 0;
}

static int responseDataCallListV6(Parcel &p, void *response, size_t responselen)
{
    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_Data_Call_Response_v6) != 0) {
        RLOGE("responseDataCallListV6: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_Data_Call_Response_v6));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    // Write version
    p.writeInt32(6);

    int num = responselen / sizeof(RIL_Data_Call_Response_v6);
    p.writeInt32(num);

    RIL_Data_Call_Response_v6 *p_cur = (RIL_Data_Call_Response_v6 *) response;
    startResponse;
    int i;
    for (i = 0; i < num; i++) {
        p.writeInt32((int)p_cur[i].status);
        p.writeInt32(p_cur[i].suggestedRetryTime);
        p.writeInt32(p_cur[i].cid);
        p.writeInt32(p_cur[i].active);
        writeStringToParcel(p, p_cur[i].type);
        writeStringToParcel(p, p_cur[i].ifname);
        writeStringToParcel(p, p_cur[i].addresses);
        writeStringToParcel(p, p_cur[i].dnses);
        writeStringToParcel(p, p_cur[i].gateways);
        appendPrintBuf("%s[status=%d,retry=%d,cid=%d,%s,%s,%s,%s,%s,%s],", printBuf,
            p_cur[i].status,
            p_cur[i].suggestedRetryTime,
            p_cur[i].cid,
            (p_cur[i].active==0)?"down":"up",
            (char*)p_cur[i].type,
            (char*)p_cur[i].ifname,
            (char*)p_cur[i].addresses,
            (char*)p_cur[i].dnses,
            (char*)p_cur[i].gateways);
    }
    removeLastChar;
    closeResponse;

    return 0;
}

static int responseDataCallListV9(Parcel &p, void *response, size_t responselen)
{
    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_Data_Call_Response_v9) != 0) {
        RLOGE("responseDataCallListV9: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_Data_Call_Response_v9));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    // Write version
    p.writeInt32(10);

    int num = responselen / sizeof(RIL_Data_Call_Response_v9);
    p.writeInt32(num);

    RIL_Data_Call_Response_v9 *p_cur = (RIL_Data_Call_Response_v9 *) response;
    startResponse;
    int i;
    for (i = 0; i < num; i++) {
        p.writeInt32((int)p_cur[i].status);
        p.writeInt32(p_cur[i].suggestedRetryTime);
        p.writeInt32(p_cur[i].cid);
        p.writeInt32(p_cur[i].active);
        writeStringToParcel(p, p_cur[i].type);
        writeStringToParcel(p, p_cur[i].ifname);
        writeStringToParcel(p, p_cur[i].addresses);
        writeStringToParcel(p, p_cur[i].dnses);
        writeStringToParcel(p, p_cur[i].gateways);
        writeStringToParcel(p, p_cur[i].pcscf);
        appendPrintBuf("%s[status=%d,retry=%d,cid=%d,%s,%s,%s,%s,%s,%s,%s],", printBuf,
            p_cur[i].status,
            p_cur[i].suggestedRetryTime,
            p_cur[i].cid,
            (p_cur[i].active==0)?"down":"up",
            (char*)p_cur[i].type,
            (char*)p_cur[i].ifname,
            (char*)p_cur[i].addresses,
            (char*)p_cur[i].dnses,
            (char*)p_cur[i].gateways,
            (char*)p_cur[i].pcscf);
    }
    removeLastChar;
    closeResponse;

    return 0;
}

static int responseDataCallListV11(Parcel &p, void *response, size_t responselen) {
    if (response == NULL && responselen != 0) {
                RLOGE("invalid response: NULL");
                return RIL_ERRNO_INVALID_RESPONSE;
    }

    // M: [OD over ePDG]
    // remark AOSP
    //if (responselen % sizeof(RIL_Data_Call_Response_v11) != 0) {
    if (responselen % sizeof(MTK_RIL_Data_Call_Response_v11) != 0) {
        RLOGE("invalid response length %d expected multiple of %d",
        // M: [OD over ePDG]
        // remark AOSP
        //(int)responselen, (int)sizeof(RIL_Data_Call_Response_v11));
        (int)responselen, (int)sizeof(MTK_RIL_Data_Call_Response_v11));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    // Write version
    p.writeInt32(11);

    // M: [OD over ePDG]
    // remark AOSP
    //int num = responselen / sizeof(RIL_Data_Call_Response_v11);
    int num = responselen / sizeof(MTK_RIL_Data_Call_Response_v11);
    p.writeInt32(num);

    // M: [OD over ePDG]
    // remark AOSP
    //RIL_Data_Call_Response_v11 *p_cur = (RIL_Data_Call_Response_v11 *) response;
    MTK_RIL_Data_Call_Response_v11 *p_cur = (MTK_RIL_Data_Call_Response_v11 *) response;
    startResponse;
    int i;
    for (i = 0; i < num; i++) {
        p.writeInt32((int)p_cur[i].status);
        p.writeInt32(p_cur[i].suggestedRetryTime);
        p.writeInt32(p_cur[i].cid);
        p.writeInt32(p_cur[i].active);
        writeStringToParcel(p, p_cur[i].type);
        writeStringToParcel(p, p_cur[i].ifname);
        writeStringToParcel(p, p_cur[i].addresses);
        writeStringToParcel(p, p_cur[i].dnses);
        writeStringToParcel(p, p_cur[i].gateways);
        writeStringToParcel(p, p_cur[i].pcscf);
        p.writeInt32(p_cur[i].mtu);
        p.writeInt32(p_cur[i].rat);
        RLOGD("ril.cpp rat %d", p_cur[i].rat);
        appendPrintBuf("%s[status=%d,retry=%d,cid=%d,%s,%s,%s,%s,%s,%s,%s,mtu=%d,rat=%d],", printBuf,
        p_cur[i].status,
        p_cur[i].suggestedRetryTime,
        p_cur[i].cid,
        (p_cur[i].active==0)?"down":"up",
        (char*)p_cur[i].type,
        (char*)p_cur[i].ifname,
        (char*)p_cur[i].addresses,
        (char*)p_cur[i].dnses,
        (char*)p_cur[i].gateways,
        (char*)p_cur[i].pcscf,
        p_cur[i].mtu,
        p_cur[i].rat);
    }
    removeLastChar;
    closeResponse;

    return 0;
}

static int responseDataCallList(Parcel &p, void *response, size_t responselen)
{
    if (s_callbacks.version <= LAST_IMPRECISE_RIL_VERSION) {
        if (s_callbacks.version < 5) {
            RLOGD("responseDataCallList: v4");
            return responseDataCallListV4(p, response, responselen);
        } else if (responselen % sizeof(RIL_Data_Call_Response_v6) == 0) {
            return responseDataCallListV6(p, response, responselen);
        } else if (responselen % sizeof(RIL_Data_Call_Response_v9) == 0) {
            return responseDataCallListV9(p, response, responselen);
        } else {
            return responseDataCallListV11(p, response, responselen);
        }
    } else { // RIL version >= 13
        // M: [OD over ePDG]
        // remark AOSP
        //if (responselen % sizeof(RIL_Data_Call_Response_v11) != 0) {
        if (responselen % sizeof(MTK_RIL_Data_Call_Response_v11) != 0) {
            // M: [OD over ePDG]
            // remark AOSP
            //RLOGE("Data structure expected is RIL_Data_Call_Response_v11");
            RLOGE("Data structure expected is MTK_RIL_Data_Call_Response_v11");
            if (!isDebuggable()) {
                return RIL_ERRNO_INVALID_RESPONSE;
            } else {
                assert(0);
            }
        }
        return responseDataCallListV11(p, response, responselen);
    }
}

static int responseSetupDataCall(Parcel &p, void *response, size_t responselen)
{
    if (s_callbacks.version < 5) {
        return responseStringsWithVersion(s_callbacks.version, p, response, responselen);
    } else {
        return responseDataCallList(p, response, responselen);
    }
}


/// M: Ims Data Framework {@
static int responseSetupDedicateDataCall(Parcel &p, void *response, size_t responselen) {
    if (response == NULL && responselen != 0) {
        LOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_Dedicate_Data_Call_Struct) != 0) {
        LOGE("invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_Dedicate_Data_Call_Struct));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    int num = responselen / sizeof(RIL_Dedicate_Data_Call_Struct);
    p.writeInt32(num);

    RIL_Dedicate_Data_Call_Struct *p_cur = (RIL_Dedicate_Data_Call_Struct *) response;
    startResponse;
    int i=0, j=0, k=0, l=0;
    for (i = 0; i < num; i++) {
        p.writeInt32(p_cur[i].interfaceId);
        p.writeInt32(p_cur[i].primaryCid);
        p.writeInt32(p_cur[i].cid);
        p.writeInt32(p_cur[i].active);
        p.writeInt32(p_cur[i].signalingFlag);
        p.writeInt32(p_cur[i].bearerId);
        p.writeInt32(p_cur[i].failCause);
        //QOS
        p.writeInt32(p_cur[i].hasQos);
        if (p_cur[i].hasQos) {
            p.writeInt32(p_cur[i].qos.qci);
            p.writeInt32(p_cur[i].qos.dlGbr);
            p.writeInt32(p_cur[i].qos.ulGbr);
            p.writeInt32(p_cur[i].qos.dlMbr);
            p.writeInt32(p_cur[i].qos.ulMbr);
        }
        //tft
        p.writeInt32(p_cur[i].hasTft);
        if (p_cur[i].hasTft) {
            p.writeInt32(p_cur[i].tft.operation);
            p.writeInt32(p_cur[i].tft.pfNumber);
            for (j=0; j<p_cur[i].tft.pfNumber; j++) { //only response necessary length
                p.writeInt32(p_cur[i].tft.pfList[j].id);
                p.writeInt32(p_cur[i].tft.pfList[j].precedence);
                p.writeInt32(p_cur[i].tft.pfList[j].direction);
                p.writeInt32(p_cur[i].tft.pfList[j].networkPfIdentifier);
                p.writeInt32(p_cur[i].tft.pfList[j].bitmap);
                writeStringToParcel(p, p_cur[i].tft.pfList[j].address);
                writeStringToParcel(p, p_cur[i].tft.pfList[j].mask);
                p.writeInt32(p_cur[i].tft.pfList[j].protocolNextHeader);
                p.writeInt32(p_cur[i].tft.pfList[j].localPortLow);
                p.writeInt32(p_cur[i].tft.pfList[j].localPortHigh);
                p.writeInt32(p_cur[i].tft.pfList[j].remotePortLow);
                p.writeInt32(p_cur[i].tft.pfList[j].remotePortHigh);
                p.writeInt32(p_cur[i].tft.pfList[j].spi);
                p.writeInt32(p_cur[i].tft.pfList[j].tos);
                p.writeInt32(p_cur[i].tft.pfList[j].tosMask);
                p.writeInt32(p_cur[i].tft.pfList[j].flowLabel);
            }
            //TftParameter
            p.writeInt32(p_cur[i].tft.tftParameter.linkedPfNumber);
            for (j=0; j<p_cur[i].tft.tftParameter.linkedPfNumber; j++) //only response necessary length
                p.writeInt32(p_cur[i].tft.tftParameter.linkedPfList[j]);

            p.writeInt32(p_cur[i].tft.tftParameter.authtokenFlowIdNumber);
            for (j=0; j<p_cur[i].tft.tftParameter.authtokenFlowIdNumber; j++) {
                p.writeInt32(p_cur[i].tft.tftParameter.authtokenFlowIdList[k].authTokenNumber);
                for (k=0; k<p_cur[i].tft.tftParameter.authtokenFlowIdList[k].authTokenNumber; k++)
                    p.writeInt32(p_cur[i].tft.tftParameter.authtokenFlowIdList[j].authTokenList[k]);

                p.writeInt32(p_cur[i].tft.tftParameter.authtokenFlowIdList[k].flowIdNumber);
                for (k=0; k<p_cur[i].tft.tftParameter.authtokenFlowIdList[k].flowIdNumber; k++) {
                    for (l=0; l<4; l++)
                        p.writeInt32(p_cur[i].tft.tftParameter.authtokenFlowIdList[j].flowIdList[k][l]);
                }
            }
        }

        //PCSCF
        p.writeInt32(p_cur[i].hasPcscf);
        if (p_cur[i].hasPcscf)
            writeStringToParcel(p, p_cur[i].pcscf);
    }
    removeLastChar;
    closeResponse;

    return 0;
}

/// M: eMBMS feature
static int responseLocalEmbmsEnable(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        LOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_EMBMS_LocalEnableResp) != 0) {
        RLOGE("responseLocalEmbmsEnable: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_EMBMS_LocalEnableResp));
        assert(false);
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_EMBMS_LocalEnableResp *p_cur = (RIL_EMBMS_LocalEnableResp *) response;
    p.writeInt32(p_cur->trans_id);
    p.writeInt32(p_cur->response);
    p.writeInt32(p_cur->interface_index_valid);
    p.writeInt32(p_cur->interface_index);
    writeStringToParcel(p, p_cur->interface_name);

    startResponse;
    appendPrintBuf("%s%d,%d,%d,%d, %s", printBuf, p_cur->trans_id, p_cur->response,
        p_cur->interface_index_valid, p_cur->interface_index, p_cur->interface_name);
    closeResponse;
    return 0;
}

static int responseLocalEmbmsDisable(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        LOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_EMBMS_LocalDisableResp) != 0) {
        RLOGE("responseLocalEmbmsDisable: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_EMBMS_LocalDisableResp));
        assert(false);
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_EMBMS_LocalDisableResp *p_cur = (RIL_EMBMS_LocalDisableResp *) response;
    p.writeInt32(p_cur->trans_id);
    p.writeInt32(p_cur->response);
    startResponse;
    appendPrintBuf("%s%d,%d", printBuf, p_cur->trans_id, p_cur->response);
    closeResponse;
    return 0;
}

static int responseLocalEmbmsNetworkTime(Parcel &p, void *response, size_t responselen) {
     if (response == NULL) {
        LOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_EMBMS_LocalGetTimeResp) != 0) {
        RLOGE("responseLocalEmbmsNetworkTime: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_EMBMS_LocalGetTimeResp));
        assert(false);
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_EMBMS_LocalGetTimeResp *p_cur = (RIL_EMBMS_LocalGetTimeResp *)response;
    p.writeInt32(p_cur->trans_id);
    p.writeInt32(p_cur->response);
    p.writeInt64(p_cur->milli_sec);
    p.writeInt32(p_cur->day_light_saving_valid);
    p.writeInt32(p_cur->day_light_saving);
    p.writeInt32(p_cur->leap_seconds_valid);
    p.writeInt32(p_cur->leap_seconds);
    p.writeInt32(p_cur->local_time_offset_valid);
    p.writeInt32(p_cur->local_time_offset);

    startResponse;
    appendPrintBuf("%s%d,%d,%lu,%d,%d,%d,%d,%d,%d", printBuf, p_cur->trans_id, p_cur->response
        , p_cur->milli_sec, p_cur->day_light_saving_valid, p_cur->day_light_saving
        , p_cur->leap_seconds_valid, p_cur->leap_seconds, p_cur->local_time_offset_valid
        , p_cur->local_time_offset);
    closeResponse;

    return 0;
}

static void dispatchLocalEmbmsStartSessionInfo(Parcel &p, RequestInfo *pRI) {
    RIL_EMBMS_LocalStartSessionReq args;
    int32_t t;
    status_t status;

    memset (&args, 0, sizeof(args));

    status = p.readInt32(&args.trans_id);
    LOGD("dispatchLocalEmbmsStartSessionInfo, trans_id = %d", args.trans_id);

    startRequest;
    appendPrintBuf("%s,trans_id=%d", printBuf, args.trans_id);

    char* tmgi = strdupReadString(p);
    LOGD("dispatchLocalEmbmsStartSessionInfo, tmgi = [%s]", tmgi);
    strncpy(args.tmgi, tmgi, EMBMS_MAX_LEN_TMGI);
    appendPrintBuf("%s,tmgi=%s", printBuf, args.tmgi);
    free(tmgi);

    status = p.readInt32(&t);
    args.session_id = (int32_t)t;
    LOGD("dispatchLocalEmbmsStartSessionInfo, session_id = %d", args.session_id);
    appendPrintBuf("%s,session_id=%d", printBuf, args.session_id);

    status = p.readInt32(&t);
    args.earfcnlist_count = (uint32_t)t;
    LOGD("dispatchLocalEmbmsStartSessionInfo, earfcnlist_count = %d", args.earfcnlist_count);
    appendPrintBuf("%s,earfcnlist_count=%d", printBuf, args.earfcnlist_count);

    for (uint32_t i = 0 ; i < args.earfcnlist_count ; i++) {
        status = p.readInt32(&t);
        if (i < EMBMS_MAX_NUM_FREQ) {
            args.earfcnlist[i] = t;
            LOGD("earfcnlist[%d]= %d", i, args.earfcnlist[i]);
            appendPrintBuf("%s,earfcnlist=%d", printBuf, args.earfcnlist[i]);
        } else {
            LOGD("earfcnlist[%d] out of index", i);
        }
    }

    status = p.readInt32(&t);
    args.saiList_count = (uint32_t)t;
    LOGD("dispatchLocalEmbmsStartSessionInfo, saiList_count = %d", args.saiList_count);
    appendPrintBuf("%s,saiList_count=%d", printBuf, args.saiList_count);

    for (uint32_t i = 0 ; i < args.saiList_count ; i++) {
        status = p.readInt32(&t);
        if (i < EMBMS_MAX_NUM_SAI) {
            args.saiList[i] = t;
            LOGD("saiList[%d]= %d", i, args.saiList[i]);
            appendPrintBuf("%s,saiList=%d", printBuf, args.saiList[i]);
        } else {
            LOGD("saiList[%d] out of index", i);
        }
    }

    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    if (status != NO_ERROR) {
        goto invalid;
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &args, sizeof(args), pRI, pRI->socket_id);

    return;

invalid:
    invalidCommandBlock(pRI);
    return;
}

static void dispatchLocalEmbmsStopSessionInfo(Parcel &p, RequestInfo *pRI) {
    RIL_EMBMS_LocalStopSessionReq args;
    /*
    int32_t     stop_type;
    int32_t     trans_id;
    char        tmgi[EMBMS_MAX_LEN_TMGI+1];
    int32_t     session_id;
    */
    int32_t t = 0;
    status_t status;

    memset (&args, 0, sizeof(args));

    startRequest;
    status = p.readInt32(&t);
    args.stop_type = (int32_t)t;
    LOGD("dispatchLocalEmbmsStopSessionInfo, status:%d, stop_type%d", status, args.stop_type);

    status = p.readInt32(&t);
    args.trans_id = (int32_t)t;
    LOGD("dispatchLocalEmbmsStopSessionInfo, status:%d, trans_id%d", status, args.trans_id);

    char* tmgi = strdupReadString(p);
    LOGD("dispatchLocalEmbmsStartSessionInfo, tmgi = [%s]", tmgi);
    strncpy(args.tmgi, tmgi, EMBMS_MAX_LEN_TMGI);
    appendPrintBuf("%s,tmgi=%s", printBuf, args.tmgi);
    free(tmgi);

    status = p.readInt32(&t);
    args.session_id = (int32_t)t;
    LOGD("dispatchLocalEmbmsStopSessionInfo, status:%d, session_id%d", status, args.session_id);

    appendPrintBuf("%s,stop_type=%d, trans_id=%d, tmgi=%s, session_id=%d,",
        printBuf, args.stop_type, args.trans_id, args.tmgi, args.session_id);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    if (status != NO_ERROR) {
        goto invalid;
    }

    CALL_ONREQUEST(pRI->pCI->requestNumber, &args, sizeof(args), pRI, pRI->socket_id);

    return;

invalid:
    invalidCommandBlock(pRI);
    return;
}

static int responseLocalEmbmsSessionInfo(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        LOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_EMBMS_LocalStartSessionResp) != 0) {
        LOGE("responseLocalEmbmsSessionInfo: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_EMBMS_LocalStartSessionResp));
        assert(false);
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    startResponse;
    RIL_EMBMS_LocalStartSessionResp *p_cur = (RIL_EMBMS_LocalStartSessionResp *)response;
    p.writeInt32(p_cur->trans_id);
    p.writeInt32(p_cur->response);
    p.writeInt32(p_cur->tmgi_info_valid);
    LOGD("responseLocalEmbmsSessionInfo trans_id=%d, response=%d, tmgi_info_valid=%d",
                    p_cur->trans_id, p_cur->response, p_cur->tmgi_info_valid);
    appendPrintBuf("%s%d,%d,%d", printBuf, p_cur->trans_id,
        p_cur->response, p_cur->tmgi_info_valid);

    writeStringToParcel(p, p_cur->tmgi);

    appendPrintBuf("%s,%s", printBuf, p_cur->tmgi);

    closeResponse;
    return 0;
}

static int responseLocalEmbmsOosNotify(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        LOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_EMBMS_LocalOosNotify) != 0) {
        LOGE("responseLocalEmbmsOosNotify: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_EMBMS_LocalOosNotify));
        assert(false);
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    startResponse;
    RIL_EMBMS_LocalOosNotify *p_cur = (RIL_EMBMS_LocalOosNotify *)response;
    p.writeInt32(p_cur->reason);
    p.writeInt32(p_cur->tmgi_info_count);
    for (uint32_t i = 0; i < p_cur->tmgi_info_count; i++) {
        writeStringToParcel(p, p_cur->tmgix[i]);
    }
    LOGD("responseLocalEmbmsOosNotify reason=%d, tmgi_info_count=%d",
                    p_cur->reason, p_cur->tmgi_info_count);
    appendPrintBuf("%s%d,%d", printBuf, p_cur->reason,
        p_cur->tmgi_info_count);

    closeResponse;
    return 0;
}

static int responseLocalEmbmsSaiNotify(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        LOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_EMBMS_LocalSaiNotify) != 0) {
        LOGE("responseLocalEmbmsSaiNotify: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_EMBMS_LocalSaiNotify));
        assert(false);
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    startResponse;
    RIL_EMBMS_LocalSaiNotify *p_cur = (RIL_EMBMS_LocalSaiNotify *)response;
    // cf_total
    p.writeInt32(p_cur->cf_total);

    // curFreqData
    for (uint32_t i = 0; i < p_cur->cf_total; i++) {
        p.writeInt32(p_cur->curFreqData[i]);
    }

    // csai_total
    p.writeInt32(p_cur->csai_total);

    // curSaiData
    for (uint32_t i = 0; i < p_cur->csai_total; i++) {
        p.writeInt32(p_cur->curSaiData[i]);
    }

    // nf_total
    p.writeInt32(p_cur->nf_total);

    // neiFreqData
    for (uint32_t i = 0; i < p_cur->nf_total; i++) {
        p.writeInt32(p_cur->neiFreqData[i]);
    }

    // nsai_count_per_group
    for (uint32_t i = 0; i < p_cur->nf_total; i++) {
        p.writeInt32(p_cur->nsai_count_per_group[i]);
    }

    // nsai_total
    p.writeInt32(p_cur->nsai_total);

    // neiSaiData
    for (uint32_t i = 0; i < p_cur->nsai_total; i++) {
        p.writeInt32(p_cur->neiSaiData[i]);
    }
    LOGD("responseLocalEmbmsSaiNotify cf_total=%d, csai_total=%d, nf_total=%d, nsai_total=%d",
                    p_cur->cf_total, p_cur->csai_total,
                    p_cur->nf_total, p_cur->nsai_total);
    appendPrintBuf("%s%d,%d,%d, %d", printBuf, p_cur->cf_total,
        p_cur->csai_total, p_cur->nf_total, p_cur->nsai_total);

    closeResponse;
    return 0;
}

static int responseLocalEmbmsSessionNotify(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        LOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_EMBMS_LocalSessionNotify) != 0) {
        LOGE("responseLocalEmbmsSessionNotify: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_EMBMS_LocalSessionNotify));
        assert(false);
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    startResponse;
    RIL_EMBMS_LocalSessionNotify *p_cur = (RIL_EMBMS_LocalSessionNotify *)response;
    p.writeInt32(p_cur->tmgi_info_count);
    for (uint32_t i = 0; i < p_cur->tmgi_info_count; i++) {
        writeStringToParcel(p, p_cur->tmgix[i]);
        writeStringToParcel(p, p_cur->session_idx[i]);
    }
    LOGD("responseLocalEmbmsSessionNotify tmgi_info_count=%d",
                    p_cur->tmgi_info_count);
    appendPrintBuf("%s%d", printBuf, p_cur->tmgi_info_count);

    closeResponse;
    return 0;
}

static int responseLocalEmbmsGetCoverageState(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        LOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_EMBMS_GetCoverageResp) != 0) {
        RLOGE("responseLocalEmbmsGetCoverageState: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_EMBMS_GetCoverageResp));
        assert(false);
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_EMBMS_GetCoverageResp *p_cur = (RIL_EMBMS_GetCoverageResp *) response;
    p.writeInt32(p_cur->trans_id);
    p.writeInt32(p_cur->response);
    p.writeInt32(p_cur->coverage_state_valid);
    p.writeInt32(p_cur->coverage_state);
    startResponse;
    appendPrintBuf("%s%d,%d,%d,%d", printBuf, p_cur->trans_id, p_cur->response,
        p_cur->coverage_state_valid, p_cur->coverage_state);
    closeResponse;
    return 0;
}
/// M: eMBMS end
static int responseRaw(Parcel &p, void *response, size_t responselen) {
    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL with responselen != 0");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    // The java code reads -1 size as null byte array
    if (response == NULL) {
        p.writeInt32(-1);
    } else {
        p.writeInt32(responselen);
        p.write(response, responselen);
    }

    return 0;
}


static int responseSIM_IO(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen != sizeof (RIL_SIM_IO_Response) ) {
        RLOGE("invalid response length was %d expected %d",
                (int)responselen, (int)sizeof (RIL_SIM_IO_Response));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_SIM_IO_Response *p_cur = (RIL_SIM_IO_Response *) response;
    p.writeInt32(p_cur->sw1);
    p.writeInt32(p_cur->sw2);
    writeStringToParcel(p, p_cur->simResponse);

    startResponse;
    appendPrintBuf("%ssw1=0x%X,sw2=0x%X,%s", printBuf, p_cur->sw1, p_cur->sw2,
        (char*)p_cur->simResponse);
    closeResponse;


    return 0;
}

static int responseCallForwards(Parcel &p, void *response, size_t responselen) {
    int num;

    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_CallForwardInfo *) != 0) {
        RLOGE("responseCallForwards: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_CallForwardInfo *));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    /* number of call info's */
    num = responselen / sizeof(RIL_CallForwardInfo *);
    p.writeInt32(num);

    startResponse;
    for (int i = 0 ; i < num ; i++) {
        RIL_CallForwardInfo *p_cur = ((RIL_CallForwardInfo **) response)[i];

        p.writeInt32(p_cur->status);
        p.writeInt32(p_cur->reason);
        p.writeInt32(p_cur->serviceClass);
        p.writeInt32(p_cur->toa);
        writeStringToParcel(p, p_cur->number);
        p.writeInt32(p_cur->timeSeconds);
        appendPrintBuf("%s[%s,reason=%d,cls=%d,toa=%d,%s,tout=%d],", printBuf,
            (p_cur->status==1)?"enable":"disable",
            p_cur->reason, p_cur->serviceClass, p_cur->toa,
            (char*)p_cur->number,
            p_cur->timeSeconds);
    }
    removeLastChar;
    closeResponse;

    return 0;
}

static int responseCallForwardsEx(Parcel &p, void *response, size_t responselen) {
    int num;

    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_CallForwardInfoEx *) != 0) {
        RLOGE("responseCallForwardsEx: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_CallForwardInfoEx *));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    /* number of call info's */
    num = responselen / sizeof(RIL_CallForwardInfoEx *);
    p.writeInt32(num);

    startResponse;
    for (int i = 0 ; i < num ; i++) {
        RIL_CallForwardInfoEx *p_cur = ((RIL_CallForwardInfoEx **) response)[i];

        p.writeInt32(p_cur->status);
        p.writeInt32(p_cur->reason);
        p.writeInt32(p_cur->serviceClass);
        p.writeInt32(p_cur->toa);
        writeStringToParcel(p, p_cur->number);
        p.writeInt32(p_cur->timeSeconds);

        writeStringToParcel(p, p_cur->timeSlotBegin);
        writeStringToParcel(p, p_cur->timeSlotEnd);

        appendPrintBuf("%s[%s,reason=%d,cls=%d,toa=%d,%s,tout=%d,timeSlot=%s,%s],", printBuf,
            (p_cur->status==1)?"enable":"disable",
            p_cur->reason, p_cur->serviceClass, p_cur->toa,
            (char*)p_cur->number,
            p_cur->timeSeconds,
            p_cur->timeSlotBegin,
            p_cur->timeSlotEnd);
    }
    removeLastChar;
    closeResponse;

    return 0;
}

static int responseSsn(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen != sizeof(RIL_SuppSvcNotification)) {
        RLOGE("invalid response length was %d expected %d",
                (int)responselen, (int)sizeof (RIL_SuppSvcNotification));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_SuppSvcNotification *p_cur = (RIL_SuppSvcNotification *) response;
    p.writeInt32(p_cur->notificationType);
    p.writeInt32(p_cur->code);
    p.writeInt32(p_cur->index);
    p.writeInt32(p_cur->type);
    writeStringToParcel(p, p_cur->number);

    startResponse;
    appendPrintBuf("%s%s,code=%d,id=%d,type=%d,%s", printBuf,
        (p_cur->notificationType==0)?"mo":"mt",
         p_cur->code, p_cur->index, p_cur->type,
        (char*)p_cur->number);
    closeResponse;

    return 0;
}

static int responseCellList(Parcel &p, void *response, size_t responselen) {
    int num;

    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof (RIL_NeighboringCell *) != 0) {
        RLOGE("responseCellList: invalid response length %d expected multiple of %d\n",
            (int)responselen, (int)sizeof (RIL_NeighboringCell *));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    startResponse;
    /* number of records */
    num = responselen / sizeof(RIL_NeighboringCell *);
    p.writeInt32(num);

    for (int i = 0 ; i < num ; i++) {
        RIL_NeighboringCell *p_cur = ((RIL_NeighboringCell **) response)[i];

        p.writeInt32(p_cur->rssi);
        writeStringToParcel (p, p_cur->cid);

        appendPrintBuf("%s[cid=%s,rssi=%d],", printBuf,
            p_cur->cid, p_cur->rssi);
    }
    removeLastChar;
    closeResponse;

    return 0;
}

/**
 * Marshall the signalInfoRecord into the parcel if it exists.
 */
static void marshallSignalInfoRecord(Parcel &p,
            RIL_CDMA_SignalInfoRecord &p_signalInfoRecord) {
    p.writeInt32(p_signalInfoRecord.isPresent);
    p.writeInt32(p_signalInfoRecord.signalType);
    p.writeInt32(p_signalInfoRecord.alertPitch);
    p.writeInt32(p_signalInfoRecord.signal);
}

static int responseCdmaInformationRecords(Parcel &p,
            void *response, size_t responselen) {
    int num;
    char* string8 = NULL;
    int buffer_lenght;
    RIL_CDMA_InformationRecord *infoRec;

    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen != sizeof (RIL_CDMA_InformationRecords)) {
        RLOGE("responseCdmaInformationRecords: invalid response length %d expected multiple of %d\n",
            (int)responselen, (int)sizeof (RIL_CDMA_InformationRecords *));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_CDMA_InformationRecords *p_cur =
                             (RIL_CDMA_InformationRecords *) response;
    num = MIN(p_cur->numberOfInfoRecs, RIL_CDMA_MAX_NUMBER_OF_INFO_RECS);

    startResponse;
    p.writeInt32(num);

    for (int i = 0 ; i < num ; i++) {
        infoRec = &p_cur->infoRec[i];
        p.writeInt32(infoRec->name);
        switch (infoRec->name) {
            case RIL_CDMA_DISPLAY_INFO_REC:
            case RIL_CDMA_EXTENDED_DISPLAY_INFO_REC:
                if (infoRec->rec.display.alpha_len >
                                         CDMA_ALPHA_INFO_BUFFER_LENGTH) {
                    RLOGE("invalid display info response length %d \
                          expected not more than %d\n",
                         (int)infoRec->rec.display.alpha_len,
                         CDMA_ALPHA_INFO_BUFFER_LENGTH);
                    return RIL_ERRNO_INVALID_RESPONSE;
                }
                string8 = (char*) calloc(infoRec->rec.display.alpha_len + 1, sizeof(char));
                if (string8 == NULL) {
                    RLOGE("Memory allocation failed for responseCdmaInformationRecords");
                    closeRequest;
                    return RIL_ERRNO_NO_MEMORY;
                }
                for (int i = 0 ; i < infoRec->rec.display.alpha_len ; i++) {
                    string8[i] = infoRec->rec.display.alpha_buf[i];
                }
                string8[(int)infoRec->rec.display.alpha_len] = '\0';
                writeStringToParcel(p, (const char*)string8);
                free(string8);
                string8 = NULL;
                break;
            case RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC:
            case RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC:
            case RIL_CDMA_CONNECTED_NUMBER_INFO_REC:
                if (infoRec->rec.number.len > CDMA_NUMBER_INFO_BUFFER_LENGTH) {
                    RLOGE("invalid display info response length %d \
                          expected not more than %d\n",
                         (int)infoRec->rec.number.len,
                         CDMA_NUMBER_INFO_BUFFER_LENGTH);
                    return RIL_ERRNO_INVALID_RESPONSE;
                }
                string8 = (char*) calloc(infoRec->rec.number.len + 1, sizeof(char));
                if (string8 == NULL) {
                    RLOGE("Memory allocation failed for responseCdmaInformationRecords");
                    closeRequest;
                    return RIL_ERRNO_NO_MEMORY;
                }
                for (int i = 0 ; i < infoRec->rec.number.len; i++) {
                    string8[i] = infoRec->rec.number.buf[i];
                }
                string8[(int)infoRec->rec.number.len] = '\0';
                writeStringToParcel(p, (const char*)string8);
                free(string8);
                string8 = NULL;
                p.writeInt32(infoRec->rec.number.number_type);
                p.writeInt32(infoRec->rec.number.number_plan);
                p.writeInt32(infoRec->rec.number.pi);
                p.writeInt32(infoRec->rec.number.si);
                break;
            case RIL_CDMA_SIGNAL_INFO_REC:
                p.writeInt32(infoRec->rec.signal.isPresent);
                p.writeInt32(infoRec->rec.signal.signalType);
                p.writeInt32(infoRec->rec.signal.alertPitch);
                p.writeInt32(infoRec->rec.signal.signal);

                appendPrintBuf("%sisPresent=%X, signalType=%X, \
                                alertPitch=%X, signal=%X, ",
                   printBuf, (int)infoRec->rec.signal.isPresent,
                   (int)infoRec->rec.signal.signalType,
                   (int)infoRec->rec.signal.alertPitch,
                   (int)infoRec->rec.signal.signal);
                removeLastChar;
                break;
            case RIL_CDMA_REDIRECTING_NUMBER_INFO_REC:
                if (infoRec->rec.redir.redirectingNumber.len >
                                              CDMA_NUMBER_INFO_BUFFER_LENGTH) {
                    RLOGE("invalid display info response length %d \
                          expected not more than %d\n",
                         (int)infoRec->rec.redir.redirectingNumber.len,
                         CDMA_NUMBER_INFO_BUFFER_LENGTH);
                    return RIL_ERRNO_INVALID_RESPONSE;
                }
                string8 = (char*) calloc(infoRec->rec.redir.redirectingNumber.len + 1,
                        sizeof(char));
                if (string8 == NULL) {
                    RLOGE("Memory allocation failed for responseCdmaInformationRecords");
                    closeRequest;
                    return RIL_ERRNO_NO_MEMORY;
                }
                for (int i = 0;
                         i < infoRec->rec.redir.redirectingNumber.len;
                         i++) {
                    string8[i] = infoRec->rec.redir.redirectingNumber.buf[i];
                }
                string8[(int)infoRec->rec.redir.redirectingNumber.len] = '\0';
                writeStringToParcel(p, (const char*)string8);
                free(string8);
                string8 = NULL;
                p.writeInt32(infoRec->rec.redir.redirectingNumber.number_type);
                p.writeInt32(infoRec->rec.redir.redirectingNumber.number_plan);
                p.writeInt32(infoRec->rec.redir.redirectingNumber.pi);
                p.writeInt32(infoRec->rec.redir.redirectingNumber.si);
                p.writeInt32(infoRec->rec.redir.redirectingReason);
                break;
            case RIL_CDMA_LINE_CONTROL_INFO_REC:
                p.writeInt32(infoRec->rec.lineCtrl.lineCtrlPolarityIncluded);
                p.writeInt32(infoRec->rec.lineCtrl.lineCtrlToggle);
                p.writeInt32(infoRec->rec.lineCtrl.lineCtrlReverse);
                p.writeInt32(infoRec->rec.lineCtrl.lineCtrlPowerDenial);

                appendPrintBuf("%slineCtrlPolarityIncluded=%d, \
                                lineCtrlToggle=%d, lineCtrlReverse=%d, \
                                lineCtrlPowerDenial=%d, ", printBuf,
                       (int)infoRec->rec.lineCtrl.lineCtrlPolarityIncluded,
                       (int)infoRec->rec.lineCtrl.lineCtrlToggle,
                       (int)infoRec->rec.lineCtrl.lineCtrlReverse,
                       (int)infoRec->rec.lineCtrl.lineCtrlPowerDenial);
                removeLastChar;
                break;
            case RIL_CDMA_T53_CLIR_INFO_REC:
                p.writeInt32((int)(infoRec->rec.clir.cause));

                appendPrintBuf("%scause%d", printBuf, infoRec->rec.clir.cause);
                removeLastChar;
                break;
            case RIL_CDMA_T53_AUDIO_CONTROL_INFO_REC:
                p.writeInt32(infoRec->rec.audioCtrl.upLink);
                p.writeInt32(infoRec->rec.audioCtrl.downLink);

                appendPrintBuf("%supLink=%d, downLink=%d, ", printBuf,
                        infoRec->rec.audioCtrl.upLink,
                        infoRec->rec.audioCtrl.downLink);
                removeLastChar;
                break;
            case RIL_CDMA_T53_RELEASE_INFO_REC:
                // TODO(Moto): See David Krause, he has the answer:)
                RLOGE("RIL_CDMA_T53_RELEASE_INFO_REC: return INVALID_RESPONSE");
                return RIL_ERRNO_INVALID_RESPONSE;
            default:
                RLOGE("Incorrect name value");
                return RIL_ERRNO_INVALID_RESPONSE;
        }
    }
    closeResponse;

    return 0;
}

static void responseRilSignalStrengthV5(Parcel &p, RIL_SignalStrength_v10 *p_cur) {
    p.writeInt32(p_cur->GW_SignalStrength.signalStrength);
    p.writeInt32(p_cur->GW_SignalStrength.bitErrorRate);
    p.writeInt32(p_cur->CDMA_SignalStrength.dbm);
    p.writeInt32(p_cur->CDMA_SignalStrength.ecio);
    p.writeInt32(p_cur->EVDO_SignalStrength.dbm);
    p.writeInt32(p_cur->EVDO_SignalStrength.ecio);
    p.writeInt32(p_cur->EVDO_SignalStrength.signalNoiseRatio);
}

static void responseRilSignalStrengthV6Extra(Parcel &p, RIL_SignalStrength_v10 *p_cur) {
    /*
     * Fixup LTE for backwards compatibility
     */
    // signalStrength: -1 -> 99
    if (p_cur->LTE_SignalStrength.signalStrength == -1) {
        p_cur->LTE_SignalStrength.signalStrength = 99;
    }
    // rsrp: -1 -> INT_MAX all other negative value to positive.
    // So remap here
    if (p_cur->LTE_SignalStrength.rsrp == -1) {
        p_cur->LTE_SignalStrength.rsrp = INT_MAX;
    } else if (p_cur->LTE_SignalStrength.rsrp < -1) {
        p_cur->LTE_SignalStrength.rsrp = -p_cur->LTE_SignalStrength.rsrp;
    }
    // rsrq: -1 -> INT_MAX
    if (p_cur->LTE_SignalStrength.rsrq == -1) {
        p_cur->LTE_SignalStrength.rsrq = INT_MAX;
    }
    // Not remapping rssnr is already using INT_MAX

    // cqi: -1 -> INT_MAX
    if (p_cur->LTE_SignalStrength.cqi == -1) {
        p_cur->LTE_SignalStrength.cqi = INT_MAX;
    }

    p.writeInt32(p_cur->LTE_SignalStrength.signalStrength);
    p.writeInt32(p_cur->LTE_SignalStrength.rsrp);
    p.writeInt32(p_cur->LTE_SignalStrength.rsrq);
    p.writeInt32(p_cur->LTE_SignalStrength.rssnr);
    p.writeInt32(p_cur->LTE_SignalStrength.cqi);
}

static void responseRilSignalStrengthV10(Parcel &p, RIL_SignalStrength_v10 *p_cur) {
    responseRilSignalStrengthV5(p, p_cur);
    responseRilSignalStrengthV6Extra(p, p_cur);
    p.writeInt32(p_cur->TD_SCDMA_SignalStrength.rscp);
}

static int responseRilSignalStrength(Parcel &p,
                    void *response, size_t responselen) {
    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_SignalStrength_v10 *p_cur;
    if (s_callbacks.version <= LAST_IMPRECISE_RIL_VERSION) {
        if (responselen >= sizeof (RIL_SignalStrength_v5)) {
            p_cur = ((RIL_SignalStrength_v10 *) response);

            responseRilSignalStrengthV5(p, p_cur);

            if (responselen >= sizeof (RIL_SignalStrength_v6)) {
                responseRilSignalStrengthV6Extra(p, p_cur);
                if (responselen >= sizeof (RIL_SignalStrength_v10)) {
                    p.writeInt32(p_cur->TD_SCDMA_SignalStrength.rscp);
                } else {
                    p.writeInt32(INT_MAX);
                }
            } else {
                p.writeInt32(99);
                p.writeInt32(INT_MAX);
                p.writeInt32(INT_MAX);
                p.writeInt32(INT_MAX);
                p.writeInt32(INT_MAX);
                p.writeInt32(INT_MAX);
            }
        } else {
            RLOGE("invalid response length");
            return RIL_ERRNO_INVALID_RESPONSE;
        }
    } else { // RIL version >= 13
        if (responselen % sizeof(RIL_SignalStrength_v10) != 0) {
            RLOGE("Data structure expected is RIL_SignalStrength_v10");
            if (!isDebuggable()) {
                return RIL_ERRNO_INVALID_RESPONSE;
            } else {
                assert(0);
            }
        }
        p_cur = ((RIL_SignalStrength_v10 *) response);
        responseRilSignalStrengthV10(p, p_cur);
    }
    startResponse;
    appendPrintBuf("%s[signalStrength=%d,bitErrorRate=%d,\
            CDMA_SS.dbm=%d,CDMA_SSecio=%d,\
            EVDO_SS.dbm=%d,EVDO_SS.ecio=%d,\
            EVDO_SS.signalNoiseRatio=%d,\
            LTE_SS.signalStrength=%d,LTE_SS.rsrp=%d,LTE_SS.rsrq=%d,\
            LTE_SS.rssnr=%d,LTE_SS.cqi=%d,TDSCDMA_SS.rscp=%d]",
            printBuf,
            p_cur->GW_SignalStrength.signalStrength,
            p_cur->GW_SignalStrength.bitErrorRate,
            p_cur->CDMA_SignalStrength.dbm,
            p_cur->CDMA_SignalStrength.ecio,
            p_cur->EVDO_SignalStrength.dbm,
            p_cur->EVDO_SignalStrength.ecio,
            p_cur->EVDO_SignalStrength.signalNoiseRatio,
            p_cur->LTE_SignalStrength.signalStrength,
            p_cur->LTE_SignalStrength.rsrp,
            p_cur->LTE_SignalStrength.rsrq,
            p_cur->LTE_SignalStrength.rssnr,
            p_cur->LTE_SignalStrength.cqi,
            p_cur->TD_SCDMA_SignalStrength.rscp);
    closeResponse;
    return 0;
}

static int responseCallRing(Parcel &p, void *response, size_t responselen) {
    if ((response == NULL) || (responselen == 0)) {
        return responseVoid(p, response, responselen);
    } else {
        return responseCdmaSignalInfoRecord(p, response, responselen);
    }
}

static int responseCdmaSignalInfoRecord(Parcel &p, void *response, size_t responselen) {
    if (response == NULL || responselen == 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen != sizeof (RIL_CDMA_SignalInfoRecord)) {
        RLOGE("invalid response length %d expected sizeof (RIL_CDMA_SignalInfoRecord) of %d\n",
            (int)responselen, (int)sizeof (RIL_CDMA_SignalInfoRecord));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    startResponse;

    RIL_CDMA_SignalInfoRecord *p_cur = ((RIL_CDMA_SignalInfoRecord *) response);
    marshallSignalInfoRecord(p, *p_cur);

    appendPrintBuf("%s[isPresent=%d,signalType=%d,alertPitch=%d\
              signal=%d]",
              printBuf,
              p_cur->isPresent,
              p_cur->signalType,
              p_cur->alertPitch,
              p_cur->signal);

    closeResponse;
    return 0;
}

static int responseCdmaCallWaiting(Parcel &p, void *response,
            size_t responselen) {
    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen < sizeof(RIL_CDMA_CallWaiting_v6)) {
        RLOGW("Upgrade to ril version %d\n", RIL_VERSION);
    }

    RIL_CDMA_CallWaiting_v6 *p_cur = ((RIL_CDMA_CallWaiting_v6 *) response);

    writeStringToParcel(p, p_cur->number);
    p.writeInt32(p_cur->numberPresentation);
    writeStringToParcel(p, p_cur->name);
    marshallSignalInfoRecord(p, p_cur->signalInfoRecord);

    if (s_callbacks.version <= LAST_IMPRECISE_RIL_VERSION) {
        if (responselen >= sizeof(RIL_CDMA_CallWaiting_v6)) {
            p.writeInt32(p_cur->number_type);
            p.writeInt32(p_cur->number_plan);
        } else {
            p.writeInt32(0);
            p.writeInt32(0);
        }
    } else { // RIL version >= 13
        if (responselen % sizeof(RIL_CDMA_CallWaiting_v6) != 0) {
            RLOGE("Data structure expected is RIL_CDMA_CallWaiting_v6");
            if (!isDebuggable()) {
                return RIL_ERRNO_INVALID_RESPONSE;
            } else {
                assert(0);
            }
        }
        p.writeInt32(p_cur->number_type);
        p.writeInt32(p_cur->number_plan);
    }

    startResponse;
    appendPrintBuf("%snumber=%s,numberPresentation=%d, name=%s,\
            signalInfoRecord[isPresent=%d,signalType=%d,alertPitch=%d\
            signal=%d,number_type=%d,number_plan=%d]",
            printBuf,
            p_cur->number,
            p_cur->numberPresentation,
            p_cur->name,
            p_cur->signalInfoRecord.isPresent,
            p_cur->signalInfoRecord.signalType,
            p_cur->signalInfoRecord.alertPitch,
            p_cur->signalInfoRecord.signal,
            p_cur->number_type,
            p_cur->number_plan);
    closeResponse;

    return 0;
}

static void responseSimRefreshV7(Parcel &p, void *response) {
    RIL_SimRefreshResponse_v7 *p_cur = ((RIL_SimRefreshResponse_v7 *) response);
    p.writeInt32(p_cur->result);
    p.writeInt32(p_cur->ef_id);
    writeStringToParcel(p, p_cur->aid);

    appendPrintBuf("%sresult=%d, ef_id=%d, aid=%s",
            printBuf,
            p_cur->result,
            p_cur->ef_id,
            p_cur->aid);

}

static int responseSimRefresh(Parcel &p, void *response, size_t responselen) {
    if (response == NULL && responselen != 0) {
        RLOGE("responseSimRefresh: invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    startResponse;
    if (s_callbacks.version <= LAST_IMPRECISE_RIL_VERSION) {
        if (s_callbacks.version >= 7) {
            responseSimRefreshV7(p, response);
        } else {
            int *p_cur = ((int *) response);
            p.writeInt32(p_cur[0]);
            p.writeInt32(p_cur[1]);
            writeStringToParcel(p, NULL);

            appendPrintBuf("%sresult=%d, ef_id=%d",
                    printBuf,
                    p_cur[0],
                    p_cur[1]);
        }
    } else { // RIL version >= 13
        if (responselen % sizeof(RIL_SimRefreshResponse_v7) != 0) {
            RLOGE("Data structure expected is RIL_SimRefreshResponse_v7");
            if (!isDebuggable()) {
                return RIL_ERRNO_INVALID_RESPONSE;
            } else {
                assert(0);
            }
        }
        responseSimRefreshV7(p, response);

    }
    closeResponse;

    return 0;
}

static int responseCellInfoListV6(Parcel &p, void *response, size_t responselen) {
    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_CellInfo) != 0) {
        RLOGE("responseCellInfoList: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_CellInfo));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    int num = responselen / sizeof(RIL_CellInfo);
    p.writeInt32(num);

    RIL_CellInfo *p_cur = (RIL_CellInfo *) response;
    startResponse;
    int i;
    for (i = 0; i < num; i++) {
        p_cur->timeStampType = RIL_TIMESTAMP_TYPE_OEM_RIL;
        p_cur->timeStamp = elapsedRealtime() * 1000000; // Time milliseconds since bootup, convert to nano seconds

        p.writeInt32((int)p_cur->cellInfoType);
        p.writeInt32(p_cur->registered);
        p.writeInt32(p_cur->timeStampType);
        p.writeInt64(p_cur->timeStamp);
        switch(p_cur->cellInfoType) {
            case RIL_CELL_INFO_TYPE_GSM: {
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.mcc);
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.mnc);
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.lac);
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.cid);
                p.writeInt32(p_cur->CellInfo.gsm.signalStrengthGsm.signalStrength);
                p.writeInt32(p_cur->CellInfo.gsm.signalStrengthGsm.bitErrorRate);
                break;
            }
            case RIL_CELL_INFO_TYPE_WCDMA: {
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.mcc);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.mnc);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.lac);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.cid);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.psc);
                p.writeInt32(p_cur->CellInfo.wcdma.signalStrengthWcdma.signalStrength);
                p.writeInt32(p_cur->CellInfo.wcdma.signalStrengthWcdma.bitErrorRate);
                break;
            }
            case RIL_CELL_INFO_TYPE_CDMA: {
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.networkId);
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.systemId);
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.basestationId);
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.longitude);
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.latitude);

                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthCdma.dbm);
                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthCdma.ecio);
                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthEvdo.dbm);
                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthEvdo.ecio);
                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio);
                break;
            }
            case RIL_CELL_INFO_TYPE_LTE: {
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.mcc);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.mnc);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.ci);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.pci);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.tac);

                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.signalStrength);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.rsrp);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.rsrq);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.rssnr);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.cqi);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.timingAdvance);
                break;
            }
            case RIL_CELL_INFO_TYPE_TD_SCDMA: {
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.mcc);
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.mnc);
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.lac);
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.cid);
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.cpid);
                p.writeInt32(p_cur->CellInfo.tdscdma.signalStrengthTdscdma.rscp);
                break;
            }
            case RIL_CELL_INFO_TYPE_NR:
            default: {
                break;
            }
        }
        p_cur += 1;
    }
    removeLastChar;
    closeResponse;

    return 0;
}

static int responseCellInfoListV12(Parcel &p, void *response, size_t responselen) {
    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof(RIL_CellInfo_v12) != 0) {
        RLOGE("responseCellInfoList: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_CellInfo_v12));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    int num = responselen / sizeof(RIL_CellInfo_v12);
    p.writeInt32(num);

    RIL_CellInfo_v12 *p_cur = (RIL_CellInfo_v12 *) response;
    startResponse;
    int i;
    for (i = 0; i < num; i++) {
        p.writeInt32((int)p_cur->cellInfoType);
        p.writeInt32(p_cur->registered);
        p.writeInt32(p_cur->timeStampType);
        p.writeInt64(p_cur->timeStamp);
        switch(p_cur->cellInfoType) {
            case RIL_CELL_INFO_TYPE_GSM: {
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.mcc);
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.mnc);
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.mnc_len);
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.lac);
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.cid);
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.arfcn);
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.bsic);
                p.writeInt32(p_cur->CellInfo.gsm.signalStrengthGsm.signalStrength);
                p.writeInt32(p_cur->CellInfo.gsm.signalStrengthGsm.bitErrorRate);
                p.writeInt32(p_cur->CellInfo.gsm.signalStrengthGsm.timingAdvance);
                // append operator name
                writeStringToParcel(p, (const char *)p_cur->CellInfo.gsm.cellIdentityGsm.operName.long_name);
                writeStringToParcel(p, (const char *)p_cur->CellInfo.gsm.cellIdentityGsm.operName.short_name);
                break;
            }
            case RIL_CELL_INFO_TYPE_WCDMA: {
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.mcc);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.mnc);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.mnc_len);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.lac);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.cid);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.psc);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.uarfcn);
                p.writeInt32(p_cur->CellInfo.wcdma.signalStrengthWcdma.signalStrength);
                p.writeInt32(p_cur->CellInfo.wcdma.signalStrengthWcdma.bitErrorRate);
                // append operator name
                writeStringToParcel(p, (const char *)p_cur->CellInfo.wcdma.cellIdentityWcdma.operName.long_name);
                writeStringToParcel(p, (const char *)p_cur->CellInfo.wcdma.cellIdentityWcdma.operName.short_name);
                break;
            }
            case RIL_CELL_INFO_TYPE_CDMA: {
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.networkId);
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.systemId);
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.basestationId);
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.longitude);
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.latitude);

                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthCdma.dbm);
                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthCdma.ecio);
                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthEvdo.dbm);
                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthEvdo.ecio);
                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio);
                // append operator name, unknown for now
                writeStringToParcel(p, (const char *)"unknown");
                writeStringToParcel(p, (const char *)"unknown");
                break;
            }
            case RIL_CELL_INFO_TYPE_LTE: {
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.mcc);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.mnc);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.mnc_len);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.ci);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.pci);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.tac);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.earfcn);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.bandwidth);

                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.signalStrength);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.rsrp);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.rsrq);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.rssnr);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.cqi);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.timingAdvance);
                // append operator name
                writeStringToParcel(p, (const char *)p_cur->CellInfo.lte.cellIdentityLte.operName.long_name);
                writeStringToParcel(p, (const char *)p_cur->CellInfo.lte.cellIdentityLte.operName.short_name);
                break;
            }
            case RIL_CELL_INFO_TYPE_TD_SCDMA: {
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.mcc);
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.mnc);
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.mnc_len);
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.lac);
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.cid);
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.cpid);
                p.writeInt32(p_cur->CellInfo.tdscdma.signalStrengthTdscdma.rscp);
                // append operator name
                writeStringToParcel(p, (const char *)p_cur->CellInfo.tdscdma.cellIdentityTdscdma.operName.long_name);
                writeStringToParcel(p, (const char *)p_cur->CellInfo.tdscdma.cellIdentityTdscdma.operName.short_name);
                break;
            }
            case RIL_CELL_INFO_TYPE_NR:
            default: {
                break;
            }
        }
        p_cur += 1;
    }
    removeLastChar;
    closeResponse;
    return 0;
}

static int responseCellInfoList(Parcel &p, void *response, size_t responselen)
{
    if (s_callbacks.version <= LAST_IMPRECISE_RIL_VERSION) {
        if (s_callbacks.version < 12) {
            RLOGD("responseCellInfoList: v6");
            return responseCellInfoListV6(p, response, responselen);
        } else {
            RLOGD("responseCellInfoList: v12");
            return responseCellInfoListV12(p, response, responselen);
        }
    } else { // RIL version >= 13
        if (responselen % sizeof(RIL_CellInfo_v12) != 0) {
            RLOGE("Data structure expected is RIL_CellInfo_v12");
            if (!isDebuggable()) {
                return RIL_ERRNO_INVALID_RESPONSE;
            } else {
                assert(0);
            }
        }
        return responseCellInfoListV12(p, response, responselen);
    }

    return 0;
}

static int responseHardwareConfig(Parcel &p, void *response, size_t responselen)
{
   if (response == NULL && responselen != 0) {
       RLOGE("invalid response: NULL");
       return RIL_ERRNO_INVALID_RESPONSE;
   }

   if (responselen % sizeof(RIL_HardwareConfig) != 0) {
       RLOGE("responseHardwareConfig: invalid response length %d expected multiple of %d",
          (int)responselen, (int)sizeof(RIL_HardwareConfig));
       return RIL_ERRNO_INVALID_RESPONSE;
   }

   int num = responselen / sizeof(RIL_HardwareConfig);
   int i;
   RIL_HardwareConfig *p_cur = (RIL_HardwareConfig *) response;

   p.writeInt32(num);
   startResponse;
   for (i = 0; i < num; i++) {
      switch (p_cur[i].type) {
         case RIL_HARDWARE_CONFIG_MODEM: {
            p.writeInt32(p_cur[i].type);
            writeStringToParcel(p, p_cur[i].uuid);
            p.writeInt32((int)p_cur[i].state);
            p.writeInt32(p_cur[i].cfg.modem.rilModel);
            p.writeInt32(p_cur[i].cfg.modem.rat);
            p.writeInt32(p_cur[i].cfg.modem.maxVoice);
            p.writeInt32(p_cur[i].cfg.modem.maxData);
            p.writeInt32(p_cur[i].cfg.modem.maxStandby);
            appendPrintBuf("%s modem: uuid=%s,state=%d,rilModel=%d,rat=%08x,maxV=%d,maxD=%d,maxS=%d", printBuf,
               p_cur[i].uuid, (int)p_cur[i].state, p_cur[i].cfg.modem.rilModel, p_cur[i].cfg.modem.rat,
               p_cur[i].cfg.modem.maxVoice, p_cur[i].cfg.modem.maxData, p_cur[i].cfg.modem.maxStandby);
            break;
         }
         case RIL_HARDWARE_CONFIG_SIM: {
            p.writeInt32(p_cur[i].type);
            writeStringToParcel(p, p_cur[i].uuid);
            p.writeInt32((int)p_cur[i].state);
            writeStringToParcel(p, p_cur[i].cfg.sim.modemUuid);
            appendPrintBuf("%s sim: uuid=%s,state=%d,modem-uuid=%s", printBuf,
               p_cur[i].uuid, (int)p_cur[i].state, p_cur[i].cfg.sim.modemUuid);
            break;
         }
      }
   }
   removeLastChar;
   closeResponse;
   return 0;
}

static int responseRadioCapability(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen != sizeof (RIL_RadioCapability) ) {
        RLOGE("invalid response length was %d expected %d",
                (int)responselen, (int)sizeof (RIL_SIM_IO_Response));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_RadioCapability *p_cur = (RIL_RadioCapability *) response;
    p.writeInt32(p_cur->version);
    p.writeInt32(p_cur->session);
    p.writeInt32(p_cur->phase);
    p.writeInt32(p_cur->rat);
    writeStringToParcel(p, p_cur->logicalModemUuid);
    p.writeInt32(p_cur->status);

    startResponse;
    appendPrintBuf("%s[version=%d,session=%d,phase=%d,\
            rat=%s,logicalModemUuid=%s,status=%d]",
            printBuf,
            p_cur->version,
            p_cur->session,
            p_cur->phase,
            p_cur->rat,
            p_cur->logicalModemUuid,
            p_cur->status);
    closeResponse;
    return 0;
}

static int responseSSData(Parcel &p, void *response, size_t responselen) {
    RLOGD("In responseSSData");
    int num;

    if (response == NULL && responselen != 0) {
        RLOGE("invalid response length was %d expected %d",
                (int)responselen, (int)sizeof (RIL_SIM_IO_Response));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen != sizeof(RIL_StkCcUnsolSsResponse)) {
        RLOGE("invalid response length %d, expected %d",
               (int)responselen, (int)sizeof(RIL_StkCcUnsolSsResponse));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    startResponse;
    RIL_StkCcUnsolSsResponse *p_cur = (RIL_StkCcUnsolSsResponse *) response;
    p.writeInt32(p_cur->serviceType);
    p.writeInt32(p_cur->requestType);
    p.writeInt32(p_cur->teleserviceType);
    p.writeInt32(p_cur->serviceClass);
    p.writeInt32(p_cur->result);

    if (isServiceTypeCfQuery(p_cur->serviceType, p_cur->requestType)) {
        RLOGD("responseSSData CF type, num of Cf elements %d", p_cur->cfData.numValidIndexes);
        if (p_cur->cfData.numValidIndexes > NUM_SERVICE_CLASSES) {
            RLOGE("numValidIndexes is greater than max value %d, "
                  "truncating it to max value", NUM_SERVICE_CLASSES);
            p_cur->cfData.numValidIndexes = NUM_SERVICE_CLASSES;
        }
        /* number of call info's */
        p.writeInt32(p_cur->cfData.numValidIndexes);

        for (int i = 0; i < p_cur->cfData.numValidIndexes; i++) {
             RIL_CallForwardInfo cf = p_cur->cfData.cfInfo[i];

             p.writeInt32(cf.status);
             p.writeInt32(cf.reason);
             p.writeInt32(cf.serviceClass);
             p.writeInt32(cf.toa);
             writeStringToParcel(p, cf.number);
             p.writeInt32(cf.timeSeconds);
             appendPrintBuf("%s[%s,reason=%d,cls=%d,toa=%d,%s,tout=%d],", printBuf,
                 (cf.status==1)?"enable":"disable", cf.reason, cf.serviceClass, cf.toa,
                  (char*)cf.number, cf.timeSeconds);
             RLOGD("Data: %d,reason=%d,cls=%d,toa=%d,num=%s,tout=%d],", cf.status,
                  cf.reason, cf.serviceClass, cf.toa, (char*)cf.number, cf.timeSeconds);
        }
    } else {
        p.writeInt32 (SS_INFO_MAX);

        /* each int*/
        for (int i = 0; i < SS_INFO_MAX; i++) {
             appendPrintBuf("%s%d,", printBuf, p_cur->ssInfo[i]);
             RLOGD("Data: %d",p_cur->ssInfo[i]);
             p.writeInt32(p_cur->ssInfo[i]);
        }
    }
    removeLastChar;
    closeResponse;

    return 0;
}

static bool isServiceTypeCfQuery(RIL_SsServiceType serType, RIL_SsRequestType reqType) {
    if ((reqType == SS_INTERROGATION) &&
        (serType == SS_CFU ||
         serType == SS_CF_BUSY ||
         serType == SS_CF_NO_REPLY ||
         serType == SS_CF_NOT_REACHABLE ||
         serType == SS_CF_ALL ||
         serType == SS_CF_ALL_CONDITIONAL)) {
        return true;
    }
    return false;
}

#ifdef MTK_RIL
void triggerEvLoop() {
#else
static void triggerEvLoop() {
#endif
    int ret;
    if (!pthread_equal(pthread_self(), s_tid_dispatch)) {
        /* trigger event loop to wakeup. No reason to do this,
         * if we're in the event loop thread */
         do {
            ret = write (s_fdWakeupWrite, " ", 1);
         } while (ret < 0 && errno == EINTR);
    }
}

static void rilEventAddWakeup(struct ril_event *ev) {
    ril_event_add(ev);
    triggerEvLoop();
}

static void sendSimStatusAppInfo(Parcel &p, int num_apps, RIL_AppStatus appStatus[]) {
        p.writeInt32(num_apps);
        startResponse;
        for (int i = 0; i < num_apps; i++) {
            p.writeInt32(appStatus[i].app_type);
            p.writeInt32(appStatus[i].app_state);
            p.writeInt32(appStatus[i].perso_substate);
            writeStringToParcel(p, (const char*)(appStatus[i].aid_ptr));
            writeStringToParcel(p, (const char*)
                                          (appStatus[i].app_label_ptr));
            p.writeInt32(appStatus[i].pin1_replaced);
            p.writeInt32(appStatus[i].pin1);
            p.writeInt32(appStatus[i].pin2);
            appendPrintBuf("%s[app_type=%d,app_state=%d,perso_substate=%d,\
                    aid_ptr=%s,app_label_ptr=%s,pin1_replaced=%d,pin1=%d,pin2=%d],",
                    printBuf,
                    appStatus[i].app_type,
                    appStatus[i].app_state,
                    appStatus[i].perso_substate,
                    appStatus[i].aid_ptr,
                    appStatus[i].app_label_ptr,
                    appStatus[i].pin1_replaced,
                    appStatus[i].pin1,
                    appStatus[i].pin2);
        }
        closeResponse;
}

static void responseSimStatusV5(Parcel &p, void *response) {
    RIL_CardStatus_v5 *p_cur = ((RIL_CardStatus_v5 *) response);

    p.writeInt32(p_cur->card_state);
    p.writeInt32(p_cur->universal_pin_state);
    p.writeInt32(p_cur->gsm_umts_subscription_app_index);
    p.writeInt32(p_cur->cdma_subscription_app_index);

    sendSimStatusAppInfo(p, p_cur->num_applications, p_cur->applications);
}

static void responseSimStatusV6(Parcel &p, void *response) {
    RIL_CardStatus_v6 *p_cur = ((RIL_CardStatus_v6 *) response);

    p.writeInt32(p_cur->card_state);
    p.writeInt32(p_cur->universal_pin_state);
    p.writeInt32(p_cur->gsm_umts_subscription_app_index);
    p.writeInt32(p_cur->cdma_subscription_app_index);
    p.writeInt32(p_cur->ims_subscription_app_index);

    sendSimStatusAppInfo(p, p_cur->num_applications, p_cur->applications);
}

static void responseSimStatusV7(Parcel &p, void *response) {
    RIL_CardStatus_v7 *p_cur = ((RIL_CardStatus_v7 *) response);

    p.writeInt32(p_cur->card_state);
    p.writeInt32(p_cur->universal_pin_state);
    p.writeInt32(p_cur->gsm_umts_subscription_app_index);
    p.writeInt32(p_cur->cdma_subscription_app_index);
    p.writeInt32(p_cur->ims_subscription_app_index);

    sendSimStatusAppInfo(p, p_cur->num_applications, p_cur->applications);

    // Parameters add from radio hidl v1.2
    p.writeInt32(p_cur->physicalSlotId);
    writeStringToParcel(p, (const char*)(p_cur->atr));
    writeStringToParcel(p, (const char*)(p_cur->iccId));
}

static void responseSimStatusV8(Parcel &p, void *response) {
    RIL_CardStatus_v8 *p_cur = ((RIL_CardStatus_v8 *) response);

    p.writeInt32(p_cur->card_state);
    p.writeInt32(p_cur->universal_pin_state);
    p.writeInt32(p_cur->gsm_umts_subscription_app_index);
    p.writeInt32(p_cur->cdma_subscription_app_index);
    p.writeInt32(p_cur->ims_subscription_app_index);

    sendSimStatusAppInfo(p, p_cur->num_applications, p_cur->applications);

    // Parameters add from radio hidl v1.2
    p.writeInt32(p_cur->physicalSlotId);
    writeStringToParcel(p, (const char*)(p_cur->atr));
    writeStringToParcel(p, (const char*)(p_cur->iccId));

    // Parameters add from radio hidl v1.4.
    writeStringToParcel(p, (const char*)(p_cur->eid));
}

static int responseSimStatus(Parcel &p, void *response, size_t responselen) {
    int i;

    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (s_callbacks.version <= LAST_IMPRECISE_RIL_VERSION) {
        if (responselen == sizeof (RIL_CardStatus_v6)) {
            responseSimStatusV6(p, response);
        } else if (responselen == sizeof (RIL_CardStatus_v5)) {
            responseSimStatusV5(p, response);
        } else {
            RLOGE("responseSimStatus: A RilCardStatus_v6 or _v5 expected\n");
            return RIL_ERRNO_INVALID_RESPONSE;
        }
    } else { // RIL version >= 13
        /*if (responselen % sizeof(RIL_CardStatus_v6) != 0) {
            RLOGE("Data structure expected is RIL_CardStatus_v6");
            if (!isDebuggable()) {
                return RIL_ERRNO_INVALID_RESPONSE;
            } else {
                assert(0);
            }
        }
        responseSimStatusV6(p, response);*/
        if (responselen == sizeof (RIL_CardStatus_v8)) {
                responseSimStatusV8(p, response);
        } else if (responselen == sizeof (RIL_CardStatus_v7)) {
            responseSimStatusV7(p, response);
        } else if (responselen == sizeof (RIL_CardStatus_v6)) {
            responseSimStatusV6(p, response);
        } else {
            RLOGE("responseSimStatus: A RilCardStatus_v7 or _v6 expected\n");
            return RIL_ERRNO_INVALID_RESPONSE;
        }
    }

    return 0;
}

static int responseGsmBrSmsCnf(Parcel &p, void *response, size_t responselen) {
    int num = responselen / sizeof(RIL_GSM_BroadcastSmsConfigInfo *);
    p.writeInt32(num);

    startResponse;
    RIL_GSM_BroadcastSmsConfigInfo **p_cur =
                (RIL_GSM_BroadcastSmsConfigInfo **) response;
    for (int i = 0; i < num; i++) {
        p.writeInt32(p_cur[i]->fromServiceId);
        p.writeInt32(p_cur[i]->toServiceId);
        p.writeInt32(p_cur[i]->fromCodeScheme);
        p.writeInt32(p_cur[i]->toCodeScheme);
        p.writeInt32(p_cur[i]->selected);

        appendPrintBuf("%s [%d: fromServiceId=%d, toServiceId=%d, \
                fromCodeScheme=%d, toCodeScheme=%d, selected =%d]",
                printBuf, i, p_cur[i]->fromServiceId, p_cur[i]->toServiceId,
                p_cur[i]->fromCodeScheme, p_cur[i]->toCodeScheme,
                p_cur[i]->selected);
    }
    closeResponse;

    return 0;
}

static int responseCdmaBrSmsCnf(Parcel &p, void *response, size_t responselen) {
    RIL_CDMA_BroadcastSmsConfigInfo **p_cur =
               (RIL_CDMA_BroadcastSmsConfigInfo **) response;

    int num = responselen / sizeof (RIL_CDMA_BroadcastSmsConfigInfo *);
    p.writeInt32(num);

    startResponse;
    for (int i = 0 ; i < num ; i++ ) {
        p.writeInt32(p_cur[i]->service_category);
        p.writeInt32(p_cur[i]->language);
        p.writeInt32(p_cur[i]->selected);

        appendPrintBuf("%s [%d: srvice_category=%d, language =%d, \
              selected =%d], ",
              printBuf, i, p_cur[i]->service_category, p_cur[i]->language,
              p_cur[i]->selected);
    }
    closeResponse;

    return 0;
}

static int responseCdmaSms(Parcel &p, void *response, size_t responselen) {
    int num;
    int digitCount;
    int digitLimit;
    uint8_t uct;
    void* dest;

    RLOGD("Inside responseCdmaSms");

    if (response == NULL && responselen != 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen != sizeof(RIL_CDMA_SMS_Message)) {
        RLOGE("invalid response length was %d expected %d",
                (int)responselen, (int)sizeof(RIL_CDMA_SMS_Message));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_CDMA_SMS_Message *p_cur = (RIL_CDMA_SMS_Message *) response;
    p.writeInt32(p_cur->uTeleserviceID);
    p.write(&(p_cur->bIsServicePresent),sizeof(uct));
    p.writeInt32(p_cur->uServicecategory);
    p.writeInt32(p_cur->sAddress.digit_mode);
    p.writeInt32(p_cur->sAddress.number_mode);
    p.writeInt32(p_cur->sAddress.number_type);
    p.writeInt32(p_cur->sAddress.number_plan);
    p.write(&(p_cur->sAddress.number_of_digits), sizeof(uct));
    digitLimit= MIN((p_cur->sAddress.number_of_digits), RIL_CDMA_SMS_ADDRESS_MAX);
    for(digitCount =0 ; digitCount < digitLimit; digitCount ++) {
        p.write(&(p_cur->sAddress.digits[digitCount]),sizeof(uct));
    }

    p.writeInt32(p_cur->sSubAddress.subaddressType);
    p.write(&(p_cur->sSubAddress.odd),sizeof(uct));
    p.write(&(p_cur->sSubAddress.number_of_digits),sizeof(uct));
    digitLimit= MIN((p_cur->sSubAddress.number_of_digits), RIL_CDMA_SMS_SUBADDRESS_MAX);
    for(digitCount =0 ; digitCount < digitLimit; digitCount ++) {
        p.write(&(p_cur->sSubAddress.digits[digitCount]),sizeof(uct));
    }

    digitLimit= MIN((p_cur->uBearerDataLen), RIL_CDMA_SMS_BEARER_DATA_MAX);
    p.writeInt32(p_cur->uBearerDataLen);
    for(digitCount =0 ; digitCount < digitLimit; digitCount ++) {
       p.write(&(p_cur->aBearerData[digitCount]), sizeof(uct));
    }

    startResponse;
    appendPrintBuf("%suTeleserviceID=%d, bIsServicePresent=%d, uServicecategory=%d, \
            sAddress.digit_mode=%d, sAddress.number_mode=%d, sAddress.number_type=%d, ",
            printBuf, p_cur->uTeleserviceID,p_cur->bIsServicePresent,p_cur->uServicecategory,
            p_cur->sAddress.digit_mode, p_cur->sAddress.number_mode,p_cur->sAddress.number_type);
    closeResponse;

    return 0;
}

static int responseDcRtInfo(Parcel &p, void *response, size_t responselen)
{
    int num = responselen / sizeof(RIL_DcRtInfo);
    if ((responselen % sizeof(RIL_DcRtInfo) != 0) || (num != 1)) {
        RLOGE("responseDcRtInfo: invalid response length %d expected multiple of %d",
                (int)responselen, (int)sizeof(RIL_DcRtInfo));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    startResponse;
    RIL_DcRtInfo *pDcRtInfo = (RIL_DcRtInfo *)response;
    p.writeInt64(pDcRtInfo->time);
    p.writeInt32(pDcRtInfo->powerState);
    appendPrintBuf("%s[time=%d,powerState=%d]", printBuf,
        pDcRtInfo->time,
        pDcRtInfo->powerState);
    closeResponse;

    return 0;
}

static int responseLceStatus(Parcel &p, void *response, size_t responselen) {
  if (response == NULL || responselen != sizeof(RIL_LceStatusInfo)) {
    if (response == NULL) {
      RLOGE("invalid response: NULL");
    }
    else {
      RLOGE("responseLceStatus: invalid response length %u expecting len: %u",
            (unsigned)sizeof(RIL_LceStatusInfo), (unsigned)responselen);
    }
    return RIL_ERRNO_INVALID_RESPONSE;
  }

  RIL_LceStatusInfo *p_cur = (RIL_LceStatusInfo *)response;
  p.write((void *)p_cur, 1);  // p_cur->lce_status takes one byte.
  p.writeInt32(p_cur->actual_interval_ms);

  startResponse;
  appendPrintBuf("LCE Status: %d, actual_interval_ms: %d",
                 p_cur->lce_status, p_cur->actual_interval_ms);
  closeResponse;

  return 0;
}

static int responseLceData(Parcel &p, void *response, size_t responselen) {
  if (response == NULL || responselen != sizeof(RIL_LceDataInfo)) {
    if (response == NULL) {
      RLOGE("invalid response: NULL");
    }
    else {
      RLOGE("responseLceData: invalid response length %u expecting len: %u",
            (unsigned)sizeof(RIL_LceDataInfo), (unsigned)responselen);
    }
    return RIL_ERRNO_INVALID_RESPONSE;
  }

  RIL_LceDataInfo *p_cur = (RIL_LceDataInfo *)response;
  p.writeInt32(p_cur->last_hop_capacity_kbps);

  /* p_cur->confidence_level and p_cur->lce_suspended take 1 byte each.*/
  p.write((void *)&(p_cur->confidence_level), 1);
  p.write((void *)&(p_cur->lce_suspended), 1);

  startResponse;
  appendPrintBuf("LCE info received: capacity %d confidence level %d \
                  and suspended %d",
                  p_cur->last_hop_capacity_kbps, p_cur->confidence_level,
                  p_cur->lce_suspended);
  closeResponse;

  return 0;
}

static int responseActivityData(Parcel &p, void *response, size_t responselen) {
  if (response == NULL || responselen != sizeof(RIL_ActivityStatsInfo)) {
    if (response == NULL) {
      RLOGE("invalid response: NULL");
    }
    else {
      RLOGE("responseActivityData: invalid response length %u expecting len: %u",
            (unsigned)sizeof(RIL_ActivityStatsInfo), (unsigned)responselen);
    }
    return RIL_ERRNO_INVALID_RESPONSE;
  }

  RIL_ActivityStatsInfo *p_cur = (RIL_ActivityStatsInfo *)response;
  p.writeInt32(p_cur->sleep_mode_time_ms);
  p.writeInt32(p_cur->idle_mode_time_ms);
  for(int i = 0; i < RIL_NUM_TX_POWER_LEVELS; i++) {
    p.writeInt32(p_cur->tx_mode_time_ms[i]);
  }
  p.writeInt32(p_cur->rx_mode_time_ms);

  startResponse;
  appendPrintBuf("Modem activity info received: sleep_mode_time_ms %d idle_mode_time_ms %d \
                  tx_mode_time_ms %d %d %d %d %d and rx_mode_time_ms %d",
                  p_cur->sleep_mode_time_ms, p_cur->idle_mode_time_ms, p_cur->tx_mode_time_ms[0],
                  p_cur->tx_mode_time_ms[1], p_cur->tx_mode_time_ms[2], p_cur->tx_mode_time_ms[3],
                  p_cur->tx_mode_time_ms[4], p_cur->rx_mode_time_ms);
   closeResponse;

  return 0;
}

/// [C2K] IRAT feature.
static int responseIratStateChange(Parcel &p, void *response, size_t responselen) {
    RLOGD("responseIratStateChange: 0 responselen = %zu, sizeof(RIL_Pdn_IratInfo) = %zu.",
        responselen, sizeof(RIL_Pdn_IratInfo));

    if (response == NULL || responselen == 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen != sizeof(RIL_Pdn_IratInfo)) {
        RLOGE("invalid response length %d expected sizeof (RIL_Pdn_IratInfo) of %d\n",
            (int) responselen, (int) sizeof(RIL_Pdn_IratInfo));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    startResponse;

    RIL_Pdn_IratInfo *irat_info = (RIL_Pdn_IratInfo*) response;
    p.writeInt32(irat_info->sourceRat);
    p.writeInt32(irat_info->targetRat);
    p.writeInt32(irat_info->action);
    p.writeInt32(irat_info->type);
    appendPrintBuf("%s [sourceRat = %d, targetRat = %d, action = %d, type = %d]", printBuf,
        irat_info->sourceRat, irat_info->targetRat, irat_info->action, irat_info->type);

    closeResponse;
    return 0;
}

static int responseCarrierRestrictions(Parcel &p, void *response, size_t responselen) {
  if (response == NULL) {
    RLOGE("invalid response: NULL");
    return RIL_ERRNO_INVALID_RESPONSE;
  }
  if (responselen != sizeof(RIL_CarrierRestrictionsWithPriority)) {
    RLOGE("responseCarrierRestrictions: invalid response length %u expecting len: %u",
          (unsigned)responselen, (unsigned)sizeof(RIL_CarrierRestrictionsWithPriority));
    return RIL_ERRNO_INVALID_RESPONSE;
  }

  RIL_CarrierRestrictionsWithPriority *p_cr = (RIL_CarrierRestrictionsWithPriority *)response;
  startResponse;

  p.writeInt32(p_cr->len_allowed_carriers);
  p.writeInt32(p_cr->len_excluded_carriers);
  appendPrintBuf(" %s len_allowed_carriers: %d, len_excluded_carriers: %d,", printBuf,
                 p_cr->len_allowed_carriers,p_cr->len_excluded_carriers);

  appendPrintBuf(" %s allowed_carriers:", printBuf);
  for(int32_t i = 0; i < p_cr->len_allowed_carriers; i++) {
    RIL_Carrier *carrier = p_cr->allowed_carriers + i;
    writeStringToParcel(p, carrier->mcc);
    writeStringToParcel(p, carrier->mnc);
    p.writeInt32(carrier->match_type);
    writeStringToParcel(p, carrier->match_data);
    appendPrintBuf(" %s [%d mcc: %s, mnc: %s, match_type: %d, match_data: %s],", printBuf,
                   i, carrier->mcc, carrier->mnc, carrier->match_type, carrier->match_data);
  }

  appendPrintBuf(" %s excluded_carriers:", printBuf);
  for(int32_t i = 0; i < p_cr->len_excluded_carriers; i++) {
    RIL_Carrier *carrier = p_cr->excluded_carriers + i;
    writeStringToParcel(p, carrier->mcc);
    writeStringToParcel(p, carrier->mnc);
    p.writeInt32(carrier->match_type);
    writeStringToParcel(p, carrier->match_data);
    appendPrintBuf(" %s [%d mcc: %s, mnc: %s, match_type: %d, match_data: %s],", printBuf,
                   i, carrier->mcc, carrier->mnc, carrier->match_type, carrier->match_data);
  }

  p.writeBool(p_cr->allowedCarriersPrioritized);
  p.writeInt32(p_cr->simLockMultiSimPolicy);

  closeResponse;

  return 0;
}

static int responsePhoneCapability(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }
    if (responselen != sizeof(RIL_PhoneCapability)) {
        RLOGE("responsePhoneCapability: invalid response length %u expecting len: %u",
              (unsigned)responselen, (unsigned)sizeof(RIL_PhoneCapability));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_PhoneCapability *p_cr = (RIL_PhoneCapability *)response;
    startResponse;

    p.writeInt32(p_cr->maxActiveData);
    p.writeInt32(p_cr->maxActiveInternetData);
    p.writeInt32(p_cr->isInternetLingeringSupported);
    for(int i = 0; i < SIM_COUNT; i++) {
        p.writeInt32(p_cr->logicalModemList[i].modemId);
    }

    appendPrintBuf("PhoneCapability received: maxActiveData %d maxActiveInternetData %d \
            isInternetLingeringSupported %d",
            p_cr->maxActiveData, p_cr->maxActiveInternetData,
            p_cr->isInternetLingeringSupported);
    closeResponse;

    return 0;
}

// MTK-START: SIM OPEN CHANNEL WITH P2
static void dispatchOpenChannelParams(Parcel &p, RequestInfo *pRI) {
    RIL_OpenChannelParams openChannelParams;
    int32_t t;
    status_t status;

    RLOGD("dispatchOpenChannelParams Enter.");

    memset(&openChannelParams, 0, sizeof(openChannelParams));

    openChannelParams.aidPtr = strdupReadString(p);

    status = p.readInt32(&t);
    openChannelParams.p2 = t;
    if (status != NO_ERROR) {
        goto invalid;
    }

    RLOGD("dispatchOpenChannelParams aid: %s, p2: %d",
            openChannelParams.aidPtr, openChannelParams.p2);

    startRequest;
    appendPrintBuf("%said=%s,p2=%d", printBuf,
            openChannelParams.aidPtr, openChannelParams.p2);
    closeRequest;

    CALL_ONREQUEST(pRI->pCI->requestNumber, &openChannelParams,
            sizeof(openChannelParams), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString(openChannelParams.aidPtr);
#endif
    if (openChannelParams.aidPtr != NULL) {
        free(openChannelParams.aidPtr);
    }
#ifdef MEMSET_FREED
    memset(&openChannelParams, 0, sizeof(openChannelParams));
#endif

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}
// MTK-END

static int responseNetworkScanResult(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        RLOGE("responseNetworkScanResult: invalid NULL response");
        return RIL_ERRNO_INVALID_RESPONSE;
    }
    if (responselen != sizeof(RIL_NetworkScanResult)) {
        RLOGE("responseNetworkScanResult: invalid response length %u, expecting %u",
                (unsigned)responselen, (unsigned)sizeof(RIL_NetworkScanResult));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_NetworkScanResult *p_result = (RIL_NetworkScanResult *)response;
    p.writeInt32(p_result->status);
    p.writeInt32(p_result->network_infos_length);

    RIL_CellInfo_v12 *p_cur = (RIL_CellInfo_v12 *) p_result->network_infos;
    for (int i = 0; i < p_result->network_infos_length; i++) {
        p.writeInt32((int)p_cur->cellInfoType);
        p.writeInt32(p_cur->registered);
        p.writeInt32(p_cur->timeStampType);
        p.writeInt64(p_cur->timeStamp);
        switch (p_cur->cellInfoType) {
            case RIL_CELL_INFO_TYPE_GSM: {
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.mcc);
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.mnc);
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.lac);
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.cid);
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.arfcn);
                p.writeInt32(p_cur->CellInfo.gsm.cellIdentityGsm.bsic);
                p.writeInt32(p_cur->CellInfo.gsm.signalStrengthGsm.signalStrength);
                p.writeInt32(p_cur->CellInfo.gsm.signalStrengthGsm.bitErrorRate);
                p.writeInt32(p_cur->CellInfo.gsm.signalStrengthGsm.timingAdvance);
                break;
            }
            case RIL_CELL_INFO_TYPE_WCDMA: {
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.mcc);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.mnc);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.lac);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.cid);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.psc);
                p.writeInt32(p_cur->CellInfo.wcdma.cellIdentityWcdma.uarfcn);
                p.writeInt32(p_cur->CellInfo.wcdma.signalStrengthWcdma.signalStrength);
                p.writeInt32(p_cur->CellInfo.wcdma.signalStrengthWcdma.bitErrorRate);
                break;
            }
            case RIL_CELL_INFO_TYPE_CDMA: {
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.networkId);
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.systemId);
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.basestationId);
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.longitude);
                p.writeInt32(p_cur->CellInfo.cdma.cellIdentityCdma.latitude);
                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthCdma.dbm);
                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthCdma.ecio);
                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthEvdo.dbm);
                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthEvdo.ecio);
                p.writeInt32(p_cur->CellInfo.cdma.signalStrengthEvdo.signalNoiseRatio);
                break;
            }
            case RIL_CELL_INFO_TYPE_LTE: {
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.mcc);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.mnc);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.ci);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.pci);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.tac);
                p.writeInt32(p_cur->CellInfo.lte.cellIdentityLte.earfcn);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.signalStrength);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.rsrp);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.rsrq);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.rssnr);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.cqi);
                p.writeInt32(p_cur->CellInfo.lte.signalStrengthLte.timingAdvance);
                break;
            }
            case RIL_CELL_INFO_TYPE_TD_SCDMA: {
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.mcc);
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.mnc);
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.lac);
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.cid);
                p.writeInt32(p_cur->CellInfo.tdscdma.cellIdentityTdscdma.cpid);
                p.writeInt32(p_cur->CellInfo.tdscdma.signalStrengthTdscdma.rscp);
                break;
            }
            case RIL_CELL_INFO_TYPE_NR:
            default: {
                break;
            }
        }
        p_cur += 1;
    }
    startResponse;
        appendPrintBuf("Network scan result: status %d, network_infos_length %d",
                p_result->status, p_result->network_infos_length);
    closeResponse;
    return 0;
}

// SMS-START
static int responseGetSmsSimMemStatusCnf(Parcel &p,void *response, size_t responselen)
{
    if (response == NULL || responselen == 0) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen != sizeof (RIL_SMS_Memory_Status)) {
        RLOGE("invalid response length %d expected sizeof (RIL_SMS_Memory_Status) of %d\n",
             (int)responselen, (int)sizeof(RIL_SMS_Memory_Status));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    startResponse;

    RIL_SMS_Memory_Status *mem_status = (RIL_SMS_Memory_Status*)response;

    p.writeInt32(mem_status->used);
    p.writeInt32(mem_status->total);

    appendPrintBuf("%s [used = %d, total = %d]", printBuf, mem_status->used, mem_status->total);

    closeResponse;

    return 0;
}

static void dispatchSmsParams(Parcel &p, RequestInfo *pRI) {
    RIL_SmsParams smsParams;
    int32_t t;
    status_t status;

    RLOGD("dispatchSmsParams Enter.");

    memset(&smsParams, 0, sizeof(smsParams));

    status = p.readInt32(&t);
    if (status != NO_ERROR) {
        goto invalid;
    }

    status = p.readInt32(&t);
    smsParams.format = t;
    if (status != NO_ERROR) {
        goto invalid;
    }

    status = p.readInt32(&t);
    smsParams.vp = t;
    if (status != NO_ERROR) {
        goto invalid;
    }

    status = p.readInt32(&t);
    smsParams.pid = t;
    if (status != NO_ERROR) {
        goto invalid;
    }

    status = p.readInt32(&t);
    smsParams.dcs = t;
    if (status != NO_ERROR) {
        goto invalid;
    }

    RLOGD("dispatchSmsParams format: %d, vp: %d, pid: %d, dcs: %d", smsParams.format,
            smsParams.vp, smsParams.pid, smsParams.dcs);

    startRequest;
    appendPrintBuf("%sformat=%d,vp=%d,pid=%d,dcs=%d", printBuf,
            smsParams.format, smsParams.vp, smsParams.pid, smsParams.dcs);
    closeRequest;

    CALL_ONREQUEST(pRI->pCI->requestNumber, &smsParams, sizeof(smsParams), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memset(&smsParams, 0, sizeof(smsParams));
#endif

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

static int responseSmsParams(Parcel &p, void *response, size_t responselen) {
    if(response == NULL) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if(responselen != (int)sizeof(RIL_SmsParams)) {
        RLOGE("invalid response length %d expected %d",
             (int)responselen, (int)sizeof(RIL_SmsParams));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_SmsParams *p_cur = (RIL_SmsParams *)response;
    p.writeInt32(p_cur->format);
    p.writeInt32(p_cur->vp);
    p.writeInt32(p_cur->pid);
    p.writeInt32(p_cur->dcs);

    startResponse;
    appendPrintBuf("%s%d,%d,%d,%d", printBuf, p_cur->format, p_cur->vp,
                   p_cur->pid, p_cur->dcs);
    closeResponse;

    return 0;
}

static int responseEtwsNotification(Parcel &p, void *response, size_t responselen) {
    if(NULL == response) {
        RLOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if(responselen != sizeof(RIL_CBEtwsNotification)) {
        RLOGE("invalid response length %zu expected %zu",
            responselen, sizeof(RIL_CBEtwsNotification));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_CBEtwsNotification *p_cur = (RIL_CBEtwsNotification *)response;
    p.writeInt32(p_cur->warningType);
    p.writeInt32(p_cur->messageId);
    p.writeInt32(p_cur->serialNumber);
    writeStringToParcel(p, p_cur->plmnId);
    writeStringToParcel(p, p_cur->securityInfo);

    startResponse;
    appendPrintBuf("%s%d,%d,%d,%s,%s", printBuf, p_cur->warningType, p_cur->messageId,
                   p_cur->serialNumber, p_cur->plmnId, p_cur->securityInfo);
    closeResponse;

    return 0;
}

// SMS-END

static int responseCrssN(Parcel &p, void *response, size_t responselen) {
    if (response == NULL) {
        LOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen != sizeof(RIL_CrssNotification)) {
        LOGE("invalid response length was %d expected %d",
             (int)responselen, (int)sizeof (RIL_CrssNotification));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    RIL_CrssNotification *p_cur = (RIL_CrssNotification *) response;
    p.writeInt32(p_cur->code);
    p.writeInt32(p_cur->type);
    writeStringToParcel(p, p_cur->number);
    writeStringToParcel(p, p_cur->alphaid);
    p.writeInt32(p_cur->cli_validity);
    return 0;
}

static int responsePcoData(Parcel &p, void *response, size_t responselen) {
  if (response == NULL) {
    RLOGE("responsePcoData: invalid NULL response");
    return RIL_ERRNO_INVALID_RESPONSE;
  }
  if (responselen != sizeof(RIL_PCO_Data)) {
    RLOGE("responsePcoData: invalid response length %u, expecting %u",
          (unsigned)responselen, (unsigned)sizeof(RIL_PCO_Data));
    return RIL_ERRNO_INVALID_RESPONSE;
  }

  RIL_PCO_Data *p_cur = (RIL_PCO_Data *)response;
  p.writeInt32(p_cur->cid);
  writeStringToParcel(p, p_cur->bearer_proto);
  p.writeInt32(p_cur->pco_id);
  p.writeInt32(p_cur->contents_length);
  p.write(p_cur->contents, p_cur->contents_length);

  startResponse;
      appendPrintBuf("PCO data received: cid %d, id %d, length %d",
                     p_cur->cid, p_cur->pco_id, p_cur->contents_length);
  closeResponse;

  return 0;
}

// M: [VzW] Data Framework @{
static int responsePcoDataAfterAttached(Parcel &p, void *response, size_t responselen) {
  if (response == NULL) {
    RLOGE("responsePcoDataAfterAttached: invalid NULL response");
    return RIL_ERRNO_INVALID_RESPONSE;
  }
  if (responselen != sizeof(RIL_PCO_Data_attached)) {
    RLOGE("responsePcoDataAfterAttached: invalid response length %u, expecting %u",
          (unsigned)responselen, (unsigned)sizeof(RIL_PCO_Data_attached));
    return RIL_ERRNO_INVALID_RESPONSE;
  }

  RIL_PCO_Data_attached *p_cur = (RIL_PCO_Data_attached *)response;
  p.writeInt32(p_cur->cid);
  writeStringToParcel(p, p_cur->apn_name);
  writeStringToParcel(p, p_cur->bearer_proto);
  p.writeInt32(p_cur->pco_id);
  p.writeInt32(p_cur->contents_length);
  p.write(p_cur->contents, p_cur->contents_length);

  startResponse;
      appendPrintBuf("PCO data received: cid %d, id %d, apn name %s, length %d",
                     p_cur->cid, p_cur->pco_id, p_cur->apn_name, p_cur->contents_length);
  closeResponse;

  return 0;
}
// M: [VzW] Data Framework @}

// PHB START
static void dispatchPhbEntry(Parcel &p, RequestInfo *pRI) {
    RIL_PhbEntryStructure args;
    int32_t t = 0;
    status_t status = NO_ERROR;

    memset (&args, 0, sizeof(args));

    // storage type
    status = p.readInt32(&t);
    args.type = (int) t;

    // index of the entry
    status = p.readInt32(&t);
    args.index = (int) t;

    // phone number
    args.number = strdupReadString(p);

    // Type of the number
    status = p.readInt32(&t);
    args.ton = (int) t;

    // alpha Id
    args.alphaId = strdupReadString(p);

    if (status != NO_ERROR) {
#ifdef MEMSET_FREED
        memsetString(args.number);
        memsetString(args.alphaId);
#endif
        free(args.number);
        free(args.alphaId);
#ifdef MEMSET_FREED
        memset(&args, 0, sizeof(args));
#endif
        goto invalid;
    }

    startRequest;
    appendPrintBuf("%s%d,index=%d,num=%s,ton=%d,alphaId=%s", printBuf, args.type,
                   args.index, (char*)args.number, args.ton,  (char*)args.alphaId);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &args, sizeof(args), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString(args.number);
    memsetString(args.alphaId);
#endif

    free(args.number);
    free(args.alphaId);

#ifdef MEMSET_FREED
    memset(&args, 0, sizeof(args));
#endif

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

static void dispatchWritePhbEntryExt(Parcel &p, RequestInfo *pRI) {
    RIL_PHB_ENTRY args;
    int32_t t = 0;
    status_t status = NO_ERROR;

    memset(&args, 0, sizeof(args));

    // index of the entry
    status = p.readInt32(&t);
    args.index = (int) t;
    // phone number
    args.number = strdupReadString(p);
    // Type of the number
    status = p.readInt32(&t);
    args.type = (int) t;
    // text
    args.text = strdupReadString(p);
    // hidden
    status = p.readInt32(&t);
    args.hidden = (int) t;

    // group
    args.group = strdupReadString(p);
    // anr
    args.adnumber = strdupReadString(p);
    // Type of the adnumber
    status = p.readInt32(&t);
    args.adtype = (int) t;
    // SNE
    args.secondtext = strdupReadString(p);
    // email
    args.email = strdupReadString(p);

    if (status != NO_ERROR) {
#ifdef MEMSET_FREED
        memsetString(args.number);
        memsetString(args.text);
        memsetString(args.group);
        memsetString(args.adnumber);
        memsetString(args.secondtext);
        memsetString(args.email);
#endif
        free(args.number);
        free(args.text);
        free(args.group);
        free(args.adnumber);
        free(args.secondtext);
        free(args.email);

#ifdef MEMSET_FREED
        memset(&args, 0, sizeof(args));
#endif
        goto invalid;
    }

    startRequest;
    appendPrintBuf("%s,index=%d,num=%s,type=%d,text=%s,hidden=%d,group=%s,adnumber=%s,adtype=%d,secondtext=%s,email=%s"
                   , printBuf, args.index, (char*)args.number, args.type, (char*)args.text, args.hidden,
                   (char*)args.group, (char*)args.adnumber, args.adtype, (char*)args.secondtext, (char*)args.email);
    closeRequest;
    printRequest(pRI->token, pRI->pCI->requestNumber);

    CALL_ONREQUEST(pRI->pCI->requestNumber, &args, sizeof(args), pRI, pRI->socket_id);

#ifdef MEMSET_FREED
    memsetString(args.number);
    memsetString(args.text);
    memsetString(args.group);
    memsetString(args.adnumber);
    memsetString(args.secondtext);
    memsetString(args.email);
#endif
    free(args.number);
    free(args.text);
    free(args.group);
    free(args.adnumber);
    free(args.secondtext);
    free(args.email);

#ifdef MEMSET_FREED
    memset(&args, 0, sizeof(args));
#endif

    return;
invalid:
    invalidCommandBlock(pRI);
    return;
}

static int responsePhbEntries(Parcel &p, void *response, size_t responselen) {
    if (response == NULL && responselen != 0) {
        LOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    if (responselen % sizeof (RIL_PhbEntryStructure *) != 0) {
        LOGE("invalid response length %d expected multiple of %d\n",
             (int)responselen, (int)sizeof (RIL_PhbEntryStructure *));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    int num = responselen / sizeof(RIL_PhbEntryStructure *);
    p.writeInt32(num);

    startResponse;
    RIL_PhbEntryStructure **p_cur =
        (RIL_PhbEntryStructure **) response;
    for (int i = 0; i < num; i++) {
        p.writeInt32(p_cur[i]->type);
        p.writeInt32(p_cur[i]->index);
        writeStringToParcel(p, p_cur[i]->number);
        p.writeInt32(p_cur[i]->ton);
        writeStringToParcel(p, p_cur[i]->alphaId);

        appendPrintBuf("%s[%d: type = %d, index = %d, \
                number = %s, ton = %d, alphaId = %s]",
                       printBuf, i, p_cur[i]->type, p_cur[i]->index,
                       p_cur[i]->number, p_cur[i]->ton,
                       p_cur[i]->alphaId);
    }
    closeResponse;

    return 0;
}

static int responseReadPhbEntryExt(Parcel &p, void *response, size_t responselen) {
    if (response == NULL && responselen != 0) {
        LOGE("invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }
    if (responselen % sizeof (RIL_PHB_ENTRY *) != 0) {
        LOGE("invalid response length %d expected multiple of %d\n",
             (int)responselen, (int)sizeof (RIL_PHB_ENTRY *));
        return RIL_ERRNO_INVALID_RESPONSE;
    }
    int num = responselen / sizeof(RIL_PHB_ENTRY *);
    p.writeInt32(num);
    startResponse;
    RIL_PHB_ENTRY **p_cur =
        (RIL_PHB_ENTRY **) response;

    for (int i = 0; i < num; i++) {
        p.writeInt32(p_cur[i]->index);
        writeStringToParcel(p, p_cur[i]->number);
        p.writeInt32(p_cur[i]->type);
        writeStringToParcel(p, p_cur[i]->text);
        p.writeInt32(p_cur[i]->hidden);
        writeStringToParcel(p, p_cur[i]->group);
        writeStringToParcel(p, p_cur[i]->adnumber);
        p.writeInt32(p_cur[i]->adtype);
        writeStringToParcel(p, p_cur[i]->secondtext);
        writeStringToParcel(p, p_cur[i]->email);

        appendPrintBuf("%s[%d: index = %d, number = %s, type = %d, text = %s, hidden = %d, group = %s, \
                        adnumber = %s, adtype = %d, sectext = %s, email = %s]",
                       printBuf, i,  p_cur[i]->index,
                       p_cur[i]->number, p_cur[i]->type,
                       p_cur[i]->text, p_cur[i]->hidden, p_cur[i]->group, p_cur[i]->adnumber,
                       p_cur[i]->adtype, p_cur[i]->secondtext, p_cur[i]->email);
    }

    closeResponse;

    return 0;
}

static int responseGetPhbMemStorage(Parcel &p, void *response, size_t responselen) {
    if (response == NULL || responselen == 0) {
        LOGE("responseGetPhbMemStorage invalid response: NULL");
        return RIL_ERRNO_INVALID_RESPONSE;
    }
    if (responselen != sizeof (RIL_PHB_MEM_STORAGE_RESPONSE)) {
        LOGE("invalid response length %d expected sizeof (RIL_PHB_MEM_STORAGE_RESPONSE) of %d\n",
             (int)responselen, (int)sizeof(RIL_PHB_MEM_STORAGE_RESPONSE));
        return RIL_ERRNO_INVALID_RESPONSE;
    }

    startResponse;

    RIL_PHB_MEM_STORAGE_RESPONSE *mem_status = (RIL_PHB_MEM_STORAGE_RESPONSE*)response;
    writeStringToParcel(p, mem_status->storage);
    p.writeInt32(mem_status->used);
    p.writeInt32(mem_status->total);

    appendPrintBuf("%s [storage = %s, used = %d, total = %d]",
            printBuf, mem_status->storage, mem_status->used, mem_status->total);

    closeResponse;
    return 0;
}
// PHB END

/**
 * A write on the wakeup fd is done just to pop us out of select()
 * We empty the buffer here and then ril_event will reset the timers on the
 * way back down
 */
static void processWakeupCallback(int fd, short flags, void *param) {
    char buff[16];
    int ret;

    RLOGV("processWakeupCallback");

    /* empty our wakeup socket out */
    do {
        ret = read(s_fdWakeupRead, &buff, sizeof(buff));
    } while (ret > 0 || (ret < 0 && errno == EINTR));
}

static void onCommandsSocketClosed(RIL_SOCKET_ID socket_id) {
    int ret;
    RequestInfo *p_cur;
    /* Hook for current context
       pendingRequestsMutextHook refer to &s_pendingRequestsMutex */
    pthread_mutex_t * pendingRequestsMutexHook = &s_pendingRequestsMutex[socket_id];
    /* pendingRequestsHook refer to &s_pendingRequests */
    RequestInfo **    pendingRequestsHook = &s_pendingRequests[socket_id];

    /* mark pending requests as "cancelled" so we dont report responses */
    ret = pthread_mutex_lock(pendingRequestsMutexHook);
    assert (ret == 0);

    p_cur = *pendingRequestsHook;

    for (p_cur = *pendingRequestsHook
            ; p_cur != NULL
            ; p_cur  = p_cur->p_next
    ) {
        p_cur->cancelled = 1;
    }

    ret = pthread_mutex_unlock(pendingRequestsMutexHook);
    assert (ret == 0);

    ///M: CC: Hangup all calls to sync with rild in phone process crash case @{
#if defined(MTK_RIL)
    RequestInfo *pRI;
    int request = RIL_REQUEST_HANGUP_ALL;

    pRI = (RequestInfo *)calloc(1, sizeof(RequestInfo));
    if (pRI == NULL) {
        RLOGE("OOM");
        return;
    }
    pRI->local = 1;
    pRI->token = 0xffffffff;
    for (int i = 0; i < (int32_t)NUM_ELEMS(s_mtk_commands); i++) {
        if (request == s_mtk_commands[i].requestNumber) {
            pRI->pCI = &(s_mtk_commands[i]);
            break;
        }
    }

    //Add check for MSIM architecture - must set correct cid according to socket_id
    //Becuase in onRequest():It will set radioState according RIL_queryMyChannelId(t) return t->cid
    //If uses wrong cid:it will get wrong radioState!
    //If the radioState is not in RADIO_ON (e.g., SIM2:wo SIM card, then get RADIO_OFF state)
    //Then RIL_REQUEST_HANGUP_ALL will be skipped due to RADIO_OFF state
     if (socket_id == RIL_SOCKET_1) {
        pRI->cid = RIL_CMD_2;
    } else if (socket_id == RIL_SOCKET_2) {
            pRI->cid = RIL_CMD2_2;
    } else if (socket_id == RIL_SOCKET_3) {
        pRI->cid = RIL_CMD3_2;
    } else if (socket_id == RIL_SOCKET_4) {
        pRI->cid = RIL_CMD4_2;
    }
    pRI->exec_cid = pRI->cid;
    pRI->socket_id = socket_id;
    ret = pthread_mutex_lock(pendingRequestsMutexHook);
    assert (ret == 0);

    pRI->p_next = *pendingRequestsHook;
    *pendingRequestsHook = pRI;

    ret = pthread_mutex_unlock(pendingRequestsMutexHook);
    assert (ret == 0);

    //For Andriod L version: it should use s_callbacksSocket instead of s_callbacks
    //Use old ril vendor lib s_callbacks will cause SIGSEGV
    //Do not porting KK solution to L directly
    RLOGD("onCommandsSocketClosed():C[locl]> %s", requestToString(request));
    CALL_ONREQUEST(RIL_REQUEST_HANGUP_ALL, NULL, 0, pRI, pRI->socket_id);

    //Use new API:issueLocalRequest() with pRI->socket_id for this local hangup action
    //But in this API, it will not fill pRI->cid (using default value 0 - RIL_URC) then can't get correct radio state
    //1.Invalid radio state (E.g.,If call is in SIM2: but only SIM2 with valid sim card) will skip this request
    //2.Even get radio state (E.g., Both SIM1 and SIM2 with valid sim cards)
    //  but it will send this request to the URC channel instead of CC channel
    //issueLocalRequest(RIL_REQUEST_HANGUP_ALL, NULL, 0, socket_id);
#endif
    /// @}


}

static void processCommandsCallback(int fd, short flags, void *param) {
    RecordStream *p_rs;
    void *p_record;
    size_t recordlen;
    int ret;
    SocketListenParam *p_info = (SocketListenParam *)param;

    assert(fd == p_info->fdCommand);

    p_rs = p_info->p_rs;

    for (;;) {
        /* loop until EAGAIN/EINTR, end of stream, or other error */
        ret = record_stream_get_next(p_rs, &p_record, &recordlen);

        if (ret == 0 && p_record == NULL) {
            /* end-of-stream */
            break;
        } else if (ret < 0) {
            break;
        } else if (ret == 0) { /* && p_record != NULL */
            processCommandBuffer(p_record, recordlen, p_info->socket_id);
        }
    }

    if (ret == 0 || !(errno == EAGAIN || errno == EINTR)) {
        /* fatal error or end-of-stream */
        if (ret != 0) {
            RLOGE("error on reading command socket errno:%d\n", errno);
        } else {
            RLOGW("EOS.  Closing command socket.");
        }

        close(fd);
        p_info->fdCommand = -1;
        s_fdCommand[p_info->socket_id] = -1;

        ril_event_del(p_info->commands_event);

        record_stream_free(p_rs);

        /* start listening for new connections again */
        rilEventAddWakeup(&s_listen_event[p_info->socket_id]);

        onCommandsSocketClosed(p_info->socket_id);
    }
}

static void resendLastNITZTimeData(RIL_SOCKET_ID socket_id) {
    if (s_lastNITZTimeData != NULL) {
        Parcel p;
        int responseType = (s_callbacks.version >= 13)
                           ? RESPONSE_UNSOLICITED_ACK_EXP
                           : RESPONSE_UNSOLICITED;
/* Modify to socket base */
#if 1
        RIL_UNSOL_RESPONSE(RIL_UNSOL_NITZ_TIME_RECEIVED,
                s_lastNITZTimeData, sizeof(char *), socket_id);
        free(s_lastNITZTimeData);
        s_lastNITZTimeData = NULL;
#else
        int ret = radio::nitzTimeReceivedInd(
                          p, (int)socket_id, RIL_UNSOL_NITZ_TIME_RECEIVED, responseType, 0,
                          RIL_E_SUCCESS, s_lastNITZTimeData, s_lastNITZTimeDataSize);
        if (ret == 0) {
            free(s_lastNITZTimeData);
            s_lastNITZTimeData = NULL;
        }
#endif
    }
}

static void onNewCommandConnect(RIL_SOCKET_ID socket_id) {
    // Inform we are connected and the ril version
    int rilVer = s_callbacks.version;
    // RIL_UNSOL_RIL_CONNECTED will be sent by RILProxy
    //RIL_UNSOL_RESPONSE(RIL_UNSOL_RIL_CONNECTED,
    //                                &rilVer, sizeof(rilVer), socket_id);

    // implicit radio state changed
    RIL_UNSOL_RESPONSE(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED,
                                    NULL, 0, socket_id);

    // Send last NITZ time data, in case it was missed
    if (s_lastNITZTimeData != NULL) {
        resendLastNITZTimeData(socket_id);
    }

    // Get version string
    if (socket_id == (RIL_get3GSIM()-1)) {
        if (s_callbacks.getVersion != NULL) {
            const char *version;
            version = s_callbacks.getVersion();
            RLOGI("RIL Daemon version: %s\n", version);

            property_set(PROPERTY_RIL_IMPL, version);
        } else {
            RLOGI("RIL Daemon version: unavailable\n");
            property_set(PROPERTY_RIL_IMPL, "unavailable");
        }
    }

}

static void listenCallback (int fd, short flags, void *param) {
    int ret;
    int err;
    int is_phone_socket;
    int fdCommand = -1;
    const char* processName;
    RecordStream *p_rs;
    MySocketListenParam* listenParam;
    RilSocket *sapSocket = NULL;
    socketClient *sClient = NULL;

    SocketListenParam *p_info = (SocketListenParam *)param;

    if(RIL_SAP_SOCKET == p_info->type) {
        listenParam = (MySocketListenParam *)param;
        sapSocket = listenParam->socket;
    }

    struct sockaddr_un peeraddr;
    socklen_t socklen = sizeof (peeraddr);

    struct ucred creds;
    socklen_t szCreds = sizeof(creds);

    struct passwd *pwd = NULL;

    if(NULL == sapSocket) {
        assert (p_info->fdCommand < 0);
        assert (fd == p_info->fdListen);
        processName = PHONE_PROCESS;
    } else {
        assert (sapSocket->getCommandFd() < 0);
        assert (fd == sapSocket->getListenFd());
        // In order to support C2K SAP, we use ril-proxy to connect
        // to BT. And ril-proxy forwards request to gsm or c2k.
        processName = PHONE_PROCESS;
    }


    fdCommand = accept(fd, (sockaddr *) &peeraddr, &socklen);

    if (fdCommand < 0 ) {
        RLOGE("Error on accept() errno:%d", errno);
        /* start listening for new connections again */
        if(NULL == sapSocket) {
            rilEventAddWakeup(p_info->listen_event);
        } else {
            rilEventAddWakeup(sapSocket->getListenEvent());
        }
        return;
    }

    /* check the credential of the other side and only accept socket from
     * phone process
     */
    errno = 0;
    is_phone_socket = 0;

    err = getsockopt(fdCommand, SOL_SOCKET, SO_PEERCRED, &creds, &szCreds);

    if (err == 0 && szCreds > 0) {
        errno = 0;
        pwd = getpwuid(creds.uid);
        if (pwd != NULL) {
            if (strcmp(pwd->pw_name, processName) == 0) {
                is_phone_socket = 1;
            } else {
                RLOGE("RILD can't accept socket from process %s", pwd->pw_name);
            }
        } else {
            RLOGE("Error on getpwuid() errno: %d", errno);
        }
    } else {
        RLOGD("Error on getsockopt() errno: %d", errno);
    }

    if (!is_phone_socket) {
        RLOGE("RILD must accept socket from %s", processName);

      close(fdCommand);
      fdCommand = -1;

        if(NULL == sapSocket) {
            onCommandsSocketClosed(p_info->socket_id);

            /* start listening for new connections again */
            rilEventAddWakeup(p_info->listen_event);
        } else {
            sapSocket->onCommandsSocketClosed();

            /* start listening for new connections again */
            rilEventAddWakeup(sapSocket->getListenEvent());
        }

      return;
    }

    ret = fcntl(fdCommand, F_SETFL, O_NONBLOCK);

    if (ret < 0) {
        RLOGE ("Error setting O_NONBLOCK errno:%d", errno);
    }

    if(NULL == sapSocket) {
        RLOGI("libril: new connection to %s", rilSocketIdToString(p_info->socket_id));

        p_info->fdCommand = fdCommand;
        p_rs = record_stream_new(p_info->fdCommand, MAX_COMMAND_BYTES);
        p_info->p_rs = p_rs;

        ril_event_set (p_info->commands_event, p_info->fdCommand, 1,
                p_info->processCommandsCallback, p_info);
        rilEventAddWakeup (p_info->commands_event);

        onNewCommandConnect(p_info->socket_id);
    #ifdef MTK_RIL
        pthread_mutex_lock(&s_pendingUrcMutex[p_info->socket_id]);
        sendPendedUrcs(p_info->socket_id, p_info->fdCommand);
        pthread_mutex_unlock(&s_pendingUrcMutex[p_info->socket_id]);
    #endif
    } else {
        RLOGI("libril: new connection");

        sapSocket->setCommandFd(fdCommand);
        p_rs = record_stream_new(sapSocket->getCommandFd(), MAX_COMMAND_BYTES);
        sClient = new socketClient(sapSocket,p_rs);
        ril_event_set (sapSocket->getCallbackEvent(), sapSocket->getCommandFd(), 1,
        sapSocket->getCommandCb(), sClient);

        rilEventAddWakeup(sapSocket->getCallbackEvent());
        sapSocket->onNewCommandConnect();
    }
}

static void freeDebugCallbackArgs(int number, char **args) {
    for (int i = 0; i < number; i++) {
        if (args[i] != NULL) {
            free(args[i]);
        }
    }
    free(args);
}

static void debugCallback (int fd, short flags, void *param) {
    int acceptFD, option;
    struct sockaddr_un peeraddr;
    socklen_t socklen = sizeof (peeraddr);
    int data;
    unsigned int qxdm_data[6];
    const char *deactData[1] = {"1"};
    char *actData[1];
    RIL_Dial dialData;
    int hangupData[1] = {1};
    int number;
    char **args;
    RIL_SOCKET_ID socket_id = RIL_SOCKET_1;
    int sim_id = 0;

    RLOGI("debugCallback for socket %s", rilSocketIdToString(socket_id));

    acceptFD = accept (fd,  (sockaddr *) &peeraddr, &socklen);

    if (acceptFD < 0) {
        RLOGE ("error accepting on debug port: %d\n", errno);
        return;
    }

    if (recv(acceptFD, &number, sizeof(int), 0) != sizeof(int)) {
        RLOGE ("error reading on socket: number of Args: \n");
        close(acceptFD);
        return;
    }

    if (number <= 0) {
        RLOGE ("Invalid number of arguments: \n");
        close(acceptFD);
        return;
    }

    args = (char **) calloc(number, sizeof(char*));
    if (args == NULL) {
        RLOGE("Memory allocation failed for debug args");
        close(acceptFD);
        return;
    }

    for (int i = 0; i < number; i++) {
        int len;
        if (recv(acceptFD, &len, sizeof(int), 0) != sizeof(int)) {
            RLOGE ("error reading on socket: Len of Args: \n");
            freeDebugCallbackArgs(i, args);
            close(acceptFD);
            return;
        }
        if (len >= (INT_MAX - 1) || len <= 0) {
            RLOGE("Invalid value of len: \n");
            freeDebugCallbackArgs(i, args);
            close(acceptFD);
            return;
        }

        // +1 for null-term
        args[i] = (char *) calloc(len + 1, sizeof(char));
        if (args[i] == NULL) {
            RLOGE("Memory allocation failed for debug args");
            freeDebugCallbackArgs(i, args);
            close(acceptFD);
            return;
        }
        if (recv(acceptFD, args[i], sizeof(char) * len, 0)
            != (int)sizeof(char) * len) {
            RLOGE ("error reading on socket: Args[%d] \n", i);
            freeDebugCallbackArgs(i, args);
            close(acceptFD);
            return;
        }
        char * buf = args[i];
        buf[len] = 0;
        if ((i+1) == number) {
            /* The last argument should be sim id 0(SIM1)~3(SIM4) */
            sim_id = atoi(args[i]);
            switch (sim_id) {
                case 0:
                    socket_id = RIL_SOCKET_1;
                    break;
                case 1:
                    socket_id = RIL_SOCKET_2;
                    break;
                case 2:
                    socket_id = RIL_SOCKET_3;
                    break;
                case 3:
                    socket_id = RIL_SOCKET_4;
                    break;
                default:
                    socket_id = RIL_SOCKET_1;
                    break;
            }
        }
    }

    switch (atoi(args[0])) {
        case 0:
            RLOGI ("Connection on debug port: issuing reset.");
            issueLocalRequestForResponse(RIL_REQUEST_RESET_RADIO, NULL, 0, socket_id);
            break;
        case 1:
            RLOGI ("Connection on debug port: issuing radio power off.");
            data = 0;
            issueLocalRequestForResponse(RIL_REQUEST_RADIO_POWER, &data, sizeof(int), socket_id);
            // Close the socket
            if (s_ril_param_socket[socket_id].fdCommand > 0) {
                close(s_ril_param_socket[socket_id].fdCommand);
                s_ril_param_socket[socket_id].fdCommand = -1;
            }
            break;
        case 2:
            RLOGI ("Debug port: issuing unsolicited voice network change.");
            RIL_UNSOL_RESPONSE(RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED, NULL, 0, socket_id);
            break;
        case 3:
            RLOGI ("Debug port: QXDM log enable.");
            qxdm_data[0] = 65536;     // head.func_tag
            qxdm_data[1] = 16;        // head.len
            qxdm_data[2] = 1;         // mode: 1 for 'start logging'
            qxdm_data[3] = 32;        // log_file_size: 32megabytes
            qxdm_data[4] = 0;         // log_mask
            qxdm_data[5] = 8;         // log_max_fileindex
            issueLocalRequestForResponse(RIL_REQUEST_OEM_HOOK_RAW, qxdm_data,
                              6 * sizeof(int), socket_id);
            break;
        case 4:
            RLOGI ("Debug port: QXDM log disable.");
            qxdm_data[0] = 65536;
            qxdm_data[1] = 16;
            qxdm_data[2] = 0;          // mode: 0 for 'stop logging'
            qxdm_data[3] = 32;
            qxdm_data[4] = 0;
            qxdm_data[5] = 8;
            issueLocalRequestForResponse(RIL_REQUEST_OEM_HOOK_RAW, qxdm_data,
                              6 * sizeof(int), socket_id);
            break;
        case 5:
            RLOGI("Debug port: Radio On");
            data = 1;
            issueLocalRequestForResponse(RIL_REQUEST_RADIO_POWER, &data, sizeof(int), socket_id);
            sleep(2);
            // Set network selection automatic.
            issueLocalRequestForResponse(RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC, NULL, 0, socket_id);
            break;
        case 6:
            if (number > 1) {
                RLOGI("Debug port: Setup Data Call, Apn :%s\n", args[1]);
                actData[0] = args[1];
                issueLocalRequestForResponse(RIL_REQUEST_SETUP_DATA_CALL, &actData,
                              sizeof(actData), socket_id);
            }
            break;
        case 7:
            RLOGI("Debug port: Deactivate Data Call");
            issueLocalRequestForResponse(RIL_REQUEST_DEACTIVATE_DATA_CALL, &deactData,
                              sizeof(deactData), socket_id);
            break;
        case 8:
            if (number > 1) {
                RLOGI("Debug port: Dial Call");
                dialData.clir = 0;
                dialData.address = args[1];
                issueLocalRequestForResponse(RIL_REQUEST_DIAL, &dialData, sizeof(dialData), socket_id);
            }
            break;
        case 9:
            RLOGI("Debug port: Answer Call");
            issueLocalRequestForResponse(RIL_REQUEST_ANSWER, NULL, 0, socket_id);
            break;
        case 10:
            RLOGI("Debug port: End Call");
            issueLocalRequestForResponse(RIL_REQUEST_HANGUP, &hangupData,
                              sizeof(hangupData), socket_id);
            break;
        default:
            RLOGE ("Invalid request");
            break;
    }
    freeDebugCallbackArgs(number, args);
    close(acceptFD);
}

static void oemCallback (int fd, short flags, void *param) {
    int acceptFD;
    struct sockaddr_un peeraddr;
    socklen_t socklen = sizeof (peeraddr);
    int number;
    char **args = NULL;

    acceptFD = accept (fd,  (sockaddr *) &peeraddr, &socklen);

    if (acceptFD < 0) {
        LOGE ("error accepting on oem port: %d\n", errno);
        return;
    }

    s_fdOem_command = acceptFD;

    if (recv(acceptFD, &number, sizeof(int), 0) != sizeof(int)) {
        LOGE ("error reading on socket: number of Args: \n");
        close(s_fdOem_command);
        s_fdOem_command = -1;
        return;
    }
    LOGI ("NUMBER:%d", number);
    if (number != 1) {
        LOGE("invalid input number");
        close(s_fdOem_command);
        s_fdOem_command = -1;
        return;
    }

    args = (char **) calloc(1, sizeof(char*) * number);
    if (args == NULL) {
        RLOGE("OOM");
        close(s_fdOem_command);
        s_fdOem_command = -1;
        return;
    }
    for (int i = 0; i < number; i++) {
        unsigned int len;
        if (recv(acceptFD, &len, sizeof(int), 0) != sizeof(int)) {
            LOGE ("error reading on socket: Len of Args: \n");
            goto error;
        }
        // +1 for null-term

        LOGI ("arg len:%d", len);
        if (len >= (INT_MAX - 1) || len <= 0) {
            RLOGE("The length of Args reached MAX_UINT_32.");
            goto error;
        }

        args[i] = (char *) calloc(1, (sizeof(char) * len) + 1);
        if (args[i] == NULL) {
            RLOGE("OOM");
            goto error;
        }
        if (recv(acceptFD, args[i], sizeof(char) * len, 0)
                != (int)(sizeof(char) * len)) {
            LOGE ("error reading on socket: Args[%d] \n", i);
            goto error;
        }
        char * buf = args[i];
        buf[len] = 0;

        LOGI ("ARGS[%d]:%s",i, buf);
    }

    if(0 < handleSpecialRequestWithArgs(number, args)){
        freeDebugCallbackArgs(number, args);
    } else {
        LOGI("Oem port: SpecialRequest not support");
        goto error;
    }
    return;

error :
    freeDebugCallbackArgs(number, args);
    close(s_fdOem_command);
    s_fdOem_command = -1;
}

static void skipWhiteSpace(char **p_cur)
{
    if (*p_cur == NULL) return;

    while (**p_cur != '\0' && isspace(**p_cur)) {
        (*p_cur)++;
    }
}

static void skipNextComma(char **p_cur)
{
    if (*p_cur == NULL) return;

    while (**p_cur != '\0' && **p_cur != ',') {
        (*p_cur)++;
    }

    if (**p_cur == ',') {
        (*p_cur)++;
    }
}

static char * nextTok(char **p_cur)
{
    char *ret = NULL;

    skipWhiteSpace(p_cur);

    if (*p_cur == NULL) {
        ret = NULL;
    } else if (**p_cur == '"') {
        (*p_cur)++;
        ret = strsep(p_cur, "\"");
        skipNextComma(p_cur);
    } else {
        ret = strsep(p_cur, ",");
    }

    return ret;
}

int at_tok_nextstr(char **p_cur, char **p_out)
{
    if (*p_cur == NULL) {
        return -1;
    }

    *p_out = nextTok(p_cur);

    return 0;
}

/** returns 1 on "has more tokens" and 0 if no */
int at_tok_hasmore(char **p_cur)
{
    return ! (*p_cur == NULL || **p_cur == '\0');
}

/**
 * Parses the next integer in the AT response line and places it in *p_out
 * returns 0 on success and -1 on fail
 * updates *p_cur
 * "base" is the same as the base param in strtol
 */

static int at_tok_nextint_base(char **p_cur, int *p_out, int base, int  uns)
{
    char *ret;

    if (*p_cur == NULL) {
        return -1;
    }

    ret = nextTok(p_cur);

    if (ret == NULL) {
        return -1;
    } else {
        long l;
        char *end;

        if (uns)
            l = strtoul(ret, &end, base);
        else
            l = strtol(ret, &end, base);

        *p_out = (int)l;

        if (end == ret) {
            return -1;
        }
    }

    return 0;
}

/**
 * Parses the next base 10 integer in the AT response line
 * and places it in *p_out
 * returns 0 on success and -1 on fail
 * updates *p_cur
 */
int at_tok_nextint(char **p_cur, int *p_out)
{
    return at_tok_nextint_base(p_cur, p_out, 10, 0);
}

static int handleSpecialRequestWithArgs(int argCount, char** args){
    char *line, *cmd = NULL;
    int err;
    int slotId = 0;
    char *param[2] = {NULL, NULL};
    RIL_SOCKET_ID socket_id = RIL_SOCKET_1;
    char sim[PROPERTY_VALUE_MAX] ={0};
    int sim_status = 0;
    int simId3G = 0;
    char org_args[768] = {0};
    int modemOn = 0;

    if (1 == argCount)
    {
        line = args[0];
        strncpy(org_args, args[0], strlen(args[0]));
        err = at_tok_nextstr(&line,&cmd);
        if (err < 0) {
            LOGD("invalid command");
            goto error;
        }
        //LOGD("handleSpecialRequestWithArgs cmd = %s", cmd);

        if (at_tok_hasmore(&line)) {
            err = at_tok_nextint(&line,&slotId);
            if (err < 0 || slotId >= SIM_COUNT) {
                LOGD("invalid slotId, %d", slotId);
                goto error;
            }
            //LOGD("handleSpecialRequestWithArgs slotId = %d", slotId);
        }

        if(at_tok_hasmore(&line)) {
            err = at_tok_nextstr(&line,&param[0]);
            if (err < 0) {
                LOGD("invalid param");
                goto error;
            }
            //LOGD("handleSpecialRequestWithArgs param[0] = %s", param[0]);
        }

        if (at_tok_hasmore(&line)) {
            err = at_tok_nextstr(&line, &param[1]);
            if (err < 0) {
                goto error;
            }
            //LOGD("handleSpecialRequestWithArgs param[1] = %s", param[1]);
        }

        if (strcmp(cmd, "THERMAL") == 0) {
            if(s_THERMAL_fd > 0) {
                close(s_THERMAL_fd); // close previous fd, avoid fd leak
            }
            s_THERMAL_fd = s_fdOem_command;
            strncpy(args[0], org_args, strlen(org_args));
            line = args[0];
            //LOGD("Thermal line = %s", line);
            err = at_tok_nextstr(&line,&cmd);
            err = at_tok_nextint(&line,&slotId);
            char prop_value[PROPERTY_VALUE_MAX] = { 0 };
            int targetSim = 0;
            property_get(PROPERTY_3G_SIM, prop_value, "1");
            targetSim = atoi(prop_value) - 1;
            //LOGD("Thermal line = %s, cmd:%s, slotId:%d, targetSim: %d", line, cmd, slotId, targetSim);
            issueLocalRequestForResponse(RIL_LOCAL_REQUEST_QUERY_MODEM_THERMAL, &line,1, (RIL_SOCKET_ID)targetSim);
            return 1;
        } else if (strcmp(cmd, "AT+ERFTX=1") == 0){
            issueLocalRequestForResponse(RIL_REQUEST_OEM_HOOK_RAW, org_args, strlen(org_args),socket_id);

            char* result = (char*)calloc(10, sizeof(char));
            if (result == NULL) {
                RLOGE("OOM");
                return 1;
            }
            strncpy(result, "OK", 2);
            int len = (int)strlen(result);
            int ret = send(s_fdOem_command, &len, sizeof(int), 0);
            LOGD("AT+ERFTX=1, fdOem:%d", s_fdOem_command);
            if (ret != sizeof(int)) {
                LOGD("Socket write Error: when sending arg length");
            } else {
                ret = send(s_fdOem_command, result, len, 0);
                if (ret != len) {
                    LOGD("lose data when send response. ");
                }
            }
            free(result);
            close(s_fdOem_command);
            s_fdOem_command = -1;

            return 1;
        } else if (strcmp(cmd, "MDTM_TOG") == 0) {
            strncpy(args[0], org_args, strlen(org_args));
            line = args[0];
            LOGD("Thermal line = %s", line);
            err = at_tok_nextstr(&line, &cmd);
            err = at_tok_nextint(&line, &modemOn);
            // LOGD("modemOn = %d", modemOn);
            issueLocalRequestForResponse(RIL_LOCAL_REQUEST_SET_MODEM_THERMAL, &modemOn, 1, socket_id);
            return 1;
        // MTK-START: SIM TMO RSU
        }else if(strcmp(cmd, "SIMMELOCK_GETKEY") == 0) {
            s_SIMLOCK_fd = s_fdOem_command;
            s_fdOem_command = -1;
            issueLocalRequestForResponse(RIL_LOCAL_REQUEST_GET_SHARED_KEY,NULL, 0,s_ril_cntx[slotId]);
            return 1;
        } else if(strcmp(cmd, "SIMMELOCK_SET") == 0) {
            s_SIMLOCK_fd = s_fdOem_command;
            s_fdOem_command = -1;
            issueLocalRequestForResponse(RIL_LOCAL_REQUEST_UPDATE_SIM_LOCK_SETTINGS,&param, 1,s_ril_cntx[slotId]);
            return 1;
        } else if(strcmp(cmd, "SIMMELOCK_GET_VERSION") == 0) {
            s_SIMLOCK_fd = s_fdOem_command;
            s_fdOem_command = -1;
            issueLocalRequestForResponse(RIL_LOCAL_REQUEST_GET_SIM_LOCK_INFO,NULL, 0,s_ril_cntx[slotId]);
            return 1;
        } else if(strcmp(cmd, "SIMMELOCK_RESET") == 0) {
            s_SIMLOCK_fd = s_fdOem_command;
            s_fdOem_command = -1;
            issueLocalRequestForResponse(RIL_LOCAL_REQUEST_RESET_SIM_LOCK_SETTINGS,NULL, 0,s_ril_cntx[slotId]);
            return 1;
        } else if(strcmp(cmd, "SIMMELOCK_GET_STATUS") == 0) {
            s_SIMLOCK_fd = s_fdOem_command;
            s_fdOem_command = -1;
            issueLocalRequestForResponse(RIL_LOCAL_REQUEST_GET_MODEM_STATUS,NULL, 0,s_ril_cntx[slotId]);
            return 1;
        // MTK-END
        } else {
            // invalid request
            LOGD("invalid request");
            goto error;
        }
    }
    else
    {
        goto error;
    }
error:
    LOGE ("Invalid request");

    return 0;
}

#ifdef MTK_RIL
void userTimerCallback (int fd, short flags, void *param) {
#else
static void userTimerCallback (int fd, short flags, void *param) {
#endif
    UserCallbackInfo *p_info;

    p_info = (UserCallbackInfo *)param;

#ifdef MTK_RIL
    if (p_info->cid != RIL_SUPPORT_CHANNELS)
    {
        enqueue(NULL, NULL, 0, p_info, RIL_SOCKET_1);
        return;
    }
    else
    {
        p_info->p_callback(p_info->userParam);
    }
#else
    p_info->p_callback(p_info->userParam);
#endif

#ifdef MTK_RIL
    pthread_mutex_lock(&s_last_wake_mutex);
#endif
    // FIXME generalize this...there should be a cancel mechanism
    if (s_last_wake_timeout_info != NULL && s_last_wake_timeout_info == p_info) {
#ifdef MTK_RIL
        RLOGD("s_last_wake_timeout_info: %p reset to NULL", s_last_wake_timeout_info);
#endif
        s_last_wake_timeout_info = NULL;
    }
#ifdef MTK_RIL
    else
    {
         RLOGD("s_last_wake_timeout_info: %p ", s_last_wake_timeout_info);
    }
    pthread_mutex_unlock(&s_last_wake_mutex);
#endif

#ifdef MTK_RIL
    if (p_info->cid < 0)
    {
        free(p_info);
    }
#else
    free(p_info);
#endif

}


static void *
eventLoop(void *param) {
    int ret;
    int filedes[2];

    ril_event_init();

    pthread_mutex_lock(&s_startupMutex);

    s_started = 1;
    pthread_cond_broadcast(&s_startupCond);

    pthread_mutex_unlock(&s_startupMutex);

    ret = pipe(filedes);

    if (ret < 0) {
        RLOGE("Error in pipe() errno:%d", errno);
        return NULL;
    }

    s_fdWakeupRead = filedes[0];
    s_fdWakeupWrite = filedes[1];

    fcntl(s_fdWakeupRead, F_SETFL, O_NONBLOCK);

    ril_event_set (&s_wakeupfd_event, s_fdWakeupRead, true,
                processWakeupCallback, NULL);

    rilEventAddWakeup (&s_wakeupfd_event);

    // Only returns on error
    ril_event_loop();
    RLOGE ("error in event_loop_base errno:%d", errno);
    // kill self to restart on error
    kill(0, SIGKILL);

    return NULL;
}

extern "C" void
RIL_startEventLoop(void) {
#ifdef MTK_RIL
    RIL_startRILProxys();
#endif /* MTK_RIL */

    /* spin up eventLoop thread and wait for it to get started */
    s_started = 0;
    pthread_mutex_lock(&s_startupMutex);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    int result = pthread_create(&s_tid_dispatch, &attr, eventLoop, NULL);
    if (result != 0) {
        RLOGE("Failed to create dispatch thread: %s", strerror(result));
        goto done;
    }

    while (s_started == 0) {
        pthread_cond_wait(&s_startupCond, &s_startupMutex);
    }

done:
    pthread_mutex_unlock(&s_startupMutex);
}

// Used for testing purpose only.
extern "C" void RIL_setcallbacks (const RIL_RadioFunctions *callbacks) {
    memcpy(&s_callbacks, callbacks, sizeof (RIL_RadioFunctions));
}

static void startListen(RIL_SOCKET_ID socket_id, SocketListenParam* socket_listen_p) {
    int fdListen = -1;
    int ret;
    char socket_name[10];

    memset(socket_name, 0, sizeof(char)*10);

    switch(socket_id) {
        case RIL_SOCKET_1:
            strncpy(socket_name, RIL_getRilSocketName(), 9);
            break;
        case RIL_SOCKET_2:
            strncpy(socket_name, SOCKET2_NAME_RIL, 9);
            break;
        case RIL_SOCKET_3:
            strncpy(socket_name, SOCKET3_NAME_RIL, 9);
            break;
        case RIL_SOCKET_4:
            strncpy(socket_name, SOCKET4_NAME_RIL, 9);
            break;
        default:
            RLOGE("Socket id is wrong!!");
            return;
    }

    RLOGI("Start to listen socket_name: %s, socketId: %s",
            socket_name, rilSocketIdToString(socket_id));

    fdListen = android_get_control_socket(socket_name);
    if (fdListen < 0) {
        RLOGE("Failed to get socket %s", socket_name);
        exit(-1);
    }

    ret = listen(fdListen, 4);

    if (ret < 0) {
        RLOGE("Failed to listen on control socket '%d': %s",
             fdListen, strerror(errno));
        exit(-1);
    }
    socket_listen_p->fdListen = fdListen;

    /* note: non-persistent so we can accept only one connection at a time */
    ril_event_set (socket_listen_p->listen_event, fdListen, false,
                listenCallback, socket_listen_p);

    rilEventAddWakeup (socket_listen_p->listen_event);
}

// External SIM [Start]
static int
sendVsimResponseRaw (const void *data, size_t dataSize, RIL_SOCKET_ID socket_id) {
    int fd = s_fdVsim_command[socket_id];
    int ret;
    uint32_t header;
    pthread_mutex_t * writeMutexHook = &s_writeVsimMutex[socket_id];

#if VDBG
    RLOGE("Send Response to %s, fd=%d", rilSocketIdToString(socket_id), fd);
#endif

    if (fd < 0) {
        RLOGE("Send Response, but fd is incorrect");
        return -1;
    }

    if (dataSize > MAX_COMMAND_BYTES) {
        RLOGE("RIL: packet larger than %u (%u)",
                MAX_COMMAND_BYTES, (unsigned int )dataSize);

        return -1;
    }

    pthread_mutex_lock(writeMutexHook);

    header = htonl(dataSize);

    ret = blockingWrite(fd, (void *)&header, sizeof(header));

    if (ret < 0) {
        pthread_mutex_unlock(writeMutexHook);
        return ret;
    }

    ret = blockingWrite(fd, data, dataSize);

    if (ret < 0) {
        pthread_mutex_unlock(writeMutexHook);
        return ret;
    }

    pthread_mutex_unlock(writeMutexHook);

    return 0;
}

static int
sendVsimResponse (Parcel &p, RIL_SOCKET_ID socket_id) {
    printResponse;
    return sendVsimResponseRaw(p.data(), p.dataSize(), socket_id);
}

static void processVsimCommandsCallback(int fd, short flags, void *param) {
    RecordStream *p_rs;
    void *p_record;
    size_t recordlen;
    int ret;
    SocketListenParam *p_info = (SocketListenParam *)param;

    assert(fd == p_info->fdCommand);

    p_rs = p_info->p_rs;

    for (;;) {
        /* loop until EAGAIN/EINTR, end of stream, or other error */
        ret = record_stream_get_next(p_rs, &p_record, &recordlen);

        if (p_info->fdCommand == s_fdVsim_command[p_info->socket_id]) {
            LOGI("processCommandsCallback read vsim data, ret=%d", ret);
        }

        if (ret == 0 && p_record == NULL) {
            /* end-of-stream */
            break;
        } else if (ret < 0) {
            //break;
        } else if (ret == 0) { /* && p_record != NULL */
            processCommandBuffer(p_record, recordlen, p_info->socket_id);
        }

    }

    if (ret == 0 || !(errno == EAGAIN || errno == EINTR)) {
        /* fatal error or end-of-stream */
        if (ret != 0) {
            RLOGE("error on reading command socket errno:%d\n", errno);
        } else {
            RLOGW("EOS.  Closing command socket.");
        }

        close(fd);
        p_info->fdCommand = -1;
        s_fdCommand[p_info->socket_id] = -1;

        //ril_event_del(p_info->commands_event);

        record_stream_free(p_info->p_rs);

        /* start listening for new connections again */
        //rilEventAddWakeup(&s_listen_event[p_info->socket_id]);

        onCommandsSocketClosed(p_info->socket_id);
    }
}

static const char SOCKET_NAME_VSIM[4][25] = {
    "rild-vsim",
    "rild-vsim2",
    "rild-vsim3",
    "rild-vsim4",
};

static void vsimCallback (int fd, short flags, void *param)
{
    int acceptFD;
    struct sockaddr_un peeraddr;
    socklen_t socklen = sizeof (peeraddr);
    RecordStream *p_rs;
    void *p_record;
    size_t recordlen;
    int ret;
    RIL_SOCKET_ID socketId = RIL_SOCKET_1;

    char *inst = (char *)param;
    for (int i = 0; i < SIM_COUNT; i++) {
        if (strcmp(inst, SOCKET_NAME_VSIM[i]) == 0) {
            socketId = (RIL_SOCKET_ID)(RIL_SOCKET_1 + i);
            break;
        }
    }
    LOGI ("[vsimCallback] enter, inst = %s, socketId = %d", inst, socketId);
    free(inst);

    SocketListenParam *p_info = (SocketListenParam *)calloc(1, sizeof(SocketListenParam));
    assert (p_info != NULL);

    while (true) {
        LOGI ("[vsimCallback] fd = %d", s_fdVsim[socketId]);

        acceptFD = accept(s_fdVsim[socketId],  (sockaddr *) &peeraddr, &socklen);
        s_fdVsim_command[socketId] = acceptFD;

        LOGI ("[vsimCallback] socektID=%d, acceptFD=%d", socketId, s_fdVsim_command[socketId]);

        if (acceptFD < 0) {
            LOGE ("[vsimCallback] error accepting on vsim port: %d\n", errno);
            return ;
        }

        LOGI ("[vsimCallback] before sendPendedVsimUrcs[%d]", socketId);
        pthread_mutex_lock(&s_pendingVsimUrcMutex[p_info->socket_id]);
        sendPendedVsimUrcs(socketId, s_fdVsim_command[socketId]);
        pthread_mutex_unlock(&s_pendingVsimUrcMutex[p_info->socket_id]);
        LOGI ("[vsimCallback] after sendPendedVsimUrcs[%d]", socketId);

        p_info->socket_id = socketId;
        p_info->fdCommand = s_fdVsim_command[socketId];
        p_rs = record_stream_new(p_info->fdCommand, MAX_COMMAND_BYTES);
        p_info->p_rs = p_rs;


        //ril_event_set (&s_vsim_commands_event[socketId], p_info->fdCommand, 1,
        //        processCommandsCallback, p_info);
        //rilEventAddWakeup (&s_vsim_commands_event[socketId]);

        processVsimCommandsCallback(s_fdVsim_command[socketId], 0, p_info);

        //s_fdVsim_command[socketId] = 0;
        LOGI ("[vsimCallback] processCommandsCallback end.");
    }

    return ;
}

static void* startListenVsim(void *param) {
    int fdListen = -1;
    int ret;
    RIL_SOCKET_ID socketId = RIL_SOCKET_1;

    char *inst = (char *)param;
    for (int i = 0; i < SIM_COUNT; i++) {
        if (strcmp(inst, SOCKET_NAME_VSIM[i]) == 0) {
            socketId = (RIL_SOCKET_ID)(RIL_SOCKET_1 + i);
            break;
        }
    }
    LOGI ("[startListenVsim] enter, inst = %s, socketId = %d", inst, socketId);

    ret = listen(s_fdVsim[socketId], 4);

    if (ret < 0) {
        RLOGE("Failed to listen on control socket '%d': %s",
             s_fdVsim[socketId], strerror(errno));
        exit(-1);
    }

    /* note: non-persistent so we can accept only one connection at a time */
    //ril_event_set (&s_vsim_listen_event[socketId], s_fdVsim[socketId], false,
    //            vsimCallback, param);

    //rilEventAddWakeup (&s_vsim_listen_event[socketId]);
    vsimCallback(0, 0, param);

    return NULL;
}
// External SIM [End]


extern "C" void
RIL_register(const RIL_RadioFunctions *callbacks) {
    int ret;
    int flags;
    int i = 0;

    RLOGI("SIM_COUNT: %d", SIM_COUNT);
    ProcessState::initWithDriver("/dev/vndbinder");
    if (callbacks == NULL) {
        RLOGE("RIL_register: RIL_RadioFunctions * null");
        return;
    }
    if (callbacks->version < RIL_VERSION_MIN) {
        RLOGE("RIL_register: version %d is to old, min version is %d",
             callbacks->version, RIL_VERSION_MIN);
        return;
    }

    RLOGE("RIL_register: RIL version %d", callbacks->version);

    if (s_registerCalled > 0) {
        RLOGE("RIL_register has been called more than once. "
                "Subsequent call ignored");
        return;
    }

    memcpy(&s_callbacks, callbacks, sizeof (RIL_RadioFunctions));

    /* Initialize socket1 parameters */
    for (i = 0; i < SIM_COUNT; i++) {
        s_ril_param_socket[i] = {
                            (RIL_SOCKET_ID)(RIL_SOCKET_1+i),             /* socket_id */
                            -1,                       /* fdListen */
                            -1,                       /* fdCommand */
                            PHONE_PROCESS,            /* processName */
                            &s_commands_event[i],        /* commands_event */
                            &s_listen_event[i],          /* listen_event */
                            processCommandsCallback,  /* processCommandsCallback */
                            NULL,                     /* p_rs */
                            RIL_TELEPHONY_SOCKET      /* type */
                            };
    }

    s_registerCalled = 1;

    RLOGI("s_registerCalled flag set, %d", s_started);

    // New rild impl calls RIL_startEventLoop() first
    // old standalone impl wants it here.

    if (s_started == 0) {
        RIL_startEventLoop();
    }

    // start listen socket1
    for (i = 0; i < SIM_COUNT; i++) {
        startListen((RIL_SOCKET_ID)(RIL_SOCKET_1+i), &s_ril_param_socket[i]);
    }

#if 1
    // start debug interface socket

    char *inst = NULL;
    if (strlen(RIL_getRilSocketName()) >= strlen(SOCKET_NAME_RIL)) {
        inst = RIL_getRilSocketName() + strlen(SOCKET_NAME_RIL);
    }

    char rildebug[MAX_DEBUG_SOCKET_NAME_LENGTH] = SOCKET_NAME_RIL_DEBUG;
    if (inst != NULL) {
        strlcat(rildebug, inst, MAX_DEBUG_SOCKET_NAME_LENGTH);
    }

    s_fdDebug = android_get_control_socket(rildebug);
    if (s_fdDebug < 0) {
        RLOGE("Failed to get socket : %s errno:%d", rildebug, errno);
        exit(-1);
    }

    ret = listen(s_fdDebug, 4);

    if (ret < 0) {
        RLOGE("Failed to listen on ril debug socket '%d': %s",
             s_fdDebug, strerror(errno));
        exit(-1);
    }
    ril_event_set (&s_debug_event, s_fdDebug, true,
                debugCallback, NULL);

    rilEventAddWakeup (&s_debug_event);
#endif

    char rildoem[MAX_DEBUG_SOCKET_NAME_LENGTH] = SOCKET_NAME_RIL_OEM;

    /* oem start */
    s_fdOem= android_get_control_socket(rildoem);
    if (s_fdOem < 0) {
        RLOGE("Failed to get socket : %s errno:%d", rildebug, errno);
        exit(-1);
    }
    ret = listen(s_fdOem, 4);

    if (ret < 0) {
        RLOGE("Failed to listen on ril OEM socket '%d': %s",
             s_fdOem, strerror(errno));
        exit(-1);
    }
    ril_event_set (&s_oem_event, s_fdOem, true,
                oemCallback, NULL);

    rilEventAddWakeup (&s_oem_event);

    // External SIM [Start]
    for (i = 0; i < SIM_COUNT; i++) {
        s_fdVsim_command[i] = -1;
        s_fdVsim[i] = android_get_control_socket(SOCKET_NAME_VSIM[i]);
        RLOGD("create vsim socket(%s), fd = %d", SOCKET_NAME_VSIM[i], s_fdVsim[i]);

        if (s_fdVsim[i] < 0) {
            RLOGE("Failed to get socket : %s errno:%d", SOCKET_NAME_VSIM[i], errno);
            //exit(-1);
        } else {
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

            char *socket_name = NULL;
            asprintf(&socket_name, "%s", SOCKET_NAME_VSIM[i]);
            int result = pthread_create(&s_vsim_dispatch, &attr, startListenVsim, (void *)socket_name);
            if (result != 0) {
                RLOGE("Failed to create dispatch thread: %s", strerror(result));
            }
        }
    }
    // External SIM [End]

    // initialize mutex
    for (int i = 0; i < MAX_SIM_COUNT; i++) {
        pthread_mutex_init(&s_pendingUrcMutex[i], NULL);
    }
    RLOGE("finish RIL_registerSocket");
}

extern "C" void
RIL_register_socket (RIL_RadioFunctions *(*Init)(const struct RIL_Env *, int, char **),
        RIL_SOCKET_TYPE socketType, int argc, char **argv) {
    RIL_RadioFunctions* UimFuncs = NULL;
    char socket_name[4][30];

    if(Init) {
        UimFuncs = Init(&RilSapSocket::uimRilEnv, argc, argv);

        switch(socketType) {
            case RIL_SAP_SOCKET:
                strncpy(socket_name[0], SOCKET_NAME_SAP, 29);
                if (SIM_COUNT >= 2) {
                    strncpy(socket_name[1], SOCKET2_NAME_SAP, 29);
                }
                if (SIM_COUNT >= 3) {
                    strncpy(socket_name[2], SOCKET3_NAME_SAP, 29);
                }
                if (SIM_COUNT >= 4) {
                    strncpy(socket_name[3], SOCKET4_NAME_SAP, 29);
                }
                break;
            default:;
        }

        for (int i = 0; i < SIM_COUNT; i++) {
            RilSapSocket::initSapSocket(socket_name[i], UimFuncs);
        }
    }
}

// Check and remove RequestInfo if its a response and not just ack sent back
static int
checkAndDequeueRequestInfoIfAck(struct RequestInfo *pRI, bool isAck) {
    int ret = 0;
    /* Hook for current context
       pendingRequestsMutextHook refer to &s_pendingRequestsMutex */
    pthread_mutex_t* pendingRequestsMutexHook = &s_pendingRequestsMutex[pRI->socket_id];
    /* pendingRequestsHook refer to &s_pendingRequests */
    RequestInfo ** pendingRequestsHook = &s_pendingRequests[pRI->socket_id];

    if (pRI == NULL) {
        return 0;
    }

    pthread_mutex_lock(pendingRequestsMutexHook);

    for(RequestInfo **ppCur = pendingRequestsHook
        ; *ppCur != NULL
        ; ppCur = &((*ppCur)->p_next)
    ) {
        if (pRI == *ppCur) {
            ret = 1;
            if (isAck) { // Async ack
                if (pRI->wasAckSent == 1) {
                    RLOGD("Ack was already sent for %s", requestToString(pRI->pCI->requestNumber));
                } else {
                    pRI->wasAckSent = 1;
                }
            } else {
                *ppCur = (*ppCur)->p_next;
            }
            break;
        }
    }

    pthread_mutex_unlock(pendingRequestsMutexHook);

    return ret;
}

static int findFd(int socket_id) {
    int fd = s_ril_param_socket[socket_id].fdCommand;
    return fd;
}

extern "C" void
RIL_onRequestAck(RIL_Token t) {
    RequestInfo *pRI;
    int ret, fd;

    size_t errorOffset;
    RIL_SOCKET_ID socket_id = RIL_SOCKET_1;

    pRI = (RequestInfo *)t;

    if (!checkAndDequeueRequestInfoIfAck(pRI, true)) {
        RLOGE ("RIL_onRequestAck: invalid RIL_Token");
        return;
    }

    socket_id = pRI->socket_id;
    fd = findFd(socket_id);

#if VDBG
    RLOGD("Request Ack, %s", rilSocketIdToString(socket_id));
#endif

    appendPrintBuf("Ack [%04d]< %s", pRI->token, requestToString(pRI->pCI->requestNumber));

    if (pRI->cancelled == 0) {
        Parcel p;

        p.writeInt32 (RESPONSE_SOLICITED_ACK);
        p.writeInt32 (pRI->token);

        if (fd < 0) {
            RLOGD ("RIL onRequestComplete: Command channel closed");
        }

        sendResponse(p, socket_id);
    }
}

extern "C" void
RIL_onRequestComplete(RIL_Token t, RIL_Errno e, void *response, size_t responselen) {
    RequestInfo *pRI;
    int ret;
    int fd = -1;
    size_t errorOffset;
    RIL_SOCKET_ID socket_id = RIL_SOCKET_1;

    pRI = (RequestInfo *)t;

    if (!checkAndDequeueRequestInfoIfAck(pRI, false)) {
        RLOGE ("RIL_onRequestComplete: invalid RIL_Token");
        return;
    }

    socket_id = pRI->socket_id;
    fd = findFd(socket_id);
#if VDBG
    RLOGD("RequestComplete, %s", rilSocketIdToString(socket_id));
#endif

    if (pRI->local > 0) {
        // Locally issued command...void only!
        // response does not go back up the command socket
        RLOGD("C[locl]< %s", requestToString(pRI->pCI->requestNumber));

        free(pRI);
        return;
    }

    appendPrintBuf("[%04d]< %s",
        pRI->token, requestToString(pRI->pCI->requestNumber));

    if(pRI->pCI->requestNumber == RIL_LOCAL_REQUEST_QUERY_MODEM_THERMAL){

        LOGD("[THERMAL] local request for THERMAL returned ");
        char* strResult = NULL;
        if(RIL_E_SUCCESS == e){
            asprintf(&strResult, "%s",(char*)response);
        } else {
            asprintf(&strResult, "ERROR");

        }

        if(s_THERMAL_fd > 0){
            LOGD("[THERMAL] s_THERMAL_fd is valid strResult is %s", strResult);

            int len = (int)strlen(strResult);
            ret = send(s_THERMAL_fd, strResult, len, MSG_NOSIGNAL);
            if (ret != len) {
                LOGD("[THERMAL] lose data when send response. ");
            }
            free(strResult);
            goto done;
        } else {
            LOGD("[EAP] s_THERMAL_fd is < 0");
            free(strResult);
            goto done;
        }

    }

    if (pRI->pCI->requestNumber == RIL_LOCAL_REQUEST_SET_MODEM_THERMAL) {
            LOGD("[MDTM_TOG] Not need to send response");
            goto done;
    }

    // MTK-START: SIM TMO RSU
    if(pRI->pCI->requestNumber == RIL_LOCAL_REQUEST_GET_SHARED_KEY
            || pRI->pCI->requestNumber == RIL_LOCAL_REQUEST_UPDATE_SIM_LOCK_SETTINGS
            || pRI->pCI->requestNumber == RIL_LOCAL_REQUEST_GET_SIM_LOCK_INFO
            || pRI->pCI->requestNumber == RIL_LOCAL_REQUEST_RESET_SIM_LOCK_SETTINGS
            || pRI->pCI->requestNumber == RIL_LOCAL_REQUEST_GET_MODEM_STATUS){

        LOGD("[SIM_LOCK] local request for SIM_LOCK returned ");

        char* strResult = NULL;
        if(RIL_E_SUCCESS == e){
            asprintf(&strResult, "%s",(char*)response);
        } else {
            asprintf(&strResult, "ERROR:%d", e);
        }

        if(s_SIMLOCK_fd > 0){
            LOGD("[SIM_LOCK] s_SIMLOCK_fd is valid strResult is %s", strResult);

            int len = (int)strlen(strResult);

            ret = send(s_SIMLOCK_fd, &len, sizeof(int), 0);
            ret = send(s_SIMLOCK_fd, strResult, len, 0);
            if (ret != len) {
                LOGD("[SIM_LOCK] lose data when send response. ");
            }
            close(s_SIMLOCK_fd);
            s_SIMLOCK_fd = -1;
            free(strResult);
            goto done;
        } else {
            LOGD("[SIM_LOCK] s_SIMLOCK_fd is < 0");
            free(strResult);
            goto done;
        }
    }
    // MTK-END

    if (pRI->cancelled == 0) {
        Parcel p;

        int responseType;
        if (s_callbacks.version >= 13 && pRI->wasAckSent == 1) {
            // If ack was already sent, then this call is an asynchronous response. So we need to
            // send id indicating that we expect an ack from RIL.java as we acquire wakelock here.
            responseType = RESPONSE_SOLICITED_ACK_EXP;
            grabPartialWakeLock();
        } else {
            responseType = RESPONSE_SOLICITED;
        }
        p.writeInt32 (responseType);

        p.writeInt32 (pRI->token);
        errorOffset = p.dataPosition();

        p.writeInt32 (e);

        if (response != NULL) {
            // there is a response payload, no matter success or not.
            RLOGD("Calling responseFunction() for token %d", pRI->token);
            ret = pRI->pCI->responseFunction(p, response, responselen);

            /* if an error occurred, rewind and mark it */
            if (ret != 0) {
                RLOGE ("responseFunction error, ret %d", ret);
                p.setDataPosition(errorOffset);
                p.writeInt32 (ret);
            }
        }

        if (e != RIL_E_SUCCESS) {
            appendPrintBuf("%s fails by %s", printBuf, failCauseToString(e));
        }

        if (fd < 0) {
            RLOGD ("RIL onRequestComplete: Command channel closed");
        }
        // External SIM [Start]
        if (fd < 0 && (pRI->pCI->requestNumber == RIL_REQUEST_VSIM_NOTIFICATION
                || pRI->pCI->requestNumber == RIL_REQUEST_VSIM_OPERATION)) {
            sendVsimResponse(p, socket_id);
        } else {
        // External SIM [End]
            sendResponse(p, socket_id);
        }
    }

done:
    free(pRI);
}

extern "C"
void resetWakelock(void) {
    RLOGD("reset Wakelock %s", ANDROID_WAKE_LOCK_NAME);
    release_wake_lock(ANDROID_WAKE_LOCK_NAME);
}

static void
grabPartialWakeLock() {
    if (s_callbacks.version >= 13) {
        int ret;
        ret = pthread_mutex_lock(&s_wakeLockCountMutex);
        assert(ret == 0);
        acquire_wake_lock(PARTIAL_WAKE_LOCK, ANDROID_WAKE_LOCK_NAME);

        UserCallbackInfo *p_info =
                internalRequestTimedCallback(wakeTimeoutCallback, NULL, &TIMEVAL_WAKE_TIMEOUT);
        if (p_info == NULL) {
            release_wake_lock(ANDROID_WAKE_LOCK_NAME);
        } else {
            s_wakelock_count++;
            if (s_last_wake_timeout_info != NULL) {
                s_last_wake_timeout_info->userParam = (void *)1;
            }
            s_last_wake_timeout_info = p_info;
        }
        ret = pthread_mutex_unlock(&s_wakeLockCountMutex);
        assert(ret == 0);
    } else {
        acquire_wake_lock(PARTIAL_WAKE_LOCK, ANDROID_WAKE_LOCK_NAME);
    }
}

void
releaseWakeLock() {
    if (s_callbacks.version >= 13) {
        int ret;
        ret = pthread_mutex_lock(&s_wakeLockCountMutex);
        assert(ret == 0);

        if (s_wakelock_count > 1) {
            s_wakelock_count--;
        } else {
            s_wakelock_count = 0;
            release_wake_lock(ANDROID_WAKE_LOCK_NAME);
            if (s_last_wake_timeout_info != NULL) {
                s_last_wake_timeout_info->userParam = (void *)1;
            }
        }

        ret = pthread_mutex_unlock(&s_wakeLockCountMutex);
        assert(ret == 0);
    } else {
        release_wake_lock(ANDROID_WAKE_LOCK_NAME);
    }
}

/**
 * Timer callback to put us back to sleep before the default timeout
 */
static void
wakeTimeoutCallback (void *param) {
    // We're using "param != NULL" as a cancellation mechanism
    if (s_callbacks.version >= 13) {
        if (param == NULL) {
            int ret;
            ret = pthread_mutex_lock(&s_wakeLockCountMutex);
            assert(ret == 0);
            s_wakelock_count = 0;
            release_wake_lock(ANDROID_WAKE_LOCK_NAME);
            ret = pthread_mutex_unlock(&s_wakeLockCountMutex);
            assert(ret == 0);
        }
    } else {
        if (param == NULL) {
            releaseWakeLock();
        }
    }
}

#if defined(ANDROID_MULTI_SIM)
extern "C"
void RIL_onUnsolicitedResponse(int unsolResponse, const void *data,
                                size_t datalen, RIL_SOCKET_ID socket_id)
#else
extern "C"
void RIL_onUnsolicitedResponse(int unsolResponse, const void *data,
                                size_t datalen)
#endif
{
    int ret;
    int64_t timeReceived = 0;
    bool shouldScheduleTimeout = false;
    RIL_RadioState newState;
    RIL_SOCKET_ID soc_id = RIL_SOCKET_1;
    UnsolResponseInfo *pUnsolRi = NULL;
    int i = 0;

#ifdef MTK_RIL
    int fdCommand = -1;
    WakeType wakeType = WAKE_PARTIAL;
#endif
#if defined(ANDROID_MULTI_SIM)
    soc_id = socket_id;
#endif


    if (s_registerCalled == 0) {
        cacheUrc(unsolResponse, data, datalen, soc_id);
        // Ignore RIL_onUnsolicitedResponse before RIL_register
        RLOGW("RIL_onUnsolicitedResponse called before RIL_register");
        return;
    }

#ifdef MTK_RIL
    fdCommand = s_ril_param_socket[soc_id].fdCommand;

    if (fdCommand == -1
        // External SIM [Start]
        && unsolResponse != RIL_UNSOL_VSIM_OPERATION_INDICATION) {
        // External SIM [End] { //cache for rilj client
        RLOGD("Can't send URC because there is no connection yet."\
                "Try to cache request:%s in RIL%s",
                requestToString(unsolResponse), rilSocketIdToString(soc_id));

        pthread_mutex_lock(&s_pendingUrcMutex[soc_id]);
        cacheUrc(unsolResponse, data, datalen , soc_id);
        pthread_mutex_unlock(&s_pendingUrcMutex[soc_id]);
        return; //cahche then return
    }

    // External SIM [Start]
    if (fdCommand == -1 && s_fdVsim_command[soc_id] == -1 && unsolResponse == RIL_UNSOL_VSIM_OPERATION_INDICATION) {
        RLOGD("Can't send URC because there is no vsim connection yet. Try to cache request:%s in RIL%s", requestToString(unsolResponse), rilSocketIdToString(soc_id));
        pthread_mutex_lock(&s_pendingVsimUrcMutex[soc_id]);
        cacheVsimUrc(unsolResponse, data, datalen , soc_id);
        pthread_mutex_unlock(&s_pendingVsimUrcMutex[soc_id]);
        return;
    }
    // External SIM [End]
#endif

   // Grab a wake lock if needed for this reponse,
   // as we exit we'll either release it immediately
   // or set a timer to release it later.
#ifdef MTK_RIL
    /// M: ril proxy
    if (unsolResponse >= RIL_LOCAL_GSM_UNSOL_VENDOR_BASE) {
        for (i = 0; i < s_mtk_local_unsol_size; i++) {
            if (unsolResponse == s_mtk_local_unsolResponses[i].requestNumber) {
                pUnsolRi = &s_mtk_local_unsolResponses[i];
                break;
            }
        }
        if (i == s_mtk_local_unsol_size) {
            RLOGE("unsupported unsolicited response code %d", unsolResponse);
            return;
        }
    }
    ///
    else if (unsolResponse >= RIL_UNSOL_VENDOR_BASE) {
        for (i = 0; i < s_mtk_unsol_size; i++) {
            if (unsolResponse == s_mtk_unsolResponses[i].requestNumber) {
                pUnsolRi = &s_mtk_unsolResponses[i];
                break;
            }
        }
        if (i == s_mtk_unsol_size) {
            RLOGE("unsupported unsolicited response code %d", unsolResponse);
            return;
        }
    }
    else
#endif /* MTK_RIL */
    {
        for (i = 0; i < s_unsol_size; i++) {
            if (unsolResponse == s_unsolResponses[i].requestNumber) {
                pUnsolRi = &s_unsolResponses[i];
                break;
            }
        }
        if (i == s_unsol_size) {
            RLOGE("unsupported unsolicited response code %d", unsolResponse);
            return;
        }
    }
    wakeType = pUnsolRi->wakeType;
    // Grab a wake lock if needed for this reponse,
    // as we exit we'll either release it immediately
    // or set a timer to release it later.
    switch (wakeType) {
        case WAKE_PARTIAL:
            grabPartialWakeLock();
            shouldScheduleTimeout = true;
        break;

        case DONT_WAKE:
        default:
            // No wake lock is grabed so don't set timeout
            shouldScheduleTimeout = false;
            break;
    }

    appendPrintBuf("[UNSL]< %s", requestToString(unsolResponse));

    Parcel p;
    if (s_callbacks.version >= 13 && wakeType == WAKE_PARTIAL) {
        p.writeInt32 (RESPONSE_UNSOLICITED_ACK_EXP);
    } else {
        p.writeInt32 (RESPONSE_UNSOLICITED);
    }
    p.writeInt32 (unsolResponse);

    ret = pUnsolRi->responseFunction(p, const_cast<void*>(data), datalen);
    if (ret != 0) {
        // Problem with the response. Don't continue;
        goto error_exit;
    }

    // some things get more payload
    switch(unsolResponse) {
        case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED:
            newState = CALL_ONSTATEREQUEST(soc_id);
            p.writeInt32(newState);
            appendPrintBuf("%s {%s}", printBuf,
                radioStateToString(CALL_ONSTATEREQUEST(soc_id)));
            break;


        case RIL_UNSOL_NITZ_TIME_RECEIVED:
            // Store the time that this was received so the
            // handler of this message can account for
            // the time it takes to arrive and process. In
            // particular the system has been known to sleep
            // before this message can be processed.
            p.writeInt64(timeReceived);
        break;
    }

    if (s_callbacks.version < 13) {
        if (shouldScheduleTimeout) {
#ifdef MTK_RIL
            pthread_mutex_lock(&s_last_wake_mutex);
#endif
            UserCallbackInfo *p_info = internalRequestTimedCallback(wakeTimeoutCallback, NULL,
                    &TIMEVAL_WAKE_TIMEOUT);

            if (p_info == NULL) {
#ifdef MTK_RIL
                pthread_mutex_unlock(&s_last_wake_mutex);
#endif
                goto error_exit;
            } else {
                // Cancel the previous request
                if (s_last_wake_timeout_info != NULL) {
                    s_last_wake_timeout_info->userParam = (void *)1;
#ifdef MTK_RIL
                     RLOGD("s_last_wake_timeout_info: %p, cancel",s_last_wake_timeout_info);
#endif
                }
                s_last_wake_timeout_info = p_info;
            }
#ifdef MTK_RIL
            pthread_mutex_unlock(&s_last_wake_mutex);
#endif
        }
    }

#if VDBG
    RLOGI("%s UNSOLICITED: %s length:%d", rilSocketIdToString(soc_id),
            requestToString(unsolResponse), p.dataSize());
#endif

    // External SIM [Start]
    if (fdCommand == -1 && s_fdVsim_command[soc_id] > 0 && unsolResponse == RIL_UNSOL_VSIM_OPERATION_INDICATION) {
        RLOGD("sendVsimResponse:%s in RIL%s", requestToString(unsolResponse), rilSocketIdToString(soc_id));
        sendVsimResponse(p, soc_id);
        return;
    }
    // External SIM [End]

    if (fdCommand >=0) {
        ret = sendResponse(p, soc_id);
        if (ret != 0 && unsolResponse == RIL_UNSOL_NITZ_TIME_RECEIVED) {
          // Unfortunately, NITZ time is not poll/update like everything
          // else in the system. So, if the upstream client isn't connected,
          // keep a copy of the last NITZ response (with receive time noted
          // above) around so we can deliver it when it is connected

          if (s_lastNITZTimeData != NULL) {
              free(s_lastNITZTimeData);
              s_lastNITZTimeData = NULL;
          }

          s_lastNITZTimeData = calloc(datalen, 1);
          if (s_lastNITZTimeData == NULL) {
              RLOGE("Memory allocation failed in RIL_onUnsolicitedResponse");
              goto error_exit;
          }
          s_lastNITZTimeDataSize = datalen;
          memcpy(s_lastNITZTimeData, data, datalen);
        }
    }
    // Normal exit
    return;

error_exit:
    if (shouldScheduleTimeout) {
        releaseWakeLock();
    }
}

/** FIXME generalize this if you track UserCAllbackInfo, clear it
    when the callback occurs
*/
static UserCallbackInfo *
internalRequestTimedCallback (RIL_TimedCallback callback, void *param,
                                const struct timeval *relativeTime)
{
    struct timeval myRelativeTime;
    UserCallbackInfo *p_info;

    p_info = (UserCallbackInfo *) calloc(1, sizeof(UserCallbackInfo));
    if (p_info == NULL) {
        RLOGE("Memory allocation failed in internalRequestTimedCallback");
        return p_info;

    }

    p_info->p_callback = callback;
    p_info->userParam = param;
#ifdef MTK_RIL
    p_info->cid = RIL_SUPPORT_CHANNELS;
#endif

    if (relativeTime == NULL) {
        /* treat null parameter as a 0 relative time */
        memset (&myRelativeTime, 0, sizeof(myRelativeTime));
    } else {
        /* FIXME I think event_add's tv param is really const anyway */
        memcpy (&myRelativeTime, relativeTime, sizeof(myRelativeTime));
    }

    ril_event_set(&(p_info->event), -1, false, userTimerCallback, p_info);

    RLOGD("internalRequestTimedCallback malloc p_info: %p , p_info->event: %p", p_info, &(p_info->event));

    ril_timer_add(&(p_info->event), &myRelativeTime);

    triggerEvLoop();
    return p_info;
}


extern "C" void
RIL_requestTimedCallback (RIL_TimedCallback callback, void *param,
                                const struct timeval *relativeTime) {
    internalRequestTimedCallback (callback, param, relativeTime);
}

const char *
failCauseToString(RIL_Errno e) {
    switch(e) {
        case RIL_E_SUCCESS: return "E_SUCCESS";
        case RIL_E_RADIO_NOT_AVAILABLE: return "E_RADIO_NOT_AVAILABLE";
        case RIL_E_GENERIC_FAILURE: return "E_GENERIC_FAILURE";
        case RIL_E_PASSWORD_INCORRECT: return "E_PASSWORD_INCORRECT";
        case RIL_E_SIM_PIN2: return "E_SIM_PIN2";
        case RIL_E_SIM_PUK2: return "E_SIM_PUK2";
        case RIL_E_REQUEST_NOT_SUPPORTED: return "E_REQUEST_NOT_SUPPORTED";
        case RIL_E_CANCELLED: return "E_CANCELLED";
        case RIL_E_OP_NOT_ALLOWED_DURING_VOICE_CALL: return "E_OP_NOT_ALLOWED_DURING_VOICE_CALL";
        case RIL_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW: return "E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW";
        case RIL_E_SMS_SEND_FAIL_RETRY: return "E_SMS_SEND_FAIL_RETRY";
        case RIL_E_SIM_ABSENT:return "E_SIM_ABSENT";
        case RIL_E_ILLEGAL_SIM_OR_ME:return "E_ILLEGAL_SIM_OR_ME";
#ifdef FEATURE_MULTIMODE_ANDROID
        case RIL_E_SUBSCRIPTION_NOT_AVAILABLE:return "E_SUBSCRIPTION_NOT_AVAILABLE";
        case RIL_E_MODE_NOT_SUPPORTED:return "E_MODE_NOT_SUPPORTED";
#endif
        case RIL_E_FDN_CHECK_FAILURE: return "E_FDN_CHECK_FAILURE";
        case RIL_E_MISSING_RESOURCE: return "E_MISSING_RESOURCE";
        case RIL_E_NO_SUCH_ELEMENT: return "E_NO_SUCH_ELEMENT";
        case RIL_E_DIAL_MODIFIED_TO_USSD: return "E_DIAL_MODIFIED_TO_USSD";
        case RIL_E_DIAL_MODIFIED_TO_SS: return "E_DIAL_MODIFIED_TO_SS";
        case RIL_E_DIAL_MODIFIED_TO_DIAL: return "E_DIAL_MODIFIED_TO_DIAL";
        case RIL_E_USSD_MODIFIED_TO_DIAL: return "E_USSD_MODIFIED_TO_DIAL";
        case RIL_E_USSD_MODIFIED_TO_SS: return "E_USSD_MODIFIED_TO_SS";
        case RIL_E_USSD_MODIFIED_TO_USSD: return "E_USSD_MODIFIED_TO_USSD";
        case RIL_E_SS_MODIFIED_TO_DIAL: return "E_SS_MODIFIED_TO_DIAL";
        case RIL_E_SS_MODIFIED_TO_USSD: return "E_SS_MODIFIED_TO_USSD";
        case RIL_E_SUBSCRIPTION_NOT_SUPPORTED: return "E_SUBSCRIPTION_NOT_SUPPORTED";
        case RIL_E_SS_MODIFIED_TO_SS: return "E_SS_MODIFIED_TO_SS";
        case RIL_E_LCE_NOT_SUPPORTED: return "E_LCE_NOT_SUPPORTED";
        case RIL_E_NO_MEMORY: return "E_NO_MEMORY";
        case RIL_E_INTERNAL_ERR: return "E_INTERNAL_ERR";
        case RIL_E_SYSTEM_ERR: return "E_SYSTEM_ERR";
        case RIL_E_MODEM_ERR: return "E_MODEM_ERR";
        case RIL_E_INVALID_STATE: return "E_INVALID_STATE";
        case RIL_E_NO_RESOURCES: return "E_NO_RESOURCES";
        case RIL_E_SIM_ERR: return "E_SIM_ERR";
        case RIL_E_INVALID_ARGUMENTS: return "E_INVALID_ARGUMENTS";
        case RIL_E_INVALID_SIM_STATE: return "E_INVALID_SIM_STATE";
        case RIL_E_INVALID_MODEM_STATE: return "E_INVALID_MODEM_STATE";
        case RIL_E_INVALID_CALL_ID: return "E_INVALID_CALL_ID";
        case RIL_E_NO_SMS_TO_ACK: return "E_NO_SMS_TO_ACK";
        case RIL_E_NETWORK_ERR: return "E_NETWORK_ERR";
        case RIL_E_REQUEST_RATE_LIMITED: return "E_REQUEST_RATE_LIMITED";
        case RIL_E_SIM_BUSY: return "E_SIM_BUSY";
        case RIL_E_SIM_FULL: return "E_SIM_FULL";
        case RIL_E_NETWORK_REJECT: return "E_NETWORK_REJECT";
        case RIL_E_OPERATION_NOT_ALLOWED: return "E_OPERATION_NOT_ALLOWED";
        case RIL_E_EMPTY_RECORD: return "E_EMPTY_RECORD";
        case RIL_E_INVALID_SMS_FORMAT: return "E_INVALID_SMS_FORMAT";
        case RIL_E_ENCODING_ERR: return "E_ENCODING_ERR";
        case RIL_E_INVALID_SMSC_ADDRESS: return "E_INVALID_SMSC_ADDRESS";
        case RIL_E_NO_SUCH_ENTRY: return "E_NO_SUCH_ENTRY";
        case RIL_E_NETWORK_NOT_READY: return "E_NETWORK_NOT_READY";
        case RIL_E_NOT_PROVISIONED: return "E_NOT_PROVISIONED";
        case RIL_E_NO_SUBSCRIPTION: return "E_NO_SUBSCRIPTION";
        case RIL_E_NO_NETWORK_FOUND: return "E_NO_NETWORK_FOUND";
        case RIL_E_DEVICE_IN_USE: return "E_DEVICE_IN_USE";
        case RIL_E_ABORTED: return "E_ABORTED";
        case RIL_E_INVALID_RESPONSE: return "INVALID_RESPONSE";
        case RIL_E_OEM_ERROR_1: return "E_OEM_ERROR_1";
        case RIL_E_OEM_ERROR_2: return "E_OEM_ERROR_2";
        case RIL_E_OEM_ERROR_3: return "E_OEM_ERROR_3";
        case RIL_E_OEM_ERROR_4: return "E_OEM_ERROR_4";
        case RIL_E_OEM_ERROR_5: return "E_OEM_ERROR_5";
        case RIL_E_OEM_ERROR_6: return "E_OEM_ERROR_6";
        case RIL_E_OEM_ERROR_7: return "E_OEM_ERROR_7";
        case RIL_E_OEM_ERROR_8: return "E_OEM_ERROR_8";
        case RIL_E_OEM_ERROR_9: return "E_OEM_ERROR_9";
        case RIL_E_OEM_ERROR_10: return "E_OEM_ERROR_10";
        case RIL_E_OEM_ERROR_11: return "E_OEM_ERROR_11";
        case RIL_E_OEM_ERROR_12: return "E_OEM_ERROR_12";
        case RIL_E_OEM_ERROR_13: return "E_OEM_ERROR_13";
        case RIL_E_OEM_ERROR_14: return "E_OEM_ERROR_14";
        case RIL_E_OEM_ERROR_15: return "E_OEM_ERROR_15";
        case RIL_E_OEM_ERROR_16: return "E_OEM_ERROR_16";
        case RIL_E_OEM_ERROR_17: return "E_OEM_ERROR_17";
        case RIL_E_OEM_ERROR_18: return "E_OEM_ERROR_18";
        case RIL_E_OEM_ERROR_19: return "E_OEM_ERROR_19";
        case RIL_E_OEM_ERROR_20: return "E_OEM_ERROR_20";
        case RIL_E_OEM_ERROR_21: return "E_OEM_ERROR_21";
        case RIL_E_OEM_ERROR_22: return "E_OEM_ERROR_22";
        case RIL_E_OEM_ERROR_23: return "E_OEM_ERROR_23";
        case RIL_E_OEM_ERROR_24: return "E_OEM_ERROR_24";
        case RIL_E_OEM_ERROR_25: return "E_OEM_ERROR_25";
        default: return "<unknown error>";
    }
}

const char *
radioStateToString(RIL_RadioState s) {
    switch(s) {
        case RADIO_STATE_OFF: return "RADIO_OFF";
        case RADIO_STATE_UNAVAILABLE: return "RADIO_UNAVAILABLE";
        case RADIO_STATE_ON:return"RADIO_ON";
        default: return "<unknown state>";
    }
}

const char *
callStateToString(RIL_CallState s) {
    switch(s) {
        case RIL_CALL_ACTIVE : return "ACTIVE";
        case RIL_CALL_HOLDING: return "HOLDING";
        case RIL_CALL_DIALING: return "DIALING";
        case RIL_CALL_ALERTING: return "ALERTING";
        case RIL_CALL_INCOMING: return "INCOMING";
        case RIL_CALL_WAITING: return "WAITING";
        default: return "<unknown state>";
    }
}

const char *
requestToString(int request) {
/*
 cat libs/telephony/ril_commands.h \
 | egrep "^ *{RIL_" \
 | sed -re 's/\{RIL_([^,]+),[^,]+,([^}]+).+/case RIL_\1: return "\1";/'


 cat libs/telephony/ril_unsol_commands.h \
 | egrep "^ *{RIL_" \
 | sed -re 's/\{RIL_([^,]+),([^}]+).+/case RIL_\1: return "\1";/'

*/
    switch(request) {
        case RIL_REQUEST_GET_SIM_STATUS: return "GET_SIM_STATUS";
        case RIL_REQUEST_ENTER_SIM_PIN: return "ENTER_SIM_PIN";
        case RIL_REQUEST_ENTER_SIM_PUK: return "ENTER_SIM_PUK";
        case RIL_REQUEST_ENTER_SIM_PIN2: return "ENTER_SIM_PIN2";
        case RIL_REQUEST_ENTER_SIM_PUK2: return "ENTER_SIM_PUK2";
        case RIL_REQUEST_CHANGE_SIM_PIN: return "CHANGE_SIM_PIN";
        case RIL_REQUEST_CHANGE_SIM_PIN2: return "CHANGE_SIM_PIN2";
        case RIL_REQUEST_ENTER_NETWORK_DEPERSONALIZATION: return "ENTER_NETWORK_DEPERSONALIZATION";
        case RIL_REQUEST_GET_CURRENT_CALLS: return "GET_CURRENT_CALLS";
        case RIL_REQUEST_DIAL: return "DIAL";
        case RIL_REQUEST_EMERGENCY_DIAL: return "EMERGENCY_DIAL";
        case RIL_LOCAL_REQUEST_EMERGENCY_DIAL: return "LOCAL_EMERGENCY_DIAL";
        case RIL_LOCAL_REQUEST_IMS_EMERGENCY_DIAL: return "LOCAL_IMS_EMERGENCY_DIAL";
        case RIL_LOCAL_REQUEST_SET_ECC_SERVICE_CATEGORY: return "LOCAL_SET_ECC_SERVICE_CATEGORY";
        case RIL_REQUEST_GET_IMSI: return "GET_IMSI";
        case RIL_REQUEST_HANGUP: return "HANGUP";
        case RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND: return "HANGUP_WAITING_OR_BACKGROUND";
        case RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND: return "HANGUP_FOREGROUND_RESUME_BACKGROUND";
        case RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE: return "SWITCH_WAITING_OR_HOLDING_AND_ACTIVE";
        case RIL_REQUEST_CONFERENCE: return "CONFERENCE";
        case RIL_REQUEST_UDUB: return "UDUB";
        case RIL_REQUEST_LAST_CALL_FAIL_CAUSE: return "LAST_CALL_FAIL_CAUSE";
        case RIL_REQUEST_SIGNAL_STRENGTH: return "SIGNAL_STRENGTH";
        case RIL_REQUEST_VOICE_REGISTRATION_STATE: return "VOICE_REGISTRATION_STATE";
        case RIL_REQUEST_DATA_REGISTRATION_STATE: return "DATA_REGISTRATION_STATE";
        case RIL_REQUEST_OPERATOR: return "OPERATOR";
        case RIL_REQUEST_RADIO_POWER: return "RADIO_POWER";
        case RIL_REQUEST_MODEM_POWEROFF: return "RIL_REQUEST_MODEM_POWEROFF";
        case RIL_REQUEST_MODEM_POWERON: return "RIL_REQUEST_MODEM_POWERON";
        case RIL_REQUEST_DTMF: return "DTMF";
        case RIL_REQUEST_SEND_SMS: return "SEND_SMS";
        case RIL_REQUEST_SEND_SMS_EXPECT_MORE: return "SEND_SMS_EXPECT_MORE";
        case RIL_REQUEST_SETUP_DATA_CALL: return "SETUP_DATA_CALL";
        case RIL_REQUEST_SIM_IO: return "SIM_IO";
        case RIL_REQUEST_SEND_USSD: return "SEND_USSD";
        case RIL_REQUEST_CANCEL_USSD: return "CANCEL_USSD";
        case RIL_REQUEST_GET_CLIR: return "GET_CLIR";
        case RIL_REQUEST_SET_CLIR: return "SET_CLIR";
        case RIL_REQUEST_QUERY_CALL_FORWARD_STATUS: return "QUERY_CALL_FORWARD_STATUS";
        case RIL_REQUEST_SET_CALL_FORWARD: return "SET_CALL_FORWARD";
        case RIL_REQUEST_QUERY_CALL_WAITING: return "QUERY_CALL_WAITING";
        case RIL_REQUEST_SET_CALL_WAITING: return "SET_CALL_WAITING";
        case RIL_REQUEST_SMS_ACKNOWLEDGE: return "SMS_ACKNOWLEDGE";
        case RIL_REQUEST_GET_IMEI: return "GET_IMEI";
        case RIL_REQUEST_GET_IMEISV: return "GET_IMEISV";
        case RIL_REQUEST_ANSWER: return "ANSWER";
        case RIL_REQUEST_DEACTIVATE_DATA_CALL: return "DEACTIVATE_DATA_CALL";
        case RIL_REQUEST_QUERY_FACILITY_LOCK: return "QUERY_FACILITY_LOCK";
        case RIL_REQUEST_SET_FACILITY_LOCK: return "SET_FACILITY_LOCK";
        case RIL_REQUEST_CHANGE_BARRING_PASSWORD: return "CHANGE_BARRING_PASSWORD";
        case RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE: return "QUERY_NETWORK_SELECTION_MODE";
        case RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC: return "SET_NETWORK_SELECTION_AUTOMATIC";
        case RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL: return "SET_NETWORK_SELECTION_MANUAL";
        case RIL_REQUEST_QUERY_AVAILABLE_NETWORKS: return "QUERY_AVAILABLE_NETWORKS";
        case RIL_REQUEST_DTMF_START: return "DTMF_START";
        case RIL_REQUEST_DTMF_STOP: return "DTMF_STOP";
        case RIL_REQUEST_BASEBAND_VERSION: return "BASEBAND_VERSION";
        case RIL_REQUEST_SEPARATE_CONNECTION: return "SEPARATE_CONNECTION";
        case RIL_REQUEST_SET_MUTE: return "SET_MUTE";
        case RIL_REQUEST_GET_MUTE: return "GET_MUTE";
        case RIL_REQUEST_QUERY_CLIP: return "QUERY_CLIP";
        case RIL_REQUEST_SET_SS_PROPERTY: return "SET_SS_PROPERTY";
        case RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE: return "LAST_DATA_CALL_FAIL_CAUSE";
        case RIL_REQUEST_DATA_CALL_LIST: return "DATA_CALL_LIST";
        case RIL_REQUEST_RESET_RADIO: return "RESET_RADIO";
        case RIL_REQUEST_OEM_HOOK_RAW: return "OEM_HOOK_RAW";
        case RIL_REQUEST_OEM_HOOK_STRINGS: return "OEM_HOOK_STRINGS";
        case RIL_REQUEST_SCREEN_STATE: return "SCREEN_STATE";
        case RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION: return "SET_SUPP_SVC_NOTIFICATION";
        case RIL_REQUEST_WRITE_SMS_TO_SIM: return "WRITE_SMS_TO_SIM";
        case RIL_REQUEST_DELETE_SMS_ON_SIM: return "DELETE_SMS_ON_SIM";
        case RIL_REQUEST_SET_BAND_MODE: return "SET_BAND_MODE";
        case RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE: return "QUERY_AVAILABLE_BAND_MODE";
        case RIL_REQUEST_STK_GET_PROFILE: return "STK_GET_PROFILE";
        case RIL_REQUEST_STK_SET_PROFILE: return "STK_SET_PROFILE";
        case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND: return "STK_SEND_ENVELOPE_COMMAND";
        case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE: return "STK_SEND_TERMINAL_RESPONSE";
        case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM: return "STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM";
        case RIL_REQUEST_EXPLICIT_CALL_TRANSFER: return "EXPLICIT_CALL_TRANSFER";
        case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE: return "SET_PREFERRED_NETWORK_TYPE";
        case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE: return "GET_PREFERRED_NETWORK_TYPE";
        case RIL_REQUEST_GET_NEIGHBORING_CELL_IDS: return "GET_NEIGHBORING_CELL_IDS";
        case RIL_REQUEST_SET_LOCATION_UPDATES: return "SET_LOCATION_UPDATES";
        case RIL_REQUEST_CDMA_SET_SUBSCRIPTION_SOURCE: return "CDMA_SET_SUBSCRIPTION_SOURCE";
        case RIL_REQUEST_CDMA_SET_ROAMING_PREFERENCE: return "CDMA_SET_ROAMING_PREFERENCE";
        case RIL_REQUEST_CDMA_QUERY_ROAMING_PREFERENCE: return "CDMA_QUERY_ROAMING_PREFERENCE";
        case RIL_REQUEST_SET_TTY_MODE: return "SET_TTY_MODE";
        case RIL_REQUEST_QUERY_TTY_MODE: return "QUERY_TTY_MODE";
        case RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE: return "CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE";
        case RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE: return "CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE";
        case RIL_REQUEST_CDMA_FLASH: return "CDMA_FLASH";
        case RIL_REQUEST_CDMA_BURST_DTMF: return "CDMA_BURST_DTMF";
        case RIL_REQUEST_CDMA_VALIDATE_AND_WRITE_AKEY: return "CDMA_VALIDATE_AND_WRITE_AKEY";
        case RIL_REQUEST_CDMA_SEND_SMS: return "CDMA_SEND_SMS";
        case RIL_REQUEST_CDMA_SMS_ACKNOWLEDGE: return "CDMA_SMS_ACKNOWLEDGE";
        case RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG: return "GSM_GET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG: return "GSM_SET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION: return "GSM_SMS_BROADCAST_ACTIVATION";
        case RIL_REQUEST_CDMA_GET_BROADCAST_SMS_CONFIG: return "CDMA_GET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_CDMA_SET_BROADCAST_SMS_CONFIG: return "CDMA_SET_BROADCAST_SMS_CONFIG";
        case RIL_REQUEST_CDMA_SMS_BROADCAST_ACTIVATION: return "CDMA_SMS_BROADCAST_ACTIVATION";
        case RIL_REQUEST_CDMA_SUBSCRIPTION: return "CDMA_SUBSCRIPTION";
        case RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM: return "CDMA_WRITE_SMS_TO_RUIM";
        case RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM: return "CDMA_DELETE_SMS_ON_RUIM";
        case RIL_REQUEST_DEVICE_IDENTITY: return "DEVICE_IDENTITY";
        case RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE: return "EXIT_EMERGENCY_CALLBACK_MODE";
        case RIL_REQUEST_GET_SMSC_ADDRESS: return "GET_SMSC_ADDRESS";
        case RIL_REQUEST_SET_SMSC_ADDRESS: return "SET_SMSC_ADDRESS";
        case RIL_REQUEST_REPORT_SMS_MEMORY_STATUS: return "REPORT_SMS_MEMORY_STATUS";
        case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING: return "REPORT_STK_SERVICE_IS_RUNNING";
        case RIL_REQUEST_CDMA_GET_SUBSCRIPTION_SOURCE: return "CDMA_GET_SUBSCRIPTION_SOURCE";
        case RIL_REQUEST_ISIM_AUTHENTICATION: return "ISIM_AUTHENTICATION";
        case RIL_REQUEST_ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU: return "ACKNOWLEDGE_INCOMING_GSM_SMS_WITH_PDU";
        case RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS: return "STK_SEND_ENVELOPE_WITH_STATUS";
        case RIL_REQUEST_VOICE_RADIO_TECH: return "VOICE_RADIO_TECH";
        case RIL_REQUEST_GET_CELL_INFO_LIST: return "GET_CELL_INFO_LIST";
        case RIL_REQUEST_SET_UNSOL_CELL_INFO_LIST_RATE: return "SET_UNSOL_CELL_INFO_LIST_RATE";
        case RIL_REQUEST_SET_INITIAL_ATTACH_APN: return "SET_INITIAL_ATTACH_APN";
        case RIL_REQUEST_IMS_REGISTRATION_STATE: return "IMS_REGISTRATION_STATE";
        case RIL_REQUEST_IMS_SEND_SMS: return "IMS_SEND_SMS";
        case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC: return "SIM_TRANSMIT_APDU_BASIC";
        case RIL_REQUEST_SIM_OPEN_CHANNEL: return "SIM_OPEN_CHANNEL";
        case RIL_REQUEST_SIM_CLOSE_CHANNEL: return "SIM_CLOSE_CHANNEL";
        case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL: return "SIM_TRANSMIT_APDU_CHANNEL";
        case RIL_REQUEST_NV_READ_ITEM: return "NV_READ_ITEM";
        case RIL_REQUEST_NV_WRITE_ITEM: return "NV_WRITE_ITEM";
        case RIL_REQUEST_NV_WRITE_CDMA_PRL: return "NV_WRITE_CDMA_PRL";
        case RIL_REQUEST_NV_RESET_CONFIG: return "NV_RESET_CONFIG";
        case RIL_REQUEST_SET_UICC_SUBSCRIPTION: return "SET_UICC_SUBSCRIPTION";
        case RIL_REQUEST_ALLOW_DATA: return "ALLOW_DATA";
        case RIL_REQUEST_GET_HARDWARE_CONFIG: return "GET_HARDWARE_CONFIG";
        case RIL_REQUEST_SIM_AUTHENTICATION: return "SIM_AUTHENTICATION";
        case RIL_REQUEST_GET_DC_RT_INFO: return "GET_DC_RT_INFO";
        case RIL_REQUEST_SET_DC_RT_INFO_RATE: return "SET_DC_RT_INFO_RATE";
        case RIL_REQUEST_SET_DATA_PROFILE: return "SET_DATA_PROFILE";
        case RIL_REQUEST_SHUTDOWN: return "SHUTDOWN";
        case RIL_REQUEST_GET_RADIO_CAPABILITY: return "GET_RADIO_CAPABILITY";
        case RIL_REQUEST_SET_RADIO_CAPABILITY: return "SET_RADIO_CAPABILITY";
        case RIL_REQUEST_START_LCE: return "START_LCE";
        case RIL_REQUEST_STOP_LCE: return "STOP_LCE";
        case RIL_REQUEST_PULL_LCEDATA: return "PULL_LCEDATA";
        case RIL_REQUEST_GET_ACTIVITY_INFO: return "GET_ACTIVITY_INFO";
        case RIL_REQUEST_SET_ALLOWED_CARRIERS: return "SET_ALLOWED_CARRIERS";
        case RIL_REQUEST_GET_ALLOWED_CARRIERS: return "GET_ALLOWED_CARRIERS";
        case RIL_REQUEST_SEND_DEVICE_STATE: return "SEND_DEVICE_STATE";
        case RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER: return "SET_UNSOLICITED_RESPONSE_FILTER";
        case RIL_REQUEST_SET_SIM_CARD_POWER: return "SET_SIM_CARD_POWER";
        case RIL_REQUEST_START_NETWORK_SCAN: return "START_NETWORK_SCAN";
        case RIL_REQUEST_STOP_NETWORK_SCAN: return "STOP_NETWORK_SCAN";
        case RIL_REQUEST_UPDATE_SUBLOCK_SETTINGS: return "UPDATE_SUBLOCK_SETTINGS";
        case RIL_REQUEST_GET_SUBLOCK_MODEM_STATUS: return "GET_SUBLOCK_MODEM_STATUS";
        case RIL_REQUEST_SET_LINK_CAPACITY_REPORTING_CRITERIA: return "SET_LINK_CAPACITY_REPORTING_CRITERIA";
        case RIL_REQUEST_SET_PREFERRED_DATA_MODEM: return "RIL_REQUEST_SET_PREFERRED_DATA_MODEM";
        case RIL_RESPONSE_ACKNOWLEDGEMENT: return "RESPONSE_ACKNOWLEDGEMENT";
        case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED: return "UNSOL_RESPONSE_RADIO_STATE_CHANGED";
        case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED: return "UNSOL_RESPONSE_CALL_STATE_CHANGED";
        case RIL_UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED: return "UNSOL_RESPONSE_VOICE_NETWORK_STATE_CHANGED";
        case RIL_UNSOL_RESPONSE_NEW_SMS: return "UNSOL_RESPONSE_NEW_SMS";
        case RIL_UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT: return "UNSOL_RESPONSE_NEW_SMS_STATUS_REPORT";
        case RIL_UNSOL_RESPONSE_NEW_SMS_ON_SIM: return "UNSOL_RESPONSE_NEW_SMS_ON_SIM";
        case RIL_UNSOL_ON_USSD: return "UNSOL_ON_USSD";
        case RIL_UNSOL_ON_USSD_REQUEST: return "UNSOL_ON_USSD_REQUEST";
        case RIL_UNSOL_NITZ_TIME_RECEIVED: return "UNSOL_NITZ_TIME_RECEIVED";
        case RIL_UNSOL_SIGNAL_STRENGTH: return "UNSOL_SIGNAL_STRENGTH";
        case RIL_UNSOL_DATA_CALL_LIST_CHANGED: return "UNSOL_DATA_CALL_LIST_CHANGED";
        case RIL_UNSOL_SUPP_SVC_NOTIFICATION: return "UNSOL_SUPP_SVC_NOTIFICATION";
        case RIL_UNSOL_SUPP_SVC_NOTIFICATION_EX: return "UNSOL_SUPP_SVC_NOTIFICATION_EX";
        case RIL_UNSOL_STK_SESSION_END: return "UNSOL_STK_SESSION_END";
        case RIL_UNSOL_STK_PROACTIVE_COMMAND: return "UNSOL_STK_PROACTIVE_COMMAND";
        case RIL_UNSOL_STK_EVENT_NOTIFY: return "UNSOL_STK_EVENT_NOTIFY";
        case RIL_UNSOL_STK_CALL_SETUP: return "UNSOL_STK_CALL_SETUP";
        case RIL_UNSOL_SIM_SMS_STORAGE_FULL: return "UNSOL_SIM_SMS_STORAGE_FULL";
        case RIL_UNSOL_SIM_REFRESH: return "UNSOL_SIM_REFRESH";
        case RIL_UNSOL_CALL_RING: return "UNSOL_CALL_RING";
        case RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED: return "UNSOL_RESPONSE_SIM_STATUS_CHANGED";
        case RIL_UNSOL_RESPONSE_CDMA_NEW_SMS: return "UNSOL_RESPONSE_CDMA_NEW_SMS";
        case RIL_UNSOL_RESPONSE_NEW_BROADCAST_SMS: return "UNSOL_RESPONSE_NEW_BROADCAST_SMS";
        case RIL_UNSOL_CDMA_RUIM_SMS_STORAGE_FULL: return "UNSOL_CDMA_RUIM_SMS_STORAGE_FULL";
        case RIL_UNSOL_RESTRICTED_STATE_CHANGED: return "UNSOL_RESTRICTED_STATE_CHANGED";
        case RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE: return "UNSOL_ENTER_EMERGENCY_CALLBACK_MODE";
        case RIL_UNSOL_CDMA_CALL_WAITING: return "UNSOL_CDMA_CALL_WAITING";
        case RIL_UNSOL_CDMA_OTA_PROVISION_STATUS: return "UNSOL_CDMA_OTA_PROVISION_STATUS";
        case RIL_UNSOL_CDMA_INFO_REC: return "UNSOL_CDMA_INFO_REC";
        case RIL_UNSOL_OEM_HOOK_RAW: return "UNSOL_OEM_HOOK_RAW";
        case RIL_UNSOL_RINGBACK_TONE: return "UNSOL_RINGBACK_TONE";
        case RIL_UNSOL_RESEND_INCALL_MUTE: return "UNSOL_RESEND_INCALL_MUTE";
        case RIL_UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED: return "UNSOL_CDMA_SUBSCRIPTION_SOURCE_CHANGED";
        case RIL_UNSOL_CDMA_PRL_CHANGED: return "UNSOL_CDMA_PRL_CHANGED";
        case RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE: return "UNSOL_EXIT_EMERGENCY_CALLBACK_MODE";
        case RIL_UNSOL_RIL_CONNECTED: return "UNSOL_RIL_CONNECTED";
        case RIL_UNSOL_VOICE_RADIO_TECH_CHANGED: return "UNSOL_VOICE_RADIO_TECH_CHANGED";
        case RIL_UNSOL_CELL_INFO_LIST: return "UNSOL_CELL_INFO_LIST";
        case RIL_UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED: return "UNSOL_RESPONSE_IMS_NETWORK_STATE_CHANGED";
        case RIL_UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED: return "UNSOL_UICC_SUBSCRIPTION_STATUS_CHANGED";
        case RIL_UNSOL_SRVCC_STATE_NOTIFY: return "UNSOL_SRVCC_STATE_NOTIFY";
        case RIL_UNSOL_HARDWARE_CONFIG_CHANGED: return "UNSOL_HARDWARE_CONFIG_CHANGED";
        case RIL_UNSOL_DC_RT_INFO_CHANGED: return "UNSOL_DC_RT_INFO_CHANGED";
        case RIL_UNSOL_RADIO_CAPABILITY: return "UNSOL_RADIO_CAPABILITY";
        case RIL_UNSOL_ON_SS: return "UNSOL_ON_SS";
        case RIL_UNSOL_STK_CC_ALPHA_NOTIFY: return "UNSOL_STK_CC_ALPHA_NOTIFY";
        case RIL_UNSOL_LCEDATA_RECV: return "UNSOL_LCEDATA_RECV";
        case RIL_UNSOL_PCO_DATA: return "UNSOL_PCO_DATA";
        case RIL_UNSOL_MODEM_RESTART: return "UNSOL_MODEM_RESTART";
        case RIL_UNSOL_NETWORK_SCAN_RESULT: return "UNSOL_NETWORK_SCAN_RESULT";
        case RIL_REQUEST_ENABLE_MODEM: return "RIL_REQUEST_ENABLE_MODEM";
        case RIL_REQUEST_GET_PHONE_CAPABILITY: return "RIL_REQUEST_GET_PHONE_CAPABILITY";
        default: return mtkRequestToString(request);
    }
}

const char *
rilSocketIdToString(RIL_SOCKET_ID socket_id)
{
    switch(socket_id) {
        case RIL_SOCKET_1:
            return "RIL_SOCKET_1";
        case RIL_SOCKET_2:
            return "RIL_SOCKET_2";
        case RIL_SOCKET_3:
            return "RIL_SOCKET_3";
        case RIL_SOCKET_4:
            return "RIL_SOCKET_4";
        default:
            return "not a valid RIL";
    }
}

int getRilProxysNum()
{
    //return RIL_SUPPORT_PROXYS;
    int supportProxy = 0;
    switch(SIM_COUNT) {
        case 2:
            supportProxy = RIL_PROXY_SET3_OFFSET;
            break;
        case 3:
            supportProxy = RIL_PROXY_SET4_OFFSET;
            break;
        case 4:
            supportProxy = RIL_SUPPORT_PROXYS;
            break;
        case 1:
        default:
            supportProxy = RIL_PROXY_OFFSET;
            break;
    }
    return supportProxy;
}

/*
 * Returns true for a debuggable build.
 */
static bool isDebuggable() {
    char debuggable[PROP_VALUE_MAX];
    property_get("ro.debuggable", debuggable, "0");
    if (strcmp(debuggable, "1") == 0) {
        return true;
    }
    return false;
}

#ifdef HAVE_AEE_FEATURE
void triggerWarning(char *pErrMsg) {
    if (pErrMsg != NULL) {
        aee_system_warning("c2krild", NULL, DB_OPT_DEFAULT, pErrMsg);
        exit(0);
    } else {
        assert(0);
    }
}
#endif

} /* namespace android */

void rilEventAddWakeup_helper(struct ril_event *ev) {
    android::rilEventAddWakeup(ev);
}

void listenCallback_helper(int fd, short flags, void *param) {
    android::listenCallback(fd, flags, param);
}

int blockingWrite_helper(int fd, void *buffer, size_t len) {
    return android::blockingWrite(fd, buffer, len);
}
