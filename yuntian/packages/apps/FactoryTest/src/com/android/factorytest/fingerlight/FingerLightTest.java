package com.android.factorytest.fingerlight;

import android.content.res.Resources;
import android.os.Bundle;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.R;

import android.content.ComponentName;
import android.content.Intent;

import com.android.factorytest.R;

import android.widget.Button;
import android.view.View;


/**
 * 指纹跑马灯测试
 */
public class FingerLightTest extends BaseActivity {
    private Button mLightBtn;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_finger_light_test);
        super.onCreate(savedInstanceState);
        setPassButtonEnabled(true);
        mLightBtn = (Button) findViewById(R.id.light_btn);
        mLightBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(Intent.ACTION_MAIN);
                ComponentName componentName = new ComponentName("net.wt.gate.dev", "net.wt.gate.dev.function.hide.FingerprintLightTest");
                intent.setComponent(componentName);
                startActivity(intent);
            }
        });
        mLightBtn.performClick();
    }
}
