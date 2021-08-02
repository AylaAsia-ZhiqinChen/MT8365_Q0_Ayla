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

public class SwVersion {
    String tag = "NfcSwVersion";
    private static final boolean DBG = true;

    private static final int HAL_IDX = 0x00;
    private static final int CORE_STACK_IDX = 0x01;
    private static final int JNI_IDX = 0x02;
    private static final int ANDROID_IDX = 0x03;
    private static final int FRAMEWORK_IDX = 0x04;
    private static final int TAG_IDX = 0x05;
    private String[] swVersions = new String[6];

    /**
     * This class contains information about the SW version of the NFC stack used by the ST21NFCB
     * chip.
     */
    /**
     * Constructor for the {@link SwVersion} object
     *
     * @param info This is an array of 5 bytes that contains some SW information.
     * @return
     */
    public SwVersion(byte[] data, String framework, String tag) {
        String version = new String(data);
        if (DBG) Log.i(tag, "constructor - " + version);

        int start = 0;
        int pos;

        for (int i = 0; i < swVersions.length - 2; i++) {
            pos = version.indexOf("+", start);
            if ((pos == -1) && (i == ANDROID_IDX)) {
                swVersions[i] = version.substring(start);
            } else {
                swVersions[i] = version.substring(start, pos);
            }
            start = pos + 1;
            if (DBG) Log.i(tag, "constructor - " + swVersions[i]);
        }

        if (DBG) Log.i(tag, "constructor - framework version = " + framework);
        if (DBG) Log.i(tag, "constructor - tag version = " + tag);

        this.swVersions[FRAMEWORK_IDX] = framework;
        this.swVersions[TAG_IDX] = tag;
    }

    /**
     * Returns the HAL version currently used.
     *
     * @return A String containing the HAL version name.
     */
    public String getHalVersion() {
        if (DBG) Log.i(tag, "getHalVersion() - " + swVersions[HAL_IDX]);
        return this.swVersions[HAL_IDX];
    }

    /**
     * Returns the core stack version currently used.
     *
     * @return A String containing the core stack version name.
     */
    public String getCoreStackVersion() {
        if (DBG) Log.i(tag, "getCoreStackVersion() - " + swVersions[CORE_STACK_IDX]);
        return this.swVersions[CORE_STACK_IDX];
    }

    /**
     * Returns the JNI version currently used.
     *
     * @return A String containing the JNI version name.
     */
    public String getJniVersion() {
        if (DBG) Log.i(tag, "getJniVersion() - " + swVersions[JNI_IDX]);
        return this.swVersions[JNI_IDX];
    }

    /**
     * Returns the Android version currently used.
     *
     * @return A String containing the Android version name.
     */
    public String getAndroidVersion() {
        if (DBG) Log.i(tag, "getAndroidVersion() - " + swVersions[ANDROID_IDX]);
        return this.swVersions[ANDROID_IDX];
    }

    /**
     * Returns the Framework (ST APIs) version currently used.
     *
     * @return A String containing the Framework (ST APIs) version name.
     */
    public String getFrameworkVersion() {
        if (DBG) Log.i(tag, "getFrameworkVersion() - " + swVersions[FRAMEWORK_IDX]);
        return this.swVersions[FRAMEWORK_IDX];
    }

    /**
     * Returns the Tag (NFC stack as tagged in codex) version currently used.
     *
     * @return A String containing the tag version name.
     */
    public String getTagVersion() {
        if (DBG) Log.i(tag, "getTagVersion() - " + swVersions[TAG_IDX]);
        return this.swVersions[TAG_IDX];
    }
}
