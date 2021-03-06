/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

package com.android.settings.datausage;

import static android.net.ConnectivityManager.TYPE_MOBILE;
import static android.net.ConnectivityManager.TYPE_WIFI;
import static android.telephony.TelephonyManager.SIM_STATE_READY;

import android.app.usage.NetworkStats.Bucket;
import android.app.usage.NetworkStatsManager;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkTemplate;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.BidiFormatter;
import android.text.format.Formatter;
import android.text.format.Formatter.BytesResult;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;

/**
 * Utility methods for data usage classes.
 */
public final class DataUsageUtils extends com.android.settingslib.net.DataUsageUtils {
    static final boolean TEST_RADIOS = false;
    static final String TEST_RADIOS_PROP = "test.radios";
    private static final boolean LOGD = false;
    private static final String ETHERNET = "ethernet";
    private static final String TAG = "DataUsageUtils";

    private DataUsageUtils() {
    }

    /**
     * Format byte value to readable string using IEC units.
     */
    public static CharSequence formatDataUsage(Context context, long byteValue) {
        final BytesResult res = Formatter.formatBytes(context.getResources(), byteValue,
                Formatter.FLAG_IEC_UNITS);
        return BidiFormatter.getInstance().unicodeWrap(context.getString(
                com.android.internal.R.string.fileSizeSuffix, res.value, res.units));
    }

    /**
     * Test if device has an ethernet network connection.
     */
    public static boolean hasEthernet(Context context) {
        if (DataUsageUtils.TEST_RADIOS) {
            return SystemProperties.get(DataUsageUtils.TEST_RADIOS_PROP).contains(ETHERNET);
        }

        final ConnectivityManager conn = ConnectivityManager.from(context);
        if (!conn.isNetworkSupported(ConnectivityManager.TYPE_ETHERNET)) {
            return false;
        }

        final TelephonyManager telephonyManager = TelephonyManager.from(context);
        final NetworkStatsManager networkStatsManager =
                context.getSystemService(NetworkStatsManager.class);
        boolean hasEthernetUsage = false;
        try {
            final Bucket bucket = networkStatsManager.querySummaryForUser(
                    ConnectivityManager.TYPE_ETHERNET, telephonyManager.getSubscriberId(),
                    0L /* startTime */, System.currentTimeMillis() /* endTime */);
            if (bucket != null) {
                hasEthernetUsage = bucket.getRxBytes() > 0 || bucket.getTxBytes() > 0;
            }
        } catch (RemoteException e) {
            Log.e(TAG, "Exception querying network detail.", e);
        }
        return hasEthernetUsage;
    }

    /**
     * Returns whether device has mobile data.
     * TODO: This is the opposite to Utils.isWifiOnly(), it should be refactored into 1 method.
     */
    public static boolean hasMobileData(Context context) {
        ConnectivityManager connectivityManager = ConnectivityManager.from(context);
        return connectivityManager != null && connectivityManager
                .isNetworkSupported(ConnectivityManager.TYPE_MOBILE);
    }

    /**
     * Test if device has a mobile data radio with SIM in ready state.
     */
    public static boolean hasReadyMobileRadio(Context context) {
        if (DataUsageUtils.TEST_RADIOS) {
            return SystemProperties.get(DataUsageUtils.TEST_RADIOS_PROP).contains("mobile");
        }
        final List<SubscriptionInfo> subInfoList =
                SubscriptionManager.from(context).getActiveSubscriptionInfoList(true);
        // No activated Subscriptions
        if (subInfoList == null) {
            if (LOGD) {
                Log.d(TAG, "hasReadyMobileRadio: subInfoList=null");
            }
            return false;
        }
        final TelephonyManager tele = TelephonyManager.from(context);
        // require both supported network and ready SIM
        boolean isReady = true;
        for (SubscriptionInfo subInfo : subInfoList) {
            isReady = isReady & tele.getSimState(subInfo.getSimSlotIndex()) == SIM_STATE_READY;
            if (LOGD) {
                Log.d(TAG, "hasReadyMobileRadio: subInfo=" + subInfo);
            }
        }
        final ConnectivityManager conn = ConnectivityManager.from(context);
        final boolean retVal = conn.isNetworkSupported(TYPE_MOBILE) && isReady;
        if (LOGD) {
            Log.d(TAG, "hasReadyMobileRadio:"
                    + " conn.isNetworkSupported(TYPE_MOBILE)="
                    + conn.isNetworkSupported(TYPE_MOBILE)
                    + " isReady=" + isReady);
        }
        return retVal;
    }

    /**
     * Whether device has a Wi-Fi data radio.
     */
    public static boolean hasWifiRadio(Context context) {
        if (TEST_RADIOS) {
            return SystemProperties.get(TEST_RADIOS_PROP).contains("wifi");
        }

        final ConnectivityManager connectivityManager = ConnectivityManager.from(context);
        return connectivityManager != null && connectivityManager.isNetworkSupported(TYPE_WIFI);
    }

    public static boolean hasSim(Context context) {
        /// M: Get the active subscription list. @{
        SubscriptionManager subManager = SubscriptionManager.from(context);
        if (subManager == null) {
            return false;
        }

        List<SubscriptionInfo> list = subManager.getActiveSubscriptionInfoList(true);
        if (list == null || list.size() == 0) {
            return false;
        }

        return true;
        /// @}
    }

    /**
     * Returns the default subscription if available else returns
     * SubscriptionManager#INVALID_SUBSCRIPTION_ID
     */
    public static int getDefaultSubscriptionId(Context context) {
        SubscriptionManager subManager = SubscriptionManager.from(context);
        if (subManager == null) {
            return SubscriptionManager.INVALID_SUBSCRIPTION_ID;
        }
        SubscriptionInfo subscriptionInfo = subManager.getDefaultDataSubscriptionInfo();
        if (subscriptionInfo == null) {
            /// M: Get the active subscription list. @{
            List<SubscriptionInfo> list = subManager.getActiveSubscriptionInfoList(true);
            if (list == null || list.size() == 0) {
                return SubscriptionManager.INVALID_SUBSCRIPTION_ID;
            }
            /// @}
            subscriptionInfo = list.get(0);
        }
        return subscriptionInfo.getSubscriptionId();
    }

    /**
     * Returns the default network template based on the availability of mobile data, Wifi. Returns
     * ethernet template if both mobile data and Wifi are not available.
     */
    public static NetworkTemplate getDefaultTemplate(Context context, int defaultSubId) {
        if (hasMobileData(context) && defaultSubId != SubscriptionManager.INVALID_SUBSCRIPTION_ID) {
            return getMobileTemplate(context, defaultSubId);
        } else if (hasWifiRadio(context)) {
            return NetworkTemplate.buildTemplateWifiWildcard();
        } else {
            return NetworkTemplate.buildTemplateEthernet();
        }
    }

}
