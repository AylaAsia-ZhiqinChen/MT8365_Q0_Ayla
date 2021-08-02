/*
 * Copyright (C) 2015 Sony Mobile Communications Inc.
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

import com.orangelabs.rcs.provider.CursorUtil;
import com.orangelabs.rcs.service.api.GeolocSharingServiceImpl;
import com.orangelabs.rcs.utils.ContactUtil;
import com.orangelabs.rcs.utils.logger.Logger;
import com.gsma.services.rcs.contact.ContactId;
import com.gsma.services.rcs.sharing.geoloc.GeolocSharing.ReasonCode;
import com.gsma.services.rcs.sharing.geoloc.GeolocSharing.State;

import android.database.Cursor;

/* Update the state of ungracefully terminated Geoloc sharing service object from
 * STARTED, INVITED and INITIATING to FAILED after rebooted. The reason of
 * ungracefully terminated could be sudden stack termination or battery drained,
 * which leads to wrong state in the database.
 * 
 * This class is used for error correction purpose, if no error was detected in
 * the database, means graceful termination is executed, so no update process will
 * be done.*/

public class UpdateGeolocSharingStateAfterUngracefulTerminationTask implements Runnable {

    private final RichCallHistory mRichCallHistory;

    private final GeolocSharingServiceImpl mGeolocService;

    private static final Logger sLogger = Logger
            .getLogger(UpdateGeolocSharingStateAfterUngracefulTerminationTask.class.getName());

    public UpdateGeolocSharingStateAfterUngracefulTerminationTask(RichCallHistory rcHistory,
            GeolocSharingServiceImpl geolocService) {
        mRichCallHistory = rcHistory;
        mGeolocService = geolocService;
    }

    @Override
    public void run() {
        if (sLogger.isActivated()) {
            sLogger.debug("initiating.");
        }
        Cursor cursor = null;
        try {
            cursor = mRichCallHistory.getInterruptedGeolocSharings();
            int sharingIdx = cursor.getColumnIndexOrThrow(GeolocSharingData.KEY_SHARING_ID);
            int contactIdx = cursor.getColumnIndexOrThrow(GeolocSharingData.KEY_CONTACT);
            int stateIdx = cursor.getColumnIndexOrThrow(GeolocSharingData.KEY_STATE);
            while (cursor.moveToNext()) {
                String sharingId = cursor.getString(sharingIdx);
                String contactNumber = cursor.getString(contactIdx);
                ContactId contact = ContactUtil.createContactIdFromTrustedData(contactNumber);
                State state = State.valueOf(cursor.getInt(stateIdx));
                switch (state) {
                    case STARTED:
                        mGeolocService.setGeolocSharingStateAndReasonCode(contact, sharingId,
                                State.FAILED, ReasonCode.FAILED_SHARING);
                        break;
                    case ACCEPTING:
                        mGeolocService.setGeolocSharingStateAndReasonCode(contact, sharingId,
                                State.ABORTED, ReasonCode.ABORTED_BY_SYSTEM);
                        break;
                    case INITIATING:
                        mGeolocService.setGeolocSharingStateAndReasonCode(contact, sharingId,
                                State.FAILED, ReasonCode.FAILED_INITIATION);
                        break;
                    case INVITED:
                        mGeolocService.setGeolocSharingStateAndReasonCode(contact, sharingId,
                                State.REJECTED, ReasonCode.REJECTED_BY_SYSTEM);
                        break;
                    default:
                        sLogger.error(getClass().getName() + " unexpected state (" + state
                                + ") detected! Error in SQL statement?");
                        break;
                }
            }

        } catch (RuntimeException e) {
            /*
             * Normally we are not allowed to catch runtime exceptions as these are genuine bugs
             * which should be handled/fixed within the code. However the cases when we are
             * executing operations on a thread unhandling such exceptions will eventually lead to
             * exit the system and thus can bring the whole system down, which is not intended.
             */
            sLogger.error(
                    "Exception occured while trying to update geoloc sharing state for interrupted geoloc sharing",
                    e);
        } finally {
            CursorUtil.close(cursor);
            if (sLogger.isActivated()) {
                sLogger.debug("done.");
            }
        }
    }
}
