/*
 * Copyright (C) 2012 The Android Open Source Project
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

package com.android.bluetooth.mesh;

import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.IBluetooth;
import android.bluetooth.BluetoothMesh;
import android.bluetooth.IBluetoothMesh;
import android.bluetooth.mesh.BluetoothMeshAccessRxMessage;
import android.bluetooth.mesh.BluetoothMeshAccessTxMessage;
import android.bluetooth.mesh.MeshModel;
import android.bluetooth.mesh.IBluetoothMeshCallback;
import android.bluetooth.mesh.MeshInitParams;
import android.bluetooth.mesh.ConfigMessageParams;
import android.bluetooth.mesh.OtaOperationParams;

import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Handler;
import android.os.Message;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.provider.Settings;
import android.util.Log;

import com.android.bluetooth.btservice.AdapterService;
import com.android.bluetooth.btservice.ProfileService;
import com.android.bluetooth.Utils;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Arrays;

/**
 * Provides Bluetooth Mesh profile, as a service in
 * the Bluetooth application.
 * @hide
 */
public class MeshService extends ProfileService {
    private static final String TAG = "MeshService";
    private static final boolean DBG = false;

    private boolean mNativeAvailable;
    private static MeshService sMeshService;
    private BluetoothDevice mTargetDevice = null;
    private IBluetoothMeshCallback mCallback;
    private int meshRole = -1;
    private boolean meshState = false;

    private static final int MESSAGE_ENABLE = 1;
    private static final int MESSAGE_DISABLE = 2;
    private static final int MESSAGE_SET_MODEL_DATA = 3;
    private static final int MESSAGE_SET_NET_KEY = 4;
    private static final int MESSAGE_SET_APP_KEY = 5;
    private static final int MESSAGE_UNPROV_SCAN = 6;
    private static final int MESSAGE_INVITE_PROV = 7;
    private static final int MESSAGE_START_PROV = 8;
    private static final int MESSAGE_SET_PRVO_FACTOR = 9;
    private static final int MESSAGE_SET_MODEL_CC_TX_MSG = 10;
    private static final int MESSAGE_SEND_PACKET = 11;
    private static final int MESSAGE_RESET_DATA = 12;
    private static final int MESSAGE_SAVE_DATA = 13;
    private static final int MESSAGE_SHOW_VERSION = 14;
    private static final int MESSAGE_DUMP_DATA = 15;
    private static final int MESSAGE_GET_ELEMENT_ADDR = 16;
    private static final int MESSAGE_SET_DEFAULT_TTL = 17;
    private static final int MESSAGE_GET_DEFAULT_TTL = 18;
    private static final int MESSAGE_BIND_MODEL_APP = 19;
    private static final int MESSAGE_ACCESS_MODEL_REPLY = 20;
    private static final int MESSAGE_SET_LOG_LEVEL = 21;
    private static final int MESSAGE_GATT_CONNECT = 22;
    private static final int MESSAGE_GATT_DISCONNECT = 23;

    static {
        classInitNative();
    }

    public String getName() {
        return TAG;
    }

    public IProfileServiceBinder initBinder() {
        return new BluetoothMeshBinder(this);
    }

    protected boolean start() {
        Log.d(TAG, "starting Bluetooth MeshService");
        initializeNative();
        mNativeAvailable=true;
        setMeshService(this);
        return true;
    }

    protected boolean stop() {
        Log.d(TAG, "Stopping Bluetooth MeshService");
        if (meshState) {
            Log.d(TAG, "disable Mesh when stopping Bluetooth MeshService");
            disable();
        }
        meshState = false;
        meshRole = -1;
        return true;
    }

    @Override
    protected void cleanup() {
        Log.d(TAG, "cleanup Bluetooth MeshService");
        if (mNativeAvailable) {
            cleanupNative();
            mNativeAvailable=false;
        }
        clearMeshService();
    }

    public static synchronized MeshService getMeshService(){
        if (sMeshService != null && sMeshService.isAvailable()) {
            if (DBG) Log.d(TAG, "getMeshService(): returning " + sMeshService);
            return sMeshService;
        }
        if (sMeshService == null) {
            Log.w(TAG, "getMeshService(): service is NULL");
        } else if (!(sMeshService.isAvailable())) {
            Log.w(TAG,"getMeshService(): service is not available");
        }
        return null;
    }

