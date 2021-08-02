package com.mediatek.bluetooth;

import android.app.Application;  
import android.bluetooth.BluetoothHeadsetClient;  
import android.bluetooth.BluetoothDevice;

public class CarBTApplication extends Application{  
    private BluetoothHeadsetClient mHeadsetClient = null;
    private BluetoothDevice mBTDevice = null;
    public BluetoothHeadsetClient getBTClient(){  
        return this.mHeadsetClient;  
    }  
    public void setBTClient(BluetoothHeadsetClient c){  
        this.mHeadsetClient = c;  
    }  
    public BluetoothDevice getBTDevice(){  
        return this.mBTDevice;  
    }  
    public void setBTDevice(BluetoothDevice c){  
        this.mBTDevice = c;  
    }
    @Override  
    public void onCreate(){  
        super.onCreate();  
    }  
}  

