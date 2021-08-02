package com.mediatek.bluetooth;


import com.mediatek.bluetooth.R;
import android.app.AlertDialog;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
//import com.mediatek.bluetooth.BluetoothProfileManager;
//import com.mediatek.bluetooth.BluetoothProfileManager.Profile;
import com.mediatek.bluetooth.common.LocalBluetoothManager;
import com.mediatek.bluetooth.common.LocalBluetoothProfileManager;
import com.mediatek.bluetooth.pbapclient.BluetoothPbapClientConstants;
import com.mediatek.bluetooth.pbapclient.BluetoothPbapClientManager;
import com.mediatek.bluetooth.pbapclient.BluetoothPabapClientCallback;
import android.bluetooth.BluetoothPbapClient;
import com.android.vcard.VCardEntry;
import com.android.vcard.VCardEntry.PhoneData;
//import android.bluetooth.client.pbap.BluetoothPbapCard;
import java.util.ArrayList;
import java.util.List;
import com.mediatek.bluetooth.util.Utils;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.content.DialogInterface;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.InputType;
import android.text.format.Time;
import android.view.MotionEvent;
import android.util.Log;
import android.view.View;
import android.view.KeyEvent;
import android.view.View.OnClickListener;
import android.view.Window;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;
import android.bluetooth.BluetoothHeadsetClient;
import android.bluetooth.BluetoothHeadsetClientCall;
import android.bluetooth.BluetoothProfile;
import android.media.AudioManager;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.telecom.TelecomManager;


public class PhoneCallActivity extends Activity implements OnClickListener{
    private static final String TAG = "PhoneCallActivity";
    public static final String DESTORY_MSG = "com.mtk.bluetooth.phonecallactivity.destory";
    private static final boolean DEBUG = Utils.D;
    private String m_strPhoneNum;
    private String m_strPhoneNum1;
    private String m_strPhoneName1;
    private String m_strPhoneNum2;
    private String m_strPhoneName2;
    private int callfrom;
    private boolean bIsWaitingCall = false ;
    private boolean bIsTwoCall = false ;
    private boolean bIsOnCalling = false;
    private boolean bIsOnAlert = false;
    private boolean bIsVoiceOut = true;
    private boolean bIsShowConfirm = false;
    private boolean mIsConferenceCall = false;
    private int mActiveCallChangeCount = 0;
    private EditText m_subcallnumber_et;
    private Editable m_subcallnumstr_edt;
    //private BluetoothHfAdapter m_hfadpter ;
    private TextView phonenumber_TV;
    private TextView phonenumber_TV2;
    private Button voiceSourceCar_Btn;
    private Button exchange_Btn;
    private TextView callstatus_TV;
    private int mstate;
    private int OldCallState;

    private BluetoothHeadsetClient mHeadsetClient = null;
    public static BluetoothDevice mConnectedDevice = null;
    private AudioManager mAudiomanager;
    private BluetoothHeadsetClientCall mActiveCall = null;
    private BluetoothHeadsetClientCall mwaitCall = null;
    private LocalBluetoothManager mLocalManager ;
    //add for pbap start
    private BluetoothPbapClientManager mManager;
    private final int PBAP_DISCONNECED =
        BluetoothPbapClientConstants.CONNECTION_STATE_DISCONNECTED;

    private static final byte MODE_INPUT_NUMBER = 0;//BluetoothPbapClient.SEARCH_ATTR_NUMBER;
    private static final int MAX_LIST_COUNT = 65535;
    private String PB_PATH = BluetoothPbapClientConstants.PB_PATH;
    private String SIM_PB_PATH = BluetoothPbapClientConstants.SIM_PB_PATH;
    private String mTargetFolder = PB_PATH;
    private final int MESSAGE_PULL_PB_VCARDLIST = 1;
    private final int MESSAGE_PULL_SIM_PB_VCARDLIST = 2;
    private final int MESSAGE_SET_DIALFRAGMENT_CALLSTATUS = 3;
    private final long HANDLER_DELAY = 100;
    //add for pbap end

    private static final String UNKOWN_PHONE_NUMBER = "unkown";
    private TelephonyManager mTelephonyManager;

    private  boolean isSoftkeyPadVisible   = false;
    private Context mContext;
    //private CarBTApplication app = null;

    private AlertDialog dialog;
    private int mPhoneState = 0;
    private PhoneStateListener mPhoneStateListener = new PhoneStateListener() {

        @Override
        public void onCallStateChanged(int state, String incomingNumber) {
            Log.d(TAG, "onCallStateChanged state ->" + state);
            mPhoneState = state;
            if(state == TelephonyManager.CALL_STATE_OFFHOOK){
                if(mHeadsetClient != null){
                    mHeadsetClient.terminateCall(mConnectedDevice,null);
                }
            }
        }
    };

