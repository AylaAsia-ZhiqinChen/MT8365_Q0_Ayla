/*
 * Copyright (C) 2009 The Android Open Source Project
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

package android.bluetooth;

import android.bluetooth.mesh.BluetoothMeshAccessRxMessage;
import android.bluetooth.mesh.BluetoothMeshAccessTxMessage;
import android.bluetooth.mesh.IBluetoothMeshCallback;
import android.bluetooth.mesh.MeshInitParams;
import android.bluetooth.mesh.MeshModel;
import android.bluetooth.mesh.ConfigMessageParams;
import android.bluetooth.mesh.OtaOperationParams;

/**
 * API for Bluetooth Mesh service
 *
 * {@hide}
 */
interface IBluetoothMesh {
    void registerCallback(in IBluetoothMeshCallback callback);
    int getMeshRole();
    boolean getMeshState();
    int enable(in MeshInitParams params);
    int disable();
    boolean setCompositionDataHeader(in int[] data);
    int addElement( in int location);
    boolean setElementAddr(in int addr);
    int addModel(in MeshModel model);
    int setNetkey(in int op, in int[] key, in int netIndex);
    int setAppkey(in int op, in int[] key, in int netIndex, in int appIndex);
    void unProvisionScan(in boolean start, in int duration);
    int inviteProvisioning(in int[] UUID, in int attentionDuration);
    int startProvisioning(in int algorithm, in int publicKey, in int authMethod, in int authAction, in int authSize,
        in int[] netkey, in int netkeyIndex, in long ivIndex, in int addr, in int flags, in int mode);
    int setProvisionFactor(in int type, in int[] buf);
    int sendConfigMessage(in int dst, in int src, in int ttl, in int netKeyIndex, in int opcode, in ConfigMessageParams param);
    int sendPacket(in int dst, in int src, in int ttl, in int netKeyIndex, in int appKeyIndex, in int[] payload);
    int publishModel(in int modelHandle, in int opCode, in int companyId, in int[] buffer);
    int setMeshMode(in int on);
    boolean resetData(in int sector);
    boolean saveData();
    void setData();
    String getVersion();
    void dump(in int type);
    int getElementAddr(in int elementIndex);
    void setDefaultTTL(in int value);
    int getDefaultTTL();
    int setIv(in long ivIndex, in int  ivPhase);
    int addDevKey(in int unicastAddr, in int[] devicekey, in int[] uuid);
    int[] getDevKey(in int unicastAddr);
    int delDevKey(in int unicastAddr);
    int modelAppBind(in int handle, in int appIndex);
    int accessModelReply(in int modelHandle, in BluetoothMeshAccessRxMessage msg, in BluetoothMeshAccessTxMessage reply);
    void setLogLevel(in long level);
    int getModelHandle(in long modelId, in int elementIndex);
    int gattConnect(in String address, in int addressType, in int serviceType);
    int gattDisconnect();
    int setHeartbeatPeriod(in int num, in long hbTimeout);
    int[] otaGetClientModelHandle();
    int otaInitiatorOperation (in OtaOperationParams params);
    int bearerAdvSetParams (in long advPeriod, in int minInterval, in int maxInterval, in int resend,
        in long scanPeriod, in int scanInterval, in int scanWindow);
    int setScanParams (in int scanInterval, in int scanWindow);
    int setSpecialPktParams (in boolean isSnIncrease, in int snIncreaseInterval, in int advInterval, in int advPeriod);
}
