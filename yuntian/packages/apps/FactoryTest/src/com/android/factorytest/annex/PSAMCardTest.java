package com.android.factorytest.annex;

import android.app.Activity;
import android.os.Bundle;
import android.os.SystemProperties;
import android.widget.TextView;
import android.widget.Toast;
import com.android.factorytest.UartCheckActivity;
import com.android.factorytest.R;
//yuntian longyao add
//Description:删除工模里面的PSAM卡测试
import android.view.View;
//yuntian longyao end
/**
 * PSAM卡测试
 */
public class PSAMCardTest extends UartCheckActivity {
    private TextView mChipTestTileTv,mCardTestTileTv;
    private TextView mChipTestResultTv,mCardTestResultTv;
    private boolean psamChipState,psamCardState;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
		setContentView(R.layout.activity_psamcard_test);
        super.onCreate(savedInstanceState);

        mChipTestTileTv = (TextView) findViewById(R.id.psam_chip_test_title);
        mChipTestResultTv = (TextView) findViewById(R.id.psam_chip_test_result);
        mCardTestTileTv = (TextView) findViewById(R.id.psam_card_test_title);
        mCardTestResultTv = (TextView) findViewById(R.id.psam_card_test_result);
    }

    @Override
    protected void onResume() {
        super.onResume();
        String chipTitle = getString(R.string.psam_chip_title);
        String cardTitle = getString(R.string.psam_card_title);
        mChipTestTileTv.setText(chipTitle);
        mCardTestTileTv.setText(cardTitle);
        //yuntian longyao add
        //Description:删除工模里面的PSAM卡测试
        if(false){
            mCardTestTileTv.setVisibility(View.GONE);
            mCardTestResultTv.setVisibility(View.GONE);
        }
        //yuntian longyao end
        
        boolean isUartReady = checkUartState();
        if(isUartReady) {
			doPSAMTest();
			}else{
				Toast.makeText(this, R.string.uart_service_not_ready, Toast.LENGTH_SHORT).show();
				}
    }
    
    @Override
    public void doOnUartStateSuccess() {
		doPSAMTest();
	}
    
    private void doPSAMTest() {
		psamChipState = getPSAMChipState();
			if(psamChipState){
				mChipTestResultTv.setText(getString(R.string.test_success));
				psamCardState = readPSAMSCardtate();
				//yuntian longyao add
				//Description:删除工模里面的PSAM卡测试
				if(psamCardState || false) {//yuntian longyao end
					mCardTestResultTv.setText(getString(R.string.test_success));
					setPassButtonEnabled(true);
					}else{
						mCardTestResultTv.setText(getString(R.string.test_fail));
						setPassButtonEnabled(false);
						}
				}else{
					mChipTestResultTv.setText(getString(R.string.test_fail));
					setPassButtonEnabled(false);
					}
		}
    
    private boolean getPSAMChipState(){
		String psamChipState = SystemProperties.get("yuntian.psam.test.uart");//读PSAM芯片属性
        boolean result = "ok".equals(psamChipState);
        return result;
	}

    private boolean readPSAMSCardtate(){
		String psamCardState = SystemProperties.get("yuntian.psamCard_test.uart");//读PSAM卡属性
        boolean result = "ok".equals(psamCardState);
        return result;
    }
    
    @Override
    protected void onPause() {
        super.onPause();
    }
}
