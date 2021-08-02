package com.mesh.test.provisioner.sqlite;

import com.mesh.test.provisioner.model.*;
import com.mesh.test.provisioner.MeshMessage;
import android.bluetooth.mesh.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Timer;
import com.mesh.test.provisioner.util.MeshUtils;



public class Node {

    private int[] uuid = null;
    private int[] devKey = null;
    private int mConfigState;
    private int mAddress;
    public int mCurrNetKeyIndex;    //the netkey used by this node currently
    private ArrayList<Integer> mNetKeyList = new ArrayList<>();   //stores all the netkey owned by this node
    private ArrayList<Integer> mAppKeyList = new ArrayList<>();   //stores all the netkey owned by this node, the first one is the default appkey in use
    public int mProvAppKeyIndex;    //the default appkey used for this node
    private int mCID;
    private int mPID;
    private int mVID;
    private int mCRPL;
    private int mFeatures;
    private int mActive = 2; //0: offline; 1: online; 2: unknown
    private int mCurretHeartBeatNumber = 0;
    private int mPrevHeartBeatNumber = 0;
    private long mHeartBeatTime = 0;
    private int mConfigTimerID = -1;
    private int mProvisionTimerID = -1;
    private int mProvisioningServiceTimerID = -1;
    private int mProxyServiceTimerID = -1;
    private int mSendMessageTimerID = -1;
    private int mHeartBeatTimerID = -1;
    private int mHeartBeatTimerNumber = 0;
    private int mNumberOfElement = 0;
    private ArrayList<Element> mElementList = new ArrayList<>();
    private ArrayList<Model> mModelList = new ArrayList<>();  //stores all the models belong to this node
    private ArrayList<MeshMessage> mSupportedMsgList = new ArrayList<>();
    private int mBearer = 0;
    private int mGattAddrType = 0;
    private String mGattDevName;
    private String mGattAddr;
    private boolean mBlacklisted;   //be blacklisted in key refresh procedure
    private double mProvisioningTime = 0; // provisioning time
    private double mConfigTime = 0; //config time
    private boolean isConfigSuccess = false;
    private int maxLost = 0;
    private int continueLost = 0;
    private boolean GattConnectState = false;// PB-Gatt connect status

    public Node() {
    }

    public Node(int[] uuid) {
        this.uuid = uuid;
    }

    public Node(int addrType, String addr) {
        this.mGattAddrType = addrType;
        this.mGattAddr = addr;
    }

    public void setProvisioningTime(double mProvisioningTime) {
        this.mProvisioningTime = mProvisioningTime;
    }

    public double getProvisioningTime() {
        return this.mProvisioningTime;
    }

    public void setConfigTime(double mConfigTime) {
        this.mConfigTime = mConfigTime;
    }

    public double getConfigTime() {
        return this.mConfigTime;
    }

    public void setConfigSuccess(boolean isConfigSuccess) {
        this.isConfigSuccess = isConfigSuccess;
    }

    public boolean isConfigSuccess() {
        return this.isConfigSuccess;
    }

    public void setGattConnectState(boolean GattConnectState) {
        this.GattConnectState = GattConnectState;
    }

    public boolean getGattConnectState() {
        return this.GattConnectState;
    }

    public void setElementNumber(int count) {
        this.mNumberOfElement = count;
    }

    public int getElementNumber() {
        return this.mNumberOfElement;
    }

    public void setGattDevName(String str) {
        this.mGattDevName = str;
    }

    public String getGattDevName() {
        return this.mGattDevName;
    }

    public void setConfigTimerID(int id) {
        this.mConfigTimerID = id;
    }

    public int getConfigTimerID() {
        return this.mConfigTimerID;
    }

    public void setProvisionTimerID(int id) {
        this.mProvisionTimerID = id;
    }

    public int getProvisionTimerID() {
        return this.mProvisionTimerID;
    }

    public void setProvisioningServiceTimerID(int id) {
        this.mProvisioningServiceTimerID = id;
    }

