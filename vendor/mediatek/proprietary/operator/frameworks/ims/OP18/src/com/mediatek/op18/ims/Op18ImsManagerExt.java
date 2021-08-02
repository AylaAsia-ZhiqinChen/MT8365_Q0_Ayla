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

package com.mediatek.op18.ims;

import android.content.Context;
import android.os.SystemProperties;
import android.provider.Settings;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.util.Log;

import com.android.ims.ImsConfig;

import com.mediatek.ims.internal.ext.ImsManagerExt;

import java.util.List;
/**
 * Plugin implementation for Imsmanager.
 */

public class Op18ImsManagerExt extends ImsManagerExt {

    private static final String TAG = "Op18ImsManagerExt";
    private static final String MULTI_IMS_SUPPORT = "persist.vendor.mims_support";

    private static final String PRIMARY_SIM = "primary_sim";
    private final String VOWIFI_ONOFF_SETTING = "vowifi_secretcode_setting";

      /// M: Ims user setting keys
    private static final String SYSTEM_PROPERTY_MAIN_PROTOCOL =
            "persist.vendor.radio.simswitch";
    private Context mContext;

    /**
     * Constructor of plugin.
     * @param context context
     */
    public Op18ImsManagerExt(Context context) {
        super();
        mContext = context;
    }

    @Override
    /**
      * An API to customize platform enabled status.
      * @param context The context for retrive plug-in.
      * @param feature The IMS feature defined in ImsConfig.FeatureConstants.
      * @param phoneId PhoneId for which feature to be enabled.
      * @return return enabled status.
      */

    public boolean isFeatureEnabledByPlatform(Context context, int feature, int phoneId) {
        boolean isEnabled = true;
        boolean isProvisioned = true;
        Log.d(TAG, "isFeatureEnabledByPlatform feature = " + feature +
            ", phoneId = " + phoneId);
        if (SystemProperties.getInt(MULTI_IMS_SUPPORT, 1) != 1) {
            final SubscriptionManager subscriptionManager = SubscriptionManager.from(context);
            final List<SubscriptionInfo> subInfoList =
                    subscriptionManager.getActiveSubscriptionInfoList();
            if ((subInfoList != null && subInfoList.size() == 1) &&
            feature != ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI ) {
                return true;
            }

            int imsPhoneId = getImsPhoneId(context, phoneId);
            Log.d(TAG, "isFeatureEnabledByPlatform feature = " + feature +
              ", phoneId = " + phoneId + "imsPhoneId = " + imsPhoneId);
            isEnabled = (imsPhoneId == phoneId);
        }

        isProvisioned = isFeatureProvisionedOnDevice(feature);
        Log.d(TAG, "isFeatureEnabledByPlatform isEnabled = " + isEnabled +
                ", isProvisioned = " + isProvisioned);
        return (isEnabled && isProvisioned);
    }

    @Override
    /**
      * An API to get phoneId of set IMS sim.
      * @param context context of hostapp
      * @param phoneId phoneId reference to be customized based on primary SIM.
      * @return return phoneId.
      */
    public int getImsPhoneId(Context context, int phoneId) {
            int subId = Settings.Global.getInt(context.getContentResolver(),
                    PRIMARY_SIM, -1);
            if (subId == -1) {
                subId = getSubIdFromMainProtocol(context);
            }
            phoneId = (subId == -1) ? phoneId : SubscriptionManager.getPhoneId(subId);
        Log.d(TAG, "getImsPhoneId phoneId = " + phoneId);
        return phoneId;
    }

    private int getSubIdFromMainProtocol(Context context) {
        Log.d(TAG, "OP18 Primary SIM getSubIdFromMainProtocol");
        final SubscriptionManager subscriptionManager = SubscriptionManager.from(context);
        final List<SubscriptionInfo> subInfoList =
                subscriptionManager.getActiveSubscriptionInfoList();

        if (subInfoList == null) {
            Log.d(TAG, "No Sim present");
            return -1;
        }

        if (subInfoList.size() == 1) {
            return subInfoList.get(0).getSubscriptionId();
        } else {
            int mainProtocol = SystemProperties.getInt(SYSTEM_PROPERTY_MAIN_PROTOCOL, -1);
            Log.d(TAG, "OP18 Primary SIM getSubIdFromMainProtocol main_protocol = " + mainProtocol);
            if (mainProtocol != -1) {
                if (subInfoList != null && subInfoList.size() > 0) {
                    int subId = subInfoList.get(mainProtocol - 1).getSubscriptionId();
                    Log.d(TAG, "OP18 Primary SIM getSubIdFromMainProtocol subId = " + subId);
                    return subId;
                } else {
                    Log.d(TAG, "OP18 Primary SIM getSubIdFromMainProtocol subInfoList size is 0");
                    return -1;
                }
            }
        }
        Log.d(TAG, "OP18 Primary SIM getSubIdFromMainProtocol -1");
        return -1;
    }


    public boolean isFeatureProvisionedOnDevice(int feature) {
        Log.d(TAG, "feature:" + feature);
        Log.d(TAG, "persist.vendor.mtk_wfc_support =" + SystemProperties
                                .get("persist.vendor.mtk_wfc_support"));
        if ((feature == ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI)) {
            int settingValue = Settings.Global.getInt(mContext
                        .getContentResolver(), VOWIFI_ONOFF_SETTING, 0);
            Log.d(TAG, "settingValue:" + settingValue);
            if (settingValue == 1) {
                return true;
            } else {
                return false;
            }
        } else {
            return true;
        }
    }
}