/*
 *  Copyright (C) 2018 ST Microelectronics S.A.
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

package com.android.nfc.dhimpl;

import android.content.Context;
import android.util.Log;

/**
 * Native interface to the NFC ST DTA Extensions functions
 *
 * <p>{@hide}
 */
public class NativeNfcStDtaExtensions {

    private static final String TAG = "NativeNfcStDtaExtensions";
    private final Context mContext;

    static {
        try {
            Log.d(
                    TAG,
                    "libnfc_st_dta_jni load from path: "
                            + System.getProperty("java.library.path")
                            + "...");
            System.loadLibrary("nfc_st_dta_jni");

            Log.d(TAG, "ST Dta load successful!!!");
        } catch (UnsatisfiedLinkError ule) {
            /** UnsatisfiedLinkError */
            Log.d(TAG, "ST Dta load failure!!!");
        }
    }

    public NativeNfcStDtaExtensions(Context context) {

        Log.d(TAG, "NativeNfcStDtaExtensions");

        mContext = context;
    }

    public native boolean initialize();

    public native boolean deinitialize();

    public native void setPatternNb(int nb);

    public native void setCrVersion(byte ver);

    public native void setConnectionDevicesLimit(byte cdlA, byte cdlB, byte cdlF, byte cdlV);

    public native void setListenNfcaUidMode(byte mode);

    public native void setT4atNfcdepPrio(byte prio);

    public native void setFsdFscExtension(boolean ext);

    public native void setLlcpMode(int miux_mode);

    public native void setSnepMode(
            byte role,
            byte server_type,
            byte request_type,
            byte data_type,
            boolean disc_incorrect_len);

    public native int enableDiscovery(
            byte con_poll,
            byte con_listen_dep,
            byte con_listen_t4tp,
            boolean con_listen_t3tp,
            boolean con_listen_acm,
            byte con_bitr_f,
            byte con_bitr_acm);

    public native boolean disableDiscovery();
}
