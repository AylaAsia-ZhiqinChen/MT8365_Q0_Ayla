package com.android.factorytest;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.view.KeyEvent;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.Toast;

import com.android.factorytest.model.TestInfo;
import com.android.factorytest.model.TestState;
import com.android.factorytest.Log;

import java.util.ArrayList;
import com.mediatek.nvram.NvRAMUtils;
/**
 * 所有测试项的基类
 */
public abstract class BaseActivity extends Activity implements View.OnClickListener {

    private Button mPassBt;
    private Button mFailBt;
    private View mBottomButtonContainer;
    private FactoryTestApplication mApplication;
    private FactoryTestDatabase mDatabase;
    private TestInfo mTestInfo;
    private Handler mHandler;

    private int mIndex;
    private long mAutoTestDelayed;
    private boolean mIsPass;
    private boolean mIsAutoTest;
    private boolean mIsTestCompleted;
    private boolean mEnabledActionBar = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        super.onCreate(savedInstanceState);
        if (mEnabledActionBar) {
            getActionBar().setDisplayHomeAsUpEnabled(true);
        }

        mIndex = -1;
        mIsPass = false;
        mIsAutoTest = false;
        mIsTestCompleted = false;
        mAutoTestDelayed = getResources().getInteger(R.integer.auto_test_delayed_time);
        mHandler = new Handler();
        mApplication = (FactoryTestApplication) getApplication();
        mDatabase = FactoryTestDatabase.getInstance(this);
        mPassBt = (Button) findViewById(R.id.bt_pass);
        mFailBt = (Button) findViewById(R.id.bt_fail);
        mBottomButtonContainer = findViewById(R.id.bottom_button_container);

        mPassBt.setOnClickListener(this);
        mFailBt.setOnClickListener(this);

