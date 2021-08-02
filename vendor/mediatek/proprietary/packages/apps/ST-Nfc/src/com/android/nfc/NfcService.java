/*
 * Copyright (C) 2010 The Android Open Source Project
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
/**
 * ****************************************************************************
 *
 * <p>The original Work has been changed by ST Microelectronics S.A.
 *
 * <p>Copyright (C) 2017 ST Microelectronics S.A.
 *
 * <p>Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of the License at
 *
 * <p>http://www.apache.org/licenses/LICENSE-2.0
 *
 * <p>Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * <p>****************************************************************************
 */
package com.android.nfc;

import com.android.nfc.st.StDeviceHost;
import android.app.ActivityManager;
import android.app.Application;
import android.app.backup.BackupManager;
import android.app.KeyguardManager;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.admin.DevicePolicyManager;
import android.app.BroadcastOptions;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.content.pm.IPackageManager;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;

import android.content.ServiceConnection;

import com.android.nfc.dhimpl.StNativeNfcManager;
import com.android.nfc.dhimpl.StNativeNfcSecureElement;
import com.android.nfc.dhimpl.NativeNfcStExtensions;
import com.android.nfc.dhimpl.NativeNfcStDtaExtensions;
import android.content.pm.UserInfo;
import android.content.res.Resources.NotFoundException;
import android.media.AudioManager;
import android.media.SoundPool;
import android.net.Uri;
import android.nfc.BeamShareData;
import android.nfc.cardemulation.CardEmulation;
import android.nfc.ErrorCodes;
import android.nfc.FormatException;
import android.nfc.IAppCallback;
import android.nfc.INfcAdapter;
import android.nfc.INfcAdapterExtras;
import android.nfc.INfcCardEmulation;
import android.nfc.INfcDta;
import android.nfc.INfcFCardEmulation;
import android.nfc.INfcTag;
import com.st.android.nfc_extensions.StConstants;
import com.st.android.nfc_extensions.StApduServiceInfo;
import com.st.android.nfc_extensions.NfcAdapterStExtensions;
import com.st.android.nfc_extensions.INfcAdapterStExtensions;
import com.st.android.nfc_dta_extensions.INfcAdapterStDtaExtensions;
import com.st.android.nfc_extensions.INfcWalletAdapter;
import com.st.android.nfc_extensions.INfcWalletLogCallback;
import android.nfc.INfcUnlockHandler;
import android.nfc.ITagRemovedCallback;
import android.nfc.NdefMessage;
import android.nfc.NfcAdapter;
import android.nfc.Tag;
import android.nfc.TechListParcel;
import android.nfc.TransceiveResult;
import android.nfc.tech.Ndef;
import android.nfc.tech.TagTechnology;
import android.os.AsyncTask;
import android.os.Binder;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.PowerManager;
import android.os.Process;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemClock;
import android.os.SystemProperties;
import android.os.UserHandle;
import android.os.UserManager;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.provider.Settings;
import android.se.omapi.ISecureElementService;
import android.service.vr.IVrManager;
import android.service.vr.IVrStateCallbacks;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;

import com.android.internal.logging.MetricsLogger;
import com.android.internal.util.ArrayUtils;
import com.android.nfc.DeviceHost.DeviceHostListener;
import com.android.nfc.DeviceHost.LlcpConnectionlessSocket;
import com.android.nfc.DeviceHost.LlcpServerSocket;
import com.android.nfc.DeviceHost.LlcpSocket;
import com.android.nfc.DeviceHost.NfcDepEndpoint;
import com.android.nfc.DeviceHost.TagEndpoint;
import com.android.nfc.cardemulation.AidRoutingManager;
import com.android.nfc.cardemulation.CardEmulationManager;
// import com.android.nfc.dhimpl.NativeNfcManager;
import com.android.nfc.handover.HandoverDataParser;
import com.android.nfc.dhimpl.StNativeNfcSecureElement;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;
import java.util.Scanner;
import java.util.HashSet;
import android.util.StatsLog;
import com.android.nfc.st.NfcAddonWrapper;
// MTK - START
import com.android.nfc.st.MtkNfcStatusNotificationUi;
// MTK - END
import com.st.android.nfc_extensions.INfcSettingsAdapter;
import com.st.android.nfc_extensions.INfcSettingsCallback;
import com.st.android.nfc_extensions.NfcSettingsAdapter;
import com.st.android.nfc_extensions.ServiceEntry;

import com.st.android.nfc_extensions.DefaultRouteEntry;

public class NfcService implements DeviceHostListener {
    static final boolean DBG = true;
    static final boolean DBG2 =
            DBG && android.os.SystemProperties.get("persist.st_nfc_debug").equals("1");
    static final String TAG = "NfcService";

    public static final String SERVICE_NAME = "nfc";

    /** Regular NFC permission */
    private static final String NFC_PERM = android.Manifest.permission.NFC;

    private static final String NFC_PERM_ERROR = "NFC permission required";

    public static final String PREF = "NfcServicePrefs";

    static final String PREF_NFC_ON = "nfc_on";
    static final boolean NFC_ON_DEFAULT = true;
    static final String PREF_NDEF_PUSH_ON = "ndef_push_on";
    static final boolean NDEF_PUSH_ON_DEFAULT = false;
    static final String PREF_SECURE_NFC_ON = "secure_nfc_on";
    static final boolean SECURE_NFC_ON_DEFAULT = false;
    static final String PREF_FIRST_BEAM = "first_beam";
    static final String PREF_FIRST_BOOT = "first_boot";

    static final String TRON_NFC_CE = "nfc_ce";
    static final String TRON_NFC_P2P = "nfc_p2p";
    static final String TRON_NFC_TAG = "nfc_tag";

    // ApduServiceInfo description of last modified service that caused a
    // routing table update
    private static final String PREF_LAST_MODIFIED_SERVICE = "last_modified_service";

    static final String NATIVE_LOG_FILE_NAME = "native_logs";

    // Wallet: is tech mute requested?
    private static final String PREF_WALLET_MUTE_A = "wallet_mute_A";
    private static final String PREF_WALLET_MUTE_B = "wallet_mute_B";
    private static final String PREF_WALLET_MUTE_F = "wallet_mute_F";

    // Default AID route - might be changed by user
    // MTK - start : make these public
    public static final String PREF_DEFAULT_AID_ROUTE = "default_aid_route";
    public static final String PREF_DEFAULT_MIFARE_ROUTE = "default_mifare_route";
    public static final String PREF_DEFAULT_ISODEP_ROUTE = "default_iso_dep_route";
    public static final String PREF_DEFAULT_FELICA_ROUTE = "default_felica_route";
    public static final String PREF_DEFAULT_AB_TECH_ROUTE = "default_default_ab_tech_route";
    public static final String PREF_DEFAULT_SC_ROUTE = "default_sc_route";
    // MTK - end

    static final int MSG_NDEF_TAG = 0;
    static final int MSG_LLCP_LINK_ACTIVATION = 1;
    static final int MSG_LLCP_LINK_DEACTIVATED = 2;
    static final int MSG_MOCK_NDEF = 3;
    static final int MSG_LLCP_LINK_FIRST_PACKET = 4;
    static final int MSG_ROUTE_AID = 5;
    static final int MSG_UNROUTE_AID = 6;
    static final int MSG_COMMIT_ROUTING = 7;
    static final int MSG_INVOKE_BEAM = 8;
    static final int MSG_RF_FIELD_ACTIVATED = 9;
    static final int MSG_RF_FIELD_DEACTIVATED = 10;
    static final int MSG_RESUME_POLLING = 11;
    static final int MSG_REGISTER_T3T_IDENTIFIER = 12;
    static final int MSG_DEREGISTER_T3T_IDENTIFIER = 13;
    static final int MSG_TAG_DEBOUNCE = 14;
    static final int MSG_UPDATE_STATS = 15;
    static final int MSG_APPLY_SCREEN_STATE = 16;
    static final int MSG_TRANSACTION_EVENT = 17;

    static final int MSG_CLEAR_ROUTING = 18;
    static final int MSG_UPDATE_ROUTING_TABLE = 19;

    static final int MSG_START_POLLING = 20;

    // Update stats every 4 hours
    static final long STATS_UPDATE_INTERVAL_MS = 4 * 60 * 60 * 1000;
    static final long MAX_POLLING_PAUSE_TIMEOUT = 40000;

    static final int TASK_ENABLE = 1;
    static final int TASK_DISABLE = 2;
    static final int TASK_BOOT = 3;

    // Polling technology masks
    static final int NFC_POLL_A = 0x01;
    static final int NFC_POLL_B = 0x02;
    static final int NFC_POLL_F = 0x04;
    static final int NFC_POLL_V = 0x08;
    static final int NFC_POLL_B_PRIME = 0x10;
    static final int NFC_POLL_KOVIO = 0x20;

    // Return values from NfcEe.open() - these are 1:1 mapped
    // to the thrown EE_EXCEPTION_ exceptions in nfc-extras.
    static final int EE_ERROR_IO = -1;
    static final int EE_ERROR_ALREADY_OPEN = -2;
    static final int EE_ERROR_INIT = -3;
    static final int EE_ERROR_LISTEN_MODE = -4;
    static final int EE_ERROR_EXT_FIELD = -5;
    static final int EE_ERROR_NFC_DISABLED = -6;

    // minimum screen state that enables NFC polling
    static final int NFC_POLLING_MODE = ScreenStateHelper.SCREEN_STATE_ON_UNLOCKED;

    // Time to wait for NFC controller to initialize before watchdog
    // goes off. This time is chosen large, because firmware download
    // may be a part of initialization.
    // MTK - start - longer timer as startup with fw update may exceed 15sec on eng bld
    static final int INIT_WATCHDOG_MS = 30000;
    // MTK - end

    // Time to wait for routing to be applied before watchdog
    // goes off
    static final int ROUTING_WATCHDOG_MS = 10000;

    // Default delay used for presence checks
    static final int DEFAULT_PRESENCE_CHECK_DELAY = 125;

    // The amount of time we wait before manually launching
    // the Beam animation when called through the share menu.
    static final int INVOKE_BEAM_DELAY_MS = 1000;

    // RF field events as defined in NFC extras
    public static final String ACTION_RF_FIELD_ON_DETECTED =
            "com.android.nfc_extras.action.RF_FIELD_ON_DETECTED";
    public static final String ACTION_RF_FIELD_OFF_DETECTED =
            "com.android.nfc_extras.action.RF_FIELD_OFF_DETECTED";

    public static boolean sIsShortRecordLayout = false;

    // for use with playSound()
    public static final int SOUND_START = 0;
    public static final int SOUND_END = 1;
    public static final int SOUND_ERROR = 2;

    public static final int NCI_VERSION_2_0 = 0x20;

    public static final int NCI_VERSION_1_0 = 0x10;

    public static final String ACTION_LLCP_UP = "com.android.nfc.action.LLCP_UP";

    public static final String ACTION_LLCP_DOWN = "com.android.nfc.action.LLCP_DOWN";

    final NotificationManager mNotificationManager;
    static final String OVERFLOW_NOTIFICATION_CHANNEL = "overflow_notification_channel";
    static final int OVERFLOW_UNIQUE_NOTIF_ID = 50001;
    // MTK - start
    private static final String MTK_OVERFLOW_SETTINGS_MENU_INTENT =
            "com.mediatek.settings.NFC_SERVICE_STATUS";
    // MTK - end
    private static final String OVERFLOW_SETTINGS_MENU_INTENT =
            "com.st.settings.NFC_SERVICE_STATUS";
    private static final String OVERFLOW_NTF_ACTION_HIDE =
            "com.android.nfc_extras.action.OF_HIDE_NTF";
    // Timeout to re-apply routing if a tag was present and we postponed it
    private static final int APPLY_ROUTING_RETRY_TIMEOUT_MS = 5000;

    private final UserManager mUserManager;

    private static int nci_version = NCI_VERSION_1_0;
    // NFC Execution Environment
    // fields below are protected by this
    private final ReaderModeDeathRecipient mReaderModeDeathRecipient =
            new ReaderModeDeathRecipient();
    private final NfcUnlockManager mNfcUnlockManager;

    private StNativeNfcSecureElement mSecureElement;
    private OpenSecureElement mOpenEe; // null when EE closed
    private final NfceeAccessControl mNfceeAccessControl;

    private final BackupManager mBackupManager;

    // cached version of installed packages requesting Android.permission.NFC_TRANSACTION_EVENTS
    List<String> mNfcEventInstalledPackages = new ArrayList<String>();

    // fields below are used in multiple threads and protected by synchronized(this)
    final HashMap<Integer, Object> mObjectMap = new HashMap<Integer, Object>();
    HashSet<String> mSePackages = new HashSet<String>();
    int mScreenState;
    boolean mInProvisionMode; // whether we're in setup wizard and enabled NFC provisioning
    boolean mIsNdefPushEnabled;
    boolean mIsSecureNfcEnabled;
    NfcDiscoveryParameters mCurrentDiscoveryParameters =
            NfcDiscoveryParameters.getNfcOffParameters();

    ReaderModeParams mReaderModeParams;

    private int mUserId;
    boolean mPollingPaused;

    static final int INVALID_NATIVE_HANDLE = -1;
    byte mDebounceTagUid[];
    int mDebounceTagDebounceMs;
    int mDebounceTagNativeHandle = INVALID_NATIVE_HANDLE;
    ITagRemovedCallback mDebounceTagRemovedCallback;

    // Only accessed on one thread so doesn't need locking
    NdefMessage mLastReadNdefMessage;

    // Metrics
    AtomicInteger mNumTagsDetected;
    AtomicInteger mNumP2pDetected;
    AtomicInteger mNumHceDetected;

    // mState is protected by this, however it is only modified in onCreate()
    // and the default AsyncTask thread so it is read unprotected from that
    // thread
    int mState; // one of NfcAdapter.STATE_ON, STATE_TURNING_ON, etc
    // fields below are final after onCreate()
    Context mContext;
    private StDeviceHost mDeviceHost;
    private SharedPreferences mPrefs;
    private SharedPreferences.Editor mPrefsEditor;
    private PowerManager.WakeLock mRoutingWakeLock;
    private PowerManager.WakeLock mEeWakeLock;
    NfcAdapterExtrasService mExtrasService;

    int mStartSound;
    int mEndSound;
    int mErrorSound;
    SoundPool mSoundPool; // playback synchronized on this
    P2pLinkManager mP2pLinkManager;
    TagService mNfcTagService;
    NfcAdapterService mNfcAdapter;
    NfcDtaService mNfcDtaService;
    NfcSettingsAdapterService mNfcSettingsAdapterService;
    // MTK - start
    MtkNfcSettingsAdapterService mMtkNfcSettingsAdapterService;
    public static final boolean HAS_MTK_SETTINGS =
            ("1".equals(SystemProperties.get("ro.vendor.mtk_nfc_addon_support"))
                    && !"1".equals(SystemProperties.get("persist.st_nfc_ignore_addon_support")));
    // MTK - end

    boolean mIsDebugBuild;
    boolean mIsHceCapable;
    boolean mIsHceFCapable;
    boolean mIsBeamCapable;
    boolean mIsSecureNfcCapable;
    boolean mPendingRoutingTableUpdate;
    boolean mPendingPowerStateUpdate;

    StExtrasService mStExtras;
    StDtaExtrasService mStDtaExtras;
    NfcWalletAdapterService mNfcWalletAdapter;

    NativeNfcStExtensions mStExtensions;
    NativeNfcStDtaExtensions mStDtaExtensions;
    byte[] techArrayConfigSave = new byte[4];
    int modeBitmapSave;
    private NfcDispatcher mNfcDispatcher;
    private PowerManager mPowerManager;
    private KeyguardManager mKeyguard;
    private HandoverDataParser mHandoverDataParser;
    private ContentResolver mContentResolver;
    private CardEmulationManager mCardEmulationManager;
    private NotificationBroadcastReceiver mNotificationBroadcastReceiver;
    private AidRoutingManager mAidRoutingManager;
    private Vibrator mVibrator;
    private VibrationEffect mVibrationEffect;
    private ISecureElementService mSEService;

    private ScreenStateHelper mScreenStateHelper;
    private ForegroundUtils mForegroundUtils;

    private static NfcService sService;
    public static boolean sIsDtaMode = false;

    private IVrManager vrManager;
    boolean mIsVrModeEnabled;

    private String mUsedAidRoute;
    private String mUsedMifareRoute;
    private String mUsedIsoDepRoute;
    private String mUsedFelicaRoute;
    private String mUsedAbTechRoute;
    private String mUsedScRoute;

    public void enforceNfceeAdminPerm(String pkg) {

        if (DBG2) Log.d(TAG, "enforceNfceeAdminPerm() - pkg: " + pkg);

        if (pkg == null) {
            throw new SecurityException("caller must pass a package name");
        }
        mContext.enforceCallingOrSelfPermission(NFC_PERM, NFC_PERM_ERROR);
        if (!mNfceeAccessControl.check(Binder.getCallingUid(), pkg)) {
            if (DBG2) Log.e(TAG, "enforceNfceeAdminPerm() - no access ");
            throw new SecurityException(
                    NfceeAccessControl.NFCEE_ACCESS_PATH + " denies NFCEE access to " + pkg);
        }
        if (UserHandle.getCallingUserId() != UserHandle.USER_OWNER) {
            if (DBG2)
                Log.e(TAG, "enforceNfceeAdminPerm() - only the owner is allowed to call SE APIs");
            throw new SecurityException("only the owner is allowed to call SE APIs");
        }
    }

    private boolean mHceF_enabled = false;

    public static NfcService getInstance() {
        return sService;
    }

    @Override
    public void onRemoteEndpointDiscovered(TagEndpoint tag) {
        sendMessage(NfcService.MSG_NDEF_TAG, tag);
    }

    public boolean getLastCommitRoutingStatus() {
        return mAidRoutingManager.getLastCommitRoutingStatus();
    }

    /** Notifies transaction */
    @Override
    public void onHostCardEmulationActivated(int technology) {
        if (mCardEmulationManager != null & mIsHceCapable) {
            mCardEmulationManager.onHostCardEmulationActivated(technology);
        }
    }

    @Override
    public void onHostCardEmulationData(int technology, byte[] data) {
        if (mCardEmulationManager != null & mIsHceCapable) {
            mCardEmulationManager.onHostCardEmulationData(technology, data);
        }
    }

    @Override
    public void onHostCardEmulationDeactivated(int technology) {
        if (mCardEmulationManager != null) {
            // Do metrics here so we don't slow the CE path down
            mNumHceDetected.incrementAndGet();
            mCardEmulationManager.onHostCardEmulationDeactivated(technology);
        }
    }

    public void onStLogData(int logtype, byte[][] data) {
        if (mNfcWalletAdapter != null) {
            mNfcWalletAdapter.onStLogData(logtype, data);
        }
    }

    /** Notifies P2P Device detected, to activate LLCP link */
    @Override
    public void onLlcpLinkActivated(NfcDepEndpoint device) {
        if (!mIsBeamCapable) return;
        sendMessage(NfcService.MSG_LLCP_LINK_ACTIVATION, device);
    }

    /** Notifies P2P Device detected, to activate LLCP link */
    @Override
    public void onLlcpLinkDeactivated(NfcDepEndpoint device) {
        if (!mIsBeamCapable) return;
        sendMessage(NfcService.MSG_LLCP_LINK_DEACTIVATED, device);
    }

    /** Notifies P2P Device detected, first packet received over LLCP link */
    @Override
    public void onLlcpFirstPacketReceived(NfcDepEndpoint device) {
        if (!mIsBeamCapable) return;
        mNumP2pDetected.incrementAndGet();
        sendMessage(NfcService.MSG_LLCP_LINK_FIRST_PACKET, device);
    }

    @Override
    public void onRemoteFieldActivated() {
        // remove message from queue
        mHandler.removeMessages(NfcService.MSG_RF_FIELD_ACTIVATED);
        // remove message from queue
        mHandler.removeMessages(NfcService.MSG_RF_FIELD_DEACTIVATED);

        // always post this message at front
        sendMessageAtFront(NfcService.MSG_RF_FIELD_ACTIVATED, null);
    }

    @Override
    public void onRemoteFieldDeactivated() {
        // remove message from queue
        mHandler.removeMessages(NfcService.MSG_RF_FIELD_ACTIVATED);
        // remove message from queue
        mHandler.removeMessages(NfcService.MSG_RF_FIELD_DEACTIVATED);

        sendMessageAtFront(NfcService.MSG_RF_FIELD_DEACTIVATED, null);
    }

    @Override
    public void onNfcTransactionEvent(byte[] aid, byte[] data, String seName) {
        byte[][] dataObj = {aid, data, seName.getBytes()};
        sendMessage(NfcService.MSG_TRANSACTION_EVENT, dataObj);
        StatsLog.write(
                StatsLog.NFC_CARDEMULATION_OCCURRED,
                StatsLog.NFC_CARDEMULATION_OCCURRED__CATEGORY__OFFHOST,
                seName);
    }

    @Override
    public void onEeUpdated() {

        if (this.mState != NfcAdapter.STATE_TURNING_ON) {
            if (DBG2) Log.d(TAG, "onEeUpdated()");

            sendMessage(NfcService.MSG_UPDATE_ROUTING_TABLE, null);
        } else {
            if (DBG2) Log.d(TAG, "onEeUpdated() - mState is STATE_TURNING_ON, skipping");
        }
    }

    private String getSeFamilly(int nfceeId) {
        switch (nfceeId) {
            case 0x81:
            case 0x83:
            case 0x85:
                return NfcSettingsAdapter.UICC_ROUTE;
            case 0x82:
            case 0x84:
            case 0x86:
                return NfcSettingsAdapter.ESE_ROUTE;
            case 0x00:
                return NfcSettingsAdapter.HCE_ROUTE;
            default:
                return NfcSettingsAdapter.DEFAULT_ROUTE;
        }
    }

    @Override
    public void onDefaultRoutesSet(
            int aidRoute,
            int mifareRoute,
            int isoDepRoute,
            int felicaRoute,
            int abTechRoute,
            int scRoute) {

        mUsedAidRoute = getSeFamilly(aidRoute);
        mUsedMifareRoute = getSeFamilly(mifareRoute);
        mUsedIsoDepRoute = getSeFamilly(isoDepRoute);
        mUsedFelicaRoute = getSeFamilly(felicaRoute);
        mUsedAbTechRoute = getSeFamilly(abTechRoute);
        mUsedScRoute = getSeFamilly(scRoute);

        if (DBG2)
            Log.d(
                    TAG,
                    "onDefaultRoutesSet() - aidRoute: "
                            + String.format("0x%02X", aidRoute)
                            + "/"
                            + mUsedAidRoute
                            + ", mifareRoute: "
                            + String.format("0x%02X", mifareRoute)
                            + "/"
                            + mUsedMifareRoute
                            + ", isoDepRoute: "
                            + String.format("0x%02X", isoDepRoute)
                            + "/"
                            + mUsedIsoDepRoute
                            + ", felicaRoute: "
                            + String.format("0x%02X", felicaRoute)
                            + "/"
                            + mUsedFelicaRoute
                            + ", abTechRoute"
                            + String.format("0x%02X", abTechRoute)
                            + "/"
                            + mUsedAbTechRoute
                            + ", scRoute"
                            + String.format("0x%02X", scRoute)
                            + "/"
                            + mUsedScRoute);

        // Signal Settings application that route was changed.
        NfcAddonWrapper.getInstance().onRouteChanged();
    }

    final class ReaderModeParams {
        public int flags;
        public IAppCallback callback;
        public int presenceCheckDelay;
    }

