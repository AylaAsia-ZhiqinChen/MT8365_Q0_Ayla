package com.android.factorytest.sensor;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;

/**
 * 接近传感器测试
 */
public class ProximitySensorTest extends BaseActivity {

    private static final String PROXIMITY_ORIGIN_FILE_PATH = "/sys/bus/platform/drivers/als_ps/ps";
    private static final int MSG_UPDATE_PROXIMITY_ORIGIN_VALUE = 0;

    private TextView mDistanceValueTv;
    private TextView mOriginValueTv;
    private View mPassTipView;
    private SensorManager mSensorManager;
    private Sensor mProximitySensor;

    private float mDistanceValue;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_proximity_test);

        super.onCreate(savedInstanceState);

        mDistanceValue = Float.MIN_VALUE;
        mSensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        mProximitySensor = mSensorManager.getDefaultSensor(Sensor.TYPE_PROXIMITY);

        mDistanceValueTv = (TextView) findViewById(R.id.proximity_distance_value);
        mOriginValueTv = (TextView) findViewById(R.id.proximity_origin_value);
        mPassTipView = findViewById(R.id.pass_tip);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mProximitySensor != null) {
            mSensorManager.registerListener(mListener, mProximitySensor, SensorManager.SENSOR_DELAY_GAME);
            mHandler.sendEmptyMessage(MSG_UPDATE_PROXIMITY_ORIGIN_VALUE);
        } else {
            Toast.makeText(this, R.string.obtain_proximity_sensor_fail, Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mProximitySensor != null) {
            mSensorManager.unregisterListener(mListener);
            mHandler.removeMessages(MSG_UPDATE_PROXIMITY_ORIGIN_VALUE);
        }
    }

    private void updateProximityOriginValue() {
        File file = new File(PROXIMITY_ORIGIN_FILE_PATH);
        if (file.exists() && file.isFile()) {
            FileReader fr = null;
            BufferedReader br = null;
            try {
                fr = new FileReader(file);
                br = new BufferedReader(fr);
                String value = br.readLine();
                mOriginValueTv.setText(value);
            } catch (Exception e) {
                Log.e(this, "updateProximityOriginValue=>error: ", e);
            } finally {
                if (br != null) {
                    try { br.close(); } catch (Exception e) {}
                    br = null;
                }
                if (fr != null) {
                    try { fr.close(); } catch (Exception e) {}
                    fr = null;
                }
            }
        }
        mHandler.sendEmptyMessageDelayed(MSG_UPDATE_PROXIMITY_ORIGIN_VALUE, 100);
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_UPDATE_PROXIMITY_ORIGIN_VALUE:
                    updateProximityOriginValue();
                    break;
            }
        }
    };

    private SensorEventListener mListener = new SensorEventListener() {
        @Override
        public void onSensorChanged(SensorEvent sensorEvent) {
            if (sensorEvent.values != null && sensorEvent.values.length > 0) {
                float value = sensorEvent.values[SensorManager.DATA_X];
                Log.d(this, "onSensorChanged=>value: " + value);
                // 0.0表示靠近， 1.0表示离开
                if (mDistanceValue == Float.MIN_VALUE) {
                    mDistanceValue = value;
                } else {
                    if (mDistanceValue < 0.1 && value > 0.9) {
                        mPassTipView.setBackgroundColor(getColor(R.color.white));
                    } else if (mDistanceValue > 0.9 && value < 0.1) {
                        mPassTipView.setBackgroundColor(getColor(R.color.red));
                        setPassButtonEnabled(true);
                    }
                    mDistanceValue = value;
                }
                mDistanceValueTv.setText(mDistanceValue + "");
            }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int i) {

        }
    };
}
