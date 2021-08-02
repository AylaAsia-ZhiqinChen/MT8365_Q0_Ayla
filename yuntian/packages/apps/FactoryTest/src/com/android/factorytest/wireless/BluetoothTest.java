package com.android.factorytest.wireless;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.ScanResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.util.ArrayList;
import java.util.List;

/**
 * 蓝牙测试
 */
public class BluetoothTest extends BaseActivity {

    private static final int MSG_BLUETOOTH_DEVICE_FOUND = 0;
    private static final int MSG_REDISCOVERY = 1;
    private static final int MSG_DISCOVERY_STARTED = 2;
    private static final int MSG_DISCOVERY_FINISHED = 3;
    private static final int MSG_STATE_CHANGED = 4;
    private static final int DELAYED_REDISCOVERY = 10000;
    private static final int DISCOVERY_TIMEOUT = 60000;

    private TextView mBluetoothStateTv;
    private ListView mBluetoothListView;
    private TextView mEmptyView;
    private BluetoothListAdapter mAdapter;
    private BluetoothAdapter mBluetoothAdapter;
    private BluetoothManager mBluetoothManager;
    private ArrayList<Bluetooth> mDevices;

    private boolean mLastBluetoothEnabled;
    private boolean mNeedOpenBluetoothWhenOff;
    private boolean mIsFirstScan;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_bluetooth_test);

        super.onCreate(savedInstanceState);

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        mBluetoothManager = (BluetoothManager) getSystemService(BLUETOOTH_SERVICE);
        mLastBluetoothEnabled = mBluetoothAdapter.isEnabled();
        mDevices = new ArrayList<Bluetooth>();
        mAdapter = new BluetoothListAdapter(this, mDevices);
        initEmptyView();

        mBluetoothStateTv = (TextView) findViewById(R.id.bluetooth_state);
        mBluetoothListView = (ListView) findViewById(R.id.bluetooth_list);
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        lp.gravity = Gravity.CENTER;
        ((ViewGroup) mBluetoothListView.getParent()).addView(mEmptyView, lp);
        mBluetoothListView.setEmptyView(mEmptyView);
        mBluetoothListView.setAdapter(mAdapter);
        mEmptyView.setText(R.string.empty_bluetooth_list);
    }

    @Override
    protected void onResume() {
        super.onResume();
        mIsFirstScan = true;
        openBluetooth();
        updateBluetoothState();
        registerBluetoothReceiver();
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterBluetoothReceiver();
        cancelDiscovery();
        closeBluetooth();
    }

    private void updateBluetoothState() {
        int state = mBluetoothAdapter.getState();
        switch (state) {
            case BluetoothAdapter.STATE_TURNING_ON:
                mBluetoothStateTv.setText(R.string.bluetooth_turning_on_state_text);
                break;

            case BluetoothAdapter.STATE_ON:
                mBluetoothStateTv.setText(R.string.bluetooth_on_state_text);
                startDiscovery();
                break;

            case BluetoothAdapter.STATE_TURNING_OFF:
                mBluetoothStateTv.setText(R.string.bluetooth_turning_off_state_text);
                break;

            case BluetoothAdapter.STATE_OFF:
                mBluetoothStateTv.setText(R.string.bluetooth_off_state_text);
                if (mNeedOpenBluetoothWhenOff) {
                    openBluetooth();
                    mNeedOpenBluetoothWhenOff = false;
                }
                break;

            default:
                mBluetoothStateTv.setText(R.string.bluetooth_unknow_state_text);
                break;
        }
    }

    private void openBluetooth() {
        if (!mBluetoothAdapter.isEnabled()) {
            if (mBluetoothAdapter.getState() == BluetoothAdapter.STATE_OFF) {
                mBluetoothAdapter.enable();
            } else {
                mNeedOpenBluetoothWhenOff = true;
            }
        }
    }

    private void closeBluetooth() {
        if (!mLastBluetoothEnabled) {
            mBluetoothAdapter.disable();
        }
    }

    private void startDiscovery() {
        if (mBluetoothAdapter.isDiscovering()) {
            mBluetoothAdapter.cancelDiscovery();
        }
        //mBluetoothAdapter.setDiscoverableTimeout(DISCOVERY_TIMEOUT);
        mBluetoothAdapter.startDiscovery();
    }

    private void cancelDiscovery() {
        mBluetoothAdapter.cancelDiscovery();
    }

    private void addBluetoothDevice(Bluetooth device) {
        if (device != null) {
            mDevices.add(device);
            mAdapter.setList(mDevices);
            setPassButtonEnabled(true);
            setTestPass(true);
        }
        if (isAutoTest()) {
            if (isTestPass()) {
                doOnAutoTest();
            }
        }
    }

    private void initEmptyView() {
        mEmptyView = new TextView(this);
        mEmptyView.setTextSize(TypedValue.COMPLEX_UNIT_SP, getResources().getInteger(R.integer.bluetooth_list_empty_view_text_size));
        mEmptyView.setGravity(Gravity.CENTER);
    }

    private void registerBluetoothReceiver() {
        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        filter.addAction(BluetoothDevice.ACTION_FOUND);
        registerReceiver(mBluetoothReceiver, filter);
    }

    private void unregisterBluetoothReceiver() {
        unregisterReceiver(mBluetoothReceiver);
    }

    private BroadcastReceiver mBluetoothReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
                mHandler.sendEmptyMessage(MSG_DISCOVERY_FINISHED);
            } else if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)) {
                mHandler.sendEmptyMessage(MSG_DISCOVERY_STARTED);
            } else if (BluetoothAdapter.ACTION_STATE_CHANGED.equals(action)) {
                mHandler.sendEmptyMessage(MSG_STATE_CHANGED);
            } else if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                String name = intent.getStringExtra(BluetoothDevice.EXTRA_NAME);
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                short rssi = intent.getExtras().getShort(BluetoothDevice.EXTRA_RSSI);
                Log.d(this, "onReceive=>name: " + name + " rssi: " + rssi);
                Bluetooth bluetooth = new Bluetooth();
                bluetooth.device = device;
                bluetooth.rssi = rssi;
                Message msg = new Message();
                msg.obj = bluetooth;
                msg.what = MSG_BLUETOOTH_DEVICE_FOUND;
                mHandler.sendMessage(msg);
            }
        }
    };

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_BLUETOOTH_DEVICE_FOUND:
                    Bluetooth device = (Bluetooth) msg.obj;
                    if (mIsFirstScan){
                        addBluetoothDevice(device);
                    } else {
                        mDevices.add(device);
                    }
                    break;

                case MSG_REDISCOVERY:
                    startDiscovery();
                    break;

                case MSG_DISCOVERY_STARTED:
                    mEmptyView.setText(R.string.scanning_bluetooth_devices_title);
                    mDevices.clear();
                    break;

                case MSG_DISCOVERY_FINISHED:
                    if (!mIsFirstScan) {
                        mAdapter.setList(mDevices);
                    }
                    mEmptyView.setText(R.string.empty_bluetooth_list);
                    mHandler.sendEmptyMessageDelayed(MSG_REDISCOVERY, DELAYED_REDISCOVERY);
                    mIsFirstScan = false;
                    if (mDevices.size() > 0) {
                        setPassButtonEnabled(true);
                        setTestPass(true);
                    }
                    if (isAutoTest()) {
                        if (isTestPass()) {
                            doOnAutoTest();
                        }
                    }
                    break;

                case MSG_STATE_CHANGED:
                    updateBluetoothState();
                    break;
            }
        }
    };

    private class BluetoothListAdapter extends BaseAdapter {

        private Context mContext;
        private LayoutInflater mInflater;
        private List<Bluetooth> mList;

        public  BluetoothListAdapter(Context context, List<Bluetooth> list) {
            mContext = context;
            mInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            mList = new ArrayList<Bluetooth>();
            mList.addAll(list);
        }

        public void setList(List<Bluetooth> list) {
            mList = null;
            mList = new ArrayList<Bluetooth>();
            mList.addAll(list);
            notifyDataSetChanged();
        }

        @Override
        public int getCount() {
            return (mList != null ? mList.size() : 0);
        }

        @Override
        public Bluetooth getItem(int i) {
            return mList.get(i);
        }

        @Override
        public long getItemId(int i) {
            return i;
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {
            ViewHolder holder;
            if (view == null) {
                view = mInflater.inflate(R.layout.bluetooth_item, viewGroup, false);
                holder = new ViewHolder();
                holder.btName = (TextView) view.findViewById(R.id.bluetooth_name);
                holder.btSignal = (TextView) view.findViewById(R.id.bluetooth_signal);
                view.setTag(holder);
            } else {
                holder = (ViewHolder) view.getTag();
            }
            Bluetooth bluetooth = getItem(i);
            String name = bluetooth.device.getName();
            if (TextUtils.isEmpty(name)) {
                name = bluetooth.device.getAddress();
            }
            holder.btName.setText(name);
            holder.btSignal.setText(mContext.getString(R.string.bluetooth_signal_title, bluetooth.rssi));
            return view;
        }

        class ViewHolder {
            TextView btName;
            TextView btSignal;
        }
    }

    class Bluetooth {
        BluetoothDevice device;
        short rssi;
    }
}
