/*
 * Copyright (C) 2016 The Android Open Source Project
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
 * limitations under the License
 */

package com.android.server.telecom;

import static android.telecom.Call.Details.DIRECTION_INCOMING;
import static android.telecom.Call.Details.DIRECTION_OUTGOING;
import static android.telecom.Call.Details.DIRECTION_UNKNOWN;

import android.net.Uri;
import android.os.Bundle;
import android.telecom.Connection;
import android.telecom.DisconnectCause;
import android.telecom.ParcelableCall;
import android.telecom.ParcelableRttCall;
import android.telecom.TelecomManager;
import android.text.TextUtils;

/// M: Mediatek import.
import com.mediatek.server.telecom.ext.ExtensionManager;
import mediatek.telecom.MtkCall;
import mediatek.telecom.MtkConnection;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

/**
 * Utilities dealing with {@link ParcelableCall}.
 */
public class ParcelableCallUtils {
    private static final int CALL_STATE_OVERRIDE_NONE = -1;

    /**
     * A list of extra keys which should be removed from a {@link ParcelableCall} when it is being
     * generated for the purpose of sending to a dialer other than the system dialer.
     * By convention we only pass keys namespaced with android.*, however there are some keys which
     * should not be passed to non-system dialer apps either.
     */
    private static List<String> EXTRA_KEYS_TO_SANITIZE;
    static {
        EXTRA_KEYS_TO_SANITIZE = new ArrayList<>();
        EXTRA_KEYS_TO_SANITIZE.add(android.telecom.Connection.EXTRA_SIP_INVITE);
    }

    /**
     * A list of extra keys which should be added to {@link ParcelableCall} when it is being
     * generated for the purpose of sending to a CallScreeningService which has access to these
     * restricted keys.
     */
    private static List<String> RESTRICTED_CALL_SCREENING_EXTRA_KEYS;
    static {
        RESTRICTED_CALL_SCREENING_EXTRA_KEYS = new ArrayList<>();
        RESTRICTED_CALL_SCREENING_EXTRA_KEYS.add(android.telecom.Connection.EXTRA_SIP_INVITE);
    }

    public static class Converter {
        public ParcelableCall toParcelableCall(Call call, boolean includeVideoProvider,
                PhoneAccountRegistrar phoneAccountRegistrar) {
            return ParcelableCallUtils.toParcelableCall(
                    call, includeVideoProvider, phoneAccountRegistrar, false, false, false);
        }

        public ParcelableCall toParcelableCallForScreening(Call call,
                boolean areRestrictedExtrasIncluded) {
            return ParcelableCallUtils.toParcelableCallForScreening(call,
                    areRestrictedExtrasIncluded);
        }
    }

    /**
     * Parcels all information for a {@link Call} into a new {@link ParcelableCall} instance.
     *
     * @param call The {@link Call} to parcel.
     * @param includeVideoProvider {@code true} if the video provider should be parcelled with the
     *      {@link Call}, {@code false} otherwise.  Since the {@link ParcelableCall#getVideoCall()}
     *      method creates a {@link VideoCallImpl} instance on access it is important for the
     *      recipient of the {@link ParcelableCall} to know if the video provider changed.
     * @param phoneAccountRegistrar The {@link PhoneAccountRegistrar}.
     * @param supportsExternalCalls Indicates whether the call should be parcelled for an
     *      {@link InCallService} which supports external calls or not.
     * @param includeRttCall {@code true} if the RTT call should be included, {@code false}
     *      otherwise.
     * @param isForSystemDialer {@code true} if this call is being parcelled for the system dialer,
     *      {@code false} otherwise.  When parceling for the system dialer, the entire call extras
     *      is included.  When parceling for anything other than the system dialer, some extra key
     *      values will be stripped for privacy sake.
     */
    public static ParcelableCall toParcelableCall(
            Call call,
            boolean includeVideoProvider,
            PhoneAccountRegistrar phoneAccountRegistrar,
            boolean supportsExternalCalls,
            boolean includeRttCall,
            boolean isForSystemDialer) {
        return toParcelableCall(call, includeVideoProvider, phoneAccountRegistrar,
                supportsExternalCalls, CALL_STATE_OVERRIDE_NONE /* overrideState */,
                includeRttCall, isForSystemDialer);
    }

