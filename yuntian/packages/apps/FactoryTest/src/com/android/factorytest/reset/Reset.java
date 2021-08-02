package com.android.factorytest.reset;

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
 * 恢复出厂设置
 */
public class Reset extends BaseActivity {
    private Button mResetBtn;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_reset);
        super.onCreate(savedInstanceState);
        setPassButtonEnabled(true);
        mResetBtn = (Button) findViewById(R.id.reset_btn);
        mResetBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(Intent.ACTION_MAIN);
                ComponentName componentName = new ComponentName("net.wt.gate.dev", "net.wt.gate.dev.function.hide.ResetActivity");
                intent.setComponent(componentName);
                startActivity(intent);
            }
        });
    }
}
