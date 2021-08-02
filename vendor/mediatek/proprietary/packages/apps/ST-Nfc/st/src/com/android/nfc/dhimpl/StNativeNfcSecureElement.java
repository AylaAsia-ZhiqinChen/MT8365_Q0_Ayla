/*
 * Copyright (C) 2010 The Android Open Source Project
 * Copyright (C) 2014 ST Microelectronics S.A.
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

package com.android.nfc.dhimpl;

import android.content.Context;
import android.content.SharedPreferences;
import android.util.Log;

/**
 * Native interface to the NFC Secure Element functions
 *
 * <p>{@hide}
 */
public class StNativeNfcSecureElement {

    static final String PREF_SE_WIRED = "se_wired";
    private static final String TAG = "StNativeNfcSecureElement";

    private final Context mContext;

    SharedPreferences mPrefs;
    SharedPreferences.Editor mPrefsEditor;

    public StNativeNfcSecureElement(Context context) {
        Log.d(TAG, "Constructor() ");
        mContext = context;

        mPrefs = mContext.getSharedPreferences(StNativeNfcManager.PREF, Context.MODE_PRIVATE);
        mPrefsEditor = mPrefs.edit();
    }

    private native int doNativeOpenSecureElementConnection();

    public int doOpenSecureElementConnection() {
        Log.d(TAG, "doOpenSecureElementConnection() ");
        mPrefsEditor.putBoolean(PREF_SE_WIRED, true);
        mPrefsEditor.apply();

        return doNativeOpenSecureElementConnection();
    }

    private native boolean doNativeDisconnectSecureElementConnection(int handle);

    public boolean doDisconnect(int handle) {
        mPrefsEditor.putBoolean(PREF_SE_WIRED, false);
        mPrefsEditor.apply();

        return doNativeDisconnectSecureElementConnection(handle);
    }

    public native byte[] doTransceive(int handle, byte[] data);

    public native int[] doGetTechList(int handle);

    public native byte[] doGetUid(int handle);
}
