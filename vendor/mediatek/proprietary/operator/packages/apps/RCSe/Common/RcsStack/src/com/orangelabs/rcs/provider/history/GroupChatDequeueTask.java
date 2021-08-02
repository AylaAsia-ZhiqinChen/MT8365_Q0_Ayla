/*
 * Copyright (C) 2015 Sony Mobile Communications Inc.
 * Copyright (C) 2010-2016 Orange.
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

package com.orangelabs.rcs.provider.history;

import com.orangelabs.rcs.core.Core;
import com.orangelabs.rcs.core.FileAccessException;
import com.orangelabs.rcs.core.content.MmContent;
import com.orangelabs.rcs.core.ims.network.NetworkException;
import com.orangelabs.rcs.core.ims.protocol.PayloadException;
import com.orangelabs.rcs.core.ims.service.SessionNotEstablishedException;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatMessage;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatUtils;
import com.orangelabs.rcs.core.ims.service.im.chat.imdn.ImdnManager;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.FileTransferUtils;
import com.orangelabs.rcs.provider.eab.ContactsManager;
import com.orangelabs.rcs.provider.messaging.FileTransferData;
import com.orangelabs.rcs.provider.messaging.MessageData;
import com.orangelabs.rcs.provider.messaging.RichMessagingHistory;
import com.orangelabs.rcs.provider.settings.RcsSettings;
import com.orangelabs.rcs.service.DequeueTask;
import com.orangelabs.rcs.service.api.ChatServiceImpl;
import com.orangelabs.rcs.service.api.FileTransferServiceImpl;
import com.orangelabs.rcs.service.api.GroupChatImpl;
//import com.orangelabs.rcs.service.api.GroupFileTransferImpl;
import com.gsma.services.rcs.ft.FileTransfer.Disposition;
import com.gsma.services.rcs.ft.FileTransfer.State;

import android.content.Context;
import android.database.Cursor;
import android.net.Uri;

/**
 * GroupChatDequeueTask tries to dequeue all group chat messages that are QUEUED and all file
 * transfers that are either QUEUED or UPLOADED but not transferred for a specific group chat.
 */
public class GroupChatDequeueTask extends DequeueTask {

    private final String mChatId;

    private final HistoryLog mHistoryLog;

    public GroupChatDequeueTask(Context ctx, Core core, RichMessagingHistory messagingLog,
            ChatServiceImpl chatService, FileTransferServiceImpl fileTransferService,
            RcsSettings rcsSettings, ContactsManager contactManager, HistoryLog historyLog,
            String chatId) {
        super(ctx, core, contactManager, messagingLog, rcsSettings, chatService,
                fileTransferService);
        mChatId = chatId;
        mHistoryLog = historyLog;
    }

    private void setGroupChatEntryAsFailedDequeue(int providerId, String chatId, String id,
            String mimeType) {
        switch (providerId) {
            case MessageData.HISTORYLOG_MEMBER_ID:
                setGroupChatMessageAsFailedDequeue(chatId, id, mimeType);
                break;
            case FileTransferData.HISTORYLOG_MEMBER_ID:
                setGroupFileTransferAsFailedDequeue(chatId, id);
                break;
            default:
                throw new IllegalArgumentException("Provider id " + providerId
                        + " not supported in this context!");
        }
    }

