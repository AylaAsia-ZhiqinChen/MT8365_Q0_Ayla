package com.android.factorytest.face;

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
 * 人脸测试
 */
public class FaceTest extends BaseActivity {
private Button mFacebtn;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_face_test);
        super.onCreate(savedInstanceState);
        setPassButtonEnabled(true);
        mFacebtn=(Button) findViewById(R.id.face_btn);
        mFacebtn.setOnClickListener(new View.OnClickListener() {
        @Override
        public void onClick(View v) {
                Intent intent = new Intent(Intent.ACTION_MAIN);
				ComponentName componentName = new ComponentName("net.wt.gate.dev", "net.wt.gate.dev.function.face.FaceTestActivity");
				intent.setComponent(componentName);
				startActivity(intent);
			}
        });
        mFacebtn.performClick();
    }
}
