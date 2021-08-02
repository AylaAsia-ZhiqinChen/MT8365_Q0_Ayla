package com.android.factorytest.annex;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Resources;
import android.os.BatteryManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Html;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.Calendar;
import java.text.DecimalFormat;
/**
 * 充电测试
 */
public class ChargeTest extends BaseActivity {

    private static final int MSG_UPDATE_BATTERY_INFO = 0;
    private static final int MSG_UPDATE_BATTERY_CURRENT = 1;
    private static final long UPDATE_BATTERY_CURRENT_DELAYED = 500;

    private static final String BATTERY_AVERAGE_CURRENT_PATH = "/sys/devices/platform/yt_hwinfo/FG_Battery_Current";

    private TextView mBatteryStatusTv;
    private TextView mBatteryVoltageTv;
    private TextView mBatteryPluggedTv;
    private TextView mBatteryCurrentTv;
    private Resources mResources;

    private int mBatteryStatus;
    private int mBatteryVoltage;
    private int mBatteryPlugged;
    private boolean mIsCharging;
    private boolean mStatusPass;
    private boolean mVoltagePass;
    private boolean mPluggedPass;
    private boolean mCurrentPass;
    private boolean mEnabledPass;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_charge_test);

        super.onCreate(savedInstanceState);

        mBatteryStatus = BatteryManager.BATTERY_STATUS_UNKNOWN;
        mBatteryVoltage = -1;
        mBatteryPlugged = -1;
        mIsCharging = false;
        mStatusPass = false;
        mVoltagePass = false;
        mPluggedPass = false;
        mCurrentPass = false;
        mEnabledPass = false;
        mResources = getResources();
        mBatteryStatusTv = (TextView) findViewById(R.id.battery_status);
        mBatteryVoltageTv = (TextView) findViewById(R.id.battery_voltage);
        mBatteryPluggedTv = (TextView) findViewById(R.id.battery_plugged);
        mBatteryCurrentTv = (TextView) findViewById(R.id.battery_current);
    }

    @Override
    protected void onResume() {
        super.onResume();
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_BATTERY_CHANGED);
        registerReceiver(mBatteryReceiver, filter);
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mBatteryReceiver);
        mHandler.removeMessages(MSG_UPDATE_BATTERY_INFO);
        mHandler.removeMessages(MSG_UPDATE_BATTERY_CURRENT);
    }

    private void updateBatteryInfo() {
        String status = null;
        boolean charging = false;
        mStatusPass = true;
        if (mBatteryStatus == BatteryManager.BATTERY_STATUS_CHARGING) {
            charging = true;
            status = mResources.getString(R.string.charge_test_battery_status_pass, mResources.getString(R.string.charge_test_battery_status_charging));
        } else if (mBatteryStatus == BatteryManager.BATTERY_STATUS_DISCHARGING && mBatteryPlugged != 0) {
            status = mResources.getString(R.string.charge_test_battery_status_pass, mResources.getString(R.string.charge_test_battery_status_discharging));
        } else if (mBatteryStatus == BatteryManager.BATTERY_STATUS_FULL) {
            status = mResources.getString(R.string.charge_test_battery_status_pass, mResources.getString(R.string.charge_test_battery_status_full));
        } else if (mBatteryStatus == BatteryManager.BATTERY_STATUS_NOT_CHARGING || mBatteryPlugged == 0) {
            status = mResources.getString(R.string.charge_test_battery_status_pass, mResources.getString(R.string.charge_test_battery_status_not_charging));
        } else {
            mStatusPass = false;
            status = mResources.getString(R.string.charge_test_battery_status_fail, mResources.getString(R.string.charge_test_battery_status_unknown));
        }
        mBatteryStatusTv.setText(Html.fromHtml(status));

        String voltage = null;
        mVoltagePass = true;
        if (mBatteryVoltage < 0) {
            mVoltagePass = false;
            voltage = mResources.getString(R.string.charge_test_battery_voltage_fail, mResources.getString(R.string.charge_test_battery_voltage_unknown));
        } else {
            voltage = mResources.getString(R.string.charge_test_battery_voltage_pass, mBatteryVoltage + "mV");
        }
        mBatteryVoltageTv.setText(Html.fromHtml(voltage));

        String plugged = null;
        mPluggedPass = true;
        if (mBatteryPlugged == BatteryManager.BATTERY_PLUGGED_AC) {
            plugged = mResources.getString(R.string.charge_test_battery_plugged_pass, mResources.getString(R.string.charge_test_battery_plugged_ac));
        } else if (mBatteryPlugged == BatteryManager.BATTERY_PLUGGED_USB) {
            plugged = mResources.getString(R.string.charge_test_battery_plugged_pass, mResources.getString(R.string.charge_test_battery_plugged_usb));
        } else if (mBatteryPlugged == BatteryManager.BATTERY_PLUGGED_WIRELESS) {
            plugged = mResources.getString(R.string.charge_test_battery_plugged_pass, mResources.getString(R.string.charge_test_battery_plugged_wireless));
        } else {
            if (charging) {
                plugged = mResources.getString(R.string.charge_test_battery_plugged_fail, mResources.getString(R.string.charge_test_battery_plugged_unknown));
            } else {
                mPluggedPass = false;
                plugged = mResources.getString(R.string.charge_test_battery_plugged_pass, "--");
            }
        }
        mBatteryPluggedTv.setText(Html.fromHtml(plugged));

        if (mIsCharging != charging) {
            mIsCharging = charging;
        }
        if (charging) {
            mHandler.sendEmptyMessage(MSG_UPDATE_BATTERY_CURRENT);
        } else {
            mHandler.removeMessages(MSG_UPDATE_BATTERY_CURRENT);
            String currentStr = mResources.getString(R.string.charge_test_battery_current_pass, "--");
            mBatteryCurrentTv.setText(Html.fromHtml(currentStr));
            mCurrentPass = true;
            updateTestResult();
        }
    }

    private void updateBatteryCurrent() {
        float current = readBatteryChargingCurrent();
        String currentStr = null;
        if (current >= 0) {
            currentStr = mResources.getString(R.string.charge_test_battery_current_pass, new DecimalFormat("0.00").format(current) + "mA");
        } else {
            currentStr = mResources.getString(R.string.charge_test_battery_current_fail, mResources.getString(R.string.charge_test_unknown_current));
        }
        mBatteryCurrentTv.setText(Html.fromHtml(currentStr));
    }

    private float readBatteryChargingCurrent() {
        char[] buffer = new char[1024];
        float current = -1;
        File file = new File(BATTERY_AVERAGE_CURRENT_PATH);
        FileReader fr = null;
        try {
            fr = new FileReader(file);
            int len = fr.read(buffer, 0, 1024);
            current = Float.valueOf((new String(buffer, 0, len)));
            Log.d("readBatteryChargingCurrent-->", "len = " + len + " ; current = " + current);
            mCurrentPass = true;
        } catch (Exception e) {
            mCurrentPass = false;
            Log.e(this, "readBatteryChargingCurrent=>error: ", e);
        } finally {
            if (fr != null) {
                try {
                    fr.close();
                } catch (Exception e) {
                }
            }
        }
        Log.d(this, "readBatteryChargingCurrent=>current: " + current);
        return current;
    }

    private void updateTestResult() {
        if (!isTestCompleted()) {
            boolean pass = false;
            if (mStatusPass && mVoltagePass && mPluggedPass && mCurrentPass) {
                pass = true;
            } else {
                pass = false;
            }
            if (mEnabledPass != pass) {
                mEnabledPass = pass;
                if (mEnabledPass) {
                    setPassButtonEnabled(true);
                    setTestCompleted(true);
                    setTestPass(true);
                } else {
                    setPassButtonEnabled(false);
                }
                if (isAutoTest()) {
                    if (isTestPass()) {
                        Toast.makeText(this, getString(R.string.auto_test_pass_tip, getAutoTestNextTestDelayedTime() / 1000), Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(this, getString(R.string.auto_test_fail_tip, getAutoTestNextTestDelayedTime() / 1000), Toast.LENGTH_SHORT).show();
                    }
                    doOnAutoTest();
                }
            }
        }
    }

    private BroadcastReceiver mBatteryReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (Intent.ACTION_BATTERY_CHANGED.equals(action)) {
                mBatteryStatus = intent.getIntExtra(BatteryManager.EXTRA_STATUS, -1);
                mBatteryVoltage = intent.getIntExtra(BatteryManager.EXTRA_VOLTAGE, -1);
                mBatteryPlugged = intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, -1);
                mHandler.sendEmptyMessage(MSG_UPDATE_BATTERY_INFO);
                Log.d(ChargeTest.this, "onReceive=>status: " + mBatteryStatus + " voltage: "
                        + mBatteryVoltage + "pluged: " + mBatteryPlugged);
            }
        }
    };

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_UPDATE_BATTERY_INFO:
                    updateBatteryInfo();
                    break;

                case MSG_UPDATE_BATTERY_CURRENT:
                    updateBatteryCurrent();
                    updateTestResult();
                    mHandler.sendEmptyMessageDelayed(MSG_UPDATE_BATTERY_CURRENT, UPDATE_BATTERY_CURRENT_DELAYED);
                    break;
            }
        }
    };
}
