package com.android.agingtest.test;

import java.io.File;
import java.io.IOException;

import android.content.Context;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.util.Log;
import android.widget.Toast;

import com.android.agingtest.BaseActivity;


public class LoudSpeakerAcitity extends BaseActivity {


    private File file;

    MediaPlayer mPlayer = null;
    private AudioManager audioManager;

    private String FileName = "/system/media/Test.mp3";

    @Override
    public void initValues() {
        // TODO Auto-generated method stub
        super.initValues();
        audioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        //Speaker默认最大声
        int maxVolume = audioManager.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
        Log.e("lsz", "maxVolume-->" + maxVolume);
        audioManager.setStreamVolume(AudioManager.STREAM_MUSIC, maxVolume - 8, AudioManager.FLAG_PLAY_SOUND);
        audioManager.setSpeakerphoneOn(true);
        audioManager.setMode(AudioManager.MODE_NORMAL);
        file = new File(FileName);
        mPlayer = new MediaPlayer();
    }

    @Override
    public void startTest() {
        // TODO Auto-generated method stub
        super.startTest();
        try {
            if (!file.exists() || file.length() == 0) {
                Toast.makeText(LoudSpeakerAcitity.this, "Can not find the test file!", Toast.LENGTH_SHORT).show();
                return;
            }
            mPlayer.setDataSource(file.getAbsolutePath());
            mPlayer.prepare();
            mPlayer.setLooping(true);
        } catch (IllegalArgumentException e) {
            mPlayer = null;
            e.printStackTrace();
        } catch (IllegalStateException e) {
            mPlayer = null;
            e.printStackTrace();
        } catch (IOException e) {
            mPlayer = null;
            e.printStackTrace();
        }

        if (mPlayer != null && !mPlayer.isPlaying()) {
            mPlayer.start();
        }
    }

    @Override
    public void stopTest(boolean isOk) {
        // TODO Auto-generated method stub
        super.stopTest(isOk);
        if (mPlayer != null) {
            mPlayer.stop();
            mPlayer.release();
            mPlayer = null;
        }
        audioManager.setSpeakerphoneOn(false);
    }

}
