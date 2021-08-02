/**
 * Copyright (C) 2014 MediaTek Inc.
 * Modification based on code covered by the mentioned copyright
 * and/or permission notice(s).
 */

package com.android.server.wifi;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.*;

import android.content.Context;
import android.net.wifi.WifiManager;
import android.os.Message;
import android.os.SystemClock;
import android.os.test.TestLooper;
import android.util.Log;

import androidx.test.filters.SmallTest;

import com.android.internal.app.IBatteryStats;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.mockito.invocation.InvocationOnMock;
import org.mockito.stubbing.Answer;

import java.util.ArrayList;

/**
 * Unit tests for {@link com.android.server.wifi.ActiveModeWardenForDeferRequest}.
 */
@SmallTest
public class ActiveModeWardenForDeferRequestTest {
    public static final String TAG = "ActiveModeWardenForDeferRequestTest";

    @Mock WifiInjector mWifiInjector;
    @Mock Context mContext;
    @Mock WifiNative mWifiNative;
    @Mock DefaultModeManager mDefaultModeManager;
    @Mock IBatteryStats mBatteryStats;
    @Mock WifiStaStateNotifier mWifiStaStateNotifier;
    @Mock ClientModeManager mClientModeManager;
    @Mock ScanOnlyModeManager mScanOnlyModeManager;
    @Mock SoftApManager mSoftApManager;
    @Mock HalDeviceManager mHalDeviceManager;
    TestLooper mLooper;
    ActiveModeWardenForDeferRequest mActiveModeWardenForDeferRequest;
    ScanOnlyModeManager.Listener mScanOnlyListener;
    WifiManager.SoftApCallback mSoftApManagerCallback;

    /**
     * Set up the test environment.
     */
    @Before
    public void setUp() throws Exception {
        Log.d(TAG, "Setting up ...");

        MockitoAnnotations.initMocks(this);
        mLooper = new TestLooper();

        when(mWifiInjector.getWifiStaStateNotifier()).thenReturn(mWifiStaStateNotifier);
        when(mWifiInjector.getHalDeviceManager()).thenReturn(mHalDeviceManager);
        when(mHalDeviceManager.isConcurrentStaPlusApSupported()).thenReturn(false);

        mActiveModeWardenForDeferRequest = createActiveModeWardenForDeferRequest();
        mLooper.dispatchAll();
    }

    private ActiveModeWardenForDeferRequest createActiveModeWardenForDeferRequest() {
        return new ActiveModeWardenForDeferRequest(mWifiInjector,
                                                   mContext,
                                                   mLooper.getLooper(),
                                                   mWifiNative,
                                                   mDefaultModeManager,
                                                   mBatteryStats);
    }

    /**
     * Clean up after tests - explicitly set tested object to null.
     */
    @After
    public void cleanUp() throws Exception {
        mActiveModeWardenForDeferRequest = null;
    }

    /**
     * Test that callback onStaToBeOff() is not called without registered defer event
     * when disabling wifi
     */
    @Test
    public void testNoCallbackWithoutRegisteredDeferEventWhenDisableWifi() throws Exception {
        mActiveModeWardenForDeferRequest.disableWifi();
        assertTrue(mActiveModeWardenForDeferRequest.getDeferredMsgInQueue().isEmpty());
        mLooper.dispatchAll();

        verify(mWifiStaStateNotifier, never()).onStaToBeOff();
    }

    /**
     * Test that callback onStaToBeOff() is called with registered defer event when disabling wifi
     */
    @Test
    public void testCallbackWithRegisteredDeferEventWhenDisableWifi() throws Exception {
        mActiveModeWardenForDeferRequest.registerStaEventCallback();
        mActiveModeWardenForDeferRequest.disableWifi();
        ArrayList<Integer> deferredMsgWhatInQueue =
                transferMessageToWhat(mActiveModeWardenForDeferRequest.getDeferredMsgInQueue());
        assertTrue(deferredMsgWhatInQueue
                .contains(ActiveModeWardenForDeferRequest.CMD_GO_WIFI_OFF));
        mLooper.dispatchAll();

        verify(mWifiStaStateNotifier).onStaToBeOff();
    }

