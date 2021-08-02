package com.android.factorytest.annex;

import android.content.SharedPreferences;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.AudioManager;
import android.media.AudioSystem;
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
import java.io.IOException;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import android.os.Handler;
import android.os.Message;

public class DualMicrophoneTest extends BaseActivity {

    public static final int FLAG_SHOW_SILENT_HINT = 1 << 7;

    private TextView mMicrophoneStateTv;
    private Button mMainMicrophoneBt;
    private Button mSubMicrophoneBt;
    private Button mDualMicrophoneBt;
    private Button mPlayOrRecordBt;
    private MediaPlayer mPlayer;
    private MediaRecorder mRecorder;
    private AudioManager mAudioManager;

    private boolean mIsPlaying;
    private MicrophoneState mMicrophoneState;
    private String mFileName;

    enum MicrophoneState {
        DUAL, MAIN, SUB
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_dual_microphone_test);

        super.onCreate(savedInstanceState);

        mPlayer = new MediaPlayer();
        mIsPlaying = true;
        mMicrophoneState = MicrophoneState.DUAL;
        mAudioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
        mFileName = Environment.getExternalStorageDirectory() + File.separator + "audiorecordtest.3gp";

        mMicrophoneStateTv = (TextView) findViewById(R.id.dual_microphone_status);
        mMainMicrophoneBt = (Button) findViewById(R.id.main_microphone);
        mSubMicrophoneBt = (Button) findViewById(R.id.sub_microphone);
        mDualMicrophoneBt = (Button) findViewById(R.id.dual_microphone);
        mPlayOrRecordBt = (Button) findViewById(R.id.record_or_play);

        mMainMicrophoneBt.setOnClickListener(mTestClickListener);
        mSubMicrophoneBt.setOnClickListener(mTestClickListener);
        mDualMicrophoneBt.setOnClickListener(mTestClickListener);
        mPlayOrRecordBt.setOnClickListener(mTestClickListener);
        //yuntian longyao add
        //Description:工厂测试模式双麦克风测试选项改为竖着显示

        mDualMicrophoneBt.setBackgroundResource(R.drawable.ic_gray_bg);
        mMainMicrophoneBt.setBackgroundResource(R.drawable.ic_gray_bg);
        mSubMicrophoneBt.setBackgroundResource(R.drawable.ic_gray_bg);
        mPlayOrRecordBt.setBackgroundResource(R.drawable.ic_gray_bg);
        //yuntian longyao end

