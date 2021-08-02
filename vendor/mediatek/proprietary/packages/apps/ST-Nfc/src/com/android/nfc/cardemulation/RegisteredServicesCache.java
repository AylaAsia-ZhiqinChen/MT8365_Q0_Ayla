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

import android.app.ActivityManager;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.ResolveInfo;
import android.content.pm.ServiceInfo;
import android.net.Uri;
import android.nfc.cardemulation.CardEmulation;
import android.nfc.cardemulation.HostApduService;
import android.nfc.cardemulation.OffHostApduService;
import android.os.UserHandle;
import android.util.AtomicFile;
import android.util.Log;
import android.util.SparseArray;
import android.util.Xml;
import com.android.internal.util.FastXmlSerializer;
import com.android.nfc.NfcService;
import com.google.android.collect.Maps;
import com.st.android.nfc_extensions.StAidGroup;
import com.st.android.nfc_extensions.StApduServiceInfo;
import com.st.android.nfc_extensions.StConstants;
import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicReference;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlSerializer;

/**
 * This class is inspired by android.content.pm.RegisteredServicesCache That class was not re-used
 * because it doesn't support dynamically registering additional properties, but generates
 * everything from the manifest. Since we have some properties that are not in the manifest, it's
 * less suited.
 */
public class RegisteredServicesCache {
    static final String XML_INDENT_OUTPUT_FEATURE =
            "http://xmlpull.org/v1/doc/features.html#indent-output";
    static final String TAG = "HCENfc_RegisteredServicesCache";
    static final boolean DBG = true;
    static final boolean DBG2 =
            DBG && android.os.SystemProperties.get("persist.st_nfc_debug").equals("1");
    static final String SERVICE_STATE_FILE_VERSION = "1.0";

    final Context mContext;
    final AtomicReference<BroadcastReceiver> mReceiver;

    final Object mLock = new Object();
    // All variables below synchronized on mLock

    // mUserServices holds the card emulation services that are running for each user
    final SparseArray<UserServices> mUserServices = new SparseArray<UserServices>();
    final Callback mCallback;
    final AtomicFile mDynamicSettingsFile;
    final AtomicFile mServiceStateFile;
    /*Installed service will be used to load all the registered services available in the device
     *key   : UID corresponding to the service - owner of the service
     *value : Hashmap of service component and corresponding state
     * */
    HashMap<String, HashMap<ComponentName, Integer>> installedServices = new HashMap<>();

    String mLastAddedPackage = null;

    public interface Callback {
        void onServicesUpdated(int userId, final List<StApduServiceInfo> services);
    };

    static class DynamicSettings {
        public final int uid;
        public final HashMap<String, StAidGroup> aidGroups = Maps.newHashMap();
        public String offHostSE;

        DynamicSettings(int uid) {
            this.uid = uid;
        }
    };

    private static class UserServices {
        /** All services that have registered */
        final HashMap<ComponentName, StApduServiceInfo> services =
                Maps.newHashMap(); // Re-built at run-time

        final HashMap<ComponentName, DynamicSettings> dynamicSettings =
                Maps.newHashMap(); // In memory cache of dynamic settings
    };

    private UserServices findOrCreateUserLocked(int userId) {

        if (DBG) Log.d(TAG, "findOrCreateUserLocked()");

        UserServices services = mUserServices.get(userId);
        if (services == null) {
            services = new UserServices();
            mUserServices.put(userId, services);
        }
        return services;
    }

