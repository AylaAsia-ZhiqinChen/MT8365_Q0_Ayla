package com.android.factorytest;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.os.IBinder;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.TextView;
import com.mediatek.nvram.NvRAMUtils;

import com.android.factorytest.model.TestInfo;
import com.android.factorytest.model.TestState;
import java.util.ArrayList;

public class TestWatermarkService extends Service {
    private WindowManager mWm;
    private TextView mFloatView;
    private boolean mIsSMT = false;

    public static final String ACTION_RESET = "action_reset";
    public static final String ACTION_ENTER_FM = "action_enter_fm";
    public static final String ACTION_EXIT_FM = "action_exit_fm";
    public static final String ACTION_CHECK = "action_check";

    public static void launcherReset(Context c) {
        Intent runIntent = new Intent(c, TestWatermarkService.class);
        runIntent.putExtra("action", ACTION_RESET);
        c.startService(runIntent);
    }

    public static void launcherShowFm(Context c) {
        Intent runIntent = new Intent(c, TestWatermarkService.class);
        runIntent.putExtra("action", ACTION_ENTER_FM);
        c.startService(runIntent);
    }

    public static void launcherHileFm(Context c) {
        Intent runIntent = new Intent(c, TestWatermarkService.class);
        runIntent.putExtra("action", ACTION_EXIT_FM);
        c.startService(runIntent);
    }

    public static void launcherCheck(Context c) {
        Intent runIntent = new Intent(c, TestWatermarkService.class);
        runIntent.putExtra("action", ACTION_CHECK);
        c.startService(runIntent);
    }

    @Override
    public void onCreate() {
        super.onCreate();
        mWm = ((WindowManager) this.getSystemService(Context.WINDOW_SERVICE));
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        hideFloatView();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        handleCommand(intent);

        return START_STICKY;
    }

    private void handleCommand(Intent intent) {
        mIsSMT = isHasSMT();
        if (null == intent) {
            if (mFloatView == null) {
                stopSelf();
            }
            return;
        }

        String action = intent.getStringExtra("action");
        if (ACTION_ENTER_FM.equals(action)) {
            if (!mIsSMT) {
                String infoStr = "SMT测试未通过";
                if (!isHasFm()) {
                    infoStr += "MMI测试未通过";
                }
                showFloatView(infoStr);
            }
        } else if (ACTION_EXIT_FM.equals(action)) {
            if (!mIsSMT) {
                hideFloatView();
            }
        } else if (ACTION_CHECK.equals(action)) {
            if (isHasFm()) {
                if (mIsSMT) {
                    hideFloatView();
                } else {
                    showFloatView("SMT测试未通过");
                }
            } else {
                if (mIsSMT) {
                    showFloatView("MMI测试未通过");
                } else {
                    showFloatView("SMT测试未通过MMI测试未通过");
                }
            }
        } else if (ACTION_RESET.equals(action)) {
            boolean isTest = isTest();
            boolean isFm = isHasFm();
            Log.d(this, "handleCommand=>test: " + isTest + ", hasFm: " + isFm);
            if (!isTest && isFm) {
                resetToPass();
            }
            if (mIsSMT && !isHasFm()) {
                showFloatView("MMI测试未通过");
            }
        }
        if (mFloatView == null) {
            stopSelf();
        }
    }

