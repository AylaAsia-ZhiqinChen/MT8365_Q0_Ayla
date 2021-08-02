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

public class GenericOnOffServerModel extends MeshModel {
    private static final String TAG = "GenericOnOffServerModel";
    private static final boolean DBG = MeshConstants.DEBUG;

    /** Generic OnOff Server States */
    public int genericOnOffState;

    public GenericOnOffServerModel(BluetoothMesh meshInst) {
        super(meshInst, MeshConstants.MESH_MODEL_DATA_OP_ADD_GENERIC_ONOFF_SERVER);
    }

    public void setTxMessageHeader(int dstAddrType, int dst, int[] virtualUUID,
                        int src, int ttl, int netKeyIndex, int appKeyIndex, int msgOpCode) {
        if (DBG) Log.d(TAG, "setTxMessageHeader");
        super.setTxMessageHeader(dstAddrType, dst, virtualUUID, src, ttl, netKeyIndex, appKeyIndex, msgOpCode);
    }

    /** Generic OnOff Server Messages */
    public void genericOnOffStatus(int presentOnOff, int targetOnOff, int remainingTime) {
        if (DBG) Log.d(TAG, "genericOnOffStatus");
        super.modelSendPacket(presentOnOff, targetOnOff, remainingTime);
    }

    /* get and set function */
    public void setGenericOnOffState(int state) {
        if (DBG) Log.d(TAG, "setGenericOnOffState state " + state);
        this.genericOnOffState = state;
    }

    public int getGenericOnOffState() {
        if (DBG) Log.d(TAG, "getGenericOnOffState state " + this.genericOnOffState);
        return this.genericOnOffState;
    }

}
