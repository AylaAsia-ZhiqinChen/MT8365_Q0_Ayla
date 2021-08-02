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

package com.orangelabs.rcs.core.ims.service.richcall.geoloc;

import static com.orangelabs.rcs.utils.StringUtils.UTF8;

import com.orangelabs.rcs.core.FileAccessException;
import com.orangelabs.rcs.core.content.GeolocContent;
import com.orangelabs.rcs.core.content.MmContent;
import com.orangelabs.rcs.core.ims.network.NetworkException;
import com.orangelabs.rcs.core.ims.network.sip.SipUtils;
import com.orangelabs.rcs.core.ims.protocol.PayloadException;
import com.orangelabs.rcs.core.ims.protocol.msrp.MsrpEventListener;
import com.orangelabs.rcs.core.ims.protocol.msrp.MsrpException;
import com.orangelabs.rcs.core.ims.protocol.msrp.MsrpManager;
import com.orangelabs.rcs.core.ims.protocol.msrp.MsrpSession;
import com.orangelabs.rcs.core.ims.protocol.sdp.SdpUtils;
import com.orangelabs.rcs.core.ims.protocol.sip.SipRequest;
import com.orangelabs.rcs.core.ims.protocol.sip.SipResponse;
import com.orangelabs.rcs.core.ims.service.ImsSessionListener;
import com.orangelabs.rcs.core.ims.service.capability.CapabilityService;
import com.orangelabs.rcs.core.ims.service.richcall.ContentSharingError;
import com.orangelabs.rcs.core.ims.service.richcall.RichcallService;
import com.orangelabs.rcs.provider.settings.RcsSettings;
import com.orangelabs.rcs.utils.logger.Logger;
import com.gsma.services.rcs.Geoloc;
import com.gsma.services.rcs.contact.ContactId;

import android.net.Uri;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.text.ParseException;

import javax2.sip.InvalidArgumentException;

/**
 * Originating geoloc sharing session (transfer)
 * 
 * @author jexa7410
 */
