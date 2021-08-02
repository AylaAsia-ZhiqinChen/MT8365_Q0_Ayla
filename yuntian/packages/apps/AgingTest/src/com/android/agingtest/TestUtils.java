package com.android.agingtest;

import java.util.ArrayList;
import java.util.HashMap;

import com.android.agingtest.test.BackTakingPictureActivity;
import com.android.agingtest.test.FlashLightActivity;
import com.android.agingtest.test.FrontTakingPictureActivity;
import com.android.agingtest.test.LoudSpeakerAcitity;
import com.android.agingtest.test.MotorZoomActivity;
import com.android.agingtest.test.PlayVideoActivity;
import com.android.agingtest.test.RebootActivity;
import com.android.agingtest.test.ReceiverActivity;
import com.android.agingtest.test.ResetActivity;
import com.android.agingtest.test.SleepActivity;
import com.android.agingtest.test.VibrateActivity;
import com.android.agingtest.test.ScreenActivity;

import android.app.KeyguardManager;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.Resources;
import android.os.PowerManager;
import android.os.Vibrator;
import android.os.SystemClock;
import android.preference.PreferenceManager;

public class TestUtils {

    public static final String IS_TEST = "is_in_test";
    public static final String REBOOT_STATE = "reboot_state";
    public static final String SLEEP_STATE = "sleep_state";
    public static final String VIBRATE_STATE = "vibrate_state";
    public static final String RECEIVER_STATE = "receiver_state";
    public static final String FRONT_TAKING_PICTURE_STATE = "front_taking_picture_state";
    public static final String BACK_TAKING_PICTURE_STATE = "back_taking_picture_state";
    public static final String PLAY_VIDEO_STATE = "play_video_state";
    public static final String CAMERA_MOTOR_STATE = "camera_motor_state";
    public static final String FLASH_LIGHT_STATE = "flash_light_state";
    public static final String LOUD_SPEAKER_STATE = "loud_speaker_state";
    public static final String CIRCULATION_STATE = "circulation_state";
    public static final String SCREEN_STATE = "screen_state";

    public static final String REBOOT_TIME = "reboot_time";
    public static final String SLEEP_TIME = "sleep_time";
    public static final String VIBRATE_TIME = "vibrate_time";
    public static final String RECEIVER_TIME = "receiver_time";
    public static final String FRONT_TAKING_PICTURE_TIME = "front_taking_picture_time";
    public static final String BACK_TAKING_PICTURE_TIME = "back_taking_picture_time";
    public static final String PLAY_VIDEO_TIME = "play_video_time";
    public static final String CAMERA_MOTOR_TIME = "camera_motor_time";
    public static final String FLASH_LIGHT_TIME = "flash_light_time";
    public static final String LOUD_SPEAKER_TIME = "loud_speaker_time";
    public static final String SCREEN_TIME = "screen_time";

    public static final String REBOOT_RESULT = "reboot_result";
    public static final String SLEEP_RESULT = "sleep_result";
    public static final String VIBRATE_RESULT = "vibrate_result";
    public static final String RECEIVER_RESULT = "receiver_result";
    public static final String FRONT_TAKING_PICTURE_RESULT = "front_taking_picture_result";
    public static final String BACK_TAKING_PICTURE_RESULT = "back_taking_picture_result";
    public static final String PLAY_VIDEO_RESULT = "play_video_result";
    public static final String CAMERA_MOTOR_RESULT = "camera_motor_result";
    public static final String FLASH_LIGHT_RESULT = "flash_light_result";
    public static final String LOUD_SPEAKER_RESULT = "loud_speaker_result";
    public static final String SCREEN_RESULT = "screen_result";

    public static final String REBOOT_START_TIME = "reboot_start_time";
    public static final String SLEEP_START_TIME = "sleep_start_time";
    public static final String VIBRATE_START_TIME = "vibrate_start_time";
    public static final String RECEIVER_START_TIME = "receiver_start_time";
    public static final String FRONT_TAKING_PICTURE_START_TIME = "front_taking_picture_start_time";
    public static final String BACK_TAKING_PICTURE_START_TIME = "back_taking_picture_start_time";
    public static final String PLAY_VIDEO_START_TIME = "play_video_start_time";
    public static final String CAMERA_MOTOR_START_TIME = "camera_motor_start_time";
    public static final String SCREEN_START_TIME = "screen_start_time";