    public RegisteredServicesCache(Context context, Callback callback) {

        if (DBG) Log.d(TAG, "constructor");

        mContext = context;
        mCallback = callback;

        final BroadcastReceiver receiver =
                new BroadcastReceiver() {
                    @Override
                    public void onReceive(Context context, Intent intent) {
                        final int uid = intent.getIntExtra(Intent.EXTRA_UID, -1);
                        String action = intent.getAction();
                        if (DBG) Log.d(TAG, "onReceive() - Intent action: " + action);
                        if (uid != -1) {
                            Uri uri = intent.getData();
                            String pkg = uri != null ? uri.getSchemeSpecificPart() : null;

                            if (DBG) Log.d(TAG, "onReceive() - pkg: " + pkg);

                            if (Intent.ACTION_PACKAGE_ADDED.equals(action)) {
                                mLastAddedPackage = pkg;
                            }

                            boolean replaced =
                                    intent.getBooleanExtra(Intent.EXTRA_REPLACING, false)
                                            && (Intent.ACTION_PACKAGE_ADDED.equals(action)
                                                    || Intent.ACTION_PACKAGE_REMOVED.equals(
                                                            action));
                            if (!replaced) {
                                int currentUser = ActivityManager.getCurrentUser();
                                if (currentUser == UserHandle.getUserId(uid)) {
                                    invalidateCache(UserHandle.getUserId(uid));
                                } else {
                                    // Cache will automatically be updated on user switch
                                }
                            } else {
                                if (DBG)
                                    Log.d(
                                            TAG,
                                            "Ignoring package intent due to package being replaced.");
                            }
                        }
                    }
                };
        mReceiver = new AtomicReference<BroadcastReceiver>(receiver);

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(Intent.ACTION_PACKAGE_ADDED);
        intentFilter.addAction(Intent.ACTION_PACKAGE_CHANGED);
        intentFilter.addAction(Intent.ACTION_PACKAGE_REMOVED);
        intentFilter.addAction(Intent.ACTION_PACKAGE_REPLACED);
        intentFilter.addAction(Intent.ACTION_PACKAGE_FIRST_LAUNCH);
        intentFilter.addAction(Intent.ACTION_PACKAGE_RESTARTED);
        intentFilter.addDataScheme("package");
        mContext.registerReceiverAsUser(mReceiver.get(), UserHandle.ALL, intentFilter, null, null);

        // Register for events related to sdcard operations
        IntentFilter sdFilter = new IntentFilter();
        sdFilter.addAction(Intent.ACTION_EXTERNAL_APPLICATIONS_AVAILABLE);
        sdFilter.addAction(Intent.ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
        mContext.registerReceiverAsUser(mReceiver.get(), UserHandle.ALL, sdFilter, null, null);

        File dataDir = mContext.getFilesDir();
        mDynamicSettingsFile = new AtomicFile(new File(dataDir, "dynamic_aids.xml"));
        mServiceStateFile = new AtomicFile(new File(dataDir, "service_state.xml"));
    }

    void initialize() {

        if (DBG) Log.d(TAG, "initialize()");

        synchronized (mLock) {
            readDynamicSettingsLocked();
        }
        invalidateCache(ActivityManager.getCurrentUser());
    }

    void dump(ArrayList<StApduServiceInfo> services) {
        for (StApduServiceInfo service : services) {
            if (DBG2) Log.d(TAG, service.toString());
        }
    }

    boolean containsStServiceLocked(
            ArrayList<StApduServiceInfo> services, ComponentName serviceName) {

        if (DBG) Log.d(TAG, "containsStServiceLocked()");

        for (StApduServiceInfo service : services) {
            if (service.getComponent().equals(serviceName)) return true;
        }
        return false;
    }

    public boolean hasService(int userId, ComponentName service) {

        if (DBG) Log.d(TAG, "hasService()");

        return getStService(userId, service) != null;
    }

    public StApduServiceInfo getStService(int userId, ComponentName service) {

        if (DBG) Log.d(TAG, "getStService()");

        synchronized (mLock) {
            UserServices userServices = findOrCreateUserLocked(userId);
            return userServices.services.get(service);
        }
    }

    public List<StApduServiceInfo> getStServices(int userId) {

        if (DBG) Log.d(TAG, "getStServices()");

        final ArrayList<StApduServiceInfo> services = new ArrayList<StApduServiceInfo>();
        synchronized (mLock) {
            UserServices userServices = findOrCreateUserLocked(userId);
            services.addAll(userServices.services.values());
        }
        return services;
    }

    public List<StApduServiceInfo> getStServicesForCategory(int userId, String category) {

        final ArrayList<StApduServiceInfo> services = new ArrayList<StApduServiceInfo>();
        synchronized (mLock) {
            UserServices userServices = findOrCreateUserLocked(userId);
            for (StApduServiceInfo service : userServices.services.values()) {
                if (service.hasCategory(category)
                        && (service.getAidCacheSizeForCategory(category) > 0))
                    services.add(service);
            }
        }

        if (DBG) Log.d(TAG, "getStServicesForCategory() - found " + services.size() + " services");

        return services;
    }

    ArrayList<StApduServiceInfo> getInstalledStServices(int userId) {

        if (DBG) Log.d(TAG, "getInstalledServices()");

        PackageManager pm;
        try {
            pm =
                    mContext.createPackageContextAsUser("android", 0, new UserHandle(userId))
                            .getPackageManager();
        } catch (NameNotFoundException e) {
            Log.e(TAG, "Could not create user package context");
            return null;
        }

        ArrayList<StApduServiceInfo> validServices = new ArrayList<StApduServiceInfo>();

        List<ResolveInfo> resolvedServices =
                new ArrayList<>(
                        pm.queryIntentServicesAsUser(
                                new Intent(HostApduService.SERVICE_INTERFACE),
                                PackageManager.GET_META_DATA,
                                userId));

        List<ResolveInfo> resolvedOffHostServices =
                pm.queryIntentServicesAsUser(
                        new Intent(OffHostApduService.SERVICE_INTERFACE),
                        PackageManager.GET_META_DATA,
                        userId);
        resolvedServices.addAll(resolvedOffHostServices);

        if (DBG)
            Log.d(
                    TAG,
                    "getInstalledStServices() - found "
                            + resolvedServices.size()
                            + " resolvedServices");

        for (ResolveInfo resolvedService : resolvedServices) {
            try {
                boolean onHost = !resolvedOffHostServices.contains(resolvedService);
                ServiceInfo si = resolvedService.serviceInfo;
                ComponentName componentName = new ComponentName(si.packageName, si.name);

                if (DBG)
                    Log.d(
                            TAG,
                            "getInstalledStServices() - componentName:  "
                                    + componentName.flattenToString());

                // Check if the package holds the NFC permission
                if (pm.checkPermission(android.Manifest.permission.NFC, si.packageName)
                        != PackageManager.PERMISSION_GRANTED) {
                    Log.e(
                            TAG,
                            "getInstalledStServices() - Skipping application component "
                                    + componentName
                                    + ": it must request the permission "
                                    + android.Manifest.permission.NFC);
                    continue;
                }
                if (!android.Manifest.permission.BIND_NFC_SERVICE.equals(si.permission)) {
                    Log.e(
                            TAG,
                            "getInstalledStServices() - Skipping APDU service "
                                    + componentName
                                    + ": it does not require the permission "
                                    + android.Manifest.permission.BIND_NFC_SERVICE);
                    continue;
                }
                StApduServiceInfo service = new StApduServiceInfo(pm, resolvedService, onHost);
                if (service != null) {
                    validServices.add(service);
                }
            } catch (XmlPullParserException e) {
                Log.w(
                        TAG,
                        "getInstalledStServices() - Unable to load component info "
                                + resolvedService.toString(),
                        e);
            } catch (IOException e) {
                Log.w(
                        TAG,
                        "getInstalledStServices() - Unable to load component info "
                                + resolvedService.toString(),
                        e);
            }
        }

        return validServices;
    }

    public void invalidateCache(int userId) {

        if (DBG) Log.d(TAG, "invalidateCache()");

        final ArrayList<StApduServiceInfo> validServices = getInstalledStServices(userId);
        if (validServices == null) {
            return;
        }
        synchronized (mLock) {
            UserServices userServices = findOrCreateUserLocked(userId);

            if (DBG) Log.d(TAG, "invalidateCache() - mLastAddedPackage: " + mLastAddedPackage);

            // Find removed services
            Iterator<Map.Entry<ComponentName, StApduServiceInfo>> it =
                    userServices.services.entrySet().iterator();
            while (it.hasNext()) {
                Map.Entry<ComponentName, StApduServiceInfo> entry =
                        (Map.Entry<ComponentName, StApduServiceInfo>) it.next();

                if (!containsStServiceLocked(validServices, entry.getKey())) {
                    Log.d(TAG, "invalidateCache() - Service removed: " + entry.getKey());
                    it.remove();
                }
            }

            /** *************************** */
            /* Fill services entry */
            /** *************************** */
            for (StApduServiceInfo service : validServices) {
                if (DBG2)
                    Log.d(
                            TAG,
                            "invalidateCache() - Adding service: "
                                    + service.getComponent()
                                    + " AIDs: "
                                    + service.getAids());

                // Save description of last added package in case of overflow
                ComponentName name = service.getComponent();
                if (mLastAddedPackage != null) {
                    if (mLastAddedPackage.equals(name.getPackageName())) {
                        String description = service.getGsmaDescription();
                        NfcService.getInstance().setLastModifiedService(description);
                    }
                }

                userServices.services.put(service.getComponent(), service);
            }

            if (mLastAddedPackage != null) {
                mLastAddedPackage = null;
            }

            /** *************************** */
            // Apply dynamic AID mappings
            /** *************************** */
            ArrayList<ComponentName> toBeRemoved = new ArrayList<ComponentName>();
            for (Map.Entry<ComponentName, DynamicSettings> entry :
                    userServices.dynamicSettings.entrySet()) {
                // Verify component / uid match
                ComponentName component = entry.getKey();
                DynamicSettings dynamicSettings = entry.getValue();
                StApduServiceInfo serviceInfo = userServices.services.get(component);
                if (serviceInfo == null || (serviceInfo.getUid() != dynamicSettings.uid)) {
                    toBeRemoved.add(component);
                    continue;
                } else {
                    for (StAidGroup group : dynamicSettings.aidGroups.values()) {
                        serviceInfo.setOrReplaceDynamicStAidGroup(group);
                    }
                    if (dynamicSettings.offHostSE != null) {
                        serviceInfo.setOffHostSecureElement(dynamicSettings.offHostSE);
                    }
                }
            }
            if (toBeRemoved.size() > 0) {
                for (ComponentName component : toBeRemoved) {
                    Log.d(
                            TAG,
                            "invalidateCache() - Removing dynamic AIDs registered by " + component);
                    userServices.dynamicSettings.remove(component);
                }
                // Persist to filesystem
                writeDynamicSettingsLocked();
            }

            /** *************************** */
            /* Update service states */
            /** *************************** */
            updateServiceStateFromFile(userId);
            Log.d(
                    TAG,
                    "invalidateCache() - 1 "
                            + Thread.currentThread().getStackTrace()[2].getMethodName()
                            + ": WriteServiceStateToFile");
            writeServiceStateToFile(userId);
        }

        mCallback.onServicesUpdated(userId, Collections.unmodifiableList(validServices));
        dump(validServices);
    }

    private void readDynamicSettingsLocked() {

        if (DBG) Log.d(TAG, "readDynamicSettingsLocked()");

        FileInputStream fis = null;
        try {
            if (!mDynamicSettingsFile.getBaseFile().exists()) {
                Log.d(TAG, "readDynamicSettingsLocked() - Dynamic AIDs file does not exist.");
                return;
            }
            fis = mDynamicSettingsFile.openRead();
            XmlPullParser parser = Xml.newPullParser();
            parser.setInput(fis, null);
            int eventType = parser.getEventType();
            while (eventType != XmlPullParser.START_TAG
                    && eventType != XmlPullParser.END_DOCUMENT) {
                eventType = parser.next();
            }
            String tagName = parser.getName();
            if ("services".equals(tagName)) {
                boolean inService = false;
                ComponentName currentComponent = null;
                int currentUid = -1;
                String currentOffHostSE = null;
                ArrayList<StAidGroup> currentGroups = new ArrayList<StAidGroup>();
                while (eventType != XmlPullParser.END_DOCUMENT) {
                    tagName = parser.getName();
                    if (eventType == XmlPullParser.START_TAG) {
                        if ("service".equals(tagName) && parser.getDepth() == 2) {
                            String compString = parser.getAttributeValue(null, "component");
                            String uidString = parser.getAttributeValue(null, "uid");
                            String offHostString = parser.getAttributeValue(null, "offHostSE");
                            if (compString == null || uidString == null) {
                                Log.e(TAG, "Invalid service attributes");
                            } else {
                                try {
                                    currentUid = Integer.parseInt(uidString);
                                    currentComponent =
                                            ComponentName.unflattenFromString(compString);
                                    currentOffHostSE = offHostString;
                                    inService = true;
                                } catch (NumberFormatException e) {
                                    Log.e(
                                            TAG,
                                            "readDynamicSettingsLocked() - Could not parse service "
                                                    + "uid");
                                }
                            }
                        }
                        if ("aid-group".equals(tagName) && parser.getDepth() == 3 && inService) {
                            StAidGroup group = StAidGroup.createFromXml(parser);
                            if (group != null) {
                                currentGroups.add(group);
                            } else {
                                Log.e(
                                        TAG,
                                        "readDynamicSettingsLocked() - Could not parse AID group.");
                            }
                        }
                    } else if (eventType == XmlPullParser.END_TAG) {
                        if ("service".equals(tagName)) {
                            // See if we have a valid service
                            if (currentComponent != null
                                    && currentUid >= 0
                                    && (currentGroups.size() > 0 || currentOffHostSE != null)) {
                                final int userId = UserHandle.getUserId(currentUid);
                                DynamicSettings dynSettings = new DynamicSettings(currentUid);
                                for (StAidGroup group : currentGroups) {
                                    dynSettings.aidGroups.put(group.getCategory(), group);
                                }
                                dynSettings.offHostSE = currentOffHostSE;
                                UserServices services = findOrCreateUserLocked(userId);
                                services.dynamicSettings.put(currentComponent, dynSettings);
                            }
                            currentUid = -1;
                            currentComponent = null;
                            currentGroups.clear();
                            inService = false;
                            currentOffHostSE = null;
                        }
                    }
                    eventType = parser.next();
                }
                ;
            }
        } catch (Exception e) {
            Log.e(
                    TAG,
                    "readDynamicSettingsLocked() - Could not parse dynamic AIDs file, trashing.");
            mDynamicSettingsFile.delete();
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                } catch (IOException e) {
                }
            }
        }
    }

    private boolean writeDynamicSettingsLocked() {

        if (DBG) Log.d(TAG, "writeDynamicSettingsLocked()");

        FileOutputStream fos = null;
        try {
            fos = mDynamicSettingsFile.startWrite();
            XmlSerializer out = new FastXmlSerializer();
            out.setOutput(fos, "utf-8");
            out.startDocument(null, true);
            out.setFeature(XML_INDENT_OUTPUT_FEATURE, true);
            out.startTag(null, "services");
            for (int i = 0; i < mUserServices.size(); i++) {
                final UserServices user = mUserServices.valueAt(i);
                for (Map.Entry<ComponentName, DynamicSettings> service :
                        user.dynamicSettings.entrySet()) {
                    out.startTag(null, "service");
                    out.attribute(null, "component", service.getKey().flattenToString());
                    out.attribute(null, "uid", Integer.toString(service.getValue().uid));
                    if (service.getValue().offHostSE != null) {
                        out.attribute(null, "offHostSE", service.getValue().offHostSE);
                    }
                    for (StAidGroup group : service.getValue().aidGroups.values()) {
                        group.writeAsXml(out);
                    }
                    out.endTag(null, "service");
                }
            }
            out.endTag(null, "services");
            out.endDocument();
            mDynamicSettingsFile.finishWrite(fos);
            return true;
        } catch (Exception e) {
            Log.e(TAG, "writeDynamicSettingsLocked() - Error writing dynamic AIDs", e);
            if (fos != null) {
                mDynamicSettingsFile.failWrite(fos);
            }
            return false;
        }
    }

    public boolean setOffHostSecureElement(
            int userId, int uid, ComponentName componentName, String offHostSE) {

        if (DBG)
            Log.d(
                    TAG,
                    "setOffHostSecureElement() - componentName: "
                            + componentName.flattenToString()
                            + " offHostSE: "
                            + offHostSE);

        ArrayList<StApduServiceInfo> newServices = null;
        synchronized (mLock) {
            UserServices services = findOrCreateUserLocked(userId);
            // Check if we can find this service
            StApduServiceInfo serviceInfo = getStService(userId, componentName);
            if (serviceInfo == null) {
                Log.e(
                        TAG,
                        "setOffHostSecureElement() - Service "
                                + componentName
                                + " does not exist.");
                return false;
            }
            if (serviceInfo.getUid() != uid) {
                // This is probably a good indication something is wrong here.
                // Either newer service installed with different uid (but then
                // we should have known about it), or somebody calling us from
                // a different uid.
                Log.e(TAG, "setOffHostSecureElement() - UID mismatch.");
                return false;
            }
            if (offHostSE == null || serviceInfo.isOnHost()) {
                Log.e(TAG, "setOffHostSecureElement() - OffHostSE mismatch with Service type");
                return false;
            }

            DynamicSettings dynSettings = services.dynamicSettings.get(componentName);
            if (dynSettings == null) {
                dynSettings = new DynamicSettings(uid);
            }
            dynSettings.offHostSE = offHostSE;

            services.dynamicSettings.put(componentName, dynSettings);

            boolean success = writeDynamicSettingsLocked();
            if (!success) {
                Log.e(TAG, "setOffHostSecureElement() - Failed to persist AID group.");
                dynSettings.offHostSE = null;
                return false;
            }

            serviceInfo.setOffHostSecureElement(offHostSE);
            newServices = new ArrayList<StApduServiceInfo>(services.services.values());
        }
        // Make callback without the lock held
        mCallback.onServicesUpdated(userId, newServices);
        return true;
    }

    public boolean unsetOffHostSecureElement(int userId, int uid, ComponentName componentName) {
        ArrayList<StApduServiceInfo> newServices = null;
        synchronized (mLock) {
            UserServices services = findOrCreateUserLocked(userId);
            // Check if we can find this service
            StApduServiceInfo serviceInfo = getStService(userId, componentName);
            if (serviceInfo == null) {
                Log.e(
                        TAG,
                        "unsetOffHostSecureElement() - Service "
                                + componentName
                                + " does not exist.");
                return false;
            }
            if (serviceInfo.getUid() != uid) {
                // This is probably a good indication something is wrong here.
                // Either newer service installed with different uid (but then
                // we should have known about it), or somebody calling us from
                // a different uid.
                Log.e(TAG, "unsetOffHostSecureElement() - UID mismatch.");
                return false;
            }
            if (serviceInfo.isOnHost() || serviceInfo.getOffHostSecureElement() == null) {
                Log.e(TAG, "unsetOffHostSecureElement() - OffHostSE is not set");
                return false;
            }

            DynamicSettings dynSettings = services.dynamicSettings.get(componentName);
            String offHostSE = dynSettings.offHostSE;
            dynSettings.offHostSE = null;
            boolean success = writeDynamicSettingsLocked();
            if (!success) {
                Log.e(TAG, "unsetOffHostSecureElement() - Failed to persist AID group.");
                dynSettings.offHostSE = offHostSE;
                return false;
            }

            serviceInfo.unsetOffHostSecureElement();
            newServices = new ArrayList<StApduServiceInfo>(services.services.values());
        }
        // Make callback without the lock held
        mCallback.onServicesUpdated(userId, newServices);
        return true;
    }

    private void updateServiceStateFromFile(int currUserId) {

        if (DBG) Log.d(TAG, "updateServiceStateFromFile()");

        FileInputStream fis = null;
        try {
            if (!mServiceStateFile.getBaseFile().exists()) {
                Log.d(TAG, "updateServiceStateFromFile()() - mServiceStateFile does not exist");
                return;
            }
            fis = mServiceStateFile.openRead();
            XmlPullParser parser = Xml.newPullParser();
            parser.setInput(fis, null);
            int eventType = parser.getEventType();
            int currUid = -1;
            ComponentName currComponent = null;
            int state = StConstants.SERVICE_STATE_ENABLED;

            while (eventType != XmlPullParser.START_TAG
                    && eventType != XmlPullParser.END_DOCUMENT) {
                eventType = parser.next();
            }
            String tagName = parser.getName();
            String fileVersion = "null";
            /**
             * Get the version of the Service state file. if the version is 1.0, service states are
             * stored as integers(0,1,2,3) or else service states are stored as boolean (true or
             * false)
             */
            if ("Version".equals(tagName)) {
                fileVersion = parser.getAttributeValue(null, "FileVersion");
                Log.d(
                        TAG,
                        "updateServiceStateFromFile() - ServiceStateFileVersion = " + fileVersion);
                eventType = parser.next();
                while (eventType != XmlPullParser.START_TAG
                        && eventType != XmlPullParser.END_DOCUMENT) {
                    eventType = parser.next();
                }
                tagName = parser.getName();
                Log.d(TAG, "Next Tag=" + tagName);
            }
            if ("services".equals(tagName)) {
                while (eventType != XmlPullParser.END_DOCUMENT) {
                    tagName = parser.getName();
                    if (eventType == XmlPullParser.START_TAG) {
                        if ("service".equals(tagName) && parser.getDepth() == 0x02) {
                            String compString = parser.getAttributeValue(null, "component");
                            String uidString = parser.getAttributeValue(null, "uid");
                            String stateString = parser.getAttributeValue(null, "serviceState");

                            if (compString == null || uidString == null || stateString == null) {
                                Log.e(
                                        TAG,
                                        "updateServiceStateFromFile() - Invalid service "
                                                + "attributes");
                            } else {
                                try {
                                    currUid = Integer.parseInt(uidString);
                                    currComponent = ComponentName.unflattenFromString(compString);

                                    if (fileVersion.equals("null")) {
                                        if (stateString.equalsIgnoreCase("false"))
                                            state = StConstants.SERVICE_STATE_DISABLED;
                                        else state = StConstants.SERVICE_STATE_ENABLED;
                                    } else if (fileVersion.equals("1.0")) {
                                        state = Integer.parseInt(stateString);
                                        if (state < StConstants.SERVICE_STATE_DISABLED
                                                || state > StConstants.SERVICE_STATE_DISABLING)
                                            Log.e(
                                                    TAG,
                                                    "updateServiceStateFromFile() - Invalid "
                                                            + "Service state");
                                    }
                                    /*Load all the servies info into local memory from xml file and
                                     *later update the xml file with updated information
                                     *This way it can retain previous user's information even
                                     * after switching to different user
                                     * */
                                    if (installedServices.containsKey(uidString)) {
                                        // Log.e(TAG, "updateServiceStateFromFile() -
                                        // installedServices contains uidString: " +uidString);
                                        HashMap<ComponentName, Integer> componentStates;
                                        componentStates = installedServices.get(uidString);
                                        componentStates.put(currComponent, state);
                                    } else {
                                        // Log.e(TAG, "updateServiceStateFromFile() -
                                        // installedServices no uidString ");
                                        HashMap<ComponentName, Integer> componentStates =
                                                new HashMap<>();
                                        componentStates.put(currComponent, state);
                                        installedServices.put(uidString, componentStates);
                                    }

                                } catch (NumberFormatException e) {
                                    Log.e(
                                            TAG,
                                            "updateServiceStateFromFile() - could not parse the "
                                                    + "service attributes");
                                }
                            }
                        }
                    } else if (eventType == XmlPullParser.END_TAG) {
                        if ("service".equals(tagName)) {
                            final int userId = UserHandle.getUserId(currUid);

                            UserServices serviceCache = findOrCreateUserLocked(userId);
                            StApduServiceInfo serviceInfo =
                                    serviceCache.services.get(currComponent);

                            if (serviceInfo == null) {
                            } else serviceInfo.setServiceState(CardEmulation.CATEGORY_OTHER, state);
                        }
                        currUid = -1;
                        currComponent = null;
                        state = StConstants.SERVICE_STATE_ENABLED;
                    }

                    eventType = parser.next();
                }
            }
        } catch (Exception e) {
            mServiceStateFile.delete();
            Log.e(
                    TAG,
                    "updateServiceStateFromFile() - could not parse the seriveState file , "
                            + "thrashing the file "
                            + e);
        } finally {
            try {
                if (fis != null) {
                    fis.close();
                }
            } catch (Exception e) {
            }
        }
    }

    private boolean writeServiceStateToFile(int currUserId) {
        FileOutputStream fos = null;

        if (DBG) Log.d(TAG, "writeServiceStateToFile()");

        if (currUserId != ActivityManager.getCurrentUser()) {
            return false;
        }
        int state = StConstants.SERVICE_STATE_ENABLED;
        try {
            fos = mServiceStateFile.startWrite();
            XmlSerializer out = new FastXmlSerializer();
            out.setOutput(fos, "utf-8");
            out.startDocument(null, true);
            out.setFeature(XML_INDENT_OUTPUT_FEATURE, true);
            out.startTag(null, "Version");
            out.attribute(null, "FileVersion", SERVICE_STATE_FILE_VERSION);
            out.endTag(null, "Version");
            out.startTag(null, "services");
            for (int userId = 0; userId < mUserServices.size(); userId++) {
                final UserServices userServices = mUserServices.valueAt(userId);

                /* Store service states for AID-based services */
                for (StApduServiceInfo serviceInfo : userServices.services.values()) {
                    if (!serviceInfo.hasCategory(CardEmulation.CATEGORY_OTHER)) {
                        continue;
                    }
                    out.startTag(null, "service");
                    out.attribute(null, "component", serviceInfo.getComponent().flattenToString());
                    Log.d(
                            TAG,
                            "writeServiceStateToFile() - component name "
                                    + serviceInfo.getComponent().flattenToString());
                    out.attribute(null, "uid", Integer.toString(serviceInfo.getUid()));

                    boolean isServiceInstalled = false;
                    if (installedServices.containsKey(Integer.toString(serviceInfo.getUid()))) {
                        HashMap<ComponentName, Integer> componentStates =
                                installedServices.get(Integer.toString(serviceInfo.getUid()));
                        if (componentStates.containsKey(serviceInfo.getComponent())) {
                            state = componentStates.get(serviceInfo.getComponent());
                            componentStates.remove(serviceInfo.getComponent());
                            if (componentStates.isEmpty()) {
                                installedServices.remove(Integer.toString(serviceInfo.getUid()));
                            }
                            isServiceInstalled = true;
                        }
                    }
                    if (!isServiceInstalled) {
                        state = serviceInfo.getServiceState(CardEmulation.CATEGORY_OTHER);
                    }
                    out.attribute(null, "serviceState", Integer.toString(state));
                    out.endTag(null, "service");
                }
            }

            out.endTag(null, "services");
            out.endDocument();
            mServiceStateFile.finishWrite(fos);
            return true;
        } catch (Exception e) {
            Log.e(TAG, "writeServiceStateToFile() - Failed to write serviceStateFile xml");
            e.printStackTrace();
            if (fos != null) {
                mServiceStateFile.failWrite(fos);
            }
            return false;
        }
    }

    public int updateServiceState(int userId, int uid, Map<String, Boolean> serviceState) {

        if (DBG) Log.d(TAG, "updateServiceState()");

        boolean success = false;

        NfcService.getInstance().resetAidRoutingTableFull();

        synchronized (mLock) {
            Iterator<Map.Entry<String, Boolean>> it = serviceState.entrySet().iterator();
            while (it.hasNext()) {
                Map.Entry<String, Boolean> entry = (Map.Entry<String, Boolean>) it.next();
                ComponentName componentName = ComponentName.unflattenFromString(entry.getKey());
                StApduServiceInfo serviceInfo = getStService(userId, componentName);
                Log.d(TAG, "updateServiceState() - key: " + entry.getKey());
                Log.d(TAG, "updateServiceState() - value:  " + entry.getValue());
                if (serviceInfo != null) {
                    serviceInfo.enableService(CardEmulation.CATEGORY_OTHER, entry.getValue());
                } else {
                    Log.e(TAG, "updateServiceState() - Could not find service " + componentName);
                    return 0xFF;
                }
            }
            Log.d(
                    TAG,
                    "2"
                            + Thread.currentThread().getStackTrace()[2].getMethodName()
                            + ":WriteServiceStateToFile");
            success = writeServiceStateToFile(userId);
        }
        invalidateCache(ActivityManager.getCurrentUser());
        return (success ? 0x00 : 0xFF);
    }

    public boolean registerAidGroupForService(
            int userId, int uid, ComponentName componentName, StAidGroup stAidGroup) {

        if (DBG)
            Log.d(
                    TAG,
                    "registerAidGroupForService() - componentName: "
                            + componentName.flattenToString());

        ArrayList<StApduServiceInfo> newServices = null;
        boolean success;
        synchronized (mLock) {
            UserServices services = findOrCreateUserLocked(userId);
            // Check if we can find this service
            StApduServiceInfo serviceInfo = getStService(userId, componentName);
            if (serviceInfo == null) {
                Log.e(
                        TAG,
                        "registerAidGroupForService() - Service "
                                + componentName
                                + " does not exist.");
                return false;
            }
            if (serviceInfo.getUid() != uid) {
                // This is probably a good indication something is wrong here.
                // Either newer service installed with different uid (but then
                // we should have known about it), or somebody calling us from
                // a different uid.
                Log.e(TAG, "registerAidGroupForService() - UID mismatch.");
                return false;
            }
            // Do another AID validation, since a caller could have thrown in a
            // modified AidGroup object with invalid AIDs over Binder.
            List<String> aids = stAidGroup.getAids();
            if (DBG) Log.d(TAG, "registerAidGroupForService() - nb of aids: " + aids.size());
            for (String aid : aids) {
                if (!CardEmulation.isValidAid(aid)) {
                    Log.e(TAG, "registerAidGroupForService() - AID " + aid + " is not a valid AID");
                    return false;
                }
            }
            serviceInfo.setOrReplaceDynamicStAidGroup(stAidGroup);
            DynamicSettings dynSettings = services.dynamicSettings.get(componentName);
            if (dynSettings == null) {
                dynSettings = new DynamicSettings(uid);
                dynSettings.offHostSE = null;
                services.dynamicSettings.put(componentName, dynSettings);
            }
            dynSettings.aidGroups.put(stAidGroup.getCategory(), stAidGroup);
            success = writeDynamicSettingsLocked();
            if (success) {
                newServices = new ArrayList<StApduServiceInfo>(services.services.values());
            } else {
                Log.e(TAG, "registerAidGroupForService() - Failed to persist AID group.");
                // Undo registration
                dynSettings.aidGroups.remove(stAidGroup.getCategory());
            }
        }
        if (success) {
            // By default, at creation, new service should be enabled
            int idx = 0;
            for (StApduServiceInfo adpuServiceInfo : newServices) {
                if (adpuServiceInfo.getComponent().equals(componentName)) {

                    if (adpuServiceInfo.hasCategory(CardEmulation.CATEGORY_OTHER) == true) {
                        adpuServiceInfo.setServiceState(
                                CardEmulation.CATEGORY_OTHER, StConstants.SERVICE_STATE_ENABLED);
                    }

                    newServices.set(idx, adpuServiceInfo);
                    break;
                }
                idx++;
            }
            // Make callback without the lock held
            mCallback.onServicesUpdated(userId, newServices);
        }

        // need to return commit status to framework API
        if (NfcService.getInstance().getLastCommitRoutingStatus() == false) {
            return false;
        } else {
            return success;
        }
    }

    public StAidGroup getAidGroupForService(
            int userId, int uid, ComponentName componentName, String category) {

        if (DBG) Log.d(TAG, "getAidGroupForService()");

        StApduServiceInfo serviceInfo = getStService(userId, componentName);
        if (serviceInfo != null) {
            if (serviceInfo.getUid() != uid) {
                Log.e(TAG, "getAidGroupForService() - UID mismatch");
                return null;
            }
            return serviceInfo.getDynamicStAidGroupForCategory(category);
        } else {
            Log.e(TAG, "getAidGroupForService() - Could not find service " + componentName);
            return null;
        }
    }

    public boolean removeAidGroupForService(
            int userId, int uid, ComponentName componentName, String category) {

        if (DBG) Log.d(TAG, "removeAidGroupForService()");

        boolean success = false;
        ArrayList<StApduServiceInfo> newServices = null;
        synchronized (mLock) {
            UserServices services = findOrCreateUserLocked(userId);
            StApduServiceInfo serviceInfo = getStService(userId, componentName);
            if (serviceInfo != null) {
                if (serviceInfo.getUid() != uid) {
                    // Calling from different uid
                    Log.e(TAG, "removeAidGroupForService() - UID mismatch");
                    return false;
                }
                if (!serviceInfo.removeDynamicStAidGroupForCategory(category)) {
                    Log.e(
                            TAG,
                            "removeAidGroupForService() - Could not find dynamic AIDs for category "
                                    + category);
                    return false;
                }
                // Remove from local cache
                DynamicSettings dynSettings = services.dynamicSettings.get(componentName);
                if (dynSettings != null) {
                    StAidGroup deletedGroup = dynSettings.aidGroups.remove(category);
                    success = writeDynamicSettingsLocked();
                    if (success) {
                        newServices = new ArrayList<StApduServiceInfo>(services.services.values());
                    } else {
                        Log.e(
                                TAG,
                                "removeAidGroupForService() - Could not persist deleted AID group"
                                        + ".");
                        dynSettings.aidGroups.put(category, deletedGroup);
                        return false;
                    }
                } else {
                    Log.e(
                            TAG,
                            "removeAidGroupForService() - Could not find aid group in local cache"
                                    + ".");
                }
            } else {
                Log.e(
                        TAG,
                        "removeAidGroupForService() - Service "
                                + componentName
                                + " does not exist.");
            }
        }
        if (success) {
            mCallback.onServicesUpdated(userId, newServices);
        }
        return success;
    }

    public void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw.println("HCENfc_RegsiteredServicesCache - Registered HCE services for current user: ");
        UserServices userServices = findOrCreateUserLocked(ActivityManager.getCurrentUser());
        for (StApduServiceInfo service : userServices.services.values()) {
            service.dump(fd, pw, args);
            pw.println("");
        }
        pw.println("");
    }

    public void updateStatusOfServices(boolean commitStatus) {

        if (DBG) Log.d(TAG, "updateStatusOfServices() - commitStatus: " + commitStatus);

        final UserServices userServices = mUserServices.get(ActivityManager.getCurrentUser());
        /* update for AID based services */
        for (StApduServiceInfo serviceInfo : userServices.services.values()) {
            if (!serviceInfo.hasCategory(CardEmulation.CATEGORY_OTHER)) {
                continue;
            }
            serviceInfo.updateServiceCommitStatus(CardEmulation.CATEGORY_OTHER, commitStatus);
        }

        Log.d(
                TAG,
                "updateStatusOfServices() - 3 "
                        + Thread.currentThread().getStackTrace()[2].getMethodName()
                        + ": WriteServiceStateToFile");
        writeServiceStateToFile(ActivityManager.getCurrentUser());
    }
}
