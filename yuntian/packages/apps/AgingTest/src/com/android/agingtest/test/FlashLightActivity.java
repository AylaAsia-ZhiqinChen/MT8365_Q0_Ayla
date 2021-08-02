package com.android.agingtest.test;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.TimeZone;

import com.android.agingtest.AgingTestInterFace;
import com.android.agingtest.Log;
import com.android.agingtest.TestUtils;
import com.android.agingtest.R;
import com.android.agingtest.ReportActivity;

import android.app.ActionBar;
import android.app.Activity;
import android.app.KeyguardManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.os.Bundle;
import android.os.Handler;
import android.os.PowerManager;
import android.os.SystemClock;
import android.preference.PreferenceManager;
import android.view.KeyEvent;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.os.RemoteException;
import com.mediatek.nvram.NvRAMUtils;

public class FlashLightActivity extends Activity implements OnClickListener, AgingTestInterFace {

    public static final int FLAG_HOMEKEY_DISPATCHED = 0x80000000;

    private static final int MSG_UPDATE_TIME = 0;
    private static final int FLASH_SWITCH = 1;
    private static final int TORCH_TEST = 2;

    private TextView mTestTimeTv;
    private Button mStopBt, flashBtn, torchBtn;
    private SharedPreferences mSharedPreferences;
    private PowerManager.WakeLock mLock;
    private PowerManager mPowerManager;

    private int mWakeUpDelayed = 2000;
    private long mFlashTime;
    private long mStartTime;
    private KeyguardManager keyguardManager;

