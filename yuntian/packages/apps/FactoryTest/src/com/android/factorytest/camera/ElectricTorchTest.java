package com.android.factorytest.camera;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

public class ElectricTorchTest extends BaseActivity implements RadioGroup.OnCheckedChangeListener {
	private RadioGroup  mtorchGroup;
	private RadioButton openTorch;
	private RadioButton closeTorch;
	private Intent openIntent;
	private Intent closeIntent;
    private static final String YT_FLASHLIGHT_OPEN = "android.intent.action.flashlight.open";
    private static final String YT_FLASHLIGHT_CLOSE = "android.intent.action.flashlight.close";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_electric_torch_test);
        super.onCreate(savedInstanceState);
        mtorchGroup = (RadioGroup) findViewById(R.id.torch_test_radio_group);
		openTorch = (RadioButton)findViewById(R.id.turn_on_torch);
		closeTorch = (RadioButton)findViewById(R.id.turn_off_torch);
		mtorchGroup.setOnCheckedChangeListener(this);
		setPassButtonEnabled(true);
        openIntent = new Intent(YT_FLASHLIGHT_OPEN);
        openIntent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);
        openIntent.addFlags(Intent.FLAG_RECEIVER_FOREGROUND);
        openIntent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);

        closeIntent = new Intent(YT_FLASHLIGHT_CLOSE);
        closeIntent.addFlags(Intent.FLAG_RECEIVER_INCLUDE_BACKGROUND);
        closeIntent.addFlags(Intent.FLAG_RECEIVER_FOREGROUND);
        closeIntent.addFlags(Intent.FLAG_INCLUDE_STOPPED_PACKAGES);
    }

    @Override
    public void onCheckedChanged(RadioGroup group, int checkedId) {
		switch (checkedId){
            case R.id.turn_on_torch:
                sendBroadcast(openIntent);
            break;
            case R.id.turn_off_torch:
                sendBroadcast(closeIntent);
            break; 
        }
    }
    
    @Override
    protected void onDestroy() {
        sendBroadcast(closeIntent);
        super.onDestroy();
	}
}