    @Override
    public void run() {
        boolean logActivated = mLogger.isActivated();
        if (logActivated) {
            mLogger.debug("Execute task to dequeue group chat messages and group file transfers for chatId "
                    .concat(mChatId));
        }
        ImdnManager imdnManager = mImService.getImdnManager();
        boolean displayedReportEnabled = false/*imdnManager
                .isRequestGroupDeliveryDisplayedReportsEnabled();*/;
        boolean deliveryReportEnabled = false/*imdnManager.isDeliveryDeliveredReportsEnabled()*/;
        int providerId = -1;
        String id = null;
        Disposition disposition;
        String mimeType = null;
        Cursor cursor = null;
        try {
            if (!isImsConnected()) {
                if (logActivated) {
                    mLogger.debug("IMS not connected, exiting dequeue task to dequeue group chat messages and group file transfers for chatId "
                            .concat(mChatId));
                }
                return;
            }
            if (isShuttingDownOrStopped()) {
                if (logActivated) {
                    mLogger.debug("Core service is shutting down/stopped, exiting dequeue task to dequeue group chat messages and group file transfers for chatId "
                            .concat(mChatId));
                }
                return;
            }
            cursor = mHistoryLog.getQueuedGroupChatMessagesAndGroupFileTransfers(mChatId);
            int providerIdIdx = cursor.getColumnIndexOrThrow(HistoryLogData.KEY_PROVIDER_ID);
            int idIdx = cursor.getColumnIndexOrThrow(HistoryLogData.KEY_ID);
            int contentIdx = cursor.getColumnIndexOrThrow(HistoryLogData.KEY_CONTENT);
            int mimeTypeIdx = cursor.getColumnIndexOrThrow(HistoryLogData.KEY_MIME_TYPE);
            int fileIconIdx = cursor.getColumnIndexOrThrow(HistoryLogData.KEY_FILEICON);
            int statusIdx = cursor.getColumnIndexOrThrow(HistoryLogData.KEY_STATUS);
            int fileSizeIdx = cursor.getColumnIndexOrThrow(HistoryLogData.KEY_FILESIZE);
            int dispositionIdx = cursor.getColumnIndexOrThrow(HistoryLogData.KEY_DISPOSITION);
            GroupChatImpl groupChat = null/*mChatService.getOrCreateGroupChat(mChatId)*/;
            while (cursor.moveToNext()) {
                try {
                    if (!isImsConnected()) {
                        if (logActivated) {
                            mLogger.debug("IMS not connected, exiting dequeue task to dequeue group chat messages and group file transfers for chatId "
                                    .concat(mChatId));
                        }
                        return;
                    }
                    if (isShuttingDownOrStopped()) {
                        if (logActivated) {
                            mLogger.debug("Core service is shutting down/stopped, exiting dequeue task to dequeue group chat messages and group file transfers for chatId "
                                    .concat(mChatId));
                        }
                        return;
                    }
                    providerId = cursor.getInt(providerIdIdx);
                    id = cursor.getString(idIdx);
                    mimeType = cursor.getString(mimeTypeIdx);
                    disposition = Disposition.valueOf(cursor.getInt(dispositionIdx));
                    switch (providerId) {
                        case MessageData.HISTORYLOG_MEMBER_ID:
                            if (!isPossibleToDequeueGroupChatMessagesAndGroupFileTransfers(mChatId)) {
                                setGroupChatMessageAsFailedDequeue(mChatId, id, mimeType);
                                continue;
                            }
                            long timestamp = System.currentTimeMillis();
                            String content = cursor.getString(contentIdx);
                            /* For outgoing message, timestampSent = timestamp */
                     //       ChatMessage message = ChatUtils.createChatMessage(id, mimeType,
                     //               content, null, null, timestamp, timestamp);
                     //       groupChat.dequeueGroupChatMessage(message);
                            break;

                        case FileTransferData.HISTORYLOG_MEMBER_ID:
                            Uri file = Uri.parse(cursor.getString(contentIdx));
                            if (!isPossibleToDequeueGroupFileTransfer(mChatId, file,
                                    cursor.getLong(fileSizeIdx))) {
                                setGroupFileTransferAsFailedDequeue(mChatId, id);
                                continue;
                            }
                            int state = cursor.getInt(statusIdx);
                            if (logActivated) {
                                mLogger.debug("Dequeue chatId=" + mChatId + " in state=" + state
                                        + " file=" + file);
                            }
                           // GroupFileTransferImpl groupFileTransfer;
                            switch (state) {
                                case 9:
                                    if (!isAllowedToDequeueGroupFileTransfer()) {
                                        continue;
                                    }
                                    MmContent fileContent = FileTransferUtils.createMmContent(file,
                                            mimeType, disposition);
                                    MmContent fileIconContent = null;
                                    String fileIcon = cursor.getString(fileIconIdx);
                                    if (fileIcon != null) {
                                        Uri fileIconUri = Uri.parse(fileIcon);
                                        fileIconContent = FileTransferUtils
                                                .createIconContent(fileIconUri);
                                    }
                                  //  mFileTransferService.dequeueGroupFileTransfer(mChatId, id,
                                  //          fileContent, fileIconContent);
                                    break;

                                case 3:
                                    if (!isPossibleToDequeueGroupChatMessagesAndGroupFileTransfers(mChatId)) {
                                        setGroupFileTransferAsFailedDequeue(mChatId, id);
                                        continue;
                                    }
                                   /* groupFileTransfer = mFileTransferService
                                            .getOrCreateGroupFileTransfer(mChatId, id);
                                    String fileInfo = FileTransferUtils
                                            .createHttpFileTransferXml(mMessagingLog
                                                    .getFileDownloadInfo(id));
                                    groupChat.dequeueGroupFileInfo(id, fileInfo,
                                            displayedReportEnabled, deliveryReportEnabled,
                                            groupFileTransfer);*/
                                    break;

                                default:
                                    break;
                            }
                            break;
                        default:
                            break;
                    }

                } /*catch (SessionNotEstablishedException | FileAccessException | NetworkException e) {
                    if (logActivated) {
                        mLogger.debug("Failed to dequeue group chat entry with id '" + id
                                + "' on group chat '" + mChatId + "' due to: " + e.getMessage());
                    }

                } catch (PayloadException e) {
                    mLogger.error("Failed to dequeue group chat entry with id '" + id
                            + "' on group chat '" + mChatId, e);
                    setGroupChatEntryAsFailedDequeue(providerId, mChatId, id, mimeType);

                } */catch (RuntimeException e) {
                    /*
                     * Normally all the terminal and non-terminal cases should be handled above so
                     * if we come here that means that there is a bug and so we output a stack trace
                     * so the bug can then be properly tracked down and fixed. We also mark the
                     * respective entry that failed to dequeue as FAILED.
                     */
                    mLogger.error("Failed to dequeue group chat entry with id '" + id
                            + "' and chatId '" + mChatId + "'", e);
                    setGroupChatEntryAsFailedDequeue(providerId, mChatId, id, mimeType);
                }
            }

        } catch (RuntimeException e) {
            /*
             * Normally all the terminal and non-terminal cases should be handled above so if we
             * come here that means that there is a bug and so we output a stack trace so the bug
             * can then be properly tracked down and fixed. We also mark the respective entry that
             * failed to dequeue as FAILED.
             */
            mLogger.error(
                    "Exception occurred while dequeueing group chat message and group file transfer with id '"
                            + id + "' and chatId '" + mChatId + "'", e);
            if (id == null) {
                return;
            }
            setGroupChatEntryAsFailedDequeue(providerId, mChatId, id, mimeType);

        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
    }
}