    public int getProvisioningServiceTimerID() {
        return this.mProvisioningServiceTimerID;
    }

    public void setProxyServiceTimerID(int id) {
        this.mProxyServiceTimerID = id;
    }

    public int getProxyServiceTimerID() {
        return this.mProxyServiceTimerID;
    }

    public void setSendMessageTimerID(int id) {
        this.mSendMessageTimerID = id;
    }

    public int getSendMessageTimerID() {
        return this.mSendMessageTimerID;
    }

    public void setHeartBeatTimerID(int id) {
        this.mHeartBeatTimerID = id;
    }

    public int getHeartBeatTimerID() {
        return this.mHeartBeatTimerID;
    }

    public void setHeartBeatTimerNumber(int number) {
        this.mHeartBeatTimerNumber = number;
    }

    public int getHeartBeatTimerNumber() {
        return this.mHeartBeatTimerNumber;
    }

    public void setHeartBeatTime(long time) {
        this.mHeartBeatTime = time;
    }

    public long getHeartBeatTime() {
        return this.mHeartBeatTime;
    }


    public void setCurrentHeartBeatNumber(int mCurretHeartBeatNumber) {
        this.mCurretHeartBeatNumber= mCurretHeartBeatNumber;
    }

    public int getCurrentHeartBeatNumber() {
        return this.mCurretHeartBeatNumber;
    }

    public void setPreHeartBeatNumber(int mPrevHeartBeatNumber) {
        this.mPrevHeartBeatNumber= mPrevHeartBeatNumber;
    }

    public int getPreHeartBeatNumber() {
        return this.mPrevHeartBeatNumber;
    }


    public void setNodeBearer(int bearer) {
        this.mBearer = bearer;
    }

    public int getNodeBearer() {
        return this.mBearer;
    }

    public void setConfigState(int state) {
        this.mConfigState = state;
    }

    public int getConfigState() {
        return this.mConfigState;
    }

    public void setContinueLost(int continueLost) {
        this.continueLost = continueLost;
    }

    public int getContinueLost() {
        return this.continueLost;
    }

    public void setMaxLost(int maxLost) {
        this.maxLost = maxLost;
    }

    public int getMaxLost() {
        return this.maxLost;
    }

    public void setBlackListState(boolean state) {
        this.mBlacklisted = state;
    }

    public boolean getBlackListState() {
        return this.mBlacklisted;
    }

    public void setCompDataHeaderInfo(int cid, int pid, int vid, int crpl, int features) {
        this.mCID = cid;
        this.mPID = pid;
        this.mVID = vid;
        this.mCRPL = crpl;
        this.mFeatures = features;
    }

    public void setFeature(int mFeatures) {
        this.mFeatures = mFeatures;
    }

    public int getFeature() {
        return mFeatures;
    }

    public void setCID(int mCID) {
        this.mCID = mCID;
    }

    public int getCID() {
        return mCID;
    }

    public void setPID(int mPID) {
        this.mPID = mPID;
    }

    public int getPID() {
        return mPID;
    }

    public void setVID(int mVID) {
        this.mVID = mVID;
    }

    public int getVID() {
        return mVID;
    }

    public void setCVPL(int mCRPL) {
        this.mCRPL = mCRPL;
    }

    public int getCVPL() {
        return mCRPL;
    }

    public void setUUID(int[] uuid) {
        this.uuid = uuid;
    }

    public int[] getUUID() {
        return this.uuid;
    }

    public void setDevKey(int[] devKey) {
        this.devKey = devKey;
    }

    public int[] getDevKey() {
        return this.devKey;
    }

    public int getActiveStatus() {
        return this.mActive;
    }

    public void setActiveStatus(int active) {
        this.mActive = active;
    }

    public void setAddr(int addr) {
        this.mAddress = addr;
    }
    public int getAddr() {
        return this.mAddress;
    }

    public void addElement(Element element){
        mElementList.add(element);
    }


    public void addModel(Model model){
        mModelList.add(model);
    }

    public void setModels(ArrayList<Model> mModelList) {
        this.mModelList = mModelList;
    }

