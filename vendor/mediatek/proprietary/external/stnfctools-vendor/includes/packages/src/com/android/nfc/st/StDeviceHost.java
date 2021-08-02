/*
 * Copyright (C) 2011 The Android Open Source Project
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

package com.android.nfc.st;

import com.android.nfc.DeviceHost;

public interface StDeviceHost extends DeviceHost {

    /**
     * Called at boot if NFC is disabled to give the device host an opportunity to check the
     * firmware version to see if it needs updating. Normally the firmware version is checked during
     * {@link #initialize()}, but the firmware may need to be updated after an OTA update.
     *
     * <p>
     *
     * <p>This is called from a thread that may block for long periods of time during the update
     * process.
     */
    public void forceRouting(int nfceeid);

    public void stopforceRouting();

    public void NfceeDiscover();

    boolean doSetMuteTech(boolean muteA, boolean muteB, boolean muteF, boolean commitNeeded);

    boolean setObserverMode(boolean enable);

    void enableStLog(boolean enabled);

    void setUserDefaultRoutesPref(
            int mifareRoute,
            int isoDepRoute,
            int felicaRoute,
            int abTechRoute,
            int scRoute,
            int aidRoute);

    boolean rotateRfParameters(boolean reset);
}
