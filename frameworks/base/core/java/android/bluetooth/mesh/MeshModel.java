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
import android.bluetooth.mesh.BluetoothMeshAccessRxMessage;
import android.bluetooth.mesh.MeshConstants;
import android.bluetooth.mesh.ConfigMessageParams;

import java.util.Arrays;

public class MeshModel implements Parcelable {
    private static final String TAG = "BluetoothMesh_MeshModel";
    private static final boolean DBG = MeshConstants.DEBUG;
    private static final boolean VDBG = MeshConstants.VERBOSE;

    protected int mModelOpcode;
    protected int mModelHandle;
    protected int mElementIndex;
    protected long mModelID;

    protected int[] mVendorMsgOpcodes;
    protected int mCompanyID;
    protected int mOpcodeCount;

    protected BluetoothMesh mMesh;
    protected ModelTxMessage mTxMsg;
    protected ModelConfigMessage mConfigMsg;

    public MeshModel(BluetoothMesh meshInst) {
        this.mMesh = meshInst;
    }

    public MeshModel(BluetoothMesh meshInst, int modelOpcode) {
        this.mMesh = meshInst;
        this.mModelOpcode = modelOpcode;
    }

    public MeshModel(BluetoothMesh meshInst, int modelOpcode, int index) {
        this.mMesh = meshInst;
        this.mModelOpcode = modelOpcode;
        this.mElementIndex = index;
    }

    public MeshModel(Parcel in) {
        this.mModelOpcode = in.readInt();
        this.mModelHandle = in.readInt();
        this.mElementIndex = in.readInt();
        this.mModelID = in.readLong();
        this.mVendorMsgOpcodes = in.createIntArray();
        this.mCompanyID = in.readInt();
        this.mOpcodeCount = in.readInt();
    }

    public int describeContents() {
        return 0;
    }

    public static final Parcelable.Creator<MeshModel> CREATOR =
            new Parcelable.Creator<MeshModel>() {
        public MeshModel createFromParcel(Parcel in) {
            return new MeshModel(in);
        }
        public MeshModel[] newArray(int size) {
            return new MeshModel[size];
        }
    };

    public void writeToParcel(Parcel out, int flags) {
        out.writeInt(mModelOpcode);
        out.writeInt(mModelHandle);
        out.writeInt(mElementIndex);
        out.writeLong(mModelID);
        out.writeIntArray(mVendorMsgOpcodes);
        out.writeInt(mCompanyID);
        out.writeInt(mOpcodeCount);
    }

    /*meshif_tx_params_t*/
    public class ModelTxMessage {
        protected int mDstAddrType;
        protected int mDst;
        protected int[] mVirtualUUID;
        protected int mSrc;
        protected int mTtl;
        protected int mNetKeyIndex;
        protected int mAppKeyIndex;
        protected int mMsgOpCode;
    }

    /*meshif_configuration_msg_tx_t*/
    public class ModelConfigMessage {
        protected int mSrc;
        protected int mDst;
        protected int mTtl;
        protected int mNetKeyIndex;
        protected int mMsgOpCode;
    }

    /*setTxMessageHeader*/
    public void setTxMessageHeader(int dstAddrType, int dst, int[] virtualUUID,
                    int src, int ttl, int netKeyIndex, int appKeyIndex, int msgOpCode) {
        if (mTxMsg == null) {
            mTxMsg = new ModelTxMessage();
        }
        this.mTxMsg.mDstAddrType = dstAddrType;
        this.mTxMsg.mDst = dst;
        this.mTxMsg.mVirtualUUID = virtualUUID;
        this.mTxMsg.mSrc = src;
        this.mTxMsg.mTtl = ttl;
        this.mTxMsg.mNetKeyIndex = netKeyIndex;
        this.mTxMsg.mAppKeyIndex = appKeyIndex;
        this.mTxMsg.mMsgOpCode = msgOpCode;
    }

