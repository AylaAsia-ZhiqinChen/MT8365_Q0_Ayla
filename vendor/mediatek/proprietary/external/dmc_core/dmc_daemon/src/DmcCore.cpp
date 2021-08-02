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

#include "DmcCore.h"

#include "SubscriptionBatchMdmRequest.h"
#include <sys/system_properties.h>
#include <unistd.h>
#include <map>

#undef TAG
#define TAG "DMC-Core"

using namespace std;

DmcCore::DmcCore(): mReady(false), mSubscriptionEnabled(false) {
    // DMC HIDL service
    m_pDmcService = new DmcService(this);
    if (m_pDmcService->registDmcHidlServiceAsyc()) {
        m_pSessionManager = new SessionManager();
        m_pReqQueueManager = new ReqQManager();
        m_pTranslatorLoader = new TranslatorLoader();
        m_pReqQueueManager->SetEventHandler(eventsCb, this);

        m_pKpiSubscriptionTable = new KpiSubscriptionTable();

        m_pLocalEventsSubscriber = new LocalEventsSubscriber(
                m_pKpiSubscriptionTable, m_pReqQueueManager, m_pTranslatorLoader);
        m_pApmEventsSubscriber = new ApmEventsSubscriber(
                m_pKpiSubscriptionTable, m_pReqQueueManager, m_pTranslatorLoader);
        m_pMdmEventsSubscriber = new MdmEventsSubscriber(
                m_pKpiSubscriptionTable, m_pReqQueueManager, m_pTranslatorLoader);

        // TODO: test code
        /*
        DMC_LOGD(TAG, "subscribe 1.1.1");
        subscribe(ObjId("1.1.1"));
        DMC_LOGD(TAG, "subscribe 1.3.4");
        subscribe(ObjId("1.3.4"));
        DMC_LOGD(TAG, "subscribe 1.1.2");
        subscribe(ObjId("1.1.2"));
        DMC_LOGD(TAG, "subscribe 1.2.2.1");
        subscribe(ObjId("1.2.2.1"));
        DMC_LOGD(TAG, "subscribe 1.3");
        subscribe(ObjId("1.3"));
        */
        m_pKpiSubscriptionTable->dump();
        mReady = true;
        DMC_LOGI(TAG, "DMC service ready!");

    } else {
        DMC_LOGE(TAG, "start DMC service failed!");
    }
}

DmcCore::~DmcCore()
{
    delete m_pSessionManager;
    m_pSessionManager = NULL;
    delete m_pLocalEventsSubscriber;
    m_pLocalEventsSubscriber = NULL;
    delete m_pApmEventsSubscriber;
    m_pApmEventsSubscriber = NULL;
    delete m_pMdmEventsSubscriber;
    m_pMdmEventsSubscriber = NULL;
    delete m_pReqQueueManager;
    m_pReqQueueManager = NULL;
    delete m_pTranslatorLoader;
    m_pTranslatorLoader = NULL;
    m_pKpiSubscriptionTable->reset();
    delete m_pKpiSubscriptionTable;
    m_pKpiSubscriptionTable = NULL;
}

bool DmcCore::isReady() {
    return mReady;
}

