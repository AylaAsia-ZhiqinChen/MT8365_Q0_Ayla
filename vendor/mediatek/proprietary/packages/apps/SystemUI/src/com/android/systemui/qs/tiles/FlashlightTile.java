/*
 * Copyright (C) 2014 The Android Open Source Project
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

package com.android.systemui.qs.tiles;

import android.app.ActivityManager;
import android.content.Intent;
import android.provider.MediaStore;
import android.service.quicksettings.Tile;
import android.widget.Switch;

import com.android.internal.logging.nano.MetricsProto.MetricsEvent;
import com.android.systemui.R;
import com.android.systemui.plugins.qs.QSTile.BooleanState;
import com.android.systemui.qs.QSHost;
import com.android.systemui.qs.tileimpl.QSTileImpl;
import com.android.systemui.statusbar.policy.FlashlightController;

import javax.inject.Inject;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.IntentFilter;

/** Quick settings tile: Control flashlight **/
public class FlashlightTile extends QSTileImpl<BooleanState> implements
        FlashlightController.FlashlightListener {

    private final Icon mIcon = ResourceIcon.get(com.android.internal.R.drawable.ic_qs_flashlight);
    private final FlashlightController mFlashlightController;

    private boolean mReceiverRegistered;
    private static final String ACTION_SET_VISIBLE = "android.intent.action.flashlight.down";
    private static final String YT_FLASHLIGHT_OPEN = "android.intent.action.flashlight.open";
    private static final String YT_FLASHLIGHT_CLOSE = "android.intent.action.flashlight.close";

    @Inject
    public FlashlightTile(QSHost host, FlashlightController flashlightController) {
        super(host);
        mFlashlightController = flashlightController;
        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_SET_VISIBLE);
        filter.addAction(YT_FLASHLIGHT_CLOSE);
        filter.addAction(YT_FLASHLIGHT_OPEN);
        mContext.registerReceiver(mReceiver, filter);
        mReceiverRegistered = true;
        mFlashlightController.observe(getLifecycle(), this);
    }

    @Override
    protected void handleDestroy() {
        super.handleDestroy();
        if (mReceiverRegistered) {
            mContext.unregisterReceiver(mReceiver);
            mReceiverRegistered = false;
        }
    }

    @Override
    public BooleanState newTileState() {
        BooleanState state = new BooleanState();
        state.handlesLongClick = false;
        return state;
    }

    @Override
    public void handleSetListening(boolean listening) {
    }

    @Override
    protected void handleUserSwitch(int newUserId) {
    }

    @Override
    public Intent getLongClickIntent() {
        return new Intent(MediaStore.INTENT_ACTION_STILL_IMAGE_CAMERA);
    }

    @Override
    public boolean isAvailable() {
        return mFlashlightController.hasFlashlight();
    }

    @Override
    protected void handleClick() {
        if (ActivityManager.isUserAMonkey()) {
            return;
        }
        boolean newState = !mState.value;
        refreshState(newState);
        mFlashlightController.setFlashlight(newState);
    }

    @Override
    public CharSequence getTileLabel() {
        return mContext.getString(R.string.quick_settings_flashlight_label);
    }

    @Override
    protected void handleLongClick() {
        handleClick();
    }

    @Override
    protected void handleUpdateState(BooleanState state, Object arg) {
        if (state.slash == null) {
            state.slash = new SlashState();
        }
        state.label = mHost.getContext().getString(R.string.quick_settings_flashlight_label);
        if (!mFlashlightController.isAvailable()) {
            state.icon = mIcon;
            state.slash.isSlashed = true;
            state.contentDescription = mContext.getString(
                    R.string.accessibility_quick_settings_flashlight_unavailable);
            state.state = Tile.STATE_UNAVAILABLE;
            return;
        }
        if (arg instanceof Boolean) {
            boolean value = (Boolean) arg;
            if (value == state.value) {
                return;
            }
            state.value = value;
        } else {
            state.value = mFlashlightController.isEnabled();
        }
        state.icon = mIcon;
        state.slash.isSlashed = !state.value;
        state.contentDescription = mContext.getString(R.string.quick_settings_flashlight_label);
        state.expandedAccessibilityClassName = Switch.class.getName();
        state.state = state.value ? Tile.STATE_ACTIVE : Tile.STATE_INACTIVE;
    }

    @Override
    public int getMetricsCategory() {
        return MetricsEvent.QS_FLASHLIGHT;
    }

    @Override
    protected String composeChangeAnnouncement() {
        if (mState.value) {
            return mContext.getString(R.string.accessibility_quick_settings_flashlight_changed_on);
        } else {
            return mContext.getString(R.string.accessibility_quick_settings_flashlight_changed_off);
        }
    }

    @Override
    public void onFlashlightChanged(boolean enabled) {
        refreshState(enabled);
    }

    @Override
    public void onFlashlightError() {
        refreshState(false);
    }

    @Override
    public void onFlashlightAvailabilityChanged(boolean available) {
        refreshState();
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (isAvailable()) {
                if (intent.getAction().equals(ACTION_SET_VISIBLE)) {
                    refreshState(!mFlashlightController.isEnabled());
                    mFlashlightController.setFlashlight(!mFlashlightController.isEnabled());
                } else if (intent.getAction().equals(YT_FLASHLIGHT_OPEN)) {
                    refreshState(true);
                    mFlashlightController.setFlashlight(true);
                } else if (intent.getAction().equals(YT_FLASHLIGHT_CLOSE)) {
                    refreshState(false);
                    mFlashlightController.setFlashlight(false);
                }
            }
        }
    };
}
