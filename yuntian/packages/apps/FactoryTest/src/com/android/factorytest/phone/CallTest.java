package com.android.factorytest.phone;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.Uri;
import android.os.Bundle;
import android.telecom.TelecomManager;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;

/**
 * 通话测试
 */
public class CallTest extends BaseActivity {

    private TelephonyManager mTelephonyManager;
    private TelecomManager mTelecomManager;

    private String mOutGoingNumber;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_call_test);

        super.onCreate(savedInstanceState);

        mOutGoingNumber = getString(R.string.call_test_number);
        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_NEW_OUTGOING_CALL);
        registerReceiver(mOutGoingReceiver, filter);
        Intent intent = new Intent("android.intent.action.CALL_PRIVILEGED", Uri.parse("tel:" + mOutGoingNumber));
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.putExtra("factory_mode", true);
        startActivity(intent);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mOutGoingReceiver);
    }

    private BroadcastReceiver mOutGoingReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.i(CallTest.this, "onReceive=>action: " + action);
            if (Intent.ACTION_NEW_OUTGOING_CALL.equals(action)) {
                String number = intent.getStringExtra(Intent.EXTRA_PHONE_NUMBER);
                Log.i(CallTest.this, "onReceive=>number: " + number);
                if (mOutGoingNumber.equals(number)) {
                    setPassButtonEnabled(true);
//                    setTestCompleted(true);
//                    setTestPass(true);
                }
            }
        }
    };
}
