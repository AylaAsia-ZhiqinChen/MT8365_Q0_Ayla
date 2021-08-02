/**
 * Copyright (C) 2014 MediaTek Inc.
 * Modification based on code covered by the mentioned copyright
 * and/or permission notice(s).
 */

package com.android.server.wifi;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.anyInt;
import static org.mockito.Mockito.doThrow;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import android.net.wifi.IStaStateCallback;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.test.TestLooper;

import androidx.test.filters.SmallTest;

import com.android.server.wifi.util.ExternalCallbackTracker;

import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

/**
 * Unit tests for {@link com.android.server.wifi.WifiStaStateNotifier}.
 */
@SmallTest
public class WifiStaStateNotifierTest {
    public static final String TAG = "WifiStaStateNotifierTest";

    private TestLooper mLooper;
    private WifiStaStateNotifier mWifiStaStateNotifier;
    private static final int TEST_STA_STATE_CALLBACK_IDENTIFIER = 18;

    @Mock IBinder mAppBinder;
    @Mock IStaStateCallback mStaStateCallback;
    @Mock ExternalCallbackTracker<IStaStateCallback> mCallbackTracker;
    @Mock WifiInjector mWifiInjector;
    @Mock ActiveModeWarden mActiveModeWarden;

    /**
     * Called before each test
     */
    @Before
    public void setUp() throws Exception {
        // Ensure looper exists
        mLooper = new TestLooper();
        MockitoAnnotations.initMocks(this);

        mWifiStaStateNotifier = new WifiStaStateNotifier(mLooper.getLooper(), mWifiInjector);
        when(mWifiInjector.getActiveModeWarden()).thenReturn(mActiveModeWarden);
    }

    /**
     * Verify that clients should be notified if STA is going to be off.
     */
    @Test
    public void testClientNotification() throws RemoteException {
        // Register Client to verify that onStaToBeOff()is called.
        mWifiStaStateNotifier.addCallback(
                mAppBinder, mStaStateCallback, TEST_STA_STATE_CALLBACK_IDENTIFIER);
        mWifiStaStateNotifier.onStaToBeOff();

        // Client should get the callback
        verify(mStaStateCallback).onStaToBeOff();
    }

    /**
     * Verify that remove client should be handled
     */
    @Test
    public void testRemoveClient() throws RemoteException {
        // Register Client to verify that onStaToBeOff()is called.
        mWifiStaStateNotifier.addCallback(
                mAppBinder, mStaStateCallback, TEST_STA_STATE_CALLBACK_IDENTIFIER);
        mWifiStaStateNotifier.removeCallback(TEST_STA_STATE_CALLBACK_IDENTIFIER);
        verify(mAppBinder).unlinkToDeath(any(), anyInt());

        mWifiStaStateNotifier.onStaToBeOff();

        // Client should not get any message after the client is removed.
        verify(mStaStateCallback, never()).onStaToBeOff();
    }

    /**
     * Verify that remove client ignores when callback identifier is wrong.
     */
    @Test
    public void testRemoveClientWithWrongIdentifier() throws RemoteException {
        // Register Client to verify that onStaToBeOff()is called.
        mWifiStaStateNotifier.addCallback(
                mAppBinder, mStaStateCallback, TEST_STA_STATE_CALLBACK_IDENTIFIER);
        mWifiStaStateNotifier.removeCallback(TEST_STA_STATE_CALLBACK_IDENTIFIER + 5);
        mLooper.dispatchAll();

        mWifiStaStateNotifier.onStaToBeOff();

        // Client should get the callback
        verify(mStaStateCallback).onStaToBeOff();
    }

    /**
     *
     * Verify that sta state notifier registers for death notification on adding client.
     */
    @Test
    public void registersForBinderDeathOnAddClient() throws Exception {
        mWifiStaStateNotifier.addCallback(
                mAppBinder, mStaStateCallback, TEST_STA_STATE_CALLBACK_IDENTIFIER);
        verify(mAppBinder).linkToDeath(any(IBinder.DeathRecipient.class), anyInt());
    }

    /**
     *
     * Verify that sta state notifier registers for death notification on adding client.
     */
    @Test
    public void addCallbackFailureOnLinkToDeath() throws Exception {
        doThrow(new RemoteException())
                .when(mAppBinder).linkToDeath(any(IBinder.DeathRecipient.class), anyInt());
        mWifiStaStateNotifier.addCallback(
                mAppBinder, mStaStateCallback, TEST_STA_STATE_CALLBACK_IDENTIFIER);
        verify(mAppBinder).linkToDeath(any(IBinder.DeathRecipient.class), anyInt());

        mWifiStaStateNotifier.onStaToBeOff();

        // Client should not get any callback
        verify(mStaStateCallback, never()).onStaToBeOff();
    }
}
