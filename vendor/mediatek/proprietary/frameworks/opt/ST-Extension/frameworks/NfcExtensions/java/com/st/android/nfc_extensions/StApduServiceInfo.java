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

import android.content.ComponentName;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.ResolveInfo;
import android.content.pm.ServiceInfo;
import android.content.res.Resources;
import android.content.res.Resources.NotFoundException;
import android.content.res.TypedArray;
import android.content.res.XmlResourceParser;
import android.graphics.Bitmap;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.nfc.cardemulation.AidGroup;
import android.nfc.cardemulation.ApduServiceInfo;
import android.nfc.cardemulation.CardEmulation;
import android.nfc.cardemulation.HostApduService;
import android.nfc.cardemulation.OffHostApduService;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.AttributeSet;
import android.util.Log;
import android.util.Xml;
import java.io.FileDescriptor;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

/// M: Logs @{
/// @}

/** @hide */
public final class StApduServiceInfo implements Parcelable {
    static final String TAG = "APINfc_StApduServiceInfo";
    static final boolean DBG = true;

    /** The service that implements this */
    final ResolveInfo mService;

    /** Description of the service */
    final String mDescription;

    /** Whether this service represents AIDs running on the host CPU */
    final boolean mOnHost;

    /** Offhost reader name. eg: SIM, eSE etc */
    String mOffHostName;

    /** Offhost reader name from manifest file. Used for unsetOffHostSecureElement() */
    final String mStaticOffHostName;

    /** Mapping from category to static AID group */
    final HashMap<String, StAidGroup> mStaticStAidGroups;

    /** Mapping from category to dynamic AID group */
    final HashMap<String, StAidGroup> mDynamicStAidGroups;

    /** Whether this service should only be started when the device is unlocked. */
    final boolean mRequiresDeviceUnlock;

    /** The id of the service banner specified in XML. */
    final int mBannerResourceId;

    /** The uid of the package the service belongs to */
    final int mUid;

    /** Settings Activity for this service */
    final String mSettingsActivityName;

    // name of secure element (as per GSMA TS12)
    static final String SECURE_ELEMENT_ESE = "eSE1";
    static final String SECURE_ELEMENT_UICC = "SIM";
    static final String SECURE_ELEMENT_UICC1 = "SIM1";
    static final String SECURE_ELEMENT_UICC2 = "SIM2";
    // name of secure element (older proprietary implementations)
    static final String SECURE_ELEMENT_ALT_ESE = "eSE";
    static final String SECURE_ELEMENT_ALT_UICC = "UICC";
    static final String SECURE_ELEMENT_ALT_UICC2 = "UICC2";
    // index of secure element
    public static final int SECURE_ELEMENT_ROUTE_ESE = 1;
    public static final int SECURE_ELEMENT_ROUTE_UICC = 2;
    public static final int SECURE_ELEMENT_ROUTE_UICC2 = 0x4;

    // power state value
    static final int POWER_STATE_SWITCH_ON = 1;
    static final int POWER_STATE_SWITCH_OFF = 2;
    static final int POWER_STATE_BATTERY_OFF = 4;

    /** The Drawable of the service banner specified by the Application Dynamically. */
    public final Drawable mBanner;

    /**
     * This says whether the Service is enabled or disabled by the user By default it is
     * disabled.This is only applicable for OTHER category. states are as follows ENABLING(service
     * creation)->ENABLED(Committed to Routing)-> DISABLING(user requested to
     * disable)->DISABLED(Removed from Routing). In ENABLED or DISABLING state, this service will be
     * accounted for routing.
     */
    int mServiceState;

    /*
     * This says if this service was accounted in the LMRT size computation.
     */
    boolean mWasAccounted;

    /** @hide */
    public StApduServiceInfo(
            ResolveInfo info,
            String description,
            ArrayList<StAidGroup> staticStAidGroups,
            ArrayList<StAidGroup> dynamicStAidGroups,
            boolean requiresUnlock,
            int bannerResource,
            int uid,
            String settingsActivityName,
            String offHost,
            String staticOffHost,
            Drawable banner) {

        if (DBG)
            Log.d(
                    TAG,
                    "Constructor - offHost: "
                            + offHost
                            + ", description: "
                            + description
                            + ", nb of staticStAidGroups: "
                            + staticStAidGroups.size()
                            + ", nb of dynamicStAidGroups: "
                            + dynamicStAidGroups.size());

        this.mService = info;
        this.mDescription = description;
        this.mStaticStAidGroups = new HashMap<String, StAidGroup>();
        this.mDynamicStAidGroups = new HashMap<String, StAidGroup>();
        this.mOffHostName = offHost;
        this.mStaticOffHostName = staticOffHost;
        this.mOnHost = (offHost == null);
        this.mRequiresDeviceUnlock = requiresUnlock;
        this.mBanner = banner;
        this.mServiceState = StConstants.SERVICE_STATE_ENABLING;
        this.mWasAccounted = true;
        if (staticStAidGroups != null) {
            for (StAidGroup stAidGroup : staticStAidGroups) {
                this.mStaticStAidGroups.put(stAidGroup.getCategory(), stAidGroup);
            }
        }

        if (dynamicStAidGroups != null) {
            for (StAidGroup stAidGroup : dynamicStAidGroups) {
                this.mDynamicStAidGroups.put(stAidGroup.getCategory(), stAidGroup);
            }
        }
        this.mBannerResourceId = bannerResource;
        this.mUid = uid;
        this.mSettingsActivityName = settingsActivityName;
    }

