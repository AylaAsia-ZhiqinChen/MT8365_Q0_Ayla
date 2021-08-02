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

public class CtlClientModel extends MeshModel {
    private static final String TAG = "CtlClientModel";
    private static final boolean DBG = MeshConstants.DEBUG;

    public CtlClientModel(BluetoothMesh meshInst) {
        super(meshInst, MeshConstants.MESH_MODEL_DATA_OP_ADD_CTL_CLIENT);
    }

    public void setTxMessageHeader(int dstAddrType, int dst, int[] virtualUUID,
                        int src, int ttl, int netKeyIndex, int appKeyIndex, int msgOpCode) {
        if (DBG) Log.d(TAG, "setTxMessageHeader");
        super.setTxMessageHeader(dstAddrType, dst, virtualUUID, src, ttl, netKeyIndex, appKeyIndex, msgOpCode);
    }

    /**
    *CTL Client Messages
    * ctlLightness should be 2 octets 0x0000 ~ 0xFFFF
    * ctlTemperature should be 2 octets 0x0000 ~ 0xFFFF
    * ctlDeltaUV should be 2 octets 0x0000 ~ 0xFFFF
    * lightness should be 2 octets 0x0000 ~ 0xFFFF
    * temperature should be 2 octets 0x0000 ~ 0xFFFF
    * deltaUV should be 2 octets 0x0000 ~ 0xFFFF
    * rangeMin should be 2 octets 0x0000 ~ 0xFFFF
    * rangeMax should be 2 octets 0x0000 ~ 0xFFFF
    * others params should be 1 octets
    */
    public void lightCTLGet() {
        super.modelSendPacket();
    }

    public void lightCTLSet(int ctlLightness, int ctlTemperature, int ctlDeltaUV, int tid, int transitionTime, int delay) {
        int[] params = new int[9];
        params[0] = super.TwoOctetsToArray(ctlLightness)[0];
        params[1] = super.TwoOctetsToArray(ctlLightness)[1];
        params[2] = super.TwoOctetsToArray(ctlTemperature)[0];
        params[3] = super.TwoOctetsToArray(ctlTemperature)[1];
        params[4] = super.TwoOctetsToArray(ctlDeltaUV)[0];
        params[5] = super.TwoOctetsToArray(ctlDeltaUV)[1];
        params[6] = tid;
        params[7] = transitionTime;
        params[8] = delay;
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightCTLSetUnacknowledged(int ctlLightness, int ctlTemperature, int ctlDeltaUV, int tid, int transitionTime, int delay) {
        lightCTLSet(ctlLightness, ctlTemperature, ctlDeltaUV, tid, transitionTime, delay);
    }

    public void lightCTLTemperatureGet() {
        super.modelSendPacket();
    }

    public void lightCTLTemperatureSet(int ctlTemperature, int ctlDeltaUV, int tid, int transitionTime, int delay) {
        int[] params = new int[7];
        params[0] = super.TwoOctetsToArray(ctlTemperature)[0];
        params[1] = super.TwoOctetsToArray(ctlTemperature)[1];
        params[2] = super.TwoOctetsToArray(ctlDeltaUV)[0];
        params[3] = super.TwoOctetsToArray(ctlDeltaUV)[1];
        params[4] = tid;
        params[5] = transitionTime;
        params[6] = delay;
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightCTLTemperatureSetUnacknowledged(int ctlTemperature, int ctlDeltaUV, int tid, int transitionTime, int delay) {
        lightCTLTemperatureSet(ctlTemperature, ctlDeltaUV, tid, transitionTime, delay);
    }

    public void lightCTLDefaultGet() {
        super.modelSendPacket();
    }

    public void lightCTLDefaultSet(int lightness, int temperature, int deltaUV) {
        int[] params = new int[6];
        params[0] = super.TwoOctetsToArray(lightness)[0];
        params[1] = super.TwoOctetsToArray(lightness)[1];
        params[2] = super.TwoOctetsToArray(temperature)[0];
        params[3] = super.TwoOctetsToArray(temperature)[1];
        params[2] = super.TwoOctetsToArray(deltaUV)[0];
        params[3] = super.TwoOctetsToArray(deltaUV)[1];
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightCTLDefaultSetUnacknowledged(int lightness, int temperature, int deltaUV) {
        lightCTLDefaultSet(lightness, temperature, deltaUV);
    }

    public void lightCTLTemperatureRangeGet() {
        super.modelSendPacket();
    }

    public void lightCTLTemperatureRangeSet(int rangeMin, int rangeMax) {
        int[] params = new int[4];
        params[0] = super.TwoOctetsToArray(rangeMin)[0];
        params[1] = super.TwoOctetsToArray(rangeMin)[1];
        params[2] = super.TwoOctetsToArray(rangeMax)[0];
        params[3] = super.TwoOctetsToArray(rangeMax)[1];
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightCTLTemperatureRangeSetUnacknowledged(int rangeMin, int rangeMax) {
        lightCTLTemperatureRangeSet(rangeMin, rangeMax);
    }
}
