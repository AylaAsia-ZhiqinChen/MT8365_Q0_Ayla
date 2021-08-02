package com.mediatek.bluetooth;

import java.util.List;

import android.bluetooth.BluetoothA2dp;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothAvrcp;
import android.bluetooth.BluetoothAvrcpController;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.RemoteException;
import android.os.SystemClock;
import android.os.SystemProperties;
import android.support.annotation.NonNull;
import android.support.v4.app.Fragment;
import android.media.browse.MediaBrowser;
import android.media.MediaDescription;
import android.media.MediaMetadata;
import android.media.session.MediaController;
import android.media.session.MediaSession;
import android.media.session.MediaSessionManager;
import android.media.session.PlaybackState;
import android.view.MotionEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.ViewConfiguration;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import com.mediatek.bluetooth.common.CachedBluetoothDeviceManager;
import com.mediatek.bluetooth.common.LocalBluetoothManager;
import com.mediatek.bluetooth.common.LocalBluetoothProfileManager;
import android.bluetooth.BluetoothA2dpSink;
//import com.mediatek.bluetooth.BluetoothProfileManager;
//import com.mediatek.bluetooth.BluetoothProfileManager.Profile;
import android.text.format.DateUtils;
import com.mediatek.bluetooth.R;
import com.mediatek.bluetooth.util.Utils;
import android.media.AudioManager;
import java.util.Locale;
import java.util.Formatter;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.content.res.Resources;

import android.util.Log;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

public class MusicFragment extends Fragment implements View.OnClickListener{

    private static final String TAG = "MusicFragment";
    private boolean VDBG = true;
    private Context mContext;
    private Handler mHandler;

    public MusicFragment() {
    }

    private boolean mIsA2dpAvrcpConnected = true;

    int mA2dpState = BluetoothProfile.STATE_DISCONNECTED;
    int mAvrcpState = BluetoothProfile.STATE_DISCONNECTED;

    private TextView a2dpsinkStateInfo;
    private TextView avrcpctStateInfo;
    private TextView mSongTitle;
    private TextView mSongArtist;
    private TextView mSongAbums;
    private TextView mTimeCurrent;
    private TextView mTimeAll;
    private long mDuration;
    Resources res;
    private String mSongTitleData = "unknown";
    private String mSongArtistData = "unknown";
    private String mSongAbumsData = "unknown";
    String mCurrenttimeString;

    ImageButton mStopButton;
    ImageButton mPlayPauseButton;
    ImageButton mPrevButton;
    ImageButton mNextButton;
    ImageButton mFastbackButton;
    ImageButton mFastspeedButton;
    MediaMetadata mMediaMetadata;
    private SeekBar mSeekBar;
    private StringBuilder sFormatBuilder = new StringBuilder();
    private Formatter sFormatter = new Formatter(sFormatBuilder, Locale.getDefault());
    private final Object[] sTimeArgs = new Object[5];

    Object obj = new Object();
    private int mTouchSlop ;
    private final int A2DP_PLAYING = BluetoothA2dpSink.STATE_PLAYING;
    private final int A2DP_NOT_PLAYING = BluetoothA2dpSink.STATE_NOT_PLAYING;
    private final int A2DP_PAUSE = 12;

    private int mPlayingState = A2DP_NOT_PLAYING;
    private int lastPlayingState = A2DP_NOT_PLAYING;
    private final String PROPERTY_KEY = "bluetooth.a2dp.playstatus";

    private BluetoothAvrcpController mBtControllerService;
    private BluetoothA2dpSink mService;

    private BluetoothDevice mConnectedDevice = null;
    private LocalBluetoothProfileManager mProfileManager = null;
    private boolean mIsAVRCPControllerConnected = false;
    private boolean mIsA2DPSinkConnected = false;
    private AudioManager mAudio;

    private static final String ACTION_TRACK_EVENT =
        "android.bluetooth.avrcp-controller.profile.action.TRACK_EVENT";
    private static final String EXTRA_METADATA =
            "android.bluetooth.avrcp-controller.profile.extra.METADATA";

    private static final String EXTRA_PLAYBACK =
            "android.bluetooth.avrcp-controller.profile.extra.PLAYBACK";

    private MediaBrowser mBrowser = null;
    private MediaController mMediaController = null;
    private MediaController.Callback mMediaCtrlCallback = null;
    private MediaSessionManager mSessionManager;
    private MediaSessionManager.OnActiveSessionsChangedListener mSessionListener;
    private ScheduledFuture<?> mScheduleFuture;
    private PlaybackState mLastPlaybackState;
    private boolean isUpdateProgress = false;

