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
import android.bluetooth.mesh.BluetoothMeshAccessRxMessage;
import android.bluetooth.mesh.MeshModel;
import android.bluetooth.mesh.MeshConstants;

public class GenericOnOffClientModel extends MeshModel {
    private static final String TAG = "GenericOnOffClientModel";
    private static final boolean DBG = MeshConstants.DEBUG;

    public GenericOnOffClientModel(BluetoothMesh meshInst) {
        super(meshInst, MeshConstants.MESH_MODEL_DATA_OP_ADD_GENERIC_ONOFF_CLIENT);
    }

    public void setTxMessageHeader(int dstAddrType, int dst, int[] virtualUUID,
                        int src, int ttl, int netKeyIndex, int appKeyIndex, int msgOpCode) {
        if (DBG) Log.d(TAG, "setTxMessageHeader");
        super.setTxMessageHeader(dstAddrType, dst, virtualUUID, src, ttl, netKeyIndex, appKeyIndex, msgOpCode);
    }

    /** Generic OnOff Client Messages */
    public void genericOnOffGet() {
        if (DBG) Log.d(TAG, "genericOnOffGet");
        super.modelSendPacket();
    }

    public void genericOnOffSet(int onOff, int tid, int transitionTime, int delay) {
        if (DBG) Log.d(TAG, "genericOnOffSet");
        super.modelSendPacket(onOff, tid, transitionTime, delay);
    }

    public void genericOnOffSetUnacknowledged(int onOff, int tid, int transitionTime, int delay) {
        if (DBG) Log.d(TAG, "genericOnOffSetUnacknowledged");
        super.modelSendPacket(onOff, tid, transitionTime, delay);
    }

}
