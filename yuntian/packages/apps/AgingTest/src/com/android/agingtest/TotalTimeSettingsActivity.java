package com.android.agingtest;

import android.app.ActionBar;
import android.app.Activity;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.res.Resources;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.text.TextUtils;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.view.View.OnClickListener;
import android.view.View.OnFocusChangeListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class TotalTimeSettingsActivity extends Activity implements OnClickListener, OnFocusChangeListener {

    private View mTitalTimeContainer;


    private EditText mTitalTimeEt;
    private Button mOk;
    private Button mCancel;
    private SharedPreferences mSharedPreferences;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        initActionBar();

        setContentView(R.layout.activity_settings_total_time);

        initValues();
        initViews();
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onResume() {
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        updateUI();
        super.onResume();
    }

    @Override
    protected void onPause() {
        getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        super.onPause();
    }

    @Override
    public boolean onMenuItemSelected(int featureId, MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                finish();
                break;
        }
        return super.onMenuItemSelected(featureId, item);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.ok:
                boolean result = updateSharedPreference();
                Toast.makeText(this, result ? R.string.setting_success : R.string.setting_fail, Toast.LENGTH_SHORT).show();
                finish();
                break;

            case R.id.cancel:
                finish();
                break;
        }
    }

    @Override
    public void onFocusChange(View v, boolean hasFocus) {
        if (hasFocus) {
            switch (v.getId()) {
                case R.id.total_time:
                    mTitalTimeEt.setSelection(mTitalTimeEt.getText().toString().length());
                    break;
            }
        }
    }

    private void initActionBar() {
        ActionBar actionBar = getActionBar();
        actionBar.setDisplayHomeAsUpEnabled(true);
    }

    private void initValues() {
        mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(this);
    }

    private void initViews() {
        mTitalTimeContainer = findViewById(R.id.total_time_container);
        mTitalTimeEt = (EditText) findViewById(R.id.total_time);
        mOk = (Button) findViewById(R.id.ok);
        mCancel = (Button) findViewById(R.id.cancel);
        mOk.setOnClickListener(this);
        mCancel.setOnClickListener(this);
    }

    private void updateUI() {
        Resources res = getResources();
        String time_key = "totaltime";
        mTitalTimeEt.setText(mSharedPreferences.getInt(time_key, res.getInteger(R.integer.default_total_test_time)) + "");
        mTitalTimeEt.setOnFocusChangeListener(this);

    }

    private boolean updateSharedPreference() {
        boolean result = true;
        int time = 0;
        Resources res = getResources();
        Editor editor = mSharedPreferences.edit();
        try {
            String time_key = "totaltime";
            String timeStr = mTitalTimeEt.getText().toString();
            if (TextUtils.isEmpty(timeStr)
                    || !TextUtils.isDigitsOnly(timeStr)) {
                editor.putInt(time_key,
                        res.getInteger(R.integer.default_total_test_time));
            } else {
                time = Integer.parseInt(timeStr);
                editor.putInt(time_key, time);
            }
        } catch (Exception e) {
            // TODO: handle exception
            e.printStackTrace();
            result = false;
        } finally {
            editor.commit();
        }
        return result;
    }

}
