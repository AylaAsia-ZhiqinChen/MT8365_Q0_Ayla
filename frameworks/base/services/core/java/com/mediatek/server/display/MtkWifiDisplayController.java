/*
* Copyright (C) 2014 MediaTek Inc.
* Modification based on code covered by the mentioned copyright
* and/or permission notice(s).
*/
/*
 * Copyright (C) 2012 The Android Open Source Project
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

package com.mediatek.server.display;

import com.android.internal.util.DumpUtils;

import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.hardware.display.WifiDisplay;
import android.hardware.display.WifiDisplaySessionInfo;
import android.hardware.display.WifiDisplayStatus;
import android.media.RemoteDisplay;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.net.wifi.WpsInfo;
import android.net.wifi.p2p.WifiP2pConfig;
import android.net.wifi.p2p.WifiP2pDevice;
import android.net.wifi.p2p.WifiP2pDeviceList;
import android.net.wifi.p2p.WifiP2pGroup;
import android.net.wifi.p2p.WifiP2pGroupList;
import android.net.wifi.p2p.WifiP2pManager;
import android.net.wifi.p2p.WifiP2pWfdInfo;
import android.net.wifi.p2p.WifiP2pManager.ActionListener;
import android.net.wifi.p2p.WifiP2pManager.Channel;
import android.net.wifi.p2p.WifiP2pManager.GroupInfoListener;
import android.net.wifi.p2p.WifiP2pManager.PeerListListener;
import android.os.Handler;
import android.provider.Settings;
import android.util.Slog;
import android.view.Surface;

import java.io.PrintWriter;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.Objects;

///M:@{
import android.app.AlarmManager;
import android.app.AlertDialog;
import android.app.StatusBarManager;
import android.media.AudioManager;
import android.media.AudioManager.OnAudioFocusChangeListener;

import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;

import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.res.Resources;
import android.hardware.input.InputManager;
import android.media.AudioManager;
import android.media.AudioManager.OnAudioFocusChangeListener;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiManager.WifiLock;
import android.os.RemoteException;
import android.os.IBinder;
import android.os.ServiceManager;
import android.os.PowerManager;
import android.os.SystemClock;
import android.provider.Settings;
import android.util.DisplayMetrics;
import android.util.Slog;
import android.view.InputDevice;
import android.view.KeyCharacterMap;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Toast;
import android.os.SystemProperties;

import com.android.server.display.WifiDisplayController;
import com.mediatek.server.powerhal.PowerHalManager;
import com.mediatek.server.MtkSystemServiceFactory;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import android.os.UserHandle;
import com.android.internal.R;
import android.widget.CheckBox;
import android.widget.ProgressBar;
import android.widget.TextView;

/**
 * This class for extend aosp wfd which mtk will add some feature or modification.
 */
public class MtkWifiDisplayController {
    private static final String TAG = "MtkWifiDisplayController";
    private static boolean DEBUG = true;

    /// GO Intent.
    private static final String goIntent = SystemProperties.get(
        "wfd.source.go_intent",
        String.valueOf(WifiP2pConfig.MAX_GROUP_OWNER_INTENT - 1));

    /// M: MTK Power: FPSGO Mechanism
    private PowerHalManager mPowerHalManager =
                    MtkSystemServiceFactory.getInstance().makePowerHalManager();

    // Initialize in config.xml
    private int WFDCONTROLLER_DISPLAY_TOAST_TIME;
    private int WFDCONTROLLER_DISPLAY_NOTIFICATION_TIME;
    private int WFDCONTROLLER_DISPLAY_RESOLUTION;
    private int WFDCONTROLLER_DISPLAY_SECURE_OPTION;
    private int mResolution;
    private int mPrevResolution;
    private int mWifiP2pChannelId = -1;
    private boolean mWifiApConnected = false;
    private int mWifiApFreq = 0;
    private int mWifiNetworkId = -1;
    private String mWifiApSsid = null;

    // Power saving part.
    private int WFDCONTROLLER_DISPLAY_POWER_SAVING_OPTION;
    private int WFDCONTROLLER_DISPLAY_POWER_SAVING_DELAY;

    private static final int RECONNECT_RETRY_DELAY_MILLIS = 1000;
    private static final int RESCAN_RETRY_DELAY_MILLIS = 2000;
    private static final int CONNECTION_TIMEOUT_SECONDS = 30;

    private int mReConnection_Timeout_Remain_Seconds;
    private WifiP2pDevice mReConnectDevice;

    private final Context mContext;
    private final Handler mHandler;
//ifdef MTK_WFD_SINK_SUPPORT

    private static final int DEFAULT_CONTROL_PORT = 7236;
    private static final int MAX_THROUGHPUT = 50;
    /// M: Modify for speed up rtsp setup
    private static final int RTSP_TIMEOUT_SECONDS = 15 + CONNECTION_TIMEOUT_SECONDS;
    private static final int RTSP_TIMEOUT_SECONDS_CERT_MODE = 120;
    private static final int RTSP_SINK_TIMEOUT_SECONDS = 10;

    // ALPS00759126: keep wifi enabled when WFD connected
    private WifiLock mWifiLock;
    private PowerManager.WakeLock mWakeLock;
    private PowerManager.WakeLock mWakeLockSink;
    private WifiManager mWifiManager;
    private boolean mStopWifiScan = false;
    private static final long WIFI_SCAN_TIMER = 100 * 1000;
    // ALPS00812236: dismiss all dialogs when wifi p2p/wfd is disabled
    private final static int WFD_WIFIP2P_EXCLUDED_DIALOG = 1;
    private final static int WFD_HDMI_EXCLUDED_DIALOG_WFD_UPDATE = 2;
    private final static int WFD_HDMI_EXCLUDED_DIALOG_HDMI_UPDATE = 3;
    private final static int WFD_RECONNECT_DIALOG = 4;
    private final static int WFD_CHANGE_RESOLUTION_DIALOG = 5;
    private final static int WFD_SOUND_PATH_DIALOG = 6;
    private final static int WFD_WAIT_CONNECT_DIALOG = 7;
    public final static int WFD_CONFIRM_CONNECT_DIALOG = 8;
    private final static int WFD_BUILD_CONNECT_DIALOG = 9;
//endif MTK_WFD_SINK_SUPPORT
    private WifiDisplayController mController;

    // for HDMI/WFD exclude
    private Object mHdmiManager;
    private static final String HDMI_MANAGER_CLASS = "com.mediatek.hdmi.HdmiNative";
    private static final String HDMI_ENABLE = "persist.vendor.sys.hdmi_hidl.enable";
//ifdef MTK_WFD_SINK_SUPPORT
    WifiP2pWfdInfo mWfdInfo;
    // M:@{ Channel Conflict UX Support
    public static final String WFD_CHANNEL_CONFLICT_OCCURS = "com.mediatek.wifi.p2p.OP.channel";
    public static final String WFD_SINK_CHANNEL_CONFLICT_OCCURS =
        "com.mediatek.wifi.p2p.freq.conflict";
    private int mP2pOperFreq = 0;
    private int mNetworkId = -1;
    private boolean mDisplayApToast;
    ///M:@{ WFD Sink Support
    private boolean mSinkEnabled = false;

    enum SinkState {
        SINK_STATE_IDLE,
        SINK_STATE_WAITING_P2P_CONNECTION,
        SINK_STATE_WIFI_P2P_CONNECTED,
        SINK_STATE_WAITING_RTSP,
        SINK_STATE_RTSP_CONNECTED,
    }

    private SinkState mSinkState;
    private String mSinkDeviceName;
    private String mSinkMacAddress;
    private String mSinkIpAddress;
    private int mSinkPort;
    private WifiP2pGroup mSinkP2pGroup;
    private Surface mSinkSurface;
//    public static final String WFD_SINK_GC_REQUEST_CONNECT = "com.mediatek.wifi.p2p.GO.GCrequest.connect";
    private static final int WFD_SINK_IP_RETRY_FIRST_DELAY = 300;
    private static final int WFD_SINK_IP_RETRY_DELAY_MILLIS = 1000;
    private static final int WFD_SINK_IP_RETRY_COUNT = 50;
    private int mSinkIpRetryCount;

    private static final int WFD_SINK_DISCOVER_RETRY_DELAY_MILLIS = 100;
    private static final int WFD_SINK_DISCOVER_RETRY_COUNT = 5;
    private int mSinkDiscoverRetryCount;
    ///@M:{ Portrait WFD support
    public static final String WFD_PORTRAIT = "com.mediatek.wfd.portrait";

    private boolean mUserDecided;
    private final WifiP2pManager mWifiP2pManager;
    private final Channel mWifiP2pChannel;

    private boolean mWifiP2pEnabled;
    private boolean mWfdEnabled;
    private boolean mWfdEnabling;

    private boolean mIsConnected_OtherP2p;
    private boolean mIsConnecting_P2p_Rtsp;

    private boolean mAutoEnableWifi;
    private AlertDialog mWifiDirectExcludeDialog;
    private AlertDialog mReConnecteDialog;
    private AlertDialog mChangeResolutionDialog;
    private AlertDialog mSoundPathDialog;
    private AlertDialog mWaitConnectDialog;
    private AlertDialog mConfirmConnectDialog;
    private AlertDialog mBuildConnectDialog;

    StatusBarManager mStatusBarManager;

