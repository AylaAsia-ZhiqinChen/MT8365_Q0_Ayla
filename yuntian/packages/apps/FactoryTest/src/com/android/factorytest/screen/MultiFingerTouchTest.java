package com.android.factorytest.screen;

import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;
import com.android.factorytest.view.MultiFingerTouchView;

/**
 * 多指触摸测试
 */
public class MultiFingerTouchTest extends BaseActivity {

    private MultiFingerTouchView mTouchView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setActionBarEnabled(false);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
//        requestWindowFeature(Window.FEATURE_NO_TITLE);
//        int flag= WindowManager.LayoutParams.FLAG_FULLSCREEN;
//        getWindow().setFlags(flag, flag);

        setContentView(R.layout.activity_multi_finger_touch_test);

        super.onCreate(savedInstanceState);

        //getActionBar().hide();
        setBottomButtonVisible(View.GONE);

        mTouchView = (MultiFingerTouchView) findViewById(R.id.multi_finger_touch_view);
        mTouchView.setOnFingerCountChangedListener(mListener);
    }

    private MultiFingerTouchView.OnFingerCountChangedListener mListener = new MultiFingerTouchView.OnFingerCountChangedListener() {
        @Override
        public void notifyFingerCountChanged(int count) {
            Log.i(MultiFingerTouchTest.this, "notifyFingerCountChanged=>count: " + count + " testCompleted: " + isTestCompleted());
            if (!isTestCompleted()) {
                boolean enabledPass = false;
                if (count >= 2) {
                    enabledPass = true;
                }
                //getActionBar().show();
                setBottomButtonVisible(View.VISIBLE);
                if (enabledPass) {
                    setTestCompleted(true);
                    setPassButtonEnabled(true);
                    setTestPass(true);
                    if (isAutoTest()) {
                        if (isTestPass()) {
                            Toast.makeText(MultiFingerTouchTest.this, getString(R.string.auto_test_pass_tip, getAutoTestNextTestDelayedTime() / 1000), Toast.LENGTH_SHORT).show();
                        } else {
                            Toast.makeText(MultiFingerTouchTest.this, getString(R.string.auto_test_fail_tip, getAutoTestNextTestDelayedTime() / 1000), Toast.LENGTH_SHORT).show();
                        }
                        doOnAutoTest();
                    }
                } else {
                    setPassButtonEnabled(false);
                }
            }
        }
    };
}
