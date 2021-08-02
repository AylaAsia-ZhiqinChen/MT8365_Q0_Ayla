package com.android.factorytest.camera;

import android.app.Activity;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.hardware.Camera;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.os.Bundle;
import android.os.SystemProperties;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.util.List;

public class IrisCameraTest extends BaseActivity implements SurfaceHolder.Callback {

    private SurfaceView mSurfaceView;
    private Camera mCamera;
    private SurfaceHolder mHolder;
    private TextView mFlashTestTip;

    private int mCameraId;
    private int mScreenOrientation;
    private boolean mHasFlash;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_iris_camera_test);

        super.onCreate(savedInstanceState);

        SystemProperties.set("net.yt.redcamera.test", "1");

        mCameraId = -1;
        mScreenOrientation = 0;
        mHasFlash = false;

        mSurfaceView = (SurfaceView)findViewById(R.id.camera_surfaceview);
        mFlashTestTip = (TextView) findViewById(R.id.flash_tip);
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
    protected void onDestroy() {
        SystemProperties.set("net.yt.redcamera.test", "0");
        super.onDestroy();
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

        mCameraId = getIrisCameraId();
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

    private int getIrisCameraId() {
        int cameraId = -1;
        CameraManager manager = (CameraManager) getSystemService(CAMERA_SERVICE);
        try {
            String id = "2";
            CameraCharacteristics c = manager.getCameraCharacteristics(id);
            Boolean flashAvailable = c.get(CameraCharacteristics.FLASH_INFO_AVAILABLE);
            Log.d(this, "getIrisCameraId=>id: " + id + " flash: " + flashAvailable);
            cameraId = Integer.parseInt(id);
            mHasFlash = flashAvailable;
        } catch (Exception e) {
            Log.e(this, "getIrisCameraId=>error: ", e);
            Toast.makeText(this, R.string.iris_camera_not_found, Toast.LENGTH_SHORT).show();
        }
        return cameraId;
    }

    private void setDisPlayOrientation() {
        if (mCamera != null && mCameraId != -1) {
            Camera.CameraInfo info = new Camera.CameraInfo();
            Camera.getCameraInfo(mCameraId, info);
            int orientation = 0;
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
            List<Camera.Area> areas = parameters.getFocusAreas();
            if (areas != null && areas.size() > 0) {
                parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
            }
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
            float scale = (float)sizes.get(i).height / sizes.get(i).width;
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
