/*
 * Copyright (C) 2014 The Android Open Source Project
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

package com.android.server.telecom;

import com.android.internal.annotations.VisibleForTesting;
import com.android.server.telecom.bluetooth.BluetoothDeviceManager;
import com.android.server.telecom.bluetooth.BluetoothRouteManager;
import com.android.server.telecom.bluetooth.BluetoothStateReceiver;
import com.android.server.telecom.components.UserCallIntentProcessor;
import com.android.server.telecom.components.UserCallIntentProcessorFactory;
import com.android.server.telecom.ui.IncomingCallNotifier;
import com.android.server.telecom.ui.MissedCallNotifierImpl.MissedCallNotifierImplFactory;
import com.android.server.telecom.BluetoothPhoneServiceImpl.BluetoothPhoneServiceImplFactory;
import com.android.server.telecom.CallAudioManager.AudioServiceFactory;
import com.android.server.telecom.DefaultDialerCache.DefaultDialerManagerAdapter;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.UserHandle;
import android.os.UserManager;
/// M: Using mtk log instead of AOSP log.
// import android.telecom.Log;
import android.telecom.PhoneAccountHandle;

/// M: Mediatek import.
import com.mediatek.server.telecom.Log;
import com.mediatek.server.telecom.MtkUtil;

import java.io.FileNotFoundException;
import java.io.InputStream;

/**
 * Top-level Application class for Telecom.
 */
public class TelecomSystem {

    /**
     * This interface is implemented by system-instantiated components (e.g., Services and
     * Activity-s) that wish to use the TelecomSystem but would like to be testable. Such a
     * component should implement the getTelecomSystem() method to return the global singleton,
     * and use its own method. Tests can subclass the component to return a non-singleton.
     *
     * A refactoring goal for Telecom is to limit use of the TelecomSystem singleton to those
     * system-instantiated components, and have all other parts of the system just take all their
     * dependencies as explicit arguments to their constructor or other methods.
     */
    public interface Component {
        TelecomSystem getTelecomSystem();
    }


    /**
     * Tagging interface for the object used for synchronizing multi-threaded operations in
     * the Telecom system.
     */
    public interface SyncRoot {
    }

    private static final IntentFilter USER_SWITCHED_FILTER =
            new IntentFilter(Intent.ACTION_USER_SWITCHED);

    private static final IntentFilter USER_STARTING_FILTER =
            new IntentFilter(Intent.ACTION_USER_STARTING);

    private static final IntentFilter BOOT_COMPLETE_FILTER =
            new IntentFilter(Intent.ACTION_BOOT_COMPLETED);

    /** Intent filter for dialer secret codes. */
    private static final IntentFilter DIALER_SECRET_CODE_FILTER;

    /**
     * Initializes the dialer secret code intent filter.  Setup to handle the various secret codes
     * which can be dialed (e.g. in format *#*#code#*#*) to trigger various behavior in Telecom.
     */
    static {
        DIALER_SECRET_CODE_FILTER = new IntentFilter(
                "android.provider.Telephony.SECRET_CODE");
        DIALER_SECRET_CODE_FILTER.addDataScheme("android_secret_code");
        DIALER_SECRET_CODE_FILTER
                .addDataAuthority(DialerCodeReceiver.TELECOM_SECRET_CODE_DEBUG_ON, null);
        DIALER_SECRET_CODE_FILTER
                .addDataAuthority(DialerCodeReceiver.TELECOM_SECRET_CODE_DEBUG_OFF, null);
        DIALER_SECRET_CODE_FILTER
                .addDataAuthority(DialerCodeReceiver.TELECOM_SECRET_CODE_MARK, null);
        DIALER_SECRET_CODE_FILTER
                .addDataAuthority(DialerCodeReceiver.TELECOM_SECRET_CODE_MENU, null);
    }

    private static TelecomSystem INSTANCE = null;

    private final SyncRoot mLock = new SyncRoot() { };
    private final MissedCallNotifier mMissedCallNotifier;
    private final IncomingCallNotifier mIncomingCallNotifier;
    private final PhoneAccountRegistrar mPhoneAccountRegistrar;
    private final CallsManager mCallsManager;
    private final RespondViaSmsManager mRespondViaSmsManager;
    private final Context mContext;
    private final BluetoothPhoneServiceImpl mBluetoothPhoneServiceImpl;
    private final CallIntentProcessor mCallIntentProcessor;
    private final TelecomBroadcastIntentProcessor mTelecomBroadcastIntentProcessor;
    private final TelecomServiceImpl mTelecomServiceImpl;
    private final ContactsAsyncHelper mContactsAsyncHelper;
    private final DialerCodeReceiver mDialerCodeReceiver;

