package com.mediatek.bluetooth;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import com.mediatek.bluetooth.R;
import com.mediatek.bluetooth.common.*;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import com.mediatek.bluetooth.pbapclient.BluetoothPbapClientConstants;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Color;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;
import android.content.SharedPreferences;
import android.widget.ToggleButton;
import android.widget.EditText;
import android.content.DialogInterface;
import android.util.Log;
import com.mediatek.bluetooth.util.Utils;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothHeadsetClient;
import android.bluetooth.BluetoothA2dpSink;
import android.bluetooth.BluetoothHeadset;
import android.bluetooth.BluetoothA2dp;
import android.os.Handler;
import android.os.Message;
import android.widget.BaseAdapter;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import com.mediatek.bluetooth.common.BluetoothCallback;
import com.mediatek.bluetooth.common.BluetoothEventManager;
import com.mediatek.bluetooth.pbapclient.BluetoothPbapClientManager;

public class SettingsFragment extends Fragment implements BluetoothCallback{

    protected static final String TAG = "SettingsFragment";
    Context mContext;
    private static final boolean DEBUG = true;

    // if HF is not connected, could not download phonebook 
    private LocalBluetoothAdapter mLocalAdapter;
    private LocalBluetoothManager mLocalManager;
    private String devicename;

    private boolean mScanning;

    private ListView mDeviceListView;
    private CachedDevicesAdapter deviceAdapter = new CachedDevicesAdapter();

    private SimpleAdapter mPairedDevicesArrayAdapter;

    private List<CachedBluetoothDevice> mDeviceList; 

    // Local Bluetooth current Status
    private boolean mOpenBluetooth = false;

    private boolean mBluetoothIsDiscovery = true;

    private TextView mMsgTextView = null;

    public static String mPin = "1234";
    private String mCurName = null;
    private boolean isScaning = false;
    private boolean mIsAutoAnswer = false;
    private boolean mIsAutoConnect = false;
    private final String REMOTE_DEVICE_NAME = "remote_device_name";
    private final String REMOTE_DEVICE_STATUS = "remote_connect_status";
    private final String REMOTE_DEVICE_MACADDR = "remote_device_macaddr";
    private String mLastConnectedDeviceAddr = null;
    private BluetoothDevice mAVRCPConnectDevice = null;

    private EditText edt_devicename = null;
    private EditText pin = null;
    private AlertDialog dialog;

//    public static int mAvrcpState = BluetoothProfileManager.STATE_DISCONNECTED;

    private static final String SHARED_PREFERENCES_NAME = "bluetooth_settings";
    private IntentFilter intentFilter;
    private int updateTimes ;

    private BluetoothPbapClientManager mManager;

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            // //TODO: put this in callback instead of receiving
            String recievedAction = intent.getAction();
            if (DEBUG)
                Log.d(TAG, "onRecieve:action->" + recievedAction);

