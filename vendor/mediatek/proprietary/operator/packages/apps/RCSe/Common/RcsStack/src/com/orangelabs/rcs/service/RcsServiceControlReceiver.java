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

package com.orangelabs.rcs.service;

import com.orangelabs.rcs.core.Core;
import com.orangelabs.rcs.provider.settings.RcsSettings;
//import com.gsma.rcs.provider.settings.RcsSettingsData.TermsAndConditionsResponse;  /*will include in rcs settings later*/
//import com.gsma.rcs.provisioning.TermsAndConditionsRequest;  /*will include in rcs settings later*/
import com.orangelabs.rcs.utils.logger.Logger;
import com.gsma.services.rcs.Intents;
import com.gsma.services.rcs.RcsService;
import com.gsma.services.rcs.capability.CapabilityService;
import com.gsma.services.rcs.chat.ChatService;
import com.gsma.services.rcs.contact.ContactService;
import com.gsma.services.rcs.extension.MultimediaSessionService;
//import com.gsma.services.rcs.filetransfer.FileTransferService;
import com.gsma.services.rcs.history.HistoryService;
//import com.gsma.services.rcs.sharing.geoloc.GeolocSharingService;
//import com.gsma.services.rcs.sharing.image.ImageSharingService;
//import com.gsma.services.rcs.sharing.video.VideoSharingService;
import com.gsma.services.rcs.upload.FileUploadService;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.os.Bundle;
import android.text.TextUtils;

import java.util.HashMap;
import java.util.Map;

/**
 * A class to control the service activation.
 *
 * @author yplo6403
 */
public class RcsServiceControlReceiver extends BroadcastReceiver {

    private static final long INTENT_RESPONSE_TIMEOUT = 1000;

    private static final int INVALID_EXTRA = -1;

    private static final Logger sLogger = Logger.getLogger(RcsServiceControlReceiver.class
            .getName());

    private RcsSettings mRcsSettings;

    private static boolean sAccurateLog = false;

    private interface IRcsCompatibility {
        boolean isCompatible(String serviceName, String codename, int version, int increment);
    }

    private static IRcsCompatibility sRcsCompatibility = new IRcsCompatibility() {
        @Override
        public boolean isCompatible(String serviceName, String codename, int version, int increment) {
            if (!RcsService.Build.API_CODENAME.equals(codename)) {
                return false;
            }
            switch (version) {
                case RcsService.Build.VERSION_CODES.BLACKBIRD:
                case RcsService.Build.VERSION_CODES.CPR:
                    return true;
                default:
                    return false;
            }
        }
    };

    private static final Map<String, IRcsCompatibility> sServiceCompatibilityMap = new HashMap<>();
    static {
        sServiceCompatibilityMap.put(CapabilityService.class.getSimpleName(), sRcsCompatibility);
        sServiceCompatibilityMap.put(ContactService.class.getSimpleName(), sRcsCompatibility);
        sServiceCompatibilityMap.put(ChatService.class.getSimpleName(), sRcsCompatibility);
        //sServiceCompatibilityMap.put(FileTransferService.class.getSimpleName(), sRcsCompatibility);
        sServiceCompatibilityMap.put(FileUploadService.class.getSimpleName(), sRcsCompatibility);
        //sServiceCompatibilityMap.put(GeolocSharingService.class.getSimpleName(), sRcsCompatibility);
        sServiceCompatibilityMap.put(HistoryService.class.getSimpleName(), sRcsCompatibility);
        //sServiceCompatibilityMap.put(ImageSharingService.class.getSimpleName(), sRcsCompatibility);
        sServiceCompatibilityMap.put(MultimediaSessionService.class.getSimpleName(),
                sRcsCompatibility);
        //sServiceCompatibilityMap.put(VideoSharingService.class.getSimpleName(), sRcsCompatibility);
    }

