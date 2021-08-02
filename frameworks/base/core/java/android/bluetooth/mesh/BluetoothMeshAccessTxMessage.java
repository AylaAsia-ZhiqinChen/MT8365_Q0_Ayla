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
public final class BluetoothMeshAccessTxMessage implements Parcelable {
    /* access opcode */
    private static int mOpCode;
    private static int mCompanyId;
    private static int[] mBuffer;
    private static int mBufferLen;


    public BluetoothMeshAccessTxMessage() { }

    public BluetoothMeshAccessTxMessage(int opCode, int companyId, int[] buffer) {
        this.mOpCode = opCode;
        this.mCompanyId = companyId;
        this.mBuffer = buffer;
        this.mBufferLen = buffer.length;
    }

    public BluetoothMeshAccessTxMessage(int opCode, int companyId, int[] buffer, int bufferLength) {
        this.mOpCode = opCode;
        this.mCompanyId = companyId;
        this.mBuffer = buffer;
        this.mBufferLen = bufferLength;
    }

    public BluetoothMeshAccessTxMessage(Parcel in) {
        this.mOpCode = in.readInt();
        this.mCompanyId = in.readInt();
        this.mBuffer = in.createIntArray();
        this.mBufferLen = in.readInt();
    }

    public int describeContents() {
        return 0;
    }

    public static final Parcelable.Creator<BluetoothMeshAccessTxMessage> CREATOR =
            new Parcelable.Creator<BluetoothMeshAccessTxMessage>() {
        public BluetoothMeshAccessTxMessage createFromParcel(Parcel in) {
            return new BluetoothMeshAccessTxMessage(in);
        }
        public BluetoothMeshAccessTxMessage[] newArray(int size) {
            return new BluetoothMeshAccessTxMessage[size];
        }
    };

    public void writeToParcel(Parcel out, int flags) {
        out.writeInt(mOpCode);
        out.writeInt(mCompanyId);
        out.writeIntArray(mBuffer);
        out.writeInt(mBufferLen);
    }

    public void setAccessOpCode (int opCode, int companyId){
        this.mOpCode = opCode;
        this.mCompanyId = companyId;
    }

    public void setBuffer (int[] buffer, int bufferLength) {
        this.mBuffer = buffer;
        this.mBufferLen = bufferLength;
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

}