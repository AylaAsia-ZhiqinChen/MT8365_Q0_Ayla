package com.mediatek.bluetooth.pbapclient;


import android.bluetooth.BluetoothDevice;
//import android.bluetooth.client.pbap.BluetoothPbapCard;
import android.bluetooth.BluetoothPbapClient;
//import android.bluetooth.client.pbap.BluetoothPbapClient.ConnectionState;
import android.bluetooth.BluetoothProfile.ServiceListener;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.android.vcard.VCardEntry;

import java.util.ArrayList;

public class BluetoothPbapClientManager {

    private static final String TAG = "BluetoothPbapClientManager";
    private Handler mHandler = null;
    private BluetoothPbapClient mBluetoothPbapClient = null;
    private static BluetoothPbapClientManager sInstance;
    private ArrayList<BluetoothPabapClientCallback> mCallbackList
            = new ArrayList<BluetoothPabapClientCallback>();
    private String mCurrentPath = "";
    private String mNextPath = "";
    private int mConnectStatus = 0;//ConnectionState.DISCONNECTED.ordinal();
    private BluetoothDevice connectedDevice = null;

    public static BluetoothPbapClientManager getInstance() {
        if (sInstance == null) {
            sInstance = new BluetoothPbapClientManager();
        }
        return sInstance;
    }

    public void initConnect(BluetoothDevice device) {
        if (mHandler == null) {
            mHandler = new Handler() {
                @Override
                public void handleMessage(Message msg) {
                    switch (msg.what) {
                        /*
                        case BluetoothPbapClient.EVENT_SET_PHONE_BOOK_DONE:
                            mCurrentPath = mNextPath;
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onSetPathDone(true);
                            }
                            break;
                        case BluetoothPbapClient.EVENT_PULL_PHONE_BOOK_DONE:
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onPullBookDone(true, msg.arg1,
                                        (ArrayList<VCardEntry>) msg.obj);
                            }
                            break;
                        case BluetoothPbapClient.EVENT_PULL_VCARD_LISTING_DONE:
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onPullVcardListingDone(true, msg.arg1,
                                        (ArrayList<BluetoothPbapCard>) msg.obj);
                            }
                            break;
                        case BluetoothPbapClient.EVENT_PULL_VCARD_ENTRY_DONE:
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onPullVcardEntryDone(true, (VCardEntry) msg.obj);
                            }
                            break;
                        case BluetoothPbapClient.EVENT_PULL_PHONE_BOOK_SIZE_DONE:
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onPullPhonebookSizeDone(true, msg.arg1);
                            }
                            break;
                        case BluetoothPbapClient.EVENT_PULL_VCARD_LISTING_SIZE_DONE:
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onPullVcardListingSizeDone(true, msg.arg1);
                            }
                            break;
                        case BluetoothPbapClient.EVENT_SET_PHONE_BOOK_ERROR:
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onSetPathDone(false);
                            }
                            break;
                        case BluetoothPbapClient.EVENT_PULL_PHONE_BOOK_ERROR:
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onPullBookDone(false, msg.arg1,
                                        (ArrayList<VCardEntry>) msg.obj);
                            }
                            break;
                        case BluetoothPbapClient.EVENT_PULL_VCARD_LISTING_ERROR:
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onPullVcardListingDone(false, msg.arg1,
                                        (ArrayList<BluetoothPbapCard>) msg.obj);
                            }
                            break;
                        case BluetoothPbapClient.EVENT_PULL_VCARD_ENTRY_ERROR:
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onPullVcardEntryDone(false, (VCardEntry) msg.obj);
                            }
                            break;
                        case BluetoothPbapClient.EVENT_PULL_PHONE_BOOK_SIZE_ERROR:
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onPullPhonebookSizeDone(false, msg.arg1);
                            }
                            break;
                        case BluetoothPbapClient.EVENT_PULL_VCARD_LISTING_SIZE_ERROR:
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onPullVcardListingSizeDone(false, msg.arg1);
                            }
                            break;
                        case BluetoothPbapClient.EVENT_SESSION_CONNECTED:
                            Log.d(TAG, "BluetoothPbapClient.EVENT_SESSION_CONNECTED");
                            //mConnectStatus = ConnectionState.CONNECTED.ordinal();
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onConnectStatusChange(true);
                            }
                            break;
                        case BluetoothPbapClient.EVENT_SESSION_DISCONNECTED:
                            Log.d(TAG, "BluetoothPbapClient.EVENT_SESSION_DISCONNECTED");
                            //mConnectStatus = ConnectionState.DISCONNECTED.ordinal();
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onConnectStatusChange(false);
                            }
                            break;
                        case BluetoothPbapClient.EVENT_SESSION_AUTH_REQUESTED:
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onAuthenticationRequest();
                            }
                            break;
                        case BluetoothPbapClient.EVENT_SESSION_AUTH_TIMEOUT:
                            for (BluetoothPabapClientCallback callback : mCallbackList) {
                                callback.onAuthenticationTimeout();
                            }
                            break;
                            */
                        default:
                            break;
                    }
                }
            };
        }

        if(mBluetoothPbapClient != null && getDevice().getAddress().equals(device.getAddress())/*&&
            getConnectState() == ConnectionState.CONNECTED.ordinal()*/){ 
            Log.d(TAG,"devices is same,so abort");
            //mBluetoothPbapClient.abort();
            //mBluetoothPbapClient.disconnect();
            //setConnectedDevice(null);
            return;
        } 

        if(mBluetoothPbapClient != null && !getDevice().getAddress().equals(device.getAddress())){
            Log.d(TAG,"device is different,so diconnect");
            //mBluetoothPbapClient.abort();  
            disconnectDevice();
            setConnectedDevice(null);
            mBluetoothPbapClient = null; 
        } 
            Log.d(TAG,"new mBluetoothPbapClient");
            mBluetoothPbapClient = null;//new BluetoothPbapClient(device, new ServiceListener());
        
    }

