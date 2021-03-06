/*
 * Copyright (C) 2018 The Android Open Source Project
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
 * limitations under the License
 */

package com.android.server.telecom.tests;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothHeadset;
import android.bluetooth.BluetoothHearingAid;
import android.content.ContentResolver;
import android.telecom.Log;
import android.test.suitebuilder.annotation.SmallTest;

import com.android.internal.os.SomeArgs;
import com.android.server.telecom.BluetoothHeadsetProxy;
import com.android.server.telecom.TelecomSystem;
import com.android.server.telecom.Timeouts;
import com.android.server.telecom.bluetooth.BluetoothDeviceManager;
import com.android.server.telecom.bluetooth.BluetoothRouteManager;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.mockito.Mock;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.List;

import static com.android.server.telecom.tests.BluetoothRouteManagerTest.DEVICE1;
import static com.android.server.telecom.tests.BluetoothRouteManagerTest.DEVICE2;
import static com.android.server.telecom.tests.BluetoothRouteManagerTest.DEVICE3;
import static com.android.server.telecom.tests.BluetoothRouteManagerTest.executeRoutingAction;
import static org.junit.Assert.assertEquals;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.ArgumentMatchers.nullable;
import static org.mockito.Mockito.clearInvocations;
import static org.mockito.Mockito.doAnswer;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@RunWith(Parameterized.class)
public class BluetoothRouteTransitionTests extends TelecomTestCase {
    private enum ListenerUpdate {
        DEVICE_LIST_CHANGED, ACTIVE_DEVICE_PRESENT, ACTIVE_DEVICE_GONE,
        AUDIO_CONNECTED, AUDIO_DISCONNECTED, UNEXPECTED_STATE_CHANGE
    }

    private static class BluetoothRouteTestParametersBuilder {
        private String name;
        private String initialBluetoothState;
        private BluetoothDevice initialDevice;
        private BluetoothDevice audioOnDevice;
        private int messageType;
        private BluetoothDevice messageDevice;
        private ListenerUpdate[] expectedListenerUpdates;
        private int expectedBluetoothInteraction;
        private BluetoothDevice expectedConnectionDevice;
        private String expectedFinalStateName;
        private BluetoothDevice[] connectedDevices;
        // the active device as returned by BluetoothHeadset#getActiveDevice
        private BluetoothDevice activeDevice = null;
        private List<BluetoothDevice> hearingAidBtDevices = Collections.emptyList();

        public BluetoothRouteTestParametersBuilder setName(String name) {
            this.name = name;
            return this;
        }

        public BluetoothRouteTestParametersBuilder setInitialBluetoothState(
                String initialBluetoothState) {
            this.initialBluetoothState = initialBluetoothState;
            return this;
        }

        public BluetoothRouteTestParametersBuilder setInitialDevice(BluetoothDevice
                initialDevice) {
            this.initialDevice = initialDevice;
            return this;
        }

        public BluetoothRouteTestParametersBuilder setMessageType(int messageType) {
            this.messageType = messageType;
            return this;
        }

        public BluetoothRouteTestParametersBuilder setMessageDevice(BluetoothDevice messageDevice) {
            this.messageDevice = messageDevice;
            return this;
        }

        public BluetoothRouteTestParametersBuilder setExpectedListenerUpdates(
                ListenerUpdate... expectedListenerUpdates) {
            this.expectedListenerUpdates = expectedListenerUpdates;
            return this;
        }

        public BluetoothRouteTestParametersBuilder setExpectedBluetoothInteraction(
                int expectedBluetoothInteraction) {
            this.expectedBluetoothInteraction = expectedBluetoothInteraction;
            return this;
        }

        public BluetoothRouteTestParametersBuilder setExpectedConnectionDevice(
                BluetoothDevice expectedConnectionDevice) {
            this.expectedConnectionDevice = expectedConnectionDevice;
            return this;
        }

        public BluetoothRouteTestParametersBuilder setExpectedFinalStateName(
                String expectedFinalStateName) {
            this.expectedFinalStateName = expectedFinalStateName;
            return this;
        }