    private static synchronized void setMeshService(MeshService instance) {
        if (instance != null && instance.isAvailable()) {
            if (DBG) Log.d(TAG, "setMeshService(): set to: " + sMeshService);
            sMeshService = instance;
        } else {
            if (sMeshService == null) {
                Log.w(TAG, "setMeshService(): service not available");
            } else if (!sMeshService.isAvailable()) {
                Log.w(TAG,"setMeshService(): service is cleaning up");
            }
        }
    }

    private static synchronized void clearMeshService() {
        sMeshService = null;
    }

    /**
     * Handlers for incoming service calls
     */
    private static class BluetoothMeshBinder extends IBluetoothMesh.Stub implements IProfileServiceBinder{
        private MeshService mService;
        public BluetoothMeshBinder(MeshService svc) {
            mService = svc;
        }

        @Override
        public void cleanup() {
            mService = null;
        }

        private MeshService getService() {
            if (!Utils.checkCaller()) {
                Log.w(TAG,"InputDevice call not allowed for non-active user");
                return null;
            }

            if (mService  != null && mService.isAvailable()) {
                return mService;
            }
            return null;
        }

        public void registerCallback (IBluetoothMeshCallback callback) {
            if (DBG) Log.d(TAG, "registerCallback");
            MeshService service = getService();
            if (service == null) return;
            service.registerCallback(callback);
        }

        public int getMeshRole(){
            if (DBG) Log.d(TAG, "getMeshRole");
            MeshService service = getService();
            if (service == null) return -1;
            return service.getMeshRole();
        }

        public boolean getMeshState(){
            if (DBG) Log.d(TAG, "getMeshState");
            MeshService service = getService();
            if (service == null) return false;
            return service.getMeshState();
        }

        public int enable(MeshInitParams params) {
            if (DBG) Log.d(TAG, "enable, role=" + params.getRole());
            MeshService service = getService();
            if (service == null) return -1;
            return service.enable(params);
        }

        public int disable() {
            if (DBG) Log.d(TAG, "disable");
            MeshService service = getService();
            if (service == null) return -1;
            return service.disable();
        }

        public boolean setCompositionDataHeader(int[] data) {
            if (DBG) Log.d(TAG, "setCompositionDataHeader, data=" + Arrays.toString(data));
            MeshService service = getService();
            if (service == null)
                return false;
            return service.setCompositionDataHeader(data);
        }

        public int addElement(int location) {
            if (DBG) Log.d(TAG, "addElement, location=" + location);
            MeshService service = getService();
            if (service == null)
                return -1;
            return service.addElement(location);
        }

        public boolean setElementAddr( int addr) {
            if (DBG) Log.d(TAG, "setElementAddr, addr=" + addr);
            MeshService service = getService();
            if (service == null)
                return false;
            return service.setElementAddr(addr);
        }

        public int addModel(MeshModel model) {
            if (DBG) Log.d(TAG, "addModel");
            if (DBG) Log.d(TAG, "modelopcode=" + model.getModelOpcode() + ",elementIndex=" + model.getElementIndex());
            MeshService service = getService();
            if (service == null) return -1;
            return service.addModel(model);
        }

        public int setNetkey(int op, int[] key, int netIndex) {
            if (DBG) Log.d(TAG, "setNetkey");
            MeshService service = getService();
            if (service == null) return 0;
            return service.setNetkey(op, key, netIndex);
        }

        public int setAppkey(int op, int[] key, int netIndex, int appIndex) {
            if (DBG) Log.d(TAG, "setAppkey");
            MeshService service = getService();
            if (service == null) return -1;
            return service.setAppkey(op, key, netIndex, appIndex);
        }


        public void unProvisionScan(boolean start, int duration) {
            if (DBG) Log.d(TAG, "unProvisionScan, start=" + start + ", duration=" + duration);
            MeshService service = getService();
            if (service == null) return;
            service.unProvisionScan(start, duration);
        }

