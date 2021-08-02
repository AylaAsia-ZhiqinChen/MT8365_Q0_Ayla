/*
 * Copyright (C) 2014 MediaTek Inc.
 * Modification based on code covered by the mentioned copyright
 * and/or permission notice(s).
*/

package com.mediatek.server.wifi;

import android.annotation.NonNull;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.wifi.supplicant.V1_0.IfaceType;
import android.hardware.wifi.supplicant.V1_0.ISupplicant;
import android.hardware.wifi.supplicant.V1_0.SupplicantStatus;
import android.hardware.wifi.supplicant.V1_0.SupplicantStatusCode;
import android.net.wifi.ScanResult;
import android.net.wifi.ScanResult.InformationElement;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiConfiguration.KeyMgmt;
import android.net.wifi.WifiEnterpriseConfig;
import android.net.wifi.WifiManager;
import android.os.HidlSupport.Mutable;
import android.os.Process;
import android.os.RemoteException;
import android.security.KeyStore;
import android.util.Log;
import android.util.MutableBoolean;

import com.android.server.wifi.SupplicantStaIfaceHal;
import com.android.server.wifi.SupplicantStaNetworkHal;
import com.android.server.wifi.WifiInjector;
import com.android.server.wifi.WifiNative;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.BitSet;
import java.util.NoSuchElementException;

import vendor.mediatek.hardware.wifi.supplicant.V2_0.ISupplicantIface;
import vendor.mediatek.hardware.wifi.supplicant.V2_0.ISupplicantNetwork;
import vendor.mediatek.hardware.wifi.supplicant.V2_0.ISupplicantStaIface;
import vendor.mediatek.hardware.wifi.supplicant.V2_0.ISupplicantStaNetwork;

public class MtkWapi {
    private static final String TAG = "MtkWapi";
    private static final int WAPI_VERSION = 1;
    private static final int WAPI_AUTH_KEY_MGMT_WAI = 0x01721400;
    private static final int WAPI_AUTH_KEY_MGMT_PSK = 0x02721400;

    /* Definition of WAPI information element ID */
    public static final int EID_WAPI = 68;

    /* Singleton MtkWapi */
    private static MtkWapi sMtkWapi = null;

    /* This flag indicates latest aliases we got from KeyStore, and used it
           to determine need to update or not*/
    public static String[] mWapiCertSelCache;

    /* Use this flag to on/off WAPI feature in fwk*/
    public static boolean mIsSystemSupportWapi = false;

    /* Use this flag to check whether we have inquired system support WAPI */
    public static boolean mIsCheckedSupport = false;
    private static ISupplicantStaIface mMtkiface;

    /**
     * Parse WAPI element
     */
    public static String parseWapiElement(InformationElement ie) {
        ByteBuffer buf = ByteBuffer.wrap(ie.bytes).order(ByteOrder.LITTLE_ENDIAN);
        String TAG = "InformationElementUtil.WAPI";
        Log.d(TAG, "parseWapiElement start");

        try {
            // version
            if (buf.getShort() != WAPI_VERSION) {
                // incorrect version
                Log.e(TAG, "incorrect WAPI version");
                return null;
            }

            // count
            int count = buf.getShort();
            if (count != 1) {
                Log.e(TAG, "WAPI IE invalid AKM count: " + count);
            }

            // found the WAPI IE, hence start building the capability string
            String security = "[WAPI";

            // keyMgmt
            int keyMgmt = buf.getInt();

            // judge keyMgmt is WAPI-PSK or WAPI-CERT
            if (keyMgmt == WAPI_AUTH_KEY_MGMT_WAI) {
                security += "-CERT";
            } else if (keyMgmt == WAPI_AUTH_KEY_MGMT_PSK) {
                security += "-PSK";
            }
            // we parsed what we want at this point
            security += "]";
            return security;
        } catch (BufferUnderflowException e) {
            Log.e("IE_Capabilities", "Couldn't parse WAPI element, buffer underflow");
            return null;
        }
    }