    /**
     * Parcels all information for a {@link Call} into a new {@link ParcelableCall} instance.
     *
     * @param call The {@link Call} to parcel.
     * @param includeVideoProvider {@code true} if the video provider should be parcelled with the
     *      {@link Call}, {@code false} otherwise.  Since the {@link ParcelableCall#getVideoCall()}
     *      method creates a {@link VideoCallImpl} instance on access it is important for the
     *      recipient of the {@link ParcelableCall} to know if the video provider changed.
     * @param phoneAccountRegistrar The {@link PhoneAccountRegistrar}.
     * @param supportsExternalCalls Indicates whether the call should be parcelled for an
     *      {@link InCallService} which supports external calls or not.
     * @param overrideState When not {@link #CALL_STATE_OVERRIDE_NONE}, use the provided state as an
     *      override to whatever is defined in the call.
     * @param isForSystemDialer {@code true} if this call is being parcelled for the system dialer,
     *      {@code false} otherwise.  When parceling for the system dialer, the entire call extras
     *      is included.  When parceling for anything other than the system dialer, some extra key
     *      values will be stripped for privacy sake.
     * @return The {@link ParcelableCall} containing all call information from the {@link Call}.
     */
    public static ParcelableCall toParcelableCall(
            Call call,
            boolean includeVideoProvider,
            PhoneAccountRegistrar phoneAccountRegistrar,
            boolean supportsExternalCalls,
            int overrideState,
            boolean includeRttCall,
            boolean isForSystemDialer) {
        int state;
        if (overrideState == CALL_STATE_OVERRIDE_NONE) {
            state = getParcelableState(call, supportsExternalCalls);
        } else {
            state = overrideState;
        }
        int capabilities = convertConnectionToCallCapabilities(call.getConnectionCapabilities());
        int properties = convertConnectionToCallProperties(call.getConnectionProperties());
        int supportedAudioRoutes = call.getSupportedAudioRoutes();

        if (call.isConference()) {
            properties |= android.telecom.Call.Details.PROPERTY_CONFERENCE;
        }

        if (call.isWorkCall()) {
            properties |= android.telecom.Call.Details.PROPERTY_ENTERPRISE_CALL;
        }

        if (call.getIsVoipAudioMode()) {
            properties |= android.telecom.Call.Details.PROPERTY_VOIP_AUDIO_MODE;
        }

        // If this is a single-SIM device, the "default SIM" will always be the only SIM.
        boolean isDefaultSmsAccount = phoneAccountRegistrar != null &&
                phoneAccountRegistrar.isUserSelectedSmsPhoneAccount(call.getTargetPhoneAccount());
        if (call.isRespondViaSmsCapable() && isDefaultSmsAccount) {
            capabilities |= android.telecom.Call.Details.CAPABILITY_RESPOND_VIA_TEXT;
        }

        if (call.isEmergencyCall()) {
            capabilities = removeCapability(
                    capabilities, android.telecom.Call.Details.CAPABILITY_MUTE);
        }

        if (state == android.telecom.Call.STATE_DIALING) {
            capabilities = removeCapability(capabilities,
                    android.telecom.Call.Details.CAPABILITY_SUPPORTS_VT_LOCAL_BIDIRECTIONAL);
            capabilities = removeCapability(capabilities,
                    android.telecom.Call.Details.CAPABILITY_SUPPORTS_VT_REMOTE_BIDIRECTIONAL);
        }

        /// M: plugin to modify capabilities.
        capabilities |= ExtensionManager.getCallMgrExt()
                .buildCallCapabilities(call.isRespondViaSmsCapable());

        String parentCallId = null;
        Call parentCall = call.getParentCall();
        if (parentCall != null) {
            parentCallId = parentCall.getId();
        }

        long connectTimeMillis = call.getConnectTimeMillis();
        List<Call> childCalls = call.getChildCalls();
        List<String> childCallIds = new ArrayList<>();
        if (!childCalls.isEmpty()) {
            long childConnectTimeMillis = Long.MAX_VALUE;
            for (Call child : childCalls) {
                if (child.getConnectTimeMillis() > 0) {
                    childConnectTimeMillis = Math.min(child.getConnectTimeMillis(),
                            childConnectTimeMillis);
                }
                childCallIds.add(child.getId());
            }
            // M: fix CR:ALPS03499301,conference call time display base on the min between
            // conference call and child call.
            if (childConnectTimeMillis != Long.MAX_VALUE) {
                if (connectTimeMillis == 0 || childConnectTimeMillis < connectTimeMillis) {
                    connectTimeMillis = childConnectTimeMillis;
                }
            }
        }

        Uri handle = call.getHandlePresentation() == TelecomManager.PRESENTATION_ALLOWED ?
                call.getHandle() : null;
        String callerDisplayName = call.getCallerDisplayNamePresentation() ==
                TelecomManager.PRESENTATION_ALLOWED ?  call.getCallerDisplayName() : null;

        List<Call> conferenceableCalls = call.getConferenceableCalls();
        List<String> conferenceableCallIds = new ArrayList<String>(conferenceableCalls.size());
        for (Call otherCall : conferenceableCalls) {
            conferenceableCallIds.add(otherCall.getId());
        }

        ParcelableRttCall rttCall = includeRttCall ? getParcelableRttCall(call) : null;
        int callDirection;
        if (call.isIncoming()) {
            callDirection = DIRECTION_INCOMING;
        } else if (call.isUnknown()) {
            callDirection = DIRECTION_UNKNOWN;
        } else {
            callDirection = DIRECTION_OUTGOING;
        }

        Bundle extras;
        if (isForSystemDialer) {
            extras = call.getExtras();
        } else {
            extras = sanitizeExtras(call.getExtras());
        }

        return new ParcelableCall(
                call.getId(),
                state,
                call.getDisconnectCause(),
                call.getCannedSmsResponses(),
                capabilities,
                properties,
                supportedAudioRoutes,
                connectTimeMillis,
                handle,
                call.getHandlePresentation(),
                callerDisplayName,
                call.getCallerDisplayNamePresentation(),
                call.getGatewayInfo(),
                call.getTargetPhoneAccount(),
                includeVideoProvider,
                includeVideoProvider ? call.getVideoProvider() : null,
                includeRttCall,
                rttCall,
                parentCallId,
                childCallIds,
                call.getStatusHints(),
                call.getVideoState(),
                conferenceableCallIds,
                call.getIntentExtras(),
                extras,
                call.getCreationTimeMillis(),
                callDirection);
    }

