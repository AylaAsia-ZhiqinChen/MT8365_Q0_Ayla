/*
 *  Copyright (C) 2015 ST Microelectronics S.A.
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

import java.util.ArrayList;
import java.util.List;

public class CeeStatus {
    String tag = "CeeStatus";
    private static final boolean DBG = true;

    private String ceeSupport, t4tStatus;
    private List<String> t4tRfTypes;

    public CeeStatus() {

        t4tRfTypes = new ArrayList<String>();
    }

    public String getCeeSupport() {
        return this.ceeSupport;
    }

    public void setCeeSupport(String status) {
        this.ceeSupport = status;
    }

    public String getT4tStatus() {
        return this.t4tStatus;
    }

    public void setT4tStatus(String status) {
        this.t4tStatus = status;
    }

    public List<String> getT4tTfTypes() {
        return this.t4tRfTypes;
    }

    public void setT4tTfTypes(String rfType) {
        this.t4tRfTypes.add(rfType);
    }
}