    /**
     * Check whether need to update WAPI certificate selection (aliases) to supplicant
     */
    public static boolean updateWapiCertSelList(WifiConfiguration config) {
        /// M: List all wapi aliases in keystore
        String[] aliases = KeyStore.getInstance().list("WAPI_CACERT_", Process.WIFI_UID);
        Arrays.sort(aliases);
        StringBuilder sortedAliases = new StringBuilder();
        for (String alias : aliases) {
            sortedAliases.append(alias).append(";");
        }
        if (isWapiCertSelListChanged(aliases) && !setWapiCertAliasList(sortedAliases.toString())) {
            Log.e(TAG, "failed to set alias list: " + sortedAliases.toString());
            return false;
        }
        return true;
    }

    /**
     * Compare newWapiCertSel with mWapiCertSelCache to determine whether need to update
     * wapiCertSel to supplicant
     */
    public static boolean isWapiCertSelListChanged(String[] newWapiCertSel) {
        if (mWapiCertSelCache == null || !Arrays.equals(mWapiCertSelCache, newWapiCertSel)) {
            mWapiCertSelCache = newWapiCertSel;
            return true;
        }
        return false;
    }

    /**
     * Check whether configuration changes, and might need to update to supplicant
     */
    public static boolean hasWapiConfigChanged(WifiConfiguration config,
            WifiConfiguration config1) {
        if (!mIsSystemSupportWapi) {
            return false;
        }
        if (!(isConfigForWapiNetwork(config) && isConfigForWapiNetwork(config1))) {
            return false;
        }
        if (config.getAuthType() == KeyMgmt.WAPI_PSK && config1.getAuthType() == KeyMgmt.WAPI_PSK) {
            return false;
        }
        if (config.wapiCertSel == null) {
            if (config1.wapiCertSel == null) {
                return false;
            } else {
                return true;
            }
        }
        if (!config.wapiCertSel.equals(config1.wapiCertSel)) {
            return true;
        }
        return false;
    }

    public static MtkWapi getInstance() {
        if (sMtkWapi == null) {
            synchronized (TAG) {
                sMtkWapi = new MtkWapi();
            }
        }
        return sMtkWapi;
    }

    /**
     * Check whether the given config is WAPI security
     */
    public static boolean isConfigForWapiNetwork(WifiConfiguration config) {
        if (config == null) return false;
        if (isWapiPskConfiguration(config) || isWapiCertConfiguration(config)) return true;
        return false;
    }

    /**
     * Check whether the given config is WAPI-PSK security
     */
    public static boolean isWapiPskConfiguration(WifiConfiguration config) {
        if (config == null) return false;
        if (config.allowedKeyManagement.get(KeyMgmt.WAPI_PSK)) {
            return true;
        }
        return false;
    }

    /**
     * Check whether the given config is WAPI-CERT security
     */
    public static boolean isWapiCertConfiguration(WifiConfiguration config) {
        if (config == null) return false;
        if (config.allowedKeyManagement.get(KeyMgmt.WAPI_CERT)) {
            return true;
        }
        return false;
    }

    /**
     * Build the ScanResult.capabilities String.
     */
    public static String generateCapabilitiesString(InformationElement[] ies,
            BitSet beaconCap, String capabilities) {
        String isWAPI = null;
        if (ies == null || beaconCap == null) {
            return capabilities;
        }

        //Check if WAPI supports or not
        if (!mIsCheckedSupport) {
            if (checkSupportWapi()) {
                init();
            }
        }

        if (!mIsSystemSupportWapi) {
            return capabilities;
        }

        // Check whether its WAPI security or not from ies.
        for (InformationElement ie : ies) {
            if (ie.id == EID_WAPI) {
                isWAPI = parseWapiElement(ie);
            }
        }
        if (isWAPI != null) {
            capabilities += isWAPI;
            if (capabilities.contains("[WEP]")) {
                return capabilities.replace("[WEP]", "");
            }
        }
        return capabilities;
    }

    /**
     * Helper method to check if the provided |scanResult| corresponds to a WAPI network or not.
     * This checks if the provided capabilities string contains WAPI encryption type or not.
     */
    public static boolean isScanResultForWapiNetwork(ScanResult scanResult) {
        return scanResult.capabilities.contains("WAPI");
    }

    public static void setupMtkIface(String ifaceName) {
    ISupplicantIface mtkIfaceHwBinder;
        mtkIfaceHwBinder = getMtkIfaceV2_0(ifaceName);
        if (mtkIfaceHwBinder == null) {
            Log.e(TAG, "setupMtkIface got null iface");
        } else {
            Log.e(TAG, "mtkIfaceHwBinder get successfully");
            mMtkiface = getMtkStaIfaceMockableV2_0(mtkIfaceHwBinder);
            if (mMtkiface == null) {
                Log.e(TAG, "Mtk sta iface null");
            }
        }
    }

