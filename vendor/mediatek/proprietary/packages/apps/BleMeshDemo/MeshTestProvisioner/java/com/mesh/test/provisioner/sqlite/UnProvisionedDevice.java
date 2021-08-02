package com.mesh.test.provisioner.sqlite;

import java.util.Arrays;
import java.io.Serializable;
import com.mesh.test.provisioner.util.MeshUtils;

public class UnProvisionedDevice implements Serializable{

    private int mBearerType;
    private int[] mUUID;
    private String mAddress;
    private int mAddressType;
    private String mGattDevName;

    public UnProvisionedDevice(int mBearerType,int[] mUUID,String mAddress, int mAddrType, String mGattDevName) {
        this.mBearerType = mBearerType;
        this.mUUID = mUUID;
        this.mAddress = mAddress;
        this.mAddressType = mAddrType;
        this.mGattDevName = mGattDevName;
    }

    public int getBearerType() {
        return mBearerType;
    }

    public void setBearerType(int mBearerType) {
        this.mBearerType = mBearerType;
    }

    public int[] getUUID() {
        return mUUID;
    }

    public void setUUID(int[] mUUID) {
        this.mUUID = mUUID;
    }

    public String getAddress() {
        return mAddress;
    }

    public void setAddress(String mAddress) {
        this.mAddress = mAddress;
    }

    public int getAddressType() {
        return this.mAddressType;
    }

    public void setAddressType(int type) {
        this.mAddressType = type;
    }

    public String getGattDevName() {
        return mGattDevName;
    }

    public void setGattDevName(String name) {
        this.mGattDevName = name;
    }

    @Override
    public String toString() {
        return "UnProvisionedDevice [mBearerType=" + mBearerType + ", mUUID=" + MeshUtils.intArrayToString(mUUID, true) + ", mAddress="
                + mAddress + ", mGattDevName=" + mGattDevName + "]";
    }

}
