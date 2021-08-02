/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2017. All rights reserved.
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

#include "RmcRadioUrcHandler.h"
#include "RfxIntsData.h"

#define RFX_LOG_TAG "RmcRadUrc"

// register handler to channel
RFX_IMPLEMENT_HANDLER_CLASS(RmcRadioUrcHandler, RIL_CMD_PROXY_URC);

RFX_REGISTER_DATA_TO_URC_ID(RfxIntsData, RFX_MSG_UNSOL_DSBP_CHANGED_INDICATION);

RmcRadioUrcHandler::RmcRadioUrcHandler(int slot_id, int channel_id) :
        RfxBaseHandler(slot_id, channel_id) {
    const char* urc[] = {
        (char *) "+EDSBP",
    };

    registerToHandleURC(urc, sizeof(urc)/sizeof(char*));
    mPreRadioState = RADIO_STATE_OFF;
}

RmcRadioUrcHandler::~RmcRadioUrcHandler() {
}

void RmcRadioUrcHandler::onHandleUrc(const sp<RfxMclMessage>& msg) {
    int err = 0;
    RfxAtLine *line = msg->getRawUrc();
    char *data = line->getLine();
    if (strStartsWith(data, "+EDSBP")) {
        handleDSBPEnhancement(line);
    } else {
        RFX_LOG_E(RFX_LOG_TAG, "we can not handle this raw urc?! %s", data);
    }
}

void RmcRadioUrcHandler::handleDSBPEnhancement(RfxAtLine *line) {
    int err = 0;
    int sbp, state;
    line->atTokStart(&err);

    // SBP value
    sbp = line->atTokNextint(&err);
    if (err < 0) {
        RFX_LOG_E(RFX_LOG_TAG, "parser +EDSBP error!");
        return;
    }
    // SBP state
    state = line->atTokNextint(&err);
    if (err < 0) {
        RFX_LOG_E(RFX_LOG_TAG, "parser +EDSBP error!");
        return;
    }
    logI(RFX_LOG_TAG, "handleDSBPEnhancement: SBP: %d, state: %d", sbp, state);

    if (DSBP_ENHANCEMENT_START == state) {
        getMclStatusManager()->setIntValue(RFX_STATUS_KEY_DSBP_ENHANCEMENT_STATE,
                DSBP_ENHANCEMENT_START);
    } else {
        getMclStatusManager()->setIntValue(RFX_STATUS_KEY_DSBP_ENHANCEMENT_STATE,
                DSBP_ENHANCEMENT_END);
    }
    sp<RfxMclMessage> urc = RfxMclMessage::obtainUrc(
        RFX_MSG_UNSOL_DSBP_CHANGED_INDICATION, m_slot_id, RfxIntsData(&state, 1));
    responseToTelCore(urc);
}