DMC_RESULT_CODE DmcCore::eventsCb(record_t *pRec, void *param) {
    DmcCore *me = (DmcCore *)param;
    DMC_RESULT_CODE ret = DMC_RESULT_FAIL;

    if (!me->mSubscriptionEnabled) {
        DMC_LOGW(TAG, "Skip eventsCb(), subscriiption disabled");
        return DMC_RESULT_SKIPPED;
    }

    if (pRec == NULL) {
        DMC_LOGE(TAG, "Invalid pRec!");
        return DMC_RESULT_SKIPPED;
    }
    SessionEntry *entry = NULL;
    ret = me->m_pSessionManager->getCurrentSessionEntry(&entry);
    if (ret != DMC_RESULT_SUCCESS) {
        DMC_LOGE(TAG, "No active session found!");
        return DMC_RESULT_SKIPPED;
    }
    KPI_OBJ_TYPE type = (KPI_OBJ_TYPE)pRec->kpiIndex;
    const SubscriptionInfo *info = me->m_pKpiSubscriptionTable->findSubscriptionInfo(type);
    if (NULL == info) {
        DMC_LOGE(TAG, "Can't find subscription entry for type = %d", type);
        return DMC_RESULT_SKIPPED;
    }
    KpiObj kpiObj(type);
    kpiObj.SetByteArray((const uint8_t *)pRec->kpiData, pRec->kpiSize);
    ObjId objId(info->oid.getOidString());

    KPI_GROUP_TYPE group = (KPI_GROUP_TYPE)me->m_pTranslatorLoader->getKpiGroupId(type);
    if (KPI_GROUP_TYPE_UNDEFINED == group) {
        DMC_LOGE(TAG, "Invalid GID with type = %d, assigned to 0", type);
        group = 0;
    }

    DMC_LOGD(TAG, "Call DMC client EventCallback, SID = %d, ID = %s, KPI = %d, OID = %s, GID = %d",
            entry->getSessionId(), entry->getSessionInfo()->identity, type, objId.getOidString(), group);
    ret = me->m_pEventCallback(entry->getSessionId(), group, objId, kpiObj, me->m_pEventParam);

    return DMC_RESULT_SUCCESS;
}

DMC_RESULT_CODE DmcCore::createSession(const SessionInfo *info, int32_t &sessionId) {
    DMC_RESULT_CODE retCode = DMC_RESULT_SUCCESS;
    sessionId = INVALID_SESSION_ID;

    do {
        // #1. Create session
        SessionEntry *pEntry = NULL;
        if (m_pSessionManager->getSessionEntry(info->identity, &pEntry)) {
            sessionId = pEntry->getSessionId();
            DMC_LOGW(TAG, "Reuse session %d for identify = %s", sessionId, info->identity);
            break;
        }
        sessionId = m_pSessionManager->createSessionEntry(info);
        if (sessionId == INVALID_SESSION_ID) {
            DMC_LOGE(TAG, "create session failed or skipped!");
            retCode = DMC_RESULT_FAIL;
            break;
        }

        // #2. Load specified trnaslator library
        // We need to start MDM service first and then load translator library,
        // Otherwise, there will not be EM/ICD layout file and parse fail.
        if (!MdmEventsSubscriber::StartMdmServiceSync()) {
            DMC_LOGE(TAG, "Failed to start MDM service!");
            retCode = DMC_RESULT_FAIL;
            break;
        }
        if (!m_pTranslatorLoader->load(info->identity, info->targetVersion)) {
            DMC_LOGE(TAG, "load translator lib failed or skipped!");
            retCode = DMC_RESULT_FAIL;
            break;
        }

        DMC_LOGI(TAG, "createSession() success! SID = %d", sessionId);
    } while(false);

    return retCode;
}

DMC_RESULT_CODE DmcCore::closeSession(int32_t sessionId) {
    SessionEntry *pEntry = NULL;
    if (m_pSessionManager->getSessionEntry(sessionId, &pEntry)) {
        DMC_LOGI(TAG, "closeSession() SID = %d", sessionId);
        // #1. Disable all subscribers
        disableSubscription();

        // #2. Unload current trnaslator library
        m_pTranslatorLoader->unload();

        // #3. Reset subscription table
        m_pKpiSubscriptionTable->reset();

        // #4. Empty request queue to clear on-going traps
        m_pReqQueueManager->EmptyRequestQueue();

        // #5. Remove session
        if (pEntry != NULL) {
            m_pSessionManager->removeSessionEntry(*pEntry);
        } else {
            DMC_LOGE(TAG, "Null pEntry with SID = %d", sessionId);
        }
        // #6. It is impossible to unregist DMC HIDL service

        return DMC_RESULT_SUCCESS;
    }
    DMC_LOGE(TAG, "closeSession() skip! SID = %d", sessionId);
    return DMC_RESULT_FAIL;
}