        public BluetoothRouteTestParametersBuilder setConnectedDevices(
                BluetoothDevice... connectedDevices) {
            this.connectedDevices = connectedDevices;
            return this;
        }

        public BluetoothRouteTestParametersBuilder setAudioOnDevice(BluetoothDevice device) {
            this.audioOnDevice = device;
            return this;
        }

        public BluetoothRouteTestParametersBuilder setActiveDevice(BluetoothDevice device) {
            this.activeDevice = device;
            return this;
        }

        public BluetoothRouteTestParametersBuilder setHearingAidBtDevices(
                List<BluetoothDevice> hearingAidBtDevices) {
            this.hearingAidBtDevices = hearingAidBtDevices;
            return this;
        }

        public BluetoothRouteTestParameters build() {
            return new BluetoothRouteTestParameters(name,
                    initialBluetoothState,
                    initialDevice,
                    messageType,
                    expectedListenerUpdates,
                    expectedBluetoothInteraction,
                    expectedConnectionDevice,
                    expectedFinalStateName,
                    connectedDevices,
                    messageDevice,
                    audioOnDevice,
                    activeDevice,
                    hearingAidBtDevices);

        }
    }

    private static class BluetoothRouteTestParameters {
        public String name;
        public String initialBluetoothState; // One of the state names or prefixes from BRM.
        public BluetoothDevice initialDevice; // null if we start from AudioOff
        public BluetoothDevice audioOnDevice; // The device (if any) that is active
        public int messageType; // Any of the commands from the state machine
        public BluetoothDevice messageDevice; // The device that should be specified in the message.
        public ListenerUpdate[] expectedListenerUpdates; // what the listener should expect.
        public int expectedBluetoothInteraction; // NONE, CONNECT, or DISCONNECT
        public BluetoothDevice expectedConnectionDevice; // Expected device to connect to.
        public String expectedFinalStateName; // Expected name of the final state.
        public BluetoothDevice[] connectedDevices; // array of connected devices
        // the active device as returned by BluetoothHeadset#getActiveDevice
        private BluetoothDevice activeDevice = null;
        private List<BluetoothDevice> hearingAidBtDevices;

        public BluetoothRouteTestParameters(String name, String initialBluetoothState,
                BluetoothDevice initialDevice, int messageType, ListenerUpdate[]
                expectedListenerUpdates, int expectedBluetoothInteraction, BluetoothDevice
                expectedConnectionDevice, String expectedFinalStateName,
                BluetoothDevice[] connectedDevices, BluetoothDevice messageDevice,
                BluetoothDevice audioOnDevice, BluetoothDevice activeDevice,
                List<BluetoothDevice> hearingAidBtDevices) {
            this.name = name;
            this.initialBluetoothState = initialBluetoothState;
            this.initialDevice = initialDevice;
            this.messageType = messageType;
            this.expectedListenerUpdates = expectedListenerUpdates;
            this.expectedBluetoothInteraction = expectedBluetoothInteraction;
            this.expectedConnectionDevice = expectedConnectionDevice;
            this.expectedFinalStateName = expectedFinalStateName;
            this.connectedDevices = connectedDevices;
            this.messageDevice = messageDevice;
            this.audioOnDevice = audioOnDevice;
            this.activeDevice = activeDevice;
            this.hearingAidBtDevices = hearingAidBtDevices;
        }

        @Override
        public String toString() {
            return "BluetoothRouteTestParameters{" +
                    "name='" + name + '\'' +
                    ", initialBluetoothState='" + initialBluetoothState + '\'' +
                    ", initialDevice=" + initialDevice +
                    ", messageType=" + messageType +
                    ", messageDevice='" + messageDevice + '\'' +
                    ", expectedListenerUpdate=" + expectedListenerUpdates +
                    ", expectedBluetoothInteraction=" + expectedBluetoothInteraction +
                    ", expectedConnectionDevice='" + expectedConnectionDevice + '\'' +
                    ", expectedFinalStateName='" + expectedFinalStateName + '\'' +
                    ", connectedDevices=" + Arrays.toString(connectedDevices) +
                    ", activeDevice='" + activeDevice + '\'' +
                    ", hearingAidBtDevices ='" + hearingAidBtDevices + '\'' +
                    '}';
        }
    }

