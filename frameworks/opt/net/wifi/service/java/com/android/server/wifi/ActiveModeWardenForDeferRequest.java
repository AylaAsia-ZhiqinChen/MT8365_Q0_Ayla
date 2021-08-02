/**
 * Copyright (C) 2014 MediaTek Inc.
 * Modification based on code covered by the mentioned copyright
 * and/or permission notice(s).
 */

package com.android.server.wifi;

import android.annotation.NonNull;
import android.content.Context;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import com.android.internal.annotations.VisibleForTesting;
import com.android.internal.app.IBatteryStats;
import com.android.internal.util.Protocol;

import java.util.ArrayList;

public class ActiveModeWardenForDeferRequest extends ActiveModeWarden implements Handler.Callback {
    private static final String TAG = "ActiveModeWardenForDeferRequest";
    private boolean mShouldDeferDisableWifi = false;
    private ArrayList<Message> mDeferredMsgInQueue;
    private static final int BASE = Protocol.BASE_WIFI + 500;
    @VisibleForTesting
    public static final int CMD_NOTIFY_GO            = BASE + 1;
    @VisibleForTesting
    public static final int CMD_DEFER_OFF_TIMEOUT    = BASE + 2;
    @VisibleForTesting
    public static final int CMD_GO_WIFI_OFF          = BASE + 3;
    @VisibleForTesting
    public static final int CMD_GO_SCAN_MODE         = BASE + 4;
    @VisibleForTesting
    public static final int CMD_GO_SOFT_AP           = BASE + 5;
    @VisibleForTesting
    public static final int CMD_GO_SHUT_DOWN         = BASE + 6;
    @VisibleForTesting
    public static final int CMD_DEFER_WIFI_ON        = BASE + 7;
    @VisibleForTesting
    public static final int TIMEOUT = 3000;
    private final Context mContext;
    private static WifiInjector mWifiInjector;
    private static WifiStaStateNotifier mWifiStaStateNotifier;
    private Handler mEventHandler;
    private static boolean mWaitForEvent = false;
    private SoftApModeConfiguration mWifiConfig;

    public ActiveModeWardenForDeferRequest(WifiInjector wifiInjector,
                             Context context,
                             Looper looper,
                             WifiNative wifiNative,
                             DefaultModeManager defaultModeManager,
                             IBatteryStats batteryStats) {
        super(wifiInjector, context, looper, wifiNative, defaultModeManager, batteryStats);

        mDeferredMsgInQueue = new ArrayList();
        mEventHandler = new Handler(looper, this);
        mContext = context;
        mWifiInjector = wifiInjector;
        mWifiStaStateNotifier = mWifiInjector.getWifiStaStateNotifier();
    }

    @Override
    public boolean handleMessage(Message msg) {
        switch (msg.what) {
            case CMD_DEFER_OFF_TIMEOUT:
                Log.d(TAG, "Defer Wi-Fi off timeout");
            case CMD_NOTIFY_GO:
                for (int i = 0; i < mDeferredMsgInQueue.size(); i++) {
                    Log.d(TAG, "mDeferredMsgInQueue: " + mDeferredMsgInQueue.get(i));
                    Message copyMsg = mEventHandler.obtainMessage();
                    copyMsg.copyFrom(mDeferredMsgInQueue.get(i));
                    mEventHandler.sendMessage(copyMsg);
                }
                mDeferredMsgInQueue.clear();
                mWaitForEvent = false;
                break;
            case CMD_GO_WIFI_OFF:
                super.disableWifi();
                break;
            case CMD_GO_SCAN_MODE:
                super.enterScanOnlyMode();
                break;
            case CMD_GO_SOFT_AP:
                super.enterSoftAPMode(mWifiConfig);
                break;
            case CMD_GO_SHUT_DOWN:
                super.shutdownWifi();
                break;
            case CMD_DEFER_WIFI_ON:
                super.enterClientMode();
                break;
            default:
                Log.e(TAG, "Unhandle message");
                break;
        }
        return true;
    }

