package com.android.nfc.st;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.SystemProperties;
import android.util.Log;
import com.android.nfc.dhimpl.NativeNfcStExtensions;
import com.android.nfc.dhimpl.StNativeNfcManager;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

interface ISeController {
    public boolean init();

    public void deinit(boolean deselect);

    public int getActiveUiccValue();

    public void onActiveUiccChangedByUser(int SeID);

    public boolean EnableSecureElement(String str, boolean enable);

    // MTK - start
    // public List<String> getSecureElementsStatus();
    public List<String> getSecureElementsStatus(boolean skipRefresh);
    // MTK - end

    interface Callback {
        public void onSecureElementStatusChanged();
        // MTK - start
        public void onSecureElementChangeProgress(boolean started);
        // MTK - end
    }
}

public class SecureElementSelector implements ISeController, NfcSimStateObserver.SimEventListener {
    private static final String TAG = "Nfc_SecureElementSelector";
    static final boolean DBG = true;
    // static private final String PREF_BACKUP_SE = "backupSeString";
    // static private final String PREF_BACKUP_SE_NONE = "none";
    public static final String PREF = "NfcServicePrefs";

    // MTK - start
    public static final boolean HAS_MTK_SETTINGS =
            ("1".equals(SystemProperties.get("ro.vendor.mtk_nfc_addon_support"))
                    && !"1".equals(SystemProperties.get("persist.st_nfc_ignore_addon_support")));
    // MTK - end

    // static private final String CFG_FILE_PATH = "system/etc/nfcse.cfg";
    // static private final String CFG_FILE_RULES[] = {
    //         "SWP1=1:SIM1=1,SIM2=2",
    //         "SWP2=2:SIM1=1,SIM2=2",
    //         "SD=3:NO=0,YES=1",
    //         "ESE=4:NO=0,YES=1",
    // };

    // static public final int USER_OFF = 0;
    // static public final int USER_SIM1 = 1;
    // static public final int USER_SIM2 = 2;
    // static public final int USER_SSD = 3;
    // static public final int USER_ESE = 4;

    // static public final int CHIP_OFF = 0;
    // static public final int CHIP_SWP1 = 1;
    // static public final int CHIP_SWP2 = 2;
    // static public final int CHIP_SSD_ESE = 3;

    /// TODO: move to setting later
    public static final String NFC_MULTISE_ON = "nfc_multise_on";
    // value type: int,0 for Off, 1 for on
    public static final String NFC_MULTISE_LIST = "nfc_multise_list"; // SIM1,SIM2,Smart SD
    public static final String NFC_USER_DESIRED_SE = "nfc_user_desired_se";

    public static final String SETTING_STR_SIM1 = "SIM1";
    public static final String SETTING_STR_SIM2 = "SIM2";
    public static final String SETTING_STR_ESE = "eSE";
    public static final String SETTING_STR_DHSE = "DHSE";
    public static final String SETTING_STR_OFF = "Off";

    public static final String SE_STATE_ACTIVATED = "Active";
    public static final String SE_STATE_AVAILABLE = "Available";
    public static final String SE_STATE_NOT_AVAILABLE = "N/A";

    public static final String SE_STATE_UNKNOWN = "?"; // not returned outside this file

    // static final private String DEFAULT_SE_NAME = SETTING_STR_SIM1;
    // static final private String ACTION_FAIL_TO_SELECT_SE
    //         = "android.nfc.action.SWITCH_FAIL_DIALOG_REQUEST";
    // static final private String ACTION_NOT_NFC_SIM
    //         = "android.nfc.action.NOT_NFC_SIM_DIALOG_REQUEST";
    // static final private String ACTION_NOT_NFC_TWO_SIM
    //         = "android.nfc.action.NOT_NFC_TWO_SIM_DIALOG_REQUEST";
    // static final private String EXTRA_WHAT_SIM = "android.nfc.extra.WHAT_SIM";
    // static final private String EMBEDDED_SE_READY = "android.nfc.EMBEDDED_SE_READY";

    // User Desired SE
    static final String PREF_ESE_ACTIVATION_STATUS = "nfc.pref.activation.ese";
    static final String PREF_DHSE_ACTIVATION_STATUS = "nfc.pref.activation.dhse";
    static final String PREF_UICC1_ACTIVATION_STATUS = "nfc.pref.activation.uicc1";
    static final String PREF_UICC2_ACTIVATION_STATUS = "nfc.pref.activation.uicc2";

    // static final private int NOTIFY_SELECTION_FAIL = 0;
    // static final private int NOTIFY_SIM1_NOT_NFC = 1;
    // static final private int NOTIFY_SIM2_NOT_NFC = 2;
    // static final private int NOTIFY_SIM1_SIM2_NOT_NFC = 3;

    // private static final int MSG_TIMEOUT = 1;
    // private static final int MSG_SWP_SIM1_RETRY = 2;
    // private static final int MSG_SWP_SIM2_RETRY = 3;

    private Context mContext;
    private ISeController.Callback mCallback;
    private Object mSyncLock;
    private int mStActiveUicc;
    private int mStActiveeSe;
    private StNativeNfcManager mNativeNfcManager;
    private MyContentProviderUtil mContentProviderUtil;
    // private ConfigUtil.IParser mConfigFileParser;
    NativeNfcStExtensions mStExtensions;
    private boolean mIsMultiSeSupported = false;
    // static private boolean mShouldSimWarningDialogPopup[] = new boolean[2];
    // private Handler mHandler;

    private static final Map<Integer, String> SeList = new HashMap<Integer, String>();

    private int mSim1Presence = NfcSimStateObserver.STATE_UNKNOWN;
    private int mSim2Presence = NfcSimStateObserver.STATE_UNKNOWN;

    private boolean init = true;
    private boolean init_done = false;
    // private boolean isFirstTime = false;
    private SharedPreferences mPrefs;
    private SharedPreferences.Editor mPrefsEditor;