    private static final int NONE = 1;
    private static final int CONNECT = 2;
    private static final int DISCONNECT = 3;

    private static final int TEST_TIMEOUT = 1000;

    private final BluetoothRouteTestParameters mParams;
    @Mock private BluetoothDeviceManager mDeviceManager;
    @Mock private BluetoothHeadsetProxy mHeadsetProxy;
    @Mock private BluetoothHearingAid mBluetoothHearingAid;
    @Mock private Timeouts.Adapter mTimeoutsAdapter;
    @Mock private BluetoothRouteManager.BluetoothStateListener mListener;

    @Override
    @Before
    public void setUp() throws Exception {
        super.setUp();
    }

    public BluetoothRouteTransitionTests(BluetoothRouteTestParameters params) {
        mParams = params;
    }

    @Test
    @SmallTest
    public void testTransitions() {
        BluetoothRouteManager sm = setupStateMachine(
                mParams.initialBluetoothState, mParams.initialDevice);

        setupConnectedDevices(mParams.connectedDevices,
                mParams.audioOnDevice, mParams.activeDevice);
        sm.setActiveDeviceCacheForTesting(mParams.activeDevice,
                mParams.hearingAidBtDevices.contains(mParams.messageDevice));
        if (mParams.initialDevice != null) {
            doAnswer(invocation -> {
                SomeArgs args = SomeArgs.obtain();
                args.arg1 = Log.createSubsession();
                args.arg2 = mParams.initialDevice.getAddress();
                when(mHeadsetProxy.getActiveDevice()).thenReturn(null);
                sm.sendMessage(BluetoothRouteManager.BT_AUDIO_LOST, args);
                return true;
            }).when(mDeviceManager).disconnectAudio();
        }

        // Go through the utility methods for these two messages
        if (mParams.messageType == BluetoothRouteManager.NEW_DEVICE_CONNECTED) {
            sm.onDeviceAdded(mParams.messageDevice.getAddress());
            sm.onActiveDeviceChanged(mParams.messageDevice,
                    mParams.hearingAidBtDevices.contains(mParams.messageDevice));
        } else if (mParams.messageType == BluetoothRouteManager.LOST_DEVICE) {
            sm.onActiveDeviceChanged(null,
                    mParams.hearingAidBtDevices.contains(mParams.messageDevice));
            if (mParams.hearingAidBtDevices.contains(mParams.messageDevice)) {
                when(mBluetoothHearingAid.getActiveDevices()).thenReturn(Arrays.asList(null, null));
            } else {
                when(mHeadsetProxy.getActiveDevice()).thenReturn(null);
            }
            sm.onDeviceLost(mParams.messageDevice.getAddress());
        } else {
            executeRoutingAction(sm, mParams.messageType,
                    mParams.messageDevice == null ? null : mParams.messageDevice.getAddress());
        }

        waitForHandlerAction(sm.getHandler(), TEST_TIMEOUT);
        waitForHandlerAction(sm.getHandler(), TEST_TIMEOUT);
        waitForHandlerAction(sm.getHandler(), TEST_TIMEOUT);
        assertEquals(mParams.expectedFinalStateName, sm.getCurrentState().getName());

        for (ListenerUpdate lu : mParams.expectedListenerUpdates) {
            switch (lu) {
                case DEVICE_LIST_CHANGED:
                    verify(mListener).onBluetoothDeviceListChanged();
                    break;
                case ACTIVE_DEVICE_PRESENT:
                    verify(mListener).onBluetoothActiveDevicePresent();
                    break;
                case ACTIVE_DEVICE_GONE:
                    verify(mListener).onBluetoothActiveDeviceGone();
                    break;
                case AUDIO_CONNECTED:
                    verify(mListener).onBluetoothAudioConnected();
                    break;
                case AUDIO_DISCONNECTED:
                    verify(mListener).onBluetoothAudioDisconnected();
                    break;
            }
        }

        switch (mParams.expectedBluetoothInteraction) {
            case NONE:
                verify(mDeviceManager, never()).connectAudio(nullable(String.class));
                break;
            case CONNECT:
                verify(mDeviceManager).connectAudio(mParams.expectedConnectionDevice.getAddress());
                verify(mDeviceManager, never()).disconnectAudio();
                break;
            case DISCONNECT:
                verify(mDeviceManager, never()).connectAudio(nullable(String.class));
                verify(mDeviceManager).disconnectAudio();
                break;
        }

        sm.getHandler().removeMessages(BluetoothRouteManager.CONNECTION_TIMEOUT);
        sm.quitNow();
    }

