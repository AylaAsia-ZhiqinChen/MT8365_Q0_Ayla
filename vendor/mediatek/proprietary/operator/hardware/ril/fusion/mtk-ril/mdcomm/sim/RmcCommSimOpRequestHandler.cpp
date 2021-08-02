/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2016. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
 * Include
 *****************************************************************************/
#include "RmcCommSimRequestHandler.h"
#include "RmcCommSimOpRequestHandler.h"
#include "RmcOpRsuSimDefs.h"
#include "RmcSimBaseHandler.h"
#include <telephony/mtk_ril.h>
#include <telephony/mtk_rilop.h>
#include "RfxMessageId.h"
#include "RfxStringData.h"
#include "RfxVoidData.h"

using ::android::String8;
using namespace std;

static const int ch1OpReqList[] = {
    RFX_MSG_REQUEST_ATT_GET_SHARED_KEY,
    RFX_MSG_REQUEST_ATT_UPDATE_SIM_LOCK_SETTINGS,
    RFX_MSG_REQUEST_ATT_GET_SIM_LOCK_VERSION,
    RFX_MSG_REQUEST_ATT_RESET_SIM_LOCK_SETTINGS,
    RFX_MSG_REQUEST_ATT_GET_MODEM_STATUS,
    RFX_MSG_REQUEST_SEND_RSU_REQUEST,
};

RFX_REGISTER_DATA_TO_REQUEST_ID(RfxVoidData, RfxStringData,
        RFX_MSG_REQUEST_ATT_GET_SHARED_KEY);
RFX_REGISTER_DATA_TO_REQUEST_ID(RfxStringData, RfxVoidData,
        RFX_MSG_REQUEST_ATT_UPDATE_SIM_LOCK_SETTINGS);
RFX_REGISTER_DATA_TO_REQUEST_ID(RfxVoidData, RfxStringData,
        RFX_MSG_REQUEST_ATT_GET_SIM_LOCK_VERSION);
RFX_REGISTER_DATA_TO_REQUEST_ID(RfxVoidData, RfxVoidData,
        RFX_MSG_REQUEST_ATT_RESET_SIM_LOCK_SETTINGS);
RFX_REGISTER_DATA_TO_REQUEST_ID(RfxVoidData, RfxStringData,
        RFX_MSG_REQUEST_ATT_GET_MODEM_STATUS);
RFX_REGISTER_DATA_TO_REQUEST_ID(RfxOpRsuRequestData, RfxOpRsuResponseData,
        RFX_MSG_REQUEST_SEND_RSU_REQUEST);

/*****************************************************************************
 * Class RmcCommSimOpRequestHandler
 *****************************************************************************/
RmcCommSimOpRequestHandler::RmcCommSimOpRequestHandler(int slot_id, int channel_id) :
        RmcCommSimRequestHandler(slot_id, channel_id) {
    setTag(String8("RmcCommSimOpReq"));
}

RmcCommSimOpRequestHandler::~RmcCommSimOpRequestHandler() {
    pthread_mutex_unlock(&simStatusMutex);
    pthread_mutex_destroy(&simStatusMutex);
}

const int* RmcCommSimOpRequestHandler::queryTable(int channel_id, int *record_num) {
    const int* superTable = RmcCommSimRequestHandler::queryTable(channel_id, record_num);
    int subRecordNumber = 0;
    int* bufTable = NULL;
    int index = 0;
    int supRecordNumber = *record_num;

    if (channel_id == RIL_CMD_PROXY_1) {
        subRecordNumber = sizeof(ch1OpReqList)/sizeof(int);
        bufTable = (int*)calloc(1, sizeof(int)* (subRecordNumber + supRecordNumber));
        RFX_ASSERT(bufTable != NULL);
        if (NULL != superTable) {
            memcpy(bufTable, superTable, sizeof(int) * supRecordNumber);
            index += supRecordNumber;
        } else {
            logE(mTag, "Null superTable", channel_id);
        }
        memcpy(bufTable + index, ch1OpReqList, sizeof(int) * subRecordNumber);
        *record_num += subRecordNumber;
    } else {
        bufTable = (int*)calloc(1, sizeof(int)* supRecordNumber);
        RFX_ASSERT(bufTable != NULL);
        if (NULL != superTable) {
            memcpy(bufTable, superTable, sizeof(int) * supRecordNumber);
        } else {
            logE(mTag, "Null superTable", channel_id);
        }
    }
    return bufTable;
}

