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

package com.android.server.ethernet;

import android.content.Context;
import android.content.pm.PackageManager;
import android.net.IEthernetManager;
import android.net.IEthernetServiceListener;
import android.net.IpConfiguration;
import android.os.Binder;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.RemoteException;
import android.util.Log;
import android.util.PrintWriterPrinter;

import com.android.internal.util.IndentingPrintWriter;

import java.io.FileDescriptor;
import java.io.PrintWriter;
import java.util.concurrent.atomic.AtomicBoolean;
//yuntian longyao add Ethernet settings
import android.net.IpConfiguration.IpAssignment;
import android.net.IpConfiguration.ProxySettings;
import android.database.ContentObserver;
import android.net.Uri;
import android.provider.Settings.Global;
import android.net.StaticIpConfiguration;
import android.net.LinkAddress;
import java.net.InetAddress;
import android.provider.Settings;
//yuntian longyao end Ethernet settings

/**
 * EthernetServiceImpl handles remote Ethernet operation requests by implementing
 * the IEthernetManager interface.
 */
public class EthernetServiceImpl extends IEthernetManager.Stub {
    private static final String TAG = "EthernetServiceImpl";

    //yuntian longyao add Ethernet settings
    public static final String ETHERNET_ON = Settings.Global.ETHERNET_ON;
    public static final String ETHERNET_USE_STATIC_IP = Settings.Global.ETHERNET_STATIC_ON;
    private final EthernetOpenedObserver mOpenObserver = new EthernetOpenedObserver();
    private final EthernetStaticObserver mStaticObserver = new EthernetStaticObserver();
    private IpConfiguration mIpConfiguration;
    //yuntian longyao end Ethernet settings
    private final Context mContext;
    private final AtomicBoolean mStarted = new AtomicBoolean(false);

    private Handler mHandler;
    private EthernetTracker mTracker;

    public EthernetServiceImpl(Context context) {
        mContext = context;
        //yuntian longyao add Ethernet settings
        mContext.getContentResolver().registerContentObserver(
                Global.getUriFor(ETHERNET_ON), false, mOpenObserver);
        mContext.getContentResolver().registerContentObserver(
                Global.getUriFor(ETHERNET_USE_STATIC_IP), false, mStaticObserver);
        //yuntian longyao end Ethernet settings
    }

    private void enforceAccessPermission() {
        mContext.enforceCallingOrSelfPermission(
                android.Manifest.permission.ACCESS_NETWORK_STATE,
                "EthernetService");
    }

    private void enforceConnectivityInternalPermission() {
        mContext.enforceCallingOrSelfPermission(
                android.Manifest.permission.CONNECTIVITY_INTERNAL,
                "ConnectivityService");
    }

    private void enforceUseRestrictedNetworksPermission() {
        mContext.enforceCallingOrSelfPermission(
                android.Manifest.permission.CONNECTIVITY_USE_RESTRICTED_NETWORKS,
                "ConnectivityService");
    }

    private boolean checkUseRestrictedNetworksPermission() {
        return mContext.checkCallingOrSelfPermission(
                android.Manifest.permission.CONNECTIVITY_USE_RESTRICTED_NETWORKS)
                == PackageManager.PERMISSION_GRANTED;
    }

    public void start() {
        Log.i(TAG, "Starting Ethernet service");

        HandlerThread handlerThread = new HandlerThread("EthernetServiceThread");
        handlerThread.start();
        mHandler = new Handler(handlerThread.getLooper());

        mTracker = new EthernetTracker(mContext, mHandler);
        //yuntian longyao add Ethernet settings
        mIpConfiguration = mTracker.getDefaultIpConfiguration();
        if (getState() == 1) {
            if (isStatic()) {
                StaticIpConfiguration staticIpConfiguration = new StaticIpConfiguration();
                staticIpConfiguration.domains = Settings.Global.getString(mContext.getContentResolver(), Settings.Global.ETHERNET_STATIC_MASK);
                try {
                    staticIpConfiguration.gateway = InetAddress.getByName(Settings.Global.getString(mContext.getContentResolver(), Settings.Global.ETHERNET_STATIC_GATEWAY));
                    staticIpConfiguration.ipAddress = new LinkAddress(InetAddress.getByName(Settings.Global.getString(mContext.getContentResolver(), Settings.Global.ETHERNET_STATIC_IP)), 24);
                    staticIpConfiguration.dnsServers.add(InetAddress.getByName(Settings.Global.getString(mContext.getContentResolver(), Settings.Global.ETHERNET_STATIC_DNS1)));
                    staticIpConfiguration.dnsServers.add(InetAddress.getByName(Settings.Global.getString(mContext.getContentResolver(), Settings.Global.ETHERNET_STATIC_DNS2)));
                } catch (Exception e) {
                    e.printStackTrace();
                }
                mIpConfiguration.ipAssignment = IpAssignment.STATIC;
                mIpConfiguration.proxySettings = ProxySettings.STATIC;
                mIpConfiguration.staticIpConfiguration = staticIpConfiguration;
            }
            mTracker.start();

            mStarted.set(true);
        }
        //yuntian longyao end Ethernet settings
    }
    //yuntian longyao add Ethernet settings
    private boolean isStatic() {
        Log.e(TAG, "EthernetServiceImpl isStatic()  "
                + Settings.Global.getInt(mContext.getContentResolver(), ETHERNET_USE_STATIC_IP, 0));
        return Settings.Global.getInt(mContext.getContentResolver(), ETHERNET_USE_STATIC_IP, 0) == 1;
    }


