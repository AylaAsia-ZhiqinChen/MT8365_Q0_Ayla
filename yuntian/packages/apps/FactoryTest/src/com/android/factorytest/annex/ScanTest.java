package com.android.factorytest.annex;

import android.app.Activity;
import android.os.Bundle;
import android.os.SystemProperties;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.UartCheckActivity;
import com.android.factorytest.R;
//yuntian longyao add
//Description:扫描头测试改为直接调用第三方APP
import android.content.Intent;
import android.content.res.Resources;
//yuntian longyao end
import com.android.factorytest.Log;

/**
 * Scan扫描头测试
 */
public class ScanTest extends UartCheckActivity {
    private TextView mTestResultTileTv;
    private TextView mTestResultTv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
		setContentView(R.layout.activity_scan_test);
        super.onCreate(savedInstanceState);
        

        mTestResultTileTv = (TextView) findViewById(R.id.scan_test_title);
        mTestResultTv = (TextView) findViewById(R.id.scan_test_result);
        //yuntian longyao add
        //Description:扫描头测试改为直接调用第三方APP
        Resources res = getResources();
        if (res.getBoolean(R.bool.use_sdlgui_app)) {
            Intent intent = new Intent();
            intent.setClassName("com.example.sdlconsoletest", "com.example.sdlconsoletest.MainActivity");
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
			try {
				startActivity(intent);
			} catch (android.content.ActivityNotFoundException ex) {
				Toast.makeText(this, "no applications", Toast.LENGTH_LONG).show();
				Log.e(this, "no applications:", ex);
			}
            setPassButtonEnabled(true);
        }
        //yuntian longyao end
    }

    @Override
    protected void onResume() {
        super.onResume();
        //yuntian longyao add
        //Description:扫描头测试改为直接调用第三方APP
        Resources res = getResources();
        if (!res.getBoolean(R.bool.use_sdlgui_app)) {
            String title = getString(R.string.scan_info_title);
            mTestResultTileTv.setText(title);
            boolean isUartReady = checkUartState();
            if (isUartReady) {
                getScanState();
            } else {
                Toast.makeText(this, R.string.uart_service_not_ready, Toast.LENGTH_SHORT).show();
            }
        } else {
            mTestResultTileTv.setVisibility(android.view.View.GONE);
		}
        //yuntian longyao end
    }
    
    @Override
    public void doOnUartStateSuccess() {
		getScanState();
	}

    private void getScanState(){
		String scanState = SystemProperties.get("yuntian.scan.test.uart");
		android.util.Log.e("wxc", "getScanState--->scanState=" + scanState);
        boolean result = "ok".equals(scanState);
        
        if(result){
            mTestResultTv.setText(getString(R.string.test_success));
            setPassButtonEnabled(true);
        }else{
            mTestResultTv.setText(getString(R.string.test_fail));
            setPassButtonEnabled(false);
        }
    }
}
