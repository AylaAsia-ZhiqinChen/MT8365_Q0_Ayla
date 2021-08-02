package com.android.factorytest.sensor;

import android.hardware.fingerprint.FingerprintManager;
import android.os.Bundle;
import android.os.CancellationSignal;
import android.os.UserHandle;
import android.widget.TextView;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import android.os.SystemProperties;
import android.os.Vibrator;//add by yt_wxc for vibrating when enrolling
import android.app.Service;

import java.nio.ByteBuffer;//add by yt_wxc for cdfinger test
//yuntian longyao add
//Description:工模里面指纹概率性无作用
import android.os.Handler;
import android.os.Message;
import android.os.UserManager;
import android.content.Context;

import android.annotation.SuppressLint;
//yuntian longyao end
public class FingerprintSensorTest extends BaseActivity {


    private TextView mFingerprintTipTv;
    private FingerprintManager mFingerprintManager = null;
    private CancellationSignal mEnrollmentCancel;
    private Vibrator mVibrator;//add by yt_wxc for vibrating when enrolling
    //yuntian longyao add
    //Description:工模里面指纹概率性无作用
    private static final String TAG = "FingerprintSensorTest";
    private static final int FINGERPRINT_ENROLL_TIMEOUT = 120000;
    private Handler mHandler = new Handler();
    private int mEnrollmentRemaining = 0;
    private int mEnrollmentSteps = -1;
    private int mUserId;
    private boolean mDone;
    //yuntian longyao end
    private boolean mEnrolling;
    private byte[] mFingerprintToken;
    public static final boolean IS_PENSOR_SUPPORT_SUNWAVE = SystemProperties.get("ro.yuntian.fingerprint.chip").equals("sunwave_ree");
    public static final boolean IS_PENSOR_SUPPORT_CDFINGER = SystemProperties.get("ro.yuntian.fingerprint.chip").equals("cdfinger_fp_ree");//add by yt_wxc for cdfinger test
    private static final int CD_FINGERPRINT_CMD_FINGER_DETECTED = 1002;//add by yt_wxc for cdfinger test
    //yuntian longyao add
    //Description:工模里面指纹概率性无作用
    private static final int CD_FINGERPRINT_CMD_SELF_CHECK = 1000;
    private static final int CD_FINGERPRINT_CMD_DEINIT= 1001;
    private static final int CD_FINGERPRINT_CMD_DEAD_PIXEL_TEST = 1004;
    private int cmd = 0;
    private boolean isPause;
    //yuntian longyao end
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_fingerprint_test);

        super.onCreate(savedInstanceState);
        //yuntian longyao add
        //Description:工模里面指纹概率性无作用
        UserManager um = UserManager.get(this);
        mUserId = getCredentialOwnerUserId(UserHandle.myUserId());
        //yuntian longyao end
        mFingerprintManager = (FingerprintManager) getSystemService(FINGERPRINT_SERVICE);

        mFingerprintTipTv = (TextView) findViewById(R.id.fingerprint_test_tip);
        mVibrator = (Vibrator) getApplication().getSystemService(Service.VIBRATOR_SERVICE);//add by yt_wxc for vibrating when enrolling
    }

    @Override
    protected void onResume() {
        super.onResume();
        //yuntian longyao add
        //Description:工模里面指纹概率性无作用
        if (IS_PENSOR_SUPPORT_CDFINGER) {
            isPause = false;
            cmd = CD_FINGERPRINT_CMD_SELF_CHECK;
            testCmd(cmd);
        }
        //yuntian longyao end
        //yuntian longyao add
        //Description:工模里面指纹概率性无作用
        if (!mEnrolling) {
            startEnrollment();
        }
        //yuntian longyao end
    }

    @Override
    protected void onPause() {
        super.onPause();
        //yuntian longyao add
        //Description:工模里面指纹概率性无作用
        if (IS_PENSOR_SUPPORT_CDFINGER) {
            isPause = true;
            cmd = CD_FINGERPRINT_CMD_DEINIT;
            testCmd(cmd);
            if (mEnrollmentCancel != null && !mEnrollmentCancel.isCanceled()){
                mEnrollmentCancel.cancel();
            }
        }
        //yuntian longyao end
        //yuntian longyao add
        //Description:工模里面指纹概率性无作用
        Log.d(TAG, "onPause=>changing: " + isChangingConfigurations());
        if (!isChangingConfigurations()) {
            cancelEnrollment();
        }
        //yuntian longyao end
        mVibrator.cancel();//add by yt_wxc for vibrating when enrolling
    }

    private void startEnrollment() {
        //yuntian longyao add
        //Description:工模里面指纹概率性无作用
        mHandler.removeCallbacks(mTimeoutRunnable);
        mEnrollmentSteps = -1;
        if (mUserId != UserHandle.USER_NULL) {
            mFingerprintManager.setActiveUser(mUserId);
        }
        //yuntian longyao end
        mEnrollmentCancel = new CancellationSignal();
        mFingerprintToken = new byte[69];
        long challenge = mFingerprintManager.preEnroll();
        for (int i = 0; i < mFingerprintToken.length; i++) {
            mFingerprintToken[i] = IS_PENSOR_SUPPORT_SUNWAVE ? ((byte) 0xFF) : ((byte) (i % 10));
        }

        if (IS_PENSOR_SUPPORT_SUNWAVE) {
            mFingerprintToken[33] = 2;
            mFingerprintToken[34] = 0;
            mFingerprintToken[35] = 0;
            mFingerprintToken[36] = 0;

            int challenge_h = (int) (challenge >> 32);
            int challenge_l = (int) (challenge & 0xFFFFFFFF);

            mFingerprintToken[8] = (byte) (challenge_h >> 24);
            mFingerprintToken[7] = (byte) ((challenge_h & 0xff0000) >> 16);
            mFingerprintToken[6] = (byte) ((challenge_h & 0xff00) >> 8);
            mFingerprintToken[5] = (byte) (challenge_h & 0xff);

            mFingerprintToken[4] = (byte) (challenge_l >> 24);
            mFingerprintToken[3] = (byte) ((challenge_l & 0xff0000) >> 16);
            mFingerprintToken[2] = (byte) ((challenge_l & 0xff00) >> 8);
            mFingerprintToken[1] = (byte) (challenge_l & 0xff);

            mFingerprintToken[0] = (byte) 0;
        }

        //add by yt_wxc for cdfinger test begin
        if (IS_PENSOR_SUPPORT_SUNWAVE) {
            mFingerprintManager.enroll(mFingerprintToken, mEnrollmentCancel, 0, mUserId, mEnrollmentCallback);
        }
        //add by yt_wxc for cdfinger test end
        mEnrolling = true;
    }

    //yuntian longyao add
    //Description:工模里面指纹概率性无作用
    private boolean cancelEnrollment() {
        mHandler.removeCallbacks(mTimeoutRunnable);
        if (mEnrolling) {
            mEnrollmentCancel.cancel();
            mEnrolling = false;
            mEnrollmentSteps = -1;
            return true;
        }
        return false;
    }

    public int getCredentialOwnerUserId(int userId) {
        UserManager um = getUserManager(this);
        return um.getCredentialOwnerProfile(userId);
    }

    public UserManager getUserManager(Context context) {
        UserManager um = UserManager.get(context);
        if (um == null) {
            throw new IllegalStateException("Unable to load UserManager");
        }
        return um;
    }

    //yuntian longyao end
    private FingerprintManager.EnrollmentCallback mEnrollmentCallback = new FingerprintManager.EnrollmentCallback() {

        @Override
        public void onEnrollmentProgress(int remaining) {
            //yuntian longyao add
            //Description:工模里面指纹概率性无作用
            Log.d(TAG, "onEnrollmentProgress=>remaining: " + remaining);
            mEnrollmentRemaining = remaining;
            mDone = remaining == 0;
            cancelEnrollment();
            //yuntian longyao end
            mVibrator.vibrate(50);//add by yt_wxc for vibrating when enrolling
            mFingerprintTipTv.setText(R.string.fingerprint_test_pass_tip);
            setPassButtonEnabled(true);
        }

        @Override
        public void onEnrollmentHelp(int helpMsgId, CharSequence helpString) {
            Log.d(TAG, "onEnrollmentHelp=>id: " + helpMsgId + " help: " + helpString);
            //yuntian longyao add
            //Description:工模里面指纹概率性无作用
            if (IS_PENSOR_SUPPORT_CDFINGER) {
                Message msg = Message.obtain();
                msg.what = cmd;
                msg.arg1 = helpMsgId - 5;
                mCDHandler.sendMessage(msg);
            }
            //yuntian longyao end	
            //add by yt_wxc for cdfinger test begin
            //yuntian longyao add
            //Description:工模里面指纹概率性无作用
            cancelEnrollment();
            //yuntian longyao end
            //add by yt_wxc for cdfinger test end
        }

        @Override
        public void onEnrollmentError(int errMsgId, CharSequence errString) {
            Log.d(TAG, "onEnrollmentError=>id: " + errMsgId + " error: " + errString);
            //yuntian longyao add
            //Description:工模里面指纹概率性无作用
            mEnrolling = false;
            //yuntian longyao end
        }

    };
    //yuntian longyao add
    //Description:工模里面指纹概率性无作用
    private final Runnable mTimeoutRunnable = new Runnable() {
        @Override
        public void run() {
            cancelEnrollment();
        }
    };

    //yuntian longyao end
    //add by yt_wxc for cdfinger test begin
    static private byte[] intToByteArray(final int integer) {
        int byteNum = (40 - Integer.numberOfLeadingZeros(integer < 0 ? ~integer : integer)) / 8;
        byte[] byteArray = new byte[4];
        for (int n = 0; n < byteNum; n++)
            byteArray[3 - n] = (byte) (integer >>> (n * 8));
        return (byteArray);
    }

    static public byte[] getFakeTokenFromCmd(int cmd) {
        ByteBuffer buffer = ByteBuffer.allocate(69);
        byte[] token_x = new byte[37];
        buffer.put(token_x);
        buffer.put(intToByteArray(cmd));
        return buffer.array();
    }
    //add by yt_wxc for cdfinger test end
    //yuntian longyao add
    //Description:工模里面指纹概率性无作用
    void testCmd(int cmd) {
        Log.d(TAG, "testCmd: cmd = " + cmd);
        mEnrollmentCancel = new CancellationSignal();
        mFingerprintManager.enroll(getFakeTokenFromCmd(cmd), mEnrollmentCancel, 0, 0, mEnrollmentCallback);
    }
    @SuppressLint("HandlerLeak")
    private Handler mCDHandler = new Handler() {
        public void handleMessage(Message msg) {
            Log.d(TAG, "testCmd:cmd = " + msg.what + ", result = " + msg.arg1);
            if (!isPause) {
                switch (msg.what) {
                    case CD_FINGERPRINT_CMD_SELF_CHECK:
                        cmd = CD_FINGERPRINT_CMD_DEAD_PIXEL_TEST;
                        testCmd(cmd);
                        break;
                    case CD_FINGERPRINT_CMD_DEAD_PIXEL_TEST:
                        cmd = CD_FINGERPRINT_CMD_FINGER_DETECTED;
                        testCmd(cmd);
                        break;
                    case CD_FINGERPRINT_CMD_FINGER_DETECTED:
                        mVibrator.vibrate(50);
                        mFingerprintTipTv.setText(R.string.fingerprint_test_pass_tip);
                        setPassButtonEnabled(true);
                        break;
                    default:
                        break;
                }
            }
        }
    };
    //yuntian longyao end

}
