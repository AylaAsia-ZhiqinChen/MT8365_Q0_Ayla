package com.android.factorytest.serialport;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by Administrator on 2018/5/31.
 */

public class SerialPortUtil {

    public static String TAG = "wxcSerialPortUtil";

    /**
     * 标记当前串口状态(true:打开,false:关闭)
     **/
    public static boolean isFlagSerial = false;
    public static boolean isRecvStop = false;

    public static SerialPort serialPort = null;
    public static InputStream inputStream = null;
    public static OutputStream outputStream = null;
    public static Thread receiveThread = null;
    public static String strData = "";

    /**
     * 打开串口
     */
    public static boolean open(String pathname, int baudrate, int flags) {
		isRecvStop = false;
        boolean isopen = false;
        Log.e(TAG, "open--->isFlagSerial=" + isFlagSerial);
        if (isFlagSerial) {
            return false;
        }
        try {
            serialPort = new SerialPort(new File(pathname), baudrate, flags);
            //yuntian longyao add
            //Description:延时串口测试收发信息
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            //yuntian longyao end
            inputStream = serialPort.getInputStream();
            outputStream = serialPort.getOutputStream();
            isopen = true;
            isFlagSerial = true;
        } catch (IOException e) {
            e.printStackTrace();
            isopen = false;
        }
        Log.e(TAG, "open--->isFlagSerial=" + isFlagSerial + " isopen=" + isopen);
        return isopen;
    }

    /**
     * 关闭串口
     */
    public static boolean close() {
		isRecvStop = true;
        boolean isClose = false;
        if (isFlagSerial) {
            try {
                if (inputStream != null) {
                    inputStream.close();
                }
                if (outputStream != null) {
                    outputStream.close();
                }
                isClose = true;
                isFlagSerial = false;//关闭串口时，连接状态标记为false
            } catch (IOException e) {
                e.printStackTrace();
                isClose = false;
            }

        }
        return isClose;
    }

    /**
     * 发送串口指令
     */
    public static void sendString(String data) {
        Log.e(TAG, "sendString--->data=" + data);
        int sendResult = 1;
        if (!isFlagSerial) {
            return;
        }

        if(outputStream == null) {
            Log.e(TAG, "sendString--->outputStream == null");
            return;
        }
        try {
            Log.e(TAG, "sendString--->write...");
            outputStream.write(ByteUtil.hex2byte(data));
            outputStream.flush();
            sendResult = 0;
        } catch (IOException e) {
            Log.e(TAG, "sendString--->write...", e);
            e.printStackTrace();
            sendResult = 1;
        }
        //
        Log.e(TAG, "sendString--->sendResult=" + sendResult);
        SerialPortCallBackUtils.doCallBackSendResult(sendResult);
    }

    /**
     * 接收串口数据的方法
     */
    public static void receive() {
        Log.e(TAG, "receive--->isFlagSerial=" + isFlagSerial + " isRecvStop=" + isRecvStop);
        if (receiveThread != null && !isFlagSerial) {
            return;
        }
        receiveThread = new Thread() {
            @Override
            public void run() {
                while (isFlagSerial) {
                    Log.e(TAG, "receive--->run...");

                    try {
                        if (inputStream == null) {
                            return;
                        }
                        //yuntian longyao add
                        //Description:延时串口测试收发信息
                        try {
                            Thread.sleep(3000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        //yuntian longyao end

                        int availableSize = inputStream.available();
                        byte[] readData = new byte[availableSize];

                        Log.e(TAG, "receive--->run...inputStream.available()=" + availableSize);
                        while(inputStream.available() > 0){
                            Log.e(TAG, "receive--->read start.............");
                            int size = inputStream.read(readData);
                            if (size > 0) {
                                //strData = ByteUtil.ByteArrToHex(readData);
                                strData = ByteUtil.bytesToHexString(readData);
                                Log.e(TAG, "receive--->strData=" + strData + " size=" + size);
                                SerialPortCallBackUtils.doCallBackMethod(strData);
                            }else{
                                SerialPortCallBackUtils.doCallBackMethod(null);
                            }

                            //读的时候延迟500ms
                            try {
                                Thread.sleep(500);
                            } catch (InterruptedException e) {
                                e.printStackTrace();
                            }
                        }
                        Log.e(TAG, "receive--->read end............!");
                        //yuntian longyao add
                        //Description:延时串口测试收发信息
                        /*try {
                            //流中没有数据，延迟1s再继续执行
                            Thread.sleep(1000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }*/
                        //yuntian longyao add
						Log.e(TAG, "receive--->isFlagSerial=" + isFlagSerial + " isRecvStop=" + isRecvStop);
                        if(isRecvStop){
							break;
							}
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        };
        receiveThread.start();
    }
}
