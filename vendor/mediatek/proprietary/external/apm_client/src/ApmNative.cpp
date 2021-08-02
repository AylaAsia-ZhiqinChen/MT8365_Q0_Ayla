/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2019. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include "ap_monitor.h"
#include "ApmSession.h"
#include "Logs.h"
#include "ap_monitor_defs_legacy.h"
#include <vendor/mediatek/hardware/apmonitor/2.0/IApmService.h>

#include <malloc.h>
#include <inttypes.h>

#undef TAG
#define TAG "APM-SessionN"

bool tryApmServiceConnection();
APM_MSGID getApmMsgId(int legacyTypeId);

//==================[ New APM Interface ]=========================
extern "C" APM_RESULT_CODE apmSubmitKpi(APM_MSGID msgId,
        uint32_t len, const void *data) {
    return apmSubmitKpiST(msgId, SIM_ID_DEFAULT, 0, len, data);
}

extern "C" APM_RESULT_CODE apmSubmitKpiS(APM_MSGID msgId, APM_SIMID simId,
        uint32_t len, const void *data) {
    return apmSubmitKpiST(msgId, simId, 0, len, data);
}

extern "C" APM_RESULT_CODE apmSubmitKpiT(APM_MSGID msgId, timeStamp timestampMs,
        uint32_t len, const void *data) {
    return apmSubmitKpiST(msgId, SIM_ID_DEFAULT, timestampMs, len, data);
}

extern "C" APM_RESULT_CODE apmSubmitKpiST(APM_MSGID msgId, APM_SIMID simId, timeStamp timestampMs,
        uint32_t len, const void *data) {
    APM_LOGD(TAG, "[%d]apmSubmitKpi: msgId = %d, time = %" PRIu64, simId, msgId, timestampMs);
    if (!tryApmServiceConnection()) {
        APM_LOGE(TAG, "tryApmServiceConnection() fail!");
        return APM_RESULT_FAILED;
    }

    if (!g_pSession->isSubscriptionStateInitialized()) {
        APM_LOGE(TAG, "APM KPI subscription state not initialized!");
        return APM_RESULT_FAILED;
    }

    if (!g_pSession->shouldSubmitKpi(msgId)) {
        APM_LOGW(TAG, "APM KPI %d not subscribed, skip~", msgId);
        return APM_RESULT_SUCCESS;
    }

    // Specify current time
    if (timestampMs == 0) {
        timestampMs = g_pSession->getCurrentTimestamp();
    }

    g_pSession->submitKpi(msgId, simId, timestampMs, len, data);

    return APM_RESULT_SUCCESS;
}

extern "C" bool apmShouldSubmitKpi(APM_MSGID msgId) {
    if (!tryApmServiceConnection()) {
        return false;
    }

    return g_pSession->shouldSubmitKpi(msgId);
}

extern "C" int64_t apmGetSessionParam() {
    if (!tryApmServiceConnection()) {
        APM_LOGE(TAG, "tryApmServiceConnection() fail!");
        return 0;
    }
    return g_pSession->getSessionParam();
}


//==================[ Legacy APM Interface ]=========================
//Send data to the socket server referred by sock_p
//Arguments:
// - type: (in)  native module type present in KPI_TYPE enum
// - data: (in) data to be sent to socket server
// - data_len: (in) data length of the data to be sent to socket server
//Returns: 0 on success, -1 for error
int apmSend(int type, void *data) {
    UNUSED(type);
    UNUSED(data);

    return -1;

    // TODO: Covert ApmSuplStatistics/ApmSuplMessage to HIDL style
    /*
    //APM_MSGID msgId = getApmMsgId(type);

    if (INVALID_APM_MSGID == msgId) {
        return -1;
    }

    if (APM_RESULT_SUCCESS != apmSubmitKpi(msgId, data)) {
        return -1;
    }
    return 0;
    */
}


//Check if logging is enabled for kpi type
//Arguments:
// - type: (in) type present in KPI_TYPE enum
//Returns: kpi type enabled status
bool apmIsKpiEnabled(int type) {
    APM_MSGID msgId = getApmMsgId(type);

    if (INVALID_APM_MSGID == msgId) {
        return false;
    }

    return apmShouldSubmitKpi(msgId);
}

bool tryApmServiceConnection() {
    if (NULL == g_pSession) {
        g_pSession = new ApmSession();
    }

    if (!g_pSession->isApmServiceReady()) {
        if (!g_pSession->connect()) {
            // APM_LOGW(TAG, "APM service is not ready!");
            return false;
        }
    }
    return true;
}

APM_MSGID getApmMsgId(int legacyTypeId) {
    return INVALID_APM_MSGID;
}