        public int inviteProvisioning(int[] UUID, int attentionDuration) {
            if (DBG) Log.d(TAG, "inviteProvisioning, UUID=" + Arrays.toString(UUID) + ", attentionDuration=" + attentionDuration);
            MeshService service = getService();
            if (service == null) return -1;
            return service.inviteProvisioning(UUID, attentionDuration);
        }

        public int startProvisioning(int algorithm, int publicKey, int authMethod, int authAction,
                int authSize, int[] netkey, int netkeyIndex, long ivIndex, int addr, int flags, int mode) {
            if (DBG) Log.d(TAG, "startProvisioning");
            MeshService service = getService();
            if (service == null) return -1;
            return service.startProvisioning(algorithm, publicKey, authMethod, authAction, authSize,
              netkey, netkeyIndex, ivIndex, addr, flags, mode);
        }

        public int setProvisionFactor(int type, int[] buf) {
            if (DBG) Log.d(TAG, "setProvisionFactor, type=" + type + ", buf=" + Arrays.toString(buf));
            MeshService service = getService();
            if (service == null) return -1;
            return service.setProvisionFactor(type, buf);
        }

        public int sendConfigMessage(int dst, int src, int ttl, int netKeyIndex, int opcode,
                                                ConfigMessageParams param) {
            if (DBG) Log.d(TAG, "sendConfigMessage");
            MeshService service = getService();
            if (service == null) return -1;
            return service.sendConfigMessage(dst, src, ttl, netKeyIndex, opcode, param);
        }

        public int sendPacket(int dst, int src, int ttl, int netKeyIndex, int appKeyIndex, int[] payload) {
            if (DBG) Log.d(TAG, "sendPacket");
            MeshService service = getService();
            if (service == null) return -1;
            return service.sendPacket(dst, src, ttl, netKeyIndex, appKeyIndex, payload);
        }

        public int publishModel(int modelHandle, int opCode, int companyId, int[] buffer) {
            if (DBG) Log.d(TAG, "publishModel, modelHandle=" + modelHandle);
            MeshService service = getService();
            if (service == null) return -1;
            return service.publishModel(modelHandle, opCode, companyId, buffer);
        }

        public int setMeshMode(int mode) {
            if (DBG) Log.d(TAG, "setMeshMode, mode=" + mode);
            MeshService service = getService();
            if (service == null) return -1;
            return service.setMeshMode(mode);
        }

        public boolean resetData(int sector) {
            if (DBG) Log.d(TAG, "resetData");
            MeshService service = getService();
            if (service == null) return false;
            return service.resetData(sector);
        }

        public boolean saveData() {
            if (DBG) Log.d(TAG, "saveData");
            MeshService service = getService();
            if (service == null) return false;
            return service.saveData();
        }

        public void setData() {
        }

        public String getVersion() {
            if (DBG) Log.d(TAG, "getVersion");
            MeshService service = getService();
            if (service == null) return null;
            return service.getVersion();
        }

        public void dump(int type) {
            if (DBG) Log.d(TAG, "dump, type=" + type);
            MeshService service = getService();
            if (service == null) return;
            service.dump(type);
        }

        public int getElementAddr(int elementIndex) {
            if (DBG) Log.d(TAG, "getElementAddr, elementIndex=" + elementIndex);
            MeshService service = getService();
            if (service == null) return -1;
            return service.getElementAddr(elementIndex);
        }

        public void setDefaultTTL(int ttl) {
            if (DBG) Log.d(TAG, "setDefaultTTL, ttl=" + ttl);
            MeshService service = getService();
            if (service == null) return;
            service.setDefaultTTL(ttl);
        }

        public int getDefaultTTL() {
            if (DBG) Log.d(TAG, "setDefaultTTL");
            MeshService service = getService();
            if (service == null) return -1;
            return service.getDefaultTTL();
        }

        public int setIv(long ivIndex, int  ivPhase) {
            if (DBG) Log.d(TAG, "setIv ivIndex " + ivIndex + "ivPhase" + ivPhase);
            MeshService service = getService();
            if (service == null) return -1;
            return service.setIv(ivIndex, ivPhase);
        }

        public int addDevKey(int unicastAddr, int[] devicekey, int[] uuid)
        {
            if (DBG) Log.d(TAG, "addDevKeyivIndex ");
            MeshService service = getService();
            if (service == null) return -1;
            return service.addDevKey(unicastAddr, devicekey, uuid);
        }