    protected static ISupplicantStaIface getMtkStaIfaceMockableV2_0(ISupplicantIface iface) {
        synchronized (getLockForSupplicantStaIfaceHal()) {
            return ISupplicantStaIface.asInterface(iface.asBinder());
        }
    }

    private static ISupplicantIface getMtkIfaceV2_0(@NonNull String ifaceName) {
        synchronized (getLockForSupplicantStaIfaceHal()) {
            /** List all supplicant Ifaces */
            final ArrayList<ISupplicant.IfaceInfo> supplicantIfaces = new ArrayList<>();
            try {
                getISupplicant().listInterfaces((SupplicantStatus status,
                ArrayList<ISupplicant.IfaceInfo> ifaces) -> {
                    if (status.code != SupplicantStatusCode.SUCCESS) {
                        Log.e(TAG, "Getting Supplicant Interfaces failed: " + status.code);
                        return;
                    }
                    supplicantIfaces.addAll(ifaces);
                });
            } catch (RemoteException e) {
                Log.e(TAG, "ISupplicant.listInterfaces exception: " + e);
                supplicantServiceDiedHandler(ifaceName);
                return null;
            }
            if (supplicantIfaces.size() == 0) {
                Log.e(TAG, "Got zero HIDL supplicant ifaces. Stopping supplicant HIDL startup.");
                return null;
            }
            Mutable<ISupplicantIface> supplicantIface = new Mutable<>();
            for (ISupplicant.IfaceInfo ifaceInfo : supplicantIfaces) {
                if (ifaceInfo.type == IfaceType.STA && ifaceName.equals(ifaceInfo.name)) {
                    try {
                        vendor.mediatek.hardware.wifi.supplicant.V2_0.ISupplicant
                                mtkSupplicantIface = getMtkSupplicantMockableV2_0();
                        if (mtkSupplicantIface == null) return null;
                        mtkSupplicantIface.getInterface(ifaceInfo,
                        (SupplicantStatus status, ISupplicantIface iface) -> {
                            if (status.code != SupplicantStatusCode.SUCCESS) {
                                Log.e(TAG, "Failed to get ISupplicantIface " + status.code);
                                return;
                            }
                            supplicantIface.value = iface;
                        });
                    } catch (RemoteException e) {
                        Log.e(TAG, "ISupplicant.getInterface exception: " + e);
                        supplicantServiceDiedHandler(ifaceName);
                        return null;
                    }
                    break;
                }
            }
            return supplicantIface.value;
        }
    }

    protected static vendor.mediatek.hardware.wifi.supplicant.V2_0.ISupplicant
            getMtkSupplicantMockableV2_0() throws RemoteException {
        synchronized (getLockForSupplicantStaIfaceHal()) {
            try {
                return vendor.mediatek.hardware.wifi.supplicant.V2_0.ISupplicant.castFrom(
                    vendor.mediatek.hardware.wifi.supplicant.V2_0.ISupplicant.getService());
            } catch (NoSuchElementException e) {
                Log.e(TAG, "Failed to get IMtkSupplicant", e);
                return null;
            }
        }
    }

    /**
     * Check whether support WAPI or not
     *
     * @param aliases String to be set.
     * @return true if request is sent successfully, false otherwise.
     */
    private static boolean checkSupportWapi() {
        SupplicantStaIfaceHal supplicant = WifiInjector.getInstance().getSupplicantStaIfaceHal();
        synchronized (getLock(supplicant)) {
            final String methodStr = "getMtkFeatureMask";
            if (!checkMtkIfaceAndLogFailure(methodStr)) return false;
            try {
                MutableBoolean statusOk = new MutableBoolean(false);
                mMtkiface.getFeatureMask((SupplicantStatus status, int maskValue) -> {
                    statusOk.value = status.code == SupplicantStatusCode.SUCCESS;
                    if (statusOk.value) {
                        mIsSystemSupportWapi =
                                ((maskValue & ISupplicantStaIface.FeatureMask.WAPI) ==
                                        ISupplicantStaIface.FeatureMask.WAPI);
                        mIsCheckedSupport = true;
                    }
                    checkStatusAndLogFailure(supplicant, status, methodStr);
                });
                    return statusOk.value;
            } catch (RemoteException e) {
                handleRemoteException(supplicant, e, methodStr);
                return false;
            }
        }
    }

