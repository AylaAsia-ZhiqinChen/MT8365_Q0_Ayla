package com.mesh.test.provisioner;

import android.bluetooth.BluetoothMesh;
import android.bluetooth.mesh.*;
import android.bluetooth.mesh.model.*;
import android.Manifest;
import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.IBinder;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;
import com.example.slideview.SlideSwitch;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import com.mesh.test.provisioner.adapter.UnprovisionedAdapter;
import com.mesh.test.provisioner.dialogfragment.ProvisionedDialogFragment;
import com.mesh.test.provisioner.dialogfragment.UnprovisionedDialogFragment;
import com.mesh.test.provisioner.listener.BluetoothMeshListener;
import com.mesh.test.provisioner.listener.NoDoubleOnItemClickListener;
import com.mesh.test.provisioner.model.Element;
import com.mesh.test.provisioner.model.Model;
import com.mesh.test.provisioner.sqlite.LouSQLite;
import com.mesh.test.provisioner.sqlite.MyCallBack;
import com.mesh.test.provisioner.sqlite.Node;
import com.mesh.test.provisioner.sqlite.StorageData;
import com.mesh.test.provisioner.sqlite.NodeData;
import com.mesh.test.provisioner.sqlite.ProvisionedDeviceEntry;
import java.lang.reflect.Field;
import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import com.mesh.test.provisioner.util.MeshUtils;
import android.widget.ProgressBar;
import android.os.Handler;
import android.widget.Button;
import android.view.Menu;
import android.view.MenuItem;
import android.view.ContextMenu;
import android.view.ViewConfiguration;
import android.widget.ArrayAdapter;
import android.app.AlertDialog;
import android.view.LayoutInflater;
import android.widget.Spinner;
import android.content.DialogInterface;
import com.mesh.test.provisioner.sqlite.StorageData;
import android.content.BroadcastReceiver;
import android.bluetooth.mesh.MeshConstants;
import com.mesh.test.provisioner.sqlite.UnProvisionedDevice;
import android.widget.AdapterView;
import java.util.HashMap;
import java.util.Map;
import com.mesh.test.provisioner.adapter.CheckBoxAdapter;
import java.util.Iterator;
import java.io.IOException;
import android.content.res.Configuration;
import android.widget.RelativeLayout;
import android.support.v7.app.AppCompatActivity;
import android.view.View.OnClickListener;
import com.mesh.test.provisioner.listener.RecyclerViewItemClickListener;
import com.mesh.test.provisioner.listener.RecyclerViewItemLongClickListener;
import com.mesh.test.provisioner.adapter.NodeAdapter;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView.Recycler;
import android.support.v7.widget.RecyclerView.State;
import android.app.Dialog;
import android.view.Gravity;
import java.util.Timer;
import java.util.TimerTask;
import android.os.SystemProperties;



public class MainActivity extends AppCompatActivity implements SlideSwitch.SlideListener, AdapterView.OnItemClickListener, OnClickListener, RecyclerViewItemClickListener, RecyclerViewItemLongClickListener {

    private static final String TAG = "ProvisionerMainActivity";

    private static final boolean DEBUG = true;

    private SlideSwitch mSlideSwitch;
    private TextView slideText;
    private TextView closeDescription;
    private LinearLayout llState;
    private LinearLayout llOpen;
    private RelativeLayout llClose;
    private CustomRecyclerView nodeRecyclerView;
    private CustomRecyclerView unProvisionedRecyclerView;
    private NodeAdapter nodeAdapter;
    private UnprovisionedAdapter unProvisionedAdapter;
    private ProgressBar mProgressBar;
    private List<StorageData> storageDataList = new ArrayList<>();
    private List<NodeData> nodeDataList = new ArrayList<>();

    //adb group provision
    private ArrayList<int[]> uuidNoneExistent = new ArrayList<>();
    private ArrayList<int[]> uuidProFail = new ArrayList<>();
    private ArrayList<int[]> uuidConfigFail = new ArrayList<>();
    private ArrayList<int[]> uuidConfigSuccess = new ArrayList<>();
    private int uuidIndex = 0;
    private int[] uuid = null;
    private int deviceSize = 0;

    //adb group node reset
    private ArrayList<int[]> nodeResetNoneExistent = new ArrayList<>();
    private ArrayList<Node> nodeResetList = new ArrayList<>();
    private ArrayList<Node> nodeAllResetList = new ArrayList<>();
    private HashMap<Integer, Node> ackNodeResetList = new HashMap<>();
    private ArrayList<Node> remainNodeList = new ArrayList<>();
    private int[] nodeResetUUID = null;
    private int nodeResetSize = 0;

    private static final int ALL_NODE_RESET = 3;
    private static final int PARTIAL_NODE_RESET = 4;
    private static final int SINGLE_NODE_RESET = 5;

    private static final int REQUEST_ENABLE_BT = 1;
    private static final int REQUEST_ENABLE_LOCATION = 2;
    private static final int SCAN_DURATION = 20*1000;

    private BluetoothAdapter mBluetoothAdapter;
    private ProvisionerService mProvisionerService;
    //private Handler mHandler = new Handler();
    private Runnable mRunnable;

    private int addappkey_netkeyIndex = 0;
    private int keyfresh_netkeyIndex = 0;
    private int ota_op_appkeyIndex = 0;
    private int ota_op_netkeyIndex = 0;
    private boolean isOpenMeshSwitch = false;
    private boolean isOpen = false;
    private boolean isScanning = false;
    private boolean meshMode = false;
    private BluetoothStateChangeBroadCast mBluetoothStateChangeBroadCast;
    private IntentFilter statusFilter;
    private Intent serviceIntent;
    private List<Node> nodes = new ArrayList<>();
    private boolean isForeground = false;
    private ProvisionedDialogFragment mProvisionedDialogFragment;
    private LinearLayoutManager mNodeLayoutManager;
    private LinearLayoutManager mUnProvisionedLayoutManager;
    private int nodePosition;
    private Dialog dialogKeyrefresh;
    private Dialog dialogNodeReset;
    private Dialog dialogGroupSendMsg;
    private Dialog dialogGattConnect;
    private Timer groupNodeResetTimer;


