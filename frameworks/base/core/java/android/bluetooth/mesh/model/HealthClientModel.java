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
import java.util.Arrays;

public class HealthClientModel extends MeshModel {
    private static final String TAG = "HealthClientModel";
    private static final boolean DBG = MeshConstants.DEBUG;

    public HealthClientModel(BluetoothMesh meshInst) {
        super(meshInst, MeshConstants.MESH_MODEL_DATA_OP_ADD_HEALTH_CLIENT);
    }

    public void setTxMessageHeader(int dstAddrType, int dst, int[] virtualUUID,
                        int src, int ttl, int netKeyIndex, int appKeyIndex, int msgOpCode) {
        if (DBG) Log.d(TAG, "setTxMessageHeader");
        super.setTxMessageHeader(dstAddrType, dst, virtualUUID, src, ttl, netKeyIndex, appKeyIndex, msgOpCode);
    }

    /**
    * Health Client Messages
    * companyId should be 2 octets 0x0000 ~ 0xFFFF
    * others params should be 1 octets
    */
    public void healthFaultGet(int companyId) {
        int[] params = new int[2];
        params[0] = super.TwoOctetsToArray(companyId)[0];
        params[1] = super.TwoOctetsToArray(companyId)[1];
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void healthFaultClear(int companyId) {
        int[] params = new int[2];
        params[0] = super.TwoOctetsToArray(companyId)[0];
        params[1] = super.TwoOctetsToArray(companyId)[1];
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void healthFaultClearUnacknowledged(int companyId) {
        healthFaultClear(companyId);
    }

    public void healthFaultTest(int testId, int companyId) {
        int[] params = new int[3];
        params[0] = testId;
        params[1] = super.TwoOctetsToArray(companyId)[0];
        params[2] = super.TwoOctetsToArray(companyId)[1];
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void healthFaultTestUnacknowledged(int testId, int companyId) {
        healthFaultTest(testId, companyId);
    }

    public void healthPeriodGet() {
        super.modelSendPacket();
    }

    public void healthPeriodSet(int fastPeriodDivisor) {
        super.modelSendPacket(fastPeriodDivisor);
    }

    public void healthPeriodSetUnacknowledged(int fastPeriodDivisor) {
        healthPeriodSet(fastPeriodDivisor);
    }

    public void healthAttentionGet() {
        super.modelSendPacket();
    }

    public void healthAttentionSet(int attention) {
        super.modelSendPacket(attention);
    }

    public void healthAttentionSetUnacknowledged(int attention) {
        healthAttentionSet(attention);
    }
}
