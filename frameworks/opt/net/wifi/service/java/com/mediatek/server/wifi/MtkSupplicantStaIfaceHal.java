/*
 * Copyright (C) 2014 MediaTek Inc.
 * Modification based on code covered by the mentioned copyright
 * and/or permission notice(s).
*/
package com.mediatek.server.wifi;

import android.annotation.NonNull;
import android.hardware.wifi.supplicant.V1_0.IfaceType;
import android.hardware.wifi.supplicant.V1_0.ISupplicant;
import android.hardware.wifi.supplicant.V1_0.SupplicantStatus;
import android.hardware.wifi.supplicant.V1_0.SupplicantStatusCode;
import android.os.HidlSupport.Mutable;
import android.os.RemoteException;
import android.util.Log;

import com.android.server.wifi.SupplicantStaIfaceHal;
import com.android.server.wifi.WifiInjector;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.NoSuchElementException;

import vendor.mediatek.hardware.wifi.supplicant.V2_0.ISupplicantIface;
import vendor.mediatek.hardware.wifi.supplicant.V2_1.ISupplicantStaIface;
import vendor.mediatek.hardware.wifi.supplicant.V2_1.ISupplicantStaIfaceCallback;

public class MtkSupplicantStaIfaceHal {

    private static MtkSupplicantStaIfaceHal sMtkSupplicantStaIfaceHal = null;

    private static ISupplicantStaIface mMtkSupplicantStaIface;

    private static final String TAG = "MtkSupplicantStaIfaceHal";

    public static MtkSupplicantStaIfaceHal getInstance() {
        if (sMtkSupplicantStaIfaceHal == null) {
            synchronized (TAG) {
                sMtkSupplicantStaIfaceHal = new MtkSupplicantStaIfaceHal();
            }
        }
        return sMtkSupplicantStaIfaceHal;
    }

    public static void setupMtkIface(String ifaceName) {
        ISupplicantIface mtkIfaceHwBinder;
        mtkIfaceHwBinder = getMtkIfaceV2_0(ifaceName);
        if (mtkIfaceHwBinder == null) {
            Log.e(TAG, "setupMtkIface got null iface");
        } else {
            SupplicantStaIfaceHalCallbackV2_1 callback2_1 = new SupplicantStaIfaceHalCallbackV2_1();
            registeCallback(mtkIfaceHwBinder, callback2_1);
        }
    }

    private static boolean registeCallback(ISupplicantIface iface,
            SupplicantStaIfaceHalCallbackV2_1 callback) {
        synchronized (getLockForSupplicantStaIfaceHal()) {
            final String methodStr = "registerMtkCallback";
            if (checkSupplicantStaIfaceAndLogFailure(iface, methodStr) == null) return false;
            try {
                ISupplicantStaIface supplicantStaIfaceV21 = getMtkStaIfaceMockableV2_1(iface);
                if (supplicantStaIfaceV21 != null) {
                    SupplicantStatus status =  supplicantStaIfaceV21.registerCallback_2_1(callback);
                    return checkStatusAndLogFailure(status, methodStr);
                } else {
                    Log.e(TAG, "registerMtkCallback" +
                            " mISupplicantStaIface is not IMtkSupplicantStaIface");
                    return false;
                }
            } catch (RemoteException e) {
                handleRemoteException(WifiInjector.getInstance().getSupplicantStaIfaceHal(), e,
                        methodStr);
                return false;
            }
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

    protected static vendor.mediatek.hardware.wifi.supplicant.V2_0.ISupplicant
            getMtkSupplicantMockableV2_0() throws RemoteException {
        synchronized (getLockForSupplicantStaIfaceHal()) {
            try {
                return vendor.mediatek.hardware.wifi.supplicant.V2_0.ISupplicant.castFrom(
                    vendor.mediatek.hardware.wifi.supplicant.V2_0.ISupplicant.getService());
            } catch (NoSuchElementException e) {
                Log.e(TAG, "Failed to get IMtkSupplicant2_0", e);
                return null;
            }
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
    protected static ISupplicantStaIface getMtkStaIfaceMockableV2_1(ISupplicantIface iface) {
        synchronized (getLockForSupplicantStaIfaceHal()) {
            return ISupplicantStaIface.asInterface(iface.asBinder());
        }
    }

    private static ISupplicantStaIface checkSupplicantStaIfaceAndLogFailure(
            ISupplicantIface iface, final String methodStr) {
        synchronized (getLockForSupplicantStaIfaceHal()) {
            ISupplicantStaIface mtkIface = getMtkStaIfaceMockableV2_1(iface);
            if (mtkIface == null) {
                Log.e(TAG, "Can't call " + methodStr + ", Mtkiface is null");
                return null;
            } else {
                Log.d(TAG, "Do Mtkiface." + methodStr);
            }
            return mtkIface;
        }
    }

    private static boolean checkStatusAndLogFailure(SupplicantStatus status,
            final String methodStr) {
        if (status.code != SupplicantStatusCode.SUCCESS) {
            Log.e(TAG, "ISupplicantStaIface2_1." + methodStr + " failed: " + status);
            return false;
        } else {
            Log.d(TAG, "ISupplicantStaIface2_1." + methodStr + " succeeded");
            return true;
        }
    }

    private static class SupplicantStaIfaceHalCallbackV2_1 extends
            ISupplicantStaIfaceCallback.Stub {
        @Override
        public void onEssDisassocImminentNotice(byte[/* 6 */] bssid, int pmfEnabled,
                int reAuthDelayInSec, String url) {
            synchronized (getLockForSupplicantStaIfaceHal()) {
                Log.d(TAG, "onEssDisassocImminentNotice --> " + bssid + " " + pmfEnabled
                        + " " + reAuthDelayInSec + " " + url);
            }
        }

        @Override
        public void onDataStallNotice(int errCode) {

            Log.d(TAG, "onDataStallNotice --> " + errCode);

            com.mediatek.powerhalservice.PowerHalWifiMonitor.getInstance()
              .supplicantHalCallback(errCode);
        }

        @Override
        public void onAssocFreqChanged(int freq) {
            synchronized (getLockForSupplicantStaIfaceHal()) {
                Log.d(TAG, "onAssocFreqChanged --> " + freq);
                String ifaceName =
                        WifiInjector.getInstance().getWifiNative().getClientInterfaceName();
                WifiInjector.getInstance().getWifiMonitor().broadcastAssocFreqChanged(ifaceName,
                        freq);
            }
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
}