    /**
     * Test that Wifi continues its shutdown after being notified defer event GO
     */
    @Test
    public void testRegisteredDeferEventContinueWhenDisableWifi() throws Exception {
        mActiveModeWardenForDeferRequest.registerStaEventCallback();
        mActiveModeWardenForDeferRequest.disableWifi();
        mLooper.dispatchAll();

        mActiveModeWardenForDeferRequest.notifyDeferEventGo();
        assertEquals(mLooper.nextMessage().what,
                ActiveModeWardenForDeferRequest.CMD_NOTIFY_GO);
        mLooper.dispatchAll();
    }

    /**
     * Test that Wifi will shut down anyway if not received defer event GO
     * after 3 seconds timeout
     */
    @Test
    public void testRegisteredDeferEventTimeoutWhenDisableWifi() throws Exception {
        mActiveModeWardenForDeferRequest.registerStaEventCallback();
        mActiveModeWardenForDeferRequest.disableWifi();
        mLooper.dispatchAll();

        SystemClock.sleep(ActiveModeWardenForDeferRequest.TIMEOUT);
        assertEquals(mLooper.nextMessage().what,
                ActiveModeWardenForDeferRequest.CMD_DEFER_OFF_TIMEOUT);
        mLooper.dispatchAll();
    }

    /**
     * Test that callback onStaToBeOff() is not called without registered defer event
     * when entering scan only mode
     */
    @Test
    public void testNoCallbackWithoutRegisteredDeferEventWhenEnterScanOnlyMode() throws Exception {
        doAnswer(
                new Answer<Object>() {
                        public ScanOnlyModeManager answer(InvocationOnMock invocation) {
                            Object[] args = invocation.getArguments();
                            mScanOnlyListener = (ScanOnlyModeManager.Listener) args[0];
                            return mScanOnlyModeManager;
                        }
                }).when(mWifiInjector).makeScanOnlyModeManager(
                        any(ScanOnlyModeManager.Listener.class));
        mActiveModeWardenForDeferRequest.enterScanOnlyMode();
        assertTrue(mActiveModeWardenForDeferRequest.getDeferredMsgInQueue().isEmpty());
        mLooper.dispatchAll();

        verify(mWifiStaStateNotifier, never()).onStaToBeOff();
    }

    /**
     * Test that callback onStaToBeOff() is called with registered defer event
     * when entering scan only mode
     */
    @Test
    public void testCallbackWithRegisteredDeferEventWhenEnterScanOnlyMode() throws Exception {
        mActiveModeWardenForDeferRequest.registerStaEventCallback();
        mActiveModeWardenForDeferRequest.enterScanOnlyMode();
        ArrayList<Integer> deferredMsgWhatInQueue =
                transferMessageToWhat(mActiveModeWardenForDeferRequest.getDeferredMsgInQueue());
        assertTrue(deferredMsgWhatInQueue
                .contains(ActiveModeWardenForDeferRequest.CMD_GO_SCAN_MODE));
        mLooper.dispatchAll();

        verify(mWifiStaStateNotifier).onStaToBeOff();
    }

    /**
     * Test that Wifi continues its shutdown after being notified defer event GO
     */
    @Test
    public void testRegisteredDeferEventContinueWhenEnterScanOnlyMode() throws Exception {
        mActiveModeWardenForDeferRequest.registerStaEventCallback();
        mActiveModeWardenForDeferRequest.enterScanOnlyMode();
        mLooper.dispatchAll();

        mActiveModeWardenForDeferRequest.notifyDeferEventGo();
        assertEquals(mLooper.nextMessage().what,
                ActiveModeWardenForDeferRequest.CMD_NOTIFY_GO);
        mLooper.dispatchAll();
    }

    /**
     * Test that Wifi will shut down anyway if not received defer event GO
     * after 3 seconds timeout
     */
    @Test
    public void testRegisteredDeferEventTimeoutWhenEnterScanOnlyMode() throws Exception {
        mActiveModeWardenForDeferRequest.registerStaEventCallback();
        mActiveModeWardenForDeferRequest.enterScanOnlyMode();
        mLooper.dispatchAll();

        SystemClock.sleep(ActiveModeWardenForDeferRequest.TIMEOUT);
        assertEquals(mLooper.nextMessage().what,
                ActiveModeWardenForDeferRequest.CMD_DEFER_OFF_TIMEOUT);
        mLooper.dispatchAll();
    }

