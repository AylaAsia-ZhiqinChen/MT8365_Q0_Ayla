/**
 * Copyright (C) 2014 MediaTek Inc.
 * Modification based on code covered by the mentioned copyright
 * and/or permission notice(s).
 */

package com.android.server.wifi;

import android.annotation.NonNull;
import android.net.wifi.IStaStateCallback;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.RemoteException;
import android.util.Log;

import com.android.server.wifi.util.ExternalCallbackTracker;


/**
 * Monitor sta state and notifies the clients
 */
public class WifiStaStateNotifier {
    private static final String TAG = "WifiStaStateNotifier";
    private final ExternalCallbackTracker<IStaStateCallback> mRegisteredCallbacks;
    private static WifiInjector mWifiInjector;

    WifiStaStateNotifier(@NonNull Looper looper, WifiInjector wifiInjector) {
        mRegisteredCallbacks = new ExternalCallbackTracker<IStaStateCallback>(
                new Handler(looper));
        mWifiInjector = wifiInjector;
    }

    /**
     * Add a new callback and register defer event.
     */
    public void addCallback(IBinder binder, IStaStateCallback callback,
                            int callbackIdentifier) {
        Log.d(TAG, "addCallback");
        if (mRegisteredCallbacks.getNumCallbacks() > 0) {
            Log.e(TAG, "Failed to add callback, only support single request!");
            return;
        }
        if (!mRegisteredCallbacks.add(binder, callback, callbackIdentifier)) {
            Log.e(TAG, "Failed to add callback");
            return;
        }
        mWifiInjector.getActiveModeWarden().registerStaEventCallback();
    }

    /**
     * Remove an existing callback, check if need to deregister defer event.
     */
    public void removeCallback(int callbackIdentifier) {
        Log.d(TAG, "removeCallback");
        mRegisteredCallbacks.remove(callbackIdentifier);
        mWifiInjector.getActiveModeWarden().unregisterStaEventCallback();
    }

    public void onStaToBeOff() {
        Log.d(TAG, "onStaToBeOff");
        for (IStaStateCallback callback : mRegisteredCallbacks.getCallbacks()) {
            try {
                Log.d(TAG, "callback onStaToBeOff");
                callback.onStaToBeOff();
            } catch (RemoteException e) {
                // Failed to reach, skip
                // Client removal is handled in WifiService
            }
        }
    }
}
