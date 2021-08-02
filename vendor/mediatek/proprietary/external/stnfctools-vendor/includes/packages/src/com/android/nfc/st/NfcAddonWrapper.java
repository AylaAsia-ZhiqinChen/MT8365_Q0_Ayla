package com.android.nfc.st;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.util.Log;
import com.android.nfc.NfcService;
import com.android.nfc.dhimpl.NativeNfcStExtensions;
import com.android.nfc.dhimpl.StNativeNfcManager;
import com.st.android.nfc_extensions.INfcSettingsCallback;
import com.st.android.nfc_extensions.NfcSettingsAdapter;
import java.util.List;

// MTK - start
import android.content.Intent;
import android.database.ContentObserver;
import android.net.Uri;
import android.os.SystemClock;
import android.os.SystemProperties;
import android.provider.Settings;
import com.android.nfc.R;
// MTK - end

public class NfcAddonWrapper implements ISeController.Callback {

    private static final String TAG = "StNfcAddonWrapper";

    public static final String PREF = "NfcServicePrefs";

    public static final String PREF_MODE_READER = "nfc.pref.mode.reader";
    public static final String PREF_MODE_P2P = "nfc.pref.mode.p2p";
    public static final String PREF_MODE_HCE = "nfc.pref.mode.card";

    static final String PREF_NFC_ON = "nfc_on";

    // MTK - start
    public static final String NFC_MULTISE_ON = "nfc_multise_on";
    public static final String NFC_MULTISE_LIST = "nfc_multise_list"; // SIM1,SIM2,Embedded SE
    public static final String NFC_MULTISE_ACTIVE = "nfc_multise_active"; // value type: String
    public static final String NFC_MULTISE_PREVIOUS = "nfc_multise_previous";
    public static final String NFC_MULTISE_IN_SWITCHING = "nfc_multise_in_switching";
    public static final String NFC_MULTISE_IN_TRANSACTION = "nfc_multise_in_transation";
    public static final String MTKSETTING_STR_SIM1 = "SIM1";
    public static final String MTKSETTING_STR_SIM2 = "SIM2";
    public static final String MTKSETTING_STR_ESE = "Embedded SE";
    public static final String MTKSETTING_STR_OFF = "Off";
    public static final String NFC_HCE_ON = "nfc_hce_on";
    private static final String PREF_AID_ROUTE_ID = "aid_route";
    public static final boolean HAS_MTK_SETTINGS =
            ("1".equals(SystemProperties.get("ro.vendor.mtk_nfc_addon_support"))
                    && !"1".equals(SystemProperties.get("persist.st_nfc_ignore_addon_support")));
    // MTK - end

    private StNativeNfcManager mNativeNfcManager;
    private SharedPreferences mPrefs;
    private SharedPreferences.Editor mPrefsEditor;
    private Context mContext;
    private static NfcAddonWrapper sSingleton;
    private ISeController mSecureElementSelector;
    NativeNfcStExtensions mStExtensions;
    // MTK - start
    private MyContentObserverUtil mContentObserverUtil;
    private boolean mUpdateSettingsUi;
    // MTK -- end

    private NfcAddonWrapper(
            Context context, StNativeNfcManager manager, NativeNfcStExtensions stExtensions) {

        mNativeNfcManager = (StNativeNfcManager) manager;
        mContext = context;
        mPrefs = mContext.getSharedPreferences(PREF, Context.MODE_PRIVATE);
        mPrefsEditor = mPrefs.edit();
        mStExtensions = stExtensions;
        Log.d(TAG, "Constructor");
        // MTK - start
        if (HAS_MTK_SETTINGS) {
            mUpdateSettingsUi = true;
            mContentObserverUtil = new MyContentObserverUtil();
        }
        // MTK - end

        mSecureElementSelector =
                new SecureElementSelector(
                        mContext, mNativeNfcManager, mStExtensions, NfcService.getInstance(), this);
    }