    private static final int TORCH_MODE = 0;
    private static final int FLASH_MODE = 1;
    private int testMode = -1;
    private String mCameraId;
    private static final String TAG = "FlashLightActivity";
    private CameraManager mCameraManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getWindow().addFlags(FLAG_HOMEKEY_DISPATCHED);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD | WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        super.onCreate(savedInstanceState);
        initActionBar();
        setContentView(R.layout.flashlight_test);
        initValues();
        initViews();
        mCameraManager = (CameraManager) getSystemService(this.CAMERA_SERVICE);
        tryInitCamera();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mLock.acquire();
        startTest();
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mLock.isHeld()) {
            mLock.release();
        }
    }

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

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.stop_test:
                stopTest(false);
                break;
            case R.id.flash_mode:
                updateTestMode(FLASH_MODE);
                break;
            case R.id.torch_mode:
                updateTestMode(TORCH_MODE);
                break;
        }
    }

    private void updateTestMode(int testMode) {
        switch (testMode) {
            case FLASH_MODE:
                mHandler.sendEmptyMessage(FLASH_SWITCH);
                break;
            case TORCH_MODE:
                mHandler.sendEmptyMessage(TORCH_TEST);
                break;
            default:
                break;
        }
    }

    private void initActionBar() {
        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
    }


    private void initViews() {
        mTestTimeTv = (TextView) findViewById(R.id.test_time);
        mStopBt = (Button) findViewById(R.id.stop_test);

        mTestTimeTv.setText(R.string.default_time_string);
        mStopBt.setOnClickListener(this);

        flashBtn = (Button) findViewById(R.id.flash_mode);
        torchBtn = (Button) findViewById(R.id.torch_mode);

        flashBtn.setOnClickListener(this);
        torchBtn.setOnClickListener(this);
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(android.os.Message msg) {
            switch (msg.what) {
                case MSG_UPDATE_TIME:
                    long testTime = System.currentTimeMillis() - mStartTime;
                    SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
                    sdf.setTimeZone(TimeZone.getTimeZone("GMT-0:00"));
                    mTestTimeTv.setText(sdf.format(new Date(testTime)));
                    if (isTestOver()) {
                        stopTest(true);
                    } else {
                        mHandler.sendEmptyMessageDelayed(MSG_UPDATE_TIME, 1000);
                    }
                    break;

                case FLASH_SWITCH:
                    turnOnFlash();
                    if (!isTestOver()) {
                        mHandler.sendEmptyMessageDelayed(FLASH_SWITCH, 1000);
                    }
                    break;

                case TORCH_TEST:
                    mHandler.removeMessages(FLASH_SWITCH);
                    turnOnTorch();
                    break;
            }
        };
    };

    private int[] alltestindex = null;
    private int key_index = -1;
    String key;

    @Override
    public void initValues() {
        // TODO Auto-generated method stub

        key_index = getIntent().getIntExtra(TestUtils.KEY_INDEX, -1);
        alltestindex = getIntent().getIntArrayExtra(TestUtils.ALLTESTKEYINDEX);
        Log.e("lsz", "key_index->" + key_index + ",alltestindex[key_index]->" + TestUtils.ALLKEYS[alltestindex[key_index]]);
        key = TestUtils.FLASH_KEY;
        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        mWakeUpDelayed = getResources().getInteger(R.integer.wake_up_delayed);
        mPowerManager = ((PowerManager) getSystemService(Context.POWER_SERVICE));
        mLock = mPowerManager.newWakeLock(1, "sprocomm");

        keyguardManager = (KeyguardManager) getSystemService(KEYGUARD_SERVICE);

        mFlashTime = mSharedPreferences.getInt(key + TestUtils.TEST_TIME, 10) * TestUtils.MILLSECOND;
        if (getResources().getBoolean(R.bool.use_total_test_time)) {
            mFlashTime = mSharedPreferences.getInt("singleTestTime", 10) * TestUtils.MILLSECOND;
        }
    }

    @Override
    public void startTest() {
        // TODO Auto-generated method stub
        turnOnFlash();
        mStartTime = System.currentTimeMillis();
        mHandler.sendEmptyMessage(MSG_UPDATE_TIME);
        mHandler.sendEmptyMessage(FLASH_SWITCH);
    }

    @Override
    public void stopTest(boolean isOk) {
        // TODO Auto-generated method stub
        turnOffTorch();
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        mHandler.removeMessages(MSG_UPDATE_TIME);
        mHandler.removeMessages(FLASH_SWITCH);
        long testTime = System.currentTimeMillis() - mStartTime;
        Editor e = mSharedPreferences.edit();
        e.putInt(TestUtils.FLASH_KEY + TestUtils.TEST_RESULT, isOk ? 1 : 0);
        e.commit();
        byte result = -1;
        if (isOk) {
            result = -2; //成功-2
        } else {
            result = -3; //失败-3
        }
        byte[] buff = new byte[1];
        buff[0] = result;
        try {
            NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_FLASH, buff);
        } catch (RemoteException e4) {

        }
        startNext();
        finish();
    }

    private void startNext() {
        Intent intent = null;
        int currentIndex = key_index + 1;
        if (currentIndex < alltestindex.length) {
            String nexKey = TestUtils.ALLKEYS[alltestindex[currentIndex]];
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


    @Override
    public boolean isTestOver() {
        // TODO Auto-generated method stub
        return System.currentTimeMillis() - mStartTime >= mFlashTime;
    }

    private boolean isFlashOn = false;
    private Camera mCamera;

    private void turnOnTorch() {
        isFlashOn = true;
        setFlashlight(true);
    }

    private void turnOnFlash() {
        if (!isFlashOn) {
            setFlashlight(true);
            isFlashOn = true;
        } else {
            setFlashlight(false);
            isFlashOn = false;
        }
    }

    private void turnOffTorch() {
        setFlashlight(false);
    }

    private void tryInitCamera() {
        try {
            mCameraId = getCameraId();
        } catch (Throwable e) {
            Log.e(TAG, "Couldn't initialize.", e);
            return;
        }
    }

    private String getCameraId() throws CameraAccessException {
        String[] ids = mCameraManager.getCameraIdList();
        for (String id : ids) {
            CameraCharacteristics c = mCameraManager.getCameraCharacteristics(id);
            Boolean flashAvailable = c.get(CameraCharacteristics.FLASH_INFO_AVAILABLE);
            Integer lensFacing = c.get(CameraCharacteristics.LENS_FACING);
            if (flashAvailable != null && flashAvailable
                    && lensFacing != null && lensFacing == CameraCharacteristics.LENS_FACING_BACK) {
                return id;
            }
        }
        return null;
    }

    public void setFlashlight(boolean enabled) {
        synchronized (this) {
            if (mCameraId == null) return;
            try {
                mCameraManager.setTorchMode(mCameraId, enabled);
            } catch (CameraAccessException e) {
                Log.e(TAG, "Couldn't set torch mode", e);
            }
        }

    }

}
