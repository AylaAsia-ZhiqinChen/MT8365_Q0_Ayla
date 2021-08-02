package com.android.agingtest;

import java.io.IOException;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnBufferingUpdateListener;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class Player implements OnBufferingUpdateListener, OnCompletionListener, MediaPlayer.OnPreparedListener,
        SurfaceHolder.Callback, OnErrorListener {
    private int videoWidth;
    private int videoHeight;
    public MediaPlayer mediaPlayer;
    private SurfaceHolder surfaceHolder;
    private PlayerListener mListener;

    public Player(SurfaceView surfaceView) {
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this);
        surfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        mediaPlayer = new MediaPlayer();
    }

    public void play() {
        if (mediaPlayer != null) {
            mediaPlayer.start();
        } else {
            try {
                mediaPlayer = new MediaPlayer();
                mediaPlayer.setDisplay(surfaceHolder);
                mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
                mediaPlayer.setOnBufferingUpdateListener(this);
                mediaPlayer.setOnErrorListener(this);
                mediaPlayer.setOnPreparedListener(this);
            } catch (Exception e) {
                Log.e(this, "pay=>error: ", e);
                if (mListener != null) {
                    mListener.onError();
                }
            }
            Log.e(this, "play()..");
        }
    }

    public void playUrl(Context context) {
        try {
            AssetFileDescriptor afd = context.getAssets().openFd("moveTest.mp4");
            mediaPlayer.reset();
            mediaPlayer.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
            mediaPlayer.setLooping(true);
            mediaPlayer.setScreenOnWhilePlaying(true);
            mediaPlayer.prepare();
            // mediaPlayer.start();
        } catch (IllegalArgumentException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IllegalStateException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    public void pause() {
        if (mediaPlayer != null && mediaPlayer.isPlaying()) {
            mediaPlayer.pause();
        }
    }

    public void stop() {
        if (mediaPlayer != null) {
            mediaPlayer.stop();
            mediaPlayer.release();
            mediaPlayer = null;
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder arg0, int arg1, int arg2, int arg3) {
        Log.e(this, "surface changed");
    }

    @Override
    public void surfaceCreated(SurfaceHolder arg0) {
        try {
            mediaPlayer.setDisplay(surfaceHolder);
            mediaPlayer.setAudioStreamType(AudioManager.STREAM_MUSIC);
            mediaPlayer.setOnBufferingUpdateListener(this);
            mediaPlayer.setOnPreparedListener(this);
        } catch (Exception e) {
            Log.e(this, "error", e);
        }
        Log.e(this, "surface created");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder arg0) {
        Log.e(this, "surface destroyed");
    }

    @Override
    /**
     * 通过onPrepared播放
     */
    public void onPrepared(MediaPlayer player) {
        videoWidth = mediaPlayer.getVideoWidth();
        videoHeight = mediaPlayer.getVideoHeight();
        if (videoHeight != 0 && videoWidth != 0) {
            player.start();
        }
        Log.i(this, "onPrepared");
    }

    @Override
    public void onCompletion(MediaPlayer arg0) {
        // TODO Auto-generated method stub

    }

    @Override
    public void onBufferingUpdate(MediaPlayer mp, int percent) {
        // TODO Auto-generated method stub

    }

    @Override
    public boolean onError(MediaPlayer mp, int what, int extra) {
        if (mListener != null) {
            mListener.onError();
        }
        return false;
    }

    public void setListener(PlayerListener listener) {
        mListener = listener;
    }

    public interface PlayerListener {
        void onError();
    }

}
