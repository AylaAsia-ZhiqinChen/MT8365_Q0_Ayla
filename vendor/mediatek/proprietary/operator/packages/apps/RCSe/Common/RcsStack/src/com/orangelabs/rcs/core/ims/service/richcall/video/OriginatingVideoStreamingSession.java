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

package com.orangelabs.rcs.core.ims.service.richcall.video;

import static com.orangelabs.rcs.utils.StringUtils.UTF8;

import com.orangelabs.rcs.core.FileAccessException;
import com.orangelabs.rcs.core.content.MmContent;
import com.orangelabs.rcs.core.ims.network.NetworkException;
import com.orangelabs.rcs.core.ims.network.sip.SipMessageFactory;
import com.orangelabs.rcs.core.ims.protocol.PayloadException;
import com.orangelabs.rcs.core.ims.protocol.sdp.MediaDescription;
import com.orangelabs.rcs.core.ims.protocol.sdp.SdpParser;
import com.orangelabs.rcs.core.ims.protocol.sdp.SdpUtils;
import com.orangelabs.rcs.core.ims.protocol.sip.SipDialogPath;
import com.orangelabs.rcs.core.ims.protocol.sip.SipRequest;
import com.orangelabs.rcs.core.ims.protocol.sip.SipResponse;
import com.orangelabs.rcs.core.ims.service.ImsSessionListener;
import com.orangelabs.rcs.core.ims.service.capability.CapabilityService;
import com.orangelabs.rcs.core.ims.service.richcall.ContentSharingError;
import com.orangelabs.rcs.core.ims.service.richcall.RichcallService;
/*import com.orangelabs.rcs.provider.contact.ContactManager;*/
import com.orangelabs.rcs.provider.settings.RcsSettings;
import com.orangelabs.rcs.utils.logger.Logger;
import com.gsma.services.rcs.contact.ContactId;
import com.gsma.services.rcs.sharing.video.IVideoPlayer;
import com.gsma.services.rcs.sharing.video.VideoCodec;

import android.os.RemoteException;

import java.text.ParseException;
import java.util.Vector;

import javax2.sip.InvalidArgumentException;

/**
 * Originating video content sharing session (streaming)
 * 
 * @author hlxn7157
 */
public class OriginatingVideoStreamingSession extends VideoStreamingSession {

    private static final Logger sLogger = Logger.getLogger(OriginatingVideoStreamingSession.class
            .getName());

    /**
     * Constructor
     * 
     * @param parent IMS service
     * @param player Media player
     * @param content Content to be shared
     * @param contact Remote contact Id
     * @param rcsSettings
     * @param timestamp Local timestamp for the session
     * @param contactManager
     * @param capabilityService
     */
    public OriginatingVideoStreamingSession(RichcallService parent, IVideoPlayer player,
            MmContent content, ContactId contact, long timestamp,
            CapabilityService capabilityService) {
        super(parent, content, contact, timestamp, capabilityService);
        createOriginatingDialogPath();
        setPlayer(player);
    }

    @Override
    public void run() {
        try {
            if (sLogger.isActivated()) {
                sLogger.info("Initiate a new live video sharing session as originating");
            }

            SipDialogPath dialogPath = getDialogPath();
            // Build SDP part
            String ipAddress = dialogPath.getSipStack().getLocalIpAddress();
            IVideoPlayer player = getPlayer();
            String videoSdp = VideoSdpBuilder.buildSdpOfferWithOrientation(
                    player.getSupportedCodecs(), player.getLocalRtpPort());
            String sdp = SdpUtils.buildVideoSDP(ipAddress, videoSdp, SdpUtils.DIRECTION_SENDONLY);

            // Set the local SDP part in the dialog path
            dialogPath.setLocalContent(sdp);

            // Create an INVITE request
            if (sLogger.isActivated()) {
                sLogger.info("Send INVITE");
            }
            SipRequest invite = SipMessageFactory.createInvite(dialogPath,
                    RichcallService.FEATURE_TAGS_VIDEO_SHARE, sdp);

            // Set the Authorization header
            getAuthenticationAgent().setAuthorizationHeader(invite);

            // Set initial request in the dialog path
            dialogPath.setInvite(invite);

            // Send INVITE request
            sendInvite(invite);

        } catch (Exception e) {
            sLogger.error("Failed to send invite!", e);
            handleError(new ContentSharingError(ContentSharingError.SESSION_INITIATION_FAILED, e));

        }
    }

    @Override
    public void prepareMediaSession() throws PayloadException, NetworkException {
        // Parse the remote SDP part
        SdpParser parser = new SdpParser(getDialogPath().getRemoteContent().getBytes(UTF8));
        MediaDescription mediaVideo = parser.getMediaDescription("video");
        String remoteHost = SdpUtils.extractRemoteHost(parser.sessionDescription, mediaVideo);
        int remotePort = mediaVideo.port;

        // Extract video codecs from SDP
        Vector<MediaDescription> medias = parser.getMediaDescriptions("video");
        Vector<VideoCodec> proposedCodecs = VideoCodecManager.extractVideoCodecsFromSdp(medias);

        IVideoPlayer player = getPlayer();
        try {
            // Codec negotiation
            VideoCodec selectedVideoCodec = VideoCodecManager.negociateVideoCodec(
                    player.getSupportedCodecs(), proposedCodecs);

            if (selectedVideoCodec == null) {
                if (sLogger.isActivated()) {
                    sLogger.debug("Proposed codecs are not supported");
                }

                closeSession(TerminationReason.TERMINATION_BY_SYSTEM);

                // Report error
                handleError(new ContentSharingError(ContentSharingError.UNSUPPORTED_MEDIA_TYPE));
                return;
            }
            getContent().setEncoding("video/" + selectedVideoCodec.getEncoding());

            // Set the video player orientation
            SdpOrientationExtension extensionHeader = SdpOrientationExtension.create(mediaVideo);
            if (extensionHeader != null) {
                // Update the orientation ID
                setOrientation(extensionHeader.getExtensionId());
            }
            // Set the video player remote info
            player.setRemoteInfo(selectedVideoCodec, remoteHost, remotePort, getOrientation());

        } catch (Exception e) {
            throw new IllegalArgumentException("Error when preparing the media session", e);
        }
    }

   // @Override
    public void openMediaSession() {
        /* Nothing to do in case of external codec */
    }

   // @Override
    public void startMediaTransfer() {
        /* Nothing to do in case of external codec */
    }

    @Override
    public void closeMediaSession() {
        /* Nothing to do in case of external codec */
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
            ((VideoStreamingSessionListener) listener).onSessionRinging(contact);
        }
    }
}