        //the int array len should be 32, and first 16 is dev key, last 16 UUID
        public int[] getDevKey(int unicastAddr)
        {
            if (DBG) Log.d(TAG, "getDevKey ");
            MeshService service = getService();
            if (service == null) return null;
            return service.getDevKey(unicastAddr);
        }

        public int delDevKey(int unicastAddr)
        {
            if (DBG) Log.d(TAG, "delDevKey unicastAddr: " + unicastAddr);
            MeshService service = getService();
            if (service == null) return -1;
            return service.delDevKey(unicastAddr);
        }

        public int modelAppBind(int handle, int appIndex) {
            if (DBG) Log.d(TAG, "modelAppBind, handle=" + handle+ ", appIndex=" + appIndex);
            MeshService service = getService();
            if (service == null) return -1;
            return service.modelAppBind(handle, appIndex);
        }

        public int accessModelReply(int handle, BluetoothMeshAccessRxMessage msg, BluetoothMeshAccessTxMessage reply) {
            if (DBG) Log.d(TAG, "accessModelReply, handle=" + handle);
            MeshService service = getService();
            if (service == null) return -1;
            return service.accessModelReply(handle, msg, reply);
        }

        public int getModelHandle(long modelId, int elementIndex) {
            if (DBG) Log.d(TAG, "getModelHandle, modelId=" + modelId + ", elementIdx=" + elementIndex);
            MeshService service = getService();
            if (service == null) return -1;
            return service.getModelHandle(modelId, elementIndex);
        }

        public void setLogLevel(long level) {
            if (DBG) Log.d(TAG, "setLogLevel, level=" + level);
            MeshService service = getService();
            if (service == null) return;
            service.setLogLevel(level);
        }

        public int gattConnect(String address, int addressType, int serviceType) {
            if (DBG) Log.d(TAG, "gattConnect");
            //if (DBG) Log.d(TAG, "gattConnect, address=" + address + ", addressType=" +addressType, + ", serviceType=" + serviceType);
            MeshService service = getService();
            if (service == null) return -1;
            return service.gattConnect(address, addressType, serviceType);
        }

        public int gattDisconnect() {
            if (DBG) Log.d(TAG, "gattDisconnect");
            MeshService service = getService();
            if (service == null) return -1;
            return service.gattDisconnect();
        }

        public int setHeartbeatPeriod(int num, long hbTimeout) {
            if (DBG) Log.d(TAG, "setHeartbeatPeriod, num=" + num + ", hbTimeout=" + hbTimeout);
            MeshService service = getService();
            if (service == null) return -1;
            return service.setHeartbeatPeriod(num, hbTimeout);
        }

        public int[] otaGetClientModelHandle() {
            if (DBG) Log.d(TAG, "otaGetClientModelHandle");
            MeshService service = getService();
            if (service == null) return null;
            return service.otaGetClientModelHandle();
        }

        public int otaInitiatorOperation (OtaOperationParams params){
            if (DBG) Log.d(TAG, "otaInitiatorOperation");
            MeshService service = getService();
            if (service == null) return -1;
            return service.otaInitiatorOperation(params);
        }

        public int bearerAdvSetParams (long advPeriod, int minInterval, int maxInterval, int resend,
                                                       long scanPeriod, int scanInterval, int scanWindow){
            if (DBG) Log.d(TAG, "bearerAdvSetParams");
            MeshService service = getService();
            if (service == null) return -1;
            return service.bearerAdvSetParams(advPeriod, minInterval, maxInterval, resend, scanPeriod, scanInterval, scanWindow);
        }

        public int setScanParams (int scanInterval, int scanWindow){
            if (DBG) Log.d(TAG, "setScanParams");
            MeshService service = getService();
            if (service == null) return -1;
            return service.setScanParams(scanInterval, scanWindow);
        }

        public int setSpecialPktParams (boolean isSnIncrease, int snIncreaseInterval, int advInterval, int advPeriod){
            if (DBG) Log.d(TAG, "setSpecialPktParams");
            MeshService service = getService();
            if (service == null) return -1;
            return service.setSpecialPktParams(isSnIncrease, snIncreaseInterval, advInterval, advPeriod);
        }
    };

