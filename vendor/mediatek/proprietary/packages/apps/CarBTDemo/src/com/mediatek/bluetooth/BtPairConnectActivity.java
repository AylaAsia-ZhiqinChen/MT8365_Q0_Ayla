/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

package com.mediatek.bluetooth;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Iterator;
import com.mediatek.bluetooth.R;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
//import com.mediatek.bluetooth.BluetoothProfileManager;
//import com.mediatek.bluetooth.BluetoothProfileManager.Profile;
import android.bluetooth.BluetoothProfile;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;
import android.app.AlertDialog;
import android.view.Window;
import android.view.WindowManager;
import android.view.Display;
import android.view.ViewGroup.LayoutParams;
import com.mediatek.bluetooth.common.*;
import com.mediatek.bluetooth.pbapclient.BluetoothPbapClientManager;
import com.mediatek.bluetooth.pbapclient.BluetoothPbapClientConstants;
import com.mediatek.bluetooth.util.*;
import com.android.vcard.VCardEntry;
import android.bluetooth.BluetoothA2dpSink;



public class BtPairConnectActivity extends Activity implements OnClickListener {
    private static final String TAG = "BtPairConnectActivity";
    private static final boolean DEBUG = true;

    private LocalBluetoothManager mLocalManager;

    private ListView mOperationsView;
    private ArrayList<HashMap<String, Object>> mOperationsList; 
    private SimpleAdapter mOperationsArrayAdapter;

    private String[] arr1 = {"Pair", "Connect", "Return"};
    private String[] arr2 = {"unPair", "Connect", "Return"};
    private String[] arr3 = {"unPair", "disConnect", "Return"};
    private String[] arr = arr1;

    private static String mRemoteAddr = null;
    private String deviceStatus = null;
    public static BluetoothDevice mSelectedDevice = null;
    public static CachedBluetoothDevice mDevice;

    private boolean mIsUnregisterReceiver = false;
    private boolean mIsconnect = false;

    public static boolean[] mCheckedItems ={false,false,false,false,false,false};    
    private CharSequence[] mConnectItems = {} ;
    private int mProfileState = 0;
    private AlertDialog mConnectAsDialog;

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            // TODO: put this in callback instead of receiving
            if (DEBUG)
                Log.d(TAG, "onRecieve:action->" + intent.getAction());