    private void showFloatView(String txt) {
        if (null == mFloatView) {
            WindowManager.LayoutParams layoutparams;
            layoutparams = new WindowManager.LayoutParams();
            layoutparams.type = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY;
            layoutparams.flags = WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL
                    | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                    | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE;
            layoutparams.width = ViewGroup.LayoutParams.WRAP_CONTENT;
            layoutparams.height = ViewGroup.LayoutParams.WRAP_CONTENT;
            layoutparams.format = PixelFormat.TRANSLUCENT;
            layoutparams.gravity = Gravity.TOP | Gravity.LEFT;
            DisplayMetrics dm = new DisplayMetrics();
            mWm.getDefaultDisplay().getMetrics(dm);
            layoutparams.x = 0;
            layoutparams.y = 0;

            mFloatView = new TextView(this);
            mFloatView.setBackgroundResource(0x00000000);
            mFloatView.setTextColor(0xffff0000);
            mFloatView.setTextSize(16);

            try {
                mWm.addView(mFloatView, layoutparams);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        mFloatView.setText(txt);
        mFloatView.setVisibility(View.VISIBLE);
    }

    private void hideFloatView() {
        if (mFloatView != null) {
            mWm.removeView(mFloatView);
            mFloatView = null;
        }
    }

    private boolean isHasSMT() {
        //yuntian longyao add
        //Description:暗码控制隐藏和显示水印
        try {
            byte[] secretCode = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_SHOW_HIDE_WATERMARK_FLAG, NvRAMUtils.SHOW_HIDE_WATERMARK_FLAG_LENGTH);
            Log.d(this, "SMT secretCode=>value: " + new String(secretCode));
            if (secretCode[0] == 'P') {
                return true;
            }
        } catch (Exception e) {
            Log.e(this, "SMT get show hide watermark flag=>error: ", e);
        }
        //yuntian longyao end
        try {
            byte[] buff = NvRAMUtils.readNV(NvRAMUtils.INDEX_FACTORY_TEST_FLAG, NvRAMUtils.FACTORY_TEST_FLAG_LENGTH);
            Log.d(this, "isHasSMT=>value: " + new String(buff));
            if (buff[0] == 'P') {
                return true;
            } else {
                byte[] buff_yt = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_FACTORY_TEST_NEW_FLAG, NvRAMUtils.FACTORY_TEST_FLAG_NEW_LENGTH);
                Log.d(this, "isHasSMTYt=>value: " + new String(buff_yt));
                if (buff_yt[0] == 'P') {
                    return true;
                }
            }
        } catch (Exception e) {
            Log.e(this, "get smt test flag=>error: ", e);
        }
        if (getResources().getBoolean(R.bool.support_hide_water_mark)) {
            return true;
        }

        return false;
    }

    private boolean isHasFm() {
        //yuntian longyao add
        //Description:暗码控制隐藏和显示水印
        try {
            byte[] secretCode = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_SHOW_HIDE_WATERMARK_FLAG, NvRAMUtils.SHOW_HIDE_WATERMARK_FLAG_LENGTH);
            Log.d(this, "MMI secretCode=>value: " + new String(secretCode));
            if (secretCode[0] == 'P') {
                return true;
            }
        } catch (Exception e) {
            Log.e(this, "MMI show hide watermark flag=>error: ", e);
        }
        //yuntian longyao end
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_MMI_TEST_FLAG, NvRAMUtils.MMI_TEST_FLAG_LENGTH);
            Log.d(this, "isHasFm=>value: " + new String(buff));
            if (buff[0] == 'P') {
                return true;
            }
        } catch (Exception e) {
            Log.e(this, "get mmi test flag=>error: ", e);
        }
        if (getResources().getBoolean(R.bool.support_hide_water_mark)) {
            return true;
        }

        return false;
    }

    private boolean isTest() {
        boolean isTest = false;
        FactoryTestApplication application = (FactoryTestApplication) getApplication();
        application.updateTestStates();
        ArrayList<TestInfo> testItems = (ArrayList<TestInfo>) application.getTestInfoList().clone();
        if (!testItems.isEmpty()) {
            TestInfo item = null;
            TestState state = TestState.UNKNOWN;
            for (int i = 0; i < testItems.size(); i++) {
                item = testItems.get(i);
                state = FactoryTestDatabase.getInstance(this).getTestState(item.getTestTitleResId());
                if (TestState.FAIL.equals(state)) {
                    if (!isTest) {
                        isTest = true;
                    }
                    break;
                } else if (TestState.PASS.equals(state)) {
                    if (!isTest) {
                        isTest = true;
                    }
                }
            }
        }
        Log.d(this, "getAllTestState=>isTest: " + isTest);
        return isTest;
    }

    private void resetToPass() {
        FactoryTestApplication application = (FactoryTestApplication) getApplication();
        FactoryTestDatabase db = FactoryTestDatabase.getInstance(this);
        ArrayList<TestInfo> testInfos = application.getTestInfoList();
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_MMI_TEST_FLAG, NvRAMUtils.MMI_TEST_FLAG_LENGTH);
            if (buff[0] != 'P') {
                for (int i = 0; i < testInfos.size() - 1; i++) {
                    testInfos.get(i).setTestState(TestState.UNKNOWN);
                    db.setTestState(testInfos.get(i));
                }
                return;
            }
        } catch (Exception e) {
            Log.e(this, "get mmi test flag=>error: ", e);
            return;
        }
        for (int i = 0; i < testInfos.size() - 1; i++) {
            testInfos.get(i).setTestState(TestState.PASS);
            db.setTestState(testInfos.get(i));
        }
    }
}
