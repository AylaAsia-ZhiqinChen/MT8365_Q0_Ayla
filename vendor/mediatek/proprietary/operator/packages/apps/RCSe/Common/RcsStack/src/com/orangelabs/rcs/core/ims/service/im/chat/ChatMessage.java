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

package com.orangelabs.rcs.core.ims.service.im.chat;

import com.gsma.services.rcs.contact.ContactId;

public class ChatMessage {

    private final ContactId mRemote;

    private final String mDisplayName;

    private final String mContent;

    private final String mMimeType;

    /**
     * Local timestamp for both incoming and outgoing message
     */
    private final long mTimestamp;

    /**
     * Timestamp sent in payload for both incoming and outgoing chat message
     */
    private final long mTimestampSent;

    private final String mMsgId;

    /**
     * Constructor for incoming message
     * 
     * @param msgId Message ID
     * @param remote Remote contact
     * @param content Text message
     * @param mimeType MIME type
     * @param timestamp Local timestamp for both incoming and outgoing chat message
     * @param timestampSent Timestamp sent in payload for both incoming and outgoing chat message
     * @param displayName the name to display
     */
    public ChatMessage(String msgId, ContactId remote, String content, String mimeType,
            long timestamp, long timestampSent, String displayName) {
        mMsgId = msgId;
        mRemote = remote;
        mContent = content;
        mMimeType = mimeType;
        mTimestamp = timestamp;
        mTimestampSent = timestampSent;
        mDisplayName = displayName;
    }

    /**
     * Gets the message MIME-type
     * 
     * @return MIME-type
     */
    public String getMimeType() {
        return mMimeType;
    }

    /**
     * Gets the message content
     * 
     * @return message content
     */
    public String getContent() {
        return mContent;
    }

    /**
     * Gets the message ID
     * 
     * @return message ID
     */
    public String getMessageId() {
        return mMsgId;
    }

    /**
     * Gets the remote user
     * 
     * @return the remote contact
     */
    public ContactId getRemoteContact() {
        return mRemote;
    }

    /**
     * Gets the local timestamp of when the chat message was sent and/or queued for outgoing
     * messages or the local timestamp of when the chat message was received for incoming messages..
     * 
     * @return timestamp
     */
    public long getTimestamp() {
        return mTimestamp;
    }

    /**
     * Get the local timestamp of when the chat message was sent and/or queued for outgoing messages
     * or the remote timestamp of when the chat message was sent for incoming messages.
     * 
     * @return timestamp sent in payload
     */
    public long getTimestampSent() {
        return mTimestampSent;
    }

    /**
     * Gets the remote display name
     * 
     * @return remote display name
     */
    public String getDisplayName() {
        return mDisplayName;
    }

    @Override
    public String toString() {
        if (mContent != null && mContent.length() < 30) {
            return "IM [from=" + mRemote + ", pseudo='" + mDisplayName + "', msg='" + mContent
                    + "', msgId=" + mMsgId + "', mimeType='" + mMimeType + "']" + "]";
        }
        return "IM [from=" + mRemote + ", pseudo='" + mDisplayName + "', msgId=" + mMsgId
                + "', mimeType='" + mMimeType + "']";
    }
}