    public static void createSingleton(
            Context context, StNativeNfcManager manager, NativeNfcStExtensions stExtensions) {
        sSingleton = new NfcAddonWrapper(context, manager, stExtensions);
    }

    public static NfcAddonWrapper getInstance() {
        return sSingleton;
    }

    public void applyDeinitializeSequence() {
        // MTK - start
        if (HAS_MTK_SETTINGS) {
            mContentObserverUtil.unregister();
        }
        // MTK - end
        mSecureElementSelector.deinit(false);
    }

    public void applyInitializeSequence() {

        PackageManager pm = mContext.getPackageManager();
        boolean isHceCapable = pm.hasSystemFeature(PackageManager.FEATURE_NFC_HOST_CARD_EMULATION);
        boolean isBeamCapable = pm.hasSystemFeature(PackageManager.FEATURE_NFC_BEAM);

        boolean isHceOn = false;

        if (isHceCapable) {
            Log.d(TAG, "applyInitializeSequence() - isHceCapable=1");

            // MTK - start
            if (HAS_MTK_SETTINGS) {
                isHceOn = Settings.Global.getInt(mContext.getContentResolver(), NFC_HCE_ON, 1) != 0;
                Settings.Global.putInt(mContext.getContentResolver(), NFC_HCE_ON, isHceOn ? 1 : 0);
                mPrefsEditor.putInt(
                        PREF_MODE_HCE,
                        isHceOn ? NfcSettingsAdapter.FLAG_ON : NfcSettingsAdapter.FLAG_OFF);
                mPrefsEditor.apply();
            } else {
                int hceFlag = mPrefs.getInt(PREF_MODE_HCE, NfcSettingsAdapter.FLAG_ON);
                if (hceFlag == 1) {
                    isHceOn = true;
                }
            }
        } else {
            mPrefsEditor.putInt(PREF_MODE_HCE, NfcSettingsAdapter.FLAG_OFF);
            mPrefsEditor.apply();
            // MTK - end
        }

        Log.d(TAG, "applyInitializeSequence() - isHceOn = " + isHceOn);
        Log.d(TAG, "applyInitializeSequence() - isBeamCapable = " + isBeamCapable);
        // Settings.Global.NFC_HCE_ON to "nfc_hce_on"
        int mode = 0;
        if (mPrefs.getInt(PREF_MODE_READER, NfcSettingsAdapter.FLAG_ON)
                == NfcSettingsAdapter.FLAG_ON) {
            mode |= NfcSettingsAdapter.MODE_READER;
        }
        if ((mPrefs.getInt(PREF_MODE_P2P, NfcSettingsAdapter.FLAG_ON) == NfcSettingsAdapter.FLAG_ON)
                && (isBeamCapable)) {
            mode |= NfcSettingsAdapter.MODE_P2P;
        }
        if (isHceOn) {
            mode |= NfcSettingsAdapter.MODE_HCE;
        }

        Log.d(TAG, "applyInitializeSequence() - mode:" + mode);

        // MTK - start
        // Initialize the values needed by mtksettings but that we don t really use
        Settings.Global.putInt(mContext.getContentResolver(), NFC_MULTISE_ON, 1);
        Settings.Global.putInt(mContext.getContentResolver(), NFC_MULTISE_IN_TRANSACTION, 0);
        // MTK - end

        mSecureElementSelector.init();

        // MTK - start
        if (HAS_MTK_SETTINGS) {
            updateStatusForMtkSettings();
            mContentObserverUtil.register();
        }
        // MTK - end

        // Retrieve current RfConfiguration
        byte[] techArray = new byte[4];
        int modeBitmap = mStExtensions.getRfConfiguration(techArray);

        if ((mode & NfcSettingsAdapter.MODE_READER) != 0) {
            modeBitmap |= 0x1;
        } else {
            modeBitmap &= ~0x1;
        }
        if ((mode & NfcSettingsAdapter.MODE_HCE) != 0) {
            modeBitmap |= 0x2;
        } else {
            modeBitmap &= ~0x2;
        }

        if ((mode & NfcSettingsAdapter.MODE_P2P) != 0) {
            modeBitmap |= 0x4; // listen
            modeBitmap |= 0x8; // poll
        } else {
            modeBitmap &= ~0x4;
            modeBitmap &= ~0x8;
        }

        mStExtensions.setRfConfiguration(modeBitmap, techArray);
    }

