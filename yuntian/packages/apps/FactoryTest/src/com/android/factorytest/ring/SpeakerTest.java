package com.android.factorytest.ring;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

/**
 * 扬声器测试
 */
public class SpeakerTest extends BaseActivity {

    public static final int FLAG_SHOW_SILENT_HINT = 1 << 7;

    private MediaPlayer mPlayer;
    private AudioManager mAudioManager;

    private String mMediaName;
    private int mCurrentMusicVolume;
    private int mMaxMusicVolume;
    private static final int MSG_SHOW_PASS = 0;

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(android.os.Message msg) {
            switch (msg.what) {
                case MSG_SHOW_PASS:
                    boolean pass = msg.getData().getBoolean("result");
                    setPassButtonEnabled(pass);
                    break;
            }
        };
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_speaker_test);

        super.onCreate(savedInstanceState);

        mMediaName = getString(R.string.speaker_test_media_name);
        mPlayer = new MediaPlayer();
        mAudioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
        mMaxMusicVolume = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
    }

    @Override
    protected void onResume() {
        super.onResume();
        playMusic();
    }

    @Override
    protected void onPause() {
        super.onPause();
        stopPlayMusic();
    }

    private void playMusic() {
        boolean enabledPassBt = true;
        try {
            mCurrentMusicVolume = mAudioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
            mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, mMaxMusicVolume, FLAG_SHOW_SILENT_HINT);
            AssetManager am = getAssets();
            AssetFileDescriptor afd = am.openFd(mMediaName);
            mPlayer = new MediaPlayer();
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
        Message msg = new Message();
        msg.what = MSG_SHOW_PASS;
        Bundle bundle = new Bundle();
        bundle.putBoolean("result", enabledPassBt);  //往Bundle中存放数据
        msg.setData(bundle);//mes利用Bundle传递数据
        mHandler.removeMessages(msg.what);
        mHandler.sendMessageDelayed(msg, 5000); //更新界面

        //	mHandler.sendMessageDelayed(mHandler.obtainMessage(MSG_SHOW_PASS), 5000);
        //   setPassButtonEnabled(enabledPassBt);
    }

    public void stopPlayMusic() {
        if (mPlayer.isPlaying()) {
            mPlayer.stop();
            mPlayer.reset();
            mPlayer.release();
        }
        mHandler.removeCallbacksAndMessages(null);
        int current = mAudioManager.getStreamVolume(AudioManager.STREAM_MUSIC);
        if (current == mMaxMusicVolume) {
            mAudioManager.setStreamVolume(AudioManager.STREAM_MUSIC, mCurrentMusicVolume, FLAG_SHOW_SILENT_HINT);
        }
    }
}
