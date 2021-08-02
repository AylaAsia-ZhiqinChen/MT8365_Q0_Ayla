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

package android.bluetooth;


import android.annotation.SdkConstant;
import android.annotation.SdkConstant.SdkConstantType;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Binder;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.HashMap;
import java.util.Arrays;

import android.bluetooth.BluetoothProfile;

import android.bluetooth.IBluetooth;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.IBluetoothManager;
import android.bluetooth.IBluetoothManagerCallback;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.IBluetoothStateChangeCallback;
import android.bluetooth.IBluetoothMesh;

import android.bluetooth.mesh.BluetoothMeshAccessRxMessage;
import android.bluetooth.mesh.BluetoothMeshAccessTxMessage;
import android.bluetooth.mesh.MeshModel;
import android.bluetooth.mesh.BluetoothMeshCallback;
import android.bluetooth.mesh.IBluetoothMeshCallback;
import android.bluetooth.mesh.MeshInitParams;
import android.bluetooth.mesh.MeshConstants;
import android.bluetooth.mesh.ConfigMessageParams;
import android.bluetooth.mesh.OtaOperationParams;

import android.bluetooth.le.ScanResult;


/**
 * Public API for the Bluetooth MESH Profile.
 *
 * <p>This class provides Bluetooth MESH functionality to enable communication
 * with Bluetooth devices.
 *
 */
public final class BluetoothMesh implements BluetoothProfile {
    private static final String TAG = "BluetoothMesh";
    private static final boolean DBG = MeshConstants.DEBUG;
    private static final boolean VDBG = MeshConstants.VERBOSE;

    private Context mContext;
    private BluetoothAdapter mAdapter;
    private ServiceListener mServiceListener;
    private volatile IBluetoothMesh mService;
    private volatile static BluetoothMesh sInstance;
    private BluetoothMeshCallback mCallback;
    private Map<Integer, MeshModel> mModelMap = new HashMap<>();


    final private IBluetoothStateChangeCallback mBluetoothStateChangeCallback =
            new IBluetoothStateChangeCallback.Stub() {
                public void onBluetoothStateChange(boolean up) {
                    if (DBG) Log.d(TAG, "onBluetoothStateChange: up=" + up);
                    if (!up) {
                        if (VDBG) Log.d(TAG,"Unbinding service...");
                        synchronized (mConnection) {
                            try {
                                mService = null;
                                mContext.unbindService(mConnection);
                            } catch (Exception re) {
                                Log.e(TAG,"",re);
                            }
                        }
                    } else {
                        synchronized (mConnection) {
                            try {
                                if (mService == null) {
                                    if (VDBG) Log.d(TAG,"Binding service...");
                                    doBind();
                                }
                            } catch (Exception re) {
                                Log.e(TAG,"",re);
                            }
                        }
                    }
                }
        };

