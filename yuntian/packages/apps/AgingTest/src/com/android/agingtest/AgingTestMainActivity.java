package com.android.agingtest;

import java.util.ArrayList;

import com.android.agingtest.test.ReceiverActivity;
import com.android.agingtest.test.SleepActivity;
import com.android.agingtest.test.VibrateActivity;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.Resources;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;

public class AgingTestMainActivity extends Activity implements OnClickListener, OnCheckedChangeListener {

    private CheckBox mRebootCb;
    private CheckBox mSleepCb;
    private CheckBox mVibrateCb;
    private CheckBox mReceiverCb;
    private CheckBox mFrontTakingPictureCb;
    private CheckBox mBackTakingPictureCb;
    private CheckBox mPlayVideoCb;
    private CheckBox mCameraMotorCb;
    private CheckBox mFlashLightCB;
    private CheckBox mLoudSpeakerCB;
    private CheckBox mResetCB;
    //	private CheckBox mCirculationCb;
    private CheckBox mScreenCB;
    private Button mStartBtn;
    private Button mSelectAllBtn;

    private SharedPreferences mSharedPreferences;

    private ArrayList<String> testkeys = null;

    private final int ALL_TEST_ITEMS = 12;
    private CheckBox[] allChecBoxes = new CheckBox[ALL_TEST_ITEMS];
    private int itemNumber = TestUtils.ALLKEYS.length;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getWindow().addFlags(android.view.WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD);
        super.onCreate(savedInstanceState);
        Log.d(this, "onCreate()...");
        setContentView(R.layout.activity_aging_test);

