package com.mediatek.bluetooth;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.bluetooth.BluetoothHeadsetClientCall;
import android.bluetooth.BluetoothPbapClient;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.CallLog;
import android.support.annotation.NonNull;
import android.support.v4.app.Fragment;
import android.support.v4.app.LoaderManager;
import android.support.v4.content.CursorLoader;
import android.support.v4.content.Loader;
import android.util.AttributeSet;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Menu;
import android.view.View.OnClickListener;
import android.view.View.OnFocusChangeListener;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.SimpleAdapter;
import android.widget.TextView;
import android.widget.Toast;

//import com.mediatek.bluetooth.BluetoothProfileManager;
import com.android.vcard.VCardEntry;
//import com.android.vcard.VCardEntry.CallTime;
import com.android.vcard.VCardEntry.PhoneData;
import com.mediatek.bluetooth.pbapclient.BluetoothPbapClientManager;
import com.mediatek.bluetooth.pbapclient.BluetoothPabapClientCallback;
import com.mediatek.bluetooth.pbapclient.BluetoothPbapClientConstants;
import com.mediatek.bluetooth.R;
import com.mediatek.bluetooth.util.ActionItem;
import com.mediatek.bluetooth.util.QuickActions;
import com.mediatek.bluetooth.util.Utils;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.List;


public class CallHistoryFragment extends Fragment {

    protected static final String TAG = "CallHistoryFragment";
    Context mContext;
    private static final boolean DEBUG = true;

    private static final int MESSAGE_GET_CALLHISTORY = 0;
    private static final int MESSAGE_PULL_PHONEBOOK_SIZE = 1;
    private static final int MESSAGE_TOAST_PULL_PHONEBOOK_SIZE_FAIL = 2;
    private static final int MESSAGE_CLEAR_DATA = 3;
    private static final int MESSAGE_DISMISS_POP = 4;

    private static final int MESSAGE_DELAY = 1000;
    private static final int MAX_SET_PATH_TIMES = 10;
    public static final int ACTIVITY_RESULT_CODE = 100;

    private final static int MSG_SHOW_RECORDNUM = 10;
    private final static int MSG_NOTIFY_DATACHANGED = 11;

    private static final int CALLLOG_HISTORY_LOADER_ID = 1;

    private static final String[] CALLLOG_PROJECTION = {
        CallLog.Calls.NUMBER,
        CallLog.Calls.DATE,
        CallLog.Calls.TYPE,
        CallLog.Calls.CACHED_NAME
    };

    private static final int INCOMING_TYPE = 1;
    private static final int OUTGOING_TYPE = 2;
    private static final int MISSED_TYPE = 3;

    private static final int ALL_MODE = 0;
    private static final int INCOMING_MODE = 1;
    private static final int OUTGOING_MODE = 2;
    private static final int MISSED_MODE = 3;
    private int mCallModeSelected = ALL_MODE;

    private ListView mHistoryListView;

   // private SimpleAdapter mAdapter;
    private int mDownloadPath;
    private int mShowedRecordNum;
    private Button received_btn;
    private Button dialed_btn;
    private Button missed_btn;
    private Button history_all_btn;
    private Button callhistory_syn_btn;
    private Button callClear;

    private long mStartTime;
    private long mEndTime;

    private boolean mIsHfConnected = false;
    public static BluetoothDevice mConnectedDevice;
    private BluetoothPbapClient mService;
    public static int mPbapClientProfileState = BluetoothProfile.STATE_DISCONNECTED;

    private ActionItem action_call = new ActionItem();

    private BluetoothPbapClientManager mManager = BluetoothPbapClientManager.getInstance();
    private ArrayList<CallHistoryData> mCallHistoryList = new ArrayList<CallHistoryData>();
    private CallHistoryAdapter mAdapter = new CallHistoryAdapter();
    private String mTargetFolder = BluetoothPbapClientConstants.CCH_PATH;
    private int mSetPathFailedTimes = 0;
    private int mGetPhonebookSizeTimes = 0;

