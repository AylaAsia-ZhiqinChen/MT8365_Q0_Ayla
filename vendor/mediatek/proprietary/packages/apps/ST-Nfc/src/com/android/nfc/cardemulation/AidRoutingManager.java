/*
 * Copyright (C) 2013 The Android Open Source Project
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
package com.android.nfc.cardemulation;

import android.util.Log;
import android.util.SparseArray;
import android.util.StatsLog;
import com.android.nfc.NfcService;
import java.io.FileDescriptor;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

public class AidRoutingManager {

    static final String TAG = "HCENfc_AidRoutingManager";

    static final boolean DBG = true;
    static final boolean DBG2 =
            DBG && android.os.SystemProperties.get("persist.st_nfc_debug").equals("1");

    static final int ROUTE_HOST = 0x00;

    // Every routing table entry is matched exact
    static final int AID_MATCHING_EXACT_ONLY = 0x00;
    // Every routing table entry can be matched either exact or prefix
    static final int AID_MATCHING_EXACT_OR_PREFIX = 0x01;
    // Every routing table entry is matched as a prefix
    static final int AID_MATCHING_PREFIX_ONLY = 0x02;
    // Every routing table entry can be matched either exact or prefix or subset only
    static final int AID_MATCHING_EXACT_OR_SUBSET_OR_PREFIX = 0x03;

    int mDefaultIsoDepRoute;
    // Let mDefaultRoute as default aid route
    int mDefaultRoute;

    int mMaxAidRoutingTableSize;

    final byte[] mOffHostRouteUicc;
    final byte[] mOffHostRouteEse;
    // Used for backward compatibility in case application doesn't specify the
    // SE
    final int mDefaultOffHostRoute;

    // How the NFC controller can match AIDs in the routing table;
    // see AID_MATCHING constants
    final int mAidMatchingSupport;

    final Object mLock = new Object();
    boolean mLastCommitStatus;

    // mAidRoutingTable contains the current routing table. The index is the route ID.
    // The route can include routes to a eSE/UICC.
    SparseArray<Set<String>> mAidRoutingTable = new SparseArray<Set<String>>();

    // Easy look-up what the route is for a certain AID
    HashMap<String, Integer> mRouteForAid = new HashMap<String, Integer>();

    private native int doGetDefaultRouteDestination();

    private native int doGetDefaultOffHostRouteDestination();

    private native byte[] doGetOffHostUiccDestination();

    private native byte[] doGetOffHostEseDestination();

    private native int doGetAidMatchingMode();

    private native int doGetDefaultIsoDepRouteDestination();

    final class AidEntry {
        boolean isOnHost;
        String offHostSE;
        int route;
        int aidInfo;
    }

    public AidRoutingManager() {

        if (DBG) Log.d(TAG, "constructor");

        mDefaultRoute = doGetDefaultRouteDestination();
        if (DBG) Log.d(TAG, "mDefaultRoute=0x" + String.format("%02X", mDefaultRoute));
        mDefaultOffHostRoute = doGetDefaultOffHostRouteDestination();
        if (DBG)
            Log.d(TAG, "mDefaultOffHostRoute=0x" + String.format("%02X", mDefaultOffHostRoute));
        mOffHostRouteUicc = doGetOffHostUiccDestination();
        if (DBG) Log.d(TAG, "mOffHostRouteUicc=" + Arrays.toString(mOffHostRouteUicc));
        mOffHostRouteEse = doGetOffHostEseDestination();
        if (DBG) Log.d(TAG, "mOffHostRouteEse=" + Arrays.toString(mOffHostRouteEse));
        mAidMatchingSupport = doGetAidMatchingMode();
        if (DBG) Log.d(TAG, "mAidMatchingSupport=0x" + Integer.toHexString(mAidMatchingSupport));

        mDefaultIsoDepRoute = doGetDefaultIsoDepRouteDestination();

        mLastCommitStatus = true;

        if (DBG) Log.d(TAG, "mDefaultIsoDepRoute=0x" + String.format("%02X", mDefaultIsoDepRoute));
    }

    public boolean supportsAidPrefixRouting() {
        return mAidMatchingSupport == AID_MATCHING_EXACT_OR_PREFIX
                || mAidMatchingSupport == AID_MATCHING_PREFIX_ONLY
                || mAidMatchingSupport == AID_MATCHING_EXACT_OR_SUBSET_OR_PREFIX;
    }

    public boolean supportsAidSubsetRouting() {
        return mAidMatchingSupport == AID_MATCHING_EXACT_OR_SUBSET_OR_PREFIX;
    }

    public int calculateAidRouteSize(HashMap<String, AidEntry> routeCache) {
        // TAG + ROUTE + LENGTH_BYTE + POWER
        int AID_HDR_LENGTH = 0x04;
        int routeTableSize = 0x00;
        for (Map.Entry<String, AidEntry> aidEntry : routeCache.entrySet()) {
            String aid = aidEntry.getKey();
            // removing prefix length
            if (aid.endsWith("*")) {
                routeTableSize += ((aid.length() - 0x01) / 0x02) + AID_HDR_LENGTH;
            } else {
                routeTableSize += (aid.length() / 0x02) + AID_HDR_LENGTH;
            }
        }
        if (DBG) Log.d(TAG, "calculateAidRouteSize() - size for current route: " + routeTableSize);
        return routeTableSize;
    }

    public boolean isOnHostDefaultRoute() {
        if (DBG) Log.d(TAG, "isOnHostDefaultRoute()");
        return (mDefaultRoute == ROUTE_HOST);
    }

    void clearNfcRoutingTableLocked() {
        if (DBG)
            Log.d(TAG, "clearNfcRoutingTableLocked() -  Nb aids to clear: " + mRouteForAid.size());
        NfcService.getInstance().clearRouting();
        mRouteForAid.clear();
        mAidRoutingTable.clear();
    }

    private int getRouteForSecureElement(String se) {
        if (se == null || se.length() <= 3) {
            return 0;
        }
        try {
            if (se.startsWith("eSE") && mOffHostRouteEse != null) {
                int index = Integer.parseInt(se.substring(3));
                if (mOffHostRouteEse.length >= index && index > 0) {
                    return mOffHostRouteEse[index - 1] & 0xFF;
                }
            } else if (se.startsWith("SIM") && mOffHostRouteUicc != null) {
                int index = Integer.parseInt(se.substring(3));
                if (mOffHostRouteUicc.length >= index && index > 0) {
                    return mOffHostRouteUicc[index - 1] & 0xFF;
                }
            }
            if (mOffHostRouteEse == null && mOffHostRouteUicc == null) return mDefaultOffHostRoute;
        } catch (NumberFormatException e) {
        }
        return 0;
    }

    HashMap<String, AidEntry> computeRouteCache(
            HashMap<String, AidEntry> aidMap, int defaultRoute) {

        if (DBG)
            Log.d(
                    TAG,
                    "computeRouteCache() - Computing route cache for defaultRoute: "
                            + String.format("0x%02X", defaultRoute));

        HashMap<String, AidEntry> aidRoutingTableCache =
                new HashMap<String, AidEntry>(aidMap.size());

        aidRoutingTableCache.clear();

        if (mAidMatchingSupport == AID_MATCHING_PREFIX_ONLY) {
            /*
             * If a non-default route registers an exact AID which is shorter
             * than this exact AID, this will create a problem with controllers
             * that treat every AID in the routing table as a prefix. For
             * example, if App A registers F0000000041010 as an exact AID, and
             * App B registers F000000004 as an exact AID, and App B is not the
             * default route, the following would be added to the routing table:
             * F000000004 -> non-default destination However, because in this
             * mode, the controller treats every routing table entry as a
             * prefix, it means F0000000041010 would suddenly go to the
             * non-default destination too, whereas it should have gone to the
             * default.
             *
             * The only way to prevent this is to add the longer AIDs of the
             * default route at the top of the table, so they will be matched
             * first.
             */
            Set<String> defaultRouteAids = mAidRoutingTable.get(defaultRoute);
            if (defaultRouteAids != null) {
                for (String defaultRouteAid : defaultRouteAids) {
                    // Check whether there are any shorted AIDs routed to
                    // non-default
                    // TODO this is O(N^2) run-time complexity...
                    for (Map.Entry<String, Integer> aidEntry : mRouteForAid.entrySet()) {
                        String aid = aidEntry.getKey();
                        int route = aidEntry.getValue();
                        if (defaultRouteAid.startsWith(aid) && route != mDefaultRoute) {
                            if (DBG)
                                Log.d(
                                        TAG,
                                        "computeRouteCache() - Adding AID "
                                                + defaultRouteAid
                                                + " for default "
                                                + "route, because a conflicting shorter AID will be "
                                                + "added to the routing table");
                            aidRoutingTableCache.put(defaultRouteAid, aidMap.get(defaultRouteAid));
                        }
                    }
                }
            }
        }

        // Add AID entries for all non-default routes
        for (int i = 0; i < mAidRoutingTable.size(); i++) {
            int route = mAidRoutingTable.keyAt(i);

            if (DBG)
                Log.d(
                        TAG,
                        "computeRouteCache() - defaultRoute: "
                                + String.format("0x%02X", defaultRoute)
                                + ", route: "
                                + String.format("0x%02X", route));

            if (route != defaultRoute) {
                Set<String> aidsForRoute = mAidRoutingTable.get(route);
                for (String aid : aidsForRoute) {
                    if (aid.endsWith("*")) {
                        if (mAidMatchingSupport == AID_MATCHING_EXACT_ONLY) {
                            Log.e(
                                    TAG,
                                    "computeRouteCache() - This device does not support prefix AIDs.");
                        } else if (mAidMatchingSupport == AID_MATCHING_PREFIX_ONLY) {
                            // if (DBG) Log.d(TAG,
                            // "computeRouteCache() - Routing prefix AID " + aid
                            // + " to route "
                            // + String.format("0x%02X",route));
                            // Cut off '*' since controller anyway treats all
                            // AIDs as a prefix
                            aidRoutingTableCache.put(
                                    aid.substring(0, aid.length() - 1), aidMap.get(aid));
                        } else if (mAidMatchingSupport == AID_MATCHING_EXACT_OR_PREFIX
                                || mAidMatchingSupport == AID_MATCHING_EXACT_OR_SUBSET_OR_PREFIX) {
                            // if (DBG) Log.d(TAG,
                            // "computeRouteCache() - Routing prefix AID " + aid
                            // + " to route "
                            // + String.format("0x%02X",route));
                            aidRoutingTableCache.put(
                                    aid.substring(0, aid.length() - 1), aidMap.get(aid));
                        }
                    } else if (aid.endsWith("#")) {
                        if (mAidMatchingSupport == AID_MATCHING_EXACT_ONLY) {
                            Log.e(
                                    TAG,
                                    "computeRouteCache() - Device does not support subset AIDs but AID ["
                                            + aid
                                            + "] is registered");
                        } else if (mAidMatchingSupport == AID_MATCHING_PREFIX_ONLY
                                || mAidMatchingSupport == AID_MATCHING_EXACT_OR_PREFIX) {
                            Log.e(
                                    TAG,
                                    "computeRouteCache() - Device does not support subset AIDs but AID ["
                                            + aid
                                            + "] is registered");
                        } else if (mAidMatchingSupport == AID_MATCHING_EXACT_OR_SUBSET_OR_PREFIX) {
                            // if (DBG) Log.d(TAG,
                            // "computeRouteCache() - Routing subset AID " + aid
                            // + " to route "
                            // + String.format("0x%02X",route));
                            aidRoutingTableCache.put(
                                    aid.substring(0, aid.length() - 1), aidMap.get(aid));
                        }
                    } else {
                        // if (DBG) Log.d(TAG,
                        // "computeRouteCache() - Routing exact AID " + aid +
                        // " to route "
                        // + String.format("0x%02X",route));
                        aidRoutingTableCache.put(aid, aidMap.get(aid));
                    }
                }
            } else if (DBG) {
                Log.d(TAG, "computeRouteCache() - Same as default route, do nothing");
            }
        }

        if (DBG)
            Log.d(
                    TAG,
                    "computeRouteCache() - Nb of AIDs in RT for Route "
                            + String.format("0x%02X", defaultRoute)
                            + ": "
                            + aidRoutingTableCache.size());

        if (defaultRoute != mDefaultIsoDepRoute) {
            if (NfcService.getInstance().getNciVersion()
                    != NfcService.getInstance().NCI_VERSION_1_0) {
                String emptyAid = "";
                AidEntry entry = new AidEntry();
                entry.route = mDefaultRoute;
                if (defaultRoute == ROUTE_HOST) {
                    entry.isOnHost = true;
                } else {
                    entry.isOnHost = false;
                }
                entry.aidInfo = RegisteredAidCache.AID_ROUTE_QUAL_PREFIX;
                aidRoutingTableCache.put(emptyAid, entry);
                if (DBG) Log.d(TAG, "computeRouteCache() - Add emptyAid into AidRoutingTable");
            }
        }

        return aidRoutingTableCache;
    }

    public boolean configureRouting(HashMap<String, AidEntry> aidMap, boolean force) {

        // Reset overflow switch, if still needed, it will be computed here
        NfcService.getInstance().resetOverflowSwitchInformation();

        boolean aidRouteResolved = false;
        HashMap<String, AidEntry> aidRoutingTableCache =
                new HashMap<String, AidEntry>(aidMap.size());
        ArrayList<Integer> seList = new ArrayList<Integer>();

        // Retrieve last default route value, it might have been modified by
        // user
        mDefaultRoute = NfcService.getInstance().getDefaultAidRoute();
        // Initialize list with default route so that it is the first route
        // checked
        if (mDefaultRoute == 0xFF) {
            mDefaultRoute = doGetDefaultRouteDestination();
            mDefaultRoute = NfcService.getInstance().getConnectedNfceeId(mDefaultRoute);
        }

        mDefaultIsoDepRoute = NfcService.getInstance().getDefaultIsoDepRoute();
        // Initialize list with default route so that it is the first route
        // checked
        if (mDefaultIsoDepRoute == 0xFF) {
            mDefaultIsoDepRoute = doGetDefaultIsoDepRouteDestination();
            mDefaultIsoDepRoute = NfcService.getInstance().getConnectedNfceeId(mDefaultIsoDepRoute);
        }

        if (DBG)
            Log.d(
                    TAG,
                    "configureRouting() - Nb of AIDs in aidMap: "
                            + aidMap.size()
                            + ", mDefaultRoute = "
                            + String.format("0x%02X", mDefaultRoute));

        seList.add(mDefaultRoute);
        if (!seList.contains(ROUTE_HOST)) {
            seList.add(ROUTE_HOST);
        }
        // seList.add(ROUTE_HOST);

        SparseArray<Set<String>> aidRoutingTable = new SparseArray<Set<String>>(aidMap.size());
        HashMap<String, Integer> routeForAid = new HashMap<String, Integer>(aidMap.size());
        HashMap<String, Integer> infoForAid = new HashMap<String, Integer>(aidMap.size());
        // Then, populate internal data structures first
        DefaultAidRouteResolveCache defaultRouteCache = new DefaultAidRouteResolveCache();

        for (Map.Entry<String, AidEntry> aidEntry : aidMap.entrySet()) {
            int route = ROUTE_HOST;
            if (!aidEntry.getValue().isOnHost) {
                String offHostSE = aidEntry.getValue().offHostSE;
                if (offHostSE == null) {
                    route = mDefaultOffHostRoute;
                } else {
                    route = getRouteForSecureElement(offHostSE);
                    if (route == 0) {
                        Log.e(TAG, "configureRouting() - Invalid Off host Aid Entry " + offHostSE);
                        continue;
                    }
                }
            }
            if (!seList.contains(route)) {
                seList.add(route);
            }
            aidEntry.getValue().route = route;
            int aidType = aidEntry.getValue().aidInfo;
            String aid = aidEntry.getKey();

            if (DBG)
                Log.d(
                        TAG,
                        "configureRouting() - aid "
                                + aid
                                + " routed to "
                                + String.format("0x%02X", route));

            Set<String> entries = aidRoutingTable.get(route, new HashSet<String>());

            entries.add(aid);
            aidRoutingTable.put(route, entries);
            routeForAid.put(aid, route);
            infoForAid.put(aid, aidType);
        }

        if (DBG) {
            Log.d(
                    TAG,
                    "configureRouting() - Nb of different routes in routing table: "
                            + aidRoutingTable.size());
            for (int i = 0; i < aidRoutingTable.size(); i++) {
                int route = aidRoutingTable.keyAt(i);
                Set<String> obj = aidRoutingTable.get(route);
                Log.d(
                        TAG,
                        "configureRouting() - Route  "
                                + String.format("0x%02X", route)
                                + " has "
                                + obj.size()
                                + " AIDs");
            }
        }

        synchronized (mLock) {
            if (routeForAid.equals(mRouteForAid) && !force) {
                if (DBG) Log.d(TAG, "configureRouting() - Routing table unchanged, not updating");
                if (mLastCommitStatus == false) {
                    NfcService.getInstance().updateStatusOfServices(false);
                    NfcService.getInstance().notifyRoutingTableFull();
                } else {
                    /*
                     * If last commit status was success, And a new service is
                     * added whose AID's are already resolved by previously
                     * installed services, service state of newly installed app
                     * needs to be updated
                     */
                    NfcService.getInstance().updateStatusOfServices(true);
                }
                return false;
            }

            // Otherwise, update internal structures and commit new routing
            clearNfcRoutingTableLocked();
            mRouteForAid = routeForAid;
            mAidRoutingTable = aidRoutingTable;

            mMaxAidRoutingTableSize = NfcService.getInstance().getAidRoutingTableSize();
            if (DBG)
                Log.d(
                        TAG,
                        "configureRouting() - mMaxAidRoutingTableSize: " + mMaxAidRoutingTableSize);

            // calculate AidRoutingTableSize for existing route destination
            for (int index = 0; index < seList.size(); index++) {
                mDefaultRoute = seList.get(index);

                aidRoutingTableCache = computeRouteCache(aidMap, mDefaultRoute);

                if (calculateAidRouteSize(aidRoutingTableCache) <= mMaxAidRoutingTableSize) {
                    if (DBG)
                        Log.d(
                                TAG,
                                "configureRouting() - -----------> Found valid RT for route : "
                                        + String.format("0x%02X", mDefaultRoute));
                    aidRouteResolved = true;
                    break;
                }
            }

            // Compute routeCache for all routes
            // Needed for overflow handling
            {
                if (DBG)
                    Log.d(
                            TAG,
                            "configureRouting() - ---------> Route cache for overflow (B) <---------");

                int defaultRoute;
                HashMap<String, AidEntry> aidRTCache = new HashMap<String, AidEntry>(aidMap.size());

                for (int index = 0; index < seList.size(); index++) {
                    defaultRoute = seList.get(index);

                    aidRTCache = computeRouteCache(aidMap, defaultRoute);

                    defaultRouteCache.updateDefaultAidRouteCache(
                            aidRTCache,
                            defaultRoute == 0 ? 0 : (defaultRoute & 0x81) == 0x81 ? 0x02 : 0x01);
                }
                if (DBG)
                    Log.d(
                            TAG,
                            "configureRouting() - ---------> Route cache for overflow (E) <---------");
            }

            if (aidRouteResolved == true) {
                int gsmaRoute =
                        (mDefaultRoute == 0 ? 0 : ((mDefaultRoute & 0x81) == 0x81 ? 0x02 : 0x01));
                NfcService.getInstance()
                        .setRoutingTableSizeNotFull(
                                defaultRouteCache.getAidCacheForRoute(gsmaRoute));
                // Switch from DH/UICC to UICC/DH
                gsmaRoute = (gsmaRoute == 0 ? 0x02 : 0);
                NfcService.getInstance()
                        .setRoutingTableSizeNotFullAlt(
                                defaultRouteCache.getAidCacheForRoute(gsmaRoute));
                NfcService.getInstance().updateStatusOfServices(true);
                mLastCommitStatus = true;

                commit(aidRoutingTableCache);
            } else {
                StatsLog.write(
                        StatsLog.NFC_ERROR_OCCURRED,
                        StatsLog.NFC_ERROR_OCCURRED__TYPE__AID_OVERFLOW,
                        0,
                        0);
                Log.e(TAG, "RoutingTable unchanged because it's full, not updating");
                NfcService.getInstance()
                        .setRoutingTableSizeFull(0x00, defaultRouteCache.getAidCacheForRoute(0x00));
                NfcService.getInstance()
                        .setRoutingTableSizeFull(0x02, defaultRouteCache.getAidCacheForRoute(0x02));
                NfcService.getInstance().notifyRoutingTableFull();
                NfcService.getInstance().updateStatusOfServices(false);

                mLastCommitStatus = false;
            }
        }
        return true;
    }

    private void commit(HashMap<String, AidEntry> routeCache) {

        if (routeCache != null) {

            for (Map.Entry<String, AidEntry> aidEntry : routeCache.entrySet()) {
                int route = aidEntry.getValue().route;
                int aidType = aidEntry.getValue().aidInfo;
                String aid = aidEntry.getKey();

                if (DBG)
                    Log.d(
                            TAG,
                            "commit() -  aid: "
                                    + aid
                                    + ", route: "
                                    + String.format("0x%02X", route));

                NfcService.getInstance().routeAids(aid, route, aidType);
            }
        }

        // And finally commit the routing
        NfcService.getInstance().commitRouting();
    }

    /**
     * This notifies that the AID routing table in the controller has been cleared (usually due to
     * NFC being turned off).
     */
    public void onNfccRoutingTableCleared() {
        // The routing table in the controller was cleared
        // To stay in sync, clear our own tables.
        synchronized (mLock) {
            mAidRoutingTable.clear();
            mRouteForAid.clear();
        }
    }

    public boolean getLastCommitRoutingStatus() {

        if (DBG)
            Log.d(TAG, "getLastCommitRoutingStatus() - mLastCommitStatus: " + mLastCommitStatus);

        return mLastCommitStatus;
    }

    public void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw.println("Routing table:");
        pw.println("    Default route: " + ((mDefaultRoute == 0x00) ? "host" : "secure element"));
        synchronized (mLock) {
            for (int i = 0; i < mAidRoutingTable.size(); i++) {
                Set<String> aids = mAidRoutingTable.valueAt(i);
                pw.println(
                        "    Routed to 0x" + Integer.toHexString(mAidRoutingTable.keyAt(i)) + ":");
                for (String aid : aids) {
                    pw.println("        \"" + aid + "\"");
                }
            }
        }
    }

    final class DefaultAidRouteResolveCache {

        private HashMap<Integer, Integer> aidCacheSize;

        DefaultAidRouteResolveCache() {

            // if (DBG) Log.d(TAG, "DefaultAidRouteResolveCache - constructor");
            aidCacheSize = new HashMap<Integer, Integer>(0x03);
            aidCacheSize.put(0, 0);
            aidCacheSize.put(1, 0);
            aidCacheSize.put(2, 0);
        }

        public void updateDefaultAidRouteCache(HashMap<String, AidEntry> routeCache, int route) {

            int routesize = 0x00;
            HashMap<String, AidEntry> tempRouteCache =
                    new HashMap<String, AidEntry>(routeCache.size());
            tempRouteCache.putAll(routeCache);
            routesize = calculateAidRouteSize(tempRouteCache);

            if (DBG)
                Log.d(
                        TAG,
                        "DefaultAidRouteResolveCache - updateDefaultAidRouteCache() - Current route: "
                                + route
                                + ", size: "
                                + routesize);

            aidCacheSize.put(route, routesize);
            // Log.d(TAG, "DefaultAidRouteResolveCache -
            // updateDefaultAidRouteCache() - Temporary
            // Routing table size: " +routesize);
        }

        public int getAidCacheForRoute(int route) {
            int size;

            if (route == 0x00) {
                size = aidCacheSize.get(0x00);
            } else {
                size = aidCacheSize.get(0x02);
            }

            if (DBG)
                Log.d(
                        TAG,
                        "DefaultAidRouteResolveCache - getAidCacheForRoute() - route: "
                                + route
                                + ", size: "
                                + size);

            return size;
        }
    }
}
