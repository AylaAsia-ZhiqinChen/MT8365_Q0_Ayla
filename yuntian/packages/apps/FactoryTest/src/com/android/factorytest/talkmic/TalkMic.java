package com.android.factorytest.talkmic;

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
public class TalkMic extends BaseActivity {
private Button mTalkMicBtn;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_talk_mic);
        super.onCreate(savedInstanceState);
        setPassButtonEnabled(true);
        mTalkMicBtn = (Button) findViewById(R.id.first_btn);
        mTalkMicBtn.setOnClickListener(new View.OnClickListener() {
        @Override
        public void onClick(View v) {
                Intent intent = new Intent(Intent.ACTION_MAIN);
				ComponentName componentName = new ComponentName("net.wt.gate.dev", "net.wt.gate.dev.function.hide.TalkMicActivity");
				intent.setComponent(componentName);
				startActivity(intent);
			}
        });
        mTalkMicBtn.performClick();
    }
}
