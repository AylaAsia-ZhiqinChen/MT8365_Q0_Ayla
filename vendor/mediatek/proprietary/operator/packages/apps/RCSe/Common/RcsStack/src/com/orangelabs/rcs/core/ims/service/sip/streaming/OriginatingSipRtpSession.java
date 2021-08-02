/*******************************************************************************
 * Software Name : RCS IMS Stack
 *
 * Copyright (C) 2010-2016 Orange.
 * Copyright (C) 2015 Sony Mobile Communications Inc.
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

import com.orangelabs.rcs.core.FileAccessException;
import com.orangelabs.rcs.core.ims.network.NetworkException;
import com.orangelabs.rcs.core.ims.protocol.PayloadException;
import com.orangelabs.rcs.core.ims.protocol.sip.SipRequest;
import com.orangelabs.rcs.core.ims.protocol.sip.SipResponse;
import com.orangelabs.rcs.core.ims.service.ImsSessionListener;
import com.orangelabs.rcs.core.ims.service.sip.SipService;
import com.orangelabs.rcs.core.ims.service.sip.SipSessionError;
import com.orangelabs.rcs.core.ims.service.sip.SipSessionListener;
import com.orangelabs.rcs.provider.settings.RcsSettings;
import com.orangelabs.rcs.utils.logger.Logger;
import com.gsma.services.rcs.contact.ContactId;

import java.text.ParseException;
import com.orangelabs.rcs.core.CoreException;


import javax2.sip.InvalidArgumentException;

/**
 * Originating SIP RTP session
 * 
 * @author Jean-Marc AUFFRET
 */
public class OriginatingSipRtpSession extends GenericSipRtpSession {

    private static final Logger sLogger = Logger.getLogger(OriginatingSipRtpSession.class
            .getSimpleName());

    /**
     * Constructor
     * 
     * @param parent SIP service
     * @param contact Remote contact Id
     * @param featureTag Feature tag
     * @param rcsSettings RCS settings
     * @param timestamp Local timestamp for the session
     * @param contactManager Contacts
     * @param encoding Encoding
     */
    public OriginatingSipRtpSession(SipService parent, ContactId contact, String featureTag, String encoding) throws CoreException {
        super(parent, contact, featureTag, encoding);
        /* Create dialog path */
        createOriginatingDialogPath();
    }

    public  void startMediaSession() throws Exception{
    		
    	
    }

    /**
     * Background processing
     */
    public void run() {
        try {
            if (sLogger.isActivated()) {
                sLogger.info("Initiate a new RTP session as originating");
            }
            /* Build SDP part */
            String sdp = generateSdp();
            /* Set the local SDP part in the dialog path */
            getDialogPath().setLocalContent(sdp);
            /* Create an INVITE request */
            if (sLogger.isActivated()) {
                sLogger.info("Send INVITE");
            }
            SipRequest invite = createInvite();
            getAuthenticationAgent().setAuthorizationHeader(invite);
            /* Set initial request in the dialog path */
            getDialogPath().setInvite(invite);
            sendInvite(invite);

        } catch (Exception e) {
            sLogger.error(
                    new StringBuilder("Session initiation has failed for CallId=")
                            .append(getDialogPath().getCallId()).append(" ContactId=")
                            .append(getRemoteContact()).toString(), e);
            handleError(new SipSessionError(SipSessionError.SESSION_INITIATION_FAILED, e));

        } 
    }

    /*@Override*/
    public boolean isInitiatedByRemote() {
        return false;
    }

    /*@Override*/
    public void handle180Ringing(SipResponse response) {
        if (sLogger.isActivated()) {
            sLogger.debug("handle180Ringing");
        }
        ContactId contact = getRemoteContactId();
      /*  for (ImsSessionListener listener : getListeners()) {
            ((SipSessionListener) listener).onSessionRinging(contact);
        }*/
    }

    /*@Override*/
    public void handleInactivityEvent() {
       //  Not need in this class 
    }
}
