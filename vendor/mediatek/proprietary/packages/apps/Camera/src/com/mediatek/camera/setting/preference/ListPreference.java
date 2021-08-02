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
 * MediaTek Inc. (C) 2014. All rights reserved.
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

package com.mediatek.camera.setting.preference;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.TypedArray;
import android.util.AttributeSet;
import android.util.TypedValue;

import com.android.camera.R;

import com.mediatek.camera.setting.SettingConstants;
import com.mediatek.camera.setting.SettingUtils;
import com.mediatek.camera.util.Log;

import java.util.ArrayList;
import java.util.List;

/**
 * A type of <code>CameraPreference</code> whose number of possible values is
 * limited.
 */
public class ListPreference extends CameraPreference {
    private static final String TAG = "ListPreference";

    public static final int UNKNOWN = -1;

    private final CharSequence[] mDefaultValues;
    private CharSequence[] mEntries;
    private CharSequence[] mEntryValues;
    private CharSequence[] mOriginalSupportedEntries;
    private CharSequence[] mOriginalSupportedEntryValues;
    private CharSequence[] mOriginalEntries;
    private CharSequence[] mOriginalEntryValues;

    protected boolean mIsShownInSetting = true;
    private boolean mLoaded = false;
    private boolean mEnabled = true;
    private boolean mClickable = true;
    private boolean mVisibled = true;

    protected String mValue;
    private String mOverrideValue;
    private final String mKey;
    protected String[] mExtendedValues;

    public ListPreference(Context context, AttributeSet attrs,
            SharedPreferencesTransfer prefTransfer) {
        super(context, attrs, prefTransfer);

        TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.ListPreference, 0, 0);

        mKey = a.getString(R.styleable.ListPreference_key);
        // mKey =
        // Util.checkNotNull(a.getString(R.styleable.ListPreference_key));

        // We allow the defaultValue attribute to be a string or an array of
        // strings. The reason we need multiple default values is that some
        // of them may be unsupported on a specific platform (for example,
        // continuous auto-focus). In that case the first supported value
        // in the array will be used.
        int attrDefaultValue = R.styleable.ListPreference_defaultValue;
        TypedValue tv = a.peekValue(attrDefaultValue);
        if (tv != null && tv.type == TypedValue.TYPE_REFERENCE) {
            mDefaultValues = a.getTextArray(attrDefaultValue);
        } else {
            mDefaultValues = new CharSequence[1];
            mDefaultValues[0] = a.getString(attrDefaultValue);
        }

        setEntries(a.getTextArray(R.styleable.ListPreference_entries));
        setEntryValues(a.getTextArray(R.styleable.ListPreference_entryValues));
        a.recycle();

