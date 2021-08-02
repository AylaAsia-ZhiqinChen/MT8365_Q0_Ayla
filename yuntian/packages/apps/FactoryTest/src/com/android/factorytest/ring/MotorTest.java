package com.android.factorytest.ring;

import android.os.Bundle;
import android.os.Vibrator;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.R;

/**
 * 振动测试
 */
public class MotorTest extends BaseActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_motor_test);

        super.onCreate(savedInstanceState);
    }

    @Override
    protected void onResume() {
        super.onResume();
        Vibrator vibrator = (Vibrator) getSystemService(VIBRATOR_SERVICE);
        if (vibrator.hasVibrator()) {
            setPassButtonEnabled(true);
        }
    }
    
    @Override
    protected void onPause() {
        super.onPause();
        Vibrator vibrator = (Vibrator) getSystemService(VIBRATOR_SERVICE);
        if (vibrator.hasVibrator()) {
            vibrator.cancel();
        }
    }    
}
