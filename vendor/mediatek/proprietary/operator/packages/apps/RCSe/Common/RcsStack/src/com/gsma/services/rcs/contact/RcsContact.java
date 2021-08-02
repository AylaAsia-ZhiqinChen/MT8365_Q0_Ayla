/*******************************************************************************
 * Software Name : RCS IMS Stack
 *
 * Copyright (C) 2010-2016 Orange.
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
 ******************************************************************************/

package com.gsma.services.rcs.contact;

import com.gsma.services.rcs.capability.Capabilities;

import android.os.Parcel;
import android.os.Parcelable;

/**
 * RCS contact
 *
 */
public class RcsContact implements Parcelable {
    /**
     * Capabilities
     */
    private Capabilities mCapabilities;

    /**
     * Contact ID
     */
    private ContactId mContact;

    /**
     * Display name
     */
    private String mDisplayName;

    /**
     * Online state
     */
    private boolean mOnline;

    /**
     * Blocking state
     */
    private boolean mBlocked;

    /**
     * Blocking timestamp
     */
    private long mBlockingTimestamp;

    /**
     * Constructor
     * 
     * @param contact Contact ID
     * @param online Online state
     * @param capabilities Capabilities
     * @param displayName Display name
     * @param blocked Blocking state
     * @param blockingTs Blocking timestamp
     * @hide
     */
    public RcsContact(ContactId contact, boolean online, Capabilities capabilities,
            String displayName, boolean blocked, long blockingTs) {
        mContact = contact;
        mOnline = online;
        mCapabilities = capabilities;
        mDisplayName = displayName;
        mBlocked = blocked;
        mBlockingTimestamp = blockingTs;
    }

    /**
     * Constructor
     * 
     * @param source Parcelable source
     * @hide
     */
    public RcsContact(Parcel source) {
        boolean containsContact = source.readInt() != 0;
        if (containsContact) {
            mContact = ContactId.CREATOR.createFromParcel(source);
        } else {
            mContact = null;
        }
        mOnline = source.readInt() != 0;
        boolean containsCapabilities = source.readInt() != 0;
        if (containsCapabilities) {
            mCapabilities = Capabilities.CREATOR.createFromParcel(source);
        } else {
            mCapabilities = null;
        }
        mDisplayName = source.readString();
        mBlocked = source.readInt() != 0;
        mBlockingTimestamp = source.readLong();
    }

    /**
     * Describe the kinds of special objects contained in this Parcelable's marshalled
     * representation
     * 
     * @return Integer
     * @hide
     */
    public int describeContents() {
        return 0;
    }

    /**
     * Write parcelable object
     * 
     * @param dest The Parcel in which the object should be written
     * @param flags Additional flags about how the object should be written
     * @hide
     */
    public void writeToParcel(Parcel dest, int flags) {
        if (mContact != null) {
            dest.writeInt(1);
            mContact.writeToParcel(dest, flags);
        } else {
            dest.writeInt(0);
        }
        dest.writeInt(mOnline ? 1 : 0);
        if (mCapabilities != null) {
            dest.writeInt(1);
            mCapabilities.writeToParcel(dest, flags);
        } else {
            dest.writeInt(0);
        }
        dest.writeString(mDisplayName);
        dest.writeInt(mBlocked ? 1 : 0);
        dest.writeLong(mBlockingTimestamp);
    }

    /**
     * Parcelable creator
     * 
     * @hide
     */
    public static final Parcelable.Creator<RcsContact> CREATOR = new Parcelable.Creator<RcsContact>() {
        public RcsContact createFromParcel(Parcel source) {
            return new RcsContact(source);
        }

        public RcsContact[] newArray(int size) {
            return new RcsContact[size];
        }
    };

    /**
     * Returns the canonical contact ID (i.e. MSISDN)
     * 
     * @return Contact ID
     */
    public ContactId getContactId() {
        return mContact;
    }

    /**
     * Is contact online (i.e. registered to the service platform)
     * 
     * @return Returns true if online else returns false
     */
    public boolean isOnline() {
        return mOnline;
    }

    /**
     * Returns the capabilities associated to the contact
     * 
     * @return Capabilities
     */
    public Capabilities getCapabilities() {
        return mCapabilities;
    }

    /**
     * Returns the display name associated to the contact
     * 
     * @return displayName
     */
    public String getDisplayName() {
        return mDisplayName;
    }

    /**
     * Is contact blocked
     * 
     * @return Returns true if blocked else returns false
     */
    public boolean isBlocked() {
        return mBlocked;
    }

    /**
     * Returns the timestamp when the blocking was activated or -1 if contact is not blocked.
     * 
     * @return Timestamp
     */
    public long getBlockingTimestamp() {
        return mBlockingTimestamp;
    }

    /**
     @hide
     */
    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((mContact == null) ? 0 : mContact.hashCode());
        return result;
    }

    /**
        @hide
     */
    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        RcsContact other = (RcsContact) obj;
        if (mContact == null) {
            if (other.mContact != null)
                return false;
        } else if (!mContact.equals(other.mContact))
            return false;
        return true;
    }

}
