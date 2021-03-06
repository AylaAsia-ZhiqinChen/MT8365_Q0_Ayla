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

package com.gsma.services.rcs.sharing.image;

/**
 * Intent for image sharing invitations
 * 
 * @author Jean-Marc AUFFRET
 */
public class ImageSharingIntent {
    /**
     * Broadcast action: a new image sharing invitation has been received.
     * <p>
     * Intent includes the following extras:
     * <ul>
     * <li> {@link #EXTRA_SHARING_ID} containing the unique ID of the image sharing.
     * </ul>
     */
    public final static String ACTION_NEW_INVITATION = "com.gsma.services.rcs.sharing.image.action.NEW_IMAGE_SHARING";

    /**
     * Unique ID of the image sharing
     */
    public final static String EXTRA_SHARING_ID = "sharingId";

    /**
     * MSISDN of the contact sending the invitation
     */
    public final static String EXTRA_CONTACT = "contact";

    /**
     * Display name of the contact sending the invitation
     */
    public final static String EXTRA_DISPLAY_NAME = "contactDisplayname";

    /**
     * Name of the file
     */
    public final static String EXTRA_FILENAME = "filename";

    /**
     * Size of the file in byte
     */
    public final static String EXTRA_FILESIZE = "filesize";

    /**
     * MIME type of the file
     */
    public final static String EXTRA_FILETYPE = "filetype";

    private ImageSharingIntent() {
    }
}