    private void log(String string) {
        if(DEBUG) {
            Log.i(TAG, string);
        }
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        log("onCreate");

        //Check Location permission
        //checkLocationPermission(this);

        // for backward compatible with MR1.1+GATT & MR2
        // it should load the static variable dynamically
        String strFeature = null;
        try {
            Class<?> cls = Class.forName("android.content.pm.PackageManager");
            Field field = cls.getField("FEATURE_BLUETOOTH_LE");
            strFeature = (String) field.get(cls);
        } catch (Exception e) {
            Toast.makeText(this, R.string.load_feature_fail + ":" + e.toString(),
                    Toast.LENGTH_SHORT).show();
            finish();
        }
        if (!getPackageManager().hasSystemFeature(strFeature)) {
            Toast.makeText(this, R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
            finish();
        }

        // Initializes a Bluetooth adapter. For API level 18 and above, get a
        // reference to
        // BluetoothAdapter through ProvisionerService.
        final BluetoothManager bluetoothManager =
                (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        mBluetoothAdapter = bluetoothManager.getAdapter();

        // Checks if Bluetooth is supported on the device.
        if (mBluetoothAdapter == null) {
            Toast.makeText(this, R.string.error_bluetooth_not_supported, Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        mSlideSwitch = (SlideSwitch) findViewById(R.id.slideSwitch);
        slideText = (TextView) findViewById(R.id.switchText);
        closeDescription = (TextView) findViewById(R.id.close_description);
        llState = (LinearLayout) findViewById(R.id.llstate);
        llClose = (RelativeLayout) findViewById(R.id.llclose);
        llOpen = (LinearLayout) findViewById(R.id.llopen);
        mProgressBar = (ProgressBar)findViewById(R.id.refresh);
        nodeRecyclerView = (CustomRecyclerView) findViewById(R.id.node);
        nodeRecyclerView.setItemAnimator(null);
        unProvisionedRecyclerView = (CustomRecyclerView) findViewById(R.id.unprovisioned);
        int nodeShowNumber = getResources().getInteger(R.integer.node_show_number);
        log("nodeShowNumber = " + nodeShowNumber);
        mNodeLayoutManager = new LinearLayoutManager(this){
            @Override
            public void onMeasure(Recycler recycler, State state,
                    int widthSpec, int heightSpec) {
                int count = state.getItemCount();

                if (count > 0) {
                    if(count > nodeShowNumber){
                        count = nodeShowNumber;
                    }
                    int realHeight = 0;
                    int realWidth = 0;
                    for(int i = 0;i < count; i++){
                        View view = recycler.getViewForPosition(0);
                        if (view != null) {
                            measureChild(view, widthSpec, heightSpec);
                            int measuredWidth = View.MeasureSpec.getSize(widthSpec);
                            int measuredHeight = view.getMeasuredHeight();
                            realWidth = realWidth > measuredWidth ? realWidth : measuredWidth;
                            realHeight += measuredHeight;
                        }
                        setMeasuredDimension(realWidth, realHeight);
                    }
                } else {
                    super.onMeasure(recycler, state, widthSpec, heightSpec);
                }
            }
        };
        mUnProvisionedLayoutManager = new LinearLayoutManager(this);

        mNodeLayoutManager.setOrientation(LinearLayoutManager.VERTICAL);
        nodeRecyclerView.setLayoutManager(mNodeLayoutManager);
        nodeRecyclerView.addItemDecoration(new RecycleViewDivider(this, RecycleViewDivider.HORIZONTAL_LIST));

        mUnProvisionedLayoutManager.setOrientation(LinearLayoutManager.VERTICAL);
        unProvisionedRecyclerView.setLayoutManager(mUnProvisionedLayoutManager);
        unProvisionedRecyclerView.addItemDecoration(new RecycleViewDivider(this, RecycleViewDivider.HORIZONTAL_LIST));
        llState.setOnClickListener(this);
        nodeRecyclerView.setOnCreateContextMenuListener(this);
        getProvisionedData();
        nodeAdapter = new NodeAdapter(this,MyApplication.nodeList);
        unProvisionedAdapter = new UnprovisionedAdapter(this);
        nodeAdapter.setHasStableIds(true);
        nodeAdapter.setOnItemClickListener(this);
        nodeAdapter.setOnItemLongClickListener(this);
        unProvisionedAdapter.setOnItemClickListener(this);
        nodeRecyclerView.setAdapter(nodeAdapter);
        unProvisionedRecyclerView.setAdapter(unProvisionedAdapter);
        registerForContextMenu(nodeRecyclerView);
        mSlideSwitch.setSlideListener(this);
        setPropertiesFile();
        serviceIntent = new Intent(this, ProvisionerService.class);
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            log("startForegroundService");
            startForegroundService(serviceIntent);
        } else {
            log("startService");
            startService(serviceIntent);
        }
        bindService(serviceIntent, mServiceConnection, BIND_AUTO_CREATE);
        getOverflowMenu();
        mBluetoothStateChangeBroadCast = new BluetoothStateChangeBroadCast();
        statusFilter = new IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED);
        statusFilter.addAction("android.action.PB_ADV_SEND_MSG");
        statusFilter.addAction("android.action.PB_GATT_SEND_MSG");
        statusFilter.addAction("android.action.MESH_GROUP_SEND_MSG");
        statusFilter.addAction("android.action.MESH_ON");
        statusFilter.addAction("android.action.MESH_OFF");
        statusFilter.addAction("android.action.PB_GATT_CONNECT");
        statusFilter.addAction("android.action.PB_GATT_DISCONNECT");
        statusFilter.addAction("android.action.MESH_ADV_PROVISION");
        statusFilter.addAction("android.action.MESH_GATT_PROVISION");
        statusFilter.addAction("android.action.MESH_ADV_REMOVE");
        statusFilter.addAction("android.action.MESH_GATT_REMOVE");
        statusFilter.addAction("android.action.MESH_SCAN");
        statusFilter.addAction("android.action.MESH_DUMP");
        statusFilter.addAction("android.action.MESH_GROUP_PROVISION");
        statusFilter.addAction("android.action.MESH_GROUP_NODE_RESET");
        registerReceiver(mBluetoothStateChangeBroadCast, statusFilter);
        //setPropertiesFile();
        if(savedInstanceState != null) {
            log("savedInstanceState != null");
            isOpen = savedInstanceState.getBoolean("isOpen");
        }

    }

    private final ServiceConnection mServiceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mProvisionerService = ((ProvisionerService.LocalBinder) service).getService();
            log("mProvisionerService = " + mProvisionerService + " MeshDev APP is running = " + !(mProvisionerService.isMeshAvailable()));
            if (false == mProvisionerService.isMeshAvailable()) {
                mSlideSwitch.setSlideable(false);
                log("MeshDev APP is running, please close MeshDev");
                Toast.makeText(MainActivity.this, "MeshDev APP is running, please close MeshDev", Toast.LENGTH_SHORT).show();
                finish();
            }
            checkLocationPermission(MainActivity.this);
            mProvisionerService.setBluetoothMeshListener(mBluetoothMeshListener);
            if(isOpen) {
                mSlideSwitch.setSwitchState(false);
                isOpen = false;
            }
            log("storageDataList.size() = " + storageDataList.size());
            if(storageDataList.size() > 0) {
                StorageData mStorageData = storageDataList.get(0);
                mProvisionerService.dataRecovery(mStorageData, MyApplication.nodeList);
            } else {    //fresh start, clear provisioner service data
                log("fresh start, clear provisioner service data , mProvisionerService.isMeshAvailable() = " + mProvisionerService.isMeshAvailable());
                if (true == mProvisionerService.isMeshAvailable()) {
                    mProvisionerService.dataReset();
                }
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {
            mProvisionerService = null;

        }
    };

    public void setPropertiesFile(){
        String filePath = "/data/data/" + this.getPackageName() + "/mesh_properties.txt";
        String content = null;
        try {
            content = MeshUtils.readFileFromAssets(this,"mesh_properties.txt");
            if(content == null) {
                log("mesh_properties.txt parse fail");
                return;
            }
        } catch (IllegalArgumentException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        if(! MeshUtils.isFileExisted(filePath)){
            MeshUtils.save(filePath, content);
        }
        int nodeSize = MyApplication.nodeList.size();
        int elementOrNodeAddress = 0;
        if (nodeSize > 0) {
            Node node = MyApplication.nodeList.get(nodeSize - 1); // last node
            int elementSize = node.getElements().size();
            if (elementSize > 0) {
                Element element = node.getElements().get(elementSize - 1); //last element
                elementOrNodeAddress =  element.getAddress();
            } else {
                elementOrNodeAddress = node.getAddr();
            }
        }
        log("elementOrNodeAddress = " + elementOrNodeAddress);
        String last_element_addr = SystemProperties.get("persist.bluetooth.last.element.addr", "NoExist");
        log("last_element_addr = " + last_element_addr);
        if (last_element_addr.equals("NoExist")) {
            MeshUtils.writeProperties(filePath, "last_element_addr", ((elementOrNodeAddress == 0)? "100" : (elementOrNodeAddress + "")));
        }
    }


    public void getProvisionedData() {
        log("getProvisionedData()");
        storageDataList = LouSQLite.query(MyCallBack.TABLE_NAME_STORAGE
                , "select * from " + MyCallBack.TABLE_NAME_STORAGE
                , null);
        nodeDataList = LouSQLite.query(MyCallBack.TABLE_NAME_NODE
                , "select * from " + MyCallBack.TABLE_NAME_NODE
                , null);
        log("storageDataList size = " + storageDataList.size());
        log("nodeDataList size = " + nodeDataList.size());
        if(storageDataList.size() == 0) {
            StorageData data = new StorageData();
            data.setId(ProvisionedDeviceEntry.STORAGE_ID);
            LouSQLite.insert(MyCallBack.TABLE_NAME_STORAGE, data);
        } else {
            ArrayList<NetKey> mAllNetKey = storageDataList.get(0).getAllNetKey();
            if(mAllNetKey != null) {
                if (mAllNetKey.size() > 0) {
                    for (int i = 0; i <= mAllNetKey.size() -1; i++) {
                        NetKey key = mAllNetKey.get(i);
                        log("key.getState() = " + key.getState());
                        if (key.getState() != MeshConstants.MESH_KEY_REFRESH_STATE_NONE) {
                            log("net key state is not MESH_KEY_REFRESH_STATE_NONE , need delete sqlite data");
                            //LouSQLite.delete(MyCallBack.TABLE_NAME_STORAGE, ProvisionedDeviceEntry.COLEUM_NAME_ID + "=?", new String[]{storageDataList.get(0).getId()});
                            LouSQLite.deleteFrom(MyCallBack.TABLE_NAME_STORAGE);
                            LouSQLite.deleteFrom(MyCallBack.TABLE_NAME_NODE);
                            storageDataList = LouSQLite.query(MyCallBack.TABLE_NAME_STORAGE
                                    , "select * from " + MyCallBack.TABLE_NAME_STORAGE
                                    , null);
                            nodeDataList = LouSQLite.query(MyCallBack.TABLE_NAME_NODE
                                    , "select * from " + MyCallBack.TABLE_NAME_NODE
                                    , null);
                            log("storageDataList size = " + storageDataList.size());
                            log("nodeDataList size = " + nodeDataList.size());
                            if(storageDataList.size() == 0 && nodeDataList.size() == 0) {
                                log("delete data success");
                                StorageData data = new StorageData();
                                data.setId(ProvisionedDeviceEntry.STORAGE_ID);
                                LouSQLite.insert(MyCallBack.TABLE_NAME_STORAGE, data);
                                return;
                            }else {
                                log("delete data fail");
                            }
                        }
                    }
                } else {
                    log("mAllNetKey.size() = 0");
                }
            }
            MyApplication.nodeList.clear();
            for (int i = 0; i < nodeDataList.size(); i++) {
                MyApplication.nodeList.add(nodeDataList.get(i).getNode());
            }
            for (int i = 0; i < MyApplication.nodeList.size(); i++) {
                Node node = MyApplication.nodeList.get(i);
                node.setActiveStatus(2);    //always set to unknown by default,, it shall be updated by heartbeat event
                node.setCurrentHeartBeatNumber(0);
                node.setPreHeartBeatNumber(0);
                node.setContinueLost(0);
                node.setMaxLost(0);
                node.setHeartBeatTime(0);
                node.setHeartBeatTimerNumber(0);
            }
        }
    }


    @Override
    protected void onResume() {
        super.onResume();
        log("onResume");
        isForeground = true;
        if (mProvisionerService != null) {
            if (false == mProvisionerService.isMeshAvailable()) {
                mSlideSwitch.setSlideable(false);
                Toast.makeText(MainActivity.this, "MeshDev APP is running, please kill it first", Toast.LENGTH_SHORT).show();
                return;
            }
            mSlideSwitch.setSlideable(true);
        }
        //Check if Bluetooth is turned on. If it is not open, pop up a prompt box and request to open Bluetooth.
        promptBtDialogIfNeeded();
    }

    @Override
    protected void onPause() {
        // TODO Auto-generated method stub
        super.onPause();
        log("onPause");
        isForeground = false;
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        log("onSaveInstanceState");
        outState.putBoolean("isOpen",isOpenMeshSwitch);
    }


    @Override
    protected void onStop() {
        // TODO Auto-generated method stub
        super.onStop();
        log("onStop");
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        // TODO Auto-generated method stub
        super.onConfigurationChanged(newConfig);
        log("onConfigurationChanged");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        log("onDestroy");
        if(null != mProvisionerService && mProvisionerService.isEnable()){
            mProvisionerService.setBluetoothMeshEnabled(false);
            mProvisionerService.setBluetoothMeshListener(null);
        }
        unbindService(mServiceConnection);
        stopService(serviceIntent);
        unregisterReceiver(mBluetoothStateChangeBroadCast);
        mProvisionerService = null;
        //if(null != mHandler){
            //mHandler.removeCallbacksAndMessages(null);
            //mHandler = null;
        //}
    }

    private void getOverflowMenu() {
        try {
            ViewConfiguration config = ViewConfiguration.get(this);
            Field menuKeyField = ViewConfiguration.class
                    .getDeclaredField("sHasPermanentMenuKey");
            if (menuKeyField != null) {
                menuKeyField.setAccessible(true);
                menuKeyField.setBoolean(config, false);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    public boolean onPrepareOptionsMenu(Menu menu) {
        // TODO Auto-generated method stub
        menu.findItem(R.id.addNetkey).setEnabled(isOpenMeshSwitch);
        menu.findItem(R.id.addAppkey).setEnabled(isOpenMeshSwitch);
        menu.findItem(R.id.keyRefresh).setEnabled(isOpenMeshSwitch);
        menu.findItem(R.id.startScan).setEnabled(isOpenMeshSwitch && !isScanning && (unProvisionedAdapter.getPosition() < 0 && nodeAdapter.getPosition() < 0));
        menu.findItem(R.id.stopScan).setEnabled(isOpenMeshSwitch && isScanning);
        menu.findItem(R.id.meshMode).setEnabled(isOpenMeshSwitch);
        menu.findItem(R.id.meshMode_on).setEnabled(isOpenMeshSwitch && (mProvisionerService.getMeshMode() != MeshConstants.MESH_MODE_ON));
        menu.findItem(R.id.meshMode_off).setEnabled(isOpenMeshSwitch && (mProvisionerService.getMeshMode() != MeshConstants.MESH_MODE_OFF));
        menu.findItem(R.id.meshMode_standby).setEnabled(isOpenMeshSwitch && (mProvisionerService.getMeshMode() != MeshConstants.MESH_MODE_STANDBY));
        menu.findItem(R.id.dump).setEnabled(isOpenMeshSwitch);
        menu.findItem(R.id.ota_op).setEnabled(isOpenMeshSwitch);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case R.id.addNetkey:
            int result = mProvisionerService.addNewNetKey();
            if(result == 0) {
                Toast.makeText(MainActivity.this,"add netkey success",Toast.LENGTH_LONG).show();
            }else {
                Toast.makeText(MainActivity.this,"add netkey fail",Toast.LENGTH_LONG).show();
            }
            break;
        case R.id.addAppkey:
            showAddAppKeyDialog();
            break;
        case R.id.keyRefresh:
            showKeyRefreshDialog();
            break;
        case R.id.startScan:
            unProvisionedAdapter.clear();
            mProgressBar.setVisibility(View.VISIBLE);
            mProvisionerService.startUnProvsionScan();
            isScanning = true;
            break;
        case R.id.stopScan:
            mProgressBar.setVisibility(View.GONE);
            mProvisionerService.stopUnProvsionScan();
            isScanning = false;
            break;
        case R.id.meshMode_on:
            if(mProvisionerService.setMeshMode(MeshConstants.MESH_MODE_ON)) {
                Toast.makeText(this, "set mesh mode on success", Toast.LENGTH_SHORT).show();
            }else {
                Toast.makeText(this, "set mesh mode on fail", Toast.LENGTH_SHORT).show();
            }
            break;
        case R.id.meshMode_off:
            if(mProvisionerService.setMeshMode(MeshConstants.MESH_MODE_OFF)) {
                Toast.makeText(this, "set mesh mode off success", Toast.LENGTH_SHORT).show();
            }else {
                Toast.makeText(this, "set mesh mode off fail", Toast.LENGTH_SHORT).show();
            }
            break;
        case R.id.meshMode_standby:
            if(mProvisionerService.setMeshMode(MeshConstants.MESH_MODE_STANDBY)) {
                Toast.makeText(this, "set mesh mode standby success", Toast.LENGTH_SHORT).show();
            }else {
                Toast.makeText(this, "set mesh mode standby fail", Toast.LENGTH_SHORT).show();
            }
            break;
        case R.id.dump:
            mProvisionerService.setDump();
            break;
        case R.id.home:
            moveTaskToBack(true);
            break;
        case R.id.ota_op_start:
            showOtaOpStartDialog();
            break;
        case R.id.ota_op_stop:
            mProvisionerService.otaStop();
            break;
        case R.id.ota_op_apply:
            mProvisionerService.otaApply();
            break;
        default:
            break;
        }
        return true;
    }

    @Override
    public void onCreateContextMenu(ContextMenu menu, View v, ContextMenu.ContextMenuInfo menuInfo) {
        super.onCreateContextMenu(menu, v, menuInfo);
        if(unProvisionedAdapter.getPosition() < 0 && nodeAdapter.getPosition() < 0) {
            menu.add(0,0,0,"Node Reset");
            menu.add(0,1,0,"All Node Reset");
            menu.add(0,2,0,"Remove");
        }
    }

    @Override
    public boolean onContextItemSelected(MenuItem item) {
        log("select position = " + nodePosition + " , item id = " + item.getItemId());
        switch (item.getItemId()) {
            case 0: // Node Reset
                adbGroupNodeReset(SINGLE_NODE_RESET);
                break;
            case 1: // All Node Reset
                adbGroupNodeReset(ALL_NODE_RESET);
                break;
            case 2: // Remove
                dialogKeyrefresh = new Dialog(this, R.style.Custom_Progress);
                dialogKeyrefresh.setContentView(R.layout.mul_picture_progressbar);
                dialogKeyrefresh.getWindow().getAttributes().gravity = Gravity.CENTER;
                dialogKeyrefresh.setCanceledOnTouchOutside(false);
                dialogKeyrefresh.show();
                Node nodeRemove = (MyApplication.nodeList).get(nodePosition);
                mProvisionerService.removeNode(nodeRemove, false);
                nodeAdapter.removeNode(nodeRemove);
                break;
            default:
                break;
        }
        return super.onContextItemSelected(item);

    }

    private void showAddAppKeyDialog() {
        ArrayList<Integer> netkeyIndexDatas = new ArrayList<Integer>();
        for(int i = 0;i < mProvisionerService.getNetKeyCnt(); i++){
            netkeyIndexDatas.add(i);
        }
        ArrayAdapter<Integer> appkeyAdapter = new ArrayAdapter<Integer>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,netkeyIndexDatas);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        LayoutInflater inflater = getLayoutInflater();
        View dialoglayout = inflater.inflate(R.layout.add_app_key, null);
        builder.setView(dialoglayout);
        builder.setCancelable(false);
        Spinner spinner = (Spinner) dialoglayout.findViewById(R.id.netkeyIndexSpinner);
        spinner.setAdapter(appkeyAdapter);
        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                addappkey_netkeyIndex = netkeyIndexDatas.get(position);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface arg0, int arg1) {
                int result = mProvisionerService.addNewAppKey(addappkey_netkeyIndex);
                if(result == 0) {
                    Toast.makeText(MainActivity.this,"add appkey success",Toast.LENGTH_LONG).show();
                }else {
                    Toast.makeText(MainActivity.this,"add appkey fail",Toast.LENGTH_LONG).show();
                }
            }
        });

        builder.setNegativeButton("Cancel", null);

        builder.create().show();
    }


    private void showKeyRefreshDialog() {
        log("showKeyRefreshDialog");
        //ArrayList<Integer> netkeyIndexDatas = new ArrayList<Integer>();
        List<String> netkeyIndexDatas = new ArrayList<String>();
        for(int i = 0;i < mProvisionerService.getNetKeyCnt(); i++){
            netkeyIndexDatas.add(MeshUtils.decimalToHexString("%04X",i));
        }
        HashMap<Integer,Node> mapNodes = mProvisionerService.getNodesByNetKey(keyfresh_netkeyIndex);
        log("mapNodes.size() =" + mapNodes.size());
        nodes.clear();
        for(Map.Entry<Integer,Node> entry:mapNodes.entrySet()) {
            nodes.add(entry.getValue());
        }
        log("nodes.size() =" + nodes.size());
        final CheckBoxAdapter cbAdapter = new CheckBoxAdapter(MyApplication.getApplication(),nodes);
        ArrayAdapter<String> netkeyAdapter = new ArrayAdapter<String>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,netkeyIndexDatas);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        LayoutInflater inflater = getLayoutInflater();
        View dialoglayout = inflater.inflate(R.layout.key_refresh, null);
        builder.setView(dialoglayout);
        builder.setCancelable(false);
        Spinner spinner = (Spinner) dialoglayout.findViewById(R.id.netkeyIndexSpinner);
        final ListView cbListView = (ListView) dialoglayout.findViewById(R.id.nodelist);
        spinner.setAdapter(netkeyAdapter);
        cbListView.setDivider(null);
        cbListView.setAdapter(cbAdapter);
        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                log("showKeyRefreshDialog onItemSelected");
                keyfresh_netkeyIndex = (int)MeshUtils.hexSrtingToDecimal(netkeyIndexDatas.get(position));
                HashMap<Integer,Node> mapNodes = mProvisionerService.getNodesByNetKey(keyfresh_netkeyIndex);
                log("mapNodes.size() =" + mapNodes.size());
                nodes.clear();
                for(Map.Entry<Integer,Node> entry:mapNodes.entrySet()) {
                    nodes.add(entry.getValue());
                }
                log("nodes.size() =" + nodes.size());
                cbAdapter.nodeChange(nodes);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                log("showKeyRefreshDialog onNothingSelected");

            }
        });
        cbListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                    int position, long id) {
                CheckBoxAdapter.ViewHolder viewHolder = (CheckBoxAdapter.ViewHolder) view.getTag();
                viewHolder.checkBox.toggle();
                CheckBoxAdapter.state.put(position, viewHolder.checkBox.isChecked());
            }
        });

        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface arg0, int arg1) {
                ArrayList<Node> nodelist = new ArrayList<>();
                Iterator<Integer> iterator = CheckBoxAdapter.state.keySet().iterator();
                while (iterator.hasNext()) {
                    Integer next = iterator.next();
                    Boolean able = CheckBoxAdapter.state.get(next);
                    if (able) {
                        nodelist.add(nodes.get(next));
                    }
                }
                keyrefresh(keyfresh_netkeyIndex, nodelist);
                /*
                log("nodelist.size() = " + nodelist.size());
                dialogKeyrefresh = new Dialog(MainActivity.this, R.style.Custom_Progress);
                dialogKeyrefresh.setContentView(R.layout.mul_picture_progressbar);
                dialogKeyrefresh.getWindow().getAttributes().gravity = Gravity.CENTER;
                dialogKeyrefresh.setCanceledOnTouchOutside(false);
                dialogKeyrefresh.show();
                log("dialogKeyrefresh = " + dialogKeyrefresh);
                if(nodelist.size() > 0){
                    mProvisionerService.keyRefreshStart(keyfresh_netkeyIndex,nodelist);
                } else {
                    mProvisionerService.keyRefreshStart(keyfresh_netkeyIndex, null);
                }
                */
            }
        });

        builder.setNegativeButton("Cancel", null);

        builder.create().show();

    }

    private void showOtaOpStartDialog() {
        log("showOtaOpStartDialog");
        ArrayList<Integer> netkeyIndexDatas = new ArrayList<Integer>();
        ArrayList<Integer> appkeyIndexDatas = new ArrayList<Integer>();
        for(int i = 0;i < mProvisionerService.getNetKeyCnt(); i++){
            netkeyIndexDatas.add(i);
        }
        for(int i = 0;i < mProvisionerService.getAppKeyCnt(); i++){
            appkeyIndexDatas.add(i);
        }
        HashMap<Integer,Node> mapNodes = mProvisionerService.getNodesByNetKey(ota_op_netkeyIndex);
        log("mapNodes.size() =" + mapNodes.size());
        nodes.clear();
        for(Map.Entry<Integer,Node> entry:mapNodes.entrySet()) {
            nodes.add(entry.getValue());
        }
        log("nodes.size() =" + nodes.size());
        final CheckBoxAdapter cbAdapter = new CheckBoxAdapter(MyApplication.getApplication(),nodes);
        ArrayAdapter<Integer> netkeyAdapter = new ArrayAdapter<Integer>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,netkeyIndexDatas);
        ArrayAdapter<Integer> appkeyAdapter = new ArrayAdapter<Integer>(MyApplication.getApplication(),android.R.layout.simple_spinner_dropdown_item,appkeyIndexDatas);
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        LayoutInflater inflater = getLayoutInflater();
        View dialoglayout = inflater.inflate(R.layout.ota_op_start, null);
        builder.setView(dialoglayout);
        builder.setCancelable(false);
        Spinner netKeyindexSpinner = (Spinner) dialoglayout.findViewById(R.id.netkeyIndexSpinner);
        Spinner appKeyindexSpinner = (Spinner) dialoglayout.findViewById(R.id.appkeyIndexSpinner);
        final ListView cbListView = (ListView) dialoglayout.findViewById(R.id.nodelist);
        netKeyindexSpinner.setAdapter(netkeyAdapter);
        appKeyindexSpinner.setAdapter(appkeyAdapter);
        cbListView.setDivider(null);
        cbListView.setAdapter(cbAdapter);
        netKeyindexSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                log("showOtaOpStartDialog netKeyindexSpinner onItemSelected");
                ota_op_netkeyIndex = netkeyIndexDatas.get(position);
                HashMap<Integer,Node> mapNodes = mProvisionerService.getNodesByNetKey(ota_op_netkeyIndex);
                log("mapNodes.size() =" + mapNodes.size());
                nodes.clear();
                for(Map.Entry<Integer,Node> entry:mapNodes.entrySet()) {
                    nodes.add(entry.getValue());
                }
                log("nodes.size() =" + nodes.size());
                cbAdapter.nodeChange(nodes);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                log("showOtaOpStartDialog onNothingSelected onNothingSelected");

            }
        });
        appKeyindexSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                log("showOtaOpStartDialog appKeyindexSpinner onItemSelected");
                ota_op_appkeyIndex = appkeyIndexDatas.get(position);
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                log("showOtaOpStartDialog appKeyindexSpinner onNothingSelected");

            }
        });

        cbListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                    int position, long id) {
                CheckBoxAdapter.ViewHolder viewHolder = (CheckBoxAdapter.ViewHolder) view.getTag();
                viewHolder.checkBox.toggle();
                CheckBoxAdapter.state.put(position, viewHolder.checkBox.isChecked());
            }
        });

        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {

            @Override
            public void onClick(DialogInterface arg0, int arg1) {
                ArrayList<Node> nodelist = new ArrayList<>();
                Iterator<Integer> iterator = CheckBoxAdapter.state.keySet().iterator();
                while (iterator.hasNext()) {
                    Integer next = iterator.next();
                    Boolean able = CheckBoxAdapter.state.get(next);
                    if (able) {
                        nodelist.add(nodes.get(next));
                        log("checked node = " + nodes.get(next).getAddr());
                    }
                }
                log("nodelist.size() = " + nodelist.size());
                if(nodelist.size() > 0){
                    mProvisionerService.otaStart(ota_op_netkeyIndex, nodelist, ota_op_appkeyIndex);
                } else {
                    //do something
                }
            }
        });

        builder.setNegativeButton("Cancel", null);

        builder.create().show();

    }

    @Override
    public void open() {
        log("open mesh");
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mProvisionerService.setEnabled(llState,false);
                mSlideSwitch.setSlideable(false);
                llState.setClickable(false);
                mProvisionerService.setBluetoothMeshEnabled(true);
                slideText.setText(getResources().getString(R.string.switch_open));
                mProgressBar.setVisibility(View.VISIBLE);
                closeDescription.setText(R.string.turning_on);
                //isOpenMeshSwitch = true;
                //isScanning = true;
            }
        });
    }

    @Override
    public void close() {
        log("close mesh");
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mProvisionerService.setBluetoothMeshEnabled(false);
                slideText.setText(getResources().getString(R.string.switch_close));
                llClose.setVisibility(View.VISIBLE);
                llOpen.setVisibility(View.GONE);
                unProvisionedAdapter.setPosition(UnprovisionedAdapter.ALL_ENABLE,true);
                nodeAdapter.setPosition(NodeAdapter.ALL_ENABLE);
                unProvisionedAdapter.clear();
                isOpenMeshSwitch = false;
                isScanning = false;
                printfGroupProvResult();
                //if(null != mRunnable){
                    //mHandler.removeCallbacks(mRunnable);
                //}
                Toast.makeText(MainActivity.this, "mesh off", Toast.LENGTH_SHORT).show();
            }
        });
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.llstate:
                if(isOpenMeshSwitch) {
                    mSlideSwitch.setSwitchState(false);
                }else {
                    mSlideSwitch.setSwitchState(true);
                }
                break;
            default:
                break;
        }

    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        switch (parent.getId()) {
            case R.id.unprovisioned:
                if(unProvisionedAdapter.getPosition() < 0 ) {
                    UnProvisionedDevice mUnProvisionedDevice = unProvisionedAdapter.getUnprovisionedDevice(position);
                    if (mUnProvisionedDevice == null) {
                        log("onItemClick , not fonund unProvisionedDevice , position = " + position);
                        return;
                    }
                    int netkeyIndexSize = mProvisionerService.getNetKeyCnt();
                    int appkeyIndexSize = mProvisionerService.getAppKeyCnt();
                    showUnprovisionedFragment(position,mUnProvisionedDevice,netkeyIndexSize,appkeyIndexSize);
                }
               break;
            case R.id.node:
                ArrayList<MeshMessage> messages = mProvisionerService.getSupportedMessages((MyApplication.nodeList).get(position));
                ArrayList<Integer> groupAddrDatas = mProvisionerService.getGroupAddrList();
                showProvisionedDialogFragment(position,messages,groupAddrDatas);
                break;
            default:
                break;
        }
    }

    @Override
    public void onRecyclerViewItemClick(View view,int position) {
        log("onRecyclerViewItemClick , position = " + position);
        if(view.getId() == nodeAdapter.getId()) {
            if(unProvisionedAdapter.getPosition() < 0 && nodeAdapter.getPosition() < 0 ) {
                ArrayList<MeshMessage> messages = mProvisionerService.getSupportedMessages((MyApplication.nodeList).get(position));
                ArrayList<Integer> groupAddrDatas = mProvisionerService.getGroupAddrList();
                MyApplication.netkeyindex = mProvisionerService.getNetKeyCnt();
                MyApplication.appkeyindex = mProvisionerService.getAppKeyCnt();
                showProvisionedDialogFragment(position,messages,groupAddrDatas);
            }
        }else if(view.getId() == unProvisionedAdapter.getId()) {
            if(unProvisionedAdapter.getPosition() < 0 && nodeAdapter.getPosition() < 0) {
                UnProvisionedDevice mUnProvisionedDevice = unProvisionedAdapter.getUnprovisionedDevice(position);
                if (mUnProvisionedDevice == null) {
                    log("onRecyclerViewItemClick , not fonund unProvisionedDevice , position = " + position);
                    return;
                }
                int netkeyIndexSize = mProvisionerService.getNetKeyCnt();
                int appkeyIndexSize = mProvisionerService.getAppKeyCnt();
                showUnprovisionedFragment(position,mUnProvisionedDevice,netkeyIndexSize,appkeyIndexSize);
            }
        }
    }

    @Override
    public void onRecyclerViewItemLongClick(View view,int position) {
        nodePosition = position;
    }

    private void showProvisionedDialogFragment(int position,ArrayList<MeshMessage> messages,ArrayList<Integer> groupAddrDatas) {
        FragmentTransaction mFragTransaction = getFragmentManager().beginTransaction();
        //Check whether there is a Fragment corresponding to the tag through the tag
        Fragment fragment =  getFragmentManager().findFragmentByTag("ProvisionedDialogFragment");
        if(fragment!=null){
            //In order not to display DialogFragment repeatedly, remove the DialogFragment that is being displayed before displaying the DialogFragment
            mFragTransaction.remove(fragment);
        }
        mProvisionedDialogFragment = ProvisionedDialogFragment.newInstance(position,messages,groupAddrDatas);
        //Display a Fragment and add a tag to the Fragment. The fragment can be found by findFragmentByTag
        mProvisionedDialogFragment.show(mFragTransaction, "ProvisionedDialogFragment");
        mProvisionedDialogFragment.setBluetoothMeshListener(mBluetoothMeshListener);
    }

    private void showUnprovisionedFragment(int position,UnProvisionedDevice unProvisionedDevice,int netkeyIndexSize,int appkeyIndexSize) {
        FragmentTransaction mFragTransaction = getFragmentManager().beginTransaction();
        //Check whether there is a Fragment corresponding to the tag through the tag
        Fragment fragment =  getFragmentManager().findFragmentByTag("UnprovisionedDialogFragment");
        if(fragment!=null){
            //In order not to display DialogFragment repeatedly, remove the DialogFragment that is being displayed before displaying the DialogFragment
            mFragTransaction.remove(fragment);
        }
        UnprovisionedDialogFragment dialogFragment = UnprovisionedDialogFragment.newInstance(position,unProvisionedDevice,netkeyIndexSize,appkeyIndexSize);
        //Display a Fragment and add a tag to the Fragment. The fragment can be found by findFragmentByTag
        dialogFragment.show(mFragTransaction, "UnprovisionedDialogFragment");
        dialogFragment.setBluetoothMeshListener(mBluetoothMeshListener);
    }

    /**
     * ACCESS_FINE_LOCATION is a dangerous permission
     * so needs to check whether the ACCESS_FINE_LOCATION permission is authorized at runtime
     */
    public static boolean checkLocationPermission(Activity activity) {
        if (ContextCompat.checkSelfPermission(activity, Manifest.permission.ACCESS_FINE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(activity,
                    new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, REQUEST_ENABLE_LOCATION);
            return false;
        } else {
            return true;
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (REQUEST_ENABLE_LOCATION == requestCode) {
            if (grantResults == null || grantResults.length == 0) {
                log("grantResults == null || grantResults.length == 0");
                return;
            }
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                invalidateOptionsMenu();
            } else {
                Toast.makeText(this, "Location permission is not granted.", Toast.LENGTH_SHORT).show();
            }
        }
    }

    private void promptBtDialogIfNeeded() {
        // Ensures Bluetooth is enabled on the device. If Bluetooth is not
        // currently enabled,
        // fire an intent to display a dialog asking the user to grant
        // permission to enable it.
        log("promptBtDialogIfNeeded");
        if (BluetoothAdapter.STATE_OFF == mBluetoothAdapter.getState()
                || BluetoothAdapter.STATE_TURNING_OFF == mBluetoothAdapter.getState()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
        }
    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        log("onActivityResult");
        // User chose not to enable Bluetooth.
        if (requestCode == REQUEST_ENABLE_BT && resultCode == Activity.RESULT_CANCELED) {
            log("deny");
            finish();
            return;
        }
        if(isOpenMeshSwitch) {
            log("allow");
            mProvisionerService.setBluetoothMeshEnabled(false);
            unProvisionedAdapter.setPosition(UnprovisionedAdapter.ALL_ENABLE,true);
            nodeAdapter.setPosition(NodeAdapter.ALL_ENABLE);
            unProvisionedAdapter.clear();
            mProvisionerService.setBluetoothMeshEnabled(true);
            //isScanning = true;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }


    private void inviteProvisioningTimeout(){
        mRunnable = new Runnable(){
            @Override
            public void run(){
                if(MyApplication.isProvisioning){
                    unProvisionedAdapter.setPosition(UnprovisionedAdapter.ALL_ENABLE);
                    //Toast.makeText(MainActivity.this, "provisioning timeout", Toast.LENGTH_SHORT).show();
                }
                isScanning = false;
            }

        };
        //mHandler.postDelayed(mRunnable,60*1000);
    }

    class BluetoothStateChangeBroadCast extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {

            switch(intent.getAction()){
                case BluetoothAdapter.ACTION_STATE_CHANGED:
                    int blueState = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, 0);
                    switch(blueState){
                        case BluetoothAdapter.STATE_OFF:
                            if(isForeground) {
                                promptBtDialogIfNeeded();
                            }else if(mProvisionerService.isEnable()){
                                mSlideSwitch.setSwitchState(false);
                            }
                            break;
                    }
                    break;
                case "android.action.PB_ADV_SEND_MSG":
                    adbSendMsg(intent, false);
                    break;
                case "android.action.PB_GATT_SEND_MSG":
                    adbSendMsg(intent, true);
                    break;
                case "android.action.MESH_GROUP_SEND_MSG":
                    adbGroupSendMsg(intent);
                    break;
                case "android.action.MESH_ON":
                    if(isOpenMeshSwitch) {
                        Toast.makeText(MainActivity.this, "mesh already on", Toast.LENGTH_SHORT).show();
                        break;
                    }
                    mSlideSwitch.setSwitchState(true);
                    break;
                case "android.action.MESH_OFF":
                    if(!isOpenMeshSwitch) {
                        Toast.makeText(MainActivity.this, "mesh already off", Toast.LENGTH_SHORT).show();
                        break;
                    }
                    mSlideSwitch.setSwitchState(false);
                    break;
                case "android.action.PB_GATT_CONNECT":
                    adbGattConnectOrDisConnect(intent, true);
                    break;
                case "android.action.PB_GATT_DISCONNECT":
                    adbGattConnectOrDisConnect(intent, false);
                    break;
                case "android.action.MESH_ADV_PROVISION":
                    adbProvision(intent, false);
                    break;
                case "android.action.MESH_GATT_PROVISION":
                    adbProvision(intent, true);
                    break;
                case "android.action.MESH_ADV_REMOVE":
                    adbRemoveNode(intent, false);
                    break;
                case "android.action.MESH_GATT_REMOVE":
                    adbRemoveNode(intent, true);
                    break;
                case "android.action.MESH_SCAN":
                    log("android.action.MESH_SCAN");
                    if(!isOpenMeshSwitch) {
                        Toast.makeText(MainActivity.this, "mesh is off, please open mesh", Toast.LENGTH_SHORT).show();
                        log("mesh is off, please open mesh");
                        break;
                    }
                    if(!(unProvisionedAdapter.getPosition() < 0 && nodeAdapter.getPosition() < 0)) {
                        Toast.makeText(MainActivity.this, "Provisioning or Configing , Please try again scan later", Toast.LENGTH_SHORT).show();
                        log("provisioning or configing , Please try again scan later");
                        break;
                    }
                    if(isScanning) {
                        mProvisionerService.stopUnProvsionScan();
                    }
                    unProvisionedAdapter.clear();
                    mProgressBar.setVisibility(View.VISIBLE);
                    mProvisionerService.startUnProvsionScan();
                    isScanning = true;
                    break;
                case "android.action.MESH_DUMP":
                    log("android.action.MESH_DUMP");
                    mProvisionerService.setDump();
                    break;
                case "android.action.MESH_GROUP_PROVISION":
                    log("android.action.MESH_GROUP_PROVISION");
                    adbGroupProvision();
                    break;
                case "android.action.MESH_GROUP_NODE_RESET":
                    log("android.action.MESH_GROUP_NODE_RESET");
                    adbGroupNodeReset(PARTIAL_NODE_RESET);
                    break;
                default :
                    break;
            }
        }
    }

    private void adbSendMsg(Intent intent, boolean PB_GATT) {
        log("adbSendMsg , PB-GATT : " + PB_GATT);
        if(!isOpenMeshSwitch) {
            log("mesh is off");
            return;
        }
        if(!(unProvisionedAdapter.getPosition() < 0 && nodeAdapter.getPosition() < 0)) {
            Toast.makeText(MainActivity.this, "MeshProv is Provisioning or Configing , please try again later", Toast.LENGTH_SHORT).show();
            log("MeshProv is Provisioning or Configing , please try again later");
            return;
        }
        String mGattAddr = intent.getStringExtra("gattAddr");
        String uuidStr = intent.getStringExtra("uuid");
        String appkeyIndexStr = intent.getStringExtra("appkeyIndex");
        String groupAddrStr = intent.getStringExtra("groupAddr");
        String on_offStr = intent.getStringExtra("on_off");
        String opCodeStr = intent.getStringExtra("opCode");
        String elementAddrStr = intent.getStringExtra("elementAddr");

        int[] uuid = new int[16];
        String gattAddress = null;
        int appkeyIndex = 0; // appkeyindex default is 0
        int groupAddr = 0; // groupAddr default is 0
        int elementAddr = -1;
        Node node = null;
        if(on_offStr == null) {
            Toast.makeText(MainActivity.this, "not set on/off", Toast.LENGTH_SHORT).show();
            log("not set on/off");
            return;
        }
        if(PB_GATT) {
            if(mGattAddr == null) { // not set PB-GATT address
                Toast.makeText(MainActivity.this, "not set PB-GATT ddress", Toast.LENGTH_SHORT).show();
                log("not set PB-GATT ddress");
                return;
            }else{ // set PB-GATT address
                if(MyApplication.nodeList.size() <= 0) {
                    log("node list is null");
                    return;
                }else{
                    int i = 0;
                    for(i = 0; i < MyApplication.nodeList.size(); i++) {
                        if(MyApplication.nodeList.get(i).getNodeBearer() == MeshConstants.MESH_BEARER_GATT) {
                            log("node Gatt address = " + MyApplication.nodeList.get(i).getGattAddr());
                            log("input gatt address = " + mGattAddr);
                            if(mGattAddr.equals(MyApplication.nodeList.get(i).getGattAddr())) {
                                gattAddress = MyApplication.nodeList.get(i).getGattAddr();
                                node = MyApplication.nodeList.get(i);
                                elementAddr = MyApplication.nodeList.get(i).getAddr();
                                break;
                            }
                        }
                    }
                    if(i == MyApplication.nodeList.size()) {
                        log("not find match node in node list");
                        return;
                    }
                }
            }
        }else{
            if(uuidStr == null) { // not set PB-ADV uuid
                Toast.makeText(MainActivity.this, "not set PB-ADV UUID", Toast.LENGTH_SHORT).show();
                log("not set PB-ADV UUID");
                return;
            }else { // set PB-ADV uuid
                if(MyApplication.nodeList.size() <= 0) {
                    log("node list is null");
                    return;
                }else {
                    int i = 0;
                    for(i = 0; i < MyApplication.nodeList.size(); i++) {
                        if(MyApplication.nodeList.get(i).getNodeBearer() == MeshConstants.MESH_BEARER_ADV) {
                            log("node uuid = " + MeshUtils.intArrayToString(MyApplication.nodeList.get(i).getUUID(), true));
                            log("input uuid = " + MeshUtils.intArrayToString(MeshUtils.StringToIntArray(uuidStr), true));
                            if(Arrays.equals(MyApplication.nodeList.get(i).getUUID(), MeshUtils.StringToIntArray(uuidStr))) {
                                uuid = MyApplication.nodeList.get(i).getUUID();
                                node = MyApplication.nodeList.get(i);
                                elementAddr = MyApplication.nodeList.get(i).getAddr();
                                break;
                            }
                        }
                    }
                    if(i == MyApplication.nodeList.size()) {
                        log("not find match node in node list");
                        return;
                    }
                }
            }

        }
        if(node == null) {
            log("node == null");
            return;
        }
        if(appkeyIndexStr == null) { // not set appkeyIndex
            log("not set appkeyIndex, appkeyIndex default value is 0");
        }else { // set appkeyIndex
            appkeyIndex = (int)(MeshUtils.hexSrtingToDecimal(appkeyIndexStr));
            log("set appkeyIndex, appkeyIndex value is " + appkeyIndex);
        }
        if(groupAddrStr == null) { // not set groupAddr
            log("not set groupAddr, groupAddr default value is 0");
        }else{ // not set groupAddr
            groupAddr = (int)(MeshUtils.hexSrtingToDecimal(groupAddrStr));
            log("set groupAddr, groupAddr value is " + groupAddr);
        }
        if(mProvisionerService.getNodes().size() <= 0) {
            log("ProversionService mNodes size is 0");
            return;
        }
        ArrayList<MeshMessage> messages = mProvisionerService.getSupportedMessages(node);
        MeshMessage mMeshMessage = null;
        int[] payload = new int[1];
        int opCode = MeshConstants.MESH_MSG_GENERIC_ONOFF_SET; //opCode default value is set
        for(int i = 0; i < messages.size(); i++) {
            if(opCodeStr != null) {
                switch(opCodeStr){
                    case "MESH_MSG_GENERIC_ONOFF_GET":
                        opCode = MeshConstants.MESH_MSG_GENERIC_ONOFF_GET;
                        log("opCode = MESH_MSG_GENERIC_ONOFF_GET");
                        break;
                    case "MESH_MSG_GENERIC_ONOFF_SET":
                        opCode = MeshConstants.MESH_MSG_GENERIC_ONOFF_SET;
                        log("opCode = MESH_MSG_GENERIC_ONOFF_SET");
                        break;
                    case "MESH_MSG_GENERIC_ONOFF_SET_UNRELIABLE":
                        opCode = MeshConstants.MESH_MSG_GENERIC_ONOFF_SET_UNRELIABLE;
                        log("opCode = MESH_MSG_GENERIC_ONOFF_SET_UNRELIABLE");
                        break;
                    default:
                        break;
                }
            }
            MeshMessage message = messages.get(i);
            if(opCode == message.getOpCode() && elementAddr == message.getElementAddr()) {
                mMeshMessage = message;
                break;
            }
        }
        if(mMeshMessage == null) {
            return;
        }
        if(on_offStr.equals("on")) {
            payload[0] = 1;
        }else if(on_offStr.equals("off")) {
            payload[0] = 0;
        }else if(on_offStr.equals("NA")){
            if(opCode == MeshConstants.MESH_MSG_GENERIC_ONOFF_GET) {
                payload = null;
            }else {
                log("opCode not match with on_off");
                return;
            }
        }else {
            log("on_offStr is invalid value");
            return;
        }
        if(mProvisionerService != null) {
            mProvisionerService.sendMessage(mMeshMessage,node,appkeyIndex,groupAddr,payload);
        }
    }

    private void adbGroupSendMsg(Intent intent) {
        log("adbGroupSendMsg");
        if(!isOpenMeshSwitch) {
            log("mesh is off");
            return;
        }
        if(!(unProvisionedAdapter.getPosition() < 0 && nodeAdapter.getPosition() < 0)) {
            Toast.makeText(MainActivity.this, "MeshProv is Provisioning or Configing , please try again later", Toast.LENGTH_SHORT).show();
            log("MeshProv is Provisioning or Configing , please try again later");
            return;
        }
        String groupAddrStr = intent.getStringExtra("groupAddr");
        String on_offStr = intent.getStringExtra("on_off");
        String opCodeStr = intent.getStringExtra("opCode");


        int appkeyIndex = 0; // appkeyindex default is 0
        int groupAddr = 0; // groupAddr default is 0
        Node node = null;
        if(mProvisionerService.getNodes().size() <= 0) {
            log("ProversionService mNodes size is 0");
            return;
        }
        node = MyApplication.nodeList.get(0);
        if (node == null) {
            log("node == null");
            return;
        }
        if(on_offStr == null) {
            Toast.makeText(MainActivity.this, "not set on/off", Toast.LENGTH_SHORT).show();
            log("not set on/off");
        }
        if(opCodeStr == null) {
            Toast.makeText(MainActivity.this, "not set opCode params", Toast.LENGTH_SHORT).show();
            log("not set opCode params");
        }
        if(groupAddrStr == null) { // not set groupAddr
            Toast.makeText(MainActivity.this, "not set groupAddr", Toast.LENGTH_SHORT).show();
            log("not set groupAddr");
            return;
        }else{ // not set groupAddr
            groupAddr = (int)(MeshUtils.hexSrtingToDecimal(groupAddrStr));
            if (groupAddr != 0xFFFF) {
                log("set groupAddr error, groupAddr value is " + groupAddr);
                return;
            }
        }
        int[] payload = new int[1];
        int opCode = MeshConstants.MESH_MSG_GENERIC_ONOFF_SET; //opCode default value is set
        switch(opCodeStr){
            case "MESH_MSG_GENERIC_ONOFF_GET":
                opCode = MeshConstants.MESH_MSG_GENERIC_ONOFF_GET;
                log("opCode = MESH_MSG_GENERIC_ONOFF_GET");
                break;
            case "MESH_MSG_GENERIC_ONOFF_SET":
                opCode = MeshConstants.MESH_MSG_GENERIC_ONOFF_SET;
                log("opCode = MESH_MSG_GENERIC_ONOFF_SET");
                break;
            case "MESH_MSG_GENERIC_ONOFF_SET_UNRELIABLE":
                opCode = MeshConstants.MESH_MSG_GENERIC_ONOFF_SET_UNRELIABLE;
                log("opCode = MESH_MSG_GENERIC_ONOFF_SET_UNRELIABLE");
                break;
            default:
                log("opCode params set error , it will use default opCode value");
                break;
        }
        if(on_offStr.equals("on")) {
            payload[0] = 1;
        }else if(on_offStr.equals("off")) {
            payload[0] = 0;
        }else if(on_offStr.equals("NA")){
            if(opCode == MeshConstants.MESH_MSG_GENERIC_ONOFF_GET) {
                payload = null;
            }else {
                log("opCode not match with on_off");
                return;
            }
        }else {
            log("on_offStr is invalid value");
            return;
        }
        MeshMessage mMeshMessage = new MeshMessage(opCode, node.getAddr(), null, null);
        if(mProvisionerService != null) {
            mProvisionerService.sendMessage(mMeshMessage,node,appkeyIndex,groupAddr,payload);
        }
    }

    private void adbProvision(Intent intent, boolean PB_GATT) {
        log("adbProvision , PB-GATT : " + PB_GATT);
        if(!isOpenMeshSwitch) {
            log("mesh is off");
            return;
        }
        if(!(unProvisionedAdapter.getPosition() < 0 && nodeAdapter.getPosition() < 0)) {
            Toast.makeText(MainActivity.this, "MeshProv is Provisioning or Configing , please try again later", Toast.LENGTH_SHORT).show();
            log("MeshProv is Provisioning or Configing , please try again later");
            return;
        }
        String mGattAddr = intent.getStringExtra("gattAddr");
        String uuidStr = intent.getStringExtra("uuid");
        log("mGattAddr = " + mGattAddr + "uuidStr = " + uuidStr);
        UnProvisionedDevice mUnProvisionedDevice = null;
        if(PB_GATT) {
            if(mGattAddr == null) {
                Toast.makeText(MainActivity.this, "not set PB-GATT ddress", Toast.LENGTH_SHORT).show();
                log("not set PB-GATT ddress");
                return;
            }else{
                mUnProvisionedDevice = unProvisionedAdapter.getUnprovisionedDeviceByGATTAddr(mGattAddr);
            }
        }else{
            if(uuidStr == null) {
                Toast.makeText(MainActivity.this, "not set PB-ADV UUID", Toast.LENGTH_SHORT).show();
                log("not set PB-ADV UUID");
                return;
            }else{
                mUnProvisionedDevice = unProvisionedAdapter.getUnprovisionedDeviceByUUID(MeshUtils.StringToIntArray(uuidStr));
            }
        }
        if(mUnProvisionedDevice == null) {
            Toast.makeText(MainActivity.this, "not found unprovisioned device", Toast.LENGTH_SHORT).show();
            log("not found unprovisioned device");
            return;
        }
        int position = unProvisionedAdapter.getPositionByUnProvisioned(mUnProvisionedDevice);
        if(position == -1) {
            log("position = -1");
            return;
        }
        unProvisionedAdapter.setPosition(position);
        if (isScanning) {
            mProvisionerService.stopUnProvsionScan();
            mProgressBar.setVisibility(View.GONE);
            isScanning = false;
        }
        mProvisionerService.inviteProvisioning(mUnProvisionedDevice.getBearerType(),mUnProvisionedDevice.getUUID(),
            mUnProvisionedDevice.getAddress(),
            mUnProvisionedDevice.getAddressType(),
            mUnProvisionedDevice.getGattDevName(),
            0,
            0);

    }

    private void adbRemoveNode(Intent intent, boolean PB_GATT) {
        log("adbRemoveNode , PB-GATT : " + PB_GATT);
        if(!isOpenMeshSwitch) {
            log("mesh is off");
            return;
        }
        if(!(unProvisionedAdapter.getPosition() < 0 && nodeAdapter.getPosition() < 0)) {
            Toast.makeText(MainActivity.this, "MeshProv is Provisioning or Configing , please try again later", Toast.LENGTH_SHORT).show();
            log("MeshProv is Provisioning or Configing , please try again later");
            return;
        }
        String mGattAddr = intent.getStringExtra("gattAddr");
        String uuidStr = intent.getStringExtra("uuid");
        Node node = null;
        if(PB_GATT) {
            if(mGattAddr == null) {
                Toast.makeText(MainActivity.this, "not set PB-GATT ddress", Toast.LENGTH_SHORT).show();
                log("not set PB-GATT ddress");
                return;
            }else{
                if(MyApplication.nodeList.size() <= 0) {
                    log("node list is null");
                    return;
                }else{
                    int i = 0;
                    for(i = 0; i < MyApplication.nodeList.size(); i++) {
                        if(MyApplication.nodeList.get(i).getNodeBearer() == MeshConstants.MESH_BEARER_GATT) {
                            log("node Gatt address = " + MyApplication.nodeList.get(i).getGattAddr());
                            log("input gatt address = " + mGattAddr);
                            if(mGattAddr.equals(MyApplication.nodeList.get(i).getGattAddr())) {
                                node = MyApplication.nodeList.get(i);
                                break;
                            }
                        }
                    }
                    if(i == MyApplication.nodeList.size()) {
                        log("not find match node in node list");
                        return;
                    }
                }
            }
        }else{
            if(uuidStr == null) { // not set PB-ADV uuid
                Toast.makeText(MainActivity.this, "not set PB-ADV UUID", Toast.LENGTH_SHORT).show();
                log("not set PB-ADV UUID");
                return;
            }else { // set PB-ADV uuid
                if(MyApplication.nodeList.size() <= 0) {
                    log("node list is null");
                    return;
                }else {
                    int i = 0;
                    for(i = 0; i < MyApplication.nodeList.size(); i++) {
                        if(MyApplication.nodeList.get(i).getNodeBearer() == MeshConstants.MESH_BEARER_ADV) {
                            log("node uuid = " + MeshUtils.intArrayToString(MyApplication.nodeList.get(i).getUUID(), true));
                            log("input uuid = " + MeshUtils.intArrayToString(MeshUtils.StringToIntArray(uuidStr), true));
                            if(Arrays.equals(MyApplication.nodeList.get(i).getUUID(), MeshUtils.StringToIntArray(uuidStr))) {
                                node = MyApplication.nodeList.get(i);
                                break;
                            }
                        }
                    }
                    if(i == MyApplication.nodeList.size()) {
                        log("not find match node in node list");
                        return;
                    }
                }
            }

        }
        if(node == null) {
            log("node == null");
            return;
        }
        dialogKeyrefresh = new Dialog(this, R.style.Custom_Progress);
        dialogKeyrefresh.setContentView(R.layout.mul_picture_progressbar);
        dialogKeyrefresh.getWindow().getAttributes().gravity = Gravity.CENTER;
        dialogKeyrefresh.setCanceledOnTouchOutside(false);
        dialogKeyrefresh.show();
        mProvisionerService.removeNode(node, true);
        nodeAdapter.removeNode(node);
    }

    private void adbGattConnectOrDisConnect(Intent intent, boolean connect) {
        log("adbGattConnectOrDisConnect connect: " + connect);
        if(!isOpenMeshSwitch) {
            log("mesh is off");
            return;
        }
        if(!(unProvisionedAdapter.getPosition() < 0 && nodeAdapter.getPosition() < 0)) {
            Toast.makeText(MainActivity.this, "MeshProv is Provisioning or Configing , please try again later", Toast.LENGTH_SHORT).show();
            log("MeshProv is Provisioning or Configing , please try again later");
            return;
        }
        String mgattAddress = intent.getStringExtra("gattAddr");
        Node nodeGatt = null;
        if(mgattAddress == null){
            log("PB-GATT not set gattAddr");
            return;
        }
        if(MyApplication.nodeList.size() <= 0) {
            log("node list is null");
            return;
        }else{
            int i = 0;
            for(i = 0; i < MyApplication.nodeList.size(); i++) {
                if(MyApplication.nodeList.get(i).getNodeBearer() == MeshConstants.MESH_BEARER_GATT) {
                    log("node Gatt address = " + MyApplication.nodeList.get(i).getGattAddr());
                    log("input gatt address = " + mgattAddress);
                    if(mgattAddress.equals(MyApplication.nodeList.get(i).getGattAddr())) {
                        nodeGatt = MyApplication.nodeList.get(i);
                        break;
                    }
                }
            }
            if(i == MyApplication.nodeList.size()) {
                log("not find match node in node list");
                return;
            }
        }
        if(nodeGatt == null) {
            log("nodeGatt == null");
            return;
        }
        if(nodeGatt.getNodeBearer() != MeshConstants.MESH_BEARER_GATT){
            log("PB-ADV");
            return;
        }
        if(connect) {
            mProvisionerService.GattConnect(nodeGatt);
        }else {
            mProvisionerService.GattDisconnect(nodeGatt);
        }
    }

    private void adbGroupProvision() {
        log("adbGroupProvision()");
        if (!isOpenMeshSwitch) {
            log("mesh is off");
            return;
        }
        deviceSize = mProvisionerService.uuidList.size();
        if (deviceSize == 0) {
            log("deviceSize == 0");
            return;
        }
        if (isScanning) {
            mProvisionerService.stopUnProvsionScan();
            mProgressBar.setVisibility(View.GONE);
            isScanning = false;
        }
        provision();
    }

    private void provision() {
        log("provision()");
        if (!isOpenMeshSwitch) {
            log("mesh is off");
            return;
        }
        for (int i = uuidIndex; i < deviceSize; i++) {
            uuidIndex++;
            uuid = MeshUtils.StringToIntArray(mProvisionerService.uuidList.get(i));
            log("uuid = " + MeshUtils.intArrayToString(uuid, true) + " , index = " + uuidIndex);
            if (uuid == null) { //mesh_properties.txt contain invalid uuid
                if (i == (deviceSize - 1)) { //last uuid device
                    printfGroupProvResult();
                    return;
                }
                continue;
            }
            UnProvisionedDevice mUnProvisionedDevice = unProvisionedAdapter.getUnprovisionedDeviceByUUID(uuid);
            if (mUnProvisionedDevice == null) { // uuid not found in unprovisioned device list
                uuidNoneExistent.add(uuid);
                if (i == (deviceSize - 1)) { // last uuid device
                    printfGroupProvResult();
                }
            } else {
                int position = unProvisionedAdapter.getPositionByUnProvisioned(mUnProvisionedDevice);
                if(position == -1) {
                    log("position = -1 , it not should happen");
                } else {
                    unProvisionedAdapter.setPosition(position);
                    mProvisionerService.inviteProvisioning(mUnProvisionedDevice.getBearerType(),mUnProvisionedDevice.getUUID(),
                        mUnProvisionedDevice.getAddress(),
                        mUnProvisionedDevice.getAddressType(),
                        mUnProvisionedDevice.getGattDevName(),
                        0,
                        0);
                    return;
                }
            }
        }
    }

    private void printfGroupProvResult() {
        log("printfGroupProvResult()");
        log("devicesize = " + deviceSize);
        StringBuffer str = new StringBuffer();
        for (int i = 0; i < uuidNoneExistent.size(); i++) {
            str.append("uuidNoneExistent ").append(i + 1).append(" = ").append(MeshUtils.intArrayToString(uuidNoneExistent.get(i), true));
            log(str.toString());
            str.delete(0, str.length());
        }
        for (int i = 0; i < uuidProFail.size(); i++) {
            str.append("uuidProFail ").append(i + 1).append(" = ").append(MeshUtils.intArrayToString(uuidProFail.get(i), true));
            log(str.toString());
            str.delete(0, str.length());
        }
        for (int i = 0; i < uuidConfigFail.size(); i++) {
            str.append("uuidConfigFail ").append(i + 1).append(" = ").append(MeshUtils.intArrayToString(uuidConfigFail.get(i), true));
            log(str.toString());
            str.delete(0, str.length());
        }
        for (int i = 0; i < uuidConfigSuccess.size(); i++) {
            str.append("uuidConfigSuccess ").append(i + 1).append(" = ").append(MeshUtils.intArrayToString(uuidConfigSuccess.get(i), true));
            log(str.toString());
            str.delete(0, str.length());
        }
        str = null;
        uuidNoneExistent.clear();
        uuidProFail.clear();
        uuidConfigFail.clear();
        uuidConfigSuccess.clear();
        uuidIndex = 0;
        uuid = null;
        deviceSize = 0;
    }

    private void adbGroupNodeReset(int state) {
        log("adbGroupNodeReset() , state = " + state);
        if (!isOpenMeshSwitch) {
            log("mesh is off");
            return;
        }
        switch (state) {
            case ALL_NODE_RESET:
                nodeResetSize = mProvisionerService.getNodes().size();
                break;
            case PARTIAL_NODE_RESET:
                nodeResetSize = mProvisionerService.nodeResetList.size();
                break;
            case SINGLE_NODE_RESET:
                nodeResetSize = nodePosition + 1;
                break;
            default:
               break;
        }
        if (nodeResetSize == 0) {
            log("nodeResetSize == 0");
            return;
        }
        if (isScanning) {
            mProvisionerService.stopUnProvsionScan();
            mProgressBar.setVisibility(View.GONE);
            isScanning = false;
        }
        nodeReset(state);
    }

    private void nodeReset(int state) {
        log("nodeReset() , state = " + state);
        nodeResetList.clear();
        nodeAllResetList.clear();
        nodeResetNoneExistent.clear();
        switch (state) {
            case ALL_NODE_RESET:
                for(Map.Entry<Integer,Node> entry:mProvisionerService.getNodes().entrySet()) {
                    Node node = entry.getValue();
                    if (node != null) {
                        nodeAllResetList.add(node);
                        if (node.isConfigSuccess()) {
                            nodeResetList.add(node);
                        } else {
                            log("all Node reset , but this node config fail , uuid = " + MeshUtils.intArrayToString(node.getUUID(),true) + " , gattAddr = " + node.getGattAddr());
                        }
                        nodeAdapter.removeNode(node);
                    } else {
                        log("this node not found");
                    }
                }
                break;
            case PARTIAL_NODE_RESET:
                for (int i = 0; i < nodeResetSize; i++) {
                    nodeResetUUID = MeshUtils.StringToIntArray(mProvisionerService.nodeResetList.get(i));
                    log("nodeResetUUID = " + MeshUtils.intArrayToString(nodeResetUUID, true));
                    Node node = mProvisionerService.getNodeByUUID(nodeResetUUID);
                    if (node == null) {
                        log("this node not found");
                        nodeResetNoneExistent.add(nodeResetUUID);
                    } else {
                        nodeAllResetList.add(node);
                        if (node.isConfigSuccess()) {
                            nodeResetList.add(node);
                        } else {
                            log("partial Node reset , but this node config fail , uuid = " + MeshUtils.intArrayToString(node.getUUID(),true));
                        }
                        nodeAdapter.removeNode(node);
                    }
                }

                break;
            case SINGLE_NODE_RESET:
                Node node = (MyApplication.nodeList).get(nodePosition);
                if (node == null) {
                    log("this node not found");
                    nodeResetNoneExistent.add(nodeResetUUID);
                } else {
                    nodeAllResetList.add(node);
                    if (node.isConfigSuccess()) {
                        nodeResetList.add(node);
                    } else {
                        log("single Node reset , but this node config fail , uuid = " + MeshUtils.intArrayToString(node.getUUID(),true)+ " , gattAddr = " + node.getGattAddr());
                    }
                    nodeAdapter.removeNode(node);
                }
                break;
            default:
               break;
        }

        log("nodeAllResetList size = " + nodeAllResetList.size() + " , nodeResetList size = " + nodeResetList.size());
        if (nodeAllResetList.size() == 0) {
            log("group node reset list is null");
            return;
        }
        mProvisionerService.groupConfigMsgNodeReset(nodeAllResetList);
        remainNodeList.clear();
        for(Map.Entry<Integer, Node> entry:mProvisionerService.getNodes().entrySet()) {
            Node node = entry.getValue();
            for (int j = 0; j < nodeAllResetList.size(); j++) {
                if (node.getAddr() == nodeAllResetList.get(j).getAddr()) {
                    break;
                }
            }
            remainNodeList.add(node);
        }
        if (nodeResetList.size() > 0) {
            dialogNodeReset = new Dialog(this, R.style.Custom_Progress);
            dialogNodeReset.setContentView(R.layout.mul_picture_progressbar);
            TextView tip = (TextView)dialogNodeReset.findViewById(R.id.tipTextView);
            if (state == ALL_NODE_RESET) {
                tip.setText("Group NodeReset...");
            } else if (state == PARTIAL_NODE_RESET) {
                tip.setText("Group NodeReset...");
            } else if (state == SINGLE_NODE_RESET) {
                tip.setText("NodeReset...");
            }
            dialogNodeReset.getWindow().getAttributes().gravity = Gravity.CENTER;
            dialogNodeReset.setCanceledOnTouchOutside(false);
            dialogNodeReset.show();
            groupNodeResetTimer = new Timer();
            groupNodeResetTimer.schedule(new TimerTask() {
                public void run() {
                    try {
                        log("groupNodeReset timeout , not all node ack reset");
                        StringBuilder nodeResetStr = new StringBuilder("reset node :");
                        StringBuilder ackNodeResetStr = new StringBuilder("ack reset node :");
                        for (int i = 0; i < nodeResetList.size(); i++) {
                            nodeResetStr.append("  ").append(MeshUtils.decimalToHexString("%04X", nodeResetList.get(i).getAddr()));
                        }
                        for(Map.Entry<Integer,Node> entry:ackNodeResetList.entrySet()) {
                            int address = entry.getKey();
                            ackNodeResetStr.append("  ").append(MeshUtils.decimalToHexString("%04X", address));
                        }
                        log(nodeResetStr.toString());
                        log(ackNodeResetStr.toString());
                        nodeResetStr = null;
                        ackNodeResetStr = null;
                        nodeResetList.clear();
                        ackNodeResetList.clear();
                        if (null != dialogNodeReset) {
                            dialogNodeReset.dismiss();
                            dialogNodeReset = null;
                        }
                        groupNodeResetTimer.cancel();
                        keyrefresh(0, remainNodeList);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }, nodeResetList.size()*2000*5 + 5000);
        } else { //directly keyrefresh
            keyrefresh(0, remainNodeList);
        }
    }

    public Node getNodeByAddr(int src) {
        for (int i = 0; i < nodeResetList.size(); i++) {
            Node node = nodeResetList.get(i);
            if (node.getAddr() == src) {
                return node;
            }
        }
        return null;
    }

    public void keyrefresh(int netKeyIndex, ArrayList<Node> targetNodes) {
        log("keyrefresh , netKeyIndex = " + netKeyIndex + " , targetNodes size = " + targetNodes.size());
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                dialogKeyrefresh = new Dialog(MainActivity.this, R.style.Custom_Progress);
                dialogKeyrefresh.setContentView(R.layout.mul_picture_progressbar);
                dialogKeyrefresh.getWindow().getAttributes().gravity = Gravity.CENTER;
                dialogKeyrefresh.setCanceledOnTouchOutside(false);
                dialogKeyrefresh.show();
                if(targetNodes.size() > 0){
                    mProvisionerService.keyRefreshStart(keyfresh_netkeyIndex, targetNodes);
                } else {
                    mProvisionerService.keyRefreshStart(keyfresh_netkeyIndex, null);
                }
            }
        });
    }

    private void adbGetProvisionTime(Intent intent , boolean PB_GATT) {
        log("adbProvision , PB-GATT : " + PB_GATT);
        if(!isOpenMeshSwitch) {
            log("mesh is off");
            return;
        }
        String mGattAddr = intent.getStringExtra("gattAddr");
        String uuidStr = intent.getStringExtra("uuid");
        log("mGattAddr = " + mGattAddr + "uuidStr = " + uuidStr);
        Node node = null;
        if (mProvisionerService.getNodes().size() == 0) {
            log("node list is null");
            return;
        }
        if (PB_GATT) {
            if (mGattAddr == null) {
                Toast.makeText(MainActivity.this, "not set PB-GATT ddress", Toast.LENGTH_SHORT).show();
                log("not set PB-GATT ddress");
                return;
            }
            node = mProvisionerService.getNodeByGattAddr(mGattAddr);
        } else {
            if (uuidStr == null) {
                Toast.makeText(MainActivity.this, "not set PB-ADV UUID", Toast.LENGTH_SHORT).show();
                log("not set PB-ADV UUID");
                return;
            }
            node = mProvisionerService.getNodeByUUID(MeshUtils.StringToIntArray(uuidStr));
        }
        if (node == null) {
            log("not find match node in node list");
            return;
        }
        double mProvisionTime = node.getProvisioningTime();
        log("mProvisionTime = " + mProvisionTime);
        StringBuilder timeStr = new StringBuilder("provision time : ");
        Toast.makeText(MainActivity.this, timeStr.append(mProvisionTime).append("s").toString(), Toast.LENGTH_SHORT).show();
    }

    private void adbGetConfigTime(Intent intent , boolean PB_GATT) {
        log("adbProvision , PB-GATT : " + PB_GATT);
        if(!isOpenMeshSwitch) {
            log("mesh is off");
            return;
        }
        String mGattAddr = intent.getStringExtra("gattAddr");
        String uuidStr = intent.getStringExtra("uuid");
        log("mGattAddr = " + mGattAddr + "uuidStr = " + uuidStr);
        Node node = null;
        if (mProvisionerService.getNodes().size() == 0) {
            log("node list is null");
            return;
        }
        if (PB_GATT) {
            if (mGattAddr == null) {
                Toast.makeText(MainActivity.this, "not set PB-GATT ddress", Toast.LENGTH_SHORT).show();
                log("not set PB-GATT ddress");
                return;
            }
            node = mProvisionerService.getNodeByGattAddr(mGattAddr);
        } else {
            if (uuidStr == null) {
                Toast.makeText(MainActivity.this, "not set PB-ADV UUID", Toast.LENGTH_SHORT).show();
                log("not set PB-ADV UUID");
                return;
            }
            node = mProvisionerService.getNodeByUUID(MeshUtils.StringToIntArray(uuidStr));
        }
        if (node == null) {
            log("not find match node in node list");
            return;
        }
        double mConfigTime = node.getConfigTime();
        log("mConfigTime = " + mConfigTime);
        StringBuilder timeStr = new StringBuilder("config time : ");
        Toast.makeText(MainActivity.this, timeStr.append(mConfigTime).append("s").toString(), Toast.LENGTH_SHORT).show();
    }

    private void adbGetHBMaxLostNumber(Intent intent , boolean PB_GATT) {
        log("adbProvision , PB-GATT : " + PB_GATT);
        if(!isOpenMeshSwitch) {
            log("mesh is off");
            return;
        }
        String mGattAddr = intent.getStringExtra("gattAddr");
        String uuidStr = intent.getStringExtra("uuid");
        log("mGattAddr = " + mGattAddr + "uuidStr = " + uuidStr);
        Node node = null;
        if (mProvisionerService.getNodes().size() == 0) {
            log("node list is null");
            return;
        }
        if (PB_GATT) {
            if (mGattAddr == null) {
                Toast.makeText(MainActivity.this, "not set PB-GATT ddress", Toast.LENGTH_SHORT).show();
                log("not set PB-GATT ddress");
                return;
            }
            node = mProvisionerService.getNodeByGattAddr(mGattAddr);
        } else {
            if (uuidStr == null) {
                Toast.makeText(MainActivity.this, "not set PB-ADV UUID", Toast.LENGTH_SHORT).show();
                log("not set PB-ADV UUID");
                return;
            }
            node = mProvisionerService.getNodeByUUID(MeshUtils.StringToIntArray(uuidStr));
        }
        if (node == null) {
            log("not find match node in node list");
            return;
        }
        int mHbMaxLostNumber = node.getMaxLost();
        log("mHbMaxLostNumber = " + mHbMaxLostNumber);
        StringBuilder timeStr = new StringBuilder("HeartBeat Max Lost Number : ");
        Toast.makeText(MainActivity.this, timeStr.append(mHbMaxLostNumber).toString(), Toast.LENGTH_SHORT).show();
    }


    private void adbGetNodeStatus(Intent intent , boolean PB_GATT) {
        log("adbProvision , PB-GATT : " + PB_GATT);
        if(!isOpenMeshSwitch) {
            log("mesh is off");
            return;
        }
        String mGattAddr = intent.getStringExtra("gattAddr");
        String uuidStr = intent.getStringExtra("uuid");
        log("mGattAddr = " + mGattAddr + "uuidStr = " + uuidStr);
        Node node = null;
        if (mProvisionerService.getNodes().size() == 0) {
            log("node list is null");
            return;
        }
        if (PB_GATT) {
            if (mGattAddr == null) {
                Toast.makeText(MainActivity.this, "not set PB-GATT ddress", Toast.LENGTH_SHORT).show();
                log("not set PB-GATT ddress");
                return;
            }
            node = mProvisionerService.getNodeByGattAddr(mGattAddr);
        } else {
            if (uuidStr == null) {
                Toast.makeText(MainActivity.this, "not set PB-ADV UUID", Toast.LENGTH_SHORT).show();
                log("not set PB-ADV UUID");
                return;
            }
            node = mProvisionerService.getNodeByUUID(MeshUtils.StringToIntArray(uuidStr));
        }
        if (node == null) {
            log("not find match node in node list");
            return;
        }
        StringBuilder statusStr = new StringBuilder("Node Status : ");
        if(node.getActiveStatus() == 1) {
            statusStr.append("Online");
        }else if (node.getActiveStatus() == 0){
            statusStr.append("Offline");
        } else {
            statusStr.append("Unknown");
        }
        log("statusStr = " + statusStr);
        Toast.makeText(MainActivity.this, statusStr.toString(), Toast.LENGTH_SHORT).show();
    }


    private NoDoubleOnItemClickListener mNoDoubleOnItemClickListener = new NoDoubleOnItemClickListener() {
        @Override
        public void onNoDoubleClick(AdapterView<?> parent, View view, int position, long id) {
            log("unProvisionedAdapter.getPosition() = " + unProvisionedAdapter.getPosition());
            log("provisionedAdapter.getPosition() = " + nodeAdapter.getPosition());
            switch (parent.getId()) {
                case R.id.unprovisioned:
                    if(unProvisionedAdapter.getPosition() < 0 && nodeAdapter.getPosition() < 0) {
                        UnProvisionedDevice mUnProvisionedDevice = unProvisionedAdapter.getUnprovisionedDevice(position);
                        if (mUnProvisionedDevice == null) {
                            log("mNoDoubleOnItemClickListener , not fonund unProvisionedDevice , position = " + position);
                            return;
                        }
                        int netkeyIndexSize = mProvisionerService.getNetKeyCnt();
                        int appkeyIndexSize = mProvisionerService.getAppKeyCnt();
                        showUnprovisionedFragment(position,mUnProvisionedDevice,netkeyIndexSize,appkeyIndexSize);
                    }
                    break;
                case R.id.node:
                    if(unProvisionedAdapter.getPosition() < 0 && nodeAdapter.getPosition() < 0 ) {
                        ArrayList<MeshMessage> messages = mProvisionerService.getSupportedMessages((MyApplication.nodeList).get(position));
                        ArrayList<Integer> groupAddrDatas = mProvisionerService.getGroupAddrList();
                        MyApplication.netkeyindex = mProvisionerService.getNetKeyCnt();
                        MyApplication.appkeyindex = mProvisionerService.getAppKeyCnt();
                        showProvisionedDialogFragment(position,messages,groupAddrDatas);
                    }
                    break;
                default:
                   break;
            }
        }
     };


    private BluetoothMeshListener mBluetoothMeshListener = new BluetoothMeshListener() {
        @Override
        public void onMeshEnabled() {
            log("onMeshEnabled()");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    mProvisionerService.setEnabled(llState,true);
                    mSlideSwitch.setSlideable(true);
                    llState.setClickable(true);
                    llClose.setVisibility(View.GONE);
                    llOpen.setVisibility(View.VISIBLE);
                    closeDescription.setText(R.string.close_description);
                    //mProvisionerService.setMeshMode(MeshConstants.MESH_MODE_STANDBY);
                    mProvisionerService.startUnProvsionScan();
                    isOpenMeshSwitch = true;
                    isScanning = true;
                    Toast.makeText(MainActivity.this, "mesh on", Toast.LENGTH_SHORT).show();
                }
            });

        }

        @Override
        public void inviteProvisioning(int[] UUID,int bearerType,String address,int addrType,String devName, int netKeyIndex,int appKeyIndex,int position) {
            log("inviteProvisioning() , UUID = " + MeshUtils.intArrayToString(UUID, true) + " , Gatt address = " + address + " , bearerType = " + bearerType + " , position = " + position);
            unProvisionedAdapter.setPosition(position);
            if (isScanning) {
                mProvisionerService.stopUnProvsionScan();
                mProgressBar.setVisibility(View.GONE);
                isScanning = false;
            }
            //MyApplication.isProvisioning = true;
            //MyApplication.provisioningTime = System.currentTimeMillis();
            mProvisionerService.inviteProvisioning(bearerType, UUID, address, addrType, devName, netKeyIndex, appKeyIndex);
            //if(null != mRunnable){
                //mHandler.removeCallbacks(mRunnable);
            //}
            //inviteProvisioningTimeout();
        }

        @Override
        public void onProvDone(int address, int[] deviceKey, boolean success, boolean gatt_bearer, double provision_time) {
            log("onProvDone() , address = " + MeshUtils.decimalToHexString("%04x", address) + " , success = " + success + " , gatt_bearer = " + gatt_bearer + " , provision_time = " + provision_time);
            if (unProvisionedAdapter.getPosition() == -1) {
                log("may be provision timeout !!!");
                return;
            }
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    //MyApplication.provisioningTime = System.currentTimeMillis() - MyApplication.provisioningTime;
                    //MyApplication.configTime = System.currentTimeMillis();
                    //double time = MeshUtils.getDoubleDecimal(MyApplication.provisioningTime*1.0/1000);
                    //if(null != mRunnable){
                        //mHandler.removeCallbacks(mRunnable);
                    //}
                    if(success) {
                        log(getResources().getString(R.string.prov_success));
                        UnProvisionedDevice mUnProvisionedDevice = unProvisionedAdapter.getUnprovisionedDevice(unProvisionedAdapter.getPosition());
                        if (mUnProvisionedDevice == null) {
                            log("not found UnProvisionedDevice , but it should not happen");
                            return;
                        }
                        unProvisionedAdapter.removeUnprovisionedDevice(mUnProvisionedDevice);
                        unProvisionedAdapter.setPosition(UnprovisionedAdapter.ALL_ENABLE,false);
                        //log("node = " + mProvisionerService.getNodeByAddr(address));
                        //log("uuid = " + mProvisionerService.getNodeByAddr(address).getUUID());
                        //log("onProvDone MyApplication nodeList size = " + MyApplication.nodeList.size());
                        nodeAdapter.addNode(mProvisionerService.getNodeByAddr(address));
                        //log("onProvDone MyApplication nodeList size = " + MyApplication.nodeList.size());
                        nodeAdapter.setPosition((MyApplication.nodeList).size()-1);
                        nodeRecyclerView.scrollToPosition(nodeAdapter.getItemCount() - 1);
                        MyApplication.isProvisioning = false;
                        String provMsg = "provision success" + "\n"
                                + "provision time: " +  provision_time + "s";
                        log(provMsg);
                        Toast.makeText(MainActivity.this,provMsg,Toast.LENGTH_LONG).show();

                    }else {
                        log(getResources().getString(R.string.prov_fail));
                        MyApplication.isProvisioning = false;
                        if (mProvisionerService != null) {
                            if (mProvisionerService.getMeshMode() != MeshConstants.MESH_MODE_STANDBY) {
                                mProvisionerService.setMeshMode(MeshConstants.MESH_MODE_STANDBY); // provision fail , need set mesh standby mode
                            }
                        }
                        String provMsg = "provision fail" + "\n"
                                + "provision time: " +  provision_time + "s";
                        log(provMsg);
                        Toast.makeText(MainActivity.this,provMsg,Toast.LENGTH_LONG).show();
                        unProvisionedAdapter.setPosition(UnprovisionedAdapter.ALL_ENABLE);
                        if (uuidIndex > 0) {
                            uuidProFail.add(uuid);
                            if (uuidIndex < deviceSize) {
                                provision();
                            } else {
                                printfGroupProvResult();
                            }
                        }
                    }
                }
            });
        }

        @Override
        public void onProvScanComplete() {
            log("onProvScanComplete()");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    mProgressBar.setVisibility(View.GONE);
                    isScanning = false;
                }
            });
        }

        @Override
        public void onScanUnProvDevice(int[] uuid, int oobInfom, int[] uriHash) {
            log("onScanUnProvDevice() uuid = " + MeshUtils.intArrayToString(uuid, true));
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    //TODO update UI to add PB-ADV UD
                    UnProvisionedDevice mUnProvisionedDevice = new UnProvisionedDevice(MeshConstants.MESH_BEARER_ADV,uuid,null,0,null);
                    boolean result = unProvisionedAdapter.addUnprovisionedDevice(mUnProvisionedDevice);
                    List<Node> nodes = nodeAdapter.getNodes();
                    for(int i=0 ; i < nodes.size() ; i++){
                        if(Arrays.equals(uuid,nodes.get(i).getUUID())) {
                            nodeAdapter.removeNodeByIndex(i);
                            break;
                        }
                    }
                    /*
                    if (result) {
                        for (int i = 0; i < mProvisionerService.uuidList.size(); i ++) {
                            if (!mProvisionerService.isProvOrConfiging && Arrays.equals(MeshUtils.StringToIntArray(mProvisionerService.uuidList.get(i)), uuid)) {
                                int position = unProvisionedAdapter.getPositionByUnProvisioned(mUnProvisionedDevice);
                                if(position == -1) {
                                    log("position = -1");
                                    return;
                                }
                                unProvisionedAdapter.setPosition(position);
                                mProvisionerService.stopUnProvsionScan();
                                mProgressBar.setVisibility(View.GONE);
                                isScanning = true;
                                mProvisionerService.inviteProvisioning(mUnProvisionedDevice.getBearerType(),mUnProvisionedDevice.getUUID(),
                                    mUnProvisionedDevice.getAddress(),
                                    mUnProvisionedDevice.getAddressType(),
                                    mUnProvisionedDevice.getGattDevName(),
                                    0,
                                    0);
                                break;
                            }
                        }
                    }
                    */
                }
            });
        }

        @Override
        public void onScanUnProvDevicePBGatt(String name, String addr, int addrType, int rssi) {
            log("onScanUnProvDevicePBGatt() addr = " + addr);
            if (null != name) {
                log("onScanUnProvDevicePBGatt() name = " + name);
            } else {
                log("name is null, this shall not happen");
                return;
            }
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    //TODO update UI to add PB-GATT UD
                    boolean isExistGatt = false;
                    boolean result = false;
                    UnProvisionedDevice mUnProvisionedDevice = null;
                    List<Node> nodes = nodeAdapter.getNodes();
                    log("nodes size = " + nodes.size());
                    /*
                    if(nodes.size() == 0) {
                        unProvisionedAdapter.addUnprovisionedDevice(new UnProvisionedDevice(MeshConstants.MESH_BEARER_GATT,null,addr,addrType,name));
                    }
                    */
                    for(int i=0 ; i < nodes.size() ; i++){
                        Node node = nodes.get(i);
                        log("node = " + node);
                        log("node.getNodeBearer() = " + node.getNodeBearer());
                        log("node.getGattAddr() = " + node.getGattAddr() );
                        if ((null != node) && (node.getNodeBearer() == MeshConstants.MESH_BEARER_GATT)) {
                            log("node.getGattAddr() = " + node.getGattAddr() );
                            isExistGatt = true;
                            if((node.getGattAddr().compareTo(addr) == 0) && name.contains("Provisioning")) {
                                mUnProvisionedDevice = new UnProvisionedDevice(MeshConstants.MESH_BEARER_GATT, null, addr, addrType, name);
                                result = unProvisionedAdapter.addUnprovisionedDevice(mUnProvisionedDevice);
                                nodeAdapter.removeNodeByIndex(i);
                                break;
                            }else if(node.getGattAddr().compareTo(addr) != 0) {
                                mUnProvisionedDevice = new UnProvisionedDevice(MeshConstants.MESH_BEARER_GATT, null, addr, addrType, name);
                                result = unProvisionedAdapter.addUnprovisionedDevice(mUnProvisionedDevice);
                            }
                        }
                    }
                    if(!isExistGatt) {
                        log("isExistGatt node.getGattAddr() = " + addr);
                        mUnProvisionedDevice = new UnProvisionedDevice(MeshConstants.MESH_BEARER_GATT, null, addr, addrType, name);
                        result = unProvisionedAdapter.addUnprovisionedDevice(mUnProvisionedDevice);
                    }
                    /*
                    if (result) {
                        for (int i = 0; i < mProvisionerService.uuidList.size(); i ++) {
                            if (!mProvisionerService.isProvOrConfiging && addr.equals(mProvisionerService.uuidList.get(i))) {
                                int position = unProvisionedAdapter.getPositionByUnProvisioned(mUnProvisionedDevice);
                                if(position == -1) {
                                    log("position = -1");
                                    return;
                                }
                                unProvisionedAdapter.setPosition(position);
                                mProvisionerService.stopUnProvsionScan();
                                mProgressBar.setVisibility(View.GONE);
                                isScanning = true;
                                mProvisionerService.inviteProvisioning(mUnProvisionedDevice.getBearerType(),mUnProvisionedDevice.getUUID(),
                                    mUnProvisionedDevice.getAddress(),
                                    mUnProvisionedDevice.getAddressType(),
                                    mUnProvisionedDevice.getGattDevName(),
                                    0,
                                    0);
                                break;
                            }
                        }
                    }
                    */
                }
            });
        }

        @Override
        public void onProvStateChanged(Node node, boolean success) {
            log("onProvStateChanged() node uuid = " + MeshUtils.intArrayToString(node.getUUID(), true));
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (success) {
                        switch (node.getConfigState()) {
                            case ProvisionerService.MESH_NODE_CONFIG_STATE_GET_COMPOSITION_DATA: {
                                //TODO Composition Data Getting
                                log("ProvisionerService.MESH_NODE_CONFIG_STATE_GET_COMPOSITION_DATA");
                                break;
                            }
                            case ProvisionerService.MESH_NODE_CONFIG_STATE_ADD_APPKEY: {
                                //TODO App Key adding
                                log("ProvisionerService.MESH_NODE_CONFIG_STATE_ADD_APPKEY");
                                break;
                            }
                            case ProvisionerService.MESH_NODE_CONFIG_STATE_MODEL_APP_BIND: {
                                //TODO Model App Binding
                                log("ProvisionerService.MESH_NODE_CONFIG_STATE_MODEL_APP_BIND");
                                break;
                            }
                            case ProvisionerService.MESH_NODE_CONFIG_STATE_IDLE: {
                                //TODO provision config complete successfully
                                log("ProvisionerService.MESH_NODE_CONFIG_STATE_IDLE");
                                log("config success , config time = " + node.getConfigTime() + "s");
                                if (mProvisionerService != null) {
                                    mProvisionerService.isProvOrConfiging = false;
                                    if (mProvisionerService.getMeshMode() != MeshConstants.MESH_MODE_STANDBY) {
                                        mProvisionerService.setMeshMode(MeshConstants.MESH_MODE_STANDBY); // config success , need set standby mode
                                    }
                                }
                                //provisionedAdapter.setPosition(ProvisionedAdapter.ALL_ENABLE);
                                //MyApplication.configTime = System.currentTimeMillis() - MyApplication.configTime;
                                //double time = MeshUtils.getDoubleDecimal(MyApplication.configTime*1.0/1000);
                                nodeAdapter.nodeTimeChange(node,NodeAdapter.ALL_ENABLE);
                                String provmesg = "Config success" + "\n"
                                        + "Config time: " +  node.getConfigTime() + "s";
                                Toast.makeText(MainActivity.this,provmesg,Toast.LENGTH_LONG).show();
                                if (uuidIndex > 0) {
                                    uuidConfigSuccess.add(uuid);
                                    if (uuidIndex < deviceSize) {
                                        provision();
                                    } else {
                                        printfGroupProvResult();
                                    }
                                }
                                break;
                            }
                            default:
                                break;
                        }
                    } else {
                        //TODO provisioning config failed
                        log("config fail , config time = " + node.getConfigTime() + "s , ProvisionerService.MESH_NODE_CONFIG_STATE_IDLE");
                        if (mProvisionerService != null) {
                            mProvisionerService.isProvOrConfiging = false;
                            if (mProvisionerService.getMeshMode() != MeshConstants.MESH_MODE_STANDBY) {
                                mProvisionerService.setMeshMode(MeshConstants.MESH_MODE_STANDBY); // config fail , need set standby mode
                            }
                        }
                        //provisionedAdapter.setPosition(ProvisionedAdapter.ALL_ENABLE);
                        //MyApplication.configTime = System.currentTimeMillis() - MyApplication.configTime;
                        //double time = MeshUtils.getDoubleDecimal(MyApplication.configTime*1.0/1000);
                        nodeAdapter.nodeTimeChange(node,NodeAdapter.ALL_ENABLE);
                        String provmesg = "Config fail" + "\n"
                                 + "Config time: " +  node.getConfigTime() + "s";
                        Toast.makeText(MainActivity.this,provmesg,Toast.LENGTH_LONG).show();
                        if (uuidIndex > 0) {
                            uuidConfigFail.add(uuid);
                            if (uuidIndex < deviceSize) {
                                provision();
                            } else {
                                printfGroupProvResult();
                            }
                        }
                    }
                }
            });
        }

        @Override
        public void sendMessage(MeshMessage msg, int position, int appKeyIndex, int groupAddr, int[] payload) {
            mProvisionerService.sendMessage(msg,(MyApplication.nodeList).get(position),appKeyIndex,groupAddr,payload);
        }

        @Override
        public void gattProvisioningConnectFail() {
            log("gattProvisioningConnectFail");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    //if(null != mRunnable){
                        //mHandler.removeCallbacks(mRunnable);
                    //}
                    unProvisionedAdapter.setPosition(UnprovisionedAdapter.ALL_ENABLE);
                    Toast.makeText(MainActivity.this,"PB-Gatt provisioning service connect fail",Toast.LENGTH_LONG).show();
                }
            });
        }

        @Override
        public void gattProxyConnectFail() {
            log("gattProxyConnectFail");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    nodeAdapter.setPosition(NodeAdapter.ALL_ENABLE);
                    Toast.makeText(MainActivity.this,"PB-Gatt proxy service connect fail",Toast.LENGTH_LONG).show();
                }
            });
        }

        @Override
        public void onNodeMsgRecieved(BluetoothMeshAccessRxMessage msg) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    int src = msg.getSrcAddr();
                    double time = MeshUtils.getDoubleDecimal(MyApplication.sendMessageTime*1.0/1000);
                    log("onNodeMsgRecieved , src = " + MeshUtils.decimalToHexString("%04X", src)+ " , status: " + msg.getBuffer()[0]);
                    switch (msg.getOpCode()) {
                        case MeshConstants.MESH_MSG_GENERIC_ONOFF_STATUS: {
                            StringBuilder sb = new StringBuilder("<<Generic OnOff Status>> from Node ");
                            sb.append(MeshUtils.decimalToHexString("%04X", src)).append(" , status: ").append(msg.getBuffer()[0]).append(" , time: ").append(time).append("s");
                            log(sb.toString());
                            Toast.makeText(MainActivity.this, sb.toString(), Toast.LENGTH_SHORT).show();
                            break;
                        }
                        default:
                            break;
                    }
                }
            });
        }

        @Override
        public void onStorageDataChange(StorageData data, int type) {
            //TODO Update database
            log("onStorageDataChange , type = " + type);
            List<StorageData> datas = LouSQLite.query(MyCallBack.TABLE_NAME_STORAGE
                , "select * from " + MyCallBack.TABLE_NAME_STORAGE
                , null);
            StorageData mStorageData = datas.get(0);
            switch (type) {
                case MyApplication.STORAGE_DATA_ALL:
                    //MyApplication.nodeList = data.getAllNode();
                    //mStorageData.setAllNode(data.getAllNode());
                    mStorageData.setAllNetKey(data.getAllNetKey());
                    mStorageData.setAllAppKey(data.getAllAppKey());
                    mStorageData.setAllGroupAddrList(data.getAllGroupAddr());
                    break;
                case MyApplication.STORAGE_DATA_NETKEY:
                    mStorageData.setAllNetKey(data.getAllNetKey());
                    break;
                case MyApplication.STORAGE_DATA_APPKEY:
                    mStorageData.setAllAppKey(data.getAllAppKey());
                    break;
                case MyApplication.STORAGE_DATA_GROUP_ADDR:
                    mStorageData.setAllGroupAddrList(data.getAllGroupAddr());
                    break;
                default:
                    break;
            }
            LouSQLite.update(MyCallBack.TABLE_NAME_STORAGE,mStorageData,ProvisionedDeviceEntry.COLEUM_NAME_ID + "=?",new String[]{mStorageData.getId()});
        }

        @Override
        public void onNodeDataChange(NodeData nodeData, int type) {
            //TODO Update database
            log("onNodeDataChange , type = " + type);
            switch (type) {
                case MyApplication.NODE_DATA_ADD:
                    LouSQLite.insert(MyCallBack.TABLE_NAME_NODE, nodeData);
                    break;
                case MyApplication.NODE_DATA_DELETE:
                    LouSQLite.delete(MyCallBack.TABLE_NAME_NODE, ProvisionedDeviceEntry.COLEUM_NAME_ADDRESS + "=?",new String[]{nodeData.getAddress()});
                    break;
                case MyApplication.NODE_DATA_UPDATE:
                    LouSQLite.update(MyCallBack.TABLE_NAME_NODE, nodeData, ProvisionedDeviceEntry.COLEUM_NAME_ADDRESS + "=?",new String[]{nodeData.getAddress()});
                    break;
                default:
                    break;
            }
        }

        @Override
        public void onNodeActiveStatusChange(Node node, int active) {
            log("onNodeActiveStatusChange");
            //TODO update UI
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    nodeAdapter.nodeStateChange(node, active);
                }
            });
        }

        @Override
        public void onNodeRemoved(boolean result, Node node) {
            log("onNodeRemoved result: " + (result ? "success" : "fail"));
            //TODO update UI
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    nodeAdapter.removeNode(node);
                }
            });
        }

        @Override
        public void onConfigMsgModelSubAdd(Node node, int eleAddr, int subAddr, long modelId) {
            log("onConfigMsgModelSubAdd");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    mProvisionerService.configMsgModelSubAdd(node,eleAddr,subAddr,modelId);
                }
            });
        }

        @Override
        public void onConfigMsgNetKeyAdd(Node node, int targetNetkeyIdx) {
            log("onConfigMsgNetKeyAdd");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    mProvisionerService.configMsgNetKeyAdd(node,targetNetkeyIdx);
                }
            });
        }

        @Override
        public void onConfigMsgAppKeyAdd(Node node, int targetNetKeyIndex, int appKeyIdx) {
            log("onConfigMsgAppKeyAdd");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    mProvisionerService.configMsgAppKeyAdd(node, targetNetKeyIndex, appKeyIdx);
                }
            });
        }


        @Override
        public void onConfigMsgCompositionDataGet(Node node, int page) {
            log("onConfigMsgCompositionDataGet");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    mProvisionerService.configMsgCompositionDataGet(node, page);
                }
            });
        }

        @Override
        public void onConfigMsgNodeReset(int position) {
            log("onConfigMsgNodeReset");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    mProvisionerService.configMsgNodeReset(MyApplication.nodeList.get(position), false, 1);
                    nodeAdapter.removeNode((MyApplication.nodeList).get(position));
                }
            });
        }

        @Override
        public void onKeyRefreshComplete(int netkeyIndex, boolean result) {
            log("onKeyRefreshComplete netkey index = " + netkeyIndex + ", result: " + (result ? "SUCCESS" : "FAIL"));
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    log("dialogKeyrefresh = " + dialogKeyrefresh);
                    remainNodeList.clear();
                    MyApplication.keyrefreshTime = System.currentTimeMillis() - MyApplication.keyrefreshTime;
                    if (mProvisionerService != null) {
                        mProvisionerService.isKeyrefreshing = false;
                        if (mProvisionerService.getMeshMode() != MeshConstants.MESH_MODE_STANDBY) {
                            if (mProvisionerService.getMeshMode() != MeshConstants.MESH_MODE_STANDBY) {
                                mProvisionerService.setMeshMode(MeshConstants.MESH_MODE_STANDBY); // keyrefresh done , need set standby mode
                            }
                        }
                    }
                    if (null != dialogKeyrefresh) {
                        dialogKeyrefresh.dismiss();
                        dialogKeyrefresh = null;
                    }
                    String keyrefreshStr = "Key refresh for netkey " + netkeyIndex + " complete: " +
                        (result ? "SUCCESS" : "FAIL") +" , time = " + MeshUtils.getDoubleDecimal(MyApplication.keyrefreshTime*1.0/1000) + "s";
                    log(keyrefreshStr);
                    Toast.makeText(MainActivity.this, keyrefreshStr, Toast.LENGTH_SHORT).show();
                }
            });
        }

        @Override
        public void onGroupSendMsg(int status, List<String> lists) {
            log("onGroupSendMsg , status = " + status);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    switch (status) {
                        case MyApplication.GROUP_SEND_MSG_START:
                            dialogGroupSendMsg = new Dialog(MainActivity.this, R.style.Custom_Progress);
                            dialogGroupSendMsg.setContentView(R.layout.mul_picture_progressbar);
                            TextView tip = (TextView)dialogGroupSendMsg.findViewById(R.id.tipTextView);
                            tip.setText("Group SendMessage...");
                            dialogGroupSendMsg.getWindow().getAttributes().gravity = Gravity.CENTER;
                            dialogGroupSendMsg.setCanceledOnTouchOutside(false);
                            dialogGroupSendMsg.show();
                            break;
                        case MyApplication.GROUP_SEND_MSG_END:
                            log("lists = " + lists);
                            String filePath = "/data/data/" + MainActivity.this.getPackageName() + "/group_message_time.txt";
                            StringBuffer buffer = new StringBuffer();
                            for (int i = 0; i < lists.size(); i++) {
                                buffer.append(lists.get(i));
                                if (i < lists.size() - 1) {
                                    buffer.append("\r\n");
                                }
                            }
                            MeshUtils.save(filePath, buffer.toString());
                            if (null != dialogGroupSendMsg) {
                                dialogGroupSendMsg.dismiss();
                                dialogGroupSendMsg = null;
                            }
                            String groupSendMsgStr = "group send msg complete , time = " + MeshUtils.getDoubleDecimal(MyApplication.groupSendMessageTime*1.0/1000) + "s";
                            log(groupSendMsgStr);
                            Toast.makeText(MainActivity.this, groupSendMsgStr, Toast.LENGTH_SHORT).show();
                            break;
                        default:
                            break;
                    }
                }
            });
        }

        @Override
        public void GattConnect(int position) {
            log("GattConnect");
            dialogGattConnect = new Dialog(MainActivity.this, R.style.Custom_Progress);
            dialogGattConnect.setContentView(R.layout.mul_picture_progressbar);
            TextView tip = (TextView)dialogGattConnect.findViewById(R.id.tipTextView);
            tip.setText("PB-GATT Connect...");
            dialogGattConnect.getWindow().getAttributes().gravity = Gravity.CENTER;
            dialogGattConnect.setCanceledOnTouchOutside(false);
            dialogGattConnect.show();
            mProvisionerService.GattConnect((MyApplication.nodeList).get(position));
        }

        @Override
        public void GattDisconnect(int position) {
            log("GattDisconnect");
            mProvisionerService.GattDisconnect((MyApplication.nodeList).get(position));
        }

        @Override
        public void GattConnectStatusChange(Node node, boolean connect) {
            log("GattConnectStatusChange node address = " + MeshUtils.decimalToHexString("%04x", node.getAddr()) + " , connect = " + connect);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if(mProvisionedDialogFragment != null) {
                        mProvisionedDialogFragment.updateGattConnectStatus(connect);
                    }
                    if(connect) {
                        if (null != dialogGattConnect) {
                            dialogGattConnect.dismiss();
                            dialogGattConnect = null;
                        }
                        Toast.makeText(MainActivity.this, "PB-GATT( Node address:"+
                            MeshUtils.decimalToHexString("%04X", node.getAddr()) + " ) " + "Connect success", Toast.LENGTH_SHORT).show();
                    }else {
                        Toast.makeText(MainActivity.this, "PB-GATT disconnect success", Toast.LENGTH_SHORT).show();
                    }
                }
            });
        }

        @Override
        public void GattConnectTimeout() {
            log("GattConnectTimeout");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (null != dialogGattConnect) {
                        dialogGattConnect.dismiss();
                        dialogGattConnect = null;
                    }
                    if(mProvisionedDialogFragment != null) {
                        mProvisionedDialogFragment.gattConnectTimeout();
                    }
                }
            });
        }

        @Override
        public void dialogFragmentCancel() {
            log("dialogFragmentCancel");
            if(mProvisionedDialogFragment != null) {
                mProvisionedDialogFragment = null;
            }
        }

        @Override
        public void updateHeartBeatReceive() {
            log("updateHeartBeatReceive");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    nodeAdapter.heartBeatChange();
                }
            });
        }

        @Override
        public void provisionTimeout(int time) {
            log("provisionTimeout");
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if(MyApplication.isProvisioning){
                        MyApplication.isProvisioning = false;
                        mProvisionerService.isProvOrConfiging = false;
                        if (mProvisionerService.getMeshMode() != MeshConstants.MESH_MODE_STANDBY) {
                            mProvisionerService.setMeshMode(MeshConstants.MESH_MODE_STANDBY); // provision timeout , need set standby mode
                        }
                        String provMsg = "provision Timeout" + "\n"
                                + "provision time: " + time + "s";
                        log(provMsg);
                        Toast.makeText(MainActivity.this,provMsg,Toast.LENGTH_LONG).show();
                        unProvisionedAdapter.setPosition(UnprovisionedAdapter.ALL_ENABLE);
                        if (uuidIndex > 0) {
                            uuidProFail.add(uuid);
                            if (uuidIndex < deviceSize) {
                                provision();
                            } else {
                                printfGroupProvResult();
                            }
                        }
                    }
                }
            });
        }

        @Override
        public void onConfigMsgAck(BluetoothMeshAccessRxMessage msg) {

            int src = msg.getSrcAddr();
            String srcStr = MeshUtils.decimalToHexString("%04X", src);
            log("onConfigMsgAck , address = " + srcStr + " , opCode = " + msg.getOpCode());

            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    //int src = msg.getSrcAddr();
                    //String srcStr = MeshUtils.decimalToHexString("%04X", src);

                    switch (msg.getOpCode()) {
                        case MeshConstants.MESH_MSG_CONFIG_BEACON_STATUS: {
                            Toast.makeText(MainActivity.this, "<<Config Beacon Status>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_COMPOSITION_DATA_STATUS: {
                            Toast.makeText(MainActivity.this, "<<Config Composition Data Status>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_DEFAULT_TTL_STATUS: {
                            Toast.makeText(MainActivity.this, "<<Config Default TTL Status>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_GATT_PROXY_STATUS: {
                            Toast.makeText(MainActivity.this, "<<Config GATT Proxy Status>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_FRIEND_STATUS: {
                            Toast.makeText(MainActivity.this, "<<Config Friend Status>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_MODEL_PUBLICATION_STATUS: {
                            Toast.makeText(MainActivity.this, "<<Config Model Publication Status>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_MODEL_SUBSCRIPTION_STATUS: {
                            Toast.makeText(MainActivity.this, "<<Config Model Subsctiption Status>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_NETWORK_TRANSMIT_STATUS: {
                            Toast.makeText(MainActivity.this, "<<Config Network Transmit Status>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_RELAY_STATUS: {
                            Toast.makeText(MainActivity.this, "<<Config Relay Status>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_SIG_MODEL_SUBSCRIPTION_LIST: {
                            Toast.makeText(MainActivity.this, "<<Config SIG Model Sbscripion List>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_VENDOR_MODEL_SUBSCRIPTION_LIST: {
                            Toast.makeText(MainActivity.this, "<<Config Vendor Model Subscription List>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_NETKEY_LIST: {
                            Toast.makeText(MainActivity.this, "<<Config NetKey List>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_NETKEY_STATUS: {
                            log("<<Config NetKey Status>> from Node " + srcStr);
                            //Toast.makeText(MainActivity.this, "<<Config NetKey Status>> from Node " + src, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_APPKEY_LIST: {
                            Toast.makeText(MainActivity.this, "<<Config AppKey List>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_APPKEY_STATUS: {
                            Toast.makeText(MainActivity.this, "<<Config AppKey Status>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_MODEL_APP_STATUS: {
                            Toast.makeText(MainActivity.this, "<<Config Model App Status>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_SIG_MODEL_APP_LIST: {
                            Toast.makeText(MainActivity.this, "<<Config SIG Model App List>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_VENDOR_MODEL_APP_LIST: {
                            Toast.makeText(MainActivity.this, "<<Config Vendor Model App List>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_NODE_IDENTITY_STATUS: {
                            Toast.makeText(MainActivity.this, "<<Config Node Identity Status>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_NODE_RESET_STATUS: {
                            log("NODE_RESET_STATUS , src = " + srcStr);
                            if (nodeResetList.size() == 0) {
                                log("nodeResetList.size() == 0");
                                return;
                            }
                            Node node = getNodeByAddr(src);
                            if (null == node) {
                                log("null == node");
                                return;
                            }
                            if (ackNodeResetList.containsKey(src)) { // repeat
                                log("MESH_MSG_CONFIG_NODE_RESET_STATUS , ackNodeResetList alread contain");
                                return;
                            }
                            ackNodeResetList.put(src, node);
                            log("nodeResetList size = " + nodeResetList.size() + " , ackNodeResetList size = " + ackNodeResetList.size());
                            if (ackNodeResetList.size() == nodeResetList.size()) { // all group node ack
                                log("all node ack node reset");
                                if (null != dialogNodeReset) {
                                    dialogNodeReset.dismiss();
                                    dialogNodeReset = null;
                                }
                                if (null != groupNodeResetTimer) {
                                    groupNodeResetTimer.cancel();
                                }
                                nodeResetList.clear();
                                ackNodeResetList.clear();
                                keyrefresh(0, remainNodeList);
                            }
                            //Toast.makeText(MainActivity.this, "<<Config Node Reset Status>> from Node " + src, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_KEY_REFRESH_PHASE_STATUS: {
                            log("<<Config Key Refresh Phase Status>> from Node " + srcStr);
                            //Toast.makeText(MainActivity.this, "<<Config Key Refresh Phase Status>> from Node " + src, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_PUBLICATION_STATUS: {
                            Toast.makeText(MainActivity.this, "<<Config Heartbeat Publication Status>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        case MeshConstants.MESH_MSG_CONFIG_HEARTBEAT_SUBSCRIPTION_STATUS: {
                            Toast.makeText(MainActivity.this, "<<Config Heartbeat Subscription Status>> from Node " + srcStr, Toast.LENGTH_SHORT).show();
                            break;
                        }
                        default: {
                            break;
                        }
                    }
                }
            });

        }
    };

}
