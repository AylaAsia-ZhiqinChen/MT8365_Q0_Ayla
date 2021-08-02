package com.android.factorytest;

import android.app.Activity;
import android.content.Intent;
import android.content.res.Resources;
import android.graphics.Typeface;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.widget.GridLayout;
import android.widget.TextView;
import android.view.MenuItem;

import com.android.factorytest.model.TestState;

import java.util.ArrayList;

public class FactoryTest extends Activity {

    private TextView mAutoTestTv;
    private GridLayout mTestsContainer;
    private FactoryTestApplication mApplication;
    private Resources mResources;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_factory_test);
        getActionBar().setDisplayHomeAsUpEnabled(true);

        mApplication = (FactoryTestApplication) getApplication();
        mResources = getResources();

        mAutoTestTv = (TextView) findViewById(R.id.auto_test);
        mTestsContainer = (GridLayout) findViewById(R.id.test_group_container);

        mAutoTestTv.setOnClickListener(mAutoTestClickListener);

        if (getResources().getBoolean(R.bool.enabled_fighting_test)) {
            TestWatermarkService.launcherShowFm(this);
        }
    }
    
    @Override
    public boolean onMenuItemSelected(int featureId, MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:          
                finish();
                break;
        }
        return super.onMenuItemSelected(featureId, item);
    }

    @Override
    protected void onResume() {
        super.onResume();
        updateViews();
    }

    @Override
    protected void onDestroy() {
        if (getResources().getBoolean(R.bool.enabled_fighting_test)) {
            TestWatermarkService.launcherHileFm(this);
        }
        super.onDestroy();
    }

    /**
     * 自动测试按钮点击事件
     */
    private View.OnClickListener mAutoTestClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            Utils.startAutoTest(FactoryTest.this, mApplication.getTestInfoList(), 0);
        }
    };

    private void updateViews() {
        mTestsContainer.removeAllViews();
        ArrayList<Integer> groups = mApplication.getTestGroupList();
        if (!groups.isEmpty()) {
            mApplication.updateTestStates();
            int columnCount = mResources.getInteger(R.integer.test_group_column_count);
            int leftPadding = mResources.getDimensionPixelSize(R.dimen
                    .gridlayout_horizontal_margin);
            int rightPadding = mResources.getDimensionPixelSize(R.dimen
                    .gridlayout_horizontal_margin);
            int height = mResources.getDimensionPixelSize(R.dimen.gridlayout_item_height);
            Drawable leftBackground = mResources.getDrawable(R.drawable.ic_left_item_background,
                    getTheme());
            Drawable rightBackground = mResources.getDrawable(R.drawable
                    .ic_right_item_background, getTheme());
            int unknown = mResources.getColor(R.color.white);
            int fail = mResources.getColor(R.color.red);
            int pass = mResources.getColor(R.color.green);
            int maxWidth = mResources.getDisplayMetrics().widthPixels - (leftPadding +
                    rightPadding);
            Log.d(this, "updateViews=>size: " + groups.size() + ", maxWidth: " + maxWidth);
            int remainder = groups.size() % columnCount;
            if (remainder != 0) {
                for (int i = 0; i < remainder; i++) {
                    //groups.add(R.string.undefined_test_title);
                }
            }
            if (groups.size() > 0) {
                for (int i = 0; i < groups.size(); i++) {
                    TextView item = new TextView(this);
                    item.setText(groups.get(i));
                    item.setTypeface(item.getTypeface(), Typeface.BOLD);
                    item.setTextSize(TypedValue.COMPLEX_UNIT_SP, mResources.getInteger(R.integer
                            .grid_layout_item_text_size));
                    item.setGravity(Gravity.CENTER);
                    TestState state = Utils.getGroupTestState(this, mApplication.getTestInfoList(), groups.get(i));
                    if (TestState.PASS.equals(state)) {
                        item.setTextColor(pass);
                    } else if (TestState.FAIL.equals(state)) {
                        item.setTextColor(fail);
                    } else {
                        item.setTextColor(unknown);
                    }
                    if (((i + 1) % 2) != 0) {
                        item.setBackground(leftBackground);
                    } else {
                        item.setBackground(rightBackground);
                    }
                    item.setTag(groups.get(i));
                    item.setOnClickListener(mTestGroupClickListener);
                    GridLayout.LayoutParams params = new GridLayout.LayoutParams();
                    params.setGravity(Gravity.CENTER);
                    params.width = maxWidth / 2;
                    params.height = height;
                    mTestsContainer.addView(item, i, params);
                }
            }
        }
    }

    private View.OnClickListener mTestGroupClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            int groupResId = (int) view.getTag();
            Intent intent = new Intent(FactoryTest.this, TestListActivity.class);
            intent.putExtra(Utils.EXTRA_GROUP_RES_ID, groupResId);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(intent);
        }
    };
}
