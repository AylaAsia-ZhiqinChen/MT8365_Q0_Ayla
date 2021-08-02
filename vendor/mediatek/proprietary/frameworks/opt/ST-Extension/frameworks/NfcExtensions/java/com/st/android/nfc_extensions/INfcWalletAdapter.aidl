/*
 *  Copyright (C) 2019 ST Microelectronics S.A.
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

import com.st.android.nfc_extensions.INfcWalletLogCallback;

/**
 * @hide
 */
interface INfcWalletAdapter
{
    /* Force to keep eSE SWP interface active.
      -- use for operations in applets that require HCI services (connectivity
      gate) when accessed over SPI. This has no effect if APDU gate is opened */
    boolean keepEseSwpActive(boolean enable);

    /* NFC firmware logs support */
    boolean registerStLogCallback(INfcWalletLogCallback cb);
    boolean unregisterStLogCallback();

    /* Observer mode support (do not reply any message in listen mode) */
    boolean setObserverMode(boolean enable);

    /* Interfaces for controlling the behavior of the NFC solution for specific
     * applets requirements, controlled by a wallet generally.
     *
     * These methods are only applied when the eSE is active.
     */
    /* Do not answer to readers in specific technologies */
    boolean setMuteTech(boolean muteA, boolean muteB, boolean muteF);

    /* Change the RF parameter set to use for initial response.
     * Set of parameters are rotated when the parameter is false,
     * and reset to default if the parameter is true.
     * The value is not persistent, i.e. after NFC off / on the default
     * parameter set is used. */
    boolean rotateRfParameters(boolean reset);
}