    // If desired SIM is not avaialbe but other SIM is, autoswitch.
    private boolean fallbackSimIfNeeded() {
        boolean changed = false;

        // Don t make change if both SIM are not in a stable presence state.
        if (!(simPresenceStableForSwp(mSim1Presence) && simPresenceStableForSwp(mSim2Presence))) {
            return false;
        }

        // Check desired
        int wanted = getUserDesiredUiccId();

        // Is the desired SIM not available ? (i.e. absent or not SWP)
        if (SE_STATE_NOT_AVAILABLE.equals(SeList.get(wanted))) {
            // Check if alternate SIM is available.
            for (Map.Entry<Integer, String> entry : SeList.entrySet()) {
                if (((wanted == 0x81) && (entry.getKey() == getSim2Id()))
                        || ((entry.getKey() == 0x81) && (wanted == getSim2Id()))) {
                    Log.d(TAG, "fallbackSimIfNeeded() - alt SIM is " + entry.getValue());
                    if (SE_STATE_AVAILABLE.equals(entry.getValue())) {
                        // Switch to that one, don t save preference.
                        onActiveUiccChanged(entry.getKey(), false);
                        changed = true;
                    }
                }
            }
        }
        return changed;
    }

    private boolean simPresenceStableForSwp(int presence) {
        if ((presence == NfcSimStateObserver.SimEventListener.ABSENT)
                || (presence == NfcSimStateObserver.SimEventListener.PRESENT_READY)) {
            return true;
        }
        return false;
    }

    private String getSim1State() {
        if (SeList.get(0x81) != null) {
            return SeList.get(0x81);
        }
        return SE_STATE_UNKNOWN;
    }

    private String getSim2State() {
        if (SeList.get(0x83) != null) {
            return SeList.get(0x83);
        }
        if (SeList.get(0x85) != null) {
            return SeList.get(0x85);
        }
        return SE_STATE_UNKNOWN;
    }

    private String getESeState() {
        if (SeList.get(0x82) != null) {
            return SeList.get(0x82);
        }
        if (SeList.get(0x86) != null) {
            return SeList.get(0x86);
        }
        return SE_STATE_UNKNOWN;
    }

    private String getDHSEState() {
        if (SeList.get(0x84) != null) {
            return SeList.get(0x84);
        }
        return SE_STATE_UNKNOWN;
    }

    private boolean st21HasSim2() {
        return (SeList.get(0x83) != null) || (SeList.get(0x85) != null);
    }

    private int getSim2Id() {
        if (SeList.containsKey(0x85)) {
            return 0x85;
        }
        return 0x83;
    }

    public void onSimStateChanged(int simId, int newPresenceState) {
        // Presence information of the SIM is updated from Telephony stack

        String currentState = SE_STATE_UNKNOWN;
        int currentId = 0x00;
        int prevPresence = NfcSimStateObserver.STATE_UNKNOWN;
        boolean changed = false;
        Log.d(
                TAG,
                "onSimStateChanged() - simId = "
                        + simId
                        + " newPresenceState = "
                        + newPresenceState
                        + " mUICC1State: "
                        + getSim1State()
                        + " mUICC2State: "
                        + getSim2State()
                        + " airplane: "
                        + NfcSimStateObserver.getInstance().isAirplaneModeHidingSimPresence());

        if (NfcSimStateObserver.getInstance().isAirplaneModeHidingSimPresence()) {
            Log.d(TAG, "onSimStateChanged() - Update ignored as we are in airplane mode");
            return;
        }

        if ((!st21HasSim2()) && (simId == NfcSimStateObserver.SimEventListener.SIM2)) {
            Log.d(TAG, "onSimStateChanged() - SIM2 update ignored as NFC has only 1 SIM connected");
            return;
        }

        if ((simId != NfcSimStateObserver.SimEventListener.SIM1)
                && (simId != NfcSimStateObserver.SimEventListener.SIM2)) {
            Log.d(TAG, "onSimStateChanged() - simId not supported " + simId);
            return;
        }

        if (simId == NfcSimStateObserver.SimEventListener.SIM1) {
            prevPresence = mSim1Presence;
            mSim1Presence = newPresenceState;
            currentId = mContentProviderUtil.getIdofString(SETTING_STR_SIM1);
            currentState = getSim1State();
        } else {
            prevPresence = mSim2Presence;
            mSim2Presence = newPresenceState;
            currentId = mContentProviderUtil.getIdofString(SETTING_STR_SIM2);
            currentState = getSim2State();
        }
        if (prevPresence == newPresenceState) {
            Log.d(TAG, "onSimStateChanged() - presence state unchanged");
            return;
        }

        if (prevPresence == NfcSimStateObserver.SimEventListener.ABSENT) {
            // The SIM was N/A because of absent, now it is not absent anymore.
            // We set Available until it will be further refined.
            mNativeNfcManager.NfceeDiscover();
            RefreshSeStatus(true);
        }

        if (newPresenceState == NfcSimStateObserver.SimEventListener.PRESENT_BUSY) {
            // SIM is inserted, but we don t apply any change for SWP state at the moment.
            return;
        }

        // If we are in init phase, wait for both SIM status to be stable.
        if ((init == true)
                && simPresenceStableForSwp(mSim1Presence)
                && ((!st21HasSim2()) || simPresenceStableForSwp(mSim2Presence))) {
            synchronized (mSyncLock) { // do not allow this to happen at the same time as user
                // action on the active SIM card.
                // MTK - start
                mCallback.onSecureElementChangeProgress(true);
                // MTK - end
                GetUiccAvailability(true);
                restorePreviousSe();
                changed = true;
                // mContentProviderUtil.setAvailableSeList();
                init = false;
            }
        } else if (init == false) {
            if (newPresenceState == NfcSimStateObserver.SimEventListener.ABSENT) {
                // SIM has been removed.
                if (currentState.equals(SE_STATE_ACTIVATED)) {
                    // MTK - start
                    mCallback.onSecureElementChangeProgress(true);
                    // MTK - end
                    mStExtensions.EnableSE(currentId, false);
                    changed = true;
                }
            } else if (newPresenceState == NfcSimStateObserver.SimEventListener.PRESENT_READY) {
                // SIM is inserted and we can activate SWP if we want.
                int wanted = getUserDesiredUiccId();
                if ((wanted == currentId) && (!currentState.equals(SE_STATE_ACTIVATED))) {
                    // The SIM selected by user has been inserted, activate it (if no other element
                    // is active).
                    int ActSe = getActiveUiccValue();
                    if (ActSe == 0) {
                        // MTK - start
                        mCallback.onSecureElementChangeProgress(true);
                        // MTK - end
                        // Now the alt SIM will appear as available even if it does not support SWP
                        // we try to enable the alt SIM first to avoid this.
                        if (wanted == 0x81) {
                            int alt = getSim2Id();
                            if (true == mStExtensions.EnableSE(alt, true)) {
                                mStExtensions.EnableSE(alt, false);
                            }
                        } else {
                            if (true == mStExtensions.EnableSE(0x81, true)) {
                                mStExtensions.EnableSE(0x81, false);
                            }
                        }
                        mStExtensions.EnableSE(currentId, true);
                        changed = true;
                    } else {
                        // There is a SIM currently activated, but it is not the wanted one
                        // ==> switch to wanted one
                        onActiveUiccChanged(wanted, false);
                        changed = true;
                    }
                }
            }
        }

        RefreshSeStatus(false);
        if (newPresenceState != NfcSimStateObserver.SimEventListener.ABSENT) {
            // Don t fallback when SIM is removed as we are notified of the two removals in
            // sequence.
            changed |= fallbackSimIfNeeded();
        }

        // Refresh routing table
        if (changed) {
            //            NfcService.getInstance().setRoutingTableDirty();
            //            NfcService.getInstance().commitRouting();
            //            NfcService.getInstance().updateRoutingTable();
            //            SetForceRoutingIfNeeded();
            Log.d(TAG, "onSimStateChanged() - SWP activation status was changed.");
            // MTK - start
            mCallback.onSecureElementChangeProgress(false);
            // MTK - end
        }

        // Refresh the Settings UI information
        broadcastSecureElementStatusChanged();

        // mContentProviderUtil.setAvailableSeList();
    }