    // Created after the Wifi P2P network is connected.
    private RemoteDisplay mRemoteDisplay;

    private AudioManager mAudioManager;

    private boolean mIsWFDConnected;
//endif MTK_WFD_SINK_SUPPORT
    public MtkWifiDisplayController(       Context context,
        Handler handler,
        WifiDisplayController controller
//ifdef MTK_WFD_SINK_SUPPORT
        ,WifiP2pManager p2pmanager,
        Channel p2pchannel
        ) {
//endif
        mContext = context;
        mHandler = handler;
        mController = controller;

        //For WFD UIBC Source Latin Characters
        PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK, "UIBC Source");

        //For WFD Sink
        mStatusBarManager = (StatusBarManager) context.getSystemService(Context.STATUS_BAR_SERVICE);
        mWakeLockSink = pm.newWakeLock(PowerManager.FULL_WAKE_LOCK, "WFD Sink");
        mHdmiManager = getHdmiService();

        registerEMObserver();
//ifdef MTK_WFD_SINK_SUPPORT
        mWifiP2pManager = p2pmanager;
        mWifiP2pChannel = p2pchannel;
        mAudioManager = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
        mWfdInfo = new WifiP2pWfdInfo();
        mIsWFDConnected = false;
//endif
    }

    public WifiP2pConfig overWriteConfig(WifiP2pConfig oldConfig) {
        WifiP2pConfig config = new WifiP2pConfig(oldConfig);

        Slog.i(TAG, "oldConfig:" + oldConfig);

        config.groupOwnerIntent = Integer.valueOf(goIntent);

        // Slog.i(TAG, "Source go_intent:" + config.groupOwnerIntent);

        // The IOT devices only support connect and doesn't support invite.
        // Use temporary group to do p2p connection instead of persistent way
        // to avoid later connection through invitation.
        if (mController.mConnectingDevice.deviceName.contains("BRAVIA")) {
            Slog.i(TAG, "Force temporary group");
            config.netId = WifiP2pGroup.TEMPORARY_NET_ID;
        }

        Slog.i(TAG, "config:" + config);

        return config;
    }

    private boolean isForce720p() {
        String sPlatform = SystemProperties.get(
            "ro.mediatek.platform", "");

        switch (sPlatform) {
            case "MT6763":
            case "MT6765":
                Slog.d(TAG, "Platform (Force 720p): " + sPlatform);
                return true;

            default:
                Slog.d(TAG, "Platform: " + sPlatform);
                return false;
        }
    }

    public void setWFD(boolean enable) {

        Slog.d(TAG, "setWFD(), enable: " + enable);

        mPowerHalManager.setWFD(enable);
    }

    private int getResolutionIndex(int settingValue) {
        switch (settingValue) {
            case 0:
            case 3:
                return 5; // 720p/30fps
            case 1:
            case 2:
                return 7;  // 1080p/30fps
            default:
                return 5;  // 720p/30fps
        }
    }

    private static String describeWifiP2pDevice(WifiP2pDevice device) {
        return device != null ? device.toString().replace('\n', ',') : "null";
    }

    private final Runnable mReConnect = new Runnable() {
        @Override
        public void run() {
            // Slog.d(TAG, "mReConnect, run()");
            for (WifiP2pDevice device : mController.mAvailableWifiDisplayPeers) {
                if (DEBUG) {
                    Slog.d(TAG, "\t" + describeWifiP2pDevice(device));
                }

                if (device.deviceAddress.equals(mReConnectDevice.deviceAddress)) {
                    Slog.i(TAG, "connect() in mReConnect. Set mReConnecting as true");
                    mReConnectDevice = null;
                    ///mController.connect(device);
                    mController.requestConnect(device.deviceAddress);
                    return;
                }
            }

            mReConnection_Timeout_Remain_Seconds = mReConnection_Timeout_Remain_Seconds -
                (RECONNECT_RETRY_DELAY_MILLIS / 1000);
            if (mReConnection_Timeout_Remain_Seconds > 0) {
                // check scan result per RECONNECT_RETRY_DELAY_MILLIS ms
                Slog.i(TAG, "post mReconnect, s:" + mReConnection_Timeout_Remain_Seconds);
                mHandler.postDelayed(mReConnect, RECONNECT_RETRY_DELAY_MILLIS);
           } else {
                Slog.e(TAG, "reconnect timeout!");
                Toast.makeText(mContext, getMtkStringResourceId("wifi_display_disconnected")
                    , Toast.LENGTH_SHORT).show();
                mReConnectDevice = null;
                mReConnection_Timeout_Remain_Seconds = 0;
                mHandler.removeCallbacks(mReConnect);
                return;
            }
        }
    };

    private void handleResolutionChange() {
        int r;
        boolean doNotRemind = true;

        r = Settings.Global.getInt(
                mContext.getContentResolver(),
                getMtkSettingsExtGlobalSetting("WIFI_DISPLAY_RESOLUTION"), 0);

        if (r == mResolution) {
            return;
        }
        else {
            mPrevResolution = mResolution;
            mResolution = r;

            Slog.d(TAG, "handleResolutionChange(), resolution:" +
                        mPrevResolution + "->" + mResolution);
        }

        int idxModified = getResolutionIndex(mResolution);
        int idxOriginal = getResolutionIndex(mPrevResolution);

        if (idxModified == idxOriginal) {
            return;
        }

        Slog.d(TAG, "index:" + idxOriginal + "->" + idxModified + ", doNotRemind:" + doNotRemind);

        SystemProperties.set("vendor.media.wfd.video-format", String.valueOf(idxModified));


        // check if need to reconnect
        if (mController.mConnectedDevice != null || mController.mConnectingDevice != null) {
            Slog.d(TAG, "-- reconnect for resolution change --");

            // reconnect again
            if (null != mController.mConnectedDevice) {
                mReConnectDevice = mController.mConnectedDevice;
            }
            mController.requestDisconnect();
        }
    }

    public void checkReConnect() {
        if (null != mReConnectDevice) {
            Slog.i(TAG, "requestStartScan() for resolution change.");
            //scan first
            mController.requestStartScan();
            // check scan result per RECONNECT_RETRY_DELAY_MILLIS ms
            mReConnection_Timeout_Remain_Seconds = CONNECTION_TIMEOUT_SECONDS;
            mHandler.postDelayed(mReConnect, RECONNECT_RETRY_DELAY_MILLIS);
        }
    }

    private void initPortraitResolutionSupport() {

         // Default on
         Settings.Global.putInt(
                 mContext.getContentResolver(),
                 getMtkSettingsExtGlobalSetting("WIFI_DISPLAY_PORTRAIT_RESOLUTION"),
                 0);

         //set system property
         SystemProperties.set("vendor.media.wfd.portrait", String.valueOf(0));
    }

    private void handlePortraitResolutionSupportChange() {
         int value = Settings.Global.getInt(
                 mContext.getContentResolver(),
                 getMtkSettingsExtGlobalSetting("WIFI_DISPLAY_PORTRAIT_RESOLUTION"),
                 0);
         Slog.i(TAG, "handlePortraitResolutionSupportChange:" + value);

         //set system property
         SystemProperties.set("vendor.media.wfd.portrait", String.valueOf(value));
    }

    private void registerEMObserver() {

        DisplayMetrics dm = new DisplayMetrics();
        WindowManager wm = (WindowManager) mContext.getSystemService(Context.WINDOW_SERVICE);
        wm.getDefaultDisplay().getRealMetrics(dm);

        // Slog.i(TAG, "RealMetrics, Width = " + dm.widthPixels + ", Height = " + dm.heightPixels);

        int widthPixels = dm.widthPixels;
        int heightPixels = dm.heightPixels;

        // Init parameter
        WFDCONTROLLER_DISPLAY_RESOLUTION =
            mContext.getResources().getInteger(
                getMtkIntegerResourceId("wfd_display_default_resolution", -1));
        WFDCONTROLLER_DISPLAY_POWER_SAVING_OPTION =
            mContext.getResources().getInteger(
                getMtkIntegerResourceId("wfd_display_power_saving_option", 1));
        WFDCONTROLLER_DISPLAY_POWER_SAVING_DELAY =
            mContext.getResources().getInteger(
                getMtkIntegerResourceId("wfd_display_power_saving_delay", 10));
//ifdef MTK_WFD_SINK_SUPPPORT
        // Init parameter 2
        WFDCONTROLLER_DISPLAY_TOAST_TIME =
            mContext.getResources().getInteger(
                getMtkIntegerResourceId("wfd_display_toast_time",5));

        WFDCONTROLLER_DISPLAY_NOTIFICATION_TIME =
            mContext.getResources().getInteger(
                getMtkIntegerResourceId("wfd_display_notification_time",5));
//endif MTK_WFD_SINK_SUPPPORT
        Slog.d(TAG, "registerObserver() w:" + widthPixels +
                                         "h:" + heightPixels +
                                         "res:" + WFDCONTROLLER_DISPLAY_RESOLUTION +
                                         ",ps:" + WFDCONTROLLER_DISPLAY_POWER_SAVING_OPTION +
                                         ",psd:" + WFDCONTROLLER_DISPLAY_POWER_SAVING_DELAY);

        int r;
        r = Settings.Global.getInt(
                mContext.getContentResolver(),
                getMtkSettingsExtGlobalSetting("WIFI_DISPLAY_RESOLUTION"),
                -1);

        // boot up for the first time
        if (r == -1) {
            boolean bForce = isForce720p();

            if (WFDCONTROLLER_DISPLAY_RESOLUTION >= 0 &&
                WFDCONTROLLER_DISPLAY_RESOLUTION <= 3) {
                mPrevResolution = mResolution = WFDCONTROLLER_DISPLAY_RESOLUTION;
            } else if (bForce) {
                mPrevResolution = mResolution = 0;  // 0: 720p,30fps  (Menu is disabled)
            } else {
                // initialize resolution and frame rate
                if (widthPixels >= 1080 && heightPixels >= 1920) {
                    mPrevResolution = mResolution = 2;  // 2: 1080p,30fps (Menu is enabled)
                } else {
                    mPrevResolution = mResolution = 0;  // 0: 720p,30fps  (Menu is disabled)
                }
            }
        }
        else {
            if (r >= 0 && r <= 3) {
                // use the previous selection
                mPrevResolution = mResolution = r;
            } else {
                mPrevResolution = mResolution = 0; // 0: 720p,30fps  (Menu is disabled)
            }
        }

        int resolutionIndex = getResolutionIndex(mResolution);
        Slog.i(TAG, "mResolution:" + mResolution + ", resolutionIndex: " + resolutionIndex);

        SystemProperties.set("vendor.media.wfd.video-format", String.valueOf(resolutionIndex));

        Settings.Global.putInt(
                mContext.getContentResolver(),
                getMtkSettingsExtGlobalSetting("WIFI_DISPLAY_RESOLUTION"),
                mResolution);
        Settings.Global.putInt(
                mContext.getContentResolver(),
                getMtkSettingsExtGlobalSetting("WIFI_DISPLAY_POWER_SAVING_OPTION"),
                WFDCONTROLLER_DISPLAY_POWER_SAVING_OPTION);
        Settings.Global.putInt(
                mContext.getContentResolver(),
                getMtkSettingsExtGlobalSetting("WIFI_DISPLAY_POWER_SAVING_DELAY"),
                WFDCONTROLLER_DISPLAY_POWER_SAVING_DELAY);

        initPortraitResolutionSupport();

        // Register observer
        mContext.getContentResolver().registerContentObserver(
                Settings.Global.getUriFor(
                        getMtkSettingsExtGlobalSetting("WIFI_DISPLAY_RESOLUTION")),
                false, mObserver);

        mContext.getContentResolver().registerContentObserver(
                Settings.Global.getUriFor(
                        getMtkSettingsExtGlobalSetting("WIFI_DISPLAY_PORTRAIT_RESOLUTION")),
                false, mObserver);

        mContext.getContentResolver().registerContentObserver(
                Settings.Global.getUriFor(Settings.Global.WIFI_DISPLAY_ON),
                false, mObserver);

//ifdef MTK_WFD_SINK_SUPPPORT
                // Register observer
        mContext.getContentResolver().registerContentObserver(
                Settings.Global.getUriFor(
                        getMtkSettingsExtGlobalSetting("WIFI_DISPLAY_DISPLAY_TOAST_TIME")),
                false, mObserver);
        mContext.getContentResolver().registerContentObserver(
                Settings.Global.getUriFor(
                        getMtkSettingsExtGlobalSetting("WIFI_DISPLAY_DISPLAY_NOTIFICATION_TIME")),
                false, mObserver);
//endif MTK_WFD_SINK_SUPPPORT
    }

    private final ContentObserver mObserver = new ContentObserver(new Handler()) {
        @Override
        public void onChange(boolean selfChange, Uri uri) {

            if (selfChange) {
                return;
            }
//ifdef MTK_WFD_SINK_SUPPPORT
            WFDCONTROLLER_DISPLAY_TOAST_TIME = Settings.Global.getInt(
                    mContext.getContentResolver(),
                    getMtkSettingsExtGlobalSetting("WIFI_DISPLAY_DISPLAY_TOAST_TIME"),
                    20);

            WFDCONTROLLER_DISPLAY_NOTIFICATION_TIME = Settings.Global.getInt(
                    mContext.getContentResolver(),
                    getMtkSettingsExtGlobalSetting("WIFI_DISPLAY_DISPLAY_NOTIFICATION_TIME"),
                    120);
//endif MTK_WFD_SINK_SUPPPORT
            updateHDMIStatus();
            handleResolutionChange();
            handlePortraitResolutionSupportChange();
        }
    };

    private int getMtkStringResourceId(String name) {
        try {
            Class<?> rCls = Class.forName("com.mediatek.internal.R$string",
                                            false, ClassLoader.getSystemClassLoader());
            Field field = rCls.getField(name);
            field.setAccessible(true);
            return field.getInt(null);
        } catch (Exception  e) {
            Slog.e(TAG, "Cannot get MTK resource - " + e);
            return 0;
        }
    }

    private String getMtkSettingsExtGlobalSetting(String name) {
        try {
            Class<?> rCls = Class.forName("com.mediatek.provider.MtkSettingsExt$Global",
                                            false, ClassLoader.getSystemClassLoader());
            Field field = rCls.getField(name);
            field.setAccessible(true);
            return (String) field.get(rCls);
        } catch (Exception  e) {
            Slog.e(TAG, "Cannot get MTK settings - " + e);
            return "";
        }
    }

    private int getMtkIntegerResourceId(String name, int defaultVal) {
        try {
            Class<?> rCls = Class.forName("com.mediatek.internal.R$integer",
                                            false, ClassLoader.getSystemClassLoader());
            Field field = rCls.getField(name);
            field.setAccessible(true);
            return field.getInt(null);
        } catch (Exception  e) {
            Slog.e(TAG, "Cannot get MTK resource - " + e);
            return defaultVal;
        }
    }
