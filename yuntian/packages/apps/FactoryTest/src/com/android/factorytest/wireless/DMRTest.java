package com.android.factorytest.wireless;

import android.app.Activity;
import android.content.res.Resources;
import android.os.Bundle;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.R;

import java.io.IOException;

import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.content.ComponentName;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

public class DMRTest extends BaseActivity {
    private static final String TAG = "DMRTest";
    private TextView mTextView;
    private final int RESULT_ID = 1001;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_dmr_test);
        super.onCreate(savedInstanceState);
        mTextView = findViewById(R.id.dmr_test_result);
        openDmrApp();
    }

    private void openDmrApp() {
        Intent intent = new Intent();
        ComponentName cName = new ComponentName("com.yt.intercom", "com.yt.intercom.IComActivity");
        intent.setComponent(cName);
        try {
            Bundle bundle = new Bundle();
            bundle.putString("arge1", "dmr_test");
            intent.putExtras(bundle);
            startActivityForResult(intent, 0);
        } catch (ActivityNotFoundException ex) {
            Toast.makeText(this, "no applications", Toast.LENGTH_LONG).show();
            setPassButtonEnabled(false);
        }
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Resources res = getResources();
        switch (resultCode) {
            case RESULT_ID:
                Bundle b = data.getExtras();
                String str = b.getString("dmr_init_state");
                Log.i("yoyo", "dmr_init_state =" + str);
                if (str != null && str.equals("succeed")) {
                    setPassButtonEnabled(true);
                    mTextView.setText(res.getString(R.string.test_success));
                    setFailButtonEnabled(false);
                } else {
                    setPassButtonEnabled(false);
                    mTextView.setText(res.getString(R.string.test_fail));
                    setFailButtonEnabled(true);
                }
                break;
            default:
                break;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
