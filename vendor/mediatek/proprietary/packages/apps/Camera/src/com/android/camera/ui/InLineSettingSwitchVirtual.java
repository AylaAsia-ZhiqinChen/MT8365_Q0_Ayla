/*
 * Copyright (C) 2011 The Android Open Source Project
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

package com.android.camera.ui;

import android.content.Context;
import android.util.AttributeSet;
import android.view.View;
import android.view.accessibility.AccessibilityEvent;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.Switch;

import com.android.camera.CameraActivity;
import com.android.camera.Log;
import com.android.camera.R;
import com.android.camera.Util;
import com.android.camera.manager.ViewManager;

import com.mediatek.camera.setting.preference.ListPreference;
import com.mediatek.camera.setting.SettingUtils;
/**
 * In Virtual item layout:
 * ListPreference.getEntries() return child list preference key
 * ListPreference.getEntryValues() return child list default value
 * ListPreference.getDefaultValue() return default string displayed for final user.
 */
public class InLineSettingSwitchVirtual extends InLineSettingItem
        implements SettingSwitchVirtualLayout.Listener,
        CameraActivity.OnOrientationListener {
    private static final String TAG = "InLineSettingSwitchVirtual";

    private CameraActivity mContext;
    private SettingSwitchVirtualLayout mSettingLayout;
    private View mSettingContainer;
    private boolean mShowingChildList;
    private ListPreference[] mChildPrefs;
    private Switch mSwitch;
    private boolean mIsSubListItemEnable;

    OnCheckedChangeListener mCheckedChangeListener = new OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean desiredState) {
            changeIndex(desiredState ? 1 : 0);
        }
    };

    private OnClickListener mOnClickListener = new OnClickListener() {

        @Override
        public void onClick(View view) {
            Log.d(TAG, "onClick() mShowingChildList=" + mShowingChildList + ", mPreference="
                    + mPreference);
            if (!mShowingChildList && mChildPrefs != null && mPreference != null
                    && mPreference.isClickable()) {
                expendChild();
            } else {
                collapseChild();
            }
        }
    };

    public InLineSettingSwitchVirtual(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = (CameraActivity) context;
    }

    @Override
    public boolean changeIndex(int index) {
        Log.i("Refocus", "switch index = " + index);
        if (mPreference != null) {
            // mPreference.setLastValue(index == 1);
            if (index == 1) {
                SettingUtils.writePreferredStereoCamera(mContext.getPreferences(),
                        SettingUtils.STEREO_CAMERA_ON);
                if (mListener != null) {
                    mListener.onStereoCameraSettingChanged(this, mPreference, 3, false);
                }
            } else {
                SettingUtils.writePreferredStereoCamera(mContext.getPreferences(),
                        SettingUtils.STEREO_CAMERA_OFF);
                if (mListener != null) {
                    mListener.onStereoCameraSettingChanged(this, mPreference, 4, false);
                }
            }
        }
        mIsSubListItemEnable = (index == 1) ? true : false;
        return true;
    }

    @Override
    protected void onFinishInflate() {
        super.onFinishInflate();
        mSwitch = (Switch) findViewById(R.id.virtual_switch);
        mSwitch.setOnCheckedChangeListener(mCheckedChangeListener);
        setOnClickListener(mOnClickListener);
    }

    @Override
    public boolean dispatchPopulateAccessibilityEvent(AccessibilityEvent event) {
        onPopulateAccessibilityEvent(event);
        return true;
    }

    @Override
    public void onPopulateAccessibilityEvent(AccessibilityEvent event) {
        super.onPopulateAccessibilityEvent(event);
        event.getText().add(mPreference.getTitle() + mPreference.getEntry());
    }

    @Override
    public void initialize(ListPreference preference) {
        Log.i(TAG, "initialize(" + preference + ")");
        setTitle(preference);
        if (preference == null) {
            mChildPrefs = null;
            return;
        }
        mPreference = preference;
        reloadPreference();
    }

    // The value of the preference may have changed. Update the UI.
    @Override
    public void reloadPreference() {
        int len = mPreference.getEntries().length;
        mChildPrefs = new ListPreference[len];
        for (int i = 0; i < len; i++) {
            String key = String.valueOf(mPreference.getEntries()[i]);
            mChildPrefs[i] = mContext.getListPreference(key);
            Log.d(TAG, "reloadPreference() mChildPrefs[" + i + "|" + key + "]=" + mChildPrefs[i]);
        }
        updateView();
    }

    @Override
    protected void updateView() {
        Log.d(TAG, "updateView() mPreference = " + mPreference + ", mChildPrefs = " + mChildPrefs);
        if (mPreference == null || mChildPrefs == null) {
            return;
        }
        mSwitch.setOnCheckedChangeListener(null);
        if (!needUpdateSwitch()
                && SettingUtils.readPreferredStereoCamera(mContext.getPreferences()).equals(
                        SettingUtils.STEREO_CAMERA_ON)) {
            mIsSubListItemEnable = true;
            mSwitch.setChecked(true);
        } else if (needUpdateSwitch()
                && SettingUtils.readPreferredStereoCamera(mContext.getPreferences()).equals(
                        SettingUtils.STEREO_CAMERA_OFF)) {
            mIsSubListItemEnable = false;
            mSwitch.setChecked(false);
        } else {
            mIsSubListItemEnable = false;
            mSwitch.setChecked(false);
        }
        setEnabled(mPreference.isEnabled());
        mSwitch.setOnCheckedChangeListener(mCheckedChangeListener);
        setOnClickListener(null);
        int len = mChildPrefs.length;
        boolean allDefault = true;
        int enableCount = 0;
        for (int i = 0; i < len; i++) {
            ListPreference pref = mChildPrefs[i];
            if (pref == null) {
                continue;
            }
            String defaultValue = String.valueOf(mPreference.getEntryValues()[i]);
            String value = pref.getOverrideValue();
            if (value == null) {
                value = pref.getValue();
            }
            if (pref.isEnabled()) {
                enableCount++;
            }
            //we assume pref and default value is not null.
            if (allDefault && !defaultValue.equals(value)) {
                allDefault = false;
            }
        }
        boolean enabled = (enableCount == len);
        mPreference.setEnabled(enabled);
        setEnabled(mPreference.isEnabled());
        setOnClickListener(mOnClickListener);
        Log.d(TAG, "updateView() enableCount=" + enableCount + ", len=" + len);
    }

    private boolean needUpdateSwitch() {
        if (mChildPrefs != null) {
            for (int i = 0; i < mChildPrefs.length; i++) {
                if (mChildPrefs[i].findIndexOfValue(mChildPrefs[i].getValue()) == 0) {
                    Log.d(TAG, "need not update Switch");
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    public boolean expendChild() {
        boolean expend = false;
        if (!mShowingChildList) {
            mShowingChildList = true;
            if (mListener != null) {
                mListener.onShow(this);
            }
            mSettingLayout = (SettingSwitchVirtualLayout) mContext.inflate(
                    R.layout.setting_switch_virtual_layout, ViewManager.VIEW_LAYER_SETTING);
            mSettingContainer = mSettingLayout.findViewById(R.id.container);
            mSettingLayout.initialize(mChildPrefs, mIsSubListItemEnable);
            mContext.addView(mSettingLayout, ViewManager.VIEW_LAYER_SETTING);
            mContext.addOnOrientationListener(this);
            mSettingLayout.setSettingChangedListener(this);
            setOrientation(mContext.getOrientationCompensation(), false);
            fadeIn(mSettingLayout);
            mSwitch.setClickable(false);
            highlight();
            expend = true;
        }
        Log.d(TAG, "expendChild() return " + expend);
        return expend;
    }

    public boolean collapseChild() {
        boolean collapse = false;
        if (mShowingChildList) {
            mContext.removeOnOrientationListener(this);
            mContext.removeView(mSettingLayout, ViewManager.VIEW_LAYER_SETTING);
            fadeOut(mSettingLayout);
            normalText();
            mSettingLayout = null;
            mShowingChildList = false;
            if (mListener != null) {
                mListener.onDismiss(this);
            }
            mSwitch.setClickable(true);
            collapse = true;
        }
        Log.d(TAG, "collapseChild() return " + collapse);
        return collapse;
    }

    private void highlight() {
        if (mTitle != null) {
//            mTitle.setTextColor(SettingUtils.getMainColor(getContext()));
        }
        setBackgroundDrawable(null);
    }

    private void normalText() {
        if (mTitle != null) {
            mTitle.setTextColor(getResources().getColor(R.color.setting_item_text_color_normal));
        }
        setBackgroundResource(R.drawable.setting_picker);
    }

    @Override
    public void onStereoCameraSettingChanged(int index, boolean isChecked) {
        if (mListener != null) {
            if (!isChecked
                    && SettingUtils.readPreferredStereoCamera(mContext.getPreferences()).equals(
                            SettingUtils.STEREO_CAMERA_ON)) {
                SettingUtils.writePreferredStereoCamera(mContext.getPreferences(),
                        SettingUtils.STEREO_CAMERA_OFF);
            }
            if (isChecked
                    && SettingUtils.readPreferredStereoCamera(mContext.getPreferences()).equals(
                            SettingUtils.STEREO_CAMERA_OFF)) {
                SettingUtils.writePreferredStereoCamera(mContext.getPreferences(),
                        SettingUtils.STEREO_CAMERA_ON);
            }
            mListener.onStereoCameraSettingChanged(this, mPreference, index, true);
        }
    }

    @Override
    public void onOrientationChanged(int orientation) {
        setOrientation(orientation, true);
    }

    private void setOrientation(int orientation, boolean animation) {
        Log.d(TAG, "setOrientation(" + orientation + ", " + animation + ")");
        if (mShowingChildList) {
            Util.setOrientation(mSettingLayout, orientation, animation);
        }
    }

    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
        mSwitch.setEnabled(enabled);
        if (!enabled) {
            collapseChild();
        }
    }
}
