package com.android.factorytest;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

import com.android.factorytest.model.TestInfo;
import com.android.factorytest.model.TestState;

import java.util.ArrayList;
import com.mediatek.nvram.NvRAMUtils;

/**
 * 测试结果Activity，用于显示当前的测试结果
 */
public class TestResultActivity extends Activity implements View.OnClickListener {

    private TextView mTestResultTv;
    private Button mResetBt;
    private ListView mListView;
    private View mHeadView;
    private View mFootView;
    private ArrayList<TestInfo> mTestItems;
    private FactoryTestApplication mApplication;
    private FactoryTestDatabase mDatabase;
    private TestResultAdapater mAdapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getActionBar().setDisplayHomeAsUpEnabled(true);
        setContentView(R.layout.test_result_activity);

        mApplication = (FactoryTestApplication) getApplication();
        mDatabase = FactoryTestDatabase.getInstance(this);
        mTestResultTv = (TextView) findViewById(R.id.test_result_text);
        mResetBt = (Button) findViewById(R.id.test_result_reset);
        mListView = (ListView) findViewById(R.id.test_result_list);
        mHeadView = findViewById(R.id.head_view);
        mFootView = findViewById(R.id.foot_view);

        mResetBt.setOnClickListener(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
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
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.test_result_reset:
                mDatabase.clearDatabase();
                if (getResources().getBoolean(R.bool.enabled_fighting_test)) {
					writeFmFlagToNV(false);
				}
                updateViews();
                break;
        }
    }

    private void updateViews() {
        mApplication.updateTestStates();
        mTestItems = (ArrayList<TestInfo>) mApplication.getTestInfoList().clone();
        removeTestResultItem();
        if (mTestItems.isEmpty()) {
            mFootView.setVisibility(View.GONE);
        } else {
            mFootView.setVisibility(View.VISIBLE);
        }
        mAdapter = new TestResultAdapater(this, mTestItems);
        mListView.setAdapter(mAdapter);
        TestState state = getAllTestState();
        if (TestState.PASS.equals(state)) {
            mTestResultTv.setTextColor(getColor(R.color.test_pass_color));
        } else if (TestState.FAIL.equals(state)) {
            mTestResultTv.setTextColor(getColor(R.color.test_fail_color));
        } else {
            mTestResultTv.setTextColor(getColor(R.color.test_unknown_color));
        }
    }

    private void removeTestResultItem() {
        String[] resultItems = getResources().getStringArray(R.array.test_result_groups);
        for (int i = 0; i < resultItems.length; i++) {
            for (int j = 0; j < mTestItems.size(); j++) {
                if (mTestItems.get(j).getClassName().equals(resultItems[i])) {
                    Log.d(this, "removeTestResultItem=>remove: " + mTestItems.get(j).getClassName());
                    mTestItems.remove(j);
                    break;
                }
            }
        }
    }

    private TestState getAllTestState() {
        TestState result = TestState.PASS;
        boolean isTest = false;
        if (!mTestItems.isEmpty()) {
            TestInfo item = null;
            TestState state = TestState.UNKNOWN;
            for (int i = 0; i < mTestItems.size(); i++) {
                item = mTestItems.get(i);
                state = mDatabase.getTestState(item.getTestTitleResId());
                if (TestState.FAIL.equals(state)) {
                    if (!isTest) {
                        isTest = true;
                    }
                    result = TestState.FAIL;
                    break;
                } else if (TestState.PASS.equals(state)) {
                    if (!isTest) {
                        isTest = true;
                    }
                } else if (TestState.UNKNOWN.equals(state)) {
                    result = TestState.UNKNOWN;
                }
            }
        }
        if (TestState.PASS.equals(result) && !isTest) {
            result = TestState.UNKNOWN;
        }
        Log.d(this, "getAllTestState=>result: " + result + ", isTest: " + isTest);
        return result;
    }
    
    private void writeFmFlagToNV(boolean pass) {
        byte[] buff = null;
        try {
            buff = new byte[1];
            buff[0] = (byte)(pass ? 'P' : 'F');
            NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_MMI_TEST_FLAG, buff);
            TestWatermarkService.launcherCheck(this);
        } catch (Exception e){
           Log.d(this, "writeFmFlagToNV=>error: ", e);
        }
    }

    private class TestResultAdapater extends BaseAdapter {

        private Context mContext;
        private LayoutInflater mInflater;
        private ArrayList<TestInfo> mList;

        public TestResultAdapater(Context context, ArrayList<TestInfo> list) {
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
                view = mInflater.inflate(R.layout.test_result_adapter, viewGroup, false);
                holder = new ViewHolder();
                holder.mId = (TextView) view.findViewById(R.id.test_result_id);
                holder.mTitle = (TextView) view.findViewById(R.id.test_result_title);
                holder.mResult = (TextView) view.findViewById(R.id.test_result);
                view.setTag(holder);
            } else {
                holder = (ViewHolder) view.getTag();
            }
            TestInfo item = mList.get(i);
            holder.mId.setText((item.getOrder() + 1) + "");
            holder.mTitle.setText(item.getTestTitleResId());
            if (TestState.PASS.equals(item.getState())) {
                holder.mResult.setText(R.string.test_result_pass_text);
                holder.mResult.setTextColor(getColor(R.color.test_pass_color));
            } else if (TestState.FAIL.equals(item.getState())) {
                holder.mResult.setText(R.string.test_result_fail_text);
                holder.mResult.setTextColor(getColor(R.color.test_fail_color));
            } else {
                holder.mResult.setText(R.string.test_result_not_tested_text);
                holder.mResult.setTextColor(getColor(R.color.test_unknown_color));
            }
            return view;
        }

        class ViewHolder {
            TextView mId;
            TextView mTitle;
            TextView mResult;
        }
    }
}