    private Handler mHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            Log.d(TAG, "handleMessage msg.what->" + msg.what);
            Log.d(TAG, "handleMessage msg.what->" + m_strPhoneNum);
            switch (msg.what) {
                case MESSAGE_PULL_PB_VCARDLIST:
                    pullVcardList(PB_PATH,m_strPhoneNum);
                    break;
                case MESSAGE_PULL_SIM_PB_VCARDLIST:
                    pullVcardList(SIM_PB_PATH,m_strPhoneNum);
                    break;
                case MESSAGE_SET_DIALFRAGMENT_CALLSTATUS:
                    DialFragment.mLastCallState = -1;
                    break;
            }

        };
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
                setWindowFlag();
        Log.d(TAG,"++++++++onCreate++++++++++");
        Drawable drawable;
        Resources res = getResources();
        drawable = res.getDrawable(R.drawable.bt_call_bg_small);
        this.getWindow().setBackgroundDrawable(drawable);

        mContext = this;

        setContentView(R.layout.bt_calling_status);

        Intent intent= getIntent();
        m_strPhoneNum= intent.getStringExtra("PhoneNumber");
        m_strPhoneNum1 = m_strPhoneNum;
        m_strPhoneName1 = Utils.getContactNameByNumber(mContext,m_strPhoneNum);
        mConnectedDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
        Log.d(TAG, "onCreate mConnectedDevice = " + mConnectedDevice);
        callfrom = intent.getIntExtra("callfrom",0);

        initViews();
        if(callfrom == 2) {
            voiceSourceCar_Btn.setVisibility(View.VISIBLE);
        }else {
            voiceSourceCar_Btn.setVisibility(View.GONE);
        }

        Log.d(TAG,"callfrom =="+callfrom);
        if((callfrom == 1) && !bIsOnCalling){
            callstatus_TV.setText(R.string.str_income_call_status);
            Button anser_btn = (Button)findViewById(R.id.btn_answer);
            anser_btn.setVisibility(View.VISIBLE);
            anser_btn.setOnClickListener(this);
        }else if((callfrom == 0 && !bIsOnCalling)){
            callstatus_TV.setText(R.string.str_out_call_status);
        }else if(callfrom == 2) {
            callstatus_TV.setText(R.string.str_bt_call_active);
        }

        //add filter
        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        //filter.addAction(BluetoothProfileManager.ACTION_PROFILE_STATE_UPDATE);
        filter.addAction(BluetoothHeadsetClient.ACTION_CALL_CHANGED);
        filter.addAction(BluetoothHeadsetClient.ACTION_AUDIO_STATE_CHANGED);
        filter.addAction(BluetoothHeadsetClient.ACTION_CONNECTION_STATE_CHANGED);
        filter.addAction(BluetoothHeadsetClient.ACTION_RESULT);
        filter.addAction(LocalBluetoothProfileManager.ACTION_PROFILE_UPDATE);
        filter.addAction(DESTORY_MSG);
        this.registerReceiver(mBroadcastReceiver, filter);

        mAudiomanager = new AudioManager(this);
        // don't fisish when touch outside
        this.setFinishOnTouchOutside(false);
        mTelephonyManager = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);
        //Utils.wakeUpAndUnlock(this,true);
        //app = (CarBTApplication)getApplication();
        ((AudioManager)getSystemService(AUDIO_SERVICE)).requestAudioFocus(null,
                AudioManager.STREAM_RING, AudioManager.AUDIOFOCUS_GAIN_TRANSIENT);
        Log.d(TAG, "oncreate requset audio focus");
    }

    @Override
    protected void onResume() {
        Log.d(TAG,"===== onResume =====");
        mLocalManager = Utils.getLocalBtManager(this);
        try{
           mHeadsetClient =
               mLocalManager.getProfileManager().getHfpClientProfile().getHeadsetClientServer();
        }catch(NullPointerException e){
           Log.e(TAG, "NullPointerExcetion occured " + e);
        }
        /*
        if(mHeadsetClient != null && mConnectedDevice == null){
            mConnectedDevice = mHeadsetClient.getConnectedDevices()!=null &&
                mHeadsetClient.getConnectedDevices().size()>0 ?
                mHeadsetClient.getConnectedDevices().get(0) : null;
            Log.d(TAG, "onResume mConnectedDevice = " + mConnectedDevice);
            Log.d(TAG, "onResume mConnectedDevice size = " + mHeadsetClient.getConnectedDevices().size());

            if (mHeadsetClient.getConnectedDevices().size()>1){
                Log.d(TAG, "onResume mConnectedDevice 1 = " + mHeadsetClient.getConnectedDevices().get(1));

            }
        }*/
        //initPBAP(m_strPhoneNum);
        mTelephonyManager.listen(mPhoneStateListener, PhoneStateListener.LISTEN_CALL_STATE);

        Log.d(TAG,"onResume, DialFragment.mLastCallState = " + DialFragment.mLastCallState);
        if (DialFragment.mLastCallState == BluetoothHeadsetClientCall.CALL_STATE_ACTIVE){
            bIsOnCalling = true;
            Button anser_btn = (Button)findViewById(R.id.btn_answer);
            anser_btn.setVisibility(View.GONE);
            voiceSourceCar_Btn.setVisibility(View.VISIBLE);
            callstatus_TV.setText(R.string.str_bt_call_active);
        }
        if(mHeadsetClient != null){
            int audo_state = mHeadsetClient.getAudioState(mConnectedDevice);
            Log.d(TAG,"onResume, audo_state = " + audo_state);
            if(audo_state == BluetoothHeadsetClient.STATE_AUDIO_CONNECTED){
               phoneCallShowVoiceSource(true);
            }else{
               phoneCallShowVoiceSource(false);
            }
        }
        super.onResume();
    }

    @Override
    protected void onStart() {
        super.onStart();
        if (DEBUG)Log.d(TAG, "===== onStart =====");
    }

    @Override
    protected void onPause() {
        Log.d(TAG, "=====onPause =====");
        super.onPause();
    }
    @Override
    protected void onDestroy() {
        Log.d(TAG, "=====onDestroy =====");
        //Message msg_set_call = mHandler.obtainMessage(MESSAGE_SET_DIALFRAGMENT_CALLSTATUS);
        //            mHandler.sendMessageDelayed(msg_set_call,200);
        Intent intent = new Intent();
        intent.setAction(DESTORY_MSG);
        this.sendBroadcast(intent);
        //Utils.wakeUpAndUnlock(this,false);
        super.onDestroy();
        ((AudioManager)getSystemService(AUDIO_SERVICE)).abandonAudioFocus(null);
        Log.d(TAG, "onDestroy abandon audio focus");
        mConnectedDevice = null;

    }

    /*
    * Function name : BroadcastReceiver
    * Parameters:
    * Description : handle received messages
    */
    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        private int mCallState = 0;
        private int mBtCallsetupState = 0;
        @Override
        public void onReceive(Context context, Intent intent) {
            if(DEBUG) Log.v(TAG, "onReceive:action->" + intent.getAction());

            String action = intent.getAction();

            if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
                BluetoothDevice device = intent
                        .getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,
                        BluetoothAdapter.ERROR);
                //mManager.setBluetoothStateInt(state);
                if(state == BluetoothAdapter.STATE_TURNING_OFF ||
                    state == BluetoothAdapter.STATE_OFF){
                    DestroyActivity();
                    DialFragment.mLastCallState = -1;
                    bIsOnAlert = false;
                    mIsConferenceCall = false;
                    mActiveCallChangeCount = 0;
                 }
            }else if(action.equals(BluetoothHeadsetClient.ACTION_CALL_CHANGED)){
                BluetoothHeadsetClientCall changedCall =
                    (BluetoothHeadsetClientCall)intent.getParcelableExtra(
                        BluetoothHeadsetClient.EXTRA_CALL);
                Log.d(TAG,"changedCall.getNumber()= " + changedCall.getNumber());
                int call_state = changedCall.getState();
                if(DEBUG) Log.d(TAG,"ACTION_CALL_CHANGED state=" + call_state);
                if(mwaitCall != null) Log.d(TAG,"ACTION_CALL_CHANGED mwaitCall =" + mwaitCall.getId());
                if(mActiveCall != null) Log.d(TAG,"ACTION_CALL_CHANGED mActiveCall =" + mActiveCall.getId());
                if(changedCall != null) Log.d(TAG,"ACTION_CALL_CHANGED changedCall =" + changedCall.getId());
                Log.d(TAG,"changedCall.getDevice() = " + changedCall.getDevice());
                if(call_state == BluetoothHeadsetClientCall.CALL_STATE_INCOMING
                    && changedCall.getDevice().equals(mConnectedDevice)){
                    if(mPhoneState == TelephonyManager.CALL_STATE_OFFHOOK){
                        showConfirmCallDialog();
                        bIsShowConfirm = true;
                    }
                    mActiveCall = changedCall;
                    if (dialog == null && mwaitCall != null && mActiveCall != null){
                        bIsTwoCall = true;
                        bIsWaitingCall = false;
                    }
                    m_strPhoneNum1 = changedCall.getNumber();
                    m_strPhoneName1 = Utils.getContactNameByNumber(mContext,m_strPhoneNum1);
                    SetText(phonenumber_TV, m_strPhoneName1, m_strPhoneNum1);
                    Log.d(TAG, "CALL_STATE_INCOMING,bIsTwoCall = " + bIsTwoCall + " mwaitCall = " + mwaitCall);
                    if (!bIsTwoCall){
                        //add for pts test
                        Button anser_btn = (Button)findViewById(R.id.btn_answer);
                        anser_btn.setVisibility(View.VISIBLE);
                        voiceSourceCar_Btn.setVisibility(View.GONE);
                        callstatus_TV.setText(R.string.str_income_call_status);
                        Log.d(TAG, "BluetoothHeadsetClientCall.CALL_STATE_INCOMING");
                        Log.d(TAG, "m_strPhoneNum1 = " + m_strPhoneNum1);
                    } else if(dialog == null && mwaitCall != null && true == bIsTwoCall){
                        showNewComingCallDialog(changedCall);
                    }

                }else if(call_state == BluetoothHeadsetClientCall.CALL_STATE_TERMINATED){
                    if(bIsTwoCall){
                        setTextHide(changedCall);
                        mActiveCall = mwaitCall;
                        mwaitCall = null;
                        exchange_Btn.setVisibility(View.GONE);
                        bIsTwoCall = false;
                        bIsWaitingCall = false;
                    }else{
                        mActiveCall = null;
                        DestroyActivity();
                        bIsOnCalling = false;
                    }
                    bIsOnAlert = false;
                    mIsConferenceCall = false;
                    mActiveCallChangeCount = 0;
                    dissmissDialog();
                }else if(call_state == BluetoothHeadsetClientCall.CALL_STATE_ACTIVE){
                    bIsOnAlert = false;
                    if(bIsShowConfirm){
                        mTelephonyManager.endCall();
                        dissmissDialog();
                        bIsShowConfirm = false;
                    }
                    Log.d(TAG, "CALL_STATE_ACTIVE, bIsWaitingCall = " + bIsWaitingCall + " bIsTwoCall = " + bIsTwoCall);
                    bIsOnCalling = true;
                    if (bIsTwoCall && mActiveCall != null){
                        if (mActiveCall.getId() != changedCall.getId()){
                            mActiveCallChangeCount ++;
                            //when active calls changed more than 2,means confrence call
                            if (mActiveCallChangeCount >= 2){
                                mIsConferenceCall = true;
                            }
                        }
                    }

                    mActiveCall = changedCall;
                    setTextHigh(mActiveCall);
                    if(bIsWaitingCall && mwaitCall != null && !bIsTwoCall){
                        bIsTwoCall = true;
                        bIsWaitingCall = false;
                    }
                    if (!bIsTwoCall){
                        mwaitCall = null;
                        dissmissDialog();
                    }
                    Button anser_btn = (Button)findViewById(R.id.btn_answer);
                    anser_btn.setVisibility(View.GONE);
                    voiceSourceCar_Btn.setVisibility(View.VISIBLE);
                    callstatus_TV.setText(R.string.str_bt_call_active);
                    m_strPhoneNum1 = changedCall.getNumber();
                    m_strPhoneName1 = Utils.getContactNameByNumber(mContext,m_strPhoneNum1);
                    if (mIsConferenceCall){
                        exchange_Btn.setVisibility(View.GONE);
                        phonenumber_TV2.setVisibility(View.GONE);
                        SetText(phonenumber_TV, "Conference calls", null);
                    } else {
                        SetText(phonenumber_TV, m_strPhoneName1, m_strPhoneNum1);
                    }

                }else if(call_state == BluetoothHeadsetClientCall.CALL_STATE_HELD){
                    Log.d(TAG, "CALL_STATE_HELD, mwaitCall = " + mwaitCall + " bIsTwoCall = " + bIsTwoCall);
                    setTextHigh(mActiveCall);
                    if(bIsWaitingCall && mwaitCall != null && !bIsTwoCall){
                        Log.d(TAG,"mwaitCall != null");
                        bIsTwoCall = true;
                        bIsWaitingCall = false;
                    }
                    if (!bIsTwoCall){
                        mActiveCall = changedCall;
                        mwaitCall = mActiveCall;
                        m_strPhoneNum1 = changedCall.getNumber();
                        m_strPhoneName1 = Utils.getContactNameByNumber(mContext,m_strPhoneNum1);
                        SetText(phonenumber_TV, m_strPhoneName1, m_strPhoneNum1);
                    } else {
                        mwaitCall = changedCall;
                        m_strPhoneNum2 = changedCall.getNumber();
                        m_strPhoneName2 = Utils.getContactNameByNumber(mContext,m_strPhoneNum2);
                        if (!bIsOnAlert){
                            exchange_Btn.setVisibility(View.VISIBLE);
                        }
                        SetText(phonenumber_TV2, m_strPhoneName2, m_strPhoneNum2);
                        phonenumber_TV2.setVisibility(View.VISIBLE);
                    }
                    mActiveCallChangeCount = 0;
                    dissmissDialog();
                }else if(call_state == BluetoothHeadsetClientCall.CALL_STATE_WAITING){
                    bIsWaitingCall = true;
                    Log.d(TAG, "CALL_STATE_WAITING,bIsTwoCall = " + bIsTwoCall + " mwaitCall = " + mwaitCall);
                    if(dialog == null && mwaitCall != null && true == bIsTwoCall){
                        m_strPhoneNum2 = changedCall.getNumber();
                        m_strPhoneName2 = Utils.getContactNameByNumber(mContext,m_strPhoneNum2);
                        showNewComingCallDialog(changedCall);

                        exchange_Btn.setVisibility(View.VISIBLE);
                        SetText(phonenumber_TV2, m_strPhoneName2, m_strPhoneNum2);
                        phonenumber_TV2.setVisibility(View.VISIBLE);
                    }
                    mwaitCall = changedCall;
                } else if (call_state == BluetoothHeadsetClientCall.CALL_STATE_ALERTING){
                    if (mActiveCall != null && !bIsTwoCall && bIsOnCalling){
                        bIsWaitingCall = true;
                        mwaitCall = mActiveCall;
                    }
                    bIsOnAlert = true;
                    mActiveCall = changedCall;
                    m_strPhoneNum1 = changedCall.getNumber();
                    m_strPhoneName1 = Utils.getContactNameByNumber(mContext,m_strPhoneNum1);
                    SetText(phonenumber_TV, m_strPhoneName1, m_strPhoneNum1);
                }

            }else if(action.equals(BluetoothHeadsetClient.ACTION_AUDIO_STATE_CHANGED)){
                int state = intent.getIntExtra(BluetoothProfile.EXTRA_STATE,
                    BluetoothHeadsetClient.STATE_AUDIO_DISCONNECTED);
                Log.d(TAG,"audio state = "+ state);
                if(state == BluetoothHeadsetClient.STATE_AUDIO_CONNECTED){
                     boolean wbsSupported =
                        intent.getBooleanExtra(BluetoothHeadsetClient.EXTRA_AUDIO_WBS, false);
                    if(DEBUG) Log.d(TAG,"wbsSupported ="+wbsSupported);
                    phoneCallShowVoiceSource(true);
                }else if(state == BluetoothHeadsetClient.STATE_AUDIO_DISCONNECTED){
                    phoneCallShowVoiceSource(false);
                }

            }else if(action.equals(BluetoothHeadsetClient.ACTION_CONNECTION_STATE_CHANGED)){
                int state = intent.getIntExtra(BluetoothProfile.EXTRA_STATE,
                    BluetoothProfile.STATE_DISCONNECTED);
                if(DEBUG) Log.d(TAG,"state = "+ state);
                if(state != BluetoothProfile.STATE_CONNECTED){
                    DestroyActivity();
                    DialFragment.mLastCallState = -1;
                    mConnectedDevice = null;
                }

            }else if(action.equals(BluetoothHeadsetClient.ACTION_RESULT)){
                int result = intent.getIntExtra(BluetoothHeadsetClient.EXTRA_RESULT_CODE,
                    BluetoothHeadsetClient.ACTION_RESULT_ERROR);
                if(DEBUG)Log.d(TAG,"result ="+result);
                if(result != BluetoothHeadsetClient.ACTION_RESULT_OK){
                    DestroyActivity();
                }
            }else if(action.equals(LocalBluetoothProfileManager.ACTION_PROFILE_UPDATE)){
                mLocalManager = Utils.getLocalBtManager(PhoneCallActivity.this);
                mHeadsetClient =
                    mLocalManager.getProfileManager().getHfpClientProfile().getHeadsetClientServer();
                if(mHeadsetClient != null && mConnectedDevice == null){
                    mConnectedDevice = mHeadsetClient.getConnectedDevices().get(0);
                    Log.d(TAG, "ACTION_PROFILE_UPDATE mConnectedDevice = " + mConnectedDevice);
                }
            } else if (action.equals(DESTORY_MSG)){
                DestroyActivity();
            }
        }
    };

    public boolean addSubPhoneCallInputString(CharSequence str) {

        int index = m_subcallnumber_et.getSelectionStart();
        m_subcallnumstr_edt = m_subcallnumber_et.getEditableText();
        if(index < 0 || index >m_subcallnumstr_edt.length()){
            m_subcallnumstr_edt.append(str);
        }else {
            m_subcallnumstr_edt.insert(index ,str);
        }
        m_subcallnumber_et.setText(m_subcallnumstr_edt);
        m_subcallnumber_et.setSelection(index+1);

        return true;
    }


    private void DestroyActivity(){
        if(DEBUG) Log.i(TAG, "Destroy Activity ");
        finish();
        unregisterCallback();
        try{
            if(mBroadcastReceiver != null) {
                this.unregisterReceiver(mBroadcastReceiver);
            }
        }catch(IllegalArgumentException e){
            Log.e("TAG","IllegalArgumentException");
        }
    }
    private void setTextHigh(BluetoothHeadsetClientCall whichcall){
        if(DEBUG) Log.i(TAG, "setTextHigh whichcall state:" + whichcall.getState());
        if(whichcall.getState() == BluetoothHeadsetClientCall.CALL_STATE_ACTIVE
            || whichcall.getState() == BluetoothHeadsetClientCall.CALL_STATE_ALERTING
            || whichcall.getState() == BluetoothHeadsetClientCall.CALL_STATE_INCOMING){
            phonenumber_TV.setTextColor(android.graphics.Color.WHITE);
            phonenumber_TV2.setTextColor(android.graphics.Color.GRAY);
        } else {
            phonenumber_TV.setTextColor(android.graphics.Color.GRAY);
            phonenumber_TV2.setTextColor(android.graphics.Color.WHITE);
        }
    }
    private void setTextHide(BluetoothHeadsetClientCall whichcall){
        if(DEBUG) Log.i(TAG, "setTextHide whichcall state:" + whichcall.getState());
        if(whichcall.getState() == BluetoothHeadsetClientCall.CALL_STATE_TERMINATED){
            phonenumber_TV.setTextColor(android.graphics.Color.WHITE);
            phonenumber_TV2.setVisibility(View.GONE);
        }
    }
    private void phoneCallShowSoftkeyPad(boolean fgShow){
        Drawable drawable;
        Resources res = getResources();
        final LinearLayout softkeyPadLayout = (LinearLayout)findViewById(R.id.calling_softkeypad);
        if(fgShow){
            if(DEBUG) Log.e(TAG, "Show SoftkeyPad ");
            //drawable = res.getDrawable(R.drawable.bt_call_bg_big);
            softkeyPadLayout.setVisibility(View.VISIBLE);
            isSoftkeyPadVisible = true;

        }else{
            if(DEBUG) Log.e(TAG, "hide SoftkeyPad");
            //drawable = res.getDrawable(R.drawable.bt_call_bg_small);
            softkeyPadLayout.setVisibility(View.GONE);
            isSoftkeyPadVisible = false ;
        }
        //this.getWindow().setBackgroundDrawable(drawable);
    }


    private void phoneCallShowVoiceSource(boolean connected){

        if(DEBUG) Log.d(TAG, "connected == "+connected);
        if(!connected){
            voiceSourceCar_Btn.setBackgroundResource(R.xml.bg_call_phonesound_btn);
            voiceSourceCar_Btn.setText(R.string.str_bt_call_phonesound);

        }else{
            voiceSourceCar_Btn.setBackgroundResource(R.xml.bg_call_carsound_btn);
            voiceSourceCar_Btn.setText(R.string.str_bt_call_carsound);
        }
        voiceSourceCar_Btn.setEnabled(true);
    }

    public void onClick(View v) {
        String dtmf_code = null;
        Byte dtmf = null;
        boolean bsenddtmf = false;
        if(mHeadsetClient == null){
            Log.e(TAG,"mHeadsetClient is null");
            return;
        }
        switch (v.getId()) {
            case R.id.btn_hangup:
                Log.d(TAG,"+++onClick+++hangup");
                if(((callfrom == 1)/*||(callfrom == 0)*/) && !bIsOnCalling){// reject call
                    Log.d(TAG,"rejectCall mConnectedDevice = "+mConnectedDevice);
                    if(mHeadsetClient.rejectCall(mConnectedDevice)){
                        bIsOnCalling = false;
                        Log.d(TAG,"rejectCall incoming OK");
                    }else{
                        Log.e(TAG,"rejectCall failed");
                    }
                } else if (mActiveCall != null
                    && mActiveCall.getState() == BluetoothHeadsetClientCall.CALL_STATE_HELD){
                    if(mHeadsetClient.rejectCall(mConnectedDevice)){
                        bIsOnCalling = false;
                        Log.d(TAG,"hold state call,rejectCall incoming OK");
                    }else{
                        Log.e(TAG,"hold state call,rejectCall failed");
                    }
                } else {
                    Log.d(TAG,"before terminateCall mConnectedDevice = " + mConnectedDevice);
                    if(mHeadsetClient.terminateCall(mConnectedDevice, null)){
                        bIsOnCalling = false;
                        Log.d(TAG,"terminateCall callout OK");
                    }else{
                        Log.e(TAG,"terminateCall failed");
                    }
                }
                break;

            case R.id.btn_answer:
                Log.d(TAG,"+++onClick+++answer");
                if(mHeadsetClient.acceptCall(mConnectedDevice,0)){
                    bIsOnCalling = true;
                    bIsVoiceOut = true;
                    voiceSourceCar_Btn.setVisibility(View.VISIBLE);
                }
                if(bIsOnCalling){
                    v.setVisibility(View.GONE);
                }
                break;

            case R.id.btn_softkeypad:
                Log.d(TAG,"+++onClick+++softkeypad");
                phoneCallShowSoftkeyPad(!isSoftkeyPadVisible);
                break;

            case R.id.btn_voiceswitch_car:
                Log.d(TAG,"+++onClick+++voiceswitch");
                v.setEnabled(false);
                int audioState = mHeadsetClient.getAudioState(mConnectedDevice);
                if (audioState == BluetoothHeadsetClient.STATE_AUDIO_DISCONNECTED) {
                    mHeadsetClient.connectAudio(mConnectedDevice);
                    Log.d(TAG, "connectAudio " + mConnectedDevice);
                } else if (audioState == BluetoothHeadsetClient.STATE_AUDIO_CONNECTED) {
                    mHeadsetClient.disconnectAudio(mConnectedDevice);
                    Log.d(TAG, "disconnectAudio " + mConnectedDevice);
                }
                break;
            case R.id.btn_exchange:
                Log.d(TAG,"+++onClick+++btn_exchange");
                mHeadsetClient.holdCall(mConnectedDevice);
                mActiveCallChangeCount = 0;
                break;
            case R.id.btn_calling_zero:
                addSubPhoneCallInputString("0");
                dtmf_code = "0" ;
                bsenddtmf = true ;
                break;

            case R.id.btn_calling_one:
                addSubPhoneCallInputString("1");
                dtmf_code = "1" ;
                bsenddtmf = true ;
                break;

            case R.id.btn_calling_two:
                addSubPhoneCallInputString("2");
                dtmf_code = "2" ;
                bsenddtmf = true ;
                break;

            case R.id.btn_calling_three:
                addSubPhoneCallInputString("3");
                dtmf_code = "3" ;
                bsenddtmf = true ;
                break;

            case R.id.btn_calling_four:
                addSubPhoneCallInputString("4");
                dtmf_code = "4" ;
                bsenddtmf = true ;
                break;

            case R.id.btn_calling_five:
                addSubPhoneCallInputString("5");
                dtmf_code = "5" ;
                bsenddtmf = true ;
                break;

            case R.id.btn_calling_six:
                addSubPhoneCallInputString("6");
                dtmf_code = "6" ;
                bsenddtmf = true ;
                break;

            case R.id.btn_calling_seven:
                addSubPhoneCallInputString("7");
                dtmf_code = "7" ;
                bsenddtmf = true ;
                break;

            case R.id.btn_calling_eight:
                addSubPhoneCallInputString("8");
                dtmf_code = "8" ;
                bsenddtmf = true ;
                break;

            case R.id.btn_calling_nine:
                addSubPhoneCallInputString("9");
                dtmf_code = "9" ;
                bsenddtmf = true ;
                break;

            case R.id.btn_calling_asterisk:
                addSubPhoneCallInputString("*");
                dtmf_code = "*" ;
                bsenddtmf = true ;
                break;

            case R.id.btn_calling_pound:
                addSubPhoneCallInputString("#");
                dtmf_code = "#" ;
                bsenddtmf = true ;
                break;

            default:
                break;
        }

        if(bsenddtmf  && (dtmf_code != null) && (mHeadsetClient != null)){
            byte[] inputBytes = dtmf_code.getBytes();
            dtmf = new Byte(inputBytes[0]);
            mHeadsetClient.sendDTMF(mConnectedDevice,dtmf);
        }

    }

    private void initViews(){

        /* three way call  */
        ((Button) findViewById(R.id.btn_hangup)).setOnClickListener(this);
        ((Button) findViewById(R.id.btn_softkeypad)).setOnClickListener(this);

        ((Button) findViewById(R.id.btn_calling_zero)).setOnClickListener(this);
        ((Button) findViewById(R.id.btn_calling_one)).setOnClickListener(this);
        ((Button) findViewById(R.id.btn_calling_two)).setOnClickListener(this);
        ((Button) findViewById(R.id.btn_calling_three)).setOnClickListener(this);
        ((Button) findViewById(R.id.btn_calling_four)).setOnClickListener(this);
        ((Button) findViewById(R.id.btn_calling_five)).setOnClickListener(this);
        ((Button) findViewById(R.id.btn_calling_six)).setOnClickListener(this);
        ((Button) findViewById(R.id.btn_calling_seven)).setOnClickListener(this);
        ((Button) findViewById(R.id.btn_calling_eight)).setOnClickListener(this);
        ((Button) findViewById(R.id.btn_calling_nine)).setOnClickListener(this);
        ((Button) findViewById(R.id.btn_calling_asterisk)).setOnClickListener(this);
        ((Button) findViewById(R.id.btn_calling_pound)).setOnClickListener(this);
        voiceSourceCar_Btn = (Button)findViewById(R.id.btn_voiceswitch_car);
        voiceSourceCar_Btn.setOnClickListener(this);
        exchange_Btn = (Button)findViewById(R.id.btn_exchange);
        exchange_Btn.setOnClickListener(this);
        exchange_Btn.setVisibility(View.GONE);
        callstatus_TV = (TextView)findViewById(R.id.bt_calling_status_tv);
        phonenumber_TV = (TextView)findViewById(R.id.bt_calling_phone_number);
        phonenumber_TV2 = (TextView)findViewById(R.id.bt_calling_phone_number2);
        phonenumber_TV2.setVisibility(View.GONE);
        SetText(phonenumber_TV,m_strPhoneName1,m_strPhoneNum);
        m_subcallnumber_et = (EditText)findViewById(R.id.calling_input_et);
        /* hide system input keyboard */
        m_subcallnumber_et.setInputType(InputType.TYPE_NULL);
        m_subcallnumber_et.setText("");
    }

    @Override
    public boolean onKeyDown(int keyCode,KeyEvent event){
        //int i = getCurrentRingValue();
        switch(keyCode){
            case KeyEvent.KEYCODE_VOLUME_DOWN:
                Log.d(TAG, "onKeyDown, KEYCODE_VOLUME_DOWN");
                mAudiomanager.adjustStreamVolume(
                AudioManager.STREAM_VOICE_CALL,
                AudioManager.ADJUST_LOWER,
                AudioManager.FLAG_SHOW_UI);
                break;
            case KeyEvent.KEYCODE_VOLUME_UP:
                Log.d(TAG, "onKeyDown, KEYCODE_VOLUME_UP");
                mAudiomanager.adjustStreamVolume(
                AudioManager.STREAM_VOICE_CALL,
                AudioManager.ADJUST_RAISE,
                AudioManager.FLAG_SHOW_UI);
                break;
            }
            return true;
    }

    private boolean pullVcardList(String pbName,String searchVal){

        if (searchVal.isEmpty()) {
            searchVal = null;
        }
        if (mManager.getCurrentPath().length() > 3) {
            pbName = null;
        } else {
            pbName = mTargetFolder+".vcf" ;
        }
        byte order = 0;//BluetoothPbapClient.ORDER_BY_ALPHABETICAL;
        if(DEBUG) Log.d(TAG,"pullVcardList pbName ="+pbName+" ,searchVal="+searchVal);
        return mManager.pullVcardList(pbName, order, MODE_INPUT_NUMBER, searchVal,
                MAX_LIST_COUNT, 0);

    }

    private synchronized void checkFolder(String targetFolder){
        String currentFolder = mManager.getCurrentPath();
        if(DEBUG) Log.d(TAG,"targetFolder + currentFolder"+currentFolder+","+targetFolder);
            if (targetFolder == null) {
                return;
            } else if (targetFolder.equals(currentFolder)) {
                if(DEBUG)Log.d(TAG, "[OK] mTargetFolder matched");
                if(targetFolder == PB_PATH){
                    Log.d(TAG, "MESSAGE_PULL_PB_VCARDLIST");
                    Message msg_pb = mHandler.obtainMessage(MESSAGE_PULL_PB_VCARDLIST);
                    mHandler.sendMessageDelayed(msg_pb,HANDLER_DELAY);

                }else{
                    Message msg_sim = mHandler.obtainMessage(MESSAGE_PULL_PB_VCARDLIST);
                        mHandler.sendMessageDelayed(msg_sim,HANDLER_DELAY);
                }

            } else {
                if (targetFolder.startsWith(currentFolder) || currentFolder.isEmpty()) {
                    Log.d(TAG,"checkFolder  setPath");
                    String nextFolder = targetFolder.substring(currentFolder.length());
                    if (nextFolder.startsWith("/")) {
                        nextFolder = nextFolder.substring(1);
                    }
                    String[] folders = nextFolder.split("/");
                    nextFolder = folders[0];
                    if (!mManager.setPhoneBookFolderDown(nextFolder)) {
                        if(DEBUG)Log.d(TAG, "setPhoneBookFolderRoot fail");
                    }
                } else {
                    if (!mManager.setPhoneBookFolderRoot()) {
                        if(DEBUG)Log.d(TAG, "setPhoneBookFolderRoot fail, reset it");
                    }
                }
            }
    }

    private void initPBAP(String strPhoneNum){
        Log.d(TAG,"initPBAP()");
        m_strPhoneNum = strPhoneNum;
        mManager = BluetoothPbapClientManager.getInstance();
        Log.d(TAG,"mConnectedDevice = "+mConnectedDevice);
        if(mManager == null || (mConnectedDevice == null)){
            Log.d(TAG,"BluetoothPbapClientManager is null ");
            return;
        }
        mstate = mManager.getConnectState();
        Log.d(TAG,"mstate == "+mstate);
        if(mstate == BluetoothPbapClientConstants.CONNECTION_STATE_DISCONNECTED ||
             mstate == BluetoothPbapClientConstants.CONNECTION_STATE_DISCONNECTING ||
             !( mManager.getDevice().getAddress().equals(mConnectedDevice.getAddress()))){
            mManager.initConnect(mConnectedDevice);
            mManager.connectDevice(mConnectedDevice);

        }
        if(m_strPhoneNum != null){
            mTargetFolder = PB_PATH;
            checkFolder(mTargetFolder);
        }

    }

    private void unregisterCallback(){
    }

    private void SetText(TextView mTextView,String mStrNam,String mStrNum){
        Log.d(TAG,"SetText mStrNam : " + mStrNam + "  mStrNum = " + mStrNum);
        if(mStrNam == null || mStrNam.equals("")){
            mTextView.setText(mStrNum);
        }else{
            mTextView.setText(mStrNam);
        }
    }

    private void showNewComingCallDialog(final BluetoothHeadsetClientCall newCall){
        Log.d(TAG,"showNewComingCallDialog");
        if(dialog != null){
            Log.d(TAG,"showNewComingCallDialog ! = null");
            return;//dialog.dismiss();
        }
        dialog = new AlertDialog.Builder(this)
                    .setPositiveButton(R.string.dialog_end, new DialogInterface.OnClickListener(){
                        public void onClick(DialogInterface dialog, int which) {
                            Log.d(TAG,"dialog_end");
                            mHeadsetClient.acceptCall(mConnectedDevice,BluetoothHeadsetClient.CALL_ACCEPT_TERMINATE);
                            dissmissDialog();
                        }

                    })
                    .setNegativeButton(R.string.dialog_retain, new DialogInterface.OnClickListener(){
                        public void onClick(DialogInterface dialog, int which) {
                            Log.d(TAG,"dialog_retain");
                            mHeadsetClient.acceptCall(mConnectedDevice,BluetoothHeadsetClient.CALL_ACCEPT_HOLD);
                            //mHeadsetClient.rejectCall(mConnectedDevice);
                            exchange_Btn.setVisibility(View.VISIBLE);
                            m_strPhoneNum2 = newCall.getNumber();
                            m_strPhoneName2 = Utils.getContactNameByNumber(mContext,m_strPhoneNum2);
                            SetText(phonenumber_TV2,m_strPhoneName2,m_strPhoneNum2);
                            phonenumber_TV2.setVisibility(View.VISIBLE);
                            dissmissDialog();
                        }

                    })
                    .setNeutralButton(R.string.dialog_reject, new DialogInterface.OnClickListener(){
                        public void onClick(DialogInterface dialog, int which) {
                            Log.d(TAG,"dialog_reject");
                            mHeadsetClient.rejectCall(mConnectedDevice);
                            dissmissDialog();
                        }

                    }).create();
        dialog.setTitle(R.string.call_wait);
        dialog.setMessage(newCall.getNumber());
        Log.d(TAG,"showNewComingCallDialog start show");
        dialog.show();
        Log.d(TAG,"showNewComingCallDialog  show end");
    }

    private void showConfirmCallDialog(){
        Log.d(TAG,"showConfirmCallDialog");
        if(dialog != null){
            Log.d(TAG,"showConfirmCallDialog ! = null");
            return;//dialog.dismiss();
        }
        dialog = new AlertDialog.Builder(this)
                    .setPositiveButton(R.string.dialog_yes, new DialogInterface.OnClickListener(){
                        public void onClick(DialogInterface dialog, int which) {
                            Log.d(TAG,"dialog_yes");
                            mTelephonyManager.endCall();
                            bIsShowConfirm = false;
                            dissmissDialog();
                        }

                    })
                    .setNegativeButton(R.string.dialog_no, new DialogInterface.OnClickListener(){
                        public void onClick(DialogInterface dialog, int which) {
                            Log.d(TAG,"dialog_no");
                            mHeadsetClient.rejectCall(mConnectedDevice);
                            bIsShowConfirm = false;
                            dissmissDialog();
                        }

                    }).create();
        dialog.setTitle(R.string.call_hangup);
        dialog.show();
    }

    private void dissmissDialog(){
        if(DEBUG) Log.i(TAG, "dissmissDialog...");
        if(dialog != null){
            dialog.dismiss();
            dialog = null;
            //mwaitCall = null;
        }
    }

    private void updateCallInfo(BluetoothHeadsetClientCall comingCall){
        m_strPhoneNum= comingCall.getNumber();
        Log.d(TAG,"updateCallInfo m_strPhoneNum = "+m_strPhoneNum);
        if(m_strPhoneNum != null){
            mTargetFolder = PB_PATH;
            checkFolder(mTargetFolder);
        }

    }

    private void setWindowFlag(){
        int flags = WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED
                        | WindowManager.LayoutParams.FLAG_IGNORE_CHEEK_PRESSES
                        | WindowManager.LayoutParams.FLAG_TURN_SCREEN_ON
                        | WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD;

        final WindowManager.LayoutParams lp = getWindow().getAttributes();
        lp.flags |= flags;
        getWindow().setAttributes(lp);
    }

}
