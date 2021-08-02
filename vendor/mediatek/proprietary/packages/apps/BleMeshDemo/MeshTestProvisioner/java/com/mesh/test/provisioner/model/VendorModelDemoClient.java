package com.mesh.test.provisioner.model;


import android.bluetooth.BluetoothMesh;
import android.bluetooth.mesh.MeshConstants;
import android.bluetooth.mesh.model.VendorModel;
import android.util.Log;

public class VendorModelDemoClient extends VendorModel {

    private static final boolean DEBUG = true;
    private static final String TAG = "VendorModelDemoClient";
    public static final int COMPANY_ID = 0x0046;    //For SIG model, this shall be 0xFFFF
    public static final long MODEL_ID = ((COMPANY_ID << 16) | 0x0001);

    //The actual vendor msg opcode is (VEDOR_MSG_OP << 16) | COMPANY_ID)
    public static final int VENDOR_MSG_OPCODE_STATE_STATUS = 0xC1;
    public static final int VENDOR_MSG_OPCODE_STATE_GET = 0xC2;
    public static final int VENDOR_MSG_OPCODE_STATE_SET = 0xC3;
    public static final int VENDOR_MSG_OPCODE_STATE_SET_UNACK = 0xC4;

    public static final int[] mSupportedRxMsg = {VENDOR_MSG_OPCODE_STATE_STATUS};

    private void log(String string) {
        if(DEBUG) {
            Log.i(TAG, string);
        }
    }

    public VendorModelDemoClient(BluetoothMesh meshInst) {
        super(meshInst);
    }

    public void setTxMessageHeader(int dstAddrType, int dst, int[] virtualUUID,
                        int src, int ttl, int netKeyIndex, int appKeyIndex, int msgOpCode) {
        log("setTxMessageHeader");
        if (this.COMPANY_ID != MeshConstants.MESH_MODEL_COMPANY_ID_NONE) {
            super.setTxMessageHeader(dstAddrType, dst, virtualUUID, src, ttl, netKeyIndex, appKeyIndex, (msgOpCode << 16 | COMPANY_ID));
        } else {
            super.setTxMessageHeader(dstAddrType, dst, virtualUUID, src, ttl, netKeyIndex, appKeyIndex, msgOpCode);
        }
    }

    public void vendorStateGet() {
        log("vendorStateGet");
        super.modelSendPacket();
    }

    public void vendorStateSet(int value) {
        log("vendorStateSet value = " + value);
        super.modelSendPacket(value);
    }

    public void vendorStateSetUnacknowledged(int value){
        log("vendorStateSetUnacknowledged value = " + value);
        super.modelSendPacket(value);
    }

}

