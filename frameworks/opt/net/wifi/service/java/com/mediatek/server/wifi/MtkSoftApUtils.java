package com.mediatek.server.wifi;

import android.content.Context;
import android.hardware.wifi.hostapd.V1_0.IHostapd;
import android.net.ConnectivityManager;
import android.net.wifi.WifiManager;
import android.net.wifi.p2p.WifiP2pManager;
import android.net.wifi.p2p.WifiP2pManager.Channel;
import android.os.Looper;
import android.os.SystemProperties;
import android.util.Log;

import com.android.server.wifi.WifiNative;
import com.android.server.wifi.util.ApConfigUtil;

public class MtkSoftApUtils {
    private static final String TAG = "MtkSoftApUtils";

    public static void stopP2p(Context context, Looper looper, WifiNative wifiNative) {
        if (context == null || wifiNative == null)
            return;
        if (wifiNative.getClientInterfaceName() == null)
            return;
        WifiP2pManager wifiP2pManager =
                (WifiP2pManager) context.getSystemService(Context.WIFI_P2P_SERVICE);
        Channel wifiP2pChannel = wifiP2pManager.initialize(context, looper, null);
        wifiP2pManager.cancelConnect(wifiP2pChannel, null);
        wifiP2pManager.removeGroup(wifiP2pChannel, null);
    }

    public static void stopSoftAp(Context context) {
        if (context == null)
            return;
        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(
                Context.CONNECTIVITY_SERVICE);
        cm.stopTethering(android.net.ConnectivityManager.TETHERING_WIFI);
    }

    public static boolean overwriteApChannelIfNeed(Context context,
            IHostapd.IfaceParams ifaceParams) {
        if (context == null || ifaceParams == null)
            return false;
        int fixChannel = SystemProperties.getInt("vendor.wifi.tethering.channel", 0);
        WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        if (fixChannel > 0) {
            Log.d(TAG, "Disable ACS & overwrite hotspot op channel to " + fixChannel);
            ifaceParams.channelParams.enableAcs = false;
            ifaceParams.channelParams.channel = fixChannel;
            if (fixChannel <= 14) {
                ifaceParams.channelParams.band = IHostapd.Band.BAND_2_4_GHZ;
            } else {
                ifaceParams.channelParams.band = IHostapd.Band.BAND_5_GHZ;
            }
            return true;
        } else if (wifiManager.getCurrentNetwork() != null) {
            int staFreq = wifiManager.getConnectionInfo().getFrequency();
            int staChannel = ApConfigUtil.convertFrequencyToChannel(staFreq);
            Log.d(TAG, "[STA+SAP] Need to config channel for STA+SAP case"
                    + ", getCurrentNetwork = " + wifiManager.getCurrentNetwork()
                    + ", staChannel = " + staChannel);

            if (staChannel >= 1 && staChannel <= 14) {
                ifaceParams.channelParams.band = IHostapd.Band.BAND_2_4_GHZ;
            } else {
                ifaceParams.channelParams.band = IHostapd.Band.BAND_5_GHZ;
            }
            ifaceParams.channelParams.channel = staChannel;
            ifaceParams.channelParams.enableAcs = false;

            Log.d(TAG, "[STA+SAP] apBand = " + ifaceParams.channelParams.band
                    + ", apChannel = " + ifaceParams.channelParams.channel);
            return true;
        }
        return false;
    }
}