RmcSimBaseHandler::SIM_HANDLE_RESULT RmcCommSimOpRequestHandler::needHandle(
        const sp<RfxMclMessage>& msg) {
    int request = msg->getId();
    RmcSimBaseHandler::SIM_HANDLE_RESULT result = RmcSimBaseHandler::RESULT_IGNORE;

    switch(request) {
        case RFX_MSG_REQUEST_ATT_GET_SHARED_KEY:
        case RFX_MSG_REQUEST_ATT_UPDATE_SIM_LOCK_SETTINGS:
        case RFX_MSG_REQUEST_ATT_GET_SIM_LOCK_VERSION:
        case RFX_MSG_REQUEST_ATT_RESET_SIM_LOCK_SETTINGS:
        case RFX_MSG_REQUEST_ATT_GET_MODEM_STATUS:
        case RFX_MSG_REQUEST_SEND_RSU_REQUEST:
            result = RmcSimBaseHandler::RESULT_NEED;
            break;
        default:
            result = RmcCommSimRequestHandler::needHandle(msg);
            break;
    }

    return result;
}

void RmcCommSimOpRequestHandler::handleRequest(const sp<RfxMclMessage>& msg) {
    int request = msg->getId();
    switch(request) {
        case RFX_MSG_REQUEST_ATT_GET_SHARED_KEY:
            handleAttGetSharedKey(msg);
            break;
        case RFX_MSG_REQUEST_ATT_UPDATE_SIM_LOCK_SETTINGS:
            handleAttUpdateSimLockSettings(msg);
            break;
        case RFX_MSG_REQUEST_ATT_GET_SIM_LOCK_VERSION:
            handleAttGetSimLockVersion(msg);
            break;
        case RFX_MSG_REQUEST_ATT_RESET_SIM_LOCK_SETTINGS:
            handleAttResetSimLockSettings(msg);
            break;
        case RFX_MSG_REQUEST_ATT_GET_MODEM_STATUS:
            handleAttGetModemStatus(msg);
            break;
        case RFX_MSG_REQUEST_SEND_RSU_REQUEST:
            handleRsuRequest(msg);
            break;
        default:
            RmcCommSimRequestHandler::handleRequest(msg);
            break;
    }
}

void RmcCommSimOpRequestHandler::handleAttGetSharedKey(const sp<RfxMclMessage>& msg) {
    String8 cmd("");
    RfxAtLine *line = NULL;
    int err = -1;
    sp<RfxAtResponse> p_response = NULL;
    sp<RfxMclMessage> response;
    RIL_Errno ret = RIL_E_GENERIC_FAILURE;
    int key_len;
    char *key = NULL;

    logD(mTag, "[RSU-SIMLOCK] handleAttGetSharedKey.\n");

    cmd.append(String8::format("AT+EATTSK=0"));
    p_response = atSendCommandSingleline(cmd, "+EATTSK:");
    cmd.clear();
    err = p_response->getError();
    if (err < 0 || p_response->getSuccess() == 0) {
        goto done;
    }
    line = p_response->getIntermediates();
    line->atTokStart(&err);
    if (err < 0) {
        goto done;
    }

    // Get shared key len
    key_len = line->atTokNextint(&err);
    if (err < 0) {
        goto done;
    }

    // Get shared key
    key = line->atTokNextstr(&err);
    if (err < 0) {
        goto done;
    }
    ret = RIL_E_SUCCESS;
done:
    logD(mTag, "[RSU-SIMLOCK] done.\n");
    if (key != NULL) {
        logD(mTag, "[RSU-SIMLOCK] key %s.\n", key);
        response = RfxMclMessage::obtainResponse(msg->getId(), ret,
                RfxStringData((void*)key, strlen(key)), msg, false);
    } else {
        response = RfxMclMessage::obtainResponse(msg->getId(), ret, RfxStringData(), msg, false);
    }
    responseToTelCore(response);
}