    public boolean EnableSecureElement(String SeId, boolean enable) {
        return mSecureElementSelector.EnableSecureElement(SeId, enable);
    }

    public List<String> getSecureElementsStatus() {
        // MTK - start
        // return mSecureElementSelector.getSecureElementsStatus();
        return mSecureElementSelector.getSecureElementsStatus(false);
        // MTK - end
    }

    final RemoteCallbackList<INfcSettingsCallback> mSettingsCallbacks =
            new RemoteCallbackList<INfcSettingsCallback>();

    public void registerNfcSettingsCallback(INfcSettingsCallback cb) {
        mSettingsCallbacks.register(cb);
    }

    public void unregisterNfcSettingsCallback(INfcSettingsCallback cb) {
        mSettingsCallbacks.unregister(cb);
    }

    // MTK - start
    private class MyContentObserverUtil {
        private String mLastAppliedSetting = "";

        public MyContentObserverUtil() {}

        public void register() {
            mContext.getContentResolver()
                    .registerContentObserver(
                            Settings.Global.getUriFor(NFC_MULTISE_ACTIVE), false, mObserver);

            mContext.getContentResolver()
                    .registerContentObserver(
                            Settings.Global.getUriFor(NFC_HCE_ON), false, mHceObserver);
        }

        public void unregister() {
            mContext.getContentResolver().unregisterContentObserver(mObserver);
            mContext.getContentResolver().unregisterContentObserver(mHceObserver);
        }

        public void setLastAppliedSetting(String se) {
            mLastAppliedSetting = se;
        }