    public SecureElementSelector(
            Context context,
            StNativeNfcManager nativeNfcManager,
            NativeNfcStExtensions stExtensions,
            Object syncLock,
            ISeController.Callback callback) {
        mContext = context;
        mCallback = callback;
        mNativeNfcManager = nativeNfcManager;
        mSyncLock = syncLock;
        Log.d(TAG, "SecureElementSelector constructor ");
        NfcSimStateObserver.createSingleton(context);
        // Shared Pref
        mPrefs = mContext.getSharedPreferences(PREF, Context.MODE_PRIVATE);
        mPrefsEditor = mPrefs.edit();

        mContentProviderUtil = new MyContentProviderUtil();
        mIsMultiSeSupported = true;
        // SeList = new HashMap<Integer, Integer>();
        mStExtensions = stExtensions;
        // mContentProviderUtil.setAvailableSeList();
    }

    public boolean init() {

        if (mIsMultiSeSupported) {
            Log.d(TAG, "init()");
            boolean result = false;
            init = true;
            init_done = false;
            mSim1Presence = NfcSimStateObserver.getInstance().GetSimState(0);
            mSim2Presence = NfcSimStateObserver.getInstance().GetSimState(1);

            // MTK - start
            // if (SystemProperties.get("persist.st_nfc_ignore_modem").equals("1")) {
            if (SystemProperties.get("persist.vendor.st_nfc_ignore_modem").equals("1")) {
                // MTK - end
                mSim1Presence = NfcSimStateObserver.STATE_PRESENT_READY;
                mSim2Presence = NfcSimStateObserver.STATE_PRESENT_READY;
                Log.d(
                        TAG,
                        "init() - persist.st_nfc_ignore_modem=1 ==> ignore all SIM state from modem "
                                + "for certif");
            }

            // MTK - start
            mCallback.onSecureElementChangeProgress(true);
            // MTK - end
            mNativeNfcManager.NfceeDiscover();
            RefreshSeStatus(true);

            if (!st21HasSim2()) {
                mSim2Presence = NfcSimStateObserver.STATE_ABSENT;
            }

            if ((simPresenceStableForSwp(mSim1Presence) && simPresenceStableForSwp(mSim2Presence))
                    || NfcSimStateObserver.getInstance().isAirplaneModeHidingSimPresence()) {
                init = false;
                GetUiccAvailability(true);
                GeteSEAvailability();

                result = restorePreviousSe();
                RefreshSeStatus(false);
                // mContentProviderUtil.setAvailableSeList();
                fallbackSimIfNeeded();
            } else {
                GeteSEAvailability();
                result = restorePreviousSe();
                RefreshSeStatus(false);
            }

            // MTK - start
            // if (!SystemProperties.get("persist.st_nfc_ignore_modem").equals("1")) {
            if (!SystemProperties.get("persist.vendor.st_nfc_ignore_modem").equals("1")) {
                // MTK - end
                NfcSimStateObserver.getInstance().registerSimEventListener(this);
            }
            init_done = true;
            // MTK - start
            mCallback.onSecureElementChangeProgress(false);
            // MTK - end
            return result;
        }
        init_done = true;
        return true;
    }