    public StApduServiceInfo(PackageManager pm, ResolveInfo info, boolean onHost)
            throws XmlPullParserException, IOException {

        if (DBG) Log.d(TAG, "Constructor - onHost: " + onHost + ", reading from meta-data");

        this.mBanner = null;
        this.mServiceState = StConstants.SERVICE_STATE_ENABLING;
        ServiceInfo si = info.serviceInfo;
        XmlResourceParser parser = null;

        try {
            if (onHost) {
                parser = si.loadXmlMetaData(pm, HostApduService.SERVICE_META_DATA);
                if (parser == null) {
                    throw new XmlPullParserException(
                            "No " + HostApduService.SERVICE_META_DATA + " meta-data");
                }
            } else {
                parser = si.loadXmlMetaData(pm, OffHostApduService.SERVICE_META_DATA);

                if (parser == null) {
                    throw new XmlPullParserException(
                            "No " + OffHostApduService.SERVICE_META_DATA + " meta-data");
                }
            }

            int eventType = parser.getEventType();
            while (eventType != XmlPullParser.START_TAG
                    && eventType != XmlPullParser.END_DOCUMENT) {
                eventType = parser.next();
            }

            String tagName = parser.getName();
            if (onHost && !"host-apdu-service".equals(tagName)) {
                throw new XmlPullParserException(
                        "Meta-data does not start with <host-apdu-service> tag");
            } else if (!onHost && !"offhost-apdu-service".equals(tagName)) {
                throw new XmlPullParserException(
                        "Meta-data does not start with <offhost-apdu-service> tag");
            }

            Resources res = pm.getResourcesForApplication(si.applicationInfo);
            AttributeSet attrs = Xml.asAttributeSet(parser);
            if (onHost) {
                TypedArray sa =
                        res.obtainAttributes(
                                attrs, com.android.internal.R.styleable.HostApduService);
                mService = info;
                mDescription =
                        sa.getString(com.android.internal.R.styleable.HostApduService_description);
                mRequiresDeviceUnlock =
                        sa.getBoolean(
                                com.android
                                        .internal
                                        .R
                                        .styleable
                                        .HostApduService_requireDeviceUnlock,
                                false);
                mBannerResourceId =
                        sa.getResourceId(
                                com.android.internal.R.styleable.HostApduService_apduServiceBanner,
                                -1);
                mSettingsActivityName =
                        sa.getString(
                                com.android.internal.R.styleable.HostApduService_settingsActivity);
                mOffHostName = null;
                mStaticOffHostName = mOffHostName;
                sa.recycle();
            } else {
                TypedArray sa =
                        res.obtainAttributes(
                                attrs, com.android.internal.R.styleable.OffHostApduService);
                mService = info;
                mDescription =
                        sa.getString(
                                com.android.internal.R.styleable.OffHostApduService_description);
                mRequiresDeviceUnlock = false;
                mBannerResourceId =
                        sa.getResourceId(
                                com.android
                                        .internal
                                        .R
                                        .styleable
                                        .OffHostApduService_apduServiceBanner,
                                -1);
                mSettingsActivityName =
                        sa.getString(
                                com.android.internal.R.styleable.HostApduService_settingsActivity);
                mOffHostName =
                        sa.getString(
                                com.android
                                        .internal
                                        .R
                                        .styleable
                                        .OffHostApduService_secureElementName);
                if (mOffHostName != null) {
                    if (mOffHostName.equals("eSE")) {
                        mOffHostName = "eSE1";
                    } else if (mOffHostName.equals("SIM")) {
                        mOffHostName = "SIM1";
                    }
                }
                mStaticOffHostName = mOffHostName;
                sa.recycle();
            }

            mStaticStAidGroups = new HashMap<String, StAidGroup>();
            mDynamicStAidGroups = new HashMap<String, StAidGroup>();
            mOnHost = onHost;
            if (DBG) Log.d(TAG, "Constructor - mService: " + mService.resolvePackageName);
            if (DBG) Log.d(TAG, "Constructor - mDescription: " + mDescription);
            if (DBG) Log.d(TAG, "Constructor - mOffHostName: " + mOffHostName);

            final int depth = parser.getDepth();
            StAidGroup currentGroup = null;

            // Parsed values for the current AID group
            while (((eventType = parser.next()) != XmlPullParser.END_TAG
                            || parser.getDepth() > depth)
                    && eventType != XmlPullParser.END_DOCUMENT) {
                tagName = parser.getName();
                if (eventType == XmlPullParser.START_TAG
                        && "aid-group".equals(tagName)
                        && currentGroup == null) {
                    final TypedArray groupAttrs =
                            res.obtainAttributes(attrs, com.android.internal.R.styleable.AidGroup);
                    // Get category of AID group
                    String groupCategory =
                            groupAttrs.getString(
                                    com.android.internal.R.styleable.AidGroup_category);
                    String groupDescription =
                            groupAttrs.getString(
                                    com.android.internal.R.styleable.AidGroup_description);
                    if (!CardEmulation.CATEGORY_PAYMENT.equals(groupCategory)) {
                        groupCategory = CardEmulation.CATEGORY_OTHER;
                    }

                    if (DBG) Log.d(TAG, "Constructor - groupCategory: " + groupCategory);

                    currentGroup = mStaticStAidGroups.get(groupCategory);
                    if (currentGroup != null) {
                        if (!CardEmulation.CATEGORY_OTHER.equals(groupCategory)) {
                            Log.e(
                                    TAG,
                                    "Not allowing multiple aid-groups in the "
                                            + groupCategory
                                            + " category");
                            currentGroup = null;
                        }
                    } else {
                        currentGroup = new StAidGroup(groupCategory, groupDescription);
                    }
                    groupAttrs.recycle();
                } else if (eventType == XmlPullParser.END_TAG
                        && "aid-group".equals(tagName)
                        && currentGroup != null) {
                    if (currentGroup.aids.size() > 0) {
                        if (!mStaticStAidGroups.containsKey(currentGroup.category)) {
                            mStaticStAidGroups.put(currentGroup.category, currentGroup);
                        }
                    } else {
                        Log.w(TAG, "Not adding <aid-group> with empty or invalid AIDs");
                    }
                    currentGroup = null;
                } else if (eventType == XmlPullParser.START_TAG
                        && "aid-filter".equals(tagName)
                        && currentGroup != null) {
                    final TypedArray a =
                            res.obtainAttributes(attrs, com.android.internal.R.styleable.AidFilter);
                    String aid =
                            a.getString(com.android.internal.R.styleable.AidFilter_name)
                                    .toUpperCase();
                    if (CardEmulation.isValidAid(aid) && !currentGroup.aids.contains(aid)) {
                        currentGroup.aids.add(aid);
                    } else {
                        Log.e(TAG, "Ignoring invalid or duplicate aid: " + aid);
                    }
                    a.recycle();
                } else if (eventType == XmlPullParser.START_TAG
                        && "aid-prefix-filter".equals(tagName)
                        && currentGroup != null) {
                    final TypedArray a =
                            res.obtainAttributes(attrs, com.android.internal.R.styleable.AidFilter);
                    String aid =
                            a.getString(com.android.internal.R.styleable.AidFilter_name)
                                    .toUpperCase();
                    // Add wildcard char to indicate prefix
                    aid = aid.concat("*");
                    if (CardEmulation.isValidAid(aid) && !currentGroup.aids.contains(aid)) {
                        currentGroup.aids.add(aid);
                    } else {
                        Log.e(TAG, "Ignoring invalid or duplicate aid: " + aid);
                    }
                    a.recycle();
                } else if (eventType == XmlPullParser.START_TAG
                        && tagName.equals("aid-suffix-filter")
                        && currentGroup != null) {
                    final TypedArray a =
                            res.obtainAttributes(attrs, com.android.internal.R.styleable.AidFilter);
                    String aid =
                            a.getString(com.android.internal.R.styleable.AidFilter_name)
                                    .toUpperCase();
                    // Add wildcard char to indicate suffix
                    aid = aid.concat("#");
                    if (CardEmulation.isValidAid(aid) && !currentGroup.aids.contains(aid)) {
                        currentGroup.aids.add(aid);
                    } else {
                        Log.e(TAG, "Ignoring invalid or duplicate aid: " + aid);
                    }
                    a.recycle();
                }
            }
        } catch (NameNotFoundException e) {
            throw new XmlPullParserException("Unable to create context for: " + si.packageName);
        } finally {
            if (parser != null) parser.close();
        }
        // Set uid
        mUid = si.applicationInfo.uid;
    }