    /**
     * Test that callback onStaToBeOff() is not called without registered defer event
     * when entering softAP mode
     */
    @Test
    public void testNoCallbackWithoutRegisteredDeferEventWhenEnterSoftAPMode() throws Exception {
        SoftApModeConfiguration softApConfig =
                new SoftApModeConfiguration(WifiManager.IFACE_IP_MODE_TETHERED, null);
        doAnswer(
                new Answer<Object>() {
                    public SoftApManager answer(InvocationOnMock invocation) {
                        Object[] args = invocation.getArguments();
                        mSoftApManagerCallback = (WifiManager.SoftApCallback) args[0];
                        assertEquals(softApConfig, (SoftApModeConfiguration) args[1]);
                        return mSoftApManager;
                    }
                }).when(mWifiInjector).makeSoftApManager(any(WifiManager.SoftApCallback.class),
                                                         any());
        mActiveModeWardenForDeferRequest.enterSoftAPMode(softApConfig);
        assertTrue(mActiveModeWardenForDeferRequest.getDeferredMsgInQueue().isEmpty());
        mLooper.dispatchAll();

        verify(mWifiStaStateNotifier, never()).onStaToBeOff();
    }

    /**
     * Test that callback onStaToBeOff() is called with registered defer event
     * when entering softAP mode
     */
    @Test
    public void testCallbackWithRegisteredDeferEventWhenEnterSoftAPMode() throws Exception {
        mActiveModeWardenForDeferRequest.registerStaEventCallback();
        mActiveModeWardenForDeferRequest.enterSoftAPMode(
                new SoftApModeConfiguration(WifiManager.IFACE_IP_MODE_TETHERED, null));
        ArrayList<Integer> deferredMsgWhatInQueue =
                transferMessageToWhat(mActiveModeWardenForDeferRequest.getDeferredMsgInQueue());
        assertTrue(deferredMsgWhatInQueue
                .contains(ActiveModeWardenForDeferRequest.CMD_GO_SOFT_AP));
        mLooper.dispatchAll();

        verify(mWifiStaStateNotifier).onStaToBeOff();
    }

    /**
     * Test that callback onStaToBeOff() is not called with registered defer event
     * when entering softAP mode under STA+AP concurrent mode
     */
    @Test
    public void testNoCallbackWithRegisteredDeferEventWhenEnterSoftAPModeUnderConcurrentMode()
            throws Exception {
        SoftApModeConfiguration softApConfig =
                new SoftApModeConfiguration(WifiManager.IFACE_IP_MODE_TETHERED, null);
        doAnswer(
                new Answer<Object>() {
                    public SoftApManager answer(InvocationOnMock invocation) {
                        Object[] args = invocation.getArguments();
                        mSoftApManagerCallback = (WifiManager.SoftApCallback) args[0];
                        assertEquals(softApConfig, (SoftApModeConfiguration) args[1]);
                        return mSoftApManager;
                    }
                }).when(mWifiInjector).makeSoftApManager(any(WifiManager.SoftApCallback.class),
                                                         any());
        when(mHalDeviceManager.isConcurrentStaPlusApSupported()).thenReturn(true);
        mActiveModeWardenForDeferRequest.registerStaEventCallback();
        mActiveModeWardenForDeferRequest.enterSoftAPMode(softApConfig);
        assertTrue(mActiveModeWardenForDeferRequest.getDeferredMsgInQueue().isEmpty());
        mLooper.dispatchAll();

        verify(mWifiStaStateNotifier, never()).onStaToBeOff();
    }

    /**
     * Test that Wifi continues its shutdown after being notified defer event GO
     */
    @Test
    public void testRegisteredDeferEventContinueWhenEnterSoftAPMode() throws Exception {
        mActiveModeWardenForDeferRequest.registerStaEventCallback();
        mActiveModeWardenForDeferRequest.enterSoftAPMode(
                new SoftApModeConfiguration(WifiManager.IFACE_IP_MODE_TETHERED, null));
        mLooper.dispatchAll();

        mActiveModeWardenForDeferRequest.notifyDeferEventGo();
        assertEquals(mLooper.nextMessage().what,
                ActiveModeWardenForDeferRequest.CMD_NOTIFY_GO);
        mLooper.dispatchAll();
    }

