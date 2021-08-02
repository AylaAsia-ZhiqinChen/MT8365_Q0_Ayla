/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.mediatek.bluetooth.util;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.app.FragmentManager;
import android.app.KeyguardManager;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothUuid;
import android.content.ContentResolver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.Resources;
import android.database.Cursor;
import android.media.AudioManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.ParcelUuid;
import android.os.PowerManager;
import android.provider.ContactsContract;
import android.widget.Toast;
import android.widget.PopupWindow;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;

import com.mediatek.bluetooth.R;
import com.mediatek.bluetooth.pbapclient.*;
import com.mediatek.bluetooth.common.*;
import com.mediatek.bluetooth.common.LocalBluetoothManager.BluetoothManagerCallback;

import java.text.NumberFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

import net.sourceforge.pinyin4j.PinyinHelper;
import net.sourceforge.pinyin4j.format.*;

/**
 * Utils is a helper class that contains constants for various Android resource
 * IDs, debug logging flags, and static methods for creating dialogs.
 */
public class Utils {
    public static final boolean V = true; // verbose logging
    public static final boolean D = true; // regular logging

    public static final int META_INT_ERROR = -1;

    private static final String KEY_ERROR = "errorMessage";
    private static final String TAG = "Bluetooth.Utils";
    static final int BD_ADDR_LEN = 6; // bytes
    static final int BD_UUID_LEN = 16; // bytes
    static PopupWindow mwindow;
    private static AlertDialog dialog;
    private static KeyguardManager km;
    private static KeyguardManager.KeyguardLock kl;
    private static PowerManager pm;
    private static PowerManager.WakeLock wl;
    private static final String TIMESTAMP_FORMAT = "yyyyMMdd'T'HHmmss";

    private Utils() {
    }

    public static String getAddressStringFromByte(BluetoothDevice device) {
        byte[] address = getByteAddress(device);
        Log.d(TAG,"getAddressStringFromByte "+address);
        if (address == null || address.length != BD_ADDR_LEN) {
            return null;
        }

        return String.format("%02X:%02X:%02X:%02X:%02X:%02X",
                address[5], address[4], address[3], address[2], address[1],
                address[0]);
    }

    public static byte[] getByteAddress(BluetoothDevice device) {
        return getBytesFromAddress(device.getAddress());
    }

    public static byte[] getBytesFromAddress(String address) {
        Log.d(TAG,"getBytesFromAddress ="+address);
        int i, j = 0;
        byte[] output = new byte[BD_ADDR_LEN];

        for (i = 0; i < address.length(); i++) {
            if (address.charAt(i) != ':') {
                output[j] = (byte) Integer.parseInt(address.substring(i, i + 2), BD_UUID_LEN);
                j++;
                i++;
            }
        }

        return output;
    }

    public static int getConnectionStateSummary(int connectionState) {
        switch (connectionState) {
        case BluetoothProfile.STATE_CONNECTED:
            return R.string.bluetooth_connected;
        case BluetoothProfile.STATE_CONNECTING:
            return R.string.bluetooth_connecting;
        case BluetoothProfile.STATE_DISCONNECTED:
            return R.string.bluetooth_disconnected;
        case BluetoothProfile.STATE_DISCONNECTING:
            return R.string.bluetooth_disconnecting;
        default:
            return 0;
        }
    }

    public static void showPopupWindow(Context context,View parent,int screenW,int screenH){
        if(mwindow == null){
            mwindow = new PopupWindow(LayoutInflater.from(context).inflate(R.layout.pro_popuwindow,null));
            mwindow.setOutsideTouchable(false);
            mwindow.setHeight(screenH);
            mwindow.setWidth(screenW);
            mwindow.showAtLocation(parent,Gravity.CENTER,0,0);
        }else{
            mwindow.showAtLocation(parent,Gravity.CENTER,0,0);
        }
    }

