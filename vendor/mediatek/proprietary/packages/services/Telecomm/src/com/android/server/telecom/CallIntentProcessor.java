package com.android.server.telecom;

import com.android.server.telecom.components.ErrorDialogActivity;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.Looper;
import android.os.Trace;
import android.os.UserHandle;
import android.os.UserManager;
import android.telecom.DefaultDialerManager;
/// M: Using mtk log instead of AOSP log.
// import android.telecom.Log;
import android.telecom.Logging.Session;
import android.telecom.PhoneAccount;
import android.telecom.PhoneAccountHandle;
import android.telecom.TelecomManager;
import android.telecom.VideoProfile;
import android.telephony.DisconnectCause;
import android.telephony.PhoneNumberUtils;
import android.widget.Toast;

/// M: Mediatek import.
import com.mediatek.server.telecom.Log;
import com.mediatek.server.telecom.MtkUtil;
import mediatek.telecom.MtkTelecomManager;

import java.util.concurrent.CompletableFuture;

/**
 * Single point of entry for all outgoing and incoming calls.
 * {@link com.android.server.telecom.components.UserCallIntentProcessor} serves as a trampoline that
 * captures call intents for individual users and forwards it to the {@link CallIntentProcessor}
 * which interacts with the rest of Telecom, both of which run only as the primary user.
 */
public class CallIntentProcessor {
    public interface Adapter {
        void processOutgoingCallIntent(Context context, CallsManager callsManager,
                Intent intent, String callingPackage);
        void processIncomingCallIntent(CallsManager callsManager, Intent intent);
        void processUnknownCallIntent(CallsManager callsManager, Intent intent);
    }

    public static class AdapterImpl implements Adapter {
        @Override
        public void processOutgoingCallIntent(Context context, CallsManager callsManager,
                Intent intent, String callingPackage) {
            CallIntentProcessor.processOutgoingCallIntent(context, callsManager, intent,
                    callingPackage);
        }

        @Override
        public void processIncomingCallIntent(CallsManager callsManager, Intent intent) {
            CallIntentProcessor.processIncomingCallIntent(callsManager, intent);
        }

        @Override
        public void processUnknownCallIntent(CallsManager callsManager, Intent intent) {
            CallIntentProcessor.processUnknownCallIntent(callsManager, intent);
        }
    }

    public static final String KEY_IS_UNKNOWN_CALL = "is_unknown_call";
    public static final String KEY_IS_INCOMING_CALL = "is_incoming_call";
    /*
     *  Whether or not the dialer initiating this outgoing call is the default dialer, or system
     *  dialer and thus allowed to make emergency calls.
     */
    public static final String KEY_IS_PRIVILEGED_DIALER = "is_privileged_dialer";

    /**
     * The user initiating the outgoing call.
     */
    public static final String KEY_INITIATING_USER = "initiating_user";


    private final Context mContext;
    private final CallsManager mCallsManager;

    public CallIntentProcessor(Context context, CallsManager callsManager) {
        this.mContext = context;
        this.mCallsManager = callsManager;
    }

    public void processIntent(Intent intent, String callingPackage) {
        final boolean isUnknownCall = intent.getBooleanExtra(KEY_IS_UNKNOWN_CALL, false);
        Log.i(this, "onReceive - isUnknownCall: %s", isUnknownCall);

        Trace.beginSection("processNewCallCallIntent");
        if (isUnknownCall) {
            processUnknownCallIntent(mCallsManager, intent);
        } else {
            /// M: Performance enhancement. Bind InCallService at the beginning. @{
            if (MtkUtil.PREBIND_INCALLSERVICE_ENABLED &&
                    !(mCallsManager.getInCallController().isBoundAndConnectedToServices())) {
                mCallsManager.getInCallController().bindToServices(null);
            }
            /// M: @}

            processOutgoingCallIntent(mContext, mCallsManager, intent, callingPackage);
        }
        Trace.endSection();
    }