    private void setupConnectedDevices(BluetoothDevice[] devices,
            BluetoothDevice audioOnDevice, BluetoothDevice activeDevice) {
        when(mDeviceManager.getNumConnectedDevices()).thenReturn(devices.length);
        when(mDeviceManager.getConnectedDevices()).thenReturn(Arrays.asList(devices));
        when(mHeadsetProxy.getConnectedDevices()).thenReturn(Arrays.asList(devices));
        when(mHeadsetProxy.getActiveDevice()).thenReturn(activeDevice);
        if (audioOnDevice != null) {
            when(mHeadsetProxy.getActiveDevice()).thenReturn(audioOnDevice);
            when(mHeadsetProxy.getAudioState(audioOnDevice))
                    .thenReturn(BluetoothHeadset.STATE_AUDIO_CONNECTED);
        }
    }

    private BluetoothRouteManager setupStateMachine(String initialState,
            BluetoothDevice initialDevice) {
        resetMocks();
        when(mDeviceManager.getHeadsetService()).thenReturn(mHeadsetProxy);
        when(mDeviceManager.getHearingAidService()).thenReturn(mBluetoothHearingAid);
        when(mDeviceManager.connectAudio(nullable(String.class))).thenReturn(true);
        when(mTimeoutsAdapter.getRetryBluetoothConnectAudioBackoffMillis(
                nullable(ContentResolver.class))).thenReturn(100000L);
        when(mTimeoutsAdapter.getBluetoothPendingTimeoutMillis(
                nullable(ContentResolver.class))).thenReturn(100000L);
        BluetoothRouteManager sm = new BluetoothRouteManager(mContext,
                new TelecomSystem.SyncRoot() { }, mDeviceManager, mTimeoutsAdapter);
        sm.setListener(mListener);
        sm.setInitialStateForTesting(initialState, initialDevice);
        /// M: Fix BluetoothRouteManager case fail. @{
        /// Use waitForHandleAction is not safely enough, StateMachine state transition in
        /// setInitialStateForTesting still may not finish after waitForHandlerAction.
        /// Solution:
        /// Call sendMessage of stateMachine before waitForHandlerAction
        /// to ensure state transition of setInitialStateForTesting can execute done.
        sm.sendMessage(BluetoothRouteManager.RUN_RUNNABLE, (Runnable) () -> { });
        /// @}
        waitForHandlerAction(sm.getHandler(), TEST_TIMEOUT);
        resetMocks();
        return sm;
    }

    private void resetMocks() {
        clearInvocations(mDeviceManager, mListener, mHeadsetProxy, mTimeoutsAdapter);
    }

