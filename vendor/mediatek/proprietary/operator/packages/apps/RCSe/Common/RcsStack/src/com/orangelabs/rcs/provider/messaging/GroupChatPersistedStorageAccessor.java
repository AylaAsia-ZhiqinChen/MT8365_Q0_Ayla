/*
 * Copyright (C) 2014 Sony Mobile Communications Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

package com.orangelabs.rcs.provider.messaging;

import com.orangelabs.rcs.core.ims.service.im.chat.ChatMessage;
import com.orangelabs.rcs.provider.settings.RcsSettings;
import com.orangelabs.rcs.service.api.ServerApiPersistentStorageException;
import com.orangelabs.rcs.utils.ContactUtil;
import com.gsma.services.rcs.RcsService.Direction;
import com.gsma.services.rcs.chat.ChatLog.Message.Content;
import com.gsma.services.rcs.chat.ChatLog.Message.Content.Status;
import com.gsma.services.rcs.chat.ChatLog.GroupChatEvent;
import com.gsma.services.rcs.chat.GroupChat.ParticipantStatus;
import com.gsma.services.rcs.chat.GroupChat.ReasonCode;
import com.gsma.services.rcs.chat.GroupChat.State;
import com.gsma.services.rcs.contact.ContactId;
import com.gsma.services.rcs.groupdelivery.GroupDeliveryInfo;

import android.database.Cursor;

import java.util.Map;
import java.util.Set;

/**
 * GroupChatPersistedStorageAccessor helps in retrieving persisted data related to a group chat from
 * the persisted storage. It can utilize caching for such data that will not be changed after
 * creation of the group chat to speed up consecutive access.
 */
public class GroupChatPersistedStorageAccessor {

    private final String mChatId;

    private final RichMessagingHistory mMessagingLog;

    private String mSubject;

    private Direction mDirection;

    private ContactId mContact;

    private long mTimestamp;

    private final RcsSettings mRcsSettings;

    public GroupChatPersistedStorageAccessor(String chatId, RichMessagingHistory messagingLog,
            RcsSettings rcsSettings) {
        mChatId = chatId;
        mMessagingLog = messagingLog;
        mRcsSettings = rcsSettings;
    }

