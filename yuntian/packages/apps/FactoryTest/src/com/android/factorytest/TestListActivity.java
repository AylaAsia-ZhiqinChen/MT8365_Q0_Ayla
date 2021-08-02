package com.android.factorytest;

import android.app.Activity;
import android.app.ListActivity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.model.TestInfo;
import com.android.factorytest.model.TestState;

import java.util.ArrayList;

/**
 * 测试组测试项列表Activity
 */
public class TestListActivity extends ListActivity implements AdapterView.OnItemClickListener {

    private FactoryTestApplication mApplication;
    private TestListAdapter mAdapter;
    private ArrayList<TestInfo> mGroupItems;

    private int mGroupResId;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getActionBar().setDisplayHomeAsUpEnabled(true);

        mApplication = (FactoryTestApplication) getApplication();
        mGroupResId = -1;
        Intent intent = getIntent();
        onNewIntent(intent);
    }

    @Override
    protected void onResume() {
        super.onResume();
        updateViews();
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        mGroupResId = intent.getIntExtra(Utils.EXTRA_GROUP_RES_ID, -1);
        updateViews();
    }

    @Override
    public boolean onMenuItemSelected(int featureId, MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                finish();
                return true;
        }
        return super.onMenuItemSelected(featureId, item);
    }

    @Override
    public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
        TestInfo item = mAdapter.getItem(i);
        startTest(item);
    /**    if (isNfcTest(item)) {
            if (setNFCSensorTestEnabled(item.getClassName())) {
                startTest(item);
            } else {
                Toast.makeText(this, R.string.nfc_sensor_test_disabled_tip, Toast.LENGTH_SHORT).show();
            }
        } else {
            startTest(item);
        }*/
    }

    private boolean isNfcTest(TestInfo info) {
        if (info.getGroupResId() == R.string.sensor_group_title) {
            if ("com.android.factorytest.sensor.NFCSensorTest".equals(info.getClassName())) {
                return true;
            }
        }
        return false;
    }

    private boolean setNFCSensorTestEnabled(String className) {
        ComponentName cn = new ComponentName(getPackageName(), className);
        PackageManager pm = getPackageManager();
        pm.setComponentEnabledSetting(cn, PackageManager.COMPONENT_ENABLED_STATE_ENABLED, PackageManager.DONT_KILL_APP);
        return (pm.getComponentEnabledSetting(cn) == PackageManager.COMPONENT_ENABLED_STATE_ENABLED);
    }

    public void updateViews() {
        if (mGroupResId != -1) {
            mApplication.updateTestStates();
            getActionBar().setTitle(getString(mGroupResId));
            mGroupItems = Utils.getGroupTestItems(this, mApplication.getTestInfoList(), mGroupResId);
            if (mGroupItems.size() == 1) {
                TestInfo item = mGroupItems.get(0);
                startTest(item);
                finish();
            } else if (mGroupItems.isEmpty()) {
                Log.e(this, "updateViews=>" + getString(mGroupResId) + " is empty group.");
                Toast.makeText(this, R.string.not_found_test_item, Toast.LENGTH_SHORT).show();
                finish();
            } else {
                mAdapter = new TestListAdapter(this, mGroupItems);
                getListView().setAdapter(mAdapter);
                getListView().setOnItemClickListener(this);
            }
        } else {
            Log.e(this, "updateView=>" + mGroupResId + " is an illegal ID.");
            Toast.makeText(this, R.string.illegal_test_group, Toast.LENGTH_SHORT).show();
            finish();
        }
    }

    private void startTest(TestInfo item) {
        Intent intent = item.getIntent();
        intent.putExtra(Utils.EXTRA_TEST_INDEX, item.getOrder());
        intent.putExtra(Utils.EXTRA_AUTO_TEST, false);
        startActivity(intent);
    }

    private class TestListAdapter extends BaseAdapter {

        private Context mContext;
        private LayoutInflater mInflater;
        private ArrayList<TestInfo> mList;

        public TestListAdapter(Context context,ArrayList<TestInfo> list) {
            mContext = context;
            mInflater = (LayoutInflater) mContext.getSystemService(LAYOUT_INFLATER_SERVICE);
            mList = list;
        }

        @Override
        public int getCount() {
            return mList != null ? mList.size() : 0;
        }

        @Override
        public TestInfo getItem(int i) {
            return mList.get(i);
        }

        @Override
        public long getItemId(int i) {
            return i;
        }

        @Override
        public View getView(int i, View view, ViewGroup viewGroup) {
            ViewHolder holder = null;
            if (view == null) {
                view = mInflater.inflate(R.layout.group_test_list, viewGroup, false);
                holder = new ViewHolder();
                holder.mTextView = (TextView) view.findViewById(R.id.test_item_title);
                view.setTag(holder);
            } else {
                holder = (ViewHolder) view.getTag();
            }
            TestInfo item = mList.get(i);
            holder.mTextView.setText(item.getTestTitleResId());
            if (TestState.UNKNOWN.equals(item.getState())) {
                holder.mTextView.setTextColor(getColor(R.color.white));
            } else if (TestState.FAIL.equals(item.getState())) {
                holder.mTextView.setTextColor(getColor(R.color.red));
            } else if (TestState.PASS.equals(item.getState())) {
                holder.mTextView.setTextColor(getColor(R.color.green));
            }
            return view;
        }

        class ViewHolder {
            TextView mTextView;
        }
    }
}
