package com.mediatek.op07.telecom;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.ServiceManager;
import android.os.RemoteException;
import android.provider.Settings;
import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;
import android.telephony.PhoneNumberUtils;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.android.ims.ImsManager;
import com.android.internal.telephony.ITelephony;
import com.mediatek.server.telecom.ext.DefaultCallMgrExt;

/**
 * Plug in implementation for OP07 Call Mananger interfaces.
 */
public class OP07CallMgrExt extends DefaultCallMgrExt {
    private static final String TAG = "OP07CallMgrExt";
    Context mContext;

    /** Constructor.
     * @param context context
     */
    public OP07CallMgrExt(Context context) {
        mContext = context;
    }

    @Override
    public boolean blockOutgoingCall(Uri handle, PhoneAccountHandle phoneAccountHandle,
            Bundle extras) {
        Log.d(TAG, "blockOutgoingCall");
        boolean result = false;
        boolean isWfcReg = false;
        int errorMessageId = -1;
        boolean isWfcEnabled = ImsManager.isWfcEnabledByUser(mContext);
        Log.d(TAG, "isWfcEnabled : " + isWfcEnabled);
        if (isWfcEnabled) {
            isWfcReg = ((TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE)).
                    isWifiCallingAvailable();
        }
        Log.d(TAG, "isWfcReg = " + isWfcReg);
        if (isWfcReg) {
            PhoneAccountHandle account =
                    TelecomManager.from(mContext).getDefaultOutgoingPhoneAccount(handle.getScheme());
            int subId = TelephonyManager.from(mContext)
                    .getSubIdForPhoneAccount(TelecomManager.from(mContext).getPhoneAccount(account));

            if (locationBasedNumber(handle) &&!isRadioOn(subId)
                && !isAirplaneModeOn(mContext)){
                errorMessageId = R.string.celullar_ran_required;
                show(errorMessageId);
                result = true;
            }
        }
        return result;
    }

     private void show(int errorMessageId) {
     final Intent errorIntent = new Intent(mContext, AlertDialogActivity.class);
         errorIntent.putExtra(AlertDialogActivity.ERROR_MESSAGE_ID_EXTRA, errorMessageId);
         errorIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
         mContext.startActivity(errorIntent);
    }

    private boolean locationBasedNumber(Uri handle) {
        boolean result = false;
        String number = handle.getSchemeSpecificPart();
        String[] locationNum = {"311", "511", "811"};

        for (int i = 0; i < locationNum.length; ++i) {
            if (locationNum[i].equals(number)) {
                result = true;
            }
        }
        Log.i(TAG, "locationBasedNumber result =" + result);
        return result;
    }

    private boolean isAirplaneModeOn(Context context) {
        return Settings.System.getInt(context.getContentResolver(),
                Settings.System.AIRPLANE_MODE_ON, 0) != 0;
    }

    private boolean isRadioOn(int subId) {
        Log.i(TAG, "isRadioOn  subId = " + subId);
        boolean isRadioOn = false;
        final ITelephony iTel = ITelephony.Stub.asInterface(
                ServiceManager.getService(Context.TELEPHONY_SERVICE));
        if (iTel != null) {
            try {
                isRadioOn = iTel.isRadioOnForSubscriber(subId, mContext.getOpPackageName());
            } catch (RemoteException e) {
                Log.i(TAG, "isRadioOn  failed to get radio state for sub = " + subId);
            }
        } else {
            Log.i(TAG, "isRadioOn  failed because  iTel= null, subId =" + subId);
        }
        Log.i(TAG, "isRadioOn isRadioOn =" + isRadioOn);
        return isRadioOn;
    }
}