    public ComponentName getComponent() {
        return new ComponentName(mService.serviceInfo.packageName, mService.serviceInfo.name);
    }

    public String getOffHostSecureElement() {
        return mOffHostName;
    }

    static ArrayList<AidGroup> stAidGroups2AidGroups(ArrayList<StAidGroup> stAidGroup) {
        ArrayList<AidGroup> aidGroups = new ArrayList<AidGroup>();
        if (stAidGroup != null) {
            for (StAidGroup stag : stAidGroup) {
                AidGroup ag = stag.getAidGroup();
                aidGroups.add(ag);
            }
        }
        return aidGroups;
    }

    /**
     * Returns a consolidated list of AIDs from the AID groups registered by this service. Note that
     * if a service has both a static (manifest-based) AID group for a category and a dynamic AID
     * group, only the dynamically registered AIDs will be returned for that category.
     *
     * @return List of AIDs registered by the service
     */
    public List<String> getAids() {
        final ArrayList<String> aids = new ArrayList<String>();
        for (StAidGroup group : getStAidGroups()) {
            aids.addAll(group.aids);
        }
        return aids;
    }

    public List<String> getPrefixAids() {
        final ArrayList<String> prefixAids = new ArrayList<String>();
        for (StAidGroup group : getStAidGroups()) {
            for (String aid : group.aids) {
                if (aid.endsWith("*")) {
                    prefixAids.add(aid);
                }
            }
        }
        return prefixAids;
    }

