package com.android.factorytest.screen;

import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.R;
import com.android.factorytest.view.SingleFingerTouchView;

/**
 * 单指触摸测试
 */
public class SingleFingerTouchTest extends BaseActivity {
    
    private SingleFingerTouchView mTouchView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setActionBarEnabled(false);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        int flag= WindowManager.LayoutParams.FLAG_FULLSCREEN;
        getWindow().setFlags(flag, flag);
   //     getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_IMMERSIVE_GESTURE_ISOLATED);

        setContentView(R.layout.activity_single_finger_touch_test);

        super.onCreate(savedInstanceState);

        setBottomButtonVisible(View.GONE);
        mTouchView = (SingleFingerTouchView) findViewById(R.id.single_finger_touch_view);
        mTouchView.setOnAllSquarePassListener(mListener);
    }

    private SingleFingerTouchView.OnAllSquarePassListener mListener = new SingleFingerTouchView.OnAllSquarePassListener() {
        @Override
        public void notifyAllSquarePass() {
            setBottomButtonVisible(View.VISIBLE);
            setTestCompleted(true);
            setPassButtonEnabled(true);
            setTestPass(true);
            if (isAutoTest()) {
                if (isTestPass()) {
                    Toast.makeText(SingleFingerTouchTest.this, getString(R.string.auto_test_pass_tip,
                            getAutoTestNextTestDelayedTime() / 1000), Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(SingleFingerTouchTest.this, getString(R.string.auto_test_fail_tip,
                            getAutoTestNextTestDelayedTime() / 1000), Toast.LENGTH_SHORT).show();
                }
                doOnAutoTest();
            }
        }
    };
}
