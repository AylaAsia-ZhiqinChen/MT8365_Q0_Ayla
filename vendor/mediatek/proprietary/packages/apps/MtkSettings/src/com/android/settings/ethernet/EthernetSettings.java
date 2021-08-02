package com.android.settings.ethernet;

import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Bundle;
import androidx.preference.Preference;
import androidx.preference.SwitchPreference;
import android.provider.Settings;
import android.util.Log;
import com.android.internal.logging.nano.MetricsProto.MetricsEvent;

import java.io.File;
import java.io.IOException;
import java.net.InetAddress;

import android.net.EthernetManager;
import android.net.StaticIpConfiguration;
import android.net.LinkAddress;
import android.net.IpConfiguration;
import com.android.settings.SettingsPreferenceFragment;
import com.android.settings.R;

public class EthernetSettings extends SettingsPreferenceFragment
        implements Preference.OnPreferenceChangeListener {

    private static final String TAG = "EthernetSettings";

    private static final String USE_ETHERNET_SETTINGS = "ethernet";

    public static final String ETHERNET_ON = Settings.Global.ETHERNET_ON;
    private SwitchPreference mUseEthernet;
    private PackageManager pm;
    private Context mContext;

    private boolean isEthernetEnabled() {
        return Settings.Global.getInt(mContext.getContentResolver(), ETHERNET_ON, 0) == 1 ? true : false;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        mContext = getActivity();
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.ethernet_settings);
        mUseEthernet = (SwitchPreference) findPreference(USE_ETHERNET_SETTINGS);
        mUseEthernet.setOnPreferenceChangeListener(this);
        pm = getPackageManager();
        if (isEthernetEnabled()) {
            mUseEthernet.setChecked(true);
        } else {
            mUseEthernet.setChecked(false);
        }
        if (pm.hasSystemFeature(PackageManager.FEATURE_ETHERNET)) {
            mUseEthernet.setEnabled(true);
        } else {
            mUseEthernet.setEnabled(false);
        }
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public int getMetricsCategory() {
        return MetricsEvent.ETHERNET;
    }

    @Override
    public void onPause() {
        super.onPause();
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object value) {
        boolean result = true;
        final String key = preference.getKey();
        if (USE_ETHERNET_SETTINGS.equals(key)) {
            Settings.Global.putInt(mContext.getContentResolver(), ETHERNET_ON,
                    ((Boolean) value) ? 1 : 0);
            setEthernetEnabled(((Boolean) value));
        }

        return result;
    }

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
            Log.e(TAG, "setEthernetEnabled=>error: ", e);
            flag = false;
        } finally {
            try {
                if (fr != null) {
                    fr.close();
                }
            } catch (IOException e) {
            }
        }
        //Log.e(TAG, "setEthernetEnabled=>enabled" + enabled);
        if (enabled) {
            setStaticIp();
        }
        return flag;
    }

    private void setStaticIp() {
        String[] mSettingNames = {
                Settings.Global.ETHERNET_STATIC_IP,
                Settings.Global.ETHERNET_STATIC_GATEWAY,
                Settings.Global.ETHERNET_STATIC_MASK,
                Settings.Global.ETHERNET_STATIC_DNS1,
                Settings.Global.ETHERNET_STATIC_DNS2
        };
        String ETHERNET_USE_STATIC_IP = Settings.Global.ETHERNET_STATIC_ON;
        boolean isEthernetStaticIpEnabled = Settings.Global.getInt(mContext.getContentResolver(), ETHERNET_USE_STATIC_IP, 0) != 0 ? true : false;
        if (isEthernetStaticIpEnabled) {
            IpConfiguration mIpConfiguration = new IpConfiguration();
            StaticIpConfiguration mStaticIpConfiguration = new StaticIpConfiguration();
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
            EthernetManager mEthernetManager = (EthernetManager) mContext.getSystemService(Context.ETHERNET_SERVICE);
            if (null != mEthernetManager && null != mIpConfiguration) {
                mEthernetManager.setConfiguration("eth0", mIpConfiguration);
            }
        }
    }
}