    private boolean getActivationModeChangeable(Context ctx) {
        switch (mRcsSettings.getEnableRcseSwitch()) { /*will include in rcs settings later*/
            case ALWAYS_SHOW:
                return true;
            case ONLY_SHOW_IN_ROAMING:
                return isDataRoamingEnabled(ctx);
            case NEVER_SHOW:
            default:
                return false;
        }
    }

    private boolean getActivationMode() {
        return mRcsSettings.isServiceActivated();
    }

    private boolean isDataRoamingEnabled(Context ctx) {
        ConnectivityManager cm = (ConnectivityManager) ctx
                .getSystemService(Context.CONNECTIVITY_SERVICE);
        return cm.getActiveNetworkInfo() != null && cm.getActiveNetworkInfo().isRoaming();
    }

    private boolean setActivationMode(Context ctx, boolean active) {
        boolean wasActivated = mRcsSettings.isServiceActivated();
        if (wasActivated == active) {
            if (sLogger.isActivated()) {
                sLogger.warn("setActivationMode: Already set to " + active);
            }
            return active;
        }
        if (!getActivationModeChangeable(ctx)) {
            sLogger.warn("setActivationMode: Cannot change activation mode - permission denied!");
            return wasActivated;
        }
        if (sLogger.isActivated()) {
            sLogger.debug("setActivationMode: " + active);
        }
        mRcsSettings.setServiceActivationState(active);
        if (active) {
            if (/*TermsAndConditionsResponse.DECLINED == mRcsSettings.getTermsAndConditionsResponse()*/ true) {
                /*
                 * Since user activates the stack he does not decline RCS service anymore.
                 */
               // mRcsSettings.setTermsAndConditionsResponse(TermsAndConditionsResponse.NO_ANSWER);
            }
            LauncherUtils.launchRcsService(ctx, false, true);
        } else {
            //TermsAndConditionsRequest.cancelTermsAndConditionsNotification(ctx);
            LauncherUtils.stopRcsService(ctx);
        }
        return active;
    }

    private boolean isCompatible(String serviceName, String codename, int version, int increment) {
        if (TextUtils.isEmpty(serviceName) || TextUtils.isEmpty(codename)
                || version == INVALID_EXTRA || increment == INVALID_EXTRA) {
            return false;
        }

        IRcsCompatibility iRcsCompatibility = sServiceCompatibilityMap.get(serviceName);
        return iRcsCompatibility != null
                && iRcsCompatibility.isCompatible(serviceName, codename, version, increment);
    }

    private class IntentProcessor extends Thread {

        public volatile boolean mHaveResult = false;

        private final Context mCtx;
        private final Intent mIntent;
        private final Bundle mResult;

        private IntentProcessor(Context ctx, Intent intent, Bundle result) {
            mCtx = ctx;
            mIntent = intent;
            mResult = result;
        }