    public void registerCallback (IBluetoothMeshCallback callback) {
        mCallback = callback;
    }

    public int getMeshRole(){
        return meshRole;
    }

    public boolean getMeshState(){
        return meshState;
    }

    public int enable(MeshInitParams params){
        meshRole = params.getRole();
        return meshEnableNative(params);
    }

    public int disable(){
        int ret = meshDisableNative();
        if (ret == 0) {
            meshState = false;
            meshRole = -1;
        }
        return ret;
    }

    public int setNetkey(int op, int[] key, int netIndex) {
        return meshSetNetKeyNative(op, key, netIndex);
    }

    public int setAppkey(int op, int[] key, int netIndex, int appIndex) {
        return meshSetAppKeyNative(op, key, netIndex, appIndex);
    }

    public int setIv(long ivIndex, int  ivPhase) {
        return meshSetIvNative(ivIndex, ivPhase);
    }

    public int addDevKey(int unicastAddr, int[] devicekey, int[] uuid)
    {
        return meshAddDevkeyNative(unicastAddr, devicekey, uuid);
    }

    //the int array len should be 32, and first 16 is dev key, last 16 UUID
    public int[] getDevKey(int unicastAddr)
    {
        return meshGetDevKeyNative(unicastAddr);
    }

    public int delDevKey(int unicastAddr)
    {
        return meshDelDevKeyNative(unicastAddr);
    }


    public void unProvisionScan(boolean start, int duration) {
        meshUnProvDevScanNative(start, duration);
    }

    public int inviteProvisioning(int[] UUID, int attentionDuration) {
        return meshInviteProvisioningNative(UUID, attentionDuration);
    }

    public int startProvisioning(int algorithm, int publicKey, int authMethod, int authAction,
            int authSize, int[] netkey, int netkeyIndex, long ivIndex, int addr, int flags, int mode) {
        return meshStartProvisioningNative(algorithm, publicKey, authMethod, authAction, authSize,
          netkey, netkeyIndex, ivIndex, addr, flags,mode);
    }

    public int setProvisionFactor(int type, int[] buf) {
        return meshSetProvisionFactorNative(type, buf);
    }

    public boolean setCompositionDataHeader(int[] data) {
        return meshSetCompositionDataHeaderNative(data);
    }

    public int addElement(int location) {
        return meshAddElementNative(location);
    }

    public boolean setElementAddr(int addr) {
        return meshSetElementAddrNative(addr);
    }

    public int getElementAddr(int elementIndex) {
        return meshGetElementAddrNative(elementIndex);
    }

    public void setDefaultTTL(int ttl) {
        meshSetDefaultTTLNative(ttl);
    }

    public int getDefaultTTL() {
        return meshGetDefaultTTLNative();
    }

    public int modelAppBind(int handle, int appIndex) {

        return meshModelAppBindNative(handle, appIndex);
    }

    public int accessModelReply(int handle, BluetoothMeshAccessRxMessage msg, BluetoothMeshAccessTxMessage reply) {
        return meshAccessModelReplyNative(handle, msg, reply);
    }

    public int getModelHandle(long modelId, int elementIndex) {
        return meshGetModelHandleNative(modelId, elementIndex);
    }

    public int addModel(MeshModel model) {
       return meshAddModelNative(model);
    }

    public int sendConfigMessage(int dst, int src, int ttl, int netKeyIndex, int opcode,
                                            ConfigMessageParams param) {
        return meshSendConfigMessageNative(dst, src, ttl, netKeyIndex, opcode, param);
    }

    public int sendPacket(int dst, int src, int ttl, int netKeyIndex, int appKeyIndex, int[] payload) {
        return meshSendPacketNative(dst, src, ttl, netKeyIndex, appKeyIndex, payload);
    }

    public int publishModel(int modelHandle, int opCode, int companyId, int[] buffer) {
        return meshPublishModelNative(modelHandle, opCode, companyId, buffer);
    }

    public int setMeshMode(int mode) {
        return meshSetMeshModeNative(mode);
    }