    public static final String CIRCULATION_EXTRA = "circulation";
    public static final String CLASS_NAME_EXTRA = "class_name";



  //  public static final long MILLSECOND = 60 * 1000;
      public static final long MILLSECOND = 1000;

    private static final ArrayList<Class> mTestList = new ArrayList<Class>();

    public static ArrayList<Class> getTestList() {
        return mTestList;
    }

    public static void updateTestList(Context context) {
        Resources res = context.getResources();
        SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(context);
        boolean reboot = sp.getBoolean(REBOOT_STATE, res.getBoolean(R.bool.default_reboot_value));
        boolean sleep = sp.getBoolean(SLEEP_STATE, res.getBoolean(R.bool.default_sleep_value));
        boolean vibrate = sp.getBoolean(VIBRATE_STATE, res.getBoolean(R.bool.default_vibrate_value));
        boolean receiver = sp.getBoolean(RECEIVER_STATE, res.getBoolean(R.bool.default_receiver_value));
        boolean frontCamera = sp.getBoolean(FRONT_TAKING_PICTURE_STATE,
                res.getBoolean(R.bool.default_front_taking_picture_value));
        boolean backCamera = sp.getBoolean(BACK_TAKING_PICTURE_STATE,
                res.getBoolean(R.bool.default_back_taking_picture_value));
        boolean playVideo = sp.getBoolean(PLAY_VIDEO_STATE, res.getBoolean(R.bool.default_play_video_value));
        boolean cameramotor = sp.getBoolean(CAMERA_MOTOR_STATE, res.getBoolean(R.bool.default_camera_motor_value));
        boolean flashLight = sp.getBoolean(FLASH_LIGHT_STATE, res.getBoolean(R.bool.default_flashlight_value));
        boolean loudspeaker = sp.getBoolean(LOUD_SPEAKER_STATE, res.getBoolean(R.bool.default_loudspeaker_value));
        boolean screen = sp.getBoolean(SCREEN_STATE, res.getBoolean(R.bool.default_screen_value));
        Log.d("TestUtils", "updateTestList=>reboot: " + reboot + " sleep: " + sleep + " vibrate: " + vibrate + " receiver: "
                + receiver + " frontCamera: " + frontCamera + " backCamera: " + backCamera + " video: " + playVideo);
        mTestList.clear();
        if (reboot) {
            mTestList.add(RebootActivity.class);
        }

        if (sleep) {
            mTestList.add(SleepActivity.class);
        }

        if (vibrate) {
            mTestList.add(VibrateActivity.class);
        }

        if (receiver) {
            mTestList.add(ReceiverActivity.class);
        }

        if (frontCamera) {
            mTestList.add(FrontTakingPictureActivity.class);
        }

        if (backCamera) {
            mTestList.add(BackTakingPictureActivity.class);
        }

        if (cameramotor) {
            mTestList.add(MotorZoomActivity.class);
        }
        if (playVideo) {
            mTestList.add(PlayVideoActivity.class);
        }
        if (flashLight) {
            mTestList.add(FlashLightActivity.class);
        }
        if (loudspeaker) {
            mTestList.add(LoudSpeakerAcitity.class);
        }

        if (screen) {
            mTestList.add(ScreenActivity.class);
        }

        Log.d("TestUtils", "updateTestList=>size: " + mTestList.size() + " first: " + (mTestList.size() > 0 ? mTestList.get(0) : "null"));
        Log.d("TestUtils", "updateTestList=>list: " + mTestList.toString());
    }

    public static void clearHistoryValue(Context context) {
        Log.d("TestUtils", "clearHistoryValue()...");
        Resources res = context.getResources();
        Editor editor = PreferenceManager.getDefaultSharedPreferences(context).edit();

        editor.remove(TestUtils.REBOOT_RESULT);
        editor.remove(TestUtils.SLEEP_RESULT);
        editor.remove(TestUtils.VIBRATE_RESULT);
        editor.remove(TestUtils.RECEIVER_RESULT);
        editor.remove(TestUtils.FRONT_TAKING_PICTURE_RESULT);
        editor.remove(TestUtils.BACK_TAKING_PICTURE_RESULT);
        editor.remove(TestUtils.PLAY_VIDEO_RESULT);
        editor.remove(TestUtils.SCREEN_RESULT);

        editor.remove(TestUtils.REBOOT_START_TIME);
        editor.remove(TestUtils.SLEEP_START_TIME);
        editor.remove(TestUtils.VIBRATE_START_TIME);
        editor.remove(TestUtils.RECEIVER_START_TIME);
        editor.remove(TestUtils.FRONT_TAKING_PICTURE_START_TIME);
        editor.remove(TestUtils.BACK_TAKING_PICTURE_START_TIME);
        editor.remove(TestUtils.PLAY_VIDEO_START_TIME);
        editor.remove(TestUtils.CAMERA_MOTOR_START_TIME);
        editor.remove(TestUtils.SCREEN_START_TIME);

        editor.commit();
    }