//ifdef MTK_WFD_SINK_SUPPORT
    private int getMtkLayoutResourceId(String name) {
        try {
            Class<?> rCls = Class.forName("com.mediatek.internal.R$layout",
                                            false, ClassLoader.getSystemClassLoader());
            Field field = rCls.getField(name);
            field.setAccessible(true);
            return field.getInt(null);
        } catch (Exception  e) {
            Slog.e(TAG, "Cannot get MTK resource - " + e);
            return 0;
        }
    }

    private int getMtkIdResourceId(String name) {
        try {
            Class<?> rCls = Class.forName("com.mediatek.internal.R$id",
                                            false, ClassLoader.getSystemClassLoader());
            Field field = rCls.getField(name);
            field.setAccessible(true);
            return field.getInt(null);
        } catch (Exception  e) {
            Slog.e(TAG, "Cannot get MTK resource - " + e);
            return 0;
        }
    }

    private int getMtkDrawableResourceId(String name) {
        try {
            Class<?> rCls = Class.forName("com.mediatek.internal.R$drawable",
                                            false, ClassLoader.getSystemClassLoader());
            Field field = rCls.getField(name);
            field.setAccessible(true);
            return field.getInt(null);
        } catch (Exception  e) {
            Slog.e(TAG, "Cannot get MTK resource - " + e);
            return 0;
        }
    }