    /* If skipactive is true, don t try and activate the PreviousSe because caller will do it
    shortly */
    private void GetUiccAvailability(boolean skipactive) {
        int toskip = 0x00, toskipAlt = 0x00;
        if (DBG) Log.d(TAG, "GetUiccAvailability() ");
        if (skipactive) {
            String seStr = mContentProviderUtil.getUserDesiredUiccString();
            if (seStr != null) {
                if (SETTING_STR_SIM1.equals(seStr)) {
                    toskip = 0x81;
                } else if (SETTING_STR_SIM2.equals(seStr)) {
                    toskip = 0x83;
                    toskipAlt = 0x85;
                }
            } else {
                toskip = 0x81;
            }
        }

        for (Map.Entry<Integer, String> entry : SeList.entrySet()) {
            Log.d(
                    TAG,
                    "GetUiccAvailability() - SeID = "
                            + String.format("0x%02X", entry.getKey())
                            + " Status = "
                            + entry.getValue()
                            + " skip = "
                            + String.format("0x%02X", toskip));

            if ((entry.getKey() != toskip)
                    && (entry.getKey() != toskipAlt)
                    && (SE_STATE_AVAILABLE.equals(entry.getValue()))) {
                if (((entry.getKey() == 0x81)
                                && (mSim1Presence == NfcSimStateObserver.STATE_PRESENT_READY))
                        || (((entry.getKey() == 0x83) || (entry.getKey() == 0x85))
                                && (mSim2Presence == NfcSimStateObserver.STATE_PRESENT_READY))) {
                    if (true == mStExtensions.EnableSE(entry.getKey(), true)) {
                        mStExtensions.EnableSE(entry.getKey(), false);
                    }
                } else if (NfcSimStateObserver.getInstance().isAirplaneModeHidingSimPresence()) {
                    if (true == mStExtensions.EnableSE(entry.getKey(), true)) {
                        mStExtensions.EnableSE(entry.getKey(), false);
                    }
                }
            }
        }
    }

    /* skipactive is not used for SE because it is not a problem to activate / deactivate
    the SWP line at startup for eSE (this creates issue for SWP/HCI tests) */
    private void GeteSEAvailability() {
        if (DBG) Log.d(TAG, "GeteSEAvailability() ");

        for (Map.Entry<Integer, String> entry : SeList.entrySet()) {
            Log.d(
                    TAG,
                    "GeteSEAvailability() - SeID = "
                            + String.format("0x%02X", entry.getKey())
                            + " Status = "
                            + entry.getValue());

            if (SE_STATE_AVAILABLE.equals(entry.getValue())) {
                if ((entry.getKey() == 0x82)
                        || (entry.getKey() == 0x84)
                        || (entry.getKey() == 0x86)) {
                    if (true == mStExtensions.EnableSE(entry.getKey(), true)) {
                        mStExtensions.EnableSE(entry.getKey(), false);
                    }
                }
            }
        }
    }

    private int getUserDesiredUiccId() {
        int desired;
        String seStr = mContentProviderUtil.getUserDesiredUiccString();
        if ((seStr == null) || (seStr.equals(SETTING_STR_SIM1))) {
            desired = 0x81;
        } else if (seStr.equals(SETTING_STR_SIM2)) {
            desired = getSim2Id();
        } else {
            desired = 0x0;
        }
        Log.d(TAG, "getUserDesiredSeId() - " + String.format("0x%02X", desired));
        return desired;
    }

    private boolean restorePreviousSe() {
        Log.d(TAG, "restorePreviousSe() - Entry");

        boolean isActiveSeChanged = false;
        String DefaultSe;
        int DesiredeSE = 0;
        int DesiredUicc = 0;
        String UiccStr = mContentProviderUtil.getUserDesiredUiccString();
        String eSEStr = mContentProviderUtil.getUserDesiredeSEString();
        Log.d(TAG, "restorePreviousSe() - UiccStr = " + UiccStr + " eSEStr= " + eSEStr);
        if (UiccStr == null && eSEStr == null) {
            // isFirstTime = true;
            // MTK - start
            // DefaultSe = SystemProperties.get("persist.st_nfc_defaut_se");
            DefaultSe = SystemProperties.get("persist.vendor.st_nfc_defaut_se");
            // MTK - end
            if (SETTING_STR_ESE.equals(DefaultSe)) {
                eSEStr = SETTING_STR_ESE;
                UiccStr = SETTING_STR_OFF;
            } else if (SETTING_STR_DHSE.equals(DefaultSe)) {
                eSEStr = SETTING_STR_DHSE;
                UiccStr = SETTING_STR_OFF;
            } else if (SETTING_STR_SIM2.equals(DefaultSe)) {
                UiccStr = SETTING_STR_SIM2;
                eSEStr = SETTING_STR_OFF;
            } else {
                // other cases, default to SIM1
                UiccStr = SETTING_STR_SIM1;
                eSEStr = SETTING_STR_ESE;
                // MTK - start
                // if MTK settings are enabled, we don t activate both SE
                if (HAS_MTK_SETTINGS) {
                    eSEStr = SETTING_STR_OFF;
                }
                // MTK - end
            }
            Log.d(
                    TAG,
                    "restorePreviousSe() - no previous record, default '"
                            + DefaultSe
                            + "', set UICC = "
                            + UiccStr
                            + ", eSE = "
                            + eSEStr);
            mContentProviderUtil.setUserDesiredeSEString(eSEStr);
            mContentProviderUtil.setUserDesiredUiccString(UiccStr);
        }

        if (SETTING_STR_SIM1.equals(UiccStr)) {
            DesiredUicc = 0x81;
        } else if (SETTING_STR_SIM2.equals(UiccStr)) {
            DesiredUicc = getSim2Id();
        }
        if (SETTING_STR_ESE.equals(eSEStr)) {
            DesiredeSE = 0x82;
            if (SeList.get(0x86) != null) {
                DesiredeSE = 0x86;
            }
        } else if (SETTING_STR_DHSE.equals(eSEStr)) {
            DesiredeSE = 0x84;
        }
        Log.d(
                TAG,
                "restorePreviousSe() - DesiredUicc = "
                        + String.format("0x%02X", DesiredUicc)
                        + "- DesiredeSE = "
                        + String.format("0x%02X", DesiredeSE));
        boolean ret = false;
        if ((DesiredUicc != 0x0)
                && ((simPresenceStableForSwp(mSim1Presence)
                                && simPresenceStableForSwp(mSim2Presence))
                        || NfcSimStateObserver.getInstance().isAirplaneModeHidingSimPresence())) {
            onActiveUiccChanged(DesiredUicc, false);
        }
        if (DesiredeSE != 0x0) {
            mStExtensions.EnableSE(DesiredeSE, true);
            RefreshSeStatus(false);
        }
        // mContentProviderUtil.setAvailableSeList();

        return ret;
    }