    private int getState() {
        int state = Settings.Global.getInt(mContext.getContentResolver(), ETHERNET_ON, 0);
        Log.e(TAG, "EthernetServiceImpl getState()  " + state);
        return state;

    }
    //yuntian longyao end Ethernet settings
    @Override
    public String[] getAvailableInterfaces() throws RemoteException {
        return mTracker.getInterfaces(checkUseRestrictedNetworksPermission());
    }

    /**
     * Get Ethernet configuration
     * @return the Ethernet Configuration, contained in {@link IpConfiguration}.
     */
    @Override
    public IpConfiguration getConfiguration(String iface) {
        enforceAccessPermission();

        if (mTracker.isRestrictedInterface(iface)) {
            enforceUseRestrictedNetworksPermission();
        }

        return new IpConfiguration(mTracker.getIpConfiguration(iface));
    }

    /**
     * Set Ethernet configuration
     */
    @Override
    public void setConfiguration(String iface, IpConfiguration config) {
        if (!mStarted.get()) {
            Log.w(TAG, "System isn't ready enough to change ethernet configuration");
        }

        enforceConnectivityInternalPermission();

        if (mTracker.isRestrictedInterface(iface)) {
            enforceUseRestrictedNetworksPermission();
        }

        // TODO: this does not check proxy settings, gateways, etc.
        // Fix this by making IpConfiguration a complete representation of static configuration.
        mTracker.updateIpConfiguration(iface, new IpConfiguration(config));
        //yuntian longyao add Ethernet settings
        mTracker.removeInterface(iface);
        mTracker.start();
        //yuntian longyao end Ethernet settings
    }

    /**
     * Indicates whether given interface is available.
     */
    @Override
    public boolean isAvailable(String iface) {
        enforceAccessPermission();

        if (mTracker.isRestrictedInterface(iface)) {
            enforceUseRestrictedNetworksPermission();
        }

        return mTracker.isTrackingInterface(iface);
    }

    /**
     * Adds a listener.
     * @param listener A {@link IEthernetServiceListener} to add.
     */
    public void addListener(IEthernetServiceListener listener) {
        if (listener == null) {
            throw new IllegalArgumentException("listener must not be null");
        }
        enforceAccessPermission();
        mTracker.addListener(listener, checkUseRestrictedNetworksPermission());
    }

    /**
     * Removes a listener.
     * @param listener A {@link IEthernetServiceListener} to remove.
     */
    public void removeListener(IEthernetServiceListener listener) {
        if (listener == null) {
            throw new IllegalArgumentException("listener must not be null");
        }
        enforceAccessPermission();
        mTracker.removeListener(listener);
    }

    @Override
    protected void dump(FileDescriptor fd, PrintWriter writer, String[] args) {
        final IndentingPrintWriter pw = new IndentingPrintWriter(writer, "  ");
        if (mContext.checkCallingOrSelfPermission(android.Manifest.permission.DUMP)
                != PackageManager.PERMISSION_GRANTED) {
            pw.println("Permission Denial: can't dump EthernetService from pid="
                    + Binder.getCallingPid()
                    + ", uid=" + Binder.getCallingUid());
            return;
        }

        pw.println("Current Ethernet state: ");
        pw.increaseIndent();
        mTracker.dump(fd, pw, args);
        pw.decreaseIndent();

        pw.println("Handler:");
        pw.increaseIndent();
        mHandler.dump(new PrintWriterPrinter(pw), "EthernetServiceImpl");
        pw.decreaseIndent();
    }

    //yuntian longyao add Ethernet settings
    private final class EthernetOpenedObserver extends ContentObserver {
        public EthernetOpenedObserver() {
            super(new Handler());
        }

        @Override
        public void onChange(boolean selfChange, Uri uri, int userId) {
            super.onChange(selfChange, uri, userId);
            Log.i(TAG, "EthernetServiceImpl isEthernetOpen onChange....");
            if (getState() == 1) {
                if (isStatic()) {
                    StaticIpConfiguration staticIpConfiguration = new StaticIpConfiguration();
                    staticIpConfiguration.domains = Settings.Global.getString(mContext.getContentResolver(), Settings.Global.ETHERNET_STATIC_MASK);

                    try {
                        staticIpConfiguration.gateway = InetAddress.getByName(Settings.Global.getString(mContext.getContentResolver(), Settings.Global.ETHERNET_STATIC_GATEWAY));
                        staticIpConfiguration.ipAddress = new LinkAddress(InetAddress.getByName(Settings.Global.getString(mContext.getContentResolver(), Settings.Global.ETHERNET_STATIC_IP)), 24);
                        staticIpConfiguration.dnsServers.add(InetAddress.getByName(Settings.Global.getString(mContext.getContentResolver(), Settings.Global.ETHERNET_STATIC_DNS1)));
                        staticIpConfiguration.dnsServers.add(InetAddress.getByName(Settings.Global.getString(mContext.getContentResolver(), Settings.Global.ETHERNET_STATIC_DNS2)));

                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    mIpConfiguration.ipAssignment = IpAssignment.STATIC;
                    mIpConfiguration.proxySettings = ProxySettings.STATIC;
                    mIpConfiguration.staticIpConfiguration = staticIpConfiguration;

                }
                mStarted.set(true);
                mTracker.start();
            } else {
                mTracker.stop();
            }
        }
    }

    private final class EthernetStaticObserver extends ContentObserver {
        public EthernetStaticObserver() {
            super(new Handler());
        }

        @Override
        public void onChange(boolean selfChange, Uri uri, int userId) {
            super.onChange(selfChange, uri, userId);
            Log.i(TAG, "EthernetServiceImpl isEthernetStaticOpen onChange....");
            if (!isStatic()) {
                mTracker.recoverDHCPIpConfiguration();
                mTracker.stop();
                mStarted.set(true);
                mTracker.start();
            }
        }
    }
    //yuntian longyao end Ethernet settings
}
