/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2016. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */
package com.st.android.nfc_extensions;

import android.content.Context;
import android.nfc.NfcAdapter;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class NfcSettingsAdapter {
    static INfcSettingsAdapter sService;

    private static final String TAG = "NfcSettingsAdapter";

    /**
     * The NfcAdapter object for each application context. There is a 1-1 relationship between
     * application context and NfcAdapter object.
     */
    static HashMap<Context, NfcSettingsAdapter> sNfcSettingsAdapters = new HashMap();

    final Context mContext;

    public static final String SERVICE_SETTINGS_NAME = "nfc_settings";

    /**
     * The mode flag to control each NFC mode. MODE_READER is used to switch Tag read/write mode
     *
     * @hide
     * @internal
     */
    public static final int MODE_READER = 1;

    /**
     * The mode flag to control each NFC mode. MODE_HCE is used to switch card emulation mode
     *
     * @hide
     * @internal
     */
    public static final int MODE_HCE = 2;

    /**
     * The mode flag to control each NFC mode. MODE_P2P is used to switch P2P mode
     *
     * @hide
     * @internal
     */
    public static final int MODE_P2P = 4;

    /**
     * To disable each NFC mode.
     *
     * @hide
     * @internal
     */
    public static final int FLAG_OFF = 0;

    /**
     * To enable each NFC mode.
     *
     * @hide
     * @internal
     */
    public static final int FLAG_ON = 1;

    /* Below values must be aligned with SecureElementSelector code */
    public static final String SE_SIM1 = "SIM1";
    public static final String SE_SIM2 = "SIM2";
    public static final String SE_ESE1 = "eSE";

    public static final String SE_STATE_ACTIVATED = "Active";
    public static final String SE_STATE_AVAILABLE = "Available";
    public static final String SE_STATE_NOT_AVAILABLE = "N/A";

    public NfcSettingsAdapter(Context context) {
        mContext = context;
        sService = getServiceInterface();
    }

    /**
     * Helper to get the default NFC Settings Adapter.
     *
     * @param context the calling application's context
     * @return the default NFC settings adapter, or null if no NFC settings adapter exists
     */
    public static NfcSettingsAdapter getDefaultAdapter(Context context) {
        if (NfcAdapter.getDefaultAdapter(context) == null) {
            Log.d(TAG, "getDefaultAdapter = null");
            return null;
        }

        NfcSettingsAdapter adapter = sNfcSettingsAdapters.get(context);
        if (adapter == null) {
            adapter = new NfcSettingsAdapter(context);
            sNfcSettingsAdapters.put(context, adapter);
        }

        if (sService == null) {
            sService = getServiceInterface();
            Log.d(TAG, "sService = " + sService);
        }

        Log.d(TAG, "adapter = " + adapter);
        return adapter;
    }

    private static INfcSettingsAdapter getServiceInterface() {
        /* get a handle to NFC service */
        IBinder b = ServiceManager.getService(SERVICE_SETTINGS_NAME);
        Log.d(TAG, "b = " + b);
        if (b == null) {
            return null;
        }
        return INfcSettingsAdapter.Stub.asInterface(b);
    }

    /**
     * Query specific NFC mode.
     *
     * @param one of the NFC mode, candidates are MODE_READER, MODE_P2P or MODE_HCE.
     * @return FLAG_ON or FLAG_OFF, -1 for failure
     * @hide
     * @internal
     */
    public int getModeFlag(int mode) {
        try {
            if (sService == null) {
                Log.e(TAG, "getModeFlag() - sService = null");
                return -1;
            }
            int flag = sService.getModeFlag(mode);

            Log.d(TAG, "getModeFlag()" + settingModeToString(mode, flag));

            return flag;
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "getModeFlag() - e = " + e.toString());
            return -1;
        }
    }

    /**
     * Control specific NFC mode.
     *
     * @param one of the NFC mode, candidates are MODE_READER, MODE_P2P or MODE_HCE.
     * @param FLAG_ON or FLAG_OFF
     * @hide
     * @internal
     */
    public void setModeFlag(int mode, int flag) {
        try {
            if (sService == null) {
                Log.e(TAG, "setModeFlag() - sService = null");
                return;
            }
            // Log.d(TAG, "sService.setModeFlag(mode)");
            Log.d(TAG, "setModeFlag()" + settingModeToString(mode, flag));
            sService.setModeFlag(mode, flag);
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "setModeFlag() - e = " + e.toString());
        }
    }

    /**
     * GSMA feature: is there an overflow of the routing table?
     *
     * @hide
     * @internal
     */
    public boolean isRoutingTableOverflow() {
        try {
            if (sService == null) {
                Log.e(TAG, "isRoutingTableOverflow() - sService = null");
                return false;
            }
            Log.d(TAG, "sService.isRoutingTableOverflow()");
            return sService.isRoutingTableOverflow();
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "isRoutingTableOverflow() - e = " + e.toString());
            return false;
        }
    }

    /**
     * GSMA feature: Should overflow menu be shown to user? Yes if GSMA mode and there is at least 1
     * service not enabled.
     *
     * @hide
     * @internal
     */
    public boolean isShowOverflowMenu() {
        try {
            if (sService == null) {
                Log.e(TAG, "isShowOverflowMenu() - sService = null");
                return false;
            }
            Log.d(TAG, "sService.isShowOverflowMenu()");
            return sService.isShowOverflowMenu();
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "isShowOverflowMenu() - e = " + e.toString());
            return false;
        }
    }

    /**
     * GSMA feature: Retrieve map of services with AIDs registered in the category "OTHER", and for
     * each one its current state.
     *
     * @hide
     * @internal
     */
    public List<ServiceEntry> getServiceEntryList(int userHandle) {
        try {
            if (sService == null) {
                Log.e(TAG, "getServiceEntryList() - sService = null");
                return null;
            }
            Log.d(TAG, "sService.getServiceEntryList()");
            return sService.getServiceEntryList(userHandle);
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "getServiceEntryList() - e = " + e.toString());
            return null;
        }
    }

    /**
     * GSMA feature: Test if a proposal for enabled/disabled services can fit in the AID routing
     * table of the NFC controller.
     *
     * @hide
     * @internal
     */
    public boolean testServiceEntryList(List<ServiceEntry> proposal) {
        try {
            if (sService == null) {
                Log.e(TAG, "testServiceEntryList() - sService = null");
                return false;
            }
            Log.d(TAG, "sService.testServiceEntryList()");
            return sService.testServiceEntryList(proposal);
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "testServiceEntryList() - e = " + e.toString());
            return false;
        }
    }

    /**
     * GSMA feature: Commit a new state of enabled/disabled services. The operation fails silently
     * in case of error.
     *
     * @hide
     * @internal
     */
    public void commitServiceEntryList(List<ServiceEntry> proposal) {
        try {
            if (sService == null) {
                Log.e(TAG, "commitServiceEntryList() - sService = null");
                return;
            }
            Log.d(TAG, "sService.commitServiceEntryList()");
            sService.commitServiceEntryList(proposal);
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "commitServiceEntryList() - e = " + e.toString());
            return;
        }
    }

    /**
     * Indicates if an UICC is connected to the ST21NFCD chip.
     *
     * @return true if an UICC is connected, false if not.
     */
    public boolean isUiccConnected() {
        boolean result = false;
        try {
            result = sService.isUiccConnected();
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "isUiccConnected() - e = " + e.toString());
        }

        Log.d(TAG, "isUiccConnected() - " + result);

        return result;
    }

    /**
     * Indicates if an eSE is connected to the ST21NFCD chip.
     *
     * @return true if an eSE is connected, false if not.
     */
    public boolean iseSEConnected() {
        boolean result = false;
        try {
            result = sService.iseSEConnected();
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "iseSEConnected() - e = " + e.toString());
        }

        Log.d(TAG, "iseSEConnected() - " + result);

        return result;
    }

    /**
     * Indicates if an SE given it HostID is connected to the ST21NFCD chip.
     *
     * @return true if an SE is connected, false if not.
     */
    public boolean isSEConnected(int HostID) {
        boolean result = false;
        try {
            result = sService.isSEConnected(HostID);
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "isSEConnected() - e = " + e.toString());
        }

        Log.d(TAG, "isSEConnected(" + HostID + ") - " + result);

        return result;
    }

    /**
     * This API activate or deactivate the given Secure Element defined by se_id.
     *
     * <p>
     *
     * @return true if successful
     */
    public boolean EnableSE(String se_id, boolean enable) {
        Log.i(TAG, "EnableSE(" + se_id + ", " + enable + ")");
        boolean status = false;

        try {
            return sService.EnableSE(se_id, enable);
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "EnableSE() - e = " + e.toString());
        }

        return status;
    }

    /* Get the current state of SWP elements:
    Each item is a pair NAME:STATUS.
    NAME and STATUS are constants NfcSettingsAdapter.SE_*
    NAME is one of: SIM1, SIM2, eSE.
    STATUS is one of: Active, Available, N/A.
    */
    public List<String> getSecureElementsStatus() {
        Log.i(TAG, "getSecureElementsStatus()");

        try {
            return sService.getSecureElementsStatus();
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "getSecureElementsStatus() e = " + e.toString());
        }

        return null;
    }

    /* Callback for UI updates */
    public void registerNfcSettingsCallback(INfcSettingsCallback cb) {
        Log.i(TAG, "registerNfcSettingsCallback()");

        try {
            sService.registerNfcSettingsCallback(cb);
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "registerNfcSettingsCallback() e = " + e.toString());
        }
    }

    public void unregisterNfcSettingsCallback(INfcSettingsCallback cb) {
        Log.i(TAG, "unregisterNfcSettingsCallback()");

        try {
            sService.unregisterNfcSettingsCallback(cb);
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "unregisterNfcSettingsCallback() e = " + e.toString());
        }
    }

    /**
     * GSMA feature: Retrieve map of services with AIDs registered in the category "OTHER", and for
     * each one its current state.
     *
     * @hide
     * @internal
     */
    public List<ServiceEntry> getNonAidBasedServiceEntryList(int userHandle) {
        try {
            if (sService == null) {
                Log.e(TAG, "getNonAidBasedServiceEntryList() - sService = null");
                return null;
            }
            Log.d(TAG, "sService.getNonAidBasedServiceEntryList()");
            return sService.getNonAidBasedServiceEntryList(userHandle);
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "getNonAidBasedServiceEntryList() - e = " + e.toString());
            return null;
        }
    }

    /**
     * GSMA feature: Commit a new state of enabled/disabled services. The operation fails silently
     * in case of error.
     *
     * @hide
     * @internal
     */
    public void commitNonAidBasedServiceEntryList(List<ServiceEntry> proposal) {
        try {
            if (sService == null) {
                Log.e(TAG, "commitNonAidBasedServiceEntryList() - sService = null");
                return;
            }
            Log.d(TAG, "sService.commitNonAidBasedServiceEntryList()");
            sService.commitNonAidBasedServiceEntryList(proposal);
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "commitNonAidBasedServiceEntryList() - e = " + e.toString());
            return;
        }
    }

    String settingModeToString(int mode, int flag) {
        String valueString = "";

        switch (mode) {
            case MODE_READER:
                valueString = "MODE_READER ";
                break;
            case MODE_P2P:
                valueString = "MODE_P2P ";
                break;
            case MODE_HCE:
                valueString = "MODE_HCE ";
                break;
            default:
                valueString = "Unknown mode ";
                break;
        }

        if (flag == FLAG_OFF) {
            valueString += "FLAG_OFF";
        } else if (flag == FLAG_ON) {
            valueString += "FLAG_ON";
        } else {
            valueString += "unknown flag value";
        }

        return valueString;
    }

    public static final String DEFAULT_AID_ROUTE = "default_aid_route";
    public static final String DEFAULT_MIFARE_ROUTE = "default_mifare_route";
    public static final String DEFAULT_ISO_DEP_ROUTE = "default_iso_dep_route";
    public static final String DEFAULT_FELICA_ROUTE = "default_felica_route";
    public static final String DEFAULT_AB_TECH_ROUTE = "default_ab_tech_route";
    public static final String DEFAULT_SC_ROUTE = "default_sc_route";

    public static final String UICC_ROUTE = "UICC";
    public static final String ESE_ROUTE = "eSE";
    public static final String HCE_ROUTE = "HCE";
    public static final String DEFAULT_ROUTE = "Default";

    /**
     * Set listen mode routing table configuration for Default Route. routeLoc is parameter which
     * fetch the text from UI and compare *
     *
     * <p>Requires {@link android.Manifest.permission#NFC} permission.
     *
     * @throws IOException If a failure occurred during Default Route Route set.
     */
    public void DefaultRouteSet(String routeLoc) throws IOException {
        Log.i(TAG, "DefaultRouteSet() - route: " + routeLoc);

        HashMap<String, String> entry = new HashMap<String, String>();
        entry.put(DEFAULT_AID_ROUTE, routeLoc);
        setUserDefaultRoutes(entry);
    }

    /**
     * Set listen mode routing table configuration for Default Route. routeLoc is parameter which
     * fetch the text from UI and compare *
     *
     * <p>Requires {@link android.Manifest.permission#NFC} permission.
     *
     * @throws IOException If a failure occurred during Default Route Route set.
     */
    public void setUserDefaultRoutes(Map<String, String> routeList) throws IOException {

        List<DefaultRouteEntry> defaultRouteList = new ArrayList<DefaultRouteEntry>();

        for (Map.Entry<String, String> entry : routeList.entrySet()) {
            String routeKey = entry.getKey();
            String routeValue = entry.getValue();

            Log.d(TAG, "setUserDefaultRoutes() - " + routeKey + ": " + routeValue);

            if ((DEFAULT_AID_ROUTE.contentEquals(routeKey) == false)
                    && (DEFAULT_MIFARE_ROUTE.contentEquals(routeKey) == false)
                    && (DEFAULT_ISO_DEP_ROUTE.contentEquals(routeKey) == false)
                    && (DEFAULT_FELICA_ROUTE.contentEquals(routeKey) == false)
                    && (DEFAULT_AB_TECH_ROUTE.contentEquals(routeKey) == false)
                    && (DEFAULT_SC_ROUTE.contentEquals(routeKey) == false)) {

                Log.e(TAG, "setUserDefaultRoutes() - " + routeKey + " does not exists");
                throw new IOException(routeKey + " does not exists");
            }

            if ((UICC_ROUTE.contentEquals(routeValue) == false)
                    && (ESE_ROUTE.contentEquals(routeValue) == false)
                    && (HCE_ROUTE.contentEquals(routeValue) == false)
                    && (DEFAULT_ROUTE.contentEquals(routeValue)) == false) {

                Log.e(TAG, "setUserDefaultRoutes() - " + routeValue + " does not exists");
                throw new IOException(routeValue + " does not exists");
            }

            DefaultRouteEntry defaultRouteEntry = new DefaultRouteEntry(routeKey, routeValue);
            defaultRouteList.add(defaultRouteEntry);
        }

        try {
            sService.setDefaultUserRoutes(defaultRouteList);
        } catch (RemoteException e) {
            Log.e(TAG, "setDefaultUserRoutes failed", e);
            throw new IOException("setDefaultUserRoutes failed");
        }
    }

    /**
     * Set listen mode routing table configuration for Default Route. routeLoc is parameter which
     * fetch the text from UI and compare *
     *
     * <p>Requires {@link android.Manifest.permission#NFC} permission.
     *
     * @throws IOException If a failure occurred during Default Route Route set.
     */
    public Map<String, String> getUserDefaultRoutes() throws IOException {
        Map<String, String> userRoutes = new HashMap<String, String>();

        List<DefaultRouteEntry> list = new ArrayList<DefaultRouteEntry>();

        try {
            list = sService.getDefaultUserRoutes();
        } catch (RemoteException e) {
            Log.e(TAG, "getUserDefaultRoutes failed", e);
            throw new IOException("getUserDefaultRoutes failed");
        }

        for (DefaultRouteEntry entry : list) {
            Log.d(
                    TAG,
                    "getUserDefaultRoutes() - "
                            + entry.getRouteName()
                            + ": "
                            + entry.getRouteLoc());
            userRoutes.put(entry.getRouteName(), entry.getRouteLoc());
        }

        return userRoutes;
    }

    /**
     * Set listen mode routing table configuration for Default Route. routeLoc is parameter which
     * fetch the text from UI and compare *
     *
     * <p>Requires {@link android.Manifest.permission#NFC} permission.
     *
     * @throws IOException If a failure occurred during Default Route Route set.
     */
    public Map<String, String> getEffectiveDefaultRoutes() throws IOException {
        Map<String, String> userRoutes = new HashMap<String, String>();

        List<DefaultRouteEntry> list = new ArrayList<DefaultRouteEntry>();

        try {
            list = sService.getEffectiveRoutes();
        } catch (RemoteException e) {
            Log.e(TAG, "getEffectiveDefaultRoutes failed", e);
            throw new IOException("getEffectiveDefaultRoutes failed");
        }

        for (DefaultRouteEntry entry : list) {
            Log.d(
                    TAG,
                    "getEffectiveDefaultRoutes() - "
                            + entry.getRouteName()
                            + ": "
                            + entry.getRouteLoc());
            userRoutes.put(entry.getRouteName(), entry.getRouteLoc());
        }

        return userRoutes;
    }

    /**
     * Retrieve how many bytes are still availabe to add AID entries in the listen mode routing
     * table.
     *
     * <p>Each entry has an overhead of 4 bytes per AID. Entries for the same route as the default
     * route don't consume space. The available space can change if the default route changes.
     *
     * <p>In case of overflow, this method returns 0.
     *
     * <p>In case of problem, returns -1
     *
     * @return
     */
    public int getAvailableSpaceForAid() {
        try {
            if (sService == null) {
                Log.e(TAG, "getAvailableSpaceForAid() - sService = null");
                return -1;
            }
            Log.d(TAG, "sService.getAvailableSpaceForAid()");
            return sService.getAvailableSpaceForAid();
        } catch (RemoteException e) {
            // attemptDeadServiceRecovery(e);
            Log.e(TAG, "getAvailableSpaceForAid() - e = " + e.toString());
            return -1;
        }
    }
}
