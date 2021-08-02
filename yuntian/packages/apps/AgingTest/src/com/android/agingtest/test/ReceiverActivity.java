package com.android.agingtest.test;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.TimeZone;

import com.android.agingtest.BaseActivity;
import com.android.agingtest.Log;
import com.android.agingtest.R;
import com.android.agingtest.ReportActivity;

import android.app.ActionBar;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.media.AudioManager;
import android.media.SoundPool;
import android.media.SoundPool.OnLoadCompleteListener;
import android.os.Bundle;
import android.os.Handler;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.preference.PreferenceManager;
import android.view.KeyEvent;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class ReceiverActivity extends BaseActivity {

    private SoundPool mSoundPool;
    private int mSoundId;
    AudioManager am;

    @Override
    public void stopTest(boolean isOk) {
        // TODO Auto-generated method stub
        super.stopTest(isOk);
        am.setMode(AudioManager.MODE_NORMAL);
        if (mSoundPool != null) {
            mSoundPool.stop(mSoundId);
            mSoundPool = null;
        }
    }

    @Override
    public void doTest() {
        // TODO Auto-generated method stub
        super.doTest();

        am = (AudioManager) getSystemService("audio");
        am.setStreamVolume(AudioManager.STREAM_VOICE_CALL, am.getStreamMaxVolume(AudioManager.STREAM_VOICE_CALL), 0);
        am.setSpeakerphoneOn(false);//关闭扬声器
        //把声音设定成Earpiece（听筒）出来，设定为正在通话中
        am.setMode(AudioManager.MODE_IN_CALL);
        mSoundPool = new SoundPool(2, AudioManager.STREAM_VOICE_CALL, 100);
        mSoundId = mSoundPool.load(this, R.raw.pizzicato, 1);
        mSoundPool.setVolume(AudioManager.STREAM_VOICE_CALL, 1.0f, 1.0f);

        mSoundPool.setOnLoadCompleteListener(new OnLoadCompleteListener() {
            @Override
            public void onLoadComplete(SoundPool soundPool, int sampleId, int status) {
                if (mSoundPool != null) {
                    mSoundPool.play(mSoundId, 1.0f, 1.0f, 1, -1, 1.0f);
                }
            }
        });
    }

}