            if (intent.getAction().equals(BluetoothDevice.ACTION_BOND_STATE_CHANGED)) {
                handleBondChanged(intent);
            } /*else if (intent.getAction().equals(
                    BluetoothProfileManager.ACTION_PROFILE_STATE_UPDATE)) {
                Profile profilename = (Profile) intent
                        .getSerializableExtra(BluetoothProfileManager.EXTRA_PROFILE);
                if (profilename.equals(BluetoothProfileManager.Profile.HID)) {
                    handleConnectChanged(intent);
                }
            } */else if (intent.getAction().equals(BluetoothDevice.ACTION_PAIRING_REQUEST)) {
                onBluetoothPairingRequest(intent);
            }else if(intent.getAction().equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
                int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,BluetoothAdapter.STATE_OFF);
                if(state == BluetoothAdapter.STATE_OFF){
                    finish();
                } 

            }
        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (DEBUG)
            Log.d(TAG, "onCreate");

        mLocalManager = Utils.getLocalBtManager(this);
        if (mLocalManager == null) {
            Toast.makeText(this, "Bluetooth is not available", Toast.LENGTH_LONG).show();
            finish();
        }
        mSelectedDevice = (BluetoothDevice)getIntent().getParcelableExtra("SELECTED_DEVICE");
        mDevice = mLocalManager.getCachedDeviceManager().findDevice(mSelectedDevice);
        if(mDevice == null){
            Log.d(TAG,"cachedDevice is null,so finish this activity");
            finish();
            return;
        }
        if(mDevice.getDevice() != null){
            String deviceName = mDevice.getDevice().getName();
            mRemoteAddr = mDevice.getDevice().getAddress();
            Log.d(TAG,"mRemoteAddr == " + mRemoteAddr + "  deviceName== " + deviceName);
            setTitle(deviceName);
        }
        deviceStatus = getResources().getString(mDevice.getConnectedState()); 
        Log.d(TAG, "deviceStatus = " + deviceStatus);
        setContentView(R.layout.bt_pair_connect);
        
        mOperationsView = (ListView) findViewById(R.id.bt_select_devices);
        mOperationsList = new ArrayList<HashMap<String, Object>>();
        // Initialize adapters for operations
        mOperationsArrayAdapter = new SimpleAdapter(this, mOperationsList,
                R.layout.pair_connect_listitem, new String[] { "operations_name" },
                new int[] { R.id.item_oparations });
        mOperationsView.setAdapter(mOperationsArrayAdapter);
        mOperationsView.setOnItemClickListener(mOperationsClickListener);

        if (mLocalManager.getBluetoothAdapter() != null) {
            if(deviceStatus.equals(this.getResources().getString(R.string.bt_status_paired))){
                arr = arr2;
            }else if(deviceStatus.equals(this.getResources().getString(R.string.bt_status_unpair))){
                arr = arr1;
            }else{
                arr = arr3;
            }
            for(int i=0; i<3; i++){
                HashMap<String, Object> map = new HashMap<String, Object>();
                String name = "";
                if (arr[i].equals("unPair")) {
                    name = this.getString(R.string.dispair_bt_string);
                } else if (arr[i].equals("Pair")) {
                    name = this.getString(R.string.trypair_bt_string);
                } else if (arr[i].equals("disConnect")) {
                    name = this.getString(R.string.disconnect_bt_string);
                } else if (arr[i].equals("Connect")) {
                    for(int j=0;j<BtPairConnectActivity.mCheckedItems.length;j++){
                        BtPairConnectActivity.mCheckedItems[j]=false;
                    }
                    name = this.getString(R.string.connect_bt_string);
                } else if (arr[i].equals("Return")) {
                    name = this.getString(R.string.bt_pairconnect_return);
                }
                map.put("operations_name", name);
                mOperationsList.add(map);
            }
            mOperationsArrayAdapter.notifyDataSetChanged();
        }	
        IntentFilter intentFilter = new IntentFilter();
        //intentFilter.addAction(BluetoothProfileManager.ACTION_PROFILE_STATE_UPDATE);
        intentFilter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        intentFilter.addAction(BluetoothDevice.ACTION_PAIRING_REQUEST);
        intentFilter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        this.registerReceiver(mReceiver, intentFilter);
        mIsUnregisterReceiver = true;

    }
    @Override
    protected void onStart() {
        super.onStart();
        if (DEBUG)
            Log.d(TAG, "onStart");

    }

    @Override
    protected void onPause() {
        super.onPause();
        if (DEBUG)
            Log.d(TAG, "onPause");
    }

    @Override
    protected void onDestroy() {
        // Stop the Bluetooth connect services
        if (DEBUG)
            Log.d(TAG, "onDestroy");
        super.onDestroy();
        if (mIsUnregisterReceiver == true) {
            unregisterReceiver(mReceiver);
            mIsUnregisterReceiver = false;
        }

    }

    private List<String> initBtSettings(CachedBluetoothDevice mCachedBluetoothDevice,
        int mProfileState, boolean firstTime) {
        int index = 0;
        List<String> items = new ArrayList<String>();
        Log.d(TAG,"mCachedBluetoothDevice = " + mCachedBluetoothDevice+" ,mProfileState = " + mProfileState);
        for (LocalBluetoothProfile profile : mCachedBluetoothDevice.getConnectableProfiles()){
            Log.d(TAG, "initBtSettings getConnectableProfiles =  "+ mCachedBluetoothDevice.getConnectableProfiles());
            items.add(getString(profile.getNameResource(mCachedBluetoothDevice.getDevice())));
            index++;
        }
        return items;
    }
    // Called when clicked on the OK button
    private final DialogInterface.OnClickListener mClickListener =
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        if (which == DialogInterface.BUTTON_POSITIVE) {
                            Log.d(TAG, "BUTTON_POSITIVE Clicked " );
                            DestroyActivity();
                        }
                    }
                };

    // Called when the individual bt profiles are clicked.
    private final DialogInterface.OnMultiChoiceClickListener mMultiClickListener =
                new DialogInterface.OnMultiChoiceClickListener() {
                    public void onClick(DialogInterface dialog, int which, boolean isChecked) {
                        if (DEBUG) {
                            Log.d(TAG, "Item " + which + " changed to " + isChecked);
                            Log.d(TAG, "Item [which]= " + mConnectItems[which]);
                        }
                        if(isChecked == true){
                            mDevice.connectProfileName(mConnectItems[which].toString());
                            Toast.makeText(getApplicationContext(), "Connecting Bluetooth device",
                                                   Toast.LENGTH_LONG).show();
                        }
                        else{
                            mDevice.disconnectProfileName(mConnectItems[which].toString());
                            Toast.makeText(getApplicationContext(), "Disconnecting Bluetooth device",
                                                    Toast.LENGTH_LONG).show();
                        }

                        mCheckedItems[which] = isChecked;
                    }
                };
    public void onClick(View v) {

    }

    // The on-click listener for all operations in the ListViews
    private OnItemClickListener mOperationsClickListener = new OnItemClickListener() {
        public void onItemClick(AdapterView<?> av, View v, int position, long arg3) {
            if (DEBUG)
                Log.d(TAG, "onItemClick ");

            HashMap<String, Object> operationsInfo = mOperationsList.get(position);
            String operations_name = (String)operationsInfo.get("operations_name");
            Log.d(TAG,"operations_name = " + operations_name);
            
            if(operations_name.equals(getString(R.string.trypair_bt_string)) ){
                // Pairing is unreliable while scanning, so cancel discovery
                if (mLocalManager.getBluetoothAdapter().isDiscovering()) {
                    mLocalManager.getBluetoothAdapter().cancelDiscovery();
                }
                if(mDevice!= null) {
                    if (mDevice.getBondState() == BluetoothDevice.BOND_BONDED) {
                        Toast.makeText(getApplicationContext(),
                                "The Bluetooth device is bonded already", Toast.LENGTH_SHORT)
                                .show();
                    } else if(mDevice.getBondState() == BluetoothDevice.BOND_BONDING) {
                        Toast.makeText(getApplicationContext(),
                                "The Bluetooth device is bonding ,please wait", Toast.LENGTH_SHORT);                            
                    }else{
                        if (!mDevice.startPairing())
                            Log.e(TAG, "createBond Error");

                    }
                }
            } else if (operations_name.equals(getString(R.string.dispair_bt_string))) {
                // disPairing is unreliable while scanning, so cancel
                // discovery
                if (mLocalManager.getBluetoothAdapter().isDiscovering()) {
                    mLocalManager.getBluetoothAdapter().cancelDiscovery();
                }
                if (mDevice != null) {
                    if (mDevice.getBondState() == BluetoothDevice.BOND_BONDED) {
                        mDevice.unpair();
                        for(int i=0;i<BtPairConnectActivity.mCheckedItems.length;i++){
                            BtPairConnectActivity.mCheckedItems[i]=false;
                        }
                        DestroyActivity();

                    } else {
                        Toast.makeText(getApplicationContext(),
                                "The Bluetooth device is unbonded already", Toast.LENGTH_LONG)
                                .show();
                    }
                }
            } else if (operations_name.equals(getString(R.string.connect_bt_string))) {
                if(mDevice != null) {
                    int state = mDevice.getBondState();
                    Log.d(TAG,"state == " + state);
                    if(state == BluetoothDevice.BOND_BONDED) {

                        //cachedDevice.onClicked();
                        List<String> mProfileList = initBtSettings(mDevice,mProfileState,false);
                        mConnectItems = (CharSequence[])mProfileList.toArray(new CharSequence[mProfileList.size()]);

                        final AlertDialog.Builder ab = new AlertDialog.Builder(BtPairConnectActivity.this);
                        ab.setTitle(getString(R.string.connect_bt_string));

                        // Profiles
                        ab.setMultiChoiceItems(mConnectItems, mCheckedItems, mMultiClickListener);
                        ab.setPositiveButton(getString(android.R.string.ok), mClickListener);
                        mConnectAsDialog = ab.create();
                        mConnectAsDialog.show();
                    } else {
                        Log.e(TAG, "pair Bluetooth first");
                        Toast.makeText(getApplicationContext(),
                            "We will pair Bluetooth device first", Toast.LENGTH_LONG)
                            .show();
                        if (!mDevice.startPairing()) {
                            Log.e(TAG, "pair Bluetooth device Error");
                        }else{
                            mIsconnect = true;
                        }
                    }
                }

            } else if (operations_name.equals(getString(R.string.disconnect_bt_string))) {
                Utils.disconnectBT(BtPairConnectActivity.this,mDevice);
                Toast.makeText(getApplicationContext(),
                    "Disconnecting Bluetooth device", Toast.LENGTH_LONG).show();
                for(int i = 0; i < BtPairConnectActivity.mCheckedItems.length; i++){
                    BtPairConnectActivity.mCheckedItems[i]=false;
                }
                DestroyActivity();

            } else if (operations_name.equals(getString(R.string.bt_pairconnect_return))) {
               DestroyActivity();
            } else {
                Toast.makeText(getApplicationContext(), "Error", Toast.LENGTH_LONG).show();
            }
        }
    };

    private void handleBondChanged(Intent intent) {
        BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
        int bondState = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE, BluetoothDevice.ERROR);
        Log.d(TAG, device + " ACTION_BOND_STATE_CHANGED " + bondState);
        if (bondState == BluetoothDevice.BOND_BONDED) {
            if (device.equals(mDevice.getDevice())) {
                mOperationsList.get(0)
                        .put("operations_name", getString(R.string.dispair_bt_string));
                mOperationsList.get(1)
                        .put("operations_name", getString(R.string.disconnect_bt_string));
                mOperationsArrayAdapter.notifyDataSetChanged();
                if (mIsconnect == true) {
                    mIsconnect = false;
                    Toast.makeText(getApplicationContext(), "Connecting Bluetooth device",
                            Toast.LENGTH_LONG).show();
                }
            }
        } else if (bondState == BluetoothDevice.BOND_NONE) {
            if (device.equals(mDevice.getDevice())) {
                mOperationsList.get(0)
                        .put("operations_name", getString(R.string.trypair_bt_string));
                mOperationsArrayAdapter.notifyDataSetChanged();
                for(int i=0;i<BtPairConnectActivity.mCheckedItems.length;i++){
                     BtPairConnectActivity.mCheckedItems[i]=false;
                 }
                mIsconnect = false;
            }
        }
    }

    private void handleConnectChanged(Intent intent) {
        String addr = null;// intent.getStringExtra(BluetoothHf.DEVICE_ADDR);
        int profilestate = 0;//intent.getIntExtra(BluetoothProfileManager.EXTRA_NEW_STATE,
                //BluetoothProfileManager.STATE_DISCONNECTED);
        Log.d(TAG, addr + " handleConnectChanged " + profilestate);
                /*
        if (profilestate == BluetoothProfileManager.STATE_CONNECTED) {
            if ((BluetoothAdapter.getDefaultAdapter().getRemoteDevice(addr))
                    .equals(mDevice.getDevice())) {
                mOperationsList.get(1).put("operations_name",
                        getString(R.string.disconnect_bt_string));
                mOperationsArrayAdapter.notifyDataSetChanged();
            }
        } else if (profilestate == BluetoothProfileManager.STATE_DISCONNECTED) {
            if ((BluetoothAdapter.getDefaultAdapter().getRemoteDevice(addr))
                    .equals(mDevice.getDevice())) {
                mOperationsList.get(1)
                        .put("operations_name", getString(R.string.connect_bt_string));
                mOperationsArrayAdapter.notifyDataSetChanged();
            }
        }*/
    }

    private void onBluetoothPairingRequest(Intent intent) {
       BluetoothDevice mSelectedDevice = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

        int type = intent.getIntExtra(BluetoothDevice.EXTRA_PAIRING_VARIANT, BluetoothDevice.ERROR);

        Toast.makeText(getApplicationContext(), mSelectedDevice.getName() + " : type = " + type,
                Toast.LENGTH_LONG).show();

        if (type == BluetoothDevice.PAIRING_VARIANT_PIN) {
            // mSelectedDevice.setPin(BluetoothDevice.convertPinToBytes(BtSettingsActivity.mPin));
        } else if (type == BluetoothDevice.PAIRING_VARIANT_PASSKEY_CONFIRMATION) {
            int passkey = intent.getIntExtra(BluetoothDevice.EXTRA_PAIRING_KEY,
                    BluetoothDevice.ERROR);
            mSelectedDevice.setPairingConfirmation(true);
        } else {
            Log.e(TAG, "Incorrect pairing type received");
        }
    }

    private void DestroyActivity() {
        if (mIsUnregisterReceiver == true) {
            this.unregisterReceiver(mReceiver);
            mIsUnregisterReceiver = false;
        }
        if (DEBUG)
            Log.i(TAG, "Destroy Activity ");
        finish();
    }

}
