package com.android.settings.ethernet;

import android.content.ContentResolver;
import android.os.Bundle;
import androidx.preference.Preference;
import androidx.preference.EditTextPreference;
import androidx.preference.SwitchPreference;

import android.provider.Settings;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.widget.Toast;

import android.text.TextUtils;

import java.net.InetAddress;

import android.net.EthernetManager;
import android.net.StaticIpConfiguration;
import android.net.LinkAddress;
import android.net.IpConfiguration;
import android.net.IpConfiguration.IpAssignment;
import android.net.IpConfiguration.ProxySettings;
import android.content.Context;

import com.android.settings.SettingsPreferenceFragment;
import com.android.settings.R;
import com.android.internal.logging.nano.MetricsProto.MetricsEvent;

public class EthernetStaticIP extends SettingsPreferenceFragment
        implements Preference.OnPreferenceChangeListener {
    private static final String TAG = "EthernetStaticIP";
    public static final boolean DEBUG = false;

    private static void LOG(String msg) {
        if (DEBUG) {
            Log.d(TAG, msg);
        }
    }

    /*-------------------------------------------------------*/

    private static final String KEY_USE_STATIC_IP = "use_static_ip";

    private static final String KEY_IP_ADDRESS = "ip_address";
    private static final String KEY_GATEWAY = "gateway";
    private static final String KEY_NETMASK = "netmask";
    private static final String KEY_DNS1 = "dns1";
    private static final String KEY_DNS2 = "dns2";
    public static final String ETHERNET_USE_STATIC_IP = Settings.Global.ETHERNET_STATIC_ON;

    private static final int MENU_ITEM_SAVE = Menu.FIRST;
    private static final int MENU_ITEM_CANCEL = Menu.FIRST + 1;

    private String[] mSettingNames = {
            Settings.Global.ETHERNET_STATIC_IP,
            Settings.Global.ETHERNET_STATIC_GATEWAY,
            Settings.Global.ETHERNET_STATIC_MASK,
            Settings.Global.ETHERNET_STATIC_DNS1,
            Settings.Global.ETHERNET_STATIC_DNS2
    };


    private String[] mPreferenceKeys = {
            KEY_IP_ADDRESS,
            KEY_GATEWAY,
            KEY_NETMASK,
            KEY_DNS1,
            KEY_DNS2,
    };

    /*-------------------------------------------------------*/

    private SwitchPreference mUseStaticIpSwitch;
    private StaticIpConfiguration mStaticIpConfiguration;
    private IpConfiguration mIpConfiguration;
    private EthernetManager mEthernetManager;

    private boolean isOnPause = false;
    private boolean chageState = false;

    public EthernetStaticIP() {
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

        addPreferencesFromResource(R.xml.ethernet_static_ip);

        mUseStaticIpSwitch = (SwitchPreference) findPreference(KEY_USE_STATIC_IP);
        mUseStaticIpSwitch.setOnPreferenceChangeListener(this);

        for (int i = 0; i < mPreferenceKeys.length; i++) {
            Preference preference = findPreference(mPreferenceKeys[i]);
            preference.setOnPreferenceChangeListener(this);
        }
        setHasOptionsMenu(true);
    }

    @Override
    public void onResume() {
        super.onResume();
        if (!isOnPause) {
            updateIpSettingsInfo();
        }
        isOnPause = false;
    }

    @Override
    public int getMetricsCategory() {
        return MetricsEvent.ETHERNET_STATIC;
    }

    @Override
    public void onPause() {
        isOnPause = true;
        super.onPause();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        menu.add(Menu.NONE, MENU_ITEM_SAVE, 0, R.string.save_satic_ethernet)
                .setEnabled(true)
                .setShowAsAction(MenuItem.SHOW_AS_ACTION_ALWAYS);
        super.onCreateOptionsMenu(menu, inflater);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {

        switch (item.getItemId()) {

            case MENU_ITEM_SAVE:
                saveIpSettingsInfo();
                if (isIpDataInUiComplete())
                    finish();
                return true;

            case MENU_ITEM_CANCEL:
                finish();
                return true;
        }

        return super.onOptionsItemSelected(item);
    }


    private void updateIpSettingsInfo() {
        LOG("Static IP status updateIpSettingsInfo");
        ContentResolver contentResolver = getContentResolver();

        mUseStaticIpSwitch.setChecked(Settings.Global.getInt(contentResolver, ETHERNET_USE_STATIC_IP, 0) != 0);

        for (int i = 0; i < mSettingNames.length; i++) {
            EditTextPreference preference = (EditTextPreference) findPreference(mPreferenceKeys[i]);
            String settingValue = Settings.Global.getString(contentResolver, mSettingNames[i]);
            preference.setText(settingValue);
            preference.setSummary(settingValue);
        }
    }


    private void saveIpSettingsInfo() {
        ContentResolver contentResolver = getContentResolver();
	  /*      
	        if(!chageState)   
	        	return;
	  */
        if (!isIpDataInUiComplete()) {
            Toast.makeText(getActivity(), R.string.eth_ip_settings_please_complete_settings, Toast.LENGTH_LONG).show();
            return;
        }

        mIpConfiguration = new IpConfiguration();
        mStaticIpConfiguration = new StaticIpConfiguration();

        for (int i = 0; i < mSettingNames.length; i++) {

            EditTextPreference preference = (EditTextPreference) findPreference(mPreferenceKeys[i]);
            String text = preference.getText();
            try {
                switch (mPreferenceKeys[i]) {
                    case KEY_IP_ADDRESS:
                        mStaticIpConfiguration.ipAddress = new LinkAddress(InetAddress.getByName(text), 24);
                        break;
                    case KEY_GATEWAY:
                        mStaticIpConfiguration.gateway = InetAddress.getByName(text);
                        break;
                    case KEY_NETMASK:
                        mStaticIpConfiguration.domains = text;
                        break;
                    case KEY_DNS1:
                        mStaticIpConfiguration.dnsServers.add(InetAddress.getByName(text));
                        break;
                    case KEY_DNS2:
                        mStaticIpConfiguration.dnsServers.add(InetAddress.getByName(text));
                        break;
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
            if (null == text || TextUtils.isEmpty(text)) {
                Settings.Global.putString(contentResolver, mSettingNames[i], null);
            } else {
                Settings.Global.putString(contentResolver, mSettingNames[i], text);
            }
        }
        mIpConfiguration.ipAssignment = IpAssignment.STATIC;
        mIpConfiguration.proxySettings = ProxySettings.STATIC;
        mIpConfiguration.staticIpConfiguration = mStaticIpConfiguration;
        mEthernetManager = (EthernetManager) getSystemService(Context.ETHERNET_SERVICE);
        if (mUseStaticIpSwitch.isChecked())
            mEthernetManager.setConfiguration("eth0", mIpConfiguration);
        Settings.Global.putInt(contentResolver, ETHERNET_USE_STATIC_IP, mUseStaticIpSwitch.isChecked() ? 1 : 0);
    }

    @Override
    public boolean onPreferenceTreeClick(Preference preference) {

        boolean result = true;
        LOG("onPreferenceTreeClick()  chageState = " + chageState);
        chageState = true;

        return result;
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        boolean result = true;

        String key = preference.getKey();
        LOG("onPreferenceChange() : key = " + key);

        if (null == key) {
            return true;
        } else if (key.equals(KEY_USE_STATIC_IP)) {


        } else if (key.equals(KEY_IP_ADDRESS)
                || key.equals(KEY_GATEWAY)
                || key.equals(KEY_NETMASK)
                || key.equals(KEY_DNS1)
                || key.equals(KEY_DNS2)) {

            String value = (String) newValue;

            LOG("onPreferenceChange() : value = " + value);


            if (TextUtils.isEmpty(value)) {

                ((EditTextPreference) preference).setText(value);

                preference.setSummary(value);

                result = true;
            } else if (!isValidIpAddress(value)) {
                LOG("onPreferenceChange() : IP address user inputed is INVALID.");

                Toast.makeText(getActivity(), R.string.ethernet_ip_settings_invalid_ip, Toast.LENGTH_LONG).show();

                return false;
            } else {

                ((EditTextPreference) preference).setText(value);

                preference.setSummary(value);

                result = true;
            }
        }
        return result;
    }

    private boolean isValidIpAddress(String value) {

        int start = 0;
        int end = value.indexOf('.');
        int numBlocks = 0;

        while (start < value.length()) {

            if (-1 == end) {
                end = value.length();
            }

            try {
                int block = Integer.parseInt(value.substring(start, end));
                if ((block > 255) || (block < 0)) {
                    Log.w(TAG, "isValidIpAddress() : invalid 'block', block = " + block);
                    return false;
                }
            } catch (NumberFormatException e) {
                Log.w(TAG, "isValidIpAddress() : e = " + e);
                return false;
            }

            numBlocks++;

            start = end + 1;
            end = value.indexOf('.', start);
        }

        return numBlocks == 4;
    }

    private boolean isIpDataInUiComplete() {
        ContentResolver contentResolver = getContentResolver();
        for (int i = 0; i < (mPreferenceKeys.length - 1); i++) {
            EditTextPreference preference = (EditTextPreference) findPreference(mPreferenceKeys[i]);
            String text = preference.getText();
            LOG("isIpDataInUiComplete() : text = " + text);
            if (null == text || TextUtils.isEmpty(text)) {
                return false;
            }
        }
        return true;
    }
}

