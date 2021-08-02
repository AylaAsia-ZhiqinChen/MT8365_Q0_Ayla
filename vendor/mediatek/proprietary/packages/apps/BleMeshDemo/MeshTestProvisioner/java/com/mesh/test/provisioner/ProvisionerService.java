package com.mesh.test.provisioner;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.bluetooth.BluetoothMesh;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.mesh.*;
import android.bluetooth.mesh.model.*;
import com.mesh.test.provisioner.listener.BluetoothMeshListener;
import com.mesh.test.provisioner.model.*;
import com.mesh.test.provisioner.sqlite.Node;
import com.mesh.test.provisioner.sqlite.StorageData;
import com.mesh.test.provisioner.sqlite.NodeData;
import android.bluetooth.le.ScanResult;
import java.util.LinkedList;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.List;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Timer;
import java.util.TimerTask;
import java.util.ListIterator;
import java.util.Iterator;
import java.util.Set;
import java.util.concurrent.locks.ReentrantLock;
import java.util.Properties;
import java.io.InputStream;
import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.FileNotFoundException;
import android.bluetooth.BluetoothProfile;
import com.mesh.test.provisioner.util.MeshUtils;
import com.mesh.test.provisioner.MyApplication;
import java.util.Map;
import android.app.NotificationChannel;
import android.graphics.Color;
import android.app.NotificationManager;
import android.app.Notification;
import android.os.SystemProperties;


public class ProvisionerService extends Service{

    private static final String TAG = "ProvisionerService";
    private static final boolean DEBUG = true;
    private boolean isEnable = false;
    private boolean isInitialized = false;
    private static final int CONFIG_MSG_RESEND_TIMES = 170;
    private static final int COMPOSITIONDATA_MSG_RESEND_TIMES = 55;
    private static final int CONFIG_MSG_DEFAULT_RESEND_TIMES = 5;
    private static final int SEND_MSG_DEFAULT_RESEND_TIMES = 4;

    private StorageData mStorageData = new StorageData();
    //private NodeData mNodeData = new NodeData();

    private int defaultTTL = 4;
    private int resendConfigMsgID = 0;
    private int resendMsgID = 0;
    private LinkedList<ResendConfigMsg> resendMsgList = new LinkedList<>();
    private final ReentrantLock resendMsgListLock = new ReentrantLock();
    private ResendMsg resendMsg;

