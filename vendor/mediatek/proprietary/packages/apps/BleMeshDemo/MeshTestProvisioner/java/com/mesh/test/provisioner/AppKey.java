package com.mesh.test.provisioner;



public class AppKey {
    int mKeyIndex;
    int mBoundNetKeyIndex;
    int[] mKey; //The currently in used
    int[] mTempKey;    //new key value set by key update, after key refresh, it stores the old key

    public AppKey() {
    }

    public AppKey(int index) {
        this.mKeyIndex = index;
    }

    public int setAppKeyMeta(int index, int[] key, int netKeyIndex) {
        if ((index > 0xFFF) || (key.length != 16) || (netKeyIndex > 0xFFF)) {
            //log("setAppKeyMeta invalid meta value");
            return -1;
        }
        this.mKeyIndex = index;
        this.mKey = key;
        this.mBoundNetKeyIndex = netKeyIndex;
        return 0;
    }


    public int getIndex() {
        return this.mKeyIndex;
    }

    public int getBoundNetKeyIndex() {
        return this.mBoundNetKeyIndex;
    }

    public int[] getValue() {
        return this.mKey;
    }

    public int[] getTempValue() {
        return this.mTempKey;
    }
}

