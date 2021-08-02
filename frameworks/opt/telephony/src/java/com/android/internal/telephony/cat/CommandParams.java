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
import android.graphics.Bitmap;

/**
 * Container class for proactive command parameters.
 *
 */
// MTK-START
public class CommandParams {
    @UnsupportedAppUsage
    public CommandDetails mCmdDet;
    // Variable to track if an optional icon load has failed.
    public boolean mLoadIconFailed = false;

    @UnsupportedAppUsage
    public CommandParams(CommandDetails cmdDet) {
        mCmdDet = cmdDet;
    }

    @UnsupportedAppUsage
    public AppInterface.CommandType getCommandType() {
        return AppInterface.CommandType.fromInt(mCmdDet.typeOfCommand);
    }
   // MTK-END

    boolean setIcon(Bitmap icon) { return true; }

    @Override
    public String toString() {
        return mCmdDet.toString();
    }
}

class LanguageParams extends CommandParams {
    String mLanguage;

    LanguageParams(CommandDetails cmdDet, String lang) {
        super(cmdDet);
        mLanguage = lang;
    }
}
