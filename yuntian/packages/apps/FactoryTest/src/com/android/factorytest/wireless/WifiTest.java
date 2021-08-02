package com.android.factorytest.wireless;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.wifi.ScanResult;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
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
 * WIFI测试
 */
public class WifiTest extends BaseActivity {

    private static final int MSG_SCAN_WIFI = 0;
    private static final int MSG_UPDATE_WIFI_LIST = 1;
    private static final int MSG_STATE_CHANGED = 2;
    private static final int DELAYED_RESCAN_WIFI = 10000;

    private TextView mWifiStateTv;
    private ListView mWifiListView;
    private WifiManager mWifiManager;
    private TextView mEmptyView;
    private WifiListAdapter mAdapter;

    private boolean mLastWifiEnabled;
    private boolean mNeedOpenWifiWhenDisabled;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_wifi_test);
        
        super.onCreate(savedInstanceState);

        initEmptyView();
        mWifiManager = (WifiManager) getApplicationContext().getSystemService(WIFI_SERVICE);
        mAdapter = new WifiListAdapter(this, new ArrayList<ScanResult>());
        mLastWifiEnabled = mWifiManager.isWifiEnabled();
        mNeedOpenWifiWhenDisabled = false;

        mWifiStateTv = (TextView) findViewById(R.id.wifi_state);
        mWifiListView = (ListView) findViewById(R.id.wifi_list);

        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        lp.gravity = Gravity.CENTER;
        ((ViewGroup) mWifiListView.getParent()).addView(mEmptyView, lp);
        mWifiListView.setEmptyView(mEmptyView);
        mWifiListView.setAdapter(mAdapter);
        mEmptyView.setText(R.string.empty_wifi_list);
    }

    @Override
    protected void onResume() {
        super.onResume();
        openWifi();
        updateWifiState();
        registerWifiChangedReceiver();
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterWifiChangedReceiver();
        closeWifi();
    }

    private void initEmptyView() {
        mEmptyView = new TextView(this);
        mEmptyView.setTextSize(TypedValue.COMPLEX_UNIT_SP, getResources().getInteger(R.integer.wifi_list_empty_view_text_size));
        mEmptyView.setGravity(Gravity.CENTER);
    }

    private void openWifi() {
        if (!mWifiManager.isWifiEnabled()) {
            if (mWifiManager.getWifiState() == WifiManager.WIFI_STATE_DISABLED) {
                mWifiManager.setWifiEnabled(true);
                mNeedOpenWifiWhenDisabled = false;
            } else {
                mNeedOpenWifiWhenDisabled = true;
            }
        }
    }

    private void updateWifiState() {
        int state = mWifiManager.getWifiState();
        switch (state) {
            case WifiManager.WIFI_STATE_ENABLING:
                mWifiStateTv.setText(R.string.wifi_openning_state_text);
                break;

            case WifiManager.WIFI_STATE_ENABLED:
                mWifiStateTv.setText(R.string.wifi_opened_state_text);
                startScanWifi();
                break;

            case WifiManager.WIFI_STATE_DISABLING:
                mWifiStateTv.setText(R.string.wifi_closing_state_text);
                break;

            case WifiManager.WIFI_STATE_DISABLED:
                mWifiStateTv.setText(R.string.wifi_closed_state_text);
                if (mNeedOpenWifiWhenDisabled) {
                    openWifi();
                    mNeedOpenWifiWhenDisabled = false;
                }
                break;

            default:
                mWifiStateTv.setText(R.string.wifi_unknow_state_text);
                break;
        }
    }

    private void registerWifiChangedReceiver() {
        IntentFilter filter = new IntentFilter();
        filter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        filter.addAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
        registerReceiver(mWifiStateChangedReceiver, filter);
    }

    private void unregisterWifiChangedReceiver() {
        unregisterReceiver(mWifiStateChangedReceiver);
    }

    private void closeWifi() {
        if (!mLastWifiEnabled) {
            mWifiManager.setWifiEnabled(false);
        }
    }

    private void startScanWifi() {
        mWifiManager.startScan();
        mEmptyView.setText(R.string.scanning_wifi_title);
    }

    private void updateWifiList() {
        List<ScanResult> results = mWifiManager.getScanResults();
        Log.d(this, "updateWifiList=>size: " + results.size());
        mAdapter.setList(results);
        if (results.size() > 0) {
            setPassButtonEnabled(true);
            setTestPass(true);
        }
        if (isAutoTest()) {
            if (isTestPass()) {
                doOnAutoTest();
            }
        }
    }

    private BroadcastReceiver mWifiStateChangedReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (WifiManager.WIFI_STATE_CHANGED_ACTION.equals(action)) {
                mHandler.sendEmptyMessage(MSG_STATE_CHANGED);
            } else if (WifiManager.SCAN_RESULTS_AVAILABLE_ACTION.equals(action)) {
                mHandler.sendEmptyMessage(MSG_UPDATE_WIFI_LIST);
            }
        }
    };

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_SCAN_WIFI:
                    startScanWifi();
                    break;

                case MSG_UPDATE_WIFI_LIST:
                    updateWifiList();
                    mEmptyView.setText(R.string.empty_wifi_list);
                    mHandler.sendEmptyMessageDelayed(MSG_SCAN_WIFI, DELAYED_RESCAN_WIFI);
                    break;

                case MSG_STATE_CHANGED:
                    updateWifiState();
                    break;
            }
        }
    };

    private class WifiListAdapter extends BaseAdapter {

        private Context mContext;
        private LayoutInflater mInflater;
        private List<ScanResult> mList;

        public  WifiListAdapter(Context context, List<ScanResult> list) {
            mContext = context;
            mInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            mList = list;
        }

        public void setList(List<ScanResult> list) {
            mList = null;
            mList = list;
            notifyDataSetChanged();
        }

        @Override
        public int getCount() {
            return (mList != null ? mList.size() : 0);
        }

        @Override
        public ScanResult getItem(int i) {
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
                view = mInflater.inflate(R.layout.wifi_item, viewGroup, false);
                holder = new ViewHolder();
                holder.wifiName = (TextView) view.findViewById(R.id.wifi_name);
                holder.wifiSignal = (TextView) view.findViewById(R.id.wifi_signal);
                view.setTag(holder);
            } else {
                holder = (ViewHolder) view.getTag();
            }
            ScanResult info = getItem(i);
            holder.wifiName.setText(info.SSID);
            holder.wifiSignal.setText(mContext.getString(R.string.wifi_signal_title, info.level));
            return view;
        }

        class ViewHolder {
            TextView wifiName;
            TextView wifiSignal;
        }
    }
}
