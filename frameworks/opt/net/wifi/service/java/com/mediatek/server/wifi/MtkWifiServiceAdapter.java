/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2015. All rights reserved.
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
package com.mediatek.server.wifi;

import android.content.Context;
import android.net.wifi.ScanResult;
import android.os.Message;
import android.os.SystemProperties;
import android.util.Log;

import com.android.internal.util.State;
import com.android.server.wifi.ScanDetail;
import dalvik.system.PathClassLoader;

import java.lang.reflect.*;
import java.util.List;

public class MtkWifiServiceAdapter {
    private static final String TAG = "MtkWifiServiceAdapter";

    private static final boolean DEBUG = Log.isLoggable(TAG, Log.DEBUG);

    private static IMtkWifiService sMWS;

    public static interface IMtkWifiService {
        public void initialize();
        public void handleScanResults(List<ScanDetail> full, List<ScanDetail> unsaved);
        public void updateRSSI(Integer newRssi, int ipAddr, int lastNetworkId);
        public boolean preProcessMessage(State state, Message msg);
        public boolean postProcessMessage(State state, Message msg, Object... args);
        public void triggerNetworkEvaluatorCallBack();
        public boolean needCustomEvaluator();
    }

    public static void initialize(Context context) {
        log("[initialize]: " + context);
        final String className = "com.mediatek.server.wifi.MtkWifiService";
        final String classPackage = "/system/framework/mtk-wifi-service.jar";
        Class<?> cls = null;
        try {
            PathClassLoader classLoader = new PathClassLoader(classPackage,
                    MtkWifiServiceAdapter.class.getClassLoader());
            cls = Class.forName(className, false, classLoader);
            Constructor constructor = cls.getConstructor(Context.class);
            sMWS = (IMtkWifiService) constructor.newInstance(context);
            sMWS.initialize();
        } catch (IllegalAccessException
                | NoSuchMethodException
                | InstantiationException
                | InvocationTargetException e1) {
            throw new Error(e1);
        } catch (ClassNotFoundException e2) {
            log("No extension found");
            e2.printStackTrace();
        }
    }

    public static void log(String message) {
        if (DEBUG) {
            Log.d(TAG, message);
        }
    }

    public static void handleScanResults(List<ScanDetail> full, List<ScanDetail> unsaved) {
        if (sMWS != null) {
            sMWS.handleScanResults(full, unsaved);
        }
    }

    public static void updateRSSI(Integer newRssi, int ipAddr, int lastNetworkId) {
        if (sMWS != null) {
            sMWS.updateRSSI(newRssi, ipAddr, lastNetworkId);
        }
    }

    public static boolean preProcessMessage(State state, Message msg) {
        if (sMWS != null) {
            return sMWS.preProcessMessage(state, msg);
        } else {
            return false;
        }
    }

    public static boolean postProcessMessage(State state, Message msg, Object... args) {
        if (sMWS != null) {
            return sMWS.postProcessMessage(state, msg, args);
        } else {
            return false;
        }
    }

    public static void triggerNetworkEvaluatorCallBack() {
        if (sMWS != null) {
            sMWS.triggerNetworkEvaluatorCallBack();
        }
    }

    public static boolean needCustomEvaluator() {
        String operator = SystemProperties.get("ro.vendor.operator.optr", "");
        if (operator != null && operator.equalsIgnoreCase("OP01")) {
            Log.i(TAG, "[needCustomEvaluator] true for OP01");
            return true;
        }
        return false;
    }
}