        @Override
        public void run() {
            String action = mIntent.getAction();
            synchronized (mResult) {
                switch (action) {
                    case Intents.Service.ACTION_GET_ACTIVATION_MODE:
                        boolean activationMode = getActivationMode();
                        mResult.putBoolean(Intents.Service.EXTRA_GET_ACTIVATION_MODE,
                                activationMode);
                        if (sAccurateLog && sLogger.isActivated()) {
                            sLogger.debug("isActivated() -> " + activationMode);
                        }
                        break;

                    case Intents.Service.ACTION_GET_COMPATIBILITY:
                        String serviceName = mIntent
                                .getStringExtra(Intents.Service.EXTRA_GET_COMPATIBILITY_SERVICE);
                        String codename = mIntent
                                .getStringExtra(Intents.Service.EXTRA_GET_COMPATIBILITY_CODENAME);
                        int version = mIntent.getIntExtra(
                                Intents.Service.EXTRA_GET_COMPATIBILITY_VERSION, INVALID_EXTRA);
                        int increment = mIntent.getIntExtra(
                                Intents.Service.EXTRA_GET_COMPATIBILITY_INCREMENT, INVALID_EXTRA);
                        boolean compatible = isCompatible(serviceName, codename, version, increment);
                        mResult.putBoolean(Intents.Service.EXTRA_GET_COMPATIBILITY_RESPONSE,
                                compatible);
                        if (sAccurateLog && sLogger.isActivated()) {
                            sLogger.debug("isCompatible(" + serviceName + ") -> " + compatible);
                        }
                        break;

                    case Intents.Service.ACTION_GET_SERVICE_STARTING_STATE:
                        Core core = Core.getInstance();
                        boolean started = core != null && core.isCoreStarted();
                        mResult.putBoolean(Intents.Service.EXTRA_GET_SERVICE_STARTING_STATE,
                                started);
                        if (sAccurateLog && sLogger.isActivated()) {
                            sLogger.debug("isServiceStarted() -> " + started);
                        }
                        break;

                    case Intents.Service.ACTION_GET_ACTIVATION_MODE_CHANGEABLE:
                        boolean activationModeChangeable = getActivationModeChangeable(mCtx);
                        mResult.putBoolean(Intents.Service.EXTRA_GET_ACTIVATION_MODE_CHANGEABLE,
                                activationModeChangeable);
                        if (sAccurateLog && sLogger.isActivated()) {
                            sLogger.debug("isActivationModeChangeAble() -> "
                                    + activationModeChangeable);
                        }
                        break;

                    case Intents.Service.ACTION_SET_ACTIVATION_MODE: {
                        boolean active = mIntent.getBooleanExtra(
                                Intents.Service.EXTRA_SET_ACTIVATION_MODE, true);
                        activationMode = setActivationMode(mCtx, active);
                        mResult.putBoolean(Intents.Service.EXTRA_SET_ACTIVATION_MODE,
                                activationMode);
                        if (sAccurateLog && sLogger.isActivated()) {
                            sLogger.debug("setActivationMode(" + active + ") -> " + activationMode);
                        }
                        break;
                    }
                }
                mHaveResult = true;
                mResult.notify();
            }
        }

    }

    @SuppressWarnings("SynchronizationOnLocalVariableOrMethodParameter")
    @Override
    public void onReceive(Context ctx, Intent intent) {
        final String action = intent.getAction();
        sLogger.debug("Received: " + action);
        if (!("com.gsma.services.rcs.action.GET_ACTIVATION_MODE_CHANGEABLE".equals(action)
              || "com.gsma.services.rcs.action.GET_ACTIVATION_MODE".equals(action)
              || "com.gsma.services.rcs.action.SET_ACTIVATION_MODE".equals(action)
              || "com.gsma.services.rcs.action.GET_COMPATIBILITY".equals(action)
              || "com.gsma.services.rcs.action.GET_SERVICE_STARTING_STATE".equals(action))) {
            return;
        }

        if(RcsSettings.getInstance()==null){
            RcsSettings.createInstance(ctx);
        }
        mRcsSettings = RcsSettings.getInstance();

        Bundle result = getResultExtras(true);
        IntentProcessor intentProcessor = new IntentProcessor(ctx, intent, result);
        intentProcessor.start();
        long endTime = System.currentTimeMillis() + INTENT_RESPONSE_TIMEOUT;
        synchronized (result) {
            while (!intentProcessor.mHaveResult) {
                long delay = endTime - System.currentTimeMillis();
                if (delay <= 0) {
                    sLogger.warn("Waiting for result for " + action + " has reached deadline!");
                    break;
                }
                try {
                    if (sAccurateLog) {
                        sLogger.debug("Waiting for result for " + action + " during max " + delay
                                + "ms");
                    }
                    result.wait(delay);

                } catch (InterruptedException e) {
                    sLogger.warn("Waiting for result for " + action + " was interrupted!");
                }
            }
        }
    }
}
