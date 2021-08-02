package com.android.agingtest.test;

import com.android.agingtest.BaseActivity;
import com.android.agingtest.Log;
import com.android.agingtest.TestUtils;
import com.android.agingtest.R;

import android.app.ActionBar;
import android.content.SharedPreferences;
import android.widget.Button;
import android.widget.TextView;

public class RebootActivity extends BaseActivity {

    public static final int FLAG_HOMEKEY_DISPATCHED = 0x80000000;
    private int mRebootDelayed;
    private int mRebootTime;
    private int counter = 0;
    private int mRebootTimes = 0;

    @Override
    public void initValues() {
        // TODO Auto-generated method stub
        super.initValues();
        mRebootDelayed = getResources().getInteger(R.integer.reboot_delayed);
        mRebootTimes = mSharedPreferences.getInt(TestUtils.REBOOT_KEY + TestUtils.TEST_TIME, 20);
        if (getResources().getBoolean(R.bool.use_total_test_time)) {
            mRebootTimes = mSharedPreferences.getInt("singleTestTime", 20);
            if (mRebootTimes > 0 && mRebootTimes/60 == 0) {//至少运行一次
                mRebootTimes = 1;
            } else {
                mRebootTimes = mRebootTimes/60;
            }
        }
        counter = mSharedPreferences.getInt(TestUtils.CURRENT_REBOOT_TIMES, 0);
    }

    @Override
    protected void onResume() {
        // TODO Auto-generated method stub
        super.onResume();
        Log.e("lsz", "startTest counter->" + counter);
        timeTextView.setText("Times:" + (counter + 1));
    }

    @Override
    public void startTest() {
        // TODO Auto-generated method stub
        isStartTest = true;
        if (isTestOver()) {
            stopTest(true);
        } else {
            mHandler.sendEmptyMessageDelayed(HOW_TO_TEST, mRebootDelayed);
        }
    }

    @Override
    public void doTest() {
        // TODO Auto-generated method stub
        super.doTest();
        counter++;
        mSharedPreferences.edit().putInt(TestUtils.CURRENT_REBOOT_TIMES, counter).commit();
        TestUtils.reboot(RebootActivity.this, "test");
    }


    @Override
    public void stopTest(boolean isOk) {
        // TODO Auto-generated method stub
        super.stopTest(isOk);
        mSharedPreferences.edit().remove(TestUtils.CURRENT_REBOOT_TIMES);
    }

    @Override
    public boolean isTestOver() {
        // TODO Auto-generated method stub
        Log.e("lsz", "isTestOver counter->" + counter + ",mRebootTimes->" + mRebootTimes);
        return counter >= mRebootTimes;
    }

}
