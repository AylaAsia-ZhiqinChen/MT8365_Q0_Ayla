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

package com.st.android.nfc_extensions;

import com.st.android.nfc_dta_extensions.INfcAdapterStDtaExtensions;
import com.st.android.nfc_extensions.INfcWalletAdapter;

/**
  * {@hide}
   */
interface INfcAdapterStExtensions {

    byte[] getFirmwareVersion();
    byte[] getHWVersion();

    byte[] getSWVersion();

    int loopback();

    boolean getHceCapability() ;

    void setRfConfiguration(int modeBitmap, in byte[] techArray);
    int   getRfConfiguration(out byte[] techArray);

    void setRfBitmap(int modeBitmap);

    boolean getProprietaryConfigSettings(int SubSetID, int byteNb, int bitNb);
    void    setProprietaryConfigSettings(int SubSetID, int byteNb, int bitNb, boolean status);

    int getPipesList(int hostId, out byte[] list);
    void getPipeInfo(int hostId, int pipeId, out byte[] info);

    byte[] getATR();

    boolean connectEE(int ceeId);
    byte[] transceiveEE(int ceeId, in byte[] dataCmd);
    boolean disconnectEE(int ceeId);

    int connectGate(int host_id, int gate_id);
    byte[] transceive(int pipe_id, int hci_cmd, in byte[] dataIn);
    void disconnectGate(int pipe_id);

    int getAvailableHciHostList(out byte[] nfceeId, out byte[] conInfo);

    boolean getBitPropConfig(int configId, int byteNb, int bitNb);
    void setBitPropConfig(int configId, int byteNb, int bitNb, boolean status);

    void forceRouting(int nfceeId, int PowerState);
    void stopforceRouting();

    void setNciConfig(int paramId, in byte[] param);
    byte[] getNciConfig(int paramId);

    void sendPropSetConfig(int subSetId, int configId, in byte[] param);
    byte[] sendPropGetConfig(int subSetId, int configId);

    byte[] sendPropTestCmd(int subCode, in byte[] paramTx);

    byte[] getCustomerData();

    INfcWalletAdapter getNfcWalletAdapterInterface();

    INfcAdapterStDtaExtensions getNfcAdapterStDtaExtensionsInterface();
}