    /**
     * Processes CALL, CALL_PRIVILEGED, and CALL_EMERGENCY intents.
     *
     * @param intent Call intent containing data about the handle to call.
     * @param callingPackage The package which initiated the outgoing call (if known).
     */
    static void processOutgoingCallIntent(
            Context context,
            CallsManager callsManager,
            Intent intent,
            String callingPackage) {

        Uri handle = intent.getData();
        String scheme = handle.getScheme();
        String uriString = handle.getSchemeSpecificPart();

        // Ensure sip URIs dialed using TEL scheme get converted to SIP scheme.
        if (PhoneAccount.SCHEME_TEL.equals(scheme) && PhoneNumberUtils.isUriNumber(uriString)) {
            /// M: Support IMS number contains "@", like "tel:test@server". @{
            handle = Uri.fromParts(PhoneNumberUtils.isUriNumber(uriString)
                    && !MtkUtil.isImsCallIntent(intent) ?
                    PhoneAccount.SCHEME_SIP : PhoneAccount.SCHEME_TEL, uriString, null);
            /// @}
        }

        PhoneAccountHandle phoneAccountHandle = intent.getParcelableExtra(
                TelecomManager.EXTRA_PHONE_ACCOUNT_HANDLE);

        Bundle clientExtras = null;
        if (intent.hasExtra(TelecomManager.EXTRA_OUTGOING_CALL_EXTRAS)) {
            clientExtras = intent.getBundleExtra(TelecomManager.EXTRA_OUTGOING_CALL_EXTRAS);
        }
        if (clientExtras == null) {
            clientExtras = new Bundle();
        }

        if (intent.hasExtra(TelecomManager.EXTRA_IS_USER_INTENT_EMERGENCY_CALL)) {
            clientExtras.putBoolean(TelecomManager.EXTRA_IS_USER_INTENT_EMERGENCY_CALL,
                    intent.getBooleanExtra(TelecomManager.EXTRA_IS_USER_INTENT_EMERGENCY_CALL,
                            false));
        }

        // Ensure call subject is passed on to the connection service.
        if (intent.hasExtra(TelecomManager.EXTRA_CALL_SUBJECT)) {
            String callsubject = intent.getStringExtra(TelecomManager.EXTRA_CALL_SUBJECT);
            clientExtras.putString(TelecomManager.EXTRA_CALL_SUBJECT, callsubject);
        }

        final int videoState = intent.getIntExtra( TelecomManager.EXTRA_START_CALL_WITH_VIDEO_STATE,
                VideoProfile.STATE_AUDIO_ONLY);
        clientExtras.putInt(TelecomManager.EXTRA_START_CALL_WITH_VIDEO_STATE, videoState);

        if (!callsManager.isSelfManaged(phoneAccountHandle,
                (UserHandle) intent.getParcelableExtra(KEY_INITIATING_USER))) {
            boolean fixedInitiatingUser = fixInitiatingUserIfNecessary(context, intent);
            // Show the toast to warn user that it is a personal call though initiated in work
            // profile.
            if (fixedInitiatingUser) {
                Toast.makeText(context, Looper.getMainLooper(),
                        context.getString(R.string.toast_personal_call_msg),
                        Toast.LENGTH_LONG).show();
            }
        } else {
            Log.i(CallIntentProcessor.class,
                    "processOutgoingCallIntent: skip initiating user check");
        }

        /// M: Copy MTK extras to the clientExtras.
        copyMtkExtras(intent.getExtras(), clientExtras);

        UserHandle initiatingUser = intent.getParcelableExtra(KEY_INITIATING_USER);

        /// M: For dial via specified slot. @{
        if (intent.hasExtra(MtkUtil.EXTRA_SLOT)) {
            int slotId = intent.getIntExtra(MtkUtil.EXTRA_SLOT, -1);
            phoneAccountHandle = MtkUtil
                    .getPhoneAccountHandleWithSlotId(context, slotId, phoneAccountHandle);
        }
        /// @}

        // Send to CallsManager to ensure the InCallUI gets kicked off before the broadcast returns
        CompletableFuture<Call> callFuture = callsManager
                .startOutgoingCall(handle, phoneAccountHandle, clientExtras, initiatingUser,
                        intent, callingPackage);

        final Session logSubsession = Log.createSubsession();
        callFuture.thenAccept((call) -> {
            if (call != null) {
                Log.continueSession(logSubsession, "CIP.sNOCI");
                try {
                    sendNewOutgoingCallIntent(context, call, callsManager, intent);
                } finally {
                    Log.endSession();
                }
            } else {
                /// M: Performance enhancement. Unbind InCallService as call is not added.
                if (MtkUtil.PREBIND_INCALLSERVICE_ENABLED) {
                    callsManager.getInCallController().unbindUselessService();
                }
                /// M: @}
            }
        });
    }

    static void sendNewOutgoingCallIntent(Context context, Call call, CallsManager callsManager,
            Intent intent) {
        // Asynchronous calls should not usually be made inside a BroadcastReceiver because once
        // onReceive is complete, the BroadcastReceiver's process runs the risk of getting
        // killed if memory is scarce. However, this is OK here because the entire Telecom
        // process will be running throughout the duration of the phone call and should never
        // be killed.
        final boolean isPrivilegedDialer = intent.getBooleanExtra(KEY_IS_PRIVILEGED_DIALER, false);

        NewOutgoingCallIntentBroadcaster broadcaster = new NewOutgoingCallIntentBroadcaster(
                context, callsManager, call, intent, callsManager.getPhoneNumberUtilsAdapter(),
                isPrivilegedDialer);

        // If the broadcaster comes back with an immediate error, disconnect and show a dialog.
        NewOutgoingCallIntentBroadcaster.CallDisposition disposition = broadcaster.evaluateCall();
        if (disposition.disconnectCause != DisconnectCause.NOT_DISCONNECTED) {
            disconnectCallAndShowErrorDialog(context, call, disposition.disconnectCause);
            return;
        }

        broadcaster.processCall(disposition);
    }

