/*
 * Copyright (C) 2013 ST Microelectronics S.A.
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
 *
 * Provide extensions for the ST implementation of the NFC stack
 */

package com.st.android.nfc_extensions;

import android.nfc.NfcAdapter;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;
import com.st.android.nfc_dta_extensions.INfcAdapterStDtaExtensions;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

/** This class contains a set of APIs for the purpose of testing the ST21NFCD chip. */
public final class NfcAdapterStExtensions {

    private static final String TAG = "NfcAdapterStExtensions";

    public static final String SERVICE_NAME = "nfc.st_ext";

    /* NCI 2.0 - Begin */
    private static final String frameworkVersion = "Framework version 01.7.0.00";
    /* NCI 2.0 - End */

    private static final String tagVersion = "C.1.17";

    /**
     * {@link Intent} action to be declared by BroadcastReceiver.
     *
     * <p>This {@link Intent} is broadcasted every time a CE operated on a SE causes the ST21NFCB to
     * return some data to the device host on the connectivity gate (using HCI over NCI). The {@link
     * Intent} contains the identity of the SE on which the CE took place ( {@link
     * EXTRA_HOST_ID_FOR_EVT}).
     */
    public static final String ACTION_EVT_TRANSACTION_RX =
            "android.nfc.action.TRANSACTION_DETECTED";
    /**
     * The name of the meta-data element that contains the identity of the SE on which the CE
     * operation took place when the {@link Intent} {@link ACTION_EVT_TRANSACTION_RX} is
     * broadcasted.
     */
    public static final String EXTRA_HOST_ID_FOR_EVT = "android.nfc.extra.HOST_ID";

    /**
     * This constant is used by the {@link changeRfConfiguration}() and {@link getRfConfiguration}()
     * APIs to indicate that the content of the corresponding {@link NfcRfConfig} object is intended
     * for the R/W mode.
     */
    public static final String MODE_READER = "MODE_READER";
    /**
     * This constant is used by the {@link changeRfConfiguration}() and {@link getRfConfiguration}()
     * APIs to indicate that the content of the corresponding {@link NfcRfConfig} object is intended
     * for the CE mode.
     */
    public static final String MODE_CE = "MODE_CE";
    /**
     * This constant is used by the {@link changeRfConfiguration}() and {@link getRfConfiguration}()
     * APIs to indicate that the content of the corresponding {@link NfcRfConfig} object is intended
     * for the P2P poll mode.
     */
    public static final String MODE_P2P_POLL = "MODE_P2P_POLL";
    /**
     * This constant is used by the {@link changeRfConfiguration}() and {@link getRfConfiguration}()
     * APIs to indicate that the content of the corresponding {@link NfcRfConfig} object is intended
     * for the P2P listen mode.
     */
    public static final String MODE_P2P_LISTEN = "MODE_P2P_LISTEN";
    /**
     * This constant is used in the {@link NfcRfConfig} object to identify the 14443-A technology.
     */
    public static final String TECH_A = "TECH_A";
    /**
     * This constant is used in the {@link NfcRfConfig} object to identify the 14443-B technology.
     */
    public static final String TECH_B = "TECH_B";
    /**
     * This constant is used in the {@link NfcRfConfig} object to identify the type F technology.
     */
    public static final String TECH_F = "TECH_F";
    /**
     * This constant is used in the {@link NfcRfConfig} object to identify the ISO 15693 technology.
     */
    public static final String TECH_15693 = "TECH_15693";
    /**
     * This constant is used in the {@link NfcRfConfig} object to identify the type ACM active mode
     * (for P2P only).
     */
    public static final String TECH_ACTIVEMODE = "TECH_ACTIVE_MODE";

    /**
     * This constant is used in the {@link NfcRfConfig} object to identify the type F technology in
     * active mode (for P2P only).
     */
    public static final String TECH_KOVIO = "TECH_KOVIO";

    /**
     * {@link Intent} action to be declared by BroadcastReceiver.
     *
     * <p>This {@link Intent} is broadcasted every time a CE operated on a SE has used select AID
     * command. The {@link Intent} contains as extra the identity of the SE on which the CE took
     * place ( {@link EXTRA_HOST_ID_FOR_AID_SEL}) and which AID was selected ( {@link
     * EXTRA_AID_SELECTION}).
     */
    public static final String ACTION_AID_SELECTION = "android.nfc.action.AID_SELECTION";
    /**
     * The name of the meta-data element that contains the identity of the SE on which the CE
     * operation took place when the {@link Intent} {@link ACTION_AID_SELECTION} is broadcasted.
     */
    public static final String EXTRA_HOST_ID_FOR_AID_SEL = "android.nfc.extra.HOST_ID_AID_SEL";
    /**
     * The name of the meta-data element that contains the AID value used during CE when the {@link
     * Intent} {@link ACTION_AID_SELECTION} is broadcasted.
     */
    public static final String EXTRA_AID_SELECTION = "android.nfc.extra.AID_SLECTION";

