/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings.network.telephony;

import android.content.Context;
import android.os.PersistableBundle;
import android.os.SystemProperties;
import android.provider.Settings;
import android.telephony.CarrierConfigManager;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import androidx.preference.ListPreference;
import androidx.preference.Preference;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.settings.R;

import com.mediatek.settings.UtilsExt;
/// M: Add more condition for enabled state.
import com.mediatek.settings.sim.TelephonyUtils;

/**
 * Preference controller for "Preferred network mode"
 */
public class PreferredNetworkModePreferenceController extends TelephonyBasePreferenceController
        implements ListPreference.OnPreferenceChangeListener {

    private static final String TAG = "PreferredNetworkModePreferenceController";

    private CarrierConfigManager mCarrierConfigManager;
    private TelephonyManager mTelephonyManager;
    private PersistableBundle mPersistableBundle;
    private boolean mIsGlobalCdma;

    public PreferredNetworkModePreferenceController(Context context, String key) {
        super(context, key);
        mCarrierConfigManager = context.getSystemService(CarrierConfigManager.class);
    }

    @Override
    public int getAvailabilityStatus(int subId) {
        /// M: for China mobile feature,user can not select network type @{
        boolean customerVisible = UtilsExt.getMobileNetworkSettingsExt(mContext)
                .customizeAutoSelectNetworkTypePreference();
        boolean cmccOpenMarketvisible
                 = SystemProperties.get("ro.vendor.cmcc_light_cust_support").equals("1");
        boolean customerTMOVisible = UtilsExt.getMobileNetworkSettingsExt(mContext)
                .customizeTMONetworkModePreference(subId);
        boolean customerATTVisible = UtilsExt.getMobileNetworkSettingsExt(mContext)
                .customizeATTNetworkModePreference(subId);
        boolean customerVWZVisible = UtilsExt.getMobileNetworkSettingsExt(mContext)
                .customizeVWZNetworkModePreference(subId);
        boolean customerRjiovisible = UtilsExt.getMobileNetworkSettingsExt(mContext)
                .customizeRjioNetworkModePreference(subId);
        Log.i(TAG, "getAvailabilityStatus customerVisible = " + customerVisible
                + " cmccOpenMarketvisible = " + cmccOpenMarketvisible
                + " customerTMOvisible = " + customerTMOVisible
                + " customerATTvisible = " + customerATTVisible
                + " customerVWZVisible = " + customerVWZVisible
                + " customerRjiovisible = " + customerRjiovisible);
        customerVisible = customerVisible
                || cmccOpenMarketvisible
                || customerTMOVisible
                || customerATTVisible
                || customerVWZVisible
                || customerRjiovisible;
        if (customerVisible) {
            return customerVisible ? CONDITIONALLY_UNAVAILABLE : AVAILABLE;
        }
        /// @ }
        final PersistableBundle carrierConfig = mCarrierConfigManager.getConfigForSubId(subId);
        final TelephonyManager telephonyManager = TelephonyManager
                .from(mContext).createForSubscriptionId(subId);
        boolean visible;
        if (subId == SubscriptionManager.INVALID_SUBSCRIPTION_ID) {
            Log.d(TAG, "getAvailabilityStatus, subId is invalid.");
            visible = false;
        } else if (carrierConfig == null) {
            Log.d(TAG, "getAvailabilityStatus, carrierConfig is null.");
            visible = false;
        } else if (carrierConfig.getBoolean(
                CarrierConfigManager.KEY_HIDE_CARRIER_NETWORK_SETTINGS_BOOL)) {
            Log.d(TAG, "getAvailabilityStatus, KEY_HIDE_CARRIER_NETWORK_SETTINGS_BOOL is true.");
            visible = false;
        } else if (carrierConfig.getBoolean(
                CarrierConfigManager.KEY_HIDE_PREFERRED_NETWORK_TYPE_BOOL)
                && !telephonyManager.getServiceState().getRoaming()
                && telephonyManager.getServiceState().getDataRegState()
                == ServiceState.STATE_IN_SERVICE) {
            Log.d(TAG, "getAvailabilityStatus, KEY_HIDE_PREFERRED_NETWORK_TYPE_BOOL is true.");
            visible = false;
        } else if (carrierConfig.getBoolean(CarrierConfigManager.KEY_WORLD_PHONE_BOOL)) {
            Log.d(TAG, "getAvailabilityStatus, KEY_WORLD_PHONE_BOOL is true.");
            visible = true;
        } else {
            visible = false;
        }

        return visible ? AVAILABLE : CONDITIONALLY_UNAVAILABLE;
    }

    @Override
    public void updateState(Preference preference) {
        super.updateState(preference);
        final ListPreference listPreference = (ListPreference) preference;
        final int networkMode = getPreferredNetworkMode();
        listPreference.setValue(Integer.toString(networkMode));
        listPreference.setSummary(getPreferredNetworkModeSummaryResId(networkMode));
        /// M: Add more condition for enabled state.
        listPreference.setEnabled(
                MobileNetworkUtils.shouldEnableMobileNetworkOption(mPhoneId));
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object object) {
        final int settingsMode = Integer.parseInt((String) object);

        if (mTelephonyManager.setPreferredNetworkType(mSubId, settingsMode)) {
            Settings.Global.putInt(mContext.getContentResolver(),
                    Settings.Global.PREFERRED_NETWORK_MODE + mSubId,
                    settingsMode);
            return true;
        }

        return false;
    }

    public void init(int subId) {
        mSubId = subId;
        /// M: Add for recording the phone Id.
        mPhoneId = SubscriptionManager.getPhoneId(subId);
        final PersistableBundle carrierConfig = mCarrierConfigManager.getConfigForSubId(mSubId);
        mTelephonyManager = TelephonyManager.from(mContext).createForSubscriptionId(mSubId);

        final boolean isLteOnCdma =
                mTelephonyManager.getLteOnCdmaMode() == PhoneConstants.LTE_ON_CDMA_TRUE;
        mIsGlobalCdma = isLteOnCdma
                && carrierConfig.getBoolean(CarrierConfigManager.KEY_SHOW_CDMA_CHOICES_BOOL);
    }

    private int getPreferredNetworkMode() {
        return Settings.Global.getInt(mContext.getContentResolver(),
                Settings.Global.PREFERRED_NETWORK_MODE + mSubId,
                Phone.PREFERRED_NT_MODE);
    }

    private int getPreferredNetworkModeSummaryResId(int NetworkMode) {
        switch (NetworkMode) {
            case TelephonyManager.NETWORK_MODE_TDSCDMA_GSM_WCDMA:
                return R.string.preferred_network_mode_tdscdma_gsm_wcdma_summary;
            case TelephonyManager.NETWORK_MODE_TDSCDMA_GSM:
                return R.string.preferred_network_mode_tdscdma_gsm_summary;
            case TelephonyManager.NETWORK_MODE_WCDMA_PREF:
                return R.string.preferred_network_mode_wcdma_perf_summary;
            case TelephonyManager.NETWORK_MODE_GSM_ONLY:
                return R.string.preferred_network_mode_gsm_only_summary;
            case TelephonyManager.NETWORK_MODE_TDSCDMA_WCDMA:
                return R.string.preferred_network_mode_tdscdma_wcdma_summary;
            case TelephonyManager.NETWORK_MODE_WCDMA_ONLY:
                return R.string.preferred_network_mode_wcdma_only_summary;
            case TelephonyManager.NETWORK_MODE_GSM_UMTS:
                return R.string.preferred_network_mode_gsm_wcdma_summary;
            case TelephonyManager.NETWORK_MODE_CDMA_EVDO:
                switch (mTelephonyManager.getLteOnCdmaMode()) {
                    case PhoneConstants.LTE_ON_CDMA_TRUE:
                        return R.string.preferred_network_mode_cdma_summary;
                    default:
                        return R.string.preferred_network_mode_cdma_evdo_summary;
                }
            case TelephonyManager.NETWORK_MODE_CDMA_NO_EVDO:
                return R.string.preferred_network_mode_cdma_only_summary;
            case TelephonyManager.NETWORK_MODE_EVDO_NO_CDMA:
                return R.string.preferred_network_mode_evdo_only_summary;
            case TelephonyManager.NETWORK_MODE_LTE_TDSCDMA:
                return R.string.preferred_network_mode_lte_tdscdma_summary;
            case TelephonyManager.NETWORK_MODE_LTE_ONLY:
                return R.string.preferred_network_mode_lte_summary;
            case TelephonyManager.NETWORK_MODE_LTE_TDSCDMA_GSM:
                return R.string.preferred_network_mode_lte_tdscdma_gsm_summary;
            case TelephonyManager.NETWORK_MODE_LTE_TDSCDMA_GSM_WCDMA:
                return R.string.preferred_network_mode_lte_tdscdma_gsm_wcdma_summary;
            case TelephonyManager.NETWORK_MODE_LTE_GSM_WCDMA:
                return R.string.preferred_network_mode_lte_gsm_wcdma_summary;
            case TelephonyManager.NETWORK_MODE_LTE_CDMA_EVDO:
                return R.string.preferred_network_mode_lte_cdma_evdo_summary;
            case TelephonyManager.NETWORK_MODE_TDSCDMA_ONLY:
                return R.string.preferred_network_mode_tdscdma_summary;
            case TelephonyManager.NETWORK_MODE_LTE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                return R.string.preferred_network_mode_lte_tdscdma_cdma_evdo_gsm_wcdma_summary;
            case TelephonyManager.NETWORK_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
                if (mTelephonyManager.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA
                        || mIsGlobalCdma
                        || MobileNetworkUtils.isWorldMode(mContext, mSubId)) {
                    return R.string.preferred_network_mode_global_summary;
                } else {
                    return R.string.preferred_network_mode_lte_summary;
                }
            case TelephonyManager.NETWORK_MODE_TDSCDMA_CDMA_EVDO_GSM_WCDMA:
                return R.string.preferred_network_mode_tdscdma_cdma_evdo_gsm_wcdma_summary;
            case TelephonyManager.NETWORK_MODE_GLOBAL:
                return R.string.preferred_network_mode_cdma_evdo_gsm_wcdma_summary;
            case TelephonyManager.NETWORK_MODE_LTE_TDSCDMA_WCDMA:
                return R.string.preferred_network_mode_lte_tdscdma_wcdma_summary;
            case TelephonyManager.NETWORK_MODE_LTE_WCDMA:
                return R.string.preferred_network_mode_lte_wcdma_summary;
            default:
                return R.string.preferred_network_mode_global_summary;
        }
    }
}