    /**
     * Creates a ParcelableCall with the bare minimum properties required for a
     * {@link android.telecom.CallScreeningService}.  We ONLY expose the following:
     * <ul>
     *     <li>Call Id (not exposed to public, but needed to associated calls)</li>
     *     <li>Call directoin</li>
     *     <li>Creation time</li>
     *     <li>Connection time</li>
     *     <li>Handle (phone number)</li>
     *     <li>Handle (phone number) presentation</li>
     * </ul>
     * All other fields are nulled or set to 0 values.
     * Where the call screening service is part of the system dialer, the
     * {@link Connection#EXTRA_SIP_INVITE} header information is also sent to the call screening
     * service (since the system dialer has access to this anyways).
     * @param call The telecom call to send to a call screening service.
     * @param areRestrictedExtrasIncluded {@code true} if the set of restricted extras defined in
     *                                    {@link #RESTRICTED_CALL_SCREENING_EXTRA_KEYS} are to
     *                                    be included in the parceled call, {@code false} otherwise.
     * @return Minimal {@link ParcelableCall} to send to the call screening service.
     */
    public static ParcelableCall toParcelableCallForScreening(Call call,
            boolean areRestrictedExtrasIncluded) {
        Uri handle = call.getHandlePresentation() == TelecomManager.PRESENTATION_ALLOWED ?
                call.getHandle() : null;
        int callDirection;
        if (call.isIncoming()) {
            callDirection = DIRECTION_INCOMING;
        } else if (call.isUnknown()) {
            callDirection = DIRECTION_UNKNOWN;
        } else {
            callDirection = DIRECTION_OUTGOING;
        }
        Bundle callExtras;
        if (areRestrictedExtrasIncluded) {
            callExtras = sanitizeRestrictedCallExtras(call.getExtras());
        } else {
            callExtras = new Bundle();
        }

        return new ParcelableCall(
                call.getId(),
                getParcelableState(call, false /* supportsExternalCalls */),
                new DisconnectCause(DisconnectCause.UNKNOWN),
                null, /* cannedSmsResponses */
                0, /* capabilities */
                0, /* properties */
                0, /* supportedAudioRoutes */
                call.getConnectTimeMillis(),
                handle,
                call.getHandlePresentation(),
                null, /* callerDisplayName */
                0 /* callerDisplayNamePresentation */,
                null, /* gatewayInfo */
                null, /* targetPhoneAccount */
                false, /* includeVideoProvider */
                null, /* videoProvider */
                false, /* includeRttCall */
                null, /* rttCall */
                null, /* parentCallId */
                null, /* childCallIds */
                null, /* statusHints */
                0, /* videoState */
                Collections.emptyList(), /* conferenceableCallIds */
                null, /* intentExtras */
                callExtras, /* callExtras */
                call.getCreationTimeMillis(),
                callDirection);
    }