    public void deinit(boolean deselect) {
        if (mIsMultiSeSupported) {
            Log.d(TAG, "deinit()");
            // MTK - start
            // if (!SystemProperties.get("persist.st_nfc_ignore_modem").equals("1")) {
            if (!SystemProperties.get("persist.vendor.st_nfc_ignore_modem").equals("1")) {
                // MTK - end
                NfcSimStateObserver.getInstance().unregisterSimEventListener();
            }
            if (deselect) {
                // mNativeNfcManager.deselectSecureElement();
            }
        }
    }

    public void onActiveUiccChangedByUser(int SeID) {
        onActiveUiccChanged(SeID, true);
    }

    private void onActiveUiccChanged(int SeID, boolean storePreference) {
        Log.d(
                TAG,
                "onActiveUiccChanged("
                        + storePreference
                        + ") - SeID = "
                        + Integer.toHexString(SeID));
        synchronized (mSyncLock) {
            {
                int ActSe, ActSeNew;
                int errors = 0;
                // Set the User Desired Uicc
                if (storePreference) {
                    mContentProviderUtil.setUserDesiredUiccString(
                            mContentProviderUtil.getStringofId(SeID));
                }

                ActSe = getActiveUiccValue();

                // If SeId =0 (OFF), disable all the enabled SEs.
                if (SeID == 0) {
                    for (Map.Entry<Integer, String> entry : SeList.entrySet()) {
                        Log.d(
                                TAG,
                                "onActiveUiccChangedByUser() - SeID = "
                                        + entry.getKey()
                                        + " Status = "
                                        + entry.getValue());
                        if ((entry.getValue().equals(SE_STATE_ACTIVATED))
                                && (entry.getKey() != 0x82)
                                && (entry.getKey() != 0x84)
                                && (entry.getKey() != 0x86)) {
                            if (false == mStExtensions.EnableSE(entry.getKey(), false)) errors++;
                        }
                    }

                    if (errors > 0) { // this may happen if SIM card was ejected when CE was active.
                        mNativeNfcManager.NfceeDiscover();
                        RefreshSeStatus(true);
                    }
                } else if (ActSe != SeID) {

                    Log.d(
                            TAG,
                            "onActiveUiccChangedByUser() - Disable "
                                    + Integer.toHexString(ActSe)
                                    + " Activate "
                                    + Integer.toHexString(SeID)
                                    + " ("
                                    + SeList.get(SeID)
                                    + ")");

                    if (!SE_STATE_NOT_AVAILABLE.equals(SeList.get(SeID))) {
                        if (ActSe != 0) {
                            if (false
                                    == mStExtensions.EnableSE(
                                            ActSe, false)) { // Disable the activated SE
                                mNativeNfcManager.NfceeDiscover();
                                RefreshSeStatus(true);
                            }
                        }
                        if (false == mStExtensions.EnableSE(SeID, true)) { // Enable the desired SE
                            Log.d(
                                    TAG,
                                    "onActiveUiccChangedByUser() - failed to activate "
                                            + Integer.toHexString(SeID));
                        }
                    }

                } else { // ActSe == SeID != 0
                    Log.d(
                            TAG,
                            "onActiveUiccChangedByUser() - reactivate request for same EE, force "
                                    + "discover "
                                    + Integer.toHexString(SeID));
                    mNativeNfcManager.NfceeDiscover();
                    mStExtensions.EnableSE(SeID, true);
                }
                ActSeNew = getActiveUiccValue();

                if ((ActSeNew != ActSe) && init_done == true) {
                    // NfcService.getInstance().commitRouting();
                    //                    NfcService.getInstance().updateRoutingTable();
                    // SetForceRoutingIfNeeded();
                    broadcastSecureElementStatusChanged();
                }
                if (ActSeNew == SeID) {
                    Log.d(
                            TAG,
                            "onActiveUiccChangedByUser() - ActSeNew = "
                                    + String.format("0x%02X", ActSeNew));
                } else {
                    // showAlternativeSeDialog(SeID, 0x0);

                }
            }
        }
    }

    /* public void SetForceRoutingIfNeeded() {
        if (DBG) Log.d(TAG, "SetForceRoutingIfNeeded() ");

        boolean isHceOn = false;

        //        ContentResolver mContentResolver = mContext.getContentResolver();
        //        int hceFlag = Settings.Global.getInt(mContentResolver, "nfc_hce_on", 0);

        //        int hceFlag = mPrefs.getInt(NfcService.PREF_MODE_HCE, 1);

        int hceFlag = NfcAddonWrapper.getInstance().getModeFlag(NfcSettingsAdapter.MODE_HCE, this);
        if (hceFlag == 1) {
            isHceOn = true;
        }
        if (!isHceOn) {
            int Active_Se = getActiveUiccValue();
            if (Active_Se != 0x0) {
                mNativeNfcManager.forceRouting(Active_Se);
            } else {
                mNativeNfcManager.stopforceRouting();
            }
        }
    }*/