    public NfcService(Application nfcApplication) {

        if (DBG2) Log.d(TAG, "Constructor(begin)");

        mUserId = ActivityManager.getCurrentUser();
        mContext = nfcApplication;

        mNfcTagService = new TagService();
        mNfcAdapter = new NfcAdapterService();
        mNfcSettingsAdapterService = new NfcSettingsAdapterService();
        // MTK - start
        if (HAS_MTK_SETTINGS) {
            mMtkNfcSettingsAdapterService = new MtkNfcSettingsAdapterService();
        }
        // MTK - end

        Log.i(TAG, "Starting NFC service");

        sService = this;

        mScreenStateHelper = new ScreenStateHelper(mContext);
        mContentResolver = mContext.getContentResolver();
        mStExtensions = new NativeNfcStExtensions(mContext);
        mStDtaExtensions = new NativeNfcStDtaExtensions(mContext);
        mStExtras = new StExtrasService();
        mStDtaExtras = new StDtaExtrasService(); // TODO: use reflexion ?
        mNfcWalletAdapter = new NfcWalletAdapterService();

        mDeviceHost = new StNativeNfcManager(mContext, this, mStExtensions);

        mNfcUnlockManager = NfcUnlockManager.getInstance();

        mHandoverDataParser = new HandoverDataParser();
        boolean isNfcProvisioningEnabled = false;
        try {
            isNfcProvisioningEnabled =
                    mContext.getResources().getBoolean(R.bool.enable_nfc_provisioning);
        } catch (NotFoundException e) {
        }

        if (isNfcProvisioningEnabled) {
            mInProvisionMode =
                    Settings.Global.getInt(mContentResolver, Settings.Global.DEVICE_PROVISIONED, 0)
                            == 0;
        } else {
            mInProvisionMode = false;
        }

        mNfcDispatcher = new NfcDispatcher(mContext, mHandoverDataParser, mInProvisionMode);

        mPrefs = mContext.getSharedPreferences(PREF, Context.MODE_PRIVATE);
        mPrefsEditor = mPrefs.edit();

        mState = NfcAdapter.STATE_OFF;

        mIsDebugBuild = "userdebug".equals(Build.TYPE) || "eng".equals(Build.TYPE);

        mPowerManager = (PowerManager) mContext.getSystemService(Context.POWER_SERVICE);

        mRoutingWakeLock =
                mPowerManager.newWakeLock(
                        PowerManager.PARTIAL_WAKE_LOCK, "NfcService:mRoutingWakeLock");

        mKeyguard = (KeyguardManager) mContext.getSystemService(Context.KEYGUARD_SERVICE);
        mUserManager = (UserManager) mContext.getSystemService(Context.USER_SERVICE);
        mVibrator = (Vibrator) mContext.getSystemService(Context.VIBRATOR_SERVICE);
        mVibrationEffect = VibrationEffect.createOneShot(200, VibrationEffect.DEFAULT_AMPLITUDE);

        mScreenState = mScreenStateHelper.checkScreenState();

        mNumTagsDetected = new AtomicInteger();
        mNumP2pDetected = new AtomicInteger();
        mNumHceDetected = new AtomicInteger();

        mBackupManager = new BackupManager(mContext);
        // MTK - start
        MtkNfcStatusNotificationUi.createSingleton(mContext);
        // MTK - end

        mNotificationManager =
                (NotificationManager) mContext.getSystemService(Context.NOTIFICATION_SERVICE);
        NotificationChannel notificationChannel =
                new NotificationChannel(
                        OVERFLOW_NOTIFICATION_CHANNEL,
                        mContext.getString(R.string.app_name),
                        NotificationManager.IMPORTANCE_HIGH);
        mNotificationManager.createNotificationChannel(notificationChannel);

        // Intents for all users
        IntentFilter filter = new IntentFilter(Intent.ACTION_SCREEN_OFF);
        filter.addAction(Intent.ACTION_SCREEN_ON);
        filter.addAction(Intent.ACTION_USER_PRESENT);
        filter.addAction(Intent.ACTION_USER_SWITCHED);
        mContext.registerReceiverAsUser(mReceiver, UserHandle.ALL, filter, null, null);

        IntentFilter ownerFilter = new IntentFilter(Intent.ACTION_EXTERNAL_APPLICATIONS_AVAILABLE);
        ownerFilter.addAction(Intent.ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
        ownerFilter.addAction(Intent.ACTION_SHUTDOWN);
        mContext.registerReceiver(mOwnerReceiver, ownerFilter);

        ownerFilter = new IntentFilter();
        ownerFilter.addAction(Intent.ACTION_PACKAGE_ADDED);
        ownerFilter.addAction(Intent.ACTION_PACKAGE_REMOVED);
        ownerFilter.addDataScheme("package");
        mContext.registerReceiver(mOwnerReceiver, ownerFilter);

        IntentFilter policyFilter =
                new IntentFilter(DevicePolicyManager.ACTION_DEVICE_POLICY_MANAGER_STATE_CHANGED);
        mContext.registerReceiverAsUser(mPolicyReceiver, UserHandle.ALL, policyFilter, null, null);

        // MTK - start
        // Make sure the NFC_RF_FIELD_ACTIVE is initialized
        Settings.Global.putInt(mContext.getContentResolver(), "nfc_rf_field_active", 0);
        // MTK - end

        updatePackageCache();

        PackageManager pm = mContext.getPackageManager();
        mIsBeamCapable = pm.hasSystemFeature(PackageManager.FEATURE_NFC_BEAM);
        mIsNdefPushEnabled =
                mPrefs.getBoolean(PREF_NDEF_PUSH_ON, NDEF_PUSH_ON_DEFAULT) && mIsBeamCapable;
        if (mIsBeamCapable) {
            mP2pLinkManager =
                    new P2pLinkManager(
                            mContext,
                            mHandoverDataParser,
                            mDeviceHost.getDefaultLlcpMiu(),
                            mDeviceHost.getDefaultLlcpRwSize());
        }
        enforceBeamShareActivityPolicy(mContext, new UserHandle(mUserId));

        mIsHceCapable =
                pm.hasSystemFeature(PackageManager.FEATURE_NFC_HOST_CARD_EMULATION)
                        || pm.hasSystemFeature(PackageManager.FEATURE_NFC_HOST_CARD_EMULATION_NFCF);
        mIsHceFCapable = pm.hasSystemFeature(PackageManager.FEATURE_NFC_HOST_CARD_EMULATION_NFCF);
        if (mIsHceCapable) {
            mAidRoutingManager = new AidRoutingManager();
            mCardEmulationManager = new CardEmulationManager(mContext, mAidRoutingManager);

            mOverflowRouteSizes.clear();
        }
        mForegroundUtils = ForegroundUtils.getInstance();

        mIsSecureNfcCapable = mNfcAdapter.deviceSupportsNfcSecure();
        mIsSecureNfcEnabled =
                mPrefs.getBoolean(PREF_SECURE_NFC_ON, SECURE_NFC_ON_DEFAULT) && mIsSecureNfcCapable;
        mDeviceHost.setNfcSecure(mIsSecureNfcEnabled);

        // Make sure this is only called when object construction is complete.
        ServiceManager.addService(SERVICE_NAME, mNfcAdapter);
        // MTK - start
        if (HAS_MTK_SETTINGS) {
            ServiceManager.addService(
                    NfcSettingsAdapter.SERVICE_SETTINGS_NAME, mMtkNfcSettingsAdapterService);
        } else {
            ServiceManager.addService(
                    NfcSettingsAdapter.SERVICE_SETTINGS_NAME, mNfcSettingsAdapterService);
        }
        // MTK - end
        ServiceManager.addService(NfcAdapterStExtensions.SERVICE_NAME, mStExtras);

        new EnableDisableTask().execute(TASK_BOOT); // do blocking boot tasks

        mHandler.sendEmptyMessageDelayed(MSG_UPDATE_STATS, STATS_UPDATE_INTERVAL_MS);

        IVrManager mVrManager =
                IVrManager.Stub.asInterface(ServiceManager.getService(mContext.VR_SERVICE));
        if (mVrManager != null) {
            try {
                mVrManager.registerListener(mVrStateCallbacks);
                mIsVrModeEnabled = mVrManager.getVrModeState();
            } catch (RemoteException e) {
                Log.e(TAG, "Failed to register VR mode state listener: " + e);
            }
        }

        mSEService = null;

        mExtrasService = new NfcAdapterExtrasService();
        mSecureElement = new StNativeNfcSecureElement(mContext);
        mNfceeAccessControl = new NfceeAccessControl(mContext);
        mEeWakeLock =
                mPowerManager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "NfcService:mEeWakeLock");

        if (DBG2) Log.d(TAG, "Constructor(end)");
    }

    private boolean tryBindSecureElementService() {
        Log.d(TAG, "tryBindSecureElementService()");
        boolean bindingSuccess = false;
        if (mSEService == null) {
            boolean isLegacy = false;
            Intent startIntent = new Intent(ISecureElementService.class.getName());
            ComponentName comp = startIntent.resolveSystemService(mContext.getPackageManager(), 0);
            if (comp == null) {
                // try legacy name
                startIntent = new Intent("android.omapi.ISecureElementService");
                comp = startIntent.resolveSystemService(mContext.getPackageManager(), 0);
                if (comp != null) isLegacy = true;
            }
            startIntent.setComponent(comp);

            try {
                if (comp != null
                        && mContext.bindService(
                                startIntent, mSecureElementConnection, Context.BIND_AUTO_CREATE)) {
                    bindingSuccess = true;
                    mIsLegacySecureElementIntent = isLegacy;
                }
            } catch (IllegalArgumentException e) {
                Log.e(TAG, "Cannot bind to service, ", e);
                Log.e(TAG, "Intent was: " + startIntent.toString());
                Log.e(TAG, "ComponentName was: " + comp.toString());
            }
            if (bindingSuccess) {
                Log.d(TAG, "tryBindSecureElementService() - bindService successful");
            } else {
                if (comp == null) {
                    Log.e(
                            TAG,
                            "tryBindSecureElementService() - bindService failed, could not resolve "
                                    + startIntent.toString());
                } else {
                    Log.e(TAG, "tryBindSecureElementService() - bindService failed");
                }
            }
        } else {
            Log.d(TAG, "tryBindSecureElementService() - binding not needed");
            bindingSuccess = true;
        }

        return bindingSuccess;
    }

    private boolean mIsLegacySecureElementIntent = false;
    private ServiceConnection mSecureElementConnection =
            new ServiceConnection() {
                public synchronized void onServiceConnected(
                        ComponentName className, IBinder service) {
                    mSEService = ISecureElementService.Stub.asInterface(service);
                    Log.d(TAG, "mSecureElementConnection - onServiceConnected()");
                }

                public void onServiceDisconnected(ComponentName className) {
                    mSEService = null;
                    Log.d(TAG, "mSecureElementConnection() - onServiceDisconnected");
                }
            };

    void initSoundPool() {
        synchronized (this) {
            if (mSoundPool == null) {
                mSoundPool = new SoundPool(1, AudioManager.STREAM_NOTIFICATION, 0);
                mStartSound = mSoundPool.load(mContext, R.raw.start, 1);
                mEndSound = mSoundPool.load(mContext, R.raw.end, 1);
                mErrorSound = mSoundPool.load(mContext, R.raw.error, 1);
            }
        }
    }

    void releaseSoundPool() {
        synchronized (this) {
            if (mSoundPool != null) {
                mSoundPool.release();
                mSoundPool = null;
            }
        }
    }

    void updatePackageCache() {
        PackageManager pm = mContext.getPackageManager();
        List<PackageInfo> packagesNfcEvents =
                pm.getPackagesHoldingPermissions(
                        new String[] {android.Manifest.permission.NFC_TRANSACTION_EVENT},
                        PackageManager.GET_ACTIVITIES);
        synchronized (this) {
            mNfcEventInstalledPackages.clear();
            if (DBG2) Log.d(TAG, "updatePackageCache() -- cleared");
            for (int i = 0; i < packagesNfcEvents.size(); i++) {
                if (DBG2)
                    Log.d(
                            TAG,
                            "updatePackageCache() -- added "
                                    + packagesNfcEvents.get(i).packageName);
                mNfcEventInstalledPackages.add(packagesNfcEvents.get(i).packageName);
            }
        }
    }

    int doOpenSecureElementConnection() {
        mEeWakeLock.acquire();
        try {
            return mSecureElement.doOpenSecureElementConnection();
        } finally {
            mEeWakeLock.release();
        }
    }

    byte[] doTransceive(int handle, byte[] cmd) {
        mEeWakeLock.acquire();
        try {
            return doTransceiveNoLock(handle, cmd);
        } finally {
            mEeWakeLock.release();
        }
    }

    byte[] doTransceiveNoLock(int handle, byte[] cmd) {
        return mSecureElement.doTransceive(handle, cmd);
    }

    void doDisconnect(int handle) {
        mEeWakeLock.acquire();
        try {
            mSecureElement.doDisconnect(handle);
        } finally {
            mEeWakeLock.release();
        }
    }

