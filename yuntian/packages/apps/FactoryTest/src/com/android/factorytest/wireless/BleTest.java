package com.android.factorytest.wireless;

import android.app.ActivityManager;
import android.os.Bundle;
import android.text.TextUtils;
import android.widget.TextView;
import android.os.SystemProperties;
import com.android.factorytest.BaseActivity;
import com.android.factorytest.R;
import android.util.Log;
import java.io.File;
import java.io.IOException;
import android.os.Handler;
import android.os.Message;

import com.android.factorytest.serialport.SerialPortUtil;
import com.android.factorytest.serialport.SerialCallBack;
import com.android.factorytest.serialport.SerialPortCallBackUtils;
import com.android.factorytest.serialport.ByteUtil;
/**
 * BLE测试
 */
public class BleTest extends BaseActivity implements SerialCallBack {
	private static final String TAG = "BleTest";
	private static final String POWER_PATH = "/dev/ttctl";
	private static final String SERIAL_PORT_PATH = "/dev/ttyS1";
	private static final String SEND_TEST="AT+B SPRO 10F\r";
	private static final String AT_PLUS_B="41542B42";  // AT+B ASIIC
	private static final String AT_MINUS_B="41542D42"; // AT-B ASIIC
	private static final String T_MINUS_B="542D42";    // T-B ASIIC
	private static final int BAUD_RATE = 115200;
	private static final int HANDLE_MSG_SUC = 0;
    private static final int HANDLE_MSG_FAIL = 1;
    private static final int HANDLE_MSG_UPDATE_RECV = 2;
	private static final int HANDLE_MSG_SEND_RESULT = 3;
	
	private boolean hasSetPower = false;
	private boolean isOpen = false;
	private boolean isReceiving = false;
	private int openFailCount = 0;
    private int recvErrorCount = 0;
    private int sendFailCount = 0;
    private TextView mBleStateTv;
	private String testOk;
	private String testFail;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_ble_test);

        super.onCreate(savedInstanceState);
        mBleStateTv = (TextView) findViewById(R.id.ble_state);
        testOk = getString(R.string.ble_test_result_ok);
        testFail = getString(R.string.ble_test_result_fail);
        initSerialPort();
    }

    @Override
    protected void onResume() {
        super.onResume();
        setTestCompleted(true);
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
		isReceiving = false;
        closeSerialPort();
    }
    
    private void setSerialPower(boolean enabled) {
        File file = new File(POWER_PATH);
        java.io.FileWriter fr = null;
        try {
            fr = new java.io.FileWriter(file);
            if (enabled) {
                fr.write("3");
                hasSetPower = true;
            } else {
                fr.write("4");
                hasSetPower = false;
            }
            fr.close();
            fr = null;
        } catch (IOException e) {
            Log.e(TAG, "setSerialPower=>error: ", e);
        } finally {
            try {
                if (fr != null) {
                    fr.close();
                }
            } catch (IOException e) {}
        }
    }

    private void closeSerialPort() {
		isReceiving = false;
		//关闭串口
        boolean isClose = SerialPortUtil.close();
        if (isClose) {
            Log.e(TAG, "close suc");
            isOpen = false;
        }
        //断电
        if(hasSetPower){
			setSerialPower(false);
		}
    }

    private void initSerialPort() {
		Log.d(TAG, "initSerialPort-----...");
        //设置回调
        SerialPortCallBackUtils.setCallBack(this);
         //上电
        if(!hasSetPower){
			 setSerialPower(true);
		}
        if(!isOpen){
            //打开串口
            isOpen = SerialPortUtil.open(SERIAL_PORT_PATH, BAUD_RATE, 0);
            if (isOpen) {
                Log.d(TAG, "initSerialPort--->open success!--->sendData...");
                //打开成功，立即发送数据
                SerialPortUtil.sendString(SEND_TEST);
            } else {
                openFailCount = openFailCount + 1;
                Log.e(TAG, "initSerialPort--->open fail!--->initSerialPort again...openFailCount=" + openFailCount);
                if(openFailCount < 5){
                    initSerialPort();
                }else{
                    mBleStateTv.setText(testFail);
                    setPassButtonEnabled(false);
                    closeSerialPort();
                }
            }
        }
    }
    
    private void receiveData() {
		isReceiving = true;
        SerialPortUtil.receive();
    }
    
    @Override
    public void onSerialPortData(String serialPortData) {
        Log.e(TAG, "recv from serial port:" + serialPortData);
        Message tempMsg = mHandler.obtainMessage();
        tempMsg.what = HANDLE_MSG_UPDATE_RECV;
        tempMsg.obj = serialPortData;
        mHandler.sendMessage(tempMsg);
    }

    @Override
    public void sendSerialPortResult(int sendResult) {
        Message tempMsg = mHandler.obtainMessage();
        tempMsg.what = HANDLE_MSG_SEND_RESULT;
        tempMsg.arg1 = sendResult;
        mHandler.sendMessage(tempMsg);
    }
       
    Handler mHandler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
				case HANDLE_MSG_SUC:
					setPassButtonEnabled(true);
					mBleStateTv.setText(testOk);
					closeSerialPort();
					break;
				case HANDLE_MSG_FAIL:
					setPassButtonEnabled(false);
					mBleStateTv.setText(testFail);
					closeSerialPort();
					break;
				case HANDLE_MSG_UPDATE_RECV:
					String recvData = (String) msg.obj;
					recvData = recvData.toUpperCase();
					Log.d(TAG, "onSerialPortData --->recvData=" + recvData);
					if((recvData != null) && (recvData.startsWith(AT_PLUS_B) || recvData.startsWith(AT_MINUS_B) || recvData.startsWith(T_MINUS_B))){
						mHandler.sendEmptyMessage(HANDLE_MSG_SUC);
					}else{
						recvErrorCount = recvErrorCount + 1;
						if(recvErrorCount < 5){
							Log.d(TAG, "onSerialPortData--->recv diff string! ---->recvErrorCount=" + recvErrorCount);
							//继续发送,至5次停止
							SerialPortUtil.sendString(SEND_TEST);
						}else {
							mHandler.sendEmptyMessage(HANDLE_MSG_FAIL);
						}
					}
					break;
				case HANDLE_MSG_SEND_RESULT:
                    int sendResult = msg.arg1;
                    if(sendResult == 0){
                        Log.e(TAG, "handleMessage--->HANDLE_MSG_SEND_RESULT--->send succ!--->receiveData...");
                        //发送数据成功之后，开始接收返回的数据
                        if(!isReceiving) {
							receiveData();
						}
                    }else{
                        sendFailCount = sendFailCount + 1;
                        //Log.e(TAG, "handleMessage--->HANDLE_MSG_SEND_RESULT--->send fail! sendFailCount=" + sendFailCount);
                        if (sendFailCount < 5){
							//发送失败，继续发送至5次
							SerialPortUtil.sendString(SEND_TEST);
                        }else{
                            //send fail 5 times
                            mBleStateTv.setText(testFail);
                            setPassButtonEnabled(false);
                            closeSerialPort();
                        }
                    }
                    break;
			}
		}
	};
	
}