    /**
     * Bluetooth Mesh callbacks. Overrides the default BluetoothMeshCallback implementation.
     */
    private final IBluetoothMeshCallback mBluetoothMeshCallback =
        new IBluetoothMeshCallback.Stub() {
                @Override
                public void onMeshEnabled() {
                    if (DBG) Log.d(TAG,"onMeshEnabled");
                    if (mCallback != null) mCallback.onMeshEnabled();
                }

                @Override
                public void onConfigReset() {
                    if (DBG) Log.d(TAG,"onConfigReset");
                    if (mCallback != null) mCallback.onConfigReset();
                }

                @Override
                public void onFriendShipStatus(int addr, int status) {
                    if (DBG) Log.d(TAG, "onFriendShipStatus addr=" + addr + ", staTus=" + status);
                    if (mCallback != null) mCallback.onFriendShipStatus(addr, status);
                }

                @Override
                public void onOTAEvent(int eventId, int errorCode, long serialNumber, long firmwareId, long timeEscaped,
                    int nodesNum, int curr_block, int total_block, int curr_chunk, int chunk_mask, int[] nodesStatus) {
                    if (DBG) Log.d(TAG, "onOTAEvent eventId=" + eventId + ", errorCode=" + errorCode + ", nodesNum=" + nodesNum +
                        ",curr_block=" + curr_block + ",total_block=" + total_block + ",curr_chunk=" + curr_chunk + ",chunk_mask =" + chunk_mask);
                    if (mCallback != null) {
                        mCallback.onOTAEvent(eventId, errorCode, serialNumber, firmwareId, timeEscaped, nodesNum, curr_block, total_block, curr_chunk, chunk_mask, nodesStatus);
                    }
                }

                @Override
                public void onAdvReport(int addrType, int[] addr, int rssi, int reportType, int[] data) {
                    if (DBG) Log.d(TAG,"onAdvReport addrType=" + addrType + ", addr=" + Arrays.toString(addr) +
                        ", rssi=" + rssi + ", reportType=" + reportType + ", data=" + Arrays.toString(addr));
                    if (mCallback != null) {
                        mCallback.onAdvReport(addrType, addr, rssi, reportType, data);
                    }
                }

                @Override
                public void onProvScanComplete() {
                    if (DBG) Log.d(TAG,"onProvScanComplete");
                    if (mCallback != null) mCallback.onProvScanComplete();
                }

                @Override
                public void onScanUnProvDevice(int[] uuid, int oobInfom, int[] uriHash) {
                    if (DBG) Log.d(TAG,"onScanUnProvDevice uuid=" + Arrays.toString(uuid) + ", oobInfom=" + oobInfom + ", uriHash=" + Arrays.toString(uriHash));
                    if (mCallback != null) mCallback.onScanUnProvDevice(uuid, oobInfom, uriHash);
                }

                @Override
                public void onProvCapabilities(int numberOfElements, int algorithms, int publicKeyType,
                                    int staticOOBType, int outputOobSize, int outputOobAction,
                                    int inputOobSize, int inputOobAction) {
                    if (DBG) Log.d(TAG,"onProvCapabilities numberOfElements=" + numberOfElements +
                        ", algorithms=" + algorithms + ", publicKeyType=" + publicKeyType +
                        ", staticOOBType=" + staticOOBType + ", outputOobSize=" + outputOobSize +
                        ", outputOobAction=" + outputOobAction + ", inputOobSize=" + inputOobSize +
                        ", inputOobAction=" + inputOobAction);
                    if (mCallback != null) mCallback.onProvCapabilities(numberOfElements, algorithms,
                                    publicKeyType, staticOOBType, outputOobSize, outputOobAction,
                                    inputOobSize, inputOobAction);
                }

                @Override
                public void onRequestOobPublicKey() {
                    if (DBG) Log.d(TAG,"onRequestOobPublicKey");
                    if (mCallback != null) mCallback.onRequestOobPublicKey();
                }

                @Override
                public void onRequestOobAuthValue(int method, int action, int size) {
                    if (DBG) Log.d(TAG,"onRequestOobAuthValue method=" + method + ", action=" + action + ", size=" + size);
                    if (mCallback != null) mCallback.onRequestOobAuthValue(method, action, size);
                }

                @Override
                public void onProvShowOobPublicKey(int[] publicKey) {
                    if (DBG) Log.d(TAG,"onProvShowOobPublicKey publicKey" + Arrays.toString(publicKey));
                    if (mCallback != null) mCallback.onProvShowOobPublicKey(publicKey);
                }

                @Override
                public void onProvShowOobAuthValue(int[] authValue) {
                    if (DBG) Log.d(TAG,"onProvShowOobAuthValue authValue =" + Arrays.toString(authValue));
                    if (mCallback != null) mCallback.onProvShowOobAuthValue(authValue);
                }

                @Override
                public void onProvDone(int address, int[] deviceKey, boolean success, boolean gattBearer) {
                    if (DBG) Log.d(TAG,"onProvDone address=" + address + ", deviceKey=" + Arrays.toString(deviceKey) +
                        ", success=" + success + ", gattBearer=" + gattBearer);
                    if (mCallback != null) mCallback.onProvDone(address, deviceKey, success, gattBearer);
                }

                @Override
                public void onScanResult(ScanResult scanResult) {
                    if (DBG) Log.d(TAG,"onScanResult scanResult=" + scanResult.toString());
                    if (mCallback != null) mCallback.onScanResult(scanResult);
                }

                @Override
                public void onKeyRefresh(int netKeyIndex, int phase) {
                    if (DBG) Log.d(TAG,"onKeyRefresh netKeyIndex=" + netKeyIndex + ", phase=" + phase);
                    if (mCallback != null) mCallback.onKeyRefresh(netKeyIndex, phase);
                }

                @Override
                public void onIvUpdate(int ivIndex, int state) {
                    if (DBG) Log.d(TAG,"onIvUpdate ivIndex=" + ivIndex + ", state=" + state);
                    if (mCallback != null) mCallback.onIvUpdate(ivIndex, state);
                }

                @Override
                public void onSeqChange(int seqNumber) {
                    if (DBG) Log.d(TAG,"onSeqChange seqNumber=" + seqNumber);
                    if (mCallback != null) mCallback.onSeqChange(seqNumber);
                }

                @Override
                public void onProvFactor(int type, int[] buf, int bufLen) {
                    if (DBG) Log.d(TAG,"onProvFactor type=" + type + ", buf[0]=" + buf[0]);
                    if (mCallback != null) mCallback.onProvFactor(type, buf, bufLen);
                }

                @Override
                public void onHeartbeat(int address, int active) {
                    if (DBG) Log.d(TAG,"onHeartbeat address=" + address + ", active=" + active);
                    if (mCallback != null) mCallback.onHeartbeat(address, active);
                }

                @Override
                public void onBearerGattStatus(long handle, int status) {
                    if (DBG) Log.d(TAG,"onBearerGattStatus: handle=" + handle + " status=" + status);
                    if (mCallback != null) mCallback.onBearerGattStatus(handle, status);
                }

                @Override
                public void onEvtErrorCode(int type) {
                    if (DBG) Log.d(TAG,"onEvtErrorCode: type=" + type);
                    if (mCallback != null) mCallback.onEvtErrorCode(type);
                }

                @Override
                public void onOTAMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
                    if (DBG) Log.d(TAG,"onOTAMsgHandler" + modelHandle);
                    if (mCallback != null) mCallback.onOTAMsgHandler(modelHandle, msg);
                }

