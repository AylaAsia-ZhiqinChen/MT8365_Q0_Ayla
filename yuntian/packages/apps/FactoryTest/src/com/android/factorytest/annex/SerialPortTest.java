package com.android.factorytest.annex;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;
import com.android.factorytest.UartCheckActivity;
import com.android.factorytest.R;
import java.io.File;
import java.io.IOException;

import com.android.factorytest.serialport.SerialPortUtil;
import com.android.factorytest.serialport.SerialCallBack;
import com.android.factorytest.serialport.SerialPortCallBackUtils;
import com.android.factorytest.serialport.ByteUtil;

public class SerialPortTest extends UartCheckActivity implements SerialCallBack {
    private static final String TAG = "wxcSerialPortTest";
    private static final String Y6520_RS485_PATH = "/dev/y6520_power";
    private static final int HANDLE_MSG_SUC = 0;
    private static final int HANDLE_MSG_FAIL = 1;
    private static final int HANDLE_MSG_UPDATE_RECV = 2;
	private static final int HANDLE_MSG_SEND_RESULT = 3;

    private TextView mSendTitleTv,mRecvTitleTv,mResultTitleTv;
    private TextView mSendDataTv,mRecvDataTv,mResultTv;
    private String mSendStr;
    private boolean isOpen = false;
    private boolean hasSetPower = false;
    private boolean isReceiving = false;
    private int openFailCount = 0;
    private int recvErrorCount = 0;
    private int sendFailCount = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
		setContentView(R.layout.activity_serial_port_test);
        super.onCreate(savedInstanceState);
        
        initView();
        
		boolean isUartReady = checkUartState();
         if(isUartReady) {
			initSerialPort();
			}else{
				Toast.makeText(this, R.string.uart_service_not_ready, Toast.LENGTH_SHORT).show();
				}
    }
    
     @Override
    public void doOnUartStateSuccess() {
		initSerialPort();
	}

    @Override
    protected void onDestroy() {
        super.onDestroy();

		isReceiving = false;
        //关闭串口
        closeSerialPort();
    }

    private void initView() {
        mSendTitleTv = (TextView) findViewById(R.id.send_title_tv);
        mSendDataTv = (TextView) findViewById(R.id.send_data_tv);
        mRecvTitleTv = (TextView) findViewById(R.id.recv_title_tv);
        mRecvDataTv = (TextView) findViewById(R.id.recv_data_tv);
        mResultTitleTv = (TextView) findViewById(R.id.result_title_tv);
        mResultTv = (TextView) findViewById(R.id.result_tv);

        mSendTitleTv.setText(getString(R.string.send_data_tiltle));
        mRecvTitleTv.setText(getString(R.string.receive_data_tiltle));
        mResultTitleTv.setText(getString(R.string.serial_port_test_result));

    }

    private void initSerialPort() {
		Log.e(TAG, "initSerialPort-----...");
        //设置回调
        SerialPortCallBackUtils.setCallBack(this);
        
         //上电
         if(!hasSetPower){
			 setSerialPower(true);
			}
        if(!isOpen){
            //打开串口
            isOpen = SerialPortUtil.open("/dev/ttyS0", 2400, 0);
            if (isOpen) {
                Log.e(TAG, "initSerialPort--->open success!--->sendData...");
                //打开成功，立即发送数据
                sendData();
            } else {
                openFailCount = openFailCount + 1;
                Log.e(TAG, "initSerialPort--->open fail!--->initSerialPort again...openFailCount=" + openFailCount);
                if(openFailCount < 5){
                    initSerialPort();
                }else{
                    mResultTv.setText(getString(R.string.test_fail));
                    setTestResult(false);
                    closeSerialPort();
                }
            }
        }
    }
    
    private void setSerialPower(boolean enabled) {
        File file = new File(Y6520_RS485_PATH);
        java.io.FileWriter fr = null;
        try {
            fr = new java.io.FileWriter(file);
            if (enabled) {
				Log.e("wxcsetSerialPower", "setSerialPower=>write.yoyo.. ");
                fr.write("31");
                hasSetPower = true;
            } else {
                fr.write("50");
                hasSetPower = false;
            }
            fr.close();
            fr = null;
        } catch (IOException e) {
            Log.e("wxcsetSerialPower", "setSerialPower=>error: ", e);
        } finally {
            try {
                if (fr != null) {
                    fr.close();
                }
            } catch (IOException e) {}
        }
    }

    private void sendData() {
        mSendStr = ByteUtil.getRandomString(32);
        Log.e(TAG, "sendData --->mSendStr=" + mSendStr);
        mSendDataTv.setText(mSendStr);
        SerialPortUtil.sendString(mSendStr);
    }

    private void receiveData() {
		isReceiving = true;
        SerialPortUtil.receive();
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
    
    private void setTestResult(boolean result) {
		if(result) {
			mResultTv.setText(getString(R.string.test_success));
            setPassButtonEnabled(true);
			}else{
				mResultTv.setText(getString(R.string.test_fail));
				setPassButtonEnabled(false);
				}
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
        //Log.e(TAG, "send data to serial port result=" + sendResult);
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
					setTestResult(true);
					Toast.makeText(SerialPortTest.this, R.string.serial_port_test_success, Toast.LENGTH_SHORT).show();
					closeSerialPort();
					break;
				case HANDLE_MSG_FAIL:
					setTestResult(false);
					Toast.makeText(SerialPortTest.this, R.string.recv_error_string_five, Toast.LENGTH_SHORT).show();
					closeSerialPort();
					break;
				case HANDLE_MSG_UPDATE_RECV:
					String recvData = (String) msg.obj;
					mRecvDataTv.setText(recvData);
					String reverStr = ByteUtil.getReverseString(mSendStr);
					Log.e(TAG, "onSerialPortData --->reverStr=" + reverStr + " recvData=" + recvData);
					if((recvData != null) && recvData.equals(reverStr)){
						mHandler.sendEmptyMessage(HANDLE_MSG_SUC);
					}else{
						recvErrorCount = recvErrorCount + 1;
						//Log.e(TAG, "onSerialPortData--->recv diff string! ---->recvErrorCount=" + recvErrorCount);
						if(recvErrorCount < 5){
								Log.e(TAG, "onSerialPortData--->recv diff string! ---->recvErrorCount=" + recvErrorCount);
								//继续发送,至5次停止
								sendData();
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
							sendData();
                        }else{
                            //send fail 5 times
                            mResultTv.setText("fail");
                            setTestResult(false);
                            Toast.makeText(SerialPortTest.this, "Failed to send five times!",Toast.LENGTH_SHORT).show();
                            closeSerialPort();
                        }
                    }
                    break;
			}
		}
	};


}
