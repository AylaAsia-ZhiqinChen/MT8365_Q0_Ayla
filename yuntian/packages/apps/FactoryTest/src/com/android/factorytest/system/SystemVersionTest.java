package com.android.factorytest.system;

import android.os.Build;
import android.os.Bundle;
import android.os.SystemProperties;
import android.telephony.TelephonyManager;
import android.text.Spannable;
import android.text.SpannableStringBuilder;
import android.text.TextUtils;
import android.text.style.ForegroundColorSpan;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

/**
 * 系统版本测试
 */

public class SystemVersionTest extends BaseActivity {

    private TextView mAndroidVersionTv;
    private TextView mKernelVersionTv;
    private TextView mSoftwareVersionTv;
    private TextView mDeviceIdTv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // 在这里调用这个方法，主要是为了基类BaseActivity可以马上获取通过和失败按钮的引用
        setContentView(R.layout.activity_system_version_test);

        super.onCreate(savedInstanceState);

        mAndroidVersionTv = (TextView) findViewById(R.id.android_version);
        mKernelVersionTv = (TextView) findViewById(R.id.kernel_version);
        mSoftwareVersionTv = (TextView) findViewById(R.id.softwore_version);
        mDeviceIdTv = (TextView) findViewById(R.id.device_id);
    }

    @Override
    protected void onResume() {
        super.onResume();
        String androidVersion = getAndroidVersion();
        String kernelVersion = getKernelVersion();
        String softwareVersion = getBuildNumber();
        String deviceId = getDeviceId();
        int redColor = getColor(R.color.red);
        int greenColor = getColor(R.color.green);
        boolean enabledPass = true;

        String title = getString(R.string.android_version_test_title);
        String info = null;
        SpannableStringBuilder builder = null;
        ForegroundColorSpan blueSpan = null;
        if (!TextUtils.isEmpty(androidVersion)) {
            info = title + androidVersion;
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(greenColor);
        } else {
            enabledPass = false;
            info = title + "null";
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(redColor);
        }
        builder.setSpan(blueSpan, title.length(), info.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        mAndroidVersionTv.setText(builder);

        title = getString(R.string.kernel_vesiont_test_title);
        if (!TextUtils.isEmpty(kernelVersion)) {
            info = title + kernelVersion;
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(greenColor);
        } else {
            enabledPass = false;
            info = title + "null";
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(redColor);
        }
        builder.setSpan(blueSpan, title.length(), info.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        mKernelVersionTv.setText(builder);

        title = getString(R.string.software_version_test_title);
        if (!TextUtils.isEmpty(softwareVersion)) {
            info = title + softwareVersion;
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(greenColor);
        } else {
            enabledPass = false;
            info = title + "null";
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(redColor);
        }
        builder.setSpan(blueSpan, title.length(), info.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        mSoftwareVersionTv.setText(builder);

        title = getString(R.string.device_id_test_title);
        if (!TextUtils.isEmpty(deviceId)) {
            info = title + deviceId;
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(greenColor);
        } else {
            if (!getResources().getBoolean(R.bool.support_hide_device_imei)) {
                enabledPass = false;
            }
            info = title + "null";
            builder = new SpannableStringBuilder(info);
            blueSpan = new ForegroundColorSpan(redColor);
        }
        builder.setSpan(blueSpan, title.length(), info.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
        mDeviceIdTv.setText(builder);
        if (getResources().getBoolean(R.bool.support_hide_device_imei)) {
            mDeviceIdTv.setVisibility(android.view.View.GONE);
        }

        setTestCompleted(true);
        if (enabledPass) {
            setPassButtonEnabled(true);
            setTestPass(true);
        } else {
            setPassButtonEnabled(false);
            setTestPass(false);
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

    /**
     * 获取Android版本名称
     *
     * @return 返回Android版本名称
     */
    public String getAndroidVersion() {
        return Build.VERSION.RELEASE;
    }

    /**
     * 获取内核版本名称
     *
     * @return 返回内核版本名称
     */
    public String getKernelVersion() {
        BufferedReader br = null;
        FileReader fr = null;
        StringBuilder sb = new StringBuilder();

        try {
            fr = new FileReader("/proc/version");
            br = new BufferedReader(fr);
            String line = null;
            while ((line = br.readLine()) != null) {
                sb.append(line);
            }
        } catch (Exception e) {
            Log.e(this, "getKernelVersion=>error: ", e);
        } finally {
            try {
                if (br != null) {
                    br.close();
                    br = null;
                }
            } catch (IOException e) {}
            try {
                if (fr != null) {
                    fr.close();
                    fr = null;
                }
            } catch (Exception e) {}
        }
        return sb.toString();
    }

    /**
     * 获取编译序号
     * @return 返回编译序号
     */
    private String getBuildNumber() {
        return SystemProperties.get("ro.build.display.id");
    }

    /**
     * 获取设备ID
     * @return 返回设备ID
     */
    private String getDeviceId() {
        TelephonyManager tm = (TelephonyManager) getSystemService(TELEPHONY_SERVICE);
        return tm.getDeviceId();
    }
}