    // At startup, wait for the SE service to be enabled, so we don t reset NFC
    // chip
    // during the SE initialization. For ST54F & H power management reasons.
    void tryWaitForSEService() {
        // Try up to 3 seconds.
        int i = 30;

        while ((mSEService == null) && (i-- >= 0)) {
            boolean res = tryBindSecureElementService();
            // wait 100ms more
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
            }
            if (res == true && mIsLegacySecureElementIntent == true) {
                // We cannot use such binding with wrong intent name.
                // fallback to AOSP method that cannot detect when service closes
                mSEService =
                        ISecureElementService.Stub.asInterface(
                                ServiceManager.getService(Context.SECURE_ELEMENT_SERVICE));
            }
        }
        Log.d(
                TAG,
                "Waited for SE service: "
                        + (30 - i)
                        + " loops , result:"
                        + ((mSEService == null) ? "failed" : "success"));
    }

    /**
     * Manages tasks that involve turning on/off the NFC controller.
     *
     * <p>
     *
     * <p>All work that might turn the NFC adapter on or off must be done through this task, to keep
     * the handling of mState simple. In other words, mState is only modified in these tasks (and we
     * don't need a lock to read it in these tasks).
     *
     * <p>
     *
     * <p>These tasks are all done on the same AsyncTask background thread, so they are serialized.
     * Each task may temporarily transition mState to STATE_TURNING_OFF or STATE_TURNING_ON, but
     * must exit in either STATE_ON or STATE_OFF. This way each task can be guaranteed of starting
     * in either STATE_OFF or STATE_ON, without needing to hold NfcService.this for the entire task.
     *
     * <p>
     *
     * <p>AsyncTask's are also implicitly queued. This is useful for corner cases like turning
     * airplane mode on while TASK_ENABLE is in progress. The TASK_DISABLE triggered by airplane
     * mode will be correctly executed immediately after TASK_ENABLE is complete. This seems like
     * the most sane way to deal with these situations.
     *
     * <p>
     *
     * <p>{@link #TASK_ENABLE} enables the NFC adapter, without changing preferences
     *
     * <p>{@link #TASK_DISABLE} disables the NFC adapter, without changing preferences
     *
     * <p>{@link #TASK_BOOT} does first boot work and may enable NFC
     */
    class EnableDisableTask extends AsyncTask<Integer, Void, Void> {
        @Override
        protected Void doInBackground(Integer... params) {
            // Sanity check mState
            switch (mState) {
                case NfcAdapter.STATE_TURNING_OFF:
                case NfcAdapter.STATE_TURNING_ON:
                    Log.e(
                            TAG,
                            "Processing EnableDisable task "
                                    + params[0]
                                    + " from bad state "
                                    + mState);
                    return null;
            }

            /* AsyncTask sets this thread to THREAD_PRIORITY_BACKGROUND,
             * override with the default. THREAD_PRIORITY_BACKGROUND causes
             * us to service software I2C too slow for firmware download
             * with the NXP PN544.
             * TODO: move this to the DAL I2C layer in libnfc-nxp, since this
             * problem only occurs on I2C platforms using PN544
             */
            Process.setThreadPriority(Process.THREAD_PRIORITY_DEFAULT);

            switch (params[0].intValue()) {
                case TASK_ENABLE:
                    if (DBG2) Log.d(TAG, "EnableDisableTask - TASK_ENABLE");

                    enableInternal();
                    break;
                case TASK_DISABLE:
                    if (DBG2) Log.d(TAG, "EnableDisableTask - TASK_DISABLE");
                    disableInternal();
                    break;
                case TASK_BOOT:
                    boolean initialized;

                    if (DBG2) Log.d(TAG, "EnableDisableTask - TASK_BOOT");

                    if (mPrefs.getBoolean(PREF_FIRST_BOOT, true)) {
                        Log.i(TAG, "First Boot");
                        mPrefsEditor.putBoolean(PREF_FIRST_BOOT, false);
                        mPrefsEditor.apply();
                        mDeviceHost.factoryReset();
                    }
                    Log.d(TAG, "checking on firmware download");
                    if (mPrefs.getBoolean(PREF_NFC_ON, android.os.SystemProperties.getBoolean("persist.sys.default.close.nfc", false) ? false : NFC_ON_DEFAULT)) {
                        Log.d(TAG, "NFC is on. Doing normal stuff");
                        tryWaitForSEService();
                        initialized = enableInternal();
                    } else {
                        Log.d(TAG, "NFC is off.  Checking firmware version");
                        initialized = mDeviceHost.checkFirmware();
                    }
                    if (initialized) {
                        SystemProperties.set("nfc.initialized", "true");
                    }
                    break;
            }

            // Restore default AsyncTask priority
            Process.setThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
            return null;
        }

        /** Enable NFC adapter functions. Does not toggle preferences. */
        boolean enableInternal() {
            if (DBG2) Log.d(TAG, "EnableDisableTask - enableInternal(begin)");

            if (mState == NfcAdapter.STATE_ON) {
                return true;
            }
            Log.i(TAG, "Enabling NFC");
            StatsLog.write(StatsLog.NFC_STATE_CHANGED, StatsLog.NFC_STATE_CHANGED__STATE__ON);
            updateState(NfcAdapter.STATE_TURNING_ON);

            WatchDogThread watchDog = new WatchDogThread("enableInternal", INIT_WATCHDOG_MS);
            watchDog.start();
            try {
                mRoutingWakeLock.acquire();
                try {
                    if (!mDeviceHost.initialize()) {
                        Log.w(TAG, "Error enabling NFC");
                        updateState(NfcAdapter.STATE_OFF);
                        return false;
                    }
                } finally {
                    mRoutingWakeLock.release();
                }
            } finally {
                watchDog.cancel();
            }

            nci_version = getNciVersion();
            Log.d(TAG, "NCI_Version: " + nci_version);

            mPendingRoutingTableUpdate = false;
            mPendingPowerStateUpdate = false;

            synchronized (NfcService.this) {
                mObjectMap.clear();
                if (mIsBeamCapable) {
                    mP2pLinkManager.enableDisable(mIsNdefPushEnabled, true);
                }
                // updateState(NfcAdapter.STATE_ON);
            }

            initSoundPool();

            mScreenState = mScreenStateHelper.checkScreenState();
            int screen_state_mask =
                    (mNfcUnlockManager.isLockscreenPollingEnabled())
                            ? (ScreenStateHelper.SCREEN_POLLING_TAG_MASK | mScreenState)
                            : mScreenState;

            if (mNfcUnlockManager.isLockscreenPollingEnabled()) applyRouting(false);

            mDeviceHost.doSetScreenState(screen_state_mask);

            // MTK - start
            Log.d(TAG, "showNotification()");
            MtkNfcStatusNotificationUi.getInstance().showNotification();
            // MTK - end

            synchronized (NfcService.this) {
                Log.d(TAG, "applyInitializeSequence");
                NfcAddonWrapper.getInstance().applyInitializeSequence();
                // Wallet
                loadWalletConfigFromPref();

                mDeviceHost.setUserDefaultRoutesPref(
                        convertPrefRouteToNfceeIdType(PREF_DEFAULT_MIFARE_ROUTE),
                        convertPrefRouteToNfceeIdType(PREF_DEFAULT_ISODEP_ROUTE),
                        convertPrefRouteToNfceeIdType(PREF_DEFAULT_FELICA_ROUTE),
                        convertPrefRouteToNfceeIdType(PREF_DEFAULT_AB_TECH_ROUTE),
                        convertPrefRouteToNfceeIdType(PREF_DEFAULT_SC_ROUTE),
                        convertPrefRouteToNfceeIdType(PREF_DEFAULT_AID_ROUTE));

                if (mIsHceCapable) {
                    // Generate the initial card emulation routing table
                    mCardEmulationManager.onNfcEnabled();
                }

                updateState(NfcAdapter.STATE_ON);
            }

            /* Start polling loop */
            //            applyRouting(true);
            startPollingLoop();

            if (DBG2) Log.d(TAG, "EnableDisableTask - enableInternal(end)");

            return true;
        }

        /** Disable all NFC adapter functions. Does not toggle preferences. */
        boolean disableInternal() {

            if (DBG2) Log.d(TAG, "EnableDisableTask - disableInternal(begin)");

            if (mState == NfcAdapter.STATE_OFF) {
                return true;
            }
            Log.i(TAG, "Disabling NFC");
            StatsLog.write(StatsLog.NFC_STATE_CHANGED, StatsLog.NFC_STATE_CHANGED__STATE__OFF);
            updateState(NfcAdapter.STATE_TURNING_OFF);

            /* Sometimes mDeviceHost.deinitialize() hangs, use a watch-dog.
             * Implemented with a new thread (instead of a Handler or AsyncTask),
             * because the UI Thread and AsyncTask thread-pools can also get hung
             * when the NFC controller stops responding */
            WatchDogThread watchDog = new WatchDogThread("disableInternal", ROUTING_WATCHDOG_MS);
            watchDog.start();

            mDeviceHost.disableDiscovery();

            if (mIsHceCapable) {
                mCardEmulationManager.onNfcDisabled();
            }

            if (mIsBeamCapable) {
                mP2pLinkManager.enableDisable(false, false);
            }

            // Stop watchdog if tag present
            // A convenient way to stop the watchdog properly consists of
            // disconnecting the tag. The polling loop shall be stopped before
            // to avoid the tag being discovered again.
            maybeDisconnectTarget();

            mNfcDispatcher.setForegroundDispatch(null, null, null);

            NfcAddonWrapper.getInstance().applyDeinitializeSequence();

            boolean result = mDeviceHost.deinitialize();
            if (DBG) Log.d(TAG, "mDeviceHost.deinitialize() = " + result);

            watchDog.cancel();

            synchronized (NfcService.this) {
                mCurrentDiscoveryParameters = NfcDiscoveryParameters.getNfcOffParameters();
                updateState(NfcAdapter.STATE_OFF);
            }

            releaseSoundPool();

            // MTK - start
            Log.d(TAG, "hideNotification()");
            MtkNfcStatusNotificationUi.getInstance().hideNotification();
            // MTK - end
            if (DBG2) Log.d(TAG, "EnableDisableTask - disableInternal(end)");

            return result;
        }

        void updateState(int newState) {
            synchronized (NfcService.this) {
                if (newState == mState) {
                    return;
                }
                mState = newState;
                Intent intent = new Intent(NfcAdapter.ACTION_ADAPTER_STATE_CHANGED);
                intent.setFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
                intent.putExtra(NfcAdapter.EXTRA_ADAPTER_STATE, mState);
                mContext.sendBroadcastAsUser(intent, UserHandle.CURRENT);
            }
        }
    }

    void saveNfcOnSetting(boolean on) {
        synchronized (NfcService.this) {
            mPrefsEditor.putBoolean(PREF_NFC_ON, on);
            mPrefsEditor.apply();
            mBackupManager.dataChanged();
        }
    }

    public void playSound(int sound) {
        synchronized (this) {
            if (mSoundPool == null) {
                Log.w(TAG, "Not playing sound when NFC is disabled");
                return;
            }

            if (mIsVrModeEnabled) {
                Log.d(TAG, "Not playing NFC sound when Vr Mode is enabled");
                return;
            }
            switch (sound) {
                case SOUND_START:
                    mSoundPool.play(mStartSound, 1.0f, 1.0f, 0, 0, 1.0f);
                    break;
                case SOUND_END:
                    mSoundPool.play(mEndSound, 1.0f, 1.0f, 0, 0, 1.0f);
                    break;
                case SOUND_ERROR:
                    mSoundPool.play(mErrorSound, 1.0f, 1.0f, 0, 0, 1.0f);
                    break;
            }
        }
    }

    synchronized int getUserId() {
        return mUserId;
    }

    void enforceBeamShareActivityPolicy(Context context, UserHandle uh) {
        UserManager um = (UserManager) context.getSystemService(Context.USER_SERVICE);
        IPackageManager mIpm =
                IPackageManager.Stub.asInterface(ServiceManager.getService("package"));
        boolean isGlobalEnabled = mIsNdefPushEnabled;
        boolean isActiveForUser =
                (!um.hasUserRestriction(UserManager.DISALLOW_OUTGOING_BEAM, uh))
                        && isGlobalEnabled
                        && mIsBeamCapable;
        if (DBG) {
            Log.d(
                    TAG,
                    "Enforcing a policy change on user: "
                            + uh.toString()
                            + ", isActiveForUser = "
                            + isActiveForUser);
        }
        try {
            mIpm.setComponentEnabledSetting(
                    new ComponentName(
                            BeamShareActivity.class.getPackageName$(),
                            BeamShareActivity.class.getName()),
                    isActiveForUser
                            ? PackageManager.COMPONENT_ENABLED_STATE_ENABLED
                            : PackageManager.COMPONENT_ENABLED_STATE_DISABLED,
                    PackageManager.DONT_KILL_APP,
                    uh.getIdentifier());
        } catch (RemoteException e) {
            Log.w(TAG, "Unable to change Beam status for user " + uh);
        }
    }

    final class NfcSettingsAdapterService extends INfcSettingsAdapter.Stub {

        @Override
        public int getModeFlag(int mode) {
            Log.d(TAG, "NfcSettingsAdapterService - getModeFlag() - mode: " + mode);

            // NfcPermissions.enforceAdminPermissions(mContext);
            return NfcAddonWrapper.getInstance().getModeFlag(mode, NfcService.this);
        }

        @Override
        public void setModeFlag(int mode, int flag) {
            Log.d(
                    TAG,
                    "NfcSettingsAdapterService - setModeFlag() - set mode: "
                            + mode
                            + ", flag: "
                            + flag);
            NfcPermissions.enforceAdminPermissionsClient(mContext);

            int currentHceMode = 0;
            if (mode == NfcSettingsAdapter.MODE_HCE) {
                currentHceMode =
                        NfcAddonWrapper.getInstance()
                                .getModeFlag(NfcSettingsAdapter.MODE_HCE, NfcService.this);
            }

            // NfcPermissions.enforceAdminPermissions(mContext);
            NfcAddonWrapper.getInstance().setModeFlag(isNfcEnabled(), mode, flag, NfcService.this);

            // Update current params
            NfcDiscoveryParameters newParams = computeDiscoveryParameters(mScreenState);
            if (!newParams.equals(mCurrentDiscoveryParameters)) {
                Log.d(TAG, "NfcSettingsAdapterService - setModeFlag() - NewParamRF");
                mCurrentDiscoveryParameters = newParams;
            }

            if (mode == NfcSettingsAdapter.MODE_HCE) {
                if (currentHceMode != flag) {
                    // LMRT needs an update
                    Log.d(
                            TAG,
                            "NfcSettingsAdapterService - setModeFlag() - HCE mode has changed, update routing table");
                    mCardEmulationManager.onUserSettingsChanged();
                }
            }
        }

        @Override
        public boolean isRoutingTableOverflow() {

            boolean status = getAidRoutingTableStatus();
            if (DBG2)
                Log.d(
                        TAG,
                        "NfcSettingsAdapterService - isRoutingTableOverflow() - status: " + status);

            return status;
        }

        @Override
        public boolean isShowOverflowMenu() {

            boolean status = isRoutingTableOverflow();
            List<StApduServiceInfo> services;

            if (status == false) {

                try {
                    services =
                            mCardEmulationManager.getStServices(
                                    getUserId(), CardEmulation.CATEGORY_OTHER);
                } catch (RemoteException e) {
                    Log.e(TAG, "Remote binder has already died.");
                    services = new ArrayList<StApduServiceInfo>();
                }

                /* Show menu if at least 1 service is not active */
                for (StApduServiceInfo srv : services) {
                    int state = srv.getServiceState(CardEmulation.CATEGORY_OTHER);
                    if (state != StConstants.SERVICE_STATE_ENABLED) {
                        status = true;
                        break;
                    }
                }
            }

            if (DBG2)
                Log.d(TAG, "NfcSettingsAdapterService - isShowOverflowMenu() - status: " + status);

            return status;
        }

        final HashMap<ComponentName, ServiceEntry> mServiceEntryMap =
                new HashMap<ComponentName, ServiceEntry>();
        final HashMap<ComponentName, StApduServiceInfo> mStApduServiceInfoMap =
                new HashMap<ComponentName, StApduServiceInfo>();
        final HashMap<ComponentName, Integer> mLmrtOccupiedSize =
                new HashMap<ComponentName, Integer>();

        @Override
        public List<ServiceEntry> getServiceEntryList(int userHandle) {
            if (DBG2) Log.d(TAG, "NfcSettingsAdapterService - getServiceEntryList()");
            NfcPermissions.enforceAdminPermissionsClient(mContext);
            mServiceEntryMap.clear();
            mStApduServiceInfoMap.clear();
            mLmrtOccupiedSize.clear();

            PackageManager pm = mContext.getPackageManager();

            final ArrayList<ServiceEntry> result = new ArrayList<ServiceEntry>();

            List<StApduServiceInfo> services;

            try {
                services =
                        mCardEmulationManager.getStServices(
                                userHandle, CardEmulation.CATEGORY_OTHER);
            } catch (RemoteException e) {
                Log.e(TAG, "Remote binder has already died.");

                services = new ArrayList<StApduServiceInfo>();
            }

            /* We build the result and the internal maps at the same time */
            for (StApduServiceInfo srv : services) {
                ComponentName comp = srv.getComponent();

                Integer sizeInLmrt = new Integer(srv.getCatOthersAidSizeInLmrt());

                Boolean wasEnabled;
                Boolean wantEnabled;

                int state = srv.getServiceState(CardEmulation.CATEGORY_OTHER);
                if (state == StConstants.SERVICE_STATE_ENABLED) {
                    wasEnabled = new Boolean(true);
                    wantEnabled = new Boolean(true);
                } else if (state == StConstants.SERVICE_STATE_DISABLED) {
                    wasEnabled = new Boolean(false);
                    wantEnabled = new Boolean(false);
                } else if (state == StConstants.SERVICE_STATE_ENABLING) {
                    wasEnabled = new Boolean(false);
                    // wantEnabled = new Boolean(true);
                    wantEnabled = new Boolean(false);
                } else if (state == StConstants.SERVICE_STATE_DISABLING) {
                    wasEnabled = new Boolean(true);
                    // wantEnabled = new Boolean(false);
                    wantEnabled = new Boolean(true);
                } else {
                    /* ? */
                    wasEnabled = new Boolean(false);
                    wantEnabled = new Boolean(false);
                }

                String tag = comp.flattenToString();
                // ArrayList<StAidGroup> aidGroups = srv.getStAidGroups();
                String title = srv.getGsmaDescription(pm);

                Integer banner = new Integer(srv.getBannerId());

                ServiceEntry srvEntry =
                        new ServiceEntry(comp, tag, title, banner, wasEnabled, wantEnabled);

                mStApduServiceInfoMap.put(comp, srv);
                mServiceEntryMap.put(comp, srvEntry);
                mLmrtOccupiedSize.put(comp, sizeInLmrt);

                result.add(srvEntry);
                if (DBG2)
                    Log.d(
                            TAG,
                            "NfcSettingsAdapterService - getServiceEntryList() - Found Service: "
                                    + comp.flattenToString()
                                    + ", "
                                    + srv.serviceStateToString(state)
                                    + ", "
                                    + sizeInLmrt
                                    + "B");
            }

            if (DBG2)
                Log.d(
                        TAG,
                        "NfcSettingsAdapterService - getServiceEntryList() - Nb of services found: "
                                + result.size()
                                + " entries");

            return result;
        }

        // This function gives only a very approximative result because we don t
        // recompute
        // anything (mAidServices, mAidCache, etc)
        // This is sufficient for most cases however.
        @Override
        public boolean testServiceEntryList(List<ServiceEntry> proposal) {
            boolean result;
            if (DBG2) Log.d(TAG, "NfcSettingsAdapterService - testServiceEntryList()");
            NfcPermissions.enforceAdminPermissionsClient(mContext);

            int availableSpace = getAidRoutingTableSize(); // total capacity of
            // LMRT
            boolean isOverflow = getAidRoutingTableStatus(); // the size if no
            // overflow
            boolean isOnHostRoute = mAidRoutingManager.isOnHostDefaultRoute(); // fit
            // with
            // any
            // default
            // route

            // Between proposal and current state, what s the change ?
            int deltaOnHost = 0; // for the proposal, what is the impact on LMRT
            // if the default route is
            // onHost
            int deltaOffHost = 0; // for the proposal, what is the impact on
            // LMRT if the default route is
            // offHost

            for (ServiceEntry srvEntry : proposal) {
                ComponentName comp = srvEntry.getComponent();
                StApduServiceInfo srv = mStApduServiceInfoMap.get(comp);
                int state = srv.getServiceState(CardEmulation.CATEGORY_OTHER);
                boolean wasAccounted = srv.getWasAccounted();

                if (wasAccounted != srvEntry.getWantEnabled().booleanValue()) {
                    int delta = 0;
                    Integer sz = mLmrtOccupiedSize.get(comp);
                    if (sz != null) {
                        delta = sz.intValue();
                    }
                    if (srvEntry.getWantEnabled().booleanValue() == false) {
                        // testing disabling this service
                        delta = -delta;
                    }
                    if (srv.isOnHost()) {
                        // changing this service will impact the size of the
                        // LMRT if default is
                        // offHost
                        deltaOffHost += delta;
                    } else {
                        // changing this service will impact the size of the
                        // LMRT if default is
                        // onHost
                        deltaOnHost += delta;
                    }
                }
            }

            if (isOverflow == false) {
                int currentSize = getRoutingTableSizeNotFull();
                result = true;
                if (isOnHostRoute) {
                    if (currentSize + deltaOnHost > availableSpace) {
                        // can it work if we change the default route to
                        // offHost?
                        currentSize = getRoutingTableSizeNotFullAlt();
                        if (currentSize + deltaOffHost > availableSpace) {
                            // still not
                            result = false;
                        }
                    }
                } else {
                    if (currentSize + deltaOffHost > availableSpace) {
                        // can it work if we change the default route to onHost?
                        currentSize = getRoutingTableSizeNotFullAlt();
                        if (currentSize + deltaOnHost > availableSpace) {
                            // still not
                            result = false;
                        }
                    }
                }
            } else {
                int currentSizeForOffHost = getRoutingTableSizeForRoute(0x02);
                int currentSizeForOnHost = getRoutingTableSizeForRoute(0x00);
                result = false;

                if (currentSizeForOnHost + deltaOnHost <= availableSpace) {
                    result = true;
                }
                if (currentSizeForOffHost + deltaOffHost <= availableSpace) {
                    result = true;
                }
            }

            if (DBG)
                Log.d(
                        TAG,
                        "NfcSettingsAdapterService - testServiceEntryList() - test result: "
                                + result);
            return result;
        }

        @Override
        public void commitServiceEntryList(List<ServiceEntry> proposal) {
            if (DBG2) Log.d(TAG, "NfcSettingsAdapterService - commitServiceEntryList()");
            NfcPermissions.enforceAdminPermissionsClient(mContext);
            Map<String, Boolean> serviceStates = new HashMap<String, Boolean>();
            for (ServiceEntry entry : proposal) {
                serviceStates.put(
                        entry.getComponent().flattenToString(),
                        entry.getWantEnabled().booleanValue());
            }
            mCardEmulationManager.updateServiceState(UserHandle.myUserId(), serviceStates);
        }

        @Override
        public List<ServiceEntry> getNonAidBasedServiceEntryList(int userHandle) {
            // if (DBG2) Log.d(TAG,
            // "NfcSettingsAdapterService - getNonAidBasedServiceEntryList()");
            // NfcPermissions.enforceAdminPermissionsClient(mContext);
            //
            // PackageManager pm = mContext.getPackageManager();
            //
            // final ArrayList<ServiceEntry> result = new
            // ArrayList<ServiceEntry>();
            //
            // HashMap<StApduServiceInfo, StNonAidBasedServiceInfo> services;
            //
            // try {
            // services =
            // mCardEmulationManager.getStNonAidBasedServices(userHandle);
            // } catch (RemoteException e) {
            // Log.e(TAG, "Remote binder has already died.");
            //
            // services = new HashMap<StApduServiceInfo,
            // StNonAidBasedServiceInfo>();
            // }
            //
            // for(Map.Entry<StApduServiceInfo, StNonAidBasedServiceInfo> entry
            // : services.entrySet()){
            // StApduServiceInfo stApduServiceInfo = entry.getKey();
            //
            // ComponentName comp = stApduServiceInfo.getComponent();
            // Boolean wasEnabled;
            // Boolean wantEnabled;
            //
            // int state =
            // stApduServiceInfo.getServiceState(CardEmulation.CATEGORY_OTHER);
            // if (state == StConstants.SERVICE_STATE_ENABLED) {
            // wasEnabled = new Boolean(true);
            // wantEnabled = new Boolean(true);
            // } else if (state == StConstants.SERVICE_STATE_DISABLED) {
            // wasEnabled = new Boolean(false);
            // wantEnabled = new Boolean(false);
            // } else if (state == StConstants.SERVICE_STATE_ENABLING) {
            // wasEnabled = new Boolean(false);
            // //wantEnabled = new Boolean(true);
            // wantEnabled = new Boolean(false);
            // } else if (state == StConstants.SERVICE_STATE_DISABLING) {
            // wasEnabled = new Boolean(true);
            // // wantEnabled = new Boolean(false);
            // wantEnabled = new Boolean(true);
            // } else {
            // /* ? */
            // wasEnabled = new Boolean(false);
            // wantEnabled = new Boolean(false);
            // }
            //
            // String tag = comp.flattenToString();
            // // ArrayList<StAidGroup> aidGroups = srv.getStAidGroups();
            // String title = stApduServiceInfo.getGsmaDescription();
            //
            // Integer banner = new Integer(stApduServiceInfo.getBannerId());
            //
            // ServiceEntry srvEntry = new ServiceEntry(comp, tag, title,
            // banner, wasEnabled,
            // wantEnabled);
            //
            // result.add(srvEntry);
            // if (DBG2) Log.d(TAG,
            // "NfcSettingsAdapterService - getNonAidBasedServiceEntryList() - Found Service: "
            // + comp.flattenToString()
            // + ", "
            // + stApduServiceInfo.serviceStateToString(state));
            // }
            //
            // return result;
            return null;
        }

        @Override
        public void commitNonAidBasedServiceEntryList(List<ServiceEntry> proposal) {
            // if (DBG2) Log.d(TAG,
            // "NfcSettingsAdapterService - commitNonAidBasedServiceEntryList()");
            // NfcPermissions.enforceAdminPermissionsClient(mContext);
            // Map<String, Boolean> serviceStates = new HashMap<String,
            // Boolean>();
            // for (ServiceEntry entry : proposal) {
            // serviceStates.put(entry.getComponent().flattenToString(),
            // entry.getWantEnabled().booleanValue());
            // }
            // mCardEmulationManager.updateNonAidBasedServiceState(UserHandle.myUserId(),
            // serviceStates);
        }

        @Override
        public boolean isUiccConnected() {

            return mStExtensions.isUiccConnected();
        }

        /* NCI 2.0 - Begin */
        @Override
        public boolean iseSEConnected() {

            return mStExtensions.iseSEConnected();
        }

        @Override
        public boolean isSEConnected(int HostId) {

            return mStExtensions.isSEConnected(HostId);
        }

        @Override
        public boolean EnableSE(String se_id, boolean enable) {
            Log.i(TAG, "EnableSE() - se_id =  " + se_id + "enable = " + enable);
            NfcPermissions.enforceAdminPermissionsClient(mContext);
            if (se_id.equals("CEE")) {
                // NDEF-EE, not HCI
                Log.i(TAG, "EnableSE() - CEE ");
                return mStExtensions.EnableSE(0x10, enable);
            }
            return NfcAddonWrapper.getInstance().EnableSecureElement(se_id, enable);
        }

        @Override
        public List<String> getSecureElementsStatus() {
            return NfcAddonWrapper.getInstance().getSecureElementsStatus();
        }

        @Override
        public void registerNfcSettingsCallback(INfcSettingsCallback cb) {
            NfcPermissions.enforceAdminPermissionsClient(mContext);
            if (cb != null) NfcAddonWrapper.getInstance().registerNfcSettingsCallback(cb);
        }

        @Override
        public void unregisterNfcSettingsCallback(INfcSettingsCallback cb) {
            NfcPermissions.enforceAdminPermissionsClient(mContext);
            if (cb != null) NfcAddonWrapper.getInstance().unregisterNfcSettingsCallback(cb);
        }

        @Override
        public void setDefaultUserRoutes(List<DefaultRouteEntry> userRoutes) {

            for (DefaultRouteEntry entry : userRoutes) {
                String routeName = entry.getRouteName();
                String routeLoc = entry.getRouteLoc();

                if (DBG2) {
                    Log.d(
                            TAG,
                            "StNfcAdapterService - setDefaultUserRoutes() - "
                                    + routeName
                                    + ": "
                                    + routeLoc);
                }

                switch (routeName) {
                    case NfcSettingsAdapter.DEFAULT_AID_ROUTE:
                        if (mPrefs.getString(
                                        PREF_DEFAULT_AID_ROUTE, NfcSettingsAdapter.DEFAULT_ROUTE)
                                != routeLoc) {
                            mPrefsEditor = mPrefs.edit();
                            mPrefsEditor.putString(PREF_DEFAULT_AID_ROUTE, routeLoc);
                            mPrefsEditor.commit();
                        }
                        break;
                    case NfcSettingsAdapter.DEFAULT_MIFARE_ROUTE:
                        if (mPrefs.getString(
                                        PREF_DEFAULT_MIFARE_ROUTE, NfcSettingsAdapter.DEFAULT_ROUTE)
                                != routeLoc) {
                            mPrefsEditor = mPrefs.edit();
                            mPrefsEditor.putString(PREF_DEFAULT_MIFARE_ROUTE, routeLoc);
                            mPrefsEditor.commit();
                        }
                        break;
                    case NfcSettingsAdapter.DEFAULT_ISO_DEP_ROUTE:
                        if (mPrefs.getString(
                                        PREF_DEFAULT_ISODEP_ROUTE, NfcSettingsAdapter.DEFAULT_ROUTE)
                                != routeLoc) {
                            mPrefsEditor = mPrefs.edit();
                            mPrefsEditor.putString(PREF_DEFAULT_ISODEP_ROUTE, routeLoc);
                            mPrefsEditor.commit();
                        }
                        break;
                    case NfcSettingsAdapter.DEFAULT_FELICA_ROUTE:
                        if (mPrefs.getString(
                                        PREF_DEFAULT_FELICA_ROUTE, NfcSettingsAdapter.DEFAULT_ROUTE)
                                != routeLoc) {
                            mPrefsEditor = mPrefs.edit();
                            mPrefsEditor.putString(PREF_DEFAULT_FELICA_ROUTE, routeLoc);
                            mPrefsEditor.commit();
                        }
                        break;
                    case NfcSettingsAdapter.DEFAULT_AB_TECH_ROUTE:
                        if (mPrefs.getString(
                                        PREF_DEFAULT_AB_TECH_ROUTE,
                                        NfcSettingsAdapter.DEFAULT_ROUTE)
                                != routeLoc) {
                            mPrefsEditor = mPrefs.edit();
                            mPrefsEditor.putString(PREF_DEFAULT_AB_TECH_ROUTE, routeLoc);
                            mPrefsEditor.commit();
                        }
                        break;
                    case NfcSettingsAdapter.DEFAULT_SC_ROUTE:
                        if (mPrefs.getString(
                                        PREF_DEFAULT_SC_ROUTE, NfcSettingsAdapter.DEFAULT_ROUTE)
                                != routeLoc) {
                            mPrefsEditor = mPrefs.edit();
                            mPrefsEditor.putString(PREF_DEFAULT_SC_ROUTE, routeLoc);
                            mPrefsEditor.commit();
                        }
                        break;
                    default:
                        break;
                }
            }

            mDeviceHost.setUserDefaultRoutesPref(
                    convertPrefRouteToNfceeIdType(PREF_DEFAULT_MIFARE_ROUTE),
                    convertPrefRouteToNfceeIdType(PREF_DEFAULT_ISODEP_ROUTE),
                    convertPrefRouteToNfceeIdType(PREF_DEFAULT_FELICA_ROUTE),
                    convertPrefRouteToNfceeIdType(PREF_DEFAULT_AB_TECH_ROUTE),
                    convertPrefRouteToNfceeIdType(PREF_DEFAULT_SC_ROUTE),
                    convertPrefRouteToNfceeIdType(PREF_DEFAULT_AID_ROUTE));

            updateRoutingTable();
        }

        @Override
        public List<DefaultRouteEntry> getDefaultUserRoutes() {
            List<DefaultRouteEntry> userRoutes = new ArrayList<DefaultRouteEntry>();

            if (DBG2) {
                Log.d(TAG, "StNfcAdapterService - getDefaultUserRoutes()");
            }

            userRoutes.add(
                    new DefaultRouteEntry(
                            NfcSettingsAdapter.DEFAULT_AID_ROUTE,
                            mPrefs.getString(
                                    PREF_DEFAULT_AID_ROUTE, NfcSettingsAdapter.DEFAULT_ROUTE)));
            userRoutes.add(
                    new DefaultRouteEntry(
                            NfcSettingsAdapter.DEFAULT_MIFARE_ROUTE,
                            mPrefs.getString(
                                    PREF_DEFAULT_MIFARE_ROUTE, NfcSettingsAdapter.DEFAULT_ROUTE)));
            userRoutes.add(
                    new DefaultRouteEntry(
                            NfcSettingsAdapter.DEFAULT_ISO_DEP_ROUTE,
                            mPrefs.getString(
                                    PREF_DEFAULT_ISODEP_ROUTE, NfcSettingsAdapter.DEFAULT_ROUTE)));
            userRoutes.add(
                    new DefaultRouteEntry(
                            NfcSettingsAdapter.DEFAULT_FELICA_ROUTE,
                            mPrefs.getString(
                                    PREF_DEFAULT_FELICA_ROUTE, NfcSettingsAdapter.DEFAULT_ROUTE)));
            userRoutes.add(
                    new DefaultRouteEntry(
                            NfcSettingsAdapter.DEFAULT_AB_TECH_ROUTE,
                            mPrefs.getString(
                                    PREF_DEFAULT_AB_TECH_ROUTE, NfcSettingsAdapter.DEFAULT_ROUTE)));
            userRoutes.add(
                    new DefaultRouteEntry(
                            NfcSettingsAdapter.DEFAULT_SC_ROUTE,
                            mPrefs.getString(
                                    PREF_DEFAULT_SC_ROUTE, NfcSettingsAdapter.DEFAULT_ROUTE)));

            return userRoutes;
        }

        @Override
        public List<DefaultRouteEntry> getEffectiveRoutes() {
            List<DefaultRouteEntry> userRoutes = new ArrayList<DefaultRouteEntry>();

            if (DBG2) {
                Log.d(TAG, "StNfcAdapterService - getEffectiveRoutes()");
            }

            userRoutes.add(
                    new DefaultRouteEntry(NfcSettingsAdapter.DEFAULT_AID_ROUTE, mUsedAidRoute));
            userRoutes.add(
                    new DefaultRouteEntry(
                            NfcSettingsAdapter.DEFAULT_MIFARE_ROUTE, mUsedMifareRoute));
            userRoutes.add(
                    new DefaultRouteEntry(
                            NfcSettingsAdapter.DEFAULT_ISO_DEP_ROUTE, mUsedIsoDepRoute));
            userRoutes.add(
                    new DefaultRouteEntry(
                            NfcSettingsAdapter.DEFAULT_FELICA_ROUTE, mUsedFelicaRoute));
            userRoutes.add(
                    new DefaultRouteEntry(
                            NfcSettingsAdapter.DEFAULT_AB_TECH_ROUTE, mUsedAbTechRoute));
            userRoutes.add(
                    new DefaultRouteEntry(NfcSettingsAdapter.DEFAULT_SC_ROUTE, mUsedScRoute));

            return userRoutes;
        }

        @Override
        public int getAvailableSpaceForAid() {
            if (DBG2) {
                Log.d(TAG, "StNfcAdapterService - getAvailableSpaceForAid()");
            }

            if (isRoutingTableOverflow()) {
                Log.d(TAG, "StNfcAdapterService - getAvailableSpaceForAid(): overflow");
                return 0;
            }
            int max_for_aid = getAidRoutingTableSize();
            int current_use = getRoutingTableSizeNotFull();
            Log.d(
                    TAG,
                    "StNfcAdapterService - getAvailableSpaceForAid(): max:"
                            + max_for_aid
                            + ", cur:"
                            + current_use);
            return max_for_aid - current_use;
        }
    }

    // MTK - start
    public void updateDefaultRoutesTo(String target) {
        Log.d(TAG, "updateDefaultRoutesTo() - " + target);
        mPrefsEditor.putString(NfcService.PREF_DEFAULT_MIFARE_ROUTE, target);
        mPrefsEditor.putString(NfcService.PREF_DEFAULT_ISODEP_ROUTE, target);
        mPrefsEditor.putString(NfcService.PREF_DEFAULT_FELICA_ROUTE, target);
        mPrefsEditor.putString(NfcService.PREF_DEFAULT_AB_TECH_ROUTE, target);
        mPrefsEditor.putString(NfcService.PREF_DEFAULT_SC_ROUTE, target);
        mPrefsEditor.putString(NfcService.PREF_DEFAULT_AID_ROUTE, target);
        mPrefsEditor.commit();
        // note: the setting will be applied after the new EE is activated.

        mDeviceHost.setUserDefaultRoutesPref(
                convertPrefRouteToNfceeIdType(PREF_DEFAULT_MIFARE_ROUTE),
                convertPrefRouteToNfceeIdType(PREF_DEFAULT_ISODEP_ROUTE),
                convertPrefRouteToNfceeIdType(PREF_DEFAULT_FELICA_ROUTE),
                convertPrefRouteToNfceeIdType(PREF_DEFAULT_AB_TECH_ROUTE),
                convertPrefRouteToNfceeIdType(PREF_DEFAULT_SC_ROUTE),
                convertPrefRouteToNfceeIdType(PREF_DEFAULT_AID_ROUTE));
    }
    // MTK - end

    // MTK - start
    final class MtkNfcSettingsAdapterService
            extends com.mediatek.nfcsettingsadapter.INfcSettingsAdapter.Stub {
        @Override
        public int getModeFlag(int mode) {
            return mNfcSettingsAdapterService.getModeFlag(mode);
        }

        @Override
        public void setModeFlag(int mode, int flag) {
            mNfcSettingsAdapterService.setModeFlag(mode, flag);
        }

        @Override
        public boolean isRoutingTableOverflow() {
            return mNfcSettingsAdapterService.isRoutingTableOverflow();
        }

        @Override
        public boolean isShowOverflowMenu() {
            return mNfcSettingsAdapterService.isShowOverflowMenu();
        }

        @Override
        public List<com.mediatek.nfcsettingsadapter.ServiceEntry> getServiceEntryList(
                int userHandle) {
            List<ServiceEntry> services =
                    mNfcSettingsAdapterService.getServiceEntryList(userHandle);

            final ArrayList<com.mediatek.nfcsettingsadapter.ServiceEntry> result =
                    new ArrayList<com.mediatek.nfcsettingsadapter.ServiceEntry>();
            for (ServiceEntry s : services) {
                com.mediatek.nfcsettingsadapter.ServiceEntry mtkSrvEntry =
                        new com.mediatek.nfcsettingsadapter.ServiceEntry(
                                s.getComponent(),
                                s.getTag(),
                                s.getTitle(),
                                s.getBannerId(),
                                s.getWasEnabled(),
                                s.getWantEnabled());

                result.add(mtkSrvEntry);
            }
            return result;
        }

        @Override
        public boolean testServiceEntryList(
                List<com.mediatek.nfcsettingsadapter.ServiceEntry> proposal) {

            final ArrayList<ServiceEntry> p = new ArrayList<ServiceEntry>();
            for (com.mediatek.nfcsettingsadapter.ServiceEntry m : proposal) {
                ServiceEntry s =
                        new ServiceEntry(
                                m.getComponent(),
                                m.getTag(),
                                m.getTitle(),
                                m.getBannerId(),
                                m.getWasEnabled(),
                                m.getWantEnabled());

                p.add(s);
            }
            return mNfcSettingsAdapterService.testServiceEntryList(p);
        }

        @Override
        public void commitServiceEntryList(
                List<com.mediatek.nfcsettingsadapter.ServiceEntry> proposal) {
            final ArrayList<ServiceEntry> p = new ArrayList<ServiceEntry>();
            for (com.mediatek.nfcsettingsadapter.ServiceEntry m : proposal) {
                ServiceEntry s =
                        new ServiceEntry(
                                m.getComponent(),
                                m.getTag(),
                                m.getTitle(),
                                m.getBannerId(),
                                m.getWasEnabled(),
                                m.getWantEnabled());

                p.add(s);
            }
            mNfcSettingsAdapterService.commitServiceEntryList(p);
        }

        @Override
        public List<com.mediatek.nfcsettingsadapter.ServiceEntry> getNonAidBasedServiceEntryList(
                int userHandle) {
            // NOT SUPPORTED
            return null;
        }

        @Override
        public void commitNonAidBasedServiceEntryList(
                List<com.mediatek.nfcsettingsadapter.ServiceEntry> proposal) {

            // NOT SUPPORTED
        }
    }
    // MTK - END

    final class NfcAdapterService extends INfcAdapter.Stub {
        @Override
        public boolean enable() throws RemoteException {
            if (DBG2) Log.d(TAG, "NfcAdapterService - enable()");
            NfcPermissions.enforceAdminPermissions(mContext);

            saveNfcOnSetting(true);

            new EnableDisableTask().execute(TASK_ENABLE);

            return true;
        }

        @Override
        public boolean disable(boolean saveState) throws RemoteException {
            if (DBG2) Log.d(TAG, "NfcAdapterService - disable() - saveState:" + saveState);
            NfcPermissions.enforceAdminPermissions(mContext);

            if (saveState) {
                saveNfcOnSetting(false);
            }

            new EnableDisableTask().execute(TASK_DISABLE);

            return true;
        }

        @Override
        public void pausePolling(int timeoutInMs) {
            NfcPermissions.enforceAdminPermissions(mContext);

            if (timeoutInMs <= 0 || timeoutInMs > MAX_POLLING_PAUSE_TIMEOUT) {
                Log.e(TAG, "Refusing to pause polling for " + timeoutInMs + "ms.");
                return;
            }

            synchronized (NfcService.this) {
                mPollingPaused = true;
                mDeviceHost.disableDiscovery();
                mHandler.sendMessageDelayed(
                        mHandler.obtainMessage(MSG_RESUME_POLLING), timeoutInMs);
            }
        }

        @Override
        public void resumePolling() {
            NfcPermissions.enforceAdminPermissions(mContext);

            synchronized (NfcService.this) {
                if (!mPollingPaused) {
                    return;
                }

                mHandler.removeMessages(MSG_RESUME_POLLING);
                mPollingPaused = false;
                new ApplyRoutingTask().execute();
            }
        }

        @Override
        public boolean isNdefPushEnabled() throws RemoteException {
            synchronized (NfcService.this) {
                return mState == NfcAdapter.STATE_ON && mIsNdefPushEnabled;
            }
        }

        @Override
        public boolean enableNdefPush() throws RemoteException {
            NfcPermissions.enforceAdminPermissions(mContext);
            synchronized (NfcService.this) {
                if (mIsNdefPushEnabled || !mIsBeamCapable) {
                    return true;
                }
                Log.i(TAG, "enabling NDEF Push");
                mPrefsEditor.putBoolean(PREF_NDEF_PUSH_ON, true);
                mPrefsEditor.apply();
                mIsNdefPushEnabled = true;
                // Propagate the state change to all user profiles
                UserManager um = (UserManager) mContext.getSystemService(Context.USER_SERVICE);
                List<UserHandle> luh = um.getUserProfiles();
                for (UserHandle uh : luh) {
                    enforceBeamShareActivityPolicy(mContext, uh);
                }
                enforceBeamShareActivityPolicy(mContext, new UserHandle(mUserId));
                if (isNfcEnabled()) {
                    mP2pLinkManager.enableDisable(true, true);
                }
                mBackupManager.dataChanged();
            }
            return true;
        }

        @Override
        public boolean isNfcSecureEnabled() throws RemoteException {
            synchronized (NfcService.this) {
                if (DBG2)
                    Log.d(TAG, "NfcAdapterService - isNfcSecureEnabled() - " + mIsSecureNfcEnabled);
                return mIsSecureNfcEnabled;
            }
        }

        @Override
        public boolean setNfcSecure(boolean enable) {
            if (DBG2) Log.d(TAG, "NfcAdapterService - setNfcSecure() - " + enable);
            NfcPermissions.enforceAdminPermissions(mContext);
            if (mKeyguard.isKeyguardLocked() && !enable) {
                Log.i(TAG, "KeyGuard need to be unlocked before setting Secure NFC OFF");
                return false;
            }

            synchronized (NfcService.this) {
                Log.i(TAG, "setting Secure NFC " + enable);
                mPrefsEditor.putBoolean(PREF_SECURE_NFC_ON, enable);
                mPrefsEditor.apply();
                mIsSecureNfcEnabled = enable;
                mBackupManager.dataChanged();
                mDeviceHost.setNfcSecure(enable);
            }
            if (mIsHceCapable) {
                mCardEmulationManager.onSecureNfcToggled();
            }
            if (enable)
                StatsLog.write(
                        StatsLog.NFC_STATE_CHANGED, StatsLog.NFC_STATE_CHANGED__STATE__ON_LOCKED);
            else StatsLog.write(StatsLog.NFC_STATE_CHANGED, StatsLog.NFC_STATE_CHANGED__STATE__ON);
            return true;
        }

        @Override
        public boolean disableNdefPush() throws RemoteException {
            NfcPermissions.enforceAdminPermissions(mContext);
            synchronized (NfcService.this) {
                if (!mIsNdefPushEnabled || !mIsBeamCapable) {
                    return true;
                }
                Log.i(TAG, "disabling NDEF Push");
                mPrefsEditor.putBoolean(PREF_NDEF_PUSH_ON, false);
                mPrefsEditor.apply();
                mIsNdefPushEnabled = false;
                // Propagate the state change to all user profiles
                UserManager um = (UserManager) mContext.getSystemService(Context.USER_SERVICE);
                List<UserHandle> luh = um.getUserProfiles();
                for (UserHandle uh : luh) {
                    enforceBeamShareActivityPolicy(mContext, uh);
                }
                enforceBeamShareActivityPolicy(mContext, new UserHandle(mUserId));
                if (isNfcEnabled()) {
                    mP2pLinkManager.enableDisable(false, true);
                }
                mBackupManager.dataChanged();
            }
            return true;
        }

        @Override
        public void setForegroundDispatch(
                PendingIntent intent, IntentFilter[] filters, TechListParcel techListsParcel) {
            NfcPermissions.enforceUserPermissions(mContext);
            if (!mForegroundUtils.isInForeground(Binder.getCallingUid())) {
                Log.e(TAG, "setForegroundDispatch: Caller not in foreground.");
                return;
            }
            // Short-cut the disable path
            if (intent == null && filters == null && techListsParcel == null) {
                mNfcDispatcher.setForegroundDispatch(null, null, null);
                return;
            }

            // Validate the IntentFilters
            if (filters != null) {
                if (filters.length == 0) {
                    filters = null;
                } else {
                    for (IntentFilter filter : filters) {
                        if (filter == null) {
                            throw new IllegalArgumentException("null IntentFilter");
                        }
                    }
                }
            }

            // Validate the tech lists
            String[][] techLists = null;
            if (techListsParcel != null) {
                techLists = techListsParcel.getTechLists();
            }

            mNfcDispatcher.setForegroundDispatch(intent, filters, techLists);
        }

        @Override
        public void setAppCallback(IAppCallback callback) {
            NfcPermissions.enforceUserPermissions(mContext);

            // don't allow Beam for managed profiles, or devices with a device owner or policy owner
            UserInfo userInfo = mUserManager.getUserInfo(UserHandle.getCallingUserId());
            if (!mUserManager.hasUserRestriction(
                            UserManager.DISALLOW_OUTGOING_BEAM, userInfo.getUserHandle())
                    && mIsBeamCapable) {
                mP2pLinkManager.setNdefCallback(callback, Binder.getCallingUid());
            } else if (DBG) {
                Log.d(TAG, "Disabling default Beam behavior");
            }
        }

        @Override
        public boolean ignore(int nativeHandle, int debounceMs, ITagRemovedCallback callback)
                throws RemoteException {
            NfcPermissions.enforceUserPermissions(mContext);

            if (debounceMs == 0
                    && mDebounceTagNativeHandle != INVALID_NATIVE_HANDLE
                    && nativeHandle == mDebounceTagNativeHandle) {
                // Remove any previous messages and immediately debounce.
                mHandler.removeMessages(MSG_TAG_DEBOUNCE);
                mHandler.sendEmptyMessage(MSG_TAG_DEBOUNCE);
                return true;
            }

            TagEndpoint tag = (TagEndpoint) findAndRemoveObject(nativeHandle);
            if (tag != null) {
                // Store UID and params
                int uidLength = tag.getUid().length;
                synchronized (NfcService.this) {
                    mDebounceTagDebounceMs = debounceMs;
                    mDebounceTagNativeHandle = nativeHandle;
                    mDebounceTagUid = new byte[uidLength];
                    mDebounceTagRemovedCallback = callback;
                    System.arraycopy(tag.getUid(), 0, mDebounceTagUid, 0, uidLength);
                }

                // Disconnect from this tag; this should resume the normal
                // polling loop (and enter listen mode for a while), before
                // we pick up any tags again.
                tag.disconnect();
                mHandler.sendEmptyMessageDelayed(MSG_TAG_DEBOUNCE, debounceMs);
                return true;
            } else {
                return false;
            }
        }

        @Override
        public void verifyNfcPermission() {
            NfcPermissions.enforceUserPermissions(mContext);
        }

        @Override
        public void invokeBeam() {
            if (!mIsBeamCapable) {
                return;
            }
            NfcPermissions.enforceUserPermissions(mContext);

            if (mForegroundUtils.isInForeground(Binder.getCallingUid())) {
                mP2pLinkManager.onManualBeamInvoke(null);
            } else {
                Log.e(TAG, "Calling activity not in foreground.");
            }
        }

        @Override
        public void invokeBeamInternal(BeamShareData shareData) {
            NfcPermissions.enforceAdminPermissions(mContext);
            Message msg = Message.obtain();
            msg.what = MSG_INVOKE_BEAM;
            msg.obj = shareData;
            // We have to send this message delayed for two reasons:
            // 1) This is an IPC call from BeamShareActivity, which is
            //    running when the user has invoked Beam through the
            //    share menu. As soon as BeamShareActivity closes, the UI
            //    will need some time to rebuild the original Activity.
            //    Waiting here for a while gives a better chance of the UI
            //    having been rebuilt, which means the screenshot that the
            //    Beam animation is using will be more accurate.
            // 2) Similarly, because the Activity that launched BeamShareActivity
            //    with an ACTION_SEND intent is now in paused state, the NDEF
            //    callbacks that it has registered may no longer be valid.
            //    Allowing the original Activity to resume will make sure we
            //    it has a chance to re-register the NDEF message / callback,
            //    so we share the right data.
            //
            //    Note that this is somewhat of a hack because the delay may not actually
            //    be long enough for 2) on very slow devices, but there's no better
            //    way to do this right now without additional framework changes.
            mHandler.sendMessageDelayed(msg, INVOKE_BEAM_DELAY_MS);
        }

        @Override
        public INfcTag getNfcTagInterface() throws RemoteException {
            return mNfcTagService;
        }

        @Override
        public INfcCardEmulation getNfcCardEmulationInterface() {
            if (mIsHceCapable) {
                return mCardEmulationManager.getNfcCardEmulationInterface();
            } else {
                return null;
            }
        }

        @Override
        public INfcFCardEmulation getNfcFCardEmulationInterface() {
            if (mIsHceFCapable) {
                return mCardEmulationManager.getNfcFCardEmulationInterface();
            } else {
                return null;
            }
        }

        @Override
        public int getState() throws RemoteException {
            synchronized (NfcService.this) {
                return mState;
            }
        }

        @Override
        protected void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
            NfcService.this.dump(fd, pw, args);
        }

        @Override
        public void dispatch(Tag tag) throws RemoteException {
            NfcPermissions.enforceAdminPermissions(mContext);
            mNfcDispatcher.dispatchTag(tag);
        }

        @Override
        public void setP2pModes(int initiatorModes, int targetModes) throws RemoteException {
            NfcPermissions.enforceAdminPermissions(mContext);
            mDeviceHost.setP2pInitiatorModes(initiatorModes);
            mDeviceHost.setP2pTargetModes(targetModes);
            applyRouting(true);
        }

        @Override
        public void setReaderMode(IBinder binder, IAppCallback callback, int flags, Bundle extras)
                throws RemoteException {
            if (DBG2) Log.d(TAG, "setReaderMode() flags:" + flags);
            int callingUid = Binder.getCallingUid();
            if (callingUid != Process.SYSTEM_UID && !mForegroundUtils.isInForeground(callingUid)) {
                Log.e(TAG, "setReaderMode: Caller is not in foreground and is not system process.");
                return;
            }
            synchronized (NfcService.this) {
                if (!isNfcEnabled()) {
                    Log.e(TAG, "setReaderMode() called while NFC is not enabled.");
                    return;
                }
                if (flags != 0) {
                    try {
                        mReaderModeParams = new ReaderModeParams();
                        mReaderModeParams.callback = callback;
                        mReaderModeParams.flags = flags;
                        mReaderModeParams.presenceCheckDelay =
                                extras != null
                                        ? (extras.getInt(
                                                NfcAdapter.EXTRA_READER_PRESENCE_CHECK_DELAY,
                                                DEFAULT_PRESENCE_CHECK_DELAY))
                                        : DEFAULT_PRESENCE_CHECK_DELAY;
                        binder.linkToDeath(mReaderModeDeathRecipient, 0);
                    } catch (RemoteException e) {
                        Log.e(TAG, "Remote binder has already died.");
                        return;
                    }
                } else {
                    try {
                        mReaderModeParams = null;
                        StopPresenceChecking();
                        binder.unlinkToDeath(mReaderModeDeathRecipient, 0);
                    } catch (NoSuchElementException e) {
                        Log.e(TAG, "Reader mode Binder was never registered.");
                    }
                }

                // Force restart of polling loop whenever reader mode is started/stopped
                applyRouting(true);
            }
        }

        @Override
        public INfcAdapterExtras getNfcAdapterExtrasInterface(String pkg) throws RemoteException {
            if (DBG2) Log.d(TAG, "getNfcAdapterExtrasInterface() pkg:" + pkg);
            NfcService.this.enforceNfceeAdminPerm(pkg);
            return mExtrasService;
        }

        @Override
        public INfcDta getNfcDtaInterface(String pkg) throws RemoteException {
            NfcPermissions.enforceAdminPermissions(mContext);
            if (mNfcDtaService == null) {
                mNfcDtaService = new NfcDtaService();
            }
            return mNfcDtaService;
        }

        @Override
        public void addNfcUnlockHandler(INfcUnlockHandler unlockHandler, int[] techList) {
            NfcPermissions.enforceAdminPermissions(mContext);

            int lockscreenPollMask = computeLockscreenPollMask(techList);
            synchronized (NfcService.this) {
                mNfcUnlockManager.addUnlockHandler(unlockHandler, lockscreenPollMask);
            }

            applyRouting(false);
        }

        @Override
        public void removeNfcUnlockHandler(INfcUnlockHandler token) throws RemoteException {
            synchronized (NfcService.this) {
                mNfcUnlockManager.removeUnlockHandler(token.asBinder());
            }

            applyRouting(false);
        }

        @Override
        public boolean deviceSupportsNfcSecure() {
            String skuList[] =
                    mContext.getResources().getStringArray(R.array.config_skuSupportsSecureNfc);
            String sku = SystemProperties.get("ro.boot.hardware.sku");
            if (TextUtils.isEmpty(sku) || !ArrayUtils.contains(skuList, sku)) {
                if (DBG2) Log.d(TAG, "NfcAdapterService - deviceSupportsNfcSecure() - false");
                return false;
            }
            if (DBG2) Log.d(TAG, "NfcAdapterService - deviceSupportsNfcSecure() - true");
            return true;
        }

        private int computeLockscreenPollMask(int[] techList) {

            Map<Integer, Integer> techCodeToMask = new HashMap<Integer, Integer>();

            techCodeToMask.put(TagTechnology.NFC_A, NfcService.NFC_POLL_A);
            techCodeToMask.put(TagTechnology.NFC_B, NfcService.NFC_POLL_B);
            techCodeToMask.put(TagTechnology.NFC_V, NfcService.NFC_POLL_V);
            techCodeToMask.put(TagTechnology.NFC_F, NfcService.NFC_POLL_F);
            techCodeToMask.put(TagTechnology.NFC_BARCODE, NfcService.NFC_POLL_KOVIO);

            int mask = 0;

            for (int i = 0; i < techList.length; i++) {
                if (techCodeToMask.containsKey(techList[i])) {
                    mask |= techCodeToMask.get(techList[i]).intValue();
                }
            }

            return mask;
        }
    }

    final class ReaderModeDeathRecipient implements IBinder.DeathRecipient {
        @Override
        public void binderDied() {
            if (DBG2) Log.d(TAG, "binderDied() mReaderModeParams:" + mReaderModeParams);
            synchronized (NfcService.this) {
                if (mReaderModeParams != null) {
                    mReaderModeParams = null;
                    applyRouting(false);
                }
            }
        }
    }

    final class StExtrasService extends INfcAdapterStExtensions.Stub {

        /* NCI 2.0 - End */
        @Override
        public byte[] getFirmwareVersion() {
            byte[] result = mStExtensions.getFirmwareVersion();
            return result;
        }

        @Override
        public byte[] getHWVersion() {
            byte[] result = mStExtensions.getHWVersion();
            return result;
        }

        @Override
        public byte[] getSWVersion() {
            byte[] result = mStExtensions.getSWVersion();
            return result;
        }

        @Override
        public int loopback() {
            int res = mStExtensions.loopback();
            return res;
        }

        // @Override
        // public void setNfcSystemProperty(String key, String value) {
        // if(key.contentEquals("nfc.st.hce_capable")){
        // NfcAddonWrapper.getInstance().setModeFlag(isNfcEnabled(),
        // NfcSettingsAdapter.MODE_HCE,
        // value.contentEquals("1") ?
        // NfcSettingsAdapter.FLAG_ON:NfcSettingsAdapter.FLAG_OFF,
        // NfcService.this);
        // } else {
        // mStExtensions.setNfcSystemProperty(key, value);
        // }
        // }

        @Override
        public boolean getHceCapability() {
            return mIsHceCapable;
        }

        @Override
        public void setRfConfiguration(int modeBitmap, byte[] techArray) {
            Log.i(TAG, "setRfConfiguration() -  modeBitmap" + String.valueOf(modeBitmap));
            synchronized (NfcService.this) {
                mStExtensions.setRfConfiguration(modeBitmap, techArray);
            }
        }

        @Override
        public int getRfConfiguration(byte[] techArray) {
            Log.i(TAG, "getRfConfiguration() ");
            return mStExtensions.getRfConfiguration(techArray);
        }

        @Override
        public void setRfBitmap(int modeBitmap) {
            Log.i(TAG, "setRfBitmap() - modeBitmap: " + Integer.toHexString(modeBitmap));
            mStExtensions.setRfBitmap(modeBitmap);
        }

        @Override
        public boolean getProprietaryConfigSettings(int SubSetID, int byteNb, int bitNb) {
            boolean status = mStExtensions.getProprietaryConfigSettings(SubSetID, byteNb, bitNb);
            Log.i(
                    TAG,
                    "getProprietaryConfigSettings() SubSet ID "
                            + Integer.toHexString(SubSetID)
                            + "- status =  "
                            + status);
            return status;
        }

        @Override
        public void setProprietaryConfigSettings(
                int SubSetID, int byteNb, int bitNb, boolean status) {
            Log.i(
                    TAG,
                    "setProprietaryConfigSettings() - byteNb = "
                            + byteNb
                            + " bitNb = "
                            + bitNb
                            + " status =  "
                            + status);
            mStExtensions.setProprietaryConfigSettings(SubSetID, byteNb, bitNb, status);
        }

        @Override
        public int getPipesList(int hostId, byte[] list) {
            Log.i(TAG, "getPipesList() - hostId = " + hostId);
            // return mStExtensions.getPipesList(hostId, list);
            int nbPipes = mStExtensions.getPipesList(hostId, list);

            for (int i = 0; i < nbPipes; i++) {
                Log.i(TAG, "getPipesList() - list[" + i + "] = " + list[i]);
            }

            return nbPipes;
        }

        @Override
        public void getPipeInfo(int hostId, int pipeId, byte[] info) {
            Log.i(TAG, "getPipeInfo() - hostId =  " + hostId + "pipeId = " + pipeId);
            mStExtensions.getPipeInfo(hostId, pipeId, info);
            for (int i = 0; i < 4; i++) {
                Log.i(TAG, "getPipesList() - info[" + i + "] = " + info[i]);
            }
        }

        @Override
        public byte[] getATR() {
            Log.i(TAG, "getATR()");
            return mStExtensions.getATR();
        }

        public boolean connectEE(int ceeId) {
            Log.i(TAG, "connectEE() - ceeId =  " + ceeId);
            return mStExtensions.connectEE(ceeId);
        }

        public byte[] transceiveEE(int cee_id, byte[] dataCmd) {
            Log.i(TAG, "transceiveEE() - ceeId =  " + cee_id);
            return mStExtensions.transceiveEE(cee_id, dataCmd);
        }

        public boolean disconnectEE(int cee_id) {
            Log.i(TAG, "disconnectEE() - ceeId =  " + cee_id);
            return mStExtensions.disconnectEE(cee_id);
        }

        public int connectGate(int host_id, int gate_id) {
            Log.i(TAG, "connectGate() - host_id =  " + host_id + " - gate_id = " + gate_id);
            return mStExtensions.connectGate(gate_id, host_id);
        }

        public byte[] transceive(int pipe_id, int hciCmd, byte[] dataIn) {
            Log.i(TAG, "transceive() - pipe_id =  " + pipe_id + " - HCI cmd = " + hciCmd);
            return mStExtensions.transceive(pipe_id, hciCmd, dataIn);
        }

        public void disconnectGate(int pipe_id) {
            Log.i(TAG, "disconnectGate() - pipe_id =  " + pipe_id);
            mStExtensions.disconnectGate(pipe_id);
        }

        /* NCI 2.0 - Begin */
        public void setNciConfig(int param_id, byte[] param) {
            mStExtensions.setNciConfig(param_id, param, param.length);
        }

        public byte[] getNciConfig(int param_id) {
            return mStExtensions.getNciConfig(param_id);
        }

        public int getAvailableHciHostList(byte[] nfceeId, byte[] conInfo) {
            return mStExtensions.getAvailableHciHostList(nfceeId, conInfo);
        }

        public boolean getBitPropConfig(int configId, int byteNb, int bitNb) {
            return mStExtensions.getProprietaryConfigSettings(configId, byteNb, bitNb);
        }

        public void setBitPropConfig(int configId, int byteNb, int bitNb, boolean status) {
            mStExtensions.setProprietaryConfigSettings(configId, byteNb, bitNb, status);
        }

        public void forceRouting(int nfceeId, int power) {
            mDeviceHost.forceRouting(nfceeId);
        }

        public void stopforceRouting() {
            mDeviceHost.stopforceRouting();
        }

        public void sendPropSetConfig(int subSetId, int configId, byte[] param) {
            mStExtensions.sendPropSetConfig(subSetId, configId, param, param.length);
        }

        public byte[] sendPropGetConfig(int subSetId, int configId) {
            Log.d(TAG, "sendPropGetConfig() ");
            return mStExtensions.sendPropGetConfig(subSetId, configId);
        }

        public byte[] sendPropTestCmd(int subCode, byte[] paramTx) {
            return mStExtensions.sendPropTestCmd(subCode, paramTx, paramTx.length);
        }

        public byte[] getCustomerData() {
            Log.d(TAG, "getCustomerData() ");
            byte[] result = mStExtensions.getCustomerData();
            return result;
        }

        @Override
        public INfcAdapterStDtaExtensions getNfcAdapterStDtaExtensionsInterface() {
            return mStDtaExtras;
        }

        @Override
        public INfcWalletAdapter getNfcWalletAdapterInterface() {
            return mNfcWalletAdapter;
        }
    }

    private boolean muteARequested = false;
    private boolean muteBRequested = false;
    private boolean muteFRequested = false;

    public void loadWalletConfigFromPref() {
        muteARequested = mPrefs.getBoolean(PREF_WALLET_MUTE_A, false);
        muteBRequested = mPrefs.getBoolean(PREF_WALLET_MUTE_B, false);
        muteFRequested = mPrefs.getBoolean(PREF_WALLET_MUTE_F, false);
        applyWalletConfigIfNeeded(false);
    }

    public boolean applyWalletConfigIfNeeded(boolean isCommitNeeded) {
        Log.i(TAG, "applyWalletConfigIfNeeded() - isCommitNeeded: " + isCommitNeeded);
        boolean isESeActive = mStExtensions.iseSEConnected();
        boolean ret = false;

        if (isESeActive) {
            ret =
                    mDeviceHost.doSetMuteTech(
                            muteARequested, muteBRequested, muteFRequested, isCommitNeeded);
        } else {
            ret = mDeviceHost.doSetMuteTech(false, false, false, isCommitNeeded);
        }
        return ret;
    }

    final class NfcWalletAdapterService extends INfcWalletAdapter.Stub {
        private int mHandle;
        private INfcWalletLogCallback mLogCallback;

        public boolean keepEseSwpActive(boolean enable) {
            Log.i(TAG, "keepEseSwpActive(" + enable + ")");
            synchronized (NfcService.this) {
                if (!isNfcEnabled()) {
                    Log.e(TAG, "keepEseSwpActive - NFC is not enabled");
                    return false;
                }
                if (enable) {
                    if (mOpenEe == null) {
                        mHandle = doOpenSecureElementConnection();
                        if (mHandle < 0) {
                            mHandle = 0;
                            return false;
                        }
                    } else {
                        Log.i(TAG, "keepEseSwpActive - APDU gate already open");
                    }
                } else {
                    if (mOpenEe != null) {
                        Log.i(TAG, "keepEseSwpActive - APDU gate is open");
                    } else if (mHandle != 0) {
                        doDisconnect(mHandle);
                        mHandle = 0;
                    }
                }
            }
            return true;
        }

        public boolean setMuteTech(boolean muteA, boolean muteB, boolean muteF) {
            Log.i(TAG, "setMuteTech(" + muteA + ", " + muteB + ", " + muteF + ")");
            muteARequested = muteA;
            muteBRequested = muteB;
            muteFRequested = muteF;

            mPrefsEditor = mPrefs.edit();
            mPrefsEditor.putBoolean(PREF_WALLET_MUTE_A, muteA);
            mPrefsEditor.putBoolean(PREF_WALLET_MUTE_B, muteB);
            mPrefsEditor.putBoolean(PREF_WALLET_MUTE_F, muteF);
            mPrefsEditor.commit();

            return applyWalletConfigIfNeeded(true);
        }

        public boolean setObserverMode(boolean enable) {
            Log.i(TAG, "setObserverMode(" + enable + ")");
            return mDeviceHost.setObserverMode(enable);
        }

        public boolean registerStLogCallback(INfcWalletLogCallback cb) {
            synchronized (NfcWalletAdapterService.this) {
                mLogCallback = cb;
                mDeviceHost.enableStLog(true);
            }
            return true;
        }

        public void onStLogData(int logtype, byte[][] data) {
            Log.d(TAG, "onStLogData : Number of entries = " + data.length);
            synchronized (NfcWalletAdapterService.this) {
                if (mLogCallback != null) {
                    for (byte[] tlv : data) {
                        try {
                            mLogCallback.onFwLogReceived(logtype, tlv);
                        } catch (RemoteException e) {
                            // Ignore
                        }
                    }
                }
            }
        }

        public boolean unregisterStLogCallback() {
            synchronized (NfcWalletAdapterService.this) {
                mDeviceHost.enableStLog(false);
                mLogCallback = null;
            }
            return true;
        }

        public boolean rotateRfParameters(boolean reset) {
            Log.i(TAG, "rotateRfParameters(" + reset + ")");
            return mDeviceHost.rotateRfParameters(reset);
        }
    }

    final class StDtaExtrasService extends INfcAdapterStDtaExtensions.Stub {

        public boolean initialize() {

            Log.i(TAG, "initialize()");
            boolean result = mStDtaExtensions.initialize();
            return result;
        }

        public boolean deinitialize() {

            Log.i(TAG, "deinitialize()");
            boolean result = mStDtaExtensions.deinitialize();
            return result;
        }

        public void setPatternNb(int iPatternNb) {

            Log.i(TAG, "setPatternNb() " + iPatternNb);
            mStDtaExtensions.setPatternNb(iPatternNb);
        }

        public void setCrVersion(byte bCrVersion) {

            Log.i(TAG, "setCrVersion() " + bCrVersion);
            mStDtaExtensions.setCrVersion(bCrVersion);
        }

        public void setConnectionDevicesLimit(byte bCdlA, byte bCdlB, byte bCdlF, byte bCdlV) {

            Log.i(
                    TAG,
                    "setConnectionDevicesLimit() typeA: "
                            + bCdlA
                            + ", typeB: "
                            + bCdlB
                            + ", typeF: "
                            + bCdlF
                            + ", typeV: "
                            + bCdlV);
            mStDtaExtensions.setConnectionDevicesLimit(bCdlA, bCdlB, bCdlF, bCdlV);
        }

        public void setListenNfcaUidMode(byte bMode) {

            Log.i(TAG, "setListenNfcaUidMode() mode: " + bMode);
            mStDtaExtensions.setListenNfcaUidMode(bMode);
        }

        public void setT4atNfcdepPrio(byte prio) {

            Log.i(TAG, "setT4atNfcdepPrio() priority: " + prio);
            mStDtaExtensions.setT4atNfcdepPrio(prio);
        }

        public void setFsdFscExtension(boolean ext) {

            Log.i(TAG, "setFsdFscExtension() extension support: " + ext);
            mStDtaExtensions.setFsdFscExtension(ext);
        }

        public void setLlcpMode(int miux_mode) {

            Log.i(TAG, "setLlcpMode() miux_mode: " + miux_mode);
            mStDtaExtensions.setLlcpMode(miux_mode);
        }

        public void setSnepMode(
                byte role,
                byte server_type,
                byte request_type,
                byte data_type,
                boolean disc_incorrect_len) {

            Log.i(
                    TAG,
                    "setSnepMode() role: "
                            + role
                            + ", server_type: "
                            + server_type
                            + ", request_type: "
                            + request_type
                            + ", data_type: "
                            + data_type
                            + ", disc_incorrect_len: "
                            + disc_incorrect_len);
            mStDtaExtensions.setSnepMode(
                    role, server_type, request_type, data_type, disc_incorrect_len);
        }

        public int enableDiscovery(
                byte con_poll,
                byte con_listen_dep,
                byte con_listen_t4tp,
                boolean con_listen_t3tp,
                boolean con_listen_acm,
                byte con_bitr_f,
                byte con_bitr_acm) {

            Log.i(
                    TAG,
                    "enableDiscovery() con_poll: "
                            + con_poll
                            + ", con_listen_dep: "
                            + con_listen_dep
                            + ", con_listen_t4tp: "
                            + con_listen_t4tp
                            + ", con_listen_t3tp: "
                            + con_listen_t3tp
                            + ", con_listen_acm: "
                            + con_listen_acm
                            + ", con_bitr_f: "
                            + con_bitr_f
                            + ", con_bitr_acm: "
                            + con_bitr_acm);
            int result =
                    mStDtaExtensions.enableDiscovery(
                            con_poll,
                            con_listen_dep,
                            con_listen_t4tp,
                            con_listen_t3tp,
                            con_listen_acm,
                            con_bitr_f,
                            con_bitr_acm);
            return result;
        }

        public boolean disableDiscovery() {

            Log.i(TAG, "disableDiscovery()");
            boolean result = mStDtaExtensions.disableDiscovery();
            return result;
        }
    }

    final class TagService extends INfcTag.Stub {
        @Override
        public int connect(int nativeHandle, int technology) throws RemoteException {
            NfcPermissions.enforceUserPermissions(mContext);

            TagEndpoint tag = null;
            if (DBG2) {
                Log.d(
                        TAG,
                        "TagService - connect() -nativeHandle:"
                                + nativeHandle
                                + " technology:"
                                + technology);
            }

            if (!isNfcEnabled()) {
                return ErrorCodes.ERROR_NOT_INITIALIZED;
            }

            /* find the tag in the hmap */
            tag = (TagEndpoint) findObject(nativeHandle);
            if (tag == null) {
                return ErrorCodes.ERROR_DISCONNECT;
            }

            if (!tag.isPresent()) {
                return ErrorCodes.ERROR_DISCONNECT;
            }

            // Note that on most tags, all technologies are behind a single
            // handle. This means that the connect at the lower levels
            // will do nothing, as the tag is already connected to that handle.
            if (tag.connect(technology)) {
                return ErrorCodes.SUCCESS;
            } else {
                return ErrorCodes.ERROR_DISCONNECT;
            }
        }

        @Override
        public int reconnect(int nativeHandle) throws RemoteException {
            NfcPermissions.enforceUserPermissions(mContext);
            if (DBG2) Log.d(TAG, "TagService - reconnect() - nativeHandle:" + nativeHandle);

            TagEndpoint tag = null;

            // Check if NFC is enabled
            if (!isNfcEnabled()) {
                return ErrorCodes.ERROR_NOT_INITIALIZED;
            }

            /* find the tag in the hmap */
            tag = (TagEndpoint) findObject(nativeHandle);
            if (tag != null) {

                if (!tag.isPresent()) {
                    return ErrorCodes.ERROR_DISCONNECT;
                }

                if (tag.reconnect()) {
                    return ErrorCodes.SUCCESS;
                } else {
                    return ErrorCodes.ERROR_DISCONNECT;
                }
            }
            return ErrorCodes.ERROR_DISCONNECT;
        }

        @Override
        public int[] getTechList(int nativeHandle) throws RemoteException {
            NfcPermissions.enforceUserPermissions(mContext);

            // Check if NFC is enabled
            if (!isNfcEnabled()) {
                return null;
            }

            /* find the tag in the hmap */
            TagEndpoint tag = (TagEndpoint) findObject(nativeHandle);
            if (tag != null) {
                return tag.getTechList();
            }
            return null;
        }

        @Override
        public boolean isPresent(int nativeHandle) throws RemoteException {
            TagEndpoint tag = null;
            if (DBG2) Log.d(TAG, "TagService - isPresent() -nativeHandle:" + nativeHandle);

            // Check if NFC is enabled
            if (!isNfcEnabled()) {
                return false;
            }

            /* find the tag in the hmap */
            tag = (TagEndpoint) findObject(nativeHandle);
            if (tag == null) {
                return false;
            }

            return tag.isPresent();
        }

        @Override
        public boolean isNdef(int nativeHandle) throws RemoteException {
            NfcPermissions.enforceUserPermissions(mContext);
            if (DBG2) Log.d(TAG, "TagService - isNdef() -nativeHandle:" + nativeHandle);

            TagEndpoint tag = null;

            // Check if NFC is enabled
            if (!isNfcEnabled()) {
                return false;
            }

            /* find the tag in the hmap */
            tag = (TagEndpoint) findObject(nativeHandle);
            int[] ndefInfo = new int[2];
            if (tag == null) {
                return false;
            }
            return tag.checkNdef(ndefInfo);
        }

        @Override
        public TransceiveResult transceive(int nativeHandle, byte[] data, boolean raw)
                throws RemoteException {
            NfcPermissions.enforceUserPermissions(mContext);
            if (DBG2) Log.d(TAG, "TagService - transceive() - nativeHandle:" + nativeHandle);

            TagEndpoint tag = null;
            byte[] response;

            // Check if NFC is enabled
            if (!isNfcEnabled()) {
                return null;
            }

            /* find the tag in the hmap */
            tag = (TagEndpoint) findObject(nativeHandle);
            if (tag != null) {
                // Check if length is within limits
                if (data.length > getMaxTransceiveLength(tag.getConnectedTechnology())) {
                    return new TransceiveResult(TransceiveResult.RESULT_EXCEEDED_LENGTH, null);
                }
                int[] targetLost = new int[1];
                response = tag.transceive(data, raw, targetLost);
                int result;
                if (response != null) {
                    result = TransceiveResult.RESULT_SUCCESS;
                } else if (targetLost[0] == 1) {
                    result = TransceiveResult.RESULT_TAGLOST;
                } else {
                    result = TransceiveResult.RESULT_FAILURE;
                }
                return new TransceiveResult(result, response);
            }
            return null;
        }

        @Override
        public NdefMessage ndefRead(int nativeHandle) throws RemoteException {
            NfcPermissions.enforceUserPermissions(mContext);
            if (DBG2) Log.d(TAG, "TagService - ndefRead() - nativeHandle:" + nativeHandle);

            TagEndpoint tag;

            // Check if NFC is enabled
            if (!isNfcEnabled()) {
                return null;
            }

            /* find the tag in the hmap */
            tag = (TagEndpoint) findObject(nativeHandle);
            if (tag != null) {
                byte[] buf = tag.readNdef();
                if (buf == null) {
                    return null;
                }

                /* Create an NdefMessage */
                try {
                    return new NdefMessage(buf);
                } catch (FormatException e) {
                    return null;
                }
            }
            return null;
        }

        @Override
        public int ndefWrite(int nativeHandle, NdefMessage msg) throws RemoteException {

            if (DBG2) Log.d(TAG, "TagService - ndefWrite() - msg:" + msg.toString());

            NfcPermissions.enforceUserPermissions(mContext);

            TagEndpoint tag;

            // Check if NFC is enabled
            if (!isNfcEnabled()) {
                return ErrorCodes.ERROR_NOT_INITIALIZED;
            }

            /* find the tag in the hmap */
            tag = (TagEndpoint) findObject(nativeHandle);
            if (tag == null) {
                return ErrorCodes.ERROR_IO;
            }

            if (msg == null) return ErrorCodes.ERROR_INVALID_PARAM;

            if (tag.writeNdef(msg.toByteArray())) {
                return ErrorCodes.SUCCESS;
            } else {
                return ErrorCodes.ERROR_IO;
            }
        }

        @Override
        public boolean ndefIsWritable(int nativeHandle) throws RemoteException {
            throw new UnsupportedOperationException();
        }

        @Override
        public int ndefMakeReadOnly(int nativeHandle) throws RemoteException {
            NfcPermissions.enforceUserPermissions(mContext);

            TagEndpoint tag;

            // Check if NFC is enabled
            if (!isNfcEnabled()) {
                return ErrorCodes.ERROR_NOT_INITIALIZED;
            }

            /* find the tag in the hmap */
            tag = (TagEndpoint) findObject(nativeHandle);
            if (tag == null) {
                return ErrorCodes.ERROR_IO;
            }

            if (tag.makeReadOnly()) {
                return ErrorCodes.SUCCESS;
            } else {
                return ErrorCodes.ERROR_IO;
            }
        }

        @Override
        public int formatNdef(int nativeHandle, byte[] key) throws RemoteException {
            NfcPermissions.enforceUserPermissions(mContext);
            if (DBG2) Log.d(TAG, "TagService - formatNdef() nativeHandle:" + nativeHandle);

            TagEndpoint tag;

            // Check if NFC is enabled
            if (!isNfcEnabled()) {
                return ErrorCodes.ERROR_NOT_INITIALIZED;
            }

            /* find the tag in the hmap */
            tag = (TagEndpoint) findObject(nativeHandle);
            if (tag == null) {
                return ErrorCodes.ERROR_IO;
            }

            if (tag.formatNdef(key)) {
                return ErrorCodes.SUCCESS;
            } else {
                return ErrorCodes.ERROR_IO;
            }
        }

        @Override
        public Tag rediscover(int nativeHandle) throws RemoteException {
            NfcPermissions.enforceUserPermissions(mContext);
            if (DBG2) Log.d(TAG, "TagService - rediscover() - nativeHandle:" + nativeHandle);

            TagEndpoint tag = null;

            // Check if NFC is enabled
            if (!isNfcEnabled()) {
                return null;
            }

            /* find the tag in the hmap */
            tag = (TagEndpoint) findObject(nativeHandle);
            if (tag != null) {
                // For now the prime usecase for rediscover() is to be able
                // to access the NDEF technology after formatting without
                // having to remove the tag from the field, or similar
                // to have access to NdefFormatable in case low-level commands
                // were used to remove NDEF. So instead of doing a full stack
                // rediscover (which is poorly supported at the moment anyway),
                // we simply remove these two technologies and detect them
                // again.
                tag.removeTechnology(TagTechnology.NDEF);
                tag.removeTechnology(TagTechnology.NDEF_FORMATABLE);
                tag.findAndReadNdef();
                // Build a new Tag object to return
                Tag newTag =
                        new Tag(
                                tag.getUid(),
                                tag.getTechList(),
                                tag.getTechExtras(),
                                tag.getHandle(),
                                this);
                return newTag;
            }
            return null;
        }

        @Override
        public int setTimeout(int tech, int timeout) throws RemoteException {
            NfcPermissions.enforceUserPermissions(mContext);
            if (DBG2)
                Log.d(TAG, "TagService - setTimeout() - tech:" + tech + " timeout:" + timeout);
            boolean success = mDeviceHost.setTimeout(tech, timeout);
            if (success) {
                return ErrorCodes.SUCCESS;
            } else {
                return ErrorCodes.ERROR_INVALID_PARAM;
            }
        }

        @Override
        public int getTimeout(int tech) throws RemoteException {
            NfcPermissions.enforceUserPermissions(mContext);

            return mDeviceHost.getTimeout(tech);
        }

        @Override
        public void resetTimeouts() throws RemoteException {
            NfcPermissions.enforceUserPermissions(mContext);

            mDeviceHost.resetTimeouts();
        }

        @Override
        public boolean canMakeReadOnly(int ndefType) throws RemoteException {
            return mDeviceHost.canMakeReadOnly(ndefType);
        }

        @Override
        public int getMaxTransceiveLength(int tech) throws RemoteException {
            return mDeviceHost.getMaxTransceiveLength(tech);
        }

        @Override
        public boolean getExtendedLengthApdusSupported() throws RemoteException {
            return mDeviceHost.getExtendedLengthApdusSupported();
        }
    }

    void _nfcEeClose(int callingPid, IBinder binder) throws IOException {
        // Blocks until a pending open() or transceive() times out.
        // TODO: This is incorrect behavior - the close should interrupt pending
        // operations. However this is not supported by current hardware.

        synchronized (NfcService.this) {
            if (!isNfcEnabledOrShuttingDown()) {
                throw new IOException("NFC adapter is disabled");
            }
            if (mOpenEe == null) {
                throw new IOException("NFC EE closed");
            }
            if (callingPid != -1 && callingPid != mOpenEe.pid) {
                throw new SecurityException("Wrong PID");
            }
            if (mOpenEe.binder != binder) {
                throw new SecurityException("Wrong binder handle");
            }

            binder.unlinkToDeath(mOpenEe, 0);
            // mDeviceHost.resetTimeouts();
            doDisconnect(mOpenEe.handle);
            mOpenEe = null;

            // applyRouting(true);
        }
    }

    final class NfcAdapterExtrasService extends INfcAdapterExtras.Stub {
        private Bundle writeNoException() {
            Bundle p = new Bundle();
            p.putInt("e", 0);
            return p;
        }

        private Bundle writeEeException(int exceptionType, String message) {
            Bundle p = new Bundle();
            p.putInt("e", exceptionType);
            p.putString("m", message);
            return p;
        }

        @Override
        public Bundle open(String pkg, IBinder b) throws RemoteException {
            if (DBG2) Log.d(TAG, "NfcAdapterExtrasService - open() ");

            NfcService.this.enforceNfceeAdminPerm(pkg);

            Bundle result;
            int handle = _open(b);
            if (handle < 0) {
                result = writeEeException(handle, "NFCEE open exception.");
            } else {
                result = writeNoException();
            }
            return result;
        }

        /**
         * Opens a connection to the secure element.
         *
         * @return A handle with a value >= 0 in case of success, or a negative value in case of
         *     failure.
         */
        private int _open(IBinder b) {
            if (DBG2) Log.d(TAG, "NfcAdapterExtrasService - _open() ");
            synchronized (NfcService.this) {
                if (!isNfcEnabled()) {
                    return EE_ERROR_NFC_DISABLED;
                }
                if (mInProvisionMode) {
                    // Deny access to the NFCEE as long as the device is being
                    // setup
                    return EE_ERROR_IO;
                }
                //                if (mP2pLinkManager.isLlcpActive()) {
                //                    // Don't allow PN544-based devices to open the SE while the
                //                    // LLCP
                //                    // link is still up or in a debounce state. This avoids race
                //                    // conditions in the NXP stack around P2P/SMX switching.
                //                    return EE_ERROR_EXT_FIELD;
                //                }
                if (mOpenEe != null) {
                    return EE_ERROR_ALREADY_OPEN;
                }

                int handle = doOpenSecureElementConnection();
                if (handle < 0) {

                    return handle;
                }
                // mDeviceHost.setTimeout(TagTechnology.ISO_DEP, 30000);

                mOpenEe = new OpenSecureElement(getCallingPid(), handle, b);
                try {
                    b.linkToDeath(mOpenEe, 0);
                } catch (RemoteException e) {
                    mOpenEe.binderDied();
                }

                // Add the calling package to the list of packages that have
                // accessed
                // the secure element.
                for (String packageName :
                        mContext.getPackageManager().getPackagesForUid(getCallingUid())) {
                    mSePackages.add(packageName);
                }

                return handle;
            }
        }

        @Override
        public Bundle close(String pkg, IBinder binder) throws RemoteException {
            NfcService.this.enforceNfceeAdminPerm(pkg);

            Bundle result;
            try {
                _nfcEeClose(getCallingPid(), binder);
                result = writeNoException();
            } catch (IOException e) {
                result = writeEeException(EE_ERROR_IO, e.getMessage());
            }
            return result;
        }

        @Override
        public Bundle transceive(String pkg, byte[] in) throws RemoteException {
            NfcService.this.enforceNfceeAdminPerm(pkg);

            Bundle result;
            byte[] out;
            try {
                out = _transceive(in);
                result = writeNoException();
                result.putByteArray("out", out);
            } catch (IOException e) {
                result = writeEeException(EE_ERROR_IO, e.getMessage());
            }
            return result;
        }

        private byte[] _transceive(byte[] data) throws IOException {
            synchronized (NfcService.this) {
                if (!isNfcEnabled()) {
                    throw new IOException("NFC is not enabled");
                }
                if (mOpenEe == null) {
                    throw new IOException("NFC EE is not open");
                }
                if (getCallingPid() != mOpenEe.pid) {
                    throw new SecurityException("Wrong PID");
                }
            }

            return doTransceive(mOpenEe.handle, data);
        }

        @Override
        public int getCardEmulationRoute(String pkg) throws RemoteException {
            NfcService.this.enforceNfceeAdminPerm(pkg);
            // return mEeRoutingState;
            return 1; // ??
        }

        @Override
        public void setCardEmulationRoute(String pkg, int route) throws RemoteException {
            NfcService.this.enforceNfceeAdminPerm(pkg);
        }

        @Override
        public void authenticate(String pkg, byte[] token) throws RemoteException {
            NfcService.this.enforceNfceeAdminPerm(pkg);
        }

        @Override
        public String getDriverName(String pkg) throws RemoteException {
            NfcService.this.enforceNfceeAdminPerm(pkg);
            return mDeviceHost.getName();
        }
    }

    /** resources kept while secure element is open */
    private class OpenSecureElement implements IBinder.DeathRecipient {
        public int pid; // pid that opened SE
        // binder handle used for DeathReceipient. Must keep
        // a reference to this, otherwise it can get GC'd and
        // the binder stub code might create a different BinderProxy
        // for the same remote IBinder, causing mismatched
        // link()/unlink()
        public IBinder binder;
        public int handle; // low-level handle

        public OpenSecureElement(int pid, int handle, IBinder binder) {
            this.pid = pid;
            this.handle = handle;
            this.binder = binder;
        }

        @Override
        public void binderDied() {
            synchronized (NfcService.this) {
                Log.i(TAG, "Tracked app " + pid + " died");
                pid = -1;
                try {
                    _nfcEeClose(-1, binder);
                } catch (IOException e) {
                    /* already closed */
                }
            }
        }

        @Override
        public String toString() {
            return new StringBuilder('@')
                    .append(Integer.toHexString(hashCode()))
                    .append("[pid=")
                    .append(pid)
                    .append(" handle=")
                    .append(handle)
                    .append("]")
                    .toString();
        }
    }

    final class NfcDtaService extends INfcDta.Stub {
        public void enableDta() throws RemoteException {
            NfcPermissions.enforceAdminPermissions(mContext);
            if (!sIsDtaMode) {
                mDeviceHost.enableDtaMode();
                sIsDtaMode = true;
                Log.d(TAG, "DTA Mode is Enabled ");
            }
        }

        public void disableDta() throws RemoteException {
            NfcPermissions.enforceAdminPermissions(mContext);
            if (sIsDtaMode) {
                mDeviceHost.disableDtaMode();
                sIsDtaMode = false;
            }
        }

        public boolean enableServer(
                String serviceName, int serviceSap, int miu, int rwSize, int testCaseId)
                throws RemoteException {
            NfcPermissions.enforceAdminPermissions(mContext);

            if (serviceName.equals(null) || !mIsBeamCapable) return false;

            mP2pLinkManager.enableExtDtaSnepServer(
                    serviceName, serviceSap, miu, rwSize, testCaseId);
            return true;
        }

        public void disableServer() throws RemoteException {
            if (!mIsBeamCapable) return;
            NfcPermissions.enforceAdminPermissions(mContext);
            mP2pLinkManager.disableExtDtaSnepServer();
        }

        public boolean enableClient(String serviceName, int miu, int rwSize, int testCaseId)
                throws RemoteException {
            NfcPermissions.enforceAdminPermissions(mContext);

            if (testCaseId == 0 || !mIsBeamCapable) return false;

            if (testCaseId > 20) {
                sIsShortRecordLayout = true;
                testCaseId = testCaseId - 20;
            } else {
                sIsShortRecordLayout = false;
            }
            Log.d("testCaseId", "" + testCaseId);
            mP2pLinkManager.enableDtaSnepClient(serviceName, miu, rwSize, testCaseId);
            return true;
        }

        public void disableClient() throws RemoteException {
            if (!mIsBeamCapable) return;
            NfcPermissions.enforceAdminPermissions(mContext);
            mP2pLinkManager.disableDtaSnepClient();
        }

        public boolean registerMessageService(String msgServiceName) throws RemoteException {
            NfcPermissions.enforceAdminPermissions(mContext);
            if (msgServiceName.equals(null)) return false;

            DtaServiceConnector.setMessageService(msgServiceName);
            return true;
        }
    };

    boolean isNfcEnabledOrShuttingDown() {
        synchronized (this) {
            return (mState == NfcAdapter.STATE_ON || mState == NfcAdapter.STATE_TURNING_OFF);
        }
    }

    boolean isNfcEnabled() {
        synchronized (this) {
            return mState == NfcAdapter.STATE_ON;
        }
    }

    class WatchDogThread extends Thread {
        final Object mCancelWaiter = new Object();
        final int mTimeout;
        boolean mCanceled = false;

        public WatchDogThread(String threadName, int timeout) {
            super(threadName);
            mTimeout = timeout;
        }

        @Override
        public void run() {
            try {
                synchronized (mCancelWaiter) {
                    mCancelWaiter.wait(mTimeout);
                    if (mCanceled) {
                        return;
                    }
                }
            } catch (InterruptedException e) {
                // Should not happen; fall-through to abort.
                Log.w(TAG, "Watchdog thread interruped.");
                interrupt();
            }
            if (mRoutingWakeLock.isHeld()) {
                Log.e(TAG, "Watchdog triggered, release lock before aborting.");
                mRoutingWakeLock.release();
            }
            Log.e(TAG, "Watchdog triggered, aborting.");
            StatsLog.write(
                    StatsLog.NFC_STATE_CHANGED, StatsLog.NFC_STATE_CHANGED__STATE__CRASH_RESTART);
            storeNativeCrashLogs();
            mDeviceHost.doAbort(getName());
        }

        public synchronized void cancel() {
            synchronized (mCancelWaiter) {
                mCanceled = true;
                mCancelWaiter.notify();
            }
        }
    }

    static byte[] hexStringToBytes(String s) {
        if (s == null || s.length() == 0) return null;
        int len = s.length();
        if (len % 2 != 0) {
            s = '0' + s;
            len++;
        }
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {
            data[i / 2] =
                    (byte)
                            ((Character.digit(s.charAt(i), 16) << 4)
                                    + Character.digit(s.charAt(i + 1), 16));
        }
        return data;
    }

    static String toHexString(byte[] buffer, int offset, int length) {
        final char[] HEX_CHARS = "0123456789abcdef".toCharArray();
        char[] chars = new char[2 * length];
        for (int j = offset; j < offset + length; ++j) {
            chars[2 * (j - offset)] = HEX_CHARS[(buffer[j] & 0xF0) >>> 4];
            chars[2 * (j - offset) + 1] = HEX_CHARS[buffer[j] & 0x0F];
        }
        return new String(chars);
    }

    /** Read mScreenState and apply NFC-C polling and NFC-EE routing */
    void applyRouting(boolean force) {
        synchronized (this) {
            if (!isNfcEnabled()) {
                return;
            }

            if (DBG2) Log.d(TAG, "applyRouting(" + force + ")");

            WatchDogThread watchDog = new WatchDogThread("applyRouting", ROUTING_WATCHDOG_MS);
            if (mInProvisionMode) {
                mInProvisionMode =
                        Settings.Global.getInt(
                                        mContentResolver, Settings.Global.DEVICE_PROVISIONED, 0)
                                == 0;
                if (!mInProvisionMode) {
                    // Notify dispatcher it's fine to dispatch to any package now
                    // and allow handover transfers.
                    mNfcDispatcher.disableProvisioningMode();
                }
            }
            // Special case: if we're transitioning to unlocked state while
            // still talking to a tag, postpone re-configuration.
            if (mScreenState == ScreenStateHelper.SCREEN_STATE_ON_UNLOCKED && isTagPresent()) {
                Log.d(TAG, "Not updating discovery parameters, tag connected.");
                mHandler.sendMessageDelayed(
                        mHandler.obtainMessage(MSG_RESUME_POLLING), APPLY_ROUTING_RETRY_TIMEOUT_MS);
                return;
            }

            try {
                watchDog.start();
                // Compute new polling parameters
                NfcDiscoveryParameters newParams = computeDiscoveryParameters(mScreenState);
                if (force || !newParams.equals(mCurrentDiscoveryParameters)) {
                    boolean shouldRestart = mCurrentDiscoveryParameters.shouldEnableDiscovery();
                    mDeviceHost.enableDiscovery(newParams, shouldRestart || force);
                    mCurrentDiscoveryParameters = newParams;
                } else {
                    Log.d(TAG, "Discovery configuration equal, not updating.");
                }
            } finally {
                watchDog.cancel();
            }

            if (DBG2) Log.d(TAG, "applyRouting() - exit");
        }
    }

    private NfcDiscoveryParameters computeDiscoveryParameters(int screenState) {
        if (DBG2) {
            Log.d(
                    TAG,
                    "computeDiscoveryParameters() screenState:"
                            + ScreenStateHelper.screenStateToString(screenState));
        }
        // Recompute discovery parameters based on screen state
        NfcDiscoveryParameters.Builder paramsBuilder = NfcDiscoveryParameters.newBuilder();
        // Polling
        modeBitmapSave = mStExtensions.getRfConfiguration(techArrayConfigSave);

        // By default, start full polling loop.
        // Adapt to current screen state/sub power state with CON_DISCOVERY_PARAM and
        // set_power_sub_state
        if ((modeBitmapSave & 0x04) == 0x4) { // P2P Listen
            if (DBG2) {
                Log.d(
                        TAG,
                        "computeDiscoveryParameters() - setTechMask "
                                + "techArrayConfigSave[3]: "
                                + Integer.toHexString(techArrayConfigSave[3])); // P2P
                // Poll
            }
            paramsBuilder.setTechMask(techArrayConfigSave[3]);
            paramsBuilder.setEnableP2p(mIsBeamCapable);
        }

        if ((modeBitmapSave & 0x01) == 0x1) { // Reader
            if (DBG2) {
                Log.d(
                        TAG,
                        "computeDiscoveryParameters() - setTechMask "
                                + "techArrayConfigSave[0]: "
                                + Integer.toHexString(techArrayConfigSave[0])); // Reader
            }
            paramsBuilder.setTechMask(techArrayConfigSave[0]);
        }

        // Check any special case where polling loop shall be adapted
        switch (screenState) {
            case ScreenStateHelper.SCREEN_STATE_ON_UNLOCKED:
                // Check if reader-mode is enabled
                if (mReaderModeParams != null) {
                    int techMask = 0;
                    if ((mReaderModeParams.flags & NfcAdapter.FLAG_READER_NFC_A) != 0)
                        techMask |= NFC_POLL_A;
                    if ((mReaderModeParams.flags & NfcAdapter.FLAG_READER_NFC_B) != 0)
                        techMask |= NFC_POLL_B;
                    if ((mReaderModeParams.flags & NfcAdapter.FLAG_READER_NFC_F) != 0)
                        techMask |= NFC_POLL_F;
                    if ((mReaderModeParams.flags & NfcAdapter.FLAG_READER_NFC_V) != 0)
                        techMask |= NFC_POLL_V;
                    if ((mReaderModeParams.flags & NfcAdapter.FLAG_READER_NFC_BARCODE) != 0)
                        techMask |= NFC_POLL_KOVIO;

                    if (DBG2) {
                        Log.d(TAG, "mReaderModeParams != null setTechMask:" + techMask);
                    }
                    paramsBuilder.setTechMask(techMask);
                    paramsBuilder.setEnableReaderMode(true);
                    paramsBuilder.setEnableP2p(false);
                }
                break;

            case ScreenStateHelper.SCREEN_STATE_ON_LOCKED:

                // No CE screeen OFF, regular cases
                boolean isLockscreenPollingEnabled = mNfcUnlockManager.isLockscreenPollingEnabled();

                if (mInProvisionMode) {
                    if (DBG2)
                        Log.d(
                                TAG,
                                "computeDiscoveryParameters() - setTechMask NfcDiscoveryParameters"
                                        + ".NFC_POLL_DEFAULT");
                    if ((modeBitmapSave & 0x04) == 0x4) { // P2P Listen
                        if (DBG2) {
                            Log.d(
                                    TAG,
                                    "computeDiscoveryParameters() - setTechMask "
                                            + "techArrayConfigSave[3]: "
                                            + Integer.toHexString(techArrayConfigSave[3])); // P2P
                            // Poll
                        }
                        paramsBuilder.setTechMask(techArrayConfigSave[3]);
                        paramsBuilder.setEnableP2p(mIsBeamCapable);
                    }
                } else if (isLockscreenPollingEnabled) {
                    int techMask = mNfcUnlockManager.getLockscreenPollMask();
                    if (DBG2) {
                        Log.d(
                                TAG,
                                "computeDiscoveryParameters() - mNfcUnlockManager"
                                        + ".getLockscreenPollMask(techMask):"
                                        + mNfcUnlockManager.getLockscreenPollMask());
                    }
                    if ((modeBitmapSave & 0x01) == 0x1) { // Reader
                        if (DBG2) {
                            Log.d(
                                    TAG,
                                    "computeDiscoveryParameters() - setTechMask : "
                                            + Integer.toHexString(techMask)); // Reader
                        }
                        paramsBuilder.setTechMask(techMask);
                    }
                    // paramsBuilder.setEnableP2p(false);
                }
                break;

            case ScreenStateHelper.SCREEN_STATE_OFF_LOCKED:
            case ScreenStateHelper.SCREEN_STATE_OFF_UNLOCKED:
            case ScreenStateHelper.SCREEN_STATE_UNKNOWN:
            default:
                break;
        }

        if (mIsHceCapable && mReaderModeParams == null) {
            // Host routing is always enabled at lock screen or later, provided
            // we aren't in reader mode
            if ((modeBitmapSave & 0x02) == 0x2) { // HCE
                paramsBuilder.setEnableHostRouting(true);
            }
        }

        return paramsBuilder.build();
    }

    private boolean isTagPresent() {
        for (Object object : mObjectMap.values()) {
            if (object instanceof TagEndpoint) {
                return ((TagEndpoint) object).isPresent();
            }
        }
        return false;
    }

    private void StopPresenceChecking() {
        Object[] objectValues = mObjectMap.values().toArray();
        for (Object object : objectValues) {
            if (object instanceof TagEndpoint) {
                TagEndpoint tag = (TagEndpoint) object;
                ((TagEndpoint) object).stopPresenceChecking();
            }
        }
    }

    /** Disconnect any target if present */
    void maybeDisconnectTarget() {
        if (!isNfcEnabledOrShuttingDown()) {
            return;
        }
        Object[] objectsToDisconnect;
        synchronized (this) {
            Object[] objectValues = mObjectMap.values().toArray();
            // Copy the array before we clear mObjectMap,
            // just in case the HashMap values are backed by the same array
            objectsToDisconnect = Arrays.copyOf(objectValues, objectValues.length);
            mObjectMap.clear();
        }
        for (Object o : objectsToDisconnect) {
            if (DBG) Log.d(TAG, "disconnecting " + o.getClass().getName());
            if (o instanceof TagEndpoint) {
                // Disconnect from tags
                TagEndpoint tag = (TagEndpoint) o;
                tag.disconnect();
            } else if (o instanceof NfcDepEndpoint) {
                // Disconnect from P2P devices
                NfcDepEndpoint device = (NfcDepEndpoint) o;
                if (device.getMode() == NfcDepEndpoint.MODE_P2P_TARGET) {
                    // Remote peer is target, request disconnection
                    device.disconnect();
                } else {
                    // Remote peer is initiator, we cannot disconnect
                    // Just wait for field removal
                }
            }
        }
    }

    Object findObject(int key) {
        synchronized (this) {
            Object device = mObjectMap.get(key);
            if (device == null) {
                Log.w(TAG, "Handle not found");
            }
            return device;
        }
    }

    Object findAndRemoveObject(int handle) {
        synchronized (this) {
            Object device = mObjectMap.get(handle);
            if (device == null) {
                Log.w(TAG, "Handle not found");
            } else {
                mObjectMap.remove(handle);
            }
            return device;
        }
    }

    void registerTagObject(TagEndpoint tag) {
        synchronized (this) {
            mObjectMap.put(tag.getHandle(), tag);
        }
    }

    void unregisterObject(int handle) {
        synchronized (this) {
            mObjectMap.remove(handle);
        }
    }

    /** For use by code in this process */
    public LlcpSocket createLlcpSocket(int sap, int miu, int rw, int linearBufferLength)
            throws LlcpException {
        return mDeviceHost.createLlcpSocket(sap, miu, rw, linearBufferLength);
    }

    /** For use by code in this process */
    public LlcpConnectionlessSocket createLlcpConnectionLessSocket(int sap, String sn)
            throws LlcpException {
        return mDeviceHost.createLlcpConnectionlessSocket(sap, sn);
    }

    /** For use by code in this process */
    public LlcpServerSocket createLlcpServerSocket(
            int sap, String sn, int miu, int rw, int linearBufferLength) throws LlcpException {
        return mDeviceHost.createLlcpServerSocket(sap, sn, miu, rw, linearBufferLength);
    }

    public int getAidRoutingTableSize() {
        int aidTableSize = 0x00;
        aidTableSize = mDeviceHost.getAidTableSize();

        if (DBG2) Log.d(TAG, "getAidRoutingTableSize() - aidTableSize: " + aidTableSize);

        return aidTableSize;
    }

    public void sendMockNdefTag(NdefMessage msg) {
        sendMessage(MSG_MOCK_NDEF, msg);
    }

    int mResolvedDefaultAidRoute = 0;

    public void routeAids(String aid, int route, int aidInfo) {

        if (DBG2)
            Log.d(TAG, "routeAids() - AID : " + aid + ", route: " + String.format("0x%02X", route));

        Message msg = mHandler.obtainMessage();
        msg.what = MSG_ROUTE_AID;
        msg.arg1 = route;
        msg.obj = aid;
        msg.arg2 = aidInfo;
        mHandler.sendMessage(msg);

        // Get resolved default AID route
        if (aid.contentEquals("")) {
            if (DBG2)
                Log.d(TAG, "routeAids() - Default AID route: " + String.format("0x%02X", route));
            mResolvedDefaultAidRoute = route;
        }
    }

    public void unrouteAids(String aid) {
        sendMessage(MSG_UNROUTE_AID, aid);
    }

    public int getNciVersion() {
        return mDeviceHost.getNciVersion();
    }

    private byte[] getT3tIdentifierBytes(String systemCode, String nfcId2, String t3tPmm) {
        ByteBuffer buffer = ByteBuffer.allocate(2 + 8 + 8); /* systemcode + nfcid2 + t3tpmm */
        buffer.put(hexStringToBytes(systemCode));
        buffer.put(hexStringToBytes(nfcId2));
        buffer.put(hexStringToBytes(t3tPmm));
        byte[] t3tIdBytes = new byte[buffer.position()];
        buffer.position(0);
        buffer.get(t3tIdBytes);

        return t3tIdBytes;
    }

    public void registerT3tIdentifier(String systemCode, String nfcId2, String t3tPmm) {
        Log.d(TAG, "request to register LF_T3T_IDENTIFIER");
        modeBitmapSave = mStExtensions.getRfConfiguration(techArrayConfigSave);
        if ((modeBitmapSave & 0x02) == 0x2) { // HCE
            mHceF_enabled = true;
            byte[] t3tIdentifier = getT3tIdentifierBytes(systemCode, nfcId2, t3tPmm);
            sendMessage(MSG_REGISTER_T3T_IDENTIFIER, t3tIdentifier);
        } else {
            mHceF_enabled = false;
        }
    }

    public void deregisterT3tIdentifier(String systemCode, String nfcId2, String t3tPmm) {
        Log.d(TAG, "request to deregister LF_T3T_IDENTIFIER");
        if (mHceF_enabled) {
            byte[] t3tIdentifier = getT3tIdentifierBytes(systemCode, nfcId2, t3tPmm);
            sendMessage(MSG_DEREGISTER_T3T_IDENTIFIER, t3tIdentifier);
        }
    }

    public void clearT3tIdentifiersCache() {
        Log.d(TAG, "clear T3t Identifiers Cache");
        mDeviceHost.clearT3tIdentifiersCache();
    }

    public int getLfT3tMax() {
        return mDeviceHost.getLfT3tMax();
    }

    public void commitRouting() {
        mHandler.sendEmptyMessage(MSG_COMMIT_ROUTING);
    }

    public void startPollingLoop() {
        mHandler.sendEmptyMessage(MSG_START_POLLING);
    }

    /**
     * Set the ApduServiceInfo description of the last modified service that caused an update of the
     * routing table
     */
    public void setLastModifiedService(String description) {
        if (DBG2) Log.d(TAG, "setLastModifiedService() -  description: " + description);

        mPrefsEditor = mPrefs.edit();
        mPrefsEditor.putString(PREF_LAST_MODIFIED_SERVICE, description);
        mPrefsEditor.commit();
    }

    /**
     * Get the ApduServiceInfo description of the last modified service that caused an update of the
     * routing table
     */
    public String getLastModifiedService() {
        String description = mPrefs.getString(PREF_LAST_MODIFIED_SERVICE, "");
        if (DBG2) Log.d(TAG, "getLastModifiedService() -  description: " + description);
        return description;
    }

    /** Resets the PREF var, changed by previous route overflow switch */
    public void resetOverflowSwitchInformation() {

        if (DBG2) Log.d(TAG, "resetOverflowSwitchInformation() ");

        // Reset routing table full flag too.
        // If routing table is still full, this will be detected when computing
        // new routing table
        mIsRoutingTableFull = false;

        mNotificationManager.cancel(OVERFLOW_UNIQUE_NOTIF_ID);
    }

    /**
     * This is executed when user press the Cancel button of the notification in case of overflow.
     */
    public static class NotificationBroadcastReceiver extends BroadcastReceiver {
        private CardEmulationManager mCardEmulationManager;
        private Context mContext;

        public NotificationBroadcastReceiver() {}

        private static NotificationBroadcastReceiver mSingleton;

        public static void createSingleton(Context context, CardEmulationManager cem) {
            Log.d(
                    TAG,
                    " createSingleton() - attach the CardEmulationManager to the existing instance");
            NotificationBroadcastReceiver instance = getInstance();
            instance.mCardEmulationManager = cem;
            instance.mContext = context;
        }

        public static NotificationBroadcastReceiver getInstance() {
            if (mSingleton == null) {
                mSingleton = new NotificationBroadcastReceiver();
            }
            return mSingleton;
        }

        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (DBG2) Log.d(TAG, "Broadcast from Notification: " + action);
            ((NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE))
                    .cancel(OVERFLOW_UNIQUE_NOTIF_ID);
            // in case there is a pendingOverflowResolution, inform that user
            // closed the notif.
            if (getInstance().mCardEmulationManager != null) {
                getInstance()
                        .mCardEmulationManager
                        .overflowNotificationClosed(OVERFLOW_NTF_ACTION_HIDE.equals(action));
            }
        }
    }

    boolean mIsRoutingTableFull = false;
    boolean mIsForegroundAllowed = false;
    int mForegroundHostId = 0x00;
    boolean mIsForcedRouting = false;

    /**
     * AID routing table is full for all routes either start forced rouitng mode is this is due to
     * foregournd app or inform user, APDU service sin categroy "other" need to be disabled
     */
    public void notifyRoutingTableFull() {
        if (DBG2) Log.d(TAG, "notifyRoutingTableFull() - !!!!overflow occured!!!!");

        // mPrefsEditor = mPrefs.edit();
        // mPrefsEditor.putBoolean(PREF_AID_ROUTING_TABLE_FULL, true);
        // mPrefsEditor.commit();
        mIsRoutingTableFull = true;

        // Check if overflow was caused by foreground app
        // boolean foregroundStatus = mPrefs.getBoolean(PREF_FOREGROUND_ALLOWED,
        // false);
        if (mIsForegroundAllowed == true) {

            if (DBG2)
                Log.d(
                        TAG,
                        "notifyRoutingTableFull() -  Overflow due to foreground app, start forced"
                                + " routing");

            // enter forced routing mode for the time app is in foreground
            // int hostId = 0x00;
            // int powerState = 0x09;
            // boolean onHost = mPrefs.getBoolean(PREF_FOREGROUND_HOST, false);
            // if (onHost == false) {
            // hostId = getActiveSeValue(); //code needs
            // // updates for eSE/UICC2 etc...
            // powerState = 0x3b;
            // }

            // mPrefsEditor.putBoolean(PREF_FORCED_ROUTING_ON, true);
            // mPrefsEditor.commit();

            mIsForcedRouting = true;
            startForcedRouting(mForegroundHostId);
        } else {

            if (DBG2)
                Log.d(
                        TAG,
                        "notifyRoutingTableFull() -  Overflow due to service modification, warn "
                                + "user");

            String description = mPrefs.getString(PREF_LAST_MODIFIED_SERVICE, "");
            String msg;

            // build Intent to start RoutingTableAlert activity
            /*
             * Intent routingAlertIntent = new Intent(mContext,
             * RoutingTableAlert.class);
             * routingAlertIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK |
             * Intent .FLAG_ACTIVITY_CLEAR_TASK);
             * routingAlertIntent.putExtra("description", description);
             * mContext.startActivity(routingAlertIntent);
             */

            if (description != null && !description.equals("")) {
                msg = String.format(mContext.getString(R.string.aid_routing_overflow), description);
            } else {
                msg = mContext.getString(R.string.aid_routing_overflow_default);
            }

            if (mNotificationBroadcastReceiver == null) {
                NotificationBroadcastReceiver.createSingleton(mContext, mCardEmulationManager);
                mNotificationBroadcastReceiver = NotificationBroadcastReceiver.getInstance();
            }

            Intent intentCancel =
                    new Intent(
                            OVERFLOW_NTF_ACTION_HIDE,
                            null,
                            mContext,
                            NotificationBroadcastReceiver.class);
            PendingIntent pendingIntentCancel =
                    PendingIntent.getBroadcast(mContext, 0, intentCancel, 0);
            Notification.Action actCancel =
                    new Notification.Action.Builder(
                                    0,
                                    mContext.getText(R.string.aid_routing_overflow_cancel),
                                    pendingIntentCancel)
                            .build();

            Intent intentChange;
            // MTK - start
            if (HAS_MTK_SETTINGS) {
                intentChange = new Intent(MTK_OVERFLOW_SETTINGS_MENU_INTENT);
            } else {
                intentChange = new Intent(OVERFLOW_SETTINGS_MENU_INTENT);
            }
            // MTK - end
            intentChange.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK);
            PendingIntent pendingIntentChange =
                    PendingIntent.getActivity(mContext, 0, intentChange, 0);
            Notification.Action actChange =
                    new Notification.Action.Builder(
                                    0, // R.drawable.ic_toolbar_swap,
                                    mContext.getText(R.string.aid_routing_overflow_change),
                                    pendingIntentChange)
                            .build();

            Notification n =
                    new Notification.Builder(mContext, OVERFLOW_NOTIFICATION_CHANNEL)
                            .setContentTitle(mContext.getString(R.string.app_name))
                            .setContentText(msg)
                            .setSmallIcon(R.drawable.notif_nfc)
                            .setContentIntent(pendingIntentChange)
                            .addAction(actCancel)
                            .addAction(actChange)
                            .build();
            mNotificationManager.notify(OVERFLOW_UNIQUE_NOTIF_ID, n);
        }
    }

    /** get info to know if AIR routing table is full */
    public boolean getAidRoutingTableStatus() {
        // boolean aidTableStatus;
        // aidTableStatus = mPrefs.getBoolean(PREF_AID_ROUTING_TABLE_FULL,
        // false);

        if (DBG2)
            Log.d(TAG, "getAidRoutingTableStatus() -  mIsRoutingTableFull: " + mIsRoutingTableFull);

        return mIsRoutingTableFull;
    }

    /**
     * Reset flag indicating AID routin gtable is full This is done when the service status is
     * updated
     */
    public void resetAidRoutingTableFull() {

        if (DBG2) Log.d(TAG, "resetAidRoutingTableFull()");

        // mPrefsEditor = mPrefs.edit();
        // mPrefsEditor.putBoolean(PREF_AID_ROUTING_TABLE_FULL, false);
        // mPrefsEditor.commit();
        mIsRoutingTableFull = false;
        mNotificationManager.cancel(OVERFLOW_UNIQUE_NOTIF_ID);
    }

    /** Empty AID routing table before writting new one */
    public void clearRouting() {
        mHandler.removeMessages(MSG_COMMIT_ROUTING);
        mHandler.removeMessages(MSG_ROUTE_AID);
        mHandler.removeMessages(MSG_CLEAR_ROUTING);
        mHandler.removeMessages(MSG_UNROUTE_AID);
        mHandler.sendEmptyMessage(MSG_CLEAR_ROUTING);
    }

    /**
     * Start forced routing mode: This happens when a foreground app has caused the routing table to
     * be full
     */
    public void startForcedRouting(int nfceeId) {

        if (DBG2) Log.d(TAG, "startForcedRouting() -  nfceeId: " + nfceeId);
        mDeviceHost.forceRouting(nfceeId);
    }

    /**
     * Stop forced routing mode: This happens when the foreground app that has caused the routing
     * table to be full is sent to background/closed
     */
    public void stopForcedRouting() {

        if (DBG2) Log.d(TAG, "stopForcedRouting()");
        mDeviceHost.stopforceRouting();
    }

    /**
     * udpates information about foreground app: set/reset flag for foregournd app indicates if
     * foreground app is on host or off host
     */
    public void setForegroundAllowed(boolean status, String hostId) {
        if (DBG2) Log.d(TAG, "setForegroundAllowed() -  status: " + status + ", hostId: " + hostId);

        mIsForegroundAllowed = status;

        if (hostId.contains("SIM")) {
            mForegroundHostId = 0x81;
        } else if (hostId.contains("eSE")) {
            mForegroundHostId = 0x82;
        } else {
            mForegroundHostId = 0x00;
        }

        // if forced routing was enabled, disable it when foreground is off
        if (status == false) {
            // boolean isForced = mPrefs.getBoolean(PREF_FORCED_ROUTING_ON,
            // false);
            if (mIsForcedRouting == true) {

                if (DBG2) Log.d(TAG, "setForegroundAllowed() -  Stop forced routing");

                stopForcedRouting();
                mIsForcedRouting = false;
                // mPrefsEditor.putBoolean(PREF_FORCED_ROUTING_ON, false);
                // mPrefsEditor.commit();
            }
        }
    }

    HashMap<Integer, Integer> mOverflowRouteSizes = new HashMap<Integer, Integer>();

    public void setRoutingTableSizeFull(int route, int size) {
        if (DBG2) Log.d(TAG, "setRoutingTableSizeFull() -  route: " + route + ", size: " + size);

        mOverflowRouteSizes.put(route, size);
        // mPrefsEditor = mPrefs.edit();
        // String pref = "";
        //
        // if (route == 0x00) {
        // pref = PREF_DH_FULL_ROUTING_TABLE_SIZE;
        // } else {
        // pref = PREF_UICC_FULL_ROUTING_TABLE_SIZE;
        // }
        // mPrefsEditor.putInt(pref, size);
        // mPrefsEditor.commit();
    }

    int mRoutingTableSizeNotFull = 0;
    int mAltRoutingTableSizeNotFull = 0;

    public void setRoutingTableSizeNotFull(int size) {
        if (DBG2) Log.d(TAG, "setRoutingTableSizeNotFull() - size: " + size);

        mRoutingTableSizeNotFull = size;

        // mPrefsEditor = mPrefs.edit();
        // mPrefsEditor.putInt(PREF_NOT_FULL_ROUTING_TABLE_SIZE, size);
        // mPrefsEditor.commit();
    }

    public void setRoutingTableSizeNotFullAlt(int size) {
        if (DBG2) Log.d(TAG, "setRoutingTableSizeNotFullAlt() - size: " + size);

        mAltRoutingTableSizeNotFull = size;
        // mPrefsEditor = mPrefs.edit();
        // mPrefsEditor.putInt(PREF_NOT_FULL_ALT_ROUTING_TABLE_SIZE, size);
        // mPrefsEditor.commit();
    }

    // This fonction is called only when a overflow of both AID routes occurs
    // Disabling an app on UICC will make room for the HCE route
    // DIsabling an app on HCE will make room for the UICC route
    public int getRoutingTableSizeForRoute(int route) {
        // if (DBG2) Log.d(TAG, "getRoutingTableSizeForRoute() -  route: " +
        // route);

        // String pref = "";
        // int size = 0;
        //
        // if (route == 0x00) {
        // pref = PREF_DH_FULL_ROUTING_TABLE_SIZE;
        // } else {
        // pref = PREF_UICC_FULL_ROUTING_TABLE_SIZE;
        // }
        //
        // size = mPrefs.getInt(pref, 0);

        int size = mOverflowRouteSizes.get(route);

        if (DBG2)
            Log.d(TAG, "getRoutingTableSizeForRoute() -  route: " + route + ", size: " + size);

        return size;
    }

    public int getRoutingTableSizeNotFull() {
        // if (DBG2) Log.d(TAG, "getRoutingTableSizeNotFull()");

        // String pref = "";
        // int size = 0;
        //
        // size = mPrefs.getInt(PREF_NOT_FULL_ROUTING_TABLE_SIZE, 0);

        if (DBG2)
            Log.d(
                    TAG,
                    "getRoutingTableSizeNotFull() -  mRoutingTableSizeNotFull: "
                            + mRoutingTableSizeNotFull);

        return mRoutingTableSizeNotFull;
    }

    public int getRoutingTableSizeNotFullAlt() {
        // if (DBG2) Log.d(TAG, "getRoutingTableSizeNotFullAlt()");

        // String pref = "";
        // int size = 0;
        //
        // size = mPrefs.getInt(PREF_NOT_FULL_ALT_ROUTING_TABLE_SIZE, 0);

        if (DBG2)
            Log.d(
                    TAG,
                    "getRoutingTableSizeNotFullAlt() -  mAltRoutingTableSizeNotFull: "
                            + mAltRoutingTableSizeNotFull);

        return mAltRoutingTableSizeNotFull;
    }

    private int convertPrefRouteToNfceeIdType(String route) {

        String prefRoute = mPrefs.getString(route, NfcSettingsAdapter.DEFAULT_ROUTE);

        int nciId = 0xFF;
        //        byte[] nfceeid = new byte[3];
        //        byte[] conInfo = new byte[3];
        // int num = mStExtensions.getAvailableHciHostList(nfceeid, conInfo);

        if (prefRoute.equals("UICC")) {
            //            for (int i = 0; i < num; i++) {
            //                //If NFCEE is connected
            //                if (conInfo[i] == 0x00) {
            //                    // Potential NFCEE Ids for UICC are
            //                    // 0x81/0x83/0x85
            //                    if ((nfceeid[i] & 0x01) != 0) {
            //                        nciId = nfceeid[i];
            //                    }
            //                }
            //            }
            nciId = 0x81;
        } else if (prefRoute.equals("eSE")) {
            //            for (int i = 0; i < num; i++) {
            //                //If NFCEE is connected
            //                if (conInfo[i] == 0x00) {
            //                    // Potential NFCEE Ids for eSE are
            //                    // 0x82/0x84/0x86
            //                    if ((nfceeid[i] & 0x01) == 0) {
            //                        nciId = nfceeid[i];
            //                    }
            //                }
            //            }
            nciId = 0x82;
        } else if (prefRoute.equals("HCE")) {
            nciId = 0x00;
        }

        if (DBG2)
            Log.d(
                    TAG,
                    "convertPrefRouteToNfceeIdType() - route Id: "
                            + route
                            + ", prefRoute: "
                            + prefRoute
                            + ", route: "
                            + String.format("0x%02X", nciId));

        return (nciId & 0xFF);
    }

    public int getDefaultAidRoute() {
        if (DBG2) Log.d(TAG, "getDefaultAidRoute()");

        return convertPrefRouteToNfceeIdType(PREF_DEFAULT_AID_ROUTE);
    }

    public int getDefaultIsoDepRoute() {
        if (DBG2) Log.d(TAG, "getDefaultIsoDepRoute()");

        return convertPrefRouteToNfceeIdType(PREF_DEFAULT_ISODEP_ROUTE);
    }

    public int getConnectedNfceeId(int id) {

        // If DH or unvalid value, use HCE
        int nciId = 0x00;
        int i;
        byte[] nfceeid = new byte[3];
        byte[] conInfo = new byte[3];
        int num = mStExtensions.getAvailableHciHostList(nfceeid, conInfo);

        switch (id) {
            case 0x81:
            case 0x83:
            case 0x85:
                for (i = 0; i < num; i++) {
                    if ((conInfo[i] == 0) && ((nfceeid[i] & 0x01) != 0)) {
                        nciId = nfceeid[i];
                        break;
                    }
                }
                break;

            case 0x82:
            case 0x84:
            case 0x86:
                for (i = 0; i < num; i++) {
                    if ((conInfo[i] == 0) && ((nfceeid[i] & 0x01) == 0)) {
                        nciId = nfceeid[i];
                        break;
                    }
                }
                break;

            default:
                break;
        }

        if (DBG2)
            Log.d(
                    TAG,
                    "getConnectedNfceeId() -  requested id: "
                            + String.format("0x%02X", id)
                            + ", corresponding connected nfceeId: "
                            + String.format("0x%02X", nciId & 0xFF));

        return (nciId & 0xFF);
    }

    public void updateRoutingTable() {
        Log.i(TAG, "updateRoutingTable() ");

        mAidRoutingManager.onNfccRoutingTableCleared();
        mCardEmulationManager.onRoutingTableChanged();

        // If there was no AIDs to route, force routing
        if (mHandler.hasMessages(MSG_COMMIT_ROUTING) == false) {
            Log.i(TAG, "updateRoutingTable() - No AID to route, force RT update");
            mHandler.sendEmptyMessage(MSG_COMMIT_ROUTING);
        }
    }

    public boolean sendData(byte[] data) {
        return mDeviceHost.sendRawFrame(data);
    }

    void sendMessage(int what, Object obj) {
        Message msg = mHandler.obtainMessage();
        msg.what = what;
        msg.obj = obj;
        mHandler.sendMessage(msg);
    }

    void sendMessageAtFront(int what, Object obj) {
        Message msg = mHandler.obtainMessage();
        msg.what = what;
        msg.obj = obj;
        mHandler.sendMessageAtFrontOfQueue(msg);
    }

    final class NfcServiceHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_ROUTE_AID:
                    {
                        if (DBG2) Log.d(TAG, "NfcServiceHandler - handleMessage(MSG_ROUTE_AID)");
                        int route = msg.arg1;
                        int aidInfo = msg.arg2;
                        String aid = (String) msg.obj;
                        mDeviceHost.routeAid(hexStringToBytes(aid), route, aidInfo);
                        // Restart polling config
                        break;
                    }
                case MSG_UNROUTE_AID:
                    {
                        if (DBG2) Log.d(TAG, "NfcServiceHandler - handleMessage(MSG_UNROUTE_AID)");
                        String aid = (String) msg.obj;
                        mDeviceHost.unrouteAid(hexStringToBytes(aid));
                        break;
                    }
                case MSG_REGISTER_T3T_IDENTIFIER:
                    {
                        Log.d(
                                TAG,
                                "NfcServiceHandler - handleMessage(MSG_REGISTER_T3T_IDENTIFIER)");
                        mDeviceHost.disableDiscovery();

                        byte[] t3tIdentifier = (byte[]) msg.obj;
                        mDeviceHost.registerT3tIdentifier(t3tIdentifier);

                        synchronized (NfcService.this) {
                            NfcDiscoveryParameters params =
                                    computeDiscoveryParameters(mScreenState);
                            boolean shouldRestart =
                                    mCurrentDiscoveryParameters.shouldEnableDiscovery();
                            mDeviceHost.enableDiscovery(params, shouldRestart);
                        }
                        break;
                    }
                case MSG_DEREGISTER_T3T_IDENTIFIER:
                    {
                        Log.d(
                                TAG,
                                "NfcServiceHandler - handleMessage(MSG_DEREGISTER_T3T_IDENTIFIER)");
                        mDeviceHost.disableDiscovery();

                        byte[] t3tIdentifier = (byte[]) msg.obj;
                        mDeviceHost.deregisterT3tIdentifier(t3tIdentifier);

                        synchronized (NfcService.this) {
                            NfcDiscoveryParameters params =
                                    computeDiscoveryParameters(mScreenState);
                            boolean shouldRestart =
                                    mCurrentDiscoveryParameters.shouldEnableDiscovery();
                            mDeviceHost.enableDiscovery(params, shouldRestart);
                        }
                        break;
                    }
                case MSG_INVOKE_BEAM:
                    {
                        mP2pLinkManager.onManualBeamInvoke((BeamShareData) msg.obj);
                        break;
                    }
                case MSG_COMMIT_ROUTING:
                    {
                        if (DBG2)
                            Log.d(TAG, "NfcServiceHandler - handleMessage(MSG_COMMIT_ROUTING)");
                        // boolean commit = true;
                        // synchronized (NfcService.this) {
                        //     if (mCurrentDiscoveryParameters.shouldEnableDiscovery()) {
                        //         commit = true;
                        //     } else {
                        //         Log.d(TAG, "Not committing routing because discovery is
                        // disabled.");
                        //     }
                        // }
                        // if (commit) {

                        // Wallet. This is called after changes to active eSE /
                        // SIM.
                        //                        applyWalletConfigIfNeeded(false);
                        // loadWalletConfigFromPref();

                        if (1
                                == Settings.Global.getInt(
                                        mContext.getContentResolver(), "nfc_rf_field_active", -1)) {
                            // we're in the field, do this later, otherwise
                            // we'll
                            // cut the transaction in the middle'
                            mPendingRoutingTableUpdate = true;
                            break;
                        }
                        mPendingRoutingTableUpdate = false;

                        mDeviceHost.commitRouting();
                        // }
                        break;
                    }

                case MSG_UPDATE_ROUTING_TABLE:
                    if (DBG2)
                        Log.d(TAG, "NfcServiceHandler - handleMessage(MSG_UPDATE_ROUTING_TABLE)");
                    updateRoutingTable();
                    break;

                case MSG_CLEAR_ROUTING:
                    {
                        if (DBG2)
                            Log.d(TAG, "NfcServiceHandler - handleMessage(MSG_CLEAR_ROUTING)");
                        mDeviceHost.clearAidTable();
                        break;
                    }

                case MSG_MOCK_NDEF:
                    {
                        NdefMessage ndefMsg = (NdefMessage) msg.obj;
                        Bundle extras = new Bundle();
                        extras.putParcelable(Ndef.EXTRA_NDEF_MSG, ndefMsg);
                        extras.putInt(Ndef.EXTRA_NDEF_MAXLENGTH, 0);
                        extras.putInt(Ndef.EXTRA_NDEF_CARDSTATE, Ndef.NDEF_MODE_READ_ONLY);
                        extras.putInt(Ndef.EXTRA_NDEF_TYPE, Ndef.TYPE_OTHER);
                        Tag tag =
                                Tag.createMockTag(
                                        new byte[] {0x00},
                                        new int[] {TagTechnology.NDEF},
                                        new Bundle[] {extras});
                        Log.d(TAG, "mock NDEF tag, starting corresponding activity");
                        Log.d(TAG, tag.toString());
                        int dispatchStatus = mNfcDispatcher.dispatchTag(tag);
                        if (dispatchStatus == NfcDispatcher.DISPATCH_SUCCESS) {
                            playSound(SOUND_END);
                        } else if (dispatchStatus == NfcDispatcher.DISPATCH_FAIL) {
                            playSound(SOUND_ERROR);
                        }
                        break;
                    }

                case MSG_NDEF_TAG:
                    if (DBG) Log.d(TAG, "Tag detected, notifying applications");
                    if (mScreenState == ScreenStateHelper.SCREEN_STATE_ON_UNLOCKED) {
                        mPowerManager.userActivity(
                                SystemClock.uptimeMillis(),
                                PowerManager.USER_ACTIVITY_EVENT_OTHER,
                                0);
                    }
                    mNumTagsDetected.incrementAndGet();
                    TagEndpoint tag = (TagEndpoint) msg.obj;
                    byte[] debounceTagUid;
                    int debounceTagMs;
                    ITagRemovedCallback debounceTagRemovedCallback;
                    synchronized (NfcService.this) {
                        debounceTagUid = mDebounceTagUid;
                        debounceTagMs = mDebounceTagDebounceMs;
                        debounceTagRemovedCallback = mDebounceTagRemovedCallback;
                    }
                    ReaderModeParams readerParams = null;
                    int presenceCheckDelay = DEFAULT_PRESENCE_CHECK_DELAY;
                    DeviceHost.TagDisconnectedCallback callback =
                            new DeviceHost.TagDisconnectedCallback() {
                                @Override
                                public void onTagDisconnected(long handle) {
                                    // applyRouting(false);
                                    if (mPendingPowerStateUpdate == true) {
                                        int screenState = mScreenStateHelper.checkScreenState();
                                        Log.d(
                                                TAG,
                                                "onTagDisconnected - applying postponed screen state "
                                                        + screenState);
                                        NfcService.getInstance()
                                                .sendMessage(
                                                        NfcService.MSG_APPLY_SCREEN_STATE,
                                                        screenState);
                                    }
                                }
                            };
                    synchronized (NfcService.this) {
                        readerParams = mReaderModeParams;
                    }
                    if (readerParams != null) {
                        presenceCheckDelay = readerParams.presenceCheckDelay;
                        if ((readerParams.flags & NfcAdapter.FLAG_READER_SKIP_NDEF_CHECK) != 0) {
                            if (DBG) Log.d(TAG, "Skipping NDEF detection in reader mode");
                            tag.startPresenceChecking(presenceCheckDelay, callback);
                            dispatchTagEndpoint(tag, readerParams);
                            break;
                        }
                    }

                    if (NfcAddonWrapper.getInstance()
                                    .getModeFlag(NfcSettingsAdapter.MODE_READER, NfcService.this)
                            == 0) {
                        if (DBG)
                            Log.d(
                                    TAG,
                                    "Skipping tag reading and dispatch because reader mode is OFF");
                        tag.startPresenceChecking(presenceCheckDelay, callback);
                        break;
                    }

                    if (tag.getConnectedTechnology() == TagTechnology.NFC_BARCODE) {
                        // When these tags start containing NDEF, they will require
                        // the stack to deal with them in a different way, since
                        // they are activated only really shortly.
                        // For now, don't consider NDEF on these.
                        if (DBG) Log.d(TAG, "Skipping NDEF detection for NFC Barcode");
                        tag.startPresenceChecking(presenceCheckDelay, callback);
                        dispatchTagEndpoint(tag, readerParams);
                        break;
                    }
                    NdefMessage ndefMsg = tag.findAndReadNdef();

                    if (ndefMsg == null) {
                        // First try to see if this was a bad tag read
                        if (!tag.reconnect()) {
                            tag.disconnect();
                            if (mScreenState == ScreenStateHelper.SCREEN_STATE_ON_UNLOCKED) {
                                Toast.makeText(
                                                mContext,
                                                R.string.tag_read_error,
                                                Toast.LENGTH_SHORT)
                                        .show();
                            }
                            break;
                        }
                    }

                    if (debounceTagUid != null) {
                        // If we're debouncing and the UID or the NDEF message of the tag match,
                        // don't dispatch but drop it.
                        if (Arrays.equals(debounceTagUid, tag.getUid())
                                || (ndefMsg != null && ndefMsg.equals(mLastReadNdefMessage))) {
                            mHandler.removeMessages(MSG_TAG_DEBOUNCE);
                            mHandler.sendEmptyMessageDelayed(MSG_TAG_DEBOUNCE, debounceTagMs);
                            tag.disconnect();
                            return;
                        } else {
                            synchronized (NfcService.this) {
                                mDebounceTagUid = null;
                                mDebounceTagRemovedCallback = null;
                                mDebounceTagNativeHandle = INVALID_NATIVE_HANDLE;
                            }
                            if (debounceTagRemovedCallback != null) {
                                try {
                                    debounceTagRemovedCallback.onTagRemoved();
                                } catch (RemoteException e) {
                                    // Ignore
                                }
                            }
                        }
                    }

                    mLastReadNdefMessage = ndefMsg;

                    tag.startPresenceChecking(presenceCheckDelay, callback);
                    dispatchTagEndpoint(tag, readerParams);
                    break;

                case MSG_LLCP_LINK_ACTIVATION:
                    if (DBG)
                        Log.d(TAG, "NfcServiceHandler - handleMessage(MSG_LLCP_LINK_ACTIVATION)");

                    mPowerManager.userActivity(
                            SystemClock.uptimeMillis(), PowerManager.USER_ACTIVITY_EVENT_OTHER, 0);
                    if (mIsDebugBuild) {
                        Intent actIntent = new Intent(ACTION_LLCP_UP);
                        mContext.sendBroadcast(actIntent);
                    }
                    llcpActivated((NfcDepEndpoint) msg.obj);
                    break;

                case MSG_LLCP_LINK_DEACTIVATED:
                    if (DBG)
                        Log.d(TAG, "NfcServiceHandler - handleMessage(MSG_LLCP_LINK_DEACTIVATED)");

                    if (mIsDebugBuild) {
                        Intent deactIntent = new Intent(ACTION_LLCP_DOWN);
                        mContext.sendBroadcast(deactIntent);
                    }
                    NfcDepEndpoint device = (NfcDepEndpoint) msg.obj;
                    boolean needsDisconnect = false;

                    Log.d(TAG, "LLCP Link Deactivated message. Restart polling loop.");
                    synchronized (NfcService.this) {
                        /* Check if the device has been already unregistered */
                        if (mObjectMap.remove(device.getHandle()) != null) {
                            /* Disconnect if we are initiator */
                            if (device.getMode() == NfcDepEndpoint.MODE_P2P_TARGET) {
                                if (DBG) Log.d(TAG, "disconnecting from target");
                                needsDisconnect = true;
                            } else {
                                if (DBG) Log.d(TAG, "not disconnecting from initiator");
                            }
                        }
                    }
                    if (needsDisconnect) {
                        device.disconnect(); // restarts polling loop
                    }

                    mP2pLinkManager.onLlcpDeactivated();
                    break;
                case MSG_LLCP_LINK_FIRST_PACKET:
                    if (DBG)
                        Log.d(TAG, "NfcServiceHandler - handleMessage(MSG_LLCP_LINK_FIRST_PACKET)");
                    mP2pLinkManager.onLlcpFirstPacketReceived();
                    break;
                case MSG_RF_FIELD_ACTIVATED:
                    // Intent fieldOnIntent = new
                    // Intent(ACTION_RF_FIELD_ON_DETECTED);
                    // sendNfcEeAccessProtectedBroadcast(fieldOnIntent);
                    debounceRfField(1, true);
                    break;
                case MSG_RF_FIELD_DEACTIVATED:
                    // Intent fieldOffIntent = new
                    // Intent(ACTION_RF_FIELD_OFF_DETECTED);
                    // sendNfcEeAccessProtectedBroadcast(fieldOffIntent);
                    debounceRfField(0, true);
                    break;
                case MSG_RESUME_POLLING:
                    mNfcAdapter.resumePolling();
                    break;
                case MSG_TAG_DEBOUNCE:
                    // Didn't see the tag again, tag is gone
                    ITagRemovedCallback tagRemovedCallback;
                    synchronized (NfcService.this) {
                        mDebounceTagUid = null;
                        tagRemovedCallback = mDebounceTagRemovedCallback;
                        mDebounceTagRemovedCallback = null;
                        mDebounceTagNativeHandle = INVALID_NATIVE_HANDLE;
                    }
                    if (tagRemovedCallback != null) {
                        try {
                            tagRemovedCallback.onTagRemoved();
                        } catch (RemoteException e) {
                            // Ignore
                        }
                    }
                    break;
                case MSG_UPDATE_STATS:
                    if (mNumTagsDetected.get() > 0) {
                        MetricsLogger.count(mContext, TRON_NFC_TAG, mNumTagsDetected.get());
                        mNumTagsDetected.set(0);
                    }
                    if (mNumHceDetected.get() > 0) {
                        MetricsLogger.count(mContext, TRON_NFC_CE, mNumHceDetected.get());
                        mNumHceDetected.set(0);
                    }
                    if (mNumP2pDetected.get() > 0) {
                        MetricsLogger.count(mContext, TRON_NFC_P2P, mNumP2pDetected.get());
                        mNumP2pDetected.set(0);
                    }
                    removeMessages(MSG_UPDATE_STATS);
                    sendEmptyMessageDelayed(MSG_UPDATE_STATS, STATS_UPDATE_INTERVAL_MS);
                    break;

                case MSG_APPLY_SCREEN_STATE:
                    mScreenState = (Integer) msg.obj;
                    if (DBG)
                        Log.d(
                                TAG,
                                "NfcServiceHandler - handleMessage(MSG_APPLY_SCREEN_STATE) - state: "
                                        + ScreenStateHelper.screenStateToString(mScreenState));

                    // If NFC is turning off, we shouldn't need any changes here
                    synchronized (NfcService.this) {
                        //                        if (mState == NfcAdapter.STATE_TURNING_OFF)

                        if (mState != NfcAdapter.STATE_ON) {
                            if (DBG2)
                                Log.d(
                                        TAG,
                                        "NfcServiceHandler - handleMessage(MSG_APPLY_SCREEN_STATE) - NFC Service is not yet on, exiting");
                            return;
                        }
                    }

                    if (mScreenState == ScreenStateHelper.SCREEN_STATE_ON_UNLOCKED) {
                        applyRouting(false);
                    }
                    int screen_state_mask =
                            (mNfcUnlockManager.isLockscreenPollingEnabled())
                                    ? (ScreenStateHelper.SCREEN_POLLING_TAG_MASK | mScreenState)
                                    : mScreenState;

                    if (mNfcUnlockManager.isLockscreenPollingEnabled()) applyRouting(true);

                    mDeviceHost.doSetScreenState(screen_state_mask);
                    break;

                case MSG_TRANSACTION_EVENT:
                    if (DBG) Log.d(TAG, "NfcServiceHandler - handleMessage(MSG_TRANSACTION_EVENT)");

                    if (mCardEmulationManager != null) {
                        mCardEmulationManager.onOffHostAidSelected();
                    }
                    byte[][] data = (byte[][]) msg.obj;
                    sendOffHostTransactionEvent(data[0], data[1], data[2]);
                    break;

                case MSG_START_POLLING:
                    if (DBG) Log.d(TAG, "NfcServiceHandler - handleMessage(MSG_START_POLLING)");
                    applyRouting(true);
                    break;

                default:
                    Log.e(TAG, "Unknown message received");
                    break;
            }
        }

        private void sendOffHostTransactionEvent(byte[] aid, byte[] data, byte[] readerByteArray) {

            if (DBG2) Log.d(TAG, "NfcServiceHandler - sendOffHostTransactionEvent()");

            if (mSEService == null) {
                tryWaitForSEService();
            }

            if ((mSEService == null) || mNfcEventInstalledPackages.isEmpty()) {
                return;
            }

            try {
                String reader = new String(readerByteArray, "UTF-8");
                String[] installedPackages = new String[mNfcEventInstalledPackages.size()];
                boolean[] nfcAccess =
                        mSEService.isNFCEventAllowed(
                                reader, aid, mNfcEventInstalledPackages.toArray(installedPackages));
                if (nfcAccess == null) {
                    if (DBG2)
                        Log.d(
                                TAG,
                                "NfcServiceHandler - sendOffHostTransactionEvent() - nfcAccess is null");
                    return;
                }
                if (DBG2)
                    Log.d(
                            TAG,
                            "NfcServiceHandler - sendOffHostTransactionEvent() - nfcAccess.length "
                                    + nfcAccess.length);
                ArrayList<String> packages = new ArrayList<String>();
                Intent intent = new Intent(NfcAdapter.ACTION_TRANSACTION_DETECTED);
                intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                intent.putExtra(NfcAdapter.EXTRA_AID, aid);
                intent.putExtra(NfcAdapter.EXTRA_DATA, data);
                intent.putExtra(NfcAdapter.EXTRA_SECURE_ELEMENT_NAME, reader);
                StringBuilder aidString = new StringBuilder(aid.length);
                for (byte b : aid) {
                    aidString.append(String.format("%02X", b));
                }
                String url = new String("nfc://secure:0/" + reader + "/" + aidString.toString());
                intent.setData(Uri.parse(url));

                final BroadcastOptions options = BroadcastOptions.makeBasic();
                options.setBackgroundActivityStartsAllowed(true);
                for (int i = 0; i < nfcAccess.length; i++) {
                    if (DBG2)
                        Log.d(
                                TAG,
                                "NfcServiceHandler - sendOffHostTransactionEvent() - "
                                        + nfcAccess[i]
                                        + " <- "
                                        + mNfcEventInstalledPackages.get(i));
                    if (nfcAccess[i]) {
                        intent.setPackage(mNfcEventInstalledPackages.get(i));
                        mContext.sendBroadcast(intent, null, options.toBundle());
                    }
                }
            } catch (RemoteException e) {
                Log.e(TAG, "Error in isNFCEventAllowed() " + e);
            } catch (UnsupportedEncodingException e) {
                Log.e(TAG, "Incorrect format for Secure Element name" + e);
            }
        }

        /* Returns the list of packages that have access to NFC Events on any SE */
        private ArrayList<String> getSEAccessAllowedPackages() {

            if (DBG2) Log.d(TAG, "NfcServiceHandler - getSEAccessAllowedPackages()");

            if (mSEService == null) {
                tryWaitForSEService();
            }

            if ((mSEService == null) || mNfcEventInstalledPackages.isEmpty()) {
                return null;
            }
            String[] readers = null;
            try {
                readers = mSEService.getReaders();
            } catch (RemoteException e) {
                Log.e(TAG, "Error in getReaders() " + e);
                return null;
            }

            if (readers == null || readers.length == 0) {
                if (DBG2)
                    Log.d(TAG, "NfcServiceHandler - getSEAccessAllowedPackages() - no reader");
                return null;
            }
            boolean[] nfcAccessFinal = null;
            String[] installedPackages = new String[mNfcEventInstalledPackages.size()];
            for (String reader : readers) {
                try {
                    boolean[] accessList =
                            mSEService.isNFCEventAllowed(
                                    reader,
                                    null,
                                    mNfcEventInstalledPackages.toArray(installedPackages));
                    if (accessList == null) {
                        if (DBG2)
                            Log.d(
                                    TAG,
                                    "NfcServiceHandler - getSEAccessAllowedPackages() - r:"
                                            + reader
                                            + " - accessList is null");
                        continue;
                    }
                    if (nfcAccessFinal == null) {
                        nfcAccessFinal = accessList;
                    }
                    for (int i = 0; i < accessList.length; i++) {
                        if (accessList[i]) {
                            if (DBG2)
                                Log.d(
                                        TAG,
                                        "NfcServiceHandler - getSEAccessAllowedPackages() - r:"
                                                + reader
                                                + " - accessList["
                                                + i
                                                + "] is true");
                            nfcAccessFinal[i] = true;
                        }
                    }
                } catch (RemoteException e) {
                    Log.e(TAG, "Error in isNFCEventAllowed() " + e);
                }
            }
            if (nfcAccessFinal == null) {
                if (DBG2)
                    Log.d(
                            TAG,
                            "NfcServiceHandler - getSEAccessAllowedPackages() - accessListFinal is null");
                return null;
            }
            ArrayList<String> packages = new ArrayList<String>();
            for (int i = 0; i < nfcAccessFinal.length; i++) {
                if (nfcAccessFinal[i]) {
                    if (DBG2)
                        Log.d(
                                TAG,
                                "NfcServiceHandler - getSEAccessAllowedPackages() - auth: "
                                        + mNfcEventInstalledPackages.get(i));
                    packages.add(mNfcEventInstalledPackages.get(i));
                }
            }
            return packages;
        }

        private void sendNfcEeAccessProtectedBroadcast(Intent intent) {

            if (DBG2) Log.d(TAG, "NfcServiceHandler - sendNfcEeAccessProtectedBroadcast()");

            intent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
            // Resume app switches so the receivers can start activites without
            // delay
            mNfcDispatcher.resumeAppSwitches();
            /// MTK: @{
            intent.setPackage("com.android.settings");
            mContext.sendBroadcast(intent);
            /// }
            synchronized (this) {
                ArrayList<String> SEPackages = getSEAccessAllowedPackages();
                if (SEPackages != null && !SEPackages.isEmpty()) {
                    for (String packageName : SEPackages) {
                        intent.setPackage(packageName);
                        mContext.sendBroadcast(intent);
                    }
                }
                PackageManager pm = mContext.getPackageManager();
                for (String packageName : mNfcEventInstalledPackages) {
                    try {
                        PackageInfo info = pm.getPackageInfo(packageName, 0);
                        if (SEPackages != null && SEPackages.contains(packageName)) {
                            continue;
                        }
                        if (info.applicationInfo != null
                                && ((info.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) != 0
                                        || (info.applicationInfo.privateFlags
                                                        & ApplicationInfo.PRIVATE_FLAG_PRIVILEGED)
                                                != 0)) {
                            intent.setPackage(packageName);
                            mContext.sendBroadcast(intent);
                        }
                    } catch (Exception e) {
                        Log.e(TAG, "Exception in getPackageInfo " + e);
                    }
                }
            }
        }

        private boolean llcpActivated(NfcDepEndpoint device) {
            Log.d(TAG, "NfcServiceHandler - llcpActivated()");

            if (device.getMode() == NfcDepEndpoint.MODE_P2P_TARGET) {
                if (DBG) Log.d(TAG, "NativeP2pDevice.MODE_P2P_TARGET");
                if (device.connect()) {
                    /* Check LLCP compliancy */
                    if (mDeviceHost.doCheckLlcp()) {
                        /* Activate LLCP Link */
                        if (mDeviceHost.doActivateLlcp()) {
                            if (DBG) Log.d(TAG, "Initiator Activate LLCP OK");
                            synchronized (NfcService.this) {
                                // Register P2P device
                                mObjectMap.put(device.getHandle(), device);
                            }
                            mP2pLinkManager.onLlcpActivated(device.getLlcpVersion());
                            return true;
                        } else {
                            /* should not happen */
                            Log.w(TAG, "Initiator LLCP activation failed. Disconnect.");
                            device.disconnect();
                        }
                    } else {
                        if (DBG) Log.d(TAG, "Remote Target does not support LLCP. Disconnect.");
                        device.disconnect();
                    }
                } else {
                    if (DBG) Log.d(TAG, "Cannot connect remote Target. Polling loop restarted.");
                    /*
                     * The polling loop should have been restarted in failing
                     * doConnect
                     */
                }
            } else if (device.getMode() == NfcDepEndpoint.MODE_P2P_INITIATOR) {
                if (DBG) Log.d(TAG, "NativeP2pDevice.MODE_P2P_INITIATOR");
                /* Check LLCP compliancy */
                if (mDeviceHost.doCheckLlcp()) {
                    /* Activate LLCP Link */
                    if (mDeviceHost.doActivateLlcp()) {
                        if (DBG) Log.d(TAG, "Target Activate LLCP OK");
                        synchronized (NfcService.this) {
                            // Register P2P device
                            mObjectMap.put(device.getHandle(), device);
                        }
                        mP2pLinkManager.onLlcpActivated(device.getLlcpVersion());
                        return true;
                    }
                } else {
                    Log.w(TAG, "checkLlcp failed");
                }
            }

            return false;
        }

        private void dispatchTagEndpoint(TagEndpoint tagEndpoint, ReaderModeParams readerParams) {
            Tag tag =
                    new Tag(
                            tagEndpoint.getUid(),
                            tagEndpoint.getTechList(),
                            tagEndpoint.getTechExtras(),
                            tagEndpoint.getHandle(),
                            mNfcTagService);
            registerTagObject(tagEndpoint);
            if (readerParams != null) {
                try {
                    if ((readerParams.flags & NfcAdapter.FLAG_READER_NO_PLATFORM_SOUNDS) == 0) {
                        mVibrator.vibrate(mVibrationEffect);
                        playSound(SOUND_END);
                    }
                    if (readerParams.callback != null) {
                        if (DBG2) Log.d(TAG, "dispatchTagEndpoint() do onTagDiscovered()");
                        readerParams.callback.onTagDiscovered(tag);
                        if (DBG2) Log.d(TAG, "onTagDiscovered() End");
                        return;
                    } else {
                        // Follow normal dispatch below
                    }
                } catch (RemoteException e) {
                    Log.e(TAG, "Reader mode remote has died, falling back.", e);
                    // Intentional fall-through
                } catch (Exception e) {
                    // Catch any other exception
                    Log.e(TAG, "App exception, not dispatching.", e);
                    return;
                }
            }
            int dispatchResult = mNfcDispatcher.dispatchTag(tag);
            if (dispatchResult == NfcDispatcher.DISPATCH_FAIL && !mInProvisionMode) {
                unregisterObject(tagEndpoint.getHandle());
                if (mScreenState == ScreenStateHelper.SCREEN_STATE_ON_UNLOCKED) {
                    Toast.makeText(mContext, R.string.tag_dispatch_failed, Toast.LENGTH_SHORT)
                            .show();
                }
                playSound(SOUND_ERROR);
            } else if (dispatchResult == NfcDispatcher.DISPATCH_SUCCESS) {
                mVibrator.vibrate(mVibrationEffect);
                playSound(SOUND_END);
            }
        }

        // Handling of screen state change when RF field is ON
        // (RFST_LISTEN_ACTIVE state)
        private void applyScreenStateChange() {
            if (DBG)
                Log.d(TAG, "NfcServiceHandler - applyScreenStateChange(MSG_APPLY_SCREEN_STATE)");

            Log.d(TAG, "MSG_APPLY_SCREEN_STATE " + mScreenState);

            // If NFC is turning off, we shouldn't need any changes here
            synchronized (NfcService.this) {
                if (mState == NfcAdapter.STATE_TURNING_OFF) return;
            }

            if (mScreenState == ScreenStateHelper.SCREEN_STATE_ON_UNLOCKED) {
                applyRouting(false);
            }
            int screen_state_mask =
                    (mNfcUnlockManager.isLockscreenPollingEnabled())
                            ? (ScreenStateHelper.SCREEN_POLLING_TAG_MASK | mScreenState)
                            : mScreenState;

            if (mNfcUnlockManager.isLockscreenPollingEnabled()) applyRouting(false);

            mDeviceHost.doSetScreenState(screen_state_mask);
        }

        // RF Field info debouncer (RFDB)
        private final ScheduledExecutorService mRFDBScheduler = Executors.newScheduledThreadPool(1);
        private boolean mRFDBScheduled = false;
        private ScheduledFuture<?> mRFDBScheduledTask = null;

        void debouncedRfField(int field) {
            Intent fieldIntent =
                    new Intent(
                            field == 1
                                    ? ACTION_RF_FIELD_ON_DETECTED
                                    : ACTION_RF_FIELD_OFF_DETECTED);
            Settings.Global.putInt(mContext.getContentResolver(), "nfc_rf_field_active", field);
            if ((field == 0) && (mPendingRoutingTableUpdate == true)) {
                Log.d(TAG, "debouncedRfField - applying postponed routing table update ");

                mDeviceHost.commitRouting();

                mPendingRoutingTableUpdate = false;
            }
            if ((field == 0) && (mPendingPowerStateUpdate == true)) {
                mScreenState = mScreenStateHelper.checkScreenState();
                Log.d(TAG, "debouncedRfField - applying postponed screen state " + mScreenState);
                // new ApplyRoutingTask().execute(Integer.valueOf(screenState));
                applyScreenStateChange();
                mPendingPowerStateUpdate = false;
            }
            sendNfcEeAccessProtectedBroadcast(fieldIntent);
        }

        private class RFDBRunnable implements Runnable {
            private int mField;

            public RFDBRunnable(int field) {
                mField = field;
            }

            @Override
            public void run() {
                synchronized (NfcService.this) {
                    if (DBG) Log.d(TAG, "Sending RF_FIELD broadcast (debounced " + mField + ")");
                    mRFDBScheduled = false;
                }
                debouncedRfField(mField);
            }
        }

        private void debounceRfField(int field, boolean inversepolarity) {
            synchronized (NfcService.this) {
                int debouncedstate =
                        Settings.Global.getInt(
                                mContext.getContentResolver(), "nfc_rf_field_active", -1);
                if (DBG)
                    Log.d(
                            TAG,
                            "NfcServiceHandler - debounceRfField() - debouncing RF_FIELD: "
                                    + field
                                    + " (cur:"
                                    + debouncedstate
                                    + ", pol:"
                                    + inversepolarity
                                    + ")");

                if (inversepolarity && (debouncedstate != 1) && (field == 1)) {
                    // In inverse polarity, we give priority to setting FIELD ON
                    // state, then we debounce the field OFF.
                    // This is needed for situation where we expect we are not
                    // responding to a reader polling but we
                    // want to trigger something on the FIELD ON events anyway.
                    debouncedRfField(field);
                } else {
                    // In the normal case or in inverse polarity after we have
                    // set the FIELD ON state, we debounce field
                    // state changes until a stable state is reached, before
                    // broadcasting it.
                    if (mRFDBScheduled == false) {
                        // If no task is scheduled, create one
                        if (field == debouncedstate) {
                            if (DBG)
                                Log.d(
                                        TAG,
                                        "NfcServiceHandler - debounceRfField() - Ignoring, already the current state");
                        } else {
                            final Runnable sender = new RFDBRunnable(field);
                            mRFDBScheduled = true;
                            mRFDBScheduledTask =
                                    mRFDBScheduler.schedule(sender, 150, TimeUnit.MILLISECONDS);
                        }
                    } else {
                        // If a task is already scheduled, cancel it
                        if (field == debouncedstate) {
                            mRFDBScheduledTask.cancel(false);
                            mRFDBScheduled = false;
                        } else {
                            if (DBG)
                                Log.d(
                                        TAG,
                                        "NfcServiceHandler - debounceRfField() - Already on fly");
                        }
                    }
                }
            }
        }
    }

    private NfcServiceHandler mHandler = new NfcServiceHandler();

    class ApplyRoutingTask extends AsyncTask<Integer, Void, Void> {
        @Override
        protected Void doInBackground(Integer... params) {
            synchronized (NfcService.this) {
                if (params == null || params.length != 1) {
                    // force apply current routing
                    applyRouting(true);
                    return null;
                }
                mScreenState = params[0].intValue();
                if (DBG2) {
                    Log.d(
                            TAG,
                            "ApplyRoutingTask.doInBackground() mScreenState:"
                                    + ScreenStateHelper.screenStateToString(mScreenState));
                }

                if (1
                        == Settings.Global.getInt(
                                mContext.getContentResolver(), "nfc_rf_field_active", -1)) {
                    Log.d(TAG, "ApplyRoutingTask.doInBackground() postponing due to RF FIELD ON");
                    mPendingPowerStateUpdate = true;
                    return null;
                }
                mPendingPowerStateUpdate = false;

                mRoutingWakeLock.acquire();
                try {
                    applyRouting(false);
                } finally {
                    mRoutingWakeLock.release();
                }
                return null;
            }
        }
    }

    private final BroadcastReceiver mReceiver =
            new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    String action = intent.getAction();
                    if (DBG2) Log.d(TAG, "BroadcastReceiver - onReceive() - action:" + action);

                    if (action.equals(Intent.ACTION_SCREEN_ON)
                            || action.equals(Intent.ACTION_SCREEN_OFF)
                            || action.equals(Intent.ACTION_USER_PRESENT)) {
                        // Perform applyRouting() in AsyncTask to serialize blocking calls
                        int screenState = mScreenStateHelper.checkScreenState();
                        if (action.equals(Intent.ACTION_SCREEN_OFF)) {
                            if (mScreenState != ScreenStateHelper.SCREEN_STATE_OFF_LOCKED) {
                                screenState =
                                        mKeyguard.isKeyguardLocked()
                                                ? ScreenStateHelper.SCREEN_STATE_OFF_LOCKED
                                                : ScreenStateHelper.SCREEN_STATE_OFF_UNLOCKED;

                                if (screenState == ScreenStateHelper.SCREEN_STATE_OFF_LOCKED) {
                                    Log.d(TAG, "BroadcastReceiver - SCREEN_STATE_OFF_LOCKED");
                                } else if (screenState
                                        == ScreenStateHelper.SCREEN_STATE_OFF_UNLOCKED) {
                                    Log.d(TAG, "BroadcastReceiver - SCREEN_STATE_OFF_UNLOCKED");
                                }
                            }
                        } else if (action.equals(Intent.ACTION_SCREEN_ON)) {
                            screenState =
                                    mKeyguard.isKeyguardLocked()
                                            ? ScreenStateHelper.SCREEN_STATE_ON_LOCKED
                                            : ScreenStateHelper.SCREEN_STATE_ON_UNLOCKED;
                            if (screenState == ScreenStateHelper.SCREEN_STATE_ON_LOCKED) {
                                Log.d(TAG, "BroadcastReceiver - SCREEN_STATE_ON_LOCKED");
                            } else {
                                Log.d(TAG, "BroadcastReceiver - SCREEN_STATE_ON_UNLOCKED");
                            }
                        } else if (action.equals(Intent.ACTION_USER_PRESENT)) {
                            screenState = ScreenStateHelper.SCREEN_STATE_ON_UNLOCKED;
                            Log.d(TAG, "BroadcastReceiver - SCREEN_STATE_ON_UNLOCKED");
                        }
                        if (nci_version != NCI_VERSION_2_0) {
                            new ApplyRoutingTask().execute(Integer.valueOf(screenState));
                        }
                        if (1
                                == Settings.Global.getInt(
                                        mContext.getContentResolver(), "nfc_rf_field_active", -1)) {
                            Log.d(TAG, "MSG_APPLY_SCREEN_STATE postponing due to RF FIELD ON");
                            mPendingPowerStateUpdate = true;
                        } else if (isTagPresent()) {
                            Log.d(TAG, "MSG_APPLY_SCREEN_STATE postponing due to TAG CONNECTED");
                            mPendingPowerStateUpdate = true;
                        } else {
                            sendMessage(NfcService.MSG_APPLY_SCREEN_STATE, screenState);
                        }
                    } else if (action.equals(Intent.ACTION_USER_SWITCHED)) {
                        int userId = intent.getIntExtra(Intent.EXTRA_USER_HANDLE, 0);
                        if (mIsBeamCapable) {
                            int beamSetting = PackageManager.COMPONENT_ENABLED_STATE_DEFAULT;
                            try {
                                IPackageManager mIpm =
                                        IPackageManager.Stub.asInterface(
                                                ServiceManager.getService("package"));
                                beamSetting =
                                        mIpm.getComponentEnabledSetting(
                                                new ComponentName(
                                                        BeamShareActivity.class.getPackageName$(),
                                                        BeamShareActivity.class.getName()),
                                                userId);
                            } catch (RemoteException e) {
                                Log.e(
                                        TAG,
                                        "Error int getComponentEnabledSetting for BeamShareActivity");
                            }
                            synchronized (this) {
                                mUserId = userId;
                                if (beamSetting
                                        == PackageManager.COMPONENT_ENABLED_STATE_DISABLED) {
                                    mIsNdefPushEnabled = false;
                                } else {
                                    mIsNdefPushEnabled = true;
                                }
                                // Propagate the state change to all user profiles
                                UserManager um =
                                        (UserManager)
                                                mContext.getSystemService(Context.USER_SERVICE);
                                List<UserHandle> luh = um.getUserProfiles();
                                for (UserHandle uh : luh) {
                                    enforceBeamShareActivityPolicy(mContext, uh);
                                }
                                enforceBeamShareActivityPolicy(mContext, new UserHandle(mUserId));
                            }
                            mP2pLinkManager.onUserSwitched(getUserId());
                        }
                        if (mIsHceCapable) {
                            mCardEmulationManager.onUserSwitched(getUserId());
                        }
                        int screenState = mScreenStateHelper.checkScreenState();
                        if (screenState != mScreenState) {
                            new ApplyRoutingTask().execute(Integer.valueOf(screenState));
                        }
                    }
                }
            };

    private final BroadcastReceiver mOwnerReceiver =
            new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    String action = intent.getAction();
                    if (action.equals(Intent.ACTION_PACKAGE_REMOVED)
                            || action.equals(Intent.ACTION_PACKAGE_ADDED)
                            || action.equals(Intent.ACTION_EXTERNAL_APPLICATIONS_AVAILABLE)
                            || action.equals(Intent.ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE)) {
                        updatePackageCache();
                    } else if (action.equals(Intent.ACTION_SHUTDOWN)) {
                        if (DBG) Log.d(TAG, "Device is shutting down.");
                        if (isNfcEnabled()) {
                            mDeviceHost.shutdown();
                        }
                    }
                }
            };

    private final BroadcastReceiver mPolicyReceiver =
            new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    String action = intent.getAction();
                    if (DevicePolicyManager.ACTION_DEVICE_POLICY_MANAGER_STATE_CHANGED.equals(
                            action)) {
                        enforceBeamShareActivityPolicy(context, new UserHandle(getSendingUserId()));
                    }
                }
            };

    private final IVrStateCallbacks mVrStateCallbacks =
            new IVrStateCallbacks.Stub() {
                @Override
                public void onVrStateChanged(boolean enabled) {
                    synchronized (this) {
                        mIsVrModeEnabled = enabled;
                    }
                }
            };

    /** for debugging only - no i18n */
    static String stateToString(int state) {
        switch (state) {
            case NfcAdapter.STATE_OFF:
                return "off";
            case NfcAdapter.STATE_TURNING_ON:
                return "turning on";
            case NfcAdapter.STATE_ON:
                return "on";
            case NfcAdapter.STATE_TURNING_OFF:
                return "turning off";
            default:
                return "<error>";
        }
    }

    private void copyNativeCrashLogsIfAny(PrintWriter pw) {
        try {
            File file = new File(mContext.getFilesDir(), NATIVE_LOG_FILE_NAME);
            if (!file.exists()) {
                return;
            }
            pw.println("---BEGIN: NATIVE CRASH LOG----");
            Scanner sc = new Scanner(file);
            while (sc.hasNextLine()) {
                String s = sc.nextLine();
                pw.println(s);
            }
            pw.println("---END: NATIVE CRASH LOG----");
            sc.close();
        } catch (IOException e) {
            Log.e(TAG, "Exception in copyNativeCrashLogsIfAny " + e);
        }
    }

    private void storeNativeCrashLogs() {
        try {
            File file = new File(mContext.getFilesDir(), NATIVE_LOG_FILE_NAME);
            if (!file.exists()) {
                file.createNewFile();
            }

            FileOutputStream fos = new FileOutputStream(file);
            mDeviceHost.dump(fos.getFD());
            fos.flush();
            fos.close();
        } catch (IOException e) {
            Log.e(TAG, "Exception in storeNativeCrashLogs " + e);
        }
    }

    void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        if (mContext.checkCallingOrSelfPermission(android.Manifest.permission.DUMP)
                != PackageManager.PERMISSION_GRANTED) {
            pw.println(
                    "Permission Denial: can't dump nfc from from pid="
                            + Binder.getCallingPid()
                            + ", uid="
                            + Binder.getCallingUid()
                            + " without permission "
                            + android.Manifest.permission.DUMP);
            return;
        }

        synchronized (this) {
            pw.println("mState=" + stateToString(mState));
            pw.println("mIsZeroClickRequested=" + mIsNdefPushEnabled);
            pw.println("mScreenState=" + ScreenStateHelper.screenStateToString(mScreenState));
            pw.println("mIsSecureNfcEnabled=" + mIsSecureNfcEnabled);
            pw.println(mCurrentDiscoveryParameters);
            if (mIsBeamCapable) mP2pLinkManager.dump(fd, pw, args);
            if (mIsHceCapable) {
                mCardEmulationManager.dump(fd, pw, args);
            }
            mNfcDispatcher.dump(fd, pw, args);
            copyNativeCrashLogsIfAny(pw);
            pw.flush();
            mDeviceHost.dump(fd);
        }
    }

    /** Update the status of all the services which were populated to commit to routing table */
    public void updateStatusOfServices(boolean commitStatus) {

        if (DBG2) Log.d(TAG, "updateStatusOfServices() - commitStatus: " + commitStatus);

        mCardEmulationManager.updateStatusOfServices(commitStatus);
    }
}
