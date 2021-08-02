/*
 * Copyright (C) 2019 ST Microelectronics S.A.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at:
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

package com.st.android.nfc_extensions;

import android.os.RemoteException;
import android.util.Log;

public final class NfcWalletAdapter {
    private static final String TAG = "NfcWalletAdapter";

    private static INfcWalletAdapter sInterface = null;

    /**
     * Constructor for the {@link NfcWalletAdapter}
     *
     * @param intf a {@link INfcWalletAdapter}, must not be null
     * @return
     */
    public NfcWalletAdapter(INfcWalletAdapter intf) {
        sInterface = intf;
    }

    /** NFC service dead - attempt best effort recovery */
    void attemptDeadServiceRecovery(Exception e) {
        Log.e(TAG, "NFC Wallet Adapter ST Extensions dead - recover by close / open, TODO");
    }

    public boolean keepEseSwpActive(boolean enable) {
        boolean result = false;
        try {
            result = sInterface.keepEseSwpActive(enable);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
        return result;
    }

    public boolean setMuteTech(boolean muteA, boolean muteB, boolean muteF) {
        boolean result = false;
        try {
            result = sInterface.setMuteTech(muteA, muteB, muteF);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
        return result;
    }

    public boolean setObserverMode(boolean enable) {
        boolean result = false;
        try {
            result = sInterface.setObserverMode(enable);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
        return result;
    }

    public boolean registerStLogCallback(INfcWalletLogCallback cb) {
        boolean result = false;
        try {
            result = sInterface.registerStLogCallback(cb);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
        return result;
    }

    public boolean unregisterStLogCallback() {
        boolean result = false;
        try {
            result = sInterface.unregisterStLogCallback();
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
        return result;
    }

    public boolean rotateRfParameters(boolean reset) {
        boolean result = false;
        try {
            result = sInterface.rotateRfParameters(reset);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
        return result;
    }
}
