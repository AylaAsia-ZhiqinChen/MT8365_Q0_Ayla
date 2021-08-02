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

public class CtlServerModel extends MeshModel {
    private static final String TAG = "CtlServerModel";
    private static final boolean DBG = MeshConstants.DEBUG;

    /** CTL Server States */
    public int lightCTLState;
    public int lightCTLDeltaUVState;
    public int lightCTLTemperatureRangeState;
    public int lightCTLTemperatureDefaultState;
    public int lightCTLDeltaUVDefaultState;
    public int lightCTLTemperatureState;
    public int genericLevelState;

    public CtlServerModel(BluetoothMesh meshInst) {
        super(meshInst, MeshConstants.MESH_MODEL_DATA_OP_ADD_MODEL);
    }

    public void setTxMessageHeader(int dstAddrType, int dst, int[] virtualUUID,
                        int src, int ttl, int netKeyIndex, int appKeyIndex, int msgOpCode) {
        if (DBG) Log.d(TAG, "setTxMessageHeader");
        super.setTxMessageHeader(dstAddrType, dst, virtualUUID, src, ttl, netKeyIndex, appKeyIndex, msgOpCode);
    }

    /**
    *CTL Server Messages
    * presentCTLLightness should be 2 octets 0x0000 ~ 0xFFFF
    * presentCTLTemperature should be 2 octets 0x0000 ~ 0xFFFF
    * targetCTLLightness should be 2 octets 0x0000 ~ 0xFFFF
    * targetCTLTemperature should be 2 octets 0x0000 ~ 0xFFFF
    * lightness should be 2 octets 0x0000 ~ 0xFFFF
    * temperature should be 2 octets 0x0000 ~ 0xFFFF
    * deltaUV should be 2 octets 0x0000 ~ 0xFFFF
    * rangeMin should be 2 octets 0x0000 ~ 0xFFFF
    * rangeMax should be 2 octets 0x0000 ~ 0xFFFF
    * others params should be 1 octets
    */
    public void lightCTLStatus(int presentCTLLightness, int presentCTLTemperature,
                    int targetCTLLightness, int targetCTLTemperature, int remainingTime){
        int[] params = new int[9];
        params[0] = super.TwoOctetsToArray(presentCTLLightness)[0];
        params[1] = super.TwoOctetsToArray(presentCTLLightness)[1];
        params[2] = super.TwoOctetsToArray(presentCTLTemperature)[0];
        params[3] = super.TwoOctetsToArray(presentCTLTemperature)[1];
        params[4] = super.TwoOctetsToArray(targetCTLLightness)[0];
        params[5] = super.TwoOctetsToArray(targetCTLLightness)[1];
        params[6] = super.TwoOctetsToArray(targetCTLTemperature)[0];
        params[7] = super.TwoOctetsToArray(targetCTLTemperature)[1];
        params[8] = remainingTime;
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightCTLTemperatureRangeStatus(int statusCode, int RangeMin, int RangeMax){
        int[] params = new int[5];
        params[0] = statusCode;
        params[1] = super.TwoOctetsToArray(RangeMin)[0];
        params[2] = super.TwoOctetsToArray(RangeMin)[1];
        params[3] = super.TwoOctetsToArray(RangeMax)[0];
        params[4] = super.TwoOctetsToArray(RangeMax)[1];
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }

    public void lightCTLDefaultStatus(int lightness, int temperature, int deltaUV){
        int[] params = new int[6];
        params[0] = super.TwoOctetsToArray(lightness)[0];
        params[1] = super.TwoOctetsToArray(lightness)[1];
        params[2] = super.TwoOctetsToArray(temperature)[0];
        params[3] = super.TwoOctetsToArray(temperature)[1];
        params[4] = super.TwoOctetsToArray(deltaUV)[0];
        params[5] = super.TwoOctetsToArray(deltaUV)[1];
        if (DBG) Log.d(TAG, "params:" + Arrays.toString(params));
        super.modelSendPacket(params);
    }
}