        initValues();
        initViews();
        updateViewsVisible();
    }

    @Override
    protected void onResume() {
        Log.d(this, "onResume()...");
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        updateUI();
        updateAllTestKeys();

        mStartBtn.setEnabled(testkeys.size() > 0);

        super.onResume();
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        super.onPause();
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.aging_test, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        switch (item.getItemId()) {
            case R.id.action_test_time:
                Log.d(this, "onOptionsItemSelected=>set time.");
                Intent settings = new Intent(this, TimeSettingsActivity.class);
                if (getResources().getBoolean(R.bool.use_total_test_time)) {
                    settings = new Intent(this, TotalTimeSettingsActivity.class);
                }
                startActivity(settings);
                return true;

            case R.id.action_test_report:
                Log.d(this, "onOptionsItemSelected=>open report.");
                Intent report = new Intent(this, ReportActivity.class);
//			report.putExtra(TestUtils.CIRCULATION_EXTRA, false);
//			report.putExtra(TestUtils.CLASS_NAME_EXTRA, this.getClass().getSimpleName());
                startActivity(report);
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        Editor editor = mSharedPreferences.edit();
        String checkKey = "";
        switch (buttonView.getId()) {
            case R.id.reboot_test:
                checkKey = TestUtils.REBOOT_KEY;
                break;

            case R.id.sleep_test:

                checkKey = TestUtils.SLEEP_KEY;
                break;

            case R.id.vibrate_test:
                checkKey = TestUtils.VIBRATE_KEY;
                break;

            case R.id.receiver_test:
                checkKey = TestUtils.RECEIVER_KEY;
                break;

            case R.id.front_taking_picture_test:

                checkKey = TestUtils.FRONT_CAM_KEY;
                break;

            case R.id.back_taking_picture_test:

                checkKey = TestUtils.BACK_CAM_KEY;
                break;

            case R.id.play_video_test:
                checkKey = TestUtils.PLAY_VIDEO_KEY;
                break;

            case R.id.camera_motor_test:
                checkKey = TestUtils.MOTORZOOM_KEY;
                break;
            case R.id.flashlight_test:

                checkKey = TestUtils.FLASH_KEY;
                break;
            case R.id.loudspeaker_test:

                checkKey = TestUtils.LOUDSPEAKER_KEY;
                break;
            case R.id.reset_test:
                checkKey = TestUtils.RESET_KEY;
                break;
            case R.id.screen_test:

                checkKey = TestUtils.SCREEN_KEY;
                break;
        }
        editor.putBoolean(checkKey, isChecked);
        editor.commit();

        updateAllTestKeys();
        mStartBtn.setEnabled(testkeys.size() > 0);
    }

    private int[] getSortedIndex() {
        int length = testkeys.size();
        int[] indexs = new int[length];
        for (int i = 0; i < length; i++) {
            indexs[i] = TestUtils.indexMap.get(testkeys.get(i));
        }
//		已不需排序了

//		for(int j=1;j<length;j++){
//			int temp = indexs[j];
//			int k = j;
//			while(k>0&&indexs[k-1]>temp){
//				indexs[k] = indexs[k-1];
//				k--;
//			}
//			indexs[k] = temp;
//		}
        for (int i = 0; i < length; i++) {
            Log.e("lsz", "indexs[" + i + "]=" + indexs[i]);
        }
        return indexs;
    }

    private void updateAllTestKeys() {
        int length = TestUtils.ALLKEYS.length;
        testkeys.clear();
        for (int i = 0; i < length; i++) {
            if (mSharedPreferences.getBoolean(TestUtils.ALLKEYS[i], false)) {
                testkeys.add(TestUtils.ALLKEYS[i]);
            }
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.start:

                updateAllTestKeys();
                int[] indexs = getSortedIndex();
                int current_index = 0;
                if (getResources().getBoolean(R.bool.use_total_test_time)) {
                    int totalTestTime = mSharedPreferences.getInt("totaltime", getResources().getInteger(R.integer.default_total_test_time))*60;
                    boolean isResetChecked = mSharedPreferences.getBoolean(TestUtils.RESET_KEY, false);
                    if (testkeys.size() > 0) {
                        if (isResetChecked) {
                            if (testkeys.size() == 1) {
                                mSharedPreferences.edit().putInt("singleTestTime", 0).commit();
                            } else {
                                mSharedPreferences.edit().putInt("singleTestTime", totalTestTime / (testkeys.size() - 1)).commit();
                            }
                        } else {
                            mSharedPreferences.edit().putInt("singleTestTime", totalTestTime / testkeys.size()).commit();
						}
                    }
                }
                mSharedPreferences.edit().putBoolean(TestUtils.IS_TEST, true).commit();
                mSharedPreferences.edit().remove(TestUtils.CURRENT_REBOOT_TIMES).commit();
                mSharedPreferences.edit().remove(TestUtils.REBOOT_START_TIME).commit();
                Intent intent = new Intent(this, TestUtils.ALLCLASSES[indexs[current_index]]);
                intent.putExtra(TestUtils.ALLTESTKEYINDEX, indexs);
                intent.putExtra(TestUtils.KEY_INDEX, current_index);
                intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
                startActivity(intent);
                break;
            case R.id.select_all:
                Resources res = getResources();
                Editor editor = mSharedPreferences.edit();
				if (testkeys.size() == itemNumber) {
                    if (res.getBoolean(R.bool.reboot_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[0], false);
                        allChecBoxes[0].setChecked(false);
                    }

                    if (res.getBoolean(R.bool.sleep_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[1], false);
                        allChecBoxes[1].setChecked(false);
                    }

                    if (res.getBoolean(R.bool.vibrate_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[2], false);
                        allChecBoxes[2].setChecked(false);
                    }

                    if (res.getBoolean(R.bool.receiver_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[3], false);
                        allChecBoxes[3].setChecked(false);
                    }

                    if (res.getBoolean(R.bool.front_taking_picture_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[4], false);
                        allChecBoxes[4].setChecked(false);
                    }

                    if (res.getBoolean(R.bool.back_taking_picture_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[5], false);
                        allChecBoxes[5].setChecked(false);
                    }

                    if (res.getBoolean(R.bool.play_video_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[6], false);
                        allChecBoxes[6].setChecked(false);
                    }

                    if (res.getBoolean(R.bool.camera_motor_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[7], false);
                        allChecBoxes[7].setChecked(false);
                    }

                    if (res.getBoolean(R.bool.flash_light_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[8], false);
                        allChecBoxes[8].setChecked(false);
                    }

                    if (res.getBoolean(R.bool.loudspeaker_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[9], false);
                        allChecBoxes[9].setChecked(false);
                    }

                    if (res.getBoolean(R.bool.screen_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[10], false);
                        allChecBoxes[10].setChecked(false);
                    }

                    if (res.getBoolean(R.bool.reset_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[11], false);
                        allChecBoxes[11].setChecked(false);
                    }
                    editor.commit();
                } else {
                    if (res.getBoolean(R.bool.reboot_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[0], true);
                        allChecBoxes[0].setChecked(true);
                    }

                    if (res.getBoolean(R.bool.sleep_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[1], true);
                        allChecBoxes[1].setChecked(true);
                    }

                    if (res.getBoolean(R.bool.vibrate_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[2], true);
                        allChecBoxes[2].setChecked(true);
                    }

                    if (res.getBoolean(R.bool.receiver_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[3], true);
                        allChecBoxes[3].setChecked(true);
                    }

                    if (res.getBoolean(R.bool.front_taking_picture_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[4], true);
                        allChecBoxes[4].setChecked(true);
                    }

                    if (res.getBoolean(R.bool.back_taking_picture_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[5], true);
                        allChecBoxes[5].setChecked(true);
                    }

                    if (res.getBoolean(R.bool.play_video_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[6], true);
                        allChecBoxes[6].setChecked(true);
                    }

                    if (res.getBoolean(R.bool.camera_motor_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[7], true);
                        allChecBoxes[7].setChecked(true);
                    }

                    if (res.getBoolean(R.bool.flash_light_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[8], true);
                        allChecBoxes[8].setChecked(true);
                    }

                    if (res.getBoolean(R.bool.loudspeaker_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[9], true);
                        allChecBoxes[9].setChecked(true);
                    }

                    if (res.getBoolean(R.bool.screen_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[10], true);
                        allChecBoxes[10].setChecked(true);
                    }

                    if (res.getBoolean(R.bool.reset_visible)) {
                        editor.putBoolean(TestUtils.ALLKEYS[11], true);
                        allChecBoxes[11].setChecked(true);
                    }
                    editor.commit();
                }
                break;
        }
    }

    private void initValues() {
        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
//		Editor e = mSharedPreferences.edit();
//		e.putBoolean(TestUtils.TEST_STATE, false);
//		e.commit();
        testkeys = new ArrayList<String>();
    }

    private void initViews() {
        mRebootCb = (CheckBox) findViewById(R.id.reboot_test);
        allChecBoxes[0] = mRebootCb;

        mSleepCb = (CheckBox) findViewById(R.id.sleep_test);
        allChecBoxes[1] = mSleepCb;

        mVibrateCb = (CheckBox) findViewById(R.id.vibrate_test);
        allChecBoxes[2] = mVibrateCb;

        mReceiverCb = (CheckBox) findViewById(R.id.receiver_test);
        allChecBoxes[3] = mReceiverCb;

        mFrontTakingPictureCb = (CheckBox) findViewById(R.id.front_taking_picture_test);
        allChecBoxes[4] = mFrontTakingPictureCb;

        mBackTakingPictureCb = (CheckBox) findViewById(R.id.back_taking_picture_test);
        allChecBoxes[5] = mBackTakingPictureCb;

        mPlayVideoCb = (CheckBox) findViewById(R.id.play_video_test);
        allChecBoxes[6] = mPlayVideoCb;

        mCameraMotorCb = (CheckBox) findViewById(R.id.camera_motor_test);
        allChecBoxes[7] = mCameraMotorCb;

        mFlashLightCB = (CheckBox) findViewById(R.id.flashlight_test);
        allChecBoxes[8] = mFlashLightCB;

        mLoudSpeakerCB = (CheckBox) findViewById(R.id.loudspeaker_test);
        allChecBoxes[9] = mLoudSpeakerCB;

        mScreenCB = (CheckBox) findViewById(R.id.screen_test);
        allChecBoxes[10] = mScreenCB;

        mResetCB= (CheckBox) findViewById(R.id.reset_test);
        allChecBoxes[11] = mResetCB;

        mStartBtn = (Button) findViewById(R.id.start);
        mSelectAllBtn = (Button) findViewById(R.id.select_all);
//		mCirculationCb = (CheckBox) findViewById(R.id.circulation_test);
//		mStopBt = (Button) findViewById(R.id.stop);

        mRebootCb.setOnCheckedChangeListener(this);
        mSleepCb.setOnCheckedChangeListener(this);
        mVibrateCb.setOnCheckedChangeListener(this);
        mReceiverCb.setOnCheckedChangeListener(this);
        mFrontTakingPictureCb.setOnCheckedChangeListener(this);
        mBackTakingPictureCb.setOnCheckedChangeListener(this);
        mPlayVideoCb.setOnCheckedChangeListener(this);
        mCameraMotorCb.setOnCheckedChangeListener(this);
        mFlashLightCB.setOnCheckedChangeListener(this);
        mLoudSpeakerCB.setOnCheckedChangeListener(this);
        mScreenCB.setOnCheckedChangeListener(this);
        mResetCB.setOnCheckedChangeListener(this);

//		mCirculationCb.setOnCheckedChangeListener(this);

        mStartBtn.setOnClickListener(this);
        mSelectAllBtn.setOnClickListener(this);
    }

    private void updateViewsVisible() {
        Resources res = getResources();
        if (!res.getBoolean(R.bool.reboot_visible)) {
            mRebootCb.setVisibility(View.GONE);
            itemNumber --;
        }

        if (!res.getBoolean(R.bool.sleep_visible)) {
            mSleepCb.setVisibility(View.GONE);
            itemNumber --;
        }

        if (!res.getBoolean(R.bool.vibrate_visible)) {
            mVibrateCb.setVisibility(View.GONE);
            itemNumber --;
        }

        if (!res.getBoolean(R.bool.receiver_visible)) {
            mReceiverCb.setVisibility(View.GONE);
            itemNumber --;
        }

        if (!res.getBoolean(R.bool.front_taking_picture_visible)) {
            mFrontTakingPictureCb.setVisibility(View.GONE);
            itemNumber --;
        }

        if (!res.getBoolean(R.bool.back_taking_picture_visible)) {
            mBackTakingPictureCb.setVisibility(View.GONE);
            itemNumber --;
        }

        if (!res.getBoolean(R.bool.play_video_visible)) {
            mPlayVideoCb.setVisibility(View.GONE);
            itemNumber --;
        }

        if (!res.getBoolean(R.bool.camera_motor_visible)) {
            mCameraMotorCb.setVisibility(View.GONE);
            itemNumber --;
        }

        if (!res.getBoolean(R.bool.flash_light_visible)) {
            mFlashLightCB.setVisibility(View.GONE);
            itemNumber --;
        }

        if (!res.getBoolean(R.bool.loudspeaker_visible)) {
            mLoudSpeakerCB.setVisibility(View.GONE);
            itemNumber --;
        }

        if (!res.getBoolean(R.bool.select_all_visible)) {
            mSelectAllBtn.setVisibility(View.GONE);
            itemNumber --;
        }

        if (!res.getBoolean(R.bool.screen_visible)) {
            mScreenCB.setVisibility(View.GONE);
            itemNumber --;
        }
        if (!res.getBoolean(R.bool.reset_visible)) {
            mResetCB.setVisibility(View.GONE);
            itemNumber --;
        }
    }

    private void updateUI() {
        Resources res = getResources();

        int length = TestUtils.ALLKEYS.length;
        for (int i = 0; i < length; i++) {
            boolean isChecked = mSharedPreferences.getBoolean(TestUtils.ALLKEYS[i], res.getBoolean(R.bool.default_reboot_value));
            allChecBoxes[i].setChecked(isChecked);
            if (isChecked) {
                testkeys.add(TestUtils.ALLKEYS[i]);
            }
            allChecBoxes[i].setTextColor(getStateColor(mSharedPreferences.getInt(TestUtils.ALLKEYS[i] + TestUtils.TEST_RESULT, -1)));
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

}
