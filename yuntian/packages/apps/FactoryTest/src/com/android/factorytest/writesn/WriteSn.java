package com.android.factorytest.writesn;

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
 * 写入sn
 */
public class WriteSn extends BaseActivity {
private Button mWriteSnBtn;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_write_sn);
        super.onCreate(savedInstanceState);
        setPassButtonEnabled(true);
        mWriteSnBtn = (Button) findViewById(R.id.write_sn_btn);
        mWriteSnBtn.setOnClickListener(new View.OnClickListener() {
        @Override
        public void onClick(View v) {
                Intent intent = new Intent(Intent.ACTION_MAIN);
				ComponentName componentName = new ComponentName("net.wt.gate.dev", "net.wt.gate.dev.hide.WriteSnActivity");
				intent.setComponent(componentName);
				startActivity(intent);
			}
        });
        mWriteSnBtn.performClick();
    }
}