    /**
     * Set all WAPI certificate selection (aliases) to supplicant
     *
     * @param aliases String to be set.
     * @return true if request is sent successfully, false otherwise.
     */
    public static boolean setWapiCertAliasList(String aliases) {
        SupplicantStaIfaceHal supplicant = WifiInjector.getInstance().getSupplicantStaIfaceHal();
        synchronized (getLock(supplicant)) {
            final String methodStr = "setWapiCertAliasList";
            if (!checkMtkIfaceAndLogFailure(methodStr)) return false;
            try {
                SupplicantStatus status = mMtkiface.setWapiCertAliasList(aliases);
                return checkStatusAndLogFailure(supplicant, status, methodStr);
            } catch (RemoteException e) {
                handleRemoteException(supplicant, e, methodStr);
                return false;
            }
        }
    }

    public static boolean setWapiCertAlias(SupplicantStaNetworkHal network, int supplicantNetworkId,
            String alias) {
        Log.d(TAG, "supplicantNetworkId= " + supplicantNetworkId);
        if (network == null) return false;
        synchronized (getLock(network)) {
            final String methodStr = "setWapiCertAlias";
            try {
                ISupplicantStaNetwork mtkIface = getMtkStaNetwork(supplicantNetworkId);
                SupplicantStatus status =  mtkIface.setWapiCertAlias(
                    alias == null ? WifiEnterpriseConfig.EMPTY_VALUE : alias);
                return checkStatusAndLogFailure(network, status, methodStr);
            } catch (RemoteException e) {
                handleRemoteException(network, e, methodStr);
                return false;
            }
        }
    }

    private static ISupplicantStaNetwork getMtkStaNetwork(int id) {
        final String methodStr = "getMtkStaNetwork";
        Mutable<ISupplicantNetwork> gotNetwork = new Mutable<>();
        try {
            mMtkiface.getNetwork(id, (SupplicantStatus status, ISupplicantNetwork network) -> {
                if (checkStatusAndLogFailure(status, methodStr)) {
                    gotNetwork.value = network;
                }
            });
        } catch (RemoteException e) {
            Log.e(TAG, "MtkStaIface." + methodStr + " failed with exception", e);
        }
        if (gotNetwork.value != null) {
            return ISupplicantStaNetwork.asInterface(gotNetwork.value.asBinder());
        } else {
            return null;
        }
    }

    private static Object getLockForSupplicantStaIfaceHal() {
        SupplicantStaIfaceHal supplicant = WifiInjector.getInstance().getSupplicantStaIfaceHal();
        return getLock(supplicant);
    }

    private static Object getLock(SupplicantStaIfaceHal supplicant) {
        Object lock;
        try {
            Field lockField = supplicant.getClass().getDeclaredField("mLock");
            lockField.setAccessible(true);
            lock = lockField.get(supplicant);
        } catch (ReflectiveOperationException e) {
            e.printStackTrace();
            lock = new Object();
        }
        return lock;
    }

    private static ISupplicant getISupplicant() {
        SupplicantStaIfaceHal supplicant = WifiInjector.getInstance().getSupplicantStaIfaceHal();
        try {
            Field field = supplicant.getClass().getDeclaredField("mISupplicant");
            field.setAccessible(true);
            return (ISupplicant) field.get(supplicant);
        } catch (ReflectiveOperationException e) {
            e.printStackTrace();
            return null;
        }
    }

    private static boolean checkStatusAndLogFailure(SupplicantStatus status,
            final String methodStr) {
        if (status.code != SupplicantStatusCode.SUCCESS) {
            Log.e(TAG, "ISupplicantStaIface." + methodStr + " failed: " + status);
            return false;
        } else {
            Log.d(TAG, "ISupplicantStaIface." + methodStr + " succeeded");
            return true;
        }
    }

