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
package com.mediatek.server;

import static android.net.NetworkStats.TAG_ALL;
import static android.net.NetworkStats.TAG_NONE;
import static android.net.NetworkStats.UID_ALL;
import static android.net.TrafficStats.MB_IN_BYTES;
import static android.net.TrafficStats.PB_IN_BYTES;
import static android.Manifest.permission.READ_NETWORK_USAGE_HISTORY;
import static android.provider.Settings.Global.NETSTATS_GLOBAL_ALERT_BYTES;

import android.app.AlarmManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.NetworkIdentity;
import android.net.NetworkStats;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.INetworkManagementService;
import android.os.Looper;
import android.os.Message;
import android.os.PowerManager;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.provider.Settings;
import android.telephony.SubscriptionManager;
import android.telephony.SubscriptionManager.OnSubscriptionsChangedListener;
import android.telephony.TelephonyManager;
import android.util.ArrayMap;
import android.util.Slog;
import java.time.Clock;

import static com.android.internal.telephony.TelephonyIntents.ACTION_BACKGROUND_MOBILE_DATA_USAGE;

import com.android.server.net.NetworkStatsFactory;
import com.android.server.net.NetworkStatsObservers;
import com.android.server.net.NetworkStatsService;
import com.android.server.net.NetworkStatsService.NetworkStatsSettings;

import com.mediatek.telephony.MtkTelephonyManagerEx;
import java.io.File;

public class MtkNetworkStatsService extends NetworkStatsService {
    private static final String TAG = MtkNetworkStatsService.class.getSimpleName();

    static final int SUBSCRIPTION_OR_SIM_CHANGED = 0;

    private Context mContext;
    private HandlerThread mHandlerThread;
    private InternalHandler mHandler;
    private long mEmGlobalAlert = 2 * MB_IN_BYTES;
    private NetworkStats mLatencyStats;

    public MtkNetworkStatsService(Context context, INetworkManagementService networkManager,
            AlarmManager alarmManager, PowerManager.WakeLock wakeLock, Clock clock,
            TelephonyManager teleManager, NetworkStatsSettings settings,
            NetworkStatsObservers statsObservers, File systemDir, File baseDir) {
        super(context, networkManager, alarmManager, wakeLock, clock, teleManager,
            settings, statsObservers, systemDir, baseDir);
        Slog.d(TAG, "MtkNetworkStatsService starting up");
        mContext = context;
        initDataUsageIntent(context);
    }

    private void initDataUsageIntent(Context context) {
        mHandlerThread = new HandlerThread("NetworkStatInternalHandler");
        mHandlerThread.start();
        mHandler = new InternalHandler(mHandlerThread.getLooper());

        SubscriptionManager.from(context)
            .addOnSubscriptionsChangedListener(mOnSubscriptionsChangedListener);
    }


    // M: Add support for Multiple ViLTE
    protected void rebuildActiveVilteIfaceMap() {
        //Do not use subId anymore.
    }

    // return false to use AOSP procedure
    protected boolean findOrCreateMultipleVilteNetworkIdentitySets(NetworkIdentity vtIdent) {
        // Also check MtkImsPhoneCallTracker for interface modification
        findOrCreateNetworkIdentitySet(mActiveIfaces,
            getVtInterface(vtIdent.getSubscriberId())).add(vtIdent);
        findOrCreateNetworkIdentitySet(mActiveUidIfaces,
            getVtInterface(vtIdent.getSubscriberId())).add(vtIdent);
        return true;
    }

    private String getVtInterface(String subscribeId) {
        return VT_INTERFACE + subscribeId;
    }

