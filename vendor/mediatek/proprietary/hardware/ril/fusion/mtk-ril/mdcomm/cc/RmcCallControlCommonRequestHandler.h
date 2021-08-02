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

#ifndef __RMC_CALL_CONTROL_COMMON_REQUEST_HANDLER_H__
#define __RMC_CALL_CONTROL_COMMON_REQUEST_HANDLER_H__

#include "RmcCallControlBaseHandler.h"

class RmcCallControlImsRequestHandler;

class RmcCallControlCommonRequestHandler : public RmcCallControlBaseHandler {
    RFX_DECLARE_HANDLER_CLASS(RmcCallControlCommonRequestHandler);

    public:
        RmcCallControlCommonRequestHandler(int slot_id, int channel_id);
        virtual ~RmcCallControlCommonRequestHandler();

    protected:
        virtual void onHandleRequest(const sp<RfxMclMessage>& msg);
        virtual void onHandleEvent(const sp < RfxMclMessage > & msg);

        virtual void onHandleTimer();

        void requestGetCurrentCalls(const sp<RfxMclMessage>& msg);
        void requestDial(const sp<RfxMclMessage>& msg, bool isEcc, bool isVt, bool isImsDial);

        void copyString(char* destStr, char* srcStr, int bufSize);

        int clccsStateToRILState(int state, RIL_CallState *p_state);
        int clccStateToRILState(int state, RIL_CallState *p_state);
        void clearCnap();

        char cachedCnap[MAX_CNAP_LENGTH] = { 0 };
        char cachedClccName[MAX_GSMCALL_CONNECTIONS][MAX_CNAP_LENGTH] = {{0}};

    private:
        void requestAnswer(const sp<RfxMclMessage>& msg);
        void requestUdub(const sp<RfxMclMessage>& msg);
        void requestDtmf(const sp<RfxMclMessage>& msg);
        void requestDtmfStart(const sp<RfxMclMessage>& msg);
        void requestDtmfStop(const sp<RfxMclMessage>& msg);
        void requestSetMute(const sp<RfxMclMessage>& msg);
        void requestGetMute(const sp<RfxMclMessage>& msg);
        void requestSetTtyMode(const sp<RfxMclMessage>& msg);
        void requestQueryTtyMode(const sp<RfxMclMessage>& msg);
        void requestLastCallFailCause(const sp<RfxMclMessage>& msg);
        void requestSetCallIndication(const sp<RfxMclMessage>& msg);
        void requestLocalSetEccServiceCategory(const sp<RfxMclMessage>& msg);
        int callFromCLCCSLine(RfxAtLine *line, RIL_Call *p_call);
        int callFromCLCCLine(RfxAtLine *line, RIL_Call *p_call);
        void requestExitEmergencyCallbackMode(const sp<RfxMclMessage>& msg);
        void handleCnapUpdate(const sp<RfxMclMessage>& msg);
        void handleClearClccName(const sp<RfxMclMessage>& msg);

        /// C2K specific start
        void requestSetPreferredVoicePrivacyMode(const sp<RfxMclMessage>& msg);
        void requestQueryPreferredVoicePrivacyMode(const sp<RfxMclMessage>& msg);
        void requestFlash(const sp<RfxMclMessage>& msg);
        void requestBurstDtmf(const sp<RfxMclMessage>& msg);
        /// C2K specific end

        /// redial @{
        void requestEmergencyRedial(const sp<RfxMclMessage>& msg);
        void requestNotifyEmergencySession(const sp<RfxMclMessage>& msg, bool isStarted);
        /// @}

        void requestEccPreferredRat(const sp<RfxMclMessage>& msg);
        void requestLocalCurrentStatus(const sp<RfxMclMessage>& msg);


        bool isNumberIncludePause(char* number);
        bool isValidDialString(const char* dialString);

        char atLog[MAX_AT_RESPONSE] = { 0 };

        int bUseLocalCallFailCause = 0;
        int dialLastError = 0;
        RmcCallControlImsRequestHandler* mImsCCReqHdlr = NULL;

        void queryEmciSupport();
};

#endif
