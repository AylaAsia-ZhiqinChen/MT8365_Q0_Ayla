package com.android.factorytest.camera;

import android.app.Activity;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.hardware.Camera;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.util.List;

//yuntian longyao add
//Description:相机测试改为调用系统相机
import android.content.Intent;
import android.content.res.Resources;
//yuntian longyao end
import android.view.MotionEvent;
import android.view.ViewGroup;
import android.os.Handler;
import android.os.Message;
import android.media.SoundPool;
import android.graphics.Point;
import android.util.DisplayMetrics;
import android.media.AudioManager;
import android.os.Build;
import java.util.ArrayList;
import com.android.factorytest.CustomToast;

public class BackCameraTest extends BaseActivity implements SurfaceHolder.Callback, View.OnTouchListener {
    private static final String TAG = "BackCameraTest";
    private SurfaceView mSurfaceView;
    private Camera mCamera;
    private SurfaceHolder mHolder;
    private TextView mFlashTestTip;

    private int mCameraId;
    private int mScreenOrientation;
    private boolean mHasFlash;

    private FocusCirceView focusCirceView;
    private SensorControler mSensorControler;
    private int mFocusSoundId;
    private SoundPool mSoundPool;
    private boolean mFocusSoundPrepared;

    private void initView() {
        focusCirceView = new FocusCirceView(BackCameraTest.this);
        mSurfaceView.setOnTouchListener(BackCameraTest.this);
        mSensorControler = SensorControler.getInstance(BackCameraTest.this);
        DisplayMetrics mDisplayMetrics = getApplicationContext().getResources().getDisplayMetrics();
        int mScreenWidth = mDisplayMetrics.widthPixels;
        int mScreenHeight = mDisplayMetrics.heightPixels;
        mSensorControler.setCameraFocusListener(new SensorControler.CameraFocusListener() {
            @Override
            public void onFocus() {
                Point point = new Point(mScreenWidth / 2, mScreenHeight * 2 / 5);
                onCameraFocus(point);
            }
        });
    }

    /**
     * 相机对焦  默认不需要延时
     *
     * @param point
     */
    private void onCameraFocus(final Point point) {
        onCameraFocus(point, false);
    }