        private final ContentObserver mObserver =
                new ContentObserver(null) {
                    @Override
                    public void onChange(boolean selfChange, Uri uri) {
                        String newConfigFromUser;
                        Log.d(TAG, "ContentObserver - onChange()");
                        Long startTime = SystemClock.elapsedRealtime();
                        int GET_ACTIVE_SE_STRING_POLL_MS = 150;
                        int WAIT_FOR_GET_ACTIVE_SE_STRING_OPERATIONS_MS = 2000;

                        do {
                            newConfigFromUser =
                                    Settings.Global.getString(
                                            mContext.getContentResolver(), NFC_MULTISE_ACTIVE);
                            if (mLastAppliedSetting.equals(newConfigFromUser)) {
                                Log.d(
                                        TAG,
                                        "ContentObserver - onChange() - newConfigFromUser was already applied");
                                return;
                            }
                            if (newConfigFromUser != null) break;
                            try {
                                Thread.sleep(GET_ACTIVE_SE_STRING_POLL_MS);
                            } catch (InterruptedException e) {
                                // Ignore
                            }
                        } while (SystemClock.elapsedRealtime() - startTime
                                < WAIT_FOR_GET_ACTIVE_SE_STRING_OPERATIONS_MS);

                        if (newConfigFromUser == null) {
                            Log.d(
                                    TAG,
                                    "ContentObserver - onChange() - "
                                            + "newConfigFromUser is still null,"
                                            + "bypass the ContentObserver onchanged routine!");
                            return;
                        } else {
                            Log.d(
                                    TAG,
                                    "ContentObserver - onChange() --> newConfigFromUser = "
                                            + newConfigFromUser
                                            + ", selfChange = "
                                            + selfChange);
                            mLastAppliedSetting = newConfigFromUser;
                        }

                        boolean simWasSwitched = false;

                        String dialogAlt =
                                null; // set to any possible choice. null if no choice possible
                        String dialogNa = null; // set to user requested SE if N/A

                        // Get current configuration
                        List<String> l = mSecureElementSelector.getSecureElementsStatus(false);

                        mUpdateSettingsUi = false;

                        // MTK: set default route to active EE -- we always have only 1
                        if (MTKSETTING_STR_SIM1.equals(newConfigFromUser)
                                || MTKSETTING_STR_SIM2.equals(newConfigFromUser)) {
                            Log.d(TAG, "ContentObserver - onChange() --> new default route = SIM");
                            NfcService.getInstance()
                                    .updateDefaultRoutesTo(NfcSettingsAdapter.UICC_ROUTE);
                        } else if (MTKSETTING_STR_ESE.equals(newConfigFromUser)) {
                            Log.d(TAG, "ContentObserver - onChange() --> new default route = ESE");
                            NfcService.getInstance()
                                    .updateDefaultRoutesTo(NfcSettingsAdapter.ESE_ROUTE);
                        } else {
                            Log.d(TAG, "ContentObserver - onChange() --> restore default routes");
                            NfcService.getInstance()
                                    .updateDefaultRoutesTo(NfcSettingsAdapter.DEFAULT_ROUTE);
                        }

                        // Change what needs to be changed to reflect new config
                        for (String v : l) {
                            String delims = "[:]";
                            String[] tokens = v.split(delims);
                            if (SecureElementSelector.SETTING_STR_SIM1.equals(tokens[0])) {
                                if (MTKSETTING_STR_SIM1.equals(newConfigFromUser)) {
                                    if (SecureElementSelector.SE_STATE_ACTIVATED.equals(
                                            tokens[1])) {
                                        // already in wanted state
                                    } else if (SecureElementSelector.SE_STATE_AVAILABLE.equals(
                                            tokens[1])) {
                                        if (EnableSecureElement(
                                                        SecureElementSelector.SETTING_STR_SIM1,
                                                        true)
                                                == true) {
                                            simWasSwitched = true;
                                        } else {
                                            dialogNa = MTKSETTING_STR_SIM1;
                                        }
                                    } else if (SecureElementSelector.SE_STATE_NOT_AVAILABLE.equals(
                                            tokens[1])) {
                                        dialogNa = MTKSETTING_STR_SIM1;
                                    }
                                } else {
                                    if (SecureElementSelector.SE_STATE_ACTIVATED.equals(
                                            tokens[1])) {
                                        // User does not want it
                                        if (!MTKSETTING_STR_SIM2.equals(newConfigFromUser)) {
                                            EnableSecureElement(
                                                    SecureElementSelector.SETTING_STR_SIM1, false);
                                        } // otherwise it will be deactivated while SIM2 is
                                        // activated.
                                    }
                                    if (!SecureElementSelector.SE_STATE_NOT_AVAILABLE.equals(
                                            tokens[1])) {
                                        dialogAlt = MTKSETTING_STR_SIM1;
                                    }
                                }
                            } else if (SecureElementSelector.SETTING_STR_SIM2.equals(tokens[0])) {
                                if (MTKSETTING_STR_SIM2.equals(newConfigFromUser)) {
                                    if (SecureElementSelector.SE_STATE_ACTIVATED.equals(
                                            tokens[1])) {
                                        // already in wanted state
                                    } else if (SecureElementSelector.SE_STATE_AVAILABLE.equals(
                                            tokens[1])) {
                                        if (EnableSecureElement(
                                                        SecureElementSelector.SETTING_STR_SIM2,
                                                        true)
                                                != true) {
                                            dialogNa = MTKSETTING_STR_SIM2;
                                        }
                                    } else if (SecureElementSelector.SE_STATE_NOT_AVAILABLE.equals(
                                            tokens[1])) {
                                        dialogNa = MTKSETTING_STR_SIM2;
                                    }
                                } else {
                                    if (SecureElementSelector.SE_STATE_ACTIVATED.equals(
                                            tokens[1])) {
                                        // User does not want it
                                        if (!simWasSwitched) {
                                            EnableSecureElement(
                                                    SecureElementSelector.SETTING_STR_SIM2, false);
                                        }
                                    }
                                    if (!SecureElementSelector.SE_STATE_NOT_AVAILABLE.equals(
                                            tokens[1])) {
                                        dialogAlt = MTKSETTING_STR_SIM2;
                                    }
                                }
                            } else if (SecureElementSelector.SETTING_STR_ESE.equals(tokens[0])) {
                                if (MTKSETTING_STR_ESE.equals(newConfigFromUser)) {
                                    if (SecureElementSelector.SE_STATE_ACTIVATED.equals(
                                            tokens[1])) {
                                        // already in wanted state
                                    } else if (SecureElementSelector.SE_STATE_AVAILABLE.equals(
                                            tokens[1])) {
                                        if (!EnableSecureElement(
                                                SecureElementSelector.SETTING_STR_ESE, true)) {
                                            dialogNa = MTKSETTING_STR_ESE;
                                        }
                                    } else if (SecureElementSelector.SE_STATE_NOT_AVAILABLE.equals(
                                            tokens[1])) {
                                        dialogNa = MTKSETTING_STR_ESE;
                                    }
                                } else {
                                    if (SecureElementSelector.SE_STATE_ACTIVATED.equals(
                                            tokens[1])) {
                                        // User does not want it
                                        EnableSecureElement(
                                                SecureElementSelector.SETTING_STR_ESE, false);
                                    }
                                    if (!SecureElementSelector.SE_STATE_NOT_AVAILABLE.equals(
                                            tokens[1])) {
                                        dialogAlt = MTKSETTING_STR_ESE;
                                    }
                                }
                            }
                        }
                        mUpdateSettingsUi = true;
                        updateStatusForMtkSettings();

                        // Changes are applied. UI was already updated by callbacks.
                        // Now show dialog if needed
                        if (dialogNa != null) {
                            // don't show if it is the first time (new device).
                            // if the UI does not have a menu, don t show
                            // If option persist.... is set, don t show.
                            showAlternativeSeDialog(dialogNa, dialogAlt);
                        }
                    }
                };