    private static String getInterfaceName() {
        WifiNative wifiNative = WifiInjector.getInstance().getWifiNative();
        try {
            Method method = wifiNative.getClass().getDeclaredMethod("getClientInterfaceName");
            method.setAccessible(true);
            return (String) method.invoke(wifiNative);
        } catch (ReflectiveOperationException e) {
            e.printStackTrace();
            return "";
        }
    }

    private static void supplicantServiceDiedHandler(@NonNull String ifaceName) {
        SupplicantStaIfaceHal supplicant = WifiInjector.getInstance().getSupplicantStaIfaceHal();
        try {
            Method method = supplicant.getClass().getDeclaredMethod(
                    "supplicantServiceDiedHandler", String.class);
            method.setAccessible(true);
            method.invoke(supplicant, ifaceName);
        } catch (ReflectiveOperationException e) {
            e.printStackTrace();
        }
    }

    private static boolean checkStatusAndLogFailure(SupplicantStaIfaceHal supplicant,
            SupplicantStatus status, final String methodStr) {
        try {
            Method method = supplicant.getClass().getDeclaredMethod(
                    "checkStatusAndLogFailure", SupplicantStatus.class, String.class);
            method.setAccessible(true);
            return (boolean) method.invoke(supplicant, status, methodStr);
        } catch (ReflectiveOperationException e) {
            e.printStackTrace();
            return false;
        }
    }

    private static void handleRemoteException(SupplicantStaIfaceHal supplicant,
            RemoteException re, String methodStr) {
        try {
            Method method = supplicant.getClass().getDeclaredMethod(
                    "handleRemoteException", RemoteException.class, String.class);
            method.setAccessible(true);
            method.invoke(supplicant, re, methodStr);
        } catch (ReflectiveOperationException e) {
            e.printStackTrace();
        }
    }

    private static Object getLock(SupplicantStaNetworkHal network) {
        Object lock;
        try {
            Field field = network.getClass().getDeclaredField("mLock");
            field.setAccessible(true);
            lock = field.get(network);
        } catch (ReflectiveOperationException e) {
            e.printStackTrace();
            lock = new Object();
        }
        return lock;
    }

    private static boolean checkMtkIfaceAndLogFailure(final String methodStr) {
        if (mMtkiface == null) {
            Log.e(TAG, "Can't call " + methodStr + ", Mtkiface is null");
            return false;
        } else {
            Log.d(TAG, "Do Mtkiface." + methodStr);
        }
        return true;
    }

    private static boolean checkStatusAndLogFailure(SupplicantStaNetworkHal network,
            SupplicantStatus status, final String methodStr) {
        try {
            Method method = network.getClass().getDeclaredMethod(
                    "checkStatusAndLogFailure", SupplicantStatus.class, String.class);
            method.setAccessible(true);
            return (boolean) method.invoke(network, status, methodStr);
        } catch (ReflectiveOperationException e) {
            e.printStackTrace();
            return false;
        }
    }

    private static void handleRemoteException(SupplicantStaNetworkHal network,
            RemoteException re, String methodStr) {
        try {
            Method method = network.getClass().getDeclaredMethod(
                    "handleRemoteException", RemoteException.class, String.class);
            method.setAccessible(true);
            method.invoke(network, re, methodStr);
        } catch (ReflectiveOperationException e) {
            e.printStackTrace();
        }
    }

    private static Context getContext() {
    WifiInjector wi = WifiInjector.getInstance();
        try {
            Field field = wi.getClass().getDeclaredField("mContext");
            field.setAccessible(true);
            return (Context) field.get(wi);
        } catch (ReflectiveOperationException e) {
            e.printStackTrace();
            return null;
        }
    }

    private static void init() {
        Context context = getContext();
        if (context == null) return;

        IntentFilter filter = new IntentFilter();
        filter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        context.registerReceiver(
            new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    if (intent.getAction().equals(WifiManager.WIFI_STATE_CHANGED_ACTION)) {
                        int state = intent.getIntExtra(
                                WifiManager.EXTRA_WIFI_STATE,
                                WifiManager.WIFI_STATE_UNKNOWN);
                        Log.d(TAG, "onReceive WIFI_STATE_CHANGED_ACTION state --> " + state);
                        if (state == WifiManager.WIFI_STATE_DISABLED) {
                            mWapiCertSelCache = null;
                        }
                    }
                }
            },
            new IntentFilter(filter));
    }
}