bool DmcCore::isValidSession(int32_t sessionId) {
    SessionEntry *pEntry = NULL;
    return m_pSessionManager->getSessionEntry(sessionId, &pEntry);
}

DMC_RESULT_CODE DmcCore::registerEventCallback(EventCallback callback, void *param) {
    m_pEventCallback = callback;
    m_pEventParam = param;
    return DMC_RESULT_SUCCESS;
}

DMC_RESULT_CODE DmcCore::enableSubscription() {
    int error = 0;
    if (!m_pMdmEventsSubscriber->enableSubscriber()) {
        DMC_LOGE(TAG, "Enable MdmEventsSubscriber fail!");
        error++;
    }
    if (!m_pApmEventsSubscriber->enableSubscriber()) {
        DMC_LOGE(TAG, "Enable ApmEventsSubscriber fail!");
        error++;
    }
    if (!m_pLocalEventsSubscriber->enableSubscriber()) {
        DMC_LOGE(TAG, "Enable LocalEventsSubscriber fail!");
        error++;
    }

    if (error == 0) {
        mSubscriptionEnabled = true;
        DMC_LOGD(TAG, "enableSubscription() success!");
        return DMC_RESULT_SUCCESS;
    }
    DMC_LOGE(TAG, "enableSubscription() fail!");
    return DMC_RESULT_FAIL;
}

DMC_RESULT_CODE DmcCore::disableSubscription() {
    mSubscriptionEnabled = false;
    m_pLocalEventsSubscriber->disableSubscriber();
    m_pApmEventsSubscriber->disableSubscriber();
    m_pMdmEventsSubscriber->disableSubscriber();

    DMC_LOGD(TAG, "disableSubscription() success!");
    return DMC_RESULT_SUCCESS;
}

DMC_RESULT_CODE DmcCore::setPacketPayloadSize(const ObjId *oid, uint32_t size) {
    return m_pApmEventsSubscriber->setPacketPayloadSize(*oid, size);
}

DMC_RESULT_CODE DmcCore::setpacketpayloadType(const ObjId *oid, OTA_PACKET_TYPE type) {
    return m_pApmEventsSubscriber->setPacketPayloadType(*oid, type);
}

DMC_RESULT_CODE DmcCore::getSessionStats(
        int32_t session, KPI_GROUP_TYPE group, SessionStats_t &stats) {
    UNUSED(session);
    UNUSED(group);
    UNUSED(stats);
    return DMC_RESULT_SUCCESS;
}

DMC_RESULT_CODE DmcCore::queryEvent(int32_t session, const ObjId *oid, KpiObj &obj) {
    // TODO: Support query mode
    UNUSED(session);
    UNUSED(oid);
    UNUSED(obj);
    return DMC_RESULT_SUCCESS;
}

DMC_RESULT_CODE DmcCore::subscribe(const ObjId *oid) {
    return subscribeHandler(oid, true, true);
}

DMC_RESULT_CODE DmcCore::unSubscribe(const ObjId *oid) {
    return subscribeHandler(oid, false, true);
}

