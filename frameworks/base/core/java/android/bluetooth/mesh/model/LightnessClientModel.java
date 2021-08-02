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

public class LightnessClientModel extends MeshModel {
    private static final String TAG = "LightnessClientModel";
    private static final boolean DBG = MeshConstants.DEBUG;

    public LightnessClientModel(BluetoothMesh meshInst) {
        super(meshInst, MeshConstants.MESH_MODEL_DATA_OP_ADD_LIGHTNESS_CLIENT);
    }

    public void setTxMessageHeader(int dstAddrType, int dst, int[] virtualUUID,
                        int src, int ttl, int netKeyIndex, int appKeyIndex, int msgOpCode) {
        if (DBG) Log.d(TAG, "setTxMessageHeader");
        super.setTxMessageHeader(dstAddrType, dst, virtualUUID, src, ttl, netKeyIndex, appKeyIndex, msgOpCode);
    }

    /**
    * Lightness Client Messages
    * lightness should be 2 octets 0x0000 ~ 0xFFFF
    * rangeMin should be 2 octets 0x0000 ~ 0xFFFF
    * rangeMax should be 2 octets 0x0000 ~ 0xFFFF
    * others params should be 1 octets
    */
    public void lightLightnessGet() {
        super.modelSendPacket();
    }

    public void lightLightnessSet(int lightness, int tid, int transitionTime, int delay) {
        int[] params = new int[5];
        params[0] = super.TwoOctetsToArray(lightness)[0];
        params[1] = super.TwoOctetsToArray(lightness)[1];
        params[2] = tid;
        params[3] = transitionTime;
        params[4] = delay;
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightLightnessSetUnacknowledged(int lightness, int tid, int transitionTime, int delay) {
        lightLightnessSet(lightness, tid, transitionTime, delay);
    }

    public void lightLightnessLinearGet() {
        super.modelSendPacket();
    }

    public void lightLightnessLinearSet(int lightness, int tid, int transitionTime, int delay) {
        int[] params = new int[5];
        params[0] = super.TwoOctetsToArray(lightness)[0];
        params[1] = super.TwoOctetsToArray(lightness)[1];
        params[2] = tid;
        params[3] = transitionTime;
        params[4] = delay;
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightLightnessLinearSetUnacknowledged(int lightness, int tid, int transitionTime, int delay) {
        lightLightnessLinearSet(lightness, tid, transitionTime, delay);
    }

    public void lightLightnessLastGet() {
        super.modelSendPacket();
    }

    public void lightLightnessDefaultGet() {
        super.modelSendPacket();
    }

    public void lightLightnessDefaultSet(int lightness) {
        int[] params = new int[2];
        params[0] = super.TwoOctetsToArray(lightness)[0];
        params[1] = super.TwoOctetsToArray(lightness)[1];
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightLightnessDefaultSetUnacknowledged(int lightness) {
        lightLightnessDefaultSet(lightness);
    }

    public void lightLightnessRangeGet() {
        super.modelSendPacket();
    }

    public void lightLightnessRangeSet(int rangeMin, int rangeMax) {
        int[] params = new int[4];
        params[0] = super.TwoOctetsToArray(rangeMin)[0];
        params[1] = super.TwoOctetsToArray(rangeMin)[1];
        params[2] = super.TwoOctetsToArray(rangeMax)[0];
        params[3] = super.TwoOctetsToArray(rangeMax)[1];
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightLightnessRangeSetUnacknowledged(int rangeMin, int rangeMax) {
        lightLightnessRangeSet(rangeMin, rangeMax);
    }
}
