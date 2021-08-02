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

import com.gsma.services.rcs.upload.FileUpload.State;
import com.gsma.services.rcs.upload.FileUploadInfo;

/**
 * Interface to perform broadcast events on FileUploadListeners
 */
public interface IFileUploadEventBroadcaster {

    void broadcastStateChanged(String uploadId, State state);

    void broadcastProgressUpdate(String uploadId, long currentSize, long totalSize);

    void broadcastUploaded(String uploadId, FileUploadInfo info);
}