    private static final long PROGRESS_UPDATE_INTERNAL = 1000;
    private static final long PROGRESS_UPDATE_INITIAL_INTERVAL = 100;

    private static final String BROWSER_SERVICE_PKG = "com.android.bluetooth.a2dpsink.mbs";
    private static final String BROWSER_SERVICE_CLASS = "com.android.bluetooth.a2dpsink.mbs.A2dpMediaBrowserService";
    private static final String A2DP_MBS_TAG = "BluetoothMediaBrowserService";

    private final ScheduledExecutorService mExecutorService =
        Executors.newSingleThreadScheduledExecutor();

    private final Runnable mUpdateProgressTask = new Runnable() {
        @Override
        public void run() {
            isUpdateProgress = true;
            updateProgress();
        }
    };    

    private final BluetoothProfile.ServiceListener mAvrcpServiceListener = 
                    new BluetoothProfile.ServiceListener() {
        
        public void onServiceConnected(int profile, BluetoothProfile proxy) {
            Log.d(TAG, "Bluetooth service connected");
            mBtControllerService = (BluetoothAvrcpController) proxy;
            Log.d(TAG, "====get BluetoothAvrcpController====");
            mBtControllerService.getConnectedDevices();
            //getCurrentPlayingStatusAndUpdate();
            Log.d(TAG,"connected devices size is :" + mBtControllerService.getConnectedDevices().size());
            if (mBtControllerService.getConnectedDevices().size() > 0) {
                mConnectedDevice = mBtControllerService.getConnectedDevices().get(0);
                mIsAVRCPControllerConnected = true;
                Log.d(TAG,"mConnectedDevice == " + mConnectedDevice);
                updateState(BluetoothProfile.AVRCP_CONTROLLER,
                    BluetoothProfile.STATE_CONNECTED);
                setButtonEnabled(mIsAVRCPControllerConnected && mIsA2DPSinkConnected);
            }
        }

        public void onServiceDisconnected(int profile) {
            Log.d(TAG, "Bluetooth service disconnected");
            mIsAVRCPControllerConnected = false;
            mBtControllerService = null;
            updateState(BluetoothProfile.AVRCP_CONTROLLER,
                BluetoothProfile.STATE_DISCONNECTED);
            resetUIInfo();
        }
    };

    private final BluetoothProfile.ServiceListener A2dpServiceListener =
            new BluetoothProfile.ServiceListener() {

        public void onServiceConnected(int profile, BluetoothProfile proxy) {
            Log.d(TAG, "A2dpServiceListener,proxy is : " + proxy.toString());
            mService = (BluetoothA2dpSink) proxy;
            Log.d(TAG, "A2dpServiceListener,device size : " + mService.getConnectedDevices().size());
            if(mService.getConnectedDevices().size()>0){
                mConnectedDevice = mService.getConnectedDevices().get(0);
                mIsA2DPSinkConnected = true;
                updateState(BluetoothProfile.A2DP_SINK,
                    BluetoothProfile.STATE_CONNECTED);
                setButtonEnabled(mIsAVRCPControllerConnected && mIsA2DPSinkConnected);
            }

        }

        public void onServiceDisconnected(int profile) {
            Log.d(TAG, "A2dp disconnected profile is : " + profile);
            updateState(BluetoothProfile.A2DP_SINK,
                    BluetoothProfile.STATE_DISCONNECTED);
            mConnectedDevice = null;
            mIsA2DPSinkConnected = false;
            resetUIInfo();
        }
    };
    private void resetUIInfo(){
        setButtonEnabled(mIsAVRCPControllerConnected && mIsA2DPSinkConnected);
        updateSongsInfoUI(true);
        stopSeekbarUpdate();
    }

    /**
     * Callback on <code>MediaBrowser.connect()</code>
     * This is relevant only on the Carkitt side, since the intent is to connect a MediaBrowser
     * to the A2dpMediaBrowser Service that is run by the Car's Bluetooth Audio App.
     * On successful connection, we obtain the handle to the corresponding MediaController,
     * so we can imitate sending media commands via the Bluetooth Audio App.
     */
    MediaBrowser.ConnectionCallback mBrowserConnectionCallback =
            new MediaBrowser.ConnectionCallback() {

                @Override
                public void onConnected() {
                    Log.d(TAG, " onConnected: session token " + mBrowser.getSessionToken());
                    MediaController mediaController = new MediaController(mContext,
                            mBrowser.getSessionToken());
                    // Update the MediaController
                    setCurrentMediaController(mediaController);
                }

                @Override
                public void onConnectionFailed() {
                    Log.d(TAG," onConnectionFailed");
                }
            };


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.d(TAG,"++++onCreate+++");
        mContext = this.getActivity();

        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        if (adapter != null) {
            adapter.getProfileProxy(getActivity(), A2dpServiceListener,
                    BluetoothProfile.A2DP_SINK);
            adapter.getProfileProxy(getActivity(), mAvrcpServiceListener,
                    BluetoothProfile.AVRCP_CONTROLLER);

        }
        mTouchSlop = ViewConfiguration.get(this.getActivity()).getScaledTouchSlop();