    public static void dismissPopupWindow(){
        Log.d(TAG,"dismissDialog");
        if(mwindow!=null && mwindow.isShowing()){
            mwindow.dismiss();
        }

    }

    // Create (or recycle existing) and show disconnect dialog.
    static AlertDialog showDisconnectDialog(Context context, AlertDialog dialog,
            DialogInterface.OnClickListener disconnectListener, CharSequence title,
            CharSequence message) {
        if (dialog == null) {
            dialog = new AlertDialog.Builder(context)
                    .setPositiveButton(android.R.string.ok, disconnectListener)
                    .setNegativeButton(android.R.string.cancel, null).create();
        } else {
            if (dialog.isShowing()) {
                dialog.dismiss();
            }
            // use disconnectListener for the correct profile(s)
            CharSequence okText = context.getText(android.R.string.ok);
            dialog.setButton(DialogInterface.BUTTON_POSITIVE, okText, disconnectListener);
        }
        dialog.setTitle(title);
        dialog.setMessage(message);
        dialog.show();
        return dialog;
    }

    // TODO: wire this up to show connection errors...
    static void showConnectingError(Context context, String name) {
        showError(context, name, R.string.bluetooth_connecting_error_message);
    }

    public static void showError(Context context, String name, int messageResId) {
        showError(context, name, messageResId, getLocalBtManager(context));
    }
    private static void showError(Context context, String name, int messageResId,
            LocalBluetoothManager manager) {
        String message = context.getString(messageResId, name);
        Context activity = manager.getForegroundActivity();
        if (manager.isForegroundActivity()) {
            /** M: Google original code phased out to adapt to our feature
            new AlertDialog.Builder(activity)
                .setTitle(R.string.bluetooth_error_title)
                .setMessage(message)
                .setPositiveButton(android.R.string.ok, null)
                .show();
             */
            /// M: Use DialogFragment instead of AlertDialog @{
            Log.d(TAG, "show ErrorDialogFragment, message is " + message);
            ErrorDialogFragment dialog = new ErrorDialogFragment();
            final Bundle args = new Bundle();
            args.putString(KEY_ERROR, message);
            dialog.setArguments(args);
            dialog.show(((Activity) activity).getFragmentManager(), "Error");
            /// @}
        } else {
            Toast.makeText(context, message, Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * Update the search Index for a specific class name and resources.
     */
    /*
     * public static void updateSearchIndex(Context context, String className,
     * String title, String screenTitle, int iconResId, boolean enabled) {
     * SearchIndexableRaw data = new SearchIndexableRaw(context); data.className
     * = className; data.title = title; data.screenTitle = screenTitle;
     * data.iconResId = iconResId; data.enabled = enabled;
     *
     * //Index.getInstance(context).updateFromSearchIndexableData(data); }
     */
    public static class ErrorDialogFragment extends DialogFragment {
        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            final String message = getArguments().getString(KEY_ERROR);

            return new AlertDialog.Builder(getActivity())
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .setTitle(R.string.bluetooth_error_title).setMessage(message)
                    .setPositiveButton(android.R.string.ok, null).show();
        }
    }

    public static void showShortToast(Context context,String content){
        Toast.makeText(context,content,Toast.LENGTH_SHORT).show();
    }

    public static void showShortToast(Context context,int content){
        Toast.makeText(context,content,Toast.LENGTH_SHORT).show();
    }


    public static void wakeUpAndUnlock(Context context,boolean on){
        if(km == null)km= (KeyguardManager) context.getSystemService(Context.KEYGUARD_SERVICE);
        if(kl == null)kl = km.newKeyguardLock("unLock");

        kl.disableKeyguard();
        if(pm == null)pm=(PowerManager) context.getSystemService(Context.POWER_SERVICE);
        if(wl == null)wl = pm.newWakeLock(PowerManager.ACQUIRE_CAUSES_WAKEUP | PowerManager.SCREEN_DIM_WAKE_LOCK,"bright"); 
        if(on){
           wl.acquire();
        }else {
           if(wl != null){
              wl.release();
              wl = null;
           }
        }
    }

    public static void disconnectBT(Context context,CachedBluetoothDevice device){

        Log.d(TAG,"device == " + device);
        if(device == null){
            Log.d(TAG,"cacheDevice == Null");
            return;
        }
        device.disconnect();
    }

    public static void disconnectPbap(){
        BluetoothPbapClientManager mPbapManager = BluetoothPbapClientManager.getInstance();
        if(mPbapManager != null){
           Log.d(TAG,"disconnect pbap");
           mPbapManager.disconnectDevice();
        }

    }
    public static void connectPBAPClient(Context context,BluetoothDevice mConnectedDevice){
        if(mConnectedDevice == null){
            showShortToast(context,"connected device is null");
            return;
        }
        LocalBluetoothManager mLocalManager = getLocalBtManager(context);
        CachedBluetoothDevice mDevice = mLocalManager.getCachedDeviceManager().findDevice(mConnectedDevice);

        if(mDevice == null){
            Log.d(TAG,"connect PBAPClient cachedDevice is null");
            return;
        }
        Resources res = context.getResources();
        mDevice.connectProfileName(res.getString(R.string.bluetooth_profile_pbap));

    }

     public static void disConnectPBAPClient(Context context,BluetoothDevice mConnectedDevice){
         if(mConnectedDevice == null){
             showShortToast(context,"disconnected device is null");
             return;
         }
         LocalBluetoothManager mLocalManager = getLocalBtManager(context);
         CachedBluetoothDevice mDevice = mLocalManager.getCachedDeviceManager().findDevice(mConnectedDevice);

         if(mDevice == null){
             Log.d(TAG,"disconnected PBAPClient cachedDevice is null");
             return;
         }
         Resources res = context.getResources();
         mDevice.disconnectProfileName(res.getString(R.string.bluetooth_profile_pbap));
     }

     public static boolean isA2dpSinkSupport(LocalBluetoothAdapter adapter) {
        boolean isA2dpSinkSupport = false;
        ParcelUuid[] localUuids = adapter.getUuids();
        if (localUuids != null) {
            isA2dpSinkSupport = BluetoothUuid.isUuidPresent(localUuids, BluetoothUuid.AudioSink);
        }
        Log.d(TAG, "localUuids : " + localUuids + " isA2dpSinkSupport : " + isA2dpSinkSupport);
        return isA2dpSinkSupport;
    }



    public static void showPbapConnectDialog(final Context context,final BluetoothDevice device){
        if(dialog != null){
            dialog.dismiss();
        }
        BluetoothAdapter adapter = BluetoothAdapter.getDefaultAdapter();
        if(adapter != null && adapter.getState()== BluetoothAdapter.STATE_OFF) return;
        dialog = new AlertDialog.Builder(context)
                    .setPositiveButton(R.string.auth_ok, new DialogInterface.OnClickListener(){
                        public void onClick(DialogInterface dialog, int which) {
                            connectPBAPClient(context,device);
                            if (dialog != null) {
                                dialog = null;
                            }
                        }

                    })
                    .setNegativeButton(R.string.auth_cancel, new DialogInterface.OnClickListener(){
                        public void onClick(DialogInterface dialog, int which) {

                            if (dialog != null) {
                                dialog = null;
                            }
                        }

                    }).create();
        dialog.setTitle(R.string.pbap_connect_alert);
        dialog.setMessage(context.getResources().getString(R.string.pbap_connect_alert_message));
        dialog.show();
    }

    public static void dismissPbapConnectDialog(){
       if(dialog != null){
            dialog.dismiss();
            dialog = null;
        }
    }

    public static String dateFormat(String datestring){

        StringBuilder sDate = new StringBuilder();
        sDate.append(datestring.substring(0,4)).append("-").append(datestring.substring(4,6))
            .append("-").append(datestring.substring(6,8));

        return sDate.toString();
    }

    public static String timeFormat(String  timestring){

        StringBuilder sTime = new StringBuilder();
        sTime.append(timestring.substring(0,2)).append(":").append(timestring.substring(2,4))
            .append(":").append(timestring.substring(4,6));

        return sTime.toString();
    }

    /** Formats a double from 0.0..100.0 with an option to round **/
    public static String formatPercentage(double percentage, boolean round) {
        final int localPercentage = round ? Math.round((float) percentage) : (int) percentage;
        return formatPercentage(localPercentage);
    }

    /** Formats the ratio of amount/total as a percentage. */
    public static String formatPercentage(long amount, long total) {
        return formatPercentage(((double) amount) / total);
    }

    /** Formats an integer from 0..100 as a percentage. */
    public static String formatPercentage(int percentage) {
        return formatPercentage(((double) percentage) / 100.0);
    }

    /** Formats a double from 0.0..1.0 as a percentage. */
    public static String formatPercentage(double percentage) {
        return NumberFormat.getPercentInstance().format(percentage);
    }

    /**
     * get that {@link AudioManager#getMode()} is in ringing/call/communication(VoIP) status.
     */
    public static boolean isAudioModeOngoingCall(Context context) {
        final AudioManager audioManager = context.getSystemService(AudioManager.class);
        final int audioMode = audioManager.getMode();
        return audioMode == AudioManager.MODE_RINGTONE
                || audioMode == AudioManager.MODE_IN_CALL
                || audioMode == AudioManager.MODE_IN_COMMUNICATION;
    }

    /**
     * Get boolean Bluetooth metadata
     *
     * @param bluetoothDevice the BluetoothDevice to get metadata
     * @param key key value within the list of BluetoothDevice.METADATA_*
     * @return the boolean metdata
     */
    public static boolean getBooleanMetaData(BluetoothDevice bluetoothDevice, int key) {
        if (bluetoothDevice == null) {
            return false;
        }
        final byte[] data = bluetoothDevice.getMetadata(key);
        if (data == null) {
            return false;
        }
        return Boolean.parseBoolean(new String(data));
    }

    /**
     * Get String Bluetooth metadata
     *
     * @param bluetoothDevice the BluetoothDevice to get metadata
     * @param key key value within the list of BluetoothDevice.METADATA_*
     * @return the String metdata
     */
    public static String getStringMetaData(BluetoothDevice bluetoothDevice, int key) {
        if (bluetoothDevice == null) {
            return null;
        }
        final byte[] data = bluetoothDevice.getMetadata(key);
        if (data == null) {
            return null;
        }
        return new String(data);
    }

    /**
     * Get integer Bluetooth metadata
     *
     * @param bluetoothDevice the BluetoothDevice to get metadata
     * @param key key value within the list of BluetoothDevice.METADATA_*
     * @return the int metdata
     */
    public static int getIntMetaData(BluetoothDevice bluetoothDevice, int key) {
        if (bluetoothDevice == null) {
            return META_INT_ERROR;
        }
        final byte[] data = bluetoothDevice.getMetadata(key);
        if (data == null) {
            return META_INT_ERROR;
        }
        try {
            return Integer.parseInt(new String(data));
        } catch (NumberFormatException e) {
            return META_INT_ERROR;
        }
    }

    /**
     * Get URI Bluetooth metadata
     *
     * @param bluetoothDevice the BluetoothDevice to get metadata
     * @param key key value within the list of BluetoothDevice.METADATA_*
     * @return the URI metdata
     */
    public static Uri getUriMetaData(BluetoothDevice bluetoothDevice, int key) {
        String data = getStringMetaData(bluetoothDevice, key);
        if (data == null) {
            return null;
        }
        return Uri.parse(data);
    }


    public static LocalBluetoothManager getLocalBtManager(Context context) {
        return LocalBluetoothManager.getInstance(context, mOnInitCallback);
    }


    private static final BluetoothManagerCallback mOnInitCallback = new BluetoothManagerCallback() {
        @Override
        public void onBluetoothManagerInitialized(Context appContext,
                LocalBluetoothManager bluetoothManager) {
            Log.d(TAG, "onBluetoothManagerInitialized ...");
            //setErrorListener(mErrorListener);
        }
    };

    public static String getDateToString(long time){
        Date d = new Date(time);
        SimpleDateFormat simpleDateFormat = new SimpleDateFormat(TIMESTAMP_FORMAT);
        String timeString = simpleDateFormat.format(d);
        if (timeString != null){
            String[] textarry = timeString.split("T");
            if(textarry != null && textarry.length > 0){
               String dateformat = Utils.dateFormat(textarry[0]);
               String timeformat = Utils.timeFormat(textarry[1]);
               Log.d(TAG, "getDateToString return " + dateformat + " " + timeformat);
               return (dateformat + " " + timeformat);
            }
        }
        Log.d(TAG, "getDateToString return null");
        return "";
    }

    public static String getContactNameByNumber(Context context, String number){
        String displayName = null;
        Cursor cursor = null;
        //Log.d(TAG, "getContactNameByNumber...");
        try{
            ContentResolver resolver = context.getContentResolver();
            Uri uri = ContactsContract.CommonDataKinds.Phone.CONTENT_URI;
            String[] projection = new String[]{
                ContactsContract.CommonDataKinds.Phone.DISPLAY_NAME,
                ContactsContract.CommonDataKinds.Phone.NUMBER
                };
            //String selection = ContactsContract.CommonDataKinds.Phone.NUMBER + " = '" + number + "'";
            //Log.d(TAG, "getContactNameByNumber, selection = " + selection);
            cursor = resolver.query(uri, projection, null, null, null);
            //Log.d(TAG, "getContactNameByNumber, cursor = " + cursor);
            if (cursor != null){
                while (cursor.moveToNext()) {
                    int numberColIdx = cursor.getColumnIndex(
                        ContactsContract.CommonDataKinds.Phone.NUMBER);
                    String phoneNumber = cursor.getString(numberColIdx);
                    phoneNumber = phoneNumber.replace("-", "");
                    //Log.d(TAG, "getContactNameByNumber, phoneNumber = " + phoneNumber);
                    if (phoneNumber.equals(number)){
                        int nameColIdx = cursor.getColumnIndex(
                            ContactsContract.CommonDataKinds.Phone.DISPLAY_NAME);
                        displayName = cursor.getString(nameColIdx);
                        //Log.d(TAG, "getContactNameByNumber, displayName = " + displayName);
                        break;
                    }
                }
            }
        } catch(Exception e){
            e.printStackTrace();
            Log.d(TAG, "getContactNameByNumber, get a exception");
        } finally{
            if (cursor != null){
                cursor.close();
            }
        }
        //Log.d(TAG, "getContactNameByNumber, end displayName = " + displayName);
        return displayName;
    }

    /**
        * get pinyin from chinese words
        */
    public static String getPinYin(String chines) {
        StringBuffer sb = new StringBuffer();
        sb.setLength(0);
        char[] nameChar = chines.toCharArray();
        HanyuPinyinOutputFormat defaultFormat = new HanyuPinyinOutputFormat();
        defaultFormat.setCaseType(HanyuPinyinCaseType.LOWERCASE);
        defaultFormat.setToneType(HanyuPinyinToneType.WITHOUT_TONE);
        for (int i = 0; i < nameChar.length; i++) {
            if (nameChar[i] > 128) {
                try {
                    sb.append(PinyinHelper.toHanyuPinyinStringArray(nameChar[i], defaultFormat)[0]);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            } else {
                sb.append(nameChar[i]);
            }
        }
        return sb.toString();
    }
}
