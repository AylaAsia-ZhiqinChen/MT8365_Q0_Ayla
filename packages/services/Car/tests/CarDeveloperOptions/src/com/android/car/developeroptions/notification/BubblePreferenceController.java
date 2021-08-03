/*
 * Copyright (C) 2019 The Android Open Source Project
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

package com.android.car.developeroptions.notification;

import static android.provider.Settings.Secure.NOTIFICATION_BUBBLES;

import android.content.Context;
import android.provider.Settings;

import com.android.car.developeroptions.R;
import com.android.car.developeroptions.core.PreferenceControllerMixin;
import com.android.settingslib.RestrictedSwitchPreference;

import androidx.fragment.app.FragmentManager;
import androidx.preference.Preference;

public class BubblePreferenceController extends NotificationPreferenceController
        implements PreferenceControllerMixin, Preference.OnPreferenceChangeListener {

    private static final String TAG = "BubblePrefContr";
    private static final String KEY = "bubble_pref";
    private static final int SYSTEM_WIDE_ON = 1;
    private static final int SYSTEM_WIDE_OFF = 0;

    private FragmentManager mFragmentManager;

    public BubblePreferenceController(Context context, NotificationBackend backend) {
        super(context, backend);
    }

    public BubblePreferenceController(Context context, FragmentManager fragmentManager,
            NotificationBackend backend) {
        super(context, backend);
        mFragmentManager = fragmentManager;
    }

    @Override
    public String getPreferenceKey() {
        return KEY;
    }

    @Override
    public boolean isAvailable() {
        if (!super.isAvailable()) {
            return false;
        }
        if (mAppRow == null && mChannel == null) {
            return false;
        }
        if (mChannel != null) {
            if (Settings.Secure.getInt(mContext.getContentResolver(),
                    NOTIFICATION_BUBBLES, SYSTEM_WIDE_ON) == SYSTEM_WIDE_OFF) {
                return false;
            }
            if (isDefaultChannel()) {
                return true;
            } else {
                return mAppRow == null ? false : mAppRow.allowBubbles;
            }
        }
        return true;
    }

    public void updateState(Preference preference) {
        if (mAppRow != null) {
            RestrictedSwitchPreference pref = (RestrictedSwitchPreference) preference;
            pref.setDisabledByAdmin(mAdmin);
            if (mChannel != null) {
                pref.setChecked(mChannel.canBubble());
                pref.setEnabled(isChannelConfigurable() && !pref.isDisabledByAdmin());
            } else {
                pref.setChecked(mAppRow.allowBubbles
                        && Settings.Secure.getInt(mContext.getContentResolver(),
                        NOTIFICATION_BUBBLES, SYSTEM_WIDE_ON) == SYSTEM_WIDE_ON);
                pref.setSummary(mContext.getString(
                        R.string.bubbles_app_toggle_summary, mAppRow.label));
            }
        }
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        final boolean value = (Boolean) newValue;
        if (mChannel != null) {
            mChannel.setAllowBubbles(value);
            saveChannel();
            return true;
        } else if (mAppRow != null) {
            RestrictedSwitchPreference pref = (RestrictedSwitchPreference) preference;
            // if the global setting is off, toggling app level permission requires extra
            // confirmation
            if (Settings.Secure.getInt(mContext.getContentResolver(),
                    NOTIFICATION_BUBBLES, SYSTEM_WIDE_ON) == SYSTEM_WIDE_OFF
                    && !pref.isChecked()) {
                new BubbleWarningDialogFragment()
                        .setPkgInfo(mAppRow.pkg, mAppRow.uid)
                        .show(mFragmentManager, "dialog");
                return false;
            } else {
                mAppRow.allowBubbles = value;
                mBackend.setAllowBubbles(mAppRow.pkg, mAppRow.uid, value);
            }
        }
        return true;
    }

    // Used in app level prompt that confirms the user is ok with turning on bubbles
    // globally. If they aren't, undo what
    public static void revertBubblesApproval(Context mContext, String pkg, int uid) {
        NotificationBackend backend = new NotificationBackend();
        backend.setAllowBubbles(pkg, uid, false);
        // changing the global settings will cause the observer on the host page to reload
        // correct preference state
        Settings.Secure.putInt(mContext.getContentResolver(),
                NOTIFICATION_BUBBLES, SYSTEM_WIDE_OFF);
    }

    // Apply global bubbles approval
    public static void applyBubblesApproval(Context mContext, String pkg, int uid) {
        NotificationBackend backend = new NotificationBackend();
        backend.setAllowBubbles(pkg, uid, true);
        // changing the global settings will cause the observer on the host page to reload
        // correct preference state
        Settings.Secure.putInt(mContext.getContentResolver(),
                NOTIFICATION_BUBBLES, SYSTEM_WIDE_ON);
    }
}
