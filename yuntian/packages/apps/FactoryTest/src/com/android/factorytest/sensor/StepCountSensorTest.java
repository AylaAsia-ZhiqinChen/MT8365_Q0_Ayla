package com.android.factorytest.sensor;

import android.content.pm.ActivityInfo;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

public class StepCountSensorTest extends BaseActivity {

    private TextView mStepCountTv;
    private SensorManager mSensorManager;
    private Sensor mStepCountSensor;

	private long mOriginCount;
    private long mCurrentCount;
    private boolean mShowZAxisValue;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_step_count_test);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        super.onCreate(savedInstanceState);

		mOriginCount = -1;
        mCurrentCount = 0;
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mStepCountSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_STEP_COUNTER);

        mStepCountTv = (TextView) findViewById(R.id.step_count);
    }

    @Override
    protected void onResume() {
        super.onResume();
        mStepCountTv.setText(mCurrentCount + "");
        if (mStepCountSensor != null) {
            mSensorManager.registerListener(mListener, mStepCountSensor, SensorManager.SENSOR_DELAY_GAME);
            setPassButtonEnabled(true);
        } else {
            Toast.makeText(this, R.string.obtain_step_count_sensor_fail, Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mStepCountSensor != null) {
            mSensorManager.unregisterListener(mListener);
        }
    }

    private SensorEventListener mListener = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent sensorEvent) {
			if (sensorEvent.values != null && sensorEvent.values.length >= 1) {
				float x = sensorEvent.values[SensorManager.DATA_X];
				Log.d(this, "onSensorChanged=>x: " + x);
				if (mOriginCount == -1) {
					mOriginCount = (long)x;
				} else {
					mCurrentCount = (long)x - mOriginCount;
				}
				mStepCountTv.setText(mCurrentCount + "");
			}
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int i) {

        }
    };
}