    public ArrayList<Model> getModels() {
        return this.mModelList;
    }

    public void setElements(ArrayList<Element> mElementList){
        this.mElementList = mElementList;
    }

    public ArrayList<Element> getElements() {
        return this.mElementList;
    }

    public Element getElementByAddr(int addr) {
        for (int i = 0; i < this.mElementList.size(); i++) {
            if (this.mElementList.get(i).getAddress() == addr) {
                return this.mElementList.get(i);
            }
        }
        return null;
    }

    public void setCurrNetkeyIndex(int index) {
        this.mCurrNetKeyIndex = index;
    }

    public int getCurrNetkeyIndex() {
        return this.mCurrNetKeyIndex;
    }

    public void setProvAppKeyIndex(int index) {
        this.mProvAppKeyIndex = index;
    }

    public int getProvAppKeyIndex() {
        return this.mProvAppKeyIndex;
    }

    public void setNodeNetKeyList(ArrayList<Integer> mNetKeyList) {
        this.mNetKeyList = mNetKeyList;
    }

    public ArrayList<Integer> getNodeNetKeyList() {
        return this.mNetKeyList;
    }


    public void setNodeAppKeyList(ArrayList<Integer> mAppKeyList) {
        this.mAppKeyList = mAppKeyList;
    }

    public ArrayList<Integer> getNodeAppKeyList() {
        return this.mAppKeyList;
    }

    public void addNetKey(int netKeyIndex) {
        for (int i = 0; i < this.mNetKeyList.size(); i++) {
            if (this.mNetKeyList.get(i) == netKeyIndex) {
                //log("Netkey index " + netKeyIndex + "already exists");
                return;
            }
        }
        this.mNetKeyList.add(netKeyIndex);
    }

    public void addAppKey(int appKeyIndex) {
        for (int i = 0; i < this.mAppKeyList.size(); i++) {
            if (this.mAppKeyList.get(i) == appKeyIndex) {
                //log("Appkey index " + appKeyIndex + "already exists");
                return;
            }
        }
        this.mAppKeyList.add(appKeyIndex);
    }

    public void setGattAddrType(int mGattAddrType) {
        this.mGattAddrType = mGattAddrType;
    }

    public int getGattAddrType() {
        return this.mGattAddrType;
    }

    public void setGattAddr(String mGattAddr) {
        this.mGattAddr = mGattAddr;
    }

    public String getGattAddr() {
        return this.mGattAddr;
    }

