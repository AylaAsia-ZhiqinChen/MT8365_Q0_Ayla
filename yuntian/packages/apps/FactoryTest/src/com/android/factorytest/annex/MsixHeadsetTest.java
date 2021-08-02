package com.android.factorytest.annex;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.io.File;

/**
 * M6耳机测试
 */
public class MsixHeadsetTest extends BaseActivity {

    private TextView mHeadsetTestTipTv;
    private Button mRecordOrPlayBt;
    private MediaRecorder mRecorder;
    private MediaPlayer mPlayer;
    private State mState;

    private String mRecordFilePath;
    private boolean mHeadsetPlugged;
    private boolean mIsRecorded;
    private boolean mIsPlayed;

    enum State {
        IDLE, RECORDING, PLAYING
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_msix_headset_test);

        super.onCreate(savedInstanceState);

        mHeadsetPlugged = false;
        mState = State.IDLE;
        mRecordFilePath = Environment.getExternalStorageDirectory() + File.separator + "record.3gp";

        mHeadsetTestTipTv = (TextView) findViewById(R.id.msix_headset_test_tip);
        mRecordOrPlayBt = (Button) findViewById(R.id.msix_record_or_play);
        mRecordOrPlayBt.setOnClickListener(mRecordOrPlayClickListener);
    }

    @Override
    protected void onResume() {
        super.onResume();
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_HEADSET_PLUG);
        registerReceiver(mHeadsetStateChangedReceiver, filter);
        if (mHeadsetPlugged) {
            mHeadsetTestTipTv.setText(R.string.headset_test_headset_plugged);
            mRecordOrPlayBt.setEnabled(true);
        } else {
            mHeadsetTestTipTv.setText(R.string.headset_test_headset_not_plugged);
            mRecordOrPlayBt.setEnabled(false);
        }
        if (mState == State.IDLE || mState == State.PLAYING) {
            mRecordOrPlayBt.setText(R.string.headset_test_start_recorder);
        } else {
            mRecordOrPlayBt.setText(R.string.headset_test_start_play);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mHeadsetStateChangedReceiver);
        stopPlay();
        stopRecord();
        mState = State.IDLE;
        File file = new File(mRecordFilePath);
        file.delete();
    }

    private void record() {
        mRecorder = new MediaRecorder();
        try {
            mRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
            mRecorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
            mRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
            mRecorder.setOutputFile(mRecordFilePath);
            mRecorder.prepare();
            mRecorder.start();
            mIsRecorded = true;
        } catch (Exception e) {
            Log.e(this, "startRecord=>error: ", e);
        }
    }

    private void play() {
        mPlayer = new MediaPlayer();
        try {
            mPlayer.setDataSource(mRecordFilePath);
            mPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            mPlayer.prepare();
            mPlayer.start();
            mIsPlayed = true;
            if (!isTestCompleted()) {
                if (mIsRecorded) {
                    setPassButtonEnabled(true);
                    setTestCompleted(true);
                    setTestPass(true);
                    if (isAutoTest()) {
                        mRecordOrPlayBt.setEnabled(false);
                        if (isTestPass()) {
                            Toast.makeText(this, getString(R.string.auto_test_pass_tip, getAutoTestNextTestDelayedTime() / 1000), Toast.LENGTH_SHORT).show();
                        } else {
                            Toast.makeText(this, getString(R.string.auto_test_fail_tip, getAutoTestNextTestDelayedTime() / 1000), Toast.LENGTH_SHORT).show();
                        }
                        doOnAutoTest();
                    }
                }
            }
        } catch (Exception e) {
            Log.e(this, "startPlayi=>error: ", e);
        }
    }

    private void stopRecord() {
        if (mRecorder != null) {
            try {
                mRecorder.stop();
            } catch (Exception e) {
                Log.e(this, "stopRecord=>error: ", e);
            } finally {
                mRecorder = null;
            }
        }
    }

    private void stopPlay() {
        if (mPlayer != null) {
            try {
                if (mPlayer.isPlaying()) {
                    mPlayer.stop();
                }
                mPlayer.release();
            } catch (Exception e) {
                Log.e(this, "stopPlay=>error: ", e);
            } finally {
                mPlayer = null;
            }
        }
    }

    private View.OnClickListener mRecordOrPlayClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if (mState == State.IDLE || mState == State.PLAYING) {
                mState = State.RECORDING;
                mRecordOrPlayBt.setText(R.string.headset_test_start_play);
                stopPlay();
                record();
            } else {
                mState = State.PLAYING;
                mRecordOrPlayBt.setText(R.string.headset_test_start_recorder);
                stopRecord();
                play();
            }
        }
    };

    private BroadcastReceiver mHeadsetStateChangedReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            int state = intent.getIntExtra("state", 0);
            Log.d(MsixHeadsetTest.this, "onReceive=>state: " + state);
            if (state == 1) {
                mHeadsetPlugged = true;
                mHeadsetTestTipTv.setText(R.string.headset_test_headset_plugged);
                mRecordOrPlayBt.setEnabled(true);
            } else {
                mHeadsetPlugged = false;
                mHeadsetTestTipTv.setText(R.string.headset_test_headset_not_plugged);
                mRecordOrPlayBt.setEnabled(false);
            }
        }
    };
}