    public boolean EnableSecureElement(String str, boolean enable) {
        if (DBG) Log.d(TAG, "EnableSecureElement() - Id: " + str + ", enable: " + enable);

        boolean status = false;
        int mPreviousActiveeSe = 0;
        int DesiredUiccId = 0;
        // MTK - start
        mCallback.onSecureElementChangeProgress(true);
        // MTK - end
        if (str.equals(SETTING_STR_ESE) || str.equals(SETTING_STR_DHSE)) {
            RefreshSeStatus(false);
            mPreviousActiveeSe = mStActiveeSe;

            // MTK - start
            if (HAS_MTK_SETTINGS && enable == true) {
                // make sure SIM is off to ensure we keep only 1 EE active
                onActiveUiccChangedByUser(DesiredUiccId);
            }
            // MTK - end

            int targetSE = mContentProviderUtil.getIdofString(str);

            if (enable == false && mStActiveeSe == targetSE) {
                mContentProviderUtil.setUserDesiredeSEString(SETTING_STR_OFF);
                status = mStExtensions.EnableSE(targetSE, false);
            } else if (enable == true && mStActiveeSe != targetSE) {
                mContentProviderUtil.setUserDesiredeSEString(str);
                if (mStActiveeSe != 0) mStExtensions.EnableSE(mStActiveeSe, false);
                status = mStExtensions.EnableSE(targetSE, true);
            }
            RefreshSeStatus(false);
            if (mStActiveeSe != mPreviousActiveeSe) {
                // NfcService.getInstance().commitRouting();
                //                NfcService.getInstance().updateRoutingTable();
                // SetForceRoutingIfNeeded();
                broadcastSecureElementStatusChanged();
            }
        } else if (str.equals(SETTING_STR_SIM1) || str.equals(SETTING_STR_SIM2)) {
            if (enable == true) DesiredUiccId = mContentProviderUtil.getIdofString(str);

            onActiveUiccChangedByUser(DesiredUiccId);
            if (mContentProviderUtil.getIdofString(str) == getActiveUiccValue()) status = true;
        }
        // MTK - start
        mCallback.onSecureElementChangeProgress(false);
        // MTK - end
        return status;
    }

    private String nciStateToString(int state) {
        if (state == 0) {
            return SE_STATE_ACTIVATED;
        } else if (state == 1) {
            return SE_STATE_AVAILABLE;
        } else if (state == 2) {
            return SE_STATE_NOT_AVAILABLE;
        } else {
            return SE_STATE_UNKNOWN;
        }
    }

    private void RefreshSeStatus(boolean initial) {

        byte[] nfceeid = new byte[3];
        byte[] conInfo = new byte[3];
        int eSeId = 0x00;
        mStActiveUicc = 0;
        mStActiveeSe = 0;
        int num = mStExtensions.getAvailableHciHostList(nfceeid, conInfo);

        Log.d(
                TAG,
                "RefreshSeStatus(start, "
                        + initial
                        + ") - "
                        + "nfceeid[0]: "
                        + String.format("0x%02X", nfceeid[0])
                        + " conInfo[0]: "
                        + String.format("0x%02X", conInfo[0])
                        + ", nfceeid[1]: "
                        + String.format("0x%02X", nfceeid[1])
                        + " conInfo[1]: "
                        + String.format("0x%02X", conInfo[1]));

        if (!NfcSimStateObserver.getInstance().isAirplaneModeHidingSimPresence()) {
            Log.d(
                    TAG,
                    "RefreshSeStatus() - not in Airplane mode, so using presence information SIM1="
                            + String.format("0x%02X", mSim1Presence)
                            + ", SIM2="
                            + String.format("0x%02X", mSim2Presence));
        }

        if (initial) {
            // Called after a NfceeDiscover, nfceeid / connInfo contain information about all EE
            SeList.clear();
        } else {
            // nfceeid / connInfo only contain EEs in state 0 or 1, the unavailble ones are not
            // listed.
            for (Map.Entry<Integer, String> entry : SeList.entrySet()) {
                SeList.put(entry.getKey(), SE_STATE_NOT_AVAILABLE);
            }
        }

        int i;
        for (i = 0; i < num; i++) {
            String s = nciStateToString(conInfo[i] & 0xFF);
            if (!NfcSimStateObserver.getInstance().isAirplaneModeHidingSimPresence()) {
                if (((nfceeid[i] & 0xFF) == 0x81)
                        && (mSim1Presence == NfcSimStateObserver.STATE_ABSENT)) {
                    s = SE_STATE_NOT_AVAILABLE;
                } else if ((((nfceeid[i] & 0xFF) == 0x83) || ((nfceeid[i] & 0xFF) == 0x85))
                        && (mSim2Presence == NfcSimStateObserver.STATE_ABSENT)) {
                    s = SE_STATE_NOT_AVAILABLE;
                }
            }
            SeList.put((nfceeid[i] & 0xFF), s);
            if (SE_STATE_ACTIVATED.equals(s)) {
                if (((nfceeid[i] & 0xFF) == 0x82)
                        || ((nfceeid[i] & 0xFF) == 0x84)
                        || ((nfceeid[i] & 0xFF) == 0x86)) {
                    mStActiveeSe = (nfceeid[i] & 0xFF);
                } else {
                    if (mStActiveUicc == 0) {
                        mStActiveUicc = (nfceeid[i] & 0xFF);
                    } else {
                        mStActiveUicc = 0xFF;
                    }
                }
            }
        }
        Log.d(
                TAG,
                "RefreshSeStatus() - mStActiveeSe = "
                        + String.format("0x%02X", mStActiveeSe)
                        + " mStActiveUicc = "
                        + String.format("0x%02X", mStActiveUicc));

        Log.d(
                TAG,
                "RefreshSeStatus(end) - mUICC1State: "
                        + getSim1State()
                        + ", mUICC2State: "
                        + getSim2State()
                        + ", meSEState: "
                        + getESeState()
                        + ", mDHSEState: "
                        + getDHSEState());
    }

    public int getActiveUiccValue() {
        RefreshSeStatus(false);
        Log.d(TAG, "getActiveUiccValue() -  mStActiveUicc= " + Integer.toHexString(mStActiveUicc));
        return mStActiveUicc;
    }