    private boolean mIsBootComplete = false;

    private final BroadcastReceiver mUserSwitchedReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.startSession("TSSwR.oR");
            try {
                synchronized (mLock) {
                    int userHandleId = intent.getIntExtra(Intent.EXTRA_USER_HANDLE, 0);
                    UserHandle currentUserHandle = new UserHandle(userHandleId);
                    mPhoneAccountRegistrar.setCurrentUserHandle(currentUserHandle);
                    mCallsManager.onUserSwitch(currentUserHandle);
                }
            } finally {
                Log.endSession();
            }
        }
    };

    private final BroadcastReceiver mUserStartingReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.startSession("TSStR.oR");
            try {
                synchronized (mLock) {
                    int userHandleId = intent.getIntExtra(Intent.EXTRA_USER_HANDLE, 0);
                    UserHandle addingUserHandle = new UserHandle(userHandleId);
                    mCallsManager.onUserStarting(addingUserHandle);
                }
            } finally {
                Log.endSession();
            }
        }
    };

    private final BroadcastReceiver mBootCompletedReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.startSession("TSBCR.oR");
            try {
                synchronized (mLock) {
                    mIsBootComplete = true;
                    mCallsManager.onBootCompleted();
                }
            } finally {
                Log.endSession();
            }
        }
    };

    public static TelecomSystem getInstance() {
        return INSTANCE;
    }

    public static void setInstance(TelecomSystem instance) {
        if (INSTANCE != null) {
            Log.w("TelecomSystem", "Attempt to set TelecomSystem.INSTANCE twice");
        }
        Log.i(TelecomSystem.class, "TelecomSystem.INSTANCE being set");
        INSTANCE = instance;
    }

    public TelecomSystem(
            Context context,
            MissedCallNotifierImplFactory missedCallNotifierImplFactory,
            CallerInfoAsyncQueryFactory callerInfoAsyncQueryFactory,
            HeadsetMediaButtonFactory headsetMediaButtonFactory,
            ProximitySensorManagerFactory proximitySensorManagerFactory,
            InCallWakeLockControllerFactory inCallWakeLockControllerFactory,
            AudioServiceFactory audioServiceFactory,
            BluetoothPhoneServiceImplFactory
                    bluetoothPhoneServiceImplFactory,
            ConnectionServiceFocusManager.ConnectionServiceFocusManagerFactory
                    connectionServiceFocusManagerFactory,
            Timeouts.Adapter timeoutsAdapter,
            AsyncRingtonePlayer asyncRingtonePlayer,
            PhoneNumberUtilsAdapter phoneNumberUtilsAdapter,
            IncomingCallNotifier incomingCallNotifier,
            InCallTonePlayer.ToneGeneratorFactory toneGeneratorFactory,
            CallAudioRouteStateMachine.Factory callAudioRouteStateMachineFactory,
            ClockProxy clockProxy,
            RoleManagerAdapter roleManagerAdapter) {
        mContext = context.getApplicationContext();

        /// M: ALPS03823512 Fix event log register multiple listeners in auto test. @{
        /// Scenario:
        /// 1.  Run one TelecomSystemTest case will create one TelecomSystem object.
        /// 2.  Multiple cases cause event log register multiple listeners without unregister.
        /// 3.  Multiple listeners cause poor performance then cause timing issue.
        /// Solution:
        /// Only call LogUtils.initLogging when telecom test process first create TelecomSystem.
        if (mContext.getPackageName().equals(TELECOM_TESTS_PACKAGE_NAME)) {
            if (!sIsInitDoneForTest) {
                LogUtils.initLogging(mContext);
                sIsInitDoneForTest = true;
            }
        } else {
            LogUtils.initLogging(mContext);
        }
        /// @}

        /// M: ALPS04694035, fix google dialer cannot support prebind feature issue.
        // Google dialer does not expect telecom will bind inCallService when processIntent stage,
        // it maybe show white screen in MO call need select phone account case. So turn off prebind
        // feature for MO call using google dialer case.
        updatePreBindFeatureEnableState();

        DefaultDialerManagerAdapter defaultDialerAdapter =
                new DefaultDialerCache.DefaultDialerManagerAdapterImpl();

        DefaultDialerCache defaultDialerCache = new DefaultDialerCache(mContext,
                defaultDialerAdapter, roleManagerAdapter, mLock);

        Log.startSession("TS.init");
        mPhoneAccountRegistrar = new PhoneAccountRegistrar(mContext, defaultDialerCache,
                new PhoneAccountRegistrar.AppLabelProxy() {
                    @Override
                    public CharSequence getAppLabel(String packageName) {
                        PackageManager pm = mContext.getPackageManager();
                        try {
                            ApplicationInfo info = pm.getApplicationInfo(packageName, 0);
                            return pm.getApplicationLabel(info);
                        } catch (PackageManager.NameNotFoundException nnfe) {
                            Log.w(this, "Could not determine package name.");
                        }

                        return null;
                    }
                });
        mContactsAsyncHelper = new ContactsAsyncHelper(
                new ContactsAsyncHelper.ContentResolverAdapter() {
                    @Override
                    public InputStream openInputStream(Context context, Uri uri)
                            throws FileNotFoundException {
                        return context.getContentResolver().openInputStream(uri);
                    }
                });
        BluetoothDeviceManager bluetoothDeviceManager = new BluetoothDeviceManager(mContext,
                new BluetoothAdapterProxy());
        BluetoothRouteManager bluetoothRouteManager = new BluetoothRouteManager(mContext, mLock,
                bluetoothDeviceManager, new Timeouts.Adapter());
        BluetoothStateReceiver bluetoothStateReceiver = new BluetoothStateReceiver(
                bluetoothDeviceManager, bluetoothRouteManager);
        mContext.registerReceiver(bluetoothStateReceiver, BluetoothStateReceiver.INTENT_FILTER);

        WiredHeadsetManager wiredHeadsetManager = new WiredHeadsetManager(mContext);
        SystemStateHelper systemStateHelper = new SystemStateHelper(mContext);

        mMissedCallNotifier = missedCallNotifierImplFactory
                .makeMissedCallNotifierImpl(mContext, mPhoneAccountRegistrar, defaultDialerCache);

        CallerInfoLookupHelper callerInfoLookupHelper =
                new CallerInfoLookupHelper(context, callerInfoAsyncQueryFactory,
                        mContactsAsyncHelper, mLock);

        EmergencyCallHelper emergencyCallHelper = new EmergencyCallHelper(mContext,
                TelecomServiceImpl.getSystemDialerPackage(mContext), timeoutsAdapter);

        InCallControllerFactory inCallControllerFactory = new InCallControllerFactory() {
            @Override
            public InCallController create(Context context, SyncRoot lock,
                    CallsManager callsManager, SystemStateHelper systemStateProvider,
                    DefaultDialerCache defaultDialerCache, Timeouts.Adapter timeoutsAdapter,
                    EmergencyCallHelper emergencyCallHelper) {
                return new InCallController(context, lock, callsManager, systemStateProvider,
                        defaultDialerCache, timeoutsAdapter, emergencyCallHelper);
            }
        };

        mCallsManager = new CallsManager(
                mContext,
                mLock,
                callerInfoLookupHelper,
                mMissedCallNotifier,
                mPhoneAccountRegistrar,
                headsetMediaButtonFactory,
                proximitySensorManagerFactory,
                inCallWakeLockControllerFactory,
                connectionServiceFocusManagerFactory,
                audioServiceFactory,
                bluetoothRouteManager,
                wiredHeadsetManager,
                systemStateHelper,
                defaultDialerCache,
                timeoutsAdapter,
                asyncRingtonePlayer,
                phoneNumberUtilsAdapter,
                emergencyCallHelper,
                toneGeneratorFactory,
                clockProxy,
                bluetoothStateReceiver,
                callAudioRouteStateMachineFactory,
                new CallAudioModeStateMachine.Factory(),
                inCallControllerFactory,
                roleManagerAdapter);

        mIncomingCallNotifier = incomingCallNotifier;
        incomingCallNotifier.setCallsManagerProxy(new IncomingCallNotifier.CallsManagerProxy() {
            @Override
            public boolean hasUnholdableCallsForOtherConnectionService(
                    PhoneAccountHandle phoneAccountHandle) {
                return mCallsManager.hasUnholdableCallsForOtherConnectionService(
                        phoneAccountHandle);
            }

            @Override
            public int getNumUnholdableCallsForOtherConnectionService(
                    PhoneAccountHandle phoneAccountHandle) {
                return mCallsManager.getNumUnholdableCallsForOtherConnectionService(
                        phoneAccountHandle);
            }

            @Override
            public Call getActiveCall() {
                return mCallsManager.getActiveCall();
            }
        });
        mCallsManager.setIncomingCallNotifier(mIncomingCallNotifier);

        mRespondViaSmsManager = new RespondViaSmsManager(mCallsManager, mLock);
        mCallsManager.setRespondViaSmsManager(mRespondViaSmsManager);

        mContext.registerReceiver(mUserSwitchedReceiver, USER_SWITCHED_FILTER);
        mContext.registerReceiver(mUserStartingReceiver, USER_STARTING_FILTER);
        mContext.registerReceiver(mBootCompletedReceiver, BOOT_COMPLETE_FILTER);

        mBluetoothPhoneServiceImpl = bluetoothPhoneServiceImplFactory.makeBluetoothPhoneServiceImpl(
                mContext, mLock, mCallsManager, mPhoneAccountRegistrar);
        mCallIntentProcessor = new CallIntentProcessor(mContext, mCallsManager);
        mTelecomBroadcastIntentProcessor = new TelecomBroadcastIntentProcessor(
                mContext, mCallsManager);

        // Register the receiver for the dialer secret codes, used to enable extended logging.
        mDialerCodeReceiver = new DialerCodeReceiver(mCallsManager);
        mContext.registerReceiver(mDialerCodeReceiver, DIALER_SECRET_CODE_FILTER,
                Manifest.permission.CONTROL_INCALL_EXPERIENCE, null);

        // There is no USER_SWITCHED broadcast for user 0, handle it here explicitly.
        final UserManager userManager = UserManager.get(mContext);
        mTelecomServiceImpl = new TelecomServiceImpl(
                mContext, mCallsManager, mPhoneAccountRegistrar,
                new CallIntentProcessor.AdapterImpl(),
                new UserCallIntentProcessorFactory() {
                    @Override
                    public UserCallIntentProcessor create(Context context, UserHandle userHandle) {
                        return new UserCallIntentProcessor(context, userHandle);
                    }
                },
                defaultDialerCache,
                new TelecomServiceImpl.SubscriptionManagerAdapterImpl(),
                new TelecomServiceImpl.SettingsSecureAdapterImpl(),
                mLock);
        Log.endSession();
    }

    @VisibleForTesting
    public PhoneAccountRegistrar getPhoneAccountRegistrar() {
        return mPhoneAccountRegistrar;
    }

    @VisibleForTesting
    public CallsManager getCallsManager() {
        return mCallsManager;
    }

    public BluetoothPhoneServiceImpl getBluetoothPhoneServiceImpl() {
        return mBluetoothPhoneServiceImpl;
    }

    public CallIntentProcessor getCallIntentProcessor() {
        return mCallIntentProcessor;
    }

    public TelecomBroadcastIntentProcessor getTelecomBroadcastIntentProcessor() {
        return mTelecomBroadcastIntentProcessor;
    }

    public TelecomServiceImpl getTelecomServiceImpl() {
        return mTelecomServiceImpl;
    }

    public Object getLock() {
        return mLock;
    }

    public boolean isBootComplete() {
        return mIsBootComplete;
    }

    /// M: ALPS03823512 Fix event log register multiple listeners in auto test. @{
    private static final String TELECOM_TESTS_PACKAGE_NAME = "com.android.server.telecom.tests";
    private static boolean sIsInitDoneForTest = false;
    /// @}

    /**
     * M: ALPS04694035, fix google dialer cannot support prebind feature issue. @{
     * Check default dialer name, turn off prebind feature if it is google dialer.
     */
    private void updatePreBindFeatureEnableState() {
        String defaultDialerName = mContext.getResources().getString(
                com.android.internal.R.string.config_defaultDialer);
        String googleDialerName = "com.google.android.dialer";
        if (googleDialerName.equals(defaultDialerName)) {
            Log.i(TelecomSystem.class,
                    "updatePreBindFeatureEnableState: disable prebind feature for google dialer");
            MtkUtil.PREBIND_INCALLSERVICE_ENABLED = false;
        } else {
            MtkUtil.PREBIND_INCALLSERVICE_ENABLED = true;
        }
    }
}
