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

public class AccelerometerSensorTest extends BaseActivity {

    private TextView mXAxisTv;
    private TextView mYAxisTv;
    private TextView mZAxisTv;
    private View mZAxisContianerView;
    private SensorManager mSensorManager;
    private Sensor mAccelerometerSensor;

    private boolean mShowZAxisValue;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_accelerometer_test);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        super.onCreate(savedInstanceState);

        mShowZAxisValue = getResources().getBoolean(R.bool.enabled_show_gravity_z_axis_value);
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mAccelerometerSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);

        mXAxisTv = (TextView) findViewById(R.id.gravity_x_value);
        mYAxisTv = (TextView) findViewById(R.id.gravity_y_value);
        mZAxisTv = (TextView) findViewById(R.id.gravity_z_value);
        mZAxisContianerView = findViewById(R.id.gravity_z_container);

        mZAxisContianerView.setVisibility(mShowZAxisValue ? View.VISIBLE : View.GONE);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mAccelerometerSensor != null) {
            mSensorManager.registerListener(mListener, mAccelerometerSensor, SensorManager.SENSOR_DELAY_GAME);
            setPassButtonEnabled(true);
        } else {
            Toast.makeText(this, R.string.obtain_accelerometer_sensor_fail, Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mAccelerometerSensor != null) {
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
            }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int i) {

        }
    };
}