        // Settings.Global.NFC_HCE_ON to "nfc_hce_on"
        private final ContentObserver mHceObserver =
                new ContentObserver(null) {
                    @Override
                    public void onChange(boolean selfChange, Uri uri) {
                        boolean isHceOn =
                                Settings.Global.getInt(mContext.getContentResolver(), NFC_HCE_ON, 0)
                                        != 0;
                        Log.d(
                                TAG,
                                "NfcService, onChanged() --> isHceOn = "
                                        + isHceOn
                                        + ", selfChange = "
                                        + selfChange);
                        setModeFlag(true, NfcSettingsAdapter.MODE_HCE, isHceOn ? 1 : 0, this);
                    }
                };
    }

    private void showAlternativeSeDialog(String requested, String alternate) {
        int alternateSe;
        String message;
        Log.d(
                TAG,
                "showAlternativeSeDialog() - User requested '"
                        + requested
                        + "' is not available, show dialog");

        if (alternate == null) {
            message = mContext.getString(R.string.mtk_se_na);
            alternate = "";
        } else {
            message = requested + mContext.getString(R.string.mtk_se_alt) + alternate + "?";
        }

        Intent intent = new Intent(mContext, MtkSecureElementConfirmActivity.class);
        intent.putExtra(
                MtkSecureElementConfirmActivity.EXTRA_TITLE,
                mContext.getString(R.string.mtk_nfc_card_emulation));
        intent.putExtra(MtkSecureElementConfirmActivity.EXTRA_MESSAGE, message);
        intent.putExtra(MtkSecureElementConfirmActivity.EXTRA_FIRSTSE, alternate);
        intent.setFlags(intent.getFlags() | Intent.FLAG_ACTIVITY_NEW_TASK);
        mContext.startActivity(intent);
    }

