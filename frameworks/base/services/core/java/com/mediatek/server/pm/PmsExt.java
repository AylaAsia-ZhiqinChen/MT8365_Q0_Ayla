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
package com.mediatek.server.pm;

import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageInfoLite;
import android.content.pm.PackageManager;
import android.content.pm.PackageParser;
import android.content.pm.ResolveInfo;
import android.os.RemoteException;
import android.os.UserHandle;
import android.util.ArrayMap;

import com.android.server.pm.PackageManagerException;
import com.android.server.pm.PackageManagerService;
import com.android.server.pm.PackageSetting;
import com.android.server.pm.Settings;
import com.android.server.pm.UserManagerService;

import java.io.File;
import java.io.PrintWriter;
import java.util.List;

public class PmsExt {

    public static final int INDEX_CIP_FW = 1;
    public static final int INDEX_VENDOR_FW = 2;
    public static final int INDEX_VENDOR_PRIV = 3;
    public static final int INDEX_OP_APP = 4;
    public static final int INDEX_ROOT_PLUGIN = 5;
    public static final int INDEX_VENDOR_PLUGIN = 6;
    public static final int INDEX_CUSTOM_APP = 7;
    public static final int INDEX_CUSTOM_PLUGIN = 8;
    public static final int INDEX_RSC_OVERLAY = 9;
    public static final int INDEX_RSC_FW = 10;
    public static final int INDEX_RSC_PRIV = 11;
    public static final int INDEX_RSC_APP = 12;
    public static final int INDEX_RSC_PLUGIN = 13;

    public PmsExt() {
    }

    public void init(PackageManagerService pms, UserManagerService ums) {
    }

    public void scanDirLI(int ident, int defParseFlags, int defScanFlags,
            long currentTime) {
    }

    public void scanMoreDirLi(int defParseFlags, int defScanFlags) {
    }

    public void checkMtkResPkg(PackageParser.Package pkg)
            throws PackageManagerException {
    }

    public boolean needSkipScanning(PackageParser.Package pkg,
            PackageSetting updatedPkg, PackageSetting ps) {
        return false;
    }

    public boolean needSkipAppInfo(ApplicationInfo ai) {
        return false;
    }

    public void onPackageAdded(String packageName, PackageSetting pkgSetting, int userId) {
    }

    public void initBeforeScan() {
    }

    public void initAfterScan(ArrayMap<String, PackageSetting> settingsPackages) {
    }

    public int customizeInstallPkgFlags(int installFlags,
            PackageInfoLite pkgLite,
            ArrayMap<String, PackageSetting> settingsPackages, UserHandle user) {
        return installFlags;
    }

    public void updatePackageSettings(int userId, String pkgName,
            PackageParser.Package newPackage, PackageSetting ps,
            int[] allUsers, String installerPackageName) {
    }

    public int customizeDeletePkgFlags(int deleteFlags, String packageName) {
        return deleteFlags;
    }

    public int customizeDeletePkg(int[] users, String packageName,
            int versionCode, int delFlags) {
        return PackageManager.DELETE_SUCCEEDED;
    }

    public boolean dumpCmdHandle(String cmd, PrintWriter pw, String[] args,
            int opti) {
        return false;
    }

    public ApplicationInfo updateApplicationInfoForRemovable(
            ApplicationInfo oldAppInfo) {
        return oldAppInfo;
    }

    public ApplicationInfo updateApplicationInfoForRemovable(String nameForUid,
            ApplicationInfo oldAppInfo) {
        return oldAppInfo;
    }

    public ActivityInfo updateActivityInfoForRemovable(ActivityInfo info)
            throws RemoteException {
        return info;
    }

    public List<ResolveInfo> updateResolveInfoListForRemovable(
            List<ResolveInfo> apps) throws RemoteException {
        return apps;
    }

    public PackageInfo updatePackageInfoForRemovable(PackageInfo oldPkgInfo) {
        return oldPkgInfo;
    }

    public boolean isRemovableSysApp(String pkgName) {
        return false;
    }

    public boolean updateNativeLibDir(ApplicationInfo info, String codePath) {
        return false;
    }
}