        res = getResources();

        mMediaCtrlCallback = new MediaControllerCallback();
        mHandler = new Handler(Looper.getMainLooper());
        mSessionManager =
            (MediaSessionManager) mContext.getSystemService(mContext.MEDIA_SESSION_SERVICE);
        mSessionListener = new SessionChangeListener();
        // Listen on Active MediaSession changes, so we can get the active session's MediaController
        if (mSessionManager != null) {
            mSessionManager.addOnActiveSessionsChangedListener(mSessionListener, null,
                    mHandler);
            List<MediaController> controllers = mSessionManager.getActiveSessions(null);
            Log.d(TAG," Num Sessions " + controllers.size());
            for (int i = 0; i < controllers.size(); i++) {
                Log.d(TAG,"Active session : " + i + ((MediaController) (controllers.get(
                        i))).getPackageName() + ((MediaController) (controllers.get(i))).getTag());
            }
            for (int i = 0; i < controllers.size(); i++) {
                MediaController controller = (MediaController) controllers.get(i);
                if ((controller.getTag().contains(A2DP_MBS_TAG))) {
                    setCurrentMediaController(controller);
                }
            }
        }

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                    Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.bt_music, container, false);
        Log.d(TAG,"++++onCreateView+++");

        initViews(view);
        initFilter();
        initState();
        return view;
    }

    @Override
    public void onStart() {
        super.onStart();
        Log.d(TAG, "onStart");
    }

    @Override
    public void onResume(){
        Log.d(TAG, "onResume, mService = " + mService);
        Log.d(TAG, "onResume, mConnectedDevice = " + mConnectedDevice);
        if (mService != null && mConnectedDevice != null){
            Log.d(TAG, "onResume, " + mService.isA2dpPlaying(mConnectedDevice));
        }
        super.onResume();
    }

    @Override
    public void onStop() {
        super.onStop();
        Log.d(TAG, "onStop");
    }

    @Override
    public void onDestroy() {
        Log.d(TAG,"onDestroy");
        try{
           if(mPlaystateReceiver != null) {
               this.getActivity().unregisterReceiver(mPlaystateReceiver);
            }
        }catch(IllegalArgumentException e){
           Log.e("TAG","IllegalArgumentException");
        }
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        if (adapter != null) {
            adapter.closeProfileProxy(BluetoothProfile.AVRCP_CONTROLLER,
                (BluetoothProfile)mBtControllerService);
            adapter.closeProfileProxy(BluetoothProfile.A2DP_SINK,
                (BluetoothProfile)mService);
        }
        setCurrentMediaController(null);
        stopSeekbarUpdate();
        mExecutorService.shutdown();
        super.onDestroy();
    }

    private void initViews(View mView) {
        mStopButton = (ImageButton) mView.findViewById(R.id.btn_music_stop);
        mPlayPauseButton = (ImageButton) mView.findViewById(R.id.btn_music_play_pause);
        mPrevButton = (ImageButton) mView.findViewById(R.id.btn_music_prev);
        mNextButton = (ImageButton) mView.findViewById(R.id.btn_music_next);
        mFastbackButton = (ImageButton) mView.findViewById(R.id.btn_music_fastback);
        mFastspeedButton = (ImageButton) mView.findViewById(R.id.btn_music_fastspeed);
        a2dpsinkStateInfo = (TextView) mView.findViewById(R.id.tv_A2DP_status);
        avrcpctStateInfo = (TextView) mView.findViewById(R.id.tv_AVRCP_status);
        mSongTitle = (TextView) mView.findViewById(R.id.song_title);
        mSongTitle.setSelected(true);
        mSongArtist = (TextView) mView.findViewById(R.id.artistname);
        mSongArtist.setSelected(true);
        mSongAbums = (TextView) mView.findViewById(R.id.albumname);
        mSongAbums.setSelected(true);
        mTimeCurrent= (TextView) mView.findViewById(R.id.time_current);
        mTimeCurrent.setText(mCurrenttimeString);
        mTimeAll= (TextView) mView.findViewById(R.id.time_all);
        mSeekBar = (SeekBar) mView.findViewById(R.id.SeekBar);

        mSeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                mTimeCurrent.setText(DateUtils.formatElapsedTime(progress / 1000));
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                stopSeekbarUpdate();
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                if (mMediaController != null){
                    mMediaController.getTransportControls().seekTo(seekBar.getProgress());
                }
                scheduleSeekbarUpdate();
            }
        });
        updateSongsInfoUI(false);

        mStopButton.setOnClickListener(this);
        mPlayPauseButton.setOnClickListener(this);
        mPrevButton.setOnClickListener(this);
        mNextButton.setOnClickListener(this);

        //mFastspeedButton.setOnTouchListener(mTouchListener);
        mFastspeedButton.setOnClickListener(this);
        //mFastbackButton.setOnTouchListener(mTouchListener);
        mFastbackButton.setOnClickListener(this);
        setButtonEnabled(mIsAVRCPControllerConnected && mIsA2DPSinkConnected);
    }

    private void setButtonEnabled(boolean enabled){
        Log.d(TAG, "setButtonEnabled, enabled = " + enabled);
        mStopButton.setEnabled(enabled);
        mPlayPauseButton.setEnabled(enabled);
        mPrevButton.setEnabled(enabled);
        mNextButton.setEnabled(enabled);
        mFastspeedButton.setEnabled(enabled);
        mFastbackButton.setEnabled(enabled);
    }
    private void initState() {

        Log.d(TAG, "initState mStopButton = " + mStopButton);

        if (mA2dpState == BluetoothProfile.STATE_CONNECTED) {
            a2dpsinkStateInfo.setText(R.string.a2dpsink_status_connected_info);
        } else {
            a2dpsinkStateInfo.setText(R.string.a2dpsink_status_notconnected_info);
        }
        if(mAvrcpState == BluetoothProfile.STATE_CONNECTED){
           avrcpctStateInfo.setText(R.string.a2dpsink_status_connected_info);
        }else{
           avrcpctStateInfo.setText(R.string.a2dpsink_status_notconnected_info);
        }
    }


    private void initFilter() {
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothA2dpSink.ACTION_PLAYING_STATE_CHANGED);
        intentFilter.addAction(BluetoothA2dpSink.ACTION_CONNECTION_STATE_CHANGED);
        intentFilter.addAction(BluetoothAvrcpController.ACTION_CONNECTION_STATE_CHANGED);
        intentFilter.addAction(ACTION_TRACK_EVENT);
        this.getActivity().registerReceiver(mPlaystateReceiver, intentFilter);
    }

    public void updateState(int profile, int state) {
        Log.d(TAG,"updateState profile = " + profile + "  state = " + state);
        if (BluetoothProfile.A2DP_SINK == profile) {
            mA2dpState = state;
            switch (state) {
            case BluetoothProfile.STATE_CONNECTED:
                a2dpsinkStateInfo.setText(R.string.a2dpsink_status_connected_info);
                break;
            case BluetoothProfile.STATE_DISCONNECTED:
                a2dpsinkStateInfo.setText(R.string.a2dpsink_status_notconnected_info);
                //mPlayPauseButton.setImageResource(R.drawable.bt_music_pause_play);
                break;
            default:
                break;
            }
        } else if (BluetoothProfile.AVRCP_CONTROLLER == profile) {
            mAvrcpState = state;
            switch (state) {
            case BluetoothProfile.STATE_CONNECTED:
                avrcpctStateInfo.setText(R.string.a2dpsink_status_connected_info);
                break;
            case BluetoothProfile.STATE_DISCONNECTED:
                 avrcpctStateInfo.setText(R.string.a2dpsink_status_notconnected_info);
                 //mPlayPauseButton.setImageResource(R.drawable.bt_music_pause_play);
                break;
            default:
                break;
            }
        }
    }

    @Override
    public void onClick(View v) {
        if (mMediaController == null){
            Log.d(TAG,"onClick, device is null or avrcp diconnected");
            return ;
        }
        PlaybackState state = mMediaController.getPlaybackState();
        MediaController.TransportControls controls = mMediaController.getTransportControls();
        if(v == mPrevButton){
            Log.d(TAG,"onClick, skipToPrevious");
            controls.skipToPrevious();
        } else if (v == mNextButton){
            Log.d(TAG,"onClick, skipToNext");
            controls.skipToNext();
        } else if (v == mPlayPauseButton){
            Log.d(TAG,"onClick, play or pause");
            if (state != null) {
                Log.d(TAG,"onClick, Play or Pause state = " + state.getState());
                switch (state.getState()) {
                    case PlaybackState.STATE_PLAYING: // fall through
                    case PlaybackState.STATE_BUFFERING:
                        controls.pause();
                        break;
                    case PlaybackState.STATE_PAUSED:
                    case PlaybackState.STATE_STOPPED:
                        controls.play();
                        break;
                    default:
                        break;
                }
            }
        } else if (v == mStopButton){
            Log.d(TAG,"onClick, stop");
            controls.stop();
            stopSeekbarUpdate();
            mSeekBar.setProgress(0);
            mTimeCurrent.setText(DateUtils.formatElapsedTime(0));
        } else if (v == mFastbackButton){
            Log.d(TAG,"onClick, rewinding");
            controls.rewind();
        } else if (v == mFastspeedButton){
            Log.d(TAG,"onClick, fast forwarding");
            controls.fastForward();
        }
    }

    OnTouchListener mTouchListener = new OnTouchListener(){
        int mLastX = -1;
        int movie_times = -1;

        public boolean onTouch(View v, MotionEvent event){
            if(mMediaController == null){
                Log.d(TAG,"device is null or avrcp diconnected");
                return false;
            }
            PlaybackState state = mMediaController.getPlaybackState();
            MediaController.TransportControls controls = mMediaController.getTransportControls();

            Log.d(TAG,"event.getAction() == " + event.getAction());

            return false;
        }

    };

    //update the play/pause button ui
    private BroadcastReceiver mPlaystateReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.d(TAG,"action-->" + action);
            if(action.equals(ACTION_TRACK_EVENT)){
                //mMediaMetadata = (MediaMetadata)intent.getExtra(EXTRA_METADATA, null);
                //Log.d(TAG,"mMediaMetadata = " + mMediaMetadata);

            }else if(action.equals(BluetoothA2dpSink.ACTION_PLAYING_STATE_CHANGED)){
                int state = intent.getIntExtra(BluetoothProfile.EXTRA_STATE,
                    BluetoothA2dpSink.STATE_NOT_PLAYING);
                int prevState = intent.getIntExtra(BluetoothProfile.EXTRA_PREVIOUS_STATE,
                    BluetoothA2dpSink.STATE_NOT_PLAYING);
                Log.d(TAG, "ACTION_PLAYING_STATE_CHANGED, state = " + state);
                Log.d(TAG, "ACTION_PLAYING_STATE_CHANGED, prevState = " + prevState);

            }else if(action.equals(BluetoothA2dpSink.ACTION_CONNECTION_STATE_CHANGED)){
                int state = intent.getIntExtra(BluetoothProfile.EXTRA_STATE,
                        BluetoothA2dpSink.STATE_DISCONNECTED);
                BluetoothDevice device = (BluetoothDevice)intent.getParcelableExtra(
                    BluetoothDevice.EXTRA_DEVICE);
                Log.d(TAG, "A2dpSink ACTION_CONNECTION_STATE_CHANGED, state = " + state);
                Log.d(TAG, "A2dpSink ACTION_CONNECTION_STATE_CHANGED, device = " + device);
                if (state == BluetoothA2dpSink.STATE_CONNECTED){
                    mIsA2DPSinkConnected = true;
                    mConnectedDevice = device;
                    updateState(
                        BluetoothProfile.A2DP_SINK,
                        BluetoothProfile.STATE_CONNECTED);
                }
                if (state == BluetoothA2dpSink.STATE_DISCONNECTED){
                    mIsA2DPSinkConnected = false;
                    updateState(
                        BluetoothProfile.A2DP_SINK,
                        BluetoothProfile.STATE_DISCONNECTED);
                }
                setButtonEnabled(mIsAVRCPControllerConnected && mIsA2DPSinkConnected);
            }else if (action.equals(
                BluetoothAvrcpController.ACTION_CONNECTION_STATE_CHANGED)){
                BluetoothDevice device = (BluetoothDevice)intent.getParcelableExtra(
                    BluetoothDevice.EXTRA_DEVICE);
                int status = intent.getIntExtra(BluetoothProfile.EXTRA_STATE,
                        BluetoothProfile.STATE_CONNECTING);
                Log.d(TAG, "Avrcp action device connect status is " +
                    status + " device = " + device);
                if (BluetoothProfile.STATE_CONNECTED == status) {
                    mIsAVRCPControllerConnected = true;
                    mConnectedDevice = device;
                    updateState(
                        BluetoothProfile.AVRCP_CONTROLLER,
                        BluetoothProfile.STATE_CONNECTED);
                }else{
                    mIsAVRCPControllerConnected = false;
                    updateState(
                        BluetoothProfile.AVRCP_CONTROLLER,
                        BluetoothProfile.STATE_DISCONNECTED);
                    updateSongsInfoUI(true);
                }
                setButtonEnabled(mIsAVRCPControllerConnected && mIsA2DPSinkConnected);
            }

        }
    };

    private void updateSongsInfo() {
        mSongTitleData = displayMetaDataTitle(mMediaMetadata);
        mSongArtistData = displayMetaDataArtist(mMediaMetadata);
        mSongAbumsData = displayMetaDataAlbum(mMediaMetadata);
        mDuration = displayMetaDataTime(mMediaMetadata);

    }

    private void updateSongsInfoUI(boolean mStop) {
        if((mMediaMetadata == null && mSongTitleData == null) || mStop){
            Log.d(TAG,"mMediaMetadata is null");
            mSongTitle.setText(res.getString(R.string.unknow));
            mSongArtist.setText(res.getString(R.string.unknow));
            mSongAbums.setText(res.getString(R.string.unknow));
            String resetTime = makeTimeString(0 / 1000);
            mTimeAll.setText(resetTime);
            mTimeCurrent.setText(resetTime);
            mSeekBar.setProgress(0);
        }else{
            mSongTitle.setText(mSongTitleData);
            mSongArtist.setText(mSongArtistData);
            mSongAbums.setText(mSongAbumsData);
            mSeekBar.setMax((int)mDuration);
            String time = makeTimeString(mDuration / 1000);
            mTimeAll.setText(time);
        }
    }
    private String displayMetaData(MediaMetadata mMetaData) {
        StringBuffer sb = new StringBuffer();
        if(mMetaData == null){
            Log.d(TAG,"mMetaData is null");
            return null;
        }

       if(mMetaData.containsKey(MediaMetadata.METADATA_KEY_TITLE)){
            sb.append(mMetaData.getString(MediaMetadata.METADATA_KEY_TITLE) + "@");
            Log.d(TAG,"METADATA_KEY_TITLE is not null = " + sb.toString());
        }
        if(mMetaData.containsKey(MediaMetadata.METADATA_KEY_ARTIST)){
            sb.append(mMetaData.getString(MediaMetadata.METADATA_KEY_ARTIST) + " @+");
            Log.d(TAG,"METADATA_KEY_ARTIST is not null = " + sb.toString());
        }
        if(mMetaData.containsKey(MediaMetadata.METADATA_KEY_ALBUM))
            sb.append(mMetaData.getString(MediaMetadata.METADATA_KEY_ALBUM) + " @@");        
        if(mMetaData.containsKey(MediaMetadata.METADATA_KEY_GENRE))
            sb.append(mMetaData.getString(MediaMetadata.METADATA_KEY_GENRE) + " @@@");
        if(mMetaData.containsKey(MediaMetadata.METADATA_KEY_DURATION))
            sb.append(Long.toString(mMetaData.getLong(MediaMetadata.METADATA_KEY_DURATION)) + " @@@@");
        if(mMetaData.containsKey(MediaMetadata.METADATA_KEY_TRACK_NUMBER))
            sb.append(Long.toString(mMetaData.getLong(MediaMetadata.METADATA_KEY_TRACK_NUMBER)) + " @@@@@");
        return sb.toString();
    }


    private String displayMetaDataAlbum(MediaMetadata mMetaData) {
        StringBuffer sb = new StringBuffer();
        if(mMetaData == null){
            Log.d(TAG,"mMetaData is null");
            return null;
        }

        if(mMetaData.containsKey(MediaMetadata.METADATA_KEY_TITLE)){
            sb.append(mMetaData.getString(MediaMetadata.METADATA_KEY_ALBUM) + " ");
            Log.d(TAG,"METADATA_KEY_TITLE is not null = " + sb.toString());
        }
        return sb.toString();
    }

    private String displayMetaDataArtist(MediaMetadata mMetaData) {
        StringBuffer sb = new StringBuffer();
        if(mMetaData == null){
            Log.d(TAG,"mMetaData is null");
            return null;
        }

        if(mMetaData.containsKey(MediaMetadata.METADATA_KEY_ARTIST)){
            sb.append(mMetaData.getString(MediaMetadata.METADATA_KEY_ARTIST));
            Log.d(TAG,"METADATA_KEY_TITLE is not null = " + sb.toString());
        }
        return sb.toString();
    }

    private String displayMetaDataTitle(MediaMetadata mMetaData) {
        StringBuffer sb = new StringBuffer();
        if(mMetaData == null){
            Log.d(TAG,"mMetaData is null");
            return null;
        }

        if(mMetaData.containsKey(MediaMetadata.METADATA_KEY_TITLE)){
            sb.append(mMetaData.getString(MediaMetadata.METADATA_KEY_TITLE));
            Log.d(TAG,"METADATA_KEY_TITLE is not null = " + sb.toString());
        }
        return sb.toString();
    }

    private int displayMetaDataTime(MediaMetadata mMetaData) {
        StringBuffer sb = new StringBuffer();
        if(mMetaData == null){
            Log.d(TAG,"mMetaData is null");
            return 0;
        }

        if(mMetaData.containsKey(MediaMetadata.METADATA_KEY_DURATION))
            sb.append(Long.toString(mMetaData.getLong(MediaMetadata.METADATA_KEY_DURATION)));
        if ("".equals(sb.toString())){
            return 0;
        }
        return Integer.valueOf(sb.toString());
    }

    private String makeTimeString(long secs) {
        String durationformat = "";
        if (isAdded()){
            durationformat = this.getString(
                secs < 3600 ? R.string.durationformatshort : R.string.durationformatlong);
        }

        /* Provide multiple arguments so the format can be changed easily
         * by modifying the xml.
         */
        sFormatBuilder.setLength(0);

        final Object[] timeArgs = sTimeArgs;
        timeArgs[0] = secs / 3600;
        timeArgs[1] = secs / 60;
        timeArgs[2] = (secs / 60) % 60;
        timeArgs[3] = secs;
        timeArgs[4] = secs % 60;
        /// M: use local format
        return sFormatter.format(Locale.getDefault(), durationformat, timeArgs).toString();
    }

    public void bluetoothMediaConnectToCarMBS() {
        ComponentName compName;
        // Create a MediaBrowser to connect to the A2dpMBS
        if (mBrowser == null) {
            compName = new ComponentName(BROWSER_SERVICE_PKG, BROWSER_SERVICE_CLASS);
            // Note - MediaBrowser connect needs to be done on the Main Thread's handler,
            // otherwise we never get the ServiceConnected callback.
            Runnable createAndConnectMediaBrowser = new Runnable() {
                @Override
                public void run() {
                    mBrowser = new MediaBrowser(mContext, compName, mBrowserConnectionCallback,
                            null);
                    if (mBrowser != null) {
                        Log.d(TAG, " Connecting to MBS");
                        mBrowser.connect();
                    } else {
                        Log.d(TAG, " Failed to create a MediaBrowser");
                    }
                }
            };

            Handler mainHandler = new Handler(mContext.getMainLooper());
            mainHandler.post(createAndConnectMediaBrowser);
        } //mBrowser
    }

    /**
     * Update the Current MediaController.
     * As has been commented above, we need the MediaController handles to the
     * BluetoothSL4AAudioSrcMBS on Phone and A2dpMediaBrowserService on Car to send and receive
     * media commands.
     *
     * @param controller - Controller to update with
     */
    private void setCurrentMediaController(MediaController controller) {
        Handler mainHandler = new Handler(mContext.getMainLooper());
        if (mMediaController == null && controller != null) {
            Log.d(TAG," Setting MediaController " + controller.getTag());
            mMediaController = controller;
            mMediaController.registerCallback(mMediaCtrlCallback);
        } else if (mMediaController != null && controller != null) {
            // We have a new MediaController that we have to update to.
            if (controller.getSessionToken().equals(mMediaController.getSessionToken())
                    == false) {
                Log.d(TAG," Changing MediaController " + controller.getTag());
                mMediaController.unregisterCallback(mMediaCtrlCallback);
                mMediaController = controller;
                mMediaController.registerCallback(mMediaCtrlCallback, mainHandler);
            }
        } else if (mMediaController != null && controller == null) {
            // Clearing the current MediaController
            Log.d(TAG," Clearing MediaController " + mMediaController.getTag());
            mMediaController.unregisterCallback(mMediaCtrlCallback);
            mMediaController = controller;
        }
    } 

    private void updatePlaybackState(PlaybackState state) {
        Log.d(TAG, "updatePlaybackState, state = " + state.getState());
        switch(state.getState()) {
            case PlaybackState.STATE_PLAYING:
                if (!isUpdateProgress){
                    scheduleSeekbarUpdate();
                }
                mPlayPauseButton.setImageResource(R.drawable.bt_music_play_pause);
                break;
            case PlaybackState.STATE_PAUSED:
            case PlaybackState.STATE_NONE:
            case PlaybackState.STATE_STOPPED:
                stopSeekbarUpdate();
                mPlayPauseButton.setImageResource(R.drawable.bt_music_pause_play);
                break;
            case PlaybackState.STATE_BUFFERING:
            default:
                break;
        }

    }

    /**
     * When the MediaController for the required MediaSession is obtained, register for its
     * callbacks.
     * Not used yet, but this can be used to verify state changes in both ends.
     */
    private class MediaControllerCallback extends MediaController.Callback {
        @Override
        public void onPlaybackStateChanged(PlaybackState state) {
            Log.d(TAG," onPlaybackStateChanged...");
            if (state == null) {
                return;
            }
            Log.d(TAG," onPlaybackStateChanged... mIsAVRCPControllerConnected = "
                + mIsAVRCPControllerConnected);
            if (mMediaMetadata == null){
                mMediaMetadata = mMediaController.getMetadata();
                if (mMediaMetadata != null){
                    Log.d(TAG,"onPlaybackStateChanged,art--> = "
                        + displayMetaData(mMediaMetadata));
                    updateSongsInfo();
                    updateSongsInfoUI(false);

                }
            }
            mLastPlaybackState = state;
            Log.d(TAG, "onPlaybackStateChanged, current time = " +
                DateUtils.formatElapsedTime(state.getPosition() / 1000));
            updatePlaybackState(state);
        }

        @Override
        public void onMetadataChanged(MediaMetadata metadata) {
            Log.d(TAG," onMetadataChanged ");
            mMediaMetadata = metadata;
            if(mMediaMetadata != null){
                Log.d(TAG,"art--> = " + displayMetaData(mMediaMetadata));
                updateSongsInfo();
                updateSongsInfoUI(false);
            }
        }
    }

    /**
     * The listener that was setup for listening to changes to Active Media Sessions.
     * This listener is useful in both Car and Phone sides.
     */
    private class SessionChangeListener
            implements MediaSessionManager.OnActiveSessionsChangedListener {
        /**
         * On the Phone side, it listens to the BluetoothSL4AAudioSrcMBS (that the SL4A app runs)
         * becoming active.
         * On the Car side, it listens to the A2dpMediaBrowserService (associated with the
         * Bluetooth Audio App) becoming active.
         * The idea is to get a handle to the MediaController appropriate for the device, so
         * that we can send and receive Media commands.
         */
        @Override
        public void onActiveSessionsChanged(List<MediaController> controllers) {
            if (VDBG) {
                Log.d(TAG," onActiveSessionsChanged : " + controllers.size());
                for (int i = 0; i < controllers.size(); i++) {
                    Log.d(TAG,"Active session : " + i + ((MediaController) (controllers.get(
                            i))).getPackageName() + ((MediaController) (controllers.get(
                            i))).getTag());
                }
            }
            // As explained above, looking for the BluetoothSL4AAudioSrcMBS (when running on Phone)
            // or A2dpMediaBrowserService (when running on Carkitt).
            for (int i = 0; i < controllers.size(); i++) {
                MediaController controller = (MediaController) controllers.get(i);
                if ((controller.getTag().contains(A2DP_MBS_TAG))) {
                    setCurrentMediaController(controller);
                    return;
                }
            }
        }
    }
    private void stopSeekbarUpdate() {
        Log.d(TAG, "stopSeekbarUpdate...");
        isUpdateProgress = false;
        if (mScheduleFuture != null) {
            mScheduleFuture.cancel(false);
        }
    }
    private void scheduleSeekbarUpdate() {
        Log.d(TAG, "scheduleSeekbarUpdate...");
        stopSeekbarUpdate();
        if (!mExecutorService.isShutdown()) {
            mScheduleFuture = mExecutorService.scheduleAtFixedRate(
                    new Runnable() {
                        @Override
                        public void run() {
                            mHandler.post(mUpdateProgressTask);
                        }
                    }, PROGRESS_UPDATE_INITIAL_INTERVAL,
                    PROGRESS_UPDATE_INTERNAL, TimeUnit.MILLISECONDS);
        }
    }

    private void updateProgress() {
        if (mLastPlaybackState == null) {
            return;
        }
        long currentPosition = mLastPlaybackState.getPosition();
        if (mLastPlaybackState.getState() == PlaybackState.STATE_PLAYING) {
            // Calculate the elapsed time between the last position update and now and unless
            // paused, we can assume (delta * speed) + current position is approximately the
            // latest position. This ensure that we do not repeatedly call the getPlaybackState()
            // on MediaControllerCompat.
            long timeDelta = SystemClock.elapsedRealtime() -
                    mLastPlaybackState.getLastPositionUpdateTime();
            currentPosition += (int) timeDelta * mLastPlaybackState.getPlaybackSpeed();
        }
        mSeekBar.setProgress((int) currentPosition);
    }
}