    /**
     * Sanitize the extras bundle passed in, removing keys which should not be sent to non-system
     * dialer apps.
     * @param oldExtras Extras bundle to sanitize.
     * @return The sanitized extras bundle.
     */
    private static Bundle sanitizeExtras(Bundle oldExtras) {
        if (oldExtras == null) {
            return new Bundle();
        }
        Bundle extras = new Bundle(oldExtras);
        for (String key : EXTRA_KEYS_TO_SANITIZE) {
            extras.remove(key);
        }

        // As a catch-all remove any that don't start with android namespace.
        Iterator<String> toCheck = extras.keySet().iterator();
        while (toCheck.hasNext()) {
            String extraKey = toCheck.next();
            if (TextUtils.isEmpty(extraKey) || !extraKey.startsWith("android.")) {
                toCheck.remove();
            }
        }
        return extras;
    }

    /**
     * Sanitize the extras bundle passed in, removing keys which should not be sent to call
     * screening services which have access to the restricted extras.
     * @param oldExtras Extras bundle to sanitize.
     * @return The sanitized extras bundle.
     */
    private static Bundle sanitizeRestrictedCallExtras(Bundle oldExtras) {
        if (oldExtras == null) {
            return new Bundle();
        }
        Bundle extras = new Bundle(oldExtras);
        Iterator<String> toCheck = extras.keySet().iterator();
        while (toCheck.hasNext()) {
            String extraKey = toCheck.next();
            if (TextUtils.isEmpty(extraKey)
                    || !RESTRICTED_CALL_SCREENING_EXTRA_KEYS.contains(extraKey)) {
                toCheck.remove();
            }
        }
        return extras;
    }

    private static int getParcelableState(Call call, boolean supportsExternalCalls) {
        int state = CallState.NEW;
        switch (call.getState()) {
            case CallState.ABORTED:
            case CallState.DISCONNECTED:
                state = android.telecom.Call.STATE_DISCONNECTED;
                break;
            case CallState.ACTIVE:
                state = android.telecom.Call.STATE_ACTIVE;
                break;
            case CallState.CONNECTING:
                state = android.telecom.Call.STATE_CONNECTING;
                break;
            case CallState.DIALING:
                state = android.telecom.Call.STATE_DIALING;
                break;
            case CallState.PULLING:
                if (supportsExternalCalls) {
                    // The InCallService supports external calls, so it must handle
                    // STATE_PULLING_CALL.
                    state = android.telecom.Call.STATE_PULLING_CALL;
                } else {
                    // The InCallService does NOT support external calls, so remap
                    // STATE_PULLING_CALL to STATE_DIALING.  In essence, pulling a call can be seen
                    // as a form of dialing, so it is appropriate for InCallServices which do not
                    // handle external calls.
                    state = android.telecom.Call.STATE_DIALING;
                }
                break;
            case CallState.DISCONNECTING:
                state = android.telecom.Call.STATE_DISCONNECTING;
                break;
            case CallState.NEW:
                state = android.telecom.Call.STATE_NEW;
                break;
            case CallState.ON_HOLD:
                state = android.telecom.Call.STATE_HOLDING;
                break;
            case CallState.RINGING:
            case CallState.ANSWERED:
                // TODO: does in-call UI need to see ANSWERED?
                state = android.telecom.Call.STATE_RINGING;
                break;
            case CallState.SELECT_PHONE_ACCOUNT:
                state = android.telecom.Call.STATE_SELECT_PHONE_ACCOUNT;
                break;
        }

        // If we are marked as 'locally disconnecting' then mark ourselves as disconnecting instead.
        // Unless we're disconnect*ED*, in which case leave it at that.
        if (call.isLocallyDisconnecting() &&
                (state != android.telecom.Call.STATE_DISCONNECTED)) {
            state = android.telecom.Call.STATE_DISCONNECTING;
        }
        return state;
    }

