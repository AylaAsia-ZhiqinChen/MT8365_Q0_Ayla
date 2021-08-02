/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

package com.mediatek.op12.phone;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceCategory;
import android.preference.PreferenceScreen;
import android.util.Log;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

import com.mediatek.internal.telephony.MtkGsmCdmaPhone;
import com.mediatek.internal.telephony.ratconfiguration.RatConfiguration;
//import com.mediatek.phone.ext.DefaultMobileNetworkSettingsExt;
import com.mediatek.op12.phone.R;



/**
 * Plugin implementation for WFC Settings plugin
 */
public class OP12MobileNetworkSettingsExt{
//extends DefaultMobileNetworkSettingsExt 
    private static final String TAG = "OP12MobileNetworkSettingsExt";
    private static final String AOSP_KEY_WFC_SETTINGS = "wifi_calling_key";
    private Context mContext = null;
    public static final String BUTTON_ENABLED_NETWORKS_KEY = "enabled_networks_key";
    private static final String FEMTOCELL_SELECTION_MODE = "femtolist";
    private static final String PROP_VZW_DEVICE_TYPE = "persist.vendor.vzw_device_type";

    private Phone mPhone;
    private MyHandler mHandler;
    private MtkGsmCdmaPhone mGsmCdmaphone;
    int mMode;
    /** Constructor.
     * @param context context
     */
    public OP12MobileNetworkSettingsExt(Context context) {
        super();
        mContext = context;
        mHandler = new MyHandler();
        mPhone = PhoneFactory.getDefaultPhone();
        mGsmCdmaphone =
                (MtkGsmCdmaPhone) PhoneFactory.getPhone(mPhone.getPhoneId());
    }


    /**
     * Updating entry and entry values of list preference.
     * @param activity preference activity
     * @param subId phone id
     */
//    @Override
    public void initOtherMobileNetworkSettings(PreferenceActivity activity, int subId) {
        Log.d(TAG, "update entry and entry values of list preference");
        PreferenceScreen prefSet = activity.getPreferenceScreen();
        ListPreference buttonEnabledNetworks = (ListPreference) prefSet.findPreference(
                BUTTON_ENABLED_NETWORKS_KEY);
        if (buttonEnabledNetworks != null && isMtkLCSupport()) {
            CharSequence entries[] = mContext.getResources().getTextArray(
                    R.array.network_mode_options);
            CharSequence entryValues[] = mContext.getResources().getTextArray(
                    R.array.network_mode_options_values);
            buttonEnabledNetworks.setEntries(entries);
            buttonEnabledNetworks.setEntryValues(entryValues);
            buttonEnabledNetworks.setValueIndex(0);
        } else if (buttonEnabledNetworks != null && isCDMALessSupport()) {
            if ("4".equals(SystemProperties.get(PROP_VZW_DEVICE_TYPE, "0"))) {
                CharSequence entries[] = mContext.getResources().getTextArray(
                    R.array.cdma_less_network_mode_options);
                CharSequence entryValues[] = mContext.getResources().getTextArray(
                        R.array.cdma_less_network_mode_options_values);
                buttonEnabledNetworks.setEntries(entries);
                buttonEnabledNetworks.setEntryValues(entryValues);
                buttonEnabledNetworks.setValueIndex(0);
            } else if ("3".equals(SystemProperties.get(
                    PROP_VZW_DEVICE_TYPE, "0"))) {
                prefSet.removePreference(buttonEnabledNetworks);
            }
        }
        if (SystemProperties.get("ro.vendor.mtk_femto_cell_support").equals("1")) {
            // Add Femtocell selection mode
            mGsmCdmaphone.queryFemtoCellSystemSelectionMode(mHandler.obtainMessage(
                MyHandler.MESSAGE_QUERY_FEMTOCELL_MODE));
            CharSequence[] femtoEntries = mContext.getResources().getTextArray(
                    R.array.femto_list_options);
            CharSequence femtoEntryValues[] = mContext.getResources().getTextArray(
                    R.array.femto_list_values);
            ListPreference mFemtoListpref = new ListPreference(prefSet.getContext());
            Log.d(TAG, "list Initialises");
            mFemtoListpref.setKey(FEMTOCELL_SELECTION_MODE); //Refer to get the pref value
            mFemtoListpref.setEntries(femtoEntries);
            mFemtoListpref.setEntryValues(femtoEntryValues);
            mFemtoListpref.setDialogTitle(mContext.getResources().getText(
                    R.string.femtocell_preference));
            mFemtoListpref.setTitle(mContext.getResources().getText(R.string.femtocell_preference));
            String prefValue = Integer.toString(mMode);
            mFemtoListpref.setValue(prefValue);
            Log.d(TAG, "Added to Prefset");
            prefSet.addPreference(mFemtoListpref);
            Preference.OnPreferenceChangeListener femtolistchangeListener =
                    new Preference.OnPreferenceChangeListener() {
                public boolean onPreferenceChange(Preference preference, Object objValue) {
                    // Code goes here
                    int mode = Integer.parseInt((String) objValue);
                    Log.d(TAG, "Update the Femto Cell Preference");
                    mGsmCdmaphone.setFemtoCellSystemSelectionMode(mode, mHandler.obtainMessage(
                        MyHandler.MESSAGE_SELECT_FEMTOCELL_MODE));
                    return true;
                }
            };
            mFemtoListpref.setOnPreferenceChangeListener(femtolistchangeListener);
            Log.d(TAG, "initOtherNetworkSetting op12 preference added");
        }
    }

