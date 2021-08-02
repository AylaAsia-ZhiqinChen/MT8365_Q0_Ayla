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
 * limitations under the License
 */

package com.android.settings.gestures;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.settings.SettingsEnums;
import android.content.Context;
import android.os.Bundle;

import com.android.settings.R;
import com.android.settings.core.instrumentation.InstrumentedDialogFragment;

/**
 * Dialog to notify user that gesture navigation is not available because of unsupported launcher.
 */
public class GestureNavigationNotAvailableDialog extends InstrumentedDialogFragment {
    private static final String TAG = "GestureNavigationNotAvailableDialog";

    public static void show(SystemNavigationGestureSettings parent) {
        if (!parent.isAdded()) {
            return;
        }

        final GestureNavigationNotAvailableDialog dialog =
                new GestureNavigationNotAvailableDialog();
        dialog.setTargetFragment(parent, 0);
        dialog.show(parent.getFragmentManager(), TAG);
    }

    @Override
    public int getMetricsCategory() {
        return SettingsEnums.SETTINGS_GESTURE_NAV_NOT_AVAILABLE_DLG;
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        final Context context = getActivity();
        final String defaultHomeAppName = SystemNavigationPreferenceController
                .getDefaultHomeAppName(context);
        final String message = getString(R.string.gesture_not_supported_dialog_message,
                defaultHomeAppName);
        return new AlertDialog.Builder(context)
                .setMessage(message)
                .setPositiveButton(R.string.okay, null)
                .create();
    }
}