    /*setConfigMessageHeader*/
    public void setConfigMessageHeader(int src, int dst, int ttl, int netKeyIndex, int msgOpCode) {
        if (mConfigMsg == null) {
            this.mConfigMsg = new ModelConfigMessage();
        }
        this.mConfigMsg.mSrc = src;
        this.mConfigMsg.mDst = dst;
        this.mConfigMsg.mTtl = ttl;
        this.mConfigMsg.mNetKeyIndex = netKeyIndex;
        this.mConfigMsg.mMsgOpCode = msgOpCode;
    }

    /* common function for models to send packet*/
    protected void modelSendPacket() {
        modelSendPacket(null);
    }

    protected void modelSendPacket(int param1) {
        int[] params = new int[1];
        params[0] = param1;
        modelSendPacket(params);
    }

    protected void modelSendPacket(int param1, int param2) {
        int[] params = new int[2];
        params[0] = param1;
        params[1] = param2;
        modelSendPacket(params);
    }

    protected void modelSendPacket(int param1, int param2, int param3) {
        int[] params = new int[3];
        params[0] = param1;
        params[1] = param2;
        params[2] = param3;
        modelSendPacket(params);
    }

    protected void modelSendPacket(int param1, int param2, int param3, int param4) {
        int[] params = new int[4];
        params[0] = param1;
        params[1] = param2;
        params[2] = param3;
        params[3] = param4;
        modelSendPacket(params);
    }

    protected void modelSendPacket(int param1, int param2, int[] array) {
        int arrayLength = array==null?0:array.length;
        int paramsHalfLength = 2;
        int[] params = new int[paramsHalfLength+arrayLength];
        params[0] = param1;
        params[1] = param2;
        if (array != null) System.arraycopy(array, 0, params, paramsHalfLength, arrayLength);
        modelSendPacket(params);
    }

    protected void modelSendPacket(int[] params) {
        if (VDBG) Log.d(TAG, "modelSendPacket: params=" + Arrays.toString(params));
        if (this.mTxMsg == null){
            if (DBG) Log.e(TAG, "TxMsg is null, should create header first");
            return;
        }
        if (this.mMesh == null){
            if (DBG) Log.e(TAG, "BluetoothMesh is null, cannot send");
            return;
        }

        int paramsLength = params==null?0:params.length;
        int opcodeLength;
        int[] payload;
        /* 1-octet opcode*/
        if(this.mTxMsg.mMsgOpCode < 0x7F) {
            opcodeLength = 1;
            payload = new int[opcodeLength+paramsLength];
            payload[0] = this.mTxMsg.mMsgOpCode & 0x00FF;
            if (DBG) Log.d(TAG, "modelSendPacket  1-octet opcode = " + this.mTxMsg.mMsgOpCode);
        }
        /* 2-octet opcode*/
        else if(this.mTxMsg.mMsgOpCode > 0x7F && this.mTxMsg.mMsgOpCode < 0xC000) {
            opcodeLength = 2;
            payload = new int[opcodeLength+paramsLength];
            payload[0] = (this.mTxMsg.mMsgOpCode & 0xFF00) >> 8;
            payload[1] = this.mTxMsg.mMsgOpCode & 0x00FF;
            if (DBG) Log.d(TAG, "modelSendPacket  2-octet opcode = " + this.mTxMsg.mMsgOpCode);
        }
        /* 3-octet opcode*/
        else if (this.mTxMsg.mMsgOpCode >= 0xC000) {
            opcodeLength = 3;
            payload = new int[opcodeLength+paramsLength];
            payload[0] = (this.mTxMsg.mMsgOpCode & 0xFF0000) >> 16;
            payload[1] = (this.mCompanyID & 0x00FF);
            payload[2] = (this.mCompanyID & 0xFF00) >> 8;
            if (DBG) Log.d(TAG, "modelSendPacket  3-octet opcode = " + this.mTxMsg.mMsgOpCode);
        }
        else {
            payload = null;
            opcodeLength = 0;
            if (DBG) Log.d(TAG, "modelSendPacket  should never here!! ");
        }

        if ((params != null) && (payload != null))
            System.arraycopy(params, 0, payload, opcodeLength, params.length);
        this.mMesh.sendPacket(this.mTxMsg.mDst, this.mTxMsg.mSrc, this.mTxMsg.mTtl,
                    this.mTxMsg.mNetKeyIndex, this.mTxMsg.mAppKeyIndex, payload);
    }

