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


package com.st.android.nfc_dta_extensions;

/**
  * {@hide}
   */
interface INfcAdapterStDtaExtensions {

    boolean initialize();
    boolean deinitialize();

    void setPatternNb(int nb);
    void setCrVersion(byte ver);
    void setConnectionDevicesLimit(byte cdlA, byte cdlB, byte cdlF, byte cdlV);
    void setListenNfcaUidMode(byte mode);
    void setT4atNfcdepPrio(byte prio);
    void setFsdFscExtension(boolean ext);
    void setLlcpMode(int miux_mode);
    void setSnepMode(byte role, byte server_type, byte request_type, byte data_type, boolean disc_incorrect_len);

    int enableDiscovery(byte con_poll, byte con_listen_dep, byte con_listen_t4tp, boolean con_listen_t3tp, boolean con_listen_acm,
                        byte con_bitr_f, byte con_bitr_acm);
    boolean disableDiscovery();

}
