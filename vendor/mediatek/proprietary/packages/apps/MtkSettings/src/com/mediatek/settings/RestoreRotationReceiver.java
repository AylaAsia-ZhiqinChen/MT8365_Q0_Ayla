/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
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

package com.mediatek.settings;


import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.provider.Settings;
import android.util.Log;
import android.os.UserHandle;

import com.mediatek.provider.MtkSettingsExt;
//yuntian longyao add Ethernet settings
import java.io.FileReader;
import java.io.File;
import java.io.IOException;
import android.os.Handler;
import android.app.ActivityManager;
import java.lang.reflect.Method;

import java.net.InetAddress;
import android.net.EthernetManager;
import android.net.IpConfiguration;
import android.net.LinkAddress;
import android.net.StaticIpConfiguration;
import android.content.ContentResolver;
//yuntian longyao end Ethernet settings

public class RestoreRotationReceiver extends BroadcastReceiver {

    public static boolean sRestoreRetore = false;
    //yuntian longyao add
    private static boolean mAdbEnable = false;
    private static final String YUNTIAN_FIRST_RUN = "keyguard_first_run";
    //yuntian longyao end
    //yuntian longyao add Ethernet settings
    public static final String ETHERNET_ON = Settings.Global.ETHERNET_ON;
    public boolean isEthernetEnabled;
    private String rj45 = "Vendor=0b95 ProdID=772b";
    private StringBuilder sb;
    private Context mContext;
    public boolean isEthernetStaticIpEnabled;
    private StaticIpConfiguration mStaticIpConfiguration;
    private IpConfiguration mIpConfiguration;
    private EthernetManager mEthernetManager;
    public static final String ETHERNET_USE_STATIC_IP = Settings.Global.ETHERNET_STATIC_ON;
    private String[] mSettingNames = {
            Settings.Global.ETHERNET_STATIC_IP,
            Settings.Global.ETHERNET_STATIC_GATEWAY,
            Settings.Global.ETHERNET_STATIC_MASK,
            Settings.Global.ETHERNET_STATIC_DNS1,
            Settings.Global.ETHERNET_STATIC_DNS2
    };
    //yuntian longyao end Ethernet settings
    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();
        //yuntian longyao add Ethernet settings
        mContext = context;
        isEthernetEnabled = Settings.Global.getInt(context.getContentResolver(), ETHERNET_ON, 0) == 1 ? true : false;
        isEthernetStaticIpEnabled = Settings.Global.getInt(context.getContentResolver(), ETHERNET_USE_STATIC_IP, 0) != 0 ? true : false;
        //yuntian longyao end Ethernet settings
        Log.v("RestoreRotationReceiver_IPO", action);
        if (action.equals(Intent.ACTION_LOCKED_BOOT_COMPLETED)) {
            sRestoreRetore = Settings.System.getIntForUser(context
                    .getContentResolver(),
                    MtkSettingsExt.System.ACCELEROMETER_ROTATION_RESTORE,
                    0, UserHandle.USER_CURRENT) != 0;
            if (sRestoreRetore) {
                Settings.System.putIntForUser(context.getContentResolver(),
                        Settings.System.ACCELEROMETER_ROTATION, 1, UserHandle.USER_CURRENT);
                Settings.System.putIntForUser(context.getContentResolver(),
                        MtkSettingsExt.System.ACCELEROMETER_ROTATION_RESTORE, 0, UserHandle.USER_CURRENT);
            }
            //yuntian longyao add
            if (android.os.SystemProperties.getBoolean("persist.sys.user.adb.enabled", false)) {
                boolean isFirstRun = Settings.System.getInt(context.getContentResolver(), YUNTIAN_FIRST_RUN, 0) == 0;
                if (isFirstRun) {
                    Settings.System.putInt(context.getContentResolver(), YUNTIAN_FIRST_RUN, 1);
                    if ("user".equals(android.os.Build.TYPE)) {
                        mAdbEnable = Settings.Global.getInt(context.getContentResolver(),
                                Settings.Global.ADB_ENABLED, 0) != 0;
                        if (!mAdbEnable) {
                            Settings.Global.putInt(context.getContentResolver(), Settings.Global.ADB_ENABLED, 1);
                        }
                    }
                }
            }
            //yuntian longyao end
        } else if (Intent.ACTION_BOOT_COMPLETED.equals(action)) {
            if (android.os.SystemProperties.getBoolean("persist.sys.add.ethernet.settings", false)) {
                if (android.os.SystemProperties.get("persist.sys.exist.ethernet", "0").equals("0")) {
                    setEthernetEnabled(true);
                    handler.postDelayed(rGetEthernet, 2000);
                    handler.postDelayed(rSetEthernet, 5000);
                } else {
                    setEthernetEnabled(isEthernetEnabled);
                }
                setStaticIp();
            }
            //yuntian longyao end Ethernet settings
        }
    }

    private void setStaticIp() {
        if (isEthernetEnabled) {
            if (isEthernetStaticIpEnabled) {
                mIpConfiguration = new IpConfiguration();
                mStaticIpConfiguration = new StaticIpConfiguration();
                try {
                    mStaticIpConfiguration.ipAddress = new LinkAddress(InetAddress.getByName(Settings.Global.getString(mContext.getContentResolver(), mSettingNames[0])), 24);
                } catch (java.net.UnknownHostException e) {
                    return;
                }
                try {
                    mStaticIpConfiguration.gateway = InetAddress.getByName(Settings.Global.getString(mContext.getContentResolver(), mSettingNames[1]));
                } catch (java.net.UnknownHostException e) {
                    return;
                }
                mStaticIpConfiguration.domains = Settings.Global.getString(mContext.getContentResolver(), mSettingNames[2]);
                try {
                    mStaticIpConfiguration.dnsServers.add(InetAddress.getByName(Settings.Global.getString(mContext.getContentResolver(), mSettingNames[3])));
                } catch (java.net.UnknownHostException e) {
                    return;
                }
                try {
                    mStaticIpConfiguration.dnsServers.add(InetAddress.getByName(Settings.Global.getString(mContext.getContentResolver(), mSettingNames[4])));
                } catch (java.net.UnknownHostException e) {
                    return;
                }
                mIpConfiguration.ipAssignment = IpConfiguration.IpAssignment.STATIC;
                mIpConfiguration.proxySettings = IpConfiguration.ProxySettings.STATIC;
                mIpConfiguration.staticIpConfiguration = mStaticIpConfiguration;
                mEthernetManager = (EthernetManager) mContext.getSystemService(Context.ETHERNET_SERVICE);
                if (null != mEthernetManager && null != mIpConfiguration) {
                    mEthernetManager.setConfiguration("eth0", mIpConfiguration);
                }
            }
        }
    }

    private Handler handler = new Handler();
    private Runnable rGetEthernet = new Runnable() {
        @Override
        public void run() {
            sb = getUsbDevicesinfo();
        }
    };
    private Runnable rSetEthernet = new Runnable() {
        @Override
        public void run() {
            //Log.e("RestoreRotationReceiver", "getUsbDevicesinfo=" + sb.toString());
            if (!"".equals(sb.toString()) && sb.toString().contains(rj45)) {
                android.os.SystemProperties.set("persist.sys.exist.ethernet", "1");
                //Log.e("RestoreRotationReceiver", "exist rj45");
                setEthernetEnabled(isEthernetEnabled);
                //yuntian longyao add
                //Description:杀死工厂模式一次，让其重新加载配置文件
                closeFactorytest("com.android.factorytest");
                //yuntian longyao end
            } else {
                setEthernetEnabled(false);
            }
        }
    };

    //yuntian longyao add
    //Description:杀死工厂模式一次，让其重新加载配置文件
    private void closeFactorytest(String packageName) {
        try {
            ActivityManager mAm = (ActivityManager) mContext.getSystemService(Context.ACTIVITY_SERVICE);
            Method method = mAm.getClass().getMethod("forceStopPackage", String.class);
            method.setAccessible(true);
            method.invoke(mAm, packageName);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    //yuntian longyao end

    private static final String MAIN_USB_DEVICES_PATH = "/sys/kernel/debug/usb/devices";
    private StringBuilder getUsbDevicesinfo() {
        StringBuilder sb = new StringBuilder();
        File file = new File(MAIN_USB_DEVICES_PATH);
        FileReader fr = null;
        try {
            fr = new FileReader(file);
            int ch = fr.read();
            while (ch != -1) {
                sb.append((char) ch);
                ch = fr.read();
            }
        } catch (IOException e) {
            Log.e("RestoreRotationReceiver", "getUsbDevicesinfo=>error: ", e);
        } finally {
            try {
                if (fr != null) {
                    fr.close();
                }
            } catch (Exception e) {
            }
        }
        //Log.e("RestoreRotationReceiver", "getUsbDevicesinfo=>result: " + sb);
        return sb;
    }

    //yuntian longyao add Ethernet settings
    private static final String ETHERNET_PATH = "/sys/class/usb_net_gpio/usb_net_gpio";
    private boolean setEthernetEnabled(boolean enabled) {
        boolean flag = false;
        File file = new File(ETHERNET_PATH);
        java.io.FileWriter fr = null;
        try {
            fr = new java.io.FileWriter(file);
            if (enabled) {
                fr.write("1");
            } else {
                fr.write("0");
            }
            fr.close();
            fr = null;
            flag = true;
        } catch (IOException e) {
            Log.e("RestoreRotationReceiver", "setEthernetEnabled=>error: ", e);
            flag = false;
        } finally {
            try {
                if (fr != null) {
                    fr.close();
                }
            } catch (IOException e) {
            }
        }
        //Log.e("RestoreRotationReceiver", "setEthernetEnabled=>enabled" + enabled);
        return flag;
    }
    //yuntian longyao end Ethernet settings
}
