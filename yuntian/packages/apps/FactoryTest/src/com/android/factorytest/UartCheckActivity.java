package com.android.factorytest;

import android.app.Activity;
import android.os.Handler;
import android.os.Message;

import android.os.Bundle;
import android.os.SystemProperties;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.R;
public class UartCheckActivity extends BaseActivity {
    private static final int HANDLE_MSG_NOT_DONE = 0;
    private static final int HANDLE_MSG_RUNNING = 1;
    private static final int HANDLE_MSG_RESTART = 2;
    private static final int HANDLE_MSG_RESTART_CHECK = 3;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
		//setContentView(R.layout.activity_uart_check);
        super.onCreate(savedInstanceState);

    }
    
    public void doOnUartStateSuccess() {
		
	}

    public boolean checkUartState() {
        String uartServiceOn = SystemProperties.get("yuntian.uart.test.done");
        boolean result = "1".equals(uartServiceOn);
        if(result){
			android.util.Log.e("wxcUartCheckActivity", "checkUartState....doOnUartStateSuccess...");
			doOnUartStateSuccess();
            return true;
        }else{
			mHandler.sendEmptyMessage(HANDLE_MSG_NOT_DONE);
        }
        return false;
    }

    private boolean isUartServiceRun() {
        String uartServiceRun = SystemProperties.get("init.svc.y6520_uart_test");
        boolean isRunning = "running".equals(uartServiceRun);
        return isRunning;
    }

    private void restartUartService() {
        SystemProperties.set("net.y6520.uart_test.restart", "1");
        mHandler.sendEmptyMessageDelayed(HANDLE_MSG_RESTART_CHECK, 10000);
    }

    Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case HANDLE_MSG_NOT_DONE:
                    boolean isUartServiceRun = isUartServiceRun();
                    if(isUartServiceRun){
                        mHandler.sendEmptyMessageDelayed(HANDLE_MSG_RUNNING, 1000);
                    }else{
                        mHandler.sendEmptyMessage(HANDLE_MSG_RESTART);
                    }
                    break;
                case HANDLE_MSG_RUNNING:
                    checkUartState();
                    break;
                case HANDLE_MSG_RESTART:
                    restartUartService();
                    break;
                case HANDLE_MSG_RESTART_CHECK:
                    checkUartState();
                    break;
            }
        }
    };


}