    private void updateStatusForMtkSettings() {
        List<String> l = mSecureElementSelector.getSecureElementsStatus(true);
        String outStr = "";
        String active = MTKSETTING_STR_OFF;

        for (String v : l) {
            String delims = "[:]";
            String[] tokens = v.split(delims);
            Log.d(
                    TAG,
                    "onSecureElementStatusChanged() - '" + tokens[0] + "' -> '" + tokens[1] + "'");
            if (SecureElementSelector.SETTING_STR_SIM1.equals(tokens[0])
                    && !SecureElementSelector.SE_STATE_NOT_AVAILABLE.equals(tokens[1])) {
                outStr += MTKSETTING_STR_SIM1 + ",";
                if (SecureElementSelector.SE_STATE_ACTIVATED.equals(tokens[1])) {
                    // if (! MTKSETTING_STR_OFF.equals(active)) {
                    //     active += "," + MTKSETTING_STR_SIM1;
                    // } else {
                    active = MTKSETTING_STR_SIM1;
                    // }
                }
            } else if (SecureElementSelector.SETTING_STR_SIM2.equals(tokens[0])
                    && !SecureElementSelector.SE_STATE_NOT_AVAILABLE.equals(tokens[1])) {
                outStr += MTKSETTING_STR_SIM2 + ",";
                if (SecureElementSelector.SE_STATE_ACTIVATED.equals(tokens[1])) {
                    // if (! MTKSETTING_STR_OFF.equals(active)) {
                    //     active += "," + MTKSETTING_STR_SIM2;
                    // } else {
                    active = MTKSETTING_STR_SIM2;
                    // }
                }
            } else if (SecureElementSelector.SETTING_STR_ESE.equals(tokens[0])
                    && !SecureElementSelector.SE_STATE_NOT_AVAILABLE.equals(tokens[1])) {
                outStr += MTKSETTING_STR_ESE + ",";
                if (SecureElementSelector.SE_STATE_ACTIVATED.equals(tokens[1])) {
                    // if (! MTKSETTING_STR_OFF.equals(active)) {
                    //     active += "," + MTKSETTING_STR_ESE;
                    // } else {
                    active = MTKSETTING_STR_ESE;
                    // }
                }
            }
        }
        outStr += MTKSETTING_STR_OFF;

        Log.d(TAG, "setAvailableSeList() - outStr = " + outStr);
        Settings.Global.putString(mContext.getContentResolver(), NFC_MULTISE_LIST, outStr);
        Log.d(TAG, "setActiveSeString() - seStr = " + active);
        // mContentObserverUtil.setLastAppliedSetting(active);
        Settings.Global.putString(mContext.getContentResolver(), NFC_MULTISE_ACTIVE, active);
        if (!MTKSETTING_STR_OFF.equals(active)) {
            Log.d(TAG, "setPreviousActiveSeString() - prevSeStr = " + active);
            Settings.Global.putString(mContext.getContentResolver(), NFC_MULTISE_PREVIOUS, active);
        }
    }
    // MTK - end

    /// ISeController.Callback
    public void onSecureElementStatusChanged() {
        final int N = mSettingsCallbacks.beginBroadcast();
        for (int i = 0; i < N; i++) {
            try {
                mSettingsCallbacks.getBroadcastItem(i).onSecureElementStatusChanged();
            } catch (RemoteException e) {
                // The RemoteCallbackList will take care of removing
                // the dead object for us.
            }
        }
        mSettingsCallbacks.finishBroadcast();
        // MTK - start
        if (mUpdateSettingsUi) {
            updateStatusForMtkSettings();
        }
        // MTK - end
    }

    // MTK - start
    /// ISeController.Callback
    public void onSecureElementChangeProgress(boolean started) {
        Settings.Global.putInt(
                mContext.getContentResolver(), NFC_MULTISE_IN_SWITCHING, started ? 1 : 0);
    }
    // MTK - end