                @Override
                public void onMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
                    if (DBG) Log.d(TAG,"onMsgHandler" + modelHandle);
                    MeshModel mModel = mModelMap.get(modelHandle);
                    if (mModel != null) mModel.onMsgHandler(modelHandle, msg);
                }

                @Override
                public void onPublishTimeoutCallback(int modelHandle) {
                    if (DBG) Log.d(TAG,"onPublishTimeoutCallback" + modelHandle);
                    MeshModel mModel = mModelMap.get(modelHandle);
                    if (mModel != null) mModel.onPublishTimeoutCallback(modelHandle);
                }
        };

    /**
     * Create a BluetoothMesh proxy object
     */
    /*package*/BluetoothMesh(Context context, ServiceListener listener) {
        mContext = context;
        mServiceListener = listener;
        mAdapter = BluetoothAdapter.getDefaultAdapter();

        IBluetoothManager mgr = mAdapter.getBluetoothManager();
        if (mgr != null) {
            try {
               mgr.registerStateChangeCallback(mBluetoothStateChangeCallback);
            } catch (RemoteException e) {
               Log.e(TAG,"",e);
            }
        }
        if (isBluetoothEnabled()) {
            if (DBG) Log.d(TAG,"bluetooth state is ON, do bind MeshService...");
            doBind();
        }
    }

    /**
     * Get the singleton instance of the BluetoothMesh.
     *
     * @param context the context
     * @param listener ServiceListener which listens mesh service connected and disconnected
     * @return the BluetoothMesh object
     */
    public static BluetoothMesh getDefaultMesh(Context context, ServiceListener listener) {
        if (context == null || listener == null) return null;
        if (sInstance == null) {
            synchronized (BluetoothMesh.class) {
                if (sInstance == null) {
                    sInstance = new BluetoothMesh(context, listener);
                }
            }
        }
        return sInstance;
    }

    boolean doBind() {
        Intent intent = new Intent(IBluetoothMesh.class.getName());
        ComponentName comp = intent.resolveSystemService(mContext.getPackageManager(), 0);
        intent.setComponent(comp);
        if (comp == null || !mContext.bindServiceAsUser(intent, mConnection, 0,
                android.os.Process.myUserHandle())) {
            Log.e(TAG, "Could not bind to Bluetooth MESH Service with " + intent);
            return false;
        }
        return true;
    }

    public void close() {
        if (VDBG) Log.d(TAG, "close()");
        IBluetoothManager mgr = mAdapter.getBluetoothManager();
        if (mgr != null) {
            try {
                mgr.unregisterStateChangeCallback(mBluetoothStateChangeCallback);
            } catch (Exception e) {
                Log.e(TAG,"",e);
            }
        }

        synchronized (mConnection) {
            if (mService != null) {
                try {
                    mService = null;
                    if (mContext == null) {
                        if (DBG) Log.d(TAG, "Context is null");
                    } else {
                        mContext.unbindService(mConnection);
                    }
                } catch (Exception re) {
                    Log.e(TAG,"",re);
                }
           }
        }
        mContext = null;
        mServiceListener = null;
        sInstance = null;
    }

    private final ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            if (DBG) Log.d(TAG, "Proxy object connected");
            mService = IBluetoothMesh.Stub.asInterface(Binder.allowBlocking(service));
            if (mServiceListener != null) {
                mServiceListener.onServiceConnected(BluetoothProfile.MESH, BluetoothMesh.this);
            }
        }
        public void onServiceDisconnected(ComponentName className) {
            if (DBG) Log.d(TAG, "Proxy object disconnected");
            mService = null;
            if (mServiceListener != null) {
                mServiceListener.onServiceDisconnected(BluetoothProfile.MESH);
            }
        }
    };
    /**
     * Bluetooth MESH interface callbacks
     */


    /**
     * Register an application callback to start using MESH.
     *
     * @param callback to handle the callbacks from framework
     * @return If true, the callback will be called to notify success or failure,
     *         false on immediate error
     */
	public boolean registerCallback(BluetoothMeshCallback callback) {
        if (DBG) Log.d(TAG, "registerCallback()");
        if (mService == null) return false;

        mCallback = callback;

        try {
            mService.registerCallback(mBluetoothMeshCallback);
        } catch (RemoteException e) {
            Log.e(TAG,"",e);
            return false;
        }

        return true;
    }

    /**
     * {@inheritDoc}
     */
    public List<BluetoothDevice> getConnectedDevices() {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public List<BluetoothDevice> getDevicesMatchingConnectionStates(int[] states) {
        return null;
    }

    /**
     * {@inheritDoc}
     */
    public int getConnectionState(BluetoothDevice device) {
        return 0;
    }

    /**
     * Get current mesh role.
     * <p>Possible return values are
     * {MeshConstants #MESH_ROLE_PROVISIONEE},
     * {MeshConstants #MESH_ROLE_PROVISIONER},
     * {-1}
     *
     * @return current mesh role: 0 provisionee, 1 provisioner, -1 none.
     */
    public int getMeshRole(){
        if (DBG) Log.d(TAG, "getMeshRole");
        try {
            if (mService != null && isBluetoothEnabled()) {
                int role = mService.getMeshRole();
                if (DBG) Log.d(TAG, "getMeshRole role=" + role);
                return role;
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Get current mesh enable state.
     *
     * @return current state of Mesh: true enabled, false disabled
     */
    public boolean getMeshState(){
        if (DBG) Log.d(TAG, "getMeshState");
        try {
            if (mService != null && isBluetoothEnabled()) {
                boolean state = mService.getMeshState();
                if (DBG) Log.d(TAG, "getMeshState state=" + state);
                return state;
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return false;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return false;
        }
    }

    /**
     * Enable Mesh.
     *
     * @param params init params for enable mesh
     * @return status: 0 if enable mesh called to sdk, others not. Enable status will be reported by callback.
     */
    public int enable(MeshInitParams params) {
        if (DBG) Log.d(TAG, "enable, role=" + params.getRole());
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.enable(params);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Disable Mesh.
     *
     * @return status: 0 if disable mesh success, others fail
     */
    public int disable() {
        if (DBG) Log.d(TAG, "disable");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.disable();
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * set composition data header.
     *
     * @param data composition data header
     * @return true if set composition data header success, false otherwise
     */
    public boolean setCompositionDataHeader(int[] data) {
        if (DBG) Log.d(TAG, "setCompositionDataHeader");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.setCompositionDataHeader(data);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return false;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return false;
        }
    }

    /**
     * Add element.
     * <p>Valid location are:
     * {MeshConstants #MESH_MODEL_ELEMENT_LOCATION_FIRST},
     * {MeshConstants #MESH_MODEL_ELEMENT_LOCATION_SECOND},
     * {MeshConstants #MESH_MODEL_ELEMENT_LOCATION_FRONT},
     * {MeshConstants #MESH_MODEL_ELEMENT_LOCATION_BACK},
     * {MeshConstants #MESH_MODEL_ELEMENT_LOCATION_TOP},
     * {MeshConstants #MESH_MODEL_ELEMENT_LOCATION_BOTTOM},
     * {MeshConstants #MESH_MODEL_ELEMENT_LOCATION_UPPER},
     * {MeshConstants #MESH_MODEL_ELEMENT_LOCATION_LOWER},
     * {MeshConstants #MESH_MODEL_ELEMENT_LOCATION_MAIN}.
     *
     * @param location - element location
     * @return element Index if add element success, -1 otherwise
     */
    public int addElement( int location) {
        if (DBG) Log.d(TAG, "addElement");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.addElement(location);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Set element address.
     *
     * @param addr element address
     * @return true if set element address success, false otherwise
     */
    public boolean setElementAddr(int addr) {
        if (DBG) Log.d(TAG, "setElementAddr");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.setElementAddr(addr);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return false;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return false;
        }
    }

    /**
     * Add Model.
     * <p>Valid model should be one of the instance of:
     * {ConfigurationServerModel},
     * {ConfigurationClientModel},
     * {HealthServerModel},
     * {HealthClientModel},
     * {GenericOnOffServerModel},
     * {GenericOnOffClientModel},
     * {GenericPowerOnOffClientModel},
     * {CtlSetupServerModel},
     * {CtlClientModel},
     * {CtlServerModel},
     * {HslSetupServerModel},
     * {HslClientModel},
     * {HslServerModel},
     * {LightnessClientModel},
     * {GenericLevelServerModel},
     * {GenericLevelClientModel},
     * {VendorModel},
     * {And more models later}.
     *
     * @param model mesh model
     * @return modelhandle if add model success, false otherwise
     */
    public int addModel(MeshModel model) {
        if (DBG) Log.d(TAG, "addModel modelopcode=0x" + Integer.toHexString(model.getModelOpcode())
            + ",elementIndex=0x" + Integer.toHexString(model.getElementIndex()));
        try {
            if (mService != null && isBluetoothEnabled()) {
                int mModelHandle = mService.addModel(model);
                if (mModelHandle > -1){
                    mModelMap.put(mModelHandle, model);
                }
                if (DBG) Log.d(TAG, "addModel modelHandle=0x" + Integer.toHexString(mModelHandle));
                return mModelHandle;
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Set netkey.
     * <p>Valid op values are:
     * {MeshConstants #MESH_KEY_OP_ADD},
     * {MeshConstants #MESH_KEY_OP_UPDATE},
     * {MeshConstants #MESH_KEY_OP_USE_NEW_NETKEY},
     * {MeshConstants #MESH_KEY_OP_REVOKE_OLD_NETKEY}.
     *
     * @param op key opcode
     * @param key netkey size should be 16: {MeshConstants #MESH_KEY_SIZE}
     * @param netIndex netkey Index
     * @return status: 0 if set netkey success, others fail
     */
    public int setNetkey(int op, int[] key, int netIndex) {
        if (DBG) Log.d(TAG, "setNetkey");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.setNetkey(op, key, netIndex);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Set appkey.
     * <p>Valid op values are:
     * {MeshConstants #MESH_KEY_OP_ADD},
     * {MeshConstants #MESH_KEY_OP_UPDATE},
     * {MeshConstants #MESH_KEY_OP_USE_NEW_NETKEY},
     * {MeshConstants #MESH_KEY_OP_REVOKE_OLD_NETKEY}.
     *
     * @param op key opcode
     * @param key appkey size should be 16: {MeshConstants #MESH_KEY_SIZE}
     * @param netIndex netkey Index
     * @param appIndex appkey Index
     * @return status: 0 if set netkey success, others fail
     */
    public int setAppkey(int op, int[] key, int netIndex, int appIndex) {
        if (DBG) Log.d(TAG, "setAppkey");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.setAppkey(op, key, netIndex, appIndex);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Start/stop mesh scan
     *
     * @param start true as startScan, false as stopScan
     * @param duration time when scan, 0 if stopScan
     * @return void
     */
    public void unProvisionScan(boolean start, int duration) {
        if (DBG) Log.d(TAG, "unProvisionScan start=" + start);
        try {
            if (mService != null && isBluetoothEnabled()) {
                mService.unProvisionScan(start, duration);
                return;
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        }
    }

    /**
     * Invite provisioning
     *
     * @param UUID target UUID
     * @param attentionDuration
     * @return status: 0 if invite provisioning success, others fail
     */
    public int inviteProvisioning(int[] UUID, int attentionDuration) {
        if (DBG) Log.d(TAG, "inviteProvisioning with UUID " + Arrays.toString(UUID));
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.inviteProvisioning(UUID, attentionDuration);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Start provisioning
     *
     * @param algorithm
     * @param publicKey
     * @param authMethod
     * @param authAction
     * @param authSize
     * @param netkey size should be 16: {MeshConstants #MESH_KEY_SIZE}
     * @param netkeyIndex
     * @param ivIndex
     * @param addr
     * @param flags
     * @param mode
     * @return status: 0 if start provisioning success, others fail
     */
    public int startProvisioning(int algorithm, int publicKey, int authMethod, int authAction, int authSize,
           int[] netkey, int netkeyIndex, long ivIndex, int addr, int flags, int mode) {
        if (DBG) Log.d(TAG, "startProvisioning");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.startProvisioning(algorithm, publicKey, authMethod, authAction, authSize,
                    netkey, netkeyIndex, ivIndex, addr, flags, mode);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Set provision factor.
     * <p>Valid type values are:
     * {MeshConstants #MESH_PROV_FACTOR_CONFIRMATION_KEY},
     * {MeshConstants #MESH_PROV_FACTOR_RANDOM_PROVISIONER},
     * {MeshConstants #MESH_PROV_FACTOR_RANDOM_DEVICE},
     * {MeshConstants #MESH_PROV_FACTOR_CONFIRMATION_PROVISIONER},
     * {MeshConstants #MESH_PROV_FACTOR_CONFIRMATION_DEVICE},
     * {MeshConstants #MESH_PROV_FACTOR_PUB_KEY},
     * {MeshConstants #MESH_PROV_FACTOR_AUTHEN_VALUE},
     * {MeshConstants #MESH_PROV_FACTOR_AUTHEN_RESULT}.
     *
     * @param type provision factor type
     * @param buf provision factor data
     * @return status: 0 if set provision factor data success, others fail
     */
    public int setProvisionFactor(int type, int[] buf) {
        if (DBG) Log.d(TAG, "setProvisionFactor type=" + type);
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.setProvisionFactor(type, buf);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * send config message.
     *
     * @param dst dst
     * @param src src
     * @param ttl ttl
     * @param netKeyIndex netkey index
     * @param opcode opcode
     * @param param config parms, set by the set function in ConfigMessageParams
     * @return status: 0 if send config message success, others fail
     * @hide
     */
    public int sendConfigMessage(int dst, int src, int ttl, int netKeyIndex, int opcode,
                                                ConfigMessageParams param) {
        if (DBG) Log.d(TAG, "sendConfigMessage");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.sendConfigMessage(dst, src, ttl, netKeyIndex, opcode, param);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * send packet.
     *
     * @param dst dst
     * @param src src
     * @param ttl ttl
     * @param netKeyIndex netkey index
     * @param appKeyIndex appkey index
     * @param payload packet data
     * @return status: 0 if send packet success, others fail
     * @hide
     */
    public int sendPacket(int dst, int src, int ttl, int netKeyIndex, int appKeyIndex, int[] payload) {
        if (DBG) Log.d(TAG, "sendPacket");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.sendPacket(dst, src, ttl, netKeyIndex, appKeyIndex, payload);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Publish model.
     *
     * @param modelHandle model handle
     * @param opCode operation code
     * @param companyId  use MeshConstants #MESH_MODEL_COMPANY_ID_NONE if this is a SIG access message
     * @param buffer the message buffer for sending.
     * @return status: 0 if publish model success, others fail
     */
    public int publishModel(int modelHandle, int opCode, int companyId, int[] buffer) {
        if (DBG) Log.d(TAG, "publishModel modelHandle=0x" + Integer.toHexString(modelHandle));
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.publishModel(modelHandle, opCode, companyId, buffer);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Set Mesh Mode to choose different mode do different things.
     * <p>Valid mode values are:
     * {MeshConstants #MESH_MODE_OFF},
     * {MeshConstants #MESH_MODE_ON},
     * {MeshConstants #MESH_MODE_STANDY}.
     *
     * @param mode mesh mode
     * @return true if reset data success, false otherwise
     */
    public int setMeshMode(int mode) {
        if (DBG) Log.d(TAG, "setMeshMode mode=" + mode);
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.setMeshMode(mode);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Reset data.
     * <p>Valid sector values are:
     * {MeshConstants #MESH_FLASH_RECORD_ROLE_PROVISIONEE|MESH_FLASH_RECORD_ALL},
     * {MeshConstants #MESH_FLASH_RECORD_ROLE_PROVISIONEE|MESH_FLASH_RECORD_DATA},
     * {MeshConstants #MESH_FLASH_RECORD_ROLE_PROVISIONEE|MESH_FLASH_RECORD_SEQ_NUM},
     * {MeshConstants #MESH_FLASH_RECORD_ROLE_PROVISIONEE|MESH_FLASH_RECORD_REMOTE_NODE},
     *
     * {MeshConstants #MESH_FLASH_RECORD_ROLE_PROVISIONER|MESH_FLASH_RECORD_ALL},
     * {MeshConstants #MESH_FLASH_RECORD_ROLE_PROVISIONER|MESH_FLASH_RECORD_DATA},
     * {MeshConstants #MESH_FLASH_RECORD_ROLE_PROVISIONER|MESH_FLASH_RECORD_SEQ_NUM},
     * {MeshConstants #MESH_FLASH_RECORD_ROLE_PROVISIONER|MESH_FLASH_RECORD_REMOTE_NODE},
     *
     * @param sector flash record
     * @return true if reset data success, false otherwise
     */
    public boolean resetData(int sector) {
        if (DBG) Log.d(TAG, "resetData, sector=0x" + Integer.toHexString(sector));
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.resetData(sector);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return false;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return false;
        }
    }

    /**
     * Save data.
     *
     * @return true if reset data success, false otherwise
     */
    public boolean saveData() {
        if (DBG) Log.d(TAG, "saveData");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.saveData();
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return false;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return false;
        }
    }

    public void setData() { /* TODO */
    }

    /**
     * Get mesh sdk version.
     *
     * @return mesh sdk version
     */
    public String getVersion() {
        if (DBG) Log.d(TAG, "getVersion");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.getVersion();
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return null;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return null;
        }
    }


    /**
     * Dump.
     * <p>Valid type values are:
     * {MeshConstants #MESH_DUMP_TYPE_UUID},
     * {MeshConstants #MESH_DUMP_TYPE_NETWORK},
     * {MeshConstants #MESH_DUMP_TYPE_TRANSPORT},
     * {MeshConstants #MESH_DUMP_TYPE_CONFIG},
     * {MeshConstants #MESH_DUMP_TYPE_MODEL},
     * {MeshConstants #MESH_DUMP_TYPE_LPN},
     * {MeshConstants #MESH_DUMP_TYPE_PROXY},
     * {MeshConstants #MESH_DUMP_TYPE_ADV},
     * {MeshConstants #MESH_DUMP_TYPE_ALL}.
     *
     * @param type dump type
     * @return void
     */
    public void dump(int type) {
        if (DBG) Log.d(TAG, "dump with type " + type);
        try {
            if (mService != null && isBluetoothEnabled()) {
                mService.dump(type);
                return;
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        }
    }

    /**
     * Get element address
     *
     * @param elementIndex element index
     * @return element address if get success, -1 otherwise
     */
    public int getElementAddr(int elementIndex) {
        if (DBG) Log.d(TAG, "getElementAddr by elementIndex " + elementIndex);
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.getElementAddr(elementIndex);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Set default TTL
     *
     * @param ttl ttl
     * @return void
     */
    public void setDefaultTTL(int ttl) {
        if (DBG) Log.d(TAG, "setDefaultTTL ttl=" + ttl);
        try {
            if (mService != null && isBluetoothEnabled()) {
                mService.setDefaultTTL(ttl);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        }
    }

    /**
     * Get default TTL
     *
     * @return ttl ttl
     */
    public int getDefaultTTL() {
        if (DBG) Log.d(TAG, "getDefaultTTL");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.getDefaultTTL();
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Set Iv
     *
     * @param ivIndex iv index
     * @param ivPhase iv phase
     * @return status: 0 if publish model success, others fail
     */
    public int setIv(long ivIndex, int  ivPhase) {
        if (DBG) Log.d(TAG, "setIv");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.setIv(ivIndex, ivPhase);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Add device key
     *
     * @param unicastAddr unicast address
     * @param devicekey size should be 16: {MeshConstants #MESH_KEY_SIZE}
     * @param uuid size should be 16: {MeshConstants #MESH_UUID_SIZE}
     * @return status: 0 if publish model success, others fail
     */
    public int addDevKey(int unicastAddr, int[] devicekey, int[] uuid)
    {
        if (DBG) Log.d(TAG, "addDevKey");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.addDevKey(unicastAddr, devicekey, uuid);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Get device key
     *
     * @param unicastAddr unicast address
     * @return device key and UUID: the int array len should be 32, and first 16 is dev key, last 16 is UUID
     */
    public int[] getDevKey(int unicastAddr)
    {
        if (DBG) Log.d(TAG, "getDevKey");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.getDevKey(unicastAddr);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return null;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return null;
        }
    }

    /**
     * Delete device key
     *
     * @param unicastAddr unicast address
     * @return status: 0 if delete device key success, others fail
     */
     public int delDevKey(int unicastAddr)
     {
         if (DBG) Log.d(TAG, "delDevKey");
        try {
             if (mService != null && isBluetoothEnabled()) {
                 return mService.delDevKey(unicastAddr);
             }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
     }

    /**
     * Model app bind
     *
     * @param handle model handle
     * @param appIndex app index
     * @return status: 0 if model app bind success, others fail
     */
    public int modelAppBind(int handle,int appIndex) {
        if (DBG) Log.d(TAG, "setModelAppBind");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.modelAppBind(handle, appIndex);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Access model reply
     *
     * @param handle model handle
     * @param msg access rx msg
     * @param reply access rx msg
     * @return status: 0 if access model reply success, others fail
     */
    public int accessModelReply(int handle, BluetoothMeshAccessRxMessage msg, BluetoothMeshAccessTxMessage reply){
        if (DBG) Log.d(TAG, "accessModelReply");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.accessModelReply(handle, msg, reply);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Set log level
     *
     * @param level log level
     * @return void
     */
    public void setLogLevel(long level) {
        if (DBG) Log.d(TAG, "setLogLevel");
        try {
            if (mService != null && isBluetoothEnabled()) {
                mService.setLogLevel(level);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
        }
    }

    /**
     * Get model handle by element and model id
     *
     * @param modelId model id
     * @param elementIndex element index
     * @return model handle if get model handle success, -1 otherwise
     */
    public int getModelHandle(long modelId, int elementIndex) {
        if (DBG) Log.d(TAG, "getModelHandle");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.getModelHandle(modelId, elementIndex);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Gatt connect
     * <p>Valid addressType values are:
     * {MeshConstants #MESH_BLE_ADDR_TYPE_PUBLIC},
     * {MeshConstants #MESH_BLE_ADDR_TYPE_RANDOM_STATIC},
     * {MeshConstants #MESH_BLE_ADDR_TYPE_RANDOM_RESOLVABLE},
     * {MeshConstants #MESH_BLE_ADDR_TYPE_RANDOM_NON_RESOLVABLE}.
     *
     * <p>Valid serviceType values are:
     * {MeshConstants #MESH_GATT_SERVICE_PROXY},
     * {MeshConstants #MESH_GATT_SERVICE_PROVISION}.
     *
     * @param address address
     * @param addressType  address type
     * @param serviceType service type
     * @return status: 0 if gatt connect success, others fail
     */
    public int gattConnect(String address, int addressType, int serviceType) {
        if (DBG) Log.d(TAG, "gattConnect");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.gattConnect(address, addressType, serviceType);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Gatt disconnect
     *
     * @return status: 0 if gatt disconnect success, others fail
     */
    public int gattDisconnect() {
        if (DBG) Log.d(TAG, "gattDisconnect");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.gattDisconnect();
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Set heartbeat period
     *
     * @param num number
     * @param hbTimeout heartbeat timeout
     * @return status: 0 if set heartbeat period success, others fail
     */
    public int setHeartbeatPeriod(int num, long hbTimeout) {
        if (DBG) Log.d(TAG, "setHeartbeatPeriod");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.setHeartbeatPeriod(num, hbTimeout);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * OTA get client model handle
     *
     * @return dist client and update client: size 2, the first int is dist client and second is update client
     */
    public int[] otaGetClientModelHandle() {
        if (DBG) Log.d(TAG, "otaGetClientModelHandle");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.otaGetClientModelHandle();
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return null;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return null;
        }
    }

    /**
     * OTA initiator operation
     * <p>Valid params should only be set by one of below:
     * {OtaOperationParams #setOtaInitiatorMsgHandler},
     * {OtaOperationParams #setOtaInitiatorFwInfoGet},
     * {OtaOperationParams #setOtaInitiatorStopParams},
     * {OtaOperationParams #setOtaInitiatorStartParams},
     * {OtaOperationParams #setOtaInitiatorApplyDistribution}.
     *
     * @param params set by set function of OtaOperationParams
     * @return status: 0 if OTA initiator operation success, others fail
     */
    public int otaInitiatorOperation (OtaOperationParams params){
        if (DBG) Log.d(TAG, "otaInitiatorOperation");
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.otaInitiatorOperation(params);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * bearer Adv Set Params
     *
     * @param advPeriod adv period in milliseconds
     * @param minInterval adv minimum interval in 625 us units
     * @param maxInterval adv maximum interval in 625 us units
     * @param resend resend number of times, 0xFF means resending forever until user discard it
     * @param scanPeriod scan period in milliseconds
     * @param scanInterval scan interval in 625 us units
     * @param scanWindow scan interval in 625 us units
     * @return status: 0 if set success, others fail
     */
    public int bearerAdvSetParams (long advPeriod, int minInterval, int maxInterval, int resend, long scanPeriod, int scanInterval, int scanWindow){
        if (DBG) Log.d(TAG, "bearerAdvSetParams:advPeriod=" + advPeriod + ", minInterval=" + minInterval +
            ", maxInterval=" + maxInterval + ", resend=" + resend + ", scanPeriod=" + scanPeriod +
            ", scanInterval=" + scanInterval + ", scanWindow=" + scanWindow);
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.bearerAdvSetParams(advPeriod, minInterval, maxInterval, resend, scanPeriod, scanInterval, scanWindow);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * bearer Adv Set Ble Scan Params
     * @param scanInterval scan interval in 625 us units
     * @param scanWindow scan interval in 625 us units
     * @return status: 0 if set success, others fail
     */
    public int setScanParams (int scanInterval, int scanWindow){
        if (DBG) Log.d(TAG, "setScanParams: scanInterval=" + scanInterval + ", scanWindow=" + scanWindow);
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.setScanParams(scanInterval, scanWindow);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * set Special Pkt Params
     *
     * @param isSnIncrease true is seq number increase, false is seq number not increase
     * @param snIncreaseInterval seq number increase interval, unit: ms
     * @return status: 0 if set success, others fail
     */
    public int setSpecialPktParams (boolean isSnIncrease, int snIncreaseInterval, int advInterval, int advPeriod){
        if (DBG) Log.d(TAG, "setSpecialPktParams: isSnIncrease=" + isSnIncrease + ", snIncreaseInterval=" + snIncreaseInterval
            + ", advInterval=" + advInterval + ",advPeriod=" + advPeriod);
        try {
            if (mService != null && isBluetoothEnabled()) {
                return mService.setSpecialPktParams(isSnIncrease, snIncreaseInterval, advInterval, advPeriod);
            }
            if (mService == null) Log.w(TAG, "Proxy not attached to service");
            return -1;
        } catch (RemoteException e) {
            Log.e(TAG, "Stack:" + Log.getStackTraceString(new Throwable()));
            return -1;
        }
    }

    /**
     * Check if bluetooth is enabled
     *
     * @return true if bluetooth is enabled, false otherwise
     * @hide
     */
    private boolean isBluetoothEnabled() {
       if (mAdapter.getState() == BluetoothAdapter.STATE_ON) return true;
       return false;
    }
}