    public void addSupportedRXMsg(int elementAddr, long modelID, Model model)
    {
        switch ((int)modelID) {
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_GENERIC_ONOFF_SERVER: {
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_GENERIC_ONOFF_GET, elementAddr, "Generic OnOff Get("+MeshUtils.decimalToHexString("%04X", elementAddr)+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_GENERIC_ONOFF_SET, elementAddr, "Generic OnOff Set("+MeshUtils.decimalToHexString("%04X", elementAddr)+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_GENERIC_ONOFF_SET_UNRELIABLE, elementAddr, "Generic OnOff Set Unack("+MeshUtils.decimalToHexString("%04X", elementAddr)+")", model));
                break;
            }
            case 0x00460000: {
                mSupportedMsgList.add(new MeshMessage(VendorModelDemoClient.VENDOR_MSG_OPCODE_STATE_GET, elementAddr, "Vendor State Get("+MeshUtils.decimalToHexString("%04X", elementAddr)+")", model));
                mSupportedMsgList.add(new MeshMessage(VendorModelDemoClient.VENDOR_MSG_OPCODE_STATE_SET, elementAddr, "Vendor State Set("+MeshUtils.decimalToHexString("%04X", elementAddr)+")", model));
                mSupportedMsgList.add(new MeshMessage(VendorModelDemoClient.VENDOR_MSG_OPCODE_STATE_SET_UNACK, elementAddr, "Vendor State Set Unack("+MeshUtils.decimalToHexString("%04X", elementAddr)+")", model));
                break;
            }
            /*case MeshConstants.MESH_MODEL_SIG_MODEL_ID_LIGHT_LIGHTNESS_SERVER: {
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_GET, elementAddr, "Light Lightness Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_SET, elementAddr, "Light Lightness Set("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_SET_UNACKNOWLEDGED, elementAddr, "Light Lightness Set Unack("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_LINEAR_GET, elementAddr, "Light Lightness Linear Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_LINEAR_SET, elementAddr, "Light Lightness Linear Set("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_LINEAR_SET_UNACKNOWLEDGED, elementAddr, "Light Lightness Linear Set Unack("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_LAST_GET, elementAddr, "Light Lightness Last Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_GET, elementAddr, "Light Lightness Default Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_RANGE_GET, elementAddr, "Light Lightness Range Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_DEFAULT_SET, elementAddr, "Light Lightness Default Set("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_LIGHTNESS_RANGE_SET, elementAddr, "Light Lightness Range Set("+elementAddr+")", model));
                break;
            }
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_LIGHT_CTL_SERVER: {
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_CTL_GET, elementAddr, "Light CTL Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_CTL_SET, elementAddr, "Light CTL Set("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_CTL_SET_UNACKNOWLEDGED, elementAddr, "Light CTL Set Unack("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_CTL_TEMPERATURE_GET, elementAddr, "Light CTL Temp Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_GET, elementAddr, "Light CTL Temp Range Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_CTL_TEMPERATURE_SET, elementAddr, "Light CTL Temp Set("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_CTL_TEMPERATURE_SET_UNACKNOWLEDGED, elementAddr, "Light CTL Temp Set Unack("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_CTL_DEFAULT_GET, elementAddr, "Light CTL Default Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_CTL_DEFAULT_SET, elementAddr, "Light CTL Default Set("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_CTL_DEFAULT_SET_UNACKNOWLEDGED, elementAddr, "Light CTL Default Set Unack("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_SET, elementAddr, "Light CTL Temp Range Set("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_CTL_TEMPERATURE_RANGE_SET_UNACKNOWLEDGED, elementAddr, "Light CTL Temp Range Set Unack("+elementAddr+")", model));
                break;
            }
            case MeshConstants.MESH_MODEL_SIG_MODEL_ID_LIGHT_HSL_SERVER: {
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_GET, elementAddr, "Light HSL Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_HUE_GET, elementAddr, "Light HSL HUE Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_HUE_SET, elementAddr, "Light HSL HUE Set("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_HUE_SET_UNACKNOWLEDGED, elementAddr, "Light HSL HUE Set Unack("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_SATURATION_GET, elementAddr, "Light HSL Saturation Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_SATURATION_SET, elementAddr, "Light HSL Saturation Set("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_SATURATION_SET_UNACKNOWLEDGED, elementAddr, "Light HSL Saturation Set Unack("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_SET, elementAddr, "Light HSL Set("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_SET_UNACKNOWLEDGED, elementAddr, "Light HSL Set Unack("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_TARGET_GET, elementAddr, "Light HSL Target Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_DEFAULT_GET, elementAddr, "Light HSL Default Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_RANGE_GET, elementAddr, "Light HSL Range Get("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_DEFAULT_SET, elementAddr, "Light HSL Default Set("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_DEFAULT_SET_UNACKNOWLEDGED, elementAddr, "Light HSL Default Set Unack("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_RANGE_SET, elementAddr, "Light HSL Range Set("+elementAddr+")", model));
                mSupportedMsgList.add(new MeshMessage(MeshConstants.MESH_MSG_LIGHT_HSL_RANGE_SET_UNACKNOWLEDGED, elementAddr, "Light HSL Range Set Unack("+elementAddr+")", model));
                break;
            }*/
            default:
                break;
        }
    }

    public void setSupportedRxMsgList(ArrayList<MeshMessage> mSupportedMsgList) {
        this.mSupportedMsgList = mSupportedMsgList;
    }

    public ArrayList<MeshMessage> getSupportedRxMsgList() {
        return this.mSupportedMsgList;
    }
}
