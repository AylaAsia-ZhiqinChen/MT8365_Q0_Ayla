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

import java.io.File;

import com.orangelabs.rcs.core.content.MmContent;
import com.orangelabs.rcs.provider.CursorUtil;
import com.orangelabs.rcs.provider.messaging.RichMessagingHistory;
import com.orangelabs.rcs.service.api.ServerApiPersistentStorageException;
import com.orangelabs.rcs.utils.ContactUtil;
import com.orangelabs.rcs.utils.logger.Logger;

import com.gsma.services.rcs.RcsService.Direction;
import com.gsma.services.rcs.RcsService.ReadStatus;
import com.gsma.services.rcs.contact.ContactId;
import com.gsma.services.rcs.ft.FileTransfer.Disposition;
import com.gsma.services.rcs.ft.FileTransfer.ReasonCode;
import com.gsma.services.rcs.ft.FileTransfer.State;

import android.database.Cursor;
import android.net.Uri;

/**
 * FileTransferPersistedStorageAccessor helps in retrieving persisted data related to a file
 * transfer from the persisted storage. It can utilize caching for such data that will not be
 * changed after creation of the File transfer to speed up consecutive access.
 */
public class FileTransferPersistedStorageAccessor {

    private final String mFileTransferId;

    private final RichMessagingHistory mMessagingLog;

    private ContactId mContact;

    private Boolean mRead;

    private Disposition mDisposition;

    private Direction mDirection;

    private String mChatId;

    private String mFileName;

    private Long mFileSize;

    private String mMimeType;

    private Uri mFile;

    private Uri mFileIcon;

    private String mFileIconMimeType;

    private Long mTimestampDelivered;

    private Long mTimestampDisplayed;

    private Long mFileExpiration = FileTransferData.UNKNOWN_EXPIRATION;

    private Long mFileIconExpiration = FileTransferData.UNKNOWN_EXPIRATION;
    
    /**
     * The logger
     */
    private Logger logger = Logger.getLogger(this.getClass().getName());

    /**
     * Constructor
     * 
     * @param fileTransferId the file transfer ID
     * @param messagingLog the messaging log accessor
     */
    public FileTransferPersistedStorageAccessor(String fileTransferId, RichMessagingHistory messagingLog) {
        mFileTransferId = fileTransferId;
        mMessagingLog = messagingLog;
    }