    public void onRouteChanged() {
        final int N = mSettingsCallbacks.beginBroadcast();
        for (int i = 0; i < N; i++) {
            try {
                mSettingsCallbacks.getBroadcastItem(i).onRouteChanged();
            } catch (RemoteException e) {
                // The RemoteCallbackList will take care of removing
                // the dead object for us.
            }
        }
        mSettingsCallbacks.finishBroadcast();
    }

    // Nfc Framework API
    public int getModeFlag(int mode, Object syncObj) {
        Log.d(TAG, "getModeFlag() - mode = " + mode);
        int flag = -1;
        synchronized (syncObj) {
            if (NfcSettingsAdapter.MODE_READER == mode) {
                flag = mPrefs.getInt(PREF_MODE_READER, NfcSettingsAdapter.FLAG_ON);
            } else if (NfcSettingsAdapter.MODE_P2P == mode) {
                flag = mPrefs.getInt(PREF_MODE_P2P, NfcSettingsAdapter.FLAG_ON);
            } else if (NfcSettingsAdapter.MODE_HCE == mode) {
                flag = mPrefs.getInt(PREF_MODE_HCE, NfcSettingsAdapter.FLAG_ON);
            }
        }
        Log.d(TAG, "getModeFlag() - return = " + flag);
        return flag;
    }

    // Nfc Framework API
    public void setModeFlag(boolean isNfcEnabled, int mode, int flag, Object syncObj) {
        Log.d(
                TAG,
                "setModeFlag() - isNfcEnabled = "
                        + isNfcEnabled
                        + ", mode = "
                        + mode
                        + ", flag = "
                        + flag
                        + ", syncObj = "
                        + syncObj);

        // Retrieve current RfConfiguration
        byte[] techArray = new byte[4];
        int modeBitmap = mStExtensions.getRfConfiguration(techArray);

        synchronized (syncObj) {
            if ((mode
                                    > (NfcSettingsAdapter.MODE_READER
                                            | NfcSettingsAdapter.MODE_P2P
                                            | NfcSettingsAdapter.MODE_HCE)
                            || mode < 0)
                    || (flag != NfcSettingsAdapter.FLAG_ON
                            && flag != NfcSettingsAdapter.FLAG_OFF)) {
                Log.d(
                        TAG,
                        "setModeFlag() - incorrect mode:" + mode + " or flag:" + flag + ", return");
                return;
            }

            if ((mode & NfcSettingsAdapter.MODE_READER) != 0) {
                mPrefsEditor.putInt(PREF_MODE_READER, flag);
                mPrefsEditor.apply();

                if (flag == NfcSettingsAdapter.FLAG_OFF) {
                    modeBitmap &= ~0x1;
                } else {
                    modeBitmap |= 0x1;
                }
            }

            if ((mode & NfcSettingsAdapter.MODE_P2P) != 0) {
                mPrefsEditor.putInt(PREF_MODE_P2P, flag);
                mPrefsEditor.apply();

                if (flag == NfcSettingsAdapter.FLAG_OFF) {
                    modeBitmap &= ~0xC;
                } else {
                    modeBitmap |= 0xC;
                }
            }

            if ((mode & NfcSettingsAdapter.MODE_HCE) != 0) {
                mPrefsEditor.putInt(PREF_MODE_HCE, flag);
                mPrefsEditor.apply();

                if (flag == NfcSettingsAdapter.FLAG_OFF) {
                    modeBitmap &= ~0x2;
                } else {
                    modeBitmap |= 0x2;
                }
            }

            Log.d(TAG, "setModeFlag() - modeBitmap = " + Integer.toHexString(modeBitmap));

            if (isNfcEnabled) {
                Log.d(TAG, "setModeFlag() - Ready for ApplyPollingLoopThread");

                mStExtensions.setRfConfiguration(modeBitmap, techArray);
            }
        }
    }
}