    /**
     * Test that Wifi will shut down anyway if not received defer event GO
     * after 3 seconds timeout
     */
    @Test
    public void testRegisteredDeferEventTimeoutWhenEnterSoftAPMode() throws Exception {
        mActiveModeWardenForDeferRequest.registerStaEventCallback();
        mActiveModeWardenForDeferRequest.enterSoftAPMode(
                new SoftApModeConfiguration(WifiManager.IFACE_IP_MODE_TETHERED, null));
        mLooper.dispatchAll();

        SystemClock.sleep(ActiveModeWardenForDeferRequest.TIMEOUT);
        assertEquals(mLooper.nextMessage().what,
                ActiveModeWardenForDeferRequest.CMD_DEFER_OFF_TIMEOUT);
        mLooper.dispatchAll();
    }

    /**
     * Test that callback onStaToBeOff() is not called without registered defer event
     * when shuting down Wifi
     */
    @Test
    public void testNoCallbackWithoutRegisteredDeferEventWhenShutdownWifi() throws Exception {
        mActiveModeWardenForDeferRequest.shutdownWifi();
        assertTrue(mActiveModeWardenForDeferRequest.getDeferredMsgInQueue().isEmpty());
        mLooper.dispatchAll();

        verify(mWifiStaStateNotifier, never()).onStaToBeOff();
    }

    /**
     * Test that callback onStaToBeOff() is called with registered defer event
     * when shuting down Wifi
     */
    @Test
    public void testCallbackWithRegisteredDeferEventWhenShutdownWifi() throws Exception {
        mActiveModeWardenForDeferRequest.registerStaEventCallback();
        mActiveModeWardenForDeferRequest.shutdownWifi();
        ArrayList<Integer> deferredMsgWhatInQueue =
                transferMessageToWhat(mActiveModeWardenForDeferRequest.getDeferredMsgInQueue());
        assertTrue(deferredMsgWhatInQueue
                .contains(ActiveModeWardenForDeferRequest.CMD_GO_SHUT_DOWN));
        mLooper.dispatchAll();

        verify(mWifiStaStateNotifier).onStaToBeOff();
    }

    /**
     * Test that Wifi continues its shutdown after being notified defer event GO
     */
    @Test
    public void testRegisteredDeferEventContinueWhenShutdownWifi() throws Exception {
        mActiveModeWardenForDeferRequest.registerStaEventCallback();
        mActiveModeWardenForDeferRequest.shutdownWifi();
        mLooper.dispatchAll();

        mActiveModeWardenForDeferRequest.notifyDeferEventGo();
        assertEquals(mLooper.nextMessage().what,
                ActiveModeWardenForDeferRequest.CMD_NOTIFY_GO);
        mLooper.dispatchAll();
    }

    /**
     * Test that Wifi will shut down anyway if not received defer event GO
     * after 3 seconds timeout
     */
    @Test
    public void testRegisteredDeferEventTimeoutWhenShutdownWifi() throws Exception {
        mActiveModeWardenForDeferRequest.registerStaEventCallback();
        mActiveModeWardenForDeferRequest.shutdownWifi();
        mLooper.dispatchAll();

        SystemClock.sleep(ActiveModeWardenForDeferRequest.TIMEOUT);
        assertEquals(mLooper.nextMessage().what,
                ActiveModeWardenForDeferRequest.CMD_DEFER_OFF_TIMEOUT);
        mLooper.dispatchAll();
    }

    /**
     * Test that enter client mode event is queue if waiting for deferred event GO
     */
    @Test
    public void testEnterClientModeAddedInQueueWhenDisableWifi() throws Exception {
        mActiveModeWardenForDeferRequest.registerStaEventCallback();
        mActiveModeWardenForDeferRequest.disableWifi();
        mActiveModeWardenForDeferRequest.enterClientMode();
        mLooper.dispatchAll();
        ArrayList<Integer> deferredMsgWhatInQueue =
                transferMessageToWhat(mActiveModeWardenForDeferRequest.getDeferredMsgInQueue());
        assertTrue(deferredMsgWhatInQueue
                .contains(ActiveModeWardenForDeferRequest.CMD_GO_WIFI_OFF));
        assertTrue(deferredMsgWhatInQueue
                .contains(ActiveModeWardenForDeferRequest.CMD_DEFER_WIFI_ON));
    }

    private ArrayList<Integer> transferMessageToWhat(ArrayList<Message> messageList) {
        ArrayList<Integer> integerList = new ArrayList<Integer>(messageList.size());
        for (int i = 0; i < messageList.size(); i++) {
            integerList.add(messageList.get(i).what);
        }
        return integerList;
    }
}
