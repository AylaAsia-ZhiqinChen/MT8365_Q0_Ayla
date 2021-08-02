/*
 * Copyright (C) 2015 The Android Open Source Project
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

import android.graphics.Bitmap;


/*
 * BIP (Bearer Independent Protocol) is the mechanism for SIM card applications
 * to access data connection through the mobile device.
 *
 * SIM utilizes proactive commands (OPEN CHANNEL, CLOSE CHANNEL, SEND DATA and
 * RECEIVE DATA to control/read/write data for BIP. Refer to ETSI TS 102 223 for
 * the details of proactive commands procedures and their structures.
 */
public class BIPClientParams extends CommandParams {
    public TextMessage mTextMsg;
    public  boolean mHasAlphaId;

    BIPClientParams(CommandDetails cmdDet, TextMessage textMsg, boolean has_alpha_id) {
        super(cmdDet);
        mTextMsg = textMsg;
        mHasAlphaId = has_alpha_id;
    }

    @Override
    boolean setIcon(Bitmap icon) {
        if (icon != null && mTextMsg != null) {
            mTextMsg.icon = icon;
            return true;
        }
        return false;
    }
}

