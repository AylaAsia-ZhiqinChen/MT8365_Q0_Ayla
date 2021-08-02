/*
 * Copyright (C) 2017 The Android Open Source Project
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

package com.android.settingslib.deviceinfo;

import android.annotation.SuppressLint;
import android.content.Context;
import android.net.ConnectivityManager;
import android.text.TextUtils;

import androidx.annotation.VisibleForTesting;
import androidx.preference.Preference;
import androidx.preference.PreferenceScreen;

import com.android.settingslib.R;
import com.android.settingslib.core.lifecycle.Lifecycle;
import java.net.NetworkInterface;

/**
 * Preference controller for Ethernet MAC address
 */
public abstract class AbstractEthernetMacAddressPreferenceController
        extends AbstractConnectivityPreferenceController {

    @VisibleForTesting
    static final String KEY_ETH_MAC_ADDRESS = "eth_mac_address";
    @VisibleForTesting
    static final int OFF = 0;
    @VisibleForTesting
    static final int ON = 1;

    private static final String[] CONNECTIVITY_INTENTS = {
            ConnectivityManager.CONNECTIVITY_ACTION,
    };

    private Preference mEthernetMacAddress;

    public AbstractEthernetMacAddressPreferenceController(Context context, Lifecycle lifecycle) {
        super(context, lifecycle);
    }

    @Override
    public boolean isAvailable() {
        return true;
    }

    @Override
    public String getPreferenceKey() {
        return KEY_ETH_MAC_ADDRESS;
    }

    @Override
    public void displayPreference(PreferenceScreen screen) {
        super.displayPreference(screen);
        mEthernetMacAddress = screen.findPreference(KEY_ETH_MAC_ADDRESS);
        updateConnectivity();
    }

    @Override
    protected String[] getConnectivityIntents() {
        return CONNECTIVITY_INTENTS;
    }

    @SuppressLint("HardwareIds")
    @Override
    protected void updateConnectivity() {
        String macAddress = getEthernetMac();
        if (TextUtils.isEmpty(macAddress)) {
            mEthernetMacAddress.setSummary(R.string.status_unavailable);
        } else {
            mEthernetMacAddress.setSummary(macAddress);
        }
    }

    public String getEthernetMac() {
        String ethernetMac = null;
        try {
            NetworkInterface nic = NetworkInterface.getByName("eth0");
            byte[] buf = null;
            if (null != nic) {
                buf = nic.getHardwareAddress();
            }
            if (null != buf) {
                ethernetMac = byteHexString(buf);
            }
        } catch (java.net.SocketException e) {
            e.printStackTrace();
        }
        return ethernetMac;
    }

    /*
     * 字节数组转16进制字符串
     */
    public String byteHexString(byte[] array) {
        StringBuilder builder = new StringBuilder();
        int i = array.length;
        for (byte b : array) {
            String hex = Integer.toHexString(b & 0xFF);
            if (hex.length() == 1) {
                hex = '0' + hex;
            }
            if (i-- != 1) {
                hex = hex + ":";
            }
            builder.append(hex);
        }
        return builder.toString();
    }
}
