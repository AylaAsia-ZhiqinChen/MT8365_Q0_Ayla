package com.android.factorytest.phone;

import android.content.Context;
import android.os.Bundle;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
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
import android.os.ServiceManager;

import com.android.factorytest.Log;
import com.android.internal.telephony.ITelephony;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.R;

import java.util.ArrayList;

/**
 * SIM卡测试
 */
public class SIMCardTest extends BaseActivity {

    private ListView mSimListView;
    private TextView mEmptyView;
    private SimAdapter mAdapter;
    private TelephonyManager mTelephonyManager;
    private SubscriptionManager mSubscriptionManager;
    private ITelephony mITelephony;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_sim_card_test);

        super.onCreate(savedInstanceState);

        mAdapter = new SimAdapter(this, new ArrayList<String>());
        mTelephonyManager = (TelephonyManager) getSystemService(TELEPHONY_SERVICE);
        mITelephony = ITelephony.Stub.asInterface(ServiceManager.getService("phone"));
        mSubscriptionManager = SubscriptionManager.from(this);
        initEmptyView();

        mSimListView = (ListView) findViewById(R.id.sim_info_list);
        LinearLayout.LayoutParams lp = new LinearLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        lp.gravity = Gravity.CENTER;
        ((ViewGroup) mSimListView.getParent()).addView(mEmptyView, lp);
        mSimListView.setEmptyView(mEmptyView);
        mSimListView.setAdapter(mAdapter);
        mEmptyView.setText(R.string.empty_sim_list);
    }

    @Override
    protected void onResume() {
        super.onResume();
        updateSimCardInfo();
        int simCount = getSimCount();
        if (simCount > 1) {
			boolean isAllInsert = true;
			for (int i = 0; i < simCount; i++) {
				if (!isSimInsert(i)) {
					isAllInsert = false;
					break;
				} 
			}
            if (isAllInsert) {
                setPassButtonEnabled(true);
                setTestPass(true);
            }
        } else {
            if (isDefaultSimInsert()) {
                setPassButtonEnabled(true);
                setTestPass(true);
            }
        }
        if (isAutoTest()) {
            if (isTestPass()) {
                doOnAutoTest();
            }
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    private void initEmptyView() {
        mEmptyView = new TextView(this);
        mEmptyView.setTextSize(TypedValue.COMPLEX_UNIT_SP, getResources().getInteger(R.integer.bluetooth_list_empty_view_text_size));
        mEmptyView.setGravity(Gravity.CENTER);
    }
    
    private int getSimCount() {
		int simCount = getResources().getInteger(R.integer.sim_count);
        int realCount = mTelephonyManager.getSimCount();
        if (simCount > realCount) {
			simCount = realCount;
		}
		return simCount;
	}

    private void updateSimCardInfo() {
        ArrayList<String> infos = new ArrayList<String>();
        int simCount = getSimCount();
        Log.d(this, "updateSimCardInfo=>simCount: " + simCount);
        if (simCount > 1) {
			for (int i = 0; i < simCount; i++) {
				updateSimCardInfoBySoltId(infos, i);
			}
        } else if (simCount == 1) {
            updateDefaultSimCardInfo(infos);
        }
        mAdapter.setList(infos);
    }

    private void updateDefaultSimCardInfo(ArrayList<String> infos) {
        infos.add(getResources().getString(R.string.default_sim_info_title));
        if (isDefaultSimInsert()) {
            String country = getDefaultSimCardCountry();
            infos.add(country);
            String operator = getDefaultSimCardOperator();
            infos.add(operator);
            String operatorName = getDefaultSimCardOperatorName();
            infos.add(operatorName);
          //  String sn = getDefaultSimCardSN();
          //  infos.add(sn);
            String subId = getDefaultSimCardSubId();
            infos.add(subId);
            String deviceId = getDefaultSimCardDeviceId();
            infos.add(deviceId);
            String number = getDefaultSimCardNumber();
            infos.add(number);
        } else {
            infos.add(getResources().getString(R.string.sim_card_not_insert));
        }
    }

    private void updateSimCardInfoBySoltId(ArrayList<String> infos, int soltId) {
        infos.add(getResources().getString(R.string.sim_info_title, soltId + 1));
        if (isSimInsert(soltId)) {
            int subId = getSimSubscriptionInfoId(soltId);
            if (subId != -1) {
                String country = getSimCardCountry(subId);
                infos.add(country);
                String operator = getSimCardOperator(subId);
                infos.add(operator);
                String operatorName = getSimCardOperatorName(subId);
                infos.add(operatorName);
               // String sn = getSimCardSN(subId);
               // infos.add(sn);
                String subIdStr = getResources().getString(R.string.sim_subid_title, subId);
                infos.add(subIdStr);
                String deviceId = getSimCardDeviceId(soltId);
                infos.add(deviceId);
                String number = getSimCardNumber(subId);
                infos.add(number);
            }
        } else {
            infos.add(getResources().getString(R.string.sim_card_not_insert));
        }
    }

    private String getSimCardCountry(int subId) {
        String countryIso = mTelephonyManager.getSimCountryIso(subId);
        return getResources().getString(R.string.sim_country_iso_title,
                TextUtils.isEmpty(countryIso) ? "" : countryIso);
    }

    private String getSimCardOperator(int subId) {
        String operator = mTelephonyManager.getSimOperator(subId);
        return getResources().getString(R.string.sim_operator_title,
                TextUtils.isEmpty(operator) ? "" : operator);
    }

    private String getSimCardOperatorName(int subId) {
        String operatorName = mTelephonyManager.getSimOperatorName(subId);
        return getResources().getString(R.string.sim_operator_name_title,
                TextUtils.isEmpty(operatorName) ? "" : operatorName);
    }

    private String getSimCardSN(int subId) {
        String sn = mTelephonyManager.getSimSerialNumber(subId);
        return getResources().getString(R.string.sim_sn_title,
                TextUtils.isEmpty(sn) ? "" : sn);
    }

    private String getSimCardDeviceId(int soltId) {
        String deviceId = mTelephonyManager.getDeviceId(soltId);
        return getResources().getString(R.string.sim_device_id_title,
                TextUtils.isEmpty(deviceId) ? "" : deviceId);
    }

    private String getSimCardNumber(int subId) {
        String number = mTelephonyManager.getLine1Number(subId);
        return getResources().getString(R.string.sim_number_title,
                TextUtils.isEmpty(number) ? "" : number);
    }

    private String getDefaultSimCardCountry() {
        String countryIso = mTelephonyManager.getSimCountryIso();
        return getResources().getString(R.string.sim_country_iso_title,
                TextUtils.isEmpty(countryIso) ? "" : countryIso);
    }

    private String getDefaultSimCardOperator() {
        String operator = mTelephonyManager.getSimOperator();
        return getResources().getString(R.string.sim_operator_title,
                TextUtils.isEmpty(operator) ? "" : operator);
    }

    private String getDefaultSimCardOperatorName() {
        String operatorName = mTelephonyManager.getSimOperatorName();
        return getResources().getString(R.string.sim_operator_name_title,
                TextUtils.isEmpty(operatorName) ? "" : operatorName);
    }

    private String getDefaultSimCardSN() {
        String sn = mTelephonyManager.getSimSerialNumber();
        return getResources().getString(R.string.sim_sn_title,
                TextUtils.isEmpty(sn) ? "" : sn);
    }

    private String getDefaultSimCardSubId() {
        int subId = SubscriptionManager.getDefaultSubscriptionId();
        return getResources().getString(R.string.sim_subid_title, subId);
    }

    private String getDefaultSimCardDeviceId() {
        String deviceId = mTelephonyManager.getDeviceId();
        return getResources().getString(R.string.sim_device_id_title,
                TextUtils.isEmpty(deviceId) ? "" : deviceId);
    }

    private String getDefaultSimCardNumber() {
        String number = mTelephonyManager.getLine1Number();
        android.util.Log.d("penghai-->","getDefaultSimCardNumber11 = "+number);
        return getResources().getString(R.string.sim_number_title,
                TextUtils.isEmpty(number) ? "" : number);
    }

    private boolean isDefaultSimInsert() {
        boolean insert = false;
        try {
            insert = mITelephony.hasIccCard();
        } catch (Exception e) {
            Log.e(this, "isDefaultSimInsert=>error: " + insert);
        }
        return insert;
    }

    private boolean isSimInsert(int slotId) {
        boolean insert = false;
        try {
            insert = mITelephony.hasIccCardUsingSlotIndex(slotId);
        } catch (Exception e) {
            Log.e(this, "isSimInsert=>error: " + insert);
        }
        return insert;
    }

    private int getSimSubscriptionInfoId(int slotId) {
        SubscriptionInfo info = mSubscriptionManager.getActiveSubscriptionInfoForSimSlotIndex(slotId);
        if (info != null) {
            return info.getSubscriptionId();
        }
        return -1;
    }

    private class SimAdapter extends BaseAdapter {

        private ArrayList<String> mList;
        private LayoutInflater mInflater;

        public SimAdapter(Context context, ArrayList<String> list) {
            mInflater = (LayoutInflater) context.getSystemService(LAYOUT_INFLATER_SERVICE);
            mList = list;
        }

        public void setList(ArrayList<String> list) {
            mList = null;
            mList = list;
            notifyDataSetChanged();
        }

        @Override
        public int getCount() {
            return (mList != null ? mList.size() : 0);
        }

        @Override
        public String getItem(int position) {
            return mList.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            ViewHolder holder = null;
            if (convertView == null) {
                convertView = mInflater.inflate(R.layout.sim_item, parent, false);
                holder = new ViewHolder();
                holder.infoTv = (TextView) convertView.findViewById(R.id.sim_info);
                convertView.setTag(holder);
            } else {
                holder = (ViewHolder) convertView.getTag();
            }
            String info = getItem(position);
            if (isTitleString(info)) {
                holder.infoTv.setTextSize(TypedValue.COMPLEX_UNIT_SP, 21);
                holder.infoTv.getPaint().setFakeBoldText(true);
            } else {
                holder.infoTv.setTextSize(TypedValue.COMPLEX_UNIT_SP, 18);
                holder.infoTv.getPaint().setFakeBoldText(false);
            }
            holder.infoTv.setText(info);
            return convertView;
        }
        
        private boolean isTitleString(String info) {
			int simCount = getSimCount();
			if (simCount > 1) {
				for (int i = 0; i < simCount; i++) {
					if (getResources().getString(R.string.sim_info_title, i + 1).equals(info)) {
						return true;
					}
				}
			} else {
				return getResources().getString(R.string.default_sim_info_title).equals(info);
			}
			return false;
		}

        class ViewHolder {
            TextView infoTv;
        }
    }
}