//endif MTK_WFD_SINK_SUPPORT

    public void turnOffHdmi() {
        if (null != mHdmiManager) {
            enableHdmi(mHdmiManager, false);
        }
    }

    public void turnOnHdmi() {
        if (null != mHdmiManager) {
            enableHdmi(mHdmiManager, true);
        }
    }

    private Object getHdmiService() {
        Object obj = null;
        try {
            Class<?> hdmiManagerClass = Class.forName(HDMI_MANAGER_CLASS, false, ClassLoader
                .getSystemClassLoader());
            Slog.d(TAG, "getHdmiService, hdmiManagerClass = " + hdmiManagerClass);
            Class<?> paraClass[] = {};
            Method method = hdmiManagerClass.getDeclaredMethod("getInstance", paraClass);
            method.setAccessible(true);
            Object noObject[] = {};
            obj = method.invoke(hdmiManagerClass, noObject);
            Slog.d(TAG, "getHdmiService, obj = " + obj);
        } catch (Exception e) {
            Slog.d(TAG, "getHdmiService, e = " + e);
            obj = null;
        }
        Slog.d(TAG, "getHdmiService, obj = " + obj);
        return obj;
    }

    public void enableHdmi(Object instance, boolean check) {
        try {
            Class<?> hdmiManagerClass = Class.forName(HDMI_MANAGER_CLASS, false, ClassLoader
                    .getSystemClassLoader());
            Class<?> paraClass[] = { boolean.class };
            Method enableHdmi = hdmiManagerClass.getDeclaredMethod("enableHdmi", paraClass);
            enableHdmi.setAccessible(true);
            enableHdmi.invoke(instance, check);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void updateHDMIStatus() {
        boolean HDMIOn = false;
        boolean wfdOn = Settings.Global.getInt(mContext.getContentResolver(),
                Settings.Global.WIFI_DISPLAY_ON, 0) != 0;
        if (!("".equals(SystemProperties.get("ro.vendor.mtk_tb_hdmi")))) {
                HDMIOn = (SystemProperties.getInt(HDMI_ENABLE, 0) == 1);
            }
        if (true == wfdOn && true == HDMIOn) {
            Slog.d(TAG, "When HDMI is on and turn on WFD --> turn off HDMI directly");
            Toast.makeText(mContext, "WFD is on, so trun off hdmi", Toast.LENGTH_SHORT).show();
            turnOffHdmi();
        }
    }
//ifdef MTK_WFD_SINK_SUPPORT
    ///M:@{
    private void resetWfdInfo() {
        mWfdInfo.setWfdEnabled(false);
        mWfdInfo.setDeviceType(WifiP2pWfdInfo.WFD_SOURCE);
        mWfdInfo.setSessionAvailable(false);
		mWfdInfo.setUibcSupported(false);
        mWfdInfo.setContentProtected(false);
    }
    private void updateWfdInfo(boolean enable)
    {
        Slog.i(TAG, "updateWfdInfo(), enable:" + enable + ",mWfdEnabling:" + mWfdEnabling);

        ///M:@{
        //WifiP2pWfdInfo wfdInfo = new WifiP2pWfdInfo();
        resetWfdInfo();

        if (!enable) {

            mWfdInfo.setWfdEnabled(false);
            mWifiP2pManager.setWFDInfo(mWifiP2pChannel, mWfdInfo, new ActionListener() {
                    @Override
                    public void onSuccess() {
                        if (DEBUG) {
                            Slog.d(TAG, "Successfully set WFD info.");
                        }
                    }

                    @Override
                    public void onFailure(int reason) {
                        if (DEBUG) {
                            Slog.d(TAG, "Failed to set WFD info with reason " + reason + ".");
                        }
                    }
             });
        } else {

            mWfdInfo.setWfdEnabled(true);

            ///M:@{ WFD Sink Support
            if (SystemProperties.get("ro.vendor.mtk_wfd_sink_support").equals("1")  && mSinkEnabled) {
                mWfdInfo.setDeviceType(WifiP2pWfdInfo.PRIMARY_SINK);
            } else {
                mWfdInfo.setDeviceType(WifiP2pWfdInfo.WFD_SOURCE);
            }

            Slog.i(TAG, "Set session available as true");
            mWfdInfo.setSessionAvailable(true);
            mWfdInfo.setControlPort(DEFAULT_CONTROL_PORT);
            mWfdInfo.setMaxThroughput(MAX_THROUGHPUT);
            if (SystemProperties.get("ro.vendor.mtk_wfd_sink_support").equals("1")  && mSinkEnabled) {
                if (SystemProperties.get("ro.vendor.mtk_wfd_sink_uibc_support").equals("1")) {
                    mWfdInfo.setUibcSupported(true);
                } else {
                    mWfdInfo.setUibcSupported(false);
                }
            } else {
                mWfdInfo.setUibcSupported(true);
            }
           //M: ALPS00799275: CP Support bit
            if (SystemProperties.get("ro.vendor.mtk_wfd_hdcp_tx_support").equals("1") ||
                SystemProperties.get("ro.vendor.mtk_dx_hdcp_support").equals("1") ||
                SystemProperties.get("ro.vendor.mtk_wfd_hdcp_rx_support").equals("1")) {
                mWfdInfo.setContentProtected(true);
            }

            Slog.i(TAG, "HDCP Tx support? " + (SystemProperties.get("ro.vendor.mtk_wfd_hdcp_tx_support").equals("1") || SystemProperties.get("ro.vendor.mtk_dx_hdcp_support").equals("1")) + ", our wfd info: " + mWfdInfo);
            Slog.i(TAG, "HDCP Rx support? " + SystemProperties.get("ro.vendor.mtk_wfd_hdcp_rx_support").equals("1") + ", our wfd info: " + mWfdInfo);


            if (mWfdEnabling) {

                mWifiP2pManager.setWFDInfo(mWifiP2pChannel, mWfdInfo, new ActionListener() {
                    @Override
                    public void onSuccess() {

                        Slog.d(TAG, "Successfully set WFD info.");

                        if (mWfdEnabling) {
                            mWfdEnabling = false;
                            mWfdEnabled = true;
                            //reportFeatureState();

                            ///M:@{
                            if (SystemProperties.get("ro.vendor.mtk_wfd_support").equals("1") &&
                                mAutoEnableWifi == true) {

                                mAutoEnableWifi = false;
                                Slog.d(TAG, "scan after enable wifi automatically.");
                            }
                            ///@}

                            mController.updateScanState();
                        }
                    }

                    @Override
                    public void onFailure(int reason) {

                        Slog.d(TAG, "Failed to set WFD info with reason " + reason + ".");

                        mWfdEnabling = false;
                    }
                });

            } else {


                mWifiP2pManager.setWFDInfo(mWifiP2pChannel, mWfdInfo, null);
            }

        }


    }
    private void stopWifiScan(boolean ifStop) {
        if (mStopWifiScan != ifStop) {
            Slog.i(TAG, "stopWifiScan()," + ifStop);
            try {
                // Get WifiInjector
                Method method = Class.forName(
                    "com.android.server.wifi.WifiInjector", false,
                        this.getClass().getClassLoader()).getDeclaredMethod("getInstance");
                method.setAccessible(true);
                Object wi = method.invoke(null);
                // Get WifiStatemachine
                Method method2 = wi.getClass().getDeclaredMethod("getClientModeImpl");
                method2.setAccessible(true);
                Object wsm = method2.invoke(wi);
                // Get WifiConnectivityManager
                Field fieldWcm = wsm.getClass().getDeclaredField("mWifiConnectivityManager");
                fieldWcm.setAccessible(true);
                Object wcm = fieldWcm.get(wsm);
                // Execute WifiConnectivityManager.enable() API
                Method method1 = wcm.getClass().getDeclaredMethod("enable", boolean.class);
                method1.setAccessible(true);
                method1.invoke(wcm, !ifStop);
            } catch (ReflectiveOperationException e) {
                e.printStackTrace();
            }
            mStopWifiScan = ifStop;
        }
    }

    public void requestEnableSink(boolean enable) {
        Slog.i(TAG, Thread.currentThread().getStackTrace()[2].getMethodName() +
            ",enable = " + enable + ", option = " +
            SystemProperties.get("ro.vendor.mtk_wfd_sink_support").equals("1")  +
            ", WfdEnabled = " + mWfdEnabled);//
// + ",Connected = " + mIsWFDConnected
        if (SystemProperties.get("ro.vendor.mtk_wfd_sink_support").equals("1")) {
            if (mSinkEnabled == enable/* || mIsWFDConnected*/) {
                return;
            }

            // Dialog displays when requestWaitConnection() is invoked
            if (enable && mIsConnected_OtherP2p) {
                Slog.i(TAG, "OtherP2P is connected! Only set variable. Ignore !");
                mSinkEnabled = enable;
                enterSinkState(SinkState.SINK_STATE_IDLE);
                return;
            }

            // stop or resume wifi scan
            stopWifiScan(enable);

            // sink: stop source action: scan
            if (enable == true) {
                mController.requestStopScan();
            }

            mSinkEnabled = enable;

            // update WFD device type to WIFI P2P FW
            updateWfdInfo(true);

            //source
            if (mSinkEnabled == false) {
                mController.requestStartScan();
            }
            // sink
            else {
                enterSinkState(SinkState.SINK_STATE_IDLE);
            }
        }
    }
    public void requestWaitConnection(Surface surface) {
        Slog.i(TAG, Thread.currentThread().getStackTrace()[2].getMethodName() + ", mSinkState:" + mSinkState);

        if (!isSinkState(SinkState.SINK_STATE_IDLE)) {
            Slog.i(TAG, "State is wrong! Ignore the request !");
            return;
        }

        if (mIsConnected_OtherP2p) {
            Slog.i(TAG, "OtherP2P is connected! Show dialog!");

            mHandler.post(new Runnable() {
                @Override
                public void run() {
                    notifyDisplayConnecting();
                }
            });

            showDialog(WFD_WIFIP2P_EXCLUDED_DIALOG);
            return;
        }

        mSinkSurface = surface;
        mIsWFDConnected = false;

        // Reset retry count;
        mSinkDiscoverRetryCount = WFD_SINK_DISCOVER_RETRY_COUNT;
        startWaitConnection();

        setSinkMiracastMode();

        enterSinkState(SinkState.SINK_STATE_WAITING_P2P_CONNECTION);

        mHandler.post(new Runnable() {
            @Override
            public void run() {
                if (isSinkState(SinkState.SINK_STATE_WAITING_P2P_CONNECTION)) {
                    notifyDisplayConnecting();
                }
            }
        });
    }
    public synchronized void disconnectWfdSink(){
        Slog.i(TAG, Thread.currentThread().getStackTrace()[2].getMethodName() +
            ", SinkState = " + mSinkState);

        if (isSinkState(SinkState.SINK_STATE_WAITING_P2P_CONNECTION) ||
            isSinkState(SinkState.SINK_STATE_WIFI_P2P_CONNECTED)) {

            mHandler.removeCallbacks(mGetSinkIpAddr);
            mHandler.removeCallbacks(mSinkDiscover);

            //if cancel building connection,p2p group state unkown,
            //cancelconnect to nego the group
            if (isSinkState(SinkState.SINK_STATE_WAITING_P2P_CONNECTION)){
                Slog.i(TAG,"WAITING_P2P_CONNECTION cancelConnect");
                //if cancel make disconnect
                //handlepsink2pconnection should not do this again,reset state first
                enterSinkState(SinkState.SINK_STATE_IDLE);

                mWifiP2pManager.cancelConnect(mWifiP2pChannel, new ActionListener() {
                    @Override
                    public void onSuccess() {
                        Slog.i(TAG, "Canceled connection success");
                        next();
                    }

                    @Override
                    public void onFailure(int reason) {
                        Slog.i(TAG, "Failed to cancel connection,remove group + delete group");
                        next();
                    }

                    private void next(){
                        if (null != mWifiP2pManager) {
                            mController.stopPeerDiscovery();
                            // Remove persistent group
                            deletePersistentGroup_ex();

                            updateIfSinkConnected(false);
                            mWifiP2pManager.setMiracastMode(WifiP2pManager.MIRACAST_DISABLED);
                        }
                    }

                });
                mHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        // Dismiss dialog
                        Slog.i(TAG, "Dismiss BuildConnectDialog");
                        dismissDialogDetail(mBuildConnectDialog);
                    }
                });
                return;
            }
            // Disconnect Wi-Fi P2P
            //remove group when p2p connected and waiting connection(p2p group may be formed when cancel during building connetion)
            //if (isSinkState(SinkState.SINK_STATE_WIFI_P2P_CONNECTED)){
            Slog.i(TAG, "Remove P2P group");
            mWifiP2pManager.removeGroup(mWifiP2pChannel, null);
            //}

            mController.stopPeerDiscovery();
            Slog.i(TAG, "Disconnected from WFD sink (P2P).");

            // Remove persistent group
            deletePersistentGroup();

            enterSinkState(SinkState.SINK_STATE_IDLE);

            updateIfSinkConnected(false);

            mWifiP2pManager.setMiracastMode(WifiP2pManager.MIRACAST_DISABLED);

            mHandler.post(new Runnable() {
                @Override
                public void run() {
                    Slog.d(TAG, "[Sink] callback onDisplayDisconnected()");
                    mController.getListener().onDisplayDisconnected();
                }
            });

        } else if (isSinkState(SinkState.SINK_STATE_WAITING_RTSP) ||
                   isSinkState(SinkState.SINK_STATE_RTSP_CONNECTED)) {

            if (mRemoteDisplay != null) {

                Slog.i(TAG, "before dispose()");
                mRemoteDisplay.dispose();
                Slog.i(TAG, "after dispose()");
            }

            mHandler.removeCallbacks(mRtspSinkTimeout);

            enterSinkState(SinkState.SINK_STATE_WIFI_P2P_CONNECTED);
            mHandler.post(new Runnable() {
                @Override
                public void run() {
                    disconnectWfdSink();
                }
            });

        }

        //Reset parameter
        mRemoteDisplay = null;
        mSinkDeviceName = null;
        mSinkMacAddress = null;
        mSinkPort = 0;
        mSinkIpAddress = null;
        mSinkSurface = null;

        mHandler.post(new Runnable() {
            @Override
            public void run() {
                // Dismiss dialog
                dismissDialogDetail(mWaitConnectDialog);
                dismissDialogDetail(mConfirmConnectDialog);
                dismissDialogDetail(mBuildConnectDialog);

                // Dismiss WiFi direct Exlude dialog and resume active Display state
                if (null != mWifiDirectExcludeDialog &&
                    mWifiDirectExcludeDialog.isShowing()) {
                    chooseNo_WifiDirectExcludeDialog();
                }
                dismissDialogDetail(mWifiDirectExcludeDialog);
            }
        });
    }
    private void deletePersistentGroup_ex(){
        Slog.i(TAG, "deletePersistentGroup_ex channel:"+mWifiP2pChannel);
        if (mSinkP2pGroup != null) {
            Slog.d(TAG, "deletePersistentGroup_ex mSinkP2pGroup!=null ");

            if (mSinkP2pGroup.getNetworkId() >= 0) {
                mWifiP2pManager.deletePersistentGroup(
                    mWifiP2pChannel, mSinkP2pGroup.getNetworkId(), null);
            }

            mSinkP2pGroup = null;
            return;
        }
        mWifiP2pManager.requestGroupInfo(mWifiP2pChannel,
            new WifiP2pManager.GroupInfoListener() {

                public void onGroupInfoAvailable(WifiP2pGroup group) {
                    Slog.i(TAG, "deletePersistentGroup_ex onGroupInfoAvailable(), mSinkState:" + mSinkState);

                    // For error handling
                    if (group == null) {
                        Slog.i(TAG, "group is null !!!");
                    }else if (group.getNetworkId() >= 0) {

                        Slog.i(TAG, "request requestGroupInfo cb deletePersistentGroup");
                        mWifiP2pManager.deletePersistentGroup(
                            mWifiP2pChannel, group.getNetworkId(), null);

                        Slog.i(TAG, "request requestGroupInfo cb removeGroup");
                        mWifiP2pManager.removeGroup(mWifiP2pChannel, null);

                    }

                    mHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            Slog.d(TAG, "[Sink] callback onDisplayDisconnected()");
                            mController.getListener().onDisplayDisconnected();
                        }
                    });
               }
            }
        );
    }
    private void removeSpecificPersistentGroup(){
        final WifiP2pDevice targetDevice = (mController.mConnectingDevice != null) ?
                 mController.mConnectingDevice : mController.mConnectedDevice;

        // The IOT devices which only support connect and doesn't support invite.
        // 1. Bravia
        if (targetDevice == null || !targetDevice.deviceName.contains("BRAVIA")) {
            return;
        }

        Slog.d(TAG, "removeSpecificPersistentGroup");

        mWifiP2pManager.requestPersistentGroupInfo(
            mWifiP2pChannel, new WifiP2pManager.PersistentGroupInfoListener() {

                public void onPersistentGroupInfoAvailable(WifiP2pGroupList groups) {
                    Slog.d(TAG, "onPersistentGroupInfoAvailable()");
                    for (WifiP2pGroup g : groups.getGroupList()) {

                        if (targetDevice.deviceAddress.equalsIgnoreCase(
                                g.getOwner().deviceAddress)) {

                            Slog.d(TAG, "deletePersistentGroup(), net id:" + g.getNetworkId());
                            mWifiP2pManager.deletePersistentGroup(
                                mWifiP2pChannel, g.getNetworkId(), null);
                        }
                    }
                }
            }
        );
    }
    private void deletePersistentGroup() {
        Slog.d(TAG, "deletePersistentGroup");

        if (mSinkP2pGroup != null) {
            Slog.d(TAG, "mSinkP2pGroup network id: " + mSinkP2pGroup.getNetworkId());

            if (mSinkP2pGroup.getNetworkId() >= 0) {
                mWifiP2pManager.deletePersistentGroup(
                    mWifiP2pChannel, mSinkP2pGroup.getNetworkId(), null);
            }

            mSinkP2pGroup = null;
        }
    }
    public void handleSinkP2PConnection(NetworkInfo networkInfo) {
        Slog.i(TAG, "handleSinkP2PConnection(), sinkState:" + mSinkState);
        if (null != mWifiP2pManager && networkInfo.isConnected()) {

            // Do state check first
            if (!isSinkState(SinkState.SINK_STATE_WAITING_P2P_CONNECTION)) {
                return;
            }

            mWifiP2pManager.requestGroupInfo(mWifiP2pChannel,
                new WifiP2pManager.GroupInfoListener() {

                public void onGroupInfoAvailable(WifiP2pGroup group) {
                    Slog.i(TAG, "onGroupInfoAvailable(), mSinkState:" + mSinkState);

                    // Do state check first
                    if (!isSinkState(SinkState.SINK_STATE_WAITING_P2P_CONNECTION)) {
                        return;
                    }

                    // For error handling
                    if (group == null) {
                        Slog.i(TAG, "Error: group is null !!!");
                        return;
                    }

                    mSinkP2pGroup = group;

                    //Get valid WFD client in the group
                    boolean found = false;

                    // Check if the group owner is me
                    if (group.getOwner().deviceAddress.equals(mController.getP2pDevice().deviceAddress)) {
                        Slog.i(TAG, "group owner is my self !");

                        for (WifiP2pDevice c : group.getClientList()) {

                            Slog.i(TAG, "Client device:" + c);

                            if (isWifiDisplaySource(c) && mSinkDeviceName.equals(c.deviceName)) {

                                mSinkMacAddress = c.deviceAddress;
                                mSinkPort = c.wfdInfo.getControlPort();
                                Slog.i(TAG, "Found ! Sink name:" + mSinkDeviceName +
                                    ",mac address:" + mSinkMacAddress + ",port:" + mSinkPort);

                                found = true;
                                break;
                            }
                        }

                    } else {
                        Slog.i(TAG, "group owner is not my self ! So I am GC.");
                        mSinkMacAddress = group.getOwner().deviceAddress;
                        mSinkPort = group.getOwner().wfdInfo.getControlPort();

                        Slog.i(TAG, "Sink name:" + mSinkDeviceName +
                            ",mac address:" + mSinkMacAddress + ",port:" + mSinkPort);
                        found = true;

                    }

                    if (found) {
                        mSinkIpRetryCount = WFD_SINK_IP_RETRY_COUNT;
                        enterSinkState(SinkState.SINK_STATE_WIFI_P2P_CONNECTED);
                        mHandler.postDelayed(mGetSinkIpAddr, WFD_SINK_IP_RETRY_FIRST_DELAY);
                    }
                }
            });
        } else {

            // Do state check first
            if (isSinkState(SinkState.SINK_STATE_WAITING_P2P_CONNECTION) ||
                isSinkState(SinkState.SINK_STATE_WIFI_P2P_CONNECTED)) {

                // Something wrong on GO group. So remove group.
                disconnectWfdSink();
            }
        }
    }

    public boolean getIfSinkEnabled() {
        Slog.i(TAG, Thread.currentThread().getStackTrace()[2].getMethodName() +
            ",enable = " + mSinkEnabled);

        return mSinkEnabled;
    }
    private void startWaitConnection() {
        Slog.i(TAG, Thread.currentThread().getStackTrace()[2].getMethodName() +
            ", mSinkState:" + mSinkState + ", retryCount:" + mSinkDiscoverRetryCount);
        mWifiP2pManager.discoverPeers(mWifiP2pChannel, new ActionListener() {
            @Override
            public void onSuccess() {
                if (isSinkState(SinkState.SINK_STATE_WAITING_P2P_CONNECTION)) {
                    Slog.d(TAG, "[sink] succeed for discoverPeers()");

                    // Show progress dialog
                    showDialog(WFD_WAIT_CONNECT_DIALOG);
                }
            }

            @Override
            public void onFailure(int reason) {
                if (isSinkState(SinkState.SINK_STATE_WAITING_P2P_CONNECTION)) {
                    Slog.e(TAG, "[sink] failed for discoverPeers(), reason:" + reason +
                        ", retryCount:" + mSinkDiscoverRetryCount);

                    if (reason == WifiP2pManager.BUSY &&
                        mSinkDiscoverRetryCount > 0) {
                        mSinkDiscoverRetryCount --;
                        mHandler.postDelayed(
                            mSinkDiscover, WFD_SINK_DISCOVER_RETRY_DELAY_MILLIS);
                        return;
                    }

                    enterSinkState(SinkState.SINK_STATE_IDLE);
                    Slog.d(TAG, "[sink] callback onDisplayConnectionFailed()");
                    mController.getListener().onDisplayConnectionFailed();
                }
            }
        });
    }
    private boolean isWifiDisplaySource(WifiP2pDevice device) {
        boolean result = device.wfdInfo != null
                && device.wfdInfo.isWfdEnabled()
                && device.wfdInfo.isSessionAvailable()  ///May have IOT issue
                && isSourceDeviceType(device.wfdInfo.getDeviceType());  ///May have IOT issue

        if (!result) {
            Slog.e(TAG, "This is not WFD source device !!!!!!");
        }
        return result;
    }
    private boolean isSourceDeviceType(int deviceType) {
        return deviceType == WifiP2pWfdInfo.WFD_SOURCE
                || deviceType == WifiP2pWfdInfo.SOURCE_OR_PRIMARY_SINK;
    }
    public void resetReconnectVariable() {
        Slog.i(TAG, Thread.currentThread().getStackTrace()[2].getMethodName());
        mIsConnected_OtherP2p = true;
        //mReScanning = false;
        mReConnectDevice = null;
        mReConnection_Timeout_Remain_Seconds = 0;
        //mReConnecting = false;
        mHandler.removeCallbacks(mReConnect);
    }
    public void requestSuspendDisplay(boolean suspend, Surface surface) {
        Slog.i(TAG, Thread.currentThread().getStackTrace()[2].getMethodName() + ",suspend = " + suspend);
        mSinkSurface = surface;

        if (isSinkState(SinkState.SINK_STATE_RTSP_CONNECTED)) {
            if (mRemoteDisplay!= null) {
                mRemoteDisplay.suspendDisplay(suspend, surface);
            }

            blockNotificationList(!suspend);
        } else {
            Slog.i(TAG, "State is wrong !!!, SinkState:" + mSinkState);
        }

    }
    private final Runnable mSinkDiscover = new Runnable() {
        @Override
        public void run() {

            Slog.d(TAG, "mSinkDiscover run(), count:" + mSinkDiscoverRetryCount);

            if (!isSinkState(SinkState.SINK_STATE_WAITING_P2P_CONNECTION)) {
                Slog.d(TAG, "mSinkState:(" + mSinkState + ") is wrong !");
                return;
            }

            startWaitConnection();
        }
    };
    private final Runnable mGetSinkIpAddr = new Runnable() {
        @Override
        public void run() {

            Slog.d(TAG, "mGetSinkIpAddr run(), count:" + mSinkIpRetryCount);

            if (!isSinkState(SinkState.SINK_STATE_WIFI_P2P_CONNECTED)) {
                Slog.d(TAG, "mSinkState:(" + mSinkState + ") is wrong !");
                return;
            }

            mSinkIpAddress = mWifiP2pManager.getPeerIpAddress(mSinkMacAddress);

            if (mSinkIpAddress == null) {
                if (mSinkIpRetryCount > 0) {
                    mSinkIpRetryCount --;
                    mHandler.postDelayed(mGetSinkIpAddr, WFD_SINK_IP_RETRY_DELAY_MILLIS);
                } else {
                    // ToDo: Check why fail
                    Slog.d(TAG, "mGetSinkIpAddr FAIL !!!!!!");
                }
            } else {

                //Add port number
                mSinkIpAddress += ":" + mSinkPort;

                Slog.i(TAG, "sink Ip address = " + mSinkIpAddress);

                //connect RTSP
                connectRtsp();
            }
        }
    };
    public void updateDeviceName(String devicename){
        mSinkDeviceName = devicename;
        Slog.i(TAG, "updateDeviceName:" + mSinkDeviceName);
    }
    private void connectRtsp() {
        Slog.d(TAG, "connectRtsp(), mSinkState:" + mSinkState);

        mRemoteDisplay = RemoteDisplay.connect(mSinkIpAddress, mSinkSurface, new RemoteDisplay.Listener() {
                @Override
                public void onDisplayConnected(Surface surface,
                        int width, int height, int flags, int session)  {

                    Slog.i(TAG, "Opened RTSP connection! w:" + width + ",h:" + height);

                    dismissDialogDetail(mBuildConnectDialog);
                    if (!isSinkState(SinkState.SINK_STATE_WAITING_RTSP)) {//may be disconnected by user before here
                        Slog.i(TAG, "Opened RTSP connection wrong state return");
                        return;
                    }
                    enterSinkState(SinkState.SINK_STATE_RTSP_CONNECTED);
                    mHandler.removeCallbacks(mRtspSinkTimeout);

                    //TODO: define multi-language string
                    WifiDisplay display = new WifiDisplay(
                        mSinkMacAddress, mSinkDeviceName, null, true, true, false);

                    ///@M:{ Portrait WFD support
                    if (width < height) {
                        sendPortraitIntent();
                    }
                    //} @M

                    Slog.d(TAG, "[sink] callback onDisplayConnected(), addr:" + mSinkMacAddress +
                           ", name:" + mSinkDeviceName);

                    updateIfSinkConnected(true);
                    mController.getListener().onDisplayConnected(display, null, 0, 0, 0);
                }

                @Override
                public void onDisplayDisconnected() {
                    Slog.i(TAG, "Closed RTSP connection! mSinkState:" + mSinkState);

                    if (!isSinkState(SinkState.SINK_STATE_WAITING_RTSP) &&
                        !isSinkState(SinkState.SINK_STATE_RTSP_CONNECTED)) {
                        return;
                    }

                    dismissDialogDetail(mBuildConnectDialog);
                    mHandler.removeCallbacks(mRtspSinkTimeout);

                    disconnectWfdSink();
                }

                @Override
                public void onDisplayError(int error) {
                    Slog.i(TAG, "Lost RTSP connection! mSinkState:" + mSinkState);

                    if (!isSinkState(SinkState.SINK_STATE_WAITING_RTSP) &&
                        !isSinkState(SinkState.SINK_STATE_RTSP_CONNECTED)) {
                        return;
                    }

                    dismissDialogDetail(mBuildConnectDialog);
                    mHandler.removeCallbacks(mRtspSinkTimeout);

                    disconnectWfdSink();
                }

                @Override
                public void onDisplayKeyEvent(int keyCode, int flags) {
                    Slog.d(TAG, "onDisplayKeyEvent:");
                }

                @Override
                public void onDisplayGenericMsgEvent(int event){

                }

            }, mHandler);

        enterSinkState(SinkState.SINK_STATE_WAITING_RTSP);

        // Use extended timeout value for certification, as some tests require user inputs
        int rtspTimeout = mController.getCertMode()?
                RTSP_TIMEOUT_SECONDS_CERT_MODE : RTSP_SINK_TIMEOUT_SECONDS;

        mHandler.postDelayed(mRtspSinkTimeout, rtspTimeout * 1000);

    }
    private void enterSinkState(SinkState state) {
        Slog.i(TAG, "enterSinkState()," + mSinkState + "->" + state);
        mSinkState = state;
    }

    private boolean isSinkState(SinkState state) {
        return  (mSinkState == state) ? true : false;
    }

    public boolean isSinkWaitingP2pConnectionState() {
        return  (mSinkState == SinkState.SINK_STATE_WAITING_P2P_CONNECTION) ? true : false;
    }

    private void updateIfSinkConnected(boolean connected) {
        if (mIsWFDConnected == connected) {
            return;
        }
        mIsWFDConnected = connected;

        // No sink notification list when connected
        //blockNotificationList(connected);

        // Prevent other source from searching to this sink device
        Slog.i(TAG, "Set session available as " + !connected);
        mWfdInfo.setSessionAvailable(!connected);
        mWifiP2pManager.setWFDInfo(mWifiP2pChannel, mWfdInfo, null);

        if (mWakeLockSink != null) {
            if (connected) {
                mWakeLockSink.acquire();
            } else {
                mWakeLockSink.release();
            }
        }
        // Pause other sound in sink phone when connected
        getAudioFocus(connected);
    }
    private void getAudioFocus(boolean grab) {
        if (grab) {
            int ret = mAudioManager.requestAudioFocus(
                        mAudioFocusListener,
                        AudioManager.STREAM_MUSIC,
                        AudioManager.AUDIOFOCUS_GAIN);
            if (ret == AudioManager.AUDIOFOCUS_REQUEST_FAILED) {
                Slog.e(TAG, "requestAudioFocus() FAIL !!!");
            }
        } else {
            mAudioManager.abandonAudioFocus(mAudioFocusListener);
        }
    }

    // Do nothing now
    private OnAudioFocusChangeListener mAudioFocusListener = new OnAudioFocusChangeListener() {
        public void onAudioFocusChange(int focusChange) {
            Slog.e(TAG, "onAudioFocusChange(), focus:" + focusChange);
            switch (focusChange) {
                case AudioManager.AUDIOFOCUS_LOSS:
                    break;
                case AudioManager.AUDIOFOCUS_GAIN:
                    break;
                default:
                    break;
            }
        }
    };

    private void setSinkMiracastMode() {
        Slog.i(TAG, "setSinkMiracastMode(), freq:" + mWifiApFreq);
        mWifiP2pManager.setMiracastMode(WifiP2pManager.MIRACAST_SINK);
    }

    private final Runnable mRtspSinkTimeout = new Runnable() {
        @Override
        public void run() {
            Slog.d(TAG, "mRtspSinkTimeout, run()");
            disconnectWfdSink();
        }
    };

    private void notifyDisplayConnecting() {
        //TODO: define multi-language string
        WifiDisplay display = new WifiDisplay(
            "Temp address", "WiFi Display Device", null,
        true, true, false);

        Slog.d(TAG, "[sink] callback onDisplayConnecting()");
        mController.getListener().onDisplayConnecting(display);
    }
    public void showDialog(int dialogID) {
        mUserDecided = false;
        prepareDialog(dialogID);

    }

    private void dismissDialog() {
        dismissDialogDetail(mWifiDirectExcludeDialog);
        dismissDialogDetail(mReConnecteDialog);

    }

    private void dismissDialogDetail(AlertDialog dialog) {
        if (null != dialog && dialog.isShowing()) {
            dialog.dismiss();
        }

    }
    private void chooseNo_WifiDirectExcludeDialog() {
        if (SystemProperties.get("ro.vendor.mtk_wfd_sink_support").equals("1")  && mSinkEnabled) {
            Slog.d(TAG, "[sink] callback onDisplayConnectionFailed()");
            mController.getListener().onDisplayConnectionFailed();
        } else {
            /*M: ALPS00758891: notify apk wfd connection isn't connected*/
            //unadvertiseDisplay();
            //actionAtDisconnected(null);
        }

    }
    private void prepareDialog(int dialogID) {
        Resources mResource = Resources.getSystem();

        if (WFD_WIFIP2P_EXCLUDED_DIALOG == dialogID) {
            // wifi direct excluded dialog
            mWifiDirectExcludeDialog = new AlertDialog.Builder(mContext)
                .setMessage(mResource.getString(
                    getMtkStringResourceId("wifi_display_wifi_p2p_disconnect_wfd_connect")))
                .setPositiveButton(mResource.getString(R.string.dlg_ok), new OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {

                            Slog.d(TAG, "[Exclude Dialog] disconnect previous WiFi P2p connection");

                            //Reset parameter
                            mIsConnected_OtherP2p = false;

                          
                            mWifiP2pManager.requestGroupInfo(mWifiP2pChannel,
                                new WifiP2pManager.GroupInfoListener() {

                                    public void onGroupInfoAvailable(WifiP2pGroup group) {
                                        Slog.i(TAG, "onGroupInfoAvailable() of non wifidisplay p2p");

                                        // For error handling
                                        if (group == null) {
                                            Slog.i(TAG, "group is null !!!");
                                        }else if (group.getNetworkId() >= 0) {

                                            Slog.i(TAG, "deletePersistentGroup of non wifidisplay p2p");
                                            mWifiP2pManager.deletePersistentGroup(
                                                mWifiP2pChannel, group.getNetworkId(), null);

                                            Slog.i(TAG, "removeGroup of non wifidisplay p2p");
                                            mWifiP2pManager.removeGroup(mWifiP2pChannel, new ActionListener() {
                                                @Override
                                                public void onSuccess() {
                                                    Slog.i(TAG,
                                                        "Disconnected from previous Wi-Fi P2p device, succeess");
                                                }

                                                @Override
                                                public void onFailure(int reason) {
                                                    Slog.i(TAG,
                                                        "Disconnected from previous Wi-Fi P2p device, failure = "
                                                        + reason);
                                                }
                                            });
                                        }
                                   }
                                }
                            ); 

                            chooseNo_WifiDirectExcludeDialog();

                            mUserDecided = true;
                        }
                    })
                .setNegativeButton(mResource.getString(R.string.decline), new OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {

                            Slog.d(TAG, "[Exclude Dialog] keep previous Wi-Fi P2p connection");

                            chooseNo_WifiDirectExcludeDialog();
                            mUserDecided = true;
                        }
                    })
                .setOnCancelListener(new DialogInterface.OnCancelListener() {
                        @Override
                        public void onCancel(DialogInterface arg0) {

                            Slog.d(TAG,
                                "[Exclude Dialog] onCancel(): keep previous Wi-Fi P2p connection");

                            chooseNo_WifiDirectExcludeDialog();
                            mUserDecided = true;
                        }
                    })
                .setOnDismissListener(new DialogInterface.OnDismissListener() {
                        @Override
                        public void onDismiss(DialogInterface arg0) {

                            Slog.d(TAG, "[Exclude Dialog] onDismiss()");

                            if (false == mUserDecided) {
                                chooseNo_WifiDirectExcludeDialog();
                            }
                        }
                    })
                .create();
            popupDialog(mWifiDirectExcludeDialog);

        } else if (WFD_RECONNECT_DIALOG == dialogID) {
            // re-connect dialog
            mReConnecteDialog = new AlertDialog.Builder(mContext)
                .setTitle(getMtkStringResourceId("wifi_display_reconnect"))
                .setMessage(getMtkStringResourceId("wifi_display_disconnect_then_reconnect"))
                .setPositiveButton(mResource.getString(R.string.dlg_ok), new OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            if (DEBUG) {
                                Slog.d(TAG, "user want to reconnect");
                            }

                            //scan first
                            //mReScanning = true;
                            mController.updateScanState();
                            // check scan result per RECONNECT_RETRY_DELAY_MILLIS ms
                            mReConnection_Timeout_Remain_Seconds = CONNECTION_TIMEOUT_SECONDS;
                            mHandler.postDelayed(mReConnect, RECONNECT_RETRY_DELAY_MILLIS);
                        }
                    })
                .setNegativeButton(mResource.getString(R.string.decline), new OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            if (DEBUG) {
                                Slog.d(TAG, "user want nothing");
                            }
                        }
                    })
                .setOnCancelListener(new DialogInterface.OnCancelListener() {
                        @Override
                        public void onCancel(DialogInterface arg0) {
                            if (DEBUG) {
                                Slog.d(TAG, "user want nothing");
                            }
                        }
                    })
                .create();
            popupDialog(mReConnecteDialog);

        } else if (WFD_CHANGE_RESOLUTION_DIALOG == dialogID) {

            // check box layout
            LayoutInflater adbInflater = LayoutInflater.from(mContext);
            View checkboxLayout = adbInflater.inflate(
                                    getMtkLayoutResourceId("checkbox"), null);
            final CheckBox checkbox = (CheckBox) checkboxLayout.findViewById(
                                                    getMtkIdResourceId("skip"));
            checkbox.setText(getMtkStringResourceId("wifi_display_do_not_remind_again"));

            // change resolution dialog
            mChangeResolutionDialog = new AlertDialog.Builder(mContext)
                .setView(checkboxLayout)
                .setMessage(getMtkStringResourceId("wifi_display_change_resolution_reminder"))
                .setPositiveButton(mResource.getString(R.string.dlg_ok), new OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {

                            boolean checked = checkbox.isChecked();
                            Slog.d(TAG, "[Change resolution]: ok. checked:" + checked);

                            // update settings
                            if (checked) {
                                Settings.Global.putInt(
                                        mContext.getContentResolver(),
                                        getMtkSettingsExtGlobalSetting(
                                                "WIFI_DISPLAY_RESOLUTION_DONOT_REMIND"),
                                        1);
                            } else {
                                Settings.Global.putInt(
                                        mContext.getContentResolver(),
                                        getMtkSettingsExtGlobalSetting(
                                                "WIFI_DISPLAY_RESOLUTION_DONOT_REMIND"),
                                        0);
                            }

                            // check again if need to reconnect
                            if (mController.mConnectedDevice != null || mController.mConnectingDevice != null) {

                                Slog.d(TAG, "-- reconnect for resolution change --");
                                /// reconnect again
                                mController.disconnect();
                                //mReconnectForResolutionChange = true;
                            }
                        }
                    })
                .setNegativeButton(mResource.getString(R.string.cancel), new OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {

                            Slog.d(TAG, "[Change resolution]: cancel");


                            //revert resolution
                            //revertResolutionChange();
                        }
                    })
                .setOnCancelListener(new DialogInterface.OnCancelListener() {
                        @Override
                        public void onCancel(DialogInterface arg0) {

                            Slog.d(TAG, "[Change resolution]: doesn't choose");


                            //revert resolution
                            //revertResolutionChange();
                        }
                    })
                .create();
            popupDialog(mChangeResolutionDialog);

        } else if (WFD_SOUND_PATH_DIALOG == dialogID) {

            // check box layout
            LayoutInflater adbInflater = LayoutInflater.from(mContext);
            View checkboxLayout = adbInflater.inflate(
                getMtkLayoutResourceId("checkbox"), null);
            final CheckBox checkbox = (CheckBox) checkboxLayout.findViewById(
                getMtkIdResourceId("skip"));
            checkbox.setText(getMtkStringResourceId("wifi_display_do_not_remind_again"));

            // default is enable
            int value = Settings.Global.getInt(
                    mContext.getContentResolver(),
                    getMtkSettingsExtGlobalSetting("WIFI_DISPLAY_SOUND_PATH_DONOT_REMIND"),
                    -1);
            if (value == -1) {
                checkbox.setChecked(true);
            }

            // sound path dialog
            mSoundPathDialog = new AlertDialog.Builder(mContext)
                .setView(checkboxLayout)
                .setMessage(getMtkStringResourceId("wifi_display_sound_path_reminder"))
                .setPositiveButton(mResource.getString(R.string.dlg_ok), new OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {

                            boolean checked = checkbox.isChecked();
                            Slog.d(TAG, "[Sound path reminder]: ok. checked:" + checked);

                            // update settings
                            if (checked) {
                                Settings.Global.putInt(
                                        mContext.getContentResolver(),
                                        getMtkSettingsExtGlobalSetting(
                                                "WIFI_DISPLAY_SOUND_PATH_DONOT_REMIND"),
                                        1);
                            } else {
                                Settings.Global.putInt(
                                        mContext.getContentResolver(),
                                        getMtkSettingsExtGlobalSetting(
                                                "WIFI_DISPLAY_SOUND_PATH_DONOT_REMIND"),
                                        0);
                            }
                        }
                    })
                .setOnCancelListener(new DialogInterface.OnCancelListener() {
                        @Override
                        public void onCancel(DialogInterface arg0) {
                            Slog.d(TAG, "[Sound path reminder]: cancel");
                        }
                    })
                .create();
            popupDialog(mSoundPathDialog);

        } else if (WFD_WAIT_CONNECT_DIALOG == dialogID) {

            // progress layout
            LayoutInflater adbInflater = LayoutInflater.from(mContext);
            View progressLayout = adbInflater.inflate(
                getMtkLayoutResourceId("progress_dialog"), null);
            final ProgressBar progressBar = (ProgressBar) progressLayout.findViewById(
                getMtkIdResourceId("progress"));
            progressBar.setIndeterminate(true);
            final TextView progressText = (TextView) progressLayout.findViewById(
                getMtkIdResourceId("progress_text"));
            progressText.setText(getMtkStringResourceId("wifi_display_wait_connection"));

            // wait connection dialog
            mWaitConnectDialog = new AlertDialog.Builder(mContext)
                .setView(progressLayout)
                .setNegativeButton(mResource.getString(R.string.cancel), new OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {

                            Slog.d(TAG, "[Wait connection]: cancel");

                            //disconnect wifi display
                            disconnectWfdSink();
                        }
                    })
                .setOnCancelListener(new DialogInterface.OnCancelListener() {
                        @Override
                        public void onCancel(DialogInterface arg0) {
                            Slog.d(TAG, "[Wait connection]: no choice");

                            //disconnect wifi display
                            disconnectWfdSink();
                        }
                    })
                .create();
            popupDialog(mWaitConnectDialog);

        }else if (WFD_CONFIRM_CONNECT_DIALOG == dialogID) {

            dismissDialogDetail(mWaitConnectDialog);

            String message = mSinkDeviceName +
                             " " +
                             mResource.getString(getMtkStringResourceId("wifi_display_confirm_connection"));

            // confirm connect dialog
            mConfirmConnectDialog = new AlertDialog.Builder(mContext)
                .setMessage(message)
                .setPositiveButton(mResource.getString(R.string.accept), new OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {

                            Slog.d(TAG, "[GC confirm connection]: accept");

                            String goIntent = SystemProperties.get(
                                "wfd.sink.go_intent",
                                String.valueOf(WifiP2pConfig.MAX_GROUP_OWNER_INTENT - 1));
                            int value = Integer.valueOf(goIntent);

                            Slog.i(TAG, "Sink go_intent:" + value);
                            mWifiP2pManager.setGCInviteResult(mWifiP2pChannel, true, value, null);

                            // Show progress dialog
                            showDialog(WFD_BUILD_CONNECT_DIALOG);
                        }
                    })
                .setNegativeButton(mResource.getString(R.string.decline), new OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {

                            Slog.d(TAG, "[GC confirm connection]: declines");
                            mWifiP2pManager.setGCInviteResult(mWifiP2pChannel, false, 0, null);

                            //disconnect wifi display
                            disconnectWfdSink();
                        }
                    })
                .setOnCancelListener(new DialogInterface.OnCancelListener() {
                        @Override
                        public void onCancel(DialogInterface arg0) {

                            Slog.d(TAG, "[Confirm connection]: cancel");
                            mWifiP2pManager.setGCInviteResult(mWifiP2pChannel, false, 0, null);

                            //disconnect wifi display
                            disconnectWfdSink();
                        }
                    })
                .create();
            popupDialog(mConfirmConnectDialog);
        } else if (WFD_BUILD_CONNECT_DIALOG == dialogID) {

            // progress layout
            LayoutInflater adbInflater = LayoutInflater.from(mContext);
            View progressLayout = adbInflater.inflate(
                getMtkLayoutResourceId("progress_dialog"), null);
            final ProgressBar progressBar = (ProgressBar) progressLayout.findViewById(
                getMtkIdResourceId("progress"));
            progressBar.setIndeterminate(true);
            final TextView progressText = (TextView) progressLayout.findViewById(
                getMtkIdResourceId("progress_text"));
            progressText.setText(getMtkStringResourceId("wifi_display_build_connection"));

            // build connection dialog
            mBuildConnectDialog = new AlertDialog.Builder(mContext)
                .setView(progressLayout)
                .setNegativeButton(mResource.getString(R.string.cancel), new OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {

                            Slog.d(TAG, "[Build connection]: cancel");

                            //disconnect wifi display
                            disconnectWfdSink();
                        }
                    })
                .setOnCancelListener(new DialogInterface.OnCancelListener() {
                        @Override
                        public void onCancel(DialogInterface arg0) {
                            Slog.d(TAG, "[Build connection]: no choice");

                            //disconnect wifi display
                            disconnectWfdSink();
                        }
                    })
                .create();
            popupDialog(mBuildConnectDialog);

        }

    }
    private void popupDialog(AlertDialog dialog) {
        dialog.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
        dialog.getWindow().getAttributes().privateFlags |=
                WindowManager.LayoutParams.PRIVATE_FLAG_SHOW_FOR_ALL_USERS;
        dialog.show();

    }

    ///M:@{ WFD UIBC Support
    public void sendUibcInputEvent(String input) {
        if (SystemProperties.get("ro.vendor.mtk_wfd_sink_uibc_support").equals("1") && mSinkEnabled) {
            if (mRemoteDisplay != null) {
                mRemoteDisplay.sendUibcEvent(input);
            }
        }
    }
    ///@}
  /*  
    private void enableWifiDisplay() {

        mHandler.removeCallbacks(mEnableWifiDelay);

        // Enable wifi
        if (SystemProperties.get("ro.vendor.mtk_wfd_support").equals("1") &&
            mWifiDisplayOnSetting && !mWifiP2pEnabled) {

            long delay = Settings.Global.getLong(mContext.getContentResolver(),
                Settings.Global.WIFI_REENABLE_DELAY_MS, 500);

            Slog.d(TAG, "Enable wifi with delay:" + delay);
            mHandler.postDelayed(mEnableWifiDelay, delay);

            //show toast
            Toast.makeText(mContext,
                getMtkStringResourceId("wifi_display_wfd_and_wifi_are_turned_on")
                , Toast.LENGTH_SHORT).show();
        }
        else {
            mAutoEnableWifi = false;
            mController.updateWfdEnableState();
        }
    }
*/
    private void sendPortraitIntent() {
        Slog.d(TAG, "sendPortraitIntent()");
        Intent intent = new Intent(WFD_PORTRAIT);
        intent.addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        mContext.sendBroadcastAsUser(intent, UserHandle.ALL);
    }

    private void blockNotificationList(boolean block) {
        Slog.i(TAG, "blockNotificationList(), block:" + block);
        if (block) {
            mStatusBarManager.disable(StatusBarManager.DISABLE_EXPAND);
        } else {
            mStatusBarManager.disable(0);
        }
    }
//endif MTK_WFD_SINK_SUPPORT
}

