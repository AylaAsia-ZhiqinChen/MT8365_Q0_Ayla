package com.android.factorytest.serialport;

/**
 *
 */
public interface SerialCallBack {
    void onSerialPortData(String serialPortData);
    void sendSerialPortResult(int sendResult);
}
