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
import java.util.HashSet;
import java.util.Set;

/**
 * This class is used by the {@link changeRfConfiguration}() and {@link getRfConfiguration}()
 * proprietary APIs defined in the {@link NfcAdapterStExtensions} class.
 *
 * <p>It contains information about the RF configuration to be applied: is the mode enabled or not,
 * and if so, which technologies must be programmed.
 *
 * <p>It also provides a set of method to to set or retrieve some information.
 */
public class NfcRfConfig {

    boolean enabled;
    Set<String> tech;
    String tag = "NfcRfConfig";

    public NfcRfConfig() {
        Log.i(tag, "Contructor");
        this.enabled = true;
        this.tech = new HashSet<String>();
    }

    public void setEnabled(boolean state) {
        Log.i(tag, "setEnabled()");
        this.enabled = state;
    }

    public boolean getEnabled() {
        Log.i(tag, "getEnabled()");
        return this.enabled;
    }

    public void setTech(String techno) {
        Log.i(tag, "setTech(" + techno + ")");
        if ((tech != null) && (techno != null)) {
            this.tech.add(techno);
        } else {
            Log.i(tag, "setTech() - tech field or techno input is null!!!!");
        }
    }

    public void removeTech(String techno) {
        Log.i(tag, "removeTech(" + techno + ")");
        this.tech.remove(techno);
    }

    public Set<String> getTechSet() {
        Log.i(tag, "getTechSet()");
        return this.tech;
    }
}
