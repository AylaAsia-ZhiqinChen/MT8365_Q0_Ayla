package com.android.factorytest.finger;

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
 * 指纹测试
 */   
public class FingerTest extends BaseActivity {
private Button mFingerbtn;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_finger_print_test);
        super.onCreate(savedInstanceState);
        setPassButtonEnabled(true);
        mFingerbtn=(Button) findViewById(R.id.finger_btn);
        mFingerbtn.setOnClickListener(new View.OnClickListener() {
        @Override
        public void onClick(View v) {
                Intent intent = new Intent(Intent.ACTION_MAIN);
				ComponentName componentName = new ComponentName("net.wt.gate.dev", "net.wt.gate.dev.function.hide.FingerTestActivity");
				intent.setComponent(componentName);
				startActivity(intent);
			}
        });   
        mFingerbtn.performClick();
    }
}