    public boolean resetData(int sector) {
        return meshResetDataNative(sector);
    }

    public boolean saveData() {
        return meshSaveDataNative();
    }

    public String getVersion() {
        char[] versionArray = meshGetVersionNative();
        if (versionArray == null) return null;
        return String.valueOf(versionArray);
    }

    public void dump(int type) {
         meshDumpNative(type);
    }

    public void setLogLevel(long level) {
        meshSetLogLevelNative(level);
    }

    public int gattConnect(String address, int addressType, int serviceType) {
        byte[] addr = Utils.getBytesFromAddress(address);
        return meshGattConnectNative(addr, addressType, serviceType);
    }

    public int gattDisconnect() {
        return meshGattDisconnectNative();
    }

    public int setHeartbeatPeriod(int num, long hbTimeout) {
        return meshSetHeartbeatPeriodNative(num, hbTimeout);
    }

    public int[] otaGetClientModelHandle() {
        return meshOtaGetClientModelHandleNative();
    }

    public int otaInitiatorOperation (OtaOperationParams params){
        return meshOtaInitiatorOperationNative(params);
    }

    public int bearerAdvSetParams (long advPeriod, int minInterval, int maxInterval, int resend,
                                                    long scanPeriod, int scanInterval, int scanWindow){
        return meshBearerAdvSetParams(advPeriod, minInterval, maxInterval, resend, scanPeriod, scanInterval, scanWindow);
    }

    public int setScanParams (int scanInterval, int scanWindow){
        return meshSetScanParamsNative(scanInterval, scanWindow);
    }

    public int setSpecialPktParams (boolean isSnIncrease, int snIncreaseInterval, int advInterval, int advPeriod){
        return meshSetSpecialPktParams(isSnIncrease, snIncreaseInterval, advInterval, advPeriod);
    }

