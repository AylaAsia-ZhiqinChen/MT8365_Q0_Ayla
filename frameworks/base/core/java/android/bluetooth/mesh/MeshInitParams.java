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
import android.bluetooth.BluetoothMesh;


public class MeshInitParams implements Parcelable {
    private int mRole;
    private int[] mProvisioneeParams;

    /*ConfigInitParams*/
    private int[] mDeviceUuid; //size: MESH_UUID_SIZE
    private int mOobInfo;
    private int mDefaultTtl;
    private byte[] mUri;

    private long mFeatureMask;
    private int[] mFriendInitParams;
    private int[] mCustomizeParams;


    public MeshInitParams(){ }

    public MeshInitParams(Parcel in){
        this.mRole = in.readInt();
        this.mProvisioneeParams = in.createIntArray();
        this.mDeviceUuid = in.createIntArray();
        this.mOobInfo = in.readInt();
        this.mDefaultTtl = in.readInt();
        this.mUri = in.createByteArray();
        this.mFeatureMask = in.readLong();
        this.mFriendInitParams = in.createIntArray();
        this.mCustomizeParams = in.createIntArray();
    }

    public int describeContents() {
        return 0;
    }

    public static final Parcelable.Creator<MeshInitParams> CREATOR =
            new Parcelable.Creator<MeshInitParams>() {
        public MeshInitParams createFromParcel(Parcel in) {
            return new MeshInitParams(in);
        }
        public MeshInitParams[] newArray(int size) {
            return new MeshInitParams[size];
        }
    };

    public void writeToParcel(Parcel out, int flags) {
        out.writeInt(mRole);
        out.writeIntArray(mProvisioneeParams);
        out.writeIntArray(mDeviceUuid);
        out.writeInt(mOobInfo);
        out.writeInt(mDefaultTtl);
        out.writeByteArray(mUri);
        out.writeLong(mFeatureMask);
        out.writeIntArray(mFriendInitParams);
        out.writeIntArray(mCustomizeParams);
    }

    public class ProvisioneeParams {
        private int mNumberOfElements;
        private int mAlgorithms;
        private int mPublicKeyType;
        private int mStaticOobType;
        private int mOutputOobSize;
        private int mOutputOobAction;
        private int mInputOobSize;
        private int mInputOobAction;

        public ProvisioneeParams (int numberOfElements, int algorithms, int publicKeyType,
            int staticOobType, int outputOobSize, int outputOobAction, int inputOobSize, int inputOobAction){
            this.mNumberOfElements = numberOfElements;
            this.mAlgorithms = algorithms;
            this.mPublicKeyType = publicKeyType;
            this.mStaticOobType = staticOobType;
            this.mOutputOobSize = outputOobSize;
            this.mOutputOobAction = outputOobAction;
            this.mInputOobSize = inputOobSize;
            this.mInputOobAction = inputOobAction;
        }

        public int getNumberOfElements() {
            return this.mNumberOfElements;
        }

        public int getAlgorithms() {
            return this.mAlgorithms;
        }

        public int getPublicKeyType() {
            return this.mPublicKeyType;
        }

        public int getStaticOobType() {
            return this.mStaticOobType;
        }

        public int getOutputOobSize() {
            return this.mOutputOobSize;
        }

        public int getOutputOobAction() {
            return this.mOutputOobAction;
        }

        public int getInputOobSize() {
            return this.mInputOobSize;
        }

        public int getInputOobAction() {
            return this.mInputOobAction;
        }
    }

    public class FriendInitParams {
        private int mLpnNumber;
        private int mQueueSize;
        private int mSubscriptionListSize;

        public FriendInitParams (int lpnNumber, int queueSize, int subscriptionListSize){
            this.mLpnNumber = lpnNumber;
            this.mQueueSize = queueSize;
            this.mSubscriptionListSize = subscriptionListSize;
        }

        public int getLpnNumber() {
            return this.mLpnNumber;
        }

        public int getQueueSize() {
            return this.mQueueSize;
        }

        public int getSubscriptionListSize() {
            return this.mSubscriptionListSize;
        }

    }

    public class CustomizeParams {
        private int mMaxRemoteNodeCnt;
        private int mSave2flash;

        public CustomizeParams (int maxRemoteNodeCnt, int save2flash){
            this.mMaxRemoteNodeCnt = maxRemoteNodeCnt;
            this.mSave2flash = save2flash;
        }

        public int getMaxRemoteNodeCnt() {
            return this.mMaxRemoteNodeCnt;
        }

        public int getSave2flash() {
            return this.mSave2flash;
        }
    }


    public void setRole(int role) {
        this.mRole = role;
    }

    public void setProvisioneeParams(ProvisioneeParams provisionee) {
        int[] param = new int[8];
        param[0] = provisionee.getNumberOfElements();
        param[1] = provisionee.getAlgorithms();
        param[2] = provisionee.getPublicKeyType();
        param[3] = provisionee.getStaticOobType();
        param[4] = provisionee.getOutputOobSize();
        param[5] = provisionee.getOutputOobAction();
        param[6] = provisionee.getInputOobSize();
        param[7] = provisionee.getInputOobAction();
        this.mProvisioneeParams = param;
    }

    /*setConfigInitParams*/
    public void setDeviceUuid(int[] deviceUuid) {
        this.mDeviceUuid = deviceUuid;
    }

    public void setOobInfo(int oobInfo) {
        this.mOobInfo = oobInfo;
    }

    public void setDefaultTtl(int defaultTtl) {
        this.mDefaultTtl = defaultTtl;
    }

    public void setUri(byte[] uri) {
        this.mUri = uri;
    }

    public void setFeatureMask(long featureMask) {
        this.mFeatureMask = featureMask;
    }

    public void setFriendInitParams(FriendInitParams friend) {
        int[] param = new int[3];
        param[0] = friend.getLpnNumber();
        param[1] = friend.getQueueSize();
        param[2] = friend.getSubscriptionListSize();
        this.mFriendInitParams = param;
    }

    public void setCustomizeParams(CustomizeParams customize) {
        int[] param = new int[2];
        param[0] = customize.getMaxRemoteNodeCnt();
        param[1] = customize.getSave2flash();
        this.mCustomizeParams = param;
    }


    public int getRole() {
        return this.mRole;
    }

    public int[] getProvisioneeParams() {
        return mProvisioneeParams;
    }


    /*getConfigInitParams*/
    public int[] getDeviceUuid() {
        return this.mDeviceUuid;
    }

    public int getOobInfo() {
        return this.mOobInfo;
    }

    public int getDefaultTtl() {
        return this.mDefaultTtl;
    }

    public byte[] getUri() {
        return this.mUri;
    }

    public long getFeatureMask() {
        return this.mFeatureMask;
    }

    public int[] getFriendInitParams() {
        return mFriendInitParams;
    }

    public int[] getCustomizeParams() {
        return mCustomizeParams;
    }

}

