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
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.io.File;

import java.io.FileReader;
import java.io.IOException;
//yuntian longyao add
//Description:耳机测试只播放音乐
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
//yuntian longyao end

/**
 * 耳机测试
 */
public class TypeCAnalogHeadsetTest extends BaseActivity {

    private TextView mTypeCAnalogHeadsetTestTipTv;
    private Button mRecordOrPlayBt;
    private MediaRecorder mRecorder;
    private MediaPlayer mPlayer;
    private State mState;

    private String mRecordFilePath;
    private boolean mHeadsetPlugged;
    private boolean mIsRecorded;
    private boolean mIsPlayed;
    private Handler mHandler = new Handler();

    enum State {
        IDLE, RECORDING, PLAYING
    }

    //yuntian longyao add
    //Description:耳机测试只播放音乐
    public static final int FLAG_SHOW_SILENT_HINT = 1 << 7;

    private AudioManager mAudioManager;

    private String mMediaName;
    private int mCurrentMusicVolume;
    private int mSafeMediaVolume;

    private void playMusic() {
        boolean enabledPassBt = true;
        try {
            if (mSafeMediaVolume != mCurrentMusicVolume) {
                mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, mSafeMediaVolume, FLAG_SHOW_SILENT_HINT);
            }
            AssetManager am = getAssets();
            AssetFileDescriptor afd = am.openFd(mMediaName);
            if (mPlayer != null) {
                mPlayer.reset();
            } else {
                mPlayer = new MediaPlayer();
            }
            mPlayer.setDataSource(afd.getFileDescriptor(),
                    afd.getStartOffset(), afd.getLength());
            mPlayer.setLooping(true);
            mPlayer.prepare();
            mPlayer.start();
        } catch (Exception e) {
            Log.e(this, "playMusic=>error: ", e);
            Toast.makeText(this, getString(R.string.speaker_test_play_fail, mMediaName), Toast.LENGTH_SHORT).show();
            enabledPassBt = false;
        }
        setPassButtonEnabled(enabledPassBt);
        setTestPass(enabledPassBt);
        if (isAutoTest()) {
            if (isTestPass()) {
                doOnAutoTest();
            }
        }
    }

    public void stopPlayMusic() {
        if (mPlayer.isPlaying()) {
            mPlayer.stop();
            mPlayer.reset();
            mPlayer.release();
        }
        if (mSafeMediaVolume != mCurrentMusicVolume) {
            mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, mCurrentMusicVolume, FLAG_SHOW_SILENT_HINT);
        }
    }
    //yuntian longyao end

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_typec_analog_headset_test);

        super.onCreate(savedInstanceState);
        //yuntian longyao add
        //Description:耳机测试只播放音乐
        mMediaName = getString(R.string.speaker_test_media_name);
        mPlayer = new MediaPlayer();
        mAudioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
        //yuntian longyao end

        mHeadsetPlugged = false;
        mState = State.IDLE;
        mRecordFilePath = Environment.getExternalStorageDirectory() + File.separator + "record.3gp";

        mTypeCAnalogHeadsetTestTipTv = (TextView) findViewById(R.id.headset_test_tip);
        mRecordOrPlayBt = (Button) findViewById(R.id.record_or_play);
        mRecordOrPlayBt.setOnClickListener(mRecordOrPlayClickListener);
        mCurrentMusicVolume = mAudioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
        mSafeMediaVolume = getResources().getInteger(com.android.internal.R.integer.config_safe_media_volume_index);
    }

    @Override
    protected void onResume() {
        super.onResume();
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_HEADSET_PLUG);
        registerReceiver(mHeadsetStateChangedReceiver, filter);
        if (mHeadsetPlugged) {
            mTypeCAnalogHeadsetTestTipTv.setText(R.string.headset_test_headset_plugged);
            mRecordOrPlayBt.setEnabled(true);
        } else {
            mTypeCAnalogHeadsetTestTipTv.setText(R.string.headset_test_headset_not_plugged);
            mRecordOrPlayBt.setEnabled(false);
        }
        if (mState == State.IDLE || mState == State.PLAYING) {
            mRecordOrPlayBt.setText(R.string.headset_test_start_recorder);
        } else {
            mRecordOrPlayBt.setText(R.string.headset_test_start_play);
        }
        mHandler.post(runnable);
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mHeadsetStateChangedReceiver);
        //yuntian longyao add
        //Description:耳机测试只播放音乐
        if (getResources().getBoolean(R.bool.headset_test_only_play_music)) {
            if (mPlayer != null) {
                stopPlayMusic();
            }
        }
        //yuntian longyao end
        stopPlay();
        stopRecord();
        mState = State.IDLE;
        File file = new File(mRecordFilePath);
        file.delete();
        mHandler.removeCallbacks(runnable);
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

    private Runnable runnable = new Runnable() {
        @Override
        public void run() {
            if (getHeadsetState() == 3) {
                mTypeCAnalogHeadsetTestTipTv.setText(R.string.headset_test_headset_plugged);
                //yuntian longyao add
                //Description:耳机测试只播放音乐
                if (!mHeadsetPlugged && getResources().getBoolean(R.bool.headset_test_only_play_music)) {
                    playMusic();
                }
                //yuntian longyao end
                mHeadsetPlugged = true;
            } else {
                mHeadsetPlugged = false;
                mTypeCAnalogHeadsetTestTipTv.setText(R.string.headset_test_headset_not_plugged);
            }
            mHandler.postDelayed(runnable, 1000);
        }
    };

    private BroadcastReceiver mHeadsetStateChangedReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            int state = intent.getIntExtra("state", 0);
            Log.d(TypeCAnalogHeadsetTest.this, "onReceive=>state: " + state);
            if (state == 1 && getHeadsetState() == 3) {
                mHeadsetPlugged = true;
                mTypeCAnalogHeadsetTestTipTv.setText(R.string.headset_test_headset_plugged);
                mRecordOrPlayBt.setEnabled(true);
                //yuntian longyao add
                //Description:耳机测试只播放音乐
                if (getResources().getBoolean(R.bool.headset_test_only_play_music)) {
                    playMusic();
                }
                //yuntian longyao end
            } else {
                mHeadsetPlugged = false;
                mTypeCAnalogHeadsetTestTipTv.setText(R.string.headset_test_headset_not_plugged);
                mRecordOrPlayBt.setEnabled(false);
                if (getResources().getBoolean(R.bool.headset_test_only_play_music)) {
                    if (mPlayer != null) {
                        stopPlayMusic();
                    }
                }
                stopPlay();
                stopRecord();
                mState = State.IDLE;
                File file = new File(mRecordFilePath);
                file.delete();
            }
        }
    };

    private static final String HEADSET_STATE_PATH = "/sys/bus/platform/drivers/Accdet_Driver/headset_type";
    private int getHeadsetState() {
        char[] result = new char[1];
        File file = new File(HEADSET_STATE_PATH);
        FileReader fr = null;
        try {
            fr = new FileReader(file);
            fr.read(result);
        } catch (IOException e) {
            Log.e("yoyo", "getHeadsetState=>error: ", e);
        } finally {
            try {
                if (fr != null) {
                    fr.close();
                }
            } catch (Exception e) {
            }
        }
        int state;
        state = Integer.parseInt(String.valueOf(result));
        Log.e("yoyo", "result: " + state);
        return state;
    }

}
