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

public class HealthServerModel extends MeshModel {
    private static final String TAG = "HealthServerModel";
    private static final boolean DBG = MeshConstants.DEBUG;

    /** Health Server States */
    public int currentFaultState;
    public int registeredFaultState;
    public int healthPeriodState;
    public int attentionTimerState;

    public HealthServerModel(BluetoothMesh meshInst) {
        super(meshInst, MeshConstants.MESH_MODEL_DATA_OP_ADD_HEALTH_SERVER);
    }

    public void setTxMessageHeader(int dstAddrType, int dst, int[] virtualUUID,
                        int src, int ttl, int netKeyIndex, int appKeyIndex, int msgOpCode) {
        if (DBG) Log.d(TAG, "setTxMessageHeader");
        super.setTxMessageHeader(dstAddrType, dst, virtualUUID, src, ttl, netKeyIndex, appKeyIndex, msgOpCode);
    }

    /**
    * Health Server Messages
    * companyId should be 2 octets 0x0000 ~ 0xFFFF
    * others params should be 1 octets
    */
    public void healthCurrentStatus(int testId, int companyId, int[] faultArray) {
        int[] params = new int[2 + faultArray.length];
        params[0] = super.TwoOctetsToArray(companyId)[0];
        params[1] = super.TwoOctetsToArray(companyId)[1];
        for (int i=0; i<faultArray.length; i++) {
            params[i+2] = faultArray[i];
        }
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void healthFaultStatus(int testId, int companyId, int[] faultArray) {
        int[] params = new int[2 + faultArray.length];
        params[0] = super.TwoOctetsToArray(companyId)[0];
        params[1] = super.TwoOctetsToArray(companyId)[1];
        for (int i=0; i<faultArray.length; i++) {
            params[i+2] = faultArray[i];
        }
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void healthPeriodStatus(int fastPeriodDevisor) {
        super.modelSendPacket(fastPeriodDevisor);
    }

    public void healthAttentionStatus(int attention) {
        super.modelSendPacket(attention);
    }

    /* get and set function */
    public void setCurrentFaultState(int state) {
        this.currentFaultState = state;
    }

    public void setRegisteredFaultState(int state) {
        this.registeredFaultState = state;
    }

    public void setHealthPeriodState(int state) {
        this.healthPeriodState = state;
    }

    public void setAttentionTimerState(int state) {
        this.attentionTimerState = state;
    }

    public int getCurrentFaultState() {
        return this.currentFaultState;
    }

    public int getRegisteredFaultState() {
        return this.registeredFaultState;
    }

    public int getHealthPeriodState() {
        return this.healthPeriodState;
    }

    public int getAttentionTimerState() {
        return this.attentionTimerState;
    }

}
