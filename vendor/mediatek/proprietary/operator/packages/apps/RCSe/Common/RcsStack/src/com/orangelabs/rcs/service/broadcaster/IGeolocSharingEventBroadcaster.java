/*
 * Copyright (C) 2014 Sony Mobile Communications Inc.
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

package com.orangelabs.rcs.service.broadcaster;

import com.gsma.services.rcs.contact.ContactId;
import com.gsma.services.rcs.sharing.geoloc.GeolocSharing.ReasonCode;
import com.gsma.services.rcs.sharing.geoloc.GeolocSharing.State;

import java.util.Set;

/**
 * Interface to perform broadcast events on GeolocSharingListeners
 */
public interface IGeolocSharingEventBroadcaster {

    void broadcastStateChanged(ContactId contact, String sharingId, State state,
            ReasonCode reasonCode);

    void broadcastProgressUpdate(ContactId contact, String sharingId, long currentSize,
            long totalSize);

    void broadcastInvitation(String sharingId);

    void broadcastDeleted(ContactId contact, Set<String> sharingIds);
}
