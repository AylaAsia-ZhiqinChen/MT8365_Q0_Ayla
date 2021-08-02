/*
 * Copyright (C) 2016 The Android Open Source Project
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

package android.bluetooth.mesh;

import android.util.Log;
import android.bluetooth.le.ScanResult;

public abstract class BluetoothMeshCallback {

    private static final String TAG = BluetoothMeshCallback.class.getSimpleName();

    /**
     * Mesh enable done callback.
     *
     * @return void
     */
    public void onMeshEnabled() {
        Log.d(TAG, "onMeshEnabled:");
    }

    /**
     * config reset callback.
     *
     * @return void
     */
    public void onConfigReset() {
        Log.d(TAG, "onConfigReset:");
    }

    /**
     * friendship status callback.
     * <p>status values are:
     * {MeshConstants #MESH_FRIENDSHIP_TERMINATED},
     * {MeshConstants #MESH_FRIENDSHIP_ESTABLISHED},
     * {MeshConstants #MESH_FRIENDSHIP_ESTABLISH_FAILED},
     * {MeshConstants #MESH_FRIENDSHIP_REQUEST_FRIEND_TIMEOUT},
     * {MeshConstants #MESH_FRIENDSHIP_SELECT_FRIEND_TIMEOUT}.
     *
     * @param addr
     * @param status friendship status
     * @return void
     */
    public void onFriendShipStatus(int addr, int status) {
        Log.d(TAG, "onFriendShipStatus + addr " + addr + "staus " + status);
    }

    /**
     * OTA event callback.
     * <p>eventId values are:
     * {MeshConstants #MESH_OTA_EVENT_DISTRIBUTION_STARTING},
     * {MeshConstants #MESH_OTA_EVENT_DISTRIBUTION_STARTED},
     * {MeshConstants #MESH_OTA_EVENT_DISTRIBUTION_ONGOING},
     * {MeshConstants #MESH_OTA_EVENT_DISTRIBUTION_STOP},
     * {MeshConstants #MESH_OTA_EVENT_DISTRIBUTION_QUEUED},
     * {MeshConstants #MESH_OTA_EVENT_DISTRIBUTION_DFU_READY}.
     *
     * <p>errorCode values are:
     * {MeshConstants #MESH_OTA_ERROR_CODE_SUCCESS},
     * {MeshConstants #MESH_OTA_ERROR_CODE_WRONG_FIRMWARE_ID},
     * {MeshConstants #MESH_OTA_ERROR_CODE_BUSY},
     * {MeshConstants #MESH_OTA_ERROR_CODE_NO_RESPONSE},
     * {MeshConstants #MESH_OTA_ERROR_CODE_USER_STOP}.
     *
     * <p>status of nodesStatus values are:
     * {MeshConstants #MESH_OTA_NODE_UPDATE_STATUS_SUCCESS},
     * {MeshConstants #MESH_OTA_NODE_UPDATE_STATUS_IN_PROGRESS},
     * {MeshConstants #MESH_OTA_NODE_UPDATE_STATUS_CANCELED},
     * {MeshConstants #MESH_OTA_NODE_UPDATE_STATUS_DFU_READY}.
     *
     * @param eventId
     * @param errorCode
     * @param serialNumber
     * @param firmwareId
     * @param timeEscaped time escaped from started
     * @param nodesNum
     * @param nodesStatus is an array with element number of nodes_num: first int is addr, second int is status
     * nodesStatus array length should be nodes_num * 2: |addr0|status0|addr1|status1|addr2|status2|....
     * @return void
     */
    public void onOTAEvent(int eventId, int errorCode, long serialNumber, long firmwareId, long timeEscaped,
        int nodesNum, int curr_block, int total_block, int curr_chunk, int chunk_mask, int[] nodesStatus) {
        Log.d(TAG, "onOTAEvent + eventId " + eventId + " errorCode " + errorCode + " nodesNum " + nodesNum +
             " curr_block " + curr_block + " total_block " + total_block + " curr_chunk " + curr_chunk + " chunk_mask " + chunk_mask);
    }

    /**
     * Adv report callback.
     * <p>reportType values are:
     * {MeshConstants #MESH_REPORT_TYPE_IND},
     * {MeshConstants #MESH_REPORT_TYPE_DIRECT_IND},
     * {MeshConstants #MESH_REPORT_TYPE_SCAN_IND},
     * {MeshConstants #MESH_REPORT_TYPE_NONCONN_IND},
     * {MeshConstants #MESH_REPORT_TYPE_SCAN_RSP}.
     *
     * @param addrType peer address type
     * @param addr peer address
     * @param rssi
     * @param reportType
     * @param data
     * @return void
     */
    public void onAdvReport(int addrType, int[] addr, int rssi, int reportType, int[] data) {
        Log.d(TAG, "onAdvReport + addrType " + addrType + " rssi " + rssi + " reportType " + reportType);
    }

    /**
     * Provision scan complete callback.
     *
     * @return void
     */
    public void onProvScanComplete() {
        Log.d(TAG, "onProvScanComplete:");
    }

    /**
     * Scan unprovisioned device callback.
     *
     * @param uuid
     * @param oobInfom
     * @param uriHash
     * @return void
     */
    public void onScanUnProvDevice(int[] uuid, int oobInfom, int[] uriHash) {
        Log.d(TAG, "onScanUnProvDevice: uuid=" + uuid + " oobInfom=" + oobInfom + " uriHash=" + uriHash);
    }

    /**
     * Provision capabilities callback.
     *
     * @param numberOfElements
     * @param algorithms
     * @param publicKeyType
     * @param staticOOBType
     * @param outputOobSize
     * @param outputOobAction
     * @param inputOobSize
     * @param inputOobAction
     * @return void
     */
    public void onProvCapabilities(int numberOfElements, int algorithms, int publicKeyType,
        int staticOOBType, int outputOobSize, int outputOobAction, int inputOobSize, int inputOobAction) {
        Log.d(TAG, "onProvCapabilities: numberOfElements=" + numberOfElements + " algorithms=" + algorithms
            + " publicKeyType=" + publicKeyType);
    }

    /**
     * Request OOB public key callback.
     *
     * @return void
     */
    public void onRequestOobPublicKey() {
        Log.d(TAG, "onRequestOobPublicKey:");
    }

    /**
     * Request OOB auth value callback.
     *
     * @param method
     * @param action
     * @param size
     * @return void
     */
    public void onRequestOobAuthValue(int method, int action, int size) {
        Log.d(TAG, "onRequestOobAuthValue: method=" + method + " action=" + action + " size=" + size);
    }

    /**
     * Provision show OOB public key callback.
     *
     * @param publicKey
     * @return void
     */
    public void onProvShowOobPublicKey(int[] publicKey) {
        Log.d(TAG, "onProvShowOobPublicKey: publicKey=" + publicKey);
    }

    /**
     * Provision show OOB auth value callback.
     *
     * @param authValue
     * @return void
     */
    public void onProvShowOobAuthValue(int[] authValue) {
        Log.d(TAG, "onProvShowOobAuthValue: authValue=" + authValue);
    }

    /**
     * Provision done callback.
     *
     * @param address
     * @param deviceKey
     * @param success true is provision success, false is provision fail
     * @param gattBearer is gatt bearer or not
     * @return void
     */
    public void onProvDone(int address, int[] deviceKey, boolean success, boolean gattBearer) {
        Log.d(TAG, "onProvDone: address=" + address + " success=" + success + " gattBearer=" + gattBearer);
    }

    /**
     * Scan result callback.
     *
     * @param scanResult
     * @return void
     */
    public void onScanResult(ScanResult scanResult) {
        Log.d(TAG, "onScanResult: scanResult=" + scanResult);
    }

    /**
     * Key refresh callback.
     * <p>phase values are:
     * {MeshConstants #MESH_KEY_REFRESH_STATE_NONE},
     * {MeshConstants #MESH_KEY_REFRESH_STATE_1},
     * {MeshConstants #MESH_KEY_REFRESH_STATE_2},
     * {MeshConstants #MESH_KEY_REFRESH_STATE_3}.
     *
     * @param netKeyIndex The network key index
     * @param phase Current key refresh phase
     * @return void
     */
    public void onKeyRefresh(int netKeyIndex, int phase) {
        Log.d(TAG, "onKeyRefresh: netKeyIndex=" + netKeyIndex + " phase=" + phase);
    }

    /**
     * IV update callback.
     * <p>state values are:
     * {MeshConstants #MESH_IV_UPDATE_STATE_NORMAL},
     * {MeshConstants #MESH_IV_UPDATE_STATE_IN_PROGRESS}.
     *
     * @param ivIndex The IV index currently used for sending messages
     * @param state Current IV update state
     * @return void
     */
    public void onIvUpdate(int ivIndex, int state) {
        Log.d(TAG, "onIvUpdate: ivIndex=" + ivIndex + " state=" + state);
    }

    /**
     * Seq change callback.
     *
     * @param seqNumber
     * @return void
     */
    public void onSeqChange(int seqNumber) {
        Log.d(TAG, "onSeqChange: seqNumber=" + seqNumber);
    }

    /**
     * Provision factor callback.
     * <p>type values are:
     * {MeshConstants #MESH_PROV_FACTOR_CONFIRMATION_KEY},
     * {MeshConstants #MESH_PROV_FACTOR_RANDOM_PROVISIONER},
     * {MeshConstants #MESH_PROV_FACTOR_RANDOM_DEVICE},
     * {MeshConstants #MESH_PROV_FACTOR_CONFIRMATION_PROVISIONER},
     * {MeshConstants #MESH_PROV_FACTOR_CONFIRMATION_DEVICE},
     * {MeshConstants #MESH_PROV_FACTOR_PUB_KEY},
     * {MeshConstants #MESH_PROV_FACTOR_AUTHEN_VALUE},
     * {MeshConstants #MESH_PROV_FACTOR_AUTHEN_RESULT},
     * {MeshConstants #MESH_PROV_FACTOR_PROV_INVITE}.
     *
     * @param type
     * @param buf
     * @param bufLen
     * @return void
     */
    public void onProvFactor(int type, int[] buf, int bufLen) {
        Log.d(TAG, "onProvFactor: type=" + type + " bufLen=" + bufLen);
    }

    /**
     * Heartbeat callback.
     *
     * @param address peer address
     * @param active active status
     * @return void
     */
    public void onHeartbeat(int address, int active) {
        Log.d(TAG, "onHeartbeat: address=" + address + " active=" + active);
    }

    /**
     * Bear Gatt connect status callback.
     * <p>status values are:
     * {MeshConstants #MESH_BEARER_GATT_STATUS_CONNECTED},
     * {MeshConstants #MESH_BEARER_GATT_STATUS_DISCONNECTED},
     * {MeshConstants #MESH_BEARER_GATT_STATUS_NO_SERVICE},
     * {MeshConstants #MESH_BEARER_GATT_STATUS_NO_CHARACTERISTIC},
     * {MeshConstants #MESH_BEARER_GATT_STATUS_WRITE_CCCD_FAILED}.
     *
     * @param handle
     * @param status gatt connect status
     * @return void
     */
    public void onBearerGattStatus(long handle, int status) {
        Log.d(TAG, "onBearerGattStatus: handle=" + handle + " status=" + status);
    }

    /**
     * Event Error Code callback.
     * <p>type values are:
     * {MeshConstants #MESH_ERROR_NO_RESOURCE_TO_ADD_REPLAYPROTECTION}.
     *
     * @param type error type
     * @return void
     */
    public void onEvtErrorCode(int type) {
        Log.d(TAG, "onEvtErrorCode: type=" + type);
    }

    /**
     * OTA message handler callback.
     *
     * @param modelHandle
     * @param msg access rx msg
     * @return void
     */
    public void onOTAMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
        Log.d(TAG, "onOTAMsgHandler: modelHandle=" + modelHandle);
    }
}