void RmcCommSimOpRequestHandler::handleAttUpdateSimLockSettings(const sp<RfxMclMessage>& msg) {
    String8 pString((char*)(msg->getData()->getData()));
    String8 cmd("");
    RfxAtLine *line = NULL;
    int err = -1;
    sp<RfxAtResponse> p_response = NULL;
    sp<RfxMclMessage> response;
    RIL_Errno ret = RIL_E_GENERIC_FAILURE;

    logD(mTag, "[RSU-SIMLOCK] handleAttUpdateSimLockSettings strings %s\n", pString.string());

    cmd.append(String8::format("AT+EATTSLB=1,%d,\"%s\"",
            (unsigned int)((pString.size() + 1)/2), pString.string()));

    p_response = atSendCommand(cmd.string());
    cmd.clear();
    err = p_response->getError();
    if (err < 0) {
        goto done;
    }

    if (p_response->getSuccess() == 0) {
        switch (p_response->atGetCmeError()) {
            logD(mTag, "[RSU-SIMLOCK] p_response %d\n", p_response->atGetCmeError());
            case CME_VERIFICATION_FAILED:
                ret = RIL_E_VERIFICATION_FAILED;
                logD(mTag, "[RSU-SIMLOCK] p_response CME_SIM_LOCK_BLOB_VERIFICATION_FAILED\n");
                break;
            case CME_REBOOT_REQUEST:
                ret = RIL_E_REBOOT_REQUEST;
                logD(mTag, "[RSU-SIMLOCK] p_response CME_REBOOT_REQUEST\n");
                break;
            default:
                goto done;
        }
    } else {
        ret = RIL_E_SUCCESS;
        sp<RfxMclMessage> unsol = RfxMclMessage::obtainUrc(RFX_MSG_URC_RESPONSE_SIM_STATUS_CHANGED,
                m_slot_id, RfxVoidData());
        responseToTelCore(unsol);
    }

done:
    response = RfxMclMessage::obtainResponse(msg->getId(), ret, RfxVoidData(), msg, false);
    responseToTelCore(response);
}

void RmcCommSimOpRequestHandler::handleAttGetSimLockVersion(const sp<RfxMclMessage>& msg) {
    String8 cmd("");
    RfxAtLine *line = NULL;
    int err = -1;
    sp<RfxAtResponse> p_response = NULL;
    sp<RfxMclMessage> response;
    RIL_Errno ret = RIL_E_GENERIC_FAILURE;
    int info_len;
    char *info = NULL;

    logD(mTag, "[RSU-SIMLOCK] handleAttGetSimLockVersion\n");

    cmd.append(String8::format("AT+EATTSLB=3"));
    p_response = atSendCommandSingleline(cmd, "+EATTSLB:");
    cmd.clear();
    err = p_response->getError();
    if (err < 0 || p_response->getSuccess() == 0) {
        logD(mTag, "[RSU-SIMLOCK] handleAttGetSimLockVersion fail.\n");
        goto done;
    }
    line = p_response->getIntermediates();
    line->atTokStart(&err);
    if (err < 0) {
        goto done;
    }

    // Get info len
    info_len = line->atTokNextint(&err);
    if (err < 0) {
        goto done;
    }

    // Get info
    info = line->atTokNextstr(&err);
    if (err < 0) {
        goto done;
    }
    ret = RIL_E_SUCCESS;
done:
    logD(mTag, "[RSU-SIMLOCK] done.\n");
    if (info != NULL) {
        logD(mTag, "[RSU-SIMLOCK] info %s.\n", info);
        response = RfxMclMessage::obtainResponse(msg->getId(), ret,
                RfxStringData((void*)info, strlen(info)), msg, false);
    } else {
        response = RfxMclMessage::obtainResponse(msg->getId(), ret, RfxStringData(), msg, false);
    }
    responseToTelCore(response);
}

void RmcCommSimOpRequestHandler::handleAttResetSimLockSettings(const sp<RfxMclMessage>& msg) {
    String8 cmd("");
    RfxAtLine *line = NULL;
    int err = -1;
    sp<RfxAtResponse> p_response = NULL;
    sp<RfxMclMessage> response;
    RIL_Errno ret = RIL_E_GENERIC_FAILURE;

    logD(mTag, "[RSU-SIMLOCK] handleAttResetSimLockSettings\n");

    cmd.append(String8::format("AT+EATTSLB=2"));

    p_response = atSendCommand(cmd.string());
    cmd.clear();
    err = p_response->getError();
    if (err < 0) {
        goto done;
    }

    if (p_response->getSuccess() == 0) {
        switch (p_response->atGetCmeError()) {
            logD(mTag, "[RSU-SIMLOCK] p_response %d\n", p_response->atGetCmeError());
            case CME_VERIFICATION_FAILED:
                ret = RIL_E_VERIFICATION_FAILED;
                logD(mTag, "[RSU-SIMLOCK] p_response CME_SIM_LOCK_BLOB_VERIFICATION_FAILED\n");
                break;
            case CME_REBOOT_REQUEST:
                ret = RIL_E_REBOOT_REQUEST;
                logD(mTag, "[RSU-SIMLOCK] p_response CME_REBOOT_REQUEST\n");
                break;
            default:
                goto done;
        }
    } else {
        sp<RfxMclMessage> unsol = RfxMclMessage::obtainUrc(RFX_MSG_URC_RESPONSE_SIM_STATUS_CHANGED,
                m_slot_id, RfxVoidData());
        responseToTelCore(unsol);
    }
    ret = RIL_E_SUCCESS;
done:
    response = RfxMclMessage::obtainResponse(msg->getId(), ret, RfxVoidData(), msg, false);
    responseToTelCore(response);
}

