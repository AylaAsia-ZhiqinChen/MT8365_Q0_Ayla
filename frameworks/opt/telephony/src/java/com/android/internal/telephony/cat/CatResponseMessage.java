/*
 * Copyright (C) 2007 The Android Open Source Project
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

package com.android.internal.telephony.cat;

import android.annotation.UnsupportedAppUsage;

public class CatResponseMessage {
        // MTK-START
        public CommandDetails mCmdDet = null;
        public ResultCode mResCode  = ResultCode.OK;
        public int mUsersMenuSelection = 0;
        public String mUsersInput  = null;
        public boolean mUsersYesNoSelection = false;
        public boolean mUsersConfirm = false;
        public boolean mIncludeAdditionalInfo = false;
        public int mAdditionalInfo = 0;
        public int mEventValue = -1;
        public byte[] mAddedInfo = null;
        // MTK-END
        public CatResponseMessage(CatCmdMessage cmdMsg) {
            mCmdDet = cmdMsg.mCmdDet;
        }

        public void setResultCode(ResultCode resCode) {
            mResCode = resCode;
        }

        public void setMenuSelection(int selection) {
            mUsersMenuSelection = selection;
        }

        public void setInput(String input) {
            mUsersInput = input;
        }

        @UnsupportedAppUsage
        public void setEventDownload(int event, byte[] addedInfo) {
            this.mEventValue = event;
            this.mAddedInfo = addedInfo;
        }

        public void setYesNo(boolean yesNo) {
            mUsersYesNoSelection = yesNo;
        }

        public void setConfirmation(boolean confirm) {
            mUsersConfirm = confirm;
        }

        public void setAdditionalInfo(int info) {
            mIncludeAdditionalInfo = true;
            mAdditionalInfo = info;
        }
        // MTK-START
        public CommandDetails getCmdDetails() {
        // MTK-END
            return mCmdDet;
        }
    }