        // 进入测试前，默认设置通过按钮为不可用
        mPassBt.setEnabled(false);
        Intent intent = getIntent();
        mIndex = intent.getIntExtra(Utils.EXTRA_TEST_INDEX, -1);
        mIsAutoTest = intent.getBooleanExtra(Utils.EXTRA_AUTO_TEST, false);
		if (mIndex != -1 && mIndex < mApplication.getTestInfoList().size()) {
            mTestInfo = mApplication.getTestInfoList().get(mIndex);
        } else {
            finish();
        }
       // onNewIntent(getIntent());
    }

    @Override
    protected void onPause() {
        super.onPause();
        // Activity退出后还没有进入下一个测试项，则清除当前自动测试线程
        mHandler.removeCallbacks(mAutoTestRunnable);
    }

    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
 /**      if (mIndex != -1 && mIndex < mApplication.getTestInfoList().size()) {
			mIndex = intent.getIntExtra(Utils.EXTRA_TEST_INDEX, -1);
			mIsAutoTest = intent.getBooleanExtra(Utils.EXTRA_AUTO_TEST, false);
            mTestInfo = mApplication.getTestInfoList().get(mIndex);
        } else {
            finish();
        }*/
    }

    /**
     *　点击界面顶部的Home菜单时，如果当前测试没有完成按测试失败处理，并退出测试模式；
     *  如果当前是自动测试模式，则会退出自动测试模式，直接进入测试结果界面。
     */
    @Override
    public boolean onMenuItemSelected(int featureId, MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                mHandler.removeCallbacks(mAutoTestRunnable);
                if (mIsAutoTest) {
                    Intent result = new Intent(this, TestResultActivity.class);
                    startActivity(result);
                }
                finish();
                break;
        }
        return super.onMenuItemSelected(featureId, item);
    }

    /**
     * 测试过程中屏蔽HOME键、MENU键、返回键（除按键测试外），不允许退出测试模式
     */
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        switch (keyCode) {
            case KeyEvent.KEYCODE_MENU:
            case KeyEvent.KEYCODE_HOME:
            case KeyEvent.KEYCODE_BACK:
                return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    /**
     * 测试过程中屏蔽HOME键、MENU键、返回键（除按键测试外），不允许退出测试模式
     */
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        switch (keyCode) {
            case KeyEvent.KEYCODE_MENU:
            case KeyEvent.KEYCODE_HOME:
            case KeyEvent.KEYCODE_BACK:
                Toast.makeText(this, R.string.exit_test_tip, Toast.LENGTH_SHORT).show();
                return true;
        }
        return super.onKeyUp(keyCode, event);
    }

    @Override
    public void onClick(View view) {
        mHandler.removeCallbacks(mAutoTestRunnable);
        switch (view.getId()) {
            case R.id.bt_pass:
                mIsPass = true;
                break;

            case R.id.bt_fail:
                mIsPass = false;
                break;
        }
        doOnTestCompleted();
    }

    /**
     * 是否是自动测试模式
     * @return 如果是自动测试模式，返回true, 否则返回false
     */
    public boolean isAutoTest() {
        return mIsAutoTest;
    }

    /**
     * 测试完成后执行的操作
     * 如果是自动测试，则跳转到下一个测试项，并结束当前测试项，防止顶层Activity结束后再次看到该Activity.
     * 如果不是自动测试，则直接结束当前测试项
     */
    public void doOnTestCompleted() {
        if (mIsPass) {
            mTestInfo.setTestState(TestState.PASS);
        } else {
            mTestInfo.setTestState(TestState.FAIL);
        }

        mDatabase.setTestState(mTestInfo);

        if (getResources().getBoolean(R.bool.enabled_fighting_test)) {
            fightingTest();
        }

        if (mIsAutoTest) {
            mIndex++;
            if (mIndex < mApplication.getTestInfoList().size()) {
                Utils.startAutoTest(this, mApplication.getTestInfoList(), mIndex);
            }
        }
        finish();
    }

    private void fightingTest() {
        mApplication.updateTestStates();
        ArrayList<TestInfo> testItems = (ArrayList<TestInfo>) mApplication.getTestInfoList().clone();
        TestState result = TestState.PASS;
        boolean isTest = false;
        if (!testItems.isEmpty()) {
            TestInfo item = null;
            TestState state = TestState.UNKNOWN;
            for (int i = 0; i < testItems.size() - 1; i++) {
                item = testItems.get(i);
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
        if (result == TestState.PASS) {
            WriteFmFlagToNV(true);
        } else {
            WriteFmFlagToNV(false);
        }
    }

    private void WriteFmFlagToNV(boolean pass) {
        byte[] buff = null;
        try {
            buff = new byte[1];
            buff[0] = (byte)(pass ? 'P' : 'F');
            try {
                NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_MMI_TEST_FLAG, buff);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
            TestWatermarkService.launcherCheck(this);
        } catch (Exception e){
           Log.d(this, "WriteFmFlagToNV=>error: ", e);
        }
    }

    public long getAutoTestNextTestDelayedTime() {
        return mAutoTestDelayed;
    }

    /**
     * 如果是自动测试模式，测试结束后直接执行该方法
     */
    public void doOnAutoTest() {
        mHandler.removeCallbacks(mAutoTestRunnable);
        mHandler.postDelayed(mAutoTestRunnable, mAutoTestDelayed);
    }

    public void doAtOnceOnAutoTest() {
        mHandler.post(mAutoTestRunnable);
    }

    /**
     * 设置当前测试是否完成
     * @param completed　测试完成状态
     */
    public void setTestCompleted(boolean completed) {
        mIsTestCompleted = completed;
    }

    /**
     * 当前测试是否完成
     * @return 返回当前测试完成状态
     */
    public boolean isTestCompleted() {
        return mIsTestCompleted;
    }

    /**
     * 设置当前测试是否测试通过
     * @param pass　测试通过状态
     */
    public void setTestPass(boolean pass) {
        mIsPass = pass;
    }

    /**
     * 获取当前测试通过状态
     * @return 如果测试通过，返回true；否则返回false
     */
    public boolean isTestPass() {
        return mIsPass;
    }

    /**
     * 设置测试通过按钮显示状态
     * @param visible　要设置的显示状态值，可以是View.GONE、View.INVISIBLE、View.VISIBLE
     */
    public void setPassButtonVisible(int visible) {
        mPassBt.setVisibility(visible);
    }

    /**
     * 设置测试失败按钮显示状态
     * @param visible　要设置的显示状态值，可以是View.GONE、View.INVISIBLE、View.VISIBLE
     */
    public void setFailButtonVisible(int visible) {
        mFailBt.setVisibility(visible);
    }

    /**
     * 设置测试通过按钮是否可用
     * @param enabled　要设置的可用状态，true表示可用，false表示不可用
     */
    public void setPassButtonEnabled(boolean enabled) {
        mPassBt.setEnabled(enabled);
    }

    /**
     * 设置测试失败按钮是否可用
     * @param enabled　要设置的可用状态，true表示可用，false表示不可用
     */
    public void setFailButtonEnabled(boolean enabled) {
        mFailBt.setEnabled(enabled);
    }

    /**
     * 设置测试通过和测试失败按钮容器是否可见　
     * @param visible　要设置的显示状态值，可以是View.GONE、View.INVISIBLE、View.VISIBLE
     */
    public void setBottomButtonVisible(int visible) {
        mBottomButtonContainer.setVisibility(visible);
    }

    public void setActionBarEnabled(boolean enabled) {
        mEnabledActionBar = enabled;
    }

    private Runnable mAutoTestRunnable = new Runnable() {
        @Override
        public void run() {
            doOnTestCompleted();
        }
    };

}
