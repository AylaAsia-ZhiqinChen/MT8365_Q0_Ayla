package com.android.factorytest.screen;

import android.content.pm.ActivityInfo;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.R;

/**
 * 屏幕背光测试
 */
public class ScreenBacklightTest extends BaseActivity {

    private static final int MSG_SWITCH_TEST_COLOR = 1;

    private View mBackgroundView;

    private int[] mTestColors;
    private int mTestTimes;
    private int mCurrentTimes;
    private int mSwitchDelayedTime;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setActionBarEnabled(false);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        int flag= WindowManager.LayoutParams.FLAG_FULLSCREEN;
        getWindow().setFlags(flag, flag);

        setContentView(R.layout.activity_screen_back_light_test);

        super.onCreate(savedInstanceState);

        Resources res = getResources();
        setBottomButtonVisible(View.GONE);
        mTestColors = res.getIntArray(R.array.screen_backlight_test_colors);
        mTestTimes = res.getInteger(R.integer.screen_backlight_test_times);
        mSwitchDelayedTime = res.getInteger(R.integer.screen_backlight_switch_color_delayed_time);
        mCurrentTimes = 0;
        mBackgroundView = findViewById(R.id.backgroup_view);
    }

    @Override
    protected void onResume() {
        super.onResume();
        int currentColor  = mTestColors[mCurrentTimes % mTestColors.length];
        mBackgroundView.setBackgroundColor(currentColor);
        if (!isTestCompleted()) {
            mHandler.sendEmptyMessageDelayed(MSG_SWITCH_TEST_COLOR, mSwitchDelayedTime);
        } else {
            setTestCompleted(true);
            setPassButtonEnabled(true);
            setBottomButtonVisible(View.VISIBLE);
        }
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_SWITCH_TEST_COLOR:
                    mCurrentTimes++;
                    if (mCurrentTimes < mTestTimes) {
                        int currentColor = mTestColors[mCurrentTimes % mTestColors.length];
                        mBackgroundView.setBackgroundColor(currentColor);
                        mHandler.sendEmptyMessageDelayed(MSG_SWITCH_TEST_COLOR, mSwitchDelayedTime);
                    } else {
                        setTestCompleted(true);
                        setPassButtonEnabled(true);
                        setBottomButtonVisible(View.VISIBLE);
                    }
                    break;
            }
        }
    };
}
