package com.mesh.test.provisioner.sqlite;

import com.mesh.test.provisioner.AppKey;
import com.mesh.test.provisioner.NetKey;
import java.util.ArrayList;



public class StorageData {
    private String id;
    private ArrayList<Node> mNodes = new ArrayList<>();
    private ArrayList<NetKey> mAllNetKey = new ArrayList<>();
    private ArrayList<AppKey> mAllAppKey = new ArrayList<>();
    private ArrayList<Integer> mAllGroupAddr = new ArrayList<>();

    public StorageData() {
    }

    public void setId(String id){
        this.id = id;
    }

    public String getId(){
        return id;
    }

    public void addNetKey(NetKey key) {
        for (int i = 0; i < this.mAllNetKey.size(); i++) {
            if (this.mAllNetKey.get(i).getIndex() == key.getIndex()){
                //netkey is already exist
                return;
            }
        }
        mAllNetKey.add(key);
    }

    public void setAllNetKey(ArrayList<NetKey> mAllNetKey){
        this.mAllNetKey = mAllNetKey;
    }

    public void deleteNetKey(int index) {
        for (int i = 0; i < this.mAllNetKey.size(); i++) {
            if (this.mAllNetKey.get(i).getIndex() == index){
                this.mAllNetKey.remove(i);
                return;
            }
        }
    }

    public ArrayList<NetKey> getAllNetKey() {
        return this.mAllNetKey;
    }

    public void addAppKey(AppKey key) {
        for (int i = 0; i < this.mAllAppKey.size(); i++) {
            if (this.mAllAppKey.get(i).getIndex() == key.getIndex()){
                //appkey is already exist
                return;
            }
        }
        mAllAppKey.add(key);
    }

    public void setAllAppKey(ArrayList<AppKey> mAllAppKey){
        this.mAllAppKey = mAllAppKey;
    }

    public void deleteAppKey(int index) {
        for (int i = 0; i < this.mAllAppKey.size(); i++) {
            if (this.mAllAppKey.get(i).getIndex() == index){
                this.mAllAppKey.remove(i);
                return;
            }
        }
    }

    public ArrayList<AppKey> getAllAppKey() {
        return this.mAllAppKey;
    }

    public void addGroupAddr(int addr) {
        for (int i = 0; i < this.mAllGroupAddr.size(); i++) {
            if (this.mAllGroupAddr.get(i) == addr){
                //GroupAddr is already exist
                return;
            }
        }
        this.mAllGroupAddr.add(addr);
    }

    public void setAllGroupAddrList(ArrayList<Integer> list) {
        this.mAllGroupAddr = list;
    }

    public ArrayList<Integer> getAllGroupAddr() {
        return this.mAllGroupAddr;
    }

    public void deleteGroupAddr(int index) {
        for (int i = 0; i < this.mAllGroupAddr.size(); i++) {
            if (this.mAllGroupAddr.get(i) == index){
                this.mAllGroupAddr.remove(i);
                return;
            }
        }
    }


}

