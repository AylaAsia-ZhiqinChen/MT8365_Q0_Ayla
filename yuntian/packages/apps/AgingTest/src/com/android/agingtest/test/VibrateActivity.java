package com.android.agingtest.test;

import com.android.agingtest.BaseActivity;
import com.android.agingtest.TestUtils;

public class VibrateActivity extends BaseActivity {

    @Override
    public void doTest() {
        // TODO Auto-generated method stub
        super.doTest();
        TestUtils.vibrate(this);
    }

    @Override
    public void stopTest(boolean isOk) {
        // TODO Auto-generated method stub
        super.stopTest(isOk);
        TestUtils.cancelVibrate(this);
    }
}
