package com.mediatek.bluetooth;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import com.mediatek.bluetooth.R;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothHeadsetClient;
import android.bluetooth.BluetoothHeadsetClientCall;
import android.bluetooth.BluetoothProfile;
import com.mediatek.bluetooth.util.Utils;
//import com.mediatek.bluetooth.BluetoothProfileManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.RemoteException;
import android.text.Editable;
import android.text.InputType;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.Toast;
import java.util.ArrayList;
import java.util.List;


public class DialFragment extends Fragment {
    private static final String TAG = "DialFragment";

    private static final boolean DEBUG = true;
    private Context mContext;
    EditText m_callnumber_et;
    Editable m_callnumstr_edt;
    String m_strMemoryCallNum;
    //public static BluetoothHfAdapter m_hfadpter;
    public static int mLastCallState = -1;
    BluetoothHeadsetClient mHeadsetClient = null;
    //BluetoothAdapter adapter = null;
    public Intent mHfConnect_finish_intent;
    private boolean mIsHfConnected = false;
    public  BluetoothDevice mConnectedDevice = null;

    private static final String UNKOWN_PHONE_NUMBER = "unkown";

    public static final String ACTION_BLUETOOTH_CALLOUT =
        "com.mediatek.CarBTDemo.DialFragment.action.BLUETOOTH_CALLOUT";
    public static final String EXTRA_BLUETOOTH_CALLOUT_NAME =
        "com.mediatek.CarBTDemo.DialFragment.extra.EXTRA_BLUETOOTH_CALLOUT_NAME";
    public static final String EXTRA_BLUETOOTH_CALLOUT_NUMBER =
        "com.mediatek.CarBTDemo.DialFragment.extra.EXTRA_BLUETOOTH_CALLOUT_NUMBER";
    private static BluetoothHeadsetClientCall redialed;
    private static BluetoothHeadsetClientCall dialed;

