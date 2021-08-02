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

package android.bluetooth.mesh;

import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;
import android.bluetooth.BluetoothMesh;
import android.bluetooth.mesh.MeshConstants;

public class OtaOperationParams implements Parcelable {
    private static final String TAG = "BluetoothMesh_OtaOperationParams";
    private static final boolean DBG = MeshConstants.DEBUG;

    private int mOpcode;

    private int mNodeAddr;

    private byte[] mObjFile;
    private int mObjSize;
    private int[] mObjId;
    private long mFwId;
    private int mAppkeyIndex;
    private int mDistributorAddr;
    private int mGroupAddr;
    private int mUpdatersNum;
    private int[] mUpdaters;
    private boolean mManualApply;

    public OtaOperationParams() { }

    public OtaOperationParams(Parcel in) {
        this.mOpcode = in.readInt();
        this.mNodeAddr = in.readInt();
        this.mObjFile = in.createByteArray();
        this.mObjSize = in.readInt();
        this.mObjId = in.createIntArray();
        this.mFwId = in.readLong();
        this.mAppkeyIndex = in.readInt();
        this.mDistributorAddr = in.readInt();
        this.mGroupAddr = in.readInt();
        this.mUpdatersNum = in.readInt();
        this.mUpdaters = in.createIntArray();
        mManualApply = in.readInt() != 0 ? true : false;
    }

    public int describeContents() {
        return 0;
    }

    public static final Parcelable.Creator<OtaOperationParams> CREATOR =
            new Parcelable.Creator<OtaOperationParams>() {
        public OtaOperationParams createFromParcel(Parcel in) {
            return new OtaOperationParams(in);
        }
        public OtaOperationParams[] newArray(int size) {
            return new OtaOperationParams[size];
        }
    };

    public void writeToParcel(Parcel out, int flags) {
        out.writeInt(mOpcode);
        out.writeInt(mNodeAddr);
        out.writeByteArray(mObjFile);
        out.writeInt(mObjSize);
        out.writeIntArray(mObjId);
        out.writeLong(mFwId);
        out.writeInt(mAppkeyIndex);
        out.writeInt(mDistributorAddr);
        out.writeInt(mGroupAddr);
        out.writeInt(mUpdatersNum);
        out.writeIntArray(mUpdaters);
        out.writeInt(mManualApply ? 1 : 0);
    }

    public void setOtaInitiatorMsgHandler(int appKeyIndex) {
        this.mOpcode = MeshConstants.MESH_OTA_INITIATOR_OP_REG_MSG_HANDLER;
        this.mAppkeyIndex = appKeyIndex;
    }

    public void setOtaInitiatorFwInfoGet(int nodeAddr) {
        this.mOpcode = MeshConstants.MESH_OTA_INITIATOR_OP_FW_INFO_GET;
        this.mNodeAddr = nodeAddr;
    }

    public void setOtaInitiatorStopParams(long fwID, int distributorAddr) {
        this.mOpcode = MeshConstants.MESH_OTA_INITIATOR_OP_STOP_DISTRIBUTION;
        this.mFwId = fwID;
        this.mDistributorAddr = distributorAddr;
    }

    public void setOtaInitiatorStartParams(byte[] objFile, int objSize, int[] objId, long fwID, int appkeyIndex,  int distributorAddr,
        int groupAddr, int updatersNum, int[] updaters, boolean manualApply) {
        this.mOpcode = MeshConstants.MESH_OTA_INITIATOR_OP_START_DISTRIBUTION;
        this.mObjFile = objFile;
        this.mObjSize = objSize;
        this.mObjId = objId;
        this.mFwId = fwID;
        this.mAppkeyIndex= appkeyIndex;
        this.mDistributorAddr = distributorAddr;
        this.mGroupAddr = groupAddr;
        this.mUpdatersNum = updatersNum;
        this.mUpdaters = updaters;
        this.mManualApply = manualApply;
    }

    public void setOtaInitiatorApplyDistribution() {
        this.mOpcode = MeshConstants.MESH_OTA_OTA_INITIATOR_OP_APPLY_DISTRIBUTION;

    }

    public int getOpcode() {
        return this.mOpcode;
    }

    public int getNodeAddr() {
        return this.mNodeAddr;
    }

    public byte[] getObjFile() {
        return this.mObjFile;
    }

    public int getObjSize() {
        return this.mObjSize;
    }

    public int[] getObjId() {
        return this.mObjId;
    }

    public long getFwId() {
        return this.mFwId;
    }

    public int getAppkeyIndex() {
        return this.mAppkeyIndex;
    }

    public int getDistributorAddr() {
        return this.mDistributorAddr;
    }

    public int getGroupAddr() {
        return this.mGroupAddr;
    }

    public int getUpdatersNum() {
        return this.mUpdatersNum;
    }

    public int[] getUpdaters() {
        return this.mUpdaters;
    }

    public boolean getManualApply() {
        return this.mManualApply;
    }

}

