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
package com.gsma.services.rcs.chat;

import com.gsma.services.rcs.chat.ChatLog.Message.Content.ReasonCode;
import com.gsma.services.rcs.chat.ChatLog.Message.Content.Status;
import com.gsma.services.rcs.contact.ContactId;

import java.util.Set;

/**
 * Chat event listener
 *
 * @author Jean-Marc AUFFRET
 */
public abstract class OneToOneChatListener {
    /**
     * Callback called when a message status/reasonCode is changed.
     * 
     * @param contact Contact ID
     * @param mimeType MIME-type of message
     * @param msgId Message Id
     * @param status Status
     * @param reasonCode Reason code
     */
    public abstract void onMessageStatusChanged(ContactId contact, String mimeType, String msgId,
            Status status, ReasonCode reasonCode);

    /**
     * Callback called when an Is-composing event has been received. If the remote is typing a
     * message the status is set to true, else it is false.
     * 
     * @param contact Contact ID
     * @param status Is-composing status
     */
    public abstract void onComposingEvent(ContactId contact, boolean status);

    /**
     * Callback called when a delete operation completed that resulted in that one or several one to
     * one chat messages was deleted specified by the msgIds parameter corresponding to a specific
     * contact.
     *
     * @param contact contact id of those deleted messages
     * @param msgIds message ids of those deleted messages
     */
    public abstract void onMessagesDeleted(ContactId contact, Set<String> msgIds);
    
    /**
     * Callback called when a new message has been received
     *
     * @param message Chat message
     * @see ChatMessage
     *//*
    public abstract void onNewMessage(ChatMessage message);

    *//**
     * Callback called when a new geoloc has been received
     *
     * @param message Geoloc message
     *//*
    public abstract void onNewGeoloc(GeolocMessage message);

    *//**
     * Callback called when a message has been delivered to the remote
     *
     * @param msgId Message ID
     *//*
    public abstract void onReportMessageDelivered(String msgId);

    *//**
     * Callback called when a message has been displayed by the remote
     *
     * @param msgId Message ID
     *//*
    public abstract void onReportMessageDisplayed(String msgId);

    *//**
     * Callback called when a message has failed to be delivered to the remote
     *
     * @param msgId Message ID
     *//*
    public abstract void onReportMessageFailed(String msgId);

    *//**
     * Callback called when a message has failed to be delivered to the remote
     *
     * @param msgId Message ID
     *//*
    public abstract void onReportMessageSent(String msgId);
    
     *//**
     * Callback called when a message has failed to be delivered to the remote
     *
     * @param msgId      Message ID
     * @param errorCode  Error code
     * @param statusCode Status Code
     *//*
    public abstract void onReportFailedMessage(String msgId, int errorCode, String statusCode);

    *//**
     * Callback called when an Is-composing event has been received. If the
     * remote is typing a message the status is set to true, else it is false.
     *
     * @param status Is-composing status
     *//*
    public abstract void onComposingEvent(boolean status);*/
}
