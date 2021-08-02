package com.android.factorytest.sensor;

import android.content.pm.ActivityInfo;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

/**
 * 加速度传感器测试
 */
public class GravitySensorTest extends BaseActivity {

    private TextView mXAxisTv;
    private TextView mYAxisTv;
    private TextView mZAxisTv;
    private View mOrientationView;
    private View mZAxisContianerView;
    private SensorManager mSensorManager;
    private Sensor mGravitySensor;

    private boolean mShowZAxisValue;
    private boolean upFlag = false;
    private boolean downFlag = false;
    private boolean leftFlag = false;
    private boolean rightFlag = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_gravity_test);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        super.onCreate(savedInstanceState);

        mShowZAxisValue = getResources().getBoolean(R.bool.enabled_show_gravity_z_axis_value);
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mGravitySensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);

        mXAxisTv = (TextView) findViewById(R.id.gravity_x_value);
        mYAxisTv = (TextView) findViewById(R.id.gravity_y_value);
        mZAxisTv = (TextView) findViewById(R.id.gravity_z_value);
        mOrientationView = findViewById(R.id.orientation_tip);
        mZAxisContianerView = findViewById(R.id.gravity_z_container);

        mZAxisContianerView.setVisibility(mShowZAxisValue ? View.VISIBLE : View.GONE);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mGravitySensor != null) {
            mSensorManager.registerListener(mListener, mGravitySensor, SensorManager.SENSOR_DELAY_GAME);
        } else {
            Toast.makeText(this, R.string.obtain_gravity_sensor_fail, Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mGravitySensor != null) {
            mSensorManager.unregisterListener(mListener);
        }
    }

    private SensorEventListener mListener = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent sensorEvent) {
            if (sensorEvent.values != null && sensorEvent.values.length >= 2) {
                float x = sensorEvent.values[SensorManager.DATA_X];
                float y = sensorEvent.values[SensorManager.DATA_Y];
                Log.d(this, "onSensorChanged=>x: " + x + " y: " + y);
                mXAxisTv.setText(x + "");
                mYAxisTv.setText(y + "");

                if (mShowZAxisValue && sensorEvent.values.length >= 3) {
                    float z = sensorEvent.values[SensorManager.DATA_Z];
                    Log.d(this, "onSensorChanged=>z: " + z);
                    mZAxisTv.setText(z + "");
                }

                if (Math.abs(x) <= Math.abs(y)) {
                    if (y < 0) {
                        mOrientationView.setBackgroundResource(R.drawable.arrow_up);
                        upFlag = true;
                    } else {
                        mOrientationView.setBackgroundResource(R.drawable.arrow_down);
                        downFlag = true;
                    }
                } else {
                    if (x < 0) {
                        mOrientationView.setBackgroundResource(R.drawable.arrow_right);
                        rightFlag = true;
                    } else {
                        mOrientationView.setBackgroundResource(R.drawable.arrow_left);
                        leftFlag = true;
                    }
                }
                if (upFlag && downFlag && rightFlag && leftFlag) {
                    setPassButtonEnabled(true);
                    setTestPass(true);
					if (isAutoTest()) {
						if (isTestPass()) {
							doAtOnceOnAutoTest();
							upFlag = false;
							downFlag = false;
							rightFlag = false;
							leftFlag = false;
						}
					}

                }
            }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int i) {

        }
    };
}
