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

package android.bluetooth.mesh.model;

import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;
import android.bluetooth.BluetoothMesh;
import android.bluetooth.mesh.MeshModel;
import android.bluetooth.mesh.MeshConstants;

public class ConfigurationServerModel extends MeshModel {
    private static final String TAG = "ConfigurationServerModel";
    private static final boolean DBG = MeshConstants.DEBUG;

    public ConfigurationServerModel(BluetoothMesh meshInst) {
        super(meshInst, MeshConstants.MESH_MODEL_DATA_OP_ADD_CONFIGURATION_SERVER);
    }

    public void setConfigMessageHeader(int src, int dst, int ttl, int netKeyIndex, int msgOpCode) {
        if (DBG) Log.d(TAG, "setConfigMessageHeader");
        super.setConfigMessageHeader(src, dst, ttl, netKeyIndex, msgOpCode);
    }

/** Configuration Server Messages */
/*public void configBeaconStatus(int beacon) {
        if (DBG) Log.d(TAG, "configBeaconStatus");
        super.modelSendConfigMessage();
    }

    public void configCompositionDataStatus(int page, int[] data) {
        if (DBG) Log.d(TAG, "configCompositionDataStatus");
        super.modelSendConfigMessage();
    }

    public void configDefaultTTLStatus(int ttl) {
        if (DBG) Log.d(TAG, "configDefaultTTLStatus");
        super.modelSendConfigMessage();
    }

    public void configGATTProxyStatus(int gattProxy) {
        if (DBG) Log.d(TAG, "configGATTProxyStatus");
        super.modelSendConfigMessage();
    }

    public void configFriendStatus(int friend) {
        if (DBG) Log.d(TAG, "configFriendStatus");
        super.modelSendConfigMessage();
    }

    public void configRelayStatus(int relay, int relayRetransmitCount, int relayRetransmitIntervalSteps) {
        if (DBG) Log.d(TAG, "configRelayStatus");
        super.modelSendConfigMessage();
    }

    public void configModelPublicationStatus(int status, int elementAddress, int publishAddress,
                int appKeyIndex, int credentialFlag, int rfu, int publishTTL, int publishPeriod,
                int publishRetransmitCount, int publishRetransmitIntervalSteps, long modelIdentifier) {
        if (DBG) Log.d(TAG, "configModelPublicationStatus");
        super.modelSendConfigMessage();
    }

    public void configModelSubscriptionStatus(int status, int elementAddress, int address, long modelIdentifier) {
        if (DBG) Log.d(TAG, "configModelSubscriptionStatus");
        super.modelSendConfigMessage();
    }

    public void configSIGModelSubscriptionList(int status, int elementAddress, long modelIdentifier, int[] addresses) {
        if (DBG) Log.d(TAG, "configBeaconStatus");
        super.modelSendConfigMessage();
    }

    public void configVendorModelSubscriptionList(int status, int elementAddress, long modelIdentifier, int[] addresses) {
        if (DBG) Log.d(TAG, "configVendorModelSubscriptionList");
        super.modelSendConfigMessage();
    }

    public void configNetKeyStatus(int status, int netKeyIndex) {
        if (DBG) Log.d(TAG, "configNetKeyStatus");
        super.modelSendConfigMessage();
    }

    public void configNetKeyList(int[] netKeyIndexes) {
        if (DBG) Log.d(TAG, "configNetKeyList");
        super.modelSendConfigMessage();
    }

    public void configAppKeyStatus(int status, int netKeyIndex, int appKeyIndex) {
        if (DBG) Log.d(TAG, "configAppKeyStatus");
        super.modelSendConfigMessage();
    }

    public void configAppKeyList(int status, int netKeyIndex, int[] appKeyIndexes) {
        if (DBG) Log.d(TAG, "configAppKeyList");
        super.modelSendConfigMessage();
    }

    public void configModelAppStatus(int status, int elementAddress, int appKeyIndexes, long modelIdentifier) {
        if (DBG) Log.d(TAG, "configModelAppStatus");
        super.modelSendConfigMessage();
    }

    public void configSIGModelAppList(int status, int elementAddress, long modelIdentifier, int[] appKeyIndexes) {
        if (DBG) Log.d(TAG, "configSIGModelAppList");
        super.modelSendConfigMessage();
    }

    public void configVendorModelAppList(int status, int elementAddress, long modelIdentifier, int[] appKeyIndexes) {
        if (DBG) Log.d(TAG, "configVendorModelAppList");
        super.modelSendConfigMessage();
    }

    public void configNodeIdentityStatus() {
        if (DBG) Log.d(TAG, "configNodeIdentityStatus");
        super.modelSendConfigMessage();
    }

    public void configNodeResetStatus() {
        if (DBG) Log.d(TAG, "configNodeResetStatus");
        super.modelSendConfigMessage();
    }

    public void configKeyRefreshPhaseStatus(int status, int netKeyIndex, int phase) {
        if (DBG) Log.d(TAG, "configKeyRefreshPhaseStatus");
        super.modelSendConfigMessage();
    }

    public void configHeartbeatPublicationStatus() {
        if (DBG) Log.d(TAG, "configHeartbeatPublicationStatus");
        super.modelSendConfigMessage();
    }

    public void configHeartbeatSubscriptionStatus(int status, int destination, int countLog, int periodLog, int ttl, int features, int netKeyIndex) {
        if (DBG) Log.d(TAG, "configHeartbeatSubscriptionStatus");
        super.modelSendConfigMessage();
    }

    public void configNetworkTransmitStatus(int networkTransmitCount, int networkTransmitIntervalSteps) {
        if (DBG) Log.d(TAG, "configNetworkTransmitStatus");
        super.modelSendConfigMessage();
    }
*/
}