    // MTK - start
    // public List<String> getSecureElementsStatus() {
    public List<String> getSecureElementsStatus(boolean skipRefresh) {
        // MTK - end
        List<String> ret = new ArrayList<String>();
        Log.d(TAG, "getSecureElementsStatus()");
        // MTK - start
        if (!skipRefresh) {
            RefreshSeStatus(false);
        }
        // MTK - end

        String s;

        // Add SIM1 information first :
        s = SeList.get(0x81);
        if (s != null) {
            ret.add(SETTING_STR_SIM1 + ":" + s);
        }

        // then SIM2 if any
        s = SeList.get(0x83);
        if (s != null) {
            ret.add(SETTING_STR_SIM2 + ":" + s);
        }
        s = SeList.get(0x85);
        if (s != null) {
            ret.add(SETTING_STR_SIM2 + ":" + s);
        }

        // finally ESE. As it is not removable, only show when at least available
        s = SeList.get(0x82);
        if (s != null && !SE_STATE_NOT_AVAILABLE.equals(s)) {
            ret.add(SETTING_STR_ESE + ":" + s);
        }
        s = SeList.get(0x86);
        if (s != null && !SE_STATE_NOT_AVAILABLE.equals(s)) {
            ret.add(SETTING_STR_ESE + ":" + s);
        }
        s = SeList.get(0x84);
        if (s != null && !SE_STATE_NOT_AVAILABLE.equals(s)) {
            ret.add(SETTING_STR_DHSE + ":" + s);
        }

        // return the list
        return ret;
    }

    private void broadcastSecureElementStatusChanged() {
        mCallback.onSecureElementStatusChanged();
    }

    private class MyContentProviderUtil {
        public MyContentProviderUtil() {}

        public int getIdofString(String str) {

            int id = 0;
            if (str.equals(SETTING_STR_SIM1)) {
                id = 0x81;
            } else if (str.equals(SETTING_STR_SIM2)) {
                id = getSim2Id();
            } else if (str.equals(SETTING_STR_ESE)) {
                id = 0x82;
                if (SeList.get(0x86) != null) {
                    id = 0x86;
                }
            } else if (str.equals(SETTING_STR_DHSE)) {
                id = 0x84;
            }

            Log.d(TAG, "getIdofString() - str = " + str + " id = " + Integer.toHexString(id));
            return id;
        }

        public String getStringofId(int id) {

            String str = "";
            if (id == 0x81) {
                str = SETTING_STR_SIM1;
            } else if ((id == 0x82) || (id == 0x86)) {
                str = SETTING_STR_ESE;
            } else if (id == 0x84) {
                str = SETTING_STR_DHSE;
            } else if ((id == 0x83) || (id == 0x85)) {
                str = SETTING_STR_SIM2;
            } else {
                str = SETTING_STR_OFF;
            }

            return str;
        }

        /*
        public void setAvailableSeList() {
            String outStr = "";
            boolean isFirst = true;

            if (SeList.containsKey(0x81) && mUICC1State != 0x2) {
                outStr += "SIM1";
                outStr += ",";
            }
            if ((SeList.containsKey(0x82) || SeList.containsKey(0x84) || SeList.containsKey(0x86))
                            && meSEState != 0x2) {
                outStr += "Embedded SE";
                outStr += ",";
            } else if ((SeList.containsKey(0x83) || SeList.containsKey(0x85))
                    && mUICC2State != 0x2) {
                outStr += "SIM2";
                outStr += ",";
            }

            outStr += "Off";

            Log.d(TAG, "setAvailableSeList() - outStr = " + outStr);
        }
         */

        public void setUserDesiredUiccString(String userDesiredUicc) {
            Log.d(TAG, "setUserDesiredUiccString() - userDesiredUicc = " + userDesiredUicc);

            mPrefsEditor = mPrefs.edit();
            if (userDesiredUicc.equals(SETTING_STR_SIM1)) {
                mPrefsEditor.putBoolean(PREF_UICC1_ACTIVATION_STATUS, true);
                mPrefsEditor.putBoolean(PREF_UICC2_ACTIVATION_STATUS, false);

            } else if (userDesiredUicc.equals(SETTING_STR_SIM2)) {
                mPrefsEditor.putBoolean(PREF_UICC1_ACTIVATION_STATUS, false);
                mPrefsEditor.putBoolean(PREF_UICC2_ACTIVATION_STATUS, true);
            } else {
                mPrefsEditor.putBoolean(PREF_UICC1_ACTIVATION_STATUS, false);
                mPrefsEditor.putBoolean(PREF_UICC2_ACTIVATION_STATUS, false);
            }
            mPrefsEditor.commit();
        }

        public String getUserDesiredUiccString() {
            if (mPrefs.contains(PREF_UICC1_ACTIVATION_STATUS)
                    || mPrefs.contains(PREF_UICC2_ACTIVATION_STATUS)) {
                if (mPrefs.getBoolean(PREF_UICC1_ACTIVATION_STATUS, false)) return SETTING_STR_SIM1;
                else if (mPrefs.getBoolean(PREF_UICC2_ACTIVATION_STATUS, false))
                    return SETTING_STR_SIM2;
                else return SETTING_STR_OFF;
            } else {
                return null;
            }
        }

        public String getUserDesiredeSEString() {
            if (mPrefs.contains(PREF_ESE_ACTIVATION_STATUS)
                    || mPrefs.contains(PREF_DHSE_ACTIVATION_STATUS)) {
                if (mPrefs.getBoolean(PREF_ESE_ACTIVATION_STATUS, false)) return SETTING_STR_ESE;
                else if (mPrefs.getBoolean(PREF_DHSE_ACTIVATION_STATUS, false))
                    return SETTING_STR_DHSE;
                else return SETTING_STR_OFF;
            } else {
                return null;
            }
        }

        public void setUserDesiredeSEString(String userDesiredeSE) {
            mPrefsEditor = mPrefs.edit();
            if (SETTING_STR_ESE.equals(userDesiredeSE)) {
                mPrefsEditor.putBoolean(PREF_ESE_ACTIVATION_STATUS, true);
                mPrefsEditor.putBoolean(PREF_DHSE_ACTIVATION_STATUS, false);
            } else if (SETTING_STR_DHSE.equals(userDesiredeSE)) {
                mPrefsEditor.putBoolean(PREF_ESE_ACTIVATION_STATUS, false);
                mPrefsEditor.putBoolean(PREF_DHSE_ACTIVATION_STATUS, true);
            } else {
                mPrefsEditor.putBoolean(PREF_ESE_ACTIVATION_STATUS, false);
                mPrefsEditor.putBoolean(PREF_DHSE_ACTIVATION_STATUS, false);
            }
            mPrefsEditor.commit();
        }

