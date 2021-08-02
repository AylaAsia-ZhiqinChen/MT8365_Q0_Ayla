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

public class HslServerModel extends MeshModel {
    private static final String TAG = "HslServerModel";
    private static final boolean DBG = MeshConstants.DEBUG;

    /** HSL Server States */
    public int lightHSLState;
    public int lightHSLHueRangeState;
    public int lightHSLHueDefaultState;
    public int lightHSLSaturationRangeState;
    public int lightHSLSaturationDefaultState;
    public int lightHSLHueState;
    public int lightHSLSaturationState;
    public int genericLevelState;

    public HslServerModel(BluetoothMesh meshInst) {
        super(meshInst, MeshConstants.MESH_MODEL_DATA_OP_ADD_MODEL);
    }

    public void setTxMessageHeader(int dstAddrType, int dst, int[] virtualUUID,
                        int src, int ttl, int netKeyIndex, int appKeyIndex, int msgOpCode) {
        if (DBG) Log.d(TAG, "setTxMessageHeader");
        super.setTxMessageHeader(dstAddrType, dst, virtualUUID, src, ttl, netKeyIndex, appKeyIndex, msgOpCode);
    }

    /** HSL Server Messages
    * hslLightness should be 2 octets 0x0000 ~ 0xFFFF
    * hslHue should be 2 octets 0x0000 ~ 0xFFFF
    * hslSaturation should be 2 octets 0x0000 ~ 0xFFFF
    * hslLightnessTarget should be 2 octets 0x0000 ~ 0xFFFF
    * hslHueTarget should be 2 octets 0x0000 ~ 0xFFFF
    * hslSaturationTarget should be 2 octets 0x0000 ~ 0xFFFF
    * lightness should be 2 octets 0x0000 ~ 0xFFFF
    * hue should be 2 octets 0x0000 ~ 0xFFFF
    * saturation should be 2 octets 0x0000 ~ 0xFFFF
    * others params should be 1 octets
    */
    public void lightHSLStatus(int hslLightness, int hslHue, int hslSaturation, int remainingTime){
        int[] params = new int[7];
        params[0] = super.TwoOctetsToArray(hslLightness)[0];
        params[1] = super.TwoOctetsToArray(hslLightness)[1];
        params[2] = super.TwoOctetsToArray(hslHue)[0];
        params[3] = super.TwoOctetsToArray(hslHue)[1];
        params[4] = super.TwoOctetsToArray(hslSaturation)[0];
        params[5] = super.TwoOctetsToArray(hslSaturation)[1];
        params[6] = remainingTime;
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightHSLTargetStatus(int hslLightnessTarget, int hslHueTarget, int hslSaturationTarget, int remainingTime){
        int[] params = new int[7];
        params[0] = super.TwoOctetsToArray(hslLightnessTarget)[0];
        params[1] = super.TwoOctetsToArray(hslLightnessTarget)[1];
        params[2] = super.TwoOctetsToArray(hslHueTarget)[0];
        params[3] = super.TwoOctetsToArray(hslHueTarget)[1];
        params[4] = super.TwoOctetsToArray(hslSaturationTarget)[0];
        params[5] = super.TwoOctetsToArray(hslSaturationTarget)[1];
        params[6] = remainingTime;
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightHSLDefaultStatus(int lightness, int hue, int saturation){
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
}


