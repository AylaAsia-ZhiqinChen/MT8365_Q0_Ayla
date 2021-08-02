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

public class HslClientModel extends MeshModel {
    private static final String TAG = "HslClientModel";
    private static final boolean DBG = MeshConstants.DEBUG;

    public HslClientModel(BluetoothMesh meshInst) {
        super(meshInst, MeshConstants.MESH_MODEL_DATA_OP_ADD_HSL_CLIENT);
    }

    public void setTxMessageHeader(int dstAddrType, int dst, int[] virtualUUID,
                        int src, int ttl, int netKeyIndex, int appKeyIndex, int msgOpCode) {
        if (DBG) Log.d(TAG, "setTxMessageHeader");
        super.setTxMessageHeader(dstAddrType, dst, virtualUUID, src, ttl, netKeyIndex, appKeyIndex, msgOpCode);
    }

    /** HSL Client Messages
    * hslLightness should be 2 octets 0x0000 ~ 0xFFFF
    * hslHue should be 2 octets 0x0000 ~ 0xFFFF
    * hslSaturation should be 2 octets 0x0000 ~ 0xFFFF
    * lightness should be 2 octets 0x0000 ~ 0xFFFF
    * hue should be 2 octets 0x0000 ~ 0xFFFF
    * saturation should be 2 octets 0x0000 ~ 0xFFFF
    * hueRangeMin should be 2 octets 0x0000 ~ 0xFFFF
    * hueRangeMax should be 2 octets 0x0000 ~ 0xFFFF
    * saturationRangeMin should be 2 octets 0x0000 ~ 0xFFFF
    * saturationRangeMax should be 2 octets 0x0000 ~ 0xFFFF
    * others params should be 1 octets
    */
    public void lightHSLGet() {
        super.modelSendPacket();
    }

    public void lightHSLSet(int hslLightness, int hslHue, int hslSaturation, int tid, int transitionTime, int delay) {
        int[] params = new int[9];
        params[0] = super.TwoOctetsToArray(hslLightness)[0];
        params[1] = super.TwoOctetsToArray(hslLightness)[1];
        params[2] = super.TwoOctetsToArray(hslHue)[0];
        params[3] = super.TwoOctetsToArray(hslHue)[1];
        params[4] = super.TwoOctetsToArray(hslSaturation)[0];
        params[5] = super.TwoOctetsToArray(hslSaturation)[1];
        params[6] = tid;
        params[7] = transitionTime;
        params[8] = delay;
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightHSLSetUnacknowledged(int hslLightness, int hslHue, int hslSaturation, int tid, int transitionTime, int delay) {
        lightHSLSet(hslLightness, hslHue, hslSaturation, tid, transitionTime, delay);
    }

    public void lightHSLTargetGet() {
        super.modelSendPacket();
    }

    public void lightHSLDefaultGet() {
        super.modelSendPacket();
    }

    public void lightHSLDefaultSet(int lightness, int hue, int saturation) {
        int[] params = new int[6];
        params[0] = super.TwoOctetsToArray(lightness)[0];
        params[1] = super.TwoOctetsToArray(lightness)[1];
        params[2] = super.TwoOctetsToArray(hue)[0];
        params[3] = super.TwoOctetsToArray(hue)[1];
        params[4] = super.TwoOctetsToArray(saturation)[0];
        params[5] = super.TwoOctetsToArray(saturation)[1];
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightHSLDefaultSetUnacknowledged(int lightness, int hue, int saturation) {
        lightHSLDefaultSet(lightness, hue, saturation);
    }

    public void lightHSLRangeGet() {
        super.modelSendPacket();
    }

    public void lightHSLRangeSet(int hueRangeMin, int hueRangeMax, int saturationRangeMin, int saturationRangeMax) {
        int[] params = new int[8];
        params[0] = super.TwoOctetsToArray(hueRangeMin)[0];
        params[1] = super.TwoOctetsToArray(hueRangeMin)[1];
        params[2] = super.TwoOctetsToArray(hueRangeMax)[0];
        params[3] = super.TwoOctetsToArray(hueRangeMax)[1];
        params[4] = super.TwoOctetsToArray(saturationRangeMin)[0];
        params[5] = super.TwoOctetsToArray(saturationRangeMin)[1];
        params[6] = super.TwoOctetsToArray(saturationRangeMax)[0];
        params[7] = super.TwoOctetsToArray(saturationRangeMax)[1];
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightHSLRangeSetUnacknowledged(int hueRangeMin, int hueRangeMax, int saturationRangeMin, int saturationRangeMax) {
        lightHSLRangeSet(hueRangeMin, hueRangeMax, saturationRangeMin, saturationRangeMax);
    }

    public void lightHSLHueGet(int hue, int tid, int transitionTime, int delay) {
        if (DBG) Log.d(TAG, "lightHSLHueGet");
        super.modelSendPacket();
    }

    public void lightHSLHueSet(int hue, int tid, int transitionTime, int delay) {
        int[] params = new int[5];
        params[0] = super.TwoOctetsToArray(hue)[0];
        params[1] = super.TwoOctetsToArray(hue)[1];
        params[2] = tid;
        params[3] = transitionTime;
        params[4] = delay;
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightHSLHueSetUnacknowledged(int hue, int tid, int transitionTime, int delay) {
        lightHSLHueSet(hue, tid, transitionTime, delay);
    }

    public void lightHSLSaturationGet() {
        super.modelSendPacket();
    }

    public void lightHSLSaturationSet(int saturation, int tid, int transitionTime, int delay) {
        int[] params = new int[5];
        params[0] = super.TwoOctetsToArray(saturation)[0];
        params[1] = super.TwoOctetsToArray(saturation)[1];
        params[2] = tid;
        params[3] = transitionTime;
        params[4] = delay;
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightHSLSaturationSetUnacknowledged(int saturation, int tid, int transitionTime, int delay) {
        lightHSLSaturationSet(saturation, tid, transitionTime, delay);
    }
}