        /*  private void onSeRefresh(int updatedSe, int event) {
            Log.d(TAG, "onSeRefresh() - updatedSe = " + updatedSe + ", event = " + event);
            int UpdadetSeId;
            if (NfcSimStateObserver.SimEventListener.NOT_READY == event && USER_SIM1 == updatedSe) {
                Log.d(TAG, "onSeRefresh() - USER_SIM1 is not ready, reset SWP_SIM1 retry mechanism.");
                mIsSwpSim1Retry = false;
            } else if (NfcSimStateObserver.SimEventListener.NOT_READY == event &&
                    USER_SIM2 == updatedSe) {
                Log.d(TAG, "onSeRefresh() - USER_SIM2 is not ready, reset SWP_SIM2 retry mechanism.");
                mIsSwpSim2Retry = false;
            }
            if (updatedSe == USER_SIM1) {
                UpdadetSeId = 0x81;
            } else {
                UpdadetSeId = 0x82;
            }
            mStExtensions.EnableSE(UpdadetSeId, true);
        }

        private void showAlternativeSeDialog(int originalSe, int alternateSe) {
            Log.d(TAG, "showAlternativeSeDialog()");
            if (mContentProviderUtil.getNfcEnabled() == false) {
                return;
            }
            if (isFirstTime == true) {
                Log.i(TAG,
                        "showAlternativeSeDialog() - Silently disabling secure element because of " +
                                "first boot");
                alternateSe = 0x0;
                mContentProviderUtil.setActiveSeString(mContentProviderUtil.getStringofId(alternateSe));
                return;
            }
            String message;
            String firstSe;

            alternateSe = 0x00;
            for (int se : SeList.keySet()) {
                if ((se != originalSe) && (SeList.get(se) != 0x02)) {
                    alternateSe = se;
                    break;
                }
            }

            Log.d(TAG, "showAlternativeSeDialog() - original " + originalSe + ", alternate "
                    + alternateSe);
            if (alternateSe == 0x00) {
                mContentProviderUtil.setActiveSeString(mContentProviderUtil.getStringofId(alternateSe));

                /* Change Feature: Remove the "No available security elements.
             * NFC Card emulation will be turned off." popup diaglog.
             * for the temporary state that the se is not exist.*/
        /*          message = mContext.getString(R.string.mtk_se_na);
                firstSe = "";
                //return;
                mContentProviderUtil.setUserDesiredSeString(
                        mContentProviderUtil.getStringofId(alternateSe));

            } else {

                message = mContentProviderUtil.getStringofId(originalSe) + mContext.getString(
                        R.string.mtk_se_alt) +
                        mContentProviderUtil.getStringofId(alternateSe) + "?";
                firstSe = mContentProviderUtil.getStringofId(alternateSe);
            }

            Intent intent = new Intent(mContext, SecureElementConfirmActivity.class);
            intent.putExtra(SecureElementConfirmActivity.EXTRA_TITLE,
                    mContext.getString(R.string.mtk_nfc_card_emulation));
            intent.putExtra(SecureElementConfirmActivity.EXTRA_MESSAGE, message);
            intent.putExtra(SecureElementConfirmActivity.EXTRA_FIRSTSE, firstSe);
            intent.setFlags(intent.getFlags() | Intent.FLAG_ACTIVITY_NEW_TASK);
            mContext.startActivity(intent);
        }
             */
        /* private void notifyApplication(int type, Object obj) {
            Log.d(TAG, "notifyApplication() - type = " + type);
            if (mContentProviderUtil.getNfcEnabled() == false) {
                return;
            }

            if (type == NOTIFY_SELECTION_FAIL) {
                Intent intent = new Intent(ACTION_FAIL_TO_SELECT_SE);
                intent.setFlags(intent.getFlags() | Intent.FLAG_ACTIVITY_NEW_TASK);
                mContext.startActivity(intent);
            } else if (type == NOTIFY_SIM1_NOT_NFC || type == NOTIFY_SIM2_NOT_NFC) {
                Log.d(TAG, "notifyApplication() - NOTIFY_SIM1_NOT_NFC or NOTIFY_SIM2_NOT_NFC");
                if (mShouldSimWarningDialogPopup[(type == NOTIFY_SIM1_NOT_NFC ? 0 : 1)]) {
                    Intent intent = new Intent(ACTION_NOT_NFC_SIM);
                    intent.putExtra(EXTRA_WHAT_SIM, (String) obj);
                    intent.setFlags(intent.getFlags() | Intent.FLAG_ACTIVITY_NEW_TASK);
                    mContext.startActivity(intent);
                    mShouldSimWarningDialogPopup[(type == NOTIFY_SIM1_NOT_NFC ? 0 : 1)] = false;
                }
            } else if (type == NOTIFY_SIM1_SIM2_NOT_NFC) {
                Log.d(TAG, "notifyApplication() - NOTIFY_SIM1_NOT_NFC and NOTIFY_SIM2_NOT_NFC");
                if (mShouldSimWarningDialogPopup[0] &&
                        mShouldSimWarningDialogPopup[1]) {

                    Intent intent = new Intent(ACTION_NOT_NFC_TWO_SIM);
                    intent.putExtra(EXTRA_WHAT_SIM, (String) obj);
                    intent.setFlags(intent.getFlags() | Intent.FLAG_ACTIVITY_NEW_TASK);
                    mContext.startActivity(intent);
                    mShouldSimWarningDialogPopup[0] = false;
                    mShouldSimWarningDialogPopup[1] = false;
                }
            }
        }-*/
    }
}