            if(recievedAction.equals(CachedBluetoothDevice.ACTION_PROFILE_STATE_CHANGED)||
                recievedAction.equals(LocalBluetoothProfileManager.ACTION_PROFILE_UPDATE)){

                deviceAdapter.notifyDataSetChanged();

            } else if(recievedAction.equals(BluetoothAdapter.ACTION_LOCAL_NAME_CHANGED)){
                handleDevicesNameChanged(intent);
            } else if(recievedAction.equals(BluetoothAdapter.ACTION_SCAN_MODE_CHANGED)){
                int mode = intent.getIntExtra(BluetoothAdapter.EXTRA_SCAN_MODE,
                                BluetoothAdapter.ERROR);
                Log.d(TAG,"mode ="+mode);
                if (mode != BluetoothAdapter.ERROR) {
                    handleModeChanged(mode);
                }
            }else if(recievedAction.equals(BluetoothDevice.ACTION_ACL_DISCONNECTED)){
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                byte reason = 0;
                boolean deviceConnected = false;

                for (int i = mDeviceList.size() - 1; i >= 0; i--) {
                    if (device.getAddress().equals(mDeviceList.get(i).getDevice().getAddress())) {
                        deviceConnected = mDeviceList.get(i).isConnected();
                    }
                }
                String s;
                reason = 0;//intent.getByteExtra(BluetoothDevice.EXTRA_DIS_REASON, reason);
                Log.d(TAG,"EXTRA_DIS_REASON =" + reason);
                if(reason == 0x13){
                    s = context.getResources().getString(R.string.dlg_mandis);
                    if (!deviceConnected)
                        showDisconnectDialog(context,s);
                }else if ((reason == 8) || (reason == 0x22)){
                    s = context.getResources().getString(R.string.dlg_autodis);
                    if (deviceConnected)
                        showDisconnectDialog(context,s);
                }
                //showDisconnectDialog(context,s);
                if(mManager == null || mManager.getDevice() == null) return;
                if(device.getAddress().equals(mManager.getDevice().getAddress())){
                    mManager.disconnectDevice();
                    mManager.setState(0);
                    Log.d(TAG,"acl disconnected,so reset pbapclient's state");
                }
            }
        }
    };


    static final int DEFAULT_DISCOVERABLE_TIMEOUT = 0; //NEVER TIME OUT

    static final String SHARED_PREFERENCES_KEY_DISCOVERABLE_END_TIMESTAMP =
            "discoverable_end_timestamp";

    private Button mOpenBluetoothButton;
    private Button mScanBluetoothButton;
    private Button mEditBluetoothButton;

    private ToggleButton mAutoConnectButton = null ;
    private ToggleButton mAutoAnswerButton = null ;
    private ToggleButton mVisibleButton = null;
    private LinearLayout mScanLayout = null;
    public SettingsFragment() {

    }

    public SettingsFragment(Context context) {
        mContext = context;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        receiverInit();
        mLocalManager = Utils.getLocalBtManager(mContext);
        mLocalManager.getEventManager().registerCallback(this);
        mLocalAdapter = mLocalManager.getBluetoothAdapter();
        mManager = BluetoothPbapClientManager.getInstance();
        this.getActivity().registerReceiver(mReceiver, intentFilter);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.bt_settings, container, false);

        if(DEBUG)Log.d(TAG, "+++ onCreateView +++");

        mDeviceListView = (ListView) view.findViewById(R.id.bluetooth_paired_devices);
        mDeviceListView.setOnItemClickListener(mDeviceClickListener);
        if (mLocalManager == null) {
            Log.d(TAG,"mLocalManager == null ");
            Utils.showShortToast(getActivity(), "Bluetooth is not available");
        }
        mDeviceList = (ArrayList)mLocalManager.getCachedDeviceManager().getCachedDevicesCopy();
        if(!mOpenBluetooth){
           mDeviceList.clear();
        }
        mDeviceListView.setAdapter(deviceAdapter);

        devicename = mLocalAdapter.getName();
        writeDeviceNamePin(devicename,mPin);
        mOpenBluetoothButton = (Button) view.findViewById(R.id.btn_open_bt);
        mEditBluetoothButton = (Button) view.findViewById(R.id.btn_edit_bt);
        mScanBluetoothButton = (Button) view.findViewById(R.id.btn_scan_bt);

        mVisibleButton = (ToggleButton)view.findViewById(R.id.btn_discovery_bt);
        mVisibleButton.setOnClickListener(settingsButtonOnclick);
        mScanLayout = (LinearLayout) view.findViewById(R.id.scan_device_layout);

        mAutoConnectButton = (ToggleButton)view.findViewById(R.id.btn_autoconnect_bt);
        mAutoConnectButton.setOnClickListener(settingsButtonOnclick);
        mAutoAnswerButton = ((ToggleButton) view.findViewById(R.id.btn_autoanswer_bt));
        mAutoAnswerButton.setOnClickListener(settingsButtonOnclick);
        mOpenBluetoothButton.setOnClickListener(
                settingsButtonOnclick);
        mEditBluetoothButton.setOnClickListener(
                settingsButtonOnclick);
        mScanBluetoothButton.setOnClickListener(
                settingsButtonOnclick);
        return view;
    }


    private void receiverInit(){
        intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothAdapter.ACTION_LOCAL_NAME_CHANGED);
        intentFilter.addAction(BluetoothAdapter.ACTION_SCAN_MODE_CHANGED);
        intentFilter.addAction(CachedBluetoothDevice.ACTION_PROFILE_STATE_CHANGED);
        //intentFilter.addAction(LocalBluetoothProfileManager.ACTION_PROFILE_UPDATE);
        intentFilter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
        this.getActivity().registerReceiver(mReceiver, intentFilter);
    }

    @Override
    public void onDestroy() {
        try{
            if(mReceiver != null){
                this.getActivity().unregisterReceiver(mReceiver);
            }
        }catch(IllegalArgumentException e){
           Log.e("TAG","IllegalArgumentException");
        }
        mLocalManager.getEventManager().unregisterCallback(this);
        super.onDestroy();
    }

    @Override
    public void onResume() {
        Log.d(TAG,"onResume");
        checkBluetoothStatus();
        refreshDataList();
        super.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mLocalManager == null) {
            return;
        }
        if(mLocalAdapter.isDiscovering()){
            scanDevice(true);
        }
        Log.d(TAG, "onPause ");
    }

    @Override
    public void onStop() {
        scanDevice(true);
        super.onStop();
    }

    private void scanDevice(final boolean enable) {
        if(DEBUG)Log.d(TAG,"scanDevice enable =="+enable);
        if (enable) {
            // Stops scanning after a pre-defined scan period.
            mLocalAdapter.stopScanning();
            mScanLayout.setVisibility(View.GONE);
        } else {
            mDeviceList.clear();
            mLocalManager.getCachedDeviceManager().clearNonBondedDevices(); 
            refreshDataList();
            mLocalAdapter.startScanning(true);
        }        
    }

    OnClickListener settingsButtonOnclick = new OnClickListener(){

        @Override
        public void onClick(View v) {
            switch (v.getId()) {
            case R.id.btn_edit_bt:
                showEditDialog();
                break;

            case R.id.btn_open_bt:
                if (mOpenBluetooth) {
                    Log.d(TAG, "set local bluetooth adapter off");

                    // mLocalManager.setBluetoothEnabled(false);
                    mLocalAdapter.setBluetoothEnabled(false);
                    mLocalAdapter.cancelDiscovery();
                    mScanLayout.setVisibility(View.GONE);
                } else {
                    Log.d(TAG, "set local bluetooth adapter on");
                    //mLocalManager.setBluetoothEnabled(true);
                    mLocalAdapter.setBluetoothEnabled(true);
                }

                break;

            case R.id.btn_discovery_bt:

                bluetoothDiscoverability();

                break;

            case R.id.btn_scan_bt:

                if (mOpenBluetooth) {
                    if (DEBUG)Log.d(TAG, "bluetooth startScanning");
                    scanDevice(isScaning);
                } else {
                    Toast.makeText(getActivity(), "Open Bluetooth first",
                    Toast.LENGTH_LONG).show();
                }

                break;
            case R.id.btn_autoanswer_bt:
                if (mIsAutoAnswer == true) {
                    if (DEBUG)
                        Log.d(TAG, "AutoAnswerToggleButton is Checked");
                    mIsAutoAnswer = false;
                    writeAutoAnswerData(mIsAutoAnswer);
                    mAutoAnswerButton.setChecked(false);
                } else {
                    if (DEBUG)
                        Log.d(TAG, "AutoAnswerToggleButton is not Checked");
                    mIsAutoAnswer = true;
                    writeAutoAnswerData(mIsAutoAnswer);
                    mAutoAnswerButton.setChecked(true);
                }
                break;
            case R.id.btn_autoconnect_bt:
                readDeviceNamePin();
                if (mIsAutoConnect == false) {
                    mIsAutoConnect = true;
                    if (DEBUG)
                        Log.d(TAG, "set autoconnect");
                    writeAutoConnectData(mIsAutoConnect);
                    mAutoConnectButton.setChecked(true);
                } else if (mIsAutoConnect == true) {
                    mIsAutoConnect = false;
                    if (DEBUG)
                        Log.d(TAG, "unset autoconnect");
                    writeAutoConnectData(mIsAutoConnect);
                    mAutoConnectButton.setChecked(false);
                }
                break;

            default:
                break;
            }

        }
    };

    private void checkBluetoothStatus() {
        if (mLocalManager.getBluetoothAdapter().isEnabled()) {
            mOpenBluetooth = true;
            mOpenBluetoothButton.setText(R.string.close_bt_string);
            mVisibleButton.setEnabled(true);
            mEditBluetoothButton.setEnabled(true); 
            mScanBluetoothButton.setEnabled(true);
            mAutoAnswerButton.setChecked(true);
            mAutoConnectButton.setChecked(true);
        } else {
            mOpenBluetooth = false;
            mOpenBluetoothButton.setText(R.string.open_bt_string);
            mVisibleButton.setEnabled(false);
            mEditBluetoothButton.setEnabled(false);
            mScanBluetoothButton.setEnabled(false);
            mAutoAnswerButton.setChecked(false);
            mAutoConnectButton.setChecked(false);
        }
        int scanMode = mLocalAdapter.getScanMode();

        handleModeChanged(scanMode);

        //read AutoConnectData, set the AutoConnect Button
        readAutoConnectData();
        mAutoConnectButton.setChecked(mIsAutoConnect);

        readAutoAnswerData();
        mAutoAnswerButton.setChecked(mIsAutoAnswer);
        Log.d(TAG, "AutoConnect= " + mIsAutoConnect + " IsAutoAnswer =" + mIsAutoAnswer);
    }

    private void bluetoothDiscoverability() {
        if (!mBluetoothIsDiscovery) {
            int timeout = DEFAULT_DISCOVERABLE_TIMEOUT;

            if (mLocalAdapter.setScanMode(BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE,
                    timeout)) {
                setDiscoveryModeStatus(true);
                if (DEBUG)
                    Log.d(TAG, "setScanMode : true");
            } else {
                setDiscoveryModeStatus(false);
                if (DEBUG)
                    Log.d(TAG, "setScanMode : false 1");
            }

        } else {
            mLocalAdapter.setScanMode(BluetoothAdapter.SCAN_MODE_CONNECTABLE);
            setDiscoveryModeStatus(false);
            if (DEBUG)
                Log.d(TAG, "setScanMode : false 2");
        }
    }

    private void setDiscoveryModeStatus(boolean isDiscoveryable) {
        mBluetoothIsDiscovery = isDiscoveryable;
        Log.d(TAG,"isDiscoveryable =="+isDiscoveryable);
        mVisibleButton.setChecked(isDiscoveryable);

    }

    // The on-click listener for all devices in the ListViews
    private OnItemClickListener mDeviceClickListener = new OnItemClickListener() {
        public void onItemClick(AdapterView<?> av, View v, int position, long arg3) {
            scanDevice(true);
            if (DEBUG)
                Log.d(TAG, "onItemClick position = " + position);

            CachedBluetoothDevice mDeviceInfo= mDeviceList.get(position);
            BluetoothDevice device = mDeviceInfo.getDevice();
            Log.d(TAG, "onItemClick mDeviceInfo = " + mDeviceInfo);
            Log.d(TAG, "onItemClick device = " + device);
            invokePairConnectActivity(device);
        }
    };

    public void handleStateChanged(int state) {
        if (DEBUG)
            Log.d(TAG, "handleStateChanged:state->" + state);
        switch (state) {
        case BluetoothAdapter.STATE_TURNING_ON:
            mOpenBluetoothButton.setText(R.string.open_bt_string);
            mOpenBluetoothButton.setEnabled(false);
            setButtonClickable(false);
            mOpenBluetooth = false;
            break;
        case BluetoothAdapter.STATE_ON:
            mOpenBluetoothButton.setText(R.string.close_bt_string);
            mOpenBluetoothButton.setEnabled(true);
            setButtonClickable(true);
            mOpenBluetooth = true;
            scanDevice(false);
            break;
        case BluetoothAdapter.STATE_TURNING_OFF:
            if(this.getUserVisibleHint()){
                mOpenBluetoothButton.setText(R.string.close_bt_string);
                mOpenBluetoothButton.setEnabled(false);
                setButtonClickable(false);
                mOpenBluetooth = true;
            }

            break;
        case BluetoothAdapter.STATE_OFF:
            if(this.getUserVisibleHint()){
                mOpenBluetoothButton.setText(R.string.open_bt_string);
                mOpenBluetoothButton.setEnabled(true);
                setButtonClickable(false);
                mOpenBluetooth = false;
                mDeviceList.clear();
                deviceAdapter.notifyDataSetChanged();
            }
            break;
        default:
            if(this.getUserVisibleHint()){
                mOpenBluetoothButton.setText(R.string.open_bt_string);
                mOpenBluetoothButton.setEnabled(true);
                setButtonClickable(false);
                mOpenBluetooth = false;

            }
            break;
        }
    }

    private void handleModeChanged(int mode) {
        if (mode == BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE) {
            setDiscoveryModeStatus(true);
        } else {
            setDiscoveryModeStatus(false);
        }
    }

    private void handleDevicesNameChanged(Intent intent){
        String newName = intent.getStringExtra(BluetoothAdapter.EXTRA_LOCAL_NAME);
        writeDeviceNamePin(newName,mPin);
    }

    public void disConnectAvrcp(Intent intent) {
        String addr = intent.getStringExtra("device_addr");
        mAVRCPConnectDevice = BluetoothAdapter.getDefaultAdapter().getRemoteDevice(addr);
    }

    public void connectAvrcp(Intent intent) {
        String addr = intent.getStringExtra("device_addr");
        mAVRCPConnectDevice = BluetoothAdapter.getDefaultAdapter().getRemoteDevice(addr);
    }

    public void updateAvrcpCtPlayerManageProxy() {
    }

    private void writeLastConnectedDeviceData(String BT_ADDR) {
        if(mContext == null)return;
        SharedPreferences.Editor sharedata =
        mContext.getSharedPreferences("preConnectedDevice_data",Context.MODE_PRIVATE).edit();
        sharedata.putString("BTADDR", BT_ADDR);
        if (!sharedata.commit()) {
            Log.w(TAG, "save last connected device failure");
        }
    }

    private void readLastConnectedDeviceData() {
        if(mContext == null)return;
        SharedPreferences sharedata = 
            mContext.getSharedPreferences("preConnectedDevice_data",Context.MODE_PRIVATE);
        mLastConnectedDeviceAddr = sharedata.getString("BTADDR", "");
    }

    private void writeDeviceNamePin(String devicename, String pin) {
        if(mContext == null)return;
        SharedPreferences.Editor sharedata = 
        mContext.getSharedPreferences("device_name_pin_data",Context.MODE_PRIVATE).edit();
        sharedata.putString("DEVICENAME", devicename);
        sharedata.putString("PIN", pin);
        sharedata.commit();
    }

    private void readDeviceNamePin() {
        if(mContext == null)return;
        SharedPreferences sharedata = 
            mContext.getSharedPreferences("device_name_pin_data",Context.MODE_PRIVATE);
        String devicename = mLocalAdapter.getName();
        mCurName = sharedata.getString("DEVICENAME",devicename); 
        mPin = sharedata.getString("PIN", "1234");
    }

    private void writeAutoConnectData(boolean isAutoConnect) {
        if(mContext == null)return;
        SharedPreferences.Editor sharedata = 
        mContext.getSharedPreferences("bt.setting.autoconnect", Context.MODE_PRIVATE).edit();
        sharedata.putBoolean("IS_BT_AUTO_CONNECT", isAutoConnect);
        if (!sharedata.commit()) {
            Log.w(TAG, "save Auto Connect Data failure");
        }
    }

    private void readAutoConnectData() {
        if(mContext == null)return;
        SharedPreferences sharedata = mContext.getSharedPreferences("bt.setting.autoconnect",
                Context.MODE_PRIVATE);
        mIsAutoConnect = sharedata.getBoolean("IS_BT_AUTO_CONNECT", false);
    }

    private void writeAutoAnswerData(boolean isAutoAnswer) {
        if(mContext == null)return;
        SharedPreferences.Editor sharedata = 
        mContext.getSharedPreferences("bt.setting.autoanswer",Context.MODE_PRIVATE).edit();
        sharedata.putBoolean("IS_BT_AUTO_ANSWER", isAutoAnswer);
        if (!sharedata.commit()) {
            if(DEBUG)Log.w(TAG, "save AutoAnswerData failure");
        }
    }

    private void readAutoAnswerData() {
        if(mContext == null)return;
        SharedPreferences sharedata = mContext.getSharedPreferences("bt.setting.autoanswer",
                Context.MODE_PRIVATE);
        mIsAutoAnswer = sharedata.getBoolean("IS_BT_AUTO_ANSWER", false);
    }

    private boolean invokePairConnectActivity(BluetoothDevice selectedDevice) {

        Intent intentPairConnect = new Intent();
        intentPairConnect.setClass(getActivity(),BtPairConnectActivity.class);
        Bundle bundle = new Bundle();
        bundle.putParcelable("SELECTED_DEVICE",selectedDevice);
        intentPairConnect.putExtras(bundle);
        startActivity(intentPairConnect);

        return true;
    }

    private void setButtonClickable(boolean clickable){
        if(this.getUserVisibleHint()){
            ((Button) getActivity().findViewById(R.id.btn_discovery_bt)).setEnabled(clickable);
            ((Button) getActivity().findViewById(R.id.btn_edit_bt)).setEnabled(clickable);
            ((Button) getActivity().findViewById(R.id.btn_scan_bt)).setEnabled(clickable);
            ((Button) getActivity().findViewById(R.id.btn_autoanswer_bt)).setEnabled(clickable);
            ((Button) getActivity().findViewById(R.id.btn_autoconnect_bt)).setEnabled(clickable);

        }

    }

    private class CachedDevicesAdapter extends BaseAdapter {

            @Override
            public int getCount() {
                if (mDeviceList != null) {
                    return mDeviceList.size();
                } else {
                    return 0;
                }
            }

            @Override
            public Object getItem(int position) {
                if (mDeviceList != null) {
                    return mDeviceList.get(position);
                } else {
                    return null;
                }
            }

            @Override
            public long getItemId(int position) {
                if (mDeviceList != null) {
                    return position ;
                } else {
                    return 0;
                }
            }

            @Override
            public View getView(int position, View convertView, ViewGroup parent) {
                synchronized (mDeviceList) {
                    if (mDeviceList != null && position < mDeviceList.size()) {
                        ItemView view = null;
                        if (convertView != null && convertView instanceof ItemView) {
                            view = (ItemView) convertView;
                        } else {
                            view = new ItemView(getActivity());
                        }
                        String deviceName = mDeviceList.get(position).getDevice().getName();
                        String deviceAddr = mDeviceList.get(position).getDevice().getAddress();
                        String displayName = (deviceName == null || deviceName.isEmpty())?
                            deviceAddr : deviceName;
                        view.getNameView().setText(displayName);
                        Log.d(TAG,"getView  deviceName = " + deviceName);
                        //Log.d(TAG,"getView state = "+getResources().getString(mDeviceList.get(position).getConnectedState()));
                        view.getStatesView().setText(mDeviceList.get(position).getConnectedState());
                        return view;
                    } else {
                        return null;
                    }
                }
            }

        }


     private class ItemView extends LinearLayout {

        private TextView mNameView = null;
        private TextView mStateView = null;
        private TextView mAddrView = null;
        public ItemView(Context context) {
            this(context, null);
        }

        public ItemView(Context context, AttributeSet attrs) {
            super(context, attrs);
            LayoutInflater inflator = (LayoutInflater) context
                    .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            View view = inflator.inflate(R.layout.device_listitem, null);
            addView(view);
            initView(view);
        }

        private void initView(View view) {
            mNameView = (TextView) findViewById(R.id.item_remote_device_name);
            mStateView = (TextView) findViewById(R.id.item_remote_connect_status);
            mAddrView = (TextView) findViewById(R.id.item_remote_device_macaddr);
        }

        public TextView getNameView() {
            return mNameView;
        }

        public TextView getStatesView() {
            return mStateView;
        }

     }


       @Override
       public void onBluetoothStateChanged(int bluetoothState){
           Log.d(TAG,"onBluetoothStateChanged bluetoothState = " + bluetoothState);
           checkBluetoothStatus();
           handleStateChanged(bluetoothState);
           refreshDataList();
       }

       @Override
       public void onScanningStateChanged(boolean started){
           Log.d(TAG,"onScanningStateChanged  started = " + started);
           if(started){
              mScanLayout.setVisibility(View.VISIBLE);
           }else{
              mScanLayout.setVisibility(View.GONE);
           }
           isScaning = started;

       }

        @Override
        public void onDeviceBondStateChanged(CachedBluetoothDevice cachedDevice, int bondState){
            deviceAdapter.notifyDataSetChanged();
        }

        @Override
        public void onConnectionStateChanged(CachedBluetoothDevice cachedDevice, int state){
            Log.d(TAG, "onConnectionStateChanged, cachedDevice = " + cachedDevice + " state = " + state);
        }

        @Override
        public void onDeviceAdded(CachedBluetoothDevice cachedDevice){
            Log.d(TAG,"onDeviceAdded");
            if(!mDeviceList.contains(cachedDevice)){
                mDeviceList.add(cachedDevice);
                deviceAdapter.notifyDataSetChanged();
            }else{
                Log.d(TAG,"onDeviceAdded contains");

            }
        }

        @Override
        public void onDeviceDeleted(CachedBluetoothDevice cachedDevice){
            Log.d(TAG,"onDeviceAdded");
            for (int i = mDeviceList.size() - 1; i >= 0; i--) {
                if (cachedDevice.getDevice().getAddress().equals( mDeviceList.get(i).getDevice().getAddress())) {
                    mDeviceList.remove(i);
                }
            }
            deviceAdapter.notifyDataSetChanged();
        }

        private void refreshDataList(){
            if(!mOpenBluetooth){
                mDeviceList.clear();
                deviceAdapter.notifyDataSetChanged();
                return;
            } 
            if(mDeviceList.isEmpty() || mDeviceList.size() <= 0){
                Set<BluetoothDevice> pairedDevices = mLocalManager.getBluetoothAdapter()
                            .getBondedDevices();
                Log.d(TAG,"pairedDevices size = "+pairedDevices.size());

                if(!pairedDevices.isEmpty() && pairedDevices.size() > 0){
                    for (BluetoothDevice device : pairedDevices) {
                        if(mLocalManager.getCachedDeviceManager().findDevice(device) == null){
                            mLocalManager.getCachedDeviceManager().addDevice(device);
                        }

                    }
                }
                mDeviceList = (ArrayList)mLocalManager.getCachedDeviceManager().getCachedDevicesCopy();
            }
            deviceAdapter.notifyDataSetChanged();
        }
    private void showDisconnectDialog(Context context,String string){
        Log.d(TAG,"showDisconnectDialog  = qiu");
        if(dialog != null){
            dialog.dismiss();
        }
        dialog = new AlertDialog.Builder(context)
                        .setNegativeButton(R.string.dlg_ok, new DialogInterface.OnClickListener(){
                            public void onClick(DialogInterface dialog, int which) {
                                Log.d(TAG,"dialog_retain    = qiu");
                                dialog.dismiss();
                                if (dialog != null) {
                                    dialog = null;
                                }
                            }
                        }).create();
        dialog.setMessage(string);
        dialog.show();

    }

        private void showEditDialog(){
            Log.d(TAG,"showEditDialog");
            LayoutInflater factory = LayoutInflater.from(getActivity());
            final View dialogView = factory.inflate(R.layout.bt_edit_namepin, null);

            edt_devicename = (EditText)dialogView.findViewById(R.id.bt_device_name_et);
            pin = (EditText)dialogView.findViewById(R.id.bt_device_pin_et);
            if((edt_devicename == null)||(pin == null)){
                Log.e(TAG, "devicename == null or pin == null");
                return;
            }
            readDeviceNamePin();
            edt_devicename.setText(mCurName);
            pin.setText(mPin);

            final Dialog alertDialog = new Dialog(getActivity());
            alertDialog.setContentView(dialogView);
            alertDialog.setTitle(R.string.bt_device_info);
            ((Button) alertDialog.findViewById(R.id.btn_edit_ok))
                    .setOnClickListener(new OnClickListener() {
                        public void onClick(View arg0) {
                            writeDeviceNamePin(edt_devicename.getText().toString(),
                            pin.getText().toString());
                            // set the local bluetooth adapter name
                            readDeviceNamePin();
                            // if(!mLocalManager.getBluetoothAdapter().setName(mCurName))
                            if (!BluetoothAdapter.getDefaultAdapter().setName(mCurName))
                                Log.e(TAG, "set local bluetooth adapter name fail");

                            alertDialog.dismiss();
                        }
                    });
            ((Button) alertDialog.findViewById(R.id.btn_edit_cancel))
                    .setOnClickListener(new OnClickListener() {
                        public void onClick(View arg0) {
                            alertDialog.dismiss();
                        }
                    });
            alertDialog.show();

        }

}
