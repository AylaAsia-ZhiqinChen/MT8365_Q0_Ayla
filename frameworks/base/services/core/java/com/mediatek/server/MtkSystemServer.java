/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2017. All rights reserved.
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
package com.mediatek.server;

import android.content.Context;

import android.util.Slog;

import android.util.TimingsTraceLog;

import com.android.server.net.NetworkPolicyManagerService;
import com.android.server.net.NetworkStatsService;
import com.android.server.NetworkManagementService;
import com.android.server.SystemServiceManager;

import dalvik.system.PathClassLoader;
import java.lang.reflect.Constructor;

public class MtkSystemServer {
    private static MtkSystemServer sInstance;
    public static PathClassLoader sClassLoader;

    public static MtkSystemServer getInstance() {
        if (null == sInstance) {
            String className = "com.mediatek.server.MtkSystemServerImpl";
            String classPackage = "/system/framework/mediatek-services.jar";
            Class<?> clazz = null;
            try {
                sClassLoader = new PathClassLoader(classPackage,
                        MtkSystemServer.class.getClassLoader());
                clazz = Class.forName(className, false, sClassLoader);
                Constructor constructor = clazz.getConstructor();
                sInstance = (MtkSystemServer) constructor.newInstance();
            } catch (Exception e) {
                Slog.e("MtkSystemServer", "getInstance: " + e.toString());
                sInstance = new MtkSystemServer();
            }
        }
        return sInstance;
    }

    public void setPrameters(TimingsTraceLog btt, SystemServiceManager ssm,
            Context context) {
    }

    public void startMtkBootstrapServices() {
    }

    public void startMtkCoreServices() {
    }

    public boolean startMtkAlarmManagerService() {
        return false;
    }

    public void startMtkOtherServices() {
    }

    public boolean startMtkStorageManagerService() {
        // By default, No MtkStorageManagerService is started
        return false;
    }

    public Object getMtkConnectivityService(NetworkManagementService networkManagement,
            NetworkStatsService networkStats,
            NetworkPolicyManagerService networkPolicy) {
        //M: To be impl. in MtkSystemServerImpl
        return null;
    }

    public void addBootEvent(String bootevent) {
    }
}