    /**
     * Updating network mode and summary.
     * @param buttonEnabledNetworks list preference
     * @param networkMode network mode
     */
//    @Override
    public void updatePreferredNetworkValueAndSummary(ListPreference buttonEnabledNetworks,
            int networkMode) {
        Log.d(TAG, "Updating network mode and summary. networkMode:" + networkMode);
        if (buttonEnabledNetworks == null) {
            Log.d(TAG, "buttonEnabledNetworks is null");
            return;
        }
//        switch (networkMode) {
//            case Phone.NT_MODE_LTE_CDMA_AND_EVDO:
//                buttonEnabledNetworks.setValue(Integer.toString(Phone.NT_MODE_LTE_CDMA_AND_EVDO));
//                buttonEnabledNetworks.setSummary(mContext.getText(R.string.global_mode));
//                break;
//            case Phone.NT_MODE_LTE_ONLY:
//                buttonEnabledNetworks.setValue(Integer.toString(Phone.NT_MODE_LTE_ONLY));
//                buttonEnabledNetworks.setSummary(mContext.getText(R.string.lte_only_mode));
//                break;
//            case Phone.NT_MODE_WCDMA_PREF :
//                buttonEnabledNetworks.setValue(Integer.toString(Phone.NT_MODE_WCDMA_PREF));
//                buttonEnabledNetworks.setSummary(mContext.getText(R.string.gsm_umts_mode));
//                break;
//            case Phone.NT_MODE_LTE_GSM_WCDMA :
//                buttonEnabledNetworks.setValue(Integer.toString(Phone.NT_MODE_LTE_GSM_WCDMA));
//                buttonEnabledNetworks.setSummary(mContext.getText(
//                        R.string.global_lte_gsm_umts_mode));
//                break;
//            default:
//        }
    }


    /**
     * C2k LCSupport (C/Lf).
     * @return true if c2k LC supported.
     */
    public boolean isMtkLCSupport() {
        boolean isSupport = RatConfiguration.isC2kSupported() &&
                RatConfiguration.isLteFddSupported() &&
                !RatConfiguration.isGsmSupported() &&
                !RatConfiguration.isWcdmaSupported() &&
                !RatConfiguration.isTdscdmaSupported();
        boolean lcSupport = false;
        if ((SystemProperties.get("ro.vendor.mtk_protocol1_rat_config")).equals("C/Lf")) {
            lcSupport = true;
        }
        Log.d(TAG, "isMtkLCSupport(): " + isSupport);
        return (isSupport && lcSupport);
    }

    /**
     * CDMA less support Device(Lf/W/G).
     * @return true if CDMA less supported.
     */
    public boolean isCDMALessSupport() {
        boolean isCDMALessSupport = false;
        if ("3".equals(SystemProperties.get(PROP_VZW_DEVICE_TYPE, "0")) ||
            "4".equals(SystemProperties.get(PROP_VZW_DEVICE_TYPE, "0"))) {
            isCDMALessSupport = true;
        }
        return isCDMALessSupport;
    }




    /**
     * Judge if it is CT Plugin.
     * @return true if it it CT Plugin.
     */
//    @Override
    public boolean isCtPlugin() {
        return true;
    }

//    @Override
    public void  customizeDualVolteOpHide(PreferenceScreen preferenceScreen,
         Preference preference, boolean showPreference) {
         Log.d(TAG, "customizeDualVolteOpHide");
         if (preference != null) {
             preferenceScreen.removePreference(preference);
             preference = null;
             Log.d(TAG, "customizeDualVolteOpHide: Remove VoLTE button");
         }
    }

//   @Override
    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
         Log.d(TAG, "onPreferenceTreeClick op12");
         Preference femtopref = preferenceScreen.findPreference(FEMTOCELL_SELECTION_MODE);
        if (preference == femtopref) {
            return true;
        } else {
            return false;
        }
    }

   /** Customize WFC pref as per operator requirement, on basis of MCC+MNC
    * @param context context
    * @param preferenceScreen preferenceScreen
    * @param callingCategory calling Category
    * @return
    */
//    @Override
    public void customizeWfcPreference(Context context, PreferenceScreen preferenceScreen,
                        PreferenceCategory callingCategory, int phoneId) {

        Preference wfcSettingsPreference = (Preference) callingCategory
                .findPreference(AOSP_KEY_WFC_SETTINGS);
        Log.d(TAG, "remove wfc preference:" + wfcSettingsPreference);
        if (callingCategory != null && wfcSettingsPreference != null) {
            callingCategory.removePreference(wfcSettingsPreference);
            Log.d(TAG, "remove wfc preference:" + wfcSettingsPreference);
        }
    }

    /**
     * Handler class to handle Femtocell selection.
     */
    private class MyHandler extends Handler {

        private static final int MESSAGE_QUERY_FEMTOCELL_MODE = 0;
        private static final int MESSAGE_SELECT_FEMTOCELL_MODE = 1;

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MESSAGE_QUERY_FEMTOCELL_MODE:
                    int arg = msg.arg1;
                    handleQueryFemtocellMode(arg);
                    break;

                case MESSAGE_SELECT_FEMTOCELL_MODE:
                    handleSetFemtocellMode();
                    break;
                default:
                    break;
            }
        }

    }
    private void handleQueryFemtocellMode(int mode) {
        mMode = mode;
    }

    private void handleSetFemtocellMode() {
        Log.d(TAG, "handleSetFemtocellMode ");
    }
}
