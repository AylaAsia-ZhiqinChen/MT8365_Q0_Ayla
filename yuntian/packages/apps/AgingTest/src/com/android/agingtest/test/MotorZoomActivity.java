package com.android.agingtest.test;

import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.TimeZone;

import android.view.SurfaceHolder.Callback;
import android.view.View.OnClickListener;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;

import com.android.agingtest.AgingTestInterFace;
import com.android.agingtest.TestUtils;
import com.android.agingtest.R;
import com.android.agingtest.ReportActivity;

import android.content.SharedPreferences.Editor;
import android.content.pm.ActivityInfo;
import android.graphics.PixelFormat;
import android.hardware.Camera;
import android.hardware.Camera.AutoFocusCallback;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.os.Bundle;
import android.os.Handler;
import android.os.PowerManager;
import android.os.SystemClock;
import android.preference.PreferenceManager;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.util.Log;
import android.net.Uri;
import android.os.RemoteException;
import com.mediatek.nvram.NvRAMUtils;

/*
 * author:liut
 */
public class MotorZoomActivity extends Activity implements AgingTestInterFace {

    SurfaceView surfaceView;
    SurfaceHolder surfaceHolder;
    private Camera camera;

    private Button stop;
    private TextView mTime;

    PowerManager pm;
    PowerManager.WakeLock mWakeLock;
    private Camera.Parameters parameters;
    int total_number;
    int rest_nubmer;
    int finish_number;
    private boolean isAutoFocus;
    private boolean focusSuccess = true;
    private SharedPreferences mSharedPreferences;

    private long mMotorTestTime = 0;
    private long mTestTimes = 0;
    private long counter = 0;

    @SuppressWarnings("deprecation")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);// 去掉标题栏
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);// 设置全屏
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD | WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().setFormat(PixelFormat.TRANSLUCENT);
        setContentView(R.layout.motor_zoom_test);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        initValues();
        initViews();
    }

    private void initViews() {
        surfaceView = (SurfaceView) findViewById(R.id.surfaceView1);
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(surfaceHolderCallback); // holder加入回调接口
        // // setType必须设置，要不出错.
        // surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        mSharedPreferences = PreferenceManager
                .getDefaultSharedPreferences(MotorZoomActivity.this);
        stop = (Button) findViewById(R.id.stop_test);
        stop.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                // TODO Auto-generated method stub
                stopTest(false);

            }
        });
        mTime = (TextView) findViewById(R.id.test_time);
    }

    /**
     * 聚焦事件内部类
     */
    AutoFocusCallback autoFocusCallback = new AutoFocusCallback() {

        @Override
        public void onAutoFocus(boolean success, Camera camera) {
            focusSuccess = success;
            if (!success) {
			/*	Toast.makeText(getApplicationContext(),
						R.string.autofocus_failed, Toast.LENGTH_SHORT)
						.show();*/
            }
        }
    };

    /**
     * 聚焦线程
     */
    private Runnable doAutoFocus = new Runnable() {
        @Override
        public void run() {
            if (isAutoFocus) {
                rest_nubmer = rest_nubmer - 1;
                finish_number = total_number - rest_nubmer;

                if (parameters.getZoom() == 10) {
                    parameters.setZoom(0);
                } else {
                    parameters.setZoom(10);
                }

                parameters.setFlashMode(Parameters.FLASH_MODE_AUTO);
                camera.setParameters(parameters);
                camera.autoFocus(autoFocusCallback);
				/*if (!focusSuccess) {
					Toast.makeText(getApplicationContext(),
							R.string.autofocus_failed, Toast.LENGTH_SHORT)
							.show();
				}*/
                mHandler.postDelayed(doAutoFocus, 2000);
                counter++;
                if (isTestOver()) {
                    stopTest(true);
                }
            }
        }
    };

    /**
     * auther:liut SurfaceHolderCallback 创建窗口
     */
    Callback surfaceHolderCallback = new Callback() {
        @Override
        public void surfaceDestroyed(SurfaceHolder arg0) {
            surfaceView = null;
            surfaceHolder = null;
        }

        @Override
        public void surfaceCreated(SurfaceHolder arg0) {
            // TODO Auto-generated method stub
            surfaceHolder = arg0;
            camera = Camera.open();
            parameters = camera.getParameters();
            // 进入预览模式
            try {
                if (camera != null) {
                    Camera.Parameters parameters = camera.getParameters();
//					parameters.setPreviewSize(800, 600);
                    camera.setParameters(parameters);
                    camera.setDisplayOrientation(90);
                    camera.setPreviewDisplay(surfaceHolder);
                    camera.startPreview();
                    startTest();
                }
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }
        }

        @Override
        public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2,
                                   int arg3) {
            // TODO Auto-generated method stub
            surfaceHolder = arg0;
        }
    };

    // 屏蔽返回键
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        // TODO Auto-generated method stub
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    protected void onResume() {
        // TODO Auto-generated method stub
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        super.onDestroy();

    }

    private long mStartTime = 0;
    private static final int UPDATE_TIME = 0;
    private Handler mHandler = new Handler() {
        public void handleMessage(android.os.Message msg) {
            if (msg.what == UPDATE_TIME) {
                long testTime = System.currentTimeMillis() - mStartTime;
                SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
                sdf.setTimeZone(TimeZone.getTimeZone("GMT-0:00"));
                mTime.setText(sdf.format(new Date(testTime)));
                mHandler.sendEmptyMessageDelayed(UPDATE_TIME, 1000);
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
        key = TestUtils.MOTORZOOM_KEY;
        pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
//		mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,"test");
//		mWakeLock.acquire();// 设置休眠锁
        pm.wakeUp(SystemClock.uptimeMillis());
        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        mTestTimes = mSharedPreferences.getInt(TestUtils.MOTORZOOM_KEY + TestUtils.TEST_TIME, 50);
        if (getResources().getBoolean(R.bool.use_total_test_time)) {
            mTestTimes = mSharedPreferences.getInt("singleTestTime", 50)* TestUtils.MILLSECOND;
        }
    }

    @Override
    public void startTest() {
        // TODO Auto-generated method stub
        mStartTime = System.currentTimeMillis();
        counter = 0;
        isAutoFocus = true;
        camera.autoFocus(autoFocusCallback);
        if (!focusSuccess) {
            Toast.makeText(getApplicationContext(), R.string.autofocus_failed,
                    Toast.LENGTH_SHORT).show();
        }
        mHandler.postDelayed(doAutoFocus, 2000);
        mHandler.sendEmptyMessage(UPDATE_TIME);
    }

    @Override
    public void stopTest(boolean isOk) {
        // TODO Auto-generated method stub
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        if (!pm.isScreenOn()) {
            pm.wakeUp(SystemClock.uptimeMillis());
        }
        Log.e("lsz", "MotorZoom stop");
        isAutoFocus = false;
        mHandler.removeMessages(UPDATE_TIME);
        mHandler.removeCallbacks(doAutoFocus);
        if (camera != null) {
            camera.stopPreview();
            camera.release();
            camera = null;
        }
        Editor e = mSharedPreferences.edit();
        e.putInt(TestUtils.MOTORZOOM_KEY + TestUtils.TEST_RESULT, isOk ? 1 : 0);
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
                    NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_MOTOR, buff);
                } catch (RemoteException e4) {

                }
        startNext();
        Log.e("lsz", "MotorZoom finish");
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
        Log.e("lsz", "counter->" + counter + ",mTestTime->" + mTestTimes);

      boolean b = System.currentTimeMillis() - mStartTime >= mTestTimes;
        return b;
     
      //  return counter >= mTestTimes;
    }
}
