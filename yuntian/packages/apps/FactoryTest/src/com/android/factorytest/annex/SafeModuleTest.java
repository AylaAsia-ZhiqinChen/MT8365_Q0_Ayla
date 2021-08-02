package com.android.factorytest.annex;

import android.app.Activity;
import android.os.Bundle;
import android.os.SystemProperties;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.UartCheckActivity;
import com.android.factorytest.R;
/**
 * SafeModule测试
 */
public class SafeModuleTest extends UartCheckActivity {
    private TextView mTestResultTileTv;
    private TextView mTestResultTv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_safe_module_test);
        super.onCreate(savedInstanceState);

        mTestResultTileTv = (TextView) findViewById(R.id.safemodule_test_title);
        mTestResultTv = (TextView) findViewById(R.id.safemodule_test_result);

    }

    @Override
    protected void onResume() {
        super.onResume();
        String title = getString(R.string.safemodule_info_title);
        mTestResultTileTv.setText(title);
        boolean isUartReady = checkUartState();
         if(isUartReady) {
			getSafeModuleState();
			}else{
				Toast.makeText(this, R.string.uart_service_not_ready, Toast.LENGTH_SHORT).show();
				}
    }
    
    @Override
    public void doOnUartStateSuccess() {
		getSafeModuleState();
	}

    private void getSafeModuleState(){
		String safeModuleState = SystemProperties.get("yuntian.security.test.uart");
        boolean result = "ok".equals(safeModuleState);
        
        if(result){
            mTestResultTv.setText(getString(R.string.test_success));
            setPassButtonEnabled(true);
        }else{
            mTestResultTv.setText(getString(R.string.test_fail));
            setPassButtonEnabled(false);
        }
    }
}
