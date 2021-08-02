package com.mediatek.bluetooth.common;

import java.util.List;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.Log;
import com.mediatek.bluetooth.CallHistoryFragment;
import com.mediatek.bluetooth.DialFragment;
import com.mediatek.bluetooth.MainActivity;
import com.mediatek.bluetooth.PhoneBookFragment;
import com.mediatek.bluetooth.PhoneCallActivity;

import android.bluetooth.BluetoothHeadsetClient;
import android.bluetooth.BluetoothHeadsetClientCall;
import android.bluetooth.BluetoothPbapClient;
import android.bluetooth.BluetoothProfile;

public class InComingCallRequest extends BroadcastReceiver {

    private static final String TAG = "InComingCallRequest";

    private LocalBluetoothProfileManager mManager;
    private BluetoothDevice mConnectedDevice;
    private static Context mContext;
    private static final String PHONECALL_DESTORY_ACTION = "com.mtk.bluetooth.phonecallactivity.destory";
    private static final int PHONECALL_DESTORY_MSG = 1;
    private static final int PHONECALL_DESTORY_MSG_DELAY = 500;

    private static final Handler mHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            Log.d(TAG, "handleMessage msg.what->" + msg.what);
            switch (msg.what) {
                case PHONECALL_DESTORY_MSG:
                    handlePhoneCallDestory();
                    break;
                default:
                    break;
            }

        };
    };

    private static void handlePhoneCallDestory(){
        Log.d(TAG, "handlePhoneCallDestory");
        Intent intent = new Intent();
        intent.setAction(PHONECALL_DESTORY_ACTION);
        mContext.sendBroadcast(intent);
    }

    private void hanleReceivedIntent(final Context context, final String cllNumber,
        final int callFrom) {

        Log.d(TAG, "hanleReceivedIntent...callFrom = " + callFrom);
        Runnable r = new Runnable() {
            @Override
            public void run() {

                Intent it = new Intent(context, PhoneCallActivity.class);
                it.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
                it.putExtra("callfrom", callFrom);
                it.putExtra("PhoneName", "");
                it.putExtra("PhoneNumber", cllNumber);
                it.putExtra(BluetoothDevice.EXTRA_DEVICE, mConnectedDevice);

                Log.d(TAG, "hanleReceivedIntent...run callFrom = " + callFrom);
                Log.d(TAG, "hanleReceivedIntent...run mConnectedDevice = " + mConnectedDevice);

                context.startActivity(it);
            }

        };

        new Thread(r).start();
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        mContext = context;
        String action = intent.getAction();
        Log.d(TAG, "onReceive action : " + action);
        if ((action == null) || ("".equals(action))) return;

        if (action.equals(BluetoothPbapClient.ACTION_CONNECTION_STATE_CHANGED)){
            int new_state = intent.getIntExtra(BluetoothProfile.EXTRA_STATE,
                                BluetoothProfile.STATE_DISCONNECTED);
            int prev_state = intent.getIntExtra(BluetoothProfile.EXTRA_PREVIOUS_STATE,
                BluetoothProfile.STATE_DISCONNECTED);
            if (new_state == BluetoothProfile.STATE_CONNECTED){
                PhoneBookFragment.mConnectedDevice = (BluetoothDevice)intent.getParcelableExtra(
                    BluetoothDevice.EXTRA_DEVICE);
                PhoneBookFragment.mPbapClientProfileState = BluetoothProfile.STATE_CONNECTED;
                CallHistoryFragment.mConnectedDevice = (BluetoothDevice)intent.getParcelableExtra(
                    BluetoothDevice.EXTRA_DEVICE);
                CallHistoryFragment.mPbapClientProfileState = BluetoothProfile.STATE_CONNECTED;
            } else {
                PhoneBookFragment.mConnectedDevice = null;
                PhoneBookFragment.mPbapClientProfileState = BluetoothProfile.STATE_DISCONNECTED;
                CallHistoryFragment.mConnectedDevice = null;
                CallHistoryFragment.mPbapClientProfileState = BluetoothProfile.STATE_DISCONNECTED;
            }
        } else if (action.equals(BluetoothHeadsetClient.ACTION_CALL_CHANGED)){
            BluetoothHeadsetClientCall call =
              (BluetoothHeadsetClientCall)intent.getParcelableExtra(BluetoothHeadsetClient.EXTRA_CALL);
            mConnectedDevice = call.getDevice();

            Log.d(TAG, "onReceive device : " + mConnectedDevice);

            Log.d(TAG, "ACTION_CALL_CHANGED BluetoothHeadsetClientCall state " + call.getState()
                     + " mLastCallState == " + DialFragment.mLastCallState);
            // incoming call
            if(call.getState()== BluetoothHeadsetClientCall.CALL_STATE_INCOMING){
                if (DialFragment.mLastCallState != BluetoothHeadsetClientCall.CALL_STATE_INCOMING){
                    Log.d(TAG, "onReceive PhoneCallActivity.mConnectedDevice : "
                        + PhoneCallActivity.mConnectedDevice);
                    if (PhoneCallActivity.mConnectedDevice == null
                            || ((PhoneCallActivity.mConnectedDevice != null)
                            &&(PhoneCallActivity.mConnectedDevice.equals(mConnectedDevice)))){
                        DialFragment.mLastCallState = BluetoothHeadsetClientCall.CALL_STATE_INCOMING;
                        Log.d(TAG, "ACTION_CALL_CHANGED invokePhoneCallActivity " + call.getNumber());
                        hanleReceivedIntent(context, call.getNumber(),1);
                    }
                }
            }else if(call.getState()== BluetoothHeadsetClientCall.CALL_STATE_DIALING ||
                    call.getState()== BluetoothHeadsetClientCall.CALL_STATE_ALERTING ) {
                if(DialFragment.mLastCallState == BluetoothHeadsetClientCall.CALL_STATE_INCOMING){
                     return;
                }
                if (DialFragment.mLastCallState != call.getState() ){
                    DialFragment.mLastCallState = call.getState();
                    Log.d(TAG, "ACTION_CALL_CHANGED invokePhoneCallActivity  " + call.getNumber());
                    hanleReceivedIntent(context, call.getNumber(),0);
                }
            }else if(call.getState()== BluetoothHeadsetClientCall.CALL_STATE_ACTIVE) {
                if (DialFragment.mLastCallState != call.getState()){
                    DialFragment.mLastCallState = call.getState();
                    hanleReceivedIntent(context, call.getNumber(),2);
                }
            } else if (call.getState() == BluetoothHeadsetClientCall.CALL_STATE_TERMINATED) {
                DialFragment.mLastCallState = -1;
                //PhoneCallActivity.mConnectedDevice = null;
            }
        }


    }

}
