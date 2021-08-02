/*
 *  Copyright (C) 2014 ST Microelectronics S.A.
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
 *  Provide extensions for the ST implementation of the NFC stack
 */

package com.st.android.nfc_extensions;

import android.util.Log;

public class PatchData {
    String tag = "NfcPatchData";
    private static final boolean DBG = true;

    private String patchNb;
    private boolean isDebug;

    /** This class contains information about the FW patch version of the ST21NFCB chip. */

    /**
     * Constructor for the {@link PatchData} object
     *
     * @param info This is an array of 8 bytes returned by the FW that contains patch information.
     *     <p class="note">In case the FW version is not patched, the array will contain only 0
     *     <p>and the {@link PatchData} object will be void.
     * @return
     */
    public PatchData(byte[] info) {
        this.isDebug = false;
        this.patchNb = "";
        for (int data : info) {
            if (DBG) Log.i(tag, "constructor - " + String.format("%02X", (0xFF & data)));
            if (data != 0) {
                // this.patchNb += (" " + Integer.toHexString(data));
                this.patchNb += (" " + String.format("%02X", (0xFF & data)));
                if (data == (-1)) this.isDebug = true;
            }
        }

        if (DBG) Log.i(tag, "constructor - " + this.patchNb);
    }

    /**
     * Returns the number of the FW patch used by the current ST21NFCB chip.
     *
     * @return A String containing the FW patch number(s).
     *     <p class="note">In case the FW version is not patched, the String object returned will be
     *     empty.
     */
    public String getPatchNb() {
        if (DBG) Log.i(tag, "getPatchNb() - " + this.patchNb);
        return this.patchNb;
    }

    /**
     * Indicates if the FW patch currently used by the current ST21NFCB chip is a debug version or
     * not.
     *
     * @return A boolean indicating if the current FW pathc is a debug version (true) or not
     *     (false).
     *     <p class="note">In case the FW version is not patched, this value shall not be taken into
     *     account.
     */
    public boolean getIsDebug() {
        if (DBG) Log.i(tag, "getIsDebug() - " + this.isDebug);
        return this.isDebug;
    }
}