void RmcCommSimOpRequestHandler::handleAttGetModemStatus(const sp<RfxMclMessage>& msg) {
    String8 cmd("");
    RfxAtLine *line = NULL;
    int err = -1;
    sp<RfxAtResponse> p_response = NULL;
    sp<RfxMclMessage> response;
    RIL_Errno ret = RIL_E_GENERIC_FAILURE;
    int status_len;
    char *status = NULL;

    logD(mTag, "[RSU-SIMLOCK] handleAttGetModemStatus\n");

    cmd.append(String8::format("AT+EATTSLB=4"));
    p_response = atSendCommandSingleline(cmd, "+EATTSLB:");
    cmd.clear();
    err = p_response->getError();
    if (err < 0 || p_response->getSuccess() == 0) {
        logD(mTag, "[RSU-SIMLOCK]handleAttGetModemStatus fail.\n");
        goto done;
    }
    line = p_response->getIntermediates();
    line->atTokStart(&err);
    if (err < 0) {
        goto done;
    }

    // Get status len
    status_len = line->atTokNextint(&err);
    if (err < 0) {
        goto done;
    }

    // Get status
    status = line->atTokNextstr(&err);
    if (err < 0) {
        goto done;
    }
    ret = RIL_E_SUCCESS;
done:
    logD(mTag, "[RSU-SIMLOCK] done.\n");
    if (status != NULL) {
        logD(mTag, "[RSU-SIMLOCK] status %s.\n", status);
        response = RfxMclMessage::obtainResponse(msg->getId(), ret,
                RfxStringData((void*)status, strlen(status)), msg, false);
    } else {
        response = RfxMclMessage::obtainResponse(msg->getId(), ret, RfxStringData(), msg, false);
    }
    responseToTelCore(response);
}

void RmcCommSimOpRequestHandler::handleRsuRequest(const sp<RfxMclMessage>& msg) {
    RIL_RsuRequestInfo* request = (RIL_RsuRequestInfo*)(msg->getData()->getData());
    logD(mTag, "[RSU-SIMLOCK] handleRsuRequest opId = %d, requestId = %d",
            request->opId, request->requestId);

    switch(request->opId) {
        case RSU_OP_VZW:
            handleVzwRsuRequest(msg);
            return;
        default:
            break;
    }

    sp<RfxMclMessage> mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
                    RIL_E_GENERIC_FAILURE, RfxVoidData(), msg);
    responseToTelCore(mclResponse);
}

void RmcCommSimOpRequestHandler::handleVzwRsuRequest(const sp<RfxMclMessage>& msg) {
    RIL_RsuRequestInfo* request = (RIL_RsuRequestInfo*)(msg->getData()->getData());

    switch(request->requestId) {
        case RSU_REQUEST_INIT_REQUEST:
            handleVzwRsuInitiate(msg);
            return;
        case RSU_REQUEST_GET_SHARED_KEY:
            handleVzwRsuGetSharedKey(msg);
            return;
        case RSU_REQUEST_UPDATE_LOCK_DATA:
            handleVzwRsuUpdateLockData(msg);
            return;
        case RSU_REQUEST_GET_LOCK_VERSION:
            handleVzwRsuGetVersion(msg);
            return;
        case RSU_REQUEST_GET_LOCK_STATUS:
            handleVzwRsuGetStatus(msg);
            return;
        case RSU_REQUEST_UNLOCK_TIMER:
            handleVzwRsuUnlockTimer(msg);
            return;
        default:
            break;
    }

    sp<RfxMclMessage> mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
                    RIL_E_GENERIC_FAILURE, RfxVoidData(), msg);
    responseToTelCore(mclResponse);
}

