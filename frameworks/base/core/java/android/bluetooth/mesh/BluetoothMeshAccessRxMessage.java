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

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;
import android.os.Parcel;
import android.os.Parcelable;

/**
 * Public Class for the Bluetooth MESH Message.
 *
 *
 */
public final class BluetoothMeshAccessRxMessage implements Parcelable {
    /* access opcode */
    private static int mOpCode;
    private static int mCompanyId;
    private static int[] mBuffer;
    private static int mBufferLen;

    /* meta data */
    private static int mSrcAddr;
    private static int mDstAddr;
    private static int mAppKeyIndex;
    private static int mNetKeyIndex;
    private static int mRssi;
    private static int mTtl;

    public BluetoothMeshAccessRxMessage() { }

    public BluetoothMeshAccessRxMessage(int opCode, int companyId, int[] buffer, int bufferLen,
            int srcAddr, int dstAddr, int appKeyIndex, int netKeyIndex, int rssi, int ttl) {
        this.mOpCode = opCode;
        this.mCompanyId = companyId;
        this.mBuffer = buffer;
        this.mBufferLen = bufferLen;
        this.mSrcAddr = srcAddr;
        this.mDstAddr = dstAddr;
        this.mAppKeyIndex = appKeyIndex;
        this.mNetKeyIndex = netKeyIndex;
        this.mRssi = rssi;
        this.mTtl = ttl;
    }

    public BluetoothMeshAccessRxMessage(Parcel in) {
        this.mOpCode = in.readInt();
        this.mCompanyId = in.readInt();
        this.mBuffer = in.createIntArray();
        this.mBufferLen = in.readInt();
        this.mSrcAddr = in.readInt();
        this.mDstAddr = in.readInt();
        this.mAppKeyIndex = in.readInt();
        this.mNetKeyIndex = in.readInt();
        this.mRssi = in.readInt();
        this.mTtl = in.readInt();
    }

    public int describeContents() {
        return 0;
    }

    public static final Parcelable.Creator<BluetoothMeshAccessRxMessage> CREATOR =
            new Parcelable.Creator<BluetoothMeshAccessRxMessage>() {
        public BluetoothMeshAccessRxMessage createFromParcel(Parcel in) {
            return new BluetoothMeshAccessRxMessage(in);
        }
        public BluetoothMeshAccessRxMessage[] newArray(int size) {
            return new BluetoothMeshAccessRxMessage[size];
        }
    };

    public void writeToParcel(Parcel out, int flags) {
        out.writeInt(mOpCode);
        out.writeInt(mCompanyId);
        out.writeIntArray(mBuffer);
        out.writeInt(mBufferLen);
        out.writeInt(mSrcAddr);
        out.writeInt(mDstAddr);
        out.writeInt(mAppKeyIndex);
        out.writeInt(mNetKeyIndex);
        out.writeInt(mRssi);
        out.writeInt(mTtl);
    }

    public void setAccessOpCode (int opCode, int companyId){
        this.mOpCode = opCode;
        this.mCompanyId = companyId;
    }

    public void setMetaData (int srcAddr, int dstAddr, int appKeyIndex, int netKeyIndex, int rssi, int ttl){
        this.mSrcAddr = srcAddr;
        this.mDstAddr = dstAddr;
        this.mAppKeyIndex = appKeyIndex;
        this.mNetKeyIndex = netKeyIndex;
        this.mRssi = rssi;
        this.mTtl = ttl;
    }

    public void setBuffer (int[] buffer) {
        this.mBuffer = buffer;
        this.mBufferLen = buffer.length;
    }

    public int getOpCode() {
        return this.mOpCode;
    }

    public int getCompanyId() {
        return this.mCompanyId;
    }

    public int[] getBuffer() {
        return this.mBuffer;
    }

    public int getSrcAddr() {
        return this.mSrcAddr;
    }

    public int getDstAddr() {
        return this.mDstAddr;
    }

    public int getAppKeyIndex() {
        return this.mAppKeyIndex;
    }

    public int getNetKeyIndex() {
        return this.mNetKeyIndex;
    }

    public int getRssi() {
        return this.mRssi;
    }

    public int getTtl() {
        return this.mTtl;
    }
}