    private int[] mMyUUID = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0xCC, 0xDD, 0xEE, 0x01};
    private int mCurrMaxNetKeyIndex = 0;
    private int mCurrMaxAppKeyIndex = 0;

    private HashMap<Integer, NetKey> mAllNetKeyMap = new HashMap<>();
    private HashMap<Integer, AppKey> mAllAppKeyMap = new HashMap<>();
    private ArrayList<Integer> mGroupAddrList = new ArrayList();

    private BluetoothMeshListener mBluetoothMeshListener;
    private BluetoothMesh mBluetoothMesh;

    private Element mPrimaryElement;
    private int mElementCnt = 0;
    private HashMap<Integer, MeshModel> mPrimaryElementSIGModels = new HashMap<>();

    //These fields value can be customized by property file---------
    private int mMyAddr=100;
    private int mLastElementAddr=100;
    private int mLPNNumber=20;
    private int mLPNMessageQueueSize=10;
    private int mLPNSubscriptionListSize=5;
    private int mRemoteNodeNumber=500;
    private int mSaveFlash=1;
    private int mHearbeatPeriodNum=6;   //2*(6-1) = 32s
    private int mHeartbeatTimeout=320;  //320s
    private int[] mDefaultAuthValue = {0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    private int mMeshNumber = 0;
    public ArrayList<String> uuidList = new ArrayList<>();
    private int nodeResetNumber = 0;
    public ArrayList<String> nodeResetList = new ArrayList<>();
    private String filePath = "/data/data/com.mesh.test.provisioner/mesh_properties.txt";
    //------------------------------------------------------

    //The below 3 list share the same list item index and list length
    private ArrayList<int[]> mUUIDList = new ArrayList<>();
    private ArrayList<String> mGATTAddrList = new ArrayList<>();
    private ArrayList<int[]> mAuthValueList = new ArrayList<>();

    private ConfigurationServerModel mConfigServer;
    private ConfigurationClientModel mConfigClient;
    private HealthServerModel mHealthServer;
    private GenericOnOffClientModel mGenericOnOffClient;
    private LightnessClientModel mLightnessClient;
    private CtlClientModel mCTLClient;
    private HslClientModel mHSLClient;
    private VendorModelDemoClient mVendorModelClient;
    private boolean gattProvisioningService = false;
    private boolean gattProxyService = false;
    private int meshMode = 0;
    public boolean isProvOrConfiging = false;
    public boolean isKeyrefreshing = false;
    public boolean isBtReoot = false;
    public boolean isGroupSendMsg = false;
    public boolean isGroupSendMsging = false;
    public int groupSMNodeAddr = 0;
    public ArrayList<Integer> cfSuccessNodeList = new ArrayList<>();
    public ArrayList<Integer> ackGroupSendMsgList = new ArrayList<>();
    public ArrayList<Integer> ackNodeSendMsgList = new ArrayList<>();
    public ArrayList<String> groupMsgTimeList = new ArrayList<String>();

    private int onOffTID = 1;   //only for ON OFF client Generic OnOff Set MSG

    private OtaOperationParams mOtaParams = new OtaOperationParams();
    private int mOtaState = -1; //Starting, started, ongoing, stop, dfu_ready

    private Node currentProvNode;
    private HashMap<Integer, Node> mNodes = new HashMap<>();
    private HashSet<Integer> supportedMsgSet = new HashSet<>();
    private HashMap<Integer, Integer> phaseOneList = new HashMap<>();

    //private final int MESH_BEARER_ADV = 0;
    //private final int MESH_BEARER_GATT = 1;

    public static final int MESH_NODE_CONFIG_STATE_IDLE                   = 0;
    public static final int MESH_NODE_CONFIG_STATE_PROVISIONING           = 1;
    public static final int MESH_NODE_CONFIG_STATE_PROVISIONED            = 2;
    public static final int MESH_NODE_CONFIG_STATE_GET_COMPOSITION_DATA   = 3;
    public static final int MESH_NODE_CONFIG_STATE_ADD_APPKEY             = 4;
    public static final int MESH_NODE_CONFIG_STATE_MODEL_APP_BIND         = 5;
    public static final int MESH_MODE_ON = 1;
    public static final int MESH_MODE_OFF = 0;
    public static final int MESH_MODE_STANDBY = 2;

    private HashMap<Integer, Timer> mTimerMap = new HashMap<>();

    private void log(String string) {
        if(DEBUG) {
            Log.i(TAG, string);
        }
    }

    private void dumpInfo() {
        log("dump APP Info -------> START");
        ArrayList<Node> nodeList = new ArrayList<Node>(mNodes.values());
        log("node count = " + nodeList.size());
        for (int i = 0; i < nodeList.size(); i++) {
            log("Address: " + nodeList.get(i).getAddr());
            log("Bearer: " + nodeList.get(i).getNodeBearer());
            log("UUID: " + MeshUtils.intArrayToString(nodeList.get(i).getUUID(), true));
            log("DeviceKey: " + MeshUtils.intArrayToString(nodeList.get(i).getDevKey(), false));
            log("Netkey Index: " + nodeList.get(i).getCurrNetkeyIndex());
        }

        ArrayList<NetKey> netKeyList = new ArrayList<NetKey>(mAllNetKeyMap.values());
        log("Netkey count = " + netKeyList.size());
        for (int i = 0; i < netKeyList.size(); i++) {
            log("Index: " + netKeyList.get(i).getIndex());
            log("Value: " + MeshUtils.intArrayToString(netKeyList.get(i).getValue(), false));
            log("TempValue: " + MeshUtils.intArrayToString(netKeyList.get(i).getTempValue(), false));
        }

        ArrayList<AppKey> appKeyList = new ArrayList<AppKey>(mAllAppKeyMap.values());
        log("Appkey count = " + appKeyList.size());
        for (int i = 0; i < appKeyList.size(); i++) {
            log("Index: " + appKeyList.get(i).getIndex());
            log("Value: " + MeshUtils.intArrayToString(appKeyList.get(i).getValue(), false));
            log("Bound netkey Index: " + appKeyList.get(i).getBoundNetKeyIndex());
        }

        log("GroupAddress count = " + mGroupAddrList.size());
        for (int i = 0; i < mGroupAddrList.size(); i++) {
            log("Address: " + mGroupAddrList.get(i));
        }

        log("dump APP Info -------> END");
    }

    public boolean isEnable(){
        return isEnable;
    }

    private int createTimer() {
        for (int i = 0; i < Integer.MAX_VALUE; i++) {   //find available time id
            if (null == mTimerMap.get(i)) {
                log("createTimer new id = " + i);
                mTimerMap.put(i, new Timer());
                return i;
            }
        }
        log("createTimer no id resource");
        return -1; //no resource, this should be impossible
    }

    private Timer getTimerById(int id) {
        return mTimerMap.get(id);
    }

    private void addSupportedAuthValue() {
        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0x8f, 0x13, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:13:8f");
        this.mAuthValueList.add(new int[]{0xb7, 0xbd, 0x70, 0x3c, 0xf3, 0xc1, 0xe8, 0x13, 0x85, 0x11, 0x56, 0xea, 0x32, 0xae, 0x9d, 0x65});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0x81, 0x14, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:14:81");
        this.mAuthValueList.add(new int[]{0x8b, 0x36, 0x1f, 0xba, 0xe3, 0x81, 0xfe, 0xf2, 0x29, 0x5d, 0xa4, 0xd3, 0x1d, 0x2a, 0x82, 0xc4});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0x71, 0x14, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:14:71");
        this.mAuthValueList.add(new int[]{0xe2, 0xd4, 0x4e, 0x4e, 0xa8, 0xc7, 0xdb, 0xfa, 0x2c, 0x9d, 0x06, 0x85, 0x95, 0xbb, 0x14, 0xd4});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0x9f, 0x14, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:14:9f");
        this.mAuthValueList.add(new int[]{0xdf, 0xac, 0x10, 0x9a, 0x68, 0xa7, 0xec, 0x20, 0x89, 0xcc, 0x3f, 0x7a, 0x0b, 0x52, 0x72, 0x9a});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0x83, 0x15, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:15:83");
        this.mAuthValueList.add(new int[]{0x53, 0x65, 0x95, 0x21, 0x66, 0xef, 0x9c, 0xb8, 0x22, 0xdd, 0xa8, 0x46, 0xa1, 0xce, 0x4c, 0xf7});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0x15, 0x14, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:14:15");
        this.mAuthValueList.add(new int[]{0x6a, 0xdf, 0xdd, 0xfe, 0xaf, 0x3c, 0x5e, 0x12, 0xfb, 0xc4, 0x8a, 0x2f, 0xdd, 0x3c, 0x88, 0x0f});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0x28, 0x14, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:14:28");
        this.mAuthValueList.add(new int[]{0xbe, 0x7a, 0x9e, 0xea, 0xb8, 0xe0, 0x1e, 0x28, 0xe1, 0x80, 0x28, 0xcb, 0x92, 0xf9, 0x9e, 0x3c});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0x47, 0x14, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:14:47");
        this.mAuthValueList.add(new int[]{0x0d, 0xc5, 0x94, 0x68, 0x94, 0xd5, 0x9d, 0xf8, 0x3a, 0x9c, 0x3d, 0xd7, 0xe6, 0xe4, 0x83, 0x5d});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0x96, 0x14, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:14:96");
        this.mAuthValueList.add(new int[]{0xfe, 0xa9, 0x69, 0x1f, 0xe6, 0xe9, 0xab, 0xb3, 0x94, 0x0c, 0xa6, 0x5d, 0xe5, 0x61, 0xbd, 0x49});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0x42, 0x16, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:16:42");
        this.mAuthValueList.add(new int[]{0xc2, 0xa8, 0x79, 0x27, 0x9c, 0x4b, 0xe9, 0xf9, 0x73, 0x7a, 0xff, 0x67, 0x38, 0xe6, 0xd8, 0x49});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0xd1, 0x13, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:13:d1");
        this.mAuthValueList.add(new int[]{0x37, 0x4f, 0x4f, 0xe8, 0x15, 0x99, 0xbc, 0x5c, 0xf9, 0xff, 0x5a, 0x7a, 0xf0, 0xd9, 0xc5, 0x3c});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0x22, 0x15, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:15:22");
        this.mAuthValueList.add(new int[]{0xfb, 0xf2, 0x98, 0x38, 0x4e, 0xd3, 0x9b, 0xe0, 0x9f, 0x44, 0x7e, 0xa2, 0x13, 0xb9, 0x10, 0x38});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0x2a, 0x14, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:14:2a");
        this.mAuthValueList.add(new int[]{0x6b, 0xc8, 0xd2, 0x3a, 0x2b, 0x7b, 0x84, 0x8d, 0x33, 0xb4, 0x24, 0x42, 0xfd, 0x7a, 0x5b, 0x90});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0x71, 0x15, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:15:71");
        this.mAuthValueList.add(new int[]{0xb3, 0xeb, 0x6e, 0xd3, 0xbf, 0x21, 0x85, 0x34, 0xef, 0x25, 0xb0, 0x94, 0xb6, 0x8a, 0xc9, 0xc2});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0xf6, 0x14, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:14:f6");
        this.mAuthValueList.add(new int[]{0xb3, 0x78, 0x4d, 0x9f, 0x0b, 0x9e, 0x18, 0x58, 0x8e, 0x01, 0xd2, 0xcf, 0xcf, 0x1e, 0xf9, 0x11});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0xe3, 0x13, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:13:e3");
        this.mAuthValueList.add(new int[]{0xf8, 0xbb, 0xa1, 0xca, 0x2f, 0x00, 0x4c, 0xa3, 0x70, 0x7a, 0x9c, 0x6e, 0x59, 0x1a, 0x0b, 0xd6});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0xc8, 0x14, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:14:c8");
        this.mAuthValueList.add(new int[]{0x34, 0x61, 0x00, 0xcb, 0xec, 0xf8, 0x28, 0xb2, 0x3f, 0xaa, 0x3f, 0xe8, 0xf9, 0x86, 0x82, 0xb7});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0xbb, 0x14, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:14:bb");
        this.mAuthValueList.add(new int[]{0x38, 0x65, 0xda, 0x2f, 0xff, 0xc1, 0xc0, 0x63, 0xa0, 0xfc, 0xf8, 0x78, 0x7c, 0x38, 0xe8, 0x81});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0xd0, 0x13, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:13:d0");
        this.mAuthValueList.add(new int[]{0x01, 0x25, 0xa3, 0x82, 0x75, 0xca, 0xd5, 0x5c, 0x4f, 0xdb, 0xfa, 0xb2, 0x0d, 0xde, 0xf4, 0x15});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0x01, 0x00, 0x00, 0x00, 0x60, 0x13, 0x11, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:11:13:60");
        this.mAuthValueList.add(new int[]{0x40, 0xd4, 0x76, 0xfd, 0x97, 0x9e, 0x03, 0x4f, 0x51, 0xdd, 0xd2, 0xf7, 0x06, 0x6e, 0x02, 0x37});

        this.mUUIDList.add(new int[]{0xa8, 0x01, 0x71, 0xba, 0x01, 0x00, 0x00, 0x1a, 0x76, 0xb8, 0x07, 0xda, 0x78, 0x00, 0x00, 0x00});
        this.mGATTAddrList.add("78:da:07:b8:76:1a");
        this.mAuthValueList.add(new int[]{0xfb, 0x30, 0x18, 0x75, 0x59, 0x00, 0xf5, 0xe0, 0xe5, 0x12, 0x34, 0x51, 0xbd, 0x4b, 0x92, 0xcf});
    }

    private int[] getAuthValue(Node node) {
        if (node != null) {
            if (node.getNodeBearer() == MeshConstants.MESH_BEARER_GATT) {
                for (int i = 0; i < this.mGATTAddrList.size(); i++) {
                    if (node.getGattAddr().compareTo(this.mGATTAddrList.get(i)) == 0) {
                        return this.mAuthValueList.get(i);
                    }
                }
             } else {
                for (int i = 0; i < this.mUUIDList.size(); i++) {
                    if (Arrays.equals(node.getUUID(), this.mUUIDList.get(i))) {
                        return this.mAuthValueList.get(i);
                    }
                }
            }
        }
        return mDefaultAuthValue;
    }

    private void updateStorageData(int type) {
        log("updateStorageData");
        if(mBluetoothMeshListener != null) {
            mBluetoothMeshListener.onStorageDataChange(mStorageData, type);
        }
    }

    private void updateNodeData(NodeData mNodeData, int type) {
        log("updateNodeData");
        if(mBluetoothMeshListener != null) {
            log("updateNodeData mNodeData address = " + mNodeData.getAddress());
            mBluetoothMeshListener.onNodeDataChange(mNodeData, type);
        }
    }

    private void notifyProvState(Node node, boolean result) {
        log("notifyProvState result = " + result);
        if(mBluetoothMeshListener != null) {
            mBluetoothMeshListener.onProvStateChanged(node, result);
        }
    }

    private void addSupportedTXMsg(boolean sigModel, int sigModelId, long vendorModelId) {
        log("addSupportedTXMsg sigModel = " + sigModel + ", modelID = 0x" + Integer.toHexString(sigModelId) + ", vendorModelId = 0x" + Long.toHexString(vendorModelId));
        if (sigModel) {
            switch (sigModelId) {
                case MeshConstants.MESH_MODEL_SIG_MODEL_ID_GENERIC_ONOFF_CLIENT: {
                    supportedMsgSet.add(MeshConstants.MESH_MSG_GENERIC_ONOFF_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_GENERIC_ONOFF_SET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_GENERIC_ONOFF_SET_UNRELIABLE);
                    break;
                }
                case MeshConstants.MESH_MODEL_SIG_MODEL_ID_LIGHT_LIGHTNESS_CLIENT: {
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_SET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_SET_UNACKNOWLEDGED);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_LINEAR_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_LINEAR_SET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_LINEAR_SET_UNACKNOWLEDGED);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_LAST_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_RANGE_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_SET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_RANGE_SET);
                    break;
                }
                case MeshConstants.MESH_MODEL_SIG_MODEL_ID_LIGHT_CTL_CLIENT: {
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_CTL_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_CTL_SET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_CTL_SET_UNACKNOWLEDGED);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_CTL_TEMPERATURE_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_CTL_TEMPERATURE_SET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_CTL_TEMPERATURE_SET_UNACKNOWLEDGED);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_CTL_DEFAULT_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_CTL_DEFAULT_SET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_CTL_DEFAULT_SET_UNACKNOWLEDGED);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_SET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACKNOWLEDGED);
                    break;
                }
                case MeshConstants.MESH_MODEL_SIG_MODEL_ID_LIGHT_HSL_CLIENT: {
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_HUE_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_HUE_SET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_HUE_SET_UNACKNOWLEDGED);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_SATURATION_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_SATURATION_SET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_SATURATION_SET_UNACKNOWLEDGED);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_SET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_SET_UNACKNOWLEDGED);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_TARGET_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_DEFAULT_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_RANGE_GET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_DEFAULT_SET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_DEFAULT_SET_UNACKNOWLEDGED);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_RANGE_SET);
                    supportedMsgSet.add(MeshConstants.MESH_MSG_LIGHT_HSL_RANGE_SET_UNACKNOWLEDGED);
                    break;
                }
                //add more...
                default:
                    break;
            }
        }else { //Vendor Model or SIG MODEL which is added by using the way of vendor model
            if (vendorModelId == VendorModelDemoClient.MODEL_ID) {
                supportedMsgSet.add(VendorModelDemoClient.VENDOR_MSG_OPCODE_STATE_GET);
                supportedMsgSet.add(VendorModelDemoClient.VENDOR_MSG_OPCODE_STATE_SET);
                supportedMsgSet.add(VendorModelDemoClient.VENDOR_MSG_OPCODE_STATE_SET_UNACK);
            }
            //Add more...
        }
    }

    private void parseCompositionData(Node node, int[] comp) {

        log("parseCompositionData node: " + node.getAddr() + ", comp.length = " + comp.length);

        node.setCompDataHeaderInfo(comp[0] | (comp[1] << 8),
                                    comp[2] | (comp[3] << 8),
                                    comp[4] | (comp[5] << 8),
                                    comp[6] | (comp[7] << 8),
                                    comp[8] | (comp[9] << 8));

        int element_index = 0;

        for (int idx = 10; idx < comp.length - 1;)
        {
            int element_address = currentProvNode.getAddr() + element_index;
            log("element_address = " + MeshUtils.decimalToHexString("%04X", element_address));
            Element ele = new Element(element_address, element_index);
            SystemProperties.set("persist.bluetooth.last.element.addr", element_address + "");
            MeshUtils.writeProperties(filePath, "last_element_addr", element_address + "");
            Model curr_model;
            ele.mLoc = comp[idx] | (comp[idx + 1] << 8);
            ele.mNumS = comp[idx + 2];
            ele.mNumV = comp[idx + 3];
            log("Find element[" + element_index + "]: Loc = " + ele.mLoc + ", NumS = " + ele.mNumS + ", NumV = " + ele.mNumV);
            idx += 4;
            if (ele.mNumS > 0) {
                for (int i = 0; i < ele.mNumS; i++) {
                    log("Find SIG model: 0x" + Integer.toHexString(comp[idx] | (comp[idx + 1] << 8)));
                    curr_model = new Model(comp[idx] | (comp[idx + 1] << 8));
                    curr_model.setElementAddr(ele.getAddress());
                    ele.addModel(curr_model);
                    node.addModel(curr_model);
                    node.addSupportedRXMsg(ele.getAddress(), curr_model.getID(), curr_model);
                    idx += 2;
                }
            }
            if (ele.mNumV > 0) {
                for (int i = 0; i < ele.mNumV; i++) {
                    //Vendor model id = company id << 16 | model id
                    log("Find Vendor model: 0x" + Integer.toHexString(((comp[idx] | (comp[idx + 1] << 8)) << 16) | (comp[idx + 2] | (comp[idx + 3] << 8))));
                    curr_model = new Model(((comp[idx] | (comp[idx + 1] << 8)) << 16) | (comp[idx + 2] | (comp[idx + 3] << 8)));
                    curr_model.setElementAddr(ele.getAddress());
                    ele.addModel(curr_model);
                    node.addModel(curr_model);
                    node.addSupportedRXMsg(ele.getAddress(), curr_model.getID(), curr_model);
                    idx += 4;
                }
            }
            node.addElement(ele);
            ele.setNodeAddr(node.getAddr());
            element_index++;
        }
        log("parseCompositionData end");
        //updateStorageData(MyApplication.STORAGE_DATA_NODE);
        updateNodeData(new NodeData(node), MyApplication.NODE_DATA_UPDATE);
    }

    private class ExTimerTask<T> extends TimerTask {
        private T mArg;
        private int mId;

        public ExTimerTask(int id, T arg) {
            this.mArg = arg;
            this.mId = id;
        }

        private T getArg() {
            return this.mArg;
        }

        private void resendConfigMsg() {
            ResendConfigMsg msg = (ResendConfigMsg)mArg;
            log("resendConfigMsg op = 0x" + Integer.toHexString(msg.mMsgOpCode) + ", msgId = " + msg.msgId + ", dst = " + MeshUtils.decimalToHexString("%04X", msg.mDst));
            if (msg.resendCnt >= CONFIG_MSG_DEFAULT_RESEND_TIMES) {
                if (msg.mMsgOpCode == MeshConstants.MESH_MSG_CONFIG_COMPOSITION_DATA_GET ||
                    msg.mMsgOpCode == MeshConstants.MESH_MSG_CONFIG_APPKEY_ADD ||
                    msg.mMsgOpCode == MeshConstants.MESH_MSG_CONFIG_MODEL_APP_BIND ) {
                    if (msg.mMsgOpCode == MeshConstants.MESH_MSG_CONFIG_COMPOSITION_DATA_GET) {
                        if (msg.resendCnt >= COMPOSITIONDATA_MSG_RESEND_TIMES) {
                            log("resendConfigMsg msgId = " + msg.msgId + " for " + COMPOSITIONDATA_MSG_RESEND_TIMES + " times, stop resend, remove it");
                            log("resendConfigMsg lock");
                            resendMsgListLock.lock();
                            try {
                                resendMsgList.remove(msg);
                            } finally {
                            }
                            log("resendConfigMsg unlock");
                            resendMsgListLock.unlock();
                            //stop resend timer task
                            msg.stopResend();
                            return;
                        }
                    } else {
                        if (msg.resendCnt >= CONFIG_MSG_RESEND_TIMES) {
                            log("resendConfigMsg msgId = " + msg.msgId + " for " + CONFIG_MSG_RESEND_TIMES + " times, stop resend, remove it");
                            log("resendConfigMsg lock");
                            resendMsgListLock.lock();
                            try {
                                resendMsgList.remove(msg);
                            } finally {
                            }
                            log("resendConfigMsg unlock");
                            resendMsgListLock.unlock();
                            //stop resend timer task
                            msg.stopResend();
                            return;
                        }
                    }

                } else {
                    log("resendConfigMsg msgId = " + msg.msgId + " for 5 times, stop resend, remove it");
                    log("resendConfigMsg lock");
                    resendMsgListLock.lock();
                    try {
                        if (msg.mMsgOpCode == MeshConstants.MESH_MSG_CONFIG_NODE_RESET) {
                            mBluetoothMesh.delDevKey(msg.mDst);
                        }
                        resendMsgList.remove(msg);
                    } finally {
                    }
                    log("resendConfigMsg unlock");
                    resendMsgListLock.unlock();
                    /*
                    //If the Config Composition Data Get or Config APP Key Add timeoutfor config flow, abort the config procedure
                    Node node = mNodes.get(msg.mDst);
                    if (node != null) {
                        log("node addr = " + MeshUtils.decimalToHexString("%04X", node.getAddr()) + " , msg.mMsgOpCode = " + msg.mMsgOpCode + " , node.getConfigState = " + node.getConfigState());
                        if (((msg.mMsgOpCode == MeshConstants.MESH_MSG_CONFIG_COMPOSITION_DATA_GET) && (node.getConfigState() == MESH_NODE_CONFIG_STATE_GET_COMPOSITION_DATA))
                            || ((msg.mMsgOpCode == MeshConstants.MESH_MSG_CONFIG_APPKEY_ADD) && (node.getConfigState() == MESH_NODE_CONFIG_STATE_ADD_APPKEY))
                            || ((msg.mMsgOpCode == MeshConstants.MESH_MSG_CONFIG_MODEL_APP_BIND) && (node.getConfigState() == MESH_NODE_CONFIG_STATE_MODEL_APP_BIND))) {
                            if (msg.mMsgOpCode == MeshConstants.MESH_MSG_CONFIG_COMPOSITION_DATA_GET) {
                                log("Abort config procedure due to Config Composition fail");
                            } else if (msg.mMsgOpCode == MeshConstants.MESH_MSG_CONFIG_APPKEY_ADD) {
                                log("Abort config procedure due to Config AppKey add fail");
                            } else if (msg.mMsgOpCode == MeshConstants.MESH_MSG_CONFIG_MODEL_APP_BIND) {
                                log("Abort config procedure due to Config model appkey bind fail");
                            }
                            try {
                                getTimerById(node.getConfigTimerID()).cancel();
                                mTimerMap.remove(node.getConfigTimerID());
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                            MyApplication.configTime = System.currentTimeMillis() - MyApplication.configTime;
                            double configTime = MeshUtils.getDoubleDecimal(MyApplication.configTime*1.0/1000);
                            node.setConfigSuccess(false);
                            node.setConfigTime(configTime);
                            node.setConfigState(MESH_NODE_CONFIG_STATE_IDLE);
                            mBluetoothMesh.gattDisconnect();
                            notifyProvState(node, false);
                            //mBluetoothMesh.setMeshMode(MeshConstants.MESH_MODE_STANDBY);
                            updateStorageData(MyApplication.STORAGE_DATA_NODE);
                        }

                    }
                    */
                    //stop resend timer task
                    msg.stopResend();
                    return;
                }
            }
            mConfigClient.setConfigMessageHeader(msg.mSrc, msg.mDst, msg.mTtl, msg.mNetKeyIndex, msg.mMsgOpCode);
            //log("resendConfigMsg op = 0x" + Integer.toHexString(msg.mMsgOpCode) + ", msgId = " + msg.msgId + ", dst = 0x" + Integer.toHexString(msg.mDst));
            switch (msg.mMsgOpCode) {
                case MeshConstants.MESH_MSG_CONFIG_BEACON_GET:
                    mConfigClient.configBeaconGet();
                    break;
                case MeshConstants.MESH_MSG_CONFIG_BEACON_SET:
                    mConfigClient.configBeaconSet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_COMPOSITION_DATA_GET:
                    mConfigClient.configCompositionDataGet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_DEFAULT_TTL_GET:
                    mConfigClient.configDefaultTTLGet();
                    break;
                case MeshConstants.MESH_MSG_CONFIG_DEFAULT_TTL_SET:
                    mConfigClient.configDefaultTTLSet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_GATT_PROXY_GET:
                    mConfigClient.configGattProxyGet();
                    break;
                case MeshConstants.MESH_MSG_CONFIG_GATT_PROXY_SET:
                    mConfigClient.configGattProxySet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_KEY_REFRESH_PHASE_GET:
                    mConfigClient.configKeyRefreshPhaseGet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_KEY_REFRESH_PHASE_SET:
                    mConfigClient.configKeyRefreshPhaseSet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_FRIEND_GET:
                    mConfigClient.configFriendGet();
                    break;
                case MeshConstants.MESH_MSG_CONFIG_FRIEND_SET:
                    mConfigClient.configFriendSet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_RELAY_GET:
                    mConfigClient.configRelayGet();
                    break;
                case MeshConstants.MESH_MSG_CONFIG_RELAY_SET:
                    mConfigClient.configRelaySet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_MODEL_PUBLICATION_GET:
                    mConfigClient.configModelPubGet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_MODEL_PUBLICATION_SET:
                    mConfigClient.configModelPubSet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_MODEL_PUBLICATION_VIRTUAL_ADDRESS_SET:
                    mConfigClient.configModelPubSet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_ADD:
                    mConfigClient.configModelSubAdd(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_ADD:
                    mConfigClient.configModelSubAdd(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE:
                    mConfigClient.configModelSubDel(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_DELETE:
                    mConfigClient.configModelSubDel(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE_ALL:
                    mConfigClient.configModelSubDelAll(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_OVERWRITE:
                    mConfigClient.configModelSubOw(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_OVERWRITE:
                    mConfigClient.configModelSubOw(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_NETWORK_TRANSMIT_GET:
                    mConfigClient.configNetworkTransmitGet();
                    break;
                case MeshConstants.MESH_MSG_CONFIG_NETWORK_TRANSMIT_SET:
                    mConfigClient.configNetworkTransmitSet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_GET:
                    mConfigClient.configSigModelSubGet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_GET:
                    mConfigClient.configVendorModelSubGet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_NETKEY_ADD:
                    mConfigClient.configNetkeyAdd(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_NETKEY_DELETE:
                    mConfigClient.configNetkeyDel(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_NETKEY_GET:
                    mConfigClient.configNetkeyGet();
                    break;
                case MeshConstants.MESH_MSG_CONFIG_NETKEY_UPDATE:
                    mConfigClient.configNetkeyUpdate(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_APPKEY_ADD:
                    mConfigClient.configAppkeyAdd(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_APPKEY_UPDATE:
                    mConfigClient.configAppkeyUpdate(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_APPKEY_DELETE:
                    mConfigClient.configAppkeyDel(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_APPKEY_GET:
                    mConfigClient.configAppkeyGet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_MODEL_APP_BIND:
                    mConfigClient.configModelAppBind(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_MODEL_APP_UNBIND:
                    mConfigClient.configModelAppUnbind(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_SIG_MODEL_APP_GET:
                    mConfigClient.configSigModelAppGet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_VENDOR_MODEL_APP_GET:
                    mConfigClient.configVendorModelAppGet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_NODE_IDENTITY_GET:
                    mConfigClient.configNodeIdentityGet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_NODE_IDENTITY_SET:
                    mConfigClient.configNodeIdentitySet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_NODE_RESET:
                    mConfigClient.configNodeReset();
                    break;
                case MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_GET:
                    mConfigClient.configHbPubGet();
                    break;
                case MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_SET:
                    mConfigClient.configHbPubSet(msg.params);
                    break;
                case MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_GET:
                    mConfigClient.configHbSubGet();
                    break;
                case MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_SET:
                    mConfigClient.configHbSubSet(msg.params);
                    break;
                default:
                    log("resendConfigMsg UNKNOWN OPCODE: " + msg.mMsgOpCode);
                    return;
            }
            msg.resendCnt++;
        }

        private void KeyRefreshTimeoutEvent() {
            NetKey key = (NetKey)mArg;
            if (key.getKeyRefreshNodes().size() == 0) {
                log("Key refresh for NetKey " + key.getIndex() + " Timeout, may FAIL");
                key.setState(MeshConstants.MESH_KEY_REFRESH_STATE_NONE);
                key.getKeyRefreshAckNodes().clear();
                key.getKeyRefreshNodes().clear();
                updateStorageData(MyApplication.STORAGE_DATA_NETKEY);
                if (mBluetoothMeshListener != null) {
                    mBluetoothMeshListener.onKeyRefreshComplete(key.getIndex(), false);
                }
            } else {
                //key.getKeyRefreshNodes().clear();
                //key.getKeyRefreshNodes().putAll(key.getKeyRefreshAckNodes());
                log("Key refresh timeout event, state= " + key.getState() + ", nodes num = " + key.getKeyRefreshNodes().size());
                try {
                    if (getTimerById(key.getKeyRefreshTimerID()) != null) {
                        getTimerById(key.getKeyRefreshTimerID()).cancel();
                        mTimerMap.remove(key.getKeyRefreshTimerID());
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }

                if (key.getState() == MeshConstants.MESH_KEY_REFRESH_STATE_1) {//Not all the nodes responses config netkey update msg
                    //make key.getKeyRefreshNodes() not change
                    //key.getKeyRefreshNodes().clear();
                    //key.getKeyRefreshNodes().putAll(key.getKeyRefreshAckNodes());
                    for(Map.Entry<Integer,Node> entryNodes:key.getKeyRefreshNodes().entrySet()) {
                        boolean flag = false;
                        Node node = entryNodes.getValue();
                        for(Map.Entry<Integer,Node> entryAckNodes:key.getKeyRefreshAckNodes().entrySet()) {
                            Node ackNode = entryAckNodes.getValue();
                            if (node.getAddr() == ackNode.getAddr()) {
                                flag = true;
                                break;
                            }
                        }
                        if (!flag) {
                            log("key refresh step 2 --> step 3 not ack node address = " + MeshUtils.decimalToHexString("%04X", node.getAddr())+ " , status = " + node.getActiveStatus());
                        }
                    }
                    int id = createTimer();
                    if (-1 != id) {
                        key.setKeyRefreshTimerID(id);
                        try {
                            if (key.getKeyRefreshNodes().size() == 0) {
                                getTimerById(key.getKeyRefreshTimerID()).schedule(new ExTimerTask<NetKey>(1, key), 5000);
                            } else {
                                getTimerById(key.getKeyRefreshTimerID()).schedule(
                                    new ExTimerTask<NetKey>(1, key), (5000 + key.getKeyRefreshNodes().size()*2000*5));
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                    log("key refresh procedure step 3 (not all node ack) --> start to use new key value");
                    mBluetoothMesh.setNetkey(MeshConstants.MESH_KEY_OP_USE_NEW_NETKEY, key.getTempValue(), key.getIndex());
                } else if (key.getState() == MeshConstants.MESH_KEY_REFRESH_STATE_2) {//Not all the nodes responses config keyrefresh phase set 2
                    for(Map.Entry<Integer,Node> entryNodes:key.getKeyRefreshNodes().entrySet()) {
                        boolean flag = false;
                        Node node = entryNodes.getValue();
                        for(Map.Entry<Integer,Node> entryAckNodes:key.getKeyRefreshAckNodes().entrySet()) {
                            Node ackNode = entryAckNodes.getValue();
                            if (node.getAddr() == ackNode.getAddr()) {
                                flag = true;
                                break;
                            }
                        }
                        if (!flag) {
                            log("key refresh step 4 --> step 5 not ack node address = " + MeshUtils.decimalToHexString("%04X", node.getAddr()) + " , status = " + node.getActiveStatus());
                        }
                    }
                    int id = createTimer();
                    if (-1 != id) {
                        key.setKeyRefreshTimerID(id);
                        try {
                            if (key.getKeyRefreshNodes().size() == 0) {
                                getTimerById(key.getKeyRefreshTimerID()).schedule(new ExTimerTask<NetKey>(1, key), 5000);
                            } else {
                                getTimerById(key.getKeyRefreshTimerID()).schedule(
                                    new ExTimerTask<NetKey>(1, key), (5000 + key.getKeyRefreshNodes().size()*2000*5));
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                    log("key refresh procedure step 5 (not all node ack) --> revoke old key");
                    mBluetoothMesh.setNetkey(MeshConstants.MESH_KEY_OP_REVOKE_OLD_NETKEY, key.getTempValue(), key.getIndex());
                } else if (key.getState() == MeshConstants.MESH_KEY_REFRESH_STATE_3) {//Not all the nodes responses config keyrefresh phase set 3
                    //not remove local blacklisted node with this netkey
                    //deleteBlacklistNodes(key.getIndex());
                    for(Map.Entry<Integer,Node> entryNodes:key.getKeyRefreshNodes().entrySet()) {
                        boolean flag = false;
                        Node node = entryNodes.getValue();
                        for(Map.Entry<Integer,Node> entryAckNodes:key.getKeyRefreshAckNodes().entrySet()) {
                            Node ackNode = entryAckNodes.getValue();
                            if (node.getAddr() == ackNode.getAddr()) {
                                flag = true;
                                break;
                            }
                        }
                        if (!flag) {
                            log("key refresh step 6 --> step 7 not ack node address = " + MeshUtils.decimalToHexString("%04X", node.getAddr()) + " , status = " + node.getActiveStatus());
                        }
                    }
                    key.setNetKeyMeta(key.getIndex(), key.getTempValue());
                    key.getKeyRefreshAckNodes().clear();
                    key.getNodes().clear();
                    key.getNodes().putAll(key.getKeyRefreshNodes());
                    key.getKeyRefreshNodes().clear();

                    key.setState(MeshConstants.MESH_KEY_REFRESH_STATE_NONE);
                    updateStorageData(MyApplication.STORAGE_DATA_NETKEY);
                    log("key refresh procedure success");
                    if (mBluetoothMeshListener != null) {
                        mBluetoothMeshListener.onKeyRefreshComplete(key.getIndex(), true);
                    }
                } else {

                }
            }
        }

        private void heartBeatEvent() {
            log("heartBeatEvent");
            resendMsgListLock.lock();
            try {
                Node node = (Node)mArg;
                int time = node.getHeartBeatTimerNumber();
                int currentHeartBeatNumber = node.getCurrentHeartBeatNumber();
                int preHeartBeatNumber = node.getPreHeartBeatNumber();
                int continueLost = node.getContinueLost();
                int maxLost = node.getMaxLost();
                time = time + 1;
                node.setHeartBeatTimerNumber(time);
                node.setHeartBeatTime(time*40000);
                log("heartBeatEvent , address=" + MeshUtils.decimalToHexString("%04X",node.getAddr()) + " , number = " + time + " , preHeartBeatNumber = " + preHeartBeatNumber + " , currentHeartBeatNumber = " + currentHeartBeatNumber);
                if(currentHeartBeatNumber > preHeartBeatNumber) {
                    preHeartBeatNumber = currentHeartBeatNumber;
                    continueLost = 0;
                    node.setPreHeartBeatNumber(preHeartBeatNumber);
                    node.setContinueLost(continueLost);
                }else {
                    continueLost++;
                    node.setContinueLost(continueLost);
                    if(continueLost > maxLost) {
                        maxLost = continueLost;
                        node.setMaxLost(maxLost);
                    }
                    log("heartBeatEvent , address=" + MeshUtils.decimalToHexString("%04X",node.getAddr()) + " , Cont Lost = " + continueLost + " , Max Lost = " + maxLost);
                }
                if(mBluetoothMeshListener != null) {
                    mBluetoothMeshListener.updateHeartBeatReceive();
                }
                if(time >= 10) {
                    Thread.sleep(1500);
                    node.setHeartBeatTimerNumber(0);
                    node.setHeartBeatTime(0);
                    node.setCurrentHeartBeatNumber(0);
                    node.setPreHeartBeatNumber(0);
                    //node.setContinueLost(0);
                    //node.setMaxLost(0);
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }finally {
            }
            resendMsgListLock.unlock();
        }

        private void gattProvisionServiceEvent() {
            log("gattProvisionServiceEvent");
            gattProvisioningService = false;
            if (null != getTimerById(currentProvNode.getProvisioningServiceTimerID())) {
                getTimerById(currentProvNode.getProvisioningServiceTimerID()).cancel();
                mTimerMap.remove(currentProvNode.getProvisioningServiceTimerID());
                currentProvNode.setProvisioningServiceTimerID(-1);
            }
        }

        private void gattProxyServiceEvent() {
            log("gattProxyServiceEvent");
            gattProxyService = false;
            if (null != getTimerById(currentProvNode.getProxyServiceTimerID())) {
                getTimerById(currentProvNode.getProxyServiceTimerID()).cancel();
                mTimerMap.remove(currentProvNode.getProxyServiceTimerID());
                currentProvNode.setProxyServiceTimerID(-1);
            }

        }

        private void gattProvisionEvent() {
            log("gattProvisionEvent");
            if (mBluetoothMeshListener != null) {
                mBluetoothMeshListener.provisionTimeout(70);
            }
            if (null != getTimerById(currentProvNode.getProvisionTimerID())) {
                getTimerById(currentProvNode.getProvisionTimerID()).cancel();
                mTimerMap.remove(currentProvNode.getProvisionTimerID());
                currentProvNode.setProvisionTimerID(-1);
            }
        }

        private void advProvisionEvent() {
            log("advProvisionEvent");
            if (mBluetoothMeshListener != null) {
                mBluetoothMeshListener.provisionTimeout(60);
            }
            if (null != getTimerById(currentProvNode.getProvisionTimerID())) {
                getTimerById(currentProvNode.getProvisionTimerID()).cancel();
                mTimerMap.remove(currentProvNode.getProvisionTimerID());
                currentProvNode.setProvisionTimerID(-1);
            }
        }

        private void sendMessageTimeoutEvent() {
            log("sendMessageTimeoutEvent , set Mesh Mode standby , isGroupSendMsg = " + isGroupSendMsg);
            if (getMeshMode() != MeshConstants.MESH_MODE_STANDBY) {
                setMeshMode(MeshConstants.MESH_MODE_STANDBY);
            }

            //all node send msg ack, cancel send msg timer
            if (null != getTimerById(currentProvNode.getSendMessageTimerID())) {
                getTimerById(currentProvNode.getSendMessageTimerID()).cancel();
                mTimerMap.remove(currentProvNode.getSendMessageTimerID());
                currentProvNode.setSendMessageTimerID(-1);
            }

            if (isGroupSendMsg) {
                MyApplication.groupSendMessageTime = System.currentTimeMillis() - MyApplication.groupSendMessageTime;
                log("adb group send message , not all node ack , ack node result as follows:");
                StringBuilder sb = new StringBuilder();
                log("ack node result as follows:");
                for (int i = 0; i < ackGroupSendMsgList.size(); i++) {
                    Node ackNode = getNodeByAddr(ackGroupSendMsgList.get(i));
                    int[] uuid = ackNode.getUUID();
                    String gattAddress = ackNode.getGattAddr();
                    if (uuid != null && gattAddress == null) {
                        sb.append("ack node ").append(i + 1).append(" = ").append(MeshUtils.intArrayToString(uuid, true));
                    }else if (uuid == null && gattAddress != null) {
                        sb.append("ack node ").append(i + 1).append(" = ").append(gattAddress);
                    }
                    log(sb.toString());
                    sb.delete(0, sb.length());
                }
                boolean flag = false;
                int index = 1;
                log("not ack node result as follows:");
                groupMsgTimeList.add("not ack group send msg node as follows:");
                for(int j = 0; j < cfSuccessNodeList.size(); j++) {
                    int nodeAddress = cfSuccessNodeList.get(j);
                    for (int k = 0; k < ackGroupSendMsgList.size(); k++) {
                        if (nodeAddress == getNodeByAddr(ackGroupSendMsgList.get(k)).getAddr()) {
                            flag = true;
                            break;
                        }
                    }
                    if (!flag) {
                        Node unAckNode = getNodeByAddr(nodeAddress);
                        int[] uuid = unAckNode.getUUID();
                        String gattAddress = unAckNode.getGattAddr();
                        if (uuid != null && gattAddress == null) {
                            sb.append("not ack node ").append(index).append(" = ").append(MeshUtils.intArrayToString(uuid, true));
                            if (unAckNode.getActiveStatus() == 0) {
                                groupMsgTimeList.add(new StringBuffer().append(MeshUtils.intArrayToString(uuid, true))
                                    .append(" , status = offline").toString());
                            } else if (unAckNode.getActiveStatus() == 1) {
                                groupMsgTimeList.add(new StringBuffer().append(MeshUtils.intArrayToString(uuid, true))
                                    .append(" , status = online").toString());
                            } else if (unAckNode.getActiveStatus() == 2) {
                                groupMsgTimeList.add(new StringBuffer().append(MeshUtils.intArrayToString(uuid, true))
                                    .append(" , status = unknown").toString());
                            }
                        }else if (uuid == null && gattAddress != null) {
                            sb.append("not ack node ").append(index).append(" = ").append(gattAddress);
                            if (unAckNode.getActiveStatus() == 0) {
                                groupMsgTimeList.add(new StringBuffer().append(gattAddress).append(" , status = offline").toString());
                            } else if (unAckNode.getActiveStatus() == 1) {
                                groupMsgTimeList.add(new StringBuffer().append(gattAddress).append(" , status = online").toString());
                            } else if (unAckNode.getActiveStatus() == 2) {
                                groupMsgTimeList.add(new StringBuffer().append(gattAddress).append(" , status = unknown").toString());
                            }
                        }
                        sb.append(" , status = ").append(unAckNode.getActiveStatus());
                        log(sb.toString());
                        sb.delete(0, sb.length());
                        index++;
                    }
                    flag = false;
                }
                if (mBluetoothMeshListener != null) {
                    isGroupSendMsging = false;
                    mBluetoothMeshListener.onGroupSendMsg(MyApplication.GROUP_SEND_MSG_END, groupMsgTimeList);
                }
            }
        }

        private void sendMessageCountEvent() {
            log("sendMessageCountEvent");
            ResendMsg msg = (ResendMsg)mArg;
            if (msg.resendCnt >= SEND_MSG_DEFAULT_RESEND_TIMES) {
                log("resendMessage " + msg.msgId + " for 4 times, stop resend");
                Node node = mNodes.get(msg.mDst);
                if (node != null) {
                    //stop resend timer task
                    msg.stopResendMsg();
                    resendMsg = null;
                }else {
                    log("not found node from mNodes");
                }
                return;
            }
            log("resendMessage op = 0x" + Integer.toHexString(msg.mMsgOpCode) + ", msgId = " + msg.msgId + ", dst = 0x" + Integer.toHexString(msg.mDst));
            switch (msg.mMsgOpCode) {
                case MeshConstants.MESH_MSG_GENERIC_ONOFF_GET:
                    mGenericOnOffClient.genericOnOffGet();
                    break;
                case MeshConstants.MESH_MSG_GENERIC_ONOFF_SET:
                case MeshConstants.MESH_MSG_GENERIC_ONOFF_SET_UNRELIABLE:
                    mGenericOnOffClient.genericOnOffSet(msg.mOnOff, onOffTID++, msg.mTransitionTime, msg.mDelay);
                    break;
                default:
                    log("resendMsg UNKNOWN OPCODE: " + msg.mMsgOpCode);
                    break;
            }
            msg.resendCnt++;
        }

        @Override
        public void run() {
            log("Timeout event id = " + mId);
            switch (mId) {
                case 0: //resend config msg timer
                    resendConfigMsg();
                    break;
                case 1: //key refresh timer
                    KeyRefreshTimeoutEvent();
                    break;
                case 2: //heart beat timer
                    heartBeatEvent();
                    break;
                case 3: //PB-GATT provision service timer
                    gattProvisionServiceEvent();
                    break;
                case 4: //PB-GATT provision timer
                    gattProvisionEvent();
                    break;
                case 5: //PB-ADV provision timer
                    advProvisionEvent();
                    break;
                case 6: //PB-GATT proxy timer
                    gattProxyServiceEvent();
                    break;
                case 7: //send message timeout timer
                    sendMessageTimeoutEvent();
                    break;
                case 8: //send message count timer
                    sendMessageCountEvent();
                    break;
                default:
                    break;
            }
        }
    }

    public class ResendConfigMsg {
        public int resendCnt = 0;
        public int msgId;

        public int mSrc;
        public int mDst;
        public int mTtl;
        public int mNetKeyIndex;
        public int mMsgOpCode;
        public ConfigMessageParams params;
        public int nodeNumber = -1;

        private Timer resendConfigTimer = new Timer();

        public ResendConfigMsg(int src, int dst, int ttl, int netKeyIdx, int msgOpCode, ConfigMessageParams params, int msgId) {
            this.msgId = msgId;
            this.mSrc = src;
            this.mDst = dst;
            this.mTtl = ttl;
            this.mNetKeyIndex = netKeyIdx;
            this.mMsgOpCode = msgOpCode;
            this.resendCnt = 0;
            this.params = params;
        }


        public ResendConfigMsg(int src, int dst, int ttl, int netKeyIdx, int msgOpCode, ConfigMessageParams params, int msgId, int nodeNumber) {
            this.msgId = msgId;
            this.mSrc = src;
            this.mDst = dst;
            this.mTtl = ttl;
            this.mNetKeyIndex = netKeyIdx;
            this.mMsgOpCode = msgOpCode;
            this.resendCnt = 0;
            this.params = params;
            this.nodeNumber = nodeNumber;
        }

        public void stopResend() {
            this.resendConfigTimer.cancel();
        }

        public void scheduleResend() {
            log("scheduleResend for msg 0x" + this.mMsgOpCode + ", id = " + this.msgId + " , address = " + this.mDst + " , nodeNumber = " + nodeNumber);
            try {
                if (nodeNumber < 0) {
                    if (mMsgOpCode == MeshConstants.MESH_MSG_CONFIG_COMPOSITION_DATA_GET) {
                        this.resendConfigTimer.schedule(new ExTimerTask(0, this), 1500, 1500);
                    } else {
                        this.resendConfigTimer.schedule(new ExTimerTask(0, this), 500, 500);
                    }
                } else {
                    this.resendConfigTimer.schedule(new ExTimerTask(0, this), nodeNumber*2000, nodeNumber*2000);
                }
            } catch (Exception e) {
                log("execute ResendConfigMsg scheduleResend error : " + e.getMessage());
            }

        }
    }

    public class ResendMsg {
        public int resendCnt = 0;
        public int msgId;
        public int mDst;

        public int mOnOff;
        public int mTid;
        public int mTransitionTime;
        public int mDelay;
        public int mMsgOpCode;

        private Timer resendMsgTimer = new Timer();

        public ResendMsg(int msgId, int dst, int onoff, int tid, int transitionTime, int delay, int msgOpCode) {
            this.msgId = msgId;
            this.mDst = dst;
            this.mOnOff = onoff;
            this.mTid = tid;
            this.mTransitionTime = transitionTime;
            this.mDelay = delay;
            this.mMsgOpCode = msgOpCode;
            this.resendCnt = 0;
        }

        public void stopResendMsg() {
            this.resendMsgTimer.cancel();
        }

        public void scheduleResendMsg() {
            log("scheduleResendMsg for msg 0x" + this.mMsgOpCode + ", id = " + this.msgId);
            try {
                /*
                if (isGroupSendMsg) { //group send Msg
                    this.resendMsgTimer.schedule(new ExTimerTask(8, this), 200*cfSuccessNodeList.size(), 200*cfSuccessNodeList.size());
                }else { //single Node send Msg
                    this.resendMsgTimer.schedule(new ExTimerTask(8, this), 200, 200);
                }
                */
                //group send Msg no need * node number
                this.resendMsgTimer.schedule(new ExTimerTask(8, this), 400, 400);
            } catch (Exception e) {
                log("execute ResendMsg scheduleResend error : " + e.getMessage());
            }
        }


    }

    private void disable() {
        log("disable lock");
        resendMsgListLock.lock();
        try {
            ListIterator<ResendConfigMsg> it = resendMsgList.listIterator(0);
            while (it.hasNext()) {
                ResendConfigMsg msg = it.next();
                msg.stopResend();
                it.remove();
            }
            //remove all timers in the mTimerMap
            Iterator<Integer> itTimer = mTimerMap.keySet().iterator();
            while (itTimer.hasNext()) {
                int timerId = itTimer.next();
                getTimerById(timerId).cancel();
                itTimer.remove();
            }
            if (resendMsg != null) {
                resendMsg.stopResendMsg();
                resendMsg = null;
            }
        } finally {
        }
        log("disable unlock");
        resendMsgListLock.unlock();

        ArrayList<Node> nodeList = new ArrayList<Node>(mNodes.values());
        if (nodeList != null && nodeList.size() > 0) {
            for (int i = 0; i < nodeList.size(); i++) {
                //set node status to be unknown, it shall be updated by heartbeat event after mesh app is re-enabled.
                nodeList.get(i).setActiveStatus(2);
                nodeList.get(i).setCurrentHeartBeatNumber(0);
                nodeList.get(i).setPreHeartBeatNumber(0);
                nodeList.get(i).setContinueLost(0);
                nodeList.get(i).setMaxLost(0);
                nodeList.get(i).setHeartBeatTime(0);
                nodeList.get(i).setHeartBeatTimerNumber(0);
            }
            //updateStorageData(MyApplication.STORAGE_DATA_NODE);
        }
    }

    public static final int MESH_ADDRESS_TYPE_UNASSIGNED = 0;   /**< unassigned address */
    public static final int MESH_ADDRESS_TYPE_UNICAST    = 1;   /**< unicast address */
    public static final int MESH_ADDRESS_TYPE_VIRTUAL    = 2;   /**< virtual address */
    public static final int MESH_ADDRESS_TYPE_GROUP      = 3;   /**< group address */

    private int parseAddressType(int addr) {
        if (addr == 0) {
            return MeshConstants.MESH_ADDRESS_TYPE_UNASSIGNED;
        } else if ((addr & 0x8000) == 0) {
            return MeshConstants.MESH_ADDRESS_TYPE_UNICAST;
        } else {
            if ((addr & 0xc000) == 0x8000) {
                return MeshConstants.MESH_ADDRESS_TYPE_VIRTUAL;
            }
            else {
                return MeshConstants.MESH_ADDRESS_TYPE_GROUP;
            }
        }
    }
    private void addResendConfigMsg(Node node, int msgOpCode, ConfigMessageParams params, int retryCount) {
        ResendConfigMsg msg = new ResendConfigMsg(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(), msgOpCode, params, resendConfigMsgID++);
        if (retryCount > CONFIG_MSG_DEFAULT_RESEND_TIMES || retryCount < 0) {
            msg.resendCnt = 0;
        } else {
            msg.resendCnt = CONFIG_MSG_DEFAULT_RESEND_TIMES - retryCount;
        }
        log("addResendConfigMsg lock");
        resendMsgListLock.lock();
        try {
            resendMsgList.add(msg);
        } finally {
        }
        resendMsgListLock.unlock();
        log("addResendConfigMsg unlock");
        msg.scheduleResend();
    }

    private synchronized void addResendConfigMsg(Node node, int msgOpCode, ConfigMessageParams params, int retryCount, int nodeNumber) {
        ResendConfigMsg msg = new ResendConfigMsg(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(), msgOpCode, params, resendConfigMsgID++, nodeNumber);
        if (retryCount > CONFIG_MSG_DEFAULT_RESEND_TIMES || retryCount < 0) {
            msg.resendCnt = 0;
        } else {
            msg.resendCnt = CONFIG_MSG_DEFAULT_RESEND_TIMES - retryCount;
        }
        log("addResendConfigMsg lock");
        resendMsgListLock.lock();
        try {
            resendMsgList.add(msg);
        } finally {
        }
        resendMsgListLock.unlock();
        log("addResendConfigMsg unlock");
        msg.scheduleResend();

    }


    private synchronized ResendConfigMsg removeResendConfigMsg(int[] opCode, int ackSrc) {
        log("removeResendConfigMsg lock");
        resendMsgListLock.lock();
        try {
            for (int i = 0; i < opCode.length; i++) {
                ListIterator<ResendConfigMsg> it = resendMsgList.listIterator(0);
                while (it.hasNext()) {
                    ResendConfigMsg msg = it.next();
                    if ((msg.mMsgOpCode == opCode[i]) && (msg.mDst == ackSrc)) {
                        log("Stop resendConfigMsg op = 0x" + Integer.toHexString(msg.mMsgOpCode) + ", msgId = " + msg.msgId + ", dst = " + MeshUtils.decimalToHexString("%04X", msg.mDst));
                        msg.stopResend();
                        it.remove();
                        log("removeResendConfigMsg unlock 1");
                        if ((getNodeByAddr(ackSrc) != null) && (getNodeByAddr(ackSrc).getConfigState() == MESH_NODE_CONFIG_STATE_IDLE)) {
                            if (isProvOrConfiging || isKeyrefreshing) {
                                log("isProvOrConfiging = " + isProvOrConfiging + ", isKeyrefreshing = " + isKeyrefreshing);
                            } else {
                                if (getMeshMode() != MeshConstants.MESH_MODE_STANDBY) {
                                    setMeshMode(MeshConstants.MESH_MODE_STANDBY);
                                }
                            }
                        }
                        resendMsgListLock.unlock();
                        return msg; //return the oldest msg belonging to the opCode[]
                    }
                }
            }
        } finally {
        }
        if ((getNodeByAddr(ackSrc) != null) && (getNodeByAddr(ackSrc).getConfigState() == MESH_NODE_CONFIG_STATE_IDLE)) {
            if (isProvOrConfiging || isKeyrefreshing) {
                log("isProvOrConfiging = " + isProvOrConfiging + ", isKeyrefreshing = " + isKeyrefreshing);
            } else {
                if (getMeshMode() != MeshConstants.MESH_MODE_STANDBY) {
                    setMeshMode(MeshConstants.MESH_MODE_STANDBY);
                }
            }
        }
        log("removeResendConfigMsg unlock");
        resendMsgListLock.unlock();
        return null;
    }

    private void addResendMsg(Node node, int onoff, int tid, int transitionTime, int delay, int msgOpCode, int retryCount) {
        log("addResendMsg, resendMsg = " + resendMsg);
        if (resendMsg != null) {
            resendMsg.stopResendMsg();
            resendMsg = null;
        }
        resendMsg = new ResendMsg(resendMsgID++, node.getAddr(), onoff, tid, transitionTime, delay, msgOpCode);
        if (retryCount > SEND_MSG_DEFAULT_RESEND_TIMES || retryCount < 0) {
            resendMsg.resendCnt = 0;
        } else {
            resendMsg.resendCnt = SEND_MSG_DEFAULT_RESEND_TIMES - retryCount;
        }
        resendMsg.scheduleResendMsg();
    }

    private void removeResendMsg(int[] opCode, int ackSrc) {
        log("removeResendMsg");
        if (resendMsg == null) {
            log("resendMsg == null , it should have stoped sendMessage in sendMessageCountEvent");
            return;
        }
        for (int i = 0; i < opCode.length; i++) {
            if ((resendMsg.mMsgOpCode == opCode[i]) && (resendMsg.mDst == ackSrc)) {
                log("removeResendMsg , found msg");
                resendMsg.stopResendMsg();
                resendMsg = null;
                return;
            }else {
                //log("removeResendMsg , not found msg");
            }
        }
    }

    public void configMsgBeaconGet(Node node) {
        log("configMsgBeaconGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_BEACON_GET);
        mConfigClient.configBeaconGet();

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_BEACON_GET, null, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgBeaconSet(Node node, int beacon) {
        log("configMsgBeaconSet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_BEACON_SET);
        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigBeaconSetParam(beacon);
        mConfigClient.configBeaconSet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_BEACON_SET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgCompositionDataGet(Node node, int page) {
        log("configMsgCompositionDataGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_COMPOSITION_DATA_GET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigCompositionDataGetParam(page);
        mConfigClient.configCompositionDataGet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_COMPOSITION_DATA_GET, params, COMPOSITIONDATA_MSG_RESEND_TIMES);
    }

    public void configMsgDefaultTTLGet(Node node) {
        log("configMsgDefaultTTLGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_DEFAULT_TTL_GET);
        mConfigClient.configDefaultTTLGet();

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_DEFAULT_TTL_GET, null, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgDefaultTTLSet(Node node, int ttl) {
        log("configMsgDefaultTTLSet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_DEFAULT_TTL_SET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigDefaultTTLSetParam(ttl);
        mConfigClient.configDefaultTTLSet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_DEFAULT_TTL_SET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgGATTProxyGet(Node node) {
        log("configMsgGATTProxyGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_GATT_PROXY_GET);
        mConfigClient.configGattProxyGet();

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_GATT_PROXY_GET, null, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgGATTProxySet(Node node, int gattProxy) {
        log("configMsgGATTProxySet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_GATT_PROXY_SET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigGattProxySetParam(gattProxy);
        mConfigClient.configGattProxySet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_GATT_PROXY_SET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgRelayGet(Node node) {
        log("configMsgRelayGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_RELAY_GET);
        mConfigClient.configRelayGet();

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_RELAY_GET, null, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgRelaySet(Node node, int relay, int rrc, int rris) {
        log("configMsgRelaySet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_RELAY_SET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigRelaySetParam(relay, rrc, rris);
        mConfigClient.configRelaySet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_RELAY_SET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgModelPubGet(Node node, int eleAddr, long modelId) {
        log("configMsgModelPubGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_MODEL_PUBLICATION_GET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigModelPubGetParam(eleAddr,modelId);
        mConfigClient.configModelPubGet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_MODEL_PUBLICATION_GET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgModelPubSet(Node node, int eleAddr, int pubAddr, int appKeyIdx, boolean credentialFlag, int pubTTL, int pubPeriod, int pubRC, int pubRIS, long modelId) {
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_MODEL_PUBLICATION_SET);

        ConfigMessageParams params = new ConfigMessageParams();
        if (MeshConstants.MESH_ADDRESS_TYPE_VIRTUAL == parseAddressType(pubAddr)) {
            log("configMsgModelPubSet, invalid publish address");
            return;
        }
        log("configMsgModelPubSet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        params.setConfigModelPubSetParam(eleAddr, parseAddressType(pubAddr), pubAddr, null, appKeyIdx, credentialFlag, pubTTL, pubPeriod, pubRC, pubRIS, modelId);
        mConfigClient.configModelPubSet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_MODEL_PUBLICATION_SET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgModelPubVAddrSet(Node node, int eleAddr, int[] labelUUID, int appKeyIdx, boolean credentialFlag, int pubTTL, int pubPeriod, int pubRC, int pubRIS, long modelId){
        log("configMsgModelPubVAddrSet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);

        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_MODEL_PUBLICATION_VIRTUAL_ADDRESS_SET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigModelPubSetParam(eleAddr, MeshConstants.MESH_ADDRESS_TYPE_VIRTUAL, 0, labelUUID, appKeyIdx, credentialFlag, pubTTL, pubPeriod, pubRC, pubRIS, modelId);
        mConfigClient.configModelPubSet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_MODEL_PUBLICATION_VIRTUAL_ADDRESS_SET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgModelSubAdd(Node node, int eleAddr, int subAddr, long modelId) {
        log("configMsgModelSubAdd to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_ADD);

        if (MeshConstants.MESH_ADDRESS_TYPE_GROUP != parseAddressType(subAddr)
            || ((subAddr & 0xFFFF) == 0xFFFF)) {
            log("configMsgModelSubAdd, invalid subscription address");
            return;
        }
        int i = 0;
        for (i = 0; i < mGroupAddrList.size(); i++) {
            if (subAddr == mGroupAddrList.get(i)) {
                break;
            }
        }
        if (i == mGroupAddrList.size()) {
            mGroupAddrList.add(subAddr);
            updateStorageData(MyApplication.STORAGE_DATA_GROUP_ADDR);
        }

        setConfigMeshMode(node);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigModelSubAddParam(eleAddr, parseAddressType(subAddr), subAddr, null, modelId);
        mConfigClient.configModelSubAdd(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_ADD, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgModelSubVAddrAdd(Node node, int eleAddr, int[] label, long modelId) {
        log("configMsgModelSubVAddrAdd to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_ADD);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigModelSubAddParam(eleAddr, MeshConstants.MESH_ADDRESS_TYPE_VIRTUAL, 0, label, modelId);
        mConfigClient.configModelSubAdd(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_ADD, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgModelSubDelete(Node node, int eleAddr, int subAddr, long modelId) {
        log("configMsgModelSubDelete to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE);

        if (MeshConstants.MESH_ADDRESS_TYPE_GROUP != parseAddressType(subAddr)
            || ((subAddr & 0xFFFF) == 0xFFFF)) {
            log("configMsgModelSubDelete, invalid subscription address");
            return;
        }
        setConfigMeshMode(node);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigModelSubDelParam(eleAddr, parseAddressType(subAddr), subAddr, null, modelId);
        mConfigClient.configModelSubDel(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgModelSubVAddrDelete(Node node, int eleAddr, int[] addr, long modelId) {
        log("configMsgModelSubVAddrDelete to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_DELETE);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigModelSubDelParam(eleAddr, MeshConstants.MESH_ADDRESS_TYPE_VIRTUAL, 0, addr, modelId);
        mConfigClient.configModelSubDel(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_DELETE, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgModelSubOw(Node node, int eleAddr, int subAddr, long modelId) {
        log("configMsgModelSubOw to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_OVERWRITE);

        if (MeshConstants.MESH_ADDRESS_TYPE_GROUP != parseAddressType(subAddr)
            || ((subAddr & 0xFFFF) == 0xFFFF)) {
            log("configMsgModelSubDelete, invalid subscription address");
            return;
        }
        setConfigMeshMode(node);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigModelSubOwParam(eleAddr, MeshConstants.MESH_ADDRESS_TYPE_GROUP, subAddr, null, modelId);
        mConfigClient.configModelSubOw(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_OVERWRITE, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgModelSubVAddrOw(Node node, int eleAddr, int[] addr, long modelId) {
        log("configMsgModelSubVAddrOw to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_OVERWRITE);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigModelSubOwParam(eleAddr, MeshConstants.MESH_ADDRESS_TYPE_VIRTUAL, 0, addr, modelId);
        mConfigClient.configModelSubOw(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_OVERWRITE, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgModelSubDeleteAll(Node node, int eleAddr, long modelId) {
        log("configMsgModelSubDeleteAll to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE_ALL);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigModelSubDelAllParam(eleAddr, modelId);
        mConfigClient.configModelSubDelAll(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE_ALL, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgSIGModelSubGet(Node node, int eleAddr, int modelId) {
        log("configMsgSIGModelSubGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_GET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigSigModelSubGetParam(eleAddr, modelId);
        mConfigClient.configSigModelSubGet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_GET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgVendorModelSubGet(Node node, int eleAddr, long modelId) {
        log("configMsgVendorModelSubGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_GET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigVendorModelSubGetParam(eleAddr, modelId);
        mConfigClient.configVendorModelSubGet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_GET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgNetKeyAdd(Node node, int targetNetkeyIdx) {
        log("configMsgNetKeyAdd to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_NETKEY_ADD);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigNetkeyAddParam(targetNetkeyIdx, mAllNetKeyMap.get(targetNetkeyIdx).getValue());
        mConfigClient.configNetkeyAdd(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_NETKEY_ADD, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);

        //add the netkey to the node appkey list, we shall remove it if the CONFIG NET KEY STATUS is failure.
        node.addNetKey(targetNetkeyIdx);
    }

    public void configMsgNetKeyUpdate(Node node, int targetNetkeyIdx , int nodeNumber) {
        log("configMsgNetKeyUpdate to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        if (getMeshMode() != MeshConstants.MESH_MODE_ON) {
            setMeshMode(MeshConstants.MESH_MODE_ON);
        }
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_NETKEY_UPDATE);

        ConfigMessageParams params = new ConfigMessageParams();
        if (null == mAllNetKeyMap.get(targetNetkeyIdx).getTempValue()) {
            log("configMsgNetKeyUpdate error, set temp value first");
            return;
        }
        params.setConfigNetkeyUpdateParam(targetNetkeyIdx, mAllNetKeyMap.get(targetNetkeyIdx).getTempValue());
        mConfigClient.configNetkeyUpdate(params);

        //addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_NETKEY_UPDATE, params, CONFIG_MSG_DEFAULT_RESEND_TIMES, nodeNumber);
        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_NETKEY_UPDATE, params, CONFIG_MSG_DEFAULT_RESEND_TIMES, nodeNumber);
    }

    public void configMsgNetKeyDelete(Node node, int targetNetkeyIdx) {
        log("configMsgNetKeyDelete to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_NETKEY_DELETE);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigNetkeyDelParam(targetNetkeyIdx);
        mConfigClient.configNetkeyDel(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_NETKEY_DELETE, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgNetKeyGet(Node node) {
        log("configMsgNetKeyGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_NETKEY_GET);

        mConfigClient.configNetkeyGet();
        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_NETKEY_GET, null, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgAppKeyAdd(Node node, int targetNetKeyIndex, int appKeyIdx) {
        log("configMsgAppKeyAdd to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_APPKEY_ADD);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigAppkeyAddParam(targetNetKeyIndex, appKeyIdx, mAllAppKeyMap.get(appKeyIdx).getValue());
        mConfigClient.configAppkeyAdd(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_APPKEY_ADD, params, CONFIG_MSG_RESEND_TIMES);

        //add the appkey to the node appkey list, we shall remove it if the CONFIG APP KEY STATUS is failure.
        node.addAppKey(appKeyIdx);
    }

    public void configMsgAppKeyUpdate(Node node, int targetNetKeyIndex, int appKeyIdx) {
        log("configMsgAppKeyUpdate to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_APPKEY_UPDATE);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigAppkeyUpdateParam(targetNetKeyIndex, appKeyIdx, mAllAppKeyMap.get(appKeyIdx).getValue());
        mConfigClient.configAppkeyUpdate(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_APPKEY_UPDATE, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgAppKeyDelete(Node node, int targetNetKeyIndex, int appKeyIdx) {
        log("configMsgAppKeyDelete to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_APPKEY_DELETE);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigAppkeyDelParam(targetNetKeyIndex, appKeyIdx);
        mConfigClient.configAppkeyDel(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_APPKEY_DELETE, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgAppKeyGet(Node node, int targetNetkeyIdx) {
        log("configMsgAppKeyGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_APPKEY_GET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigAppkeyGetParam(targetNetkeyIdx);
        mConfigClient.configAppkeyGet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_APPKEY_GET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgNodeIdGet(Node node, int targetNetkeyIdx) {
        log("configMsgNodeIdGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_NODE_IDENTITY_GET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigNodeIdentityGetParam(targetNetkeyIdx);
        mConfigClient.configNodeIdentityGet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_NODE_IDENTITY_GET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgNodeIdSet(Node node, int targetNetkeyIdx, int id) {
        log("configMsgNodeIdSet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_NODE_IDENTITY_SET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigNodeIdentitySetParam(targetNetkeyIdx, id);
        mConfigClient.configNodeIdentitySet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_NODE_IDENTITY_SET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgModelAppBind(Node node, int elementAddr, int appkeyIndex, long modelId) {
        log("configMsgModelAppBind to " + MeshUtils.decimalToHexString("%04X", node.getAddr()) + " , modelId = " + MeshUtils.decimalToHexString("%04X", modelId));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_MODEL_APP_BIND);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigModelAppBindParam(elementAddr, appkeyIndex, modelId);
        mConfigClient.configModelAppBind(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_MODEL_APP_BIND, params, CONFIG_MSG_RESEND_TIMES);

        //add the appkey to the model appkey list, we shall remove it if the CONFIG MODEL APP STATUS is failure.
        if (null != node.getElementByAddr(elementAddr)) {
            node.getElementByAddr(elementAddr).getModelById(modelId).getBoundAppKeySet().add(appkeyIndex);
        }
    }

    public void configMsgModelAppUnbind(Node node, int elementAddr, int appkeyIndex, int modelId) {
        log("configMsgModelAppUnbind to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_MODEL_APP_UNBIND);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigModelAppUnbindParam(elementAddr, appkeyIndex, modelId);
        mConfigClient.configModelAppUnbind(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_MODEL_APP_UNBIND, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgSIGModelAppGet(Node node, int eleAddr, long modelId) {
        log("configMsgVendorModelAppGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_SIG_MODEL_APP_GET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigSigModelAppGetParam(eleAddr, modelId);
        mConfigClient.configSigModelAppGet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_SIG_MODEL_APP_GET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgVendorModelAppGet(Node node, int eleAddr, long modelId) {
        log("configMsgVendorModelAppGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_VENDOR_MODEL_APP_GET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigVendorModelAppGetParam(eleAddr, modelId);
        mConfigClient.configVendorModelAppGet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_VENDOR_MODEL_APP_GET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgNodeReset(Node node, boolean retry, int nodeNumber) {
        log("configMsgNodeReset to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_NODE_RESET);

        mConfigClient.configNodeReset();
        if(retry) {
            //addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_NODE_RESET, null, CONFIG_MSG_DEFAULT_RESEND_TIMES, nodeNumber);
            addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_NODE_RESET, null, CONFIG_MSG_DEFAULT_RESEND_TIMES, nodeNumber);
        }
        deleteLocalNode(node, true);
    }

    public void groupConfigMsgNodeReset(ArrayList<Node> nodeList) {
        log("groupConfigMsgNodeReset()");
        int nodeListSize = nodeList.size();
        for (int i = 0; i < nodeListSize; i++) {
            Node node = nodeList.get(i);
            if (node.isConfigSuccess()) { // config success need send config node reset msg
                configMsgNodeReset(node, true, nodeListSize);
            } else { // config success not need send config node reset msg , only remove it
                log("node address = " + MeshUtils.decimalToHexString("%04X", node.getAddr()) + " config fail");
                deleteLocalNode(node, false);
            }
        }
        log("mNodes size = " + mNodes.size());
    }

    public void configMsgFriendGet(Node node) {
        log("configMsgFriendGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_FRIEND_GET);

        mConfigClient.configFriendGet();

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_FRIEND_GET, null, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgFriendSet(Node node, int meshFriend) {
        log("configMsgFriendSet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_FRIEND_SET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigFriendSetParam(meshFriend);
        mConfigClient.configFriendSet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_FRIEND_SET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgKeyRefreshPhaseGet(Node node, int targetNetkeyIdx) {
        log("configMsgKeyRefreshPhaseGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        if (getMeshMode() != MeshConstants.MESH_MODE_ON) {
            setMeshMode(MeshConstants.MESH_MODE_ON); // need set mesh mode when keyrefresh
        }
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_KEY_REFRESH_PHASE_GET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigKeyRefreshPhaseGetParam(targetNetkeyIdx);
        mConfigClient.configKeyRefreshPhaseGet(params);

        //addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_KEY_REFRESH_PHASE_GET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_KEY_REFRESH_PHASE_GET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgKeyRefreshPhaseSet(Node node, int targetNetkeyIdx, int transition, int nodeNumber) {
        log("configMsgKeyRefreshPhaseSet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        if (getMeshMode() != MeshConstants.MESH_MODE_ON) {
            setMeshMode(MeshConstants.MESH_MODE_ON); // need set mesh mode when keyrefresh
        }
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_KEY_REFRESH_PHASE_SET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigKeyRefreshPhaseSetParam(targetNetkeyIdx, transition);
        mConfigClient.configKeyRefreshPhaseSet(params);

        //addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_KEY_REFRESH_PHASE_SET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES, nodeNumber);
        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_KEY_REFRESH_PHASE_SET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES, nodeNumber);
    }

    public void configMsgHbPubGet(Node node) {
        log("configMsgHbPubGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_GET);

        mConfigClient.configHbPubGet();

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_GET, null, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgHbPubSet(Node node, int dest, int countLog, int periodLog, int ttl, int features, int targetNetkeyIdx) {
        log("configMsgHbPubSet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_SET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigHbPubSetParam(dest, countLog, periodLog, ttl, features, targetNetkeyIdx);
        mConfigClient.configHbPubSet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_SET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgHbSubGet(Node node) {
        log("configMsgHbSubGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_GET);

        mConfigClient.configHbSubGet();

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_GET, null, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgHbSubSet(Node node, int source, int dest, int periodLog) {
        log("configMsgHbSubSet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_SET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigHbSubSetParam(source, dest, periodLog);
        mConfigClient.configHbSubSet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_SET, null, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgLpnPollTimeGet(Node node, int lpnAddr) {
        log("configMsgLpnPollTimeGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));

        setConfigMeshMode(node);

    }

    public void configMsgNetTransmitGet(Node node) {
        log("configMsgNetTransmitGet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_NETWORK_TRANSMIT_GET);

        mConfigClient.configNetworkTransmitGet();
        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_NETWORK_TRANSMIT_GET, null, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    public void configMsgNetTransmitSet(Node node, int NTC, int NTIS) {
        log("configMsgNetTransmitSet to " + MeshUtils.decimalToHexString("%04X", node.getAddr()));
        setConfigMeshMode(node);
        mConfigClient.setConfigMessageHeader(mPrimaryElement.getAddress(), node.getAddr(), defaultTTL, node.getCurrNetkeyIndex(),
                                        MeshConstants.MESH_MSG_CONFIG_NETWORK_TRANSMIT_SET);

        ConfigMessageParams params = new ConfigMessageParams();
        params.setConfigNetworkTransmitSetParam(NTC, NTIS);
        mConfigClient.configNetworkTransmitSet(params);

        addResendConfigMsg(node, MeshConstants.MESH_MSG_CONFIG_NETWORK_TRANSMIT_SET, params, CONFIG_MSG_DEFAULT_RESEND_TIMES);
    }

    private void configClientMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
        int src = msg.getSrcAddr();
        Node srcNode = getNodeByAddr(src);

        if (srcNode == null) {
            if (msg.getOpCode() != MeshConstants.MESH_MSG_CONFIG_NODE_RESET_STATUS) {
                log("configClientMsgHandler unknown node!!!");
                return;
            }
            log("configClientMsgHandler unknown node , but it is node reset status , address = " + MeshUtils.decimalToHexString("%04X", src));
        }

        switch (msg.getOpCode()) {
            case MeshConstants.MESH_MSG_CONFIG_BEACON_STATUS: {
                log("CONFIG_BEACON_STATUS beacon = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_BEACON_GET, MeshConstants.MESH_MSG_CONFIG_BEACON_SET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_COMPOSITION_DATA_STATUS: {
                log("CONFIG_COMPOSITION_DATA_STATUS page = " + msg.getBuffer()[0] + " , config state = " + currentProvNode.getConfigState());

                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_COMPOSITION_DATA_GET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }

                if (0 == msg.getBuffer()[0]) {
                    if (src == currentProvNode.getAddr()
                        && (currentProvNode.getConfigState() != MESH_NODE_CONFIG_STATE_GET_COMPOSITION_DATA)
                        && (currentProvNode.getConfigState() != MESH_NODE_CONFIG_STATE_IDLE)) {
                        log("Current node is in config procedure, return");
                        return;
                    }
                    log("parse the node...");
                    //parseCompositionData(mNodes.get(src), msg.getBuffer());
                    parseCompositionData(mNodes.get(src), Arrays.copyOfRange(msg.getBuffer(), 1, msg.getBuffer().length));

                    if ((src == currentProvNode.getAddr()) && (currentProvNode.getConfigState() == MESH_NODE_CONFIG_STATE_GET_COMPOSITION_DATA)) {
                        currentProvNode.setConfigState(MESH_NODE_CONFIG_STATE_ADD_APPKEY);
                        notifyProvState(currentProvNode, true);
                        configMsgAppKeyAdd(currentProvNode, currentProvNode.getCurrNetkeyIndex(), currentProvNode.getNodeAppKeyList().get(0));
                    }
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_DEFAULT_TTL_STATUS: {
                log("CONFIG_DEFAULT_TTL_STATUS TTL = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_DEFAULT_TTL_GET, MeshConstants.MESH_MSG_CONFIG_DEFAULT_TTL_SET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_GATT_PROXY_STATUS: {
                log("CONFIG_GATT_PROXY_STATUS status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_GATT_PROXY_GET, MeshConstants.MESH_MSG_CONFIG_GATT_PROXY_SET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_FRIEND_STATUS: {
                log("CONFIG_FRIEND_STATUS status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_FRIEND_GET, MeshConstants.MESH_MSG_CONFIG_FRIEND_SET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_MODEL_PUBLICATION_STATUS: {
                log("CONFIG_MODEL_PUBLICATION_STATUS status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_MODEL_PUBLICATION_GET,
                                MeshConstants.MESH_MSG_CONFIG_MODEL_PUBLICATION_SET,
                                MeshConstants.MESH_MSG_CONFIG_MODEL_PUBLICATION_VIRTUAL_ADDRESS_SET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_STATUS: {
                log("CONFIG_MODEL_SUBSCRIPTION_STATUS status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_ADD,
                                MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_ADD,
                                MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE,
                                MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_DELETE,
                                MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_OVERWRITE,
                                MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_VIRTUAL_ADDRESS_OVERWRITE,
                                MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_DELETE_ALL};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_NETWORK_TRANSMIT_STATUS: {
                log("CONFIG_NETWORK_TRANSMIT_STATUS status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_NETWORK_TRANSMIT_GET,
                                MeshConstants.MESH_MSG_CONFIG_NETWORK_TRANSMIT_SET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_RELAY_STATUS: {
                log("CONFIG_RELAY_STATUS status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_RELAY_GET,
                                MeshConstants.MESH_MSG_CONFIG_RELAY_SET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_LIST: {
                log("CONFIG_SIG_MODEL_SUBSCRIPTION_LIST status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_GET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_LIST: {
                log("CONFIG_VENDOR_MODEL_SUBSCRIPTION_LIST status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_GET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_NETKEY_LIST: {
                log("CONFIG_NETKEY_LIST status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_NETKEY_GET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_NETKEY_STATUS: {
                log("CONFIG_NETKEY_STATUS status code = " + msg.getBuffer()[0] + ", netkeyIndex = " +
                    (msg.getBuffer()[1] + (msg.getBuffer()[2] << 8)) + " , address = " + MeshUtils.decimalToHexString("%04X", src));
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_NETKEY_ADD,
                                MeshConstants.MESH_MSG_CONFIG_NETKEY_UPDATE,
                                MeshConstants.MESH_MSG_CONFIG_NETKEY_DELETE};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                int netKeyIndex = msg.getBuffer()[1] + (msg.getBuffer()[2] << 8);

                NetKey netKey = mAllNetKeyMap.get(netKeyIndex);
                if (null == netKey) {
                    log("netkey is not found");
                    return;
                }

                if (0 != msg.getBuffer()[0]) {
                    //TODO remove the added net key due to adding failure
                    // if fail , need get refresh phase state
                    log("netKey.getState() = " + netKey.getState() + " , contain src = " + netKey.getKeyRefreshAckNodes().containsKey(src));
                    if ((netKey.getState() == MeshConstants.MESH_KEY_REFRESH_STATE_1) && (!netKey.getKeyRefreshAckNodes().containsKey(src))) {
                        netKey.getKeyRefreshAckNodes().put(src, getNodeByAddr(src));
                        configMsgKeyRefreshPhaseGet(srcNode,netKeyIndex);
                        log("keyrefresh node size = " + netKey.getKeyRefreshNodes().size() + " , ack node size = " + netKey.getKeyRefreshAckNodes().size()
                            + " , address = " + MeshUtils.decimalToHexString("%04X", src) + " , state = " + netKey.getState() + " , code = " + msg.getBuffer()[0]);
                        if (netKey.getKeyRefreshAckNodes().size() >= netKey.getKeyRefreshNodes().size()) {
                            Set<Integer> set = netKey.getKeyRefreshNodes().keySet();
                            for (Iterator it = set.iterator(); it.hasNext();) {
                                if (null == netKey.getKeyRefreshAckNodes().get((Integer)it.next())) {
                                    //not all the kr nodes replied
                                    log("not all the kr nodes replied");
                                    return;
                                }
                            }
                            //all the kr nodes replied, switch to use new key
                            log("key refresh procedure step 3 --> start to use new key value");
                            mBluetoothMesh.setNetkey(MeshConstants.MESH_KEY_OP_USE_NEW_NETKEY, netKey.getTempValue(), netKeyIndex);
                        }
                    }
                } else {
                    log("netkey.getState = " + netKey.getState() + " , src = " + MeshUtils.decimalToHexString("%04X", src));
                    if (netKey.getState() == MeshConstants.MESH_KEY_REFRESH_STATE_1) {
                        if (netKey.getKeyRefreshAckNodes().containsKey(src)) {
                            log("ack nodes has contain src , src = " + MeshUtils.decimalToHexString("%04X", src));
                            return;
                        }
                        netKey.getKeyRefreshAckNodes().put(src, getNodeByAddr(src));
                        log("keyrefresh node size = " + netKey.getKeyRefreshNodes().size() + " , ack node size = " + netKey.getKeyRefreshAckNodes().size()
                            + " , address = " + MeshUtils.decimalToHexString("%04X", src) + " , state = " + netKey.getState() + " , code = " + msg.getBuffer()[0]);
                        if (netKey.getKeyRefreshAckNodes().size() >= netKey.getKeyRefreshNodes().size()) {
                            Set<Integer> set = netKey.getKeyRefreshNodes().keySet();
                            for (Iterator it = set.iterator(); it.hasNext();) {
                                if (null == netKey.getKeyRefreshAckNodes().get((Integer)it.next())) {
                                    //not all the kr nodes replied
                                    log("not all the kr nodes replied");
                                    return;
                                }
                            }
                            //all the kr nodes replied, switch to use new key
                            log("key refresh procedure step 3 --> start to use new key value");
                            mBluetoothMesh.setNetkey(MeshConstants.MESH_KEY_OP_USE_NEW_NETKEY, netKey.getTempValue(), netKeyIndex);
                        }
                    }
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_APPKEY_LIST: {
                log("CONFIG_APPKEY_LIST status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_NETKEY_GET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_APPKEY_STATUS: {
                log("CONFIG_APPKEY_STATUS status code = " + msg.getBuffer()[0] + " , config state = " + srcNode.getConfigState());

                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_APPKEY_ADD,
                                MeshConstants.MESH_MSG_CONFIG_APPKEY_UPDATE,
                                MeshConstants.MESH_MSG_CONFIG_APPKEY_DELETE};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }

                if ((src == currentProvNode.getAddr()) && (currentProvNode.getConfigState() == MESH_NODE_CONFIG_STATE_ADD_APPKEY)) {
                    if (0 == msg.getBuffer()[0]) {
                        currentProvNode.setConfigState(MESH_NODE_CONFIG_STATE_MODEL_APP_BIND);
                        notifyProvState(currentProvNode, true);
                        configMsgModelAppBind(currentProvNode, currentProvNode.getElements().get(0).getAddress(),
                                            currentProvNode.getNodeAppKeyList().get(0), currentProvNode.getModels().get(0).getID());
                    } else {
                        log("config node failed due to AppKeyAdd fail");
                        currentProvNode.getNodeAppKeyList().remove(0);  //remove the appkey for the node
                        try {
                            getTimerById(currentProvNode.getConfigTimerID()).cancel();
                            mTimerMap.remove(currentProvNode.getConfigTimerID());
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                        MyApplication.configTime = System.currentTimeMillis() - MyApplication.configTime;
                        double configTime = MeshUtils.getDoubleDecimal(MyApplication.configTime*1.0/1000);
                        currentProvNode.setConfigSuccess(false);
                        currentProvNode.setConfigTime(configTime);
                        currentProvNode.setConfigState(MESH_NODE_CONFIG_STATE_IDLE);
                        if(currentProvNode.getNodeBearer() == MeshConstants.MESH_BEARER_GATT) {
                            mBluetoothMesh.gattDisconnect();
                        }
                        notifyProvState(currentProvNode, false);
                        updateNodeData(new NodeData(currentProvNode), MyApplication.NODE_DATA_UPDATE);
                        //mBluetoothMesh.setMeshMode(MeshConstants.MESH_MODE_STANDBY);
                        //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                    }
                }
                else {
                    if (0 != msg.getBuffer()[0]) {
                        //TODO remove the added app key due to adding failure
                    }
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_MODEL_APP_STATUS: {
                log("CONFIG_MODEL_APP_STATUS status code = " + msg.getBuffer()[0] + " , config state = " + srcNode.getConfigState());

                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_MODEL_APP_BIND,
                                MeshConstants.MESH_MSG_CONFIG_MODEL_APP_UNBIND};
                //removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }

                int element_addr = msg.getBuffer()[1] + (msg.getBuffer()[2] << 8);
                int appkey_idx = msg.getBuffer()[3] + (msg.getBuffer()[4] << 8);
                long model_id = 0;
                if (7 == msg.getBuffer().length) {//SIG model
                    model_id = msg.getBuffer()[5] + (msg.getBuffer()[6] << 8);
                }
                else if (9 == msg.getBuffer().length) {    //Vendor model
                    model_id = msg.getBuffer()[5] + (msg.getBuffer()[6] << 8) + (msg.getBuffer()[7] << 16) + (msg.getBuffer()[8] << 24);
                }
                log("element_addr = " + element_addr + ", appkey_idx = " + appkey_idx + ", model_id = " + model_id);
                if ((src == currentProvNode.getAddr()) && (currentProvNode.getConfigState() == MESH_NODE_CONFIG_STATE_MODEL_APP_BIND)) {
                    int i = 0;
                    int modelSize = currentProvNode.getModels().size();
                    for (i = 0; i < modelSize; i++) {
                        if ((element_addr == currentProvNode.getModels().get(i).getElementAddr())
                            && (model_id == currentProvNode.getModels().get(i).getID())) {
                            if (0 != msg.getBuffer()[0]) {
                                //remove the bound app key due to adding failure
                                currentProvNode.getModels().get(i).getBoundAppKeySet().remove(Integer.valueOf(appkey_idx));
                            }
                            if ((element_addr == currentProvNode.getModels().get(i).getElementAddr())
                                && currentProvNode.getModels().get(i).isBindAppkey()) {
                                return;
                            } else {
                                currentProvNode.getModels().get(i).setBindAppkey(true);
                                i++;
                            }
                            break;
                        }
                    }
                    removeResendConfigMsg(opcodes, src);
                    log("modelSize = " + modelSize + " , i = " + i);
                    if (i < modelSize && currentProvNode.getModels().get(i).getID() == 0x0000) { //ConfigrationServerModel ID
                        log(" ConfigrationServerModel ID , not need bind appkey");
                        i++;
                    }
                    if (i < modelSize) {
                        log("goto next Model APP Bind , i = " + i);
                        configMsgModelAppBind(currentProvNode, currentProvNode.getModels().get(i).getElementAddr(),
                                            currentProvNode.getNodeAppKeyList().get(0), currentProvNode.getModels().get(i).getID());
                    } else {
                        log("Configuration DONE, node: " + src);
                        try {
                            getTimerById(currentProvNode.getConfigTimerID()).cancel();
                            mTimerMap.remove(currentProvNode.getConfigTimerID());
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                        MyApplication.configTime = System.currentTimeMillis() - MyApplication.configTime;
                        double configTime = MeshUtils.getDoubleDecimal(MyApplication.configTime*1.0/1000);
                        currentProvNode.setConfigSuccess(true);
                        currentProvNode.setConfigTime(configTime);
                        currentProvNode.setConfigState(MESH_NODE_CONFIG_STATE_IDLE);
                        for (int j = 0; j < currentProvNode.getModels().size(); j++) {
                            currentProvNode.getModels().get(j).setBindAppkey(false);
                        }
                        if(currentProvNode.getNodeBearer() == MeshConstants.MESH_BEARER_GATT) {
                            mBluetoothMesh.gattDisconnect();
                        }

                        //config heartbeat
                        configMsgHbPubSet(currentProvNode, mPrimaryElement.getAddress(), 0xFF, mHearbeatPeriodNum, defaultTTL, 0x0F, currentProvNode.getCurrNetkeyIndex());
                        getTimerById(currentProvNode.getHeartBeatTimerID()).schedule(new ExTimerTask<Node>(2, currentProvNode), 40000,40000);
                        configMsgModelSubAdd(currentProvNode, mPrimaryElement.getAddress(), 0xC001, MeshConstants.MESH_MODEL_SIG_MODEL_ID_GENERIC_ONOFF_SERVER);
                        configMsgModelSubAdd(currentProvNode, currentProvNode.getAddr(), 0xC002, MeshConstants.MESH_MODEL_SIG_MODEL_ID_OBJECT_TRANSFER_SERVER); //for ota test

                        notifyProvState(currentProvNode, true);
                        //mBluetoothMesh.setMeshMode(MeshConstants.MESH_MODE_STANDBY);
                        //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                        updateNodeData(new NodeData(currentProvNode), MyApplication.NODE_DATA_UPDATE); //config success
                        return;
                    }
                }
                else {
                    removeResendConfigMsg(opcodes, src);
                    if (0 != msg.getBuffer()[0]) {
                        //TODO remove the bound app key due to adding failure
                    }
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_SIG_MODEL_APP_LIST: {
                log("CONFIG_SIG_MODEL_APP_LIST status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_SIG_MODEL_APP_GET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_VENDOR_MODEL_APP_LIST: {
                log("CONFIG_VENDOR_MODEL_APP_LIST status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_VENDOR_MODEL_APP_GET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_NODE_IDENTITY_STATUS: {
                log("CONFIG_NODE_IDENTITY_STATUS status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_NODE_IDENTITY_GET, MeshConstants.MESH_MSG_CONFIG_NODE_IDENTITY_SET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_NODE_RESET_STATUS: {
                log("CONFIG_NODE_RESET_STATUS\n");
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_NODE_RESET};
                removeResendConfigMsg(opcodes, src);
                mBluetoothMesh.delDevKey(src);
                if (mBluetoothMeshListener != null) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_KEY_REFRESH_PHASE_STATUS: {
                int netKeyIndex = msg.getBuffer()[1] +(msg.getBuffer()[2] << 8);
                int phase = msg.getBuffer()[3];

                log("CONFIG_KEY_REFRESH_PHASE_STATUS status code = " + msg.getBuffer()[0] +
                    ", netKeyIndex = " + netKeyIndex + ", phase = " + phase + " , address = " + MeshUtils.decimalToHexString("%04X", src));
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_KEY_REFRESH_PHASE_GET, MeshConstants.MESH_MSG_CONFIG_KEY_REFRESH_PHASE_SET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }

                NetKey netKey = mAllNetKeyMap.get(netKeyIndex);
                if (null == netKey) {
                    log("netkey is not found");
                    return;
                }

                if(netKey.getState() == MeshConstants.MESH_KEY_REFRESH_STATE_1 && phase == 2 && msg.getBuffer()[0] ==0) {
                    if (phaseOneList.containsKey(src)) {
                        log("CONFIG_NETKEY_STATUS status code ! = 0 , and alread contain");
                        return;
                    }
                    log("CONFIG_NETKEY_STATUS status code ! = 0");
                    phaseOneList.put(src, src);
                } else if(netKey.getState() == MeshConstants.MESH_KEY_REFRESH_STATE_2 && phase == 2){
                    Set<Integer> set = phaseOneList.keySet();
                    for (Iterator it = set.iterator(); it.hasNext();) {
                        int phaseOneSrc = (Integer)it.next();
                        netKey.getKeyRefreshAckNodes().put(phaseOneSrc, getNodeByAddr(phaseOneSrc));
                    }
                    phaseOneList.clear();
                } else {
                    phaseOneList.clear();
                }

                if (0 == msg.getBuffer()[0]) {
                    if ((netKey.getState() == MeshConstants.MESH_KEY_REFRESH_STATE_1) ||
                        (netKey.getState() == MeshConstants.MESH_KEY_REFRESH_STATE_2) ||
                        (netKey.getState() == MeshConstants.MESH_KEY_REFRESH_STATE_3)) {
                        if (netKey.getKeyRefreshAckNodes().containsKey(src)) {
                            log("ack nodes has contain src , src = " + MeshUtils.decimalToHexString("%04X", src));
                            return;
                        }
                        netKey.getKeyRefreshAckNodes().put(src, getNodeByAddr(src));
                        log("keyrefresh node size = " + netKey.getKeyRefreshNodes().size() + " , ack node size = " + netKey.getKeyRefreshAckNodes().size()
                            + " , address = " + MeshUtils.decimalToHexString("%04X", src) + " , state = " + netKey.getState());
                        if (netKey.getKeyRefreshAckNodes().size() >= netKey.getKeyRefreshNodes().size()) {
                            Set<Integer> set = netKey.getKeyRefreshNodes().keySet();
                            for (Iterator it = set.iterator(); it.hasNext();) {
                                if (null == netKey.getKeyRefreshAckNodes().get((Integer)it.next())) {
                                    //not all the kr nodes replied
                                    log("not all the kr nodes replied");
                                    return;
                                }
                            }
                            if (netKey.getState() == MeshConstants.MESH_KEY_REFRESH_STATE_1) {
                                /*
                                //all the kr nodes replied, switch to use new key
                                log("key refresh procedure step 3 --> start to use new key value");
                                log("netkey keyrefreshTimerID = " + getTimerById(netKey.getKeyRefreshTimerID()));
                                if (getTimerById(netKey.getKeyRefreshTimerID()) != null) {
                                    getTimerById(netKey.getKeyRefreshTimerID()).cancel();
                                    mTimerMap.remove(netKey.getKeyRefreshTimerID());
                                }

                                mBluetoothMesh.setNetkey(MeshConstants.MESH_KEY_OP_USE_NEW_NETKEY, netKey.getTempValue(), netKeyIndex);

                                int id = createTimer();
                                if (-1 != id) {
                                    netKey.setKeyRefreshTimerID(id);
                                    try {
                                        if (netKey.getKeyRefreshNodes().size() == 0) {
                                            getTimerById(netKey.getKeyRefreshTimerID()).schedule(new ExTimerTask<NetKey>(1, netKey), 5000);
                                        } else {
                                            getTimerById(netKey.getKeyRefreshTimerID()).schedule(
                                                new ExTimerTask<NetKey>(1, netKey), (40000 + netKey.getKeyRefreshNodes().size()*5000));
                                        }
                                    } catch (Exception e) {
                                        e.printStackTrace();
                                    }
                                }
                                   */
                            } else if (netKey.getState() == MeshConstants.MESH_KEY_REFRESH_STATE_2) {
                                //Trigger to switch to Key Refresh Phase3,  onKeyRefresh() event with MESH_KEY_REFRESH_STATE_NONE will be received.
                                log("key refresh procedure step 5 --> revoke old key");
                                log("netkey keyrefreshTimerID = " + getTimerById(netKey.getKeyRefreshTimerID()));
                                if (getTimerById(netKey.getKeyRefreshTimerID()) != null) {
                                    getTimerById(netKey.getKeyRefreshTimerID()).cancel();
                                    mTimerMap.remove(netKey.getKeyRefreshTimerID());
                                }

                                int id = createTimer();
                                if (-1 != id) {
                                    netKey.setKeyRefreshTimerID(id);
                                    try {
                                        if (netKey.getKeyRefreshNodes().size() == 0) {
                                            getTimerById(netKey.getKeyRefreshTimerID()).schedule(new ExTimerTask<NetKey>(1, netKey), 5000);
                                        } else {
                                            getTimerById(netKey.getKeyRefreshTimerID()).schedule(
                                                new ExTimerTask<NetKey>(1, netKey), (5000 + netKey.getKeyRefreshNodes().size()*2000*5));
                                        }
                                    } catch (Exception e) {
                                        e.printStackTrace();
                                    }
                                }

                                mBluetoothMesh.setNetkey(MeshConstants.MESH_KEY_OP_REVOKE_OLD_NETKEY, netKey.getTempValue(), netKeyIndex);

                            } else if (netKey.getState() == MeshConstants.MESH_KEY_REFRESH_STATE_3) {
                                log("key refresh procedure step 7 --> success , configClientMsgHandler");
                                //not remove local blacklisted node with this netkey
                                //deleteBlacklistNodes(netKey.getIndex());
                                netKey.setNetKeyMeta(netKey.getIndex(), netKey.getTempValue());
                                netKey.getKeyRefreshAckNodes().clear();
                                netKey.getNodes().clear();
                                netKey.getNodes().putAll(netKey.getKeyRefreshNodes());
                                netKey.getKeyRefreshNodes().clear();
                                try {
                                    log("netkey keyrefreshTimerID = " + getTimerById(netKey.getKeyRefreshTimerID()));
                                    if (getTimerById(netKey.getKeyRefreshTimerID()) != null) {
                                        getTimerById(netKey.getKeyRefreshTimerID()).cancel();
                                        mTimerMap.remove(netKey.getKeyRefreshTimerID());
                                    }
                                } catch (Exception e) {
                                    e.printStackTrace();
                                }
                                netKey.setState(MeshConstants.MESH_KEY_REFRESH_STATE_NONE);
                                updateStorageData(MyApplication.STORAGE_DATA_NETKEY);
                                if (mBluetoothMeshListener != null) {
                                    mBluetoothMeshListener.onKeyRefreshComplete(netKey.getIndex(), true);
                                }
                            }
                        }
                    }
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_STATUS: {
                log("CONFIG_HEARTBEAT_PUBLICATION_STATUS status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_GET, MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_SET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            case MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_STATUS: {
                log("CONFIG_HEARTBEAT_SUBSCRIPTION_STATUS status code = " + msg.getBuffer()[0]);
                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_GET, MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_SET};
                removeResendConfigMsg(opcodes, src);
                if ((srcNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && (mBluetoothMeshListener != null)) {
                    mBluetoothMeshListener.onConfigMsgAck(msg);
                    //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                }
                break;
            }
            default: {
                break;
            }
        }
    }

    private String getUuidOrGattAddrByNodeAddr(int addr) {
        Node mNode = getNodeByAddr(addr);
        if (mNode == null) {
            log("getUuidOrGattAddrByNodeAddr , mNode = null");
            return null;
        }
        int[] uuid = mNode.getUUID();
        String gattAddress = mNode.getGattAddr();
        log("getUuidOrGattAddrByNodeAddr , uuid = " + uuid + " , gattAddress = " + gattAddress);
        if (uuid != null && gattAddress == null) {
            return MeshUtils.intArrayToString(uuid, true);
        } else if (uuid == null && gattAddress != null) {
            return gattAddress;
        } else {
            return null;
        }

    }

    private void genericOnOffClientMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
        int msgOpCode = msg.getOpCode();
        int src = msg.getSrcAddr();
        log("genericOnOffClientMsgHandler msgOpCode = " + MeshUtils.decimalToHexString("%04X", msgOpCode) + " , src = " + MeshUtils.decimalToHexString("%04X", src) + " , isGroupSend = " + isGroupSendMsg);
        if (isGroupSendMsg) { // group send msg
            if (!cfSuccessNodeList.contains(src)) {
                log("this node isn't belong cfSuccessNodeList");
                return;
            }
            if (ackGroupSendMsgList.contains(src)) {
                log("ackGroupSendMsgList already contain src");
                return;
            }
            ackGroupSendMsgList.add(src);
            String uuidOrGattAddr = getUuidOrGattAddrByNodeAddr(src);
            log("uuidOrGattAddr = " + uuidOrGattAddr);
            if (uuidOrGattAddr != null) {
                groupMsgTimeList.add(new StringBuffer().append(uuidOrGattAddr).append(" : ")
                    .append((System.currentTimeMillis() - MyApplication.sendMessageTime)*1.0/1000).append("s").toString());
            }
            if (ackGroupSendMsgList.size() == cfSuccessNodeList.size()) {

                MyApplication.groupSendMessageTime = System.currentTimeMillis() - MyApplication.groupSendMessageTime;

                //all node send msg ack, cancel send msg timer
                if (null != getTimerById(currentProvNode.getSendMessageTimerID())) {
                    getTimerById(currentProvNode.getSendMessageTimerID()).cancel();
                    mTimerMap.remove(currentProvNode.getSendMessageTimerID());
                    currentProvNode.setSendMessageTimerID(-1);
                }

                if (getMeshMode() != MeshConstants.MESH_MODE_STANDBY) {
                    log("Set Mesh Mode Standby");
                    setMeshMode(MeshConstants.MESH_MODE_STANDBY);
                }
                switch (msg.getOpCode()) {
                    case MeshConstants.MESH_MSG_GENERIC_ONOFF_STATUS: {
                        int[] opcodes = {MeshConstants.MESH_MSG_GENERIC_ONOFF_GET, MeshConstants.MESH_MSG_GENERIC_ONOFF_SET, MeshConstants.MESH_MSG_GENERIC_ONOFF_SET_UNRELIABLE};
                        removeResendMsg(opcodes, groupSMNodeAddr);
                        break;
                    }
                    default:
                        break;
                }
                log("adb group send message , all node ack , result as follows:");
                StringBuilder sb = new StringBuilder();
                for (int i = 0; i < ackGroupSendMsgList.size(); i++) {
                    Node ackNode = getNodeByAddr(ackGroupSendMsgList.get(i));
                    int[] uuid = ackNode.getUUID();
                    String gattAddress = ackNode.getGattAddr();
                    if (uuid != null && gattAddress == null) {
                        sb.append("ack node ").append(i + 1).append(" = ").append(MeshUtils.intArrayToString(uuid, true));
                    }else if (uuid == null && gattAddress != null) {
                        sb.append("ack node ").append(i + 1).append(" = ").append(gattAddress);
                    }
                    log(sb.toString());
                    sb.delete(0, sb.length());
                }

                if (mBluetoothMeshListener != null) {
                    isGroupSendMsging = false;
                    mBluetoothMeshListener.onGroupSendMsg(MyApplication.GROUP_SEND_MSG_END, groupMsgTimeList);
                }

            }
        } else { // single node send msg
            if (ackNodeSendMsgList.contains(src)) {
                log("already contain src");
                return;
            }
            MyApplication.sendMessageTime = System.currentTimeMillis() - MyApplication.sendMessageTime;
            ackNodeSendMsgList.add(src);
            if (getMeshMode() != MeshConstants.MESH_MODE_STANDBY) {
                log("Set Mesh Mode Standby");
                setMeshMode(MeshConstants.MESH_MODE_STANDBY);
            }
            if (getTimerById(currentProvNode.getSendMessageTimerID()) != null) {
                getTimerById(currentProvNode.getSendMessageTimerID()).cancel(); // cancel sendMessageTimer
                mTimerMap.remove(currentProvNode.getSendMessageTimerID());
                currentProvNode.setSendMessageTimerID(-1);
            }
            switch (msg.getOpCode()) {
                case MeshConstants.MESH_MSG_GENERIC_ONOFF_STATUS: {
                    int[] opcodes = {MeshConstants.MESH_MSG_GENERIC_ONOFF_GET, MeshConstants.MESH_MSG_GENERIC_ONOFF_SET, MeshConstants.MESH_MSG_GENERIC_ONOFF_SET_UNRELIABLE};
                    removeResendMsg(opcodes, src);
                    if (msg.getBuffer().length == 1) {
                        log("MESH_MSG_GENERIC_ONOFF_STATUS presentOnOff = " + msg.getBuffer()[0]);
                    } else if (msg.getBuffer().length == 2) {
                        log("MESH_MSG_GENERIC_ONOFF_STATUS presentOnOff = " + msg.getBuffer()[0] + ", TargetOnOff = " + msg.getBuffer()[1]);
                    } else if (msg.getBuffer().length == 3) {
                        log("MESH_MSG_GENERIC_ONOFF_STATUS presentOnOff = " + msg.getBuffer()[0] + ", TargetOnOff = " + msg.getBuffer()[1] +
                            ", RemaningTime = " + msg.getBuffer()[2]);
                    } else {
                        log("No valid data");
                        return;
                    }
                    if (mBluetoothMeshListener != null) {
                        mBluetoothMeshListener.onNodeMsgRecieved(msg);
                    }
                    break;
                }
                default:
                    break;
            }

        }
    }

    private void lightnessClientMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
        log("genericOnOffClientMsgHandler msgOpCode = 0x" + Integer.toHexString(msg.getOpCode()));
        switch (msg.getOpCode()) {
            default:
                break;
        }
    }

    private void ctlClientMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
        log("genericOnOffClientMsgHandler msgOpCode = 0x" + Integer.toHexString(msg.getOpCode()));
        switch (msg.getOpCode()) {
            case MeshConstants.MESH_MSG_GENERIC_ONOFF_STATUS: {
                log("MESH_MSG_GENERIC_ONOFF_STATUS presentOnOff = " + msg.getBuffer()[0] + ", TargetOnOff = " + msg.getBuffer()[1] + ", RemaningTime = " + msg.getBuffer()[2]);
                break;
            }
            default:
                break;
        }
    }

    private void hslClientMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
        log("genericOnOffClientMsgHandler msgOpCode = 0x" + Integer.toHexString(msg.getOpCode()));
        switch (msg.getOpCode()) {
            case MeshConstants.MESH_MSG_GENERIC_ONOFF_STATUS: {
                log("MESH_MSG_GENERIC_ONOFF_STATUS presentOnOff = " + msg.getBuffer()[0] + ", TargetOnOff = " + msg.getBuffer()[1] + ", RemaningTime = " + msg.getBuffer()[2]);
                break;
            }
            default:
                break;
        }
    }

    private void vendorModelClientMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
        log("vendorModelClientMsgHandler msgOpCode = 0x" + Integer.toHexString(msg.getOpCode()));
        switch (msg.getOpCode()) {
            case VendorModelDemoClient.VENDOR_MSG_OPCODE_STATE_STATUS: {
                log("VENDOR_MSG_OPCODE_STATE_STATUS value = " + msg.getBuffer()[0]);
                break;
            }
            default:
                break;
        }
    }

    private void createUUID() {
        int[] macValue = new int[6];
        BluetoothAdapter ba = BluetoothAdapter.getDefaultAdapter();
        String mac = ba.getAddress();
        String[] str = mac.split(":");

        for (int i = 0; i < str.length && i < macValue.length; i++) {
            macValue[i] = Integer.parseInt(str[i], 16);
        }

        System.arraycopy(macValue, 0, this.mMyUUID, 0, 6);
        System.arraycopy(macValue, 0, this.mMyUUID, 6, 6);
        System.arraycopy(macValue, 0, this.mMyUUID, 12, 4);
        log("My UUID = " + MeshUtils.intArrayToString(this.mMyUUID, true));
    }

    public int[] getUUID() {
        return this.mMyUUID;
    }

    private void parsePropertyFile() {
        log("parsePropertyFile");
        InputStream ins = null;
        try {
            ins = new BufferedInputStream(new FileInputStream(filePath));
            Properties p = new Properties();
            p.load(ins);
            if (null != p.getProperty("primary_element_addr")) {
                mMyAddr = Integer.valueOf(p.getProperty("primary_element_addr")).intValue();
                log("primary_element_addr=" + p.getProperty("primary_element_addr"));
            }
            if (null != p.getProperty("last_element_addr")) {
                mLastElementAddr = Integer.valueOf(p.getProperty("last_element_addr")).intValue();
                log("last_element_addr=" + p.getProperty("last_element_addr"));
            }
            if (null != p.getProperty("lpn_number")) {
                mLPNNumber = Integer.valueOf(p.getProperty("lpn_number")).intValue();
                log("lpn_number=" + p.getProperty("lpn_number"));
            }
            if (null != p.getProperty("lpn_message_queue_size")) {
                mLPNMessageQueueSize = Integer.valueOf(p.getProperty("lpn_message_queue_size")).intValue();
                log("lpn_message_queue_size=" + p.getProperty("lpn_message_queue_size"));
            }
            if (null != p.getProperty("lpn_subscription_list_size")) {
                mLPNSubscriptionListSize = Integer.valueOf(p.getProperty("lpn_subscription_list_size")).intValue();
                log("lpn_subscription_list_size=" + p.getProperty("lpn_subscription_list_size"));
            }
            if (null != p.getProperty("remote_node_number")) {
                mRemoteNodeNumber = Integer.valueOf(p.getProperty("remote_node_number")).intValue();
                log("remote_node_number=" + p.getProperty("remote_node_number"));
            }
            if (null != p.getProperty("save_flash")) {
                mSaveFlash = Integer.valueOf(p.getProperty("save_flash")).intValue();
                log("save_flash=" + p.getProperty("save_flash"));
            }
            if (null != p.getProperty("heartbeat_period_num")) {
                mHearbeatPeriodNum= Integer.valueOf(p.getProperty("heartbeat_period_num")).intValue();
                log("heartbeat_period=" + p.getProperty("heartbeat_period_num"));
            }
            if (null != p.getProperty("heartbeat_timeout")) {
                mHeartbeatTimeout = Integer.valueOf(p.getProperty("heartbeat_timeout")).intValue();
                log("heartbeat_timeout=" + p.getProperty("heartbeat_timeout"));
            }

            if (null != p.getProperty("auth_value")) {
                String str = p.getProperty("auth_value");
                String[] strArray = str.split(" ");
                for (int i = 0; i < 16; i++) {
                    mDefaultAuthValue[i] = Integer.valueOf(strArray[i]).intValue();
                }
                log("Default AuthValue = " + MeshUtils.intArrayToString(mDefaultAuthValue, false));
            }
            if (null != p.getProperty("mesh_device_number")) {
                mMeshNumber = Integer.valueOf(p.getProperty("mesh_device_number")).intValue();
                log("mesh_device_number=" + p.getProperty("mesh_device_number"));
            }
            for (int i = 0; i < mMeshNumber; i++) {
                String uuidId = "mesh_device_uuid_" + (i + 1);
                String uuid = p.getProperty(uuidId);
                log(uuidId + " = " + uuid);
                if (null != uuid) {
                    uuidList.add(uuid);
                }
            }
            if (null != p.getProperty("node_reset_device_number")) {
                nodeResetNumber = Integer.valueOf(p.getProperty("node_reset_device_number")).intValue();
                log("node_reset_device_number=" + p.getProperty("node_reset_device_number"));
            }
            for (int i = 0; i < nodeResetNumber; i++) {
                String nodeResetId = "node_reset_device_" + (i + 1);
                String nodeResetStr = p.getProperty(nodeResetId);
                log(nodeResetId + " = " + nodeResetStr);
                if (null != nodeResetStr) {
                    nodeResetList.add(nodeResetStr);
                }
            }

        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (ins != null) {
                try {
                    ins.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    private void initialize() {
        log("initialize isInitialized = " + isInitialized);
        if (!isInitialized) {
            log("mStorageData.getAllNetKey().size()= " + mStorageData.getAllNetKey().size());
            if (mStorageData.getAllNetKey().size() == 0) {
                log("Initialize mandatory data");

                //add the default group address
                mGroupAddrList.add(0);      //add an invalid address for list view
                mGroupAddrList.add(0xFFFF); //all nodes
                mGroupAddrList.add(0xFFFE); //all relays
                mGroupAddrList.add(0xFFFD); //all friends
                mGroupAddrList.add(0xFFFC); //all proxy
                mStorageData.setAllGroupAddrList(mGroupAddrList);
                updateStorageData(MyApplication.STORAGE_DATA_GROUP_ADDR);

                addNewNetKey();
                addNewAppKey(MeshConstants.MESH_PRIMARY_NETWORK_KEY_INDEX);
            } else {
                log("storage data has been recovered from database");
            }

            //mBluetoothMesh.registerCallback(mBluetoothMeshCallback);

            //instantiate  SIG models
                mConfigServer = new ConfigurationServerModel(mBluetoothMesh);
            mPrimaryElementSIGModels.put(MeshConstants.MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_SERVER, mConfigServer);

                mHealthServer = new HealthServerModel(mBluetoothMesh);
            mPrimaryElementSIGModels.put(MeshConstants.MESH_MODEL_SIG_MODEL_ID_HEALTH_SERVER, mHealthServer);

                mConfigClient = new ConfigurationClientModel(mBluetoothMesh) {
                    @Override
                    public void onMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
                        configClientMsgHandler(modelHandle, msg);
                    }
                };
            mPrimaryElementSIGModels.put(MeshConstants.MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_CLIENT, mConfigClient);

                mGenericOnOffClient = new GenericOnOffClientModel(mBluetoothMesh) {
                    @Override
                    public void onMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
                        genericOnOffClientMsgHandler(modelHandle, msg);
                    }
                };
            mPrimaryElementSIGModels.put(MeshConstants.MESH_MODEL_SIG_MODEL_ID_GENERIC_ONOFF_CLIENT, mGenericOnOffClient);

                mLightnessClient = new LightnessClientModel(mBluetoothMesh) {
                    @Override
                    public void onMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
                        lightnessClientMsgHandler(modelHandle, msg);
                    }
                };
            mPrimaryElementSIGModels.put(MeshConstants.MESH_MODEL_SIG_MODEL_ID_LIGHT_LIGHTNESS_CLIENT, mLightnessClient);

                mCTLClient = new CtlClientModel(mBluetoothMesh) {
                    @Override
                    public void onMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
                        ctlClientMsgHandler(modelHandle, msg);
                    }
                };
            mPrimaryElementSIGModels.put(MeshConstants.MESH_MODEL_SIG_MODEL_ID_LIGHT_CTL_CLIENT, mCTLClient);

                mHSLClient = new HslClientModel(mBluetoothMesh) {
                    @Override
                    public void onMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
                        hslClientMsgHandler(modelHandle, msg);
                    }
                };
            mPrimaryElementSIGModels.put(MeshConstants.MESH_MODEL_SIG_MODEL_ID_LIGHT_HSL_CLIENT, mHSLClient);

            //instantiate Vendor models
            mVendorModelClient = new VendorModelDemoClient(mBluetoothMesh) {
                @Override
                public void onMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
                    vendorModelClientMsgHandler(modelHandle, msg);
                }
            };
            //Attention: vendor model or SIG model added with this way shall setModelParams
            mVendorModelClient.setModelParams(VendorModelDemoClient.mSupportedRxMsg, VendorModelDemoClient.COMPANY_ID, VendorModelDemoClient.mSupportedRxMsg.length);

            isInitialized = true;
        }
    }

    @Override
    public void onCreate() {
        log("onCreate()");
        super.onCreate();

        startForegroundProv();
        parsePropertyFile();

        mBluetoothMesh = BluetoothMesh.getDefaultMesh(this,mServiceListener);
        addSupportedAuthValue();

        createUUID();
        isInitialized = false;
    }

    private BluetoothProfile.ServiceListener mServiceListener = new BluetoothProfile.ServiceListener() {
        @Override
        public void onServiceConnected(int profile, BluetoothProfile proxy) {
            log("Connection made to bluetooth mesh");
            //mBluetoothMesh = BluetoothMesh.getDefaultMesh(ProvisionerService.this,this);
            mBluetoothMesh.registerCallback(mBluetoothMeshCallback);
            mBluetoothMesh.bearerAdvSetParams(200, 0x10, 0x10, 1, 50, 0x80, 0x30);
        }

        @Override
        public void onServiceDisconnected(int profile) {
            log("Disconnection made to bluetooth mesh");
        }
    };

    public class LocalBinder extends Binder {
        ProvisionerService getService() {
            return ProvisionerService.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        log("onBind");
        return mBinder;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        log("onUnbind");
        if(MyApplication.GattConnectStatus == MyApplication.PB_GATT_CONNECT) {
            MyApplication.GattConnectStatus = MyApplication.PB_GATT_DISCONNECT;
        }
        mBluetoothMesh.close();
        return super.onUnbind(intent);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        log("onDestroy");
    }

    public void startForegroundProv(){
        log("startForeground()");
        String CHANNEL_ID = "com.mesh.provisioner";
        String CHANNEL_NAME = "Channel One";
        NotificationChannel notificationChannel = null;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            notificationChannel = new NotificationChannel(CHANNEL_ID,
                    CHANNEL_NAME, NotificationManager.IMPORTANCE_HIGH);
            notificationChannel.enableLights(true);
            notificationChannel.setLightColor(Color.RED);
            notificationChannel.setShowBadge(true);
            notificationChannel.setLockscreenVisibility(Notification.VISIBILITY_PUBLIC);
            NotificationManager manager = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);
            manager.createNotificationChannel(notificationChannel);
        }
        //Create the notification object through the builder
        Notification noti = new Notification.Builder(this)
                .setChannelId(CHANNEL_ID)
                .build();

        // call startForeground
        startForeground(1234, noti);
    }


    private final IBinder mBinder = new LocalBinder();

    public void setEnabled(View view , boolean enabled) {
        if(null == view) {
            return;
        }
        if(view instanceof ViewGroup) {
            ViewGroup viewGroup = (ViewGroup)view;
            LinkedList<ViewGroup> queue = new LinkedList<ViewGroup>();
            queue.add(viewGroup);
            while(!queue.isEmpty()) {
                ViewGroup current = queue.removeFirst();
                current.setEnabled(enabled);
                for(int i = 0;i<current.getChildCount();i++) {
                    if(current.getChildAt(i) instanceof ViewGroup ) {
                        queue.addLast((ViewGroup)current.getChildAt(i));
                    } else {
                        current.getChildAt(i).setEnabled(enabled);
                    }
                }
            }

        }else {
            view.setEnabled(enabled);
        }
    }

    public void dataReset() {
        log("dataReset");
        mBluetoothMesh.resetData(MeshConstants.MESH_ROLE_PROVISIONER);  //clear flash data
        mBluetoothMesh.disable();   //clear runtime data
    }

    //recovery data from database
    public void dataRecovery(StorageData mStorageData, List<Node> mNodeDatas) {
        log("dataRecovery");

        if (mNodeDatas != null) {
            if (mNodeDatas.size() > 0) {
                log("dataRecovery --> all nodes OK");
                for (int i = 0; i < mNodeDatas.size(); i++) {
                    Node node = mNodeDatas.get(i);
                    if (node.getNodeBearer() == MeshConstants.MESH_BEARER_ADV) {
                        log("dataRecovery node addr " + node.getAddr()+ ", uuid = " + MeshUtils.intArrayToString(node.getUUID(), true));
                    } else if (node.getNodeBearer() == MeshConstants.MESH_BEARER_GATT) {
                        log("dataRecovery node addr " + node.getAddr()+ ", gattAddr = " + node.getGattAddr());
                    }
                    node.setActiveStatus(2);    //always set to unknown by default,, it shall be updated by heartbeat event
                    node.setCurrentHeartBeatNumber(0);
                    node.setPreHeartBeatNumber(0);
                    node.setContinueLost(0);
                    node.setMaxLost(0);
                    node.setHeartBeatTime(0);
                    node.setHeartBeatTimerNumber(0);
                    mNodes.put(node.getAddr(), node);
                    //mStorageData.addNode(node);
                }
            } else {
                log("dataRecovery --> no nodes");
            }
        }

        if (mStorageData.getAllNetKey() != null) {
            if (mStorageData.getAllNetKey().size() > 0) {
                log("dataRecovery --> all netkey OK");
                mCurrMaxNetKeyIndex = mStorageData.getAllNetKey().size() - 1;
                for (int i = 0; i <= mCurrMaxNetKeyIndex; i++) {
                    NetKey key = mStorageData.getAllNetKey().get(i);
                    log("dataRecovery netkey index " + i + ", value = " + MeshUtils.intArrayToString(key.getValue(), false));
                    if (key.getState() != MeshConstants.MESH_KEY_REFRESH_STATE_NONE) {
                        log("reset netkey refresh state to STATE_NONE");
                        key.setState(MeshConstants.MESH_KEY_REFRESH_STATE_NONE);
                    }
                    mAllNetKeyMap.put(key.getIndex(), key);
                    this.mStorageData.addNetKey(key);
                }
            } else {
                log("dataRecovery --> no netkey");
            }
        }

        if (mStorageData.getAllAppKey() != null) {
            if (mStorageData.getAllAppKey().size() > 0) {
                log("dataRecovery --> all appkey OK");
                mCurrMaxAppKeyIndex = mStorageData.getAllAppKey().size() - 1;
                for (int i = 0; i <= mCurrMaxAppKeyIndex; i++) {
                    AppKey key = mStorageData.getAllAppKey().get(i);
                    log("dataRecovery appkey index " + i + ", value = " + MeshUtils.intArrayToString(key.getValue(), false));
                    mAllAppKeyMap.put(key.getIndex(), key);
                    this.mStorageData.addAppKey(key);
                }
            } else {
                log("dataRecovery --> no appkey");
            }
        }

        if (mStorageData.getAllGroupAddr() != null) {
            if (mStorageData.getAllGroupAddr().size() > 0) {
                log("dataRecovery --> all group address OK");
                for (int i = 0; i < mStorageData.getAllGroupAddr().size(); i++) {
                    int addr = mStorageData.getAllGroupAddr().get(i);
                    if (!mGroupAddrList.contains(addr)) {
                        log("dataRecovery groupAddr " + MeshUtils.decimalToHexString("%04X", addr));
                        mGroupAddrList.add(addr);
                    }
                }
                this.mStorageData.setAllGroupAddrList(mGroupAddrList);
            } else {
                log("dataRecovery --> no group address");
            }
        }
    }

    private void createMeshDevice() {
        int modelHandle = 0;
        int elementIndex = 0;
        int[] compDataHeader = {
            0x8A, 0x01, // cid
            0x1A, 0x00, // pid
            0x01, 0x00, // vid
            0xF4, 0x01, // crpl
            MeshConstants.MESH_FEATURE_PROXY | MeshConstants.MESH_FEATURE_FRIEND, 0x00, // features
        };
        log("createMeshDevice");

        if (isEnable()) {
            log("Mesh already enabled, create device is prohibited");
            return;
        }

        mBluetoothMesh.setCompositionDataHeader(compDataHeader);

        //add primary element
        elementIndex = mBluetoothMesh.addElement(MeshConstants.MESH_MODEL_ELEMENT_LOCATION_FRONT);
        if (elementIndex < 0) {
            log("createMeshDevice add element failed");
            return;
        }
        mElementCnt++;
        mPrimaryElement = new Element(mMyAddr, elementIndex);

        //Add SIG models
        Set<Integer> set = mPrimaryElementSIGModels.keySet();
        for (Iterator it = set.iterator(); it.hasNext();) {
            int modelId = ((Integer)it.next()).intValue();
            mPrimaryElementSIGModels.get(modelId).setElementIndex(mPrimaryElement.getIndex());
            mPrimaryElementSIGModels.get(modelId).setModelID(modelId);
            log("createMeshDevice add model : " + MeshUtils.decimalToHexString("%04X", modelId));
            modelHandle = mBluetoothMesh.addModel(mPrimaryElementSIGModels.get(modelId));
            if (modelHandle < 0) {
                log("createMeshDevice add model : " + MeshUtils.decimalToHexString("%04X", modelId) + " failed");
                if ((modelId == MeshConstants.MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_CLIENT)
                    || (modelId == MeshConstants.MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_SERVER)) {
                    //these two models shall present
                    return;
                }
            }
            mPrimaryElementSIGModels.get(modelId).setModelHandle(modelHandle);
            mPrimaryElement.addModel(new Model(modelId, mPrimaryElement.getAddress(), modelHandle));
            if ((modelId != MeshConstants.MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_CLIENT)
                && (modelId != MeshConstants.MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_SERVER)
                && (modelId != MeshConstants.MESH_MODEL_SIG_MODEL_ID_HEALTH_SERVER)) {
                    addSupportedTXMsg(true, modelId, 0/*useless*/);
            }

        }
        //Add Vendor models
        mVendorModelClient.setElementIndex(mPrimaryElement.getIndex());
        mVendorModelClient.setModelID(VendorModelDemoClient.MODEL_ID);
        modelHandle = mBluetoothMesh.addModel(mVendorModelClient);
        if (modelHandle < 0) {
            log("createMeshDevice add Vendor model 0x" + Long.toHexString(VendorModelDemoClient.MODEL_ID) + " failed");
        } else {
            log("createMeshDevice mVendorModelClient handle = " + modelHandle);
            mVendorModelClient.setModelHandle(modelHandle);
            mPrimaryElement.addModel(new Model(VendorModelDemoClient.MODEL_ID, mPrimaryElement.getAddress(), modelHandle));
            addSupportedTXMsg(false, 0/*useless*/, VendorModelDemoClient.MODEL_ID);
        }

        //set element addr(element addr shall be set at the end of creating device)
        mBluetoothMesh.setElementAddr(mPrimaryElement.getAddress());
    }

    public boolean isMeshAvailable() {
        log("isMeshAvailable , mBluetoothMesh.getMeshState() = " + mBluetoothMesh.getMeshState() + " , mBluetoothMesh.getMeshRole() = " + mBluetoothMesh.getMeshRole());
        if (mBluetoothMesh.getMeshState() && (mBluetoothMesh.getMeshRole() != MeshConstants.MESH_ROLE_PROVISIONER)) {
            log("isMeshAvailable, Fatal error, provisionee is running, return");
            return false;
        } else {
            return true;
        }
    }

    public boolean setBluetoothMeshEnabled(boolean enabled) {
        log("setBluetoothMeshEnabled , enabled = " + enabled);
        if(enabled) {
            log("enable isEnable = " + isEnable);
            if (mBluetoothMesh.getMeshState() && (mBluetoothMesh.getMeshRole() != MeshConstants.MESH_ROLE_PROVISIONER)) {
                log("Fatal error, provisionee is running, return");
                return false;
            }

            setMeshMode(MeshConstants.MESH_MODE_STANDBY);

            initialize();

            //parsePropertyFile();
            createMeshDevice();
            //updateStorageData(MyApplication.STORAGE_DATA_ALL);

            if (mBluetoothMesh.getMeshState() && (mBluetoothMesh.getMeshRole() == MeshConstants.MESH_ROLE_PROVISIONER)) {
                log("Mesh for provisioner has been enabled");
                isEnable = true;
            }

            MeshInitParams initParams = new MeshInitParams();
            initParams.setRole(MeshConstants.MESH_ROLE_PROVISIONER);
            initParams.setDeviceUuid(this.mMyUUID);
            initParams.setOobInfo(MeshConstants.MESH_PROV_OOB_INFO_FIELD_NUMBER | MeshConstants.MESH_PROV_OOB_INFO_FIELD_STRING);
            initParams.setUri(null);
            initParams.setDefaultTtl(4);
            initParams.setFeatureMask(MeshConstants.MESH_FEATURE_MASK_HEARTBEAT | MeshConstants.MESH_FEATURE_MASK_OTA);
            initParams.setFriendInitParams(initParams.new FriendInitParams(mLPNNumber, mLPNMessageQueueSize, mLPNSubscriptionListSize));
            initParams.setCustomizeParams(initParams.new CustomizeParams(mRemoteNodeNumber, mSaveFlash));

            if (isEnable) {// if mesh is already enabled, there is no onMeshEnabled event, we shall notify UI here
                if(mBluetoothMeshListener != null) {
                    mBluetoothMeshListener.onMeshEnabled();
                }
                for(Map.Entry<Integer,Node> entry:mNodes.entrySet()) {
                    Node node = entry.getValue();
                    if(!node.isConfigSuccess()) { // if node config fail,not set timer for node
                        break;
                    }
                    int i = createTimer();
                    if(-1 != i) {
                        node.setHeartBeatTimerID(i);
                        getTimerById(i).schedule(new ExTimerTask<Node>(2, node), 40000,40000);
                    }
                }
            }
            mBluetoothMesh.enable(initParams);
        } else {
            disable();
            log("disable");
            if(MyApplication.GattConnectStatus == MyApplication.PB_GATT_CONNECT) {
                mBluetoothMesh.gattDisconnect();// close mesh , should disconnect gattconnect
            }
            MyApplication.GattConnectStatus = MyApplication.PB_GATT_DISCONNECT;
            setMeshMode(MeshConstants.MESH_MODE_OFF);
            mBluetoothMesh.disable();
            isEnable = false;
        }
        return true;
    }

    public int inviteProvisioning(int bearer, int[] uuid, String addr, int addrType, String devName, int netkeyIndex, int appkeyIndex){
        log("inviteProvisioning , bearer = " + ((bearer == MeshConstants.MESH_BEARER_GATT)? "PB-GATT , address = " + addr : "PB-ADV , uuid = " + MeshUtils.intArrayToString(uuid, true))  +
                                " , netKeyIndex = " + netkeyIndex + " , appKeyIndex = " + appkeyIndex + " , current meshmode = " + getMeshMode());
        if (null != mAllNetKeyMap.get(netkeyIndex)) {
            log("netkey: " + MeshUtils.intArrayToString(mAllNetKeyMap.get(netkeyIndex).getValue(), false));
        } else {
            log("inviteProvisioning the netkey is not found");
            return -1;
        }
        if (null != mAllAppKeyMap.get(appkeyIndex)) {
            log("appkey: " + MeshUtils.intArrayToString(mAllAppKeyMap.get(appkeyIndex).getValue(), false));
        } else {
            log("inviteProvisioning the appkey is not found");
            return -1;
        }
        //provisioning start time
        MyApplication.isProvisioning = true;
        MyApplication.provisioningTime = System.currentTimeMillis();

        mBluetoothMesh.setHeartbeatPeriod(mHearbeatPeriodNum, mHeartbeatTimeout);

        isProvOrConfiging = true;

        if (bearer == MeshConstants.MESH_BEARER_GATT) {
            //setMeshMode(MeshConstants.MESH_MODE_STANDBY); // not need set mesh standby , default value is standby
            log("GATT device addr = " + addr);
            currentProvNode = new Node(MeshConstants.MESH_BLE_ADDR_TYPE_PUBLIC, addr);
            currentProvNode.setNodeBearer(MeshConstants.MESH_BEARER_GATT);
            currentProvNode.setGattDevName(devName);
            currentProvNode.setGattAddrType(addrType);
            currentProvNode.setCurrNetkeyIndex(netkeyIndex);
            currentProvNode.addNetKey(netkeyIndex);
            currentProvNode.addAppKey(appkeyIndex);    //appkey 0 is the default appkey for each node
            currentProvNode.setConfigState(MESH_NODE_CONFIG_STATE_PROVISIONING);
            int id = createTimer();
            if (-1 != id) {
                gattProvisioningService = true;
                currentProvNode.setProvisioningServiceTimerID(id);
                getTimerById(id).schedule(new ExTimerTask<Node>(3, currentProvNode), 60000); // gatt provisioning service time : 60s
            }
            id = createTimer();
            if (-1 != id) {
                currentProvNode.setProvisionTimerID(id);
                getTimerById(id).schedule(new ExTimerTask<Node>(4, currentProvNode), 70000); // gatt provision time : 70s
            }
            //start gatt connect
            return mBluetoothMesh.gattConnect(addr, currentProvNode.getGattAddrType(), MeshConstants.MESH_GATT_SERVICE_PROVISION);
        } else {
            if (getMeshMode() != MeshConstants.MESH_MODE_ON) {
                setMeshMode(MeshConstants.MESH_MODE_ON);
            }
            //log("ADV device uuid = " + MeshUtils.intArrayToString(uuid));
            currentProvNode = new Node(uuid);
            currentProvNode.setNodeBearer(MeshConstants.MESH_BEARER_ADV);
            currentProvNode.setCurrNetkeyIndex(netkeyIndex);
            currentProvNode.addNetKey(netkeyIndex);
            currentProvNode.addAppKey(appkeyIndex);    //appkey 0 is the default appkey for each node
            currentProvNode.setConfigState(MESH_NODE_CONFIG_STATE_PROVISIONING);
            int id = createTimer();
            if (-1 != id) {
                currentProvNode.setProvisionTimerID(id);
                getTimerById(id).schedule(new ExTimerTask<Node>(5, currentProvNode), 60000); // gatt provision time : 60s
            }
            return mBluetoothMesh.inviteProvisioning(uuid, 5);
        }

    }


    /**
     * start scan unprovisioned device
     */
    public void startUnProvsionScan() {
        log("startUnProvsionScan");
        mBluetoothMesh.unProvisionScan(true , 15000);    //scan 5s by default
    }

    /**
     * stop scan unprovisioned device
     */
    public void stopUnProvsionScan() {
        log("stopUnProvsionScan");
        mBluetoothMesh.unProvisionScan(false , 0);
    }

    /**
     * set mesh mode on off
     */
    public boolean setMeshMode(int mode) {
        log("setMeshMode " + mode);
        int ret = mBluetoothMesh.setMeshMode(mode);
        meshMode = mode;
        log("setMeshMode ret=" + ret);
        return ret == 0 ? true: false;
    }

    public void setConfigMeshMode(Node node) {
        log("setConfigMeshMode , node addr = " + MeshUtils.decimalToHexString("%04X", node.getAddr()) + " , node bearer = " +
            ((node.getNodeBearer() == MeshConstants.MESH_BEARER_ADV) ? "PB-ADV" : "PB-GATT") + " , current mesh mode = " + getMeshMode());
        if (node.getNodeBearer() == MeshConstants.MESH_BEARER_ADV) { // PB-ADV
            if (getMeshMode() != MeshConstants.MESH_MODE_ON) {
                setMeshMode(MeshConstants.MESH_MODE_ON);
            }
        } else { // PB-GATT , not need to set mesh standby mode , if is mesh mode on , is indicate previous node maybe configMsgxxxx
        }
    }

    public int getMeshMode() {
        return meshMode;
    }

    /**
     * set dump
     */
    public void setDump() {
        log("setDump ");
        dumpInfo();
        mBluetoothMesh.dump(MeshConstants.MESH_DUMP_TYPE_ALL);
    }

    public void GattConnect(Node node) {
        log("GattConnect");
        currentProvNode = node;
        try {
            gattProxyService = true;
            if (getTimerById(currentProvNode.getProxyServiceTimerID()) == null) {
                int id = createTimer();
                if (-1 != id) {
                    currentProvNode.setProxyServiceTimerID(id);
                }
            }
            getTimerById(currentProvNode.getProxyServiceTimerID()).schedule(new TimerTask() {
                public void run() {
                    log("proxy service connect timeout when click connect button");
                    gattProxyService = false;
                    if (mBluetoothMeshListener != null) {
                        mBluetoothMeshListener.GattConnectTimeout();
                    }
                    try {
                        if (getTimerById(currentProvNode.getProxyServiceTimerID()) != null) {
                            getTimerById(currentProvNode.getProxyServiceTimerID()).cancel();
                            mTimerMap.remove(currentProvNode.getProxyServiceTimerID());
                            currentProvNode.setProxyServiceTimerID(-1);
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

            }, 60000);

        } catch (Exception e) {
            e.printStackTrace();
        }
        mBluetoothMesh.gattConnect(node.getGattAddr(), node.getGattAddrType(), MeshConstants.MESH_GATT_SERVICE_PROXY);

    }

    public void GattDisconnect(Node node) {
        log("GattDisconnect");
        currentProvNode = node;
        mBluetoothMesh.gattDisconnect();
    }

    private BluetoothMeshCallback mBluetoothMeshCallback = new BluetoothMeshCallback() {
        @Override
        public void onMeshEnabled() {
            super.onMeshEnabled();
            log("onMeshEnabled");
            log("mBluetoothMeshListener = " + ProvisionerService.this.mBluetoothMeshListener);
            isEnable = true;

            //add all netkey and appkey resource
            Set<Integer> set = mAllNetKeyMap.keySet();
            for (Iterator it = set.iterator(); it.hasNext();) {
                NetKey netKey = mAllNetKeyMap.get(it.next());
                if (0 == mBluetoothMesh.setNetkey(MeshConstants.MESH_KEY_OP_ADD, netKey.getValue(), netKey.getIndex())) {
                    log("Add netkey " + netKey.getIndex() + ", value: " + MeshUtils.intArrayToString(netKey.getValue(), false) + ", success");
                } else {
                    log("Add netkey " + netKey.getIndex() + ", value: " + MeshUtils.intArrayToString(netKey.getValue(), false) + ", fail");
                }
            }
            set = mAllAppKeyMap.keySet();
            for (Iterator it = set.iterator(); it.hasNext();) {
                AppKey appKey = mAllAppKeyMap.get(it.next());
                if (0 == mBluetoothMesh.setAppkey(MeshConstants.MESH_KEY_OP_ADD, appKey.getValue(), appKey.getBoundNetKeyIndex(), appKey.getIndex())) {
                    log("Add appkey " + appKey.getIndex() + ", value: " + MeshUtils.intArrayToString(appKey.getValue(), false) + ", success");
                } else {
                    log("Add appkey " + appKey.getIndex() + ", value: " + MeshUtils.intArrayToString(appKey.getValue(), false) + ", fail");
                }
            }

            //bind all appkey for all local models
            set = mPrimaryElementSIGModels.keySet();
            for (Iterator it = set.iterator(); it.hasNext();) {
                int modelId = ((Integer)it.next()).intValue();
                if ((modelId != MeshConstants.MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_CLIENT)
                    && (modelId != MeshConstants.MESH_MODEL_SIG_MODEL_ID_CONFIGURATION_SERVER)) {

                    Set<Integer> appKeySet = mAllAppKeyMap.keySet();
                    for (Iterator appKeyIt = appKeySet.iterator(); appKeyIt.hasNext();) {
                        int appKeyIndex = (Integer)appKeyIt.next();
                        log("Bind model " + modelId + " to AppKey " + appKeyIndex);
                        if (0 != mBluetoothMesh.modelAppBind(mPrimaryElementSIGModels.get(modelId).getModelHandle(), appKeyIndex)) {
                            log("Bind SIG model " + modelId + " to AppKey " + appKeyIndex + "failed");
                        }
                        //bind Vendor models
                        if (0 != mBluetoothMesh.modelAppBind(mVendorModelClient.getModelHandle(), appKeyIndex)) {
                            log("Bind Vendor model " + modelId + " to AppKey " + appKeyIndex + "failed");
                        }
                    }
                }
            }
            log("mBluetoothMeshListener = " + mBluetoothMeshListener);
            if(mBluetoothMeshListener != null) {
                mBluetoothMeshListener.onMeshEnabled();
            }
            for(Map.Entry<Integer,Node> entry:mNodes.entrySet()) {
                Node node = entry.getValue();
                if(!node.isConfigSuccess()) { // if node config fail,not set timer for node
                    break;
                }
                int i = createTimer();
                if(-1 != i && node.isConfigSuccess()) {
                    node.setHeartBeatTimerID(i);
                    getTimerById(i).schedule(new ExTimerTask<Node>(2, node), 40000,40000);
                }
            }
        }

        @Override
        public void onProvScanComplete() {
            super.onProvScanComplete();
            log("onProvScanComplete");
            if(mBluetoothMeshListener != null) {
                mBluetoothMeshListener.onProvScanComplete();
            }

        }

        @Override
        public void onScanUnProvDevice(int[] uuid, int oobInfom, int[] uriHash) {
            super.onScanUnProvDevice(uuid, oobInfom, uriHash);
            //save ud for UI display
            log("onScanUnProvDevice, uuid = " + MeshUtils.intArrayToString(uuid, true));
            deleteLocalNodeByBear(MeshConstants.MESH_BEARER_ADV, uuid, null);
            if(mBluetoothMeshListener != null) {
                mBluetoothMeshListener.onScanUnProvDevice(uuid, oobInfom, uriHash);
            }

        }

        @Override
        public void onProvCapabilities(int numberOfElements, int algorithms, int publicKeyType, int staticOOBType, int outputOobSize, int outputOobAction, int inputOobSize, int inputOobAction) {
            super.onProvCapabilities(numberOfElements, algorithms, publicKeyType, staticOOBType, outputOobSize, outputOobAction, inputOobSize, inputOobAction);

            log("onProvCapabilities");
            log("NumOfElements = " + numberOfElements);
            log("Algorithms = " + algorithms);
            log("PublicKeyType = " + publicKeyType);
            log("StaticOobType = " + staticOOBType);
            log("OutputOobSize = " + outputOobSize);
            log("OutputOobAction = " + outputOobAction);
            log("InputOobSize = " + inputOobSize);
            log("InputOobAction = " + inputOobAction);

            int authMethod = MeshConstants.MESH_PROV_START_AUTHEN_METHOD_NO_OOB;
            //if (staticOOBType == MeshConstants.MESH_PROV_CAPABILITY_OOB_STATIC_TYPE_SUPPORTED) {
                //authMethod = MeshConstants.MESH_PROV_START_AUTHEN_METHOD_STATIC_OOB;
            //}
            //else {
                //authMethod = MeshConstants.MESH_PROV_START_AUTHEN_METHOD_NO_OOB;
            //}

            currentProvNode.setElementNumber(numberOfElements);

            //set the addr of the current prov node
            int addr = SystemProperties.getInt("persist.bluetooth.last.element.addr", mLastElementAddr) + 1;
            /*
            int addr = 0;
            if (mNodes.size() == 0) {
                addr = mMyAddr + mElementCnt;
            } else {    //find the max assigned address
                Set<Integer> set = mNodes.keySet();
                for (Iterator it = set.iterator(); it.hasNext();) {
                    int temp = ((Integer)it.next()).intValue();
                    log("temp = " + temp);
                    if (temp > addr) {
                        addr = temp;
                    }
                }
                if ((addr == 0) || (addr <= mMyAddr)) {
                    log("onProvCapabilities assign new addr failed");
                    return;
                }
                if (mNodes.get(addr).getElements().size() == 0) { //provisioning success but parse composition data fail, the element list is empty
                    addr = addr + mNodes.get(addr).getElementNumber();
                } else {
                    addr = mNodes.get(addr).getElements().size() + addr;
                }
            }
            */
            if (addr > 32700) { // 32767
                addr = 101;
            }
            SystemProperties.set("persist.bluetooth.last.element.addr", addr+"");
            log("startProvisioning with addr: 0x" + MeshUtils.decimalToHexString("%04X", addr));
            log("startProvisioning netkey = " + MeshUtils.intArrayToString(mAllNetKeyMap.get(currentProvNode.getCurrNetkeyIndex()).getValue(), true));
            mBluetoothMesh.startProvisioning(MeshConstants.MESH_PROV_START_ALGORITHM_FIPS_P256_ELLIPTIC_CURVE,
                                            MeshConstants.MESH_PROV_START_PUBLIC_KEY_NO_OOB,
                                            authMethod,
                                            0, 0,
                                            mAllNetKeyMap.get(currentProvNode.getCurrNetkeyIndex()).getValue(),
                                                currentProvNode.getCurrNetkeyIndex(),
                                            0,
                                            addr,
                                            0, 0);
        }

        @Override
        public void onRequestOobPublicKey() {
            super.onRequestOobPublicKey();
            log("onRequestOobPublicKey");
        }

        @Override
        public void onRequestOobAuthValue(int method, int action, int size) {
            log("onRequestOobAuthValue, method = " + method);
            super.onRequestOobAuthValue(method, action, size);
            if (method == MeshConstants.MESH_PROV_START_AUTHEN_METHOD_STATIC_OOB) {
                int[] authValue = getAuthValue(currentProvNode);
                log("AuthValue = " + MeshUtils.intArrayToString(authValue, false));
                mBluetoothMesh.setProvisionFactor(MeshConstants.MESH_PROV_FACTOR_AUTHEN_VALUE, authValue);
            }
        }

        @Override
        public void onProvShowOobPublicKey(int[] publicKey) {
            super.onProvShowOobPublicKey(publicKey);
            log("onProvShowOobPublicKey");
        }

        @Override
        public void onProvShowOobAuthValue(int[] authValue) {
            super.onProvShowOobAuthValue(authValue);
            log("onProvShowOobAuthValue");
        }

        @Override
        public void onProvDone(int address, int[] deviceKey, boolean success, boolean gatt_bearer) {
            super.onProvDone(address, deviceKey, success, gatt_bearer);
            log("onProvDone, addr = " + MeshUtils.decimalToHexString("%04X",address)+ ", success = " + success + ", gatt_bearer = " + gatt_bearer);
            //provisioning end time
            MyApplication.provisioningTime = System.currentTimeMillis() - MyApplication.provisioningTime;
            MyApplication.configTime = System.currentTimeMillis();
            double time = MeshUtils.getDoubleDecimal(MyApplication.provisioningTime*1.0/1000);
            if (getTimerById(currentProvNode.getProvisionTimerID()) != null) {
                getTimerById(currentProvNode.getProvisionTimerID()).cancel(); // cancel provision timer
                mTimerMap.remove(currentProvNode.getProvisionTimerID()); // remove provision timer
                currentProvNode.setProvisionTimerID(-1);
            }
            log("provision time = " + time + "s");
            if (true == success) {
                currentProvNode.setConfigState(MESH_NODE_CONFIG_STATE_PROVISIONED);
                currentProvNode.setAddr(address);
                currentProvNode.setDevKey(deviceKey);
                currentProvNode.setProvisioningTime(time);
                mNodes.put(currentProvNode.getAddr(), currentProvNode);
                if(mBluetoothMeshListener != null) {
                    mBluetoothMeshListener.onProvDone(address,deviceKey,success,gatt_bearer,time);
                }
                log("onProvDone, mNodes = " + mNodes.size());
                int id = createTimer();
                if (-1 != id) {
                    currentProvNode.setConfigTimerID(id);
                }
                id = createTimer();
                if (-1 != id) {
                    currentProvNode.setHeartBeatTimerID(id);
                }
                //mStorageData.addNode(currentProvNode);
                updateNodeData(new NodeData(currentProvNode), MyApplication.NODE_DATA_ADD);
                mAllNetKeyMap.get(currentProvNode.getCurrNetkeyIndex()).getNodes().put(currentProvNode.getAddr(), currentProvNode);
                updateStorageData(MyApplication.STORAGE_DATA_ALL);
                log("start to config the node " + address);
                if (false == gatt_bearer) { // PB-ADV
                    try {
                        getTimerById(currentProvNode.getConfigTimerID()).schedule(new TimerTask() { // adv config timer
                            public void run() {
                                log("Config node " + currentProvNode.getAddr() + " timeout, maybe FAILED!!!");
                                MyApplication.configTime = System.currentTimeMillis() - MyApplication.configTime;
                                //double configTime = MeshUtils.getDoubleDecimal(MyApplication.configTime*1.0/1000);
                                currentProvNode.setConfigSuccess(false);
                                currentProvNode.setConfigTime(80.0);
                                currentProvNode.setConfigState(MESH_NODE_CONFIG_STATE_IDLE);
                                notifyProvState(currentProvNode, false);
                                //mBluetoothMesh.setMeshMode(MeshConstants.MESH_MODE_STANDBY);
                                //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                                updateNodeData(new NodeData(currentProvNode), MyApplication.NODE_DATA_UPDATE);
                                try {
                                    getTimerById(currentProvNode.getConfigTimerID()).cancel(); //cancel gatt config timer after 80s
                                    mTimerMap.remove(currentProvNode.getConfigTimerID());// remove config timer
                                    currentProvNode.setConfigTimerID(-1);
                                } catch (Exception e) {
                                    e.printStackTrace();
                                }
                                //remove config msg retry when config timeout
                                int[] opcodes = {MeshConstants.MESH_MSG_CONFIG_COMPOSITION_DATA_GET, MeshConstants.MESH_MSG_CONFIG_APPKEY_ADD, MeshConstants.MESH_MSG_CONFIG_MODEL_APP_BIND};
                                removeResendConfigMsg(opcodes, currentProvNode.getAddr());
                            }

                        }, 80000);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    currentProvNode.setConfigState(MESH_NODE_CONFIG_STATE_GET_COMPOSITION_DATA);
                    notifyProvState(currentProvNode, true);
                    configMsgCompositionDataGet(currentProvNode, 0);
                } else { // PB-GATT
                    log("gattDisconnect due to prov done for PB-GATT");
                    try {
                        gattProxyService = true;
                        id = createTimer();
                        if (-1 != id) {
                            currentProvNode.setProxyServiceTimerID(id);
                            getTimerById(id).schedule(new ExTimerTask<Node>(6, currentProvNode), 60000); // gatt proxy service timer : 60s
                        }
                        getTimerById(currentProvNode.getConfigTimerID()).schedule(new TimerTask() { //gatt config timer
                            public void run() {
                                log("Config node " + currentProvNode.getAddr() + " timeout, maybe FAILED!!!");
                                gattProxyService = false;
                                MyApplication.configTime = System.currentTimeMillis() - MyApplication.configTime;
                                //double configTime = MeshUtils.getDoubleDecimal(MyApplication.configTime*1.0/1000);
                                currentProvNode.setConfigSuccess(false);
                                currentProvNode.setConfigTime(80.0);
                                currentProvNode.setConfigState(MESH_NODE_CONFIG_STATE_IDLE);
                                mBluetoothMesh.gattDisconnect();
                                notifyProvState(currentProvNode, false);
                                //mBluetoothMesh.setMeshMode(MeshConstants.MESH_MODE_STANDBY);
                                //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                                updateNodeData(new NodeData(currentProvNode), MyApplication.NODE_DATA_UPDATE);
                                try {
                                    getTimerById(currentProvNode.getConfigTimerID()).cancel(); //cancel gatt config timer after 70s
                                    mTimerMap.remove(currentProvNode.getConfigTimerID()); // remove config timer
                                    currentProvNode.setConfigTimerID(-1);
                                } catch (Exception e) {
                                    e.printStackTrace();
                                }
                            }

                        }, 80000);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    mBluetoothMesh.gattDisconnect();
                }
            } else {
                if(mBluetoothMeshListener != null) {
                    mBluetoothMeshListener.onProvDone(address,deviceKey,false,gatt_bearer,time);
                }
                if ((currentProvNode.getNodeBearer() == MeshConstants.MESH_BEARER_GATT) && (currentProvNode.getConfigState() == MESH_NODE_CONFIG_STATE_PROVISIONING)) {
                    log("gattDisconnect due to prov fail for PB-GATT");
                    mBluetoothMesh.gattDisconnect();
                }
                currentProvNode.setConfigState(MESH_NODE_CONFIG_STATE_IDLE);
                //mBluetoothMesh.setMeshMode(MeshConstants.MESH_MODE_STANDBY);
            }

        }

        @Override
        public void onScanResult(ScanResult scanResult) {
            super.onScanResult(scanResult);
            log("onScanResult");
        }

        @Override
        public void onKeyRefresh(int netKeyIndex, int phase) {
            super.onKeyRefresh(netKeyIndex, phase);
            log("onKeyRefresh netkey_index = " + netKeyIndex + ", phase = " + phase + " , current mesh mode = " + getMeshMode());

            NetKey netKey = mAllNetKeyMap.get(netKeyIndex);
            if (null == netKey) {
                log("onKeyRefresh, netkey is not found");
                return;
            }
            if (MeshConstants.MESH_KEY_REFRESH_STATE_1 == phase) {
                //This event indicates that SDK starts key refresh procedure
                int id = createTimer();
                if (-1 != id) {
                    netKey.setKeyRefreshTimerID(id);
                    try {
                        if (netKey.getKeyRefreshNodes().size() == 0) {
                            getTimerById(netKey.getKeyRefreshTimerID()).schedule(new ExTimerTask<NetKey>(1, netKey), 5000);
                            log("step 3 timeout = " + (5000 + netKey.getKeyRefreshNodes().size()*2000*5));
                        } else {
                            getTimerById(netKey.getKeyRefreshTimerID()).schedule(
                                new ExTimerTask<NetKey>(1, netKey), (5000 + netKey.getKeyRefreshNodes().size()*2000*5));
                            log("step 2 timeout = " + (5000 + netKey.getKeyRefreshNodes().size()*2000*5));
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

                netKey.setState(phase);
                netKey.setNetKeyMeta(netKey.getIndex(), netKey.getTempValue());
                //SDK save the key to flash at this stage, APP shall also save it
                updateStorageData(MyApplication.STORAGE_DATA_NETKEY);
                //remove local blacklisted node with this netkey
                deleteBlacklistNodes(netKey.getIndex());
                if (netKey.getKeyRefreshNodes().size() > 0) {
                    // Distribute new key to the whitelist nodes(key refresh nodes)
                    log("key refresh procedure step 2 --> distribute new key to all nodes");
                    Set<Integer> set = netKey.getKeyRefreshNodes().keySet();
                    log("set.size() = " + set.size());
                    for (Iterator it = set.iterator(); it.hasNext();) {
                        configMsgNetKeyUpdate(getNodeByAddr((Integer)it.next()), netKeyIndex, set.size());
                    }
                } else {
                    //switch to use new key directly
                    log("key refresh procedure step 3 --> start to use new key value");
                    mBluetoothMesh.setNetkey(MeshConstants.MESH_KEY_OP_USE_NEW_NETKEY, netKey.getTempValue(), netKeyIndex);
                }
            } else if (MeshConstants.MESH_KEY_REFRESH_STATE_2 == phase) {
                netKey.setState(phase);
                //netKey.setNetKeyMeta(netKey.getIndex(), netKey.getTempValue()); //sdk enable the netkey in runtime in this timing
                if (netKey.getKeyRefreshNodes().size() > 0) {
                    netKey.getKeyRefreshAckNodes().clear();
                    log("key refresh procedure step 4 --> send Config Key Refresh Phase 2");

                    log("netkey keyrefreshTimerID = " + getTimerById(netKey.getKeyRefreshTimerID()));
                    if (getTimerById(netKey.getKeyRefreshTimerID()) != null) {
                        getTimerById(netKey.getKeyRefreshTimerID()).cancel();
                        mTimerMap.remove(netKey.getKeyRefreshTimerID());
                    }

                    int id = createTimer();
                    if (-1 != id) {
                        netKey.setKeyRefreshTimerID(id);
                        try {
                            if (netKey.getKeyRefreshNodes().size() == 0) {
                                getTimerById(netKey.getKeyRefreshTimerID()).schedule(new ExTimerTask<NetKey>(1, netKey), 5000);
                            } else {
                                getTimerById(netKey.getKeyRefreshTimerID()).schedule(
                                    new ExTimerTask<NetKey>(1, netKey), (5000 + netKey.getKeyRefreshNodes().size()*2000*5));
                                log("step 4 timeout = " + (5000 + netKey.getKeyRefreshNodes().size()*2000*5));
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }

                    // Send Config Key Refresh Phase Set (Transition = 0x02) to all the kr_node to swith to use new key
                    Set<Integer> set = netKey.getKeyRefreshNodes().keySet();
                    log("set.size() = " + set.size());
                    for (Iterator it = set.iterator(); it.hasNext();) {
                        configMsgKeyRefreshPhaseSet(getNodeByAddr((Integer)it.next()), netKeyIndex, MeshConstants.MESH_KEY_REFRESH_STATE_2, set.size());
                    }
                } else {
                    log("key refresh procedure step 5 --> revoke old key");
                    //Trigger to switch to Key Refresh Phase3,  onKeyRefresh() event with MESH_KEY_REFRESH_STATE_NONE will be received.
                    mBluetoothMesh.setNetkey(MeshConstants.MESH_KEY_OP_REVOKE_OLD_NETKEY, netKey.getTempValue(), netKeyIndex);
                }
            } else if (MeshConstants.MESH_KEY_REFRESH_STATE_NONE == phase) {
                if (netKey.getState() == MeshConstants.MESH_KEY_REFRESH_STATE_2) {
                    //SDK save the key to flash at this stage, APP shall also save it
                    updateStorageData(MyApplication.STORAGE_DATA_NETKEY);

                    if (netKey.getKeyRefreshNodes().size() > 0) {
                        netKey.setState(MeshConstants.MESH_KEY_REFRESH_STATE_3);
                        netKey.getKeyRefreshAckNodes().clear();
                        log("key refresh procedure step 6 --> send Config Key Refresh Phase 3");

                        log("netkey keyrefreshTimerID = " + getTimerById(netKey.getKeyRefreshTimerID()));
                        if (getTimerById(netKey.getKeyRefreshTimerID()) != null) {
                            getTimerById(netKey.getKeyRefreshTimerID()).cancel();
                            mTimerMap.remove(netKey.getKeyRefreshTimerID());
                        }

                        int id = createTimer();
                        if (-1 != id) {
                            netKey.setKeyRefreshTimerID(id);
                            try {
                                if (netKey.getKeyRefreshNodes().size() == 0) {
                                    getTimerById(netKey.getKeyRefreshTimerID()).schedule(new ExTimerTask<NetKey>(1, netKey), 5000);
                                } else {
                                    getTimerById(netKey.getKeyRefreshTimerID()).schedule(
                                        new ExTimerTask<NetKey>(1, netKey), (5000 + netKey.getKeyRefreshNodes().size()*2000*5));
                                    log("step 6 timeout = " + (5000 + netKey.getKeyRefreshNodes().size()*2000*5));
                                }
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                        }

                        // Send Config Key Refresh Phase Set (Transition = 0x03) to all the kr_node
                        Set<Integer> set = netKey.getKeyRefreshNodes().keySet();
                        log("set.size() = " + set.size());
                        for (Iterator it = set.iterator(); it.hasNext();) {
                            configMsgKeyRefreshPhaseSet(getNodeByAddr((Integer)it.next()), netKeyIndex, MeshConstants.MESH_KEY_REFRESH_STATE_3, set.size());
                        }
                    } else {
                        log("key refresh procedure step 7 --> success , onKeyRefresh");
                        //not remove local blacklisted node with this netkey
                        //deleteBlacklistNodes(netKey.getIndex());
                        for(Map.Entry<Integer,Node> entryNodes:netKey.getKeyRefreshNodes().entrySet()) {
                            boolean flag = false;
                            Node node = entryNodes.getValue();
                            for(Map.Entry<Integer,Node> entryAckNodes:netKey.getKeyRefreshAckNodes().entrySet()) {
                                Node ackNode = entryAckNodes.getValue();
                                if (node.getAddr() == ackNode.getAddr()) {
                                    flag = true;
                                    break;
                                }
                            }
                            if (!flag) {
                                log("key refresh step 6 --> step 7 not ack node address = " + MeshUtils.decimalToHexString("%04X", node.getAddr()) + " , status = " + node.getActiveStatus());
                            }
                        }
                        netKey.setState(MeshConstants.MESH_KEY_REFRESH_STATE_NONE);
                        netKey.getKeyRefreshAckNodes().clear();
                        netKey.getNodes().clear();
                        netKey.getNodes().putAll(netKey.getKeyRefreshNodes());
                        netKey.getKeyRefreshNodes().clear();
                        try {
                            getTimerById(netKey.getKeyRefreshTimerID()).cancel();
                            mTimerMap.remove(netKey.getKeyRefreshTimerID());
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                        updateStorageData(MyApplication.STORAGE_DATA_NETKEY);
                        netKey.setState(phase);
                        if (mBluetoothMeshListener != null) {
                            mBluetoothMeshListener.onKeyRefreshComplete(netKey.getIndex(), true);
                        }
                    }
                }
            } else {
                log("onKeyRefresh invalid phase state");
            }
        }

        @Override
        public void onHeartbeat(int address, int active) {
            Node node = getNodeByAddr(address);
            if (node == null) {
                log("heartbeat , but the node is not belong Node list");
                return ;
            }
            if (active == 1) { //online
                int currentHeartBeatNumber = node.getCurrentHeartBeatNumber();
                int preHeartBeatNumber = node.getPreHeartBeatNumber();
                currentHeartBeatNumber = currentHeartBeatNumber + 1;
                node.setCurrentHeartBeatNumber(currentHeartBeatNumber);
                log("onHeartbeat , address=" + MeshUtils.decimalToHexString("%04X",address) + " , active = " + active + ", currentHeartBeatNumber = " + currentHeartBeatNumber + " , preHeartBeatNumber = " + preHeartBeatNumber);
            }else { //offline
                log("onHeartbeat , address=" + MeshUtils.decimalToHexString("%04X",address) + " , active = " + active);
            }
            log("previous = " + node.getActiveStatus());
            if (active != node.getActiveStatus()) {
                node.setActiveStatus(active);
                //updateStorageData(MyApplication.STORAGE_DATA_NODE);
                //update UI here
                if(mBluetoothMeshListener != null) {
                    mBluetoothMeshListener.onNodeActiveStatusChange(node, active);
                }
            }
        }

        @Override
        public void onIvUpdate(int ivIndex, int state) {
            super.onIvUpdate(ivIndex, state);
            log("onKeyRefresh ivIndex = " + ivIndex + ", state = " + state);
        }

        @Override
        public void onSeqChange(int seqNumber) {
            super.onSeqChange(seqNumber);
        }

        @Override
        public void onAdvReport(int addrType, int[] addr, int rssi, int reportType, int[] data) {
            log("onAdvReport + addrType =" + addrType + ", rssi =" + rssi + ", reportType =" + reportType);
            StringBuilder sb = new StringBuilder();
            String devName = null;
            //String uuidStr = null;
            String svcName = null;
            for (int i = 0; i < 6; i++) {
                sb.append(String.format("%02x", addr[i]));
                if(i != 5) {
                    sb.append(":");
                }
            }
            log("adv report data: " + MeshUtils.intArrayToString(data, false));
            log("onAdvReport addr = " + sb.toString());

            int len = data.length;
            int index = 0;
            while(len > 0) {
                int current_adv_struct_len = data[index];
                log("current_adv_struct_len = " + current_adv_struct_len);
                if((data[index+1] == 0x09) || (data[index+1] == 0x08)) //BTM_BLE_AD_TYPE_NAME_CMPL or SHORT
                {
                    byte[] name = new byte[current_adv_struct_len - 1];
                    int j = 0;
                    for (int i = index+2; i <= index + current_adv_struct_len; i++) {
                        //log("device cmpl name: 0x" + String.format("%02x", data[i]));
                        name[j++] = Integer.valueOf(data[i]).byteValue();
                    }
                    devName = new String(name);
                    log("device name = " + devName);
                //} else if (data[index+1] == 0x07) {
                    //int[] uuid = new int[current_adv_struct_len - 1];
                    //int j = 0;
                    //for (int i = index+2; i <= index + current_adv_struct_len; i++) {
                        //uuid[j++] = data[i];
                    //}
                    //uuidStr = String.format("%02x%02x%02x%02x", uuid[0], uuid[1], uuid[2], uuid[3]);
                    //log("uuidStr = " + uuidStr);
                } else if (data[index+1] == 0x03) {
                    int svcUuid = data[index+2] | (data[index+3] << 8);
                    if (0x1827 == svcUuid) {
                        svcName = String.format("Provisioning");
                    } else if (0x1828 == svcUuid) {
                        //svcName = String.format("Proxy");
                        log("svcName = Proxy , it is Proxy service Gatt , not show in UI" );
                        return;
                    }
                    log("svcName = " + svcName);
                }
                len = len - (current_adv_struct_len+1);
                index = index + (current_adv_struct_len+1);
            }
            if (mBluetoothMeshListener != null) {
                if ((svcName != null) && (svcName.compareTo("Provisioning") == 0)) {
                    deleteLocalNodeByBear(MeshConstants.MESH_BEARER_GATT, null, sb.toString());
                    mBluetoothMeshListener.onScanUnProvDevicePBGatt(new StringBuffer().append(devName).append("--").append(svcName).toString(), sb.toString(), addrType, rssi);
                }
            }
        }

        @Override
        public void onBearerGattStatus(long handle, int status) {
            log("onBearerGattStatus: handle = " + handle + ", status = " + status);
            log("currentProvNode.configState = " + currentProvNode.getConfigState());
            if (status == MeshConstants.MESH_BEARER_GATT_STATUS_CONNECTED) {
                if (currentProvNode.getConfigState() == MESH_NODE_CONFIG_STATE_PROVISIONING) { // gatt provisioning connect success
                    log("PB-GATT provisioning service connect success , Start inviteProvisioning");
                    int[] dummy_uuid = new int[16];
                    mBluetoothMesh.inviteProvisioning(dummy_uuid, 5);
                    try {
                        if (getTimerById(currentProvNode.getProvisioningServiceTimerID())!= null) {
                            getTimerById(currentProvNode.getProvisioningServiceTimerID()).cancel();//gatt provisioning connect success , cancel provisioning timer
                            mTimerMap.remove(currentProvNode.getProvisioningServiceTimerID());
                            currentProvNode.setProvisioningServiceTimerID(-1);
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                } else if (currentProvNode.getConfigState() == MESH_NODE_CONFIG_STATE_PROVISIONED) { //gatt proxy connect success
                    log("PB-GATT proxy service connect success , Start to config PB-GATT device");
                    currentProvNode.setConfigState(MESH_NODE_CONFIG_STATE_GET_COMPOSITION_DATA);
                    currentProvNode.setGattConnectState(true);
                    MyApplication.GattConnectStatus = MyApplication.PB_GATT_CONNECT;
                    notifyProvState(currentProvNode, true);
                    try {
                        if (getTimerById(currentProvNode.getProxyServiceTimerID()) != null) {
                            getTimerById(currentProvNode.getProxyServiceTimerID()).cancel();//gatt proxy connect success , cancel proxy timer
                            mTimerMap.remove(currentProvNode.getProxyServiceTimerID());
                            currentProvNode.setProxyServiceTimerID(-1);
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    configMsgCompositionDataGet(currentProvNode, 0);
                } else if((currentProvNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) && currentProvNode.isConfigSuccess()) { // callback when click "Connect" button in Message list
                    try {
                        if (getTimerById(currentProvNode.getProxyServiceTimerID()) != null) {
                            getTimerById(currentProvNode.getProxyServiceTimerID()).cancel();//gatt proxy connect success , cancel proxy timer
                            mTimerMap.remove(currentProvNode.getProxyServiceTimerID());
                            currentProvNode.setProxyServiceTimerID(-1);
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    if(mBluetoothMeshListener != null) {
                        currentProvNode.setGattConnectState(true);
                        MyApplication.GattConnectStatus = MyApplication.PB_GATT_CONNECT;
                        mBluetoothMeshListener.GattConnectStatusChange(currentProvNode, true);
                    }
                }
            } else if (status == MeshConstants.MESH_BEARER_GATT_STATUS_DISCONNECTED) {
                MyApplication.GattConnectStatus = MyApplication.PB_GATT_DISCONNECT;
                if (currentProvNode.getConfigState() == MESH_NODE_CONFIG_STATE_PROVISIONED) {
                    log("PB-GATT provisioning service disconnect, start to connect proxy service");
                    //Strictly, we shall start the connection on MESH_GATT_SERVICE_PROXY ADV REPORT
                    mBluetoothMesh.gattConnect(currentProvNode.getGattAddr(), currentProvNode.getGattAddrType(), MeshConstants.MESH_GATT_SERVICE_PROXY);
                }else if(currentProvNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE && currentProvNode.isConfigSuccess()) { // callback when click "Disconnect" button in Message list
                        currentProvNode.setGattConnectState(false);
                        mBluetoothMeshListener.GattConnectStatusChange(currentProvNode, false);
                }
            } else if(status == MeshConstants.MESH_BEARER_GATT_STATUS_CONNECT_FAILED){ //0x3e
                if(currentProvNode.getConfigState() == MESH_NODE_CONFIG_STATE_PROVISIONING) { // gatt provisioning connect fail
                    if (gattProvisioningService) {
                        log("PB-GATT provisioning service connect fail , Start reconnect");
                        mBluetoothMesh.gattConnect(currentProvNode.getGattAddr(), currentProvNode.getGattAddrType(), MeshConstants.MESH_GATT_SERVICE_PROVISION);
                    }
                }else if(currentProvNode.getConfigState() == MESH_NODE_CONFIG_STATE_PROVISIONED) { //gatt proxy connect fail
                    if (gattProxyService) {
                        log("PB-GATT proxy service connect fail , Start reconnect ");
                        mBluetoothMesh.gattConnect(currentProvNode.getGattAddr(), currentProvNode.getGattAddrType(), MeshConstants.MESH_GATT_SERVICE_PROXY);
                    }
                }else if(currentProvNode.getConfigState() == MESH_NODE_CONFIG_STATE_IDLE) { //gatt proxy connect fail when click "connect" button in the Node
                    log("gatt proxy connect fail when click connect button in the Node , Start reconnect");
                    if (gattProxyService) {
                        mBluetoothMesh.gattConnect(currentProvNode.getGattAddr(), currentProvNode.getGattAddrType(), MeshConstants.MESH_GATT_SERVICE_PROXY);
                    }
                }
            }
        }

        @Override
        public void onOTAMsgHandler(int modelHandle, BluetoothMeshAccessRxMessage msg) {
            log("onOTAMsgHandler" + modelHandle + "msg opCode = 0x" + Integer.toHexString(msg.getOpCode()));
        }

        @Override
        public void onOTAEvent(int eventId, int errorCode, long serialNumber, long firmwareId, long timeEscaped,
            int nodesNum, int curr_block, int total_block, int curr_chunk, int chunk_mask, int[] nodesStatus) {
            log("onOTAEvent + eventId " + eventId + ", errorCode " + errorCode + ", nodesNum" + nodesNum +
                ",curr_block=" + curr_block + ",total_block=" + total_block + ",curr_chunk=" + curr_chunk + ",chunk_mask =" + chunk_mask);
            if (eventId == MeshConstants.MESH_OTA_EVENT_DISTRIBUTION_STOP) {
                mOtaState = -1;
                setMeshMode(MeshConstants.MESH_MODE_STANDBY);
            }
        }
    };

    public int getNetKeyCnt() {
        log("getNetKeyCnt count = " + this.mAllNetKeyMap.size());
        return this.mAllNetKeyMap.size();
    }

    public int getAppKeyCnt() {
        log("getNetKeyCnt count = " + this.mAllAppKeyMap.size());
        return this.mAllAppKeyMap.size();
    }

    public ArrayList<Integer> getGroupAddrList() {
        return this.mGroupAddrList;
    }

    public int addNewNetKey() {
        log("addNewNetKey mCurrMaxNetKeyIndex = " + mCurrMaxNetKeyIndex);
        NetKey key = new NetKey();
        int[] keyValue = new int[16];

        if ((mCurrMaxNetKeyIndex == 0) && (this.mAllNetKeyMap.size() == 0)) { //primay netkey, generic a random value
            //generate a random value
            /*
            long time = System.currentTimeMillis();
            keyValue[0] = (int)(time & 0xFF);
            keyValue[1] = (int)((time >> 8) & 0xFF);
            keyValue[2] = (int)((time >> 16) & 0xFF);
            keyValue[3] = (int)((time >> 24) & 0xFF);

            System.arraycopy(keyValue, 0, keyValue, 4, 4);
            System.arraycopy(keyValue, 0, keyValue, 8, 4);
            System.arraycopy(keyValue, 0, keyValue, 12, 4);*/
            System.arraycopy(mMyUUID, 0, keyValue, 0, 16);
            log("Primary NetKey = " + MeshUtils.intArrayToString(keyValue, false));
        } else {
            //Set the new key value based on the current max key value
            System.arraycopy(mAllNetKeyMap.get(mCurrMaxNetKeyIndex).getValue(), 0, keyValue, 0, 16);
            for (int i = 0; i < 16; i++) {
                keyValue[i] = keyValue[i] + 1;
            }
            if (isEnable()) {   //If enabled, we shall add the key to SDK, otherwise, the key will be added on enabled
                if (0 == mBluetoothMesh.setNetkey(MeshConstants.MESH_KEY_OP_ADD, keyValue, (mCurrMaxNetKeyIndex+1))) {
                    log("Add netkey " + (mCurrMaxNetKeyIndex+1) + ", value: " + MeshUtils.intArrayToString(keyValue, false) + ", success");
                } else {
                    log("Add netkey " + (mCurrMaxNetKeyIndex+1) + ", value: " + MeshUtils.intArrayToString(keyValue, false) + ", fail");
                    return -1;
                }
            }
            mCurrMaxNetKeyIndex++;
        }
        log("addNewNetKey, index = " + mCurrMaxNetKeyIndex + ", value = " + MeshUtils.intArrayToString(keyValue, false));
        key.setNetKeyMeta(mCurrMaxNetKeyIndex, keyValue);
        this.mAllNetKeyMap.put(mCurrMaxNetKeyIndex, key);
        mStorageData.addNetKey(key);
        updateStorageData(MyApplication.STORAGE_DATA_NETKEY);
        return 0;
    }

    public int addNewAppKey(int netkeyIndex) {
        log("addNewAppKey, mCurrMaxAppKeyIndex = " + mCurrMaxAppKeyIndex + ", netkey index = " + netkeyIndex);
        if (netkeyIndex > 0xFFF) {
            log("addNewAppKey invalid meta value");
            return -1;
        }
        if (null == mAllNetKeyMap.get(netkeyIndex)) {
            log("addNewAppKey, the netkey does not exist");
            return -2;
        }

        AppKey key = new AppKey();
        int[] keyValue = new int[16];

        if ((mCurrMaxAppKeyIndex == 0) && (this.mAllAppKeyMap.size() == 0)) { //first appkey, generic a random value
            //generate a random value
            /*
            long time = System.currentTimeMillis();
            keyValue[0] = (int)(time & 0xFF);
            keyValue[1] = (int)((time >> 8) & 0xFF);
            keyValue[2] = (int)((time >> 16) & 0xFF);
            keyValue[3] = (int)((time >> 24) & 0xFF);

            System.arraycopy(keyValue, 0, keyValue, 4, 4);
            System.arraycopy(keyValue, 0, keyValue, 8, 4);
            System.arraycopy(keyValue, 0, keyValue, 12, 4);*/
            System.arraycopy(mMyUUID, 0, keyValue, 0, 16);
            keyValue[15] = keyValue[15] + 1;
            log("First AppKey = " + MeshUtils.intArrayToString(keyValue, false));
        } else {
            //Set the new key value based on the current max key value
            System.arraycopy(mAllAppKeyMap.get(mCurrMaxAppKeyIndex).getValue(), 0, keyValue, 0, 16);
            for (int i = 0; i < 16; i++) {
                keyValue[i] = keyValue[i] + 1;
            }
            if (isEnable()) {   //If enabled, we shall add the key to SDK, otherwise, the key will be added on enabled
                if (0 == mBluetoothMesh.setAppkey(MeshConstants.MESH_KEY_OP_ADD, keyValue, netkeyIndex, (mCurrMaxAppKeyIndex+1))) {
                    log("Add appkey " + (mCurrMaxAppKeyIndex+1) + ", value: " + MeshUtils.intArrayToString(keyValue, false) + ", success");
                } else {
                    log("Add appkey " + (mCurrMaxAppKeyIndex+1) + ", value: " + MeshUtils.intArrayToString(keyValue, false) + ", fail");
                    return -1;
                }
            }
            mCurrMaxAppKeyIndex++;
        }
        log("addNewAppKey, index = " + mCurrMaxNetKeyIndex + ", value = " + MeshUtils.intArrayToString(keyValue, false) + ", with netkeyIndex = " + netkeyIndex);
        key.setAppKeyMeta(mCurrMaxAppKeyIndex, keyValue, netkeyIndex);
        this.mAllAppKeyMap.put(mCurrMaxAppKeyIndex, key);
        mStorageData.addAppKey(key);
        updateStorageData(MyApplication.STORAGE_DATA_APPKEY);
        return 0;
    }

    public Node getNodeByAddr(int addr) {
        return mNodes.get(addr);
    }

    public Node getNodeByUUID(int[] uuid) {
        for(Map.Entry<Integer,Node> entry:mNodes.entrySet()) {
            Node node = entry.getValue();
            //log("node.getUUID() = " + MeshUtils.intArrayToString(node.getUUID(), true));
            log("uuid = " + MeshUtils.intArrayToString(uuid, true));
            if (Arrays.equals(node.getUUID(), uuid)) {
                return node;
            }
        }
        return null;
    }

    public Node getNodeByGattAddr(String address) {
        for(Map.Entry<Integer,Node> entry:mNodes.entrySet()) {
            Node node = entry.getValue();
            if (node.getGattAddr().equals(address)) {
                return node;
            }
        }
        return null;
    }

    public NetKey getNetKeyByIndex(int index) {
        return mAllNetKeyMap.get(index);
    }

    public HashMap<Integer, Node> getNodes(){
        return mNodes;
    }

    //get all the nodes which are using the netkey
    public HashMap<Integer, Node> getNodesByNetKey(int netKeyIndex) {
        log("getNodesByNetKey node number = " + mAllNetKeyMap.get(netKeyIndex).getNodes().size());
        return mAllNetKeyMap.get(netKeyIndex).getNodes();
    }

    private void deleteLocalNode(Node node, boolean nodeReset) {
        log("deleteLocalNode, addr = " + node.getAddr() + ", uuid = " + MeshUtils.intArrayToString(node.getUUID(), true) + " , nodeReset = " + nodeReset);
        //remove the node and its device key
        int dst = node.getAddr();
        int mHeartBeatTimerID = node.getHeartBeatTimerID();
        //mStorageData.deleteNode(node.getAddr());
        updateNodeData(new NodeData(node), MyApplication.NODE_DATA_DELETE);
        mAllNetKeyMap.get(node.getCurrNetkeyIndex()).getNodes().remove(node.getAddr());
        if (!nodeReset) {
            mBluetoothMesh.delDevKey(node.getAddr());
        }
        mNodes.remove(node.getAddr());
        updateStorageData(MyApplication.STORAGE_DATA_ALL);
        if(node.getNodeBearer() == MeshConstants.MESH_BEARER_GATT) {// PB-GATT device need disconnect when  remove from node list
            mBluetoothMesh.gattDisconnect();
        }
        resendMsgListLock.lock();
        try {
            ListIterator<ResendConfigMsg> it = resendMsgList.listIterator(0);
            while (it.hasNext()) {
                ResendConfigMsg msg = it.next();
                if(dst == msg.mDst) {
                    if(!nodeReset) {
                        msg.stopResend();
                        it.remove();
                    }
                }
            }
            //remove the timer bound by the node
            Iterator<Integer> itTimer = mTimerMap.keySet().iterator();
            while (itTimer.hasNext()) {
                int timerId = itTimer.next();
                if(mHeartBeatTimerID == timerId) {
                    getTimerById(timerId).cancel();
                    itTimer.remove();
                }
            }
        } finally {
        }
        resendMsgListLock.unlock();
    }

    private void deleteLocalNodeByBear(int bearType, int[] uuid, String address) {
        log("deleteLocalNodeByBear, bearType = " + bearType);
        Set<Integer> set = mNodes.keySet();
        for (Iterator it = set.iterator(); it.hasNext();) {
            int index = (Integer)it.next();
            Node node = mNodes.get(index);
            if (bearType == MeshConstants.MESH_BEARER_GATT && node.getNodeBearer() == MeshConstants.MESH_BEARER_GATT) {
                if (node.getGattAddr().compareTo(address) == 0) {
                    log("deleteLocalNodeByBear, addr = " + node.getAddr() + ", MAC address= " + address);
                    //remove the node and its device key
                    deleteLocalNode(node, false);
                    return;
                }
            } else if(bearType == MeshConstants.MESH_BEARER_ADV && node.getNodeBearer() == MeshConstants.MESH_BEARER_ADV){
                if (Arrays.equals(uuid, node.getUUID())) {
                    log("deleteLocalNodeByBear, addr = " + node.getAddr() + ", uuid = " + MeshUtils.intArrayToString(uuid, true));
                    //remove the node and its device key
                    deleteLocalNode(node, false);
                    return;
                }
            }
        }
        log("deleteLocalNodeByBear, no such a node is found");
    }

    private synchronized void deleteBlacklistNodes(int netkeyIndex) {
        log("deleteBlacklistNodes , netkeyIndex = " + netkeyIndex);
        Set<Integer> set = mNodes.keySet();
        ArrayList<Integer> list = new ArrayList<>();
        for (Iterator it = set.iterator(); it.hasNext();) {
            int addr = (Integer)it.next();
            if ((mNodes.get(addr).getCurrNetkeyIndex() == netkeyIndex) &&
                (!mAllNetKeyMap.get(netkeyIndex).getKeyRefreshNodes().containsKey(addr))) {
                list.add(addr);
            }
        }
        for (int i = 0; i < list.size(); i++) {
            if (mBluetoothMeshListener != null) {
                mBluetoothMeshListener.onNodeRemoved(true, mNodes.get(list.get(i)));
            }
            deleteLocalNode(mNodes.get(list.get(i)), false);
        }
    }

    public int keyRefreshStart(int netKeyIndex, ArrayList<Node> targetNodes) {
        log("keyRefreshStart netkey index = " + netKeyIndex + " , current mesh mode = " + getMeshMode());
        //return 0;
        NetKey netKey = mAllNetKeyMap.get(netKeyIndex);
        int nodeCount = 0;

        if (null == netKey) {
            log("keyRefreshStart error, the netkey " + netKeyIndex + " does not exist");
            return -1;
        }


        if (netKey.getState() != MeshConstants.MESH_KEY_REFRESH_STATE_NONE) {
            log("key refresh is ongoing, please wait and try again");
            return -3;
        }
        if(targetNodes != null) {
            for (int i = 0; i < targetNodes.size(); i++) {
                if (targetNodes.get(i).getCurrNetkeyIndex() != netKeyIndex) {
                    log("keyRefreshStart error, not all the nodes are in the same network");
                    netKey.getKeyRefreshNodes().clear();
                    return -4;
                }
                // put targetNodes to netKey.getKeyRefreshNodes()
                netKey.getKeyRefreshNodes().put(targetNodes.get(i).getAddr(), targetNodes.get(i));
                nodeCount++;
            }
        }
        log("keyRefreshStart nodeCount = " + nodeCount);

        //Just modify the last byte of the key as the new key value
        int[] key = new int[16];
        System.arraycopy(netKey.getValue(), 0, key, 0, 16);
        key[netKeyIndex] = key[netKeyIndex] + 1;

        if (getMeshMode() != MeshConstants.MESH_MODE_ON) {
            setMeshMode(MeshConstants.MESH_MODE_ON);
        }

        log("key refresh procedure step 1 --> update local new key value");
        netKey.setTempValue(key);
        log("new key value: " + MeshUtils.intArrayToString(key, false));
        int ret = mBluetoothMesh.setNetkey(MeshConstants.MESH_KEY_OP_UPDATE, key, netKeyIndex);
        if (0 == ret) {
            log("keyRefreshStart success");
            isKeyrefreshing = true;
            MyApplication.keyrefreshTime = System.currentTimeMillis();
        } else {
            log("keyRefreshStart fail");
            if (mBluetoothMeshListener != null) {
                mBluetoothMeshListener.onKeyRefreshComplete(netKey.getIndex(), false);
            }
        }

        return ret;
    }

    public boolean removeNode(Node node, boolean bNodeReset) {
        log("removeNode, bNodeReset = " + bNodeReset);
        if (!mNodes.containsKey(node.getAddr())) {
            log("node is not exist");
            return false;
        }
        log("removeNode, node addr =" + MeshUtils.decimalToHexString("%04X", node.getAddr()));

        if (bNodeReset) {
            configMsgNodeReset(node, true, 1);   //configMsgNodeReset not need retry
        }

        //Trigger key refresh for the other nodes which share the same netkey with the node to be removed.
        HashMap<Integer, Node> nodeMap = getNodesByNetKey(node.getCurrNetkeyIndex());
        if (null == nodeMap) {
            log("warning, nodeMap is null, this shall not happen...");

            //remove the node and its device key
            //mStorageData.deleteNode(node.getAddr());
            updateNodeData(new NodeData(node), MyApplication.NODE_DATA_DELETE);
            mBluetoothMesh.delDevKey(node.getAddr());
            mNodes.remove(node.getAddr());
            //updateStorageData(MyApplication.STORAGE_DATA_NODE);

            keyRefreshStart(node.getCurrNetkeyIndex(), null);
        } else {
            log("nodeMap size for netkey " + node.getCurrNetkeyIndex() + " is " + nodeMap.size());
            ArrayList<Node> nodeList = new ArrayList<Node>(nodeMap.values());
            for (int i = 0; i < nodeList.size(); i++) {
                if (nodeList.get(i).getAddr() == node.getAddr()) {
                    log("remove node " + node.getAddr() + " from whitelist");
                    nodeList.remove(i);
                    break;
                }
            }
            if (!bNodeReset) {  //node reset will delete the node
                deleteLocalNode(node, false);
            }
            if (nodeList.size() == 0) {
                keyRefreshStart(node.getCurrNetkeyIndex(), null);
            } else {
                keyRefreshStart(node.getCurrNetkeyIndex(), nodeList);
            }
        }

        return true;
    }

    public boolean otaStart(int netKeyIndex, ArrayList<Node> targetNodes, int appkeyIndex) {
        log("otaStart() netkey = " + netKeyIndex + ", appkey = " + appkeyIndex);
        if (mOtaState != -1) {
            log("current an ota procedure is ongoing, please wait its completion!!!");
            return false;
        }

        if (targetNodes == null || targetNodes.size() == 0) {
            log("invalid target nodes!!!");
            return false;
        }

        mOtaState = MeshConstants.MESH_OTA_EVENT_DISTRIBUTION_STARTING;

        mOtaParams.setOtaInitiatorMsgHandler(appkeyIndex);
        mBluetoothMesh.otaInitiatorOperation(mOtaParams);

        //hard code just for ota flow test
        String fw = "/data/data/com.android.bluetooth/ota_obj_file_FF000028.bin";
        int[] updaters = new int[targetNodes.size()];
        for (int i = 0; i < targetNodes.size(); i++) {
            updaters[i] = targetNodes.get(i).getAddr();
        }
        int[] obj_id = {0x1a, 0xf3, 0xf6, 0x47, 0xFF, 0x00, 0x00, 0x28};
        mOtaParams.setOtaInitiatorStartParams(fw.getBytes(), 0x25ab0, obj_id, 0xFF000028, appkeyIndex, mPrimaryElement.getAddress(),
                                                0xC002, targetNodes.size(), updaters, false);
        if (0 != mBluetoothMesh.otaInitiatorOperation(mOtaParams)) {
            log("ota start fail!!!");
            mOtaState = -1;
            return false;
        }

        setMeshMode(MeshConstants.MESH_MODE_ON);
        log("otaStart() done");
        return true;
    }

    public boolean otaStop() {
        log("otaStop()");
        if (mOtaState == -1) {
            log("nothing to stop");
            return false;
        } else {
            mOtaParams.setOtaInitiatorStopParams(mOtaParams.getFwId(), mOtaParams.getDistributorAddr());
            mBluetoothMesh.otaInitiatorOperation(mOtaParams);
        }
        return true;
    }

    public boolean otaApply() {
        log("otaApply()");
        if (mOtaState != MeshConstants.MESH_OTA_EVENT_DISTRIBUTION_DFU_READY) {
            log("nothing to apply");
            return false;
        } else {
            mOtaParams.setOtaInitiatorApplyDistribution();
            mBluetoothMesh.otaInitiatorOperation(mOtaParams);
        }
        return true;
    }

    public void setBluetoothMeshListener(BluetoothMeshListener mBluetoothMeshListener) {
        this.mBluetoothMeshListener = mBluetoothMeshListener;
        log("mBluetoothMeshListener = " + mBluetoothMeshListener);
    }

    public ArrayList<MeshMessage> getSupportedMessages(Node node) {
        ArrayList<MeshMessage> list= new ArrayList<>();
        for (int i = 0; i < node.getSupportedRxMsgList().size(); i++) {
            Iterator<Integer> it = supportedMsgSet.iterator();
            while (it.hasNext()) {
                try {
                    if (it.next().intValue() == node.getSupportedRxMsgList().get(i).getOpCode()) {
                        //This message opode is supported by both
                        list.add(node.getSupportedRxMsgList().get(i));
                    }
                } catch (Exception e) {
                    log(e.toString());
                }
            }
        }
        return list;
    }

    public void sendMessage(MeshMessage msg, Node node, int appKeyIndex, int groupAddr, int[] payload) {
        int dst = 0;

        log("sendMessage opcode = " + MeshUtils.decimalToHexString("%04X", msg.getOpCode())+ ", groupAddr = " + MeshUtils.decimalToHexString("%04X", groupAddr) +
            ", msg.addr = " + MeshUtils.decimalToHexString("%04X", msg.getElementAddr()) + " , current mesh mode = " + getMeshMode());

        dst = ( groupAddr == 0 ) ? msg.getElementAddr() : groupAddr;

        if (isGroupSendMsging) {
            log("group sending msg state, return!!");
            return;
        }

        cfSuccessNodeList.clear();
        ackNodeSendMsgList.clear();
        ackGroupSendMsgList.clear();
        groupMsgTimeList.clear();

        if (dst == 0xFFFF) { // group send msg
            if (msg.getOpCode() == MeshConstants.MESH_MSG_GENERIC_ONOFF_GET || msg.getOpCode() == MeshConstants.MESH_MSG_GENERIC_ONOFF_SET) {
                isGroupSendMsg = true;
                isGroupSendMsging = true;
                groupSMNodeAddr = node.getAddr();

                groupMsgTimeList.add("config fail node as follows:");
                for(Map.Entry<Integer,Node> entry:mNodes.entrySet()) {
                    if (entry.getValue().isConfigSuccess()) { //config success node
                        cfSuccessNodeList.add(entry.getKey());
                    } else { //config fail node
                        if (entry.getValue().getNodeBearer() == MeshConstants.MESH_BEARER_ADV) {
                            groupMsgTimeList.add(MeshUtils.intArrayToString(entry.getValue().getUUID(), true));
                        } else if (entry.getValue().getNodeBearer() == MeshConstants.MESH_BEARER_GATT) {
                            groupMsgTimeList.add(entry.getValue().getGattAddr());
                        }
                    }
                }
                log("all node size = " + mNodes.size() + " , config success node size = " + cfSuccessNodeList.size());
                if (cfSuccessNodeList.size() == 0) {
                    return;
                }

                if (mBluetoothMeshListener != null) {
                    groupMsgTimeList.add("ack group send msg node as follows:");
                    mBluetoothMeshListener.onGroupSendMsg(MyApplication.GROUP_SEND_MSG_START, null);
                }
            }
        } else {
            isGroupSendMsg = false;
            isGroupSendMsging = false;
            groupSMNodeAddr = 0;
        }

        if (getMeshMode() != MeshConstants.MESH_MODE_ON) {
            setMeshMode(MeshConstants.MESH_MODE_ON);
        }
        currentProvNode = node;
        int id = createTimer();
        if (-1 != id) {
            currentProvNode.setSendMessageTimerID(id);
            if (isGroupSendMsg) {
                getTimerById(id).schedule(new ExTimerTask<Node>(7, currentProvNode), 2000 * (cfSuccessNodeList.size())); // send message timeout : 2s * node number
            } else {
                getTimerById(id).schedule(new ExTimerTask<Node>(7, currentProvNode), 2000); // send message timeout : 2s
            }
        }

        MyApplication.sendMessageTime = System.currentTimeMillis();
        MyApplication.groupSendMessageTime = System.currentTimeMillis();

        switch (msg.getOpCode()) {
            case MeshConstants.MESH_MSG_GENERIC_ONOFF_GET: {
                mGenericOnOffClient.setTxMessageHeader(parseAddressType(dst),
                                                    dst,
                                                    null,
                                                    mPrimaryElement.getAddress(),
                                                    defaultTTL, node.getCurrNetkeyIndex(), appKeyIndex, msg.getOpCode());
                mGenericOnOffClient.genericOnOffGet();
                addResendMsg(node, 0, 0, 0, 0, MeshConstants.MESH_MSG_GENERIC_ONOFF_GET, SEND_MSG_DEFAULT_RESEND_TIMES);
                break;
            }
            case MeshConstants.MESH_MSG_GENERIC_ONOFF_SET:
                mGenericOnOffClient.setTxMessageHeader(parseAddressType(dst),
                                                    dst,
                                                    null,
                                                    mPrimaryElement.getAddress(),
                                                    defaultTTL, node.getCurrNetkeyIndex(), appKeyIndex, msg.getOpCode());
                mGenericOnOffClient.genericOnOffSet(payload[0], onOffTID++, 0, 0);
                addResendMsg(node, payload[0] , 0, 0, 0, MeshConstants.MESH_MSG_GENERIC_ONOFF_SET, SEND_MSG_DEFAULT_RESEND_TIMES);
                break;
            case MeshConstants.MESH_MSG_GENERIC_ONOFF_SET_UNRELIABLE: {
                mGenericOnOffClient.setTxMessageHeader(parseAddressType(dst),
                                                    dst,
                                                    null,
                                                    mPrimaryElement.getAddress(),
                                                    defaultTTL, node.getCurrNetkeyIndex(), appKeyIndex, msg.getOpCode());
                mGenericOnOffClient.genericOnOffSetUnacknowledged(payload[0], onOffTID++, 0, 0);
                break;
            }
            case VendorModelDemoClient.VENDOR_MSG_OPCODE_STATE_GET: {
                mVendorModelClient.setTxMessageHeader(parseAddressType(dst),
                                                    dst,
                                                    null,
                                                    mPrimaryElement.getAddress(),
                                                    defaultTTL, node.getCurrNetkeyIndex(), appKeyIndex, msg.getOpCode());
                mVendorModelClient.vendorStateGet();
                break;
            }
            case VendorModelDemoClient.VENDOR_MSG_OPCODE_STATE_SET:
            case VendorModelDemoClient.VENDOR_MSG_OPCODE_STATE_SET_UNACK: {
                mVendorModelClient.setTxMessageHeader(parseAddressType(dst),
                                                    dst,
                                                    null,
                                                    mPrimaryElement.getAddress(),
                                                    defaultTTL, node.getCurrNetkeyIndex(), appKeyIndex, msg.getOpCode());
                mVendorModelClient.vendorStateSet(payload[0]);
                break;
            }
            //Add more...
            default:
                break;
        }
    }
}
