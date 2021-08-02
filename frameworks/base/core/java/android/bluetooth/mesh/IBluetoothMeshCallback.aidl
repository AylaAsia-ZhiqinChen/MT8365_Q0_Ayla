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
package android.bluetooth.mesh;

import android.os.ParcelUuid;
import android.bluetooth.le.ScanResult;
import android.bluetooth.mesh.BluetoothMeshAccessRxMessage;


/**
 * Callback definitions for interacting with MESH Call Back
 * @hide
 */
interface IBluetoothMeshCallback {

    void onMeshEnabled(); // for enable call back

    void onConfigReset();

    void onFriendShipStatus(in int addr, in int status);

    void onOTAEvent(in int event_id, in int error_code, in long serial_number, in long firmware_id, in long time_escaped,
        in int nodes_num, in int curr_block, in int total_block, in int curr_chunk, in int chunk_mask, in int[] nodes_status);

    void onAdvReport(in int addr_type, in int[] addr, in int rssi, in int report_type, in int[] data);

    void onProvScanComplete();

    void onScanUnProvDevice(in int[] uuid, in int oobInfom, in int[] uriHash);

    void onProvCapabilities(in int numberOfElements, in int algorithms, in int publicKeyType,
        in int staticOOBType, in int outputOobSize, in int outputOobAction,
        in int inputOobSize, in int inputOobAction);

    void onRequestOobPublicKey();  //to read OOB Public Key

    void onRequestOobAuthValue(in int method, in int action, in int size);

    void onProvShowOobPublicKey(in int[] publicKey);  //APP shall save the oob public key

    void onProvShowOobAuthValue(in int[] authValue);

    void onProvDone(in int address, in int[] deviceKey, in boolean success, in boolean gatt_bearer);

    void onScanResult(in ScanResult scanResult);

    void onKeyRefresh(in int netKeyIndex, in int phase);

    void onIvUpdate(in int ivIndex, in int state);

    void onSeqChange(in int seqNumber);

    void onProvFactor(in int type, in int[] buf, in int bufLen);

    void onHeartbeat(in int address, in int active);

    void onBearerGattStatus(in long handle, in int status);

    void onEvtErrorCode(in int type);

    void onOTAMsgHandler(in int modelHandle, in BluetoothMeshAccessRxMessage msg);

    void onMsgHandler(in int modelHandle, in BluetoothMeshAccessRxMessage msg);

    void onPublishTimeoutCallback(in int modelHandle);

}
