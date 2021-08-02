package com.android.agingtest.test;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TimeZone;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.util.DisplayMetrics;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.android.agingtest.BaseActivity;

import android.view.View.OnClickListener;

import com.android.agingtest.TestUtils;
import com.android.agingtest.R;

import android.widget.Button;
import android.widget.TextView;
import android.os.PowerManager.WakeLock;
import android.view.View;
import android.view.WindowManager;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

import com.android.agingtest.ReportActivity;

import android.app.ActionBar;
import android.view.KeyEvent;
import android.widget.Toast;
import android.view.View;

import com.android.agingtest.Log;

import android.preference.PreferenceManager;
import android.os.PowerManager;
import android.os.RemoteException;
import com.mediatek.nvram.NvRAMUtils;

public class ScreenActivity extends Activity implements OnClickListener, SurfaceHolder.Callback {

    public static final int FLAG_HOMEKEY_DISPATCHED = 0x80000000;


    private boolean mainThreadExitFlag;        //线程结束标志
    private SurfaceHolder mSurfaceHolder = null;
    private SurfaceView mSurfaceView = null;
    private static final int UPDATE_MSG = 1;    //自定义消息
    private static final int LOOP_MSG = 2;    //自定义消息
    private TextView mTestTimeTitleTv;
    private TextView mTestTimeTv;
    private Button mStopBt;
    private WakeLock mLock;
    private PowerManager mPowerManager;
    private SharedPreferences mSharedPreferences;
    private long mScreenTestTime;
    private long mStartTime;
    private volatile MyThread myThread;

    @Override
    public void onCreate(Bundle savedInstanceState) {

        super.onCreate(savedInstanceState);
        getWindow().addFlags(FLAG_HOMEKEY_DISPATCHED);
        getWindow().addFlags(View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().addFlags(android.view.WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD);

        initActionBar();
        setContentView(R.layout.activity_screen_test);
        mainThreadExitFlag = false;
        mSurfaceView = (SurfaceView) findViewById(R.id.lcd_preview);
        initValues();
        initViews();
    }

    private void startTest() {
        mStartTime = System.currentTimeMillis();
        mHandler.sendEmptyMessage(UPDATE_MSG);
    }

    private void stopTest(boolean isOk) {
        mainThreadExitFlag = false;
        mHandler.removeMessages(UPDATE_MSG);
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        Editor e = mSharedPreferences.edit();
        e.putInt(TestUtils.SCREEN_KEY + TestUtils.TEST_RESULT, isOk ? 1 : 0);
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
            NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_SCREEN, buff);
        } catch (RemoteException e4) {

        }
        startNext();
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

    public boolean isTestOver() {
        // TODO Auto-generated method stub
        boolean b = System.currentTimeMillis() - mStartTime >= mScreenTestTime;
        return b;
    }

    private void initActionBar() {
        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
    }

    public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {
        mSurfaceHolder = arg0;
    }

