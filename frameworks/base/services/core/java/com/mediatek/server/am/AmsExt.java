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
package com.mediatek.server.am;

import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.SystemProperties;
import android.util.SparseArray;

import com.android.internal.os.ProcessCpuTracker;
import com.android.server.wm.ActivityRecord;
import com.android.server.am.ProcessRecord;
import android.content.pm.ApplicationInfo;

import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;


public class AmsExt {
    public static final int COLLECT_PSS_FG_MSG = 2;

    ///M: 512MB project keep 6 process in background to save memory.
    public static int DEFAULT_MAX_CACHED_PROCESSES_EX = ("0x20000000").equals(
           SystemProperties.get("ro.vendor.mtk_config_max_dram_size")) ? 6 : 32;

    public void onAddErrorToDropBox(String dropboxTag, String info, int pid) {}
    public void enableMtkAmsLog() {}
    public void onSystemReady(Context context) {}
    public void onBeforeActivitySwitch(ActivityRecord lastResumedActivity, ActivityRecord
            nextResumedActivity, boolean pausing, int nextResumedActivityType) {}
    public void onAfterActivityResumed(ActivityRecord resumedActivity) {}
    public void onUpdateSleep(boolean wasSleeping, boolean isSleepingAfterUpdate) {}
    public void setAalMode(int mode) {}
    public void setAalEnabled(boolean enabled) {}
    public int amsAalDump(PrintWriter pw, String[] args, int opti) { return opti; }
    public void onStartProcess(String hostingType, String packageName) {}
    public void onNotifyAppCrash(int pid,int uid, String packageName) {}
    public void onEndOfActivityIdle(Context context, Intent idleIntent) {}
    public void onWakefulnessChanged(int wakefulness) {}
    public void addDuraSpeedService() {}
    public void startDuraSpeedService(Context context) {}
    public String onReadyToStartComponent(String packageName, int uid,
            String suppressReason, String className) {
        return null;
    }
    public boolean onBeforeStartProcessForStaticReceiver(String packageName) {
        return false;
    }
    public void addToSuppressRestartList(String packageName) {}
    public boolean notRemoveAlarm(String packageName) { return false; }

    /**
     * Enable ams log and running process activity thread log.
     * This function is used to enable ams log when reboot device.
     */
    public void enableAmsLog(ArrayList<ProcessRecord> lruProcesses) {}

    /**
     * Enable ams log and running process activity thread log.
     * This function is used for adb command.
     */
    public void enableAmsLog(PrintWriter pw, String[] args,
            int opti, ArrayList<ProcessRecord> lruProcesses) {}

    /**
     * Pre-launch application
     */
    public boolean preLaunchApplication(String callingPackage, Intent intent, String resolvedType,
            int startFlags) {
        return false;
    }

    /**
     * Add for enhance wtf.
     */
    public boolean IsBuildInApp() {
        return true;
    }

    public boolean checkAutoBootPermission(Context context, String packageName, int userId,
            ArrayList<ProcessRecord> runningProcess, int callingPid) {
        return true;
    }

    /**
     * Add for AGO project heavy loading performance.
     * Force stop the related 3rd apps to improve the performance.
     */
    public void forceStopRelatedApps(Context context, ProcessRecord app, ApplicationInfo appInfo,
            int userId, ArrayList<ProcessRecord> lruProcesses) {}

    /**
     * Add for AGO project heavy loading performance.
     * check if the related component need start,to void app A continue start app B
     * when B have alredy forced stop.
     */
    public boolean isComponentNeedsStart(String packageName, String suppressReason) {
        return true;
    }
}