    /**
     * {@link Intent} action to be declared by BroadcastReceiver.
     *
     * <p>This {@link Intent} is broadcasted every time a CE operated on a SE has been taken
     * following a technology routing decision. The {@link Intent} contains as extra the identity of
     * the SE on which the CE took place ( {@link EXTRA_HOST_ID_FOR_TECHPROTO_SEL}) and which was
     * the technology used ({@link EXTRA_TECHPROTO_SEL})
     */
    public static final String ACTION_TECHNOLOGY_ROUTING_DECISION =
            "android.nfc.action.TECHNOLOGY_ROUTING";
    /**
     * {@link Intent} action to be declared by BroadcastReceiver.
     *
     * <p>This {@link Intent} is broadcasted every time a CE operated on a SE has been taken
     * following a protocol routing decision. The {@link Intent} contains as extra the identity of
     * the SE on which the CE took place ( {@link EXTRA_HOST_ID_FOR_TECHPROTO_SEL}) and which was
     * the protocol used ({@link EXTRA_TECHPROTO_SEL})
     */
    public static final String ACTION_PROTOCOL_ROUTING_DECISION =
            "android.nfc.action.PROTCOL_ROUTING";
    /**
     * The name of the meta-data element that contains the identity of the SE on which the CE
     * operation took place when the {@link Intent} {@link ACTION_TECHPROTO_ROUTING_DECISION} is
     * broadcasted.
     */
    public static final String EXTRA_HOST_ID_FOR_TECHPROTO_SEL =
            "android.nfc.extra.HOST_ID_TECHPROTO_SEL";
    /**
     * The name of the meta-data element that contains the decision routing trigger used during CE
     * when the {@link Intent} {@link ACTION_TECHPROTO_ROUTING_DECISION} is broadcasted.
     */
    public static final String EXTRA_TECHPROTO_VALUE = "android.nfc.extra.TECHPROTO_VAL";

    // protected by NfcAdapterStExtensions.class, and final after first construction,
    // except for attemptDeadServiceRecovery() when NFC crashes - we accept a
    // best effort recovery
    private static INfcAdapterStExtensions sInterface = null;
    // contents protected by NfcAdapterExtras.class
    private static final HashMap<NfcAdapter, NfcAdapterStExtensions> sNfcStExtensions =
            new HashMap();

    /**
     * Constructor for the {@link NfcAdapterStExtensions}
     *
     * @param adapter a {@link NfcAdapter}, must not be null
     * @return
     */
    public NfcAdapterStExtensions() {
        sInterface = getNfcAdapterStExtensionsInterface();
    }

    /** NFC service dead - attempt best effort recovery */
    void attemptDeadServiceRecovery(Exception e) {
        Log.e(TAG, "NFC Adapter ST Extensions dead - attempting to recover");

        IBinder b = ServiceManager.getService(SERVICE_NAME);
        if (b == null) throw new RuntimeException("Cannot retrieve service :" + SERVICE_NAME);
        sInterface = INfcAdapterStExtensions.Stub.asInterface(b);
    }

    public static INfcAdapterStExtensions getNfcAdapterStExtensionsInterface() {
        if (sInterface == null) {
            IBinder b = ServiceManager.getService(SERVICE_NAME);
            if (b == null) throw new RuntimeException("Cannot retrieve service :" + SERVICE_NAME);
            sInterface = INfcAdapterStExtensions.Stub.asInterface(b);
        }
        return sInterface;
    }