    public void surfaceCreated(SurfaceHolder holder) {
        if (null != myThread) {
            myThread.stopMyThread();
        }
        myThread = new MyThread(1); // 1秒以后启动线程
        myThread.start();
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        mSurfaceHolder = null;
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mSurfaceView != null) {
            mSurfaceHolder = mSurfaceView.getHolder();
            mSurfaceHolder.addCallback(this);
            mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        }
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
        }
    }

    private int[] alltestindex = null;
    private int key_index = -1;
    String key;

    private void initValues() {

        key_index = getIntent().getIntExtra(TestUtils.KEY_INDEX, -1);
        alltestindex = getIntent().getIntArrayExtra(TestUtils.ALLTESTKEYINDEX);
        Log.e("lsz", "key_index->" + key_index + ",alltestindex[key_index]->" + TestUtils.ALLKEYS[alltestindex[key_index]]);
        key = TestUtils.SCREEN_KEY;
        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        mPowerManager = (PowerManager) getSystemService(Context.POWER_SERVICE);
        mLock = mPowerManager.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "screen_test");
        mLock.acquire();


        mScreenTestTime = mSharedPreferences.getInt(TestUtils.SCREEN_KEY + TestUtils.TEST_TIME,
                getResources().getInteger(R.integer.default_screen_time)) * TestUtils.MILLSECOND;

        if (getResources().getBoolean(R.bool.use_total_test_time)) {
            mScreenTestTime = mSharedPreferences.getInt("singleTestTime", 20) * TestUtils.MILLSECOND;
        }
        Log.d(this, "initValues=>startTime: " + mStartTime + " time: " + mScreenTestTime);
    }

    private void initViews() {
        mTestTimeTv = (TextView) findViewById(R.id.test_time);
        mTestTimeTitleTv = (TextView) findViewById(R.id.test_time_title);
        mStopBt = (Button) findViewById(R.id.stop_test);
        mSurfaceView = (SurfaceView) findViewById(R.id.lcd_preview);

        mTestTimeTv.setText(R.string.default_time_string);
        mStopBt.setOnClickListener(this);
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case UPDATE_MSG:
                    long testTime = System.currentTimeMillis() - mStartTime;
                    SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
                    sdf.setTimeZone(TimeZone.getTimeZone("GMT-0:00"));
                    mTestTimeTv.setText(sdf.format(new Date(testTime)));
                    if (isTestOver()) {
                        stopTest(true);
                        mHandler.removeMessages(UPDATE_MSG);
                        mainThreadExitFlag = true;
                    } else {
                        mHandler.sendEmptyMessageDelayed(UPDATE_MSG, 1000);
                    }
                    break;
                case LOOP_MSG:
                    if (null != myThread) {
                        myThread.stopMyThread();
                    }
                    myThread = new MyThread(0); // 0秒以后启动线程
                    myThread.start();
                    break;
            }
        }
    };


    class MyThread extends Thread {
        private int daily = 0;

        public MyThread(int daily) {
            this.daily = daily;
        }

        void LcdDelay() {
            try {
                Thread.sleep(1000);    //每种颜色延时1秒
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        void stopMyThread() {
            MyThread tmpThread = myThread;
            myThread = null;
            if (tmpThread != null) {
                tmpThread.interrupt();
            }
        }

        void DrawColor(Paint mpaint, int color) {
            DisplayMetrics displayMetrics = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
            int height = displayMetrics.heightPixels;
            int width = displayMetrics.widthPixels;    //获得当前LCD的宽高
            if (mSurfaceHolder == null) {
                return;
            }
            Canvas canvas = mSurfaceHolder.lockCanvas(null);
            mpaint.setColor(color);
            canvas.drawRect(new RectF(0, 0, width, height), mpaint);    //在画布上以特定颜色画矩形

            if (mSurfaceHolder != null) {
                mSurfaceHolder.unlockCanvasAndPost(canvas);    //刷屏
            } else {
                return;
            }
        }

        public void run() {
            if (mainThreadExitFlag) {    //如果中间有线程退出标志被置，退出当前线程
                return;
            }
            if (myThread == null) {
                return; // stopped before started.
            }
            try {
                for (int i = 0; i < daily; i++) {
                    sleep(1000);
                }
                Thread.yield(); // let another thread have some time perhaps to stop this one.
                if (Thread.currentThread().isInterrupted()) {
                    throw new InterruptedException("Stopped by ifInterruptedStop()");
                }
                //do something add
                Paint mPaint = new Paint();
                DrawColor(mPaint, Color.RED);    //分别刷各种颜色
                mTestTimeTitleTv.setTextColor(Color.GREEN);
                mTestTimeTv.setTextColor(Color.GREEN);
                if (mainThreadExitFlag) {
                    return;
                }
                LcdDelay();
                if (mainThreadExitFlag) {
                    return;
                }
                DrawColor(mPaint, Color.BLUE);
                mTestTimeTitleTv.setTextColor(Color.RED);
                mTestTimeTv.setTextColor(Color.RED);
                if (mainThreadExitFlag) {
                    return;
                }
                LcdDelay();
                if (mainThreadExitFlag) {
                    return;
                }
                DrawColor(mPaint, Color.GREEN);
                mTestTimeTitleTv.setTextColor(Color.BLUE);
                mTestTimeTv.setTextColor(Color.BLUE);
                if (mainThreadExitFlag) {
                    return;
                }
                LcdDelay();
                if (mainThreadExitFlag) {
                    return;
                }
                DrawColor(mPaint, Color.WHITE);
                mTestTimeTitleTv.setTextColor(Color.BLACK);
                mTestTimeTv.setTextColor(Color.BLACK);
                if (mainThreadExitFlag) {
                    return;
                }
                LcdDelay();
                if (mainThreadExitFlag) {
                    return;
                }
                DrawColor(mPaint, Color.BLACK);
                mTestTimeTitleTv.setTextColor(Color.WHITE);
                mTestTimeTv.setTextColor(Color.WHITE);
                if (mainThreadExitFlag) {
                    return;
                }
                LcdDelay();
                if (mainThreadExitFlag) {
                    return;
                }
                DrawColor(mPaint, Color.CYAN);
                mTestTimeTitleTv.setTextColor(Color.BLUE);
                mTestTimeTv.setTextColor(Color.BLUE);
                if (mainThreadExitFlag) {
                    return;
                }
                LcdDelay();
                if (mainThreadExitFlag) {
                    return;
                }
                DrawColor(mPaint, Color.YELLOW);
                mTestTimeTitleTv.setTextColor(Color.RED);
                mTestTimeTv.setTextColor(Color.RED);
                if (mainThreadExitFlag) {
                    return;
                }
                LcdDelay();
                if (mainThreadExitFlag) {
                    return;
                }
                DrawColor(mPaint, Color.GRAY);
                mTestTimeTitleTv.setTextColor(Color.GREEN);
                mTestTimeTv.setTextColor(Color.GREEN);
                if (mainThreadExitFlag) {
                    return;
                }
                LcdDelay();
                if (mainThreadExitFlag) {
                    return;
                }
                DrawColor(mPaint, Color.DKGRAY);
                mTestTimeTitleTv.setTextColor(Color.YELLOW);
                mTestTimeTv.setTextColor(Color.YELLOW);
                if (mainThreadExitFlag) {
                    return;
                }
                LcdDelay();
                if (mainThreadExitFlag) {
                    return;
                }
                DrawColor(mPaint, Color.MAGENTA);
                mTestTimeTitleTv.setTextColor(Color.BLACK);
                mTestTimeTv.setTextColor(Color.BLACK);
                if (mainThreadExitFlag) {
                    return;
                }
                LcdDelay();
                if (mainThreadExitFlag) {
                    return;
                }
                mHandler.sendEmptyMessage(LOOP_MSG);
                //do something end
            } catch (Throwable t) {
            }
        }
    }
}
