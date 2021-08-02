package com.android.factorytest.system;

import android.app.ActivityManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.text.format.DateFormat;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.R;

import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * 系统时间测试
 */
public class RtcTest extends BaseActivity {

    private static final int MSG_UPDATE_SYTEM_CLOCK = 1;
    private static final int UPDATE_SYSTEM_CLOCK_DELAYED = 50;

    private TextView mSystemClockTv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_rtc_test);

        super.onCreate(savedInstanceState);

        mSystemClockTv = (TextView) findViewById(R.id.system_clock);
    }

    @Override
    protected void onResume() {
        super.onResume();

        boolean result = updateSystemClock();

        setTestCompleted(true);
        if (result) {
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

    @Override
    protected void onPause() {
        super.onPause();
        mHandler.removeMessages(MSG_UPDATE_SYTEM_CLOCK);
    }

    public boolean updateSystemClock() {
        boolean result = false;
        mHandler.removeMessages(MSG_UPDATE_SYTEM_CLOCK);
        boolean is24 = DateFormat.is24HourFormat(this);
        SimpleDateFormat sdf = new SimpleDateFormat("hh:mm:ss");
        if (is24) {
            sdf = new SimpleDateFormat("kk:mm:ss");
        }
        String time = sdf.format(new Date());
        mSystemClockTv.setText(time);
        if (!TextUtils.isEmpty(time)) {
            result = true;
            mHandler.sendEmptyMessageDelayed(MSG_UPDATE_SYTEM_CLOCK, UPDATE_SYSTEM_CLOCK_DELAYED);
        }
        return result;
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_UPDATE_SYTEM_CLOCK:
                    updateSystemClock();
                    break;
            }
        }
    };
}
