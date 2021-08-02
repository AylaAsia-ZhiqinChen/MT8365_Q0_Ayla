package com.android.agingtest.test;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.TimeZone;

import com.android.agingtest.Log;
import com.android.agingtest.TestUtils;
import com.android.agingtest.Player;
import com.android.agingtest.Player.PlayerListener;
import com.android.agingtest.R;
import com.android.agingtest.ReportActivity;

import android.app.ActionBar;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.preference.PreferenceManager;
import android.view.KeyEvent;
import android.view.MenuItem;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.os.RemoteException;
import com.mediatek.nvram.NvRAMUtils;

public class PlayVideoActivity extends Activity implements OnClickListener, PlayerListener {

    public static final int FLAG_HOMEKEY_DISPATCHED = 0x80000000;
    private static final String VIDEO_PATH = Environment.getExternalStorageDirectory() + "/moveTest.mp4";

    private static final int MSG_UPDATE_TIME = 0;
    private static final int MSG_PLAY_VIDEO = 1;

    private SurfaceView mSurfaceView;
    private TextView mTestTimeTv;
    private Button mStopBt;
    private SharedPreferences mSharedPreferences;
    private SurfaceView surfaceView;
    private Button btnPause, btnPlayUrl, btnStop;
    private Player mPlayer;
    private PowerManager mPowerManager;
    private WakeLock mLock;

    private long mPlayVideoTime;
    private long mStartTime;

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(this, "onCreate()...");
        getWindow().addFlags(FLAG_HOMEKEY_DISPATCHED);
        getWindow().addFlags(View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        getWindow().addFlags(android.view.WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD);
        initActionBar();
        setContentView(R.layout.activity_play_video);

        initValues();
        initViews();
    }

    private void startTest() {
        mStartTime = System.currentTimeMillis();
        mHandler.sendEmptyMessage(MSG_UPDATE_TIME);
        mHandler.sendEmptyMessageDelayed(MSG_PLAY_VIDEO, 1000);
    }

    private void stopTest(boolean isOk) {
        mHandler.removeMessages(MSG_UPDATE_TIME);
        mHandler.removeMessages(MSG_PLAY_VIDEO);
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        mPlayer.stop();
        Editor e = mSharedPreferences.edit();
        e.putInt(TestUtils.PLAY_VIDEO_KEY + TestUtils.TEST_RESULT, isOk ? 1 : 0);
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
                    NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_VIDEO, buff);
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
        boolean b = System.currentTimeMillis() - mStartTime >= mPlayVideoTime;
        Log.e("playvideo", "isTesOver->" + b);
        return b;
    }


    @Override
    protected void onResume() {
        super.onResume();
        Log.d(this, "onResume()...");
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

    private void initActionBar() {
        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
    }

    private int[] alltestindex = null;
    private int key_index = -1;
    String key;

    private void initValues() {

        key_index = getIntent().getIntExtra(TestUtils.KEY_INDEX, -1);
        alltestindex = getIntent().getIntArrayExtra(TestUtils.ALLTESTKEYINDEX);
        Log.e("lsz", "key_index->" + key_index + ",alltestindex[key_index]->" + TestUtils.ALLKEYS[alltestindex[key_index]]);
        key = TestUtils.PLAY_VIDEO_KEY;
        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        mPowerManager = (PowerManager) getSystemService(Context.POWER_SERVICE);
        mLock = mPowerManager.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "play_video_test");
        mLock.acquire();


        mPlayVideoTime = mSharedPreferences.getInt(TestUtils.PLAY_VIDEO_KEY + TestUtils.TEST_TIME,
                getResources().getInteger(R.integer.default_play_video_time)) * TestUtils.MILLSECOND;
        if (getResources().getBoolean(R.bool.use_total_test_time)) {
            mPlayVideoTime = mSharedPreferences.getInt("singleTestTime",
                    getResources().getInteger(R.integer.default_play_video_time)) * TestUtils.MILLSECOND;
        }

        Log.d(this, "initValues=>startTime: " + mStartTime + " time: " + mPlayVideoTime);
    }

    private void initViews() {
        mTestTimeTv = (TextView) findViewById(R.id.test_time);
        mStopBt = (Button) findViewById(R.id.stop_test);
        mSurfaceView = (SurfaceView) findViewById(R.id.video_view);
        mPlayer = new Player(mSurfaceView);
        mPlayer.setListener(this);

        mTestTimeTv.setText(R.string.default_time_string);
        mStopBt.setOnClickListener(this);
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
                    Log.d("PlayVideoActivity", "handleMessage=>testTime: " + testTime);
                    if (isTestOver()) {
                        stopTest(true);
                    } else {
                        mHandler.sendEmptyMessageDelayed(MSG_UPDATE_TIME, 1000);
                    }
                    break;
                case MSG_PLAY_VIDEO:
                    mPlayer.playUrl(getApplicationContext());
                    break;
            }
        };
    };

    @Override
    public void onError() {
        mHandler.removeMessages(MSG_UPDATE_TIME);
        mHandler.removeMessages(MSG_PLAY_VIDEO);
        if (mPlayer != null) {
            mPlayer.stop();
        }
        stopTest(false);
    }

}