    /**
     * If the call is initiated from managed profile but there is no work dialer installed, treat
     * the call is initiated from its parent user.
     *
     * @return whether the initiating user is fixed.
     */
    static boolean fixInitiatingUserIfNecessary(Context context, Intent intent) {
        final UserHandle initiatingUser = intent.getParcelableExtra(KEY_INITIATING_USER);
        if (UserUtil.isManagedProfile(context, initiatingUser)) {
            boolean noDialerInstalled = DefaultDialerManager.getInstalledDialerApplications(context,
                    initiatingUser.getIdentifier()).size() == 0;
            if (noDialerInstalled) {
                final UserManager userManager = UserManager.get(context);
                UserHandle parentUserHandle =
                        userManager.getProfileParent(
                                initiatingUser.getIdentifier()).getUserHandle();
                intent.putExtra(KEY_INITIATING_USER, parentUserHandle);

                Log.i(CallIntentProcessor.class, "fixInitiatingUserIfNecessary: no dialer installed"
                        + " for current user; setting initiator to parent %s" + parentUserHandle);
                return true;
            }
        }
        return false;
    }

    static void processIncomingCallIntent(CallsManager callsManager, Intent intent) {
        PhoneAccountHandle phoneAccountHandle = intent.getParcelableExtra(
                TelecomManager.EXTRA_PHONE_ACCOUNT_HANDLE);

        if (phoneAccountHandle == null) {
            Log.w(CallIntentProcessor.class,
                    "Rejecting incoming call due to null phone account");
            return;
        }
        if (phoneAccountHandle.getComponentName() == null) {
            Log.w(CallIntentProcessor.class,
                    "Rejecting incoming call due to null component name");
            return;
        }

        Bundle clientExtras = null;
        if (intent.hasExtra(TelecomManager.EXTRA_INCOMING_CALL_EXTRAS)) {
            clientExtras = intent.getBundleExtra(TelecomManager.EXTRA_INCOMING_CALL_EXTRAS);
        }
        if (clientExtras == null) {
            clientExtras = new Bundle();
        }

        Log.d(CallIntentProcessor.class,
                "Processing incoming call from connection service [%s]",
                phoneAccountHandle.getComponentName());
        callsManager.processIncomingCallIntent(phoneAccountHandle, clientExtras);
    }

    static void processUnknownCallIntent(CallsManager callsManager, Intent intent) {
        PhoneAccountHandle phoneAccountHandle = intent.getParcelableExtra(
                TelecomManager.EXTRA_PHONE_ACCOUNT_HANDLE);

        if (phoneAccountHandle == null) {
            Log.w(CallIntentProcessor.class, "Rejecting unknown call due to null phone account");
            return;
        }
        if (phoneAccountHandle.getComponentName() == null) {
            Log.w(CallIntentProcessor.class, "Rejecting unknown call due to null component name");
            return;
        }

        callsManager.addNewUnknownCall(phoneAccountHandle, intent.getExtras());
    }

    private static void disconnectCallAndShowErrorDialog(
            Context context, Call call, int errorCode) {
        call.disconnect();
        final Intent errorIntent = new Intent(context, ErrorDialogActivity.class);
        int errorMessageId = -1;
        switch (errorCode) {
            case DisconnectCause.INVALID_NUMBER:
            case DisconnectCause.NO_PHONE_NUMBER_SUPPLIED:
                errorMessageId = R.string.outgoing_call_error_no_phone_number_supplied;
                break;
        }
        if (errorMessageId != -1) {
            errorIntent.putExtra(ErrorDialogActivity.ERROR_MESSAGE_ID_EXTRA, errorMessageId);
            errorIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivityAsUser(errorIntent, UserHandle.CURRENT);
        }
    }

    /** M: Copy extras from bundle. */
    private static void copyMtkExtras(Bundle src, Bundle dst) {
        if (src == null || dst == null) {
            return;
        }
        // VoLTE - Conference Dial
        if (src.containsKey(MtkTelecomManager.EXTRA_START_VOLTE_CONFERENCE)) {
            dst.putBoolean(MtkTelecomManager.EXTRA_START_VOLTE_CONFERENCE,
                    src.getBoolean(MtkTelecomManager.EXTRA_START_VOLTE_CONFERENCE, false));
        }
        // VoLTE - Conference Dial
        if (src.containsKey(MtkTelecomManager.EXTRA_VOLTE_CONFERENCE_NUMBERS)) {
            dst.putStringArrayList(MtkTelecomManager.EXTRA_VOLTE_CONFERENCE_NUMBERS,
                    src.getStringArrayList(MtkTelecomManager.EXTRA_VOLTE_CONFERENCE_NUMBERS));
        }
        // VoLTE - Incoming conference call
        if (src.containsKey(MtkTelecomManager.EXTRA_INCOMING_VOLTE_CONFERENCE)) {
            dst.putBoolean(MtkTelecomManager.EXTRA_INCOMING_VOLTE_CONFERENCE,
                    src.getBoolean(MtkTelecomManager.EXTRA_INCOMING_VOLTE_CONFERENCE, false));
        }
    }
}
