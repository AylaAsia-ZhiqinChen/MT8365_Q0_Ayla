package com.android.factorytest.sensor;

import android.content.pm.ActivityInfo;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.ImageView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

public class GyroscopeSensorTest extends BaseActivity {

	

    private static final float NS2S = 1.0f / 1000000000.0f; 
    
    private TextView mXAxisTv;
    private TextView mYAxisTv;
    private TextView mZAxisTv;
    private ImageView mAirPlayIv;
    private SensorManager mSensorManager;
    private Sensor mGyroscopeSensor;

	private float mAngles[];
	private long mLastTimesTamp;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_gyroscope_test);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        super.onCreate(savedInstanceState);

		mLastTimesTamp = 0;
		mAngles = new float[3];
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mGyroscopeSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);

        mXAxisTv = (TextView) findViewById(R.id.gravity_x_value);
        mYAxisTv = (TextView) findViewById(R.id.gravity_y_value);
        mZAxisTv = (TextView) findViewById(R.id.gravity_z_value);
        mAirPlayIv = (ImageView) findViewById(R.id.airplay);

    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mGyroscopeSensor != null) {
            mSensorManager.registerListener(mListener, mGyroscopeSensor, SensorManager.SENSOR_DELAY_GAME);
            setPassButtonEnabled(true);
        } else {
            Toast.makeText(this, R.string.obtain_gyroscope_sensor_fail, Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mGyroscopeSensor != null) {
            mSensorManager.unregisterListener(mListener);
        }
    }

    private SensorEventListener mListener = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent sensorEvent) {
            if (sensorEvent.values != null && sensorEvent.values.length >= 2) {
                float x = sensorEvent.values[SensorManager.DATA_X];
                float y = sensorEvent.values[SensorManager.DATA_Y];
                float z = sensorEvent.values[SensorManager.DATA_Z];
                Log.d(this, "onSensorChanged=>x: " + x + " y: " + y + " z: " + z);
                mXAxisTv.setText(x + "");
                mYAxisTv.setText(y + "");
                mZAxisTv.setText(z + "");
                
                if (mLastTimesTamp != 0) {  
					final float dT = (sensorEvent.timestamp - mLastTimesTamp) * NS2S;  
				    mAngles[0] += (float) Math.toDegrees(x * dT);
					mAngles[1] += (float) Math.toDegrees(y * dT);
					mAngles[2] += (float) Math.toDegrees(z * dT);
					mAirPlayIv.setRotation(mAngles[0]);
                    mAirPlayIv.setRotationX(mAngles[1]);
                    mAirPlayIv.setRotationY(mAngles[2]);
				}
				mLastTimesTamp = sensorEvent.timestamp;  
            }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int i) {

        }
    };
}
