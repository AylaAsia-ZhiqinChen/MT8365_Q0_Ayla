/*******************************************************************************
 * Software Name : RCS IMS Stack
 *
 * Copyright (C) 2010 France Telecom S.A.
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
 ******************************************************************************/

package com.mediatek.presence.core.ims.network.sip;

import java.util.ListIterator;

import javax2.sip.header.ViaHeader;
import javax2.sip.header.WarningHeader;
import javax2.sip.message.Request;

import com.mediatek.presence.core.ims.network.ImsNetworkInterface;
import com.mediatek.presence.core.ims.protocol.sip.KeepAliveManager;
import com.mediatek.presence.core.ims.protocol.sip.RSTException;
import com.mediatek.presence.core.ims.protocol.sip.SipDialogPath;
import com.mediatek.presence.core.ims.protocol.sip.SipException;
import com.mediatek.presence.core.ims.protocol.sip.SipInterface;
import com.mediatek.presence.core.ims.protocol.sip.SipMessage;
import com.mediatek.presence.core.ims.protocol.sip.SipRequest;
import com.mediatek.presence.core.ims.protocol.sip.SipResponse;
import com.mediatek.presence.core.ims.protocol.sip.SipTransactionContext;
import com.mediatek.presence.provider.settings.RcsSettings;
import com.mediatek.presence.provider.settings.RcsSettingsManager;
import com.mediatek.presence.utils.logger.Logger;

/**
 * SIP manager
 *
 * @author JM. Auffret
 */
public class SipManager {

    /**
     * SIP timeout for SIP transaction (in seconds)
     */
    public static int TIMEOUT = 0;

    /**
     * IMS network interface
     */
    private ImsNetworkInterface networkInterface;

    /**
     * SIP stack
     */
    private SipInterface sipstack = null;

    /**
     * The logger
     */
    private Logger logger = null;

    private int mSlotId = 0;

    /**
     * Constructor
     *
     * @param parent IMS network interface
     */
    public SipManager(ImsNetworkInterface parent) {
        this.networkInterface = parent;
        mSlotId = parent.getImsModule().getSlotId();
        logger = Logger.getLogger(mSlotId, "SipManager");
        TIMEOUT = RcsSettingsManager.getRcsSettingsInstance(mSlotId)
                .getSipTransactionTimeout();

        if (logger.isActivated()) {
            logger.info("SIP manager started");
        }
    }

    /**
     * Returns the network interface
     *
     * @return Network interface
     */
    public ImsNetworkInterface getNetworkInterface() {
        return networkInterface;
    }

    /**
     * Returns the SIP stack
     *
     * @return SIP stack
     */
    public SipInterface getSipStack() {
        return sipstack;
    }

    /**
     * Terminate the manager
     */
    public void terminate() {
        if (logger.isActivated()) {
            logger.info("Terminate the SIP manager");
        }

        // Close the SIP stack
        if (sipstack != null) {
            closeStack();
        }

        if (logger.isActivated()) {
            logger.info("SIP manager has been terminated");
        }
    }

    /**
     * Initialize the SIP stack
     *
     * @param slotId Slot index
     * @param localAddr Local IP address
     * @param proxyAddr Outbound proxy address
     * @param proxyPort Outbound proxy port
     * @param isSecure Need secure connection or not
     * @param networkType type of network
     * @return SIP stack
     * @throws SipException
     */
    public synchronized void initStack(String localAddr, String proxyAddr,
            int proxyPort, String protocol, int networkType) throws SipException {
        // Close the stack if necessary
        closeStack();

        // Create the SIP stack
        sipstack = new SipInterface(
                networkInterface.getImsModule(), localAddr,
                proxyAddr, proxyPort, protocol, networkType);
    }

