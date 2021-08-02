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

public class HwInfo {
    String tag = "NfcHwInfo";
    private static final boolean DBG = true;

    private static final int CHIP_ID_IDX = 0x00;
    private static final int SILICON_IDX = 0x01;
    private String chipId;
    private String siliconRevNb;

    /** This class contains information about the HW version of the ST21NFCB chip. */

    /**
     * Constructor for the {@link FwVersion} object
     *
     * @param info This is an array of 3 bytes returned by the FW that contains some HW information.
     * @return
     */
    public HwInfo(byte[] result) {
        switch (result[CHIP_ID_IDX]) {
            case 5:
                this.chipId = "ST54J";
                break;
            case 4:
                this.chipId = "ST21NFCD";
                break;
            case 3:
                this.chipId = "ST21NFCC";
                break;
            case 2:
                this.chipId = "ST21NFCB";
                break;

            case 1:
                this.chipId = "ST21NFCA";
                break;

            default:
                this.chipId = "Unknown Chip Id";
                break;
        }
        if (DBG) Log.i(tag, "constructor - " + result[CHIP_ID_IDX] + " " + this.chipId);

        this.siliconRevNb = "Rev Nb " + Integer.toString(result[SILICON_IDX]);
        if (DBG) Log.i(tag, "constructor - " + result[SILICON_IDX] + " " + this.siliconRevNb);
    }

    /**
     * Returns the identity of the current ST NFC chip used.
     *
     * @return A String containing the ST NFC chip identity.
     */
    public String getChipId() {
        if (DBG) Log.i(tag, "getChipId() - " + this.chipId);
        return this.chipId;
    }

    /**
     * Returns information about the ST21NFCB silicon version.
     *
     * @return A String the name of the ST21NFCB silicon version.
     */
    public String getSiliconRev() {
        if (DBG) Log.i(tag, "getSiliconRev() - " + this.siliconRevNb);
        return this.siliconRevNb;
    }
}