    private static final int[] CONNECTION_TO_CALL_CAPABILITY = new int[] {
        Connection.CAPABILITY_HOLD,
        android.telecom.Call.Details.CAPABILITY_HOLD,

        Connection.CAPABILITY_SUPPORT_HOLD,
        android.telecom.Call.Details.CAPABILITY_SUPPORT_HOLD,

        Connection.CAPABILITY_MERGE_CONFERENCE,
        android.telecom.Call.Details.CAPABILITY_MERGE_CONFERENCE,

        Connection.CAPABILITY_SWAP_CONFERENCE,
        android.telecom.Call.Details.CAPABILITY_SWAP_CONFERENCE,

        Connection.CAPABILITY_RESPOND_VIA_TEXT,
        android.telecom.Call.Details.CAPABILITY_RESPOND_VIA_TEXT,

        Connection.CAPABILITY_MUTE,
        android.telecom.Call.Details.CAPABILITY_MUTE,

        Connection.CAPABILITY_MANAGE_CONFERENCE,
        android.telecom.Call.Details.CAPABILITY_MANAGE_CONFERENCE,

        Connection.CAPABILITY_SUPPORTS_VT_LOCAL_RX,
        android.telecom.Call.Details.CAPABILITY_SUPPORTS_VT_LOCAL_RX,

        Connection.CAPABILITY_SUPPORTS_VT_LOCAL_TX,
        android.telecom.Call.Details.CAPABILITY_SUPPORTS_VT_LOCAL_TX,

        Connection.CAPABILITY_SUPPORTS_VT_LOCAL_BIDIRECTIONAL,
        android.telecom.Call.Details.CAPABILITY_SUPPORTS_VT_LOCAL_BIDIRECTIONAL,

        Connection.CAPABILITY_SUPPORTS_VT_REMOTE_RX,
        android.telecom.Call.Details.CAPABILITY_SUPPORTS_VT_REMOTE_RX,

        Connection.CAPABILITY_SUPPORTS_VT_REMOTE_TX,
        android.telecom.Call.Details.CAPABILITY_SUPPORTS_VT_REMOTE_TX,

        Connection.CAPABILITY_SUPPORTS_VT_REMOTE_BIDIRECTIONAL,
        android.telecom.Call.Details.CAPABILITY_SUPPORTS_VT_REMOTE_BIDIRECTIONAL,

        Connection.CAPABILITY_SEPARATE_FROM_CONFERENCE,
        android.telecom.Call.Details.CAPABILITY_SEPARATE_FROM_CONFERENCE,

        Connection.CAPABILITY_DISCONNECT_FROM_CONFERENCE,
        android.telecom.Call.Details.CAPABILITY_DISCONNECT_FROM_CONFERENCE,

        Connection.CAPABILITY_CAN_UPGRADE_TO_VIDEO,
        android.telecom.Call.Details.CAPABILITY_CAN_UPGRADE_TO_VIDEO,

        Connection.CAPABILITY_CAN_PAUSE_VIDEO,
        android.telecom.Call.Details.CAPABILITY_CAN_PAUSE_VIDEO,

        Connection.CAPABILITY_CAN_SEND_RESPONSE_VIA_CONNECTION,
        android.telecom.Call.Details.CAPABILITY_CAN_SEND_RESPONSE_VIA_CONNECTION,

        Connection.CAPABILITY_CANNOT_DOWNGRADE_VIDEO_TO_AUDIO,
        android.telecom.Call.Details.CAPABILITY_CANNOT_DOWNGRADE_VIDEO_TO_AUDIO,

        Connection.CAPABILITY_CAN_PULL_CALL,
        android.telecom.Call.Details.CAPABILITY_CAN_PULL_CALL,

        Connection.CAPABILITY_SUPPORT_DEFLECT,
        android.telecom.Call.Details.CAPABILITY_SUPPORT_DEFLECT,

        /// M: MTK capabilities pairs. @{
        MtkConnection.CAPABILITY_CAPABILITY_CALL_RECORDING,
        MtkCall.MtkDetails.MTK_CAPABILITY_CALL_RECORDING,

        MtkConnection.CAPABILITY_INVITE_PARTICIPANTS,
        MtkCall.MtkDetails.MTK_CAPABILITY_INVITE_PARTICIPANTS,

        MtkConnection.CAPABILITY_CONSULTATIVE_ECT,
        MtkCall.MtkDetails.MTK_CAPABILITY_CONSULTATIVE_ECT,

        MtkConnection.CAPABILITY_BLIND_OR_ASSURED_ECT,
        MtkCall.MtkDetails.MTK_CAPABILITY_BLIND_OR_ASSURED_ECT,

        MtkConnection.CAPABILITY_VIDEO_RINGTONE,
        MtkCall.MtkDetails.MTK_CAPABILITY_VIDEO_RINGTONE
        /// M: @}
    };

