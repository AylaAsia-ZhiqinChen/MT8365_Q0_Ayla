package com.android.factorytest.phone;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.telephony.TelephonyManager;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Method;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * 移动数据测试
 */
public class MobileDataTest extends BaseActivity {

    private String mWebsite;
    private static final int MSG_SET_MOBILE_DATA_TEST = 0;
    private static final int MSG_OPEN_WEBSITE_TEST = 1;
    private boolean mWifiEnabled;
    private boolean mMobileDatanabled;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_mobile_data_test);
        super.onCreate(savedInstanceState);
        mWifiEnabled = isWifiOpen();
        mMobileDatanabled = getMobileDataState();
        mWebsite = getString(R.string.mobile_data_test_website);

        mHandler.sendEmptyMessageDelayed(MSG_SET_MOBILE_DATA_TEST, 800);
        mHandler.sendEmptyMessageDelayed(MSG_OPEN_WEBSITE_TEST, 1200);
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mWifiEnabled) {
            openWiFi();
        }
        setMobileDataState(mMobileDatanabled);
        mHandler.removeMessages(MSG_SET_MOBILE_DATA_TEST);
        mHandler.removeMessages(MSG_OPEN_WEBSITE_TEST);
        mHandler.removeCallbacksAndMessages(null);
    }

    /**
     * wifi是否打开
     *
     * @return
     */
    private boolean isWifiOpen() {
        WifiManager wifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        int state = wifiManager.getWifiState();
        if (state == WifiManager.WIFI_STATE_DISABLED || state == WifiManager.WIFI_STATE_DISABLING) {
            return false;
        } else if (state == WifiManager.WIFI_STATE_ENABLED || state == WifiManager.WIFI_STATE_ENABLING) {
            return true;
        }
        return false;
    }

    /**
     * 打开WiFi
     *
     * @return
     */
    private boolean openWiFi() {
        WifiManager wifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        boolean bRet = false;
        if (!wifiManager.isWifiEnabled()) {
            bRet = wifiManager.setWifiEnabled(true);
        }
        return bRet;
    }


    /**
     * 关闭WiFi
     *
     * @return
     */
    private boolean closeWiFi() {
        WifiManager wifiManager = (WifiManager) getSystemService(Context.WIFI_SERVICE);
        boolean bRet = false;
        if (wifiManager.isWifiEnabled()) {
            bRet = wifiManager.setWifiEnabled(false);
        }
        return bRet;
    }

    public void setMobileDataState(boolean enabled) {
        TelephonyManager telephonyService = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
        try {
            Method setDataEnabled = telephonyService.getClass().getDeclaredMethod("setDataEnabled", boolean.class);
            if (null != setDataEnabled) {
                setDataEnabled.invoke(telephonyService, enabled);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public boolean getMobileDataState() {
        TelephonyManager telephonyService = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
        try {
            Method getDataEnabled = telephonyService.getClass().getDeclaredMethod("getDataEnabled");
            if (null != getDataEnabled) {
                return (Boolean) getDataEnabled.invoke(telephonyService);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_SET_MOBILE_DATA_TEST:
                    if (mWifiEnabled) {
                        closeWiFi();
                    }
                    setMobileDataState(true);
                    setPassButtonEnabled(true);
                    break;
                case MSG_OPEN_WEBSITE_TEST:
                    Intent intent = new Intent();
                    Uri uri = Uri.parse(mWebsite);
                    intent = new Intent(Intent.ACTION_VIEW, uri);
                    intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                    intent.putExtra("factory_mode", true);
                    startActivity(intent);
                    break;
            }
        }
    };
}