    private final int MESSAGE_REDIAL_TIME_OUT = 0;
    private final int REDIAL_TIME_OUT_DELAY = 4500;

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {

            Log.d(TAG, "handleMessage msg.what->" + msg.what);
            switch (msg.what) {
                case MESSAGE_REDIAL_TIME_OUT:
                    Utils.showShortToast(getActivity(),"Redial timeout,maybe you need dial first!");
                    redialed = null;
                    break;
                default:
                    break;
            }
        }
    };

    public DialFragment() {
    }

    public DialFragment(Context context) {
        mContext = context;
    }
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.d(TAG,"onCteate()");
        super.onCreate(savedInstanceState);
        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothHeadsetClient.ACTION_CALL_CHANGED);
        filter.addAction(BluetoothHeadsetClient.ACTION_AG_EVENT);
        filter.addAction(BluetoothHeadsetClient.ACTION_LAST_VTAG);
        filter.addAction(BluetoothHeadsetClient.ACTION_RESULT);

        getActivity().registerReceiver(mBroadcastReceiver, filter);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.bt_call, container, false);

        //TextView viewhello = (TextView) view.findViewById(R.id.textview);
        //viewhello.setText("DialFragment");
        ((Button) view.findViewById(R.id.btn_call)).setOnClickListener(new DialPadButtonOnclick());
        ((Button) view.findViewById(R.id.btn_recall)).setOnClickListener(new DialPadButtonOnclick());
        ((Button) view.findViewById(R.id.btn_call_zero)).setOnClickListener(new DialPadButtonOnclick());
        ((Button) view.findViewById(R.id.btn_call_one)).setOnClickListener(new DialPadButtonOnclick());
        ((Button) view.findViewById(R.id.btn_call_two)).setOnClickListener(new DialPadButtonOnclick());
        ((Button) view.findViewById(R.id.btn_call_three)).setOnClickListener(new DialPadButtonOnclick());
        ((Button) view.findViewById(R.id.btn_call_four)).setOnClickListener(new DialPadButtonOnclick());
        ((Button) view.findViewById(R.id.btn_call_five)).setOnClickListener(new DialPadButtonOnclick());
        ((Button) view.findViewById(R.id.btn_call_six)).setOnClickListener(new DialPadButtonOnclick());
        ((Button) view.findViewById(R.id.btn_call_seven)).setOnClickListener(new DialPadButtonOnclick());
        ((Button) view.findViewById(R.id.btn_call_eight)).setOnClickListener(new DialPadButtonOnclick());
        ((Button) view.findViewById(R.id.btn_call_nine)).setOnClickListener(new DialPadButtonOnclick());
        ((Button) view.findViewById(R.id.btn_call_asterisk)).setOnClickListener(new DialPadButtonOnclick());
        ((Button) view.findViewById(R.id.btn_call_pound)).setOnClickListener(new DialPadButtonOnclick());
        ((Button) view.findViewById(R.id.btn_clear_all_num)).setOnClickListener(new DialPadButtonOnclick());
        ((ImageButton) view.findViewById(R.id.btn_del_num)).setOnClickListener(new DialPadButtonOnclick());

        m_callnumber_et = (EditText)view.findViewById(R.id.text_call_info);
        /* hide system input keyboard */
        m_callnumber_et.setInputType(InputType.TYPE_NULL);

        m_callnumber_et.setText("");
        m_callnumstr_edt = m_callnumber_et.getText();
        m_strMemoryCallNum = null;

        if (DEBUG)
            Log.i(TAG, "onCreate finish mHeadsetClient :" + mHeadsetClient);
        return view;

    }
    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.d(TAG, "Received " + intent.getAction());

            String action = intent.getAction();

            if (action.equals(BluetoothHeadsetClient.ACTION_CALL_CHANGED)) {
                BluetoothHeadsetClientCall call =
                    (BluetoothHeadsetClientCall)intent.getParcelableExtra(BluetoothHeadsetClient.EXTRA_CALL);

                if(call.getState()== BluetoothHeadsetClientCall.CALL_STATE_ALERTING
                    || call.getState()== BluetoothHeadsetClientCall.CALL_STATE_DIALING){
                    mHandler.removeMessages(MESSAGE_REDIAL_TIME_OUT);
                    Log.d(TAG, "ACTION_CALL_CHANGED removeMessages, MESSAGE_REDIAL_TIME_OUT");
                }

            }else if(action.equals(BluetoothHeadsetClient.ACTION_RESULT)){
                int result = intent.getIntExtra(BluetoothHeadsetClient.EXTRA_RESULT_CODE,
                BluetoothHeadsetClient.ACTION_RESULT_ERROR);
                if(DEBUG)Log.d(TAG,"result ="+result);
                if(result != BluetoothHeadsetClient.ACTION_RESULT_OK){
                    redialed = null;
                    Utils.showShortToast(getActivity(),"Can't call out,please check input number or callhistory");
                }

            }else if(action.equals(BluetoothHeadsetClient.ACTION_AG_EVENT)){
                int result = intent.getIntExtra(BluetoothHeadsetClient.EXTRA_NETWORK_STATUS,0);
                if(result == 0){
                    dialed = null;
                }

            }

        }
    };

    /*
    * function name : invokePhoneCallActivity parameters: boolean
    * isIcommingCall ------ if it is incoming call set it as true ,or else if
    * it is outgoing call set it as false String strPhoneNumber ------ the
    * phone number ,if the number is not sure,set it as "unknown" Description :
    */
    public boolean invokePhoneCallActivity(boolean isIcommingCall,String strName, String strPhoneNumber) {

        Intent intentPhoneCall = new Intent();
        intentPhoneCall.setClass(getActivity(), PhoneCallActivity.class);
        intentPhoneCall.addCategory(Intent.CATEGORY_HOME);
        intentPhoneCall.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

        if (isIcommingCall)
            intentPhoneCall.putExtra("callfrom", 1);
        else
            intentPhoneCall.putExtra("callfrom", 0);
        intentPhoneCall.putExtra("PhoneName", strName);
        intentPhoneCall.putExtra("PhoneNumber", strPhoneNumber);
        intentPhoneCall.putExtra(BluetoothDevice.EXTRA_DEVICE, mConnectedDevice);
        startActivity(intentPhoneCall);

        return true;
    }

    /*
    * function name : getCallNumber Description :
    */
    private String getCallNumber() {
        m_callnumstr_edt = m_callnumber_et.getText();
        return m_callnumstr_edt.toString();
    }

    /*
    * function name : addDialPadInputString Description :
    */
    private boolean addDialPadInputString(CharSequence str) {

        if (str == null)
            return false;

        int index = m_callnumber_et.getSelectionStart();
        m_callnumstr_edt = m_callnumber_et.getText();
        if (index < 0 || index > m_callnumstr_edt.length()) {
            m_callnumstr_edt.append(str);
        } else {
            m_callnumstr_edt.insert(index, str);
        }
        m_callnumber_et.setText(m_callnumstr_edt);
        m_callnumber_et.setSelection(index + 1);

        return true;
    }

    /*
    * function name : delelteOneDialPadString Description :
    */
    private boolean delelteOneDialPadString() {

        int index = m_callnumber_et.getSelectionStart();
        m_callnumstr_edt = m_callnumber_et.getText();
        // Log.i(TAG, "delelteOneChar "+ "index="+ index);
        if (index >= 1) {
            m_callnumstr_edt.delete(index - 1, index);
        } else {
            return false;
        }
        m_callnumber_et.setText(m_callnumstr_edt);
        m_callnumber_et.setSelection(index - 1);
        return true;
    }

    private boolean delelteAllDialPadString() {
        m_callnumstr_edt = m_callnumber_et.getText();
        m_callnumstr_edt.clear();
        m_callnumber_et.setText(m_callnumstr_edt);
        return true;
    }


    private void ShowNoticeDeviceNotAvaible() {
        String notice_text = getString(R.string.str_bluetooth_phone_not_connected);
        Toast.makeText(getActivity(), notice_text, Toast.LENGTH_LONG).show();

    }

    private boolean checkHandsfreeDeviceConnected() {
        if (mIsHfConnected) {
            return true;
        } else {
            ShowNoticeDeviceNotAvaible();
            return false;
        }
    }

    public void setHeadsetClientProxy(BluetoothHeadsetClient proxy){
        Log.d(TAG, "setHeadsetClientProxy from:" + mHeadsetClient + ",to: " + proxy);
        mHeadsetClient = proxy;
    }

    public void setHeadsetClientDeviceState(BluetoothDevice device,boolean isConnected ){
        Log.d(TAG, "setHeadsetClientConnectedDevice device:" + device +",isConnected:" + isConnected);
        mConnectedDevice = device;
        mIsHfConnected = isConnected;
    }

    /*
    * (non-Javadoc)
    *
    * @see android.view.View.OnClickListener#onClick(android.view.View)
    */

    private class DialPadButtonOnclick implements OnClickListener{

        //private int position;

        public DialPadButtonOnclick() {
        //this.position = position;
        }

        @Override
        public void onClick(View v) {
            // Button Click Listener
            switch (v.getId()) {
                case R.id.btn_call:
                    if(mConnectedDevice == null || mHeadsetClient == null || !checkHandsfreeDeviceConnected()){
                        Log.d(TAG,"mHeadsetClient is null Or hfp not connected");
                        return;
                    }

                    m_strMemoryCallNum = getCallNumber();

                    Log.d(TAG,"m_strMemoryCallNum == " + m_strMemoryCallNum);
                    if(m_strMemoryCallNum.isEmpty()){
                        Utils.showShortToast(getActivity(),"please input dial number");
                        dialed = null;
                        return;
                    }
                    if(m_strMemoryCallNum.length()>40){
                        Utils.showShortToast(getActivity(),"It's not invalid number");
                        dialed = null;
                        return;
                    }
                    if(dialed != null){
                        Utils.showShortToast(getActivity(),"you have dial out,please wait");
                    }else {
                        dialed = mHeadsetClient.dial(mConnectedDevice,m_strMemoryCallNum);
                    }
                break;

                case R.id.btn_recall:
                    Log.d(TAG, "===== onClick ===== btn_recall mHeadsetClient :" + mHeadsetClient);
                    if(mConnectedDevice == null || mHeadsetClient == null|| !checkHandsfreeDeviceConnected()){
                        Log.d(TAG,"mHeadsetClient=null or HandsfreeDeviceConnected false");
                        return;
                    }
                    //add this code for redial when the phone call history is null.
                    Message msg = mHandler.obtainMessage(MESSAGE_REDIAL_TIME_OUT);
                    mHandler.sendMessageDelayed(msg, REDIAL_TIME_OUT_DELAY);

                    if(redialed != null){
                        Utils.showShortToast(getActivity(),"you have dial out,please wait");
                    }else {
                        redialed = mHeadsetClient.dial(mConnectedDevice,null);
                    }
                break;

                case R.id.btn_call_zero:
                    addDialPadInputString("0");
                break;

                case R.id.btn_call_one:
                    addDialPadInputString("1");
                break;

                case R.id.btn_call_two:
                    addDialPadInputString("2");
                break;

                case R.id.btn_call_three:
                    addDialPadInputString("3");
                break;

                case R.id.btn_call_four:
                    addDialPadInputString("4");
                break;

                case R.id.btn_call_five:
                    addDialPadInputString("5");
                break;

                case R.id.btn_call_six:
                    addDialPadInputString("6");
                break;

                case R.id.btn_call_seven:
                    addDialPadInputString("7");
                break;

                case R.id.btn_call_eight:
                    addDialPadInputString("8");
                break;

                case R.id.btn_call_nine:
                    addDialPadInputString("9");
                break;

                case R.id.btn_call_asterisk:
                    addDialPadInputString("*");
                break;

                case R.id.btn_call_pound:
                    addDialPadInputString("#");
                break;

                case R.id.btn_del_num:
                    delelteOneDialPadString();
                break;

                case R.id.btn_clear_all_num:
                    delelteAllDialPadString();
                break;

                default:
                break;
            }
        }
    }

    @Override
    public void onDestroy() {
        Log.d(TAG,"onDestroy()");
        try{
            if(mBroadcastReceiver != null){
                this.getActivity().unregisterReceiver(mBroadcastReceiver);
            }
        }catch(IllegalArgumentException e){
            Log.e("TAG","IllegalArgumentException");
        }
        super.onDestroy();
    }

    @Override
    public void onResume() {
        redialed = null;
        dialed = null;
        super.onResume();
    }

}
