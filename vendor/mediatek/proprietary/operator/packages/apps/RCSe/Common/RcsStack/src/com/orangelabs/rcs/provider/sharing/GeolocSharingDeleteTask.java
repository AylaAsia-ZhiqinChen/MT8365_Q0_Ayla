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

package com.orangelabs.rcs.provider.sharing;

import com.orangelabs.rcs.core.ims.network.NetworkException;
import com.orangelabs.rcs.core.ims.protocol.PayloadException;
import com.orangelabs.rcs.core.ims.service.richcall.RichcallService;
import com.orangelabs.rcs.core.ims.service.richcall.geoloc.GeolocTransferSession;
import com.orangelabs.rcs.provider.DeleteTask;
import com.orangelabs.rcs.provider.LocalContentResolver;
import com.orangelabs.rcs.service.api.GeolocSharingServiceImpl;
import com.orangelabs.rcs.utils.logger.Logger;
import com.gsma.services.rcs.contact.ContactId;

import java.util.Set;

public class GeolocSharingDeleteTask extends DeleteTask.GroupedByContactId {

    private static final Logger sLogger = Logger.getLogger(GeolocSharingDeleteTask.class.getName());

    private final GeolocSharingServiceImpl mGeolocSharingService;

    private final RichcallService mRichcallService;

    /**
     * Deletion of all Geoloc sharing.
     * 
     * @param geolocSharingService the geoloc service impl
     * @param richcallService the rich call service
     * @param contentResolver the local content resolver
     */
    public GeolocSharingDeleteTask(GeolocSharingServiceImpl geolocSharingService,
            RichcallService richcallService, LocalContentResolver contentResolver) {
        super(contentResolver, GeolocSharingData.CONTENT_URI, GeolocSharingData.KEY_SHARING_ID,
                GeolocSharingData.KEY_CONTACT, (String) null);
        mGeolocSharingService = geolocSharingService;
        mRichcallService = richcallService;
    }

    /**
     * Deletion of all geoloc sharing with a specific contact.
     * 
     * @param geolocSharingService the geoloc service impl
     * @param richcallService the rich call service
     * @param contentResolver the local content resolver
     * @param contact the contact id
     */
    public GeolocSharingDeleteTask(GeolocSharingServiceImpl geolocSharingService,
            RichcallService richcallService, LocalContentResolver contentResolver, ContactId contact) {
        super(contentResolver, GeolocSharingData.CONTENT_URI, GeolocSharingData.KEY_SHARING_ID,
                GeolocSharingData.KEY_CONTACT, contact);
        mGeolocSharingService = geolocSharingService;
        mRichcallService = richcallService;
    }

    /**
     * Deletion of a specific geoloc sharing.
     * 
     * @param geolocSharingService the geoloc service impl
     * @param richcallService the rich call service
     * @param contentResolver the local content resolver
     * @param transferId the transfer id
     */
    public GeolocSharingDeleteTask(GeolocSharingServiceImpl geolocSharingService,
            RichcallService richcallService, LocalContentResolver contentResolver, String transferId) {
        super(contentResolver, GeolocSharingData.CONTENT_URI, GeolocSharingData.KEY_SHARING_ID,
                GeolocSharingData.KEY_CONTACT, null, transferId);
        mGeolocSharingService = geolocSharingService;
        mRichcallService = richcallService;
    }

    @Override
    protected void onRowDelete(ContactId contact, String sharingId) throws PayloadException {
        GeolocTransferSession session = mRichcallService.getGeolocTransferSession(sharingId);
        if (session == null) {
            mGeolocSharingService.removeGeolocSharing(sharingId);
            return;
        }
        try {
            session.removeSession();
        } catch (Exception e) {
            /*
             * If network is lost during a delete operation the remaining part of the delete
             * operation (delete from persistent storage) can succeed to 100% anyway since delete
             * can be executed anyway while no network connectivity is present and still succeed.
             */
            if (sLogger.isActivated()) {
                sLogger.debug(e.getMessage());
            }
        }
        mGeolocSharingService.removeGeolocSharing(sharingId);
    }

    @Override
    protected void onCompleted(ContactId contact, Set<String> deletedIds) {
        mGeolocSharingService.broadcastDeleted(contact, deletedIds);
    }

}