    private void cacheData() {
        Cursor cursor = null;
        try {
            cursor = mMessagingLog.getGroupChatData(mChatId);
            if (!cursor.moveToNext()) {
                throw new ServerApiPersistentStorageException("Data not found for group chat "
                        + mChatId);
            }
            mSubject = cursor.getString(cursor.getColumnIndexOrThrow(ChatData.KEY_SUBJECT));
            mDirection = Direction.valueOf(cursor.getInt(cursor
                    .getColumnIndexOrThrow(ChatData.KEY_DIRECTION)));
            String contact = cursor.getString(cursor
                    .getColumnIndexOrThrow(ChatData.KEY_CONTACT));
            if (contact != null) {
                mContact = ContactUtil.createContactIdFromTrustedData(contact);
            }
            mTimestamp = cursor.getLong(cursor.getColumnIndexOrThrow(ChatData.KEY_TIMESTAMP));
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
    }

    public Direction getDirection() {
        /*
         * Utilizing cache here as direction can't be changed in persistent storage after entry
         * insertion anyway so no need to query for it multiple times.
         */
        if (mDirection == null) {
            cacheData();
        }
        return mDirection;
    }

    public State getState() {
        State state = mMessagingLog.getGroupChatState(mChatId);
        if (state == null) {
            throw new ServerApiPersistentStorageException("State not found for group chat "
                    + mChatId);
        }
        return state;
    }

    public ReasonCode getReasonCode() {
        ReasonCode reasonCode = mMessagingLog.getGroupChatReasonCode(mChatId);
        if (reasonCode == null) {
            throw new ServerApiPersistentStorageException("Reason code not found for group chat "
                    + mChatId);
        }
        return reasonCode;
    }

    public String getSubject() {
        /*
         * Utilizing cache here as subject can't be changed in persistent storage after entry
         * insertion anyway so no need to query for it multiple times.
         */
        if (mSubject == null) {
            cacheData();
        }
        return mSubject;
    }

    public long getTimestamp() {
        /*
         * Utilizing cache here as timestamp can't be changed in persistent storage after entry
         * insertion anyway so no need to query for it multiple times.
         */
        if (mTimestamp == 0) {
            cacheData();
        }
        return mTimestamp;
    }

    public ContactId getRemoteContact() {
        /* Remote contact is null for outgoing group chat */
        if (Direction.OUTGOING == getDirection()) {
            return null;
        }
        /*
         * Utilizing cache here as remote contact can't be changed in persistent storage after entry
         * insertion anyway so no need to query for it multiple times.
         */
        if (mContact == null) {
            cacheData();
        }
        return mContact;
    }

    public Map<String, ParticipantStatus> getParticipants() {
        return mMessagingLog.getParticipants(mChatId);
    }

    public Map<String, ParticipantStatus> getParticipants(Set<ParticipantStatus> statuses) {
       // return mMessagingLog.getParticipants(mChatId, statuses);
        return null;
    }

    public int getMaxParticipants() {
        return mRcsSettings.getMaxChatParticipants();
    }

    public boolean setStateAndReasonCode(State state, ReasonCode reasonCode) {
        return mMessagingLog.setGroupChatStateAndReasonCode(mChatId, state.toInt(), reasonCode.toInt());
    }

    public boolean setMessageStatusAndReasonCode(String msgId, Status status,
            Content.ReasonCode reasonCode) {
        return mMessagingLog.setChatMessageStatusAndReasonCode(msgId, status.toInt(), reasonCode.toInt());
    }

    public boolean setMessageStatusDelivered(String msgId, long timestampDelivered) {
        return mMessagingLog.setChatMessageStatusDelivered(msgId, timestampDelivered);
    }

    public boolean setMessageStatusDisplayed(String msgId, long timestampDisplayed) {
        return mMessagingLog.setChatMessageStatusDisplayed(msgId, timestampDisplayed);
    }

    public boolean setGroupDeliveryInfoStatusAndReasonCode(String chatId, ContactId contact,
            String msgId, GroupDeliveryInfo.Status status, GroupDeliveryInfo.ReasonCode reasonCode) {
        return mMessagingLog.setGroupChatDeliveryInfoStatusAndReasonCode(chatId, contact.toString(), msgId,
                status.toInt(), reasonCode.toInt());
    }

    public boolean isDeliveredToAllRecipients(String msgId) {
        return mMessagingLog.isDeliveredToAllRecipients(msgId);
    }

    public boolean isDisplayedByAllRecipients(String msgId) {
        return mMessagingLog.isDisplayedByAllRecipients(msgId);
    }

    public boolean setRejoinId(String rejoinId, boolean updateStateToStarted) {
        return mMessagingLog.setGroupChatRejoinId(mChatId, rejoinId, updateStateToStarted);
    }

    public void addGroupChat(ContactId contact, String subject,
            Map<ContactId, ParticipantStatus> participants, State state, ReasonCode reasonCode,
            Direction direction, long timestamp) {
        mContact = contact;
        mSubject = subject;
        mDirection = direction;
        mTimestamp = timestamp;
     /*   mMessagingLog.addGroupChat(mChatId, contact.toString(), subject, participants, state, reasonCode,
                direction, timestamp); */
    }

    public void addGroupChatEvent(ContactId contact, GroupChatEvent.Status status, long timestamp) {
        mContact = contact;
        mTimestamp = timestamp;
        mMessagingLog.addGroupChatEvent(mChatId, contact, status, timestamp);
    }

    public void addIncomingGroupChatMessage(ChatMessage msg, boolean imdnDisplayedRequested) {
        mMessagingLog.addIncomingGroupChatMessage(mChatId, msg, imdnDisplayedRequested);
    }

    public void addOutgoingGroupChatMessage(ChatMessage msg, Set<ContactId> recipients,
            Status status, Content.ReasonCode reasonCode) {
        mMessagingLog.addOutgoingGroupChatMessage(mChatId, msg, recipients, status, reasonCode);
    }

    public boolean setRejectNextGroupChatNextInvitation() {
        return mMessagingLog.setRejectNextGroupChatNextInvitation(mChatId);
    }

    public boolean setParticipantsStateAndReasonCode(
            Map<ContactId, ParticipantStatus> participants, State state, ReasonCode reasonCode) {
       /* return mMessagingLog.setGroupChatParticipantsStateAndReasonCode(mChatId, participants,
                state, reasonCode); */
        return true;
    }

    public boolean setGroupChatDeliveryInfoDelivered(String chatId, ContactId contact,
            String msgId, long timestampDelivered) {
        return mMessagingLog.setGroupChatDeliveryInfoDelivered(chatId, contact.toString(), msgId,
                timestampDelivered);
    }

    public boolean setDeliveryInfoDisplayed(String chatId, ContactId contact, String msgId,
            long timestampDisplayed) {
        return mMessagingLog.setGroupChatDeliveryInfoDisplayed(chatId, contact.toString(), msgId,
                timestampDisplayed);
    }

    public void addGroupChatFailedDeliveryMessage(ChatMessage msg) {
        mMessagingLog.addGroupChatFailedDeliveryMessage(mChatId, msg);
    }
}