DMC_RESULT_CODE DmcCore::subscribeHandler(const ObjId *oid, bool bSubscribe, bool bBatchMode) {
    DMC_LOGD(TAG, "subscribeHandler(%s), bSubscribe = %d", oid->getOidString(), bSubscribe);

    map<KPI_OBJ_TYPE, ObjId *> kpiMap;
    int count = m_pTranslatorLoader->getKpiTypeMapByOid(*oid, kpiMap);

    if (count == 0) {
        DMC_LOGD(TAG, "no matching KPI type");
        return DMC_RESULT_FAIL;
    }

    map<KPI_OBJ_TYPE, ObjId *>::iterator it;
    SubscriptionBatchMdmRequest *pMdmBatchRequest = new SubscriptionBatchMdmRequest();
    if (pMdmBatchRequest == NULL) {
        DMC_LOGE(TAG, "NULL pMdmBatchRequest!");
        return DMC_RESULT_FAIL;
    }

    for (it = kpiMap.begin(); it != kpiMap.end(); ++it) {
        KPI_OBJ_TYPE type = it->first;
        ObjId *pOid = it->second;

        if (bSubscribe) {
            if (!m_pKpiSubscriptionTable->isKpiSubscribed(type)) {
                SubscriptionInfo *pInfo = new SubscriptionInfo(type, *pOid);
                m_pKpiSubscriptionTable->regist(pInfo);
                DMC_LOGD(TAG, "regist KPI type map<type, oid> [%d, %s]", type, pOid->getOidString());
            } else {
                DMC_LOGW(TAG, "skip subscribe KPI type = %d, oid = %s", type, pOid->getOidString());
                continue;
            }
        } else {
            if (!m_pKpiSubscriptionTable->isKpiSubscribed(type)) {
                DMC_LOGW(TAG, "skip unsubscribe KPI type = %d, oid = %s", type, pOid->getOidString());
                continue;
            } else {
                // Remove subscribed entry
                m_pKpiSubscriptionTable->unregist(type);
                DMC_LOGD(TAG, "unregist KPI type map<type, oid> [%d, %s]", type, pOid->getOidString());
            }
        }

        const TrapTable *pTrapTable = m_pTranslatorLoader->getTrapTableByKpi(type);
        if (pTrapTable != NULL) {
            const TrapTable &trapTable = *pTrapTable;

            if (trapTable.Size() == 0) {
                DMC_LOGD(TAG, "get empty trapTable for KPI type = %d", type);
                continue;
            }

            for (size_t i = 0; i < trapTable.Size(); ++i) {
                TrapInfo info = trapTable[i];
                KPI_SOURCE_TYPE source = info.m_sourceType;
                MSGID msgId = info.m_msgId;
                DMC_LOGD(TAG, "get TrapInfo msgId = %" PRIu64", source = %s",
                        msgId, KpiUtils::kpiSource2String(source));

                switch(source) {
                    case KPI_SOURCE_TYPE_LOCAL:
                        m_pLocalEventsSubscriber->SubscribeHandler(bSubscribe, type, source, msgId);
                        break;
                    case KPI_SOURCE_TYPE_APM:
                        m_pApmEventsSubscriber->SubscribeHandler(bSubscribe, type, source, msgId);
                        break;
                    case KPI_SOURCE_TYPE_MDM_EM_MSG:
                    case KPI_SOURCE_TYPE_MDM_EM_OTA:
                    case KPI_SOURCE_TYPE_MDM_ICD_EVENT:
                    case KPI_SOURCE_TYPE_MDM_ICD_RECORD:
                        if (bBatchMode) {
                            // Batch subscribe MDM events
                            pMdmBatchRequest->add(source, SubscriptionRequest(type, msgId));
                        } else {
                            m_pMdmEventsSubscriber->SubscribeHandler(bSubscribe, type, source, msgId);
                        }
                        break;
                    case KPI_SOURCE_TYPE_PKM:
                        DMC_LOGE(TAG, "Should NOT get KPI_SOURCE_TYPE_PKM!");
                        break;
                    default:
                        DMC_LOGE(TAG, "Unexpected trap source: %s", KpiUtils::kpiSource2String(source));
                        break;
                }
            }
        } else {
            DMC_LOGE(TAG, "Null TrapTable for KPI type %d, oid = %s", type, pOid->getOidString());
        }
    }

    // Batch subscribe MDM events
    // In current modem ICD design, need to take 3 seconds (timeout) for a single request.
    if (bBatchMode && pMdmBatchRequest->size() > 0) {
        // pMdmBatchRequest->dump();
        m_pMdmEventsSubscriber->SubscribeHandlerBatch(bSubscribe, pMdmBatchRequest);
        pMdmBatchRequest->clear();
    }
    delete pMdmBatchRequest;

    return DMC_RESULT_SUCCESS;
}