    /**
     * Get the firmware version of the ST21NFCD chip
     *
     * @return An object of the type {@link FwVersion} that contains information about the FW
     *     version.
     */
    public FwVersion getFirmwareVersion() {
        byte[] result = null;
        try {
            result = sInterface.getFirmwareVersion();
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        FwVersion fwVersion = new FwVersion(result);
        return fwVersion;
    }

    /**
     * Get the HW version of the ST21NFCD chip
     *
     * @return An object of the type {@link HwInfo}.
     *     <p>This object contains information about the ST21NFCB HW version.
     */
    public HwInfo getHWVersion() {
        byte[] result = null;
        try {
            result = sInterface.getHWVersion();
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        HwInfo hwInfo = new HwInfo(result);
        return hwInfo;
    }

    /**
     * Get the SW versions of the NFC stack.
     *
     * @return An object of the type {@link SwVersion}.
     *     <p>This object contains information about the NFC stack used for the ST21NFCD chip.
     */
    public SwVersion getSWVersion() {
        byte[] result = null;
        try {
            result = sInterface.getSWVersion();
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
        if (result != null) {
            SwVersion swVersion = new SwVersion(result, frameworkVersion, tagVersion);

            return swVersion;
        }
        return null;
    }

    /**
     * This API performs a loopback on the HCI loopback gate over NCI:
     *
     * <p>240 fixed bytes are transmitted to the ST21NFCD chip on its HCI loopback gate. The
     * ST21NFCD shall return to the NFC stack the same amount of bytes and the same data.
     *
     * <p class="note">The rate at which the loopback is performed shall be fixed by the caller.
     *
     * @return 0 if an error has occurred during the loopback and 1 if everything is OK.
     */
    public int loopback() {
        int res = 0;
        try {
            res = sInterface.loopback();
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
        return res;
    }

    /**
     * Indicates if the NFC stack currently supports HCE.
     *
     * <p>
     *
     * <p class="note">If HCE is not supported, this may be because the HCE capability is not
     * supported the device (check of {@link FEATURE_NFC_HOST_CARD_EMULATION} feature) or because
     * the {@link setHceCapable()} API was previously called to disable the HCE.
     *
     * @return true if HCE is supported or enabled and false otherwise.
     */
    public boolean getHceCapability() {
        boolean res = false;
        try {
            res = sInterface.getHceCapability();
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
        return res;
    }

    /**
     * This API allows to build a custom RF_DISCOVER_CMD.
     *
     * <p>When called, this API causes the RF state to return to idle state. It then resets all the
     * previously programmed technologies and disables polling, listening and P2P. Then the
     * technologies and modes selected by the caller are programmed and a new RF_DISCOVER_CMD is
     * issued.
     *
     * <p class="note">To restore the configuration used in the libnfc-st.com file, the NFC service
     * can be disabled and re-enabled or the caller can reprogram it using this API.
     *
     * @param config is a Map that gets the NFC mode ({@link MODE_READER}, {@link MODE_CE}, {@link
     *     MODE_P2P_POLL}, {@link MODE_P2P_LISTEN}) as keys, and a {@link NfcRfCOnfig} object as
     *     values.
     *     <p>The {@link NfcRfCOnfig} object contains the RF configuration for corresponding mode.
     */
    public void changeRfConfiguration(Map<String, NfcRfConfig> config) {
        byte[] techArray = new byte[4];
        int modeBitmap = 0;
        NfcRfConfig mode = new NfcRfConfig();

        Log.i(TAG, "changeRfConfiguration()");

        // Loop on config content
        for (Map.Entry<String, NfcRfConfig> entry : config.entrySet()) {
            int idx = 0;

            if (entry.getKey() == MODE_READER) {
                Log.i(TAG, "Reader mode configuration");
                idx = 0;
            } else if (entry.getKey() == MODE_CE) {
                Log.i(TAG, "Listen mode configuration");
                idx = 1;
            } else if (entry.getKey() == MODE_P2P_POLL) {
                Log.i(TAG, "P2P poll mode configuration");
                idx = 3;
            } else if (entry.getKey() == MODE_P2P_LISTEN) {
                Log.i(TAG, "P2P listen mode configuration");
                idx = 2;
            }

            mode = entry.getValue();
            if (mode.enabled) {
                if (entry.getKey() == MODE_P2P_POLL) {
                    modeBitmap |= (0x1 << 2);
                } else {
                    modeBitmap |= (0x1 << idx);
                }

                Set<String> tech = mode.tech;
                Iterator<String> it = tech.iterator();

                while (it.hasNext()) {
                    String next = it.next();

                    if (next.equals(TECH_A)) techArray[idx] |= 0x1;
                    if ((entry.getKey() != MODE_P2P_POLL) && (entry.getKey() != MODE_P2P_LISTEN)) {
                        if (next.equals(TECH_B)) techArray[idx] |= 0x2;
                    }
                    if (entry.getKey() != MODE_CE) {
                        if (next.equals(TECH_F)) techArray[idx] |= 0x4;
                    }
                    if (entry.getKey() == MODE_READER) {
                        if (next.equals(TECH_15693)) techArray[idx] |= 0x8;
                    }
                    if (entry.getKey() == MODE_READER) {
                        if (next.equals(TECH_KOVIO)) techArray[idx] |= 0x20;
                    }
                    if ((entry.getKey() == MODE_P2P_POLL) || (entry.getKey() == MODE_P2P_LISTEN)) {
                        if (next.equals(TECH_ACTIVEMODE)) techArray[idx] |= 0x40;
                    }
                }
            }
        }

        Log.i(TAG, "changeRfConfiguration() - modeBitmap = " + String.valueOf(modeBitmap));
        Log.i(TAG, "changeRfConfiguration() - techArray[0] = " + String.valueOf(techArray[0]));
        Log.i(TAG, "changeRfConfiguration() - techArray[1] = " + String.valueOf(techArray[1]));
        Log.i(TAG, "changeRfConfiguration() - techArray[2] = " + String.valueOf(techArray[2]));

        try {
            sInterface.setRfConfiguration(modeBitmap, techArray);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
    }

    /**
     * This API allows to retrieve what was programmed in the last RF_DISCOVERY_CMD issued.
     *
     * <p>
     *
     * @return a Map that contains the NFC mode ({@link MODE_READER}, {@link MODE_CE}, {@link
     *     MODE_P2P_POLL}, {@link MODE_P2P_LISTEN}) as keys, and a {@link NfcRfCOnfig} object as
     *     values.
     *     <p>The {@link NfcRfCOnfig} object contains the RF configuration for corresponding mode.
     */
    public Map<String, NfcRfConfig> getRfConfiguration() {
        Log.i(TAG, "getRfConfiguration()");
        int modeBitmap = 0;
        byte[] techArray = new byte[4];
        Map<String, NfcRfConfig> rfConfig = new HashMap<String, NfcRfConfig>();

        try {
            modeBitmap = sInterface.getRfConfiguration(techArray);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        for (int i = 0; i < techArray.length; i++) {
            Log.i(TAG, "getRfConfiguration() - techArray[" + i + "] = " + techArray[i]);
        }

        NfcRfConfig modeConfigPoll = new NfcRfConfig();
        // Poll mode
        if ((modeBitmap & 0x1) == 1) // Poll mode enabled
        {
            Log.i(TAG, "getRfConfiguration() - Poll mode configuration");
            modeConfigPoll.setEnabled(true);
            if ((techArray[0] & 0x1) == 1) { // Tech A
                Log.i(TAG, "getRfConfiguration() - TECH_A used");
                modeConfigPoll.setTech(TECH_A);
            }
            if ((techArray[0] & 0x2) == 2) { // Tech B
                Log.i(TAG, "getRfConfiguration() - TECH_B used");
                modeConfigPoll.setTech(TECH_B);
            }
            if ((techArray[0] & 0x4) == 4) { // Tech F
                Log.i(TAG, "getRfConfiguration() - TECH_F used");
                modeConfigPoll.setTech(TECH_F);
            }
            if ((techArray[0] & 0x8) == 8) { // Tech 15693
                Log.i(TAG, "getRfConfiguration() - TECH_15693 used");
                modeConfigPoll.setTech(TECH_15693);
            }
            if ((techArray[0] & 0x20) == 0x20) { // Tech Kovio
                Log.i(TAG, "getRfConfiguration() - TECH_KOVIO used");
                modeConfigPoll.setTech(TECH_KOVIO);
            }
        } else {
            modeConfigPoll.setEnabled(false);
        }
        rfConfig.put(MODE_READER, modeConfigPoll);

        NfcRfConfig modeConfigP2pPoll = new NfcRfConfig();
        // P2P poll mode
        if ((modeBitmap & 0x4) == 4) // P2P mode enabled => process poll
        {
            Log.i(TAG, "getRfConfiguration() - P2P Poll mode configuration");
            modeConfigP2pPoll.setEnabled(true);
            if ((techArray[3] & 0x1) == 1) { // Tech A
                Log.i(TAG, "getRfConfiguration() - TECH_A used");
                modeConfigP2pPoll.setTech(TECH_A);
            }
            if ((techArray[3] & 0x4) == 4) { // Tech B
                Log.i(TAG, "getRfConfiguration() - TECH_F used");
                modeConfigP2pPoll.setTech(TECH_F);
            }
            if ((techArray[3] & 0x40) == 0x40) { // Tech A Active
                Log.i(TAG, "getRfConfiguration() - TECH_ACTIVE_MODE used");
                modeConfigP2pPoll.setTech(TECH_ACTIVEMODE);
            }
        } else {
            modeConfigP2pPoll.setEnabled(false);
        }
        rfConfig.put(MODE_P2P_POLL, modeConfigP2pPoll);

        // Listen mode
        NfcRfConfig modeConfigListen = new NfcRfConfig();
        if ((modeBitmap & 0x2) == 2) // Listen mode enabled
        {
            Log.i(TAG, "getRfConfiguration() - Listen mode configuration");
            modeConfigListen.setEnabled(true);
            if ((techArray[1] & 0x1) == 1) { // Tech A
                Log.i(TAG, "getRfConfiguration() - TECH_A used");
                modeConfigListen.setTech(TECH_A);
            }
            if ((techArray[1] & 0x2) == 2) { // Tech B
                Log.i(TAG, "getRfConfiguration() - TECH_B used");
                modeConfigListen.setTech(TECH_B);
            }
        } else {
            modeConfigListen.setEnabled(false);
        }
        rfConfig.put(MODE_CE, modeConfigListen);

        // P2P listen mode
        NfcRfConfig modeConfigP2pListen = new NfcRfConfig();
        if ((modeBitmap & 0x4) == 4) // P2P  mode enabled => process listen
        {
            Log.i(TAG, "getRfConfiguration() - P2P listen mode configuration");
            modeConfigP2pListen.setEnabled(true);
            if ((techArray[2] & 0x1) == 1) { // Tech A
                Log.i(TAG, "getRfConfiguration() - TECH_A used");
                modeConfigP2pListen.setTech(TECH_A);
            }
            if ((techArray[2] & 0x4) == 4) { // Tech B
                Log.i(TAG, "getRfConfiguration() - TECH_F used");
                modeConfigP2pListen.setTech(TECH_F);
            }
            if ((techArray[2] & 0x40) == 0x40) { // Tech A Active
                Log.i(TAG, "getRfConfiguration() - TECH_ACTIVE_MODE used");
                modeConfigP2pListen.setTech(TECH_ACTIVEMODE);
            }
        } else {
            modeConfigP2pListen.setEnabled(false);
        }
        rfConfig.put(MODE_P2P_LISTEN, modeConfigP2pListen);

        return rfConfig;
    }

    /**
     * This API sets the current Tag Detector status in the CLF configuration parameters.
     *
     * <p>
     *
     * @param status True if the Tag Detector shall be enabled and false otherwise.
     */
    public void setTagDetectorStatus(boolean status) {
        Log.i(TAG, "setTagDetectorStatus()");
        int byteNb, bitNb, regAdd;

        // Tag Detector mapping changed with new FW version
        byte[] result = null;
        try {
            result = sInterface.getFirmwareVersion();
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        if ((result[0] == 0x01) && (result[1] == 0x00)) {
            // FW 1.0.xxxx
            byteNb = 0;
            bitNb = 0;
            regAdd = 0x11;
        } else {
            byteNb = 0;
            bitNb = 4;
            regAdd = 0x01;
        }

        status = (!status);

        try {
            sInterface.setProprietaryConfigSettings(regAdd, byteNb, bitNb, status);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
    }

    /**
     * This API retrieves the current Tag detector status from the CLF configuration parameters.
     *
     * <p>
     *
     * @return true if the tag detector is enabled and false otherwise.
     */
    public boolean getTagDetectorStatus() {
        Log.i(TAG, "getTagDetectorStatus()");
        int byteNb, bitNb, regAdd;

        // Tag Detector mapping changed with new FW version
        byte[] result = null;
        try {
            result = sInterface.getFirmwareVersion();
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        if ((result[0] == 0x01) && (result[1] == 0x00)) {
            // FW 1.0.xxxx
            byteNb = 0;
            bitNb = 0;
            regAdd = 0x11;
        } else {
            byteNb = 0;
            bitNb = 4;
            regAdd = 0x01;
        }

        boolean status = false;

        try {
            status = sInterface.getProprietaryConfigSettings(regAdd, byteNb, bitNb);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        return (!status);
    }

    /**
     * This API retrieves the list and status of pipes for a given host.
     *
     * <p>
     *
     * @param hostId Identity of the host to investigate
     * @return a structure {@link PipesInfo} containing the list and status of the pipes attached to
     *     the host.
     */
    public PipesInfo getPipesInfo(int hostId) {
        Log.i(TAG, "getPipesInfo() - for host " + hostId);
        int nbPipes = 0;
        byte[] list = new byte[10];
        byte[] info = new byte[5];

        try {
            nbPipes = sInterface.getPipesList(hostId, list);
            Log.i(TAG, "getPipesInfo() - Found " + nbPipes + " for host " + hostId);
            PipesInfo retrievedInfo = new PipesInfo(nbPipes);

            for (int i = 0; i < nbPipes; i++) {
                Log.i(TAG, "getPipesInfo() - retrieving info for pipe " + list[i]);
                sInterface.getPipeInfo(hostId, list[i], info);
                retrievedInfo.setPipeInfo(list[i], info);
            }

            return retrievedInfo;
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
        // Log.i(TAG, "getPipesInfo() - Found " + nbPipes + "for host " + hostId);

        return null;
    }

    /**
     * This API retrieves the ATR of the eSE.
     *
     * <p>
     *
     * @return an array of bytes containing the ATR.
     */
    public byte[] getATR() {
        Log.i(TAG, "getATR()");

        try {
            return sInterface.getATR();
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        return null;
    }

    /**
     * This constant is used by the {@link convertCeeIdToInt}(), {@link transceiveEE}() and {@link
     * connectEE}() APIs to identify the T4T CEE.
     */
    public static final String T4T_CEE = "T4T_CEE";

    private int convertCeeIdToInt(String ceeId) {
        int cee_id = 0xFF;
        if (ceeId.contentEquals(T4T_CEE)) {
            cee_id = 0x10;
        }

        return cee_id;
    }

    /**
     * This API connects the specified CEE.
     *
     * <p>
     *
     * @return true if successful, false if not
     */
    public boolean connectEE(String ceeId) {
        Log.i(TAG, "connectEE(" + ceeId + ")");
        try {
            int cee_id = convertCeeIdToInt(ceeId);

            if (cee_id != 0xFF) {
                return sInterface.connectEE(cee_id);
            } else {
                return false;
            }
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
        return false;
    }

    /**
     * This API transmits and receive data on the specified CEE.
     *
     * <p>
     *
     * @return the received data
     */
    public byte[] transceiveEE(String ceeId, byte[] dataCmd) {
        Log.i(TAG, "transceiveEE(" + ceeId + ")");

        int cee_id = convertCeeIdToInt(ceeId);
        try {
            return sInterface.transceiveEE(cee_id, dataCmd);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        return null;
    }

    /**
     * This API disconnects the specified CEE.
     *
     * <p>
     *
     * @return true if successful, false otherwise
     */
    public boolean disconnectEE(String ceeId) {
        Log.i(TAG, "disconnectEE(" + ceeId + ")");
        int cee_id = convertCeeIdToInt(ceeId);
        try {
            return sInterface.disconnectEE(cee_id);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
        return false;
    }

    /**
     * This constant is used by the {@link connectGate}(), {@link transceive}() and {@link
     * disconnectGate}() APIs to identify the UICC host.
     */
    public static final String UICC_HOST = "UICC_HOST";

    /**
     * This constant is used by the {@link connectGate}(), {@link transceive}() and {@link
     * disconnectGate}() APIs to identify the eSE host.
     */
    public static final String ESE_HOST = "ESE_HOST";

    private int convertHostIdToInt(String hostId) {
        int host_id = 0xFF;

        if (hostId.contentEquals(UICC_HOST)) {
            host_id = 0x2;
        } else if (hostId.contentEquals(ESE_HOST)) {
            host_id = 0xC0;
        }

        return host_id;
    }

    /**
     * This API connects the DH and the specified host on HCI using the specified gate.
     *
     * <p>
     *
     * @return the identity of the created pipe if successful, 0xFF otherwise
     */
    public int connectGate(String hostId, int gateId) {
        Log.i(TAG, "connectGate(" + hostId + " - " + String.valueOf(gateId) + ")");
        int host_id = convertHostIdToInt(hostId);
        try {
            return sInterface.connectGate(host_id, gateId);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        return 0xFF;
    }

    /**
     * This API transmits an HCI command on a specified pipe.
     *
     * <p>
     *
     * @return the response data.
     */
    public byte[] transceive(int pipeId, int hciCmd, byte[] dataIn) {
        Log.i(TAG, "transceive(" + pipeId + " - HCI cmd " + hciCmd + ")");
        try {
            return sInterface.transceive(pipeId, hciCmd, dataIn);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        return null;
    }

    /**
     * This API disconnects the specified pipe.
     *
     * <p>
     *
     * @return the response data.
     */
    public void disconnectGate(int pipeId) {
        Log.i(TAG, "disconnectGate(" + pipeId + ")");
        try {
            sInterface.disconnectGate(pipeId);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
    }

    public static final String STATUS_OK = "STATUS_OK";
    public static final String STATUS_KO = "STATUS_KO";
    public static final String RF_TYPE_A = "RF_TYPE_A";
    public static final String RF_TYPE_B = "RF_TYPE_B";
    public static final String CEE_ON = "CEE_ON";
    public static final String CEE_OFF = "CEE_OFF";

    public CeeStatus getCeeSetup() {
        CeeStatus currentStatus = new CeeStatus();
        boolean status = false;
        int byteNb = 1;
        int bitNb = 4;
        Log.i(TAG, "getCeeSetup()");

        try {
            status = sInterface.getProprietaryConfigSettings(0x01, byteNb, bitNb);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        if (status == false) {
            Log.i(TAG, "getCeeSetup() - T4T CEE support KO");
            // No ISO CEE setup
            currentStatus.setT4tStatus(STATUS_KO);
        } else {
            Log.i(TAG, "getCeeSetup() - T4T CEE support OK");
            currentStatus.setT4tStatus(STATUS_OK);
        }

        return currentStatus;
    }

    public void setupCEE(String ceeId, String state) {
        Log.i(TAG, "setupCEE(" + ceeId + ", " + state + ")");

        int byteNb = 1;
        int bitNb = 4;
        boolean status = false;

        if (state.contentEquals(CEE_ON)) {
            status = true;
        }

        try {
            sInterface.setProprietaryConfigSettings(0x01, byteNb, bitNb, status);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
        try {

            // DM_UPDATE_AID
            if (ceeId.contentEquals(T4T_CEE) && state.contentEquals(CEE_ON)) {
                Log.i(TAG, "setupCEE() - Update AID table for T4T (entries 1 & 2) CEE");
                byte[] dmUpdateAid1 = {
                    0x01,
                    0x09,
                    (byte) 0xA0,
                    0x00,
                    0x00,
                    0x00,
                    (byte) 0x96,
                    (byte) 0xF0,
                    0x02,
                    0x01,
                    0x01,
                    0x00,
                    0x00,
                    0x00,
                    0x00,
                    0x00,
                    0x00,
                    0x00,
                    (byte) 0x9F,
                    (byte) 0xE1
                };

                byte[] rsltdmUpdateAid = sInterface.transceive(0x02, 0x20, dmUpdateAid1);

                byte[] dmUpdateAid2 = {
                    0x02,
                    0x07,
                    (byte) 0xD2,
                    0x76,
                    0x00,
                    0x00,
                    (byte) 0x85,
                    0x01,
                    0x01,
                    0x00,
                    0x00,
                    0x00,
                    0x00,
                    0x00,
                    0x00,
                    0x00,
                    0x00,
                    0x00,
                    (byte) 0x9F,
                    (byte) 0xE2
                };

                rsltdmUpdateAid = sInterface.transceive(0x02, 0x20, dmUpdateAid2);
            }
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
    }

    public static final String HCI_HOST_UICC1 = "SIM1";
    public static final String HCI_HOST_UICC2 = "SIM2";
    public static final String HCI_HOST_ESE = "ESE";
    public static final String HCI_HOST_EUICCSE = "eUICC-SE";
    public static final String HCI_HOST_DHSE = "DHSE";
    public static final String HCI_HOST_ACTIVE = "ACTIVE";
    public static final String HCI_HOST_INACTIVE = "INACTIVE";
    public static final String HCI_HOST_UNRESPONSIVE = "UNRESPONSIVE";

    public Map<String, String> getAvailableHciHostList() {
        Map<String, String> result = new HashMap<String, String>();
        byte[] nfceeId = new byte[3];
        byte[] conInfo = new byte[3];
        int nbHost = 0;
        int i;

        Log.i(TAG, "getAvailableHciHostList()");

        try {
            nbHost = sInterface.getAvailableHciHostList(nfceeId, conInfo);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        for (i = 0; i < nbHost; i++) {
            Log.i(
                    TAG,
                    "getHostList() - nfceeId["
                            + i
                            + "] = "
                            + nfceeId[i]
                            + ", conInfo["
                            + i
                            + "] = "
                            + conInfo[i]);
        }

        String nfcee;
        String status;

        for (i = 0; i < nbHost; i++) {
            nfcee = "";
            status = "";

            switch (nfceeId[i]) {
                case (byte) 0x81:
                    nfcee = HCI_HOST_UICC1;
                    break;

                case (byte) 0x82:
                    nfcee = HCI_HOST_ESE;
                    break;

                case (byte) 0x83:
                case (byte) 0x85:
                    nfcee = HCI_HOST_UICC2;
                    break;

                case (byte) 0x84:
                    nfcee = HCI_HOST_DHSE;
                    break;
                case (byte) 0x86:
                    nfcee = HCI_HOST_EUICCSE;
                    break;
            }

            switch (conInfo[i]) {
                case 0x00: // Active
                    status = HCI_HOST_ACTIVE;
                    break;
                case 0x01: // Inactive
                    status = HCI_HOST_INACTIVE;
                    break;
                case 0x02: // Unresponsive
                    status = HCI_HOST_UNRESPONSIVE;
                    break;
            }

            result.put(nfcee, status);
        }

        return result;
    }

    /**
     * This API gets status of the 2-UICC mode.
     *
     * <p>
     *
     * <p>
     *
     * @return status True if the dual uicc mode is enabled and false otherwise.
     */
    public boolean getDualSimFeature() {
        boolean status = false;

        Log.i(TAG, "getDualSimFeature()");
        try {
            status = sInterface.getBitPropConfig(0x02, 0x00, 3);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        return status;
    }

    /**
     * This API sets the NFCC in 2 UICC mode.
     *
     * <p>
     *
     * <p>
     *
     * @param status True if the dual uicc mode shall be enabled and false otherwise.
     */
    public void setDualSimFeature(boolean status) {
        Log.i(TAG, "setDualSimFeature(" + status + ")");

        try {
            sInterface.setBitPropConfig(0x02, 0x00, 3, status);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
    }

    /**
     * This API allows to force all the routings to a given NFCEE id
     *
     * <p>
     */
    public void forceRouting(int nfceeId, int PowerState) {
        try {
            sInterface.forceRouting(nfceeId, 0);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
    }

    /**
     * This API allows to stop forcing all the routings to a given NFCEE id. The routing set by
     * SET_LISTEN_MODE_ROUTING is being applied.
     *
     * <p>
     */
    public void stopforceRouting() {
        try {
            sInterface.stopforceRouting();
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
    }

    public byte getNfceeHwConfig() {
        byte conf = 0;
        boolean status = false;

        Log.i(TAG, "getNfceeHwConfig()");

        try {
            /* bit 3, 4, 6 and 7 are exclusive
             * all to 0 : UICC + eSE (conf = 0)
             * bit 3 : Dual UICC (conf = 1)
             * bit 4 : Dual UICC + eSE (conf = 2)
             * bit 6 : Dual UICC + InSE (conf = 4)
             * bit 7 : UICC + SPI-SE (conf = 3) */
            status = sInterface.getBitPropConfig(0x02, 0x00, 3);
            if (status) conf = 1;
            else {
                status = sInterface.getBitPropConfig(0x02, 0x00, 4);
                if (status) conf = 2;
                else {
                    status = sInterface.getBitPropConfig(0x02, 0x00, 7);
                    if (status) conf = 3;
                    else {
                        status = sInterface.getBitPropConfig(0x02, 0x00, 6);
                        if (status) conf = 4;
                    }
                }
            }
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        return conf;
    }

    public void setNfceeHwConfig(byte conf) {
        Log.i(TAG, "setNfceeHwConfig(" + conf + ")");

        try {
            switch (conf) {
                case 1:
                    sInterface.setBitPropConfig(0x02, 0x00, 3, true);
                    sInterface.setBitPropConfig(0x02, 0x00, 4, false);
                    sInterface.setBitPropConfig(0x02, 0x00, 6, false);
                    sInterface.setBitPropConfig(0x02, 0x00, 7, false);
                    break;
                case 2:
                    sInterface.setBitPropConfig(0x02, 0x00, 3, false);
                    sInterface.setBitPropConfig(0x02, 0x00, 4, true);
                    sInterface.setBitPropConfig(0x02, 0x00, 6, false);
                    sInterface.setBitPropConfig(0x02, 0x00, 7, false);
                    break;
                case 3:
                    sInterface.setBitPropConfig(0x02, 0x00, 3, false);
                    sInterface.setBitPropConfig(0x02, 0x00, 4, false);
                    sInterface.setBitPropConfig(0x02, 0x00, 6, false);
                    sInterface.setBitPropConfig(0x02, 0x00, 7, true);
                    break;
                case 4:
                    sInterface.setBitPropConfig(0x02, 0x00, 3, false);
                    sInterface.setBitPropConfig(0x02, 0x00, 4, false);
                    sInterface.setBitPropConfig(0x02, 0x00, 6, true);
                    sInterface.setBitPropConfig(0x02, 0x00, 7, false);
                    break;
                default:
                    sInterface.setBitPropConfig(0x02, 0x00, 3, false);
                    sInterface.setBitPropConfig(0x02, 0x00, 4, false);
                    sInterface.setBitPropConfig(0x02, 0x00, 6, false);
                    sInterface.setBitPropConfig(0x02, 0x00, 7, false);
                    break;
            }
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
    }

    /**
     * This API allows to set Nci params
     *
     * <p>
     */
    public void setNciConfig(int paramId, byte[] param) {
        Log.i(TAG, "setNciParam(" + paramId + ")");

        try {
            sInterface.setNciConfig(paramId, param);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
    }

    /**
     * This API gets the value of a NCI parameter.
     *
     * <p>
     *
     * <p>
     *
     * @param paramId NCI parameter id.
     * @return param NCI parameter value.
     */
    public byte[] getNciConfig(int paramId) {
        Log.i(TAG, "getNciParam(" + paramId + ")");

        try {
            return sInterface.getNciConfig(paramId);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        return null;
    }

    /**
     * This API sets a ST proprietary configuration in the NFCC.
     *
     * <p>
     *
     * <p>
     *
     * @param subSetId The identifier of the configuration sub-set.
     * @param configId The identifier of the specific configuration parameter
     * @param param The value of the specific configuration parameter
     */
    public void sendPropSetConfig(int subSetId, int configId, byte[] param) {
        Log.i(TAG, "sendPropSetConfig(" + subSetId + ")");

        try {
            sInterface.sendPropSetConfig(subSetId, configId, param);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
    }

    /**
     * This API gets a ST proprietary configuration in the NFCC.
     *
     * <p>
     *
     * <p>
     *
     * @param subSetId The identifier of the configuration sub-set.
     * @param configId The identifier of the specific configuration parameter
     * @param param The value of the specific configuration parameter
     */
    public byte[] sendPropGetConfig(int subSetId, int configId) {
        Log.i(TAG, "sendPropGetConfig(" + subSetId + ")");

        try {
            return sInterface.sendPropGetConfig(subSetId, configId);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
        return null;
    }
    /**
     * This API allows to send Proprietary test commands.
     *
     * <p>
     */
    public byte[] sendPropTestCmd(int subCode, byte[] paramTx) {
        Log.i(TAG, "sendPropTestCmd(" + subCode + ")");

        try {
            return sInterface.sendPropTestCmd(subCode, paramTx);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        return null;
    }

    public byte[] getCustomerData() {
        Log.i(TAG, "getCustomerData()");
        try {
            return sInterface.getCustomerData();
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        return null;
    }

    /**
     * This API sets the low power mode in the UICC configuration register.
     *
     * <p>
     *
     * <p>
     *
     * @param status True if the Low power shall be enabled and false otherwise.
     */
    public void setUiccLowPowerStatus(boolean status) {
        Log.i(TAG, "setUiccLowPowerStatus()");
        int byteNb, bitNb, regAdd;

        byteNb = 0;
        bitNb = 3;
        regAdd = 0x0A;

        status = (!status);

        try {
            sInterface.setProprietaryConfigSettings(regAdd, byteNb, bitNb, status);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }
    }

    /**
     * This API retrieves the current low power mode in the UICC configuration register.
     *
     * <p>
     *
     * @return true if the Low power is enabled and false otherwise.
     */
    public boolean getUiccLowPowerStatus() {
        Log.i(TAG, "getUiccLowPowerStatus()");
        int byteNb, bitNb, regAdd;

        byteNb = 0;
        bitNb = 3;
        regAdd = 0x0A;

        boolean status = false;

        try {
            status = sInterface.getProprietaryConfigSettings(regAdd, byteNb, bitNb);
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        return (!status);
    }

    public INfcAdapterStDtaExtensions getNfcAdapterStDtaExtensionsInterface() {
        Log.i(TAG, "getNfcAdapterStDtaExtensionsInterface()");
        try {
            return sInterface.getNfcAdapterStDtaExtensionsInterface();
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        return null;
    }

    public INfcWalletAdapter getNfcWalletAdapterInterface() {
        Log.i(TAG, "getNfcWalletAdapterInterface()");
        try {
            return sInterface.getNfcWalletAdapterInterface();
        } catch (RemoteException e) {
            attemptDeadServiceRecovery(e);
        }

        return null;
    }
}
