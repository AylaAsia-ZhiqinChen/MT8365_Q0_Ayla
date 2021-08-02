package com.mediatek.bluetooth;

import android.bluetooth.BluetoothAdapter;
import android.content.DialogInterface;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.view.View;
import android.widget.Toast;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.TextView;
import android.widget.ImageView;
import java.util.ArrayList;
import java.util.List;
import com.mediatek.bluetooth.pbapclient.BluetoothPbapClientManager;
import com.mediatek.bluetooth.pbapclient.BluetoothPabapClientCallback;
import com.mediatek.bluetooth.pbapclient.BluetoothPbapClientConstants;
import android.bluetooth.BluetoothPbapClient;
import com.android.vcard.VCardEntry;
import com.android.vcard.VCardEntry.PhoneData;
import com.android.internal.app.AlertActivity;
import com.android.internal.app.AlertController;
import android.bluetooth.BluetoothDevice;

public class VcardEntryActivity extends AlertActivity implements
    DialogInterface.OnClickListener{


    private static final String TAG = "VcardEntryActivity";
    public static final String EXTRA_TARGET_FOLDER = "extra_target_folder";
    public static final String DISPLAY_NAME = "display_name";
    public static final String PHONE_NUMBER = "phone_number";
    private BluetoothPbapClientManager mManager;

    private TextView mTextView;
    private String mTargetFolder = "";
    public  BluetoothDevice mConnectedDevice = null;
    private String pbName;
    private String pbnumber;
    Intent bt_callout_intent = new Intent(DialFragment.ACTION_BLUETOOTH_CALLOUT);


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mTargetFolder = getIntent().getStringExtra(EXTRA_TARGET_FOLDER);

        mConnectedDevice = this.getIntent().getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
        try{
            pbName = getIntent().getStringExtra(DISPLAY_NAME);
            pbnumber = getIntent().getStringExtra(PHONE_NUMBER);
        }catch(NullPointerException e){

        }
        entryDetailDialog(pbName);
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        this.registerReceiver(btStateCahangedReceiver, intentFilter);
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        Log.d(TAG, "onDestroy");
        super.onDestroy();
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }

    private void entryDetailDialog(String titleName) {
        final AlertController.AlertParams p = mAlertParams;
        p.mTitle =/*this.getString(titleResId)*/titleName;
        p.mView = createView();
        p.mPositiveButtonText = this.getString(R.string.bt_entry_dialog_positive);
        p.mPositiveButtonListener = this;
        p.mNegativeButtonText = this.getString(R.string.bt_entry_dialog_negative);
        p.mNegativeButtonListener = this;
        setupAlert();
    }

    private View createView() {
        View view = getLayoutInflater().inflate(R.layout.bt_entry_activity, null);
        TextView mMessageView = (TextView) view.findViewById(R.id.subtitle);
        mMessageView.setText(pbnumber);
        ImageView mImageView = (ImageView) view.findViewById(R.id.image);
        mImageView.setImageResource(R.drawable.pbap_contacts_image);
        return view;
    }

    public void onClick(DialogInterface dialogInterface, int button) {
        Log.d(TAG, "onClick");
        if (button == DialogInterface.BUTTON_POSITIVE) {
            if(pbnumber != null){
                bt_callout_intent.putExtra(DialFragment.EXTRA_BLUETOOTH_CALLOUT_NAME,
                                    pbName);
                bt_callout_intent.putExtra(DialFragment.EXTRA_BLUETOOTH_CALLOUT_NUMBER,
                                    pbnumber);
                this.sendBroadcast(bt_callout_intent);
            }
        } else {
            this.finish();
        }
    }

    private final BroadcastReceiver btStateCahangedReceiver = new BroadcastReceiver(){

        @Override
        public void onReceive(Context context, Intent intent) {
            final int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,
            BluetoothAdapter.ERROR);
            if(state == BluetoothAdapter.STATE_OFF){
                VcardEntryActivity.this.finish();
            }
        }
    };
}