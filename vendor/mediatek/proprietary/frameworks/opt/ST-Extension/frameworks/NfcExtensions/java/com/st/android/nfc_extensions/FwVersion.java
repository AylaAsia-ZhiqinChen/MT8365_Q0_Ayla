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

public class FwVersion {
    String tag = "NfcFwVersion";
    private static final boolean DBG = true;

    private static final int FW_IDX = 2;
    private static final int ROM_IDX = 1;
    private int fmVersionMinor;
    private int fmVersionMajor;
    private int fwRevisionMinor;
    private int fwRevisionMajor;
    private String info;

    /** This class contains information about the FW version of the ST21NFCB chip. */

    /**
     * Constructor for the {@link FwVersion} object
     *
     * @param info This is an array of 3 bytes returned by the FW that contains some FW information.
     * @return
     */
    public FwVersion(byte[] info) {
        this.fwRevisionMajor = info[0];
        this.fwRevisionMinor = info[1];
        this.fmVersionMajor = info[2];
        this.fmVersionMinor = info[3];
        // Log.d(tag, "fwRevisionMinor - " + this.fwRevisionMinor + " "+ "fwRevisionMajor " + this
        // .fwRevisionMajor );

        this.info =
                String.format("%02X", info[0] & 0xFF)
                        + "."
                        + String.format("%02X", info[1] & 0xFF)
                        + "."
                        + String.format("%02X", info[2] & 0xFF)
                        + String.format("%02X", info[3] & 0xFF);

        if (DBG) Log.i(tag, "Contructor - " + this.info);

        // Log.d(tag, "Contructor - " + result + " " + this.fwRevision);
    }

    /**
     * Returns the number of the FW used by the current ST21NFCB chip.
     *
     * @return A String containing the FW number.
     */
    public String getInfo() {
        if (DBG) Log.i(tag, "getInfo()" + this.info);
        return this.info;
    }
}
