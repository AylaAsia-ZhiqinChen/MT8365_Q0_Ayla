package com.android.factorytest.screen;

import android.annotation.TargetApi;
import android.content.pm.ActivityInfo;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;
import com.android.factorytest.view.ScreenColorView;

/**
 * 屏幕颜色测试
 */
public class ScreenColorTest extends BaseActivity {

    private ScreenColorView mScreenColorView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setActionBarEnabled(false);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        int flag= WindowManager.LayoutParams.FLAG_FULLSCREEN;
        getWindow().setFlags(flag, flag);

        setContentView(R.layout.activity_screen_color_test);

        super.onCreate(savedInstanceState);

        setBottomButtonVisible(View.GONE);
        mScreenColorView = (ScreenColorView) findViewById(R.id.screen_color_view);
        mScreenColorView.setOnScreenColorTestCompletedListener(mListener);
    }

    private ScreenColorView.OnScreenColorTestCompletedListener mListener = new ScreenColorView.OnScreenColorTestCompletedListener() {
        @Override
        public void notifyScreenColorTestCompleted() {
            setBottomButtonVisible(View.VISIBLE);
            setTestCompleted(true);
            setPassButtonEnabled(true);
        }
    };
}
