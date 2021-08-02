package com.mesh.test.provisioner;

import com.mesh.test.provisioner.sqlite.Node;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Timer;


public class NetKey {

    private int mKeyIndex;
    private int mState;
    private int mNodeIdentity;
    private int mFlag;
    private int[] mKey; //The currently in used
    private int[] mTempKey;    //new key value set by key update, after key refresh, it stores the old key
    private ArrayList<Integer> mAppKeyList = new ArrayList<>();
    private HashMap<Integer, Node> mNodeMap = new HashMap<>();    //all the nodes which is using this netkey currently
    private HashMap<Integer, Node> mKeyRefreshNodeMap = new HashMap<>();    //the count of node which is not blacklisted to this netkey for key refresh
    private HashMap<Integer, Node> mKeyRefreshAckNodeMap = new HashMap<>();     //the node which reply for Key refresh related messages.
    private int mKeyRefreshTimerID;

    public NetKey() {
    }

    public NetKey(int index) {
        this.mKeyIndex = index;
    }

    public int setNetKeyMeta(int index, int[] key) {
        if ((index > 0xFFF) || (key.length != 16)) {
            //log("setNetKeyMeta invalid meta value");
            return -1;
        }
        this.mKeyIndex = index;
        this.mKey = key;
        return 0;
    }

    public void setKeyRefreshTimerID(int id) {
        this.mKeyRefreshTimerID = id;
    }

    public int getKeyRefreshTimerID() {
        return this.mKeyRefreshTimerID;
    }

    public void setState(int state) {
        this.mState = state;
    }

    public int getState() {
        return this.mState;
    }

    public int getIndex() {
        return this.mKeyIndex;
    }

    public int[] getValue() {
        return this.mKey;
    }

    public int[] getTempValue() {
        return this.mTempKey;
    }

    public int setTempValue(int[] key) {
        if (key.length != 16) {
            //log("setTempValue invalid key value length");
            return -1;
        }
        this.mTempKey = key;
        return 0;
    }

    public ArrayList<Integer> getAppKeyList() {
        return this.mAppKeyList;
    }

    public HashMap<Integer, Node> getNodes() {
        return this.mNodeMap;
    }

    public HashMap<Integer, Node> getKeyRefreshNodes() {
        return this.mKeyRefreshNodeMap;
    }

    public HashMap<Integer, Node> getKeyRefreshAckNodes() {
        return this.mKeyRefreshAckNodeMap;
    }

}

