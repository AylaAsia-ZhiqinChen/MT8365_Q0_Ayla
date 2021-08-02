package com.android.factorytest.wireless;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

import java.text.DecimalFormat;

/**
 * 收音机测试
 */
public class FmRadioTest extends BaseActivity {

    private TextView mFmradioStationTv;
    private Button mPlayOrStopBt;

    private int mFmradioStation;
    private boolean mIsPlaying;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_fm_radio_test);

        super.onCreate(savedInstanceState);

        mIsPlaying = false;
        mFmradioStation = getResources().getInteger(R.integer.fmradio_test_station);

        mFmradioStationTv = (TextView) findViewById(R.id.fm_test_station);
        mPlayOrStopBt = (Button) findViewById(R.id.play_or_stop);

        mPlayOrStopBt.setOnClickListener(mClickListener);

        DecimalFormat df = new DecimalFormat("#.0");
        mFmradioStationTv.setText(getString(R.string.fmradio_test_station_tip,
                "FM " + df.format(mFmradioStation / 10.0) + " MHz"));
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mIsPlaying) {
            stopFMRadio();
        }
    }

    private boolean startFMRadio() {
        try {
            Intent intent = getPackageManager().getLaunchIntentForPackage(
                    "com.android.fmradio");
            intent.putExtra("factory_test", true);
            intent.putExtra("action", "play");
            intent.putExtra("station", getResources().getInteger(R.integer.fmradio_test_station));
            startActivity(intent);
            setPassButtonEnabled(true);
            return true;
        } catch (Exception e) {
            Log.e(this, "startFMRadio=>error: ", e);
            Toast.makeText(this, R.string.play_fmradio_fail, Toast.LENGTH_SHORT).show();
        }
        return false;
    }

    private boolean stopFMRadio() {
        try {
            Intent intent = getPackageManager().getLaunchIntentForPackage(
                    "com.android.fmradio");
            intent.putExtra("factory_test", true);
            intent.putExtra("action", "stop");
            intent.putExtra("station", getResources().getInteger(R.integer.fmradio_test_station));
            startActivity(intent);
            return true;
        } catch (Exception e) {
            Log.e(this, "startFMRadio=>error: ", e);
            Toast.makeText(this, R.string.stop_fmradio_fail, Toast.LENGTH_SHORT).show();
        }
        return false;
    }

    private View.OnClickListener mClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            if (mIsPlaying) {
                if (stopFMRadio()) {
                    mIsPlaying = false;
                    mFmradioStationTv.setText(R.string.play_fmradio_station);
                }
            } else {
                if (startFMRadio()) {
                    mIsPlaying = true;
                    mFmradioStationTv.setText(R.string.stop_fmradio_station);
                }
            }
        }
    };
}
