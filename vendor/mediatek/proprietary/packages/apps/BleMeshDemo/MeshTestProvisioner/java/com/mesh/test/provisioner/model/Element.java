package com.mesh.test.provisioner.model;

import com.mesh.test.provisioner.sqlite.Node;

import java.util.ArrayList;
import android.util.Log;

public class Element {

    private static final String TAG = "Element";
    private int mAddress;
    private int mIndex;
    private int mNodeAddr; //the node that the element belongs to
    public int mLoc;
    public int mNumS;
    public int mNumV;
    private ArrayList<Model> models = new ArrayList<>();

    public Element() {
    }

    public Element(int address) {
        this.mAddress = address;
    }

    public Element(int address,int index) {
        this.mAddress = address;
        this.mIndex = index;
    }

    public int getAddress() {
        return this.mAddress;
    }

    public void setAddress(int address) {
        this.mAddress = address;
    }

    public int getIndex() {
        return this.mIndex;
    }

    public void setIndex(int index) {
        this.mIndex = index;
    }

    public void setNodeAddr(int addr) {
        this.mNodeAddr = addr;
    }

    public int getNodeAddr() {
        return this.mNodeAddr;
    }

    public void addModel(Model model){
        Log.i(TAG,"addModel id = " + model.getID());
        models.add(model);
    }

    public ArrayList<Model> getModels(){
        return models;
    }

    public Model getModelById(long id) {
        for (int i = 0; i < this.models.size(); i++) {
            if (this.models.get(i).getID() == id) {
                return this.models.get(i);
            }
        }
        return null;
    }
}