    public static int getCurrentTestIndex(Class c) {
        int index = -1;
        for (int i = 0; i < mTestList.size(); i++) {
            Log.d("TestUtils", "getCurrentTestIndex=>" + i + ": " + mTestList.get(i).getSimpleName() + " c: " + c.getSimpleName());
            if (mTestList.get(i).equals(c)) {
                index = i;
                break;
            }
        }
        return index;
    }

    public static void reboot(Context context, String reason) {
        PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
        pm.reboot(reason);
    }

    public static void goToSleep(Context context) {
        PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
        pm.goToSleep(SystemClock.uptimeMillis());
    }

    public static void vibrate(Context context) {
        Vibrator v = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
        v.vibrate(new long[]{0, 1000, 0, 1000, 0, 1000}, 0);
    }

    public static void cancelVibrate(Context context) {
        Vibrator v = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
        if (v.hasVibrator()) {
            v.cancel();
        }
    }

    public static final String ALL_REBOOT_TIMES = "all_reboot_times";
    public static final String CURRENT_REBOOT_TIMES = "current_reboot_times";

    public static final String REBOOT_KEY = "reboot";
    public static final String SLEEP_KEY = "sleep";
    public static final String VIBRATE_KEY = "vibrate";
    public static final String RECEIVER_KEY = "receiver";
    public static final String FRONT_CAM_KEY = "front_camera";
    public static final String BACK_CAM_KEY = "back_camera";
    public static final String PLAY_VIDEO_KEY = "video";
    public static final String MOTORZOOM_KEY = "motor";
    public static final String FLASH_KEY = "flashlight";
    public static final String LOUDSPEAKER_KEY = "loudspeaker";
    public static final String RESET_KEY = "reset";
    public static final String SCREEN_KEY = "screen";

    public static final String KEY_INDEX = "current_test_index";
    public static final String ALLTESTKEYINDEX = "all_index";
    public static final String[] ALLKEYS = {REBOOT_KEY, SLEEP_KEY, VIBRATE_KEY, RECEIVER_KEY, FRONT_CAM_KEY, BACK_CAM_KEY, PLAY_VIDEO_KEY, MOTORZOOM_KEY, FLASH_KEY, LOUDSPEAKER_KEY/*,RESET_KEY*/, SCREEN_KEY, RESET_KEY};
    public static final String TEST_TIME = "time";
    public static final String TEST_RESULT = "result";
    public static final String TEST_STATE = "state";

    public static final Class[] ALLCLASSES = {RebootActivity.class, SleepActivity.class, VibrateActivity.class, ReceiverActivity.class,
            FrontTakingPictureActivity.class, BackTakingPictureActivity.class, PlayVideoActivity.class,
            MotorZoomActivity.class, FlashLightActivity.class, LoudSpeakerAcitity.class/*,ResetActivity.class*/, ScreenActivity.class, ResetActivity.class};

    public static final HashMap<String, Integer> indexMap = new HashMap<String, Integer>();

    static {
        indexMap.put(REBOOT_KEY, 0);
        indexMap.put(SLEEP_KEY, 1);

        indexMap.put(VIBRATE_KEY, 2);
        indexMap.put(RECEIVER_KEY, 3);

        indexMap.put(FRONT_CAM_KEY, 4);
        indexMap.put(BACK_CAM_KEY, 5);

        indexMap.put(PLAY_VIDEO_KEY, 6);
        indexMap.put(MOTORZOOM_KEY, 7);

        indexMap.put(FLASH_KEY, 8);
        indexMap.put(LOUDSPEAKER_KEY, 9);
        indexMap.put(SCREEN_KEY, 10);
		indexMap.put(RESET_KEY, 11);

    }
}

