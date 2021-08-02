package com.android.nfc.st;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.SystemProperties;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.util.Log;
import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.TelephonyIntents;

// import com.android.internal.telephony.PhoneConstants;
// import com.mediatek.nfc.porting.*;

public class NfcSimStateObserver extends BroadcastReceiver {
    private static final String TAG = "NfcSimStateObserver";
    private static final int MSG_TIMEOUT = 1;
    private Context mContext;
    // private Handler mHandler;
    // private IntentFilter mFilter;
    // private Callback mCallback;
    private boolean mIsAirplaneModeOn;
    private boolean mAirplaneModeHidesSims;
    public static final int STATE_ABSENT = 0;
    public static final int STATE_PRESENT_BUSY = 1;
    public static final int STATE_PRESENT_READY = 2;
    public static final int STATE_UNKNOWN = 0xff;
    public static final int PENDING_NONE = STATE_UNKNOWN;

    private int mSim1State = STATE_UNKNOWN;
    private int mSim2State = STATE_UNKNOWN;

    // Logic to handle the case when intent is delivered before NFC stack starts
    private int mPendingSim1 = PENDING_NONE;
    private int mPendingSim2 = PENDING_NONE;

    public NfcSimStateObserver() {}

    private static NfcSimStateObserver mSingleton;
    private SimEventListener mSimListener;
    // private SdCardEventListener mSdCardListener;

    /*
        mtk(old) : airplane mode: no SIM notifications on eject / insert.
        mtk(old) :enter airplane mode: no bradcast about SIM state.
        mtk(old) :leave airplane, no SIM: NOT_READY -> ABSENT
        mtk(old) :leave airplane, SIM is here: NOT_READY -> READY -> IMSI -> LOADED

        mtk(new) : airplane mode: same notifications as non-airplane.
            qc      : airplane mode: same notifications as non-airplane.

        mtk:eject SIM: ABSENT
        mtk:insert SIM: READY -> IMSI -> LOADED

            qc:eject SIM: ABSENT
            qc:insert SIM: (NOT_READY -> LOCKED ->) NOT_READY -> READY -> LOADED
    */

    private int convertTelephonyState(int telephonyState) {
        switch (telephonyState) {
            case TelephonyManager.SIM_STATE_ABSENT:
                // Card not inserted
                return STATE_ABSENT;

            case TelephonyManager.SIM_STATE_NOT_READY:
                // For above state, it means a SIM is here but not goot time to initialize SWP
                return STATE_PRESENT_BUSY;

            case TelephonyManager.SIM_STATE_READY:
            case TelephonyManager.SIM_STATE_PERM_DISABLED:
            case TelephonyManager.SIM_STATE_PIN_REQUIRED:
            case TelephonyManager.SIM_STATE_PUK_REQUIRED:
            case TelephonyManager.SIM_STATE_CARD_RESTRICTED:
            case TelephonyManager.SIM_STATE_NETWORK_LOCKED:
            case TelephonyManager.SIM_STATE_CARD_IO_ERROR:
                // For these states, SIM is inserted and no more ISO activity so we can try SWP
                return STATE_PRESENT_READY;

            case TelephonyManager.SIM_STATE_UNKNOWN:
                // These states we need to wait
                return STATE_UNKNOWN;

            default:
                Log.d(TAG, "State " + telephonyState + " is unknown");
                return STATE_UNKNOWN;
        }
    }

    public int GetSimState(int simId) {
        Log.d(
                TAG,
                "GetSimState simId = "
                        + simId
                        + ", mSim1State = "
                        + mSim1State
                        + " mSim2State = "
                        + mSim2State);

        if (simId == 0) {
            if (mSim1State == STATE_UNKNOWN) {
                int s = TelephonyManager.getDefault().getSimState(simId);
                Log.d(TAG, "Retrieved SIM1 state: " + s);
                mSim1State = convertTelephonyState(s);
            }
            return mSim1State;
        } else if (simId == 1) {
            if (mSim2State == STATE_UNKNOWN) {
                int s = TelephonyManager.getDefault().getSimState(simId);
                Log.d(TAG, "Retrieved SIM2 state: " + s);
                mSim2State = convertTelephonyState(s);
            }
            return mSim2State;
        }
        return STATE_UNKNOWN;
    }