    /**
     * Close the SIP stack
     */
    public synchronized void closeStack() {
        if (sipstack == null) {
            // Already closed
            return;
        }

        try {
            // Close the SIP stack
            sipstack.close();
            sipstack = null;
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Can't close SIP stack properly", e);
            }
        }
    }

    /**
     * Send a SIP message and create a context to wait a response
     *
     * @param message SIP message
     * @return Transaction context
     * @throws SipException
     */
    public SipTransactionContext sendSipMessageAndWait(SipMessage message) throws SipException {
        return sendSipMessageAndWait(message, SipManager.TIMEOUT);
    }

    /**
     * Send a SIP message and create a context to wait a response
     *
     * @param message SIP message
     * @param timeout SIP timeout
     * @return Transaction context
     * @throws SipException
     */
    public SipTransactionContext sendSipMessageAndWait(SipMessage message, int timeout) throws SipException {
        if (sipstack != null) {
            SipTransactionContext ctx = null;
            ctx = sipstack.sendSipMessageAndWait(message);

            // wait the response
            ctx.waitResponse(timeout);

            // Analyze the received response
            if (    message instanceof SipRequest
                    && !((SipRequest)message).getMethod().equals(Request.REGISTER)
                    && !((SipRequest)message).getMethod().equals(Request.OPTIONS)
                    && ctx.isSipResponse()) {
                // Check if not registered and warning header
                WarningHeader warn = (WarningHeader) ctx.getSipResponse().getHeader(WarningHeader.NAME);
                if ((ctx.getStatusCode() == 403) && (warn == null) &&
                    !RcsSettingsManager.getRcsSettingsInstance(mSlotId).isSingleRegistrationFeasible()) {
                    // Launch new registration
                    networkInterface.getRegistrationManager().restart();

                    // Throw not registered exception
                    throw new SipException("Not registered");
                }
            }

            KeepAliveManager keepAliveManager = networkInterface.getSipManager().getSipStack().getKeepAliveManager();
            keepAliveManager.setIMSNetworkInterface(networkInterface);

            if (message instanceof SipRequest && ctx.isSipResponse()) {
                String method = ((SipRequest) message).getMethod();
                if (method != null && keepAliveManager != null) {
                    if (method.equals(Request.INVITE) || method.equals(Request.REGISTER)) {
                        // Message is a response to INVITE or REGISTER: analyze "keep" flag of "Via" header
                        int viaKeep = -1;
                        ListIterator<ViaHeader> iterator = ctx.getSipResponse().getViaHeaders();
                        if (iterator != null) {
                            ViaHeader respViaHeader = iterator.next();
                            // Retrieve "keep" value
                            String keepStr = respViaHeader.getParameter("keep");
                            if (logger.isActivated()){
                                    logger.info("ABC keepAliveManager:" + keepStr );
                            }
                            if (keepStr != null) {
                                // Convert "keep" value to integer
                                try {
                                    viaKeep = Integer.parseInt(keepStr);
                                    if (viaKeep > 0) {
                                        // If "keep" value is valid, set keep alive period
                                        if (logger.isActivated()){
                                            logger.info("ABC1 keepAliveManager:" + keepStr + "viakeep: " + viaKeep);
                                        }
                                        viaKeep = 200;
                                        keepAliveManager.setPeriod(viaKeep);
                                    } else {
                                        if (logger.isActivated())
                                            logger.warn("Non positive keep value \"" + keepStr + "\"");
                                    }
                                } catch (NumberFormatException e) {
                                    if (logger.isActivated())
                                        logger.warn("Non-numeric keep value \"" + keepStr + "\"");
                                }
                            }
                        }
                        // If "keep" value is invalid or not present, set keep alive period to default value
                        if (viaKeep <= 0) {
                            keepAliveManager.setPeriod(RcsSettingsManager.getRcsSettingsInstance(mSlotId)
                                    .getSipKeepAlivePeriod());
                        }
                    }
                }
            }

            // Return the transaction context
            return ctx;
        } else {
            throw new SipException("Stack not initialized");
        }
    }


    /**
     * Send a SIP response
     *
     * @param response SIP response
     * @throws SipException
     */
    public void sendSipResponse(SipResponse response) throws SipException {
        if (sipstack != null) {
            sipstack.sendSipResponse(response);
        } else {
            throw new SipException("Stack not initialized");
        }
    }

    /**
     * Send a SIP ACK
     *
     * @param dialog Dialog path
     * @throws SipException
     */
    public void sendSipAck(SipDialogPath dialog) throws SipException {
        if (sipstack != null) {
            sipstack.sendSipAck(dialog);
        } else {
            throw new SipException("Stack not initialized");
        }
    }

    /**
     * Send a SIP BYE
     *
     * @param dialog Dialog path
     * @throws SipException
     */
    public void sendSipBye(SipDialogPath dialog) throws SipException {
        if (sipstack != null) {
            sipstack.sendSipBye(dialog);
        } else {
            throw new SipException("Stack not initialized");
        }
    }

    /**
     * Send a SIP Group BYE
     *
     * @param dialog Dialog path
     * @throws SipException
     */
    public void sendSipGroupBye(SipDialogPath dialog) throws SipException {
        if (sipstack != null) {
            sipstack.sendSipGroupBye(dialog);
        } else {
            throw new SipException("Stack not initialized");
        }
    }


    /**
     * Send a SIP CANCEL
     *
     * @param dialog Dialog path
     * @throws SipException
     */
    public void sendSipCancel(SipDialogPath dialog) throws SipException {
        if (sipstack != null) {
            sipstack.sendSipCancel(dialog);
        } else {
            throw new SipException("Stack not initialized");
        }
    }

    /**
     * Send a subsequent SIP request
     *
     * @param dialog Dialog path
     * @param request Request
     * @throws SipException
     */
    public SipTransactionContext sendSubsequentRequest(SipDialogPath dialog, SipRequest request) throws SipException {
        return sendSubsequentRequest(dialog, request, SipManager.TIMEOUT);
    }

    /**
     * Send a subsequent SIP request
     *
     * @param dialog Dialog path
     * @param request Request
     * @param timeout SIP timeout
     * @throws SipException
     */
    public SipTransactionContext sendSubsequentRequest(SipDialogPath dialog, SipRequest request, int timeout) throws SipException {
        if (sipstack != null) {
            SipTransactionContext ctx = sipstack.sendSubsequentRequest(dialog, request);

            // wait the response
            ctx.waitResponse(timeout);

            // Analyze the received response
            if (ctx.isSipResponse()) {
                int code = ctx.getStatusCode();
                // Check if not registered and warning header
                WarningHeader warn = (WarningHeader)ctx.getSipResponse().getHeader(WarningHeader.NAME);
                if ((code == 403) && (warn == null)) {
                    // Launch new registration
                    networkInterface.getRegistrationManager().restart();

                    // Throw not registered exception
                    throw new SipException("Not registered");
                }
            }
            return ctx;
        } else {
            throw new SipException("Stack not initialized");
        }
    }
}