        setPassButtonEnabled(true);
    }

    @Override
    protected void onResume() {
        super.onResume();
        updateMicrophoneState();
    }

    @Override
    protected void onPause() {
        super.onPause();
        stopPlaying();
        stopRecording();
        mIsPlaying = false;
        mPlayOrRecordBt.setText(R.string.dual_microphone_record_title);
        AudioSystem.setParameters("SET_MIC_CHOOSE=0");
        mMicrophoneState = MicrophoneState.DUAL;
        File file = new File(mFileName);
        file.delete();
    }

    private void updateMicrophoneState() {
        switch (mMicrophoneState) {
            case DUAL:
                mMicrophoneStateTv.setText(getString(R.string.dual_microphone_test_state,
                        getString(R.string.dual_microphone_title)));
                break;

            case MAIN:
                mMicrophoneStateTv.setText(getString(R.string.dual_microphone_test_state,
                        getString(R.string.main_microphone_title)));
                break;

            case SUB:
                mMicrophoneStateTv.setText(getString(R.string.dual_microphone_test_state,
                        getString(R.string.sub_microphone_title)));
                break;
        }
    }

    private boolean startPlaying() {
		stopRecording();
        mPlayer = new MediaPlayer();
        try {
            File file = new File(mFileName);
            if (!file.exists() || file.length() == 0) {
                Toast.makeText(this, getString(R.string.record_file_not_exists), Toast.LENGTH_SHORT).show();
            } else {
				mPlayer.setDataSource(mFileName); 
				mPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
				mPlayer.prepare();
				mPlayer.start();
				return true;	
            }
        } catch (IOException e) {
            Log.e(this, "startPlaying:", e);
        }
        return false;
    }

    private void stopPlaying() {
        if (mPlayer != null) {
            mPlayer.release();
            mPlayer = null;
        }
    }

    private boolean startRecording() {
		stopPlaying();
        mRecorder = new MediaRecorder();
        try {
            mRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
            mRecorder.setOutputFormat(MediaRecorder.OutputFormat.THREE_GPP);
            mRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AMR_NB);
            mRecorder.setOutputFile(mFileName);

            mRecorder.prepare();
            mRecorder.start();
            return true;
        } catch (Exception e) {
            Log.e(this, "startRecording=>error: ", e);
            Toast.makeText(this, R.string.dual_microphone_test_record_fail, Toast.LENGTH_SHORT).show();
        }
        return false;
    }


    private void stopRecording() {
        if (mRecorder != null) {
            mRecorder.stop();
            mRecorder.release();
            mRecorder = null;
        }
    }

    private View.OnClickListener mTestClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            switch (v.getId()) {
                case R.id.dual_microphone:
                    mMicrophoneState = MicrophoneState.DUAL;
                    AudioSystem.setParameters("SET_MIC_CHOOSE=0");
                    updateMicrophoneState();
                    //yuntian longyao add
                    //Description:工厂测试模式双麦克风测试选项改为竖着显示
                    mDualMicrophoneBt.setBackgroundResource(R.drawable.ic_pass_bg);
                    mMainMicrophoneBt.setBackgroundResource(R.drawable.ic_gray_bg);
                    mSubMicrophoneBt.setBackgroundResource(R.drawable.ic_gray_bg);
                    //yuntian longyao end
                    break;

                case R.id.main_microphone:
                    mMicrophoneState = MicrophoneState.MAIN;
                    AudioSystem.setParameters("SET_MIC_CHOOSE=1");
                    updateMicrophoneState();
                    //yuntian longyao add
                    //Description:工厂测试模式双麦克风测试选项改为竖着显示
                    mMainMicrophoneBt.setBackgroundResource(R.drawable.ic_pass_bg);
                    mDualMicrophoneBt.setBackgroundResource(R.drawable.ic_gray_bg);
                    mSubMicrophoneBt.setBackgroundResource(R.drawable.ic_gray_bg);
                    //yuntian longyao end
                    break;

                case R.id.sub_microphone:
                    mMicrophoneState = MicrophoneState.SUB;
                    AudioSystem.setParameters("SET_MIC_CHOOSE=2");
                    //yuntian longyao add
                    //Description:工厂测试模式双麦克风测试选项改为竖着显示
                    mSubMicrophoneBt.setBackgroundResource(R.drawable.ic_pass_bg);
                    mDualMicrophoneBt.setBackgroundResource(R.drawable.ic_gray_bg);
                    mMainMicrophoneBt.setBackgroundResource(R.drawable.ic_gray_bg);
                    //yuntian longyao end
                    updateMicrophoneState();
                    break;

                case R.id.record_or_play:
                    if (mIsPlaying) {
                        if (startRecording()) {
                            mHandler.sendEmptyMessageDelayed(MSG_RECORDING, 100);
                            mPlayOrRecordBt.setEnabled (false);
                        }
                    } else {
                        if (startPlaying()) {
                            mHandler.sendEmptyMessageDelayed(MSG_PLAYING, 100);
                            mPlayOrRecordBt.setEnabled (false);
                        }
                    }
                    break;
            }
        }
    };

    private final static int MSG_RECORDING = 0;
    private final static int MSG_PLAYING = 1;
    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_RECORDING:
                    mPlayOrRecordBt.setText(R.string.dual_microphone_play_title);
                    mIsPlaying = false;
                    //yuntian longyao add
                    //Description:工厂测试模式双麦克风测试选项改为竖着显示
                    mPlayOrRecordBt.setBackgroundResource(R.drawable.ic_pass_bg);
                    //yuntian longyao end
                    mPlayOrRecordBt.setEnabled (true);
                    break;
                case MSG_PLAYING:
                    mPlayOrRecordBt.setText(R.string.dual_microphone_record_title);
                    mIsPlaying = true;
                    //yuntian longyao add
                    //Description:工厂测试模式双麦克风测试选项改为竖着显示
                    mPlayOrRecordBt.setBackgroundResource(R.drawable.ic_gray_bg);
                    //yuntian longyao add
                    mPlayOrRecordBt.setEnabled (true);
                    break;
            }
        }
    };
}
