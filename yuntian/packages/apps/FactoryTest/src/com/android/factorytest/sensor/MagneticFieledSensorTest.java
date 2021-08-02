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
import com.android.factorytest.view.CompassView;
import com.android.factorytest.Log;
import com.android.factorytest.R;

public class MagneticFieledSensorTest extends BaseActivity {

    private TextView mXAxisTv;
    private TextView mYAxisTv;
    private TextView mZAxisTv;
    private CompassView mCompassView;
    private SensorManager mSensorManager;
    private Sensor mAccelerometerSensor;
    private Sensor mMagneticSensor;
    //private Sensor mOrientationSensor;

	private float[] accelerometerValues = new float[3]; 
	private float[] magneticFieldValues = new float[3]; 
	private float[] values = new float[3]; 
	private float[] rotate = new float[9];
    private boolean mShowZAxisValue;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_magnetic_fieled_test);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        super.onCreate(savedInstanceState);

        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mMagneticSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD);
        mAccelerometerSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        //mOrientationSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION);

        mXAxisTv = (TextView) findViewById(R.id.gravity_x_value);
        mYAxisTv = (TextView) findViewById(R.id.gravity_y_value);
        mZAxisTv = (TextView) findViewById(R.id.gravity_z_value);
        mCompassView = (CompassView) findViewById(R.id.compass_view);

    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mMagneticSensor != null) {
            mSensorManager.registerListener(mListener, mMagneticSensor, SensorManager.SENSOR_DELAY_GAME);
            mSensorManager.registerListener(mListener, mAccelerometerSensor, SensorManager.SENSOR_DELAY_GAME);
            //mSensorManager.registerListener(mListener, mOrientationSensor, SensorManager.SENSOR_DELAY_GAME);
            setPassButtonEnabled(true);
        } else {
            Toast.makeText(this, R.string.obtain_magnetic_fieled_sensor_fail, Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mMagneticSensor != null) {
            mSensorManager.unregisterListener(mListener);
        }
    }

    private SensorEventListener mListener = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent sensorEvent) {
			if (sensorEvent.sensor.getType() == Sensor.TYPE_ACCELEROMETER) { 
				accelerometerValues = sensorEvent.values; 
			} 
			
			if (sensorEvent.sensor.getType() == Sensor.TYPE_MAGNETIC_FIELD) {
				magneticFieldValues = sensorEvent.values; 
				if (sensorEvent.values != null && sensorEvent.values.length >= 3) {
					float x = sensorEvent.values[SensorManager.DATA_X];
					float y = sensorEvent.values[SensorManager.DATA_Y];
					float z = sensorEvent.values[SensorManager.DATA_Z];
					//Log.d(this, "onSensorChanged=>x: " + x + " y: " + y + " z: " + z);
					mXAxisTv.setText(x + "");
					mYAxisTv.setText(y + "");
					mZAxisTv.setText(z + "");
				}
			} 
			
			/*
			if (sensorEvent.sensor.getType() == Sensor.TYPE_ORIENTATION) {
				if (sensorEvent.values != null && sensorEvent.values.length >= 1) {
					float x = sensorEvent.values[SensorManager.DATA_X];
					Log.d(this, "onSensorChanged=>x: " + x);
				}
			}
			*/
			
			SensorManager.getRotationMatrix(rotate, null, accelerometerValues, magneticFieldValues); 
			SensorManager.getOrientation(rotate, values); //经过SensorManager.getOrientation(rotate, values);得到的values值为弧度 
			//转换为角度 
			values[0]=(float)Math.toDegrees(values[0]); 
			values[0] = (values[0] + 360) % 360;
			mCompassView.setAngle(values[0]);
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int i) {

        }
    };
}