    /**
     * 相机对焦
     *
     * @param point
     * @param needDelay 是否需要延时
     */
    public void onCameraFocus(final Point point, boolean needDelay) {
        long delayDuration = needDelay ? 300 : 0;

        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                if (!mSensorControler.isFocusLocked()) {
                    if (onFocus(point, autoFocusCallback)) {
                        mSensorControler.lockFocus();
                        float x = point.x;
                        float y = point.y;
                        if (focusCirceView != null) {
                            //清除画布,并移除视图
                            focusCirceView.deleteCanvas();
                            ViewGroup parent = ((ViewGroup) focusCirceView.getParent());
                            if (parent != null) {
                                parent.removeView(focusCirceView);
                            }
                            focusCirceView.myViewScaleAnimation(focusCirceView, x, y);//动画效果
                            focusCirceView.setPoint(x, y);
                            addContentView(focusCirceView, new
                                    ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                                    ViewGroup.LayoutParams.MATCH_PARENT)); //添加视图FocusCirceView
                            mHandler.sendEmptyMessage(MSG_DELETE_CANVAS);
                        }
                    }
                }
            }
        }, delayDuration);
    }

    /**
     * 手动聚焦
     *
     * @param point 触屏坐标
     */
    protected boolean onFocus(Point point, Camera.AutoFocusCallback callback) {
        if (mCamera == null) {
            return false;
        }
        Camera.Parameters parameters = null;
        try {
            parameters = mCamera.getParameters();
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        //不支持设置自定义聚焦，则使用自动聚焦，返回
        if (Build.VERSION.SDK_INT >= 14) {

            if (parameters.getMaxNumFocusAreas() <= 0) {
                return focus(callback);
            }

            Log.i(TAG, "onCameraFocus:" + point.x + "," + point.y);

            List<Camera.Area> areas = new ArrayList<Camera.Area>();
            int left = point.x - 300;
            int top = point.y - 300;
            int right = point.x + 300;
            int bottom = point.y + 300;
            left = left < -1000 ? -1000 : left;
            top = top < -1000 ? -1000 : top;
            right = right > 1000 ? 1000 : right;
            bottom = bottom > 1000 ? 1000 : bottom;
            areas.add(new Camera.Area(new Rect(left, top, right, bottom), 100));
            parameters.setFocusAreas(areas);
            try {
                //本人使用的小米手机在设置聚焦区域的时候经常会出异常，看日志发现是框架层的字符串转int的时候出错了，
                //目测是小米修改了框架层代码导致，在此try掉，对实际聚焦效果没影响
                mCamera.setParameters(parameters);
            } catch (Exception e) {
                // TODO: handle exception
                e.printStackTrace();
                return false;
            }
        }
        return focus(callback);
    }

    private boolean focus(Camera.AutoFocusCallback callback) {
        try {
            mCamera.autoFocus(callback);
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        return true;
    }

    private final Camera.AutoFocusCallback autoFocusCallback = new Camera.AutoFocusCallback() {
        @Override
        public void onAutoFocus(boolean success, Camera camera) {
            //聚焦之后根据结果修改图片
            if (success) {
                CustomToast.showToast(BackCameraTest.this, "自动对焦成功", 1000);
            } else {
                CustomToast.showToast(BackCameraTest.this, "自动对焦失败", 1000);
            }
            //播放对焦音效
            if (mFocusSoundPrepared) {
                mSoundPool.play(mFocusSoundId, 1.0f, 0.5f, 1, 0, 1.0f);
            }
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    //1秒之后才能再次对焦
                    mSensorControler.unlockFocus();
                }
            }, 1000);
        }
    };

    private SoundPool getSoundPool() {
        if (mSoundPool == null) {
            mSoundPool = new SoundPool(5, AudioManager.STREAM_MUSIC, 0);
            mFocusSoundId = mSoundPool.load(BackCameraTest.this, R.raw.camera_focus, 1);
            mFocusSoundPrepared = false;
            mSoundPool.setOnLoadCompleteListener(new SoundPool.OnLoadCompleteListener() {
                @Override
                public void onLoadComplete(SoundPool soundPool, int sampleId, int status) {
                    mFocusSoundPrepared = true;
                }
            });
        }
        return mSoundPool;
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                List<String> focusModes = mCamera.getParameters().getSupportedFocusModes();
                if (focusModes.contains(Camera.Parameters.FOCUS_MODE_AUTO)) {
                    mSensorControler.lockFocus();
                    float x = event.getX();
                    float y = event.getY();
                    if (focusCirceView != null) {
                        //清除画布,并移除视图
                        focusCirceView.deleteCanvas();
                        ViewGroup parent = ((ViewGroup) focusCirceView.getParent());
                        if (parent != null) {
                            parent.removeView(focusCirceView);
                        }
                        focusCirceView.myViewScaleAnimation(focusCirceView, x, y);//动画效果
                        focusCirceView.setPoint(x, y);
                        mHandler.removeMessages(MSG_DELETE_CANVAS);
                        mHandler.removeMessages(MSG_DISPLAY_FOCUS_FRAME);
                        mHandler.sendEmptyMessageDelayed(MSG_DISPLAY_FOCUS_FRAME, 500);
                    }
                }
                break;
            case MotionEvent.ACTION_MOVE:
                break;
            case MotionEvent.ACTION_UP:
                mHandler.sendEmptyMessageDelayed(MSG_DELETE_CANVAS, 500);
                break;
        }
        return true;
    }

    private static final int MSG_DELETE_CANVAS = 0;
    private static final int MSG_DISPLAY_FOCUS_FRAME = 1;
    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_DELETE_CANVAS:
                    if (focusCirceView != null) {
                        //清除画布,并移除视图
                        focusCirceView.deleteCanvas();
                        ViewGroup parent = ((ViewGroup) focusCirceView.getParent());
                        if (parent != null) {
                            parent.removeView(focusCirceView);
                        }
                    }
                    mSensorControler.restFoucs();
                    mSensorControler.unlockFocus();
                    break;
                case MSG_DISPLAY_FOCUS_FRAME:
                    addContentView(focusCirceView, new
                            ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                            ViewGroup.LayoutParams.MATCH_PARENT)); //添加视图FocusCirceView
                    if (mCamera != null) {
                        mCamera.cancelAutoFocus();
                        mCamera.getParameters().setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
                        mCamera.autoFocus(new Camera.AutoFocusCallback() {
                            @Override
                            public void onAutoFocus(boolean success, Camera camera) {
                                if (success) {
                                    CustomToast.showToast(BackCameraTest.this, "手动对焦成功", 1000);
                                } else {
                                    CustomToast.showToast(BackCameraTest.this, "手动对焦失败", 1000);
                                }
                                //播放对焦音效
                                if (mFocusSoundPrepared) {
                                    mSoundPool.play(mFocusSoundId, 1.0f, 0.5f, 1, 0, 1.0f);
                                }
                            }
                        });
                    }
                    break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_back_camera_test);

        super.onCreate(savedInstanceState);
        Resources res = getResources();
        mCameraId = -1;
        mScreenOrientation = 0;
        mHasFlash = false;

        mSurfaceView = (SurfaceView) findViewById(R.id.camera_surfaceview);
        mFlashTestTip = (TextView) findViewById(R.id.flash_tip);
        //yuntian longyao add
        //Description:相机测试改为调用系统相机
        if (res.getBoolean(R.bool.invoking_system_camera)) {
            Intent intent = new Intent();
            intent.setClassName("com.mediatek.camera", "com.mediatek.camera.CameraLauncher");
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(intent);
            mSurfaceView.setVisibility(View.GONE);
            mFlashTestTip.setVisibility(View.GONE);
            setPassButtonEnabled(true);
        }
        //yuntian longyao end
        initView();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mHolder = mSurfaceView.getHolder();
        mHolder.addCallback(this);
        mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
    }

    @Override
    protected void onPause() {
        super.onPause();
        stopCameraPreview();
    }

    @Override
    public void onStart() {
        super.onStart();
        mSensorControler.onStart();
        mSoundPool = getSoundPool();
    }

    @Override
    public void onStop() {
        super.onStop();
        mSensorControler.onStop();
        mSoundPool.release();
        mSoundPool = null;
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        startCameraPreview();
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }

    private void startCameraPreview() {
        if (mCamera != null) {
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }

        mCameraId = getBackCameraId();
        if (mCameraId >= 0) {
            try {
                // 获取前摄像头Camera对象
                mCamera = Camera.open(mCameraId);
                // 设置前摄像头的显示方向
                setDisPlayOrientation();
                // 设置相机参数
                setCameraParameters();
                // 设置显示预览界面
                mCamera.setPreviewDisplay(mHolder);
                // 开启预览
                mCamera.startPreview();
                setPassButtonEnabled(true);
                mSensorControler.restFoucs();
                mSensorControler.unlockFocus();
            } catch (Exception e) {
                Log.e(this, "startCameraPreview=>error: ", e);
                Toast.makeText(this, R.string.camera_open_fail, Toast.LENGTH_SHORT).show();
                stopCameraPreview();
            }
        }
    }

    private void stopCameraPreview() {
        if (mCamera != null) {
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }
    }

    private int getBackCameraId() {
        int cameraId = -1;
        CameraManager manager = (CameraManager) getSystemService(CAMERA_SERVICE);
        try {
            String[] ids = manager.getCameraIdList();
            for (String id : ids) {
                CameraCharacteristics c = manager.getCameraCharacteristics(id);
                Boolean flashAvailable = c.get(CameraCharacteristics.FLASH_INFO_AVAILABLE);
                Integer lensFacing = c.get(CameraCharacteristics.LENS_FACING);
                Log.d(this, "getBackCameraId=>id: " + id + " flash: " + flashAvailable);
                if (lensFacing != null && lensFacing == CameraCharacteristics.LENS_FACING_BACK) {
                    cameraId = Integer.parseInt(id);
                    mHasFlash = flashAvailable.booleanValue();
                    break;
                }
            }
        } catch (Exception e) {
            Log.e(this, "getFrontCameraId=>error: ", e);
            Toast.makeText(this, R.string.back_camera_not_found, Toast.LENGTH_SHORT).show();
        }
        return cameraId;
    }

    private void setDisPlayOrientation() {
        if (mCamera != null && mCameraId != -1) {
            Camera.CameraInfo info = new Camera.CameraInfo();
            Camera.getCameraInfo(mCameraId, info);
            int orientation = (info.orientation - mScreenOrientation + 360) % 360 + getResources().getInteger(R.integer.camera_test_orientation_degree);
            mCamera.setDisplayOrientation(orientation);
        }
    }

    private void setCameraParameters() {
        if (mCamera != null && mCameraId != -1) {
            Camera.Parameters parameters = mCamera.getParameters();
            parameters.setPictureFormat(PixelFormat.JPEG);
            //parameters.set("orientation", "portrait");
            Camera.Size previewSize = getPreviewSize();
            parameters.setPreviewSize(previewSize.width, previewSize.height);
/*            List<String> focusModes = parameters.getSupportedFocusModes();
            if (focusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
                mCamera.cancelAutoFocus();
                parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
            }*/
            if (mHasFlash) {
                parameters.setFlashMode(Camera.Parameters.FLASH_MODE_TORCH);
            }
            mFlashTestTip.setVisibility(mHasFlash ? View.VISIBLE : View.GONE);
            mCamera.setParameters(parameters);
        }
    }

    private Camera.Size getPreviewSize() {
        Rect frame = mHolder.getSurfaceFrame();
        int surfaceWidth = frame.right - frame.left;
        int surfaceHeight = frame.bottom - frame.top;
        Log.d(this, "getPreviewSize=>surfaceW:" + surfaceWidth + " sufaceH: " + surfaceHeight);
        List<Camera.Size> sizes = mCamera.getParameters().getSupportedPreviewSizes();
        Camera.Size previewSize = sizes.get(0);
        for (int i = 0; i < sizes.size(); i++) {
            Log.d(this, "getPreviewSize=>size: " + sizes.get(i).width + "x" + sizes.get(i).height);
            float scale = (float) sizes.get(i).height / sizes.get(i).width;
            Log.d(this, "getPreviewSize=>scale: " + scale);
            if ((scale > 0.5 && scale < 0.7)
                    && (sizes.get(i).width - surfaceHeight < 100 || sizes.get(i).width - surfaceHeight > 100)) {
                if (previewSize.width < sizes.get(i).width) {
                    previewSize = sizes.get(i);
                }
            }
        }
        Log.d(this, "getPreviewSize=>width: " + previewSize.width + " height: " + previewSize.height);
        return previewSize;
    }
}
