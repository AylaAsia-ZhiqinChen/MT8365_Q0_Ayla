package com.android.factorytest.sensor;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.R;

/**
 * 光感传感器测试
 */
public class LightSensorTest extends BaseActivity {

    private TextView mCurrentValueTv;
    private ProgressBar mCurrentValuePb;
    private SensorManager mSensorManager;
    private Sensor mLightSensor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_light_test);

        super.onCreate(savedInstanceState);

        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mLightSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_LIGHT);

        mCurrentValueTv = (TextView) findViewById(R.id.current_light_value);
        mCurrentValuePb = (ProgressBar) findViewById(R.id.current_light_progress);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mLightSensor != null) {
            //mCurrentValuePb.setMax((int) mLightSensor.getMaximumRange());
            mCurrentValuePb.setMax(1250);
            mSensorManager.registerListener(mListener, mLightSensor,SensorManager.SENSOR_DELAY_GAME);
            setPassButtonEnabled(true);
        } else {
            Toast.makeText(this, R.string.light_sensor_not_support, Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mLightSensor != null) {
            mSensorManager.unregisterListener(mListener);
        }
    }

    private SensorEventListener mListener = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent sensorEvent) {
            if (sensorEvent.values != null && sensorEvent.values.length > 0) {
                float value = sensorEvent.values[SensorManager.DATA_X];
                mCurrentValueTv.setText((int) value + "");
                mCurrentValuePb.setProgress((int) value);
            }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int i) {

        }
    };
}