    /* common function for models to send config message*/
    protected void modelSendConfigMessage() {
        if (this.mConfigMsg == null){
            if (DBG) Log.e(TAG, "TxMsg is null, should create header first");
            return;
        }
        if (this.mMesh == null){
            if (DBG) Log.e(TAG, "BluetoothMesh is null, cannot send");
            return;
        }
        this.mMesh.sendConfigMessage(this.mConfigMsg.mDst, this.mConfigMsg.mSrc,
                    this.mConfigMsg.mTtl, this.mConfigMsg.mNetKeyIndex, this.mConfigMsg.mMsgOpCode, null);
    }

    protected void modelSendConfigMessage(ConfigMessageParams param) {
        if (this.mConfigMsg == null){
            if (DBG) Log.e(TAG, "TxMsg is null, should create header first");
            return;
        }
        if (this.mMesh == null){
            if (DBG) Log.e(TAG, "BluetoothMesh is null, cannot send");
            return;
        }
        this.mMesh.sendConfigMessage(this.mConfigMsg.mDst, this.mConfigMsg.mSrc,
                    this.mConfigMsg.mTtl, this.mConfigMsg.mNetKeyIndex, this.mConfigMsg.mMsgOpCode, param);
    }

    /* split those two octets param to array for Mesh Models*/
    protected int[] TwoOctetsToArray(int src) {
        if (src > 0xFFFF || src < 0) {
            Log.w(TAG, "Param should be 0x0000~0xFFFF. Wrong param 0x" + Integer.toHexString(src)
                + ", will keep the last 2 bytes 0x" + Integer.toHexString(src&0xFFFF));
        }
        int[] dstArray = new int[2];
        dstArray[0] = src & 0x00FF;
        dstArray[1] = (src & 0xFF00) >> 8;
        return dstArray;
    }

    /* set/get function */
    public void setModelOpcode(int modelOpcode) {
        this.mModelOpcode = modelOpcode;
    }

    public void setElementIndex(int index) {
        this.mElementIndex = index;
    }

    public void setModelHandle(int handle) {
        this.mModelHandle = handle;
    }

    public void setModelID(long modelID) {
        this.mModelID = modelID;
    }

    public void setVendorMsgOpcodes(int[] opcodes) {
        this.mVendorMsgOpcodes = opcodes;
    }

    public void setCompanyID(int companyID) {
        this.mCompanyID = companyID;
    }

    public void setOpcodeCount(int opcodeCount) {
        this.mOpcodeCount = opcodeCount;
    }

    public int getModelOpcode() {
        return this.mModelOpcode;
    }

    public int getElementIndex() {
        return this.mElementIndex;
    }

    public int getModelHandle() {
        return this.mModelHandle;
    }

    public long getModelID() {
        return this.mModelID;
    }

    public int[] getVendorMsgOpcodes() {
        return this.mVendorMsgOpcodes;
    }

    public int getCompanyID() {
        return this.mCompanyID;
    }

    public int getOpcodeCount() {
        return this.mOpcodeCount;
    }

    /* callback function */
    /**
     * App shall override this method to handle RX messages to this model instance
     *
     */
    public void onMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
    }

    /**
     * App shall override this method to handle publish timeout callback to this model instance
     *
     */
    public void onPublishTimeoutCallback(int modelHandle) {
    }

}

