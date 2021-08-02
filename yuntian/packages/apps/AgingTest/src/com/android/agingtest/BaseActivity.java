package com.android.agingtest;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.TimeZone;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.os.Handler;
import android.os.PowerManager;
import android.os.SystemClock;
import android.preference.PreferenceManager;
import android.view.KeyEvent;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.os.RemoteException;
import com.mediatek.nvram.NvRAMUtils;

public class BaseActivity extends Activity implements AgingTestInterFace {
    //	所有需要测试的key的索引
    private int[] alltestindex = null;
    //	当前测试的key的在alltestindex中的索引
    private int key_index = -1;
    String key;
    protected SharedPreferences mSharedPreferences;
    public long mStartTime = 0;
    private long mTestTime = 0;
    private Button stopBtn;
    protected TextView timeTextView;
    public PowerManager.WakeLock mLock;
    public PowerManager mPowerManager;

    protected boolean isStartTest = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD | WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        super.onCreate(savedInstanceState);

        setContentView(R.layout.base_layout);
        key_index = getIntent().getIntExtra(TestUtils.KEY_INDEX, -1);
        alltestindex = getIntent().getIntArrayExtra(TestUtils.ALLTESTKEYINDEX);
        Log.e("lsz", "key_index->" + key_index);
        if (key_index != -1) {
            key = TestUtils.ALLKEYS[alltestindex[key_index]];
        } else {
            Log.e("lsz", "no this test key");
            finish();
        }
        initValues();
        stopBtn = (Button) findViewById(R.id.stop_test);
        stopBtn.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                stopTest(false);
            }
        });
        timeTextView = (TextView) findViewById(R.id.test_time);
    }

    @Override
    public void initValues() {
        // TODO Auto-generated method stub
        mPowerManager = ((PowerManager) getSystemService(Context.POWER_SERVICE));
        mPowerManager.wakeUp(SystemClock.uptimeMillis());
//		mLock = mPowerManager.newWakeLock(1, "sprocomm");

        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        mTestTime = mSharedPreferences.getInt(key + TestUtils.TEST_TIME, 10) * TestUtils.MILLSECOND;
        if (getResources().getBoolean(R.bool.use_total_test_time)) {
            mTestTime = mSharedPreferences.getInt("singleTestTime", 10) * TestUtils.MILLSECOND;
        }
    }

    @Override
    public void startTest() {
        // TODO Auto-generated method stub
        isStartTest = true;
//		mLock.acquire();
        mStartTime = System.currentTimeMillis();
        mHandler.sendEmptyMessage(UPDATE_TIME);
        mHandler.sendEmptyMessage(HOW_TO_TEST);
    }

    @Override
    public void stopTest(boolean isOk) {
        // TODO Auto-generated method stub
//		mLock.release();
        mHandler.removeMessages(UPDATE_TIME);
        mHandler.removeMessages(HOW_TO_TEST);
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        if (!mPowerManager.isScreenOn()) {
            mPowerManager.wakeUp(SystemClock.uptimeMillis());
        }
        stopBtn.setEnabled(false);
        Editor e = mSharedPreferences.edit();
        e.putInt(key + TestUtils.TEST_RESULT, isOk ? 1 : 0);
        byte result = -1;
        if (isOk) {
            result = -2; //成功-2
        } else {
            result = -3; //失败-3
        }
        byte[] buff = new byte[1];
        buff[0] = result;
        switch (key) {

            case TestUtils.REBOOT_KEY:
                try {
                    NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_REBOOT, buff);
                } catch (RemoteException e1) {
                }
                break;

            case TestUtils.SLEEP_KEY:
                try {
                    NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_SLEEP, buff);
                } catch (RemoteException e2) {

                }

                break;

            case TestUtils.VIBRATE_KEY:
                try {
                    NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_VIBRATION, buff);
                } catch (RemoteException e3) {

                }
                break;

            case TestUtils.FRONT_CAM_KEY:
                try {
                    NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_FRONT_CAMERA, buff);
                } catch (RemoteException e4) {

                }
                break;

            case TestUtils.PLAY_VIDEO_KEY:
                try {
                    NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_VIDEO, buff);
                } catch (RemoteException e5) {

                }
                break;


            case TestUtils.BACK_CAM_KEY:
                try {
                    NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_BACK_CAMERA, buff);
                } catch (RemoteException e6) {

                }

                break;


            case TestUtils.MOTORZOOM_KEY:
                try {
                    NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_MOTOR, buff);
                } catch (RemoteException e7) {

                }

                break;


            case TestUtils.FLASH_KEY:
                try {
                    NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_FLASH, buff);
                } catch (RemoteException e8) {

                }
                break;


            case TestUtils.LOUDSPEAKER_KEY:
                try {
                    NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_SPEAKER, buff);
                } catch (RemoteException e9) {

                }
                break;


            case TestUtils.SCREEN_KEY:
                try {
                    NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_SCREEN, buff);
                } catch (RemoteException e10) {

                }
                break;

            case TestUtils.RECEIVER_KEY:
                try {
                    NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_RECEIVER, buff);
                } catch (RemoteException e10) {

                }
                break;

            default:
                break;

        }

        Log.e("lsz", "stopTest key->" + key);
        e.commit();
        startNext();
    }


    @Override
    protected void onResume() {
        // TODO Auto-generated method stub
        super.onResume();
        if (!isStartTest) {
            startTest();
        }
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        super.onPause();

    }


    @Override
    public boolean isTestOver() {
        // TODO Auto-generated method stub
        boolean isTestOver = System.currentTimeMillis() - mStartTime >= mTestTime;
        Log.e("lsz", "isTestOver-->" + isTestOver);
        return isTestOver;
    }


    public void startNext() {
        Intent intent = null;
        int currentIndex = key_index + 1;
        Log.e("lsz", "startNext-currentIndex->" + currentIndex + ",alltestindex->" + alltestindex.length);
        if (currentIndex < alltestindex.length) {
            String nexKey = TestUtils.ALLKEYS[alltestindex[currentIndex]];
            Log.e("lsz", "key->" + nexKey + ",alltestindex->" + alltestindex.toString());
            intent = new Intent(this, TestUtils.ALLCLASSES[alltestindex[currentIndex]]);
            intent.putExtra(TestUtils.ALLTESTKEYINDEX, alltestindex);
            intent.putExtra(TestUtils.KEY_INDEX, currentIndex);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
            startActivity(intent);
        } else {

            intent = new Intent(this, ReportActivity.class);
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);
            startActivity(intent);
        }
        finish();
    }

    public void doTest() {
        Log.e("lsz", "doTest-->");
    }

    public static final int UPDATE_TIME = 0;
    public static final int HOW_TO_TEST = 1;
    public Handler mHandler = new Handler() {
        public void handleMessage(android.os.Message msg) {
            Log.e("lsz", "handleMessage-what->" + msg.what);
            switch (msg.what) {
                case UPDATE_TIME:
                    long testTime = System.currentTimeMillis() - mStartTime;
                    SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
                    sdf.setTimeZone(TimeZone.getTimeZone("GMT-0:00"));
                    timeTextView.setText(sdf.format(new Date(testTime)));
                    Log.d("SleepActivity", "handleMessag=>testTime: " + testTime);
                    if (isTestOver()) {
                        stopTest(true);
                    } else {
                        mHandler.sendEmptyMessageDelayed(UPDATE_TIME, 1000);
                    }
                    break;
                case HOW_TO_TEST:
                    doTest();
                default:
                    break;
            }
        };
    };


    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        Log.d(this, "dispatchKeyEvent=>keycode: " + event.getKeyCode());
        switch (event.getKeyCode()) {
            case KeyEvent.KEYCODE_BACK:
            case KeyEvent.KEYCODE_HOME:
                Toast.makeText(this, R.string.testing_tip, Toast.LENGTH_SHORT).show();
                return true;
        }
        return super.dispatchKeyEvent(event);
    }


}
