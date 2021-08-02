/*
 *
 *  The original Work has been changed by NXP Semiconductors.
 *
 *  Copyright (C) 2013-2014 NXP Semiconductors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
/**
 * ****************************************************************************
 *
 * <p>The original Work has been changed by ST Microelectronics S.A.
 *
 * <p>Copyright (C) 2017 ST Microelectronics S.A.
 *
 * <p>Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of the License at
 *
 * <p>http://www.apache.org/licenses/LICENSE-2.0
 *
 * <p>Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * <p>****************************************************************************
 */
package com.st.android.nfc_extensions;

/** This class provides the constants ID types. */
public final class StConstants {
    /** UICC ID to be able to select it as the default Secure Element */
    public static final String UICC_ID = "SIM1";

    /** @hide */
    public static final int UICC_ID_TYPE = 2;

    /** UICC2 ID to be able to select it as the default Secure Element */
    public static final String UICC2_ID = "SIM2";

    /** @hide */
    public static final int UICC2_ID_TYPE = 4;

    /** eSE ID to be able to select it as the default Secure Element */
    public static final String ESE_ID = "eSE";

    /** @hide */
    public static final int ESE_ID_TYPE = 1;
    /** UICC ID to be able to select it as the default Secure Element */

    /** */
    public static final String HOST_ID = "DH";

    /** @hide */
    public static final int HOST_ID_TYPE = 0;

    public static final String ACTION_ROUTING_TABLE_FULL =
            "nfc.intent.action.AID_ROUTING_TABLE_FULL";

    public static final String PERMISSIONS_NFC = "android.permission.NFC";

    /**
     * Indicates the states of an APDU service. Service is enabled only when the commit to routing
     * table is successful
     */
    public static final int SERVICE_STATE_DISABLED = 0;

    public static final int SERVICE_STATE_ENABLED = 1;
    public static final int SERVICE_STATE_ENABLING = 2;
    public static final int SERVICE_STATE_DISABLING = 3;
}
