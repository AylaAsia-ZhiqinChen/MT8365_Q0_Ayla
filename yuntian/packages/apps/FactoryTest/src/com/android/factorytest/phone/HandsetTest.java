package com.android.factorytest.phone;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

/**
 * 听筒测试
 */
public class HandsetTest extends BaseActivity {

    public static final int FLAG_SHOW_SILENT_HINT = 1 << 7;

    private MediaPlayer mPlayer;
    private AudioManager mAudioManager;

    private String mMediaName;
    private int mCurrentVoiceCallVolume;
    private int mMaxVoiceCallVolume;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_handset_test);

        super.onCreate(savedInstanceState);

        mMediaName = getString(R.string.speaker_test_media_name);
        mPlayer = new MediaPlayer();
        mAudioManager = (AudioManager) getSystemService(AUDIO_SERVICE);
        mMaxVoiceCallVolume = mAudioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
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
            mCurrentVoiceCallVolume = mAudioManager.getStreamVolume(AudioManager.STREAM_VOICE_CALL);
            mAudioManager.setStreamVolume(AudioManager.STREAM_VOICE_CALL, mMaxVoiceCallVolume, FLAG_SHOW_SILENT_HINT);
            //add by yt_wxc for avoid playing music by speaker begin
            mAudioManager.setMode(AudioManager.MODE_IN_CALL);
            mAudioManager.setSpeakerphoneOn(false);
            //add by yt_wxc for avoid playing music by speaker end
            AssetManager am = getAssets();
            AssetFileDescriptor afd = am.openFd(mMediaName);
            mPlayer.reset();
            mPlayer.setDataSource(afd.getFileDescriptor(),
                    afd.getStartOffset(), afd.getLength());
            mPlayer.setAudioStreamType(AudioManager.STREAM_VOICE_CALL);
            mPlayer.setLooping(true);
            mPlayer.prepare();
            mPlayer.start();
        } catch (Exception e) {
            Log.e(this, "playMusic=>error: ", e);
            Toast.makeText(this, getString(R.string.speaker_test_play_fail, mMediaName), Toast.LENGTH_SHORT).show();
            enabledPassBt = false;
        }
        setPassButtonEnabled(enabledPassBt);
    }

    public void stopPlayMusic() {
        if (mPlayer.isPlaying()) {
            try {
                mPlayer.stop();
                mPlayer.reset();
                mPlayer.release();
            } catch (Exception e) {
                Log.d(this, "stopPlayMusic=>error: ", e);
            }
        }
        int current = mAudioManager.getStreamVolume(AudioManager.STREAM_VOICE_CALL);
        if (current == mMaxVoiceCallVolume) {
            mAudioManager.setStreamVolume(AudioManager.STREAM_VOICE_CALL, mCurrentVoiceCallVolume, FLAG_SHOW_SILENT_HINT);
        }
        //add by yt_wxc for avoid playing music by speaker begin
        if(mAudioManager != null){
			mAudioManager.setMode(AudioManager.MODE_NORMAL);
			}
		//add by yt_wxc for avoid playing music by speaker end
    }
}