void RmcCommSimOpRequestHandler::handleVzwRsuInitiate(const sp<RfxMclMessage>& msg) {
    int err = 0;
    int errCode = 0;

    String8 cmd("");
    RfxAtLine *pLine = NULL;
    sp<RfxAtResponse> atResponse;
    sp<RfxMclMessage> mclResponse;

    int opId = -1;
    int requestId = -1;
    char* generateRequest = NULL;

    RIL_RsuRequestInfo* request = (RIL_RsuRequestInfo*)(msg->getData()->getData());
    RIL_RsuResponseInfo* response = (RIL_RsuResponseInfo*)calloc(1, sizeof(RIL_RsuResponseInfo));
    RFX_ASSERT(response != NULL);
    response->opId = request->opId;
    response->requestId = request->requestId;

    cmd.append(String8::format("AT+ESMLRSU=%d,%d", request->opId, RSU_MODEM_REQUEST_INIT_REQUEST));
    atResponse = atSendCommandSingleline(cmd, "+ESMLRSU:");
    if (atResponse == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuInitiate atResponse is null");
        goto error;
    }

    if (atResponse->isAtResponseFail()) {
        errCode = atResponse->atGetCmeError();
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuInitiate errCode = %d", errCode);
        response->errCode = errCode;
        mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
                RIL_E_GENERIC_FAILURE,
                RfxOpRsuResponseData(response, sizeof(RIL_RsuResponseInfo)), msg);
        responseToTelCore(mclResponse);
        free(response);
        return;
    }

    pLine = atResponse->getIntermediates();
    if (pLine == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuInitiate pLine is null");
        goto error;
    }
    pLine->atTokStart(&err);
    if (err < 0) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuInitiate atTokStart error");
        goto error;
    }

    opId = pLine->atTokNextint(&err);
    if (err < 0 || opId != request->opId) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuInitiate err < 0 or opId = %d"
                " not equal ot request opId = %d", opId, request->opId);
        goto error;
    }
    requestId = pLine->atTokNextint(&err);
    if (err < 0 || requestId != RSU_MODEM_REQUEST_INIT_REQUEST) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuInitiate err < 0 or requestId = %d"
                " not equal ot request requestId = %d", requestId, request->requestId);
        goto error;
    }

    generateRequest = pLine->atTokNextstr(&err);
    if (err < 0 || generateRequest == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuInitiate err < 0 or generateRequest is null");
        goto error;
    }
    response->data = generateRequest;
    logD(mTag, "[RSU-SIMLOCK] handleVzwRsuInitiate generateRequest = %s", response->data);

    mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
            RIL_E_SUCCESS, RfxOpRsuResponseData(response, sizeof(RIL_RsuResponseInfo)), msg);
    responseToTelCore(mclResponse);
    free(response);
    return;

error:
    free(response);
    mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
            RIL_E_GENERIC_FAILURE, RfxVoidData(), msg);
    responseToTelCore(mclResponse);
}

void RmcCommSimOpRequestHandler::handleVzwRsuGetSharedKey(const sp<RfxMclMessage>& msg) {
    int err = 0;
    int errCode = 0;

    String8 cmd("");
    RfxAtLine *pLine = NULL;
    sp<RfxAtResponse> atResponse;
    sp<RfxMclMessage> mclResponse;

    int opId = -1;
    int requestId = -1;
    char* sharedKey = NULL;

    RIL_RsuRequestInfo* request = (RIL_RsuRequestInfo*)(msg->getData()->getData());
    RIL_RsuResponseInfo* response = (RIL_RsuResponseInfo*)calloc(1, sizeof(RIL_RsuResponseInfo));
    RFX_ASSERT(response != NULL);
    response->opId = request->opId;
    response->requestId = request->requestId;

    cmd.append(String8::format("AT+ESMLRSU=%d,%d,\"%s\"",
            request->opId, RSU_MODEM_REQUEST_GET_SHARED_KEY, request->data));
    atResponse = atSendCommandSingleline(cmd, "+ESMLRSU:");
    if (atResponse == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetSharedKey atResponse is null");
        goto error;
    }

    if (atResponse->isAtResponseFail()) {
        errCode = atResponse->atGetCmeError();
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetSharedKey errCode = %d", errCode);
        response->errCode = errCode;
        mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
                RIL_E_GENERIC_FAILURE,
                RfxOpRsuResponseData(response, sizeof(RIL_RsuResponseInfo)), msg);
        responseToTelCore(mclResponse);
        free(response);
        return;
    }

    pLine = atResponse->getIntermediates();
    if (pLine == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetSharedKey pLine is null");
        goto error;
    }
    pLine->atTokStart(&err);
    if (err < 0) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetSharedKey atTokStart error");
        goto error;
    }

    opId = pLine->atTokNextint(&err);
    if (err < 0 || opId != request->opId) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetSharedKey err < 0 or opId = %d"
                " not equal ot request opId = %d", opId, request->opId);
        goto error;
    }
    requestId = pLine->atTokNextint(&err);
    if (err < 0 || requestId != RSU_MODEM_REQUEST_GET_SHARED_KEY) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetSharedKey err < 0 or requestId = %d"
                " not equal ot request requestId = %d", requestId, request->requestId);
        goto error;
    }

    sharedKey = pLine->atTokNextstr(&err);
    if (err < 0 || sharedKey == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetSharedKey err < 0 or sharedKey is null");
        goto error;
    }
    response->data = sharedKey;
    logD(mTag, "[RSU-SIMLOCK] handleVzwRsuGetSharedKey sharedKey = %s", response->data);

    mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
            RIL_E_SUCCESS, RfxOpRsuResponseData(response, sizeof(RIL_RsuResponseInfo)), msg);
    responseToTelCore(mclResponse);
    free(response);
    return;