    private static int convertConnectionToCallCapabilities(int connectionCapabilities) {
        int callCapabilities = 0;
        for (int i = 0; i < CONNECTION_TO_CALL_CAPABILITY.length; i += 2) {
            if ((CONNECTION_TO_CALL_CAPABILITY[i] & connectionCapabilities) ==
                    CONNECTION_TO_CALL_CAPABILITY[i]) {

                callCapabilities |= CONNECTION_TO_CALL_CAPABILITY[i + 1];
            }
        }
        return callCapabilities;
    }

    private static final int[] CONNECTION_TO_CALL_PROPERTIES = new int[] {
        Connection.PROPERTY_HIGH_DEF_AUDIO,
        android.telecom.Call.Details.PROPERTY_HIGH_DEF_AUDIO,

        Connection.PROPERTY_WIFI,
        android.telecom.Call.Details.PROPERTY_WIFI,

        Connection.PROPERTY_GENERIC_CONFERENCE,
        android.telecom.Call.Details.PROPERTY_GENERIC_CONFERENCE,

        Connection.PROPERTY_EMERGENCY_CALLBACK_MODE,
        android.telecom.Call.Details.PROPERTY_EMERGENCY_CALLBACK_MODE,

        Connection.PROPERTY_IS_EXTERNAL_CALL,
        android.telecom.Call.Details.PROPERTY_IS_EXTERNAL_CALL,

        Connection.PROPERTY_HAS_CDMA_VOICE_PRIVACY,
        android.telecom.Call.Details.PROPERTY_HAS_CDMA_VOICE_PRIVACY,

        Connection.PROPERTY_SELF_MANAGED,
        android.telecom.Call.Details.PROPERTY_SELF_MANAGED,

        Connection.PROPERTY_ASSISTED_DIALING_USED,
        android.telecom.Call.Details.PROPERTY_ASSISTED_DIALING_USED,

        Connection.PROPERTY_IS_RTT,
        android.telecom.Call.Details.PROPERTY_RTT,

        Connection.PROPERTY_NETWORK_IDENTIFIED_EMERGENCY_CALL,
        android.telecom.Call.Details.PROPERTY_NETWORK_IDENTIFIED_EMERGENCY_CALL,


        /// M: MTK property pairs. @{
        MtkConnection.PROPERTY_VOICE_RECORDING,
        MtkCall.MtkDetails.MTK_PROPERTY_VOICE_RECORDING,

        MtkConnection.PROPERTY_VOLTE,
        MtkCall.MtkDetails.MTK_PROPERTY_VOLTE,

        MtkConnection.PROPERTY_CDMA,
        MtkCall.MtkDetails.MTK_PROPERTY_CDMA,

        MtkConnection.PROPERTY_CONFERENCE_PARTICIPANT,
        MtkCall.MtkDetails.PROPERTY_CONFERENCE_PARTICIPANT
        /// M: @}
    };

    private static int convertConnectionToCallProperties(int connectionProperties) {
        int callProperties = 0;
        for (int i = 0; i < CONNECTION_TO_CALL_PROPERTIES.length; i += 2) {
            if ((CONNECTION_TO_CALL_PROPERTIES[i] & connectionProperties) ==
                    CONNECTION_TO_CALL_PROPERTIES[i]) {

                callProperties |= CONNECTION_TO_CALL_PROPERTIES[i + 1];
            }
        }
        return callProperties;
    }

    /**
     * Removes the specified capability from the set of capabilities bits and returns the new set.
     */
    private static int removeCapability(int capabilities, int capability) {
        return capabilities & ~capability;
    }

    private static ParcelableRttCall getParcelableRttCall(Call call) {
        if (!call.isRttCall()) {
            return null;
        }
        return new ParcelableRttCall(call.getRttMode(), call.getInCallToCsRttPipeForInCall(),
                call.getCsToInCallRttPipeForInCall());
    }

    private ParcelableCallUtils() {}
}