    public void setConnectedDevice(BluetoothDevice device){
        connectedDevice = device;
    }

    public BluetoothDevice getDevice(){
        return connectedDevice;
    }

    public void connectDevice(BluetoothDevice device) {
        Log.d(TAG, "connectDevice mBluetoothPbapClient = " + mBluetoothPbapClient);
        Log.d(TAG, "connectDevice device = " + device);
        //mConnectStatus = ConnectionState.CONNECTING.ordinal();
        mBluetoothPbapClient.connect(device);
        mCurrentPath = "";
        setConnectedDevice(device);
        Log.d(TAG, "connectDevice finished");
    }

    public void disconnectDevice() {
        //mConnectStatus = ConnectionState.DISCONNECTING.ordinal();
        new Thread() {
            @Override
            public void run() {
                Log.d(TAG, "disconnectDevice start");
                if (mBluetoothPbapClient != null) {
                    //mBluetoothPbapClient.abort();
                    mBluetoothPbapClient.disconnect(null);
                    mCurrentPath = "";
                }
                Log.d(TAG, "disconnectDevice finished");
            }
        }.start();
    }

    public void setState(int state){
        Log.d(TAG,"old state ="+mConnectStatus+" new state = "+state);
        mConnectStatus = state;
    }

    public int getConnectState() {
        Log.d(TAG,"getConnectState  mConnectStatus start = "+mConnectStatus);
        //if(mBluetoothPbapClient != null && 
        //    mConnectStatus != mBluetoothPbapClient.getState().ordinal()){
        //    mConnectStatus = mBluetoothPbapClient.getState().ordinal();
        //}
        Log.d(TAG,"getConnectState  mConnectStatus end = "+mConnectStatus);
        return mConnectStatus;
    }

    public void registerCallback(BluetoothPabapClientCallback callback) {
        if (!mCallbackList.contains(callback)) {
            mCallbackList.clear();
            mCallbackList.add(callback);
        }
    }

    public void unregisterCallback(BluetoothPabapClientCallback callback) {
        if(checkIfContains(callback)){
            mCallbackList.remove(callback);
        }
    }

    public boolean checkIfContains(BluetoothPabapClientCallback callback){
        if(mCallbackList != null && mCallbackList.size()>0){
            if(mCallbackList.contains(callback)){
                return true;
            }else{
                return false;
            }
        }else{
            return false;
        }

    }

    public boolean setPhoneBookFolderRoot() {
        mNextPath = "";
        return false;//mBluetoothPbapClient.setPhoneBookFolderRoot();
    }

    public boolean setPhoneBookFolderUp() {
        int position = mCurrentPath.lastIndexOf("/");
        mNextPath = mCurrentPath.substring(0, position - 1);
        return false;//mBluetoothPbapClient.setPhoneBookFolderUp();
    }

    public boolean setPhoneBookFolderDown(String folder) {
        if (mCurrentPath.length() > 0) {
            mNextPath = mCurrentPath + "/" + folder;
        } else {
            mNextPath = folder;
        }
        return false;//mBluetoothPbapClient.setPhoneBookFolderDown(folder);
    }

    public boolean pullPhoneBookSize(String pbName) {
        return false;//mBluetoothPbapClient.pullPhoneBookSize(pbName);
    }

    public boolean pullVcardListingSize(String folder) {
        return false;//mBluetoothPbapClient.pullVcardListingSize(folder);
    }


    public boolean pullPhoneBook(String pbName, int maxListCount, int listStartOffset) {
        return false;//mBluetoothPbapClient.pullPhoneBook(pbName, maxListCount, listStartOffset);
    }

    public boolean pullPhoneBook(String pbName, long filter, byte format, int maxListCount,
            int listStartOffset) {
        return false;//mBluetoothPbapClient.pullPhoneBook(pbName, filter, format, maxListCount,
                //listStartOffset);
    }

    public boolean pullVcardEntry(String handle, long filter, byte format) {
        Log.d(TAG," handle ,"+handle+" format ,"+format+" filter ,"+filter);
        return false;//mBluetoothPbapClient.pullVcardEntry(handle, filter, format);
    }

    public boolean pullVcardEntry(String handle) {
        Log.d(TAG," handle ,"+handle);
        return false;//mBluetoothPbapClient.pullVcardEntry(handle);
    }

    public boolean pullVcardList(String folder, byte order, byte searchAttr,
            String searchVal, int maxListCount, int listStartOffset) {
        return false;//mBluetoothPbapClient.pullVcardListing(folder, order, searchAttr, searchVal,
                //maxListCount, listStartOffset);
    }

    public boolean setAuthResponse(String key) {
        return false;//mBluetoothPbapClient.setAuthResponse(key);
    }

    public String getCurrentPath() {
        return mCurrentPath;
    }
}
