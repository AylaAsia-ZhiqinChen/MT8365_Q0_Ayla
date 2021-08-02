/*******************************************************************************

 * Software Name : RCS IMS Stack
 *
 * Copyright (C) 2010-2016 Orange.
 * Copyright (C) 2014 Sony Mobile Communications Inc.
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
 *
 * NOTE: This file has been modified by Sony Mobile Communications Inc.
 * Modifications are licensed under the License.
 ******************************************************************************/

package com.orangelabs.rcs.core.ims.service.sip.streaming;

import com.orangelabs.rcs.core.ims.ImsModule;
import com.orangelabs.rcs.core.ims.network.NetworkException;
import com.orangelabs.rcs.core.ims.protocol.PayloadException;
import com.orangelabs.rcs.core.ims.protocol.sip.SipDialogPath;
import com.orangelabs.rcs.core.ims.protocol.sip.SipRequest;
import com.orangelabs.rcs.core.ims.protocol.sip.SipResponse;
import com.orangelabs.rcs.core.ims.protocol.sip.SipTransactionContext;
import com.orangelabs.rcs.core.ims.service.ImsSessionListener;
import com.orangelabs.rcs.core.ims.service.SessionTimerManager;
import com.orangelabs.rcs.core.ims.service.sip.GenericSipSession;
import com.orangelabs.rcs.core.ims.service.sip.SipService;
import com.orangelabs.rcs.core.ims.service.sip.SipSessionError;
import com.orangelabs.rcs.core.ims.service.sip.SipSessionListener;
import com.orangelabs.rcs.utils.logger.Logger;
import com.gsma.services.rcs.contact.ContactId;


import android.content.Intent;

import java.util.Collection;
import com.orangelabs.rcs.core.CoreException;

/**
 * Terminating SIP RTP session
 * 
 * @author jexa7410
 */
public class TerminatingSipRtpSession extends GenericSipRtpSession {

    private static final Logger sLogger = Logger.getLogger(TerminatingSipRtpSession.class
            .getSimpleName());

    private final Intent mSessionInvite;

    private final ImsModule mImsModule;

    /**
     * Constructor
     * 
     * @param parent SIP service
     * @param invite Initial INVITE request
     * @param imsModule
     * @param contact
     * @param sessionInvite
     * @param timestamp Local timestamp for the session
     */
    public TerminatingSipRtpSession(SipService parent, SipRequest invite, ImsModule imsModule,
            ContactId contact, Intent sessionInvite ) throws CoreException {
        super(parent, contact, /*GenericSipSession.getIariFeatureTag(invite.getFeatureTags())*/null,
                GenericSipRtpSession.getEncoding(invite.getContent()));

        mSessionInvite = sessionInvite;
        mImsModule = imsModule;
        /* Create dialog path */
        createTerminatingDialogPath(invite);
    }

    @Override
    public void run() {
        boolean logActivated = sLogger.isActivated();
        if (logActivated) {
            sLogger.info("Initiate a new RTP session as terminating");
        }
        try {
            SipDialogPath dialogPath = getDialogPath();
            send180Ringing(dialogPath.getInvite(), dialogPath.getLocalTag());

            Collection<ImsSessionListener> listeners = getListeners();
            ContactId contact = getRemoteContactId();
            for (ImsSessionListener listener : listeners) {
                ((SipSessionListener) listener).onInvitationReceived(contact, mSessionInvite);
            }

            int answer = waitInvitationAnswer();
            switch (answer) {
                case INVITATION_REJECTED_DECLINE:
                    /* Intentional fall through */
                case INVITATION_REJECTED_BUSY_HERE:
                    if (logActivated) {
                        sLogger.debug("Session has been rejected by user");
                    }
                    sendErrorResponse(dialogPath.getInvite(), dialogPath.getLocalTag(), answer);

                    removeSession();

                    for (ImsSessionListener listener : listeners) {
                        listener.handleSessionTerminatedByRemote();
                    }
                    return;

                case INVITATION_TIMEOUT:
                    if (logActivated) {
                        sLogger.debug("Session has been rejected on timeout");
                    }

                    // Ringing period timeout
                    send486Busy(dialogPath.getInvite(), dialogPath.getLocalTag());

                    removeSession();

                    for (ImsSessionListener listener : listeners) {
                        listener.handleSessionTerminatedByRemote();

                    }
                    return;

                case INVITATION_REJECTED_BY_SYSTEM:
                    if (logActivated) {
                        sLogger.debug("Session has been aborted by system");
                    }
                    removeSession();
                    return;

                case INVITATION_CANCELED:
                    if (logActivated) {
                        sLogger.debug("Session has been canceled");
                    }
                    removeSession();
                    for (ImsSessionListener listener : listeners) {
                        listener.handleSessionTerminatedByRemote();
                    }
                    return;

                case INVITATION_ACCEPTED:
                    setSessionAccepted();
                    for (ImsSessionListener listener : listeners) {
                        listener.handleSessionStarted();
                    }
                    break;

                case INVITATION_DELETED:
                    if (logActivated) {
                        sLogger.debug("Session has been deleted");
                    }
                    removeSession();
                    return;

                default:
                    throw new IllegalArgumentException(new StringBuilder(
                            "Unknown invitation answer in run; answer=").append(answer).toString());
            }

            String sdp = generateSdp();
            
            /* Set the local SDP part in the dialog path */
            dialogPath.setLocalContent(sdp);

            /* Test if the session should be interrupted */
            if (isInterrupted()) {
                if (logActivated) {
                    sLogger.debug("Session has been interrupted: end of processing");
                }
                return;
            }

            /* Prepare Media Session */
            prepareMediaSession();

            /* Create a 200 OK response */
            if (logActivated) {
                sLogger.info("Send 200 OK");
            }
            SipResponse resp = create200OKResponse();

            /* The signalisation is established */
            dialogPath.sigEstablished();

            /* Send response */
            SipTransactionContext ctx = mImsModule.getSipManager().sendSipMessageAndWait(resp);

            /* Analyze the received response */
            if (ctx.isSipAck()) {
                if (logActivated) {
                    sLogger.info("ACK request received");
                }

                /* The session is established */
                dialogPath.sessionEstablished();

                /* Start Media transfer */
                startMediaTransfer();

                /* Start session timer */
                SessionTimerManager sessionTimerManager = getSessionTimerManager();
                if (sessionTimerManager.isSessionTimerActivated(resp)) {
                    sessionTimerManager.start(SessionTimerManager.UAS_ROLE,
                            dialogPath.getSessionExpireTime());
                }

                for (ImsSessionListener listener : getListeners()) {
                    listener.handleSessionStarted();
                }

            } else {
                if (logActivated) {
                    sLogger.debug("No ACK received for INVITE");
                }

                // No response received: timeout
                handleError(new SipSessionError(SipSessionError.SESSION_INITIATION_FAILED));
            }

        } catch (Exception e) {
            sLogger.error(
                    new StringBuilder("Session initiation has failed for CallId=")
                            .append(getDialogPath().getCallId()).append(" ContactId=")
                            .append(getRemoteContact()).toString(), e);
            handleError(new SipSessionError(SipSessionError.MEDIA_FAILED, e));

        }
    }

   // @Override
    public boolean isInitiatedByRemote() {
        return true;
    }

   // @Override
    public void handleInactivityEvent() {
        /* Not need in this class */
    }
    
    public  void startMediaSession() throws Exception{
    		
    }

}
