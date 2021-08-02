package com.android.factorytest.jni;

import android.util.Log;

public class Factorytestjni {

    public static native int batteryChargingCurrent(); //获取充电电流

    public static native int proximityOriginValue(); //获取距离传感器原始数据

    public static native int openCloseWhiteLed(boolean status);// 控制白色LED开关状态

    static {
        try {
            System.loadLibrary("Factorytest_jni");
        } catch (UnsatisfiedLinkError e) {
            Log.e("Factorytestjni", "Couldn't load lib:   - " + e.getMessage());
        }
    }
}
