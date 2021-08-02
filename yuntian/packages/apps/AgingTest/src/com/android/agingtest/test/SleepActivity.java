package com.android.agingtest.test;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.TimeZone;

import com.android.agingtest.AgingTestInterFace;
import com.android.agingtest.BaseActivity;
import com.android.agingtest.Log;
import com.android.agingtest.R;
import com.android.agingtest.ReportActivity;
import com.android.agingtest.TestUtils;

import android.app.ActionBar;
import android.app.Activity;
import android.app.KeyguardManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.os.Handler;
import android.os.PowerManager;
import android.os.SystemClock;
import android.preference.PreferenceManager;
import android.view.KeyEvent;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class SleepActivity extends BaseActivity {


    private boolean isSleep;
    private static final long SLEEP_DELAY = 3001;
    private long testTimes = 0;
    private int counter = 0;
    private PowerManager.WakeLock mWakeLock;
    private long mStartTime_ex = 0;
    private static final int UPDATE_TIME_MSG = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);

    }


    @Override
    public void startTest() {
        // TODO Auto-generated method stub
//		super.startTest();

        mStartTime_ex = System.currentTimeMillis();
        isStartTest = true;
        counter = 0;
        mHandler.sendEmptyMessage(HOW_TO_TEST);
        sHandler.sendEmptyMessage(UPDATE_TIME_MSG);
    }

    private Handler sHandler = new Handler() {
        @Override
        public void handleMessage(android.os.Message msg) {
            switch (msg.what) {
                case UPDATE_TIME_MSG:
                    long testTime = System.currentTimeMillis() - mStartTime_ex;
                    SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
                    sdf.setTimeZone(TimeZone.getTimeZone("GMT-0:00"));
                    timeTextView.setText(sdf.format(new Date(testTime)));
                    sHandler.sendEmptyMessageDelayed(UPDATE_TIME_MSG, 1000);
                    break;
            }
        };
    };

    @Override
    public void initValues() {
        // TODO Auto-generated method stub
        super.initValues();
        mWakeLock = mPowerManager.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "AGINGTEST");
        if (mWakeLock != null && !mWakeLock.isHeld()) {
            mWakeLock.acquire();
            Log.i("SleepActivity", "mWakeLock.acquire()");
        }

        testTimes = mSharedPreferences.getInt(TestUtils.SLEEP_KEY + TestUtils.TEST_TIME, 20);
        if (getResources().getBoolean(R.bool.use_total_test_time)) {
            testTimes = mSharedPreferences.getInt("singleTestTime", 20) * TestUtils.MILLSECOND;
        }
    }

    @Override
    public void doTest() {
        // TODO Auto-generated method stub

        if (isTestOver()) {
            stopTest(true);
            mHandler.removeMessages(UPDATE_TIME_MSG);
        } else {
            Log.e("lsz", "doTest counter-->" + counter);
            if (!isSleep) {
                mPowerManager.goToSleep(SystemClock.uptimeMillis());
                isSleep = true;
                counter++;
            } else {
                mPowerManager.wakeUp(SystemClock.uptimeMillis());
                isSleep = false;
            }
            mHandler.sendEmptyMessageDelayed(HOW_TO_TEST, SLEEP_DELAY);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mWakeLock != null && mWakeLock.isHeld()) {
            mWakeLock.release();
            Log.i("SleepActivity", "mWakeLock.release()");
        }
        mPowerManager.wakeUp(SystemClock.uptimeMillis());
    }


    @Override
    public boolean isTestOver() {
        // TODO Auto-generated method stub
        Log.e("lsz", "counter->" + counter + ",testtime->" + testTimes);
        boolean b = System.currentTimeMillis() - mStartTime_ex >= testTimes;
        return b;
        // return counter >= testTimes;
    }

}
