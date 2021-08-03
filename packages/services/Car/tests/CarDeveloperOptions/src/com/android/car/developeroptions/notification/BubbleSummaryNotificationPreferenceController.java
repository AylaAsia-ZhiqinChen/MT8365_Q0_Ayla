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
import com.android.car.developeroptions.core.BasePreferenceController;

import androidx.annotation.VisibleForTesting;

public class BubbleSummaryNotificationPreferenceController extends BasePreferenceController {

    @VisibleForTesting
    static final int ON = 1;

    public BubbleSummaryNotificationPreferenceController(Context context, String preferenceKey) {
        super(context, preferenceKey);
    }

    @Override
    public CharSequence getSummary() {
        return mContext.getString(
                areBubblesEnabled() ? R.string.switch_on_text : R.string.switch_off_text);
    }

    @Override
    public int getAvailabilityStatus() {
        return AVAILABLE;
    }

    private boolean areBubblesEnabled() {
        return Settings.Secure.getInt(mContext.getContentResolver(),
                NOTIFICATION_BUBBLES, ON) == ON;
    }
}
