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

/**
 * Intent for group chat conversation
 *
 * @author Jean-Marc AUFFRET
 */
public class GroupChatIntent {  
    /**
     * Broadcast action: a new group chat invitation has been received.
     * <p>Intent includes the following extras:
     * <ul>
     * <li> {@link #EXTRA_CHAT_ID} containing the unique ID of the chat conversation.
     * <li> {sessionId} containing the session Id of the group chat.
     * </ul>
     */
    public final static String ACTION_SESSION_REPLACED = "com.gsma.services.rcs.chat.action.REPLACED_GROUP_CHAT";

    /**
     * Broadcast action: a new group chat invitation has been received.
     * <p>Intent includes the following extras:
     * <ul>
     * <li> {@link #EXTRA_CONTACT} containing the MSISDN of the contact
     *  sending the invitation.
     * <li> {@link #EXTRA_DISPLAY_NAME} containing the display name of the
     *  contact sending the invitation (extracted from the SIP address).
     * <li> {@link #EXTRA_CHAT_ID} containing the unique ID of the chat conversation.
     * <li> {@link #EXTRA_SUBJECT} containing the subject associated to the conversation.
     * </ul>
     */
    public final static String ACTION_REINVITATION = "com.gsma.services.rcs.chat.action.NEW_REINVITATION";

    /**
     * MSISDN of the contact sending the invitation
     */
    public final static String EXTRA_CONTACT = "contact";

    /**
     * Display name of the contact sending the invitation (extracted from the SIP address)
     */
    public final static String EXTRA_DISPLAY_NAME = "contactDisplayname";

    /**
     * Chat message associated with this intent
     */
    public final static String EXTRA_CHAT_MESSAGE = "chatmessage";

    /**
     * Subject associated to the conversation (optional)
     */
    public final static String EXTRA_SUBJECT = "subject";

    /**
     * Session identitiy to the conversation
     */
    public final static String EXTRA_SESSION_IDENTITY = "sessionIdentity";
    
    /**
     * Broadcast action: a new group chat invitation has been received.
     * <p>
     * Intent includes the following extras:
     * <ul>
     * <li> {@link #EXTRA_CHAT_ID} containing the unique ID of the chat conversation.
     * </ul>
     */
    public final static String ACTION_NEW_INVITATION = "com.gsma.services.rcs.chat.action.NEW_GROUP_CHAT";

    /**
     * Broadcast action: a new group chat message has been received.
     * <p>
     * Intent includes the following extras:
     * <ul>
     * <li> {@link #EXTRA_MESSAGE_ID} containing the message ID of chat message.
     * </ul>
     */
    public final static String ACTION_NEW_GROUP_CHAT_MESSAGE = "com.gsma.services.rcs.chat.action.NEW_GROUP_CHAT_MESSAGE";

    /**
     * Unique ID of the chat conversation
     */
    public final static String EXTRA_CHAT_ID = "chatId";

    /**
     * MIME-type of received message
     */
    public final static String EXTRA_MIME_TYPE = "mimeType";

    /**
     * Message ID of received message
     */
    public final static String EXTRA_MESSAGE_ID = "messageId";
}
