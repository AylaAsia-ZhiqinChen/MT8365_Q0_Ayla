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
import android.bluetooth.mesh.ConfigMessageParams;

public class ConfigurationClientModel extends MeshModel {
    private static final String TAG = "ConfigurationClientModel";
    private static final boolean DBG = MeshConstants.DEBUG;

    public ConfigurationClientModel(BluetoothMesh meshInst) {
        super(meshInst, MeshConstants.MESH_MODEL_DATA_OP_ADD_CONFIGURATION_CLIENT);
    }

    public void setConfigMessageHeader(int src, int dst, int ttl, int netKeyIndex, int msgOpCode) {
        if (DBG) Log.d(TAG, "setConfigMessageHeader");
        super.setConfigMessageHeader(src, dst, ttl, netKeyIndex, msgOpCode);
    }

    /** configuration tx msg */
    public void configBeaconGet() {
        if (DBG) Log.d(TAG, "configBeaconGet");
        super.modelSendConfigMessage();
    }

    public void configBeaconSet(ConfigMessageParams param) {}

    public void configCompositionDataGet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configCompositionDataGet");
        super.modelSendConfigMessage(param);
    }

    public void configDefaultTTLGet() {
        if (DBG) Log.d(TAG, "configDefaultTTLGet");
        super.modelSendConfigMessage();
    }

    public void configDefaultTTLSet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configDefaultTTLSet");
        super.modelSendConfigMessage(param);
    }

    public void configGattProxyGet() {
        if (DBG) Log.d(TAG, "configGattProxyGet");
        super.modelSendConfigMessage();
    }

    public void configGattProxySet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configGattProxySet");
        super.modelSendConfigMessage(param);
    }

    public void configFriendGet() {
        if (DBG) Log.d(TAG, "configFriendGet");
        super.modelSendConfigMessage();
    }

    public void configFriendSet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configFriendSet");
        super.modelSendConfigMessage(param);
    }

    public void configRelayGet() {
        if (DBG) Log.d(TAG, "configRelayGet");
        super.modelSendConfigMessage();
    }

    public void configRelaySet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configRelaySet");
        super.modelSendConfigMessage(param);
    }

    public void configModelPubGet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configModelPubGet");
        super.modelSendConfigMessage(param);
    }

    public void configModelPubSet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configModelPubSet");
        super.modelSendConfigMessage(param);
    }

    public void configModelSubAdd(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configModelSubAdd");
        super.modelSendConfigMessage(param);
    }

    public void configModelSubDel(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configModelSubDel");
        super.modelSendConfigMessage(param);
    }

    public void configModelSubOw(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configModelSubOw");
        super.modelSendConfigMessage(param);
    }

    public void configModelSubDelAll(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configModelSubDelAll");
        super.modelSendConfigMessage(param);
    }

    public void configSigModelSubGet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configSigModelSubGet");
        super.modelSendConfigMessage(param);
    }

    public void configVendorModelSubGet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configVendorModelSubGet");
        super.modelSendConfigMessage(param);
    }

    public void configNetkeyAdd(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configNetkeyAdd");
        super.modelSendConfigMessage(param);
    }

    public void configNetkeyUpdate(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configNetkeyUpdate");
        super.modelSendConfigMessage(param);
    }

    public void configNetkeyDel(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configNetkeyDel");
        super.modelSendConfigMessage(param);
    }

    public void configNetkeyGet() {
        if (DBG) Log.d(TAG, "configNetkeyGet");
        super.modelSendConfigMessage();
    }

    public void configAppkeyAdd(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configAppkeyAdd");
        super.modelSendConfigMessage(param);
    }

    public void configAppkeyUpdate(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configAppkeyUpdate");
        super.modelSendConfigMessage(param);
    }

    public void configAppkeyDel(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configAppkeyDel");
        super.modelSendConfigMessage(param);
    }

    public void configAppkeyGet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configAppkeyGet");
        super.modelSendConfigMessage(param);
    }

    public void configModelAppBind(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configModelAppBind");
        super.modelSendConfigMessage(param);
    }

    public void configModelAppUnbind(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configModelAppUnbind");
        super.modelSendConfigMessage(param);
    }

    public void configSigModelAppGet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configSigModelAppGet");
        super.modelSendConfigMessage(param);
    }

    public void configVendorModelAppGet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configVendorModelAppGet");
        super.modelSendConfigMessage(param);
    }

    public void configNodeIdentityGet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configNodeIdentityGet");
        super.modelSendConfigMessage(param);
    }

    public void configNodeIdentitySet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configNodeIdentitySet");
        super.modelSendConfigMessage(param);
    }

    public void configNodeReset() {
        if (DBG) Log.d(TAG, "configNodeReset");
        super.modelSendConfigMessage();
    }

    public void configKeyRefreshPhaseGet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configKeyRefreshPhaseGet");
        super.modelSendConfigMessage(param);
    }

    public void configKeyRefreshPhaseSet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configKeyRefreshPhaseSet");
        super.modelSendConfigMessage(param);
    }

    public void configHbPubGet() {
        if (DBG) Log.d(TAG, "configHbPubGet");
        super.modelSendConfigMessage();
    }

    public void configHbPubSet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configHbPubSet");
        super.modelSendConfigMessage(param);
    }

    public void configHbSubGet() {
        if (DBG) Log.d(TAG, "configHbSubGet");
        super.modelSendConfigMessage();
    }

    public void configHbSubSet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configHbSubSet");
        super.modelSendConfigMessage(param);
    }

    public void configNetworkTransmitGet() {
        if (DBG) Log.d(TAG, "configNetworkTransmitGet");
        super.modelSendConfigMessage();
    }

    public void configNetworkTransmitSet(ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "configNetworkTransmitSet");
        super.modelSendConfigMessage(param);
    }

}
