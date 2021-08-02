package com.android.factorytest.phone;

import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.io.File;

/**
 * 话筒回音测试
 */
public class MicrophoneEchoTest extends BaseActivity {

    private Button mRecordOrPlayBt;
    private MediaPlayer mPlayer;
    private MediaRecorder mRecorder;
    private State mState;

    private String mRecordFilePath;
    private boolean mIsRecorded;
    private boolean mIsPlayed;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_micro_phone_echo_test);

        super.onCreate(savedInstanceState);

        mState = State.NORMAL;
        mIsRecorded = false;
        mIsPlayed = false;
        mRecordFilePath = Environment.getExternalStorageDirectory() + File.separator + "record.3gp";
        mRecordOrPlayBt = (Button) findViewById(R.id.record_or_play);
        mRecordOrPlayBt.setOnClickListener(mRecordOrPlayButtonClickListener);
    }

    @Override
    protected void onResume() {
        super.onResume();
        mRecordOrPlayBt.setText(R.string.microphone_echo_test_start_record_title);
        if (mIsRecorded && mIsPlayed) {
            setPassButtonEnabled(true);
//            setTestCompleted(true);
//            setTestPass(true);
//            if (isAutoTest()) {
//                mRecordOrPlayBt.setEnabled(false);
//                if (isTestPass()) {
//                    Toast.makeText(this, getString(R.string.auto_test_pass_tip, getAutoTestNextTestDelayedTime() / 1000), Toast.LENGTH_SHORT).show();
//                } else {
//                    Toast.makeText(this, getString(R.string.auto_test_fail_tip, getAutoTestNextTestDelayedTime() / 1000), Toast.LENGTH_SHORT).show();
//                }
//                doOnAutoTest();
//            }
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mState == State.RECORD) {
            stopRecord();
        }
        if (mState == State.PLAYING) {
            stopPlay();
        }
        mState = State.NORMAL;
        File file = new File(mRecordFilePath);
        file.delete();
    }

    private void startRecord() {
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

    private void startPlay() {
        mPlayer = new MediaPlayer();
        try {
            mPlayer.setDataSource(mRecordFilePath);
            mPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            mPlayer.prepare();
            mPlayer.start();
            mIsPlayed = true;
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
        } catch (Exception e) {
            Log.e(this, "startPlayi=>error: ", e);
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

    private View.OnClickListener mRecordOrPlayButtonClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            switch (v.getId()) {
                case R.id.record_or_play:
                    if (mState == State.NORMAL || mState == State.PLAYING) {
                        stopPlay();
                        startRecord();
                        mState = State.RECORD;
                        mRecordOrPlayBt.setText(R.string.microphone_echo_test_play_record_title);
                    } else {
                        stopRecord();
                        startPlay();
                        mState = State.PLAYING;
                        mRecordOrPlayBt.setText(R.string.microphone_echo_test_start_record_title);
                    }
                    break;
            }
        }
    };

    enum State {
        NORMAL, RECORD, PLAYING
    }
}
