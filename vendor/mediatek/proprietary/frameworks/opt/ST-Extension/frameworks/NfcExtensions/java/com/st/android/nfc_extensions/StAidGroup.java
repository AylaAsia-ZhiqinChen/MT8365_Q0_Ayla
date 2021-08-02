/*
 * Copyright (C) 2015 NXP Semiconductors
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
 */
/**
 * ****************************************************************************
 *
 * <p>The original Work has been changed by ST Microelectronics S.A.
 *
 * <p>Copyright (C) 2017 ST Microelectronics S.A.
 *
 * <p>Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of the License at
 *
 * <p>http://www.apache.org/licenses/LICENSE-2.0
 *
 * <p>Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * <p>****************************************************************************
 */
package com.st.android.nfc_extensions;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlSerializer;

import android.nfc.cardemulation.AidGroup;
import android.nfc.cardemulation.CardEmulation;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;
/// M: Logs @{
import android.os.Build;
/// @}

/** @hide */
public final class StAidGroup implements Parcelable {

    static final String TAG = "APINfc_StAidGroup";
    /// M: Logs @{
    static final boolean DBG = !("user".equals(Build.TYPE) || "userdebug".equals(Build.TYPE));
    /// @}

    final List<String> aids;
    final String category;
    final String description;

    public StAidGroup(List<String> aids, String category, String description) {
        if (DBG)
            Log.d(TAG, "constructor - category: " + category + ", description: " + description);
        if (aids == null || aids.size() == 0) {
            this.aids = new ArrayList<String>();
        } else {
            if (aids.size() > AidGroup.MAX_NUM_AIDS) {
                throw new IllegalArgumentException("Too many AIDs in AID group.");
            }
            for (String aid : aids) {
                if (!CardEmulation.isValidAid(aid)) {
                    throw new IllegalArgumentException("AID " + aid + " is not a valid AID.");
                }
            }
            this.aids = new ArrayList<String>(aids.size());
            for (String aid : aids) {
                this.aids.add(aid.toUpperCase());
            }
        }
        if (isValidCategory(category)) {
            this.category = category;
        } else {
            this.category = CardEmulation.CATEGORY_OTHER;
        }
        this.description = description;
    }

    // public StAidGroup(List<String> aids, String category) {
    //     if (DBG) Log.d(TAG, "constructor - category: " + category );
    //     this.innerAidGroup = new AidGroup(aids, category);
    // }

    public StAidGroup(String category, String description) {
        if (DBG)
            Log.d(TAG, "constructor - category: " + category + ", description: " + description);
        this.aids = new ArrayList<String>();
        this.category = category;
        this.description = description;
    }

    // public StAidGroup(AidGroup aid) {
    //     if (DBG) Log.d(TAG, "constructor");
    //     this.innerAidGroup = aid;
    // }

    public StAidGroup(AidGroup aid, String description) {
        if (DBG) Log.d(TAG, "constructor");
        this.aids = aid.getAids();
        this.category = aid.getCategory();
        this.description = description;
    }

    /** @return the category of this AID group */
    public String getCategory() {
        return category;
    }

    /** @return the list of AIDs in this group */
    public List<String> getAids() {
        return aids;
    }

    // public void addAid(String aid) {
    //     if (aids.size() >= MAX_NUM_AIDS) {
    //         throw new IllegalArgumentException("Too many AIDs in AID group.");
    //     }
    //     this.aids.add(aid.toUpperCase());
    // }

    public AidGroup getAidGroup() {
        return new AidGroup(this.aids, this.category);
    }

    /** @return the decription of this AID group */
    public String getDescription() {
        if (DBG) Log.d(TAG, "getDescription() - description: " + description);
        return description;
    }

    @Override
    public String toString() {
        StringBuilder out = new StringBuilder("Category: " + category + ", AIDs:");
        for (String aid : aids) {
            out.append(aid);
            out.append(", ");
        }
        return out.toString();
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeString(category);
        dest.writeInt(aids.size());
        if (aids.size() > 0) {
            dest.writeStringList(aids);
        }
        dest.writeString(description);
    }

    public static final Parcelable.Creator<StAidGroup> CREATOR =
            new Parcelable.Creator<StAidGroup>() {

                @Override
                public StAidGroup createFromParcel(Parcel source) {
                    String category = source.readString();
                    int listSize = source.readInt();
                    ArrayList<String> aidList = new ArrayList<String>();
                    if (listSize > 0) {
                        source.readStringList(aidList);
                    }
                    String description = source.readString();
                    return new StAidGroup(aidList, category, description);
                }

                @Override
                public StAidGroup[] newArray(int size) {
                    return new StAidGroup[size];
                }
            };

    public static StAidGroup createFromXml(XmlPullParser parser)
            throws XmlPullParserException, IOException {
        if (DBG) Log.d(TAG, "createFromXml()");
        String category = null;
        String description = null;
        ArrayList<String> aids = new ArrayList<String>();
        StAidGroup group = null;
        boolean inGroup = false;

        int eventType = parser.getEventType();
        int minDepth = parser.getDepth();
        while (eventType != XmlPullParser.END_DOCUMENT && parser.getDepth() >= minDepth) {
            String tagName = parser.getName();
            if (eventType == XmlPullParser.START_TAG) {
                if (tagName.equals("aid")) {
                    if (inGroup) {
                        String aid = parser.getAttributeValue(null, "value");
                        if (aid != null) {
                            aids.add(aid.toUpperCase());
                        }
                    } else {
                        Log.d(TAG, "Ignoring <aid> tag while not in group");
                    }
                } else if (tagName.equals("aid-group")) {
                    category = parser.getAttributeValue(null, "category");
                    if (category == null) {
                        Log.e(TAG, "<aid-group> tag without valid category");
                        return null;
                    }
                    description = parser.getAttributeValue(null, "description");
                    inGroup = true;
                } else {
                    Log.d(TAG, "Ignoring unexpected tag: " + tagName);
                }
            } else if (eventType == XmlPullParser.END_TAG) {
                if (tagName.equals("aid-group") && inGroup && aids.size() > 0) {
                    group = new StAidGroup(aids, category, description);
                    break;
                }
            }
            eventType = parser.next();
        }
        if (DBG)
            Log.d(TAG, "createFromXml() - category: " + category + ", description: " + description);
        return group;
    }

    public void writeAsXml(XmlSerializer out) throws IOException {
        if (DBG) Log.d(TAG, "writeAsXml()");
        out.startTag(null, "aid-group");
        out.attribute(null, "category", category);
        if (description != null) out.attribute(null, "description", description);
        for (String aid : aids) {
            out.startTag(null, "aid");
            out.attribute(null, "value", aid);
            out.endTag(null, "aid");
        }
        out.endTag(null, "aid-group");
    }

    static boolean isValidCategory(String category) {
        return CardEmulation.CATEGORY_PAYMENT.equals(category)
                || CardEmulation.CATEGORY_OTHER.equals(category);
    }
}