    public static void createSingleton(Context context) {
        Log.d(TAG, " createSingleton() - attach the context to the existing instance");
        NfcSimStateObserver instance = getInstance();
        instance.mContext = context;
        // IntentFilter mFilter = new IntentFilter();
        // mFilter.addAction(Intent.ACTION_MEDIA_CHECKING);
        // mFilter.addAction(Intent.ACTION_MEDIA_EJECT);
        // mFilter.addDataScheme("file");
        // instance.mContext.registerReceiver(instance.mSdCardBroadcastReceiver, mFilter);

        instance.mContext.registerReceiver(
                instance, new IntentFilter(Intent.ACTION_AIRPLANE_MODE_CHANGED));

        instance.mIsAirplaneModeOn =
                (Settings.Global.getInt(
                                context.getContentResolver(), Settings.Global.AIRPLANE_MODE_ON, 0)
                        != 0);

        instance.mAirplaneModeHidesSims =
                SystemProperties.get("persist.vendor.st_nfc_modem_airplane").equals("1");
    }

    private void updateSimState(int simId, int newState) {
        NfcSimStateObserver instance = getInstance();
        int prevState = STATE_UNKNOWN;
        int prevPending = PENDING_NONE;
        if (simId == 0) {
            prevState = instance.mSim1State;
            prevPending = instance.mPendingSim1;
            instance.mSim1State = newState;
        } else if (simId == 1) {
            prevState = instance.mSim2State;
            prevPending = instance.mPendingSim2;
            instance.mSim2State = newState;
        }

        if (instance.mSimListener != null) {
            if ((prevPending != PENDING_NONE) || (newState != prevState)) {
                if (simId == 0) {
                    instance.mPendingSim1 = PENDING_NONE;
                } else if (simId == 1) {
                    instance.mPendingSim2 = PENDING_NONE;
                }
                instance.mSimListener.onSimStateChanged(simId, newState);
            }
        } else {
            if (simId == 0) {
                instance.mPendingSim1 = newState;
            } else if (simId == 1) {
                instance.mPendingSim2 = newState;
            }
        }
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();
        if (action == null) {
            Log.e(TAG, "onReceive() action == null");
            return;
        }
        Log.d(TAG, "onReceive: " + action);

        if (action.equals(TelephonyIntents.ACTION_SIM_STATE_CHANGED)) {
            String iccState;
            int simId;
            iccState = intent.getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE);
            simId = intent.getIntExtra(PhoneConstants.PHONE_KEY, -1);
            if (iccState == null) {
                iccState = "NULL";
            }
            Log.d(
                    TAG,
                    "ACTION_SIM_STATE_CHANGED receiver with iccState = "
                            + iccState
                            + ", simId = "
                            + simId);
            if (simId == -1) simId = 0;
            if (iccState.equals(IccCardConstants.INTENT_VALUE_ICC_LOADED)) {
                updateSimState(simId, STATE_PRESENT_READY);
            } else if (iccState.equals(IccCardConstants.INTENT_VALUE_ICC_ABSENT)) {
                updateSimState(simId, STATE_ABSENT);
            } else if (iccState.equals(IccCardConstants.INTENT_VALUE_ICC_CARD_IO_ERROR)) {
                updateSimState(simId, STATE_PRESENT_READY);
            } else if (iccState.equals(IccCardConstants.INTENT_VALUE_ICC_LOCKED)) {
                // can be for PIN, PUK, or NETWORK
                updateSimState(simId, STATE_PRESENT_READY);
            } else if (iccState.equals(IccCardConstants.INTENT_VALUE_ICC_READY)) {
                // modem still working on it, not good time to init SWP
                updateSimState(simId, STATE_PRESENT_BUSY);
            } else if (iccState.equals(IccCardConstants.INTENT_VALUE_ICC_IMSI)) {
                updateSimState(simId, STATE_PRESENT_BUSY);
            } else if (iccState.equals(IccCardConstants.INTENT_VALUE_ABSENT_ON_PERM_DISABLED)) {
                updateSimState(simId, STATE_PRESENT_READY);
            }
        } else if (action.equals(Intent.ACTION_AIRPLANE_MODE_CHANGED)) {
            NfcSimStateObserver instance = getInstance();
            instance.mIsAirplaneModeOn = intent.getBooleanExtra("state", false);
            Log.d(
                    TAG,
                    "ACTION_AIRPLANE_MODE_CHANGED receiver, state = "
                            + instance.mIsAirplaneModeOn
                            + ", mSimListener="
                            + instance.mSimListener);
        }
    }

    // private BroadcastReceiver mSdCardBroadcastReceiver = new BroadcastReceiver() {
    //     @Override
    //     public void onReceive(Context context, Intent intent) {
    //         String action = intent.getAction();
    //         NfcSimStateObserver instance = getInstance();
    //         if (action == null) {
    //             Log.e(TAG, "onReceive() action == null");
    //             return;
    //         }
    //         if (action.equals(Intent.ACTION_MEDIA_EJECT)) {
    //             Log.d(TAG, "Intent.ACTION_MEDIA_EJECT");
    //             if (instance.mSdCardListener != null) {
    //                 /// currently don't care, simply trigger update
    //                 instance.mSdCardListener.onSdCardStateChanged(0);
    //             }
    //         } else if (action.equals(Intent.ACTION_MEDIA_CHECKING)) {
    //             Log.d(TAG, "Intent.ACTION_MEDIA_CHECKING");
    //             if (instance.mSdCardListener != null) {
    //                 /// currently don't care, simply trigger update
    //                 instance.mSdCardListener.onSdCardStateChanged(0);
    //             }
    //         }
    //     }
    // };

    public static NfcSimStateObserver getInstance() {
        if (mSingleton == null) {
            mSingleton = new NfcSimStateObserver();
        }
        return mSingleton;
    }

    public interface SimEventListener {
        public static final int SIM1 = 0;
        public static final int SIM2 = 1;
        public static final int ABSENT = NfcSimStateObserver.STATE_ABSENT;
        public static final int PRESENT_BUSY = NfcSimStateObserver.STATE_PRESENT_BUSY;
        public static final int PRESENT_READY = NfcSimStateObserver.STATE_PRESENT_READY;

        public void onSimStateChanged(int simId, int event);
    }

    // public interface SdCardEventListener {
    //     static public final int REMOVE = 0;
    //     static public final int INSERT = 1;

    //     public void onSdCardStateChanged(int event);
    // }

    public void registerSimEventListener(SimEventListener simListener) {
        Log.d(TAG, "registerSimEventListener() " + simListener);
        NfcSimStateObserver instance = getInstance();

        synchronized (instance) {
            instance.mSimListener = simListener;

            // Apply any pending results
            new Thread(
                            new Runnable() {
                                @Override
                                public void run() {
                                    NfcSimStateObserver instance = getInstance();
                                    synchronized (instance) {
                                        Log.d(
                                                TAG,
                                                "mPendingSim1="
                                                        + instance.mPendingSim1
                                                        + " mPendingSim2="
                                                        + instance.mPendingSim2
                                                        + " simListener="
                                                        + instance.mSimListener);
                                        if (instance.mSimListener != null) {
                                            if (instance.mPendingSim1 != PENDING_NONE) {
                                                instance.mSimListener.onSimStateChanged(
                                                        SimEventListener.SIM1,
                                                        instance.mPendingSim1);
                                                instance.mPendingSim1 = PENDING_NONE;
                                            }

                                            if (instance.mPendingSim2 != PENDING_NONE) {
                                                instance.mSimListener.onSimStateChanged(
                                                        SimEventListener.SIM2,
                                                        instance.mPendingSim2);
                                                instance.mPendingSim2 = PENDING_NONE;
                                            }
                                        }
                                    }
                                }
                            })
                    .start();
        }
    }

    // public void registerSdCardEventListener(SdCardEventListener sdCardListener) {
    //     NfcSimStateObserver instance = getInstance();
    //     instance.mSdCardListener = sdCardListener;
    // }

    public void unregisterSimEventListener() {
        Log.v(TAG, "unregisterSimEventListener");
        NfcSimStateObserver instance = getInstance();
        synchronized (instance) {
            instance.mSimListener = null;
        }
    }

    // public void unregisterSdCardEventListener() {
    //     NfcSimStateObserver instance = getInstance();
    //     instance.mSdCardListener = null;
    // }

    public boolean isAirplaneModeHidingSimPresence() {
        NfcSimStateObserver instance = getInstance();
        // By default, SIM presence is updated even if in airplane mode
        // except in older modems where SIM power is shutdown in airplane mode.
        if (instance.mAirplaneModeHidesSims) {
            return instance.mIsAirplaneModeOn;
        }
        return false;
    }
}
