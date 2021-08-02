package com.android.factorytest.doorbutton;

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
 * 门把按钮测试
 */
public class DoorButtonTest extends BaseActivity {
    private Button mDoorButtonTestBtn;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_door_button_test);
        super.onCreate(savedInstanceState);
        setPassButtonEnabled(true);
        mDoorButtonTestBtn = (Button) findViewById(R.id.door_button_test_btn);
        mDoorButtonTestBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(Intent.ACTION_MAIN);
                ComponentName componentName = new ComponentName("net.wt.gate.dev", "net.wt.gate.dev.function.hide.LockHandleTestActivity");
                intent.setComponent(componentName);
                startActivity(intent);
            }
        });
        mDoorButtonTestBtn.performClick();
    }
}
