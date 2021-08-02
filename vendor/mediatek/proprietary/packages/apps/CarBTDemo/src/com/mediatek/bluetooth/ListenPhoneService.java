package com.mediatek.bluetooth;


import android.content.Intent;
import android.app.Service;
import android.content.Context;
import android.os.IBinder;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.bluetooth.BluetoothHeadsetClient;  
import android.bluetooth.BluetoothDevice;
import android.util.Log;    
import com.mediatek.bluetooth.common.LocalBluetoothManager;
import android.content.Context;

public class ListenPhoneService extends Service
{   
    TelephonyManager tManager;
    private CarBTApplication app = null;
    private BluetoothHeadsetClient mHeadsetClient;
    private BluetoothDevice mBTDevice;
    private LocalBluetoothManager mServiceLocalManager ;
    private Context context;
    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }
    @Override
    public void onCreate()
    {
        Log.d("ListenPhoneService","onCreate");
        tManager = (TelephonyManager) getSystemService
                (Context.TELEPHONY_SERVICE);
        context = this;
            PhoneStateListener listener = new PhoneStateListener()
            {
                @Override
                public void onCallStateChanged(int state, String detail)
                {
                    Log.e("ListenPhoneService","state  = " + state);
                    switch (state)
                    {
                        case TelephonyManager.CALL_STATE_IDLE:
                            break;
                        case TelephonyManager.CALL_STATE_OFFHOOK:
                            break;

                        case TelephonyManager.CALL_STATE_RINGING:
                            break;
                        default:
                            break;
                    }
                    super.onCallStateChanged(state, detail);
                }
            };
            tManager.listen(listener, PhoneStateListener.LISTEN_CALL_STATE);
        }

     @Override
     public void onDestroy()
        {
            super.onDestroy();

        }
}


