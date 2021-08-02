package com.android.agingtest.test;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Date;
import java.util.List;
import java.util.TimeZone;

import com.android.agingtest.Log;
import com.android.agingtest.TestUtils;
import com.android.agingtest.R;
import com.android.agingtest.ReportActivity;

import android.app.ActionBar;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.Parameters;
import android.hardware.camera2.CameraManager;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.preference.PreferenceManager;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MenuItem;
import android.view.SurfaceHolder;
import android.view.WindowManager;
import android.view.SurfaceHolder.Callback;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;
import android.os.RemoteException;
import com.mediatek.nvram.NvRAMUtils;

public class BackTakingPictureActivity extends Activity implements OnClickListener, Callback {

    public static final int FLAG_HOMEKEY_DISPATCHED = 0x80000000;

    private static final int MSG_UPDATE_TIME = 0;
    private static final int MSG_WAKE_UP = 1;
    private static final int MSG_GOTO_SLEEP = 2;

    private TextView mTestTimeTv;
    private Button mStopBt;
    private SharedPreferences mSharedPreferences;
    private Camera mCamera;
    private SurfaceHolder mSurfaceHolder;
    private SurfaceView mSurfaceView;
    private CameraManager mCameraManager;
    private PowerManager mPowerManager;
    private WakeLock mLock;

    private String currentPicturePath;
    private long mBackTakingPictureTime;
    private long mStartTime;
    private int mCameraId;
    private int mBackCameraId = -1;
    private int mFrontCameraId = -1;
    private int mDisplayRotation;
    private int mDisplayOrientation;
    private boolean mIsPreviewing;
    private boolean safeToTakePicture;
    private boolean bIfPhoto;