    public List<String> getSubsetAids() {
        final ArrayList<String> subsetAids = new ArrayList<String>();
        for (StAidGroup group : getStAidGroups()) {
            for (String aid : group.aids) {
                if (aid.endsWith("#")) {
                    subsetAids.add(aid);
                }
            }
        }
        return subsetAids;
    }

    /** Returns the registered AID group for this category. */
    public StAidGroup getDynamicStAidGroupForCategory(String category) {
        return mDynamicStAidGroups.get(category);
    }

    public boolean removeDynamicStAidGroupForCategory(String category) {
        return (mDynamicStAidGroups.remove(category) != null);
    }

    /**
     * Returns a consolidated list of AID groups registered by this service. Note that if a service
     * has both a static (manifest-based) AID group for a category and a dynamic AID group, only the
     * dynamically registered AID group will be returned for that category.
     *
     * @return List of AIDs registered by the service
     */
    public ArrayList<StAidGroup> getStAidGroups() {
        final ArrayList<StAidGroup> groups = new ArrayList<StAidGroup>();
        for (Map.Entry<String, StAidGroup> entry : mDynamicStAidGroups.entrySet()) {
            groups.add(entry.getValue());
        }
        for (Map.Entry<String, StAidGroup> entry : mStaticStAidGroups.entrySet()) {
            if (!mDynamicStAidGroups.containsKey(entry.getKey())) {
                // Consolidate AID groups - don't return static ones
                // if a dynamic group exists for the category.
                groups.add(entry.getValue());
            }
        }
        return groups;
    }

    /* Same except it returns Android-standard format */
    public ArrayList<AidGroup> getLegacyAidGroups() {
        return stAidGroups2AidGroups(getStAidGroups());
    }

    /**
     * Returns the category to which this service has attributed the AID that is passed in, or null
     * if we don't know this AID.
     */
    public String getCategoryForAid(String aid) {
        ArrayList<StAidGroup> groups = getStAidGroups();
        for (StAidGroup group : groups) {
            if (group.aids.contains(aid.toUpperCase())) {
                return group.category;
            }
        }
        return null;
    }

    public boolean hasCategory(String category) {
        return (mStaticStAidGroups.containsKey(category)
                || mDynamicStAidGroups.containsKey(category));
    }

    public boolean isOnHost() {
        return mOnHost;
    }

    public boolean requiresUnlock() {
        return mRequiresDeviceUnlock;
    }

    public String getDescription() {
        if (mDescription != null) {
            return mDescription;
        }
        return mService.serviceInfo.name;
    }

    public String getGsmaDescription() {
        String result = getDescription();
        String aidDescr = null;

        StAidGroup grp = mDynamicStAidGroups.get(CardEmulation.CATEGORY_OTHER);
        if (grp != null) {
            if (grp.getDescription() != null) {
                aidDescr = grp.getDescription();
            }
        } else {
            grp = mStaticStAidGroups.get(CardEmulation.CATEGORY_OTHER);
            if ((grp != null) && (grp.getDescription() != null)) {
                aidDescr = grp.getDescription();
            }
        }

        if (aidDescr != null) {
            result += " (" + aidDescr + ")";
        }

        return result;
    }

