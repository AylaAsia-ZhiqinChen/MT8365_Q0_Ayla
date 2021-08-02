package com.android.factorytest.sensor;

import android.app.PendingIntent;
import android.content.ComponentName;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.nfc.NfcAdapter;
import android.nfc.tech.IsoDep;
import android.nfc.tech.NfcF;
import android.nfc.tech.NfcV;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;

import com.android.factorytest.BaseActivity;
import com.android.factorytest.Log;
import com.android.factorytest.R;
//yuntian longyao add
//Description:nfc测试多加一层判断（解决白卡测试不过）
import android.content.BroadcastReceiver;
import android.content.Context;
//yuntian longyao end

public class NFCSensorTest extends BaseActivity {

    private TextView mNfcTestTipTv;
    private NfcAdapter mNfcAdapter;
    private PendingIntent mPendingIntent;

    private boolean mLastNfcEnabled;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        setContentView(R.layout.activity_nfc_test);

        super.onCreate(savedInstanceState);
        mNfcAdapter = NfcAdapter.getDefaultAdapter(this);
        mPendingIntent = PendingIntent.getActivity(this, 0,
                new Intent(this, getClass()).addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP), 0);

        mNfcTestTipTv = (TextView) findViewById(R.id.nfc_test_tip);

        if (mNfcAdapter != null) {
            mLastNfcEnabled = mNfcAdapter.isEnabled();
            if (!mLastNfcEnabled) {
                mNfcAdapter.enable();
            }
        } else {
            Toast.makeText(this, R.string.nfc_adapter_is_null, Toast.LENGTH_SHORT).show();
        }
        onNewIntent(getIntent());
        //yuntian longyao add
        //Description:nfc测试多加一层判断（解决白卡测试不过）
        IntentFilter filter = new IntentFilter();
        filter.addAction(YT_NFC_SUCCESS_ACTION);
        registerReceiver(mNfcReceiver, filter);
        //yuntian longyao end
    }

    @Override
    protected void onNewIntent(Intent intent) {
    //    super.onNewIntent(intent);
        if (intent != null) {
            String action = intent.getAction();
            if (NfcAdapter.ACTION_TECH_DISCOVERED.equals(action)
                    || NfcAdapter.ACTION_TAG_DISCOVERED.equals(action)) {
                mNfcTestTipTv.setText(R.string.nfc_test_pass_tip);
                mNfcTestTipTv.setTextColor(getColor(R.color.green));
                setPassButtonEnabled(true);
            }
        }
    }

    //yuntian longyao add
    //Description:nfc测试多加一层判断（解决白卡测试不过）
    private static final String YT_NFC_SUCCESS_ACTION = "com.android.nfc.action.Success";
    BroadcastReceiver mNfcReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (YT_NFC_SUCCESS_ACTION.equals(intent.getAction())) {
                mNfcTestTipTv.setText(R.string.nfc_test_pass_tip);
                mNfcTestTipTv.setTextColor(getColor(R.color.green));
                setPassButtonEnabled(true);
                //Log.i("NFCSensorTest", "nfc success");
            }
        }
    };
    //yuntian longyao end

    @Override
    protected void onResume() {
        super.onResume();
        if (mNfcAdapter != null) {
            try {
                mNfcAdapter.enableForegroundDispatch(this, mPendingIntent,
                        new IntentFilter[] { new IntentFilter(NfcAdapter.ACTION_TECH_DISCOVERED, "*/*") },
                        new String[][] { { IsoDep.class.getName() }, { NfcV.class.getName() },
                                { NfcF.class.getName() }, });
            } catch (IntentFilter.MalformedMimeTypeException e) {
                Log.e(this, "onResume=>error: ", e);
            }
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mNfcAdapter != null) {
            mNfcAdapter.disableForegroundDispatch(this);
            if (!mLastNfcEnabled) {
                mNfcAdapter.disable();
            }
        }
    }

    @Override
    protected void onDestroy() {
        if (!mLastNfcEnabled) {
            if (mNfcAdapter != null) {
                mNfcAdapter.disable();
            }
        }
        super.onDestroy();
        //yuntian longyao add
        //Description:nfc测试多加一层判断（解决白卡测试不过）
        unregisterReceiver(mNfcReceiver);
        //yuntian longyao add
    }

    private boolean setNFCSensorTestDisabled() {
        ComponentName cn = new ComponentName(getPackageName(), this.getClass().getName());
        PackageManager pm = getPackageManager();
        pm.setComponentEnabledSetting(cn, PackageManager.COMPONENT_ENABLED_STATE_DISABLED, PackageManager.DONT_KILL_APP);
        return (pm.getComponentEnabledSetting(cn) == PackageManager.COMPONENT_ENABLED_STATE_DISABLED);
    }
}