    private boolean isStopTest = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        getWindow().addFlags(FLAG_HOMEKEY_DISPATCHED);
        getWindow().addFlags(android.view.WindowManager.LayoutParams.FLAG_DISMISS_KEYGUARD);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        super.onCreate(savedInstanceState);
        Log.d(this, "onCreate()...");
        initActionBar();
        setContentView(R.layout.activity_taking_picture);
        initValues();
        initViews();
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(this, "onResume()...");
        startTest();
    }

    private void startTest() {
        mStartTime = System.currentTimeMillis();
        mCameraId = mBackCameraId;
        mSurfaceHolder = mSurfaceView.getHolder();
        mSurfaceHolder.addCallback(this);
        mSurfaceHolder.setType(3);
        startPreview(mCameraId);
        mHandler.sendEmptyMessage(MSG_UPDATE_TIME);
    }

    private void stopTest(boolean isOk) {
        if (!isStopTest) {
            mHandler.removeMessages(MSG_UPDATE_TIME);
            getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
            stopPreview();
            Thread delete = new Thread(new Runnable() {
                @Override
                public void run() {
                    File[] tmps = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM).listFiles();
                    if (tmps != null) {
                        for (int i = 0; i < tmps.length; i++) {
                            if (tmps[i].getName().endsWith("test.jpg")) {
                                tmps[i].delete();
                            }
                        }
                    }
                }
            });
            delete.start();
            Editor e = mSharedPreferences.edit();
            e.putInt(TestUtils.BACK_CAM_KEY + TestUtils.TEST_RESULT, isOk ? 1 : 0);
            e.commit();
     byte result = -1;
        if (isOk) {
            result = -2; //成功-2
        } else {
            result = -3; //失败-3
        }
        byte[] buff = new byte[1];
        buff[0] = result;
           try {
                    NvRAMUtils.writeNVToYt(NvRAMUtils.INDEX_AGINGTEST_BACK_CAMERA, buff);
                } catch (RemoteException e4) {

                }
            startNext();
            isStopTest = true;
        }
    }

    private void startNext() {
        Intent intent = null;
        int currentIndex = key_index + 1;
        if (currentIndex < alltestindex.length) {
            String nexKey = TestUtils.ALLKEYS[alltestindex[currentIndex]];
            intent = new Intent(this, TestUtils.ALLCLASSES[alltestindex[currentIndex]]);
            intent.putExtra(TestUtils.ALLTESTKEYINDEX, alltestindex);
            intent.putExtra(TestUtils.KEY_INDEX, currentIndex);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
            startActivity(intent);
        } else {
            intent = new Intent(this, ReportActivity.class);
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TASK | Intent.FLAG_ACTIVITY_CLEAR_TOP);
            startActivity(intent);
        }
        finish();
    }

    private boolean isTestOver() {
        return System.currentTimeMillis() - mStartTime >= mBackTakingPictureTime;
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mLock.isHeld()) {
            mLock.release();
        }
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        Log.d(this, "dispatchKeyEvent=>keycode: " + event.getKeyCode());
        switch (event.getKeyCode()) {
            case KeyEvent.KEYCODE_BACK:
            case KeyEvent.KEYCODE_HOME:
                Toast.makeText(this, R.string.testing_tip, Toast.LENGTH_SHORT).show();
                return true;
        }
        return super.dispatchKeyEvent(event);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.stop_test:
                stopTest(false);
                break;
        }
    }

    private void initActionBar() {
        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
    }

    private int[] alltestindex = null;
    private int key_index = -1;
    String key;

    private void initValues() {
        key_index = getIntent().getIntExtra(TestUtils.KEY_INDEX, -1);
        alltestindex = getIntent().getIntArrayExtra(TestUtils.ALLTESTKEYINDEX);
        Log.e("lsz", "key_index->" + key_index + ",alltestindex[key_index]->" + TestUtils.ALLKEYS[alltestindex[key_index]]);
        key = TestUtils.BACK_CAM_KEY;
        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        mPowerManager = (PowerManager) getSystemService(Context.POWER_SERVICE);
        mLock = mPowerManager.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "back_taking_picture_test");
        mLock.acquire();


        mBackTakingPictureTime = mSharedPreferences.getInt(TestUtils.BACK_CAM_KEY + TestUtils.TEST_TIME,
                getResources().getInteger(R.integer.default_back_taking_picture_time)) * TestUtils.MILLSECOND;
        if (getResources().getBoolean(R.bool.use_total_test_time)) {
            mBackTakingPictureTime = mSharedPreferences.getInt("singleTestTime",
                    getResources().getInteger(R.integer.default_back_taking_picture_time)) * TestUtils.MILLSECOND;
        }

        Log.d(this, "initValues=>startTime: " + mStartTime + " time: " + mBackTakingPictureTime);
        getCameraId();
    }

    private void initViews() {
        mTestTimeTv = (TextView) findViewById(R.id.test_time);
        mStopBt = (Button) findViewById(R.id.stop_test);
        mSurfaceView = (SurfaceView) findViewById(R.id.camera_surface);

        mTestTimeTv.setText(R.string.default_time_string);
        mStopBt.setOnClickListener(this);
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(android.os.Message msg) {
            switch (msg.what) {
                case MSG_UPDATE_TIME:
                    long testTime = System.currentTimeMillis() - mStartTime;
                    SimpleDateFormat sdf = new SimpleDateFormat("HH:mm:ss");
                    sdf.setTimeZone(TimeZone.getTimeZone("GMT-0:00"));
                    mTestTimeTv.setText(sdf.format(new Date(testTime)));
                    Log.d("BackTakingPictureActivity", "handleMessage=>testTime: " + testTime);
                    if (isTestOver()) {
                        stopTest(true);

                    } else {
                        mHandler.sendEmptyMessageDelayed(MSG_UPDATE_TIME, 1000);
                    }
                    break;
            }
        };
    };

    public void startPreview(int cameraId) {
        bIfPhoto = false;
        Log.d(this, "startPreview=>id: " + cameraId + " camera: " + mCamera);
        if (mCamera == null) {
            try {
                mCamera = Camera.open(cameraId);
            } catch (RuntimeException e) {
                stopTest(false);
                return;
            }
        }
        if (mIsPreviewing) {
            stopPreview();
        }
        if ((mCamera != null) && (!mIsPreviewing)) {
            setCameraParameters();
            setPreviewDisplay(mSurfaceHolder, cameraId);
            try {
                mCamera.startPreview();
                safeToTakePicture = true;
                mIsPreviewing = true;
                new Handler().postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        onTakePictures();
                    }
                }, 1000);
            } catch (Exception e) {
                stopTest(false);
            }

        }
    }

    private void stopPreview() {
        if ((mCamera != null) && (mIsPreviewing)) {
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }
        mIsPreviewing = false;
    }

    private void closeCamera() {
        if (mCamera != null) {
            mCamera.release();
            mCamera = null;
            mIsPreviewing = false;
        }
    }

    private void setCameraParameters() {
        Camera.Parameters mParameters = mCamera.getParameters();

        mParameters.setPictureSize(640, 480);
        List<String> flashModes = mParameters.getSupportedFlashModes();
        if (flashModes != null) {
            mParameters.setFlashMode(Parameters.FLASH_MODE_AUTO);
        }
        Camera.Size size = mParameters.getPictureSize();
        List<Camera.Size> sizes = mParameters.getSupportedPreviewSizes();
        Camera.Size optimalSize = getOptimalPreviewSize(sizes, ((double) size.width / (double) size.height));
        if (optimalSize != null) {
            mParameters.setPreviewSize(optimalSize.width, optimalSize.height);
        }
        mCamera.setParameters(mParameters);
    }

    private Camera.Size getOptimalPreviewSize(List<Camera.Size> sizes, double targetRatio) {
        Camera.Size result = null;
        if (result != null) {
            Camera.Size size = null;
            Double d = Double.MAX_VALUE;
            Display display = getWindowManager().getDefaultDisplay();
            int min = Math.min(display.getHeight(), display.getWidth());
            if (min <= 0) {
                min = display.getHeight();
            }
            for (int i = 0; i < sizes.size(); i++) {
                size = sizes.get(i);
                if ((Math.abs(size.width / size.height - targetRatio) <= 0.05) && (Math.abs(size.height - i) < d)) {
                    result = size;
                    d = (double) Math.abs(size.height - i);
                }

            }
        }

        return result;
    }

    public int getDisplayOrientation(int degrees, int cameraId) {
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(cameraId, info);
        Log.d(this, "getDisplayOrientation=>degrees: " + degrees + " camera: " + info.orientation);
        if (info.facing == 1) {
            return ((info.orientation + degrees + 180) % 0x168);
        } else {
            int result = ((info.orientation - degrees) + 0x168) % 0x168;
            return result;
        }
    }

    private void setPreviewDisplay(SurfaceHolder holder, int cameraId) {
        try {
            mCamera.setPreviewDisplay(holder);
            mDisplayRotation = getDisplayRotation(this);
            mDisplayOrientation = getDisplayOrientation(mDisplayRotation, cameraId);
            mCamera.setDisplayOrientation(mDisplayOrientation);
            return;
        } catch (IOException e) {
            closeCamera();
            e.printStackTrace();
        }
    }

    public int getDisplayRotation(Activity activity) {
        int rotation = activity.getWindowManager().getDefaultDisplay().getRotation();
        switch (rotation) {
            case 0:
                return 0;

            case 1:
                return 90;

            case 2:
                return 180;

            case 3:
                return 270;

            default:
                return 0;
        }

    }

    private void onTakePictures() {
        currentPicturePath = System.currentTimeMillis() + "test.jpg";
        if ((mCamera != null) && (mIsPreviewing)) {
            if (!bIfPhoto) {
                bIfPhoto = true;
                mCamera.autoFocus(autofocusCallback);
            }
        }
    }

    private Camera.PictureCallback jpegCallback = new Camera.PictureCallback() {

        public void onPictureTaken(byte[] _data, Camera _camera) {
            mCamera.startPreview();
            Bitmap bm = BitmapFactory.decodeByteArray(_data, 0x0, _data.length);
            File myCaptureFile = new File(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM),
                    currentPicturePath);
            Log.d(this, "onPictureTake=>path: " + myCaptureFile.getAbsolutePath());
            try {
                if (!myCaptureFile.exists()) {
                    myCaptureFile.createNewFile();
                }
                BufferedOutputStream bos = new BufferedOutputStream(new FileOutputStream(myCaptureFile));
                bm.compress(Bitmap.CompressFormat.JPEG, 80, bos);
                bm.recycle();
                bos.flush();
                bos.close();
            } catch (Exception e) {
                Log.d(this, "onPictureTaken=>error: ", e);
            } finally {
                stopPreview();
                startPreview(mCameraId);
            }
        }
    };
    private Camera.AutoFocusCallback autofocusCallback = new Camera.AutoFocusCallback() {

        public void onAutoFocus(boolean success, Camera camera) {
            if (safeToTakePicture) {
                mCamera.takePicture(null, null, jpegCallback);
                safeToTakePicture = false;
            }
        }
    };
    private Camera.ShutterCallback shutterCallback = new Camera.ShutterCallback() {

        public void onShutter() {
        }
    };
    private Camera.PictureCallback rawCallback = new Camera.PictureCallback() {

        public void onPictureTaken(byte[] _data, Camera _camera) {
        }
    };

    public void surfaceCreated(SurfaceHolder holder) {
    }

    public void surfaceChanged(SurfaceHolder surfaceholder, int format, int w, int h) {
        mSurfaceHolder = surfaceholder;
        if (mCamera == null) {
            return;
        }
        if ((mIsPreviewing) && (surfaceholder.isCreating())) {
            setPreviewDisplay(surfaceholder, mCameraId);
            return;
        }
        startPreview(mCameraId);
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
    }


    private void getCameraId() {
        int mNumberOfCameras = Camera.getNumberOfCameras();
        CameraInfo[] mInfo = new CameraInfo[mNumberOfCameras];
        for (int i = 0; i < mNumberOfCameras; i++) {
            mInfo[i] = new CameraInfo();
            Camera.getCameraInfo(i, mInfo[i]);
        }

        // get the first (smallest) back and first front camera id
        for (int i = 0; i < mNumberOfCameras; i++) {
            if (mBackCameraId == -1 && mInfo[i].facing == CameraInfo.CAMERA_FACING_BACK) {
                mBackCameraId = i;
            } else if (mFrontCameraId == -1 && mInfo[i].facing == CameraInfo.CAMERA_FACING_FRONT) {
                mFrontCameraId = i;
            }
        }

        Log.d(this, "getCameraId=>back: " + mBackCameraId + " front: " + mFrontCameraId);
    }

}