    private void cacheData() {
        Cursor cursor = null;
        try {
            cursor = mMessagingLog.getFileTransferData(mFileTransferId);            
            if (!cursor.moveToNext()) {
                throw new ServerApiPersistentStorageException("Data not found for file transfer "
                        + mFileTransferId);
            }
            if (logger.isActivated()) {
                logger.info("cacheData cursor found");
            }
            String contact = cursor.getString(cursor
                    .getColumnIndexOrThrow(FileTransferData.KEY_CONTACT));
            try {
                if (contact != null) {
                    mContact = ContactUtil.createContactIdFromTrustedData(contact);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
            mDirection = Direction.valueOf(cursor.getInt(cursor
                    .getColumnIndexOrThrow(FileTransferData.KEY_DIRECTION)));
            mDisposition = Disposition.valueOf(cursor.getInt(cursor
                    .getColumnIndexOrThrow(FileTransferData.KEY_DISPOSITION)));
            mChatId = cursor.getString(cursor.getColumnIndexOrThrow(FileTransferData.KEY_CHAT_ID));
            mFileName = cursor.getString(cursor
                    .getColumnIndexOrThrow(FileTransferData.KEY_FILE));
            mMimeType = cursor.getString(cursor
                    .getColumnIndexOrThrow(FileTransferData.KEY_MIME_TYPE));
            /*mFile = Uri.parse(cursor.getString(cursor
                    .getColumnIndexOrThrow(FileTransferData.KEY_FILE)));*/
            mFile = Uri.fromFile(new File(cursor.getString(cursor
                    .getColumnIndexOrThrow(FileTransferData.KEY_FILE))));
            
            if (logger.isActivated()) {
                if(mFile != null){
                    logger.info("cacheData file :" + mFile.getPath());
                } else {
                    logger.info("cacheData file is null");
                }
            }
            String fileIcon = cursor.getString(cursor
                    .getColumnIndexOrThrow(FileTransferData.KEY_FILEICON));
            if (fileIcon != null) {
                mFileIcon = Uri.parse(fileIcon);
            }
            if (!Boolean.TRUE.equals(mRead)) {
                mRead = ReadStatus.READ.toInt() == cursor.getInt(cursor
                        .getColumnIndexOrThrow(FileTransferData.KEY_READ_STATUS));
            }
            mFileSize = cursor.getLong(cursor.getColumnIndexOrThrow(FileTransferData.KEY_FILESIZE));
            mFileIconMimeType = cursor.getString(cursor
                    .getColumnIndexOrThrow(FileTransferData.KEY_FILEICON_MIME_TYPE));
            if (mTimestampDelivered == null || mTimestampDelivered == 0) {
                mTimestampDelivered = cursor.getLong(cursor
                        .getColumnIndexOrThrow(FileTransferData.KEY_TIMESTAMP_DELIVERED));
            }
            if (mTimestampDisplayed == null || mTimestampDisplayed == 0) {
                mTimestampDisplayed = cursor.getLong(cursor
                        .getColumnIndexOrThrow(FileTransferData.KEY_TIMESTAMP_DISPLAYED));
            }
            if (mFileExpiration == FileTransferData.UNKNOWN_EXPIRATION) {
                mFileExpiration = cursor.getLong(cursor
                        .getColumnIndexOrThrow(FileTransferData.KEY_FILE_EXPIRATION));
            }
            if (mFileIconExpiration == FileTransferData.UNKNOWN_EXPIRATION) {
                mFileIconExpiration = cursor.getLong(cursor
                        .getColumnIndexOrThrow(FileTransferData.KEY_FILEICON_EXPIRATION));
            }
        } finally {
            CursorUtil.close(cursor);
        }
    }

    public String getChatId() {
        /*
         * Utilizing cache here as chatId can't be changed in persistent storage after entry
         * insertion anyway so no need to query for it multiple times.
         */
        if (mChatId == null) {
            cacheData();
        }
        return mChatId;
    }

    public ContactId getRemoteContact() {
        /*
         * Utilizing cache here as contact can't be changed in persistent storage after entry
         * insertion anyway so no need to query for it multiple times.
         */
        if (mContact == null) {
            cacheData();
        }
        return mContact;
    }

    public Uri getFile() {
        /*
         * Utilizing cache here as file can't be changed in persistent storage after entry insertion
         * anyway so no need to query for it multiple times.
         */
        if (mFile == null) {
            cacheData();
        }
        return mFile;
    }

    public String getFileName() {
        /*
         * Utilizing cache here as file name can't be changed in persistent storage after entry
         * insertion anyway so no need to query for it multiple times.
         */
        if (mFileName == null) {
            cacheData();
        }
        return mFileName;
    }

    public Long getFileSize() {
        /*
         * Utilizing cache here as file size can't be changed in persistent storage after entry
         * insertion anyway so no need to query for it multiple times.
         */
        if (mFileSize == null) {
            cacheData();
        }
        return mFileSize;
    }

    public String getMimeType() {
        /*
         * Utilizing cache here as mime type can't be changed in persistent storage after entry
         * insertion anyway so no need to query for it multiple times.
         */
        if (mMimeType == null) {
            cacheData();
        }
        return mMimeType;
    }

    public Uri getFileIcon() {
        /*
         * Utilizing cache here as file icon can't be changed in persistent storage after entry
         * insertion anyway so no need to query for it multiple times.
         */
        if (mFileIcon == null) {
            cacheData();
        }
        return mFileIcon;
    }

    public String getFileIconMimeType() {
        /*
         * Utilizing cache here as file icon mime type can't be changed in persistent storage after
         * entry insertion anyway so no need to query for it multiple times.
         */
        if (mFileIconMimeType == null) {
            cacheData();
        }
        return mFileIconMimeType;
    }

    public long getTimestamp() {
        Long timestamp = mMessagingLog.getFileTransferTimestamp(mFileTransferId);
        if (timestamp == null) {
            throw new ServerApiPersistentStorageException("Timestamp not found for file transfer "
                    + mFileTransferId);
        }
        return timestamp;
    }

    public long getTimestampSent() {
        Long timestamp = mMessagingLog.getFileTransferSentTimestamp(mFileTransferId);
        if (timestamp == null) {
            throw new ServerApiPersistentStorageException(
                    "TimestampSent not found for file transfer " + mFileTransferId);
        }
        return timestamp;
    }

    public long getTimestampDelivered() {
        /*
         * Utilizing cache here as Timestamp delivered can't be changed in persistent storage after
         * it has been set to some value bigger than zero, so no need to query for it multiple
         * times.
         */
        if (mTimestampDelivered == null || mTimestampDelivered == 0) {
            cacheData();
        }
        return mTimestampDelivered;
    }

    public Long getTimestampDisplayed() {
        /*
         * Utilizing cache here as Timestamp displayed can't be changed in persistent storage after
         * it has been set to some value bigger than zero, so no need to query for it multiple
         * times.
         */
        if (mTimestampDisplayed == null || mTimestampDisplayed == 0) {
            cacheData();
        }
        return mTimestampDisplayed;
    }

    public State getState() {
        State state = mMessagingLog.getFileTransferState(mFileTransferId);
        if (state == null) {
            throw new ServerApiPersistentStorageException("State not found for file transfer "
                    + mFileTransferId);
        }
        return state;
    }

    public ReasonCode getReasonCode() {
        ReasonCode reasonCode = mMessagingLog.getFileTransferReasonCode(mFileTransferId);
        if (reasonCode == null) {
            throw new ServerApiPersistentStorageException(
                    "Reason code not found for file transfer " + mFileTransferId);
        }
        return reasonCode;
    }

    public Disposition getDisposition() {
        /*
         * Utilizing cache here as disposition can't be changed in persistent storage after entry
         * insertion anyway so no need to query for it multiple times.
         */
        if (mDisposition == null) {
            cacheData();
        }
        return mDisposition;
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

    public Boolean isRead() {
        /*
         * No need to read from provider unless incoming and not already marked as read.
         */
        if (Direction.INCOMING == getDirection() && !Boolean.TRUE.equals(mRead)) {
            cacheData();
        }
        return mRead;
    }

    public boolean setStateAndReasonCode(State state, ReasonCode reasonCode) {
        return mMessagingLog.setFileTransferStateAndReasonCode(mFileTransferId, state, reasonCode);
    }

    public boolean setProgress(long currentSize) {
        return mMessagingLog.setFileTransferProgress(mFileTransferId, currentSize);
    }

    public boolean setTransferred(MmContent content, long fileExpiration, long fileIconExpiration,
            long deliveryExpiration) {
        return mMessagingLog.setFileTransferred(mFileTransferId, content, fileExpiration,
                fileIconExpiration, deliveryExpiration);
    }

    public void addOneToOneFileTransfer(ContactId contact, Direction direction, MmContent content,
            MmContent fileIcon, State status, ReasonCode reasonCode, long timestamp,
            long timestampSent, long fileExpiration, long fileIconExpiration) {
        mContact = contact;
        mDirection = direction;
        mMessagingLog.addFileTransfer(mFileTransferId, contact.toString(), direction.toInt(), content,
                fileIcon, status.toInt(), reasonCode.toInt(), timestamp, timestampSent, fileExpiration,
                fileIconExpiration);
    }

    public void addIncomingGroupFileTransfer(String chatId, ContactId contact, MmContent content,
            MmContent fileicon, State state, ReasonCode reasonCode, long timestamp,
            long timestampSent, long fileExpiration, long fileIconExpiration) {
        mChatId = chatId;
        mContact = contact;
        mMessagingLog.addIncomingGroupFileTransfer(mFileTransferId, chatId, contact.toString(), content,
                fileicon, state.toInt(), reasonCode.toInt(), timestamp, timestampSent, fileExpiration,
                fileIconExpiration);
    }

    //public FtHttpResume getFileTransferResumeInfo() {
        //return mMessagingLog.getFileTransferResumeInfo(mFileTransferId);
     //   return null;
    //}

    /**
     * Returns the time for when file on the content server is no longer valid to download.
     * 
     * @return time
     */
    public long getFileExpiration() {
        /* No need to read from provider unless outgoing and expiration is unknown. */
        if (Direction.OUTGOING == getDirection()
                && FileTransferData.UNKNOWN_EXPIRATION == mFileExpiration) {
            cacheData();
        }
        return mFileExpiration;
    }

    /**
     * Returns the time for when file icon on the content server is no longer valid to download.
     * 
     * @return time
     */
    public long getFileIconExpiration() {
        /* No need to read from provider unless outgoing and expiration is unknown. */
        if (Direction.OUTGOING == getDirection()
                && FileTransferData.UNKNOWN_EXPIRATION == mFileIconExpiration) {
            cacheData();
        }
        return mFileIconExpiration;
    }

    /**
     * Returns true if delivery for this file has expired or false otherwise. Note: false means
     * either that delivery for this file has not yet expired, delivery has been successful,
     * delivery expiration has been cleared (see clearFileTransferDeliveryExpiration) or that this
     * particular file is not eligible for delivery expiration in the first place.
     * 
     * @return deliveryExpiration
     */
    public boolean isExpiredDelivery() {
        Boolean expiredDelivery = mMessagingLog.isFileTransferExpiredDelivery(mFileTransferId);
        if (expiredDelivery == null) {
            throw new ServerApiPersistentStorageException(
                    "Expired Delivery not found for file transfer " + mFileTransferId);
        }
        return expiredDelivery;
    }

    public boolean setStateAndTimestamp(State state, ReasonCode reasonCode, long timestamp,
            long timestampSent) {
        return mMessagingLog.setFileTransferStateAndTimestamp(mFileTransferId, state, reasonCode,
                timestamp, timestampSent);
    }

    //public boolean setFileInfoDequeued(long deliveryExpiration) {
       // return mMessagingLog.setFileInfoDequeued(mFileTransferId, deliveryExpiration);
    //}

    /**
     * Returns the number of transferred bytes.
     * 
     * @return the number of transferred bytes.
     */
    public long getFileTransferProgress() {
        Long transferred = mMessagingLog.getFileTransferProgress(mFileTransferId);
        if (transferred == null) {
            throw new ServerApiPersistentStorageException(
                    "Transferred not found for file transfer " + mFileTransferId);
        }
        return transferred;
    }
}