    private Handler mHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
        Log.d(TAG, "handleMessage msg.what-> " + msg.what);
            switch (msg.what) {
                case MESSAGE_GET_CALLHISTORY:
                    getCallLogHistory();
                    break;
                case MESSAGE_PULL_PHONEBOOK_SIZE:
                    if(!pullPhoneBookSize()){
                        Utils.dismissPopupWindow();
                        Utils.showShortToast(getActivity(),"pullphonebooksize failed");
                    }
                    break;
               case MESSAGE_CLEAR_DATA:
                    Log.d(TAG,"start clear");
                    mCallHistoryList.clear();
                    mAdapter.notifyDataSetChanged();
                    break;
                case MESSAGE_DISMISS_POP:
                    Utils.dismissPopupWindow();
                    break;
            }

        };
    };

    public CallHistoryFragment() {}

    public CallHistoryFragment(Context context) {
        mContext = context;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        if (adapter != null) {
            adapter.getProfileProxy(getActivity(), PbapClientServiceListener,
                    BluetoothProfile.PBAP_CLIENT);
        }
    }
    private final BluetoothProfile.ServiceListener PbapClientServiceListener =
            new BluetoothProfile.ServiceListener() {

        public void onServiceConnected(int profile, BluetoothProfile proxy) {
            Log.d(TAG, "PbapClientServiceListener,proxy is : " + proxy.toString());
            mService = (BluetoothPbapClient) proxy;
            Log.d(TAG, "PbapClientServiceListener,device size : " + mService.getConnectedDevices().size());
            if(mService.getConnectedDevices().size()>0){
                mConnectedDevice = mService.getConnectedDevices().get(0);
                mPbapClientProfileState = BluetoothProfile.STATE_CONNECTED;
                Log.d(TAG, "onServiceConnected, mPbapClientProfileState = " + mPbapClientProfileState);
            }

        }

        public void onServiceDisconnected(int profile) {
            Log.d(TAG, "PbapClientServiceListener disconnected profile is : " + profile);
            mConnectedDevice = null;
            mPbapClientProfileState = BluetoothProfile.STATE_DISCONNECTED;
        }
    };

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.bt_callhistory, container, false);
        Log.d(TAG, "onCreateView start");
        mHistoryListView = (ListView) view.findViewById(R.id.history_listview);
        received_btn = (Button)view.findViewById(R.id.btn_received);
        dialed_btn = (Button)view.findViewById(R.id.btn_dialed);
        missed_btn = (Button)view.findViewById(R.id.btn_missed);
        history_all_btn = (Button)view.findViewById(R.id.btn_history_all);
        callhistory_syn_btn = (Button)view.findViewById(R.id.bt_callhistory_syn);
        callClear = (Button)view.findViewById(R.id.btn_history_clean);

        received_btn.setOnClickListener(CallHistoryButtonOnclick);
        dialed_btn.setOnClickListener(CallHistoryButtonOnclick);
        missed_btn.setOnClickListener(CallHistoryButtonOnclick);
        history_all_btn.setOnClickListener(CallHistoryButtonOnclick);
        callhistory_syn_btn.setOnClickListener(CallHistoryButtonOnclick);
        callClear.setOnClickListener(CallHistoryButtonOnclick);

        mHistoryListView.setAdapter(mAdapter);
        mHistoryListView.setOnItemClickListener(mHistoryListClickListener);
        Log.d(TAG,"onCreateView end");
        return view;
    }



    @Override
    public void onResume() {
        Log.d(TAG,"onResume");
        if(mPbapClientProfileState == BluetoothProfile.STATE_DISCONNECTED) {
            Log.d(TAG,"BluetoothPbapClient not connected");
            cleanData();
        }
        Utils.dismissPopupWindow();
        registerReceiver();
        super.onResume();
    }

    @Override
    public void onStop() {
        Log.d(TAG, "onStop");
        Utils.dismissPopupWindow();
        super.onStop();
        clearCallLogLoader();
    }

    @Override
    public void onDestroy() {
        Log.d(TAG,"onDestroy");
        Utils.dismissPopupWindow();
        try{
            if(mReceiver != null){
                this.getActivity().unregisterReceiver(mReceiver);
            }
        }catch(IllegalArgumentException e){
            Log.e("TAG","IllegalArgumentException");
        }
        super.onDestroy();
    }

    OnClickListener CallHistoryButtonOnclick = new OnClickListener(){

        @Override
        public void onClick(View v) {
            if(mPbapClientProfileState == BluetoothProfile.STATE_DISCONNECTED){
                Log.d(TAG, "onClick, MainActivity.mConnectedDevice = " + MainActivity.mConnectedDevice);
                Log.d(TAG, "onClick, mConnectedDevice = " + mConnectedDevice);
                if(MainActivity.mConnectedDevice != null){
                   Utils.showPbapConnectDialog(getActivity(),MainActivity.mConnectedDevice);
                }else{
                   Log.d(TAG,"CallHistoryButtonOnclick device is null");
                   Utils.showShortToast(getActivity(),R.string.pbap_no_device_to_connect);
                }
                cleanData();
                return;
            }else if(mPbapClientProfileState == BluetoothProfile.STATE_CONNECTING){
                Utils.showShortToast(getActivity(),R.string.pbap_is_connecting);
                return;
            }

            switch (v.getId()) {

                case R.id.btn_received:
                    mCallHistoryList.clear();
                    mCallModeSelected = INCOMING_MODE;
                    Log.d(TAG, "Received");
                    mAdapter.notifyDataSetChanged();
                    Utils.showPopupWindow(getActivity(),CallHistoryFragment.this.getView(),
                        MainActivity.screenW,MainActivity.screenH);

                    received_btn.setBackgroundResource(R.drawable.bt_180_50_btn_disable_focused);
                    dialed_btn.setBackgroundResource(R.drawable.btn_180_50_bg);
                    missed_btn.setBackgroundResource(R.drawable.btn_180_50_bg);
                    history_all_btn.setBackgroundResource(R.drawable.btn_180_50_bg);

                    Message msg_received = mHandler.obtainMessage(MESSAGE_GET_CALLHISTORY);
                    mHandler.sendMessageDelayed(msg_received, MESSAGE_DELAY);

                    break;
                case R.id.btn_dialed:
                    mCallHistoryList.clear();
                    mCallModeSelected = OUTGOING_MODE;
                    Log.d(TAG, "Dial ");
                    mAdapter.notifyDataSetChanged();
                    Utils.showPopupWindow(getActivity(),CallHistoryFragment.this.getView(),
                        MainActivity.screenW,MainActivity.screenH);

                    dialed_btn.setBackgroundResource(R.drawable.bt_180_50_btn_disable_focused);
                    received_btn.setBackgroundResource(R.drawable.btn_180_50_bg);
                    missed_btn.setBackgroundResource(R.drawable.btn_180_50_bg);
                    history_all_btn.setBackgroundResource(R.drawable.btn_180_50_bg);

                    Message msg_dialed = mHandler.obtainMessage(MESSAGE_GET_CALLHISTORY);
                    mHandler.sendMessageDelayed(msg_dialed, MESSAGE_DELAY);

                    break;
                case R.id.btn_missed:

                    mCallHistoryList.clear();
                    mCallModeSelected = MISSED_MODE;
                    Log.d(TAG, "Missed");
                    mAdapter.notifyDataSetChanged();
                    Utils.showPopupWindow(getActivity(),CallHistoryFragment.this.getView(),
                        MainActivity.screenW,MainActivity.screenH);

                    mTargetFolder = BluetoothPbapClientConstants.MCH_PATH;
                    missed_btn.setBackgroundResource(R.drawable.bt_180_50_btn_disable_focused);
                    received_btn.setBackgroundResource(R.drawable.btn_180_50_bg);
                    dialed_btn.setBackgroundResource(R.drawable.btn_180_50_bg);
                    history_all_btn.setBackgroundResource(R.drawable.btn_180_50_bg);
                    Message msg_missed = mHandler.obtainMessage(MESSAGE_GET_CALLHISTORY);
                    mHandler.sendMessageDelayed(msg_missed, MESSAGE_DELAY);

                    break;
                case R.id.btn_history_all:
                    mCallHistoryList.clear();
                    mCallModeSelected = ALL_MODE;
                    Log.d(TAG, "All history");
                    mAdapter.notifyDataSetChanged();
                    Utils.showPopupWindow(getActivity(),CallHistoryFragment.this.getView(),
                        MainActivity.screenW,MainActivity.screenH);
                    mTargetFolder = BluetoothPbapClientConstants.CCH_PATH;

                    history_all_btn.setBackgroundResource(R.drawable.bt_180_50_btn_disable_focused);
                    received_btn.setBackgroundResource(R.drawable.btn_180_50_bg);
                    dialed_btn.setBackgroundResource(R.drawable.btn_180_50_bg);
                    missed_btn.setBackgroundResource(R.drawable.btn_180_50_bg);

                    Message msg_all = mHandler.obtainMessage(MESSAGE_GET_CALLHISTORY);
                    mHandler.sendMessageDelayed(msg_all, MESSAGE_DELAY);

                    break;
                case R.id.bt_callhistory_syn:
                    mCallModeSelected = ALL_MODE;
                    Utils.showPopupWindow(getActivity(),CallHistoryFragment.this.getView(),
                        MainActivity.screenW,MainActivity.screenH);
                    Message msg_syn = mHandler.obtainMessage(MESSAGE_GET_CALLHISTORY);
                    mHandler.sendMessageDelayed(msg_syn, MESSAGE_DELAY);

                    break;
                case R.id.btn_history_clean:
                    mCallHistoryList.clear();
                    mAdapter.notifyDataSetChanged();
                    if(MainActivity.mConnectedDevice != null){
                        Utils.disConnectPBAPClient(getActivity(),MainActivity.mConnectedDevice);
                        Utils.showShortToast(getActivity(),R.string.pbap_is_disconnected);
                    }
                    Utils.dismissPopupWindow();
                    break;
                default:
                    break;
            }

        }


    };

    private class CallHistoryAdapter extends BaseAdapter {

        @Override
        public int getCount() {
            if (mCallHistoryList != null) {
                return mCallHistoryList.size();
            } else {
                return 0;
            }
        }

        @Override
        public Object getItem(int position) {
            if (mCallHistoryList != null) {
                return mCallHistoryList.get(position);
            } else {
                return null;
            }
        }

        @Override
        public long getItemId(int position) {
            if (mCallHistoryList != null) {
                return position ;
            } else {
                return 0;
            }
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            synchronized (mCallHistoryList) {
                if (mCallHistoryList != null && position < mCallHistoryList.size()) {
                    ItemView view = null;
                    if (convertView != null && convertView instanceof ItemView) {
                        view = (ItemView) convertView;
                    } else {
                        view = new ItemView(getActivity());
                    }
                    String displayName = mCallHistoryList.get(position).getDisplayName();
                    view.getNameView().setText(displayName);

                    String number = mCallHistoryList.get(position).getNumbers();
                    if (number != null) {
                        view.getNumberView().setText(number);
                    }

                    long time = mCallHistoryList.get(position).getTime();
                    String date = Utils.getDateToString(time);
                    view.getCallTimeView().setText(date);

                    int callType = mCallHistoryList.get(position).getCallType();
                    String callTypeString = null;
                    if(callType == INCOMING_TYPE){
                       callTypeString = getActivity().getString(R.string.received_string);
                    }else if(callType == OUTGOING_TYPE){
                       callTypeString = getActivity().getString(R.string.dialed_string);
                    }else if(callType == MISSED_TYPE) {
                       callTypeString = getActivity().getString(R.string.missed_string);
                    }
                    view.getCallTypeView().setText(callTypeString);
                    return view;
                } else {
                    return null;
                }
            }
        }

    }

     private synchronized void checkFolderPath() {
        String currentFolder = mManager.getCurrentPath();
        Log.d(TAG, "checkFolderPath() started, mTargetFolder = " + mTargetFolder
                + ", mCurrentFolder = " + currentFolder);
        if (mTargetFolder == null) {
            Log.e(TAG, "[Error] mTargetFolder == null");
        } else if (mTargetFolder.startsWith(currentFolder) || currentFolder.isEmpty()) {
            Log.d(TAG, "[OK] mTargetFolder matched");
            if (!pullPhoneBookSize()) {
                Message message1 = mHandler.obtainMessage(MESSAGE_PULL_PHONEBOOK_SIZE,
                            MESSAGE_DELAY);
                mHandler.sendMessage(message1);
            }
        } else {
            Log.d(TAG, "mTargetFolder = " + mTargetFolder + ", mCurrentFolder = " + currentFolder);
            if (!mManager.setPhoneBookFolderRoot()) {
                if(this.getUserVisibleHint()){
                    Log.d(TAG, "setPhoneBookFolderRoot fail, reset it");
                    Message message = mHandler.obtainMessage(MESSAGE_GET_CALLHISTORY);
                    mHandler.sendMessageDelayed(message, MESSAGE_DELAY);
                }
            }
        }
    }

    private void pullPhoneBook(String pbName,int size) {
        pbName = mTargetFolder + ".vcf";
        Log.d(TAG, "pullPhoneBook pbName : " + pbName);
        if (mManager.pullPhoneBook(pbName, size, 0)) {
            Log.d(TAG, "pullPhoneBook start");
        } else {
            Utils.dismissPopupWindow();
            Utils.showShortToast(this.getActivity(),"pull phonebook error");
            Log.d(TAG, "pullPhoneBook error");            
        }
    }

    private boolean pullPhoneBookSize() {
        String pbName = null;
        pbName = mTargetFolder + ".vcf";
        return mManager.pullPhoneBookSize(pbName);
    }

    // The on-click listener for all devices in the ListViews
    private OnItemClickListener mHistoryListClickListener = new OnItemClickListener() {
        public void onItemClick(AdapterView<?> av, View v, int arg2, long arg3) {

            final String pbname;
            final String pbnumber;

            //HashMap<String, String> map = new HashMap<String, String>();
            pbname = mCallHistoryList.get(arg2).getDisplayName();
            String number = mCallHistoryList.get(arg2).getNumbers();
            if(number == null || number.isEmpty())
               return ;

            Intent intent = new Intent();
            intent.setClass(getActivity(), VcardEntryActivity.class);
            intent.putExtra(VcardEntryActivity.DISPLAY_NAME, pbname);
            intent.putExtra(VcardEntryActivity.PHONE_NUMBER, number);
            intent.putExtra(VcardEntryActivity.EXTRA_TARGET_FOLDER, mTargetFolder);
            Log.d(TAG,"currentfolderPath = " + mManager.getCurrentPath());
            startActivity(intent);

            return;

        }
    };

    private class ItemView extends LinearLayout {

        private TextView mNameView = null;
        private TextView mNumberView = null;
        private TextView mCalltimeView = null;
        private TextView mcallTypeView = null;
        
        public ItemView(Context context) {
            this(context, null);
        }

        public ItemView(Context context, AttributeSet attrs) {
            super(context, attrs);
            LayoutInflater inflator = (LayoutInflater) context
                    .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            View view = inflator.inflate(R.layout.phonebook_listitem, null);
            addView(view);
            initView(view);
        }

        private void initView(View view) {
            mNameView = (TextView) findViewById(R.id.item_phonebook_name);
            mNumberView = (TextView) findViewById(R.id.item_phonebook_number);
            mcallTypeView = (TextView)findViewById(R.id.item_phonebook_type);
            mCalltimeView = (TextView) findViewById(R.id.item_phonebook_calltime);
        }

        public TextView getNameView() {
            return mNameView;
        }

        public TextView getNumberView() {
            return mNumberView;
        }

        public TextView getCallTimeView() {
            return mCalltimeView;
        }

        public TextView getCallTypeView() {
            return mcallTypeView;
        }
    }


    private void registerReceiver(){
        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        filter.addAction(BluetoothPbapClient.ACTION_CONNECTION_STATE_CHANGED);
        getActivity().registerReceiver(mReceiver, filter);
    }

    // refresh the vcardlist after incoming call end
    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.v(TAG, "Received " + intent.getAction());

            String action = intent.getAction();
            if(action.equals(BluetoothDevice.ACTION_BOND_STATE_CHANGED)){
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                int bondState = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE, BluetoothDevice.ERROR);
                if(bondState == BluetoothDevice.BOND_NONE){
                    if(mManager == null || mManager.getDevice()== null)return;
                    if(device.getAddress().equals(mManager.getDevice().getAddress())){
                        cleanData();
                    }
                }

            }else if(action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)){
                int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,BluetoothAdapter.STATE_OFF);
                if(state == BluetoothAdapter.STATE_OFF){
                    Utils.dismissPbapConnectDialog();
                    if(mManager == null)return;
                    cleanData();
                }

            } else if (action.equals(BluetoothPbapClient.ACTION_CONNECTION_STATE_CHANGED)){
                int new_state = intent.getIntExtra(BluetoothProfile.EXTRA_STATE,
                    BluetoothProfile.STATE_DISCONNECTED);
                int prev_state = intent.getIntExtra(BluetoothProfile.EXTRA_PREVIOUS_STATE,
                    BluetoothProfile.STATE_DISCONNECTED);
                if (new_state == BluetoothProfile.STATE_CONNECTED){
                    mConnectedDevice = (BluetoothDevice)intent.getParcelableExtra(
                        BluetoothDevice.EXTRA_DEVICE);
                    mPbapClientProfileState = BluetoothProfile.STATE_CONNECTED;
                } else {
                    mConnectedDevice = null;
                    mPbapClientProfileState = BluetoothProfile.STATE_DISCONNECTED;
                }
            }

        }
    };

    public void cleanData(){
        Log.d(TAG, "cleanData");
        Message clear = mHandler.obtainMessage(MESSAGE_CLEAR_DATA,
                                MESSAGE_DELAY);
        mHandler.sendMessage(clear);
    }

    private void clearCallLogLoader(){
        Log.d(TAG, "clearCallLogLoader...");
        getLoaderManager().destroyLoader(CALLLOG_HISTORY_LOADER_ID);
    }
    private void getCallLogHistory(){
        clearCallLogLoader();
        Log.d(TAG, "getCallLogHistory start");
        // Fetch the phone number from the contacts db using another loader.
        getLoaderManager().initLoader(CALLLOG_HISTORY_LOADER_ID, Bundle.EMPTY,
            new LoaderManager.LoaderCallbacks<Cursor>() {
                @Override
                public Loader<Cursor> onCreateLoader(int id, Bundle args) {
                Log.d(TAG, "onCreateLoader");
                    return new CursorLoader(getContext(),
                        CallLog.Calls.CONTENT_URI, CALLLOG_PROJECTION,
                        null, null, null);
                }

                public void onLoadFinished(Loader<Cursor> loader, Cursor cursor) {
                    mCallHistoryList.clear();
                    Log.d(TAG, "onLoadFinished, begin calllog list size = " + mCallHistoryList.size());
                    while (cursor.moveToNext()) {
                        int timeColIdx = cursor.getColumnIndex(CallLog.Calls.DATE);
                        String time = cursor.getString(timeColIdx);
                        int numberColIdx = cursor.getColumnIndex(CallLog.Calls.NUMBER);
                        String number = cursor.getString(numberColIdx);
                        int typeColIdx = cursor.getColumnIndex(CallLog.Calls.TYPE);
                        String type = cursor.getString(typeColIdx);
                        //Log.d(TAG, "onLoadFinished, time = " + time);
                        //Log.d(TAG, "onLoadFinished, number = " + number);
                        String ext_number = "";
                        if (number.length() > 15){//05516531751182295
                            ext_number = number.substring(12);
                            number = number.substring(0, 12);
                        } else if (number.length() > 14){//653-175-1182295
                            ext_number = number.substring(10);
                            number = number.substring(0, 10);
                        }
                        //Log.d(TAG, "onLoadFinished, ext_number = " + ext_number);
                        //Log.d(TAG, "onLoadFinished, 2 number = " + number);
                        if (!ext_number.equals("")){
                            StringBuffer sb = new StringBuffer();
                            sb = sb.append(number.toString());
                            sb = sb.append(",");
                            sb = sb.append(ext_number.toString());
                            number = sb.toString();
                        }
                        //Log.d(TAG, "onLoadFinished, 3 number = " + number);
                        number = number.replace("-", "");
                        //Log.d(TAG, "onLoadFinished, type = " + type);

                        String displayName = Utils.getContactNameByNumber(mContext, number);
                        if (displayName == null){
                            displayName = number;
                        }
                        //Log.d(TAG, "onLoadFinished, displayName = " + displayName);

                        CallHistoryData callHistoryData = new CallHistoryData();
                        callHistoryData.setNumbers(number);
                        callHistoryData.setCallType(Integer.parseInt(type));
                        callHistoryData.setTime(Long.parseLong(time));
                        callHistoryData.setDisplayName(displayName);
                        if ((Integer.parseInt(type) == INCOMING_TYPE)
                            && (mCallModeSelected == INCOMING_MODE)){
                            mCallHistoryList.add(callHistoryData);
                        } else if ((Integer.parseInt(type) == OUTGOING_TYPE)
                            && (mCallModeSelected == OUTGOING_MODE)){
                            mCallHistoryList.add(callHistoryData);
                        } else if ((Integer.parseInt(type) == MISSED_TYPE)
                            && (mCallModeSelected == MISSED_MODE)){
                            mCallHistoryList.add(callHistoryData);
                        } else if (mCallModeSelected == ALL_MODE){
                            mCallHistoryList.add(callHistoryData);
                        }

                    }
                    Collections.sort(mCallHistoryList);
                    mAdapter.notifyDataSetChanged();
                    Log.d(TAG, "onLoadFinished, end calllog list size = " + mCallHistoryList.size());
                    Message msg = mHandler.obtainMessage(MESSAGE_DISMISS_POP);
                    mHandler.sendMessage(msg);
                }

                public void onLoaderReset(Loader loader) {}
            });
    }

    private class CallHistoryData implements Comparable<CallHistoryData>{
        private String mNumbers;
        private String mDisplayName;
        private int mCallType;
        private long mTime;

        public String getNumbers(){
            return mNumbers;
        }
        public void setNumbers(String numbers){
            mNumbers = numbers;
        }

        public String getDisplayName(){
            return mDisplayName;
        }

        public void setDisplayName(String displayName){
            mDisplayName = displayName;
        }
        public int getCallType(){
            return mCallType;
        }
        public void setCallType(int callType){
            mCallType = callType;
        }
        public long getTime(){
            return mTime;
        }
        public void setTime(long time){
            mTime = time;
        }


        @Override
        public int compareTo(@NonNull CallHistoryData other) {
            if (getTime() != other.getTime()){
                return (other.getTime() > getTime() ? 1 : -1);
            }
            return -1;
        }

    }

}

