package com.android.agingtest;

import java.util.ArrayList;

import android.app.ActionBar;
import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.Handler;
import android.os.RemoteException;
import android.preference.PreferenceManager;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

import com.mediatek.nvram.NvRAMUtils;

public class ReportActivity extends Activity implements OnClickListener {

    private static final int MSG_CIRCULATION = 0;

    private View mRebootContainer;
    private View mSleepContainer;
    private View mVibrateContainer;
    private View mReceiverContainer;
    private View mFrontTakingPictureContainer;
    private View mBackTakingPictureContainer;
    private View mPlayVideoContainer;
    private View mCameraMotorContainer;
    private View mFlashLightContainer;
    private View mLoudSpeakerContainer;
    private View mResetContainer;
    private View mScreenContainer;

    private TextView mRebootTv;
    private TextView mSleepTv;
    private TextView mVibrateTv;
    private TextView mReceiverTv;
    private TextView mFrontTakingPictureTv;
    private TextView mBackTakingPictureTv;
    private TextView mPlayVideoTv;
    private TextView mCameraMotor;
    private TextView mFlashLight;
    private TextView mloudSpeaker;
    private TextView mScreen;
    private TextView mReset;
    private Button mOk;
    private Button mCancel;

    private SharedPreferences mSharedPreferences;
    int isResetState = -1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD | WindowManager.LayoutParams.FLAG_SHOW_WHEN_LOCKED);
        Log.d(this, "onCreate()...");
        super.onCreate(savedInstanceState);
        byte currentResetTime = 0;

        try {
            //currentResetTime = NvRAMUtils.readNV(NvRAMUtils.INDEX_RESET_CURRENT_TIME);
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_RESET_CURRENT_TIME, NvRAMUtils.RESET_CURRENT_TIME_LENGTH);
            currentResetTime = buff[0];
        } catch (RemoteException e) {
            // TODO: handle exception
        }
        Log.i("lsz", "onCreate onCreate");
        Log.e("lsz", "currentResetTime->" + currentResetTime);
        //没有做过老化测试就返回0,测试成功返回-2,测试失败返回-3
        if (currentResetTime == 0) {
            isResetState = -1;
        } else if (currentResetTime == -2) {
            isResetState = 1;
        } else if (currentResetTime == -3) {
            isResetState = 0;
        }

        Log.e("lsz", "isResetSuccess->" + isResetState);
        initActionBar();

        setContentView(R.layout.activity_report);

        initValues();
        initViews();
        mSharedPreferences.edit().remove(TestUtils.IS_TEST).commit();
    }

    @Override
    public boolean onNavigateUp() {
        // TODO Auto-generated method stub
        Intent intent = new Intent();
        intent = new Intent(this, AgingTestMainActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
        startActivity(intent);
        return super.onNavigateUp();
    }

    @Override
    protected void onStart() {
        Log.d(this, "onStart()...");
        updateViewsVisible();
        super.onStart();
    }

    @Override
    protected void onResume() {
        updateUI();
        Log.i("lsz", "updateUI updateUI");
        //getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
//		Intent intent = getIntent();
//		boolean enabledCirculation = intent.getBooleanExtra(TestUtils.CIRCULATION_EXTRA, false);
//		String className = intent.getStringExtra(TestUtils.CLASS_NAME_EXTRA);
//		boolean isCirculation = PreferenceManager.getDefaultSharedPreferences(this)
//				.getBoolean(TestUtils.CIRCULATION_STATE, getResources().getBoolean(R.bool.default_circulation_value));
//		Log.d(this, "onResume=>enabled: " + enabledCirculation + " className: " + className + " circulation: " + isCirculation);
//		if (isCirculation && enabledCirculation) {
//			if (!mHandler.hasMessages(MSG_CIRCULATION)) {
//				mHandler.sendEmptyMessageDelayed(MSG_CIRCULATION, getResources().getInteger(R.integer.circulation_delayed));
//			}
//		} else {
//			Editor e = mSharedPreferences.edit();
//			e.putBoolean(TestUtils.TEST_STATE, true);
//			e.commit();
//		}
        super.onResume();
    }

    @Override
    protected void onPause() {
        Log.d(this, "onPause()...");
        //getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        super.onPause();
//		mHandler.removeMessages(MSG_CIRCULATION);
    }


    @Override
    public void onClick(View v) {
        Intent intent = null;
        switch (v.getId()) {
            case R.id.ok:
                mHandler.removeMessages(MSG_CIRCULATION);
                intent = new Intent(this, AgingTestMainActivity.class);
                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
                startActivity(intent);
                break;

            case R.id.cancel:
                mHandler.removeMessages(MSG_CIRCULATION);
                intent = new Intent(this, AgingTestMainActivity.class);
                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
                startActivity(intent);
                break;
        }
//		Editor e = mSharedPreferences.edit();
//		e.putBoolean(TestUtils.TEST_STATE, true);
//		e.commit();
    }

    private void initActionBar() {
        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
    }

    private void initValues() {
        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
    }

    private void initViews() {
        mRebootContainer = findViewById(R.id.reboot_container);
        mSleepContainer = findViewById(R.id.sleep_container);
        mVibrateContainer = findViewById(R.id.vibrate_container);
        mReceiverContainer = findViewById(R.id.receiver_container);
        mFrontTakingPictureContainer = findViewById(R.id.front_taking_picture_container);
        mBackTakingPictureContainer = findViewById(R.id.back_taking_picture_container);
        mPlayVideoContainer = findViewById(R.id.play_video_container);
        mCameraMotorContainer = findViewById(R.id.camera_motor_container);
        mFlashLightContainer = findViewById(R.id.flash_light_container);
        mLoudSpeakerContainer = findViewById(R.id.loudspeaker_container);
        mResetContainer = findViewById(R.id.reset_container);
        mScreenContainer = findViewById(R.id.screen_container);

        mRebootTv = (TextView) findViewById(R.id.reboot_report);
        mSleepTv = (TextView) findViewById(R.id.sleep_report);
        mVibrateTv = (TextView) findViewById(R.id.vibrate_report);
        mReceiverTv = (TextView) findViewById(R.id.receiver_report);
        mFrontTakingPictureTv = (TextView) findViewById(R.id.front_taking_picture_report);
        mBackTakingPictureTv = (TextView) findViewById(R.id.back_taking_picture_report);
        mPlayVideoTv = (TextView) findViewById(R.id.play_video_report);
        mCameraMotor = (TextView) findViewById(R.id.camera_motor_report);
        mFlashLight = (TextView) findViewById(R.id.flash_light_report);
        mloudSpeaker = (TextView) findViewById(R.id.loud_speaker_report);
        mReset = (TextView) findViewById(R.id.reset_report);
        mScreen = (TextView) findViewById(R.id.screen_report);

        mOk = (Button) findViewById(R.id.ok);
        mCancel = (Button) findViewById(R.id.cancel);

        mOk.setOnClickListener(this);
//		mCancel.setOnClickListener(this);
    }

    private void updateViewsVisible() {
        Resources res = getResources();

        if (!res.getBoolean(R.bool.reboot_visible)) {
            mRebootContainer.setVisibility(View.GONE);
        }

        if (!res.getBoolean(R.bool.sleep_visible)) {
            mSleepContainer.setVisibility(View.GONE);
        }

        if (!res.getBoolean(R.bool.vibrate_visible)) {
            mVibrateContainer.setVisibility(View.GONE);
        }

        if (!res.getBoolean(R.bool.receiver_visible)) {
            mReceiverContainer.setVisibility(View.GONE);
        }

        if (!res.getBoolean(R.bool.front_taking_picture_visible)) {
            mFrontTakingPictureContainer.setVisibility(View.GONE);
        }

        if (!res.getBoolean(R.bool.back_taking_picture_visible)) {
            mBackTakingPictureContainer.setVisibility(View.GONE);
        }

        if (!res.getBoolean(R.bool.play_video_visible)) {
            mPlayVideoContainer.setVisibility(View.GONE);
        }

        if (!res.getBoolean(R.bool.camera_motor_visible)) {
            mCameraMotorContainer.setVisibility(View.GONE);
        }

        if (!res.getBoolean(R.bool.flash_light_visible)) {
            mFlashLightContainer.setVisibility(View.GONE);
        }

        if (!res.getBoolean(R.bool.loudspeaker_visible)) {
            mLoudSpeakerContainer.setVisibility(View.GONE);
        }

        if (!res.getBoolean(R.bool.reset_visible)) {
            mResetContainer.setVisibility(View.GONE);
        }

        if (!res.getBoolean(R.bool.screen_visible)) {
            mScreenContainer.setVisibility(View.GONE);
        }

    }

    private String result = TestUtils.TEST_RESULT;

    private void updateUI() {
        int state = mSharedPreferences.getInt(TestUtils.REBOOT_KEY + result, -1);
        byte reboot = 0;
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_AGINGTEST_REBOOT, NvRAMUtils.INDEX_AGINGTEST_REBOOT_LENGTH);
            reboot = buff[0];
        } catch (RemoteException e) {

        }
        Log.e("lsz", "reboot->" + reboot);
        if (reboot == 0) {
            state = -1;
        } else if (reboot == -2) {
            state = 1;
        } else if (reboot == -3) {
            state = 0;
        }
        mRebootTv.setText(getStateText(state));
        mRebootTv.setTextColor(getStateColor(state));


        state = mSharedPreferences.getInt(TestUtils.SLEEP_KEY + result, -1);
        byte sleep = 0;
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_AGINGTEST_SLEEP, NvRAMUtils.INDEX_AGINGTEST_SLEEP_LENGTH);
            sleep = buff[0];
        } catch (RemoteException e) {

        }
        Log.e("lsz", "sleep->" + sleep);
        if (sleep == 0) {
            state = -1;
        } else if (sleep == -2) {
            state = 1;
        } else if (sleep == -3) {
            state = 0;
        }
        mSleepTv.setText(getStateText(state));
        mSleepTv.setTextColor(getStateColor(state));

        state = mSharedPreferences.getInt(TestUtils.VIBRATE_KEY + result, -1);
        byte vibrate = 0;
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_AGINGTEST_VIBRATION, NvRAMUtils.INDEX_AGINGTEST_VIBRATION_LENGTH);
            vibrate = buff[0];
        } catch (RemoteException e) {

        }
        Log.e("lsz", "vibrate->" + vibrate);
        if (vibrate == 0) {
            state = -1;
        } else if (vibrate == -2) {
            state = 1;
        } else if (vibrate == -3) {
            state = 0;
        }
        mVibrateTv.setText(getStateText(state));
        mVibrateTv.setTextColor(getStateColor(state));

        state = mSharedPreferences.getInt(TestUtils.RECEIVER_KEY + result, -1);
        byte receiver = 0;
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_AGINGTEST_RECEIVER, NvRAMUtils.INDEX_AGINGTEST_RECEIVER_LENGTH);
            receiver = buff[0];
        } catch (RemoteException e) {

        }
        Log.e("lsz", "receiver->" + receiver);
        if (receiver == 0) {
            state = -1;
        } else if (receiver == -2) {
            state = 1;
        } else if (receiver == -3) {
            state = 0;
        }
        mReceiverTv.setText(getStateText(state));
        mReceiverTv.setTextColor(getStateColor(state));


        state = mSharedPreferences.getInt(TestUtils.FRONT_CAM_KEY + result, -1);
        byte frontCamera = 0;
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_AGINGTEST_FRONT_CAMERA, NvRAMUtils.INDEX_AGINGTEST_FRONT_CAMERA_LENGTH);
            frontCamera = buff[0];
        } catch (RemoteException e) {

        }
        Log.e("lsz", "frontCamera->" + frontCamera);
        if (frontCamera == 0) {
            state = -1;
        } else if (frontCamera == -2) {
            state = 1;
        } else if (frontCamera == -3) {
            state = 0;
        }

        mFrontTakingPictureTv.setText(getStateText(state));
        mFrontTakingPictureTv.setTextColor(getStateColor(state));

        state = mSharedPreferences.getInt(TestUtils.BACK_CAM_KEY + result, -1);
        byte backCamera = 0;
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_AGINGTEST_BACK_CAMERA, NvRAMUtils.INDEX_AGINGTEST_BACK_CAMERA_LENGTH);
            backCamera = buff[0];
        } catch (RemoteException e) {

        }
        Log.e("lsz", "backCamera->" + backCamera);
        if (backCamera == 0) {
            state = -1;
        } else if (backCamera == -2) {
            state = 1;
        } else if (backCamera == -3) {
            state = 0;
        }

        mBackTakingPictureTv.setText(getStateText(state));
        mBackTakingPictureTv.setTextColor(getStateColor(state));

        state = mSharedPreferences.getInt(TestUtils.PLAY_VIDEO_KEY + result, -1);
        byte video = 0;
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_AGINGTEST_VIDEO, NvRAMUtils.INDEX_AGINGTEST_VIDEO_LENGTH);
            video = buff[0];
        } catch (RemoteException e) {

        }
        Log.e("lsz", "video->" + video);
        if (video == 0) {
            state = -1;
        } else if (video == -2) {
            state = 1;
        } else if (video == -3) {
            state = 0;
        }


        mPlayVideoTv.setText(getStateText(state));
        mPlayVideoTv.setTextColor(getStateColor(state));

        state = mSharedPreferences.getInt(TestUtils.MOTORZOOM_KEY + result, -1);

        byte motor = 0;
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_AGINGTEST_MOTOR, NvRAMUtils.INDEX_AGINGTEST_MOTOR_LENGTH);
            motor = buff[0];
        } catch (RemoteException e) {

        }
        Log.e("lsz", "motor->" + motor);
        if (motor == 0) {
            state = -1;
        } else if (motor == -2) {
            state = 1;
        } else if (motor == -3) {
            state = 0;
        }

        mCameraMotor.setText(getStateText(state));
        mCameraMotor.setTextColor(getStateColor(state));

        state = mSharedPreferences.getInt(TestUtils.FLASH_KEY + result, -1);

        byte flash = 0;
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_AGINGTEST_FLASH, NvRAMUtils.INDEX_AGINGTEST_FLASH_LENGTH);
            flash = buff[0];
        } catch (RemoteException e) {

        }
        Log.e("lsz", "flash->" + flash);
        if (flash == 0) {
            state = -1;
        } else if (flash == -2) {
            state = 1;
        } else if (flash == -3) {
            state = 0;
        }
        mFlashLight.setText(getStateText(state));
        mFlashLight.setTextColor(getStateColor(state));

        state = mSharedPreferences.getInt(TestUtils.LOUDSPEAKER_KEY + result, -1);
        byte speaker = 0;
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_AGINGTEST_SPEAKER, NvRAMUtils.INDEX_AGINGTEST_SPEAKER_LENGTH);
            speaker = buff[0];
        } catch (RemoteException e) {

        }
        Log.e("lsz", "speaker->" + speaker);
        if (speaker == 0) {
            state = -1;
        } else if (speaker == -2) {
            state = 1;
        } else if (speaker == -3) {
            state = 0;
        }

        mloudSpeaker.setText(getStateText(state));
        mloudSpeaker.setTextColor(getStateColor(state));

        state = mSharedPreferences.getInt(TestUtils.SCREEN_KEY + result, -1);
        byte screen = 0;
        try {
            byte[] buff = NvRAMUtils.readNVFromYt(NvRAMUtils.INDEX_AGINGTEST_SCREEN, NvRAMUtils.INDEX_AGINGTEST_SCREEN_LENGTH);
            screen = buff[0];
        } catch (RemoteException e) {

        }
        Log.e("lsz", "screen->" + screen);
        if (screen == 0) {
            state = -1;
        } else if (screen == -2) {
            state = 1;
        } else if (screen == -3) {
            state = 0;
        }

        mScreen.setText(getStateText(state));
        mScreen.setTextColor(getStateColor(state));

        mReset.setText(getStateText(isResetState));
        mReset.setTextColor(getStateColor(isResetState));
    }

    private int getStateText(int state) {
        switch (state) {
            case 0:
                return R.string.fail;

            case 1:
                return R.string.pass;

            default:
                return R.string.not_tested;
        }
    }

    private int getStateColor(int state) {
        Resources res = getResources();
        switch (state) {
            case 0:
                return res.getColor(R.color.fail_text_color);

            case 1:
                return res.getColor(R.color.pass_text_color);

            default:
                return res.getColor(R.color.not_test_text_color);
        }
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(android.os.Message msg) {
            Log.d("ReportActivity", "handleManager=>what: " + msg.what);
            switch (msg.what) {
                case MSG_CIRCULATION:
			/*	ArrayList<Class> testList = TestUtils.getTestList();
				Log.d("ReportActivity", "handlMessage=>size: " + testList.size() + " first: " + testList.get(0).toString());
				if (testList.contains(RebootActivity.class)) {
					Editor e = mSharedPreferences.edit();
					e.putBoolean(TestUtils.TEST_STATE, true);
					e.commit();
				}
				if (testList.size() > 0) {
					TestUtils.clearHistoryValue(ReportActivity.this);
					Intent intent = new Intent(ReportActivity.this, testList.get(0));
					intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK|Intent.FLAG_ACTIVITY_CLEAR_TASK);
					startActivity(intent);
				}*/
                    break;
            }
        };
    };

}
