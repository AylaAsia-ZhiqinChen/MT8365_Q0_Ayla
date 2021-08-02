package com.android.factorytest.versioninfo;

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
 * 版本信息
 */
public class VersionInfo extends BaseActivity {
    private Button mVersionInfoBtn;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_version_info);
        super.onCreate(savedInstanceState);
        setPassButtonEnabled(true);
        mVersionInfoBtn = (Button) findViewById(R.id.version_info_btn);
        mVersionInfoBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(Intent.ACTION_MAIN);
                ComponentName componentName = new ComponentName("net.wt.gate.dev", "net.wt.gate.dev.function.hide.VersionInfoActivity");
                intent.setComponent(componentName);
                startActivity(intent);
            }
        });
        mVersionInfoBtn.performClick();
    }
}