error:
    free(response);
    mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
            RIL_E_GENERIC_FAILURE, RfxVoidData(), msg);
    responseToTelCore(mclResponse);
}

void RmcCommSimOpRequestHandler::handleVzwRsuUpdateLockData(const sp<RfxMclMessage>& msg) {
    int err = 0;
    int errCode = 0;

    String8 cmd("");
    RfxAtLine *pLine = NULL;
    sp<RfxAtResponse> atResponse;
    sp<RfxMclMessage> mclResponse;

    int opId = -1;
    int requestId = -1;
    char* lockData = NULL;

    RIL_RsuRequestInfo* request = (RIL_RsuRequestInfo*)(msg->getData()->getData());
    RIL_RsuResponseInfo* response = (RIL_RsuResponseInfo*)calloc(1, sizeof(RIL_RsuResponseInfo));
    RFX_ASSERT(response != NULL);
    response->opId = request->opId;
    response->requestId = request->requestId;

    cmd.append(String8::format("AT+ESMLRSU=%d,%d,\"%s\"",
            request->opId, RSU_MODEM_REQUEST_UPDATE_LOCK_DATA, request->data));
    atResponse = atSendCommandSingleline(cmd, "+ESMLRSU:");
    if (atResponse == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUpdateLockData atResponse is null");
        goto error;
    }

    if (atResponse->isAtResponseFail()) {
        errCode = atResponse->atGetCmeError();
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUpdateLockData errCode = %d", errCode);
        response->errCode = errCode;
        mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
                RIL_E_GENERIC_FAILURE,
                RfxOpRsuResponseData(response, sizeof(RIL_RsuResponseInfo)), msg);
        responseToTelCore(mclResponse);
        free(response);
        return;
    }

    pLine = atResponse->getIntermediates();
    if (pLine == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUpdateLockData pLine is null");
        goto error;
    }
    pLine->atTokStart(&err);
    if (err < 0) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUpdateLockData atTokStart error");
        goto error;
    }

    opId = pLine->atTokNextint(&err);
    if (err < 0 || opId != request->opId) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUpdateLockData err < 0 or opId = %d"
                " not equal ot request opId = %d", opId, request->opId);
        goto error;
    }
    requestId = pLine->atTokNextint(&err);
    if (err < 0 || requestId != RSU_MODEM_REQUEST_UPDATE_LOCK_DATA) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUpdateLockData err < 0 or requestId = %d"
                " not equal ot request requestId = %d", requestId, request->requestId);
        goto error;
    }

    lockData = pLine->atTokNextstr(&err);
    if (err < 0 || lockData == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUpdateLockData err < 0 or lockData is null");
        goto error;
    }
    response->data = lockData;
    logD(mTag, "[RSU-SIMLOCK] handleVzwRsuUpdateLockData lockData = %s", response->data);

    mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
            RIL_E_SUCCESS, RfxOpRsuResponseData(response, sizeof(RIL_RsuResponseInfo)), msg);
    responseToTelCore(mclResponse);
    free(response);
    return;

error:
    free(response);
    mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
            RIL_E_GENERIC_FAILURE, RfxVoidData(), msg);
    responseToTelCore(mclResponse);
}

