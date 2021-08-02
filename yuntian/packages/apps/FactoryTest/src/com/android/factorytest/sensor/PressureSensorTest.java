package com.android.factorytest.sensor;

import android.content.pm.ActivityInfo;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;

import java.text.DecimalFormat;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

public class PressureSensorTest extends BaseActivity {

    private TextView mAirPressureTv;
    private SensorManager mSensorManager;
    private Sensor mPressureSensor;
    private DecimalFormat mDecimalFormat;

	private float mAirPressure;
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_pressure_test);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        super.onCreate(savedInstanceState);

		mAirPressure = Float.MIN_VALUE;
		mDecimalFormat = new DecimalFormat("#.00");
		
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mPressureSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_PRESSURE);

        mAirPressureTv = (TextView) findViewById(R.id.air_pressure);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mPressureSensor != null) {
            mSensorManager.registerListener(mListener, mPressureSensor, SensorManager.SENSOR_DELAY_GAME);
            setPassButtonEnabled(true);
        } else {
            Toast.makeText(this, R.string.obtain_pressure_sensor_fail, Toast.LENGTH_SHORT).show();
        }
        
        if (Float.compare(Float.MIN_VALUE, mAirPressure) == 0) {
			mAirPressureTv.setText("-- hPa");
		} else {
			mAirPressureTv.setText(mDecimalFormat.format(mAirPressure) + " hPa");
		}
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mPressureSensor != null) {
            mSensorManager.unregisterListener(mListener);
        }
    }

    private SensorEventListener mListener = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent sensorEvent) {
			if (sensorEvent.values != null && sensorEvent.values.length >= 1) {
				float x = sensorEvent.values[SensorManager.DATA_X];
				Log.d(this, "onSensorChanged=>x: " + x);
				mAirPressure = x;
				mAirPressureTv.setText(mDecimalFormat.format(mAirPressure) + " hPa");
			}
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int i) {

        }
    };
}