    @Override
    public void enterClientMode() {
        if (!mWaitForEvent) {
            super.enterClientMode();
            return;
        }
        Log.d(TAG, "enterClientMode, mWaitForEvent " + mWaitForEvent);
        mDeferredMsgInQueue.add(mEventHandler.obtainMessage(CMD_DEFER_WIFI_ON));
    }

    @Override
    public void disableWifi() {
        if (!mShouldDeferDisableWifi) {
            super.disableWifi();
            return;
        }
        Log.d(TAG, "disableWifi, mShouldDeferDisableWifi: " + mShouldDeferDisableWifi);
        mWaitForEvent = true;
        notifyStaToBeOff();
        mDeferredMsgInQueue.add(mEventHandler.obtainMessage(CMD_GO_WIFI_OFF));
        mEventHandler.sendMessageDelayed(
                mEventHandler.obtainMessage(CMD_DEFER_OFF_TIMEOUT), TIMEOUT);
    }

    @Override
    public void enterScanOnlyMode() {
        if (!mShouldDeferDisableWifi) {
            super.enterScanOnlyMode();
            return;
        }
        Log.d(TAG, "enterScanOnlyMode, mShouldDeferDisableWifi: " + mShouldDeferDisableWifi);
        mWaitForEvent = true;
        notifyStaToBeOff();
        mDeferredMsgInQueue.add(mEventHandler.obtainMessage(CMD_GO_SCAN_MODE));
        mEventHandler.sendMessageDelayed(
                mEventHandler.obtainMessage(CMD_DEFER_OFF_TIMEOUT), TIMEOUT);
    }

    @Override
    public void enterSoftAPMode(@NonNull SoftApModeConfiguration wifiConfig) {
        boolean isApStaConcurrencySupport =
                mWifiInjector.getHalDeviceManager().isConcurrentStaPlusApSupported();
        Log.d(TAG, "isApStaConcurrencySupport: " + isApStaConcurrencySupport);
        if (isApStaConcurrencySupport) {
            super.enterSoftAPMode(wifiConfig);
            return;
        }
        if (!mShouldDeferDisableWifi) {
            super.enterSoftAPMode(wifiConfig);
            return;
        }
        Log.d(TAG, "enterSoftAPMode, mShouldDeferDisableWifi: " + mShouldDeferDisableWifi);
        mWaitForEvent = true;
        mWifiConfig = wifiConfig;
        notifyStaToBeOff();
        mDeferredMsgInQueue.add(mEventHandler.obtainMessage(CMD_GO_SOFT_AP));
        mEventHandler.sendMessageDelayed(
                mEventHandler.obtainMessage(CMD_DEFER_OFF_TIMEOUT), TIMEOUT);
    }

    @Override
    public void shutdownWifi() {
        if (!mShouldDeferDisableWifi) {
            super.shutdownWifi();
            return;
        }
        Log.d(TAG, "shutdownWifi, mShouldDeferDisableWifi: " + mShouldDeferDisableWifi);
        mWaitForEvent = true;
        notifyStaToBeOff();
        mDeferredMsgInQueue.add(mEventHandler.obtainMessage(CMD_GO_SHUT_DOWN));
        mEventHandler.sendMessageDelayed(
                mEventHandler.obtainMessage(CMD_DEFER_OFF_TIMEOUT), TIMEOUT);
    }

    @Override
    public void registerStaEventCallback() {
        mShouldDeferDisableWifi = true;
    }

    @Override
    public void unregisterStaEventCallback() {
        notifyDeferEventGo();
        mShouldDeferDisableWifi = false;
    }

    @VisibleForTesting
    public void notifyStaToBeOff() {
        mWifiStaStateNotifier.onStaToBeOff();
    }

    @VisibleForTesting
    public void notifyDeferEventGo() {
        if (mShouldDeferDisableWifi) {
            mEventHandler.sendMessage(
                    mEventHandler.obtainMessage(CMD_NOTIFY_GO));
        }
    }

    @VisibleForTesting
    public ArrayList<Message> getDeferredMsgInQueue() {
        return mDeferredMsgInQueue;
    }
}
