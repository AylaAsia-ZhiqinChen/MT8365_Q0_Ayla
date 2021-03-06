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
import android.database.ContentObserver;
import android.net.Uri;
import android.os.Handler;
import android.os.Looper;
import android.os.PersistableBundle;
import android.provider.Settings;
import android.telephony.CarrierConfigManager;
import android.telephony.PhoneStateListener;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import androidx.annotation.VisibleForTesting;
import androidx.preference.Preference;
import androidx.preference.PreferenceScreen;
import androidx.preference.SwitchPreference;

import com.android.ims.ImsManager;
import com.android.settingslib.core.lifecycle.LifecycleObserver;
import com.android.settingslib.core.lifecycle.events.OnStart;
import com.android.settingslib.core.lifecycle.events.OnStop;

/// M: Add more condition for enabled state.
import com.mediatek.settings.sim.TelephonyUtils;

/**
 * Preference controller for "Video Calling"
 */
public class VideoCallingPreferenceController extends TelephonyTogglePreferenceController implements
        LifecycleObserver, OnStart, OnStop,
        Enhanced4gLtePreferenceController.On4gLteUpdateListener {

    private static final String TAG = "VideoCallingPreferenceController";

    private Preference mPreference;
    private TelephonyManager mTelephonyManager;
    private CarrierConfigManager mCarrierConfigManager;
    @VisibleForTesting
    ImsManager mImsManager;
    /// M: Phone state has been listened in MobileNetworkActivity.
    // private PhoneCallStateListener mPhoneStateListener;
    private DataContentObserver mDataContentObserver;

    public VideoCallingPreferenceController(Context context, String key) {
        super(context, key);
        mCarrierConfigManager = context.getSystemService(CarrierConfigManager.class);
        mDataContentObserver = new DataContentObserver(new Handler(Looper.getMainLooper()));
        /// M: Phone state has been listened in MobileNetworkActivity.
        // mPhoneStateListener = new PhoneCallStateListener(Looper.getMainLooper());
    }

    @Override
    public int getAvailabilityStatus(int subId) {
        /// M: Add for log. @{
        boolean enabled = false;
        if (subId != SubscriptionManager.INVALID_SUBSCRIPTION_ID) {
            /// M: Don't check whether the WiFi calling is enabled or not here.
            enabled = isVideoCallEnabled(subId);
        }
        /// @}

        Log.i(TAG, "getAvailabilityStatus, subId=" + subId + ", enabled=" + enabled);

        return enabled ? AVAILABLE : CONDITIONALLY_UNAVAILABLE;
    }

    @Override
    public void displayPreference(PreferenceScreen screen) {
        super.displayPreference(screen);
        mPreference = screen.findPreference(getPreferenceKey());
    }

    @Override
    public void onStart() {
        /// M: Phone state has been listened in MobileNetworkActivity.
        // mPhoneStateListener.register(mSubId);
        mDataContentObserver.register(mContext, mSubId);
    }

    @Override
    public void onStop() {
        /// M: Phone state has been listened in MobileNetworkActivity.
        // mPhoneStateListener.unregister();
        mDataContentObserver.unRegister(mContext);
    }

    @Override
    public void updateState(Preference preference) {
        super.updateState(preference);
        final SwitchPreference switchPreference = (SwitchPreference) preference;
        final boolean videoCallEnabled = isVideoCallEnabled(mSubId, mImsManager);
        switchPreference.setVisible(videoCallEnabled);
        if (videoCallEnabled) {
            final boolean is4gLteEnabled = mImsManager.isEnhanced4gLteModeSettingEnabledByUser()
                    && mImsManager.isNonTtyOrTtyOnVolteEnabled();
            /// M: Add more condition for enabled state.
            preference.setEnabled(is4gLteEnabled
                    && MobileNetworkUtils.shouldEnableMobileNetworkOption(mPhoneId));
            switchPreference.setChecked(is4gLteEnabled && mImsManager.isVtEnabledByUser());
        }
    }

    @Override
    public boolean setChecked(boolean isChecked) {
        mImsManager.setVtSetting(isChecked);
        return true;
    }

    @Override
    public boolean isChecked() {
        /// M: Add for checking whether ImsManager is null or not first.
        return mImsManager != null && mImsManager.isVtEnabledByUser();
    }

    public VideoCallingPreferenceController init(int subId) {
        mSubId = subId;
        /// M: Add for recording the phone Id.
        mPhoneId = SubscriptionManager.getPhoneId(subId);
        mTelephonyManager = TelephonyManager.from(mContext).createForSubscriptionId(mSubId);
        if (mSubId != SubscriptionManager.INVALID_SUBSCRIPTION_ID) {
            mImsManager = ImsManager.getInstance(mContext, SubscriptionManager.getPhoneId(mSubId));
        }

        return this;
    }

    private boolean isVideoCallEnabled(int subId) {
        final ImsManager imsManager = subId != SubscriptionManager.INVALID_SUBSCRIPTION_ID
                ? ImsManager.getInstance(mContext, SubscriptionManager.getPhoneId(subId))
                : null;
        return isVideoCallEnabled(subId, imsManager);
    }

    @VisibleForTesting
    boolean isVideoCallEnabled(int subId, ImsManager imsManager) {
        final PersistableBundle carrierConfig = mCarrierConfigManager.getConfigForSubId(subId);
        final TelephonyManager telephonyManager = TelephonyManager
                .from(mContext).createForSubscriptionId(subId);
        return carrierConfig != null && imsManager != null
                && imsManager.isVtEnabledByPlatform()
                && imsManager.isVtProvisionedOnDevice()
                && MobileNetworkUtils.isImsServiceStateReady(imsManager)
                && (carrierConfig.getBoolean(
                CarrierConfigManager.KEY_IGNORE_DATA_ENABLED_CHANGED_FOR_VIDEO_CALLS)
                || telephonyManager.isDataEnabled());
    }

    @Override
    public void on4gLteUpdated() {
        updateState(mPreference);
    }

    private class PhoneCallStateListener extends PhoneStateListener {

        public PhoneCallStateListener(Looper looper) {
            super(looper);
        }

        @Override
        public void onCallStateChanged(int state, String incomingNumber) {
            updateState(mPreference);
        }

        public void register(int subId) {
            mSubId = subId;
            mTelephonyManager.listen(this, PhoneStateListener.LISTEN_CALL_STATE);
        }

        public void unregister() {
            mTelephonyManager.listen(this, PhoneStateListener.LISTEN_NONE);
        }
    }

    /**
     * Listener that listens mobile data state change.
     */
    public class DataContentObserver extends ContentObserver {

        public DataContentObserver(Handler handler) {
            super(handler);
        }

        @Override
        public void onChange(boolean selfChange) {
            super.onChange(selfChange);
            updateState(mPreference);
        }

        public void register(Context context, int subId) {
            Uri uri = Settings.Global.getUriFor(Settings.Global.MOBILE_DATA);
            if (TelephonyManager.getDefault().getSimCount() != 1) {
                uri = Settings.Global.getUriFor(Settings.Global.MOBILE_DATA + subId);
            }
            context.getContentResolver().registerContentObserver(uri,
                    false /* notifyForDescendants */, this /* observer */);
        }

        public void unRegister(Context context) {
            context.getContentResolver().unregisterContentObserver(this);
        }
    }
}