    /* Call back function start */
    public void onMeshEnabled() {
        if (DBG) Log.d(TAG, "onMeshEnabled");
        meshState = true;
        if (mCallback != null) {
            try {
                mCallback.onMeshEnabled();
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onConfigReset() {
        if (DBG) Log.d(TAG, "onConfigReset");
        if (mCallback != null) {
            try {
                mCallback.onConfigReset();
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onFriendShipStatus(int addr, int status) {
        if (DBG) Log.d(TAG, "onFriendShipStatus + addr " + addr + "staus " + status);
        if (mCallback != null) {
            try {
                mCallback.onFriendShipStatus(addr, status);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }


    /*
    * nodes_status is an array with element number of nodes_num, first int is addr, second int is status
    * nodes_status array length should be nodes_num * 2
    */
    public void onOTAEvent(int event_id, int error_code, long serial_number, long firmware_id, long time_escaped,
        int nodes_num, int curr_block, int total_block, int curr_chunk, int chunk_mask, int[] nodes_status) {
        if (DBG) Log.d(TAG, "onOTAEvent + event_id " + event_id + " error_code " + error_code + " nodes_num " + nodes_num +
            " curr_block " + curr_block + " total_block " + total_block + " curr_chunk " + curr_chunk + " chunk_mask " + chunk_mask);
        if (mCallback != null) {
            try {
                mCallback.onOTAEvent(event_id, error_code, serial_number, firmware_id, time_escaped, nodes_num,
                    curr_block, total_block, curr_chunk, chunk_mask, nodes_status);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onAdvReport(int addr_type, int[] addr, int rssi, int report_type, int[] data) {
        if (DBG) Log.d(TAG, "onAdvReport + addr_type " + addr_type +  " addr " + Arrays.toString(addr) + " rssi " + rssi
            + " report_type" + report_type + " data " + Arrays.toString(data));
        if (mCallback != null) {
            try {
                mCallback.onAdvReport(addr_type, addr, rssi, report_type, data);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onProvScanComplete() {
        if (DBG) Log.d(TAG, "onProvScanComplete");
        if (mCallback != null) {
            try {
                mCallback.onProvScanComplete();
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onProvScanResult(int[] uuid, int oobInfom, int[] uriHash) {
        if (DBG) Log.d(TAG, "onProvScanResult, uuid=" + Arrays.toString(uuid)
            + ", oobInfom=" + oobInfom + ", uriHash=" + Arrays.toString(uriHash));
        if (mCallback != null) {
            try {
                mCallback.onScanUnProvDevice(uuid, oobInfom, uriHash);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onProvCapabilities(int numberOfElements, int algorithms, int publicKeyType, int staticOOBType, int outputOobSize, int outputOobAction,
        int inputOobSize, int inputOobAction) {
        if (DBG) Log.d(TAG, "onProvCapabilities");
        if (mCallback != null) {
            try {
                mCallback.onProvCapabilities(numberOfElements, algorithms, publicKeyType, staticOOBType, outputOobSize, outputOobAction, inputOobSize, inputOobAction);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onRequestOobPublicKey() {
        if (DBG) Log.d(TAG, "onRequestOobPublicKey");
        if (mCallback != null) {
            try {
                mCallback.onRequestOobPublicKey();
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onRequestOobAuthValue(int method, int action, int size) {
        if (DBG) Log.d(TAG, "onRequestOobAuthValue method=" + method + ", action=" + action + ", size=" + size);
        if (mCallback != null) {
            try {
                mCallback.onRequestOobAuthValue(method, action, size);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onProvShowOobPublicKey(int[] public_key) {
        if (DBG) Log.d(TAG, "onProvShowOobPublicKey, public_key=" + Arrays.toString(public_key));
        if (mCallback != null) {
            try {
                mCallback.onProvShowOobPublicKey(public_key);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onProvShowOobAuthValue(int[] auth_key) {
        if (DBG) Log.d(TAG, "onProvShowOobAuthValue, auth_key=" + Arrays.toString(auth_key));
        if (mCallback != null) {
            try {
                mCallback.onProvShowOobAuthValue(auth_key);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onProvisionDone(int[] deviceKey, int address, boolean success, boolean gatt_bearer) {
        if (DBG) Log.d(TAG, "onProvisionDone, deviceKey=" + Arrays.toString(deviceKey)
            + ", address=" + address + ", success=" + success + ", gatt_bearer=" + gatt_bearer);
        if (mCallback != null) {
            try {
                mCallback.onProvDone(address, deviceKey, success, gatt_bearer);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onKeyRefresh(int netKeyIndex, int phase) {
        if (DBG) Log.d(TAG, "onKeyRefresh, netKeyIndex=" + netKeyIndex + ", phase=" + phase);
        if (mCallback != null) {
            try {
                mCallback.onKeyRefresh(netKeyIndex, phase);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onIvUpdate(int ivIndex, int state) {
        if (DBG) Log.d(TAG, "onIvUpdate, ivIndex=" + ivIndex + ", state=" + state);
        if (mCallback != null) {
            try {
                mCallback.onIvUpdate(ivIndex, state);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onSeqChange(int seqNumber) {
        if (DBG) Log.d(TAG, "onSeqChange, seqNumber=" + seqNumber);
        if (mCallback != null) {
            try {
                mCallback.onSeqChange(seqNumber);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onProvFactor(int type, int[] buf, int bufLen) {
        if (DBG) Log.d(TAG, "onProvFactor type=" + type + ", buf=" + Arrays.toString(buf));
        if (mCallback != null) {
            try {
                mCallback.onProvFactor(type, buf, bufLen);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onHeartbeat(int address, int active) {
        if (DBG) Log.d(TAG, "onHeartbeat address=" + address + ", active=" + active);
        if (mCallback != null) {
            try {
                mCallback.onHeartbeat(address, active);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onBearerGattStatus(long handle, int status) {
        if (DBG) Log.d(TAG, "onBearerGattStatus handle=" + handle + ", status=" + status);
        if (mCallback != null) {
            try {
                mCallback.onBearerGattStatus(handle, status);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onEvtErrorCode(int type) {
        if (DBG) Log.d(TAG, "onEvtErrorCode type=" + type);
        if (mCallback != null) {
            try {
                mCallback.onEvtErrorCode(type);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onOTAMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
        if (DBG) Log.d(TAG, "onOTAMsgHandler modelHandle=" + modelHandle);
        if (mCallback != null) {
            try {
                mCallback.onOTAMsgHandler(modelHandle, msg);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
        if (DBG) Log.d(TAG, "onMsgHandler modelHandle=" + modelHandle);
        if (mCallback != null) {
            try {
                mCallback.onMsgHandler(modelHandle, msg);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    public void onPublishTimeoutCallback(int modelHandle) {
        if (DBG) Log.d(TAG, "onPublishTimeoutCallback modelHandle=" + modelHandle);
        if (mCallback != null) {
            try {
                mCallback.onPublishTimeoutCallback(modelHandle);
            } catch (Exception e) {
                Log.e(TAG, "" + e);
            }
        }
    }

    BluetoothMeshAccessRxMessage CreateAccessMessageRxObject(int opcode, int company_id, int[] buf,
        int buf_len, int src_addr, int dst_addr, int appkey_index, int netkey_index, int rssi, int ttl) {

        return new BluetoothMeshAccessRxMessage(opcode, company_id, buf,
                buf_len, src_addr, dst_addr, appkey_index, netkey_index, rssi, ttl);
    }
    /* Call back function end */


    // Constants matching Hal header file bt_mesh.h
    private native static void classInitNative();
    private native void initializeNative();
    private native void cleanupNative();
    private native int meshEnableNative(MeshInitParams init_param);
    private native int meshDisableNative();
    private native int meshSetAppKeyNative(int op, int[] key, int netIndex, int appIndex);
    private native int meshSetNetKeyNative(int op, int[] key, int netIndex);
    private native void meshUnProvDevScanNative(boolean start, int duration);
    private native int meshInviteProvisioningNative(int[] UUID, int attentionDuration);
    private native int meshStartProvisioningNative(int algorithm, int public_key, int auth_method, int auth_action, int auth_size,
            int[] netkey, int netkey_index, long iv_index, int addr, int flags, int mode);
    private native int meshSetProvisionFactorNative(int type, int[] buf);
    private native boolean meshSetCompositionDataHeaderNative(int[] data);
    private native int meshAddElementNative(int location);
    private native boolean meshSetElementAddrNative(int addr);
    private native int meshAddModelNative(MeshModel model);
    private native int meshSendConfigMessageNative(int dst, int src, int ttl, int netKeyIndex, int opcode, ConfigMessageParams param);
    private native int meshSendPacketNative(int dst, int src, int ttl, int netKeyIndex, int appKeyIndex, int[] payload);
    private native int meshPublishModelNative(int modelHandle, int opCode, int companyId, int[] buffer);
    private native int meshSetMeshModeNative(int mode);
    private native boolean meshResetDataNative(int sector);
    private native boolean meshSaveDataNative();
    private native char[] meshGetVersionNative();
    private native void meshDumpNative(int type);
    private native int meshGetElementAddrNative(int element_index);
    private native void meshSetDefaultTTLNative(int ttl);
    private native int meshGetDefaultTTLNative();
    private native int meshSetIvNative(long ivIndex, int ivPhase);
    private native int meshAddDevkeyNative(int unicastAddr, int[] devicekey, int[] uuid);
    private native int[] meshGetDevKeyNative(int unicastAddr);
    private native int meshDelDevKeyNative(int unicastAddr);
    private native int meshModelAppBindNative(int handle, int appkey_index);
    private native int meshGetModelHandleNative(long modelId, int elementIndex);
    private native int meshAccessModelReplyNative(int handle, BluetoothMeshAccessRxMessage msg, BluetoothMeshAccessTxMessage reply);
    private native void meshSetLogLevelNative(long level);
    private native int meshGattConnectNative(byte[] address, int addressType, int serviceType);
    private native int meshGattDisconnectNative();
    private native int meshSetHeartbeatPeriodNative(int num, long hpTimeout);
    private native int[] meshOtaGetClientModelHandleNative();
    private native int meshOtaInitiatorOperationNative(OtaOperationParams params);
    private native int meshBearerAdvSetParams(long advPeriod, int minInterval, int maxInterval, int resend,
            long scanPeriod, int scanInterval, int scanWindow);
    private native int meshSetScanParamsNative(int scanInterval, int scanWindow);
    private native int meshSetSpecialPktParams(boolean isSnIncrease, int snIncreaseInterval, int advInterval, int advPeriod);
}