void RmcCommSimOpRequestHandler::handleVzwRsuGetVersion(const sp<RfxMclMessage>& msg) {
    int err = 0;
    int errCode = 0;
    String8 cmd("");

    RfxAtLine *pLine = NULL;
    sp<RfxAtResponse> atResponse;
    sp<RfxMclMessage> mclResponse;

    int opId = -1;
    int requestId = -1;
    char* majorVersion = NULL;
    char* minorVersion = NULL;

    RIL_RsuRequestInfo* request = (RIL_RsuRequestInfo*)(msg->getData()->getData());
    RIL_RsuResponseInfo* response = (RIL_RsuResponseInfo*)calloc(1, sizeof(RIL_RsuResponseInfo));
    RFX_ASSERT(response != NULL);
    response->opId = request->opId;
    response->requestId = request->requestId;

    cmd.append(String8::format("AT+ESMLRSU=%d,%d",
            request->opId, RSU_MODEM_REQUEST_GET_LOCK_VERSION));
    atResponse = atSendCommandSingleline(cmd, "+ESMLRSU:");
    if (atResponse == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetVersion atResponse is null");
        goto error;
    }

    if (atResponse->isAtResponseFail()) {
        errCode = atResponse->atGetCmeError();
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetVersion errCode = %d", errCode);
        response->errCode = errCode;
        mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
                RIL_E_GENERIC_FAILURE,
                RfxOpRsuResponseData(response, sizeof(RIL_RsuResponseInfo)), msg);
        responseToTelCore(mclResponse);
        free(response);
        return;
    }

    pLine = atResponse->getIntermediates();
    if (pLine == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetVersion pLine is null");
        goto error;
    }
    pLine->atTokStart(&err);
    if (err < 0) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetVersion atTokStart error");
        goto error;
    }

    opId = pLine->atTokNextint(&err);
    if (err < 0 || opId != request->opId) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetVersion err < 0 or opId = %d"
                " not equal ot request opId = %d", opId, request->opId);
        goto error;
    }
    requestId = pLine->atTokNextint(&err);
    if (err < 0 || requestId != RSU_MODEM_REQUEST_GET_LOCK_VERSION) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetVersion err < 0 or requestId = %d"
                " not equal ot request requestId = %d", requestId, request->requestId);
        goto error;
    }

    majorVersion = pLine->atTokNextstr(&err);
    if (err < 0 || majorVersion == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetVersion err < 0 or majorVersion is NULL");
        goto error;
    }
    if (pLine->atTokHasmore()) {
        minorVersion = pLine->atTokNextstr(&err);
        if (err < 0 || minorVersion == NULL) {
            logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetVersion err < 0 or minorVersion is NULL");
            goto error;
        }
    } else {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetVersion no minorVersion");
        goto error;
    }
    response->version = ((atoi(majorVersion) << 16) + atoi(minorVersion));
    logD(mTag, "[RSU-SIMLOCK] handleVzwRsuGetVersion version = %d", response->version);

    mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
            RIL_E_SUCCESS, RfxOpRsuResponseData(response, sizeof(RIL_RsuResponseInfo)), msg);
    responseToTelCore(mclResponse);
    free(response);
    return;

error:
    free(response);
    mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
            RIL_E_GENERIC_FAILURE, RfxVoidData(), msg);
    responseToTelCore(mclResponse);
}

void RmcCommSimOpRequestHandler::handleVzwRsuGetStatus(const sp<RfxMclMessage>& msg) {
    int err = 0;
    int errCode = 0;
    String8 cmd("");

    RfxAtLine *pLine = NULL;
    sp<RfxAtResponse> atResponse;
    sp<RfxMclMessage> mclResponse;

    int opId = -1;
    int requestId = -1;
    char* unlockStatus = NULL;
    char* unlockTime = NULL;

    RIL_RsuRequestInfo* request = (RIL_RsuRequestInfo*)(msg->getData()->getData());
    RIL_RsuResponseInfo* response = (RIL_RsuResponseInfo*)calloc(1, sizeof(RIL_RsuResponseInfo));
    RFX_ASSERT(response != NULL);
    response->opId = request->opId;
    response->requestId = request->requestId;

    cmd.append(String8::format("AT+ESMLRSU=%d,%d",
            request->opId, RSU_MODEM_REQUEST_GET_LOCK_STATUS));
    atResponse = atSendCommandSingleline(cmd, "+ESMLRSU:");
    if (atResponse == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetStatus atResponse is null");
        goto error;
    }

    if (atResponse->isAtResponseFail()) {
        errCode = atResponse->atGetCmeError();
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetStatus errCode = %d", errCode);
        response->errCode = errCode;
        mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
                RIL_E_GENERIC_FAILURE,
                RfxOpRsuResponseData(response, sizeof(RIL_RsuResponseInfo)), msg);
        responseToTelCore(mclResponse);
        free(response);
        return;
    }

    pLine = atResponse->getIntermediates();
    if (pLine == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetStatus pLine is null");
        goto error;
    }
    pLine->atTokStart(&err);
    if (err < 0) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetStatus atTokStart error");
        goto error;
    }

    opId = pLine->atTokNextint(&err);
    if (err < 0 || opId != request->opId) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetStatus err < 0 or opId = %d"
                " not equal ot request opId = %d", opId, request->opId);
        goto error;
    }
    requestId = pLine->atTokNextint(&err);
    if (err < 0 || requestId != RSU_MODEM_REQUEST_GET_LOCK_STATUS) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetStatus err < 0 or requestId = %d"
                " not equal ot request requestId = %d", requestId, request->requestId);
        goto error;
    }

    unlockStatus = pLine->atTokNextstr(&err);
    if (err < 0 || unlockStatus == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetStatus err < 0 or unlockStatus is NULL");
        goto error;
    }
    response->status = atoi(unlockStatus);
    if (pLine->atTokHasmore()) {
        unlockTime = pLine->atTokNextstr(&err);
        if (err < 0 || unlockTime == NULL) {
            logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetStatus err < 0 or unlockTime is null");
            goto error;
        }
        response->time = atoll(unlockTime);
    } else {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuGetStatus no unlockTime");
        goto error;
    }
    logD(mTag, "[RSU-SIMLOCK] handleVzwRsuGetStatus unlockStatus = %d unlockTime = %lld",
            response->status, response->time);

    mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
            RIL_E_SUCCESS, RfxOpRsuResponseData(response, sizeof(RIL_RsuResponseInfo)), msg);
    responseToTelCore(mclResponse);
    free(response);
    return;

