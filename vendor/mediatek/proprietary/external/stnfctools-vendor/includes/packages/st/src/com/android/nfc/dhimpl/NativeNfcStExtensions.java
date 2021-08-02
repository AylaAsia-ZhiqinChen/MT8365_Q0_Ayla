/*
 *  Copyright (C) 2013 ST Microelectronics S.A.
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
import android.content.SharedPreferences;

/**
 * Native interface to the NFC ST Extensions functions
 *
 * <p>{@hide}
 */
public class NativeNfcStExtensions {

    private final Context mContext;

    SharedPreferences mPrefs;
    SharedPreferences.Editor mPrefsEditor;

    private final String TAG = "NativeNfcStExtensions";

    public NativeNfcStExtensions(Context context) {
        mContext = context;
        mPrefs = mContext.getSharedPreferences(StNativeNfcManager.PREF, Context.MODE_PRIVATE);
        mPrefsEditor = mPrefs.edit();
    }

    public native boolean isUiccConnected();

    /* NCI 2.0 - Begin */
    public native boolean iseSEConnected();

    public native boolean isSEConnected(int HostID);

    /* NCI 2.0 - End */
    public native byte[] getFirmwareVersion();

    public native byte[] getCRCConfiguration();

    public native byte[] getHWVersion();

    public native byte[] getSWVersion();

    public native int loopback();

    public native int getHceUserProp();

    public native void setRfConfiguration(int modeBitmap, byte[] techArray);

    public native int getRfConfiguration(byte[] techArray);

    public native void setRfBitmap(int modeBitmap);

    public native int updatePipesInfo();

    public native void setDefaultOffHostRoute(int route);

    public native int getDefaultOffHostRoute();

    /* NCI 2.0 - Begin */
    public native boolean getProprietaryConfigSettings(int SubSetID, int byteNb, int bitNb);

    public native void setProprietaryConfigSettings(
            int SubSetID, int byteNb, int bitNb, boolean status);
    /* NCI 2.0 - End */

    public native int getPipesList(int host_id, byte[] list);

    public native void getPipeInfo(int host_id, int pipe_id, byte[] info);

    public native byte[] getATR();

    public native boolean EnableSE(int se_id, boolean enable);

    public native boolean connectEE(int cee_id);

    public native byte[] transceiveEE(int cee_id, byte[] dataCmd);

    public native boolean disconnectEE(int cee_id);

    public native int connectGate(int gate_id, int host_id);

    public native byte[] transceive(int pipe_id, int hciCmd, byte[] dataCmd);

    public native void disconnectGate(int pipe_id);

    /* NCI 2.0 - Begin */
    public native void setNciConfig(int param_id, byte[] param, int length);

    public native byte[] getNciConfig(int param_id);

    public native int getAvailableHciHostList(byte[] nfceeId, byte[] conInfo);

    public native void sendPropSetConfig(int subSetId, int configId, byte[] param, int length);

    public native byte[] sendPropGetConfig(int subSetId, int configId);

    public native byte[] sendPropTestCmd(int subCode, byte[] paramTx, int lengthTx);

    public native byte[] getCustomerData();
    /* NCI 2.0 - End */
}
