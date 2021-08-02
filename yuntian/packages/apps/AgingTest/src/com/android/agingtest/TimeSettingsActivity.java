package com.android.agingtest;

import android.app.ActionBar;
import android.app.Activity;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.Resources;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.view.View.OnFocusChangeListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class TimeSettingsActivity extends Activity implements OnClickListener, OnFocusChangeListener {

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
    private View mScreenContainer;
    private View mResetContainer;

    private EditText mRebootEt;
    private EditText mSleepEt;
    private EditText mVibrateEt;
    private EditText mReceiverEt;
    private EditText mFrontTakingPictureEt;
    private EditText mBackTakingPictureEt;
    private EditText mPlayVideoEt;
    private EditText mCameraMotor;
    private EditText mFlashLight;
    private EditText mLoudSpeaker;
    private EditText mScreen;
    private EditText mReset;

    private EditText[] allTimeEt = new EditText[11];
    private Button mOk;
    private Button mCancel;
    private SharedPreferences mSharedPreferences;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        initActionBar();

        setContentView(R.layout.activity_settings);

        initValues();
        initViews();
    }

    @Override
    protected void onStart() {
        updateViewsVisible();
        super.onStart();
    }

    @Override
    protected void onResume() {
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        updateUI();
        super.onResume();
    }

    @Override
    protected void onPause() {
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        super.onPause();
    }

    @Override
    public boolean onMenuItemSelected(int featureId, MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                finish();
                break;
        }
        return super.onMenuItemSelected(featureId, item);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.ok:
                boolean result = updateSharedPreference();
                Toast.makeText(this, result ? R.string.setting_success : R.string.setting_fail, Toast.LENGTH_SHORT).show();
                finish();
                break;

            case R.id.cancel:
                finish();
                break;
        }
    }

    @Override
    public void onFocusChange(View v, boolean hasFocus) {
        if (hasFocus) {
            switch (v.getId()) {
                case R.id.reboot_time:
                    mRebootEt.setSelection(mRebootEt.getText().toString().length());
                    break;

                case R.id.sleep_time:
                    mSleepEt.setSelection(mSleepEt.getText().toString().length());
                    break;

                case R.id.vibrate_time:
                    mVibrateEt.setSelection(mVibrateEt.getText().toString().length());
                    break;

                case R.id.receiver_time:
                    mReceiverEt.setSelection(mReceiverEt.getText().toString().length());
                    break;

                case R.id.front_taking_picture_time:
                    mFrontTakingPictureEt.setSelection(mFrontTakingPictureEt.getText().toString().length());
                    break;

                case R.id.back_taking_picture_time:
                    mBackTakingPictureEt.setSelection(mBackTakingPictureEt.getText().toString().length());
                    break;

                case R.id.play_video_time:
                    mPlayVideoEt.setSelection(mPlayVideoEt.getText().toString().length());
                    break;
                case R.id.motor_time:
                    mCameraMotor.setSelection(mCameraMotor.getText().toString().length());
                    break;

                case R.id.flashlight_time:
                    mFlashLight.setSelection(mFlashLight.getText().toString().length());
                    break;

                case R.id.loudspeaker_time:
                    mLoudSpeaker.setSelection(mLoudSpeaker.getText().toString().length());
                    break;

                case R.id.screen_time:
                    mScreen.setSelection(mScreen.getText().toString().length());
                    break;


                case R.id.reset_time:
                    mReset.setSelection(mReset.getText().toString().length());
                    break;

            }
        }
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
        mCameraMotorContainer = findViewById(R.id.motor_container);
        mFlashLightContainer = findViewById(R.id.flashlight_container);
        mLoudSpeakerContainer = findViewById(R.id.loudspeaker_container);
        mScreenContainer = findViewById(R.id.screen_container);
        mResetContainer = findViewById(R.id.reset_container);

        mRebootEt = (EditText) findViewById(R.id.reboot_time);
        mSleepEt = (EditText) findViewById(R.id.sleep_time);
        mVibrateEt = (EditText) findViewById(R.id.vibrate_time);
        mReceiverEt = (EditText) findViewById(R.id.receiver_time);
        mFrontTakingPictureEt = (EditText) findViewById(R.id.front_taking_picture_time);
        mBackTakingPictureEt = (EditText) findViewById(R.id.back_taking_picture_time);
        mPlayVideoEt = (EditText) findViewById(R.id.play_video_time);
        mCameraMotor = (EditText) findViewById(R.id.motor_time);
        mFlashLight = (EditText) findViewById(R.id.flashlight_time);
        mLoudSpeaker = (EditText) findViewById(R.id.loudspeaker_time);
        mScreen = (EditText) findViewById(R.id.screen_time);
        mReset = (EditText) findViewById(R.id.reset_time);

        allTimeEt[0] = mRebootEt;
        allTimeEt[1] = mSleepEt;
        allTimeEt[2] = mVibrateEt;
        allTimeEt[3] = mReceiverEt;
        allTimeEt[4] = mFrontTakingPictureEt;
        allTimeEt[5] = mBackTakingPictureEt;
        allTimeEt[6] = mPlayVideoEt;
        allTimeEt[7] = mCameraMotor;
        allTimeEt[8] = mFlashLight;
        allTimeEt[9] = mLoudSpeaker;
        allTimeEt[10] = mScreen;
        allTimeEt[11] = mReset;
        mOk = (Button) findViewById(R.id.ok);
        mCancel = (Button) findViewById(R.id.cancel);
        mOk.setOnClickListener(this);
        mCancel.setOnClickListener(this);
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

        if (!res.getBoolean(R.bool.screen_visible)) {
            mScreenContainer.setVisibility(View.GONE);
        }

        if (!res.getBoolean(R.bool.reset_visible)) {
            mResetContainer.setVisibility(View.GONE);
        }

    }

    private void updateUI() {
        Resources res = getResources();

        int length = allTimeEt.length;
        for (int i = 0; i < length; i++) {
            String time_key = TestUtils.ALLKEYS[i] + TestUtils.TEST_TIME;
            EditText et = allTimeEt[i];
            et.setText(mSharedPreferences.getInt(time_key, res.getInteger(R.integer.default_test_time)) + "");
            et.setOnFocusChangeListener(this);
        }

    }

    private boolean updateSharedPreference() {
        boolean result = true;
        int time = 0;
        Resources res = getResources();
        Editor editor = mSharedPreferences.edit();
        String testTime = TestUtils.TEST_TIME;
        int length = allTimeEt.length;
        try {
            for (int i = 0; i < length; i++) {
                String time_key = TestUtils.ALLKEYS[i] + TestUtils.TEST_TIME;
                String timeStr = allTimeEt[i].getText().toString();
                if (TextUtils.isEmpty(timeStr)
                        || !TextUtils.isDigitsOnly(timeStr)) {
                    editor.putInt(time_key,
                            res.getInteger(R.integer.default_reboot_time));
                } else {
                    time = Integer.parseInt(timeStr);
                    editor.putInt(time_key, time);
                }
            }
        } catch (Exception e) {
            // TODO: handle exception
            e.printStackTrace();
            result = false;
        } finally {
            editor.commit();
        }
        return result;
    }

}
