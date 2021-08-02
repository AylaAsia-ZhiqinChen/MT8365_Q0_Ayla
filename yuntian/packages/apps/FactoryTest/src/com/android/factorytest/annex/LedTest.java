package com.android.factorytest.annex;

import android.app.Activity;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.Message;
import android.os.Handler;
import android.view.View;
import android.widget.RadioButton;
import android.widget.RadioGroup;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.R;

import java.io.File;
import java.io.IOException;

import android.content.Context;
import android.app.NotificationManager;

public class LedTest extends BaseActivity implements RadioGroup.OnCheckedChangeListener {

    private RadioGroup mLedGroup;
    private RadioButton mTurnOffLedRb;
    private RadioButton mRedLedRb;
    private RadioButton mBlueLedRb;
    private RadioButton mGreenLedRb;
    private RadioButton mWhiteLedRb;
    private static final int RED_FLASH = 0;
    private static final int GREEN_FLASH = 1;
    private static final int BLUE_FLASH = 2;
    private static final int TURN_OFF = 3;
    private static final int WHITE_FLASH = 4;
    public LedHandler mHandler = new LedHandler();
    private final String TAG = "LedTest";
    private NotificationManager mNotificationManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_led_test);

        super.onCreate(savedInstanceState);

        mLedGroup = (RadioGroup) findViewById(R.id.led_test_radio_group);
        mTurnOffLedRb = (RadioButton) findViewById(R.id.turn_off_led);
        mRedLedRb = (RadioButton) findViewById(R.id.red_led);
        mBlueLedRb = (RadioButton) findViewById(R.id.blue_led);
        mGreenLedRb = (RadioButton) findViewById(R.id.green_led);
        mWhiteLedRb = (RadioButton) findViewById(R.id.white_led);

        mNotificationManager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);

        mLedGroup.setOnCheckedChangeListener(this);

        setPassButtonEnabled(true);
        filterRadioButton();
        mTurnOffLedRb.setChecked(true);
        mNotificationManager.factoryLedTestTurnOff();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (android.os.SystemProperties.getBoolean("persist.sys.led.test.notification", false)) {
            mNotificationManager.factoryLedTestTurnOff();
        } else {
            setRedLightEnabled(false);
            setGreenLightEnabled(false);
            setBlueLightEnabled(false);
            setWhiteLightEnabled(false);
        }
        if (mHandler != null) {
            mHandler = null;
        }
    }

    @Override
    public void onCheckedChanged(RadioGroup group, int checkedId) {
        switch (checkedId) {
            case R.id.turn_off_led:
                mHandler.sendEmptyMessage(TURN_OFF);
                break;

            case R.id.red_led:
                mHandler.sendEmptyMessage(RED_FLASH);
                break;

            case R.id.blue_led:
                mHandler.sendEmptyMessage(BLUE_FLASH);
                break;

            case R.id.green_led:
                mHandler.sendEmptyMessage(GREEN_FLASH);
                break;

            case R.id.white_led:
                mHandler.sendEmptyMessage(WHITE_FLASH);
                break;
        }
    }

    private void filterRadioButton() {
        Resources res = getResources();
        if (!res.getBoolean(R.bool.support_red_led)) {
            mRedLedRb.setVisibility(View.GONE);
        }
        if (!res.getBoolean(R.bool.support_blue_led)) {
            mBlueLedRb.setVisibility(View.GONE);
        }
        if (!res.getBoolean(R.bool.support_green_led)) {
            mGreenLedRb.setVisibility(View.GONE);
        }
        if (!res.getBoolean(R.bool.support_white_led)) {
            mWhiteLedRb.setVisibility(View.GONE);
        }
    }

    class LedHandler extends Handler {
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case RED_FLASH:
                    if (mHandler != null) {
                        if (android.os.SystemProperties.getBoolean("persist.sys.led.test.notification", false)) {
                            mNotificationManager.factoryLedTestTurnOff();
                            mNotificationManager.factoryLedTestSetColor(0xFFFF0000);
                        } else {
                            setRedLightEnabled(true);
                            setGreenLightEnabled(false);
                            setBlueLightEnabled(false);
                            setWhiteLightEnabled(false);
                        }
                    }
                    break;
                case GREEN_FLASH:
                    if (mHandler != null) {
                        if (android.os.SystemProperties.getBoolean("persist.sys.led.test.notification", false)) {
                            mNotificationManager.factoryLedTestTurnOff();
                            mNotificationManager.factoryLedTestSetColor(0xFF00FF00);
                        } else {
                            setRedLightEnabled(false);
                            setGreenLightEnabled(true);
                            setBlueLightEnabled(false);
                            setWhiteLightEnabled(false);
                        }
                    }
                    break;
                case BLUE_FLASH:
                    if (mHandler != null) {
                        if (android.os.SystemProperties.getBoolean("persist.sys.led.test.notification", false)) {
                            mNotificationManager.factoryLedTestTurnOff();
                            mNotificationManager.factoryLedTestSetColor(0xFF0000FF);
                        } else {
                            setRedLightEnabled(false);
                            setGreenLightEnabled(false);
                            setBlueLightEnabled(true);
                            setWhiteLightEnabled(false);
                        }
                    }
                    break;
                case TURN_OFF:
                    if (mHandler != null) {
                        if (android.os.SystemProperties.getBoolean("persist.sys.led.test.notification", false)) {
                            mNotificationManager.factoryLedTestTurnOff();
                        } else {
                            setRedLightEnabled(false);
                            setGreenLightEnabled(false);
                            setBlueLightEnabled(false);
                            setWhiteLightEnabled(false);
                        }
                    }
                    break;
                case WHITE_FLASH:
                    if (mHandler != null) {
                        setRedLightEnabled(false);
                        setGreenLightEnabled(false);
                        setBlueLightEnabled(false);
                        setWhiteLightEnabled(true);
                    }
                    break;
            }
            super.handleMessage(msg);
        }
    }

    private static final String REDLIGHT_PATH = "/sys/devices/platform/yt_hwinfo/pmu_led1";
    private static final String GREENLIGHT_PATH = "/sys/devices/platform/yt_hwinfo/pmu_led2";
    private static final String BLUELIGHT_PATH = "/sys/devices/platform/yt_hwinfo/pmu_led3";
    private static final String WHITELIGHT_PATH = "/sys/devices/platform/yt_hwinfo/HPT_White_Led";

    public boolean setRedLightEnabled(boolean enabled) {
        boolean flag = false;
        File file = new File(REDLIGHT_PATH);
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
            android.util.Log.e("yoyo", "setRedLightEnabled=>error: ", e);
            flag = false;
        } finally {
            try {
                if (fr != null) {
                    fr.close();
                }
            } catch (IOException e) {
            }
        }
        return flag;
    }

    public boolean setGreenLightEnabled(boolean enabled) {
        boolean flag = false;
        File file = new File(GREENLIGHT_PATH);
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
            android.util.Log.e("yoyo", "setGreenLightEnabled=>error: ", e);
            flag = false;
        } finally {
            try {
                if (fr != null) {
                    fr.close();
                }
            } catch (IOException e) {
            }
        }
        return flag;
    }

    public boolean setBlueLightEnabled(boolean enabled) {
        boolean flag = false;
        File file = new File(BLUELIGHT_PATH);
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
            android.util.Log.e("yoyo", "setBlueLightEnabled=>error: ", e);
            flag = false;
        } finally {
            try {
                if (fr != null) {
                    fr.close();
                }
            } catch (IOException e) {
            }
        }
        return flag;
    }

    public boolean setWhiteLightEnabled(boolean enabled) {
        boolean flag = false;
        File file = new File(WHITELIGHT_PATH);
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
            android.util.Log.e("yoyo", "setWhiteLightEnabled=>error: ", e);
            flag = false;
        } finally {
            try {
                if (fr != null) {
                    fr.close();
                }
            } catch (IOException e) {
            }
        }
        return flag;
    }
}
