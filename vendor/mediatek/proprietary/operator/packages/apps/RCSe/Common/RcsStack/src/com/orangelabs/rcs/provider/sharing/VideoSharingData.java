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

package com.orangelabs.rcs.provider.sharing;

import com.gsma.services.rcs.RcsService;
import com.gsma.services.rcs.sharing.video.VideoSharing;
import com.gsma.services.rcs.sharing.video.VideoSharingLog;

import android.net.Uri;

/**
 * Video sharing data constants
 * 
 * @author Jean-Marc AUFFRET
 */
public class VideoSharingData {

    /**
     * Content provider URI
     */
    /* package private */public static final Uri CONTENT_URI = Uri
            .parse("content://com.orangelabs.rcs.videoshare/videoshare");

    /**
     * History log member id
     */
    public static final int HISTORYLOG_MEMBER_ID = VideoSharingLog.HISTORYLOG_MEMBER_ID;

    /**
     * Unique history log id
     */
    /* package private */static final String KEY_BASECOLUMN_ID = VideoSharingLog.BASECOLUMN_ID;

    /**
     * Unique sharing identifier
     */
    /* package private */static final String KEY_SHARING_ID = VideoSharingLog.SHARING_ID;

    /**
     * ContactId formatted number of the remote contact
     */
    /* package private */static final String KEY_CONTACT = VideoSharingLog.CONTACT;

    /**
     * Date of the sharing
     */
    /* package private */static final String KEY_TIMESTAMP = VideoSharingLog.TIMESTAMP;

    /**
     * @see VideoSharing.State for the list of states
     */
    /* package private */static final String KEY_STATE = VideoSharingLog.STATE;

    /**
     * Reason code associated with the video sharing state.
     * 
     * @see VideoSharing.ReasonCode
     */
    /* package private */static final String KEY_REASON_CODE = VideoSharingLog.REASON_CODE;

    /**
     * Incoming sharing or outgoing sharing.
     * 
     * @see RcsService.Direction
     */
    /* package private */static final String KEY_DIRECTION = VideoSharingLog.DIRECTION;

    /**
     * Duration of the sharing in milliseconds. The value is only set at the end of the sharing.
     */
    /* package private */static final String KEY_DURATION = VideoSharingLog.DURATION;

    /**
     * Encoding of the shared video
     */
    /* package private */static final String KEY_VIDEO_ENCODING = VideoSharingLog.VIDEO_ENCODING;

    /**
     * Width of the shared video
     */
    /* package private */static final String KEY_WIDTH = VideoSharingLog.WIDTH;

    /**
     * Height of the shared video
     */
    /* package private */static final String KEY_HEIGHT = VideoSharingLog.HEIGHT;

}
