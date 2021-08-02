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

package com.orangelabs.rcs.service.broadcaster;

import com.orangelabs.rcs.utils.logger.Logger;
import com.gsma.services.rcs.upload.FileUpload.State;
import com.gsma.services.rcs.upload.FileUploadInfo;
import com.gsma.services.rcs.upload.IFileUploadListener;

import android.os.RemoteCallbackList;
import android.os.RemoteException;

/**
 * FileUploadEventBroadcaster maintains the registering and unregistering of IFileUploadListener and
 * also performs broadcast events on these listeners upon the trigger of corresponding callbacks.
 */
public class FileUploadEventBroadcaster implements IFileUploadEventBroadcaster {

    private final RemoteCallbackList<IFileUploadListener> mFileUploadListeners = new RemoteCallbackList<>();

    private final Logger logger = Logger.getLogger(getClass().getName());

    public FileUploadEventBroadcaster() {
    }

    public void addEventListener(IFileUploadListener listener) {
        mFileUploadListeners.register(listener);
    }

    public void removeEventListener(IFileUploadListener listener) {
        mFileUploadListeners.unregister(listener);
    }

    @Override
    public void broadcastStateChanged(String uploadId, State state) {
        int rcsState = state.toInt();
        final int N = mFileUploadListeners.beginBroadcast();
        for (int i = 0; i < N; i++) {
            try {
                mFileUploadListeners.getBroadcastItem(i).onStateChanged(uploadId, rcsState);
            } catch (RemoteException e) {
                if (logger.isActivated()) {
                    logger.error("Can't notify listener", e);
                }
            }
        }
        mFileUploadListeners.finishBroadcast();
    }

    @Override
    public void broadcastProgressUpdate(String uploadId, long currentSize, long totalSize) {
        final int N = mFileUploadListeners.beginBroadcast();
        for (int i = 0; i < N; i++) {
            try {
                mFileUploadListeners.getBroadcastItem(i).onProgressUpdate(uploadId, currentSize,
                        totalSize);
            } catch (RemoteException e) {
                if (logger.isActivated()) {
                    logger.error("Can't notify listener", e);
                }
            }
        }
        mFileUploadListeners.finishBroadcast();
    }

    @Override
    public void broadcastUploaded(String uploadId, FileUploadInfo info) {
        final int N = mFileUploadListeners.beginBroadcast();
        for (int i = 0; i < N; i++) {
            try {
                mFileUploadListeners.getBroadcastItem(i).onUploaded(uploadId, info);
            } catch (RemoteException e) {
                if (logger.isActivated()) {
                    logger.error("Can't notify listener", e);
                }
            }
        }
        mFileUploadListeners.finishBroadcast();
    }
}