public class OriginatingGeolocTransferSession extends GeolocTransferSession implements
        MsrpEventListener {

    private MsrpManager msrpMgr;

    private static final Logger sLogger = Logger.getLogger(OriginatingGeolocTransferSession.class
            .getName());

    /**
     * Constructor
     * 
     * @param parent Richcall service
     * @param content Content to be shared
     * @param contact Remote contact Id
     * @param geoloc Geoloc info
     * @param rcsSettings The RCS settings accessor
     * @param timestamp Local timestamp for the session
     * @param contactManager The contact manager accessor
     * @param capabilityService The capability service
     */
    public OriginatingGeolocTransferSession(RichcallService parent, MmContent content,
            ContactId contact, Geoloc geoloc, long timestamp,
            CapabilityService capabilityService) {
        super(parent, content, contact, timestamp, capabilityService);
        createOriginatingDialogPath();
        setGeoloc(geoloc);
    }

    @Override
    public void run() {
        try {
            if (sLogger.isActivated()) {
                sLogger.info("Initiate a new sharing session as originating");
            }

            // Set setup mode
            String localSetup = createMobileToMobileSetupOffer();
            if (sLogger.isActivated()) {
                sLogger.debug("Local setup attribute is " + localSetup);
            }

            // Set local port
            int localMsrpPort = 9; // See RFC4145, Page 4

            // Create the MSRP manager
            String localIpAddress = getImsService().getImsModule().getCurrentNetworkInterface()
                    .getNetworkAccess().getIpAddress();
            msrpMgr = new MsrpManager(localIpAddress, localMsrpPort);

            // Build SDP part
            String ntpTime = SipUtils.constructNTPtime(System.currentTimeMillis());
            String ipAddress = getDialogPath().getSipStack().getLocalIpAddress();
            String sdp = "v=0" + SipUtils.CRLF + "o=- " + ntpTime + " " + ntpTime + " "
                    + SdpUtils.formatAddressType(ipAddress) + SipUtils.CRLF + "s=-" + SipUtils.CRLF
                    + "c=" + SdpUtils.formatAddressType(ipAddress) + SipUtils.CRLF + "t=0 0"
                    + SipUtils.CRLF + "m=message " + localMsrpPort + " TCP/MSRP *" + SipUtils.CRLF
                    + "a=path:" + msrpMgr.getLocalMsrpPath() + SipUtils.CRLF + "a=setup:"
                    + localSetup + SipUtils.CRLF + "a=accept-types:" + getContent().getEncoding()
                    + SipUtils.CRLF + "a=file-transfer-id:" + getFileTransferId() + SipUtils.CRLF
                    + "a=file-disposition:render" + SipUtils.CRLF + "a=sendonly" + SipUtils.CRLF;

            // Set File-selector attribute
            String selector = getFileSelectorAttribute();
            if (selector != null) {
                sdp += "a=file-selector:" + selector + SipUtils.CRLF;
            }

            // Set File-location attribute
            Uri location = getFileLocationAttribute();
            if (location != null) {
                sdp += "a=file-location:" + location.toString() + SipUtils.CRLF;
            }

            // Set the local SDP part in the dialog path
            getDialogPath().setLocalContent(sdp);

            // Create an INVITE request
            if (sLogger.isActivated()) {
                sLogger.info("Send INVITE");
            }
            SipRequest invite = createInvite();

            // Set the Authorization header
            getAuthenticationAgent().setAuthorizationHeader(invite);

            // Set initial request in the dialog path
            getDialogPath().setInvite(invite);

            // Send INVITE request
            sendInvite(invite);

        } catch (Exception e) {
            /**
             * Intentionally catch runtime exceptions as else it will abruptly end the thread and
             * eventually bring the whole system down, which is not intended.
             */
            sLogger.error("Failed initiate a new sharing session as originating!", e);
            handleError(new ContentSharingError(ContentSharingError.SESSION_INITIATION_FAILED, e));
        }

        if (sLogger.isActivated()) {
            sLogger.debug("End of thread");
        }
    }

    @Override
    public void prepareMediaSession() {
        // Changed by Deutsche Telekom
        // Get the remote SDP part
        byte[] sdp = getDialogPath().getRemoteContent().getBytes(UTF8);

        // Changed by Deutsche Telekom
        // Create the MSRP session
        MsrpSession session = msrpMgr.createMsrpSession(sdp, this);
        session.setFailureReportOption(true);
        session.setSuccessReportOption(false);
    }

    //@Override
    public void openMediaSession() throws NetworkException, PayloadException {
        try {
            msrpMgr.openMsrpSession();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    //@Override
    public void startMediaTransfer() throws NetworkException {
        /* Start sending data chunks */
        byte[] data = ((GeolocContent) getContent()).getData();
        InputStream stream = new ByteArrayInputStream(data);
        try {
            msrpMgr.sendChunks(stream, getFileTransferId(), getContent().getEncoding(), getContent()
                    .getSize());
        } catch (MsrpException e) {
            e.printStackTrace();
        }

    }

    @Override
    public void closeMediaSession() {
        if (msrpMgr != null) {
            msrpMgr.closeSession();
        }
        if (sLogger.isActivated()) {
            sLogger.debug("MSRP session has been closed");
        }
    }

    @Override
    public void msrpDataTransfered(String msgId) {
        try {
            if (sLogger.isActivated()) {
                sLogger.info("Data transferred");
            }
            setGeolocTransferred();
            closeMediaSession();
            closeSession(TerminationReason.TERMINATION_BY_USER);
            removeSession();
            ContactId contact = getRemoteContactId();
            Geoloc geoloc = getGeoloc();
            boolean initiatedByRemote = isInitiatedByRemote();
            for (ImsSessionListener listener : getListeners()) {
                ((GeolocTransferSessionListener) listener).onContentTransferred(contact, geoloc,
                        initiatedByRemote);
            }
        } catch (Exception e) {
            /*
             * Normally we are not allowed to catch runtime exceptions as these are genuine bugs
             * which should be handled/fixed within the code. However the cases when we are
             * executing operations on a thread unhandling such exceptions will eventually lead to
             * exit the system and thus can bring the whole system down, which is not intended.
             */
            sLogger.error(new StringBuilder("Failed to notify msrp data transfered for msgId : ")
                    .append(msgId).toString(), e);
        }
    }

    /*@Override
    public void receiveMsrpData(String msgId, byte[] data, String mimeType) {
        // Not used in originating side
    }*/

    @Override
    public void msrpTransferProgress(long currentSize, long totalSize) {
        // Not used for geolocation sharing
    }

    @Override
    public boolean msrpTransferProgress(long currentSize, long totalSize, byte[] data) {
        // Not used in originating side
        return false;
    }

    @Override
    public void msrpTransferAborted() {
        if (sLogger.isActivated()) {
            sLogger.info("Data transfer aborted");
        }
    }

    @Override
    public void msrpTransferError(String msgId, String error) {
        try {
            if (isSessionInterrupted()) {
                return;
            }
            if (sLogger.isActivated()) {
                sLogger.info("Data transfer error " + error);
            }
            closeMediaSession();
            closeSession(TerminationReason.TERMINATION_BY_SYSTEM);
            ContactId contact = getRemoteContactId();
            mCapabilityService.requestContactCapabilities(contact.toString());
            removeSession();
            for (ImsSessionListener listener : getListeners()) {
                ((GeolocTransferSessionListener) listener).onSharingError(contact,
                        new ContentSharingError(ContentSharingError.MEDIA_TRANSFER_FAILED));
            }
        } catch (Exception e) {
            /*
             * Normally we are not allowed to catch runtime exceptions as these are genuine bugs
             * which should be handled/fixed within the code. However the cases when we are
             * executing operations on a thread unhandling such exceptions will eventually lead to
             * exit the system and thus can bring the whole system down, which is not intended.
             */
            sLogger.error(
                    new StringBuilder("Failed to handle msrp error").append(error)
                            .append(" for message ").append(msgId).toString(), e);
        }
    }

    @Override
    public boolean isInitiatedByRemote() {
        return false;
    }

    @Override
    public void handle180Ringing(SipResponse response) {
        if (sLogger.isActivated()) {
            sLogger.debug("handle180Ringing");
        }
        ContactId contact = getRemoteContactId();
        for (ImsSessionListener listener : getListeners()) {
            ((GeolocTransferSessionListener) listener).onSessionRinging(contact);
        }
    }
    
    @Override
    public void msrpDataReceived(String msgId, byte[] data, String mimeType) {
        // Not used in originating side
    }
}