    public String getGsmaDescription(PackageManager pm) {
        CharSequence label = this.loadLabel(pm);
        if (label == null) {
            label = this.loadAppLabel(pm);
        }
        if (label != null) {
            return label.toString() + " - " + this.getGsmaDescription();
        }
        return this.getGsmaDescription();
    }

    public int getUid() {
        return mUid;
    }

    public void setOrReplaceDynamicStAidGroup(StAidGroup aidGroup) {
        mDynamicStAidGroups.put(aidGroup.getCategory(), aidGroup);
    }

    public void setOffHostSecureElement(String offHost) {
        mOffHostName = offHost;
    }

    /**
     * Resets the off host Secure Element to statically defined by the service in the manifest file.
     */
    public void unsetOffHostSecureElement() {
        mOffHostName = mStaticOffHostName;
    }

    public CharSequence loadLabel(PackageManager pm) {
        return mService.loadLabel(pm);
    }

    public CharSequence loadAppLabel(PackageManager pm) {
        try {
            return pm.getApplicationLabel(
                    pm.getApplicationInfo(
                            mService.resolvePackageName, PackageManager.GET_META_DATA));
        } catch (PackageManager.NameNotFoundException e) {
            return null;
        }
    }

    public Drawable loadIcon(PackageManager pm) {
        return mService.loadIcon(pm);
    }

    public Drawable loadBanner(PackageManager pm) {
        Resources res;
        Drawable banner = null;
        if (mBannerResourceId == -1) {
            banner = mBanner;
        } else {
            try {
                res = pm.getResourcesForApplication(mService.serviceInfo.packageName);
                if (res != null) {
                    banner = res.getDrawable(mBannerResourceId);
                }
            } catch (NotFoundException e) {
                Log.e(TAG, "Could not load banner.");
            } catch (NameNotFoundException e) {
                Log.e(TAG, "Could not load banner (name)");
            }
        }
        return banner;
    }

    public String getSettingsActivityName() {
        return mSettingsActivityName;
    }