    // M: ALPS04668496 improve network performance
    private BroadcastReceiver mMobileReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (ACTION_BACKGROUND_MOBILE_DATA_USAGE.equals(intent.getAction())) {
                Slog.i(TAG, "mMobileReceiver, update LatencyStats");
                updateLatencyStats();
            }
        }
    };

    @Override
    public void systemReady() {
        super.systemReady();
        final IntentFilter mobileFilter = new IntentFilter(ACTION_BACKGROUND_MOBILE_DATA_USAGE);
        mContext.registerReceiver(mMobileReceiver,
                                  mobileFilter, READ_NETWORK_USAGE_HISTORY, mHandler);
    }

    @Override
    protected void shutdownLocked() {
        super.shutdownLocked();
        mContext.unregisterReceiver(mMobileReceiver);
    }

    @Override
    protected NetworkStats getNetworkStatsUidDetail(String[] ifaces) throws RemoteException {
        NetworkStats uidSnapshot = super.getNetworkStatsUidDetail(ifaces);
        final NetworkStats latencyStats = getLatencyStats();
        if (latencyStats != null) {
            latencyStats.filter(UID_ALL, ifaces, TAG_ALL);
            NetworkStatsFactory.apply464xlatAdjustments(uidSnapshot, latencyStats,
                    mUseBpfTrafficStats);
            uidSnapshot.combineAllValues(latencyStats);
        }
        uidSnapshot.combineAllValues(mUidOperations);
        return uidSnapshot;
    }

    @Override
    protected NetworkStats getNetworkStatsXt() throws RemoteException {
         NetworkStats xtSnapshot = super.getNetworkStatsXt();
         final NetworkStats latencyStats = getLatencyStats();
         if (latencyStats != null) {
             xtSnapshot.combineAllValues(latencyStats);
         }
         return xtSnapshot;
    }

    private void updateLatencyStats() {
        int subId = SubscriptionManager.getDefaultDataSubscriptionId();
        int phoneId = SubscriptionManager.getPhoneId(subId);
        NetworkStats stats = MtkTelephonyManagerEx.getDefault().getMobileDataUsage(phoneId);

        if (mLatencyStats != null && stats != null) {
            for (int i=0; i<stats.size(); i++) {
                NetworkStats.Entry entry = stats.getValues(i, null);
                int index = mLatencyStats.findIndex(entry.iface, entry.uid, entry.set, entry.tag,
                        entry.metered, entry.roaming, entry.defaultNetwork);
                if (index == -1) {
                    continue;
                }
                NetworkStats.Entry hentry = mLatencyStats.getValues(index, null);

                if (entry.txBytes < hentry.txBytes) {
                    Slog.e(TAG, "updateLatencyStats found nagative netstats!"
                                + "iface = " + entry.iface
                                + "entry.txBytes = " + entry.txBytes
                                + "hentry.txBytes = " + hentry.txBytes);
                    MtkTelephonyManagerEx.getDefault().setMobileDataUsageSum(phoneId,
                            hentry.txBytes, hentry.txPackets, hentry.rxBytes,
                            hentry.rxPackets);
                    stats = MtkTelephonyManagerEx.getDefault().getMobileDataUsage(phoneId);
                }
            }
        }
        Slog.i(TAG, "updateLatencyStats subId:" + subId + ", phoneId:" + phoneId
                    + ", NetworkStats : " + stats);
        mLatencyStats = stats;
    }

    private NetworkStats getLatencyStats() {
        return mLatencyStats;
    }
    // end

    private class InternalHandler extends Handler {
        public InternalHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case SUBSCRIPTION_OR_SIM_CHANGED:
                    handleSimChange();
                    break;
                default:
                    break;
            }
        }
    }

    private void handleSimChange() {
        boolean isTestSim = isTestSim();
        mEmGlobalAlert = Settings.Global.getLong(mContext.getContentResolver(),
                NETSTATS_GLOBAL_ALERT_BYTES, 0);
        if (isTestSim) {
            if (mEmGlobalAlert != 2 * PB_IN_BYTES) {
                Settings.Global.putLong(mContext.getContentResolver(),
                    NETSTATS_GLOBAL_ALERT_BYTES, 2 * PB_IN_BYTES);
                advisePersistThreshold(Long.MAX_VALUE / 1000);
                Slog.d(TAG, "Configure for test sim with 2TB");
            }
        } else {
            if (mEmGlobalAlert == 2 * PB_IN_BYTES) {
                Settings.Global.putLong(mContext.getContentResolver(),
                    NETSTATS_GLOBAL_ALERT_BYTES, 2 * MB_IN_BYTES);
                advisePersistThreshold(Long.MAX_VALUE / 1000);
                Slog.d(TAG, "Restore for test sim with 2MB");
            }
        }
    }

    private final OnSubscriptionsChangedListener mOnSubscriptionsChangedListener =
            new OnSubscriptionsChangedListener() {
        @Override
        public void onSubscriptionsChanged() {
            mHandler.sendEmptyMessage(SUBSCRIPTION_OR_SIM_CHANGED);
        }
    };

    public static boolean isTestSim() {
        boolean isTestSim = false;
        isTestSim = SystemProperties.get("vendor.gsm.sim.ril.testsim").equals("1") ||
                   SystemProperties.get("vendor.gsm.sim.ril.testsim.2").equals("1") ||
                   SystemProperties.get("vendor.gsm.sim.ril.testsim.3").equals("1") ||
                   SystemProperties.get("vendor.gsm.sim.ril.testsim.4").equals("1");
        return isTestSim;
    }

}