    @Parameterized.Parameters(name = "{0}")
    public static Collection<BluetoothRouteTestParameters> generateTestCases() {
        List<BluetoothRouteTestParameters> result = new ArrayList<>();
        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("New device connected while audio off")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_OFF_STATE_NAME)
                .setInitialDevice(null)
                .setConnectedDevices(DEVICE1)
                .setMessageType(BluetoothRouteManager.NEW_DEVICE_CONNECTED)
                .setMessageDevice(DEVICE1)
                .setExpectedListenerUpdates(ListenerUpdate.DEVICE_LIST_CHANGED,
                        ListenerUpdate.ACTIVE_DEVICE_PRESENT)
                .setExpectedBluetoothInteraction(NONE)
                .setExpectedConnectionDevice(null)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_OFF_STATE_NAME)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Nonspecific connection request while audio off with BT-active device")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_OFF_STATE_NAME)
                .setInitialDevice(null)
                .setConnectedDevices(DEVICE2, DEVICE1)
                .setActiveDevice(DEVICE1)
                .setMessageType(BluetoothRouteManager.CONNECT_HFP)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_CONNECTED)
                .setExpectedBluetoothInteraction(CONNECT)
                .setExpectedConnectionDevice(DEVICE1)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_CONNECTING_STATE_NAME_PREFIX
                        + ":" + DEVICE1)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Connection to a device succeeds after pending")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTING_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setAudioOnDevice(DEVICE2)
                .setConnectedDevices(DEVICE2, DEVICE1)
                .setMessageType(BluetoothRouteManager.BT_AUDIO_IS_ON)
                .setMessageDevice(DEVICE2)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_CONNECTED)
                .setExpectedBluetoothInteraction(NONE)
                .setExpectedConnectionDevice(null)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_CONNECTED_STATE_NAME_PREFIX
                        + ":" + DEVICE2)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Device loses HFP audio but remains connected. No fallback.")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTED_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setConnectedDevices(DEVICE2)
                .setMessageType(BluetoothRouteManager.BT_AUDIO_LOST)
                .setMessageDevice(DEVICE2)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_DISCONNECTED)
                .setExpectedBluetoothInteraction(NONE)
                .setExpectedConnectionDevice(null)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_OFF_STATE_NAME)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Device loses HFP audio but remains connected."
                        + " No fallback even though other devices available.")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTED_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setConnectedDevices(DEVICE2, DEVICE1, DEVICE3)
                .setMessageType(BluetoothRouteManager.BT_AUDIO_LOST)
                .setMessageDevice(DEVICE2)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_DISCONNECTED)
                .setExpectedBluetoothInteraction(NONE)
                .setExpectedConnectionDevice(null)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_OFF_STATE_NAME)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Switch the device that audio is being routed to")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTED_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setConnectedDevices(DEVICE2, DEVICE1, DEVICE3)
                .setMessageType(BluetoothRouteManager.CONNECT_HFP)
                .setMessageDevice(DEVICE3)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_CONNECTED)
                .setExpectedBluetoothInteraction(CONNECT)
                .setExpectedConnectionDevice(DEVICE3)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_CONNECTING_STATE_NAME_PREFIX
                        + ":" + DEVICE3)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Switch to another device before first device has connected")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTING_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setConnectedDevices(DEVICE2, DEVICE1, DEVICE3)
                .setMessageType(BluetoothRouteManager.CONNECT_HFP)
                .setMessageDevice(DEVICE3)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_CONNECTED)
                .setExpectedBluetoothInteraction(CONNECT)
                .setExpectedConnectionDevice(DEVICE3)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_CONNECTING_STATE_NAME_PREFIX
                        + ":" + DEVICE3)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Device gets disconnected while active. No fallback.")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTED_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setActiveDevice(DEVICE2)
                .setConnectedDevices()
                .setMessageType(BluetoothRouteManager.LOST_DEVICE)
                .setMessageDevice(DEVICE2)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_DISCONNECTED,
                        ListenerUpdate.DEVICE_LIST_CHANGED, ListenerUpdate.ACTIVE_DEVICE_GONE)
                .setExpectedBluetoothInteraction(NONE)
                .setExpectedConnectionDevice(null)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_OFF_STATE_NAME)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Device gets disconnected while active."
                        + " No fallback even though other devices available.")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTED_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setConnectedDevices(DEVICE3)
                .setMessageType(BluetoothRouteManager.LOST_DEVICE)
                .setMessageDevice(DEVICE2)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_DISCONNECTED,
                        ListenerUpdate.DEVICE_LIST_CHANGED)
                .setExpectedBluetoothInteraction(NONE)
                .setExpectedConnectionDevice(null)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_OFF_STATE_NAME)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Connection to DEVICE2 times out but device 1 still connected.")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTING_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setConnectedDevices(DEVICE2, DEVICE1)
                .setAudioOnDevice(DEVICE1)
                .setMessageType(BluetoothRouteManager.CONNECTION_TIMEOUT)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_CONNECTED)
                .setExpectedBluetoothInteraction(NONE)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_CONNECTED_STATE_NAME_PREFIX
                        + ":" + DEVICE1)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("DEVICE1 somehow becomes active when DEVICE2 is still pending.")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTING_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setConnectedDevices(DEVICE2, DEVICE1)
                .setAudioOnDevice(DEVICE1)
                .setMessageType(BluetoothRouteManager.BT_AUDIO_IS_ON)
                .setMessageDevice(DEVICE1)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_CONNECTED)
                .setExpectedBluetoothInteraction(NONE)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_CONNECTED_STATE_NAME_PREFIX
                        + ":" + DEVICE1)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Device gets disconnected while pending."
                        + " No fallback even though other devices available.")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTING_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setConnectedDevices(DEVICE3)
                .setMessageType(BluetoothRouteManager.LOST_DEVICE)
                .setMessageDevice(DEVICE2)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_DISCONNECTED,
                        ListenerUpdate.DEVICE_LIST_CHANGED)
                .setExpectedBluetoothInteraction(NONE)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_OFF_STATE_NAME)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Audio disconnect comes with a null device")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTED_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setConnectedDevices(DEVICE2)
                .setMessageType(BluetoothRouteManager.BT_AUDIO_LOST)
                .setMessageDevice(null)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_DISCONNECTED)
                .setExpectedBluetoothInteraction(NONE)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_OFF_STATE_NAME)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Device gets disconnected while pending. No fallback.")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTING_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setConnectedDevices()
                .setMessageType(BluetoothRouteManager.LOST_DEVICE)
                .setMessageDevice(DEVICE2)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_DISCONNECTED,
                        ListenerUpdate.DEVICE_LIST_CHANGED)
                .setExpectedBluetoothInteraction(NONE)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_OFF_STATE_NAME)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Device gets audio-off while in another device's audio on state")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTED_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setConnectedDevices(DEVICE2, DEVICE1)
                .setMessageType(BluetoothRouteManager.BT_AUDIO_LOST)
                .setMessageDevice(DEVICE1)
                .setExpectedListenerUpdates(ListenerUpdate.UNEXPECTED_STATE_CHANGE)
                .setExpectedBluetoothInteraction(NONE)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_CONNECTED_STATE_NAME_PREFIX
                        + ":" + DEVICE2)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Audio routing requests HFP disconnection while a device is active")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTED_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setConnectedDevices(DEVICE2, DEVICE3)
                .setMessageType(BluetoothRouteManager.DISCONNECT_HFP)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_DISCONNECTED)
                .setExpectedBluetoothInteraction(DISCONNECT)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_OFF_STATE_NAME)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Audio routing requests HFP disconnection while a device is pending")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTING_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setConnectedDevices(DEVICE2, DEVICE3)
                .setMessageType(BluetoothRouteManager.DISCONNECT_HFP)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_DISCONNECTED)
                .setExpectedBluetoothInteraction(DISCONNECT)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_OFF_STATE_NAME)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Bluetooth turns itself on.")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_OFF_STATE_NAME)
                .setInitialDevice(null)
                .setConnectedDevices(DEVICE2, DEVICE3)
                .setMessageType(BluetoothRouteManager.BT_AUDIO_IS_ON)
                .setMessageDevice(DEVICE3)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_CONNECTED)
                .setExpectedBluetoothInteraction(NONE)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_CONNECTED_STATE_NAME_PREFIX
                        + ":" + DEVICE3)
                .build());

        result.add(new BluetoothRouteTestParametersBuilder()
                .setName("Hearing aid device disconnects with headset present")
                .setInitialBluetoothState(BluetoothRouteManager.AUDIO_CONNECTED_STATE_NAME_PREFIX)
                .setInitialDevice(DEVICE2)
                .setConnectedDevices(DEVICE2, DEVICE3)
                .setHearingAidBtDevices(Collections.singletonList(DEVICE2))
                .setMessageType(BluetoothRouteManager.LOST_DEVICE)
                .setMessageDevice(DEVICE2)
                .setExpectedListenerUpdates(ListenerUpdate.AUDIO_DISCONNECTED,
                        ListenerUpdate.DEVICE_LIST_CHANGED)
                .setExpectedBluetoothInteraction(NONE)
                .setExpectedFinalStateName(BluetoothRouteManager.AUDIO_OFF_STATE_NAME)
                .build());
        return result;
    }
}