        mOriginalEntryValues = mEntryValues;
        mOriginalEntries = mEntries;
    }

    @Override
    public void reloadValue() {
        this.mLoaded = false;
    }

    @Override
    public String toString() {
        return new StringBuilder().append("ListPreference(mKey=").append(mKey).append(", mTitle=")
                .append(getTitle()).append(", mOverride=").append(mOverrideValue)
                .append(", mEnable=").append(mEnabled).append(", mValue=").append(mValue)
                .append(", mClickable=").append(mClickable).append(")").toString();
    }

    public String getKey() {
        return mKey;
    }

    public CharSequence[] getEntries() {
        return mEntries;
    }

    public CharSequence[] getEntryValues() {
        return mEntryValues;
    }

    public void setOriginalEntries(CharSequence entries[]) {
        mOriginalEntries = entries == null ? new CharSequence[0] : entries;
    }

    public void setOriginalEntryValues(CharSequence values[]) {
        mOriginalEntryValues = values == null ? new CharSequence[0] : values;
    }

    public void setEntries(CharSequence entries[]) {
        mEntries = entries == null ? new CharSequence[0] : entries;
    }

    public void setEntryValues(CharSequence values[]) {
        mEntryValues = values == null ? new CharSequence[0] : values;
    }

    public String getValue() {
        if (!mLoaded) {
            String defaultValue = findSupportedDefaultValue();
            mValue = getSharedPreferences(mKey).getString(mKey, defaultValue);
            mLoaded = true;
            if (!isValid(mValue)) {
                mValue = defaultValue;
            }
        }
        return mValue;
    }

    public String getDefaultValue() {
        if (mDefaultValues != null && mDefaultValues.length > 0) {
            if (mDefaultValues[0] == null) {
                return null;
            } else {
                return String.valueOf(mDefaultValues[0]);
            }
        }
        return null;
    }

    /**
     * Return the first value which is supported on current platform in
     * the default values.
     * @return Return the supported default value, or null if no supported value.
     */
    public String findSupportedDefaultValue() {
        for (int i = 0; i < mDefaultValues.length; i++) {
            for (int j = 0; j < mEntryValues.length; j++) {
                // Note that mDefaultValues[i] may be null (if unspecified
                // in the xml file).
                if (mEntryValues[j].equals(mDefaultValues[i])) {
                    return mDefaultValues[i].toString();
                }
            }
        }
        return null;
    }

    public void setValue(String value) {
        if (findIndexOfValue(value) < 0) {
            throw new IllegalArgumentException();
        }
        mValue = value;
        persistStringValue(value);
    }

    /**
     * Update the preference current value as the value at the input position
     * in the entry values.
     * @param index The position in the entry values.
     */
    public void setValueIndex(int index) {
        if (index < 0 || index >= mEntryValues.length) {
//            print();
            Log.w(TAG, "setValueIndex(" + index + ")", new Throwable());
            return;
        }
        setValue(mEntryValues[index].toString());
    }

    /**
     * Return the position of the input value in the entry values.
     * @param value The input value.
     * @return The position of the input value, return -1 if this value is not
     *    in the entry values.
     */
    public int findIndexOfValue(String value) {
        for (int i = 0, n = mEntryValues.length; i < n; ++i) {
            if (SettingUtils.equals(mEntryValues[i], value)) {
                return i;
            }
        }
//        print();
        Log.w(TAG, "[findIndexOfValue]" + value + ") not find!!");
        return -1;
    }

    /**
     * Return the entry of current value which is used to show on UI.
     * @return Return the entry of current value.
     */
    public String getEntry() {
        int index = findIndexOfValue(getValue());
        if (index < 0 || index >= mEntries.length) {
//            print();
            Log.w(TAG, "[getEntry]", new Throwable());
            return null;
        }

        return mEntries[index].toString();
    }

    /**
     * Filter the unsupported values on this platform.
     * @param supported The supported values on this platform.
     */
    public void filterUnsupported(List<String> supported) {
        ArrayList<CharSequence> entries = new ArrayList<CharSequence>();
        ArrayList<CharSequence> entryValues = new ArrayList<CharSequence>();
        for (int i = 0, len = mOriginalEntryValues.length; i < len; i++) {
            if (supported.indexOf(mOriginalEntryValues[i].toString()) >= 0) {
                entries.add(mOriginalEntries[i]);
                entryValues.add(mOriginalEntryValues[i]);
            }
        }
        int size = entries.size();
        mEntries = entries.toArray(new CharSequence[size]);
        mEntryValues = entryValues.toArray(new CharSequence[size]);

        // / here remember all supported values
        mOriginalSupportedEntries = mEntries;
        mOriginalSupportedEntryValues = mEntryValues;
    }

    /**
     * Filter the unsupported entries on this platform.
     * @param supported The supported values on this platform.
     */
    public void filterUnsupportedEntries(List<String> supported) {
        ArrayList<CharSequence> entries = new ArrayList<CharSequence>();
        ArrayList<CharSequence> entryValues = new ArrayList<CharSequence>();
        for (int i = 0, len = mEntries.length; i < len; i++) {
            if (supported.indexOf(mEntries[i]) >= 0) {
                entries.add(mEntries[i]);
                entryValues.add(mEntryValues[i]);
            }
        }
        int size = entries.size();
        mEntries = entries.toArray(new CharSequence[size]);
        mEntryValues = entryValues.toArray(new CharSequence[size]);

        // / here remember all supported values
        mOriginalSupportedEntries = mEntries;
        mOriginalSupportedEntryValues = mEntryValues;
    }

    /**
     * Filter the entries which are no need to show on UI.
     * @param supported The values which are need to show on UI.
     */
    public void filterDisabled(List<String> supported) {
        ArrayList<CharSequence> entries = new ArrayList<CharSequence>();
        ArrayList<CharSequence> entryValues = new ArrayList<CharSequence>();
        for (int i = 0, len = mOriginalSupportedEntryValues.length; i < len; i++) {
            if (supported.indexOf(mOriginalSupportedEntryValues[i].toString()) >= 0) {
                entries.add(mOriginalSupportedEntries[i]);
                entryValues.add(mOriginalSupportedEntryValues[i]);
            }
        }
        int size = entries.size();
        mEntries = entries.toArray(new CharSequence[size]);
        mEntryValues = entryValues.toArray(new CharSequence[size]);
    }

    /**
     * Restore the entry values as the original values which are supported
     * on current platform.
     */
    public void restoreSupported() {
        if (mOriginalSupportedEntries != null) {
            mEntries = mOriginalSupportedEntries;
        }
        if (mOriginalSupportedEntryValues != null) {
            mEntryValues = mOriginalSupportedEntryValues;
        }
    }

    /**
     * Set the preference override value.
     * @param override The override value.
     * @param restoreSupported True restore entry values
     *     as original entry values. False no thing to do.
     */
    public void setOverrideValue(String override, boolean restoreSupported) {
        mOverrideValue = override;
        if (override == null) { // clear
            mEnabled = true;
            if (restoreSupported) {
                restoreSupported();
            }
        } else if (SettingUtils.isBuiltList(override)) {
            //mEnabled = true;
            mOverrideValue = SettingUtils.getDefaultValue(override);
            filterDisabled(SettingUtils.getEnabledList(override));
            // Some times the override values has no intersection with original supported
            // entry values, or intersection has only one item, the list preference should
            // be disabled.
            if (mEntryValues.length <= 1) {
                mEnabled = false;
            }
        } else if (SettingUtils.isDisableValue(override)) { // disable
            mEnabled = false;
            mOverrideValue = null;
        } else { // reset
            mEnabled = false;
            if (restoreSupported) {
                restoreSupported();
            }
            // for special case, override value may be not in list.
            // for example: HDR not in user list, but can be set by user.
            if (mOverrideValue != null && findIndexOfValue(mOverrideValue) == -1) {
                mOverrideValue = findSupportedDefaultValue();
                Log.w(TAG, "setOverrideValue(" + override + ") not in list! mOverrideValue="
                        + mOverrideValue);
            }
        }
        mLoaded = false;
    }

    /**
     * Set the override value.
     * @param override The override value.
     */
    public void setOverrideValue(String override) {
        setOverrideValue(override, true);
    }

    public String getOverrideValue() {
        return mOverrideValue;
    }

    public int getIconId(int index) {
        return UNKNOWN;
    }

    public boolean isEnabled() {
        return mEnabled;
    }

    public void setEnabled(boolean enabled) {
        mEnabled = enabled;
    }

    /**
     * Whether preference is visible or not.
     * @return True means this preference can be shown on UI.
     */
    public boolean isVisibled() {
        return mVisibled;
    }

    /**
     * Set preference visibility.
     * @param visibled Set visible.
     */
    public void setVisibled(boolean visibled) {
        mVisibled = visibled;
    }

    public CharSequence[] getOriginalEntryValues() {
        return mOriginalEntryValues;
    }

    public CharSequence[] getOriginalEntries() {
        return mOriginalEntries;
    }

    public CharSequence[] getOriginalSupportedEntryValues() {
        return mOriginalSupportedEntryValues;
    }

    public CharSequence[] getOriginalSupportedEntries() {
        return mOriginalSupportedEntries;
    }

    public void setClickable(boolean clickable) {
        mClickable = clickable;
    }

    public boolean isClickable() {
        return mClickable;
    }

    public void showInSetting(boolean isShowInSetting) {
        mIsShownInSetting = isShowInSetting;
    }

    public boolean isShowInSetting() {
        return mIsShownInSetting;
    }

    public String[] getExtendedValues() {
        return mExtendedValues;
    }

    public void setExtendedValues(String[] values) {
        mExtendedValues = values;
    }

    /**
     * Print entry values and default values.
     */
    public void print() {
        if (mEntryValues == null || mDefaultValues == null) {
            Log.w(TAG, "[print]mEntryValues=" + mEntryValues +
                    ", mDefaultValues=" + mDefaultValues);
            return;
        }
        Log.v(TAG, "[print] key=" + getKey() + ". value=" + getValue());
        for (int i = 0; i < mEntryValues.length; i++) {
            Log.v(TAG, "[print]entryValues[" + i + "]=" + mEntryValues[i]);
        }
        for (int i = 0; i < mDefaultValues.length; i++) {
            Log.v(TAG, "[print]defaultValues[" + i + "]=" + mDefaultValues[i]);
        }
    }

    private void persistStringValue(String value) {
        SharedPreferences.Editor editor = getSharedPreferences(mKey).edit();
        editor.putString(mKey, value);
        editor.apply();
    }

    private boolean isValid(String value) {
        boolean isValid = false;

        // Camera id preference's entry values is null.
        if (SettingConstants.KEY_CAMERA_ID.equals(mKey)) {
            return true;
        }
        // Quality don't has fixed default value.
        if (SettingConstants.KEY_VIDEO_QUALITY.endsWith(mKey)) {
            return true;
        }

        // face beauty value is get from parameter, no need reference to xml.
        if (SettingConstants.KEY_FACE_BEAUTY_SMOOTH.equals(mKey)
                || SettingConstants.KEY_FACE_BEAUTY_SKIN_COLOR.equals(mKey)
                || SettingConstants.KEY_FACE_BEAUTY_SHARP.equals(mKey)) {
            return true;
        }

        for (int i = 0, n = mOriginalEntryValues.length; i < n; ++i) {
            if (SettingUtils.equals(mOriginalEntryValues[i], value)) {
                isValid = true;
                break;
            }
        }

        return isValid;
    }
}