    @Override
    public String toString() {
        StringBuilder out = new StringBuilder("StApduService: ");
        out.append(getComponent());
        out.append(", description: " + mDescription);
        out.append(", Static AID Groups: ");
        for (StAidGroup aidGroup : mStaticStAidGroups.values()) {
            out.append(aidGroup.toString());
        }
        out.append(", Dynamic AID Groups: ");
        for (StAidGroup aidGroup : mDynamicStAidGroups.values()) {
            out.append(aidGroup.toString());
        }
        return out.toString();
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof StApduServiceInfo)) return false;
        StApduServiceInfo thatService = (StApduServiceInfo) o;

        return thatService.getComponent().equals(this.getComponent());
    }

    @Override
    public int hashCode() {
        return getComponent().hashCode();
    }

    @Override
    public int describeContents() {
        return 0;
    }

    public ResolveInfo getResolveInfo() {
        return mService;
    }

    /**
     * This is a convenience function to create an ApduServiceInfo object of the current
     * StApduServiceInfo. It is required for legacy functions which expect an ApduServiceInfo on a
     * Binder interface.
     *
     * @return An ApduServiceInfo object which can be correctly serialized as parcel
     */
    public ApduServiceInfo createApduServiceInfo() {
        return new ApduServiceInfo(
                this.mService,
                this.getDescription(),
                stAidGroups2AidGroups(this.getStaticStAidGroups()),
                stAidGroups2AidGroups(this.getDynamicStAidGroups()),
                this.requiresUnlock(),
                this.getBannerId(),
                this.getUid(),
                this.getSettingsActivityName(),
                this.getOffHostSecureElement(),
                this.mStaticOffHostName);
    }

    /**
     * This api can be used to find the total aid size registered by this service.
     *
     * <p>This returns the size of only {@link #CardEmulation.CATEGORY_OTHER}.
     *
     * <p>This includes both static and dynamic aid groups
     *
     * @param category The category of the corresponding service .{@link
     *     #CardEmulation.CATEGORY_OTHER}.
     * @return The aid cache size for particular category.
     */
    public int getAidCacheSize(String category) {
        int aidSize = 0x00;
        if (!CardEmulation.CATEGORY_OTHER.equals(category)
                || !hasCategory(CardEmulation.CATEGORY_OTHER)) {
            return 0x00;
        }
        aidSize = getAidCacheSizeForCategory(CardEmulation.CATEGORY_OTHER);
        return aidSize;
    }

    public int getAidCacheSizeForCategory(String category) {
        ArrayList<StAidGroup> stAidGroups = new ArrayList<StAidGroup>();
        List<String> aids;
        int aidCacheSize = 0x00;
        int aidLen = 0x00;
        stAidGroups.add(mStaticStAidGroups.get(category));
        stAidGroups.add(mDynamicStAidGroups.get(category));
        for (StAidGroup aidCache : stAidGroups) {
            if (aidCache == null) {
                continue;
            }
            aids = aidCache.getAids();
            if (aids == null) {
                continue;
            }
            for (String aid : aids) {
                if (aid == null) {
                    continue;
                }
                aidLen = aid.length();
                if (aid.endsWith("*") || aid.endsWith("#")) {
                    aidLen = aidLen - 1;
                }
                aidCacheSize += aidLen >> 1;
            }
        }
        return aidCacheSize;
    }

    /**
     * This api gives the size occupied by this service's AIDs in routing table Only the AIDs of
     * {@link #CardEmulation.CATEGORY_OTHER} are accounted. This API does not show the really used
     * size, just what AID size is if it needs to be stored.
     *
     * @return The num of bytes
     */
    public int getCatOthersAidSizeInLmrt() {
        // Compute Aid group size: length of AID + 4 bytes per AID (NCI routing info)
        return getAidCacheSizeForCategory(CardEmulation.CATEGORY_OTHER)
                + getTotalAidNum(CardEmulation.CATEGORY_OTHER) * 4;
    }

    /**
     * This api can be used to find the total aids count registered by this service.
     *
     * <p>This returns the size of only {@link #CardEmulation.CATEGORY_OTHER}.
     *
     * <p>This includes both static and dynamic aid groups
     *
     * @param category The category of the corresponding service .{@link
     *     #CardEmulation.CATEGORY_OTHER}.
     * @return The num of aids corresponding to particular cateogry
     */
    public int getTotalAidNum(String category) {
        int aidTotalNum = 0x00;
        if (!CardEmulation.CATEGORY_OTHER.equals(category)
                || !hasCategory(CardEmulation.CATEGORY_OTHER)) {
            return 0x00;
        }
        aidTotalNum = getTotalAidNumCategory(CardEmulation.CATEGORY_OTHER);
        return aidTotalNum;
    }

    private int getTotalAidNumCategory(String category) {
        ArrayList<StAidGroup> aidGroups = new ArrayList<StAidGroup>();
        List<String> aids;
        int aidTotalNum = 0x00;
        aidGroups.add(mStaticStAidGroups.get(category));
        aidGroups.add(mDynamicStAidGroups.get(category));
        for (StAidGroup aidCache : aidGroups) {
            if (aidCache == null) {
                continue;
            }
            aids = aidCache.getAids();
            if (aids == null) {
                continue;
            }
            for (String aid : aids) {
                if (aid != null && aid.length() > 0x00) {
                    aidTotalNum++;
                }
            }
        }
        return aidTotalNum;
    }

    /** @hide */
    public ArrayList<StAidGroup> getStaticStAidGroups() {
        final ArrayList<StAidGroup> groups = new ArrayList<StAidGroup>();

        for (Map.Entry<String, StAidGroup> entry : mStaticStAidGroups.entrySet()) {
            groups.add(entry.getValue());
        }
        return groups;
    }

    /** @hide */
    public ArrayList<StAidGroup> getDynamicStAidGroups() {
        final ArrayList<StAidGroup> groups = new ArrayList<StAidGroup>();
        for (Map.Entry<String, StAidGroup> entry : mDynamicStAidGroups.entrySet()) {
            groups.add(entry.getValue());
        }
        return groups;
    }

    public int getBannerId() {
        return mBannerResourceId;
    }

    public boolean isServiceEnabled(String category) {
        if (category != CardEmulation.CATEGORY_OTHER) {
            return true;
        }

        if ((mServiceState == StConstants.SERVICE_STATE_ENABLED)
                || (mServiceState == StConstants.SERVICE_STATE_DISABLING)) {
            return true;
        } else {
            /*SERVICE_STATE_DISABLED or SERVICE_STATE_ENABLING*/
            return false;
        }
    }

    /**
     * This method is invoked before the service is commited to routing table. mServiceState is
     * previous state of the service, and, user is now requesting to enable/disable (using
     * flagEnable) this service before committing all the services to routing table.
     *
     * @param flagEnable To Enable/Disable the service. FALSE Disable service TRUE Enable service
     */
    public void enableService(String category, boolean flagEnable) {
        if (category != CardEmulation.CATEGORY_OTHER) {
            return;
        }
        Log.d(
                TAG,
                "setServiceState:Description:"
                        + mDescription
                        + ":InternalState:"
                        + mServiceState
                        + ":flagEnable:"
                        + flagEnable);
        if (((mServiceState == StConstants.SERVICE_STATE_ENABLED) && (flagEnable == true))
                || ((mServiceState == StConstants.SERVICE_STATE_DISABLED) && (flagEnable == false))
                || ((mServiceState == StConstants.SERVICE_STATE_DISABLING) && (flagEnable == false))
                || ((mServiceState == StConstants.SERVICE_STATE_ENABLING)
                        && (flagEnable == true))) {
            /*No change in state*/
            return;
        } else if ((mServiceState == StConstants.SERVICE_STATE_ENABLED) && (flagEnable == false)) {
            mServiceState = StConstants.SERVICE_STATE_DISABLING;
        } else if ((mServiceState == StConstants.SERVICE_STATE_DISABLED) && (flagEnable == true)) {
            mServiceState = StConstants.SERVICE_STATE_ENABLING;
        } else if ((mServiceState == StConstants.SERVICE_STATE_DISABLING) && (flagEnable == true)) {
            mServiceState = StConstants.SERVICE_STATE_ENABLED;
        } else if ((mServiceState == StConstants.SERVICE_STATE_ENABLING) && (flagEnable == false)) {
            mServiceState = StConstants.SERVICE_STATE_DISABLED;
        }
    }

    public int getServiceState(String category) {
        if (category != CardEmulation.CATEGORY_OTHER) {
            return StConstants.SERVICE_STATE_ENABLED;
        }

        return mServiceState;
    }

    public int setServiceState(String category, int state) {
        if (category != CardEmulation.CATEGORY_OTHER) {
            return StConstants.SERVICE_STATE_ENABLED;
        }

        mServiceState = state;
        return mServiceState;
    }

    public boolean getWasAccounted() {
        return mWasAccounted;
    }

    /**
     * Updates the state of the service based on the commit status This method needs to be invoked
     * after current service is pushed for the commit to routing table
     *
     * @param commitStatus Result of the commit. FALSE if the commit failed. Reason for ex: there
     *     was an overflow of routing table TRUE if the commit was successful
     */
    public void updateServiceCommitStatus(String category, boolean commitStatus) {
        if (category != CardEmulation.CATEGORY_OTHER) {
            return;
        }
        Log.d(
                TAG,
                "updateServiceCommitStatus(enter) - Description: "
                        + mDescription
                        + ", InternalState: "
                        + mServiceState
                        + ", commitStatus: "
                        + commitStatus);

        if (commitStatus) {
            /*Commit was successful and all newly added services were registered,
             * disabled applications were removed/unregistered from routing entries*/
            if (mServiceState == StConstants.SERVICE_STATE_DISABLING) {
                mServiceState = StConstants.SERVICE_STATE_DISABLED;
            } else if (mServiceState == StConstants.SERVICE_STATE_ENABLING) {
                mServiceState = StConstants.SERVICE_STATE_ENABLED;
            }

            mWasAccounted = (mServiceState == StConstants.SERVICE_STATE_ENABLED);

        } else {
            /*Commit failed and all newly added services were not registered successfully.
             * disabled applications were not successfully disabled*/
            mWasAccounted =
                    (mServiceState == StConstants.SERVICE_STATE_ENABLED)
                            || (mServiceState == StConstants.SERVICE_STATE_ENABLING);

            if (mServiceState == StConstants.SERVICE_STATE_DISABLING) {
                mServiceState = StConstants.SERVICE_STATE_ENABLED;
            } else if (mServiceState == StConstants.SERVICE_STATE_ENABLING) {
                mServiceState = StConstants.SERVICE_STATE_DISABLED;
            }
        }

        Log.d(TAG, "updateServiceCommitStatus(exit) - InternalState: " + mServiceState);
    }

    public static String serviceStateToString(int state) {
        switch (state) {
            case StConstants.SERVICE_STATE_DISABLED:
                return "DISABLED";
            case StConstants.SERVICE_STATE_ENABLED:
                return "ENABLED";
            case StConstants.SERVICE_STATE_ENABLING:
                return "ENABLING";
            case StConstants.SERVICE_STATE_DISABLING:
                return "DISABLING";
            default:
                return "UNKNOWN";
        }
    }

    //    public void writeToXml(XmlSerializer out) throws IOException {
    //        out.attribute(null, "description", mDescription);
    //        out.attribute(null, "uid", Integer.toString(mUid));
    //        out.attribute(null, "seId", Integer.toString(mSeExtension.seId));
    //        out.attribute(null, "bannerId", Integer.toString(mBannerResourceId));
    //        for (StAidGroup group : mDynamicStAidGroups.values()) {
    //            group.writeAsXml(out);
    //        }
    //    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        mService.writeToParcel(dest, flags);
        dest.writeString(mDescription);
        dest.writeInt(mOnHost ? 1 : 0);
        dest.writeString(mOffHostName);
        dest.writeString(mStaticOffHostName);
        dest.writeInt(mStaticStAidGroups.size());
        if (mStaticStAidGroups.size() > 0) {
            dest.writeTypedList(new ArrayList<StAidGroup>(mStaticStAidGroups.values()));
        }
        dest.writeInt(mDynamicStAidGroups.size());
        if (mDynamicStAidGroups.size() > 0) {
            dest.writeTypedList(new ArrayList<StAidGroup>(mDynamicStAidGroups.values()));
        }
        dest.writeInt(mRequiresDeviceUnlock ? 1 : 0);
        dest.writeInt(mBannerResourceId);
        dest.writeInt(mUid);
        dest.writeString(mSettingsActivityName);

        if (mBanner != null) {
            Bitmap bitmap = (Bitmap) ((BitmapDrawable) mBanner).getBitmap();
            dest.writeParcelable(bitmap, flags);
        } else {
            dest.writeParcelable(null, flags);
        }
        dest.writeInt(mServiceState);
    };

    public static final Parcelable.Creator<StApduServiceInfo> CREATOR =
            new Parcelable.Creator<StApduServiceInfo>() {
                @Override
                public StApduServiceInfo createFromParcel(Parcel source) {
                    ResolveInfo info = ResolveInfo.CREATOR.createFromParcel(source);
                    String description = source.readString();
                    boolean onHost = source.readInt() != 0;
                    String offHostName = source.readString();
                    String staticOffHostName = source.readString();
                    ArrayList<StAidGroup> staticStAidGroups = new ArrayList<StAidGroup>();
                    int numStaticGroups = source.readInt();
                    if (numStaticGroups > 0) {
                        source.readTypedList(staticStAidGroups, StAidGroup.CREATOR);
                    }
                    ArrayList<StAidGroup> dynamicStAidGroups = new ArrayList<StAidGroup>();
                    int numDynamicGroups = source.readInt();
                    if (numDynamicGroups > 0) {
                        source.readTypedList(dynamicStAidGroups, StAidGroup.CREATOR);
                    }
                    boolean requiresUnlock = source.readInt() != 0;
                    int bannerResource = source.readInt();
                    int uid = source.readInt();
                    String settingsActivityName = source.readString();
                    Drawable drawable = null;
                    if (getClass().getClassLoader() != null) {
                        Bitmap bitmap = (Bitmap) source.readParcelable(getClass().getClassLoader());
                        if (bitmap != null) {
                            drawable = new BitmapDrawable(bitmap);
                            // bannerResource = -1;
                        }
                    }
                    StApduServiceInfo service =
                            new StApduServiceInfo(
                                    info,
                                    description,
                                    staticStAidGroups,
                                    dynamicStAidGroups,
                                    requiresUnlock,
                                    bannerResource,
                                    uid,
                                    settingsActivityName,
                                    offHostName,
                                    staticOffHostName,
                                    drawable);
                    service.setServiceState(CardEmulation.CATEGORY_OTHER, source.readInt());
                    return service;
                }

                @Override
                public StApduServiceInfo[] newArray(int size) {
                    return new StApduServiceInfo[size];
                }
            };

    public void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        String desc;
        pw.println("    " + getComponent() + " (Description: " + getDescription() + ")");
        if (mOnHost) {
            pw.println("    On Host Service");
        } else {
            pw.println("    Off-host Service");
            pw.println(
                    "        "
                            + "Current off-host SE"
                            + mOffHostName
                            + " static off-host: "
                            + mOffHostName);
        }
        pw.println("    Static off-host Secure Element:");
        pw.println("    Static AID groups:");
        for (StAidGroup group : mStaticStAidGroups.values()) {
            pw.println("        Category: " + group.category);
            desc = group.getDescription();
            if (desc != null) {
                pw.println("           Descr: " + desc);
            }
            for (String aid : group.aids) {
                pw.println("            AID: " + aid);
            }
        }
        pw.println("    Dynamic AID groups:");
        for (StAidGroup group : mDynamicStAidGroups.values()) {
            pw.println("        Category: " + group.category);
            desc = group.getDescription();
            if (desc != null) {
                pw.println("           Descr: " + desc);
            }
            for (String aid : group.aids) {
                pw.println("            AID: " + aid);
            }
        }
        pw.println("    Settings Activity: " + mSettingsActivityName);
        if (hasCategory(CardEmulation.CATEGORY_OTHER)) {
            pw.println("    Service State: " + serviceStateToString(mServiceState));
        }
    }
}