error:
    free(response);
    mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
            RIL_E_GENERIC_FAILURE, RfxVoidData(), msg);
    responseToTelCore(mclResponse);
}

void RmcCommSimOpRequestHandler::handleVzwRsuUnlockTimer(const sp<RfxMclMessage>& msg) {
    int err = 0;
    int errCode = 0;

    String8 cmd("");
    RfxAtLine *pLine = NULL;
    sp<RfxAtResponse> atResponse;
    sp<RfxMclMessage> mclResponse;

    int opId = -1;
    int requestId = -1;
    char* timerStatus = NULL;
    char* passedTime = NULL;

    RIL_RsuRequestInfo* request = (RIL_RsuRequestInfo*)(msg->getData()->getData());
    RIL_RsuResponseInfo* response = (RIL_RsuResponseInfo*)calloc(1, sizeof(RIL_RsuResponseInfo));
    RFX_ASSERT(response != NULL);
    response->opId = request->opId;
    response->requestId = request->requestId;

    cmd.append(String8::format("AT+ESMLRSU=%d,%d,%d",
            request->opId, RSU_MODEM_REQUEST_UNLOCK_TIMER, (request->requestType == 2 ? 0 : 1)));
    atResponse = atSendCommandSingleline(cmd, "+ESMLRSU:");
    if (atResponse == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUnlockTimer atResponse is null");
        goto error;
    }

    if (atResponse->isAtResponseFail()) {
        errCode = atResponse->atGetCmeError();
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUnlockTimer errCode = %d", errCode);
        response->errCode = errCode;
        mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
                RIL_E_GENERIC_FAILURE,
                RfxOpRsuResponseData(response, sizeof(RIL_RsuResponseInfo)), msg);
        responseToTelCore(mclResponse);
        free(response);
        return;
    }

    pLine = atResponse->getIntermediates();
    if (pLine == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUnlockTimer pLine is null");
        goto error;
    }
    pLine->atTokStart(&err);
    if (err < 0) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUnlockTimer atTokStart error");
        goto error;
    }

    opId = pLine->atTokNextint(&err);
    if (err < 0 || opId != request->opId) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUnlockTimer er < 0 or opId = %d"
                " not equal ot request opId = %d", opId, request->opId);
        goto error;
    }
    requestId = pLine->atTokNextint(&err);
    if (err < 0 || requestId != RSU_MODEM_REQUEST_UNLOCK_TIMER) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUnlockTimer err < 0 or requestId = %d"
                " not equal ot request requestId = %d", requestId, request->requestId);
        goto error;
    }

    timerStatus = pLine->atTokNextstr(&err);
    if (err < 0 || timerStatus == NULL) {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUnlockTimer err < 0 or timerStatus is NULL");
        goto error;
    }
    response->errCode = atoi(timerStatus);
    if (pLine->atTokHasmore()) {
        passedTime = pLine->atTokNextstr(&err);
        if (err < 0 || passedTime == NULL) {
            logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUnlockTimer err < 0 or passedTime is null");
            goto error;
        }
        response->time = atoll(passedTime);
    } else {
        logE(mTag, "[RSU-SIMLOCK] handleVzwRsuUnlockTimer no passedTime");
        goto error;
    }
    logD(mTag, "[RSU-SIMLOCK] handleVzwRsuUnlockTimer timerStatus = %d passedTime = %lld",
            response->status, response->time);

    mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
            RIL_E_SUCCESS, RfxOpRsuResponseData(response, sizeof(RIL_RsuResponseInfo)), msg);
    responseToTelCore(mclResponse);
    free(response);
    return;

error:
    free(response);
    mclResponse = RfxMclMessage::obtainResponse(msg->getId(),
            RIL_E_GENERIC_FAILURE, RfxVoidData(), msg);
    responseToTelCore(mclResponse);
}
